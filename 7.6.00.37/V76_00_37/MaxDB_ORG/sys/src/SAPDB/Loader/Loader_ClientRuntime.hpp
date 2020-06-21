/*!
  \file    Loader_ClientRuntime.hpp
  \author  SteffenS
  \brief   Implements the interface runtime used in Loader.

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (C) 2002-2004 SAP AG

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

#ifndef LOADER_CLIENTRUNTIME_HPP
#define LOADER_CLIENTRUNTIME_HPP
/*
#ifndef LOADER_TRACING
#define LOADER_TRACING 1
#endif
*/

#include "Interfaces/SQLDBC/SQLDBC_IRuntime.h"
#include "Interfaces/SQLDBC/SQLDBC_ClientRuntime.h"
#include "Loader/Loader_Types.hpp"
//#ifdef LOADER_TRACING
//    #include "Interfaces/SQLDBC/SQLDBC_ClientTrace.h"
//#endif


#ifdef WIN32
#define LOADER_DLLEXPORT __declspec(dllexport)
#else
#define LOADER_DLLEXPORT
#endif



// dummy class to supply a constructor for SQLDBC_IRuntime
class Loader_ClientRuntime_SpinLock : public SQLDBC_Spinlock
{
    public:
        Loader_ClientRuntime_SpinLock (){};
        virtual ~Loader_ClientRuntime_SpinLock() {}

        virtual LOADER_Bool lock() {
            return LOADER_TRUE;}

        virtual LOADER_Bool unlock() {
            return LOADER_TRUE;}

        virtual LOADER_Bool deleteLock() {
            return LOADER_TRUE;}
};


//----------------------------------------------------------------------
class Loader_ClientRuntime : public SQLDBC_IRuntime
{
public:
    /*!
       @brief constructor
     */
    LOADER_DLLEXPORT
    Loader_ClientRuntime(LOADER_Char* pErrorText, LOADER_Int4 lErrorTextSize);

    /*!
       @brief destructor
     */
    virtual ~Loader_ClientRuntime()
    {
        // nothing to do so far
    }

    virtual const char *getIdentifier() const;
   
    /**
     * Retrieves the <b>global</b> allocator of the environment.
     * The global allocator is used for allocating globally used memory.
     * It is also used if no special allocator is specified when a connection
     * is created.
     * @internal
     * The <code>SAPDBMem_SynchronizedRawAllocator</code> would 
     * fit into this.
     * @return A reference to the global allocator.
     */
    virtual SAPDBMem_IRawAllocator& getGlobalAllocator()
    {
        return m_SQLDBCRuntime->getGlobalAllocator();
    }

    // Packet and Session Handling

    /**
     * Requests a session. The session is used to send SQL statement to
     * the database server. Depending on the application context, this
     * method may open a client connection, or re-use an existing
     * context.
     * @param connectUrl The URL for the physical connection. If the parameter
     *   is not used, its value must be zero.
     * @param connectCommand The SQL CONNECT statement, as it will be 
     *   sent to the database null-terminated. If the parameter
     *   is not used, its value must be zero.
     * @param password The unencrypted password. If the parameter
     *   is not used, its value must be zero.
     * @param passwordLength The length of the <var>password</var> in bytes.
     * @param commandEncoding The encoding of the connect command 
     *    and the password.
     * @param sessionID [out] The ID of the session. It must be
     *  a number &gt; zero.
     * @param packetSize [out] The size of the packet in bytes.
     * @param packetListSize [in|out] The size of the packet list. 
     *        The number of returned packets may be smaller but must not 
     *        exceed the number of packets specified in the list. 
     *        The number of packets actually created is reported in this 
     *        variable return.
     * @param packetList The packet list. An array of 
     *   <var>packetListSize</var> pointers to <code>void *</code>,
     *   which point to a block having the size of <var>packetSize</var>
     *   which are to be used in the request() method.
     * @param sessionInfoReply Pointer reply to th to the CONNECT statement.
     *        The reply contains at least a <i>session info part</i> to inform 
     *        the application on details about the database (Unicode flag,
     *   date format, etc.). The reply is associated with the first element
     *   of the returned packet list.
     * @param errorMessages A message list, which filled in case of an error.
     * @param allocator The allocator for allocating memory.
     * @return <code>true</code> if the session is established, <code>false</code>
     *         if it fails. In this case the <var>errorMessages</var> are filled.
     */
    virtual LOADER_Bool getSession(const char*                  connectUrl,
                                   const char*                  connectCommand,
                                   const char*                  password,
                                   SAPDB_Int4                   passwordLength,
                                   const SQLDBC_StringEncoding  commandEncoding,
                                   SAPDB_Int8&                  sessionID,
                                   SAPDB_UInt4&                 packetSize,
                                   SAPDB_Int4&                  packetListSize,
                                   void**                       packetList,  
                                   void**                       sessionInfoReply,
                                   SQLDBC_IRuntime::Error&      errorMessages,
                                   SAPDBMem_IRawAllocator*      allocator = 0);


