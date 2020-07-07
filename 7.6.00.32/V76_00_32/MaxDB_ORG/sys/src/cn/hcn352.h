/*!
    \file    hcn352.h
    \file    TiloH
    \ingroup cn layer
    \brief   connection between dbmsrv and external backup tool NetWorker

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

#if !defined (_HCN352_H_)
#define _HCN352_H_


// -----------------------------------------------------------------------------
// includes
// -----------------------------------------------------------------------------

#include "hni31.h"
#include "hcn35.h"
#include "hcn36.h"

#include "ToolsCommon/Time/ToolsTime_TimeFormat.hpp"
#include "ToolsCommon/Tools_Vector.hpp"


// -----------------------------------------------------------------------------
// class tcn352_NetWorkerConnection
// -----------------------------------------------------------------------------

class tcn352_LevelActionMapEntry;

class tcn352_NetWorkerConnector:public tcn35_BackupToolConnector
{
  public:
    tcn352_NetWorkerConnector(tcn35d_BackupInfo                     * TheBackupInfo,
                              tcn35d_DBBackupControllerLogWriter    * TheLogWriterToUse,
                              tcn35d_DBBackupControllerErrorHandler * TheErrorHandlerToUse,
                              tcn35d_ConfigurationVariableHandler   * TheConfigurationVariableHandler,
                              char                                  * TheExternalBackupIDList=0);
    ~tcn352_NetWorkerConnector();
    
    virtual const char * GiveToolName() const;
    virtual const char * GiveLongToolName() const;
    virtual const DBMSrvBTools_ToolEnum ToolEnum() const;

  private:
    int CheckToolInstallation(int & IsInstalled);
    int PrepareFirst();
    int PrepareAgain();
    int LastMinutePreparation(int NumberOfCurrentCommandLine);

    int CleanUpTool();
    int CollectExternalBackupIDs(char * ExternalBackupID[MAX_NUMBER_OF_TOOLPROCESSES_CN35], int & NumberOfEBIDsFound);
    int CheckPipes();
    int ConstructNameForPart(char *& Name, int NumberOfPipe);

    int GiveInformationOfLine(const char *                   Line,
                              const char *                 & EBIDInLine,
                              size_t                       & EBIDLength,
                              tcn35d_BackupInfo::ADataType & DataType,
                              const char *                 & DateTimeString,
                              int                          & LineContainsEBID);

    int BuildEBIDFromNSavetime(char       * & EBID,
                               const char *   StartOfNSavetime,
                               const char *   EndOfNSavetime);
    int ToolWasSuccessful(teo00_Uint8 DBBackupSize);
    int ToolWasSuccessfulSoFar();
    int AnalyzeNSROutputLine(const char   * FileName,
                             const char   * Line,
                             int            NumberOfPart,
                             int &          FoundCompletionMessage,
                             int &          FoundNSavetimeMessage,
                             teo00_Uint8  & BytesInCompletionMessage,
                             unsigned int & NumberOfErrorMessages);
    int FoundFileNameFollowedByWarning(const char * FileName,
                                       const char * NSROutputLine);
    int LineContainsCompletionMessage(int           NumberOfPart,
                                      const char *  Line,
                                      teo00_Uint8 & BytesInCompletionMessage);
    int CheckForNSavetimeMessage(const char * FileName,
                                 int          NumberOfPart,
                                 const char * Line,
                                 int        & ContainsNSavetime);


    void DeleteListOfErrorsToIgnore();

    static struct DataTypeToNSRLevelMapEntry
    {
        unsigned                        Index;
        tcn35d_BackupInfo::ADataType    DataType;
        const char                   *  NSRLevel;
    } DataTypeToNSRLevelMap[];

    unsigned int IndexAccordingToNSRFullSetting(unsigned CurrentIndex);
    const char * LevelForAction(tcn35d_BackupInfo::ADataType DataType,
                                int                          NumberOfPipe);
    int          DataTypeForLevelN(tcn35d_BackupInfo::ADataType & DataType,
                                   const char *                   Level,
                                   size_t                         LevelLength);


    char * PathOfNSR_ENVFile;
    char * NameOfNSR_HOST;
    char * NameOfNSR_POOL;
    char * NSR_EXPIREValue;

    char * PathOfNSRSave;
    char * PathOfNSRRecover;
    char * PathOfNSRMminfo;

    int    BuildBackupToolCall();   // TODO: move to base-class
    int    CreateNSRDirectivesFile();
    int    CreateNSRDirectivesFileWriteALine(tni34_ATextFile & DFile, const char * Line, const char *DFileName);
    int    CreateTempOutputFiles();

    long   StartSaveTime;
    long   EndSaveTime;

    int NSRUseDotNSRFile;                       //!< should a .nsr file be used or multiple directive files
    Tools_Vector<char *> DirectiveFileNames;    //!< vector containing names of used directives files

    enum LevelFullType {DataMigration, DataRecovery, DataMigrationAndDataRecovery};    
    LevelFullType LevelFullForBackupType;

    enum VerboseFlagForType {None, Save, Recover, SaveAndRecover};
    VerboseFlagForType VerboseFlagFor;

    enum NSREBIDTypeEnum {SSID, NSaveTime};
    NSREBIDTypeEnum     NSREBIDType;

    char *              TempEBID;
    static const char * NSaveTimeEBIDPrefix;
    static const char * NSaveTimeEBIDPartDelimiter;

    enum NSROutputResultType {NotChecked, FoundError, FoundNoError};
    NSROutputResultType NSROutputResult;
    NSROutputResultType ErrorInOutput[MAX_NUMBER_OF_TOOLPROCESSES_CN35];
    int                UseNSROutput;
    ni31_List<char *>  ListOfErrorsToIgnore;

    int NSRSaveInterval;

    char * NSRBackupName;

    int    NSRAvoidMMInfoBug;
    char * NSRAvoidMMInfoTimeBug;

    ToolsTime_TimeFormat ExplicitDateTimeFormat;
    ToolsTime_TimeFormat StandardDateTimeFormat2DigitYear;
    ToolsTime_TimeFormat StandardDateTimeFormat4DigitYear;
    ToolsTime_TimeFormat OutputDateTimeFormat;
    int                  UseExplicitDateTimeFormat;

    int AllocConstructAndCheckExe(char *& Path, const char * DirectoryPath, const char * Name, int & ExistsAndIsExecutable);

    enum NSRRoundUpValue {NoRoundup, FirstDecimal, Normal, LastInteger};
    NSRRoundUpValue Roundup;

    int    UseLLOptionForSave;
    char * NSRExternalBackupID[MAX_NUMBER_OF_TOOLPROCESSES_CN35];
    int    NSRNumberOfEBIDsFound;

    enum NSRClientNameUsage {UseForMminfo, UseForSaveAndMMInfo};
    char               * NSRClientName;
    NSRClientNameUsage   NSROptionUseClientFor;
    const char         * NodeNameToUse() const;

    bool m_NSRAvoidVSSBug;

    ni31_List<char *> m_NSRAdditionalSaveParameters;
    ni31_List<char *> m_NSRAdditionalMMInfoParameters;
    ni31_List<char *> m_NSRAdditionalRecoverParameters;
    void deleteListOfAdditionalSaveParameters(
            ni31_List<char *> & additionalParameterList);
    void deleteAllListsOfAdditionalSaveParameters();
    bool calculateAndStoreAdditionalParameters(
            const char *         additionalParameters,
            ni31_List<char *>  & additionalParameterList);
    bool addAdditionalParameters(
            tni33_ArgumentList      & destList,
            const ni31_List<char *> & additionalParameters);
};

#endif
