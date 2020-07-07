/*!
 * \file    OMS_VarObjInfo.hpp
 * \author  MarkusSi, PeterG, Roterring
 * \brief   Transient description of variable sized objects.
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


#ifndef __OMS_VAROBJINFO_HPP
#define __OMS_VAROBJINFO_HPP

#include "Oms/OMS_VarObj.hpp"
#include "Oms/OMS_Trace.hpp"
#include "Oms/OMS_Globals.hpp"
#include "geo00.h"

class OMS_Context;

class OMS_VarObjInfoNode {
public :
  OMS_VarObjInfoNode() 
    : m_vobjOid()
    , m_vobjSeq()
    , m_refCnt(1)
    , m_newFlag(false)
    , m_nodeCnt(1)
    , m_vobjNext(NULL)
  {
  }

  OMS_VarObjInfoNode(const OMS_ObjectId8& oid, const tgg91_PageRef& objSeq) 
    : m_vobjOid(oid)
    , m_vobjSeq(objSeq)
    , m_refCnt(1)
    , m_newFlag(false)
    , m_nodeCnt(1)
    , m_vobjNext(NULL)
  {
  }

  static void* operator new(size_t size, OMS_Context& c); 
#if defined(OMS_PLACEMENT_DELETE)
  void operator delete(void* p, OMS_Context& c); 
#endif
  void deleteSelf (OMS_Context& c);


  /*! Oid which is/can be used for storing a portion of the variable sized
  **  object in the kernel.
  */
  OMS_ObjectId8           m_vobjOid;

  /*! Object sequence number (log-oid) corresponding the the oid stored in
  **  m_vobjOid.
  */
  tgg91_PageRef           m_vobjSeq;

  /*! Counter which contains the number of references pointing to this infonode.
  **  Everytime a new before-image of the corresponding variable sized object
  **  is created (OMS_BeforeImageList::insertBeforeImage), this counter is increased 
  **  and if an reference is freed OMS_VarObjInfo::freeVarObj then the counter is 
  **  decremented. Finally if it equals zero the infonode is removed from the memory.
  */
  tsp00_Int2              m_refCnt;  

  /*! Number of InfoNodes in the chain of the current variable sized object.
  ** This counter is only filled for the first infonode in the chain, for all other
  ** infonodes the value of this field is undefined. If an object contains more
  ** infonodes as can be count in an unsigned short (objectsize > 0.5 GB), then 
  ** this variable equals 0.
  */
  unsigned short          m_nodeCnt;    // PTS 1121449

  /*! If the oid of this infonode is newly requested from the kernel, then
  **  this flag is set to 'true'. This information is necessary, as it is not
  **  allowed to aquire a lock in the kernel on a newly created object and 
  **  there is no separate information (like in the header of OmsObjectContainer
  **  for 'normal' objects) for infonodes to check this..
  */
  bool                    m_newFlag;    // PTS 1121449

  /*! Pointer to the next entry in the chain */
  OMS_VarObjInfoNode*     m_vobjNext;
};





class OMS_VarObjInfo : public OmsAbstractObject {  
public :
  OMS_VarObjInfo() 
    : m_vobjSize(0)
    , m_refCnt(0)
    , m_pvobj(NULL)
    , m_vobjInfo() 
  {}

  OMS_VarObjInfo(const OMS_ObjectId8& oid, size_t size, const tgg91_PageRef& objSeq) 
    : m_vobjSize(size)
    , m_refCnt(0)
    , m_pvobj(NULL)
    , m_vobjInfo(oid, objSeq) 
  {}

  void freeVarObj (OMS_Context* context);
  void freeVarObjData(OMS_Context* context);

  void incRefCnt()  
  {
    OMS_VarObjInfoNode* curr = m_vobjInfo.m_vobjNext;
    while (NULL != curr)
    {
      ++curr->m_refCnt;
      curr = curr->m_vobjNext;
    }
  }

  void lock(OMS_VarObjLockType lockReq) {
    switch (lockReq) {
    case VarObjNoLock :
      //OMS_TRACE(omsTrLock, OMS_Globals::m_globalsInstance->GetCurrentLcSink(), "  Get VarObjNoLock " << m_vobjInfo.m_vobjOid);
      return;
    case VarObjShareLock :
      if (m_refCnt >= 0) {
        ++m_refCnt;
        //OMS_TRACE(omsTrLock, OMS_Globals::m_globalsInstance->GetCurrentLcSink(), "  Get VarObjShareLock " << m_vobjInfo.m_vobjOid << " Cnt:" << m_refCnt);
        return;
      }
      break;
    case VarObjExclusiveLock :
      if (0 == m_refCnt) {
        m_refCnt = -1;
        //OMS_TRACE(omsTrLock, OMS_Globals::m_globalsInstance->GetCurrentLcSink(), "  Get VarObjExclusiveLock " << m_vobjInfo.m_vobjOid);
        return;
      }
    }
    //OMS_TRACE(omsTrLock, OMS_Globals::m_globalsInstance->GetCurrentLcSink(), "  Lock-Collision " << m_vobjInfo.m_vobjOid << " Cnt:" << m_refCnt 
    //  << (lockReq == VarObjShareLock ? " Shared " : " Exclusive ") << "requested");
    OMS_Globals::Throw(e_duplicate_object, "OMS_VarObjInfo::lock", __MY_FILE__, __LINE__);
  }

  void unlock() {
    if (m_refCnt > 0) {
      /* share lock */
      --m_refCnt;
      //OMS_TRACE(omsTrLock, OMS_Globals::m_globalsInstance->GetCurrentLcSink(), "  Free VarObjSharedLock " << m_vobjInfo.m_vobjOid << " Cnt:" << m_refCnt);
    }
    else if (m_refCnt < 0) {
      /* exclusive lock */
      m_refCnt = 0;
      //OMS_TRACE(omsTrLock, OMS_Globals::m_globalsInstance->GetCurrentLcSink(), "  Free VarObjExclusiveLock " << m_vobjInfo.m_vobjOid);
    }
    else {
     //OMS_TRACE(omsTrLock, OMS_Globals::m_globalsInstance->GetCurrentLcSink(), "  Free unlocked Object " << m_vobjInfo.m_vobjOid);
    }
  }

  void* operator new(size_t sz, OmsAbstractObject* pObj) {
    return pObj;
  }
#if defined(OMS_PLACEMENT_DELETE)
  void operator delete(void* p, OmsAbstractObject* pObj) {}
#endif

  /*! Current size of the object in the oms-cache. This size is the
  **  sum of all portions.
  */
  size_t                 m_vobjSize;

  /*! Counter for the references to the variable sized object. This 
  **  should prevent, that within one context the application creates
  **  several copies of the object and later the former copies (together
  **  with their updates) are lost.
  **  If the value is larger than 0, then there are as many shared
  **  locks and if the value equals -1, then there exists an exclusive
  **  lock on the object.
  */
  tsp00_Int2             m_refCnt;

public :
  /*! Pointer to the variable sized object in the oms-cache. */
  unsigned char*         m_pvobj;

  /*! Head of the chain which contains the desciption for the oids which are 
  **  used to store the various portions of the variable sized object in the
  **  kernel.
  */
  OMS_VarObjInfoNode     m_vobjInfo;
};

#endif  // __OMS_VAROBJINFO_HPP