    virtual LOADER_Bool getSession(const char *connectUrl,
                                   const char *connectCommand,
                                   const char *username,
                                   SAPDB_Int4  usernamelength,
                                   const char *password,
                                   SAPDB_Int4  passwordLength,
                                   const SQLDBC_StringEncoding commandEncoding,
                                   SAPDB_Int8&  sessionID,
                                   SAPDB_UInt4& packetSize,
                                   SAPDB_Int4& packetListSize,
                                   void **packetList,  
                                   void **sessionInfoReply,
                                   SQLDBC_IRuntime::Error&      errorMessages,
                                   SAPDBMem_IRawAllocator* allocator = 0);
    
    /**
     * Closes a session previously fetched via getSession().
     * @param sessionID The session id previously acquired using 
     *   <code>getSession()</code>.
     * @param errorMessages A message list, which filled in case of an error.
     * @return <code>true</code> if the execution succeeds, <code>false</code>
     *         if it fails. In this case the message list is filled.
     */
    virtual LOADER_Bool releaseSession(LOADER_Int8              sessionID,
                                       SQLDBC_IRuntime::Error&  errorMessages);

    //*
    //* Get parameters of last successful connection
    //*
    void
    getLastConnectionParameters(LOADER_Int8&            LastSessionID,
                                void**                  ppLastPacketList,
                                LOADER_UInt4&           lLastPacketSize,
                                LOADER_UInt4&           lLastPageSize,
                                LOADER_DateTimeFormat&  LastDateTimeFormat,
                                LOADER_Char*            pszLastUserGroupDefault,
                                LOADER_Char*            pszLastSYSDBADefault) const;
    
    /**
     * Performs a request to the database kernel. 
     * @param sessionID The session ID previously fetched using getSession().
     * @param requestData Pointer to the request data, usually an element of 
     * the packet list fetched using <code>getSession()</code>.
     * @param requestDataLength The number of bytes to be sent.
     * @param errorMessages A message list, which filled in case of an error.
     * @return <code>true</code> if the execution succeeds, <code>false</code>
     *         if it fails. In this case the message list is filled.
     */
    virtual LOADER_Bool request(LOADER_Int8  sessionID,
                               void       *requestData,
                               LOADER_UInt4 requestDataLength,
                               SQLDBC_IRuntime::Error& error)
    {
        return m_SQLDBCRuntime->request(sessionID, requestData, requestDataLength, error);
    }

    /**
     * Retrieves data from the database kernel. 
     * @param sessionID The session ID previously fetched using getSession().
     * @param replyData Pointer to the pointer to the reply.
     * @param replyDataLength Length of the reply returnes.
     * @param errorMessages A message list, which filled in case of an error.
     * @return <code>true</code> if the execution succeeds, <code>false</code>
     *         if it fails. In this case the message list is filled.
     */
    virtual LOADER_Bool receive(LOADER_Int8 sessionID,
                               void **replyData,
                               LOADER_Int4& replyDataLength,
                               SQLDBC_IRuntime::Error& error)
    {
        return m_SQLDBCRuntime->receive(sessionID, replyData, replyDataLength, error);
    }

