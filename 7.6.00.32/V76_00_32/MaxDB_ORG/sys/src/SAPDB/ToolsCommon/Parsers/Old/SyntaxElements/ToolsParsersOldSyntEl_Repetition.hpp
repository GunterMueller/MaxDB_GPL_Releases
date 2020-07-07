/*!
    \file    ToolsParsersOldSyntEl_Repetition.hpp
    \author  TiloH
    \ingroup backup tool simulators for tests
    \brief   declaration of a class for handling a list of self repeating parameters  

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

#if !defined(ToolsParsersOldSyntEl_Repetition_HPP)
#define ToolsParsersOldSyntEl_Repetition_HPP

#include <limits.h>

#include "SAPDB/ToolsCommon/Tools_List.hpp"
#include "ToolsCommon/Parsers/Utilities/ToolsParsersUtil_String.hpp"
#include "ToolsCommon/Parsers/Old/SyntaxElements/ToolsParsersOldSyntEl_ParameterBase.hpp"


/*! \brief A base class for self repeating program parameters such as "dbmcli ... [-d ...] [-u ...] ..."
    (or more precisely written EBNF-like as "dbmcli {... [-d ...] [-u ...] ...} <command> ...").
    The parameters in this list are parsed repeatedly until nothing new was parsed. If a parameter
    is found more than once it depends on the used parameter object, whether only
    the last or a list of all values will be stored.
    
    To add parameters to the group use the push_back() function.
    \see ToolsParsersOldSyntEl_ParameterGroup::push_back

    If you want more than one value for one parameter, use DBMBToolSimParam_MultipleValueList
    \see DBMBToolSimParam_MultipleValueList */

class ToolsParsersOldSyntEl_Repetition: public ToolsParsersOldSyntEl_ParameterBase {
public:
    /*! \brief constructor accepting nothing, use the push_back function for
        adding parameters to the list */
    ToolsParsersOldSyntEl_Repetition( ToolsParsersOldSyntEl_ParameterBase* repeatedElement);
    virtual ~ToolsParsersOldSyntEl_Repetition() {}

    virtual bool printSyntax(char* outText, size_t spaceLeft);

protected:
    /*! \brief function implementing the virtual parse function of ToolsParsersOldSyntEl_ParameterBase
        for ToolsParsersOldSyntEl_Repetition.
        
        Parsing in array arrayToParse starting at the first
        array element. The child objects organized by a ToolsParsersOldSyntEl_Repetition will be
        changed accordingly during the parse function, as thier parse functions are called.
        
        \param arrayToParse     [IN]  array containing the parameter strings to parse
        \param sizeOfArray      [IN]  size of arrayToParse
        \param parsedUpTo       [OUT] number of first unparsed element 
        \param foundSyntaxError [OUT] true if a syntax error was found during parsing, false otherwise
        \return true if the object and its children are still valid (no memory allocation problems during parsing or before), false otherwise */
    virtual bool parse(const char * const * arrayToParse,
                       const int            sizeOfArray,
                       int                & parsedUpTo,
                       bool               & foundSyntaxError);

    /*! \brief function implementing the virtual simulateParse function of ToolsParsersOldSyntEl_ParameterBase
        for ToolsParsersOldSyntEl_Repetition.
        
        Simulating the parsing (parse ahead) in array arrayToParse
        starting at the first array element. simulateParse does not change anything other than the valid state
        of the object and of the child objects managed by it.
        
        \param arrayToParse     [IN]  array containing the parameter strings to parse
        \param sizeOfArray      [IN]  size of arrayToParse
        \param parsedUpTo       [OUT] number of first unparsed element 
        \param foundSyntaxError [OUT] true if a syntax error was found during parsing, false otherwise
        \param foundSomething   [OUT] true if one example a managed child object could be parsed successfully (note 0==parsedUpTo means the child object was optional or we had an empty list!)
        \return true if the object and its children are still valid (no memory allocation problems during simulating parsing or before), false otherwise */
    virtual bool simulateParse(const char * const * arrayToParse,
                               const int            sizeOfArray,
                               int                & parsedUpTo,
                               bool               & foundSyntaxError,
                               bool               & foundSomething);

private:
    ToolsParsersOldSyntEl_ParameterBase* m_RepeatedElement;
};

#endif
