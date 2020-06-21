/*!
  @defgroup       GarbageCollection Remove history records not used anymore
*/
/*!
  @ingroup        GarbageCollection
  @file           gbd930.h
  @author         FerdiF
  @brief          Remove history records not used anymore
*/
/*
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


*/
#ifndef GBD930_H
#define GBD930_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include <new.h>

#ifndef GSP03_H
#   include "gsp03.h"
#endif
#ifndef GSP03_3_H
#   include "gsp03_3.h"
#endif
#ifndef GGG12_H
#   include "ggg12.h"
#endif
#ifndef HGG01_H
#   include "hgg01.h" //g01abort,g01opmsg 
#endif
#ifndef HGG01_1_H
#   include "hgg01_1.h" //g01abort,g01opmsg 
#endif
#ifndef HGG01_3_H
#   include "hgg01_3.h"   //get cserv parameter g01maxuser
#endif
#ifndef HGG08_H
#   include "hgg08.h"   //g08garbage
#endif
#ifndef HEO55K_H
#   include "heo55k.h"  //vbegexcl
#endif
#ifndef HEO56_H
#   include "heo56.h"   //vsuspend,vresume
#endif
#ifndef GBD900_H
#   include "gbd900.h"
#endif
#ifndef LVC_WAITFORDELOBJFILEQUEUE_HPP
#   include "liveCache/LVC_WaitForDelObjFileQueue.hpp"            // PTS 1112228 AK 19/10/2001 
#endif
#ifndef LOG_HISTORY_HPP
#   include "Logging/Log_History.hpp"
#endif
#ifndef RTEMEM_ALLOCATOR_HPP
#   include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"        //PTS 1110854 TS 2001-07-09
#endif
#ifndef SAPDBMEM_NEWDESTROY_HPP
#   include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp" //PTS 1110854 TS 2001-07-09
#endif
#ifndef GC_IGARBAGECOLLECTOR_HPP
#   include "GarbageCollector/GC_IGarbageCollector.hpp" // PTS xxxxxxx FF 2002-02-12
#endif
#ifndef GC_IGCCONTROLLER_HPP
#   include "GarbageCollector/GC_IGCController.hpp" 
#endif
#ifndef TRACE_ENTRY_HPP
#   include "Trace/Trace_Entry.hpp"
#endif
#ifndef MSG_LIST_HPP
#   include "Messages/Msg_List.hpp"
#endif
#ifndef RTE_CRASH_HPP
#   include "RunTime/RTE_Crash.hpp"
#endif
#ifndef FILEDIR_OMS_HPP
#   include "FileDirectory/FileDir_Oms.hpp"
#endif
#include "ServerTasks/SrvTasks_ICoordinator.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define NIL_HISTORY_FILE_INDEX_BD930  -1

/* local defines uses in this file only */
#define GC_IS_STARTED                 true
#define GC_IS_STOPPED                 false

#if COMPILEMODE_MEO00 >= QUICK_MEO00 
#  define BD930_CHECK_GC  bd930_CheckGC()
#else 
#  define BD930_CHECK_GC
#endif

/* Timeinterval which is used to control starting GCs via Timeout process   */ 
#define TIMEOUT_CHECK_CANCELLED_RED      3
#define TIMEOUT_CHECK_CANCELLED          60

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  FORWARD DECLARATIONS  OF CLASSES, STRUCTURES, TYPES, UNIONS              *
 *===========================================================================*/

class cbd930_GarbageCollector;
class cbd930_GCState;
class cbd930_GCCounters;
class cbd930_GarbCollController;
class cbd930_HistoryFileInfo;
class cbd930_HistoryFileDir;
class cbd930_HistoryFileIter;

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *==========================================================f=================*/

typedef union 
{
    SAPDB_Int8           m_int8;
    struct cbd930_HistFileSync
    {
        SAPDB_UInt4      m_HistoryFileNo;
        tsp00_Int2       m_CurrActivGC;
        tsp00_Bool       m_Filler1;
        SAPDB_UInt1      m_GcStartCycle;
    } s;
} cbd930_HistFileEntry;

