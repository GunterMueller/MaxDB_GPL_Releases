/*!
  \file    vcn35b.cpp
  \author  TiloH
  \ingroup cn layer
  \brief   implementing class for controling backups from the database
           kernel to an external backup tool
  \see

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
    ========== licence end


\endif
*/


// -----------------------------------------------------------------------------
// includes
// -----------------------------------------------------------------------------

#include <time.h>

#include "hcn20.h"
#include "hcn31.h"
#include "hcn32.h"
#include "hcn35b.h"
#include "hcn36.h"
#include "hcn42.h"
#include "hcn50.h"
#include "hcn51.h"
#include "hcn53.h"
#include "DBM/Srv/Backup/Media/DBMSrvBMedia_MediaGroup.hpp"

#include "DBM/Srv/DBMSrv_Reply.hpp"
#include "DBM/Srv/HSS/DBMSrvHSS_Nodes.hpp"
#include "DBM/Srv/KernelAccess/DBMSrvKnl_GlobalSessions.hpp"


// -----------------------------------------------------------------------------
// member functions of class tcn35b_DBMExternalBackupController
// -----------------------------------------------------------------------------

class tcn35b_DBMExternalBackupControllerConstructorArguments
{
  public:
    tcn35_BackupToolConnector                      * TheBackupToolConnector;
    tcn35e_DBMBackupControllerErrorHandler * TheErrorHandler;
    tcn35d_DBBackupControllerLogWriter             * TheLogWriter;
    tcn35d_BackupInfo                              * TheBackupInfo;
    tcn35e_ConfigurationVariableHandler            * TheConfigurationVariableHandler;
    char                                           * TheReplyBuffer;
    int                                            * TheReplyBufferLength;
    DBMSrvKnl_Session                              * TheSQLSession;
    char                                           * TheKernelCommand;
    long                                             ThePageSize;
};

const char * tcn35b_DBMExternalBackupController::DataTypeToListSaveTypeMap[5]=
{
    BA_EBIDS_TYPE_DAT_REC_CN003C,
    BA_EBIDS_TYPE_DAT_MIG_CN003C,
    BA_EBIDS_TYPE_PAG_REC_CN003C,
    BA_EBIDS_TYPE_PAG_MIG_CN003C,
    BA_EBIDS_TYPE_LOG_REC_CN003C
};

tcn35b_DBMExternalBackupController::tcn35b_DBMExternalBackupController(tcn35b_DBMExternalBackupControllerConstructorArguments & Args)
    :tcn35a_DBBackupController(Args.TheBackupToolConnector, Args.TheErrorHandler, Args.TheLogWriter, Args.TheBackupInfo, Args.TheConfigurationVariableHandler),
    ConfigVarHandler(Args.TheConfigurationVariableHandler),
    ReplyData(Args.TheReplyBuffer),
    ReplyLen(Args.TheReplyBufferLength),
    SQLSession(Args.TheSQLSession),
    KernelCommand(Args.TheKernelCommand),
    SQLCodeOfRequest(0),
    SQLErrForRequest(false),
    FirstLogPageNo(0),
    LastLogPageNo(0),
    ToolErrorText(0),
    DBRequestType(BackupOrRestore),
    InternalBackupID(0),
    StartDateTime(0),
    LogRequiredFlag(1),
    PageSize(Args.ThePageSize),
    ErrorHandlerAsDBMErrorHandler(Args.TheErrorHandler)
{
    //TheDBResult must not been initialized
    SQLErrText.rawAssign("");
}

tcn35b_DBMExternalBackupController::~tcn35b_DBMExternalBackupController()
{
    cn36_StrDealloc(KernelCommand);

    cn36_StrDealloc(FirstLogPageNo);
    cn36_StrDealloc(LastLogPageNo);
    cn36_StrDealloc(ToolErrorText);

    cn36_StrDealloc(InternalBackupID);
    cn36_StrDealloc(StartDateTime);
}

int tcn35b_DBMExternalBackupController::ListEBIDsOfAvailableBackups(DBMSrvBHist_EBIDList & AvailableEBIDs)
{
    int rc=1,
        ToolIsInstalled=0;

    AvailableEBIDs.Free();

    ErrorHandler->SetPhaseTo(tcn35d_DBBackupControllerErrorHandler::ToolCheck);

    if(!BackupToolConnector->CheckToolInstallation(ToolIsInstalled))
        rc=0;

    if(rc && ToolIsInstalled)
    {
        ErrorHandler->SetPhaseTo(tcn35d_DBBackupControllerErrorHandler::PrepareOperation);
        LogWriter->Write("Preparing query for available backups.\n", tcn35d_DBBackupControllerLogWriter::AddALevelToPrefix);
        
        rc=Prepare();

        if(rc)
            rc=BackupToolConnector->Prepare();

        LogWriter->Write(rc? "Prepare passed successfully.\n": "Prepare failed.\n", tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefixAndEndParagraph);

        if(rc)
        {
            ErrorHandler->SetPhaseTo(tcn35d_DBBackupControllerErrorHandler::ToolRequest);
            if(!BackupToolConnector->StartTool())
                rc=0;

            if(rc)
            {
                long lastOutputTime=0;
                long outputIntervall=0;
                bool outputResult;

                ErrorHandler->SetPhaseTo(tcn35d_DBBackupControllerErrorHandler::OperationCheck);
                LogWriter->Write("Waiting for end of the query.\n", tcn35d_DBBackupControllerLogWriter::AddALevelToPrefix);

                while(BackupToolConnector->ToolIsRunning(outputResult=(outputIntervall<=cn36_SecondsBetween(cn36_GetCurrentTime(), lastOutputTime))))
                {
                    if(outputResult)                          //if the backup tool status was told this time...
                    {
                        lastOutputTime=cn36_GetCurrentTime(); //...we have a new time for the last told backup status
                        outputIntervall+=5;                   //...we increase the output intervall to reduce output over time
                    }

                    Sleep(1, false);
                }

                ErrorHandler->SetPhaseTo(tcn35d_DBBackupControllerErrorHandler::BackupOperation);

                if(!BackupToolConnector->ToolWasSuccessful(DBBackupSize))
                    ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseBackupToolFailed, BackupToolConnector->SumOfToolExitCodes()));  //report the problem but do not set rc=0 (same as with IsInstalled)

                sprintf(MsgBuf_cn36,
                        "%s has finished the query %s.\n",
                        BackupToolConnector->GiveToolName(),
                        BackupToolConnector->ToolWasSuccessful(DBBackupSize)?"successfully":"unsuccessfully");
                LogWriter->Write(MsgBuf_cn36, tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefixAndEndParagraph);

                ErrorHandler->SetPhaseTo(tcn35d_DBBackupControllerErrorHandler::PostOperation);

                if(BackupToolConnector->ToolWasSuccessful(DBBackupSize)) //proceed only if the tool was successful
                {
                    rc=BackupToolConnector->PostOperationsAfterSuccess();

                    if(rc)  // generate list for output
                    {
                        rc=BackupToolConnector->PrepareToolOutputOfAvailableBackups();

                        if(rc)
                        {
                            int          EBIDAvailable;
                            const char * EBID=0;
                            size_t       EBIDLength=0;

                            tcn35d_BackupInfo::ADataType DataType;
                            const char * DateTime=0;
                            int          LineContainsEBID=0;
                            int          ReachedEnd=0;

                            sprintf(MsgBuf_cn36, "Analyzing %s's answer.\n", BackupToolConnector->GiveToolName());
                            LogWriter->Write(MsgBuf_cn36, tcn35d_DBBackupControllerLogWriter::AddALevelToPrefix);

                            do
                            {
                                rc=BackupToolConnector->GiveNextAvailableBackup(EBIDAvailable, EBID, EBIDLength, DataType, DateTime, ReachedEnd);

                                if(rc && !ReachedEnd)
                                {
                                    if(0==EBID || 0==EBIDLength || 0==DateTime)
                                    {
                                        rc=0;
                                        ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
                                    }
                                    else
                                    {
                                        if(OK_CN00!=AvailableEBIDs.AddToEnd(ReplyData,
                                                                            ReplyLen,
                                                                            EBIDAvailable?(DBMSrvBHist_Part::EBIDStatusAvailable):(DBMSrvBHist_Part::EBIDStatusUnavailable),
                                                                            "",                          //we dont have a key in this situation
                                                                            0,
                                                                            "",                          //and no label too
                                                                            0,
                                                                            EBID,
                                                                            EBIDLength,
                                                                            DataTypeToListSaveTypeMap[DataType],
                                                                            DateTime))
                                        {
                                            ErrorHandler->Report(tcn35d_DBBackupError::OutOfMemory.Set());
                                        }
                                    }
                                }
                            }
                            while(rc && !ReachedEnd);

                            sprintf(MsgBuf_cn36, "Finished analyzing %s's answer.\n", BackupToolConnector->GiveToolName());
                            LogWriter->Write(MsgBuf_cn36, tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefixAndEndParagraph);
                        }

                        BackupToolConnector->CloseToolOutputOfAvailableBackups();
                    }
                }
            }
        }

        CleanUp();  // cleanup used resources
    }

    ErrorHandler->SetPhaseTo(tcn35d_DBBackupControllerErrorHandler::CommonError);
    LogWriter->Write("\n");

    return rc;
}

void tcn35b_DBMExternalBackupController::ChangeReplyBuffer(char * TheReplyBuffer, 
                                                           int  * TheReplyBufferLength)
{
    ReplyData=TheReplyBuffer;
    ReplyLen=TheReplyBufferLength;

    ErrorHandlerAsDBMErrorHandler->SetReplyArrayTo(TheReplyBuffer, TheReplyBufferLength);
}

tcn00_Error tcn35b_DBMExternalBackupController::GiveLastError()
{
    return ((tcn35e_DBMBackupControllerErrorHandler*)ErrorHandler)->GiveLastError();
}

int tcn35b_DBMExternalBackupController::GiveSQLResult()
{
    int rc=ERR_CN00;    //if we have no real answer from the db kernel, we answer with an error at least

    switch(RequestState())
    {
        case NoRequestSend:         // return SQLCodeOfRequest to comply with part for normal media (utility session can only be used for receiv the reply of the db)
        case RequestSend:           // return SQLCodeOfRequest to comply with part for normal media (utility session can only be used for receiv the reply of the db)
        case RequestReplied:        // return SQLCodeOfRequest to comply with part for normal media (utility session can be used for new backup commands)
        case CanceledByDBMSrv:      // have send a cancel to the utility task, utility task has ended and reply was read (SQLCodeOfRequest should contain an error)
            rc=SQLCodeOfRequest;
        break;

        case CancelSendByDBMSrv:    // have send a cancel to the utility task, no answer yet
        case TerminatedByDBMSrv:    // have no further idea to stop db-kernel
        break;                      // we can not give any reply of the db kernel

        default:
            ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
    }

    return rc;
}

int tcn35b_DBMExternalBackupController::IsBad()
{
    return ErrorHandler->AnErrorOccured();
}

