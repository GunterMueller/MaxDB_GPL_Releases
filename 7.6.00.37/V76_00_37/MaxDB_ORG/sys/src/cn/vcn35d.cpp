/*!*****************************************************************************
  module:       vcn35d.cpp
  ------------------------------------------------------------------------------
  responsible:  TiloH
  special area: cn layer
  description:  base classes for controling backups/replications to external
                backup tools
  last change:  2001-11-08 14:19
  version:      7.3.0
  see also:     
  ------------------------------------------------------------------------------
                         Copyright (c) 2001-2005 SAP AG


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



*******************************************************************************/

// -----------------------------------------------------------------------------
// includes
// -----------------------------------------------------------------------------

#include "gcn003.h"
#include "hcn35d.h"
#include "hcn36.h"

#include <stdio.h>


// -----------------------------------------------------------------------------
// member functions tcn35d_DBBackupControllerErrorHandler
// -----------------------------------------------------------------------------

tcn35d_DBBackupControllerErrorHandler::tcn35d_DBBackupControllerErrorHandler()
   :CurrentErrorPhase(CommonError),
    AnErrorWasReported(0)
{
    SetNewErrorHandler(this);
}

tcn35d_DBBackupControllerErrorHandler::~tcn35d_DBBackupControllerErrorHandler()
{
    if(ErrorHandler_ni35==this)
        SetErrorHandlerToDefault();
}

void tcn35d_DBBackupControllerErrorHandler::Report(const tni35_AError & Err)
{
    //do nothing with the error
    AnErrorWasReported=1;
}

void tcn35d_DBBackupControllerErrorHandler::reportString(const char * string)
{
    //do nothing with the error
    AnErrorWasReported=1;
}

void tcn35d_DBBackupControllerErrorHandler::reportMessageList(const DBMSrvMsg_Error & err)
{
    //do nothing with the error
    AnErrorWasReported=1;
}

int tcn35d_DBBackupControllerErrorHandler::AnErrorOccured()
{
    return AnErrorWasReported;
}

void tcn35d_DBBackupControllerErrorHandler::SetPhaseTo(ErrorPhase Phase)
{
    CurrentErrorPhase=Phase;
}


// -----------------------------------------------------------------------------
// member functions tcn35d_DBBackupControllerLogWriter
// -----------------------------------------------------------------------------

tcn35d_DBBackupControllerLogWriter::~tcn35d_DBBackupControllerLogWriter()
{
    //nothing to do
}

void tcn35d_DBBackupControllerLogWriter::setProperty(MessageModifier Modifier)
{
    //do nothing
}

int tcn35d_DBBackupControllerLogWriter::Write(const char * Message, MessageModifier Modifier)
{
    return 1;   //do nothing and report it was OK
}

int tcn35d_DBBackupControllerLogWriter::Write(MessageModifier Modifier)
{
    return 1;   //do nothing and report it was OK
}

int tcn35d_DBBackupControllerLogWriter::Write(tni34_AFile & TheFileToMove, const char * FileDescription)
{
    return 1;   //do nothing and report it was OK
}

// -----------------------------------------------------------------------------
// member functions tcn35d_BackupInfo
// -----------------------------------------------------------------------------

