/*!
    \file    ToolsParsersSyntEl_Repetition.hpp
    \author  TiloH
    \ingroup syntax elements for parsers
    \brief   declaration of a class for handling a list of self repeating
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

#if !defined(TOOLSPARSERSSYNTEL_REPETITION_HPP)
#define TOOLSPARSERSSYNTEL_REPETITION_HPP

#include <limits.h>

#include "SAPDB/ToolsCommon/Tools_List.hpp"
#include "ToolsCommon/Parsers/Utilities/ToolsParsersUtil_String.hpp"
#include "ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_NonTerminal.hpp"


/*! \brief A basic class for program parameters that can be repeated such as
    "dbmcli ... [-d ...] [-u ...] ..." (or more precisely written EBNF-like as
    "dbmcli {... [-d ...] [-u ...] ...} \<command\> ..."). Note that the
    sequence of the parameters must still be matched. If your syntax allowes
    arbitrary sequences of these parameters you should combine the repetition
    with the class ToolsParsersSyntEl_Set.
    
    A repetion must be found at least once (BNF: ()+), otherwise a syntax error
    is reported. Combine the repetition with the class
    ToolsParsersSyntEl_Option to allow repetitions of 0 or more times (EBNF: 
    {}, BNF: ()*).

    The parameters in this list are parsed repeatedly until nothing that
    matches is found anymore. If a parameter is found more than once it depends
    on the used parameter objects, whether only the last or a list of all
    values will be stored. If you want more than one value for one parameter,
    use ToolsParsersSyntEl_MultipleValueVariable.

    \see ToolsParsersSyntEl_ParameterGroup::push_back
    \see ToolsParsersSyntEl_Option
    \see ToolsParsersSyntEl_Set
    \see ToolsParsersSyntEl_MultipleValueVariable */
class ToolsParsersSyntEl_Repetition: public ToolsParsersSyntEl_NonTerminal
{
  public:
    /*! \brief constructor accepting the element that should be repeated

        \param repeatedElement [IN] the element/group that should be repeated */
    ToolsParsersSyntEl_Repetition(
        ToolsParsersSyntEl_ParameterBase * repeatedElement);

    /*! destructor, doing nothing */
    virtual ~ToolsParsersSyntEl_Repetition();

    /*! \brief function implementing the virtual parse function of
        ToolsParsersSyntEl_ParameterBase for ToolsParsersSyntEl_Repetition.
        
        Parsing in array arrayToParse starting at the first array element. The
        child object managed by a ToolsParsersSyntEl_Repetition will be changed
        accordingly during the parse function, as its parse functions is called.
        
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
        ToolsParsersSyntEl_ParameterBase for ToolsParsersSyntEl_Repetition.
        
        Simulating the parsing (parse ahead) in array arrayToParse starting at
        the first array element. simulateParse() does not change anything other
        than the valid state of the object and of the child object managed by
        it.
        
        \param arrayToParse     [IN]  array containing the parameter strings to
            parse
        \param sizeOfArray      [IN]  size of arrayToParse
        \param parsedUpTo       [OUT] number of first unparsed element 
        \param foundSyntaxError [OUT] true if a syntax error was found during
            parsing, false otherwise
        \param foundSomething   [OUT] true if one example a managed child
            object could be parsed successfully (note 0==parsedUpTo means the child object was optional or we had an empty list!)
        \return true if the object and its children are still valid (no memory allocation problems during simulating parsing or before), false otherwise */
    virtual bool simulateParse(
        const char * const * arrayToParse,
        const int            sizeOfArray,
        int                & parsedUpTo,
        bool               & foundSyntaxError,
        bool               & foundSomething);

    /*! \brief virtual function for printing a repetition into a
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
    ToolsParsersSyntEl_ParameterBase * m_RepeatedElement;
};

#endif
