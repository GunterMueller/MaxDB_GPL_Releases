/*!
  @file           DBMCliEvtDispCmd_Delete.cpp
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

#include "DBM/Cli/Eventing/Dispatcher/Commands/DBMCliEvtDispCmd_Delete.hpp"
#include "DBM/Cli/Eventing/Dispatcher/DBMCliEvtDisp_Properties.hpp"
#include "DBM/Cli/Eventing/Dispatcher/KeyMatch/DBMCliEvtDispKeyMatch_Expression.hpp"
#include "SAPDBCommon/Messages/SDBMsg_DBMEd.h"
#include "ToolsCommon/Tools_DynamicUTF8String.hpp"

DBMCliEvtDispCmd_Delete::DBMCliEvtDispCmd_Delete(
        Msg_List& outList,
        const char* cfgFile,
        const char* entryID)
      : DBMCliEvtDispCmd_List(outList, cfgFile),
        m_EntryValid(false),
        m_EntryID(-1) {

    if( Tools_DynamicUTF8String(entryID).FindFirstNotOf("0123456789", 0) != Tools_DynamicUTF8String::NPos ) {
        m_OutList.Overrule(
                Msg_List(Msg_List::Error,
	            SDBMSG_DBMED_ILLDISPENTRYID,
	            Msg_Arg("DISPENTRYID",entryID)));
        return;
    }

    m_EntryID = atoi(entryID);
    m_EntryValid = true;
}

bool DBMCliEvtDispCmd_Delete::run() {

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

    for( Tools_List<Tools_Property*>::iterator it(m_AllProps->begin()); it != m_AllProps->end(); ++it ) {
        Tools_DynamicUTF8String currentKey((*it)->getKey());
        Tools_DynamicUTF8String::BasisElementIndex firstDot(currentKey.Find(DBMCliEvtDispKeyMatch_Expression::KeySeparator));
        if( firstDot == Tools_DynamicUTF8String::NPos ) {
            m_OutList.Overrule(
                Msg_List(Msg_List::Error,
                SDBMSG_DBMED_CONFIGSYNTAX,
                Msg_Arg("DISPCONFIG",m_PropertiesFile.CharPtr()),
                Msg_Arg("DISPCONDITION", currentKey.CharPtr())));
            return false;
        }
        if( m_EntryID == atoi(currentKey.SubStrBasis(0, firstDot).CharPtr())) {
            propertiesObject.removeProperty(Tools_DynamicUTF8String((*it)->getKey()));
            if( !propertiesObject.store(m_OutList) ) {
                m_OutList.Overrule(
                    Msg_List(Msg_List::Error,
                    SDBMSG_DBMED_SAVECONFIGFILE,
                    Msg_Arg("DISPCONFIG",m_PropertiesFile.CharPtr())));
                return false;
            }
            else {
                // print a reply
                m_OutList.Overrule(
                        Msg_List(Msg_List::Error,
                        SDBMSG_DBMED_CMDDELETE_REPLY,
                        Msg_Arg("DISPENTRYID",m_EntryID),
                        Msg_Arg("DISPCONFIG",m_PropertiesFile.CharPtr())));
                return true;
            }
        }
    }
    return true;
}