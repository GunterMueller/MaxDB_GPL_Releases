/*!
  @file           vos88k.cpp
  @author         RaymondR
  @brief          USER KERNEL Thread
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
//#include "gos00.h"
#include "gos41.h"
#include "heo00.h"
#include "heo46.h"
#include "geo007_1.h"
#include "gos00k.h"
#include "geo002.h"
#include "heo38.h"
#include "heo69.h"
#include "gos74.h"
#include "RunTime/Tasking/RTETask_Task.hpp"
#include "RunTime/System/RTESys_MicroTime.h"
#include "RunTime/System/RTESys_SystemInfo.h"
#include "RunTime/Tasking/RTETask_LegacyUKTCtrl.h"

#if defined(_FASTCAP)
# include "fastcap.h"   /* nocheck */
#endif

/*
 *  DEFINES
 */
#define MOD__  "VOS88KC : "
#define MF__   MOD__"UNDEFINED"

/*
 *  MACROS
 */


/*
 *  LOCAL TYPE AND STRUCT DEFINITIONS
 */


/*
 * EXTERNAL VARIABLES AND FUNCTIONS
 */

extern "C" bool LVC_InProcDispatcher_dispInitComCache(void);

/*
 *  EXPORTED VARIABLES
 */


/*
 * LOCAL VARIABLES
 */


/*
 * LOCAL FUNCTION PROTOTYPES
 */
PSZ           sql88k_print_task_type           ( PTASK_CTRL_REC       pTaskCtrl );
VOID          sql88k_kernel_task               ( VOID );
VOID CALLBACK sql88k_kernel_fiber              ( PVOID                lpParameter );  // fiber data
VOID          sql88k_free_task_stack           ( VOID ); // no parameter passing via stack call!
VOID          sql88k_release_curr_fiber        ( PUKT_CTRL_REC        pUKT );


/*
 * ========================== GLOBAL FUNCTIONS ================================
 */

