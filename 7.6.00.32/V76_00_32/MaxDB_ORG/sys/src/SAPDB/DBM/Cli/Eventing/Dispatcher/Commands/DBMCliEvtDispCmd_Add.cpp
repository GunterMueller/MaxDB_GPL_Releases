/*!
  @file           DBMCliEvtDispCmd_Add.cpp
  @author         MarcW
  @brief          

    ========== licence begin  GPL
    Copyright (c) 2003-2006 SAP AG

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

#include "heo06.h"
#include "DBM/Cli/Eventing/Common/DBMCliEvtCm_Event.hpp"
#include "DBM/Cli/Eventing/Dispatcher/Commands/DBMCliEvtDispCmd_Add.hpp"
#include "DBM/Cli/Eventing/Dispatcher/DBMCliEvtDisp_Properties.hpp"
#include "DBM/Cli/Eventing/Dispatcher/KeyMatch/DBMCliEvtDispKeyMatch_Expression.hpp"
#include "DBM/VersionInfo/DBMVI_Strings.hpp"
#include "SAPDBCommon/Messages/SDBMsg_DBMEd.h"
#include "ToolsCommon/Tools_DynamicUTF8String.hpp"

DBMCliEvtDispCmd_Add::DBMCliEvtDispCmd_Add(
        Msg_List& outList,
        const char* cfgFile,
        ToolsParsersOldSyntEl_Variable& nameValue,
        ToolsParsersOldSyntEl_Keyword& priorityLow,
        ToolsParsersOldSyntEl_Keyword& priorityMedium,
        ToolsParsersOldSyntEl_Keyword& priorityHigh,
        ToolsParsersOldSyntEl_Variable& value1Value,
        ToolsParsersOldSyntEl_Keyword& value1EQ,
        ToolsParsersOldSyntEl_Keyword& value1GE,
        ToolsParsersOldSyntEl_Keyword& value1SE,
        ToolsParsersOldSyntEl_Keyword& value1GT,
        ToolsParsersOldSyntEl_Keyword& value1ST,
        ToolsParsersOldSyntEl_Variable& value2Value,
        ToolsParsersOldSyntEl_Keyword& value2EQ,
        ToolsParsersOldSyntEl_Keyword& value2GE,
        ToolsParsersOldSyntEl_Keyword& value2SE,
        ToolsParsersOldSyntEl_Keyword& value2GT,
        ToolsParsersOldSyntEl_Keyword& value2ST,
        const char* commandLine)
      : DBMCliEvtDispCmd_List(outList, cfgFile),
        m_KeyValid(false),
        m_CommandLine(commandLine) {

    // we know we can build a correct condition, we can do it right here
    m_DispatchingCondition = "(";

    bool needAnd(false);

    // event name
    if( nameValue.isSet() ) {
        // event name is specified...
        Tools_DynamicUTF8String value(nameValue.getValue());
        if( value.Length() >= 1 ) {
            m_DispatchingCondition += "(";
            m_DispatchingCondition += DBMCliEvtCm_Event::AttribName;
            if( value[value.Length()-1] == '*' )
                m_DispatchingCondition += " like ";
            else
                m_DispatchingCondition += " == ";
            m_DispatchingCondition += "\"";
            m_DispatchingCondition += value.ToUpper();
            m_DispatchingCondition += "\"";
            m_DispatchingCondition += ")";
            needAnd = true;
        }
        else
            return;
    }

    // event priority
    if( priorityLow.isSet() || priorityMedium.isSet() ||priorityHigh.isSet() ) {
        // a priority is specified
        if( needAnd )
            m_DispatchingCondition += " && ";

        m_DispatchingCondition += "(";
        m_DispatchingCondition += DBMCliEvtCm_Event::AttribPriority;
        m_DispatchingCondition += " == ";
        if( priorityLow.isSet() )         // low
            m_DispatchingCondition += "1";
        else if( priorityMedium.isSet() ) // medium
            m_DispatchingCondition += "2";
        else                              // high
            m_DispatchingCondition += "3";     
        m_DispatchingCondition += ")";
        needAnd = true;
    }

    // value1
    if( value1Value.isSet() ) {
        // value1 is specified
        if( needAnd )
            m_DispatchingCondition += " && ";

        Tools_DynamicUTF8String value1(value1Value.getValue());
        if( value1.Length() < 1 )
            return;
        if( value1.FindFirstNotOf("0123456789", (value1[0] == '-')?1:0) != Tools_DynamicUTF8String::NPos )
            return;
        m_DispatchingCondition += "(";
        m_DispatchingCondition += DBMCliEvtCm_Event::AttribValue1;
        m_DispatchingCondition += " ";
        if( value1EQ.isSet() )
            m_DispatchingCondition += "==";
        else if(value1GE.isSet() )
            m_DispatchingCondition += ">=";
        else if(value1SE.isSet() )
            m_DispatchingCondition += "<=";
        else if(value1GT.isSet() )
            m_DispatchingCondition += ">";
        else if(value1ST.isSet() )
            m_DispatchingCondition += "<";
        m_DispatchingCondition += " ";
        SAPDB_Int4 value1Num(atol(value1Value.getValue()));
        m_DispatchingCondition += SAPDB_ToString(value1Num);
        m_DispatchingCondition += ")";
        needAnd = true;
    }

    // value2
    if( value2Value.isSet() ) {
        // value2 is specified
        if( needAnd )
            m_DispatchingCondition += " && ";

        Tools_DynamicUTF8String value2(value2Value.getValue());
        if( value2.Length() < 1 )
            return;
        if( value2.FindFirstNotOf("0123456789", (value2[0] == '-')?1:0) != Tools_DynamicUTF8String::NPos )
            return;
        m_DispatchingCondition += "(";
        m_DispatchingCondition += DBMCliEvtCm_Event::AttribValue2;
        m_DispatchingCondition += " ";
        if( value2EQ.isSet() )
            m_DispatchingCondition += "==";
        else if(value2GE.isSet() )
            m_DispatchingCondition += ">=";
        else if(value2SE.isSet() )
            m_DispatchingCondition += "<=";
        else if(value2GT.isSet() )
            m_DispatchingCondition += ">";
        else if(value2ST.isSet() )
            m_DispatchingCondition += "<";
        m_DispatchingCondition += " ";
        SAPDB_Int4 value2Num(atol(value2Value.getValue()));
        m_DispatchingCondition += SAPDB_ToString(value2Num);
        m_DispatchingCondition += ")";
        needAnd = true;
    }

    if( !needAnd ) {
        // no condition was specified at all
        // we create an "artificial" condition that is always true
        m_DispatchingCondition += "(";
        m_DispatchingCondition += DBMCliEvtCm_Event::AttribName;
        m_DispatchingCondition += " like ";
        m_DispatchingCondition += "\"";
        m_DispatchingCondition += "*";
        m_DispatchingCondition += "\"";
        m_DispatchingCondition += ")";
    }

    m_DispatchingCondition += ")";
    m_KeyValid = true;
}

bool DBMCliEvtDispCmd_Add::run() {

    clearProperties();

    bool createdFile(false);
    tsp05_RteFileInfo finfo;
    tsp05_RteFileError ferror;
    sqlfinfoc (m_PropertiesFile.CharPtr(), &finfo, &ferror);
    if( ferror.sp5fe_result == vf_ok && !finfo.sp5fi_exists ) {
        // if the files does not exist yet, we create it
        tsp00_Int4 fileHandle;
        sqlfopenc(
            m_PropertiesFile.CharPtr(),
            sp5vf_text,
            sp5vf_write,
            sp5bk_buffered,
            &fileHandle,
            &ferror);
        sqlfclosec( fileHandle, sp5vf_close_normal, &ferror);            
        // something went wrong
        if( ferror.sp5fe_result != vf_ok ) {
            m_OutList.Overrule(
                Msg_List(Msg_List::Error,
                SDBMSG_DBMED_CREATECONFIGFILE,
                Msg_Arg("DISPCONFIG",m_PropertiesFile.CharPtr())));
            return false;
        }
        else
            createdFile = true;
    }

    DBMCliEvtDisp_Properties propertiesObject(m_PropertiesFile.CharPtr());
    if( !propertiesObject.load(m_OutList) ) {
        m_OutList.Overrule(
            Msg_List(Msg_List::Error,
            SDBMSG_DBMED_LOADCONFIGFILE,
            Msg_Arg("DISPCONFIG",m_PropertiesFile.CharPtr())));
        return false;
    }
    
    Tools_DynamicUTF8String versionKey(DBMCliEvtDisp_Properties::FileVersionKey);
    if( createdFile ) {
        propertiesObject.setProperty(versionKey, Tools_DynamicUTF8String(DBMVI_Strings::FormatedBuildVersion));
    }

    m_AllProps = propertiesObject.getAllProperties();
    if( m_AllProps == NULL ) {
        m_OutList.Overrule(
            Msg_List(Msg_List::Error,
            SDBMSG_DBMED_LOADCONFIGFILE,
            Msg_Arg("DISPCONFIG",m_PropertiesFile.CharPtr())));
        return false;
    }

    int lastIndex(-1);
    int currIndex(-1);
    for( Tools_List<Tools_Property*>::iterator it(m_AllProps->begin()); it != m_AllProps->end(); ++it ) {
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

        // smallest free index calculation
        currIndex = atoi(currentKey.SubStrBasis(0, firstDot).CharPtr());
        if( currIndex > lastIndex+1 ) {
            // found a slot
            ++lastIndex;
            break;
        }
        else
            lastIndex = currIndex;
    }
    if( lastIndex == currIndex )
        ++lastIndex;

    // leading zeros, 10 digits altogether
    m_DispatchingCondition = Tools_DynamicUTF8String(SAPDB_ToString(lastIndex, 10, _T_z)).Append(Tools_DynamicUTF8String(".")).Append(m_DispatchingCondition);

    // add the new command
    propertiesObject.setProperty(m_DispatchingCondition, m_CommandLine);
    if( !propertiesObject.store(m_OutList) ) {
        m_OutList.Overrule(
            Msg_List(Msg_List::Error,
            SDBMSG_DBMED_SAVECONFIGFILE,
            Msg_Arg("DISPCONFIG",m_PropertiesFile.CharPtr())));
        return false;
    }

    // print a reply
    m_OutList.Overrule(
            Msg_List(Msg_List::Error,
            SDBMSG_DBMED_CMDADD_REPLY,
            Msg_Arg("DISPENTRYID",lastIndex),
            Msg_Arg("DISPCONFIG",m_PropertiesFile.CharPtr())));
    return true;
}