/*!
    \file    ToolsParsersOldSyntEl_AlternativeList.cpp
    \author  TiloH
    \ingroup backup tool simulators for tests
    \brief   implementation of a class for handling a list of alternative parameters 

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

#include "ToolsCommon/Parsers/Old/SyntaxElements/ToolsParsersOldSyntEl_AlternativeList.hpp"


bool ToolsParsersOldSyntEl_AlternativeList::parse(const char * const * arrayToParse,
                                             const int            sizeOfArray,
                                             int                & parsedUpTo,
                                             bool               & foundSyntaxError)
{
    parsedUpTo=0;           //nothing parsed so far
    foundSyntaxError=false; //no syntax error found so far
    found=false;            //

    if(valid)
    {
        if(list.empty())    //no alternatives can always be found without parsing anything
            found=true;
        else
        {
            //simulate parsing for all members of the list, until first is found (so ordering in list can be important)
            for(Tools_List<ToolsParsersOldSyntEl_ParameterBase *>::iterator i=list.begin(); valid && i!=list.end(); i++)
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

bool ToolsParsersOldSyntEl_AlternativeList::simulateParse(
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
        for(Tools_List<ToolsParsersOldSyntEl_ParameterBase *>::iterator i=list.begin(); valid && i!=list.end(); i++)
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

bool ToolsParsersOldSyntEl_AlternativeList::printSyntax(char* outText, size_t spaceLeft) {
    if( spaceLeft >= 1 ) {
        strcat(outText, "(");
        --spaceLeft;
    }
    else
        return false;

    bool printSpace(false);
    for(Tools_List<ToolsParsersOldSyntEl_ParameterBase *>::iterator i=list.begin(); i!=list.end(); i++) {
        if( spaceLeft <= 0 )
            return false;
        else if( printSpace && spaceLeft >= 1 ) {
            strcat(outText, "|");
            --spaceLeft;
        }

        if( !(*i)->printSyntax(outText, spaceLeft))
            return false;
        else {
            printSpace = true;
        }
    }

    if( spaceLeft >= 1 ) {
        strcat(outText, ")");
        --spaceLeft;
        return true;
    }
    else
        return false;
}

