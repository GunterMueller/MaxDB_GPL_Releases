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

#include "heo00x.h" // suspend reasons
#include "heo56.h"  // vsuspend, vresume
#include "heo670.h" // eo670_CTraceStack    
#include "heo922.h" // vGetRteInfoTaskDetails
#include "hbd01.h"
#include "hbd02.h"
#include "hbd03.h"
#include "hbd21.h"
#include "hbd73.h"
#include "hgg01.h"
#include "hgg01_3.h"
#include "hgg04.h"
#include "hkb742.h"
#include "hak101.h"

#include "hta01.h"
#include "hta01_3.h"


#include "RunTime/Synchronisation/RTESync_Spinlock.hpp"
#include "Container/Container_Vector.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "SQLManager/SQLMan_MessBlock.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "Join/Join_InvSelectIterator.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Streaming.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "KernelCommon/Kernel_OpMsg.hpp"
#include "RunTime/Tasking/RTETask_SuspendReason.h"

extern SAPDBTrace_Topic Join_Trace;   //!< join tracing

static const int c_max_server_tasks = 64; //!< max number of server tasks used

struct Join_ServerStatistics
{
    Join_ServerStatistics()
        : m_requests(0)
        , m_keyCount(0)
        , m_bufferOverflows(0)
        , m_waitForRequest(0)
        , m_ioCount(0)
        , m_virtualIoCount(0)
        , m_rowLength(0)
    {
    }
    void Trace(int serverId);
    SAPDB_UInt8 m_requests;
    SAPDB_UInt8 m_keyCount;
    SAPDB_UInt8 m_bufferOverflows;
    SAPDB_UInt8 m_waitForRequest;
    SAPDB_UInt8 m_ioCount;
    SAPDB_UInt8 m_virtualIoCount;
    int         m_rowLength;
};

void Join_ServerStatistics::Trace(int serverId)
{
    if ( Join_Trace.TracesLevel( 1 ) )
    {
        if (m_requests > 0)
        {
            Kernel_VTrace trace;
            trace << "Statistics for server " << serverId;
            trace << "Request           : " << m_requests; 
            trace << "Key count         : " << m_keyCount;
            trace << "Buffer overflows  : " << m_bufferOverflows;
            trace << "Waits for request : " << m_waitForRequest;
            trace << "virtual I/O count : " << m_virtualIoCount;
            trace << "I/O count         : " << m_ioCount;
            trace << "Row Length        : " << m_rowLength;
            trace << "--------------------------------------";
        }
    }
}

//-------------------------------------------------------------------------------

static Join_ServerStatistics g_serverStatistics[c_max_server_tasks];

/*!
   @brief defines the interface of a server task of the Join_InvSelectIterator iterator
          (used by Join_InvSelectIterator only)

          A server receives requests from an iterator. A request consists of a pointer to
          a key and a number of keys to be selected.
          For each key the corresponding base table row is selected. If the row is qualified the
          required output row is placed into the servers row buffer. This buffer is organized as a
          ring buffer, where the server stores and the iterator reads rows concurrently. 
 */
class Join_InvSelectServer
{
public :
    // constructor
    Join_InvSelectServer(Join_InvSelectIterator& iterator, int slot);
    // enters a request, i.e. a pointer to the first key and the number of keys to be handled
    void EnterRequest(SAPDB_Byte*  pBuffer, int keyCount, bool storeLastKey);
    // reads a request, i.e. a pointer to the first key and the number of keys to be handled
    bool GetRequest (SAPDB_Byte*& pBuffer, int& keyCount);
    // returns the next row or error e_no_next_record if none is available
    tgg00_BasisError GetNextRow (void* buf, int bufSize);
    // positions to the next row read position 
    inline SAPDB_Byte * IncRowBufferPointer(SAPDB_Byte* p);
    // increments to number of rows written into the buffer, informs iterator if necessary
    inline void IncWritten();
    // informs iterator about row in the buffer, if necessary
    inline void NotifyIterator();
    // informs iterator about end of a request
    void NotifyEndOfRequest();
    // registers the buffer where server stores its result rows
    void RegisterRowBuffer (SAPDB_Byte* pBuffer, int bufferSize, int rowSize);
    // reserves a buffer area for the next row
    tsp00_MoveObjPtr ReserveRowBuffer (SAPDB_Int4& bufferSize);
    // resumes the server if suspended
    inline void Resume(bool condition);
    // sets an error
    void SetError(tgg00_BasisError e);
    // sets the task id of the server
    void SetTaskId   (tsp00_TaskId taskId);
    // stop the server
    void StopServer();
    // returns true, if the current server has to provide the last key to the iterator
    inline bool StoreLastKey () const;
    // stores statistics
    void StoreStatistics (SAPDB_UInt8);
private :
    Join_InvSelectIterator& m_iterator;            // reference to the iterator (coordinator)
    RTESync_Spinlock        m_lock;                // synchronization object 
    tsp00_TaskId            m_serverTaskId;        // server task id
    SAPDB_Byte*             m_pKeyBuffer;          // pointer to the current requested key
    int                     m_keyCount;            // number of keys to be selected
    bool                    m_serverSuspended;     // true, if server is suspended
    bool                    m_stopServer;          // true, if server must stop
    bool                    m_waitingForRequest;   // true, if server is waiting for request
    bool                    m_storeLastKey;        // true, if server has to inform iterator about last key handled
    SAPDB_Byte*             m_pRowBuffer;          // address of row buffer 
    int                     m_bufferSize;          // size of row buffer
    int                     m_rowSize;             // size of a row in row buffer
    int                     m_written;             // number of rows written into row buffer
    int                     m_read;                // number of rows read from row buffer 
    int                     m_maxRowCount;         // maximal number of rows that can be stored in row buffer 
    int                     m_unRecognizedRows;    // number of row not notified to the iterator
    SAPDB_Byte*             m_pRowServer;          // current write position in row buffer 
    SAPDB_Byte*             m_pRowCoordinator;     // current read position in row buffer 
    tgg00_BasisError        m_error;               // server global error
    Join_ServerStatistics*  m_statistics;          // statistics 
};

//-------------------------------------------------------------------------------

