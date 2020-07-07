/*!
  @file           veo07.c
  @author         JoergM
  @special area   Multithreading Runtine OS Independend
  @brief          Multithread Interface using WIN32 Threads, CThreads or
              PThreads DRAFT 4-10. See DEFINES section below...
  @see            example.html ...

\if EMIT_LICENCE

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


//
//   FreeBSD portions added by Kai Mosebach,
//   For more informations see : http://www.komadev.de/sapdb
//

\endif
*/

/*
 * _REENTRANT is defined for all UNIX platforms if compiled multithreaded
 * but for WIN32 this is _MT ...
 */
#if ( defined(_REENTRANT) && !defined(_WIN32) ) || ( defined(_MT) && defined(_WIN32) )

#define MOD__ "veo07.c :"

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#ifdef _WIN32
#include <windows.h>
#include <limits.h>
#else

#if defined(SUN) || defined(SOLARIS)
#include <errno.h>
#endif

#if defined(HP9)
#  if defined(PA11) || defined(PA20W)
#     define _POSIX_C_SOURCE 199506L
#  endif /* PA11 || PA20W */
#endif /* HP9 */

#if defined(HP_IA64)
#  define _POSIX_C_SOURCE 199506L
#endif /* HP_IA64 */

#include <pthread.h>
#include <unistd.h>

#endif

#if defined(FREEBSD)
#include <sys/mman.h>
#endif

#include <signal.h>

#include "geo00_0.h"
#include "geo57.h"
#include "heo07.h"
#include "geo001.h"
#include "geo007_1.h" /* For error messages */
#include "geo007_2.h" /* For error messages */
#include "SAPDBCommon/SAPDB_string.h"
#include "SAPDBCommon/SAPDB_sprintf.h"
#if defined(KERNEL_LZU)
#     include "geo50_0.h" /* nocheck */ /* sql57k_pmalloc, sql57k_pfree, vabort */
#  if !defined(_WIN32)
extern SAPDB_Bool RTE_PseudoAllocStack(SAPDB_ULong stackSize, SAPDB_ULong guardSize);
extern void RTE_PseudoFreeStack(SAPDB_ULong stackSize, SAPDB_ULong guardSize);
#  endif /*_WIN32 */
#endif /* KERNEL_LZU */

#if defined(KERNEL_LZU) && !defined(WIN32)
#    include "RunTime/RTE_Coroutine.h" /*nocheck*/
#  ifndef SAPDB_WITH_EXCEPTION_SAFE_COROUTINES
/* #define SAPDB_WITH_EXCEPTION_SAFE_COROUTINES 1 */
#    define SAPDB_WITH_EXCEPTION_SAFE_COROUTINES 1
#  endif

#  if SAPDB_WITH_EXCEPTION_SAFE_COROUTINES > 0
/* #    define GET_SELF ((teo07_ThreadObj *)eo07_GetSelf()) */
#    define GET_SELF ((teo07_ThreadObj *)sqlgettls(self_key_eo07))
#  else
#    define GET_SELF ((teo07_ThreadObj *)sqlgettls(self_key_eo07))
#  endif
#else
#  define GET_SELF ((teo07_ThreadObj *)sqlgettls(self_key_eo07))
#endif

#define STATE_PARENT_DETACHED  0x01
#define STATE_THREAD_ENDED     0x02

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#if defined(_WIN32)

# define YIELD_CALL Sleep(0)

#else

/* All UNIX platforms use pthreads */
# define PTHREADS 

# undef HAVE_PTHREAD_SCOPE_PROCESS
# undef TIMEOUT_SYSTEM_RC
    /* PTS 1105217 */
# undef USE_GETTID_FOR_THREAD_ID
# undef USE_EO07_GET_SEQUENCE_NUMBER_FOR_THREAD_ID
# undef USE_GETSEQUENCE_NP_FOR_THREAD_ID
# undef USE_CLONE_PID_FOR_THREAD_ID

# if defined(SUN) || defined(SOLARIS)
#  define TIMEOUT_SYSTEM_RC ETIMEDOUT
#  define USE_EO07_GET_SEQUENCE_NUMBER_FOR_THREAD_ID 1
#  define YIELD_CALL sched_yield()
#  define HAVE_PTHREAD_SCOPE_PROCESS
# endif

# if defined(OSF1)
#  define TIMEOUT_SYSTEM_RC ETIMEDOUT
#  define USE_GETSEQUENCE_NP_FOR_THREAD_ID 1
#  define YIELD_CALL sched_yield()
#  define HAVE_PTHREAD_SCOPE_PROCESS
#  define WANT_HIGHEST_ADDRESS_AS_STACK_ADDRESS
# endif

# if defined(_IBMR2)
#  define TIMEOUT_SYSTEM_RC ETIMEDOUT
#  define USE_EO07_GET_SEQUENCE_NUMBER_FOR_THREAD_ID 1
#  define YIELD_CALL yield()
#  define HAVE_PTHREAD_SCOPE_PROCESS
#  define WANT_HIGHEST_ADDRESS_AS_STACK_ADDRESS
# endif

# if defined(PA11) || defined(PA20W) || defined(HP_IA64)
#  define TIMEOUT_SYSTEM_RC ETIMEDOUT
#  define USE_EO07_GET_SEQUENCE_NUMBER_FOR_THREAD_ID 1
#  define YIELD_CALL sched_yield()
# endif

# if defined(LINUX)

#  define LINUXTHREAD_CHECKS
#  define TIMEOUT_SYSTEM_RC ETIMEDOUT

#  if !defined(HAS_NPTL)
#    define USE_CLONE_PID_FOR_THREAD_ID 1
#    define YIELD_CALL sched_yield()
#  else
#    define YIELD_CALL pthread_yield()
/*!
  The manual page for gettid() shows an alternative to using syscall() but
  unfortunatly this does not work on linuxia64! Using syscall(__NR_gettid)
  instead works fine on all Linux plattforms tested 
  (linux,linuxx86_64,linuxpppc,linuxia64,linuxs390x)
 */
#    define USE_GETTID_FOR_THREAD_ID
#       include <sys/syscall.h>
#    define HAVE_PTHREAD_SCOPE_PROCESS
#  endif /* HAS_NPTL */

# endif  /* LINUX */

# if defined(FREEBSD)
#  define LINUXTHREAD_CHECKS
#  define YIELD_CALL sched_yield()
#  define HAVE_PTHREAD_SCOPE_PROCESS
#  define TIMEOUT_SYSTEM_RC ETIMEDOUT
#ifndef FREEBSD_THREADS  /* linuxthreads */
#  define USE_CLONE_PID_FOR_THREAD_ID 1
#else                   /* KSE */
#  define USE_EO07_GET_SEQUENCE_NUMBER_FOR_THREAD_ID 1
#endif
#endif

#endif /* !_WIN32 */

#if defined(_WIN32)
# define THREAD_TYPENAME_EO07 HANDLE
# define MUTEX_TYPENAME_EO07  CRITICAL_SECTION
#else
# define THREAD_TYPENAME_EO07 pthread_t
# define MUTEX_TYPENAME_EO07  pthread_mutex_t
# define COND_TYPENAME_EO07   pthread_cond_t
#endif

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

#ifdef USE_MALLOC
#include <malloc.h>
#undef ALLOC_MEM_EO57
#define ALLOC_MEM_EO57(_pBuf,_s) \
    ( (*_pBuf = malloc(_s)) != NULL ? NO_ERROR_EO001 : ERROR_NOT_ENOUGH_MEMORY_EO001 )
#undef FREE_MEM_EO57
#define FREE_MEM_EO57(_Buf) free(_Buf)
#endif /* USE_MALLOC */

#ifdef DO_ABORT
#  undef DO_ABORT
#endif /* DO_ABORT */

#if defined(_WIN32)
#  define DO_ABORT() ABORT()
#else
#  if defined(KERNEL_LZU)
#include "hen50.h" /* nocheck */
#    define DO_ABORT() vabort(1) /* WRITE_CORE ... */
#  else
#include "hen40.h" /* nocheck */
#    define DO_ABORT() sqlabort()
#  endif
#endif

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/* TYPEDEFS */
typedef struct _teo07_Thread
{
  THREAD_TYPENAME_EO07  thread;
  teo07_ThreadId        thread_id;
  teo07_ThreadSemaphore suspend_sem; /* for suspended start and selfsuspend */
  tsp00_Int4            start_suspended;
  SAPDB_ULong           givenStackSize;
#if (defined LINUX && !defined(HAS_NPTL)) || (defined FREEBSD && defined LINUXTHREADS && !defined FREEBSD_THREADS)
  char *                StackAddr;
  size_t                StackSize;
#  if SAPDB_WITH_EXCEPTION_SAFE_COROUTINES > 0
  int                   threadLocalErrno;
  int                  *pThreadLocalErrno;
  int                   threadLocalHErrno;
  int                  *pThreadLocalHErrno;
  int                   libcSpecificTlsKeys[RTE_MAXIMUM_LIBC_SPECIFIC_KEYS];
#  endif
#endif
#if SAPDB_WITH_EXCEPTION_SAFE_COROUTINES > 0
  void              *** tlsKeys;
#endif
  void *              (*proc)(void *);
  void                 *arg;
  void                 *thread_class_pointer; /* for C++ interfacing */
  int                  state;
} teo07_ThreadObj;

struct _teo07_Mutex
{
  MUTEX_TYPENAME_EO07   mutex;
  teo07_ThreadId        owning_thread;
  tsp00_Int4            lock_count;
};

#if !defined(_WIN32)
typedef struct _teo07_ThreadSemaphore
{
    tsp00_Int4          token;
    tsp00_Int4          waiter;
    MUTEX_TYPENAME_EO07 mutex;
    COND_TYPENAME_EO07  condition;
} teo07_SemObj;
#endif /* !WIN32 */


/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/

#  if SAPDB_WITH_EXCEPTION_SAFE_COROUTINES > 0
#if defined LINUX && !defined(HAS_NPTL)
static int            eo07_mainThreadLocalErrno = 0;
static int            eo07_mainThreadLocalHErrno = 0;
static int            eo07_mainThreadLibcSpecificTlsKeys[RTE_MAXIMUM_LIBC_SPECIFIC_KEYS];
#endif

static void        ***eo07_mainThreadTlsKeys = (void ***)0;
#endif

static int            eo07_EverInitialized = 0;
/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/

static char *EO07_ERR_NO_MEM           = { ERRMSG_ALLOC_MEMORY };
static char *EO07_ERR_STACKSIZE        = { ERRMSG_STACKSIZE };
static char *EO07_ERR_RESOURCE_LIMIT   = { ERRMSG_RESOURCE_LIMIT };
static char *EO07_ERR_PROCADDRESS_NULL = { ERRMSG_PROCADDRESS_NULL };
static char *EO07_ERR_UNKNOWN_THREADID = { ERRMSG_UNKNOWN_THREADID };
static char *EO07_ERR_INVALID_SIGNAL   = { ERRMSG_INVALID_SIGNAL };
static char *EO07_ERR_DEADLOCK         = { ERRMSG_DEADLOCK };
static char *EO07_ERR_INVALID_KEY      = { ERRMSG_INVALID_TLSKEY };
static char *EO07_ERR_BUSY_KEY         = { ERRMSG_TLSKEY_BUSY };
static char *EO07_ERR_NOTIMPLEMENTED   = { ERRMSG_NOT_YET_IMPLEMENTED };

#ifndef SAPDB_WITH_COROUTINES
static tsp00_Int4 self_key_eo07 = 0;
#endif

#ifdef LINUXTHREAD_CHECKS
static int has_linuxthread_implementation = 0;
static int has_limited_pthread_attr_setstacksize = 0;
#endif /* LINUXTHREAD_CHECKS */

