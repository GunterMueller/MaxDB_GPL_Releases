/*!
    \file    vcn35.cpp
    \author  TiloH
    \ingroup cn layer
    \brief   implementing connection between dbmsrv and abstract external
             backup tool, declared in hcn35.h 

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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

#include "hcn35.h"
#include "hcn351.h"
#include "hcn352.h"
#include "hcn353.h"
#include "hcn36.h"

#include "SAPDBCommon/SAPDB_string.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


// -----------------------------------------------------------------------------
// member functions of class tcn35_BackupToolConnector
// -----------------------------------------------------------------------------

tcn35_BackupToolConnector::tcn35_BackupToolConnector(tcn35d_BackupInfo                     *  TheBackupInfo,
                                                     tcn35d_DBBackupControllerLogWriter    *  TheLogWriterToUse,
                                                     tcn35d_DBBackupControllerErrorHandler *  TheErrorHandlerToUse,
                                                     tcn35d_ConfigurationVariableHandler   *  TheConfigurationVariableHandler,
                                                     char                                  *& TheExternalBackupIDList)
    :BackupInfo(TheBackupInfo),
    LogWriter(TheLogWriterToUse),
    ErrorHandler(TheErrorHandlerToUse),
    ConfigurationVariableHandler(TheConfigurationVariableHandler),
    ExternalBackupIDList(0),
    AlreadyPrepared(0),
    ToolTimeoutSuccess(300),
    ToolTimeoutSuccessIsDefaultFlag(1),
    ToolTimeoutFailure(60),
    ToolTimeoutFailureIsDefaultFlag(1),
    KernelTimeoutSuccess(300),
    KernelTimeoutFailure(60)
{
    int i;

    NumberOfCommandLines=0;
    NumberOfRunningToolProcesses=0;
    for(i=0; i<MAX_NUMBER_OF_TOOLPROCESSES_CN35; i++)
    {
        CommandLine[i]=0;
        ToolProcess[i]=0;
        ToolProcessWasRunning[i]=0;
        ToolProcessOutput[i]=0;
        ToolProcessError[i]=0;
    }

    TempDateTimeString=0;
    ToolProcessOutputAsText=0;

    if(BackupInfo->DataDirection()==tcn35d_BackupInfo::ARestore &&
       (0==TheExternalBackupIDList || 0>=strlen(TheExternalBackupIDList)))
    {
        ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseNeedExternalBackupIDForRestore));
    }

    if(0!=TheExternalBackupIDList)
    {
        size_t LengthOfExternalBackupIDList=strlen(TheExternalBackupIDList);

        if(LengthOfExternalBackupIDList>2 &&                                                                    //the list of Backup ID's has more than 2 characters
           '"'==(*TheExternalBackupIDList) && '"'==(*(TheExternalBackupIDList+LengthOfExternalBackupIDList-1))) //the whole thing is enclosed in double qoutes
        {
            cn36_StrNAllocCpy(ExternalBackupIDList=0, TheExternalBackupIDList+1, LengthOfExternalBackupIDList-2);
            cn36_StrDealloc(TheExternalBackupIDList);
        }
        else
        {
            ExternalBackupIDList=TheExternalBackupIDList;
            TheExternalBackupIDList=0;
        }
    }//else ExternalBackupIDList equals allready 0

    if(MAX_NUMBER_OF_TOOLPROCESSES_CN35<BackupInfo->NumberOfPipes())
        ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseTooManyPipes));

    char * TempDirName=0;
    int    FoundTempDirEnv=0;

    if(DetermineValueOfEnvironmentVariable("TEMP", TempDirName, "directory for temporary files and pipes" , BackupInfo->DBName()) &&
       0!=strlen(TempDirName))
    {
        FoundTempDirEnv=1;
    }
    else
        if(DetermineValueOfEnvironmentVariable("TMP", TempDirName, "directory for temporary files and pipes" , BackupInfo->DBName()) &&
           0!=strlen(TempDirName))
        {
            FoundTempDirEnv=1;
        }

    if(FoundTempDirEnv)
    {
        tni34_ADirectory TempDir(TempDirName, 0, 0);

        if(TempDir.IsGood())
            tni34_ADataEntry::SetTemporaryDataEntryDirectory(TempDir);
    }

    char * TimeoutAsString=0;

    if(DetermineValueOfEnvironmentVariable("DBM_TTO_SUCCESS",
                                           TimeoutAsString,
                                           "timeout for the external backup tool in case of success",
                                           BackupInfo->DBName(),
                                           0,
                                           0)                                               &&
       0!=strlen(TimeoutAsString))
    {
        if(cn36_StrToInt(i, TimeoutAsString))
            SetToolTimeoutSuccess(i);
    }

    cn36_StrDealloc(TimeoutAsString);

    if(DetermineValueOfEnvironmentVariable("DBM_TTO_FAILURE",
                                           TimeoutAsString,
                                           "timeout for the external backup tool in case of failure",
                                           BackupInfo->DBName(),
                                           0,
                                           0)                                                 &&
       0!=strlen(TimeoutAsString))
    {
        if(cn36_StrToInt(i, TimeoutAsString))
            SetToolTimeoutFailure(i);
    }

    cn36_StrDealloc(TimeoutAsString);

    if(DetermineValueOfEnvironmentVariable("DBM_KTO_SUCCESS",
                                           TimeoutAsString,
                                           "timeout for database kernel in case of success",
                                           BackupInfo->DBName(),
                                           0,
                                           0)                                               &&
       0!=strlen(TimeoutAsString))
    {
        if(cn36_StrToInt(i, TimeoutAsString))
            KernelTimeoutSuccess=i;
    }

    cn36_StrDealloc(TimeoutAsString);

    if(DetermineValueOfEnvironmentVariable("DBM_KTO_FAILURE",
                                           TimeoutAsString,
                                           "timeout for database kernel in case of failure",
                                           BackupInfo->DBName(),
                                           0,
                                           0)                                               &&
       0!=strlen(TimeoutAsString))
    {
        if(cn36_StrToInt(i, TimeoutAsString))
            KernelTimeoutFailure=i;
    }

    cn36_StrDealloc(TimeoutAsString);

    TheExternalBackupIDList=0;          //"avoid" deletion of TheExternalBackupIDList
}

tcn35_BackupToolConnector::~tcn35_BackupToolConnector()
{
    int i;

    for(i=0; i<MAX_NUMBER_OF_TOOLPROCESSES_CN35; i++)
    {
        cn36_StrDealloc(CommandLine[i]);

        if(0!=ToolProcess[i])
            delete ToolProcess[i];

        if(0!=ToolProcessOutput[i])
            delete ToolProcessOutput[i];

        if(0!=ToolProcessError[i])
            delete ToolProcessError[i];
    }

    cn36_StrDealloc(TempDateTimeString);
    if(0!=ToolProcessOutputAsText)
        delete ToolProcessOutputAsText;

    cn36_StrDealloc(ExternalBackupIDList);
}

int tcn35_BackupToolConnector::SetExternalBackupIDList(const char * NewEBIDList)
{
    return cn36_StrAllocCpy(ExternalBackupIDList, NewEBIDList);
}

int tcn35_BackupToolConnector::PrepareToolOutputOfAvailableBackups()
{
    int rc=0;

    if(0!=ToolProcessOutputAsText)
        delete ToolProcessOutputAsText;
    
    ToolProcessOutputAsText=new tni34_ATextFile(ToolProcessOutput[0]->GiveName(), tni34_AFile::FromDisk, 0, 0); // do not create, do not open now

    if(0==ToolProcessOutputAsText)
        ErrorHandler->Report(tcn35d_DBBackupError::OutOfMemory.Set());
    else
    {
        if(ToolProcessOutputAsText->IsGood() && ToolProcessOutputAsText->Open())
            rc=1;
    }

    return rc;
}

int tcn35_BackupToolConnector::GiveNextAvailableBackup(int          & EBIDIsAvailable,
                                                       const char * & EBID,
                                                       size_t       & EBIDLength,
                                                       tcn35d_BackupInfo::ADataType    & DataType,
                                                       const char * & DateTimeString,
                                                       int          & ReachedEnd)
{
    int rc=1;
    const char *Line=0;

    ReachedEnd=0;

    if(!ToolProcessOutputAsText->IsGood())
        rc=0;
    else
    {
        int LineContainsEBID=0;

        while(!LineContainsEBID && rc && !ReachedEnd)
        {
            rc=ToolProcessOutputAsText->ReadLine(Line);
                
            if(rc)
            {
                if(0!=Line)
                {
                    rc=GiveInformationOfLine(Line,
                                             EBID,
                                             EBIDLength,
                                             DataType,
                                             DateTimeString,
                                             LineContainsEBID);
                }
                else
                    ReachedEnd=1;
            }
        }

        if(LineContainsEBID)
            EBIDIsAvailable=1;
        else
            EBIDIsAvailable=0;
    }

    return rc;
}

int tcn35_BackupToolConnector::CloseToolOutputOfAvailableBackups()
{
    int rc=1;
    
    if(0!=ToolProcessOutputAsText)
    {
        rc=ToolProcessOutputAsText->Close();

        delete ToolProcessOutputAsText;
        ToolProcessOutputAsText=0;
    }

    return rc;
}

int tcn35_BackupToolConnector::CheckPipes()
{
    int rc=1;

    if(MAX_NUMBER_OF_TOOLPROCESSES_CN35<BackupInfo->NumberOfPipes())
    {
        rc=0;
        ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseTooManyPipes));
    }

    return rc;
}

int tcn35_BackupToolConnector::Prepare(int PrepareCompletly)
{
    int rc=0;

    if(AlreadyPrepared && !PrepareCompletly)
        rc=PrepareAgain();
    else
        rc=PrepareFirst();

    AlreadyPrepared=1;

    return rc;
}

int tcn35_BackupToolConnector::LastMinutePreparation(int NumberOfCurrentCommandLine)
{
    return 1;   //do nothing and return with OK.
}

int tcn35_BackupToolConnector::StartTool()
{
    int i,rc=1;

    sprintf(MsgBuf_cn36,"Starting %s.\n", GiveToolName());
    LogWriter->Write(MsgBuf_cn36, tcn35d_DBBackupControllerLogWriter::AddALevelToPrefix);
    
    if(0==NumberOfRunningToolProcesses)
    {
        for(i=0; i<NumberOfCommandLines && rc; i++)
        {
            if(!LastMinutePreparation(i))
                rc=0;
            else
            {
                if(0==ToolProcessOutput[i] || 0==ToolProcessError[i])
                {
                    ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
                    rc=0;
                }
                else
                {
                    if(0!=ToolProcess[i])
                        delete ToolProcess[i];

                    if(0!=CommandLine[i])
                    {
                        sprintf(MsgBuf_cn36,
                                "Starting %s process '%s >>%s 2>>%s'.\n",
                                GiveToolName(),
                                CommandLine[i],
                                ToolProcessOutput[i]->GiveName(),
                                ToolProcessError[i]->GiveName());

                        LogWriter->Write(MsgBuf_cn36);

                        ToolProcess[i]=new tni33_ABackgroundProcess(CommandLine[i], (tni34_AFile *)(0), ToolProcessOutput[i], 1, ToolProcessError[i], 1);
                    }
                    else
                        if(0!=ArgumentList[i].GiveCommandLine())
                        {
                            sprintf(MsgBuf_cn36,
                                    "Starting %s process '%s >>%s 2>>%s'.\n",
                                    GiveToolName(),
                                    ArgumentList[i].GiveCommandLine(),
                                    ToolProcessOutput[i]->GiveName(),
                                    ToolProcessError[i]->GiveName());
                            
                            LogWriter->Write(MsgBuf_cn36);

                            ToolProcess[i]=new tni33_ABackgroundProcess(ArgumentList[i], (tni34_AFile *)(0), ToolProcessOutput[i], 1, ToolProcessError[i], 1);
                        }
                        else
                            ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
                }

                if(0==ToolProcess[i])
                {
                    ErrorHandler->Report(tcn35d_DBBackupError::OutOfMemory.Set());
                    rc=0;
                }
                else
                    if(!ToolProcess[i]->WasStarted())
                        rc=0;
                    else
                    {
                        NumberOfRunningToolProcesses++;
                        ToolProcessWasRunning[i]=1;
                    }

                if(rc)
                    LogWriter->Write("Process was started successfully.\n");
                else
                    LogWriter->Write("Start of process failed.\n");
            }
        }
    }
    else
        rc=0;

    if(rc)
        sprintf(MsgBuf_cn36, "%s has been started successfully.\n", GiveToolName());
    else
        sprintf(MsgBuf_cn36, "Could not start %s correctly.\n", GiveToolName());

    LogWriter->Write(MsgBuf_cn36, tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefixAndEndParagraph);

    return rc;
}

int tcn35_BackupToolConnector::MaxWaitingTimeForToolInWaitingPhase(bool forTool, bool actionWasSuccessful)
{
    int rc;

    if(forTool)
    {
        if(actionWasSuccessful)
            rc=ToolTimeoutSuccess;
        else
            rc=ToolTimeoutFailure;
    }
    else
    {
        if(actionWasSuccessful)
            rc=KernelTimeoutSuccess;
        else
            rc=KernelTimeoutFailure;
    }

    return rc;
}

int tcn35_BackupToolConnector::CheckTool()
{
    return 1;
};

int tcn35_BackupToolConnector::ToolIsRunning(bool beVerbose)
{
    int FinishedCompletely=0;

    return ToolIsRunning(FinishedCompletely, beVerbose);
}

int tcn35_BackupToolConnector::ToolIsRunning(int & FinishedCompletely, bool beVerbose)
{
    int i,
        rc=1;
    
    FinishedCompletely=1;

    for(i=0; i<NumberOfRunningToolProcesses; i++)
    {
        if(!ToolProcessWasRunning[i])
            rc=0;
        else
        {
            if(!ToolProcess[i]->IsRunning())
            {
                rc=0;

                sprintf(MsgBuf_cn36,"%s backup tool process has finished work with return code %d.\n", NumberOfRunningToolProcesses==1 ? "The" : "A", int(ToolProcess[i]->GiveReturnCode()));
                LogWriter->Write(MsgBuf_cn36, tcn35d_DBBackupControllerLogWriter::UsePrefixAndDateTime);

                ToolProcessWasRunning[i]=0;
            }
            else
                FinishedCompletely=0;
        }
    }

    if(FinishedCompletely)
        rc=0;   // just to be sure in the case of 0>=NumberOfRunningToolProcesses

    if(beVerbose)
    {
        if(rc)
            LogWriter->Write("The backup tool is running.\n", tcn35d_DBBackupControllerLogWriter::UsePrefixAndDateTime/*, tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefix*//*AndEndParagraph*/);
        else
            LogWriter->Write(FinishedCompletely?"The backup tool is not running.\n":"The backup tool is not running completely.\n", tcn35d_DBBackupControllerLogWriter::UsePrefixAndDateTime/*, tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefix*//*AndEndParagraph*/);
    }

    return rc;
}

