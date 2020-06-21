/*!
 * \file    OMS_RWLock.cpp
 * \author  ChristophR
 * \brief   Reader-writer lock.
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

#include "Oms/OMS_RWLock.hpp"
#include "Oms/OMS_Handle.hpp"
#include "Oms/OMS_Globals.hpp"
#include "Oms/OMS_Session.hpp"
#include "Oms/OMS_DbpError.hpp"
#include "liveCache/LVC_KernelInterface.hpp"
#include "liveCache/LVC_LockRequest.hpp"
#include "Oms/OMS_Defines.h"


/*-----------------------------------------------------*/

OMS_RWLock::OMS_RWLock( OmsTypeInt4 areaId ,OmsTypeInt4 lockId, tsp00_TaskId taskId)
  : m_areaId(areaId),
    m_lockId(lockId),
    m_taskId(taskId),
    m_isLocked(false), 
    m_isExclusive(false),
    m_isCreated(false),
	  m_lockAddress(NULL)
{}

/*-----------------------------------------------------*/

void* OMS_RWLock::operator new (size_t sz, OMS_Session* pSession)
{
  return pSession->allocate(sz);
}

/*-----------------------------------------------------*/

void OMS_RWLock::operator delete (void *ptr, OMS_Session* pSession)
{
  return pSession->deallocate(ptr);
}

/*-----------------------------------------------------*/

short OMS_RWLock::enter(bool exclusive)
{
  if (m_isLocked)
    return e_already_in_critical_section;

  if (m_lockAddress == NULL) {
    LVC_LockRequest lock(LVC_LockRequest::RWLOCK_CREATE, m_areaId, m_lockId, m_lockAddress, m_taskId);
    short err = OMS_Globals::GetKernelInterface()->LockRequest(lock);
    if (err != e_ok)
      return err;
    m_lockAddress = lock.m_lockAddress;
  }

  LVC_LockRequest lock(exclusive ? LVC_LockRequest::RWLOCK_LOCK_EXCLUSIVE :
    LVC_LockRequest::RWLOCK_LOCK_SHARED, m_areaId, m_lockId, m_lockAddress, m_taskId);
  short err = OMS_Globals::GetKernelInterface()->LockRequest(lock);
  if (err != e_ok)
    return err;

  m_isLocked = true;
  m_isExclusive = exclusive;
  return e_ok;
}

/*-----------------------------------------------------*/

short OMS_RWLock::tryEnter(bool exclusive)
{
  if (m_isLocked)
    return e_already_in_critical_section;

  if (m_lockAddress == NULL) {
    LVC_LockRequest lock(LVC_LockRequest::RWLOCK_CREATE, m_areaId, m_lockId, m_lockAddress, m_taskId);
    short err = OMS_Globals::GetKernelInterface()->LockRequest(lock);
    if (err != e_ok)
      return err;
	  m_lockAddress = lock.m_lockAddress;
  }

  LVC_LockRequest lock(exclusive ? LVC_LockRequest::RWLOCK_TRYLOCK_EXCLUSIVE :
    LVC_LockRequest::RWLOCK_TRYLOCK_SHARED, m_areaId, m_lockId, m_lockAddress, m_taskId);
  short err = OMS_Globals::GetKernelInterface()->LockRequest(lock);
  if (err != e_ok)
    return err;
 
  m_isLocked = true;
  m_isExclusive = exclusive;
  return e_ok;
}

/*-----------------------------------------------------*/

short OMS_RWLock::leave()
{
  if (!m_isLocked)
    return e_not_in_critical_section;

  LVC_LockRequest lock(m_isExclusive ? LVC_LockRequest::RWLOCK_UNLOCK_EXCLUSIVE :
    LVC_LockRequest::RWLOCK_UNLOCK_SHARED, m_areaId, m_lockId, m_lockAddress, m_taskId);
  short err = OMS_Globals::GetKernelInterface()->LockRequest(lock);
  if (err != e_ok)
    return err;

  m_isLocked = false;
  m_isExclusive = false;
  return e_ok;
}


/*-----------------------------------------------------*/

OMS_RWLockDirectory::OMS_RWLockDirectory() 
: m_lockCnt(0),
  m_pSession(NULL)
{
  for(int slot = 0; slot < HEAD_ENTRIES; ++slot) {
    m_head[slot] = NULL;
  }
}

