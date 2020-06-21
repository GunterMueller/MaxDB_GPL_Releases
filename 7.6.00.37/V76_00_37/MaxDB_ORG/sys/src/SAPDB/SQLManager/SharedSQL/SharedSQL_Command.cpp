/*!
    @file     SharedSQL_Command.cpp
    @ingroup  SharedSQL
    @author   DirkT
    @brief    SQLCommand cached by the SharedSQL_CommandCache
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

#include "SQLManager/SharedSQL/SharedSQL_Command.hpp"
#include "SQLManager/SharedSQL/SharedSQL_CommandCache.hpp"


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

//----------------------------------------------------------------------------

const SharedSQL_SQLStmt SharedSQL_CachedCommand::GetSQLStmt( void )
{
	if ( mFlgExtern )
	{
		// this routine is only called by SharedSQL_ExecuteHandle::GetSQLStmt
		// if there is an execute handle SharedSQL_CommandCache::CleanUpCommand will not call UnloadSQLStmt
		// BUT SharedSQL_CommandCache::NewPrepareHandle may also try to call ReloadSQLStmt

		SharedSQL_LockedScope Lock(mpPrepareLock, SharedSQL_LockedScope::Exclusive); // DDT : ???
		if ( mFlgExtern ) 
		{
			SQLMan_Context* pContext = (SQLMan_Context*) vGetAcvPtrFromCurrentTask();
			tgg00_TransContext& TransContext = pContext->TransContext();
			mCommandCache.ReloadSQLStmt(TransContext, this);
		}
	}
   	return (mSQLStmt);
}

const bool SharedSQL_CachedCommand::ReloadStatementIntoBuffer( void* Buffer, SAPDB_UInt8 BufferSize )
{
	if (mFlgExtern) 
	{
		SharedSQL_LockedScope Lock(mpCommandLock, SharedSQL_LockedScope::Exclusive);
		if (mFlgExtern) 
		{
			SQLMan_Context* pContext = (SQLMan_Context*) vGetAcvPtrFromCurrentTask();
			tgg00_TransContext& TransContext = pContext->TransContext();
			return mCommandCache.ReloadSQLStmtIntoBuffer(Buffer, BufferSize, TransContext, this);
		}
		return false;
	}
	return true;
}

void SharedSQL_CachedCommand::GetCommandInfoReset( SharedSQL_CommandInfo& I )
{
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_CachedCommand::GetStatisticsReset", SharedSQL_Trace, 5);

    I.mStatistics   = mStatisticsReset;
    I.mResetTime    = mCommandCache.GetCommandStatisticsResetTime();
    FillCommandInfo(I);
}

//----------------------------------------------------------------------------

void SharedSQL_CachedCommand::IncCacheAccessCountExecute( void ) { mCommandCache.IncAccessCountExecute(); }
void SharedSQL_CachedCommand::IncCacheSuccessfulAccessCountExecute( void ) { mCommandCache.IncSuccessfulAccessCountExecute(); }
void SharedSQL_CachedCommand::IncCacheUnSuccessfulAccessCountExecute( void ) { mCommandCache.IncUnSuccessfulAccessCountExecute(); }

//----------------------------------------------------------------------------