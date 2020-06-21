/*!
    \file    ToolsParsersSyntEl_Set.cpp
    \author  MarcW
    \ingroup syntax elements for parsers
    \brief   implementation of a class for handling a list of parameters
             succeeding each other in arbitrary order

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

#include "ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_Set.hpp"


bool ToolsParsersSyntEl_Set::parse(
        const char* const* arrayToParse,
        const int sizeOfArray,
        int& parsedUpTo,
        bool& foundSyntaxError) {

    parsedUpTo=0;           //nothing parsed so far
    foundSyntaxError=false; //no syntax error found so far
    found=false;

    if( valid ) {
        if( list.empty() )    //nothing always be found without parsing anything
            found=true;
        else {
            Tools_List<ToolsParsersSyntEl_ParameterBase *>::iterator parsee(list.begin());
            
            for(; parsee != list.end() && valid && !foundSyntaxError; ++parsee ) {
                Tools_List<ToolsParsersSyntEl_ParameterBase *>::iterator swapPartner(parsee);
                for(; swapPartner != list.end() && valid; ++swapPartner ) {
                    // in first loop parsee and swapPartner are equal...
                    ToolsParsersSyntEl_ParameterBase* tmp = *parsee;
                    *parsee = *swapPartner;
                    *swapPartner = tmp;
                    bool foundSomething(false);
                    int tempParsedUpTo(0);

                    if( (*parsee)->simulateParse(arrayToParse+parsedUpTo, sizeOfArray-parsedUpTo, tempParsedUpTo, foundSyntaxError, foundSomething) ) {
                        if( foundSyntaxError || (tempParsedUpTo == 0 && foundSomething) ) {
                            // could not parse this element, try other or
                            // parsee is optional and not here. consider this situation as if it was not optional and try later...
                        }
                        else {
                            if( foundSomething ) {
                                int argsParsed(0);
                                valid = (*parsee)->parse(arrayToParse+parsedUpTo, sizeOfArray-parsedUpTo, argsParsed, foundSyntaxError);
                                parsedUpTo += argsParsed;
                                break;
                            }
                        }
                    }
                }
            }
            if( !foundSyntaxError )
                found=true;
        }
    }
    return valid; //report any memory problems of children during alternative list parsing
}

bool ToolsParsersSyntEl_Set::simulateParse(
    const char * const * arrayToParse,
    const int            sizeOfArray,
    int                & parsedUpTo,
    bool               & foundSyntaxError,
    bool               & foundSomething)
{
    parsedUpTo=0; //nothing parsed so far
    foundSyntaxError=false;
    foundSomething=false;
  
    if( valid ) {
        if( list.empty() )
            return true;
        else {
            Tools_List<ToolsParsersSyntEl_ParameterBase *>::iterator parsee(list.begin());
            for(; parsee != list.end() && valid && !foundSyntaxError; ++parsee ) {
                Tools_List<ToolsParsersSyntEl_ParameterBase *>::iterator swapPartner(parsee);    
                for(; swapPartner != list.end() && valid; ++swapPartner ) {
                    // in first loop parsee and swapPartner are equal...
                    ToolsParsersSyntEl_ParameterBase* tmp = *parsee;
                    *parsee = *swapPartner;
                    *swapPartner = tmp;

                    bool tempFoundSomething(false);
                    int  tempParsedUpTo(0);

                    valid=(*parsee)->simulateParse(arrayToParse+parsedUpTo, sizeOfArray-parsedUpTo, tempParsedUpTo, foundSyntaxError, tempFoundSomething);
                    if( valid && !foundSyntaxError ) {
                        parsedUpTo += tempParsedUpTo;
                        break;
                    }

                }
            }
            if(!foundSyntaxError)
                foundSomething=true;
        }
    }

    return valid; //report any memory problems of children during simulated set parsing
}

void ToolsParsersSyntEl_Set::printSyntax(
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

    //print opening, round bracket only during debug mode, not in simple mode
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
    
    //if opening bracket was printed, print closing round bracket followed by an exclamation mark, 
    if(printedOpeningBracket)
    {
        if(ToolsParsersSyntEl_ParameterBase::SimpleSyntax==style)
            printChar(')', position, spaceLeft, spaceNeeded, spaceTooSmall);
        else
            printString(")!", position, spaceLeft, spaceNeeded, spaceTooSmall);
    }
}

