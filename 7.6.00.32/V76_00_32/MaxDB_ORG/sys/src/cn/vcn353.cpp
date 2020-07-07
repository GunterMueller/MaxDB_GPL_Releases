/*!
    \file    vcn353.cpp
    \author  TiloH
    \ingroup cn layer
    \brief   implementing connection between dbmsrv and Backint for MaxDB
             conforming backup tools

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2000-2006 SAP AG

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

#include "gcn003.h"
#include "hni34.h"
#include "hcn353.h"
#include "hcn36.h"
#include "hcn42.h"

#include "SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_EBIDHistoryFile.hpp"

#include <limits.h>
#include <string.h>


// -----------------------------------------------------------------------------
// constants defined by the Backint interface
// -----------------------------------------------------------------------------

#define MAX_LENGTH_BID 16
#define MAX_LENGTH_BSIPATH 255

class tcn353_BackintError: public tcn35d_DBBackupError
{
  public:
    enum BackintErrorCase
    {   
        CasePipeOrFileNameTooLong,
        CaseUnspecifiedBackintPath,
        CaseTwoDifferentBIDs,
        CaseWrongBIDReturned,
        CaseWrongFilesProcessed,
        CaseCouldNotBackupBackupHistoryFiles,
        CaseCouldNotRestoreBackupHistoryFiles,
        CaseCouldNotReadExternalBackupHistory,
        CaseUnknownBackintOutputFormat
    };

    tcn353_BackintError(BackintErrorCase ErrorCase);
    tcn353_BackintError(BackintErrorCase ErrorCase, const char * ErrInfo);

  private:
    static DBBackupErrorDescription BackintErrorToText[];
    void   SetThisError(int CaseToSet);
};

tcn353_BackintError::tcn353_BackintError(BackintErrorCase ErrorCase)
:tcn35d_DBBackupError()
{
    SetThisError(ErrorCase);
    Set();
}

tcn353_BackintError::tcn353_BackintError(BackintErrorCase ErrorCase, const char * ErrInfo)
:tcn35d_DBBackupError()
{
    SetThisError(ErrorCase);
    Set(ErrInfo);
}

void tcn353_BackintError::SetThisError(int CaseToSet)
{
    if(BackintErrorToText[CaseToSet].Case==CaseToSet)
        ThisError=BackintErrorToText+CaseToSet;
    else
    {
        ThisError=BackintErrorToText;
        
        while(ThisError->Case!=CaseToSet && ThisError->Case!=CaseUnknownBackintOutputFormat)
            ThisError++;

        if(CaseToSet!=ThisError->Case)
            ThisError=GiveDescriptionOfInternalError();
    }
}

tcn353_BackintError::DBBackupErrorDescription tcn353_BackintError::BackintErrorToText[]=
{
    {CasePipeOrFileNameTooLong,
     AString,
     "A pipe or file name is too long.",
     "The pipe or file name '%s' is too long."},

    {CaseUnspecifiedBackintPath,
     AString,
     "Path of the Backint for MaxDB program was not specified in the configuration file.",
     "Path of the Backint for MaxDB program was not specified in the configuration file '%s'."},

    {CaseTwoDifferentBIDs,
     AString,
     "Backint returned two different backup IDs for the same file or pipe.",
     "Backint returned two different backup IDs for '%s'."},

    {CaseWrongBIDReturned,
     AString,
     "Backint for MaxDB returned the wrong BID for a file or a pipe.",
     "Backint for MaxDB returned the wrong BID for '%s'."},
     
    {CaseWrongFilesProcessed,
     AString,
     "A wrong file was processed by Backint for MaxDB.",
     "A wrong file ('%s') was processed by Backint for MaxDB."},

    {CaseCouldNotBackupBackupHistoryFiles,
     None,
     "Could not save the Backup History or the External Backup History with Backint for MaxDB.",
     0},

    {CaseCouldNotRestoreBackupHistoryFiles,
     None,
     "Could not restore the Backup History or the External Backup History with Backint for MaxDB.",
     0},

    {CaseCouldNotReadExternalBackupHistory,
     AString,
     "Could not read the External Backup History.\n",
     "Could not read the External Backup History.\n%s"},

    {CaseUnknownBackintOutputFormat,
     AString,
     "Found an unknown format in the Backint for MaxDB output file.",
     "Found an unknown format in the Backint for MaxDB output file in line '%s'."}
};

// -----------------------------------------------------------------------------
// implementation of class  tcn353_Backint2InputLine
// -----------------------------------------------------------------------------

class tcn353_Backint2InputLine
{
  public:
    enum BackintActionType{Backup, Inquire, Restore};

    tcn353_Backint2InputLine(tcn35d_DBBackupControllerErrorHandler * TheErrorHandler,
                             tcn35d_DBBackupControllerLogWriter    * TheLogWriter);
    ~tcn353_Backint2InputLine();

    int SetValues(const char * TheName,
                  int          IsAPipe,
                  const char * TheBID=0,
                  const char * TheDestination=0,
                  const char * EndOfDestination=0,
                  tcn35d_BackupInfo::ADataType TheDataType=tcn35d_BackupInfo::DataRecovery,
                  const char * TheDateTime=0);

    int GenerateInputLine(char * & TheLine, BackintActionType Action);

    int TestAgainstOutputLine(const BackintActionType Action,
                              int &                   InputAndOutputLineAreEqual,
                              int &                   FoundForTheFirstTime,
                              const char *            OutFileName,
                              size_t                  OutFileNameLength,
                              const char *            OutFileBID,
                              size_t                  OutFileBIDLength);

    int SetCompleteExternalBID(const char * DBName);

    const char * GiveName() const;
    const char * GiveBID() const;
    const char * GiveExternalBackupID() const;
    tcn35d_BackupInfo::ADataType GiveDataType() const;
    const char * GiveDateTime() const;
    int          WasFound() const;

  private:
    int    IsPipe;

    char * Name;
    char * Destination;
    char * BID;
    char * EBID;

    char * DateTime;

    tcn35d_BackupInfo::ADataType DataType;
    int AllreadyFound;
    
    tcn35d_DBBackupControllerErrorHandler * ErrorHandler;
    tcn35d_DBBackupControllerLogWriter    * LogWriter;
};

tcn353_Backint2InputLine::tcn353_Backint2InputLine(tcn35d_DBBackupControllerErrorHandler * TheErrorHandler,
                                                   tcn35d_DBBackupControllerLogWriter    * TheLogWriter)
    :IsPipe(0),
    Name(0),
    Destination(0),
    BID(0),
    EBID(0),
    DataType(tcn35d_BackupInfo::DataRecovery),
    DateTime(0),
    AllreadyFound(0),
    ErrorHandler(TheErrorHandler),
    LogWriter(TheLogWriter)
{
}

tcn353_Backint2InputLine::~tcn353_Backint2InputLine()
{
    cn36_StrDealloc(Name);
    cn36_StrDealloc(Destination);
    cn36_StrDealloc(BID);
    cn36_StrDealloc(EBID);
    cn36_StrDealloc(DateTime);
}

int tcn353_Backint2InputLine::SetValues(const char                   * TheName,
                                        int                            IsAPipe,
                                        const char                   * TheBID,
                                        const char                   * TheDestination,
                                        const char                   * EndOfDestination,
                                        tcn35d_BackupInfo::ADataType   TheDataType,
                                        const char                   * TheDateTime)
{
    int rc=1;

    if(strlen(TheName)>MAX_LENGTH_BSIPATH) // Backint for MaxDB allows only 256 chars in a path
    {
        rc=0;
        ErrorHandler->Report(tcn353_BackintError(tcn353_BackintError::CasePipeOrFileNameTooLong, TheName));
    }

    //if(0!=TheBID && strlen(TheBID)>MAX_LENGTH_BID)        rc=0;

    if(0!=TheDestination && EndOfDestination-TheDestination>MAX_LENGTH_BSIPATH)
    {
        rc=0;

        char *HelpString;

        if(cn36_StrNAllocCpy(HelpString, TheDestination, EndOfDestination-TheDestination))  //try to get a copy of the string for a better error message
        {
            ErrorHandler->Report(tcn353_BackintError(tcn353_BackintError::CasePipeOrFileNameTooLong, HelpString));
            cn36_StrDealloc(HelpString);
        }
        else
            ErrorHandler->Report(tcn353_BackintError(tcn353_BackintError::CasePipeOrFileNameTooLong));
    }

    if(rc)
    {
        IsPipe=IsAPipe;
        
        rc=cn36_StrAllocCpy(Name, TheName);

        if(rc)
        {
            if(0!=TheBID)
                rc=cn36_StrAllocCpy(BID, TheBID);
            else
                cn36_StrDealloc(BID);
        }

        if(rc)
        {
            if(0!=TheDestination)
                rc=cn36_StrNAllocCpy(Destination, TheDestination, EndOfDestination-TheDestination);
            else
                cn36_StrDealloc(Destination);
        }

        DataType=TheDataType;

        if(rc)
        {
            if(0!=TheDateTime)
                rc=cn36_StrAllocCpy(DateTime, TheDateTime);
            else
                cn36_StrDealloc(DateTime);
        }
    }

    return rc;
}

int tcn353_Backint2InputLine::GenerateInputLine(char * & TheLine, BackintActionType Action)
{
    int rc=1;

    switch(Action)
    {
        case Backup:
            if(IsPipe)
            {
                const char * PipeKeyWord="#PIPE";

                if(!cn36_StrAlloc(TheLine, strlen(Name)+1+strlen(PipeKeyWord)))
                    rc=0;
                else
                    sprintf(TheLine, "%s %s", Name, PipeKeyWord);   // "<pipename> #PIPE"
            }
            else
            {
                if(!cn36_StrAllocCpy(TheLine, Name))    // "<filename>"
                    rc=0;
            }
            break;

        case Inquire:
        case Restore:
        {
            size_t Length;
            int    UseDestination=0;
            const char * EffectiveBID;

            if(0!=BID && BID[0]!='\0')
                EffectiveBID=BID;
            else
                EffectiveBID="#NULL";

            Length=strlen(EffectiveBID)+1+strlen(Name);

            if(Restore==Action && 0!=Destination && '\0'!=Destination[0])
            {
                Length+=1+strlen(Destination);
                UseDestination=1;
            }

            if(!cn36_StrAlloc(TheLine, Length))
                rc=0;
            else
            {
                if(UseDestination)
                    sprintf(TheLine, "%s %s %s", EffectiveBID, Name, Destination);  // "<BID>|#NULL <filename>|<pipename> <destination>"
                else
                    sprintf(TheLine, "%s %s", EffectiveBID, Name);                  // "<BID>|#NULL <filename>|<pipename>
            }
        }break;

        default:
            ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
            rc=0;
    }

    return rc;
}

int tcn353_Backint2InputLine::TestAgainstOutputLine(const BackintActionType Action,
                                                    int &                   InputAndOutputLineAreEqual,
                                                    int &                   FoundForTheFirstTime,
                                                    const char *            OutFileName,
                                                    size_t                  OutFileNameLength,
                                                    const char *            OutFileBID,
                                                    size_t                  OutFileBIDLength)
{
    int rc=1,
        BIDsAreEqual=0;

    FoundForTheFirstTime=InputAndOutputLineAreEqual=0;

    if(!cn36_StrStrNCmp(Name, OutFileName, OutFileNameLength)) // if it is the input and output have the same name
	{
        if(0==BID)
        {
            rc=cn36_StrNAllocCpy(BID, OutFileBID, OutFileBIDLength);
            BIDsAreEqual=1;
        }
        else
        {
            if(0==cn36_StrStrNCmp(BID, OutFileBID, OutFileBIDLength))
            {
                BIDsAreEqual=1;

                if(AllreadyFound)
                {
                    sprintf(MsgBuf_cn36, "BID '%s' was reported twice for '%s'! This is ignored.\n", BID, Name);
                    LogWriter->Write(MsgBuf_cn36);
                }
            }
        }

        switch(Action)
        {
            case Backup:
            case Restore:
                if(0==BIDsAreEqual)
                {
                    rc=0;

                    if(AllreadyFound)
                        ErrorHandler->Report(tcn353_BackintError(tcn353_BackintError::CaseTwoDifferentBIDs, Name));
                    else
                        ErrorHandler->Report(tcn353_BackintError(tcn353_BackintError::CaseWrongBIDReturned, Name));
                }
                else
                    InputAndOutputLineAreEqual=1;
            break;

            case Inquire:
                if(BIDsAreEqual)
                    InputAndOutputLineAreEqual=1;   // just ignore other pairs of BID/name while making an inquire!
            break;

            default:
                ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
                rc=0;
        }

        if(rc && InputAndOutputLineAreEqual)
        {
            if(!AllreadyFound)
                FoundForTheFirstTime=1;

            AllreadyFound=1; // the Name (and BID) was found in the output file
        }
    }

    return rc;
}

int tcn353_Backint2InputLine::SetCompleteExternalBID(const char * DBName)
{
    return tcn353_Backint2Connector::BuildAEBIDFrom(EBID, DBName, BID, Name);
}

const char * tcn353_Backint2InputLine::GiveName() const
{
    return Name;
}

const char * tcn353_Backint2InputLine::GiveBID() const
{
    return BID;
}

const char * tcn353_Backint2InputLine::GiveExternalBackupID() const
{
    return EBID;
}

tcn35d_BackupInfo::ADataType tcn353_Backint2InputLine::GiveDataType() const
{
    return DataType;
}

const char * tcn353_Backint2InputLine::GiveDateTime() const
{
    return DateTime;
}

int tcn353_Backint2InputLine::WasFound() const
{
    return AllreadyFound;
}


// -----------------------------------------------------------------------------
// implementation of class  tcn353Backint2DBMKNLLine
// -----------------------------------------------------------------------------

class tcn353Backint2DBMKNLLine  //TODO something like that should be included in vcn32.cpp
{
  public:
    tcn353Backint2DBMKNLLine();
    ~tcn353Backint2DBMKNLLine();

    int TakeBackint2InfoFromDBMKNLLine(tcn35d_DBBackupControllerErrorHandler * ErrorHandler,
                                       int                                   & IsABackint2Line,
                                       const char                            * ADBMKNLLine,
                                       const char                            * NameOfDBMKNL);

    const char * GiveKeyLabel() const;
    const char * GiveDateTime() const;
    
    tcn35d_BackupInfo::ADataType GiveType() const;

  private:
    char * StringData;    // just for easier handling of dynamically allocated memory

    const char * KeyLabel;
    const char * DateTime;

    tcn35d_BackupInfo::ADataType DataType;

    static const char SectionDelimiter;
};

const char tcn353Backint2DBMKNLLine::SectionDelimiter='|';

tcn353Backint2DBMKNLLine::tcn353Backint2DBMKNLLine()
    :StringData(0),
    KeyLabel(0),
    DateTime(0)
{
}

tcn353Backint2DBMKNLLine::~tcn353Backint2DBMKNLLine()
{
    cn36_StrDealloc(StringData);
}

int tcn353Backint2DBMKNLLine::TakeBackint2InfoFromDBMKNLLine(tcn35d_DBBackupControllerErrorHandler * ErrorHandler,
                                                             int                                   & IsABackint2Line,
                                                             const char                            * ADBMKNLLine,
                                                             const char                            * NameOfDBMKNL)
{
    int rc=1;

    IsABackint2Line=0;

    if('\0'!=ADBMKNLLine[0])
    {
        const char * Medianame=cn36_LoopedStrChr(ADBMKNLLine, SectionDelimiter, 10);   //the media name starts after the 10th '|'

        if(0==Medianame || '\0'==Medianame[0])
        {
            rc=0;
            ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseUnknownFormatOfDBMKnlFile, NameOfDBMKNL));
        }
        else
        {                                                               // we have at least 10 '|' in the string, so the subsequent calls to cn36_LoopedStrChr() and cn36_LoopedStrChr can be made without danger
            DBMSrvBTools_ToolEnum BackupToolType(++Medianame);

            if(DBMSrvBTools_ToolEnum::BACK==BackupToolType) // was the backup of the ADBMKNLLine made with Backint for MaxDB?
            {
                const char * EndKeyLabel;
                
                KeyLabel   =ADBMKNLLine;
                EndKeyLabel=cn36_LoopedStrChr(KeyLabel, SectionDelimiter, 2);

                if(0==strncmp("SAVE ", EndKeyLabel+1, strlen("SAVE "))) // the action string must start with SAVE_ all other entries (RESTORE, HISTLOST, ...) must be ignored
                {
                    const char * EndDateTime;
                    const char * LogStatus;
                    const char * EndLogStatus;
                    int NeedLog=0;

                    IsABackint2Line=1;

                    DateTime   =cn36_LoopedStrChr(EndKeyLabel+1, SectionDelimiter, 3)+1;
                    EndDateTime=cn36_StrChr(DateTime+1, SectionDelimiter);

                    LogStatus   =cn36_LoopedStrChr(EndDateTime+1, SectionDelimiter, 3)+1;
                    EndLogStatus=cn36_StrChr(LogStatus+1, SectionDelimiter);

                    if(0==cn36_StrStrNCmp("YES", LogStatus, EndLogStatus-LogStatus))
                        NeedLog=1;
                    else
                        if(0!=cn36_StrStrNCmp("NO ", LogStatus, EndLogStatus-LogStatus) &&
                           0!=cn36_StrStrNCmp("   ", LogStatus, EndLogStatus-LogStatus))
                        {
                            rc=0;
                            ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseUnknownFormatOfDBMKnlFile, NameOfDBMKNL));
                        }

                    if(rc)
                    {
                        const char * Label=cn36_StrChr(KeyLabel, SectionDelimiter)+1;

                        if(0==strncmp(Label, "DAT_", strlen("DAT_")))
                        {
                            if(NeedLog)
                                DataType=tcn35d_BackupInfo::DataRecovery;
                            else
                                DataType=tcn35d_BackupInfo::DataMigration;
                        }
                        else
                            if(0==strncmp(Label, "PAG_", strlen("PAG_")))
                            {
                                if(NeedLog)
                                    DataType=tcn35d_BackupInfo::PagesRecovery;
                                else
                                    DataType=tcn35d_BackupInfo::PagesMigration;
                            }
                            else
                                if(0==strncmp(Label, "LOG_",strlen("LOG_")))
                                    DataType=tcn35d_BackupInfo::Log;
                                else
                                {
                                    rc=0;
                                    ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseUnknownFormatOfDBMKnlFile, NameOfDBMKNL));
                                }
                    }

                    if(rc)
                        rc=cn36_StrAlloc(StringData, (EndKeyLabel-KeyLabel)+1+(EndDateTime-DateTime));

                    if(rc)
                    {
                        char * CurrentPosition;

                        cn36_StrNCpy(StringData, KeyLabel, (EndKeyLabel-KeyLabel));
                        KeyLabel=StringData;

                        CurrentPosition=cn36_TerminatingZeroOf(StringData)+1;
                        cn36_StrNCpy(CurrentPosition, DateTime ,(EndDateTime-DateTime));
                        DateTime=CurrentPosition;
                    }
                }
                //else ignore RESTORE, HISTLOST and others
            }
        }
    }

    return rc;
}

const char * tcn353Backint2DBMKNLLine::GiveKeyLabel() const
{
    return KeyLabel;
}

tcn35d_BackupInfo::ADataType tcn353Backint2DBMKNLLine::GiveType() const
{
    return DataType;
}

const char * tcn353Backint2DBMKNLLine::GiveDateTime() const
{
    return DateTime;
}

// -----------------------------------------------------------------------------
// implementation of class tcn353Backint2EBID
// -----------------------------------------------------------------------------

class tcn353Backint2EBID
{
  public:
    tcn353Backint2EBID();
    ~tcn353Backint2EBID();

    int TakeInfoFromString(int        & ContainsInfo,
                           const char * String,
                           const char   ListDelimiter);

    int TakeInfoFromStringList(int          & ContainsInfo,
                               const char * & CurrentPosition,
                               const char     ListDelimiter);

    const char * GiveUser() const;
    const char * GivePath() const;
    const char * GiveBID() const;

  private:
    char * StringData;    // just for easier handling of dynamically allocated memory

    const char * User;
    const char * BID;
    const char * Path;
};

tcn353Backint2EBID::tcn353Backint2EBID()
    :StringData(0), User(0), BID(0), Path(0)
{
}

tcn353Backint2EBID::~tcn353Backint2EBID()
{
    cn36_StrDealloc(StringData);
}

int tcn353Backint2EBID::TakeInfoFromString(int & ContainsInfo, const char * String, const char ListDelimiter/*size_t StringLength*/)
{
    const char * StringList=String;

    return TakeInfoFromStringList(ContainsInfo, StringList, ListDelimiter);
}

