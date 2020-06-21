/*!
    \file    ToolsParsersOldSyntEl_Keyword.hpp
    \author  TiloH
    \ingroup backup tool simulators for tests
    \brief   declaration of a class for key words in command lines 

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

#include "ToolsCommon/Parsers/Utilities/ToolsParsersUtil_String.hpp"
#include "ToolsCommon/Parsers/Old/SyntaxElements/ToolsParsersOldSyntEl_ParameterBase.hpp"

/*! \brief a basic class for keyword parameters such as "dbmcli ... \<database\> ..." */
class ToolsParsersOldSyntEl_Keyword:public ToolsParsersOldSyntEl_ParameterBase
{
  public:
    /*! \brief constructor accepting the keyword
    
        a copy of keyword is stored in the object, if memory allocation for that copy was succesful
        can be checked with the isValid() function of the base class ToolsParsersOldSyntEl_ParameterBase
        \see ToolsParsersOldSyntEl_ParameterBase::isValid */
    ToolsParsersOldSyntEl_Keyword(const char * keyword, bool caseInsensitive = false);

    /*! a destructor */
    virtual ~ToolsParsersOldSyntEl_Keyword();

    virtual bool printSyntax(char* outText, size_t spaceLeft);

  protected:
    /*! \brief function implementing the virtual parse function of ToolsParsersOldSyntEl_ParameterBase
        for ToolsParsersOldSyntEl_Keyword.
        
        Parsing the first element in array arrayToParse. If this first 
        element matches the keyword string managed by the object, the object is in the state found
        no syntax error did occure.
        
        \param arrayToParse     [IN]  array containing the parameter strings to parse
        \param sizeOfArray      [IN]  size of arrayToParse
        \param parsedUpTo       [OUT] number of first unparsed element 
        \param foundSyntaxError [OUT] true if a syntax error was found during parsing, false otherwise
        \return true if the object is still valid (no memory allocation problems during parsing or before), false otherwise */
    virtual bool parse(const char * const * arrayToParse,
                       const int            sizeOfArray,
                       int                & parsedUpTo,
                       bool               & foundSyntaxError);

    /*! \brief function implementing the virtual simulateParse function of ToolsParsersOldSyntEl_ParameterBase
        for ToolsParsersOldSyntEl_Keyword.
        
        Simulating the parsing (parse ahead) in array arrayToParse
        starting at the first array element.
        
        \param arrayToParse     [IN]  array containing the parameter strings to parse
        \param sizeOfArray      [IN]  size of arrayToParse
        \param parsedUpTo       [OUT] number of first unparsed element 
        \param foundSyntaxError [OUT] true if a syntax error was found during parsing, false otherwise
        \param foundSomething   [OUT] true if first element of arrayToParse was the keyword managed by the object
        \return true if the object is still valid (no memory allocation problems during parsing or before), false otherwise */
    virtual bool simulateParse(const char * const * arrayToParse,
                               const int            sizeOfArray,
                               int                & parsedUpTo,
                               bool               & foundSyntaxError,
                               bool               & foundSomething);

private:
    ToolsParsersUtil_String m_Keyword;
    bool m_CaseInsensitive;

    /** @brief check if the keyword exists depending of case sensitivity flag */
    bool keywordEquals(const char* string) {
        return
            (!m_CaseInsensitive && m_Keyword.equals(string)) ||
            (m_CaseInsensitive && m_Keyword.equalsIgnoreCase(string));
    }
};