int tcn35_BackupToolConnector::ExistToolProcesses()
{
    int i,rc=0;

    for(i=0;i<MAX_NUMBER_OF_TOOLPROCESSES_CN35 && 0==rc;i++)
        if(0!=ToolProcess[i] && ToolProcess[i]->IsRunning())
            rc=1;

    return rc;
}

int tcn35_BackupToolConnector::SignalEndToTool(int DBWasSuccessful)
{
    return 1;
}

void tcn35_BackupToolConnector::CancelTool()
{
    int PipeDirectionKnown=1,
        i;

    tni34_APipe::ADirection PipeDirection; 

    switch(BackupInfo->DataDirection())
    {
        case tcn35d_BackupInfo::ABackup:
            PipeDirection=tni34_APipe::Out;
        break;

        case tcn35d_BackupInfo::ARestore:
            PipeDirection=tni34_APipe::In;
        break;

        default:
            ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
            PipeDirectionKnown=0;
    }

    if(PipeDirectionKnown)
        for(i=0; i<BackupInfo->NumberOfPipes(); i++)
        {
            int PipeWasRemoved=0;

            tni34_APipe Pipe(BackupInfo->NameOfPipe(i), PipeDirection, 0, 0);    //do not create now, do not open now

            sprintf(MsgBuf_cn36, "Trying to break pipe '%s'.\n", Pipe.GiveName());
            LogWriter->Write(MsgBuf_cn36);

            if(Pipe.BreakAndRemovePipeIfOpened(PipeWasRemoved))
                LogWriter->Write("Pipe has been broken successfully.\n");
            else
                LogWriter->Write("Pipe couldn't be broken successfully.\n");

            if(PipeWasRemoved)
            {
                sprintf(MsgBuf_cn36, "The pipe '%s' was removed.\n", Pipe.GiveName());
                LogWriter->Write(MsgBuf_cn36);
                BackupInfo->MarkPipeAsRemoved(i);   //set a flag such that the pipe creating BackupController does not try to remove the pipe a second time
            }
        }
}

