/*!
    @file     SharedSQL_CommandCache.hpp
    @ingroup  SharedSQL
    @author   DirkT
    @brief    Cache for SQLCommands, shared
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

#ifndef SHAREDSQL_COMMANDCACHE_HPP
#define SHAREDSQL_COMMANDCACHE_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SQLManager/SharedSQL/SharedSQL_CommandCacheStatistics.hpp"
#include "SQLManager/SharedSQL/SharedSQL_CommandCacheIterator.hpp"
#include "SQLManager/SharedSQL/SharedSQL_Types.hpp"

#include "SAPDBCommon/SAPDB_Singleton.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/Algorithms/SAPDBAlgo_FNVHash.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_SynchronizedRawAllocator.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDB/Container/Container_Vector.hpp"

#include "RunTime/Synchronisation/RTESync_RWRegion.hpp"
#include "RunTime/System/RTESys_AtomicOperation.hpp"
#include "RunTime/System/RTESys_MicroTime.h"

#include "ggg00.h"

class SQLMan_Context;
class SharedSQL_CachedCommand;
class SharedSQL_PrepareHandle;
class Msg_List;

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/// State of the temporary file (NoFile, FileError, FileOk)
enum SharedSQL_TempFileState { NoFile, FileError, FileOk };

/// ...
#define SharedSQL_OldLRUPart    4   // use 1/4 = 25% of the LRU list for OLD commands

/*---------------------------------------------------------------------------*/
/*!
    @class          SharedSQL_CommandCache
    @brief          Shared cache to store SharedSQL_CachedCommand
 */
class SharedSQL_CommandCache : public SAPDB_Singleton, public SAPDBMem_IRawAllocator
{
private:
    /*!
        @brief      Private constructor
        @param      KBSize [in] - Size of the cache in kByte
		@param      ExpectedStatements [in] - Number of statements to cache expected
        @param      Slots  [in] - Number of slots within the hashtable
        @param      SpinLockPoolSlotsSize  [in] - Number of spinlocks to be used by ReaderWriterLocks for the hash table
        @param      SpinLockPoolCommandsSize  [in] - Number of spinlocks to be used by ReaderWriterLocks for the cached commands
		@param      MsgList [out] - MessageList
    */
	SharedSQL_CommandCache( SAPDB_ULong KBSize, SharedSQL_HashValue ExpectedStatements, SharedSQL_HashValue Slots, 
                            SharedSQL_HashValue SpinLockPoolSlotsSize, SharedSQL_HashValue SpinLockPoolCommandsSize,
							Msg_List& MsgList );
public:
    /*!
        @brief  Returns a pointer to the singleton instance of the command cache (may be null)
        @param  Create [in] - If 'Create' is TRUE the instance will be created if it does not exsists
        @return SharedSQL_CommandCache*
    */
    static SharedSQL_CommandCache* Instance( SAPDB_Bool Create );

    /*!
        @brief      Destructor
    */
    ~SharedSQL_CommandCache( void );

    // Begin - Methods derived from SAPDBMem_IRawAllocator
    /*! @copydoc SAPDBMem_IRawAllocator::Allocate() */
    virtual void* Allocate( SAPDB_ULong ByteCount );
    /*! @copydoc SAPDBMem_IRawAllocator::Allocate() */
    virtual void* Allocate( SAPDB_ULong ByteCount, const void* Hint );
    /*! @copydoc SAPDBMem_IRawAllocator::Deallocate() */
    virtual void  Deallocate( void* p );
    /*! @copydoc SAPDBMem_IRawAllocator::GetBaseAllocator() */
    virtual SAPDBMem_RawAllocator* GetBaseAllocator( void );
    /*! @copydoc SAPDBMem_IRawAllocator::GetBaseAllocatorCallStatistics() */
    virtual void GetBaseAllocatorCallStatistics( SAPDB_ULong &CountAlloc, SAPDB_ULong &CountDealloc ) const;
    /*! @copydoc SAPDBMem_IRawAllocator::GetCallStatistics() */
    virtual void GetCallStatistics( SAPDB_ULong &CountAlloc, SAPDB_ULong &CountDealloc ) const;
    /*! @copydoc SAPDBMem_IRawAllocator::GetAllocFailedCount() */
    virtual SAPDB_ULong  GetAllocFailedCount( void ) const;
    // End - Methods derived from SAPDBMem_IRawAllocator

