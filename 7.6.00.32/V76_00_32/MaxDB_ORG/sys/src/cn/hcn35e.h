/*******************************************************************************
  module:       hcn35e.h
  ------------------------------------------------------------------------------
  responsible:  TiloH
  special area: cn layer
  description:  class for controling errors, log writers and configuration
                variables in the DBMServer
  last change:  2002-04-02 19:34
  version:      7.4.3
  see also:
  ------------------------------------------------------------------------------
                         Copyright (c) 2002-2005 SAP AG


    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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

#if !defined (_HCN35E_H_)
#define _HCN35E_H_


// -----------------------------------------------------------------------------
// includes
// -----------------------------------------------------------------------------
#include <stdio.h>

#include "gcn003.h"
#include "hni31.h"
#include "hcn35d.h"


// -----------------------------------------------------------------------------
// class tcn35e_DBMBackupControllerLogWriter
// -----------------------------------------------------------------------------

class tcn35e_DBMBackupControllerLogWriter: public tcn35d_DBBackupControllerLogWriter
{
  public:
    tcn35e_DBMBackupControllerLogWriter(const char * TheNameOfProtFile,
                                        const char * TheNameOfLogFile,
                                        const char * WishedNumberOfActions);
    ~tcn35e_DBMBackupControllerLogWriter();

    void setProperty(MessageModifier Modifier);
    int Write(const char * Message, MessageModifier Modifier=UsePrefix);
    int Write(MessageModifier Modifier);
    int Write(tni34_AFile & TheFileToMove, const char * FileDescription);

    int IsGood();

    void ReinitLogWriter();

    static tcn35e_DBMBackupControllerLogWriter BasicLogWriter_cn35e;

    static const char * ParameterNameDBMEBLSize;

  private:  
    static const int    MaxPrefixChars;        //=256+1;
    static const int    CharsPerPrefixLevel;   //=4;
    static const int    MaxPrefixLevels;       //=(MaxPrefixChars_cn36-1)/CharsPerPrefixLevel_cn36;
    static const char * LogFileActionStart;
    static const char * LogFileActionEnd;

    int    ClearProtFile;
    int    WishedNumberOfActionsInLogFile;
    char * NameOfProtFile;
    char * NameOfLogFile;
    int    CurrentPrefixLevel;
    char * LevelPrefix;
    int    IsGoodFlag;
    int    DelayedNewline;

    char   CurrentTime[30];
    bool   useTimeInPrefix;

    static const char * LineDelimiter;

    FILE * ProtStream;  //TODO replace with tni34_AFile
    FILE * LogStream;

    int  OpenStream();
    void CloseStream();

    int PrintTimeIfNecessary(MessageModifier Modifier);
    int PrintDelayedNewLineIfNecessary();

    void         constructCurrentTimeString();
    bool         isCurrentTimeStringEmpty() const;
    const char * giveCurrentTimeString() const;

    int ReadNextLine(tsp00_Int4 & File, char *& Line, size_t & LineLength, int & ReachedEndOfFile);
};


// -----------------------------------------------------------------------------
// class tcn35e_DBMBackupControllerErrorHandler
// -----------------------------------------------------------------------------

class tcn35e_DBMBackupControllerErrorHandler: public tcn35d_DBBackupControllerErrorHandler
{
  public:
    tcn35e_DBMBackupControllerErrorHandler(char                               * TheReplyBuffer, 
                                           int                                * TheReplyBufferLength, 
                                           tcn35d_DBBackupControllerLogWriter * TheLogWriter);
    ~tcn35e_DBMBackupControllerErrorHandler();

    void SetReplyArrayTo(char * TheNewReplyBuffer,
                         int  * TheNewReplyBufferLength);

    void Report(const tni35_AError & Err);
    void reportString(const char * string);
    void reportMessageList(const DBMSrvMsg_Error & err);
    tcn00_Error GiveLastError();

    static tcn35e_DBMBackupControllerErrorHandler BasicErrorHandler_cn35e;

  private:
    static struct APhaseToDBMErrorMap
        {tcn35d_DBBackupControllerErrorHandler::ErrorPhase  ErrorPhase;
         tcn00_Error                                        TheDBMError;} ErrorMap[11];

    char      * ReplyData;
    int       * ReplyLen;
    tcn00_Error LastError;
    int         ReplyAlreadyConstructed;

    tcn35d_DBBackupControllerLogWriter * LogWriter;

    void AnswerCNError( _TCHAR * szBuffer, int * pnDataLen, const tcn00_Error nError, const char * szInfo);
};


// -----------------------------------------------------------------------------
// class tcn35e_DBMBackupError
// -----------------------------------------------------------------------------

class tcn35e_DBMBackupError: public tcn35d_DBBackupError
{
  public:
    enum DBMBackupErrorCase
    {
        CaseIllegalBlockingFactor,
        CaseIllegalBlockingFactorParameter,
        CaseIllegalPageSize,

        CaseIllegalStageFileSize,
        CaseInsufficientHisoryData,
        CaseStageFileMissing,
        CaseStageChangedInSize,
        CaseStageChangedInDateTime,
        CaseCanNotReadLabel,
        CaseStageChangedInLabel,

        CaseIncorrectPagesTransfered,
        CaseNoPagesTransfered,

        CaseBackupToolAndDBFailed,
        CaseBackupToolKilled,
        CaseBackupToolFailedAndDBKernelRequestCanceled,
        CaseBackupToolKilledAndDBKernelRequestCanceled,
        CaseDBKernelRequestCanceled,
        CaseCancelDBKernelRequestFailed,
        CaseBackupToolFailedAndCancelDBKernelRequestFailed,
        CaseBackupToolKilledAndCancelDBKernelRequestFailed,

        CasePipe2FileFailed,
        CaseBackupToolAndPipe2FileFailed,
        CasePipe2FileKilled,
        CaseBackupToolFailedAndPipe2FileKilled,
        CaseBackupToolAndPipe2FileKilled,
        CaseStageVerifyFailed,

        CaseCouldNotMovePipe2FileOutputToToolProtFile,

        CaseSQLError,
        CaseCN00Error,

        CaseNoMediumFound,

        CaseCouldNotGetNameOfToolHistFile,
        CaseCouldNotGetInternalBackupIDs,
        CaseCouldNotGetExternalBackupIDs,
        CaseCouldNotWriteToToolHistFile,
        CaseNoLogPageInformation,
        CaseCouldNotGetNameOfBackupHistory,
        CaseCouldNotGetNameOfToolLogFile,
        CaseCouldNotGetNameOfToolProtFile   //must remain the last error case (or change SetThisError())
    };

    tcn35e_DBMBackupError(DBMBackupErrorCase ErrCase);
    tcn35e_DBMBackupError(DBMBackupErrorCase ErrCase, const char *ErrInfo);
    tcn35e_DBMBackupError(DBMBackupErrorCase ErrCase, int TheDBCode, const char * TheSQLErrText);
    tcn35e_DBMBackupError(DBMBackupErrorCase ErrCase, int TheDBCode, int ErrInfo);
    tcn35e_DBMBackupError(DBMBackupErrorCase ErrCase, int ErrInfo);

    virtual void FillErrorMsgBuffer() const;

    enum DBMBackupErrorInfoType {None=tcn35d_DBBackupError::None,
                                 AString=tcn35d_DBBackupError::AString,
                                 AIntCode=tcn35d_DBBackupError::AIntCode,
                                 AIntAndADBCode,
                                 ADBCodeAndAInt,
                                 ADBCodeAndAString};

  protected:
    virtual void FillErrorMsgBuffer(const DBBackupErrorDescription &ErrDesc) const;

  private:
    static DBBackupErrorDescription DBMBackupErrorToText[];

    int DBCode;

    void SetThisError(int CaseToSet);
};


// -----------------------------------------------------------------------------
// class tcn35e_ConfigurationVariableHandler
// -----------------------------------------------------------------------------

class tcn35e_ConfigVariable
{
  public:
    tcn35e_ConfigVariable(char *TheName, char *TheValue);
    ~tcn35e_ConfigVariable();

    const char * GiveName();
    const char * GiveValue();
    int          SetValue(const char * NewValue);

  private:
    char *Name;
    char *Value;
};

class tcn35e_ConfigurationVariableHandler: public tcn35d_ConfigurationVariableHandler
{
  public:
    tcn35e_ConfigurationVariableHandler();
    ~tcn35e_ConfigurationVariableHandler();

    int GetConfigurationVariable(const tcn35d_BackupInfo *  BackupInfo,
                                 const char              *  VariableName,
                                 char                    *& FileEnvVar);

    const ni31_List<tcn35e_ConfigVariable *> & ConfigurationVariableToExportList();

    void ReinitConfigVarHandler();

  private:
    int                                ConfigVarListFilled;
    ni31_List<tcn35e_ConfigVariable *> ConfigVarList;

    static const char * MagicStringExportVariables;

    int FillConfigVarList(const char * DBName);
};


// -----------------------------------------------------------------------------
// class tcn35e_TheDBMBackupControllerFactory
// -----------------------------------------------------------------------------

class tcn35e_TheDBMBackupControllerFactory
{
  public:
    int MediumInfo(const char                             *& MediumName,
                   int                                    &  NumberOfPipes,
                   char                                   *  ThePipeNames[MAX_NUMBER_OF_DATAPIPES_CN35D],
                   char                                   *  TheMediumTypes[MAX_NUMBER_OF_DATAPIPES_CN35D],
                   long                                   &  BytesPerBlock,
                   long                                   &  PagesPerBlock,
                   long                                   &  BytesPerPage,
                   long                                      TheCountInPages[MAX_NUMBER_OF_DATAPIPES_CN35D],
                   const char                             *  TheDBName,
                   const tcn003_Medium                    *  pMedium,
                   const tcn003_MediaGroup                *  pMediaGroup,
                   tcn35e_DBMBackupControllerErrorHandler &  ErrorHandler);

  protected:
    int RunDirectory(char                                   *& TheRunDirectory,
                     const char                             *  TheDBName,
                     tcn35e_DBMBackupControllerErrorHandler *  ErrorHandler,
                     tcn35e_DBMBackupControllerLogWriter    *  LogWriter);

    int BackupHistories(char                                  *& BackupHistory,
                        char                                  *& ExternalBackupHistory,
                        const char                            *  DBName,
                        tcn35e_DBMBackupControllerErrorHandler &  ErrorHandler);
};

#endif
