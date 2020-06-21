/*!
    @file     SharedSQL_Command.hpp
    @ingroup  SharedSQL
    @author   DirkT
    @brief    SQLCommands cached by the SharedSQL_CommandCache
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

#ifndef SHAREDSQL_COMMAND_HPP
#define SHAREDSQL_COMMAND_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SQLManager/SharedSQL/SharedSQL_CommandInfo.hpp"
#include "SQLManager/SharedSQL/SharedSQL_CommandStatistics.hpp"

#include "SQLManager/SharedSQL/SharedSQL_Plan.hpp"
#include "SQLManager/SharedSQL/SharedSQL_Types.hpp"

#include "SQLManager/SQLMan_Context.hpp"
#include "SQLManager/Catalog/Catalog_Object.hpp"

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"

#include "RunTime/System/RTESys_AtomicOperation.hpp"
#include "RunTime/Synchronisation/RTESync_RWRegion.hpp"

#include "heo58.h"

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

class SharedSQL_CommandCache;

/*---------------------------------------------------------------------------*/
/*!
    @class          SharedSQL_CachedCommand
    @brief          CachedCommand
 */
class SharedSQL_CachedCommand
{
public:
    /*!
        @brief  Constructor
        @param  ID           [in]
        @param  Hash         [in]
        @param  Slot         [in]
        @param  Cache        [in]
        @param  Allocator    [in]
        @param  Context      [in]
        @param  Stmt         [in]
        @param  Internal     [in]
        @param  SpinLockPool [in]
    */
    SharedSQL_CachedCommand( SharedSQL_CommandID ID, SharedSQL_HashValue Hash, SharedSQL_HashValue Slot, 
							    SharedSQL_CommandCache& Cache, SAPDBMem_IRawAllocator& Allocator,
                                SharedSQL_SQLContext& Context, SharedSQL_SQLStmt& Stmt, SAPDB_Bool Internal,
                                RTESync_SpinlockPool& SpinLockPool 
                                ) 
		: mAllocator(Allocator)
		, mCommandCache(Cache)
        , mpCommandLock ( RTESync_CreateRWRegion(2*ID,   SpinLockPool, Allocator) )
        , mpPrepareLock ( RTESync_CreateRWRegion(2*ID+1, SpinLockPool, Allocator) )
        , mNextHashCommand(0)
        , mPrevHashCommand(0)
	    , mNextLRUCommand(0)
	    , mPrevLRUCommand(0)
        , mLRUOldFlg(false)
        , mSQLStmt(Stmt)
        , mpActPlan(0)
	    , mpOldPlans(0)
	    , mCommandID(ID)
		, mFileKey(ID)
	    , mHashValue(Hash)
        , mSlot(Slot)
	    , mStatus(New)
	    , mFlgExtern(SAPDB_FALSE)
	    , mSQLContext(Context)
        , mPrepareCount(0)
        , mParseIDCount(0)
        , mExecuteCount(0)
        , mStatistics()
        , mStatisticsReset()
        , mInternal(Internal)
		, mQueryRewrite(No_Rewrite)
    {
    };

    /*!
        @brief  Destructor
    */
    ~SharedSQL_CachedCommand( void )
	{
		if ( mpCommandLock )
			RTESync_DestroyRWRegion(mpCommandLock, mAllocator);
		if ( mpPrepareLock )
			RTESync_DestroyRWRegion(mpPrepareLock, mAllocator);

		if ( mpActPlan )
			destroy(mpActPlan, mAllocator);
		while ( SharedSQL_Plan* A = mpOldPlans )
		{
			mpOldPlans = mpOldPlans->NextPlan();
			destroy(A, mAllocator);
		}
	}

protected:
    friend class SharedSQL_CommandCache;
	friend class SharedSQL_ParseIDCache;	
	friend class SharedSQL_PrepareHandle;
	friend class SharedSQL_ParseIDHandle;
	friend class SharedSQL_ExecuteHandle;
	friend class SharedSQL_CommandCacheIterator;
	friend class SysView_CommandStatistics;			
	friend class SysView_CommandStatisticsReset;	
	friend class SysView_UnloadedStatements;

    /*!
        @brief  CompletelyConstructed
        @return ok
    */
    inline SAPDB_Bool CompletelyConstructed( void );

    /*!
        @brief  Returns the prepare counter
        @return SAPDB_Int4
    */
    inline SAPDB_Int4 GetPrepareCount();
    /*!
        @brief  Increment prepare counter (only to be used by SharedSQL_PrepareHandle)
    */
    inline void IncPrepareCount( SharedSQL_Plan* P );
    /*!
        @brief  Decrement prepare counter (only to be used by SharedSQL_PrepareHandle)
    */
    inline void DecPrepareCount( SharedSQL_Plan* P );

