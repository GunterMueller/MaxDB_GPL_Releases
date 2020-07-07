/****************************************************************************

  module      : vbd930.cpp

  -------------------------------------------------------------------------

  responsible : FerdiF

  special area: GarbageCollection
  description : description ...

  version     : 7.4.4.1
  last changed: 2003-01-22  11:32
  see also    : example.html ...

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG



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

#include "ggg00.h"
#include "gbd05.h"
#include "gbd900.h"
#include "gbd910.h"
#include "gbd930.h"
#include "ggg91.h"
#include "Logging/Log_History.hpp"
#include "Converter/Converter_ICommon.hpp"
#include "Converter/Converter_Types.hpp"
#include "hbd06.h"
#include "hbd17.h"
#include "hbd21.h"
#include "hbd22.h"  //OMS Key Management
#include "heo55k.h" //vbegexcl
#include "heo56.h"  //vsuspend
#include "heo51.h"  //vgetpid
#include "SAPDB/SAPDBCommon/SAPDB_RangeCode.hpp" // Kernel_move_and_fill
#include "SAPDB/SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "hkb51.h"
#include "RunTime/System/RTESys_Time.h"
#include "KernelCommon/Kernel_OpMsg.hpp"           
#include "KernelCommon/Kernel_IAdminInfo.hpp" 
#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "RunTime/Tasking/RTETask_SuspendReason.h"
#include "RunTime/System/RTESys_AtomicOperation.hpp"
#include "Transaction/Trans_Context.hpp"
#include "hkb53.h"
#include "hkb57.h"
#include "hbd91.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define DC_OCCUPANCY_LIMIT_BD930    80
/* maximum occupancy of the datacache in percent */
/* before the garbage collection is started      */

#define CRITICAL_DC_OCCUPANCY_LIMIT_BD930  95
/* maximum occupancy of the datacache in percent before the garbage collectors      */
/* use an accurate but expensive check to decide wheteher an object can be released */

#define HUNDERT_PERCENT_BD930            100

/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL FUNCTIONS (PROTOTYPES)                                             *
 *===========================================================================*/

/*===========================================================================*
 *  GLOBAL FUNCTIONS (CODE)                                                  *
 *===========================================================================*/


/*---------------------------------------------------------------------------*/
/* METHOD IMPLEMENTATION FOR cbd930_HistoryFileInfo                          */
/*---------------------------------------------------------------------------*/

inline bool
cbd930_HistoryFileInfo::bd930ClearHistoryFile (tgg00_TransContext        &Trans,
        GC_IGarbageCollector      &IGarbColl,
        Msg_List                  &errlist) 
{
    bool  bAnyObjectDeleted;   
    if (! Log_History::GetInstance().RemoveHistory ( Trans,
                                                     hfi_bd930.s.m_HistoryFileNo,
                                                     Log_History::Normal,
                                                     IGarbColl, // PTS 1113185 UH 2001-12-18
                                                     bAnyObjectDeleted,
                                                     errlist ) )
	{
        RTE_Message(errlist);
        if(Trans.trError_gg00==e_ok)Trans.trError_gg00=e_history_remove_error;
	}

    return bAnyObjectDeleted;
}

/*---------------------------------------------------------------------------*/
/* METHOD IMPLEMENTATION FOR cbd930_HistoryFileIter                          */
/*---------------------------------------------------------------------------*/

inline cbd930_HistoryFileInfo*
cbd930_HistoryFileIter::bd930SetToHistFileWithOldestTrans(tgg00_TransContext &Trans)
{
    int           Index;
    tgg91_TransNo OldestTransId;
    tgg91_TransNo FirstTransId;

    hitCurrHFInfoPtr_bd930 = NULL;
    hitCurrHFIndex_bd930   = NIL_HISTORY_FILE_INDEX_BD930;

    Log_History& history = Log_History::GetInstance();

    /* find first history file with a nonzero oldest transid */
    for (Index=0; Index < hitHFDir_bd930.hfdNumHFInfos_bd930; ++Index)
    {
        OldestTransId = history.GetOldestTransNo(Index);

        if (!OldestTransId.gg90IsNil())
        {
            hitCurrHFIndex_bd930 = Index;
            break;
        }
    }

    /* check if there is any history file with an older first transid */
    if (NIL_HISTORY_FILE_INDEX_BD930 != hitCurrHFIndex_bd930)
    {
        for (Index=hitCurrHFIndex_bd930; Index < hitHFDir_bd930.hfdNumHFInfos_bd930; ++Index)
        {
            FirstTransId = history.GetOldestTransNo(Index);

            if (!FirstTransId.gg90IsNil() && OldestTransId > FirstTransId)
            {
                hitCurrHFIndex_bd930 = Index;
                OldestTransId        = FirstTransId;
            }
        }

        hitCurrHFInfoPtr_bd930 = hitHFDir_bd930.hfdFirstHFInfoPtr_bd930 + hitCurrHFIndex_bd930;
    }

    return hitCurrHFInfoPtr_bd930;
}

/*---------------------------------------------------------------------------*/
/* METHOD IMPLEMENTATION FOR GC_IGabageCollector Interface                   */
/*---------------------------------------------------------------------------*/


void
cbd930_GarbageCollector::IncHistCreateObjFileCount()
{
    ++gcoController_bd930.gccGCCounters_bd930[gcoIndex_bd930].gccntHistCreateObjFile;
    IncHistEntryReleaseCount();
};

/*---------------------------------------------------------------------------*/

void
cbd930_GarbageCollector::IncHistDeleteObjCount()
{
    ++gcoController_bd930.gccGCCounters_bd930[gcoIndex_bd930].gccntHistDeleteObj;
    IncHistEntryReleaseCount();
};

/*---------------------------------------------------------------------------*/

void
cbd930_GarbageCollector::IncHistDropObjFileCount()
{
    ++gcoController_bd930.gccGCCounters_bd930[gcoIndex_bd930].gccntHistDropObjFile;
    IncHistEntryReleaseCount();
};

/*---------------------------------------------------------------------------*/

