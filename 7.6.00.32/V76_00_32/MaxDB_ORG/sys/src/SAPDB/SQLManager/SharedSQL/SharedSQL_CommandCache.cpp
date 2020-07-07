/*!
    @file     SharedSQL_CommandCache.cpp
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

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SQLManager/SharedSQL/SharedSQL_CommandCache.hpp"
#include "SQLManager/SharedSQL/SharedSQL_CommandCacheIterator.hpp"
#include "SQLManager/SharedSQL/SharedSQL_Command.hpp"
#include "SQLManager/SharedSQL/SharedSQL_Handles.hpp"
#include "SQLManager/SharedSQL/SharedSQL_Plan.hpp"
#include "SQLManager/SharedSQL/SharedSQL_Types.hpp"

#include "SQLManager/SQLMan_Context.hpp"

#include "FileDirectory/FileDir_FileNo.hpp"

#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "SAPDBCommon/SAPDB_Singleton.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/Algorithms/SAPDBAlgo_FNVHash.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_SynchronizedRawAllocator.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"

#include "KernelCommon/Kernel_VTrace.hpp"
#include "KernelCommon/ParameterNames/KernelParam_SharedSQL.hpp"

#include "RunTime/MemoryManagement/RTEMem_RawAllocator.hpp"
#include "RunTime/MemoryManagement/RTEMem_BlockAllocator.hpp"
#include "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp"
#include "RunTime/System/RTESys_MemoryBarrier.h"
#include "RunTime/System/RTESys_AtomicOperation.hpp"
#include "RunTime/System/RTESys_MicroTime.h"

#include "ggg00.h"
#include "hgg04.h"
#include "hbd01.h"
#include "hbd02.h"
#include "heo58.h"

#include "SAPDB/SAPDBCommon/Messages/SDBMsg_SharedSQL.h"

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

SharedSQL_CommandCache* SharedSQL_CommandCache::mInstance = (SharedSQL_CommandCache*) 0;
SAPDB_Bool SharedSQL_CommandCache::mCreateError = false;

SharedSQL_CommandCache* SharedSQL_CommandCache::Instance( SAPDB_Bool Create )
{
    static RTESync_Spinlock lock;
    RTESync_LockedScope scope(lock);

    if ( !mInstance && Create && !mCreateError )
    {
		SQLMan_Context* context = SQLMan_Context::GetContext();
		Msg_List& errMsg = context->GetErrorList();

		// first read installation parameters from cserv.pcf
        RTEConf_Parameter::Integer  Param_CacheSizeKB;
        RTEConf_Parameter::Integer  Param_ExpectedStatements = 1500;


        if( RTEConf_ParameterAccess::Instance()->GetInteger( UTF8( KERNELPARAM_SHAREDSQL_COMMANDCACHESIZE ), Param_CacheSizeKB, errMsg ))
        {
            if( ! RTEConf_ParameterAccess::Instance()->GetInteger( UTF8( KERNELPARAM_SHAREDSQL_EXPECTEDSTATEMENTCOUNT ), Param_ExpectedStatements, errMsg ))
                mCreateError = true;
        }
        else
        {
            mCreateError = true;
        }

		if ( mCreateError )
			return mInstance;

        // and then calculate the 'real' parameters...

        SAPDB_ULong Size    = (SAPDB_ULong)( Param_CacheSizeKB >= 8*1024L ? Param_CacheSizeKB : 8*1024L );

        int SizeCategory;
        if ( Param_ExpectedStatements > 1024 )           // > 2**10
        {
            if ( Param_ExpectedStatements > 4096 )       // > 2**12
            {
                if ( Param_ExpectedStatements > 8192 )   // > 2**13
                    SizeCategory = 14;
                else
                    SizeCategory = 13;
            }
            else                                         // <= 2**12
            {
                if ( Param_ExpectedStatements > 2048 )   // > 2**11
                    SizeCategory = 12;
                else
                    SizeCategory = 11;
            }
        }
        else                                             // <= 2**10
        {
            if ( Param_ExpectedStatements > 256 )        // > 2**8
            {
                if ( Param_ExpectedStatements > 512 )    // > 2**9
                    SizeCategory = 10;
                else
                    SizeCategory = 9;
            }
            else                                         // <= 2**8
            {
                if ( Param_ExpectedStatements > 128 )    // > 2**7
                    SizeCategory = 8;
                else
                    SizeCategory = 7;
            }
        }

        SharedSQL_HashValue Slots                       = (SharedSQL_HashValue) (1 << (SizeCategory+1)); // min:256 max:32768
        SharedSQL_HashValue SpinLockPoolForSlotsSize    = (SharedSQL_HashValue) (1 << (SizeCategory-4)); // min:  8 max: 1024
        SharedSQL_HashValue SpinLockPoolForCommandsSize = (SharedSQL_HashValue) (1 << (SizeCategory-5)); // min:  4 max:  256
        
        if ( (mInstance = new(RTEMem_RawAllocator::Instance()) SharedSQL_CommandCache(Size, Slots, (SharedSQL_HashValue)Param_ExpectedStatements, SpinLockPoolForSlotsSize, SpinLockPoolForCommandsSize, errMsg)) )
		{
			if ( !mInstance->CompletelyConstructed() )
	            mCreateError = true;
		}
		else
		{
            mCreateError = true;
		}
		if ( mCreateError )
		{
			errMsg.AppendNewMessage( Msg_List(Msg_List::Error,
									SDBMSG_SHAREDSQL_CREATE_INSTANCE) );
        }
    }
    return mInstance;
}

//----------------------------------------------------------------------------
SharedSQL_CommandCache::SharedSQL_CommandCache( SAPDB_ULong KBSize, SharedSQL_HashValue Slots, 
											    SharedSQL_HashValue ExpectedStatements,
                                                SharedSQL_HashValue SpinLockPoolSlotsSize,
                                                SharedSQL_HashValue SpinLockPoolCommandsSize,
												Msg_List& MsgList ) 
	: SAPDB_Singleton()
    , mAllocator(  (const SAPDB_UTF8 *) "CommandCacheAllocator",
                RTEMem_BlockAllocator::Instance(),
                 16384*1024,         // FirstBlockSize      = 16MB (minimum according to cserv.pcf)
                  2048*1024,         // SupplementBlockSize =  2MB
                SAPDBMem_RawAllocator::FREE_RAW_EXTENDS,
                KBSize*1024          // MaxCacheSize        (default 256MB according to cserv.pcf)
                )
    , mTempFileState(NoFile)
    , mActCommandID(Slots)
    //
   	, mHashTable(newarray(mHashTable, Slots, mAllocator))
    //
    // RWRegions
    , mSpinLockPoolForSlots   ((const SAPDB_UTF8 *)"CommandCache:SlotLock", SpinLockPoolSlotsSize, RTEMem_RawAllocator::Instance())
    , mSpinLockPoolForCommands((const SAPDB_UTF8 *)"CommandCache:CommandLock", SpinLockPoolCommandsSize, RTEMem_RawAllocator::Instance())
    , mSpinLockPoolForOthers  ((const SAPDB_UTF8 *)"CommandCache:DivLock", 2, RTEMem_RawAllocator::Instance())
    , mpLRULock          (RTESync_CreateRWRegion(1, mSpinLockPoolForOthers, RTEMem_RawAllocator::Instance()))
    , mpCleanUpLock      (RTESync_CreateRWRegion(2, mSpinLockPoolForOthers, RTEMem_RawAllocator::Instance()))
    , mpCreateFileLock   (RTESync_CreateRWRegion(3, mSpinLockPoolForOthers, RTEMem_RawAllocator::Instance()))
    , mpSlotLocks( (RTESync_IRWRegion**)RTEMem_RawAllocator::Instance().Allocate(Slots * sizeof(RTESync_IRWRegion *) ))
    //
    , mNumberOfSlots(Slots)
    , mFirstLRUCommand(0)
    , mLastLRUCommand(0)
    , mMiddleLRUCommand(0)
    , mLRUCounter(0)
	, mExpectedStmtCnt(ExpectedStatements)
    , mCacheInfo()
    , mCacheInfoReset()
    , mCommandStatisticsResetTime(RTESys_MicroSecondTime())
	, mErrorInitialising(false)
	, mWaitAfterFullCnt(0)
{
    if ( mpLRULock==NULL ||
         mpCleanUpLock==NULL ||
         mpCreateFileLock==NULL )
    {
		MsgList.AppendNewMessage( Msg_List(Msg_List::Error,
    								SDBMSG_SHAREDSQL_CREATE_LOCKS) );
        mErrorInitialising = true;
    }
    if ( mHashTable && mpSlotLocks )
    {
        for (SharedSQL_HashValue i=0; i<mNumberOfSlots; i++) 
        {
            mHashTable[i] = 0;
            mpSlotLocks[i] = RTESync_CreateRWRegion(i+1, mSpinLockPoolForSlots, mAllocator);
            if ( mpSlotLocks[i]==0 )
            {
				MsgList.AppendNewMessage( Msg_List(Msg_List::Error,
    										SDBMSG_SHAREDSQL_CREATE_LOCKS) );
                mErrorInitialising = true;
            }
        }
    }
    else
    {
		MsgList.AppendNewMessage( Msg_List(Msg_List::Error,
    								SDBMSG_SHAREDSQL_CREATE_LOCKS) );
        mErrorInitialising = true;
    }
    mAllocator.DisableOpMessages();


#ifdef SHAREDSQL_PROTECT
    mAllocator.WriteProtect();  // DDT: should be removed after testing !
#endif
}

SharedSQL_CommandCache::~SharedSQL_CommandCache( void )
{
    if ( mHashTable )
    {
	    for (SharedSQL_HashValue i=0; i<mNumberOfSlots; i++)
        {
		    if ( SharedSQL_CachedCommand* Act = mHashTable[i] )
		    {
			    SharedSQL_CachedCommand* Next;
			    do 
			    {
				    Next = Act->NextHashCommand();
                    if ( Act->SQLStmt().mStmt )
                    {
                        Deallocate(Act->SQLStmt().mStmt);
                        Act->SQLStmt().mStmt = 0;
                    }
                    destroy(Act, *this);                       
               	    --mCacheInfo.CommandCount;
				    Act = Next;
			    }while(Act);
		    }
        }
    	destroyarray(mHashTable, mNumberOfSlots, mAllocator);
    }
    SAPDBERR_ASSERT_STATE( mCacheInfo.CommandCount == 0 ); // DDT
    mInstance = (SharedSQL_CommandCache*) 0;
    if ( mpLRULock )
        RTESync_DestroyRWRegion(mpLRULock, mAllocator);
    if ( mpCleanUpLock )
        RTESync_DestroyRWRegion(mpCleanUpLock, mAllocator);
    if ( mpCreateFileLock )
        RTESync_DestroyRWRegion(mpCreateFileLock, mAllocator);
    for (SharedSQL_HashValue j=0; j<mNumberOfSlots; j++)
    {
        if (mpSlotLocks[j])
            RTESync_DestroyRWRegion(mpSlotLocks[j], mAllocator);
    }
}

//----------------------------------------------------------------------------
SharedSQL_PrepareHandle* SharedSQL_CommandCache::NewPrepareHandle( SAPDBMem_IRawAllocator& Alloc, SharedSQL_SQLContext& Context, SharedSQL_SQLStmt& Stmt, SAPDB_Bool Internal, SAPDB_Bool ParseAgain, Msg_List& MsgList )
{
    SAPDBTRACE_METHOD_DEBUG("SharedSQL_CommandCache::GetPrepareHandle", SharedSQL_Trace, 5);
    SAPDBERR_ASSERT_ARGUMENT( Stmt.mStmt != 0 );

    ++mCacheInfoReset.AccessCountPrepare; 

    if ( Internal ) 
    {
//		return 0;	// DT : due to duplicate locking we cant have internal commands so far

        // PTS 1132984
        // don't consider the following members
        memset(Context.UserID,     0, sizeof(SharedSQL_UserID));
        memset(Context.CurrSchema, 0, sizeof(Context.CurrSchema));
        Context.NLSParam_DFLength = 0;
        Context.SQLMode           = sqlm_internal;
        Context.DateTimeFormat    = dtf_normal;
    } 

    SharedSQL_HashValue Hash = CalcHashValue(Context, Stmt);
    SharedSQL_HashValue Slot = CalcSlot(Hash);
    SAPDB_Int2 ChainEntryCount = 0;

    mpSlotLocks[Slot]->enter(SharedSQL_LockedScope::Exclusive);
    if ( SharedSQL_CachedCommand* ActCommand = mHashTable[Slot] )
	{
		do
		{
            ++ChainEntryCount;
			if (    ActCommand->GetHashValue()==Hash     && 
                    ActCommand->SQLContext()==Context &&        
                    ActCommand->SQLStmt().mStmtSize==Stmt.mStmtSize &&
                    ActCommand->SQLStmt().mDescriptionSize==Stmt.mDescriptionSize)	
            {
                ActCommand->EnterLockPrepare(1);
                if ( ActCommand->GetFlgExtern() )
                {
                    SQLMan_Context* pContext = (SQLMan_Context*) vGetAcvPtrFromCurrentTask();
                    tgg00_TransContext& TransContext = pContext->TransContext();

                    if (!ReloadSQLStmt(TransContext, ActCommand))
                    {
						MsgList.AddDetail(SDBMSG_SHAREDSQL_COMPARE_COMMANDS);
                        // No return. Just continue!
                    }
                }
				if ( ActCommand->SQLStmt() == Stmt )
				{
                    mpSlotLocks[Slot]->leave(SharedSQL_LockedScope::Exclusive);

                    void* X = Alloc.Allocate(sizeof(SharedSQL_PrepareHandle)); 
                    // this may result in CleanUp() if Alloc==*this - therefore we have to do this allocation BEFORE locking the command!

                    if ( X )
                    {
                        ActCommand->EnterLockCommand(1);
					    bool MustPrepare = false;
					    SharedSQL_PrepareHandle *H = new(X) SharedSQL_PrepareHandle(&Alloc, ActCommand, MustPrepare);
                        // we need this placement new to avoid CleanUp during memory allocation!

						if ( H )
						{
							if ( H->HasPlan() )
							{
								if ( MustPrepare )
								{
            						++mCacheInfoReset.UnSuccessfulAccessCountPrepare;
    								ActCommand->Statistics().IncTotalPrepareCount();
								}
								else
								{
									ActCommand->LockPrepareSwitchToNonExclusive();
    								++mCacheInfoReset.SuccessfulAccessCountPrepare;
								}
								ActCommand->SetInternal(Internal);
								ActCommand->LeaveLockCommand(1);
								LRUListMoveCommandToTop(ActCommand, true); 
								SHARED_SQL_TRACE ("SharedSQL_CommandCache FoundCmd[" << ToStr(ActCommand->GetCommandID()) << "]");
							}
							else
							{
							    H->~SharedSQL_PrepareHandle();
								Alloc.Deallocate((void*)H);
								H = 0;
								ActCommand->Invalidate(Invalid);
		                        ActCommand->LeaveLockCommand(1);
							}
						}
						else
						{
	                        ActCommand->LeaveLockCommand(1);
						}
						if ( !H )
						{
							ActCommand->LeaveLockPrepare(1);
						}
						return H;
                    }
					ActCommand->LeaveLockPrepare(1);
                    return 0;
				}
                ActCommand->LeaveLockPrepare(1);
            }
        }while ( ActCommand = ActCommand->NextHashCommand() );
    }
	// Command is NOT in the list - create and insert at first position

    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // Allocate Buffer for Stmt AND Description!!! 
    //  The Statement will start with Buffer[0], 
    //  the description will start with Buffer[mStmtSize] 
    // ATTENTION: Within SharedSQL it will be assumed that there was only one buffer allocated ! 
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    void* Buffer = 0;
    long  BuffLen = Stmt.mStmtSize + Stmt.mDescriptionSize;
    if ( Stmt.mStmtSize>0 && Stmt.mStmt )
    {
        Buffer = Allocate(BuffLen);
    }
    if ( Buffer )
    {
        void* StmtStr = Buffer;
        void* DescStr = 0;

        SAPDB_MemCopyNoCheck(StmtStr, Stmt.mStmt, Stmt.mStmtSize);

        if ( Stmt.mDescriptionSize>0 && Stmt.mDescription )
        {
            DescStr = ((char*)Buffer) + Stmt.mStmtSize;
            SAPDB_MemCopyNoCheck(DescStr, Stmt.mDescription, Stmt.mDescriptionSize);
        }

        SharedSQL_SQLStmt SQLStmt(StmtStr, Stmt.mStmtSize, DescStr, Stmt.mDescriptionSize);

        SharedSQL_CachedCommand* NewCommand = new(*this) SharedSQL_CachedCommand(NewCommandID(), Hash, Slot, *this, *this, Context, SQLStmt, Internal, mSpinLockPoolForCommands);
        if ( NewCommand )
        {
            if ( NewCommand->CompletelyConstructed() )
            {
				RTESys_WriteMemoryBarrier();
                SHARED_SQL_TRACE ("SharedSQL_CommandCache InsertCmd[" << ToStr(NewCommand->GetCommandID()) << "]");

                ++mCacheInfo.CommandCount;
                if ( mCacheInfo.CommandCount > mCacheInfo.MaxCommandCount )
                    mCacheInfo.MaxCommandCount = mCacheInfo.CommandCount;
                ++mCacheInfoReset.InsertCommandCount;
                ++mCacheInfoReset.UnSuccessfulAccessCountPrepare;
                ++ChainEntryCount;
                if ( ChainEntryCount > mCacheInfoReset.HashTab_MaxChainEntryCount )
                    mCacheInfoReset.HashTab_MaxChainEntryCount = ChainEntryCount;

                NewCommand->EnterLockPrepare(1);
				bool MustPrepare = false;
                SharedSQL_PrepareHandle* H = new(Alloc) SharedSQL_PrepareHandle(&Alloc, NewCommand, MustPrepare);
				if ( H )
				{
					if ( H->HasPlan() )
					{
						SAPDBERR_ASSERT_STATE(MustPrepare);
						{   // HastTable[Slot] is already locked !
							SharedSQL_LockedScope Lock3(mpLRULock, SharedSQL_LockedScope::Exclusive);
							AttachCommand(NewCommand, Slot);
						}
						mpSlotLocks[Slot]->leave(SharedSQL_LockedScope::Exclusive);
  						NewCommand->Statistics().IncTotalPrepareCount();
						return H;
					}
					else
					{
						destroy(H, Alloc);
					}
				}
            }
            destroy(NewCommand, *this);	// this will destroy the StmtText buffer points to also
  			Buffer = 0;					// avoid crash by duplicate deallocation
        }
    }
    if ( Buffer )
        Deallocate(Buffer);

    MsgList.AppendNewMessage( Msg_List(Msg_List::Error,
                              SDBMSG_SHAREDSQL_CREATE_COMMAND) );
    mpSlotLocks[Slot]->leave(SharedSQL_LockedScope::Exclusive);
    return 0;
}

void SharedSQL_CommandCache::DestroyPrepareHandle( SharedSQL_PrepareHandle* PH )
{
    SharedSQL_PrepareHandle* Handle = REINTERPRET_CAST(SharedSQL_PrepareHandle*, PH);
    SAPDBMem_IRawAllocator& Alloc = Handle->GetAllocator();
    Handle->~SharedSQL_PrepareHandle();
    Alloc.Deallocate((void*)Handle);
    PH = 0;
}

//----------------------------------------------------------------------------

void* SharedSQL_CommandCache::Allocate( SAPDB_ULong ByteCount )
{
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_CommandCache::Allocate(ByteCount)", SharedSQL_Trace, 5);

    void* Mem = 0;
#if defined ( SAPDB_SLOW ) && defined ( SDB_LCSTATE_DEV )
	SAPDB_UInt8 TTime = RTESys_MicroSecTimer();
	if ( (TTime/1000)%31 != 0 )
	{
#endif
		if ( mCacheInfo.CommandCount > mExpectedStmtCnt )
			Mem = mAllocator.AllocateWithoutGrowth(ByteCount);  // try allocation without extending 
		else
			Mem = mAllocator.Allocate(ByteCount);
		if ( !Mem )
		{
			SharedSQL_LockedScope Lock1(mpCleanUpLock, SharedSQL_LockedScope::Exclusive);
			SharedSQL_LockedScope Lock3(mpLRULock, SharedSQL_LockedScope::Exclusive);

			SQLMan_Context* pContext = (SQLMan_Context*) vGetAcvPtrFromCurrentTask();
			tgg00_TransContext& TransContext = pContext->TransContext();

			SAPDB_ULong BytesUsed, MaxBytesUsed, BytesControlled;
			mAllocator.CalcStatistics(BytesUsed, MaxBytesUsed, BytesControlled);
			if ( SharedSQL_Trace.TracesLevel(3) )
			{
				SHARED_SQL_TRACE ("SharedSQL_CommandCache CleanUp due to low memory: Free: "<< BytesControlled-BytesUsed << " Requested: " << ByteCount);
			}

			SAPDB_UInt4 StopCnt	= (SAPDB_UInt4) mCacheInfo.CommandCount/SharedSQL_OldLRUPart;

			++mCacheInfoReset.CleanUpCount;
			SharedSQL_CachedCommand* Act = mLastLRUCommand;
			SharedSQL_CachedCommand* Next;

			// First only try to CleanUp to old part of the LRU list
			SAPDB_UInt4 LRUPos = 0;
			while ( Act && LRUPos<StopCnt )
			{
				++LRUPos;
				Next = Act->PrevLRUCommand();
				if ( Act->TryEnterLockCommand(1) )
				{
					if (CleanUpCommand(TransContext, Act, SAPDB_TRUE, LRUPos))
						Act->LeaveLockCommand(1);
				}
				Act = Next;
			};

			Mem = mAllocator.Allocate(ByteCount);
			if ( !Mem )
			{
				// continued with the rest of the LRU list
				while ( Act )
				{
					++LRUPos;
					Next = Act->PrevLRUCommand();
					if ( Act->TryEnterLockCommand(1) )
					{
						if (CleanUpCommand(TransContext, Act, SAPDB_TRUE, LRUPos))
							Act->LeaveLockCommand(1);
					}
					Act = Next;
				};

				Mem = mAllocator.Allocate(ByteCount);
				if ( !Mem )
				{
					SQLMan_Context* pContext = (SQLMan_Context*) vGetAcvPtrFromCurrentTask();
					pContext->GetErrorList().AppendNewMessage(
													Msg_List(Msg_List::Error,
													SDBMSG_SHAREDSQL_NO_MEMORY,
													Msg_Arg("MEMREQUEST", ByteCount)) );
					++mCacheInfoReset.FailedAllocateCount;
//					mWaitAfterFullCnt = mCacheInfo.CommandCount / 10;
				}
			}
		}
#if defined ( SAPDB_SLOW ) && defined ( SDB_LCSTATE_DEV )
	}
	else
	{
		Kernel_VTrace() << "SharedSQL : Random memory allocation failure !!!";
	}
#endif
    return Mem;
}

//----------------------------------------------------------------------------
void* SharedSQL_CommandCache::Allocate( SAPDB_ULong ByteCount, const void* Hint )
{
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_CommandCache::Allocate(ByteCount,Hint)", SharedSQL_Trace, 5);
    return Allocate(ByteCount);
}

void SharedSQL_CommandCache::Deallocate( void* p )
{
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_CommandCache::Deallocate", SharedSQL_Trace, 5);

    mAllocator.Deallocate(p);
	//if ( mWaitAfterFullCnt > 0 )
	//	--mWaitAfterFullCnt;
}

//----------------------------------------------------------------------------
void SharedSQL_CommandCache::CleanUpAll( void )
{
    SHARED_SQL_TRACE ("SharedSQL_CommandCache::CleanUpAll");
    
    SQLMan_Context* pContext = (SQLMan_Context*) vGetAcvPtrFromCurrentTask();
    tgg00_TransContext& TransContext = pContext->TransContext();

    SharedSQL_LockedScope Lock1(mpCleanUpLock, SharedSQL_LockedScope::Exclusive);
    SharedSQL_LockedScope Lock3(mpLRULock, SharedSQL_LockedScope::Exclusive);

    ++mCacheInfoReset.CleanUpCount;
    SharedSQL_CachedCommand* Act = mLastLRUCommand;
    SharedSQL_CachedCommand* Next;


	SAPDB_UInt4 LRUPos = 0;
    while ( Act )
    {
		++LRUPos;
        Next = Act->PrevLRUCommand();
		if ( Act->TryEnterLockCommand(1) )
		{
			if (CleanUpCommand(TransContext, Act, SAPDB_FALSE, LRUPos) )  // FALSE: No unload to tempfile
				Act->LeaveLockCommand(1);
		}
        Act = Next;
    };
}

//----------------------------------------------------------------------------

void SharedSQL_CommandCache::Invalidate( bool NotifyApplication )
{
    SHARED_SQL_TRACE ("SharedSQL_CommandCache::InvalidateAll");
    SAPDBERR_ASSERT_STATE( mNumberOfSlots > 0 );
   
    ++mCacheInfoReset.InvalidateCount;

	SharedSQL_LockedScope Lock3(mpLRULock, SharedSQL_LockedScope::NonExclusive);
    SharedSQL_CachedCommand* Act = mFirstLRUCommand;
    while ( Act )
    {
		Act->EnterLockCommand(1);
		if ( Act->GetCurrentStatus() != Preparing )
			Act->Invalidate((NotifyApplication?Invalid:Dropped));
		Act->LeaveLockCommand(1);
        Act = Act->NextLRUCommand();
    };
}

void SharedSQL_CommandCache::Invalidate( SQLMan_Context& Context, SharedSQL_Schema Schema, bool NotifyApplication )
{
    SHARED_SQL_TRACE ("SharedSQL_CommandCache::InvalidateSchema");
    SAPDBERR_ASSERT_STATE( mNumberOfSlots > 0 );
    
    ++mCacheInfoReset.InvalidateCount;

	SharedSQL_LockedScope Lock3(mpLRULock, SharedSQL_LockedScope::NonExclusive);
    SharedSQL_CachedCommand* Act = mFirstLRUCommand;
    while ( Act )
    {
		Act->Invalidate(Context, Schema, (NotifyApplication?Invalid:Dropped));
        Act = Act->NextLRUCommand();
    };
}

void SharedSQL_CommandCache::Invalidate( SQLMan_Context& Context, SharedSQL_TableTree* Tables, bool NotifyApplication )
{
    SHARED_SQL_TRACE ("SharedSQL_CommandCache::InvalidateTable");
    SAPDBERR_ASSERT_STATE( mNumberOfSlots > 0 );

    SharedSQL_LockedScope Lock3(mpLRULock, SharedSQL_LockedScope::NonExclusive);
    ++mCacheInfoReset.InvalidateCount;
    SharedSQL_CachedCommand* Act = mFirstLRUCommand;
    while ( Act )
    {
		Act->Invalidate(Context, Tables, (NotifyApplication?Invalid:Dropped));
        Act = Act->NextLRUCommand();
    };
}

//----------------------------------------------------------------------------
void SharedSQL_CommandCache::ResetCommandInfos( void )
{
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_CommandCache::ResetCommandInfos", SharedSQL_Trace, 5);
    
    SharedSQL_LockedScope Lock3(mpLRULock, SharedSQL_LockedScope::NonExclusive);

    mCommandStatisticsResetTime = RTESys_MicroSecondTime();

    SharedSQL_CachedCommand* Act = mLastLRUCommand;
    while ( Act )
    {
        Act->ResetStatisticsInfo();
        Act = Act->PrevLRUCommand();
    };
}

//----------------------------------------------------------------------------
SAPDB_Bool SharedSQL_CommandCache::CleanUpCommand( tgg00_TransContext& TransContext, SharedSQL_CachedCommand* Command, SAPDB_Bool FlgUnload, SAPDB_UInt4 LRUPos )
{
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_CommandCache::CleanUpCommand", SharedSQL_Trace, 8);
    SAPDBERR_ASSERT_ARGUMENT( Command != 0 );

    SAPDB_Int4 _PrepareCount = Command->GetPrepareCount();
    SAPDB_Int4 _ParseIDCount = Command->GetParseIDCount();
    SAPDB_Int4 _ExecuteCount = Command->GetExecuteCount();
    bool noaction = true;

    ++mCacheInfoReset.CleanUpCommandCount; 
    if ( !_PrepareCount 
         && !_ParseIDCount 
		 )           // No handles: Stmt can be deleted
    {
		SAPDBERR_ASSERT_STATE( !_ExecuteCount );

        SharedSQL_HashValue Slot = Command->GetSlot();
        if ( mpSlotLocks[Slot]->tryEnter(SharedSQL_LockedScope::Exclusive) )
        {
			noaction = false;
			SHARED_SQL_TRACE ("SharedSQL_CommandCache DestroyCmd[" << ToStr(Command->GetCommandID()) << "] LRUPos=" << ToStr(LRUPos) );
			if ( Command->SQLStmt().mStmt )
			{
				Deallocate(Command->SQLStmt().mStmt);
				Command->SQLStmt().mStmt = 0;
			}
			if ( Command->GetFlgExtern() )
			{
				DeleteUnloadedSQLStmt(TransContext, Command);
			}
			DetachCommand(Command, Slot);
			mpSlotLocks[Slot]->leave(SharedSQL_LockedScope::Exclusive);

			RTESys_WriteMemoryBarrier();
			destroy(Command, *this);
			++mCacheInfoReset.DeleteCommandCount;
    		--mCacheInfo.CommandCount;
			return (SAPDB_FALSE);
        }
    }

	if ( Command->CleanUpPlans(LRUPos) )
	{
        noaction = false;
//        SHARED_SQL_TRACE ("SharedSQL_CommandCache DeleteCurrentPlans[" << ToStr(Command->GetCommandID()) << "] LRUPos=" << ToStr(LRUPos) );
		++mCacheInfoReset.DeletePlanCount; 
	}

	if ( FlgUnload && !Command->GetFlgExtern()
	     && !_ExecuteCount // because of SharedSQL_ExecuteHandle::GetSQLStmt
       )                    
    {
        // unload SQLStmt to temp file
	    if ( UnloadSQLStmt(TransContext, Command) )
		{
		    noaction = false;
		    SHARED_SQL_TRACE ("SharedSQL_CommandCache UnloadSQLStmt[" << ToStr(Command->GetCommandID()) << "] LRUPos=" << ToStr(LRUPos) );
		}
    }

    if ( noaction )
    {
        SHARED_SQL_TRACE ("SharedSQL_CommandCache NoAction[" << ToStr(Command->GetCommandID()) << "] LRUPos=" << ToStr(LRUPos) 
            << " PrepareCount=" << _PrepareCount 
            << " ParseIDCount=" << _ParseIDCount 
            << " ExecuteCount=" << _ExecuteCount);
    }
	
    return (SAPDB_TRUE);
}


SAPDBMem_RawAllocator* SharedSQL_CommandCache::GetBaseAllocator( void )
{
#ifdef SHAREDSQL_PROTECT
    return &mAllocator;
#else
    return 0;
#endif
}

//----------------------------------------------------------------------------
void SharedSQL_CommandCache::GetBaseAllocatorCallStatistics( SAPDB_ULong &CountAlloc, SAPDB_ULong &CountDealloc ) const 
{
    mAllocator.GetBaseAllocatorCallStatistics(CountAlloc, CountDealloc);
}

//----------------------------------------------------------------------------
void SharedSQL_CommandCache::GetCallStatistics( SAPDB_ULong &CountAlloc, SAPDB_ULong &CountDealloc ) const
{
    mAllocator.GetCallStatistics(CountAlloc, CountDealloc);
}

//----------------------------------------------------------------------------
SAPDB_ULong SharedSQL_CommandCache::GetAllocFailedCount( void ) const 
{
    return mCacheInfo.FailedAllocateCount; 
}

//----------------------------------------------------------------------------
SAPDB_Bool SharedSQL_CommandCache::UnloadSQLStmt( tgg00_TransContext& TransContext, SharedSQL_CachedCommand* Command )
{
    // Command has to be locked before this method is called
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_CommandCache::FlushSQLStmt", SharedSQL_Trace, 5);
    SAPDBERR_ASSERT_ARGUMENT( Command != 0 );

    if ( !Command->GetFlgExtern() )
    {
        if ( WriteSQLStmtToTempFile(TransContext,Command->GetFileKey(), &(Command->SQLStmt())) )
        {
            Deallocate(Command->SQLStmt().mStmt); // mStmt points to the memory allocated for mStmt+mDescription !!!
            Command->SQLStmt().mStmt = 0;
            Command->SQLStmt().mDescription = 0;
            Command->SetFlgExtern(SAPDB_TRUE);
            ++mCacheInfoReset.UnloadStmtCount; 
            ++mCacheInfoReset.CurrentUnloadStmtCount;
        }
        else
        {
            SQLMan_Context* pContext = (SQLMan_Context*) vGetAcvPtrFromCurrentTask();
            pContext->GetErrorList().AppendNewMessage(
                                        Msg_List(Msg_List::Error,
                                        SDBMSG_SHAREDSQL_UNLOAD_DATA) );
            return SAPDB_FALSE;
        }
    }
   	return SAPDB_TRUE;
}

//----------------------------------------------------------------------------
SAPDB_Bool SharedSQL_CommandCache::ReloadSQLStmt( tgg00_TransContext& TransContext, SharedSQL_CachedCommand* Command )
{
    // Command has to be locked before this method is called
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_CommandCache::ReloadSQLStmt", SharedSQL_Trace, 5);
    SAPDBERR_ASSERT_ARGUMENT( Command != 0 );

    if ( Command->GetFlgExtern() )
    {
        long RequestSize = Command->SQLStmt().mStmtSize + Command->SQLStmt().mDescriptionSize;
        Command->SQLStmt().mStmt = Allocate(RequestSize);
        if ( Command->SQLStmt().mStmt != 0 )
        {
            if ( ReadSQLStmtFromTempFile(TransContext, Command->GetFileKey(), &(Command->SQLStmt())) )
            {
                DeleteSQLStmtFromTempFile(TransContext, Command->GetFileKey(), &(Command->SQLStmt()));
                Command->SetFlgExtern(SAPDB_FALSE);     
                ++mCacheInfoReset.ReloadStmtCount; 
                --mCacheInfoReset.CurrentUnloadStmtCount;
            	return SAPDB_TRUE;
            }
            else
            {
                SQLMan_Context* pContext = (SQLMan_Context*) vGetAcvPtrFromCurrentTask();
                pContext->GetErrorList().AppendNewMessage(
                                            Msg_List(Msg_List::Error, 
                                            SDBMSG_SHAREDSQL_RELOAD_DATA) );
                Deallocate(Command->SQLStmt().mStmt);
                Command->SQLStmt().mStmt = 0;
            }
        }
        else
        {
            SQLMan_Context* pContext = (SQLMan_Context*) vGetAcvPtrFromCurrentTask();
            pContext->GetErrorList().AppendNewMessage(
                                        Msg_List(Msg_List::Error, 
                                        SDBMSG_SHAREDSQL_RELOAD_DATA) );
        }
    }
    return SAPDB_FALSE;
}

SAPDB_Bool SharedSQL_CommandCache::ReloadSQLStmtIntoBuffer( void* Buffer, SAPDB_UInt8 RequestSize, tgg00_TransContext& TransContext, SharedSQL_CachedCommand* Command )
{
	return ReadSQLStmtFromTempFile(TransContext, Command->GetFileKey(), Buffer, RequestSize);
}

//----------------------------------------------------------------------------
SAPDB_Bool SharedSQL_CommandCache::DeleteUnloadedSQLStmt( tgg00_TransContext& TransContext, SharedSQL_CachedCommand* Command )
{
    // Command has to be locked before this method is called
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_CommandCache::DeleteUnloadedSQLStmt", SharedSQL_Trace, 5);
    SAPDBERR_ASSERT_ARGUMENT( Command != 0 );

    if ( Command->GetFlgExtern() )
    {
        if ( DeleteSQLStmtFromTempFile(TransContext, Command->GetFileKey(), &(Command->SQLStmt())) )
        {
            Command->SetFlgExtern(SAPDB_FALSE); 
            --mCacheInfoReset.CurrentUnloadStmtCount;
        }
        else
        {
            SQLMan_Context* pContext = (SQLMan_Context*) vGetAcvPtrFromCurrentTask();
            pContext->GetErrorList().AppendNewMessage(
                                        Msg_List(Msg_List::Error, 
                                        SDBMSG_SHAREDSQL_DELETE_DATA) );
            return SAPDB_FALSE;
        }
    }
	return SAPDB_TRUE;
}

//----------------------------------------------------------------------------

// / ...
//#define MaxTempFileRecSize  MAX_RECLEN_GG00    // MaximumSize of One Record within the TempFile

//----------------------------------------------------------------------------
void SharedSQL_CommandCache::GetTempFileID( tgg00_TransContext& TransContext, tgg00_FileId& FileID )
{
    g04build_temp_tree_id(FileID, TransContext);
    FileID.fileTabId_gg00() = mCacheTableId;
    FileID.fileTfn_gg00().becomes(tfnTemp_egg00);
    FileID.fileType_gg00().addElement(ftsTemp_egg00);
    FileID.fileType_gg00().addElement(ftsConcurrent_egg00);
    FileID.fileType_gg00().addElement(ftsShared_egg00);
}

//----------------------------------------------------------------------------

const int SharedSQL_CommandCache::GetLKeyLen( void ) const
{
	return 2+sizeof(SharedSQL_CommandID);
}

const int SharedSQL_CommandCache::GetUsableRecLen( void ) const
{
	return RECBODY_MXGG00-GetLKeyLen();
}

void SharedSQL_CommandCache::GetLKey(SharedSQL_CommandID ID, int PartNo, tgg00_Lkey& Key, int& KeyLen)
{
    KeyLen = GetLKeyLen();

    Key.keyRecLenSpace_gg00() = cgg_rec_key_offset+KeyLen;
    Key.keyLen_gg00() = KeyLen;
    Key.keyVarOffSpace_gg00() = 0;
    Key.keyVarCntSpace_gg00() = 0;
    char* Data = (char*) &(Key.keyVal_gg00());
    Data[0] = 0;
    SAPDB_MemCopyNoCheck(&(Data[1]), &ID, KeyLen-2);
    Data[KeyLen-1] = (SAPDB_Int1)PartNo;
}

//----------------------------------------------------------------------------
SAPDB_Bool SharedSQL_CommandCache::CreateTempFile( tgg00_TransContext& TransContext )
{
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_CommandCache::CreateTempFile", SharedSQL_Trace, 5);

    if ( mTempFileState == NoFile )
    {
        SharedSQL_LockedScope Lock(mpCreateFileLock, SharedSQL_LockedScope::Exclusive);
        if ( mTempFileState == NoFile )
        {
            tgg00_FileId FileID;
            GetTempFileID(TransContext, FileID);
            b01tcreate_file(TransContext, FileID);
            
            if (TransContext.trError_gg00 == e_ok)
            {
                mTempFileState = FileOk;
                mCacheTableId = FileID.fileTabId_gg00();
            }
            else
            {
                mTempFileState = FileError;
                SQLMan_Context* pContext = (SQLMan_Context*) vGetAcvPtrFromCurrentTask();
                pContext->GetErrorList().AppendNewMessage(
                                            Msg_List(Msg_List::Error, 
                                            SDBMSG_SHAREDSQL_CREATE_TEMP_FILE,
											Msg_OptArg("FileID", (char*)&FileID, 0, sizeof(FileID), _T_h),
											Msg_OptArg("TransError", TransContext.trError_gg00)));
            }
        }
    }
    return (mTempFileState==FileOk);
}

//----------------------------------------------------------------------------
SAPDB_Bool SharedSQL_CommandCache::WriteSQLStmtToTempFile( tgg00_TransContext& TransContext, SharedSQL_CommandID ID, SharedSQL_SQLStmt* Stmt)
{
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_CommandCache::WriteSQLStmtToTempFile", SharedSQL_Trace, 5);

    if ( CreateTempFile(TransContext) )
    {
		tgg00_FileId FileID;
		GetTempFileID(TransContext,FileID);

		long BuffLen = Stmt->mStmtSize + Stmt->mDescriptionSize; // mStmt points to the memory allocated for mStmt+mDescription !!!
		int UsableRecLen = GetUsableRecLen();
        SAPDB_Int1 RecordCount = (SAPDB_Int1) BuffLen/UsableRecLen + (BuffLen%UsableRecLen?1:0);
        for (int i = 1; i<=RecordCount; i++)
        {
            tgg00_Rec Rec;
            int CurrStmtPartLen = (i==RecordCount?BuffLen%UsableRecLen:UsableRecLen);
            int KeyLen = GetLKeyLen();

            Rec.len() = cgg_rec_key_offset + KeyLen + CurrStmtPartLen;
            Rec.keylen() = KeyLen;
            Rec.space_var_offset() = 0;
            Rec.space_var_cnt() = 0;
            char* Data = (char*) &(Rec.info());
            Data[0] = 0;
            SAPDB_MemCopyNoCheck(&(Data[1]), &ID, KeyLen-2);
            Data[KeyLen-1] = (SAPDB_Int1) i;
            SAPDB_MemCopyNoCheck(&(Data[KeyLen]), &(((char*)Stmt->mStmt)[(i-1)*UsableRecLen]), CurrStmtPartLen);

            b02add_record(TransContext, FileID, Rec);
            if ( TransContext.trError_gg00 != e_ok )
            {
                SQLMan_Context* pContext = (SQLMan_Context*) vGetAcvPtrFromCurrentTask();
                pContext->GetErrorList().AppendNewMessage(
                                            Msg_List(Msg_List::Error,
                                            SDBMSG_SHAREDSQL_WRITE_TEMP_FILE,
											Msg_OptArg("FileID", (char*)&FileID, 0, sizeof(FileID), _T_h),
											Msg_OptArg("TransError", TransContext.trError_gg00),
											Msg_OptArg("BuffLen", BuffLen),
											Msg_OptArg("UsableRecLen", UsableRecLen),
											Msg_OptArg("RecordCount", RecordCount),
											Msg_OptArg("i", i)));

                for (int k=i-1; k>0; k--)
                {
                    tgg00_Lkey Key;
                    int KeyLen;
                    GetLKey(ID, k, Key, KeyLen);
                    b02del_record(TransContext, FileID, Key);
                }
                return SAPDB_FALSE;
            }
        }
        return SAPDB_TRUE;
    }
    else
    {
        SQLMan_Context* pContext = (SQLMan_Context*) vGetAcvPtrFromCurrentTask();
        pContext->GetErrorList().AppendNewMessage(
                                    Msg_List(Msg_List::Error,
                                    SDBMSG_SHAREDSQL_WRITE_TEMP_FILE) );
        return SAPDB_FALSE;
    }
}

//----------------------------------------------------------------------------

SAPDB_Bool SharedSQL_CommandCache::ReadSQLStmtFromTempFile( tgg00_TransContext& TransContext, SharedSQL_CommandID ID, SharedSQL_SQLStmt* Stmt)
{
	long RequestSize = Stmt->mStmtSize + Stmt->mDescriptionSize; // mStmt points to the memory allocated for mStmt+mDescription !!!
	void* Buffer = Stmt->mStmt;

	if ( ReadSQLStmtFromTempFile(TransContext, ID, Buffer, RequestSize) )
	{
	    if ( Stmt->mDescriptionSize )
			Stmt->mDescription = ((char*)Stmt->mStmt) + Stmt->mStmtSize; // mStmt point to the memory allocated for mStmt + mDescription !!!
		else
	        Stmt->mDescription = 0;
		return true;
	}
	return false;
}


SAPDB_Bool SharedSQL_CommandCache::ReadSQLStmtFromTempFile( tgg00_TransContext& TransContext, SharedSQL_CommandID ID, void* Buffer, SAPDB_UInt8 RequestSize )
{
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_CommandCache::ReadSQLStmtFromTempFile", SharedSQL_Trace, 5);
    SAPDBERR_ASSERT_ARGUMENT(Buffer);

    tgg00_FileId FileID;
    GetTempFileID(TransContext,FileID);

	int UsableRecLen = GetUsableRecLen();
    SAPDB_Int1 RecordCount = (SAPDB_Int1) RequestSize/UsableRecLen + (RequestSize%UsableRecLen?1:0);
    for (int i = 1; i<=RecordCount; i++)
    {
        tgg00_Lkey Key;
        tgg00_Rec  Rec;
        int KeyLen;

        GetLKey(ID, i, Key, KeyLen);
        b02get_record(TransContext, FileID, Key, Rec);
        if ( TransContext.trError_gg00 == e_ok )
        {
            int CurrStmtPartLen = Rec.len() - cgg_rec_key_offset - KeyLen;
            char* Data = (char*) &(Rec.info());
            SAPDB_MemCopyNoCheck(&(((char*)Buffer)[(i-1)*UsableRecLen]), &(Data[KeyLen]), CurrStmtPartLen);
        }
        else
        {
            SQLMan_Context* pContext = (SQLMan_Context*) vGetAcvPtrFromCurrentTask();
            pContext->GetErrorList().AppendNewMessage(
                                        Msg_List(Msg_List::Error,
                                        SDBMSG_SHAREDSQL_READ_TEMP_FILE,
										Msg_OptArg("FileID", (char*)&FileID, 0, sizeof(FileID), _T_h),
										Msg_OptArg("TransError", TransContext.trError_gg00)));
            return SAPDB_FALSE;
        }
    }
    return SAPDB_TRUE;
}

//----------------------------------------------------------------------------
SAPDB_Bool SharedSQL_CommandCache::DeleteSQLStmtFromTempFile( tgg00_TransContext& TransContext, SharedSQL_CommandID ID, SharedSQL_SQLStmt* Stmt)
{
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_CommandCache::DeleteSQLStmtFromTempFile", SharedSQL_Trace, 5);

    tgg00_FileId FileID;
    GetTempFileID(TransContext,FileID);

    long BuffLen = Stmt->mStmtSize + Stmt->mDescriptionSize; // mStmt points to the memory allocated for mStmt+mDescription !!!
	int UsableRecLen = GetUsableRecLen();
    SAPDB_Int1 RecordCount = (SAPDB_Int1) BuffLen/UsableRecLen + (BuffLen%UsableRecLen?1:0);
    for (int i = 1; i<=RecordCount; i++)
    {
        tgg00_Lkey Key;
        int KeyLen;

        GetLKey(ID, i, Key, KeyLen);
        b02del_record(TransContext, FileID, Key);
        if ( TransContext.trError_gg00 != e_ok )
        {
            SQLMan_Context* pContext = (SQLMan_Context*) vGetAcvPtrFromCurrentTask();
            pContext->GetErrorList().AppendNewMessage(
                                        Msg_List(Msg_List::Error,
                                        SDBMSG_SHAREDSQL_DELETE_TEMP_FILE,
										Msg_OptArg("FileID", (char*)&FileID, 0, sizeof(FileID), _T_h),
										Msg_OptArg("TransError", TransContext.trError_gg00)));
            return SAPDB_FALSE;
        }
    }
    return SAPDB_TRUE;
}

//----------------------------------------------------------------------------

void SharedSQL_CommandCache::AttachCommand( SharedSQL_CachedCommand* Command, SharedSQL_HashValue Slot )
{
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_CommandCache::AttachCommand", SharedSQL_Trace, 5);
    SAPDBERR_ASSERT_ARGUMENT( Command != 0 );
    LRUListAttachCommand(Command);
    HashTableAttachCommand(Command, Slot);
}

void SharedSQL_CommandCache::DetachCommand( SharedSQL_CachedCommand* Command, SharedSQL_HashValue Slot )
{
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_CommandCache::DetachCommand", SharedSQL_Trace, 5);
    SAPDBERR_ASSERT_ARGUMENT( Command != 0 );
    
	LRUListDetachCommand(Command);
    HashTableDetachCommand(Command, Slot);
}

//----------------------------------------------------------------------------
void SharedSQL_CommandCache::HashTableAttachCommand( SharedSQL_CachedCommand* Command, SharedSQL_HashValue Slot )
{
    // Command and HashSlot have to be locked before this method is called
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_CommandCache::HashTableAttachCommand", SharedSQL_Trace, 8);
    SAPDBERR_ASSERT_ARGUMENT( Command != 0 );
    SAPDBERR_ASSERT_RANGE( Slot, 0, mNumberOfSlots-1 );

    // Attach Command at first position
    Command->PrevHashCommand() = 0;           
    if ( mHashTable[Slot] )
    {
        Command->NextHashCommand() = mHashTable[Slot];
        mHashTable[Slot]->PrevHashCommand() = Command;
        mHashTable[Slot] = Command;
    }
    else
    {
        mHashTable[Slot] = Command;
    	++mCacheInfoReset.HashTab_UsedSlotCount;
        if ( mCacheInfoReset.HashTab_UsedSlotCount > mCacheInfoReset.HashTab_MaxUsedSlotCount )
            mCacheInfoReset.HashTab_MaxUsedSlotCount = mCacheInfoReset.HashTab_UsedSlotCount;
    }
}

//----------------------------------------------------------------------------
void SharedSQL_CommandCache::HashTableDetachCommand( SharedSQL_CachedCommand* Command, SharedSQL_HashValue Slot  )
{
    // Command and HashSlot have to be locked before this method is called
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_CommandCache::HashTableDetachCommand", SharedSQL_Trace, 8);
    SAPDBERR_ASSERT_ARGUMENT( Command != 0 );
    SAPDBERR_ASSERT_RANGE( Slot, 0, mNumberOfSlots-1 );

    if ( Command->PrevHashCommand() )
		Command->PrevHashCommand()->NextHashCommand() = Command->NextHashCommand();
	else 
	{
		if ( mHashTable[Slot] == Command ) 
        {
            mHashTable[Slot] = Command->NextHashCommand();
		    if ( mHashTable[Slot] )
                mHashTable[Slot]->PrevHashCommand() = 0;
            else
                --mCacheInfoReset.HashTab_UsedSlotCount;
        }
        else
        {
            SQLMan_Context* pContext = (SQLMan_Context*) vGetAcvPtrFromCurrentTask();
            pContext->GetErrorList().AppendNewMessage(
                                        Msg_List(Msg_List::Error,
                                        SDBMSG_SHAREDSQL_HASHTABLE) );
        }
	}
	if ( Command->NextHashCommand() )
		Command->NextHashCommand()->PrevHashCommand() = Command->PrevHashCommand();
	Command->NextHashCommand() = Command->PrevHashCommand() = 0;
}

//----------------------------------------------------------------------------
void SharedSQL_CommandCache::LRUListMoveCommandToTop( SharedSQL_CachedCommand* Cmd, bool force )
{
    // LRUList has NOT to be locked before this method is called
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_CommandCache::LRUListMoveCommandToTop", SharedSQL_Trace, 5);
    SAPDBERR_ASSERT_ARGUMENT( Cmd != 0 );

    if ( Cmd->GetFlgLRUListOld() || force )  // dirty read
    {
        SharedSQL_LockedScope Lock(mpLRULock, SharedSQL_LockedScope::Exclusive);

        bool oldcmd = Cmd->GetFlgLRUListOld();
        if ( oldcmd || force )  // double check
        {
            SHARED_SQL_TRACE ("SharedSQL_CommandCache LRUMoveToTop[" << ToStr(Cmd->GetCommandID()) << "]" );

            SharedSQL_CachedCommand* Command = REINTERPRET_CAST(SharedSQL_CachedCommand*, Cmd);
            if ( mFirstLRUCommand != Command )
            {
                IncCountLRUListMoveToTop();

                if ( mMiddleLRUCommand == Command )
                {
                    mMiddleLRUCommand = mMiddleLRUCommand->PrevLRUCommand();
                }
                // Detach
                if ( Command->PrevLRUCommand() )
		            Command->PrevLRUCommand()->NextLRUCommand() = Command->NextLRUCommand();
	            if ( Command->NextLRUCommand() )
		            Command->NextLRUCommand()->PrevLRUCommand() = Command->PrevLRUCommand();
	            else 
	            {
		            if ( mLastLRUCommand == Command ) 
			            mLastLRUCommand = Command->PrevLRUCommand();
                    else
                    {
                        SQLMan_Context* pContext = (SQLMan_Context*) vGetAcvPtrFromCurrentTask();
                        pContext->GetErrorList().AppendNewMessage(
                                                    Msg_List(Msg_List::Error,
                                                    SDBMSG_SHAREDSQL_LRULIST) );
                    }
	            }
                // Attach at first position
	            Command->PrevLRUCommand() = 0;
	            if ( mFirstLRUCommand )
                {
                    mFirstLRUCommand->PrevLRUCommand() = Command;
    	            Command->NextLRUCommand() = mFirstLRUCommand;
                }
                mFirstLRUCommand = Command;
                if ( !mLastLRUCommand )
                    mLastLRUCommand = Command;

                if ( oldcmd )
                {
                    Command->SetFlgLRUListOld(false);
                    SAPDBERR_ASSERT_STATE( mMiddleLRUCommand != 0 );
                    mMiddleLRUCommand->SetFlgLRUListOld(true);
                    mMiddleLRUCommand = mMiddleLRUCommand->PrevLRUCommand();
                }
            }
        }
    }
}

//----------------------------------------------------------------------------
void SharedSQL_CommandCache::LRUListAttachCommand( SharedSQL_CachedCommand* Command )
{
    // LRUList has to be locked before this method is called
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_CommandCache::LRUListAttachCommand", SharedSQL_Trace, 8);
    SAPDBERR_ASSERT_ARGUMENT( Command != 0 );

    if ( mFirstLRUCommand )
    {
        mFirstLRUCommand->PrevLRUCommand() = Command;
    	Command->NextLRUCommand() = mFirstLRUCommand;
	    Command->PrevLRUCommand() = 0;
    }
    mFirstLRUCommand = Command;
    if ( !mLastLRUCommand )
        mLastLRUCommand = Command;

    //
    if ( !mMiddleLRUCommand )
        mMiddleLRUCommand = Command;
    ++mLRUCounter;
    if ( mLRUCounter % SharedSQL_OldLRUPart == 0 )
    {
        SAPDBERR_ASSERT_STATE( mMiddleLRUCommand != 0 );
        mMiddleLRUCommand->SetFlgLRUListOld(true);
        mMiddleLRUCommand = mMiddleLRUCommand->PrevLRUCommand();
        SAPDBERR_ASSERT_STATE( mMiddleLRUCommand != 0 );
    }
}

//----------------------------------------------------------------------------
void SharedSQL_CommandCache::LRUListDetachCommand( SharedSQL_CachedCommand* Command )
{
    // LRUList has to be locked before this method is called
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_CommandCache::LRUListDetachCommand", SharedSQL_Trace, 8);
    SAPDBERR_ASSERT_ARGUMENT( Command != 0 );

    if ( Command->GetFlgLRUListOld() )
    {
        if ( mLRUCounter % SharedSQL_OldLRUPart != 0 )
        {
            SAPDBERR_ASSERT_STATE( mMiddleLRUCommand != 0 );
            mMiddleLRUCommand->SetFlgLRUListOld(true);
            mMiddleLRUCommand = mMiddleLRUCommand->PrevLRUCommand();
            SAPDBERR_ASSERT_STATE( mMiddleLRUCommand != 0 );
        }
    }
    else
    {
        if ( mLRUCounter % SharedSQL_OldLRUPart == 0 )
        {
            SAPDBERR_ASSERT_STATE( mMiddleLRUCommand != 0 );
            if ( mMiddleLRUCommand = mMiddleLRUCommand->NextLRUCommand() )
                mMiddleLRUCommand->SetFlgLRUListOld(false);
        }
        if ( Command == (SharedSQL_CachedCommand*)mMiddleLRUCommand )
        {
            SAPDBERR_ASSERT_STATE( mMiddleLRUCommand != 0 );
            mMiddleLRUCommand = mMiddleLRUCommand->PrevLRUCommand();
        }
    }
	--mLRUCounter;

    if ( Command->PrevLRUCommand() )
		Command->PrevLRUCommand()->NextLRUCommand() = Command->NextLRUCommand();
	else 
	{
		if (mFirstLRUCommand == Command) 
			mFirstLRUCommand = Command->NextLRUCommand();
        else
        {
            SQLMan_Context* pContext = (SQLMan_Context*) vGetAcvPtrFromCurrentTask();
            pContext->GetErrorList().AppendNewMessage(
                                        Msg_List(Msg_List::Error,
                                        SDBMSG_SHAREDSQL_LRULIST) );
        }
	}
	if ( Command->NextLRUCommand() )
		Command->NextLRUCommand()->PrevLRUCommand() = Command->PrevLRUCommand();
	else 
	{
		if (mLastLRUCommand == Command) 
			mLastLRUCommand = Command->PrevLRUCommand();
        else
        {
            SQLMan_Context* pContext = (SQLMan_Context*) vGetAcvPtrFromCurrentTask();
            pContext->GetErrorList().AppendNewMessage(
                                        Msg_List(Msg_List::Error,
                                        SDBMSG_SHAREDSQL_LRULIST) );
        }
	}
	Command->NextLRUCommand() = Command->PrevLRUCommand() = 0;
}

//----------------------------------------------------------------------------

SharedSQL_CommandCacheIterator SharedSQL_CommandCache::Begin()
{ 
	SAPDB_Bool Inv = false;
    SharedSQL_CachedCommand* Act = 0;
    SharedSQL_HashValue Slot=0;
	for (SharedSQL_HashValue i=0; (!Act && i<mNumberOfSlots); i++)
    {
        if ( Act = mHashTable[i] )
            Slot = i;
    }
	return SharedSQL_CommandCacheIterator(Act, Slot, mNumberOfSlots, this); 
}

SharedSQL_CommandCacheIterator SharedSQL_CommandCache::End()
{ 
    return SharedSQL_CommandCacheIterator(0, mNumberOfSlots-1, mNumberOfSlots, this); 
}

//----------------------------------------------------------------------------

void SharedSQL_CommandCache::EnterLockForStatisticsIterator( void )
{
	mpCleanUpLock->enter(SharedSQL_LockedScope::NonExclusive);
}

void SharedSQL_CommandCache::LeaveLockForStatisticsIterator( void )
{
	mpCleanUpLock->leave(SharedSQL_LockedScope::NonExclusive);
}

//----------------------------------------------------------------------------
