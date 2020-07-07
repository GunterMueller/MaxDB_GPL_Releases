/*!
  @file        Trans_ExtendedMonitoring.cpp
  @author      Ferdi
  @ingroup     Transaction
  @brief       extended monitoring for long running transaction
  @since       2004-09-18

\if EMIT_LICENCE
  copyright:    Copyright (c) 2001-2005 SAP AG


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

#include "hgg01.h"
#include "hgg08.h"
#include "heo55k.h"     // vbegexcl, vendexcl
#include "gkb05.h"
#include "hkb52.h"
#include "hkb51_2.h"
#include "Transaction/Trans_ExtendedMonitoring.hpp"
#include "Transaction/Trans_ExtendedMonitoringIter.hpp"
#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include "SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "RunTime/Synchronisation/RTESync_RWRegion.hpp"

/// global pointer where the memory structur for transaction monitoring is anchored
static  Trans_ExtendedMonitoring*  g_ExtMonitoringPtr = NULL;

/// global switch to enable/disable transaction monitoring
static  bool                       g_MonitoringWanted = false;

///
Trans_ExtendedMonitoring::Trans_ExtendedMonitoring(SAPDBMem_IRawAllocator& alloc, SAPDB_UInt4 size)
: m_alloc(alloc)
, m_size(size)
, m_next(0)
, m_full(false)
, m_historyList(NULL)
, m_SpinLockPool(NULL)
, m_IRWLockRegion(NULL)
{
    m_SpinLockPool = reinterpret_cast<RTESync_SpinlockPool*>(
        m_alloc.Allocate(sizeof(RTESync_SpinlockPool)) );
    if ( m_SpinLockPool )
    {
        m_SpinLockPool = new(m_SpinLockPool) RTESync_SpinlockPool( UTF8("Trans_ExtendedMonitoring "),1);
        m_IRWLockRegion = RTESync_CreateRWRegion( 1, *m_SpinLockPool, m_alloc);
        m_historyList = reinterpret_cast<Trans_ExtendedMonitoringContext*>(
            m_alloc.Allocate(sizeof(Trans_ExtendedMonitoringContext) * m_size) );
        if ( m_historyList )
        {
            Trans_ExtendedMonitoringContext* p = m_historyList;
            for ( SAPDB_UInt4 i=0; i < m_size; ++i, ++p )
                p  = new(p) Trans_ExtendedMonitoringContext;
        }
    }
};

///
SAPDBMem_IRawAllocator& 
Trans_ExtendedMonitoring::GetAllocator()
{
    return g_ExtMonitoringPtr->m_alloc;
};

Trans_ExtendedMonitoringContext
Trans_ExtendedMonitoring::ReplaceOldest(tsp00_TaskId  TaskId, Trans_ExtendedMonitoringContext &newCtx)
{
    Trans_ExtendedMonitoringContext old;
    m_IRWLockRegion->enter(true, TaskId);

    old.m_transactionId      = newCtx.m_transactionId;
    old.m_taskId             = newCtx.m_taskId,
    old.m_allocatedLen       = m_historyList[m_next].m_allocatedLen;
    old.m_Comment            = m_historyList[m_next].m_Comment;
    m_historyList[m_next]    = newCtx;

    ++m_next;
    if ( m_next == m_size )
    {
        m_next = 0;
        m_full = true;
    }
    m_IRWLockRegion->leave(true, TaskId);
    return old;
};

///
void 
Trans_ExtendedMonitoring::InitializeMonitoring()
{
    SAPDB_UInt4  initval = g01trans_history_size();
    if (initval)
    {
       g_MonitoringWanted = false;
       SAPDBMem_IRawAllocator& allocator = RTEMem_Allocator::Instance();
       g_ExtMonitoringPtr = new(allocator) Trans_ExtendedMonitoring(allocator, initval);
       if ( g_ExtMonitoringPtr && g_ExtMonitoringPtr->m_historyList )
           g_MonitoringWanted = true;
    }
    else
         g_MonitoringWanted = false;
};

///
void 
Trans_ExtendedMonitoring::StartTransaction(tgg00_TransContext&   Trans,  
                                           Trans_ExtendedMonitoringContext*& ExtMon_ContextPtr)
{
    if ( g_MonitoringWanted )
    {
        if ( ! ExtMon_ContextPtr )
        {
            if( RTETask_ITask::User == RTETask_ITask::ByTaskID(Trans.trTaskId_gg00)->Type())
                ExtMon_ContextPtr = new(g_ExtMonitoringPtr->GetAllocator()) Trans_ExtendedMonitoringContext();
        }
        if ( ExtMon_ContextPtr )
        {
            ExtMon_ContextPtr->m_taskId  = Trans.trTaskId_gg00;
        }
    }
};

///
void 
Trans_ExtendedMonitoring::EndTransaction(tsp00_TaskId      TaskId, 
                                         tgg00_TransIndex  TransIndex, 
                                         SAPDB_Bool        committed)
{
    if ( g_MonitoringWanted )
    {
        tkb05_TransEntryPtr ThisTrans;
        k52acc_trans_entry (TransIndex, ThisTrans);
        if ( ThisTrans && ThisTrans->teExtTransMonPtr_kb05 )
        {
            SAPDB_UInt8  endTimeUTC = RTESys_Time();
          
            /* check if monitor data must be preserved */
            tkb05_RegionNo TransRegion = k51tr_index_region(TransIndex);
            vbegexcl (TaskId, g08lock0 + TransRegion);
            k52acc_trans_entry (TransIndex, ThisTrans);
            Trans_ExtendedMonitoringContext& ctx  = *reinterpret_cast<Trans_ExtendedMonitoringContext*>(ThisTrans->teExtTransMonPtr_kb05);
            ctx.m_endTimeUTC         = endTimeUTC;
            ctx.m_writeTransactionId = ThisTrans->teWriteTransId_kb05;
            ctx.m_transactionId      = ThisTrans->teSelectTransId_kb05;
            ctx.m_committed          = committed;

            // check if transaction used a ConsistentView or needed a WriteTransId or called 
            // OmsSetTransactionComment
            // and check if transaction is "long running" 
            if ((ctx.m_inUse == SAPDB_TRUE) &&
                (( endTimeUTC - ctx.m_startTimeUTC ) > g01trans_threshold_value()))
            {
              /* insert into Monitor-list */
                ctx.m_inUse = SAPDB_FALSE;
                ctx = g_ExtMonitoringPtr->ReplaceOldest(TaskId, ctx);
            }
            ctx.m_inUse          = SAPDB_FALSE;
            ctx.m_CommentLength  = 0;
            vendexcl (TaskId, g08lock0 + TransRegion);

        }
    }
};

