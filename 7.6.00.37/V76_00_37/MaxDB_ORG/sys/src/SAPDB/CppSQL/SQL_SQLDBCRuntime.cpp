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


#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "CppSQL/SQL_SQLDBCRuntime.hpp"
#include "liveCache/LVC_IliveCacheSink.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"

static const int c_traceBufferSize = 132;

#define DUMMYTRACE_METHOD_DEBUG(X,Y,Z)


//--------------------------------------------------------------------------

SQL_SQLDBCRuntime::SQL_SQLDBCRuntime(IliveCacheSink& kernelSink, SAPDBMem_IRawAllocator& allocator, int packetSize)
: SQLDBC_IRuntime(&m_dummySpinlock)
, m_kernelSink(kernelSink)
, m_allocator(allocator)
, m_pTraceBuffer(0)
, m_traceBufferLength(0)
, m_pPacket(0)
, m_packetSize(packetSize)
, m_traceLevel(0)
{
    DUMMYTRACE_METHOD_DEBUG ("SQL_SQLDBCRuntime::SQL_SQLDBCRuntime", KernelSQL_Trace, 1);
    
    short error;
    m_kernelSink.IsUnicodeInstance(m_unicodeinstance, error);
    
    if (packetSize < 4096)
    {
        packetSize = 4096;
    }
    this->InitTraceContext();
}

//--------------------------------------------------------------------------

SQL_SQLDBCRuntime::~SQL_SQLDBCRuntime()
{
    DUMMYTRACE_METHOD_DEBUG ("SQL_SQLDBCRuntime::~SQL_SQLDBCRuntime", KernelSQL_Trace, 1);
    m_allocator.Deallocate(m_pPacket);
    m_pPacket = 0;
    m_allocator.Deallocate(m_pTraceBuffer);
    m_pTraceBuffer = 0;
}

//--------------------------------------------------------------------------

SAPDBMem_IRawAllocator& SQL_SQLDBCRuntime::getGlobalAllocator()
{
    DUMMYTRACE_METHOD_DEBUG ("SQL_SQLDBCRuntime::getGlobalAllocator", KernelSQL_Trace, 1);
    return m_allocator;
}

//--------------------------------------------------------------------------
SAPDB_Bool SQL_SQLDBCRuntime::getSession(const char *connectUrl,
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
                                    SAPDBMem_IRawAllocator* allocator)
{
    return getSession(connectUrl,
                      connectCommand,
                      password,
                      passwordLength,
                      commandEncoding,
                      sessionID,
                      packetSize,
                      packetListSize,
                      packetList,
                      sessionInfoReply,
                      error,
                      allocator);
}

SAPDB_Bool SQL_SQLDBCRuntime::getSession(const char *connectUrl,
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
                                    SAPDBMem_IRawAllocator* allocator)
{
    DUMMYTRACE_METHOD_DEBUG ("SQL_SQLDBCRuntime::getSession", KernelSQL_Trace, 1);
    packetSize     = m_packetSize;
    packetListSize = 1;
    m_pPacket      = m_allocator.Allocate(packetSize);
    *packetList    = m_pPacket; 
    if (0 != *packetList)
    {
        sessionID = 1; /* must be greater 0, but not unique */
        tsp00_Int4  bufLength = sizeof(tsp00_Int4);
        short       e;
        tsp00_Int4* pI4 = reinterpret_cast<tsp00_Int4*>(m_pPacket);
        *pI4 = m_packetSize;
        m_kernelSink.MultiPurpose (m_connect, mm_nil, 
            &bufLength, reinterpret_cast<unsigned char*>(m_pPacket), &e);
        if (0 == e)
        {
            *sessionInfoReply = *packetList;
            return true;
        }
    }
    return false;
}

//--------------------------------------------------------------------------

SAPDB_Bool SQL_SQLDBCRuntime::releaseSession(SAPDB_Int8 sessionID,
                                        SQLDBC_IRuntime::Error& error)
{
    DUMMYTRACE_METHOD_DEBUG ("SQL_SQLDBCRuntime::releaseSession", KernelSQL_Trace, 1);
    if (0 != m_pPacket)
    {
        m_allocator.Deallocate(m_pPacket);
        m_pPacket = 0;
    }
    return true;
}

//--------------------------------------------------------------------------

