/*!
    \file    ToolsParsers_ProgramParameterParser.cpp
    \author  TiloH
    \ingroup backup tool simulators for tests
    \brief   implementation of a parser for parameters

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

#include "ToolsCommon/Parsers/ToolsParsers_ProgramParameterParser.hpp"


ToolsParsers_ProgramParameterParser::ToolsParsers_ProgramParameterParser(
    int                  argc,
    const char * const * argv,
    const char *         syntax)
  : m_Syntax(syntax),
    m_ArgCount((0<argc && 0!=argv)?argc:1),
    m_ArgVector((0<argc && 0!=argv)?argv:m_DummyVector),
    m_notParsed(true)
{
    m_DummyVector[0]=""; //simulate the program name
    m_DummyVector[1]=0;  //last array element in argv is always a 0 pointer
}

const ToolsParsers_ParseResult & ToolsParsers_ProgramParameterParser::parse()
{
    if(m_notParsed)
    {
        m_notParsed=false; //do not parse more than once

        if(ToolsParsers_ParseResult::NoError==m_Syntax.getResult().getResultCode())
        {
            int  argsParsed=0;
            bool syntaxErrorFound=false;

            m_Syntax.getSyntax()->parse(m_ArgVector+1, m_ArgCount-1, argsParsed, syntaxErrorFound); // +1 -> do not parse the program name

            if(syntaxErrorFound ||          //no syntax error must be found during parsing
               argsParsed+1 < m_ArgCount)   //all arguments except the program name (m_ArgVector[0]) must be parsed
            {
                m_Syntax.getResult().setErrorSyntax();
            }
        }
    }

    return m_Syntax.getResult();
}

const ToolsParsersSyntax_Syntax & ToolsParsers_ProgramParameterParser::getSyntax(
        ) const
{
    return m_Syntax;
}