int tcn35b_DBMExternalBackupController::Prepare()
{
    int rc=1;

    const ni31_ListElem<tcn35e_ConfigVariable *> *ListElem;

    for(ListElem=ConfigVarHandler->ConfigurationVariableToExportList().First(); rc && 0!=ListElem; ListElem=ListElem->Next())
    {
        const char * VariableName=ListElem->GiveInfo()->GiveName();
        const char * VariableValue=ListElem->GiveInfo()->GiveValue();

        const char * EnvVar=cn36_GetEnv(VariableName);
        
        if(0==EnvVar ||
           0!=strcmp(VariableValue, EnvVar))
        {
            sprintf(MsgBuf_cn36, "Exporting configuration variable '%s' with value '%s'.\n", VariableName, VariableValue);
            LogWriter->Write(MsgBuf_cn36);

            rc=cn36_PutEnv(VariableName, VariableValue);
        }
        else
        {
            sprintf(MsgBuf_cn36, "The environment variable '%s' has already the value '%s'.\n", VariableName, VariableValue);
            LogWriter->Write(MsgBuf_cn36);
        }
    }

    return rc;
}

int tcn35b_DBMExternalBackupController::StartDBAction()
{
    int rc=1;

    LogWriter->Write("Starting database action for", tcn35d_DBBackupControllerLogWriter::AddALevelToPrefix);
    LogWriter->Write(tcn35d_BackupInfo::ABackup==BackupInfo->DataDirection()?" the backup.\n":" the restore.\n", tcn35d_DBBackupControllerLogWriter::NoPrefix);

    rc=SendBackupRestoreRequestToKernel(*SQLSession, KernelCommand, SQLCodeOfRequest);

    if(!rc)
        LogWriter->Write("Could not start database action.\n", tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefixAndEndParagraph);
    else
        LogWriter->Write("The database is working on the request.\n", tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefixAndEndParagraph);

    return rc;
}

int  tcn35b_DBMExternalBackupController::DBIsReady()
{
    int IsReady=0;

    if(tcn35d_BackupInfo::ABackup==BackupInfo->DataDirection() ||
       tcn35d_BackupInfo::ARestore==BackupInfo->DataDirection()  )
    {
        sprintf(MsgBuf_cn36, "Waiting until database has prepared the %s.\n", BackupInfo->getDataDirectionAsString());
        LogWriter->Write(MsgBuf_cn36, tcn35d_DBBackupControllerLogWriter::AddALevelToPrefix);

        tcn003_BackupResult StatusInformation;
        int ReplyFromDBKernel=DBReplyAvailable(false),
            SleepTime=1;

        while(!ReplyFromDBKernel && !IsReady) // db-kernel should send reply in case of error or set szPagesLeft to a value
        {
            if(GetStatusOfUtilityTask(StatusInformation))
            {
                if(StatusInformation.szPagesLeft[0]!='\0')
                {
                    IsReady=1;
                    sprintf(MsgBuf_cn36, "Database has finished preparation of the %s.\n", BackupInfo->getDataDirectionAsString());
                }
                else
                {
                    sprintf(MsgBuf_cn36, "Database is still preparing the %s.\n", BackupInfo->getDataDirectionAsString());
                }

                LogWriter->Write(MsgBuf_cn36, tcn35d_DBBackupControllerLogWriter::UsePrefixAndDateTime);
            }

            ReplyFromDBKernel=DBReplyAvailable(false);

            if(!ReplyFromDBKernel && !IsReady)
                Sleep(SleepTime++);
        }

        if(ReplyFromDBKernel) // Reply available means, that db-kernel has finished the action
        {
            IsReady=0;
            ReceiveBackupRestoreRequestReplyFromDB();
        }

        sprintf(MsgBuf_cn36, "The database has prepared the %s successfully.\n", BackupInfo->getDataDirectionAsString());

        LogWriter->Write(ReplyFromDBKernel?"Database has already finished the request.\n":MsgBuf_cn36, tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefixAndEndParagraph);
    }
    else
        IsReady=1;

    return IsReady;
}

int  tcn35b_DBMExternalBackupController::DBReplyAvailable(bool OutputMessagesForNonEvents)
{
    int rc=1;

    if(SQLSession->ReplyAvailable())
        LogWriter->Write("The database has finished work on the request.\n", tcn35d_DBBackupControllerLogWriter::UsePrefixAndDateTime);
    else
    {
        if(OutputMessagesForNonEvents)
            LogWriter->Write("The database is working on the request.\n", tcn35d_DBBackupControllerLogWriter::UsePrefixAndDateTime);

        rc=0;
    }

    return rc;
}

void tcn35b_DBMExternalBackupController::ReceiveBackupRestoreRequestReplyFromDB()
{
    bool IsNoSQLError=true;
    tcn003_BackupResult TheDeleteLogResult;
    DBMSrvMsg_Error     KernelSessionError;

    LogWriter->Write("Receiving a reply from the database kernel.\n");

    if(BackupOrRestore==DBRequestType)
    {
        int SendBackupRestoreIgnore=0;

        do
        {
            SendBackupRestoreIgnore=0;

            KernelSessionError.ClearMessageList();
            ReceiveReplyFromDBKernel(*SQLSession, SQLCodeOfRequest, TheDBResult, IsNoSQLError, true, KernelSessionError);

            if(0==SQLCodeOfRequest)
            {
                cn36_StrAllocCpy(FirstLogPageNo, TheDBResult.szFirstLogPageNo);    // we check for error (0==FirstLogPageNo) before usage
                cn36_StrAllocCpy(LastLogPageNo, TheDBResult.szLastLogPageNo);      // we check for error (0==LastLogPageNo) before usage

                if('\0'!=TheDBResult.szPagesTransfered[0]) // the Pages Transfered is not empty
                {
                    if(!cn36_StrConsistsOfDigits(TheDBResult.szPagesTransfered) ||
                       !cn36_StrToUInt8(DBBackupSize, TheDBResult.szPagesTransfered))
                    {
                        ErrorHandler->Report(tcn35e_DBMBackupError(tcn35e_DBMBackupError::CaseIncorrectPagesTransfered, TheDBResult.szPagesTransfered));
                        //ErrorHandler->AnErrorOccured() is checked later, so no rc=0 is necessary (also not usefull)
                    }
                    else
                    {
                        DBBackupSize*=PageSize;
                        DBBackupSize+=BackupInfo->NumberOfPipes()*BackupInfo->BlockSize();  //for every pipe the LZU adds a leading block
                    }
                }
                else
                {
                    ErrorHandler->Report(tcn35e_DBMBackupError(tcn35e_DBMBackupError::CaseNoPagesTransfered));
                    //ErrorHandler->AnErrorOccured() is checked later, so no rc=0 is necessary (also not usefull)
                }
            }
            else
                if(BA_NEXTMEDIUM_RC_CN003==SQLCodeOfRequest &&                                                                      //-8020,
                   1<BackupInfo->NumberOfPipes() && BackupInfo->DataDirection()==tcn35d_BackupInfo::ARestore)                       //except non-parallel restores
                {                                                                                                                   //are answered by
                    BackupRestoreIgnore();                                                                                          //an "... IGNORE" command.
                    SendBackupRestoreIgnore=1;                                                                                      //remember that we requested an ignore
                }
                else
                {
                    cn36_StrDealloc(FirstLogPageNo);
                    cn36_StrDealloc(LastLogPageNo);
                }
        }
        while(SendBackupRestoreIgnore);                         //repeat as long as we requested an ignore
    }
    else
        ReceiveReplyFromDBKernel(*SQLSession, SQLCodeOfRequest, TheDeleteLogResult, IsNoSQLError, true, KernelSessionError); //using TheDeleteLogResult, to avoid overwriting of tTheDBResult if all is done correctly

    SQLErrForRequest=!IsNoSQLError;

    if(0!=SQLCodeOfRequest && SQLErrForRequest) // we know it's an SQL error
        SQLErrText.rawAssign(KernelSessionError.getTagValueAsString(SDBMSGTAG_DBMSRV_SQL__ERRORTEXT));

    if(0!=SQLCodeOfRequest)
        if(DeleteLog==DBRequestType)
            TheDBResult=TheDeleteLogResult;    //if we get an error, overwrite the tTheDBResult of the successful backup with the TheDeleteLogResult of the failed "DELETE LOG ..."

}

void tcn35b_DBMExternalBackupController::BackupRestoreIgnore()
{
    int  AllOK=1;
    char IgnoreCommand[BA_COMMAND_LENGTH_MXCN003];

    // build command
    if(tcn35d_BackupInfo::ABackup==BackupInfo->DataDirection())                                 //if it's a backup ...
        cn31_BuildSaveIgnoreCommand(BackupInfo->BackupTypeAsString(), IgnoreCommand);           //... "SAVE <BackupType> IGNORE"
    else
        if(tcn35d_BackupInfo::ARestore==BackupInfo->DataDirection())                            //if it's a restore ...
            cn31_BuildRestoreIgnoreCommand(BackupInfo->BackupTypeAsString(), IgnoreCommand);    //... "RESTORE <BackupType> IGNORE"

    // execute command
    AllOK=SendBackupRestoreRequestToKernel(*SQLSession, IgnoreCommand, SQLCodeOfRequest);
}

