/*!
    \file    ToolsParsersSyntEl_Invalid.cpp
    \author  TiloH
    \ingroup syntax elements for parsers
    \brief   implementation of a class for an unusable elements 

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


#include "ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_Invalid.hpp"


bool ToolsParsersSyntEl_Invalid::parse(
    const char * const * arrayToParse,
    const int            sizeOfArray,
    int                & parsedUpTo,
    bool               & foundSyntaxError)
{
    parsedUpTo=0;
    foundSyntaxError=true;

    return true;
}

bool ToolsParsersSyntEl_Invalid::simulateParse(
        const char * const * arrayToParse,
        const int            sizeOfArray,
        int                & parsedUpTo,
        bool               & foundSyntaxError,
        bool               & foundSomething)
{
    parsedUpTo=0;
    foundSyntaxError=true;
    foundSomething=false;

    return true;
}

void ToolsParsersSyntEl_Invalid::printSyntax(
        PrintSyntaxStyle    style,
        bool                isOuterElement,
        char             *& position,
        size_t            & spaceLeft,
        size_t            & spaceNeeded,
        bool              & spaceTooSmall) const
{
    //just do nothing
}
