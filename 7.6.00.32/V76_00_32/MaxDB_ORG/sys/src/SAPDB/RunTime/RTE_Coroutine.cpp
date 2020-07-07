/*!---------------------------------------------------------------------
  @file           RTE_Coroutine.cpp
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


//
//   FreeBSD portions added by Kai Mosebach,
//   For more informations see : http://www.komadev.de/sapdb
//

\endif
---------------------------------------------------------------------*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#ifdef FREEBSD
#include <pthread.h>
#endif
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "RunTime/RTE_Coroutine.h"
#include "RunTime/System/RTESys_AtomicOperation.hpp"
#include "geo002.h"
#define USE_KGS
#include "geo50_0.h"
#ifdef DEBUG_RTE
#include "SAPDBCommon/SAPDB_sprintf.h" /*nocheck*/
#include "geo007_1.h" /*nocheck*/
#include "gsp00.h" /*nocheck*/
#include "geo60.h" /*nocheck*/
#endif

extern RTE_TaskId RTE_GetOwnTaskId();
extern RTE_CoroutineContext *RTE_GetTaskContext(RTE_TaskId id);

extern "C" {
#if defined(LINUX)
#  define MAKECONTEXT_USES_STACK_BASE
#  define MAKECONTEXT_ZERO_ARGUMENT_COUNT 0
typedef void * stackPointerType;
#endif /* LINUX */

#if defined(ALPHA) || defined (FREEBSD)
#  define MAKECONTEXT_USES_STACK_BASE
#  define MAKECONTEXT_ZERO_ARGUMENT_COUNT 0
typedef char * stackPointerType;
#endif  /* ALPHA*/

#if defined(PA20W)
#  define MAKECONTEXT_USES_STACK_BASE
#  define MAKECONTEXT_ZERO_ARGUMENT_COUNT 0
typedef void * stackPointerType;
#endif  /* PA20W */

#if defined(HP_IA64)
#  define  MAKECONTEXT_USES_STACK_BASE
#  define MAKECONTEXT_ZERO_ARGUMENT_COUNT 0
typedef void * stackPointerType;
#endif  /* HP_IA64 */

#if defined(_IBMR2)
#  define MAKECONTEXT_USES_STACK_BASE
#  define MAKECONTEXT_ZERO_ARGUMENT_COUNT 0
typedef void * stackPointerType;
# endif /* _IBMR2 */

#if (defined(SUN) && defined(SVR4)) || defined(SOLARIS)
# if defined(__SunOS_5_8) || defined(TEST_FOR_SUN_SOLARIS8)
#  undef  MAKECONTEXT_USES_STACK_BASE
#  define MAKECONTEXT_ZERO_ARGUMENT_COUNT 1
# else
#  define MAKECONTEXT_USES_STACK_BASE
#  define MAKECONTEXT_ZERO_ARGUMENT_COUNT 0
# endif
typedef void * stackPointerType;
#endif  /*SUN && SVR4 || SOLARIS */
}; /* extern "C" */

#if SAPDB_WITH_EXCEPTION_SAFE_COROUTINES > 0
#define THREAD_KEYS_FIRST_LEVEL  8
#define THREAD_KEYS_SECOND_LEVEL 32

#endif


extern "C" void en88memcpy(void *a,void *b,SAPDB_UInt s); ///< module external memcpy call

/*!--------------------------------------------------------------------
   @brief Switch from one coroutine context to another

   This call actually returns after the chain of coroutines finally decidded
   to switch back to the 'oldContext'. To allow this it stores the current
   registers and stack context into oldContext, and retrieve the registers
   and stack context from newContext. It the 'returns' into the new context.

   @param pOld [out] current context
   @param pNew [out] new context
   @return none
 --------------------------------------------------------------------*/
