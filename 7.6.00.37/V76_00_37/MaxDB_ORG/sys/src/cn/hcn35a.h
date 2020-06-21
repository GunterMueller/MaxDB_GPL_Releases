/*!
    \file    hcn35a.h
    \author  TiloH
    \ingroup cn layer
    \brief   base class for controling backups/replications to external
             backup tools

\if EMIT_LICENCE

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

\endif
*/

#if !defined (_HCN35A_H_)
#define _HCN35A_H_


// -----------------------------------------------------------------------------
// includes
// -----------------------------------------------------------------------------

#include "hcn35.h"
#include "hcn35d.h"


// -----------------------------------------------------------------------------
// class tcn35a_DBBackupController
// -----------------------------------------------------------------------------

class tcn35a_DBBackupController
{
  public:
    tcn35a_DBBackupController(tcn35_BackupToolConnector             * TheBackupToolConnector,
                              tcn35d_DBBackupControllerErrorHandler * TheErrorHandler,
                              tcn35d_DBBackupControllerLogWriter    * TheLogWriter,
                              tcn35d_BackupInfo                     * TheBackupInfo,
                              tcn35d_ConfigurationVariableHandler   * TheConfigurationVariableHandler);

    virtual ~tcn35a_DBBackupController();

    virtual void BeginBackupOperation();
    virtual int  BackupOperationIsRunning();
    virtual void EndBackupOperation();

    int AnErrorOccured();

  protected:
    virtual int  Prepare()=0;
    virtual int  StartDBAction()=0;
    virtual int  DBIsReady()=0;
    virtual int  DBReplyAvailable(bool OutputMessagesForNonEvents=true)=0;
    virtual void ReceiveBackupRestoreRequestReplyFromDB()=0;
    virtual int  DBMayBeUsingLogDevspaceForLogRestore()=0;
    virtual int  DBActionWasSuccessful()=0;
    virtual void SignalEndToDB(int elapsedTimeout)=0;
    virtual void TerminateDBRequest()=0;
    virtual void WriteHistoryFile()=0;
    virtual void DBPostOperation()=0;
    virtual void FillReplyBuffer()=0;

    tcn35_BackupToolConnector             * BackupToolConnector;
    tcn35d_DBBackupControllerErrorHandler * ErrorHandler;
    tcn35d_DBBackupControllerLogWriter    * LogWriter;
    tcn35d_BackupInfo                     * BackupInfo;
    tcn35d_ConfigurationVariableHandler   * ConfigurationVariableHandler;

    char * ExternalBackupID[MAX_NUMBER_OF_TOOLPROCESSES_CN35];

    enum ARequestState{NoRequestSend, RequestSend, CancelSendByDBMSrv, RequestReplied, CanceledByDBMSrv, TerminatedByDBMSrv};
    ARequestState RequestState();
    ARequestState StatusOfRequest;

    enum AToolState{NotStarted, Started, EndedPartially, Ended, KilledByDBMSrv};
    AToolState ToolState();
    AToolState StatusOfTool;

    enum AWaitingPhase{NoWaiting, WaitForDBAndBackupTool, WaitForDB, WaitForBackupTool, WaitForKilled};
    AWaitingPhase  CurrentWaitingPhase;

    void CleanUp();

    teo00_Uint8 DBBackupSize;
    
    int IsGoodFlag;

    void Sleep(int Seconds, bool OutputMessage=true);

    int CreatePipes();
    int CheckPipes();
    int RemovePipes();

  private:
    int  WaitForeverOnDB;
    long StartOfCurrentWaitingPhase;
    int  MaxWaitingTimeForTool();
	int  MaxWaitingTimeForDB();
    long OutputInterval;
    long LastOutput;

    void ReceiveReplyFromDB();
    void TerminateDBAction();

    int             NumberOfExistingPipes;
    tni34_APipe   * TheDataPipe[MAX_NUMBER_OF_DATAPIPES_CN35D];
};

#endif