///
void 
Trans_ExtendedMonitoring::RestartTransaction(Trans_ExtendedMonitoringContext*& ExtMon_ContextPtr)
{
    if ( g_MonitoringWanted && ExtMon_ContextPtr && ( SAPDB_FALSE == ExtMon_ContextPtr->m_inUse ))
    {
        ExtMon_ContextPtr->m_startTimeUTC   = RTESys_Time();
        ExtMon_ContextPtr->m_inUse          = SAPDB_TRUE;
        if ( ExtMon_ContextPtr->m_Comment && ExtMon_ContextPtr->m_allocatedLen )
        {
            memset(&ExtMon_ContextPtr->m_Comment[0], ' ', ExtMon_ContextPtr->m_allocatedLen);
            ExtMon_ContextPtr->m_CommentLength  = 0;
        }
    }
};

///
void 
Trans_ExtendedMonitoring::SetTransactionComment(tsp00_TaskId      TaskId, 
                                                tgg00_TransIndex  TransIndex, 
                                                tsp00_Int2        commentLength, 
                                                SAPDB_Char*       pComment)
{
    if ( g_MonitoringWanted )
    {
        tkb05_TransEntryPtr ThisTrans;
        k52acc_trans_entry (TransIndex, ThisTrans);
        if ( ThisTrans && ThisTrans->teExtTransMonPtr_kb05 )
        {
            /* check if monitor data must be preserved */
            tkb05_RegionNo TransRegion = k51tr_index_region(TransIndex);
            vbegexcl (TaskId, g08lock0 + TransRegion);
            k52acc_trans_entry (TransIndex, ThisTrans);

            Trans_ExtendedMonitoringContext&  ctx  = *reinterpret_cast<Trans_ExtendedMonitoringContext*>(ThisTrans->teExtTransMonPtr_kb05);

            /* Mark Transaction as active, if it isn't already active */
            if ( SAPDB_FALSE == ctx.m_inUse )
            {
                ctx.m_startTimeUTC = RTESys_Time();
                ctx.m_inUse        = SAPDB_TRUE;
            }

            if ( Trans_ExtendedMonitoringContext::ExtMon_CommentLength > commentLength )
            {
                if ( ctx.m_allocatedLen < commentLength )
                {
                    SAPDB_Char* p = reinterpret_cast<SAPDB_Char*>(
                        Trans_ExtendedMonitoring::GetAllocator().Allocate(commentLength));
                    if (p) 
                    {
                        ctx.m_allocatedLen = commentLength;
                        if ( ctx.m_Comment && ctx.m_CommentLength )
                        {
                            Trans_ExtendedMonitoring::GetAllocator().Deallocate(ctx.m_Comment);
                        }
                        ctx.m_Comment = p;
                    }
                }
                if ( ctx.m_allocatedLen >= ( commentLength ))
                {
                    ctx.m_CommentLength = commentLength; 
                    SAPDB_MemCopyNoCheck (&ctx.m_Comment[0], pComment, commentLength);
                }
            }
            vendexcl (TaskId, g08lock0 + TransRegion);
        }
    }
};


