/*!
    @file           DBMCliEvtDisp_DispatcherMain.cpp
    @author         MarcW
    @brief          Event Dispatcher  - main function and class

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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

/* 
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include <stdio.h>
#include <signal.h>

#include "RunTime/RTE_ISystem.hpp"
#include "ToolsCommon/Tools_PipeCall.hpp"
#include "SAPDBCommon/Messages/SDBMsg_DBMEd.h"
#include "SAPDBCommon/Messages/SDBMsg_UniqueMessageNumber.hpp"
#include "DBM/Cli/Eventing/Dispatcher/DBMCliEvtDisp_Dispatcher.hpp"
#include "DBM/Cli/Eventing/Dispatcher/DBMCliEvtDisp_EventListener.hpp"
#include "DBM/Cli/Eventing/Dispatcher/DBMCliEvtDisp_MsgDumper.hpp"
#include "DBM/Cli/Eventing/Dispatcher/DBMCliEvtDisp_Permissions.hpp"
#include "DBM/Cli/DBMCli_Stuff.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_ErrorHandler.hpp"
#include "DBM/Srv/Process/DBMSrvProc_DeamonProcess.hpp"
#include "DBM/Srv/SharedMemory/DBMSrvShM_LineAccessorEventing.hpp" // just for constants...

DBMCliEvtDisp_Dispatcher::~DBMCliEvtDisp_Dispatcher() {
    if( m_DispatchingList != NULL )
        clearDispatchingList();
}

DBMCliEvtDisp_Dispatcher::DBMCliEvtDisp_Dispatcher(
        bool internal,
        bool remoteAccess,
        const DBMCli_String& aNode,
        const DBMCli_String& aDb,
        const DBMCli_String& aUserpw,
        const DBMCli_String& aCfgfile,
        const DBMCli_String& aLogfile,
        Msg_List& outList)
        : dispatcherprop(),
          m_IsInternal(internal),
          m_userpw(aUserpw),
          m_NodeName(aNode),
          m_ConfigFile(aCfgfile),
          m_LogFile(aLogfile),
          m_InstanceID(-1),
          m_Logger(DBMLog_Logger::getInstance()),
          m_EvtCountSelf(0),
          m_EvtCountKernel(0),
          m_bRunning(false),
		  m_TasklimitMessageFromDBM(false),
          m_NotOnlineDispatched(false),
          m_DispatchingList(NULL),
          DBMCliEvtDisp_EventListener(aNode, aDb, aUserpw, outList) {

    //nodename
    if( m_NodeName.IsEmpty() )
        m_NodeName = RTE_ISystem::Instance().GetLocalNodeName();

    // are we connected at all?
    if( !IsConnected() ) {
        // no, report error if there is one...
        outList.Overrule(Msg_List(Msg_List::Error,SDBMSG_DBMED_NOCONNECT));
        return;
    }

    // check if the user has permission to start an event dispatcher
    DBMCli_String thisUserName(m_userpw.Left(m_userpw.Find(',')));
    DBMCli_Users& dbmUsers(GetUsers());
    if( !dbmUsers.Refresh(outList) ) {
        outList.Overrule(
                Msg_List(Msg_List::Error,
	            SDBMSG_DBMED_USERS));
        return;
    }
    bool permissionToStart(false);
    DBMCli_UserArray& dbmUserArray(dbmUsers.UserArray());
    for( int i=dbmUserArray.GetSize()-1; i>=0; --i ) {
        if( 0 == dbmUserArray.ElementAt(i).Name().CompareNoCase(thisUserName) ) {
            // I found myself in all the dbm users...
            DBMCli_UserRightArray& permissionArray(dbmUserArray.ElementAt(i).ServerRightArray());
            DBMCli_String permissionUse(DBMCliEvtDisp_Permissions_EvtDispUse_Name);
            DBMCli_String permissionAdmin(DBMCliEvtDisp_Permissions_EvtDispAdmin_Name);
            bool checkedUse(false);
            bool checkedAdmin(false);
            for( int permissionIndex=permissionArray.GetSize()-1; permissionIndex>=0; --permissionIndex ) {
                if( permissionArray.ElementAt(permissionIndex).Name() == permissionUse ) {
                    checkedUse = true;
                    permissionToStart = permissionArray.ElementAt(permissionIndex).Value();
                }
                else if( permissionArray.ElementAt(permissionIndex).Name() == permissionAdmin ) {
                    checkedAdmin = true;
                    permissionToStart = permissionArray.ElementAt(permissionIndex).Value();
                }
                if( permissionToStart || (checkedUse && checkedAdmin) )
                    break;
            }
            break;
        }
    }

    if( !permissionToStart ) {
        outList.Overrule(
                Msg_List(Msg_List::Error,
	            SDBMSG_DBMED_PERMISSION));
        return;
    }

	// version dependency
	DBMCli_NodeInfo & nodeInfo = this->NodeInfo();
	Msg_List list;
	if( !nodeInfo.Refresh(list) ) {
        outList.Overrule(list);
		outList.Overrule(Msg_List(Msg_List::Error, SDBMSG_DBMED_NO_SERVER_VERSION));
        return;
	}
	DBMCli_Version& dbmServerVersion = nodeInfo.Version();
    int versionMajor =  dbmServerVersion.GetMajor();
    int versionMinor =  dbmServerVersion.GetMinor();
	int versionRevis =  dbmServerVersion.GetRevision();

	// how to handle the situation, that the task limit for event tasks is reached
	m_TasklimitMessageFromDBM = (8 <= versionMajor) ||
                                (7 <= versionMajor && 7 <= versionMinor) ||
                                (7 <= versionMajor && 6 <= versionMinor && 14 <= versionRevis);

    // config file
    Tools_DynamicUTF8String cfg((const char*)aCfgfile);
    m_bRunning = dispatcherprop.load(outList, cfg);

    if( !m_bRunning ) {
        outList.Overrule(
            Msg_List(
                Msg_List::Error, SDBMSG_DBMED_ABORT,
                Msg_Arg("DISPCONFIG", (const char*)cfg.StrPtr())));
        return;
    }

    Tools_List<Tools_Property*>* rawDispatchingList(dispatcherprop.getAllProperties());
    fillDispatchingList(rawDispatchingList);
    clearRawList(rawDispatchingList);

    // obtain an instance number
    DBMCli_String addCommand("dbmevtdisp_internal_add");
    addCommand = addCommand + " ";
    addCommand = addCommand + SAPDB_ToString((int) RTESys_ProcessID());
    addCommand = addCommand + " ";
    addCommand = addCommand + SAPDB_ToString((int)(m_IsInternal?DBMSrvShM_LineAccessorEventing::INTERNAL:DBMSrvShM_LineAccessorEventing::OPEN));
    addCommand = addCommand + " ";
    addCommand = addCommand + SAPDB_ToString((int)(remoteAccess?DBMSrvShM_LineAccessorEventing::LOCALREMOTE:DBMSrvShM_LineAccessorEventing::LOCAL));
    addCommand = addCommand + " ";
    addCommand = addCommand + thisUserName; // username without password
    addCommand = addCommand + " \"";
    addCommand = addCommand + m_ConfigFile;
    addCommand = addCommand + "\" \"";
    addCommand = addCommand + m_LogFile;
    addCommand = addCommand + "\"";

    m_bRunning = DBMCli_Session::Execute((const char*) addCommand, outList);
    if( m_bRunning ) {
        DBMCli_Result result(DBMCli_Session::GetResult());
        DBMCli_String answerIndex;
        //result.GetLine(answerIndex); // read away the OK
        result.GetLine(answerIndex);
        m_InstanceID = atoi((const char*)answerIndex);
    }
}

bool DBMCliEvtDisp_Dispatcher::clearDispatchingList() {
    for(Tools_List<DBMCliEvtDispKeyMatch_Expression*>::iterator i = m_DispatchingList->begin();
        i != m_DispatchingList->end();
        ++i) {
        delete (*i);
    }
    delete m_DispatchingList;
    m_DispatchingList = NULL;
    return true;
}

bool DBMCliEvtDisp_Dispatcher::clearRawList(Tools_List<Tools_Property*>* aRawList) {
    for(Tools_List<Tools_Property*>::iterator i = aRawList->begin();
        i != aRawList->end();
        ++i) {
        delete (*i);
    }
    delete aRawList;
    return true;
}

bool DBMCliEvtDisp_Dispatcher::fillDispatchingList(Tools_List<Tools_Property*>* aRawList) {
    m_DispatchingList = new Tools_List<DBMCliEvtDispKeyMatch_Expression*>;
    for(Tools_List<Tools_Property*>::iterator i = aRawList->begin();
        i != aRawList->end();
        ++i) {
        if( 0 == SAPDB_strcmp((*i)->getKey(), DBMCliEvtDisp_Properties::FileVersionKey) )
            continue;
        m_DispatchingList->push_back(new DBMCliEvtDispKeyMatch_Expression((*i)->getKey(), (*i)->getValue()));
    }
    return true;
}

bool DBMCliEvtDisp_Dispatcher::Run() {

    // start message
    Msg_List statusMsg(
        Msg_List::Info,
        SDBMSG_DBMED_CURRCONFIG,
        Msg_Arg("DISPCONFIG", (const char*) m_ConfigFile));

    m_Logger.addMessageList(statusMsg);
    DBMCliEvtDisp_MsgDumper::dump(statusMsg, stdout);

    // create and send a dispatcher start event
    static char buf[512];
    sprintf(buf, (const char*)EVT_TEXT_DISPATCHERSTART, (const char*)m_NodeName, (const char*)DatabaseName());
    DBMCliEvtCm_Event dispStartEvent(
        EVT_NAME_DISPATCHERSTART,
        DBMCliEvtCm_Event::prio_low,
        buf,
        m_EvtCountSelf++);
    EventProc(dispStartEvent);

    // check if the database is offline now
    DBMCli_State& initialState(GetState());
    Msg_List stateErr;
    initialState.Refresh(stateErr);
    m_NotOnlineDispatched = (initialState.Value() != DBMCLI_DBSTATE_WARM);

    Msg_List& msgList(GetMsgObj());
    msgList.ClearMessageList();

    bool doListen(true);
    bool databaseWasRunning(false);
    bool databaseIsOffline(false);
    Msg_List oDummy;
    while ( doListen ) {
        bool doHandle(true);
        while( !DBMCli_Session::Execute("event_available", msgList) ) {

            // the database was at least one time in a running, this is important for the crash check because of
            // a database which does never run could not crash
            databaseWasRunning = databaseWasRunning || (msgList.ID() == DBMSrvMsg_Error::get_tcn00_Error(DBMSrvMsg_Error::DBMSrv_EVENTNONEAVAIL));

            // currently the database does not run it _maybe_ this is a crash
            databaseIsOffline  = (msgList.ID() == DBMSrvMsg_Error::get_tcn00_Error(DBMSrvMsg_Error::DBMSrv_DBNORUN));

            // check for database crash            
            if (databaseWasRunning                              &&               // 1. the database was at least on time running
                databaseIsOffline                               &&               // 2. above the database state was offline
                (GetParameters().GetValueAsInt("_DIAG_SEM")==1) &&               // 3. we check the crash flag and
                DBMCli_Database::GetState().Refresh(oDummy)     &&               // 4. we refresh the state
                (DBMCli_Database::GetState().Value()==DBMCLI_DBSTATE_OFFLINE)) { // 5. and check for offline again

                // Yeah, this _is_ a real crash!

                // generate the crash event
                DBMCliEvtCm_Event crashEvent(
                    EVT_NAME_DBCRASH,
                    msgList.ID(),
                    DBMCliEvtCm_Event::prio_high,
                    EVT_TEXT_DBCRASH,
                    m_EvtCountSelf++,
                    msgList.DateTime());
                EventProc(crashEvent);
            } // end if

            if( databaseIsOffline && databaseWasRunning) {
                // release the event session, it's dead meat!
                DBMCli_Session::Execute("event_release", oDummy);
            } // end if

            // forget the running state after the crash check if the database is offline
            databaseWasRunning = !databaseIsOffline && databaseWasRunning;

            // we got an error, what is it?
            if( HandleListenError() == StopListening ) {  // handle that
                executeStopCommand();
                return msgList.IsEmpty();
            } // end if

            // now check if we can get a command from server
            msgList.ClearMessageList();
            DBMCliEvtDisp_SrvCommand::ID commandFromServer(getCommandFromServer(msgList));
            if( !msgList.IsEmpty() )
                DBMCliEvtDisp_MsgDumper::dump(msgList);
            else 
                switch( commandFromServer ) {
                    case DBMCliEvtDisp_SrvCommand::NONE:
                        // do nothing...
                        break;
                    case DBMCliEvtDisp_SrvCommand::STOP: {
                        return executeStopCommand();
                        break;
                    }
                    default:
                        // that is an error
                        DBMCliEvtDisp_MsgDumper::dump(msgList);
                        break;
                }
            msgList.ClearMessageList();
            RTE_ISystem::DoSleep(500); // 0.5 seconds
            // reread the configuration file
            bool didReload(false);
            dispatcherprop.load(msgList, true, didReload); // reload just if necessary
            if( didReload ) {
                // delete the old dispatching list
                clearDispatchingList();
                Tools_List<Tools_Property*>* rawDispatchingList(dispatcherprop.getAllProperties());
                fillDispatchingList(rawDispatchingList);
                clearRawList(rawDispatchingList);
            }
        }

        // there is an event -> database is running!
        databaseWasRunning = true;        

        // there is an event let's get it
        doHandle = DBMCli_Session::Execute("event_receive", msgList);

        if( doHandle ) {
            //event_receive returned without error, so we
            //can create an event object and send it to handler
            DBMCliEvtCm_Event oEvent(DBMCli_Session::GetResult());

            doListen = EventProc(oEvent);

            // check if we missed an event...
            if( m_EvtCountKernel == 0 )
                m_EvtCountKernel = oEvent.GetCount();
            else {
                if( m_EvtCountKernel + 1 < oEvent.GetCount() ) {
                    // not good
                    for( ++m_EvtCountKernel; m_EvtCountKernel < oEvent.GetCount(); ++m_EvtCountKernel )
                        // create a special event for all missed events
                        HandleMissedEvent();
                }
                else {
                    // good
                    ++m_EvtCountKernel;
                }
            }
        }
        else {
            //error occurred while receiving the event
            if( HandleListenError() == StopListening ) {  // handle that
                executeStopCommand();
                return msgList.IsEmpty();
            }

            doListen = true;     // and keep going
        }
        msgList.ClearMessageList();
    }
    return msgList.IsEmpty();
}

bool DBMCliEvtDisp_Dispatcher::executeStopCommand() {
    // create and send a dispatcher stop event
    char buf[128];
    sprintf(buf, (const char*)EVT_TEXT_DISPATCHERSTOP, (const char*)m_NodeName, (const char*)DatabaseName());
    DBMCliEvtCm_Event dispStopEvent(
        EVT_NAME_DISPATCHERSTOP,
        DBMCliEvtCm_Event::prio_low,
        buf,
        m_EvtCountSelf++);
    EventProc(dispStopEvent);
    
    // remove line, build the command
    DBMCli_String deleteCommand("dbmevtdisp_internal_delete");
    deleteCommand = deleteCommand + " ";
    
    sprintf(buf, "%d", m_InstanceID);
    deleteCommand = deleteCommand + buf;

    return DBMCli_Session::Execute(deleteCommand, GetMsgObj());   
}


  /*!
      @brief called if an event is received

      Here, the actual dispatching takes place: The configuration is evaluated and for each handler
      found there, the variables from the configuration are replaced with their actual values.
      @param oEvent kernel event received from database
      @return indicator whether event listening should continue (always true here)
  */
