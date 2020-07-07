/*!
    \file    ToolsTime_Time.cpp
    \author  TiloH
    \ingroup time handling for Tools
    \brief   definition of a class for handling time data

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

#include <time.h>
#include <string.h>

#include "hcn36.h"
#include "SAPDB/ToolsCommon/Time/ToolsTime_Time.hpp"
#include "SAPDB/ToolsCommon/Time/ToolsTime_TimeFormat.hpp"


int ToolsTime_Time::monthConversionFactor=10000000;
int ToolsTime_Time::dayConversionFactor=100000;
int ToolsTime_Time::secondsPerHour=3600;
int ToolsTime_Time::secondsPerMinute=60;

int ToolsTime_Time::maxMonthDaySeconds=123186399; //will generate a compiler warning if int is too small

ToolsTime_Time::ToolsTime_Time(void)
{
    setTo();
}

ToolsTime_Time::ToolsTime_Time(const char * string, const ToolsTime_TimeFormat & format)
{
    setTo(string, format);
}

ToolsTime_Time::ToolsTime_Time(const char * string, const ToolsTime_TimeFormat & format, const char * & endOfDateTime)
{
    setTo(string, format, endOfDateTime);
}

void ToolsTime_Time::setTo()
{
    time_t ltime=0;
    struct tm * stime=0;

    //get current date and time
    time(&ltime);
    
    //convert according to timezone
    stime=localtime(&ltime);

    //set the private members accordingly, pay attention to the rules for a struct tm
    setMonthDaySecondsTo(1900+stime->tm_year,
                         1+stime->tm_mon,
                         stime->tm_mday,
                         stime->tm_hour,
                         stime->tm_min,
                         stime->tm_sec);
}

void ToolsTime_Time::setTo(const char * time, const ToolsTime_TimeFormat & format)
{
    const char * endOfTime=0;

    setTo(time, format, endOfTime); //recycle more complex setTo version
}

void ToolsTime_Time::setTo(const char * time, const ToolsTime_TimeFormat & format, const char * & endOfDateTime)
{
    int  parts[ToolsTime_TimeFormat::Nothing]={0,1,1,0,0,0,0};   //set month and hour to 1
    bool allOk=true;

    ToolsTime_TimeFormat::AMPMType typeAMPM=ToolsTime_TimeFormat::NoAMOrPM; //so far we have not encountered any form of AM or PM

    size_t lengthOfTimeString=strlen(time);

    size_t usedLength=0,
           previouslyUsedLength=0;

    const char * previousPosition=time; //we start at the begin of time
    const char * currentPosition=time;

    for(int i=0; ToolsTime_TimeFormat::Nothing!=format.getPartType(i) && allOk && i<ToolsTime_TimeFormat::Nothing; i++)
    {
        for(int j=0; j<format.getNumberOfGapsInFrontOf(i); j++) //ignore the gaps in front of the next part
            currentPosition=cn36_FirstNonWhiteSpaceOf(cn36_FirstWhiteSpaceOf(currentPosition));

        if(format.getPartDigits(format.getPartType(i))) //if part is with at least 1 position in the format (that should be always the case)
        {
            //skip any fixed number of characters between last gap and current part
            usedLength=(currentPosition-time)+format.getPartPositionFromGap(i);

            //we are positioned correctly, so try to parse the current part
            if(ToolsTime_TimeFormat::AMPM==format.getPartType(i))
            {
                size_t sizeAMPM=0;

                typeAMPM=format.parseAMPMTypeFrom(time+usedLength, sizeAMPM); //determine if the expected am or pm is there

                if(ToolsTime_TimeFormat::NoAMOrPM==typeAMPM) //no AM or PM was found in String
                {
                    //ignore the whole part -> AM and PM is optional
                    usedLength=previouslyUsedLength;
                    currentPosition=previousPosition;
                }
                else
                {
                    if(ToolsTime_TimeFormat::NoDelimiter!=format.getPartDelimiter(i))  //AM/PM is delimited by a space or a non digit
                    {
                        while(isdigit(time[usedLength+sizeAMPM]))
                            sizeAMPM++;
                    }

                    usedLength+=sizeAMPM;
                    currentPosition=time+usedLength;
                }
            }
            else
                if(ToolsTime_TimeFormat::FixedEnd==format.getPartType(i)) //are we at any fixed end after the time string?
                {
                    size_t lengthOfFixedEnd=format.getPartDigits(format.getPartType(i));

                    if(lengthOfTimeString<usedLength+lengthOfFixedEnd) //fixed ends are optional and can differ in length, if it would be too long
                        usedLength=lengthOfTimeString;                 //skip it at the terminating zero
                    else
                    {
                        if(ToolsTime_TimeFormat::NoDelimiter!=format.getPartDelimiter(i))  //fixed end is delimited by a space (non spaces will be added to the fixed end's length by ToolsTime_TimeFormat)
                        {
                            while(!isspace(time[usedLength+lengthOfFixedEnd]))             //so search for it
                                lengthOfFixedEnd++;
                        }

                        usedLength+=lengthOfFixedEnd;
                    }

                    currentPosition=time+usedLength;
                }
                else    //must be a part consisting of digits
                {
                    if(ToolsTime_TimeFormat::NoDelimiter==format.getPartDelimiter(i))
                    {
                        if(lengthOfTimeString<usedLength+format.getPartDigits(format.getPartType(i)) ||
                           !cn36_StrNToInt(parts[format.getPartType(i)], time+usedLength, format.getPartDigits(format.getPartType(i))))
                        {
                            allOk=0;
                        }
                        else
                            usedLength+=format.getPartDigits(format.getPartType(i));
                    }
                    else
                    {
                        if(lengthOfTimeString>usedLength)
                        {
                            size_t lengthOfPart=0;

                            while(isdigit(time[usedLength+lengthOfPart]))  //not reached delimiter or '\0'
                                lengthOfPart++;

                            if(cn36_StrNToInt(parts[format.getPartType(i)], time+usedLength, lengthOfPart))
                                usedLength+=lengthOfPart;
                            else
                                allOk=0;
                        }
                        else
                            allOk=0;
                    }

                    if(allOk)
                        currentPosition=time+usedLength;
                }
        }
        else
            allOk=false;

        previousPosition=currentPosition;
        previouslyUsedLength=usedLength;
    }

    if(allOk)
    {
        //deal with 2 digits for year
        if(2==format.getPartDigits(ToolsTime_TimeFormat::Year))
        {
            int baseYear=1900;

            if(parts[ToolsTime_TimeFormat::Year]%100<30)
                baseYear=2000;

            parts[ToolsTime_TimeFormat::Year]=baseYear+parts[ToolsTime_TimeFormat::Year]%100;
        }

        //adjust hour for PM Flag
        if(ToolsTime_TimeFormat::PM==typeAMPM && 0<=parts[ToolsTime_TimeFormat::Hour] && parts[ToolsTime_TimeFormat::Hour]<12)
           parts[ToolsTime_TimeFormat::Hour]+=12;

        //now set the time information
        setMonthDaySecondsTo(parts[tcn36_DateTimeFormat::Year],
                             parts[tcn36_DateTimeFormat::Month],
                             parts[tcn36_DateTimeFormat::Day],
                             parts[tcn36_DateTimeFormat::Hour],
                             parts[tcn36_DateTimeFormat::Minute],
                             parts[tcn36_DateTimeFormat::Second]);
    }
    else
        m_MonthDaySecond=0; //used to flag a invalid time

    endOfDateTime=time+usedLength;
}

bool ToolsTime_Time::isValidTime() const
{
    return (0!=m_MonthDaySecond);
}

bool ToolsTime_Time::operator<=(const ToolsTime_Time & otherDateTime) const
{ 
    return (m_Year<otherDateTime.m_Year || (m_Year==otherDateTime.m_Year && m_MonthDaySecond<=otherDateTime.m_MonthDaySecond));
}

bool ToolsTime_Time::operator==(const ToolsTime_Time & otherDateTime) const
{
    return (m_Year==otherDateTime.m_Year && m_MonthDaySecond==otherDateTime.m_MonthDaySecond);
}

bool ToolsTime_Time::operator!=(const ToolsTime_Time & otherDateTime) const
{
    return (m_Year!=otherDateTime.m_Year || m_MonthDaySecond!=otherDateTime.m_MonthDaySecond);
}

bool ToolsTime_Time::operator>(const ToolsTime_Time & otherDateTime) const
{
    return (m_Year>otherDateTime.m_Year || (m_Year==otherDateTime.m_Year && m_MonthDaySecond>otherDateTime.m_MonthDaySecond));
}

void ToolsTime_Time::setMonthDaySecondsTo(int year, int month, int day, int hour, int minute, int second)
{
    m_Year=year;

    if(0<month && 12>=month &&
       0<day && 31>=day &&
       0<=hour && 23>=hour &&
       0<=minute && 60>minute &&
       0<=second && 60>second)
    {
        m_MonthDaySecond=month*ToolsTime_Time::monthConversionFactor
                         +day*ToolsTime_Time::dayConversionFactor
                         +hour*ToolsTime_Time::secondsPerHour
                         +minute*ToolsTime_Time::secondsPerMinute
                         +second;
    }
    else
        m_MonthDaySecond=0; //used to flag a invalid time
}

void ToolsTime_Time::getParts(int & year, int & month, int & day, int & hour, int & minute, int & second) const
{
    int h=m_MonthDaySecond;

    year=m_Year;

    month=h/ToolsTime_Time::monthConversionFactor;
    h%=ToolsTime_Time::monthConversionFactor;

    day=h/ToolsTime_Time::dayConversionFactor;
    h%=ToolsTime_Time::dayConversionFactor;

    hour=h/ToolsTime_Time::secondsPerHour;
    h%=ToolsTime_Time::secondsPerHour;

    minute=h/ToolsTime_Time::secondsPerMinute;
    second=h%ToolsTime_Time::secondsPerMinute;
}

int ToolsTime_Time::getYear() const
{
    return m_Year;
}

int ToolsTime_Time::getMonth() const
{
    return m_MonthDaySecond/ToolsTime_Time::monthConversionFactor;
}

int ToolsTime_Time::getDay() const
{
    int h=m_MonthDaySecond;

    h%=ToolsTime_Time::monthConversionFactor;

    return h/ToolsTime_Time::dayConversionFactor;
}

int ToolsTime_Time::getHour() const
{
    int h=m_MonthDaySecond;

    h%=ToolsTime_Time::monthConversionFactor;
    h%=ToolsTime_Time::dayConversionFactor;

    return h/ToolsTime_Time::secondsPerHour;
}

int ToolsTime_Time::getMinute() const
{
    int h=m_MonthDaySecond;

    h%=ToolsTime_Time::monthConversionFactor;
    h%=ToolsTime_Time::dayConversionFactor;
    h%=ToolsTime_Time::secondsPerHour;

    return h/ToolsTime_Time::secondsPerMinute;
}

int ToolsTime_Time::getSecond() const
{
    int h=m_MonthDaySecond;

    h%=ToolsTime_Time::monthConversionFactor;
    h%=ToolsTime_Time::dayConversionFactor;
    h%=ToolsTime_Time::secondsPerHour;

    return h%ToolsTime_Time::secondsPerMinute;
}
