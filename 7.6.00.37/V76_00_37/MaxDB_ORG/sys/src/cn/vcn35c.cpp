/*!
  \file    vcn35c.cpp
  \author  TiloH
  \ingroup cn layer
  \brief   implementing class for controling backups of staging area files
           to an external backup tool
  \see

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2001-2006 SAP AG

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

#include "hcn35c.h"
#include "hcn36.h"
#include "hcn42.h"
#include "hcn51.h"
#include "hcn90.h"
#include "DBM/Srv/BackupHistory/DBMSrvBHist_History.hpp"
#include "DBM/Srv/Backup/Label/DBMSrvBLabel_Label.hpp"
#include "DBM/Srv/Backup/Media/DBMSrvBMedia_MediaGroup.hpp"
#include "DBM/Srv/HSS/DBMSrvHSS_Nodes.hpp"


// -----------------------------------------------------------------------------
// class StageFileInfo
// -----------------------------------------------------------------------------

class StageFileInfo
{
  public:
    StageFileInfo(const char * AbsoluteNameOfStageFile,
                  const char * Version,
                  size_t       VersionLength,
                  size_t       blockSize);
    ~StageFileInfo();

    int          DetermineDateTimeFileSize();
    bool         determineBackupLabel(
        char * replyData,
        int  & replyLen,
        int    replyLenMax);
    int          SetInternalBackupID(const char * Key, const char * Label);
    int          SetEBID(const char * NewEBID);

    const char * GiveNameOfStageFile() const;
    const char * GiveBaseNameOfStageFile() const;
    const char * GiveInternalBackupID() const;
    const char * GiveEBID() const;

    teo00_Uint8  GiveFileSize() const;
    bool         containsAtLeast2Blocks() const;
    const tcn36_DateTimeString & getLastModificationDateTime() const;
    char *       getLastModificationDateTimeAsString() const;
    char *       getStartDateTimeAsString() const;
    int          GiveVersionNumber() const;

    const DBMSrvBLabel_Label & getBackupLabel() const;

    int IsGood();
    int InternalBackupIDIsSet();

    static const char * DefaultInternalBackupID;

  private:
    char * NameOfStageFile;
    char * BaseName;
    char * InternalBackupID;
    char * EBID;
    teo00_Uint8 FileSize;
    tcn36_DateTimeString m_LastModificationDateTime;
    size_t               m_BlockSize;
    DBMSrvBLabel_Label m_Label;
    int    VersionNumber;

    static const tcn36_DateTimeFormat HistoryDateTimeFormat;
    static const tcn36_DateTimeFormat FileDateTimeFormat;
};

const tcn36_DateTimeFormat StageFileInfo::HistoryDateTimeFormat("yyyy-mm-dd HH:MM:SS");
const tcn36_DateTimeFormat StageFileInfo::FileDateTimeFormat("yyyymmddHHMMSS");

const char * StageFileInfo::DefaultInternalBackupID="UNKNOWN*****|LOG_*****";

StageFileInfo::StageFileInfo(const char * AbsoluteNameOfStageFile,
                             const char * Version,
                             size_t       VersionLength,
                             size_t       blockSize)
  :NameOfStageFile(0),
   BaseName(0),
   InternalBackupID(0),
   EBID(0),
   FileSize(0),
   m_LastModificationDateTime(0),
   m_BlockSize(blockSize),
   m_Label(),
   VersionNumber(0)
{
    size_t LengthAbsoluteName=strlen(AbsoluteNameOfStageFile);  //absolute Name without version suffix

    if(cn36_StrAlloc(NameOfStageFile, LengthAbsoluteName+VersionLength))   //create a copy of the absolute stage file name including the version number
    {
        SAPDB_strcpy(NameOfStageFile, AbsoluteNameOfStageFile);
        strncpy(NameOfStageFile+LengthAbsoluteName, Version, VersionLength);
        NameOfStageFile[LengthAbsoluteName+VersionLength]='\0';

        BaseName=cn36_BaseNameFrom(NameOfStageFile);

        cn36_StrNToInt(VersionNumber, Version, VersionLength);
    
        cn36_StrAllocCpy(InternalBackupID, DefaultInternalBackupID);
    }
}

StageFileInfo::~StageFileInfo()
{
    cn36_StrDealloc(NameOfStageFile);
    cn36_StrDealloc(InternalBackupID);
    cn36_StrDealloc(EBID);
}

int StageFileInfo::SetEBID(const char * NewEBID)
{
    return cn36_StrAllocCpy(EBID, NewEBID);
}

const char * StageFileInfo::GiveNameOfStageFile() const
{
    return NameOfStageFile;
}

const char * StageFileInfo::GiveBaseNameOfStageFile() const
{
    return BaseName;
}

const char * StageFileInfo::GiveInternalBackupID() const
{
    return InternalBackupID;
}

const char * StageFileInfo::GiveEBID() const
{
    return EBID;
}

int StageFileInfo::DetermineDateTimeFileSize()
{
    int  rc=1;
    char Date[9];
    char Time[9];

    tni34_AVirtualFile StageFile(NameOfStageFile, tni34_AVirtualFile::FromDisk, tni34_AVirtualFile::DoNotOpenNow);

    if(!StageFile.IsGood() || !StageFile.GetDateTimeSize(Date, Time, FileSize))   //determine size of all stage files
        rc=0;
    else
    {
        char H[16];

        strncpy(H, Date, 8);
        strncpy(H+8, Time, 6);
        H[15]='\0';

        m_LastModificationDateTime.SetTo(H, FileDateTimeFormat);
    }

    return rc;
}

bool StageFileInfo::determineBackupLabel(
    char * replyData,
    int  & replyLen,
    int    replyLenMax)
{
    replyData[0]='\0';
    replyLen=0;

    return m_Label.readLabelFrom(NameOfStageFile, m_BlockSize, DBMSrvBMedia_DeviceType::File, replyData, &replyLen, replyLenMax);
}

int StageFileInfo::SetInternalBackupID(const char * Key, const char * Label)
{
    int rc=1;

    if(cn36_StrAlloc(InternalBackupID, strlen(Key)+1+strlen(Label)))
    {
        SAPDB_strcpy(InternalBackupID, Key);
        InternalBackupID[strlen(Key)]='|';
        SAPDB_strcpy(InternalBackupID+strlen(Key)+1, Label);
    }
    else
        rc=0;

    return rc;
}

teo00_Uint8 StageFileInfo::GiveFileSize() const
{
    return FileSize;
}

bool StageFileInfo::containsAtLeast2Blocks() const
{
    return FileSize>=2*m_BlockSize; //file contains RTE block and info block containing the label
}

int StageFileInfo::IsGood()
{
    return (0!=NameOfStageFile && 0!=InternalBackupID);
}

const tcn36_DateTimeString & StageFileInfo::getLastModificationDateTime() const
{
    return m_LastModificationDateTime;
}

char * StageFileInfo::getLastModificationDateTimeAsString() const
{
    return m_LastModificationDateTime.CreateDateTimeString(HistoryDateTimeFormat);
}

char * StageFileInfo::getStartDateTimeAsString() const
{
    tcn36_DateTimeString startTime;

    if(m_Label.getStartDateTime(startTime))
        return startTime.CreateDateTimeString(HistoryDateTimeFormat);
    else
        return 0;
}

int StageFileInfo::GiveVersionNumber() const
{
    return VersionNumber;
}

const DBMSrvBLabel_Label & StageFileInfo::getBackupLabel() const
{
    return m_Label;
}

int StageFileInfo::InternalBackupIDIsSet()
{
    return (0!=strcmp(InternalBackupID, StageFileInfo::DefaultInternalBackupID));
}


// -----------------------------------------------------------------------------
// member functions cn35c_SuffixList
// -----------------------------------------------------------------------------

int cn35c_SuffixList::FillList(const char * SuffixListAsString,
                               size_t       SuffixListLength)
{
    int rc=1;

    if(0!=SuffixListAsString)
    {
        const char * CurrentPos=SuffixListAsString;
        size_t       CurrentLength=SuffixListLength;
        const char * NextComma;
        const char * NextMinus;
        int          Value1,
                     Value2,
                     SomethingToAdd;

        while(rc && 0<CurrentLength) //while all ok and not processed to many characters
        {
            SomethingToAdd=1;

            NextMinus=cn36_StrNChr(CurrentPos, '-', CurrentLength); //find next minus

            if(0==NextMinus)                                        //if nothing found 
                NextMinus=SuffixListAsString+SuffixListLength;      //let it point to the end of the string

            NextComma=cn36_StrNChr(CurrentPos, ',', CurrentLength); //same for next comma

            if(0==NextComma)
                NextComma=SuffixListAsString+SuffixListLength;

            if(NextMinus<NextComma)                                 //if there is a minus in front of the comma
            {
                if(cn36_StrNToInt(Value1, CurrentPos, NextMinus-CurrentPos) &&
                   cn36_StrNToInt(Value2, NextMinus+1, NextComma-(NextMinus+1)))    // 10-, will be equivalent to 10-0, == 0-10,
                {
                    if(Value2<Value1)   //be sure that Value1<=Value2
                    {
                        int h=Value2;
                        Value2=Value1;
                        Value1=h;
                    }
                }
                else
                    SomethingToAdd=0;   //don't add somthing to the list
            }
            else
            {
                if(cn36_StrNToInt(Value1, CurrentPos, NextComma-CurrentPos))
                    Value2=Value1;      //add just that one value
                else
                    SomethingToAdd=0;   //don't add somthing to the list
            }

            for(Value1; SomethingToAdd && rc && Value1<=Value2; Value1++)
            {
                if(0==List.Add(Value1))
                {
                    rc=0;
                    ErrorHandler_ni35->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseOutOfMemory));
                }
            }

            CurrentLength-=(NextComma-CurrentPos);

            if(0<CurrentLength)
            {
                CurrentLength--;
                CurrentPos=NextComma+1;
            }
        }

        List.Reverse();
    }

    return rc;
}

const ni31_ListElem<int> * cn35c_SuffixList::First()
{
    return List.First();
}


// -----------------------------------------------------------------------------
// member functions tcn35c_DBMStageBackupController
// -----------------------------------------------------------------------------

tcn35c_DBMStageBackupController::tcn35c_DBMStageBackupController(tcn35_BackupToolConnector              * TheBackupToolConnector,
                                                                 tcn35e_DBMBackupControllerErrorHandler * TheErrorHandler,
                                                                 tcn35e_DBMBackupControllerLogWriter    * TheLogWriter,
                                                                 tcn35d_BackupInfo                      * TheBackupInfo,
                                                                 tcn35e_ConfigurationVariableHandler    * TheConfigurationVariableHandler,
                                                                 char                                   * TheReplyData,
                                                                 int                                    * TheReplyLen,
                                                                 const tcn003_Medium                    * TheStage,
                                                                 size_t                                   bytesPerBlockOnStage,
                                                                 int                                      WithVerify,
                                                                 int                                      RemoveStageFiles,
                                                                 const char                             * SuffixListStart,
                                                                 size_t                                   SuffixListLength)
    :tcn35a_DBBackupController(TheBackupToolConnector, TheErrorHandler, TheLogWriter, TheBackupInfo, TheConfigurationVariableHandler),
    ConfigurationVariableHandler(TheConfigurationVariableHandler),
    DBMLogWriter(TheLogWriter),
    ReplyData(TheReplyData),
    ReplyLen(TheReplyLen),
    Stage(*TheStage),
    m_BytesPerBlockOnStage(bytesPerBlockOnStage),
    ReuseStageFiles(0),
    NumberOfProcessedStageFiles(0),
    NumberOfDeletedStageFiles(0),
    NumberOfUnprocessedStageFiles(0),
    BytesProcessed(0),
    CurrentStageFile(0),
    Pipe2FileOutput(0),
    Pipe2FileError(0),
    Pipe2FileProcess(0),
    Pipe2FileState(NotStarted),
    RemoveStageFilesFlag(RemoveStageFiles),
    VerifyStageFilesFlag(WithVerify),
    ErrorHandlerAsDBMErrorHandler(TheErrorHandler),
    SuffixesWereWished(0)
{
    if(0!=SuffixListStart && 0<SuffixListLength)
    {
        SuffixesWereWished=1;
        ListOfWishedSuffixes.FillList(SuffixListStart, SuffixListLength);
    }
}

tcn35c_DBMStageBackupController::~tcn35c_DBMStageBackupController()
{
    while(0!=StageFiles.First())    //free memeory used by StageFiles
    {
        delete StageFiles.First()->GiveInfo();  //free the content the element is pointing to
        StageFiles.Del();                       //free the list element itself
    }

    if(0!=Pipe2FileOutput)
        delete Pipe2FileOutput;

    if(0!=Pipe2FileError)
        delete Pipe2FileError;

    if(0!=Pipe2FileProcess)
        delete Pipe2FileProcess;

    cn36_StrDealloc(BytesProcessed);
}

void tcn35c_DBMStageBackupController::BeginBackupOperation()
{
    int ToolIsInstalled=0;

    ErrorHandler->SetAsCurrentErrorHandler();
    ErrorHandler->SetPhaseTo(tcn35d_DBBackupControllerErrorHandler::ToolCheck);

    if(BackupToolConnector->CheckToolInstallation(ToolIsInstalled) && ToolIsInstalled)
    {
        int MediumIsOK;

        ErrorHandler->SetPhaseTo(tcn35d_DBBackupControllerErrorHandler::MediumCheck);

        LogWriter->Write("Checking medium.\n", tcn35d_DBBackupControllerLogWriter::AddALevelToPrefix);
        MediumIsOK=(CheckPipes() && BackupToolConnector->CheckPipes());   //pipes are ok for db and tool
        LogWriter->Write(MediumIsOK?"Check passed successfully.\n":"Check failed.\n", tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefixAndEndParagraph);

        if(MediumIsOK)
        {
            int AllOk=1;

            ErrorHandler->SetPhaseTo(tcn35d_DBBackupControllerErrorHandler::PrepareOperation);

            const char * PrepareBackupRestoreString="Preparing stage backup.\n";

            LogWriter->Write(PrepareBackupRestoreString, tcn35d_DBBackupControllerLogWriter::AddALevelToPrefix);

            AllOk=Prepare();

            LogWriter->Write(AllOk?"Prepare passed successfully.\n":"Prepare failed.\n", tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefixAndEndParagraph);

            if(AnErrorOccured() || !AllOk)  // if something went wrong, stop everything and clean up the allocated resources (pipes, files, ...)
                EndBackupOperation();
        }
    }
}

int tcn35c_DBMStageBackupController::StartBackupOfNextStageFile(int & IsFirstFile, tcn35d_BackupInfo::ADataDirection DataDirection)
{
    int rc=1;

    ErrorHandler->SetPhaseTo(tcn35d_DBBackupControllerErrorHandler::PrepareOperation);
        
    LogWriter->Write("Preparing backup tool.\n", tcn35d_DBBackupControllerLogWriter::AddALevelToPrefix);
    rc=BackupToolConnector->Prepare(IsFirstFile);
    LogWriter->Write(rc?"Prepare passed successfully.\n":"Prepare failed.\n", tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefixAndEndParagraph);
    
    if(IsFirstFile)
        IsFirstFile=0;

    if(rc)
    {
        rc=0;
        ErrorHandler->SetPhaseTo(tcn35e_DBMBackupControllerErrorHandler::DBRequest);

        CurrentWaitingPhase=NoWaiting;  //we start a completely new backup

        if(StartDBAction())  // start the tool only, when Request was send
        {
            StatusOfRequest=RequestSend;

            if(DBIsReady())
            {
                ErrorHandler->SetPhaseTo(tcn35d_DBBackupControllerErrorHandler::ToolRequest);

                if(BackupToolConnector->StartTool())
                {
                    StatusOfTool=Started;
                    BackupToolConnector->CheckTool();

                    rc=1;    // only if we get here, we have started the db request and the backup tool correctly
                }
            }
            else
                StatusOfRequest=RequestReplied;
        }
        else
            StatusOfRequest=NoRequestSend;
    }

    return rc;
}

int tcn35c_DBMStageBackupController::BackupOrVerifyStageFiles(tcn35d_BackupInfo::ADataDirection DataDirection)
{
    int rc=(!AnErrorOccured());

    if(DataDirection==tcn35d_BackupInfo::ABackup || DataDirection==tcn35d_BackupInfo::ARestore)
    {
        int IsBackup=(DataDirection==tcn35d_BackupInfo::ABackup),
            IsFirstFile=1;

        if(IsBackup)
            BackupInfo->SetDataDirection(tcn35d_BackupInfo::ABackup);
        else
            BackupInfo->SetDataDirection(tcn35d_BackupInfo::ARestore);

        while(rc && 0!=CurrentStageFile)
        {
            if(!IsBackup)
                rc=BackupToolConnector->SetExternalBackupIDList(CurrentStageFile->GiveInfo()->GiveEBID());

            if(rc)
            {
                sprintf(MsgBuf_cn36, IsBackup?"Backing up stage file '%s'.\n":"Verifying stage file '%s'.\n", CurrentStageFile->GiveInfo()->GiveNameOfStageFile());
                LogWriter->Write(MsgBuf_cn36, tcn35d_DBBackupControllerLogWriter::AddALevelToPrefix);

                if(!CreatePipes())
                    rc=0;
                else
                {
                    StartBackupOfNextStageFile(IsFirstFile, DataDirection);

                    LogWriter->Write(IsBackup?"Waiting for end of the backup operation.\n":"Waiting for end of the verify.\n", tcn35d_DBBackupControllerLogWriter::AddALevelToPrefix);

                    while(BackupOperationIsRunning())
                    {
                        Sleep(1, false);
                    }

                    LogWriter->Write(IsBackup?"The backup operation has ended.\n":"The verify has ended.\n", tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefixAndEndParagraph);

                    CheckForErrors();

                    ErrorHandler->SetPhaseTo(tcn35d_DBBackupControllerErrorHandler::PostOperation);

                    if(!AnErrorOccured() && IsBackup)  //don't write the history during verify
                        WriteHistoryFile();

                    if(!AnErrorOccured())            // no error reported so far -> do all necessary post-operations
                    {
                        ErrorHandler->SetPhaseTo(tcn35d_DBBackupControllerErrorHandler::PostOperation); //set possible errorcode back to ERR_POSTOP_CN00
                        BackupToolConnector->PostOperationsAfterSuccess();                              //currently used to backup history files with Backint for MaxDB
                    }
                }

                CleanUp();

                if(IsBackup)
                    sprintf(MsgBuf_cn36, "The backup of stage file '%s' was %ssuccessful.\n", CurrentStageFile->GiveInfo()->GiveNameOfStageFile(), AnErrorOccured()?"un":"");
                else
                    sprintf(MsgBuf_cn36, "The stage file '%s' could %sbe verified successfully.\n", CurrentStageFile->GiveInfo()->GiveNameOfStageFile(), AnErrorOccured()?"not ":"");

                LogWriter->Write(MsgBuf_cn36, tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefixAndEndParagraph);
                 
                CurrentStageFile=CurrentStageFile->Next();

                if(rc)
                    rc=(!AnErrorOccured());
            }
        }
    }
    else
    {
        rc=0;
        ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
    }

    return rc;
}

void tcn35c_DBMStageBackupController::EndBackupOperation()
{
    ErrorHandler->SetAsCurrentErrorHandler();

    // don't have to check that Backup is/was running. BackupOperationIsRunning() reports error if necessary
    if(!AnErrorOccured())   //if no problems were encountered and reported during preparation
    {
        if(0==StageFiles.First())
        {
            LogWriter->Write("Nothing to do, no stage files found.\n", tcn35d_DBBackupControllerLogWriter::EndParagraph);
            PrintBackupAnswer(1);    //nothing to do, just report an OK
        }
        else
        {
            BackupOrVerifyStageFiles(tcn35d_BackupInfo::ABackup);

            if(!AnErrorOccured() && VerifyStageFilesFlag)
            {
                CurrentStageFile=StageFiles.First();
                BackupOrVerifyStageFiles(tcn35d_BackupInfo::ARestore);  //ARestore -> we will restore the files and verify them
            }

            if(!AnErrorOccured())
                RemoveStageFiles(); //removes stage files if

            CleanUpFinally();

            ErrorHandler->SetPhaseTo(tcn35d_DBBackupControllerErrorHandler::BackupOperation);   //reports of real backup/restore errors are constructed here -> set the correct error type
            FillReplyBuffer();
        }
    }
}

tcn00_Error tcn35c_DBMStageBackupController::RecycleForRepeat(char                               *  TheReplyBuffer, 
                                                              int                                *  TheReplyBufferLength,
                                                              const tcn003_Medium                *  Medium,
                                                              const tcn003_MediaGroup            *  MediaGroup,
                                                              int                                   WithVerify,
                                                              int                                   RemoveStageFiles)
{
    tcn00_Error rc=OK_CN00;

    ReuseStageFiles=1; //use the list StageFile from the last try
    NumberOfDeletedStageFiles=0;

    ReplyData=TheReplyBuffer;
    ReplyLen=TheReplyBufferLength;

    ErrorHandlerAsDBMErrorHandler->SetReplyArrayTo(TheReplyBuffer, TheReplyBufferLength);


    const char * MediumName;
    int          NumberOfPipes;
    char       * ThePipeNames[MAX_NUMBER_OF_DATAPIPES_CN35D];
    char       * TheMediumTypes[MAX_NUMBER_OF_DATAPIPES_CN35D];
    long         BytesPerBlock;
    long         PagesPerBlock;
    long         BytesPerPage;
    long         CountInPages[MAX_NUMBER_OF_DATAPIPES_CN35D];

    if(!TheDBMStageBackupControllerFactory_cn35c.MediumInfo(MediumName,
                                                            NumberOfPipes,
                                                            ThePipeNames,
                                                            TheMediumTypes,
                                                            BytesPerBlock,
                                                            PagesPerBlock,
                                                            BytesPerPage,
                                                            CountInPages,
                                                            BackupInfo->DBName(),
                                                            Medium,
                                                            MediaGroup,
                                                            *ErrorHandlerAsDBMErrorHandler))
    {
        rc=ErrorHandlerAsDBMErrorHandler->GiveLastError();
    }

    if(!BackupInfo->SetMedium(tcn35d_BackupInfo::ABackup, //we start everytime with the backup of stage files
                              MediumName,
                              NumberOfPipes,
                              ThePipeNames,
                              TheMediumTypes,
                              BytesPerBlock,
                              PagesPerBlock,
                              CountInPages))
    {
        rc=ErrorHandlerAsDBMErrorHandler->GiveLastError();
    }

    RemoveStageFilesFlag=RemoveStageFiles;
    VerifyStageFilesFlag=WithVerify;

    ConfigurationVariableHandler->ReinitConfigVarHandler(); //assure that all environment variables are reread from dbm.cfg
    DBMLogWriter->ReinitLogWriter();

    //recreate BackupToolConnector
    if(0!=BackupToolConnector)
    {
        delete BackupToolConnector;
        BackupToolConnector=0;
    }

    if(!tcn35_BackupToolConnector::ConstructBackupToolConnector(BackupToolConnector,
                                                                DBMSrvBMedia_MediaGroup::BackupTool(Medium, MediaGroup),
                                                                BackupInfo,
                                                                LogWriter,
                                                                ErrorHandler,
                                                                ConfigurationVariableHandler,
                                                                0))
    {
        rc=ErrorHandlerAsDBMErrorHandler->GiveLastError();
    }

    return rc;
}

tcn00_Error tcn35c_DBMStageBackupController::GiveLastError()
{
    return ((tcn35e_DBMBackupControllerErrorHandler*)ErrorHandler)->GiveLastError();
}

int tcn35c_DBMStageBackupController::GiveSQLResult()
{
    int rc=ERR_CN00;

    if(OK_CN00==GiveLastError())
        rc=OK_CN00;

    return rc;
}

int tcn35c_DBMStageBackupController::Prepare()
{
    int rc=1;

    //export environment variables configured in dbm.cfg
    const ni31_ListElem<tcn35e_ConfigVariable *> *ListElem;

    for(ListElem=ConfigurationVariableHandler->ConfigurationVariableToExportList().First(); rc && 0!=ListElem; ListElem=ListElem->Next())
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

    //determine the files to backup
    if(rc)
    {
        if(ReuseStageFiles)
        {
            for(CurrentStageFile=StageFiles.First(); rc && 0!=CurrentStageFile; CurrentStageFile=CurrentStageFile->Next())
            {
                tni34_AVirtualFile StageFile(CurrentStageFile->GiveInfo()->GiveNameOfStageFile(), tni34_AVirtualFile::FromDisk, tni34_AVirtualFile::DoNotOpenNow);

                if(!StageFile.IsGood())
                    rc=0;

                if(rc && !StageFile.Exists())
                {
                    rc=0;
                    ErrorHandler->Report(tcn35e_DBMBackupError(tcn35e_DBMBackupError::CaseStageFileMissing, StageFile.GiveName()));
                }

                if(rc)
                {
                    char        Date[9];
                    char        Time[9];

                    teo00_Uint8 FileSize;

                    if(!StageFile.GetDateTimeSize(Date, Time, FileSize)) //determine size, date and time of the current file
                        rc=0;
                    else
                    {
                        char DateTimeString[17];

                        SAPDB_strcpy(DateTimeString, Date);
                        SAPDB_strcpy(DateTimeString+8, Time);

                        tcn36_DateTimeString DateTime(DateTimeString, "yyyymmddHHMMSS");

                        if(FileSize!=CurrentStageFile->GiveInfo()->GiveFileSize())
                        {
                            rc=0;
                            ErrorHandler->Report(tcn35e_DBMBackupError(tcn35e_DBMBackupError::CaseStageChangedInSize, StageFile.GiveName()));
                        }

                        if(rc && DateTime!=CurrentStageFile->GiveInfo()->getLastModificationDateTime())
                        {
                            rc=0;
                            ErrorHandler->Report(tcn35e_DBMBackupError(tcn35e_DBMBackupError::CaseStageChangedInDateTime, StageFile.GiveName()));
                        }

                        if(rc)
                        {
                            DBMSrvBLabel_Label m_Label;
                            char reply[1025];
                            int  replyLength=0;
                            int  maxReplyLength=1024;

                            if(m_Label.readLabelFrom(CurrentStageFile->GiveInfo()->GiveNameOfStageFile(), m_BytesPerBlockOnStage, DBMSrvBMedia_DeviceType::File, reply, &replyLength, maxReplyLength))   //determine label of file
                            {
                                if(!m_Label.equals(CurrentStageFile->GiveInfo()->getBackupLabel()))
                                {
                                    rc=0;
                                    ErrorHandler->Report(tcn35e_DBMBackupError(tcn35e_DBMBackupError::CaseStageChangedInLabel, CurrentStageFile->GiveInfo()->GiveNameOfStageFile()));

                                    sprintf(MsgBuf_cn36, "Label of stage file '%s' changed from %s to %s.\n",
                                        CurrentStageFile->GiveInfo()->GiveNameOfStageFile(),
                                        CurrentStageFile->GiveInfo()->getBackupLabel().getLabel(),
                                        m_Label.getLabel());
                                    LogWriter->Write(MsgBuf_cn36);
                                }
                            }
                            else
                            {
                                rc=0;
                                ErrorHandler->Report(tcn35e_DBMBackupError(tcn35e_DBMBackupError::CaseCanNotReadLabel, CurrentStageFile->GiveInfo()->GiveNameOfStageFile()));
                            }
                        }
                    }
                }
            }
        }
        else
        {
            DBMSrvBHist_History        BackupHistory;
            ni31_List<StageFileInfo *> ListOfFilesFound;
            unsigned long              NumberOfPossibleFilesFound=0;

            char * NameOfStageDir=0;
            char * AbsoluteNameOfStageFile=0;

            //read the backup histories
            LogWriter->Write("Reading backup histories.\n", tcn35e_DBMBackupControllerLogWriter::AddALevelToPrefix);
            tcn00_Error FuncStatus=BackupHistory.ReadBasicHistories(BackupInfo->DBName(), ReplyData, ReplyLen);
            sprintf(MsgBuf_cn36, "Finished reading backup histories %ssuccessfully.\n", OK_CN00==FuncStatus?"":"un");
            LogWriter->Write(MsgBuf_cn36, tcn35e_DBMBackupControllerLogWriter::SubALevelFromPrefixAndEndParagraph);

            if(OK_CN00==FuncStatus ||                               //either it's all OK
               (SuffixesWereWished && ERR_MEM_CN00!=FuncStatus))    //or  suffixes were wished and backup history is not needed at all costs (then ERR_MEM_CN00 is the only real error, other errors are "just" open/read errors on the files)
            {
                if(cn36_StrNAllocCpy(NameOfStageDir, Stage.szLocation, cn36_EndOfDirectory(Stage.szLocation)-Stage.szLocation) &&
                   cn36_StrAllocCpy(AbsoluteNameOfStageFile, Stage.szLocation))
                {
                    if('\0'==(*NameOfStageDir) ||               //if we have no stage directory
                       !cn36_IsAbsolutePath(NameOfStageDir))    //or it's just relative to the rundirectory
                    {
                        char * NewNameOfStageDir=0;

                        if(cn36_ConstructCompletePath(NewNameOfStageDir, BackupInfo->DefaultDirectory(), NameOfStageDir))               //construct the absolut stage directory
                        {
                            cn36_StrDealloc(NameOfStageDir);
                            NameOfStageDir=NewNameOfStageDir;
                        }
                        else
                            rc=0;

                        if(rc)
                            rc=cn36_ConstructCompletePath(AbsoluteNameOfStageFile, BackupInfo->DefaultDirectory(), Stage.szLocation);   //and the absolut stage location
                    }

                    if(rc)
                    {
                        sprintf(MsgBuf_cn36, "Determining stage files in directory '%s'.\n", NameOfStageDir);
                        LogWriter->Write(MsgBuf_cn36, tcn35e_DBMBackupControllerLogWriter::AddALevelToPrefix);

                        rc=DetermineFilesInDirectory(ListOfFilesFound,
                                                     NameOfStageDir,
                                                     cn36_BaseNameFrom(Stage.szLocation),
                                                     AbsoluteNameOfStageFile);

                        LogWriter->Write(tcn35d_DBBackupControllerLogWriter::EndParagraph);

                        if(rc)
                            rc=determineLabels(ListOfFilesFound);

                        LogWriter->Write(tcn35d_DBBackupControllerLogWriter::EndParagraph);

                        if(rc)
                        {
                            NumberOfPossibleFilesFound=ListOfFilesFound.Length();

                            if(0>=NumberOfPossibleFilesFound)
                                LogWriter->Write("Did not find any stage files.\n", tcn35d_DBBackupControllerLogWriter::EndParagraph);
                            else
                            {
                                sprintf(MsgBuf_cn36, "Found %lu stage file%s.\n", (unsigned long)NumberOfPossibleFilesFound, NumberOfPossibleFilesFound==1?"":"s");
                                LogWriter->Write(MsgBuf_cn36, tcn35d_DBBackupControllerLogWriter::EndParagraph);
                            }
                        }
                    }
                }
                else
                    rc=0;
            }
            else
                rc=0;   //error already reported by ReadHistories()

            if(rc)  //compare backup history and backup media history with the files found on the stage
            {
                ni31_ListElem<StageFileInfo *> * FileListElem;

                BackupHistory.ReduceToFileLogBackupsWithName(AbsoluteNameOfStageFile, BackupInfo->DefaultDirectory());

                for(FileListElem=ListOfFilesFound.First(); rc && 0!=FileListElem /*&& 0==FileFoundAndAdded*/; FileListElem=FileListElem->Next())    //for all found files
                {
                    const char * BaseNameInDirectory=FileListElem->GiveInfo()->GiveBaseNameOfStageFile();
                    const char * Key;
                    const char * Label;
                    tcn36_DateTimeString startTimeBackupLabel;
                    const char * backupLabel=FileListElem->GiveInfo()->getBackupLabel().getLabel();

                    FileListElem->GiveInfo()->getBackupLabel().getStartDateTime(startTimeBackupLabel);

                    if(BackupHistory.ContainsFileBackup(
                        BaseNameInDirectory,
                        backupLabel,
                        startTimeBackupLabel,
                        Key,
                        Label))
                    {
                        if(!FileListElem->GiveInfo()->SetInternalBackupID(Key, Label))
                            rc=0;

                        if(rc && 0>=FileListElem->GiveInfo()->GiveFileSize())  //file size should be greater than 0
                        {
                            rc=0;
                            ErrorHandler->Report(tcn35e_DBMBackupError(tcn35e_DBMBackupError::CaseIllegalStageFileSize,
                                                                       int(FileListElem->GiveInfo()->GiveFileSize()),
                                                                       FileListElem->GiveInfo()->GiveNameOfStageFile()));
                        }

                        if(rc)
                        {
                            char SizeAsString[50];

                            if(!cn36_UInt8ToStrN(SizeAsString, 50, FileListElem->GiveInfo()->GiveFileSize()))
                            {
                                rc=0;
                                ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
                            }
                            else
                            {
                                char * DateTime=FileListElem->GiveInfo()->getStartDateTimeAsString();

                                sprintf(MsgBuf_cn36,
                                        "Stage file '%s' created at '%s' with a size of %s bytes will be processed.\n",
                                        FileListElem->GiveInfo()->GiveNameOfStageFile(),
                                        0==DateTime?"":DateTime,
                                        SizeAsString);

                                LogWriter->Write(MsgBuf_cn36);

                                cn36_StrDealloc(DateTime);
                            }
                        }
                    }
                    else
                    {
                        char * DateTime=FileListElem->GiveInfo()->getStartDateTimeAsString();
                        char * DateTimeModified=FileListElem->GiveInfo()->getLastModificationDateTimeAsString();

                        sprintf(
                            MsgBuf_cn36,
                            "Stage file '%s' created at '%s' and lastly modified at '%s' will not be processed, as it can not be found in the backup history.\n",
                            FileListElem->GiveInfo()->GiveNameOfStageFile(),
                            0==DateTime?"":DateTime,
                            0==DateTimeModified?"":DateTimeModified);

                        LogWriter->Write(MsgBuf_cn36);

                        cn36_StrDealloc(DateTime);
                        cn36_StrDealloc(DateTimeModified);
                    }
                }

                LogWriter->Write(tcn35d_DBBackupControllerLogWriter::EndParagraph);

                //check if all stages (maybe without the last) were found in the backup history
                int AllFilesFound=1;
                int NumberOfFilesWithOutInfo=0;
                tcn36_DateTimeString MaxSoFar(0);
                int                  MaxVersionSoFar=-1;
                ni31_ListElem<StageFileInfo *> * MaxFileListElem=0;
                ni31_ListElem<StageFileInfo *> * LastUnset=0;
                teo00_Uint8                      TotalSize=0;
 
                if(!SuffixesWereWished)
                {
                    for(FileListElem=ListOfFilesFound.First(); rc && 0!=FileListElem; FileListElem=FileListElem->Next())
                    {
                        if(!FileListElem->GiveInfo()->InternalBackupIDIsSet()) //stage file was not found in history
                        {
                            NumberOfFilesWithOutInfo++;
                            LastUnset=FileListElem;
                        }

                        if(FileListElem->GiveInfo()->getLastModificationDateTime()>MaxSoFar ||             //we are looking for the newest stage file 
                           (FileListElem->GiveInfo()->getLastModificationDateTime()==MaxSoFar &&           //or if more than one is the newest
                            FileListElem->GiveInfo()->GiveVersionNumber()>MaxVersionSoFar)) //we look for that one with the highest version number
                        {
                            MaxSoFar=FileListElem->GiveInfo()->getLastModificationDateTime();
                            MaxVersionSoFar=FileListElem->GiveInfo()->GiveVersionNumber();
                            MaxFileListElem=FileListElem;
                        }
                    }
                }

                NumberOfUnprocessedStageFiles=NumberOfFilesWithOutInfo;

                if(0==NumberOfFilesWithOutInfo ||
                   (1==NumberOfFilesWithOutInfo && MaxFileListElem==LastUnset))
                {
                    while(rc && 0!=ListOfFilesFound.First())    //copy complete list but without LastUnset //and sorted for creationDate TODO????
                    {
                        if(LastUnset!=ListOfFilesFound.First())
                        {
                            if(!StageFiles.Add(ListOfFilesFound.First()->GiveInfo()))
                            {
                                ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseOutOfMemory));
                                rc=0;
                            }
                            else
                            {
                                NumberOfProcessedStageFiles++;
                                TotalSize+=ListOfFilesFound.First()->GiveInfo()->GiveFileSize();
                            }
                        }
                        else
                            delete ListOfFilesFound.First()->GiveInfo();

                        ListOfFilesFound.Del();
                    }

                    if(cn36_StrAlloc(BytesProcessed, 30))
                    {
                        if(!cn36_UInt8ToStrN(BytesProcessed, 30, TotalSize))
                        {
                            ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
                            rc=0;
                        }
                    }
                    else
                        rc=0;
                }
                else
                {
                    ErrorHandler->Report(tcn35e_DBMBackupError(tcn35e_DBMBackupError::CaseInsufficientHisoryData, LastUnset->GiveInfo()->GiveNameOfStageFile()));
                    rc=0;
                }
       
                if(rc && 0==StageFiles.Length() && 0<NumberOfPossibleFilesFound) //there were suspected stage files, but none of them was found in the history
                    LogWriter->Write("Did not find any stage files, mentioned in the backup history.\n");

                sprintf(MsgBuf_cn36, "Finished determination of stage files %ssuccessfully.\n", rc?"":"un");
                LogWriter->Write(MsgBuf_cn36, tcn35e_DBMBackupControllerLogWriter::SubALevelFromPrefixAndEndParagraph);
            }

            while(0!=ListOfFilesFound.First())  //last chance to free whatever is left in ListofFilesFound
            {
                delete ListOfFilesFound.First()->GiveInfo();
                ListOfFilesFound.Del();
            }
        }
    }

    if(rc)
        CurrentStageFile=StageFiles.First();

    if(rc && 0!=StageFiles.First()) //all ok and something to backup
    {
        if(0!=Pipe2FileOutput)
            delete Pipe2FileOutput;

        if(0!=Pipe2FileError)
            delete Pipe2FileError;

        Pipe2FileOutput=new tni34_AFile(tni34_AFile::ToDiskTruncate, 0, 0); // do not create on open and do not open it now
        Pipe2FileError=new tni34_AFile(tni34_AFile::ToDiskTruncate, 0, 0); // do not create on open and do not open it now

        if(0==Pipe2FileOutput || 0==Pipe2FileError)
        {
            rc=0;
            ErrorHandler->Report(tcn35d_DBBackupError::OutOfMemory.Set());
        }
        else
        {
            if(Pipe2FileOutput->IsGood())
            {
                sprintf(MsgBuf_cn36, "Created temporary file '%s' as output for pipe2file.\n", Pipe2FileOutput->GiveName());
                LogWriter->Write(MsgBuf_cn36);
            }
            else
                rc=0;

            if(Pipe2FileError->IsGood())
            {
                sprintf(MsgBuf_cn36, "Created temporary file '%s' as error output for pipe2file.\n", Pipe2FileError->GiveName());
                LogWriter->Write(MsgBuf_cn36);
            }
            else
                rc=0;
        }
    }

    if(rc && 0!=StageFiles.First())  //get the name of the external backup history
    {
        if(0!=BackupInfo->PathOfExternalBackupHistory())
        {
            sprintf(MsgBuf_cn36, "Using '%s' as external backup history.\n", BackupInfo->PathOfExternalBackupHistory());
            LogWriter->Write(MsgBuf_cn36);
        }
    }

    return rc;
}

