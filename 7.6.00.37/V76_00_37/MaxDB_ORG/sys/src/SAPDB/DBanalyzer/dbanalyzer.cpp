/*!
  -----------------------------------------------------------------------------
  module: dbanalyzer.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: DBanalyzer - The performance analyse tool for SAPDB
  description:  dbanalyzer main function
  version:      7.4 and higher 
  -----------------------------------------------------------------------------

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




  -----------------------------------------------------------------------------
*/

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>

#include "SAPDB/ToolsCommon/Tools_System.hpp"
#include "SAPDB/ToolsCommon/Tools_PipeCall.hpp"

#include "SAPDB/DBanalyzer/dbanalyzer.hpp"
#include "SAPDB/DBanalyzer/arguments.hpp"
#include "SAPDB/DBanalyzer/configuration.hpp"
#include "SAPDB/DBanalyzer/error.hpp"
#include "SAPDB/DBanalyzer/database.hpp"
#include "SAPDB/DBanalyzer/snapshot.hpp"
#include "SAPDB/DBanalyzer/protocol.hpp"

#define DBA_DEFAULT_INTERVAL 900

#define DBA_CONFIG_FILE "dbanalyzer.cfg"
#define DBA_OUTPUT_DIR  "analyzer"

#define REMOVE_SESSION_DATA        true
#define DO_NOT_REMOVE_SESSION_DATA false

DBA_arguments     * pArguments = NULL;
DBA_configuration * pConfiguration = NULL;
DBA_database      * pDatabase = NULL;
DBA_protocol      * pProtocol = NULL;

DBA_error gError;

//
// abortHandler
//
extern "C" void abortHandler(int sig) {

  switch (sig) {
    case SIGINT:
    case SIGTERM:
    case SIGABRT:
      if (pDatabase != NULL) {
        pDatabase->Disconnect();
      } // end if
      if (pProtocol != NULL) {
        pProtocol->RemoveSessionData();
      } // end if
      exit(0);
      break;

    default:
      break;
  } // end switch
} // end abortHandler

