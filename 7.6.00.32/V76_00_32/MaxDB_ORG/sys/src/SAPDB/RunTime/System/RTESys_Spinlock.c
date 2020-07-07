/*!---------------------------------------------------------------------
  @file           RTESys_Spinlock.c
  @author         JoergM
  @brief          RunTime: Spinlock C Routines
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


//
//   FreeBSD portions added by Kai Mosebach,
//   For more informations see : http://www.komadev.de/sapdb
//

\endif
---------------------------------------------------------------------*/




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#if defined(_REENTRANT)

/*
  Plattform specific header file for 'sched_yield()' call
 */
#if defined(OSF1)
#define _XOPEN_SOURCE_EXTENDED
#define _POSIX_C_SOURCE 199506L
#include <pthread.h>
#endif /* OSF1 */

#if defined(AIX) || defined(HPUX) || defined(NMP) || defined(LINUX) || defined(FREEBSD)
#include <sched.h>
#endif

#if defined(SUN) || defined(SOLARIS)
#include <unistd.h>
#endif

#endif /*_REENTRANT*/

#include "RunTime/System/RTESys_Spinlock.h"
#include "RunTime/System/RTESys_SystemInfo.h"

#if defined(WIN32)
#  include <windows.h>
#endif /* WIN32 */

#include "RunTime/System/RTESys_AtomicOperation.h"

#if defined(KERNEL_LZU)
#include "RunTime/Tasking/RTETask_Yield.h" /*nocheck*/
#endif /* KERNEL_LZU */

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define MULTI_PROCESS_HOT_LOCK_LOOP_COUNT 30000

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  STRUCTURES, TYPES, UNIONS ...                                            *
 *===========================================================================*/

static SAPDB_Int4 LockLoopCount = MULTI_PROCESS_HOT_LOCK_LOOP_COUNT;

/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

#if defined(HPUX) && !defined(HP_IA64)
/* If it would not be a routine, the compilers optimizer would have a chance to garble the coding... */
/*HPUX && HP_IA64*/ void RTESys_CUnlock(RTE_Lock *lock_)
/*HPUX && HP_IA64*/ {
/*HPUX && HP_IA64*/     *RTESys_CachelineAddress(lock_) = 1;
/*HPUX && HP_IA64*/ }
#endif

#if defined(LINUX) && defined(S390X)
/*LINUX && S390X*/  bool RTESys_CTestAndLock( RTE_Lock * lock )
/*LINUX && S390X*/ {
/*LINUX && S390X*/  unsigned int result, reg;
/*LINUX && S390X*/  __asm__ __volatile("    slr   %0,%0\n" \
/*LINUX && S390X*/             "    basr  %1,0\n" \
/*LINUX && S390X*/             "0:  cs    %0,%1,0(%2)" \
/*LINUX && S390X*/             : "=&d" (result), "=&d" (reg) \
/*LINUX && S390X*/             : "a" (&lock->lock) : "cc", "memory" );
/*LINUX && S390X*/  return !result;
/*LINUX && S390X*/    //return spin_trylock(lock);
/*LINUX && S390X*/ }
/*LINUX && S390X*/
/*LINUX && S390X*/  void  RTESys_CUnlock( RTE_Lock * lock )
/*LINUX && S390X*/ {
/*LINUX && S390X*/  __asm__ __volatile("    xc 0(4,%0),0(%0)\n" \
/*LINUX && S390X*/                         "    bcr 15,0" \
/*LINUX && S390X*/             : : "a" (&lock->lock) : "memory", "cc" );
/*LINUX && S390X*/    //spin_unlock(lock);
/*LINUX && S390X*/ }
#endif /* LINUX && S390X */

#if defined(LINUX) && defined(SDBonPPC64)
/*LINUX && SDBonPPC64*/  bool RTESys_CTestAndLock( RTE_Lock * lock )
/*LINUX && SDBonPPC64*/ {
/*LINUX && SDBonPPC64*/  unsigned int result;
/*LINUX && SDBonPPC64*/  __asm__ __volatile__ ("0:       lwarx   %0,0,%1\n"
/*LINUX && SDBonPPC64*/                        "         cmpwi   0,%0,0\n"
/*LINUX && SDBonPPC64*/                        "         bne     1f\n"
/*LINUX && SDBonPPC64*/                        "         stwcx.  %2,0,%1\n"
/*LINUX && SDBonPPC64*/                        "         bne-    0b\n"
/*LINUX && SDBonPPC64*/                        "         isync\n"
/*LINUX && SDBonPPC64*/                        "1:"      : "=&r" (result)
/*LINUX && SDBonPPC64*/                                 : "r" (lock), "r" (1)
/*LINUX && SDBonPPC64*/                                 : "cr0", "memory");
/*LINUX && SDBonPPC64*/ return result;
/*LINUX && SDBonPPC64*/ }
/*LINUX && SDBonPPC64*/
/*LINUX && SDBonPPC64*/  void  RTESys_CUnlock( RTE_Lock * lock )
/*LINUX && SDBonPPC64*/ {
/*LINUX && SDBonPPC64*/  __asm__ __volatile__("sync     ": : :"memory");
/*LINUX && SDBonPPC64*/    *lock[0] = 0;
/*LINUX && SDBonPPC64*/ }
#endif /* LINUX && SDBonPPC64*/