void
cbd930_GarbageCollector::IncHistEntryReleaseCount()
{
    ++gcoController_bd930.gccGCCounters_bd930[gcoIndex_bd930].gccntHistEntriesReleased;
};

/*---------------------------------------------------------------------------*/

void
cbd930_GarbageCollector::IncHistLockObjCount()
{
    ++gcoController_bd930.gccGCCounters_bd930[gcoIndex_bd930].gccntHistLockObj;
    IncHistEntryReleaseCount();
};

/*---------------------------------------------------------------------------*/

void
cbd930_GarbageCollector::IncHistInsertObjCount()
{
    ++gcoController_bd930.gccGCCounters_bd930[gcoIndex_bd930].gccntHistInsertObj;
    IncHistEntryReleaseCount();
};

/*---------------------------------------------------------------------------*/

void
cbd930_GarbageCollector::IncHistNewObjCount()
{
    ++gcoController_bd930.gccGCCounters_bd930[gcoIndex_bd930].gccntHistNewObj;
    IncHistEntryReleaseCount();
};

/*---------------------------------------------------------------------------*/

void
cbd930_GarbageCollector::IncHistUpdateObjCount()
{
    ++gcoController_bd930.gccGCCounters_bd930[gcoIndex_bd930].gccntHistUpdateObj;
    IncHistEntryReleaseCount();
};

/*---------------------------------------------------------------------------*/

void
cbd930_GarbageCollector::IncObjReleaseCount()
{
    ++gcoController_bd930.gccGCCounters_bd930[gcoIndex_bd930].gccntObjectsDeleted;
};

/*---------------------------------------------------------------------------*/

void
cbd930_GarbageCollector::IncReleaseEmptyPageCount()
{
    ++gcoController_bd930.gccGCCounters_bd930[gcoIndex_bd930].gccntEmtyPagesReleased;
};

/*---------------------------------------------------------------------------*/

bool
cbd930_GarbageCollector::IsInterrrupted () const
{
	// PTS 1114490 FF 2002-03-12
	if (gcoController_bd930.gccCheckSavepointNeeded_bd930 &&
		Converter_ICommon::Instance().SavepointNeeded())
    {
		gcoController_bd930.gccStopGC_bd930 = true; /* this will stop all GCs */
        return true;
    }
	return( gcoController_bd930.gccSpecialOperationIsActive_bd930 );
};

/*---------------------------------------------------------------------------*/
/* METHOD IMPLEMENTATION FOR cbd930_GarbageCollector                         */
/*---------------------------------------------------------------------------*/

inline bool
cbd930_GarbageCollector::bd930_TryToReleaseUnusedObjDataPages(tgg00_TransContext &Trans)
{
    tsp00_TaskId    WaitingTaskId;
    FileDir_FileNo  ObjFileNo;  
    while (gcoController_bd930.GetFirstWaitForDelEntry (WaitingTaskId, ObjFileNo)) 
    {
        vendexcl (Trans.trTaskId_gg00, g08garbage);
        bd91DropObjFile(Trans, ObjFileNo); 
        if ( cgg_nil_pid != WaitingTaskId)
            vresume(WaitingTaskId);
        vbegexcl (Trans.trTaskId_gg00, g08garbage);
    }
    return false;
}

/*---------------------------------------------------------------------------*/