VOID _System sql88k_ukt ( PUKT_CTRL_REC pUKT )
  {
  #undef  MF__
  #define MF__ MOD__"sql88k_ukt"
  LONG                           lStackTop      = 0; //- must be the first var.
  ULONG                          rc             = NO_ERROR;
  PSZ                            pszPath        = NULL;
  PSZ                            pszName        = NULL;
  PUCHAR                         pStackLimit;
  ULONG                          ulRetry = 10;

  DBGIN;

  __try 
   {
#  if defined(_FASTCAP)
	 CAPNameUKT ( pUKT->ulUKTIndex ); 
#  endif

    pUKT->DispatcherLockingTask = 0;

    // --- save UKT control record pointer
    if (!TlsSetValue(kgs.TlsUKTCtrlIdx, pUKT))
      {
      rc = GetLastError();
      DBG1 ((MF__, "Error on setting thread local storage: rc = %u\n", rc));
      MSGD (( ERR_SETTING_TLS, rc));
      ABORT();
      }

    pszPath = SEM_UKT;
    pszName = kgs.pSemaNames[pUKT->ulSemaNo];

    do
      {
      sql41c_build_unique_obj_name ( pszName );

      // --- Create UKT event semaphore (SET)
      rc = sql41c_create_event_sem ( &pUKT->ThrdCtrlHeader.hevSem,
                                     pszPath, pszName,
                                     CREATE_EVENT_SEM_SHARED, FALSE,
                                     &kgs.WorldSA );
       // --- unique name already in use?
       if ( rc == ERROR_DUPLICATE_NAME )
         SLEEP ( 1 );
       }
    while (( rc == ERROR_DUPLICATE_NAME ) && (--ulRetry) );

    if ( rc != NO_ERROR )
      {
      if ( rc == ERROR_DUPLICATE_NAME )
        MSGCD (( ERR_CREATE_UKT_SEM, pszName, rc ));

      DBG1 ((MF__, "Error on creating semaphore: %s, rc = %u\n", "UKT", rc));
      ABORT();
      }

    // --- set the thread priority
    rc = SET_THREAD_PRIO ( pUKT->ThrdCtrlHeader.lThrdPrio,
                           pUKT->ThrdCtrlHeader.Tid,
                           pUKT->ThrdCtrlHeader.hThrd );

    if ( rc != NO_ERROR )
      {
      MSGD (( ERR_SETTING_THRD_PRIO, pUKT->ThrdCtrlHeader.Tid));
      DBG1 ((MF__, "Cannot set the thread priority, TID: %u, rc=%d",
                    pUKT->ThrdCtrlHeader.Tid, rc ));
      ABORT();
      }

    sql88k_set_UKT_CPU_relationship( pUKT, false );

    // --- set thread control info
    pUKT->ThrdCtrlHeader.ThrdState   = KT_RUNNING;
    pUKT->ThrdCtrlHeader.ulStartTime = kgs.ulCurrTime;

    // --- set the UKT start time in microseconds.
    //     Used by 'vccheck' to compute CPU time
    pUKT->IdleTimeMeasureBeginMicroseconds = pUKT->ulUKTStartTimeMicroSeconds = RTESys_MicroSecTimer();

    /* CR 1104345 */
    DBG3 (( MF__, "dispComInitialize for UKT %d called ...", pUKT->ulUKTIndex ));

    if ( !LVC_InProcDispatcher_dispInitComCache() )
    {
      MSGD (( ERR_DBSTART_UKT_COINITIALIZE_FAILED, pUKT->ulUKTIndex ));
      ABORT();
    }

    if ( XPARAM(fUseFibers) )
      {
      pUKT->pRelaseFiber = ConvertThreadToFiber(pUKT);

      if ( pUKT->pRelaseFiber == NULL )
        {
        MSGCD (( ERR_CONVERTING_THREAD_TO_FIBER, GetLastError() ));
        ABORT();
        }
      }
    else
      {
      // --- prepare the threads stack to create the task stacks.
      //     There are no large structs or arrays allocated in the current thread,
      //     so, the next page boundary above any local (auto) variable
      //     will be the top of the current thread's stack.
      //     WARNING: This code is highly machine dependent!
      pStackLimit = (PUCHAR)&lStackTop - (kgs.ulSystemPageSize / 2);
      pStackLimit = (PUCHAR)ALIGN( (tsp00_Longuint)pStackLimit, kgs.ulSystemPageSize );

      sql71k_init_ukt_and_task_stacks  ( pUKT, pStackLimit );
      }

    MSGD (( INFO_UKT_STARTED, pUKT->ThrdCtrlHeader.Tid ));

    /*
     * -- is the timer task in this thread?  - start it!
     */

    if ( kgs.pTI->pUKT == pUKT )
      {
      if ( NO_ERROR != sql88k_create_task_or_fiber ( kgs.pTI ))
          ABORT();

      pUKT->pCTask = kgs.pTI;

      // we altered the current task in legacy code,
      // so we have to alter the current task object in
      // the TaskScheduler object
      UpdateCurrentTaskInTaskScheduler(pUKT);

      DBG3 (( MF__, "Entering first task" ));
      }
    else
      {
      DBG3 (( MF__, "Entering dispatcher to wait for first task" ));
      pUKT->pCTask = NULL;
      sql74k_dispatcher ( pUKT );
      }

    if ( XPARAM(fUseFibers) )
      {
      SwitchToFiber( pUKT->pCTask->pFiberAddress );  // This switch returns with the first
                                                     // fiber release request

      for (;;) //  For now this main fiber handles all fiber release requests
        {
        sql88k_release_curr_fiber ( pUKT );

        pUKT->pCTask = NULL;
        sql74k_dispatcher ( pUKT );

        // --- switch to the requested task and wait for the next release request
        pUKT->taskSwitches++;
        SwitchToFiber( pUKT->pCTask->pFiberAddress );
        }
      }
    else
      {
      GOTO_FIRST_TASK (pUKT->pCTask);
      }

    MSGD (( ERR_TASK_RETURNED ));   // --- we should never return here
    ABORT();
    }
  __except( sql50k_excepthandler(GetExceptionInformation()) ) {;}

  DBGOUT;
  EXITTHREAD( 0, pUKT->ThrdCtrlHeader.hThrd );
  }

/*------------------------------*/