class cbd930_HistoryFileInfo
{
public: //members
    cbd930_HistFileEntry   hfi_bd930;

public: //methods
              cbd930_HistoryFileInfo   ();
         bool bd930ClearHistoryFile    (tgg00_TransContext        &Trans,
                                        GC_IGarbageCollector      &IGarbColl,
                                        Msg_List                  &errlist);
private:
    /* this check is necessary for vallocat */
    CHECK_MAPPABLE_CLASS_GIVEN_SIZE_SP00 (cbd930_HistoryFileInfo,  8)
};

/*---------------------------------------------------------------------------*/

class cbd930_HistoryFileDir
{
    friend class cbd930_HistoryFileIter;

public: // methods
              cbd930_HistoryFileDir   ();
  inline void bd930AddHistoryFile     (tgg00_TransContext &Trans, 
                                       SAPDB_UInt4         HistoryFileNo);
  inline void bd930ClearAll           ();
         void bd930DumpHistoryFileDir (tgg00_VfFileref     &HostFile,
                                       tsp00_Int4            BufferSize,
                                       tsp00_Page           &Buffer,
                                       tsp00_Int4           &OutPno,
                                       tsp00_Int4           &OutPos,
                                       tsp00_VfReturn       &HostError,
                                       tsp00_ErrText        &ErrText);
  inline void bd930Init               ();
private: // members
  cbd930_HistoryFileInfo *hfdFirstHFInfoPtr_bd930;    // pointer to the first entry in the directory
  tsp00_Int4              hfdNumHFInfos_bd930;        // number of HistoryFileInfo in the directory
  tsp00_Int4              hfdMaxNumHFInfos_bd930;     // maximum number of HistoryFileInfo which can be stored  
};

/*---------------------------------------------------------------------------*/

class cbd930_HistoryFileIter
{
public: // methods
  inline cbd930_HistoryFileInfo* operator () ();
  inline cbd930_HistoryFileInfo* operator ++ ();
  inline cbd930_HistoryFileInfo* bd930First  ();
         cbd930_HistoryFileInfo* bd930SetToHistFileWithOldestTrans(tgg00_TransContext &Trans);
         cbd930_HistoryFileIter (cbd930_HistoryFileDir & HistoryFileDir);

private: // members
  cbd930_HistoryFileDir  &hitHFDir_bd930;
  cbd930_HistoryFileInfo *hitCurrHFInfoPtr_bd930;
  int                     hitCurrHFIndex_bd930;
};

/*-----------------------------------------------------------*/

class cbd930_GarbageCollector : public GC_IGarbageCollector
{
public:
  /* methods defined by GC_IGarbageCollector interface */
  virtual void        IncHistCreateObjFileCount();
  virtual void        IncHistDeleteObjCount();
  virtual void        IncHistDropObjFileCount();
  virtual void        IncHistEntryReleaseCount();
  virtual void        IncHistInsertObjCount();
  virtual void        IncHistLockObjCount();
  virtual void        IncHistNewObjCount();
  virtual void        IncHistUpdateObjCount();
  virtual void        IncObjReleaseCount();
  virtual void        IncReleaseEmptyPageCount();
  virtual bool        IsInterrrupted() const;

public:
  cbd930_GarbageCollector (tgg00_TransContext          &Trans,
                           cbd930_GarbCollController   &GarbCollController);
  
private: // members
    tgg00_TransContext         &gcoTrans_bd930;
    const int                   gcoIndex_bd930;
    cbd930_GarbCollController  &gcoController_bd930;
    tgg00_TransContext          gcoAuxTrans_bd930;
    bool                        gcoInSession_bd930;

private: // methods
  inline void bd930_HistoryErrorHandler              ();
  inline void bd930_WriteMessageIntoVtrace     (const bool bStartStopFlag);
         void bd930_WritePageStatusIntoVtrace        (tgg00_TransContext    &Trans,
                                                      tbd_nodeptr            pPage,
                                                      tgg00_VtraceType_Enum  TraceObject);
         bool bd930_TryToReleaseUnusedObjDataPages   (tgg00_TransContext  &Trans);
};

