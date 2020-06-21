/*!
    \file    ToolsParsersOldSyntEl_Repetition.cpp
    \author  TiloH
    \ingroup backup tool simulators for tests
    \brief   implementation of a class for handling a list of self repeating parameters 

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

#include "ToolsCommon/Parsers/Old/SyntaxElements/ToolsParsersOldSyntEl_Repetition.hpp"

ToolsParsersOldSyntEl_Repetition::ToolsParsersOldSyntEl_Repetition(ToolsParsersOldSyntEl_ParameterBase* repeatedElement)
        : ToolsParsersOldSyntEl_ParameterBase(),
        m_RepeatedElement(repeatedElement) {
}


bool ToolsParsersOldSyntEl_Repetition::parse(
        const char * const * arrayToParse,
        const int            sizeOfArray,
        int                & parsedUpTo,
        bool               & foundSyntaxError) {

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

bool ToolsParsersOldSyntEl_Repetition::simulateParse(
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

bool ToolsParsersOldSyntEl_Repetition::printSyntax(char* outText, size_t spaceLeft) {
    if( spaceLeft >= 1 ) {
        strcat(outText, "(");
        --spaceLeft;
        if( m_RepeatedElement->printSyntax(outText, spaceLeft) ) {
            if( spaceLeft >= 2 ) {
                strcat(outText, ")+");
                spaceLeft -= 2;
                return true;
            }
        }
    }
    return false;
}