void tcn35b_DBMExternalBackupController::ReceiveReplyFromDBKernel(DBMSrvKnl_Session   & Session,
                                                                  int                 & SQLCode,
                                                                  tcn003_BackupResult & tResult,
                                                                  bool                & IsNoSQLError,
                                                                  bool                  verbose,
                                                                  DBMSrvMsg_Error     & KernelSessionError)
{
    SQLCode = cn31BackupResultReceive(Session, IsNoSQLError, tResult, KernelSessionError);

    if(verbose)
    {
        LogWriter->Write("Got the following reply from db-kernel:\n", tcn35d_DBBackupControllerLogWriter::AddALevelToPrefix);

        sprintf(MsgBuf_cn36, "SQL-Code              :%d\n", int(SQLCode));
        LogWriter->Write(MsgBuf_cn36);

        if('\0'!=tResult.szDate[0])
        {
            sprintf(MsgBuf_cn36, "Date                  :%s\n", tResult.szDate);
            LogWriter->Write(MsgBuf_cn36);
        }

        if('\0'!=tResult.szTime[0])
        {
            sprintf(MsgBuf_cn36, "Time                  :%s\n", tResult.szTime);
            LogWriter->Write(MsgBuf_cn36);
        }

        if('\0'!=tResult.szDatabase[0])
        {
            sprintf(MsgBuf_cn36, "Database              :%s\n", tResult.szDatabase);
            LogWriter->Write(MsgBuf_cn36);
        }

        if('\0'!=tResult.szServer[0])
        {
            sprintf(MsgBuf_cn36, "Server                :%s\n", tResult.szServer);
            LogWriter->Write(MsgBuf_cn36);
        }

        if('\0'!=tResult.szKernelVersion[0])
        {
            sprintf(MsgBuf_cn36, "KernelVersion         :%s\n", tResult.szKernelVersion);
            LogWriter->Write(MsgBuf_cn36);
        }

        if('\0'!=tResult.szPagesTransfered[0])
        {
            sprintf(MsgBuf_cn36, "PagesTransfered       :%s\n", tResult.szPagesTransfered);
            LogWriter->Write(MsgBuf_cn36);
        }

        if('\0'!=tResult.szPagesLeft[0])
        {
            sprintf(MsgBuf_cn36, "PagesLeft             :%s\n", tResult.szPagesLeft);
            LogWriter->Write(MsgBuf_cn36);
        }

        if('\0'!=tResult.szVolumes[0])
        {
            sprintf(MsgBuf_cn36, "Volumes               :%s\n", tResult.szVolumes);
            LogWriter->Write(MsgBuf_cn36);
        }

        if('\0'!=tResult.szMediaName[0])
        {
            sprintf(MsgBuf_cn36, "MediaName             :%s\n", tResult.szMediaName);
            LogWriter->Write(MsgBuf_cn36);
        }

        if('\0'!=tResult.szLocation[0])
        {
            sprintf(MsgBuf_cn36, "Location              :%s\n", tResult.szLocation);
            LogWriter->Write(MsgBuf_cn36);
        }

        if('\0'!=tResult.szErrortext[0])
        {
            sprintf(MsgBuf_cn36, "Errortext             :%s\n", tResult.szErrortext);
            LogWriter->Write(MsgBuf_cn36);
        }

        if('\0'!=tResult.szLabel[0])
        {
            sprintf(MsgBuf_cn36, "Label                 :%s\n", tResult.szLabel);
            LogWriter->Write(MsgBuf_cn36);
        }

        if('\0'!=tResult.szIsConsistent[0])
        {
            sprintf(MsgBuf_cn36, "IsConsistent          :%s\n", tResult.szIsConsistent);
            LogWriter->Write(MsgBuf_cn36);
        }

        if('\0'!=tResult.szFirstLogPageNo[0])
        {
            sprintf(MsgBuf_cn36, "FirstLogPageNo        :%s\n", tResult.szFirstLogPageNo);
            LogWriter->Write(MsgBuf_cn36);
        }

        if('\0'!=tResult.szLastLogPageNo[0])
        {
            sprintf(MsgBuf_cn36, "LastLogPageNo         :%s\n", tResult.szLastLogPageNo);
            LogWriter->Write(MsgBuf_cn36);
        }

        if('\0'!=tResult.szDBStamp1Date[0])
        {
            sprintf(MsgBuf_cn36, "DBStamp1Date          :%s\n", tResult.szDBStamp1Date);
            LogWriter->Write(MsgBuf_cn36);
        }

        if('\0'!=tResult.szDBStamp1Time[0])
        {
            sprintf(MsgBuf_cn36, "DBStamp1Time          :%s\n", tResult.szDBStamp1Time);
            LogWriter->Write(MsgBuf_cn36);
        }

        if('\0'!=tResult.szDBStamp2Date[0])
        {
            sprintf(MsgBuf_cn36, "DBStamp2Date          :%s\n", tResult.szDBStamp2Date);
            LogWriter->Write(MsgBuf_cn36);
        }

        if('\0'!=tResult.szDBStamp2Time[0])
        {
            sprintf(MsgBuf_cn36, "DBStamp2Date          :%s\n", tResult.szDBStamp2Time);
            LogWriter->Write(MsgBuf_cn36);
        }

        if('\0'!=tResult.szBDPageCount[0])
        {
            sprintf(MsgBuf_cn36, "BDPageCount           :%s\n", tResult.szBDPageCount);
            LogWriter->Write(MsgBuf_cn36);
        }

        if('\0'!=tResult.szDevicesUsed[0])
        {
            sprintf(MsgBuf_cn36, "DevicesUsed           :%s\n", tResult.szDevicesUsed);
            LogWriter->Write(MsgBuf_cn36);
        }

        if('\0'!=tResult.szDatabaseID[0])
        {
            sprintf(MsgBuf_cn36, "DatabaseID            :%s\n", tResult.szDatabaseID);
            LogWriter->Write(MsgBuf_cn36);
        }

        if('\0'!=tResult.szMaxUsedDataPage[0])
        {
            sprintf(MsgBuf_cn36, "Max Used Data Page    :%s\n", tResult.szMaxUsedDataPage);
            LogWriter->Write(MsgBuf_cn36);
        }

        if('\0'!=tResult.szConvPageCnt[0])
        {
            sprintf(MsgBuf_cn36, "Converter Page Count  :%s\n", tResult.szConvPageCnt);
            LogWriter->Write(MsgBuf_cn36);
        }

        if('\0'!=tResult.szRedoTransRead[0])
        {
            sprintf(MsgBuf_cn36, "Redo Transactions Read:%s\n", tResult.szRedoTransRead);
            LogWriter->Write(MsgBuf_cn36);
        }

        if('\0'!=tResult.szRedoTransDone[0])
        {
            sprintf(MsgBuf_cn36, "Redo Transactions Done:%s\n", tResult.szRedoTransDone);
            LogWriter->Write(MsgBuf_cn36);
        }

        LogWriter->Write(tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefixAndEndParagraph);
    }
}

int tcn35b_DBMExternalBackupController::GetStatusOfUtilityTask(tcn003_BackupResult &StatusInformation)
{
    DBMSrvKnl_Session NormalSQLSession;
    tcn00_Error       rcDBM=OK_CN00;
    int               SQLCode=0;
    int               GotReplyFromKernel=0;
    DBMSrvMsg_Error   KernelSessionError;

    tcn003_BackupSession    * pBackupSession=(tcn003_BackupSession *) cn00DBMServerData::vcontrol()->pBackupSession;
    const DBMSrvKnl_Session * serviceSession=DBMSrvKnl_GlobalSessions::GetSession(DBMSrvKnl_GlobalSessions::SessionService);

    // connect temporary
    if(0!=pBackupSession &&
       pBackupSession->bCheckOnly &&
       0!=serviceSession)
    {
        if(!serviceSession->duplicateSessionAndConnect(NormalSQLSession, KernelSessionError))
            rcDBM=KernelSessionError.ID();
    }
    else
    {
        cn50DBMUser usrDBM( BackupInfo->DBName(), cn50DBMUser::getUser(BackupInfo->DBName(), FIRSTDBM_CN50));

        if (!NormalSQLSession.NormalConnect(BackupInfo->DBName(),
                                usrDBM.getUserName().asCharp(),
                                usrDBM.getClearMasterPwd().asCharp(),
                                KernelSessionError)) {
            rcDBM=KernelSessionError.ID();
        }
    }

    // ask for state
    if(OK_CN00==rcDBM)
    {
        LogWriter->Write("Asking for state of database.\n");

        const char * Command=0;

        switch(BackupInfo->DataDirection())
        {
            case tcn35d_BackupInfo::ABackup:
                Command="STATE SAVE";
            break;

            case tcn35d_BackupInfo::ARestore:
                Command="STATE RESTORE";
            break;

            default:
                ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError)); //0==Command already
        }

        if(0!=Command)
        {
            SendBackupRestoreRequestToKernel(NormalSQLSession, Command, SQLCode, 1, 0);    //quiet and not for backup

            // print reply
            if(0==SQLCode)
            {
                bool IsNoSQLError;
                ReceiveReplyFromDBKernel(NormalSQLSession, SQLCode, StatusInformation, IsNoSQLError, false, KernelSessionError);

                if(0==SQLCode)                // ignore IsNoSQLError
                    GotReplyFromKernel=1;
                else
                    ErrorHandler->reportMessageList(KernelSessionError);
            }
        }
    }

    NormalSQLSession.Release(KernelSessionError);

    return GotReplyFromKernel;
}

const char * tcn35b_DBMExternalBackupController::FindNextSectionInLine(const char * Line, int Number)
{
    while(0!=Line && Number>0)
    {
        Line=strchr(Line, '|');

        if(0!=Line)
            Line++;

        Number--;
    }

    return Line; // returns 0 if the next Section can not be found
}


const char * tcn35b_DBMExternalBackupController::GetBackupIDFromDBMKnl(const char *  AbsoluteNameOfBackupHistory,
                                                                       const char *  Label,
                                                                       const char *  szDBStamp1Date,
                                                                       const char *  szDBStamp1Time,
                                                                       const char *& DBStartDateTime)
{
    int Found=0;
    static char BackupID[48]; // TODO: why 48????

    LogWriter->Write(tcn35d_DBBackupControllerLogWriter::AddALevelToPrefix);

    tni34_ATextFile DBMKnl(AbsoluteNameOfBackupHistory, tni34_AFile::FromDisk, 0);

    if(DBMKnl.IsGood())
    {
        char DateTimeInDBMKnlFormat[20]="yyyy-mm-dd hh:mm:ss";

        char *CurrPos=DateTimeInDBMKnlFormat;

        cn36_StrNCpy(CurrPos, szDBStamp1Date, 4);       //copy year
        CurrPos+=strlen(CurrPos);

        *(CurrPos++)='-';

        cn36_StrNCpy(CurrPos, szDBStamp1Date+4, 2);     //copy month
        CurrPos+=strlen(CurrPos);

        *(CurrPos++)='-';

        cn36_StrNCpy(CurrPos, szDBStamp1Date+6, 2);     //copy day
        CurrPos+=strlen(CurrPos);

        *(CurrPos++)=' ';

        cn36_StrNCpy(CurrPos, szDBStamp1Time+2, 2);     //copy hour, without first two 00
        CurrPos+=strlen(CurrPos);

        *(CurrPos++)=':';

        cn36_StrNCpy(CurrPos, szDBStamp1Time+4, 2);     //copy minute
        CurrPos+=strlen(CurrPos);

        *(CurrPos++)=':';

        cn36_StrNCpy(CurrPos, szDBStamp1Time+6, 2);     //copy second

        sprintf(MsgBuf_cn36, "Looking for %s with DB Stamp1 %s.\n", Label, DateTimeInDBMKnlFormat);
        LogWriter->Write(MsgBuf_cn36);

        int HistoryFormatOk=1;

        const char * Line;

        while(HistoryFormatOk && DBMKnl.ReadLine(Line) && Line!=0)
        {
            if(Line[0]!='\0')
            {
                const char * LabelInLine       =FindNextSectionInLine(Line);
                const char * TypeInLine        =FindNextSectionInLine(LabelInLine);
                const char * DateTimeInLine    =FindNextSectionInLine(TypeInLine);
                const char * StartDateInLine   =FindNextSectionInLine(DateTimeInLine, 2);
                const char * EndStartDateInLine=(0!=StartDateInLine)?strchr(StartDateInLine, '|'):0;
                const char * LogRequired       =FindNextSectionInLine(StartDateInLine, 4);
                const char * EndLogRequired    =(0!=LogRequired)?strchr(LogRequired, '|'):0;

                if(0!=LabelInLine &&
                   0!=TypeInLine &&
                   0!=DateTimeInLine &&
                   0!=StartDateInLine &&
                   0!=EndStartDateInLine &&
                   0!=LogRequired &&
                   0!=EndLogRequired)
                {
                    int LabelMatches   = (!strncmp(LabelInLine,Label, strlen(Label)) && '|'== *(LabelInLine+strlen(Label))); // are labels equal?
                    int TypeMatches    = ((!strncmp(TypeInLine, "SAVE WARM", strlen("SAVE WARM")) && '|'== *(TypeInLine+strlen("SAVE WARM")))
                                          || (!strncmp(TypeInLine, "SAVE COLD", strlen("SAVE COLD")) && '|'== *(TypeInLine+strlen("SAVE COLD")))
                                         ); //save type must be "SAVE COLD" or "SAVE WARM"
                    int DateTimeMatches= (!strncmp(DateTimeInLine,DateTimeInDBMKnlFormat,strlen(DateTimeInDBMKnlFormat)) && '|'== *(DateTimeInLine+strlen(DateTimeInDBMKnlFormat))); //are date,time equal?

                    if(LabelMatches && DateTimeMatches && TypeMatches)
                    {
                        Found++;

                        cn36_StrNAllocCpy(InternalBackupID, Line, strchr(strchr(Line, '|')+1, '|')-Line);   // there exist at least 3 '|' in Line
                        cn36_StrNAllocCpy(StartDateTime, StartDateInLine, EndStartDateInLine-StartDateInLine);

                        if(0==cn36_StrNUprCmp(LogRequired, "NO", strlen("NO")) &&                           //LogRequired starts with NO
                           (EndLogRequired==LogRequired+strlen("NO") ||                                     // and that is all or
                            (LogRequired+strlen("NO")==cn36_FirstWhiteSpaceOf(LogRequired+strlen("NO")) &&  //is followed by a whitespace
                             EndLogRequired==cn36_FirstNonWhiteSpaceOf(LogRequired+strlen("NO")))))         //and nothing more than whitespaces
                        {
                            LogRequiredFlag=0;
                        }
                        else
                            LogRequiredFlag=1;
                    }
                }
                else
                {
                    HistoryFormatOk=0;
                    ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseUnknownFormatOfDBMKnlFile, AbsoluteNameOfBackupHistory));
                }
            }
        }

        if(0!=InternalBackupID && 0<Found && 0!=StartDateTime)
            sprintf(MsgBuf_cn36, "Found %d Internal Backup ID%s %s.\n", Found, Found>1?"'s. Last was":". It was", InternalBackupID);
        else
            sprintf(MsgBuf_cn36, "Could not determine the Internal Backup ID.\n");

        LogWriter->Write(MsgBuf_cn36);
    }

    LogWriter->Write(tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefix/*AndEndParagraph*/);

    if(1==Found)
    {
        DBStartDateTime=StartDateTime;
        return InternalBackupID;
    }
    else
        return 0;
}