Join_InvSelectServer::Join_InvSelectServer(Join_InvSelectIterator& iterator, int slot)
: m_iterator(iterator)
, m_lock()
, m_serverTaskId(0)
, m_pKeyBuffer(0)
, m_keyCount(0)
, m_serverSuspended(false)
, m_stopServer(false)
, m_waitingForRequest(true)
, m_pRowBuffer(0)
, m_bufferSize(0)
, m_rowSize(0)
, m_written(0)
, m_read(0)
, m_maxRowCount(0)
, m_unRecognizedRows(0)
, m_pRowServer(0)
, m_pRowCoordinator(0)
, m_error(e_ok)
, m_statistics(0)
{
    m_statistics = &g_serverStatistics[slot]; 
}

//-------------------------------------------------------------------------------

void Join_InvSelectServer::EnterRequest(SAPDB_Byte* pBuffer, int keyCount, bool storeLastKey)
{
    SAPDBTRACE_METHOD_DEBUG( "Join_InvSelectServer::EnterRequest", Join_Trace, 1 );
    SAPDBTRACE_WRITELN( Join_Trace, 5, "pBuffer : " << (char*) ToStr((void*) pBuffer) << ", keyCount : " << keyCount );
    RTESync_LockedScope lockedScope(m_lock);
    m_pKeyBuffer    = pBuffer;
    m_keyCount      = keyCount;
    m_storeLastKey  = storeLastKey;
    m_written       = 0;
    m_read          = 0;
    ++m_statistics->m_requests;
    m_statistics->m_keyCount += keyCount;
    this->Resume(true);
}

//-------------------------------------------------------------------------------

tgg00_BasisError Join_InvSelectServer::GetNextRow (void* buf, int bufSize)
{
    SAPDBTRACE_METHOD_DEBUG( "Join_InvSelectServer::GetNextRow", Join_Trace, 1 );
    if (e_ok != m_error)
    {
        return m_error;
    }
    RTESync_LockedScope lockedScope(m_lock);
    if (m_written > m_read)
    {
        SAPDB_MemCopyNoCheck (buf, m_pRowCoordinator, m_rowSize);
        ++m_read;
        m_pRowCoordinator = IncRowBufferPointer(m_pRowCoordinator);
        this->Resume(false);
        return e_ok;
    }
    return e_no_next_record;
}

//-------------------------------------------------------------------------------

bool Join_InvSelectServer::GetRequest (SAPDB_Byte*& pBuffer, int& keyCount)
{
    SAPDBTRACE_METHOD_DEBUG( "Join_InvSelectServer::GetRequest", Join_Trace, 1 );
    if (m_unRecognizedRows) 
    {
        m_iterator.IncRowCount(m_unRecognizedRows);
        m_unRecognizedRows = 0;
    }
    m_lock.Lock();
    if ((0 == m_keyCount) && !m_stopServer)
    {
        m_waitingForRequest = true;
        m_serverSuspended   = true;
        m_lock.Unlock();
        ++m_statistics->m_waitForRequest;
        SAPDBTRACE_WRITELN( Join_Trace, 1, "Suspend Server(" << m_serverTaskId << ")");
        vsuspend (m_serverTaskId, SUSPEND_REASON_INV_SEL_SERVER_REQUEST);
        SAPDBTRACE_WRITELN( Join_Trace, 1, "Server(" << m_serverTaskId << ") waked up");
    }
    else
    {
        m_lock.Unlock();
    }
    m_waitingForRequest = false;
    pBuffer             = m_pKeyBuffer;
    keyCount            = m_keyCount;
    m_keyCount          = 0;
    SAPDBTRACE_WRITELN( Join_Trace, 5, "keyCount : " << keyCount );
    return ((keyCount > 0) && !m_stopServer);
}

//-------------------------------------------------------------------------------

inline SAPDB_Byte * Join_InvSelectServer::IncRowBufferPointer(SAPDB_Byte* p) 
{ 
    SAPDBTRACE_METHOD_DEBUG( "Join_InvSelectServer::IncRowBufferPointer", Join_Trace, 5 );
    SAPDB_Byte * pRes = p + m_rowSize;
    pRes = (pRes + m_rowSize <= m_pRowBuffer + m_bufferSize) ? pRes : m_pRowBuffer;
    SAPDBTRACE_WRITELN( Join_Trace, 5, "row ptr : " << (char*) ToStr((void*) pRes) );
    return pRes;
}

//-------------------------------------------------------------------------------

inline void Join_InvSelectServer::IncWritten()
{
    SAPDBTRACE_METHOD_DEBUG( "Join_InvSelectServer::IncWritten", Join_Trace, 1 );
    RTESync_LockedScope lockedScope(m_lock);
    m_pRowServer = this->IncRowBufferPointer(m_pRowServer);
    ++m_written;
    this->NotifyIterator();
}

//-------------------------------------------------------------------------------

void Join_InvSelectServer::NotifyEndOfRequest()
{
    SAPDBTRACE_METHOD_DEBUG( "Join_InvSelectServer::NotifyEndOfRequest", Join_Trace, 1 );
    if (m_unRecognizedRows > 0)
    {
        // there are rows that have not been noted by the iterator
        m_iterator.IncRowCount(m_unRecognizedRows);
        m_unRecognizedRows = 0;
    }
    m_iterator.NotifyRequestFinished();
}

//-------------------------------------------------------------------------------

inline void Join_InvSelectServer::NotifyIterator()
{
    ++m_unRecognizedRows;
    SAPDBTRACE_WRITELN( Join_Trace, 5, "NotifyIterator, unrecognized " << m_unRecognizedRows);
    if (m_iterator.IsSuspended())
    {
        m_iterator.IncRowCount(m_unRecognizedRows);
        m_unRecognizedRows = 0;
    }
    // we don't have to inform the iterator immediatly, if it is not suspended. This may save
    // syncronization calls. Note that dirty read is permissable because the outstanding
    // messages will be send if the buffer becomes full or the server terminates. 
}

//-------------------------------------------------------------------------------

