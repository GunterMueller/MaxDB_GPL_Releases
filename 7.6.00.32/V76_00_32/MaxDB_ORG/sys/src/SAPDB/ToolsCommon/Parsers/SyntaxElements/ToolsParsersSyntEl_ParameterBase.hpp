/*!
    \file    ToolsParsersSyntEl_ParameterBase.hpp
    \author  TiloH
    \ingroup syntax elements for parsers
    \brief   declaration of a base class for command line parts 

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

#if !defined(TOOLSPARSERSSYNTEL_PARAMETERBASE_HPP)
#define TOOLSPARSERSSYNTEL_PARAMETERBASE_HPP

#include <string.h>

/*! \brief a base class for different program parameter types */
class ToolsParsersSyntEl_ParameterBase
{
  public:
    /*! \brief a simple constuctor */
    ToolsParsersSyntEl_ParameterBase();

    /*! \brief a virtual destructor to be prepared for the future */
    virtual ~ToolsParsersSyntEl_ParameterBase();

    /*! \brief abstract virtual function for parsing parameter into the object
        
        Parsing in array arrayToParse starting at the first array element. The
        object will change its status, value, etc. during that function
        according to the actual parameters found in the array.
        
        \param arrayToParse     [IN]  array containing the parameter strings to
            parse
        \param sizeOfArray      [IN]  size of arrayToParse
        \param parsedUpTo       [OUT] number of first unparsed element
        \param foundSyntaxError [OUT] true if a syntax error was found during
            parsing, false otherwise
        \return true if the parsing object is still valid (no memory allocation
            problems during parsing or before), false otherwise */
    virtual bool parse(const char * const * arrayToParse,
                       const int            sizeOfArray,
                       int                & parsedUpTo,
                       bool               & foundSyntaxError)=0;

    /*! \brief abstract virtual function for simulating the parsing of
        parameter(s) into the object
        
        Simulating the parsing (parse ahead) in array arrayToParse starting at
        the first array element. simulateParse() does - if anything at all -
        change the valid state of the object and of the child objects managed
        by it.
        
        \param arrayToParse     [IN]  array containing the parameter strings to
            parse
        \param sizeOfArray      [IN]  size of arrayToParse
        \param parsedUpTo       [OUT] number of first unparsed element 
        \param foundSyntaxError [OUT] true if a syntax error was found during
            parsing, false otherwise
        \param foundSomething   [OUT] true if object could be parsed
            successfully (note 0==parsedUpTo means the child object was
            optional or an empty list!)
        \return true if the object and all its children are still valid (no
            memory allocation problems during simulating parsing or before),
            false otherwise */
    virtual bool simulateParse(
        const char * const * arrayToParse,
        const int            sizeOfArray,
        int                & parsedUpTo,
        bool               & foundSyntaxError,
        bool               & foundSomething)=0;

    /*! \brief types of styles supported by function printSyntax() */
    enum PrintSyntaxStyle
    {
        CompleteSyntax, /*!< everything is printed, especially all brackets */
        SimpleSyntax    /*!< some brackets are removed */
    };

    /*! \brief virtual function for printing a syntax element/group/tree into a
            character buffer

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
        bool              & spaceTooSmall) const =0;

    /*! \brief function for determining wether the object is a terminal in the
        syntax (e.g. keyword, variable) or not (e.g. (), [], {})
        
        \return true if object is a terminal, false otherwise */
    virtual bool isTerminal() const=0;

    /*! \brief function for determining the validity of the object
    
        \return true if object and any child objects are valid (no memory
            problems occured so far), false otherwise */
    bool isValid() const;

    /*! function for determining the presence of the parameter (or parameter
        group) after parsing */
    bool isSet() const;

  protected:
    bool valid; //!<state of the object (true if object can be normally used, false otherwise)
    bool found; //!<state of the parameter represented by the object, \see isSet()

    /*! function for printing a character into a buffer

        This function does nothing, if there is no space left in the buffer
        
        \param character   [IN] the character to print
        \param position    [IN/OUT] end of the string in the buffer, updated, if charcter was inserted
        \param spaceLeft   [IN/OUT] space left in the buffer, including space for the terminated zero, updated, if character was inserted
        \param spaceNeeded [IN/OUT] space needed for the complete result, always increased by 1
        \param spaceTooSmall [OUT] set to true if no space is left for a character */
    void printChar(
        char      character,
        char   *& position,
        size_t  & spaceLeft,
        size_t  & spaceNeeded,
        bool    & spaceTooSmall) const;

    /*! function for printing a string into a buffer

        This function does not change the buffer, if there is not enough space
        left in the buffer for the complete string
        
        \param string      [IN] the string to print
        \param position    [IN/OUT] end of the string in the buffer, updated, if string was inserted
        \param spaceLeft   [IN/OUT] space left in the buffer, including space for the terminated zero, updated, if string was inserted
        \param spaceNeeded [IN/OUT] space needed for the complete result, always increased by length of string
        \param spaceTooSmall [OUT] set to true if no space is left for the complete string */
    void printString(
        const char  * string,
        char       *& position,
        size_t      & spaceLeft,
        size_t      & spaceNeeded,
        bool        & spaceTooSmall) const;
};

#endif