    /**
     * Cancels the current SQL statment.
     * @param sessionID The session ID previously fetched using getSession().
     * @param errorMessages A message list, which is filled in case of an error.
     * @return <code>true</code> if the execution succeeds, <code>false</code>
     *         if it fails. In this case the message list is filled.
     */
    virtual LOADER_Bool cancelCurrentCommand(LOADER_Int8 sessionID,
                                             SQLDBC_IRuntime::Error& error)
    {
        return m_SQLDBCRuntime->cancelCurrentCommand(sessionID, error);
    }


    virtual LOADER_Bool checkKeyOption(const char *keyoption,
                                       char *servernode,
                                       SAPDB_Int4& servernodelength,
                                       char *serverdb,
                                       SAPDB_Int4& serverdblength,
                                       SAPDB_Int4& isolationlevel,
                                       SAPDB_Int4& cachelimit,
                                       SAPDB_Int4& sqlmode,
                                       SAPDB_Int4& timeout,
                                       char *username,
                                       SAPDB_Int4& usernamelength,
                                       SQLDBC_StringEncoding& usernameEncoding,
                                       SQLDBC_IRuntime::Error& error)
    {
        return m_SQLDBCRuntime->checkKeyOption(keyoption, servernode, servernodelength, serverdb, serverdblength, isolationlevel, cachelimit, sqlmode,
                                               timeout, username, usernamelength, usernameEncoding, error);
    }



    // -- synchronization primitives


    virtual SQLDBC_Spinlock* createSpinlock(SAPDBMem_IRawAllocator& allocator)
    {
        // noop in Loader
        return &m_dummySpinlock;
    }

    virtual SAPDB_Bool releaseSpinlock(SAPDBMem_IRawAllocator& allocator, SQLDBC_Spinlock* lock)
    {
        // noop in Loader
        return LOADER_TRUE;
    }




    /**
     * Creates a mutex.
     * @param mutexHandle The handle to the mutex, that is set 
     *   on return.
     * @param allocator The allocator used allocate the mutex handle.
     * @param errorMessages A message list, which is filled in case of an error.
     * @return <code>true</code> if the execution succeeds, <code>false</code>
     *         if it fails. In this case the message list is filled.
     */    
    virtual LOADER_Bool createMutex(MutexHandle&            mutexHandle, 
                                    SAPDBMem_IRawAllocator& allocator,
                                    SQLDBC_IRuntime::Error& errorMessages)
    {
        // prevent null pointer
        mutexHandle = (void *) this;
        // noop in Loader
        return LOADER_TRUE;
    }

    /**
     * Locks a mutex.
     * @param mutexHandle The handle to the mutex.
     * @return <code>true</code> if the method succeeds,
     * <code>false</code>  if it fails.
     */
    virtual LOADER_Bool lockMutex(MutexHandle mutexHandle)
    {
        // noop in Loader
        return LOADER_TRUE;
    }
    
    /**
     * Unlocks a mutex.
     * @param mutexHandle The handle to the mutex that shall be unlocked.
     * @return <code>true</code> if the mutex was unlocked successfully,
     * <code>false</code> if it fails. 
     */
    virtual LOADER_Bool releaseMutex(MutexHandle mutexHandle)
    {
        // noop in Loader
        return LOADER_TRUE;
    }
    
    /**
     * Destroys a mutex. Write a zero in the handle.
     * @param mutexHandle The handle to the mutex.
     * @param allocator The allocator used for de-allocating the memory used by the mutex.
     * @param errorMessages A message list, which is filled in case of an error.
     * @return <code>true</code> if the execution succeeds, <code>false</code>
     *         if it fails. In this case the message list is filled.
     */    
    virtual LOADER_Bool destroyMutex(MutexHandle& mutexHandle, 
                                     SAPDBMem_IRawAllocator& allocator,
                                     SQLDBC_IRuntime::Error& error)
    {
        // noop in Loader
        return LOADER_TRUE;
    }


