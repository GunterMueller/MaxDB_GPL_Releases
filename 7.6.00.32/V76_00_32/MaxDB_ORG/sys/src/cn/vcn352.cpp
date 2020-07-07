/*!
    \file    vcn352.cpp
    \author  TiloH
    \ingroup cn layer
    \brief   implementing connection between dbmsrv and external backup tool
             NetWorker

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 1999-2006 SAP AG

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

#include "hni34.h"
#include "hcn352.h"
#include "hcn36.h"
#include "ToolsCommon/Time/ToolsTime_Time.hpp"
#include "ToolsCommon/Time/ToolsTime_TimeFormatter.hpp"
#include "ToolsCommon/Tools_CommandlineToArgumentsConverter.hpp"

#include "SAPDBCommon/SAPDB_string.h"

#include <time.h>
#include <math.h>
#include <stdio.h>


const char * tcn352_NetWorkerConnector::NSaveTimeEBIDPrefix="NST";
const char * tcn352_NetWorkerConnector::NSaveTimeEBIDPartDelimiter=" ";

// -----------------------------------------------------------------------------
// class tcn352_NSRError
// -----------------------------------------------------------------------------

class tcn352_NSRError: public tcn35d_DBBackupError
{
  public:
    enum NSRErrorCase
    {
        CaseCanNotDetermineNSR_HOME,
        CaseCanNotDetermineNSR_HOST,
        CaseDifferentPipeDirectories,
        CaseWrongEBIDType,
        CaseUnknownFormatOfMminfoOutput     //must remain the last case
    };

    tcn352_NSRError(NSRErrorCase ErrorCase);
    tcn352_NSRError(NSRErrorCase ErrorCase, const char * ErrInfo);

  private:
    static DBBackupErrorDescription NSRErrorToText[];
    void   SetThisError(int CaseToSet);
};

tcn352_NSRError::tcn352_NSRError(NSRErrorCase ErrorCase)
:tcn35d_DBBackupError()
{
    SetThisError(ErrorCase);
    Set();
}

tcn352_NSRError::tcn352_NSRError(NSRErrorCase ErrorCase, const char * ErrInfo)
:tcn35d_DBBackupError()
{
    SetThisError(ErrorCase);
    Set(ErrInfo);
}

void tcn352_NSRError::SetThisError(int CaseToSet)
{
    if(NSRErrorToText[CaseToSet].Case==CaseToSet)
        ThisError=NSRErrorToText+CaseToSet;
    else
    {
        ThisError=NSRErrorToText;
        
        while(ThisError->Case!=CaseToSet && ThisError->Case!=CaseUnknownFormatOfMminfoOutput)
            ThisError++;

        if(CaseToSet!=ThisError->Case)
            ThisError=GiveDescriptionOfInternalError();
    }
}

tcn352_NSRError::DBBackupErrorDescription tcn352_NSRError::NSRErrorToText[]=
{
    {CaseCanNotDetermineNSR_HOME,
     AString,
     "Could not determine value of 'NSR_HOME'.",
     "Could not determine value of 'NSR_HOME' from file '%s'."},

    {CaseCanNotDetermineNSR_HOST,
     AString,
     "Could not determine value of 'NSR_HOST'.",
     "Could not determine value of 'NSR_HOST' from file '%s'."},

    {CaseDifferentPipeDirectories,
     AString,
     "The pipes of the medium are located in different directories.",
     "The pipes of the medium must be located in the same directory as the first pipe (%s)."},

    {CaseWrongEBIDType,
     AString,
     "An External Backup ID did not match the configured External Backup ID type.",
     "The External Backup ID '%s' did not match the configured External Backup ID type."},

    {CaseUnknownFormatOfMminfoOutput,
     AString,
     "Found unknown format of mminfo output.",
     "Found unknown format of mminfo output '%s'."}
};


// -----------------------------------------------------------------------------
// implementation of class tcn352_NetWorkerConnector
// -----------------------------------------------------------------------------

tcn352_NetWorkerConnector::tcn352_NetWorkerConnector(tcn35d_BackupInfo                     * TheBackupInfo,
                                                     tcn35d_DBBackupControllerLogWriter    * TheLogWriterToUse,
                                                     tcn35d_DBBackupControllerErrorHandler * TheErrorHandlerToUse,
                                                     tcn35d_ConfigurationVariableHandler   * TheConfigurationVariableHandler,
                                                     char                                  * TheExternalBackupIDList)
    :tcn35_BackupToolConnector(TheBackupInfo, TheLogWriterToUse, TheErrorHandlerToUse, TheConfigurationVariableHandler, TheExternalBackupIDList),
     StandardDateTimeFormat2DigitYear("mm/dd/yy HH:MM:SS AM"),
     StandardDateTimeFormat4DigitYear("mm/dd/yyyy HH:MM:SS AM"),
     OutputDateTimeFormat("yyyy-mm-dd HH:MM:SS")
{
    const char * DefaultPathOfNSR_ENVFile=
    #if defined(WIN32)
                            "C:\\Program Files\\nsr\\sapdb\\env";
    #elif defined(UNIX)
                            "/nsr/sapdb/env";
    #else
        #error tcn352_NetWorkerConnector::tcn352_NetWorkerConnector() only coded for UNIX and Windows (define one of them).
    #endif

    cn36_StrAllocCpy(PathOfNSR_ENVFile=0, DefaultPathOfNSR_ENVFile);

    NameOfNSR_HOST=0;
    NameOfNSR_POOL=0;
    NSR_EXPIREValue=0;

    PathOfNSRSave=0;
    PathOfNSRRecover=0;
    PathOfNSRMminfo=0;

    StartSaveTime=long(time(0));
    EndSaveTime=0;
    
    NSRUseDotNSRFile=0;

    LevelFullForBackupType=DataMigration;
    VerboseFlagFor=SaveAndRecover;
    NSREBIDType=NSaveTime;
    TempEBID=0;

    NSROutputResult=NotChecked;

    int i=0;

    for(i=0; i<MAX_NUMBER_OF_TOOLPROCESSES_CN35; i++)
    {
        ErrorInOutput[i]=NotChecked;
        NSRExternalBackupID[i]=0;
    }

    UseNSROutput=1;

    NSRSaveInterval=10;

    NSRBackupName=0;

    NSRAvoidMMInfoBug=0;
    NSRAvoidMMInfoTimeBug=0;

    UseExplicitDateTimeFormat=0;

    Roundup=NoRoundup;

    UseLLOptionForSave=1;
    NSRNumberOfEBIDsFound=0;

    NSRClientName=0;
    NSROptionUseClientFor=tcn352_NetWorkerConnector::UseForMminfo;

    m_NSRAvoidVSSBug=false;
}

tcn352_NetWorkerConnector::~tcn352_NetWorkerConnector()
{
    int i;

    cn36_StrDealloc(PathOfNSR_ENVFile);
    cn36_StrDealloc(NameOfNSR_HOST);
    cn36_StrDealloc(NameOfNSR_POOL);
    cn36_StrDealloc(NSR_EXPIREValue);
    cn36_StrDealloc(PathOfNSRSave);
    cn36_StrDealloc(PathOfNSRRecover);
    cn36_StrDealloc(PathOfNSRMminfo);

    while(!DirectiveFileNames.empty())
    {
        cn36_StrDealloc(DirectiveFileNames.back());
        DirectiveFileNames.pop_back();
    }

    cn36_StrDealloc(TempEBID);

    cn36_StrDealloc(NSRBackupName);

    DeleteListOfErrorsToIgnore();

    cn36_StrDealloc(NSRAvoidMMInfoTimeBug);

    for(i=0; i<MAX_NUMBER_OF_TOOLPROCESSES_CN35; i++)
        cn36_StrDealloc(NSRExternalBackupID[i]);

    cn36_StrDealloc(NSRClientName);
    deleteAllListsOfAdditionalSaveParameters();
}

const char * tcn352_NetWorkerConnector::GiveToolName() const
{
    return "NetWorker";
}

const char * tcn352_NetWorkerConnector::GiveLongToolName() const
{
    return "Legato's NetWorker with save, recover and mminfo";
}

const DBMSrvBTools_ToolEnum tcn352_NetWorkerConnector::ToolEnum() const
{
    return DBMSrvBTools_ToolEnum(DBMSrvBTools_ToolEnum::NSR);
}

int tcn352_NetWorkerConnector::AllocConstructAndCheckExe(char *& Path, const char * DirectoryPath, const char * Name, int & ExistsAndIsExecutable)
{
    int rc=1;

    ExistsAndIsExecutable=1;

    if(cn36_StrAlloc(Path, strlen(DirectoryPath)+strlen(Name)))
    {
        SAPDB_strcpy(Path, DirectoryPath);
        strcat(Path, Name);
        
        if(!cn36_CheckExe(Path))
        {
            ExistsAndIsExecutable=0;
            ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseAFileIsNotExecutable, Path));
        }
    }
    else
        rc=0;

    return rc;
}

class tcn352_NSREnvFileEntry
{
  public:
    const char * KeyWord;
    char **      Destination; 
};

static unsigned cn352_NumberOfSubStringInString(const char * String, const char * SubString)
{
    unsigned NumberOfOccurence=0;

    while(0!=String)
    {
        String=strstr(String, SubString);

        if(0!=String)
        {
            NumberOfOccurence++;
            String+=strlen(SubString);
        }
    }

    return NumberOfOccurence;
}

int tcn352_NetWorkerConnector::CheckToolInstallation(int & IsInstalled)
{
    int rc=1;

    IsInstalled=1;

    // The default value of the path of the NetWorker variables file is set in the constructor
    // That default can be overridden by the 'NSR_ENV' environment variable.
    // For security, it is exported if not found in the environment.    
    LogWriter->Write("Checking existence and configuration of NetWorker.\n", tcn35d_DBBackupControllerLogWriter::AddALevelToPrefix);

    DeleteListOfErrorsToIgnore();
    deleteAllListsOfAdditionalSaveParameters();

    rc=DetermineValueOfEnvironmentVariable("NSR_ENV", PathOfNSR_ENVFile, "path of NetWorker's configuration file", BackupInfo->DBName());

    if(rc) // all is ok, so far?
        if(!cn36_CheckRead(cn36_RemoveOuterDoubleQuotes(PathOfNSR_ENVFile))) //TODO remove function cn36_RemoveOuterDoubleQuotes(), as soon as classes for dbm.cfg handle double qoutes correctly
        {
            ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseAFileIsMissingOrUnreadable, PathOfNSR_ENVFile));
            IsInstalled=0;
        }
        else
        {
            char * PathOfNSR_HOMEDirectory=0;
            char * NSRFullLevelAsString=0;
            char * NSRNotVerboseAsString=0;
            char * NSREBIDTypeAsString=0;
            char * NSROutputScanAsString=0;
            char * NSRIgnoreError=0;
            char * NSRSaveIntervalAsString=0;
            char * NSRAvoidMMInfoBugAsString=0;
            char * NSRSaveTimeFormatAsString=0;
            char * NSRRoundupAsString=0;
            char * NSRUseLLOptionForSaveAsString=0;
            char * NSRUseDotNSRFileAsString=0;
            char * NSRUseClientForAsString=0;
            char * NSRAvoidVSSBugAsString=0;
            char * NSRAdditionalSaveParametersDummy=0;
            char * NSRAdditionalMMInfoParametersDummy=0;
            char * NSRAdditionalRecoverParametersDummy=0;

            tni34_ATextFile NSR_ENVFile(PathOfNSR_ENVFile, tni34_AFile::FromDisk, 0);

            if(NSR_ENVFile.IsGood())
            {
                const char *Line;

                tcn352_NSREnvFileEntry AcceptedNSREnvFileEntries[]={{"NSR_HOME",          &PathOfNSR_HOMEDirectory},
                                                                    {"NSR_HOST",          &NameOfNSR_HOST},
                                                                    {"NSR_POOL",          &NameOfNSR_POOL},
                                                                    {"NSR_EXPIRE",        &NSR_EXPIREValue},
                                                                    {"NSR_FULL",          &NSRFullLevelAsString},
                                                                    {"NSR_NOTVERBOSE",    &NSRNotVerboseAsString},
                                                                    {"NSR_EBIDTYPE",      &NSREBIDTypeAsString},
                                                                    {"NSR_USEOUTPUT",     &NSROutputScanAsString},
                                                                    {"NSR_IGNOREERROR",   &NSRIgnoreError},
                                                                    {"NSR_SAVEINTERVAL",  &NSRSaveIntervalAsString},
                                                                    {"NSR_BACKUPNAME",    &NSRBackupName},
                                                                    {"NSR_AVOIDPSSIDBUG", &NSRAvoidMMInfoBugAsString},
                                                                    {"NSR_SAVETIMEFORMAT",&NSRSaveTimeFormatAsString},
                                                                    {"NSR_SIZEROUNDUP",   &NSRRoundupAsString},
                                                                    {"NSR_AVOIDTIMEBUG",  &NSRAvoidMMInfoTimeBug},
                                                                    {"NSR_USELLOPTION",   &NSRUseLLOptionForSaveAsString},
                                                                    {"NSR_CLIENT",        &NSRClientName},
                                                                    {"NSR_USE.NSRFILE",   &NSRUseDotNSRFileAsString},
                                                                    {"NSR_USECLIENTFOR",  &NSRUseClientForAsString},
                                                                    {"NSR_AVOIDVSSBUG",   &NSRAvoidVSSBugAsString},
                                                                    {"NSR_ADDITIONAL_SAVE_PARAMETERS",    &NSRAdditionalSaveParametersDummy},
                                                                    {"NSR_ADDITIONAL_MMINFO_PARAMETERS",  &NSRAdditionalMMInfoParametersDummy},
                                                                    {"NSR_ADDITIONAL_RECOVER_PARAMETERS", &NSRAdditionalRecoverParametersDummy},
                                                                    {0,                   0}};

                while(rc && NSR_ENVFile.ReadLine(Line) && Line!=0)
                {
                    const char * NSRStart=cn36_FirstNonWhiteSpaceOf(Line); // ignore white spaces

                    if(!strncmp(NSRStart, "NSR_", strlen("NSR_"))) // all meaningful lines start with NSR_
                    {
                        char **Destination=0;
                        tcn352_NSREnvFileEntry * CurrentNSREnvFileEntry=AcceptedNSREnvFileEntries;

                        while(0==Destination && 0!=CurrentNSREnvFileEntry->KeyWord)
                        {
                            if(0==strncmp(NSRStart, CurrentNSREnvFileEntry->KeyWord, strlen(CurrentNSREnvFileEntry->KeyWord)))
                                Destination=CurrentNSREnvFileEntry->Destination;
                            else
                                CurrentNSREnvFileEntry++;
                        }

                        if(0==Destination)
                        {
                            LogWriter->Write("The following line of NetWorker's variables file has an unknown format and is ignored:\n", tcn35d_DBBackupControllerLogWriter::AddALevelToPrefix);
                            LogWriter->Write(Line);
                            LogWriter->Write("\n", tcn35d_DBBackupControllerLogWriter::NoPrefix);
                            LogWriter->Write(tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefix);
                        }
                        else
                        {
                            const char *StartOfValue=cn36_FirstNonWhiteSpaceOf(cn36_FirstWhiteSpaceOf(NSRStart)); //ignore keyword and following whitespaces
                            const char *EndOfValue;
                            int         FoundValue=0;

                            if(Destination==&NSRAdditionalSaveParametersDummy ||
                               Destination==&NSRAdditionalMMInfoParametersDummy ||
                               Destination==&NSRAdditionalRecoverParametersDummy  )
                            {
                                EndOfValue=cn36_StartOfTrailingWhiteSpaces(StartOfValue); //take the complete line
                            }
                            else
                                cn36_GetStringBetweenDoubleQuotesOrWhiteSpaces(StartOfValue, EndOfValue);

                            if(rc)
                            {
                                if('\0'==StartOfValue[0])   // empty strings are not useful for parameters
                                {
                                    LogWriter->Write("The following line of NetWorker's variables file contains a format error and is ignored:\n", tcn35d_DBBackupControllerLogWriter::AddALevelToPrefix);
                                    LogWriter->Write(Line);
                                    LogWriter->Write("\n", tcn35d_DBBackupControllerLogWriter::NoPrefix);
                                    LogWriter->Write(tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefix);
                                }
                                else
                                {
                                    rc=cn36_StrNAllocCpy(*Destination, StartOfValue, (EndOfValue-StartOfValue));

                                    if(rc)
                                    {
                                        sprintf(MsgBuf_cn36,"Found NetWorker setting for '%s': '%s'.\n", CurrentNSREnvFileEntry->KeyWord, *Destination);
                                        LogWriter->Write(MsgBuf_cn36);
                                    }

                                    if(rc && Destination==&NSRIgnoreError)  // special handling for the list of errors to ignore
                                    {
                                        if((0==ListOfErrorsToIgnore.Add(*Destination)))   // try to add the pointer to the string in the list
                                        {
                                            ErrorHandler->Report(tcn35d_DBBackupError::OutOfMemory.Set());
                                            rc=0;
                                        }
                                        else
                                            *Destination=0;  //forget about the string here to avoid freeing it (a pointer to the string is now in the list)
                                    }

                                    if(rc && Destination==&NSRAdditionalSaveParametersDummy) // special handling for the list of additional save arguments
                                        rc=calculateAndStoreAdditionalParameters(*Destination, m_NSRAdditionalSaveParameters);

                                    if(rc && Destination==&NSRAdditionalMMInfoParametersDummy) // special handling for the list of additional save arguments
                                        rc=calculateAndStoreAdditionalParameters(*Destination, m_NSRAdditionalMMInfoParameters);

                                    if(rc && Destination==&NSRAdditionalRecoverParametersDummy) // special handling for the list of additional save arguments
                                        rc=calculateAndStoreAdditionalParameters(*Destination, m_NSRAdditionalRecoverParameters);
                                }
                            }    
                        }
                    }
                }
            }
            else
                rc=0;

            if(rc)  // IsInstalled==1 holds allways
            {
                if(0==PathOfNSR_HOMEDirectory || '\0'==PathOfNSR_HOMEDirectory[0])
                {
                    IsInstalled=0;
                    ErrorHandler->Report(tcn352_NSRError(tcn352_NSRError::CaseCanNotDetermineNSR_HOME, PathOfNSR_ENVFile));
                }

                if(0==NameOfNSR_HOST || '\0'==NameOfNSR_HOST)
                {
                    IsInstalled=0;
                    ErrorHandler->Report(tcn352_NSRError(tcn352_NSRError::CaseCanNotDetermineNSR_HOST, PathOfNSR_ENVFile));
                }
            }

            if(rc && IsInstalled)
            {
                tni34_ADirectory TheNSR_HOMEDirectory(PathOfNSR_HOMEDirectory, 0, 0); // do not create or check now

                rc=TheNSR_HOMEDirectory.IsGood();   //check if directory object was constructed successful
                
                if(rc && !TheNSR_HOMEDirectory.IsADirectory())
                {
                    IsInstalled=0;

                    ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseADirectoryIsMissing, TheNSR_HOMEDirectory.GiveName()));

                    sprintf(MsgBuf_cn36, "The NetWorker directory '%s' is missing or no directory.\n", TheNSR_HOMEDirectory.GiveName());
                    LogWriter->Write(MsgBuf_cn36);
                }

                if(rc && IsInstalled)
                {
                    #if defined (WIN32)
                    const char *NameOfSave="\\save.exe";
                    const char *NameOfRecover="\\recover.exe";
                    const char *NameOfMminfo="\\mminfo.exe";

                    #elif defined (UNIX)
                    const char *NameOfSave="/save";
                    const char *NameOfRecover="/recover";
                    const char *NameOfMminfo="/mminfo";

                    #else
                        #error tcn352_NetWorkerConnector::ToolIsInstalled() was only coded for UNIX and NT (define one of them).
                    #endif

                    rc=AllocConstructAndCheckExe(PathOfNSRSave, TheNSR_HOMEDirectory.GiveName(), NameOfSave, IsInstalled);

                    if(rc && IsInstalled)
                        rc=AllocConstructAndCheckExe(PathOfNSRRecover, TheNSR_HOMEDirectory.GiveName(), NameOfRecover, IsInstalled);

                    if(rc && IsInstalled)
                        rc=AllocConstructAndCheckExe(PathOfNSRMminfo, TheNSR_HOMEDirectory.GiveName(), NameOfMminfo, IsInstalled);

                    if(rc && IsInstalled)
                    {
                        LogWriter->Write("Using NetWorker programs:\n", tcn35d_DBBackupControllerLogWriter::AddALevelToPrefix);
                        sprintf(MsgBuf_cn36, "'%s'\n", PathOfNSRSave);
                        LogWriter->Write(MsgBuf_cn36);
                        sprintf(MsgBuf_cn36, "'%s'\n", PathOfNSRRecover);
                        LogWriter->Write(MsgBuf_cn36);
                        sprintf(MsgBuf_cn36, "'%s'\n", PathOfNSRMminfo);
                        LogWriter->Write(MsgBuf_cn36);
                        LogWriter->Write(tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefix);
                    }
                }
            }

            if(rc && IsInstalled)
            {
                char * NameOfNSR_POOLFromEnvironment=0;

                rc=DetermineValueOfEnvironmentVariable("NSR_POOL", NameOfNSR_POOLFromEnvironment, "NetWorker pool", BackupInfo->DBName(), 0);

                if(rc)
                {
                    if('\0'!=NameOfNSR_POOLFromEnvironment[0])                          // values from the environment are used instead of values from the configuration file
                        rc=cn36_StrAllocCpy(NameOfNSR_POOL, NameOfNSR_POOLFromEnvironment);
                    else
                    {
                        if(0==NameOfNSR_POOL)                       // if we have no value at all, it is the last chance to init NameOfNSR_POOL as required by the following code
                            rc=cn36_StrAllocCpy(NameOfNSR_POOL, "");
                    }
                }

                cn36_StrDealloc(NameOfNSR_POOLFromEnvironment);
            }

            if(rc && IsInstalled)
            {
                char * NSR_EXPIREValueFromEnvironment=0;

                rc=DetermineValueOfEnvironmentVariable("NSR_EXPIRE", NSR_EXPIREValueFromEnvironment, "NetWorker expiry period", BackupInfo->DBName(), 0);

                if(rc)
                {
                    if('\0'!=NSR_EXPIREValueFromEnvironment[0])
                        rc=cn36_StrAllocCpy(NSR_EXPIREValue, NSR_EXPIREValueFromEnvironment);
                    else
                    {
                        if(0==NSR_EXPIREValue)
                            rc=cn36_StrAllocCpy(NSR_EXPIREValue, "");
                    }
                }

                cn36_StrDealloc(NSR_EXPIREValueFromEnvironment);
            }

            if(rc && IsInstalled)
            {
                if(0!=NSRFullLevelAsString && '\0'!=NSRFullLevelAsString[0])
                {
                    if(0==strcmp(NSRFullLevelAsString, "SAVEDATA") ||
                       0==strcmp(NSRFullLevelAsString, "DATA MIGRATION"))
                    {
                        LevelFullForBackupType=DataMigration;
                    }
                    else
                        if(0==strcmp(NSRFullLevelAsString, "SAVEDTNCHK") ||
                           0==strcmp(NSRFullLevelAsString, "DATA RECOVERY"))
                        {
                            LevelFullForBackupType=DataRecovery;
                        }
                        else
                            if(0==strcmp(NSRFullLevelAsString, "SAVEDATA+SAVEDTNCHK") ||
                               0==strcmp(NSRFullLevelAsString, "DATA MIGRATION+DATA RECOVERY") ||
                               0==strcmp(NSRFullLevelAsString, "DATA RECOVERY+DATA MIGRATION"))
                            {
                                LevelFullForBackupType=DataMigrationAndDataRecovery;
                            }
                            else
                            {
                                sprintf(MsgBuf_cn36, "The unknown value '%s' for variable 'NSR_FULL' is ignored.\n", NSRFullLevelAsString);
                                LogWriter->Write(MsgBuf_cn36);
                            }
                }
                else
                    LevelFullForBackupType=DataMigration;

                if(0!=NSRNotVerboseAsString && '\0'!=NSRNotVerboseAsString[0])
                {
                    if(0==strcmp(NSRNotVerboseAsString, "NONE"))
                        VerboseFlagFor=SaveAndRecover;
                    else
                        if(0==strcmp(NSRNotVerboseAsString, "SAVE"))
                            VerboseFlagFor=Recover;
                        else
                            if(0==strcmp(NSRNotVerboseAsString, "RECOVER"))
                                VerboseFlagFor=Save;
                            else
                                if(0==strcmp(NSRNotVerboseAsString, "SAVE AND RECOVER"))
                                    VerboseFlagFor=None;
                                else
                                {
                                    sprintf(MsgBuf_cn36, "The unknown value '%s' for variable 'NSR_NOTVERBOSE' is ignored.\n", NSRNotVerboseAsString);
                                    LogWriter->Write(MsgBuf_cn36);
                                }
                }
                else
                    VerboseFlagFor=SaveAndRecover;

                if(0!=NSREBIDTypeAsString && '\0'!=NSREBIDTypeAsString[0])
                {
                    if(0==strcmp(NSREBIDTypeAsString, "SSID"))
                        NSREBIDType=SSID;
                    else
                        if(0==strcmp(NSREBIDTypeAsString, "NSAVETIME"))
                            NSREBIDType=NSaveTime;
                        else
                        {
                            sprintf(MsgBuf_cn36, "The unknown value '%s' for variable 'NSR_EBIDTYPE' is ignored.\n", NSREBIDTypeAsString);
                            LogWriter->Write(MsgBuf_cn36);
                        }
                }
                else
                    NSREBIDType=NSaveTime;

                if(0!=NSROutputScanAsString && '\0'!=NSROutputScanAsString[0])
                {
                    if(0==strcmp(NSROutputScanAsString, "SAVE"))
                        UseNSROutput=1;
                    else
                        if(0==strcmp(NSROutputScanAsString, "NONE"))
                            UseNSROutput=0;
                }
                else
                    UseNSROutput=1;

                if(0!=NSRSaveIntervalAsString && '\0'!=NSRSaveIntervalAsString[0])
                {
                    if(!cn36_StrToInt(NSRSaveInterval, NSRSaveIntervalAsString))
                    {
                        sprintf(MsgBuf_cn36, "The value '%s' for variable 'NSR_SAVEINTERVAL' is ignored.\n", NSRSaveIntervalAsString);
                        LogWriter->Write(MsgBuf_cn36);
                        NSRSaveInterval=10;
                    }
                    else
                    {
                        if(0>NSRSaveInterval)
                        {
                            NSRSaveInterval=0;
                            sprintf(MsgBuf_cn36, "The value '%s' for variable 'NSR_SAVEINTERVAL' was changed to %d.\n", NSRSaveIntervalAsString, int(NSRSaveInterval));
                            LogWriter->Write(MsgBuf_cn36);
                        }
                    }
                }
                else
                    NSRSaveInterval=10;

                if(0!=NSRBackupName && '\0'!=NSRBackupName[0])
                {
                    const char * DBNamePlaceHolder="<database_name>";
                    const char * DBNodePlaceHolder="<node>";
                    const char * DBName=BackupInfo->DBName();
                    const char * DBNode=NodeNameToUse();
                    
                    char * NewNSRBackupName=0;
                    
                    unsigned NumberOfDBNamePlaceHolder=cn352_NumberOfSubStringInString(NSRBackupName, DBNamePlaceHolder),
                             NumberOfDBNodePlaceHolder=cn352_NumberOfSubStringInString(NSRBackupName, DBNodePlaceHolder);

                    size_t Length=strlen(NSRBackupName);

                    if(tcn35d_BackupInfo::GiveBackupInfo==BackupInfo->DataDirection())
                        DBName=BackupInfo->SourceDBName();

                    Length-=NumberOfDBNamePlaceHolder*strlen(DBNamePlaceHolder);
                    Length+=NumberOfDBNamePlaceHolder*strlen(DBName);

                    Length-=NumberOfDBNodePlaceHolder*strlen(DBNodePlaceHolder);
                    Length+=NumberOfDBNodePlaceHolder*strlen(DBNode);

                    if(!cn36_StrAlloc(NewNSRBackupName, Length))
                        rc=0;
                    else
                    {
                        const char *PositionOld=NSRBackupName;
                        char *PositionNew=NewNSRBackupName;

                        while('\0'!=(*PositionOld))
                        {
                            if(0==strncmp(DBNamePlaceHolder, PositionOld, strlen(DBNamePlaceHolder)))   //we have reached a "<database_name>" in the old string
                            {
                                SAPDB_strcpy(PositionNew, DBName);                                            //so insert the actual DBName in the new string
                                PositionOld+=strlen(DBNamePlaceHolder);
                                PositionNew+=strlen(DBName);
                            }
                            else
                                if(0==strncmp(DBNodePlaceHolder, PositionOld, strlen(DBNodePlaceHolder)))   //we have reached a "<node>" in the old string
                                {
                                    SAPDB_strcpy(PositionNew, DBNode);                                            //so insert the actual node name in the new string
                                    PositionOld+=strlen(DBNodePlaceHolder);
                                    PositionNew+=strlen(DBNode);
                                }
                                else
                                    *(PositionNew++)=(*(PositionOld++));    //just copy the character and move the positions in the string
                        }

                        *PositionNew='\0';  //set a terminating zero in the new string

                        if(strlen(NewNSRBackupName)!=Length)
                        {
                            rc=0;
                            ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
                            cn36_StrDealloc(NewNSRBackupName);
                        }
                        else
                        {
                            cn36_StrDealloc(NSRBackupName);
                            NSRBackupName=NewNSRBackupName;
                        }
                    }
                }
                else
                    if(tcn35d_BackupInfo::GiveBackupInfo==BackupInfo->DataDirection())
                        rc=cn36_StrAllocCpy(NSRBackupName, BackupInfo->SourceDBName());
                    else
                        rc=cn36_StrAllocCpy(NSRBackupName, BackupInfo->DBName());
            }

            if(rc && IsInstalled)
            {
                if(0!=NSRAvoidMMInfoBugAsString &&
                   '\0'!=NSRAvoidMMInfoBugAsString[0] &&
                   0==strcmp(NSRAvoidMMInfoBugAsString, "YES"))
                {
                    NSRAvoidMMInfoBug=1;
                }
                else
                    NSRAvoidMMInfoBug=0;
                
                if(0!=NSRSaveTimeFormatAsString && '\0'!=NSRSaveTimeFormatAsString[0])
                {
                    UseExplicitDateTimeFormat=1;
                    ExplicitDateTimeFormat=NSRSaveTimeFormatAsString;

                    if(ExplicitDateTimeFormat.isAFaultyObject())
                        rc=0;
                }
                else
                {
                    UseExplicitDateTimeFormat=0;
                    ExplicitDateTimeFormat="";

                    if(ExplicitDateTimeFormat.isAFaultyObject())
                        rc=0;
                }
            }

            if(rc && IsInstalled && 0!=NSRRoundupAsString && '\0'!=NSRRoundupAsString[0])
            {
                if(0==strcmp(NSRRoundupAsString, "NOROUNDUP"))
                    Roundup=NoRoundup;
                else
                    if(0==strcmp(NSRRoundupAsString, "FIRSTDECIMAL"))
                        Roundup=FirstDecimal;
                    else
                        if(0==strcmp(NSRRoundupAsString, "NORMAL"))
                            Roundup=Normal;
                        else
                            if(0==strcmp(NSRRoundupAsString, "LASTINTEGER"))
                                Roundup=LastInteger;
            }

            if(rc && IsInstalled)
            {
                if(0!=NSRUseLLOptionForSaveAsString && '\0'!=NSRUseLLOptionForSaveAsString[0])
                {
                    if(0==cn36_StrUprCmp(NSRUseLLOptionForSaveAsString, "YES"))
                        UseLLOptionForSave=1;
                    else
                        if(0==cn36_StrUprCmp(NSRUseLLOptionForSaveAsString, "NO"))
                            UseLLOptionForSave=0;
                        else
                        {
                            sprintf(MsgBuf_cn36, "The unknown value '%s' for variable 'NSR_USELLOPTION' is ignored.\n", NSRUseLLOptionForSaveAsString);
                            LogWriter->Write(MsgBuf_cn36);
                        }
                }
                else
                    UseLLOptionForSave=1;
            }

            if(rc && IsInstalled)
            {
                NSRUseDotNSRFile=0;

                if(0!=NSRUseDotNSRFileAsString)
                {
                    if(0==cn36_StrUprCmp(NSRUseDotNSRFileAsString, "YES"))
                        NSRUseDotNSRFile=1;
                    else
                        if(0!=cn36_StrUprCmp(NSRUseDotNSRFileAsString, "NO"))
                        {
                            sprintf(MsgBuf_cn36, "The unknown value '%s' for variable 'NSR_USE.NSRFILE' is ignored.\n", NSRUseDotNSRFileAsString);
                            LogWriter->Write(MsgBuf_cn36);
                        }
                }
            }

            if(rc && IsInstalled)
            {
                NSROptionUseClientFor=tcn352_NetWorkerConnector::UseForMminfo;

                if(0!=NSRUseClientForAsString) //found a string for NSRUseClientFor in the config file
                {
                    if(0==NSRClientName) // if NSR_CLIENT is not set NSR_USECLIENTFOR has no effect
                    {
                        sprintf(MsgBuf_cn36, "The value '%s' for variable 'NSR_USECLIENTFOR' has no effect, because variable 'NSR_CLIENT' is not set.\n", NSRUseClientForAsString);
                        LogWriter->Write(MsgBuf_cn36);
                    }
                    else
                    {
                        if(0==cn36_StrUprCmp(NSRUseClientForAsString, "SAVE AND MMINFO"))
                            NSROptionUseClientFor=tcn352_NetWorkerConnector::UseForSaveAndMMInfo;
                        else
                            if(0!=cn36_StrUprCmp(NSRUseClientForAsString, "MMINFO"))
                            {
                                sprintf(MsgBuf_cn36, "The unknown value '%s' for variable 'NSR_USECLIENTFOR' is ignored. The default 'MMINFO' is used instead.\n", NSRUseClientForAsString);
                                LogWriter->Write(MsgBuf_cn36);
                            }
                    }
                }
            }

            if(rc && IsInstalled)
            {
                if(0!=NSRAvoidVSSBugAsString &&
                   '\0'!=NSRAvoidVSSBugAsString[0] &&
                   0==cn36_StrUprCmp(NSRAvoidVSSBugAsString, "YES"))
                {
                    m_NSRAvoidVSSBug=true;
                }
                else
                    m_NSRAvoidVSSBug=false;
            }

            if(rc && IsInstalled)
            {
                m_NSRAdditionalSaveParameters.Reverse();
                m_NSRAdditionalMMInfoParameters.Reverse();
                m_NSRAdditionalRecoverParameters.Reverse();
            }

            cn36_StrDealloc(PathOfNSR_HOMEDirectory);
            cn36_StrDealloc(NSRFullLevelAsString);
            cn36_StrDealloc(NSRNotVerboseAsString);
            cn36_StrDealloc(NSREBIDTypeAsString);
            cn36_StrDealloc(NSROutputScanAsString);
            cn36_StrDealloc(NSRIgnoreError);
            cn36_StrDealloc(NSRSaveIntervalAsString);
            cn36_StrDealloc(NSRAvoidMMInfoBugAsString);
            cn36_StrDealloc(NSRSaveTimeFormatAsString);
            cn36_StrDealloc(NSRRoundupAsString);
            cn36_StrDealloc(NSRUseLLOptionForSaveAsString);
            cn36_StrDealloc(NSRUseDotNSRFileAsString);
            cn36_StrDealloc(NSRUseClientForAsString);
            cn36_StrDealloc(NSRAvoidVSSBugAsString);
            cn36_StrDealloc(NSRAdditionalSaveParametersDummy);
            cn36_StrDealloc(NSRAdditionalMMInfoParametersDummy);
            cn36_StrDealloc(NSRAdditionalRecoverParametersDummy);
        }

    LogWriter->Write((rc && IsInstalled)?"Check passed successfully.\n":"Check failed.\n", tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefixAndEndParagraph);

    return rc;
}

//some constants for "hard coded" level manipulations
static const int    DataMigrationIndex_cn36=0;
static const int    DataRecoveryIndex_cn36=3;
static const int    DataMigrationParallelIndex_cn36=5;

static const unsigned int NumberOfDataTypeToNSRLevelMapEntries=6;

tcn352_NetWorkerConnector::DataTypeToNSRLevelMapEntry tcn352_NetWorkerConnector::DataTypeToNSRLevelMap[]=
{
    {DataMigrationIndex_cn36,         tcn35d_BackupInfo::DataMigration,  "full"},
    {1,                               tcn35d_BackupInfo::PagesMigration, "incr"},
    {2,                               tcn35d_BackupInfo::Log,            "1"},
    //{                                                                  "2", "SAVELOGSEG"}, obsolete since 6.2.8, but do not reuse this level as long as possible
    {DataRecoveryIndex_cn36,          tcn35d_BackupInfo::DataRecovery,   "3"},
    {4,                               tcn35d_BackupInfo::PagesRecovery,  "4"},
    //{                                                                  "5"},  //                      "SAVEHISTORY"},
    {DataMigrationParallelIndex_cn36, tcn35d_BackupInfo::DataMigration,  "6"}
};

const char * tcn352_NetWorkerConnector::LevelForAction(tcn35d_BackupInfo::ADataType DataType,
                                                       int                          NumberOfPipe)
{
    const char *rc=0;

    unsigned i=0;
    int      Found=0;

    while(!Found && i<NumberOfDataTypeToNSRLevelMapEntries)
    {
        if(DataTypeToNSRLevelMap[i].DataType==DataType)
            Found=1;
        else
            i++;
    }

    if(Found)
    {
        i=IndexAccordingToNSRFullSetting(i);    //take the setting of NSRFULL from the configuration file into account for DataMigration/DataRecovery
        
        if(DataMigrationIndex_cn36==i && 1<=NumberOfPipe)   //if level would be "full", all pipe with number unequal 0 get level "6"
            i=DataMigrationParallelIndex_cn36;

        rc=DataTypeToNSRLevelMap[i].NSRLevel; //return the determined and "corrected" level
    }
    else
        ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));

    return rc;
}

unsigned int tcn352_NetWorkerConnector::IndexAccordingToNSRFullSetting(unsigned int CurrentIndex)
{
    switch(LevelFullForBackupType)
    {
        case DataRecovery:  //substitute FullMigrationIndex_cn36 and FullRecoveryIndex_cn36 with each other
        {
            if(CurrentIndex==DataMigrationIndex_cn36)
                CurrentIndex=DataRecoveryIndex_cn36;
            else
                if(CurrentIndex==DataRecoveryIndex_cn36)
                    CurrentIndex=DataMigrationIndex_cn36;
        }
        break;

        case DataMigrationAndDataRecovery:  //chnage FullRecoveryIndex_cn36 to FullMigrationIndex_cn36
        {
            if(tcn35d_BackupInfo::ABackup==BackupInfo->DataDirection())   //on a backup...
            {
                if(CurrentIndex==DataRecoveryIndex_cn36)    //... give Data Recovery ...
                    CurrentIndex=DataMigrationIndex_cn36;   //... the same level as Data Migration
            }
            else
                if(tcn35d_BackupInfo::GiveBackupInfo==BackupInfo->DataDirection())            //but after the backup ...
                {
                    if(CurrentIndex==DataMigrationIndex_cn36)   //... all Data Migration have to be considered as ...
                        CurrentIndex=DataRecoveryIndex_cn36;    //... Data Recovery (a Data Migration can always used like a Data Recovery, but not vice versa)
                }
                //else do nothing
        }
        break;

        case DataMigration: //do nothing
        default:
        break;
    }

    return CurrentIndex;
}

int tcn352_NetWorkerConnector::DataTypeForLevelN(tcn35d_BackupInfo::ADataType & DataType,
                                                 const char *                   Level,
                                                 size_t                         LevelLength)
{
    unsigned i=0;
    int      Found=0;

    //determine the action for the level
    while(!Found && i<NumberOfDataTypeToNSRLevelMapEntries)                             // not at end of list
    {
        if(0==cn36_StrStrNCmp(DataTypeToNSRLevelMap[i].NSRLevel, Level, LevelLength))   
            Found=1;
        else                                                                            // strings differ in length or in letters
            i++;
    }

    if(Found)
    {
        //take care of relationship of levels "full" and "6"
        if(DataMigrationParallelIndex_cn36==i)  //if the level equals "6" ...
            i=DataMigrationIndex_cn36;          //...simulate the level "full" to get the correct result

        //take care of NSR_FULL settings
        i=IndexAccordingToNSRFullSetting(i);

        DataType=DataTypeToNSRLevelMap[i].DataType;
    }

    return Found;
}

int tcn352_NetWorkerConnector::CreateNSRDirectivesFile()
{
    int rc=1;

    if(tcn35d_BackupInfo::ABackup==BackupInfo->DataDirection()) //write NetWorkers directives file for a backup
    {
        #if defined(WIN32)
        //do not use any directive files on Windows
        NSRUseDotNSRFile=1; //avoid usage of NetWorkers save with "-f"
        #elif defined(UNIX)
        int CurrentDFile;
        int NumberOfDFiles=BackupInfo->NumberOfPipes();

        if(NSRUseDotNSRFile)    //if .nsr should be used
            NumberOfDFiles=1;   //only one .nsr file should be used

        while(!DirectiveFileNames.empty())  //empty the vector of directives file names
        {
            cn36_StrDealloc(DirectiveFileNames.back());
            DirectiveFileNames.pop_back();
        }

        for(CurrentDFile=0; CurrentDFile<NumberOfDFiles; CurrentDFile++)    //for all pipes create a file <path>/.<pipename>
        {
            char *       NewDFileName=0;
            const char * EndOfDFileDirectory=cn36_BaseNameFrom(BackupInfo->NameOfPipe(CurrentDFile));

            if(NSRUseDotNSRFile)                                                                                            //if one file .nsr should be used...
            {
                if(!cn36_StrAlloc(NewDFileName, EndOfDFileDirectory-BackupInfo->NameOfPipe(CurrentDFile)+strlen(".nsr")))   // the name is <path>/.nsr
                    rc=0;
            }
            else
                if(!cn36_StrAlloc(NewDFileName,                                                                             //otherwise it's <path>/.<pipename>
                                  EndOfDFileDirectory-BackupInfo->NameOfPipe(CurrentDFile)
                                  +strlen(".")
                                  +strlen(cn36_BaseNameFrom(BackupInfo->NameOfPipe(CurrentDFile)))))
                {
                    rc=0;
                }

            if(rc)
            {
                cn36_StrNCpy(NewDFileName, BackupInfo->NameOfPipe(CurrentDFile), EndOfDFileDirectory-BackupInfo->NameOfPipe(CurrentDFile));

                if(NSRUseDotNSRFile)
                    strcat(NewDFileName, ".nsr");
                else
                {
                    strcat(NewDFileName, ".");
                    strcat(NewDFileName, cn36_BaseNameFrom(BackupInfo->NameOfPipe(CurrentDFile)));
                }

                DirectiveFileNames.push_back(NewDFileName);
            }

            if(rc)
                LogWriter->Write(tcn35d_DBBackupControllerLogWriter::EndParagraph);

            if(rc) //if we could construct the file name
            {
                tni34_ATextFile DFile(NewDFileName, tni34_AFile::ToDiskTruncate);

                if(DFile.IsGood())
                {
                    int          i;
                    const char * RawASMDirective="rawasm:";
                    char *       Line=0;
                    size_t       LineLength=strlen(RawASMDirective);

                    if(!NSRUseDotNSRFile)   //if a directive file for every pipe is used, it must start with a line like "<< "+<dirname>+" >>"
                    {
                        size_t DirLength=strlen("<< ")+strlen(" >>");
                        
                        if(0==EndOfDFileDirectory-BackupInfo->NameOfPipe(CurrentDFile))             //if pipe has only a relative name
                            DirLength+=strlen(".");                                                 //use "." as <dirname>
                        else
                            DirLength+=(EndOfDFileDirectory-BackupInfo->NameOfPipe(CurrentDFile));  //otherwise use the directory of the pipe

                        if(!cn36_StrAlloc(Line, DirLength))
                            rc=0;
                        else
                        {
                            SAPDB_strcpy(Line, "<< ");

                            if(0==EndOfDFileDirectory-BackupInfo->NameOfPipe(CurrentDFile))
                                strcat(Line, ".");
                            else
                                cn36_StrNCpy(Line+strlen("<< "), BackupInfo->NameOfPipe(CurrentDFile), EndOfDFileDirectory-BackupInfo->NameOfPipe(CurrentDFile));

                            strcat(Line, " >>");

                            rc=CreateNSRDirectivesFileWriteALine(DFile, Line, NewDFileName);  //write Line to the directive file

                            cn36_StrDealloc(Line);  //forget about the current line
                        }
                    }

                    if(NSRUseDotNSRFile)
                    {
                        for(i=0; i<BackupInfo->NumberOfPipes(); i++)
                           LineLength+=(1+strlen(cn36_BaseNameFrom(BackupInfo->NameOfPipe(i))));
                    }
                    else
                        LineLength+=(1+strlen(cn36_BaseNameFrom(BackupInfo->NameOfPipe(CurrentDFile))));

                    if(cn36_StrAlloc(Line, LineLength))
                    {
                        SAPDB_strcpy(Line, RawASMDirective);

                        if(NSRUseDotNSRFile)
                        {
                            for(i=0; i<BackupInfo->NumberOfPipes(); i++)
                            {
                                strcat(Line, " ");
                                strcat(Line, cn36_BaseNameFrom(BackupInfo->NameOfPipe(i)));
                            }
                        }
                        else
                        {
                            strcat(Line, " ");
                            strcat(Line, cn36_BaseNameFrom(BackupInfo->NameOfPipe(CurrentDFile)));
                        }

                        rc=CreateNSRDirectivesFileWriteALine(DFile, Line, NewDFileName);

                        cn36_StrDealloc(Line);
                    }
                    else
                        rc=0;

                    if(!DFile.Close())
                        rc=0;
                }
                else
                    rc=0;
            }

            if(rc)
                sprintf(MsgBuf_cn36, "Constructed NetWorker's directives file '%s' successfully.\n", NewDFileName);
            else
                sprintf(MsgBuf_cn36, "Could not constructed NetWorker's directives file '%s'.\n", NewDFileName);

            LogWriter->Write(MsgBuf_cn36);
        }

        if(rc)
            LogWriter->Write(tcn35d_DBBackupControllerLogWriter::EndParagraph);

        #else
            #error tcn352_NetWorkerConnector::CreateNSRDirectivesFile() only coded for WIN32 and UNIX, define one of them
        #endif
    }

    return rc;
}

int tcn352_NetWorkerConnector::CreateNSRDirectivesFileWriteALine(tni34_ATextFile & DFile, const char * Line, const char *DFileName)
{
    int  rc=1;
    long BytesWritten;

    LogWriter->Write("Writing '");
    LogWriter->Write(Line, tcn35d_DBBackupControllerLogWriter::NoPrefix);
                        
    if(NSRUseDotNSRFile)
        LogWriter->Write("' to the .nsr file.\n", tcn35d_DBBackupControllerLogWriter::NoPrefix);
    else
    {
        sprintf(MsgBuf_cn36, "' to the directive file %s.\n", DFileName);
        LogWriter->Write(MsgBuf_cn36, tcn35d_DBBackupControllerLogWriter::NoPrefix);
    }

    if(!DFile.WriteLine(Line, BytesWritten))
        rc=0;

    return rc;
}


int tcn352_NetWorkerConnector::BuildBackupToolCall()
{
    int rc=1,
        i;

    if(NumberOfCommandLines!=0) //if old commandlines exists, free the memory
    {
        for(i=0; i<NumberOfCommandLines; i++)
        {
            ArgumentList[i].DeleteAll();

            if(0!=CommandLine[i])
            {
                delete [] CommandLine[i];
                CommandLine[i]=0;
            }
        }
        
        NumberOfCommandLines=0;
    }

    if(rc)  //construct the list of arguments for the NetWorker client to use
    {
        int    UseVerboseFlag=0;

        NSREBIDTypeEnum UsedEBIDType;   //only used for restore
        const char * StartNSavetime;    //only used for restore with EBID=="NST..."
        const char * EndNSavetime;      //only used for restore with EBID=="NST..."
        const char * StartOfSaveClient; //only used for restore with EBID=="NST..."
        const char * EndOfSaveClient;   //only used for restore with EBID=="NST..."
        const char * StartOfEBID;       //only used for restore
        const char * EndOfEBID;         //only used for restore

        NumberOfCommandLines=0;

        switch(BackupInfo->DataDirection())
        {
            case tcn35d_BackupInfo::ABackup:
            {
                UseVerboseFlag=(Save==VerboseFlagFor || SaveAndRecover==VerboseFlagFor);    //must the "-v" option be used

                Tools_Vector<char *>::iterator DFileName=DirectiveFileNames.begin();

                for(i=0; rc && i<BackupInfo->NumberOfPipes(); i++)
                {
                    if(!NSRUseDotNSRFile && DFileName==DirectiveFileNames.end())
                        rc=0;
                    else
                    {
                        if(!ArgumentList[i].AddArgument(PathOfNSRSave) ||                                   // "...\save.exe" or ".../save"
                           (UseVerboseFlag && !ArgumentList[i].AddArgument("-v")) ||                        // "-v"
                           !ArgumentList[i].AddArgument("-s") ||                                            // "-s"
                           !ArgumentList[i].AddArgument(NameOfNSR_HOST) ||                                  // "<NSR_HOST>"
                           (0!=NSRClientName &&
                            tcn352_NetWorkerConnector::UseForSaveAndMMInfo==NSROptionUseClientFor &&
                            !ArgumentList[i].AddArgument("-c")                                       ) ||   //"-c"
                           (0!=NSRClientName &&
                            tcn352_NetWorkerConnector::UseForSaveAndMMInfo==NSROptionUseClientFor &&
                            !ArgumentList[i].AddArgument(NodeNameToUse())                            ) ||   //"<save_client>"
                           ('\0'!=NameOfNSR_POOL[0] && !ArgumentList[i].AddArgument("-b")) ||               // "-b"
                           ('\0'!=NameOfNSR_POOL[0] && !ArgumentList[i].AddArgument(NameOfNSR_POOL)) ||     // "<NSR_POOL>"
                           ('\0'!=NSR_EXPIREValue[0] && !ArgumentList[i].AddArgument("-e")) ||              // "-e"
                           ('\0'!=NSR_EXPIREValue[0] && !ArgumentList[i].AddArgument(NSR_EXPIREValue)) ||   // "<NSR_EXPIRE>"
                           !ArgumentList[i].AddArgument("-N") ||                                            //"-N"
                           !ArgumentList[i].AddArgument(NSRBackupName) ||                                   //"<NSRBackupName>" // <NSRBackupName>==<dbname> if not configured otherwise
                           !ArgumentList[i].AddArgument("-l") ||                                            //"-l"
                           !ArgumentList[i].AddArgument(LevelForAction(BackupInfo->DataType(), i)) ||       //"<Level>"
                           (!NSRUseDotNSRFile && !ArgumentList[i].AddArgument("-f")) ||                     //"-f"
                           (!NSRUseDotNSRFile && !ArgumentList[i].AddArgument(*DFileName)) ||               //<dir file name>
                           (m_NSRAvoidVSSBug && !ArgumentList[i].AddArgument("-o")) ||                      //"-o"
                           (m_NSRAvoidVSSBug && !ArgumentList[i].AddArgument("\"VSS:*=off\"")))             //"\"VSS:*=off\""
                        {
                            rc=0;
                        }

                        if(rc) //add customer defined parameters for save
                            rc=addAdditionalParameters(ArgumentList[i], m_NSRAdditionalSaveParameters);

                        if(rc)
                        {
                            if((UseLLOptionForSave && !ArgumentList[i].AddArgument("-LL")) ||                   //"-LL"
                               !ArgumentList[i].AddArgument(BackupInfo->NameOfPipe(i)))                         // "<pipename>"
                            {
                                rc=0;
                            }
                        }      

                        if(!NSRUseDotNSRFile)
                            DFileName++;
                    }
                }

                if(rc)
                    NumberOfCommandLines=BackupInfo->NumberOfPipes();
            }break;

            case tcn35d_BackupInfo::ARestore:
            {
                EndOfEBID=ExternalBackupIDList;

                for(i=0; rc && i<BackupInfo->NumberOfPipes(); i++)
                {
                    if(','==EndOfEBID[0])
                        StartOfEBID=cn36_FirstNonWhiteSpaceOf(EndOfEBID+1);
                    else
                        StartOfEBID=cn36_FirstNonWhiteSpaceOf(EndOfEBID);
                    
                    EndOfEBID=strchr(StartOfEBID, ',');

                    if(0==EndOfEBID)
                        EndOfEBID=cn36_TerminatingZeroOf(StartOfEBID);

                    if('\0'==(*StartOfEBID))    //the current EBID is empty
                    {
                        rc=0;
                        ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseTooFewEBIDs, BackupInfo->NumberOfPipes()));
                    }

                    if(rc)
                    {   //determine the type of the EBID (TODO: should be separate function)
                        if(size_t(EndOfEBID-StartOfEBID)>strlen(NSaveTimeEBIDPrefix)+2+2*strlen(NSaveTimeEBIDPartDelimiter) &&                      //at least space for a NSaveTimeEBIDPrefix and two NSaveTimeEBIDPartDelimiter and 2 other chars (one for savetime and one for client)
                           0==cn36_StrNUprCmp(StartOfEBID, NSaveTimeEBIDPrefix, strlen(NSaveTimeEBIDPrefix)) &&                                     // the EBID starts with "NST"
                           0==strncmp(StartOfEBID+strlen(NSaveTimeEBIDPrefix), NSaveTimeEBIDPartDelimiter, strlen(NSaveTimeEBIDPartDelimiter)) &&   // which is followed with a NSaveTimeEBIDPartDelimiter
                           0!=strstr(StartOfEBID+strlen(NSaveTimeEBIDPrefix)+strlen(NSaveTimeEBIDPartDelimiter), NSaveTimeEBIDPartDelimiter))       // which is followed by something and another NSaveTimeEBIDPartDelimiter
                        {
                            UsedEBIDType=NSaveTime;
                            StartNSavetime=StartOfEBID+strlen(NSaveTimeEBIDPrefix)+strlen(NSaveTimeEBIDPartDelimiter);
                            EndNSavetime=strstr(StartNSavetime, NSaveTimeEBIDPartDelimiter);
                            StartOfSaveClient=EndNSavetime+strlen(NSaveTimeEBIDPartDelimiter);
                            EndOfSaveClient=EndOfEBID;
                        }
                        else
                            if(0<(EndOfEBID-StartOfEBID) &&
                               cn36_StrNConsistsOfDigits(StartOfEBID, (EndOfEBID-StartOfEBID)))
                            {
                                UsedEBIDType=SSID;
                            }
                            else
                            {
                                rc=0;
                                ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseSyntaxErrorEBIDs, i)); //it's the i-th element of the list
                            }

                        if(rc && UsedEBIDType!=NSREBIDType) //all OK so far but the determined EBIDType does not match the configured EBIDType (default: NSAVETIME)
                        {
                            rc=0;
                            char * CopyOfEBIDInQuestion=0;

                            if(!cn36_StrNAllocCpy(CopyOfEBIDInQuestion, StartOfEBID, (EndOfEBID-StartOfEBID)))  //try to create a copy of the EBID in question
                                ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::OutOfMemory));
                            else
                                ErrorHandler->Report(tcn352_NSRError(tcn352_NSRError::CaseWrongEBIDType, CopyOfEBIDInQuestion));

                            cn36_StrDealloc(CopyOfEBIDInQuestion);  //last chance of freeing the copy
                        }

                        if(rc && i+1==BackupInfo->NumberOfPipes())  //we have found the i-th valid EBID
                        {
                            if('\0'!=(*EndOfEBID))  // the list must contain only BackupInfo->NumberOfPipes() EBID's and nothing more, have a look what is comming
                            {                       //'\0'!=(*EndOfEBID)->','==(*EndOfEBID)
                                if('\0'!=(*cn36_FirstNonWhiteSpaceOf(EndOfEBID+1))) //a ',' followed by not only white spaces
                                {
                                    rc=0;
                                    ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseTooManyEBIDs, BackupInfo->NumberOfPipes()));
                                }
                            }
                        }

                        if(rc)
                        {
                            UseVerboseFlag=(Recover==VerboseFlagFor || SaveAndRecover==VerboseFlagFor);    //must the "-v"-option be used

                            if(ArgumentList[i].AddArgument(PathOfNSRRecover) &&             //"...\recover.exe" or ".../recover"
                               (!UseVerboseFlag || ArgumentList[i].AddArgument("-v")) &&    //"-v"
                               ArgumentList[i].AddArgument("-s") &&                         //"-s"
                               ArgumentList[i].AddArgument(NameOfNSR_HOST))                 //"<NSR_HOST>"
                            {
                                if(SSID==UsedEBIDType)
                                {
                                    if(!ArgumentList[i].AddArgument("-S") ||                                //"-S"
                                       !ArgumentList[i].AddArgument(StartOfEBID, EndOfEBID-StartOfEBID))    //"<EBID>"=="<SSID>"
                                    {
                                        rc=0;
                                    }
                                }
                                else
                                {
                                    if(!ArgumentList[i].AddArgument("-a") ||                                        //"-a"
                                       !ArgumentList[i].AddArgument("-t") ||                                        //"-t"
                                       !ArgumentList[i].AddArgument(StartNSavetime, EndNSavetime-StartNSavetime) || //"<nsavetime>"
                                       !ArgumentList[i].AddArgument("-c") ||                                        //"-c"
                                       !ArgumentList[i].AddArgument(StartOfSaveClient, EndOfSaveClient-StartOfSaveClient))  //"<save_client>"
                                    {
                                        rc=0;
                                    }
                                }

                                if(rc) //add customer defined parameters for recover
                                    rc=addAdditionalParameters(ArgumentList[i], m_NSRAdditionalRecoverParameters);

                                if(rc)
                                {
                                    if(!ArgumentList[i].AddArgument("-iY") ||               //"-iY"
                                       !ArgumentList[i].AddArgument(BackupInfo->NameOfPipe(i)))     //"<pipename>"
                                    {
                                        rc=0;
                                    }
                                }
                            }
                            else
                                rc=0;
                        }
                    }
                }

                if(rc)
                    NumberOfCommandLines=BackupInfo->NumberOfPipes();
            }break;

            case tcn35d_BackupInfo::GiveBackupInfo:
            {
                const char * reportColumnsPart1="name(26),pssid,level(4),savetime(";
                const char * reportColumnsPart2=0;
                char *       reportColumns=0;
                char         charactersForDataTimeAsString[30]="23"; //23 so that mminfo reports date and time with American time format "dd/mm/yyyy HH:MM:SS AM|PM", 

                // "calculate" the reportColumns argument according to a used NSR_SAVETIMEFORMAT
                if( UseExplicitDateTimeFormat && 0<ExplicitDateTimeFormat.getMaximalLengthOfTimeString() )
                {
                    int charactersForDateTime = int(2 + ExplicitDateTimeFormat.getMaximalLengthOfTimeString()); //2 needed by mminfo, don't ask me

                    sprintf(charactersForDataTimeAsString, "%d", int(charactersForDateTime));
                }

                if(SSID==NSREBIDType)
                    reportColumnsPart2="),totalsize,ssid";        //pssid -> we can "grep" for lines with pssid==0
                else
                    reportColumnsPart2="),totalsize,nsavetime";   //pssid -> we can "grep" for lines with pssid==0

                if (cn36_StrAlloc(reportColumns,
                                  strlen(reportColumnsPart1)+
                                  strlen(charactersForDataTimeAsString)+
                                  strlen(reportColumnsPart2)))
                {
                    SAPDB_strcpy(reportColumns, reportColumnsPart1);
                    SAPDB_strcat(reportColumns, charactersForDataTimeAsString);
                    SAPDB_strcat(reportColumns, reportColumnsPart2);
                }
                else
                    rc=0;

                //construct the argument list
                if(ArgumentList[0].AddArgument(PathOfNSRMminfo) &&                      //"...\mminfo.exe" or ".../mminfo"
                   ArgumentList[0].AddArgument("-a") &&                                 //"-a"
                   ArgumentList[0].AddArgument("-s") &&                                 //"-s"
                   ArgumentList[0].AddArgument(NameOfNSR_HOST) &&                       //"<NSR_HOST>"
                   ArgumentList[0].AddArgument("-c") &&                                 //"-c"
                   ArgumentList[0].AddArgument(NodeNameToUse()) &&                      //"<save_client>"
                   ArgumentList[0].AddArgument("-r") &&                                 //"-r"
                   ArgumentList[0].AddArgument(reportColumns) &&                        //"name(26),..."
                   ArgumentList[0].AddArgument("-o") &&                                 //"-o"
                   ArgumentList[0].AddArgument("t") &&                                  //"t"
                   (NSRAvoidMMInfoBug || ArgumentList[0].AddArgument("-q")) &&          //"-q"
                   (NSRAvoidMMInfoBug || ArgumentList[0].AddArgument("pssid=0")) &&     //"pssid=0"
                   (0==NSRAvoidMMInfoTimeBug || ArgumentList[0].AddArgument("-t")) &&
                   (0==NSRAvoidMMInfoTimeBug || ArgumentList[0].AddArgument(NSRAvoidMMInfoTimeBug)) &&
                   ArgumentList[0].AddArgument("-N") &&                                 //"-N"
                   ArgumentList[0].AddArgument(NSRBackupName))                          //"<NSRBackupName>" // <NSRBackupName>==<dbname> if not configured otherwise
                {
                    NumberOfCommandLines=1;
                }
                else
                    rc=0;

                if(rc) //add customer defined parameters for mminfo
                     rc=addAdditionalParameters(ArgumentList[0], m_NSRAdditionalMMInfoParameters);

                cn36_StrDealloc(reportColumns); //no longer needed, as ArgumentList is making copy's of strings added via AddArgument
            }break;

            default:
                ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
                rc=0;
        }

        for(i=0; rc && i<NumberOfCommandLines; i++)
        {
            if(0==ArgumentList[i].GiveCommandLine())
                rc=0;
            else
            {
                sprintf(MsgBuf_cn36, "Constructed NetWorker call '%s'.\n", ArgumentList[i].GiveCommandLine());
                LogWriter->Write(MsgBuf_cn36);
            }
        }
    }

    return rc;
}

int tcn352_NetWorkerConnector::CreateTempOutputFiles()
{
    int i,rc=1;

    for(i=0; rc && i<NumberOfCommandLines; i++)
    {
        ToolProcessOutput[i]=new tni34_AFile(tni34_AFile::ToDiskAppend, 0, 0); // do not create on open and do not open it now
        ToolProcessError[i]=new tni34_AFile(tni34_AFile::ToDiskAppend, 0, 0); // do not create on open and do not open it now

        if(0==ToolProcessOutput[i] || 0==ToolProcessError[i])
        {
            rc=0;
            ErrorHandler->Report(tcn35d_DBBackupError::OutOfMemory.Set());
        }
        else
        {
            if(ToolProcessOutput[i]->IsGood())
            {
                sprintf(MsgBuf_cn36, "Created temporary file '%s' as output for NetWorker.\n", ToolProcessOutput[i]->GiveName());
                LogWriter->Write(MsgBuf_cn36);
            }
            else
            {
                rc=0;

                delete ToolProcessOutput[i];    //avoid any problems with the incorrect file object
                ToolProcessOutput[i]=0;
            }

            if(ToolProcessError[i]->IsGood())
            {
                sprintf(MsgBuf_cn36, "Created temporary file '%s' as error output for NetWorker.\n", ToolProcessError[i]->GiveName());
                LogWriter->Write(MsgBuf_cn36);
            }
            else
            {
                rc=0;

                delete ToolProcessError[i];    //avoid any problems with the incorrect file object
                ToolProcessError[i]=0;
            }
        }
    }

    NSROutputResult=NotChecked;  //just "created" the NetWorker output files, so they are not checked so far

    for(i=0; i<MAX_NUMBER_OF_TOOLPROCESSES_CN35; i++)
    {
        cn36_StrDealloc(NSRExternalBackupID[i]);
        NSRNumberOfEBIDsFound=0;
        ErrorInOutput[i]=NotChecked;
    }

    return rc;
}

int tcn352_NetWorkerConnector::PrepareFirst()
{
    int rc=1;

    NumberOfRunningToolProcesses=0;

    switch(BackupInfo->DataDirection())
    {
        case tcn35d_BackupInfo::ABackup:
        case tcn35d_BackupInfo::ARestore:
        case tcn35d_BackupInfo::GiveBackupInfo:
            break;

        default:
        {
            ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
            rc=0;
        }break;
    }

    if(rc)
        rc=CreateNSRDirectivesFile();

    if(rc)
        rc=BuildBackupToolCall();

    if(rc)
        rc=CreateTempOutputFiles();

    return rc;
}

int tcn352_NetWorkerConnector::PrepareAgain()
{
    int rc;

    NumberOfRunningToolProcesses=0;

    rc=CreateNSRDirectivesFile(); //currently needed as the directives file is always deleted in CleanUpTool

    if(rc && tcn35d_BackupInfo::ARestore==BackupInfo->DataDirection())
        rc=BuildBackupToolCall();

    if(rc)
        rc=CreateTempOutputFiles();

    return rc;
}

int tcn352_NetWorkerConnector::LastMinutePreparation(int NumberOfCurrentCommandLine)
{
    if(tcn35d_BackupInfo::ABackup==BackupInfo->DataDirection())   //if it's a backup
    {
        if(0==NumberOfCurrentCommandLine)   //if it's the first save command 
        {
            // assure, that no two backups have the same time stamp
            long CurrentTime=long(time(0));

            while(CurrentTime==StartSaveTime)
            {
                Sleep(1); //sleep a second
                CurrentTime=long(time(0));
            }

            //store the current time (used later to distinguish between old and current backups)
            StartSaveTime=CurrentTime;
        }
        else
            Sleep(NSRSaveInterval);    //wait NSRSaveInterval seconds, if it's not the first save command 
    }

    return 1;
}

int tcn352_NetWorkerConnector::CleanUpTool()
{
    int rc=1;

    while(!DirectiveFileNames.empty())
    {
        tni34_AFile DFile(DirectiveFileNames.back(), tni34_AFile::FromDisk, 0, 0); // do not open, do not create now

        if(DFile.Exists() && DFile.Remove())
            sprintf(MsgBuf_cn36, "Removed NetWorker's directives file '%s'.\n", DirectiveFileNames.back());
        else
        {
            rc=0;
            sprintf(MsgBuf_cn36, "Could not remove NetWorker's directives file '%s'.\n", DirectiveFileNames.back());
        }

        LogWriter->Write(MsgBuf_cn36);

        cn36_StrDealloc(DirectiveFileNames.back());
        DirectiveFileNames.pop_back();
    }

    return rc;
}

int tcn352_NetWorkerConnector::ConstructNameForPart(char *& Name, int NumberOfPipe)
{
    int rc=1;

    rc=cn36_StrAllocCpy(Name, NSRBackupName);   //at the moment NetWorkers backup name is the name of all parts

    /*  alternative code for old 6.2.10-style names for parts 
    if(1>NumberOfPipe)
        rc=cn36_StrAllocCpy(Name, BackupInfo->DBName());
    else
    {
        size_t Length;
    
        char NumFilesAsString[30]={'\0',};
        char NumberOfPipeAsString[30]={'\0'};
        
        NumberOfPipe++; // translate numbers 0 ... BackupInfo->NumberOfPipes()-1 to 1 ... BackupInfo->NumberOfPipes()

        sprintf(NumFilesAsString, "%d", int(BackupInfo->NumberOfPipes()));
        sprintf(NumberOfPipeAsString, "%d", int(NumberOfPipe));

        Length=strlen(BackupInfo->DBName())+
               1+strlen(NumberOfPipeAsString)+
               2+strlen(NumFilesAsString);

        if(!cn36_StrAlloc(Name, Length))
            rc=0;
        else
        {
            sprintf(Name, "%s_%dof%d", BackupInfo->DBName(), int(NumberOfPipe), int(BackupInfo->NumberOfPipes()));

            if(Length!=strlen(Name))
            {
                rc=0;
                ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
            }
        }
    }*/

    return rc;
}

