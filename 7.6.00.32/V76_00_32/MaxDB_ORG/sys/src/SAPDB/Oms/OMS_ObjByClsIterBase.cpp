/*!
 * \file    OMS_ObjByClsIterBase.cpp
 * \author  MarkusSi, Roterring
 * \brief   implementation of an oid-iterator
 */

/*

    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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

#include "Oms/OMS_ObjByClsIterBase.hpp"
#include "Oms/OMS_ObjectContainer.hpp"
#include "Oms/OMS_Session.hpp"
#include "Oms/OMS_KernelClsIter.hpp"
#include "Oms/OMS_VersionClsIter.hpp"

/*===================================================================================*/

OmsObjByClsIterBase::OmsObjByClsIterBase(const OmsObjByClsIterBase& iter) 
{
  m_pSession        = iter.m_pSession;
  m_pContainerInfo  = iter.m_pContainerInfo;
  m_readFromVersion = iter.m_readFromVersion;
  m_end             = iter.m_end;
  m_isVarObjContainer = iter.m_isVarObjContainer;  // PTS 

  m_pKernelIter = iter.m_pKernelIter;
  if (m_pKernelIter != NULL){
    m_pKernelIter->IncRefCnt();
  }

  m_pVersionIter = iter.m_pVersionIter;
  if (m_pVersionIter != NULL){
    m_pVersionIter->IncRefCnt();
  }
}

/*===================================================================================*/

OmsObjByClsIterBase::OmsObjByClsIterBase( OMS_Session        *pSession,
                                          OMS_ContainerEntry *pContainerInfo,
                                          int                  maxBufferSize ) 
 : m_pKernelIter(NULL)
 , m_pVersionIter(NULL)
 , m_pSession(pSession)
 , m_pContainerInfo(pContainerInfo)
 , m_readFromVersion(false)
 , m_end(false) 
{
  m_isVarObjContainer = m_pContainerInfo->GetClassEntry().IsVarObject();

  // Create kernel iterator
  m_pKernelIter = REINTERPRET_CAST(OMS_KernelClsIter*, pSession->allocate(sizeof(OMS_KernelClsIter)));
  new(m_pKernelIter) OMS_KernelClsIter (m_pSession, m_pContainerInfo, maxBufferSize);

  if (!(*m_pKernelIter)){
    // There are no objects in the kernel iterator...
    if (!m_pSession->InVersion()){
      // ... and as there is no version active, the iterator has reached its end
      m_end = true;
    }
    else {
      // ... and as a version is active, try to find objects in the version iterator
      m_pVersionIter = REINTERPRET_CAST(OMS_VersionClsIter*, pSession->allocate(sizeof(OMS_VersionClsIter)));
      new(m_pVersionIter) OMS_VersionClsIter (m_pSession, m_pContainerInfo);
      m_readFromVersion = true;
      if (!(*m_pVersionIter)){
        // Version is empty, too
        m_end = true;
      }
    }
  }  
}

/*===================================================================================*/

OmsObjByClsIterBase::operator bool () const
{
  return !m_end;
}

/*===================================================================================*/

const OMS_ObjectId8& OmsObjByClsIterBase::operator() () const
{
  const char* msg = "OmsObjByClsIterBase::operator() ";

  // Deref is only possible if iterator is valid
  if (m_end){
    return OMS_Globals::m_globalsInstance->m_nilOid;
  }

  return m_readFromVersion 
          ? m_pVersionIter->GetCurrOid()
          : m_pKernelIter->GetCurrOid();
}

/*===================================================================================*/
  
OmsAbstractObject* OmsObjByClsIterBase::omsDeRef (bool forUpd, bool doLock, bool shared) const
{
  const char* msg = "OmsObjByClsIterBase::omsDeRefObj ";

  if (m_isVarObjContainer){
    //error
  }

  if (m_end){
    return NULL;
  }

  OmsObjectContainer* found = ( m_readFromVersion 
                              ? m_pVersionIter->GetCurrObj()
                              : m_pKernelIter->GetCurrObj());

  // Lock object if requested
  if (doLock && !m_pSession->m_context->IsVersion()) {
    if (!shared && !found->LockedFlag()){
      m_pSession->m_context->LockObj(found->m_oid);
    }
    else if (shared && !found->LockedSharedFlag()){
      m_pSession->m_context->LockSharedObj(found->m_oid);
    }
  }

  return (forUpd 
           ? m_pSession->ForUpdPtr(found) 
           : &found->m_pobj);
}

/*===================================================================================*/

const void* OmsObjByClsIterBase::omsDeRefVarObj (bool doLock, bool shared) const
{
  const char* msg = "OmsObjByClsIterBase::omsDeRefVarObj ";

  if (!m_isVarObjContainer){
    //error
  }

  if (m_end){
    return NULL;
  }

  OmsObjectContainer* found = ( m_readFromVersion 
                              ? m_pVersionIter->GetCurrObj()
                              : m_pKernelIter->GetCurrObj());

  // Lock object if requested
  if (doLock && !m_pSession->m_context->IsVersion()) {
    if (!shared && !found->LockedFlag()){
      m_pSession->m_context->LockObj(found->m_oid);
    }
    else if (shared && !found->LockedSharedFlag()){
      m_pSession->m_context->LockSharedObj(found->m_oid);
    }
  }

  return m_pSession->DeRefVarObject(found->m_oid);
}

/*===================================================================================*/

void OmsObjByClsIterBase::operator++ ()
{
  const char* msg = "OmsObjByClsIterBase::++ ";

#if defined(OMSTST)
  m_pSession->CheckCancelCountDown();
  m_pSession->CheckExceptionCountDown();
#endif

  if (m_end){
    m_pSession->ThrowDBError (e_invalid_iterator, msg, __MY_FILE__, __LINE__);
    return;
  }

  if ((*m_pKernelIter) && !m_readFromVersion){
    // increment kernel iterator
    ++(*m_pKernelIter);
  }
  
  if (!(*m_pKernelIter)){
    // Kernel iterator has reached its end 
    if (!m_pSession->InVersion()){
      // As there is no version active the merge iterator has reached its end, too
      m_end = true;
    }
    else {
      if (!m_readFromVersion){
        // Create iterator for version objects and position it onto the first object
        m_pVersionIter = REINTERPRET_CAST(OMS_VersionClsIter*, m_pSession->allocate(sizeof(OMS_VersionClsIter)));
        new(m_pVersionIter) OMS_VersionClsIter (m_pSession, m_pContainerInfo);
        m_readFromVersion = true;
      }
      else {
        // increment version iterator
        ++(*m_pVersionIter);
      }
      if (!(*m_pVersionIter)){
        // Version iterator has reached its end and as the kernel objects have been
        // read before, the merge iterator has reached its end
        m_end = true;
      }
    }
  }
}

/*===================================================================================*/

OmsObjByClsIterBase::~OmsObjByClsIterBase()
{
  if (m_pKernelIter && m_pKernelIter->DecRefCnt() == 0) {
    m_pKernelIter->DeleteSelf();
    m_pKernelIter = NULL;
  }
  if (m_pVersionIter && m_pVersionIter->DecRefCnt() == 0) {
    m_pVersionIter->DeleteSelf();
    m_pVersionIter = NULL;
  }
}
