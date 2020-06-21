/*!
    @file     SharedSQL_SQLCache.cpp
    @ingroup  SharedSQL
    @author   DirkT
    @brief    SQLCache for calling SharedSQL from SQLManager
    @see            

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

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SQLManager/SharedSQL/SharedSQL_SQLCache.hpp"
#include "SQLManager/SharedSQL/SharedSQL_CommandCache.hpp"
#include "SQLManager/SharedSQL/SharedSQL_CommandCacheStatistics.hpp"
#include "SQLManager/SharedSQL/SharedSQL_CommandCacheIterator.hpp"

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"

#include "SAPDB/SAPDBCommon/Messages/SDBMsg_SharedSQL.h"

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

//----------------------------------------------------------------------------
/*!
    @brief       constructor
    @param       Alloc        [in]
*/
SharedSQL_SQLCache::SharedSQL_SQLCache( SAPDBMem_IRawAllocator& Alloc ) :
//        mCommandCache(0),
        mParseIDCache(Alloc)
{
}

//----------------------------------------------------------------------------
/*!
    @brief       destructor - does nothing
*/
SharedSQL_SQLCache::~SharedSQL_SQLCache( void )
{
}

SharedSQL_PrepareHandle* SharedSQL_SQLCache::NewPrepareHandle( SAPDBMem_IRawAllocator& Alloc, SharedSQL_SQLContext& Context, SharedSQL_SQLStmt& Stmt, SAPDB_Bool Internal, SAPDB_Bool ParseAgain, Msg_List& MsgList )
{
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_SQLCache::GetPrepareHandle", SharedSQL_Trace, 8);

	if (SharedSQL_CommandCache* CommandCache = SharedSQL_CommandCache::Instance(SAPDB_TRUE))
        return CommandCache->NewPrepareHandle(Alloc, Context, Stmt, Internal, ParseAgain, MsgList);
    else
        return 0;
}

void SharedSQL_SQLCache::DestroyPrepareHandle( SharedSQL_PrepareHandle* PH )
{
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_SQLCache::DestroyPrepareHandle", SharedSQL_Trace, 8);

	if (SharedSQL_CommandCache* CommandCache = SharedSQL_CommandCache::Instance(SAPDB_FALSE))
      	CommandCache->DestroyPrepareHandle(PH);
    else
    {
        // this must never happen !!!
        RTE_Crash( Msg_List ( Msg_List::Error,
                              SDBMSG_SHAREDSQL_ILLEGAL_OPERATION ));
    }
}

SharedSQL_ExecuteHandle* SharedSQL_SQLCache::NewExecuteHandle( SAPDBMem_IRawAllocator& Alloc, const SharedSQL_ParseID& PID, bool CountExecuting, SAPDB_Bool& Valid, SAPDB_Bool& M9, const SharedSQL_ModuleName* &pModulename, Msg_List& MsgList )
{
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_SQLCache::GetExecuteHandle", SharedSQL_Trace, 8);

	return mParseIDCache.NewExecuteHandle(Alloc, PID, CountExecuting, Valid, M9, pModulename, MsgList);
}

void SharedSQL_SQLCache::DestroyExecuteHandle( SharedSQL_ExecuteHandle* EH  )
{
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_SQLCache::DestroyExecuteHandle", SharedSQL_Trace, 8);

	mParseIDCache.DestroyExecuteHandle(EH);
}

SAPDB_Bool SharedSQL_SQLCache::PutParseID( const SharedSQL_PrepareHandle* PH, const SharedSQL_ParseID& PID, const SharedSQL_ModuleName& ModuleName, const SAPDB_Bool M9, Msg_List& MsgList )
{
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_SQLCache::PutParseID", SharedSQL_Trace, 8);

	return mParseIDCache.PutParseID(PH, PID, ModuleName, M9, MsgList);
}

SAPDB_Bool SharedSQL_SQLCache::DropParseID( const SharedSQL_ParseID& PID )
{
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_SQLCache::DropParseID", SharedSQL_Trace, 8);

	return mParseIDCache.DropParseID(PID);
}

const SharedSQL_ModuleName* SharedSQL_SQLCache::GetModuleName( const SharedSQL_ParseID& PID )
{
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_SQLCache::GetModuleName", SharedSQL_Trace, 8);

	return mParseIDCache.GetModuleName(PID);
}

SAPDB_Bool SharedSQL_SQLCache::SetExecuteStatisticData(
                                    const SharedSQL_ParseID& PID,
                                    SharedSQL_CommandID& CommandID,
                                    SAPDB_UInt8 ExecuteTimeSec, SAPDB_UInt8 ExecuteTimeMicroSec,
                                    SAPDB_Int8 ReadRowCount, SAPDB_Int8 QualifiedRowCount,
                                    SAPDB_Int8 VirtualReadCount, SAPDB_Int8 PhysicalReadCount,
                                    SAPDB_Int8 FetchRowCount, 
                                    SAPDB_Int8 SuspendCount, SAPDB_Int8 WaitCount )
{
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_SQLCache::SetExecuteStatisticData", SharedSQL_Trace, 8);

    return mParseIDCache.SetExecuteStatisticData(
                                        PID, CommandID,
                                        ExecuteTimeSec, ExecuteTimeMicroSec,
                                        ReadRowCount, QualifiedRowCount,
                                        VirtualReadCount, PhysicalReadCount,
                                        FetchRowCount, 
                                        SuspendCount, WaitCount);
}

