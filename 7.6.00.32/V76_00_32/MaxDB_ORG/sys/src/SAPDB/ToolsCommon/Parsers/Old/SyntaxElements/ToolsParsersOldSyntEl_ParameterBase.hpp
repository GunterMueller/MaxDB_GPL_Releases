/*!
    \file    ToolsParsersOldSyntEl_ParameterBase.hpp
    \author  TiloH
    \ingroup backup tool simulators for tests
    \brief   declaration of a base class for command line parts 

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

#if !defined(ToolsParsersOldSyntEl_ParameterBase_HPP)
#define ToolsParsersOldSyntEl_ParameterBase_HPP

#include <string.h>

/*! \brief a base class for different program parameter types */
class ToolsParsersOldSyntEl_ParameterBase
{
  public:
    /*! a simple constuctor */
    ToolsParsersOldSyntEl_ParameterBase():valid(true),found(false){}
    /*! a virtual destructor to be prepared for the future */
    virtual ~ToolsParsersOldSyntEl_ParameterBase() {}

    virtual bool printSyntax(char* outText, size_t spaceLeft) = 0;

    /*! \brief abstract virtual function for parsing parameter into the object
        
        Parsing in array arrayToParse starting at the first array element. The object will change
        is status, value, etc. during that function according to the actual parameters found in the array.
        
        \param arrayToParse     [IN]  array containing the parameter strings to parse
        \param sizeOfArray      [IN]  size of arrayToParse
        \param parsedUpTo       [OUT] number of first unparsed element
        \param foundSyntaxError [OUT] true if a syntax error was found during parsing, false otherwise
        \return true if the parsing object is still valid (no memory allocation problems during parsing or before), false otherwise */
    virtual bool parse(const char * const * arrayToParse,
                       const int            sizeOfArray,
                       int                & parsedUpTo,
                       bool               & foundSyntaxError)=0;

    /*! \brief abstract virtual function for simulating the parsing of parameter(s) into the object
        
        Simulating the parsing (parse ahead) in array arrayToParse starting at the first array element.
        simulateParse does if at all change the valid state of the object and of the child objects
        managed by it.
        
        \param arrayToParse     [IN]  array containing the parameter strings to parse
        \param sizeOfArray      [IN]  size of arrayToParse
        \param parsedUpTo       [OUT] number of first unparsed element 
        \param foundSyntaxError [OUT] true if a syntax error was found during parsing, false otherwise
        \param foundSomething   [OUT] true if object could be parsed successfully (note 0==parsedUpTo means the child object was optional or an empty list!)
        \return true if the object and all its children are still valid (no memory allocation problems during simulating parsing or before), false otherwise */
    virtual bool simulateParse(const char * const * arrayToParse,
                               const int            sizeOfArray,
                               int                & parsedUpTo,
                               bool               & foundSyntaxError,
                               bool               & foundSomething)=0;


    /*! function for determining the validity of the object
    
        \return true if object and any child objects are valid (no memory problems occured so far), false otherwise */
    bool isValid(){return valid;}

    /*! function for determining the presence of the parameter (or parameter group) after parsing */
    bool isSet(){return found;}

  protected:
    bool valid; //!<state of the object (true if object can be normally used, false otherwise)
    bool found; //!<state of the parameter represented by the object, \see isSet()

    /*so far unused function - maybe it is of use later
    virtual void unsetFoundFlag(){found=false;}*/
};

#endif