    /*!
        @brief  Returns the parseid counter
        @return SAPDB_Int4
    */
    inline SAPDB_Int4 GetParseIDCount();
    /*!
        @brief  Increment parseid counter (only to be used by SharedSQL_ParseIDHandle)
    */
//    void IncParseIDCount( SharedSQL_ParseID CompareParseID );
    inline void IncParseIDCount( SharedSQL_Plan* P );
    /*!
        @brief  Decrement parseid counter (only to be used by SharedSQL_ParseIDHandle)
    */
    inline void DecParseIDCount( SharedSQL_Plan*& P );

    /*!
        @brief  Returns the execution
        @return SAPDB_Int4
    */
    inline SAPDB_Int4 GetExecuteCount( void );
    /*!
        @brief  Increment execution counter (only to be used by SharedSQL_ExecuteHandle)
    */
    inline void IncExecuteCount( SharedSQL_Plan* P );
    /*!
        @brief  Decrement execution count (only to be used by SharedSQL_ExecuteHandle)
    */
    inline void DecExecuteCount( SharedSQL_Plan*& P );

    /// ...
	inline SAPDB_Bool HasPreparedPlan( void );

    /// ...
	inline SharedSQL_CommandStatus GetCurrentStatus( void );

    /// ...
	inline void SetStatusPreparing( void );
    /// ...
	inline void SetStatusPrepared( SharedSQL_Plan* P );

    /// ...
	inline void Invalidate( SharedSQL_CommandStatus Status );
    /// ...
	inline void Invalidate( SQLMan_Context& Context, SharedSQL_Schema Schema, SharedSQL_CommandStatus Status );
    /// ...
	inline void Invalidate( SQLMan_Context& Context, SharedSQL_TableTree* Tables, SharedSQL_CommandStatus Status );
    /// ...
	inline void Invalidate( SharedSQL_Plan* P, SharedSQL_CommandStatus Status );

    /// ...
	inline void ClearTableInfo( void );

	/*!
        @brief  Returns FlgExtern that indicated wheather or not the sql statement is unloaded to the temporary file
        @return SAPDB_Byte
    */
    inline SAPDB_Byte GetFlgExtern( void );
    /*!
        @brief  Sets FlgExtern of the command (only to be used the SharedSQL_CommandCache)
        @param  FlgExtern [in]
    */
    inline void SetFlgExtern( SAPDB_Byte FlgExtern );

    /*!
        @brief  Returns FlgLRUListOld that indicated wheather or not the command belongs to the OLD part of the LRU list
        @return SAPDB_Bool
    */
    inline SAPDB_Bool GetFlgLRUListOld( void );
    /// ...
    inline void SetFlgLRUListOld( SAPDB_Bool Flg );

    /*!
        @brief  Returns the sql statement
        @return SharedSQL_SQLStmt&
    */
    inline SharedSQL_SQLStmt&               SQLStmt( void );

    /*!
        @brief  Returns the sql context
        @return SharedSQL_SQLContext&
    */
    inline SharedSQL_SQLContext&            SQLContext( void );

    /*!
        @brief  Returns an interface to the execution plan
        @return SharedSQL_Plan&
    */
	inline SharedSQL_Plan* GetActPlan( void );

    /// ...
    inline SharedSQL_Plan* NewPlan( void );
//    /// ...
//	inline SAPDB_Bool PutPlan( SharedSQL_ParseID MPID, void *const P, SAPDB_ULong Size, SharedSQL_Plan*& pPlan, Msg_List& MsgList );
//    /// ...
//	inline SAPDB_Bool InsertTableInfo(  const SharedSQL_Table& Table, Msg_List& MsgList );
    /// ...
	inline void ErrorWhilePreparing( void );
    /// ...
	inline SAPDB_Bool CleanUpPlans( SAPDB_UInt4 LRUPos );

	/*!
        @brief  Returns an interface to the command cache the command is stored in
        @return SharedSQL_CommandCache&
    */
    inline SharedSQL_CommandCache& CommandCache( void );

    /*!
        @brief  Returns the statistical information. NOTE: SetStatistics() will return a reference to the actual 
		statistics which may be within the mNewerCommand, while Statistics() will alway return a reference to the
		own stastistic information!!!
        @return SharedSQL_CommandStatistics&
    */
    inline SharedSQL_CommandStatistics&     Statistics( void );
    /// ...
    inline SharedSQL_CommandStatistics&     StatisticsReset( void );

