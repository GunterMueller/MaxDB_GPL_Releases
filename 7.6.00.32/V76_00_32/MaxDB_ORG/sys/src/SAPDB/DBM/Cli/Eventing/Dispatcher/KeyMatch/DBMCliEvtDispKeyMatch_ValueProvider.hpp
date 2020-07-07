/*!
  @file           DBMCliEvtDispKeyMatch_ValueProvider.hpp
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

#ifndef DBMCliEvtDispKeyMatch_ValueProvider_hpp
#define DBMCliEvtDispKeyMatch_ValueProvider_hpp

#include "DBM/Cli/Eventing/Dispatcher/KeyMatch/DBMCliEvtDispKeyMatch_Variable.hpp"
#include "ToolsCommon/Tools_Expression.hpp"
#include "ToolsCommon/Tools_Vector.hpp"

class DBMCliEvtDispKeyMatch_ValueProvider: public Tools_ValueProvider {
public:
    DBMCliEvtDispKeyMatch_ValueProvider() {}
    ~DBMCliEvtDispKeyMatch_ValueProvider() {}

    void addVariable(DBMCliEvtDispKeyMatch_Variable* newVariable);

    Tools_ExpressionValue* AskForValue(const Tools_DynamicUTF8String& variableName);
    Tools_ExpressionValueArray * AskForArray(const Tools_DynamicUTF8String& arrayName) {
        return NULL;
    }

private:
    Tools_Vector<DBMCliEvtDispKeyMatch_Variable*> m_Variables;
};

inline Tools_ExpressionValue* DBMCliEvtDispKeyMatch_ValueProvider::AskForValue(
        const Tools_DynamicUTF8String& variableName) {
    for(Tools_Vector<DBMCliEvtDispKeyMatch_Variable*>::iterator i=m_Variables.begin(); i!=m_Variables.end(); ++i) {
        if( variableName == (*i)->getName() )
            return (*i);
    }
    return NULL;
}

inline void DBMCliEvtDispKeyMatch_ValueProvider::addVariable(
        DBMCliEvtDispKeyMatch_Variable* newVariable) {
    m_Variables.push_back(newVariable);
}

#endif //DBMCliEvtDispKeyMatch_ValueProvider_hpp