/*---------------------------------------------------------------------------*/

class cbd930_GCCounters
{
public:
  inline cbd930_GCCounters();
public:
  tsp00_8ByteCounter      gccntActivateCount;     
  tsp00_8ByteCounter      gccntObjectsDeleted;     
  tsp00_8ByteCounter      gccntHistEntriesReleased;
  tsp00_8ByteCounter      gccntEmtyPagesReleased;            // pages released because of empty-page handling
  tsp00_8ByteCounter      gccntDropPagesReleased;            // pages released because of drop page handling
  tsp00_8ByteCounter      gccntDropFiles;                    // number of files dropped 
  tsp00_8ByteCounter      gccntHistCreateObjFile;
  tsp00_8ByteCounter      gccntHistDeleteObj;
  tsp00_8ByteCounter      gccntHistDropObjFile;
  tsp00_8ByteCounter      gccntHistLockObj;
  tsp00_8ByteCounter      gccntHistInsertObj;
  tsp00_8ByteCounter      gccntHistNewObj;
  tsp00_8ByteCounter      gccntHistUpdateObj;
};


/*---------------------------------------------------------------------------*/

class cbd930_GCState
{
public:
  inline cbd930_GCState();
public:
  tsp00_TaskId gcsTaskId_bd930;
  tsp00_Bool   gcsActive_bd930;
  SAPDB_UInt1  gcsStartCycle_bd930;     // used to minimize calls to bd930ClearHistoryFile
};

/*---------------------------------------------------------------------------*/

class  cbd930_GarbCollController : public GC_IGCController
{
  friend class cbd930_GarbageCollector;
  
private://members
  SAPDB_UInt8                  gccLastTimeGCsStarted;
  SAPDB_UInt8                  gccLastTimeCancelCheck;
  SAPDB_Int4                   gccStartingGCs;                    // used to avoid collisions on GC Region              
  tsp00_Int4                   gccNumGC_bd930;                    // # of all garbage collectors (GC) 
  tsp00_Int4                   gccNumGCUsed_bd930;                // # of used GCs  
  tgg91_TransNo                gccLastWriteTransNo;               // active WriteTrans when tested last time   
  tsp00_TaskId                 gccStopGCManagerTID_bd930;         // Id of the task coordinating the suspension of all GCs
  cbd930_HistoryFileDir        gccHistoryFileDir_bd930;
  tsp00_Bool                   gccStopGC_bd930;                   // flag that displays whether all GCs have to stoped
  tsp00_Bool                   gccSpecialOperationIsActive_bd930; // savepoint/b01restart_filesystem or     PTS 1113552 FF
  tsp00_Bool                   gccGCStartRequested_bd930;         // flag: all GCs have to be started after Timeintervall PTS 1116157 FF                                                                        
  // "Cold Verify" or "DIAGNOSE REPAIR" is active 
  tsp00_Bool                   gccCheckSavepointNeeded_bd930;       
  tsp00_Bool                   gccRestartOfAllGCsInAction;
  SAPDB_UInt1				   gccStartCycle_bd930;				  // 
  tsp00_Bool				   gccHistoryRemovalEnabled;	      // enable history removal during restart
  tsp00_Bool                   gccTryingToStartGc_bd930;          // flag: all GCs have to be started after Timeintervall PTS 1116157 FF                                                                        

  LVC_WaitForDelObjFileQueue  *m_pWaitForDelObjFileQueue;         // PTS 1112228 AK 19/10/2001 7.4.01 -> 7.4.02 Merge FF 29/11/2001
public:
 
