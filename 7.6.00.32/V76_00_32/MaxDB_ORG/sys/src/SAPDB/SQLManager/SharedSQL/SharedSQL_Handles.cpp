/*!
    @file     SharedSQL_Handles.cpp
    @ingroup  SharedSQL
    @author   DirkT
    @brief    Handles for SharedSQL_Commands
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

#include "SQLManager/SharedSQL/SharedSQL_Handles.hpp"
#include "SQLManager/SharedSQL/SharedSQL_Plan.hpp"
#include "SQLManager/SharedSQL/SharedSQL_Command.hpp"
#include "SQLManager/SharedSQL/SharedSQL_CommandCache.hpp"
#include "SQLManager/SharedSQL/SharedSQL_Types.hpp"

#include "SQLManager/SQLMan_Context.hpp"

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"

#include "heo58.h"

#include "SAPDB/SAPDBCommon/Messages/SDBMsg_SharedSQL.h"

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

//----------------------------------------------------------------------------
SharedSQL_PrepareHandle::SharedSQL_PrepareHandle( SAPDBMem_IRawAllocator* Alloc, SharedSQL_CachedCommand* R, bool& MustPrepare  ) 
    : mAllocator(Alloc)
    , mRef(R)
    , mFlgMustPrepare(SAPDB_FALSE)
    , mOldStatus(New)
	, mpPlan(0)
{ 
    if (mRef)
    {
		mOldStatus = mRef->GetCurrentStatus();
		mpPlan = mRef->GetActPlan();
		SAPDBERR_ASSERT_STATE( (mOldStatus==Prepared?(mpPlan?1:0):1) );

		if ( mFlgMustPrepare = !mRef->HasPreparedPlan() )	// assignment is correct !
		{
			SAPDBERR_ASSERT_STATE( !mpPlan );
            mpPlan = mRef->NewPlan();
	        mRef->SetStatusPreparing();
		}
        mRef->IncPrepareCount(mpPlan);	

		MustPrepare = mFlgMustPrepare;
    }
};

//----------------------------------------------------------------------------
SharedSQL_PrepareHandle& SharedSQL_PrepareHandle::operator=( const SharedSQL_PrepareHandle& H ) 
{
    if ( mRef == H.mRef) 
    {
        mFlgMustPrepare = H.mFlgMustPrepare;
        mOldStatus      = H.mOldStatus;
		mpPlan = H.mpPlan;
        if ( mRef )
           mRef->IncPrepareCount(mpPlan);
    }
    else
    {
        if ( H.mRef )
			H.mRef->IncPrepareCount(mpPlan);
        if ( mRef )
			mRef->DecPrepareCount(mpPlan); 
        mAllocator      = H.mAllocator;
        mRef            = H.mRef;
        mFlgMustPrepare = H.mFlgMustPrepare;
        mOldStatus      = H.mOldStatus;
		mpPlan = H.mpPlan;
    }
    return *this;
}

//----------------------------------------------------------------------------
SharedSQL_PrepareHandle::~SharedSQL_PrepareHandle( void ) 
{ 
    if ( mRef )
        mRef->DecPrepareCount(mpPlan);
}

SAPDBMem_IRawAllocator& SharedSQL_PrepareHandle::GetAllocator(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_PrepareHandle::GetAllocator", SharedSQL_Trace, 5);
    return *mAllocator;
}

//----------------------------------------------------------------------------
void SharedSQL_PrepareHandle::ReleaseHandle( SAPDB_UInt8 ParseTimeSec, SAPDB_UInt8 ParseTimeMicroSec, SQLMan_QueryRewriteMode QRWDone )
{
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_PrepareHandle::ReleaseHandle", SharedSQL_Trace, 5);
    SAPDBERR_ASSERT_STATE( mRef );

    if ( mRef )
    {
        if ( mFlgMustPrepare )
        {
			SAPDB_UInt8 Time = ParseTimeSec*1000000 + ParseTimeMicroSec;
			if ( Time > 0 )
	            mRef->Statistics().SetParseTime(ParseTimeSec*1000000 + ParseTimeMicroSec);

            mRef->SetStatusPrepared(mpPlan);
			mRef->SetQueryRewrite(QRWDone);

			mRef->EnterLockCommand(1);
			mRef->DecPrepareCount(mpPlan);
			mRef->LeaveLockCommand(1);
            mRef->LeaveLockPrepare(1);
        }
        else
        {
			mRef->EnterLockCommand(1);
			mRef->DecPrepareCount(mpPlan);
			mRef->LeaveLockCommand(1);
            mRef->LeaveLockPrepare(0);
        }
        mRef = 0;
    }
}

//----------------------------------------------------------------------------
const SharedSQL_ParseID* SharedSQL_PrepareHandle::GetMasterParseID( void ) const
{
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_PrepareHandle::GetMasterParseID", SharedSQL_Trace, 5);
    SAPDBERR_ASSERT_STATE( mRef && mpPlan );

	if ( mRef && mpPlan )
		return &(mpPlan->GetMasterParseID());
    return 0;
}

//----------------------------------------------------------------------------
void SharedSQL_PrepareHandle::ErrorWhilePreparing( void ) const
{
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_PrepareHandle::ErrorWhileParsing", SharedSQL_Trace, 5);
    SAPDBERR_ASSERT_STATE( mRef );
	SAPDBERR_ASSERT_STATE( mFlgMustPrepare );

    if ( mRef )
		mRef->ErrorWhilePreparing();
}

//----------------------------------------------------------------------------
SAPDB_Bool SharedSQL_PrepareHandle::MustPrepare( SharedSQL_CommandStatus& Status ) const
{
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_PrepareHandle::MustParse", SharedSQL_Trace, 5);
    SAPDBERR_ASSERT_STATE( mRef );

    if ( mRef )
    {
        Status = mOldStatus;
        return mFlgMustPrepare;
    }
    else
    {
        Status = Invalid;
        return SAPDB_TRUE;
    }
}

//----------------------------------------------------------------------------
bool SharedSQL_PrepareHandle::PutPlan( SharedSQL_ParseID MPID, void *const P, SAPDB_ULong Size, Msg_List& MsgList )
{ 
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_PrepareHandle::PutPlan", SharedSQL_Trace, 5);
    SAPDBERR_ASSERT_STATE( mRef && mFlgMustPrepare );
    SAPDBERR_ASSERT_ARGUMENT( P );

    if ( mRef && mFlgMustPrepare && P )
    {
        if ( mpPlan->Insert(MPID, P, Size, MsgList) )
            return true;
    }
    MsgList.AppendNewMessage( Msg_List(Msg_List::Error, SDBMSG_SHAREDSQL_PUTPLAN) );
    return false; 
}

//----------------------------------------------------------------------------

bool SharedSQL_PrepareHandle::PutTable( const SharedSQL_Table& Table, Msg_List& MsgList ) const
{
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_PrepareHandle::PutTable", SharedSQL_Trace, 5);
    SAPDBERR_ASSERT_STATE( mRef && mFlgMustPrepare );

	if ( mRef && mFlgMustPrepare )
	{
        if ( mpPlan->InsertTableInfo(Table, MsgList) )
			return true;
	}
    MsgList.AppendNewMessage( Msg_List(Msg_List::Error, SDBMSG_SHAREDSQL_PUTTABLES) );
    return false;
}

void SharedSQL_PrepareHandle::ClearTables( void ) const
{
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_PrepareHandle::ClearTables", SharedSQL_Trace, 5);
    SAPDBERR_ASSERT_STATE( mRef );

	if ( mRef )
		mRef->ClearTableInfo();
}

//----------------------------------------------------------------------------
const void* SharedSQL_PrepareHandle::GetPlan( void* Key ) const
{
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_PrepareHandle::GetPlan", SharedSQL_Trace, 5);
    SAPDBERR_ASSERT_STATE( mRef && mpPlan );
    SAPDBERR_ASSERT_ARGUMENT( Key );

    if ( mRef && mpPlan )
        return mpPlan->Find(Key);
    return 0;
}

const void*	SharedSQL_PrepareHandle::GetPlanObjectPtr( void ) const
{
	return mpPlan;
}

const SharedSQL_Plan::PlanPartIterator SharedSQL_PrepareHandle::GetPlanPartIterator( void ) const
{
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_PrepareHandle::GetPlanPartIterator", SharedSQL_Trace, 5);
    SAPDBERR_ASSERT_STATE( mRef && mpPlan );

    if ( mRef && mpPlan )
        return mpPlan->GetPlanPartIterator();
	return SharedSQL_Plan::PlanPartIterator();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
SharedSQL_ParseIDHandle::SharedSQL_ParseIDHandle( void ) 
    : mAllocator(0)
    , mRef(0)
    , mMinus9(false)
	, mpPlan(0)
{
}

//----------------------------------------------------------------------------
SharedSQL_ParseIDHandle::SharedSQL_ParseIDHandle( SAPDBMem_IRawAllocator* Alloc, const SharedSQL_PrepareHandle& H, const SAPDB_Bool M9 ) 
    : mAllocator(Alloc)
    , mRef(H.mRef)
    , mMinus9(M9)
	, mpPlan(H.mpPlan)
{ 
    SAPDBERR_ASSERT_STATE( mRef && mpPlan );

    if ( mRef && mpPlan )
        mRef->IncParseIDCount(mpPlan); 
}

//----------------------------------------------------------------------------
SharedSQL_ParseIDHandle& SharedSQL_ParseIDHandle::operator=(const SharedSQL_ParseIDHandle& H ) 
{
    if ( mRef == H.mRef) 
    {
        if ( mRef )
            mRef->IncParseIDCount(mpPlan); 
    }
    else
    {
        if ( H.mRef )
            H.mRef->IncParseIDCount(H.mpPlan);
        if ( mRef )
            mRef->DecParseIDCount(mpPlan);
        mAllocator  = H.mAllocator;
        mRef        = H.mRef;
		mpPlan      = H.mpPlan;
        mMinus9     = H.mMinus9;
    }
    return *this;
}

//----------------------------------------------------------------------------
SharedSQL_ParseIDHandle::~SharedSQL_ParseIDHandle( void ) 
{ 
    if ( mRef )
	{
		SAPDBERR_ASSERT_STATE( mpPlan );
		if ( mpPlan )
	        mRef->DecParseIDCount(mpPlan); 
	}
}

SAPDBMem_IRawAllocator& SharedSQL_ParseIDHandle::GetAllocator(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_ParseIDHandle::GetAllocator", SharedSQL_Trace, 5);
    return *mAllocator;
}

//----------------------------------------------------------------------------
SAPDB_Bool SharedSQL_ParseIDHandle::SetExecuteStatisticData(    
                            SAPDB_UInt8 ExecuteTimeSec, SAPDB_UInt8 ExecuteTimeMicroSec,
                            SAPDB_Int8 ReadRowCount, SAPDB_Int8 QualifiedRowCount,
                            SAPDB_Int8 VirtualReadCount, SAPDB_Int8 PhysicalReadCount,
                            SAPDB_Int8 FetchRowCount, 
                            SAPDB_Int8 SuspendCount, SAPDB_Int8 WaitCount   ) const
{
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_ParseIDHandle::StatisticsData", SharedSQL_Trace, 5);
    SAPDBERR_ASSERT_STATE( mRef != 0 );

    if ( mRef )
    {
        SAPDB_Int8 ExecTime = ExecuteTimeSec*1000000 + ExecuteTimeMicroSec;  // DDT
        mRef->Statistics().IncExecuteTime(ExecTime);
        if (ExecTime > mRef->Statistics().GetMaxExecuteTime())  // DDT !!!
           mRef->Statistics().SetMaxExecuteTime(ExecTime);
        if (ExecTime < mRef->Statistics().GetMinExecuteTime())  // DDT !!!
           mRef->Statistics().SetMinExecuteTime(ExecTime);
        mRef->Statistics().IncReadRowCount(ReadRowCount);	
        mRef->Statistics().IncQualifiedRowCount(QualifiedRowCount);	
        mRef->Statistics().IncVirtualReadCount(VirtualReadCount);
        mRef->Statistics().IncPhysicalReadCount(PhysicalReadCount);
        mRef->Statistics().IncFetchRowCount(FetchRowCount);	
        mRef->Statistics().IncSuspendCount(SuspendCount);
        mRef->Statistics().IncWaitCount(WaitCount);

        return SAPDB_TRUE;
    }
    return SAPDB_FALSE;
}

void SharedSQL_ParseIDHandle::GetCommandID( SharedSQL_CommandID& ID ) const
{
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_ParseIDHandle::GetCommandID", SharedSQL_Trace, 5);
    SAPDBERR_ASSERT_STATE( mRef );

    if ( mRef )
        ID = mRef->GetCommandID();
    else
        ID = 0;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
SharedSQL_ExecuteHandle::SharedSQL_ExecuteHandle( SAPDBMem_IRawAllocator* Alloc, const SharedSQL_ParseIDHandle& H, bool CountExecuting ) 
	: mAllocator(Alloc)
	, mRef(H.mRef)
	, mpPlan(H.mpPlan)
{ 
    SAPDBERR_ASSERT_STATE( mRef && mpPlan );

    if ( mRef && mpPlan )
    {
        mRef->CommandCache().LRUListMoveCommandToTop(mRef, false);

		mRef->IncExecuteCount(mpPlan); 
        if ( CountExecuting )
          mRef->Statistics().IncTotalExecuteCount();

        mMinus9 = H.mMinus9;
    }
};

//----------------------------------------------------------------------------
SharedSQL_ExecuteHandle& SharedSQL_ExecuteHandle::operator=(const SharedSQL_ExecuteHandle& H ) 
{
    if ( mRef == H.mRef) 
    {
        if ( mRef )
            mRef->IncExecuteCount(mpPlan); 
    }
    else
    {
        if ( H.mRef )
           H.mRef->IncExecuteCount(H.mpPlan); 
        if ( mRef )
            mRef->DecExecuteCount(mpPlan); 
        mAllocator  = H.mAllocator;
        mRef        = H.mRef;
        mMinus9     = H.mMinus9;
		mpPlan      = H.mpPlan;
    }
    return *this;
}

//----------------------------------------------------------------------------
SharedSQL_ExecuteHandle::~SharedSQL_ExecuteHandle( void ) 
{ 
    if ( mRef )
	{
		SAPDBERR_ASSERT_STATE( mpPlan );
		if ( mpPlan )
	        mRef->DecExecuteCount(mpPlan);
	}
}

SAPDBMem_IRawAllocator& SharedSQL_ExecuteHandle::GetAllocator(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_ExecuteHandle::GetAllocator", SharedSQL_Trace, 5);
    return *mAllocator;
}

//----------------------------------------------------------------------------
const SharedSQL_ParseID* SharedSQL_ExecuteHandle::GetMasterParseID( void ) const
{
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_ExecuteHandle::GetMasterParseID", SharedSQL_Trace, 5);
    SAPDBERR_ASSERT_STATE( mRef && mpPlan );

    if ( mRef && mpPlan )
        return &(mpPlan->GetMasterParseID());
    return 0;
}

//----------------------------------------------------------------------------

void SharedSQL_ExecuteHandle::SetPlanInvalid( void ) const
{ 
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_ExecuteHandle::SetPlanInvalid", SharedSQL_Trace, 5);
    SAPDBERR_ASSERT_STATE( mRef && mpPlan );

	if ( mRef && mpPlan )
		mRef->Invalidate(mpPlan, Invalid);
}

void SharedSQL_ExecuteHandle::SetSilentReparse( void ) const
{ 
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_ExecuteHandle::SetSilentReparse", SharedSQL_Trace, 5);
    SAPDBERR_ASSERT_STATE( mRef && mpPlan );

	if ( mRef && mpPlan )
		mRef->Invalidate(mpPlan, Dropped);
}

//----------------------------------------------------------------------------
SAPDB_Bool SharedSQL_ExecuteHandle::IsPlanValid( SAPDB_Bool& M9 ) const
{ 
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_ExecuteHandle::IsPlanValid", SharedSQL_Trace, 5);
    SAPDBERR_ASSERT_STATE( mRef && mpPlan );

    if ( mRef && mpPlan )
    {
        M9 = mMinus9;
        return ( mpPlan->GetStatus() == Prepared );	// this is dirty read
    }
	M9 = false;
    return SAPDB_FALSE;
}

//----------------------------------------------------------------------------
void SharedSQL_ExecuteHandle::ReleaseHandle( void )
{ 
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_ExecuteHandle::ReleaseHandle", SharedSQL_Trace, 5);
    SAPDBERR_ASSERT_STATE( mRef && mpPlan );

    if ( mRef )
    {
		mRef->DecExecuteCount(mpPlan);
        mRef = 0;
    }
}

//----------------------------------------------------------------------------
const void* SharedSQL_ExecuteHandle::GetPlan( void* Key ) const
{
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_ExecuteHandle::GetPlan", SharedSQL_Trace, 5);
    SAPDBERR_ASSERT_STATE( mRef && mpPlan );
    SAPDBERR_ASSERT_ARGUMENT( Key != 0);

    if ( mRef && mpPlan )
        return mpPlan->Find(Key);
    return 0;
}

const void*	SharedSQL_ExecuteHandle::GetPlanObjectPtr( void ) const
{
	return mpPlan;
}

//----------------------------------------------------------------------------
const SharedSQL_SQLStmt SharedSQL_ExecuteHandle::GetSQLStmt( const SharedSQL_SQLContext*& Context ) const
{
    SAPDBTRACE_METHOD_DEBUG ("SharedSQL_ExecuteHandle::GetSQLStmt", SharedSQL_Trace, 5);
    SAPDBERR_ASSERT_STATE( mRef != 0 );

    if ( mRef )
    {
        Context = &(mRef->SQLContext());
        return mRef->GetSQLStmt();
    }
    Context = 0;
    return SharedSQL_SQLStmt();
}

//----------------------------------------------------------------------------