tcn35d_BackupInfo::tcn35d_BackupInfo(ADataDirection TheDataDirection,
                                     ADataType      TheDataType,
                                     const char *   TheMediaName,
                                     int            TheNumberOfPipes,
                                     char       *   ThePipeName[MAX_NUMBER_OF_DATAPIPES_CN35D],
                                     char       **  TheMediumType,
                                     int            TheBlockSize,
                                     int            ThePagesPerBlock,
                                     long           TheCountInPages[MAX_NUMBER_OF_DATAPIPES_CN35D],
                                     const char *   TheDefaultDirectory,
                                     const char *   TheNodeName,
                                     const char *   TheDBName,
                                     const char *   TheSourceNodeName,
                                     const char *   TheSourceDBName,
                                     const char *   ThePathOfBackupHistory,
                                     const char *   ThePathOfExternalBackupHistory)
   :NumberPipes(TheNumberOfPipes),
    MDataDirection(TheDataDirection),
    MDataType(TheDataType),
    MNodeName(0),
    MDBName(0),
    MSourceNodeName(0),
    MSourceNodeNameSetExplicitly(0),
    MSourceDBName(0),
    MMediaName(0),    
    MBlockSize(TheBlockSize),
    MPagesPerBlock(ThePagesPerBlock),
    MDefaultDirectory(0),
    MPathOfBackupHistory(0),
    MPathOfExternalBackupHistory(0),
    IsGoodFlag(1)
{
    int i;

    if(!cn36_StrAllocCpy(MMediaName, TheMediaName) ||
       !cn36_StrAllocCpy(MDefaultDirectory, TheDefaultDirectory) ||

       !cn36_StrAllocCpy(MNodeName, TheNodeName) ||
       !cn36_StrAllocCpy(MDBName, TheDBName) ||
       (0!=TheSourceNodeName && !cn36_StrAllocCpy(MSourceNodeName, TheSourceNodeName)) ||
       (0==TheSourceNodeName && !cn36_StrAllocCpy(MSourceNodeName, TheNodeName)) ||
       (0!=TheSourceDBName && !cn36_StrAllocCpy(MSourceDBName, TheSourceDBName)) ||
       (0==TheSourceDBName && !cn36_StrAllocCpy(MSourceDBName, TheDBName)) ||

       (0!=ThePathOfBackupHistory && !cn36_StrAllocCpy(MPathOfBackupHistory, ThePathOfBackupHistory)) ||
       (0!=ThePathOfBackupHistory && !cn36_StrAllocCpy(MPathOfExternalBackupHistory, ThePathOfExternalBackupHistory)))
    {
        IsGoodFlag=0;
    }

    if(0!=TheSourceNodeName)
        MSourceNodeNameSetExplicitly=1;

    for(i=0; i<MAX_NUMBER_OF_DATAPIPES_CN35D; i++)
    {
        PipeName[i]=ThePipeName[i];
        ThePipeName[i]=0;   //avoid deletion of the strings via ThePipeName[]

        PipeRemoved[i]=0;

        MCountInPages[i]=TheCountInPages[i];
    }

    for(i=0; i<NumberPipes; ++i)
    {
        if(0!=TheMediumType)
        {
            MMediumType[i]=TheMediumType[i];
            TheMediumType[i]=0; //avoid deletion of the strings via TheMediumType[]
        }
        else
            MMediumType[i]=0;
    }

    for(i=NumberPipes; i<MAX_NUMBER_OF_DATAPIPES_CN35D; ++i)
        MMediumType[i]=0;
}

int tcn35d_BackupInfo::SetMedium(ADataDirection TheDataDirection,
                                 const char   * TheNewMediaName,
                                 int            TheNewNumberOfPipes,
                                 char         * TheNewPipeName[MAX_NUMBER_OF_DATAPIPES_CN35D],
                                 char        ** TheNewMediumType,
                                 int            TheNewBlockSize,
                                 int            TheNewPagesPerBlock,
                                 long           TheCountInPages[MAX_NUMBER_OF_DATAPIPES_CN35D])
{
    int rc=1,
        i;
    MDataDirection=TheDataDirection;
    NumberPipes=TheNewNumberOfPipes;

    if(!cn36_StrAllocCpy(MMediaName, TheNewMediaName))
    {
        IsGoodFlag=0;
        rc=0;
    }

    for(i=0; i<MAX_NUMBER_OF_DATAPIPES_CN35D; i++)
    {
        cn36_StrDealloc(PipeName[i]);   //last chance to throw the old pipe name away
        PipeName[i]=TheNewPipeName[i];  
        TheNewPipeName[i]=0;            //avoid deletion of the strings via ThePipeName[]

        PipeRemoved[i]=0;

        cn36_StrDealloc(MMediumType[i]); //last chance ...

        MCountInPages[i]=TheCountInPages[i];
    }

    for(i=0; i<NumberPipes; ++i)
    {
        if(0!=TheNewMediumType)
        {
            MMediumType[i]=TheNewMediumType[i];
            TheNewMediumType[i]=0;
        }
        else
            MMediumType[i]=0;
    }

    MBlockSize=TheNewBlockSize;
    MPagesPerBlock=TheNewPagesPerBlock;

    return rc;
}

