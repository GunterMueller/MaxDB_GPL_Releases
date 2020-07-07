/*!
  \file    DBMSrvExpHlp_Part.hpp
  \author  TiloH
  \ingroup Basics for DBMServer commands explain and help
  \brief   definition of classes for handling explain strings
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

#if !defined(DBMSrvExpHlp_Part_hpp)
#define DBMSrvExpHlp_Part_hpp


//-----------------------------------------------------------------------------
// class DBMSrvExpHlp_Part
//-----------------------------------------------------------------------------

//! \brief a class filtering a part from an explain string
class DBMSrvExpHlp_Part
{
  public:
    DBMSrvExpHlp_Part(const char * PartKeyWord,
                      const char * InfoString,
                      bool         HasShortName=true,
                      bool         HasLongName=false,
                      const char * SuperPartKeyWord=0);

    void setTo(const char * InfoString);
    bool setToNextParameter();

    size_t LongestIDOfSameType() const;
    size_t LongestLongNameOfSameType() const;

    const char * giveIDStart() const;
    size_t       giveIDLength() const;
    const char * giveLongNameStart() const;
    size_t       giveLongNameLength() const;
    const char * giveExplanationStart() const;
    const char * giveExplanationEnd() const;
    size_t       giveExplanationLength() const;

    bool wasFound() const;
    bool hasExplanation() const;

  private:
    const char * iD;
    const char * iDEnd;
    const char * longName;
    const char * longNameEnd;
    const char * explanation;
    const char * explanationEnd;
    const char * partKeyWord;
    const char * superPartKeyWord;
    bool         hasShortName;
    bool         hasLongName;

    void GiveNextWordInDoubleQuotes(const char *& Position, const char *& WordStart, const char *& WordEnd);
};

#endif
