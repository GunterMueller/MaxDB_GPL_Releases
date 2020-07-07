/*!---------------------------------------------------------------------
  @file           RTESys_Spinlock.h
  @author         JoergM
  @brief          RunTime: Spinlock C_Functions
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
---------------------------------------------------------------------*/



#ifndef RTESYS_SPINLOCK_H
#define RTESYS_SPINLOCK_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/RTE_Types.h"
#include "RunTime/RTE_CompilerFeatures.h" /* for externC and VAR_ARRAY_REF .. */
#include <string.h> /* for memset() */

#if defined(LINUX) && defined(IA64) && defined __INTEL_COMPILER
#include <ia64intrin.h>
#endif

#if defined(HPUX) && defined(HP_IA64)
#include <machine/sys/inline.h>
#define FENCE (_Asm_fence)(_UP_CALL_FENCE | _UP_SYS_FENCE | _DOWN_CALL_FENCE | _DOWN_SYS_FENCE )
#define LOCKED 1 /* spinlock is locked */
#define UNLOCKED 0 /* spinlock is free */
#endif

#if defined(WIN32)
#include <windows.h>
#endif

#if defined(_REENTRANT)

/*
  Plattform specific header file for 'sched_yield()' call
 */
#if defined(OSF1)
#define _XOPEN_SOURCE_EXTENDED
#define _POSIX_C_SOURCE 199506L
#include <pthread.h>
#endif /* OSF1 */

#if defined(AIX) || defined(HPUX) || defined(NMP) || defined(LINUX)
#include <sched.h>
#elif defined(SUN) || defined(SOLARIS)
#ifndef __EXTENSIONS__
#define __EXTENSIONS__ 1
#endif
#include <unistd.h>
#endif

#endif /*_REENTRANT*/

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!--------------------------------------------------------------------
  @brief  Initialize a given Spinlock

  Needed before a spinlock can be used,, but not necessarily inside the program.
  If the spinlock i.e. is found in a shared memory segment, it could have been
  initialized outside already.
  @param          lock [inout] the lock to initialize to unlocked state

 --------------------------------------------------------------------*/

#if defined(HPUX) && !defined(HP_IA64)

/*! returned cacheline aligned HPUX lock value address */
#define RTESys_CachelineAddress(lock_)                \
   (volatile int *) (                                 \
       ((char *)0)                                    \
     + (                                              \
         (                                            \
             (((char *)lock_)-(char *)0)              \
           + (SAPDB_CACHELINE_SIZE - 1)         \
         )                                            \
       &                                              \
         (                                            \
           ~(SAPDB_CACHELINE_SIZE - 1)          \
         )                                            \
       )                                              \
   )
/* Only this platform uses '1' as unlocked state... */
/* CACHELINE_SIZE defined in RTE_Types.h */

#  define RTESys_InitLock(lock_) * RTESys_CachelineAddress(lock_) = 1

#elif defined(LINUX) && defined(S390X)

#  define RTESys_InitLock(lock_) spin_lock_init(lock_)

#else

#  define RTESys_InitLock(lock_) memset((RTE_Lock *)lock_, 0, sizeof(RTE_Lock))

#endif

/*!--------------------------------------------------------------------
  @brief    Test and lock a given Spinlock

  If locking fails, it is the caller responsibility to react (i.e. by giving up timeslice or call again...)
  @param  lock [inout] the lock to test and lock
  @return true if lock was already in use, false if lock was acquired successfully

 --------------------------------------------------------------------*/

#if defined(AIX)
#define RTESys_TestAndLock(lock_) _check_lock(&(*(RTE_Lock *)lock_)[0], 0, 1)

#elif defined(HPUX) && !defined(HP_IA64)
externC int RTESys_AsmTestAndLock( volatile int * lock );
/*
  The assembler routine RTESys_AsmTestAndLock returns 1 if lock was set, and 0 if lock was already set
  Therefore we have to invert the result
 */
#define RTESys_TestAndLock(lock_) \
   ( 1 != RTESys_AsmTestAndLock(RTESys_CachelineAddress(lock_)) )

#elif defined(HPUX) && defined(HP_IA64)
#define RTESys_TestAndLock(lock_)           \
    (                                       \
        _Asm_mov_to_ar                      \
        (                                   \
            (_Asm_app_reg)_AREG_CCV,        \
            (unsigned int)UNLOCKED,         \
            FENCE                           \
        ),                                  \
        0 != _Asm_cmpxchg                   \
            (                               \
                (_Asm_sz)_SZ_W,             \
                (_Asm_sem)_SEM_ACQ,         \
                (RTE_Lock *)lock_,          \
                (unsigned int)LOCKED,       \
                (_Asm_ldhint)_LDHINT_NONE   \
            )                               \
    )

#elif defined(LINUX) && defined(IA64) && defined __INTEL_COMPILER
#define RTESys_TestAndLock(lock_) (_InterlockedCompareExchange_acq ((volatile unsigned int *)lock_, 1, 0) != 0)

#elif defined(LINUX) && defined(S390X)
externC bool RTESys_CTestAndLock( RTE_Lock * lock );
#define RTESys_TestAndLock(lock_) RTESys_CTestAndLock((RTE_Lock *)lock_)