bool DBMCliEvtDisp_Dispatcher::EventProc ( const DBMCliEvtCm_Event& oEvent ) {
    bool foundAMatch(false);
    for(
            Tools_List<DBMCliEvtDispKeyMatch_Expression*>::iterator dispatchingLine = m_DispatchingList->begin();
            dispatchingLine != m_DispatchingList->end();
            ++dispatchingLine) {
        
        if( (*dispatchingLine)->matchesWith(oEvent) ) {
            foundAMatch = true;
            DBMCli_String commandLine((*dispatchingLine)->getCommandLine().CharPtr());

            replaceVar(commandLine, DBMCliEvtDisp_Dispatcher::DBNAME, (const char*) DatabaseName());
            replaceVar(commandLine, DBMCliEvtDisp_Dispatcher::DBMUSERPW, (const char*) m_userpw);
            replaceVar(commandLine, DBMCliEvtDisp_Dispatcher::EVTALL, (const char*) oEvent.serialize());
            replaceVar(commandLine, DBMCliEvtDisp_Dispatcher::EVTDATE, (const char*) oEvent.GetDate().Get(DBMCLI_DT_INT_DATE_FMT));
            replaceVar(commandLine, DBMCliEvtDisp_Dispatcher::EVTNAME, (const char*) oEvent.GetName());
            replaceVar(commandLine, DBMCliEvtDisp_Dispatcher::EVTTIME, (const char*) oEvent.GetTime().Get(DBMCLI_DT_INT_TIME_FMT));
            char szBuf[128];
            sprintf(szBuf, "%d", oEvent.GetValue1());
            replaceVar(commandLine, DBMCliEvtDisp_Dispatcher::EVTVAL1, szBuf);
            sprintf(szBuf, "%d", oEvent.GetValue2());
            replaceVar(commandLine, DBMCliEvtDisp_Dispatcher::EVTVAL2, szBuf);
            // get host name even if it was not passed as parameter
            replaceVar(commandLine, DBMCliEvtDisp_Dispatcher::SERVERNAME, m_NodeName);
            sprintf(szBuf, "%d", oEvent.GetPriority());
            replaceVar(commandLine, DBMCliEvtDisp_Dispatcher::EVTPRIO, szBuf);
            replaceVar(commandLine, DBMCliEvtDisp_Dispatcher::EVTTEXT, (const char*) oEvent.GetText());
            replaceVar(commandLine, DBMCliEvtDisp_Dispatcher::EVTDESC, (const char*) oEvent.GetDescription());
            sprintf(szBuf, "%d", oEvent.GetCount());
            replaceVar(commandLine, DBMCliEvtDisp_Dispatcher::EVTCOUNT, szBuf);

            // actually call the handler
            doCall(commandLine);
        }
    }        

    Msg_List outList;
    if( foundAMatch )
        outList = Msg_List(Msg_List::Info,
                        SDBMSG_DBMED_EVENTDISPATCHED,
	                    Msg_Arg("EVENTNAME",(const char *) oEvent.GetName()),
                        Msg_Arg("EVENTCOUNT", oEvent.GetCount()));
    else
        outList = Msg_List(Msg_List::Info,
                        SDBMSG_DBMED_EVENTNOTDISPATCHED,
	                    Msg_Arg("EVENTNAME",(const char *) oEvent.GetName()),
                        Msg_Arg("EVENTCOUNT", oEvent.GetCount()));

    m_Logger.addMessageList(outList);
    DBMCliEvtDisp_MsgDumper::dump(outList, stdout);
    return true;
}