void tcn35b_DBMExternalBackupController::FreeLogAfterLogBackup()
{
    if(DBActionWasSuccessful()) //something like: if(RequestReplied==StatusOfRequest && 0==SQLCodeOfRequest)
    {
        if(0!=FirstLogPageNo && 0<strlen(FirstLogPageNo) &&     //FirstLogPageNo is not empty
           0!=LastLogPageNo && 0<strlen(LastLogPageNo))         //LastLogPageNo is not empty
        {
            int AllOk=0;

            char * Command=0;
            const char * FreeCommandPart1="DELETE LOG FROM ";
            const char * FreeCommandPart2=" TO ";

            sprintf(MsgBuf_cn36, "Freeing log pages from %s to %s for overwriting.\n", FirstLogPageNo, LastLogPageNo);
            LogWriter->Write(MsgBuf_cn36, tcn35d_DBBackupControllerLogWriter::AddALevelToPrefix);

            if(cn36_StrAlloc(Command,
                             strlen(FreeCommandPart1)
                             +strlen(FirstLogPageNo)
                             +strlen(FreeCommandPart2)
                             +strlen(LastLogPageNo)))
            {
                sprintf(Command, "%s%s%s%s", FreeCommandPart1, FirstLogPageNo, FreeCommandPart2, LastLogPageNo);

                if(SendBackupRestoreRequestToKernel(*SQLSession, Command, SQLCodeOfRequest))
                {
                    LogWriter->Write("The database is working on the request.\n");

                    DBRequestType=DeleteLog;                    //change to DeleteLog to activate special handling in ReceiveBackupRestoreRequestReplyFromDB()
                    ReceiveBackupRestoreRequestReplyFromDB();
                    DBRequestType=BackupOrRestore;              //change it back

                    if(0==SQLCodeOfRequest)
                        AllOk=1;
                }
                else
                    LogWriter->Write("Could not request freeing of log for overwriting.\n"/*, SubALevelFromPrefix*/);

                cn36_StrDealloc(Command);
            }

            sprintf(MsgBuf_cn36, AllOk?"Log pages freed successfully.\n":"Could not free logpages.\n");
            LogWriter->Write(MsgBuf_cn36, tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefixAndEndParagraph);
        }
        else
            ErrorHandler->Report(tcn35e_DBMBackupError(tcn35e_DBMBackupError::CaseNoLogPageInformation));
    }
    else
        LogWriter->Write("Not freeing log pages for overwriting, because the backup was unsuccessful.\n", tcn35d_DBBackupControllerLogWriter::EndParagraph);
}

int tcn35b_DBMExternalBackupController::DBMayBeUsingLogDevspaceForLogRestore()
{
    int rc=0;

    if(tcn35d_BackupInfo::ARestore==BackupInfo->DataDirection() &&              //a restore...
       tcn35d_BackupInfo::Log==BackupInfo->DataType())                          //... of log is still running (the is still running part must be assured by the caller of this function)
    {
        tcn003_BackupResult StatusInformation;

        if(GetStatusOfUtilityTask(StatusInformation) &&                                             //could get make a "STATE RESTORE"
           0!=StatusInformation.szPagesTransfered && '\0'!=StatusInformation.szPagesTransfered[0])  //szPagesTransfered is not empty
        {
            teo00_Uint8 PagesTransfered;

            if(cn36_StrToUInt8(PagesTransfered, StatusInformation.szPagesTransfered) && //StatusInformation.szPagesTransfered could be converted to a teo00_Uint8
               0<PagesTransfered)                                                       //the database kernel has read already read (at least) one page of the pipe
            {
                rc=1;
            }
        }
    }

    return rc;
}

int tcn35b_DBMExternalBackupController::DBActionWasSuccessful()
{
    int rc=0;

    if(RequestReplied==RequestState())
    {
        if(tcn35d_BackupInfo::ARestore==BackupInfo->DataDirection() &&
           tcn35d_BackupInfo::Log==BackupInfo->DataType())
        {
            rc=(0==SQLCodeOfRequest || -8020==SQLCodeOfRequest);
        }
        else
            rc=(0==SQLCodeOfRequest);
    }

    return rc;
}

void tcn35b_DBMExternalBackupController::SignalEndToDB(int elapsedTimeout)
{
    if(0>=elapsedTimeout)
        sprintf(MsgBuf_cn36, "Canceling Utility-task ... ");
    else
        sprintf(MsgBuf_cn36, "Canceling Utility-task after a timeout of %d second%s elapsed ... ", (int)elapsedTimeout, 1==elapsedTimeout?"":"s");

    LogWriter->Write(MsgBuf_cn36, tcn35d_DBBackupControllerLogWriter::UsePrefixAndDateTime);
    SQLSession->Cancel();
    LogWriter->Write("OK.\n", tcn35d_DBBackupControllerLogWriter::NoPrefix);
}

void tcn35b_DBMExternalBackupController::TerminateDBRequest()
{
    //do nothing
    //if the cancel on the utility task did not help, a kill is useless
}