    /**
     * Creates an atomic counter.
     * @param counterHandle The handle to the counter that is created.
     * @param allocator The allocator used to allocate memory to the counter handle.
     * @param errorMessages A message list, which is filled in case of an error.
     * @return <code>true</code> if the execution succeeds, <code>false</code>
     *         if it fails. In this case the message list is filled.
     */    
    virtual LOADER_Bool  createCounter(CounterHandle& counterHandle,
                                       SAPDBMem_IRawAllocator& allocator,
                                       SQLDBC_IRuntime::Error& error)
    {
        return m_SQLDBCRuntime->createCounter(counterHandle, allocator, error);
    }

    
    /** 
     * Increments the counter.
     * @param counterHandle The handle to the counter be incremented.
     * @return The new value of the counter.
     */
    virtual LOADER_UInt4 nextCounter(CounterHandle counterHandle)
    {
        return m_SQLDBCRuntime->nextCounter(counterHandle);
    }
    
    /**
     * Destroys an atomic counter.
     * @param counterHandle The counter to be destroyed.
     * @param allocator The allocator used for deallocating the memory used
     *                  by the counter.
     * @param errorMessages A message list, which filled in case of an error.
     * @return <code>true</code> if the execution succeeds, <code>false</code>
     *         if it fails. In this case the message list is filled.
     */    
    virtual LOADER_Bool  destroyCounter(CounterHandle counterHandle,
                                        SAPDBMem_IRawAllocator& allocator,
                                        SQLDBC_IRuntime::Error& error)
    {
        return m_SQLDBCRuntime->destroyCounter(counterHandle, allocator, error);
    }
           
    /**
     * Returns the ID of the current task. If it is single threaded, the 
     * implementation needs not distinguish between threads
     *  and can return a constant value.
     * @return The task ID of the current task.
     */
    virtual TaskID getCurrentTaskID()
    {
        return m_SQLDBCRuntime->getCurrentTaskID();
    }
                                      
    /**
     * Creates a semaphore.
     * @param semaphoreHandle Handle to the semaphore that is created.
     * @param initialValue The initial value of the counter.
     * @param allocator The allocator used to allocate memory to the semaphore.
     * @param errorMessages A message list, which is filled in case of an error.
     * @return <code>true</code> if the execution succeeds, <code>false</code>
     *         if it fails. In this case the message list is filled.
     */
    virtual LOADER_Bool createSemaphore(SemaphoreHandle& semaphoreHandle,
                                        LOADER_Int4 initialValue,
                                        SAPDBMem_IRawAllocator& allocator,
                                        SQLDBC_IRuntime::Error& errorMessages)
    {
        // prevent null pointer
        semaphoreHandle = (void *) this;
        // noop in Loader
        return LOADER_TRUE;
    }


    /**
     * Waits for the semaphore, decreases its value by one and
     * waits if the value of the semaphore is less or equal zero.
     * @param semaphoreHandle The handle to the semaphore.
     * @return <code>true</code> if the execution succeeds, <code>false</code>
     *         if it fails.
     */
    virtual LOADER_Bool waitSemaphore(SemaphoreHandle semaphoreHandle)
    {
        // noop in Loader
        return LOADER_TRUE;
    }


    /**
     * Release a semaphore, increases its value by one.
     * @param semaphoreHandle The handle to the semaphore.
     * @return <code>true</code> if the execution succeeds, <code>false</code>
     *         if it fails.
     */
    virtual LOADER_Bool signalSemaphore(SemaphoreHandle semaphoreHandle)
    {
        // noop in Loader
        return LOADER_TRUE;
    }

    
    /**
     * Destroys a semaphore.
     * @param semaphoreHandle The handle to the semaphore that is destroyed.
     * @param allocator The allocator used for de-allocating the memory used
     *                  by the semaphore.
     * @param errorMessages A message list, which is filled in case of an error.
     * @return <code>true</code> if the execution succeeds, <code>false</code>
     *         if it fails. In this case the message list is filled.
     */    
    virtual LOADER_Bool destroySemaphore(SemaphoreHandle& semaphoreHandle,
                                         SAPDBMem_IRawAllocator& allocator,
                                         SQLDBC_IRuntime::Error& errorMessages)
    {
        // noop in Loader
        return LOADER_TRUE;
    }

    
    // -- Tracing 
    