void tcn35_BackupToolConnector::TerminateTool()
{
    int i;

    for(i=0;i<MAX_NUMBER_OF_TOOLPROCESSES_CN35;i++)
        if(0!=ToolProcess[i] && ToolProcess[i]->IsRunning())
            ToolProcess[i]->Stop();
}

int tcn35_BackupToolConnector::PostOperationsAfterSuccess()
{
    return 1;   // normaly we have no real post operations, we just do clean up (see CleanUp())
}

int tcn35_BackupToolConnector::DetermineValueOfEnvironmentVariable(const char * const VariableName,
                                                                         char * &     VariableValue,
                                                                   const char *       VariableComment,
                                                                   const char *       DBName,
                                                                         int          ExportDefaultIfNotFound,
                                                                         int          ExportConfigVariableIfNotInEnvironment)
{
    int rc=1;

    char * FileEnvVar=0;

    if(0==VariableComment)
        VariableComment="unspecified variable";

    if(!ConfigurationVariableHandler->GetConfigurationVariable(BackupInfo, VariableName, FileEnvVar))
        rc=0;
    else
    {
        const char * pszEnvVar=cn36_GetEnv(VariableName);

        if(0!=FileEnvVar && FileEnvVar[0]!='\0')    //if found a non empty value for the variable in dbm.cfg
        {
            rc=cn36_StrAllocCpy(VariableValue, FileEnvVar); //use the value

            if(rc)
            {
                sprintf(MsgBuf_cn36,"Using configuration variable '%s' = '%s'%s%s.\n", VariableName,
                                                                                       VariableValue,
                                                                                       0==VariableComment?"":" as ",
                                                                                       0==VariableComment?"":VariableComment);
                LogWriter->Write(MsgBuf_cn36);

                if((0==pszEnvVar && ExportConfigVariableIfNotInEnvironment) ||      // if environment variable has no value we export it if 0!=ExportConfigVariableIfNotInEnvironment
                   (0!=pszEnvVar && strcmp(pszEnvVar, FileEnvVar))              )   // if environment variable has other value export the configuration value to the environment
                {
                    sprintf(MsgBuf_cn36,
                            "Setting environment variable '%s'%s%s %s%s%sto configuration value '%s'.\n",
                            VariableName,
                            0==VariableComment?"":" for the ",
                            0==VariableComment?"":VariableComment,
                            0==pszEnvVar?"":"from '",
                            0==pszEnvVar?"":pszEnvVar,
                            0==pszEnvVar?"":"' ",
                            VariableValue);

                    LogWriter->Write(MsgBuf_cn36);

                    rc=cn36_PutEnv(VariableName, VariableValue);
                }
            }
        }
        else
        {
            if(0==pszEnvVar)
            {
                if(0==VariableValue)
                {
                    VariableValue=new char[1]; // be sure, that it can be deleted by delete []
                    VariableValue[0]='\0';
                }

                if(ExportDefaultIfNotFound)
                {
                    sprintf(MsgBuf_cn36, "Setting environment variable '%s'%s%s to default '%s'.\n", VariableName,
                                                                                                     0==VariableComment?"":" for the ",
                                                                                                     0==VariableComment?"":VariableComment,
                                                                                                     VariableValue);
                    LogWriter->Write(MsgBuf_cn36);

                    rc=cn36_PutEnv(VariableName, VariableValue);
                }
            }
            else
            {
                rc=cn36_StrAllocCpy(VariableValue, pszEnvVar);

                if(rc)
                {
                    sprintf(MsgBuf_cn36,"Using environment variable '%s' with value '%s'%s%s.\n", VariableName,
                                                                                                  VariableValue,
                                                                                                  0==VariableComment?"":" as ",
                                                                                                  0==VariableComment?"":VariableComment);

                    LogWriter->Write(MsgBuf_cn36);
                }
            }
        }
    }

    cn36_StrDealloc(FileEnvVar);

    return rc;
}