int tcn352_NetWorkerConnector::CollectExternalBackupIDs(char * ExternalBackupID[MAX_NUMBER_OF_TOOLPROCESSES_CN35], int & NumberOfEBIDsFound)
{
    int rc=1;

    NumberOfEBIDsFound=0;

    if(UseLLOptionForSave && NSaveTime==NSREBIDType)
    {
        int i;
        
        NumberOfEBIDsFound=NSRNumberOfEBIDsFound;

        for(i=0; i<NSRNumberOfEBIDsFound; i++)
        {
            cn36_StrDealloc(ExternalBackupID[i]);
            ExternalBackupID[i]=NSRExternalBackupID[i];
            NSRExternalBackupID[i]=0;
        }

        NSRNumberOfEBIDsFound=0;
    }
    else
    {
        tni33_ArgumentList ArgList;
        const char * ReportColumns="name(26),pssid,nsavetime,ssid,level(4)";    //pssid -> we can "grep" for lines with pssid==0 and use of nsavetime or SSID is possible

        LogWriter->Write("Requesting External Backup ID's from NetWorker.\n", tcn35d_DBBackupControllerLogWriter::AddALevelToPrefix);

        if(ArgList.AddArgument(PathOfNSRMminfo) &&                  //"...\mminfo.exe" or ".../mminfo"
           ArgList.AddArgument("-s") &&                             //"-s"
           ArgList.AddArgument(NameOfNSR_HOST) &&                   //"<NSR_HOST>"
           ArgList.AddArgument("-c") &&                             //"-c"
           ArgList.AddArgument(NodeNameToUse()) &&                  //"<machine_name>"
           ArgList.AddArgument("-o") &&                             //"-o"
           ArgList.AddArgument("t") &&                              //"t"
           ArgList.AddArgument("-a") &&                             //"-a"
           ArgList.AddArgument("-r") &&                             //"-r"
           ArgList.AddArgument(ReportColumns) &&                    //"name(26),..."
           (NSRAvoidMMInfoBug || ArgList.AddArgument("-q")) &&      //"-q", if "-q pssid=0" can be used with the NetWorker version
           (NSRAvoidMMInfoBug || ArgList.AddArgument("pssid=0")) && //"pssid=0", if "-q pssid=0" can be used with the NetWorker version
           (0==NSRAvoidMMInfoTimeBug || ArgList.AddArgument("-t")) &&
           (0==NSRAvoidMMInfoTimeBug || ArgList.AddArgument(NSRAvoidMMInfoTimeBug)) &&
           ArgList.AddArgument("-N") &&                             //"-N"
           ArgList.AddArgument(NSRBackupName) &&                    //"<NSRBackupName>" // <NSRBackupName>==<dbname> if not configured otherwise
           0!=ArgList.GiveCommandLine())                            //we have memory to construct the command line too
        {
            sprintf(MsgBuf_cn36, "Constructed the %s call '%s'.\n", GiveToolName(), ArgList.GiveCommandLine());
            LogWriter->Write(MsgBuf_cn36);

            tni34_ATextFile MminfoOutput(tni34_AFile::FromDisk, 0, 0);

            if(MminfoOutput.IsGood() && MminfoOutput.Exists())
            {
                tni33_AProcess Mminfo(ArgList, 0, &MminfoOutput);

                if(0==Mminfo.GiveReturnCode())
                {
                    EndSaveTime=long(time(0));

                    if(MminfoOutput.Open())
                    {
                        const char * Line=0;
                        rc=1;

                        while(rc && MminfoOutput.ReadLine(Line) && Line!=0)  // while no read error and not end of file
                        {
                            const char * CurrentItem=cn36_FirstNonWhiteSpaceOf(Line);
                            const char * EndOfCurrentItem=cn36_FirstWhiteSpaceOf(CurrentItem);

                            if(0==cn36_StrStrNCmp(NSRBackupName, Line, EndOfCurrentItem-CurrentItem))   //Line starts with NetWorkers name of the backups requested
                            {
                                CurrentItem=cn36_FirstNonWhiteSpaceOf(EndOfCurrentItem); //CurrentItem="<pssid> ..."
                                EndOfCurrentItem=cn36_FirstWhiteSpaceOf(CurrentItem);

                                if(0==cn36_StrStrNCmp("0", CurrentItem, EndOfCurrentItem-CurrentItem))  //we are interested only in save sets with parent save set ID of 0 (first save set of a backup)
                                {
                                    int SaveTime;

                                    CurrentItem=cn36_FirstNonWhiteSpaceOf(EndOfCurrentItem); //CurrentItem="<nsavetime> ..."
                                    EndOfCurrentItem=cn36_FirstWhiteSpaceOf(CurrentItem);

                                    if(cn36_StrNToInt(SaveTime, CurrentItem, EndOfCurrentItem-CurrentItem))  // <- this is a year 2038 problem if int SaveTime is signed 32 bit
                                    {
                                        if(StartSaveTime<=SaveTime && SaveTime<=EndSaveTime)    // the savetime must be between begin and end of the backup
                                        {
                                            if(NumberOfEBIDsFound>=NumberOfCommandLines)  //we have found one Extenal Backup ID too many
                                            {
                                                rc=0;
                                                sprintf(MsgBuf_cn36,
                                                        "Found %d. External Backup ID '%s' for %d pipes.\n",
                                                        int(NumberOfEBIDsFound+1),
                                                        ExternalBackupID[NumberOfEBIDsFound],
                                                        int(NumberOfCommandLines));
                                                LogWriter->Write(MsgBuf_cn36);
                                            }
                                            else
                                            {
                                                if(SSID==NSREBIDType)   // if we use SSID as identifier ...
                                                {
                                                    CurrentItem=cn36_FirstNonWhiteSpaceOf(EndOfCurrentItem);    //... we must take the next item in the line (CurrentItem="<SSID> ...")
                                                    EndOfCurrentItem=cn36_FirstWhiteSpaceOf(CurrentItem);
                            
                                                    if(!cn36_StrNAllocCpy(ExternalBackupID[NumberOfEBIDsFound], CurrentItem, EndOfCurrentItem-CurrentItem))
                                                        rc=0;
                                                }
                                                else
                                                    rc=BuildEBIDFromNSavetime(ExternalBackupID[NumberOfEBIDsFound], CurrentItem, EndOfCurrentItem);
                                            }

                                            if(rc)
                                            {
                                                sprintf(MsgBuf_cn36,
                                                        "Found External Backup ID '%s' for pipe '%s'.\n",
                                                        ExternalBackupID[NumberOfEBIDsFound],
                                                        BackupInfo->NameOfPipe(NumberOfEBIDsFound));
                                                LogWriter->Write(MsgBuf_cn36);

                                                NumberOfEBIDsFound++;
                                            }
                                            else
                                                NumberOfEBIDsFound=0; // provoke an error
                                        } //else : we ignore older ID's from older saves
                                    }
                                    else
                                    {
                                        ErrorHandler->Report(tcn352_NSRError(tcn352_NSRError::CaseUnknownFormatOfMminfoOutput, Line));
                                        rc=0;
                                    }
                                }   //else : we ignore this line, because it has not 0 as pssid
                            } //else : we ignore this line, because it is not starting with the name of the i-th part
                        }

                        if(rc && 0==NumberOfEBIDsFound)
                        {
                            rc=0;
                            LogWriter->Write("Output of mminfo contains no save information.\n");
                        }

                        if(!MminfoOutput.Remove()) //TODO: remove output in CleanUp()
                            rc=0;
                    }
                    else
                        rc=0;
                }
                else
                    rc=0;
            }
            else
                rc=0;
        }

        if(rc)
        {
            if(NumberOfEBIDsFound==1)
                sprintf(MsgBuf_cn36, "Determined the External Backup ID successfully.\n");
            else
                sprintf(MsgBuf_cn36, "Determined the %d External Backup ID's successfully.\n", (int)NumberOfEBIDsFound);
        }
        else
            sprintf(MsgBuf_cn36, "Could not determine the External Backup ID's of the last backup.\n");
    
        LogWriter->Write(MsgBuf_cn36, tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefixAndEndParagraph);
    }

    return rc;
}

