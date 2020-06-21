/*!
 * \file    OMS_ObjByKeyIterBase.cpp
 * \author  MarkusSi, Roterring
 * \brief   implementation of key range iterator
 */

/*
    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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



#include "Oms/OMS_ObjByKeyIterBase.hpp"
#include "Oms/OMS_ObjectContainer.hpp"
#include "Oms/OMS_Handle.hpp"
#include "Oms/OMS_Session.hpp"
#include "Oms/OMS_KernelVersionMergeKeyIter.hpp"
#include <new>


/*----------------------------------------------------------------------*/

OmsObjByKeyIterBase::OmsObjByKeyIterBase
(
 OmsHandle&        h, 
 ClassID  guid, 
 OmsSchemaHandle   Schema, 
 OmsContainerNo    ContainerNo,
 const void*       pStartKey,
 const void*       pUpperKey,
 int               maxBufferSize
)
 : m_pIterator(NULL)
{
  OMS_ContainerEntry* pContainerInfo = h.m_pSession->m_context->GetContainerDir().GetContainerEntry(guid, Schema, ContainerNo);
  
  m_pIterator = new(h.m_pSession->allocate(sizeof(OMS_KernelVersionMergeKeyIter)))
                    OMS_KernelVersionMergeKeyIter(pStartKey, pStartKey, pUpperKey, OMS_ASCENDING,
                                     h.m_pSession, pContainerInfo, maxBufferSize);
}

/*----------------------------------------------------------------------*/

OmsObjByKeyIterBase::OmsObjByKeyIterBase
(
 OmsHandle&        h, 
 ClassID  guid, 
 OmsSchemaHandle   Schema, 
 OmsContainerNo    ContainerNo,
 const void*       pStartKey,
 const void*       pLowerKey,
 const void*       pUpperKey,
 OmsIterDirection  direction,
 int               maxBufferSize
)
 : m_pIterator(NULL)
{
  OMS_ContainerEntry* pContainerInfo = h.m_pSession->m_context->GetContainerDir().GetContainerEntry(guid, Schema, ContainerNo);

  m_pIterator = new(h.m_pSession->allocate(sizeof(OMS_KernelVersionMergeKeyIter)))
                    OMS_KernelVersionMergeKeyIter(pStartKey, pLowerKey, pUpperKey, direction,
                                     h.m_pSession, pContainerInfo, maxBufferSize);
}


/*----------------------------------------------------------------------*/

OmsObjByKeyIterBase::OmsObjByKeyIterBase(const OmsObjByKeyIterBase& source)
{
  this->m_pIterator = source.m_pIterator;
  m_pIterator->IncRefCnt();
}

/*----------------------------------------------------------------------*/

OmsObjByKeyIterBase::operator bool () const
{
  return bool(*m_pIterator); 
}

/*----------------------------------------------------------------------*/

void OmsObjByKeyIterBase::operator++()
{
  ++(*m_pIterator);
}

/*----------------------------------------------------------------------*/

void OmsObjByKeyIterBase::operator--()
{
  --(*m_pIterator);
}

/*----------------------------------------------------------------------*/

OmsAbstractObject* OmsObjByKeyIterBase::omsDeRef (bool forUpd, bool doLock, bool shared) const
{
  const char* msg = "OmsObjByKeyIterBase::omsDeRef ";
  
  if (*this) 
  {
    OMS_Session&  Session       = *m_pIterator->GetSession();
    OMS_Context&  Context       = *Session.m_context;
    OMS_ContainerEntry& ContainerInfo = *m_pIterator->GetContainerInfo();
    Session.IncDeref();

    OmsObjectContainerPtr found = m_pIterator->GetCurrObj();

    if (found->DeletedFlag()) {
      Session.ThrowDBError (e_object_not_found, msg, found->m_oid, __MY_FILE__, __LINE__);
      return NULL;
    }
    else {
      if (doLock && !Context.IsVersion()){
        if (!shared  && !found->LockedFlag()) {
          Context.LockObj(found->m_oid);
        }
        else  if (shared  && !found->LockedSharedFlag()) {
          Context.LockSharedObj(found->m_oid);
        }
      }
      OMS_TRACE(omsTrKeyRange, Session.m_lcSink, msg << 
                OMS_UnsignedCharBuffer(ContainerInfo.GetClassEntry().GetKeyPtr(found), ContainerInfo.GetClassEntry().GetKeyDesc().GetLen()));
        
      if (forUpd) {
        return Session.ForUpdPtr(found);
      }
      else {
        return &found->m_pobj;
      }
    }
  }
  return NULL;
}

/*----------------------------------------------------------------------*/

OmsObjByKeyIterBase::~OmsObjByKeyIterBase()
{
  if (NULL != m_pIterator && 0 == m_pIterator->DecRefCnt()){
    m_pIterator->DeleteSelf();
    m_pIterator = NULL;
  } 
}