SAPDB_Bool SQL_SQLDBCRuntime::request(SAPDB_Int8  sessionID,
                                 void       *requestData,
                                 SAPDB_UInt4 requestDataLength,
                                 SQLDBC_IRuntime::Error& error)
{
    DUMMYTRACE_METHOD_DEBUG ("SQL_SQLDBCRuntime::request", KernelSQL_Trace, 1);
    m_kernelSink.SQLRequest(requestDataLength, reinterpret_cast<unsigned char*>(requestData)); 
    return true;
}

//--------------------------------------------------------------------------

SAPDB_Bool SQL_SQLDBCRuntime::receive(SAPDB_Int8 sessionID,
                                 void **replyData,
                                 SAPDB_Int4& replyDataLength,
                                 SQLDBC_IRuntime::Error& error)
{
    DUMMYTRACE_METHOD_DEBUG ("SQL_SQLDBCRuntime::receive", KernelSQL_Trace, 1);
    *replyData      = m_pPacket;
    replyDataLength = m_packetSize;
    return true;
}

//--------------------------------------------------------------------------

SAPDB_Bool SQL_SQLDBCRuntime::cancelCurrentCommand(SAPDB_Int8 sessionID,
                                              SQLDBC_IRuntime::Error& error)
{
    // noop in liboms
    return false;
}

//--------------------------------------------------------------------------

SAPDB_Bool SQL_SQLDBCRuntime::createMutex(MutexHandle& mutexHandle, 
                                     SAPDBMem_IRawAllocator& allocator,
                                     SQLDBC_IRuntime::Error& error)
{
    // prevent null pointer
    mutexHandle = (void *) this;
    // noop in liboms
    return true;
}

//--------------------------------------------------------------------------

SAPDB_Bool SQL_SQLDBCRuntime::lockMutex(MutexHandle mutexHandle)
{
    // noop in liboms
    return true;
}

//--------------------------------------------------------------------------

SAPDB_Bool SQL_SQLDBCRuntime::releaseMutex(MutexHandle mutexHandle)
{
    // noop in liboms
    return true;
}

//--------------------------------------------------------------------------

SAPDB_Bool SQL_SQLDBCRuntime::destroyMutex(MutexHandle& mutexHandle, 
                                      SAPDBMem_IRawAllocator& allocator,
                                      SQLDBC_IRuntime::Error& error)
{
    // noop in liboms
    return true;
}

//--------------------------------------------------------------------------

SAPDB_Bool  SQL_SQLDBCRuntime::createCounter(CounterHandle& counterHandle,
                                        SAPDBMem_IRawAllocator& allocator,
                                        SQLDBC_IRuntime::Error& error)
{
    SAPDB_UInt4* pI4 = new(allocator) SAPDB_UInt4;
    if (pI4)
    {
        *pI4 = 0;
        counterHandle = pI4;
    }
    return (counterHandle != 0);
}

//--------------------------------------------------------------------------

SAPDB_UInt4 SQL_SQLDBCRuntime::nextCounter(CounterHandle counterHandle)
{
    SAPDB_UInt4* pI4 = REINTERPRET_CAST(SAPDB_UInt4*, counterHandle);
    ++(*pI4);
    return *pI4;
}

//--------------------------------------------------------------------------

SAPDB_Bool  SQL_SQLDBCRuntime::destroyCounter(CounterHandle counterHandle,
                                         SAPDBMem_IRawAllocator& allocator,
                                         SQLDBC_IRuntime::Error& error)
{
    allocator.Deallocate(counterHandle);
    return true;
}

//--------------------------------------------------------------------------

SQLDBC_IRuntime::TaskID SQL_SQLDBCRuntime::getCurrentTaskID()
{
    tsp00_TaskId taskId;
    void*        dummySession;
    pasbool*     dummyToCancel;
    bool         dummyOptimizedStreamCommunication; // PTS 1130281
    m_kernelSink.GetDefaultContext(REINTERPRET_CAST(void**, &dummySession),&dummyToCancel, dummyOptimizedStreamCommunication, taskId);
    return taskId;
}

//--------------------------------------------------------------------------

SAPDB_Bool SQL_SQLDBCRuntime::createSemaphore(SemaphoreHandle& semaphoreHandle,
                                         SAPDB_Int4 initialValue,
                                         SAPDBMem_IRawAllocator& allocator,
                                         SQLDBC_IRuntime::Error& error)
{
    // prevent null pointer
    semaphoreHandle = (void *) this;
    // noop in liboms
    return true;
}

