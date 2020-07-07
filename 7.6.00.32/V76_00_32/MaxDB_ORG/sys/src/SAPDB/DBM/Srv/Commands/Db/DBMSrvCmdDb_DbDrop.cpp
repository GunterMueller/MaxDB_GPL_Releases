/*!
    \file    DBMSrvCmdDb_DbDrop.cpp
    \author  MarcW
    \ingroup DBMServer commands
    \brief   definition of class for handling DBMServer
             command db_online

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

*/

#include "hcn20.h"
#include "hcn42.h"
#include "heo06.h"
#include "heo11.h"
#include "gsp09.h"

#include "DBM/Logging/DBMLog_Logger.hpp"
#include "DBM/Srv/Commands/Db/DBMSrvCmdDb_DbDrop.hpp"
#include "DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerStop.hpp"
#include "DBM/Srv/Commands/DBMSrvCmd_SilentDispatcherUsingCommand.hpp"
#include "DBM/Srv/SharedMemory/DBMSrvShM_ShmInfo.hpp"
#include "DBM/Srv/HSS/DBMSrvHSS_Nodes.hpp"
#include "DBM/Srv/KernelAccess/DBMSrvKnl_State.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_RTEError.hpp"
#include "RunTime/Diagnose/RTEDiag_ClientCrashHist.h"
#include "RunTime/RTE_DBRegister.hpp"
#include "RunTime/RTE_UserProfileContainer.hpp"
#include "ToolsCommon/Tools_System.hpp"

const char* DBMSrvCmdDb_DbDrop::WITHOUTFILES("WITHOUTFILES");

DBMSrvCmdDb_DbDrop::DBMSrvCmdDb_DbDrop()
: DBMSrv_Command( DBMSrv_Command::KeyUsingCommandFactory, false )
{
    m_longHelp=
        "@command db_drop With this command you delete a database instance. "
            "requires a database name."
        "@preconditions The database is in the operational state OFFLINE. You have the server authorization InstallMgm."
        "@syntax db_drop [WITHOUTFILES]"
        "@param WITHOUTFILES If you specify the option WITHOUTFILES for this command, the database "
            "files on the host are retained. If you do not specify this option, the files for the "
            "database instance are also deleted."
        "@reply OK<NL>";
}

tcn00_Error DBMSrvCmdDb_DbDrop::run(
    VControlDataT* vcontrol,
    CommandT* command,
    DBMSrv_Reply& theReply) {
        
    tcn00_Error out(OK_CN00);
    bool withFiles(true);

    // check the argument
    if(command->oArguments.getCount() == 1 ) {
        Tools_DynamicUTF8String withoutFiles(command->oArguments.getValue(1));
        if( withoutFiles.Trim().ToUpper() != WITHOUTFILES )
            return theReply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_PARAM));
        else
            withFiles = false;
    }
    
    bool dropLogVolume(!vcontrol->pHSSNodes->Exists(RTE_ISystem::Instance().GetLocalNodeName()));
    return doDrop( vcontrol->dbname, vcontrol->dbroot, withFiles, dropLogVolume, theReply, false );
}


