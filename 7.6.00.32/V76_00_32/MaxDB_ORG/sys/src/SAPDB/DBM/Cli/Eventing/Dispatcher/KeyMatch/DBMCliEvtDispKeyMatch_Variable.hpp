/*!
  @file           DBMCliEvtDispKeyMatch_Variable.hpp
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

#ifndef DBMCliEvtDispKeyMatch_Variable_hpp
#define DBMCliEvtDispKeyMatch_Variable_hpp

#include "ToolsCommon/Tools_Expression.hpp"

class DBMCliEvtDispKeyMatch_Variable: public Tools_ExpressionValue {
public:
    DBMCliEvtDispKeyMatch_Variable(const char* varName);
    DBMCliEvtDispKeyMatch_Variable(const char* varName, ExType varType);
    DBMCliEvtDispKeyMatch_Variable(const char* varName, const ExDouble& initValue);
    DBMCliEvtDispKeyMatch_Variable(const char* varName, const ExString& initValue);
    DBMCliEvtDispKeyMatch_Variable(const char* varName, const ExDouble& initValue, ExType varType);
    DBMCliEvtDispKeyMatch_Variable(const char* varName, const ExString& initValue, ExType varType);

    ~DBMCliEvtDispKeyMatch_Variable() {}

    const Tools_DynamicUTF8String& getName() const {return m_Name;}

private:
    Tools_DynamicUTF8String m_Name;
};

inline DBMCliEvtDispKeyMatch_Variable::DBMCliEvtDispKeyMatch_Variable(
        const char* varName)
      : Tools_ExpressionValue(),
        m_Name(varName){
}

inline DBMCliEvtDispKeyMatch_Variable::DBMCliEvtDispKeyMatch_Variable(
        const char* varName,
        ExType varType)
      : Tools_ExpressionValue(varType),
        m_Name(varName){
}

inline DBMCliEvtDispKeyMatch_Variable::DBMCliEvtDispKeyMatch_Variable(
        const char* varName,
        const ExDouble& initValue)
      : Tools_ExpressionValue(initValue),
        m_Name(varName){
}

inline DBMCliEvtDispKeyMatch_Variable::DBMCliEvtDispKeyMatch_Variable(
        const char* varName,
        const ExString& initValue)
      : Tools_ExpressionValue(initValue),
        m_Name(varName){
}

inline DBMCliEvtDispKeyMatch_Variable::DBMCliEvtDispKeyMatch_Variable(
        const char* varName,
        const ExDouble& initValue,
        ExType varType)
      : Tools_ExpressionValue(initValue, varType),
        m_Name(varName){
}

inline DBMCliEvtDispKeyMatch_Variable::DBMCliEvtDispKeyMatch_Variable(
        const char* varName,
        const ExString& initValue,
        ExType varType)
      : Tools_ExpressionValue(initValue, varType),
        m_Name(varName){
}

#endif //DBMCliEvtDispKeyMatch_Variable_hpp