int tcn35c_DBMStageBackupController::StartDBAction()    //it's not really the DB but pipe2file with the current stage file
{
    int rc=1;

    Pipe2FileArguments.DeleteAll();
    Pipe2FileState=NotStarted;

    if(!Pipe2FileArguments.AddArgument("pipe2file") ||
       !Pipe2FileArguments.AddArgument("-d") ||
       (tcn35d_BackupInfo::ABackup==BackupInfo->DataDirection() && !Pipe2FileArguments.AddArgument("file2pipe")) ||
       (tcn35d_BackupInfo::ABackup!=BackupInfo->DataDirection() && !Pipe2FileArguments.AddArgument("compare")) ||
       !Pipe2FileArguments.AddArgument("-f") ||
       !Pipe2FileArguments.AddArgument(CurrentStageFile->GiveInfo()->GiveNameOfStageFile()) ||
       !Pipe2FileArguments.AddArgument("-p") ||
       !Pipe2FileArguments.AddArgument(BackupInfo->NameOfPipe(0)) ||
       !Pipe2FileArguments.AddArgument("-nowait"))
    {
        rc=0;
        ErrorHandler->Report(tcn35d_DBBackupError::OutOfMemory.Set());
    }
    else
    {
        if(0==Pipe2FileArguments.GiveCommandLine())
            rc=0;
        else
        {
            sprintf(MsgBuf_cn36, "Constructed pipe2file call '%s'.\n", Pipe2FileArguments.GiveCommandLine());
            LogWriter->Write(MsgBuf_cn36, tcn35d_DBBackupControllerLogWriter::EndParagraph);

            sprintf(MsgBuf_cn36,"Starting pipe2file for stage file '%s'.\n", CurrentStageFile->GiveInfo()->GiveNameOfStageFile());
            LogWriter->Write(MsgBuf_cn36, tcn35d_DBBackupControllerLogWriter::AddALevelToPrefix);

            if(0==Pipe2FileOutput || 0==Pipe2FileError)
            {
                ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
                rc=0;
            }
            else
            {
                sprintf(MsgBuf_cn36,
                        "Starting pipe2file process '%s >>%s 2>>%s'.\n",
                        Pipe2FileArguments.GiveCommandLine(),
                        Pipe2FileOutput->GiveName(),
                        Pipe2FileError->GiveName());
                
                LogWriter->Write(MsgBuf_cn36);

                Pipe2FileProcess=new tni33_ABackgroundProcess(Pipe2FileArguments, (tni34_AFile *)(0), Pipe2FileOutput, 1, Pipe2FileError, 1);    

                if(0==Pipe2FileProcess)
                {
                    ErrorHandler->Report(tcn35d_DBBackupError::OutOfMemory.Set());
                    rc=0;
                }
                else
                    if(!Pipe2FileProcess->WasStarted())
                        rc=0;
                    else
                        Pipe2FileState=Running;

                LogWriter->Write(rc?"Process was started successfully.\n":"Start of process failed.\n");
            }

            LogWriter->Write(rc?"Pipe2file has been started successfully.\n":"Could not start pipe2file correctly.\n", tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefixAndEndParagraph);
        }
    }

    return rc;
}

