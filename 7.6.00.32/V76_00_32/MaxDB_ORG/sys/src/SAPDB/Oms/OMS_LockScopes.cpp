/*!
 * \file    OMS_LockScopes.cpp
 * \author  ChristophR
 * \brief   shared/exclusive shared scopes.
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

#include "Oms/OMS_LockScopes.hpp"
#include "Oms/OMS_Handle.hpp"
#include "Oms/OMS_RWLock.hpp"
#include "Oms/OMS_DbpError.hpp"
#include "Oms/OMS_Defines.h"
#include "Oms/OMS_Session.hpp"
#include "Oms/OMS_Globals.hpp"


/*-----------------------------------------------------*/

/// Lock the specified lock in constructor.
OmsLockScope::OmsLockScope(OmsHandle &h, OmsTypeInt4 areaId, OmsTypeInt4 lockId, bool exclusive)
{
  OMS_TRACE(omsTrInterface, OMS_Globals::GetCurrentLcSink(), "OmsLockScope::OmsLockScope : areaId = " 
              << areaId << ", lockId = " << lockId << ", exclusive = " << exclusive); 
  // check if the areaId and lockId is valid for lcapps
  if( areaId < 1){
    char buffer[256];
    sp77sprintf(buffer,256, "OmsLockScope::OmsLockScope : areaId invalid ; areaId = %d , lockId = %d , exclusive = %s", 
                                                areaId, lockId, exclusive == true ? "true" : "false" );
    OMS_Globals::Throw(e_OMS_invalid_parameter, buffer, __MY_FILE__, __LINE__);
  }
  if( lockId < 1){
    char buffer[256];
    sp77sprintf(buffer,256, "OmsLockScope::OmsLockScope : lockId invalid ; areaId = %d , lockId = %d , exclusive = %s", 
                                                areaId, lockId, exclusive == true ? "true" : "false");
    OMS_Globals::Throw(e_OMS_invalid_parameter, buffer, __MY_FILE__, __LINE__);
  }
  // get the specified lock from session R/W lock directory
  m_lock = &h.m_pSession->GetRWLockDir().getLock(areaId,lockId);
  short err = m_lock->enter(exclusive);
  if( err != e_ok){
    char buffer[256];
    sp77sprintf(buffer,256,"OmsLockScope::OmsLockScope : cannot lock RWLock ; areaId = %d , lockId = %d , exclusive = %s", 
                                                areaId, lockId, exclusive == true ? "true" : "false");
    OMS_Globals::Throw(err, buffer, __MY_FILE__, __LINE__);
  }
}

/*-----------------------------------------------------*/

/// Unlock the lock in destructor.
OmsLockScope::~OmsLockScope()
{
  OMS_TRACE(omsTrInterface, OMS_Globals::GetCurrentLcSink(), "OmsLockScope::~OmsLockScope : areaId = " 
              << m_lock->getAreaId() << ", lockId = " << m_lock->getLockId()); 
  short err = m_lock->leave();
  if( err != e_ok){
    char buffer[256];
    sp77sprintf(buffer,256,"OmsLockScope::~OmsLockScope : cannot unlock RWLock ; areaId = %d , lockId = %d", 
                                                m_lock->getAreaId(), m_lock->getLockId());
    OMS_Globals::Throw(err, buffer, __MY_FILE__, __LINE__);
  }

}

/*-----------------------------------------------------*/

/// Lock the specified lock shared in constructor.
OmsReleasableScope::OmsReleasableScope(OmsHandle &h, OmsTypeInt4 areaId, OmsTypeInt4 lockId, bool exclusive)
{
  OMS_TRACE(omsTrInterface, OMS_Globals::GetCurrentLcSink(), "OmsReleasableScope::OmsReleasableScope : areaId = " << areaId 
              << ", lockId = " << lockId << ", exclusive = " << exclusive); 
  // check if the areaId and lockId is valid for lcapps
  if( areaId < 1){
    char buffer[256];
    sp77sprintf(buffer,256, "OmsReleasableScope::OmsReleasableScope : areaId invalid ; areaId = %d , lockId = %d , exclusive = %s", 
                                                areaId, lockId, exclusive == true ? "true" : "false" );
    OMS_Globals::Throw(e_OMS_invalid_parameter, buffer, __MY_FILE__, __LINE__);
  }
  if( lockId < 1){
    char buffer[256];
    sp77sprintf(buffer,256, "OmsReleasableScope::OmsReleasableScope : lockId invalid ; areaId = %d , lockId = %d , exclusive = %s", 
                                                areaId, lockId, exclusive == true ? "true" : "false");
    OMS_Globals::Throw(e_OMS_invalid_parameter, buffer, __MY_FILE__, __LINE__);
  }
  // get the specified lock from session R/W lock directory
  m_lock = &h.m_pSession->GetRWLockDir().getLock(areaId,lockId);
  short err = m_lock->enter(exclusive);
  if( err != e_ok){
    char buffer[256];
    sp77sprintf(buffer,256,"OmsReleasableScope::OmsReleasableScope : cannot lock RWLock ; areaId = %d , lockId = %d , exclusive = %s", 
                                                areaId, lockId, exclusive == true ? "true" : "false");
    OMS_Globals::Throw(err, buffer, __MY_FILE__, __LINE__);
  }
}

