/*!
  @file           SQLDBC_IRuntime.h
  @author         D039759
  @ingroup        SQLDBC
  @brief          Interface to the runtime environment.
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
#ifndef SQLDBC_IRUNTIME_H
#define SQLDBC_IRUNTIME_H

#ifdef SQLDBC_FOR_KERNEL
#  define IFR_NAMESPACE SQLDBC_Kernel
#  define SQLDBC SQLDBC_Kernel
#  define SQLDBC_RUNTIME_IN_NAMESPACE
#endif

// For encoding
#include "Interfaces/SQLDBC/SQLDBC_Types.h"
#include "SAPDBCommon/SAPDB_Types.h"
#include "SAPDBCommon/SAPDB_ReuseLib.h"
#include "SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDB/Interfaces/SQLDBC/SQLDBC_Internals.h"

#ifdef IFR_NAMESPACE
namespace IFR_NAMESPACE {
class IFR_Environment;
};
using namespace IFR_NAMESPACE;
#else
class IFR_Environment;
#endif

namespace SQLDBC {
    class SQLDBC_Connection;
}
using namespace SQLDBC;

#ifdef SQLDBC_RUNTIME_IN_NAMESPACE
namespace SQLDBC {
#endif

/** 
 * SQLDBC_IRuntime is the interface to the runtime environment abstraction. 
 * 
 * The applications (for example client applications or the database kernel) 
 * may use different environments.. The SQLDBC_IRuntime encapsulates all 
 * calls related to the runtime environment like synchronization, tracing, 
 * and <b>global</b> memory allocation.
 * 
 * The implemented client runtime environment is the default for client 
 * applications.
 *
 * If another runtime implementation is required , it must be set when the 
 * SQLDBC_Environment is initialized.
 *
 * Example:
 *
 * <pre>
 * MyRuntime runtime;
 * SQLDBC_Environment *env=
 *   SQLDBC_Environment::createEnvironment(&runtime);
 * </pre>
 * 
 * For creating a new runtime environment, implement the interface. 
 * It must be completely initialized when the runtime environment is passed
 * on as argument to the SQLDBC environment. Do not destroy the runtime 
 * environment object before the SQLDBC environment has been destroyed.
 *
 * Decide whether the  runtime object shall be 'default constructible', 
 * 'copy constructible', or 'assignable'. The interface runtime guarantees 
 * only that no other instances are created by the runtime itself using copy 
 * and/or assignment. only those instances are used that are handed out to an 
 * environment, and no other instances are created by the runtime itself 
 * through copy and/or assignment.
 *
 * The interfaces makes also no statement about the relation between the 
 * runtime, environments, and connections. With the client runtime, there is 
 * a relation <code>1:n:m</code> between these, but if you guarantee by your
 * application for example an <code>1:1:1</code> relation you may be able to 
 * omit a lot of functionality, or supply dummy implementations.
 *
 * The same applies to multi-threading - if it is not needed you may 
 * supply dummy placeholder implementations for the lock operations.
 */
class SQLDBC_IRuntime
{
public:

    /**
     * Error handle that is used to report errors 
     * from methods.
     */
    struct Error
    {
        inline Error()
        :errorcode(0)
        {
            errortext[0]='\0';
        }

        inline operator SAPDB_Bool () const { return errorcode != 0; }

        SAPDB_Int4 errorcode;     //!< Error code.
        char       errortext[80]; //!< Error text (Nullterminated ASCII string).
    };
    
    /**
     * Type definition for a mutex.
     */
    typedef void *MutexHandle;

    /**
     * Type definition for a semaphore.
     */
    typedef void *SemaphoreHandle;

    /**
     * Type definition for a counter handle.
     */
    typedef void *CounterHandle;

