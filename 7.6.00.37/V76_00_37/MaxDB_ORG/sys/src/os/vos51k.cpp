/*!
  @file           vos51k.cpp
  @author         RaymondR
  @brief          initializations
  @see

\if EMIT_LICENCE
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




/*
 * INCLUDE FILES
 */
#include "gos00.h"
#include "heo51.h"
#include "gos41.h"
#include "heo00.h"
#include "heo46.h"
#include "geo007_1.h"
#include "gos00k.h"
#include "geo002.h"
#include "heo79.h"
#include "gos003.h"
#include "heo56.h"
#include "heo69.h"
#include "heo92.h"
#include "heo15.h"
#include "RunTime/RTE_KSS.h"
#include "RunTime/RTE_KernelMain.h"
#include "RunTime/RTE_SpawnRestartShutdown.h"
#include "RunTime/RTE_DiagPipe.h"
#include "RunTime/Tasking/RTETask_Task.hpp"
#include "RunTime/RTE_Version.h"
#include "RunTime/RTE_Message.hpp"

/*
 *  DEFINES
 */
#define MOD__  "VOS51KC : "
#define MF__   MOD__"UNDEFINED"


// --- VSHUTDOWN --------------------------------------------------------------

typedef enum1                   SHUTDOWN_MODE;

#define SHUTDOWN_KILL           0
#define SHUTDOWN_NORMAL         1

/*
 *  MACROS
 */


/*
 *  LOCAL TYPE AND STRUCT DEFINITIONS
 */


/*
 * EXTERNAL VARIABLES
 */


/*
 *  EXPORTED VARIABLES
 */


/*
 * LOCAL VARIABLES
 */


/*
 * LOCAL FUNCTION PROTOTYPES
 */
static VOID sql51k_start_multiple_tasks ( PTASK_CTRL_REC pFirstMultipleTask,
                                          ULONG          ulNumTasks );

/*
 * ========================== GLOBAL FUNCTIONS ================================
 */

VOID vinitok ( VOID )
  {
  #undef  MF__
  #define MF__ MOD__"vinitok"
  PUKT_CTRL_REC                  pTmpUKTCtrl;
  SAPDBErr_MessageList           messageList;
  #ifdef DEBUG_RTE
   PTASK_CTRL_REC                pTaskCtrl = THIS_UKT_CTRL->pCTask;
  #endif

  DBGIN_T (pTaskCtrl->ulTaskIndex);


  if ( THIS_UKT_CTRL->pCTask->TaskType != TT_TI_EO00 )
    {
    MSGD (( ERR_VINITOK_NOT_TIMEOUT ));
    ABORT();
    }

  MSGD (( INFO_VINITOK ));
  //
  //
  //  - send startup requests to all non user and server tasks
  //
  //
  if ( false == kgs.pAL->pTaskObject->Startup( messageList ))
      RTE_Crash (messageList);

  if ( false == kgs.pTW->pTaskObject->Startup( messageList ))
      RTE_Crash (messageList);

  sql51k_start_multiple_tasks ( kgs.pFirstDWTaskCtrl    , XPARAM(ulMaxDataWriter) ) ;
  sql51k_start_multiple_tasks ( kgs.pFirstBackUpTaskCtrl, XPARAM(ulMaxBackupTasks) ) ;
  sql51k_start_multiple_tasks ( kgs.pFirstGCTaskCtrl,     XPARAM(ulMaxGarbageCollector) ) ;


  if ( XPARAM(ulMaxServer) > 0 )
    {

    if ( XPARAM(fDynamicServerTasks) == FALSE )
      /*
       *  send startup requests to all  server tasks
       */
      sql51k_start_multiple_tasks ( kgs.pFirstServerTaskCtrl, XPARAM(ulMaxServer) ) ;

    }

  //
  //
  // - wake up all user kernel threads
  //
  //
  for ( pTmpUKTCtrl =  kgs.pFirstUKTCtrl;
        pTmpUKTCtrl <= kgs.pLastUKTCtrl;
        pTmpUKTCtrl++ )
    {
    if ( pTmpUKTCtrl->ThrdCtrlHeader.hevSem != (HEV) UNDEF )
      sql72k_wake_ukt( pTmpUKTCtrl );
    }


  DBGOUT_T (pTaskCtrl->ulTaskIndex);
  return;
  }