/*-----------------------------------------------------*/

/// Unlock the lock in destructor.
OmsReleasableScope::~OmsReleasableScope()
{
  OMS_TRACE(omsTrInterface, OMS_Globals::GetCurrentLcSink(), "OmsReleasableScope::~OmsReleasableScope : areaId = " 
              << m_lock->getAreaId() << ", lockId = " << m_lock->getLockId()); 
  if(m_lock->getLocked()){
    short err = m_lock->leave();
    if( err != e_ok) {
      char buffer[256];
      sp77sprintf(buffer,256,"OmsReleasableScope::~OmsReleasableScope : cannot unlock RWLock ; areaId = %d , lockId = %d , exclusive = %s", 
                                                  m_lock->getAreaId(), m_lock->getLockId(), m_lock->getExclusive() ? "true" : "false");
      OMS_Globals::Throw(err, buffer, __MY_FILE__, __LINE__);
    }
  }
}

/*-----------------------------------------------------*/

/// Reenter the scope.
void OmsReleasableScope::reenter(bool exclusive)
{
  OMS_TRACE(omsTrInterface, OMS_Globals::GetCurrentLcSink(), "OmsReleasableScope::reenter : areaId = " 
            << m_lock->getAreaId() << ", lockId = " << m_lock->getLockId() << ", exclusive = " << exclusive); 
  short err = m_lock->enter(exclusive);
  if( err != e_ok){
    char buffer[256];
    sp77sprintf(buffer,256,"OmsReleasableScope::reenter : cannot lock RWLock ; areaId = %d , lockId = %d , exclusive = %s", 
                                                m_lock->getAreaId(), m_lock->getLockId(), exclusive == true ? "true" : "false");
    OMS_Globals::Throw(err, buffer, __MY_FILE__, __LINE__);
  }
}

/*-----------------------------------------------------*/

/// Release the scope.
void OmsReleasableScope::leave()
{
  OMS_TRACE(omsTrInterface, OMS_Globals::GetCurrentLcSink(), "OmsReleasableScope::leave : areaId = " 
              << m_lock->getAreaId() << ", lockId = " << m_lock->getLockId()); 
  short err = m_lock->leave();
  if( err != e_ok){
    char buffer[256];
    sp77sprintf(buffer,256,"OmsReleasableScope::leave : cannot unlock RWLock ; areaId = %d , lockId = %d , exclusive = %s", 
                                                m_lock->getAreaId(), m_lock->getLockId(), m_lock->getExclusive() ? "true" : "false");
    OMS_Globals::Throw(err, buffer, __MY_FILE__, __LINE__);
  }
}

/*-----------------------------------------------------*/

