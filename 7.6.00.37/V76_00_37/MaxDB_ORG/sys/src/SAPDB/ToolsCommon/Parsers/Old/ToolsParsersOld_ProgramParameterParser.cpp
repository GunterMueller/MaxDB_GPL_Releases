/*!
    \file    ToolsParsersOld_ProgramParameterParser.cpp
    \author  TiloH
    \ingroup backup tool simulators for tests
    \brief   implementation of a parser for parameters

\if EMIT_LICENCE

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

\endif
*/

#include "ToolsCommon/Parsers/Old/ToolsParsersOld_ProgramParameterParser.hpp"


ToolsParsersOld_ProgramParameterParser::ToolsParsersOld_ProgramParameterParser(
    int                               argc,
    const char * const              * argv,
    ToolsParsersOldSyntEl_ParameterBase * syntaxTree)
    :argC(argc),
    argV(argv),
    m_SyntaxTree(syntaxTree)
{
}

/*
void ToolsParsersOld_ProgramParameterParser::setParameterToParse(ToolsParsersOldSyntEl_ParameterGroup * parameterTree)
{
    this->parameterTree=parameterTree;
}*/

bool ToolsParsersOld_ProgramParameterParser::parse(bool & syntaxErrorFound)
{
    bool                 rc=true;
    int                  numberOfArgsLeft=argC-1; // -1 -> do not parse the program name
    const char * const * args=argV+1;             // +1 -> do not parse the program name
    int                  argsParsed=0;

    if(numberOfArgsLeft<0)
        numberOfArgsLeft=0;

    rc= m_SyntaxTree->parse(args, numberOfArgsLeft, argsParsed, syntaxErrorFound);
    syntaxErrorFound = syntaxErrorFound || (argsParsed < numberOfArgsLeft);

/*
    for(Tools_List<ToolsParsersOldSyntEl_ParameterBase *>::iterator i=parameterTree.begin(); rc && !syntaxErrorFound && i!=parameterTree.end(); i++)
    {
        int argsParsed=0;

        rc=(*i)->parse(args, numberOfArgsLeft, argsParsed, syntaxErrorFound);

        numberOfArgsLeft-=argsParsed;
        args+=argsParsed;
    }*/

    return rc;
}
