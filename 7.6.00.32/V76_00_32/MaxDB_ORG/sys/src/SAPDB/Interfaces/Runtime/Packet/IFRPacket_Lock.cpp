/*!
  @file           IFRPacket_Lock.cpp
  @author         D039759
  @ingroup        IFR_Packet
  @brief          Management of locks in the request/reply packet.
  @see            

\if EMIT_LICENCE

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



\endif
*/
#include "Interfaces/Runtime/IFR_Common.h"
#include "Interfaces/Runtime/Packet/IFRPacket_Lock.h"
#include "Interfaces/Runtime/Packet/IFRPacket_RequestPacket.h"
#include "Interfaces/Runtime/IFR_Connection.h"


//----------------------------------------------------------------------
// IFRPacket_Lock
//----------------------------------------------------------------------
IFRPacket_Lock::~IFRPacket_Lock()
{
}

//----------------------------------------------------------------------
void 
IFRPacket_Lock::acquireExclusiveLock()
{}

//----------------------------------------------------------------------
void 
IFRPacket_Lock::releaseExclusiveLock()
{}


//----------------------------------------------------------------------
void 
IFRPacket_Lock::acquireShareLock()
{}

//----------------------------------------------------------------------
void 
IFRPacket_Lock::releaseShareLock()
{}

//----------------------------------------------------------------------
IFR_Bool
IFRPacket_Lock::isLocked()
{
    return false;
}

//----------------------------------------------------------------------
// IFRPacket_RootLock
//----------------------------------------------------------------------
IFRPacket_RootLock::IFRPacket_RootLock(SQLDBC_IRuntime& runtime,
                                       SAPDBMem_IRawAllocator& iallocator,
                                       IFR_Bool& memory_ok)
:IFRPacket_Lock(runtime, iallocator),
 m_exclusivethread(0),
 m_recursiveexclusive(0),
 m_recursiveshare(0),
 m_lock(0),
 m_exclusivesemaphore(0)
{
    DBUG_METHOD_ENTER(IFRPacket_RootLock,IFRPacket_RootLock);
    if(!memory_ok) {
        return;
    }
    SQLDBC_IRuntime::Error error;
    runtime.createMutex(m_lock, allocator, error);
    if(m_lock == 0) {
        memory_ok = false;
        return;
    }
    runtime.createSemaphore(m_exclusivesemaphore,
                            1,
                            allocator,
                            error);
    if(m_exclusivesemaphore == 0) {
        runtime.destroyMutex(m_lock, allocator, error);
        m_lock = 0;
        memory_ok = false;
        return;
    }
      
    // TBD: React on the fail of this call 
}

//----------------------------------------------------------------------
IFRPacket_RootLock::~IFRPacket_RootLock()
{
    DBUG_METHOD_ENTER(IFRPacket_RootLock,~IFRPacket_RootLock);
    if(m_lock ==0) {
        return;
    }
    SQLDBC_IRuntime::Error ignored;
    
    runtime.lockMutex(m_lock);
    if(m_exclusivethread != runtime.getCurrentTaskID()) {
        runtime.releaseMutex(m_lock);
        runtime.waitSemaphore(m_exclusivesemaphore);
        runtime.lockMutex(m_lock);   
        runtime.destroySemaphore(m_exclusivesemaphore, 
                                 allocator, 
                                 ignored);
        runtime.releaseMutex(m_lock);
    } else {
        runtime.destroySemaphore(m_exclusivesemaphore, 
                                 allocator, 
                                 ignored);
        runtime.releaseMutex(m_lock);
    }
    runtime.destroyMutex(m_lock,
                         allocator,
                         ignored);
}


//----------------------------------------------------------------------
void
IFRPacket_RootLock::acquireExclusiveLock()
{

    DBUG_METHOD_ENTER(IFRPacket_RootLock,acquireExclusiveLock);
    // compute as much as possible in the unsynchronized
    // regions
    SQLDBC_IRuntime::TaskID my_threadid
        = runtime.getCurrentTaskID();
    // we will need to modify internal structures, therefore
    // we must lock internal structures
    runtime.lockMutex(m_lock);
    if(m_exclusivethread == my_threadid) {
        // we are in a recursive call by the lock holder.
        // thus we increase the lock count.
        m_recursiveexclusive++;
        runtime.releaseMutex(m_lock);
        return;
    } else {
        // in this case, we must wait on the semaphore
        // we release the data lock as the current holder
        // must be possible to get rid of the packet
        runtime.releaseMutex(m_lock);
        runtime.waitSemaphore(m_exclusivesemaphore);        
        // fine we have now the semaphore, thus we are alone.
        // mark that we were here. therefore acquire the 
        // data lock again.
        runtime.lockMutex(m_lock);
        m_exclusivethread=my_threadid;
        m_recursiveexclusive=1;
        runtime.releaseMutex(m_lock);
    }
}