#ifdef PTHREADS
/*
 * This is used for synchronization of access to nextThreadId_eo77
 */
#ifndef HAS_GETSEQUENCE_NP
 static teo07_ThreadId nextThreadId_eo07 = (teo07_ThreadId)0;
# ifdef PTHREAD_MUTEX_INITIALIZER
   static pthread_mutex_t threadIdMutex_eo07 = PTHREAD_MUTEX_INITIALIZER;
# else
   static pthread_mutex_t threadIdMutex_eo07;
# endif /* PTHREAD_MUTEX_INITIALIZER */
#endif /* HAS_GETSEQUENCE_NP */

#ifdef PTHREAD_MUTEX_INITIALIZER
 static pthread_mutex_t onceMutex_eo07        = PTHREAD_MUTEX_INITIALIZER;
 static pthread_mutex_t delResourceMutex_eo07 = PTHREAD_MUTEX_INITIALIZER;
#else
 static pthread_mutex_t onceMutex_eo07;
 static pthread_mutex_t delResourceMutex_eo07;
#endif /* PTHREAD_MUTEX_INITIALIZER */

#endif /* PTHREADS */

/*===========================================================================*
 *  LOCAL FUNCTIONS (PROTOTYPES)                                             *
 *===========================================================================*/

#ifdef LINUXTHREAD_CHECKS
static int eo07_TestOnLinuxthreadBehaviour();
static void * eo07_TestThreadEntry( void *arg );
#endif

static void eo07_CreateThread( tsp00_Int4 StackSize,
                               void *StackStartAddress,
                               void *(*proc)(void *),
                               void *arg,
                               tsp00_Int4 flags,
                               teo07_Thread *thread,
                               tsp00_ErrTextc errtext,
                               teo07_ThreadErr *ok);

#ifdef _WIN32

static teo07_ThreadErr eo07_GetError(tsp00_ErrTextc errtext);
static DWORD eo07_Win32threadEntry( void *arg );

#else

static void *eo07_PthreadEntry( void *arg );
# ifdef USE_EO07_GET_SEQUENCE_NUMBER_FOR_THREAD_ID
static teo07_ThreadId eo07_GetSequenceNumber();
# endif /* USE_EO07_GET_SEQUENCE_NUMBER_FOR_THREAD_ID */

#endif

#if (defined LINUX && !defined(HAS_NPTL)) || (defined FREEBSD && defined LINUXTHREADS && !defined FREEBSD_THREADS)
/* PTS 1105262/1105263 */
static void * eo07_NoheapMalloc(size_t size);
static void   eo07_NoheapFree(void *ptr, size_t size);
static void * eo07_DummyPthreadEntry( void *arg );
#endif

#ifdef LINUX
#  ifndef HAS_NPTL
/* PTS 1106738 */
/* With RedHat6.2 the mmap() prototype was fixed. This prevented the kernel from */
/* starting. The message given was 'invalid argument'. This Message was given due */
/* to the last argument of mmap() which was defined as 'off_t' and later 'off64_t' */
/* So on RedHat6.2 the last argument was expected to be a 64bit value, therefore */
/* the extra 32bits are collected from the stack. Such an offset is not PAGESIZE */
/* aligned, so it leads to 'invalid argument'... */
#    undef __USE_FILE_OFFSET64
#    define __USE_LARGEFILE64
#  endif /* !HAS_NPTL */
#  include <sys/mman.h>
#endif /* LINUX */


/*===========================================================================*
 *  GLOBAL FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

void sqlinit_multithreading( teo07_Thread *thread,
                             tsp00_ErrTextc errtext,
                             teo07_ThreadErr *ok)
{
#undef MF__
#define MF__ MOD__"sqlinit_multithreading"

    static teo07_ThreadObj *pThreadObj = NULL;

#if defined(KERNEL_LZU)
#ifdef LINUXTHREAD_CHECKS
    if ( !eo07_TestOnLinuxthreadBehaviour() )
    {
        strcpy(errtext, "Linux installation problem");
        *ok = THR_NOT_OK_EO07;
        return;
    }
#endif
#endif

    if ( thread == NULL )
    {
        *ok = THR_NOT_OK_EO07;
        if ( errtext != NULL )
        {
            SAPDB_memcpy(errtext, EO07_ERR_UNKNOWN_THREADID, strlen(EO07_ERR_UNKNOWN_THREADID)+1);
        }
        return;
    }

    if ( pThreadObj == NULL )
    {
        if ( ALLOC_MEM_EO57( (void **)&pThreadObj, sizeof(teo07_ThreadObj) ) != NO_ERROR_EO001 )
        {
            *ok = THR_NOT_OK_EO07;
            if (errtext != NULL)
            {
                strcpy(errtext, EO07_ERR_NO_MEM);
            }
            return;
        }
        *thread = (teo07_Thread)pThreadObj;
        *ok = THR_OK_EO07;
    }
    else
    {
        *thread = (teo07_Thread)pThreadObj;
        *ok = THR_OK_EO07;
        return;
    }

#ifdef _WIN32
    DuplicateHandle(GetCurrentProcess(),
                    GetCurrentThread(),
                    GetCurrentProcess(),
                    & pThreadObj->thread,
                    DUPLICATE_SAME_ACCESS,
                    FALSE,
                    0);
    pThreadObj->thread_id = GetCurrentThreadId();
#else

# ifndef HAS_GETSEQUENCE_NP
    nextThreadId_eo07 = (teo07_ThreadId)1;
# endif /* HAS_GETSEQUENCE_NP */

# ifndef PTHREAD_MUTEX_INITIALIZER
#  ifndef HAS_GETSEQUENCE_NP
    (void)pthread_mutex_init(&threadIdMutex_eo07, NULL);
#  endif /* HAS_GETSEQUENCE_NP */
    (void)pthread_mutex_init(&onceMutex_eo07, NULL);
    (void)pthread_mutex_init(&delResourceMutex_eo07, NULL);
# endif

    pThreadObj->thread    = pthread_self();

# ifdef USE_GETTID_FOR_THREAD_ID
    pThreadObj->thread_id = (teo07_ThreadId)syscall(__NR_gettid);
# endif

# ifdef USE_EO07_GET_SEQUENCE_NUMBER_FOR_THREAD_ID
    pThreadObj->thread_id = eo07_GetSequenceNumber();
# endif /* USE_EO07_GET_SEQUENCE_NUMBER_FOR_THREAD_ID */

# ifdef USE_GETSEQUENCE_NP_FOR_THREAD_ID
    pThreadObj->thread_id = (teo07_ThreadId)pthread_getsequence_np(pThreadObj->thread);
# endif

# ifdef USE_CLONE_PID_FOR_THREAD_ID
    /* PTS 1105217 */
    pThreadObj->thread_id = (teo07_ThreadId)getpid();
#endif

#endif

    sqlcreatesem(&(pThreadObj->suspend_sem), 0, errtext, ok);
    if ( *ok != THR_OK_EO07 )
    {
        FREE_MEM_EO57(pThreadObj);
        pThreadObj = NULL;
        *thread    = NULL;
        return;
    }
    pThreadObj->start_suspended = 0;
    pThreadObj->proc            = NULL;
    pThreadObj->arg             = 0;
    pThreadObj->givenStackSize  = 0;

#ifndef SAPDB_WITH_COROUTINES
    sqlcreatetls(&self_key_eo07, errtext, ok);
#else
    eo07_RegisterSelf(pThreadObj,
#  if  defined(LINUX) && !defined(HAS_NTPL)
    pThreadObj->pThreadLocalErrno = &eo07_mainThreadLocalErrno;
    pThreadObj->pThreadLocalHErrno = &eo07_mainThreadLocalHErrno;
    SAPDB_memcpy(pThreadObj->libcSpecificTlsKeys,
           eo07_mainThreadLibcSpecificTlsKeys,
           sizeof(int)*RTE_MAXIMUM_LIBC_SPECIFIC_KEYS);
#  endif /* LINUX&&!HAS_NPTL */
    pThreadObj->tlsKeys = eo07_mainThreadTlsKeys;
#endif

    if ( *ok == THR_OK_EO07 )
    {
        eo07_EverInitialized = 1;
#ifndef SAPDB_WITH_COROUTINES
        sqlsettls(self_key_eo07, pThreadObj, errtext, ok);
#endif
    }
    else
    {
        FREE_MEM_EO57(pThreadObj);
        pThreadObj = NULL;
        *thread    = NULL;
    }

} /* sqlinit_multithreading */

/*---------------------------------------------------------------------------*/

void DeleteResources( teo07_ThreadObj *pThreadObj,
                      int              stateCondition,
                      int              newState )
{
    SAPDB_Bool    delResource = false;

#if defined (_WIN32)
    static LONG lock = 1;
#else 
    int rc;
#endif


#if defined (_WIN32)
    while(InterlockedExchange(&lock,0) == 0) { Sleep(50); }
#else
    rc = pthread_mutex_lock( &delResourceMutex_eo07 );

    if ( rc != 0 )
    {
        MSGD((INFO_TEST_WILDCARD, "DeleteResources: Work is done without lock!"));
    }
#endif

    if ( pThreadObj->state & stateCondition ) 
        delResource = true;
    else
        pThreadObj->state |= newState;


#if defined (_WIN32)
    InterlockedExchange(&lock,1);  /* release the lock just acquired */
#else
    pthread_mutex_unlock( &delResourceMutex_eo07 );
#endif


    if ( delResource )
    {
        sqldestroysem( pThreadObj->suspend_sem );

        SAPDB_memset(pThreadObj, 0, sizeof( teo07_ThreadObj ));
        FREE_MEM_EO57(pThreadObj);
    }
}


/*---------------------------------------------------------------------------*/


void sqlbeginthread(tsp00_Int4 StackSize,
                    void *(*proc)(void *),
                    void *arg,
                    tsp00_Int4 flags,
                    teo07_Thread *thread,
                    tsp00_ErrTextc errtext,
                    teo07_ThreadErr *ok)
{
#undef MF__
#define MF__ MOD__"sqlbeginthread"
    DBGPAS;

    eo07_CreateThread(StackSize, (void *)0, proc, arg, flags, thread, errtext, ok);
} /* sqlbeginthread */

/*---------------------------------------------------------------------------*/

tsp00_Int4 sqlget_thread_min_stack()
{
#if defined(WIN32)
    return 0;
#else
    static tsp00_Int4 min_thread_stack_size = -1;
    if ( -1 == min_thread_stack_size )
    {
#if defined(_SC_THREAD_STACK_MIN)
        min_thread_stack_size = (SAPDB_Int4)sysconf(_SC_THREAD_STACK_MIN);
#endif
        if ( min_thread_stack_size <= 0 )
        {
            min_thread_stack_size = 0;
#ifdef PTHREAD_STACK_MIN
            min_thread_stack_size = ((tsp00_Int4)PTHREAD_STACK_MIN);
#endif
        }
#ifdef PTHREAD_STACK_MIN
        else
        {
            if ( ((tsp00_Int4)PTHREAD_STACK_MIN) > min_thread_stack_size )
            {
                min_thread_stack_size = ((tsp00_Int4)PTHREAD_STACK_MIN);
            }
        }
#endif
    }

    return min_thread_stack_size;
#endif
}

/*---------------------------------------------------------------------------*/

void sqlbeginthread_on_own_stack(tsp00_Int4 StackSize,
                    void *StackStartAddress,
                    void *(*proc)(void *),
                    void *arg,
                    tsp00_Int4 flags,
                    teo07_Thread *thread,
                    tsp00_ErrTextc errtext,
                    teo07_ThreadErr *ok)
{
#undef MF__
#define MF__ MOD__"sqlbeginthread_on_own_stack"

    DBGPAS;

    eo07_CreateThread(StackSize, StackStartAddress, proc, arg, flags, thread, errtext, ok);
} /* sqlbeginthread_on_own_stack */