void tcn35c_DBMStageBackupController::CleanUp()
{
    int AllOk=1;

    ErrorHandler->SetPhaseTo(tcn35d_DBBackupControllerErrorHandler::CleanUp);

    LogWriter->Write("Cleaning up.\n", tcn35d_DBBackupControllerLogWriter::AddALevelToPrefix);
    
    if(!RemovePipes())                 // remove all created "data-transfer" pipes (must not be all pipes of medium) 
        AllOk=0;

    if(!BackupToolConnector->MoveToolOutputToProtocolFile()) // move stdout and stderr of started tool processes to the protocol (dbm.ebp)
        AllOk=0;

    if(!BackupToolConnector->CleanUpTool())                  // do tool specific cleanup
        AllOk=0;

    if(!DBActionWasSuccessful() && !MovePipe2FileOutputToLogFile())  //copy output of pipe2file to the log file only if it was unsuccessful
        AllOk=0;

    LogWriter->Write(AllOk?"Have finished clean up successfully.\n":"Could not clean up all correctly.\n",
        tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefixAndEndParagraph);
}

void tcn35c_DBMStageBackupController::CleanUpFinally()
{
    int AllOk=1;

    ErrorHandler->SetPhaseTo(tcn35d_DBBackupControllerErrorHandler::CleanUp);

    LogWriter->Write("Cleaning up.\n", tcn35d_DBBackupControllerLogWriter::AddALevelToPrefix);

    if(0!=Pipe2FileOutput)
    {
        sprintf(MsgBuf_cn36, Pipe2FileOutput->Remove()?"Removed temporary output file of pipe2file ('%s') successfully.\n":"Could not remove temporary output file of pipe2file ('%s' ).\n", Pipe2FileOutput->GiveName());
        LogWriter->Write(MsgBuf_cn36);
    }


    if(0!=Pipe2FileError)
    {
        sprintf(MsgBuf_cn36, Pipe2FileError->Remove()?"Removed temporary error output file of pipe2file ('%s') successfully.\n":"Could not remove temporary output file of pipe2file ('%s' ).\n", Pipe2FileError->GiveName());
        LogWriter->Write(MsgBuf_cn36);
    }

    LogWriter->Write(AllOk?"Have finished clean up successfully.\n":"Could not clean up all correctly.\n",
        tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefixAndEndParagraph);
}

