/*!
    \file    ToolsParsersSyntEl_Repetition.cpp
    \author  TiloH
    \ingroup syntax elements for parsers
    \brief   implementation of a class for handling a list of self repeating
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

#include "ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_Repetition.hpp"

ToolsParsersSyntEl_Repetition::ToolsParsersSyntEl_Repetition(
    ToolsParsersSyntEl_ParameterBase * repeatedElement)
  : ToolsParsersSyntEl_NonTerminal(),
    m_RepeatedElement(repeatedElement)
{
}

ToolsParsersSyntEl_Repetition::~ToolsParsersSyntEl_Repetition()
{
    if(0!=m_RepeatedElement && !m_RepeatedElement->isTerminal())
        delete m_RepeatedElement;
}

bool ToolsParsersSyntEl_Repetition::parse(
    const char * const * arrayToParse,
    const int            sizeOfArray,
    int                & parsedUpTo,
    bool               & foundSyntaxError)
{
    parsedUpTo = 0; //nothing parsed so far
    found = false;
    bool parsedSomething(true);

    if(sizeOfArray <= 0) {
        //nothing to parse, that's an error
        foundSyntaxError = true;
        parsedSomething = false;
    }
    else
        foundSyntaxError = false; //no syntax error found so far

    while( parsedSomething ) {
        //can only be true, if isValid() && !foundSyntaxError
        bool tempSyntaxError=false;
        bool tempFoundSomething=false;
        parsedSomething=false;

        //try to parse the element 
        int argsParsed=0;

        valid=m_RepeatedElement->simulateParse(arrayToParse+parsedUpTo,
                                    sizeOfArray-parsedUpTo,
                                    argsParsed,
                                    tempSyntaxError,
                                    tempFoundSomething);


        if( valid && !tempSyntaxError && 0<argsParsed ) {
            //no syntax error
            parsedSomething = true;

            //now parse it
            int argsParsed = 0;
            valid=m_RepeatedElement->parse(arrayToParse+parsedUpTo,
                                sizeOfArray-parsedUpTo,
                                argsParsed,
                                tempSyntaxError);

            if(valid) {
                parsedUpTo+=argsParsed;
                found=true;
            }
        }
    }

    if( !found )
        foundSyntaxError = true;

    return valid;
}

bool ToolsParsersSyntEl_Repetition::simulateParse(
    const char * const * arrayToParse,
    const int            sizeOfArray,
    int                & parsedUpTo,
    bool               & foundSyntaxError,
    bool               & foundSomething)
{
    parsedUpTo=0; //nothing parsed so far
    foundSyntaxError=false; //no syntax error found so far
    foundSomething=false;

    if(valid)
    {
        bool parsedSomething=true;
    
        while(parsedSomething) //can only be true, if isValid() && !foundSyntaxError
        {
            bool tempSyntaxError=false;
            bool tempFoundSomething=false; 

            parsedSomething=false;

            //simulate to parse the complete list without syntax error 
                int argsParsed=0;

                valid=m_RepeatedElement->simulateParse(arrayToParse+parsedUpTo,
                                            sizeOfArray-parsedUpTo,
                                            argsParsed,
                                            tempSyntaxError,
                                            tempFoundSomething);


            if(valid && !tempSyntaxError && 0<argsParsed) //no syntax error in previous rounds and in current round
            {
                parsedSomething=true;
                parsedUpTo+=argsParsed;
                foundSomething=true;
            }
        }

        if(!foundSomething)          //not one example of the parameters was found, but at least one should have been
            foundSyntaxError=true;
    }

    return valid;
}

void ToolsParsersSyntEl_Repetition::printSyntax(
        PrintSyntaxStyle    style,
        bool                isOuterElement,
        char             *& position,
        size_t            & spaceLeft,
        size_t            & spaceNeeded,
        bool              & spaceTooSmall) const
{
    printChar('{', position, spaceLeft, spaceNeeded, spaceTooSmall);
    m_RepeatedElement->printSyntax(style, true, position, spaceLeft, spaceNeeded, spaceTooSmall); //the one element within {...} is always the only element -> true
    printChar('}', position, spaceLeft, spaceNeeded, spaceTooSmall);

    //alternative code if repetitions are marked by "..." instead of "{ }"
    //m_RepeatedElement->printSyntax(style, false, position, spaceLeft, spaceNeeded, spaceTooSmall); //the one element within {...} is always the only element, but we use no brackets for the repetition -> false
    //printString("...", position, spaceLeft, spaceNeeded, spaceTooSmall);
}

