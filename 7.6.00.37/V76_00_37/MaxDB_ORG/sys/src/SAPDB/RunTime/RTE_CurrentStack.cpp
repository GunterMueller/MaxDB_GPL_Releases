/*!
  @file           RTE_CurrentStack.cpp
  @author         JoergM
  @ingroup        RunTime
  @brief          Current stack handling

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


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/RTE_CurrentStack.hpp"
#define KERNEL_LZU
#include "geo50_0.h"

#if  defined(IA64) || defined(HP_IA64) || defined(_M_IA64)
#  include "RunTime/RTE_IA64GetBsp.h" /* nocheck */
#endif

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define SAPDB_STACK_PATTERN 0xBBBBBBBBU
#define SAPDB_STACK_PATTERN_BYTE 0xBB
#define SAPDB_MAX_HEURISTIC_CHECK (81920/sizeof(SAPDB_Int4))

// At the end of the stack some bytes are not overwritten. The amount is machine dependend
#if defined(_WIN32)
#  define SAPDB_STACK_SKIP 16
#elif defined(PA20W) || defined(PA11)
#  define SAPDB_STACK_SKIP 12
#  define HAS_INVERTED_STACK_GROW
#else
#  define SAPDB_STACK_SKIP 8
#  undef  HAS_INVERTED_STACK_GROW
#endif

 /*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/
/*ALL*/
/*ALL*/static void          GetCurrentStack ( SAPDB_UInt4 * &currentStack );
/*ALL*/
/*ALL*/static TASK_CTRL     GetTaskControl  ( RTE_TaskId taskId );
/*ALL*/
/*ALL*/static SAPDB_UInt4   GetCurrentStackUsage(TASK_CTRL pTaskCtrl);

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

/*------------------------------------------------------------------------------------
  Implements vscheck() for kernel tasks
 */
/*ALL*/SAPDB_UInt4 RTE_CurrentStackUsage(RTE_TaskId taskId)
/*ALL*/{
/*ALL*/           TASK_CTRL pTaskCtrl = GetTaskControl(taskId);
/*ALL*/           if ( !pTaskCtrl )
/*ALL*/           {
/*ALL*/               return 0;
/*ALL*/           }
/*ALL*/           SAPDB_UInt4 currentStackUsage = GetCurrentStackUsage(pTaskCtrl);
/*ALL*/           return currentStackUsage;
/*ALL*/}

//---------------------------------------------------------

/*ALL*/SAPDB_UInt4 RTE_CurrentStackFree(RTE_TaskId taskId)
/*ALL*/{
/*ALL*/           TASK_CTRL pTaskCtrl = GetTaskControl(taskId);
/*ALL*/           if ( !pTaskCtrl )
/*ALL*/           {
/*ALL*/               return 0;
/*ALL*/           }
/*ALL*/           SAPDB_UInt4 currentStackUsage = GetCurrentStackUsage(pTaskCtrl);
/*ALL*/           SAPDB_UInt4 stackSize;
#ifdef WIN32
/*WIN*/           stackSize = (SAPDB_UInt4)pTaskCtrl->ulTaskStackSize;
#else
/*UNIX*/          stackSize = pTaskCtrl->stack_size;
#endif
/*ALL*/           return stackSize - currentStackUsage;
/*ALL*/}



/*------------------------------------------------------------------------------------
  Implements vsfill() for kernel tasks if called as RTE_ReinitializeStack(0,0)
 */