void Join_InvSelectServer::RegisterRowBuffer (SAPDB_Byte* pBuffer, int bufferSize, int rowSize)
{
    SAPDBTRACE_METHOD_DEBUG( "Join_InvSelectServer::RegisterRowBuffer", Join_Trace, 1 );
    SAPDBTRACE_WRITELN( Join_Trace, 5, "pBuffer " << (char*) ToStr((void*) pBuffer) << ", Size " << bufferSize << ", rowSize " << rowSize);
    if (rowSize & 1)
    {
        ++rowSize;
    }
    RTESync_LockedScope lockedScope(m_lock);
    m_statistics->m_rowLength = rowSize;
    m_pRowBuffer      = pBuffer;
    m_bufferSize      = bufferSize;
    m_rowSize         = rowSize;
    m_maxRowCount     = bufferSize / rowSize;
    m_pRowServer      = this->IncRowBufferPointer(pBuffer);
    m_pRowCoordinator = pBuffer;
    m_written         = 1;
    m_read            = 0;
    this->NotifyIterator();
}

//-------------------------------------------------------------------------------

tsp00_MoveObjPtr Join_InvSelectServer::ReserveRowBuffer (SAPDB_Int4& bufferSize)
{
    SAPDBTRACE_METHOD_DEBUG( "Join_InvSelectServer::ReserveRowBuffer", Join_Trace, 1 );
    bool doSuspend;
    {
        RTESync_LockedScope lockedScope(m_lock);
        if (m_stopServer) return 0;
        doSuspend         = (m_written - m_read == m_maxRowCount);
        m_serverSuspended = doSuspend;
    }
    if (doSuspend)
    {
        SAPDBTRACE_WRITELN( Join_Trace, 1, "Suspend Server(" << m_serverTaskId << "), buffer full");
        if (m_unRecognizedRows > 0)
        {
            // there are rows that have not been noted by the iterator
            // do it now before suspending
            m_iterator.IncRowCount(m_unRecognizedRows);
            m_unRecognizedRows = 0;
        }
        ++m_statistics->m_bufferOverflows;
        vsuspend (m_serverTaskId, SUSPEND_REASON_INV_SEL_SERVER_RESERVE_ROW_BUFFER);  
    }
    bufferSize = m_rowSize;
    return reinterpret_cast<tsp00_MoveObjPtr>(m_pRowServer);  
}

//-------------------------------------------------------------------------------

inline void Join_InvSelectServer::Resume(bool waitForRequest)
{
    SAPDBTRACE_METHOD_DEBUG( "Join_InvSelectServer::Resume", Join_Trace, 1 );
    SAPDBTRACE_WRITELN( Join_Trace, 5, "serverSuspended : " << m_serverSuspended << " waitForRequest : " << waitForRequest);
    if (m_serverSuspended)
    {
        if ((waitForRequest && m_waitingForRequest) ||
            (!waitForRequest && !m_waitingForRequest))
        {
            SAPDBTRACE_WRITELN( Join_Trace, 1, "Resume Server(" << m_serverTaskId << ")");
            m_serverSuspended = false;
            vresume (m_serverTaskId);
        }
    }
}

//-------------------------------------------------------------------------------

inline void Join_InvSelectServer::SetTaskId (tsp00_TaskId taskId)
{
    m_serverTaskId = taskId; 
}

//-------------------------------------------------------------------------------

void Join_InvSelectServer::SetError(tgg00_BasisError e)
{
    SAPDBTRACE_METHOD_DEBUG( "Join_InvSelectServer::SetError", Join_Trace, 1 );
    m_error = e;
}

//-------------------------------------------------------------------------------

void Join_InvSelectServer::StopServer()
{
    SAPDBTRACE_METHOD_DEBUG( "ServerRequest::StopServer", Join_Trace, 1 );
    RTESync_LockedScope lockedScope(m_lock);
    m_stopServer        = true;
    m_waitingForRequest = true;
    this->Resume(true);
}

//-------------------------------------------------------------------------------

inline bool Join_InvSelectServer::StoreLastKey () const
{
   return m_storeLastKey;
}

//-------------------------------------------------------------------------------

void Join_InvSelectServer::StoreStatistics (SAPDB_UInt8 ioCount)
{
    if ( Join_Trace.TracesLevel( 1 ) )
    {
        m_statistics->m_virtualIoCount += ioCount;
        tsp92_RteTaskDetailsInfo taskInfo;
        vGetRteInfoTaskDetails ( m_serverTaskId, &taskInfo);
        m_statistics->m_ioCount += (SAPDB_UInt8) (taskInfo.SelfIORead.Cnt + taskInfo.DEVIORead.Cnt);
    }
}

//-------------------------------------------------------------------------------
// implementation of class Join_InvSelectIterator
//-------------------------------------------------------------------------------

bool Join_InvSelectIterator::m_serversInUse   = false;

//-------------------------------------------------------------------------------

Join_InvSelectIterator::Join_InvSelectIterator
(
 tgg00_TransContext      &Trans,
 SQLMan_MessBlock        &messBlock,
 tgg00_FileId            &InvTree,
 tgg00_SelectFieldsParam &SelectParam,
 tgg00_StackDesc         &StackDesc,
 const SAPDB_Int4        &maxServerTasks)
 : m_lock()
 , m_messBlock(messBlock)
 , m_selFieldsParam(SelectParam)
 , m_tree(messBlock.mb_qual()->mtree())
 , m_invFileId (InvTree)
 , m_pInvKey(0)
 , m_pStopKey(0)
 , m_allocator(SQLMan_Context::GetContext()->GetAllocator())
 , m_stop(false)
 , m_suspended(false)
 , m_isOpen(false)
 , m_countStatistics(true)
 , m_error(e_ok)
 , m_bdInvSet()
 , m_maxServers(maxServerTasks)
 , m_servers(m_allocator)
 , m_coordinatorTrans(Trans)
 , m_countRowsProvided(0)
 , m_countRowsHandled(0)
 , m_runningServers(0)
 , m_startedServers(0)
 , m_keyCount(0)
 , m_getInvError(e_buffer_limit)
 , m_currServer(0)
 , m_WaitingTask(Trans_Context::GetContext(Trans).Task())
 , m_Jobs(Trans_Context::GetContext(Trans).Allocator())

{
}

//-------------------------------------------------------------------------------