int tcn353Backint2EBID::TakeInfoFromStringList(int          & ContainsInfo,
                                               const char * & CurrentPosition,
                                               const char     ListDelimiter)
{
    int rc=1;

    ContainsInfo=0;

    if(0!=CurrentPosition)
    {
        User                  =cn36_FirstNonWhiteSpaceOf(CurrentPosition);  //go to the start of the User
        const char * EndOfUser=cn36_FirstWhiteSpaceOf(User);                //go to end of User

        BID                  =cn36_FirstNonWhiteSpaceOf(EndOfUser);         //go to start of BID
        const char * EndOfBID=cn36_FirstWhiteSpaceOf(BID);                  //go to end of BID

        Path=cn36_FirstNonWhiteSpaceOf(EndOfBID);                           //go to start of Path

        if('\0'!=(*Path) && ListDelimiter!=(*Path))
        {
            ContainsInfo=1;

            const char * EndOfPath=strchr(Path, ListDelimiter);

            if(0==EndOfPath)
                CurrentPosition=EndOfPath=cn36_TerminatingZeroOf(Path);
            else
                CurrentPosition=EndOfPath+1;

            rc=cn36_StrAlloc(StringData, (EndOfUser-User)+1+(EndOfBID-BID)+1+(EndOfPath-Path));

            if(rc)
            {
                char *h, *h2;

                cn36_StrNCpy(StringData, User, (EndOfUser-User));   // copy User to StringData
                h=StringData+(EndOfUser-User)+1;                    // h points to place for BID
                User=StringData;                                    // User points to new copy of user

                cn36_StrNCpy(h, BID, (EndOfBID-BID));               // copy BID to StringData
                h2=h+(EndOfBID-BID)+1;                              // h2 points to place for Path
                BID=h;                                              // BID points to new copy of backup ID

                cn36_StrNCpy(h2, Path, (EndOfPath-Path));           // copy Path to StrinData
                Path=h2;                                            // Path points to new copy of Path
            }
        }
    }

    return rc;
}