int tcn35c_DBMStageBackupController::DBIsReady()
{
    int rc=0;

    if(Pipe2FileState==Running && Pipe2FileProcess->IsRunning())
        rc=1;

    return rc;
}

int tcn35c_DBMStageBackupController::DBReplyAvailable(bool OutputMessagesForNonEvents)
{
    int rc=0;

    if(0==Pipe2FileProcess || Pipe2FileState!=Running)
        rc=0;
    else
    {
        if(!Pipe2FileProcess->IsRunning())
        {
            rc=1;

            sprintf(MsgBuf_cn36,"The pipe2file process has finished work with return code %d.\n", int(Pipe2FileProcess->GiveReturnCode()));
            LogWriter->Write(MsgBuf_cn36, tcn35d_DBBackupControllerLogWriter::UsePrefixAndDateTime);

            Pipe2FileState=Ended;
        }
    }

    if(OutputMessagesForNonEvents)
    {
        LogWriter->Write(rc?"Pipe2file is not running.\n":"Pipe2file is running.\n", tcn35d_DBBackupControllerLogWriter::UsePrefixAndDateTime);
        LogWriter->Write(tcn35d_DBBackupControllerLogWriter::EndParagraph);
    }
    
    return rc;
}

void tcn35c_DBMStageBackupController::ReceiveBackupRestoreRequestReplyFromDB()
{
    //nothing to do pipe2file had ended and its return code is already known
    if(DBActionWasSuccessful())
        DBBackupSize=CurrentStageFile->GiveInfo()->GiveFileSize();  //pipe2file has copied the whole file to the pipe
    else
        DBBackupSize=0; //not the whole file was coppied to the pipe, 0 bytes is a approximation, which is good enough
}

