/*!
    \file    ToolsTime_TimeFormatter.cpp
    \author  TiloH
    \ingroup time handling for Tools
    \brief   definition of a class for formatting time data into strings

    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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

#include <string.h>
#include <stdio.h>

#include "ToolsCommon/Time/ToolsTime_Time.hpp"
#include "ToolsCommon/Time/ToolsTime_TimeFormat.hpp"
#include "ToolsCommon/Time/ToolsTime_TimeFormatter.hpp"
#include "SAPDBCommon/SAPDB_string.h"


size_t ToolsTime_TimeFormatter::calculateResultingStringLength(
        const ToolsTime_Time       & time,
        const ToolsTime_TimeFormat & format) const
{
    size_t neededPlace=format.getLengthOfBasicFormatSection();
    int    hour=time.getHour();

    //compute needed place, for all parts
    if(0<format.getPartDigits(ToolsTime_TimeFormat::AMPM))
    {
        neededPlace-=format.getPartDigits(ToolsTime_TimeFormat::AMPM);

        if(hour<12)
            neededPlace+=strlen(format.getAMString());
        else
            neededPlace+=strlen(format.getPMString());
    }

    return neededPlace;
}

bool ToolsTime_TimeFormatter::writeStringIntoBuffer(
        char                        * buffer,
        size_t                        usableBufferSpace,
        const ToolsTime_Time       & time,
        const ToolsTime_TimeFormat & format) const
{
    bool   spaceIsSufficient=true;
    bool   wasPM=false;
    int    partsAsInt[1+(ToolsTime_TimeFormat::Second)]={0,1,1,0,0,0};   //set month and day to 1
    size_t pos=0;

    time.getParts(
        partsAsInt[ToolsTime_TimeFormat::Year],
        partsAsInt[ToolsTime_TimeFormat::Month],
        partsAsInt[ToolsTime_TimeFormat::Day],
        partsAsInt[ToolsTime_TimeFormat::Hour],
        partsAsInt[ToolsTime_TimeFormat::Minute],
        partsAsInt[ToolsTime_TimeFormat::Second]);

    //adjust hour for pm, if there is AM/PM in the format and hour is a PM-hour
    if(0<format.getPartDigits(ToolsTime_TimeFormat::AMPM) && partsAsInt[ToolsTime_TimeFormat::Hour]>=12)
    {
        partsAsInt[ToolsTime_TimeFormat::Hour]-=12;
        wasPM=true;
    }

    //create string "format part" by "format part"
    for(int i=0; ToolsTime_TimeFormat::Nothing!=format.getPartType(i) && i<ToolsTime_TimeFormat::Nothing; i++)
    {
        //we copy anything between previous part and the current part, if we have space in the buffer
        if(!copyStringNWithLimit(
                buffer,                                            //into the buffer
                usableBufferSpace,                                 //watch out for end of buffer
                format.getStartOfFormatSection()+pos,              //start copying in source at position of previous part
                format.getPartPosition(format.getPartType(i))-pos))//end copying right at start of current part
        {
            spaceIsSufficient=false;
        }

        pos=format.getPartPosition(format.getPartType(i)); //skip the copied part

        //now add the part itself
        if(ToolsTime_TimeFormat::AMPM==format.getPartType(i))
        {
            if(wasPM)
            {
                if(!copyStringNWithLimit(buffer, usableBufferSpace, format.getPMString(), strlen(format.getPMString())))
                    spaceIsSufficient=false;
            }
            else
            {
                if(!copyStringNWithLimit(buffer, usableBufferSpace, format.getAMString(), strlen(format.getAMString())))
                    spaceIsSufficient=false;
            }
        }   
        else
        {
            if(!copyIntWithLimit(                    //place the part in the string
                    buffer,
                    usableBufferSpace,
                    partsAsInt[format.getPartType(i)],
                    format.getPartDigits(format.getPartType(i))))
            {
                spaceIsSufficient=false;
            }
        }

        //update position in format buffer
        pos+=format.getPartDigits(format.getPartType(i));
    }

    //write terminating zero
    if(0<usableBufferSpace)
        (*buffer)='\0';
    else
        spaceIsSufficient=false;

    return spaceIsSufficient;
}

char * ToolsTime_TimeFormatter::createString(
        const ToolsTime_Time       & time,
        const ToolsTime_TimeFormat & format) const
{
    char * rc=0;
    size_t neededSpace=calculateResultingStringLength(time, format)+1; //+1 for terminating zero

    rc=new char [neededSpace];

    if(0!=rc)
        writeStringIntoBuffer(rc, neededSpace, time, format); //return code should be always true, as length was calculated correctly

    return rc;
}

bool ToolsTime_TimeFormatter::copyStringNWithLimit(char * & buffer, size_t & limit, const char * string, size_t stringLength) const
{
    bool rc=true;

    if(stringLength>limit)  //prevent buffer overflow
    {
        stringLength=limit;
        rc=false;           //buffer is not large enough
    }

    SAPDB_strncpy(buffer, string, stringLength);

    buffer+=stringLength;
    limit-=stringLength;

    return rc;
}

bool ToolsTime_TimeFormatter::copyIntWithLimit(char * & buffer, size_t & limit, int integer, size_t digits) const
{
    bool rc=true;

    char   integerAsString[100]; //should be enough for any int up to my 65-th birth day
    size_t length=0;
    size_t curPos=0;

    sprintf(integerAsString, "%d", integer);
    length=strlen(integerAsString);

    if(digits>limit) //honor limit under all circumstances
    {
        digits=limit;
        rc=false;           //buffer is not large enough
    }

    //create leading zeros, if integer has not enough digits, always watch for limit
    while(digits>length && limit>0) 
    {
        *(buffer++)='0';
        digits--;        //one charcater less can be taken from the integer
        limit--;         //one character less space in the buffer
    }

    //place the integer itself, watch for digits (and thereby for limit too) and copy the lower significant digits of integer, if space is to short for the integer
    SAPDB_strncpy(buffer, integerAsString+(length-digits), digits);

    buffer+=digits;
    limit-=digits;

    return rc;
}