void DBMCliEvtDisp_Dispatcher::doCall(DBMCli_String& handler ) {

    // check if we must pass arguments via pipe
    int count(0);
    bool pipeCall(false);
    while( !pipeCall && m_ViaPipe[count].GetLength() > 0 ) {
        pipeCall =
                handler.GetLength() >= m_ViaPipe[count].GetLength() &&
                handler.Left(m_ViaPipe[count].GetLength()) == m_ViaPipe[count];
        count++;
    }

    if( pipeCall ) {
        // okay pipe is requested
        Tools_PipeCall::CallProgram((const char*) handler, Tools_PipeCall::CallAsynchron);
    }
    else {
        // no pipe
        DBMSrvMsg_ErrorHandler errorHandler;
        errorHandler.activate();
        DBMSrvProc_DeamonProcess proc((const char*) handler);
        Msg_List outList;
        errorHandler.getLastError(outList);
        proc.getMessagesInto(outList);
        errorHandler.deactivate();
        m_Logger.addMessageList(outList);
        DBMCliEvtDisp_MsgDumper::dump(outList, stderr);
    }
}

void DBMCliEvtDisp_Dispatcher::replaceVar( DBMCli_String& str, const char* varName, const char* varVal) {
    if( varVal == NULL )
        return;
    int at(str.Find(varName));
    int len((int)strlen(varName));
    // one variable can occur more than once...
    while( at >= 0) {
        str = str.Left(at) + varVal + str.Right(str.GetLength()-at-len);
        at = str.Find(varName);
    }
}

