/*!
    @file     SharedSQL_CommandCacheStatistics.hpp
    @ingroup  SharedSQL
    @author   DirkT
    @brief    Information about the CommandCache
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

#ifndef SHAREDSQL_COMMANDCACHEINFO_HPP
#define SHAREDSQL_COMMANDCACHEINFO_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"

#include <memory.h>

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*---------------------------------------------------------------------------*/
/*!
    @class          SharedSQL_CommandCacheInfo
    @brief          statistics for the CommandCache
 */
class SharedSQL_CommandCacheInfo
{
public:
    /*!
        @brief  Constructor
    */
    SharedSQL_CommandCacheInfo( void ) :
        UsableSize(0),
        UsedSize(0),
        MaxUsedSize(0),
	    CommandCount(0),
	    MaxCommandCount(0),
	    InsertCommandCount(0),
	    DeleteCommandCount(0),
        AccessCountPrepare(0),
        SuccessfulAccessCountPrepare(0),
        UnSuccessfulAccessCountPrepare(0),
        AccessCountExecute(0),
        SuccessfulAccessCountExecute(0),
        UnSuccessfulAccessCountExecute(0),
        LRUListMoveToTopCount(0),
        FailedAllocateCount(0),
        InvalidateCount(0),
        CleanUpCount(0),
        CleanUpCommandCount(0),
        CurrentUnloadStmtCount(0),
        UnloadStmtCount(0),
        ReloadStmtCount(0),
        DeletePlanCount(0),
        HashTab_SlotCount(0),
        HashTab_UsedSlotCount(0),
        HashTab_MaxUsedSlotCount(0),
	    HashTab_MaxChainEntryCount(0)
    {
    };

    /*!
        @brief  Destructor
    */
    ~SharedSQL_CommandCacheInfo( void ) 
    {
    };

    /*!
        @brief  Operator =
        @param  S [in]
        @return  SharedSQL_CommandCacheInfo&
    */
    SharedSQL_CommandCacheInfo& operator=( const SharedSQL_CommandCacheInfo& S ) 
    {
        SAPDB_MemCopyNoCheck(this, &S, sizeof(*this));
        return *this;
    }

