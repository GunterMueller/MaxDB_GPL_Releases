/*!
  \file    DBMSrvExpHlp_Part.cpp
  \author  TiloH
  \ingroup Basics for DBMServer commands explain and help
  \brief   implementation of classes for handling explain strings
           for the DBMServer commands explain and help

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------

#include <limits.h>
#include <string.h>

#include "SAPDB/DBM/Srv/ExplainHelp/DBMSrvExpHlp_Part.hpp"
#include "hcn36.h"


//-----------------------------------------------------------------------------
// class DBMSrvExpHlp_Part
//-----------------------------------------------------------------------------

DBMSrvExpHlp_Part::DBMSrvExpHlp_Part(const char * PartKeyWord,
                                     const char * InfoString,
                                     bool         HasShortName,
                                     bool         HasLongName,
                                     const char * SuperPartKeyWord)
  :iD(""),
   iDEnd(iD),
   longName(""),
   longNameEnd(longName),
   explanation(""),
   explanationEnd(explanation),
   partKeyWord(PartKeyWord),
   hasShortName(HasShortName),
   hasLongName(HasLongName),
   superPartKeyWord(SuperPartKeyWord)
{
    setTo(InfoString);
}

void DBMSrvExpHlp_Part::setTo(const char * InfoString)
{
    const char * NextWord=0;

    iD="";
    iDEnd=iD;
    longName="";
    longNameEnd=longName;
    explanation="";
    explanationEnd=explanation;

    //find the keyword
    const char * FirstOccurrenceOfKeyWord=strstr(InfoString, partKeyWord);

    //look for the keyword of the super part, if one is found it must be after the keyword
    if(0!=FirstOccurrenceOfKeyWord && 0!=superPartKeyWord)
    {
        const char * FirstOccurrenceOfSuperPartKeyWord=strstr(InfoString, superPartKeyWord);

        if(0!=FirstOccurrenceOfSuperPartKeyWord &&                                               //if we found a super part
           (FirstOccurrenceOfSuperPartKeyWord-InfoString)<(FirstOccurrenceOfKeyWord-InfoString)) //and it comes before the found part
        {
            FirstOccurrenceOfKeyWord=0;                                                              //ignore the part as it belongs to the next super part
        }                                                                                            //e.g. something like "@param ... @param ... @value"
    }

    //found keyword and it belongs to current super part
    if(0!=FirstOccurrenceOfKeyWord)
    {
        NextWord=cn36_FirstNonWhiteSpaceOf(cn36_FirstWhiteSpaceOf(FirstOccurrenceOfKeyWord));

        if(hasShortName)
        {
            GiveNextWordInDoubleQuotes(NextWord, iD, iDEnd);

            if(iD==iDEnd) //there should be a short name, but there isn't one
                FirstOccurrenceOfKeyWord=0; //ignore this invalid part
        }
        else
        {
            iD   =FirstOccurrenceOfKeyWord;     //use the keyword as dummy ID
            iDEnd=cn36_FirstWhiteSpaceOf(iD);
        }
    }

    if(0!=FirstOccurrenceOfKeyWord)    //all is valid so far
    {
        if(hasLongName)
            GiveNextWordInDoubleQuotes(NextWord, longName, longNameEnd);
        else
        {
            longName=iD;       //if part type has no longName use the shortName as long name
            longNameEnd=iDEnd;
        }

        explanation=NextWord;
        explanationEnd=strchr(explanation, '@');

        if(0==explanationEnd) //no other part following
            explanationEnd=cn36_TerminatingZeroOf(explanation);
    }

    if(0==FirstOccurrenceOfKeyWord)
    {
        iD="";
        iDEnd=iD;
    }
}

bool DBMSrvExpHlp_Part::setToNextParameter()
{
    setTo(giveExplanationEnd());

    return wasFound();
}

size_t DBMSrvExpHlp_Part::LongestIDOfSameType() const
{
    size_t LongestLength=0;

    if(wasFound())
    {
        DBMSrvExpHlp_Part TempPart(*this);

        LongestLength=TempPart.giveIDLength();

        while(TempPart.setToNextParameter())
        {
            if(TempPart.giveIDLength()>LongestLength)
                LongestLength=TempPart.giveIDLength();
        }
    }

    return  LongestLength;
}

size_t DBMSrvExpHlp_Part::LongestLongNameOfSameType() const
{
    size_t LongestLength=0;

    if(wasFound())
    {
        DBMSrvExpHlp_Part TempPart(*this);

        LongestLength=TempPart.giveLongNameLength();

        while(TempPart.setToNextParameter())
        {
            if(TempPart.giveLongNameLength()>LongestLength)
                LongestLength=TempPart.giveLongNameLength();
        }
    }

    return  LongestLength;
}

const char * DBMSrvExpHlp_Part::giveIDStart() const
{
    return iD;
}

size_t DBMSrvExpHlp_Part::giveIDLength() const
{
    return iDEnd-iD;
}

const char * DBMSrvExpHlp_Part::giveLongNameStart() const
{
    return longName;
}

size_t DBMSrvExpHlp_Part::giveLongNameLength() const
{
    return longNameEnd-longName;
}

const char * DBMSrvExpHlp_Part::giveExplanationStart() const
{
    return explanation;
}
const char * DBMSrvExpHlp_Part::giveExplanationEnd() const
{
    return explanationEnd;
}

size_t DBMSrvExpHlp_Part::giveExplanationLength() const
{
    return explanationEnd-explanation;
}

bool DBMSrvExpHlp_Part::wasFound() const
{
    return (0!=giveIDLength());
}

bool DBMSrvExpHlp_Part::hasExplanation() const
{
    return (0!=giveExplanationLength());
}

void DBMSrvExpHlp_Part::GiveNextWordInDoubleQuotes(const char *& Position, const char *& WordStart, const char *& WordEnd)
{
    const char * NextPart=strchr(Position, '@');

    WordStart=cn36_FirstNonWhiteSpaceOf(Position);

    if(0!=NextPart && WordStart-Position>NextPart-Position)
    {
        WordStart=WordEnd=Position; //return an empty word
        Position=NextPart;
    }
    else
    {
        const char * SecondDoubleQuote=('"'==WordStart[0])?strchr(WordStart+1, '"'):0;

        if(0==SecondDoubleQuote) //no first or no second double quote
        {
            WordEnd=cn36_FirstWhiteSpaceOf(WordStart);

            if(0!=NextPart && WordEnd-Position>NextPart-Position)
                WordEnd=NextPart;

            Position=cn36_FirstNonWhiteSpaceOf(WordEnd);
        }
        else
        {
            WordStart++;               //skip the first double quote
            WordEnd=SecondDoubleQuote; //skip the second double quote

            Position=cn36_FirstNonWhiteSpaceOf(SecondDoubleQuote+1);
        }
    }
}