cbd930_GarbageCollector::cbd930_GarbageCollector (tgg00_TransContext        &Trans,
        cbd930_GarbCollController &GarbCollController)
        : gcoTrans_bd930          (Trans),
        gcoIndex_bd930            (GarbCollController.bd930_AddGC (Trans)),
        gcoController_bd930       (GarbCollController)
{

    ROUTINE_DBG_MEO00  ("cbd930_GarbageCollector");

    gcoInSession_bd930 = false;

    Msg_List&    ErrorList = Trans_Context::GetContext(Trans).ErrorList();

    cbd930_HistoryFileIter					HistFileIter  (GarbCollController.gccHistoryFileDir_bd930);
    bool									bSomethingDone;
    tsp00_TaskId							ResumeTaskId = cgg_nil_pid;  // PTS 1111366 TS 2001-08-10
    cbd930_GCState                         &gcState = gcoController_bd930.gccGCState_bd930[gcoIndex_bd930];
    cbd930_GCCounters                      &gcCounters = gcoController_bd930.gccGCCounters_bd930[gcoIndex_bd930];

    /* allocate the AuxBufferPtr which is needed for the new tree-handling classes */
    /* g10MvAux in vbd600.cpp */
    b21init_temp_cache (Trans.trTaskId_gg00, 
                        false                  /* alloc buffers */,
                        false                  /* no control session*/,
                        Trans.trBdTcachePtr_gg00, Trans.trError_gg00);

    while (true)
    {
        /*=====================================================*/
        vsuspend (Trans.trTaskId_gg00, TSK_VSUSPEND_REASON_NO_WORK);
        /*=====================================================*/

        if (!gcoInSession_bd930)
        {
            k53StartInternalSession (Trans, gcoAuxTrans_bd930, false, false);
            if(Trans.trError_gg00==e_ok)
            {
                gcoInSession_bd930 = true;
            }
            else
            {
                Kernel_OpWarning ( csp3_bd_msg, csp3_n_obj )
                    << __FILE__ << __LINE__ << "internal session start error: "
                    << Trans.trError_gg00;

                Kernel_VTrace ()
                    << __FILE__ << __LINE__ << "internal session start error: "
                    << Trans.trError_gg00;
                Trans.trError_gg00 = e_ok;
            }

        }

        if (gcoInSession_bd930)
        {
            bd930_WriteMessageIntoVtrace(GC_IS_STARTED);

            vbegexcl (Trans.trTaskId_gg00, g08garbage);

            /* check, if garbagecollector is allowed to run */
            if ( !gcoController_bd930.gccSpecialOperationIsActive_bd930 &&
                !gcoController_bd930.gccStopGC_bd930)
            {
                ++gcCounters.gccntActivateCount;

                /*first try to remove empty pages or dropped files */
                bd930_TryToReleaseUnusedObjDataPages(Trans);

                if ( gcoController_bd930.gccHistoryRemovalEnabled )
                    do
                    {
                        cbd930_HistFileEntry expectedVal;
                        cbd930_HistFileEntry newVal;
                        cbd930_HistFileEntry oldVal;

                        vendexcl (Trans.trTaskId_gg00, g08garbage);
                        bSomethingDone = false;

                        /* go trough all history files and try to truncate them */
                        for (HistFileIter.bd930First(); 
                            (NULL != HistFileIter()) && 
                            (HistFileIter()->hfi_bd930.s.m_GcStartCycle != gcState.gcsStartCycle_bd930 ); 
                        ++HistFileIter)
                        {
                            if (gcoController_bd930.gccStopGC_bd930) break;

                            expectedVal.s.m_HistoryFileNo = HistFileIter()->hfi_bd930.s.m_HistoryFileNo;
                            expectedVal.s.m_CurrActivGC   = HistFileIter()->hfi_bd930.s.m_CurrActivGC;
                            expectedVal.s.m_Filler1       = HistFileIter()->hfi_bd930.s.m_Filler1;
                            expectedVal.s.m_GcStartCycle  = HistFileIter()->hfi_bd930.s.m_GcStartCycle;

                            /* remove unused object */
                            if ((expectedVal.s.m_CurrActivGC == NIL_GC_INDEX_BD900)) // &&
                                //(expectedVal.s.m_GcStartCycle != gcState.gcsStartCycle_bd930 ))
                            {
                                newVal = expectedVal;
                                newVal.s.m_CurrActivGC  = gcoIndex_bd930;
                                newVal.s.m_GcStartCycle = gcState.gcsStartCycle_bd930;

                                if ( RTESys_CompareAndExchange( HistFileIter()->hfi_bd930.m_int8,
                                    expectedVal.m_int8, newVal.m_int8, oldVal.m_int8))
                                {

                                    if ( HistFileIter()->bd930ClearHistoryFile (Trans,
                                        static_cast<GC_IGarbageCollector&>(*this), ErrorList)) 
                                    {
                                        bSomethingDone    = true;
                                    }
                                    bd930_HistoryErrorHandler();
                                    HistFileIter()->hfi_bd930.s.m_CurrActivGC = NIL_GC_INDEX_BD900;
                                }
                            }
                        }
                        vbegexcl (Trans.trTaskId_gg00, g08garbage);
                        bd930_TryToReleaseUnusedObjDataPages(Trans);

                        if (gcoController_bd930.gccGCStartRequested_bd930 || bSomethingDone)
                        {
                            gcoController_bd930.gccStartCycle_bd930++;
                            gcoController_bd930.gccGCStartRequested_bd930 = false;
                            bSomethingDone = true;
                        }
                    }
                    while (bSomethingDone && !gcoController_bd930.gccStopGC_bd930);
            }

            if (!Trans.trWriteTransId_gg00.gg90IsNil())
            {
                    Kernel_OpWarning ( csp3_bd_msg, csp3_n_obj )
                        << "Garbagecollector used it's write transaction";
#                   if COMPILEMODE_MEO00 >= QUICK_MEO00 
                    g01abort (csp3_bd_msg, csp3_n_obj,"GC has write trans set  ", Trans.trError_gg00);
#                   else
                    /* because Commit may re-define MinTrans, leave region */
                    vendexcl (Trans.trTaskId_gg00, g08garbage);
                    k53commit(Trans, mm_nil);
                    vbegexcl (Trans.trTaskId_gg00, g08garbage);
#                   endif
            }

            if ( gcoController_bd930.gccStopGC_bd930 ) 
            {
                /* because Commit may re-define MinTrans, leave region */
                vendexcl (Trans.trTaskId_gg00, g08garbage);
                k53StopInternalSession (Trans, gcoAuxTrans_bd930, m_commit);
                if(Trans.trError_gg00!=e_ok)
                {
                    Kernel_OpWarning ( csp3_bd_msg, csp3_n_obj )
                        << __FILE__ << __LINE__ << "internal session stop error: "
                        << Trans.trError_gg00;

                    Kernel_VTrace ()
                        << __FILE__ << __LINE__ << "internal session stop error: "
                        << Trans.trError_gg00;
                    g01abort (csp3_bd_msg, csp3_n_obj,"k53StopInternalSession: ", Trans.trError_gg00);
                }
                gcoInSession_bd930 = false;
                vbegexcl (Trans.trTaskId_gg00, g08garbage);
            }
            gcoController_bd930.TerminateGarbagecollector(gcoIndex_bd930, ResumeTaskId);
#           if COMPILEMODE_MEO00 >= QUICK_MEO00 
            gcoController_bd930.bd930_CheckGC();
#           endif
            vendexcl (Trans.trTaskId_gg00, g08garbage);

            if (cgg_nil_pid != ResumeTaskId)
            {
                /*=======================================================================*/
                vresume (ResumeTaskId);
                /*=======================================================================*/

                ResumeTaskId = cgg_nil_pid;
            };
            bd930_WriteMessageIntoVtrace(GC_IS_STOPPED); // PTS 1111366
        }
    };
}

/*---------------------------------------------------------------------------*/

void
cbd930_GarbageCollector::bd930_WritePageStatusIntoVtrace (tgg00_TransContext    &Trans,
        tbd_nodeptr            pPage,
        tgg00_VtraceType_Enum  TraceObject)
{
    ROUTINE_DBG_MEO00 ("bd930_WritePageStatusIntoVtrace");

    tgg12_OmsPageStatTrace     TrLinePage;

    TrLinePage.opsTrType_gg12.becomes(otPageStat_egg12);
    TrLinePage.opsTrType2_gg12.becomes(ot2Nil_egg12);
    TrLinePage.opsPno_gg12           = pPage->nd_id();
    TrLinePage.opsOccObjCnt_gg12     = pPage->nd_occ_obj_cnt();
    TrLinePage.opsFreeObjCnt_gg12    = pPage->nd_free_obj_cnt();
    TrLinePage.opsFreeEotObjCnt_gg12 = pPage->nd_free_eot_obj_cnt();
    TrLinePage.opsResObjCnt_gg12     = pPage->nd_max_obj_cnt()
                                       -pPage->nd_occ_obj_cnt()
                                       -pPage->nd_free_obj_cnt()
                                       -pPage->nd_free_eot_obj_cnt();

    Trace_CommonEntry entry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(TraceObject),
                       REINTERPRET_CAST( tsp00_BytePtr, &TrLinePage ), sizeof (TrLinePage) );
}