tcn35d_BackupInfo::~tcn35d_BackupInfo()
{
    int i;

    cn36_StrDealloc(MNodeName);
    cn36_StrDealloc(MDBName);
    cn36_StrDealloc(MSourceNodeName);
    cn36_StrDealloc(MSourceDBName);
    cn36_StrDealloc(MMediaName);
    cn36_StrDealloc(MDefaultDirectory);
    cn36_StrDealloc(MPathOfBackupHistory);
    cn36_StrDealloc(MPathOfExternalBackupHistory);

    for(i=0; i<MAX_NUMBER_OF_DATAPIPES_CN35D; i++)
    {
        cn36_StrDealloc(PipeName[i]);
        cn36_StrDealloc(MMediumType[i]);
    }
}

int tcn35d_BackupInfo::NumberOfPipes() const
{
    return NumberPipes;
}

const char * tcn35d_BackupInfo::NameOfPipe(unsigned int i) const
{
    return PipeName[i];
}

void tcn35d_BackupInfo::MarkPipeAsRemoved(unsigned int i)
{
    PipeRemoved[i]=1;
}

int tcn35d_BackupInfo::WasMarkedAsRemoved(unsigned int i) const
{
    return (1==PipeRemoved[i]);
}

const char * tcn35d_BackupInfo::MediumType(unsigned int i) const
{
    return MMediumType[i];
}

void tcn35d_BackupInfo::SetDataDirection(ADataDirection TheNewDataDirection)
{
    MDataDirection=TheNewDataDirection;
}

tcn35d_BackupInfo::ADataDirection tcn35d_BackupInfo::DataDirection() const
{
    return MDataDirection;
}

const char * tcn35d_BackupInfo::getDataDirectionAsString() const
{
    switch(MDataDirection)
    {
        case tcn35d_BackupInfo::ABackup:        return "backup"; break;
        case tcn35d_BackupInfo::ARestore:       return "restore"; break;
        case tcn35d_BackupInfo::GiveBackupInfo: return "inquiry of backup information"; break;
        default: return "";
    }
}

tcn35d_BackupInfo::ADataType tcn35d_BackupInfo::DataType() const
{
    return MDataType;
}

const char * tcn35d_BackupInfo::BackupTypeAsString() const
{
    if(DataRecovery==MDataType || DataMigration==MDataType)
        return ME_SAVETYPE_DAT_CN003;
    else
        if(PagesRecovery==MDataType || PagesMigration==MDataType)
            return ME_SAVETYPE_PAG_CN003;
        else
            if(Log==MDataType)
                return ME_SAVETYPE_LOG_CN003;
            else
                return "";
}

const char * tcn35d_BackupInfo::NodeName() const
{
    return MNodeName;
}

const char * tcn35d_BackupInfo::DBName() const
{
    return MDBName;
}

const char * tcn35d_BackupInfo::SourceNodeName() const
{
    return MSourceNodeName;
}

int tcn35d_BackupInfo::SourceNodeNameSetExplicitly() const
{
    return MSourceNodeNameSetExplicitly;
}

const char * tcn35d_BackupInfo::SourceDBName() const
{
    return MSourceDBName;
}

const char * tcn35d_BackupInfo::MediaName() const
{
    return MMediaName;
}

int tcn35d_BackupInfo::BlockSize() const
{
    return MBlockSize;
}

int tcn35d_BackupInfo::PagesPerBlock() const
{
    return MPagesPerBlock;
}

long tcn35d_BackupInfo::CountInPages(unsigned int i) const
{
    return MCountInPages[i];
}

const char * tcn35d_BackupInfo::DefaultDirectory() const
{
    return MDefaultDirectory;
}