Join_InvSelectIterator::~Join_InvSelectIterator()
{
    SAPDBTRACE_METHOD_DEBUG( "Join_InvSelectIterator::~Join_InvSelectIterator", Join_Trace, 1 );
    this->StopServers();
    if ( Join_Trace.TracesLevel( 1 ) )
    {
        Kernel_VTrace trace;
        trace << "parallel join coordinator";
        trace << "virtual I/O count : " << m_coordinatorTrans.trIoCount_gg00;
        tsp92_RteTaskDetailsInfo taskInfo;
        vGetRteInfoTaskDetails ( m_coordinatorTrans.trTaskId_gg00, &taskInfo );
        trace << "I/O count         : " << (taskInfo.SelfIORead.Cnt + taskInfo.DEVIORead.Cnt);
        for (int ix = 0; ix < m_maxServers; ++ix)
        {
            g_serverStatistics[ix].Trace(ix);
        }
    }
}

//-------------------------------------------------------------------------------

void Join_InvSelectIterator::Close()
{
    SAPDBTRACE_METHOD_DEBUG( "Join_InvSelectIterator::Close", Join_Trace, 1 );
    this->StopServers();
    m_isOpen = false;
}

//-------------------------------------------------------------------------------

bool Join_InvSelectIterator::FillKeyBuffer()
{
    SAPDBTRACE_METHOD_DEBUG( "Join_InvSelectIterator::FillKeyBuffer", Join_Trace, 1 );
    if (e_buffer_limit == m_getInvError)
    {
        SAPDB_Int4  fillLength;
        tgg00_Lkey  currKey;
        b03get_inv (m_coordinatorTrans, m_invFileId, *m_pInvKey,
            m_startKey, *m_pStopKey, currKey, m_bdInvSet,
            false, lckFree_egg00, m_keyBuffer, m_keyCount, fillLength);
        m_getInvError = m_coordinatorTrans.trError_gg00;
        m_coordinatorTrans.trError_gg00 = e_ok;
        m_bdInvSet.delElement(incl_first);
        if ((m_getInvError == e_ok) || (m_getInvError == e_buffer_limit))
        {
            if (0 == m_keyCount)
            {
                m_getInvError = e_no_next_record;
                return false;
            }
            if ((m_maxServers > 1) && (m_keyCount > 4) && CreatedParallelServers())
            {
                b73cmd_count(iget_inv_parallel);
                if (m_countStatistics)
                {
                    m_countStatistics = false;
                    b73cmd_count(ijoin_parallel);
                }
                this->EnterRequest (m_keyCount);
            }
            else
            {
                m_pKeyBuffer = reinterpret_cast<SAPDB_Byte*>(&m_keyBuffer);
            }
            return true;
        }
        else
        {
            m_error = m_getInvError; // unexpected error
        }
    }
    return false;
}

//-------------------------------------------------------------------------------

inline void Join_InvSelectIterator::CopyKey(const tgg00_Lkey& source, tgg00_Lkey& dest)
{
    dest.keyLen_gg00() = source.keyLen_gg00();
    SAPDB_MemCopyNoCheck (&dest.keyVal_gg00(), &dest.keyVal_gg00(), source.keyLen_gg00());
}

//-------------------------------------------------------------------------------

bool Join_InvSelectIterator::CreatedParallelServers()
{
    SAPDBTRACE_METHOD_DEBUG( "Join_InvSelectIterator::CreatedParallelServers", Join_Trace, 1 );
    SAPDBTRACE_WRITELN( Join_Trace, 5, "m_startedServers : " << m_startedServers);
    if (m_startedServers > 0)
    {
        // servers already assigned to current session
        return true;
    }
    {
        static RTESync_Spinlock serversInUseLock;
        RTESync_LockedScope lockedScope(serversInUseLock);
        // are servers currently in use ?
        if (!m_serversInUse)
        {
             m_serversInUse = true;
        }
        else
        {
            return false;
        }
    }
    int availableServers = bd01UsableServerForParallelExecution ();
    if (availableServers <= 1)
    {
        // not enough servers available
        m_serversInUse = false;
        return false;
    }
    // try parallel execution
    if (!m_servers.Initialize(availableServers))
    {
        // no memory to initialize vector
        m_serversInUse = false;
        return false;
    }
    SAPDBTRACE_WRITELN( Join_Trace, 1, "starting " << availableServers << " servers");
    for (int serverId = 0; serverId < availableServers; ++serverId)
    {
        m_servers[serverId] = new(m_allocator) Join_InvSelectServer(*this, serverId);
        if (!m_servers[serverId])
        {
            for (int ix = 0; ix < serverId - 1; ++ix)
            {
                destroy (m_servers[serverId], m_allocator);
            }
            return false;
        }
    }
    for (int servId = 0; servId < availableServers; ++servId)
    {
        this->StartServer(servId);
        if (this->NoError())
        {
            ++m_startedServers;
        }
        else
        {
            break;
        }
    }
    if (e_ok != m_error)
    {
        this->StopServers();
        return false;
    }
    m_coordinatorTrans.trSyncParallelServers_gg00 = true;
    return true;
}

//-------------------------------------------------------------------------------

void Join_InvSelectIterator::EnterRequest (int keyCount)
{
    SAPDBTRACE_METHOD_DEBUG( "Join_InvSelectIterator::EnterRequest", Join_Trace, 1 );
    SAPDBTRACE_WRITELN( Join_Trace, 1, "keyCount " << keyCount);
    m_countRowsProvided = 0;
    m_countRowsHandled  = 0;
    m_currServer        = 0;
    SAPDB_Int2 keyLength;
    SAPDB_Byte* l1 = reinterpret_cast<SAPDB_Byte*>(&keyLength);
    SAPDB_Byte* l2 = l1 + 1;
    int countServers  = m_servers.GetSize();
    if (keyCount < countServers)
    {
        countServers = keyCount;
    }
    m_runningServers  = countServers;
    int keysPerServer = keyCount / countServers;
    int count = 0;
    SAPDB_Byte* pBuffer = reinterpret_cast<SAPDB_Byte*>(&m_keyBuffer);
    const bool c_storeLastKey = true;
    for (int ix = 0; ix < countServers - 1; ++ix)
    {
        m_servers[ix]->EnterRequest(pBuffer, keysPerServer, !c_storeLastKey);
        for (count = 0; count < keysPerServer; ++count)
        {
          *l1 = *pBuffer++;
          *l2 = *pBuffer++;
          pBuffer += keyLength;
        }
    }
    m_servers[countServers-1]->EnterRequest(pBuffer, keysPerServer + keyCount % countServers, c_storeLastKey);
}

