/*!
    \file    ToolsParsersOldSyntEl_Keyword.cpp
    \author  TiloH
    \ingroup backup tool simulators for tests
    \brief   implementation of a class for key words in command lines 

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

#include "ToolsCommon/Parsers/Old/SyntaxElements/ToolsParsersOldSyntEl_Keyword.hpp"

ToolsParsersOldSyntEl_Keyword::ToolsParsersOldSyntEl_Keyword(const char* keyword, bool caseInsensitive)
        : ToolsParsersOldSyntEl_ParameterBase(),
          m_CaseInsensitive(caseInsensitive) {
    if(!m_Keyword.allocCopy(keyword))
        valid=false;
}

ToolsParsersOldSyntEl_Keyword::~ToolsParsersOldSyntEl_Keyword()
{
}

bool ToolsParsersOldSyntEl_Keyword::parse(const char * const * arrayToParse,
                                     const int            sizeOfArray,
                                     int                & parsedUpTo,
                                     bool               & foundSyntaxError)
{
    parsedUpTo=0;           //nothing parsed so far
    foundSyntaxError=false; //no syntax error found so far

    if(valid)
    {
        if(1>sizeOfArray)   //nothing to parse, but this flag is not optional!
            foundSyntaxError=true;
        else
        {
            if(keywordEquals(arrayToParse[0]))
            {
                found=true;
                parsedUpTo=1;
            }
            else
                foundSyntaxError=true; //the flag should have been there, but it is not!
        }
    }

    return valid; //we can not encounter any further memory problems during flag parsing, but an invalid object can not be used for parsing
}

bool ToolsParsersOldSyntEl_Keyword::simulateParse(
    const char * const * arrayToParse,
    const int            sizeOfArray,
    int                & parsedUpTo,
    bool               & foundSyntaxError,
    bool               & foundSomething)
{
    parsedUpTo=0;
    foundSyntaxError=true;
    foundSomething=false;
    
    if(1<=sizeOfArray)   //this flag is not optional, so there must be at least one element left in the array
    {
        if(keywordEquals(arrayToParse[0]))
        {
            parsedUpTo=1;
            foundSyntaxError=false;
            foundSomething=true;
        }
    }
    return valid;
}

bool ToolsParsersOldSyntEl_Keyword::printSyntax(char* outText, size_t spaceLeft) {
    if( strlen(m_Keyword) <= spaceLeft) {
        strcat(outText, m_Keyword);
        spaceLeft -= strlen(m_Keyword);
        return true;
    }
    else
        return false;
}