  ARRAY_VAR_MSP00 (gccGCState_bd930, cbd930_GCState,   MAX_OBJ_GARB_COLL_BD00);
  ARRAY_VAR_MSP00 (gccGCCounters_bd930, cbd930_GCCounters,   MAX_OBJ_GARB_COLL_BD00);
  
public://methods
  virtual void bd930CheckRetentionTime         ( tsp00_TaskId       TaskId);
  virtual void TriggerGarbageCollection        ( tsp00_TaskId       TaskId,
                                                 bool               withHighPriority );
  virtual void RegisterHistoryFile             (tgg00_TransContext &Trans, 
                                                SAPDB_UInt4         HistoryFileNo);
  virtual void ClearAll                        (); 
  virtual SAPDB_UInt8 GetGCLastTimeStarted();

  inline bool bd930CheckIfTaskIsAGC            (const tsp00_TaskId        TaskId) const;
         void bd930DumpGCController            (tgg00_VfFileref      &HostFile,
                                                tsp00_Int4            BufferSize,
                                                tsp00_Page           &Buffer,
                                                tsp00_Int4           &OutPno,
                                                tsp00_Int4           &OutPos,
                                                tsp00_VfReturn       &HostError,
                                                tsp00_ErrText        &ErrText);
  inline tsp00_Int4 bd930GetNumGCs             () const;
  inline void bd930Init                        (
                                                );
  void bd930StartAllGCs                        (tsp00_TaskId   TaskId,
                                                SAPDB_UInt8    actTime = 0,
                                                bool           startall = true);
  inline void bd930EnableOMSHistoryRemoval     (tsp00_TaskId   TaskId);
  inline void bd930StartAllGCsAfterSpecialOp   (tsp00_TaskId   TaskId);
         void bd930StopAllGCsForSpecialOp      (tsp00_TaskId   TaskId);
         void bd930StartGCsIfRequired          (tsp00_TaskId   TaskId);
  inline void InsertIntoWaitForDelObjFileQueue (tsp00_TaskId    TaskId,     // PTS 1112228 AK 19/10/2001 
                                                FileDir_FileNo &ObjFileNo);
  inline void WakeUpTasksWaitingForDelObjFile  (tsp00_TaskId    TaskId,     // PTS 1112228 AK 19/10/2001 
                                                FileDir_FileNo &ObjFileNo);
  inline tsp00_Bool GetFirstWaitForDelEntry    (tsp00_TaskId    &TaskId,     // PTS 1112228 AK 19/10/2001 
                                                FileDir_FileNo &ObjFileNo);
  inline void TerminateGarbagecollector        (const int        gcIndex,
                                                tsp00_TaskId    &ResumeTaskId);
  inline bool GCMustStop();

private : // methods used by the friend class cbd930_GarbageCollector
  inline tsp00_Int4 bd930_AddGC        (tgg00_TransContext &Trans);
  inline void       bd930_CheckGC      ();   
  inline void       bd930_FindUnusedGC (tsp00_TaskId       &UnusedGCTaskId);
};

/*---------------------------------------------------------------------------*/
/* METHOD IMPLEMENTATION FOR cbd930_GarbCollController                       */
/*---------------------------------------------------------------------------*/

inline bool
cbd930_GarbCollController::bd930CheckIfTaskIsAGC (const tsp00_TaskId  TaskId) const
{
  /* check if the given task id belongs to a Garabage Collector */
  for  (int iGC=0; iGC< gccNumGC_bd930; ++iGC)
  {
    if(gccGCState_bd930 [iGC].gcsTaskId_bd930 == TaskId)
      return true;
  }
  return false;
}
  
/*---------------------------------------------------------------------------*/

inline tsp00_Int4 
cbd930_GarbCollController::bd930GetNumGCs() const
{
  return gccNumGC_bd930;
};

/*---------------------------------------------------------------------------*/

