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

#include "heo51.h" // vgetpid
#include "hak51.h"
#include "hak93.h"

#include "SAPDBCommon/MemoryManagement/SAPDBMem_SynchronizedRawAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "RunTime/MemoryManagement/RTEMem_BlockAllocator.hpp"
#include "SQLManager/KernelSQL/KSQL_Runtime.hpp"
#include "SQLManager/SQLMan_SQLStatementContext.hpp"
#include "SQLManager/SQLMan_TempFileCollection.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "PacketInterface/PIn_Packet.h"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp" // Routine trace macros
#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"

extern SAPDBTrace_Topic KernelSQL_Trace;   //!< kernel sql trace

static const int c_traceBufferSize = 132;

//--------------------------------------------------------------------------

KSQL_Runtime::KSQL_Runtime(SQLMan_Context& context)
: SQLDBC_IRuntime(&m_dummySpinlock)
, m_context(context)
, m_pTraceBuffer(0)
, m_traceBufferLength(0)
, m_level(0)
{
    SAPDBTRACE_METHOD_DEBUG ("KSQL_Runtime::KSQL_Runtime", KernelSQL_Trace, 1);
    this->InitTraceContext();
#if defined (SAPDB_SLOW) || defined (SAPDB_QUICK)
    for (int ix = 0; ix < maxLevel; ++ix)
    {
        m_pPacket[ix] = 0;
    }
#endif
}

//--------------------------------------------------------------------------

KSQL_Runtime::~KSQL_Runtime()
{
    SAPDBTRACE_METHOD_DEBUG ("KSQL_Runtime::~KSQL_Runtime", KernelSQL_Trace, 1);
    while (m_level > 0)
    {
        --m_level;
        m_context.GetAllocator().Deallocate(m_pPacket[m_level]);
    }
    m_context.GetAllocator().Deallocate(m_pTraceBuffer);
    m_pTraceBuffer = 0;
}

//--------------------------------------------------------------------------

SAPDBMem_IRawAllocator& KSQL_Runtime::getGlobalAllocator()
{
    SAPDBTRACE_METHOD_DEBUG ("KSQL_Runtime::getGlobalAllocator", KernelSQL_Trace, 1);
    return m_context.GetAllocator();
}