void tcn35b_DBMExternalBackupController::WriteHistoryFile()
{
    if(DBActionWasSuccessful())
    {
        int AllIsOK=0;

        LogWriter->Write("Updating external backup history.\n", tcn35d_DBBackupControllerLogWriter::AddALevelToPrefix);

        if(0!=BackupInfo->PathOfBackupHistory())
        {
            sprintf(MsgBuf_cn36, "Using '%s' as backup history.\n", BackupInfo->PathOfBackupHistory());
            LogWriter->Write(MsgBuf_cn36);

            const char * DBStartDateTime="0000-00-00 00:00:00";
            const char * DBBackupID=GetBackupIDFromDBMKnl(BackupInfo->PathOfBackupHistory(),
                                                          TheDBResult.szLabel,
                                                          TheDBResult.szDBStamp1Date,
                                                          TheDBResult.szDBStamp1Time,
                                                          DBStartDateTime);

            if(DBBackupID==NULL)
                ErrorHandler->Report(tcn35e_DBMBackupError(tcn35e_DBMBackupError::CaseCouldNotGetInternalBackupIDs));    //AllIsOK is allready 0
            else
            {
                sprintf(MsgBuf_cn36, "Got '%s' as Internal Backup ID and '%s' as start date.\n", DBBackupID, DBStartDateTime);
                LogWriter->Write(MsgBuf_cn36, tcn35d_DBBackupControllerLogWriter::EndParagraph);

                if(0!=BackupInfo->PathOfExternalBackupHistory())
                {
                    int    NumberOfExternalBackupIDsFound=0;
                    char * lineForHSSCommand=0; //!< used to hold the backup_history_append command for HSS systems
                    bool   isHSS=DBMSrvHSS_Nodes::isHSS();
                    bool   toolWasSuccessful=BackupToolConnector->ToolWasSuccessful(DBBackupSize)?true:false;
                    int    toolError=toolWasSuccessful?0:int(GiveLastError());

                    AllIsOK=1;

                    sprintf(MsgBuf_cn36, "Using '%s' as external backup history.\n", BackupInfo->PathOfExternalBackupHistory());
                    LogWriter->Write(MsgBuf_cn36);

                    if(toolWasSuccessful)
                    {
                        if(!BackupToolConnector->CollectExternalBackupIDs(ExternalBackupID, NumberOfExternalBackupIDsFound) ||
                           NumberOfExternalBackupIDsFound!=BackupInfo->NumberOfPipes())
                        {
                            tcn35e_DBMBackupError error(tcn35e_DBMBackupError::CaseCouldNotGetExternalBackupIDs);
                            ErrorHandler->Report(error);

                            //we had no previous error (toolWasSuccessful==true), so prepare to report this new error in dbm.ebf
                            toolError=GiveLastError();
                            cn36_StrAllocCpy(ToolErrorText, error.GiveErrorMsgBuffer());
                            toolWasSuccessful=false;
                        }
                    }

                    if(!toolWasSuccessful)
                    {
                        for(int i=0; AllIsOK && i<BackupInfo->NumberOfPipes(); i++)
                        {
                            if(!cn36_StrAllocCpy(ExternalBackupID[i], ""))
                                AllIsOK=0;
                        }

                        if(AllIsOK)
                            NumberOfExternalBackupIDsFound=BackupInfo->NumberOfPipes();
                    }

                    if(AllIsOK && isHSS && !cn36_StrAllocCpy(lineForHSSCommand, "backup_history_append"))
                    {
                        ErrorHandler->Report(tcn35d_DBBackupError::OutOfMemory.Set());
                        AllIsOK=0;
                    }

                    if(AllIsOK)
                    {
                        tni34_ATextFile TheLogFile(BackupInfo->PathOfExternalBackupHistory(), tni34_AFile::ToDiskAppend, 1, 1);

                        if(TheLogFile.IsGood())
                        {
                            char NewLogFileEntry[2048];
                            int i;

                            for(i=0; i<NumberOfExternalBackupIDsFound && AllIsOK; i++)
                            {
                                if(0==ExternalBackupID[i])
                                {
                                    AllIsOK=0; //end the loop
                                    ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
                                }
                                else
                                {
                                    tcn35d_BackupInfo::ADataType DataType=BackupInfo->DataType();

                                    if(0==LogRequiredFlag)  //no log is required for the newly created backup
                                    {
                                        if(tcn35d_BackupInfo::DataRecovery==DataType)   //but we requested only a data backup for recovery
                                            DataType=tcn35d_BackupInfo::DataMigration;
                                        else
                                            if(tcn35d_BackupInfo::PagesRecovery==DataType)  //but we requested only a pages backup for recovery
                                                DataType=tcn35d_BackupInfo::PagesMigration;
                                    }

                                    sprintf(NewLogFileEntry,
                                            "%s|%s|%s|%s|%s|%d|%s|%s|vftPipe|NO|%ld|%d|%c|%s|%d|",
                                            DBBackupID,
                                            ExternalBackupID[i],
                                            DataTypeToListSaveTypeMap[DataType],
                                            DBStartDateTime,
                                            BackupToolConnector->ToolEnum().AsString(),
                                            toolError,
                                            toolWasSuccessful?"":(0==ToolErrorText?"":ToolErrorText),
                                            BackupInfo->MediaName(),
                                            long(BackupInfo->CountInPages(i)),
                                            int(BackupInfo->PagesPerBlock()),
                                            BackupInfo->NumberOfPipes()>1?'M':'S',
                                            BackupInfo->NameOfPipe(i),
                                            (int)i);

                                    long BytesWritten;

                                    if(TheLogFile.WriteLine(NewLogFileEntry, BytesWritten))
                                    {
                                        sprintf(MsgBuf_cn36, "Wrote '%s' to the external backup history.\n", NewLogFileEntry);
                                        LogWriter->Write(MsgBuf_cn36);
                                    }
                                    else
                                    {
                                        AllIsOK=0; //end the loop
                                        ErrorHandler->Report(tcn35e_DBMBackupError(tcn35e_DBMBackupError::CaseCouldNotWriteToToolHistFile, BackupInfo->PathOfExternalBackupHistory()));
                                    }

                                    if(isHSS) //append NewLogFileEntry to the HSS command line, if this is a HSS
                                    {
                                        char * newLineForHSSCommand=0;

                                        if(cn36_StrAlloc(newLineForHSSCommand, strlen(lineForHSSCommand)+2+strlen(NewLogFileEntry)+1))
                                        {
                                            strcpy(newLineForHSSCommand, lineForHSSCommand);
                                            strcat(newLineForHSSCommand, " \"");
                                            strcat(newLineForHSSCommand, NewLogFileEntry);
                                            strcat(newLineForHSSCommand, "\"");

                                            cn36_StrDealloc(lineForHSSCommand);
                                            lineForHSSCommand=newLineForHSSCommand;
                                        }
                                        else
                                        {
                                            ErrorHandler->Report(tcn35d_DBBackupError::OutOfMemory.Set());
                                            AllIsOK=0;
                                        }
                                    }
                                }
                            }

                            int AllIsOKBeforeClose=AllIsOK;

                            if(!TheLogFile.Close())
                                AllIsOK=0;

                            if(AllIsOKBeforeClose && isHSS)
                            {
                                DBMSrv_Reply tempReply; //use a temporary reply buffer, as we are not really interested in the output during update of dbm.ebf

                                cn00DBMServerData::vcontrol()->pHSSNodes->ExecuteOnAllOtherNodes(lineForHSSCommand, tempReply);
                            }
                        }
                        else
                            AllIsOK=0;
                    }

                    cn36_StrDealloc(lineForHSSCommand); //last chance to free the memory
                }
                else
                    ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));  //AllIsOK is already 0
            }
        }
        else
            ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));  //AllIsOK is  already 0

        if(AllIsOK)
            LogWriter->Write("Have updated external backup history successfully.\n", tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefixAndEndParagraph);
        else
            LogWriter->Write("Could not update external backup history.\n", tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefixAndEndParagraph);
    }
}

void tcn35b_DBMExternalBackupController::DBPostOperation()
{
    if(tcn35d_BackupInfo::Log==BackupInfo->DataType() && tcn35d_BackupInfo::ABackup==BackupInfo->DataDirection())  //we make a log backup
    {
        FreeLogAfterLogBackup();
    }
}

void tcn35b_DBMExternalBackupController::FillReplyBuffer()
{
    LogWriter->Write("Filling reply buffer.\n", tcn35d_DBBackupControllerLogWriter::AddALevelToPrefix);

    switch(RequestState())
    {
        case NoRequestSend:
        case RequestSend:
        case CancelSendByDBMSrv:
            ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError)); // must have reported already an error!  (wheter tool was started or not)
        break;

        case RequestReplied:
        {
            switch(ToolState())
            {
                case NotStarted:
                {
                    if(!IsBad()) // tool was not started, because db had finished already (no error reported, tool not started -> nobody tried to start the tool
                        PrintBackupAnswer(); //just tell what the db-kernel sayed
                    else  //the error was already reported (written in the reply buffer), FillReplyBuffer() should not have been called in this case!
                        ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
                }
                break;
                case Ended:
                {
                    if(BackupToolConnector->ToolWasSuccessful(DBBackupSize) || ToolMustNotBeSuccessful())
                        PrintBackupAnswer();
                    else
                    {
                        if(DBActionWasSuccessful())
                        {
                            tcn35d_DBBackupError Error(tcn35d_DBBackupError::CaseBackupToolFailed, BackupToolConnector->SumOfToolExitCodes());
                            ErrorHandler->Report(Error);

                            if(0!=Error.GiveErrorMsgBuffer())
                                cn36_StrAllocCpy(ToolErrorText, Error.GiveErrorMsgBuffer());
                        }
                        else
                            ErrorHandler->Report(tcn35e_DBMBackupError(tcn35e_DBMBackupError::CaseBackupToolAndDBFailed, SQLCodeOfRequest, BackupToolConnector->SumOfToolExitCodes()));
                    }
                }
                break;
                case KilledByDBMSrv:
                {
                    tcn35e_DBMBackupError Error(tcn35e_DBMBackupError::CaseBackupToolKilled, SQLCodeOfRequest, BackupToolConnector->SumOfToolExitCodes());
                    ErrorHandler->Report(Error);

                    if(0!=Error.GiveErrorMsgBuffer())
                        cn36_StrAllocCpy(ToolErrorText, Error.GiveErrorMsgBuffer());
                }
                break;
                default: ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
            }
        }
        break;

        case CanceledByDBMSrv:
        {
            switch(ToolState())
            {
                case NotStarted:
                {
                    ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
                    // if no error was reported so far(!IsBad()), tool must have been started
                    // if error was reported already(IsBad()), this function must not have been called
                }
                break;
                case Ended:
                {
                    if(BackupToolConnector->ToolWasSuccessful(DBBackupSize))  // do not override a tool error
                        ErrorHandler->Report(tcn35e_DBMBackupError(tcn35e_DBMBackupError::CaseDBKernelRequestCanceled, SQLCodeOfRequest, BackupToolConnector->SumOfToolExitCodes()));
                    else
                        ErrorHandler->Report(tcn35e_DBMBackupError(tcn35e_DBMBackupError::CaseBackupToolFailedAndDBKernelRequestCanceled, SQLCodeOfRequest, BackupToolConnector->SumOfToolExitCodes()));
                }
                break;
                case KilledByDBMSrv:
                {
                    ErrorHandler->Report(tcn35e_DBMBackupError(tcn35e_DBMBackupError::CaseBackupToolKilledAndDBKernelRequestCanceled, SQLCodeOfRequest, BackupToolConnector->SumOfToolExitCodes()));
                }
                break;
                default: ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
            }
        }
        break;

        case TerminatedByDBMSrv:
        {
            switch(ToolState())
            {
                case NotStarted:
                {
                    ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
                    // if no error was reported so far(!IsBad()), tool must have been started
                    // if error was reported already(IsBad()), this function must not have been called
                }
                break;
                case Ended:
                {
                    if(BackupToolConnector->ToolWasSuccessful(DBBackupSize))  // do not override a tool error
                        ErrorHandler->Report(tcn35e_DBMBackupError(tcn35e_DBMBackupError::CaseCancelDBKernelRequestFailed, BackupToolConnector->SumOfToolExitCodes()));
                    else
                        ErrorHandler->Report(tcn35e_DBMBackupError(tcn35e_DBMBackupError::CaseBackupToolFailedAndCancelDBKernelRequestFailed, BackupToolConnector->SumOfToolExitCodes()));
                }
                break;
                case KilledByDBMSrv:
                {
                    ErrorHandler->Report(tcn35e_DBMBackupError(tcn35e_DBMBackupError::CaseBackupToolKilledAndCancelDBKernelRequestFailed));
                }
                break;
                default: ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
            }
        }
        break;

        default: ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
    }

    LogWriter->Write("Reply buffer filled.\n", tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefixAndEndParagraph);
}

int tcn35b_DBMExternalBackupController::SendBackupRestoreRequestToKernel(DBMSrvKnl_Session & Session,
                                                                         const char        * TheKernelCommand,
                                                                         int               & SQLCode,
                                                                         int                 BeQuiet,
                                                                         int                 ForBackup)
{
    int rc=1;
    DBMSrvMsg_Error KernelSessionError;

    if(!BeQuiet)
    {
        sprintf(MsgBuf_cn36, "Requesting '%s' from db-kernel.\n", TheKernelCommand);
        LogWriter->Write(MsgBuf_cn36);
    }

    if (!Session.Request(TheKernelCommand, KernelSessionError)) 
    {
        if( KernelSessionError==DBMSrvMsg_Error::DBMSrv_SQL )
        {
            SQLCode = KernelSessionError.getTagValueAsInteger(SDBMSGTAG_DBMSRV_SQL__RETURNCODE);
        } 
        else
            SQLCode=DBMSrvMsg_Error::get_tcn00_Error(DBMSrvMsg_Error::DBMSrv_ERR);

        ErrorHandler->reportMessageList(KernelSessionError);

        rc=0;
    }

    return rc;
}