/*---------------------------------------------------------------------------*/
/* METHOD IMPLEMENTATION FOR cbd930_GarbCollController                       */
/*---------------------------------------------------------------------------*/

void 
cbd930_GarbCollController::TriggerGarbageCollection( tsp00_TaskId  TaskId
                                                    ,bool          withHighPriority )
{ 
    if (gccTryingToStartGc_bd930 || 
        ((SAPDB_FALSE == withHighPriority) && (gccNumGCUsed_bd930 > 0)))
        gccGCStartRequested_bd930 = true;
    else
        bd930StartAllGCs (TaskId, 0 /*act time*/, !(SAPDB_FALSE == withHighPriority) /* only 1 gc or all to be started */);
}

/*---------------------------------------------------------------------------*/

void
cbd930_GarbCollController::RegisterHistoryFile (tgg00_TransContext &Trans, 
                                                SAPDB_UInt4         HistoryFileNo)
{
    vbegexcl (Trans.trTaskId_gg00, g08garbage); 
    /* make sure no GC is using the history file iterator */
    if ( gccNumGCUsed_bd930 <= 0 )
        gccHistoryFileDir_bd930.bd930AddHistoryFile (Trans, HistoryFileNo);
    else
        g01abort (csp3_bd_msg, csp3_n_obj, "RegisterHistoryFile #GCs", gccNumGCUsed_bd930);
    vendexcl (Trans.trTaskId_gg00, g08garbage); 
}

/*---------------------------------------------------------------------------*/

