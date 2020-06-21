/*!
 * \file    OMS_ObjectContainer.hpp
 * \author  MarkusSi, PeterG, Roterring
 * \brief   Object frame, which is used to store the object in the cache of the liboms
 */
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
#ifndef __OMS_OBJECTCONTAINER_HPP
#define __OMS_OBJECTCONTAINER_HPP

#include "Oms/OMS_ObjectContainerDef.hpp"
#include "Oms/OMS_ContainerEntry.hpp"
#include "Oms/OMS_ClassEntry.hpp"
#include "Oms/OMS_VarObjInfo.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"

/*----------------------------------------------------------------------*/

inline void OmsObjectContainer::Init()
{
  void** pvtptr     = REINTERPRET_CAST(void**, (&m_pobj));
  *pvtptr           = NULL;

  m_hashnext        = NULL;
#if !defined(BIT64)
  m_filler          = 0;
#endif
  //m_oid
  m_objseq.gg91SetNilRef();
  m_state           = 0;
  m_verstate        = 0;
  SetBeforeImageMask(0);
  SetRevisionNumber(0);
  m_pContainerInfo  = NULL;
}

/*----------------------------------------------------------------------*/

inline void OmsObjectContainer::InitObjContainer(const OMS_ClassEntry &classEntry)
{
  void** pvtptr  = REINTERPRET_CAST(void**, (&m_pobj));
  *pvtptr        = classEntry.GetVTblPtr();

  m_hashnext        = NULL;
#if !defined(BIT64)
  m_filler          = 0;
#endif
  //m_oid
  m_objseq.gg91SetNilRef();
  m_state           = 0;
  m_verstate        = 0;
  SetBeforeImageMask(0);
  SetRevisionNumber(0);
  m_pContainerInfo  = NULL;

  if (classEntry.IsVarObject()) {
    MarkVarObj();
    new(&m_pobj) OMS_VarObjInfo();
  }
}

/*----------------------------------------------------------------------*/

inline OMS_ContainerEntry* OmsObjectContainer::GetContainerInfo(const char *pFile, unsigned int line)
{
  m_pContainerInfo->CheckIfNotDropped(pFile, line);
  return m_pContainerInfo;
}

/*----------------------------------------------------------------------*/

inline OMS_ContainerEntry* OmsObjectContainer::GetContainerInfoNoCheck() 
{
  return m_pContainerInfo;
}

/*----------------------------------------------------------------------*/

inline void OmsObjectContainer::Copy (OMS_Context* pContext, OmsObjectContainer* pBeforeImage, size_t ObjectSize)
{
  tsp00_Int2 refCnt;
  if (this->VarObjFlag()) {
    _OMSASSERT(pContext->GetSession(),
      REINTERPRET_CAST(OMS_VarObjInfo*, &pBeforeImage->m_pobj)->m_vobjInfo.m_vobjNext == 
      REINTERPRET_CAST(OMS_VarObjInfo*, &this->m_pobj)->m_vobjInfo.m_vobjNext);
    refCnt = REINTERPRET_CAST(OMS_VarObjInfo*, &m_pobj)->m_refCnt;
    REINTERPRET_CAST(OMS_VarObjInfo*, &m_pobj)->freeVarObjData(pContext);
  }

  //SAPDB_MemCopyNoCheck(&m_oid, &pBeforeImage->m_oid, ObjectSize - sizeof(m_hashnext));
  SAPDB_MemCopyNoCheck(&m_oid, &pBeforeImage->m_oid, ObjectSize - ((size_t)&m_oid - (size_t)this));

  if (this->VarObjFlag()) {
    REINTERPRET_CAST(OMS_VarObjInfo*, &m_pobj)->m_refCnt = refCnt;
    REINTERPRET_CAST(OMS_VarObjInfo*, &pBeforeImage->m_pobj)->m_pvobj = NULL;
  }
}

