/*!
    \file    ToolsParsersOldSyntEl_Variable.cpp
    \author  TiloH
    \ingroup backup tool simulators for tests
    \brief   implementation of a class for variable command line parts 

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

#include "ToolsCommon/Parsers/Old/SyntaxElements/ToolsParsersOldSyntEl_Variable.hpp"

ToolsParsersOldSyntEl_Variable::ToolsParsersOldSyntEl_Variable(
        const char * syntaxToken,
        const char* defaultValue)
        : ToolsParsersOldSyntEl_ParameterBase() {
    if(!name.allocCopy(syntaxToken))
        valid=false;

    if( 0 != defaultValue ) {
        if(!value.allocCopy(defaultValue))
            valid=false;
    }
}

bool ToolsParsersOldSyntEl_Variable::parse(const char * const * arrayToParse,
                                      const int            sizeOfArray,
                                      int                & parsedUpTo,
                                      bool               & foundSyntaxError)
{
    parsedUpTo=0; //nothing parsed so far
    foundSyntaxError=false; //no syntax error found so far

    if(isValid())
    {
        if(1>sizeOfArray)   //nothing to parse, but this variable is not optional!
            foundSyntaxError=true;
        else
        {
            if(value.allocCopy(arrayToParse[0]))
            {
                parsedUpTo=1;
                found=1;
            }
            else
                valid=false;
        }
    }

    return isValid(); //an invalid object can not be used for parsing
}

bool ToolsParsersOldSyntEl_Variable::simulateParse(const char * const * arrayToParse,
                                                       const int            sizeOfArray,
                                                       int                & parsedUpTo,
                                                       bool               & foundSyntaxError,
                                                       bool               & foundSomething)
{
    if(1>sizeOfArray)   //nothing to parse, but this variable is not optional!
    {
        parsedUpTo=0;
        foundSyntaxError=true;
        foundSomething=0;
    }
    else
    {
        parsedUpTo=1;
        foundSyntaxError=false;
        foundSomething=1;
    }

    return valid;
}

bool ToolsParsersOldSyntEl_Variable::printSyntax(char* outText, size_t spaceLeft) {
    if( spaceLeft >= 1 ) {
        strcat(outText, "<");
        --spaceLeft;
    }
    else
        return false;

    if( strlen(name) <= spaceLeft) {
        strcat(outText, name);
        spaceLeft -= strlen(name);
    }
    else
        return false;

    if( spaceLeft >= 1 ) {
        strcat(outText, ">");
        --spaceLeft;
        return true;
    }
    else
        return false;
}