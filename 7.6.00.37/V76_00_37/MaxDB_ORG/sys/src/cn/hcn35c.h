/*******************************************************************************
  module:       hcn35c.h
  ------------------------------------------------------------------------------
  responsible:  TiloH
  special area: cn layer
  description:  class for controling backups of staging area files to an
                external backup tool
  last change:  2002-03-11 18:38
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

#if !defined (_HCN35C_H_)
#define _HCN35C_H_

// -----------------------------------------------------------------------------
// includes
// -----------------------------------------------------------------------------

#include "gcn00.h"
#include "gcn003.h"
#include "hcn35a.h"
#include "hcn35e.h"


// -----------------------------------------------------------------------------
// class tcn35c_DBMStageBackupController
// -----------------------------------------------------------------------------

class StageFileInfo;

class cn35c_SuffixList
{
  public:
    int FillList(const char * SuffixListAsString,
                 size_t       SuffixListLength);

    const ni31_ListElem<int> * First();

  private:
    ni31_List<int> List;
};

class tcn35c_DBMStageBackupController: public tcn35a_DBBackupController
{
  public:
    tcn35c_DBMStageBackupController(tcn35_BackupToolConnector              * TheBackupToolConnector,
                                    tcn35e_DBMBackupControllerErrorHandler * TheErrorHandler,
                                    tcn35e_DBMBackupControllerLogWriter    * TheLogWriter,
                                    tcn35d_BackupInfo                      * TheBackupInfo,
                                    tcn35e_ConfigurationVariableHandler    * TheConfigurationVariableHandler,
                                    char                                   * TheReplyData,
                                    int                                    * TheReplyLen,
                                    const tcn003_Medium                    * TheStage,
                                    size_t                                   bytePerBlockOnStage,
                                    int                                      WithVerify,
                                    int                                      RemoveStageFiles,
                                    const char                            *  SuffixListStart,
                                    size_t                                   SuffixListLength);
    ~tcn35c_DBMStageBackupController();

    void BeginBackupOperation();
    void EndBackupOperation();

    tcn00_Error RecycleForRepeat(char                               *  TheReplyBuffer, 
                                 int                                *  TheReplyBufferLength,
                                 const tcn003_Medium                *  Medium,
                                 const tcn003_MediaGroup            *  MediaGroup,
                                 int                                   WithVerify,
                                 int                                   RemoveStageFiles);

    tcn00_Error GiveLastError();
    int         GiveSQLResult();

  private:
    int  Prepare();
    int  StartDBAction();
    int  DBIsReady();
    int  DBReplyAvailable(bool OutputMessagesForNonEvents=true);
    void ReceiveBackupRestoreRequestReplyFromDB();
    int  DBMayBeUsingLogDevspaceForLogRestore();
    int  DBActionWasSuccessful();
    void SignalEndToDB(int elapsedTimeout);
    void TerminateDBRequest();
    void WriteHistoryFile();
    void DBPostOperation();
    void FillReplyBuffer();
    void CleanUp();
    void CleanUpFinally();

    int DetermineFilesInDirectory(ni31_List<StageFileInfo *> & ListOfFilesFound,//ni31_List<char *> & ListOfFilesFound,
                                  const char        * NameOfDirectory,
                                  const char        * FileNameStart,
                                  const char        * AbsoluteNameWitoutVersion);

    bool determineLabels(
        ni31_List<StageFileInfo *> & listOfFilesFound);

    tcn35e_DBMBackupControllerLogWriter * DBMLogWriter;
    tcn35e_ConfigurationVariableHandler * ConfigurationVariableHandler;

    char * ReplyData;
    int  * ReplyLen;

    tcn003_Medium Stage;
    size_t        m_BytesPerBlockOnStage;

    ni31_List<StageFileInfo *>       StageFiles;
    int                              ReuseStageFiles;
    int                              NumberOfProcessedStageFiles;
    int                              NumberOfDeletedStageFiles;
    int                              NumberOfUnprocessedStageFiles;
    char                           * BytesProcessed;
    ni31_ListElem<StageFileInfo *> * CurrentStageFile;
    cn35c_SuffixList                 ListOfWishedSuffixes;
    int                              SuffixesWereWished;

    tni33_ArgumentList         Pipe2FileArguments;
    tni34_AFile *              Pipe2FileOutput;
    tni34_AFile *              Pipe2FileError;
    tni33_ABackgroundProcess * Pipe2FileProcess;
    enum APipe2FileState{NotStarted, Running, Ended};
    APipe2FileState            Pipe2FileState;

    int RemoveStageFilesFlag;
    int VerifyStageFilesFlag;

    tcn35e_DBMBackupControllerErrorHandler * ErrorHandlerAsDBMErrorHandler;

    int StartBackupOfNextStageFile(int & IsFirstFile, tcn35d_BackupInfo::ADataDirection DataDirection);
    int BackupOrVerifyStageFiles(tcn35d_BackupInfo::ADataDirection DataDirection);

    void PrintBackupAnswer(int NoAction=0, int IsFinal=0);
    void CheckForErrors(int IsFinal=0);
    int  RemoveStageFiles();
    int  MovePipe2FileOutputToLogFile();
    int  MoveFileToLogFile(tni34_AFile & TheFileToMove, const char * FileDescription);

    int HistoryEntryMatchesStageFileName(const char *  HistoryFileName,
                                         const char *  RunDirectory,
                                         const char *  AbsoluteStageFileName,
                                         size_t     & FileNameLengthWithOutVersion);
};

// -----------------------------------------------------------------------------
// class tcn35c_TheDBMStageBackupControllerFactory
// -----------------------------------------------------------------------------

class tcn35c_TheDBMStageBackupControllerFactory: public tcn35e_TheDBMBackupControllerFactory
{
  public:
    tcn00_Error ConstructADBMStageBackupController(tcn35c_DBMStageBackupController    *& TheNewBackupController,
                                                   char                               *  TheReplyBuffer, 
                                                   int                                *  TheReplyBufferLength,
                                                   const tcn003_Medium                *  Medium,
                                                   const tcn003_MediaGroup            *  MediaGroup,
                                                   const tcn003_Medium                *  Stage,
                                                   const tcn003_MediaGroup            *  StageGroup,
                                                   const char                         *  TheDBName,
                                                   int                                   WithVerify,
                                                   int                                   RemoveStageFiles,
                                                   const char                         *  SuffixListStart,
                                                   size_t                                SuffixListLength);
};

extern tcn35c_TheDBMStageBackupControllerFactory TheDBMStageBackupControllerFactory_cn35c;

#endif
