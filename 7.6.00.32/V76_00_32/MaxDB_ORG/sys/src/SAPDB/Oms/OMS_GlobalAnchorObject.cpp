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
#include "Oms/OMS_GlobalAnchorObject.hpp"
#include "Oms/OMS_GlobalAnchorItem.hpp"
#include "Oms/OMS_GlobalUserAllocator.hpp"
#include "Oms/OMS_Handle.hpp"
#include "Oms/OMS_Session.hpp"
#include "Oms/OMS_RWLock.hpp"
#include "liveCache/LVC_IliveCacheSink.hpp"


/*-----------------------------------------------------*/


OmsGlobalAnchorScopeBase::OmsGlobalAnchorScopeBase(OmsHandle &h, OmsTypeInt4 areaId, OmsTypeInt4 anchorId, size_t anchorClassSize, OmsTypeInt4 anchorClassId)  
: m_areaId(areaId), m_anchorId(anchorId)
{
  OMS_TRACE(omsTrInterface, OMS_Globals::GetCurrentLcSink(), "OmsGlobalAnchorScopeBase::OmsGlobalAnchorScopeBase : areaId = " 
                 << m_areaId << ", m_anchorId = " << anchorId);
  // Check if areaId and anchorId are valid
  if( areaId <= 0 || anchorId <= 0)
    OMS_Globals::Throw(e_OMS_invalid_parameter, 
      "OmsGlobalAnchorScopeBase::OmsGlobalAnchorScopeBase : invalid id", __MY_FILE__, __LINE__);
  // Get the anchor item from the global anchor register
  m_pAnchorItem = OMS_Globals::m_globalsInstance->m_globalAnchorDirectory.get(&h, areaId, anchorId);
  // Get corresponding R/W lock from session lock directory and lock shared
  OMS_TRACE(omsTrInterface, OMS_Globals::GetCurrentLcSink(), "OmsGlobalAnchorScopeBase::OmsGlobalAnchorScopeBase : lockId = " 
                 << m_pAnchorItem->m_lockId);
  m_pLock = &h.m_pSession->GetRWLockDir().getLock(OmsGlobalAnchorScopeBase::ANCHOR_LOCK_AREA,m_pAnchorItem->m_lockId);
  short err = m_pLock->enter(false/*share*/);
  if(err != e_ok){ //Could not lock R/W lock, throw DbpError 
    char buffer[256];
    sp77sprintf(buffer,256, "OmsGlobalAnchorScopeBase::OmsGlobalAnchorScopeBase : cannot share lock RWLock ; areaId = %d , lockId = %d", 
                                                OmsGlobalAnchorScopeBase::ANCHOR_LOCK_AREA, m_pAnchorItem->m_lockId );
    OMS_Globals::Throw(err, buffer, __MY_FILE__, __LINE__);
  }
  // If anchor pointer allready exists, check if anchor id and size are the same 
  if(m_pAnchorItem->m_pAnchor != NULL && m_pAnchorItem->m_anchorClassSize != anchorClassSize 
                              && m_pAnchorItem->m_anchorClassId != anchorClassId){
    m_pLock->leave();
    char buffer[256];
    sp77sprintf(buffer,256, 
      "OmsGlobalAnchorScopeBase::OmsGlobalAnchorScopeBase : try to create template with wrong parameter ; old class id  = %d , new class id = %d , old class size = %d , new class size = %d", 
                            m_pAnchorItem->m_anchorClassId, anchorClassId, 
                            m_pAnchorItem->m_anchorClassSize, anchorClassSize);
    OMS_Globals::Throw(e_OMS_invalid_parameter, buffer, __MY_FILE__, __LINE__);
  }
}

/*-----------------------------------------------------*/


OmsGlobalAnchorScopeBase::~OmsGlobalAnchorScopeBase() {
  OMS_TRACE(omsTrInterface, OMS_Globals::GetCurrentLcSink(), "OmsGlobalAnchorScopeBase::~OmsGlobalAnchorScopeBase() : areaId = " 
      << m_areaId << ", anchorId = " << m_anchorId << ", lockId = " << m_pAnchorItem->m_lockId);
  // Unlock RWLock
  m_pLock->leave();
}

/*-----------------------------------------------------*/

OmsAllocatorInterface& OmsGlobalAnchorScopeBase::getAllocator(){
  return *m_pAnchorItem->m_pAlloc;
}

/*-----------------------------------------------------*/

void  *OmsGlobalAnchorScopeBase::getAnchorPtr(){
  return m_pAnchorItem->m_pAnchor;
}

/*-----------------------------------------------------*/

void OmsGlobalAnchorScopeBase::setAnchorPtr(void *p, size_t anchorClassSize, OmsTypeInt4 anchorClassId){
  m_pAnchorItem->m_pAnchor = p;
  m_pAnchorItem->m_anchorClassSize = anchorClassSize;
  m_pAnchorItem->m_anchorClassId = anchorClassId;
}

/*-----------------------------------------------------*/

bool OmsGlobalAnchorScopeBase::isValid(){
  // Check if the global valid flag of this global shared memory area is set
  return m_pAnchorItem->m_isValid;
}

/*-----------------------------------------------------*/

void OmsGlobalAnchorScopeBase::lock(bool exclusive){
  // First unlock the existing lock, then lock share or exclusive.
  m_pLock->leave();
  short err = m_pLock->enter(exclusive);
  if(err != e_ok) {// Could not lock R/W lock, throw DbpError
    char buffer[256];
    sp77sprintf(buffer,256, "OmsGlobalAnchorScopeBase::lock : cannot lock RWLock ; areaId = %d , lockId = %d, exclusive = %s", 
                                               OmsGlobalAnchorScopeBase::ANCHOR_LOCK_AREA, m_pAnchorItem->m_lockId, 
                                               exclusive == true ? "true" : "false"  );
    OMS_Globals::Throw(err, buffer, __MY_FILE__, __LINE__);
  }
}

/*-----------------------------------------------------*/

void OmsGlobalAnchorScopeBase::setAnchorName(const OmsTypeWyde *anchorName){
  /*TODO*/
}

/*-----------------------------------------------------*/

void *OmsGlobalAnchorScopeBase::operator new(size_t size){
  // Define of operator new method to solve unsatisfied symbol errors
  // during template instantiation
  return NULL;
}

  