int tcn35_BackupToolConnector::PutEnvironmentVariable(const char * VariableName,
                                                      const char * VariableValue)
{
    int rc;

    sprintf(MsgBuf_cn36, "Setting environment variable '%s' to value '%s'.\n", VariableName, VariableValue);
    LogWriter->Write(MsgBuf_cn36);
    
    rc=cn36_PutEnv(VariableName, VariableValue);

    return rc;
}

int tcn35_BackupToolConnector::Remove(const char *Name)
{
    int rc=0;

    tni34_AFile File(Name, tni34_AFile::ToDiskTruncate, 0, 0); // do not open, do not create now

    if(File.Exists())
    {
        if(File.Remove())
            rc=1;
    }
    else
        ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseAFileIsMissingOrUnreadable, Name));

    if(0==rc)
    {
        sprintf(MsgBuf_cn36, "Could not remove the file '%s'.\n", Name);
        LogWriter->Write(MsgBuf_cn36);
    }

    return rc;
}

void tcn35_BackupToolConnector::Sleep(int Seconds, bool beVerbose)
{
    if(beVerbose)
    {
        sprintf(MsgBuf_cn36,"Waiting %d second%s ... ", (int)Seconds, Seconds>1?"s":"");
        LogWriter->Write(MsgBuf_cn36);
    }

    cn36_Sleep(Seconds);

    if(beVerbose)
        LogWriter->Write("Done.\n", tcn35d_DBBackupControllerLogWriter::NoPrefix);
}