    /**
     * A structure used by the tracing mechanism.
     */ 
    struct TaskTraceContext
    {
        void           *currentEntry;   //!< Current stack entry.
        // -- further settings, used by the trace stream abstraction.
        SAPDB_Bool  hex;               //!< True if hexadecimal number or a dump are output
        SAPDB_UInt4 inputlength;       //!< Input length for character strings.
        SQLDBC_StringEncoding encoding;//!< Input code.
        SAPDB_UInt4 indent;            //!< Line indent
        SAPDB_UInt4 flags;             //!< A combination of flags which defines the running trace.
        SAPDB_UInt4 traceableLimit;    //!< Size limit for 'traceable' instances (packets, parts).
        SAPDB_UInt4 pos;               //!< Internal flag.
    };
        
    /**
     * Type definition for a task ID.
     */
    typedef SAPDB_UInt4 TaskID;

    /**
     * Destructor.
     */
    virtual ~SQLDBC_IRuntime() {}

    /**
     * Returns an implementation identifier for diagnostic purposes.
     * @return An implementation identifier to be used 
     *         for diagnostic purposes.
     */
    virtual const char *getIdentifier() const { return ""; }

    /**
     * Analyzes a key option. This means an XUSER record or 
     * the @c SQLOPT environment variable is evaluated.
     *
     * @param keyoption The KEY option value.
     * @param servernode The name of the database server (ASCII)
     * @param servernodelength The length in bytes of the database server name.
     * @param serverdb The name of the database instance (ASCII).
     * @param serverdblength The length in bytes of the database instance name.
     * @param isolationlevel The isolation level.
     * @param cachelimit The @c CACHELIMIT value.
     * @param sqlmode The SQL mode.
     * @param timeout The @c TIMEOUT value.
     * @param username The name of the database user. 
     * The encoding of this string depends on the <var>usernameEncoding</var>
     * argument.
     * @param usernamelength The length in bytes of the database user name.
     * @param usernameEncoding The character encoding of the <var>username</var>.
     * @param error A error object, which filled in case of an error.
     * @return <code>true</code> if the call succeeds, <code>false</code>
     *         if it fails, or there was nothing to do. In case of a failure, the error object is filled. 
     *         If the call succeeds, @c servernode, @c servernodelength, @c serverdb,
     *         @c serverdblength, @c isolationlevel, @c isolationlevel, @c cachelimit,
     *         @c sqlmode, @c timeout, @c username, and @c usernamelength are filled
     *         with the respective data extracted from the URL.
     */
    virtual SAPDB_Bool checkKeyOption(const char *keyoption,
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
        return false;
    }
                                  

    
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
    virtual SAPDBMem_IRawAllocator& getGlobalAllocator() = 0;

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
     * @param error A error object, which filled in case of an error.
     * @param allocator The allocator for allocating memory.
     * @return <code>true</code> if the session is established, <code>false</code>
     *         if it fails. In this case the <var>errorMessages</var> are filled.
     */
    virtual SAPDB_Bool getSession(const char *connectUrl,
                                  const char *connectCommand,
                                  const char *password,
                                  SAPDB_Int4  passwordLength,
                                  const SQLDBC_StringEncoding commandEncoding,
                                  SAPDB_Int8&  sessionID,
                                  SAPDB_UInt4& packetSize,
                                  SAPDB_Int4& packetListSize,
                                  void **packetList,  
                                  void **sessionInfoReply,
                                  SQLDBC_IRuntime::Error& error,
                                  SAPDBMem_IRawAllocator* allocator = 0) = 0;


    virtual SAPDB_Bool getSession(const char *connectUrl,
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
                                  SQLDBC_IRuntime::Error& error,
                                  SAPDBMem_IRawAllocator* allocator = 0) = 0;

    /**
     * Closes a session previously fetched via getSession().
     * @param sessionID The session id previously acquired using 
     *   <code>getSession()</code>.
     * @param error An error object to be filled in case of error.
     * @return <code>true</code> if the execution succeeds, <code>false</code>
     *         if it fails. In this case the error object is filled.
     */
    virtual SAPDB_Bool releaseSession(SAPDB_Int8 sessionID,
                                      SQLDBC_IRuntime::Error& error) = 0;
    
