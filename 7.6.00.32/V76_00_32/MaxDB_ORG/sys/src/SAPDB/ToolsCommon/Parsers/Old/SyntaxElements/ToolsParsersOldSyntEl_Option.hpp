/*!
    \file    ToolsParsersOldSyntEl_Option.hpp
    \author  TiloH
    \ingroup backup tool simulators for tests
    \brief   declaration of a class for handling a list of optional parameters  

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

#if !defined(ToolsParsersOldSyntEl_Option_HPP)
#define ToolsParsersOldSyntEl_Option_HPP

#include "ToolsCommon/Parsers/Old/SyntaxElements/ToolsParsersOldSyntEl_ParameterBase.hpp"

/*! \brief a base class for handling lists of optional program parameters such as dbmcli ... [UNTIL \<date\> \<time\>]

    use the push_back function of base class ToolsParsersOldSyntEl_ParameterGroup for adding optional
    parameters to the list. Effects are undefined, if one syntax part is added to more than one
    such list using push_back, so don't do it.
    \see ToolsParsersOldSyntEl_ParameterGroup::push_back */
    
class ToolsParsersOldSyntEl_Option: public ToolsParsersOldSyntEl_ParameterBase {
public:
    ToolsParsersOldSyntEl_Option(ToolsParsersOldSyntEl_ParameterBase* optionalElement);
    virtual ~ToolsParsersOldSyntEl_Option() {}

    virtual bool printSyntax(char* outText, size_t spaceLeft);

protected:
    /*! \brief function implementing the virtual parse function of ToolsParsersOldSyntEl_ParameterBase
        for ToolsParsersOldSyntEl_Option.
        
        Parsing in array arrayToParse starting at the first
        array element. The child objects organized by a ToolsParsersOldSyntEl_Option will be
        changed accordingly during the parse function, as thier parse functions are called.
        
        \param arrayToParse     [IN]  array containing the parameter strings to parse
        \param sizeOfArray      [IN]  size of arrayToParse
        \param parsedUpTo       [OUT] number of first unparsed element 
        \param foundSyntaxError [OUT] true if a syntax error was found during parsing, false otherwise
        \return true if the object and its children are still valid (no memory allocation problems during parsing or before), false otherwise */

    /*! \brief function for parsing a list of optional arguments
    
        Implements base classes virtual parse function and therefore all the parse
        work for a list of optional parameters */
    virtual bool parse(const char * const * arrayToParse,
                       const int            sizeOfArray,
                       int                & parsedUpTo,
                       bool               & foundSyntaxError);

    /*! \brief function implementing the virtual simulateParse function of ToolsParsersOldSyntEl_ParameterBase
        for ToolsParsersOldSyntEl_Option.
        
        Simulating the parsing (parse ahead) in array arrayToParse
        starting at the first array element. simulateParse does not change anything other than the valid state
        of the object and of the child objects managed by it.
        
        \param arrayToParse     [IN]  array containing the parameter strings to parse
        \param sizeOfArray      [IN]  size of arrayToParse
        \param parsedUpTo       [OUT] number of first unparsed element 
        \param foundSyntaxError [OUT] true if a syntax error was found during parsing, false otherwise
        \param foundSomething   [OUT] true if a managed child object could be parsed successfully (note 0==parsedUpTo means the child object was optional or an empty list!)
        \return true if the object and its children are still valid (no memory allocation problems during simulating parsing or before), false otherwise */
    virtual bool simulateParse(const char * const * arrayToParse,
                               const int            sizeOfArray,
                               int                & parsedUpTo,
                               bool               & foundSyntaxError,
                               bool               & foundSomething);

private:
    ToolsParsersOldSyntEl_ParameterBase* m_OptionalElement;
};
#endif