    /*!
        @brief  Free as much memory as possible by destroying unnecessary elements
    */
    void CleanUpAll( void );
    
    /*!
        @brief  Invalidate all commands within the cache
    */
    void Invalidate( bool NotifyApplication );

    /*!
        @brief  Invalidate commands that belong to the given schema within the cache
    */
	void Invalidate( SQLMan_Context& Context, SharedSQL_Schema Schema, bool NotifyApplication );
    /*!
        @brief  Invalidate commands that belong to tables within the cache
    */
    void Invalidate( SQLMan_Context& Context, SharedSQL_TableTree* Tables, bool NotifyApplication );

    /*!
        @brief  Get an interface to a SharedSQL_PrepareHandle for some SharedSQL_CachedCommand
        @param  Alloc      [in]
        @param  Context    [in]
        @param  Stmt       [in]
        @param  Internal   [in]
		@param  ParseAgain [in]
        @param  MsgList    [out]
        @return SharedSQL_PrepareHandle*
    */
	SharedSQL_PrepareHandle*	NewPrepareHandle( SAPDBMem_IRawAllocator& Alloc, SharedSQL_SQLContext& Context, SharedSQL_SQLStmt& Stmt, SAPDB_Bool Internal, SAPDB_Bool ParseAgain, Msg_List& MsgList );

	/*!
        @brief  Destroy the Handle created with NewPrepareHandle
        @param  PH [in]
    */
    void DestroyPrepareHandle( SharedSQL_PrepareHandle* PH );
    /*!
        @brief  ReloadSQLStmt
        @param  TransContext [in]
        @param  Command [in]
        @return ...
    */
	SAPDB_Bool  ReloadSQLStmt( tgg00_TransContext& TransContext, SharedSQL_CachedCommand* Command );
    /// ...
	SAPDB_Bool  ReloadSQLStmtIntoBuffer( void* Buffer, SAPDB_UInt8 RequestSize, tgg00_TransContext& TransContext, SharedSQL_CachedCommand* Command );
    /*!
        @brief  DeleteUnloadedSQLStmt
        @param  TransContext [in]
        @param  Command [in]
        @return ...
    */
	SAPDB_Bool	DeleteUnloadedSQLStmt( tgg00_TransContext& TransContext, SharedSQL_CachedCommand* Command );
    /*!
        @brief  LRUListMoveCommandToTop
        @param  Cmd   [in]
		@param  force [in]
    */
    void LRUListMoveCommandToTop( SharedSQL_CachedCommand* Cmd, bool force );
    /*!
        @brief  GetHashTableEntry
        @param  i [in]
        @return ...
    */
    inline SharedSQL_CachedCommand* GetHashTableEntry(SharedSQL_HashValue i);
    /*!
        @brief  Get information about the CommandCache
        @param  S [out]
    */
    inline void GetCacheInfo( SharedSQL_CommandCacheInfo& S )
    {
        SAPDBTRACE_METHOD_DEBUG ("SharedSQL_CommandCache::GetStatistics", SharedSQL_Trace, 5);
        S   = mCacheInfo;
        S  += mCacheInfoReset;

        SAPDB_ULong BytesUsed, MaxBytesUsed, BytesControlled;
        mAllocator.CalcStatistics(BytesUsed, MaxBytesUsed, BytesControlled);
        S.HashTab_SlotCount = mNumberOfSlots;
        S.UsedSize      = BytesUsed/1024 + (BytesUsed%1024 ? 1 : 0);
        S.MaxUsedSize   = MaxBytesUsed/1024 + (MaxBytesUsed%1024 ? 1 : 0);
        S.UsableSize    = BytesControlled/1024 + (BytesControlled%1024 ? 1 : 0);
    };
    /*!
        @brief  Get information about the CommandCache (reset version)
        @param  S [out]
    */
    inline void GetCacheInfoReset( SharedSQL_CommandCacheInfo& S )
    {
        SAPDBTRACE_METHOD_DEBUG ("SharedSQL_CommandCache::GetStatisticsReset", SharedSQL_Trace, 5);

        S = mCacheInfoReset;
        S.ResetTime = mCommandStatisticsResetTime;
    };
    /*!
        @brief  Reset statistics information of the cache
    */
    inline void ResetCacheInfo( void )
    {
        SAPDBTRACE_METHOD_DEBUG ("SharedSQL_CommandCache::ResetStatistics", SharedSQL_Trace, 5);
        mCacheInfo += mCacheInfo;
        mCacheInfoReset = SharedSQL_CommandCacheInfo();
    };
    /*!
        @brief  Reset statistics information of the commands stored in the cache
    */
    void ResetCommandInfos( void );

