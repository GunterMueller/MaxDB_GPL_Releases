/*!
    \file    ToolsParsersSyntEl_AlternativeList.cpp
    \author  TiloH
    \ingroup syntax elements for parsers
    \brief   implementation of a class for handling a list of alternative
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

#include "ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_AlternativeList.hpp"


bool ToolsParsersSyntEl_AlternativeList::parse(const char * const * arrayToParse,
                                             const int            sizeOfArray,
                                             int                & parsedUpTo,
                                             bool               & foundSyntaxError)
{
    parsedUpTo=0;           //nothing parsed so far
    foundSyntaxError=false; //no syntax error found so far
    found=false;

    if(valid)
    {
        if(list.empty())    //no alternatives can always be found without parsing anything
            found=true;
        else
        {
            //simulate parsing for all members of the list, until first is found (so ordering in list can be important)
            for(Tools_List<ToolsParsersSyntEl_ParameterBase *>::iterator i=list.begin(); valid && i!=list.end(); i++)
            {
                bool tempSyntaxError=false;
                bool tempFound=false;
                int  tempParsedUpTo=0;

                valid=(*i)->simulateParse(arrayToParse, sizeOfArray, tempParsedUpTo, tempSyntaxError, tempFound);

                //if a alternative could be parsed, the list was found in the parameters 
                if(valid && !tempSyntaxError && tempFound)
                    found=true;

                //if a non-empty alternative could be parsed, we have the alternative we were looking for, end further parsing
                if(valid && !tempSyntaxError && 0<tempParsedUpTo) //we have found something allowed (could be also something not exsiting, but optional)
                {
                    valid=(*i)->parse(arrayToParse, sizeOfArray, parsedUpTo, foundSyntaxError);

                    break;
                }
            }

            if(!found)          //at least one of the alternatives should have been found (e.g. an empty one, if all were optional)
                foundSyntaxError=true;
        }
    }

    return valid; //report any memory problems of children during alternative list parsing
}

bool ToolsParsersSyntEl_AlternativeList::simulateParse(
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
        for(Tools_List<ToolsParsersSyntEl_ParameterBase *>::iterator i=list.begin(); valid && i!=list.end(); i++)
        {
            bool tempSyntaxError=false;
            bool tempFoundSomething=false;
            int  tempParsedUpTo=0;

            valid=(*i)->simulateParse(arrayToParse, sizeOfArray, tempParsedUpTo, tempSyntaxError, tempFoundSomething);

            if(tempFoundSomething)
                foundSomething=true;

            if(valid && !tempSyntaxError && 0<tempParsedUpTo) //we have found something allowed (could be also something not exsiting, but optional)
            {
                parsedUpTo=tempParsedUpTo;

                break;
            }
        }

        if(!foundSomething)          //none of the alternatives was optional, so at least one should have been found, but was not
            foundSyntaxError=true;
    }

    return valid; //report any memory problems of children during alternative list parsing
}

void ToolsParsersSyntEl_AlternativeList::printSyntax(
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

    //do not print opening bracket, if style us simple and list contains just one element
    if(ToolsParsersSyntEl_ParameterBase::SimpleSyntax!=style || //either syntax should be complete
       (!isOuterElement && numberListElements>1))               //or it is neither a outer element nor a list with 0 or 1 elements
    {
        printChar('(', position, spaceLeft, spaceNeeded, spaceTooSmall);

        printedOpeningBracket=true;
    }

    //print all elements separated by " | "
    for(Tools_List<ToolsParsersSyntEl_ParameterBase *>::const_iterator i=list.begin(); i!=list.end(); i++)
    {
        if(passedFirstElement)
            printString(" | ", position, spaceLeft, spaceNeeded, spaceTooSmall);
        else
            passedFirstElement=true;

        (*i)->printSyntax(style, childrenAreOuter, position, spaceLeft, spaceNeeded, spaceTooSmall);
    }

    //print closing bracket only if needed
    if(printedOpeningBracket)
        printChar(')', position, spaceLeft, spaceNeeded, spaceTooSmall);
}