/*---------------------------------------------------------------------------*/

void sqlendthread(tsp00_Int4 returncode)
{
#undef MF__
#define MF__ MOD__"sqlendthread"

    teo07_ThreadObj *pThreadObj = (teo07_ThreadObj *)sqlgettls(self_key_eo07);

#if defined(KERNEL_LZU)
  if ( pThreadObj != 0)
  {
      if ( pThreadObj->givenStackSize > 0 ) RTE_PseudoFreeStack(pThreadObj->givenStackSize, 0);
  }
#endif

  /* delete resource if the parent has been detached */
  DeleteResources( pThreadObj, STATE_PARENT_DETACHED, STATE_THREAD_ENDED );

#ifdef _WIN32
  ExitThread(returncode);
#else
  pthread_exit((void *)returncode);
#endif
} /* sqlendthread */

/*---------------------------------------------------------------------------*/

void sqlkillthread(teo07_Thread thread,
                   tsp00_ErrTextc errtext,
                   teo07_ThreadErr *ok)
{
#undef MF__
#define MF__ MOD__"sqlkillthread"

    sqlsigthread(thread, SIGTERM, errtext, ok);
} /* sqlkillthread */

/*---------------------------------------------------------------------------*/

void sqlsigthread(teo07_Thread thread,
                  tsp00_Int4 sig,
                  tsp00_ErrTextc errtext,
                  teo07_ThreadErr *ok)
{
#undef MF__
#define MF__ MOD__"sqlsigthread"

#ifndef _WIN32
    teo001_SystemRc rc;
#endif /* !_WIN32 */

    teo07_ThreadObj *pThreadObj = (teo07_ThreadObj *)thread;

    if ( pThreadObj == NULL )
    {
        *ok = THR_NOT_OK_EO07;
    }

#ifdef _WIN32
    if(!TerminateThread(pThreadObj->thread, sig))
    {
        *ok = eo07_GetError(errtext);
    }
    else
    {
        *ok = THR_OK_EO07;
    }
#else /* UNIX */

    rc = pthread_kill(pThreadObj->thread, sig);
    if (rc != 0)
    {
        *ok = THR_NOT_OK_EO07;
        if(errtext != NULL)
        {
            switch(rc)
            {
            case (ESRCH):
                strcpy(errtext, EO07_ERR_UNKNOWN_THREADID);
                break;

            case (EINVAL):
                strcpy(errtext, EO07_ERR_INVALID_SIGNAL);
                break;

            default:
                strcpy(errtext, EO07_ERR_NOTIMPLEMENTED);
                break;
            }
        }
    }
    else
    {
        *ok = THR_OK_EO07;
    }

#endif /* UNIX */
} /* sqlsigthread */

/*---------------------------------------------------------------------------*/

void sqlcancelthread(teo07_Thread thread,
                     tsp00_Int4 exitcode,
                     tsp00_ErrTextc errtext,
                     teo07_ThreadErr *ok)
{
#undef MF__
#define MF__ MOD__"sqlcancelthread"

    teo07_ThreadObj *pThreadObj = (teo07_ThreadObj *)thread;

    *ok = THR_OK_EO07;

    if ( (pThreadObj == NULL) || (pThreadObj->thread_id == 0) )
    {
        return;
    }

#ifdef _WIN32
    if ( (pThreadObj->thread != ((HANDLE)0))
      && (pThreadObj->thread != INVALID_HANDLE_VALUE) )
    {
        if ( !TerminateThread ( pThreadObj->thread, exitcode ) )
        {
            *ok = eo07_GetError(errtext);
        }
    }
#else
    {
        teo001_SystemRc rc;
        rc = pthread_cancel(pThreadObj->thread);
        if ( rc != 0
          && rc != ESRCH
          && rc != EINVAL )
        {
            *ok = THR_NOT_OK_EO07;
            if ( errtext != NULL )
            {
                switch(rc)
                {
                case (EDEADLK):
                    strcpy(errtext, EO07_ERR_DEADLOCK);
                    break;

                default:
                    strcpy(errtext, EO07_ERR_NOTIMPLEMENTED);
                    break;
                }
            }
        }
    }
#endif
}

/*---------------------------------------------------------------------------*/

void sqlsetthreadclasspointer (teo07_Thread thread, void *thread_class_pointer)
{
    teo07_ThreadObj *pThreadObj = (teo07_ThreadObj *)thread;

    if ( (pThreadObj == NULL) || (pThreadObj->thread_id == 0) || (pThreadObj->thread_class_pointer != 0) )
    {
        return;
    }
    pThreadObj->thread_class_pointer = thread_class_pointer;
}

/*---------------------------------------------------------------------------*/

void *sqlgetthreadclasspointer (teo07_Thread thread)
{
    teo07_ThreadObj *pThreadObj = (teo07_ThreadObj *)thread;

    if ( (pThreadObj == NULL) || (pThreadObj->thread_id == 0) )
    {
        return 0;
    }
    return pThreadObj->thread_class_pointer;
}

/*---------------------------------------------------------------------------*/

void sqlforgetthread (teo07_Thread thread)
{
#undef MF__
#define MF__ MOD__"sqlforgetthread"

    teo07_ThreadObj *pThreadObj = (teo07_ThreadObj *)thread;

    if ( (pThreadObj == NULL) || (pThreadObj->thread_id == 0) )
    {
        return;
    }

#ifdef _WIN32
    if ( (pThreadObj->thread != ((HANDLE)0))
      && (pThreadObj->thread != INVALID_HANDLE_VALUE) )
    {
        CloseHandle ( pThreadObj->thread );
    }
#else
    {
        int rc;
        rc = pthread_detach (pThreadObj->thread);
        if ( rc != 0 )
        {
            char buffer[256];
            if ( EINVAL == rc )
            {
                SAPDB_sprintf(buffer, sizeof(buffer), 
                    "pthread_detach(tid=%d) called for non joinable thread", pThreadObj->thread_id);
            }
            else if ( ESRCH == rc )
            {
                SAPDB_sprintf(buffer, sizeof(buffer), 
                    "pthread_detach(tid=%d) called for non existing thread", pThreadObj->thread_id);
            }
            else
            {
                SAPDB_sprintf(buffer, sizeof(buffer), 
                    "pthread_detach(tid=%d) called, but returned unspecified error %d", pThreadObj->thread_id, rc);
            }
            MSGD((INFO_TEST_WILDCARD, buffer));
        }
    }
#endif

    /* delete resource if thread has stopped */
    DeleteResources( pThreadObj, STATE_THREAD_ENDED, STATE_PARENT_DETACHED );

} /* sqlforgetthread */

/*---------------------------------------------------------------------------*/

void sqlyieldthread()
{
#undef MF__
#define MF__ MOD__"sqlyieldthread"

    YIELD_CALL;
}

/*---------------------------------------------------------------------------*/

teo07_Thread sqlgetthread()
{
#undef MF__
#define MF__ MOD__"sqlgetthread"
    return (teo07_Thread)GET_SELF;
}

/*---------------------------------------------------------------------------*/

#if SAPDB_WITH_EXCEPTION_SAFE_COROUTINES > 0
#if defined LINUX && !defined(HAS_NPTL)
int *sqlErrnoLocation()
{
    if ( !eo07_EverInitialized ) return &eo07_mainThreadLocalErrno;

    return GET_SELF->pThreadLocalErrno;
}

/*---------------------------------------------------------------------------*/

int *sqlHErrnoLocation()
{
    if ( !eo07_EverInitialized ) return &eo07_mainThreadLocalHErrno;

    return GET_SELF->pThreadLocalHErrno;
}

/*---------------------------------------------------------------------------*/

extern int sqlLibc_internal_tsd_set(int key, const void *arg)
{
    if ( key < 0 || key > RTE_MAXIMUM_LIBC_SPECIFIC_KEYS )
    {
        return EINVAL;
    }

    if ( !eo07_EverInitialized )
    {
        eo07_mainThreadLibcSpecificTlsKeys[key] = (void *)arg;
    }
    else
    {
        GET_SELF->libcSpecificTlsKeys[key] = (void *)arg;
    }
}

/*---------------------------------------------------------------------------*/

extern void *sqlLibc_internal_tsd_get(int key)
{
    if ( key < 0 || key > RTE_MAXIMUM_LIBC_SPECIFIC_KEYS )
    {
        return (void *)0;
    }

    if ( !eo07_EverInitialized )
    {
        return eo07_mainThreadLibcSpecificTlsKeys[key];
    }
    else
    {
        return GET_SELF->libcSpecificTlsKeys[key];
    }
}
#endif /* LINUX&&!HAS_NPTL */

/*---------------------------------------------------------------------------*/

void ****sqlGetTlsArray()
{
    if ( !eo07_EverInitialized ) return &eo07_mainThreadTlsKeys;

    return &(GET_SELF->tlsKeys);
}
#endif /* SAPDB_WITH_EXCEPTION_SAFE_COROUTINES  */

/*---------------------------------------------------------------------------*/

teo07_ThreadErr sqlgetthreadpriority(  teo07_Thread thread,
                                       tsp00_Int4 * pPriority )
{
    int rc = 0;
    teo07_ThreadObj *pThreadObj = (teo07_ThreadObj *)thread;

#if defined(_WIN32)
    int priority;
    priority = GetThreadPriority(pThreadObj->thread);
    if ( THREAD_PRIORITY_ERROR_RETURN == priority )
    {
        rc = 1;
    }
    else
    {
        *pPriority = priority;
    }
#else
    int schedpolicy;
    struct sched_param schedparam;

    rc = pthread_getschedparam(pThreadObj->thread, &schedpolicy, &schedparam);
    if ( rc == 0 )
    {
        *pPriority = schedparam.sched_priority;
    }
#endif

    return ( rc == 0 ? THR_OK_EO07 : THR_NOT_OK_EO07 );
}

/*---------------------------------------------------------------------------*/

teo07_ThreadErr sqlgetmythreadpriority( tsp00_Int4 * pPriority )
{
    int rc = 0;

#if defined(_WIN32)
    HANDLE myThread = GetCurrentThread();
    int priority;

    priority = GetThreadPriority(myThread);
    if ( THREAD_PRIORITY_ERROR_RETURN == priority )
    {
        rc = 1;
    }
    else
    {
        *pPriority = priority;
    }
#else
    int schedpolicy;
    struct sched_param schedparam;
    pthread_t myThread = pthread_self();

    rc = pthread_getschedparam(myThread, &schedpolicy, &schedparam);
    if ( rc == 0 )
    {
        *pPriority = schedparam.sched_priority;
    }
#endif

    return ( rc == 0 ? THR_OK_EO07 : THR_NOT_OK_EO07 );
}

/*---------------------------------------------------------------------------*/

teo07_ThreadErr sqlsetthreadpriority( teo07_Thread thread,
                                      tsp00_Int4 priority )
{
    int rc = 0;
    teo07_ThreadObj *pThreadObj = (teo07_ThreadObj *)thread;

#if defined(_WIN32)
    rc = ( 0 != SetThreadPriority(pThreadObj->thread, (int)priority) );
#else
    int schedpolicy;
    struct sched_param schedparam;
    pthread_t myThread = pthread_self();

    rc = pthread_getschedparam(pThreadObj->thread, &schedpolicy, &schedparam);
    if ( rc == 0 )
    {
        schedparam.sched_priority = priority;
        rc = pthread_setschedparam(pThreadObj->thread, schedpolicy, &schedparam);
    }
#endif

    return ( rc == 0 ? THR_OK_EO07 : THR_NOT_OK_EO07 );
}

