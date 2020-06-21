/*!
  @file           IFRPacket_Lock.h
  @author         D039759
  @ingroup        IFR_Packet
  @brief          The packet lock, i.e. the data structure that manages the lock.
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
#ifndef IFRPACKET_LOCK_H
#define IFRPACKET_LOCK_H

#include "SAPDBCommon/SAPDB_ReuseLib.h"

#include "Interfaces/Runtime/IFR_Types.h"
#include "Interfaces/Runtime/Util/IFRUtil_Vector.h"
#include "Interfaces/Runtime/Util/IFRUtil_RuntimeItem.h"

IFR_BEGIN_NAMESPACE

class IFR_Connection;

/**
 * Common base class for packet locks. This actually is a 
 * no-op packet lock
 */
class IFRPacket_Lock
    : public IFRUtil_RuntimeItem
{
public:
    virtual ~IFRPacket_Lock();
    virtual void acquireExclusiveLock();
    virtual void releaseExclusiveLock();
    virtual void acquireShareLock();
    virtual void releaseShareLock();
    virtual IFR_Bool isLocked();

protected:
    /**
     * Constructor. Made protected so that this one cannot
     * be instantiated directly.
     */
    inline IFRPacket_Lock(IFRUtil_RuntimeItem& item)
    :IFRUtil_RuntimeItem(item) 
    {}
    
    /**
     * Constructor. Made protected so that this one cannot
     * be instantiated directly.
     */
    inline IFRPacket_Lock(IFRUtil_RuntimeItem& item,
                          SAPDBMem_IRawAllocator& allocator)
    :IFRUtil_RuntimeItem(item, allocator) 
    {}
    
    /**
     * Constructor. Made protected so that this one cannot
     * be instantiated directly.
     */
    inline IFRPacket_Lock(SQLDBC_IRuntime& runtime,
                          SAPDBMem_IRawAllocator& allocator)
    :IFRUtil_RuntimeItem(runtime, allocator) 
    {}

    
};

/**
 * @ingroup IFR_Packet
 * The root packet lock manages the concurrency of one SQL packet, which
 * includes the reply packet of the packet, this is the packet that was 
 * retrieved by the RTE call.
 * Each request packet has an unique packet lock instance that is 
 * shared among all request and reply packet instances, and only 
 * deleted when the connection terminates, and the root packet is 
 * deleted.
 */
class IFRPacket_RootLock
    : public IFRPacket_Lock
{
public:
    /**
     * Creates the packet lock structures.
     * @param runtime The runtime to be used.
     * @param allocator The allocator to be used.
     */
    IFRPacket_RootLock(SQLDBC_IRuntime& runtime,
                       SAPDBMem_IRawAllocator& allocator,
                       IFR_Bool&  memory_ok);
    
    /**
     * Destroys the packet lock. Acquires an exclusive lock before.
     */
    ~IFRPacket_RootLock();
    
    /**
     * Acquires an exclusive lock. One thread at a time may have an
     * exclusive lock (of unlimited recursion). This thread also may
     * have shared locks.  This call may block if another thread is
     * holding the exclusive lock, or some share locks are held by
     * other threads. This call notifies all listeners before locking,
     * if there are any.
     */
    virtual void acquireExclusiveLock();
    
    /**
     * Releases the exclusive lock. If the recursion counter is 0, and 
     * there are no share locks, the semaphore is fired.
     */
    virtual void releaseExclusiveLock();
    
    /**
     * Acquires a share lock. Any thread may hold a share lock, they are
     * counted all together for recursion. This call blocks if another
     * thread holds the exclusive lock.
     */
    virtual void acquireShareLock();
    
    /**
     * Releases a share lock. If this leads to the fact that no one
     * has any kind of lock on this item, the semaphore is fired.
     */
    virtual void releaseShareLock();
    
    /**
     * Returns true if at this moment in time there is a lock that
     * would impose a wait.
     */
    virtual IFR_Bool isLocked();
    
private:
    SQLDBC_IRuntime::MutexHandle  m_lock;               //!< Lock that protects the internal data.
    
    SQLDBC_IRuntime::TaskID          m_exclusivethread;    //!< Thread id of thread that holds exclusive lock, or 0.
    SQLDBC_IRuntime::SemaphoreHandle m_exclusivesemaphore; //!< Semaphore of exclusive lock.
    IFR_Int4                         m_recursiveexclusive; //!< Number of recursive exclusive locks.
    IFR_Int4                         m_recursiveshare;     //!< Number of recursive shared locks.
};


/**
 * Dynamic Packet Lock
 */
class IFRPacket_DynamicLock
    : public IFRPacket_Lock
{
public:
    IFRPacket_DynamicLock(SQLDBC_IRuntime& runtime, 
                          SAPDBMem_IRawAllocator& allocator,
                          IFR_Connection *connection);
    ~IFRPacket_DynamicLock();
    
    virtual void acquireExclusiveLock();
    virtual void releaseExclusiveLock();
    virtual IFR_Bool isLocked();
    
    inline void lock() { runtime.lockMutex(m_lock); }
    inline void unlock() { runtime.releaseMutex(m_lock); }
    void freePacket(void *packet);
private:
    SQLDBC_IRuntime::MutexHandle m_lock;
    IFR_Int4                     m_recursivecount;
    IFR_Connection              *m_connection;
};

IFR_END_NAMESPACE

#endif  