/*-----------------------------------------------------*/

OMS_RWLockDirectory::~OMS_RWLockDirectory() 
{
  OMS_TRACE(omsTrLock, OMS_Globals::GetCurrentLcSink(), "OMS_RWLockDirectory::~OMS_RWLockDirectory ; taskId = " << m_pSession->GetTaskId());

  for(int slot = 0; slot < HEAD_ENTRIES; ++slot) {
    OMS_RWLock *curr = m_head[slot];
    OMS_RWLock *prev = NULL;
    while (curr != NULL) {
      prev = curr;
      curr = curr->m_pHashNext;
      m_pSession->deallocate(prev);
    }
    m_head[slot] = NULL;
  }
}

/*-----------------------------------------------------*/

OMS_RWLock * OMS_RWLockDirectory::create(int areaId, int lockId)
{
  OMS_TRACE(omsTrLock, OMS_Globals::GetCurrentLcSink(), "OMS_RWLockDirectory::create ; areaId = " << areaId << ", lockId = " << lockId 
              << "taskId = " << m_pSession->GetTaskId());

  ++m_lockCnt;
  if((m_lockCnt % (HEAD_ENTRIES*2)) == 0)
    cleanup(); // Hash is filled, cleanup not locked RWLocks

  // Create new OMS_RWLock and insert into hash
  OMS_RWLock *p = new(m_pSession) OMS_RWLock(areaId, lockId, m_pSession->GetTaskId());
  int slot = hash(areaId,lockId);
  p->m_pHashNext = m_head[slot];
  m_head[slot] = p;
  return p;
}

/*-----------------------------------------------------*/

void OMS_RWLockDirectory::cleanup()
{
  OMS_TRACE(omsTrLock, OMS_Globals::GetCurrentLcSink(), "OMS_RWLockDirectory::cleanup ; taskId = " << m_pSession->GetTaskId()); 

  for(int slot = 0; slot < HEAD_ENTRIES; ++slot){
    OMS_RWLock *curr = m_head[slot];
    OMS_RWLock *prev = NULL;
    OMS_RWLock *del = NULL;
    while (curr != NULL) {
      if(!curr->getLocked()){
        //RWLock is not locked, we can delete it
        if(prev == NULL)
          m_head[slot] = curr->m_pHashNext;
        else
          prev->m_pHashNext = curr->m_pHashNext;
        del = curr;
        curr = curr->m_pHashNext;
        m_pSession->deallocate(del);
        --m_lockCnt;
      }else {
        // RWLock is locked, we have to keep it
        prev = curr;
        curr = curr->m_pHashNext;
      }
    }
  }
}

/*-----------------------------------------------------*/

void OMS_RWLockDirectory::transactionEnd()
{
  //cgg251dclIterator<OMS_RWLock,OMS_Session> iter =  m_transactionLockList.begin();
  //while(iter){
  //  OMS_RWLock *curr = iter();
  //  iter++;
  //  curr->leave();
  //}
  //m_transactionLockList.remove_all();
}

/*-----------------------------------------------------*/

/// Lock the specified lock in constructor.
OMS_InternalLockScope::OMS_InternalLockScope(OMS_Session *pSession, OmsTypeInt4 areaId, OmsTypeInt4 lockId, bool exclusive)
{
  OMS_TRACE(omsTrLock, OMS_Globals::GetCurrentLcSink(), "OMS_InternalLockScope::OMS_InternalLockScope : areaId = " 
              << areaId << ", lockId = " << lockId << ", exclusive = " << exclusive); 
  // check if the areaId and lockId is valid for liboms
  if( areaId > 0){
    char buffer[256];
    sp77sprintf(buffer,256, "OMS_InternalLockScope::OMS_InternalLockScope : areaId invalid ; areaId = %d , lockId = %d , exclusive = %s", 
                                                areaId, lockId, exclusive == true ? "true" : "false" );
    OMS_Globals::Throw(e_OMS_invalid_parameter, buffer, __MY_FILE__, __LINE__);
  }
  if( lockId < 1){
    char buffer[256];
    sp77sprintf(buffer,256, "OMS_InternalLockScope::OMS_InternalLockScope : lockId invalid ; areaId = %d , lockId = %d , exclusive = %s", 
                                                areaId, lockId, exclusive == true ? "true" : "false");
    OMS_Globals::Throw(e_OMS_invalid_parameter, buffer, __MY_FILE__, __LINE__);
  }
  // get the specified lock from session R/W lock directory
  m_lock = &pSession->GetRWLockDir().getLock(areaId,lockId);
  short err = m_lock->enter(exclusive);
  if (err != 0) { // Cannot lock RWLock
    char buffer[256];
    sp77sprintf(buffer,256,"OMS_InternalLockScope::OMS_InternalLockScope : cannot lock RWLock ; areaId = %d , lockId = %d, exclusive = %s", 
                                                areaId, lockId, exclusive == true ? "true" : "false");
    OMS_Globals::Throw(err, buffer, __MY_FILE__, __LINE__);
  }

}