extern "C" SAPDB_Bool RTE_ContextSwitch( RTE_CoroutineContext & oldContext
                                       , RTE_CoroutineContext & newContext )
{
#ifdef DEBUG_RTE
char eBuffer[256];
SAPDB_sprintf(eBuffer, sizeof(eBuffer), "Switching from 0x%p to 0x%p\n",
              &oldContext.context,
              &newContext.context);
MSGD(( INFO_TEST_WILDCARD, eBuffer ));
#endif

    if ( XPARAM(fUseUcontext) )
    {
#ifdef HP_IA64
/* This is a workaround for broken swapcontect() on HPUX IA64. During the swapcontext() call the page-fault- */
/* handling is deactivated, so that ucontext structures that are paged out cannot be accessed. In such cases */
/* the function fails and errno is EFAULT. if this is detected, the ucontext structures are copied (just to  */
/* make sure they are paged into main memory) and swapcontext() is called again. This is done in a loop      */
/* because the ucontexts may be paged out again just before the swapcontext() call. */
/* HP has announced a patch for this problem for January 2005, this workaround code may be eliminated then...*/
        int retval;
        ucontext_t dummyContext;
        while( ((retval = swapcontext(&oldContext.context.uC, &newContext.context.uC)) != 0) && (errno == EFAULT) )
        {
            en88memcpy(&dummyContext,&oldContext.context.uC,sizeof(ucontext_t));
            en88memcpy(&dummyContext,&newContext.context.uC,sizeof(ucontext_t));
        }
        return ( 0 == retval );
#else
        return ( swapcontext(&oldContext.context.uC, &newContext.context.uC) == 0 );
#endif
    }
    else
    {
#if defined(USE_OWN_COROUTINE_CONTEXT)
        RTE_OwnSwapContext(&oldContext.context.ownC, &newContext.context.ownC);
        return true;
#else
        return false;
#endif
    }
}

/*!--------------------------------------------------------------------
   @brief Dynamically allocate the context
   @param The allocator that provides the memory
   @return context if allocation succeeded, 0 if it failed
 --------------------------------------------------------------------*/
extern "C" SAPDB_UInt4 RTE_ContextSize()
{
    return (SAPDB_UInt4)sizeof(RTE_CoroutineContext);
}

/*!--------------------------------------------------------------------
   @brief Dynamically allocate the context
   @param The allocator that provides the memory
   @return context if allocation succeeded, 0 if it failed
 --------------------------------------------------------------------*/
extern "C" RTE_CoroutineContext * RTE_ContextAllocate(SAPDBMem_IRawAllocator &allocator)
{
    void *allocatedMemory = allocator.Allocate(RTE_ContextSize()+16);

    if ( !allocatedMemory )
    {
        return 0;
    }

    RTE_CoroutineContext *pContext = (RTE_CoroutineContext *)
        (((SAPDB_Byte *)0) + ( ( ((SAPDB_ULong) (((SAPDB_Byte *)allocatedMemory) - ((SAPDB_Byte *)0))) + 15 ) & ~15 ));

    pContext->allocatedMemory = allocatedMemory;
    pContext->stackStart = 0;
    pContext->stackSize = 0;
    return pContext;
}

/*!--------------------------------------------------------------------
   @brief Dynamically allocate the context
   @param The allocator that provides the memory
   @return context if allocation succeeded, 0 if it failed
 --------------------------------------------------------------------*/
extern "C" void RTE_ContextFree(RTE_CoroutineContext *pContext, SAPDBMem_IRawAllocator &allocator)
{
    if ( pContext )
    {
        allocator.Deallocate(pContext->allocatedMemory);
    }
}

/*--------------------------------------------------------------------*/

extern "C" {
    /*! Converter needed since C++ does not like to convert function pointer */
    /*! this wail fail if the size of a function pointer does not match size of void *... */
    union callPointerConverter {
        void *anyPointer;              ///< input pointer
        void (*initialContextCall)();  ///< output function call pointer
    };
};

/*!--------------------------------------------------------------------
   @brief Prepare a context used for context switch
   @param pContext [out]       context to setup
   @param stackPtr [in]        address of first byte of stack
   @param stackSize [in]       total size of stack area
   @param initialFunction [in] initial function to call
   @return none
 --------------------------------------------------------------------*/