/*------------------------------*/

VOID vgetuktid ( tsp00_Int4 VAR_VALUE_REF id )
  {
  #undef  MF__
  #define MF__ MOD__"vgetuktid"

  id = (tsp00_Int4)THIS_UKT_CTRL->ulUKTIndex;
  }

/*------------------------------*/

VOID vgetpid ( tsp00_TaskId VAR_VALUE_REF    pid )
{
#undef  MF__
#define MF__ MOD__"vgetpid"

    PUKT_CTRL_REC                 pUKT      = THIS_UKT_CTRL;
    if ( !pUKT )
    {
        pid = 0; /* called from other type of thread */
    }
    else
    {
#ifdef DEBUG_RTE
        PTASK_CTRL_REC                pTaskCtrl = pUKT->pCTask;
#endif
        DBGPAS_T (pTaskCtrl->ulTaskIndex);

        DBG3 (( MF__, "[T:0x%03u] called ", pTaskCtrl->ulTaskIndex ))

        pid = pUKT->pCTask->ulTaskIndex;
    }
}

/*------------------------------*/

VOID vmovelock ( PROCESS_ID  pid )
  {
  #undef  MF__
  #define MF__ MOD__"vmovelock"

  DBGPAS_T (pTaskCtrl->ulTaskIndex);

  // set temporary move lock
  //
  #ifdef DEVELOP_CHECKS
   sql74k_temp_move_lock ( THIS_UKT_CTRL->pCTask, 1, true );
  #else
   sql74k_temp_move_lock ( &kgs.pFirstTaskCtrl[pid - 1], 1, true );
  #endif
  }

/*------------------------------*/

VOID vmoveunlock ( PROCESS_ID  pid )
  {
  #undef  MF__
  #define MF__ MOD__"vmovelock"

  DBGPAS_T (pTaskCtrl->ulTaskIndex);

  // remove temporary move lock
  #ifdef DEVELOP_CHECKS
   sql74k_temp_move_lock ( THIS_UKT_CTRL->pCTask, 1, false );
  #else
   sql74k_temp_move_lock ( &kgs.pFirstTaskCtrl[pid - 1], 1, false );
  #endif
  }

/*------------------------------*/

externPascal tsp00_Bool visutilitytaskconnected()
{
    return  ( kgs.pUT != 0
           && kgs.pUT->pUserCommCtrl != 0
           && 0 != kgs.pUT->pUserCommCtrl->ulConnected );
}

/*------------------------------*/

VOID vgetfirstuserpid ( tsp00_TaskId VAR_VALUE_REF   pid )
  {
  #undef  MF__
  #define MF__ MOD__"vgetfirstuserpid"
  #ifdef DEBUG_RTE
   PTASK_CTRL_REC                pTaskCtrl = THIS_UKT_CTRL->pCTask;
  #endif

  DBGPAS_T (pTaskCtrl->ulTaskIndex);

  DBG3 (( MF__, "[T:0x%03u] called ", pTaskCtrl->ulTaskIndex ))

  pid = kgs.pFirstUserTaskCtrl->ulTaskIndex;
  }

/*------------------------------*/

VOID vdebug_break ( INT4 i4DebugBreakPosition )
  {
  #undef  MF__
  #define MF__ MOD__"vdebug_break"

  if ( i4DebugBreakPosition < 10000 )
    {
    MSGALL (( ERR_WRONG_DEBUG_BREAK_POS, i4DebugBreakPosition, 10000 ));
    DBGOUT;
    return;
    }

  sql51k_debug_break ((ULONG)i4DebugBreakPosition );

  DBGOUT;
  return;
  }

/*------------------------------*/

