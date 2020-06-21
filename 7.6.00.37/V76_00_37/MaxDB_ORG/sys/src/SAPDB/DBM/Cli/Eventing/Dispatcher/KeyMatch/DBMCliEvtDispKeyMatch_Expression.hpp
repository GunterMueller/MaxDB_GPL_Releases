/*!
  @file           DBMCliEvtDispKeyMatch_Expression.hpp
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

#ifndef DBMCliEvtDispKeyMatch_Expression_hpp
#define DBMCliEvtDispKeyMatch_Expression_hpp

#include "DBM/Cli/Eventing/Common/DBMCliEvtCm_Event.hpp"
#include "DBM/Cli/Eventing/Dispatcher/KeyMatch/DBMCliEvtDispKeyMatch_ValueProvider.hpp"
#include "DBM/Cli/Eventing/Dispatcher/KeyMatch/DBMCliEvtDispKeyMatch_Variable.hpp"

class DBMCliEvtDispKeyMatch_Expression {
public:
    DBMCliEvtDispKeyMatch_Expression(const char* expressionString, const char* commandLine);
    ~DBMCliEvtDispKeyMatch_Expression() {};

    bool matchesWith( const DBMCliEvtCm_Event& theEvent );
    
    // should return a const reference...
    Tools_DynamicUTF8String& getCommandLine() {return m_CommandLine;}

    static const char KeySeparator;
    static const char* const OperatorLikeText;
    static const char* const OperatorEqualsText;

private:
    Tools_DynamicUTF8String m_CommandLine;
    DBMCliEvtDispKeyMatch_Variable m_EventName;
    DBMCliEvtDispKeyMatch_Variable m_EventPriority;
    DBMCliEvtDispKeyMatch_Variable m_EventValue1;
    DBMCliEvtDispKeyMatch_Variable m_EventValue2;

    DBMCliEvtDispKeyMatch_ValueProvider m_ValueProvider;
    Tools_Expression m_Expression;
};

#endif //DBMCliEvtDispKeyMatch_Expression_hpp