/*ALL*/void RTE_ReinitializeStack(RTE_TaskId taskId, SAPDB_UInt4 maxUsed)
/*ALL*/{
/*ALL*/    TASK_CTRL pTaskCtrl = GetTaskControl(taskId);
/*ALL*/    SAPDB_UInt4 *pMaxFillPosition;
/*ALL*/    SAPDB_UInt4 *pStackImage;
/*ALL*/    SAPDB_UInt4 *pFirstUsedOfStack; // top of stack
/*ALL*/    SAPDB_UInt4 *pLastUsedOfStack;  // bottom of stack stck grows from top to bottom
/*ALL*/    SAPDB_UInt4  stackSize;         // total stack size
/*ALL*/
#if defined(_WIN32)
/*WIN*/    stackSize = (SAPDB_UInt4)pTaskCtrl->ulTaskStackSize;
/*WIN*/    pFirstUsedOfStack = (SAPDB_UInt4 *)(pTaskCtrl->Context.pStackLimit);
/*WIN*/    pLastUsedOfStack = (SAPDB_UInt4 *)(pTaskCtrl->Context.pStackBase + SAPDB_STACK_SKIP);
#else
/*UNIX*/   stackSize = pTaskCtrl->stack_size;
#  ifdef HAS_INVERTED_STACK_GROW
/*UNIX*/   /* Machine stack grows from low to high addresses */
/*UNIX*/   pFirstUsedOfStack = (SAPDB_UInt4 *)(pTaskCtrl->StackChunkStart);
/*UNIX*/   pLastUsedOfStack = (SAPDB_UInt4 *)(pTaskCtrl->StackChunkEnd - SAPDB_STACK_SKIP);
#  else
/*UNIX*/   /* Machine stack grows from high to low addresses */
/*UNIX*/   pFirstUsedOfStack = (SAPDB_UInt4*)(pTaskCtrl->StackChunkEnd);
#    if defined (IA64) || defined (HP_IA64)
/*UNIX IA64 */  /* On IA 64 Unix machines (HPUX and Linux), not the beginning of the allocated area but */
/*UNIX IA64 */  /* the top of the backing store marks the beginning of the free stack area              */
/*UNIX IA64 */  pLastUsedOfStack = (SAPDB_UInt4*)RTE_IA64GetBsp();
#    else
/*sane UNIX*/   pLastUsedOfStack = (SAPDB_UInt4*)(pTaskCtrl->StackChunkStart + SAPDB_STACK_SKIP);
#    endif
#  endif
#endif
/*ALL*/
/*ALL*/    GetCurrentStack(pMaxFillPosition);
/*ALL*/
/*ALL*/    if ( pLastUsedOfStack < pFirstUsedOfStack )
/*ALL*/    {
/*ALL*/        pStackImage = pLastUsedOfStack;
/*ALL*/        if ( maxUsed > SAPDB_STACK_SKIP
/*ALL*/          && maxUsed < stackSize )
/*ALL*/        {
/*ALL*/            pStackImage += ((stackSize - maxUsed)/sizeof(SAPDB_UInt4));
/*ALL*/        }
/*ALL*/    }
/*ALL*/    else
/*ALL*/    {
/*ALL*/        pStackImage = pMaxFillPosition;
/*ALL*/        pMaxFillPosition = pLastUsedOfStack;
/*ALL*/        if ( maxUsed > SAPDB_STACK_SKIP
/*ALL*/          && maxUsed < stackSize )
/*ALL*/        {
/*ALL*/            pMaxFillPosition -= ((stackSize - maxUsed)/sizeof(SAPDB_UInt4));
/*ALL*/        }
/*ALL*/    }
/*ALL*/
/*ALL*/    memset( pStackImage, SAPDB_STACK_PATTERN_BYTE, 
/*ALL*/              (pMaxFillPosition - pStackImage)*sizeof(SAPDB_UInt4) );
/*ALL*/}
/*ALL*/
/*------------------------------------------------------------------------------------
  Implements vsscan() for kernel tasks if called as RTE_MaximumStackUsage(0)
 */