VOID vptype ( tsp00_TaskId                     pid,
              tsp2_process_type VAR_VALUE_REF  proc_type )
  {
  #undef  MF__
  #define MF__ MOD__"vptype"
  #ifdef DEVELOP_CHECKS
   PTASK_CTRL_REC                 pTaskCtrl = THIS_UKT_CTRL->pCTask;
  #else
   PTASK_CTRL_REC                 pTaskCtrl = &kgs.pFirstTaskCtrl[pid - 1];
  #endif

  DBGIN_T (pTaskCtrl->ulTaskIndex);

  #ifdef DEVELOP_CHECKS
   if ( (ULONG)pid != pTaskCtrl->ulTaskIndex )
     {
     MSGD (( ERR_VXXXX_WRONG_TASK, "vptype", pid ));
     DBG1 (( MF__, "[T:0x%03u] Wrong pid %u", pTaskCtrl->ulTaskIndex, pid ));
     ABORT();
     }
  #endif

  switch ( pTaskCtrl->TaskType )
    {
    case TT_UT_EO00:
        proc_type.becomes(sp2pt_utility);
        break;
    case TT_TI_EO00:
        proc_type.becomes(sp2pt_timeout);
        break;
    case TT_US_EO00:
        proc_type.becomes(sp2pt_user);
        break;
    case TT_AL_EO00:
        proc_type.becomes(sp2pt_log_writer);
        break;
    case TT_TW_EO00:
        proc_type.becomes(sp2pt_trace_writer);
        break;
    case TT_SV_EO00:
        proc_type.becomes(sp2pt_server);
        break;
    case TT_DW_EO00:
        proc_type.becomes(sp2pt_pager);
        break;
    case TT_EV_EO00:
        proc_type.becomes(sp2pt_event);
        break;
    case TT_GC_EO00:
        proc_type.becomes(sp2pt_garbage_collector);
        break;
    case TT_BUP_EO00:
        proc_type.becomes(sp2pt_backup);
        break;
    default:
        MSGALL(( ERR_VPTYPE_WRONG_TASK_TYPE, pTaskCtrl->TaskType ));
        ABORT();
        break;
    }

  DBG3 (( MF__, "[T:0x%03u] *proc_type: %d", pTaskCtrl->ulTaskIndex, *proc_type ))

  DBGOUT_T (pTaskCtrl->ulTaskIndex);
  return;
  }

/*------------------------------*/

VOID vversion ( tsp00_Version  VAR_ARRAY_REF kernelversion,
                tsp00_Version  VAR_ARRAY_REF lzuversion )

  {
  #undef  MF__
  #define MF__ MOD__"vversion"
  eo46CtoP ( (tsp00_Byte*)lzuversion.asCharp(), RTE_GetRTEVersion(), sizeof(tsp00_Version) );
  return;
  }

/*------------------------------*/

VOID  vos ( tsp00_Os VAR_VALUE_REF os )
  {
  #undef  MF__
  #define MF__ MOD__"vos"
  DBGPAS;

  os.becomes(os_win32);

  return;
  }

/*------------------------------*/

VOID vinit ( tsp00_RteCommAddr VAR_VALUE_REF   rte_comm_ptr,
             tsp00_Int2                        len )
  {
  #undef  MF__
  #define MF__ MOD__"vinit"
  PUKT_CTRL_REC                  pUKT      = THIS_UKT_CTRL;
  PTASK_CTRL_REC                 pCurrTask = pUKT->pCTask;

  DBGIN_T (pCurrTask->ulTaskIndex);

  //
  // --- This is the first procedure called by the kernel code
  //
  sql51k_debug_break ( 1000 );

  if ( len != sizeof(RTE_COMM_REC) )
    {
    MSGD (( ERR_WRONG_COMM_REC_SIZE, len ))
    ABORT();
    }

  //
  // --- initialize task control
  //
  sql70k_init_task_ctrl ( pCurrTask );

  // - alter task activ counters
  sql74k_alter_task_cnt( pCurrTask, ADD_ACTIVE_COUNTER );

  /* Remove inactive move lock */
  sql74k_long_term_move_lock( pCurrTask, TASK_IS_INACTIVE, false ); 

  // --- return address of the kernel communication record
  memset ( &pCurrTask->KernelCtrlRecord, 0, sizeof(pCurrTask->KernelCtrlRecord) );
  rte_comm_ptr = &pCurrTask->KernelCtrlRecord;

  pCurrTask->TaskState = TSK_RUNNING_EO00;

  DBGOUT_T (pCurrTask->ulTaskIndex);
  return;
  }