SAPDB_Bool RTE_ContextWithOptionalArgumentPrepare( RTE_CoroutineContext &    GivenContextPointer
                                                 , void *                    stackPtr
                                                 , SAPDB_ULong               stackSize
                                                 , void *                    initialFunction
                                                 , SAPDB_Bool                wantArgument
                                                 , SAPDB_Int4                argument )
{
    GivenContextPointer.stackStart = stackPtr;
    GivenContextPointer.stackSize = stackSize;
#if SAPDB_WITH_EXCEPTION_SAFE_COROUTINES > 0
#ifdef LINUX
    GivenContextPointer.localErrno = 0;
    GivenContextPointer.localHErrno = 0;
    memset(GivenContextPointer.libcSpecificTlsKeys, 0, RTE_MAXIMUM_LIBC_SPECIFIC_KEYS * sizeof(void*));
#endif /* LINUX */
    GivenContextPointer.tlsKeys = NULL;
#endif

    callPointerConverter u;
    u.anyPointer = initialFunction; // C++ does not allow to cast from void * to void (*)()
    if ( XPARAM(fUseUcontext) )
    {
        int rc;
        ucontext_t *pContext = &GivenContextPointer.context.uC;

        /* on AIX we would otherwise get a floating point exception sometimes....*/
        memset(pContext, 0, sizeof(ucontext_t));

        rc = getcontext ( pContext );
        if ( -1 == rc )
        {
            return false;
        }

        if ( !stackPtr
          || !stackSize
          || !initialFunction )
        {
            return true; // just want to get own context
        }

        pContext->uc_link = 0;
#ifdef MAKECONTEXT_USES_STACK_BASE
#ifndef FREEBSD
        pContext->uc_stack.ss_sp    = (stackPointerType)stackPtr;
#else
        pContext->uc_stack.ss_sp    = (char*)stackPtr;
#endif //FREEBSD
#else
        pContext->uc_stack.ss_sp    = (stackPointerType)(((char *)stackPtr) + stackSize);
#endif
        pContext->uc_stack.ss_size  = stackSize;
        pContext->uc_stack.ss_flags = 0;

#if (defined(SUN) && defined(SVR4)) || defined(SOLARIS)
#  if defined(__SunOS_5_8)  || defined(TEST_FOR_SUN_SOLARIS8)
#    if defined(__sparc) || defined(__sparcv9)
        // Fix only needed on Solaris8: Sparc needs 16 byte aligned stacks!
        pContext->uc_stack.ss_sp = ((char *)pContext->uc_stack.ss_sp) - 16;
        pContext->uc_stack.ss_size -= 16;
#    else
// we assume that makecontext () returns a valid stack ChristophB,DanielD
#    endif
#  endif
#endif
        if ( wantArgument )
        {
            makecontext ( pContext, u.initialContextCall, MAKECONTEXT_ZERO_ARGUMENT_COUNT + 1, argument);
        }
        else
        {
            makecontext ( pContext, u.initialContextCall, MAKECONTEXT_ZERO_ARGUMENT_COUNT);
        }

#if defined(LINUX) && defined(X86_64)
		// fix machine context stack pointer
		// caused by broken makecontext ()
		// according the x86-64 abi (%rsp - 8) must be a multiple of 16
#ifndef REG_RSP
#define REG_RSP 15
#endif
		unsigned long sp = pContext->uc_mcontext.gregs[REG_RSP];
		if ((sp & (unsigned long) 15) != (unsigned long) 8) {
			sp &= ~(unsigned long) 15;
			sp -= 8;
	        pContext->uc_mcontext.gregs[REG_RSP] = sp;
		}
#endif

        return true;
    }
    else
    {
#if defined(USE_OWN_COROUTINE_CONTEXT)
        if ( !stackPtr
          || !stackSize
          || !initialFunction )
        {
            return true; // just want to get own context. Not needed with this implementation
        }

        if ( wantArgument )
        {
            return false; // not supported (yet???)
        }

#ifdef PA20W
        GivenContextPointer.context.ownC.sp   = stackPtr;
#else
        GivenContextPointer.context.ownC.sp   = (char*)stackPtr + stackSize;
#endif
        GivenContextPointer.context.ownC.addr = u.initialContextCall;
        RTE_OwnMakeContext( &GivenContextPointer.context.ownC );

        return true;
#else
        return false;
#endif /* USE_UCONTEXT */
    }
}

/*!--------------------------------------------------------------------
   @brief Prepare a context used for context switch
   @param pContext [out]       context to setup
   @param stackPtr [in]        address of first byte of stack
   @param stackSize [in]       total size of stack area
   @param initialFunction [in] initial function to call
   @return none
 --------------------------------------------------------------------*/
externC SAPDB_Bool RTE_ContextPrepareWithArgument( RTE_CoroutineContext &    GivenContextPointer
                                         , void *                    stackPtr
                                         , SAPDB_ULong               stackSize
                                         , void *                    initialFunction
                                         , SAPDB_Int4                argument )
{
    return RTE_ContextWithOptionalArgumentPrepare( GivenContextPointer
                                                 , stackPtr
                                                 , stackSize
                                                 , initialFunction
                                                 , true
                                                 , argument );
}

/*!--------------------------------------------------------------------
   @brief Prepare a context used for context switch
   @param pContext [out]       context to setup
   @param stackPtr [in]        address of first byte of stack
   @param stackSize [in]       total size of stack area
   @param initialFunction [in] initial function to call
   @return none
 --------------------------------------------------------------------*/
externC SAPDB_Bool RTE_ContextPrepare( RTE_CoroutineContext &    GivenContextPointer
                              ,void *                    stackPtr
                              ,SAPDB_ULong               stackSize
                              ,void *                    initialFunction )
{
    return RTE_ContextWithOptionalArgumentPrepare( GivenContextPointer
                                                 , stackPtr
                                                 , stackSize
                                                 , initialFunction
                                                 , false
                                                 , 0 );
}