//-------------------------------------------------------------------------------

void Join_InvSelectIterator::IncRowCount(int rowCount)
 {
     SAPDBTRACE_METHOD_DEBUG( "Join_InvSelectIterator::IncRowCount", Join_Trace, 1 );
     SAPDBTRACE_WRITELN( Join_Trace, 5, "rowCount : " << rowCount);
     m_lock.Lock();
     m_countRowsProvided += rowCount;
     this->Resume();
     m_lock.Unlock();
 }

//-------------------------------------------------------------------------------

inline bool Join_InvSelectIterator::IsSuspended() const
{
    return m_suspended;
}

//-------------------------------------------------------------------------------

 tgg00_BasisError Join_InvSelectIterator::NextRecord(void* buf)
 {
     SAPDBTRACE_METHOD_DEBUG( "Join_InvSelectIterator::NextRecord", Join_Trace, 1 );
     if (0 == m_keyCount)
     {
         return e_buffer_limit;
     }
     tgg00_BasisError e;
     SAPDB_Int2 keyLength;
     SAPDB_Byte* k1 = reinterpret_cast<SAPDB_Byte*>(&keyLength);
     SAPDB_Byte* k2 = k1 + 1;
     while (true)
     {
         *k1 = *m_pKeyBuffer++;
         *k2 = *m_pKeyBuffer++;
         pasbool           dummyBool;
         tgg00_LockReqMode dummyGranted;
         m_selFieldsParam.sfp_bd_mess2_type().becomes(mm_direct);
         m_selFieldsParam.sfp_m_result_len()  = 0;
         m_selFieldsParam.sfp_m_result_cnt()  = 1; // at most one result
         SAPDBTRACE_WRITELN( Join_Trace, 5, "result_addr : " << (char*) ToStr((void*) m_selFieldsParam.sfp_m_result_addr()));
         m_tree.fileBdUse_gg00().clear();
         
         m_selFieldsParam.sfp_rows_read() = 0;
         m_selFieldsParam.sfp_rows_qual() = 0;
            
         b02kb_select_rec (m_coordinatorTrans, m_tree, m_pKeyBuffer, keyLength,
             m_pKeyBuffer, keyLength, 0, 0, false, m_selFieldsParam, m_messBlock.mb_qual()->mstack_desc(),
             dummyBool, dummyGranted);

        if ( m_messBlock.mb_trns()->trBdTcachePtr_gg00 != NULL )
        {
            b21mp_rows_read_put (m_messBlock.mb_trns()->trBdTcachePtr_gg00, m_selFieldsParam.sfp_rows_read());
            b21mp_rows_qual_put (m_messBlock.mb_trns()->trBdTcachePtr_gg00, m_selFieldsParam.sfp_rows_qual());
        }
        

         e =  m_coordinatorTrans.trError_gg00;
         bool found;
         if (e_ok == e)
         {
             // found a qualified row
             found = true;
         }
         else
         {
             m_coordinatorTrans.trError_gg00 = e_ok;
             found = false;
             if ((e_key_not_found  == e) || (e_qual_violation == e) || (e_view_violation == e))
             {
                 // no or unqualified row found, continue searching
                 e = e_ok;
             }
             else
             {
                 // unexpected error, aborts execution
                 break;
             }
         }
         --m_keyCount;
         if (0 == m_keyCount)
         {
             // last key of buffer handled
             if (e_buffer_limit == m_getInvError)
             {
                 // inv list not already traversed completly, store last key for next inv list buffer 
                 m_startKey.keyLen_gg00() = keyLength;
                 SAPDB_MemCopyNoCheck (&m_startKey.keyVal_gg00(), m_pKeyBuffer, keyLength);
                 if (!found)
                 {
                     e = e_buffer_limit;
                 }
             }
             else
             {
                 if (!found)
                 {
                     e = e_no_next_record;
                 }
             }
             break; // exit search
         }
         else
         {
             m_pKeyBuffer += keyLength;
             if (found)
             {
                 break;
             }
         }
     }
     SAPDBTRACE_WRITELN( Join_Trace, 5, "NextRecord result : " << SAPDBTrace::BasisError(e));
     return e;
 }

//-------------------------------------------------------------------------------

 tgg00_BasisError Join_InvSelectIterator::NextRow(void* buf)
 {
     SAPDBTRACE_METHOD_DEBUG( "Join_InvSelectIterator::NextRow", Join_Trace, 1 );
 again :
     if (e_ok != m_error)
     {
         return this->NextRowResult (m_error);
     }
     if (0 == m_startedServers)
     {
         tgg00_BasisError e = this->NextRecord(buf);
         if (e_buffer_limit == e)
         {
             if (this->FillKeyBuffer())
             {
                 goto again;
             }
             e = e_no_next_record;
         }
         return this->NextRowResult (e);
     }
     else
     {
         m_lock.Lock();
         if (m_countRowsHandled == m_countRowsProvided)
         {
             if (0 == m_runningServers)
             {
                 // if no server is running any more, we are ready
                 m_lock.Unlock();
                 if (this->FillKeyBuffer())
                 {
                     goto again;
                 }
                 return this->NextRowResult (e_no_next_record);
             }
             // no row available, wait ...
             this->Suspend();
             // note : coordinator is waked up if m_runningServer is 0 or at least one row is available
             if ((0 == m_runningServers) && (m_countRowsHandled == m_countRowsProvided))
             {
                 if (this->FillKeyBuffer())
                 {
                     goto again;
                 }
                 return this->NextRowResult (e_no_next_record);
             }
         }
         else
         {
             m_lock.Unlock();
         }
         // here we are sure, that at least one server can provide a row. 
         for (int server = 0; server < m_startedServers; ++server)
         {
             if (e_ok != m_error)
             {
                 return m_error;
             }
             Join_InvSelectServer* pServer = m_servers[m_currServer];
             // ask current server for a row
             tgg00_BasisError e = pServer->GetNextRow(m_selFieldsParam.sfp_m_result_addr(), m_selFieldsParam.sfp_m_result_size());
             if (e_ok == e)
             {
                 ++m_countRowsHandled;
                 return e_ok;
             }
             if (e_no_next_record == e)
             {
                 // currrent server has no row, try next one
                 if (m_currServer + 1 == m_startedServers)
                 {
                     m_currServer = 0;
                 }
                 else
                 {
                     ++m_currServer;
                 }
             }
             else 
             {
                 // something unexpected happend 
                 return this->NextRowResult (e);
             }
         }
         SAPDBTRACE_WRITELN( Join_Trace, 5, "unexpected : no server provides a row, provided " << m_countRowsProvided << " handled " << m_countRowsHandled); 
         return this->NextRowResult (e_invalid);
     }
 }