/*------------------------------*/

VOID vfinish ( VOID )
  {
  #undef  MF__
  #define MF__ MOD__"vfinish"

  DBGPAS;

  sql51k_finish ();

  return;
  }


/*------------------------------*/

void  vutil_startup( tsp1_utility_startup VAR_VALUE_REF  startupReason )
{
#undef  MF__
#define MF__ MOD__"tsp1_utility_startup"
#ifdef DEBUG_RTE
    PTASK_CTRL_REC                 pTaskCtrl         = THIS_UKT_CTRL->pCTask;
#endif

    DBGIN_T (pTaskCtrl->ulTaskIndex);

    if (
      ( kgs.pUT->pUserCommCtrl->ulConnected == 0 ) &&
      ( kgs.pUT->TaskType                   == TT_UT_EO00 ) &&
        kgs.fWinShutdownPending )
    {
        MSGD (( INFO_AUTOSHUTDOWN, kgs.szServerDB ));
        startupReason.becomes(sp1us_shutdown);
    }
    else
      startupReason.becomes(sp1us_connect);

    DBGOUT_T (pTaskCtrl->ulTaskIndex);
}

/*------------------------------*/

void  vrestart ( tsp00_TaskId                 pid ,
                 tsp00_DevName VAR_ARRAY_REF  sysdevspace )
{
#undef  MF__
#define MF__ MOD__"vrestart"
#ifdef DEBUG_RTE
    PTASK_CTRL_REC     pTaskCtrl = THIS_UKT_CTRL->pCTask;
#endif

    DBGIN_T (pTaskCtrl->ulTaskIndex);

#ifdef DEVELOP_CHECKS
     if ( (ULONG)pid != THIS_UKT_CTRL->pCTask->ulTaskIndex )
     {
         MSGD (( ERR_VXXXX_WRONG_TASK, "vrestart", pid ));
         DBG1 (( MF__, "[T:0x%03u] Wrong pid %u", pTaskCtrl->ulTaskIndex, pid ));
         ABORT();
     }
#endif

    kgs.fRestarted    = TRUE;

    if ( kgs.ulStartupOptions & FORCE_AUTORESTART )
    {
        RTE_CloseDiagPipe();
        sql61k_post_xstart_sem ( kgs.szServerDB, FALSE );
    }

    RTE_SetDatabaseState(SERVER_ONLINE_EO00);

    DBGOUT_T (pTaskCtrl->ulTaskIndex);
    return;
}