/*-----------------------------------------------------*/

/// Unlock the lock in destructor.
OMS_InternalLockScope::~OMS_InternalLockScope()
{
  OMS_TRACE(omsTrLock, OMS_Globals::GetCurrentLcSink(), "OMS_InternalLockScope::~OMS_InternalLockScope : areaId = " 
              << m_lock->getAreaId() << ", lockId = " << m_lock->getLockId()); 
  short err = m_lock->leave();
  if (err != 0) { // Cannot unlock RWLock
    char buffer[256];
    sp77sprintf(buffer,256,"OMS_InternalLockScope::~OMS_InternalLockScope : cannot unlock RWLock ; areaId = %d , lockId = %d, exclusive = %s", 
                                                m_lock->getAreaId(), m_lock->getLockId(), m_lock->getExclusive() ? "true" : "false");
    OMS_Globals::Throw(err, buffer, __MY_FILE__, __LINE__);
  }
}

/*-----------------------------------------------------*/

/// Lock the specified lock in constructor.
OMS_InternalKernelLockScope::OMS_InternalKernelLockScope(
  OmsTypeInt4  areaId, 
  OmsTypeInt4  lockId, 
  bool         exclusive
  )  
: m_exclusive(exclusive),
  m_areaId(areaId),
  m_lockId(lockId)
{
  //OMS_TRACE(omsTrInterface, OMS_Globals::GetCurrentLcSink(), "OMS_InternalKernelLockScope::OMS_InternalKernelLockScope : areaId = " 
  //            << areaId << ", lockId = " << lockId << ", exclusive = " << exclusive); 
  // check if the areaId and lockId is valid for liboms
  if( areaId > 0){
    char buffer[256];
    sp77sprintf(buffer,256, "OMS_InternalKernelLockScope::OMS_InternalKernelLockScope : areaId invalid ; areaId = %d , lockId = %d , exclusive = %s", 
                                                areaId, lockId, exclusive == true ? "true" : "false" );
    OMS_Globals::Throw(e_OMS_invalid_parameter, buffer, __MY_FILE__, __LINE__);
  }
  if( lockId < 1){
    char buffer[256];
    sp77sprintf(buffer,256, "OMS_InternalKernelLockScope::OMS_InternalKernelLockScope : lockId invalid ; areaId = %d , lockId = %d , exclusive = %s", 
                                                areaId, lockId, exclusive == true ? "true" : "false");
    OMS_Globals::Throw(e_OMS_invalid_parameter, buffer, __MY_FILE__, __LINE__);
  }

  LVC_LockRequest lockCreate(LVC_LockRequest::RWLOCK_CREATE, m_areaId, m_lockId, NULL);
  short err = OMS_Globals::m_globalsInstance->GetKernelInterface()->LockRequest(lockCreate);
  if (err != 0) { // Cannot create RWLock
    char buffer[256];
    sp77sprintf(buffer,256,"OMS_InternalKernelLockScope::OMS_InternalKernelLockScope : cannot create RWLock ; areaId = %d , lockId = %d", m_areaId, m_lockId);
    OMS_Globals::Throw(err, buffer, __MY_FILE__, __LINE__);
  }
  m_lockAddress = lockCreate.m_lockAddress;

  LVC_LockRequest lockEnter( m_exclusive == true ? LVC_LockRequest::RWLOCK_LOCK_EXCLUSIVE : LVC_LockRequest::RWLOCK_LOCK_SHARED, 
                             m_areaId, m_lockId, m_lockAddress);
  err = OMS_Globals::m_globalsInstance->GetKernelInterface()->LockRequest(lockEnter);  
  if (err != 0) { // Cannot lock RWLock
    char buffer[256];
    sp77sprintf(buffer,256,"OMS_InternalKernelLockScope::OMS_InternalKernelLockScope : cannot lock RWLock ; areaId = %d , lockId = %d, exclusive = %s", 
                                                m_areaId, m_lockId, m_exclusive == true ? "true" : "false");
    OMS_Globals::Throw(err, buffer, __MY_FILE__, __LINE__);
  }
}

