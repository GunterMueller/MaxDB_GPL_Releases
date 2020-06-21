/*!
  @file           DBMCliEvtDispCmd_Add.hpp
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

#ifndef DBMCliEvtDispCmd_Add_HPP
#define DBMCliEvtDispCmd_Add_HPP



#include "DBM/Cli/Eventing/Dispatcher/Commands/DBMCliEvtDispCmd_List.hpp"
#include "ToolsCommon/Parsers/Old/ToolsParsersOld_ProgramParameterParser.hpp"

class DBMCliEvtDispCmd_Add: public DBMCliEvtDispCmd_List {
public:
    DBMCliEvtDispCmd_Add(
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
            const char* commandLine);

    ~DBMCliEvtDispCmd_Add() {}

    bool run();

private:
    bool m_KeyValid;
    Tools_DynamicUTF8String m_DispatchingCondition;
    Tools_DynamicUTF8String m_CommandLine;
};
#endif //DBMCliEvtDispCmd_Add_HPP
