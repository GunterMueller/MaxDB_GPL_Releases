/*!
    \file    ToolsParsersSyntax_Symbol.cpp
    \author  TiloH
    \ingroup syntax for parser
    \brief   declaration of a class for handling symbols of a syntax string

    ========== licence begin  GPL
    Copyright (c) 1998-2006 SAP AG

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

#include "ToolsCommon/Parsers/Syntax/ToolsParsersSyntax_Symbol.hpp"


ToolsParsersSyntax_Symbol::ToolsParsersSyntax_Symbol(
    )
  : m_terminalObject(0),
    m_nonTerminal(0)
{
}

ToolsParsersSyntax_Symbol::ToolsParsersSyntax_Symbol(
    ToolsParsersSyntEl_ParameterBase * terminalObject)
  : m_terminalObject(terminalObject),
    m_nonTerminal(0)
{
}

ToolsParsersSyntax_Symbol::ToolsParsersSyntax_Symbol(
    char nonTerminal)
  : m_terminalObject(0),
    m_nonTerminal(nonTerminal)
{
}

bool ToolsParsersSyntax_Symbol::equals(char c) const
{
    return (0==m_terminalObject && c==m_nonTerminal);
}

bool ToolsParsersSyntax_Symbol::isTerminal() const
{
    return 0!=m_terminalObject;
}

ToolsParsersSyntEl_ParameterBase * ToolsParsersSyntax_Symbol::getTerminalObject()
{
    return m_terminalObject;
}

char ToolsParsersSyntax_Symbol::getNonTerminal() const
{
    return m_nonTerminal;
}
