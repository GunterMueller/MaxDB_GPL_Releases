/*!
 * \file    OMS_VarObj.hpp
 * \author  IvanS, MarkusSi, PeterG
 * \brief   Classes to handle variable sized persistent objects
 */
/*

    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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


#ifndef __OMS_VAROBJ_HPP
#define __OMS_VAROBJ_HPP

#include "Oms/OMS_Oid.hpp"
//#include "Oms/OMS_VarOid.hpp"
#include "ggg00.h"
#include "Oms/OMS_ContainerHandle.hpp"      // FF 2003-09-23 new FileDir
#include "FileDirectory/FileDir_FileNo.hpp"

enum OMS_VarObjLockType {
  VarObjNoLock,
  VarObjShareLock,
  VarObjExclusiveLock
};

#define VAR_OBJ_CHUNK_SIZE (MAX_RECLEN_GG00 - cgg_rec_key_offset - sizeof(OMS_ObjectId8) - sizeof(FileDir_FileNo) - 4 - sizeof (OmsObjectId) - sizeof (size_t))

class OMS_VarObjContainer 
{
public:
  unsigned char vobjInfo[VAR_OBJ_CHUNK_SIZE];
  size_t        m_vobjSize;
  OMS_ObjectId8 m_vobjNext;
};

class OMS_VarObjChunk 
{
public:
  tgg91_PageRef            m_seq;
  tgg91_TransNo            m_updTransId;
  OMS_ObjectId8            m_oid;
  OMS_ContainerHandle      m_containerHandle; // FF 2003-09-23 new FileDir
  OMS_VarObjContainer      m_container;
  void Clear() {
    m_oid.setNil();
  }
};

#endif // __OMS_VAROBJ_HPP