//--------------------------------------------------------------------------

SAPDB_Bool SQL_SQLDBCRuntime::waitSemaphore(SemaphoreHandle semaphoreHandle)
{
    // noop in liboms
    return true;
}

//--------------------------------------------------------------------------

SAPDB_Bool SQL_SQLDBCRuntime::signalSemaphore(SemaphoreHandle semaphoreHandle)
{
     // noop in liboms
    return true;
}

//--------------------------------------------------------------------------

SAPDB_Bool SQL_SQLDBCRuntime::destroySemaphore(SemaphoreHandle& semaphoreHandle,
                                          SAPDBMem_IRawAllocator& allocator,
                                          SQLDBC_IRuntime::Error& error)
{
     // noop in liboms
    return true;
}

//--------------------------------------------------------------------------

SQLDBC_IRuntime::TaskTraceContext* SQL_SQLDBCRuntime::getTaskTraceContext()
{
   return &m_interfaceTaskTraceContext;
}

//--------------------------------------------------------------------------

void SQL_SQLDBCRuntime::write(const char *s, SAPDB_Int4 size)
{
    if (0 == m_pTraceBuffer)
    {
        m_pTraceBuffer = (char*) m_allocator.Allocate(c_traceBufferSize);
        if (0 == m_pTraceBuffer)
        {
            return;
        }
    }
    if (0 == size)
    {
        return;
    }
    if (size < 0)
    {
        size = strlen(s);
    }
    if (m_traceBufferLength + size >= c_traceBufferSize)
    {
        this->FlushTrace();       
    }
    if (size >= c_traceBufferSize)
    {
        size = c_traceBufferSize - 1;
    }
    SAPDB_MemCopyNoCheck(&m_pTraceBuffer[m_traceBufferLength], s, size);
    m_traceBufferLength += size;
}

//--------------------------------------------------------------------------

void SQL_SQLDBCRuntime::writeln(const char *s, SAPDB_Int4 size)
{
    this->write (s, size);
    this->FlushTrace();      
}

//--------------------------------------------------------------------------
void
SQL_SQLDBCRuntime::updateTraceFlags(SAPDB_Bool& traceEnabled) 
{
// http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1139282
// Leave va
//     if(m_interfaceTaskTraceContext.flags) {
//         traceEnabled = true;
//     } else {
//         traceEnabled = false;
//     }
}

//--------------------------------------------------------------------------
void
SQL_SQLDBCRuntime::addTraceFlags(unsigned int flags)
{
    m_interfaceTaskTraceContext.flags |= flags;
    return;
}

//--------------------------------------------------------------------------
void
SQL_SQLDBCRuntime::removeTraceFlags(unsigned int flags)
{
    unsigned int mask = 0xFFFFFFFF - flags;
    m_interfaceTaskTraceContext.flags &= mask;
    return;
}


//--------------------------------------------------------------------------

void SQL_SQLDBCRuntime::FlushTrace()
{
    m_kernelSink.Vtrace(m_traceBufferLength, reinterpret_cast<unsigned char*>(m_pTraceBuffer)); 
    m_traceBufferLength = 0;
}

//--------------------------------------------------------------------------

void SQL_SQLDBCRuntime::SetTraceLevel(int level)
{
    enum 
    {
        SQLDBC_TRACE_SHORT         = 1,   // Calls without arguments
        SQLDBC_TRACE_LONG          = 3,   // Calls with arguments
        SQLDBC_TRACE_PACKET        = 4,   // packet content
        SQLDBC_TRACE_PROFILE       = 8,   // additional timestamp
        SQLDBC_TRACE_SQL           = 16   // just sql statements and in/out data
    }; 
    if (SQLDBC_TRACE_SHORT    == level ||
        SQLDBC_TRACE_LONG     == level ||
        SQLDBC_TRACE_PACKET   == level ||
        SQLDBC_TRACE_PROFILE  == level ||
        SQLDBC_TRACE_SQL      == level)
    {
        return; // TODO crashes this way
        m_interfaceTaskTraceContext.flags = level;
    }
}

//--------------------------------------------------------------------------

void SQL_SQLDBCRuntime::InitTraceContext()
{
    SAPDB_MemFillNoCheck (&m_interfaceTaskTraceContext, 0, sizeof(m_interfaceTaskTraceContext));
}
