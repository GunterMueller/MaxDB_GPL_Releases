/*!*****************************************************************************

  module      : X2DC_Attribute_Checker.hpp

  ----------------------------------------------------------------------------

  responsible : PeterG (GoertzP)

  special area: Generator of persistent class description code
  description : Perform checks not possible during validation
			
  ----------------------------------------------------------------------------

    ========== licence begin LGPL
    Copyright (C) 2000 SAP AG

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
    ========== licence end

*******************************************************************************/


#ifndef X2DC_ATTRIBUTECHECKER_HPP
#define X2DC_ATTRIBUTECHECKER_HPP


#include <string>
#include <stdio.h>

#include "idl/XML2DiscCpp/XML2DiscCpp.hpp"


/******************************************************************************/
/*     Attribute checker : Perfom checks not possible during validation       */
/******************************************************************************/

class Attribute_Checker
{
public:
  /* Constructor */
  Attribute_Checker(int * pError);
  /* Destructor */
  ~Attribute_Checker();
  /* Others */
  inline bool LengthValid(const char * ident);
  void        StartElement( int & error, const char * name, const char ** atts );
  void        EndElement(const char * name);
private:
  int * m_pError;
  int   m_isWithin_Import;
  bool  m_isWithin_Enum;
  bool  m_isWithin_GenEnum;
  bool  m_isWithin_Struct;
};

inline bool Attribute_Checker::LengthValid(const char * ident) {
  if ( (1 <= strlen(ident)) && (strlen(ident) <= nameLen) ) {
    return true;
  }
  else {
    return false;
  }
}


#endif /* X2DC_ATTRIBUTECHECKER_HPP */