tcn00_Error DBMSrvCmdDb_DbDrop::doDrop(
    tsp00_DbNamec         szDbName,
    const tsp00_Pathc     szDbRoot,
    const bool            bWithFiles,
    const bool            bDeleteLog,
    DBMSrv_Reply&         reply,
    bool                  bServiceDB) {

    tcn00_Error          nFuncReturn = OK_CN00;
    tcn002_XpValueString szRunDir;
    tsp9_rte_xerror      xError;
    tsp01_RteError       rteError;
    int                  nParam(0);
    int                  nNumber(0);
    tsp00_XpKeyTypec     szParameter;
    tcn002_XpValueString szDevFile;
    tcn002_XpValueString szDevType;
    tsp05_RteFileError   fileErr;
    DBMSrvMsg_Error      oParamGetMsg;


    tcn00_DevspaceParams aParamTable[] = XP_DEVSPACE_PARAMS_CN00;

    // check kernel state
    DBMSrvKnl_State::StateValue aState(DBMSrvKnl_State::StateError);
    aState = DBMSrvKnl_State::State(szDbName);
    if (aState == DBMSrvKnl_State::StateError)
        return reply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_STATE));
    else if (aState != DBMSrvKnl_State::StateOffline)
        return reply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_DBRUN));

    if (!bServiceDB) {
        // stop scheduler
        if( cn20XParamGetValue(szDbName, PAN_RUNDIR, szRunDir, oParamGetMsg) ) {
            tsp05_RteFileInfo schedFinfo;
            tsp05_RteFileError schedFerr;
            sqlfinfoc(szRunDir, &schedFinfo, &schedFerr);
            if( schedFinfo.sp5fi_exists && schedFerr.sp5fe_result == vf_ok ) {
                // only if a rundirectory can be retrieved and it exists

                // stop the scheduler
                CommandT cmdLineStopScheduler;
                cmdLineStopScheduler.setArgs("");
                cmdLineStopScheduler.nIndex = CommandT::InvalidIndex;
                DBMSrvCmdScd_SchedulerStop schedulerStop;
                nFuncReturn = schedulerStop.run(
                    cn00DBMServerData::vcontrol(),
                    &cmdLineStopScheduler,
                    reply);

                if( nFuncReturn != OK_CN00 )
                    return nFuncReturn;

                // stop the internal event dispatcher
                CommandT cmdLineStopDispatcher;
                if( bWithFiles ) 
                    cmdLineStopDispatcher.setArgs(DBMSrvCmd_SilentDispatcherUsingCommand::StopAndDelete);
                else
                    cmdLineStopDispatcher.setArgs(DBMSrvCmd_SilentDispatcherUsingCommand::StopAll);
                cmdLineStopDispatcher.nIndex = 1;
                DBMSrvCmd_SilentDispatcherUsingCommand
                    dispatcherStopAndDelete(DBMSrv_Command::KeyUnknownCommand);
                nFuncReturn = dispatcherStopAndDelete.run(
                    cn00DBMServerData::vcontrol(),
                    &cmdLineStopDispatcher,
                    reply);

                if( nFuncReturn != OK_CN00 )
                    return nFuncReturn;

            }
        } // end if
    } // end if

    /*
    // check whether we are the only DBM Server
    DBMSrvMsg_Error checkServersError;
    if (!waitForOtherServers(szDbName, checkServersError))
        return reply.startWithMessageList(checkServersError);
    */

    sqlxclear( szDbName, szDbRoot, &xError );

    //      sqlxunregisterdb (szDbName, csp9_any_pgm, &xError);

    Msg_List err;        
    RTE_ActiveInstallation * activeInstallation = (RTE_DBRegister::Instance()).GetActiveInstallationByDBName(szDbName,err);

    if(!activeInstallation->UnregisterDatabase(szDbName,err))
    {
        //        error!
    }
    delete activeInstallation;
    if(Msg_List::Error == err.Type()){
        nFuncReturn = reply.startWithMessageList(DBMSrvMsg_RTEError(err));
    } else if (bWithFiles) {

        // -- devspaces

        // look for DATA params
        DBMSrvMsg_Error error;
        int maxDataVolumes=0;   //-> maxDataVolumes is 0 in case of errors

        DBMSrvPar_Parameters * parameters=DBMSrvPar_Parameters::GetInstance(cn00DBMServerData::vcontrol()->dbname, true, error);

        if(0!=parameters)
        {
            DBMSrvPar_Parameter * maxDataVolumesParameter=parameters->Get(PAN_MAXDATADEVSPACES);

            if(0!=maxDataVolumesParameter)
                maxDataVolumes=atoi(maxDataVolumesParameter->szActiveValue.asCharp());

            delete parameters;
        }

        // change to rundir
        if (sqlchdirc((char *) szRunDir, &rteError)) {

            // iterate trough devspacetypes
            nParam = 0;
            while (aParamTable[nParam].nDevSpace != XPDevSpaceUnknown_ecn00) {
                nNumber = 0;

                if(aParamTable[nParam].nDevSpace == XPDevSpaceDat_ecn00 ||
                   aParamTable[nParam].nDevSpace == XPDevSpaceLog_ecn00 ||
                   aParamTable[nParam].nDevSpace == XPDevSpaceMLog_ecn00  )
                {
                    if (bDeleteLog || (aParamTable[nParam].nDevSpace != XPDevSpaceLog_ecn00)) {

                        // iterate to devspaces of one type
                        bool bLoop = true;
                        do {
                            oParamGetMsg.ClearMessageList();
                            nNumber++;

                            // look for devicename
                            sp77sprintf(szParameter, szParameter.size(), aParamTable[nParam].szName, nNumber);

                            if (cn20XParamGetValue(szDbName, szParameter, szDevFile, oParamGetMsg)) {
                                // look for devicetype
                                sp77sprintf(szParameter, szParameter.size(), aParamTable[nParam].szType, nNumber);

                                // delete device
                                if (cn20XParamGetValue(szDbName, szParameter, szDevType, oParamGetMsg)) {
                                    if (szDevType[0] == 'F') {
                                        sqlferasec(szDevFile, &fileErr);
                                    } // end if
                                } // end if
                            } else {
                                if(aParamTable[nParam].nDevSpace != XPDevSpaceDat_ecn00 || nNumber>=maxDataVolumes)
                                    bLoop = false;
                            } // end if

                        } while (bLoop);

                    } // end if
                }

                nParam++;
            } // end while

            sqlchdirc(szDbRoot.asCharp(), &rteError);
        } // end if

        // -- all other file
        cn42RemoveFiles(szDbName);

        // -- remove all Crash-Histories
        tcn002_XpValueString  szTmp;

        if (cn20XParamGetValue(szDbName, PAN_CRASH_HISTORY_PATH, szTmp, oParamGetMsg)) {
            tsp00_Pathc szCrashHistoryPath;
            szCrashHistoryPath.rawAssign(szTmp.asCharp());
            RTEDiag_DropAllCrashHist (szDbName.asCharp(), szCrashHistoryPath.asCharp());
        } //end if

        // -- xparam
        sqlXParamRemoveAll(szDbName, &rteError);

        // -- user profile
        err.ClearMessageList();
        RTE_UserProfileContainer Profile;
        Profile.Open(szDbName, false, err);
        if (!err.IsEmpty() ) {
            Profile.DeleteContainer(err);
        } // end if

        // -- rundir
        if (sqlchdirc((char *) szRunDir, &rteError)) {
            sqlremove_empty_dirc( "dbahist", &rteError);
            sqlremove_empty_dirc( "rtedump_dir", &rteError);
            sqlremove_empty_dirc( "analyzer", &rteError);
            sqlchdirc(szDbRoot.asCharp(), &rteError);
        } // end if
        sqlremove_empty_dirc(szRunDir, &rteError);

        // remove shared memory files
        DBMSrvMsg_Error shmDeleteError;
        DBMSrvShM_ShmAdmin::TheAdmin().close(shmDeleteError);
        DBMSrvShM_ShmAdmin::TheAdmin().removePersistence(shmDeleteError);
    } // end if

    // generate answer
    if (nFuncReturn == OK_CN00) {
        setToDropped();
    } // end if

    return nFuncReturn;
}

 void DBMSrvCmdDb_DbDrop::setToDropped() {

    cn00DBMServerData::vcontrol()->bIsDropped = true;
    // before 
    DBMLog_Logger::getInstance().freeResources();
    tsp00_Pathc   szLogFilePath;
    DBMLog_LogFile* normalFile(NULL);
    DBMLog_LogFile* panicFile(NULL);
    tsp00_DbNamec emptyDbName;
    emptyDbName.Init();
    cn42GetFileName(
        emptyDbName,
        FGET_DBMSRV_CN42,
        szLogFilePath);
    normalFile = new DBMLog_LogFile(szLogFilePath.asCharp(), false);
    panicFile = new DBMLog_LogFile(szLogFilePath.asCharp(), true);
    DBMLog_Logger::getInstance().addFile(normalFile);
    DBMLog_Logger::getInstance().addFile(panicFile);

} // end DBMSrvCmdDb_DbDrop::setToDropped