/*============================================*/
/*============================================*/
/*============================================*/
#if SAPDB_WITH_EXCEPTION_SAFE_COROUTINES > 0
/*============================================*/
/*============================================*/
/*============================================*/

/*!--------------------------------------------------------------------
   Overloading needed for thread and exception savety
 */
typedef void (*RTE_TLSCleanupFunc)(void *arg);

static SAPDB_Int4 RTE_TLSKeys_Locked = 0;

typedef enum {
  SYSTEM_UNKNOWN,
  SYSTEM_LIBC,
  SYSTEM_USER
} RTE_TLSSystem;

struct RTE_pthread_tls_key
{
  int inUse;
  RTE_TLSCleanupFunc pCleanup;
  RTE_TLSSystem system;
} RTE_TLSKeys[THREAD_KEYS_FIRST_LEVEL*THREAD_KEYS_SECOND_LEVEL] = { 0 };

int       RTE_Coroutine_tlsGetAlt(int key, void * &val);
int       RTE_Coroutine_tlsSetAlt(int key, void * val);
int       RTE_Coroutine_tlsCreateAlt(int &key, RTE_TLSCleanupFunc pCleanup, bool usekey);
int       RTE_Coroutine_tlsDestroyAlt(int key);

//-----------------------------------------------------

int RTE_Coroutine_tlsCreateAlt(int &key, RTE_TLSCleanupFunc pCleanup, bool usekey)
{
    int iKey;
    SAPDB_Int4 oldValue;
    while ( !RTESys_CompareAndExchange(RTE_TLSKeys_Locked, 0, 1, oldValue) )
        ;

    if (usekey) {
      iKey = key - 1;
      if ( !RTE_TLSKeys[iKey].inUse )
      {
        // use specified key
        RTE_TLSKeys[iKey].inUse = 1;
        RTE_TLSKeys[iKey].pCleanup = pCleanup;
        RTE_TLSKeys[iKey].system = SYSTEM_UNKNOWN;
      } else {
        // already used
        iKey = (THREAD_KEYS_FIRST_LEVEL*THREAD_KEYS_SECOND_LEVEL);
      }
    } else {
      for ( iKey = 0; iKey < (THREAD_KEYS_FIRST_LEVEL*THREAD_KEYS_SECOND_LEVEL); iKey++ )
      {
        if ( !RTE_TLSKeys[iKey].inUse )
        {
          RTE_TLSKeys[iKey].inUse = 1;
          RTE_TLSKeys[iKey].pCleanup = pCleanup;
          RTE_TLSKeys[iKey].system = SYSTEM_UNKNOWN;
          break;
        }
      }
    }

    RTESys_CompareAndExchange(RTE_TLSKeys_Locked, 1, 0, oldValue);

    if ( iKey == (THREAD_KEYS_FIRST_LEVEL*THREAD_KEYS_SECOND_LEVEL) )
        return EAGAIN;

    key = iKey + 1;

    return 0;
}

//-----------------------------------------------------

int RTE_Coroutine_tlsDestroyAlt(int key)
{
    RTE_TLSCleanupFunc pCleanup;
    SAPDB_Int4 oldValue;
    int result;

    if ( key < 1
      || key > (THREAD_KEYS_FIRST_LEVEL*THREAD_KEYS_SECOND_LEVEL) )
    {
        return EINVAL;
    }

    while ( !RTESys_CompareAndExchange(RTE_TLSKeys_Locked, 0, 1, oldValue) )
        ;

    if ( RTE_TLSKeys[key].inUse == 1 )
    {
        pCleanup = RTE_TLSKeys[key].pCleanup;
        RTE_TLSKeys[key].pCleanup = 0;
        if ( !pCleanup )
        {
            RTE_TLSKeys[key].inUse = 0;
        }
        else
        {
            RTE_TLSKeys[key].inUse = 2;
        }
        result = 0;
    }
    else
    {
        pCleanup = 0;
        result = EINVAL;
    }

    RTESys_CompareAndExchange(RTE_TLSKeys_Locked, 1, 0, oldValue);

    if ( pCleanup )
    {
        // XXX silently ignored for now... Must call cleanup for all threads and all tasks where value != 0...

        // finally reset inUse
        while ( !RTESys_CompareAndExchange(RTE_TLSKeys_Locked, 0, 1, oldValue) )
            ;

        RTE_TLSKeys[key].inUse = 0;

        RTESys_CompareAndExchange(RTE_TLSKeys_Locked, 1, 0, oldValue);
    }

    return result;
}

//-----------------------------------------------------

