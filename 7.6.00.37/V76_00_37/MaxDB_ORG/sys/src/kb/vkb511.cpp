/*!***************************************************************************

  module      : vkb511.cpp
  responsible : FerdiF
  special area: KB_LockListe
  last changed: 2003-06-17
  Copyright (c) 2000-2005 SAP AG
  see also    :
  description : methods to implement Reader/Writer ConsistView Region


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


*****************************************************************************/

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include "SAPDBCommon/Tracing/SAPDBTrace_Streaming.hpp"
#include "RunTime/Synchronisation/RTESync_RWRegion.hpp"
#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "KernelCommon/Kernel_Trace.hpp"
#include "hgg08.h" 
#include "heo55k.h"     // vbegexcl, vendexcl
#include "gkb05.h"
#include "hkb511.h" // PTS 1124101 UH 2003-09-10 added for type safety

/*-----------------------------------------------------------------------------------*/
static RTESync_SpinlockPool *g_SpinLockPool1   = NULL;
static RTESync_SpinlockPool *g_SpinLockPool2   = NULL; /* PTS 1130354 FF 2004-07-08 */
static RTESync_IRWRegion    *g_IRWLockRegion1  = NULL;
static RTESync_IRWRegion    *g_IRWLockRegion2  = NULL; /* PTS 1130354 FF 2004-07-08 */
/*-----------------------------------------------------------------------------------*/
static void kb511_PrintTransEntry (tkb05_TransEntryPtr entry)
{
	// PTS 1124101 UH 2003-09-10 new

    if ( entry == 0 )
        return;
    
    Kernel_VTrace() << "TRANSENTRY T" << entry->teTaskId_kb05
                    << " [" << entry->teOwnIndex_kb05
                    << "], waiting: " << entry->teWaiting_kb05
                    << ", lockMode: " << entry->teLockMode_kb05
                    << ", reqMode: " << entry->teReqMode_kb05;
}

/*-----------------------------------------------------------------------------------*/
/* PTS 1126669 FF 2003-12-23 */
externPascal void kb511InitRWConsistLocks()
{
    if ( NULL == g_SpinLockPool1) {                   
        g_SpinLockPool1  = reinterpret_cast<RTESync_SpinlockPool*>(
            ( RTEMem_Allocator::Instance())
            .Allocate(sizeof(RTESync_SpinlockPool)) );
        g_SpinLockPool1  = new(g_SpinLockPool1) RTESync_SpinlockPool( UTF8("kb511:ConsistViewRWLock_1 "),1);
        g_IRWLockRegion1 = RTESync_CreateRWRegion( 1, *g_SpinLockPool1, RTEMem_Allocator::Instance());
    }

	/* PTS 1130354 FF 2004-07-08 */
    if ( NULL == g_SpinLockPool2) {                   
        g_SpinLockPool2  = reinterpret_cast<RTESync_SpinlockPool*>(
            ( RTEMem_Allocator::Instance())
            .Allocate(sizeof(RTESync_SpinlockPool)) );
        g_SpinLockPool2  = new(g_SpinLockPool2) RTESync_SpinlockPool( UTF8("kb511:ConsistViewRWLock_2 "),1);
        g_IRWLockRegion2 = RTESync_CreateRWRegion( 1, *g_SpinLockPool2, RTEMem_Allocator::Instance());
    }
}

/*-----------------------------------------------------------------------------------*/

externPascal void kb511EnterConsistView1 (
    tsp00_TaskId                         Taskid,
    pasbool                              exclusiveRequest)
{
    g_IRWLockRegion1->enter((0 != exclusiveRequest), Taskid);
}

/*-----------------------------------------------------------------------------------*/

externPascal void kb511LeaveConsistView1 (
    tsp00_TaskId                         Taskid,
    pasbool                              exclusiveRequest)
{
    g_IRWLockRegion1->leave((0 != exclusiveRequest), Taskid);
}

/*-----------------------------------------------------------------------------------*/
/* PTS 1130354 FF 2004-07-08 */
externPascal void kb511EnterConsistView2 (
    tsp00_TaskId                         Taskid,
    pasbool                              exclusiveRequest)
{
    g_IRWLockRegion2->enter((0 != exclusiveRequest), Taskid);
}

/*-----------------------------------------------------------------------------------*/
/* PTS 1130354 FF 2004-07-08 */
externPascal void kb511LeaveConsistView2 (
    tsp00_TaskId                         Taskid,
    pasbool                              exclusiveRequest)
{
    g_IRWLockRegion2->leave((0 != exclusiveRequest), Taskid);
}