int tcn352_NetWorkerConnector::CheckPipes()
{
    int rc=tcn35_BackupToolConnector::CheckPipes();

    int VerifyThatPipesAreInOneDirectory=1;
    
    #if defined(WIN32)
    //all pipes must be in \\.\pipe\ on Windows
    #elif defined(UNIX)
    if(!NSRUseDotNSRFile)                   // a single directive file is used for every pipe
        VerifyThatPipesAreInOneDirectory=0; // the pipes can be in different places
    #else
        #error tcn352_NetWorkerConnector::CreateNSRDirectivesFile() only coded for WIN32 and UNIX, define one of them
    #endif

    if(rc && 1<BackupInfo->NumberOfPipes() && VerifyThatPipesAreInOneDirectory)   //more than one pipe is used and we are on Windows or a .nsr file is used
    {
        char * PipeDirectory=0;
        int i;
        
        rc=cn36_StrNAllocCpy(PipeDirectory,
                             BackupInfo->NameOfPipe(0),
                             cn36_BaseNameFrom(BackupInfo->NameOfPipe(0))-BackupInfo->NameOfPipe(0)); //get the Directory out from the path of the 1st pipe
       
        for(i=1; rc && i<BackupInfo->NumberOfPipes(); i++)  //compare the directories of all other pipes with the directory of the 1st pipe
        {
            if(0!=cn36_StrStrNCmp(PipeDirectory, BackupInfo->NameOfPipe(i), cn36_BaseNameFrom(BackupInfo->NameOfPipe(i))-BackupInfo->NameOfPipe(i)))
            {
                rc=0;
                
                if('\0'!=PipeDirectory[0])
                    ErrorHandler->Report(tcn352_NSRError(tcn352_NSRError::CaseDifferentPipeDirectories, PipeDirectory));
                else
                    ErrorHandler->Report(tcn352_NSRError(tcn352_NSRError::CaseDifferentPipeDirectories));
            }
        }

        if(rc && '\0'!=PipeDirectory[0]) //all ok, all pipes are in the same directory and that directory has even a non empty name
        {
            sprintf(MsgBuf_cn36, "All pipes are located in '%s'.\n",PipeDirectory);
            LogWriter->Write(MsgBuf_cn36);
        }

        cn36_StrDealloc(PipeDirectory);
    }

    return rc;
}