//-------------------------------------------------------------------------------

inline tgg00_BasisError Join_InvSelectIterator::NextRowResult(tgg00_BasisError e)
{
    if (e_ok != e)
    {
        this->StopServers();
    }
    return e;
}

//-------------------------------------------------------------------------------

void Join_InvSelectIterator::NotifyRequestFinished()
 {
     SAPDBTRACE_METHOD_DEBUG( "Join_InvSelectIterator::NotifyRequestFinished", Join_Trace, 1 );
     RTESync_LockedScope lockedScope(m_lock);
     --m_runningServers;
     if (0 == m_runningServers)
     {
         this->Resume();
     }
 }

 //-------------------------------------------------------------------------------

tgg00_BasisError Join_InvSelectIterator::Open (
     tgg00_Lkey          &invKey,
     tgg00_Lkey          &startKey,
     tgg00_Lkey          &stopKey,
     tgg00_BdInvSet      &InvRangeSet)
{
     SAPDBTRACE_METHOD_DEBUG( "Join_InvSelectIterator::Open", Join_Trace, 1 );
     if (m_isOpen)
     {
         Kernel_OpError (csp3_msg, csp3_n_join) << "Join_InvSelectIterator::Open : already open ";
         eo670_CTraceStack();   
         m_error = e_invalid;
         return m_error;
     }
     m_pInvKey  = &invKey;
     m_pStopKey = &stopKey;
     m_bdInvSet = InvRangeSet;
     CopyKey (startKey, m_startKey);
     m_stop              = false;
     m_suspended         = false;
     m_error             = e_ok;
     m_countRowsProvided = 0;
     m_countRowsHandled  = 0;
     m_runningServers    = 0;
     m_startedServers    = 0;
     m_keyCount          = 0;
     m_getInvError       = e_buffer_limit;
     m_currServer        = 0;
     m_isOpen            = true;
     return e_ok;
}

//-------------------------------------------------------------------------------

inline void Join_InvSelectIterator::Resume()
 {
     SAPDBTRACE_METHOD_DEBUG( "Join_InvSelectIterator::Resume", Join_Trace, 1 );
     if (m_suspended)
     {
         SAPDBTRACE_WRITELN( Join_Trace, 1, "Resume Coordinator");
         m_suspended = false;
         vresume (m_coordinatorTrans.trTaskId_gg00);
         SAPDBTRACE_WRITELN( Join_Trace, 1, "rows provided " << m_countRowsProvided << "row handled " << m_countRowsHandled);
     }
 }

//-------------------------------------------------------------------------------

void Join_InvSelectIterator::SetMaxServerTasks(SAPDB_Int4 max)
{
    if (max > c_max_server_tasks)
    {
        max = c_max_server_tasks;
    }
    m_maxServers = a101_SetMaxParallelServers( max );

    SAPDBTRACE_WRITELN( Join_Trace, 1, "max Servers " << m_maxServers );
}

//-------------------------------------------------------------------------------

SAPDB_Int Join_InvSelectIterator::GetMaxServerTasks()
{
    return m_maxServers;
}
//-------------------------------------------------------------------------------