/*----------------------------------------------------------------------*/

inline int OmsObjectContainer::Compare(OMS_Context* pContext, OmsObjectContainerPtr pBeforeImage, size_t ObjectSize)
{
  if (this->VarObjFlag()) {
    // TODO: compare VarObjs
    return 0;
  } else {
    return memcmp(reinterpret_cast<const void**>(&m_pobj) + 1,
      reinterpret_cast<const void**>(&pBeforeImage->m_pobj) + 1,
      ObjectSize);
  }
}

/*----------------------------------------------------------------------*/

inline OmsObjectContainer** OmsObjectContainer::GetNextAddr()
{
//#ifdef _ASSERT_OMS
  CheckConsistency();
//#endif

  return &m_hashnext;
}

/*----------------------------------------------------------------------*/

inline void OmsObjectContainer::InitializeForFreeList(int caller)
{
//#ifdef _ASSERT_OMS
  *reinterpret_cast<tsp00_Uint4*>(this) = 0xadadadad;

  void** pvtptr     = REINTERPRET_CAST(void**, (&m_pobj));
  *pvtptr           = NULL;
  SetBeforeImageMask(0);
  SetRevisionNumber(0);
  m_pContainerInfo  = NULL;
  *reinterpret_cast<tsp00_Uint8*>(&m_oid) = 0;
  memset(&m_objseq, 0, sizeof(tgg91_PageRef));

  m_state     = caller;
  m_verstate  = 0;
//#endif
}

/*----------------------------------------------------------------------*/

inline void OmsObjectContainer::InitializeForAllocator(int caller)
{
//#ifdef _ASSERT_OMS
  CheckFreeListState();
  m_verstate  = caller;
  *reinterpret_cast<tsp00_Uint4*>(&m_pContainerInfo) = 0xbdbdbdbd;
//#endif
}

/*----------------------------------------------------------------------*/

inline void OmsObjectContainer::CheckFreeListState()
{
//#ifdef _ASSERT_OMS
  unsigned int pattern = 0xadadadad;
  if (pattern != *reinterpret_cast<tsp00_Uint4*>(this)){
    PrintError("Next-pointer of frame in freelist has been overwritten.", this);
  }

  tsp00_Uint8 nullOid = 0;
  char nullPattern[] = {0,0,0,0,0,0};
  void** pvtptr = reinterpret_cast<void**>(&m_pobj);
  if (*pvtptr != NULL 
    || GetBeforeImageMask() != 0 
    || GetRevisionNumber() != 0 
    || m_oid != *reinterpret_cast<OMS_ObjectId8*>(&nullOid) 
    || memcmp(&m_objseq, &nullPattern, sizeof(tgg91_PageRef))
     )
  {
    // Error
    PrintError("Header of frame in freelist has been overwritten.", this);
  }
//#endif
}

/*----------------------------------------------------------------------*/

inline OmsObjectContainer*  OmsObjectContainer::GetNextFreeList()
{
  return reinterpret_cast<OmsObjectContainer*>(m_pContainerInfo);
}

/*----------------------------------------------------------------------*/

inline void OmsObjectContainer::SetNextFreeList(OmsObjectContainer *p)
{
  *reinterpret_cast<OmsObjectContainer**>(&m_pContainerInfo) = p;
}

/*----------------------------------------------------------------------*/

inline void OmsObjectContainer::SetRevisionNumber(const tgg91_TransNo &updTransId)
{
  // Only consider the last 4 bytes
  y.x.m_revisionNumber = static_cast<tsp00_Uint4>(updTransId.gg91GetUInt8());
}

/*----------------------------------------------------------------------*/

//void OmsObjectContainer::PrintError(const char* msg, const OmsObjectContainer* p) const
//Definition is placed in OMS_Globals.cpp because of dependency problems  

/*----------------------------------------------------------------------*/

#endif  // __OMS_OBJECTCONTAINER_HPP