void 
Trans_ExtendedMonitoring::GetFirstCtx(tsp00_TaskId TaskId, Trans_ExtendedMonitoringContext &ctx, SAPDB_UInt4 &pos)
{
    pos = Trans_ExtendedMonitoringIter::cInvalidPos;
    if ( g_MonitoringWanted )
    {
        m_IRWLockRegion->enter(true, TaskId);
        if ( !m_full )
        {
            if (0 != m_next)
                pos = 0;
        }
        else
            pos = m_next;

        if ( Trans_ExtendedMonitoringIter::cInvalidPos != pos )
        {
            Trans_ExtendedMonitoringContext *ctx_help = 
                static_cast<Trans_ExtendedMonitoringContext*>(&g_ExtMonitoringPtr->m_historyList[pos]);
            ctx = *ctx_help;
            ctx.m_Comment      = NULL;
            ctx.m_allocatedLen = 0;
            if ( ctx.m_CommentLength > 0 )
            {
                ctx.m_Comment = reinterpret_cast<SAPDB_Char*>(
                    Trans_ExtendedMonitoring::GetAllocator().Allocate(ctx.m_CommentLength));
                if ( ctx.m_Comment )
                {
                    SAPDB_MemCopyNoCheck (&ctx.m_Comment[0], &ctx_help->m_Comment[0], ctx.m_CommentLength);
                    ctx.m_allocatedLen = ctx.m_CommentLength;
                }
                else
                    ctx.m_CommentLength = 0;
            }
        }
        m_IRWLockRegion->leave(true, TaskId);
    }
};

void 
Trans_ExtendedMonitoring::GetNextCtx(tsp00_TaskId TaskId, Trans_ExtendedMonitoringContext &ctx, SAPDB_UInt4 &pos)
{
    if ( g_MonitoringWanted )
    {
        m_IRWLockRegion->enter(true, TaskId);
        ++pos;
        if ( pos == m_size )
            pos = 0;
        if ( pos == m_next )
        {
            pos = Trans_ExtendedMonitoringIter::cInvalidPos;
        }
        else 
        {
            Trans_ExtendedMonitoringContext *ctx_help = 
                static_cast<Trans_ExtendedMonitoringContext*>(&g_ExtMonitoringPtr->m_historyList[pos]);
            ctx.m_transactionId      = ctx_help->m_transactionId;
            ctx.m_taskId             = ctx_help->m_taskId;
            ctx.m_writeTransactionId = ctx_help->m_writeTransactionId;
            ctx.m_startTimeUTC       = ctx_help->m_startTimeUTC;
            ctx.m_endTimeUTC         = ctx_help->m_endTimeUTC;
            ctx.m_committed          = ctx_help->m_committed;
            ctx.m_inUse              = ctx_help->m_inUse;

            if ( ctx.m_allocatedLen < ctx_help->m_CommentLength )
            {
                if ( ctx.m_Comment )
                    Trans_ExtendedMonitoring::GetAllocator().Deallocate(ctx.m_Comment);
                ctx.m_Comment = reinterpret_cast<SAPDB_Char*>(
                    Trans_ExtendedMonitoring::GetAllocator().Allocate(ctx_help->m_CommentLength));
                if ( ctx.m_Comment )
                {
                    ctx.m_CommentLength = ctx_help->m_CommentLength;
                    ctx.m_allocatedLen  = ctx_help->m_CommentLength;
                }
                else
                {
                    ctx.m_CommentLength = 0;
                    ctx.m_allocatedLen  = 0;
                }
            }
            if ( ctx.m_Comment )
                SAPDB_MemCopyNoCheck (&ctx.m_Comment[0], &ctx_help->m_Comment[0], ctx.m_CommentLength);
        }
        m_IRWLockRegion->leave(true, TaskId);
    }
    else
        pos = Trans_ExtendedMonitoringIter::cInvalidPos;
};

 ///
Trans_ExtendedMonitoringContext* 
Trans_ExtendedMonitoring::GetFirstHistoryContext(tsp00_TaskId                     TaskId, 
                                                 Trans_ExtendedMonitoringContext &ctx, 
                                                 SAPDB_UInt4                     &pos)
{
    g_ExtMonitoringPtr->GetFirstCtx(TaskId, ctx, pos);
    if ( pos == Trans_ExtendedMonitoringIter::cInvalidPos )
        return NULL;
    return &ctx;
};

///
Trans_ExtendedMonitoringContext* 
Trans_ExtendedMonitoring::GetNextHistoryContext(tsp00_TaskId                     TaskId, 
                                                Trans_ExtendedMonitoringContext &ctx, 
                                                SAPDB_UInt4                     &pos)
{
     g_ExtMonitoringPtr->GetNextCtx(TaskId, ctx, pos);
    if ( pos == Trans_ExtendedMonitoringIter::cInvalidPos )
        return NULL;
    return &ctx;
};

