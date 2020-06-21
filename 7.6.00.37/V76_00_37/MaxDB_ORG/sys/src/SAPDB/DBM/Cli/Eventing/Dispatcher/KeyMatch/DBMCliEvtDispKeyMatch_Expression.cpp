/*!
  @file           DBMCliEvtDispKeyMatch_Expression.cpp
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

#include "DBM/Cli/Eventing/Common/DBMCliEvtCm_Event.hpp"
#include "DBM/Cli/Eventing/Dispatcher/KeyMatch/DBMCliEvtDispKeyMatch_Expression.hpp"

const char DBMCliEvtDispKeyMatch_Expression::KeySeparator('.');
const char* const DBMCliEvtDispKeyMatch_Expression::OperatorLikeText("like");
const char* const DBMCliEvtDispKeyMatch_Expression::OperatorEqualsText("==");

DBMCliEvtDispKeyMatch_Expression::DBMCliEvtDispKeyMatch_Expression(
        const char* expressionString,
        const char* commandLine)
      : m_CommandLine(commandLine),
        m_EventName(DBMCliEvtCm_Event::AttribName, Tools_ExpressionValue::ExTypeString),
        m_EventPriority(DBMCliEvtCm_Event::AttribPriority, Tools_ExpressionValue::ExTypeNumeric),
        m_EventValue1(DBMCliEvtCm_Event::AttribValue1, Tools_ExpressionValue::ExTypeNumeric),
        m_EventValue2(DBMCliEvtCm_Event::AttribValue2, Tools_ExpressionValue::ExTypeNumeric),
        m_Expression(m_ValueProvider) {

    m_ValueProvider.addVariable(&m_EventName);
    m_ValueProvider.addVariable(&m_EventPriority);
    m_ValueProvider.addVariable(&m_EventValue1);
    m_ValueProvider.addVariable(&m_EventValue2);

    Tools_DynamicUTF8String temp(expressionString);
    // replace
    Tools_DynamicUTF8String::BasisElementIndex pos(temp.Find(KeySeparator)); // remove the dot stuff
    if( pos != Tools_DynamicUTF8String::NPos )
        temp = temp.SubStr(pos+1);

    m_Expression.SetExpression(temp);
}
    
bool DBMCliEvtDispKeyMatch_Expression::matchesWith(
        const DBMCliEvtCm_Event& theEvent ) {

    // set variable values from event object
    m_EventName.SetNewValue(Tools_ExpressionValue::ExString((const char*)theEvent.GetName()));
    m_EventPriority.SetNewValue((Tools_ExpressionValue::ExInt)theEvent.GetPriority());
    m_EventValue1.SetNewValue((Tools_ExpressionValue::ExInt)theEvent.GetValue1());
    m_EventValue2.SetNewValue((Tools_ExpressionValue::ExInt)theEvent.GetValue2());

    // that's all     
    m_Expression.ParseExpression();
    Tools_ExpressionValue isMatching(Tools_ExpressionValue::ExTypeNumeric);
    m_Expression.Value(isMatching);
    return isMatching.GetIntegerValue() != 0;
}
