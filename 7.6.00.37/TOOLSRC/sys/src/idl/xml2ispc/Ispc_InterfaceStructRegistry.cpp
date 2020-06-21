/*!*****************************************************************************

  module      : Ispc_InterfaceStructRegistry.cpp

  ----------------------------------------------------------------------------

  responsible : PeterG (GoertzP)

  special area: Generator of interface description code
  description : Registry for C-structs

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


#include "idl/xml2ispc/Ispc_InterfaceStructRegistry.hpp"


/* PTS 1134734 */
Ispc_Synonym::Ispc_Synonym(const char * name) {

  strcpy( &m_name[0], name );
  m_next = NULL;

}

/*----- PTS 1134734 ----------------------------------------------------------*/

void Ispc_Synonym::setNext(Ispc_Synonym * ifSynonym) {
  m_next = ifSynonym;
}

/*----------------------------------------------------------------------------*/

Ispc_Member::Ispc_Member(const char * name) {

  strcpy( &m_name[0], name );
  m_typeStruct = NULL;
  m_typeScalar = false; /* PTS 1134734 */
  m_next       = NULL;

}

/*----------------------------------------------------------------------------*/

bool Ispc_Member::setTypeStruct(Ispc_InterfaceStruct * ifStruct) { /* PTS 1134734 */

  if (m_typeScalar) {
    return false;
  }
  else {
    m_typeStruct = ifStruct;
    return true;
  }

}

/*----- PTS 1134734 ----------------------------------------------------------*/

bool Ispc_Member::setTypeScalar() {

  if (m_typeStruct) {
    return false;
  }
  else {
    m_typeScalar = true;
    return true;
  }

}

/*----- PTS 1134734 ----------------------------------------------------------*/

const bool Ispc_Member::typeIsRegistered() const {

  if ( m_typeStruct || m_typeScalar ) {
    return true;
  }
  else {
    return false;
  }

}

/*----------------------------------------------------------------------------*/

void Ispc_Member::setNext(Ispc_Member * next) {
  m_next = next;
}

/*----------------------------------------------------------------------------*/

void Ispc_Member::addToEnum( std::string & colEnum, const char * entry, unsigned int & colNo ) {

  int entryLen = strlen(entry);
  char * entryCont = new char[entryLen+3+nameLen+1];
  char number[16];
  memset( number, '\0', sizeof(number) );

  strcpy( entryCont, entry );
  strcpy( &entryCont[entryLen], "___" );
  strcpy( &entryCont[entryLen+3], m_name );

  if (m_typeStruct) {
    m_typeStruct->addMembersToEnum( colEnum, entryCont, colNo );
  }
  else {
    if (0 == colNo) {
      colEnum.append("  ");
    }
    else {
      colEnum.append(", ");
    }
    colEnum.append(entryCont);
    sprintf( number, " = %d", colNo );
    colEnum.append(number);
    colEnum.append("\n");
    colNo++;
  }

  delete [] entryCont;

}

/*----------------------------------------------------------------------------*/

Ispc_InterfaceStruct::Ispc_InterfaceStruct(const char * name) {

  strcpy( &m_name[0], name );
  /* PTS 1134734 */
  m_synonym     = NULL;
  m_firstMember = NULL;
  m_lastMember  = NULL;
  m_next        = NULL;

}

/*----------------------------------------------------------------------------*/

Ispc_InterfaceStruct::~Ispc_InterfaceStruct() {

  /* delete synonyms */
  Ispc_Synonym * ifSynonym = NULL;
  while (m_synonym) {
    ifSynonym = m_synonym;
    m_synonym = ifSynonym->getNext();
    delete ifSynonym;
  }

  /* delete members */
  Ispc_Member * member = NULL;
  while (m_firstMember) {
    member = m_firstMember;
    m_firstMember = member->getNext();
    delete member;
  }

}

/*----------------------------------------------------------------------------*/

void Ispc_InterfaceStruct::setNext(Ispc_InterfaceStruct * ifStruct) {
  m_next = ifStruct;
}

/*----- PTS 1134734 ----------------------------------------------------------*/

void Ispc_InterfaceStruct::addSynonym(const char * name) {

  Ispc_Synonym * newIfSynonym = new Ispc_Synonym(name);

  if (NULL == m_synonym) {
    m_synonym = newIfSynonym;
  }
  else {
    Ispc_Synonym * ifSynonym = m_synonym;
    while (NULL != ifSynonym->getNext()) {
      ifSynonym = ifSynonym->getNext();
    }
    ifSynonym->setNext(newIfSynonym);
  }

}