const char * tcn353Backint2EBID::GiveUser() const
{
    return User;
}

const char * tcn353Backint2EBID::GiveBID() const
{
    return BID;
}

const char * tcn353Backint2EBID::GivePath() const
{
    return Path;
}


// -----------------------------------------------------------------------------
// implementation of class tcn353Backint2DBMEBFLine
// -----------------------------------------------------------------------------

class tcn353Backint2DBMEBFLine    //TODO something like that should be included in vcn32.cpp
{
  public:
    tcn353Backint2DBMEBFLine();
    ~tcn353Backint2DBMEBFLine();

    int TakeBackint2InfoFromDBMEBFLine(tcn35d_DBBackupControllerErrorHandler * ErrorHandler,
                                       int                                   & ContainsInfo,
                                       int                                   & NeedsDBMKNLLine,
                                       const DBMSrvBHist_Part                * ADBMEBFLine,
                                       const char                            * NameOfDBMEBF);
private:
    int EqualsInKeyAndLabel(const tcn353Backint2DBMKNLLine * DBMKNLInfo);
public:
    const char *                 GiveUser() const;
    const char *                 GivePath() const;
    const char *                 GiveBID() const;
    tcn35d_BackupInfo::ADataType GiveType() const;
    const char *                 GiveDateTime() const;

  private:
    tcn353Backint2EBID             Backint2EBID;
    char                         * KeyLabelDateTime;
    tcn35d_BackupInfo::ADataType   Type;
    char                         * DateTime;

    static const char SectionDelimiter;
};

const char tcn353Backint2DBMEBFLine::SectionDelimiter='|';

tcn353Backint2DBMEBFLine::tcn353Backint2DBMEBFLine()
    :KeyLabelDateTime(0),
    Backint2EBID(),
    Type(tcn35d_BackupInfo::DataRecovery)
{
}

tcn353Backint2DBMEBFLine::~tcn353Backint2DBMEBFLine()
{
    cn36_StrDealloc(KeyLabelDateTime);
}

int tcn353Backint2DBMEBFLine::TakeBackint2InfoFromDBMEBFLine(tcn35d_DBBackupControllerErrorHandler * ErrorHandler,
                                                             int                                   & ContainsInfo,
                                                             int                                   & NeedsDBMKNLLine,
                                                             const DBMSrvBHist_Part                * ADBMEBFLine,
                                                             const char                            * NameOfDBMEBF)
{
    int Rc=1;

    ContainsInfo=0;
    NeedsDBMKNLLine=1;

    if('\0'!=ADBMEBFLine && DBMSrvBTools_ToolEnum::BACK==ADBMEBFLine->GiveUsedBackupTool())
    {
        Rc=Backint2EBID.TakeInfoFromString(ContainsInfo, ADBMEBFLine->GiveEBID(), '\0');

        if(Rc && ContainsInfo)
        {
            const char * SDateTime="0000-00-00 00:00:00";       //if we find a part of dbm.ebf written with previous versions (only 3 columns)
            size_t       SDateTimeLength=strlen(SDateTime);     //let's use some defaults

            const char * SType=ADBMEBFLine->getBackupType();
            size_t       STypeLength=strlen(SType);

            if(ADBMEBFLine->IsNewFormat())
                NeedsDBMKNLLine=0;  //we do not need a corresponding line from dbm.knl at all costs

            if(0==cn36_StrStrNCmp(BA_EBIDS_TYPE_DAT_REC_CN003C, SType, STypeLength))
                Type=tcn35d_BackupInfo::DataRecovery;
            else
                if(0==cn36_StrStrNCmp(BA_EBIDS_TYPE_DAT_MIG_CN003C, SType, STypeLength))
                    Type=tcn35d_BackupInfo::DataMigration;
                else
                    if(0==cn36_StrStrNCmp(BA_EBIDS_TYPE_PAG_REC_CN003C, SType, STypeLength))
                        Type=tcn35d_BackupInfo::PagesRecovery;
                    else
                        if(0==cn36_StrStrNCmp(BA_EBIDS_TYPE_PAG_MIG_CN003C, SType, STypeLength))
                            Type=tcn35d_BackupInfo::PagesMigration;
                        else
                            if(0==cn36_StrStrNCmp(BA_EBIDS_TYPE_LOG_REC_CN003C, SType, STypeLength))
                                Type=tcn35d_BackupInfo::Log;
                            else
                            {
                                ContainsInfo=0;
                                Rc=0;
                                ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseUnknownFormatOfDBMEbfFile, NameOfDBMEBF));
                            }

            if(Rc)
            {
                SDateTime      =ADBMEBFLine->getBackupDateTime();
                SDateTimeLength=strlen(SDateTime);
            }

            if(ContainsInfo)
            {
                Rc=cn36_StrAlloc(KeyLabelDateTime, strlen(ADBMEBFLine->GiveKey())+1+strlen(ADBMEBFLine->GiveLabel())+1+SDateTimeLength);

                if(Rc)
                {
                    sprintf(KeyLabelDateTime, "%s|%s", ADBMEBFLine->GiveKey(), ADBMEBFLine->GiveLabel());

                    DateTime=KeyLabelDateTime+strlen(KeyLabelDateTime)+1;    //Type+STypeLength+1;
                    strncpy(DateTime, SDateTime, SDateTimeLength);
                    DateTime[SDateTimeLength]='\0';
                }
            }
        }
    }

    return Rc;
}

int tcn353Backint2DBMEBFLine::EqualsInKeyAndLabel(const tcn353Backint2DBMKNLLine * DBMKNLInfo)
{
    int rc=0;

    if(0==strcmp(KeyLabelDateTime, DBMKNLInfo->GiveKeyLabel())) //Keylabel is separated from DateTime by a '\0'
        rc=1;

    return rc;
}

const char * tcn353Backint2DBMEBFLine::GiveUser() const
{
    return Backint2EBID.GiveUser();
}

const char * tcn353Backint2DBMEBFLine::GiveBID() const
{
    return Backint2EBID.GiveBID();
}

const char * tcn353Backint2DBMEBFLine::GivePath() const
{
    return Backint2EBID.GivePath();
}

tcn35d_BackupInfo::ADataType tcn353Backint2DBMEBFLine::GiveType() const
{
    return Type;
}

const char * tcn353Backint2DBMEBFLine::GiveDateTime() const
{
    return DateTime;
}


// -----------------------------------------------------------------------------
// implementation of class tcn353_Backint2Connector
// -----------------------------------------------------------------------------

tcn353_Backint2Connector::tcn353_Backint2Connector(tcn35d_BackupInfo                     * TheBackupInfo,
                                                   tcn35d_DBBackupControllerLogWriter    * TheLogWriterToUse,
                                                   tcn35d_DBBackupControllerErrorHandler * TheErrorHandlerToUse,
                                                   tcn35d_ConfigurationVariableHandler   * TheConfigurationVariableHandler,
                                                   char                                  * TheExternalBackupIDList)
    :tcn35_BackupToolConnector(TheBackupInfo, TheLogWriterToUse, TheErrorHandlerToUse, TheConfigurationVariableHandler, TheExternalBackupIDList),
    NumberOfBackupHistories(NumberOfBackupHistories_cn353)
{
    const char * BackintEnvFileDefaultName  ="bsi.env";
    const char * BackintInFileDefaultName   ="bsi.in";
    const char * BackintOutFileDefaultName  ="bsi.out";
    const char * BackintErrorFileDefaultName="bsi.err";

    cn36_ConstructCompletePath(PathToEnvFile=0, BackupInfo->DefaultDirectory(), BackintEnvFileDefaultName);

    BackintPath=0;
    cn36_ConstructCompletePath(BackintInFile=0, BackupInfo->DefaultDirectory(), BackintInFileDefaultName);
    cn36_ConstructCompletePath(BackintOutFile=0, BackupInfo->DefaultDirectory(), BackintOutFileDefaultName);
    cn36_ConstructCompletePath(BackintErrorFile=0, BackupInfo->DefaultDirectory(), BackintErrorFileDefaultName);
    BackintParamFile=0;

    int i;

    NumberOfRequestedNames=0;

    BackintDataType=DataPipes;

    SuffixBackupHistory[0]=".knl";
    SuffixBackupHistory[1]=".ebf";
    SuffixOfCopyOfBackupHistory[0]=".knc";
    SuffixOfCopyOfBackupHistory[1]=".ebc";

    for(i=0; i<NumberOfBackupHistories; i++)
    {
        OriginalBackupHistoryMovedTo[i]=0;
        OriginalBackupHistoryName[i]=0;
        PathOfMigrationBackintHistory[i]=0;
        BackupHistoryState[i]=Unknown;
    }

    InFileItem=0;
    TempBID=0;

    SourceDBName=0;

    SecondBackupCall=0;

    checkBackupAvailability=tcn353_Backint2Connector::check;
}

tcn353_Backint2Connector::~tcn353_Backint2Connector()
{
    cn36_StrDealloc(PathToEnvFile);

    cn36_StrDealloc(BackintPath);
    cn36_StrDealloc(BackintInFile);
    cn36_StrDealloc(BackintOutFile);
    cn36_StrDealloc(BackintErrorFile);
    cn36_StrDealloc(BackintParamFile);

    int i;

    DeleteBackintInputList();
    
    for(i=0; i<NumberOfBackupHistories; i++)
    {
        cn36_StrDealloc(OriginalBackupHistoryMovedTo[i]);
        cn36_StrDealloc(OriginalBackupHistoryName[i]);
        cn36_StrDealloc(PathOfMigrationBackintHistory[i]);
    }

    cn36_StrDealloc(TempBID);
    cn36_StrDealloc(SourceDBName);
}

const char * tcn353_Backint2Connector::GiveToolName() const
{
    return "Backint for MaxDB";
}

const char * tcn353_Backint2Connector::GiveLongToolName() const
{
    return "Backint for MaxDB Interface";
}

const DBMSrvBTools_ToolEnum tcn353_Backint2Connector::ToolEnum() const
{
    return DBMSrvBTools_ToolEnum(DBMSrvBTools_ToolEnum::BACK);
}