    /*!
        @brief  Returns the CommandInfo
        @param  I [out]
    */
    inline void GetCommandInfo( SharedSQL_CommandInfo& I );
    /*!
        @brief  Returns the reet version of the CommandInfo
        @param  I [out]
    */
    /*inline*/ void GetCommandInfoReset( SharedSQL_CommandInfo& I );
    /*!
        @brief  Resets the statistical data
    */
    inline void ResetStatisticsInfo( void );

    /*!
        @brief  Returns the unique ID that identifys the command
        @return SharedSQL_CommandID
    */
    inline SharedSQL_CommandID GetCommandID( void );

    /// ...
	inline SharedSQL_CommandID GetFileKey( void );

	/*!
        @brief  Returns the hash value 
        @return SharedSQL_HashValue
    */
    inline SharedSQL_HashValue	GetHashValue( void );

    /// ...
    inline SharedSQL_HashValue	GetSlot( void );
	/*!
        @brief  Returns wheather or not the command is an internal one
        @return SAPDB_Bool
    */
    inline SAPDB_Bool IsInternal( void );
    /*!
        @brief  Sets FlgInternal of the command (only to be used the SharedSQL_CommandCache)
        @param  Internal [in]
    */
    inline void SetInternal(SAPDB_Bool Internal);

    /// ...
	inline void SetQueryRewrite(SQLMan_QueryRewriteMode QRWDone);

    /*!
        @brief  Returns the sql statement
        @return SharedSQL_SQLStmt
    */
    const SharedSQL_SQLStmt GetSQLStmt( void );

    /// ...
    const bool ReloadStatementIntoBuffer( void* Buffer, SAPDB_UInt8 BufferSize );

	/*!
        @brief  Lock the command
        @param  exclusive [in]
    */
    inline void EnterLockCommand( bool exclusive );
    /// ...
	inline void EnterLockPrepare( bool exclusive );
    /*!
        @brief  Try to lock the command
        @param  exclusive [in]
        @return true, if lock could be entered, else false
    */
    inline bool TryEnterLockCommand( bool exclusive );
    /// ...
    inline bool TryEnterLockPrepare( bool exclusive );
    /*!
        @brief  Leave the lock
        @param  exclusive [in]
    */
    inline void LeaveLockCommand( bool exclusive );
    /// ...
    inline void LeaveLockPrepare( bool exclusive );

    /// ...
	inline void LockPrepareSwitchToNonExclusive( void );

    /*!
        @brief  Returns the next command within the same slot of the hash table
        @return SharedSQL_CachedCommand*
    */
    inline SharedSQL_CachedCommand*        NextHashCommand( void ) const ;

    /// ...
    void IncCacheAccessCountExecute( void );
    /// ...
    void IncCacheSuccessfulAccessCountExecute( void );
    /// ...
    void IncCacheUnSuccessfulAccessCountExecute( void );

    /*!
        @brief  NextHashCommand
        @return
    */
    inline SharedSQL_CachedCommand*&        NextHashCommand( void );

    /*!
        @brief  PrevHashCommand
        @return
    */
    inline SharedSQL_CachedCommand*&        PrevHashCommand( void );

    /*!
        @brief  NextLRUCommand
        @return
    */
    inline SharedSQL_CachedCommand*&        NextLRUCommand( void );

    /*!
        @brief  PrevLRUCommand
        @return
    */
    inline SharedSQL_CachedCommand*&        PrevLRUCommand( void );

private:
    /// ...
	inline void CheckPlanArgument( SharedSQL_Plan* P );
    /// ...
	inline void QueueActPlanToOldPlans( SharedSQL_CommandStatus Status, bool Force=false );
    /// ...
	inline bool CleanUpPlan( SharedSQL_Plan*& P );
    /// ...
	inline bool DeletePlan( SharedSQL_Plan*& P );
    /// ...
	inline void SetStatusForOldPlans( SharedSQL_CommandStatus Status );
    /// ...
	inline void SetStatusForOldPlans( SQLMan_Context& Context, SharedSQL_Schema Schema, SharedSQL_CommandStatus Status );
    /// ...
	inline void SetStatusForOldPlans( SQLMan_Context& Context, SharedSQL_TableTree* Tables, SharedSQL_CommandStatus Status );
    /// ...
    inline void FillCommandInfo( SharedSQL_CommandInfo& I );

    /// ...
	SAPDBMem_IRawAllocator&				mAllocator;