/*------------------------------*/
/* PTS 1115383 */
void  voffline ( tsp00_TaskId              pid,
                 tsp00_ShutdownMode_Param  mode )
  {
#undef  MF__
#define MF__ MOD__"voffline"
#ifdef DEVELOP_CHECKS
   PUKT_CTRL_REC                 pUKT      = THIS_UKT_CTRL;
   PTASK_CTRL_REC                pTaskCtrl = pUKT->pCTask;
#else
   PTASK_CTRL_REC                pTaskCtrl = &kgs.pFirstTaskCtrl[pid - 1];
   PUKT_CTRL_REC                 pUKT      = pTaskCtrl->pUKT;
#endif

  DBGPAS_T (pTaskCtrl->ulTaskIndex);

#ifdef DEVELOP_CHECKS
   if ( (ULONG)pid != pTaskCtrl->ulTaskIndex )
     {
     MSGD (( ERR_VXXXX_WRONG_TASK, "voffline", pid ));
     DBG1 (( MF__, "[T:0x%03u] Wrong pid %u", pTaskCtrl->ulTaskIndex, pid ));
     ABORT();
     }
#endif

  kgs.taskResponsibleForOffline = (SAPDB_Int4)pid; // PTS 1134089

  pTaskCtrl->TaskState = TSK_VSHUTDOWN_EO00;

  kgs.fRestarted = FALSE;

  if ( mode == SHUTDOWN_KILL )
  {
      kgs.dumpDiagFiles = true;
      MSGCD (( INFO_SHUTDOWN_KILL_REQ  ));
      *kgs.pDBState = SERVER_SHUTDOWNKILL;
  }
  else
  {
      MSGCD (( INFO_SHUTDOWN_NORMAL_REQ  ));
      *kgs.pDBState = SERVER_SHUTDOWN;
  }

  UPDATE_SERVICE_OR_CONSOLE_STATE(*kgs.pDBState);

  // --- wakeup coordinator thread
  sql41c_post_event_sem ( kgs.Coord.ThrdCtrlHeader.hevSem, "Coord" );

  if ( kgs.fWinShutdownPending )
  {
      /* Former implementation used wait on utility task TSK_INACTIVE state...
         But this would wait forever, since the utility task is blocked in a
         vsuspend that waits for a server task to complete its job... That server
         task will be terminated by 'EXITTHREAD'... */
      kgs.fWinShutdownCompleted = true;
      /* This new flag indicates the loop is terminated... */
  }

  EXITTHREAD( 0, pUKT->ThrdCtrlHeader.hThrd );
  }

/*------------------------------*/

VOID sql51k_finish ( VOID )
  {
  #undef  MF__
  #define MF__ MOD__"sql51k_finish"
  PUKT_CTRL_REC               pUKT       = THIS_UKT_CTRL;
  PTASK_CTRL_REC              pCurrTask  = pUKT->pCTask;

  DBGIN;

  /* Don't move this task during finish and next startup phase... */
  sql74k_long_term_move_lock( pCurrTask, TASK_IS_INACTIVE, true ); 

  // PTS 1110953
  if ( XPARAM(showMaxStackUse) )
  {
      vsShowMaxStackUse( pUKT->pCTask->ulTaskIndex, eo92GetTaskTypeName(pUKT->pCTask->TaskType) );
  }
  // - alter task activ counters
  sql74k_alter_task_cnt( pCurrTask, SUB_ACTIVE_COUNTER );

  switch ( pCurrTask->TaskType )
    {
    case TT_EV_EO00:
    case TT_UT_EO00:
    case TT_US_EO00:
    case TT_SV_EO00:
      // release all open connections
      pCurrTask->pTaskObject->ClientConnections().ReleaseAllConnections();

      // --- release the current task or fiber
      //     this function should never return!!!
      sql88k_release_curr_task_or_fiber( &pUKT );

      //
      // --- we should never return here
      //
      break;

    case TT_TW_EO00:

      MSGD (( INFO_RELEASING_TRACEWRITER ));
      // --- preliminary: crash server after tracewriter wrote trace (and dump)
      switch ( *kgs.pDBState )
        {
        case SERVER_SHUTDOWN :
        case SERVER_SHUTDOWNKILL :
        case SERVER_SHUTDOWNREINIT :
        case SERVER_ABORT :
        case SERVER_KILL :
        case SERVER_STOP :
          pUKT->ThrdCtrlHeader.ThrdState = UKT_TERMINATED;
          pCurrTask->TaskState           = TSK_TERMINATED_EO00;
          ExitThread( 0 );
          break;
        }

      MSGD (( ERR_VFINISH_TW_DUR_NORM_OP, *kgs.pDBState ));
      ABORT();
      break;

    default:
      //
      // - only user tasks and the utility task should leave the kernel code
      //
      MSGD (( ERR_KERNELMAINPRG_RETURNED ));
      ABORT();
      break;
    }

  //
  // --- we should never return here
  //
  MSGD (( ERR_TASK_RETURNED ));
  ABORT();

  DBGOUT;
  return;
  }

/*------------------------------*/