int tcn353_Backint2Connector::CheckToolInstallation(int & IsInstalled)
{
    int rc=1;

    IsInstalled=1;

    LogWriter->Write("Checking existence and configuration of Backint for MaxDB.\n", tcn35d_DBBackupControllerLogWriter::AddALevelToPrefix);

    rc=DetermineValueOfEnvironmentVariable("BSI_ENV",
                                           PathToEnvFile ,
                                           "path of the configuration file of Backint for MaxDB",
                                           BackupInfo->DBName(),
                                           0);

    if(rc)
        if(!cn36_CheckRead(PathToEnvFile))
        {
            IsInstalled=0;
            ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseAFileIsMissingOrUnreadable, PathToEnvFile));

            LogWriter->Write("Can not read the Backint for MaxDB configuration file.\n");
        }
        else
        {
            unsigned i=0;
            char * OriginalRundirectory=0;
            char * checkAvailabilityAsString=0;
            const unsigned NumberOfEnvKeyWords=9;
            const char * EnvKeyWord[NumberOfEnvKeyWords]={"BACKINT",
                                                          "INPUT",
                                                          "OUTPUT",
                                                          "ERROROUTPUT",
                                                          "PARAMETERFILE",
                                                          "TIMEOUT_SUCCESS",
                                                          "TIMEOUT_FAILURE",
                                                          "ORIGINAL_RUNDIRECTORY",
                                                          "CHECK_AVAILABILITY"};

            char ** StringValueForKeyWord[NumberOfEnvKeyWords]={&BackintPath,
                                                                &BackintInFile,
                                                                &BackintOutFile,
                                                                &BackintErrorFile,
                                                                &BackintParamFile,
                                                                0,
                                                                0,
                                                                &OriginalRundirectory,
                                                                &checkAvailabilityAsString};
            int BackintTimeOutSuccessFromConfFile=-1,
                BackintTimeOutFailureFromConfFile=-1;

            int * IntValueForKeyWord[NumberOfEnvKeyWords]={0,
                                                           0,
                                                           0,
                                                           0,
                                                           0,
                                                           &BackintTimeOutSuccessFromConfFile,
                                                           &BackintTimeOutFailureFromConfFile,
                                                           0,
                                                           0};

            tni34_ATextFile TheEnvFile(PathToEnvFile,
                                       tni34_AFile::FromDisk,
                                       0);

            if(TheEnvFile.IsGood())
            {
                const char * Line=0;
                char * FirstWordInUpperLetters=0;

                sprintf(MsgBuf_cn36, "Reading the Backint for MaxDB configuration file '%s'.\n", PathToEnvFile);
                LogWriter->Write(MsgBuf_cn36, tcn35d_DBBackupControllerLogWriter::AddALevelToPrefix);

                while(rc && TheEnvFile.ReadLine(Line) && 0!=Line)
                {
                    unsigned j;
                    int      FoundAKeyWord=0;

                    const char * FirstWord=cn36_FirstNonWhiteSpaceOf(Line);
                    size_t LengthOfFirstWord=cn36_FirstWhiteSpaceOf(FirstWord)-FirstWord;

                    if(cn36_StrNAllocCpy(FirstWordInUpperLetters, FirstWord, LengthOfFirstWord))
                    {
                        cn36_StrUpr(FirstWordInUpperLetters);
                        
                        for(j=0; j<NumberOfEnvKeyWords; j++)
                            if(!strncmp(FirstWordInUpperLetters, EnvKeyWord[j], strlen(EnvKeyWord[j])))
                            {
                                const char * SecondWord=cn36_FirstNonWhiteSpaceOf(FirstWord+LengthOfFirstWord);
                                const char * EndOfSecondWord;
                                size_t       LengthOfSecondWord;
                                int          AFormatErrorOccured=0;

                                // if the value contains white spaces, it is enclosed in a pair of double quotes
                                cn36_GetStringBetweenDoubleQuotesOrWhiteSpaces(SecondWord, EndOfSecondWord);
                                LengthOfSecondWord=EndOfSecondWord-SecondWord;

                                FoundAKeyWord=1;

                                if(0!=StringValueForKeyWord[j])
                                {
                                    if('\0'==SecondWord[0])
                                        AFormatErrorOccured=1;
                                    else
                                        if(!cn36_StrNAllocCpy(*StringValueForKeyWord[j], SecondWord, LengthOfSecondWord))
                                            rc=0;
                                        else
                                        {
                                            sprintf(MsgBuf_cn36, "Found keyword '%s' with value '%s'.\n", EnvKeyWord[j], *StringValueForKeyWord[j]);
                                            LogWriter->Write(MsgBuf_cn36);
                                        }
                                }

                                if(0!=IntValueForKeyWord[j])
                                {
                                    int NewValue=0;

                                    if(!cn36_StrNToInt(NewValue, SecondWord, LengthOfSecondWord) ||
                                        0>NewValue)
                                    {
                                        AFormatErrorOccured=1;
                                    }
                                    else
                                    {
                                        *(IntValueForKeyWord[j])=NewValue;

                                        sprintf(MsgBuf_cn36, "Found keyword '%s' with value '%d'.\n", EnvKeyWord[j], int(NewValue)); //we use NewValue because int(*(IntValueForKeyWord[j])) is not liked by the dec compiler ?:(?
                                        LogWriter->Write(MsgBuf_cn36);
                                    }
                                }

                                if(AFormatErrorOccured)
                                {
                                    LogWriter->Write("The following line of the Backint for MaxDB configuration file contains a format error and is ignored:\n", tcn35d_DBBackupControllerLogWriter::AddALevelToPrefix);
                                    LogWriter->Write(Line);
                                    LogWriter->Write("\n", tcn35d_DBBackupControllerLogWriter::NoPrefix);
                                    LogWriter->Write(tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefix);
                                }

                                j=NumberOfEnvKeyWords; // we have found a keyword no other will match
                            }

                        if(!FoundAKeyWord && Line[0]!='\0')
                        {
                            LogWriter->Write("The following line of the Backint for MaxDB configuration file does not start with a proper keyword and is ignored:\n", tcn35d_DBBackupControllerLogWriter::AddALevelToPrefix);
                            LogWriter->Write(Line);
                            LogWriter->Write("\n", tcn35d_DBBackupControllerLogWriter::NoPrefix);
                            LogWriter->Write(tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefix);
                        }
                    }
                    else
                        rc=0;
                }

                cn36_StrDealloc(FirstWordInUpperLetters);

                LogWriter->Write("Finished reading of the Backint for MaxDB configuration file.\n", tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefixAndEndParagraph);

                if(rc)
                {
                    const char * BackHist=BackupInfo->PathOfBackupHistory();
                    const char * ExtBackHist=BackupInfo->PathOfExternalBackupHistory();

                    if(0==BackHist || 0==ExtBackHist)
                    {
                        cn36_StrDealloc(PathOfMigrationBackintHistory[0]);
                        cn36_StrDealloc(PathOfMigrationBackintHistory[1]);
                    }
                    else
                    {
                        if(0!=OriginalRundirectory) //if found original rundirectory
                        {
                            if(!cn36_ConstructCompletePath(PathOfMigrationBackintHistory[0], OriginalRundirectory, cn36_BaseNameFrom(BackHist)) ||  //construct the original path
                               !cn36_ConstructCompletePath(PathOfMigrationBackintHistory[1], OriginalRundirectory, cn36_BaseNameFrom(ExtBackHist))) //construct the original path
                            {
                                rc=0;
                            }
                        }
                        else    //no original rundirectory was specified in the configuration file
                        {
                            if(!cn36_StrAllocCpy(PathOfMigrationBackintHistory[0], BackHist) ||
                               !cn36_StrAllocCpy(PathOfMigrationBackintHistory[1], ExtBackHist))
                            {
                                rc=0;
                            }
                        }
                    }
                }

                cn36_StrDealloc(OriginalRundirectory); //we do not need it anymore, if 0==OriginalRundirectory nothing is done

                if(rc)  //1==IsInstalled holds
                {
                    if(0==BackintPath)    
                    {
                        ErrorHandler->Report(tcn353_BackintError(tcn353_BackintError::CaseUnspecifiedBackintPath, PathToEnvFile));
                        IsInstalled=0;
                    }
                    else
                    {
                        if(!cn36_CheckExe(BackintPath))
                        {
                            ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseAFileIsNotExecutable, BackintPath));
                            IsInstalled=0;

                            sprintf(MsgBuf_cn36, "The Backint for MaxDB program '%s' is missing or not executable.\n", BackintPath);
                            LogWriter->Write(MsgBuf_cn36);
                        }
                    }

                    if(cn36_CheckExist(BackintInFile))
                    {
                        ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseAFileAlreadyExists, BackintInFile));
                        IsInstalled=0;

                        sprintf(MsgBuf_cn36, "'%s' already exists and can not be used as input file of Backint for MaxDB.\n", BackintInFile);
                        LogWriter->Write(MsgBuf_cn36);
                    }

                    if(cn36_CheckExist(BackintOutFile))
                    {
                        ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseAFileAlreadyExists, BackintOutFile));
                        IsInstalled=0;
                        
                        sprintf(MsgBuf_cn36, "'%s' already exists and can not be used as output file of Backint for MaxDB.\n", BackintOutFile);
                        LogWriter->Write(MsgBuf_cn36);
                    }

                    if(cn36_CheckExist(BackintErrorFile))
                    {
                        ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseAFileAlreadyExists, BackintErrorFile));
                        IsInstalled=0;
                        
                        sprintf(MsgBuf_cn36, "'%s' already exists and can not be used as error output file of Backint for MaxDB.\n", BackintErrorFile);
                        LogWriter->Write(MsgBuf_cn36);
                    }

                    if(IsInstalled)
                    {
                        sprintf(MsgBuf_cn36, "Using '%s' as %s program.\n", BackintPath, GiveToolName());
                        LogWriter->Write(MsgBuf_cn36);

                        sprintf(MsgBuf_cn36, "Using '%s' as input file for %s.\n", BackintInFile, GiveToolName());
                        LogWriter->Write(MsgBuf_cn36);

                        sprintf(MsgBuf_cn36, "Using '%s' as output file for %s.\n", BackintOutFile, GiveToolName());
                        LogWriter->Write(MsgBuf_cn36);

                        sprintf(MsgBuf_cn36, "Using '%s' as error output file for %s.\n", BackintErrorFile, GiveToolName());
                        LogWriter->Write(MsgBuf_cn36);

	                    if(0==BackintParamFile)
                            sprintf(MsgBuf_cn36, "Using no parameter file for %s.\n", GiveToolName());
	                    else
		                    sprintf(MsgBuf_cn36, "Using '%s' as parameter file for %s.\n", BackintParamFile, GiveToolName());

                        LogWriter->Write(MsgBuf_cn36);

                        if(ToolTimeoutSuccessIsDefault()) //timeouts from dbm.cfg or environment overrule timeouts from the configuration file
                        {
                            if(0<=BackintTimeOutSuccessFromConfFile)
                                SetToolTimeoutSuccess(BackintTimeOutSuccessFromConfFile);
                            else
                                SetToolTimeoutSuccess(300); // on success use a time out of 300 seconds (5 minutes), if nothing else is specified in the environment or the environment file
                        }

                        sprintf(MsgBuf_cn36, "Using '%d' seconds as timeout for %s in the case of success.\n", int(GetToolTimeoutSuccess()), GiveToolName());
                        LogWriter->Write(MsgBuf_cn36);

                        if(ToolTimeoutFailureIsDefault())
                        {
                            if(0<=BackintTimeOutFailureFromConfFile)
                                SetToolTimeoutFailure(BackintTimeOutFailureFromConfFile);
                            else
                                SetToolTimeoutFailure(300); // same default value for error situations as for success situations
                        }

                        sprintf(MsgBuf_cn36, "Using '%d' seconds as timeout for %s in the case of failure.\n", int(GetToolTimeoutFailure()), GiveToolName());
                        LogWriter->Write(MsgBuf_cn36);
                        
                        if(0!=PathOfMigrationBackintHistory[0])
                        {
                            sprintf(MsgBuf_cn36, "Using '%s' as backup history of a database to migrate.\n", PathOfMigrationBackintHistory[0]);
                            LogWriter->Write(MsgBuf_cn36);
                        }

                        if(0!=PathOfMigrationBackintHistory[1])
                        {
                            sprintf(MsgBuf_cn36, "Using '%s' as external backup history of a database to migrate.\n", PathOfMigrationBackintHistory[1]);
                            LogWriter->Write(MsgBuf_cn36);
                        }

                        if(0!=checkAvailabilityAsString && 0==cn36_StrUprCmp(checkAvailabilityAsString, "NO"))
                        {
                            checkBackupAvailability=tcn353_Backint2Connector::dontCheck;

                            LogWriter->Write("Backint's inquire function will never be used, all backups are assumed as available via Backint for SAP DB.\n");
                        }
                        else
                            LogWriter->Write("Checking availability of backups using backint's inquire function.\n");
                    }
                }
            }
            else
                rc=0;
        }

    LogWriter->Write((rc && IsInstalled)?"Check passed successful.\n":"Check failed.\n", tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefixAndEndParagraph);

	return rc;
}