/// Lock the specified locks in constructor.
OmsMultiScope::OmsMultiScope(OmsHandle &h, OmsTypeInt4 areaId, const OmsTypeInt4 *lockId, OmsTypeInt4 lockCnt, bool exclusive)
: m_handle(&h), m_lockCnt(lockCnt), m_areaId(areaId)
{
  OMS_TRACE(omsTrInterface, OMS_Globals::GetCurrentLcSink(), "OmsMultiScope::OmsMultiScope : areaId = " 
              << m_areaId << ", lockCnt = " << m_lockCnt << ", exclusive = " << exclusive);
  for (int j=0; j<m_lockCnt; ++j){
    OMS_TRACE(omsTrInterface, OMS_Globals::GetCurrentLcSink(), "  " << lockId[j]);
  }

  // check if the lockCnt is valid
  if( m_lockCnt < 0){
    char buffer[256];
    sp77sprintf(buffer,256, "OmsMultiScope::OmsMultiScope : lockCnt invalid ; areaId = %d , lockCnt = %d , exclusive = %s", 
                                                    areaId, lockCnt, exclusive == true ? "true" : "false");
    OMS_Globals::Throw(e_OMS_invalid_parameter, buffer, __MY_FILE__, __LINE__);
  }
  // check if areaId is valid for lcapps
  if( m_areaId < 1){
    char buffer[256];
    sp77sprintf(buffer,256, "OmsMultiScope::OmsMultiScope : areaId invalid ; areaId = %d , lockCnt = %d , exlcusive = %s", 
                                                    areaId, lockCnt, exclusive == true ? "true" : "false");
    OMS_Globals::Throw(e_OMS_invalid_parameter, buffer, __MY_FILE__, __LINE__);
  }
  if( m_lockCnt == 0){
    // nothing todo
    return;
  }

  // check if parameter lockIds are valid
  int i;
  for(i=0; i<m_lockCnt; ++i){
    // check if the lockId is valid
    if( lockId[i] < 1 ){
      char buffer[256];
      sp77sprintf(buffer,256,"OmsMultiScope::OmsMultiScope : lockId invalid ; areaId = %d , lockId[%d] = %d", areaId, i, lockId[i]);
      OMS_Globals::Throw(e_OMS_invalid_parameter, buffer, __MY_FILE__, __LINE__);
    }
    // The ids must be sorted ascending, otherwise deadlocks might occur
    if(i>0 && lockId[i] <= lockId[i-1]){
      char buffer[256];
      sp77sprintf(buffer,256,"OmsMultiScope::OmsMultiScope : Ids must be sorted ascending ; areaId = %d , lockId[%d] = %d , lockId[%d] = %d , exclusive = %s", 
                                                  areaId, i-1, lockId[i-1], i, lockId[i], exclusive == true ? "true" : "false");
      OMS_Globals::Throw(e_OMS_invalid_parameter, buffer, __MY_FILE__, __LINE__);
    }
  }

  m_locks = reinterpret_cast<OMS_RWLock**>( m_handle->m_pSession->allocate(sizeof(OMS_RWLock*) * lockCnt));
  for(i=0; i<m_lockCnt; ++i){
    // get the specified lock from session R/W lock directory
    m_locks[i] = &m_handle->m_pSession->GetRWLockDir().getLock(areaId,lockId[i]);
    short err = m_locks[i]->enter(exclusive);
    if (err != 0) { // Cannot lock RWLock, cleanup
      // write error message into knldiag.err
      DbpBase opMsg(m_handle->m_pSession->m_lcSink);
      opMsg.dbpOpError("OmsMultiScope::OmsMultiScope : Error %d ; areaId = %d , lockId[%d] = %d, exclusive = %s", 
                                                  err, areaId, i, lockId[i], exclusive == true ? "true" : "false");
      unlock(i); // unlock all allready locked locks
      char buffer[256];
      sp77sprintf(buffer,256,"OmsMultiScope::OmsMultiScope : cannot lock RWLock ; areaId = %d , lockId[%d] = %d, exclusive = %s", 
                                                  areaId, i, lockId[i], exclusive == true ? "true" : "false");
      OMS_Globals::Throw(err, buffer, __MY_FILE__, __LINE__);
    }
  }
}

/*-----------------------------------------------------*/

/// Unlock the locks in destructor.
OmsMultiScope::~OmsMultiScope()
{
  OMS_TRACE(omsTrInterface, OMS_Globals::GetCurrentLcSink(), "OmsMultiScope::~OmsMultiScope : areaId = " 
                << m_areaId << ", lockCnt = " << m_lockCnt);
  for (int j=0; j<m_lockCnt; ++j){
    OMS_TRACE(omsTrInterface, OMS_Globals::GetCurrentLcSink(), "  " << m_locks[j]->getLockId());
  }
  if( m_lockCnt == 0){
    // nothing todo
    return;
  }
  unlock(m_lockCnt);
  m_handle->m_pSession->deallocate(m_locks);
}

/*-----------------------------------------------------*/