//----------------------------------------------------------------------
void
IFRPacket_RootLock::releaseExclusiveLock()
{
    DBUG_METHOD_ENTER(IFRPacket_RootLock,releaseExclusiveLock);
    SQLDBC_IRuntime::TaskID my_threadid
        =runtime.getCurrentTaskID();
    runtime.lockMutex(m_lock);
    if(my_threadid == m_exclusivethread) {
        m_recursiveexclusive--;
        // erase the owner if we are the last exclusive!
        if(m_recursiveexclusive==0) {
            m_exclusivethread=0;
        }
        // we will let the other threads only enter if 
        // this thread did not acquire a share lock during the 
        // possession of the request packet (this is the common case).
        if(m_recursiveexclusive==0 && m_recursiveshare==0) {
            runtime.signalSemaphore(m_exclusivesemaphore);
        }
    } else {
        // INCONSISTENT STATUS!
    }
    runtime.releaseMutex(m_lock);
    return;
}


//----------------------------------------------------------------------
void
IFRPacket_RootLock::acquireShareLock()
{
    DBUG_METHOD_ENTER(IFRPacket_RootLock,acquireShareLock);
    SQLDBC_IRuntime::TaskID my_threadid = 
        runtime.getCurrentTaskID();
    runtime.lockMutex(m_lock);
    // the easy way - if the thread has already the exclusive lock,
    // he can add as many share locks as possible.
    if(my_threadid == m_exclusivethread) {
        m_recursiveshare++;
        runtime.releaseMutex(m_lock);
        return;
    }
    // also easy - if there are only share locks, 
    // we add us to them ...
    if(m_exclusivethread == 0 && m_recursiveshare>0) {
        m_recursiveshare++;
        runtime.releaseMutex(m_lock);
        return;
    }
    // if it is abandoned, or exclusively locked by some other, 
    // we cannot do anything that makes sense here
    if((m_exclusivethread == 0 && m_recursiveshare==0) || 
       (m_exclusivethread!=my_threadid && m_exclusivethread!=0)) {
        // INCONSISTENT STATE ???
    }
    runtime.releaseMutex(m_lock);
}

//----------------------------------------------------------------------
void 
IFRPacket_RootLock::releaseShareLock()
{
    DBUG_METHOD_ENTER(IFRPacket_RootLock,releaseShareLock);
    runtime.lockMutex(m_lock);
    m_recursiveshare--;
    // if it is 0, we fire a signal for the next waiting writer
    if(m_recursiveshare==0 && m_recursiveexclusive==0) {
        runtime.signalSemaphore(m_exclusivesemaphore);
    }
    runtime.releaseMutex(m_lock);
}


//----------------------------------------------------------------------
IFR_Bool
IFRPacket_RootLock::isLocked()
{
    DBUG_METHOD_ENTER(IFRPacket_RootLock,isLocked);
    DBUG_RETURN(m_recursiveexclusive && m_recursiveshare);
}


//----------------------------------------------------------------------
IFRPacket_DynamicLock::IFRPacket_DynamicLock(SQLDBC_IRuntime& iruntime,
                                             SAPDBMem_IRawAllocator& iallocator,
                                             IFR_Connection* connection)
:IFRPacket_Lock(iruntime, iallocator),
 m_lock(0),
 m_recursivecount(0),
 m_connection(connection)
{
    DBUG_METHOD_ENTER(IFRPacket_DynamicLock, IFRPacket_DynamicLock);
    SQLDBC_IRuntime::Error ignored;
    runtime.createMutex(m_lock, allocator, ignored);
}

//----------------------------------------------------------------------
IFRPacket_DynamicLock::~IFRPacket_DynamicLock()
{
    DBUG_METHOD_ENTER(IFRPacket_DynamicLock, ~IFRPacket_DynamicLock);
    SQLDBC_IRuntime::Error ignored;
    runtime.destroyMutex(m_lock, allocator, ignored);
}

//----------------------------------------------------------------------
void
IFRPacket_DynamicLock::acquireExclusiveLock()
{
    DBUG_METHOD_ENTER(IFRPacket_DynamicLock, acquireExclusiveLock);
    runtime.lockMutex(m_lock);
    m_recursivecount++;
    runtime.releaseMutex(m_lock);
}

//----------------------------------------------------------------------
void
IFRPacket_DynamicLock::releaseExclusiveLock()
{
    DBUG_METHOD_ENTER(IFRPacket_DynamicLock, releaseExclusiveLock);
    runtime.lockMutex(m_lock);
    m_recursivecount--;
    runtime.releaseMutex(m_lock);
}

//----------------------------------------------------------------------
IFR_Bool
IFRPacket_DynamicLock::isLocked()
{
    DBUG_METHOD_ENTER(IFRPacket_DynamicLock, isLocked);
    return m_recursivecount != 0;
}

//----------------------------------------------------------------------
void
IFRPacket_DynamicLock::freePacket(void *packet)
{
    m_connection->freeRequestPacket(packet);
}
