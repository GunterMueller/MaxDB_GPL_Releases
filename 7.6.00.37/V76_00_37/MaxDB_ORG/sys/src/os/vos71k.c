/*!
  @file           vos71k.c
  @author         RaymondR
  @brief          kernel task stack management
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
#include "heo00.h"
#include "heo46.h"
#include "geo007_1.h"
#include "gos00k.h"
#include "geo002.h"
#include "geo70k.h"

extern LPVOID RTE_AllocStack(LPVOID startAddress, SAPDB_ULong stackSize, SAPDB_ULong guardSize, SAPDB_Bool doCommit);
extern SAPDB_Bool RTE_FreeStack(LPVOID startAddress, SAPDB_ULong stackSize, SAPDB_ULong guardSize);

/*
 *  DEFINES
 */
#define MOD__  "VOS71KC : "
#define MF__   MOD__"UNDEFINED"


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


/*
 * ========================== GLOBAL FUNCTIONS ================================
 */

//
//  Initialized (freed) Task Stack:
//
//                 ~                ~
//                 +----------------+               TASK x-1
//                 |    COMMITED    |
//                 +----------------+
//                 |    GUARD_PAGE  |
//                 +================+ <- StackLimit
//                 |    COMMITED    | <- StackPointer
//                 +----------------+
//                 |    COMMITED    |
//                 +----------------+
//                 |    COMMITED    |
//                 +----------------+               TASK x
//                 ~                ~
//                 ~                ~
//                 +----------------+
//                 |    COMMITED    |
//                 +----------------+
//                 |    COMMITED    |
//                 +----------------+
//                 |    GUARD_PAGE  |
//                 +================+ <- StackBase
//                 |    COMMITED    |               TASK x+1
//                 ~                ~
//

VOID sql71k_init_ukt_and_task_stacks ( PUKT_CTRL_REC pUKTCtrl, 
                                       PUCHAR        pStackLimit )
  {
  #undef  MF__
  #define MF__ MOD__"sql71k_init_ukt_stack"
  PTASK_CTRL_REC              pCurrTask;
  PUCHAR                      pTaskStackBase;
  APIRET                      rc = NO_ERROR;

  DBGPAS;

  DBG3 (( MF__, "Threads stack limit: %#x ", pStackLimit ));

  //
  // --- prepare temporary stack for freeing tasks stack
  //
  pUKTCtrl->Context.pStackLimit =
    pStackLimit - UKT_BASE_STACK_SIZE;

  pUKTCtrl->Context.pStackPointer = (PCHAR)
    ALIGN((tsp00_Longuint)(pUKTCtrl->Context.pStackLimit - ALIGNMENT_VALUE - 1),
           ALIGNMENT_VALUE);

  pUKTCtrl->Context.pStackBase =
    pUKTCtrl->Context.pStackLimit - UKT_TEMP_STACK_SIZE;

  if ( sql02_get_platform_id () == VER_PLATFORM_WIN32_NT )
  {
     if ( !RTE_AllocStack( pUKTCtrl->Context.pStackBase
                         , pUKTCtrl->Context.pStackLimit - pUKTCtrl->Context.pStackBase - 1
                         , 0
                         , true ) )
     {
       rc = GetLastError();
     }
  }

  // PTS 1106437
  if ( rc == NO_ERROR )
  {
      DWORD  dwTaskStacks = (DWORD)(pUKTCtrl->ulUKTStackSize - kgs.ulUKTBaseStackSize);
 
      pTaskStackBase = RTE_AllocStack( NULL
                                     , dwTaskStacks + 1
                                     , 0
                                     , false );

      if ( pTaskStackBase != NULL )
      {
        DBG1 ((MF__, "%2d: Stack for tasks at %0X, Base = %X", 
			         pUKTCtrl->ulUKTIndex, pTaskStackBase, pTaskStackBase + dwTaskStacks ));
        pTaskStackBase += dwTaskStacks ;
      }
      else 
      {
        rc = GetLastError();
        MSGD(( ERR_STACK_ALLOC_HEAP_FAILED, dwTaskStacks, rc));
      }
  }

  if ( rc != NO_ERROR )
    {
    MSGD  (( ERR_ALLOC_TASK_STACK, rc ));
    DBG1 ((MF__, "Could not prepare the tasks stack, rc = %d", rc ));
    DBGOUT;
    ABORT();
    }

  //
  // --- prepare stack for tasks
  //

  // --- repeat until the end of the task-control-record-queue
  for ( pCurrTask =  kgs.pFirstTaskCtrl;
        pCurrTask <= kgs.pLastTaskCtrl;
        pCurrTask++)
    {
    if ( pCurrTask->pUKT != pUKTCtrl ) // ignore foreign tasks
      continue;

    pCurrTask->Context.pStackLimit    = pTaskStackBase;
    pCurrTask->Context.pStackPointer  = pTaskStackBase;
    pTaskStackBase                   -= pCurrTask->ulTaskStackSize;
    pCurrTask->Context.pStackBase     = pTaskStackBase;
    pCurrTask->pCurrStackPointer      = pCurrTask->Context.pStackLimit;

    /* PTS 1106437 */
    pTaskStackBase                   -= NUM_GUARDPAGES_EO70 * kgs.ulSystemPageSize;

    DBG3 (( MF__, "%2d: Task index: %d, type: %d", pUKTCtrl->ulUKTIndex, 
		                                           pCurrTask->ulTaskIndex, pCurrTask->TaskType ));
    DBG3 (( MF__, "%2d: Stack base: %#x, limit: %#x", pUKTCtrl->ulUKTIndex, 
		                                              pCurrTask->Context.pStackBase, pCurrTask->Context.pStackLimit ));
    DBG3 (( MF__, "%2d: Guard page: %#x            ", pUKTCtrl->ulUKTIndex, pTaskStackBase ));

    if ( pCurrTask->TaskType == TT_UT_EO00 )
      {
      if ( sql71k_alloc_task_stack ( &pCurrTask->Context ) != NO_ERROR )
        ABORT();
      }
    }

  #ifdef DEBUG_RTE
   DBG3 (( MF__, "Initialized stack :" ));
   sqlk01_view_stack_page_flags ( );
  #endif

  }