APIRET sql88k_create_task_or_fiber ( PTASK_CTRL_REC pTaskCtrl )
  {
  #undef  MF__
  #define MF__ MOD__"sql88k_create_task_or_fiber"
  APIRET    rc  = NO_ERROR;

  if ( XPARAM(fUseFibers) )
  {
      pTaskCtrl->pFiberAddress = sql81k_create_kernel_fiber( pTaskCtrl->ulTaskStackSize
                                                           , sql88k_kernel_fiber
                                                           , pTaskCtrl);
	  if( NULL == pTaskCtrl->pFiberAddress ) 
      {
          rc = GetLastError();

          if ( ERROR_NOT_ENOUGH_MEMORY == rc )
          {
              MSGCD (( ERR_CREATING_FIBER_OUT_OF_MEMORY ));
          }
          else
          {
              MSGCD (( ERR_CREATING_FIBER, rc ));
          }
      }
  }
  else
  {
      if ( pTaskCtrl->TaskType == TT_UT_EO00 )
      {
          sql71k_init_task_stack ( &pTaskCtrl->Context, sql88k_kernel_task );
      }
      else
      {
          rc = sql71k_alloc_task_stack ( &pTaskCtrl->Context );

          if ( rc == NO_ERROR )
              sql71k_init_task_stack( &pTaskCtrl->Context, sql88k_kernel_task );
      }
  }

  return ( rc );
  }

/*------------------------------*/

VOID sql88k_release_curr_task_or_fiber ( PUKT_CTRL_REC* ppUKT )
  {
  #undef  MF__
  #define MF__ MOD__"sql88k_release_curr_task_or_fiber"
  APIRET            rc   = NO_ERROR;

#if defined(UNCONNECT_TASK_LIST)
  if (( (*ppUKT)->pCTask->MoveLock.Type.LongTerm != NEVER_MOVE_TASK ) &&
      ( (*ppUKT)->pCTask->Connectable            == true ))
  {
    sql74k_temp_move_lock( (*ppUKT)->pCTask, 1, true ); // don't move this task during remove phase...

    (*ppUKT)->pCTask->SpecDLQElem2.ulReqType = REQ_REMOVE_TASK_FROM_UKT;               
    sql73k_UKT_enqu ( *ppUKT, &(*ppUKT)->pCTask->SpecDLQElem2 );
    GOTO_DISP(ppUKT);
  }
#endif

  if ( XPARAM(fUseFibers) )
    {
    // --- Switch to UKT startup fiber to release this fiber.
    //     Killing this fiber directly would force a kill of the whole thread!!!!!!!
    //
    //     ATTENTION: Switching to the main fiber relases the current fiber!
    //
    SwitchToFiber( (*ppUKT)->pRelaseFiber );
    }
  else
    {
    // --- are we already on the temporary UKT stack?
    if ( (*ppUKT)->fRunningOnTempStack == FALSE )
      {
      (*ppUKT)->fRunningOnTempStack = TRUE;

      // --- switch to UKTs stack to leave the current task stack
      sql71k_init_task_stack (&(*ppUKT)->Context, sql88k_free_task_stack);

      if (sql71k_chg_task_stack (&(*ppUKT)->Context, &(*ppUKT)->pCTask->Context))
        {
        MSGD  (( ERR_STACK_OVERFLOW, (*ppUKT)->pCTask->ulTaskIndex ));
        ABORT ();
        }
      }
    else
      sql88k_free_task_stack ();
    }
  }
/*
 * ========================== LOCAL FUNCTIONS =================================
 */


static VOID sql88k_kernel_task ( VOID )
  {
  #undef  MF__
  #define MF__ MOD__"sql88k_kernel_task"
  PUKT_CTRL_REC               pUKT = THIS_UKT_CTRL;

  DBGIN_T (pUKT->pCTask->ulTaskIndex);

  DBG3 (( MF__, "[T:0x%03u] Task type %5s",
           pUKT->pCTask->ulTaskIndex,
           sql88k_print_task_type( pUKT->pCTask ) ));

  // PTS 1110953
  if ( XPARAM(showMaxStackUse) )
  {
      vsReinit(pUKT->pCTask->ulTaskIndex,0);
  }

  kgs.pKernelEntryFunc ();

  sql51k_finish ();

  MSGD (( ERR_TASK_RETURNED ));   // --- we should never return here
  ABORT();

  return;
  }

