/*!
    \file    ToolsParsersOldSyntEl_Variable.hpp
    \author  TiloH
    \ingroup backup tool simulators for tests
    \brief   declaration of a class for variable command line parts 

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

#if !defined(ToolsParsersOldSyntEl_Variable_HPP)
#define ToolsParsersOldSyntEl_Variable_HPP

#include "ToolsCommon/Parsers/Utilities/ToolsParsersUtil_String.hpp"
#include "ToolsCommon/Parsers/Old/SyntaxElements/ToolsParsersOldSyntEl_ParameterBase.hpp"

/*! \brief a base class for variable program parameters such as dbmcli ... <database>

    If that variable is encountered more than once during parsing, only the last value is
    stored in the object. If you need all values for that variable use class
    ToolsParsersOldSyntEl_MultipleValueVariable
    \see ToolsParsersOldSyntEl_MultipleValueVariable */
class ToolsParsersOldSyntEl_Variable:public ToolsParsersOldSyntEl_ParameterBase {
public:
    /*! \brief constructor accepting the variableToken, that would be <database>
        in the example "dbmcli ... <database> ..." */
    ToolsParsersOldSyntEl_Variable(const char * syntaxToken, const char* defaultValue = 0);

    /* \brief function for accessing the parsed value */
    const char * getValue(){return (const char *)value;}

    virtual bool printSyntax(char* outText, size_t spaceLeft);

protected:
    
    /*! \brief function implementing the virtual parse function of ToolsParsersOldSyntEl_ParameterBase
        for ToolsParsersOldSyntEl_Variable.
        
        Parsing the first element in array arrayToParse. If there is such a first 
        element, it is  copied into the value if this object and no syntax error did occure.
        
        \param arrayToParse     [IN]  array containing the parameter strings to parse
        \param sizeOfArray      [IN]  size of arrayToParse
        \param parsedUpTo       [OUT] number of first unparsed element (1 if no syntax error occurred, 0 otherwise)
        \param foundSyntaxError [OUT] true if a syntax error was found during parsing, false otherwise
        \return true if the object is still valid (no memory allocation problems during parsing or before), false otherwise */
    virtual bool parse(const char * const * arrayToParse,
                       const int sizeOfArray,
                       int & parsedUpTo,
                       bool & foundSyntaxError);

    /*! \brief function implementing the virtual simulateParse function of ToolsParsersOldSyntEl_ParameterBase
        for ToolsParsersOldSyntEl_Variable.
        
        Simulating the parsing (parse ahead) in array arrayToParse
        starting at the first array element.
        
        \param arrayToParse     [IN]  array containing the parameter strings to parse
        \param sizeOfArray      [IN]  size of arrayToParse
        \param parsedUpTo       [OUT] number of first unparsed element (1 if no syntax error occurred, 0 otherwise)
        \param foundSyntaxError [OUT] true if a syntax error was found during parsing, false otherwise
        \param foundSomething   [OUT] true if first element of arrayToParse existed
        \return true if the object is still valid (no memory allocation problems during parsing or before), false otherwise */
    virtual bool simulateParse(const char * const * arrayToParse,
                               const int            sizeOfArray,
                               int                & parsedUpTo,
                               bool               & foundSyntaxError,
                               bool               & foundSomething);
    
private:
    ToolsParsersUtil_String name;   //!< member holding the syntactical name of the parameter, unused so far
    ToolsParsersUtil_String value;  //!< member holding the last parsed value of the parameter
};

#endif