extern "C" void ****sqlGetTlsArray();
int RTE_Coroutine_tlsGetAlt(int key, void *&value)
{
    if ( key < 1
      || key > (THREAD_KEYS_FIRST_LEVEL*THREAD_KEYS_SECOND_LEVEL)
      || 1 != RTE_TLSKeys[key-1].inUse )
        return EINVAL;

    if (RTE_TLSKeys[key-1].system == SYSTEM_UNKNOWN) {
      RTE_TLSKeys[key-1].system = SYSTEM_USER;
    } else if (RTE_TLSKeys[key-1].system != SYSTEM_USER) {
      return 1; // retry with system
    }

    void ***tlsKeys;
    RTE_TaskId id = RTE_GetOwnTaskId();
    if ( id == RTE_UnknownTaskId )
    {
        void ****pTlsKeys = sqlGetTlsArray();
        if ( !pTlsKeys )
        {
            value = (void *)0;
            return 0;
        }
        tlsKeys = *pTlsKeys;
    }
    else
    {
        tlsKeys = RTE_GetTaskContext(id)->tlsKeys;
    }

    if ( !tlsKeys )
    {
        value = (void *)0;
        return 0;
    }

    void **firstLevel = tlsKeys[(key-1)&(THREAD_KEYS_FIRST_LEVEL-1)];
    if ( !firstLevel )
    {
        value = (void *)0;
        return 0;
    }

    value = firstLevel[(key-1)/THREAD_KEYS_FIRST_LEVEL];

    return 0;
}

//-----------------------------------------------------

int RTE_Coroutine_tlsSetAlt(int key, void *value)
{
    if ( key < 1
      || key > (THREAD_KEYS_FIRST_LEVEL*THREAD_KEYS_SECOND_LEVEL)
      || 1 != RTE_TLSKeys[key-1].inUse )
        return EINVAL;

    if (RTE_TLSKeys[key-1].system == SYSTEM_UNKNOWN) {
      RTE_TLSKeys[key-1].system = SYSTEM_USER;
    } else if (RTE_TLSKeys[key-1].system != SYSTEM_USER) {
      return 1; // retry with system
    }

    void ****pTlsKeys;
    RTE_TaskId id = RTE_GetOwnTaskId();
    if ( id == RTE_UnknownTaskId )
    {
        pTlsKeys = sqlGetTlsArray();
    }
    else
    {
        pTlsKeys = &(RTE_GetTaskContext(id)->tlsKeys);
    }

    if ( !*pTlsKeys )
    {
        *pTlsKeys = (void ***)malloc(sizeof(void **) * THREAD_KEYS_FIRST_LEVEL);
        if ( !*pTlsKeys )
            return EAGAIN;
        memset(*pTlsKeys, 0, sizeof(void **) * THREAD_KEYS_FIRST_LEVEL);
    }

    void ***pFirstLevel = &((*pTlsKeys)[(key-1)&(THREAD_KEYS_FIRST_LEVEL-1)]);
    if ( !*pFirstLevel )
    {
        *pFirstLevel = (void **)malloc(sizeof(void *) * THREAD_KEYS_SECOND_LEVEL);
        if ( !*pFirstLevel )
            return EAGAIN;
        memset(*pFirstLevel, 0, sizeof(void *) * THREAD_KEYS_SECOND_LEVEL);
    }

    (*pFirstLevel)[(key-1)/THREAD_KEYS_FIRST_LEVEL] = value;

    return 0;
}

#if (defined(SUN) && defined(SVR4)) || defined(SOLARIS)
/************************************************************************
 * SUN THREAD API BASED THREADING                                       *
 ************************************************************************/

#include <thread.h>