DBMCliEvtDisp_SrvCommand::ID DBMCliEvtDisp_Dispatcher::getCommandFromServer(Msg_List& outList) {
    outList.ClearMessageList();

    char numberasText[128];

    DBMCli_String commandRequest("dbmevtdisp_internal_communication");
    commandRequest = commandRequest + " ";

    sprintf(numberasText, "%d", (int)DBMCliEvtDisp_SrvCommand::REQUESTCOMMAND);
    commandRequest = commandRequest + numberasText;
    commandRequest = commandRequest + " ";

    sprintf(numberasText, "%d", m_InstanceID);    
    commandRequest = commandRequest + numberasText;

    DBMCli_Session::Execute((const char*)commandRequest, outList);

    if( !outList.IsEmpty() ) {
        outList.Overrule(
                Msg_List(Msg_List::Error,
                SDBMSG_DBMED_INTCOM,
                Msg_Arg("DBMCMD", (const char*)commandRequest)));
    }
    else {
        // command executed successfully
        DBMCli_Result result(DBMCli_Session::GetResult());
        result.SetPos(0);
        if( result.Check(outList) ) {
            DBMCli_String resultLine;
            //result.GetLine(resultLine); // read away the okay
            result.GetLine(resultLine);
            return (DBMCliEvtDisp_SrvCommand::ID) atoi((const char*) resultLine);
        }
        else {
            outList.Overrule(
                    Msg_List(Msg_List::Error,
                    SDBMSG_DBMED_INTCOM,
                    Msg_Arg("DBMCMD", (const char*)commandRequest)));
        }
    }
    return DBMCliEvtDisp_SrvCommand::NONE;
}

