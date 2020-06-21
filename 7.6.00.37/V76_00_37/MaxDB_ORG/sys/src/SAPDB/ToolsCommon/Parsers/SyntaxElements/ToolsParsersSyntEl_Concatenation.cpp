/*!
    \file    ToolsParsersSyntEl_Concatenation.cpp
    \author  TiloH
    \ingroup syntax elements for parsers
    \brief   implementation of a class for handling a list of succeeding
             parameters 

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

#include "ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_Concatenation.hpp"


bool ToolsParsersSyntEl_Concatenation::parse(
    const char * const * arrayToParse,
    const int            sizeOfArray,
    int                & parsedUpTo,
    bool               & foundSyntaxError)
{
    parsedUpTo=0;           //nothing parsed so far
    foundSyntaxError=false; //no syntax error found so far
    found=false;

    if(valid)
    {
        if(list.empty())    //nothing always be found without parsing anything
            found=true;
        else
        {
            for(Tools_List<ToolsParsersSyntEl_ParameterBase *>::iterator i=list.begin(); valid && !foundSyntaxError && i!=list.end(); i++)
            {
                int argsParsed=0;

                valid=(*i)->parse(arrayToParse+parsedUpTo, sizeOfArray-parsedUpTo, argsParsed, foundSyntaxError);

                parsedUpTo+=argsParsed;
            }

            if(!foundSyntaxError)
                found=true;
        }
    }

    return valid; //report any memory problems of children during alternative list parsing
}

bool ToolsParsersSyntEl_Concatenation::simulateParse(
    const char * const * arrayToParse,
    const int            sizeOfArray,
    int                & parsedUpTo,
    bool               & foundSyntaxError,
    bool               & foundSomething)
{
    parsedUpTo=0; //nothing parsed so far
    foundSyntaxError=false;
    foundSomething=false;
  
    if(valid && !list.empty())
    {
        for(Tools_List<ToolsParsersSyntEl_ParameterBase *>::iterator i=list.begin(); valid && !foundSyntaxError && i!=list.end(); i++)
        {
            bool tempFoundSomething=false;
            int  tempParsedUpTo=0;

            valid=(*i)->simulateParse(arrayToParse+parsedUpTo, sizeOfArray-parsedUpTo, tempParsedUpTo, foundSyntaxError, tempFoundSomething);
            if( valid && !foundSyntaxError )
                parsedUpTo += tempParsedUpTo;
        }

        if(!foundSyntaxError)
            foundSomething=true;
    }

    return valid; //report any memory problems of children during simulated concatenation list parsing
}

void ToolsParsersSyntEl_Concatenation::printSyntax(
    PrintSyntaxStyle    style,
    bool                isOuterElement,
    char             *& position,
    size_t            & spaceLeft,
    size_t            & spaceNeeded,
    bool              & spaceTooSmall) const
{
    bool   passedFirstElement=false;
    size_t numberListElements=list.size();
    bool   printedOpeningBracket=false;
    bool   childrenAreOuter=false;
    
    if(1>=numberListElements)
        childrenAreOuter=true;

    //do not print opening bracket, if style is simple and list contains just one element
    if(ToolsParsersSyntEl_ParameterBase::SimpleSyntax!=style ||
       (!isOuterElement && numberListElements>1))
    {
        printChar('(', position, spaceLeft, spaceNeeded, spaceTooSmall);

        printedOpeningBracket=true;
    }

    //print all elements separated by ' '
    for(Tools_List<ToolsParsersSyntEl_ParameterBase *>::const_iterator i=list.begin(); i!=list.end(); i++)
    {
        if(passedFirstElement)
            printChar(' ', position, spaceLeft, spaceNeeded, spaceTooSmall);
        else
            passedFirstElement=true;

        (*i)->printSyntax(style, childrenAreOuter, position, spaceLeft, spaceNeeded, spaceTooSmall);
    }

    //print closing bracket only if opening bracket was printed
    if(printedOpeningBracket)
        printChar(')', position, spaceLeft, spaceNeeded, spaceTooSmall);
}