void tcn35_BackupToolConnector::SetToolTimeoutSuccess(int NewToolTimeoutSuccess)
{
    if(0<=NewToolTimeoutSuccess)
    {
        ToolTimeoutSuccess=NewToolTimeoutSuccess;
        ToolTimeoutSuccessIsDefaultFlag=0;
    }
}

void tcn35_BackupToolConnector::SetToolTimeoutFailure(int NewToolTimeoutFailure)
{
    if(0<=NewToolTimeoutFailure)
    {
        ToolTimeoutFailure=NewToolTimeoutFailure;
        ToolTimeoutFailureIsDefaultFlag=0;
    }
}

int tcn35_BackupToolConnector::ToolTimeoutSuccessIsDefault() const
{
    return ToolTimeoutSuccessIsDefaultFlag;
}

int tcn35_BackupToolConnector::ToolTimeoutFailureIsDefault() const
{
    return ToolTimeoutFailureIsDefaultFlag;
}

int tcn35_BackupToolConnector::GetToolTimeoutSuccess() const
{
    return ToolTimeoutSuccess;
}

int tcn35_BackupToolConnector::GetToolTimeoutFailure() const
{
    return ToolTimeoutFailure;
}

int tcn35_BackupToolConnector::MoveFileToProtocolFile(const char *FileDesignation, tni34_AFile & TheFileToMove)
{
    int rc=1;

    char * FileDescription=0;
    
    if(cn36_StrAlloc(FileDescription, strlen(FileDesignation)+strlen(" of ")+strlen(GiveToolName())))
    {
        SAPDB_strcpy(FileDescription, FileDesignation);
        strcat(FileDescription, " of ");
        strcat(FileDescription, GiveToolName());
    }

    rc=LogWriter->Write(TheFileToMove, 0==FileDescription?"a file":FileDescription);

    if(!rc)
    {
        ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseCouldNotMoveToolOutputToToolProtFile, TheFileToMove.GiveName())); // now report the write-Problem

        sprintf(MsgBuf_cn36,
                "Not removing %s's temporary %s file (%s).\n",
                GiveToolName(),
                FileDesignation,
                TheFileToMove.GiveName());

        LogWriter->Write(MsgBuf_cn36);
    }

    if(rc && TheFileToMove.Exists() && TheFileToMove.IsAFile())
    {
        if(!TheFileToMove.Remove())
        {
            rc=0;

            sprintf(MsgBuf_cn36,
                    "Could not remove %s's temporary %s file '%s'.\n",
                    GiveToolName(),
                    FileDesignation,
                    TheFileToMove.GiveName());

            LogWriter->Write(MsgBuf_cn36);
        }
        else
        {
            sprintf(MsgBuf_cn36,
                    "Removed %s's temporary %s file '%s'.\n",
                    GiveToolName(),
                    FileDesignation,
                    TheFileToMove.GiveName());

            LogWriter->Write(MsgBuf_cn36);
        }
    }
    else
        rc=0;

    return rc;
}