void Join_InvSelectByServer (      Trans_Context                      &servertrans,
                             const SrvTasks_JobJoinInvSelector::Input &input)
{
    SAPDBTRACE_ROUTINE_DEBUG ("Join_InvSelectByServer", Join_Trace, 1 );
    
    Join_InvSelectIterator &iter           = *(reinterpret_cast<Join_InvSelectIterator*>(input.m_pIterator));
    Join_InvSelectServer   &server         = *(reinterpret_cast<Join_InvSelectServer*>  (input.m_pServer));
    tgg00_TransContext     &originalTrans  = *(reinterpret_cast<tgg00_TransContext*>    (input.m_pTrans));
	tak_all_command_glob   &acv            = *(reinterpret_cast<tak_all_command_glob*>  (originalTrans.trAcvPtr_gg00));
    tgg00_MessBlock         m;

    server.SetTaskId(servertrans.Task().ID());

    g01mblock_init (servertrans.OldTrans(), m_select_row, mm_parallel, m);

    m.mb_trns()->trSessionId_gg00            = originalTrans.trSessionId_gg00;
    m.mb_trns()->trIndex_gg00                = originalTrans.trIndex_gg00;
    m.mb_trns()->trTransId_gg00              = originalTrans.trTransId_gg00;
    m.mb_trns()->trWriteTransId_gg00         = originalTrans.trWriteTransId_gg00;
    m.mb_trns()->trSubtransId_gg00           = originalTrans.trSubtransId_gg00;
    m.mb_trns()->trConsistView_gg00          = originalTrans.trConsistView_gg00;
    m.mb_trns()->trState_gg00                = originalTrans.trState_gg00;
    m.mb_trns()->trAcvPtr_gg00               = originalTrans.trAcvPtr_gg00;
    m.mb_trns()->trIoCount_gg00              = 0;
    m.mb_trns()->trSyncParallelServers_gg00  = true; // synchronize lock list against parallel servers 
    
    m.mb_data()       = iter.m_messBlock.mb_data();
    m.mb_data_size()  = iter.m_messBlock.mb_data_size();
    m.mb_qual()       = iter.m_messBlock.mb_qual();
    m.mb_qual_size()  = iter.m_messBlock.mb_qual_size();
    m.mb_strat()      = iter.m_messBlock.mb_strat();
    m.mb_strat_size() = iter.m_messBlock.mb_strat_size();

    m.mb_qual()->mtree() = iter.GetTree();
    m.mb_qual()->mtree().fileBdUse_gg00().clear();

    m.mb_work_st_max()  = g01kb_work_st_size() / sizeof (tgg00_StackEntry);
    m.mb_workbuf_size() = g01kb_workbuf_size();

    SAPDBMem_IRawAllocator& allocator = servertrans.Allocator();

    m.mb_workbuf() = reinterpret_cast<tsp00_MoveObjPtr  >(allocator.Allocate(m.mb_workbuf_size()));
    m.mb_work_st() = reinterpret_cast<tgg00_StackListPtr>(allocator.Allocate(g01kb_work_st_size()));

    if (!m.mb_workbuf() || !m.mb_work_st())
    {
        server.SetError(e_no_more_memory);
    }
    else
    {
        SAPDB_Int2 _keyLength;
        SAPDB_Byte* _l1 = reinterpret_cast<SAPDB_Byte*>(&_keyLength);
        SAPDB_Byte* _l2 = _l1 + 1;
        tsp00_Buf _buffer;

        while(true)
        {
            SAPDB_Byte* _pBuffer;
            tsp00_Key   _key;
            int         _keyCount;
            if (!server.GetRequest(_pBuffer, _keyCount))
            {
                break;
            }
            tgg00_SelectFieldsParam _sel;
            g04init_select_fields (_sel, &m.mb_data()->mbp_buf(), m.mb_data_size(),
                m.mb_work_st(), m.mb_work_st_max(), m.mb_workbuf(), m.mb_workbuf_size(),
                m.mb_qual()->msqlmode(), &m.mb_fieldlists());
            _sel.sfp_bd_mess_type().becomes(m_select);
            _sel.sfp_bd_mess2_type().becomes(mm_direct);
            _sel.sfp_acv_addr()   = reinterpret_cast<tsp00_Addr>(originalTrans.trAcvPtr_gg00);
            _sel.sfp_m_result_addr() = reinterpret_cast<tsp00_MoveObjPtr>(&_buffer);
            _sel.sfp_m_result_size() = sizeof (_buffer);
            _sel.sfp_result_wanted() = true;
            bool _bufferRegistered = false;
            while (_keyCount > 0)
            {
                *_l1 = *_pBuffer++;
                *_l2 = *_pBuffer++;
                SAPDB_MemCopyNoCheck (&_key, _pBuffer, _keyLength);
                _pBuffer += _keyLength;
                --_keyCount;
                pasbool           _dummyBool;
                tgg00_LockReqMode _dummyGranted;
                _sel.sfp_m_result_len()  = 0;
                _sel.sfp_result_length() = 0;
                if (_bufferRegistered)
                {
                    _sel.sfp_m_result_addr() = server.ReserveRowBuffer(_sel.sfp_m_result_size());
                }
                if (iter.Abort())
                {
                    break;
                }
                b02kb_select_rec (*m.mb_trns(), m.mb_qual()->mtree(), 
                    reinterpret_cast<tsp00_BytePtr>(&_key), _keyLength,
                    reinterpret_cast<tsp00_BytePtr>(&_key), _keyLength,
                    0, 0, 
                    false, _sel, m.mb_qual()->mstack_desc(), _dummyBool, _dummyGranted);
                tgg00_BasisError _e =  m.mb_trns()->trError_gg00;
                if ((e_ok == _e) || (e_buffer_limit == _e))
                {
                    if (_bufferRegistered)
                    {
                        server.IncWritten();
                    }
                    else
                    {
                        server.RegisterRowBuffer(&_buffer[0], sizeof(_buffer), _sel.sfp_result_length());
                        _bufferRegistered = true;
                    }
                }
                else
                {
                    if ((e_key_not_found  != _e) && (e_qual_violation != _e) && (e_view_violation != _e))
                    {
                        // something unexpected happend
                        server.SetError(_e);
                        break;
                    }
                }
            } // while keyCount > 0
            if (server.StoreLastKey())
            {
                iter.StoreLastKey(_key, _keyLength);
            }
            server.NotifyEndOfRequest();
        }
    }
    server.StoreStatistics ((SAPDB_UInt8) m.mb_trns()->trIoCount_gg00);
    m.mb_trns()->trAcvPtr_gg00 = 0;
    m.mb_qual_len()            = 0;
    m.mb_data_len()            = 0;
    allocator.Deallocate (m.mb_workbuf());
    allocator.Deallocate (m.mb_work_st());
}
//-------------------------------------------------------------------------------

void Join_InvSelectIterator::StartServer (int serverId)
{
    SAPDBTRACE_METHOD_DEBUG( "Join_InvSelectIterator::StartServer", Join_Trace, 1 );

    Trans_Context &trans = Trans_Context::GetContext(m_coordinatorTrans);

    SrvTasks_JobJoinInvSelector *pNewJob =
        new(trans.Allocator()) SrvTasks_JobJoinInvSelector
            ( serverId, SrvTasks_JobJoinInvSelector::Input(this, m_servers[serverId], &m_coordinatorTrans) );

    if ( 0 == pNewJob )
    {
        m_error = e_no_more_memory;
        return;
    }
    
    if ( pNewJob->ExecuteByServer(trans.Task(), trans.ErrorList(), &m_WaitingTask)
         !=
         SrvTasks_Job::executeOK )
	{
        destroy(pNewJob,trans.Allocator());
        m_error = e_start_server_task_error;
        return;
	}
    if ( ! m_Jobs.InsertEnd(pNewJob) )
    {
        destroy(pNewJob,trans.Allocator());
        m_error = e_no_more_memory;
        return;
    }
}

//-------------------------------------------------------------------------------