BOOL  sql51k_auto_shutdown ( VOID )
{
#undef  MF__
#define MF__ MOD__"sql51k_auto_shutdown"
    static  BOOL            fAutoShutdownInProgress = FALSE;
    SAPDBErr_MessageList    messageList;

    BOOLEAN volatile *pGracefully;

    DBGPAS;

    if (  pkss
       && pkss->gracefully
       && ( fAutoShutdownInProgress == FALSE )
       && ( *kgs.pDBState           == SERVER_WARM ) )
    {
        pGracefully = const_cast<BOOLEAN volatile *>(&(pkss->gracefully));
        fAutoShutdownInProgress = TRUE;
        if ( kgs.fWinShutdownPending
          || !RTE_SpawnRestartShutdown( false ) )
        {
            kgs.ulTerminationTimeOut    = sql50k_get_termination_timeout() / 3;

            if ( sql53k_comm_cancel ( kgs.pUT, commErrCrash_esp01 ))
            {
                while ( ( kgs.pUT->pUserCommCtrl->ulConnected != 0 )            &&
                       ( kgs.pUT->TaskState                  != TSK_INACTIVE_EO00 ) &&
                       --kgs.ulTerminationTimeOut &&
                       *pGracefully )
                    SLEEP (1000);
            }

            if ( *pGracefully )
            {
                kgs.pUT->TaskState  = TSK_RUNNING_EO00;

                if ( sql88k_create_task_or_fiber ( kgs.pUT ) == NO_ERROR )
                {
                    if ( false == kgs.pUT->pTaskObject->Startup( messageList ))
                        RTE_Message (messageList, MSG_DIAG_CONSOLE);
                    else
                    {
                        kgs.ulTerminationTimeOut += (sql50k_get_termination_timeout() / 3) * 2;

                        while ( !kgs.fWinShutdownCompleted
                            &&  --kgs.ulTerminationTimeOut
                            && *pGracefully )
                            SLEEP (1000);
                    }
                }
            }
        }
        else
        {
            kgs.ulTerminationTimeOut    = sql50k_get_termination_timeout();

            while ( ( *kgs.pDBState == SERVER_WARM )
                &&  --kgs.ulTerminationTimeOut
                &&  *pGracefully )
                SLEEP (1000);
        }
    }

    kgs.ulTerminationTimeOut = 0;

    return ( !kgs.fRestarted );
}

/*------------------------------*/

VOID sql51k_debug_break ( ULONG ulDebugBreakPosition )
  {
  #undef  MF__
  #define MF__ MOD__"sql51k_debug_break"
  #ifdef DEBUG_RTE
   PTASK_CTRL_REC                pTaskCtrl = THIS_UKT_CTRL->pCTask;
  #endif

  DBGPAS_T (pTaskCtrl->ulTaskIndex);

  if ((kgs.ulSingleDebugBreakPos == ulDebugBreakPosition) ||
      (kgs.ulStaticDebugBreakPos == ulDebugBreakPosition))
    {
    if (kgs.ulSingleDebugBreakPos == ulDebugBreakPosition)
      kgs.ulSingleDebugBreakPos = 0;

    if ( !(kgs.ulServiceType & SERVICE_INTERACTIVE_PROCESS) )
      {
      MSGALL (( WRN_NO_INTERACT_WITH_DESCTOP ));
      DBGOUT;
      return;
      }

    DebugBreak();
    }


  DBGOUT;
  return;
  }

/*
 * ========================== LOCAL FUNCTIONS =================================
 */

static VOID sql51k_start_multiple_tasks ( PTASK_CTRL_REC pFirstMultipleTask,
                                          ULONG          ulNumTasks )
{
  #undef  MF__
  #define MF__ MOD__"sql51k_start_multiple_tasks"

  ULONG                          uldx;
  PTASK_CTRL_REC                 pTmpTaskCtrl;
  SAPDBErr_MessageList           messageList;

  DBGIN;

  pTmpTaskCtrl = pFirstMultipleTask;
  for ( uldx = 0; uldx < ulNumTasks; uldx++ )
    {
        if ( false == pTmpTaskCtrl->pTaskObject->Startup( messageList ))
            RTE_Crash (messageList);

    pTmpTaskCtrl++;
    }

  DBGOUT;
  return;
}

/*
 * =============================== END ========================================
 */