/*------------------------------*/

VOID CALLBACK sql88k_kernel_fiber( PVOID lpParameter )  // fiber data
  {
  #undef  MF__
  #define MF__ MOD__"sql88k_kernel_fiber"
  LONG                        lStackTop      = 0; //- must be the first var.
  PUKT_CTRL_REC               pUKT = THIS_UKT_CTRL;
  PUCHAR                      pStackLimit;

  DBGIN_T (pUKT->pCTask->ulTaskIndex);

  __try 
    {
#  if defined(_FASTCAP)
//    CAPNameFiber(pUKT->pCTask->ulTaskIndex, sql88k_print_task_type(pUKT->pCTask) );
    NameProfile( "TaskName", PROFILE_FIBERLEVEL, PROFILE_CURRENTID);
#  endif

    pStackLimit = (PUCHAR)&lStackTop - (kgs.ulSystemPageSize / 2);
    pStackLimit = (PUCHAR)ALIGN( (tsp00_Longuint)pStackLimit, kgs.ulSystemPageSize );

    pUKT->pCTask->pCurrStackPointer         = (PCHAR)&pUKT;
    pUKT->pCTask->Context.pStackPointer     = (PUCHAR)pUKT->pCTask->pCurrStackPointer;
    pUKT->pCTask->Context.pStackLimit       = pStackLimit;
    pUKT->pCTask->Context.pStackBase        = pUKT->pCTask->Context.pStackLimit -
                                              pUKT->pCTask->ulTaskStackSize;

    // PTS 1110953
    if ( XPARAM(showMaxStackUse) )
    {
        vsReinit(pUKT->pCTask->ulTaskIndex,0);
    }

    kgs.pKernelEntryFunc ();

    sql51k_finish ();

    MSGD (( ERR_TASK_RETURNED ));   // --- we should never return here
    ABORT();
    }
  __except( sql50k_excepthandler(GetExceptionInformation()) ) {;}

  return;
  }

/*------------------------------*/

static VOID sql88k_free_task_stack ( VOID ) // no parameter passing via stack call!
  {
  #undef  MF__
  #define MF__ MOD__"sql88k_free_task_stack"
  PUKT_CTRL_REC               pUKT       = THIS_UKT_CTRL;
  PTASK_CTRL_REC              pCurrTask  = pUKT->pCTask;

  DBGIN;

  // --- free the old tasks stack
  if ( pCurrTask->TaskType != TT_UT_EO00 )
    sql71k_free_task_stack ( &pCurrTask->Context );

  // --- signal that we have left the stack
  pCurrTask->pCurrStackPointer = (PCHAR)pCurrTask->Context.pStackLimit;

  // --- signal that the task is inactive and able to
  //     handle a new start request
  pCurrTask->TaskState = TSK_INACTIVE_EO00;

  //  Enqueue task in list of unconnected tasks of the TaskScheduler. If the
  //  task type is not connectable this call is ignored
  pUKT->pCTask->pTaskObject->EnqueueIntoUnconnectedTasksQueue();

  DBG3 (( MF__, "Entering dispatcher to wait for a new task" ));
  pUKT->pCTask  = NULL;
  sql74k_dispatcher ( pUKT );

  pUKT->fRunningOnTempStack = FALSE;

  if ( sql71k_chg_task_stack ( &pUKT->pCTask->Context, &pUKT->Context ) )
    {
    MSGD  (( ERR_STACK_OVERFLOW, 0 ));
    ABORT ();
    }

  MSGD (( ERR_TASK_RETURNED ));   // --- we should never return here
  ABORT();

  DBGOUT;
  return;
  }

/*------------------------------*/