    /// Pointer to the next command within the same hash table slot 
	SharedSQL_CachedCommand*	        mNextHashCommand;
    /// Pointer to the previous command within the same hash table slot 
	SharedSQL_CachedCommand*	        mPrevHashCommand;
    /// Pointer to the next command within the lru list 
	SharedSQL_CachedCommand*	        mNextLRUCommand;
    /// Pointer to the previous command within the lru list 
	SharedSQL_CachedCommand*	        mPrevLRUCommand;
    /// Unique ID to identify the command
	SharedSQL_CommandID					mCommandID;
	/// ...
	SharedSQL_CommandID					mFileKey;
    /// Hash value of the command
	SharedSQL_HashValue					mHashValue;
    /// Hash table slot that the command can be found in
    SharedSQL_HashValue                 mSlot;

    /// Reference to the command cache the command is stored in
	SharedSQL_CommandCache&             mCommandCache;

    /// ...
    RTESync_IRWRegion                   *mpCommandLock;          
    /// ...
    RTESync_IRWRegion                   *mpPrepareLock;          

    /// Status of the command (New, Invalid, Dropped, Preparing or Prepared) 
    volatile SharedSQL_CommandStatus	mStatus;
    /// Flag to indicated wheater or not the statement string is unloaded to the temporary file
	volatile SAPDB_Byte			    	mFlgExtern;
    /// Number of SharedSQL_PrepareHandle that refer to this command; 
    SAPDB_Int4                          mPrepareCount; 	
    /// Number of SharedSQL_ParseIDHandle that refer to this command; if this is >0 the command must not be removed from the cache
    SAPDB_Int4                          mParseIDCount;
    /// Number of SharedSQL_ExecuteHandle that refer to this command; if the is >0 the plan must not be dropped
    SAPDB_Int4                          mExecuteCount;

    /// The SQL statement
    SharedSQL_SQLStmt					mSQLStmt;
    /// The SQL context
	SharedSQL_SQLContext				mSQLContext;
    /// The actual (prepared) execution plan
    SharedSQL_Plan*                     mpActPlan;
	/// Old (invalid or dropped) plans for that parseids still exist
    SharedSQL_Plan*                     mpOldPlans;

    /// Flag to indicate wheather or not the command is an internal one
    SAPDB_Bool                          mInternal;

    /// Flag for LRU list to indicate that this command belongs to the OLD part of the list
    SAPDB_Bool                          mLRUOldFlg;
	/// Information if the statement has been rewritten
	SQLMan_QueryRewriteMode	            mQueryRewrite;

    /// Statistical information about the command (used by SystemView COMMANDSTATISTICS)
    SharedSQL_CommandStatistics        	mStatistics;
    /// Reset version of the statistical information (used by SystemView COMMANDSTATISTICSRESET)
    SharedSQL_CommandStatistics        	mStatisticsReset;
    //
};  // SharedSQL_CachedCommand

/* ------------------------------------------------------------------------- */

inline void SharedSQL_CachedCommand::CheckPlanArgument( SharedSQL_Plan* P )
{
	SAPDBERR_ASSERT_ARGUMENT( P )

#if defined (SAPDB_SLOW)
	if ( P != mpActPlan )
	{
		bool found = false;
		SharedSQL_Plan* X = mpOldPlans;
		while ( X && !found )
		{
			if ( X==P )
				found=true;
			X = X->NextPlan();
		}
		SAPDBERR_ASSERT_STATE( found );
	}
#endif
}

inline SAPDB_Bool SharedSQL_CachedCommand::CompletelyConstructed( void )
{
    return ( mpCommandLock && mpPrepareLock );
}

inline void SharedSQL_CachedCommand::FillCommandInfo( SharedSQL_CommandInfo& I )
{
  	I.mCommandID    = mCommandID;
    I.mInternal     = mInternal;
	I.mHashValue    = mHashValue;
    I.mSlot         = mSlot;
    //
    I.mSQLContext   = mSQLContext;
    //
    I.mStatus       = mStatus;
	I.mFlgExtern    = mFlgExtern;
    I.mPrepareCount = GetPrepareCount();
    I.mParseIDCount = GetParseIDCount();
    I.mExecuteCount = GetExecuteCount();
    //
    I.mStmtSize     = mSQLStmt.mStmtSize;
	I.mDescriptionSize = mSQLStmt.mDescriptionSize;
    I.mStmt         = mSQLStmt.mStmt;   
	//
	EnterLockCommand(0);
	if ( mpActPlan )
		I.mPlanSize     = mpActPlan->GetSize();
	else
		if ( mpOldPlans )
			I.mPlanSize = mpOldPlans->GetSize();
	LeaveLockCommand(0);
	//
	I.mQueryRewrite = mQueryRewrite;
}

