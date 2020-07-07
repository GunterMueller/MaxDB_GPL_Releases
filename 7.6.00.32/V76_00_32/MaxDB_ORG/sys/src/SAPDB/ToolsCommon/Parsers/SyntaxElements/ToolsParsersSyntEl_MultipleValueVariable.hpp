/*!
    \file    ToolsParsersSyntEl_MultipleValueVariable.hpp
    \author  TiloH
    \ingroup syntax elements for parsers
    \brief   declaration of a class for variable command line parts, that
             stores all parsed values 

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

#if !defined(TOOLSPARSERSSYNTEL_MULTIPLEVALUEVARIABLE_HPP)
#define TOOLSPARSERSSYNTEL_MULTIPLEVALUEVARIABLE_HPP

#include "SAPDB/ToolsCommon/Tools_Vector.hpp"
#include "ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_Terminal.hpp"

/*! \brief a base class for variable program parameters with multiple values
    such as "rm ... \<filename<sub>1</sub>\> \<filename<sub>2</sub>\> ..."
    
    As the list of values is managed by the object itself, do not access the
    values after deleting this object. */
class ToolsParsersSyntEl_MultipleValueVariable:public ToolsParsersSyntEl_Terminal
{
  public:
    /*! \brief A constructor accepting the variables syntax token, that would
        be \<filename\> for the EBNF-like "rm ... \<filename\> {\<filename\>}"
        
        \param syntaxToken [IN] the name of the variable in the syntax string */
    ToolsParsersSyntEl_MultipleValueVariable(const char * syntaxToken);
    /*! \brief A destructor deleting also all value objects, added to this
        object during parsing */
    virtual ~ToolsParsersSyntEl_MultipleValueVariable();

    /*! \brief A function returning the number of values already parsed into the
        object

        \return number of values available */
    size_t getNumberOfValues() const {return m_Values.size();}

    /*! \brief A function for accesing the first value parsed into the object

        The memory of the returned value is deleted together with this object,
        so do not use the pointer longer than this object exists.

        \return the first value that was parsed into the object */
    const char * getValue() const {return (const char *)(*(m_Values[0]));}

    /*! \brief A function for accesing all parsed values

        The memory of the returned value is deleted together with this object,
        so do not use the pointer longer than this object exists.

        \param i [IN] number of the value that is to be accessed,
                      if 0<=i<getNumberOfValues() does not hold, effects are
                      undetermined, so don't try it
        \return i-th value parsed into the object */
    const char * getValue(size_t i) const {return (const char *)(*(m_Values[i]));}

    /*! \brief function implementing the virtual parse function of
        ToolsParsersSyntEl_ParameterBase for
        ToolsParsersSyntEl_MultipleValueVariable.
        
        Parsing the first element in array arrayToParse. If there is such a
        first element, it is added to the list of values managed by this object
        and no syntax error did occure.
        
        \param arrayToParse     [IN]  array containing the parameter strings to
            parse
        \param sizeOfArray      [IN]  size of arrayToParse
        \param parsedUpTo       [OUT] number of first unparsed element (1 if no
            syntax error occurred, 0 otherwise)
        \param foundSyntaxError [OUT] true if a syntax error was found during
            parsing, false otherwise
        \return true if the object is still valid (no memory allocation
            problems during parsing or before), false otherwise */ 
    virtual bool parse(const char * const * arrayToParse,
                       const int sizeOfArray,
                       int & parsedUpTo,
                       bool & foundSyntaxError);

    /*! \brief function implementing the virtual simulateParse function of
        ToolsParsersSyntEl_ParameterBase for
        ToolsParsersSyntEl_MultipleValueVariable.
        
        Simulating the parsing (parse ahead) in array arrayToParse
        starting at the first array element.
        
        \param arrayToParse     [IN]  array containing the parameter strings to
            parse
        \param sizeOfArray      [IN]  size of arrayToParse
        \param parsedUpTo       [OUT] number of first unparsed element (1 if no
            syntax error occurred, 0 otherwise)
        \param foundSyntaxError [OUT] true if a syntax error was found during
            parsing, false otherwise
        \param foundSomething   [OUT] true if first element of arrayToParse
            existed
        \return true if the object is still valid (no memory allocation
            problems during parsing or before), false otherwise */
    virtual bool simulateParse(const char * const * arrayToParse,
                               const int            sizeOfArray,
                               int                & parsedUpTo,
                               bool               & foundSyntaxError,
                               bool               & foundSomething);

    /*! \brief implementing virtual printSyntax() function for class
        ToolsParsersSyntEl_MultipleValueVariable

        The function prints the variable name enclosed into angle brackets into
        the buffer. If the variable name contains already enclosing angle
        brackets, no second pair of brackets is added.

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

  private:
    Tools_Vector<ToolsParsersUtil_String *> m_Values; //!<member holding the values parsed into this object
};

#endif