    /**
     * Requests the task trace context. Must return for each task a unique pointer to the data
     * structure of the type TaskTraceContext.
     * @return For each task a unique pointer to the data structure of the 
     * type TaskTraceContext, or <code>0</code> if this runtime
     * implementation does not support tracing.
     */
    virtual TaskTraceContext* getTaskTraceContext()
    {
        return m_SQLDBCRuntime->getTaskTraceContext();
    }
    
    /**
     * Writes in the trace. 
     * @param s The string to be printed, in UTF8 code.
     * @param size The size of the string or -1 if the size of the string
     *             is to be determined using <code>strlen</code>.
     */
	virtual void write(const char *s, LOADER_Int4 size)
    {
        return m_SQLDBCRuntime->write(s, size);
    }
    
    /**
     * Writes in the trace an adds a new line.
     * @param s The string to be printed, in UTF8 code.
     * @param size The size of the string or -1 if the size of the string
     *             is to be determined using <code>strlen</code>.
     */
    virtual void writeln(const char *s, LOADER_Int4 size)
    {
        return m_SQLDBCRuntime->writeln(s, size);
    }
    
    /**
     * Updates the internal trace flags, so that changes in trace settings are
     * detected. This method is called by the interface runtime from time to 
     * check whether updates are available. If the update fails, there shall
     * be no error reported.
     * @param traceEnabled Flag, which is is set to  @c true if any trace is
     *     enabled, and set to @c false if there is no trace that can be 
     *     enabled.
     */ 
    virtual void updateTraceFlags(LOADER_Bool& traceEnabled)
    {
        m_SQLDBCRuntime->updateTraceFlags(traceEnabled);
    }

    /**
     * Called when an error is traced. The runtime may count
     * or disable tracing as result of this method call.
     * @param errorcode The error code that has happened.
     * @return @c true if some trace was switched off.
     */
//    virtual LOADER_Bool traceError(LOADER_Int4 errorcode) { return false; }


    /**
     * Set trace flags programatically.
     * @param flags Bitmask that is added to the current setting.
     */
    virtual void addTraceFlags(LOADER_UInt4 flags)
    {
        return m_SQLDBCRuntime->addTraceFlags(flags);
    }

    /**
     * Set trace flags programatically.
     * @param flags Bitmask that is removed from the current setting.
     */   
    virtual void removeTraceFlags(LOADER_UInt4 flags)
    {
        return m_SQLDBCRuntime->removeTraceFlags(flags);
    }


#ifdef LOADER_TRACING
    void setLoaderTraceLevel(LOADER_TraceLevel level);
#endif


    //LOADER_Bool IsTraceEnabled(SQLDBC_TraceLevel level)
    LOADER_Bool IsTraceEnabled()
    {
        return m_TraceIsOn;
/*        if (NULL != this->m_SQLDBCRuntime)
        {
            char flags[512];
            (static_cast<SQLDBC_SingleThreadedRuntime*>(m_SQLDBCRuntime))->getFlags(flags);
            return (flags & static_cast<LOADER_Char>(level));
        }*/
    }

public:
#ifdef LOADER_TRACING
    SQLDBC_ClientTrace*             m_pClientTrace;
#endif

    
private:
    SQLDBC_IRuntime*                m_SQLDBCRuntime;

    LOADER_Int8                     m_SessionID;
    void*                           m_PacketList[2];       // we need always 2 packet pointer
    LOADER_Int4                     m_PacketCount;
    LOADER_UInt4                    m_PacketSize;

    LOADER_UInt4                    m_PageSize;
    LOADER_DateTimeFormat           m_DateTimeFormat;
    LOADER_Char                     m_szUserGroupDefault[LOADER_KNL_ID_SIZE + sizeof(char)];
    LOADER_Char                     m_szSYSDBADefault[LOADER_KNL_ID_SIZE + sizeof(char)];


    LOADER_Bool                     m_TraceIsOn;

    Loader_ClientRuntime_SpinLock   m_dummySpinlock;
};

#endif //LOADER_CLIENTRUNTIME_HPP