/*-----------------------------------------------------------------------------------*/

externPascal void kb511PrintTabLock (
    tsp00_Int4                           ident,
    tkb05_TabLockPtr                     tab,
    tkb05_TransEntryPtr                  thisTrans,
    pasbool                              collided,
    tgg00_BasisError                     berr)
{
	// PTS 1124101 UH 2003-09-10 new

    if ( tab == 0 )
        return;
    
    Kernel_VTrace trace;
    
    trace << "TAB LOCK ("<<ident<<")" << FlushLine;

    if ( thisTrans != 0 )
    {
        trace << "TAB LOCK THIS TRANS:" << FlushLine;
        kb511_PrintTransEntry(thisTrans);
    }
 
    trace << "TAB LOCK TABID: 0-"
          << (const char*)SAPDB_ToString(tab->tlTabId_kb05.asCharp()+4, 4, SAPDB_ToStringClass::hex)
          << ", collided: " << collided
          << ", berr: "     << SAPDBTrace::BasisError(berr)
          << FlushLine;
    trace << "TAB LOCK IsSysExcl: " << tab->tlIsSysExcl_kb05
          << ", ExcTrIdx: "    << tab->tlExclTransIndex_kb05
          << ", TabOptVers: "  << tab->tlTabOptimVers_kb05
          << ", Share1TrIdx: " << tab->tlShare1TransIndex_kb05
          << ", Share2TrIdx: " << tab->tlShare2TransIndex_kb05
          << FlushLine;
	trace << "TAB LOCK curr lock #: SysShare: "        << tab->tlSysShareLocks_kb05
          << ", TabShare: " << tab->tlTabShareLocks_kb05
          << ", RowShare: " << tab->tlRowShareLocks_kb05
          << ", RowOptim: " << tab->tlRowOptimCnt_kb05
          << ", RowExcl: "  << tab->tlRowExclLocks_kb05
          << FlushLine;

    if ( tab->tlFirstReqTab_kb05 != 0 )
    {
        trace << "TAB LOCK TAB REQUESTS:" << FlushLine;
        tkb05_ReqPtr req = tab->tlFirstReqTab_kb05;
        while ( req != 0 )
        {
            kb511PrintRequest (0, req, e_ok);
            req = req->rqNextReq_kb05;
        }
    }

    if ( tab->tlFirstReqRow_kb05 != 0 )
    {
        trace << "TAB LOCK ROW REQUESTS:" << FlushLine;
        tkb05_ReqPtr req = tab->tlFirstReqRow_kb05;
        while ( req != 0 )
        {
            kb511PrintRequest (0, req, e_ok);
            req = req->rqNextReq_kb05;
        }
    }
}
/*-----------------------------------------------------------------------------------*/
externPascal void kb511PrintRequest (
    tsp00_Int4                           ident,
    tkb05_ReqPtr                         req,
    tgg00_BasisError                     collisionErr)
{
	// PTS 1124101 UH 2003-09-10 new

    if ( req == 0 )
        return;
    
    Kernel_VTrace() << "LOCK REQ ("<<ident<<") T" << req->rqChkTaskId_kb05
                    << "[" << req->rqTransIndex_kb05
                    << "] , berr: " << SAPDBTrace::BasisError(collisionErr)
                    << ", eot: " << req->rqEotWanted_kb05
                    << ", reqMode: " << tgg00_LockReqMode(req->rqReqMode_kb05)
                    << ", lockMode: " << tgg00_LockReqMode(req->rqLockMode_kb05)
                    << ", ownRowLocks(share): " << req->rqRowShareLocks_kb05
                    << ", (excl): " << req->rqRowExclLocks_kb05;
}
/*-----------------------------------------------------------------------------------*/
externPascal void kb511PrintTransEntry (
    tkb05_TransEntryPtr                  entry)
{
	// PTS 1124101 UH 2003-09-10 new

    if ( entry == 0 )
        return;
    
    Kernel_VTrace trace;
    kb511_PrintTransEntry(entry);
    /*
    trace << "TRANSENTRY REQUESTS:";
	kb511PrintRequest (1, entry->teReqTab_kb05); TabTrans
	kb511PrintRequest (2, entry->teReqRow_kb05); RowTrans
    */
/*
        CurrTab := teFirstTab_kb05;
        ItemCnt := 0;
        WHILE (CurrTab <> NIL) AND (ItemCnt <= k51glob.glMaxItemsPerRegion_kb05) DO
            BEGIN
            Uptr1.uiTabTrans_kb05 := CurrTab;
            t01addr (debug, ' TR   Tab   ', Uptr1.uiSupplyItem_kb05);
            CurrRow := CurrTab^.ttFirstRow_kb05;
            WHILE (CurrRow <> NIL) AND (ItemCnt <= k51glob.glMaxItemsPerRegion_kb05) DO
                BEGIN
                Uptr1.uiRowTrans_kb05 := CurrRow;
                t01addr (debug, ' TR   Row   ', Uptr1.uiSupplyItem_kb05);
                ItemCnt := ItemCnt + 1;
                CurrRow := CurrRow^.rtNextRow_kb05
                END;
            (*ENDWHILE*) 
            ItemCnt := ItemCnt + 1;
            CurrTab := CurrTab^.ttNextTab_kb05
            END;
        (*ENDWHILE*) 
        IF  ItemCnt > k51glob.glMaxItemsPerRegion_kb05
        THEN
            k52x_loop_error ('k52xtrans               ', 1)
        (*ENDIF*) 
        END
    (*ENDWITH*) 
ELSE
    t01addr_1 (debug, 'TransEntry  ', TransEntry)
(*ENDIF*) 
END;
*/
}
/*-----------------------------------------------------------------------------------*/
externPascal void kb511PrintWantedLock (
    tsp00_Int4                           ident,
    tkb05_WantedLock      VAR_VALUE_REF  wanted)
{
	// PTS 1124101 UH 2003-09-10 new

    Kernel_VTrace trace;
    trace << "WANTED ("<<ident<<") TABID: 0-"
          << (const char*)SAPDB_ToString(wanted.wlTabId_kb05.asCharp()+4, 4, SAPDB_ToStringClass::hex)
          << FlushLine;
    trace << "WANTED lockmode: " << wanted.wlMode_kb05
          << ", state: ";
    if ( wanted.wlState_kb05.includes(lrsConsistent_egg00) )
        trace << " consistent";
    if ( wanted.wlState_kb05.includes(lrsEotExcl_egg00) )
        trace << " eotExcl";
    if ( wanted.wlState_kb05.includes(lrsForInsert_egg00) )
        trace << " forInsert";
    if ( wanted.wlState_kb05.includes(lrsIgnoreInsert_egg00) )
        trace << " ignoreInsert";
    if ( wanted.wlState_kb05.includes(lrsTemp_egg00) )
        trace << " temp";
	trace << FlushLine;
    trace << "WANTED handling: ";
    if ( wanted.wlHandling_kb05.includes(hdCheckCollision_ekb05) )
        trace << " checkColl";
    if ( wanted.wlHandling_kb05.includes(hdInsLock_ekb05) )
        trace << " insLock";
    if ( wanted.wlHandling_kb05.includes(hdInsReq_ekb05) )
        trace << " insReq";
    if ( wanted.wlHandling_kb05.includes(hdCheckOwnReq_ekb05) )
        trace << " checkOwnReq";
    trace << FlushLine;
    trace << "WANTED keylen: " << wanted.wlKeyLen_kb05
          << ", rowWanted: " << wanted.wlRowWanted_kb05
          << FlushLine;
    if ( wanted.wlRowWanted_kb05 )
        Kernel_TraceBuffer (&(wanted.wlRowId_kb05()), sizeof(wanted.wlRowId_kb05()), "WANTED ROW", sizeof(wanted.wlRowId_kb05()));
}
/*-----------------------------------------------------------------------------------*/
externPascal void kb511PrintSignal (
    tsp00_TaskId                         srcTaskid,
    tsp00_TaskId                         destTaskid,
    pasbool                              destWasWaiting)
{
	// PTS 1124101 UH 2003-09-10 new

    Kernel_VTrace() << "SIGNAL REQUEST: From T" << srcTaskid
                    << " to T"                  << destTaskid
                    << (destWasWaiting ? " (is waiting)" : " (is running)");
}
/*-----------------------------------------------------------------------------------*/
externPascal void kb511WriteStringToTrace (
    tsp00_C40             VAR_ARRAY_REF  string,
    tgg00_TransIndex                     index,
    tgg00_BasisError                     err)
{
	// PTS 1124101 UH 2003-09-10 new

    Kernel_VTrace() << "[" << index << "] err: " << SAPDBTrace::BasisError(err)
                    << ", '" << string.asCharp() << "'";
}
