/*!
    \file    ToolsTime_TimeFormat.cpp
    \author  TiloH
    \ingroup time handling for Tools
    \brief   definition of a class for handling time formats

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

#include "hcn36.h"
#include "ToolsCommon/Time/ToolsTime_TimeFormat.hpp"


ToolsTime_TimeFormat::ToolsTime_TimeFormat()
    :m_FormatString(0),
     m_StartOfFormatSection(0),
     m_EndOfFormatSection(0),
     m_AMString(0),
     m_PMString(0),
     m_FormatSectionSeparator(';')
{
    (*this)=""; //use the redefined operator = (const char *)
}

ToolsTime_TimeFormat::ToolsTime_TimeFormat(const char * theFormatString)
    :m_FormatString(0),
     m_StartOfFormatSection(0),
     m_EndOfFormatSection(0),
     m_AMString(0),
     m_PMString(0),
     m_FormatSectionSeparator(';')
{
    (*this)=theFormatString; //use the redefined operator = (const char *)
}

ToolsTime_TimeFormat::~ToolsTime_TimeFormat()
{
    cn36_StrDealloc(m_FormatString);
    cn36_StrDealloc(m_AMString);
    cn36_StrDealloc(m_PMString);
}

void ToolsTime_TimeFormat::operator=(const char * newFormatString)
{
    int i;

    //initialize the object
    cn36_StrDealloc(m_FormatString);
    m_StartOfFormatSection=0;
    m_EndOfFormatSection=0;

    for(i=0; i<ToolsTime_TimeFormat::Nothing; i++) //initialize arrays for all handeled part types
    {
        m_Position[i]=0;
        m_Digits[i]=0;
        m_Type[i]=ToolsTime_TimeFormat::Nothing;
        m_GapsInFront[i]=0;
        m_RelativePositionToGap[i]=0;
        m_Delimiter[i]=ToolsTime_TimeFormat::NoDelimiter;
    }

    cn36_StrDealloc(m_AMString),
    cn36_StrDealloc(m_PMString),
    m_FormatSectionSeparator=';';

    m_IsFaultyObjectFlag=false;

    //now analyze the new time format string
    if(0!=newFormatString && '\0'!=newFormatString[0])
    {
        //make a local copy of the format string
        m_IsFaultyObjectFlag=!cn36_StrAllocCpy(m_FormatString, newFormatString);

        if(!m_IsFaultyObjectFlag)
        {
            size_t CurPos=0;
            bool   parsedFormatSection=false;

            while('\0'!=m_FormatString[CurPos] && !isAFaultyObject()) //while not at the end of the new format string, parse for the allowed format sections
            {
                bool         parsedSomething=false;
                const char * parsedUpTo=m_FormatString+CurPos;
                
                //try to parse separator section (e.g. "separator=;")
                parseForSeparatorSection(m_FormatString+CurPos, parsedUpTo, parsedSomething);

                //try to prase AM section (e.g. "AM=morning")
                if(!parsedSomething && !isAFaultyObject())
                    parseForAMPMSection(ToolsTime_TimeFormat::AM, m_FormatString+CurPos, parsedUpTo, parsedSomething, m_AMString);
                
                //try to parse PM section (e.g. "p.m.=afternoon")
                if(!parsedSomething && !isAFaultyObject())
                    parseForAMPMSection(ToolsTime_TimeFormat::PM, m_FormatString+CurPos, parsedUpTo, parsedSomething, m_PMString);

                //parse for the classical format section and make a copy of it
                if(!parsedSomething && !isAFaultyObject() && !parsedFormatSection)
                {
                    parseForFormatSection(m_FormatString+CurPos, parsedUpTo, parsedSomething);

                    if(parsedSomething)
                    {
                        parsedFormatSection=true; //do not parse the format section more then once, the first one is used, qall others are ignored

                        m_StartOfFormatSection=CurPos;
                        m_EndOfFormatSection=parsedUpTo-m_FormatString;
                    }
                }

                if(parsedSomething)
                    CurPos=parsedUpTo-m_FormatString; //update current position

                //search the section separator following the section (all valid sections are parsed maximally up to the section separator, if nothing was parsed the invalid section is just ignored)
                while('\0'!=m_FormatString[CurPos] && m_FormatSectionSeparator!=m_FormatString[CurPos])
                    CurPos++;

                //position to start of next section if there is any, so skip any section separators
                while(m_FormatSectionSeparator==m_FormatString[CurPos])
                    CurPos++;
            }

            if(!isAFaultyObject())
            {
                adjustEmptyAMPMString(ToolsTime_TimeFormat::AM, m_AMString);
                adjustEmptyAMPMString(ToolsTime_TimeFormat::PM, m_PMString);
            }
        }
    }
}

const char * ToolsTime_TimeFormat::getStartOfFormatSection() const
{
    return m_FormatString+m_StartOfFormatSection;
}

size_t ToolsTime_TimeFormat::getLengthOfBasicFormatSection() const
{
    return m_EndOfFormatSection-m_StartOfFormatSection;
}

size_t ToolsTime_TimeFormat::getMaximalLengthOfTimeString() const
{
    size_t rc=getLengthOfBasicFormatSection(); //format section without "SEPARATOR= ", "AM=..." or "PM=..."

    if(0<getPartDigits(ToolsTime_TimeFormat::AMPM)) // if there is some kind of AM/PM in the resulting string
    {
        size_t maxAdditionalAMPM=strlen(m_AMString); // it will be either the string for AM

        if(strlen(m_PMString)>maxAdditionalAMPM)     // or the one for PM
            maxAdditionalAMPM=strlen(m_PMString);    // we calculate with whatever is longer

        rc-=getPartDigits(ToolsTime_TimeFormat::AMPM); // the place holder for AM/PM in the format section will not be used
        rc+=maxAdditionalAMPM;                          // the configured AM and PM strings are used
    }

    return rc;
}

const char * ToolsTime_TimeFormat::getFormatString() const
{
    return m_FormatString;
}

ToolsTime_TimeFormat::AMPMType ToolsTime_TimeFormat::parseAMPMTypeFrom(const char *string, size_t & charsUsedByAMPM) const
{
    ToolsTime_TimeFormat::AMPMType rc=ToolsTime_TimeFormat::NoAMOrPM;
    charsUsedByAMPM=0;

    if(0!=m_AMString && 0==strncmp(string, m_AMString, strlen(m_AMString))) //if there is an explicit AM string configured check for it
    {
        rc=ToolsTime_TimeFormat::AM;
        charsUsedByAMPM=strlen(m_AMString);
    }
    else
        if(0!=m_PMString && 0==strncmp(string, m_PMString, strlen(m_PMString))) //if there is an explicit PM string configured check for it
        {
            rc=ToolsTime_TimeFormat::PM;
            charsUsedByAMPM=strlen(m_PMString);
        }
        else
            rc=startsWithAMPMPlaceHolder(string, charsUsedByAMPM);

    return rc;
}

const char * ToolsTime_TimeFormat::getAMString() const
{
    return m_AMString;
}

const char * ToolsTime_TimeFormat::getPMString() const
{
    return m_PMString;
}

ToolsTime_TimeFormat::AMPMType ToolsTime_TimeFormat::startsWithAMPMPlaceHolder(const char *string, size_t & charsUsedByAMPMPlaceHolder)
{
    ToolsTime_TimeFormat::AMPMType rc=ToolsTime_TimeFormat::NoAMOrPM;
    charsUsedByAMPMPlaceHolder=2;

    if(0==cn36_StrNUprCmp("AM", string, strlen("AM")))
        rc=ToolsTime_TimeFormat::AM;
    else
        if(0==cn36_StrNUprCmp("PM", string, strlen("PM")))
            rc=ToolsTime_TimeFormat::PM;
        else
        {
            charsUsedByAMPMPlaceHolder=4;

            if(0==cn36_StrNUprCmp("A.M.", string, strlen("A.M.")))
                rc=ToolsTime_TimeFormat::AM;
            else
                if(0==cn36_StrNUprCmp("P.M.", string, strlen("P.M.")))
                    rc=ToolsTime_TimeFormat::PM;
        }

    if(ToolsTime_TimeFormat::NoAMOrPM==rc)
        charsUsedByAMPMPlaceHolder=0;

    return rc;
}

void ToolsTime_TimeFormat::parseForAMPMSection(ToolsTime_TimeFormat::AMPMType     typeDesired,
                                               const char                      *  formatString,
                                               const char                      *& parsedUpTo,
                                               bool                             & parsedSomething,
                                               char                            *& newString)
{
    size_t digitsAMPM=0;

    //init the reference parameters
    parsedUpTo=formatString;
    parsedSomething=false;

    //skip white spaces
    formatString=cn36_FirstNonWhiteSpaceOf(formatString);

    //parse the section, if it is the right one
    if(typeDesired==startsWithAMPMPlaceHolder(formatString, digitsAMPM) &&   //section must start with the desired type
       formatString[digitsAMPM]=='='                                       ) // followed by an equal sign
    {
        size_t lengthAMString=0;

        //search end of string or end of section
        while(formatString[digitsAMPM+1+lengthAMString]!='\0' &&                   //while not at end of string and
              formatString[digitsAMPM+1+lengthAMString]!=m_FormatSectionSeparator) //not at end of section
        {
            lengthAMString++;                                                      //go to next character
        }

        if(!cn36_StrNAllocCpy(newString, formatString+digitsAMPM+1, lengthAMString)) //make a local copy of the new AM/PM string
            m_IsFaultyObjectFlag=true;

        parsedUpTo=formatString+digitsAMPM+1+lengthAMString;
        parsedSomething=true;
    }
}

void ToolsTime_TimeFormat::parseForSeparatorSection(const char * formatString, const char * & parsedUpTo, bool & parsedSomething)
{
    const char * separatorKeyWord="SEPARATOR=";
    size_t       separatorKeyWordLength=strlen(separatorKeyWord);

    //init the reference parameters
    parsedUpTo=formatString;
    parsedSomething=false;

    //skip white spaces
    formatString=cn36_FirstNonWhiteSpaceOf(formatString);

    //search for the separator sections keyword
    if(0==cn36_StrNUprCmp(separatorKeyWord, formatString, separatorKeyWordLength) &&    //sections starts with "SEPARATOR="
       '\0'!=formatString[separatorKeyWordLength]                                    )  //followed by at least one character
    {
        m_FormatSectionSeparator=formatString[separatorKeyWordLength]; //take the character as new section separator
        parsedUpTo=formatString+separatorKeyWordLength+1;              //tell what was parsed
        parsedSomething=true;                                          //tell that something was parsed
    }
}

void ToolsTime_TimeFormat::parseForFormatSection(const char * formatString, const char *& parsedUpTo, bool & parsedSomething)
{
    size_t                          currentPosition=0;
    ToolsTime_TimeFormat::PartType currentPartType=ToolsTime_TimeFormat::Nothing;
    int                             currentPart=0;  //the part we are working on (a part is something of the form "...<spaces><nonspaces><year|month...>")
    size_t                          digitsAMPM=0;

    ToolsTime_TimeFormat::AnalysisState previousState=ToolsTime_TimeFormat::FoundNormalCharacter;
    ToolsTime_TimeFormat::AnalysisState newState=ToolsTime_TimeFormat::FoundNormalCharacter;

    int gapsInFrontOfNextPart=0;
    int relativePositionToGapOfNextPart=0;

    //init the reference parameters
    parsedUpTo=formatString;
    parsedSomething=false;

    //do the parsing
    while('\0'!=formatString[currentPosition] &&                      //stop parsing at end of string,
          m_FormatSectionSeparator!=formatString[currentPosition] &&  //end of section and
          currentPart<ToolsTime_TimeFormat::Nothing)                   //after all allowed format parts were found (including any fixed end)
    {
        //find out the format part that we are looking at
        switch(formatString[currentPosition])
        {
            case 'y': currentPartType=Year;       break;
            case 'm': currentPartType=Month;      break;
            case 'd': currentPartType=Day;        break;
            case 'H': currentPartType=Hour;       break;
            case 'M': currentPartType=Minute;     break;
            case 'S': currentPartType=Second;     break;
            case 'A':
            case 'a':
            case 'P':
            case 'p':
            {
                ToolsTime_TimeFormat::AMPMType AMPMFlag=startsWithAMPMPlaceHolder(formatString+currentPosition, digitsAMPM);

                if(ToolsTime_TimeFormat::NoAMOrPM!=AMPMFlag)
                    currentPartType=ToolsTime_TimeFormat::AMPM;
            }break;
            default : currentPartType=ToolsTime_TimeFormat::Nothing;    break;
        }

        //correct the type of the current state
        if(ToolsTime_TimeFormat::Nothing!=currentPartType && 0==m_Digits[currentPartType])  //found part and found it for the first time
            newState=ToolsTime_TimeFormat::FoundPlaceholder;
        else
            currentPartType=ToolsTime_TimeFormat::Nothing; //if we found a part for a second time, ignore it

        if(ToolsTime_TimeFormat::Nothing==currentPartType)
        {
            if(isspace(formatString[currentPosition]))
                newState=ToolsTime_TimeFormat::FoundSpace;
            else
                newState=ToolsTime_TimeFormat::FoundNormalCharacter;
        }

        //do what is necessary for a transition between the parse states

        //new charcter was a normal character
        if(ToolsTime_TimeFormat::FoundNormalCharacter==newState)
        {
            if(ToolsTime_TimeFormat::FoundNormalCharacter==previousState && 0!=currentPosition)   //from 0 to 0 -> a normal character is following a normal character and it's not the first character at all (we initialized PrevState with 0)
                m_Digits[FixedEnd]++;         //just one more character for the FixedEnd
            //else
            if(ToolsTime_TimeFormat::FoundSpace==previousState ||                           //character follows a gap or
               ToolsTime_TimeFormat::FoundPlaceholder==previousState ||                     //a place holder or
               (ToolsTime_TimeFormat::FoundNormalCharacter==previousState && 0==currentPosition))    //we have found a new FixedEnd
            {
                m_Position[FixedEnd]=currentPosition; //throw away information about a previous one by initializing all relevant variables
                m_Digits[FixedEnd]=1;
                m_RelativePositionToGap[FixedEnd]=0;  //Fixed ends can not have a relative position
                m_Type[FixedEnd]=FixedEnd;

                if(ToolsTime_TimeFormat::FoundSpace==previousState)  //if a Fixed end follows a gap
                    m_GapsInFront[FixedEnd]=gapsInFrontOfNextPart;    //the previous gaps count
                else
                    m_GapsInFront[FixedEnd]=0;                        //otherwise they must be ignored

                m_Delimiter[FixedEnd]=NoDelimiter;
            }

            if(ToolsTime_TimeFormat::FoundPlaceholder==previousState)    //from 2 to 0 -> a normal character is following a placeholder
            {
                if(isdigit(formatString[currentPosition])) //if this normal charcter ...
                    m_Delimiter[currentPart]=NoDelimiter;    //...is a digit, the part is delimited by fixed length
                else
                    m_Delimiter[currentPart]=NonDigit;       //...is no digit, the placeholder is delimited by the character

                currentPart++;
            }

            if(currentPart<FixedEnd)               //we have found a normal character, so if another part could follow
                relativePositionToGapOfNextPart++; //the character adds to the relative position of that part
        }

        //new character was a space
        if(ToolsTime_TimeFormat::FoundSpace==newState)
        {
            if(ToolsTime_TimeFormat::FoundNormalCharacter==previousState)    //from 0 to 1 -> a space is following normal character
            {
                gapsInFrontOfNextPart++;             //this space starts a new gap in front of the next part (if one is coming)
                relativePositionToGapOfNextPart=0;   //all previous normal character do not count for the next part (as there is a new gap)

                m_Delimiter[FixedEnd]=Space;  //the fixed end (ending with the previous normal character) is delimited by the space
            }

            //if(ToolsTime_TimeFormat::FoundSpace==PrevState) do nothing as a white space following a white space is just the same gap

            if(ToolsTime_TimeFormat::FoundPlaceholder==previousState)    //from 2 to 1 -> a space is following a placeholder
            {
                m_Delimiter[currentPart]=Space;   //previous part is delimited by a space
                currentPart++;                  

                gapsInFrontOfNextPart++;    //equals here a =1, last was a placeholder, so it's the first gap for the next part
            }
        }

        //new character was a placeholder
        if(ToolsTime_TimeFormat::FoundPlaceholder==newState)
        {
            if(ToolsTime_TimeFormat::FoundNormalCharacter==previousState ||
               ToolsTime_TimeFormat::FoundSpace==previousState) //from 0,1 to 2 -> found a new placeholder
            {                                                    //therefore a previous fixed end is invalidated
                m_Position[FixedEnd]=0;                          //assure that this information is lost
                m_Digits[FixedEnd]=0;
                m_RelativePositionToGap[FixedEnd]=0;
                m_Type[FixedEnd]=ToolsTime_TimeFormat::Nothing;
                m_GapsInFront[FixedEnd]=0;
                m_Delimiter[FixedEnd]=NoDelimiter;
            }
            //else
            if(ToolsTime_TimeFormat::FoundPlaceholder==previousState)    //from 2 to 2 -> a new placeholder, directly behind another placeholder (yyyymm...)
            {
                m_Delimiter[currentPart]=NoDelimiter; //part is delimited by fixed length
                currentPart++; //this new part is delimiting the previous part
            }

            m_GapsInFront[currentPart]=gapsInFrontOfNextPart;                     //we have a new part and we know, how many gaps are there
            m_RelativePositionToGap[currentPart]=relativePositionToGapOfNextPart; //we know the relative position

            m_Position[currentPartType]=currentPosition; //the absolute position
            m_Digits[currentPartType]=1;                 //it has at least 1 digit
            m_Type[currentPart]=currentPartType;         //and it has a type

            gapsInFrontOfNextPart=0;            //the next part (if any exists) has no gap in front so far
            relativePositionToGapOfNextPart=0;  //and also no relative displacement

            //go to the end of the newly found placeholder
            if(AMPM==currentPartType)
            {
                currentPosition+=(digitsAMPM-1);           //skip the M from "AM" or "PM" to avoid conflicts with M for Minute (same with a.m., A.M., ...)
                m_Digits[currentPartType]+=(digitsAMPM-1); //adjust m_Digits[AMPM] accordingly
            }
            else
                while(formatString[currentPosition+1]==formatString[currentPosition])
                {
                    m_Digits[currentPartType]++;
                    currentPosition++;
                }
        }

        previousState=newState;
        currentPosition++;
    }

    //do necessary postprocessing

    if(0!=m_Digits[FixedEnd] && FixedEnd==m_Type[FixedEnd] && currentPart<FixedEnd) //if there is a fixed end and not all other parts are used...
    {                                                                       //...copy the part related information about fixed end to the current part
        m_RelativePositionToGap[currentPart]=m_RelativePositionToGap[FixedEnd];
        m_Type[currentPart]=FixedEnd;
        m_GapsInFront[currentPart]=m_GapsInFront[FixedEnd];
        m_Delimiter[currentPart]=m_Delimiter[FixedEnd];

        m_RelativePositionToGap[FixedEnd]=0; //and set the part related information of the last part to the defaults for signaling the part as unused
        m_Type[FixedEnd]=ToolsTime_TimeFormat::Nothing;
        m_GapsInFront[FixedEnd]=0;
        m_Delimiter[FixedEnd]=NoDelimiter;
    }

    parsedUpTo=formatString+currentPosition;

    if(formatString!=parsedUpTo)
        parsedSomething=true;
}

void ToolsTime_TimeFormat::adjustEmptyAMPMString(ToolsTime_TimeFormat::AMPMType     typeDesired,
                                                  char                            * & string)
{
    if(0==string) //string was not set before -> make sure we have always strings for AM and PM
    {
        if(0<getPartDigits(ToolsTime_TimeFormat::AMPM))
        {
            if(!cn36_StrNAllocCpy(string, m_FormatString+m_StartOfFormatSection+m_Position[ToolsTime_TimeFormat::AMPM], getPartDigits(ToolsTime_TimeFormat::AMPM))) //make a local copy of the new AM/PM string
                m_IsFaultyObjectFlag=true;
            else
            {
                //first character could be wrong adjust it anyway, e.g. p.m. instead of a.m.
                bool smallCharacter=('a'==string[0] || 'p'==string[0]);

                if(typeDesired==ToolsTime_TimeFormat::AM)
                    string[0]=smallCharacter?'a':'A';
                else
                    string[0]=smallCharacter?'p':'P';
            }
        }
        else
        {
            if(!cn36_StrAllocCpy(string, (typeDesired==ToolsTime_TimeFormat::AM)?"AM":"PM"))
                m_IsFaultyObjectFlag=true;
        }
    }
}