//--------------------------------------------------------------------------
SAPDB_Bool KSQL_Runtime::getSession(const char *connectUrl,
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

//----------------------------------------------------------------------
SAPDB_Bool KSQL_Runtime::getSession(const char *connectUrl,
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
    SAPDBTRACE_METHOD_DEBUG ("KSQL_Runtime::getSession", KernelSQL_Trace, 1);
    if (m_level + 1 == maxLevel)
    {
        return false;
    }
    packetSize         = 4096 * 4;
    packetListSize     = 1;
    SAPDBERR_ASSERT_STATE( 0 ==  m_pPacket[m_level]);
    m_pPacket[m_level] = m_context.GetAllocator().Allocate(packetSize);
    *packetList = m_pPacket[m_level];
    if (0 != *packetList)
    {
        ++m_level;
        sessionID = 1; /* must be greater 0, but not unique */
        SAPDB_Int4 cmdId = m_context.a_command_id;
        {
            m_context.a_command_id = m_context.a_command_sequence + 1;
            SQLMan_SQLStatementContext newStatementScope(m_context);
            tsp1_packet* pPacket = REINTERPRET_CAST(tsp1_packet*, *packetList);
            m_context.SetPacket(pPacket);
            m_context.a_out_packet = m_context.a_in_packet;
            PIn_RequestPacket packet(pPacket, packetSize, false);
            m_context.a_return_segm = packet.AddSegment (sp1m_dbs, sp1sm_session_sqlmode).GetRawSegment();
            a51CreateConnectReturnSegment(m_context);
            *sessionInfoReply = *packetList;
        }
        m_context.a_command_id = cmdId;
        return true;
    }
    return false;
}

//--------------------------------------------------------------------------

SAPDB_Bool KSQL_Runtime::releaseSession(SAPDB_Int8 sessionID,
                                        SQLDBC_IRuntime::Error& error)
{
    SAPDBTRACE_METHOD_DEBUG ("KSQL_Runtime::releaseSession", KernelSQL_Trace, 1);
    --m_level;
    if (0 != m_pPacket[m_level])
    {
        m_context.GetAllocator().Deallocate(m_pPacket[m_level]);
        m_pPacket[m_level] = 0;
    }
    return true;
}

//--------------------------------------------------------------------------

SAPDB_Bool KSQL_Runtime::request(SAPDB_Int8  sessionID,
                                 void       *requestData,
                                 SAPDB_UInt4 requestDataLength,
                                 SQLDBC_IRuntime::Error& error)
{
    SAPDBTRACE_METHOD_DEBUG ("KSQL_Runtime::request", KernelSQL_Trace, 1);
    ++m_context.a_dbproc_level;
    m_context.GetTempFileCollection().IncrementProcLevel();
    a93request (&m_context, (tsp1_packet_ptr) requestData);
    --m_context.a_dbproc_level;
    m_context.GetTempFileCollection().DecrementProcLevel();
    return true;
}

//--------------------------------------------------------------------------

SAPDB_Bool KSQL_Runtime::receive(SAPDB_Int8 sessionID,
                                 void **replyData,
                                 SAPDB_Int4& replyDataLength,
                                 SQLDBC_IRuntime::Error& error)
{
    SAPDBTRACE_METHOD_DEBUG ("KSQL_Runtime::receive", KernelSQL_Trace, 1);
    *replyData      = m_context.a_saved_packet_ptr;
    replyDataLength = m_context.a_saved_reply_length;
    return true;
}

//--------------------------------------------------------------------------

SAPDB_Bool KSQL_Runtime::cancelCurrentCommand(SAPDB_Int8 sessionID,
                                              SQLDBC_IRuntime::Error& error)
{
    // noop in kernel
    // TODO write error list
    return false;
}

//--------------------------------------------------------------------------

SAPDB_Bool KSQL_Runtime::createMutex(MutexHandle& mutexHandle,
                                     SAPDBMem_IRawAllocator& allocator,
                                     SQLDBC_IRuntime::Error& error)
{
    // prevent null pointer
    mutexHandle = (void *) this;
    // noop in kernel
    return true;
}

//--------------------------------------------------------------------------

SAPDB_Bool KSQL_Runtime::lockMutex(MutexHandle mutexHandle)
{
    // noop in kernel
    return true;
}

//--------------------------------------------------------------------------

SAPDB_Bool KSQL_Runtime::releaseMutex(MutexHandle mutexHandle)
{
    // noop in kernel
    return true;
}

//--------------------------------------------------------------------------

SAPDB_Bool KSQL_Runtime::destroyMutex(MutexHandle& mutexHandle,
                                      SAPDBMem_IRawAllocator& allocator,
                                      SQLDBC_IRuntime::Error& error)
{
    // noop in kernel
    return true;
}

//--------------------------------------------------------------------------

SAPDB_Bool  KSQL_Runtime::createCounter(CounterHandle& counterHandle,
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

SAPDB_UInt4 KSQL_Runtime::nextCounter(CounterHandle counterHandle)
{
    SAPDB_UInt4* pI4 = REINTERPRET_CAST(SAPDB_UInt4*, counterHandle);
    ++(*pI4);
    return *pI4;
}

//--------------------------------------------------------------------------

SAPDB_Bool  KSQL_Runtime::destroyCounter(CounterHandle counterHandle,
                                         SAPDBMem_IRawAllocator& allocator,
                                         SQLDBC_IRuntime::Error& error)
{
    allocator.Deallocate(counterHandle);
    return true;
}

//--------------------------------------------------------------------------

SQLDBC_IRuntime::TaskID KSQL_Runtime::getCurrentTaskID()
{
    tsp00_TaskId pid;
    vgetpid(pid);
    return pid;
}

//--------------------------------------------------------------------------

SAPDB_Bool KSQL_Runtime::createSemaphore(SemaphoreHandle& semaphoreHandle,
                                         SAPDB_Int4 initialValue,
                                         SAPDBMem_IRawAllocator& allocator,
                                         SQLDBC_IRuntime::Error& error)
{
    // prevent null pointer
    semaphoreHandle = (void *) this;
    // noop in kernel
    return true;
}

//--------------------------------------------------------------------------

SAPDB_Bool KSQL_Runtime::waitSemaphore(SemaphoreHandle semaphoreHandle)
{
    // noop in kernel
    return true;
}

//--------------------------------------------------------------------------

SAPDB_Bool KSQL_Runtime::signalSemaphore(SemaphoreHandle semaphoreHandle)
{
     // noop in kernel
    return true;
}

//--------------------------------------------------------------------------

SAPDB_Bool KSQL_Runtime::destroySemaphore(SemaphoreHandle& semaphoreHandle,
                                          SAPDBMem_IRawAllocator& allocator,
                                          SQLDBC_IRuntime::Error& error)
{
     // noop in kernel
    return true;
}

//--------------------------------------------------------------------------

SQLDBC_IRuntime::TaskTraceContext* KSQL_Runtime::getTaskTraceContext()
{
   return &m_interfaceTaskTraceContext;
}

//--------------------------------------------------------------------------

void KSQL_Runtime::write(const char *s, SAPDB_Int4 size)
{
    if (0 == m_pTraceBuffer)
    {
        m_pTraceBuffer = (char*) m_context.GetAllocator().Allocate(c_traceBufferSize);
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
    SAPDB_MemCopyNoCheck (&m_pTraceBuffer[m_traceBufferLength], s, size);
    m_traceBufferLength += size;
}

//--------------------------------------------------------------------------

void KSQL_Runtime::writeln(const char *s, SAPDB_Int4 size)
{
    this->write (s, size);
    this->FlushTrace();
}

//--------------------------------------------------------------------------
void
KSQL_Runtime::updateTraceFlags(SAPDB_Bool& traceEnabled)
{
    if(m_interfaceTaskTraceContext.flags) {
        traceEnabled = true;
    } else {
        traceEnabled = false;
    }
}

//--------------------------------------------------------------------------
void
KSQL_Runtime::addTraceFlags(unsigned int flags)
{
    m_interfaceTaskTraceContext.flags |= flags;
    return;
}

//--------------------------------------------------------------------------
void
KSQL_Runtime::removeTraceFlags(unsigned int flags)
{
    unsigned int mask = 0xFFFFFFFF - flags;
    m_interfaceTaskTraceContext.flags &= mask;
    return;
}


//--------------------------------------------------------------------------

void KSQL_Runtime::request (void  *packetPtr)
{
    SAPDBTRACE_METHOD_DEBUG ("KSQL_Runtime::request", KernelSQL_Trace, 1);
    ++m_context.a_dbproc_level;
    m_context.GetTempFileCollection().IncrementProcLevel();
    a93request (&m_context, (tsp1_packet_ptr) packetPtr);
    --m_context.a_dbproc_level;
    m_context.GetTempFileCollection().DecrementProcLevel();
}

//--------------------------------------------------------------------------

void KSQL_Runtime::receive (void** packetPtr, tsp00_Int4& packetLength)
{
    SAPDBTRACE_METHOD_DEBUG ("KSQL_Runtime::receive", KernelSQL_Trace, 1);
    *packetPtr    = m_context.a_saved_packet_ptr;
    packetLength  = m_context.a_saved_reply_length;
}

//--------------------------------------------------------------------------

void KSQL_Runtime::FlushTrace()
{
    m_pTraceBuffer[m_traceBufferLength] = 0;
    Kernel_VTrace () << m_pTraceBuffer;
    m_traceBufferLength = 0;
}

//--------------------------------------------------------------------------

void KSQL_Runtime::InitTraceContext()
{
    enum
    {
        SQLDBC_TRACE_SHORT         = 1,   // Calls without arguments
        SQLDBC_TRACE_LONG          = 3,   // Calls with arguments
        SQLDBC_TRACE_PACKET        = 4,   // packet content
        SQLDBC_TRACE_PROFILE       = 8,   // additional timestamp
        SQLDBC_TRACE_SQL           = 16   // just sql statements and in/out data
    };

    memset (&m_interfaceTaskTraceContext, 0, sizeof(m_interfaceTaskTraceContext));
    if (KernelSQL_Trace.TracesLevel(5))
    {
        m_interfaceTaskTraceContext.flags = SQLDBC_TRACE_LONG;
    }
}
