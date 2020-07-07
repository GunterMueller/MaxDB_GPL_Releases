/*!
    \file    ToolsParsersSyntEl_Keyword.hpp
    \author  TiloH
    \ingroup syntax elements for parsers
    \brief   declaration of a class for key words in command lines 

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

*/

#if !defined (TOOLSPARSERSSYNTEL_KEYWORD_HPP)
#define TOOLSPARSERSSYNTEL_KEYWORD_HPP

#include "ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_Terminal.hpp"

/*! \brief class for keyword parameters such as "dbmcli ... \<database\> ..." */
class ToolsParsersSyntEl_Keyword: public ToolsParsersSyntEl_Terminal //Base
{
  public:
    /*! \brief constructor accepting a string representing the key word 
    
        A copy of parameter keyword is stored in the object. Whether memory
        allocation for that copy was succesful or not, can be checked with
        the isValid() function of base class ToolsParsersSyntEl_ParameterBase.

        \see ToolsParsersSyntEl_ParameterBase::isValid()
        
        \param keyword [IN] the keyword as zero terminated string */
    ToolsParsersSyntEl_Keyword(
        const char * keyword);

    /*! \brief constructor accepting string representing a key word
    
        A copy of parameter keyword is stored in the object. Whether memory
        allocation for that copy was succesful or not, can be checked with
        the isValid() function of base class ToolsParsersSyntEl_ParameterBase.

        \see ToolsParsersSyntEl_ParameterBase::isValid()
        
        \param startKeyword [IN] pointer to first character of the keyword
        \param endKeyword [IN] pointer to first character behind the last
            character of keyword */
    ToolsParsersSyntEl_Keyword(
        const char * startKeyword,
        const char * endKeyword);

    /*! \brief function implementing the virtual parse function of
        ToolsParsersSyntEl_ParameterBase for ToolsParsersSyntEl_Keyword.
        
        Parsing the first element in array arrayToParse. If this first element
        matches the keyword string managed by the object, the object is in the
        state found otherwise a syntax error occured at the current parse
        position.
        
        \param arrayToParse     [IN]  array containing the parameter strings to
            parse
        \param sizeOfArray      [IN]  size of arrayToParse
        \param parsedUpTo       [OUT] number of first unparsed element 
        \param foundSyntaxError [OUT] true if a syntax error was found during
            parsing, false otherwise
        \return true if the object is still valid (no memory allocation
            problems during parsing or before), false otherwise */
    virtual bool parse(const char * const * arrayToParse,
                       const int            sizeOfArray,
                       int                & parsedUpTo,
                       bool               & foundSyntaxError);

    /*! \brief function implementing the virtual simulateParse function of
        ToolsParsersSyntEl_ParameterBase for ToolsParsersSyntEl_Keyword.
        
        Simulating the parsing (parse ahead) in array arrayToParse
        starting at the first array element.
        
        \param arrayToParse     [IN]  array containing the parameter strings to
            parse
        \param sizeOfArray      [IN]  size of arrayToParse
        \param parsedUpTo       [OUT] number of first unparsed element 
        \param foundSyntaxError [OUT] true if a syntax error was found during
            parsing, false otherwise
        \param foundSomething   [OUT] true if first element of arrayToParse was
            the keyword managed by the object
        \return true if the object is still valid (no memory allocation
            problems during simulated parsing or before), false otherwise */
    virtual bool simulateParse(const char * const * arrayToParse,
                               const int            sizeOfArray,
                               int                & parsedUpTo,
                               bool               & foundSyntaxError,
                               bool               & foundSomething);

    /*! \brief implementing virtual printSyntax function for class ToolsParsersSyntEl_Keyword
        
        The function prints the keyword into the buffer.

        \see ToolsParsersSyntEl_ParameterBase::printSyntax

        \param style         [IN]     style of output (complete syntax or
            simple syntax)
        \param isOuterElement [IN]     true, if element must not print
            enclosing brackets, false otherwise
        \param position      [IN/OUT] position of first unused buffer character
        \param spaceLeft     [IN/OUT] space left in the buffer including space
            for the terminating zero
        \param spaceNeeded   [IN/OUT] space needed to print the complete syntax
            string
        \param spaceTooSmall [OUT] unchanged if buffer was large enough to
            print complete syntax string, true otherwise */
    virtual void printSyntax(
        PrintSyntaxStyle    style,
        bool                isOuterElement,
        char             *& position,
        size_t            & spaceLeft,
        size_t            & spaceNeeded,
        bool              & spaceTooSmall) const;
};

#endif