const char * tcn353_Backint2Connector::BI_BACKUPStringForActionString(tcn35d_BackupInfo::ADataType DataType)
{
    const char * rc=0;

    if(tcn35d_BackupInfo::DataRecovery==DataType ||
       tcn35d_BackupInfo::DataMigration==DataType)
    {
        rc="FULL";
    }
    else
        if(tcn35d_BackupInfo::PagesRecovery==DataType ||
           tcn35d_BackupInfo::PagesMigration==DataType)
        {
            rc="PARTIAL";
        }
        else
            if(tcn35d_BackupInfo::Log==DataType)
                rc="ARCHIVE";

    return rc;
}

int tcn353_Backint2Connector::PrepareFirst()
{
    int rc=1,
        NumberOfInFileEntries;

    const char * ValueBI_REQUEST="NEW";

    DeleteBackintInputList();

    if((tcn35d_BackupInfo::ABackup==BackupInfo->DataDirection() && HistoryFiles==BackintDataType) ||
       SecondBackupCall)  //the backup of history files is part of the DATA/PAGES/LOG backup
    {
        ValueBI_REQUEST="OLD";                                      //-> it is an old(second) request during one backup
    }

    if(!PutEnvironmentVariable("BI_CALLER", "DBMSRV") ||
       !PutEnvironmentVariable("BI_REQUEST", ValueBI_REQUEST))
    {
        rc=0;
    }

    if(rc)
        switch(BackupInfo->DataDirection())
        {
            case tcn35d_BackupInfo::ABackup:   // on backup ...
            case tcn35d_BackupInfo::ARestore:  // ... and restore
            {
                if(tcn35d_BackupInfo::ABackup==BackupInfo->DataDirection() ||     //a backup
                   DataPipes==BackintDataType)      //or a restore of data pipes
                {
                    //set the backup type (data, pages, log) in environment variable BI_BACKUP
                    const char * BI_BackupType=BI_BACKUPStringForActionString(BackupInfo->DataType());

                    if(0==BI_BackupType)
                    {
                        rc=0;
                        ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
                    }
                    else
                        if(!PutEnvironmentVariable("BI_BACKUP", BI_BackupType))
                            rc=0;
                }
                else
                    if(tcn35d_BackupInfo::ARestore==BackupInfo->DataDirection() &&
                       HistoryFiles==BackintDataType)   //history files will be restored allone (at backup time they are a part of the DATA/PAGES/LOG backup)
                    {
                        if(!PutEnvironmentVariable("BI_BACKUP", ""))   //try to unset BI_BACKUP
                            rc=0;
                    }
                    else
                    {
                        ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
                        rc=0;
                    }
            }break;

            case tcn35d_BackupInfo::GiveBackupInfo:
            {
                if(!PutEnvironmentVariable("BI_BACKUP", ""))
                    rc=0;
                else
                {
                    switch(BackintDataType)
                    {
                        case DataPipes:
                        {
                            int i, AllBackupHistoriesExist=1;

                            const char * PathDBMKNL=BackupInfo->PathOfBackupHistory();
                            const char * PathDBMEBF=BackupInfo->PathOfExternalBackupHistory();
        
                            if(0==PathDBMKNL || 0==PathDBMEBF)  //be sure that BackupInfo can supply the path's in this case
                            {
                                rc=0;
                                ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
                            }
                            else
                            {
                                rc=cn36_StrAllocCpy(OriginalBackupHistoryName[0], PathDBMKNL);
            
                                if(rc)
                                    rc=cn36_StrAllocCpy(OriginalBackupHistoryName[1], PathDBMEBF);

                                if(rc)
                                {
                                    for(i=0; i<NumberOfBackupHistories; i++)    // check the availability of all history files
                                    {
                                        if(cn36_CheckExist(OriginalBackupHistoryName[i]))
                                            BackupHistoryState[i]=Exist;
                                        else
                                        {
                                            BackupHistoryState[i]=NotExist;
                                            AllBackupHistoriesExist=0;
                                        }
                                    }
                        
                                    if(0!=strcmp(BackupInfo->SourceDBName(), BackupInfo->DBName()) ||  // source db is different from current db
                                       !AllBackupHistoriesExist)                        // the backup history or the external backup history or both do not exist
                                    {
                                        rc=BackupOrRestoreHistoryWithBackint();         // get the needed history files from Backint for MaxDB
                                    }
                                }
                            }
                        }break;

                        case HistoryFiles:
                        break;

                        default:
                        {
                            ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
                            rc=0;
                        }
                    }
                }
            }break;

            default:
            {
                ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
                rc=0;
            }
        }

    if(rc)
        rc=DetermineAndCountInputEntries(NumberOfInFileEntries);

    if(rc)
        rc=BuildBackupToolCall();

    if(rc)
    {
        if(0!=ToolProcessOutput[0])
            delete ToolProcessOutput[0];

        if(0!=ToolProcessError[0])
            delete ToolProcessError[0];

        ToolProcessOutput[0]=new tni34_AFile(BackintOutFile, tni34_AFile::ToDiskAppend, 0, 0); // do not create on open and do not open it now
        ToolProcessError[0]=new tni34_AFile(BackintErrorFile, tni34_AFile::ToDiskAppend, 0, 0); // do not create on open and do not open it now

        if(0==ToolProcessOutput[0] || 0==ToolProcessError[0])
        {
            rc=0;
            ErrorHandler->Report(tcn35d_DBBackupError::OutOfMemory.Set());
        }
        else
        {
            if(ToolProcessOutput[0]->IsGood() && ToolProcessOutput[0]->Create())
            {
                sprintf(MsgBuf_cn36, "Created temporary file '%s' as output for Backint for MaxDB.\n", ToolProcessOutput[0]->GiveName());
                LogWriter->Write(MsgBuf_cn36);
            }
            else
                rc=0;

            if(ToolProcessError[0]->IsGood() && ToolProcessError[0]->Create())
            {
                sprintf(MsgBuf_cn36, "Created temporary file '%s' as error output for Backint for MaxDB.\n", ToolProcessError[0]->GiveName());
                LogWriter->Write(MsgBuf_cn36);
            }
            else
                rc=0;
        }
    }

    if(rc)
    {
        tni34_ATextFile BackintIn(BackintInFile, tni34_AFile::ToDiskTruncate, 1, 0);

        if(BackintIn.IsGood() && BackintIn.Open())
        {
            char * Line=0;

            InFileItem=ListOfFilesProcessedByBackint.First();

            while(rc && 0!=InFileItem)
            {       
                switch(BackupInfo->DataDirection())
                {
                    case tcn35d_BackupInfo::ABackup:
                        rc=InFileItem->GiveInfo()->GenerateInputLine(Line, tcn353_Backint2InputLine::Backup);
                        break;

                    case tcn35d_BackupInfo::ARestore:
                        rc=InFileItem->GiveInfo()->GenerateInputLine(Line, tcn353_Backint2InputLine::Restore);
                        break;

                    case tcn35d_BackupInfo::GiveBackupInfo:
                    {
                        switch(BackintDataType)
                        {
                            case DataPipes:
                                rc=InFileItem->GiveInfo()->GenerateInputLine(Line, tcn353_Backint2InputLine::Inquire);
                            break;

                            case HistoryFiles:
                                rc=InFileItem->GiveInfo()->GenerateInputLine(Line, tcn353_Backint2InputLine::Restore);
                            break;
                            
                            default:
                                ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
                                rc=0;
                        }
                    }
                    break;

                    default:
                        ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
                        rc=0;
                }

                if(rc)
                {
                    sprintf(MsgBuf_cn36, "Writing '%s' to the input file.\n", Line);
                    LogWriter->Write(MsgBuf_cn36);

                    if(!BackintIn.WriteLine(Line))
                    {
                        rc=0;
                        LogWriter->Write("Could not write the line to the input file '%s'.\n");
                    
                    }
                }

                InFileItem=InFileItem->Next();
            }
                    
            if(rc)
                NumberOfRequestedNames=NumberOfInFileEntries;
            else
                NumberOfRequestedNames=0;
            
            cn36_StrDealloc(Line);  // last chance to free the memory

            if(!BackintIn.Close())
            {
                rc=0;
                sprintf(MsgBuf_cn36, "Could not close the input file of Backint for MaxDB (%s).\n", BackintIn.GiveName());
                LogWriter->Write(MsgBuf_cn36);
            }
        }
    }

    return rc;
}

int tcn353_Backint2Connector::PrepareAgain()
{
    NumberOfRunningToolProcesses=0;
    SecondBackupCall=1;

    return PrepareFirst();
}