void tcn35b_DBMExternalBackupController::PrintBackupAnswer()
{
    if(0!=SQLCodeOfRequest && SQLErrForRequest)
        ErrorHandler->Report(tcn35e_DBMBackupError(tcn35e_DBMBackupError::CaseSQLError, SQLCodeOfRequest, (char *)SQLErrText));
    else
    {
        char * pCurrPos = 0;

        sprintf(ReplyData, "%s%s", ANSWER_OK_CN00, LINE_SEPSTRING_CN00);
        pCurrPos = ReplyData + strlen(ReplyData);

        sprintf(pCurrPos, "%-20s%s%d%s",BA_RESULT_RC_CN003, VALUE_SEPSTRING_CN00, SQLCodeOfRequest, LINE_SEPSTRING_CN00);
        pCurrPos = ReplyData + strlen(ReplyData);

        sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_DATE_CN003, VALUE_SEPSTRING_CN00, TheDBResult.szDate, LINE_SEPSTRING_CN00);
        pCurrPos = ReplyData + strlen(ReplyData);

        sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_TIME_CN003, VALUE_SEPSTRING_CN00, TheDBResult.szTime, LINE_SEPSTRING_CN00);
        pCurrPos = ReplyData + strlen(ReplyData);

        sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_SERVER_CN003, VALUE_SEPSTRING_CN00, TheDBResult.szServer, LINE_SEPSTRING_CN00);
        pCurrPos = ReplyData + strlen(ReplyData);

        sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_DATABASE_CN003, VALUE_SEPSTRING_CN00, TheDBResult.szDatabase, LINE_SEPSTRING_CN00);
        pCurrPos = ReplyData + strlen(ReplyData);

        sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_KNLVER_CN003, VALUE_SEPSTRING_CN00, TheDBResult.szKernelVersion, LINE_SEPSTRING_CN00);
        pCurrPos = ReplyData + strlen(ReplyData);

        sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_PGSTRAN_CN003, VALUE_SEPSTRING_CN00, TheDBResult.szPagesTransfered, LINE_SEPSTRING_CN00);
        pCurrPos = ReplyData + strlen(ReplyData);

        sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_PGSLEFT_CN003, VALUE_SEPSTRING_CN00, TheDBResult.szPagesLeft, LINE_SEPSTRING_CN00);
        pCurrPos = ReplyData + strlen(ReplyData);

        sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_VOLUMES_CN003, VALUE_SEPSTRING_CN00, TheDBResult.szVolumes, LINE_SEPSTRING_CN00);
        pCurrPos = ReplyData + strlen(ReplyData);

        sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_MEDNAME_CN003, VALUE_SEPSTRING_CN00, TheDBResult.szMediaName, LINE_SEPSTRING_CN00);
        pCurrPos = ReplyData + strlen(ReplyData);

        sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_LOCATION_CN003, VALUE_SEPSTRING_CN00, TheDBResult.szLocation, LINE_SEPSTRING_CN00);
        pCurrPos = ReplyData + strlen(ReplyData);

        sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_ERRTEXT_CN003, VALUE_SEPSTRING_CN00, TheDBResult.szErrortext, LINE_SEPSTRING_CN00);
        pCurrPos = ReplyData + strlen(ReplyData);

        sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_LABEL_CN003, VALUE_SEPSTRING_CN00, TheDBResult.szLabel, LINE_SEPSTRING_CN00);
        pCurrPos = ReplyData + strlen(ReplyData);

        sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_ISCONS_CN003, VALUE_SEPSTRING_CN00, TheDBResult.szIsConsistent, LINE_SEPSTRING_CN00);
        pCurrPos = ReplyData + strlen(ReplyData);

        sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_FLOGPG_CN003, VALUE_SEPSTRING_CN00, TheDBResult.szFirstLogPageNo, LINE_SEPSTRING_CN00);
        pCurrPos = ReplyData + strlen(ReplyData);

        sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_LLOGPG_CN003, VALUE_SEPSTRING_CN00, TheDBResult.szLastLogPageNo, LINE_SEPSTRING_CN00);
        pCurrPos = ReplyData + strlen(ReplyData);

        sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_DBSTMD1_CN003, VALUE_SEPSTRING_CN00, TheDBResult.szDBStamp1Date, LINE_SEPSTRING_CN00);
        pCurrPos = ReplyData + strlen(ReplyData);

        sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_DBSTMT1_CN003, VALUE_SEPSTRING_CN00, TheDBResult.szDBStamp1Time, LINE_SEPSTRING_CN00);
        pCurrPos = ReplyData + strlen(ReplyData);

        sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_DBSTMD2_CN003, VALUE_SEPSTRING_CN00, TheDBResult.szDBStamp2Date, LINE_SEPSTRING_CN00);
        pCurrPos = ReplyData + strlen(ReplyData);

        sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_DBSTMT2_CN003, VALUE_SEPSTRING_CN00, TheDBResult.szDBStamp2Time, LINE_SEPSTRING_CN00);
        pCurrPos = ReplyData + strlen(ReplyData);

        sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_PGCNT_CN003, VALUE_SEPSTRING_CN00, TheDBResult.szBDPageCount, LINE_SEPSTRING_CN00);
        pCurrPos = ReplyData + strlen(ReplyData);

        sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_DEVUSE_CN003, VALUE_SEPSTRING_CN00, TheDBResult.szDevicesUsed, LINE_SEPSTRING_CN00);
        pCurrPos = ReplyData + strlen(ReplyData);

        sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_DBID_CN003, VALUE_SEPSTRING_CN00, TheDBResult.szDatabaseID, LINE_SEPSTRING_CN00);
        pCurrPos = ReplyData + strlen(ReplyData);

        sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_MUSEDATAPG_CN003, VALUE_SEPSTRING_CN00, TheDBResult.szMaxUsedDataPage, LINE_SEPSTRING_CN00);
        pCurrPos = ReplyData + strlen(ReplyData);

        sprintf(pCurrPos, "%-20s%s%s%s",BA_RESULT_CONVPGCNT_CN003, VALUE_SEPSTRING_CN00, TheDBResult.szConvPageCnt, LINE_SEPSTRING_CN00);
        pCurrPos = ReplyData + strlen(ReplyData);

        //szRedoTransRead and szRedoTransDone should not be displayed for backup/restore commands

        LogWriter->Write("Constructed the following reply:\n", tcn35d_DBBackupControllerLogWriter::AddALevelToPrefix);

        sprintf(MsgBuf_cn36, "%s%s", ANSWER_OK_CN00, LINE_SEPSTRING_CN00);
        LogWriter->Write(MsgBuf_cn36);

        sprintf(MsgBuf_cn36, "%-20s%s%d%s",BA_RESULT_RC_CN003, VALUE_SEPSTRING_CN00, SQLCodeOfRequest, LINE_SEPSTRING_CN00);
        LogWriter->Write(MsgBuf_cn36);

        sprintf(MsgBuf_cn36, "%-20s%s%s%s",BA_RESULT_DATE_CN003, VALUE_SEPSTRING_CN00, TheDBResult.szDate, LINE_SEPSTRING_CN00);
        LogWriter->Write(MsgBuf_cn36);

        sprintf(MsgBuf_cn36, "%-20s%s%s%s",BA_RESULT_TIME_CN003, VALUE_SEPSTRING_CN00, TheDBResult.szTime, LINE_SEPSTRING_CN00);
        LogWriter->Write(MsgBuf_cn36);

        sprintf(MsgBuf_cn36, "%-20s%s%s%s",BA_RESULT_SERVER_CN003, VALUE_SEPSTRING_CN00, TheDBResult.szServer, LINE_SEPSTRING_CN00);
        LogWriter->Write(MsgBuf_cn36);

        sprintf(MsgBuf_cn36, "%-20s%s%s%s",BA_RESULT_DATABASE_CN003, VALUE_SEPSTRING_CN00, TheDBResult.szDatabase, LINE_SEPSTRING_CN00);
        LogWriter->Write(MsgBuf_cn36);

        sprintf(MsgBuf_cn36, "%-20s%s%s%s",BA_RESULT_KNLVER_CN003, VALUE_SEPSTRING_CN00, TheDBResult.szKernelVersion, LINE_SEPSTRING_CN00);
        LogWriter->Write(MsgBuf_cn36);

        sprintf(MsgBuf_cn36, "%-20s%s%s%s",BA_RESULT_PGSTRAN_CN003, VALUE_SEPSTRING_CN00, TheDBResult.szPagesTransfered, LINE_SEPSTRING_CN00);
        LogWriter->Write(MsgBuf_cn36);

        sprintf(MsgBuf_cn36, "%-20s%s%s%s",BA_RESULT_PGSLEFT_CN003, VALUE_SEPSTRING_CN00, TheDBResult.szPagesLeft, LINE_SEPSTRING_CN00);
        LogWriter->Write(MsgBuf_cn36);

        sprintf(MsgBuf_cn36, "%-20s%s%s%s",BA_RESULT_VOLUMES_CN003, VALUE_SEPSTRING_CN00, TheDBResult.szVolumes, LINE_SEPSTRING_CN00);
        LogWriter->Write(MsgBuf_cn36);

        sprintf(MsgBuf_cn36, "%-20s%s%s%s",BA_RESULT_MEDNAME_CN003, VALUE_SEPSTRING_CN00, TheDBResult.szMediaName, LINE_SEPSTRING_CN00);
        LogWriter->Write(MsgBuf_cn36);

        sprintf(MsgBuf_cn36, "%-20s%s%s%s",BA_RESULT_LOCATION_CN003, VALUE_SEPSTRING_CN00, TheDBResult.szLocation, LINE_SEPSTRING_CN00);
        LogWriter->Write(MsgBuf_cn36);

        sprintf(MsgBuf_cn36, "%-20s%s%s%s",BA_RESULT_ERRTEXT_CN003, VALUE_SEPSTRING_CN00, TheDBResult.szErrortext, LINE_SEPSTRING_CN00);
        LogWriter->Write(MsgBuf_cn36);

        sprintf(MsgBuf_cn36, "%-20s%s%s%s",BA_RESULT_LABEL_CN003, VALUE_SEPSTRING_CN00, TheDBResult.szLabel, LINE_SEPSTRING_CN00);
        LogWriter->Write(MsgBuf_cn36);

        sprintf(MsgBuf_cn36, "%-20s%s%s%s",BA_RESULT_ISCONS_CN003, VALUE_SEPSTRING_CN00, TheDBResult.szIsConsistent, LINE_SEPSTRING_CN00);
        LogWriter->Write(MsgBuf_cn36);

        sprintf(MsgBuf_cn36, "%-20s%s%s%s",BA_RESULT_FLOGPG_CN003, VALUE_SEPSTRING_CN00, TheDBResult.szFirstLogPageNo, LINE_SEPSTRING_CN00);
        LogWriter->Write(MsgBuf_cn36);

        sprintf(MsgBuf_cn36, "%-20s%s%s%s",BA_RESULT_LLOGPG_CN003, VALUE_SEPSTRING_CN00, TheDBResult.szLastLogPageNo, LINE_SEPSTRING_CN00);
        LogWriter->Write(MsgBuf_cn36);

        sprintf(MsgBuf_cn36, "%-20s%s%s%s",BA_RESULT_DBSTMD1_CN003, VALUE_SEPSTRING_CN00, TheDBResult.szDBStamp1Date, LINE_SEPSTRING_CN00);
        LogWriter->Write(MsgBuf_cn36);

        sprintf(MsgBuf_cn36, "%-20s%s%s%s",BA_RESULT_DBSTMT1_CN003, VALUE_SEPSTRING_CN00, TheDBResult.szDBStamp1Time, LINE_SEPSTRING_CN00);
        LogWriter->Write(MsgBuf_cn36);

        sprintf(MsgBuf_cn36, "%-20s%s%s%s",BA_RESULT_DBSTMD2_CN003, VALUE_SEPSTRING_CN00, TheDBResult.szDBStamp2Date, LINE_SEPSTRING_CN00);
        LogWriter->Write(MsgBuf_cn36);

        sprintf(MsgBuf_cn36, "%-20s%s%s%s",BA_RESULT_DBSTMT2_CN003, VALUE_SEPSTRING_CN00, TheDBResult.szDBStamp2Time, LINE_SEPSTRING_CN00);
        LogWriter->Write(MsgBuf_cn36);

        sprintf(MsgBuf_cn36, "%-20s%s%s%s",BA_RESULT_PGCNT_CN003, VALUE_SEPSTRING_CN00, TheDBResult.szBDPageCount, LINE_SEPSTRING_CN00);
        LogWriter->Write(MsgBuf_cn36);

        sprintf(MsgBuf_cn36, "%-20s%s%s%s",BA_RESULT_DEVUSE_CN003, VALUE_SEPSTRING_CN00, TheDBResult.szDevicesUsed, LINE_SEPSTRING_CN00);
        LogWriter->Write(MsgBuf_cn36);

        sprintf(MsgBuf_cn36, "%-20s%s%s%s",BA_RESULT_DBID_CN003, VALUE_SEPSTRING_CN00, TheDBResult.szDatabaseID, LINE_SEPSTRING_CN00);
        LogWriter->Write(MsgBuf_cn36);

        sprintf(MsgBuf_cn36, "%-20s%s%s%s",BA_RESULT_MUSEDATAPG_CN003, VALUE_SEPSTRING_CN00, TheDBResult.szMaxUsedDataPage, LINE_SEPSTRING_CN00);
        LogWriter->Write(MsgBuf_cn36);

        sprintf(MsgBuf_cn36, "%-20s%s%s%s",BA_RESULT_CONVPGCNT_CN003, VALUE_SEPSTRING_CN00, TheDBResult.szConvPageCnt, LINE_SEPSTRING_CN00);
        LogWriter->Write(MsgBuf_cn36);

        LogWriter->Write(tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefixAndEndParagraph);
    }
}