/*---------------------------------------------------------------------------*/

teo07_ThreadErr sqlsetmythreadpriority( tsp00_Int4 priority)
{
    int rc = 0;

#if defined(_WIN32)
    HANDLE myThread = GetCurrentThread();

    rc = ( 0 != SetThreadPriority(myThread, (int)priority) );
#else
    int schedpolicy;
    struct sched_param schedparam;
    pthread_t myThread = pthread_self();

    rc = pthread_getschedparam(myThread, &schedpolicy, &schedparam);
    if ( rc == 0 )
    {
        schedparam.sched_priority = priority;
        rc = pthread_setschedparam(myThread, schedpolicy, &schedparam);
    }
#endif

    return ( rc == 0 ? THR_OK_EO07 : THR_NOT_OK_EO07 );
}

/*---------------------------------------------------------------------------*/

teo07_ThreadId sqlgetthreadid()
{
#undef MF__
#define MF__ MOD__"sqlgetthreadid"

#ifdef _WIN32
    return (teo07_ThreadId)GetCurrentThreadId();
#else

# ifdef USE_GETTID_FOR_THREAD_ID
    return (teo07_ThreadId)syscall(__NR_gettid);
# endif

# ifdef USE_CLONE_PID_FOR_THREAD_ID
    return (teo07_ThreadId)getpid();
# else

    teo07_ThreadObj *pThreadObj;

    if ( !eo07_EverInitialized )
    {
        return 1;
    }

    pThreadObj = GET_SELF;
    if ( pThreadObj == NULL )
    {
        union {
            pthread_t      pt;
            teo07_ThreadId id;
        } u;

        u.pt = pthread_self();
        return u.id;
    }
    else
    {
      return pThreadObj->thread_id;
    }
# endif /* USE_CLONE_PID_FOR_THREAD_ID */

#endif
}

/*---------------------------------------------------------------------------*/

teo07_ThreadId sqlthreadid(teo07_Thread thread)
{
#undef MF__
#define MF__ MOD__"sqlthreadid"

    return ((teo07_ThreadObj *)thread)->thread_id;
}

/*---------------------------------------------------------------------------*/

void sqlsuspendthread(teo07_Thread thread,
                      tsp00_ErrTextc errtext,
                      teo07_ThreadErr* ok)
{
#undef MF__
#define MF__ MOD__"sqlsuspendthread"

  if ( (void *)GET_SELF != (void *)thread )
  {
      *ok = THR_NOT_IMPLEMENTED_EO07;
      if(errtext != NULL)
          strcpy(errtext, EO07_ERR_NOTIMPLEMENTED);
  }
  else
  {
      DBG1(( MF__,"suspending thread %d\n", sqlthreadid(thread) ));
      sqlwaitsem( ((teo07_ThreadObj *)thread)->suspend_sem );
      *ok = THR_OK_EO07;
  }
}

/*---------------------------------------------------------------------------*/

tsp00_Bool sqlIsThreadSuspended(teo07_Thread thread)
{
#undef MF__
#define MF__ MOD__"sqlsuspendthread"
    tsp00_Bool isSuspended = false;
#ifdef WIN32
    /* I found no API to find out if a thread is already suspended by waiting on semaphore */
#else
    teo07_SemObj *semObj = (teo07_SemObj*) ( ((teo07_ThreadObj *)thread)->suspend_sem );

    pthread_mutex_lock (&semObj->mutex);
    isSuspended = (semObj->waiter > 0);
    pthread_mutex_unlock (&semObj->mutex);

#endif
    return isSuspended;
}

/*---------------------------------------------------------------------------*/

void sqlresumethread(teo07_Thread thread,
                     tsp00_ErrTextc errtext,
                     teo07_ThreadErr* ok)
{
#undef MF__
#define MF__ MOD__"sqlresumethread"
  DBG1(( MF__,"resuming thread %d\n", sqlthreadid(thread) ));
  sqlsignalsem( ((teo07_ThreadObj *)thread)->suspend_sem );
  *ok = THR_OK_EO07;
}

/*---------------------------------------------------------------------------*/

void sqljointhread(teo07_Thread thread,
                   tsp00_Int4 *status,
                   tsp00_ErrTextc errtext,
                   teo07_ThreadErr* ok)
{
#undef MF__
#define MF__ MOD__"sqljointhread"

    teo07_ThreadObj *pThreadObj;
#ifndef _WIN32
    teo001_SystemRc rc;
    void *exitStatus;
#endif

    pThreadObj = (teo07_ThreadObj *)thread;

    if ( pThreadObj == NULL )
    {
      *ok = THR_NOT_OK_EO07;
      strcpy(errtext, EO07_ERR_UNKNOWN_THREADID);
      return;
    }

#if defined (KERNEL_LZU)
    MSGD(( INFO_THREAD_JOIN, pThreadObj->thread_id ));
#endif

#ifdef _WIN32
    for(;;)
    {
		/* PTS 1109286 MaO	*/
		WaitForSingleObject(pThreadObj->thread, INFINITE);

        if(!GetExitCodeThread(pThreadObj->thread, status))
        {
            *ok = eo07_GetError(errtext);
            break;
        }
        if (*status == STILL_ACTIVE)
        {
            *ok = eo07_GetError(errtext);
            break;
        }
        else
        {
            *ok = THR_OK_EO07;
            break;
        }
    }

    if ( (pThreadObj->thread != ((HANDLE)0))
      && (pThreadObj->thread != INVALID_HANDLE_VALUE) )
    {
        CloseHandle ( pThreadObj->thread );
    }
#else /* UNIX */

    /**/
    /* LINUX problem: Deadly to put it here.... This leads to dying */
    /* manager threads due to threads that die in _exit()... */
    /* It seems the manager thread does something with the stack of the */
    /* thread that is deadly... */
    /* Instead a sleep(1) was introduced to reduce CPU consumption */
    /**/
    /* rc=pthread_join(pThreadObj->thread, &exitStatus); */

#if (defined LINUX && !defined(HAS_NPTL)) || (defined FREEBSD && defined LINUXTHREADS && !defined FREEBSD_THREADS)
    if ( pThreadObj->StackSize != 0 ) /* PTS 1106187 */
    {
    /*
     * Some words.... PTS 1105262/1105263
     * There was a LINUX specific crash without this YIELD_CALL loop. The linuxthread
     * pthread manager does not synchronize the pthread_join() call with the actions
     * of freeing internal references to the joined thread. Since these references are
     * situated on the pthread stack, freeing this stack results in program crashes depending
     * on the scheduling. Forcing the join to wait until the thread actually was terminating
     * solves part of this problem. Creating another dummy thread solves the synchronization.
     */
    /* PTS 1105678 poll after join not before join... and use kill instead of pthread_kill */
      while ( kill( pThreadObj->thread_id, 0) == 0 )
      {
        sleep(1); /* YIELD_CALL; */ /* let him finsh */
      }
    }
#endif /* (LINUX&&!HAS_NPTL) || (FREEBSD&&LINUXTHREAD&&!FREEBSD_THREADS) */

    rc=pthread_join(pThreadObj->thread, &exitStatus);

    if (rc != 0)
    {
        *ok = THR_NOT_OK_EO07;
        switch(rc)
        {
        case (ESRCH):
        case (EINVAL):
            if (rc == EINVAL)
                *ok = THR_NOT_OK_EO07;
            else
                *ok = THR_NO_DATA_FOUND_EO07;
            if(errtext != NULL)
                strcpy(errtext, EO07_ERR_UNKNOWN_THREADID);
            break;

        case (EDEADLK):
        default:
            *ok = THR_NOT_OK_EO07;
            if(errtext != NULL)
                strcpy(errtext, EO07_ERR_DEADLOCK);
            break;
        }
    }
    else
    {
        *status = (tsp00_Int4)(((char *)exitStatus)-((char *)0));
        *ok = THR_OK_EO07;
    }

#if (defined LINUX && !defined(HAS_NPTL)) || (defined FREEBSD && defined LINUXTHREADS && !defined FREEBSD_THREADS)
    if ( pThreadObj->StackSize != 0 ) /* PTS 1106187 */
    {
      /* PTS 1105262/1105263/1106187 */
      pthread_t DummyThread;
      pthread_attr_t DummyAttr;

      if ( pthread_attr_init(&DummyAttr) != 0 )
      {
        MSGD(( ERR_CREATING_THREAD, "Dummy", errno ));
        DO_ABORT();
      }
      if ( pthread_attr_setdetachstate(&DummyAttr, PTHREAD_CREATE_DETACHED) != 0 )
      {
        MSGD(( ERR_CREATING_THREAD, "Dummy", errno ));
        DO_ABORT();
      }

      if ( pthread_create( &DummyThread,
                            NULL,
                            eo07_DummyPthreadEntry,
                            (void *)&DummyThread ) != 0 )
      {
        MSGD(( ERR_CREATING_THREAD, "Dummy", errno ));
        DO_ABORT();
      }

      eo07_NoheapFree(pThreadObj->StackAddr, pThreadObj->StackSize);
    }
#endif /* (LINUX&&!HAS_NPTL) || (FREEBSD&&LINUXTHREAD&&!FREEBSD_THREADS) */
#endif

    DeleteResources( pThreadObj, STATE_THREAD_ENDED, STATE_PARENT_DETACHED );

} /* sqljointhread */

/*---------------------------------------------------------------------------*/

int sqlthreadalive( teo07_Thread thread,
                    tsp00_ErrTextc errtext,
                    teo07_ThreadErr* ok)
{
#undef MF__
#define MF__ MOD__"sqlthreadalive"

    teo07_ThreadObj *pThreadObj;
    teo001_SystemRc rc;

    pThreadObj = (teo07_ThreadObj *)thread;
    if ( errtext )
    {
        errtext[0] = 0;
    }
    *ok = THR_OK_EO07;

#ifdef _WIN32
    rc = ((WaitForSingleObject(pThreadObj->thread, 0) == WAIT_TIMEOUT) ? 0 : 1);
#else
    rc = pthread_kill( pThreadObj->thread, 0);
#endif

    return ( rc == 0 );
}

/*---------------------------------------------------------------------------*/

void sqlonce (tsp00_Int4* initialized,
              teo07_ThreadCallback callback,
              void* arg)
{
#undef MF__
#define MF__ MOD__"sqlonce"

#ifdef _WIN32
    static LONG lock = 1;

    while(InterlockedExchange(&lock,0) == 0)
    {
        Sleep(50);
    }
    if (*initialized == 1)
    {
        lock = 1; /* release the lock just acquired */
        return;
    }

    callback(arg);
    *initialized = 1;
    lock = 1; /* release the lock just acquired */
#else
    int rc;
    if ( 0 == pthread_mutex_lock( &onceMutex_eo07 ) )
    {
        if (*initialized == 1)
        {
            pthread_mutex_unlock( &onceMutex_eo07 );
            return;
        }

        callback(arg);
        *initialized = 1;
        pthread_mutex_unlock( &onceMutex_eo07 );
    }
#endif
} /* sqlonce */

/*---------------------------------------------------------------------------*/

/*
 Mutex
 */

void sqlcreatemutex(teo07_Mutex *ppMutex)
{
#undef MF__
#define MF__ MOD__"sqlcreatemutex"
    teo07_Mutex pMutex;

    if( ppMutex == NULL )
    {
        MSGD(( ERR_INVALID_PARAMETER, "createMutex" ));
        DO_ABORT();
    }

    if ( ALLOC_MEM_EO57( (void **)ppMutex, sizeof(struct _teo07_Mutex) ) != NO_ERROR_EO001 )
    {
      MSGD(( ERR_CANT_ALLOC_HEAP_MEMORY, errno ));
      DO_ABORT();
    }
    pMutex = *ppMutex;
    SAPDB_memset(pMutex, 0, sizeof(struct _teo07_Mutex));

#ifdef _WIN32
    InitializeCriticalSection(&(pMutex->mutex));
#else
    pthread_mutex_init(&(pMutex->mutex), NULL);
#endif
} /* sqlcreatemutex */

