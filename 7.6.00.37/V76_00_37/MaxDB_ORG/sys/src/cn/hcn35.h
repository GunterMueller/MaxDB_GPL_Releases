/*!
    \file    hcn35.h
    \author  TiloH
    \ingroup cn layer
    \brief   connection between dbmsrv and abstract external backup tool 

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

#if !defined (_HCN35_H_)
#define _HCN35_H_


// -----------------------------------------------------------------------------
// includes
// -----------------------------------------------------------------------------

#include "hcn35d.h"
#include "hni33.h"
#include "SAPDB/DBM/Srv/BackupTools/DBMSrvBTools_ToolEnum.hpp"


// -----------------------------------------------------------------------------
// constants
// -----------------------------------------------------------------------------

#define MAX_NUMBER_OF_TOOLPROCESSES_CN35 MAX_NUMBER_OF_DATAPIPES_CN35D


// -----------------------------------------------------------------------------
// class tcn35_BackupToolConnector
// -----------------------------------------------------------------------------

class tcn35_BackupToolConnector
{
  public:
    enum DataType{DataConsistent, DataInconsistent, PagesConsistent, PagesInconsistent, Log, DataExtract, TableExtract};
    enum EBIDStatus{NoEBID, EBIDUnknown, EBIDAvailable, EBIDUnavailable};

    tcn35_BackupToolConnector(tcn35d_BackupInfo                     *  BackupInfo,
                              tcn35d_DBBackupControllerLogWriter    *  TheLogWriterToUse,
                              tcn35d_DBBackupControllerErrorHandler *  TheErrorHandlerToUse,
                              tcn35d_ConfigurationVariableHandler   *  TheConfigurationVariableHandler,
                              char                                  *& TheExternalBackupIDList);
    virtual ~tcn35_BackupToolConnector();

    int SetExternalBackupIDList(const char * NewEBIDList);

    virtual const char * GiveToolName() const =0;
    virtual const char * GiveLongToolName() const =0;
    virtual const DBMSrvBTools_ToolEnum ToolEnum() const =0;

    virtual int CheckTool();
    virtual int CheckToolInstallation(int & IsInstalled)=0;
            int Prepare(int PrepareCompletly=0);
    virtual int PrepareFirst()=0;
    virtual int PrepareAgain()=0;
    virtual int LastMinutePreparation(int NumberOfCurrentCommandLine);
    virtual int StartTool();
    virtual int SignalEndToTool(int DBWasSuccessful);

    int  ToolIsRunning(bool beVerbose=true);
    int  ToolIsRunning(int & FinishedCompletely, bool beVerbose);
    int  ExistToolProcesses();
    void CancelTool();
    void TerminateTool();

    int MaxWaitingTimeForToolInWaitingPhase(bool forTool, bool actionWasSuccessful);

    virtual int ToolWasSuccessful(teo00_Uint8 DBBackupSize);
    virtual int ToolWasSuccessfulSoFar();
    virtual int CollectExternalBackupIDs(char * ExternalBackupID[MAX_NUMBER_OF_TOOLPROCESSES_CN35], int & NumberOfEBIDsFound)=0;
    virtual int PostOperationsAfterSuccess();
    virtual int MoveToolOutputToProtocolFile();
    virtual int CleanUpTool()=0;

    virtual int CheckPipes();

    virtual int SumOfToolExitCodes();

    virtual int PrepareToolOutputOfAvailableBackups();
    virtual int GiveNextAvailableBackup(int          & EBIDIsAvailable,
                                        const char * & EBID,
                                        size_t       & EBIDLength,
                                        tcn35d_BackupInfo::ADataType    & DataType,
                                        const char * & DateTimeString,
                                        int          & ReachedEnd);
    virtual int CloseToolOutputOfAvailableBackups();

    static int ConstructBackupToolConnector(tcn35_BackupToolConnector             *& rpDeterminedTool,
                                            DBMSrvBTools_ToolEnum                    BackupToolAsEnum,
                                            tcn35d_BackupInfo                     *  BackupInfo,
                                            tcn35d_DBBackupControllerLogWriter    *  TheLogWriterToUse,
                                            tcn35d_DBBackupControllerErrorHandler *  TheErrorHandlerToUse,
                                            tcn35d_ConfigurationVariableHandler   *  TheConfigurationVariableHandler,
                                            char                                  *  TheExternalBackupIDList=0);

  protected:
    tcn35d_BackupInfo                     * BackupInfo;
    tcn35d_DBBackupControllerLogWriter    * LogWriter;
    tcn35d_DBBackupControllerErrorHandler * ErrorHandler;
    tcn35d_ConfigurationVariableHandler   * ConfigurationVariableHandler;

    int                        NumberOfCommandLines;
    char *                     CommandLine[MAX_NUMBER_OF_TOOLPROCESSES_CN35];       // TODO: should be an array of strings
    tni33_ArgumentList         ArgumentList[MAX_NUMBER_OF_TOOLPROCESSES_CN35];
    int                        NumberOfRunningToolProcesses;
    tni33_ABackgroundProcess * ToolProcess[MAX_NUMBER_OF_TOOLPROCESSES_CN35];
    int                        ToolProcessWasRunning[MAX_NUMBER_OF_TOOLPROCESSES_CN35];
    tni34_AFile *              ToolProcessOutput[MAX_NUMBER_OF_TOOLPROCESSES_CN35];
    tni34_AFile *              ToolProcessError[MAX_NUMBER_OF_TOOLPROCESSES_CN35];

    char *                     TempDateTimeString;
    tni34_ATextFile *          ToolProcessOutputAsText;

    char *               ExternalBackupIDList;    //used for restore

    virtual int GiveInformationOfLine(const char                   *   Line,
                                      const char                   * & EBIDInLine,
                                      size_t                         & EBIDLength,
                                      tcn35d_BackupInfo::ADataType   & DataType,
                                      const char                   * & DateTimeString,
                                      int                            & LineContainsEBID)=0;

    int DetermineValueOfEnvironmentVariable(const char * const VariableName,
                                                  char * &     VariableValue,
                                            const char *       VariableComment,
                                            const char *       DBName,
                                                  int          ExportDefaultIfNotFound=1,                   //!< determines, what to do if nothing is found in dbm.cfg and in the environment
                                                  int          ExportConfigVariableIfNotInEnvironment=1);   //!< determines, what to do if something is found in dbm.cfg but not in the environment

    int PutEnvironmentVariable(const char * VariableName,
                               const char * VariableValue);

    int  Remove(const char *Name);
    void Sleep(int Seconds, bool beVerbose=true);

    void SetToolTimeoutSuccess(int NewToolTimeoutSuccess); //!< sets ToolTimeoutSuccess to a new value, if it has only the default value so far
    void SetToolTimeoutFailure(int NewToolTimeoutFailure); //!< sets ToolTimeoutFailure to a new value, if it has only the default value so far
    int  ToolTimeoutSuccessIsDefault() const;
    int  ToolTimeoutFailureIsDefault() const;
    int  GetToolTimeoutSuccess() const;
    int  GetToolTimeoutFailure() const;

  private:
    int MoveFileToProtocolFile(const char *FileDesignation, tni34_AFile & TheFileToMove);

    int ToolTimeoutSuccess;
    int ToolTimeoutSuccessIsDefaultFlag;
    int ToolTimeoutFailure;
    int ToolTimeoutFailureIsDefaultFlag;

    int KernelTimeoutSuccess; // kernel timeout - currently here in tool handling, because only tool handling knowns about the environment //TODO
    int KernelTimeoutFailure; // kernel timeout - currently here in tool handling, because only tool handling knowns about the environment //TODO

    int AlreadyPrepared;
};

#endif
