/*!*****************************************************************************
  module:       hcn35d.h
  ------------------------------------------------------------------------------
  responsible:  TiloH
  special area: cn layer
  description:  base classes for controling backups/replications to external
                backup tools
  last change:  2001-10-11 19:13
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

#if !defined (_HCN35D_H_)
#define _HCN35D_H_


// -----------------------------------------------------------------------------
// includes
// -----------------------------------------------------------------------------

#include "hni34.h"
#include "hni35.h"

#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"


// -----------------------------------------------------------------------------
// constants
// -----------------------------------------------------------------------------

#define MAX_NUMBER_OF_DATAPIPES_CN35D 64


// -----------------------------------------------------------------------------
// class tcn35d_DBBackupControllerErrorHandler
// -----------------------------------------------------------------------------

class tcn35d_DBBackupControllerErrorHandler: public tni35_AErrorHandler
{
  public:
    enum ErrorPhase {CommonError=0,
                     ToolCheck=1,
                     MediumCheck=2,
                     PrepareOperation=3,
                     DBRequest=4,
                     ToolRequest=5,
                     OperationCheck=6,
                     PostOperation=7,
                     BackupOperation=8,
                     CleanUp=9,
                     MediumNotFound=10};

    tcn35d_DBBackupControllerErrorHandler();
    virtual ~tcn35d_DBBackupControllerErrorHandler();

    virtual void Report(const tni35_AError & Err);
    virtual void reportString(const char * string);
    virtual void reportMessageList(const DBMSrvMsg_Error & err);

    int          AnErrorOccured();
    void         SetPhaseTo(ErrorPhase Phase);

  protected:
    ErrorPhase CurrentErrorPhase;
    int        AnErrorWasReported;
};


// -----------------------------------------------------------------------------
// class tcn35d_DBBackupControllerLogWriter
// -----------------------------------------------------------------------------

class tcn35d_DBBackupControllerLogWriter
{
  public:
    virtual ~tcn35d_DBBackupControllerLogWriter();

    enum MessageModifier{UsePrefix,
                         NoPrefix,
                         JustDateTime,
                         UsePrefixAndDateTime,
                         AddALevelToPrefix,
                         SubALevelFromPrefix,
                         EndParagraph,
                         SubALevelFromPrefixAndEndParagraph};

    virtual void setProperty(MessageModifier Modifier);
    virtual int Write(const char * Message, MessageModifier Modifier=UsePrefix);
    virtual int Write(MessageModifier Modifier);
    virtual int Write(tni34_AFile & TheFileToMove, const char * FileDescription);
};


// -----------------------------------------------------------------------------
// class tcn35d_BackupInfo
// -----------------------------------------------------------------------------

class tcn35d_BackupInfo
{
  public:
    enum ADataDirection {ABackup, ARestore, GiveBackupInfo};
    enum ADataType {DataRecovery=0, DataMigration=1, PagesRecovery=2, PagesMigration=3, Log=4};

    tcn35d_BackupInfo(ADataDirection TheDataDirection,
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
                      const char *   TheSourceNodeName=0,
                      const char *   TheSourceDBName=0,
                      const char *   ThePathOfBackupHistory=0,
                      const char *   ThePathOfExternalBackupHistory=0);

    void SetDataDirection(ADataDirection TheNewDataDirection);
    int  SetMedium(ADataDirection TheDataDirection,
                   const char   * TheNewMediaName,
                   int            TheNewNumberOfPipes,
                   char         * TheNewPipeName[MAX_NUMBER_OF_DATAPIPES_CN35D],
                   char         * TheNewMediumType[MAX_NUMBER_OF_DATAPIPES_CN35D],
                   int            TheNewBlockSize,
                   int            ThePagesPerBlock,
                   long           TheCountInPages[MAX_NUMBER_OF_DATAPIPES_CN35D]);

    ~tcn35d_BackupInfo();

    int          NumberOfPipes() const;
    const char * NameOfPipe(unsigned int i) const;
    void         MarkPipeAsRemoved(unsigned int i);
    int          WasMarkedAsRemoved(unsigned int i) const;
    const char * MediumType(unsigned int i) const;

    
    ADataDirection DataDirection() const;
    const char *   getDataDirectionAsString() const;
    ADataType      DataType() const;

    const char * BackupTypeAsString() const;

    const char * NodeName() const;
    const char * DBName() const;
    const char * SourceNodeName() const;
    int          SourceNodeNameSetExplicitly() const;
    const char * SourceDBName() const;

    const char * MediaName() const;

    int          BlockSize() const;
    int          PagesPerBlock() const;
    long         CountInPages(unsigned int i) const;
    
    const char * DefaultDirectory() const;

    const char * PathOfBackupHistory() const;
    const char * PathOfExternalBackupHistory() const;

    int IsGood() const;

  private:
    int    NumberPipes;
    char * PipeName[MAX_NUMBER_OF_DATAPIPES_CN35D];
    int    PipeRemoved[MAX_NUMBER_OF_DATAPIPES_CN35D];
    char * MMediumType[MAX_NUMBER_OF_DATAPIPES_CN35D];

    ADataDirection MDataDirection;
    ADataType      MDataType;

    char * MNodeName;      // TODO: should be a string<> (the other (char *) too)
    char * MDBName;
    char * MSourceNodeName;
    int    MSourceNodeNameSetExplicitly;
    char * MSourceDBName;

    char * MMediaName;
    int    MBlockSize;
    int    MPagesPerBlock;
    long   MCountInPages[MAX_NUMBER_OF_DATAPIPES_CN35D];

    char * MDefaultDirectory;

    char * MPathOfBackupHistory;
    char * MPathOfExternalBackupHistory;

    int IsGoodFlag;
};


// -----------------------------------------------------------------------------
// class tcn35d_ConfigurationVariableHandler
// -----------------------------------------------------------------------------

class tcn35d_ConfigurationVariableHandler
{
  public:
    virtual ~tcn35d_ConfigurationVariableHandler();

    virtual int GetConfigurationVariable(const tcn35d_BackupInfo *  BackupInfo,
                                         const char              *  VariableName,
                                         char                    *& FileEnvVar);
};


// -----------------------------------------------------------------------------
// class tcn35d_DBBackupError
// -----------------------------------------------------------------------------

class tcn35d_DBBackupError: public tni35_AError
{
  public:
    enum DBBackupErrorCase
    {
        CaseOutOfMemory,

        CaseADirectoryIsMissing,
        CaseAFileIsMissingOrUnreadable,
        CaseAFileIsNotExecutable,
        CaseAFileAlreadyExists,

        CaseTooManyPipes,
        CaseTooFewPipes,
        CaseWrongMediumType,
        CaseCouldNotCreateDataPipe,
        CaseSyntaxErrorEBIDs,
        CaseEBIDsOfDifferentDBs,
        CaseEBIDsOfDifferentBackups,
        CaseTooManyEBIDs,
        CaseTooFewEBIDs,
        CaseCanNotSignalEndToTool,

        CaseNoBackupIsRunning,
        CaseBackupToolFailed,

        CaseUnknownFormatOfDBMKnlFile,
        CaseUnknownFormatOfDBMEbfFile,
        CaseCouldNotMoveToolOutputToToolProtFile,

        CaseNeedExternalBackupIDForRestore,
        
        CaseInternalError   //must remain the last error case (or change SetThisError())
    };

    enum DBBackupErrorSetFlag {DoNotSetErrorNow, SetErrorNow};

    tcn35d_DBBackupError(DBBackupErrorCase ErrCase, DBBackupErrorSetFlag SetFlag=SetErrorNow);
    tcn35d_DBBackupError(DBBackupErrorCase ErrCase, const char *ErrInfo );
    tcn35d_DBBackupError(DBBackupErrorCase ErrCase, int ErrInfo);
    tcn35d_DBBackupError(DBBackupErrorCase ErrCase, int TheSQLCode, int ErrInfo);

    //using tni35_AError::Set; <- 'using' is not supported by all compilers!
    //tni35_AError & Set();
    //tni35_AError & Set(const char *ErrInfo);
    tcn35d_DBBackupError & SetI(int ErrInfo);

    virtual void FillErrorMsgBuffer() const;
    
    static tcn35d_DBBackupError OutOfMemory;

    enum DBBackupErrorInfoType {None, AString, AIntCode};

    struct DBBackupErrorDescription // must be public for initialization of static member DBBackupErrorToText
    {
        int          Case;
        int          InfoType;
        const char * PlainErrorText;
        const char * ExtendedErrorText;
    };

  protected:
    tcn35d_DBBackupError();

    const  DBBackupErrorDescription * ThisError;
    
    virtual void SetThisError(int CaseToSet);

    static const int MaximalCharsForAnInt;

    virtual void FillErrorMsgBuffer(const DBBackupErrorDescription &ErrDesc) const;
    int IntCode;

    const DBBackupErrorDescription * GiveDescriptionOfInternalError();

  private:
    static DBBackupErrorDescription DBBackupErrorToText[];
};

#endif
