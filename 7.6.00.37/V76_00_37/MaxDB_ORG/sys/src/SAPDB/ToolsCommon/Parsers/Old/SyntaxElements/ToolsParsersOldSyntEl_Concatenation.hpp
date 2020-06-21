/*!
    \file    ToolsParsersOldSyntEl_Concatenation.hpp
    \author  TiloH
    \ingroup backup tool simulators for tests
    \brief   declaration of a class for handling a list of succeeding parameters  

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

#if !defined (ToolsParsersOldSyntEl_Concatenation_HPP)
#define ToolsParsersOldSyntEl_Concatenation_HPP

#include "ToolsCommon/Parsers/Old/SyntaxElements/ToolsParsersOldSyntEl_ParameterGroup.hpp"

/*! \brief A class for handling a group of succeeding parameters such as "dbmcli -d <dbname> -u <user>,<passwd> ..."

    To add parameters to the group use the push_back() function.
    \see ToolsParsersOldSyntEl_ParameterGroup::push_back */
class ToolsParsersOldSyntEl_Concatenation: public ToolsParsersOldSyntEl_ParameterGroup
{
  protected:
    /*! \brief function implementing the virtual parse function of ToolsParsersOldSyntEl_ParameterBase
        for ToolsParsersOldSyntEl_Concatenation.
        
        Parsing in array arrayToParse starting at the first
        array element. The child objects organized by a ToolsParsersOldSyntEl_Concatenation will be
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
        for ToolsParsersOldSyntEl_Concatenation.
        
        Simulating the parsing (parse ahead) in array arrayToParse
        starting at the first array element. simulateParse does not change anything other than the valid state
        of the object and of the child objects managed by it.
        
        \param arrayToParse     [IN]  array containing the parameter strings to parse
        \param sizeOfArray      [IN]  size of arrayToParse
        \param parsedUpTo       [OUT] number of first unparsed element 
        \param foundSyntaxError [OUT] true if a syntax error was found during parsing, false otherwise
        \param foundSomething   [OUT] true if all managed child objects can be parsed successfully
        \return true if the object and its children are still valid (no memory allocation problems
                during simulating parsing or before), false otherwise */
    virtual bool simulateParse(const char * const * arrayToParse,
                               const int            sizeOfArray,
                               int                & parsedUpTo,
                               bool               & foundSyntaxError,
                               bool               & foundSomething);

public:
    virtual bool printSyntax(char* outText, size_t spaceLeft);
};

#endif