/*----- PTS 1134734 ----------------------------------------------------------*/

bool Ispc_InterfaceStruct::isSynonym(const char * name) const {

  bool found = false;
  Ispc_Synonym * ifSynonym = m_synonym;

  while ( (!found) && (ifSynonym) ) {
    if (0 == strcmp( ifSynonym->getName(), name )) {
      found = true;
    }
    else {
      ifSynonym = ifSynonym->getNext();
    }
  }

  return found;

}

/*----------------------------------------------------------------------------*/

void Ispc_InterfaceStruct::addMember(Ispc_Member * member) {

  if (NULL == m_firstMember) {
    m_firstMember = member;
  }
  else {
    m_lastMember->setNext(member);
  }

  m_lastMember = member;

}

/*----- PTS 1134734 ----------------------------------------------------------*/

const bool Ispc_InterfaceStruct::allMemberTypesRegistered() const {

  Ispc_Member * member = m_firstMember;

  while (member) {
    if (!(member->typeIsRegistered())) {
      return false;
    }
    member = member->getNext();
  }

  return true;

}

/*----------------------------------------------------------------------------*/

void Ispc_InterfaceStruct::genColEnum(std::string & colEnum) {

  unsigned int colNo = 0;

  colEnum = "typedef enum {";
  colEnum.append("\n");

  Ispc_Member * nextMember = getFirstMember();

  while (nextMember) {
    nextMember->addToEnum( colEnum, m_name, colNo );
    nextMember = nextMember->getNext();
  }

  colEnum.append("} ");
  colEnum.append(&m_name[0]);
  colEnum.append("___ColEnum;");
  colEnum.append("\n");

}

/*----------------------------------------------------------------------------*/

void Ispc_InterfaceStruct::addMembersToEnum( std::string & colEnum, const char * entry, unsigned int & colNo ) {

  int entryLen = strlen(entry);
  char * entryCont = new char[entryLen+3+nameLen+1];
  char number[16];
  memset( number, '\0', sizeof(number) );

  strcpy( entryCont, entry );
  strcpy( &entryCont[entryLen], "___" );

  Ispc_Member * nextMember = getFirstMember();

  while (nextMember) {
    strcpy( &entryCont[entryLen+3], nextMember->getName() );
    Ispc_InterfaceStruct * ifStruct = nextMember->getTypeStruct();
    if (ifStruct) {
      ifStruct->addMembersToEnum( colEnum, entryCont, colNo );
    }
    else {
      if (0 == colNo) {
        colEnum.append("  ");
      }
      else {
        colEnum.append(", ");
      }
      colEnum.append(entryCont);
      sprintf( number, " = %d", colNo );
      colEnum.append(number);
      colEnum.append("\n");
      colNo++;
    }
    nextMember = nextMember->getNext();
  }

  delete [] entryCont;

}

/*----------------------------------------------------------------------------*/

Ispc_InterfaceStructRegistry::Ispc_InterfaceStructRegistry() {

  m_first = NULL;
  m_last  = NULL;

}

/*----------------------------------------------------------------------------*/

Ispc_InterfaceStructRegistry::~Ispc_InterfaceStructRegistry() {

  Ispc_InterfaceStruct * ifStruct = NULL;

  while (m_first) {
    ifStruct = m_first;
    m_first = ifStruct->getNext();
    delete ifStruct;
  }

}

/*----------------------------------------------------------------------------*/

void Ispc_InterfaceStructRegistry::registerStruct(Ispc_InterfaceStruct * ifStruct) {

  if (NULL == m_first) {
    m_first = ifStruct;
  }
  else {
    m_last->setNext(ifStruct);
  }

  m_last = ifStruct;

}

/*----------------------------------------------------------------------------*/

Ispc_InterfaceStruct * Ispc_InterfaceStructRegistry::findStruct(const char * name) const {

  Ispc_InterfaceStruct * ifStruct = m_first;
  bool found = false;

  while ( (!found) && (ifStruct) ) {
    if ( (0 == strcmp( ifStruct->getName(), name )) || (ifStruct->isSynonym(name)) ) { /* PTS 1134734 */
      found = true;
    }
    else {
      ifStruct = ifStruct->getNext();
    }
  }

  return ifStruct;

}