void
cbd930_GarbCollController::ClearAll ()
{
    tsp00_TaskId taskId;
    vgetpid( taskId );

    vbegexcl (taskId, g08garbage);
    /* make sure no GC is using the history file iterator */
    if ( gccNumGCUsed_bd930 <= 0 )
        gccHistoryFileDir_bd930.bd930ClearAll();
    else
        g01abort (csp3_bd_msg, csp3_n_obj, "Clear HistoryFile #GCs: ", gccNumGCUsed_bd930);
    vendexcl (taskId, g08garbage); 
    gccHistoryRemovalEnabled = false;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt8 
cbd930_GarbCollController::GetGCLastTimeStarted() 
{
    return gccLastTimeGCsStarted;
}

/*---------------------------------------------------------------------------*/

void
cbd930_GarbCollController::bd930DumpGCController (tgg00_VfFileref     &HostFile,
        tsp00_Int4            BufferSize,
        tsp00_Page           &Buffer,
        tsp00_Int4           &OutPno,
        tsp00_Int4           &OutPos,
        tsp00_VfReturn       &HostError,
        tsp00_ErrText        &ErrText)
{
    ROUTINE_DBG_MEO00 ("bd930DumpGCController");

    tsp00_Int2        CodeVars        = 252;
    const char       *MarkVars        = "B91VARS "; // PTS 1104704 TS 1999-12-02
    tsp00_Int2        CodeGCState     = 253;
    const char       *MarkGCState     = "B91GCSTA"; // PTS 1104704 TS 1999-12-02

    tgg00_BasisError  MoveError       = e_ok;

    // OutPos is handled as a Pascal (!) Position
    // only for C-Routines 1 must be subtracted

    HostError.becomes(vf_ok);

    g01new_dump_page (HostFile, Buffer, OutPno, OutPos, HostError, ErrText);

    // --- GARBAGE COLLECTOR GLOBALS ---

    if (vf_ok == HostError)
    {
        SAPDB_RangeMove( __FILE__, 1,
                DUMPMARKSIZE_BD900, BufferSize, MarkVars, 1,
                Buffer, OutPos, DUMPMARKSIZE_BD900 , MoveError);// PTS 1104704 TS 1999-12-02
        MoveError = e_ok; // ignore error
        OutPos += DUMPMARKSIZE_BD900;

        SAPDB_MemCopyNoCheck (Buffer + OutPos - 1, &CodeVars, sizeof(tsp00_Int2));
        OutPos += sizeof(tsp00_Int2);

        SAPDB_MemCopyNoCheck (Buffer + OutPos - 1, &gccNumGC_bd930, sizeof(gccNumGC_bd930));
        OutPos += sizeof(gccNumGC_bd930);

        SAPDB_MemCopyNoCheck (Buffer + OutPos - 1, &gccNumGCUsed_bd930, sizeof(gccNumGCUsed_bd930));
        OutPos += sizeof(gccNumGCUsed_bd930);

        SAPDB_MemCopyNoCheck (Buffer + OutPos - 1, &gccStartingGCs, sizeof(gccStartingGCs));
        OutPos += sizeof(gccStartingGCs);

        SAPDB_MemCopyNoCheck (Buffer + OutPos - 1, &gccStopGCManagerTID_bd930, sizeof(gccStopGCManagerTID_bd930));
        OutPos += sizeof(gccStopGCManagerTID_bd930);

        SAPDB_MemCopyNoCheck (Buffer + OutPos - 1, &gccStopGC_bd930, sizeof(gccStopGC_bd930));
        OutPos += sizeof(gccStopGC_bd930);

        SAPDB_MemCopyNoCheck (Buffer + OutPos - 1, &gccSpecialOperationIsActive_bd930, sizeof(gccSpecialOperationIsActive_bd930)); // PTS 1113552 FF
        OutPos += sizeof(gccSpecialOperationIsActive_bd930); // PTS 1113552 FF

        SAPDB_MemCopyNoCheck (Buffer + OutPos - 1, &gccCheckSavepointNeeded_bd930, sizeof(gccCheckSavepointNeeded_bd930));
        OutPos += sizeof(gccCheckSavepointNeeded_bd930);
    }

    // --- GARBAGE COLLECTOR STATE ---

    if (vf_ok == HostError)
    {
        SAPDB_RangeMove( __FILE__, 2,
                DUMPMARKSIZE_BD900, BufferSize, MarkGCState, 1,
                Buffer, OutPos, DUMPMARKSIZE_BD900 , MoveError);// PTS 1104704 TS 1999-12-02
        MoveError = e_ok; // ignore error
        OutPos += DUMPMARKSIZE_BD900;

        SAPDB_MemCopyNoCheck (Buffer + OutPos - 1, &CodeGCState, sizeof(tsp00_Int2));
        OutPos += sizeof(tsp00_Int2);

        SAPDB_MemCopyNoCheck (Buffer + OutPos - 1, &gccNumGC_bd930, sizeof(gccNumGC_bd930));
        OutPos += sizeof(gccNumGC_bd930);

        for (int iGC=0; iGC<MAX_OBJ_GARB_COLL_BD00; ++iGC)
        {
            if (cgg_nil_pid != gccGCState_bd930[iGC].gcsTaskId_bd930)
            {
                SAPDB_MemCopyNoCheck (Buffer + OutPos - 1, &gccGCState_bd930[iGC].gcsTaskId_bd930,
                        sizeof(gccGCState_bd930[iGC].gcsTaskId_bd930));
                OutPos += sizeof(gccGCState_bd930[iGC].gcsTaskId_bd930);

                SAPDB_MemCopyNoCheck (Buffer + OutPos - 1, &gccGCState_bd930[iGC].gcsActive_bd930,
                        sizeof(gccGCState_bd930[iGC].gcsActive_bd930));
                OutPos += sizeof(gccGCState_bd930[iGC].gcsActive_bd930);
            }
        }
    }

    /* dump member variable gccHistoryFileDir_bd930 */
    if (vf_ok == HostError)
    {
        gccHistoryFileDir_bd930.bd930DumpHistoryFileDir (HostFile,
                BufferSize, Buffer, OutPno, OutPos, HostError,ErrText);
    }
}

/*---------------------------------------------------------------------------*/

void
cbd930_GarbCollController::bd930_CheckGC ()
{
}

/*-----------------------------------------------------------*/
// PTS 1111366 TS 2001-08-10; PTS 1113552 FF, PTS 1116157 FF 2002-06-07
void
cbd930_GarbCollController::bd930StartAllGCs (tsp00_TaskId   TaskId,
                                             SAPDB_UInt8    actTime,
                                             bool           startall)
{
    ROUTINE_DBG_MEO00  ("bd930StartAllGCs");

    if (!gccSpecialOperationIsActive_bd930) 
    {
        if ( ! Log_History::GetInstance().AnyHistoryExists()
             &&
             ! m_pWaitForDelObjFileQueue->AnyObjFileToDelete() )
            return;

        int iGC;
        tsp00_TaskId toStartTaskId[MAX_OBJ_GARB_COLL_BD00];
        int toStartTaskIdCnt = 0;

        gccTryingToStartGc_bd930 = true;

        vbegexcl (TaskId, g08garbage); 
        if (!gccSpecialOperationIsActive_bd930) 
        {

            gccGCStartRequested_bd930 = false;

            /* only if all GCs are started the timestamp should be changed */
            if (startall)
            {
                if ( 0 == actTime )
                    gccLastTimeGCsStarted = RTESys_Time();
                else
                    gccLastTimeGCsStarted = actTime;
            }

            gccStartCycle_bd930++;
            for (iGC=0; iGC<gccNumGC_bd930; ++iGC)
            {
                /* search for suspended GCs */
                if (!gccGCState_bd930[iGC].gcsActive_bd930)
                {
                    /* remove this GC from the pool of suspended GCs */
                    gccGCState_bd930[iGC].gcsActive_bd930 = true;
                    gccNumGCUsed_bd930 ++;
                    gccGCState_bd930[iGC].gcsStartCycle_bd930 = gccStartCycle_bd930;
                    toStartTaskId[toStartTaskIdCnt] = gccGCState_bd930[iGC].gcsTaskId_bd930;
                    ++toStartTaskIdCnt;
                    if ( !startall ) 
                        break;
                }
            }
        }
        gccTryingToStartGc_bd930 = false;
        vendexcl (TaskId, g08garbage); 

        /* resume GCs */
        for (iGC=0; iGC<toStartTaskIdCnt; ++iGC)
        {
            /* ============================================================== */
            vresume (toStartTaskId[iGC]);
            /* ============================================================== */
        }
    }
}

/*-----------------------------------------------------------*/
void 
cbd930_GarbCollController::bd930CheckRetentionTime(tsp00_TaskId TaskId)
{
    Converter_GarbageCollectionReturnCode retCode = 
        Converter_ICommon::Instance().GarbageCollectionNeeded();

    tgg91_TransNo  ConsistViewNo;
    tsp00_Int4     StartDate;
    tsp00_Int4     StartTime;
    SAPDB_UInt8    ActTime = RTESys_Time();
    SAPDB_UInt8    cancelTimeSlice  = TIMEOUT_CHECK_CANCELLED;
    bool           bUseMinRetentionTime = false;

    tgg91_TransNo   MinTransId;
    pasbool         RetainHistory = 0;
    kb51GetMinTrans ( TaskId, MinTransId, RetainHistory );
    if (!RetainHistory &&
        (TIMEOUT_CHECK_CANCELLED_RED < (ActTime - gccLastTimeGCsStarted)))
    {
        tgg91_TransNo   ActWriteTransId;
        kb57GetLastWriteTransNo(TaskId, ActWriteTransId);
        if ( ActWriteTransId != gccLastWriteTransNo )
        {
            gccLastWriteTransNo = ActWriteTransId;
            bd930StartAllGCs (TaskId, ActTime);
        }
    }

    if (Converter_GCNeeded == retCode)
    {
        bUseMinRetentionTime = true;
        cancelTimeSlice  = TIMEOUT_CHECK_CANCELLED_RED;
    }

    if ((cancelTimeSlice < (ActTime - gccLastTimeCancelCheck)) &&
        Kernel_IAdminInfo::Instance().KernelStateIsOnline()) 
    {
        gccLastTimeCancelCheck = ActTime;
        kb51CancelConsistTrans (TaskId, ConsistViewNo, 
            StartDate, StartTime, bUseMinRetentionTime);
        if (!ConsistViewNo.gg90IsNil())
        {
            int i;
            Kernel_OpWarning  opWarnMsg( csp3_bd_msg, csp3_n_obj );
            opWarnMsg << "ConsistView:";
            for (i=0; i < sizeof(ConsistViewNo); i++)
                opWarnMsg << ToStr((SAPDB_UInt1)ConsistViewNo[i], 2, _T_z, _T_h);
            opWarnMsg << " cancelled - creation date:" << StartDate
                << " time:" << StartTime;

            Kernel_VTrace     vTrace;
            vTrace << "ConsistView:";
            for (i=0; i < sizeof(ConsistViewNo); i++)
                vTrace << ToStr((SAPDB_UInt1)ConsistViewNo[i], 2, _T_z, _T_h);
            vTrace << " cancelled - creation date:" << StartDate
                << " time:" << StartTime;
        }
        else
            bd930StartGCsIfRequired (TaskId);
    }
};

/*-----------------------------------------------------------*/
/* PTS 1116157 FF 2002-06-11                                 */
/* FF 03-SEP-2002 PTS 1117682                                */
/* GC MUST never call bd930StartGCsIfRequired because        */
/* if "startExactlyOneGC == true" and other GCs are active   */
/* GC will wait forever within bd930StopAllGCsForSpecialOp   */
void
cbd930_GarbCollController::bd930StartGCsIfRequired (tsp00_TaskId   TaskId)
{
    if (!gccHistoryRemovalEnabled)
        return;

    if ( gccNumGCUsed_bd930 == gccNumGC_bd930)
        return;

    SAPDB_Int4 expectedVal = gccStartingGCs;
    SAPDB_Int4 newVal = 1;
    SAPDB_Int4 oldVal;
    if ( gccStartingGCs )
        return;
    if ( !RTESys_CompareAndExchange( gccStartingGCs, expectedVal, newVal, oldVal ))
        return;

    SAPDB_UInt8 gccActTime = RTESys_Time();
    bool startExactlyOneGC = false;
    
    Converter_GarbageCollectionReturnCode retCode = 
        Converter_ICommon::Instance().GarbageCollectionNeeded();
    
    if (Converter_GCNeeded == retCode)
    {
        /* if (gccCheckSavepointNeeded_bd930 == true)                                  */
        /*     cbd930_GarbageCollector::IsInterrrupted() check filling of devspaces    */
        if (gccCheckSavepointNeeded_bd930 == false) {
            Kernel_OpWarning  opWarnMsg( csp3_bd_msg, csp3_n_obj );
            opWarnMsg << "Garbagecollection enters phase red";
        }
        gccCheckSavepointNeeded_bd930 = true;        
    }
    else if (Converter_GCNotNeeded == retCode)
    { 
        if (gccCheckSavepointNeeded_bd930) {
            Kernel_OpWarning  opWarnMsg( csp3_bd_msg, csp3_n_obj );
            opWarnMsg << "Garbagecollection leaves phase red";
        }
        gccCheckSavepointNeeded_bd930 = false;
    }
    bd930StartAllGCs (TaskId, gccActTime);
    gccStartingGCs = 0;
}

/*-----------------------------------------------------------*/

void
cbd930_GarbCollController::bd930StopAllGCsForSpecialOp (tsp00_TaskId    TaskId)
{
    ROUTINE_DBG_MEO00  ("bd930StopAllGCs");

    /* This routine stops all running garbage collectors */

    tsp00_TaskId ResumeTaskId  = cgg_nil_pid; // FF 03-SEP-2002 PTS 1117682 

    int iGC;
    tsp00_TaskId toStartTaskId[MAX_OBJ_GARB_COLL_BD00];
    int toStartTaskIdCnt = 0;

    vbegexcl (TaskId, g08garbage);

    gccStartCycle_bd930++;
    for (iGC=0; iGC<gccNumGC_bd930; ++iGC)
    {
      /* search for suspended GCs */
      if (!gccGCState_bd930[iGC].gcsActive_bd930)
      {
        /* remove this GC from the pool of suspended GCs */
        gccGCState_bd930[iGC].gcsActive_bd930 = true;
        gccNumGCUsed_bd930 ++;
        gccGCState_bd930[iGC].gcsStartCycle_bd930 = gccStartCycle_bd930;
        toStartTaskId[toStartTaskIdCnt] = gccGCState_bd930[iGC].gcsTaskId_bd930;
        ++toStartTaskIdCnt;
      }
    }

    gccSpecialOperationIsActive_bd930 = true;      // PTS 1113552 FF
    if (gccStopGC_bd930)                           // FF 03-SEP-2002 PTS 1117682
      ResumeTaskId = gccStopGCManagerTID_bd930;    // FF 03-SEP-2002 PTS 1117682
    else                                           // FF 03-SEP-2002 PTS 1117682
      gccStopGC_bd930 = true;
    gccStopGCManagerTID_bd930  = TaskId;

    vendexcl (TaskId, g08garbage);

    /* resume GCs */
    for (iGC=0; iGC<toStartTaskIdCnt; ++iGC)
    {
      /* ============================================================== */
      vresume (toStartTaskId[iGC]);
      /* ============================================================== */
    }

    g01opmsg (sp3p_knldiag, sp3m_info, csp3_bd_msg, csp3_n_obj,
      "Stop active garbage coll", gccNumGCUsed_bd930);

    /*=====================================================*/
    vsuspend (TaskId, 229);
    /*=====================================================*/

    g01opmsg (sp3p_knldiag, sp3m_info, csp3_bd_msg, csp3_n_obj,
      "Garbage coll stopped    ", gccNumGCUsed_bd930);

    if (cgg_nil_pid != ResumeTaskId)  /* FF 03-SEP-2002 PTS 1117682 */
    {
      /*=======================================================================*/
      vresume (ResumeTaskId);  /*FF 03-SEP-2002 PTS 1117682 */
      /*=======================================================================*/
    };

    /* check if really all GCs are suspended */
    if ((0<gccNumGCUsed_bd930) || gccStopGC_bd930 )
    {
        tsp00_Int4 expectedVal = 0;
        tsp00_Int4 newVal = 0;
        tsp00_Int4 oldVal;

        if ( !RTESys_CompareAndExchange( gccNumGCUsed_bd930, expectedVal, newVal, oldVal))
        {
#           if COMPILEMODE_MEO00 >= QUICK_MEO00 
            g01abort (csp3_bd_msg, csp3_n_obj, "BD91_StopGC: GC ACTIVE  ", oldVal);
#           else
            g01opmsg (sp3p_knldiag, sp3m_error, csp3_bd_msg, csp3_n_obj, 
              "BD91_StopGC: GC ACTIVE  ", gccNumGCUsed_bd930);
#           endif
        }
    }
}

/*---------------------------------------------------------------------------*/
/* METHOD IMPLEMENTATION FOR cbd930_HistoryFileDir                           */
/*---------------------------------------------------------------------------*/

void
cbd930_HistoryFileDir::bd930DumpHistoryFileDir (tgg00_VfFileref     &HostFile,
        tsp00_Int4            BufferSize,
        tsp00_Page           &Buffer,
        tsp00_Int4           &OutPno,
        tsp00_Int4           &OutPos,
        tsp00_VfReturn       &HostError,
        tsp00_ErrText        &ErrText)
{
    ROUTINE_DBG_MEO00 ("bd930DumpHistoryFileDir");

    tsp00_Int2        CodeVars        = 256;
    const char       *MarkVars        = "B930HFD ";
    tgg00_BasisError  MoveError       = e_ok;

    // OutPos is handled as a Pascal (!) Position
    // only for C-Routines 1 must be subtracted

    HostError.becomes(vf_ok);

    g01new_dump_page (HostFile, Buffer, OutPno, OutPos, HostError, ErrText);

    if (vf_ok == HostError)
    {
        SAPDB_RangeMove( __FILE__, 3,
                DUMPMARKSIZE_BD900, BufferSize, MarkVars, 1,
                Buffer, OutPos, DUMPMARKSIZE_BD900 , MoveError);
        MoveError = e_ok; // ignore error
        OutPos += DUMPMARKSIZE_BD900;

        SAPDB_MemCopyNoCheck (Buffer + OutPos - 1, &CodeVars, sizeof(tsp00_Int2));
        OutPos += sizeof(tsp00_Int2);

        SAPDB_MemCopyNoCheck (Buffer + OutPos - 1, &hfdNumHFInfos_bd930, sizeof(hfdNumHFInfos_bd930));
        OutPos += sizeof(hfdNumHFInfos_bd930);

        SAPDB_MemCopyNoCheck (Buffer + OutPos - 1, &hfdMaxNumHFInfos_bd930, sizeof(hfdMaxNumHFInfos_bd930));
        OutPos += sizeof(hfdMaxNumHFInfos_bd930);

        SAPDB_Bool1 isInitialized = Log_History::IsInitialized();

        SAPDB_MemCopyNoCheck (Buffer + OutPos - 1, &isInitialized, sizeof(isInitialized));
        OutPos += sizeof(bool);

        if ( Log_History::IsInitialized() )
        {
            Log_History &history = Log_History::GetInstance();

            /* +++ PTS 1122219 2003-05-21 FF +++ */
            cbd930_HistoryFileInfo *HFInfoDummy = NULL;
            tsp00_Int4 maxEntryOnPage = (BufferSize - OutPos + sizeof(maxEntryOnPage)) /
                (sizeof(Log_History::ExtendedHistoryFileInfo) +
                sizeof(HFInfoDummy->hfi_bd930.s.m_CurrActivGC)   +
                sizeof(HFInfoDummy->hfi_bd930.s.m_Filler1));
            
            if ( hfdNumHFInfos_bd930 < maxEntryOnPage )
                maxEntryOnPage = hfdNumHFInfos_bd930;
            SAPDB_MemCopyNoCheck (Buffer + OutPos - 1, &maxEntryOnPage, sizeof(maxEntryOnPage));
            OutPos += sizeof(maxEntryOnPage);
            /* --- PTS 1122219 2003-05-21 FF --- */

            for (int iHFI=0; iHFI<hfdNumHFInfos_bd930; ++iHFI)
            {
                cbd930_HistoryFileInfo &HFInfo = *(hfdFirstHFInfoPtr_bd930 + iHFI);

                /* PTS 1122219 2003-05-21 FF */
                if ( (tsp00_Uint4)BufferSize < 
                    (OutPos + sizeof(Log_History::ExtendedHistoryFileInfo) +
                    sizeof(HFInfo.hfi_bd930.s.m_CurrActivGC)   +
                    sizeof(HFInfo.hfi_bd930.s.m_Filler1)))
                {
                    g01new_dump_page (HostFile, Buffer, OutPno, OutPos, HostError, ErrText);
                    
                    if (vf_ok != HostError)
                        return;

                    SAPDB_RangeMove( __FILE__, 4,    
                        DUMPMARKSIZE_BD900, BufferSize, MarkVars, 1,
                        Buffer, OutPos, DUMPMARKSIZE_BD900 , MoveError);
                    MoveError = e_ok; // ignore error 
                    OutPos += DUMPMARKSIZE_BD900;
                    
                    SAPDB_MemCopyNoCheck (Buffer + OutPos - 1, &CodeVars, sizeof(tsp00_Int2));
                    OutPos += sizeof(tsp00_Int2);
                    
                    SAPDB_MemCopyNoCheck (Buffer + OutPos - 1, &hfdNumHFInfos_bd930, sizeof(hfdNumHFInfos_bd930));
                    OutPos += sizeof(hfdNumHFInfos_bd930);
                    
                    SAPDB_MemCopyNoCheck (Buffer + OutPos - 1, &hfdMaxNumHFInfos_bd930, sizeof(hfdMaxNumHFInfos_bd930));
                    OutPos += sizeof(hfdMaxNumHFInfos_bd930);
                    
                    SAPDB_MemCopyNoCheck (Buffer + OutPos - 1, &isInitialized, sizeof(isInitialized));
                    OutPos += sizeof(bool);
        
                    if ((hfdNumHFInfos_bd930 - iHFI) < maxEntryOnPage )
                        maxEntryOnPage = ( hfdNumHFInfos_bd930 - iHFI);
                    SAPDB_MemCopyNoCheck (Buffer + OutPos - 1, &maxEntryOnPage, sizeof(maxEntryOnPage));
                    OutPos += sizeof(maxEntryOnPage);
                }

                SAPDB_MemCopyNoCheck (Buffer + OutPos - 1, history.GetDumpInfo (HFInfo.hfi_bd930.s.m_HistoryFileNo),
                        sizeof(Log_History::ExtendedHistoryFileInfo));
                OutPos += sizeof(Log_History::ExtendedHistoryFileInfo);

                SAPDB_MemCopyNoCheck (Buffer + OutPos - 1, &HFInfo.hfi_bd930.s.m_CurrActivGC, sizeof(HFInfo.hfi_bd930.s.m_CurrActivGC));
                OutPos += sizeof(HFInfo.hfi_bd930.s.m_CurrActivGC);

                SAPDB_MemCopyNoCheck (Buffer + OutPos - 1, &HFInfo.hfi_bd930.s.m_Filler1, sizeof(HFInfo.hfi_bd930.s.m_Filler1));
                OutPos += sizeof(HFInfo.hfi_bd930.s.m_Filler1);
            }
        }
        else
        {
            /* +++ PTS 1122219 2003-05-21 FF +++ */
            cbd930_HistoryFileInfo *HFInfoDummy = NULL;
            tsp00_Int4 maxEntryOnPage = (BufferSize - OutPos + sizeof(maxEntryOnPage)) /
                (sizeof(HFInfoDummy->hfi_bd930.s.m_CurrActivGC)   +
                sizeof(HFInfoDummy->hfi_bd930.s.m_Filler1));
            
            if ( hfdNumHFInfos_bd930 < maxEntryOnPage )
                maxEntryOnPage = hfdNumHFInfos_bd930;
            SAPDB_MemCopyNoCheck (Buffer + OutPos - 1, &maxEntryOnPage, sizeof(maxEntryOnPage));
            OutPos += sizeof(maxEntryOnPage);
            /* --- PTS 1122219 2003-05-21 FF --- */

            for (int iHFI=0; iHFI<hfdNumHFInfos_bd930; ++iHFI)
            {
                cbd930_HistoryFileInfo &HFInfo = *(hfdFirstHFInfoPtr_bd930 + iHFI);

                /* PTS 1122219 2003-05-21 FF */
                if ((tsp00_Uint4)BufferSize < 
                    (OutPos + sizeof(HFInfo.hfi_bd930.s.m_CurrActivGC) +
                    sizeof(HFInfo.hfi_bd930.s.m_Filler1)))
                {
                    g01new_dump_page (HostFile, Buffer, OutPno, OutPos, HostError, ErrText);
                    
                    if (vf_ok != HostError)
                        return;

                    SAPDB_RangeMove( __FILE__, 5,    
                        DUMPMARKSIZE_BD900, BufferSize, MarkVars, 1,
                        Buffer, OutPos, DUMPMARKSIZE_BD900 , MoveError);
                    MoveError = e_ok; // ignore error 
                    OutPos += DUMPMARKSIZE_BD900;
                    
                    SAPDB_MemCopyNoCheck (Buffer + OutPos - 1, &CodeVars, sizeof(tsp00_Int2));
                    OutPos += sizeof(tsp00_Int2);
                    
                    SAPDB_MemCopyNoCheck (Buffer + OutPos - 1, &hfdNumHFInfos_bd930, sizeof(hfdNumHFInfos_bd930));
                    OutPos += sizeof(hfdNumHFInfos_bd930);
                    
                    SAPDB_MemCopyNoCheck (Buffer + OutPos - 1, &hfdMaxNumHFInfos_bd930, sizeof(hfdMaxNumHFInfos_bd930));
                    OutPos += sizeof(hfdMaxNumHFInfos_bd930);
                    
                    SAPDB_MemCopyNoCheck (Buffer + OutPos - 1, &isInitialized, sizeof(isInitialized));
                    OutPos += sizeof(bool);
        
                    if ((hfdNumHFInfos_bd930 - iHFI) < maxEntryOnPage )
                        maxEntryOnPage = ( hfdNumHFInfos_bd930 - iHFI);
                    SAPDB_MemCopyNoCheck (Buffer + OutPos - 1, &maxEntryOnPage, sizeof(maxEntryOnPage));
                    OutPos += sizeof(maxEntryOnPage);
                }

                SAPDB_MemCopyNoCheck (Buffer + OutPos - 1, &HFInfo.hfi_bd930.s.m_CurrActivGC, sizeof(HFInfo.hfi_bd930.s.m_CurrActivGC));
                OutPos += sizeof(HFInfo.hfi_bd930.s.m_CurrActivGC);

                SAPDB_MemCopyNoCheck (Buffer + OutPos - 1, &HFInfo.hfi_bd930.s.m_Filler1, sizeof(HFInfo.hfi_bd930.s.m_Filler1));
                OutPos += sizeof(HFInfo.hfi_bd930.s.m_Filler1);
            }
        }
    }
}

/*===========================================================================*
 *  LOCAL FUNCTIONS (CODE)                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