int tcn35_BackupToolConnector::MoveToolOutputToProtocolFile()
{
    int rc=1,
        i;

    for(i=0; i<MAX_NUMBER_OF_TOOLPROCESSES_CN35; i++)
    {
        if(0!=ToolProcessOutput[i])
        {
            if(!MoveFileToProtocolFile("output", *(ToolProcessOutput[i])))
                rc=0;

            delete ToolProcessOutput[i];
            ToolProcessOutput[i]=0;
        }
    
        if(0!=ToolProcessError[i])
        {
            if(!MoveFileToProtocolFile("error output", *(ToolProcessError[i])))
                rc=0;

            delete ToolProcessError[i];
            ToolProcessError[i]=0;
        }
    }

    return rc;
}

int tcn35_BackupToolConnector::SumOfToolExitCodes()
{
    int i,sum;

    for(sum=i=0; i<MAX_NUMBER_OF_TOOLPROCESSES_CN35; i++)
        if(0!=ToolProcess[i])
            sum+=ToolProcess[i]->GiveReturnCode();

    return sum;
}

int tcn35_BackupToolConnector::ToolWasSuccessful(teo00_Uint8 DBBackupSize)
{
    int rc=1;

    if(ExistToolProcesses())
        rc=0;
    else
    {
        int i;

        for(i=0; i<MAX_NUMBER_OF_TOOLPROCESSES_CN35 && rc!=0; i++)
            if(0!=ToolProcess[i] && 0!=(ToolProcess[i]->GiveReturnCode()))
                rc=0;
    }
    
    return rc;
}

