/*!
    \file    ToolsParsersSyntEl_AlternativeList.hpp
    \author  TiloH
    \ingroup syntax elements for parsers
    \brief   declaration of a class for handling a list of alternative
             parameters  

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

#if !defined (ToolsParsersSyntEl_AlternativeList_HPP)
#define ToolsParsersSyntEl_AlternativeList_HPP

#include "ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_ParameterGroup.hpp"

/*! \brief A class for handling a list of alternative parameters such as
    "dbmcli ... recover_start ...  EBID | ExternalBackupID  ...

    To add parameters to the list use the push_back() function.
    \see ToolsParsersSyntEl_ParameterGroup::push_back */
class ToolsParsersSyntEl_AlternativeList: public ToolsParsersSyntEl_ParameterGroup
{
  public:
    /*! \brief function implementing the virtual parse function of
        class ToolsParsersSyntEl_ParameterBase for class
        ToolsParsersSyntEl_AlternativeList.
        
        Parsing in array arrayToParse, starting at the first array element. The
        child objects organized by a ToolsParsersSyntEl_AlternativeList will be
        changed accordingly during the parse function, as thier parse functions
        are called. The first list element matching during a parse run wins, so
        sequence of addition of the list members can be important (e.g. with
        syntax "x|\<y\>", and actual parameter "x", keyword "x" will be found
        without a syntax error, but variable \<y\> will not be found.
        
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
        class ToolsParsersSyntEl_ParameterBase for class
        ToolsParsersSyntEl_AlternativeList.
        
        Simulating the parsing (parse ahead) in array arrayToParse starting at
        the first array element. Function simulateParse() does not change
        anything other than the valid state of the object and of the child
        objects managed by it.
        
        \param arrayToParse     [IN]  array containing the parameter strings
            to parse
        \param sizeOfArray      [IN]  size of arrayToParse
        \param parsedUpTo       [OUT] number of first unparsed element 
        \param foundSyntaxError [OUT] true if a syntax error was found during
            parsing, false otherwise
        \param foundSomething   [OUT] true if a managed child object could be
            parsed successfully (note 0==parsedUpTo means the child object was
            optional or an empty list!)
        \return true if the object and its children are still valid (no memory
            allocation problems during simulating parsing or before), false
            otherwise */
    virtual bool simulateParse(
        const char * const * arrayToParse,
        const int            sizeOfArray,
        int                & parsedUpTo,
        bool               & foundSyntaxError,
        bool               & foundSomething);

    /*! \brief implementing virtual printSyntax() function for class
        ToolsParsersSyntEl_AlternativeList

        The function prints the alternative list managed by the object into a
        buffer. The list is enclosed in round brackets unless the output style
        is simple and the list contains just one element.

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
};

#endif