/*ALL*/
/*ALL*/SAPDB_UInt4 RTE_MaximumStackUsage(RTE_TaskId taskId)
/*ALL*/{
/*ALL*/   SAPDB_UInt4 *pHeuristicCheck;
/*ALL*/   SAPDB_UInt4 *pStackImage;
/*ALL*/   SAPDB_UInt4 *pFirstUsedOfStack;    // top of stack
/*ALL*/   SAPDB_UInt4 *pLastUsedOfStack; // bottom of stack stck grows from top to bottom
/*ALL*/   TASK_CTRL pTaskCtrl = GetTaskControl(taskId);
/*ALL*/   SAPDB_UInt4 maximumStackUsage;
/*ALL*/   SAPDB_Int4 heuristicCounter;
/*ALL*/
#if defined(_WIN32)
/*WIN*/    pFirstUsedOfStack = (SAPDB_UInt4 *)(pTaskCtrl->Context.pStackLimit);
/*WIN*/    pLastUsedOfStack = (SAPDB_UInt4 *)(pTaskCtrl->Context.pStackBase + SAPDB_STACK_SKIP);
#else
#  ifdef HAS_INVERTED_STACK_GROW
/*UNIX*/   /* Machine stack grows from low to high addresses */
/*UNIX*/   pFirstUsedOfStack = (SAPDB_UInt4 *)(pTaskCtrl->StackChunkStart);
/*UNIX*/   pLastUsedOfStack = (SAPDB_UInt4 *)(pTaskCtrl->StackChunkEnd - SAPDB_STACK_SKIP);
#  else
/*UNIX*/   /* Machine stack grows from high to low addresses */
/*UNIX*/   pFirstUsedOfStack = (SAPDB_UInt4*)(pTaskCtrl->StackChunkEnd);
/*UNIX*/   pLastUsedOfStack = (SAPDB_UInt4*)(pTaskCtrl->StackChunkStart + SAPDB_STACK_SKIP);
#  endif
#endif
/*ALL*/   /* When offline() is called in a very early state, the task controls are not initialized. Just do nothing then.  PTS 1138752 */
/*ALL*/   if ( 0 == pFirstUsedOfStack || 0 == pLastUsedOfStack )
/*ALL*/   {
/*ALL*/       return 0;
/*ALL*/   }
/*ALL*/
/*ALL*/   if ( 0 != pTaskCtrl->maxStackPointer )
/*ALL*/   {
/*ALL*/       pStackImage = pTaskCtrl->maxStackPointer;
/*ALL*/   }
/*ALL*/   else
/*ALL*/   {
/*ALL*/       pStackImage = (SAPDB_UInt4 *)pTaskCtrl->pCurrStackPointer;
/*ALL*/   }
/*ALL*/
/*ALL*/   if ( 0 == pStackImage )   /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1139222 Since PTS 112501 it is possible that tasks never enter the dispatcher. In this case, pTaskCtrl->maxStackPointer and pTaskCtrl->pCurrStackPointer are 0 */
/*ALL*/   {
/*ALL*/       return 0;
/*ALL*/   }
/*ALL*/
/*ALL*/   if ( pFirstUsedOfStack > pLastUsedOfStack )
/*ALL*/   {
/*ALL*/       do {
/*ALL*/           while ( pStackImage > pLastUsedOfStack
/*ALL*/            && *pStackImage != SAPDB_STACK_PATTERN )
/*ALL*/           {
/*ALL*/               --pStackImage;
/*ALL*/           }
/*ALL*/
/*ALL*/           heuristicCounter = SAPDB_MAX_HEURISTIC_CHECK;
/*ALL*/           pHeuristicCheck = pStackImage;
/*ALL*/
/*ALL*/           while( heuristicCounter > 0
/*ALL*/               && pHeuristicCheck > pLastUsedOfStack )
/*ALL*/           {
/*ALL*/               if ( *pHeuristicCheck != SAPDB_STACK_PATTERN )
/*ALL*/               {
/*ALL*/                   pStackImage = pHeuristicCheck;
/*ALL*/                   break;
/*ALL*/               }
/*ALL*/               --pHeuristicCheck;
/*ALL*/               --heuristicCounter;
/*ALL*/           }
/*ALL*/
/*ALL*/           if ( pHeuristicCheck <= pLastUsedOfStack )
/*ALL*/           {
/*ALL*/               heuristicCounter = 0;
/*ALL*/           }
/*ALL*/
/*ALL*/       } while ( pStackImage > pLastUsedOfStack && heuristicCounter != 0 );
/*ALL*/
/*ALL*/       maximumStackUsage = (SAPDB_UInt4)(((SAPDB_Byte *)pFirstUsedOfStack) - (SAPDB_Byte *)pStackImage);
/*ALL*/   }
/*ALL*/   else
/*ALL*/   {
/*ALL*/       do {
/*ALL*/           while ( pStackImage < pLastUsedOfStack
/*ALL*/            && *pStackImage != SAPDB_STACK_PATTERN )
/*ALL*/           {
/*ALL*/               ++pStackImage;
/*ALL*/           }
/*ALL*/
/*ALL*/           heuristicCounter = SAPDB_MAX_HEURISTIC_CHECK;
/*ALL*/           pHeuristicCheck = pStackImage;
/*ALL*/
/*ALL*/           while( heuristicCounter > 0
/*ALL*/               && pHeuristicCheck < pLastUsedOfStack )
/*ALL*/           {
/*ALL*/               if ( *pHeuristicCheck != SAPDB_STACK_PATTERN )
/*ALL*/               {
/*ALL*/                   pStackImage = pHeuristicCheck;
/*ALL*/                   break;
/*ALL*/               }
/*ALL*/               ++pHeuristicCheck;
/*ALL*/               --heuristicCounter;
/*ALL*/           }
/*ALL*/
/*ALL*/           if ( pHeuristicCheck >= pLastUsedOfStack )
/*ALL*/           {
/*ALL*/               heuristicCounter = 0;
/*ALL*/           }
/*ALL*/
/*ALL*/       } while ( pStackImage < pLastUsedOfStack && heuristicCounter != 0 );
/*ALL*/
/*ALL*/       maximumStackUsage = (SAPDB_UInt4)(((SAPDB_Byte *)pStackImage) - (SAPDB_Byte *)pFirstUsedOfStack);
/*ALL*/   }
/*ALL*/
/*ALL*/   if ( maximumStackUsage > pTaskCtrl->totalMaxStack )
/*ALL*/   {
/*ALL*/       pTaskCtrl->totalMaxStack = maximumStackUsage;
/*ALL*/   }
/*ALL*/
/*ALL*/   return maximumStackUsage;
/*ALL*/}
/*ALL*/