//
// disconnect
//
void disconnect(bool bWithProtocols) {
  if (pDatabase != NULL) {
    pDatabase->Disconnect();
  } // end if
  if (pProtocol != NULL && bWithProtocols) {
    pProtocol->RemoveSessionData();
  } // end if
} // disconnect
//
// main
//
int main (int argcIn, char *argvIn[])
{

  // some values 
  int        nRC             = 0;
  int        argc            = 0; 
  char **    argv            = NULL;
  DBA_ulong  nCount          = 0;
  DBA_ulong  nNumber         = 0;
  DBA_uint   nConfigInterval = DBA_DEFAULT_INTERVAL;
  DBA_string sDuration;
  time_t     tStartSnapshot  = 0;
  time_t     tStart          = 0;
  time_t     tLast           = 0;
  time_t     tSnapshot       = 0;
  time_t     tLastSnapshot   = 0;
  time_t     tWakeup         = 0;
  time_t     tAct            = 0;
  DBA_uint   nConsole        = 0;
  DBA_bool   bDisconnected   = false;
  DBA_bool   bStartup        = false;

  // read arguments from a pipe
  nRC = Tools_PipeCall::ReadArguments(argcIn, argvIn, argc, argv, Tools_PipeCall::ReadXUser, "w");
  if(nRC != 0) {
    gError.SetError(DBA_ERR_ARGS);
    gError.SetExtText(Tools_PipeCall::ErrorReason());
    gError.ShowErrorMsg(pProtocol);
    disconnect(DO_NOT_REMOVE_SESSION_DATA); return 1;
  } // end if

  // create argument object and parse arguments
  pArguments = new DBA_arguments(argc, argv);
  if (!pArguments) {
    gError.SetError(DBA_ERR_NOMEM);
    gError.ShowErrorMsg(pProtocol);
    disconnect(DO_NOT_REMOVE_SESSION_DATA); return 1;
  }

  // check program arguments
  if (!pArguments->IsOK()) {
    gError.ShowErrorMsg(pProtocol);
    disconnect(DO_NOT_REMOVE_SESSION_DATA); return 1;
  }

  // check for reconnect argument
  if (pArguments->GetReconnect()) {
    gError.SetError(DBA_ERR_RECONNECT);
    gError.ShowErrorMsg(pProtocol);
    disconnect(DO_NOT_REMOVE_SESSION_DATA); return 1;
  } // end if


  // terminate after version info
  if (pArguments->GetVersion()) {
    disconnect(DO_NOT_REMOVE_SESSION_DATA); return 1;
  }    

  // show usage infos
  if (pArguments->GetUsage()) {
    pArguments->Usage();
    disconnect(DO_NOT_REMOVE_SESSION_DATA); return 1;
  }    

  // create database object
  pDatabase = new DBA_database(pArguments->GetServer(),
                               pArguments->GetDatabase(),
                               pArguments->GetUser(),
                               pArguments->GetPassword());

  if (!pDatabase) {
    gError.SetError(DBA_ERR_NOMEM);
    gError.ShowErrorMsg(pProtocol);
    disconnect(DO_NOT_REMOVE_SESSION_DATA); return 1;
  }

  // connect to database
  if (!pDatabase->Connect()) {
    gError.ShowErrorMsg(pProtocol);
    disconnect(DO_NOT_REMOVE_SESSION_DATA); return 1;
  }

  // lock the system tables
  if (!pDatabase->LockSystemTables()) {
    gError.ShowErrorMsg(pProtocol);
    disconnect(DO_NOT_REMOVE_SESSION_DATA); return 1;
  } // end if

  // read basic infos about the database session into the databas eobject
  if (!pDatabase->GetInfos()) {
    gError.ShowErrorMsg(pProtocol);
    disconnect(DO_NOT_REMOVE_SESSION_DATA); return 1;
  } // end if

  // check/create ouput directory
  DBA_string sOutputDir;
  DBA_string sRunDir;
  if (pArguments->GetOutputDir().Empty()) {
    sRunDir = pDatabase->GetRundirectory();
    DBA_string sOutDir = DBA_string(DBA_OUTPUT_DIR);
    sOutputDir = ToolsSys_joinpath(sRunDir.CharPtr(), sOutDir.CharPtr());
  } else {
    sOutputDir = pArguments->GetOutputDir();
  }
  if (!ToolsSys_createdir(sOutputDir.CharPtr())) {
    gError.SetError(DBA_ERR_CREATEDIR, sOutputDir);
    gError.ShowErrorMsg(pProtocol);
    disconnect(DO_NOT_REMOVE_SESSION_DATA); return 1;
  }

  // check for a console argument
  if (!pArguments->GetConsole().Empty()) {
    nConsole = atoi(pArguments->GetConsole().CharPtr());
    if (nConsole == 0) {
      nConsole = 1;
    } // end if
  } // end if

  // create the protocol object
  sOutputDir = ToolsSys_fullpath(sOutputDir.CharPtr());
  pDatabase->GetTimestamp(tAct);
  pProtocol = new DBA_protocol(sOutputDir, nConsole, pArguments->GetIgnore(), tAct);
  if (!pProtocol) {
    gError.SetError(DBA_ERR_NOMEM);
    gError.ShowErrorMsg(pProtocol);
    disconnect(DO_NOT_REMOVE_SESSION_DATA); return 1;
  } else {
    if (!pArguments->GetNoLogo()) {
      printf("Used protocol directory: %s\n", sOutputDir.CharPtr());
    } // end if

  } // end if

  // get the dbanalyzer state
  if (pArguments->GetState()) {
    printf("\n");
    pDatabase->CheckAnalyzer(pProtocol->GetSid(), pProtocol->GetPid(), sOutputDir);

    if ((gError.GetError() == DBA_ERR_ACTIVE   ) || 
        (gError.GetError() == DBA_ERR_NOTACTIVE)    ) {
      gError.ShowErrorMsg(NULL, true);
//      if (gError.GetError() == DBA_ERR_ACTIVE) {
        printf(pProtocol->GetInfo().CharPtr());
//      } // end if
      disconnect(DO_NOT_REMOVE_SESSION_DATA); return 0;
    } else {
      gError.ShowErrorMsg(pProtocol);
      disconnect(DO_NOT_REMOVE_SESSION_DATA); return 1;
    } // end if
  } // end if

  DBA_string sEvent = sOutputDir;
  DBA_string sSid;
  DBA_string sPid;
  sEvent.Append("SAPDBDbanalyzer");
  DBA_string::BasisElementIndex nFind;
  while ((nFind = sEvent.Find("\\")) != DBA_string::NPos) {
    sEvent.Replace(nFind, 1, "x");
  } // end while
  while ((nFind = sEvent.Find(":")) != DBA_string::NPos) {
    sEvent.Replace(nFind, 1, "x");
  } // end while
  DBA_string sTemp = sEvent;
  sEvent = "Global\\";
  sEvent.Append(sTemp);

  // stops the dbanalyzer
  if (pArguments->GetStop()) {
    printf("\n");
    if (!pDatabase->CheckAnalyzer(pProtocol->GetSid(), pProtocol->GetPid(), sOutputDir)) {

      // unlock dbanlock table
      pDatabase->Commit();

#ifdef _WIN32
      sSid.ConvertFromInt(pProtocol->GetSid());
      sPid.ConvertFromInt(pProtocol->GetPid());
      sEvent.Append(sSid);
      sEvent.Append(sPid);

      HANDLE hEvent = OpenEvent(EVENT_MODIFY_STATE, FALSE, sEvent.CharPtr());
      if (hEvent != 0) {
        if (!SetEvent(hEvent)) {
          gError.SetError(DBA_ERR_KILL);
          gError.ShowErrorMsg(pProtocol);
          disconnect(DO_NOT_REMOVE_SESSION_DATA); return 1;
        } // end if
      } // end if
#else
      DBA_uint nPid = pProtocol->GetPid();
      if (nPid != 0) {
        if (!ToolsSys_killprocess(nPid)) {
          gError.SetError(DBA_ERR_KILL);
          gError.ShowErrorMsg(pProtocol);
          disconnect(DO_NOT_REMOVE_SESSION_DATA); return 1;
        } // end if
      } // end if
#endif
      // wait for other dbanalyzer
      int nCounter = 0;
      do {
          if (nCounter == 3) {
              break;
          } // end if
          ToolsSys_sleep((DBA_uint) 6);
          ++nCounter;
      } while (!pDatabase->CheckAnalyzer(pProtocol->GetSid(), pProtocol->GetPid(), sOutputDir));

      disconnect(DO_NOT_REMOVE_SESSION_DATA); return 0;
    } else {
      gError.ShowErrorMsg(pProtocol);
      disconnect(DO_NOT_REMOVE_SESSION_DATA); return 1;
    } // end if
  } // end if

  // create configuration object
  pConfiguration = new DBA_configuration();
  if (!pConfiguration) {
    gError.SetError(DBA_ERR_NOMEM);
    gError.ShowErrorMsg(pProtocol);
    disconnect(DO_NOT_REMOVE_SESSION_DATA); return 1;
  }

  // open and read the configuration file 
  if (!pConfiguration->Open(pArguments->GetConfigFile(),
                            pDatabase->GetMajor(),
                            pDatabase->GetMinor(),
                            pDatabase->GetLevel(),
                            pDatabase->GetBuild(),
                            pDatabase->GetInstanceType())) {
    gError.ShowErrorMsg(pProtocol);
    disconnect(DO_NOT_REMOVE_SESSION_DATA); return 1;
  } else {
    if (!pArguments->GetNoLogo()) {
      printf("Used configuration file: %s\n\n", pConfiguration->GetConfigFile().CharPtr());
    } // end if
    pDatabase->SetIgnoreErrors(pConfiguration->GetSetting(DBA_CNFTXT_IER));
  } // end if

  // init protocol files
  if (!pConfiguration->InitProtocolFiles(pProtocol)) {
    gError.ShowErrorMsg(pProtocol);
    disconnect(DO_NOT_REMOVE_SESSION_DATA); return 1;
  }

  // delete files and exit
  if (pArguments->GetDeleteFlag()) {
    if (!pProtocol->DeleteFiles(pArguments->GetDeleteDate())) {
      gError.ShowErrorMsg(pProtocol);
      disconnect(DO_NOT_REMOVE_SESSION_DATA); return 1;
    } // end if
    disconnect(DO_NOT_REMOVE_SESSION_DATA); return 0;
  } // end if

  // check the session id
  if (!pDatabase->CheckAnalyzer(pProtocol->GetSid(), pProtocol->GetPid(), sOutputDir)) {
    gError.ShowErrorMsg(pProtocol);
    disconnect(DO_NOT_REMOVE_SESSION_DATA); return 1;
  } // end if

  // overwrite interval constant 
  if (!pArguments->GetInterval().Empty()) {
    if (!pConfiguration->SetParameterValue("INTERVAL", pArguments->GetInterval())) {
      gError.ShowErrorMsg(pProtocol);
      disconnect(REMOVE_SESSION_DATA); return 1;
    }
  }
  
  // set the vars
  nCount          = 0;
  nConfigInterval = atoi(pConfiguration->GetParameterValue("INTERVAL").CharPtr());

  // check and save the analyzer information
  if (!pProtocol->CheckAndSaveInfo(pArguments->GetServer(),
                                   pArguments->GetDatabase(),
                                   pDatabase->GetRundirectory(),
                                   pConfiguration->GetConfigFile(),
                                   nConfigInterval,
                                   pArguments->GetReconnect(),
                                   pDatabase->GetSid(),
                                   pDatabase->GetPid(),
                                   pDatabase->GetDriver())) {
    gError.ShowErrorMsg(pProtocol);
    disconnect(DO_NOT_REMOVE_SESSION_DATA); return 1;
  } // end if

  // parse the expressions in the configuration file
  if (!pConfiguration->ParseExpressions()) {
    gError.ShowErrorMsg(pProtocol);
    disconnect(REMOVE_SESSION_DATA); return 1;
  } // end if

  // create specified views 
  if (!pConfiguration->InitViews(pDatabase)) {
    gError.ShowErrorMsg(pProtocol);
    disconnect(REMOVE_SESSION_DATA); return 1;
  }

  // get environment values 
  if (!pConfiguration->InitEnvironment(pDatabase)) {
    gError.ShowErrorMsg(pProtocol);
    disconnect(REMOVE_SESSION_DATA); return 1;
  }

  // get the start of execution time
  tStartSnapshot = time(&tStartSnapshot);

  // get the first snapshot
  if (!pConfiguration->GetSnapshot(pDatabase, tSnapshot, bStartup)) {
      gError.ShowErrorMsg(pProtocol);
      disconnect(REMOVE_SESSION_DATA); return 1;
  } // end if

  // get the start time
  tStart = time(&tStart);

  // set the snapshot execution time
  sDuration.ConvertFromInt((int) (tStart - tStartSnapshot));
  if (!pConfiguration->SetParameterValue("SNAPSHOTDURATION", sDuration)) {
    gError.ShowErrorMsg(pProtocol);
    disconnect(REMOVE_SESSION_DATA); return 1;
  } // end if
    
  // make the first calculation of parameters
  if (!pConfiguration->CalculateParameters(true)) {
    gError.ShowErrorMsg(pProtocol);
    disconnect(REMOVE_SESSION_DATA); return 1;
  } // end if

  // make the first calculation of arrays
  if (!pConfiguration->CalculateArrays(true)) {
    gError.ShowErrorMsg(pProtocol);
    disconnect(REMOVE_SESSION_DATA); return 1;
  } // end if

  // make the first calculation of monitors ("startup")
  if (!pConfiguration->CalculateMonitors(true)) {
    gError.ShowErrorMsg(pProtocol);
    disconnect(REMOVE_SESSION_DATA); return 1;
  } // end if

  // write the monitors to the protocol files
  if (!pConfiguration->ProtocolHistory(pProtocol, nCount, tSnapshot, true)) {
    gError.ShowErrorMsg(pProtocol);
    disconnect(REMOVE_SESSION_DATA); return 1;
  } // end if

  // check for a number argument
  if (!pArguments->GetNumber().Empty()) {
    nNumber = atol(pArguments->GetNumber().CharPtr());
  } // end if

#ifdef _WIN32
  sSid.ConvertFromInt(pProtocol->GetSid());
  sPid.ConvertFromInt(pProtocol->GetPid());
  sEvent.Append(sSid);
  sEvent.Append(sPid);

  SECURITY_DESCRIPTOR oSD;
  SECURITY_ATTRIBUTES oSA;
  HANDLE              hEvent;
  if (InitializeSecurityDescriptor(&oSD, SECURITY_DESCRIPTOR_REVISION))  {  
    if (SetSecurityDescriptorDacl(&oSD, TRUE, NULL, FALSE)) {  
      oSA.nLength = sizeof (SECURITY_ATTRIBUTES);
      oSA.lpSecurityDescriptor = &oSD;
      oSA.bInheritHandle = FALSE;
      hEvent = CreateEvent(&oSA, FALSE, FALSE, sEvent.CharPtr());
    } else {
      hEvent = CreateEvent(NULL, FALSE, FALSE, sEvent.CharPtr());
    } // end if
  } else {
    hEvent = CreateEvent(NULL, FALSE, FALSE, sEvent.CharPtr());
  } // end if
#endif

  // now we are here and can unlock the system tables by a commit
  if (!pDatabase->Commit()) {
    gError.ShowErrorMsg(pProtocol);
    disconnect(REMOVE_SESSION_DATA); return 1;
  } // end if

  // the looooooooooop
  bool bLoop = true; // use variable to avoid compiler warnings
  while (bLoop) {

    ++nCount;

    // accept signals
    signal(SIGINT,   abortHandler);
    signal(SIGTERM,  abortHandler);
    signal(SIGABRT,  abortHandler);

#ifdef _WIN32
    if (WaitForSingleObject(hEvent, 0) == WAIT_OBJECT_0) {
      abortHandler(SIGINT);
    } // end if
#endif

    // start the timer.
    tWakeup = tStart + nConfigInterval;
    tAct    = time(&tAct);
    tWakeup = (tAct + 5) > tWakeup ? tAct + 5 : tWakeup;
    while (tAct < tWakeup) {
#ifdef _WIN32
      if (WaitForSingleObject(hEvent, 0) == WAIT_OBJECT_0) {
        abortHandler(SIGINT);
      } // end if
#endif
      ToolsSys_sleep((DBA_uint) 5);
      // check the session id
      if (!pDatabase->CheckSession()) {
          // session down or new session ID by implicit reconnect
          gError.ShowErrorMsg(pProtocol);
          disconnect(REMOVE_SESSION_DATA); return 1;
      } // end if
      tAct    = time(&tAct);
    } // end while


#ifdef _WIN32
    if (WaitForSingleObject(hEvent, 0) == WAIT_OBJECT_0) {
      abortHandler(SIGINT);
    } // end if
#endif

    // ignore signals
    signal(SIGINT,   SIG_IGN);
    signal(SIGTERM,  SIG_IGN);
    signal(SIGABRT,  SIG_IGN);

    // lock the system tables
    if (!pDatabase->LockSystemTables()) {
      gError.ShowErrorMsg(pProtocol);
      disconnect(REMOVE_SESSION_DATA); return 1;
    } // end if

    // get the start of execution time
    tStartSnapshot = time(&tStartSnapshot);

    // get the next snapshot
    tLastSnapshot = tSnapshot;
    if (!pConfiguration->GetSnapshot(pDatabase, tSnapshot, bStartup)) {
      if (!gError.GetIgnore()) { 
          if (gError.GetNativeError() == -4004) {
              // create specified views 
              if (!pConfiguration->InitViews(pDatabase)) {
                  gError.ShowErrorMsg(pProtocol);
                  disconnect(REMOVE_SESSION_DATA); return 1;
              } else {
                  if (!pConfiguration->GetSnapshot(pDatabase, tSnapshot, bStartup)) {
                      gError.ShowErrorMsg(pProtocol);
                      disconnect(REMOVE_SESSION_DATA); return 1;
                  } // end if
              } // end if
          } else {
              gError.ShowErrorMsg(pProtocol);
              disconnect(REMOVE_SESSION_DATA); return 1;
          } // end if
      } // end if
    } // end if

    // make the time stamp
    tLast = tStart;
    tStart = time(&tStart);

    if (!gError.GetIgnore()) { 

      // update INTERVAL value
      sDuration.ConvertFromInt((int) (tStart - tLast));
      pConfiguration->SetParameterValue("INTERVAL", sDuration);

      // set the snapshot execution time
      sDuration.ConvertFromInt((int) (tStart - tStartSnapshot));
      pConfiguration->SetParameterValue("SNAPSHOTDURATION", sDuration);

      // calculate parameters 
      if (!pConfiguration->CalculateParameters(bStartup)) {
        gError.ShowErrorMsg(pProtocol);
        disconnect(REMOVE_SESSION_DATA); return 1;
      } // end if

      // calculate arrays
      if (!pConfiguration->CalculateArrays(bStartup)) {
        gError.ShowErrorMsg(pProtocol);
        disconnect(REMOVE_SESSION_DATA); return 1;
      } // end if

      // calculate monitors
      if (!pConfiguration->CalculateMonitors(bStartup)) {
        gError.ShowErrorMsg(pProtocol);
        disconnect(REMOVE_SESSION_DATA); return 1;
      } // end if

      // write the parameters to the protocol files
      pDatabase->GetTimestamp(tAct);
      if (!pConfiguration->ProtocolParameters(pProtocol, nCount, tSnapshot, tLastSnapshot, tAct)) {
        gError.ShowErrorMsg(pProtocol);
        disconnect(REMOVE_SESSION_DATA); return 1;
      } // end if

      // write the monitors to the protocol files
      if (!pConfiguration->ProtocolHistory(pProtocol, nCount, tSnapshot, bStartup)) {
        gError.ShowErrorMsg(pProtocol);
        disconnect(REMOVE_SESSION_DATA); return 1;
      } // end if
      if (bStartup) {
        if (!pConfiguration->ProtocolHistory(pProtocol, nCount, tSnapshot, false)) {
          gError.ShowErrorMsg(pProtocol);
          disconnect(REMOVE_SESSION_DATA); return 1;
        } // end if
      } // end if
    
    } else {
      pProtocol->SetHistoryStatus(nCount, tSnapshot); 
      if (gError.GetIgnore()) {
        pProtocol->SetHistoryIgnore();
        pProtocol->WriteHistory(); 
        gError.ShowErrorMsg(pProtocol, true);
      } // end if
    }

    // now we are here and can unlock the system tables by a commit
    if (!pDatabase->Commit()) {
      gError.ShowErrorMsg(pProtocol);
      disconnect(REMOVE_SESSION_DATA); return 1;
    } // end if

    // check the number of analyses
    if (nNumber != 0) {
      if (nCount == nNumber) {
        break;
      }
    }
  } // end while

  disconnect(REMOVE_SESSION_DATA); return 0;
} // end main
