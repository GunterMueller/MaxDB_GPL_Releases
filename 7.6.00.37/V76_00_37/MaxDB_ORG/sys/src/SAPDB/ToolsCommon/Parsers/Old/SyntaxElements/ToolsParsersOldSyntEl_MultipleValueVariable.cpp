/*!
    \file    ToolsParsersOldSyntEl_MultipleValueVariable.cpp
    \author  TiloH
    \ingroup backup tool simulators for tests
    \brief   implementation of a class for variable command line parts, that store all parsed
             values

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

#include "ToolsCommon/Parsers/Old/SyntaxElements/ToolsParsersOldSyntEl_MultipleValueVariable.hpp"

ToolsParsersOldSyntEl_MultipleValueVariable::ToolsParsersOldSyntEl_MultipleValueVariable(
        const char * syntaxToken)
        : ToolsParsersOldSyntEl_ParameterBase() {
    
    if(!name.allocCopy(syntaxToken))
        valid=false;
}

ToolsParsersOldSyntEl_MultipleValueVariable::ToolsParsersOldSyntEl_MultipleValueVariable(
        const char * syntaxToken, 
        const Tools_Vector<ToolsParsersUtil_String *> defaultValues)
        : ToolsParsersOldSyntEl_ParameterBase() {

    if(!name.allocCopy(syntaxToken))
        valid=false;

    for(Tools_Vector<ToolsParsersUtil_String *>::iterator i=defaultValues.begin(); valid && i!=defaultValues.end(); i++) {
        ToolsParsersUtil_String* newValue(new ToolsParsersUtil_String);
        if( 0 == newValue )
            valid = false;
        else {
            newValue = *i;
            values.push_back(newValue);
        }
    }
}


ToolsParsersOldSyntEl_MultipleValueVariable::~ToolsParsersOldSyntEl_MultipleValueVariable()
{
    for(Tools_Vector<ToolsParsersUtil_String *>::iterator i=values.begin(); i!=values.end(); i++)
        delete (*i);
}

bool ToolsParsersOldSyntEl_MultipleValueVariable::parse(
    const char * const * arrayToParse,
    const int            sizeOfArray,
    int                & parsedUpTo,
    bool               & foundSyntaxError)
{
    parsedUpTo=0; //nothing parsed so far
    foundSyntaxError=false; //no syntax error found so far

    if(valid)
    {
        if(1>sizeOfArray)   //nothing to parse, but this variable is not optional!
            foundSyntaxError=true;
        else
        {
            ToolsParsersUtil_String *s=new ToolsParsersUtil_String;

            if(0!=s && s->allocCopy(arrayToParse[0]) && values.push_back(s))
            {
                parsedUpTo=1;
                found=1;
            }
            else
                valid=false;
        }
    }

    return valid; //an invalid object can not be used for parsing
}

bool ToolsParsersOldSyntEl_MultipleValueVariable::simulateParse(const char * const * arrayToParse,
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

bool ToolsParsersOldSyntEl_MultipleValueVariable::printSyntax(char* outText, size_t spaceLeft) {
    if( strlen(name) <= spaceLeft) {
        strcat(outText, name);
        spaceLeft -= strlen(name);
        return true;
    }
    else
        return false;
}