int tcn353_Backint2Connector::DetermineAndCountInputEntries(int & NumberOfEntries)
{
    int rc=1,
        i;

    switch(BackupInfo->DataDirection())
    {
        case tcn35d_BackupInfo::ABackup:
        {
            switch(BackintDataType)
            {
                case DataPipes:
                {
                    NumberOfEntries=BackupInfo->NumberOfPipes(); // take all pipes of the medium

                    for(i=NumberOfEntries-1; 0<=i; i--) //count down to get them finaly in list in the same order as in the medium
                        rc=AppendToBackintInputList(BackupInfo->NameOfPipe(i), 1);  // the i-th pipe path, 1==it is a pipe
                }
                break;

                case HistoryFiles:
                {
                    const char * PathDBMKNL=BackupInfo->PathOfBackupHistory();
                    const char * PathDBMEBF=BackupInfo->PathOfExternalBackupHistory();

                    if(2!=NumberOfBackupHistories ||    // assure, that 2==NumberOfBackupHistories
                       0==PathDBMKNL || 0==PathDBMEBF)  // and that the names of the history files are known in this case (backup of history files after backup of data pipes)
                    {
                        rc=0;
                        ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
                    }
                    else
                    {
                        NumberOfEntries=NumberOfBackupHistories;

                        rc=AppendToBackintInputList(PathDBMKNL, 0);    // the backup history file, 0==it is no pipe

                        if(rc)
                            rc=AppendToBackintInputList(PathDBMEBF, 0); // the external backup history file, 0==it is no pipe
                    }
                }
                break;

                default:
                    ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
                    rc=0;
            }
        }break;

        case tcn35d_BackupInfo::ARestore:
        {
            tcn353Backint2EBID Backint2EBID;
            const char *CurrentPositionInEBIDList=ExternalBackupIDList;
            int ContainsABackint2EBID;

            NumberOfEntries=0;

            LogWriter->Write("Got the following list of external backup ID's:\n", tcn35d_DBBackupControllerLogWriter::AddALevelToPrefix);
            LogWriter->Write("\"");
            LogWriter->Write(ExternalBackupIDList, tcn35d_DBBackupControllerLogWriter::NoPrefix);  //don't sprintf it to MsgBuf_cn36 as usual, can be some 1000 characters (in worst case)
            LogWriter->Write("\"\n", tcn35d_DBBackupControllerLogWriter::NoPrefix);
            LogWriter->Write(tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefixAndEndParagraph);

            do
            {
                rc=Backint2EBID.TakeInfoFromStringList(ContainsABackint2EBID,
                                                       CurrentPositionInEBIDList,
                                                       ',');

                if(rc)
                {
                    if(ContainsABackint2EBID)
                    {
                        if(0==NumberOfEntries)
                            rc=cn36_StrAllocCpy(SourceDBName, Backint2EBID.GiveUser());
                        else
                            if(0!=strcmp(SourceDBName, Backint2EBID.GiveUser()))
                            {
                                rc=0;
                                ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseEBIDsOfDifferentDBs, SourceDBName));    //all EBID's must contain the same user (=dbname)
                            }

                        if(rc)
                        {
                            if(NumberOfEntries>=BackupInfo->NumberOfPipes())
                            {
                                rc=0;
                                ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseTooManyEBIDs, BackupInfo->NumberOfPipes())); //the list contains more EBID's than the medium pipes
                            }
                            else
                            {
                                if(0==strcmp(BackupInfo->NameOfPipe(NumberOfEntries), Backint2EBID.GivePath()))
                                {
                                    rc=AppendToBackintInputList(Backint2EBID.GivePath(),
                                                                1,
                                                                Backint2EBID.GiveBID());
                                }
                                else
                                {
                                    rc=AppendToBackintInputList(Backint2EBID.GivePath(),
                                                                1,
                                                                Backint2EBID.GiveBID(),
                                                                BackupInfo->NameOfPipe(NumberOfEntries),
                                                                cn36_TerminatingZeroOf(BackupInfo->NameOfPipe(NumberOfEntries)));
                                }

                                NumberOfEntries++;
                            }
                        }
                    }
                    else
                    {
                        rc=0;
                        ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseSyntaxErrorEBIDs, NumberOfEntries+1)); //the current EBID is no EBID
                    }
                }
            }
            while(rc && '\0'!=(*CurrentPositionInEBIDList));

            ListOfFilesProcessedByBackint.Reverse();

            if(rc && NumberOfEntries<BackupInfo->NumberOfPipes())
            {
                ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseTooFewEBIDs, BackupInfo->NumberOfPipes())); //the list contains fewer EBID's than the medium pipes
                rc=0;
            }

        }break;

        case tcn35d_BackupInfo::GiveBackupInfo:
        {
            if(rc)
            {
                switch(BackintDataType)
                {
                    case DataPipes:
                    {
                        NumberOfEntries=0;

                        DBMSrvBHist_EBIDHistoryFile ExternalHistory;
                        DBMSrv_Reply                Reply;

                        Reply.setToGlobalDummy();

                        if(OK_CN00!=ExternalHistory.readBasicFile(OriginalBackupHistoryName[1], Reply))
                            rc=0;

                        if(rc)
                        {
                            tcn353Backint2DBMEBFLine LineInfo;
                            int                      LineContainesInfo;
                            int                      LineNeedsDBMKNLInfo;

                            for(Tools_List<DBMSrvBHist_Part *>::iterator EBID=ExternalHistory.getEBIDList().begin(); EBID!=ExternalHistory.getEBIDList().end(); ++EBID)
                            {
                                rc=LineInfo.TakeBackint2InfoFromDBMEBFLine(ErrorHandler,
                                                                            LineContainesInfo,
                                                                            LineNeedsDBMKNLInfo,
                                                                            (*EBID),
                                                                            OriginalBackupHistoryName[1]);

                                if(rc && LineContainesInfo && 0==strcmp(BackupInfo->SourceDBName(), LineInfo.GiveUser()))    // inquire only backups made by the db named by BackupInfo->SourceDBName()
                                {
                                    if(!LineNeedsDBMKNLInfo) //if we did not found a corresponding line in dbm.knl but we also do not not need one
                                    {
                                        rc=AppendToBackintInputList(LineInfo.GivePath(),
                                                                    1,
                                                                    LineInfo.GiveBID(),
                                                                    0,
                                                                    0,
                                                                    LineInfo.GiveType(),
                                                                    LineInfo.GiveDateTime());

                                        if(rc)
                                            NumberOfEntries++;
                                    }
                                }
                            }
                        }
                        else
                        {
                            if('\0'==Reply.giveData()[0])
                                ErrorHandler->Report(tcn353_BackintError(tcn353_BackintError::CaseCouldNotReadExternalBackupHistory));
                            else
                                ErrorHandler->Report(tcn353_BackintError(tcn353_BackintError::CaseCouldNotReadExternalBackupHistory, Reply.giveData()));
                        }

                        if(!BringHistoryFilesToFormerState())
                            rc=0;
                    }break;

                    case HistoryFiles:
                    {
                        if(0!=strcmp(BackupInfo->SourceDBName(), BackupInfo->DBName()))    // we are looking for backups of another database
                        {
                            NumberOfEntries=NumberOfBackupHistories;

                            for(i=0; rc && i<NumberOfBackupHistories; i++)  // all needed histories of the other database must be restored
                            {
                                if(0==PathOfMigrationBackintHistory[i])
                                {
                                    rc=0;
                                    ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError)); //0==PathOfMigrationBackintHistory[i] is possible, but not in this case
                                }
                                else
                                {
                                    if(cn36_CheckExist(OriginalBackupHistoryName[i]))   // if the history already exists...
                                    {
                                        BackupHistoryState[i]=Exist;

                                        if(RenameABackupHistoryFile(OriginalBackupHistoryName[i], i))   // ...try to rename it
                                            BackupHistoryState[i]=Renamed;
                                        else
                                            rc=0;
                                    }
                                    else
                                        BackupHistoryState[i]=Recovered; //at the moment it is not existing (=NotExist; would be better), but here we set the state we desire (we would like to recover the file)!

                                    if(rc)  // if existed or not, restore the corresponding history of the other database
                                        rc=AppendToBackintInputList(PathOfMigrationBackintHistory[i],   // the name of the i-th backup history file
                                                                    0,                                  // it is no pipe
                                                                    0,                                  // no specific BID (-> #NULL is used)
                                                                    OriginalBackupHistoryName[i],       // we must use the destination feature
                                                                    cn36_EndOfDirectory(OriginalBackupHistoryName[i]));
                                }
                            }  
                        }
                        else    //we are looking for backups of the same database
                        {
                            NumberOfEntries=0;

                            for(i=0; rc && i<NumberOfBackupHistories; i++)
                                if(NotExist==BackupHistoryState[i]) //==!cn36_CheckExist(OriginalBackupHistoryName[i])
                                {
                                    BackupHistoryState[i]=Recovered;
                                    rc=AppendToBackintInputList(OriginalBackupHistoryName[i], 0);  // the i-th backup history file, 0 == it is no pipe
                                    NumberOfEntries++;
                                }
                        }
                    }break;

                    default:
                        ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
                        rc=0;
                }
            }
        }break;

        default:
            ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
            rc=0;
        break;
    }

    return rc;
}

int tcn353_Backint2Connector::BringHistoryFilesToFormerState()
{
    int rc=1,
        i;

    if(tcn35d_BackupInfo::GiveBackupInfo==BackupInfo->DataDirection())
    {
        // try to bring the backup history files in the same state as found at the beginning of the inquire
        for(i=0; i<NumberOfBackupHistories; i++)
        {
            switch(BackupHistoryState[i])
            {
                case Exist:     //it has existed and was not touched, or was touched and allready brought to former state
                case NotExist:  //it has not existed and was not touched, or was restored and allready deleted again
                break;

                case Recovered: //it has not existed before and was restored -> remove it
                    if(cn36_CheckExist(OriginalBackupHistoryName[i]))
                    {
                        sprintf(MsgBuf_cn36, "Removing recovered backup history file '%s'.\n", OriginalBackupHistoryName[i]);
                        LogWriter->Write(MsgBuf_cn36);

                        if(!Remove(OriginalBackupHistoryName[i]))
                            rc=0;
                        else
                            BackupHistoryState[i]=NotExist;
                    }
                break;

                case Renamed:   //it existed, was moved to another location and a history was restored ...
                    if(cn36_CheckExist(OriginalBackupHistoryName[i]))
                    {
                        sprintf(MsgBuf_cn36, "Removing recovered backup history file '%s'.\n", OriginalBackupHistoryName[i]);
                        LogWriter->Write(MsgBuf_cn36);

                        if(!Remove(OriginalBackupHistoryName[i]))  // remove the restored history ...
                            rc=0;
                    }
                
                    if(rc)
                    {
                        sprintf(MsgBuf_cn36, "Moving renamed backup history file from '%s' back to '%s'.\n", OriginalBackupHistoryMovedTo[i], OriginalBackupHistoryName[i]);
                        LogWriter->Write(MsgBuf_cn36);

                        if(!cn36_MoveFile(OriginalBackupHistoryMovedTo[i], OriginalBackupHistoryName[i]))   // ... and move the original history back 
                        {
                            BackupHistoryState[i]=Moved;
                            rc=0;
                        }
                        else
                            BackupHistoryState[i]=Exist;
                    }
                break;

                case Moved:
                    sprintf(MsgBuf_cn36, "Moving renamed backup history file from '%s' back to '%s'.\n", OriginalBackupHistoryMovedTo[i], OriginalBackupHistoryName[i]);
                    LogWriter->Write(MsgBuf_cn36);

                    if(!cn36_MoveFile(OriginalBackupHistoryMovedTo[i], OriginalBackupHistoryName[i]))   // move the original history back 
                        rc=0;
                    else
                        BackupHistoryState[i]=Exist;

                case Unknown:
                default:
                    ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
                    rc=0;
            }
        }
    }

    return rc;
}

int tcn353_Backint2Connector::RenameABackupHistoryFile(const char * NameOfBackupHistoryFile,
                                                       int          NumberOfBackupHistoryFile)
{
    int rc=0;

    char * & DestName=OriginalBackupHistoryMovedTo[NumberOfBackupHistoryFile];    //just a shortcut
    
    if(cn36_StrAllocCpy(DestName, NameOfBackupHistoryFile)) //make a copy of the name
    {
        char * LastDot=strrchr(DestName, '.');  //search the last '.' in the name

        if(0!=LastDot && 0==strcmp(LastDot, SuffixBackupHistory[NumberOfBackupHistoryFile])) //the name must end with the expected suffix
        {
            SAPDB_strcpy(LastDot, SuffixOfCopyOfBackupHistory[NumberOfBackupHistoryFile]);    //change the suffix
                
            if(cn36_CheckExist(DestName))   //the destination file must not exist
                ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseAFileAlreadyExists, DestName));
            else
                if(cn36_MoveFile(NameOfBackupHistoryFile, DestName))
                    rc=1;
        }
        else
            ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError)); // we should always know the correct suffix
    }

    return rc;
}