inline void
cbd930_GarbCollController::bd930Init ()
{
  gccLastTimeGCsStarted            = 0;
  gccLastTimeCancelCheck           = 0;
  gccStartingGCs                   = 0;
  gccNumGC_bd930                   = 0;
  gccNumGC_bd930                   = 0;
  gccNumGCUsed_bd930               = 0;                    
  gccLastWriteTransNo.gg90SetNil();             
  gccStopGCManagerTID_bd930        = cgg_nil_pid;
  gccStopGC_bd930                  = false;
  gccSpecialOperationIsActive_bd930 = true; // PTS 1113552 FF  
  gccGCStartRequested_bd930        = true;  // PTS 1116157 FF
  gccTryingToStartGc_bd930         = false;
  gccCheckSavepointNeeded_bd930    = false;

  SAPDB_UInt maxserver = SrvTasks_ICoordinator::Instance().GetMaxServerTasks();
  // PTS 1112228 AK 19/10/2001 FF 7.4.01 -> 7.4.02 Merge 29/11/2001
  SAPDBMem_IRawAllocator &Allocator = RTEMem_Allocator::Instance();
  m_pWaitForDelObjFileQueue = new (Allocator) LVC_WaitForDelObjFileQueue (Allocator, maxserver);
  if (!m_pWaitForDelObjFileQueue)
     g01abort (csp3_bd_msg, csp3_n_obj, "bd930Init:WaitDelObjFilQ", -9999);
  
  gccStartCycle_bd930		       = 0;
  gccRestartOfAllGCsInAction       = false;
  gccHistoryRemovalEnabled         = false;
  gccHistoryFileDir_bd930.bd930Init();
}
/*---------------------------------------------------------------------------*/

inline tsp00_Int4
cbd930_GarbCollController::bd930_AddGC(tgg00_TransContext   &Trans)
{
    vbegexcl (Trans.trTaskId_gg00, g08garbage); 

    /* update number of registerd GCs */
    const int IndexGC = gccNumGC_bd930++;

    /* check Number of  garbage collectores */
    if (gccNumGC_bd930 > MAX_OBJ_GARB_COLL_BD00)
        g01abort (csp3_bd_msg, csp3_n_obj, "BD930RegGC:No more slots", IndexGC);

    /* check that this slot is really free */
    if (cgg_nil_pid != gccGCState_bd930[IndexGC].gcsTaskId_bd930)
        g01abort (csp3_bd_msg, csp3_n_obj, "BD930RegGC:Slot not free", IndexGC);

    /* assign TaskIds */ 
    gccGCState_bd930[IndexGC].gcsTaskId_bd930 = Trans.trTaskId_gg00;

    vendexcl (Trans.trTaskId_gg00, g08garbage); 

    return IndexGC;   
}

/*-----------------------------------------------------------*/

inline void
cbd930_GarbCollController::bd930_FindUnusedGC (tsp00_TaskId  &UnusedGCTaskId)      
{
  
  ROUTINE_DBG_MEO00 ("bd930_FindUnusedGC");
  
  /* This function looks for an unused GC  */
  /* If one found it is marked and         */
  /* bUnusedGCExists ist set to true       */
  
  /* check if one is exclusively in the g08garbage region */
# if COMPILEMODE_MEO00 >= SLOW_MEO00 
  g08check_excl (g08garbage);
# endif
  
  UnusedGCTaskId = cgg_nil_pid;
  
  /* decide if there is still an unused GC which can be started  */
  if (gccNumGCUsed_bd930 < gccNumGC_bd930 && ! gccSpecialOperationIsActive_bd930) // PTS 1113552 FF
  {               
    /* find free GC */
    for  (int iGC=0; iGC< gccNumGC_bd930; ++iGC)
    {
      if(!gccGCState_bd930 [iGC].gcsActive_bd930)
      {
        gccGCState_bd930 [iGC].gcsActive_bd930 = true;
        UnusedGCTaskId = gccGCState_bd930 [iGC].gcsTaskId_bd930;
        ++gccNumGCUsed_bd930;
        break;
      }
    }
  }
}