/*---------------------------------------------------------------------------*/

void sqldestroymutex(teo07_Mutex *ppMutex)
{
#undef MF__
#define MF__ MOD__"sqldestroymutex"

    teo07_Mutex pMutex;

    if( ppMutex == NULL )
    {
        MSGD(( ERR_INVALID_PARAMETER, "destroyMutex" ));
        DO_ABORT();
    }

    pMutex = *ppMutex;

    if ( pMutex != NULL )
    {
#ifdef _WIN32
        DeleteCriticalSection(&(pMutex->mutex));
#else
        pthread_mutex_destroy(&(pMutex->mutex));
#endif
        FREE_MEM_EO57(pMutex);
        *ppMutex = NULL;
    }
} /* sqldestroymutex */

/*---------------------------------------------------------------------------*/

void sqlbeginmutex(teo07_Mutex *ppMutex)
{
#undef MF__
#define MF__ MOD__"sqlbeginmutex"
    teo07_Mutex pMutex;

    if( ppMutex == NULL )
    {
        MSGD(( ERR_INVALID_PARAMETER, "beginMutex" ));
        DO_ABORT();
    }

    pMutex = *ppMutex;

    if ( pMutex == NULL )
    {
        MSGD(( ERR_INVALID_PARAMETER, "beginMutex Value" ));
        DO_ABORT();
    }

    /* this prevents the thread to deadlock itselves */
    if( pMutex->owning_thread != sqlgetthreadid())
    {
#ifdef TRACEX
        printf("beginmutex %#x %ld %ld, %ld\n",
            pMutex,
            sqlgetthreadid(),
            pMutex->owning_thread,
            pMutex->lock_count);
#endif

#ifdef _WIN32
        EnterCriticalSection(&(pMutex->mutex));
#else
        if ( 0 != pthread_mutex_lock(&(pMutex->mutex)) )
        {
            MSGD(( ERR_INVALID_PARAMETER, "Mutex Pointer" ));
            DO_ABORT();
        }
#endif
        pMutex->lock_count = 1;
        pMutex->owning_thread = sqlgetthreadid();
    }
    else
    {
        pMutex->lock_count++;
    }
} /* sqlbeginmutex */

/*---------------------------------------------------------------------------*/

int sqlissetmutex(teo07_Mutex *ppMutex)
{
    return ( (*ppMutex)->lock_count != 0 );
} /* sqlissetmutex */

/*---------------------------------------------------------------------------*/

tsp00_Int4 sqltrybeginmutex(teo07_Mutex *ppMutex)
{
#undef MF__
#define MF__ MOD__"sqltrybeginmutex"
    teo001_SystemRc rc=0;
    teo07_Mutex pMutex;

    if( ppMutex == NULL )
    {
        MSGD(( ERR_INVALID_PARAMETER, "trybeginMutex" ));
        DO_ABORT();
    }

    pMutex = *ppMutex;

    if ( pMutex == NULL )
    {
        MSGD(( ERR_INVALID_PARAMETER, "trybeginMutex Value" ));
        DO_ABORT();
    }

    /* this prevents the thread to deadlock itselves */
    /* Windows NT doesn't deadlock sections but it works also */
    if( pMutex->owning_thread != sqlgetthreadid())
    {
#ifdef TRACEX
        printf("beginmutex %#x %ld %ld, %ld\n",
            pMutex,
            sqlgetthreadid(),
            pMutex->owning_thread,
            pMutex->lock_count);
#endif

        if( (pMutex->owning_thread) == 0 )
        {
#ifdef _WIN32
        /* TryEnterCriticalSection not yet implemented on Windows 95 */
        /* so uses this to emulate TryEnterCriticalSection */
            EnterCriticalSection(&(pMutex->mutex));
            if (pMutex->owning_thread != 0)
            {
                LeaveCriticalSection(&(pMutex->mutex));
                rc = 1;
            }
#else
            rc = (pthread_mutex_trylock(&(pMutex->mutex)) != 0);
#endif
        }

        if ( rc == 0 )
        {
            pMutex->lock_count = 1;
            pMutex->owning_thread = sqlgetthreadid();
        }
    }
    else
    {
        pMutex->lock_count++;
    }
    return(rc);
} /* sqltrybeginmutex */

/*---------------------------------------------------------------------------*/

void sqlendmutex(teo07_Mutex *ppMutex)
{
#undef MF__
#define MF__ MOD__"sqlendmutex"

    teo07_Mutex pMutex;

    if ( ppMutex == NULL )
    {
        MSGD(( ERR_INVALID_PARAMETER, "endMutex" ));
        DO_ABORT();
    }

    pMutex = *ppMutex;

    if( pMutex == NULL )
    {
        MSGD(( ERR_INVALID_PARAMETER, "endMutex Value" ));
        DO_ABORT();
    }

    if( pMutex->owning_thread == sqlgetthreadid()
     && pMutex->lock_count > 1 )
    {
        pMutex->lock_count--;
    }
    else
    {
#ifdef TRACEX
        printf("end  mutex %#x %ld %ld, %ld\n",
                pMutex, sqlgetthreadid(), pMutex->owning_thread, pMutex->lock_count);
#endif
        pMutex->lock_count = 0;
        pMutex->owning_thread = 0;
#ifdef _WIN32
        LeaveCriticalSection(&pMutex->mutex);
#else
        pthread_mutex_unlock(&pMutex->mutex);
#endif
    }
} /* sqlendmutex */

/*---------------------------------------------------------------------------*/

/*
 Thread Local Storage
 */

void sqlcreatetls(tsp00_Int4 *phKey,
                  tsp00_ErrTextc errtext,
                  teo07_ThreadErr* ok)
{
#undef MF__
#define MF__ MOD__"sqlcreatetls"

#ifdef _WIN32
  if ((*phKey = TlsAlloc()) == 0xFFFFFFFF)
  {
    *ok = eo07_GetError(errtext);
  }
  else
  {
    *ok = THR_OK_EO07;
  }
#else
  teo001_SystemRc rc;
  *phKey = 0; /* Solaris returns 0 if no threadlib is linked with the Applikation */
  rc = pthread_key_create((pthread_key_t*)phKey, NULL);
  if (rc == ENOMEM)
  {
    *ok = THR_NOT_OK_EO07;
    if(errtext != NULL)
    {
      strcpy(errtext, EO07_ERR_NO_MEM);
    }
  }
  else
  {
    *ok = THR_OK_EO07;
  }
#endif
} /* sqlcreatetls */

/*---------------------------------------------------------------------------*/

void sqldestroytls(tsp00_Int4 hKey,
                   tsp00_ErrTextc errtext,
                   teo07_ThreadErr* ok)
{
#undef MF__
#define MF__ MOD__"sqldestroytls"

#ifdef _WIN32
  if(TlsFree((DWORD)hKey) == 0)
  {
    *ok = eo07_GetError(errtext);
  }
  else
  {
    *ok = THR_OK_EO07;
  }
#else /* UNIX */

  teo001_SystemRc rc;
  rc = pthread_key_delete((pthread_key_t)hKey);
  if (rc != 0)
  {
    *ok = THR_NOT_OK_EO07;
    if(errtext != NULL)
    {
      switch(rc)
      {
      case (EBUSY):
        strcpy(errtext, EO07_ERR_BUSY_KEY);
        break;

      case (EINVAL):
      default:
        strcpy(errtext, EO07_ERR_INVALID_KEY);
        break;
      }
    }
  }
  else
  {
    *ok = THR_OK_EO07;
  }
#endif /* UNIX */
} /* sqldestroytls */

/*---------------------------------------------------------------------------*/

void sqlsettls(tsp00_Int4 hKey,
               void*  pObjPtr,
               tsp00_ErrTextc errtext,
               teo07_ThreadErr* ok)
{
#undef MF__
#define MF__ MOD__"sqlsettls"

#ifdef _WIN32
  if(TlsSetValue((DWORD)hKey, pObjPtr) == 0)
  {
    *ok = eo07_GetError(errtext);
  }
  else
  {
    *ok = THR_OK_EO07;
  }
#else /* UNIX */

  teo001_SystemRc rc;
  rc = pthread_setspecific((pthread_key_t)hKey, pObjPtr);
  if (rc != 0)
  {
    *ok = THR_NOT_OK_EO07;
    if(errtext != NULL)
    {
      switch(rc) {
      case (ENOMEM):
        strcpy(errtext, EO07_ERR_NO_MEM);
        break;

      case (EINVAL):
      default:
        strcpy(errtext, EO07_ERR_INVALID_KEY);
        break;
      }
    }
  }
  else
  {
    *ok = THR_OK_EO07;
  }
#endif /* UNIX */
} /* sqlsettls */

/*---------------------------------------------------------------------------*/

void *sqlgettls(tsp00_Int4 hKey)
{
#undef MF__
#define MF__ MOD__"sqlgettls"

#ifdef _WIN32
  return(TlsGetValue((DWORD)hKey));
#else
  return pthread_getspecific((pthread_key_t)hKey);
#endif
} /* sqlgettls */

/*---------------------------------------------------------------------------*/

void sqlcreatesem (teo07_ThreadSemaphore* sem,
                   tsp00_Int4 initval,
                   tsp00_ErrTextc errtext,
                   teo07_ThreadErr* ok)
{
#undef MF__
#define MF__ MOD__"sqlcreatesem"
#ifdef _WIN32
    *sem = CreateSemaphore (NULL, initval, LONG_MAX, NULL);
    if (*sem == INVALID_HANDLE_VALUE)
    {
        *ok = eo07_GetError(errtext);
    }
    else
    {
        *ok = THR_OK_EO07;
    }
#else /* UNIX */

    teo07_SemObj *semObj = NULL;

    if ( ALLOC_MEM_EO57( (void **)&semObj, sizeof (teo07_SemObj)) != NO_ERROR_EO001 )
    {
        *ok = THR_NOT_OK_EO07;
        if (errtext != NULL)
        {
            strcpy(errtext, EO07_ERR_NO_MEM);
        }
        return;
    }
    semObj->token = initval;
    semObj->waiter = 0;
    (void)pthread_mutex_init (&semObj->mutex, NULL);
    (void)pthread_cond_init (&semObj->condition, NULL);
    *sem = semObj;
    *ok = THR_OK_EO07;
#endif /* UNIX */
} /* sqlcreatesem */

/*---------------------------------------------------------------------------*/

void sqldestroysem (teo07_ThreadSemaphore sem)
{
#undef MF__
#define MF__ MOD__"sqldestroysem"
#ifdef _WIN32
    if ( sem != ((HANDLE)0)
      && sem != INVALID_HANDLE_VALUE)
    {
      CloseHandle (sem);
    }
#else /* UNIX */

    teo07_SemObj *semObj = (teo07_SemObj*) sem;

    pthread_mutex_destroy (&semObj->mutex);
    pthread_cond_destroy (&semObj->condition);
    FREE_MEM_EO57(sem);

#endif /* UNIX */
} /* sqldestroysem */

/*---------------------------------------------------------------------------*/