/*!--------------------------------------------------------------------
   @description    Lock a given spinlock


  On a Multiprocessor maschine the 'best' spinlock count is hard to describe...
  I use the static variable value currently, but there will be space for better ideas...
  The variable can be overridden any time using 'SetLockLoopCount'...
   @argument       lock [in] pointer to spinlock

 --------------------------------------------------------------------*/

void RTESys_Lock( RTE_Lock *lock )
{
    if ( RTESys_TestAndLock( lock ) )
    {
#if defined(WIN32) || defined(_REENTRANT)
        SAPDB_Int4 loop = RTESys_GetLockLoopCount();
        while ( loop-- > 0 )
        {
            if ( !RTESys_TestAndLock( lock ) )
            {
                return;
            }
        }
#endif /* WIN32 || _REENTRANT */
        do
        {
#ifdef KERNEL_LZU
            RTETask_Yield(false);          /* maybe in task group, stay locked to UKT... */
#else
            RTESys_GiveUpTimeSlice(); /* no task groups */
#endif
        } while ( RTESys_TestAndLock(lock) );
    }
}

/*!--------------------------------------------------------------------
  @brief    Set the loop count used for multi processor hot loops

  The caller must choose a intelligent way to find this parameter...
  The value is used in every RTESys_Lock() call, but only on multiprocessor maschines.
  On a single processor maschine , this call will always reset to zero.
  If negativ value is used, the RTESys_Lock() call will not use a hot loop....
  @param loopCount [in] number of hot loops before giving up time slice in RTESys_Lock() call

 --------------------------------------------------------------------*/

void RTESys_SetLockLoopCount(SAPDB_Int4 loopCount)
{
#if defined(WIN32) || defined(_REENTRANT)
    if ( RTESys_NumberOfCPU() <= 1 )
    {
        LockLoopCount = 0;
    }
    else
    {
        LockLoopCount = loopCount;
    }
#else
    LockLoopCount = 0; /* single threaded application always giveup timeslice immediatly */
#endif
}

/*!--------------------------------------------------------------------
  @brief    Test an lock with update of collision count and lock statistic

  @return true if lock was not acquiered, false if locked now
 --------------------------------------------------------------------*/
bool RTESys_TestAndLockWithStatistic( RTE_Lock * lock
                                     ,SAPDB_Int8 *pCollisions
                                     ,SAPDB_Int8 *pLocks )
{
    if ( RTESys_TestAndLock(lock) )
    {
#ifdef RTESYS_HAS_NATIVE_ATOMIC_OPERATION
        RTESys_AtomicModifyInt8(pCollisions, 1);
#else
        /*
          We may loose some increments here, but we cannot use CompareAndExchange since it is
          implemented using exactly this spinlock code leading into endless recursive
          code...
         */
        SAPDB_Int8 collisionCount = RTESys_AtomicReadInt8(pCollisions);
        RTESys_AtomicWriteInt8(pCollisions, collisionCount+1);
#endif
        return true;
    }

    ++(*pLocks);
    return false;
}

/*!--------------------------------------------------------------------
  @brief    Get the loop count used for multi processor hot loops

  On a single process machine or if compiled without thread support always 0 is returned

  @return number of hot loops before giving up time slice in RTESys_Lock() call
 --------------------------------------------------------------------*/
SAPDB_Int4 RTESys_GetLockLoopCount()
{
#if defined(WIN32) || defined(_REENTRANT)
    static int everCalled = 0;
    if ( !everCalled )
    {
        if ( RTESys_NumberOfCPU() <= 1 )
        {
            LockLoopCount = 0;
        }
        everCalled = 1;
    }
#else
    LockLoopCount = 0; /* single threaded application always giveup timeslice immediatly */
#endif /* WIN32 || _REENTRANT */
    return LockLoopCount;
}
/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
