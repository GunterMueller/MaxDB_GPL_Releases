/*!
    \file    hcn353.h
    \author  TiloH
    \ingroup cn layer
    \brief   connection between dbmsrv and Backint for MaxDB conforming
             backup tools

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

#if !defined (_HCN353_H_)
#define _HCN353_H_


// -----------------------------------------------------------------------------
// includes
// -----------------------------------------------------------------------------

#include "hni31.h"
#include "hcn35.h"


// -----------------------------------------------------------------------------
// class tcn353_Backint2Connection
// -----------------------------------------------------------------------------

class tcn353_Backint2InputLine;

const int NumberOfBackupHistories_cn353=2;  // ==2, use the backup history file and the external backup history file

class tcn353_Backint2Connector:public tcn35_BackupToolConnector
{
  public:
    tcn353_Backint2Connector(tcn35d_BackupInfo                     * TheBackupInfo,
                             tcn35d_DBBackupControllerLogWriter    * TheLogWriterToUse,
                             tcn35d_DBBackupControllerErrorHandler * TheErrorHandlerToUse,
                             tcn35d_ConfigurationVariableHandler   * TheConfigurationVariableHandler,
                             char                                  * TheExternalBackupIDList=0);
    ~tcn353_Backint2Connector();

    virtual const char * GiveToolName() const;
    virtual const char * GiveLongToolName() const;
    virtual const DBMSrvBTools_ToolEnum ToolEnum() const;

    static int BuildAEBIDFrom(char * &     EBID,
                              const char * User,
                              const char * BID,
                              const char * Path);

  private:
    int CheckToolInstallation(int & IsInstalled);

    int PrepareFirst();
    int PrepareAgain();

    int DetermineAndCountInputEntries(int & NumberOfEntries);
    int RenameABackupHistoryFile(const char * NameOfBackupHistoryFile,
                                 int          NumberOfBackupHistoryFile);

    int BringHistoryFilesToFormerState();

    int AppendToBackintInputList(const char * TheName,
                                 int          IsAPipe,
                                 const char * BID=0,
                                 const char * Destination=0,
                                 const char * EndOfDestination=0,
                                 tcn35d_BackupInfo::ADataType DataType=tcn35d_BackupInfo::DataRecovery,
                                 const char * DateTime=0);
    void DeleteBackintInputList();

    int PostOperationsAfterSuccess();
    int CleanUpTool();
    int CollectExternalBackupIDs(char * ExternalBackupID[MAX_NUMBER_OF_TOOLPROCESSES_CN35], int & NumberOfEBIDsFound);

    int PrepareToolOutputOfAvailableBackups();
    int GiveNextAvailableBackup(int          & EBIDAvailable,
                                const char * & EBID,
                                size_t       & EBIDLength,
                                tcn35d_BackupInfo::ADataType    & DataType,
                                const char * & DateTimeString,
                                int          & ReachedEnd);
    int CloseToolOutputOfAvailableBackups();

    int GiveInformationOfLine(const char                   *   Line,
                              const char                   * & EBIDInLine,
                              size_t                         & EBIDLength,
                              tcn35d_BackupInfo::ADataType   & DataType,
                              const char                   * & DateTimeString,
                              int                            & LineContainsEBID);

    int BuildBackupToolCall();  // TODO: move to base-class

    int GetBIDAndNameFromLine(const char *   Line,
                              int          & ContainsABID,
                              const char * & BackupID,
                              size_t       & LengthOfBackupID,
                              const char * & Name,
                              size_t       & LengthOfName);

    int ToolWasSuccessful(teo00_Uint8 DBBackupSize);

    int BackupOrRestoreHistoryWithBackint();

    char * PathToEnvFile;

    char * BackintPath;
    char * BackintInFile;
    char * BackintOutFile;
    char * BackintErrorFile;
    char * BackintParamFile;

    int    NumberOfRequestedNames;


    ni31_List<tcn353_Backint2InputLine *> ListOfFilesProcessedByBackint;

    enum BackintDataTypeEnum{DataPipes, HistoryFiles};

    BackintDataTypeEnum BackintDataType;
    
    const int NumberOfBackupHistories; // ==2, use the backup history file and the external backup history file
    enum TypeBackupHistoryState{Unknown, NotExist, Exist, Recovered, Renamed, Moved};
    
    const char *           SuffixBackupHistory[NumberOfBackupHistories_cn353];         //TODO: new class containing all information abut one history file
    const char *           SuffixOfCopyOfBackupHistory[NumberOfBackupHistories_cn353];
    char *                 OriginalBackupHistoryName[NumberOfBackupHistories_cn353];
    char *                 OriginalBackupHistoryMovedTo[NumberOfBackupHistories_cn353];
    char *                 PathOfMigrationBackintHistory[NumberOfBackupHistories_cn353];
    TypeBackupHistoryState BackupHistoryState[NumberOfBackupHistories_cn353];

    const ni31_ListElem<tcn353_Backint2InputLine *> *InFileItem;
    char *                                           TempBID;

    const char * BI_BACKUPStringForActionString(tcn35d_BackupInfo::ADataType DataType);

    char * SourceDBName;

    int SecondBackupCall;

    enum CheckAvailabilityEnum
    {
        check,
        dontCheck
    };

    CheckAvailabilityEnum checkBackupAvailability;
};

#endif
