/*!
  @file           DBMCliEvtDispCmd_Start.cpp
  @author         MarcW
  @brief          

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

#include "DBM/Cli/Eventing/Dispatcher/Commands/DBMCliEvtDispCmd_Start.hpp"
#include "DBM/Cli/Eventing/Dispatcher/DBMCliEvtDisp_Main.hpp"
#include "DBM/Cli/Eventing/Dispatcher/DBMCliEvtDisp_MsgDumper.hpp"
#include "DBM/Logging/DBMLog_Logger.hpp"
#include "SAPDBCommon/Messages/SDBMsg_DBMEd.h"

DBMCliEvtDispCmd_Start::DBMCliEvtDispCmd_Start(
        bool internal,
        bool remoteAccess,
        const char* nodeName,
        const char* dbName,
        const char* userName,
        const char* cfgFile,
        const char* logFile,
        Msg_List& outList)
        : DBMCliEvtDispCmd_Base(),
          m_ReadyToRun(false),
          m_OutList(outList),
          m_Dispatcher(
                internal,
                remoteAccess,
                DBMCli_String(nodeName),
                DBMCli_String(dbName),
                DBMCli_String(userName),
                DBMCli_String(cfgFile),
                DBMCli_String(logFile),
                outList) {

    m_ReadyToRun = m_Dispatcher.isListening();
}

bool DBMCliEvtDispCmd_Start::run() {

    DBMLog_Logger& theLogger(DBMLog_Logger::getInstance());

    // has anything gone wrong during dispatcher initialisation?
    if( !m_ReadyToRun ) {
        // this case was already handled
        return false;
    }
    else {
        DBMCliEvtDisp_MsgDumper::dump(
            Msg_List(Msg_List::Info,
                    SDBMSG_DBMED_CMDSTART_REPLY,
                    Msg_Arg("DISPID",m_Dispatcher.getInstanceID())));

        // listen...
        DBMCliEvtDisp_Main_RunDispatcher(m_Dispatcher);

        // done listening
        m_OutList.Overrule(Msg_List(Msg_List::Info,SDBMSG_DBMED_DISPSTOP));
        DBMCliEvtDisp_MsgDumper::dump( m_OutList, stdout );
        theLogger.addMessageList(m_OutList);
        return true;
    }
}