int tcn35_BackupToolConnector::ToolWasSuccessfulSoFar()
{
    int rc=1,
        i;

    for(i=0; i<MAX_NUMBER_OF_TOOLPROCESSES_CN35 && rc!=0; i++)
        if(0!=ToolProcess[i] && !ToolProcess[i]->IsRunning() && 0!=(ToolProcess[i]->GiveReturnCode()))
            rc=0;
    
    return rc;
}

int tcn35_BackupToolConnector::ConstructBackupToolConnector(tcn35_BackupToolConnector             *& DeterminedTool,
                                                            DBMSrvBTools_ToolEnum                    BackupToolAsEnum,
                                                            tcn35d_BackupInfo                     *  BackupInfo,
                                                            tcn35d_DBBackupControllerLogWriter    *  TheLogWriterToUse,
                                                            tcn35d_DBBackupControllerErrorHandler *  TheErrorHandlerToUse,
                                                            tcn35d_ConfigurationVariableHandler   *  TheConfigurationVariableHandler,
                                                            char                                  *  TheExternalBackupIDList)
{
    int rc=0;

    if(0!=DeterminedTool)
        TheErrorHandlerToUse->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
    else
    {
        rc=1;

        TheLogWriterToUse->Write(tcn35d_DBBackupControllerLogWriter::JustDateTime);

        switch(BackupToolAsEnum)
        {
            case DBMSrvBTools_ToolEnum::ADSM:
                DeterminedTool=new tcn351_ADSMConnector(BackupInfo,
                                                        TheLogWriterToUse,
                                                        TheErrorHandlerToUse,
                                                        TheConfigurationVariableHandler,
                                                        TheExternalBackupIDList);
                break;

            case DBMSrvBTools_ToolEnum::NSR:
                DeterminedTool=new tcn352_NetWorkerConnector(BackupInfo,
                                                             TheLogWriterToUse,
                                                             TheErrorHandlerToUse,
                                                             TheConfigurationVariableHandler,
                                                             TheExternalBackupIDList);
                break;

            case DBMSrvBTools_ToolEnum::BACK:
                DeterminedTool=new tcn353_Backint2Connector(BackupInfo,
                                                            TheLogWriterToUse,
                                                            TheErrorHandlerToUse,
                                                            TheConfigurationVariableHandler,
                                                            TheExternalBackupIDList);
                break;

            case DBMSrvBTools_ToolEnum::None:
                break;

            default:
            {
                rc=0;
                TheErrorHandlerToUse->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
            }
            break;
        }

        if(rc && DBMSrvBTools_ToolEnum::None!=BackupToolAsEnum)
        {
            if(0==DeterminedTool)
            {
                rc=0;
                TheErrorHandlerToUse->Report(tcn35d_DBBackupError::OutOfMemory.Set());

                TheLogWriterToUse->Write("No memory to allocate backup tool object.\n");
            }
            else
            {
                if(TheErrorHandlerToUse->AnErrorOccured())
                    rc=0;
            }
        }

        if(rc && 0!=DeterminedTool)
        {
            sprintf(MsgBuf_cn36, "Using connection to %s.\n", DeterminedTool->GiveLongToolName());
            TheLogWriterToUse->Write(MsgBuf_cn36, tcn35d_DBBackupControllerLogWriter::EndParagraph);
        }
    }

    return rc;
}

