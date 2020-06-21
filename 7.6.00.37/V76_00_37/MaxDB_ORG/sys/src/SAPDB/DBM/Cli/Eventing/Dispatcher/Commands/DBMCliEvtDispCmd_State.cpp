/*!
  @file           DBMCliEvtDispCmd_State.cpp
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

#include "DBM/Cli/Eventing/Dispatcher/Commands/DBMCliEvtDispCmd_State.hpp"
#include "DBM/Cli/Eventing/Dispatcher/DBMCliEvtDisp_MsgDumper.hpp"
#include "DBM/Cli/Eventing/Dispatcher/DBMCliEvtDisp_SrvCommand.hpp"
#include "SAPDBCommon/Messages/SDBMsg_DBMEd.h"

DBMCliEvtDispCmd_State::DBMCliEvtDispCmd_State(
        const char* nodeName,
        const char* dbName,
        const char* userName,
        Msg_List& outList)
        : DBMCliEvtDispCmd_Base(),
          m_ReadyToRun(false),
          m_OutList(outList),
          m_Session(
                DBMCli_String(nodeName),
                DBMCli_String(dbName),
                DBMCli_String(userName),
                outList,
                false) {
    m_ReadyToRun = m_OutList.IsEmpty();
}

bool DBMCliEvtDispCmd_State::run() {

    if( !m_ReadyToRun )
        return false;

    if( !m_Session.Connect(m_OutList) ) {
        // message is good enough, no need to add another one here
        return false;
    }
    
    char cmdAsChar[10];
    sprintf(cmdAsChar, "%d", (int)DBMCliEvtDisp_SrvCommand::STATE);

    DBMCli_String stateCommand("dbmevtdisp_internal_communication");
    stateCommand = stateCommand + " ";

    stateCommand = stateCommand + cmdAsChar;

    if( m_Session.Execute( stateCommand, m_OutList ) ) {
        DBMCli_String answerLine;
        if( m_Session.GetResult().GetLine(answerLine) ) {
            while(answerLine.GetLength() > 0 ) {
                Tools_DynamicUTF8String wholeLine((const char*) answerLine);
                // now divide the line into the 5 tokens
                Tools_DynamicUTF8String::BasisElementIndex tokenEnd(wholeLine.Find(','));
                Tools_DynamicUTF8String::BasisElementIndex tokenStart(0);
                Tools_DynamicUTF8String tokens[5];
                for( int i=0; tokenEnd != Tools_DynamicUTF8String::NPos && tokenStart != Tools_DynamicUTF8String::NPos && i < 4; ++i ) {
                    tokens[i] = wholeLine.SubStrBasis(tokenStart, tokenEnd);
                    tokenStart = tokenEnd + 1;
                    tokenEnd = wholeLine.Find(',', tokenStart);
                }
                if( tokenStart != Tools_DynamicUTF8String::NPos )
                    tokens[4] = wholeLine.SubStr(tokenStart);

                if( tokenStart == Tools_DynamicUTF8String::NPos ) {
                    // unexpected answer
                    m_OutList.Overrule(
                            Msg_List(Msg_List::Error,
	                        SDBMSG_DBMED_INTCOM,
	                        Msg_Arg("DBMCMD",(const char*)stateCommand)));
                    return false;
                }
                
                m_OutList.Overrule(
                        Msg_List(Msg_List::Error,
                            SDBMSG_DBMED_CMDSTATE_REPLYLINE_LOG,
	                        Msg_Arg("FILENAME",tokens[4].CharPtr())));
                m_OutList.Overrule(
                        Msg_List(Msg_List::Error,
                            SDBMSG_DBMED_CMDSTATE_REPLYLINE_CONFIG,
	                        Msg_Arg("FILENAME",tokens[3].CharPtr())));
                m_OutList.Overrule(
                        Msg_List(Msg_List::Error,
                            SDBMSG_DBMED_CMDSTATE_REPLYLINE_HOST,
	                        Msg_Arg("HOSTNAME",tokens[2].CharPtr())));
                m_OutList.Overrule(
                        Msg_List(Msg_List::Error,
                            SDBMSG_DBMED_CMDSTATE_REPLYLINE_USERID,
	                        Msg_Arg("USERID",tokens[1].CharPtr())));
                m_OutList.Overrule(
                        Msg_List(Msg_List::Error,
                            SDBMSG_DBMED_CMDSTATE_REPLYLINE_INSTANCEID,
	                        Msg_Arg("DISPID",tokens[0].CharPtr())));
                m_Session.GetResult().GetLine(answerLine);
            }
        }
        return true;
    }
    else {
        m_OutList.Overrule(
                Msg_List(Msg_List::Error,
	            SDBMSG_DBMED_INTCOM,
	            Msg_Arg("DBMCMD",(const char*)stateCommand)));
        return false;
    }
}