int tcn35b_DBMExternalBackupController::ToolMustNotBeSuccessful()
{
    const int NumberOfErrorsAllowedForRestore=6;
    int rc=0,
        ToolErrorAllowedForRestoreSQLCode[NumberOfErrorsAllowedForRestore]={0, -3004, -7075, -7076, -8003, -8020};

    switch(BackupInfo->DataDirection())
    {
        case tcn35d_BackupInfo::ABackup:   break;  //The tool must always be successful on backup

        case tcn35d_BackupInfo::ARestore:
        {
            if(tcn35d_BackupInfo::Log==BackupInfo->DataType())  //while restoring log ignore the errors from ToolErrorAllowedForRestoreSQLCode
            {
                int i;

                for(i=0; 0==rc && i<NumberOfErrorsAllowedForRestore; i++)
                    if(SQLCodeOfRequest==ToolErrorAllowedForRestoreSQLCode[i])
                        rc=1;
            }
        }break;

        default:
            ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
    }

    return rc;
}


// -----------------------------------------------------------------------------
// members of class tcn35b_DBMExternalBackupControllerFactory
// -----------------------------------------------------------------------------

tcn35b_DBMExternalBackupControllerFactory TheDBMExternalBackupControllerFactory_cn35b;

tcn00_Error tcn35b_DBMExternalBackupControllerFactory::ConstructADBMExternalBackupController(tcn35b_DBMExternalBackupController *& TheNewBackupController,
                                                                                             char                               *  TheReplyBuffer,
                                                                                             int                                *  TheReplyBufferLength,
                                                                                             tcn35d_BackupInfo::ADataDirection     TheDataDirection,
                                                                                             const char                         *  BackupType,
                                                                                             const int                             WithoutCheckpoint,
                                                                                             DBMSrvKnl_Session                  *  TheSQLSession,
                                                                                             const char                         *  TheKernelCommand,
                                                                                             const tcn003_Medium                *  pMedium,
                                                                                             const tcn003_MediaGroup            *  pMediaGroup,
                                                                                             const char                         *  TheDBName,
                                                                                             const char                         *  TheSourceNodeName,
                                                                                             const char                         *  TheSourceDBName,
                                                                                             const char                         *  ExternalBackupIDList,
                                                                                             const char                         *  EndOfExternalBackupIDList)
{
    tcn00_Error rc=OK_CN00;

    tcn35d_BackupInfo::ADataType TheDataType;

    tcn35e_DBMBackupControllerErrorHandler::BasicErrorHandler_cn35e.SetReplyArrayTo(TheReplyBuffer, TheReplyBufferLength);

    if(DataTypeForDBMBackupType(TheDataType, BackupType, WithoutCheckpoint, tcn35e_DBMBackupControllerErrorHandler::BasicErrorHandler_cn35e))
    {
        const char * MediumName;
        int          NumberOfPipes;
        long         BytesPerBlock;
        long         PagesPerBlock;
        long         BytesPerPage;
        long         TheCountInPages[MAX_NUMBER_OF_DATAPIPES_CN35D];
        char       * ThePipeNames[MAX_NUMBER_OF_DATAPIPES_CN35D];
        char       * TheMediumTypes[MAX_NUMBER_OF_DATAPIPES_CN35D];
        char       * BackupHistory=0;
        char       * ExternalBackupHistory=0;

        if(!MediumInfo(MediumName,
                       NumberOfPipes,
                       ThePipeNames,
                       TheMediumTypes,
                       BytesPerBlock,
                       PagesPerBlock,
                       BytesPerPage,
                       TheCountInPages,
                       TheDBName,
                       pMedium,
                       pMediaGroup,
                       tcn35e_DBMBackupControllerErrorHandler::BasicErrorHandler_cn35e))
        {
            rc=tcn35e_DBMBackupControllerErrorHandler::BasicErrorHandler_cn35e.GiveLastError();
        }

        if(OK_CN00==rc)
        {
            if(!BackupHistories(BackupHistory,
                                ExternalBackupHistory,
                                TheDBName,
                                tcn35e_DBMBackupControllerErrorHandler::BasicErrorHandler_cn35e))
            {
                rc=tcn35e_DBMBackupControllerErrorHandler::BasicErrorHandler_cn35e.GiveLastError();
            }
        }

        if(OK_CN00==rc)
        {
            InputStructForConstruct HelpStruct;

            HelpStruct.TheReplyBuffer=TheReplyBuffer;
            HelpStruct.TheReplyBufferLength=TheReplyBufferLength;
            HelpStruct.TheDataDirection=TheDataDirection;
            HelpStruct.TheDataType=TheDataType;
            HelpStruct.TheSQLSession=TheSQLSession;
            HelpStruct.TheKernelCommand=TheKernelCommand;
            HelpStruct.BackupToolType=DBMSrvBMedia_MediaGroup::BackupTool(pMedium, pMediaGroup);
            HelpStruct.MediumName=MediumName;
            HelpStruct.NumberOfPipes=NumberOfPipes;
            HelpStruct.ThePipeNames=ThePipeNames;
            HelpStruct.TheMediumTypes=TheMediumTypes;
            HelpStruct.BytesPerBlock=BytesPerBlock;
            HelpStruct.PagesPerBlock=PagesPerBlock;
            HelpStruct.BytesPerPage=BytesPerPage;
            HelpStruct.CountInPages=TheCountInPages;
            HelpStruct.TheDBName=TheDBName;
            HelpStruct.TheSourceNodeName=TheSourceNodeName;
            HelpStruct.TheSourceDBName=TheSourceDBName;
            HelpStruct.BackupHistory=BackupHistory;
            HelpStruct.ExternalBackupHistory=ExternalBackupHistory;
            HelpStruct.ExternalBackupIDList=ExternalBackupIDList;
            HelpStruct.EndOfExternalBackupIDList=EndOfExternalBackupIDList;

            rc=ConstructADBMExternalBackupController(TheNewBackupController,
                                                     HelpStruct);
        }
        else
        {
            int i;

            for(i=0; i<MAX_NUMBER_OF_DATAPIPES_CN35D; i++)
            {
                cn36_StrDealloc(ThePipeNames[i]);
                cn36_StrDealloc(TheMediumTypes[i]);
            }
        }

        cn36_StrDealloc(BackupHistory);
        cn36_StrDealloc(ExternalBackupHistory);
    }
    else
        rc=tcn35e_DBMBackupControllerErrorHandler::BasicErrorHandler_cn35e.GiveLastError();

    return rc;
}

tcn00_Error tcn35b_DBMExternalBackupControllerFactory::ConstructADBMExternalBackupController(tcn35b_DBMExternalBackupController *& TheNewBackupController,
                                                                                             char                               *  TheReplyBuffer,
                                                                                             int                                *  TheReplyBufferLength,
                                                                                             tcn35d_BackupInfo::ADataDirection     TheDataDirection,
                                                                                             DBMSrvBTools_ToolEnum                 BackupToolType,
                                                                                             const char                         *  TheDBName,
                                                                                             const char                         *  TheSourceDBName,
                                                                                             const char                         *  TheSourceNodeName)
{
    tcn00_Error rc=OK_CN00;

    char * ThePipeNames[MAX_NUMBER_OF_DATAPIPES_CN35D];
    long   TheCountInPages[MAX_NUMBER_OF_DATAPIPES_CN35D];
    char * TheMediumTypes[MAX_NUMBER_OF_DATAPIPES_CN35D];
    int i;

    char * BackupHistory=0;
    char * ExternalBackupHistory=0;

    tcn35e_DBMBackupControllerErrorHandler::BasicErrorHandler_cn35e.SetReplyArrayTo(TheReplyBuffer, TheReplyBufferLength);

    for(i=0; i<MAX_NUMBER_OF_DATAPIPES_CN35D; i++)
    {
        ThePipeNames[i]=0;
        TheCountInPages[i]=0;
        TheMediumTypes[i]=0;
    }

    if(!BackupHistories(BackupHistory,
                        ExternalBackupHistory,
                        TheDBName,
                        tcn35e_DBMBackupControllerErrorHandler::BasicErrorHandler_cn35e))
    {
        rc=tcn35e_DBMBackupControllerErrorHandler::BasicErrorHandler_cn35e.GiveLastError();
    }
    else
    {
        InputStructForConstruct HelpStruct;

        HelpStruct.TheReplyBuffer=TheReplyBuffer;
        HelpStruct.TheReplyBufferLength=TheReplyBufferLength;
        HelpStruct.TheDataDirection=TheDataDirection;
        HelpStruct.TheDataType=tcn35d_BackupInfo::DataMigration;
        HelpStruct.TheSQLSession=0;
        HelpStruct.TheKernelCommand=0;
        HelpStruct.BackupToolType=BackupToolType;
        HelpStruct.MediumName="";
        HelpStruct.NumberOfPipes=0;
        HelpStruct.ThePipeNames=ThePipeNames;
        HelpStruct.TheMediumTypes=TheMediumTypes;
        HelpStruct.BytesPerBlock=0;
        HelpStruct.PagesPerBlock=0;
        HelpStruct.BytesPerPage=0;
        HelpStruct.CountInPages=TheCountInPages;
        HelpStruct.TheDBName=TheDBName;
        HelpStruct.TheSourceNodeName=TheSourceNodeName;
        HelpStruct.TheSourceDBName=TheSourceDBName;
        HelpStruct.BackupHistory=BackupHistory;
        HelpStruct.ExternalBackupHistory=ExternalBackupHistory;
        HelpStruct.ExternalBackupIDList=0;
        HelpStruct.EndOfExternalBackupIDList=0;

        rc=ConstructADBMExternalBackupController(TheNewBackupController, HelpStruct);
    }

    cn36_StrDealloc(BackupHistory);
    cn36_StrDealloc(ExternalBackupHistory);

    return rc;
}

