/*!
    \file    ToolsParsersOldSyntEl_MultipleValueVariable.hpp
    \author  TiloH
    \ingroup backup tool simulators for tests
    \brief   declaration of a class for variable command line parts, that stores all parsed
             values 

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

#if !defined(ToolsParsersOldSyntEl_MultipleValueVariable_HPP)
#define ToolsParsersOldSyntEl_MultipleValueVariable_HPP

#include "SAPDB/ToolsCommon/Tools_Vector.hpp"
#include "ToolsCommon/Parsers/Utilities/ToolsParsersUtil_String.hpp"
#include "ToolsCommon/Parsers/Old/SyntaxElements/ToolsParsersOldSyntEl_ParameterBase.hpp"

/*! \brief a base class for variable program parameters with multiple values such
    as 'rm ... \<filename<sub>1</sub>\> \<filename<sub>2</sub>\> ...'
    
    As the list of values is managed by the object itself, do not access the values
    after deleting the if this object */
class ToolsParsersOldSyntEl_MultipleValueVariable:public ToolsParsersOldSyntEl_ParameterBase
{
  public:
    /*! \brief A constructor accepting the variables syntax token, that would be \<filename\>
        for the EBNF-like "rm ... <filename> {<filename>}"
        \param syntaxToken [IN] the name of the variable in the syntax string */
    ToolsParsersOldSyntEl_MultipleValueVariable(const char * syntaxToken);
    
    /*! @brief constructor accepting a list of default values */
    ToolsParsersOldSyntEl_MultipleValueVariable(
            const char * syntaxToken, 
            const Tools_Vector<ToolsParsersUtil_String *> defaultValues);
    
    /*! \brief A destructor deleting also all value objects, added to this
        object during parsing */
    virtual ~ToolsParsersOldSyntEl_MultipleValueVariable();

    /* \brief A function returning the number of values already parsed into the object
       \return number of values available */
    unsigned int getNumberOfValues() const {return (int)values.size();}

    /*! \brief A function for accesing the first value parsed
        \return the first value that was parsed into the object */
    const char * getValue() const {return (const char *)(*(values[0]));}

    /*! \brief A function for accesing an arbitrary parsed value
        \param i [IN] number of the value that is to be accessed, if 0<=i<getNumberOfValues()
                      does not hold, effects are undetermined, so don't try it
        \return i-th value parsed into the object */
    const char * getValue(size_t i) const {return (const char *)(*(values[i]));}

  protected:
    /*! \brief function implementing the virtual parse function of ToolsParsersOldSyntEl_ParameterBase
        for ToolsParsersOldSyntEl_MultipleValueVariable.
        
        Parsing the first element in array arrayToParse. If there is such a first 
        element, it is added to the list of values managed by this object and no
        syntax error did occure.
        
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
        for ToolsParsersOldSyntEl_MultipleValueVariable.
        
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

public:
    virtual bool printSyntax(char* outText, size_t spaceLeft);

private:
    ToolsParsersUtil_String name; //!< member holding the syntactical name of the parameter, unused so far
    Tools_Vector<ToolsParsersUtil_String *> values; //!<member holding the values parsed into this object
};

#endif
