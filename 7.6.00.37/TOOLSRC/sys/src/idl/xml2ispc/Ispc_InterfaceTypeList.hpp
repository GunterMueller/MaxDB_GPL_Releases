/*!*****************************************************************************

  module      : Ispc_InterfaceTypeList.hpp

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


#ifndef ISPC_IFTYPELIST_HPP
#define ISPC_IFTYPELIST_HPP


class Ispc_InterfaceType
{
public:
  const char *         getType() const {return m_type;};
  void                 setType(const char * type);
  const char *         getDefault() const {return m_default;};
  void                 setDefault(const char * defValue);
  Ispc_InterfaceType * getNext() const {return m_next;};
  void                 setNext(Ispc_InterfaceType * next);
private:
  char                 m_type[64];
  char                 m_default[10];
  Ispc_InterfaceType * m_next;
};


class Ispc_InterfaceTypeList
{
public:
  Ispc_InterfaceTypeList();
  ~Ispc_InterfaceTypeList();
  bool                       isRegistered(const char * name) const;
  const Ispc_InterfaceType * findType(const char * type) const;
  void                       registerType( const char * name, const char * defValue );
  bool                       skipTypeReg(const char * name) const;
private:
  Ispc_InterfaceType * m_first;
  Ispc_InterfaceType * m_last;
};


#endif /* ISPC_IFTYPELIST_HPP */