int tcn353_Backint2Connector::AppendToBackintInputList(const char * TheName,
                                                       int          IsAPipe,
                                                       const char * BID,
                                                       const char * Destination,
                                                       const char * EndOfDestination,
                                                       tcn35d_BackupInfo::ADataType DataType,
                                                       const char * DateTime)
{
    int rc=0;

    tcn353_Backint2InputLine * FileItem=new tcn353_Backint2InputLine(ErrorHandler, LogWriter);

    if(0==FileItem)
        ErrorHandler->Report(tcn35d_DBBackupError::OutOfMemory.Set());
    else
    {
        rc=FileItem->SetValues(TheName, IsAPipe, BID, Destination, EndOfDestination, DataType, DateTime);
                            
        if(rc && 0==ListOfFilesProcessedByBackint.Add(FileItem))    // try to add FileItem to the list
        {
            ErrorHandler->Report(tcn35d_DBBackupError::OutOfMemory.Set());
            rc=0;
        }
        //else nothing to do, because rc!=0 holds already
    }

    return rc;
}

void tcn353_Backint2Connector::DeleteBackintInputList()
{
    ni31_ListElem<tcn353_Backint2InputLine *> * FileItem=ListOfFilesProcessedByBackint.First();

    while(0!=FileItem)  // while(we have a list element)
    {
        delete FileItem->GiveInfo();            //delete the tcn353_Backint2InputLine (were the list element's info part is pointing to)
        ListOfFilesProcessedByBackint.Del();    //delete the list element itself
        FileItem=ListOfFilesProcessedByBackint.First(); //get a pointer to the next list element
    }
}

int tcn353_Backint2Connector::BuildBackupToolCall()
{
    int rc=1;
    
    const char * BackintAction=0;
    const char * BackintUser=0;

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

    switch(BackupInfo->DataDirection()) //determine Backint-action
    {
        case tcn35d_BackupInfo::ABackup:
            BackintAction="backup";     // backup data
            BackintUser=BackupInfo->DBName();
        break;
        case tcn35d_BackupInfo::ARestore:
            BackintAction="restore";    // restore a backup
            BackintUser=SourceDBName;   //do not use BackupInfo->SourceDBName(); to allow migration between different databases
        break;
        case tcn35d_BackupInfo::GiveBackupInfo:
        {
            switch(BackintDataType)
            {
                case DataPipes:
                    BackintAction="inquire";    // inquire info about former backups of data
                break;

                case HistoryFiles:
                    BackintAction="restore";    // restore the last version of history files (#NULL <history file name>)
                break;

                default:
                {
                    ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
                    rc=0;
                }
            }
            
            BackintUser=BackupInfo->SourceDBName();
        }
        break;
        default:
            ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
            rc=0;
    }   

    if(rc)
    {
        if(BackupInfo->DataDirection()==tcn35d_BackupInfo::GiveBackupInfo &&
           BackintDataType==DataPipes &&
           tcn353_Backint2Connector::dontCheck==checkBackupAvailability)
        {
            LogWriter->Write("Constructed no Backint for SAP DB call, as option CHECK_AVAILABILITY was set to NO. The following steps are executed only pro forma.\n");
        }
        else
        {
            size_t Length=strlen(BackintPath)           // ...backint(.exe)
                        +4+strlen(BackintUser)        // " -u <DBName>"
                        +4+strlen(BackintAction)      // " -f <action>"
                        +8;                           // " -t file"

            if(0!=BackintParamFile && '\0'!=BackintParamFile[0])
                Length+=4+strlen(BackintParamFile);     // " -p <paramfile>

            Length+=4+strlen(BackintInFile);            // " -i <infile>"
            Length+=3;                                  // " -c"

            rc=cn36_StrAlloc(CommandLine[0], Length);

            if(rc)
            {
                sprintf(CommandLine[0], "%s -u %s -f %s -t file", BackintPath, BackintUser, BackintAction);

                if(0!=BackintParamFile && '\0'!=BackintParamFile[0])
                {
                    strcat(CommandLine[0], " -p ");
                    strcat(CommandLine[0], BackintParamFile);
                }

                strcat(CommandLine[0], " -i ");
                strcat(CommandLine[0], BackintInFile);
            
                strcat(CommandLine[0], " -c");

                if(Length!=strlen(CommandLine[0]))
                    ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
                else
                {
                    NumberOfCommandLines=1;

                    sprintf(MsgBuf_cn36, "Constructed Backint for MaxDB call '%s'.\n", CommandLine[0]);
                    LogWriter->Write(MsgBuf_cn36);
                }
            }
        }
    }

    return rc;
}

int tcn353_Backint2Connector::PostOperationsAfterSuccess()
{
    int rc;

    switch(BackupInfo->DataDirection())
    {
        case tcn35d_BackupInfo::ABackup:
        {
            if(0!=BackupInfo->PathOfBackupHistory() && 0!=BackupInfo->PathOfExternalBackupHistory())
                rc=BackupOrRestoreHistoryWithBackint();  //save the Backup History and the External Backup History File after every successful backup
        }
        break;

        case tcn35d_BackupInfo::GiveBackupInfo:
        {
            int NumberOfEBIDsFound;

            rc=CollectExternalBackupIDs(0, NumberOfEBIDsFound); // do not check NumberOfEBIDsFound against NumberOfRequestedNames, as this is not useful on inquire
        }
        break;

        default:
            rc=1;   // do nothing in all other cases
    }

    return rc;
}

int tcn353_Backint2Connector::CleanUpTool()
{
    int rc=1;
    
    tni34_AFile BackintIn(BackintInFile, tni34_AFile::FromDisk, 0, 0); // do not open, do not create now

    if(BackintIn.Exists())
    {
        if(BackintIn.Remove())
        {
            sprintf(MsgBuf_cn36, "Removed the %s input file '%s'.\n", GiveToolName(), BackintIn.GiveName());
            LogWriter->Write(MsgBuf_cn36);
        }
        else
        {
            rc=0;

            sprintf(MsgBuf_cn36, "Could not remove the %s input file '%s'.\n", GiveToolName(), BackintIn.GiveName());
            LogWriter->Write(MsgBuf_cn36);
        }
    }

    DeleteBackintInputList();   // free the memory used by the list

    if(!BringHistoryFilesToFormerState())
        rc=0;

    return rc;
}

int tcn353_Backint2Connector::CollectExternalBackupIDs(char * ExternalBackupID[MAX_NUMBER_OF_TOOLPROCESSES_CN35], int & NumberOfEBIDsFound)
{
    int rc=1;

    NumberOfEBIDsFound=0; //we have not found any ID so far

    tni34_ATextFile BackintOut(BackintOutFile, tni34_AFile::FromDisk, 0, 0);    // do not open, do not create now

    if(BackintOut.IsGood())
    {
        if(BackintOut.Open())
        {
            const char * Line,
                       * CurrentBID;
            const char * Name;
            size_t       CurrentBIDLength,
                         NameLength;

            int LineContainsABID;

            while(rc && BackintOut.ReadLine(Line) && 0!=Line)
            {
                rc=GetBIDAndNameFromLine(Line, LineContainsABID, CurrentBID, CurrentBIDLength, Name, NameLength);

                if(rc && LineContainsABID)
                {
                    int FoundForTheFirstTime,
                        InputLineFound=0;

                    tcn353_Backint2InputLine::BackintActionType Action;
                    
                    switch(BackupInfo->DataDirection())
                    {
                        case tcn35d_BackupInfo::ABackup:
                            Action=tcn353_Backint2InputLine::Backup;
                        break;

                        case tcn35d_BackupInfo::ARestore:
                            Action=tcn353_Backint2InputLine::Restore;
                        break;

                        case tcn35d_BackupInfo::GiveBackupInfo:
                            Action=tcn353_Backint2InputLine::Inquire;
                        break;

                        default:
                            ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
                            rc=0;
                    }

                    InFileItem=ListOfFilesProcessedByBackint.First();

                    while(rc && !InputLineFound && 0!=InFileItem)
                    {
                        if(rc)
                            rc=InFileItem->GiveInfo()->TestAgainstOutputLine(Action,
                                                                             InputLineFound,
                                                                             FoundForTheFirstTime,
                                                                             Name,
                                                                             NameLength,
                                                                             CurrentBID,
                                                                             CurrentBIDLength);

                        if(rc && FoundForTheFirstTime)
                        {
                            if(tcn35d_BackupInfo::ABackup==BackupInfo->DataDirection() && 0!=ExternalBackupID)   // if we make a backup but no backup of history files
                            {
                                rc=InFileItem->GiveInfo()->SetCompleteExternalBID(BackupInfo->DBName());    // Make a copy of the complete EBID found. It will be written to the External Backup History.

                                if(rc)
                                {
                                    sprintf(MsgBuf_cn36, "Found External Backup ID '%s'.\n", InFileItem->GiveInfo()->GiveExternalBackupID());
                                    LogWriter->Write(MsgBuf_cn36);
                                }
                            }
                    
                            NumberOfEBIDsFound++; // count the Number of returned BackupIDs regardless of the performed action
                        }

                        InFileItem=InFileItem->Next();
                    }

                    if(rc && !InputLineFound)   // no appropriate line was found
					{
                        rc=0;
                        ErrorHandler->Report(tcn353_BackintError(tcn353_BackintError::CaseWrongFilesProcessed, Name));
					}
                }
            }

            if(tcn35d_BackupInfo::GiveBackupInfo!=BackupInfo->DataDirection())   // if we have made no inquire with backint
            {
                if(rc && NumberOfEBIDsFound<NumberOfRequestedNames) // not all expected names were found in the output
                {                                                   // do not set rc=0, caller decides if this is an error
                    if(0<NumberOfEBIDsFound)
                        LogWriter->Write("Not all files were processed correctly by Backint for MaxDB.\n");
                    else
                        LogWriter->Write("No file was processed correctly by Backint for MaxDB.\n");    
                }
            }
            //else on inquire it is normal, that not all requested items are also mentioned in the output

            if(rc && tcn35d_BackupInfo::ABackup==BackupInfo->DataDirection() && 0!=ExternalBackupID) // if all is ok and we make a backup but no backup of history files
            {
                InFileItem=ListOfFilesProcessedByBackint.First();
                int i=0;

                for(InFileItem=ListOfFilesProcessedByBackint.First(); rc && 0!=InFileItem; InFileItem=InFileItem->Next(), i++)   //in the order of the medium pipes...
                    if(0!=InFileItem->GiveInfo()->GiveExternalBackupID())
                        rc=cn36_StrAllocCpy(ExternalBackupID[i], InFileItem->GiveInfo()->GiveExternalBackupID());                    //... copy the found BID. It will be written to the External Backup History
            }
        }
        else
            rc=0;
    }
    else
        rc=0;

    return rc;
}

int tcn353_Backint2Connector::BuildAEBIDFrom(char * &     EBID,
                                             const char * User,
                                             const char * BID,
                                             const char * Path)
{
    int rc;

    rc=cn36_StrAlloc(EBID, strlen(User)+1+strlen(BID)+1+strlen(Path));

    if(rc)
        sprintf(EBID, "%s %s %s", User, BID, Path);

    return rc;  
}