#elif defined(LINUX) && defined(SDBonPPC64)
externC bool RTESys_CTestAndLock( RTE_Lock * lock );
#define RTESys_TestAndLock(lock_) RTESys_CTestAndLock((RTE_Lock *)lock_)

#elif defined(WIN32)
#define RTESys_TestAndLock(lock_) (InterlockedExchange(lock_, 1)!=0)

#else
/* special assembler language implementation (see RTESys_Spinlock.s) */
externC bool RTESys_AsmTestAndLock( RTE_Lock * lock );
#define RTESys_TestAndLock(lock_) RTESys_AsmTestAndLock(lock_)

#endif

/*!--------------------------------------------------------------------
  @brief    Test an lock with update of collision count and lock statistic

  @param  lock [inout] the lock to test and lock
  @param  pCollision [inout] pointer to collision counter
  @param  pLocks [inout] pointer to sucessfull lock counter
  @return true if lock was not acquiered, false if locked now
 --------------------------------------------------------------------*/
externC bool RTESys_TestAndLockWithStatistic( RTE_Lock * lock
                                             ,SAPDB_Int8 *pCollisions
                                             ,SAPDB_Int8 *pLocks );

/*!--------------------------------------------------------------------
  @brief    Lock a given Spinlock

  This routine will block the caller until the lock is available.

  See RTESys_SetLockLoopCount() for how to influence behaviour of this call.
  @param          lock [inout] the lock to lock

 --------------------------------------------------------------------*/

externC void RTESys_Lock( RTE_Lock * lock );

/*!--------------------------------------------------------------------
  @brief    Unlock a given Spinlock

  The spinlock is reset into unlocked state if locked or not locked...
  @param          lock [inout] the lock to unlock

 --------------------------------------------------------------------*/

#if defined(HPUX) && !defined(HP_IA64)
/* If it would not be a routine, the compilers optimizer would be able to garble the coding... */
/* Infact it is just setting the spinlock memory to '1' again... */
externC void RTESys_CUnlock(RTE_Lock *lock);
#define RTESys_Unlock(lock_) RTESys_CUnlock(lock_)

#elif defined(HPUX) && defined(HP_IA64)
#define RTESys_Unlock(lock_)            \
{                                       \
    int old;                            \
    _Asm_mov_to_ar(                     \
        (_Asm_app_reg)_AREG_CCV,        \
        (unsigned int)LOCKED,           \
        FENCE                           \
    );                                  \
    old = _Asm_cmpxchg(                 \
            (_Asm_sz)_SZ_W,             \
            (_Asm_sem)_SEM_REL,         \
            (RTE_Lock *)lock_,          \
            (unsigned int)UNLOCKED,     \
            (_Asm_ldhint)_LDHINT_NONE   \
    );                                  \
}

#elif defined(AIX)
#define RTESys_Unlock(lock_) _clear_lock(&(*lock_)[0], 0)

#elif defined(LINUX) && defined(IA64) && defined __INTEL_COMPILER
#define RTESys_Unlock(lock_) _InterlockedCompareExchange_rel ((volatile unsigned int *)lock_, 0, 1)

#elif defined(LINUX) && defined(S390X)
externC void RTESys_CUnlock(RTE_Lock *lock);
#define RTESys_Unlock(lock_) RTESys_CUnlock((RTE_Lock *)lock_)

#elif defined(LINUX) && defined(SDBonPPC64)
externC void RTESys_CUnlock(RTE_Lock *lock);
#define RTESys_Unlock(lock_) RTESys_CUnlock((RTE_Lock *)lock_)

#elif defined(WIN32)
#define RTESys_Unlock(lock_) InterlockedExchange(lock_, 0)

#else
/* special assembler language implementation (see RTESys_Spinlock.s) */
externC void RTESys_AsmUnlock( RTE_Lock * lock );
#define RTESys_Unlock(lock_) RTESys_AsmUnlock(lock_)

#endif

/*!--------------------------------------------------------------------
  @brief    Giveup current time slice

  This call is in the RTESys_Spinlock module, since it is primarily used to wait in RTESys_Lock() call.
  On some OS this call will return immediatly, if no other thread/process needs the current cpu.
  The length of a time slice is very system dependend.
 --------------------------------------------------------------------*/

#if defined(WIN32)
/*WINDOWS*/
#define RTESys_GiveUpTimeSlice() SleepEx(0, false)

#elif !defined(_REENTRANT)
/*UNIX_SINGLE_THREADED*/
#define RTESys_GiveUpTimeSlice() sleep(0)

#  elif defined(SUN) || defined(SOLARIS)
/*UNIX_MULTI_THREADED ON Solaris*/ 
#define RTESys_GiveUpTimeSlice() yield()

#  else
/*UNIX_MULTI_THREADED NOT ON Solaris*/ 
#define RTESys_GiveUpTimeSlice() sched_yield()

#endif /* RTESys_GiveUpTimeSlice definition */

/*!--------------------------------------------------------------------
  @brief    Set the loop count used for multi processor hot loops

  The caller must choose a intelligent way to find this parameter...
 --------------------------------------------------------------------*/
externC SAPDB_Int4 RTESys_GetLockLoopCount();

#endif  /* RTESYS_SPINLOCK_H */