// overrides
extern "C" {
    typedef void (*__thr_cleanup_fnc)(void *);

    // original functions
    extern int _thr_keycreate(thread_key_t *key, __thr_cleanup_fnc cleanup);
    extern int _thr_getspecific(thread_key_t key, void **val);
    extern int _thr_setspecific(thread_key_t key, void *arg);

    static bool thr_coroutines_inited = false;

    static void thr_initcoroutines()
    {
      // lock the spinlock
      SAPDB_Int4 oldValue;
      while ( !RTESys_CompareAndExchange(RTE_TLSKeys_Locked, 0, 1, oldValue) )
        ;
      thr_coroutines_inited = true;

      for ( int iKey = 0; iKey < (THREAD_KEYS_FIRST_LEVEL*THREAD_KEYS_SECOND_LEVEL); iKey++ )
      {
        if ( RTE_TLSKeys[iKey].inUse && RTE_TLSKeys[iKey].system == SYSTEM_LIBC )
        {
          // convert to user keys
          RTE_TLSKeys[iKey].system = SYSTEM_USER;
          void *val;
          int res = _thr_getspecific(iKey + 1, &val);
          if (res == 0) {
            RTE_Coroutine_tlsSetAlt(iKey + 1, val);
          }
        }
      }

      RTESys_CompareAndExchange(RTE_TLSKeys_Locked, 1, 0, oldValue);
    }

    extern int thr_getspecific(thread_key_t key, void **val)
    {
      int res;
      if (kgs && kgs->XParam && XPARAM(fUseCoroutines)) {
        if (!thr_coroutines_inited) {
          thr_initcoroutines();
        }
        res = RTE_Coroutine_tlsGetAlt(key, *val);
        if (res > 0) {
          // must use system routines
          res = _thr_getspecific(key, val);
        }
      } else {
        res = _thr_getspecific(key, val);
      }

      return res;
    }

    extern int thr_setspecific(thread_key_t key, void *arg)
    {
      if (kgs && kgs->XParam && XPARAM(fUseCoroutines)) {
        if (!thr_coroutines_inited) {
          thr_initcoroutines();
        }
        int res = RTE_Coroutine_tlsSetAlt(key, const_cast<void*>(arg));
        if (res > 0) {
          return _thr_setspecific(key, arg);
        }
        return res;
      } else {
        if (key >= 1 && key <= (THREAD_KEYS_FIRST_LEVEL*THREAD_KEYS_SECOND_LEVEL)) {
          // must use system routines forever
          RTE_TLSKeys[key-1].system = SYSTEM_LIBC;
        }
        return _thr_setspecific(key, arg);
      }
    }

    extern int thr_keycreate(thread_key_t *key, __thr_cleanup_fnc cleanup)
    {
      // create system-specific key (we don't know yet if we use coroutines)
      int res = _thr_keycreate(key, cleanup);
      if (res != 0) {
        return res;
      }

      // create our local record as well
      return RTE_Coroutine_tlsCreateAlt(*(int*)key, (RTE_TLSCleanupFunc) cleanup, true);
    }

/*
        // it seems that Sun doesn't need overriding mutexes, they work
        // independent of thread stack pointer

        extern int mutex_init(mutex_t *mutex, int type, void *arg)
        {
                reinterpret_cast<RTE_Mutex&>(mutex->data).reset();
                mutex->flags.flag[0] = 1;
                return 0;
        }

        extern int mutex_trylock(mutex_t *mutex)
        {
                if (!mutex->flags.flag[0]) {
                        mutex_init(mutex, 0, NULL);
                }
                if (reinterpret_cast<RTE_Mutex&>(mutex->data).trylock()) {
                        return 0;
                } else {
                        return EBUSY;
                }
        }

        extern int mutex_lock(mutex_t *mutex)
        {
                if (!mutex->flags.flag[0]) {
                        mutex_init(mutex, 0, NULL);
                }
                reinterpret_cast<RTE_Mutex&>(mutex->data).lock();
                return 0;
        }

        extern int mutex_unlock(mutex_t *mutex)
        {
                reinterpret_cast<RTE_Mutex&>(mutex->data).unlock();
                return 0;
        }

        extern int mutex_destroy(mutex_t *mutex)
        {
                if (reinterpret_cast<RTE_Mutex&>(mutex->data).trylock()) {
                        return 0;
                } else {
                        return EBUSY;
                }
        }
*/
};

#elif defined(AIX)

#include <pthread.h>

// overrides
extern "C" {
  typedef void (*__thr_cleanup_fnc)(void *);

  extern void *pthread_getspecific(pthread_key_t key)
  {
    void *val;
    RTE_Coroutine_tlsGetAlt(key, val);
    return val;
  }

  extern int pthread_setspecific(pthread_key_t key, const void *arg)
  {
    return RTE_Coroutine_tlsSetAlt(key, const_cast<void*>(arg));
  }

  extern int pthread_key_create(pthread_key_t *key, __thr_cleanup_fnc cleanup)
  {
    return RTE_Coroutine_tlsCreateAlt(*(int*)key, (RTE_TLSCleanupFunc) cleanup, false);
  }
}

#endif /* platform */

