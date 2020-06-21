/*!
  @file           SQL_SQLDBCRuntime.hpp
  @author         ThomasA
  @brief          Implements the interface runtime used in the liboms.

\if EMIT_LICENCE

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



\endif
*/

#ifndef SQL_SQLDBCRUNTIME_HPP
#define SQL_SQLDBCRUNTIME_HPP

#include "SAPDB/Interfaces/SQLDBC/SQLDBC_IRuntime.h"

struct  IliveCacheSink;

class SQL_SpinLock
  : public SQLDBC_Spinlock {
  public:
    SQL_SpinLock (){};
    virtual ~SQL_SpinLock() {}
  
    virtual SAPDB_Bool lock() {   
      return SQLDBC_TRUE;}
    
    virtual SAPDB_Bool unlock() {
      return SQLDBC_TRUE;}

    virtual SAPDB_Bool deleteLock() {
      return SQLDBC_TRUE;}
};

/*!
   @brief   kernel implementation of the interface runtime in the liboms environment
   @copydoc SQLDBC_IRuntime
 */

class SQL_SQLDBCRuntime : public SQLDBC_IRuntime
{
public :
    /*!
       @brief constructor
       @param kernelSink callback to the kernel
       @param allocator  allocator to be used for dynamic memory requests
       @param packetSize requied size of the sql packet

     */
    SQL_SQLDBCRuntime(IliveCacheSink& kernelSink, SAPDBMem_IRawAllocator& allocator, int packetSize);
    /*!
       @brief destructor
     */
    ~SQL_SQLDBCRuntime();

    virtual SAPDBMem_IRawAllocator& getGlobalAllocator();

    virtual SAPDB_Bool getSession(const char *connectUrl,
        const char *connectCommand,
        const char *username,
        SAPDB_Int4  usernameLength,
        const char *password,
        SAPDB_Int4  passwordLength,
        const SQLDBC_StringEncoding commandEncoding,
        SAPDB_Int8& sessionID,
        SAPDB_UInt4& packetSize,
        SAPDB_Int4& packetListSize,
        void **packetList,  
        void **sessionInfoReply,
        SQLDBC_IRuntime::Error& error,
        SAPDBMem_IRawAllocator* allocator = 0);

    virtual SAPDB_Bool getSession(const char *connectUrl,
        const char *connectCommand,
        const char *password,
        SAPDB_Int4  passwordLength,
        const SQLDBC_StringEncoding commandEncoding,
        SAPDB_Int8& sessionID,
        SAPDB_UInt4& packetSize,
        SAPDB_Int4& packetListSize,
        void **packetList,  
        void **sessionInfoReply,
        SQLDBC_IRuntime::Error& error,
        SAPDBMem_IRawAllocator* allocator = 0);

    virtual SAPDB_Bool releaseSession(SAPDB_Int8 sessionID,
        SQLDBC_IRuntime::Error& error);

    virtual SAPDB_Bool request(SAPDB_Int8  sessionID,
        void       *requestData,
        SAPDB_UInt4 requestDataLength,
        SQLDBC_IRuntime::Error& error); 

    virtual SAPDB_Bool receive(SAPDB_Int8 sessionID,
        void **replyData,
        SAPDB_Int4& replyDataLength,
        SQLDBC_IRuntime::Error& error);

    virtual SAPDB_Bool cancelCurrentCommand(SAPDB_Int8 sessionID,
        SQLDBC_IRuntime::Error& error);

    virtual SQLDBC_Spinlock* createSpinlock(SAPDBMem_IRawAllocator& allocator){
         return &m_dummySpinlock;
    }

    virtual SAPDB_Bool releaseSpinlock(SAPDBMem_IRawAllocator& allocator, SQLDBC_Spinlock* lock){
      return true;
    }

    virtual SAPDB_Bool createMutex(MutexHandle& mutexHandle, 
        SAPDBMem_IRawAllocator& allocator,
        SQLDBC_IRuntime::Error& error);

    virtual SAPDB_Bool lockMutex(MutexHandle mutexHandle);

    virtual SAPDB_Bool releaseMutex(MutexHandle mutexHandle);

    virtual SAPDB_Bool destroyMutex(MutexHandle& mutexHandle, 
        SAPDBMem_IRawAllocator& allocator,
        SQLDBC_IRuntime::Error& error);
  
    virtual SAPDB_Bool  createCounter(CounterHandle& counterHandle,
        SAPDBMem_IRawAllocator& allocator,
        SQLDBC_IRuntime::Error& error);

    virtual SAPDB_UInt4 nextCounter(CounterHandle counterHandle);

    virtual SAPDB_Bool  destroyCounter(CounterHandle counterHandle,
        SAPDBMem_IRawAllocator& allocator,
        SQLDBC_IRuntime::Error& error);

    virtual TaskID getCurrentTaskID();                                      

    virtual SAPDB_Bool createSemaphore(SemaphoreHandle& semaphoreHandle,
        SAPDB_Int4 initialValue,
        SAPDBMem_IRawAllocator& allocator,
        SQLDBC_IRuntime::Error& error);

    virtual SAPDB_Bool waitSemaphore(SemaphoreHandle semaphoreHandle);

    virtual SAPDB_Bool signalSemaphore(SemaphoreHandle semaphoreHandle);

    virtual SAPDB_Bool destroySemaphore(SemaphoreHandle& semaphoreHandle,
        SAPDBMem_IRawAllocator& allocator,
        SQLDBC_IRuntime::Error& error);

    virtual TaskTraceContext* getTaskTraceContext();
    /*!
       @brief write trace into trace buffer. If buffer becomes full, flushes into vtrace
     */
    virtual void write(const char *s, SAPDB_Int4 size);
    /*!
       @brief  write trace into trace buffer and flushes into vtrace
     */
    virtual void writeln(const char *s, SAPDB_Int4 size);

    /*!
       @brief  Check whether tracing is enabled.
     */
    virtual void updateTraceFlags(SAPDB_Bool& traceEnabled);

    /**
     * Set trace flags programatically.
     * @param flags Bitmask that is added to the current setting.
     */
    virtual void addTraceFlags(unsigned int flags);

    /**
     * Set trace flags programatically.
     * @param flags Bitmask that is removed from the current setting.
     */   
    virtual void removeTraceFlags(unsigned int flags);


    /*!
       @brief initializes the trace context
     */
    void InitTraceContext();
    /*!
       @brief enables/desables tracing
     */
    void SetTraceLevel(int level);   
    
    inline bool isUnicodeInstance() const { return m_unicodeinstance; }
private :
    void FlushTrace(); //!< flushes the trace buffer into the kernel vtrace
    IliveCacheSink&                   m_kernelSink;                  //!< reference to the kernel callback
    SAPDBMem_IRawAllocator&           m_allocator;                   //!< the allocator to be used for dynamic memory requests
    char*                             m_pTraceBuffer;                //!< pointer to a buffer used for tracing, 0 if tracing disabled
    int                               m_traceBufferLength;           //!< current used length of trace buffer
    void*                             m_pPacket;                     //!< pointer to the sql packet used by SQLDBC
    int                               m_packetSize;                  //!< size of the packet
    int                               m_traceLevel;                  //!< current trace level
    bool                              m_unicodeinstance;             //!< Current user unicode?
    SQLDBC_IRuntime::TaskTraceContext m_interfaceTaskTraceContext;   //!< trace context used by SQLDBC
    SQL_SpinLock m_dummySpinlock;
};

#endif