DBMCliEvtDisp_Dispatcher::NextActionType DBMCliEvtDisp_Dispatcher::HandleListenError() {

    Msg_List& list(GetMsgObj());

    if( !list.IsEmpty() ) {
        if( list.ID() == (SAPDB_UInt4) SDBMsg_UniqueMessageNumber::ERR_DBNORUN ) {
            // that's okay
            if( !m_NotOnlineDispatched ) {
                DBMCliEvtCm_Event shutdownEvent(
                    EVT_NAME_SHUTDOWN,
                    list.ID(),
                    DBMCliEvtCm_Event::prio_medium,
                    list.Message(),
                    m_EvtCountSelf++,
                    list.DateTime());
                EventProc(shutdownEvent);
                m_NotOnlineDispatched = true;
            }
            return GoonListening;
        }

        if( list.ID() == (SAPDB_UInt4) SDBMsg_UniqueMessageNumber::ERR_SHUTDOWN ) {
            // that's okay
            DBMCliEvtCm_Event shutdownEvent(
                EVT_NAME_SHUTDOWN,
                list.ID(),
                DBMCliEvtCm_Event::prio_medium,
                list.Message(),
                m_EvtCountSelf++,
                list.DateTime());
            EventProc(shutdownEvent);
            m_NotOnlineDispatched = true;
            return GoonListening;
        }


        if( list.ID() == (SAPDB_UInt4) SDBMsg_UniqueMessageNumber::ERR_EVENTNONEAVAIL )
            // that's okay
            return GoonListening;
        
        if( list.ID() == (SAPDB_UInt4) -4 ) {     // no connection to DBM Server
            // that's an error
            DBMLog_Logger::getInstance().addMessageList(list);
            DBMCliEvtCm_Event connLostEvent(
                EVT_NAME_CLCONNECTIONLOST,
                list.ID(),
                DBMCliEvtCm_Event::prio_high,
                EVT_TEXT_CLCONNECTIONLOST,
                m_EvtCountSelf++,
                list.DateTime());
            EventProc(connLostEvent);
            return StopListening;
        }


        bool tasklimitReached(false);
        if( m_TasklimitMessageFromDBM ) {
            tasklimitReached = (list.ID() == (SAPDB_UInt4) SDBMsg_UniqueMessageNumber::ERR_SESSIONLIMIT);
        }
        else {
            tasklimitReached =
                (list.ID() == (SAPDB_UInt4) SDBMsg_UniqueMessageNumber::ERR_RTE) &&
                (list.NextMessage() != NULL) &&
                (list.NextMessage()->ID() == (SAPDB_UInt4) 2);
        }

        if( tasklimitReached ) {
            DBMCliEvtCm_Event noEventTaskEvent(
                EVT_NAME_NOEVENTTASK,
                list.ID(),
                DBMCliEvtCm_Event::prio_high,
                EVT_TEXT_NOEVENTTASK,
                m_EvtCountSelf++,
                list.DateTime());
            EventProc(noEventTaskEvent);
            return StopListening;
        }

        if( (list.ID() == (SAPDB_UInt4) SDBMsg_UniqueMessageNumber::ERR_RTE) &&
            (list.NextMessage() != NULL) &&
            (list.NextMessage()->ID() == (SAPDB_UInt4) 5)) {
            // the database is offline, this is no error and not worth an event
            return GoonListening;
        }

        // we cannot interpret it, create an unknown event
        //printf(">>\n");
        //DBMCliEvtDisp_MsgDumper::dump(list);
        //printf(">>\n");
        DBMCliEvtCm_Event unknownErrEvent(
            EVT_NAME_UNKNOWNERR,
            list.ID(),
            DBMCliEvtCm_Event::prio_unknown,
            list.Message(),
            m_EvtCountSelf++,
            list.DateTime());
        EventProc(unknownErrEvent);
        // log details of unknown event
        m_Logger.addMessageList(list);
        return GoonListening;
    }
    else {
        // there is an error, but the list is empty
        RTE_ISystem::DateTime now;
        RTE_ISystem::GetLocalDateTime( now );
        DBMCliEvtCm_Event unknownErrEvent(
            EVT_NAME_UNKNOWNERR,
            0,
            DBMCliEvtCm_Event::prio_unknown,
            EVT_TEXT_UNKNOWNERR,
            m_EvtCountSelf++,
            now);
        EventProc(unknownErrEvent);
        return GoonListening;
    }
}