    /**
     * Performs a request to the database kernel. 
     * @param sessionID The session ID previously fetched using getSession().
     * @param requestData Pointer to the request data, usually an element of 
     * the packet list fetched using <code>getSession()</code>.
     * @param requestDataLength The number of bytes to be sent.
     * @param error An error object to be filled in case of error.
     * @return <code>true</code> if the execution succeeds, <code>false</code>
     *         if it fails. In this case the error object is filled.
     */
    virtual SAPDB_Bool request(SAPDB_Int8  sessionID,
                               void       *requestData,
                               SAPDB_UInt4 requestDataLength,
                               SQLDBC_IRuntime::Error& error) = 0; 

    /**
     * Retrieves data from the database kernel. 
     * @param sessionID The session ID previously fetched using getSession().
     * @param replyData Pointer to the pointer to the reply.
     * @param replyDataLength Length of the reply returnes.
     * @param error A error object, which filled in case of an error.
     * @return <code>true</code> if the execution succeeds, <code>false</code>
     *         if it fails. In this case the error object is filled.
     */
    virtual SAPDB_Bool receive(SAPDB_Int8 sessionID,
                               void **replyData,
                               SAPDB_Int4& replyDataLength,
                               SQLDBC_IRuntime::Error& error) = 0;

    /**
     * Checks whether there is reply available for the last command.
     * @param sessionID The session id of the session to query.
     */
    virtual SAPDB_Bool isReplyAvailable(SAPDB_Int8 sessionID)
    {
        return true;
    }
    
    /**
     * Cancels the current SQL statment.
     * @param sessionID The session ID previously fetched using getSession().
     * @param error An error object to be filled in case of error.
     * @return <code>true</code> if the execution succeeds, <code>false</code>
     *         if it fails. In this case the error object is filled.
     */
    virtual SAPDB_Bool cancelCurrentCommand(SAPDB_Int8 sessionID,
                                            SQLDBC_IRuntime::Error& error) = 0;

    // -- synchronization primitives
   /**
     * Creates a Spinlock object.
     * @param allocator The allocator used allocate the spinlock handle.
     * @return <code>SQLDBC_Spinlock</code> if the execution succeeds, <code>0</code>
     *         if it fails. In this case the error object is filled.
     */    
    virtual SQLDBC_Spinlock* createSpinlock(SAPDBMem_IRawAllocator& allocator)  = 0;

    /**
     * Unlocks a Spinlock object.
     * @param allocator The allocator used allocate the spinlock handle.
     * @param lock The handle to the Spinlock object that shall be unlocked.
     * @return <code>true</code> if the Spinlock object was unlocked successfully,
     * <code>false</code> if it fails. 
     */
    virtual SAPDB_Bool releaseSpinlock(SAPDBMem_IRawAllocator& allocator, SQLDBC_Spinlock* lock) = 0;

    /**
     * Creates a mutex.
     * @param mutexHandle The handle to the mutex, that is set 
     *   on return.
     * @param allocator The allocator used allocate the mutex handle.
     * @param error An error object to be filled in case of error.
     * @return <code>true</code> if the execution succeeds, <code>false</code>
     *         if it fails. In this case the error object is filled.
     */    
    virtual SAPDB_Bool createMutex(MutexHandle& mutexHandle, 
                                   SAPDBMem_IRawAllocator& allocator,
                                   SQLDBC_IRuntime::Error& error)  = 0;

    /**
     * Locks a mutex.
     * @param mutexHandle The handle to the mutex.
     * @return <code>true</code> if the method succeeds,
     * <code>false</code>  if it fails.
     */
    virtual SAPDB_Bool lockMutex(MutexHandle mutexHandle) = 0;
    
    /**
     * Unlocks a mutex.
     * @param mutexHandle The handle to the mutex that shall be unlocked.
     * @return <code>true</code> if the mutex was unlocked successfully,
     * <code>false</code> if it fails. 
     */
    virtual SAPDB_Bool releaseMutex(MutexHandle mutexHandle) = 0;
    
    /**
     * Destroys a mutex. Write a zero in the handle.
     * @param mutexHandle The handle to the mutex.
     * @param allocator The allocator used for de-allocating the memory used by the mutex.
     * @param error An error object to be filled in case of error.
     * @return <code>true</code> if the execution succeeds, <code>false</code>
     *         if it fails. In this case the error object is filled.
     */    
    virtual SAPDB_Bool destroyMutex(MutexHandle& mutexHandle, 
                                    SAPDBMem_IRawAllocator& allocator,
                                    SQLDBC_IRuntime::Error& error) = 0;
    