#define STACK_BASE_WITH_GUARD(_StackBase) ((_StackBase) - NUM_GUARDPAGES_EO70 * kgs.ulSystemPageSize)
#define STACK_SIZE_WITH_GUARD(_StackSize) ((_StackSize) + NUM_GUARDPAGES_EO70 * kgs.ulSystemPageSize)

/*------------------------------*/

LONG sql71k_alloc_task_stack ( CONTEXT_REC     *pContext )
  {
  #undef  MF__
  #define MF__ MOD__"sql71k_alloc_task_stack"

  APIRET                      rc = NO_ERROR;
  ULONG                       StackSize = (ULONG)(pContext->pStackLimit - pContext->pStackBase);

  DBGIN;


   if ( sql02_get_platform_id () == VER_PLATFORM_WIN32_NT )
   {
     DBG3 (( MF__, "Committing Base : %#x size: %#x", 
                         STACK_BASE_WITH_GUARD(pContext->pStackBase),
                         STACK_SIZE_WITH_GUARD(StackSize) - 1));

     if ( !RTE_AllocStack( STACK_BASE_WITH_GUARD(pContext->pStackBase)
                         , STACK_SIZE_WITH_GUARD(StackSize) - 1
                         , NUM_GUARDPAGES_EO70 * kgs.ulSystemPageSize
                         , true ) )
     {
       rc = GetLastError();
     }
   }

  if ( rc != NO_ERROR )
    {
    MSGD  (( ERR_COMMIT_TASK_STACK, STACK_BASE_WITH_GUARD(pContext->pStackBase), STACK_SIZE_WITH_GUARD(StackSize) - 1, rc ));
    DBG1 ((MF__, "Could not prepare the tasks stack, rc = %d", rc ));
    DBGOUT;
    return ( rc );
    }
  else
  {
     // PTS 1106437
     // Only the first page after the stack has to be marked.
     // An access to this page should generate a guard exception.
     // In case of NUM_GUARDPAGES_EO70 > 1 the exception handler needs more then
     // one page, and should not generate another guard exception.

     DBG3 (( MF__, "Guard page Base : %#x size: %#x", 
                         pContext->pStackBase - kgs.ulSystemPageSize,
                         kgs.ulSystemPageSize));
     if (!VirtualAlloc( pContext->pStackBase - kgs.ulSystemPageSize,
                        kgs.ulSystemPageSize,
                        MEM_COMMIT, PAGE_READWRITE | PAGE_GUARD ))
     {
       rc = GetLastError();
       MSGD  (( ERR_SET_TASK_STACK_GUARD, pContext->pStackBase - kgs.ulSystemPageSize, rc ));
     }
  }

  #ifdef DEBUG_RTE
   DBG3 (( MF__, "Freed stack :" ));
   sqlk01_view_stack_page_flags ( );
  #endif

  DBGOUT;
  return ( rc );
  }

/*------------------------------*/

VOID sql71k_init_task_stack ( CONTEXT_REC     *pContext,
                              VOID            (* pFunc) (VOID) )
  {
  #undef  MF__
  #define MF__ MOD__"sql71k_init_task_stack"
  APIRET                      rc = NO_ERROR;

  DBGIN;

  // --- compute the stack pointer
  pContext->pStackPointer = (PCHAR)
      ALIGN((tsp00_Longuint)(pContext->pStackLimit-ALIGNMENT_VALUE-1), ALIGNMENT_VALUE);

  sql71k_init_task_stack_frame ( pContext, pFunc );

  DBGOUT;
  return;
  }

/*------------------------------*/

VOID sql71k_free_task_stack ( CONTEXT_REC *pContext )
  {
  #undef  MF__
  #define MF__ MOD__"sql71k_free_task_stack"

  APIRET                      rc = NO_ERROR;
  #if !defined(_WIN32)
   ULONG                      ulRegionSize;
   ULONG                      ulAttributeFlags;
  #endif


  DBGIN;

   if ( sql02_get_platform_id () == VER_PLATFORM_WIN32_NT )
     {
     DBG3 (( MF__, "Freeing    Base : %#x size: %#x", 
                         STACK_BASE_WITH_GUARD(pContext->pStackBase),
                         STACK_SIZE_WITH_GUARD(pContext->pStackLimit-pContext->pStackBase)-1));
     if (!RTE_FreeStack( STACK_BASE_WITH_GUARD(pContext->pStackBase),
                         STACK_SIZE_WITH_GUARD(pContext->pStackLimit-pContext->pStackBase) - 1,
                         NUM_GUARDPAGES_EO70 * kgs.ulSystemPageSize ) )
       {
       rc = GetLastError();
       }
     }

  if ( rc != NO_ERROR )
    {
    MSGD  (( ERR_FREE_TASK_STACK, rc ));
    DBG1 ((MF__, "Could not prepare the tasks stack, rc = %d", rc ));
    ABORT();
    }

  DBGOUT;
  }

/*
 * ========================== LOCAL FUNCTIONS =================================
 */


/*
 * =============================== END ========================================
 */