    /*!
        @brief  Operator +=
        @param  S [in]
        @return SharedSQL_CommandCacheInfo&
    */
    SharedSQL_CommandCacheInfo& operator+=( const SharedSQL_CommandCacheInfo& S ) 
    {
        UsableSize                   = S.UsableSize;
        UsedSize                    += S.UsedSize;
        MaxUsedSize                  = (MaxUsedSize > S.MaxUsedSize ? MaxUsedSize : S.MaxUsedSize);
	    CommandCount                += S.CommandCount;
        MaxCommandCount              = (MaxCommandCount > S.MaxCommandCount ? MaxCommandCount : S.MaxCommandCount);
	    InsertCommandCount          += S.InsertCommandCount;
	    DeleteCommandCount          += S.DeleteCommandCount;
        
        AccessCountPrepare              += S.AccessCountPrepare;
        SuccessfulAccessCountPrepare    += S.SuccessfulAccessCountPrepare;
        UnSuccessfulAccessCountPrepare  += S.UnSuccessfulAccessCountPrepare;
        
        AccessCountExecute              += S.AccessCountExecute;
        SuccessfulAccessCountExecute    += S.SuccessfulAccessCountExecute;
        UnSuccessfulAccessCountExecute  += S.UnSuccessfulAccessCountExecute;

        LRUListMoveToTopCount           += S.LRUListMoveToTopCount;

        FailedAllocateCount         += S.FailedAllocateCount;
        InvalidateCount             += S.InvalidateCount;
        CleanUpCount                += S.CleanUpCount;
        CleanUpCommandCount         += S.CleanUpCommandCount;
        CurrentUnloadStmtCount       = S.CurrentUnloadStmtCount;
        UnloadStmtCount             += S.UnloadStmtCount;
        ReloadStmtCount             += S.ReloadStmtCount;
        DeletePlanCount             += S.DeletePlanCount;
        HashTab_SlotCount            = S.HashTab_SlotCount;
        HashTab_UsedSlotCount        = S.HashTab_UsedSlotCount;
        HashTab_MaxUsedSlotCount     = (HashTab_MaxUsedSlotCount > S.HashTab_MaxUsedSlotCount ? HashTab_MaxUsedSlotCount : S.HashTab_MaxUsedSlotCount);
        HashTab_MaxChainEntryCount   = (HashTab_MaxChainEntryCount > S.HashTab_MaxChainEntryCount ? HashTab_MaxChainEntryCount : S.HashTab_MaxChainEntryCount);
        return *this;
    }
    //
    /// ResetTime
    SAPDB_UInt8                         ResetTime;
    /// Usable size of the command cache [kByte]
    SAPDB_Int4                          UsableSize;
    /// Used size of the command cache [kByte]
    SAPDB_Int4                          UsedSize;
    /// Maximum used size of the command cache [kByte]
    SAPDB_Int4                          MaxUsedSize;
    //
    /// Number of commands stored in the command cache (CommandCound should be InsertCommandCount-DeleteCommandCount)
	SAPDB_Int4		                    CommandCount;
    /// Maximum number of commands stored in the command cache
	SAPDB_Int4		                    MaxCommandCount;
    /// Number of commands inserted into the command cache
	SAPDB_Int4		                    InsertCommandCount;
    /// Number of command deleted from the command cache
	SAPDB_Int4		                    DeleteCommandCount;
    //
    /// Counts how often CommandCache::NewPrepareHandle was called 
    SAPDB_Int4                          AccessCountPrepare;
    /// Counts how often the statement was found within the cache when CommandCache::NewPrepareHandle was called and the plan ws PREPARED
    SAPDB_Int4                          SuccessfulAccessCountPrepare;
    /// Counts how often the statement was not found within the cache when CommandCache::NewPrepareHandle was called or the plan was not PREPARED
    SAPDB_Int4                          UnSuccessfulAccessCountPrepare;
    //
    /// Counts how often ExecuteHandle::IsPlanValid was called 
    SAPDB_Int4                          AccessCountExecute;
    /// Counts how often the statement was found within the cache when CommandCache::NewExecuteHandle was called 
    SAPDB_Int4                          SuccessfulAccessCountExecute;
    /// Counts how often the statement was not found within the cache when CommandCache::NewExecuteHandle was called 
    SAPDB_Int4                          UnSuccessfulAccessCountExecute;

    /// Counts how often commands were moved to the top of LRUList
    SAPDB_Int4                          LRUListMoveToTopCount;

    //
    /// Number of memory allocations that failed
    SAPDB_Int4                          FailedAllocateCount;
    /// Number of cache invalidations
    SAPDB_Int4                          InvalidateCount;
    /// Number of "cleanup" operations that had been performed
    SAPDB_Int4                          CleanUpCount;
    /// Number of commands that had been "cleaned"
    SAPDB_Int4                          CleanUpCommandCount;
    //
    /// Number of sql statements that are currently unloaded to the temporary file
    SAPDB_Int4                          CurrentUnloadStmtCount;
    /// Number of unloads to the temporary file
    SAPDB_Int4                          UnloadStmtCount;
    /// Number of reloads from the temporary file
    SAPDB_Int4                          ReloadStmtCount;
    /// Number of execution plan deletions
    SAPDB_Int4                          DeletePlanCount;
    //
    /// Number of slots within the hash table
    SAPDB_Int2		                    HashTab_SlotCount;
    /// Number of used slots within the hash table
    SAPDB_Int2                          HashTab_UsedSlotCount;
    /// Number of maximum used slots within the hash table
    SAPDB_Int2                          HashTab_MaxUsedSlotCount;
    /// Length of the longest chain in a hash table slot
	SAPDB_Int2	                    	HashTab_MaxChainEntryCount;
    //
};  // SharedSQL_CommandCacheInfo


#endif  /* SHAREDSQL_COMMANDCACHEINFO_HPP */