int tcn352_NetWorkerConnector::GiveInformationOfLine(const char *                   Line,
                                                     const char *                 & EBIDInLine,
                                                     size_t                       & EBIDLength,
                                                     tcn35d_BackupInfo::ADataType & DataType,
                                                     const char *                 & DateTimeString,
                                                     int                          & LineContainsEBID)
{
    int rc=1;

    LineContainsEBID=0;

    if(0!=Line)
    {
        const char * Name=cn36_FirstNonWhiteSpaceOf(Line);
        const char * EndOfName=cn36_FirstWhiteSpaceOf(Name);

        size_t       LengthOfBackupName=strlen(NSRBackupName);

        if(!strncmp(Name, NSRBackupName, LengthOfBackupName) &&     // Line must start with the name of the backup (==the name of the saved db, if not configured otherwise)
           size_t(EndOfName-Name)==LengthOfBackupName)              // must start exactly with the name of the backup (==the name saved database, if not configured otherwise)
        {
            const char * PSSID=cn36_FirstNonWhiteSpaceOf(EndOfName);
            const char * EndOfPSSID=cn36_FirstWhiteSpaceOf(PSSID);

            if(0==cn36_StrStrNCmp("0", PSSID, EndOfPSSID-PSSID))  //we are interested only in save sets with parent save set ID of 0 (first save set of a backup)
            {
                const char * Level=cn36_FirstNonWhiteSpaceOf(EndOfPSSID);
                const char * EndOfLevel=cn36_FirstWhiteSpaceOf(Level);

                const char * SaveDate=cn36_FirstNonWhiteSpaceOf(EndOfLevel);
                const char * EndOfSaveDate;

                ToolsTime_TimeFormat * DateTimeFormatToUse=&StandardDateTimeFormat2DigitYear;

                if(UseExplicitDateTimeFormat)
                    DateTimeFormatToUse=&ExplicitDateTimeFormat;
                else
                {
                    if(10==cn36_FirstWhiteSpaceOf(SaveDate)-SaveDate)
                        DateTimeFormatToUse=&StandardDateTimeFormat4DigitYear;
                }
                
                ToolsTime_Time DateTimeInLine(SaveDate, *DateTimeFormatToUse, EndOfSaveDate);

                if(DateTimeInLine.isValidTime())
                {
                    const char * TotalSize=cn36_FirstNonWhiteSpaceOf(cn36_FirstWhiteSpaceOf(EndOfSaveDate));    // TotalSize will not be tested or used
                    const char * EndOfTotalSize=cn36_FirstWhiteSpaceOf(TotalSize);

                    const char * ID=cn36_FirstNonWhiteSpaceOf(EndOfTotalSize);  //can be SSID or NSaveTime
                    const char * EndOfID=cn36_FirstWhiteSpaceOf(ID);
            
                    if('\0'!=(*ID) &&                                 // ID must be not empty (Line must have enough parts)
                       '\0'==(*cn36_FirstNonWhiteSpaceOf(EndOfID)) && // only white spaces are allowed behind ID
                       cn36_StrNConsistsOfDigits(ID, EndOfID-ID))     // the ID must only consist of digits
                    {
                        if(SSID==NSREBIDType)
                        {
                            EBIDInLine=ID;
                            EBIDLength=EndOfID-ID;
                        }
                        else
                        {
                            rc=BuildEBIDFromNSavetime(TempEBID, ID, EndOfID);

                            if(rc)
                            {
                                EBIDInLine=TempEBID;    //EBIDInLine="NST <savetime> <machinename>"
                                EBIDLength=strlen(TempEBID);
                            }
                        }

                        if(rc)
                        {
                            if(DataTypeForLevelN(DataType, Level, EndOfLevel-Level))    // the level must be known and therefore correspond to an DataType
                            {
                                if(0!=TempDateTimeString)
                                    cn36_StrDealloc(TempDateTimeString);

                                TempDateTimeString=ToolsTime_TimeFormatter().createString(DateTimeInLine, OutputDateTimeFormat);

                                if(0==TempDateTimeString)
                                    rc=0;
                                else
                                {
                                    DateTimeString=TempDateTimeString;
                                    LineContainsEBID=1;
                                }
                            }
                            else
                            {
                                sprintf(MsgBuf_cn36, "The line '%s' is ignored, as '", Line);
                                strncat(MsgBuf_cn36, Level, EndOfLevel-Level);
                                strcat(MsgBuf_cn36, "' is an unknown backup level.\n");
                                LogWriter->Write(MsgBuf_cn36);
                            }
                        }
                    }
                    else
                    {
                        if('\0'==(*ID))
                            sprintf(MsgBuf_cn36, "The line '%s' is ignored, as it does not contain a backup ID after '%s'.\n", Line, EndOfSaveDate);
                        else
                            sprintf(MsgBuf_cn36, "The line '%s' is ignored, as it does not contain a backup ID at '%s'.\n", Line, ID);

                        LogWriter->Write(MsgBuf_cn36);
                    }
                }
                else
                {
                    const char * DateTimeFormatString=DateTimeFormatToUse->getFormatString();

                    if(0!=DateTimeFormatString && '\0'!=DateTimeFormatString[0])
                        sprintf(MsgBuf_cn36, "The line '%s' is ignored, as it does not have a date/time of the format %s.\n", Line, DateTimeFormatString);
                    else
                        sprintf(MsgBuf_cn36, "The line '%s' is ignored, as it does not have a date/time of expected format.\n", Line);

                    LogWriter->Write(MsgBuf_cn36);
                }
            }
            else
            {
                sprintf(MsgBuf_cn36, "The line '%s' is ignored, as it does not have the Previous SSID 0.\n", Line);
                LogWriter->Write(MsgBuf_cn36);
            }
        }
        else
            if('\0'!=(*Line))
            {
                sprintf(MsgBuf_cn36, "The line '%s' is ignored, as it does not start with the backup name %s.\n", Line, NSRBackupName);
                LogWriter->Write(MsgBuf_cn36);
            }
    }

    return rc;
}