const char * tcn35d_BackupInfo::PathOfBackupHistory() const
{
    return MPathOfBackupHistory;
}
const char * tcn35d_BackupInfo::PathOfExternalBackupHistory() const
{
    return MPathOfExternalBackupHistory;
}

int tcn35d_BackupInfo::IsGood() const
{
    return IsGoodFlag;
}


// -----------------------------------------------------------------------------
// member functions of class tcn35d_ConfigurationVariableHandler
// -----------------------------------------------------------------------------

tcn35d_ConfigurationVariableHandler::~tcn35d_ConfigurationVariableHandler()
{
    //nothing to do
}

int tcn35d_ConfigurationVariableHandler::GetConfigurationVariable(const tcn35d_BackupInfo *  BackupInfo,
                                                                  const char              *  VariableName,
                                                                  char                    *& FileEnvVar)
{
    FileEnvVar=0;

    return 1;
}

// -----------------------------------------------------------------------------
// member of class tcn35d_DBBackupError
// -----------------------------------------------------------------------------

const int tcn35d_DBBackupError::MaximalCharsForAnInt=20;
tcn35d_DBBackupError tcn35d_DBBackupError::OutOfMemory(tcn35d_DBBackupError::CaseOutOfMemory, tcn35d_DBBackupError::DoNotSetErrorNow);

tcn35d_DBBackupError::tcn35d_DBBackupError()    // This constructor is protected.
:tni35_AError(0)
{
    SetThisError(CaseInternalError);
    IntCode=0;
}

tcn35d_DBBackupError::tcn35d_DBBackupError(DBBackupErrorCase ErrCase, DBBackupErrorSetFlag SetFlag)
:tni35_AError(0) // do not set basis error, to avoid doing the same twice
{
    SetThisError(ErrCase);
    IntCode=0;

    if(SetErrorNow==SetFlag)
        Set();
}

tcn35d_DBBackupError::tcn35d_DBBackupError(DBBackupErrorCase ErrCase, const char *ErrInfo )
{
    SetThisError(ErrCase);
    IntCode=0;
    Set(ErrInfo);
}

tcn35d_DBBackupError::tcn35d_DBBackupError(DBBackupErrorCase ErrCase, int ErrInfo)
{
    SetThisError(ErrCase);
    IntCode=ErrInfo;
    Set("");
}

tcn35d_DBBackupError & tcn35d_DBBackupError::SetI(int ErrInfo)
{
    IntCode=ErrInfo;

    Set("");

    return *this;
}

void tcn35d_DBBackupError::SetThisError(int CaseToSet)
{
    if(DBBackupErrorToText[CaseToSet].Case==CaseToSet)
        ThisError=DBBackupErrorToText+CaseToSet;
    else
    {
        ThisError=DBBackupErrorToText;
        
        while(ThisError->Case!=CaseToSet && ThisError->Case!=CaseInternalError)
            ThisError++;
    }
}

const tcn35d_DBBackupError::DBBackupErrorDescription * tcn35d_DBBackupError::GiveDescriptionOfInternalError()
{
    return DBBackupErrorToText+CaseInternalError;
}

void tcn35d_DBBackupError::FillErrorMsgBuffer() const
{
    FillErrorMsgBuffer(*ThisError);
}