/*-----------------------------------------------------*/

/// Unlock the lock in destructor.
OMS_InternalKernelLockScope::~OMS_InternalKernelLockScope()
{

  //OMS_TRACE(omsTrInterface, OMS_Globals::GetCurrentLcSink(), "OMS_InternalKernelLockScope::~OMS_InternalKernelLockScope : areaId = " 
  //            << m_areaId << ", m_lockId = " << m_lockId); 
  LVC_LockRequest lockLeave( m_exclusive == true ? LVC_LockRequest::RWLOCK_UNLOCK_EXCLUSIVE : LVC_LockRequest::RWLOCK_UNLOCK_SHARED, 
                             m_areaId, m_lockId, m_lockAddress);
  short err = OMS_Globals::m_globalsInstance->GetKernelInterface()->LockRequest(lockLeave);  

  // ATTENTION: For scopes there should never occur an error during 'leave'. 
  // As no exception should be thrown, and as no return parameter can be specified in
  // a destructor, the program is stopped.
  if (err != NULL) { // Cannot unlock RWLock
    DbpBase opMsg(OMS_Globals::m_globalsInstance->GetCurrentLcSink());
    opMsg.dbpOpError("OMS_InternalKernelLockScope::~OMS_InternalKernelLockScope : Error %d ; areaId = %d , lockId = %d , exclusive = %s", 
                                                  err, m_areaId, m_lockId, m_exclusive == true ? "true" : "false");
    OMS_Globals::m_globalsInstance->GetKernelInterface()->Crash("OMS_InternalKernelLockScope::~OMS_InternalKernelLockScope");
  }
}

/*-----------------------------------------------------*/

