/*!
  @file        Trans_Iterator.cpp
  @author      UweH
  @ingroup     Transaction
  @brief       implementation of class Trans_Iterator
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
#include "Transaction/Trans_Iterator.hpp"
#include "ggg00.h"
#include "gkb05.h"
#include "hkb52.h"
#include "hgg08.h"
#include "hkb51_2.h"
#include "hkb03_1.h"
#include "heo55k.h" // vbegexcl/vendexcl
#include "Logging/Log_Transaction.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "Transaction/Trans_ExtendedMonitoringContext.hpp"
#include "Transaction/Trans_GlobalContext.hpp"
#include "Transaction/Trans_Context.hpp"
#include "SAPDBCommon/Messages/SDBMsg_Transaction.h"

/// returns the next available global transentry if false is returned an error occured
bool Trans_Iterator::NextTransaction (SAPDB_Int2 &index,
                                      bool        getFirst)
{
    SAPDB_UInt          transCount;
    SAPDB_UInt          transLimit;
    tkb05_RegionNo      currRegion;
	tkb05_TransEntryPtr currTrans;
    
	if ( getFirst )
    {
        index      = cgg_nil_transindex;
        currRegion = 1;
        currTrans  = 0;
    }
    else
    {
        k52acc_trans_entry (index, currTrans);
        index      = cgg_nil_transindex;
        currRegion = currTrans->teOwnRegion_kb05;
        currTrans  = currTrans->teNextTrans_kb05;
        if ( 0 == currTrans )
            currRegion++;
    }
    while ( currRegion <= k51glob.glTransRegions_kb05 )
    {
        if ( getFirst )
        {
            getFirst = false; // although no ref param this is ok
            if ( IsRegionLocked() )
            {
                UnlockRegion();
                return false;
            }
        }
        LockRegion (currRegion);
        if ( 0 == currTrans )
            currTrans = (*k51glob.glRegionGlob_kb05)[currRegion-1]->rglTransGlob_kb05->trFirst_kb05;
        transLimit = k52used_trans_of_region ((*(*k51glob.glRegionGlob_kb05)[currRegion-1]));
        transCount = 0;
        while ( currTrans != 0
                &&
                transCount <= transLimit )
        {
            switch ( m_Type )
            {
                case All:
                default:
                        index = currTrans->teOwnIndex_kb05;
                    break;
                case WriteTransaction:
                    if ( currTrans->teLog_kb05.tlogTransPtr_kb05 != 0 )
                        index = currTrans->teOwnIndex_kb05;
                    break;
                case RedoTransaction:
                    if ( currTrans->teLog_kb05.tlogTransPtr_kb05 != 0
                         &&
                         reinterpret_cast<Log_Transaction*>(currTrans->teLog_kb05.tlogTransPtr_kb05)
                             ->IsOpenInRedo() )
                        index = currTrans->teOwnIndex_kb05;
                    break;
                case HasTempFileDirectory:
                    if ( currTrans->teLocalFileDirPtr_kb05 != 0 )
                        index = currTrans->teOwnIndex_kb05;
                    break;
                case HasMonitoringInfo:
                    if ( currTrans->teExtTransMonPtr_kb05 != 0 )
                        index = currTrans->teOwnIndex_kb05;
                    break;
            }
            if ( index != cgg_nil_transindex ) 
                break; // element found 
            currTrans  = currTrans->teNextTrans_kb05;
            transCount++;
        }
        if ( index != cgg_nil_transindex ) 
            return true; // element found but region is kept
        else
        {
            UnlockRegion();

            currRegion++;

            if (currRegion <= k51glob.glTransRegions_kb05)
                LockRegion (currRegion);
        }
    }

    UnlockRegion();

    return true;
}

/// Locks region 'region'. If lock is already set to a different
/// region, this lock will be released first.
void Trans_Iterator::LockRegion(tkb05_RegionNo region)
{
    if ( m_UseRegion )
    {
        if ( (g08lock0 + region) == m_InRegionId )
            return;

        UnlockRegion();

        m_InRegionId = g08lock0 + region;
        vbegexcl(m_Task.ID(), m_InRegionId);
    }
}

/// Unlocks the current region (if it is locked).
void Trans_Iterator::UnlockRegion()
{
    if ( m_UseRegion
         &&
         TRANS_ITER_NIL_REGION != m_InRegionId )
    {
        vendexcl(m_Task.ID(), m_InRegionId);
        m_InRegionId = TRANS_ITER_NIL_REGION;
    }
}

/// Returns true, if a region is locked.
SAPDB_Bool Trans_Iterator::IsRegionLocked()
{
    if ( m_UseRegion
         &&
         TRANS_ITER_NIL_REGION != m_InRegionId )
        return true;

    return false;
}

/// sets the iterator to the next transaction which matches the given TransType
bool Trans_Iterator::Next()
{
    if ( ! NextTransaction (m_TransIndex, cgg_nil_transindex == m_TransIndex) )
        m_TransIndex = cgg_nil_transindex;

    SAPDBERR_ASSERT_STATE(
        m_UseRegion ?
        (IsValid() ? IsRegionLocked() : true)
        : true );

    return IsValid();
}


/// returns pointer to extended monitoring information block
Trans_ExtendedMonitoringContext* Trans_Iterator::GetMonitoringContext() const
{
    if ( ! IsValid() )
        return 0;
    tkb05_TransEntryPtr currTrans = 0;
    k52acc_trans_entry (m_TransIndex, currTrans);
    if ( 0 == currTrans )
        return 0;
    return reinterpret_cast<Trans_ExtendedMonitoringContext*>(currTrans->teExtTransMonPtr_kb05);
}

/// returns the temp file directory pointer
FileDir_IDirectory* Trans_Iterator::GetTempFileDirContext() const
{
    if ( ! IsValid() )
        return 0;
	tkb05_TransEntryPtr currTrans = 0;
	k52acc_trans_entry (m_TransIndex, currTrans);
    if ( 0 == currTrans )
        return 0;
    return reinterpret_cast<FileDir_IDirectory*>(currTrans->teLocalFileDirPtr_kb05);
}

/// returns log trans context if available
Log_Transaction* Trans_Iterator::GetLogTransPtr() const
{
    if ( ! IsValid() )
        return 0;
	tkb05_TransEntryPtr currTrans = 0;
	k52acc_trans_entry (m_TransIndex, currTrans);
    if ( 0 == currTrans )
        return 0;
    return reinterpret_cast<Log_Transaction*>(currTrans->teLog_kb05.tlogTransPtr_kb05);
}

/// returns the session number
void Trans_Iterator::GetSessionNo (tgg91_SessionNo &sessiono) const
{
    sessiono.gg90SetNil();
    if ( ! IsValid() )
        return;
	tkb05_TransEntryPtr currTrans = 0;
	k52acc_trans_entry (m_TransIndex, currTrans);
    if ( 0 == currTrans )
        return;
    sessiono = currTrans->teSessionId_kb05;
}

/// returns the consistent view number
void Trans_Iterator::GetConsistViewNo (tgg91_TransNo &transno) const
{
    transno.gg90SetNil();
    if ( ! IsValid() )
        return;
	tkb05_TransEntryPtr currTrans = 0;
	k52acc_trans_entry (m_TransIndex, currTrans);
    if (( 0 == currTrans ) || ( currTrans->teConsistView_kb05 == NULL ))
        return;
    transno = currTrans->teConsistView_kb05->cstConsistViewNo_kb05.surrTransId_kb05();
};

/// returns the transaction number
void Trans_Iterator::GetTransNo (tgg91_TransNo &transno) const
{
    transno.gg90SetNil();
    if ( ! IsValid() )
        return;
	tkb05_TransEntryPtr currTrans = 0;
	k52acc_trans_entry (m_TransIndex, currTrans);
    if ( 0 == currTrans )
        return;
    transno = currTrans->teSelectTransId_kb05;
}

/// returns transaction start time
const SAPDB_Char* 
Trans_Iterator::GetTransStartTime(RTESys_SQLTimestamp& timeBuffer)
{
    if ( ! IsValid() )
        return NULL;
    Trans_GlobalContext *pCtx = 
        Trans_GlobalContext::GetContextByTransindex(m_TransIndex);
    if ( NULL == pCtx )
        return NULL;
    return pCtx->GetTransactionStartTimeAsTimeStamp(timeBuffer);
};

/// returns transaction OMS lock mode
tkb05_TransLockMode_Param Trans_Iterator::GetOmsLockMode() const
{
    if ( ! IsValid() )
        return tlmFree_ekb05;
    tkb05_TransEntryPtr currTrans = 0;
    k52acc_trans_entry (m_TransIndex, currTrans);
    if ( 0 == currTrans )
        return tlmFree_ekb05;
    return currTrans->teOmsLockMode_kb05;
};

/// returns transaction SQL lock mode
tkb05_TransLockMode_Param Trans_Iterator::GetSqlLockMode() const
{
    if ( ! IsValid() )
        return tlmFree_ekb05;
    tkb05_TransEntryPtr currTrans = 0;
    k52acc_trans_entry (m_TransIndex, currTrans);
    if ( 0 == currTrans )
        return tlmFree_ekb05;
    return currTrans->teLockMode_kb05;
};

/// returns session SQL lock request timeout
SAPDB_Int Trans_Iterator::GetSqlLockReqTimeout(tkb05_TransLockMode_Param &ReqMode) const
{
    ReqMode = tlmFree_ekb05;
    if ( ! IsValid() )
        return 0;
    tkb05_TransEntryPtr currTrans = 0;
    k52acc_trans_entry (m_TransIndex, currTrans);
    if ( 0 == currTrans )
        return 0;
    if ( currTrans->teObjReq_kb05 == cgg_nil_transindex )
    {
        ReqMode = currTrans->teReqMode_kb05;
        return currTrans->teReqTimeout_kb05;
    }
    return 0;
};

/// returns session SQL lock request timeout
SAPDB_Int Trans_Iterator::GetOmsLockReqTimeout(tkb05_TransLockMode_Param &ReqMode) const
{
    ReqMode = tlmFree_ekb05;
    if ( ! IsValid() )
        return 0;
    tkb05_TransEntryPtr currTrans = 0;
    k52acc_trans_entry (m_TransIndex, currTrans);
    if ( 0 == currTrans )
        return 0;
    if ( currTrans->teObjReq_kb05 != cgg_nil_transindex )
    {
        ReqMode = tlmExcl_ekb05;
        return currTrans->teObjReqTimeout_kb05;
    }
    return 0;
};

/// returns session startdate
SAPDB_Int Trans_Iterator::GetStartDate() const
{
    if ( ! IsValid() )
        return 0;
    tkb05_TransEntryPtr currTrans = 0;
    k52acc_trans_entry (m_TransIndex, currTrans);
    if ( 0 == currTrans )
        return 0;
    return currTrans->teStartDate_kb05;
};

/// returns session starttime
SAPDB_Int Trans_Iterator::GetStartTime() const
{
    if ( ! IsValid() )
        return 0;
    tkb05_TransEntryPtr currTrans = 0;
    k52acc_trans_entry (m_TransIndex, currTrans);
    if ( 0 == currTrans )
        return 0;
    return currTrans->teStartTime_kb05;
};
/// returns Task identification
tgg00_TransIndex Trans_Iterator::GetTaskId() const
{
    if ( ! IsValid() )
        return cgg_nil_transindex;
    tkb05_TransEntryPtr currTrans = 0;
    k52acc_trans_entry (m_TransIndex, currTrans);
    if ( cgg_nil_transindex == currTrans )
        return cgg_nil_transindex;
    return currTrans->teTaskId_kb05;
};

/// returns the transaction number
SAPDB_Int Trans_Iterator::GetTimeLastWrite() const
{
    if ( ! IsValid() )
        return 0;
    tkb05_TransEntryPtr currTrans = 0;
    k52acc_trans_entry (m_TransIndex, currTrans);
    if ( 0 == currTrans )
        return 0;
    return currTrans->teTimeLastWrite_kb05;
};

/// returns the write transno
void Trans_Iterator::GetWriteTransNo (tgg91_TransNo &transno) const
{
    transno.gg90SetNil();
    if ( ! IsValid() )
        return;
	tkb05_TransEntryPtr currTrans = 0;
	k52acc_trans_entry (m_TransIndex, currTrans);
    if ( 0 == currTrans )
        return;
    transno = currTrans->teWriteTransId_kb05;
}
/// if a region was entered it is left now
Trans_Iterator::~Trans_Iterator()
{
    UnlockRegion();
}

/// SelfTest
bool Trans_Iterator::Test(Trans_Context &trans)
{
    Trans_Iterator  iter (trans.Task(), All);
    SAPDB_UInt      numEntries    = 0;
    SAPDB_UInt      numFoundTrans = 0;
    tgg91_TransNo   transno;
    
    while ( iter.Next() )
    {
        ++numEntries;
        iter.GetTransNo(transno);
        if ( ! transno.gg90IsNil() )
            numFoundTrans++;
    }
    if ( 0 == numEntries )
    {
        trans.ErrorList().AppendNewMessage(Msg_List(Msg_List::Error, SDBMSG_TRANSACTION_TRANS_ITERATOR_SELF_TEST_ERROR,
                                                    Msg_Arg("ERRORTEXT","No global trans entry found (expected > 0)"),
                                                    Msg_Arg("NO","0")));
        return false;
    }
    if ( 0 == numFoundTrans )
    {
        trans.ErrorList().AppendNewMessage(Msg_List(Msg_List::Error, SDBMSG_TRANSACTION_TRANS_ITERATOR_SELF_TEST_ERROR,
                                                    Msg_Arg("ERRORTEXT","No open transaction found (expected minimum is 1)"),
                                                    Msg_Arg("NO","0")));
        return false;
    }
    return true;
}