    /**
     * Creates an atomic counter.
     * @param counterHandle The handle to the counter that is created.
     * @param allocator The allocator used to allocate memory to the counter handle.
     * @param error An error object to be filled in case of error.
     * @return <code>true</code> if the execution succeeds, <code>false</code>
     *         if it fails. In this case the error object is filled.
     */    
    virtual SAPDB_Bool  createCounter(CounterHandle& counterHandle,
                                      SAPDBMem_IRawAllocator& allocator,
                                      SQLDBC_IRuntime::Error& error)  = 0;

    
    /** 
     * Increments the counter.
     * @param counterHandle The handle to the counter be incremented.
     * @return The new value of the counter.
     */
    virtual SAPDB_UInt4 nextCounter(CounterHandle counterHandle) = 0;
    
    /**
     * Destroys an atomic counter.
     * @param counterHandle The counter to be destroyed.
     * @param allocator The allocator used for deallocating the memory used
     *                  by the counter.
     * @param error An error object to be filled in case of error.
     * @return <code>true</code> if the execution succeeds, <code>false</code>
     *         if it fails. In this case the error object is filled.
     */    
    virtual SAPDB_Bool  destroyCounter(CounterHandle counterHandle,
                                      SAPDBMem_IRawAllocator& allocator,
                                      SQLDBC_IRuntime::Error& error)  = 0;
           
    /**
     * Returns the ID of the current task. If it is single threaded, the 
     * implementation needs not distinguish between threads
     *  and can return a constant value.
     * @return The task ID of the current task.
     */
    virtual TaskID getCurrentTaskID() = 0;                                      
                                      
    /**
     * Creates a semaphore.
     * @param semaphoreHandle Handle to the semaphore that is created.
     * @param initialValue The initial value of the counter.
     * @param allocator The allocator used to allocate memory to the semaphore.
     * @param error An error object to be filled in case of error.
     * @return <code>true</code> if the execution succeeds, <code>false</code>
     *         if it fails. In this case the error object is filled.
     */
    virtual SAPDB_Bool createSemaphore(SemaphoreHandle& semaphoreHandle,
                                       SAPDB_Int4 initialValue,
                                       SAPDBMem_IRawAllocator& allocator,
                                       SQLDBC_IRuntime::Error& error) = 0;

    /**
     * Waits for the semaphore, decreases its value by one and
     * waits if the value of the semaphore is less or equal zero.
     * @param semaphoreHandle The handle to the semaphore.
     * @return <code>true</code> if the execution succeeds, <code>false</code>
     *         if it fails.
     */
    virtual SAPDB_Bool waitSemaphore(SemaphoreHandle semaphoreHandle) =0;

    /**
     * Release a semaphore, increases its value by one.
     * @param semaphoreHandle The handle to the semaphore.
     * @return <code>true</code> if the execution succeeds, <code>false</code>
     *         if it fails.
     */
    virtual SAPDB_Bool signalSemaphore(SemaphoreHandle semaphoreHandle) =0;
    
    /**
     * Destroys a semaphore.
     * @param semaphoreHandle The handle to the semaphore that is destroyed.
     * @param allocator The allocator used for de-allocating the memory used
     *                  by the semaphore.
     * @param error An error object to be filled in case of error.
     * @return <code>true</code> if the execution succeeds, <code>false</code>
     *         if it fails. In this case the error object is filled.
     */    
    virtual SAPDB_Bool destroySemaphore(SemaphoreHandle& semaphoreHandle,
                                        SAPDBMem_IRawAllocator& allocator,
                                        SQLDBC_IRuntime::Error& error)  = 0;
    
    // -- Tracing    
    /**
     * Requests the task trace context. Must return for each task a unique pointer to the data
     * structure of the type TaskTraceContext.
     * @return For each task a unique pointer to the data structure of the 
     * type TaskTraceContext, or <code>0</code> if this runtime
     * implementation does not support tracing.
     */
    virtual TaskTraceContext* getTaskTraceContext() = 0;
    