/*---------------------------------------------------------------------------*/
inline void 
cbd930_GarbCollController::bd930EnableOMSHistoryRemoval     (tsp00_TaskId   TaskId)
{
  gccHistoryRemovalEnabled = true;
};

/*---------------------------------------------------------------------------*/

inline void
cbd930_GarbCollController::bd930StartAllGCsAfterSpecialOp (tsp00_TaskId   TaskId)
{ 
    gccRestartOfAllGCsInAction        = true;      // avoid dropping consistent view
    gccSpecialOperationIsActive_bd930 = false;     // PTS 1113552 FF
    gccGCStartRequested_bd930         = true;
    gccTryingToStartGc_bd930          = false;
    bd930StartAllGCs (TaskId);
    
    g01opmsg (sp3p_knldiag, sp3m_info, csp3_bd_msg, csp3_n_obj,
                  "Restarted Garbage coll  ", gccNumGCUsed_bd930);

    gccRestartOfAllGCsInAction = false;
}

/*---------------------------------------------------------------------------*/
// PTS 1112228 AK 19/10/2001 FF 7.4.01 -> 7.4.02 Merge 29/11/2001
inline void
cbd930_GarbCollController::InsertIntoWaitForDelObjFileQueue (tsp00_TaskId    TaskId,
                                                             FileDir_FileNo &ObjFileNo)
{
  m_pWaitForDelObjFileQueue->Insert (TaskId, ObjFileNo);
}

/*---------------------------------------------------------------------------*/
// PTS 1112228 AK 19/10/2001 FF 7.4.01 -> 7.4.02 Merge 29/11/2001
inline void
cbd930_GarbCollController::WakeUpTasksWaitingForDelObjFile (tsp00_TaskId    TaskId,
                                                            FileDir_FileNo &ObjFileNo)
{
  m_pWaitForDelObjFileQueue->WakeUpWaitingTasks (TaskId, ObjFileNo);
}
    
/*---------------------------------------------------------------------------*/
inline tsp00_Bool
cbd930_GarbCollController::GetFirstWaitForDelEntry (tsp00_TaskId    &TaskId,
                                                    FileDir_FileNo &ObjFileNo)
{
    return m_pWaitForDelObjFileQueue->GetFirstWaitForDelEntry (TaskId, ObjFileNo);
}

/*---------------------------------------------------------------------------*/
inline void 
cbd930_GarbCollController::TerminateGarbagecollector (const int     gcIndex,
                                                      tsp00_TaskId &ResumeTaskId)
{
    gccGCState_bd930[gcIndex].gcsActive_bd930 = false;
    gccNumGCUsed_bd930--;

    if (gccStopGC_bd930 && (0 == gccNumGCUsed_bd930))
    {
        gccGCStartRequested_bd930 = true;          
        gccStopGC_bd930 = false;
        ResumeTaskId = gccStopGCManagerTID_bd930;
        gccStopGCManagerTID_bd930 = cgg_nil_pid;
    }
};

/*---------------------------------------------------------------------------*/
inline bool 
cbd930_GarbCollController::GCMustStop()
{
    return gccStopGC_bd930;
};

/*---------------------------------------------------------------------------*/
/* METHOD IMPLEMENTATION FOR cbd930_GarbageCollector                         */
/*---------------------------------------------------------------------------*/

inline void
cbd930_GarbageCollector::bd930_WriteMessageIntoVtrace (const bool bStart)
{
    if (g01vtrace.vtrOmsFree_gg00 )
    {
        tgg12_OmsGarbCollTrace TrLineGC;
        
        TrLineGC.ogrbTrType_gg12.becomes(otGarbColl_egg12);
        TrLineGC.ogrbTrType2_gg12.becomes(ot2Nil_egg12);
        TrLineGC.ogrbCollectorIndex_gg12 = gcoIndex_bd930;
        TrLineGC.ogrbIsActiv_gg12        = bStart;
        
        Trace_CommonEntry entry( tgg00_Debug::fromConst(bd), 
            tgg00_VtraceType::fromConst(b930Garbcol), 
            REINTERPRET_CAST( tsp00_BytePtr, &TrLineGC ), sizeof (TrLineGC) );
    }
}