bool DBMSrvCmdDb_DbDrop::waitForOtherServers(const char* dbName, DBMSrvMsg_Error & errOut) {

    // check whether we are the only DBM Server
    // try it 5 times to give disconnecting DBMServer a chance to complete their disconnect
    int             nTryCount=5;
    bool            bRc      = true;

    while (!(bRc = noOtherServersUp(dbName, errOut)) && nTryCount>0) {
        errOut.ClearMessageList();
        --nTryCount;
        ToolsSys_sleep(5-nTryCount);
    } // end while

    return bRc;

} // end DBMSrvCmdDb_DbDrop::waitForOtherServers

bool DBMSrvCmdDb_DbDrop::noOtherServersUp(const char* dbName, DBMSrvMsg_Error & errOut) {

    // make sure that there is no other DBMServer connected to this database
    DBMSrvShM_ShmInfo shmInfo;
    if( !shmInfo.initialize(dbName, errOut) )
        return false;

    DBMSrvShM_ParticipantSet playerSet;
    bool gotPlayers(shmInfo.getParticipantSet(playerSet, errOut));
    
    if( gotPlayers && playerSet.removeZombies(errOut) ) {
        // have only living processes in the list
        int dbmserverCount(0);
        Tools_Vector<RTE_OSPid> playerVector;
        Tools_Vector<RTE_OSPid>::iterator playerIterator = NULL;
        playerSet.getPIDsInto(playerVector);
        RTE_OSPid pid(0);
        Tools_DynamicUTF8String pidList;
        for( playerIterator=playerVector.begin(); playerIterator!=playerVector.end(); ++playerIterator ) {
            pid = *playerIterator;
            if( pid !=  RTE_OSPid(0) ) {
                ++dbmserverCount;
                if( pid != RTESys_ProcessID() ) {
                    char pidBuffer[128];
                    sprintf(pidBuffer, "%d, ", (int)pid);
                    pidList.Append(pidBuffer);
                }
            }
        }
        if( dbmserverCount != 1 ) {
            // remove trailing comma and blank
            if( pidList.Length() > 0 ) {
                pidList = pidList.SubStr(0, pidList.Length()-2);
            }
            // it is not only me
            errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_NOT_EXCLUSIVE,
	            Msg_Arg(SDBMSGTAG_DBMSRV_NOT_EXCLUSIVE__PIDLIST, pidList.CharPtr())));
        }
    }

    DBMSrvMsg_Error closeError;
    shmInfo.close(closeError);
    errOut.Overrule(closeError);

    return errOut.IsEmpty();
}