int tcn352_NetWorkerConnector::BuildEBIDFromNSavetime(char       * & EBID,
                                                      const char *   StartOfNSavetime,
                                                      const char *   EndOfNSavetime)
{
    int rc=1;

    size_t LengthOfNSavetime=EndOfNSavetime-StartOfNSavetime;

    if(!cn36_StrAlloc(EBID, strlen(NSaveTimeEBIDPrefix)     // try to get memory for a EBID of form "NST <nsavetime> <machinename>"
                            +strlen(NSaveTimeEBIDPartDelimiter)
                            +LengthOfNSavetime
                            +strlen(NSaveTimeEBIDPartDelimiter)
                            +strlen(NodeNameToUse()))) 
    {
        rc=0;
    }
    else
    {                       //build the EBID "NST <nsavetime> <machinename>"
        size_t CurrPos;

        SAPDB_strcpy(EBID, NSaveTimeEBIDPrefix);                              // EBID="NST"
        CurrPos=strlen(NSaveTimeEBIDPrefix);

        SAPDB_strcpy(EBID+CurrPos, NSaveTimeEBIDPartDelimiter);               // EBID="NST "
        CurrPos+=strlen(NSaveTimeEBIDPartDelimiter);

        cn36_StrNCpy(EBID+CurrPos, StartOfNSavetime, LengthOfNSavetime);// EBID="NST <nsavetime>"
        CurrPos+=LengthOfNSavetime;

        SAPDB_strcpy(EBID+CurrPos, NSaveTimeEBIDPartDelimiter);               // EBID="NST <nsavetime> "
        CurrPos+=strlen(NSaveTimeEBIDPartDelimiter);

        SAPDB_strcpy(EBID+CurrPos, NodeNameToUse());                          // EBID="NST <nsavetime> <machinename>"
    }

    return rc;
}