/// Lock the specified locks in constructor.
OMS_InternalMultiScope::OMS_InternalMultiScope(OMS_Session *pSession, OmsTypeInt4 areaId, const OmsTypeInt4 *lockId, OmsTypeInt4 lockCnt, bool exclusive)
: m_pSession(pSession), m_lockCnt(lockCnt), m_areaId(areaId)
{
  OMS_TRACE(omsTrLock, OMS_Globals::GetCurrentLcSink(), "OMS_InternalMultiScope::OMS_InternalMultiScope : areaId = " 
              << m_areaId << ", lockCnt = " << m_lockCnt << ", exclusive = " << exclusive);

  // check if the lockCnt is valid
  if( m_lockCnt < 0){
    char buffer[256];
    sp77sprintf(buffer,256, "OMS_InternalMultiScope::OMS_InternalMultiScope : lockCnt invalid ; areaId = %d , lockCnt = %d , exclusive = %s", 
                                                    areaId, lockCnt, exclusive == true ? "true" : "false");
    OMS_Globals::Throw(e_OMS_invalid_parameter, buffer, __MY_FILE__, __LINE__);
  }
  // check if areaId is valid for liboms
  if( m_areaId > 0){
    char buffer[256];
    sp77sprintf(buffer,256, "OMS_InternalMultiScope::OMS_InternalMultiScope : areaId invalid ; areaId = %d , lockCnt = %d , exlcusive = %s", 
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
    OMS_TRACE(omsTrLock, OMS_Globals::GetCurrentLcSink(), "OMS_InternalMultiScope::OMS_InternalMultiScope : areaId = " 
              << m_areaId << ", lockId[" << i << "] = " << lockId[i] << ", exclusive = " << exclusive);
    if( lockId[i] < 1 ){
      char buffer[256];
      sp77sprintf(buffer,256,"OMS_InternalMultiScope::OMS_InternalMultiScope : lockId invalid ; areaId = %d , lockId[%d] = %d", areaId, i, lockId[i]);
      OMS_Globals::Throw(e_OMS_invalid_parameter, buffer, __MY_FILE__, __LINE__);
    }
    // The ids must be sorted ascending, otherwise deadlocks might occur
    if(i>0 && lockId[i] <= lockId[i-1]){
      char buffer[256];
      sp77sprintf(buffer,256,"OMS_InternalMultiScope::OMS_InternalMultiScope : Ids must be sorted ascending ; areaId = %d , lockId[%d] = %d , lockId[%d] = %d , exclusive = %s", 
                                                  areaId, i-1, lockId[i-1], i, lockId[i], exclusive == true ? "true" : "false");
      OMS_Globals::Throw(e_OMS_invalid_parameter, buffer, __MY_FILE__, __LINE__);
    }
  }

  m_locks = reinterpret_cast<OMS_RWLock**>( m_pSession->allocate(sizeof(OMS_RWLock*) * lockCnt));
  for(i=0; i<m_lockCnt; ++i){
    // get the specified lock from session R/W lock directory
    m_locks[i] = &m_pSession->GetRWLockDir().getLock(areaId,lockId[i]);
    short err = m_locks[i]->enter(exclusive);
    if (err != 0) { // Cannot lock RWLock, cleanup
      // write error message into knldiag.err
      DbpBase opMsg(m_pSession->m_lcSink);
      opMsg.dbpOpError("OMS_InternalMultiScope::OMS_InternalMultiScope : Error %d ; areaId = %d , lockId[%d] = %d , exclusive = %s", 
                                                  err, areaId, i, lockId[i], exclusive == true ? "true" : "false");
      unlock(i); // unlock all allready locked locks
      m_pSession->deallocate(m_locks); // deallocate memory
      char buffer[256];
      sp77sprintf(buffer,256,"OMS_InternalMultiScope::OMS_InternalMultiScope : cannot lock RWLock ; areaId = %d , lockId[%d] = %d , exclusive = %s", 
                                                  areaId, i, lockId[i], exclusive == true ? "true" : "false");
      OMS_Globals::Throw(err, buffer, __MY_FILE__, __LINE__);
    }
  }
}

/*-----------------------------------------------------*/

/// Unlock the locks in destructor.
OMS_InternalMultiScope::~OMS_InternalMultiScope()
{
  OMS_TRACE(omsTrLock, OMS_Globals::GetCurrentLcSink(), "OMS_InternalMultiScope::~OMS_InternalMultiScope : areaId = " 
                << m_areaId << ", lockCnt = " << m_lockCnt);
  if( m_lockCnt == 0){
    // nothing todo
    return;
  }
  unlock(m_lockCnt);
}

/*-----------------------------------------------------*/

/// Unlock all locked RWLocks.
void OMS_InternalMultiScope::unlock(OmsTypeInt4 lockCnt)
{ 
  short error = e_ok;
  char buffer[256];
  for(int i=0; i<lockCnt; ++i){
    OMS_TRACE(omsTrLock, OMS_Globals::GetCurrentLcSink(), "OMS_InternalMultiScope::unlock : areaId = " 
      << m_areaId << ", lockId[" << i << "] = " << m_locks[i]->getLockId() << ", exclusive = " << m_locks[i]->getExclusive());
    short err = m_locks[i]->leave();
    if( err != e_ok){
      // write error message into knldiag.err
      DbpBase opMsg(m_pSession->m_lcSink);
      opMsg.dbpOpError("OMS_InternalMultiScope::unlock : Error %d ; areaId = %d , lockId[%d] = %d , exclusive = %s", 
                                                  err, m_areaId, i, m_locks[i]->getLockId(), m_locks[i]->getExclusive() ? "true" : "false");
    
      sp77sprintf(buffer,256,"OMS_InternalMultiScope::unlock : cannot unlock RWLock ; areaId = %d , lockId[%d] = %d , exclusive = %s", 
                                                  m_areaId, i, m_locks[i]->getLockId(), m_locks[i]->getExclusive() ? "true" : "false");
      error = err;
    }
  }
  m_pSession->deallocate(m_locks); // deallocate memory
  m_locks = NULL;
  m_lockCnt = 0;
  if( error != e_ok){
    OMS_Globals::Throw(error, buffer, __MY_FILE__, __LINE__);
  }
}