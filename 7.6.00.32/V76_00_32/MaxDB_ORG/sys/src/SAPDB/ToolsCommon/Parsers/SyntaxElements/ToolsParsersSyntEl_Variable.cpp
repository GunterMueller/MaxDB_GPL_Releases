/*!
    \file    ToolsParsersSyntEl_Variable.cpp
    \author  TiloH
    \ingroup syntax elements for parsers
    \brief   implementation of a class for variable command line parts 

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

#include "ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_Variable.hpp"

ToolsParsersSyntEl_Variable::ToolsParsersSyntEl_Variable(
    const char * syntaxToken)
  : ToolsParsersSyntEl_Terminal(syntaxToken)
{
    if(!m_value.allocCopy(""))
        valid=false;
}

ToolsParsersSyntEl_Variable::ToolsParsersSyntEl_Variable(
    const char * startSyntaxToken,
    const char * endSyntaxToken,
    const char * startDefault,
    const char * endDefault) 
  : ToolsParsersSyntEl_Terminal(startSyntaxToken, endSyntaxToken)
{
    if(0!=startDefault && 0!=endDefault && !m_value.allocCopy(startDefault, endDefault))
        valid=false;
}

const char * ToolsParsersSyntEl_Variable::getValue() const
{
    return (const char *)m_value;
}

bool ToolsParsersSyntEl_Variable::parse(
    const char * const * arrayToParse,
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
            if(m_value.allocCopy(arrayToParse[0]))
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

bool ToolsParsersSyntEl_Variable::simulateParse(
    const char * const * arrayToParse,
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

void ToolsParsersSyntEl_Variable::printSyntax(
        PrintSyntaxStyle    style,
        bool                isOuterElement,
        char             *& position,
        size_t            & spaceLeft,
        size_t            & spaceNeeded,
        bool              & spaceTooSmall) const
{
    size_t nameLength=strlen(m_Name);
    bool nameHasNoAngleBrackets=true;

    if(m_Name[0]=='<' && 1<nameLength && m_Name[nameLength-1]=='>')
        nameHasNoAngleBrackets=false;

    if(nameHasNoAngleBrackets)
        printChar('<', position, spaceLeft, spaceNeeded, spaceTooSmall);

    printString(m_Name, position, spaceLeft, spaceNeeded, spaceTooSmall);
        
    if(nameHasNoAngleBrackets)
        printChar('>', position, spaceLeft, spaceNeeded, spaceTooSmall);
}
