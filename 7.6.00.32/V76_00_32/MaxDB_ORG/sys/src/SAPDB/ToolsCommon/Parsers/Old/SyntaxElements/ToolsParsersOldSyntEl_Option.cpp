/*!
    \file    ToolsParsersOldSyntEl_Option.cpp
    \author  TiloH
    \ingroup backup tool simulators for tests
    \brief   implementation of a class for handling a list of optional parameters 

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

#include "ToolsCommon/Parsers/Old/SyntaxElements/ToolsParsersOldSyntEl_Option.hpp"

ToolsParsersOldSyntEl_Option::ToolsParsersOldSyntEl_Option(ToolsParsersOldSyntEl_ParameterBase* optionalElement)
        : ToolsParsersOldSyntEl_ParameterBase(),
        m_OptionalElement(optionalElement) {
}

bool ToolsParsersOldSyntEl_Option::parse(const char * const * arrayToParse,
                                        const int            sizeOfArray,
                                        int                & parsedUpTo,
                                        bool               & foundSyntaxError)
{
    parsedUpTo=0; //nothing parsed so far
    foundSyntaxError=false; //no syntax error found so far

    if( valid ) {
        found=true;     //a list of optional arguments can always be found

        if( sizeOfArray > 0 ) {
            // there is something to parse
            int argsParsed = 0;
            bool tempFound = false;
            valid = m_OptionalElement->simulateParse(arrayToParse+parsedUpTo, sizeOfArray-parsedUpTo, argsParsed, foundSyntaxError, tempFound);

            if(valid) {
                if(foundSyntaxError) {
                    //element was not found -> optional element is not set, done
                    foundSyntaxError=false;
                }
                else {
                    int argsParsed = 0;
                    valid = m_OptionalElement->parse(arrayToParse+parsedUpTo, sizeOfArray-parsedUpTo, argsParsed, foundSyntaxError);
                    if( valid && !foundSyntaxError )
                        parsedUpTo+=argsParsed;
                }
            }
        }
    }

    return valid; //report any memory problems of children objects to caller
}

bool ToolsParsersOldSyntEl_Option::simulateParse(
    const char * const * arrayToParse,
    const int            sizeOfArray,
    int                & parsedUpTo,
    bool               & foundSyntaxError,
    bool               & foundSomething)
{
    parsedUpTo=0; //nothing parsed so far
    foundSyntaxError=false; //no syntax error found so far
    foundSomething=false;

    if( valid ) {
        foundSomething=true; //a list of optional arguments can always be found

        if( sizeOfArray > 0 ) {
            //something to parse
            int argsParsed = 0;
            bool tempFoundSomething = false;

            valid = m_OptionalElement->simulateParse(
                    arrayToParse+parsedUpTo,
                    sizeOfArray-parsedUpTo,
                    argsParsed,
                    foundSyntaxError,
                    tempFoundSomething);

            if ( valid ) {
                if( foundSyntaxError ) {
                    parsedUpTo=0;
                    foundSyntaxError = false;
                }
                else
                    parsedUpTo += argsParsed;
            }
        }
    }
    return valid; //report any memory problems of children objects to caller
}

bool ToolsParsersOldSyntEl_Option::printSyntax(char* outText, size_t spaceLeft) {
    if( spaceLeft >= 1 ) {
        strcat(outText, "[");
        --spaceLeft;
        if( m_OptionalElement->printSyntax(outText, spaceLeft) ) {
            if( spaceLeft >= 1 ) {
                strcat(outText, "]");
                --spaceLeft;
                return true;
            }
        }
    }
    return false;
}