//-----------------------------------------------------
#ifdef LINUX
#if 0         // quick fix - this doesn't work yet
//-----------------------------------------------------
/*LINUX*/
/*LINUX*/int RTE_Coroutines_libcTlsSet(RTE_TaskId id, int key, void*value)
/*LINUX*/{
/*LINUX*/    if ( key < 0 || key >= RTE_MAXIMUM_LIBC_SPECIFIC_KEYS )
/*LINUX*/    {
/*LINUX*/        return EINVAL;
/*LINUX*/    }
/*LINUX*/    // TODO: the keys are not necessarily already allocated!
/*LINUX*/    RTE_GetTaskContext(id)->libcSpecificTlsKeys[key] = value;
/*LINUX*/}
/*LINUX*/
/*LINUX*///-----------------------------------------------------
/*LINUX*/
/*LINUX*/void * RTE_Coroutines_libcTlsGet(RTE_TaskId id, int key)
/*LINUX*/{
/*LINUX*/    if ( key < 0 || key >= RTE_MAXIMUM_LIBC_SPECIFIC_KEYS )
/*LINUX*/    {
/*LINUX*/        return (void *)0;
/*LINUX*/    }
/*LINUX*/
/*LINUX*/    return RTE_GetTaskContext(id)->libcSpecificTlsKeys[key];
/*LINUX*/}
/*LINUX*/
/*LINUX*///-----------------------------------------------------
/*LINUX*/
/*LINUX*/int * RTE_Coroutine_ErrnoLocation(RTE_TaskId id)
/*LINUX*/{
/*LINUX*/    return &RTE_GetTaskContext(id)->localErrno;
/*LINUX*/}
/*LINUX*/
/*LINUX*///-----------------------------------------------------
/*LINUX*/
/*LINUX*/int * RTE_Coroutine_HErrnoLocation(RTE_TaskId id)
/*LINUX*/{
/*LINUX*/    return &RTE_GetTaskContext(id)->localHErrno;
/*LINUX*/}
/*LINUX*/
/*LINUX*//************************************************************************/
/*LINUX*//* POSIX THREAD API BASED THREADING                                     */
/*LINUX*//************************************************************************/
/*LINUX*/
#include <pthread.h>
pthread_t RTE_Coroutine_pthreadSelf(RTE_TaskId id);
int *     RTE_Coroutine_ErrnoLocation(RTE_TaskId id);
int *     RTE_Coroutine_HErrnoLocation(RTE_TaskId id);
int       RTE_Coroutines_libcTlsSet(RTE_TaskId id, int key, void* value);
void *    RTE_Coroutines_libcTlsGet(RTE_TaskId id, int key);
/*LINUX*/
/*LINUX*/// overrides
/*LINUX*/extern "C" {
/*LINUX*/
#define ALIAS_FNC(fncname) _ALIAS_FNC(fncname, __##fncname)
#define _ALIAS_FNC(fncname, aliasname) extern __typeof (fncname) aliasname __attribute__((alias(#fncname)))
#define ALIAS_FNC_LIBC(fncname) void (*__##fncname)() = (void(*)()) fncname;
/*LINUX*/
/*LINUX*/    extern void * pthread_getspecific(pthread_key_t key)
/*LINUX*/    {
/*LINUX*/        void *val;
/*LINUX*/        return ( RTE_Coroutine_tlsGetAlt(*(int *)&key, val) == 0 ? val : (void *)0 );
/*LINUX*/    }
/*LINUX*/
/*LINUX*/    extern int pthread_setspecific(pthread_key_t key, const void *arg)
/*LINUX*/    {
/*LINUX*/        return RTE_Coroutine_tlsSetAlt(key, const_cast<void*>(arg));
/*LINUX*/    }
/*LINUX*/
/*LINUX*/    extern int pthread_key_create(pthread_key_t *key, void (*cleanup)(void *))
/*LINUX*/    {
/*LINUX*/        return RTE_Coroutine_tlsCreateAlt((int)*key, (RTE_TLSCleanupFunc) cleanup, false);
/*LINUX*/    }
/*LINUX*/
/*LINUX*/    extern int pthread_key_delete(pthread_key_t key)
/*LINUX*/    {
/*LINUX*/        return RTE_Coroutine_tlsDestroyAlt(key);
/*LINUX*/    }
/*LINUX*/
/*LINIX*/extern int *sqlErrnoLocation();
/*LINUX*/    int *__errno_location(void)
/*LINUX*/    {
/*LINUX*/        /* each thread has its own herrno location in its control block */
/*LINUX*/        RTE_TaskId id = RTE_GetOwnTaskId();
/*LINUX*/        return ( id == RTE_UnknownTaskId ? sqlErrnoLocation()
/*LINUX*/                                          : RTE_Coroutine_ErrnoLocation(id) );
/*LINUX*/    }
/*LINUX*/
/*LINUX*/extern int *sqlHErrnoLocation();
/*LINUX*/    int *__h_errno_location(void)
/*LINUX*/    {
/*LINUX*/        /* each thread has its own herrno location in its control block */
/*LINUX*/        RTE_TaskId id = RTE_GetOwnTaskId();
/*LINUX*/        return ( id == RTE_UnknownTaskId ? sqlHErrnoLocation()
/*LINUX*/                                          : RTE_Coroutine_HErrnoLocation(id) );
/*LINUX*/    }
/*LINUX*/
/*LINUX*/extern int sqlLibc_internal_tsd_set(int key, const void *arg);
/*LINUX*/    int libc_internal_tsd_set(int key, const void *arg)
/*LINUX*/    {
/*LINUX*/        /* libc has some own internal TLS keys separate from 'normal' key handling */
/*LINUX*/        /* the task specific array for these must be used instead of the thread common array */
/*LINUX*/        /* keys are defined for i.e. 'malloc,dl_error or rpc_vars' */
/*LINUX*/        RTE_TaskId id = RTE_GetOwnTaskId();
/*LINUX*/        return ( id == RTE_UnknownTaskId ? sqlLibc_internal_tsd_set(key,arg)
/*LINUX*/                                         : RTE_Coroutines_libcTlsSet(id,key,const_cast<void*>(arg)) );
/*LINUX*/    }
/*LINUX*/
/*LINUX*/extern void *sqlLibc_internal_tsd_get(int key);
/*LINUX*/    void *libc_internal_tsd_get(int key)
/*LINUX*/    {
/*LINUX*/        /* libc has some own internal TLS keys separate from 'normal' key handling */
/*LINUX*/        /* the task specific array for these must be used instead of the thread common array */
/*LINUX*/        /* keys are defined for i.e. 'malloc,dl_error or rpc_vars' */
/*LINUX*/        RTE_TaskId id = RTE_GetOwnTaskId();
/*LINUX*/        return ( id == RTE_UnknownTaskId ? sqlLibc_internal_tsd_get(key)
/*LINUX*/                                         : RTE_Coroutines_libcTlsGet(id,key) );
/*LINUX*/    }
/*LINUX*/
/*LINUX*/    ALIAS_FNC(pthread_getspecific);
/*LINUX*/    ALIAS_FNC(pthread_setspecific);
/*LINUX*/    ALIAS_FNC(pthread_key_create);
/*LINUX*/    ALIAS_FNC(pthread_key_delete);
/*LINUX*/    ALIAS_FNC_LIBC(libc_internal_tsd_get);
/*LINUX*/    ALIAS_FNC_LIBC(libc_internal_tsd_set);
/*LINUX*/}
//-----------------------------------------------------
#endif  /* 0 */
#endif /* LINUX */
//-----------------------------------------------------

