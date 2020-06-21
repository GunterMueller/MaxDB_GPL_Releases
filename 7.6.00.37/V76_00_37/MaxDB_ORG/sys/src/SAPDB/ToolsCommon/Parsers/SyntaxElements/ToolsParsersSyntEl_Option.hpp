/*!
    \file    ToolsParsersSyntEl_Option.hpp
    \author  TiloH
    \ingroup syntax elements for parsers
    \brief   declaration of a class for handling a list of optional parameters  

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

#if !defined(TOOLSPARSERSSYNTEL_OPTION_HPP)
#define TOOLSPARSERSSYNTEL_OPTION_HPP

#include "ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_NonTerminal.hpp"

/*! \brief a base class for handling lists of optional program parameters such
    as "dbmcli ... [UNTIL \<date\> \<time\>]"

    Use the push_back function of base class ToolsParsersSyntEl_ParameterGroup
    for adding optional parameters to the list. Effects are undefined, if one
    syntax part is added to more than one such list using push_back, so don't
    do it.

    \see ToolsParsersSyntEl_ParameterGroup::push_back */
    
class ToolsParsersSyntEl_Option: public ToolsParsersSyntEl_NonTerminal
{
  public:
    /*! \brief constructor
    
        The newly create object will contain a pointer to the parameter or
        parameter group that is optional.
        
        \param optionalElement [IN] optional parameter or parameter group */
    ToolsParsersSyntEl_Option(
        ToolsParsersSyntEl_ParameterBase * optionalElement);
    
    /*! \brief destructor */
    virtual ~ToolsParsersSyntEl_Option();

    /*! \brief function implementing the virtual parse function of
        ToolsParsersSyntEl_ParameterBase for ToolsParsersSyntEl_Option.
        
        Parsing in array arrayToParse starting at the first array element. The
        child object managed by a ToolsParsersSyntEl_Option will be changed
        accordingly during the parse function, as its parse function is called.
        
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

    /*! \brief function implementing the virtual simulateParse() function of
        ToolsParsersSyntEl_ParameterBase for ToolsParsersSyntEl_Option.
        
        Simulating the parsing (parse ahead) in array arrayToParse starting at
        the first array element. simulateParse() does not change anything other
        than the valid state of the object or the child object managed by it.
        
        \param arrayToParse     [IN]  array containing the parameter strings to
            parse
        \param sizeOfArray      [IN]  size of arrayToParse
        \param parsedUpTo       [OUT] number of first unparsed element 
        \param foundSyntaxError [OUT] true if a syntax error was found during
            parsing, false otherwise
        \param foundSomething   [OUT] true if a managed child object could be
            parsed successfully (note 0==parsedUpTo means the child object was
            optional or an empty list!)
        \return true if the object and its children are still valid (no memory
            allocation problems during simulating parsing or before), false otherwise */
    virtual bool simulateParse(
        const char * const * arrayToParse,
        const int            sizeOfArray,
        int                & parsedUpTo,
        bool               & foundSyntaxError,
        bool               & foundSomething);

    /*! \brief virtual function for printing a optional element into a
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
        bool              & spaceTooSmall) const;

  private:
    ToolsParsersSyntEl_ParameterBase * m_OptionalElement;
};

#endif
