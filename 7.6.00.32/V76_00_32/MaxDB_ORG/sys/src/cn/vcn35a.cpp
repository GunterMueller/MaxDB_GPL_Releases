/*!
  \file    vcn35a.cpp
  \author  TiloH
  \ingroup cn layer
  \brief   implementing base class for controling backups/replications to
           external backup tools

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

#include <stdio.h>

#include "gcn003.h"
#include "hcn35a.h"
#include "hni00b.h"
#include "hcn36.h"


// -----------------------------------------------------------------------------
// member functions of class tcn35a_DBBackupController
// -----------------------------------------------------------------------------

tcn35a_DBBackupController::tcn35a_DBBackupController(tcn35_BackupToolConnector             * TheBackupToolConnector,
                                                     tcn35d_DBBackupControllerErrorHandler * TheErrorHandler,
                                                     tcn35d_DBBackupControllerLogWriter    * TheLogWriter,
                                                     tcn35d_BackupInfo                     * TheBackupInfo,
                                                     tcn35d_ConfigurationVariableHandler   * TheConfigurationVariableHandler)
    :BackupToolConnector(TheBackupToolConnector),
    ErrorHandler(TheErrorHandler),
    LogWriter(TheLogWriter),
    BackupInfo(TheBackupInfo),
    ConfigurationVariableHandler(TheConfigurationVariableHandler),
    DBBackupSize(0),
    IsGoodFlag(1),
    StatusOfRequest(NoRequestSend),
    StatusOfTool(NotStarted),
    CurrentWaitingPhase(NoWaiting),
    WaitForeverOnDB(0),
    StartOfCurrentWaitingPhase(-1),
    OutputInterval(0),
    LastOutput(0),
    NumberOfExistingPipes(0)
{
    if(0==BackupToolConnector || 0==ErrorHandler || 0==LogWriter || 0==BackupInfo)
        IsGoodFlag=0;
    else
    {
        int i;

        for(i=0; i<MAX_NUMBER_OF_TOOLPROCESSES_CN35; i++)
            ExternalBackupID[i]=0;

        for(i=0; i<MAX_NUMBER_OF_DATAPIPES_CN35D; i++)
            TheDataPipe[i]=0;
    }
}

tcn35a_DBBackupController::~tcn35a_DBBackupController()
{
    int i;

    if(0!=BackupToolConnector)
        delete BackupToolConnector;

    if(0!=ErrorHandler)
        delete ErrorHandler;

    if(0!=LogWriter)
        delete LogWriter;

    if(0!=BackupInfo)
        delete BackupInfo;

    if(0!=ConfigurationVariableHandler)
        delete ConfigurationVariableHandler;

    for(i=0; i<MAX_NUMBER_OF_DATAPIPES_CN35D; i++)
        if(0!=TheDataPipe[i])
            delete TheDataPipe[i];

    for(i=0; i<MAX_NUMBER_OF_TOOLPROCESSES_CN35; i++)
        cn36_StrDealloc(ExternalBackupID[i]);
}

void tcn35a_DBBackupController::BeginBackupOperation()
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

            const char * PrepareBackupRestoreString=0;

            if(tcn35d_BackupInfo::ABackup==BackupInfo->DataDirection())
                PrepareBackupRestoreString="Preparing backup.\n";
            else
                if(tcn35d_BackupInfo::ARestore==BackupInfo->DataDirection())
                    PrepareBackupRestoreString="Preparing restore.\n";
                else
                {
                    ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
                    AllOk=0;
                }

            if(AllOk)
            {
                LogWriter->Write(PrepareBackupRestoreString, tcn35d_DBBackupControllerLogWriter::AddALevelToPrefix);
            
                AllOk=Prepare();

                if(AllOk)
                    AllOk=BackupToolConnector->Prepare();

                LogWriter->Write(AllOk?"Prepare passed successfully.\n":"Prepare failed.\n", tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefixAndEndParagraph);
            }

            if(AllOk)
            {
                AllOk=0;

                if(CreatePipes())
                {
                    ErrorHandler->SetPhaseTo(tcn35d_DBBackupControllerErrorHandler::DBRequest);
                    
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

                                AllOk=1;    // only if we get here, we have started the db request and the backup tool correctly
                            }
                        }
                        else
                            StatusOfRequest=RequestReplied;
                    }
                    else
                        StatusOfRequest=NoRequestSend;
                }
            }

            if(AnErrorOccured() || !AllOk)  // if something went wrong, stop everything and clean up the allocated resources (pipes, files, ...)
                EndBackupOperation();
        }
    }
}


int tcn35a_DBBackupController::BackupOperationIsRunning()
{
    //function returns 0 if backup operation is not running or has already finished
    //                 1 if database or backup tool is working
    //         uses RequestSend==StatusOfRequest to indicate running database request
    //              Started==StatusOfTool to indicate running backup tool       

    int  rc=1;
    int  LastWaitingPhase=CurrentWaitingPhase;
    long currentTime=cn36_GetCurrentTime();
    bool OutputMessagesForNonEvents=(OutputInterval <= cn36_SecondsBetween(currentTime, LastOutput));

    ErrorHandler->SetAsCurrentErrorHandler();
    ErrorHandler->SetPhaseTo(tcn35d_DBBackupControllerErrorHandler::OperationCheck);

    //check that db and tool were started correctly at first entrance!
    if(NoWaiting==CurrentWaitingPhase)
    {
        WaitForeverOnDB=0;

        if(NotStarted==StatusOfTool &&                  // the tool was not started
           NoRequestSend==StatusOfRequest &&            // no request was send to the db
           !AnErrorOccured())                           // no error was reported
        {
            ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseNoBackupIsRunning)); // no error was reported and nothing is running -> no backup/restore on the way
        }

        if(RequestSend==StatusOfRequest && Started==StatusOfTool)
            CurrentWaitingPhase=WaitForDBAndBackupTool; //database and tool are working, start waiting for the end of one or the other
        else
            CurrentWaitingPhase=WaitForDB;              //database or tool or both are not working, same as one has already ended

        StartOfCurrentWaitingPhase=cn36_GetCurrentTime();   //get the current time
        OutputInterval=5;                                   //use initial output interval of 5 seconds for every backup operation
    }

    //check if database and tool are running
    if(WaitForDBAndBackupTool==CurrentWaitingPhase) // (RequestSend==StatusOfRequest && Started==StatusOfTool), db and backup tool were working, when checked last time.
    {                                               // then
        int ToolWasSuccessfulSoFar=1;
        int ToolFinishedCompletely=0;

        if(!BackupToolConnector->ToolIsRunning(ToolFinishedCompletely, OutputMessagesForNonEvents))   // check backup tool again, but mostly quietly
        {   
            if(ToolFinishedCompletely)
                StatusOfTool=Ended;
            else
            {
                StatusOfTool=EndedPartially;
                ToolWasSuccessfulSoFar=BackupToolConnector->ToolWasSuccessfulSoFar();   //were the parts that finished already successful so far
            }
        }

        if(DBReplyAvailable(OutputMessagesForNonEvents))  // check db again
            ReceiveReplyFromDB();

        if(RequestReplied==StatusOfRequest ||                            // if db has finished the operation
           Ended==StatusOfTool ||                                        // or tool finished completely
           (EndedPartially==StatusOfTool && !ToolWasSuccessfulSoFar))    // or tool finished partially unsuccessful
        {
            CurrentWaitingPhase=WaitForDB;  //go to next phase

            if(RequestSend==StatusOfRequest &&          // still now answer from the db
               DBMayBeUsingLogDevspaceForLogRestore())  // test if we are doing a restor of log
            {
                WaitForeverOnDB=1;
            }

            StartOfCurrentWaitingPhase=cn36_GetCurrentTime();   //set maximal reaction time and get the current time
        }
    }

    if(WaitForDB==CurrentWaitingPhase)  // wait maximal MaxWaitingTimeOfPhase[CurrentWaitingPhase] seconds for reaction of the db kernel, if it is still running
    {
        int ChangePhase=1;
        int elapsedTimeout=0;

        if(RequestSend==StatusOfRequest)
        {
            if(WaitForeverOnDB ||
               MaxWaitingTimeForDB()>=cn36_SecondsBetween(StartOfCurrentWaitingPhase, cn36_GetCurrentTime()))
            {
                if(DBReplyAvailable(OutputMessagesForNonEvents)) // check db again
                    ReceiveReplyFromDB();
                else
                    ChangePhase=0;
            }
            else
                elapsedTimeout=MaxWaitingTimeForDB();
        }

        if(ChangePhase)
        {
            int ToolFinishedCompletely=0;

            CurrentWaitingPhase=WaitForBackupTool;  //go to next phase

            StartOfCurrentWaitingPhase=cn36_GetCurrentTime();

            if(RequestSend==StatusOfRequest)
            {
                SignalEndToDB(elapsedTimeout);      //start cancel DB action after an elapsed timeout
                StatusOfRequest=CancelSendByDBMSrv; //StatusOfRequest changes to CancelSendByDBMSrv -> should start cancel db only once
            }

            if(Ended!=StatusOfTool)
            {
                if(BackupToolConnector->ToolIsRunning(ToolFinishedCompletely, true) || !ToolFinishedCompletely)
                    BackupToolConnector->SignalEndToTool(DBActionWasSuccessful());
                else
                    StatusOfTool=Ended;
            }
        }
    }

    if(WaitForBackupTool==CurrentWaitingPhase)
    {
        int ChangePhase=1;
        int ToolFinishedCompletely=0;

        if(Started==StatusOfTool || EndedPartially==StatusOfTool)  //was the tool running or partially running?
        {
            if(MaxWaitingTimeForTool()>=cn36_SecondsBetween(StartOfCurrentWaitingPhase, cn36_GetCurrentTime()))
            {
                if(!BackupToolConnector->ToolIsRunning(ToolFinishedCompletely, OutputMessagesForNonEvents) && ToolFinishedCompletely)
                    StatusOfTool=Ended;
                else
                    ChangePhase=0;
            }
            else
            {
                if(BackupToolConnector->ToolIsRunning(ToolFinishedCompletely, OutputMessagesForNonEvents) || !ToolFinishedCompletely)
                {
                    sprintf(MsgBuf_cn36, "Canceling tool after the timeout of %d seconds elapsed.\n", int(MaxWaitingTimeForTool()));
                    LogWriter->Write(MsgBuf_cn36, tcn35d_DBBackupControllerLogWriter::UsePrefixAndDateTime);

                    BackupToolConnector->CancelTool();
                }
                else
                    StatusOfTool=Ended;
            }
        }
        
        if(ChangePhase)
        {
            CurrentWaitingPhase=WaitForKilled;  //go to next phase

            StartOfCurrentWaitingPhase=cn36_GetCurrentTime();
        }
    }

    if(WaitForKilled==CurrentWaitingPhase)  // wait maximal MaxWaitingTimeOfPhase[CurrentWaitingPhase] seconds for reaction of the db kernel, if it is still running or the tool if it is still running
    {
        int ToolFinishedCompletely=0;

        if(RequestSend==StatusOfRequest || CancelSendByDBMSrv==StatusOfRequest) // was the DB working on the request?
        {
            if(MaxWaitingTimeForDB()>=cn36_SecondsBetween(StartOfCurrentWaitingPhase, cn36_GetCurrentTime()))
            {
                if(DBReplyAvailable(OutputMessagesForNonEvents))  // check db again
                    ReceiveReplyFromDB();
            }
            else 
                TerminateDBAction();
        }

        if(Started==StatusOfTool || EndedPartially==StatusOfTool)  //was the tool running or partialy running?
        {
            if(MaxWaitingTimeForTool()>=cn36_SecondsBetween(StartOfCurrentWaitingPhase, cn36_GetCurrentTime()))
            {
                if(!BackupToolConnector->ToolIsRunning(ToolFinishedCompletely, OutputMessagesForNonEvents) && ToolFinishedCompletely)
                    StatusOfTool=Ended;
            }
            else
                if(KilledByDBMSrv!=StatusOfTool)
                {
                    if(BackupToolConnector->ToolIsRunning(ToolFinishedCompletely, OutputMessagesForNonEvents) || !ToolFinishedCompletely)
                    {
                        sprintf(MsgBuf_cn36, "Terminating tool after the timeout of %d seconds elapsed.\n", int(MaxWaitingTimeForTool()));
                        LogWriter->Write(MsgBuf_cn36, tcn35d_DBBackupControllerLogWriter::UsePrefixAndDateTime);

                        BackupToolConnector->TerminateTool();
                        StatusOfTool=KilledByDBMSrv;    // calling TerminateTool() only once
                    }
                    else
                        StatusOfTool=Ended;
                }              
        }

        if((RequestSend!=StatusOfRequest && CancelSendByDBMSrv!=StatusOfRequest) &&      // if db request &&
           (Started!=StatusOfTool && EndedPartially!=StatusOfTool))                      // tool are not working
            rc=0;                                                                        // set return-code appropriate
    }

    if(rc)
        LogWriter->Write(tcn35d_DBBackupControllerLogWriter::EndParagraph);

    if(OutputMessagesForNonEvents)
    {
        LastOutput=currentTime; //use the start of this function call as new reference
        OutputInterval+=5;      //increase interval between two outputs
    }

    if(LastWaitingPhase!=CurrentWaitingPhase)  // the waiting phase has changed,
        OutputInterval=5;                      // -> reset output interval

    return rc;
}

void tcn35a_DBBackupController::EndBackupOperation()
{
    ErrorHandler->SetAsCurrentErrorHandler();

    LogWriter->Write(tcn35d_BackupInfo::ABackup==BackupInfo->DataDirection()?
                     "Waiting for end of the backup operation.\n":
                     "Waiting for end of the restore operation.\n",
                     tcn35d_DBBackupControllerLogWriter::AddALevelToPrefix);

    // must not check that Backup is/was running. BackupOperationIsRunning() reports error if necessary
    while(BackupOperationIsRunning())
    {
        Sleep(1, false);
    }
    
    LogWriter->Write(tcn35d_BackupInfo::ABackup==BackupInfo->DataDirection()?
                     "The backup operation has ended.\n":
                     "The restore operation has ended.\n",
                     tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefixAndEndParagraph);

    ErrorHandler->SetPhaseTo(tcn35d_DBBackupControllerErrorHandler::PostOperation);

    if(tcn35d_BackupInfo::ABackup==BackupInfo->DataDirection() &&   //to be surely backward compatible ...
       DBActionWasSuccessful() &&
       NotStarted!=StatusOfTool)
    {
        BackupToolConnector->ToolWasSuccessful(DBBackupSize);       //.. determine the result from the backup tools point of view
    }

    if(!AnErrorOccured())   // no error reported so far -> fill the reply buffer
    {
        ErrorHandler->SetPhaseTo(tcn35d_DBBackupControllerErrorHandler::BackupOperation);   //reports of real backup/restore errors are constructed here -> set the correct error type
        FillReplyBuffer();
    }

    if(tcn35d_BackupInfo::ABackup==BackupInfo->DataDirection() &&
       DBActionWasSuccessful())
    {
        WriteHistoryFile();

        if(NotStarted!=StatusOfTool &&
           BackupToolConnector->ToolWasSuccessful(DBBackupSize) &&
           !AnErrorOccured())                                                 //no error occurred (also not in WriteLogFile())
        {
            DBPostOperation();
        }
    }

    if(!AnErrorOccured() && DBActionWasSuccessful())            // no error reported so far -> do all necessary post-operations
    {
        ErrorHandler->SetPhaseTo(tcn35d_DBBackupControllerErrorHandler::PostOperation); // set possible errorcode back to ERR_POSTOP_CN00
        BackupToolConnector->PostOperationsAfterSuccess();
    }
    
    CleanUp();
}

int tcn35a_DBBackupController::AnErrorOccured()
{
    return (0==IsGoodFlag || ErrorHandler->AnErrorOccured());
}

tcn35a_DBBackupController::ARequestState tcn35a_DBBackupController::RequestState()
{
    return StatusOfRequest;
}

tcn35a_DBBackupController::AToolState tcn35a_DBBackupController::ToolState()
{
    return StatusOfTool;
}

void tcn35a_DBBackupController::Sleep(int Seconds, bool OutputMessage)
{
    if(OutputMessage)
    {
        sprintf(MsgBuf_cn36,"Waiting %d second%s ... ", (int)Seconds, Seconds>1?"s":"");
        LogWriter->Write(MsgBuf_cn36);
    }

    cn36_Sleep(Seconds);

    if(OutputMessage)
        LogWriter->Write("Done.\n", tcn35d_DBBackupControllerLogWriter::NoPrefix);
}

int tcn35a_DBBackupController::CreatePipes()
{
    int rc=1;
    
    #if defined (WIN32)
    //database or backup tool have to create the pipes under NT
    #elif defined (UNIX)
    int i;
    LogWriter->Write("Creating pipes for data transfer.\n", tcn35d_DBBackupControllerLogWriter::AddALevelToPrefix);

    for(i=0; i<BackupInfo->NumberOfPipes() && NumberOfExistingPipes==i; i++)
    {
        if(0!=TheDataPipe[i])
            delete TheDataPipe[i]; //last chance to free that memory

        TheDataPipe[i]=new tni34_APipe(BackupInfo->NameOfPipe(i), tni34_APipe::In, 0, 0);

        if(0==TheDataPipe[i])
            ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::OutOfMemory));
        else
        {
            sprintf(MsgBuf_cn36,"Creating pipe '%s' ... ", BackupInfo->NameOfPipe(i));
            LogWriter->Write(MsgBuf_cn36);

            if(!TheDataPipe[i]->Create())
            {
                LogWriter->Write("Failed.\n", tcn35d_DBBackupControllerLogWriter::NoPrefix);
                ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseCouldNotCreateDataPipe, TheDataPipe[i]->GiveName()));

                delete TheDataPipe[i];
                TheDataPipe[i]=0;
            }
            else
            {
                LogWriter->Write("Done.\n", tcn35d_DBBackupControllerLogWriter::NoPrefix);
                NumberOfExistingPipes++;
            }
        }
    }

    if(NumberOfExistingPipes==BackupInfo->NumberOfPipes())
        LogWriter->Write("All data transfer pipes have been created.\n",
                         tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefixAndEndParagraph);
    else
    {
        rc=0;
        LogWriter->Write(0==NumberOfExistingPipes?
                         "No data transfer pipes have been created.\n":
                         "Not all data transfer pipes have been created.\n",
                         tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefixAndEndParagraph);
    }
    #else
        #error tcn35a_DBBackupController::CreatePipes() was only coded for WIN32 and UNIX (define one of them).
    #endif

    return rc;
}

int tcn35a_DBBackupController::CheckPipes()
{
    int rc=1;

    if(MAX_NUMBER_OF_DATAPIPES_CN35D<BackupInfo->NumberOfPipes())
    {
        rc=0;
        ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseTooManyPipes));
    }

    // check type "PIPE" for each used medium
    for(int i=0; rc && i<BackupInfo->NumberOfPipes(); i++)
        if(0!=BackupInfo->MediumType(i) && 0!=cn36_StrUprCmp(ME_TYPE_PIPE_CN003, BackupInfo->MediumType(i))) //if we know the medium type, but it is not a pipe
        {
            ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseWrongMediumType, BackupInfo->MediumType(i)));
            rc=0;
        }

    return rc;
}

int tcn35a_DBBackupController::RemovePipes()
{
    int i, rc=0;
    
    if(0<NumberOfExistingPipes)
    {
        LogWriter->Write("Removing data transfer pipes.\n", tcn35d_DBBackupControllerLogWriter::AddALevelToPrefix);

        for(i=NumberOfExistingPipes-1; 0<=i; i--)
        {
            if(BackupInfo->WasMarkedAsRemoved(i))
            {
                sprintf(MsgBuf_cn36, "Data transfer pipe %s was already removed before (see above).\n", TheDataPipe[i]->GiveName());
                LogWriter->Write(MsgBuf_cn36);
                NumberOfExistingPipes--;
            }
            else
            {
                sprintf(MsgBuf_cn36, "Removing data transfer pipe %s ... ", TheDataPipe[i]->GiveName());
                LogWriter->Write(MsgBuf_cn36);

                if(TheDataPipe[i]->Remove())
                {
                    NumberOfExistingPipes--;
                    LogWriter->Write("Done.\n", tcn35d_DBBackupControllerLogWriter::NoPrefix);
                }
            }
        }

        if(0==NumberOfExistingPipes)
        {
            rc=1;
            sprintf(MsgBuf_cn36, "Removed data transfer pipes successfully.\n");
        }
        else
        {
            NumberOfExistingPipes=0;  // declare all pipes as removed, no way for handling unremoveable pipes
            sprintf(MsgBuf_cn36, "Not all data transfer pipes could be removed.\n");
        }

        LogWriter->Write(MsgBuf_cn36, tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefixAndEndParagraph);
    }
    else
        rc=1;

    return rc;
}

int tcn35a_DBBackupController::MaxWaitingTimeForTool()
{
    return BackupToolConnector->MaxWaitingTimeForToolInWaitingPhase(true, DBActionWasSuccessful()?true:false);
}

int tcn35a_DBBackupController::MaxWaitingTimeForDB()
{
    return BackupToolConnector->MaxWaitingTimeForToolInWaitingPhase(false, BackupToolConnector->ToolWasSuccessfulSoFar()?true:false);
}

void tcn35a_DBBackupController::ReceiveReplyFromDB()
{
    ReceiveBackupRestoreRequestReplyFromDB();   //receive the reply

    if(CancelSendByDBMSrv==StatusOfRequest)     //change the status of the request
        StatusOfRequest=CanceledByDBMSrv;
    else
        StatusOfRequest=RequestReplied;
}

void tcn35a_DBBackupController::TerminateDBAction()
{
    if(TerminatedByDBMSrv!=StatusOfRequest)
    {
        if(DBReplyAvailable())  // check db for a last time again
            ReceiveReplyFromDB();
        else
        {
            TerminateDBRequest();   //last possibility to end the request (if possible at all)
            StatusOfRequest=TerminatedByDBMSrv;
        }
    }
}

void tcn35a_DBBackupController::CleanUp()
{
    int AllOk=1;

    ErrorHandler->SetPhaseTo(tcn35d_DBBackupControllerErrorHandler::CleanUp);
    
    LogWriter->Write("Cleaning up.\n", tcn35d_DBBackupControllerLogWriter::AddALevelToPrefix);
    
    if(!RemovePipes())                  // remove all created "data-transfer" pipes (must not be all pipes of medium) 
        AllOk=0;

    if(!BackupToolConnector->MoveToolOutputToProtocolFile()) // move stdout and stderr of started tool processes to the protocol (dbm.ebp)
        AllOk=0;

    if(!BackupToolConnector->CleanUpTool())                  // do tool specific cleanup
        AllOk=0;
    
    LogWriter->Write(AllOk?"Have finished clean up successfully.\n":"Could not clean up all correctly.\n",
                     tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefixAndEndParagraph);
}