void sqlwaitsem (teo07_ThreadSemaphore sem)
{
#undef MF__
#define MF__ MOD__"sqlwaitsem"
#ifdef _WIN32

    WaitForSingleObject (sem, INFINITE);

#else /* UNIX */

    teo07_SemObj *semObj = (teo07_SemObj*) sem;

    if ( 0 != pthread_mutex_lock(&semObj->mutex) )
    {
        MSGD(( ERR_INVALID_PARAMETER, "sem lock mutex pointer" ));
        DO_ABORT();
    }
    while ( semObj->token < 1 )
    {
      ++semObj->waiter;
      if ( EINVAL == pthread_cond_wait (&semObj->condition, &semObj->mutex) )
      {
          MSGD(( ERR_INVALID_PARAMETER, "sem condition or mutex pointer" ));
          DO_ABORT();
      }
      --semObj->waiter;
    }
    --semObj->token;
    if ( 0 != pthread_mutex_unlock (&semObj->mutex) )
    {
        MSGD(( ERR_INVALID_PARAMETER, "sem unlock mutex pointer" ));
        DO_ABORT();
    }

#endif /* UNIX */
} /* sqlwaitsem */

/*---------------------------------------------------------------------------*/

void sqltimedwaitsem (teo07_ThreadSemaphore sem,
                      tsp00_Int4 seconds,
                      teo07_ThreadErr *ok)
{
#undef MF__
#define MF__ MOD__"sqltimedwaitsem"
    if ( seconds < 0 )
    {
        seconds = 0;
    }
    {
#ifdef _WIN32

    DWORD result = WaitForSingleObject (sem, seconds*1000);
    if ( result == WAIT_TIMEOUT )
    {
      *ok = THR_TIMEOUT_EO07;
    }
    else if ( result == WAIT_OBJECT_0 )
    {
      *ok = THR_OK_EO07;
    }
    else
    {
      *ok = THR_NOT_OK_EO07;
    }
#else /* UNIX */

    teo07_SemObj *semObj = (teo07_SemObj*) sem;
    tsp00_Bool        blocked;
    tsp00_Int4          result;
    teo001_SystemRc   rc;
    struct timespec timeout;
#if defined(KERNEL_LZU)
    timeout.tv_sec = KGS->current_time + seconds;
#else
    timeout.tv_sec = time (NULL) + seconds;
#endif
    timeout.tv_nsec = 0;

    if ( 0 != pthread_mutex_lock (&semObj->mutex) )
    {
        MSGD(( ERR_INVALID_PARAMETER, "sem lock mutex pointer" ));
        DO_ABORT();
    }

    if ( seconds > 0 )
    {
        while ( semObj->token < 1 )
        {
          ++semObj->waiter;
          rc = pthread_cond_timedwait (&semObj->condition, &semObj->mutex, &timeout);
          --semObj->waiter;
          if (rc == TIMEOUT_SYSTEM_RC)
          {
            break;
          }
          if ( EINVAL == rc )
          {
              MSGD(( ERR_INVALID_PARAMETER, "sem condition, mutex pointer or timeout invalid" ));
              DO_ABORT();
          }
        }
    }
    if ( semObj->token >= 1 )
    {
      --semObj->token;
      *ok = THR_OK_EO07;
    }
    else
    {
      *ok = THR_TIMEOUT_EO07;
    }
    if ( 0 != pthread_mutex_unlock (&semObj->mutex) )
    {
        MSGD(( ERR_INVALID_PARAMETER, "sem unlock mutex pointer" ));
        DO_ABORT();
    }

#endif /* UNIX */
    }
} /* sqltimedwaitsem */

/*---------------------------------------------------------------------------*/

void sqlsignalsem (teo07_ThreadSemaphore sem)
{
#undef MF__
#define MF__ MOD__"sqlsignalsem"
#ifdef _WIN32

    LONG        previous;

    ReleaseSemaphore (sem, 1, &previous);

#else /* UNIX */

    teo07_SemObj *semObj = (teo07_SemObj*) sem;
    tsp00_Int4         token;

    if ( 0 != pthread_mutex_lock (&semObj->mutex) )
    {
        MSGD(( ERR_INVALID_PARAMETER, "sem lock mutex pointer" ));
        DO_ABORT();
    }
    ++semObj->token;
    if ( semObj->waiter )
    {
      if ( EINVAL == pthread_cond_signal (&semObj->condition) )
      {
          MSGD(( ERR_INVALID_PARAMETER, "sem condition" ));
          DO_ABORT();
      }
    }
    if ( 0 != pthread_mutex_unlock (&semObj->mutex) )
    {
        MSGD(( ERR_INVALID_PARAMETER, "sem unlock mutex pointer" ));
        DO_ABORT();
    }

#endif /* UNIX */
} /* sqlsignalsem */

/*===========================================================================*
 *  LOCAL FUNCTIONS (CODE)                                                   *
 *===========================================================================*/

#ifdef FREEBSD_THREADS
static int curr_proc_counter = 0;
#endif

static void eo07_CreateThread( tsp00_Int4 StackSize,
                               void *StackStartAddress,
                               void *(*proc)(void *),
                               void *arg,
                               tsp00_Int4 flags,
                               teo07_Thread *thread,
                               tsp00_ErrTextc errtext,
                               teo07_ThreadErr *ok)
{
#undef MF__
#define MF__ MOD__"eo07_CreateThread"

  teo07_ThreadObj *pThreadObj = NULL;
#ifndef _WIN32
  char msgBuffer[256];
  teo001_SystemRc rc;
  pthread_attr_t attr;
#endif

  *ok = THR_OK_EO07;
  if ( proc == NULL )
  {
      *ok = THR_NOT_OK_EO07;
      if (errtext != NULL)
          strcpy(errtext, EO07_ERR_PROCADDRESS_NULL);
      return;
  }

  if ( ALLOC_MEM_EO57( (void **)&pThreadObj, sizeof(teo07_ThreadObj) ) != NO_ERROR_EO001 )
  {
      *ok = THR_NOT_OK_EO07;
      if (errtext != NULL)
        strcpy(errtext, EO07_ERR_NO_MEM);
      return;
  }

  pThreadObj->proc = proc;
  pThreadObj->arg  = arg;
  pThreadObj->thread_id = 0;
  pThreadObj->state = 0;
#ifdef _WIN32
  /* System default minimum stack size 1 MByte */
  pThreadObj->givenStackSize = ( (StackSize >= (1024*1024)) ? StackSize : (1024*1024) );
#else
  if ( StackSize > 0 && StackStartAddress != 0 )
  {
      if ( StackSize < sqlget_thread_min_stack() )
      {
          StackSize = sqlget_thread_min_stack();
      }
      pThreadObj->givenStackSize = 0; /* given stack already allocated outside... */
  }
  else
  {
      if ( StackSize > 0 )
      {
          /* System default minimum stack size may be os configuration dependend */
          if ( StackSize < sqlget_thread_min_stack() )
          {
              StackSize = sqlget_thread_min_stack();
          }
      }
      if ( StackSize < sqlget_thread_min_stack() )
      {
          pThreadObj->givenStackSize = sqlget_thread_min_stack();
      }
      else
      {
          pThreadObj->givenStackSize = StackSize;
      }

      if ( pThreadObj->givenStackSize <= 0 ) pThreadObj->givenStackSize = (1024*1024);
  }
#endif

  *thread = (teo07_Thread)pThreadObj;

  pThreadObj->start_suspended = ((flags & THR_CREATE_SUSPENDED_EO07) != 0);

  sqlcreatesem(&(pThreadObj->suspend_sem), 0, errtext, ok);
  if ( THR_OK_EO07 != *ok )
  {
      FREE_MEM_EO57(pThreadObj);
      *thread = NULL;
      return;
  }

#ifdef _WIN32
#if defined(KERNEL_LZU)
  if ( pThreadObj->givenStackSize > 0 && !RTE_PseudoAllocStack(pThreadObj->givenStackSize, 0) )
  {
      SetLastError(ERROR_NOT_ENOUGH_MEMORY);
      *ok = eo07_GetError(errtext);
  }
  else
#endif
  {
      pThreadObj->thread = CreateThread( NULL,
                                StackSize,
                                (LPTHREAD_START_ROUTINE)eo07_Win32threadEntry,
                                (void *)pThreadObj,
                                CREATE_SUSPENDED,
                                &pThreadObj->thread_id);
      if (pThreadObj->thread == NULL)
      {
        *ok = eo07_GetError(errtext);
#if defined(KERNEL_LZU)
        if ( pThreadObj->givenStackSize > 0 ) RTE_PseudoFreeStack(pThreadObj->givenStackSize, 0);
#endif
      } 
      else
      {
        if ( ResumeThread(pThreadObj->thread) == (DWORD)-1 )
        {
          *ok = eo07_GetError(errtext);
#if defined(KERNEL_LZU)
          if ( pThreadObj->givenStackSize > 0 ) RTE_PseudoFreeStack(pThreadObj->givenStackSize, 0);
#endif
        }
      }
  }

#else

  /* UNIX specific part */
#if defined(KERNEL_LZU)
  if ( pThreadObj->givenStackSize > 0 
    && !RTE_PseudoAllocStack(pThreadObj->givenStackSize, 0) )
  { 
      strcpy(errtext, EO07_ERR_NO_MEM);
      FREE_MEM_EO57(pThreadObj);
      *thread = NULL;
      *ok = THR_NOT_OK_EO07;
      return;
  }
#endif

  rc = pthread_attr_init(&attr);
  if ( rc != 0 )
  {
      switch( rc )
      {
      case ENOMEM:
          strcpy(errtext, EO07_ERR_NO_MEM);
          break;
      default:
          SAPDB_sprintf(errtext, sizeof(tsp00_ErrTextc), "attr_init:%d bad library version?");
          break;
      }
      FREE_MEM_EO57(pThreadObj);
      *thread = NULL;
      *ok = THR_NOT_OK_EO07;
#if defined(KERNEL_LZU)
      if ( pThreadObj->givenStackSize > 0 ) RTE_PseudoFreeStack(pThreadObj->givenStackSize, 0);
#endif
      return;
  }

  if ( (flags & THR_CREATE_PROCESS_SCOPE_EO07) != 0 )
  {
#ifdef HAVE_PTHREAD_SCOPE_PROCESS
    rc = pthread_attr_setscope(&attr, PTHREAD_SCOPE_PROCESS);
    if ( rc != 0 )
    {
        SAPDB_sprintf(msgBuffer, sizeof(msgBuffer), "pthread_attr_setscope(PTHREAD_SCOPE_PROCESS) failed to set process scope:%d", rc);
        MSGD((INFO_TEST_WILDCARD, msgBuffer));
    }
#else
    SAPDB_sprintf(msgBuffer, sizeof(msgBuffer), "pthread_attr_setscope(PTHREAD_SCOPE_PROCESS) not supported");
    MSGD((INFO_TEST_WILDCARD, msgBuffer));
#endif
  }
  else
  {
#ifdef PTHREAD_SCOPE_SYSTEM
    rc = pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
    if ( rc != 0 )
    {
        SAPDB_sprintf(msgBuffer, sizeof(msgBuffer), "No multiprocessor scaling expected... pthread_attr_setscope(PTHREAD_SCOPE_SYSTEM) failed:%d", rc);
        MSGD((INFO_TEST_WILDCARD, msgBuffer));
    }
#else
    SAPDB_sprintf(msgBuffer, sizeof(msgBuffer), "No multiprocessor scaling expected... pthread_attr_setscope(PTHREAD_SCOPE_SYSTEM) not supported");
    MSGD((INFO_TEST_WILDCARD, msgBuffer));
#endif
  }

  if ( (flags & THR_CREATE_DETACHED_EO07) == 0 )
  {
#if defined(PTHREAD_CREATE_UNDETACHED) || defined(PTHREAD_CREATE_JOINABLE)

#if   defined(PTHREAD_CREATE_UNDETACHED)
    rc = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_UNDETACHED);
#else
    rc = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
#endif
    if ( rc != 0 )
    {
        SAPDB_sprintf(msgBuffer, sizeof(msgBuffer), "Join problem expected... pthread_attr_setdetachstate(PTHREAD_CREATE_JOINABLE) failed:%d", rc);
        MSGD((INFO_TEST_WILDCARD, msgBuffer));
    }

#else
    SAPDB_sprintf(msgBuffer, sizeof(msgBuffer), "pthread_attr_setdetachstate(PTHREAD_CREATE_JOINABLE) not supported");
    MSGD((INFO_TEST_WILDCARD, msgBuffer));

#endif
  }
  else
  {
    pThreadObj->state |= STATE_PARENT_DETACHED;
#ifdef PTHREAD_CREATE_DETACHED
    rc = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if ( rc != 0 )
    {
        SAPDB_sprintf(msgBuffer, sizeof(msgBuffer), "Resource leak expected... pthread_attr_setdetachstate(PTHREAD_CREATE_DETACHED) failed:%d", rc);
        MSGD((INFO_TEST_WILDCARD, msgBuffer));
    }
#else
    MSGD((INFO_TEST_WILDCARD, "Resource leak expected... pthread_attr_setdetachstate(PTHREAD_CREATE_DETACHED) not supported"));
#endif
  }