int tcn35c_DBMStageBackupController::DBMayBeUsingLogDevspaceForLogRestore()
{
    return 0;   //during a stage backup we never use a log devspace for restore
}

int tcn35c_DBMStageBackupController::DBActionWasSuccessful()
{
    int rc=0;

    if(0!=Pipe2FileProcess && Pipe2FileState==Ended && 0==Pipe2FileProcess->GiveReturnCode())
        rc=1;

    return rc;
}

void tcn35c_DBMStageBackupController::SignalEndToDB(int elapsedTimeout)
{
    TerminateDBRequest();   //do nothing here, we can not "cancel" pipe2file just kill it
}

void tcn35c_DBMStageBackupController::TerminateDBRequest()
{
    if(0!=Pipe2FileProcess && Pipe2FileState!=Ended && Pipe2FileProcess->IsRunning())
    {
        LogWriter->Write("Killing not reacting pipe2file process.\n", tcn35d_DBBackupControllerLogWriter::AddALevelToPrefix);
        LogWriter->Write(Pipe2FileProcess->Stop()?"Pipe2file killed successfully.\n":"Pipe2file could not been killed.\n", tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefixAndEndParagraph);
    }
}

void tcn35c_DBMStageBackupController::WriteHistoryFile()
{
    if(OK_CN00==GiveLastError() && RequestReplied==RequestState())
    {
        int i, rc=1;

        LogWriter->Write("Updating external backup history.\n", tcn35d_DBBackupControllerLogWriter::AddALevelToPrefix);

        int    NumberOfExternalBackupIDsFound=0;
        char * lineForHSSCommand=0; //!< used to hold the backup_history_append command for HSS systems
        bool   isHSS=DBMSrvHSS_Nodes::isHSS();

        for(i=0; i<MAX_NUMBER_OF_TOOLPROCESSES_CN35; i++)
            cn36_StrDealloc(ExternalBackupID[i]);

        if(!BackupToolConnector->CollectExternalBackupIDs(ExternalBackupID, NumberOfExternalBackupIDsFound) ||
            NumberOfExternalBackupIDsFound!=BackupInfo->NumberOfPipes())
        {
            ErrorHandler->Report(tcn35e_DBMBackupError(tcn35e_DBMBackupError::CaseCouldNotGetExternalBackupIDs));
            rc=0;
        }

        if(rc && isHSS && !cn36_StrAllocCpy(lineForHSSCommand, "backup_history_append"))
        {
            ErrorHandler->Report(tcn35d_DBBackupError::OutOfMemory.Set());
            rc=0;
        }

        if(rc)
        {
            tni34_ATextFile TheLogFile(BackupInfo->PathOfExternalBackupHistory(), tni34_AFile::ToDiskAppend, 1, 1);

            if(TheLogFile.IsGood())
            {
                char NewLogFileEntry[2048];
                int i;

                for(i=0; i<NumberOfExternalBackupIDsFound && rc; i++)
                {
                    char * DateTime=CurrentStageFile->GiveInfo()->getStartDateTimeAsString();

                    if(0==DateTime)
                        rc=0;

                    if(rc && 0==ExternalBackupID[i])
                    {
                        rc=0; //end the loop
                        ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
                    }

                    if(rc)
                        rc=CurrentStageFile->GiveInfo()->SetEBID(ExternalBackupID[i]);

                    if(rc)
                    {
                        sprintf(NewLogFileEntry,
                                "%s|%s|%s|%s|%s|0||%s|vftPipe|NO|%ld|%d|%c|%s|%d|", //<DBMRc> is always 0 and <ErrorText> always ""
                                CurrentStageFile->GiveInfo()->GiveInternalBackupID(),
                                ExternalBackupID[i],
                                BA_EBIDS_TYPE_LOG_REC_CN003C,
                                DateTime,
                                BackupToolConnector->ToolEnum().AsString(),
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
                            rc=0; //end the loop
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
                                rc=0;
                            }
                        }
                    }

                    if(0!=DateTime)
                        cn36_StrDealloc(DateTime);
                }

                int AllIsOKBeforeClose=rc;

                if(!TheLogFile.Close())
                    rc=0;

                if(AllIsOKBeforeClose && isHSS)
                {
                    DBMSrv_Reply tempReply; //use a temporary reply buffer, as we are not really interested in the output during update of dbm.ebf

                    cn00DBMServerData::vcontrol()->pHSSNodes->ExecuteOnAllOtherNodes(lineForHSSCommand, tempReply);
                }
            }
            else
                rc=0;
        }

        cn36_StrDealloc(lineForHSSCommand); //last chance to free the memory

        LogWriter->Write(rc?"Have updated external backup history successfully.\n":"Could not update external backup history.\n", tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefixAndEndParagraph);
    }
}