/*---------------------------------------------------------------------------*/

inline void 
cbd930_GarbageCollector::bd930_HistoryErrorHandler()
{
  if (e_ok != gcoTrans_bd930.trError_gg00)
  {
    g01opmsg (sp3p_knldiag, sp3m_error, csp3_bd_msg, csp3_n_obj, 
      "BD930HistGC: Error      ", gcoTrans_bd930.trError_gg00);
    g01abort (csp3_bd_msg, csp3_n_obj,"BD930HistGC: Index      ", gcoIndex_bd930);
  }
}

/*---------------------------------------------------------------------------*/
/* METHOD IMPLEMENTATION FOR cbd930_GCCounters                               */
/*---------------------------------------------------------------------------*/

inline 
cbd930_GCCounters::cbd930_GCCounters()
:
gccntActivateCount (0),
gccntObjectsDeleted (0),
gccntHistEntriesReleased (0),
gccntEmtyPagesReleased (0),
gccntDropPagesReleased (0),
gccntDropFiles (0),
gccntHistCreateObjFile (0),
gccntHistDeleteObj (0),
gccntHistDropObjFile (0),
gccntHistLockObj (0),
gccntHistInsertObj (0),
gccntHistNewObj (0),
gccntHistUpdateObj (0)
{};

/*---------------------------------------------------------------------------*/
/* METHOD IMPLEMENTATION FOR cbd930_GCState                                  */
/*---------------------------------------------------------------------------*/

inline 
cbd930_GCState::cbd930_GCState()
:
gcsTaskId_bd930 (cgg_nil_pid),
gcsActive_bd930 (false),
gcsStartCycle_bd930( 0 )
{};

/*---------------------------------------------------------------------------*/
/* METHOD IMPLEMENTATION FOR cbd930_HistoryFileInfo                          */
/*---------------------------------------------------------------------------*/

inline
cbd930_HistoryFileInfo::cbd930_HistoryFileInfo()
{
    hfi_bd930.s.m_HistoryFileNo = SAPDB_MAX_UINT4;
    hfi_bd930.s.m_CurrActivGC   = NIL_GC_INDEX_BD900;
    hfi_bd930.s.m_Filler1       = false;
    hfi_bd930.s.m_GcStartCycle  = 0;
}

/*---------------------------------------------------------------------------*/
/* METHOD IMPLEMENTATION FOR cbd930_HistoryFileDir                           */
/*---------------------------------------------------------------------------*/

inline void
cbd930_HistoryFileDir::bd930Init()
{
    hfdNumHFInfos_bd930    = 0;
    hfdMaxNumHFInfos_bd930 = g01maxuser();
    
    /* write note about allocation into knldiag */
    g01allocate_msg (csp3_n_dynpool, "HIST_DIR  item size    :", sizeof (cbd930_HistoryFileInfo));
    g01allocate_msg (csp3_n_dynpool, "HIST_DIR  num items    :", hfdMaxNumHFInfos_bd930);
    g01allocate_msg (csp3_n_dynpool, "DYNP_B930_HIST_DIR     :", hfdMaxNumHFInfos_bd930 * sizeof (cbd930_HistoryFileInfo));
    
    // PTS 1110854 TS 2001-07-09 
    /* allocate memory for the history file entries */
    //vallocat (hfdMaxNumHFInfos_bd930 * sizeof (cbd930_HistoryFileInfo), (void **) &hfdFirstHFInfoPtr_bd930, &bOk);
    
    newarray (hfdFirstHFInfoPtr_bd930, hfdMaxNumHFInfos_bd930, RTEMem_Allocator::Instance());
    
    if ( NULL == hfdFirstHFInfoPtr_bd930 ) 
      g01abort (csp3_sysbuf_storage_exceeded, csp3_n_config,"DYNP_B930_HIST_DIR      ", 0);
    
    /* initialize allocated memory */
    /*
    if(bOk)
    
      new (hfdFirstHFInfoPtr_bd930) cbd930_HistoryFileInfo [hfdMaxNumHFInfos_bd930]; 
      else
      g01abort (csp3_sysbuf_storage_exceeded, csp3_n_config,"DYNP_B930_HIST_DIR      ", 0);
    */
}

