/*!
    \file    ToolsParsersSyntEl_Invalid.hpp
    \author  TiloH
    \ingroup syntax elements for parsers
    \brief   declaration of a class for an unusable elements 

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

#if !defined(TOOLSPARSERSSYNTEL_INVALID_HPP)
#define TOOLSPARSERSSYNTEL_INVALID_HPP


#include "ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_NonTerminal.hpp"


/*! \brief class simulating an unusable syntax element */
class ToolsParsersSyntEl_Invalid: public ToolsParsersSyntEl_NonTerminal
{
  public:
    /*! \brief implementation of abstract virtual function of base class
        ToolsParsersSyntEl_ParameterBase for parsing a parameter into an
        object.
        
        This class will parse nothing and will always return a syntax error.
        
        \param arrayToParse     [IN]  array containing the parameter strings to
            parse
        \param sizeOfArray      [IN]  size of arrayToParse
        \param parsedUpTo       [OUT] 0
        \param foundSyntaxError [OUT] true
        \return true */
    virtual bool parse(const char * const * arrayToParse,
                       const int            sizeOfArray,
                       int                & parsedUpTo,
                       bool               & foundSyntaxError);

    /*! \brief implementation of abstract virtual function of base class
        ToolsParsersSyntEl_ParameterBase for simulating the parsing of
        parameter(s) into the object
        
        Simulating the parsing (parse ahead) in array arrayToParse starting at
        the first array element. As this class parses nothing, simulateParse()
        will always return a syntax error.
        
        \param arrayToParse     [IN]  array containing the parameter strings to
            parse
        \param sizeOfArray      [IN]  size of arrayToParse
        \param parsedUpTo       [OUT] 0
        \param foundSyntaxError [OUT] true
        \param foundSomething   [OUT] false
        \return true  */
    virtual bool simulateParse(
        const char * const * arrayToParse,
        const int            sizeOfArray,
        int                & parsedUpTo,
        bool               & foundSyntaxError,
        bool               & foundSomething);

    /*! \brief implementation of abstract virtual function of base class 
        ToolsParsersSyntEl_ParameterBase for printing a syntax element/group/
        tree into a character buffer

        This class implements the non usable syntax element that can not been
        printed

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
