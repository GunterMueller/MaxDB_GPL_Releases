/*!
  @file           DBMCliEvtDispCmd_Stop.cpp
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

#include "DBM/Cli/Eventing/Dispatcher/Commands/DBMCliEvtDispCmd_Stop.hpp"
#include "DBM/Cli/Eventing/Dispatcher/DBMCliEvtDisp_MsgDumper.hpp"
#include "DBM/Cli/Eventing/Dispatcher/DBMCliEvtDisp_SrvCommand.hpp"
#include "SAPDBCommon/Messages/SDBMsg_DBMEd.h"

DBMCliEvtDispCmd_Stop::DBMCliEvtDispCmd_Stop(
        const char* instanceID,
        const char* nodeName,
        const char* dbName,
        const char* userName,
        Msg_List& outList)
        : DBMCliEvtDispCmd_Base(),
          m_ReadyToRun(false),
          m_InstanceID(NULL),
          m_OutList(outList),
          m_Session(
                DBMCli_String(nodeName),
                DBMCli_String(dbName),
                DBMCli_String(userName),
                outList,
                false) {

    m_InstanceID = new char[strlen(instanceID)+1];
    if( m_InstanceID == NULL )
        return;
    
    m_InstanceID[0] = '\0';
    SAPDB_strcat(m_InstanceID, instanceID);
    
    m_ReadyToRun = m_OutList.IsEmpty();
}

DBMCliEvtDispCmd_Stop::~DBMCliEvtDispCmd_Stop() {
    if( m_InstanceID != NULL )
        delete[] m_InstanceID;
}

bool DBMCliEvtDispCmd_Stop::run() {

    if( !m_ReadyToRun )
        return false;

    if( !m_Session.Connect(m_OutList) ) {
        // message is good enough, no need to add another one here
    }
    
    char cmdAsChar[10];
    sprintf(cmdAsChar, "%d", (int)DBMCliEvtDisp_SrvCommand::STOP);

    DBMCli_String stopCommand("dbmevtdisp_internal_communication");
    stopCommand = stopCommand + " ";

    stopCommand = stopCommand + cmdAsChar;
    stopCommand = stopCommand + " ";

    stopCommand = stopCommand + m_InstanceID;

    return m_Session.Execute(stopCommand, m_OutList);
}