    /// ...
    inline SAPDB_UInt8 GetCommandStatisticsResetTime( void )
    {
        return mCommandStatisticsResetTime;
    }
    /*!
        @brief  Calculate the slot to store the command in
        @param  Hash [in]
        @return SharedSQL_HashValue
    */
    inline SharedSQL_HashValue CalcSlot( const SharedSQL_HashValue Hash )
    {
        return Hash%mNumberOfSlots;
    }
    /*!
        @brief  Begin
        @return SharedSQL_CommandCacheIterator
    */
    SharedSQL_CommandCacheIterator Begin();
    /*!
        @brief  End
        @return SharedSQL_CommandCacheIterator
    */
    SharedSQL_CommandCacheIterator End();

    /// ...
	void EnterLockForStatisticsIterator( void );
    /// ...
	void LeaveLockForStatisticsIterator( void );

    /// ...
    inline void IncCountLRUListMoveToTop( void );
    /// ...
    inline void IncAccessCountExecute( void );
    /// ...
    inline void IncSuccessfulAccessCountExecute( void );
    /// ...
    inline void IncUnSuccessfulAccessCountExecute( void );

    /// ...
    friend class SharedSQL_CommandCacheIterator;

private:
	/*!
        @brief  Returns a new unique ID top identify the SharedSQL_CachedCommand
        @return SharedSQL_CommandID
    */
	inline SharedSQL_CommandID	NewCommandID( void );

    /*!
        @brief  Calculates the hashvalue for a given statement and context
        @param  Context [in] 
        @param  Stmt    [in]
        @return SharedSQL_HashValue
    */
    inline SharedSQL_HashValue	CalcHashValue( const SharedSQL_SQLContext& Context, const SharedSQL_SQLStmt& Stmt );

    /*!
        @brief  Cleans a given Command. Dependend on certain counters the plan is droped, the statement unloaded to a temoray file or the command removed from the cache and completely deleted
        @param  TransContext [in] - The transaction context; necessary to handle the temporary file
        @param  Command      [in]
        @param  FlgUnload    [in] - The statement string may be unloaded to a temporary file if'FlgUnload' is TRUE
        @return TRUE is the Command still exists, FALSE if the command was deleted 
    */
    SAPDB_Bool CleanUpCommand( tgg00_TransContext& TransContext, SharedSQL_CachedCommand* Command, SAPDB_Bool FlgUnload, SAPDB_UInt4 LRUPos );

    /*!
        @brief  Unloads the statement strinf of the given command to a temorary file
        @param  TransContext [in] - The transaction context; necessary to handle the temporary file
        @param  Command      [in]
        @return TRUE if succesfull
    */
	SAPDB_Bool UnloadSQLStmt( tgg00_TransContext& TransContext, SharedSQL_CachedCommand* Command );

    /*!
        @brief  Creates the temporary file if it does not exist
        @param  TransContext [in] - The transaction context; necessary to handle the temporary file
        @return TRUE if succesfull
    */
    SAPDB_Bool CreateTempFile( tgg00_TransContext& TransContext );
    
    /*!
        @brief  Returns the ID for the temporary file
        @param  TransContext [in] - The transaction context; necessary to handle the temporary file
        @param  FileID       [out]
    */
    void GetTempFileID( tgg00_TransContext& TransContext, tgg00_FileId& FileID );
	
	/// ...
	const int GetLKeyLen( void ) const;

	/// ...
	const int GetUsableRecLen( void ) const;