int tcn352_NetWorkerConnector::ToolWasSuccessful(teo00_Uint8 DBBackupSize)
{
    int rc=tcn35_BackupToolConnector::ToolWasSuccessful(DBBackupSize);  //check the NetWorker return code 

    if(rc && tcn35d_BackupInfo::ABackup==BackupInfo->DataDirection())     //if return code is 0 and we make a backup and we should look through NetWorkers output
    {
        if(FoundError==NSROutputResult)  // already searched the output of the NetWorker call and found an error
            rc=0;
        else
        {
            if(NotChecked==NSROutputResult)  //we have never before checked the current NetWorker output
            {
                teo00_Uint8 NSRBackupSize=0;

                int i;

                unsigned int NumberOfErrorMessage=0, 
                             NumberOfMissingCompletionMessages=0;

                sprintf(MsgBuf_cn36,"Analyzing output of %s.\n", GiveToolName());
                LogWriter->Write(MsgBuf_cn36, tcn35d_DBBackupControllerLogWriter::AddALevelToPrefix);
               
                for(i=0; rc && i<BackupInfo->NumberOfPipes(); i++)    //scan all NetWorker output and error output files for errors
                {
                    int FoundCompletionMessageInOutput=0,
                        FoundCompletionMessageInErrorOutput=0,
                        FoundNSavetimeMessage=0;
                    teo00_Uint8 SizeInMessageFromOutput=0,
                                SizeInMessageFromErrorOutput=0;
                    
                    tni34_ATextFile NSROutput(ToolProcessOutput[i]->GiveName(), tni34_AFile::FromDisk, 0, 1);    //do not create, open now

                    if(NSROutput.IsGood())
                    {
                        const char *Line;

                        while(rc && NSROutput.ReadLine(Line) && Line!=0)    //for all lines of the file
                            rc=AnalyzeNSROutputLine(NSROutput.GiveName(), Line, i, FoundCompletionMessageInOutput, FoundNSavetimeMessage, SizeInMessageFromOutput, NumberOfErrorMessage);
                    }

                    if(rc)
                    {
                        tni34_ATextFile NSRErrOutput(ToolProcessError[i]->GiveName(), tni34_AFile::FromDisk, 0, 1);    //do not create, open now

                        if(NSRErrOutput.IsGood())
                        {
                            const char *Line;

                            while(rc && NSRErrOutput.ReadLine(Line) && Line!=0)    //for all lines of the file
                                rc=AnalyzeNSROutputLine(NSRErrOutput.GiveName(), Line, i, FoundCompletionMessageInErrorOutput, FoundNSavetimeMessage, SizeInMessageFromErrorOutput, NumberOfErrorMessage);
                        }
                    }

                    if(UseNSROutput)
                    {
                        if(FoundCompletionMessageInOutput && FoundCompletionMessageInErrorOutput)   //if found a completion message in output and error output
                        {
                            if(SizeInMessageFromOutput>SizeInMessageFromErrorOutput)                //add the minimum of the both values to the sum
                                NSRBackupSize+=SizeInMessageFromErrorOutput;
                            else
                                NSRBackupSize+=SizeInMessageFromOutput;
                        }
                        else
                            if(FoundCompletionMessageInOutput)                                      //if found a completion message only in output
                                NSRBackupSize+=SizeInMessageFromOutput;                             //add the value to the sum
                            else
                                if(FoundCompletionMessageInErrorOutput)                             //if found a completion message only in error output
                                    NSRBackupSize+=SizeInMessageFromErrorOutput;                    //add the value to the sum
                                else
                                {
                                    NumberOfMissingCompletionMessages++;    //we must find a completion message for every save-process (if we can not open the file, we have not found one!)
                                    sprintf(MsgBuf_cn36,
                                            "Could not find a completion message for pipe '%s' with a backup size in the %s output '%s' and '%s'.\n",
                                            BackupInfo->NameOfPipe(i),
                                            GiveToolName(),
                                            ToolProcessOutput[i]->GiveName(),
                                            ToolProcessError[i]->GiveName());
                                    LogWriter->Write(MsgBuf_cn36);
                                }
                    }

                    if(UseLLOptionForSave && NSaveTime==NSREBIDType)
                    {
                        if(!FoundNSavetimeMessage)
                        {
                            sprintf(MsgBuf_cn36,
                                    "Could not find a savetime message for pipe '%s' in the %s output '%s' and '%s', despite using the -LL option for %s's SAVE.\n",
                                    BackupInfo->NameOfPipe(i),
                                    GiveToolName(),
                                    ToolProcessOutput[i]->GiveName(),
                                    ToolProcessError[i]->GiveName(),
                                    GiveToolName());

                            LogWriter->Write(MsgBuf_cn36);
                        }
                        else
                            NSRNumberOfEBIDsFound++;
                    }
                }

                if(0<NumberOfErrorMessage)
                {
                    const char * PluralS=(1==NumberOfErrorMessage==1)?"":"s";
                    const char * PluralMessage=(1==NumberOfErrorMessage==1)?"the message":"one of the messages";

                    sprintf(MsgBuf_cn36, "\nFound %u error message%s in %s output (see above).\n", NumberOfErrorMessage, PluralS, GiveToolName());
                    LogWriter->Write(MsgBuf_cn36);
                    sprintf(MsgBuf_cn36, "%s does not distinguish between errors, warnings and informative messages.\n", GiveToolName());
                    LogWriter->Write(MsgBuf_cn36);
                    sprintf(MsgBuf_cn36, "If %s above is no real error message, configure the Database Manager to ignore "
                                         "this message by inserting a NSR_IGNOREERROR statement for this error message in the file '%s'.\n",
                                         PluralMessage,
                                         PathOfNSR_ENVFile);
                    LogWriter->Write(MsgBuf_cn36);

                    rc=0;
                }

                if(0<NumberOfMissingCompletionMessages)
                {
                    const char * PluralS=(1==NumberOfMissingCompletionMessages)?"":"s";
                    const char * IsAre=(1==NumberOfMissingCompletionMessages)?"is":"are";

                    sprintf(MsgBuf_cn36, "\nThe completion message%s for %u pipe%s %s missing in %s output (see above).\n",
                                         PluralS,
                                         NumberOfMissingCompletionMessages,
                                         PluralS,
                                         IsAre,
                                         GiveToolName());
                    LogWriter->Write(MsgBuf_cn36);

                    rc=0;
                }

                if(UseNSROutput)
                {
                    if(!rc)
                        sprintf(MsgBuf_cn36, "Errors found in %s's output.\n", GiveToolName());
                    else
                    {
                        if(DBBackupSize>NSRBackupSize)  //NetWorkers NSRBackupSize should be greater or eqaul than the database's backup size
                        {
                            const int SizeOfSizeAsString=30;    //21 would be enough for 2^64
                            char ReportedSizeAsString[SizeOfSizeAsString];
                            char DBSizeAsString[SizeOfSizeAsString];

                            rc=0;

                            if(!cn36_UInt8ToStrN(ReportedSizeAsString, SizeOfSizeAsString, NSRBackupSize) ||
                               !cn36_UInt8ToStrN(DBSizeAsString, SizeOfSizeAsString, DBBackupSize))
                            {
                                ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
                            }
                            else
                            {
                                sprintf(MsgBuf_cn36, "NetWorker has saved only %s bytes (the database kernel has saved %s bytes).\n", ReportedSizeAsString, DBSizeAsString);
                                LogWriter->Write(MsgBuf_cn36);
                            }
                        }

                        if(rc)
                            sprintf(MsgBuf_cn36, "No error message found in %s output.\n", GiveToolName());
                        else
                            sprintf(MsgBuf_cn36, "The backup size of database and %s are different.\n", GiveToolName());
                    }
                }
                else
                    sprintf(MsgBuf_cn36, "Finished analyzing output of %s.\n", GiveToolName());

                if(rc)
                    NSROutputResult=FoundNoError;
                else
                    NSROutputResult=FoundError;

                LogWriter->Write(MsgBuf_cn36, tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefixAndEndParagraph);
            }
            //else NetWorker output was already checked and no error was found -> return code is decisivly
        }
    }
    
    return rc;
}