int tcn353_Backint2Connector::GetBIDAndNameFromLine(const char *   Line,
                                                    int          & ContainsABID,
                                                    const char * & BackupID,
                                                    size_t       & LengthOfBackupID,
                                                    const char * & Name,
                                                    size_t       & LengthOfName)
{
    int rc=1;
    const char * KeyWord;
    
    ContainsABID=0;

    switch(BackupInfo->DataDirection()) //determine Backint-action
    {
        case tcn35d_BackupInfo::ABackup:
            KeyWord="#SAVED";
            break;
        case tcn35d_BackupInfo::ARestore:
            KeyWord="#RESTORED";
            break;
        case tcn35d_BackupInfo::GiveBackupInfo:
            switch(BackintDataType)
            {
                case DataPipes:
                    KeyWord="#BACKUP";  // inquired info about former backups of data
                break;

                case HistoryFiles:
                    KeyWord="#RESTORED";    // restored the last version of history files (#NULL <history file name>)
                break;

                default:
                {
                    ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
                    rc=0;
                }
            }
            break;
        default:
            ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
            rc=0;
    }

    if(rc)
    {
        if(strlen(Line)>strlen(KeyWord) &&              // Is Line longer then the keyword?
           !strncmp(Line, KeyWord, strlen(KeyWord)) &&  // Starts Line with the keyword?
           isspace(Line[strlen(KeyWord)]))              // is a white space following the keyword?
        {
            const char * EndBackupID,
                       * EndName;

            BackupID   =cn36_FirstNonWhiteSpaceOf(Line+strlen(KeyWord));
            EndBackupID=cn36_FirstWhiteSpaceOf(BackupID);
            
            Name=cn36_FirstNonWhiteSpaceOf(EndBackupID);
            EndName=cn36_FirstWhiteSpaceOf(Name);

            rc=0;

            if('\0'!=Name[0]) // if this is true, '\0'!=BackupID[0] is also true
            {
                LengthOfBackupID=EndBackupID-BackupID;
                LengthOfName=EndName-Name;

                if(MAX_LENGTH_BID>=LengthOfBackupID && 0<LengthOfBackupID && 
                   MAX_LENGTH_BSIPATH>=LengthOfName && 0<LengthOfName)
                {
                    rc=1;
                    ContainsABID=1;
                }
            }
            
            if(!rc)
                ErrorHandler->Report(tcn353_BackintError(tcn353_BackintError::CaseUnknownBackintOutputFormat, Line));
        }
    }

    return rc;
}

int tcn353_Backint2Connector::PrepareToolOutputOfAvailableBackups()
{
    ListOfFilesProcessedByBackint.Reverse();            //Reverse the list to get the Backups in the right order (older backups first, newest backup at last)
    InFileItem=ListOfFilesProcessedByBackint.First();

    return 1;
}

int tcn353_Backint2Connector::GiveNextAvailableBackup(int          & EBIDIsAvailable,
                                                      const char * & EBID,
                                                      size_t       & EBIDLength,
                                                      tcn35d_BackupInfo::ADataType    & DataType,
                                                      const char * & DateTimeString,
                                                      int          & ReachedEnd)
{
    int rc=1;

    if(0==InFileItem)
        ReachedEnd=1;
    else
    {
        tcn353_Backint2InputLine * TheLine=InFileItem->GiveInfo();
        
        ReachedEnd=0;

        if(TheLine->WasFound() ||  // did Backint for MaxDB return the BID and Name of this item in its inquire output
           tcn353_Backint2Connector::dontCheck==checkBackupAvailability) //or didn't we call Backint at all
        {
            EBIDIsAvailable=1;
        }
        else
            EBIDIsAvailable=0;


        rc=BuildAEBIDFrom(TempBID, BackupInfo->SourceDBName(), TheLine->GiveBID(), TheLine->GiveName());

        if(rc)
        {
            EBID=TempBID;
            EBIDLength=strlen(EBID);

            DataType=TheLine->GiveDataType();
            DateTimeString=TheLine->GiveDateTime();
        }
        
        InFileItem=InFileItem->Next();
    }

    return rc;
}

int tcn353_Backint2Connector::CloseToolOutputOfAvailableBackups()
{
    cn36_StrDealloc(TempBID);
    return 1;
}

int tcn353_Backint2Connector::GiveInformationOfLine(const char                   *   Line,
                                                    const char                   * & EBIDInLine,
                                                    size_t                         & EBIDLength,
                                                    tcn35d_BackupInfo::ADataType   & DataType,
                                                    const char                   * & DateTimeString,
                                                    int                            & LineContainsEBID)
{
    int rc=0;

    return rc;
}

int tcn353_Backint2Connector::ToolWasSuccessful(teo00_Uint8 DBBackupSize)
{
    int rc=1;

    if(ExistToolProcesses())
        rc=0;
    else
    {
        int i,
            ToolReturncode;

        for(i=0; i<MAX_NUMBER_OF_TOOLPROCESSES_CN35 && rc!=0; i++)
        {
            if(0!=ToolProcess[i])
            {
                ToolReturncode=ToolProcess[i]->GiveReturnCode();

                if(tcn35d_BackupInfo::GiveBackupInfo==BackupInfo->DataDirection() && DataPipes==BackintDataType)  //backint for MaxDB was called with "-f inquire"
                {
                    if(0>ToolReturncode || 2<ToolReturncode)    //return code can be 0,1 or 2 (with 2 being a harmless concession for some existing Backint for Oracle implementations)
                        rc=0;
                }
                else    //on backup or restore
                {
                    if(0!=ToolReturncode && 1!=ToolReturncode)  //successful backup and restore must end with return code 0 or 1
                        rc=0;
                }
            }
        }
    }

    return rc;
}

int tcn353_Backint2Connector::BackupOrRestoreHistoryWithBackint()
{
    int rc=1;

    switch(BackupInfo->DataDirection())
    {
        case tcn35d_BackupInfo::ABackup:
            LogWriter->Write("Making a backup of the Backup History files.\n", tcn35d_DBBackupControllerLogWriter::AddALevelToPrefix);
            break;
        case tcn35d_BackupInfo::GiveBackupInfo:
            LogWriter->Write("Restoring the needed Backup History files.\n", tcn35d_DBBackupControllerLogWriter::AddALevelToPrefix);
            break;
        default:
            ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
            rc=0;
    }

    if(rc)
    {
        if(ToolIsRunning())
        {
            rc=0;
            ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError)); // TODO : extra error for this case?
        }
        else
        {
            NumberOfRunningToolProcesses=0;

            LogWriter->Write("\n");

            if(tcn35d_BackupInfo::ABackup==BackupInfo->DataDirection() && !MoveToolOutputToProtocolFile())    // move stdout and stderr of the backup operation to the protocol (dbm.ebp)
                rc=0;
                
            if(rc)
            {
                BackintDataTypeEnum OldBackintDataType=BackintDataType; // save the current data type
                BackintDataType=HistoryFiles;                           // set the type temporarily to tcn353_Backint2Connector::HistoryFiles

                LogWriter->Write("\n");

                switch(BackupInfo->DataDirection())
                {
                    case tcn35d_BackupInfo::ABackup:
                        LogWriter->Write("Preparing backup.\n", tcn35d_DBBackupControllerLogWriter::AddALevelToPrefix);
                        rc=Prepare();
                        LogWriter->Write(rc?"Prepare passed successfully.\n":"Prepare failed.\n", tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefixAndEndParagraph);
                        break;
                    case tcn35d_BackupInfo::GiveBackupInfo:
                        LogWriter->Write("Preparing restore.\n", tcn35d_DBBackupControllerLogWriter::AddALevelToPrefix);
                        rc=Prepare();
                        LogWriter->Write(rc? "Prepare passed successfully.\n": "Prepare failed.\n", tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefixAndEndParagraph);
                        break;
                    default:
                        ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
                        rc=0;
                }

                if(rc)
                {
                    if(StartTool())
                    {
                        if(CheckTool())
                        {
                            long lastOutputTime=0;
                            long outputIntervall=0;
                            bool outputResult;

                            LogWriter->Write("Waiting for the end of Backint for MaxDB.\n", tcn35d_DBBackupControllerLogWriter::AddALevelToPrefix);

                            while(ToolIsRunning(outputResult=(outputIntervall<=cn36_SecondsBetween(cn36_GetCurrentTime(), lastOutputTime))))
                            {
                                if(outputResult)                          //if the backup tool status was told this time...
                                {
                                    lastOutputTime=cn36_GetCurrentTime(); //...we have a new time for the last told backup status
                                    outputIntervall+=5;                   //...we increase the output intervall to reduce output over time
                                }

                                Sleep(1, false);
                            }


                            NumberOfRunningToolProcesses=0; //tool is not longer running, set NumberOfRunningToolProcesses=0 to allow further calls of the tool

                            LogWriter->Write("Ended the waiting.\n", tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefixAndEndParagraph);

                            if(!ToolWasSuccessful(0))   // BackupSize==0, because the backup size is unimportant with Backint for MaxDB
                            {
                                rc=0;
                                ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseBackupToolFailed, SumOfToolExitCodes()));
                            }
                            else
                            {
                                int NumberOfEBIDsFound;

                                LogWriter->Write("Checking output of Backint for MaxDB.\n", tcn35d_DBBackupControllerLogWriter::AddALevelToPrefix);

                                rc=CollectExternalBackupIDs(0, NumberOfEBIDsFound); //just check if the Backint for MaxDB has assigned any Backup ID's to the files

                                if(rc && NumberOfEBIDsFound!=NumberOfRequestedNames)
                                {
                                    rc=0;
                                    ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseBackupToolFailed));
                                }

                                LogWriter->Write(rc?"Have found all BID's as expected.\n":"Could not found all BID's as expected.\n", tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefixAndEndParagraph);
                            }

                            if(!rc)
                                switch(BackupInfo->DataDirection())
                                {
                                    case tcn35d_BackupInfo::ABackup:
                                        ErrorHandler->Report(tcn353_BackintError(tcn353_BackintError::CaseCouldNotBackupBackupHistoryFiles));
                                        break;
                                    case tcn35d_BackupInfo::GiveBackupInfo:
                                        ErrorHandler->Report(tcn353_BackintError(tcn353_BackintError::CaseCouldNotRestoreBackupHistoryFiles));
                                        break;
                                    default:
                                        ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
                                        rc=0;
                                }
                        }
                        else
                            rc=0;
                    }
                    else
                        rc=0;

                    if(tcn35d_BackupInfo::GiveBackupInfo==BackupInfo->DataDirection())
                    {
                        DeleteBackintInputList();   //do not confuse the inquire with the restore of backup history files

                        if(!MoveToolOutputToProtocolFile()) // move stdout and stderr of the restore operation to the protocol (dbm.ebp)
                            rc=0;
                    }
                }

                BackintDataType=OldBackintDataType;                     // set the type back to the former value
            }
        }
    }

    switch(BackupInfo->DataDirection())
    {
        case tcn35d_BackupInfo::ABackup:
            LogWriter->Write(rc?"Have saved the Backup History files successfully.\n":
                             "Could not save the Backup History files correctly.\n",
                             tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefixAndEndParagraph);
            break;
        case tcn35d_BackupInfo::GiveBackupInfo:
            LogWriter->Write(rc?"Have restored the needed Backup History files successfully.\n":
                             "Could not restore the needed Backup History files correctly.\n",
                             tcn35d_DBBackupControllerLogWriter::SubALevelFromPrefixAndEndParagraph);
            break;
        default:
            ErrorHandler->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
            rc=0;
    }

    return rc;
}