//---------------------------------------------------------

/*ALL*/void RTE_ResetCommandMaxStack(RTE_TaskId taskId)
/*ALL*/{
/*ALL*/    TASK_CTRL pTaskCtrl = GetTaskControl(taskId);
/*ALL*/    pTaskCtrl->commandMaxStack = 0;
/*ALL*/}

//------------------------------------------------------------------------------------

/*ALL*/SAPDB_UInt4 RTE_GetCommandMaxStack(RTE_TaskId taskId)
/*ALL*/{
/*ALL*/    TASK_CTRL pTaskCtrl = GetTaskControl(taskId);
/*ALL*/    return pTaskCtrl->commandMaxStack;
/*ALL*/}

//------------------------------------------------------------------------------------

/*ALL*/static void GetCurrentStack(SAPDB_UInt4 * &currentStack)
/*ALL*/{
/*ALL*/    SAPDB_UInt4     wordOnCurrentStack = 0;
/*ALL*/    currentStack = &wordOnCurrentStack;
#  if defined __INTEL_COMPILER
/* From holgerH: PTS 1140067
   The RTE_ReinitializeStack method with GetCurrentStack only works, if
   'memset' does not use the stack for saving registers or whatever.
   This is the case for the 'memset' from the Intel-library used by the
   Intel compiler icc for x86_64.
   But in general, it can a problem everywhere, where 'memset' uses the stack.
   Because of this reason, we reserve an additional buffer of SAPDB_STACK_SKIP 4byte words
   (totally 4 * SAPDB_STACK_SKIP bytes) here. This works with icc on x86_64, but it is
   possible that this buffer is still too small on other systems.
 */
#  if defined HAS_INVERTED_STACK_GROW
           currentStack += SAPDB_STACK_SKIP;
#else
           currentStack -= SAPDB_STACK_SKIP;
#  endif /* HAS_INVERTED_STACK_GROW */
#  endif /* __INTEL_COMPILER */
/*ALL*/}

//---------------------------------------------------------

/*ALL*/static SAPDB_UInt4 GetCurrentStackUsage(TASK_CTRL pTaskCtrl)
/*ALL*/{
/*ALL*/           SAPDB_UInt4 currentStackUsage;
#if defined(_WIN32)
/*WIN*/           currentStackUsage = (SAPDB_UInt4)(((SAPDB_Byte *)pTaskCtrl->Context.pStackLimit) - ((SAPDB_Byte *)&pTaskCtrl));
#  if defined (_M_IA64)
/*WIN IA64 */     /* On IA64, there is a special memory area for swapping out registers used for local variables */
/*WIN IA64 */     /* called backing store. On Windows, this area starts at the top of the stack area and is as   */
/*WIN IA64 */     /* big as the stack. So, a stack overflow occurs when either the stack pointer reaches the     */
/*WIN IA64 */     /* bottom of the stack or the backing store pointer reaches the top of the backing store. To   */
/*WIN IA64 */     /* make sure that a stack overflow can be detected, the stack usage is calculated by the       */
/*WIN IA64 */     /* maximum amount of memory used in the stack area and the backing store area.                 */
/*WIN IA64 */     /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1128113 */
/*WIN IA64 */     SAPDB_UInt4 currentBackingStoreUsage = (SAPDB_UInt4)((SAPDB_Byte *)RTE_IA64GetBsp() - (SAPDB_Byte *)pTaskCtrl->Context.pStackLimit);
/*WIN IA64 */     currentStackUsage = max( currentStackUsage , currentBackingStoreUsage );
#  endif
#else /* ------------------------------------------------------------------------------- */
/*UNIX*/          SAPDB_Int4 s ;
#  ifdef HAS_INVERTED_STACK_GROW
/*UNIX*/          s = (SAPDB_Int4)( (SAPDB_Byte *) & pTaskCtrl - pTaskCtrl->StackChunkStart );
#  else
/*UNIX*/          s = (SAPDB_Int4)( pTaskCtrl->StackChunkEnd - (SAPDB_Byte *) & pTaskCtrl );
#    if defined (IA64) || defined (HP_IA64)
/*UNIX IA64 */    /* On Unix, the stack and the backing store (see comment to Windows on IA64 above) share the */
/*UNIX IA64 */    /* same memory area: The stack pointer starts at the top and grows downwards, the backing    */
/*UNIX IA64 */    /* store pointer starts at the bottom and grows upwards. So the stack usage is calculated by */
/*UNIX IA64 */    /* adding the amount of memory used by the stack and the amount of memory used by the backing*/
/*UNIX IA64 */    /* store. */
/*UNIX IA64 */    /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1128113 */
/*UNIX IA64 */    s += (SAPDB_Int4)((SAPDB_Byte *)RTE_IA64GetBsp() - pTaskCtrl->StackChunkStart);
#    endif
#  endif
/*UNIX*/          currentStackUsage = (SAPDB_UInt4) ( (s < 0) ? -s : s );
#endif
/*ALL*/           if ( currentStackUsage > pTaskCtrl->commandMaxStack )
/*ALL*/           {
/*ALL*/               pTaskCtrl->commandMaxStack = currentStackUsage;
/*ALL*/               if ( currentStackUsage > pTaskCtrl->totalMaxStack )
/*ALL*/               {
/*ALL*/                   pTaskCtrl->totalMaxStack = currentStackUsage;
/*ALL*/               }
/*ALL*/           }
/*ALL*/           return currentStackUsage;
/*ALL*/}