int tcn352_NetWorkerConnector::ToolWasSuccessfulSoFar()
{
    int rc=tcn35_BackupToolConnector::ToolWasSuccessfulSoFar();  //check the NetWorker return code 

    if(rc && tcn35d_BackupInfo::ABackup==BackupInfo->DataDirection())     //if return code is 0 and we make a backup and we should look through NetWorkers output
    {
        int          i;
        unsigned int ErrorMessages=0;

        for(i=0; rc && i<BackupInfo->NumberOfPipes(); i++)    //scan all already available NetWorker output and error output files for errors
        {
            if(FoundError==ErrorInOutput[i])    // already searched the output of the i-th NetWorker call and found an error
                rc=0;
            else
            {
                if(NotChecked==ErrorInOutput[i] &&  //we have never before checked the current NetWorker output
                   0!=ToolProcess[i] &&             
                   !ToolProcess[i]->IsRunning())    //and that process in not longer running
                {
                    int FoundCompletionMessageInOutput=0,
                        FoundCompletionMessageInErrorOutput=0,
                        FoundNSavetimeMessage=0;
                    teo00_Uint8 SizeInMessageFromOutput=0,
                                SizeInMessageFromErrorOutput=0;
                    
                    tni34_ATextFile NSROutput(ToolProcessOutput[i]->GiveName(), tni34_AFile::FromDisk, 0, 1);    //do not create, open now

                    if(NSROutput.IsGood())
                    {
                        const char *Line;

                        while(rc && NSROutput.ReadLine(Line) && Line!=0)    //for all lines of the file
                        {
                            rc=AnalyzeNSROutputLine(NSROutput.GiveName(), Line, i, FoundCompletionMessageInOutput, FoundNSavetimeMessage, SizeInMessageFromOutput, ErrorMessages);

                            if(rc && 0!=ErrorMessages)
                                rc=0;
                        }
                    }

                    if(rc)
                    {
                        tni34_ATextFile NSRErrOutput(ToolProcessError[i]->GiveName(), tni34_AFile::FromDisk, 0, 1);    //do not create, open now

                        if(NSRErrOutput.IsGood())
                        {
                            const char *Line;

                            while(rc && NSRErrOutput.ReadLine(Line) && Line!=0)    //for all lines of the file
                            {
                                rc=AnalyzeNSROutputLine(NSRErrOutput.GiveName(), Line, i, FoundCompletionMessageInErrorOutput, FoundNSavetimeMessage, SizeInMessageFromErrorOutput, ErrorMessages);

                                if(rc && 0!=ErrorMessages)
                                    rc=0;
                            }
                        }
                    }

                    if(UseNSROutput && rc && (!FoundCompletionMessageInOutput && !FoundCompletionMessageInErrorOutput))
                    {
                        //we must find a completion message for every save-process (if we can not open the file, we have not found one!)
                        sprintf(MsgBuf_cn36,
                                "Could not find a completion message for pipe '%s' with a backup size in the %s output '%s' and '%s'.\n",
                                BackupInfo->NameOfPipe(i),
                                GiveToolName(),
                                ToolProcessOutput[i]->GiveName(),
                                ToolProcessError[i]->GiveName());
                        LogWriter->Write(MsgBuf_cn36);

                        rc=0;
                    }

                    if(rc && (UseLLOptionForSave && NSaveTime==NSREBIDType))
                    {
                        if(!FoundNSavetimeMessage)
                        {
                            sprintf(MsgBuf_cn36,
                                    "Could not find a savetime message for pipe '%s' in the %s output '%s' and '%s', despite using the -LL option for %s's SAVE.\n",
                                    BackupInfo->NameOfPipe(i),
                                    GiveToolName(),
                                    ToolProcessOutput[i]->GiveName(),
                                    ToolProcessError[i]->GiveName(),
                                    GiveToolName());

                            LogWriter->Write(MsgBuf_cn36);

                            rc=0;
                        }
                    }

                    if(rc)
                        ErrorInOutput[i]=FoundNoError;
                    else
                        ErrorInOutput[i]=FoundError;
                }
            }
        }
    }

    return rc;
}

int tcn352_NetWorkerConnector::AnalyzeNSROutputLine(const char   * FileName,
                                                    const char   * Line,
                                                    int            NumberOfPart,
                                                    int          & FoundCompletionMessage,
                                                    int          & FoundNSavetimeMessage,
                                                    teo00_Uint8  & BytesInCompletionMessage,
                                                    unsigned int & NumberOfErrorMessages)
{
    int rc=1;

    const char * SavePrefix="save:";

    if(UseNSROutput)
    {
        if(0==strncmp(Line, SavePrefix, strlen(SavePrefix))) //Line starts with SavePrefix
        {
            const char * StartNextPart=cn36_FirstNonWhiteSpaceOf(Line+strlen(SavePrefix));
            const char * EndNextPart=cn36_FirstWhiteSpaceOf(StartNextPart);

            if(StartNextPart!=EndNextPart)  // there is something in the Line
            {
                if(FoundFileNameFollowedByWarning(BackupInfo->NameOfPipe(NumberOfPart), StartNextPart))
                    NumberOfErrorMessages++;
                else    //is it the completion message (save: <dbname> level=<level>, <size> B|KB|MB|GB|TB ..." )? 
                {
                    if(LineContainsCompletionMessage(NumberOfPart, StartNextPart, BytesInCompletionMessage))
                    {
                        FoundCompletionMessage=1;

                        sprintf(MsgBuf_cn36, "Have found a completion message in the %s output file '%s' in line:\n%s\n", GiveToolName(), FileName, Line);
                        LogWriter->Write(MsgBuf_cn36);
                    }
                }
            }
        }
        else
        {
            if(FoundFileNameFollowedByWarning(BackupInfo->NameOfPipe(NumberOfPart), Line))     //in some NetWorker versions "warnings" are written without "save:"
                NumberOfErrorMessages++;
            else
                if(UseLLOptionForSave && NSaveTime==NSREBIDType)
                    rc=CheckForNSavetimeMessage(FileName, NumberOfPart, cn36_FirstNonWhiteSpaceOf(Line), FoundNSavetimeMessage);
        }
    }
    else   //just look for the NSaveTimeMessage, but not for errors, etc.
        if(UseLLOptionForSave && NSaveTime==NSREBIDType)
            rc=CheckForNSavetimeMessage(FileName, NumberOfPart, cn36_FirstNonWhiteSpaceOf(Line), FoundNSavetimeMessage);

    return rc;
}