void SharedSQL_SQLCache::GetCommandCacheInfo( SharedSQL_CommandCacheInfo& S )
{
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_SQLCache::GetCommandCacheInfo", SharedSQL_Trace, 8);

	if (SharedSQL_CommandCache* CommandCache = SharedSQL_CommandCache::Instance(SAPDB_FALSE))
        CommandCache->GetCacheInfo(S);
    else
        S = SharedSQL_CommandCacheInfo();
}

void SharedSQL_SQLCache::GetCommandCacheInfoReset( SharedSQL_CommandCacheInfo& S )
{
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_SQLCache::GetCommandCacheInfoReset", SharedSQL_Trace, 8);

	if (SharedSQL_CommandCache* CommandCache = SharedSQL_CommandCache::Instance(SAPDB_FALSE))
        CommandCache->GetCacheInfoReset(S);
    else
    {
        S = SharedSQL_CommandCacheInfo();
        S.ResetTime = 0;
    }
}

void SharedSQL_SQLCache::ResetCommandCacheInfo( void )
{
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_SQLCache::ResetCommandCacheInfo", SharedSQL_Trace, 8);

	if (SharedSQL_CommandCache* CommandCache = SharedSQL_CommandCache::Instance(SAPDB_FALSE))
        CommandCache->ResetCacheInfo();
}

void SharedSQL_SQLCache::ResetCommandInfos( void )
{
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_SQLCache::ResetCommandCacheInfo", SharedSQL_Trace, 8);

	if (SharedSQL_CommandCache* CommandCache = SharedSQL_CommandCache::Instance(SAPDB_FALSE))
        CommandCache->ResetCommandInfos();
}

SharedSQL_CommandCacheIterator SharedSQL_SQLCache::GetCommandCacheIterator( void )
{
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_SQLCache::GetCommandCacheIterator", SharedSQL_Trace, 8);

	if (SharedSQL_CommandCache* CommandCache = SharedSQL_CommandCache::Instance(SAPDB_FALSE))
        return CommandCache->Begin();
    else
        return SharedSQL_CommandCacheIterator();
}

void SharedSQL_SQLCache::EnterLockForStatisticsIterator( void )
{
	if (SharedSQL_CommandCache* CommandCache = SharedSQL_CommandCache::Instance(SAPDB_FALSE))
	{
		SAPDB_UInt2& N = mParseIDCache.LocksForCommandCacheIterator();
		if ( !N )
		{
			++N;
			CommandCache->EnterLockForStatisticsIterator();
		}
	}
}

void SharedSQL_SQLCache::LeaveLockForStatisticsIterator( void )
{
	if (SharedSQL_CommandCache* CommandCache = SharedSQL_CommandCache::Instance(SAPDB_FALSE))
	{
		SAPDB_UInt2& N = mParseIDCache.LocksForCommandCacheIterator();
		--N;
		if ( !N )
		{
			CommandCache->LeaveLockForStatisticsIterator();
		}
	}
}

void SharedSQL_SQLCache::CleanUpAll( void )
{
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_SQLCache::CleanUpAll", SharedSQL_Trace, 8);

    if (SharedSQL_CommandCache* CommandCache = SharedSQL_CommandCache::Instance(SAPDB_FALSE))
        CommandCache->CleanUpAll();
}

void SharedSQL_SQLCache::Invalidate( bool NotifyApplication )
{
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_SQLCache::Invalidate", SharedSQL_Trace, 8);

    if (SharedSQL_CommandCache* CommandCache = SharedSQL_CommandCache::Instance(SAPDB_FALSE))
        CommandCache->Invalidate(NotifyApplication);
}

void SharedSQL_SQLCache::Invalidate( SQLMan_Context& Context, SharedSQL_Schema Schema, bool NotifyApplication )
{
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_SQLCache::Invalidate", SharedSQL_Trace, 8);

    if (SharedSQL_CommandCache* CommandCache = SharedSQL_CommandCache::Instance(SAPDB_FALSE))
        CommandCache->Invalidate(Context, Schema, NotifyApplication);
}

void SharedSQL_SQLCache::Invalidate( SQLMan_Context& Context, SharedSQL_TableTree* Tables, bool NotifyApplication )
{
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_SQLCache::Invalidate", SharedSQL_Trace, 8);

    if (SharedSQL_CommandCache* CommandCache = SharedSQL_CommandCache::Instance(SAPDB_FALSE))
        CommandCache->Invalidate(Context, Tables, NotifyApplication);
}

//----------------------------------------------------------------------------