inline void SharedSQL_CachedCommand::GetCommandInfo( SharedSQL_CommandInfo& I )
{
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_CachedCommand::GetStatistics", SharedSQL_Trace, 5);

    I.mStatistics   = mStatistics;
    I.mStatistics  += mStatisticsReset;
    FillCommandInfo(I);
}

inline void SharedSQL_CachedCommand::ResetStatisticsInfo( void )
{
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_CachedCommand::ResetStatistics", SharedSQL_Trace, 5);

    mStatistics += mStatisticsReset;
    mStatisticsReset = SharedSQL_CommandStatistics();
}

inline SAPDB_Bool SharedSQL_CachedCommand::HasPreparedPlan( void )
{
	SAPDBERR_ASSERT_STATE( (mStatus==Prepared?(mpActPlan?1:0):1) );
	return ( (mStatus == Prepared) && (mpActPlan) );
}

inline SharedSQL_CommandStatus SharedSQL_CachedCommand::GetCurrentStatus( void )
{
	SAPDBERR_ASSERT_STATE( (mStatus==Prepared?(mpActPlan?1:0):1) );
	return mStatus;
}

inline void SharedSQL_CachedCommand::SetStatusPreparing( void )
{
    SAPDBERR_ASSERT_STATE( mStatus != Preparing );
//	SharedSQL_LockedScope Lock(mpCommandLock, SharedSQL_LockedScope::Exclusive);
	mStatus = Preparing;
}

inline void SharedSQL_CachedCommand::SetStatusPrepared( SharedSQL_Plan* P )
{
	SharedSQL_LockedScope Lock(mpCommandLock, SharedSQL_LockedScope::Exclusive);
	if ( mStatus == Preparing && mpActPlan )
	{
		SHARED_SQL_TRACE ("   SharedSQL_Command SetStatusPrepared");
		SAPDBERR_ASSERT_STATE( P == mpActPlan );

		Catalog_Object::Catalog_Key key;
		SAPDBERR_ASSERT_STATE( mpActPlan->Find(&key) );

		if ( P == mpActPlan )
		{
			mStatus = Prepared;
			mpActPlan->SetStatus(Prepared);
		}
		else
		{
			mStatus = Invalid;
		}
	}
}

inline void SharedSQL_CachedCommand::Invalidate( SharedSQL_CommandStatus Status ) 
{
    SAPDBERR_ASSERT_ARGUMENT( (Status == Dropped) || (Status == Invalid) );

//	SharedSQL_LockedScope Lock(mpCommandLock, SharedSQL_LockedScope::Exclusive);
//  // only in this version of Invalidate the locking is done before calling the method!
    SHARED_SQL_TRACE ("   SharedSQL_Command Invalidate[" << ToStr(mCommandID) << "]");

	if ( Status == Invalid )
		SetStatusForOldPlans(Status);
	QueueActPlanToOldPlans(Status);
}

inline void SharedSQL_CachedCommand::Invalidate( SQLMan_Context& Context, SharedSQL_Schema Schema, SharedSQL_CommandStatus Status ) 
{
    SAPDBERR_ASSERT_ARGUMENT( (Status == Dropped) || (Status == Invalid) );

	SharedSQL_LockedScope Lock(mpCommandLock, SharedSQL_LockedScope::Exclusive);
	if ( Status != Preparing )
	{
		SHARED_SQL_TRACE ("   SharedSQL_Command Invalidate[" << ToStr(mCommandID) << "]");

		if ( Status == Invalid )
			SetStatusForOldPlans(Context, Schema, Status);
		if ( mpActPlan )
		{
			if ( mpActPlan->AffectedBySchema(Context, mSQLContext, Schema) )
				QueueActPlanToOldPlans(Status);
		}
	}
}

inline void SharedSQL_CachedCommand::Invalidate( SQLMan_Context& Context, SharedSQL_TableTree* Tables, SharedSQL_CommandStatus Status ) 
{
    SAPDBERR_ASSERT_ARGUMENT( (Status == Dropped) || (Status == Invalid) );

	SharedSQL_LockedScope Lock(mpCommandLock, SharedSQL_LockedScope::Exclusive);
	if ( Status != Preparing )
	{
		SHARED_SQL_TRACE ("   SharedSQL_Command Invalidate[" << ToStr(mCommandID) << "]");

		if ( Status == Invalid )
			SetStatusForOldPlans(Context, Tables, Status);
		if ( mpActPlan )
		{
			if ( mpActPlan->AffectedByTables(Context, mSQLContext, Tables) )
				QueueActPlanToOldPlans(Status);
		}
	}
}