void DBMCliEvtDisp_Dispatcher::HandleMissedEvent() {
    RTE_ISystem::DateTime now;
    RTE_ISystem::GetLocalDateTime(now);
    DBMCliEvtCm_Event missedEventEvent(
        EVT_NAME_MISSEDEVENT,
        m_EvtCountKernel,
        DBMCliEvtCm_Event::prio_medium,
        EVT_TEXT_MISSEDEVENT,
        m_EvtCountSelf++,
        now);
    EventProc(missedEventEvent);
}

// event handlers tat understand parameter passing via pipe
const DBMCli_String DBMCliEvtDisp_Dispatcher::m_ViaPipe[] =
            {"dbmevthndl_display",
             "dbmevthndl_winlog",
             "dbmevthndl_extendDB",
             "dbmcli",
             ""};

// variables for configueration file
const char* const DBMCliEvtDisp_Dispatcher::DBNAME = "$DBNAME$";
const char* const  DBMCliEvtDisp_Dispatcher::DBMUSERPW = "$DBMUSERPW$";
const char* const  DBMCliEvtDisp_Dispatcher::SERVERNAME = "$SERVERNAME$";
const char* const  DBMCliEvtDisp_Dispatcher::EVTALL = "$EVTALL$";
const char* const  DBMCliEvtDisp_Dispatcher::EVTNAME = "$EVTNAME$";
const char* const  DBMCliEvtDisp_Dispatcher::EVTVAL1 = "$EVTVAL1$";
const char* const  DBMCliEvtDisp_Dispatcher::EVTVAL2 = "$EVTVAL2$";
const char* const  DBMCliEvtDisp_Dispatcher::EVTDATE = "$EVTDATE$";
const char* const  DBMCliEvtDisp_Dispatcher::EVTTIME = "$EVTTIME$";
const char* const  DBMCliEvtDisp_Dispatcher::EVTDESC = "$EVTDESC$";
const char* const  DBMCliEvtDisp_Dispatcher::EVTTEXT = "$EVTTEXT$";
const char* const  DBMCliEvtDisp_Dispatcher::EVTCOUNT = "$EVTCOUNT$";
const char* const  DBMCliEvtDisp_Dispatcher::EVTPRIO = "$EVTPRIO$";

