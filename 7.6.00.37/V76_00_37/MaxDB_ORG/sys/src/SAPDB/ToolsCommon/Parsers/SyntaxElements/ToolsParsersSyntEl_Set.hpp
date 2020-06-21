/*!
    \file    ToolsParsersSyntEl_Set.hpp
    \author  MarcW
    \ingroup syntax elements for parsers
    \brief   declaration of a class for handling a list of parameters
             succeeding each other in arbitrary order

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

#if !defined (TOOLSPARSERSSYNTEL_SET_HPP)
#define TOOLSPARSERSSYNTEL_SET_HPP

#include "ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_ParameterGroup.hpp"

/*! \brief A class for handling a group of succeeding parameters such as
    "dbmcli -d \<dbname\> -u \<user\>,\<passwd\> ..."

    If the parameters can occure more then once combine with classes
    ToolsParsersSyntEl_Repetition and ToolsParsersSyntEl_Option.

    To add parameters to the group use the push_back() function.

    \see ToolsParsersSyntEl_Option
    \see ToolsParsersSyntEl_ParameterGroup::push_back
    \see ToolsParsersSyntEl_Repetition */
class ToolsParsersSyntEl_Set: public ToolsParsersSyntEl_ParameterGroup
{
  public:
    /*! \brief function implementing the virtual parse function of
        ToolsParsersSyntEl_ParameterBase for ToolsParsersSyntEl_Set.
        
        Parsing in array arrayToParse starting at the first array element. The
        child objects managed by a ToolsParsersSyntEl_Set will be changed
        during the parse function, as their parse functions are called.
        
        \param arrayToParse     [IN]  array containing the parameter strings to
            parse
        \param sizeOfArray      [IN]  size of arrayToParse
        \param parsedUpTo       [OUT] number of first unparsed element 
        \param foundSyntaxError [OUT] true if a syntax error was found during
            parsing, false otherwise
        \return true if the object and its children are still valid (no memory
            allocation problems during parsing or before), false otherwise */
    virtual bool parse(
        const char * const * arrayToParse,
        const int            sizeOfArray,
        int                & parsedUpTo,
        bool               & foundSyntaxError);

    /*! \brief function implementing the virtual simulateParse function of
        ToolsParsersSyntEl_ParameterBase for ToolsParsersSyntEl_Set.
        
        Simulating the parsing (parse ahead) in array arrayToParse starting at
        the first array element. simulateParse() does not change anything other
        than the valid state of the object and of the child objects managed by it.
        
        \param arrayToParse     [IN]  array containing the parameter strings to
            parse
        \param sizeOfArray      [IN]  size of arrayToParse
        \param parsedUpTo       [OUT] number of first unparsed element 
        \param foundSyntaxError [OUT] true if a syntax error was found during
            parsing, false otherwise
        \param foundSomething   [OUT] true if all managed child objects can be
            parsed successfully
        \return true if the object and its children are still valid (no memory
            allocation problems during simulating parsing or before), false
            otherwise */
    virtual bool simulateParse(
        const char * const * arrayToParse,
        const int            sizeOfArray,
        int                & parsedUpTo,
        bool               & foundSyntaxError,
        bool               & foundSomething);

    /*! \brief virtual function for printing a set into a character buffer

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