inline void SharedSQL_CachedCommand::Invalidate( SharedSQL_Plan* P, SharedSQL_CommandStatus Status ) 
{
    SAPDBERR_ASSERT_ARGUMENT( (Status == Dropped) || (Status == Invalid) );

	SharedSQL_LockedScope Lock(mpCommandLock, SharedSQL_LockedScope::Exclusive);
	CheckPlanArgument(P);

	if ( P )
	{
        SHARED_SQL_TRACE ("   SharedSQL_Command InvalidatePlan[" << ToStr(mCommandID) << "] : " << ToStr(P));

		if ( P == mpActPlan )
			QueueActPlanToOldPlans(Status);
		else
			P->SetStatus(Status);
	}
}

inline SAPDB_Int4 SharedSQL_CachedCommand::GetPrepareCount() { return RTESys_AtomicReadInt4(mPrepareCount); }

inline void SharedSQL_CachedCommand::IncPrepareCount( SharedSQL_Plan* P ) 
{ 
//	SharedSQL_LockedScope Lock(mpCommandLock, SharedSQL_LockedScope::Exclusive);	// this is done outside !
	++mPrepareCount; 
	if ( P )
		P->IncParseIDCount();					// we use ParseIDCount here !
}

inline void SharedSQL_CachedCommand::DecPrepareCount( SharedSQL_Plan* P ) 
{ 
//	SharedSQL_LockedScope Lock(mpCommandLock, SharedSQL_LockedScope::Exclusive);	// this is done outside !
	--mPrepareCount; 
	if ( P )
	{
		SAPDB_Int4 _cnt = P->DecParseIDCount();	// we use ParseIDCount here !
		if ( P != mpActPlan )
			CleanUpPlan(P);
	}
}

inline SAPDB_Int4 SharedSQL_CachedCommand::GetParseIDCount()    { return mParseIDCount; }

inline void SharedSQL_CachedCommand::IncParseIDCount( SharedSQL_Plan* P ) 
{ 
	SharedSQL_LockedScope Lock(mpCommandLock, SharedSQL_LockedScope::Exclusive);
	CheckPlanArgument(P);

	++mParseIDCount;
	if ( P )
		P->IncParseIDCount();
}

inline void SharedSQL_CachedCommand::DecParseIDCount( SharedSQL_Plan*& P ) 
{ 
	SharedSQL_LockedScope Lock(mpCommandLock, SharedSQL_LockedScope::Exclusive);
	CheckPlanArgument(P);

	--mParseIDCount;
	if ( P )
	{
		SAPDB_Int4 _cnt = P->DecParseIDCount();
		if ( P != mpActPlan )
			CleanUpPlan(P);
	}
}

inline SAPDB_Int4 SharedSQL_CachedCommand::GetExecuteCount( void )  { return mExecuteCount; }

inline void SharedSQL_CachedCommand::IncExecuteCount( SharedSQL_Plan* P ) 
{ 
	SharedSQL_LockedScope Lock(mpCommandLock, SharedSQL_LockedScope::Exclusive);
	CheckPlanArgument(P);

	++mExecuteCount;
	if ( P )
		P->IncExecuteCount();
}

inline void SharedSQL_CachedCommand::DecExecuteCount( SharedSQL_Plan*& P ) 
{ 
	SharedSQL_LockedScope Lock(mpCommandLock, SharedSQL_LockedScope::Exclusive);
	CheckPlanArgument(P);

	--mExecuteCount;
	if ( P )
	{
		SAPDB_Int4 _cnt = P->DecExecuteCount();
		if ( P != mpActPlan )
			CleanUpPlan(P);
	}
}

inline SAPDB_Byte SharedSQL_CachedCommand::GetFlgExtern( void )                         { return mFlgExtern; }
inline void SharedSQL_CachedCommand::SetFlgExtern( SAPDB_Byte FlgExtern )               { mFlgExtern = FlgExtern; }

inline SAPDB_Bool SharedSQL_CachedCommand::GetFlgLRUListOld( void )                     { return mLRUOldFlg; }
inline void SharedSQL_CachedCommand::SetFlgLRUListOld( SAPDB_Bool Flg )                 { mLRUOldFlg = Flg; }

inline SharedSQL_CommandStatistics& SharedSQL_CachedCommand::Statistics( void )			{ return mStatistics; }
inline SharedSQL_CommandStatistics& SharedSQL_CachedCommand::StatisticsReset( void )	{ return mStatisticsReset; }

inline SharedSQL_CommandCache& SharedSQL_CachedCommand::CommandCache( void )            { return mCommandCache; }

