/*!
    \file    Tools_CommandlineToArgumentsConverter.hpp
    \author  TiloH
    \ingroup backup tool simulators for tests
    \brief   declaration of a class for converting a commandline into
             an array of zero terminated C strings

    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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

#include <ctype.h>
#include "SAPDBCommon/SAPDB_string.h"
#include "ToolsCommon/Tools_CommandlineToArgumentsConverter.hpp"

Tools_CommandlineToArgumentsConverter::Tools_CommandlineToArgumentsConverter()
:size(0),
 values(0),
 capacity(0)
{
}

Tools_CommandlineToArgumentsConverter::~Tools_CommandlineToArgumentsConverter()
{
    if(0!=values)
    {
        for(size_t i=0; i<size; i++) //delete all parameter strings
        {
            if(0!=values[i])
                delete [] values[i];
        }

        delete [] values; //delete the array of pointers
    }
}

bool Tools_CommandlineToArgumentsConverter::setTo(const char * commandLine, EscapingRules escapingRules)
{
    bool rc=true;
    const char *s=commandLine; //a shortcut

    char * tempArgument=new char[strlen(commandLine)+1]; //space for one argument (in most cases too large, but a.) definitely large enough and b.) not out of proportion)

    if(0==tempArgument)
        rc=false;

    while(rc && 0!=*s) //while all is fine and something is still in commandline
    {
        bool       insideOneArgument=true;
        bool       inEscapeMode=false;
        size_t pos=0;
        const char * argumentStartInS=0;

        while('\0'!=(*s) && isspace(*s))
            s++;

        argumentStartInS=s;

        while('\0'!=(*s) && insideOneArgument)
        {
            switch(*s)
            {
                case '\'':
                    {
                        if(WINDOWS_STYLE==escapingRules)
                            tempArgument[pos++]=(*(s++)); //just copy the character and move one character forward in both strings
                        else //UNIX_STYLE
                        {
                            //'\'' escapes absolutely everything until next '\''
                            s++;

                            while('\0'!=(*s) && '\''!=(*s))
                                tempArgument[pos++]=(*(s++));

                            if('\''==(*s)) // the '\'', that is ending the escaped sequence, is removed from the argument
                                s++;
                        }
                    } break;
                case '\\':
                    {
                        if(WINDOWS_STYLE==escapingRules)
                        {
                            size_t numberOfConsecutiveBackslashes=1; //we have found one already

                            while('\\'==s[numberOfConsecutiveBackslashes])
                                numberOfConsecutiveBackslashes++;

                            if('\"'==s[numberOfConsecutiveBackslashes]) //if a sequence of backslashes is ended by a '\"' (space escape mode or not)
                            {
                                SAPDB_memcpy(tempArgument+pos, s, numberOfConsecutiveBackslashes/2); //the backslashes escape each other -> only half of them survive
                                pos+=numberOfConsecutiveBackslashes/2; //position to end of copy
                                s+=numberOfConsecutiveBackslashes;     //position to end of backslashes in original

                                if(0!=numberOfConsecutiveBackslashes%2) //the double quote is escaped by the last backslash
                                {
                                    tempArgument[pos++]='\"'; //copy the double quote escaped by the last unpaired backslash
                                    s++;                      //and go over the double quote in the commandline
                                }
                                //else nothing to do, following double qoute is a normal double quote
                            }
                            else //if a sequence of backslashes is not followed by a double quote, all backslashes remain in the string
                            {
                                SAPDB_memcpy(tempArgument+pos, s, numberOfConsecutiveBackslashes);
                                pos+=numberOfConsecutiveBackslashes; //position to end of copy
                                s+=numberOfConsecutiveBackslashes;   //position to end of backslashes in original
                            }
                        }
                        else //UNIX_STYLE
                        {
                            if(inEscapeMode)
                            {
                                if('$'==s[1] || '`'==s[1] || '\"'==s[1] || '\\'==s[1] || '\n'==s[1])
                                {
                                    tempArgument[pos++]=s[1]; //a backslash between double qoutes is still escaping the characters above, so drop the backslash and copy the next character
                                    s+=2;                     //position behind the escaped character
                                }
                                else
                                    tempArgument[pos++]=(*(s++)); //use the backslash, that is qouted by double qoutes
                            }
                            else
                            {  //outside of double and single qoutes a backslash qoutes everthing including itself
                                if('\0'!=s[1])
                                {
                                    tempArgument[pos++]=s[1];
                                    s+=2;
                                }
                                else //in an original bash you would get in this case a: "bash: command substitution: line 2: syntax error: unexpected end of file"
                                    tempArgument[pos++]=(*(s++)); //we just copy the backslash for simplicity
                            }
                        }
                    } break;
                case '\"': //a normal double quote, that is not masked by any backslash
                    {
                        if(inEscapeMode)
                        {
                            inEscapeMode=false;

                            if(WINDOWS_STYLE==escapingRules)
                            {
                                if('\"'==s[1]) //if we leave escape mode and this is followed by a '\"', that '\"' is itself escaped like with a '\\' and does not switch on escape mode again (try it on the Windows command line :))
                                {
                                    tempArgument[pos++]='\"'; //copy the double quote escaped by the previous space escape ending double quote
                                    s++; //position behind first double quote (onto the second), but together with the s++ below, we position behind the second, escaped double quote
                                }
                            }
                            //else nothing to do for Unix style
                        }
                        else
                            inEscapeMode=true;

                        s++; //go over the just space escaping double quote
                    } break;
                default:
                    {
                        if((!inEscapeMode && isspace(*s)) || '\0'==(*s)) //argument end at non-escaped spaces or at the end of the commandline
                            insideOneArgument=false;
                        else                            
                            tempArgument[pos++]=(*(s++)); //just copy the character and move one character forward in both strings
                    }
            }
        }

        tempArgument[pos]='\0'; //terminate with zero in all cases

        if(argumentStartInS!=s) //we have found more than just unescaped spaces in the commandline
            rc=addValue(tempArgument);
    }

    if(0!=tempArgument)
        delete [] tempArgument;

    return rc;
}

bool Tools_CommandlineToArgumentsConverter::addValue(const char *newArgument)
{
    bool rc=true;
    char * s=new char[strlen(newArgument)+1];

    if(0==s)
        rc=false;
    else
    {
        SAPDB_strcpy(s, newArgument);

        if(size>=capacity)
            rc=increaseCapacity((0>=capacity)?2:(2*capacity));

        if(rc)
            values[size++]=s;
    }

    return rc;
}

bool Tools_CommandlineToArgumentsConverter::increaseCapacity(size_t newCapacity)
{
    bool rc=true;

    if(newCapacity>capacity)
    {
        char ** newValues=new char *[newCapacity+1];

        if(0==newValues)
            rc=false;
        else
        {
            if(0!=capacity)
            {
                SAPDB_memcpy(newValues, values, (capacity)*sizeof(char *));

                delete [] values;
            }
  
            for(size_t i=capacity; i<newCapacity; i++)
                newValues[i]=0;

            values=newValues;
            capacity=newCapacity;
        }
    }

    return rc;
}