    /**
     * Writes in the trace. 
     * @param s The string to be printed, in UTF8 code.
     * @param size The size of the string or -1 if the size of the string
     *             is to be determined using <code>strlen</code>.
     */
    virtual void write(const char *s, SAPDB_Int4 size) = 0;
    
    /**
     * Writes in the trace an adds a new line.
     * @param s The string to be printed, in UTF8 code.
     * @param size The size of the string or -1 if the size of the string
     *             is to be determined using <code>strlen</code>.
     */
    virtual void writeln(const char *s, SAPDB_Int4 size) = 0;
    
    /**
     * Updates the internal trace flags, so that changes in trace settings are
     * detected. This method is called by the interface runtime from time to 
     * check whether updates are available. If the update fails, there shall
     * be no error reported.
     * @param traceEnabled Flag, which is is set to  @c true if any trace is
     *     enabled, and set to @c false if there is no trace that can be 
     *     enabled.
     */ 
    virtual void updateTraceFlags(SAPDB_Bool& traceEnabled) {
        TaskTraceContext *ctx = getTaskTraceContext();
        if(ctx) {
            if(ctx->flags) {
                traceEnabled = true;
                return;
            }
        } 
        traceEnabled = false;
        return;
    }

    /**
     * Called when an error is traced. The runtime may count
     * or disable tracing as result of this method call.
     * @param errorcode The error code that has happened.
     * @return @c true if some trace was switched off.
     */
    virtual SAPDB_Bool traceError(SAPDB_Int4 errorcode) { return false; }

    //-- internally for client runtime used, no use in Kernel/OMS/...
    
    virtual void setTraceOptions(const char *optionstring) 
    { 
        return; 
    }
    
    virtual SAPDB_Int4 getTraceOptions(char *options, SAPDB_Int4 optionslength) 
    { 
        if(options && optionslength>0) {
            *options = '\0';
        }
        return 0; 
    }

    /**
     * Set trace flags programatically.
     * @param flags Bitmask that is added to the current setting.
     */
    virtual void addTraceFlags(unsigned int flags) = 0;

    /**
     * Set trace flags programatically.
     * @param flags Bitmask that is removed from the current setting.
     */   
    virtual void removeTraceFlags(unsigned int flags) = 0;


    /**
     * Retrieve specific internally needed system information.
     */
    virtual SAPDB_Bool getSessionInformation(SQLDBC_Connection *connection,
                                             SAPDB_Int8& sessionID,
                                             SAPDB_UInt4& packetSize,
                                             SAPDB_UInt4& pagesSize,
                                             SAPDB_Int4&  packetListSize,
                                             void **packetList,
                                             char *usergroup,
                                             char *sysdba,
                                             SQLDBC_StringEncoding& userencoding)
    {
        return false;
    }
    

protected:
    struct EnvironmentList
    {
        EnvironmentList   *next;
        EnvironmentList   *prev;
        SQLDBC_Spinlock *spinlock;
    };
    
    struct Profile
    {
        SQLDBC_UInt8      counter[SQLDBC_PROFILE_MAX];
        void             *environmentlist;  // SUN cannot use pointer to EnvironmentList list here 
                                            // (compiler bug)
    };

    SQLDBC_IRuntime (SQLDBC_Spinlock* lock)
    {
        m_environmentlist.next = &m_environmentlist;
        m_environmentlist.prev = &m_environmentlist;
        m_environmentlist.spinlock = lock;
        
        memset(m_profile.counter, 0, sizeof(m_profile.counter));
        m_profile.environmentlist = &m_environmentlist;
    }
    
    EnvironmentList m_environmentlist;
    Profile         m_profile;
    
private:
    friend class IFR_Environment;
    friend class IFR_ParseInfoCacheImpl;
    friend class IFR_ParseInfoCache;
};

#ifdef SQLDBC_RUNTIME_IN_NAMESPACE
};
#endif

#endif
   
