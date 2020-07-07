/*!---------------------------------------------------------------------
  @file           RTE_Coroutine.h
  @author         JoergM
  @brief          Kernel RunTime: Coroutine handling

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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

#   FreeBSD portions added by Kai Mosebach,
#   For more informations see : http://www.komadev.de/sapdb
#

\endif
---------------------------------------------------------------------*/
#ifndef RTE_COROUTINE_H
#define RTE_COROUTINE_H

#include "SAPDBCommon/SAPDB_Types.h"
#include "RunTime/RTE_Types.h"
#include "RunTime/RTE_CompilerFeatures.h"

#if defined LINUX || defined FREEBSD
#define RTE_MAXIMUM_LIBC_SPECIFIC_KEYS 8
#endif

/* ffs. */
/* #define SAPDB_WITH_EXCEPTION_SAFE_COROUTINES 1 */
/*!
  This definition is used for conditional compiling exception safe coroutine support.
  The problem with 'Coroutine local storage' instead of 'thread local storage' is not
  completly solved. Therefore the corresponding code is still disabled by '0'.
*/
#if defined(SUN) || defined(SOLARIS)
#    define SAPDB_WITH_EXCEPTION_SAFE_COROUTINES 1
#else
#    define SAPDB_WITH_EXCEPTION_SAFE_COROUTINES 0
#endif

/*===================================================================*/
/*  OS/Machine dependend context definitions                         */
/*===================================================================*/

/*
 * If this is defined the ucontext_t defined by the OS is used.
 * If not a self defined ucontext_t is used.
 */
#undef USE_UCONTEXT                  /* Will be setup in this file... */

/*
 * If this is defined the ucontext_t is replaced be an own context containing only stack pointer
 * and start address.
 */
#undef USE_OWN_COROUTINE_CONTEXT   /* Will be setup alternative in this file... */

/*
 * If this is defined the OS makecontext() call uses STACK_BASE address
 * instead of STACK_TOP address for initial stack setup.
 * If not defined STACK_TOP is used.
 */
#undef MAKECONTEXT_USES_STACK_BASE   /* Will be setup in this file... */

/*
 * If this is defined the OS makecontext() call uses ARGUMENT_COUNT
 * as bias to number of arguments passed to makecontext call
 */
#undef MAKECONTEXT_ZERO_ARGUMENT_COUNT    /* Will be setup in this file... */

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(LINUX)
#  include <ucontext.h>
#  define USE_UCONTEXT LINUX
#if defined (I386)
#  include "RunTime/RTE_SystemLinux.h"
#  define USE_OWN_COROUTINE_CONTEXT
#endif
#endif /* LINUX */

#if defined(ALPHA)
#  include <ucontext.h>
#  define USE_UCONTEXT DEC
#endif  /* ALPHA*/

#if defined(PA20W) || defined (HP_IA64)
#  include <ucontext.h>
/*    USE_UCONTEXT failed completely..... bug in libc on HPUX? But it compiles and maybe some day... */
#  define USE_UCONTEXT HPUX

#  if !defined (HP_IA64)
#  include "RunTime/RTE_SystemPA.h" /* nocheck */
#  define USE_OWN_COROUTINE_CONTEXT
#endif
#endif  /* PA20W || HP_IA64 */

#if defined(_IBMR2)
#  include <ucontext.h>
#  define USE_UCONTEXT AIX
# endif /* _IBMR2 */

#if (defined(SUN) && defined(SVR4)) || defined(SOLARIS)
/* This selects the new makecontext interface, but still allows compatibility test for Solaris8 code... */
#  if defined(__SunOS_5_9) && !defined(TEST_FOR_SUN_SOLARIS8)
#    define __MAKECONTEXT_V2_SOURCE
#  endif
#  include <ucontext.h>
#  define USE_UCONTEXT SUN
#endif  /*SUN && SVR4 || SOLARIS */

#if defined(__cplusplus)
}; /* extern "C" */
#endif

/*!
   Extended coroutine context containing hooks for coroutine local storage
 */