    /*!
        @brief  Returns the LKey for the data to be inserted, read or deleted
        @param  ID     [in]
        @param  PartNo [in]
        @param  Key    [out]
        @param  KeyLen [out]
    */
    void GetLKey(SharedSQL_CommandID ID, int PartNo, tgg00_Lkey& Key, int& KeyLen);
	
    /*!
        @brief  WriteSQLStmtToTempFile
        @param  TransContext [in] - The transaction context; necessary to handle the temporary file
        @param  ID           [in]
        @param  Stmt         [in]
        @return TRUE if succesfull
    */
    SAPDB_Bool WriteSQLStmtToTempFile( tgg00_TransContext& TransContext, SharedSQL_CommandID ID, SharedSQL_SQLStmt* Stmt );
	
    /*!
        @brief  ReadSQLStmtFromTempFile
        @param  TransContext [in] - The transaction context; necessary to handle the temporary file
        @param  ID           [in]
        @param  Stmt         [in]
        @return TRUE if succesfull
    */
    SAPDB_Bool ReadSQLStmtFromTempFile( tgg00_TransContext& TransContext, SharedSQL_CommandID ID, SharedSQL_SQLStmt* Stmt );
    SAPDB_Bool ReadSQLStmtFromTempFile( tgg00_TransContext& TransContext, SharedSQL_CommandID ID, void* Buffer, SAPDB_UInt8 RequestSize );
	
    /*!
        @brief  DeleteSQLStmtFromTempFile
        @param  TransContext [in] - The transaction context; necessary to handle the temporary file
        @param  ID           [in]
        @param  Stmt         [in]
        @return TRUE if succesfull
    */
    SAPDB_Bool DeleteSQLStmtFromTempFile( tgg00_TransContext& TransContext, SharedSQL_CommandID ID, SharedSQL_SQLStmt* Stmt );

    /*!
        @brief  Attach the given command to hash table and lru list. Command and lru list must be locked before the method is called.
        @param  Command [in]
    */
//    void AttachCommand( SharedSQL_CachedCommand* Command );
    void AttachCommand( SharedSQL_CachedCommand* Command, SharedSQL_HashValue Slot );

    /*!
        @brief  Detach the given command from hash table and lru list. Command and lru list must be locked before the method is called.
        @param  Command [in]
    */
//    void DetachCommand( SharedSQL_CachedCommand* Command );
    void DetachCommand( SharedSQL_CachedCommand* Command, SharedSQL_HashValue Slot );

    /*!
        @brief  Attach the given command to the hash table. Command must be locked before the method is called.
        @param  Command [in]
        @param  Slot    [in]
    */
    void HashTableAttachCommand( SharedSQL_CachedCommand* Command, SharedSQL_HashValue Slot );

    /*!
        @brief  Detach the given command from the hash table. Command must be locked before the method is called.
        @param  Command [in]
        @param  Slot    [in]
    */
    void HashTableDetachCommand( SharedSQL_CachedCommand* Command, SharedSQL_HashValue Slot );

    /*!
        @brief  Attach the given command to the lru list. LRU list must be locked before the method is called.
        @param  Command [in]
    */
	void LRUListAttachCommand( SharedSQL_CachedCommand* Command );
	
    /*!
        @brief  Detach the given command from the lru list. LRU list must be locked before the method is called.
        @param  Command [in]
    */
	void LRUListDetachCommand( SharedSQL_CachedCommand* Command );

	inline bool CompletelyConstructed( void )
	{
		return !mErrorInitialising;
	}