void tcn35c_DBMStageBackupController::DBPostOperation()
{
}

void tcn35c_DBMStageBackupController::PrintBackupAnswer(int NoAction, int IsFinal)
{
    if(NoAction || DBActionWasSuccessful())
    {
        char * CurrPos=0;

        sprintf(ReplyData, "%s%s", ANSWER_OK_CN00, LINE_SEPSTRING_CN00);
        CurrPos=ReplyData+strlen(ReplyData);

        sprintf(CurrPos, "%-20s%s%d%s", BA_RESULT_STAGEFILES_PROCESSED_CN003, VALUE_SEPSTRING_CN00, int(NumberOfProcessedStageFiles), LINE_SEPSTRING_CN00);
        CurrPos=ReplyData+strlen(ReplyData);
        
        sprintf(CurrPos, "%-20s%s%s%s", BA_RESULT_STAGEFILES_BYTES_CN003, VALUE_SEPSTRING_CN00, 0!=BytesProcessed?BytesProcessed:"0", LINE_SEPSTRING_CN00);
        CurrPos=ReplyData+strlen(ReplyData);

        sprintf(CurrPos, "%-20s%s%d%s", BA_RESULT_STAGEFILES_DELETED_CN003, VALUE_SEPSTRING_CN00, int(NumberOfDeletedStageFiles), LINE_SEPSTRING_CN00);
        CurrPos=ReplyData+strlen(ReplyData);

        sprintf(CurrPos, "%-20s%s%d%s",
                BA_RESULT_STAGEFILES_UNPROCESSED_CN003,
                VALUE_SEPSTRING_CN00,
                int(NumberOfUnprocessedStageFiles+(NumberOfProcessedStageFiles-NumberOfDeletedStageFiles)),
                LINE_SEPSTRING_CN00);

        *ReplyLen=(int)strlen(ReplyData);

        if(IsFinal)
        {
            LogWriter->Write("Constructed the following reply:\n", tcn35d_DBBackupControllerLogWriter::AddALevelToPrefix);
            LogWriter->Write(ReplyData);

            LogWriter->Write(tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefixAndEndParagraph);
        }
    }
    else
        ErrorHandler->Report(tcn35e_DBMBackupError(tcn35e_DBMBackupError::CasePipe2FileFailed, Pipe2FileProcess->GiveReturnCode()));
}

void tcn35c_DBMStageBackupController::CheckForErrors(int IsFinal)
{
    if(!AnErrorOccured())   // no error reported so far -> fill the reply buffer
    {
        int StepWasSuccessful=(RequestState()==RequestReplied && ToolState()==tcn35a_DBBackupController::Ended && BackupToolConnector->ToolWasSuccessful(DBBackupSize));

        if(IsFinal || !StepWasSuccessful)
            LogWriter->Write("Filling reply buffer.\n", tcn35d_DBBackupControllerLogWriter::AddALevelToPrefix);

        ErrorHandler->SetPhaseTo(tcn35d_DBBackupControllerErrorHandler::BackupOperation);   //reports of real backup/restore errors are constructed here -> set the correct error type

        switch(RequestState())
        {
            case NoRequestSend:
            case RequestSend:
            case CancelSendByDBMSrv:
                ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError)); // must have reported already an error!  (wheter tool was started or not)
            break;

            case RequestReplied:
            {
                if(tcn35d_BackupInfo::ARestore==BackupInfo->DataDirection() && 2==Pipe2FileProcess->GiveReturnCode())   //after a failed compare we are not interested in tool problems
                    ErrorHandler->Report(tcn35e_DBMBackupError(tcn35e_DBMBackupError::CaseStageVerifyFailed, CurrentStageFile->GiveInfo()->GiveNameOfStageFile()));
                else
                {
                    switch(ToolState())
                    {
                        case tcn35a_DBBackupController::NotStarted:
                        {
                            if(ErrorHandler->AnErrorOccured()) // tool was not started, because db had finished already (no error reported, tool not started -> nobody tried to start the tool
                                PrintBackupAnswer(0, IsFinal); //just tell what pipe2file sayed
                            else  //the error was already reported (written in the reply buffer), FillReplyBuffer() should not have been called in this case!
                                ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
                        }
                        break;
                        case tcn35a_DBBackupController::Ended:
                        {
                            if(BackupToolConnector->ToolWasSuccessful(DBBackupSize))
                                PrintBackupAnswer(0, IsFinal);
                            else
                            {
                                if(DBActionWasSuccessful())
                                    ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseBackupToolFailed, BackupToolConnector->SumOfToolExitCodes()));
                                else
                                    ErrorHandler->Report(tcn35e_DBMBackupError(tcn35e_DBMBackupError::CaseBackupToolAndPipe2FileFailed, Pipe2FileProcess->GiveReturnCode(), BackupToolConnector->SumOfToolExitCodes()));
                            }
                        }
                        break;
                        case tcn35a_DBBackupController::KilledByDBMSrv:
                        {
                            ErrorHandler->Report(tcn35e_DBMBackupError(tcn35e_DBMBackupError::CaseBackupToolKilled, Pipe2FileProcess->GiveReturnCode(), BackupToolConnector->SumOfToolExitCodes()));
                        }
                        break;
                        default: ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
                    }
                }
            }
            break;

            case CanceledByDBMSrv:      //both times pipe2file was killed!
            case TerminatedByDBMSrv:
            {
                switch(ToolState())
                {
                    case tcn35a_DBBackupController::NotStarted:
                    {
                        ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
                        // if no error was reported so far(!IsBad()), tool must have been started
                        // if error was reported already(IsBad()), this function must not have been called
                    }
                    break;
                    case tcn35a_DBBackupController::Ended:
                    {
                        if(BackupToolConnector->ToolWasSuccessful(DBBackupSize))  // do not override a tool error
                            ErrorHandler->Report(tcn35e_DBMBackupError(tcn35e_DBMBackupError::CasePipe2FileKilled, BackupToolConnector->SumOfToolExitCodes()));
                        else
                            ErrorHandler->Report(tcn35e_DBMBackupError(tcn35e_DBMBackupError::CaseBackupToolFailedAndPipe2FileKilled, BackupToolConnector->SumOfToolExitCodes()));
                    }
                    break;
                    case tcn35a_DBBackupController::KilledByDBMSrv:
                    {
                        ErrorHandler->Report(tcn35e_DBMBackupError(tcn35e_DBMBackupError::CaseBackupToolAndPipe2FileKilled));
                    }
                    break;
                    default: ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
                }
            }
            break;

            default: ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
        }

        if(IsFinal || !StepWasSuccessful)
            LogWriter->Write("Reply buffer filled.\n", tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefixAndEndParagraph);
    }
}