int tcn352_NetWorkerConnector::FoundFileNameFollowedByWarning(const char * FileName,
                                                              const char * NSROutputLine)
{
    int rc=0;
    const char * BaseName=cn36_BaseNameFrom(FileName);
    const char * StartOfLine=cn36_FirstNonWhiteSpaceOf(NSROutputLine);
    const char * BehindTheFileName=0;

    if(0==cn36_StrStrNCmp(FileName, StartOfLine, strlen(FileName)))     //NSROutputLine starts with the pipe name
        BehindTheFileName=cn36_FirstNonWhiteSpaceOf(StartOfLine+strlen(FileName));
    else
        if(0==cn36_StrStrNCmp(BaseName, StartOfLine, strlen(BaseName))) //NSROutputLine starts with the pipe name without the path
            BehindTheFileName=cn36_FirstNonWhiteSpaceOf(StartOfLine+strlen(BaseName));

    if(0!=BehindTheFileName &&      //the line started with the FileName
       ':'==BehindTheFileName[0])   //and there is a ':' behind it. We do not check for further spaces and the error message itself, because the ':' should not be there on success.
    {
        ni31_ListElem<char *> *h=ListOfErrorsToIgnore.First();
        int                   ErrorIsValid=1;

        while(0!=h && ErrorIsValid)     //loop through the list of errors which must be ignored
        {
            if(0==strcmp(BehindTheFileName+1, h->GiveInfo()))
                ErrorIsValid=0;

            h=h->Next();
        }

        if(ErrorIsValid)    //if the current error was not in the list it is an error
        {
            sprintf(MsgBuf_cn36, "Interpreting '%s' from NetWorker output as error message for pipe '%s'.\n", NSROutputLine, FileName);
            LogWriter->Write(MsgBuf_cn36);

            rc=1;
        }
    }

    return rc;
}

int tcn352_NetWorkerConnector::LineContainsCompletionMessage(int           NumberOfPart,
                                                             const char *  Line,                     //it's not the complete Line ("save: " is missing)
                                                             teo00_Uint8 & BytesInCompletionMessage)
{    
    int rc=1;

    const char * StartNextPart=Line;
    const char * EndNextPart=cn36_FirstWhiteSpaceOf(StartNextPart);

    if(0!=cn36_StrStrNCmp(NSRBackupName, StartNextPart, (EndNextPart-StartNextPart)))   //does the line start with NetWorker's backup name
        rc=0;

    if(rc)
    {
        const char * LevelKeyWord="level=";
        const char * ExpectedLevel=LevelForAction(BackupInfo->DataType(), NumberOfPart);

        StartNextPart=cn36_FirstNonWhiteSpaceOf(EndNextPart);
        EndNextPart=cn36_FirstWhiteSpaceOf(StartNextPart);

        if(size_t(EndNextPart-StartNextPart)<strlen(LevelKeyWord)+strlen(ExpectedLevel) ||          //next part is shorter than "level=" plus <expected level>
           size_t(EndNextPart-StartNextPart)>strlen(LevelKeyWord)+strlen(ExpectedLevel)+1 ||        //next part contains more than "level=<expected level>" or "level=<expected level>,"
            0!=strncmp(LevelKeyWord, StartNextPart, strlen(LevelKeyWord)) ||                        //next part does not start with "level="
            0!=strncmp(ExpectedLevel, StartNextPart+strlen(LevelKeyWord), strlen(ExpectedLevel)) || //"level=" is not followed by the expected NetWorker level string
            (EndNextPart==StartNextPart+strlen(LevelKeyWord)+strlen(ExpectedLevel)+1 &&             //next part contains more than "level=<expected level>"
             ','!= *(StartNextPart+strlen(LevelKeyWord)+strlen(ExpectedLevel))))                    //but does not end with ','
        {
            rc=0;
        }
    }

    if(rc)
    {
        StartNextPart=cn36_FirstNonWhiteSpaceOf(EndNextPart);
        EndNextPart=cn36_FirstWhiteSpaceOf(StartNextPart);

        teo00_Uint8 ReportedSize,
                    ReportedFraction=0,
                    FactorForUnit=1,
                    FactorBetweenUnits=1024;

        const char *Separator=StartNextPart;
        int         UsedFractionPart=0;
        size_t      NumberOfDigitsInFraction=0;

        while(EndNextPart!=Separator &&
              ('.'!= (*Separator)) &&
              (','!= (*Separator)))
        {
            Separator++;
        }

        if(EndNextPart!=Separator && EndNextPart!=(Separator+1))    //found a separator and something behind the separator
        {
            UsedFractionPart=1;
            NumberOfDigitsInFraction=EndNextPart-(Separator+1); // >0 as Separator+1<EndNextPart
        }

        if(cn36_StrNConsistsOfDigits(StartNextPart, (Separator-StartNextPart)) &&                               //next part starts with a number
           (!UsedFractionPart || cn36_StrNConsistsOfDigits(Separator+1, NumberOfDigitsInFraction)) &&           //no separator or a number after the separator
           cn36_StrNToUInt8(ReportedSize, StartNextPart, (Separator-StartNextPart)) &&                          //could get the number into a teo00_Uint8
           (!UsedFractionPart || cn36_StrNToUInt8(ReportedFraction, Separator+1, NumberOfDigitsInFraction)))    //no separator or could get the fraction into a teo00_Uint8
        {
            StartNextPart=cn36_FirstNonWhiteSpaceOf(EndNextPart);
            EndNextPart=cn36_FirstWhiteSpaceOf(StartNextPart);

            if(0!=cn36_StrStrNUprCmp("B", StartNextPart, (EndNextPart-StartNextPart)) &&
               0!=cn36_StrStrNUprCmp("BYTE", StartNextPart, (EndNextPart-StartNextPart)))
            {
                FactorForUnit*=FactorBetweenUnits;

                if(0!=cn36_StrStrNUprCmp("KB", StartNextPart, (EndNextPart-StartNextPart)))
                {
                    FactorForUnit*=FactorBetweenUnits;

                    if(0!=cn36_StrStrNUprCmp("MB", StartNextPart, (EndNextPart-StartNextPart)))
                    {
                        FactorForUnit*=FactorBetweenUnits;

                        if(0!=cn36_StrStrNUprCmp("GB", StartNextPart, (EndNextPart-StartNextPart)))
                        {
                            FactorForUnit*=FactorBetweenUnits;
                    
                            if(0!=cn36_StrStrNUprCmp("TB", StartNextPart, (EndNextPart-StartNextPart)))
                                rc=0;   // non of the known units fits -> we asume it is no completion message
                        }
                    }
                }
            }

            if(rc)
            {
                if(!UsedFractionPart && NoRoundup!=Roundup)   //Roundup is only used, if no fraction is reported so far
                {
                    if(LastInteger==Roundup)
                        ReportedSize+=1;
                    else
                    {
                        UsedFractionPart=1;
                        NumberOfDigitsInFraction=1;

                        if(Normal==Roundup)
                            ReportedFraction=4;         //1 is added later -> equals 0.5
                        else
                            if(FirstDecimal==Roundup)
                                ReportedFraction=0;     //1 is added later -> equals 0.1
                            else                        //if it's not one of the RoundupMethods above it we did something wrong 
                            {
                                rc=0;
                                ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
                            }
                    }
                }

                ReportedSize*=FactorForUnit; //appply the determined unit

                if(!UsedFractionPart)       //no fraction part at all (not even for rounding purposes)
                    ReportedFraction=0;
                else                        //try to deal with the problem of converting 0.986752413537825147673 TByte to a number in Bytes in a teo00_Uint8
                {                           //we hope(!) that a long double is precise enough to do the following an all machines
                    long double RFraction=(long double)(teo00_Int8(ReportedFraction+1));   //if NetWorker says 3.89 MB it might mean lower than 3.90 MB (depends on NetWorkers rounding) -> be sure and change 89 to 90 
                    long double Scale=pow(double(10), double(NumberOfDigitsInFraction));   //get the divisor to scale the fraction correctly

                    if(0!=Scale)
                    {
                        RFraction/=Scale;  //after that 0<=RFraction<=1, therefore the following line should not give an overflow, as FactorForUnit is also a teo00_Uint8
                        ReportedFraction=teo00_Uint8(RFraction*=(long double)(teo00_Int8(FactorForUnit)));    //convert the fraction to Bytes and put the result back into the integer ReportedFraction 
                    }
                    else
                    {
                        rc=0;
                        ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError)); //report an internal error if long double is obviously not precise enough for the math needed
                    }
                }

                BytesInCompletionMessage=ReportedSize+ReportedFraction;
            }
        }
        else
            rc=0;
    }

    return rc;
}

int tcn352_NetWorkerConnector::CheckForNSavetimeMessage(const char * FileName,
                                                        int          NumberOfPart,
                                                        const char * Line,
                                                        int        & FoundNSavetimeMessage)
{
    int          rc=1;
    const char * NSaveTimeMessageStart="completed savetime=";
    int          LineContainsNSavetime=0;

    if(0==strncmp(Line, NSaveTimeMessageStart, strlen(NSaveTimeMessageStart)))      //line starts with "completed savetime=" ...
    {
        const char * Savetime=cn36_FirstNonWhiteSpaceOf(Line+strlen(NSaveTimeMessageStart));
        const char * EndSavetime=cn36_FirstWhiteSpaceOf(Savetime);

        if(0!=EndSavetime-Savetime &&                                               //...and is followed by something
           cn36_StrNConsistsOfDigits(Savetime, EndSavetime-Savetime))               //which are only digits
        {
            LineContainsNSavetime=1;

            rc=BuildEBIDFromNSavetime(NSRExternalBackupID[NSRNumberOfEBIDsFound], Savetime, EndSavetime);    //NumberOfEBIDsFound is increased later,it could be in the second output as well
        }
    }

    if(rc && LineContainsNSavetime)
    {
        FoundNSavetimeMessage=1;

        sprintf(MsgBuf_cn36, "Have found a savetime message in the %s output file '%s' in line:\n%s\n", GiveToolName(), FileName, Line);
        LogWriter->Write(MsgBuf_cn36);
    }

    return rc;
}

void tcn352_NetWorkerConnector::DeleteListOfErrorsToIgnore()
{
    while(0!=ListOfErrorsToIgnore.First())                          //while something is in the list
    {
        delete ListOfErrorsToIgnore.First()->GiveInfo();            //delete the string the first element is pointing to
        ListOfErrorsToIgnore.Del();                                 //delete the firt list element itself
    }
}

const char * tcn352_NetWorkerConnector::NodeNameToUse() const
{
    const char * rc=0;

    if(BackupInfo->SourceNodeNameSetExplicitly() || 0==NSRClientName)   //whenever the name was set explicitly or not configured in any way (the first implies tcn35d_BackupInfo::GiveBackupInfo==BackupInfo->DataDirection() by the way)
        rc=BackupInfo->SourceNodeName();                                //use the source node name (equals NodeName during backup)
    else                                                                //otherwise
        rc=NSRClientName;                                               //use the configured name instead of the real machine name

    return rc;
}

void tcn352_NetWorkerConnector::deleteListOfAdditionalSaveParameters(ni31_List<char *> & additionalParameterList)
{
    while(0!=additionalParameterList.First())                          //while something is in the list
    {
        delete additionalParameterList.First()->GiveInfo();            //delete the string the first element is pointing to
        additionalParameterList.Del();                                 //delete the firt list element itself
    }
}

void tcn352_NetWorkerConnector::deleteAllListsOfAdditionalSaveParameters()
{
    deleteListOfAdditionalSaveParameters(m_NSRAdditionalSaveParameters);
    deleteListOfAdditionalSaveParameters(m_NSRAdditionalMMInfoParameters);
    deleteListOfAdditionalSaveParameters(m_NSRAdditionalRecoverParameters);
}

bool tcn352_NetWorkerConnector::addAdditionalParameters(
        tni33_ArgumentList      &destList,
        const ni31_List<char *> &additionalParameters)
{
    bool rc=true;

    const ni31_ListElem<char *> *h=additionalParameters.First();

    while(0!=h && rc) //loop through the list of additional parameters
    {
        if(!destList.AddArgument(h->GiveInfo()))
            rc=false;

        h=h->Next();
    }

    return rc;
}

bool tcn352_NetWorkerConnector::calculateAndStoreAdditionalParameters(
        const char *         additionalParameters,
        ni31_List<char *>  & additionalParameterList)
{
    bool rc=true;

    Tools_CommandlineToArgumentsConverter SpaceConverter;

    if(!SpaceConverter.setTo(additionalParameters, Tools_CommandlineToArgumentsConverter::WINDOWS_STYLE))
    {
        ErrorHandler->Report(tcn35d_DBBackupError::OutOfMemory.Set());
        rc=false;
    }

    for(size_t i=0; rc && i<SpaceConverter.getNumberOfArguments(); i++)
    {
        char *h=0;

        if(cn36_StrAllocCpy(h, SpaceConverter.getArgumentArray()[i]))
        {
            if(0==additionalParameterList.Add(h))
            {
                cn36_StrDealloc(h);
                ErrorHandler->Report(tcn35d_DBBackupError::OutOfMemory.Set());
                rc=false;
            }
        }
        else
            rc=false;
    }

    return rc;
}
