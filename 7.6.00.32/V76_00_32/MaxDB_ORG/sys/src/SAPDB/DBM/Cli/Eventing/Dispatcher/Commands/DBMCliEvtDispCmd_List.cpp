/*!
  @file           DBMCliEvtDispCmd_List.cpp
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

#include "DBM/Cli/Eventing/Dispatcher/Commands/DBMCliEvtDispCmd_List.hpp"
#include "DBM/Cli/Eventing/Dispatcher/DBMCliEvtDisp_MsgDumper.hpp"
#include "DBM/Cli/Eventing/Dispatcher/DBMCliEvtDisp_Properties.hpp"
#include "DBM/Cli/Eventing/Dispatcher/KeyMatch/DBMCliEvtDispKeyMatch_Expression.hpp"
#include "DBM/Srv/Commands/DBMSrvCmd_EventAdministrationConstants.hpp"
#include "SAPDBCommon/Messages/SDBMsg_DBMEd.h"
#include "ToolsCommon/Tools_DynamicUTF8String.hpp"

DBMCliEvtDispCmd_List::DBMCliEvtDispCmd_List(
        Msg_List& outList,
        const char* cfgFile)
      : DBMCliEvtDispCmd_Base(),
        m_OutList(outList),
        m_AllProps(NULL),
        m_PropertiesFile(cfgFile) {
}

DBMCliEvtDispCmd_List::~DBMCliEvtDispCmd_List(){
    if( m_AllProps != NULL ) {
        for( Tools_List<Tools_Property*>::iterator it(m_AllProps->begin()); it != m_AllProps->end(); ++it )
            delete *it;
        delete m_AllProps;
    }
}

void DBMCliEvtDispCmd_List::clearProperties() {
    if( m_AllProps != NULL ) {
        for( Tools_List<Tools_Property*>::iterator it(m_AllProps->begin()); it != m_AllProps->end(); ++it )
            delete *it;
        delete m_AllProps;
    }
}

bool DBMCliEvtDispCmd_List::run() {

    clearProperties();

    // try to open the file
    DBMCliEvtDisp_Properties propertiesObject(m_PropertiesFile.CharPtr());
    if( !propertiesObject.load(m_OutList) ) {
        m_OutList.Overrule(
            Msg_List(Msg_List::Error,
            SDBMSG_DBMED_LOADCONFIGFILE,
            Msg_Arg("DISPCONFIG",m_PropertiesFile.CharPtr())));
        return false;
    }

    m_AllProps = propertiesObject.getAllProperties();
    if( m_AllProps == NULL ) {
        m_OutList.Overrule(
            Msg_List(Msg_List::Error,
            SDBMSG_DBMED_LOADCONFIGFILE,
            Msg_Arg("DISPCONFIG",m_PropertiesFile.CharPtr())));
        return false;
    }
    
    Tools_DynamicUTF8String versionKey(DBMCliEvtDisp_Properties::FileVersionKey);
    for( Tools_List<Tools_Property*>::reverse_iterator it(m_AllProps->rbegin()); it != m_AllProps->rend(); ++it ) {
        Tools_DynamicUTF8String currentKey((*it)->getKey());
        if( currentKey == versionKey )
            continue;
        Tools_DynamicUTF8String::BasisElementIndex firstDot(currentKey.Find(DBMCliEvtDispKeyMatch_Expression::KeySeparator));

        if( firstDot == Tools_DynamicUTF8String::NPos ) {
            m_OutList.Overrule(
                Msg_List(Msg_List::Error,
                SDBMSG_DBMED_CONFIGSYNTAX,
                Msg_Arg("DISPCONFIG",m_PropertiesFile.CharPtr()),
                Msg_Arg("DISPCONDITION", currentKey.CharPtr())));
            return false;
        }
        
        // print a reply entry
        m_OutList.Overrule(
                Msg_List(Msg_List::Info,
	            SDBMSG_DBMED_CMDLIST_REPLYLINE_COMMAND,
	            Msg_Arg("DISPCOMMAND",(*it)->getValue())));
        
        Tools_DynamicUTF8String condition(currentKey.SubStr(firstDot+1));
        // replace the operator like with a == in the condition
        Tools_DynamicUTF8String::BasisElementIndex
                pos(condition.Find(DBMCliEvtDispKeyMatch_Expression::OperatorLikeText));
        if( pos != Tools_DynamicUTF8String::NPos &&
            pos <= (Tools_DynamicUTF8String::BasisElementIndex)SAPDB_strlen(DBMCliEvtCm_Event::AttribName)+4) { /* "((Name l" */
            condition.Replace(                                                                                  /*  ^^    ^^  */
                    pos,
                    SAPDB_strlen(DBMCliEvtDispKeyMatch_Expression::OperatorLikeText),
                    DBMCliEvtDispKeyMatch_Expression::OperatorEqualsText);
        }
        // replace the numeric priority value with it's word
        const char* const bad("&& (Priority == "); // I know it's bad!
        pos = condition.Find(bad);
        if( pos != Tools_DynamicUTF8String::NPos ) {
            pos = pos + SAPDB_strlen(bad);
            condition.Replace(
                    pos,
                    1,
                    DBMSrvCmd_EventAdministrationConstants::getTextFromEventPriority((tsp31_event_prio_Enum)atoi(condition.SubStr(pos).CharPtr())));
        }


        m_OutList.Overrule(
                Msg_List(Msg_List::Info,
	            SDBMSG_DBMED_CMDLIST_REPLYLINE_CONDITION,
	            Msg_Arg("DISPCONDITION",condition.CharPtr())));
        m_OutList.Overrule(
                Msg_List(Msg_List::Info,
	            SDBMSG_DBMED_CMDLIST_REPLYLINE_ENTRYID,
	            Msg_Arg("DISPENTRYID",atoi(currentKey.SubStrBasis(0, firstDot).CharPtr()))));
    }
    return true;
}