void Join_InvSelectIterator::StopServers()
{
    SAPDBTRACE_METHOD_DEBUG( "Join_InvSelectIterator::StopServers", Join_Trace, 1 );
    m_stop = true;
    if (m_startedServers > 0)
    {
        for (int serverId = 0; serverId < m_startedServers; ++serverId)
        {
            m_servers[serverId]->StopServer();
        }
        
        Trans_Context &trans = Trans_Context::GetContext(m_coordinatorTrans);
        
        tgg00_BasisError                                        e              = e_ok;
        int                                                     startedServers = m_startedServers;
        SrvTasks_JobList<SrvTasks_JobJoinInvSelector>::Iterator finishedJob;
        
        while ( 0 < m_startedServers )
        {
            m_coordinatorTrans.trError_gg00 = e_ok;
            if ( m_Jobs.WaitForAnyJobFinished (m_WaitingTask, finishedJob) )
            {
                m_coordinatorTrans.trError_gg00 = (*finishedJob)->GetResult(trans.ErrorList());
            
                if ( m_coordinatorTrans.trError_gg00 == e_ok
                     &&
                     (*finishedJob)->GetState() == SrvTasks_Job::aborted )
                {
                    m_coordinatorTrans.trError_gg00 = e_cancelled;
                }
                m_Jobs.RemoveJob(finishedJob);
            }
            else
            {
                m_coordinatorTrans.trError_gg00 = e_no_more_jobs_to_wait_for;
            }
            

            if (e_ok != m_coordinatorTrans.trError_gg00) 
            {
                e = m_coordinatorTrans.trError_gg00;
                Kernel_OpError (csp3_msg, csp3_n_join) << "join server returned error " << e;
            }
            --m_startedServers;
            SAPDBTRACE_WRITELN( Join_Trace, 1, "still running servers : " << m_startedServers);
        }
        for (int ix = 0; ix < startedServers; ++ix)
        {
            destroy (m_servers[ix], m_allocator);
        }
        // TODO message list
        m_serversInUse = (e != e_ok); // don't use servers any more, if rcv_child returned an error
        if (m_serversInUse)
        {
            Kernel_OpError (csp3_msg, csp3_n_join) << "parallel join disabled";
        }
        m_coordinatorTrans.trSyncParallelServers_gg00 = false;
    }
}

//-------------------------------------------------------------------------------

void Join_InvSelectIterator::StoreLastKey (const tsp00_Key& key, int keyLength)
{
    SAPDBTRACE_METHOD_DEBUG( "Join_InvSelectIterator::StoreLastKey", Join_Trace, 1 );
    m_startKey.keyLen_gg00() = keyLength;
    SAPDB_MemCopyNoCheck (&m_startKey.keyVal_gg00(), &key, keyLength);
}

//-------------------------------------------------------------------------------

void Join_InvSelectIterator::Suspend()
{
    SAPDBTRACE_METHOD_DEBUG( "Join_InvSelectIterator::Suspend", Join_Trace, 1 );
    m_suspended = true;
    m_lock.Unlock();
    SAPDBTRACE_WRITELN( Join_Trace, 1, "Suspend Coordinator");
    vsuspend ( m_coordinatorTrans.trTaskId_gg00, SUSPEND_REASON_INV_SEL_ITER_SUSPEND); 
}

//-------------------------------------------------------------------------------

tgg00_VoidPtr k742CreateInvIterator (
    tgg00_MessBlock          VAR_VALUE_REF  m,
    tgg00_SelectFieldsParam  VAR_VALUE_REF  sel,
    tgg07_select_param       VAR_VALUE_REF  selrec)
{
    SAPDBTRACE_ROUTINE_DEBUG ("k742CreateInvIterator", Join_Trace, 1 );
    if ( g01optimize_parallel_server() > 0 )
    {
        SQLMan_MessBlock* pMessBlock = reinterpret_cast<SQLMan_MessBlock*>(&m);
        Join_InvSelectIterator* pIterator = new(SQLMan_Context::GetContext()->GetAllocator()) 
            Join_InvSelectIterator(*m.mb_trns(), *pMessBlock, selrec.selr().selr_inv_id, sel, 
                    m.mb_qual()->mstack_desc(), g01optimize_parallel_server());
        return pIterator;
    }
    return 0;
}

//-------------------------------------------------------------------------------

void k742CloseIterator (tgg00_VoidPtr pIter)
{
    SAPDBTRACE_ROUTINE_DEBUG ("k742CloseIterator", Join_Trace, 1 );
    Join_InvSelectIterator* pIterator = reinterpret_cast<Join_InvSelectIterator*>(pIter);
    pIterator->Close();
}

//-------------------------------------------------------------------------------

void k742DestroyIterator (tgg00_VoidPtr pIter)
{
    SAPDBTRACE_ROUTINE_DEBUG ("k742DestroyIterator", Join_Trace, 1 );
    Join_InvSelectIterator* pIterator = reinterpret_cast<Join_InvSelectIterator*>(pIter);
    destroy (pIterator, SQLMan_Context::GetContext()->GetAllocator());
}

//-------------------------------------------------------------------------------

tgg00_BasisError k742Next (tgg00_VoidPtr pIter)
{
    SAPDBTRACE_ROUTINE_DEBUG ("k742Next", Join_Trace, 1 );
    Join_InvSelectIterator* pIterator = reinterpret_cast<Join_InvSelectIterator*>(pIter);
    void* pBuf = 0;
    return pIterator->NextRow(pBuf);
}

//-------------------------------------------------------------------------------

void k742OpenIterator(tgg00_VoidPtr                           pIter,
                      tgg00_SelectFieldsParam  VAR_VALUE_REF  sel,
                      tgg07_select_param       VAR_VALUE_REF  selrec,
                      tgg00_BdInvSet           VAR_ARRAY_REF  bdInvSet)
{
    SAPDBTRACE_ROUTINE_DEBUG ("k742OpenIterator", Join_Trace, 1 );
    Join_InvSelectIterator* pIterator = reinterpret_cast<Join_InvSelectIterator*>(pIter);
    sel.sfp_m_result_addr() = reinterpret_cast<tsp00_MoveObjPtr>(&selrec.selr_selectbuffer().rbuf);
    sel.sfp_m_result_size() = sizeof (selrec.selr_selectbuffer().rbuf);
    (void) pIterator->Open (selrec.selr().selr_invkey, selrec.selr().selr_startkey, selrec.selr().selr_stopkey, bdInvSet);
}