/*!--------------------------------------------------------------------
@brief Free task-local variables (used in exception-safe coroutines)
@param pContext [in]       context to clean up
@return none
--------------------------------------------------------------------*/
void RTE_ContextFreeTLS( RTE_CoroutineContext &GivenContextPointer )
{
#ifdef LINUX
  GivenContextPointer.localErrno = 0;
  GivenContextPointer.localHErrno = 0;
  if (GivenContextPointer.libcSpecificTlsKeys) {
    memset(GivenContextPointer.libcSpecificTlsKeys, 0, RTE_MAXIMUM_LIBC_SPECIFIC_KEYS * sizeof(void*));
  }
#endif /* LINUX */
  if (GivenContextPointer.tlsKeys) {
    // clean up all TLS keys

    for (int i = 0; i < THREAD_KEYS_FIRST_LEVEL; ++i) {
      void **sl = GivenContextPointer.tlsKeys[i];
      if (sl) {
        for (int j = 0; j < THREAD_KEYS_SECOND_LEVEL; ++j) {
          if (sl[j]) {
            // destroy this key
            int key = i | (j * THREAD_KEYS_FIRST_LEVEL);
            if (RTE_TLSKeys[key].pCleanup != NULL) {
              RTE_TLSKeys[key].pCleanup(sl[j]);
            }
          }
        }
        free(sl);
      }
    }
    free(GivenContextPointer.tlsKeys);
    GivenContextPointer.tlsKeys = NULL;
  }
}

/*============================================*/
/*============================================*/
/*============================================*/
#else /* SAPDB_WITH_EXCEPTION_SAFE_COROUTINES > 0 */
/*============================================*/
/*============================================*/
/*============================================*/

/*!--------------------------------------------------------------------
@brief Free task-local variables (used in exception-safe coroutines)
@param pContext [in]       context to clean up
@return none
--------------------------------------------------------------------*/
void RTE_ContextFreeTLS( RTE_CoroutineContext &GivenContextPointer )
{
}

/*============================================*/
/*============================================*/
/*============================================*/
#endif /* SAPDB_WITH_EXCEPTION_SAFE_COROUTINES > 0 */
/*============================================*/
/*============================================*/
/*============================================*/
