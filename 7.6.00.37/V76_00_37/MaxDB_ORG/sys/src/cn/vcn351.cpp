/*!
    \file    vcn351.cpp
    \author  TiloH
    \ingroup cn layer
    \brief   implementing connection between dbmsrv and external backup tool
             ADSM

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

#include"hcn351.h"
#include"hcn36.h"
#include"hni34.h"

#include "SAPDBCommon/SAPDB_string.h"

#include<time.h>
#include<stdio.h>

#if defined (WIN32)
static const char szOSLineDelimiter[3]={13, 10, '\0'};
#elif defined (UNIX)
const char   szOSLineDelimiter[2]={'\n','\0'};
#else
    #error vcn351.cpp was only coded for WIN32 and UNIX (define one of them).
#endif


// -----------------------------------------------------------------------------
// class tcn351_ADSMError
// -----------------------------------------------------------------------------

class tcn351_ADSMError: public tcn35d_DBBackupError
{
  public:
    enum ADSMErrorCase
    {
        CaseCanNotCreateADSMInputPipe,
        CaseCanNotBuildBackupLabel
    };

    tcn351_ADSMError(ADSMErrorCase ErrorCase);
    tcn351_ADSMError(ADSMErrorCase ErrorCase, int ErrorInfo);

  private:
    static DBBackupErrorDescription ADSMErrorToText[];
    void   SetThisError(int CaseToSet);
};

tcn351_ADSMError::tcn351_ADSMError(ADSMErrorCase ErrorCase)
:tcn35d_DBBackupError()
{
    SetThisError(ErrorCase);
    Set();
}

tcn351_ADSMError::tcn351_ADSMError(ADSMErrorCase ErrorCase, int ErrorInfo)
:tcn35d_DBBackupError()
{
    SetThisError(ErrorCase);
    SetI(ErrorInfo);
}

void tcn351_ADSMError::SetThisError(int CaseToSet)
{
    if(ADSMErrorToText[CaseToSet].Case==CaseToSet)
        ThisError=ADSMErrorToText+CaseToSet;
    else
    {
        ThisError=ADSMErrorToText;
        
        while(ThisError->Case!=CaseToSet && ThisError->Case!=CaseCanNotBuildBackupLabel)
            ThisError++;

        if(CaseToSet!=ThisError->Case)
            ThisError=GiveDescriptionOfInternalError();
    }
}

tcn351_ADSMError::DBBackupErrorDescription tcn351_ADSMError::ADSMErrorToText[]=
{
    {CaseCanNotCreateADSMInputPipe,
     AString,
     "Could not create the input pipe for ADSM.",
     "Could not create the input pipe '%s' for ADSM."},

    {CaseCanNotBuildBackupLabel,
     AString,
     "Could not construct save label.",
     "Could not construct save label (%s characters)."},

};


// -----------------------------------------------------------------------------
// implementation of class tcn351_ADSMConnector
// -----------------------------------------------------------------------------

tcn351_ADSMConnector::tcn351_ADSMConnector(tcn35d_BackupInfo                     * BackupInfo,
                                           tcn35d_DBBackupControllerLogWriter    * TheLogWriterToUse,
                                           tcn35d_DBBackupControllerErrorHandler * TheErrorHandlerToUse,
                                           tcn35d_ConfigurationVariableHandler   * TheConfigurationVariableHandler,
                                           char                                  * TheExternalBackupIDList)
  :tcn35_BackupToolConnector(BackupInfo, TheLogWriterToUse, TheErrorHandlerToUse, TheConfigurationVariableHandler, TheExternalBackupIDList),
   TempDataType(tcn35d_BackupInfo::DataRecovery)
{
    Label=0;
    ADSMInput=0;
    const char * DefaultADSMDirectory=
    #if defined(WIN32)
                                "c:\\win32app\\ibm\\adsm\\adint2";
    #elif defined(UNIX)
                                "/usr/adint";
    #else
        #error tcn351_ADSMConnector::tcn351_ADSMConnector() only coded for UNIX and NT (define one of them).
    #endif

    cn36_StrAllocCpy(ADSMDirectory=0, DefaultADSMDirectory);

    ADSMProgram=0;
}

tcn351_ADSMConnector::~tcn351_ADSMConnector()
{
    cn36_StrDealloc(Label);
    cn36_StrDealloc(ADSMDirectory);
    cn36_StrDealloc(ADSMProgram);

    if(0!=ADSMInput)
        delete ADSMInput;

    while(0!=TempEBIDList.First())
    {
        delete TempEBIDList.First()->GiveInfo();
        TempEBIDList.Del();
    }
}

const char * tcn351_ADSMConnector::GiveToolName() const
{
    return "ADSM";
}

const char * tcn351_ADSMConnector::GiveLongToolName() const
{
    return "IBM's ADSM with client adint2";
}

const DBMSrvBTools_ToolEnum tcn351_ADSMConnector::ToolEnum() const
{
    return DBMSrvBTools_ToolEnum(DBMSrvBTools_ToolEnum::ADSM);
}

int tcn351_ADSMConnector::CheckToolInstallation(int & IsInstalled)
{
    int rc=1;
    
    IsInstalled=1;
    
    LogWriter->Write("Checking existence and configuration of adint2.\n", tcn35d_DBBackupControllerLogWriter::AddALevelToPrefix);

    // The default value of the ADSM installation directory is set in the constructor
    // That default can be overridden by the 'ADINT' environment variable.
    // For security, it is exported if not found in the environment.

    rc=DetermineValueOfEnvironmentVariable("ADINT", ADSMDirectory, "adint2 directory", BackupInfo->DBName());

    if(rc) // all is ok, so far?
    {
        tni34_ADirectory TheADSMDirectory(ADSMDirectory, 0, 0); // do not create or check now

        rc=TheADSMDirectory.IsGood();   //check if directory object was constructed successful
                
        if(rc && !TheADSMDirectory.IsADirectory())
        {
            ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseADirectoryIsMissing, TheADSMDirectory.GiveName()));
            IsInstalled=0;
        }

        if(rc && IsInstalled)   // is all ok, so far?
        {
            // The ADSM client 'adint2' must be an executable program in that directory.
            const char * pszADSMProgramDefault=
            #if defined (WIN32)
                                         "\\adint2.exe";
            #elif defined (UNIX)
                                         "/adint2";
            #else
            #error tcn351_ADSMConnector::ToolIsInstalled() was only coded for UNIX and NT (define one of them).
            #endif

            if(!cn36_StrAlloc(ADSMProgram, strlen(TheADSMDirectory.GiveName())+strlen(pszADSMProgramDefault)))
                rc=0;
            else
            {
                SAPDB_strcpy(ADSMProgram, TheADSMDirectory.GiveName());
                strcat(ADSMProgram, pszADSMProgramDefault);

                if(!cn36_CheckExe(ADSMProgram))
                {
                    ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseAFileIsNotExecutable, ADSMProgram));
                    IsInstalled=0;
                }
            }

            if(rc && IsInstalled)  // is all ok, so far?
            {
                sprintf(MsgBuf_cn36,"Using ADSM program '%s'.\n", ADSMProgram);
                LogWriter->Write(MsgBuf_cn36);

                // The default name of the options file is just a convention,
                // it can be overridden by the environment variable 'ADA_OPT'.
                // As the DBName is not known to the ADSM client, this MUST be exported
                // if it was set in the program (== not already found in the environment).

                char * ADSMOptionsFileName=0;
                
                if(!cn36_StrAlloc(ADSMOptionsFileName, strlen(TheADSMDirectory.GiveName())+1+4+strlen(BackupInfo->DBName())+4))
                    rc=0;
                else
                {
                    #if defined (WIN32)
                    sprintf(ADSMOptionsFileName, "%s\\init%s.opt", TheADSMDirectory.GiveName(), BackupInfo->DBName());
                    #elif defined (UNIX)
                    sprintf(ADSMOptionsFileName, "%s/init%s.opt", TheADSMDirectory.GiveName(), BackupInfo->DBName());
                    #else
                        #error tcn351_ADSMConnector::CheckToolInstallation() was only coded for UNIX and NT (define one of them).
                    #endif

                    rc=DetermineValueOfEnvironmentVariable("ADA_OPT", ADSMOptionsFileName, "ADSM options file", BackupInfo->DBName());

                    if(rc)  // is all ok, so far?
                        if(!cn36_CheckRead(ADSMOptionsFileName))
                        {
                            ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseAFileIsMissingOrUnreadable, ADSMOptionsFileName));
                            IsInstalled=0;
                        }
                }
            }
        }
    }

    LogWriter->Write((rc && IsInstalled)?"Check passed successful.\n":"Check failed.\n", tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefixAndEndParagraph);

    return rc;
}

int tcn351_ADSMConnector::PrepareFirst()
{
    int rc=1;

    NumberOfRunningToolProcesses=0;

    switch(BackupInfo->DataDirection())
    {
        case tcn35d_BackupInfo::ABackup:
        {
            if(BuildLabel() && BuildBackupToolCall())
            {
                ADSMInput=new tni34_APipe(tni34_APipe::Out, 1, 0); //create a temporary pipe, but do not open it now

                if(0==ADSMInput)
                {
                    rc=0;
                    ErrorHandler->Report(tcn35d_DBBackupError::OutOfMemory.Set());
                }
                else
                    if(ADSMInput->IsGood())
                    {
                        sprintf(MsgBuf_cn36, "Created temporary pipe '%s' as input for adint2.\n", ADSMInput->GiveName());
                        LogWriter->Write(MsgBuf_cn36);
                    }
                    else
                    {
                        rc=0;
                        ErrorHandler->Report(tcn351_ADSMError(tcn351_ADSMError::CaseCanNotCreateADSMInputPipe));

                        delete ADSMInput;   //avoid any problems with the incorrect pipe object
                        ADSMInput=0;
                    }
            }
            else
                rc=0;
        }break;

        case tcn35d_BackupInfo::ARestore:
        {
            int          i=0;
            const char * StartOfEBID=0;
            const char * EndOfEBID=ExternalBackupIDList;

            cn36_StrDealloc(Label); //forget about any old labels

            while(rc && 0!=EndOfEBID && '\0'!=EndOfEBID[0])
            {
                i++;

                if(','==EndOfEBID[0])   // if end of previous EBID was a ',' ...
                    StartOfEBID=cn36_FirstNonWhiteSpaceOf(EndOfEBID+1);
                else
                    StartOfEBID=cn36_FirstNonWhiteSpaceOf(EndOfEBID);
                    
                EndOfEBID=strchr(StartOfEBID, ','); //search for the next ','

                if(0==EndOfEBID)
                    EndOfEBID=cn36_TerminatingZeroOf(StartOfEBID);

                if('\0'!=StartOfEBID[0])    //we have found a new EBID
                {
                    char * CopyOfEBID=0;

                    rc=cn36_StrNAllocCpy(CopyOfEBID, StartOfEBID, EndOfEBID-StartOfEBID);   //short cut for shorter code

                    if(rc)
                    {
                        *cn36_StartOfTrailingWhiteSpaces(CopyOfEBID)='\0';  //skip trailing white spaces

                        if(0==cn36_LoopedStrChr(CopyOfEBID, '_', 5))    //there must be at least 5 underscores in an EBID of adint2
                        {
                            rc=0;
                            ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseSyntaxErrorEBIDs, i));
                        }
                        else
                        {
                            int NumberOfPieces=0;
                            char * NewLabel=0;

                            if(0!=cn36_LoopedStrChr(CopyOfEBID, '_', 6))   //we could have a "_<number>pcs" or "_<number>ofn" from a parallel backup
                            {
                                const char * PcsSection=strrchr(CopyOfEBID, '_');

                                if(IsPiecesPart(PcsSection, strlen(PcsSection), NumberOfPieces))
                                    rc=cn36_StrAllocCpy(NewLabel, CopyOfEBID);
                                else
                                    if(IsPiecePart(PcsSection, NumberOfPieces))
                                        rc=ConstructParallelLabelFromEBID(NewLabel, CopyOfEBID, PcsSection, NumberOfPieces);
                            }

                            if(rc && 0==NewLabel)
                            {
                                rc=cn36_StrAllocCpy(NewLabel, CopyOfEBID);
                                NumberOfPieces=1;
                            }

                            if(rc)
                            {
                                if(0==Label)
                                    Label=NewLabel;
                                else
                                {
                                    if(0!=strcmp(Label, NewLabel))
                                    {
                                        rc=0;
                                        ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseEBIDsOfDifferentBackups));
                                    }
                                
                                    cn36_StrDealloc(NewLabel);  //throw it away, we have a string allready as Label
                                }
                            }

                            if(rc && NumberOfPieces>BackupInfo->NumberOfPipes())
                            {
                                rc=0;

                                if(i>BackupInfo->NumberOfPipes())   //Did we get also more "real" EBID's from the user? (one EBID from the user can be equal to a set of real EBID's)
                                    ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseTooManyEBIDs)); //if so we have too many EBID's
                                else
                                    ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseTooFewPipes));  //otherwise we can only say that the medium has too few pipes
                            }

                            if(rc && NumberOfPieces<BackupInfo->NumberOfPipes())
                            {
                                rc=0;
                                ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseTooManyPipes));
                            }

                            cn36_StrDealloc(CopyOfEBID);
                        }
                    }
                }
            }

            if(rc && 0==Label)
            {
                rc=0;
                ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseNeedExternalBackupIDForRestore));
            }

            if(rc)
            {
                if(BuildBackupToolCall())
                    ADSMInput=0;
                else
                    rc=0;
            }

        }break;

        case tcn35d_BackupInfo::GiveBackupInfo:
        {
            if(BuildLabel() && BuildBackupToolCall())
                ADSMInput=0;
            else
                rc=0;
        }break;

        default:
        {
            ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
            rc=0;
        }break;
    }

    if(rc)
    {
        if(0!=ToolProcessOutput[0])
            delete ToolProcessOutput[0];

        if(0!=ToolProcessError[0])
            delete ToolProcessError[0];

        ToolProcessOutput[0]=new tni34_AFile(tni34_AFile::ToDiskAppend, 0, 0); // do not create on open and do not open it now
        ToolProcessError[0]=new tni34_AFile(tni34_AFile::ToDiskAppend, 0, 0); // do not create on open and do not open it now

        if(0==ToolProcessOutput[0] || 0==ToolProcessError[0])
        {
            rc=0;
            ErrorHandler->Report(tcn35d_DBBackupError::OutOfMemory.Set());
        }
        else
        {
            if(ToolProcessOutput[0]->IsGood())
            {
                sprintf(MsgBuf_cn36, "Created temporary file '%s' as output for adint2.\n", ToolProcessOutput[0]->GiveName());
                LogWriter->Write(MsgBuf_cn36);
            }
            else
            {
                rc=0;
        
                delete ToolProcessOutput[0];    //avoid any problems with the incorrect file object
                ToolProcessOutput[0]=0;
            }

            if(ToolProcessError[0]->IsGood())
            {
                sprintf(MsgBuf_cn36, "Created temporary file '%s' as error output for adint2.\n", ToolProcessError[0]->GiveName());
                LogWriter->Write(MsgBuf_cn36);
            }
            else
            {
                rc=0;

                delete ToolProcessError[0];    //avoid any problems with the incorrect file object
                ToolProcessError[0]=0;
            }
        }
    }

    return rc;
}

int tcn351_ADSMConnector::PrepareAgain()
{
    int rc=1;

    if(tcn35d_BackupInfo::ABackup==BackupInfo->DataDirection() ||
       tcn35d_BackupInfo::ARestore==BackupInfo->DataDirection())
        rc=PrepareFirst();
    else
    {
        ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
        rc=0;
    }

    return rc;
}

int tcn351_ADSMConnector::StartTool()
{
    int i,rc=1;

    LogWriter->Write("Starting adint2.\n", tcn35d_DBBackupControllerLogWriter::AddALevelToPrefix);

    if(0==NumberOfRunningToolProcesses)
    {
        for(i=0;i<NumberOfCommandLines && rc;i++)
        {
            if(0!=ADSMInput)
            {
                sprintf(MsgBuf_cn36,
                        "Calling backup tool process '%s <%s >>%s 2>>%s' ... ",
                        CommandLine[i],
                        ADSMInput->GiveName(),
                        ToolProcessOutput[i]->GiveName(),
                        ToolProcessError[i]->GiveName());
            }
            else
            {
                sprintf(MsgBuf_cn36,
                        "Calling backup tool process '%s >>%s 2>>%s' ... ",
                        CommandLine[i],
                        ToolProcessOutput[i]->GiveName(),
                        ToolProcessError[i]->GiveName());
            }

            LogWriter->Write(MsgBuf_cn36);

            if(0!=ToolProcess[i])
                delete ToolProcess[i];

            ToolProcess[i]=new tni33_ABackgroundProcess(CommandLine[i],
                                                        ADSMInput,          //0==ADSMInput is OK.
                                                        ToolProcessOutput[i],
                                                        1,
                                                        ToolProcessError[i],
                                                        1);

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
                LogWriter->Write("OK.\n", tcn35d_DBBackupControllerLogWriter::NoPrefix);
            else
                LogWriter->Write("Failed.\n", tcn35d_DBBackupControllerLogWriter::NoPrefix);
        }

        if(rc && 1<=NumberOfRunningToolProcesses && 0!=ADSMInput)
            if(!ADSMInput->Open())
                rc=0;
    }
    else
    {
        ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
        rc=0;
    }

    LogWriter->Write(rc?"adint2 has been started successfully.\n":"Could not start adint2 correctly.\n",
                     tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefixAndEndParagraph);

    return rc;
}

int tcn351_ADSMConnector::SignalEndToTool(int DBWasSuccessful)
{
    int rc=1;

    if(ToolProcess[0]->IsRunning() && tcn35d_BackupInfo::ABackup==BackupInfo->DataDirection())
    {
        const char * ADSMMsg;
        long BytesWritten;

        if(DBWasSuccessful)
            ADSMMsg="COMMIT \n";
        else
            ADSMMsg="ROLLBACK \n";

        int wrc=ADSMInput->Write(ADSMMsg, long(strlen(ADSMMsg)), BytesWritten);

        if(-1==wrc)
            ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseCanNotSignalEndToTool));

        if(1!=rc)
            rc=0;
    }

    return rc;
}

int tcn351_ADSMConnector::CleanUpTool()
{
    int rc=1;

    if(0!=ADSMInput)
    {
        sprintf(MsgBuf_cn36,"Removing temporary adint2 input pipe %s ...", ADSMInput->GiveName());
        LogWriter->Write(MsgBuf_cn36);

        if(!ADSMInput->Remove())
        {
            LogWriter->Write(" Failed.\n", tcn35d_DBBackupControllerLogWriter::NoPrefix);
            rc=0;
        }
        else
            LogWriter->Write(" Done.\n", tcn35d_DBBackupControllerLogWriter::NoPrefix);

        delete ADSMInput;
        ADSMInput=0;
    }

    return rc;
}

int tcn351_ADSMConnector::CollectExternalBackupIDs(char * ExternalBackupID[MAX_NUMBER_OF_TOOLPROCESSES_CN35], int & NumberOfEBIDsFound)
{
    int rc=1;
        NumberOfEBIDsFound=0;

    if(1==BackupInfo->NumberOfPipes())
    {
        if(cn36_StrAllocCpy(ExternalBackupID[0], Label))
            NumberOfEBIDsFound=1;
        else
            rc=0;
    }
    else
    {
        int i;
        size_t LabelLengthWithoutPieces=strlen(Label)-strlen(strrchr(Label, '_'));

        for(i=0; rc && i<BackupInfo->NumberOfPipes(); i++)
        {
            rc=ConstructEBIDFromParallelLabel(ExternalBackupID[i], i, BackupInfo->NumberOfPipes(), Label, LabelLengthWithoutPieces);
            
            if(rc) 
                NumberOfEBIDsFound++;
        }
    }

    return rc;
}

int tcn351_ADSMConnector::GiveNextAvailableBackup(int          & EBIDIsAvailable,
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

        if(0!=TempEBIDList.First()) //did we generate a list of EBID's for one adint2-line containing a label of a parallel backup?
        {
            delete TempEBIDList.First()->GiveInfo();  //the first member of this list was already reported last time this function was called
            TempEBIDList.Del();
        }

        if(0!=TempEBIDList.First()) //if there is still something in the list
        {
            LineContainsEBID=1;                     //report it back
            EBID=TempEBIDList.First()->GiveInfo();
            EBIDLength=strlen(EBID);
            DataType=TempDataType;
            DateTimeString=TempDateTimeString;
        }
        else
            while(!LineContainsEBID && rc && !ReachedEnd)   //try to read the next line from adint2 output as usual
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

int tcn351_ADSMConnector::GiveInformationOfLine(const char                   *   Line,
                                                const char                   * & EBIDInLine,
                                                size_t                         & EBIDLength,
                                                tcn35d_BackupInfo::ADataType   & DataType,
                                                const char                   * & DateTimeString,
                                                int                            & LineContainsEBID)
{
    int rc=1;

    LineContainsEBID=0;

    if(0!=Line)                                 // we have a Line
    {
        Line=cn36_FirstNonWhiteSpaceOf(Line);

        if(!strncmp(Line, Label, strlen(Label)-1)) // Label has form "HostName_DBName_*", -1 to ignore "*" of Label
        {
            const char * EndOfEBID=cn36_FirstWhiteSpaceOf(Line);

            EBIDInLine=Line;
            EBIDLength=EndOfEBID-Line;

            const char * StartOfDate=cn36_LoopedStrNChr(Line, EBIDLength, '_', 2);   // date starts behind second '_'
            const char * StartOfType=0;
            const char * EndOfType=0;

            if(0!=StartOfDate)
            {
                StartOfDate++;                                                                 // ignore the '_'
                StartOfType=cn36_LoopedStrNChr(StartOfDate, EndOfEBID-StartOfDate, '_', 2);     // the action(type) string starts behind the time
            }

            if(0!=StartOfType)
            {
                StartOfType++;                                                 // ignore the '_'
                EndOfType=cn36_StrNChr(StartOfType, '_', EndOfEBID-StartOfType);
            }

            if(0!=EndOfType)                                            // we have found all '_' we were looking for
            {   
                size_t DateTimeLength=StartOfType-1-StartOfDate;        // -1 for the '_' in front of StartOfType

                if(!cn36_StrAlloc(TempDateTimeString, 19))
                    rc=0;
                else
                {
                    int TempDateTimeConstructed=0;

                    if(17==DateTimeLength && cn36_StrNConsistsOfDigits(StartOfDate, 2))     // year is coded with only two digits, must be a migration backup from 6.2 or 7.1
                    {
                        int Year;

                        if(cn36_StrNToInt(Year, StartOfDate, 2))
                        {
                            if(Year<30)
                                SAPDB_strcpy(TempDateTimeString, "20");               // init missing century with 00
                            else
                                SAPDB_strcpy(TempDateTimeString, "19");

                            cn36_StrNCpy(TempDateTimeString+2, StartOfDate, DateTimeLength);    // +2 for missing century
                            TempDateTimeConstructed=1;
                        }
                    }
                    else
                        if(19==DateTimeLength)
                        {
                            cn36_StrNCpy(TempDateTimeString, StartOfDate, DateTimeLength);
                            TempDateTimeConstructed=1;
                        }

                    if(TempDateTimeConstructed)    // we have successfully constructed/copied the date to TempDateTimeString
                    {
                        char * FirstDot=TempDateTimeString+4;
                        char * SecondDot=TempDateTimeString+7;
                        char * StartOfTime=TempDateTimeString+10;
                        char * ThirdDot=TempDateTimeString+13;
                        char * FourthDot=TempDateTimeString+16;
                
                        *FirstDot=*SecondDot=*StartOfTime=*ThirdDot=*FourthDot='0'; //set temporary all possible non digits to '0'

                        if(cn36_StrConsistsOfDigits(TempDateTimeString))    // only digits should remain in the string
                        {
                            *FirstDot=*SecondDot='-';
                            *StartOfTime=' ';
                            *ThirdDot=*FourthDot=':';

                            DateTimeString=TempDateTimeString;

                            rc=DataTypeForActionString(DataType, StartOfType, EndOfType-StartOfType);

                            if(rc)
                            {
                                //check for optional session column
                                bool foundSessionColumn=false;
                                int  numberOfSessionsInSessionColumn=0;
                                const char * sessionColumn=cn36_FirstNonWhiteSpaceOf(EndOfEBID);
                                
                                if('['==sessionColumn[0])
                                {
                                    const char * endSessionColumn=strchr(++sessionColumn, ']');

                                    if(0!=endSessionColumn && 0<(endSessionColumn-sessionColumn)) // found "[<something>]", <something> is not empty
                                    {
                                        const char * p=cn36_StrNStr(sessionColumn, endSessionColumn-sessionColumn, "of");

                                        if(0==p)
                                            p=endSessionColumn;

                                        if(0<(p-sessionColumn) &&
                                           cn36_StrNToInt(numberOfSessionsInSessionColumn, sessionColumn, p-sessionColumn) &&
                                           0!=numberOfSessionsInSessionColumn)
                                        {
                                            foundSessionColumn=true;
                                        }
                                    }
                                }

                                //check for pieces part in label
                                const char * PcsSection=0;
                                int  NumberOfPieces=1;

                                if(0!=cn36_StrNChr(EndOfType+1, '_', EndOfEBID-(EndOfType+1)))    //there is either a _ in the media name or it was a parallel backup with a "_<number>pcs" section
                                {
                                    PcsSection=cn36_StrNRChr(EndOfType+1, EndOfEBID-(EndOfType+1), '_');
                                    
                                    if(!IsPiecesPart(PcsSection, EndOfEBID-PcsSection, NumberOfPieces))
                                    {
                                        PcsSection=0;
                                        NumberOfPieces=1;
                                    }
                                }

                                if(!foundSessionColumn || numberOfSessionsInSessionColumn==NumberOfPieces)
                                {
                                    LineContainsEBID=1;

                                    if(0!=PcsSection)
                                    {
                                        int i;

                                        for(i=0; rc && i<NumberOfPieces; i++)
                                        {
                                            char * h=0;

                                            rc=ConstructEBIDFromParallelLabel(h, i, NumberOfPieces, Line, PcsSection-Line);

                                            if(rc && 0==TempEBIDList.Add(h))
                                            {
                                                cn36_StrDealloc(h); //last chance to free the memory
                                                rc=0;
                                                ErrorHandler->Report(tcn35d_DBBackupError::OutOfMemory.Set());
                                            }
                                        }

                                        if(rc)  //if successfully created list of parts of the parallel backup ...
                                        {
                                            TempEBIDList.Reverse(); //reverse the list to get an ascending order (_1ofn, _2ofn, ...)

                                            EBIDInLine=TempEBIDList.First()->GiveInfo();    //change EBID to the first list entry
                                            EBIDLength=strlen(EBIDInLine);

                                            TempDataType=DataType;  //keep that for the other parts of this parallel backup
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return rc;
}

int tcn351_ADSMConnector::BuildLabel()
{
    int rc=0;

    switch(BackupInfo->DataDirection())
    {
        case tcn35d_BackupInfo::ABackup:
        {
            char * NewLabel=0;

            rc=1;

            do
            {
                time_t ltime ;
                char   szDatTim[30];
                char   szPieces[20];

                // Get current date and time, convert according to timezone, make string.
                // Time separator is '.' instead of ':' because of NT name analysis!
                time(&ltime);
                strftime(szDatTim, sizeof(szDatTim), "%Y.%m.%d_%H.%M.%S", localtime(&ltime));

                if(BackupInfo->NumberOfPipes()>1)
                    sprintf(szPieces, "_%dpcs", BackupInfo->NumberOfPipes());
                else
                    szPieces[0]='\0';

                char * AdjustedMediumName=0;

                cn36_StrAllocCpy(AdjustedMediumName, BackupInfo->MediaName());

                if(0!=AdjustedMediumName)   //if we got a valid copy
                {
                    int    NumberOfPieces;
                    char * LastUnderlineInMediumName=strrchr(AdjustedMediumName, '_');  //search the last underline 

                    while(0!=LastUnderlineInMediumName &&                                                                  //as long as there is a last underline and
                          (IsPiecesPart(LastUnderlineInMediumName, strlen(LastUnderlineInMediumName), NumberOfPieces) ||   //(this underline is a start for "_<number>pcs" or
                           IsPiecePart(LastUnderlineInMediumName, NumberOfPieces)    ))                                    // this underline is a start for "_<number>of<totalnumber>")
                    {
                        *LastUnderlineInMediumName='\0';                                //skip that part from the medium name to avoid confusion with labels of parallel backups
                        LastUnderlineInMediumName=strrchr(AdjustedMediumName, '_');     //search for the "next" last underline
                    }

                    cn36_RemoveSpaces(AdjustedMediumName); //remove any spaces from medianame either, to avoid conflicts with "adints -q ..." session column

                    const char * ActionString;

                    if(ActionStringForDataType(ActionString, BackupInfo->DataType()))
                    {
                        if(cn36_StrAlloc(NewLabel, strlen(BackupInfo->NodeName())+1
                                                   +strlen(BackupInfo->DBName())+1
                                                   +strlen(szDatTim)+1
                                                   +strlen(ActionString)+1
                                                   +strlen(AdjustedMediumName)
                                                   +strlen(szPieces)))
                        {
                            sprintf(NewLabel,
                                    "%s_%s_%s_%s_%s%s",
                                    BackupInfo->NodeName(),
                                    BackupInfo->DBName(),
                                    szDatTim,
                                    ActionString,
                                    AdjustedMediumName,
                                    szPieces);

                            size_t iChars=strlen(NewLabel);
                            // date+time: 19 chars, 4 more components, 4 separators => min. 27 chars
                            if(iChars<25)
                            {
                                rc=0;
                                ErrorHandler->Report(tcn351_ADSMError(tcn351_ADSMError::CaseCanNotBuildBackupLabel, int(iChars)));
                            }
                        }
                        else
                            rc=0;
                    }
                    else
                        rc=0;

                    cn36_StrDealloc(AdjustedMediumName);
                }
                else
                    rc=0;
            }
            while(rc && 0!=Label && 0==strcmp(Label, NewLabel));  //try until the new label differs from the previous one (to prevent the same EBID for two stage files)

            if(rc)
            {
                cn36_StrDealloc(Label);
                Label=NewLabel;

                sprintf(MsgBuf_cn36,"Constructed save label '%s'.\n", Label);
                LogWriter->Write(MsgBuf_cn36);
            }
            else
                cn36_StrDealloc(NewLabel);

        }break;

        case tcn35d_BackupInfo::GiveBackupInfo:
        {
            size_t LengthOfLabel=strlen(BackupInfo->SourceNodeName())+1+strlen(BackupInfo->SourceDBName())+2;

            if(cn36_StrAlloc(Label, LengthOfLabel))
            {
                sprintf(Label, "%s_%s_*", BackupInfo->SourceNodeName(), BackupInfo->SourceDBName());

                if(strlen(Label)!=LengthOfLabel)
                    ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
                else
                {
                    sprintf(MsgBuf_cn36,"Constructed save label '%s'.\n", Label);
                    LogWriter->Write(MsgBuf_cn36);
                    rc=1;
                }
            }
        }break;

        default:
            ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
    }

    return rc;
}

int tcn351_ADSMConnector::BuildBackupToolCall()
{
    char * Call=0;
    int rc=0;
    const char * Adint2ActionTypeFlag;

    if(NumberOfCommandLines!=0) //if old commandlines exists, free the memory
    {
        int i;

        for(i=0;i<NumberOfCommandLines;i++)
            if(0!=CommandLine[i])
            {
                delete [] CommandLine[i];
                CommandLine[i]=0;
            }
        
        NumberOfCommandLines=0;
    }

    switch(BackupInfo->DataDirection()) //determine adint2-action-flag
    {
        case tcn35d_BackupInfo::ABackup:
            Adint2ActionTypeFlag="-b";
            break;
        case tcn35d_BackupInfo::ARestore:
            Adint2ActionTypeFlag="-r";
            break;
        case tcn35d_BackupInfo::GiveBackupInfo:
            Adint2ActionTypeFlag="-q";
            break;
        default:
            ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
    }

    char szBlockSize[20];
    sprintf(szBlockSize,"%d", BackupInfo->BlockSize());

    int i;
    size_t Length=strlen(ADSMProgram)                       // "...\adint2"
                  +1+strlen(Adint2ActionTypeFlag)           // " -b", "-r" or "-q"
                  +4+strlen(Label);                         // " -n <label>"

    if(tcn35d_BackupInfo::ABackup==BackupInfo->DataDirection() || tcn35d_BackupInfo::ARestore==BackupInfo->DataDirection())
    {
        Length+=4+strlen(szBlockSize);                      // " -c <BlockSize>"
        Length+=BackupInfo->NumberOfPipes()*strlen(" -p ");               // " -p <pipe>" for every pipe 

        for(i=0;i<BackupInfo->NumberOfPipes();i++)
            Length+=strlen(BackupInfo->NameOfPipe(i));
    }

    if(cn36_StrAlloc(Call, Length))                         // the new commandline has length Length
    {
        sprintf(Call, "%s %s -n %s", ADSMProgram, Adint2ActionTypeFlag, Label);

        if(tcn35d_BackupInfo::ABackup==BackupInfo->DataDirection() || tcn35d_BackupInfo::ARestore==BackupInfo->DataDirection())
        {
            strcat(Call, " -c ");
            strcat(Call, szBlockSize);

            for(i=0; i<BackupInfo->NumberOfPipes(); i++)
            {
                strcat(Call, " -p ");
                strcat(Call, BackupInfo->NameOfPipe(i));
            }
        }

        if(Length!=strlen(Call))
            ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
        else
        {
            NumberOfCommandLines=1; // we start only one tool process, while using ADSM with adint2
            CommandLine[0]=Call;
            rc=1;

            sprintf(MsgBuf_cn36, "Constructed adint2 call '%s'.\n", Call);
            LogWriter->Write(MsgBuf_cn36);
        }
    }

    return rc;
}

const int tcn351_ADSMConnector::NumberOfDataTypeActionStringMapEntries=5;

struct tcn351_ADSMConnector::DataTypeActionStringMapEntry tcn351_ADSMConnector::DataTypeActionStringMap[tcn351_ADSMConnector::NumberOfDataTypeActionStringMapEntries]=
{
    {tcn35d_BackupInfo::DataRecovery, "SAVEDTNCHK"},
    {tcn35d_BackupInfo::DataMigration, "SAVEDATA"},
    {tcn35d_BackupInfo::PagesRecovery, "SAVEPGNCHK"},
    {tcn35d_BackupInfo::PagesMigration, "SAVEPAGES"},
    {tcn35d_BackupInfo::Log, "SAVELOG"}
};

int tcn351_ADSMConnector::DataTypeForActionString(tcn35d_BackupInfo::ADataType & DataType,
                                                  const char                   * StartOfActionString,
                                                  size_t                         LengthOfActionString)
{
    int rc=1;

    if(0!=StartOfActionString)
    {
        int i=0, Found=0;

        for(i=0; !Found && i<NumberOfDataTypeActionStringMapEntries; i++)
            if(0==cn36_StrStrNCmp(DataTypeActionStringMap[i].ActionString, StartOfActionString, LengthOfActionString))
            {
                Found=1;
                DataType=DataTypeActionStringMap[i].DataType;
            }

        if(!Found)
            rc=0;
    }

    return rc;    
}

int tcn351_ADSMConnector::ActionStringForDataType(const char                   * & ActionString,
                                                  tcn35d_BackupInfo::ADataType     DataType)
{
    int rc=1;

    int i, Found=0;

    for(i=0; !Found && i<NumberOfDataTypeActionStringMapEntries; i++)
        if(DataType==DataTypeActionStringMap[i].DataType)
        {
            Found=1;
            ActionString=DataTypeActionStringMap[i].ActionString;
        }

    if(!Found)
        ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));

    return rc;
}

int tcn351_ADSMConnector::ConstructEBIDFromParallelLabel(char       *& NewEBID,
                                                         int           NumberOfEBID,
                                                         int           NumberOfPieces,
                                                         const char *  TheLabel,
                                                         size_t        LabelLengthWithoutPieces)
{
    int rc=1;
    
    char PartSuffix[100];

    sprintf(PartSuffix, "_%dof%d", NumberOfEBID+1, NumberOfPieces);   //99 byte should be large enough for that sprintf

    if(cn36_StrAlloc(NewEBID, LabelLengthWithoutPieces+strlen(PartSuffix)))
    {
        cn36_StrNCpy(NewEBID, TheLabel, LabelLengthWithoutPieces);
        SAPDB_strcpy(NewEBID+LabelLengthWithoutPieces, PartSuffix);
    }
    else
        rc=0;

    return rc;
}

int tcn351_ADSMConnector::ConstructParallelLabelFromEBID(char       *& ParallelLabel,
                                                         const char *  EBID,
                                                         const char *  PieceSection,
                                                         int           NumberOfPieces)
{
    int    rc;
    char   PiecesPart[20];
    size_t LengthEBID=strlen(EBID);

    sprintf(PiecesPart, "_%dpcs", NumberOfPieces);

    rc=cn36_StrAlloc(ParallelLabel, LengthEBID-strlen(PieceSection)+strlen(PiecesPart));

    if(rc)
    {
        cn36_StrNCpy(ParallelLabel, EBID, LengthEBID-strlen(PieceSection));
        SAPDB_strcpy(ParallelLabel+LengthEBID-strlen(PieceSection), PiecesPart);
    }

    return rc;
}

int tcn351_ADSMConnector::IsPiecesPart(const char * Part, size_t LengthPart, int & NumberOfPieces)
{
    int rc=0;

    if(0!=Part)
    {
        if(LengthPart>1+strlen("pcs") &&                                        //Part has room for a "_"+something+"pcs"
           '_'==Part[0] &&                                                      //Part starts with an '_'
           0==strncmp(Part+LengthPart-strlen("pcs"), "pcs", strlen("pcs")) &&   //Part ends with a "pcs"
           cn36_StrNConsistsOfDigits(Part+1, LengthPart-1-strlen("pcs")) &&     //the something in Part is a number
           cn36_StrNToInt(NumberOfPieces, Part+1, LengthPart-1-strlen("pcs")))  //that number could be converted to an int without overflow
        {
            rc=1;
        }
    }

    return rc;
}

int tcn351_ADSMConnector::IsPiecePart(const char * Part, int & NumberOfPieces)
{
    int rc=0;

    if(0!=Part)
    {
        size_t LengthPart=strlen(Part);

        if(LengthPart>1+strlen("of") &&
           '_'==Part[0])
        {
            const char * PositionOf=strstr(Part, "of");
            int PartNumber;

            if(0!=PositionOf &&
               cn36_StrNConsistsOfDigits(Part+1, PositionOf-1-Part) &&
               cn36_StrConsistsOfDigits(PositionOf+strlen("of")) &&
               cn36_StrNToInt(PartNumber, Part+1, PositionOf-1-Part) &&
               cn36_StrToInt(NumberOfPieces, PositionOf+strlen("of")))
            {
                if(0<PartNumber && PartNumber<=NumberOfPieces)  //that must hold for a vaild piece part "_<PartNumber>of<NumberOfPieces>" too
                    rc=1;
            }
        }
    }

    return rc;
}
