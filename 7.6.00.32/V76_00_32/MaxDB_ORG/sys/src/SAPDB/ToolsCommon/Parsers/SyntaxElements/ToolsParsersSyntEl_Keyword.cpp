/*!
    \file    ToolsParsersSyntEl_Keyword.cpp
    \author  TiloH
    \ingroup syntax elements for parsers
    \brief   implementation of a class for key words in command lines 

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

#include "ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_Keyword.hpp"

ToolsParsersSyntEl_Keyword::ToolsParsersSyntEl_Keyword(
    const char* keyword)
  :ToolsParsersSyntEl_Terminal(keyword)
{
}

ToolsParsersSyntEl_Keyword::ToolsParsersSyntEl_Keyword(
    const char * startKeyword,
    const char * endKeyword)
  : ToolsParsersSyntEl_Terminal(startKeyword, endKeyword)
{
}


bool ToolsParsersSyntEl_Keyword::parse(const char * const * arrayToParse,
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
            if(m_Name.equals(arrayToParse[0]))
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

bool ToolsParsersSyntEl_Keyword::simulateParse(
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
        if(m_Name.equals(arrayToParse[0]))
        {
            parsedUpTo=1;
            foundSyntaxError=false;
            foundSomething=true;
        }
    }
    return valid;
}

void ToolsParsersSyntEl_Keyword::printSyntax(
        PrintSyntaxStyle    style,
        bool                isOuterElement,
        char             *& position,
        size_t            & spaceLeft,
        size_t            & spaceNeeded,
        bool              & spaceTooSmall) const
{
    printString(m_Name, position, spaceLeft, spaceNeeded, spaceTooSmall);
}