/*---------------------------------------------------------------------------*/
/* PTS 1111248 AK 3/08/20001                                                 */
inline void
cbd930_HistoryFileDir::bd930AddHistoryFile (tgg00_TransContext &Trans, 
                                            SAPDB_UInt4         HistoryFileNo)
{
  if (hfdNumHFInfos_bd930 < hfdMaxNumHFInfos_bd930)
  {
    (hfdFirstHFInfoPtr_bd930 + hfdNumHFInfos_bd930)->
        hfi_bd930.s.m_HistoryFileNo = HistoryFileNo;
    hfdNumHFInfos_bd930 ++;
  }
  else
    g01abort (csp3_bd_msg, csp3_n_obj,"BD930:Too Many HistFiles", hfdNumHFInfos_bd930);
  
}

/*---------------------------------------------------------------------------*/

inline void
cbd930_HistoryFileDir::bd930ClearAll ()
{
  hfdNumHFInfos_bd930 = 0;
}

/*---------------------------------------------------------------------------*/

inline
cbd930_HistoryFileDir::cbd930_HistoryFileDir()
:
hfdFirstHFInfoPtr_bd930 (NULL),
hfdNumHFInfos_bd930     (0),
hfdMaxNumHFInfos_bd930  (0)
{};

/*---------------------------------------------------------------------------*/
/* METHOD IMPLEMENTATION FOR cbd930_HistoryFileIter                          */
/*---------------------------------------------------------------------------*/

inline 
cbd930_HistoryFileIter::cbd930_HistoryFileIter (cbd930_HistoryFileDir & HistoryFileDir)
:
hitHFDir_bd930         (HistoryFileDir),
hitCurrHFInfoPtr_bd930 (NULL),
hitCurrHFIndex_bd930   (NIL_HISTORY_FILE_INDEX_BD930)
{};

/*---------------------------------------------------------------------------*/

inline cbd930_HistoryFileInfo*
cbd930_HistoryFileIter::operator () ()
{
  return hitCurrHFInfoPtr_bd930;
}

/*---------------------------------------------------------------------------*/

inline cbd930_HistoryFileInfo*
cbd930_HistoryFileIter::operator ++ ()
{
  hitCurrHFIndex_bd930 ++;
  
  if (hitCurrHFIndex_bd930 < hitHFDir_bd930.hfdNumHFInfos_bd930)
    hitCurrHFInfoPtr_bd930 = hitHFDir_bd930.hfdFirstHFInfoPtr_bd930 + hitCurrHFIndex_bd930;
  else
    hitCurrHFInfoPtr_bd930 = NULL;
  
  return hitCurrHFInfoPtr_bd930;
}

/*---------------------------------------------------------------------------*/

inline cbd930_HistoryFileInfo*
cbd930_HistoryFileIter::bd930First()
{
  if (hitHFDir_bd930.hfdNumHFInfos_bd930 > 0)
  {
    hitCurrHFIndex_bd930   = 0;
    hitCurrHFInfoPtr_bd930 = hitHFDir_bd930.hfdFirstHFInfoPtr_bd930;
  }
  else
  {
    hitCurrHFIndex_bd930   = NIL_HISTORY_FILE_INDEX_BD930;
    hitCurrHFInfoPtr_bd930 = NULL;
  }
  
  return hitCurrHFInfoPtr_bd930;
}

/*---------------------------------------------------------------------------*/

#endif  /* GBD930_H */
