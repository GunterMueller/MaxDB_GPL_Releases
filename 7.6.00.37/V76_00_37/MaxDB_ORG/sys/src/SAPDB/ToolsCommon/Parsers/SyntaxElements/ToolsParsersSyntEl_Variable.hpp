/*!
    \file    ToolsParsersSyntEl_Variable.hpp
    \author  TiloH
    \ingroup syntax elements for parsers
    \brief   declaration of a class for variable command line parts 

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

#if !defined(ToolsParsersSyntEl_Variable_HPP)
#define ToolsParsersSyntEl_Variable_HPP

#include "ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_Terminal.hpp"

/*! \brief a class for variable program parameters such as "dbmcli ...
    <database> ..."

    If the variable is encountered more than once during parsing, only the last
    value is stored in the object. If you need all values for that variable use
    class ToolsParsersSyntEl_MultipleValueVariable
    \see ToolsParsersSyntEl_MultipleValueVariable */
class ToolsParsersSyntEl_Variable:public ToolsParsersSyntEl_Terminal
{
  public:
    /*! \brief constructor accepting the syntax token representing a variable,
        within a syntax.
        
        In the example "dbmcli ... <database> ..." the syntax token would be
        "<database>". A copy of syntax token is stored in the object. Whether
        memory allocation for that copy was succesful, can be checked with the
        isValid() function of the base class ToolsParsersSyntEl_ParameterBase.

        \param syntaxToken [IN] the name of the syntax token as zero terminated
            string */
    ToolsParsersSyntEl_Variable(
        const char * syntaxToken);

    /*! \brief constructor accepting the syntax token representing a variable,
        within a syntax.
        
        In the example "dbmcli ... <database> ..." the syntax token would be
        "<database>". A copy of syntax token is stored in the object. Whether
        memory allocation for that copy was succesful, can be checked with the
        isValid() function of the base class ToolsParsersSyntEl_ParameterBase.

        \param startSyntaxToken [IN] first character of the name of the
            syntax token within a string
        \param endSyntaxToken   [IN] first character following the name of
            the syntax token within a string
        \param startDefault     [IN] first character of the default value
        \param endDefault       [IN] first character following the default
            value */
    ToolsParsersSyntEl_Variable(
        const char * startSyntaxToken,
        const char * endSyntaxToken,
        const char * startDefault,
        const char * endDefault);

    /* \brief function for accessing the parsed value
       \return parsed value as zero terminated string, or "" if variable was
           not found during parsing */
    const char * getValue() const;
    
    /*! \brief function implementing the virtual parse function of
        ToolsParsersSyntEl_ParameterBase for ToolsParsersSyntEl_Variable.
        
        Parsing the first element in array arrayToParse. If there is such a
        first element, a copy is stored by this object, otherwise a syntax
        error is reported. The object stores always only the last parsed value.
        
        \param arrayToParse     [IN]  array containing the parameter strings to
            parse
        \param sizeOfArray      [IN]  size of arrayToParse
        \param parsedUpTo       [OUT] number of first unparsed element (1 if no
            syntax error occurred, 0 otherwise)
        \param foundSyntaxError [OUT] true if a syntax error was found during
            parsing, false otherwise
        \return true if the object is still valid (no memory allocation
            problems during parsing or before), false otherwise */
    virtual bool parse(
        const char * const * arrayToParse,
        const int            sizeOfArray,
        int                & parsedUpTo,
        bool               & foundSyntaxError);

    /*! \brief function implementing the virtual simulateParse() function of
        ToolsParsersSyntEl_ParameterBase for ToolsParsersSyntEl_Variable.
        
        Simulating the parsing (parse ahead) of a variable in array
        arrayToParse starting at the first array element. If the array contains
        at least one element the function succeeds.
        
        \param arrayToParse     [IN]  array containing the parameter strings to
            parse
        \param sizeOfArray      [IN]  size of arrayToParse
        \param parsedUpTo       [OUT] number of first unparsed element (1 if no
            syntax error occurred, 0 otherwise)
        \param foundSyntaxError [OUT] true if a syntax error was found during
            parsing, false otherwise
        \param foundSomething   [OUT] true if first element of arrayToParse
            existed
        \return true if the object is still valid (no memory allocation
            problems during parsing or before), false otherwise */
    virtual bool simulateParse(
        const char * const * arrayToParse,
        const int            sizeOfArray,
        int                & parsedUpTo,
        bool               & foundSyntaxError,
        bool               & foundSomething);

    /*! \brief implementing virtual printSyntax() function for class
        ToolsParsersSyntEl_Variable

        The function prints the variable name enclosed into angle brackets into
        the buffer. If the variable name contains already enclosing angle
        brackets, no second pair of brackets is added.

        \see ToolsParsersSyntEl_ParameterBase::printSyntax()

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

  private:
    ToolsParsersUtil_String m_value;  //!< member holding the last parsed value of the parameter
};

#endif