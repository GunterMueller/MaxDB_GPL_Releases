/*


    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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
#ifndef __OMS_CONTAINERHANDLE_HPP
#define __OMS_CONTAINERHANDLE_HPP

#include "gsp00.h"
#include "SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp"

/*--------------------------------------------------------------------------*/
/// Definition of OMS_ContainerHandle
/*!
** OMS_ContainerHandle is the unique ID, which identifies the container within 
** the file directory.
**
** \ingroup oms
*/
class OMS_ContainerHandle {
  tsp00_Uint8 m_data;

public :
  OMS_ContainerHandle() 
  : m_data(0)
  {}

  OMS_ContainerHandle(const tsp00_C8 &data) 
  {
    SAPDB_MemCopyNoCheck(&m_data, &data, sizeof(tsp00_Uint8));
  }

  bool operator!= (const OMS_ContainerHandle &op2) const 
  { 
    return (m_data != op2.m_data); 
  }

  bool operator== (const OMS_ContainerHandle &op2) const 
  { 
    return (m_data == op2.m_data); 
  }

  int operator% ( const int op2 ) const
  {
    return static_cast<int>(m_data % op2);
  }
  
  //const tsp00_C8& operator() () const 
  //{
  //  return *reinterpret_cast<const tsp00_C8*>(&m_data); 
  //}

  const tsp00_Uint8& GetUint8() const 
  {
    return m_data;
  }


  //******** ONLY USED IN THE SIMULATOR ***********
  tsp00_Uint4 GetOldContainerHandle() const 
  {
    return static_cast< tsp00_Uint4 >( m_data ); 
  }

  void SetOldContainerHandle(const tsp00_Uint4 cont) 
  {
    m_data = cont; 
  }
};

#endif  // __OMS_CONTAINERHANDLE_HPP