void tcn35c_DBMStageBackupController::FillReplyBuffer()
{
    CheckForErrors(1);
}

int tcn35c_DBMStageBackupController::DetermineFilesInDirectory(ni31_List<StageFileInfo *> & ListOfFilesFound,
                                                               const char        * NameOfDirectory,
                                                               const char        * FileNameStart,
                                                               const char        * AbsoluteNameWitoutVersion)
{
    int rc=1;

    tni34_ADirectory Dir(NameOfDirectory);

    if(Dir.IsGood() && Dir.Open())
    {
        const char *DirEntry=0;

        while(rc && (rc=Dir.Read(DirEntry)) && 0!=DirEntry)
        {
            int AddedAsPossibleStageFile=0;

            if(0==cn36_PathNCmp(DirEntry, FileNameStart, strlen(FileNameStart)) &&  //found <FileName> followed by 
               '.'==DirEntry[strlen(FileNameStart)] &&                              //'.', followed by
               cn36_StrConsistsOfDigits(DirEntry+strlen(FileNameStart)+1))          //a number (in other words we are only interested in files with a name like <FileName>.<Number>)
            {
                int FileNumberIsWished=1;

                if(SuffixesWereWished)
                {
                    int SuffixOfFile=0;

                    if(!cn36_StrToInt(SuffixOfFile, DirEntry+strlen(FileNameStart)+1))
                    {
                        ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
                        rc=0;
                    }
                    else
                    {
                        FileNumberIsWished=0;

                        const ni31_ListElem<int> *h;

                        for(h=ListOfWishedSuffixes.First(); !FileNumberIsWished && rc && 0!=h; h=h->Next()) //check against all wished numbers
                        {
                            if(h->GiveInfo()==SuffixOfFile &&                                               //if the number of the current file is in the list
                               3<=strlen(DirEntry+strlen(FileNameStart)+1))                                 //<Number> greater than 99 or of form 00? 0??
                            {
                                FileNumberIsWished=1;                                                       //we found it in the list -> it can be added
                            }
                        }
                    }
                }

                if(rc && FileNumberIsWished)
                {
                    StageFileInfo *StageInfo=new StageFileInfo(AbsoluteNameWitoutVersion,
                                                               DirEntry+strlen(FileNameStart),
                                                               strlen(DirEntry+strlen(FileNameStart)),
                                                               m_BytesPerBlockOnStage);

                    if(0==StageInfo || 0==ListOfFilesFound.Add(StageInfo))
                    {
                        ErrorHandler->Report(tcn35d_DBBackupError::OutOfMemory.Set());
                        rc=0;
                    }
                    else
                        if(!StageInfo->IsGood() || !StageInfo->DetermineDateTimeFileSize())
                            rc=0;

                    if(rc)
                        AddedAsPossibleStageFile=1;
                }
            }

            if(rc)
            {
                sprintf(MsgBuf_cn36, "Found directory entry '%s'. It %s a stage file.\n", DirEntry, AddedAsPossibleStageFile?"could be":"is not");
                LogWriter->Write(MsgBuf_cn36);
            }
        }
    }
    else
        rc=0;

    return rc;
}

bool tcn35c_DBMStageBackupController::determineLabels(
    ni31_List<StageFileInfo *> & listOfFilesFound)
{
    bool rc=true;
    ni31_ListElem<StageFileInfo *> * fileListElem;
    
    char               replyData[1024+1]="";
    int                replyLen=0;
    int                replyLenMax=1204;

    ni31_List<StageFileInfo *> shortenedListOfFilesFound;

    listOfFilesFound.Reverse();

    for(fileListElem=listOfFilesFound.First(); rc && 0!=fileListElem; fileListElem=fileListElem->Next())    //for all found files
    {
        bool            gotLabel=false;
        StageFileInfo * info=fileListElem->GiveInfo();

        if(!info->containsAtLeast2Blocks()) //make sure that label was already written
        {
            sprintf(MsgBuf_cn36, "Could not determine label of potential stage file %s.\n", info->GiveNameOfStageFile());
            LogWriter->Write(MsgBuf_cn36, tcn35e_DBMBackupControllerLogWriter::AddALevelToPrefix);
            LogWriter->Write("File does not contain 2 blocks.");
            LogWriter->Write(tcn35e_DBMBackupControllerLogWriter::SubALevelFromPrefix);
        }
        else
        {
            if(info->determineBackupLabel(replyData, replyLen, replyLenMax))
            {
                if(!shortenedListOfFilesFound.Add(info))
                {
                    rc=false;
                    cn90AnswerIError(replyData, &replyLen, ERR_MEM_CN00);
                }
                else
                {
                    char * h=info->getStartDateTimeAsString();

                    gotLabel=true;

                    sprintf(MsgBuf_cn36, "Stage file %s has label %s and the backup started at %s.\n",
                            info->GiveNameOfStageFile(),
                            info->getBackupLabel().getLabel(),
                            h?h:"");
                    LogWriter->Write(MsgBuf_cn36);

                    cn36_StrDealloc(h);
                }
            }
            else
            {
                sprintf(MsgBuf_cn36, "Could not determine label of potential stage file %s.\n", info->GiveNameOfStageFile());
                LogWriter->Write(MsgBuf_cn36, tcn35e_DBMBackupControllerLogWriter::AddALevelToPrefix);
                LogWriter->Write(replyData);
                LogWriter->Write(tcn35e_DBMBackupControllerLogWriter::SubALevelFromPrefix);
            }
        }

        if(!gotLabel && 0!=info)
            delete info;
    }

    //move shortened list to original and get it back in previous order
    listOfFilesFound.DelAll();
    shortenedListOfFilesFound.MoveAllToBeginOf(listOfFilesFound);

    return rc;
}

int tcn35c_DBMStageBackupController::RemoveStageFiles()
{
    int rc=1;

    if(RemoveStageFilesFlag)
    {
        LogWriter->Write("Removing stage files.\n", tcn35d_DBBackupControllerLogWriter::AddALevelToPrefix);
        CurrentStageFile=StageFiles.First();

        while(0!=CurrentStageFile && !AnErrorOccured())
        {
            tni34_AVirtualFile TheFile(CurrentStageFile->GiveInfo()->GiveNameOfStageFile(), tni34_AVirtualFile::ToDiskAppend, tni34_AVirtualFile::DoNotOpenNow);

            if(!TheFile.IsGood() || !TheFile.Remove())
                rc=0;
            else
            {
                sprintf(MsgBuf_cn36, "Removed stage file '%s'.\n", TheFile.GiveName());
                LogWriter->Write(MsgBuf_cn36);

                NumberOfDeletedStageFiles++;
            }

            CurrentStageFile=CurrentStageFile->Next();
        }

        LogWriter->Write(rc?"Removed stage files successfully.\n":"Could not remove all stage files successfully.\n", tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefixAndEndParagraph);
    }

    return rc;
}

int tcn35c_DBMStageBackupController::MovePipe2FileOutputToLogFile()
{
    int rc=1;

    if(0!=Pipe2FileOutput && !MoveFileToLogFile(*Pipe2FileOutput, "pipe2file output"))
        rc=0;

    if(0!=Pipe2FileError && !MoveFileToLogFile(*Pipe2FileError, "pipe2file error output"))
        rc=0;

    return rc;
}