tcn00_Error tcn35b_DBMExternalBackupControllerFactory::ConstructADBMExternalBackupController(tcn35b_DBMExternalBackupController *& TheNewBackupController,
                                                                                             InputStructForConstruct             & TheInputPars)
{
    tcn00_Error rc=OK_CN00;

    TheNewBackupController=0;

    tcn35e_DBMBackupControllerLogWriter    * LogWriter=0;
    tcn35e_DBMBackupControllerErrorHandler * ErrorHandler=0;
    tcn35_BackupToolConnector              * BackupTool=0;
    tcn35d_BackupInfo                      * BackupInfo=0;

    tsp00_Pathc   AbsoluteNameOfProtocolFile;   //dbm.ebp
    tsp00_Pathc   AbsoluteNameOfLogFile;        //ebm.ebl
    tsp00_DbNamec tDBName;

    tDBName.rawAssign(TheInputPars.TheDBName);

    rc=cn42GetFileName(tDBName, FGET_BACKEBP_CN42, AbsoluteNameOfProtocolFile);

    if(OK_CN00!=rc)
        tcn35e_DBMBackupControllerErrorHandler::BasicErrorHandler_cn35e.Report(tcn35e_DBMBackupError(tcn35e_DBMBackupError::CaseCouldNotGetNameOfToolProtFile));
    else
    {
        rc=cn42GetFileName(tDBName, FGET_BACKEBL_CN42, AbsoluteNameOfLogFile);

        if(OK_CN00!=rc)
            tcn35e_DBMBackupControllerErrorHandler::BasicErrorHandler_cn35e.Report(tcn35e_DBMBackupError(tcn35e_DBMBackupError::CaseCouldNotGetNameOfToolLogFile));
    }

    if(OK_CN00==rc)
    {
        tsp00_Namec   VariableName;
        VariableName.rawAssign(tcn35e_DBMBackupControllerLogWriter::ParameterNameDBMEBLSize);

        cn51DBMConfigValue WishedActionsInLogFile(tDBName, VariableName);

        LogWriter=new tcn35e_DBMBackupControllerLogWriter(AbsoluteNameOfProtocolFile, AbsoluteNameOfLogFile, WishedActionsInLogFile);

        if(0==LogWriter || !LogWriter->IsGood())
        {
            rc=ERR_MEM_CN00;
            tcn35e_DBMBackupControllerErrorHandler::BasicErrorHandler_cn35e.Report(tcn35d_DBBackupError::OutOfMemory.Set());
        }
    }

    if(OK_CN00==rc)
    {
        ErrorHandler=new tcn35e_DBMBackupControllerErrorHandler(TheInputPars.TheReplyBuffer,
                                                                TheInputPars.TheReplyBufferLength,
                                                                LogWriter);

        if(0==ErrorHandler)
        {
            rc=ERR_MEM_CN00;
            tcn35e_DBMBackupControllerErrorHandler::BasicErrorHandler_cn35e.Report(tcn35d_DBBackupError::OutOfMemory.Set());
        }

    }

    if(OK_CN00==rc)
    {
        char * Rundirectory=0;
        char * TheNodeName=0;

        ErrorHandler->SetPhaseTo(tcn35d_DBBackupControllerErrorHandler::PrepareOperation);

        if(OK_CN00==rc && !RunDirectory(Rundirectory, TheInputPars.TheDBName, ErrorHandler, LogWriter))
            rc=ErrorHandler->GiveLastError();

        if(OK_CN00==rc)
        {
            const char * TheNodeNameConst=cn36_GetHostName();

            if(0==TheNodeNameConst)
                rc=ErrorHandler->GiveLastError();
            else
                if(!cn36_StrAllocCpy(TheNodeName, TheNodeNameConst))    //make a copy of the string
                    rc=ErrorHandler->GiveLastError();

            if(OK_CN00==rc)
            {
                BackupInfo=new tcn35d_BackupInfo(TheInputPars.TheDataDirection,
                                                 TheInputPars.TheDataType,
                                                 TheInputPars.MediumName,
                                                 TheInputPars.NumberOfPipes,
                                                 TheInputPars.ThePipeNames,
                                                 TheInputPars.TheMediumTypes,
                                                 TheInputPars.BytesPerBlock,
                                                 TheInputPars.PagesPerBlock,
                                                 TheInputPars.CountInPages,
                                                 Rundirectory,
                                                 TheNodeName,
                                                 TheInputPars.TheDBName,
                                                 TheInputPars.TheSourceNodeName,
                                                 TheInputPars.TheSourceDBName,
                                                 TheInputPars.BackupHistory,
                                                 TheInputPars.ExternalBackupHistory);

                if(0==BackupInfo)
                {
                    rc=ERR_MEM_CN00;
                    ErrorHandler->Report(tcn35d_DBBackupError::OutOfMemory.Set());
                }
                else
                    if(!BackupInfo->IsGood())
                        rc=ErrorHandler->GiveLastError();
            }
        }

        cn36_StrDealloc(Rundirectory);
        cn36_StrDealloc(TheNodeName);
    }

    if(OK_CN00!=rc) //clean up if something went wrong
    {
        if(0==BackupInfo)
        {
            int i;

            for(i=0; i<MAX_NUMBER_OF_DATAPIPES_CN35D; i++)
                cn36_StrDealloc(TheInputPars.ThePipeNames[i]);
        }
        else
        {
            delete BackupInfo;
            BackupInfo=0;
        }
    }

    tcn35e_ConfigurationVariableHandler * ConfigurationHandler=0;

    if(OK_CN00==rc)
    {
        ConfigurationHandler=new tcn35e_ConfigurationVariableHandler();
        char * CopyOfExternalBackupIDList=0;

        if(0==ConfigurationHandler)
        {
            rc=ERR_MEM_CN00;
            ErrorHandler->Report(tcn35d_DBBackupError::OutOfMemory.Set());
        }


        if(OK_CN00==rc &&                                                               //all ok so far
           0!=TheInputPars.ExternalBackupIDList &&                                      //ExternalBackupIDList has a start
           0!=TheInputPars.EndOfExternalBackupIDList &&                                 //ExternalBackupIDList has an end
           0<TheInputPars.EndOfExternalBackupIDList-TheInputPars.ExternalBackupIDList)  //ExternalBackupIDList is not empty
        {
            if(!cn36_StrNAllocCpy(CopyOfExternalBackupIDList, TheInputPars.ExternalBackupIDList, TheInputPars.EndOfExternalBackupIDList-TheInputPars.ExternalBackupIDList))
                rc=ERR_MEM_CN00;
        }

        if(OK_CN00==rc)
        {
            if(!tcn35_BackupToolConnector::ConstructBackupToolConnector(BackupTool,
                                                                        TheInputPars.BackupToolType,
                                                                        BackupInfo,
                                                                        LogWriter,
                                                                        ErrorHandler,
                                                                        ConfigurationHandler,
                                                                        CopyOfExternalBackupIDList))
            {
                rc=ErrorHandler->GiveLastError();
            }
        }

        if(OK_CN00!=rc)
        {
            if(0==BackupTool)
            {
                if(0!=ConfigurationHandler)
                    delete ConfigurationHandler;

                cn36_StrDealloc(CopyOfExternalBackupIDList);
            }
            else
            {
                delete BackupTool;
                BackupTool=0;
            }
        }
    }

    char * CopyOfKernelCommand=0;

    if(OK_CN00==rc && 0!=TheInputPars.TheKernelCommand)
    {
        if(!cn36_StrAllocCpy(CopyOfKernelCommand, TheInputPars.TheKernelCommand))
            rc=ERR_MEM_CN00;
    }

    if(OK_CN00==rc)
    {
        tcn35b_DBMExternalBackupControllerConstructorArguments Args;

        Args.TheBackupToolConnector=BackupTool;
        Args.TheErrorHandler=ErrorHandler;
        Args.TheLogWriter=LogWriter;
        Args.TheBackupInfo=BackupInfo;
        Args.TheConfigurationVariableHandler=ConfigurationHandler;
        Args.TheReplyBuffer=TheInputPars.TheReplyBuffer;
        Args.TheReplyBufferLength=TheInputPars.TheReplyBufferLength;
        Args.TheSQLSession=TheInputPars.TheSQLSession;
        Args.TheKernelCommand=CopyOfKernelCommand;
        Args.ThePageSize=TheInputPars.BytesPerPage;

        TheNewBackupController=new tcn35b_DBMExternalBackupController(Args);

        if(0==TheNewBackupController)
        {
            rc=ERR_MEM_CN00;
            ErrorHandler->Report(tcn35d_DBBackupError::OutOfMemory.Set());
        }
    }

    if(OK_CN00!=rc && 0==TheNewBackupController)
    {
        if(0!=LogWriter)
            delete LogWriter;

        if(0!=ErrorHandler)
            delete ErrorHandler;

        if(0!=BackupTool)
            delete BackupTool;

        if(0!=BackupInfo)
            delete BackupInfo;

        if(0!=ConfigurationHandler)
            delete ConfigurationHandler;

        cn36_StrDealloc(CopyOfKernelCommand);
    }

    return rc;
}

int tcn35b_DBMExternalBackupControllerFactory::DataTypeForDBMBackupType(tcn35d_BackupInfo::ADataType          & DataType,
                                                                        const char                            * DBMBackupType,
                                                                        int                                     WithoutCheckpoint,
                                                                        tcn35d_DBBackupControllerErrorHandler & ErrorHandler)
{
    int rc=1;

    if(0==cn36_StrUprCmp(DBMBackupType, BA_KEY_DATA_CN003))
    {
        if(WithoutCheckpoint)
            DataType=tcn35d_BackupInfo::DataRecovery;
        else
            DataType=tcn35d_BackupInfo::DataMigration;
    }
    else
        if(0==cn36_StrUprCmp(DBMBackupType, BA_KEY_PAGES_CN003))
        {
            if(WithoutCheckpoint)
                DataType=tcn35d_BackupInfo::PagesRecovery;
            else
                DataType=tcn35d_BackupInfo::PagesMigration;
        }
        else
            if(0==cn36_StrUprCmp(DBMBackupType, BA_KEY_LOG_CN003))
                DataType=tcn35d_BackupInfo::Log;
            else
            {
                rc=0;
                ErrorHandler.Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
            }

    return rc;
}
