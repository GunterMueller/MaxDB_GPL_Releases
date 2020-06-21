/*!*****************************************************************************

  module      : Ispc_InterfaceTypeList.cpp

  ----------------------------------------------------------------------------

  responsible : PeterG (GoertzP)

  special area: Generator of interface description code
  description : Linear list for supported C++ base types

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


#include "idl/xml2ispc/Ispc_InterfaceTypeList.hpp"
#include <string.h>


void Ispc_InterfaceType::setType(const char * type) {
  strcpy( &m_type[0], type );
}

/*----------------------------------------------------------------------------*/

void Ispc_InterfaceType::setDefault(const char * defValue) {
  strcpy( &m_default[0], defValue );
}

/*----------------------------------------------------------------------------*/

void Ispc_InterfaceType::setNext(Ispc_InterfaceType * next) {
  m_next = next;
}

/*----------------------------------------------------------------------------*/

Ispc_InterfaceTypeList::Ispc_InterfaceTypeList() {

  Ispc_InterfaceType * ifType = NULL;

  /* OmsTypeChar */
  m_first = new Ispc_InterfaceType;
  m_first->setType("OmsTypeChar");
  m_first->setDefault("' '");
  m_first->setNext(NULL);
  m_last = m_first;

  /* OmsTypeWyde */
  ifType = new Ispc_InterfaceType;
  ifType->setType("OmsTypeWyde");
  ifType->setDefault("' '");
  ifType->setNext(m_first);
  m_first = ifType;

  /* OmsTypeNumc */
  ifType = new Ispc_InterfaceType;
  ifType->setType("OmsTypeNumc");
  ifType->setDefault("'0'");
  ifType->setNext(m_first);
  m_first = ifType;

  /* OmsTypeUInt1 */
  ifType = new Ispc_InterfaceType;
  ifType->setType("OmsTypeUInt1");
  ifType->setDefault("0");
  ifType->setNext(m_first);
  m_first = ifType;

  /* OmsTypeInt4 */
  ifType = new Ispc_InterfaceType;
  ifType->setType("OmsTypeInt4");
  ifType->setDefault("0");
  ifType->setNext(m_first);
  m_first = ifType;

  /* OmsTypeByte */
  ifType = new Ispc_InterfaceType;
  ifType->setType("OmsTypeByte");
  ifType->setDefault("0x00");
  ifType->setNext(m_first);
  m_first = ifType;

}

/*----------------------------------------------------------------------------*/

Ispc_InterfaceTypeList::~Ispc_InterfaceTypeList() {

  Ispc_InterfaceType * ifType = NULL;

  while (m_first) {
    ifType = m_first;
    m_first = ifType->getNext();
    delete ifType;
  }

}

/*----------------------------------------------------------------------------*/

bool Ispc_InterfaceTypeList::isRegistered(const char * name) const {

  Ispc_InterfaceType * ifType = m_first;
  bool found = false;

  while ( (!found) && (ifType) ) {
    if (0 == strcmp( ifType->getType(), name )) {
      found = true;
    }
    else {
      ifType = ifType->getNext();
    }
  }

  return found;

}

/*----------------------------------------------------------------------------*/

const Ispc_InterfaceType * Ispc_InterfaceTypeList::findType(const char * type) const {

  Ispc_InterfaceType * ifType = m_first;
  bool found = false;

  while ( (!found) && (ifType) ) {
    if (0 == strcmp( ifType->getType(), type )) {
      found = true;
    }
    else {
      ifType = ifType->getNext();
    }
  }

  return ifType;

}
  
/*----------------------------------------------------------------------------*/

void Ispc_InterfaceTypeList::registerType( const char * name,
                                           const char * defValue ) {

  Ispc_InterfaceType * newIfType = new Ispc_InterfaceType;
  newIfType->setType(name);
  newIfType->setDefault(defValue);
  newIfType->setNext(NULL);

  m_last->setNext(newIfType);
  m_last = newIfType;

}

/*----------------------------------------------------------------------------*/

bool Ispc_InterfaceTypeList::skipTypeReg(const char * name) const {

  if ( 0 == strcmp( name, "OmsTypeInt64" )    ||
       0 == strcmp( name, "OmsTypeUInt64" )   ||
       0 == strcmp( name, "OmsTypeInt8" )     ||
       0 == strcmp( name, "OmsTypeUInt8" )    ||
       0 == strcmp( name, "DbpTypeUnicode" )  ||
       0 == strcmp( name, "OmsTypeAscii7" )   ||
       0 == strcmp( name, "OmsTypeUTF8" )     ||
       0 == strcmp( name, "OmsTypeInt1" )     ||
       0 == strcmp( name, "OmsTypeUInt4" )    ||
       0 == strcmp( name, "OmsTypeBool" )     ||
       0 == strcmp( name, "OmsSchemaHandle" ) ||
       0 == strcmp( name, "OmsContainerNo" ) ) {
    return true;
  }
  else {
    return false;
  }

}

