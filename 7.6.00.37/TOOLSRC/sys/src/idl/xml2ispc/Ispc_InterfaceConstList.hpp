/*!*****************************************************************************

  module      : Ispc_InterfaceConstList.hpp

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


#ifndef ISPC_IFCONSTLIST_HPP
#define ISPC_IFCONSTLIST_HPP


typedef enum { coPlus, coMinus } Ispc_ConstOperator;

typedef enum { cotFixed, cotIdent } Ispc_ConstOperandType;


class Ispc_InterfaceConst
{
public:
  Ispc_InterfaceConst(const char * name);
  ~Ispc_InterfaceConst();
  const char *            getName() const {return m_name;};
  void                    setOperator( int & error, const Ispc_ConstOperator cOperator );
  void                    resetOperator();
  Ispc_ConstOperandType * getNextOperandType() const {return m_nextOperandType;};
  void                    setNextOperandType( int & error, const Ispc_ConstOperandType coType );
  void                    resetNextOperandType();
  const int *             getValuePtr() const {return m_value;};
  const int               getValue() const;
  void                    setValue( int & error, const int value );
  Ispc_InterfaceConst *   getNext() const {return m_next;};
  void                    setNext(Ispc_InterfaceConst * next);
private:
  char                    m_name[64];
  Ispc_ConstOperator *    m_operator;
  int                     m_operandCntr;
  Ispc_ConstOperandType * m_nextOperandType;
  int *                   m_value;
  Ispc_InterfaceConst *   m_next;
};


class Ispc_InterfaceConstList
{
public:
  Ispc_InterfaceConstList();
  ~Ispc_InterfaceConstList();
  const Ispc_InterfaceConst * findConst(const char * name) const;
  void                        registerConst(Ispc_InterfaceConst * ifConst);
private:
  Ispc_InterfaceConst * m_first;
  Ispc_InterfaceConst * m_last;
};


#endif /* ISPC_IFCONSTLIST_HPP */