inline SharedSQL_CommandID SharedSQL_CachedCommand::GetCommandID( void )                { return mCommandID; }
inline SharedSQL_CommandID SharedSQL_CachedCommand::GetFileKey( void )                  { return mFileKey; }
inline SharedSQL_HashValue SharedSQL_CachedCommand::GetHashValue( void )                { return mHashValue; }
inline SharedSQL_HashValue SharedSQL_CachedCommand::GetSlot( void )                     { return mSlot; }
inline SAPDB_Bool          SharedSQL_CachedCommand::IsInternal( void )                  { return mInternal; }

inline void SharedSQL_CachedCommand::SetInternal( SAPDB_Bool Internal )
{
    if (!Internal)
        mInternal = Internal;
}

inline void SharedSQL_CachedCommand::SetQueryRewrite( SQLMan_QueryRewriteMode QRWDone ) { mQueryRewrite = QRWDone; }

inline SharedSQL_SQLStmt&           SharedSQL_CachedCommand::SQLStmt( void )         { return mSQLStmt; }
inline SharedSQL_SQLContext&        SharedSQL_CachedCommand::SQLContext( void )      { return mSQLContext; }

inline void SharedSQL_CachedCommand::EnterLockCommand( bool exclusive )              { mpCommandLock->enter(exclusive); }
inline bool SharedSQL_CachedCommand::TryEnterLockCommand( bool exclusive )           { return mpCommandLock->tryEnter(exclusive); }
inline void SharedSQL_CachedCommand::LeaveLockCommand( bool exclusive )              { mpCommandLock->leave(exclusive); }

inline void SharedSQL_CachedCommand::EnterLockPrepare( bool exclusive )              { mpPrepareLock->enter(exclusive); }
inline bool SharedSQL_CachedCommand::TryEnterLockPrepare( bool exclusive )           { return mpPrepareLock->tryEnter(exclusive); }
inline void SharedSQL_CachedCommand::LeaveLockPrepare( bool exclusive )              { mpPrepareLock->leave(exclusive); }
inline void SharedSQL_CachedCommand::LockPrepareSwitchToNonExclusive( void )         { mpPrepareLock->SwitchToNonExclusiveAccess(); }

inline SharedSQL_CachedCommand*     SharedSQL_CachedCommand::NextHashCommand( void ) const { return mNextHashCommand; }
inline SharedSQL_CachedCommand*&    SharedSQL_CachedCommand::NextHashCommand( void ) { return mNextHashCommand; }
inline SharedSQL_CachedCommand*&    SharedSQL_CachedCommand::PrevHashCommand( void ) { return mPrevHashCommand; }
inline SharedSQL_CachedCommand*&    SharedSQL_CachedCommand::NextLRUCommand( void )  { return mNextLRUCommand; }
inline SharedSQL_CachedCommand*&    SharedSQL_CachedCommand::PrevLRUCommand( void )  { return mPrevLRUCommand; }

inline void SharedSQL_CachedCommand::SetStatusForOldPlans( SharedSQL_CommandStatus Status )
{
//	private method : lock must be entered before
	SharedSQL_Plan* A = mpOldPlans;
	while ( A )
	{
		A->SetStatus(Status);
		A = A->NextPlan();
	}
}

inline void SharedSQL_CachedCommand::SetStatusForOldPlans( SQLMan_Context& Context, SharedSQL_Schema Schema, SharedSQL_CommandStatus Status )
{
//	private method : lock must be entered before
	SharedSQL_Plan* A = mpOldPlans;
	while ( A )
	{
		if ( A->AffectedBySchema(Context, mSQLContext, Schema) )
			A->SetStatus(Status);
		A = A->NextPlan();
	}
}

inline void SharedSQL_CachedCommand::SetStatusForOldPlans( SQLMan_Context& Context, SharedSQL_TableTree* Tables, SharedSQL_CommandStatus Status )
{
//	private method : lock must be entered before
	SharedSQL_Plan* A = mpOldPlans;
	while ( A )
	{
		if ( A->AffectedByTables(Context, mSQLContext, Tables) )
			A->SetStatus(Status);
		A = A->NextPlan();
	}
}

inline SharedSQL_Plan* SharedSQL_CachedCommand::GetActPlan( void )
{
	SAPDBERR_ASSERT_STATE( mpActPlan?(mpActPlan->GetStatus() == Prepared):true );
	return mpActPlan;
}

inline SharedSQL_Plan* SharedSQL_CachedCommand::NewPlan( void )
{
    SAPDBERR_ASSERT_STATE ( !mpActPlan );

	if ( !mpActPlan )
    {
		mpActPlan = new(mAllocator) SharedSQL_Plan(mAllocator);
        SHARED_SQL_TRACE ("   SharedSQL_Command NewPlan[" << ToStr(mCommandID) << "] : " << ToStr(mpActPlan));
        return mpActPlan;
    }
    return 0;
}