// for creating own events
const DBMCli_String DBMCliEvtDisp_Dispatcher::EVT_NAME_SHUTDOWN("DISPWARN:SHUTDOWN");
const DBMCli_String DBMCliEvtDisp_Dispatcher::EVT_NAME_DBCRASH("DISPERR:DBCRASH");
const DBMCli_String DBMCliEvtDisp_Dispatcher::EVT_NAME_UNKNOWNERR("DISPWARN:UNKNOWN");
const DBMCli_String DBMCliEvtDisp_Dispatcher::EVT_NAME_MISSEDEVENT("DISPWARN:MISSEDEVT");
const DBMCli_String DBMCliEvtDisp_Dispatcher::EVT_NAME_DISPATCHERSTOP("DISPINFO:DISPSTOP");
const DBMCli_String DBMCliEvtDisp_Dispatcher::EVT_NAME_DISPATCHERSTART("DISPINFO:DISPSTART");
const DBMCli_String DBMCliEvtDisp_Dispatcher::EVT_NAME_CLCONNECTIONLOST("DISPERR:CONNLOST");
const DBMCli_String DBMCliEvtDisp_Dispatcher::EVT_NAME_SRCONNECTIONLOST("DISPERR:KERNELLOST");
const DBMCli_String DBMCliEvtDisp_Dispatcher::EVT_NAME_NOEVENTTASK("DISPERR:NOEVENTTASK");
const DBMCli_String DBMCliEvtDisp_Dispatcher::EVT_TEXT_DBCRASH("the database crashed");
const DBMCli_String DBMCliEvtDisp_Dispatcher::EVT_TEXT_UNKNOWNERR("unknown error");
const DBMCli_String DBMCliEvtDisp_Dispatcher::EVT_TEXT_MISSEDEVENT("event is missing");
const DBMCli_String DBMCliEvtDisp_Dispatcher::EVT_TEXT_DISPATCHERSTOP("dispatcher stopped. host: %s, db: %s");
const DBMCli_String DBMCliEvtDisp_Dispatcher::EVT_TEXT_DISPATCHERSTART("dispatcher started. host: %s, db: %s");
const DBMCli_String DBMCliEvtDisp_Dispatcher::EVT_TEXT_CLCONNECTIONLOST("connection to DBM Server lost");
const DBMCli_String DBMCliEvtDisp_Dispatcher::EVT_TEXT_SRCONNECTIONLOST("DBM Server lost connection to kernel");
const DBMCli_String DBMCliEvtDisp_Dispatcher::EVT_TEXT_NOEVENTTASK("no event task in kernel");
