/*!*****************************************************************************

  module      : Ispc_InterfaceConstList.cpp

  ----------------------------------------------------------------------------

  responsible : PeterG (GoertzP)

  special area: Generator of interface description code
  description : Linear list for constants and expressions

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


#include "idl/xml2ispc/Ispc_InterfaceConstList.hpp"
#include <string.h>
#include "idl/xml2ispc/IspcErrorHandling.h"


Ispc_InterfaceConst::Ispc_InterfaceConst(const char * name) {

  strcpy( &m_name[0], name );
  m_operator        = NULL;
  m_operandCntr     = 0;
  m_nextOperandType = NULL;
  m_value           = NULL;
  m_next            = NULL;

}

/*----------------------------------------------------------------------------*/

Ispc_InterfaceConst::~Ispc_InterfaceConst() {

  delete m_operator;
  delete m_nextOperandType;
  delete m_value;

}

/*----------------------------------------------------------------------------*/

void Ispc_InterfaceConst::setOperator( int & error, const Ispc_ConstOperator cOperator ) {

  if (NULL != m_operator) {
    Ispc_ErrorMsg( C_DUPLICATE_OPERATOR_SPECIFICATION, m_name );
    if (!error) {
      error = - C_DUPLICATE_OPERATOR_SPECIFICATION;
    }
  }
  else {
    m_operator = new Ispc_ConstOperator;
    *m_operator = cOperator;
  }

}

/*----------------------------------------------------------------------------*/

void Ispc_InterfaceConst::resetOperator() {

  if (NULL != m_operator) {
    delete m_operator;
    m_operator = NULL;
  }

}

/*----------------------------------------------------------------------------*/

void Ispc_InterfaceConst::setNextOperandType( int & error, const Ispc_ConstOperandType coType ) {

  if (NULL != m_nextOperandType) {
    Ispc_ErrorMsg( C_DUPLICATE_OPERAND_SPECIFICATION, m_name );
    if (!error) {
      error = - C_DUPLICATE_OPERAND_SPECIFICATION;
    }

  }
  else {
    m_nextOperandType = new Ispc_ConstOperandType;
    *m_nextOperandType = coType;
  }

}

/*----------------------------------------------------------------------------*/

void Ispc_InterfaceConst::resetNextOperandType() {

  if (NULL != m_nextOperandType) {
    delete m_nextOperandType;
    m_nextOperandType = NULL;
  }

}

/*----------------------------------------------------------------------------*/

const int Ispc_InterfaceConst::getValue() const {

  if (m_value) {
    return *m_value;
  }
  else {
    return 0;
  }

}

/*----------------------------------------------------------------------------*/

void Ispc_InterfaceConst::setValue( int & error, const int value ) {

  if (2 == m_operandCntr) {
    Ispc_ErrorMsg( C_TOO_MANY_OPERANDS, m_name );
    if (!error) {
      error = - C_TOO_MANY_OPERANDS;
    }
    return;
  }

  if (NULL == m_value) {
    m_value = new int; 
    *m_value = value;
  }
  else {
    if (NULL == m_operator) {
      Ispc_ErrorMsg( C_MISSING_OPERATOR, m_name );
      if (!error) {
        error = - C_MISSING_OPERATOR;
      }
      return;
    }
    else {
      switch(*m_operator) {
        case coPlus :
          *m_value = *m_value + value;
          break;
        case coMinus :
          *m_value = *m_value - value;
          break;
      }
    }
  }
  m_operandCntr++;

}

/*----------------------------------------------------------------------------*/

void Ispc_InterfaceConst::setNext(Ispc_InterfaceConst * next) {
  m_next = next;
}

/*----------------------------------------------------------------------------*/

Ispc_InterfaceConstList::Ispc_InterfaceConstList() {

  m_first = NULL;
  m_last  = NULL;

}

/*----------------------------------------------------------------------------*/

Ispc_InterfaceConstList::~Ispc_InterfaceConstList() {

  Ispc_InterfaceConst * ifConst = NULL;

  while (m_first) {
    ifConst = m_first;
    m_first = ifConst->getNext();
    delete ifConst;
  }

}

/*----------------------------------------------------------------------------*/

const Ispc_InterfaceConst * Ispc_InterfaceConstList::findConst(const char * name) const {

  Ispc_InterfaceConst * ifConst = m_first;
  bool found = false;

  while ( (!found) && (ifConst) ) {
    if (0 == strcmp( ifConst->getName(), name )) {
      found = true;
    }
    else {
      ifConst = ifConst->getNext();
    }
  }

  return ifConst;

}
  
/*----------------------------------------------------------------------------*/

void Ispc_InterfaceConstList::registerConst(Ispc_InterfaceConst * ifConst) {

  if (NULL == m_first) {
    m_first = ifConst;
  }
  else {
    m_last->setNext(ifConst);
  }

  m_last = ifConst;

}