static VOID sql88k_release_curr_fiber ( PUKT_CTRL_REC     pUKT ) 
  {
  #undef  MF__
  #define MF__ MOD__"sql88k_release_curr_fiber"

  DBGIN;

  sql81k_delete_kernel_fiber( pUKT->pCTask->ulTaskStackSize
                            , pUKT->pCTask->pFiberAddress );

  // --- signal that the task is inactive and able to
  //     handle a new start request
  pUKT->pCTask->TaskState = TSK_INACTIVE_EO00;

  //  Enqueue task in list of unconnected tasks of the TaskScheduler. If the
  //  task type is not connectable this call is ignored
  pUKT->pCTask->pTaskObject->EnqueueIntoUnconnectedTasksQueue();

  DBGOUT;
  return;
  }

/*------------------------------*/

SAPDB_Bool sql88k_reset_UKT_CPU_relationship ( PUKT_CTRL_REC     pUKT )
{
    SAPDB_Bool      ok   = false;

    if ( pUKT->ThrdCtrlHeader.Processor ) // reset necessary?
    {
        DWORD_PTR 	    SystemAffinityMask;
        DWORD_PTR 	    ProcessAffinityMask;

        if ( !GetProcessAffinityMask( GetCurrentProcess(),
                                      &ProcessAffinityMask,
                                      &SystemAffinityMask ))
        {
            ProcessAffinityMask = 1;
        }

        if (( -1 == SetThreadIdealProcessor( pUKT->ThrdCtrlHeader.hThrd, MAXIMUM_PROCESSORS )) ||
            (  0 == SetThreadAffinityMask( pUKT->ThrdCtrlHeader.hThrd , ProcessAffinityMask )))
        {
            MSGALL (( ERR_SETTING_THREAD_AFFINITY, GetLastError() ));
        }
        else
        {
            
            WAIT_UNTIL_ALONE( &kgs.lExclusive );
            kgs.ProcessorUsageMask  &= ~pUKT->ThrdCtrlHeader.Processor;
            CLEARLOCK( &kgs.lExclusive );

            pUKT->ThrdCtrlHeader.Processor       = 0;
            pUKT->ThrdCtrlHeader.CPURelationship = UKT_CPU_RELATIONSHIP_NONE;
            ok = true;
        }

    }

    return ok;
}

/*------------------------------*/