#if SAPDB_WITH_EXCEPTION_SAFE_COROUTINES > 0
#if defined(LINUX) && !defined(HAS_NPTL)
  pThreadObj->threadLocalErrno = 0;
  pThreadObj->pThreadLocalErrno = &pThreadObj->threadLocalErrno;
  pThreadObj->threadLocalHErrno = 0;
  pThreadObj->pThreadLocalHErrno = &pThreadObj->threadLocalHErrno;
  SAPDB_memset(pThreadObj->libcSpecificTlsKeys, 0, sizeof(int)*RTE_MAXIMUM_LIBC_SPECIFIC_KEYS);
#  endif /* LINUX&&!HAS_NPTL */
  pThreadObj->tlsKeys = (void ***)0;
#endif

#if (defined (LINUX) && !defined(HAS_NPTL)) || (defined (FREEBSD) && defined (LINUXTHREADS) && !defined FREEBSD_THREADS)
  pThreadObj->StackAddr = NULL;
  pThreadObj->StackSize = 0;
#endif /* (LINUX&&!HAS_NPTL) || (FREEBSD&&LINUXTHREAD&&!FREEBSD_THREADS) */
  if ( StackSize > 0 )
  {
#if (defined (LINUX) && !defined(HAS_NPTL)) || (defined (FREEBSD) && defined (LINUXTHREADS) && !defined FREEBSD_THREADS)
      /* PTS 1105262/1105263 */
      /*
       * With linuxthreads stack size is variable, but other than in the standard
       * implementation if stacksize differs, stackaddr must be set from outside...
       * StackAddr given must point above the Stack...
       */
      StackSize = ((StackSize + 4095)&(~4095)); /* round up */

      if ( !StackStartAddress )
      {
          pThreadObj->StackAddr = (char *)eo07_NoheapMalloc( StackSize );
      }
      else
      {
          pThreadObj->StackAddr = StackStartAddress;
      }
      pThreadObj->StackSize = StackSize;
      if ( pThreadObj->StackAddr == NULL )
      {
          MSGD(( ERR_CANT_ALLOC_HEAP_MEMORY, errno ));
          DO_ABORT();
      }
#if defined(_POSIX_THREAD_ATTR_STACKSIZE)
      /* PTS 1130316: first try the library function, then the workaround (workaround did not work on RedHat/IA64 */
      if ( EINVAL == pthread_attr_setstacksize(&attr,StackSize)
        || EINVAL == pthread_attr_setstackaddr(&attr,pThreadObj->StackAddr + StackSize ) )
#endif
      {
        /* PTS 1109531 Workaround due to bug (4MB Limit) in pthread_attr_setstacksize() pthread-0.9.so */
        attr.__stacksize = StackSize;
        attr.__stackaddr = pThreadObj->StackAddr + StackSize;
        attr.__stackaddr_set = 1;
      }
#elif defined (LINUX) && defined(HAS_NPTL)
      /* Non Single Unix Extension pthread_attr_setstack available... */
      if ( StackStartAddress )
      {
          rc = pthread_attr_setstack(&attr, StackStartAddress, StackSize);
          if ( rc != 0 )
          {
              SAPDB_sprintf(msgBuffer
                    , sizeof(msgBuffer)
                    , "pthread_attr_setstack(%p, 0x%x) failed:%d"
                    , ((char *)StackStartAddress)+StackSize, StackSize
                    , rc);
              MSGD((ERR_TEST_WILDCARD, msgBuffer));
              SAPDB_strncat_zero_terminated(errtext, msgBuffer, sizeof(tsp00_ErrText)+1);
              FREE_MEM_EO57(pThreadObj);
              *thread = NULL;
              *ok = THR_NOT_OK_EO07;
#if defined(KERNEL_LZU)
              if ( pThreadObj->givenStackSize > 0 ) RTE_PseudoFreeStack(pThreadObj->givenStackSize, 0);
#endif
              return;
          }
      }
      else
      {
          rc = pthread_attr_setstacksize(&attr, StackSize);
          if ( rc != 0 )
          {
              SAPDB_sprintf(msgBuffer, sizeof(msgBuffer), "pthread_attr_setstacksize(%ld) failed:%d", StackSize, rc);
              MSGD((ERR_TEST_WILDCARD, msgBuffer));
              SAPDB_strncat_zero_terminated(errtext, msgBuffer, sizeof(tsp00_ErrText)+1);
              FREE_MEM_EO57(pThreadObj);
              *thread = NULL;
              *ok = THR_NOT_OK_EO07;
#if defined(KERNEL_LZU)
              if ( pThreadObj->givenStackSize > 0 ) RTE_PseudoFreeStack(pThreadObj->givenStackSize, 0);
#endif
              return;
          }
      }
#else
      rc = pthread_attr_setstacksize(&attr, StackSize);
      if ( rc != 0 )
      {
          SAPDB_sprintf(msgBuffer, sizeof(msgBuffer), "pthread_attr_setstacksize(%ld) failed:%d", StackSize, rc);
          MSGD((ERR_TEST_WILDCARD, msgBuffer));
          SAPDB_strncat_zero_terminated(errtext, msgBuffer, sizeof(tsp00_ErrText)+1);
          FREE_MEM_EO57(pThreadObj);
          *thread = NULL;
          *ok = THR_NOT_OK_EO07;
#if defined(KERNEL_LZU)
          if ( pThreadObj->givenStackSize > 0 ) RTE_PseudoFreeStack(pThreadObj->givenStackSize, 0);
#endif
          return;
      }

      if ( StackStartAddress )
      {
#ifdef WANT_HIGHEST_ADDRESS_AS_STACK_ADDRESS
          StackStartAddress = ((char *)StackStartAddress)+StackSize;
#endif
          rc = pthread_attr_setstackaddr(&attr, StackStartAddress);
          if ( rc != 0 )
          {
              SAPDB_sprintf(msgBuffer, sizeof(msgBuffer), "pthread_attr_setstackaddr(%p) failed:%d", StackStartAddress, rc);
              MSGD((ERR_TEST_WILDCARD, msgBuffer));
              SAPDB_strncat_zero_terminated(errtext, msgBuffer, sizeof(tsp00_ErrText)+1);
              FREE_MEM_EO57(pThreadObj);
              *thread = NULL;
              *ok = THR_NOT_OK_EO07;
#if defined(KERNEL_LZU)
              if ( pThreadObj->givenStackSize > 0 ) RTE_PseudoFreeStack(pThreadObj->givenStackSize, 0);
#endif
              return;
          }
      }
#endif /* (LINUX&&!HAS_NPTL) || (FREEBSD&&LINUXTHREAD&&!FREEBSD_THREADS) */
  }

#ifdef USE_EO07_GET_SEQUENCE_NUMBER_FOR_THREAD_ID
  pThreadObj->thread_id = eo07_GetSequenceNumber();
#else
  pThreadObj->thread_id == 0;
#endif /* USE_EO07_GET_SEQUENCE_NUMBER_FOR_THREAD_ID */

  rc = pthread_create(&pThreadObj->thread, &attr, eo07_PthreadEntry, (void *)pThreadObj);
  if ( rc == 0 )
  {
      rc = pthread_attr_destroy(&attr);
      if ( rc != 0 )
      {
          SAPDB_sprintf(msgBuffer, sizeof(msgBuffer), "Resource problem expected... pthread_attr_destroy() failed:%d", rc);
          MSGD((INFO_TEST_WILDCARD, msgBuffer));
      }
  }
  else
  {
    *ok = THR_NOT_OK_EO07;
    if (errtext != NULL)
    {
      switch(rc)
      {
      case ENOMEM:
          strcpy(errtext, EO07_ERR_NO_MEM);
          break;

      case EINVAL:
          strcpy(errtext, EO07_ERR_STACKSIZE);
          break;

      case EAGAIN:
      default:
          strcpy(errtext, EO07_ERR_RESOURCE_LIMIT);
          break;
      }
    }
  }

#ifdef FREEBSD_THREADS
    curr_proc_counter++;
#endif

  if ( THR_OK_EO07 != *ok )
  {
      sqldestroysem(pThreadObj->suspend_sem);
#if defined(KERNEL_LZU)
      if ( pThreadObj->givenStackSize > 0 ) RTE_PseudoFreeStack(pThreadObj->givenStackSize, 0);
#endif
  }
  else
  {
# ifdef USE_GETSEQUENCE_NP_FOR_THREAD_ID
      /* done twice: here and in PthreadEntry to make sure scheduler will not make a problem */
      /* Callable only after thread exists... */
      pThreadObj->thread_id = (teo07_ThreadId)pthread_getsequence_np(pThreadObj->thread);
# elif !defined(USE_EO07_GET_SEQUENCE_NUMBER_FOR_THREAD_ID)
      volatile teo07_ThreadObj *pTheThreadObj = pThreadObj;
      while ( pTheThreadObj->thread_id == 0 )
      {
          YIELD_CALL;
      }
# endif
  }
#endif /* End of UNIX specific part */

  if ( *ok != THR_OK_EO07 )
  {
#ifdef _WIN32
      if ( (pThreadObj->thread != ((HANDLE)0))
        && (pThreadObj->thread != INVALID_HANDLE_VALUE) )
      {
          CloseHandle ( pThreadObj->thread );
      }
#endif
      FREE_MEM_EO57(pThreadObj);
      *thread = NULL;
  }

} /* eo07_CreateThread */

#ifdef _WIN32
static teo07_ThreadErr eo07_GetError(tsp00_ErrTextc errtext)
{
#undef MF__
#define MF__ MOD__"eo07_GetError"
  LPVOID lpMsgBuf;
  DWORD err1 = GetLastError();
  DWORD len;

  len = FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL, err1, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                       (LPTSTR)&lpMsgBuf,
                       0,
                       NULL);
  if (len != 0)
  {
    strncpy(errtext, lpMsgBuf, sizeof(tsp00_ErrText));
    errtext[sizeof(tsp00_ErrText)] = '\0';
    LocalFree(lpMsgBuf);
  }
  else
  {
    DWORD err2;
    UCHAR szTmp[80];
    err2 = GetLastError();
    sprintf(szTmp, __FILE__" FormatMessage failed (%ld)\x0d\x0a GetLastError (%ld)",
        err2, err1);
    FatalAppExit(0, szTmp);
  }
  return(THR_NOT_OK_EO07);
} /* eo07_GetError */

/*---------------------------------------------------------------------------*/

/*
 * Needed to setup TLS key
 */