/// Unlock all locked RWLocks.
void OmsMultiScope::unlock(OmsTypeInt4 lockCnt)
{ 
  short error = e_ok;
  char buffer[256];
  for(int i=0; i<lockCnt; ++i){
    short err = m_locks[i]->leave();
    if( err != e_ok){
      // write error message into knldiag.err
      DbpBase opMsg(m_handle->m_pSession->m_lcSink);
      opMsg.dbpOpError("OmsMultiScope::unlock : Error %d ; areaId = %d , lockId[%d] = %d , exclusive = %s", 
                                                  err, m_areaId, i, m_locks[i]->getLockId(), m_locks[i]->getExclusive() ? "true" : "false");
    
      sp77sprintf(buffer,256,"OmsMultiScope::unlock : cannot unlock RWLock ; areaId = %d , lockId[%d] = %d, exclusive = %s", 
                                                  m_areaId, i, m_locks[i]->getLockId(), m_locks[i]->getExclusive() ? "true" : "false");
      error = err;
    }
  }
  m_handle->m_pSession->deallocate(m_locks); // deallocate memory
  m_locks = NULL;
  m_lockCnt = 0;
  if( error != e_ok){
    OMS_Globals::Throw(error, buffer, __MY_FILE__, __LINE__);
  }
}

/*-----------------------------------------------------*/


/*---- OLD INTERFACES - NO LONGER SUPPORTED------------*/
/*-----------------------------------------------------*/

OmsRWLock::OmsRWLock(OmsHandle &h, OmsTypeInt4 areaId, OmsTypeInt4 lockId)
{
  // check if the areaId and lockId is valid for lcapps
  if( areaId < 1 || lockId < 1){
    OMS_Globals::Throw(e_OMS_invalid_parameter, "Cannot create OmsRWLock", __MY_FILE__, __LINE__);
  }
  // get the specified lock from session R/W lock directory
  m_lock = &h.m_pSession->GetRWLockDir().getLock(areaId,lockId); 
}

/*-----------------------------------------------------*/

void OmsRWLock::enter(bool exclusive)
{
  short err = m_lock->enter(exclusive);
  if( err != e_ok){
    OMS_Globals::Throw(err, "Error in OmsRWLock::enter", __MY_FILE__, __LINE__);
  }
}

/*-----------------------------------------------------*/

bool OmsRWLock::tryEnter(bool exclusive)
{
  short err = m_lock->tryEnter(exclusive);
  if(err == e_OMS_lock_request_timeout)
    return false;
  else if( err != e_ok){
    OMS_Globals::Throw(err, "Error in OmsRWLock::tryEnter", __MY_FILE__, __LINE__);
  }
  return true;
}

/*-----------------------------------------------------*/

void OmsRWLock::leave()
{
  short err = m_lock->leave();
  if( err != e_ok){
    OMS_Globals::Throw(err, "Error in OmsRWLock::leave", __MY_FILE__, __LINE__);
  }
}

/*-----------------------------------------------------*/

OmsRWLock::~OmsRWLock()
{
}

/*-----------------------------------------------------*/
 
bool OmsRWLock::getLocked()
{
  return m_lock->getLocked();
}

/*-----------------------------------------------------*/

bool OmsRWLock::getExclusive()
{
  return m_lock->getExclusive();
}

/*-----------------------------------------------------*/

/// Lock the specified lock exclusively in constructor.
OmsExclusiveScope::OmsExclusiveScope(OmsHandle &h, OmsTypeInt4 areaId, OmsTypeInt4 lockId)
{
  // check if the areaId and lockId is valid for lcapps
  if( areaId < 1 || lockId < 1){
    OMS_Globals::Throw(e_OMS_invalid_parameter, "Cannot create ExclusiveScope", __MY_FILE__, __LINE__);
  }
  // get the specified lock from session R/W lock directory
  m_lock = &h.m_pSession->GetRWLockDir().getLock(areaId,lockId); 
  m_lock->enter(true);
}

/*-----------------------------------------------------*/

/// Unlock the lock in destructor.
OmsExclusiveScope::~OmsExclusiveScope()
{
  m_lock->leave();
}

/*-----------------------------------------------------*/

/// Lock the specified lock shared in constructor.
OmsSharedScope::OmsSharedScope(OmsHandle &h, OmsTypeInt4 areaId, OmsTypeInt4 lockId)
{
  // check if the areaId and lockId is valid for lcapps
  if( areaId < 1 || lockId < 1){
    OMS_Globals::Throw(e_OMS_invalid_parameter, "Cannot create SharedScope", __MY_FILE__, __LINE__);
  }
  // get the specified lock from session R/W lock directory
  m_lock = &h.m_pSession->GetRWLockDir().getLock(areaId,lockId);
  m_lock->enter(false);
}

/*-----------------------------------------------------*/

/// Unlock the lock in destructor.
OmsSharedScope::~OmsSharedScope()
{
  m_lock->leave();
}

/*-----------------------------------------------------*/
/*-----------------------------------------------------*/