SAPDB_Bool sql88k_set_UKT_CPU_relationship( PUKT_CTRL_REC     pUKT,
                                            SAPDB_Bool        forceAffinity )
{
    SAPDB_UInt4     logProccesorNo;
    SAPDB_UInt4     processorRelatedness[MAX_PROCESSORS];
    SAPDB_UInt4     numOfPhysProcAllowed = 0; // number of physical processor where the process is allowed to run on
    SAPDB_Bool      ok   = false;

    if (( pUKT->ulNumUserTasks == 0 ) && ( false == forceAffinity )) // user ukts only
        return true;

    // get physical procesor information depending on the process affinity mask
    numOfPhysProcAllowed = RTESys_GetPhysicalProcessorInfomation( processorRelatedness, MAX_PROCESSORS, true );

    if ( numOfPhysProcAllowed > XPARAM(ulMaxCPU) && kgs.fMoreUserUKTsThanMaxCPU )
    {
          SAPDB_UInt4 Proccesor = (pUKT->ulUKTIndex % XPARAM(ulMaxCPU)) + 1;

          // - find processor
          for ( logProccesorNo = 0; logProccesorNo < MAX_PROCESSORS; logProccesorNo++ )
          {
              if ( Proccesor == processorRelatedness[logProccesorNo] )
              {
                  if ( -1 == SetThreadIdealProcessor( pUKT->ThrdCtrlHeader.hThrd, logProccesorNo ) )
                  {
                      MSGALL (( ERR_SETTING_THREAD_AFFINITY, GetLastError() ));
                  }
                  else
                  {
                      pUKT->ThrdCtrlHeader.Processor        = 1 << logProccesorNo;
                      pUKT->ThrdCtrlHeader.CPURelationship  = UKT_CPU_RELATIONSHIP_IDEAL_PROCESSOR;
                      kgs.ProcessorUsageMask               |= 1 << logProccesorNo;
                      ok = true;
                  }
              }
          }
    }
    else
    {
        if ( numOfPhysProcAllowed  ==  1 )
            return true; // nothing to do!

        if ( forceAffinity  || UKT_CPU_RELATIONSHIP_NONE != kgs.UKTProcessorRelationship )
        {
            SAPDB_ULong logProccesorMask;

            // - find free logical processors
            for ( logProccesorMask = 1, logProccesorNo = 0;   
                  logProccesorMask != 0 &&          // mask overflow
                  logProccesorNo < MAX_PROCESSORS;  // maximum processors?
                  logProccesorMask <<= 1, logProccesorNo++ ) 
            { 
                if ((~kgs.ProcessorUsageMask & logProccesorMask ) && // logical processor unused ?
                    ( 0 != processorRelatedness[logProccesorNo] ))   // allowed to run on this processor?
                {
                    // get the processor core
                    SAPDB_UInt4 Proccesor = processorRelatedness[logProccesorNo];
                    SAPDB_UInt4 ProcNo;

                    // find next logical processors residing in the core 
                    for ( ProcNo = 0; ProcNo < kgs.ulNumOfProcessors; ProcNo++ )
                    {
                        if ( Proccesor == processorRelatedness[ProcNo] )
                            logProccesorMask |= 1 << ProcNo;
                    }

                    WAIT_UNTIL_ALONE( &kgs.lExclusive );
                    if (~kgs.ProcessorUsageMask & logProccesorMask) // logical processor unused ?
                    {
                        kgs.ProcessorUsageMask |= logProccesorMask;
                        CLEARLOCK( &kgs.lExclusive );
                        break;
                    }
                    CLEARLOCK( &kgs.lExclusive );
                }
            }

            // - free processor found?
            if (( logProccesorMask != 0 ) && ( logProccesorNo < MAX_PROCESSORS ))
            {
                if (( true == forceAffinity ) || ( UKT_CPU_RELATIONSHIP_AFFINITY == kgs.UKTProcessorRelationship ))
                    ok = ( 0 != SetThreadAffinityMask( pUKT->ThrdCtrlHeader.hThrd, logProccesorMask ));
                else
                    ok = ( -1 != SetThreadIdealProcessor( pUKT->ThrdCtrlHeader.hThrd, logProccesorNo ) );

                if ( false == ok )
                {
                    MSGALL (( ERR_SETTING_THREAD_AFFINITY, GetLastError() ))

                    WAIT_UNTIL_ALONE( &kgs.lExclusive );
                    kgs.ProcessorUsageMask  &= ~logProccesorMask;
                    CLEARLOCK( &kgs.lExclusive );
                }
                else
                {
                    pUKT->ThrdCtrlHeader.Processor        = logProccesorMask;
                    pUKT->ThrdCtrlHeader.CPURelationship  = ( true == forceAffinity ) ? UKT_CPU_RELATIONSHIP_AFFINITY : kgs.UKTProcessorRelationship;
                }
            }
        }
    }

    return ok;
}

/*------------------------------*/

#if defined(DEBUG_RTE) | defined(FASTCAP)
static PSZ sql88k_print_task_type ( PTASK_CTRL_REC    pTaskCtrl )
  {
  static  CHAR szLine[20] ;

  switch ( pTaskCtrl->TaskType )
    {
    case TT_TI_EO00:
      strcpy ( szLine,  "TI" );
      break;
    case TT_AL_EO00:
      strcpy ( szLine,  "AL" );
      break;
    case TT_TW_EO00:
      strcpy ( szLine,  "TW" );
      break;
    case TT_DW_EO00:
      strcpy ( szLine,  "DW" );
      break;
    case TT_RC_EO00:
      strcpy ( szLine,  "RC" );
      break;
    case TT_SV_EO00:
      strcpy ( szLine,  "SV" );
      break;
    case TT_UT_EO00:
      strcpy ( szLine,  "UT" );
      break;
    case TT_US_EO00:
      strcpy ( szLine,  "US" );
      break;
    case TT_EV_EO00:
      strcpy ( szLine,  "EV" );
      break;
    case TT_GC_EO00:
      strcpy ( szLine,  "GC" );
      break;
    case TT_BUP_EO00:
      strcpy ( szLine,  "BUP" );
      break;
    default:
      strcpy ( szLine,  "UNKNW" );
    }

  return ( szLine );
  }
#endif

/*
 * =============================== END ========================================
 */