int tcn35c_DBMStageBackupController::MoveFileToLogFile(tni34_AFile & TheFileToMove, const char * FileDescription)
{
    int rc;

    rc=LogWriter->Write(TheFileToMove, FileDescription);

    if(!rc)
    {
        ErrorHandler->Report(tcn35e_DBMBackupError(tcn35e_DBMBackupError::CaseCouldNotMovePipe2FileOutputToToolProtFile, TheFileToMove.GiveName())); // now report the write-Problem

        sprintf(MsgBuf_cn36,
                "Not removing %s (%s).\n",
                FileDescription,
                TheFileToMove.GiveName());

        LogWriter->Write(MsgBuf_cn36);
    }

    if(rc && TheFileToMove.Exists() && TheFileToMove.IsAFile())
    {
        if(!TheFileToMove.Remove())
        {
            rc=0;

            sprintf(MsgBuf_cn36,
                    "Could not remove %s '%s'.\n",
                    FileDescription,
                    TheFileToMove.GiveName());
        }
        else
        {
            sprintf(MsgBuf_cn36,
                    "Removed %s '%s'.\n",
                    FileDescription,
                    TheFileToMove.GiveName());   
        }
        
        LogWriter->Write(MsgBuf_cn36);
    }
    else
        rc=0;

    return rc;
}


// -----------------------------------------------------------------------------
// member functions tcn35c_TheDBMStageBackupControllerFactory
// -----------------------------------------------------------------------------

tcn00_Error tcn35c_TheDBMStageBackupControllerFactory::ConstructADBMStageBackupController(tcn35c_DBMStageBackupController    *& TheNewBackupController,
                                                                                          char                               *  TheReplyBuffer, 
                                                                                          int                                *  TheReplyBufferLength,
                                                                                          //const char                         *  BackupType,
                                                                                          const tcn003_Medium                *  Medium,
                                                                                          const tcn003_MediaGroup            *  MediaGroup,
                                                                                          const tcn003_Medium                *  Stage,
                                                                                          const tcn003_MediaGroup            *  StageGroup,
                                                                                          const char                         *  TheDBName,
                                                                                          int                                   WithVerify,
                                                                                          int                                   RemoveStageFiles,
                                                                                          const char                         *  SuffixListStart,
                                                                                          size_t                                SuffixListLength)
{
    tcn00_Error rc=OK_CN00;

    TheNewBackupController=0;

    tcn35e_DBMBackupControllerLogWriter    * LogWriter=0;
    tcn35e_DBMBackupControllerErrorHandler * ErrorHandler=0;
    tcn35_BackupToolConnector              * BackupTool=0;
    tcn35d_BackupInfo                      * BackupInfo=0;
    tcn35e_ConfigurationVariableHandler    * ConfigurationHandler=0;

    tsp00_Pathc   AbsoluteNameOfProtocolFile;   //dbm.ebp
    tsp00_Pathc   AbsoluteNameOfLogFile;        //ebm.ebl
    tsp00_DbNamec tDBName;

    long stageBytesPerBlock=0;

    tcn35e_DBMBackupControllerErrorHandler::BasicErrorHandler_cn35e.SetReplyArrayTo(TheReplyBuffer, TheReplyBufferLength);

    tDBName.rawAssign(TheDBName);

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
        ErrorHandler=new tcn35e_DBMBackupControllerErrorHandler(TheReplyBuffer,
                                                                TheReplyBufferLength,
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

        if(OK_CN00==rc && !RunDirectory(Rundirectory, TheDBName, ErrorHandler, LogWriter))
            rc=ErrorHandler->GiveLastError();

        if(OK_CN00==rc)
        {
            const char * MediumName;
            int          NumberOfPipes;
            long         BytesPerBlock;
            long         PagesPerBlock;
            long         BytesPerPage;
            long         CountInPages[MAX_NUMBER_OF_DATAPIPES_CN35D];
            char       * ThePipeNames[MAX_NUMBER_OF_DATAPIPES_CN35D];
            char       * TheMediumTypes[MAX_NUMBER_OF_DATAPIPES_CN35D];
            char       * BackupHistory=0;
            char       * ExternalBackupHistory=0;

            const char * TheNodeNameConst=cn36_GetHostName();

            if(0==TheNodeNameConst)
                rc=ErrorHandler->GiveLastError();
            else
                if(!cn36_StrAllocCpy(TheNodeName, TheNodeNameConst))    //make a copy of the string
                    rc=ErrorHandler->GiveLastError();

            if(OK_CN00==rc)
            {
                if(!MediumInfo(MediumName,
                               NumberOfPipes,
                               ThePipeNames,
                               TheMediumTypes,
                               BytesPerBlock,
                               PagesPerBlock,
                               BytesPerPage,
                               CountInPages,
                               TheDBName,
                               Medium,
                               MediaGroup,
                               *ErrorHandler))
                {
                    rc=ErrorHandler->GiveLastError();
                }
            }

            if(OK_CN00==rc)
            {
                if(!BackupHistories(BackupHistory,
                                    ExternalBackupHistory,
                                    TheDBName,
                                    *ErrorHandler))
                {
                    rc=ErrorHandler->GiveLastError();
                }
            }

            if(OK_CN00==rc)
            {
                BackupInfo=new tcn35d_BackupInfo(tcn35d_BackupInfo::ABackup,
                                                 tcn35d_BackupInfo::Log,
                                                 MediumName,
                                                 NumberOfPipes,
                                                 ThePipeNames,
                                                 TheMediumTypes,
                                                 BytesPerBlock,
                                                 PagesPerBlock,
                                                 CountInPages,
                                                 Rundirectory,
                                                 TheNodeName,
                                                 TheDBName,
                                                 0,//TheSourceNodeName,
                                                 0,//TheSourceDBName,
                                                 BackupHistory,
                                                 ExternalBackupHistory);

                if(0==BackupInfo)
                {
                    rc=ERR_MEM_CN00;
                    ErrorHandler->Report(tcn35d_DBBackupError::OutOfMemory.Set());
                }
                else
                    if(!BackupInfo->IsGood())
                        rc=ErrorHandler->GiveLastError();
            }

            if(OK_CN00!=rc) //clean up if something went wrong
            {
                if(0==BackupInfo)
                {
                    int i;

                    for(i=0; i<MAX_NUMBER_OF_DATAPIPES_CN35D; i++)
                        cn36_StrDealloc(ThePipeNames[i]);
                }
                else
                {
                    delete BackupInfo;
                    BackupInfo=0;
                }
            }

            cn36_StrDealloc(BackupHistory);
            cn36_StrDealloc(ExternalBackupHistory);
        }

        cn36_StrDealloc(Rundirectory);
        cn36_StrDealloc(TheNodeName);   
    }

    if(OK_CN00==rc)
    {
        ConfigurationHandler=new tcn35e_ConfigurationVariableHandler();
        
        if(0==ConfigurationHandler)
        {
            rc=ERR_MEM_CN00;
            ErrorHandler->Report(tcn35d_DBBackupError::OutOfMemory.Set());
        }
        else
        {
            if(!tcn35_BackupToolConnector::ConstructBackupToolConnector(BackupTool,
                                                                        DBMSrvBMedia_MediaGroup::BackupTool(Medium, MediaGroup),
                                                                        BackupInfo,
                                                                        LogWriter,
                                                                        ErrorHandler,
                                                                        ConfigurationHandler,
                                                                        0))//CopyOfExternalBackupIDList))
            {
                rc=ErrorHandler->GiveLastError();
            }
        }

        if(OK_CN00!=rc) //clean up if something went wrong
        {
            if(0==BackupTool)
            {
                if(0!=ConfigurationHandler)
                    delete ConfigurationHandler;

                //cn36_StrDealloc(CopyOfExternalBackupIDList);
            }
            else
            {
                delete BackupTool;
                BackupTool=0;
            }
        }
    }

    if(OK_CN00==rc)
    {
        //determine block size of stage medium
        const char * stageMediumName=0;
        int          stageNumberOfFiles=0;
        long         stagePagesPerBlock=0;
        long         stageBytesPerPage=0;
        long         stageCountInPages[MAX_NUMBER_OF_DATAPIPES_CN35D];
        char       * stageFileNames[MAX_NUMBER_OF_DATAPIPES_CN35D];
        char       * stageMediumTypes[MAX_NUMBER_OF_DATAPIPES_CN35D];
       
        if(!MediumInfo(stageMediumName,
                       stageNumberOfFiles,
                       stageFileNames,
                       stageMediumTypes,
                       stageBytesPerBlock,
                       stagePagesPerBlock,
                       stageBytesPerPage,
                       stageCountInPages,
                       TheDBName,
                       Stage,
                       0,
                       *ErrorHandler))
        {
            rc=ErrorHandler->GiveLastError();
        }

        for(int i=0; i<MAX_NUMBER_OF_DATAPIPES_CN35D; i++)
        {
            cn36_StrDealloc(stageFileNames[i]);
            cn36_StrDealloc(stageMediumTypes[i]);
        }
    }

    if(OK_CN00==rc)
    {
        TheNewBackupController=new tcn35c_DBMStageBackupController(BackupTool,
                                                                   ErrorHandler,
                                                                   LogWriter,
                                                                   BackupInfo,
                                                                   ConfigurationHandler,
                                                                   TheReplyBuffer, 
                                                                   TheReplyBufferLength,
                                                                   Stage,
                                                                   stageBytesPerBlock,
                                                                   WithVerify,
                                                                   RemoveStageFiles,
                                                                   SuffixListStart,
                                                                   SuffixListLength);

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
    }
  
    return rc;
}

tcn35c_TheDBMStageBackupControllerFactory TheDBMStageBackupControllerFactory_cn35c;
