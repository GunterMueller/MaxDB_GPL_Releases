/*!
    \file    ToolsParsersSyntEl_ParameterBase.cpp
    \author  TiloH
    \ingroup syntax elements for parsers
    \brief   implementation of a base class for command line parts 

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

#include "SAPDBCommon/SAPDB_string.h"
#include "ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_ParameterBase.hpp"


ToolsParsersSyntEl_ParameterBase::ToolsParsersSyntEl_ParameterBase(
    )
  : valid(true),
    found(false)
{
}

ToolsParsersSyntEl_ParameterBase::~ToolsParsersSyntEl_ParameterBase()
{
}

bool ToolsParsersSyntEl_ParameterBase::isValid() const
{
    return valid;
}

bool ToolsParsersSyntEl_ParameterBase::isSet() const
{
    return found;
}

void ToolsParsersSyntEl_ParameterBase::printChar(
    char      character,
    char   *& position,
    size_t  & spaceLeft,
    size_t  & spaceNeeded,
    bool    & spaceTooSmall) const
{
    spaceNeeded++;

    if(1<spaceLeft)
    {
        position[0]=character;
        position[1]='\0';

        spaceLeft--;
        position++;
    }
    else
        spaceTooSmall=true;
}

void ToolsParsersSyntEl_ParameterBase::printString(
        const char  * string,
        char       *& position,
        size_t      & spaceLeft,
        size_t      & spaceNeeded,
        bool        & spaceTooSmall) const
{
    size_t lengthString=strlen(string);

    spaceNeeded+=lengthString;

    if(lengthString<spaceLeft)
    {
        SAPDB_memmove(position, string, lengthString+1);

        spaceLeft-=lengthString;
        position+=lengthString;
    }
    else
        spaceTooSmall=true;
}