struct RTE_CoroutineContext
{
    /*! coroutine context container union */
    union contextUnion
    {
#if defined(USE_UCONTEXT)
        ucontext_t uC; /*!< context used by getcotent/makecontext/swapcontext calls */
#endif
#if defined(USE_OWN_COROUTINE_CONTEXT)
        struct RTE_OwnCoroutineContext ownC; /*!< own context used by assembly code */
#endif
    } context;

#if SAPDB_WITH_EXCEPTION_SAFE_COROUTINES > 0
#if defined (LINUX) || defined (FREEBSD)
    int         localErrno; /*!< LINUX gcc libc internal coroutine local errno */
    int         localHErrno;/*!< LINUX gcc libc internal coroutine local herrno */
    void     *  libcSpecificTlsKeys[RTE_MAXIMUM_LIBC_SPECIFIC_KEYS]; /*!< LINUX gcc libc internal keys. Currently only three really needed... */
#endif /* LINUX */
    void    *** tlsKeys; /*!< pointer to array of coroutine local storage keys */
#endif
    void *allocatedMemory; /*!< pointer to memory allocated for RTE_CoroutineContext */
    void *stackStart;
    SAPDB_ULong stackSize;
};

/*!--------------------------------------------------------------------
   @brief Return size needed for context, to allow dynamically allocated contexts
   @return context size in bytes
 --------------------------------------------------------------------*/
externC SAPDB_UInt4 RTE_ContextSize();

/*!--------------------------------------------------------------------
   @brief Return an dynamically allocated context

   The size of a context is not enaugh... i.e. on IA64 Machines the context itself must be 16 bytes aligned...

   @param allocator [in] allocator to use

   @return pointer to context or 0 if no memory
 --------------------------------------------------------------------*/
#if defined(__cplusplus)
class SAPDBMem_IRawAllocator;
externC struct RTE_CoroutineContext *RTE_ContextAllocate(SAPDBMem_IRawAllocator &allocator);

/*!--------------------------------------------------------------------
   @brief Free an dynamically allocated context

   This uses the 'allocatedMemory' pointer inside the context to free it
   @param pContext [in] context to free
   @param allocator [in] allocator to use
   @return none
 --------------------------------------------------------------------*/
externC void RTE_ContextFree(struct RTE_CoroutineContext *pContext, SAPDBMem_IRawAllocator &allocator);
#endif

/*!--------------------------------------------------------------------
   @brief Switch from one coroutine context to another

   This call actually returns after the chain of coroutines finally decidded
   to switch back to the 'oldContext'. To allow this it stores the current
   registers and stack context into oldContext, and retrieve the registers
   and stack context from newContext. It the 'returns' into the new context.

   @param pOldContext [out] current context
   @param pNewContext [out] new context
   @return true is context switched, false if not (stack overflow?)
 --------------------------------------------------------------------*/
externC SAPDB_Bool RTE_ContextSwitch( struct RTE_CoroutineContext VAR_VALUE_REF pOldContext
                                    , struct RTE_CoroutineContext VAR_VALUE_REF pNewContext );

/*!--------------------------------------------------------------------
   @brief Prepare a context used for context switch
   @param pContext [out]       context to setup
   @param stackPtr [in]        address of first byte of stack
   @param stackSize [in]       total size of stack area
   @param initialFunction [in] initial function to call
   @return none
 --------------------------------------------------------------------*/
externC SAPDB_Bool RTE_ContextPrepare( struct RTE_CoroutineContext VAR_VALUE_REF pContext
                                      ,void *                                    stackPtr
                                      ,SAPDB_ULong                               stackSize
                                      ,void *                                    initialFunction );

/*!--------------------------------------------------------------------
   @brief Prepare a context used for context switch with additional arguement for initialFunction
   @param pContext [out]       context to setup
   @param stackPtr [in]        address of first byte of stack
   @param stackSize [in]       total size of stack area
   @param initialFunction [in] initial function to call
   @return none
 --------------------------------------------------------------------*/
externC SAPDB_Bool RTE_ContextPrepareWithArgument( struct RTE_CoroutineContext VAR_VALUE_REF pContext
                                                  ,void *                                    stackPtr
                                                  ,SAPDB_ULong                               stackSize
                                                  ,void *                                    initialFunction
                                                  ,SAPDB_Int4                                argument );

/*!--------------------------------------------------------------------
   @brief Free task-local variables (used in exception-safe coroutines)
   @param pContext [in]       context to clean up
   @return none
 --------------------------------------------------------------------*/
externC void RTE_ContextFreeTLS( struct RTE_CoroutineContext VAR_VALUE_REF pContext );

#endif /* RTE_COROUTINE_H */