    // ...
    SAPDBMem_SynchronizedRawAllocator   mAllocator;
    /// Pointer to the singleton instance if the SharedSQL_CommandCache
    static SharedSQL_CommandCache*      mInstance; 
    /// CommandStatisticsResetTime
    SAPDB_UInt8                         mCommandStatisticsResetTime;
    /// Flag to indicate if errors occured during initializing the cache instance
    SAPDB_Bool                          mErrorInitialising;
    /// Status of the temporary file. May be NoFile, FileError or FileOk)
    volatile SharedSQL_TempFileState    mTempFileState;
    //
    /// Counter to be used to generate unique ID's
	SharedSQL_CommandID		            mActCommandID;
    /// Number of slots within the hash table
    SharedSQL_HashValue					mNumberOfSlots;
    /// Pointer to the hash table
	SharedSQL_CachedCommand**	        mHashTable;
	//
    /// Spinlock pool used by ReaderWriter Regions for the hash table
    RTESync_SpinlockPool                mSpinLockPoolForSlots;
    /// Spinlock pool used by ReaderWriter Regions for the cached commands
    RTESync_SpinlockPool                mSpinLockPoolForCommands;
    /// Spinlock pool used by ReaderWriter Regions for other purpose
    RTESync_SpinlockPool                mSpinLockPoolForOthers;
    /// ...
    RTESync_IRWRegion**                 mpSlotLocks;        
    /// ...
    RTESync_IRWRegion*                  mpLRULock;    
	/// ...
    RTESync_IRWRegion*                  mpCleanUpLock;     
    /// ...
    RTESync_IRWRegion*                  mpCreateFileLock;     
    //
    /// Pointer to the first command within the lru list
	SharedSQL_CachedCommand*	        mFirstLRUCommand;
    /// Pointer to the last command within the lru list
	SharedSQL_CachedCommand*	        mLastLRUCommand;
    /// Pointer to the command separating OLD and NEW part of the LRU list
    SharedSQL_CachedCommand*            mMiddleLRUCommand;
    /// Number of elements within the LRU list;
    SAPDB_Int4                          mLRUCounter;
    /// Expected Statements;
    SAPDB_Int4                          mExpectedStmtCnt;
    /// Statistical information  about the command cache (used by SystemView COMMANDCACHESTATISTICS)
    SharedSQL_CommandCacheInfo	        mCacheInfo;
    /// Reset version of statistical information  (used by SystemView COMMANDCACHESTATISTICSRESET)
    SharedSQL_CommandCacheInfo	        mCacheInfoReset;
    /// ID of the temporary table
    tgg00_Surrogate                     mCacheTableId;
	/// ...
	int                                 mWaitAfterFullCnt;
    /// ...
    static SAPDB_Bool                   mCreateError;
    //
};  // SharedSQL_CommandCache

#endif  /* SHAREDSQL_COMMANDCACHE_HPP */

//----------------------------------------------------------------------------

inline SharedSQL_CommandID SharedSQL_CommandCache::NewCommandID( void )
{
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_CommandCache::NewCommandID", SharedSQL_Trace, 8);
    return RTESys_AtomicModify((SAPDB_UInt8&)mActCommandID, (SAPDB_Int8)1);
}

inline SharedSQL_HashValue	SharedSQL_CommandCache::CalcHashValue( const SharedSQL_SQLContext& Context, const SharedSQL_SQLStmt& Stmt )
{
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_CommandCache::CalcHashValue", SharedSQL_Trace, 8);
    SAPDBERR_ASSERT_ARGUMENT( Stmt.mStmt != 0 && Stmt.mStmtSize > 0 );

    SharedSQL_HashValue Hash = FNV_32_INIT;
    FNV32Hash((void*)Stmt.mStmt, Stmt.mStmtSize, Hash); 
	if ( Stmt.mDescriptionSize > 0 )
		FNV32Hash((void*)Stmt.mDescription, Stmt.mDescriptionSize, Hash); 
    FNV32Hash((void*)&Context.UserID, sizeof(Context.UserID), Hash);
    FNV32Hash((void*)&Context.DBProc, sizeof(Context.DBProc), Hash);
    return Hash;
}

inline void SharedSQL_CommandCache::IncCountLRUListMoveToTop( void )              
{ 
    ++mCacheInfoReset.LRUListMoveToTopCount; 
}

inline void SharedSQL_CommandCache::IncAccessCountExecute( void )              
{ 
    ++mCacheInfoReset.AccessCountExecute; 
}

inline void SharedSQL_CommandCache::IncSuccessfulAccessCountExecute( void )    
{ 
    ++mCacheInfoReset.SuccessfulAccessCountExecute; 
}

inline void SharedSQL_CommandCache::IncUnSuccessfulAccessCountExecute( void )  
{ 
    ++mCacheInfoReset.UnSuccessfulAccessCountExecute; 
}

inline SharedSQL_CachedCommand* SharedSQL_CommandCache::GetHashTableEntry(SharedSQL_HashValue i)
{
    return mHashTable[i];
}

//----------------------------------------------------------------------------