inline void SharedSQL_CachedCommand::ErrorWhilePreparing( void )
{
	SAPDBERR_ASSERT_STATE( mStatus != Prepared );

	// this will be called from SharedSQL_PrepareHandle only and has to invalidate all plans

	SharedSQL_LockedScope Lock(mpCommandLock, SharedSQL_LockedScope::Exclusive);
	SetStatusForOldPlans(Invalid);
	QueueActPlanToOldPlans(Invalid, true);
}

inline void SharedSQL_CachedCommand::ClearTableInfo( void )
{
	SAPDBERR_ASSERT_STATE( mStatus != Prepared );

	SharedSQL_LockedScope Lock(mpCommandLock, SharedSQL_LockedScope::Exclusive);
	if ( mpActPlan )
	{
	    SHARED_SQL_TRACE ("   SharedSQL_Command ClearTableInfo[" << ToStr(mCommandID) << "]");
		mpActPlan->ClearTableInfo();
	}
}

inline void SharedSQL_CachedCommand::QueueActPlanToOldPlans( SharedSQL_CommandStatus Status, bool Force )
{
//	private method : lock must be entered before
    if ( !((mStatus == Invalid) && (Status==Dropped)) )
        mStatus = Status; 

	if ( mpActPlan && ( mStatus!=Preparing || Force ) )
	{
        SHARED_SQL_TRACE ("   SharedSQL_Command QueueToOldPlans[" << ToStr(mCommandID) << "] : " << ToStr(mpActPlan));
		mpActPlan->SetStatus(Status);

		SAPDB_Int4 _cnt = mpActPlan->GetExecuteCount() 
						+ mpActPlan->GetParseIDCount();
		if ( !_cnt )
		{
			SHARED_SQL_TRACE ("    SharedSQL_Command DeletePlanElements : " << ToStr(mpActPlan) << " QueueActPlanToOldPlans");
			mpActPlan->DeletePlanElements();
		}

		if ( mpOldPlans )
		{
			mpOldPlans->PrevPlan() = mpActPlan;
			mpActPlan->NextPlan() = mpOldPlans;
		}
		mpOldPlans = mpActPlan;
		mpActPlan  = 0;
	}
}

inline SAPDB_Bool SharedSQL_CachedCommand::CleanUpPlans( SAPDB_UInt4 LRUPos )
{
//	lock must be entered before
	SHARED_SQL_TRACE ("   SharedSQL_Command CleanUpPlans[" << ToStr(mCommandID) << "] LRUPos=" << ToStr(LRUPos) );
	bool deleted = false;

	QueueActPlanToOldPlans(Dropped);

	SharedSQL_Plan* P = mpOldPlans;
	while ( P )
	{
		SharedSQL_Plan* Next = P->NextPlan();
		deleted |= CleanUpPlan(P);
		P = Next;
	}
	return deleted;
}

inline bool SharedSQL_CachedCommand::CleanUpPlan( SharedSQL_Plan*& P )
{
	SAPDBERR_ASSERT_STATE( P );
	if ( P )
	{
		if ( !P->GetParseIDCount() )
			return DeletePlan(P);
		else
			if ( !P->GetExecuteCount() )
			{
				SHARED_SQL_TRACE ("    SharedSQL_Command DeletePlanElements : " << ToStr(P) );
				return P->DeletePlanElements();
			}
	}
	return false;
}


inline bool SharedSQL_CachedCommand::DeletePlan( SharedSQL_Plan*& P )
{
//	private method : lock must be entered before
	if ( P )
	{
		CheckPlanArgument(P);

		if ( P != mpActPlan )
		{
			if ( P == mpOldPlans )	// first plan in queue
			{
				if ( mpOldPlans = P->NextPlan() )	// assignment is correct here !
				{
					mpOldPlans->PrevPlan() = 0;
					P->NextPlan() = 0;
				}
			}
			else
			{
				if ( P->NextPlan() )
					P->NextPlan()->PrevPlan() = P->PrevPlan();
				if ( P->PrevPlan() )
					P->PrevPlan()->NextPlan() = P->NextPlan();

				P->NextPlan() = P->PrevPlan() = 0;
			}
			SAPDBERR_ASSERT_STATE( !P->PrevPlan() && !P->NextPlan() );
		}
		SHARED_SQL_TRACE ("    SharedSQL_Command DeletePlan[" << ToStr(mCommandID) << "] : " << ToStr(P));
		destroy(P, mAllocator);
		P = 0;
		return true;
	}
	return false;
}

/* ------------------------------------------------------------------------- */

#endif  /* SHAREDSQL_COMMAND_HPP */