static DWORD eo07_Win32threadEntry( void *arg )
{
#undef MF__
#define MF__ MOD__"eo07_Win32threadEntry"
    teo07_ThreadObj *pThreadObj;
    DWORD result;

    pThreadObj = (teo07_ThreadObj *)arg;

    if ( TlsSetValue((DWORD)self_key_eo07, pThreadObj) == 0)
    {
#if defined(KERNEL_LZU)
        if ( pThreadObj->givenStackSize > 0 ) RTE_PseudoFreeStack(pThreadObj->givenStackSize, 0);
#endif
        /* delete resource if the parent has been detached */
        DeleteResources( pThreadObj, STATE_PARENT_DETACHED, STATE_THREAD_ENDED );
        return GetLastError();
    }

#if defined (KERNEL_LZU)
    MSGD(( INFO_THREAD_START, pThreadObj->thread_id ));
#endif

    if ( pThreadObj->start_suspended )
    {
        sqlwaitsem(pThreadObj->suspend_sem);
    }

    result = (DWORD)(tsp00_Longuint)(*(pThreadObj->proc))(pThreadObj->arg);
#if defined(KERNEL_LZU)
    if ( pThreadObj->givenStackSize > 0 ) RTE_PseudoFreeStack(pThreadObj->givenStackSize, 0);
#endif

    /* delete resource if the parent has been detached */
    DeleteResources( pThreadObj, STATE_PARENT_DETACHED, STATE_THREAD_ENDED );
    return result;
} /* eo07_Win32threadEntry */
#endif /* _WIN32 */

/*---------------------------------------------------------------------------*/

#ifdef PTHREADS
/*
 * Since pthreads do not support suspend, selfsuspending is done via a mutex
 */
static void *eo07_PthreadEntry( void *arg )
{
#undef MF__
#define MF__ MOD__"eo07_PthreadEntry"
    void *result;
    teo001_SystemRc rc;
    teo07_ThreadObj *pThreadObj;

    pThreadObj = (teo07_ThreadObj *)arg;

    rc = pthread_setspecific((pthread_key_t)self_key_eo07, pThreadObj);
    if ( rc != 0 )
    {
#if defined(KERNEL_LZU)
        if ( pThreadObj->givenStackSize > 0 ) RTE_PseudoFreeStack(pThreadObj->givenStackSize, 0);
#endif
        /* delete resource if the parent has been detached */
        DeleteResources( pThreadObj, STATE_PARENT_DETACHED, STATE_THREAD_ENDED );
        return (void *)rc;
    }

# ifdef USE_GETTID_FOR_THREAD_ID
    pThreadObj->thread_id = (teo07_ThreadId)syscall(__NR_gettid);
# endif

# ifdef USE_GETSEQUENCE_NP_FOR_THREAD_ID
      /* Callable only after thread exists... */
    pThreadObj->thread_id = (teo07_ThreadId)pthread_getsequence_np(pThreadObj->thread);
# endif /* USE_GETSEQUENCE_NP_FOR_THREAD_ID */

# ifdef USE_CLONE_PID_FOR_THREAD_ID
    /* PTS 1105217 */
    pThreadObj->thread_id = (teo07_ThreadId)getpid();
# endif /* USE_CLONE_PID_FOR_THREAD_ID */

#if defined (KERNEL_LZU)
    MSGD(( INFO_THREAD_START, pThreadObj->thread_id ));
#endif

    if ( pThreadObj->start_suspended )
    {
        sqlwaitsem(pThreadObj->suspend_sem);
    }

    result = (*(pThreadObj->proc))(pThreadObj->arg);
#if defined(KERNEL_LZU)
    if ( pThreadObj->givenStackSize > 0 ) RTE_PseudoFreeStack(pThreadObj->givenStackSize, 0);
#endif

    /* delete resource if the parent has been detached */
    DeleteResources( pThreadObj, STATE_PARENT_DETACHED, STATE_THREAD_ENDED );
    return result;
} /* eo07_PthreadEntry */

/*---------------------------------------------------------------------------*/

#ifdef USE_EO07_GET_SEQUENCE_NUMBER_FOR_THREAD_ID

static teo07_ThreadId eo07_GetSequenceNumber()
{
    teo07_ThreadId id;

    (void)pthread_mutex_lock(&threadIdMutex_eo07);
    id = nextThreadId_eo07++;
    (void)pthread_mutex_unlock(&threadIdMutex_eo07);
    return id;
}

#endif /* USE_EO07_GET_SEQUENCE_NUMBER_FOR_THREAD_ID */

#endif /* PTHREADS */

/*---------------------------------------------------------------------------*/

#if (defined LINUX && !defined(HAS_NPTL)) || (defined FREEBSD && defined LINUXTHREADS && !defined FREEBSD_THREADS)
/* PTS 1105262/1105263 */
static void *
eo07_NoheapMalloc(size_t size)
{
    caddr_t ra;

    if ((ra = (caddr_t)mmap((caddr_t) 0, size,
                   (PROT_READ | PROT_WRITE), MAP_PRIVATE|MAP_ANONYMOUS,
                   -1, 0)) == (caddr_t)-1) {
        ra = NULL;
    }
    else
    {
	memset(ra, 0, size);
    }

    return((void *)ra);
}

/*---------------------------------------------------------------------------*/

static void
eo07_NoheapFree(void *ptr, size_t size)
{
    if (ptr == NULL) return;

    if (munmap((caddr_t)ptr, size))
        perror("munmap");
/*
 * To test on any program using the already freed memory, use the mprotect() call
 * below as alternative.
 *

  if ( mprotect(ptr, size, PROT_NONE) )
	perror("mprotect");

 */
}

/*---------------------------------------------------------------------------*/

static void *
eo07_DummyPthreadEntry( void *arg )
{
  return arg;
}
#endif /* (LINUX&&!HAS_NPTL) || (FREEBSD&&LINUXTHREAD&&!FREEBSD_THREADS) */

#ifdef LINUXTHREAD_CHECKS

/*
  This thread test on linuxthread behaviour (process id mismatch...)
 */
static void *
eo07_TestThreadEntry( void *pMainPid )
{
    pid_t myPid;

    myPid = getpid();

    return ( myPid != *(pid_t *)pMainPid ? pMainPid : (void *)0 );
}

/*
  Test for linux thread implementation bug workarounds...
 */
static int eo07_TestOnLinuxthreadBehaviour()
{
    pthread_t testThread;
    pthread_attr_t testThreadAttr;
    int rc;
    void *testThreadExitStatus;
    pid_t myPid = getpid();

    rc = pthread_attr_init(&testThreadAttr);
    if ( rc != 0 )
    {
        MSGD(( ERR_CREATING_THREAD, "testThread attr_init", rc ));
        return false;
    }

    rc = pthread_attr_setdetachstate(&testThreadAttr, PTHREAD_CREATE_JOINABLE);
    if ( rc != 0 )
    {
        MSGD(( ERR_CREATING_THREAD, "testThread joinable", rc ));
        return false;
    }

    rc = pthread_create( &testThread,
                         NULL,
                         eo07_TestThreadEntry,
                         (void *)&myPid );
    if ( rc != 0 )
    {
        MSGD(( ERR_CREATING_THREAD, "testThread create", rc ));
        return false;
    }

    rc = pthread_join(testThread, &testThreadExitStatus);
    if ( rc != 0 )
    {
        MSGD(( ERR_CREATING_THREAD, "testThread join", rc ));
        return false;
    }

    has_linuxthread_implementation = (testThreadExitStatus != 0);
    if ( has_linuxthread_implementation )
    {
        MSGD((INFO_DB_START_VERSION, "Linux thread implementation is Linuxthreads"));
#ifdef HAS_NPTL
        MSGD(( ERR_CREATING_THREAD, "Compiled with NPTL support, but Linuxthreads implementation detected", 0 ));
        if ( getenv("LD_ASSUME_KERNEL") )
        {
            MSGD(( ERR_CREATING_THREAD, "I found LD_ASSUME_KERNEL in your database environment!!!! Unset it before starting x_server...", 0 ));
        }
        return false;
#endif
    }
    else
    {
        MSGD((INFO_DB_START_VERSION, "Linux thread implementation is more POSIX compliant (NPTL)"));
#ifndef HAS_NPTL
        MSGD(( ERR_CREATING_THREAD, "Compiled with Linuxthreads support, but no Linuxthreads implementation detected", 0 ));
        if ( getenv("LD_ASSUME_KERNEL") )
        {
            if ( strcmp("2.2.5", getenv("LD_ASSUME_KERNEL")) == 0 )
            {
                MSGD(( ERR_CREATING_THREAD, "If you run into problems, set LD_ASSUME_KERNEL=2.4.1 in your database environment before starting x_server...", 0 ));
            }
            else if ( strcmp("2.4.1", getenv("LD_ASSUME_KERNEL")) == 0 )
            {
                MSGD(( ERR_CREATING_THREAD, "Even LD_ASSUME_KERNEL=2.4.1 does not help on your LINUX...", 0 ));
            }
            else
            {
                MSGD(( ERR_CREATING_THREAD, "Found unusual setting of LD_ASSUME_KERNEL in your database enviroment...", 0 ));
                MSGD(( ERR_CREATING_THREAD, "If you run into problems, set LD_ASSUME_KERNEL=2.4.1 in your database environment before starting x_server...", 0 ));
            }
        }
        else
        {
            MSGD(( ERR_CREATING_THREAD, "If you run into problems, set LD_ASSUME_KERNEL=2.4.1 in your database environment before starting x_server...", 0 ));
        }
#endif
    }

    /*
       Second test on pthread stack size limitation 
     */
    rc = pthread_attr_init(&testThreadAttr);
    if ( rc != 0 )
    {
        MSGD(( ERR_CREATING_THREAD, "testThread attr_reinit", rc ));
        return false;
    }

#if defined(_POSIX_THREAD_ATTR_STACKSIZE)
    rc = pthread_attr_setstacksize(&testThreadAttr, 9 * 1024 * 1024);
#else
    rc = EINVAL;
#endif

    has_limited_pthread_attr_setstacksize = ( rc == EINVAL );

    if ( !has_limited_pthread_attr_setstacksize )
    {
        if ( rc != 0 )
        {
            MSGD(( ERR_CREATING_THREAD, "testThread with 9MByte stack", rc ));
            return false;
        }

        rc = pthread_attr_setdetachstate(&testThreadAttr, PTHREAD_CREATE_JOINABLE);
        if ( rc != 0 )
        {
            MSGD(( ERR_CREATING_THREAD, "testThread joinable", rc ));
            return false;
        }

        rc = pthread_create( &testThread,
                             NULL,
                             eo07_TestThreadEntry,
                             (void *)&myPid );
        if ( rc != 0 )
        {
            MSGD(( ERR_CREATING_THREAD, "testThread create", rc ));
            return false;
        }

        rc = pthread_join(testThread, &testThreadExitStatus);
        if ( rc != 0 )
        {
            MSGD(( ERR_CREATING_THREAD, "testThread join", rc ));
            return false;
        }
    }
    else
    {
#ifdef HAS_NPTL
        MSGD(( ERR_CREATING_THREAD, "Compiled with NPTL support, but stack limitation by pthread_attr_setstacksize reported", rc ));
        return false;
#endif
    }

    if ( has_limited_pthread_attr_setstacksize )
    {
        MSGD((INFO_DB_START_VERSION, "Linux thread implementation cannot set individual thread stack size"));
    }
    else
    {
        MSGD((INFO_DB_START_VERSION, "Linux thread implementation allows setting of individual thread stack size"));
    }

    return true;
}
#endif /* LINUXTHREAD_CHECKS */

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/

#endif /* ( defined(_REENTRANT) && !defined(_WIN32) ) || ( defined(_MT) && defined(_WIN32) ) */