//---------------------------------------------------------

/*ALL*/static TASK_CTRL GetTaskControl(RTE_TaskId taskId)
/*ALL*/{
/*ALL*/           TASK_CTRL pTaskCtrl;
/*ALL*/           if ( 0 == taskId )
/*ALL*/           {
#ifdef WIN32
/*WIN32*/             PUKT_CTRL_REC pUkt = THIS_UKT_CTRL;
#ifdef SAPDB_SLOW
/*WIN*/               if ( !pUkt 
/*WIN*/                 || !KGS 
/*WIN*/                 || !KGS->pFirstUKTCtrl
/*WIN*/                 || pUkt < KGS->pFirstUKTCtrl
/*WIN*/                 || !KGS->pLastUKTCtrl
/*WIN*/                 || pUkt > KGS->pLastUKTCtrl )
/*WIN*/               {
/*WIN*/                   return 0;
/*WIN*/               }
#endif
/*WIN*/               pTaskCtrl = pUkt->pCTask;
#else
/*UNIX*/              ten50_UKT_Control * pUkt = THIS_UKT_CTRL;
#ifdef SAPDB_SLOW
/*UNIX*/              if ( !pUkt 
/*UNIX*/                 || !KGS 
/*UNIX*/                 || !KGS->first_kp 
/*UNIX*/                 || pUkt < KGS->first_kp 
/*UNIX*/                 || !KGS->last_kp 
/*UNIX*/                 || pUkt > KGS->last_kp )
/*UNIX*/              {
/*UNIX*/                  return 0;
/*UNIX*/              }
#endif
/*UNIX*/              pTaskCtrl = pUkt->curr_task;
#endif
/*ALL*/           }
/*ALL*/           else
/*ALL*/           {
/*ALL*/               pTaskCtrl = (KGS->pFirstTaskCtrl + taskId - 1);
/*ALL*/           }
/*ALL*/           return pTaskCtrl;
/*ALL*/}

//---------------------------------------------------------
/*------------------------------*/
/* Old Pascal interface         */
/*------------------------------*/

externC void vsinit ()
{
    return;
}

/*------------------------------*/

externC void vscheck ( SAPDB_Int4 &stackused)
{
    stackused = (SAPDB_Int4)RTE_CurrentStackUsage(0);
}

/*------------------------------*/

externC void vsleft ( SAPDB_Int4 &freestacksize)
{
    freestacksize = (SAPDB_Int4)RTE_CurrentStackFree(0);
}

/*------------------------------*/

externC void vsfill ()
{
    RTE_ReinitializeStack(0,0);
}

/*------------------------------*/

externC void vsscan ( SAPDB_Int4 &maxstacksize)
{
    maxstacksize = RTE_MaximumStackUsage(0);
}

/*------------------------------*/

externC void vsReinit(RTE_TaskId taskId, SAPDB_Int4 maxUsed)
{
    RTE_ReinitializeStack(taskId, maxUsed);
}

/*------------------------------*/

externC void vsGetCmdMax(RTE_TaskId taskId, SAPDB_Int4 & cmdMax)
{
    cmdMax = (SAPDB_Int4)RTE_GetCommandMaxStack(taskId);
}

/*------------------------------*/

externC void vsResetCmdMax(RTE_TaskId taskId)
{
    RTE_ResetCommandMaxStack(taskId);
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