void tcn35d_DBBackupError::FillErrorMsgBuffer(const DBBackupErrorDescription &ErrDesc) const
{
    size_t LengthOfExtendedErrorMsg;

    switch(ErrDesc.InfoType)
    {
        case tcn35d_DBBackupError::None:
            LengthOfExtendedErrorMsg=0;
        break;

        case AString:
            LengthOfExtendedErrorMsg=strlen(ThisError->ExtendedErrorText)-2+strlen(GiveErrorInfoBuffer());
        break;

        case AIntCode:
            LengthOfExtendedErrorMsg=strlen(ThisError->ExtendedErrorText)-2+MaximalCharsForAnInt;
        break;

        default:
            ErrorHandler_ni35->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
    }

    if(ErrorMsgBufferLength_ni35-1 >= LengthOfExtendedErrorMsg)
    {
        switch(ErrDesc.InfoType)
        {
            case None:
                cn36_StrNCpy(ErrorMsgBuffer, ThisError->PlainErrorText, ErrorMsgBufferLength_ni35-1);
            break;

            case AString:
                sprintf(ErrorMsgBuffer, ThisError->ExtendedErrorText, GiveErrorInfoBuffer());
            break;

            case AIntCode:
                sprintf(ErrorMsgBuffer, ThisError->ExtendedErrorText, IntCode);
            break;

            default:
                ErrorHandler_ni35->Report(tcn35d_DBBackupError(tcn35d_DBBackupError::CaseInternalError));
        }
    }
    else
        cn36_StrNCpy(ErrorMsgBuffer, ThisError->PlainErrorText, ErrorMsgBufferLength_ni35-1);
}

tcn35d_DBBackupError::DBBackupErrorDescription tcn35d_DBBackupError::DBBackupErrorToText[]=
{
     {CaseOutOfMemory,
     None,
     "Out of memory.",
     0},


     {CaseADirectoryIsMissing,
     AString,
     "A directory is missing.",
     "The directory %s is missing."},

    {CaseAFileIsMissingOrUnreadable,
     AString,
     "A file is missing or unreadable.",
     "The file %s is missing or unreadable."},

    {CaseAFileIsNotExecutable,
     AString,
     "A file is not executable.",
     "The file %s is not executable."},

    {CaseAFileAlreadyExists,
     AString,
     "A file already exists.",
     "The file '%s' already exists."},


    {CaseTooManyPipes,
     None,
     "The medium consists of too many pipes.",
     0},

    {CaseTooFewPipes,
     None,
     "The medium consists of too few pipes.",
     0},

    {CaseWrongMediumType,
     AString,
     "A medium has an illegal medium type.",
     "The medium type %s is not allowed."},

    {CaseCouldNotCreateDataPipe,
     AString,
     "Could not create a data transfer pipe.",
     "Could not create the data transfer pipe '%s'."},

    {CaseSyntaxErrorEBIDs,
     AIntCode,
     "The list of external backup ID's contains not only external backup ID's (syntax error).",
     "The list of external backup ID's contains no external backup ID at list element %d."},

    {CaseEBIDsOfDifferentDBs,
     AString,
     "The list of external backup ID's contains ID's from different databases.",
     "The list of external backup ID's contains not only ID's from database %s."},

    {CaseEBIDsOfDifferentBackups,
     None,
     "The list of external backup ID's contains ID's from different backups.",
     0},

    {CaseTooManyEBIDs,
     AIntCode,
     "The list of external backup ID's contains too many ID's.",
     "The list of external backup ID's contains more than %d ID's."},

    {CaseTooFewEBIDs,
     AIntCode,
     "The list of external backup ID's contains too few ID's",
     "The list of external backup ID's contains less than %d ID's."},

    {CaseCanNotSignalEndToTool,
     None,
     "Could not signal end to tool.",
     0},


    {CaseNoBackupIsRunning,
     None,
     "No backup is running.",
     0},

    {CaseBackupToolFailed,
     AIntCode,
     "The backup tool failed.",
     "The backup tool failed with %d as sum of exit codes."},


    {CaseUnknownFormatOfDBMKnlFile,
     AString,
     "Unknown format of backup history.",
     "Unknown format of backup history %s."},

    {CaseUnknownFormatOfDBMEbfFile,
     AString,
     "Unknown format of external backup history.",
     "Unknown format of external backup history %s."},

     {CaseCouldNotMoveToolOutputToToolProtFile,
     AString,
     "Could not move output of tool to protocol file.",
     "Could not move output of tool from file %s to protocol file."},


     {CaseNeedExternalBackupIDForRestore,
     None,
     "An external backup ID is needed, but not supplied.",
     0},


     {CaseInternalError,
     None,
     "An internal error occured.",
     0}
};
