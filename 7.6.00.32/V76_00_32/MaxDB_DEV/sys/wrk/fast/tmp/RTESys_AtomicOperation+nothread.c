/*!
  @file           RTESys_AtomicOperation.c
  @author         JoergM
  @ingroup        Runtime
  @brief          Atomic operation (cmpxchg, atomic_increment, atomic_decrement) implementation

  For SAPDB_Int4, SAPDB_Int8, SAPDB_UInt4, SAPDB_UInt8 and any pointer individual function are exposed to
  prevent casting operations.

  The atomicity of all these calls is possible either if the machine specific implementation has the according
  atomic operations or the memory position is protected with the same spinlock. To decrease the number of collisions,
  the spinlocks used are contained in a spinlock pool, which is defined elsewhere...
  Once the spinlock pool is used (that is the first time any of these functions are called...) its size can no
  longer be modified.

  NOTE FOR THE FINETUNER: If a platform implementation allows to use a machine specific 'cmpxchg' assembly coding,
  the increment/decrement functions must no longer use the spinlocks and vice versa. Otherwise a very severy
  debugging problem occures :-( Everybody synchronizes, but the spinlock does not protect from cmpxchg...

  Best way to handle this is to make sure that first a 'cmpxchg' with implicit memory barrier is found and
  only after that the atomic increment/decrement calls are replaced.

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

#include "RunTime/System/RTESys_AtomicOperation.h"
#include "RunTime/RTE_Types.h"
#include <stdlib.h>
/* LINUX & i386 uses inline assembly */

#if defined(WIN32)
#include <windows.h>
#endif /* WIN32 */

#if defined(AIX)
#include <sys/atomic_op.h>
#endif /* AIX */

#if defined(OSF1)
#  include <alpha/builtins.h>
#endif /*OSF1*/

#if defined LINUX && defined IA64
#  if defined __INTEL_COMPILER
#    include <ia64intrin.h>
#  endif
#endif

#if (defined(SOLARIS) && (!defined(SDB_HAS_SYS_ATOMIC) || !defined(BIT64))) || defined(SVR4)
#  if defined(BIT64)
extern int RTESys_AtomicCmpxchgWithMembarInt4( SAPDB_Int4 *address,
                                               SAPDB_Int4 expectedValue,
                                               SAPDB_Int4 newValuecmpxchg4b,
                                               SAPDB_Int4 *pOldValue);
extern int RTESys_AtomicCmpxchgWithMembarInt8( SAPDB_Int8 *address,
                                               SAPDB_Int8 expectedValue,
                                               SAPDB_Int8 newValuecmpxchg4b,
                                               SAPDB_Int8 *pOldValue);
#  else
/* local dummy implementation for 32Bit */
int RTESys_AtomicCmpxchgWithMembarInt4( SAPDB_Int4 *address,
                                        SAPDB_Int4 expectedValue,
                                        SAPDB_Int4 newValuecmpxchg4b,
                                        SAPDB_Int4 *pOldValue);
int RTESys_AtomicCmpxchgWithMembarInt8( SAPDB_Int8 *address,
                                        SAPDB_Int8 expectedValue,
                                        SAPDB_Int8 newValuecmpxchg4b,
                                        SAPDB_Int8 *pOldValue);
#  endif
#endif /* (SOLARIS&&(!SDB_HAS_SYS_ATOMIC||!BIT64)) || SVR4 */

/* #ifndef KERNEL_LZU */
#include "RunTime/System/RTESys_SystemInfo.h"
#include "RunTime/System/RTESys_Spinlock.h"

static SAPDB_UInt4 AtomicLockPoolLock(void *memoryAddress);
/*! define */
#define RTESys_AtomicLockPoolLock AtomicLockPoolLock
static void AtomicLockPoolUnlock(void *memoryAddress, SAPDB_UInt4 lockIndex);
/*! define */
#define RTESys_AtomicLockPoolUnlock AtomicLockPoolUnlock
/* #endif */ /* KERNEL_LZU */

#if !defined( InterlockedCompareExchange ) && ( ( _MSC_VER >= 1310 ) || defined( __INTEL_COMPILER ) )
#pragma intrinsic( _InterlockedCompareExchange )
/*! define */
#define InterlockedCompareExchange _InterlockedCompareExchange
#endif

#if !defined( InterlockedCompareExchange ) && ( ( _MSC_VER >= 1310 ) || defined( __INTEL_COMPILER ) )
#pragma intrinsic( _InterlockedCompareExchangePointer )
/*! define */
#define InterlockedCompareExchangePointer _InterlockedCompareExchangePointer
#endif

/*=========================================================================================*/
/* Atomic read/write of values */
/*=========================================================================================*/
#if defined(PA20W)

/*
   On HPUX PA-RISC no memory barrier is needed to synchronize with other processors.
   Therefore atomic operations are idnetical with 'normal' operations...
 */
SAPDB_Int8  RTESys_AtomicReadInt8( SAPDB_Int8 * memoryPosition )   { return *memoryPosition; }
/*---*/
SAPDB_UInt8 RTESys_AtomicReadUInt8( SAPDB_UInt8 * memoryPosition ) { return *memoryPosition; }
/*---*/
SAPDB_Int4  RTESys_AtomicReadInt4( SAPDB_Int4 * memoryPosition )   { return *memoryPosition; }
/*---*/
SAPDB_UInt4 RTESys_AtomicReadUInt4( SAPDB_UInt4 * memoryPosition ) { return *memoryPosition; }
/*---*/
void *      RTESys_AtomicReadPointer( void ** memoryPosition )     { return *memoryPosition; }
/*---*/
void RTESys_AtomicWriteInt8( SAPDB_Int8 * memoryPosition, SAPDB_Int8 const newValue )    { *memoryPosition=newValue; }
/*---*/
void RTESys_AtomicWriteUInt8( SAPDB_UInt8 * memoryPosition, SAPDB_UInt8 const newValue ) { *memoryPosition=newValue; }
/*---*/
void RTESys_AtomicWriteInt4( SAPDB_Int4 * memoryPosition, SAPDB_Int4 const newValue )    { *memoryPosition=newValue; }
/*---*/
void RTESys_AtomicWriteUInt4( SAPDB_UInt4 * memoryPosition, SAPDB_UInt4 const newValue ) { *memoryPosition=newValue; }
/*---*/
void RTESys_AtomicWritePointer( void * * memoryPosition, void * const newValue )         { *memoryPosition=newValue; }

#else
/*
  These functions have been introducded to allow consistent atomic access on various types

  With more widespread use of CompareAndExchange and to prevent otherwise needed MemoryBarriers
  the usage was extended to atomic access on shared data!
 */

/*=========================================================================================*/
/* Atomic read values */
/*=========================================================================================*/
SAPDB_Int8 RTESys_AtomicReadInt8( SAPDB_Int8 * memoryPosition )
{
    SAPDB_Int8 result = 0;
    RTESys_CmpxchgInt8(memoryPosition, (SAPDB_Int8)4711, (SAPDB_Int8)4711, &result);
    return result;
}
/*---*/
SAPDB_UInt8 RTESys_AtomicReadUInt8( SAPDB_UInt8 * memoryPosition )
{
    SAPDB_UInt8 result = 0;
    RTESys_CmpxchgUInt8(memoryPosition, (SAPDB_UInt8)4711U, (SAPDB_UInt8)4711U, &result);
    return result;
}
/*---*/
SAPDB_Int4 RTESys_AtomicReadInt4( SAPDB_Int4 * memoryPosition )
{
    SAPDB_Int4 result = 0;
    RTESys_CmpxchgInt4(memoryPosition, (SAPDB_Int4)4711, (SAPDB_Int4)4711, &result);
    return result;
}
/*---*/
SAPDB_UInt4 RTESys_AtomicReadUInt4( SAPDB_UInt4 * memoryPosition )
{
    SAPDB_UInt4 result = 0;
    RTESys_CmpxchgUInt4(memoryPosition, (SAPDB_UInt4)4711, (SAPDB_UInt4)4711, &result);
    return result;
}
/*---*/
void * RTESys_AtomicReadPointer( void ** memoryPosition )
{
    void * result = 0;
    RTESys_CmpxchgPointer(memoryPosition, (void *)(((char *)0) + 4711), (void *)(((char *)0) + 4711), &result);
    return result;
}

/*=========================================================================================*/
/* Atomic write values */
/*=========================================================================================*/
void RTESys_AtomicWriteInt8( SAPDB_Int8 * memoryPosition, SAPDB_Int8 const newValue )
{
    SAPDB_Int8 expectedValue = *memoryPosition; /* dirty read */
    SAPDB_Int8 foundValue = 0;
    while ( !RTESys_CmpxchgInt8(memoryPosition, expectedValue, newValue, &foundValue) )
        expectedValue = foundValue; /* should not loop too often... */
}
/*---*/
void RTESys_AtomicWriteUInt8( SAPDB_UInt8 * memoryPosition, SAPDB_UInt8 const newValue )
{
    SAPDB_UInt8 expectedValue = *memoryPosition; /* dirty read */
    SAPDB_UInt8 foundValue = 0;
    while ( !RTESys_CmpxchgUInt8(memoryPosition, expectedValue, newValue, &foundValue) )
        expectedValue = foundValue; /* should not loop too often... */
}
/*---*/
void RTESys_AtomicWriteInt4( SAPDB_Int4 * memoryPosition, SAPDB_Int4 const newValue )
{
    SAPDB_Int4 expectedValue = *memoryPosition; /* dirty read */
    SAPDB_Int4 foundValue = 0;
    while ( !RTESys_CmpxchgInt4(memoryPosition, expectedValue, newValue, &foundValue) )
        expectedValue = foundValue; /* should not loop too often... */
}
/*---*/
void RTESys_AtomicWriteUInt4( SAPDB_UInt4 * memoryPosition, SAPDB_UInt4 const newValue )
{
    SAPDB_UInt4 expectedValue = *memoryPosition; /* dirty read */
    SAPDB_UInt4 foundValue = 0;
    while ( !RTESys_CmpxchgUInt4(memoryPosition, expectedValue, newValue, &foundValue) )
        expectedValue = foundValue; /* should not loop too often... */
}
/*---*/
void RTESys_AtomicWritePointer( void * * memoryPosition, void * const newValue )
{
    void * expectedValue = *memoryPosition; /* dirty read */
    void * foundValue = 0;
    while ( !RTESys_CmpxchgPointer(memoryPosition, expectedValue, newValue, &foundValue) )
        expectedValue = foundValue; /* should not loop too often... */
}

#endif

/*=========================================================================================*/
/*! Compare and Exchange

   NOTE: since normaly some modification have been made on a structure, these calls IMPLICITLY enforces
   memory barriers, so that all updates are visible.
 */
/*=========================================================================================*/

#if defined(RTESYS_HAS_NATIVE_ATOMIC_OPERATION)

#if defined(WIN32) && !defined(_WIN64)

SAPDB_Bool RTESys_CmpxchgInt4( SAPDB_Int4 *memoryPosition,
                               SAPDB_Int4 const expectedValue,
                               SAPDB_Int4 const newValue,
                               SAPDB_Int4 *pOldValue)
{
	SAPDB_Int4 oldValue;
	_asm {
		mov eax, expectedValue
		mov esi, memoryPosition
		mov ebx, newValue
		lock cmpxchg [esi], ebx
		mov oldValue, eax
	}
    *pOldValue = oldValue;
	return oldValue == expectedValue;
}

SAPDB_Bool RTESys_CmpxchgUInt4( SAPDB_UInt4 *memoryPosition,
                                SAPDB_UInt4 const expectedValue,
                                SAPDB_UInt4 const newValue,
                                SAPDB_UInt4 *pOldValue)
{
	SAPDB_UInt4 oldValue;
	_asm {
		mov eax, expectedValue
		mov esi, memoryPosition
		mov ebx, newValue
		lock cmpxchg [esi], ebx
		mov oldValue, eax
	}
    *pOldValue = oldValue;
	return oldValue == expectedValue;
}

SAPDB_Bool RTESys_CmpxchgInt8( SAPDB_Int8 *memoryPosition,
                               SAPDB_Int8 const expectedValue,
                               SAPDB_Int8 const newValue,
                               SAPDB_Int8 *pOldValue)
{
	SAPDB_Int8 oldValue;
	_asm {
		mov eax, dword ptr expectedValue
		mov edx, dword ptr expectedValue + 4
		mov ebx, dword ptr newValue
		mov ecx, dword ptr newValue + 4
        mov esi, memoryPosition
		lock cmpxchg8b [esi]
		mov dword ptr oldValue, eax
		mov dword ptr oldValue + 4, edx
	}
    *pOldValue = oldValue;
	return oldValue == expectedValue;
}

SAPDB_Bool RTESys_CmpxchgUInt8( SAPDB_UInt8 *memoryPosition,
                                SAPDB_UInt8 const expectedValue,
                                SAPDB_UInt8 const newValue,
                                SAPDB_UInt8 *pOldValue)
{
	SAPDB_UInt8 oldValue;
	_asm {
		mov eax, dword ptr expectedValue
		mov edx, dword ptr expectedValue + 4
		mov ebx, dword ptr newValue
		mov ecx, dword ptr newValue + 4
        mov esi, memoryPosition
		lock cmpxchg8b [esi]
		mov dword ptr oldValue, eax
		mov dword ptr oldValue + 4, edx
	}
    *pOldValue = oldValue;
	return oldValue == expectedValue;
}

SAPDB_Bool RTESys_CmpxchgPointer( void *      *memoryPosition,
                                  void * const expectedValue,
                                  void * const newValue,
                                  void *      *pOldValue )
{
	void * oldValue;
	_asm {
		mov eax, expectedValue
		mov esi, memoryPosition
		mov ebx, newValue
		lock cmpxchg [esi], ebx
		mov oldValue, eax
	}
    *pOldValue = oldValue;
	return oldValue == expectedValue;
}

#elif defined(WIN32) && defined(_WIN64)

SAPDB_Bool RTESys_CmpxchgInt4( SAPDB_Int4 *memoryPosition,
                               SAPDB_Int4 const expectedValue,
                               SAPDB_Int4 const newValue,
                               SAPDB_Int4 *pOldValue)
{
    *pOldValue = (SAPDB_Int4)InterlockedCompareExchange( (LPLONG)memoryPosition,
                                                         (LONG)newValue,
                                                         (LONG)expectedValue );
	return ( *pOldValue == expectedValue );
}

SAPDB_Bool RTESys_CmpxchgUInt4( SAPDB_UInt4 *memoryPosition,
                                SAPDB_UInt4 const expectedValue,
                                SAPDB_UInt4 const newValue,
                                SAPDB_UInt4 *pOldValue)
{
    *pOldValue = (SAPDB_UInt4)InterlockedCompareExchange( (LPLONG)memoryPosition,
                                                          (LONG)newValue,
                                                          (LONG)expectedValue );
	return ( *pOldValue == expectedValue );
}

SAPDB_Bool RTESys_CmpxchgInt8( SAPDB_Int8 *memoryPosition,
                               SAPDB_Int8 const expectedValue,
                               SAPDB_Int8 const newValue,
                               SAPDB_Int8 *pOldValue)
{
	/* Workaround for itanium */
	/* There is no special compare & exchange function for 8 byte values available on itanium */
    *pOldValue = (SAPDB_Int8)InterlockedCompareExchangePointer((void *)memoryPosition,
						   									   (void *)newValue,
															   (void *)expectedValue);

	return ( *pOldValue == expectedValue );
}

SAPDB_Bool RTESys_CmpxchgUInt8( SAPDB_UInt8 *memoryPosition,
                                SAPDB_UInt8 const expectedValue,
                                SAPDB_UInt8 const newValue,
                                SAPDB_UInt8 *pOldValue)
{
	/* Workaround for itanium */
	/* There is no special compare & exchange function for 8 byte values available on itanium */
    *pOldValue = (SAPDB_UInt8)InterlockedCompareExchangePointer((void *)memoryPosition,
																 (void *)newValue,
																 (void *)expectedValue);

	return ( *pOldValue == expectedValue );
}

SAPDB_Bool RTESys_CmpxchgPointer( void * *memoryPosition,
                                  void * const expectedValue,
                                  void * const newValue,
                                  void * *pOldValue)
{
    *pOldValue = InterlockedCompareExchangePointer(memoryPosition,
                                                   newValue,
                                                   expectedValue);

	return ( *pOldValue == expectedValue );
}

#elif defined(LINUX) && defined(I386)

SAPDB_Bool RTESys_CmpxchgInt4( SAPDB_Int4 *memoryPosition,
                               SAPDB_Int4 const expectedValue,
                               SAPDB_Int4 const newValue,
                               SAPDB_Int4 *pOldValue)
{
	__asm__ __volatile__ (
		"movl %3, %%eax\n\t"
		"lock; cmpxchgl %2, %1\n\t"
		"movl %%eax, %0\n\t"
		: "=g"(*pOldValue): "m"(*memoryPosition), "r"(newValue), "g"(expectedValue)
		: "eax", "memory"
	);
    return ( expectedValue == (*pOldValue) );
}

SAPDB_Bool RTESys_CmpxchgUInt4( SAPDB_UInt4 *memoryPosition,
                                SAPDB_UInt4 const expectedValue,
                                SAPDB_UInt4 const newValue,
                                SAPDB_UInt4 *pOldValue)
{
	__asm__ __volatile__ (
		"movl %3, %%eax\n\t"
		"lock; cmpxchgl %2, %1\n\t"
		"movl %%eax, %0\n\t"
		: "=g"(*pOldValue): "m"(*memoryPosition), "r"(newValue), "g"(expectedValue)
		: "eax", "memory"
	);
    return ( expectedValue == (*pOldValue) );
}

SAPDB_Bool RTESys_CmpxchgInt8( SAPDB_Int8 *memoryPosition,
                               SAPDB_Int8 const expectedValue,
                               SAPDB_Int8 const newValue,
                               SAPDB_Int8 *pOldValue)
{
    SAPDB_UInt4 resultLow = newValue >> 32;
    SAPDB_UInt4 resultHigh = expectedValue >> 32;
    __asm__ __volatile__ (
        "push %%ebx\n\t"
		"movl %3, %%eax\n\t"
		"movl %4, %%edx\n\t"
		"movl %1, %%ebx\n\t"
		"movl %2, %%ecx\n\t"
		"lock; cmpxchg8b %0\n\t"
		"movl %%edx, %6\n\t"
		"movl %%eax, %5\n\t"
        "pop %%ebx\n\t"
		:
		: "m"(*memoryPosition),
		  "m"(newValue),
		  "m"(resultLow),
		  "m"(expectedValue),
		  "m"(resultHigh),
		  "m"(resultLow),
		  "m"(resultHigh)
		: "eax", "ebx", "ecx", "edx", "memory"
	);
    *pOldValue = (SAPDB_Int8)(resultLow | (( ((SAPDB_UInt8)resultHigh) << 32 )));
    return ( expectedValue == (*pOldValue) );
}

SAPDB_Bool RTESys_CmpxchgUInt8( SAPDB_UInt8 *memoryPosition,
                                SAPDB_UInt8 const expectedValue,
                                SAPDB_UInt8 const newValue,
                                SAPDB_UInt8 *pOldValue)
{
#if 0
    /* logically equivalent, but does not work.... */
    __asm__ __volatile__ (
		"movl %3, %%eax\n\t"
		"movl %4, %%edx\n\t"
		"movl %1, %%ebx\n\t"
		"movl %2, %%ecx\n\t"
		"lock; cmpxchg8b %0\n\t"
		"movl %%edx, %6\n\t"
		"movl %%eax, %5\n\t"
		:
		: "m"(*memoryPosition),
		  "m"(*(((SAPDB_UInt4 *)&newValue)+0)),
		  "m"(*(((SAPDB_UInt4 *)&newValue)+1)),
		  "m"(*(((SAPDB_UInt4 *)&expectedValue)+0)),
		  "m"(*(((SAPDB_UInt4 *)&expectedValue)+1)),
		  "m"(*(((SAPDB_UInt4 *)pOldValue)+0)),
		  "m"(*(((SAPDB_UInt4 *)pOldValue)+1))
		: "eax", "ebx", "ecx", "edx", "memory"
	);
#else
    SAPDB_UInt4 resultLow = newValue >> 32;
    SAPDB_UInt4 resultHigh = expectedValue >> 32;
    __asm__ __volatile__ (
        "push %%ebx\n\t"
		"movl %3, %%eax\n\t"
		"movl %4, %%edx\n\t"
		"movl %1, %%ebx\n\t"
		"movl %2, %%ecx\n\t"
		"lock; cmpxchg8b %0\n\t"
		"movl %%edx, %6\n\t"
		"movl %%eax, %5\n\t"
        "pop %%ebx\n\t"
		:
		: "m"(*memoryPosition),
		  "m"(newValue),
		  "m"(resultLow),
		  "m"(expectedValue),
		  "m"(resultHigh),
		  "m"(resultLow),
		  "m"(resultHigh)
		: "eax", "ebx", "ecx", "edx", "memory"
	);
    *pOldValue = resultLow | (( ((SAPDB_UInt8)resultHigh) << 32 ));
#endif
    return ( expectedValue == (*pOldValue) );
}

SAPDB_Bool RTESys_CmpxchgPointer( void * *memoryPosition,
                                  void * const expectedValue,
                                  void * const newValue,
                                  void * *pOldValue)
{
	__asm__ __volatile__ (
		"movl %3, %%eax\n\t"
		"lock; cmpxchgl %2, %1\n\t"
		"movl %%eax, %0\n\t"
		: "=g"(*pOldValue): "m"(*memoryPosition), "r"(newValue), "g"(expectedValue)
		: "eax", "memory"
	);
    return ( expectedValue == (*pOldValue) );
}

#elif defined(LINUX) && defined(X86_64)

SAPDB_Bool RTESys_CmpxchgInt4( SAPDB_Int4 *memoryPosition,
                               SAPDB_Int4 const expectedValue,
                               SAPDB_Int4 const newValue,
                               SAPDB_Int4 *pOldValue)
{
	__asm__ __volatile__ (
		"movl %3, %%eax\n\t"
		"lock; cmpxchgl %2, %1\n\t"
		"movl %%eax, %0\n\t"
		: "=g"(*pOldValue): "m"(*memoryPosition), "r"(newValue), "g"(expectedValue)
		: "eax", "memory"
	);
    return ( expectedValue == (*pOldValue) );
}

SAPDB_Bool RTESys_CmpxchgUInt4( SAPDB_UInt4 *memoryPosition,
                                SAPDB_UInt4 const expectedValue,
                                SAPDB_UInt4 const newValue,
                                SAPDB_UInt4 *pOldValue)
{
	__asm__ __volatile__ (
		"movl %3, %%eax\n\t"
		"lock; cmpxchgl %2, %1\n\t"
		"movl %%eax, %0\n\t"
		: "=g"(*pOldValue): "m"(*memoryPosition), "r"(newValue), "g"(expectedValue)
		: "eax", "memory"
	);
    return ( expectedValue == (*pOldValue) );
}

SAPDB_Bool RTESys_CmpxchgInt8( SAPDB_Int8 *memoryPosition,
                               SAPDB_Int8 const expectedValue,
                               SAPDB_Int8 const newValue,
                               SAPDB_Int8 *pOldValue)
{
	__asm__ __volatile__ (
		"movq %3, %%rax\n\t"
		"lock; cmpxchgq %2, %1\n\t"
		"movq %%rax, %0\n\t"
		: "=g"(*pOldValue): "m"(*memoryPosition), "r"(newValue), "g"(expectedValue)
		: "rax", "memory"
	);
    return ( expectedValue == (*pOldValue) );
}

SAPDB_Bool RTESys_CmpxchgUInt8( SAPDB_UInt8 *memoryPosition,
                                SAPDB_UInt8 const expectedValue,
                                SAPDB_UInt8 const newValue,
                                SAPDB_UInt8 *pOldValue)
{
	__asm__ __volatile__ (
		"movq %3, %%rax\n\t"
		"lock; cmpxchgq %2, %1\n\t"
		"movq %%rax, %0\n\t"
		: "=g"(*pOldValue): "m"(*memoryPosition), "r"(newValue), "g"(expectedValue)
		: "rax", "memory"
	);
    return ( expectedValue == (*pOldValue) );
}

SAPDB_Bool RTESys_CmpxchgPointer( void * *memoryPosition,
                                  void * const expectedValue,
                                  void * const newValue,
                                  void * *pOldValue)
{
	__asm__ __volatile__ (
		"movq %3, %%rax\n\t"
		"lock; cmpxchgq %2, %1\n\t"
		"movq %%rax, %0\n\t"
		: "=g"(*pOldValue): "m"(*memoryPosition), "r"(newValue), "g"(expectedValue)
		: "rax", "memory"
	);
    return ( expectedValue == (*pOldValue) );
}

#elif defined(LINUX) && defined(IA64)

SAPDB_Bool RTESys_CmpxchgInt4( SAPDB_Int4 *memoryPosition,
                               SAPDB_Int4 const expectedValue,
                               SAPDB_Int4 const newValue,
                               SAPDB_Int4 *pOldValue)
{
#if defined __INTEL_COMPILER
/*LINUX && IA64*/	*pOldValue = _InterlockedCompareExchange_rel (memoryPosition, newValue, expectedValue);
#elif defined __GNUC__
/*LINUX && IA64*/	SAPDB_UInt8 expectedValueAsLong = (SAPDB_UInt8)((SAPDB_UInt4)expectedValue);
/*LINUX && IA64*/   __asm__ __volatile__ ("mov ar.ccv = %0 ;;" :: "rO"(expectedValueAsLong) );
/*LINUX && IA64*/	__asm__ __volatile__ ("cmpxchg4.rel %0=%2,%3,ar.ccv"
/*LINUX && IA64*/			: "=r"(*pOldValue),"=m"(*memoryPosition)
/*LINUX && IA64*/			: "m"(*memoryPosition), "r"(newValue));
#endif
/*LINUX && IA64*/  return ( expectedValue == (*pOldValue) );
}

SAPDB_Bool RTESys_CmpxchgUInt4( SAPDB_UInt4 *memoryPosition,
                                SAPDB_UInt4 const expectedValue,
                                SAPDB_UInt4 const newValue,
                                SAPDB_UInt4 *pOldValue)
{
#if defined __INTEL_COMPILER
/*LINUX && IA64*/	*pOldValue = _InterlockedCompareExchange_rel (memoryPosition, newValue, expectedValue);
#elif defined __GNUC__
/*LINUX && IA64*/	SAPDB_UInt8 expectedValueAsLong = expectedValue;
/*LINUX && IA64*/   __asm__ __volatile__ ("mov ar.ccv = %0 ;;" :: "rO"(expectedValueAsLong) );
/*LINUX && IA64*/	__asm__ __volatile__ ("cmpxchg4.rel %0=%2,%3,ar.ccv"
/*LINUX && IA64*/			: "=r"(*pOldValue),"=m"(*memoryPosition)
/*LINUX && IA64*/			: "m"(*memoryPosition), "r"(newValue));
#endif
/*LINUX && IA64*/  return ( expectedValue == (*pOldValue) );
}

SAPDB_Bool RTESys_CmpxchgInt8( SAPDB_Int8 *memoryPosition,
                               SAPDB_Int8 const expectedValue,
                               SAPDB_Int8 const newValue,
                               SAPDB_Int8 *pOldValue)
{
#if defined __INTEL_COMPILER
/*LINUX && IA64*/	*pOldValue = _InterlockedCompareExchange64_rel (memoryPosition, newValue, expectedValue);
#elif defined __GNUC__
/*LINUX && IA64*/	__asm__ __volatile__ ("mov ar.ccv = %0 ;;" :: "rO"(expectedValue) );
/*LINUX && IA64*/	__asm__ __volatile__ ("cmpxchg8.rel %0=%2,%3,ar.ccv"
/*LINUX && IA64*/			: "=r"(*pOldValue),"=m"(*memoryPosition)
/*LINUX && IA64*/			: "m"(*memoryPosition), "r"(newValue));
#endif
/*LINUX && IA64*/  return ( expectedValue == (*pOldValue) );
}

SAPDB_Bool RTESys_CmpxchgUInt8( SAPDB_UInt8 *memoryPosition,
                                SAPDB_UInt8 const expectedValue,
                                SAPDB_UInt8 const newValue,
                                SAPDB_UInt8 *pOldValue)
{
#if defined __INTEL_COMPILER
/*LINUX && IA64*/	*pOldValue = _InterlockedCompareExchange64_rel (memoryPosition, newValue, expectedValue);
#elif defined __GNUC__
/*LINUX && IA64*/	__asm__ __volatile__ ("mov ar.ccv = %0 ;;" :: "rO"(expectedValue) );
/*LINUX && IA64*/	__asm__ __volatile__ ("cmpxchg8.rel %0=%2,%3,ar.ccv"
/*LINUX && IA64*/			: "=r"(*pOldValue),"=m"(*memoryPosition)
/*LINUX && IA64*/			: "m"(*memoryPosition), "r"(newValue));
#endif
/*LINUX && IA64*/  return ( expectedValue == (*pOldValue) );
}

SAPDB_Bool RTESys_CmpxchgPointer( void * *memoryPosition,
                                  void * const expectedValue,
                                  void * const newValue,
                                  void * *pOldValue)
{
#if defined __INTEL_COMPILER
/*LINUX && IA64*/	*pOldValue = _InterlockedCompareExchangePointer (memoryPosition, newValue, expectedValue);
#elif defined __GNUC__
/*LINUX && IA64*/	__asm__ __volatile__ ("mov ar.ccv = %0 ;;" :: "rO"(expectedValue) );
/*LINUX && IA64*/	__asm__ __volatile__ ("cmpxchg8.rel %0=%2,%3,ar.ccv"
/*LINUX && IA64*/			: "=r"(*pOldValue),"=m"(*memoryPosition)
/*LINUX && IA64*/			: "m"(*memoryPosition), "r"(newValue));
#endif
/*LINUX && IA64*/  return ( expectedValue == (*pOldValue) );
}

#elif defined(AIX)

SAPDB_Bool RTESys_CmpxchgInt4( SAPDB_Int4 *memoryPosition,
                               SAPDB_Int4 const expectedValue,
                               SAPDB_Int4 const newValue,
                               SAPDB_Int4 *pOldValue)
{
    int oldValue = (int)expectedValue;
    if ( compare_and_swap( (atomic_p)memoryPosition, &oldValue, (int)newValue) )
    {
        *pOldValue = expectedValue;
        return true;
    }
    else
    {
        *pOldValue = (SAPDB_Int4)oldValue;
        return false;
    }
}

SAPDB_Bool RTESys_CmpxchgUInt4( SAPDB_UInt4 *memoryPosition,
                                SAPDB_UInt4 const expectedValue,
                                SAPDB_UInt4 const newValue,
                                SAPDB_UInt4 *pOldValue)
{
    int oldValue = (int)expectedValue;
    if ( compare_and_swap( (atomic_p)memoryPosition, &oldValue, (int)newValue) )
    {
        *pOldValue = expectedValue;
        return true;
    }
    else
    {
        *pOldValue = (SAPDB_UInt4)oldValue;
        return false;
    }
}

SAPDB_Bool RTESys_CmpxchgInt8( SAPDB_Int8 *memoryPosition,
                               SAPDB_Int8 const expectedValue,
                               SAPDB_Int8 const newValue,
                               SAPDB_Int8 *pOldValue)
{
    long oldValue = (long)expectedValue;
    if ( compare_and_swaplp( (atomic_l)memoryPosition, &oldValue, (long)newValue) )
    {
        *pOldValue = expectedValue;
        return true;
    }
    else
    {
        *pOldValue = (SAPDB_Int8)oldValue;
        return false;
    }
}

SAPDB_Bool RTESys_CmpxchgUInt8( SAPDB_UInt8 *memoryPosition,
                                SAPDB_UInt8 const expectedValue,
                                SAPDB_UInt8 const newValue,
                                SAPDB_UInt8 *pOldValue)
{
    long oldValue = (long)expectedValue;
    if ( compare_and_swaplp( (atomic_l)memoryPosition, &oldValue, (long)newValue) )
    {
        *pOldValue = expectedValue;
        return true;
    }
    else
    {
        *pOldValue = (SAPDB_UInt8)oldValue;
        return false;
    }
}

SAPDB_Bool RTESys_CmpxchgPointer( void * *memoryPosition,
                                  void * const expectedValue,
                                  void * const newValue,
                                  void * *pOldValue)
{
    long oldValue = (long)(((SAPDB_Char *)expectedValue)-(SAPDB_Char *)0);
    if ( compare_and_swaplp( (atomic_l)memoryPosition, &oldValue,
                             (long)(((SAPDB_Char *)newValue)-(SAPDB_Char *)0) ) )
    {
        *pOldValue = expectedValue;
        return true;
    }
    else
    {
        *pOldValue = (void *)(((SAPDB_Char *)0) + oldValue);
        return false;
    }
}

#elif defined(OSF1)

SAPDB_Bool RTESys_CmpxchgInt4( SAPDB_Int4 *memoryPosition,
                               SAPDB_Int4 const expectedValue,
                               SAPDB_Int4 const newValue,
                               SAPDB_Int4 *pOldValue)
{
    int oldValue = (int)expectedValue;
    if ( __CMP_STORE_LONG((volatile void *)memoryPosition,
                                        (int)oldValue,
                                        (int)newValue,
                                        (volatile void *)memoryPosition ) )
    {
        *pOldValue = expectedValue;
        return true;
    }
    else
    {
        *pOldValue = *memoryPosition;
        return false;
    }
}

SAPDB_Bool RTESys_CmpxchgUInt4( SAPDB_UInt4 *memoryPosition,
                                SAPDB_UInt4 const expectedValue,
                                SAPDB_UInt4 const newValue,
                                SAPDB_UInt4 *pOldValue)
{
    int oldValue = (int)expectedValue;
    if ( __CMP_STORE_LONG((volatile void *)memoryPosition,
                                        (int)oldValue,
                                        (int)newValue,
                                        (volatile void *)memoryPosition ) )
    {
        *pOldValue = expectedValue;
        return true;
    }
    else
    {
        *pOldValue = *memoryPosition;
        return false;
    }
}

SAPDB_Bool RTESys_CmpxchgInt8( SAPDB_Int8 *memoryPosition,
                               SAPDB_Int8 const expectedValue,
                               SAPDB_Int8 const newValue,
                               SAPDB_Int8 *pOldValue)
{
    long oldValue = (long)((SAPDB_Char *)expectedValue-(SAPDB_Char *)0);
    if ( __CMP_STORE_QUAD((volatile void *)memoryPosition,
                                        (long)oldValue,
                                        (long)newValue,
                                        (volatile void *)memoryPosition ) )
    {
        *pOldValue = expectedValue;
        return true;
    }
    else
    {
        *pOldValue = *memoryPosition;
        return false;
    }
}

SAPDB_Bool RTESys_CmpxchgUInt8( SAPDB_UInt8 *memoryPosition,
                                SAPDB_UInt8 const expectedValue,
                                SAPDB_UInt8 const newValue,
                                SAPDB_UInt8 *pOldValue)
{
    long oldValue = (long)expectedValue;
    if ( __CMP_STORE_QUAD((volatile void *)memoryPosition,
                                        (long)oldValue,
                                        (long)newValue,
                                        (volatile void *)memoryPosition ) )
    {
        *pOldValue = expectedValue;
        return true;
    }
    else
    {
        *pOldValue = *memoryPosition;
        return false;
    }
}

SAPDB_Bool RTESys_CmpxchgPointer( void * *memoryPosition,
                                  void * const expectedValue,
                                  void * const newValue,
                                  void * *pOldValue)
{
    long oldValue = (long)(((SAPDB_Char *)expectedValue)-(SAPDB_Char *)0);
    if ( __CMP_STORE_QUAD((volatile void *)memoryPosition,
                                        (long)oldValue,
                                        (long)newValue,
                                        (volatile void *)memoryPosition ) )
    {
        *pOldValue = expectedValue;
        return true;
    }
    else
    {
        *pOldValue = (void *)*((volatile void **)memoryPosition);
        return false;
    }
}

#elif (defined(SOLARIS) && (!defined(SDB_HAS_SYS_ATOMIC) || !defined(BIT64))) || defined(SVR4)

#if !defined(BIT64)
/*SUN*/int RTESys_AtomicCmpxchgWithMembarInt4( SAPDB_Int4 *address,
/*SUN*/                                        SAPDB_Int4 expectedValue,
/*SUN*/                                        SAPDB_Int4 newValue,
/*SUN*/                                        SAPDB_Int4 *pOldValue)
/*SUN*/{
/*SUN*/    *pOldValue = *address;
/*SUN*/    if ( *pOldValue == expectedValue )
/*SUN*/    {
/*SUN*/        *address = newValue;
/*SUN*/        return 1;
/*SUN*/    }
/*SUN*/    else
/*SUN*/    {
/*SUN*/        return 0;
/*SUN*/    }
/*SUN*/}

/*SUN*/int RTESys_AtomicCmpxchgWithMembarInt8( SAPDB_Int8 *address,
/*SUN*/                                        SAPDB_Int8 expectedValue,
/*SUN*/                                        SAPDB_Int8 newValue,
/*SUN*/                                        SAPDB_Int8 *pOldValue)
/*SUN*/{
/*SUN*/    *pOldValue = *address;
/*SUN*/    if ( *pOldValue == expectedValue )
/*SUN*/    {
/*SUN*/        *address = newValue;
/*SUN*/        return 1;
/*SUN*/    }
/*SUN*/    else
/*SUN*/    {
/*SUN*/        return 0;
/*SUN*/    }
/*SUN*/}
#endif

SAPDB_Bool RTESys_CmpxchgInt4( SAPDB_Int4 *memoryPosition,
                               SAPDB_Int4 const expectedValue,
                               SAPDB_Int4 const newValue,
                               SAPDB_Int4 *pOldValue)
{
    return ( RTESys_AtomicCmpxchgWithMembarInt4((SAPDB_Int4 *)memoryPosition,
                                                (SAPDB_Int4)expectedValue,
                                                (SAPDB_Int4)newValue,
                                                (SAPDB_Int4 *)pOldValue) );
}

SAPDB_Bool RTESys_CmpxchgUInt4( SAPDB_UInt4 *memoryPosition,
                                SAPDB_UInt4 const expectedValue,
                                SAPDB_UInt4 const newValue,
                                SAPDB_UInt4 *pOldValue)
{
    return ( RTESys_AtomicCmpxchgWithMembarInt4((SAPDB_Int4 *)memoryPosition,
                                                (SAPDB_Int4)expectedValue,
                                                (SAPDB_Int4)newValue,
                                                (SAPDB_Int4 *)pOldValue) );
}

SAPDB_Bool RTESys_CmpxchgInt8( SAPDB_Int8 *memoryPosition,
                               SAPDB_Int8 const expectedValue,
                               SAPDB_Int8 const newValue,
                               SAPDB_Int8 *pOldValue)
{
    return ( RTESys_AtomicCmpxchgWithMembarInt8( (SAPDB_Int8 *)memoryPosition,
                                                 (SAPDB_Int8)expectedValue,
                                                 (SAPDB_Int8)newValue,
                                                 (SAPDB_Int8 *)pOldValue) );
}

SAPDB_Bool RTESys_CmpxchgUInt8( SAPDB_UInt8 *memoryPosition,
                                SAPDB_UInt8 const expectedValue,
                                SAPDB_UInt8 const newValue,
                                SAPDB_UInt8 *pOldValue)
{
    return ( RTESys_AtomicCmpxchgWithMembarInt8( (SAPDB_Int8 *)memoryPosition,
                                       (SAPDB_Int8)expectedValue,
                                       (SAPDB_Int8)newValue,
                                       (SAPDB_Int8 *)pOldValue) );
}

SAPDB_Bool RTESys_CmpxchgPointer( void * *memoryPosition,
                                  void * const expectedValue,
                                  void * const newValue,
                                  void * *pOldValue)
{
#if defined(BIT64)
    return ( RTESys_AtomicCmpxchgWithMembarInt8( (SAPDB_Int8 *)memoryPosition,
                                       (SAPDB_Int8)((SAPDB_Char *)expectedValue-(SAPDB_Char *)0),
                                       (SAPDB_Int8)((SAPDB_Char *)newValue-(SAPDB_Char *)0),
                                       (SAPDB_Int8 *)pOldValue) );
#else
    return ( RTESys_AtomicCmpxchgWithMembarInt4( (SAPDB_Int4 *)memoryPosition,
                                       (SAPDB_Int4)((SAPDB_Char *)expectedValue-(SAPDB_Char *)0),
                                       (SAPDB_Int4)((SAPDB_Char *)newValue-(SAPDB_Char *)0),
                                       (SAPDB_Int4 *)pOldValue) );
#endif /* BIT64 */
}

#elif defined(SOLARIS) && defined(SDB_HAS_SYS_ATOMIC) && defined(BIT64) /* Solaris 10 onwards */

#include <sys/atomic.h>

SAPDB_Bool RTESys_CmpxchgInt4( SAPDB_Int4 *memoryPosition,
                               SAPDB_Int4 const expectedValue,
                               SAPDB_Int4 const newValue,
                               SAPDB_Int4 *pOldValue)
{
	*pOldValue = (SAPDB_Int4)atomic_cas_32( (volatile uint32_t *)memoryPosition,
						(uint32_t)expectedValue,
						(uint32_t)newValue);
	return *pOldValue == expectedValue;
}

SAPDB_Bool RTESys_CmpxchgUInt4( SAPDB_UInt4 *memoryPosition,
                                SAPDB_UInt4 const expectedValue,
                                SAPDB_UInt4 const newValue,
                                SAPDB_UInt4 *pOldValue)
{
	*pOldValue = (SAPDB_UInt4)atomic_cas_32((volatile uint32_t *)memoryPosition,
						(uint32_t)expectedValue,
						(uint32_t)newValue);
	return *pOldValue == expectedValue;
}

SAPDB_Bool RTESys_CmpxchgInt8( SAPDB_Int8 *memoryPosition,
                               SAPDB_Int8 const expectedValue,
                               SAPDB_Int8 const newValue,
                               SAPDB_Int8 *pOldValue)
{
	*pOldValue = (SAPDB_Int8)atomic_cas_64( (volatile uint64_t *)memoryPosition,
						(uint64_t)expectedValue,
						(uint64_t)newValue);
	return *pOldValue == expectedValue;
}

SAPDB_Bool RTESys_CmpxchgUInt8( SAPDB_UInt8 *memoryPosition,
                                SAPDB_UInt8 const expectedValue,
                                SAPDB_UInt8 const newValue,
                                SAPDB_UInt8 *pOldValue)
{
	*pOldValue = (SAPDB_UInt8)atomic_cas_64((volatile uint64_t *)memoryPosition,
					 	(uint64_t)expectedValue,
						(uint64_t)newValue);
	return *pOldValue == expectedValue;
}

SAPDB_Bool RTESys_CmpxchgPointer( void * *pointerPosition,
                                  void * const expectedValue,
                                  void * const newValue,
                                  void * *pOldValue)
{
	/* NOTE: the cast looks strange, but is correct */
	*pOldValue = atomic_cas_ptr((volatile void *)pointerPosition,
			 	    (void *)expectedValue,
				    (void *)newValue);
	return *pOldValue == expectedValue;
}

#elif defined (HP_IA64)

#include <machine/sys/inline.h>

SAPDB_Bool RTESys_CmpxchgInt4( SAPDB_Int4 *memoryPosition,
                               SAPDB_Int4 const expectedValue,
                               SAPDB_Int4 const newValue,
                               SAPDB_Int4 *pOldValue)
{
    _Asm_mov_to_ar
    (
        (_Asm_app_reg)_AREG_CCV,
        (SAPDB_UInt8)((SAPDB_UInt4)expectedValue),
        FENCE
    );
    *pOldValue = _Asm_cmpxchg
    (
        (_Asm_sz)_SZ_W,
        (_Asm_sem)_SEM_ACQ,
        (void *)memoryPosition,
        (SAPDB_UInt8)newValue,
        (_Asm_ldhint)_LDHINT_NONE
    );
    return (*pOldValue == expectedValue);
}

SAPDB_Bool RTESys_CmpxchgUInt4( SAPDB_UInt4 *memoryPosition,
                                SAPDB_UInt4 const expectedValue,
                                SAPDB_UInt4 const newValue,
                                SAPDB_UInt4 *pOldValue)
{
    _Asm_mov_to_ar
    (
        (_Asm_app_reg)_AREG_CCV,
        (SAPDB_UInt8)expectedValue,
        FENCE
    );
    *pOldValue = _Asm_cmpxchg
    (
        (_Asm_sz)_SZ_W,
        (_Asm_sem)_SEM_ACQ,
        (void *)memoryPosition,
        (SAPDB_UInt8)newValue,
        (_Asm_ldhint)_LDHINT_NONE
    );
    return (*pOldValue == expectedValue);
}

SAPDB_Bool RTESys_CmpxchgInt8( SAPDB_Int8 *memoryPosition,
                               SAPDB_Int8 const expectedValue,
                               SAPDB_Int8 const newValue,
                               SAPDB_Int8 *pOldValue)
{
    _Asm_mov_to_ar
    (
        (_Asm_app_reg)_AREG_CCV,
        (SAPDB_UInt8)expectedValue,
        FENCE
    );
    *pOldValue = _Asm_cmpxchg
    (
        (_Asm_sz)_SZ_D,
        (_Asm_sem)_SEM_ACQ,
        (void *)memoryPosition,
        (SAPDB_UInt8)newValue,
        (_Asm_ldhint)_LDHINT_NONE
    );
    return (*pOldValue == expectedValue);
}

SAPDB_Bool RTESys_CmpxchgUInt8( SAPDB_UInt8 *memoryPosition,
                                SAPDB_UInt8 const expectedValue,
                                SAPDB_UInt8 const newValue,
                                SAPDB_UInt8 *pOldValue)
{
    _Asm_mov_to_ar
    (
        (_Asm_app_reg)_AREG_CCV,
        (SAPDB_UInt8)expectedValue,
        FENCE
    );
    *pOldValue = _Asm_cmpxchg
    (
        (_Asm_sz)_SZ_D,
        (_Asm_sem)_SEM_ACQ,
        (void *)memoryPosition,
        (SAPDB_UInt8)newValue,
        (_Asm_ldhint)_LDHINT_NONE
    );
    return (*pOldValue == expectedValue);
}

SAPDB_Bool RTESys_CmpxchgPointer( void * *memoryPosition,
                                  void * const expectedValue,
                                  void * const newValue,
                                  void * *pOldValue)
{
    _Asm_mov_to_ar
    (
        (_Asm_app_reg)_AREG_CCV,
        (SAPDB_UInt8)expectedValue,
        FENCE
    );
    *pOldValue = (void *)_Asm_cmpxchg
        (
            (_Asm_sz)_SZ_D,
            (_Asm_sem)_SEM_ACQ,
            (void *)memoryPosition,
            (SAPDB_UInt8)newValue,
            (_Asm_ldhint)_LDHINT_NONE
        );
    return (*pOldValue == expectedValue);
}

#endif

#else

/*! define */
#define CompareAndExchange(funcName_, valueType_) \
    SAPDB_Bool funcName_ ( valueType_ *     memoryPosition, \
                           valueType_ const expectedValue, \
                           valueType_ const newValue, \
                           valueType_ *     oldValue ) \
    { \
        valueType_ currentValue; \
        SAPDB_UInt4 lockIndex = RTESys_AtomicLockPoolLock(memoryPosition); \
        currentValue = *memoryPosition; \
        if ( expectedValue == currentValue ) \
        { \
           *memoryPosition = newValue; \
        } \
        RTESys_AtomicLockPoolUnlock(memoryPosition, lockIndex); \
        *oldValue = currentValue; \
        return ( expectedValue == currentValue ); \
    }

/*!
  @brief CompareAndExchange for SAPDB_Int4
  @param memoryPosition [in]
  @param expectedValue [in]
  @param newValue [in]
  @param oldValue [out]
 */
CompareAndExchange(RTESys_CmpxchgInt4,    SAPDB_Int4)
/*!
  @brief CompareAndExchange for SAPDB_UInt4
  @param memoryPosition [in]
  @param expectedValue [in]
  @param newValue [in]
  @param oldValue [out]
 */
CompareAndExchange(RTESys_CmpxchgUInt4,   SAPDB_UInt4)
/*!
  @brief CompareAndExchange for SAPDB_Int8
  @param memoryPosition [in]
  @param expectedValue [in]
  @param newValue [in]
  @param oldValue [out]
 */
CompareAndExchange(RTESys_CmpxchgInt8,    SAPDB_Int8)
/*!
  @brief CompareAndExchange for SAPDB_UInt8
  @param memoryPosition [in]
  @param expectedValue [in]
  @param newValue [in]
  @param oldValue [out]
 */
CompareAndExchange(RTESys_CmpxchgUInt8,   SAPDB_UInt8)
/*!
  @brief CompareAndExchange for void *
  @param memoryPosition [in]
  @param expectedValue [in]
  @param newValue [in]
  @param oldValue [out]
 */
CompareAndExchange(RTESys_CmpxchgPointer, void *)

#endif /* RTESYS_HAS_NATIVE_ATOMIC_OPERATION */

/*=========================================================================================*/
/*! Atomic modification (increment/decrement) */
/*=========================================================================================*/

/*! define */
#define AtomicModify(name_, deltaType_) SAPDB_##name_ RTESys_AtomicModify##name_ ( SAPDB_##name_ * memoryPosition, \
                                      deltaType_ const deltaValue ) \
    { \
        SAPDB_##name_ oldValue = 0; \
        SAPDB_##name_ newValue = 0; \
        SAPDB_##name_ foundValue = *memoryPosition; \
        do { \
            oldValue = foundValue; \
            newValue = oldValue + deltaValue; \
        } while( !RTESys_Cmpxchg##name_ (memoryPosition, oldValue, newValue, &foundValue) ); \
        return newValue; \
    }

/*!
  @brief Atomic modify for SAPDB_Int8
  @param memoryPosition [in]
  @param deltaValue [in]
  @return newValue
 */
AtomicModify(Int8,  SAPDB_Int8)
/*!
  @brief Atomic modify for SAPDB_UInt8
  @param memoryPosition [in]
  @param deltaValue [in]
  @return newValue
 */
AtomicModify(UInt8, SAPDB_Int8)
/*!
  @brief Atomic modify for SAPDB_Int4
  @param memoryPosition [in]
  @param deltaValue [in]
  @return newValue
 */
AtomicModify(Int4,  SAPDB_Int4)
/*!
  @brief Atomic modify for SAPDB_UInt4
  @param memoryPosition [in]
  @param deltaValue [in]
  @return newValue
 */
AtomicModify(UInt4, SAPDB_Int4)
/*!
  @brief Atomic modify for SAPDB_UInt8
  @param memoryPosition [in]
  @param scaling [in]
  @param deltaValue [in]
  @return newValue
 */
void * RTESys_AtomicModifyPointer( void * *          memoryPosition,
                                 SAPDB_ULong const scaling,
                                 SAPDB_Long const  deltaValue )
{
    void * oldValue = 0;
    void * newValue = 0;
    void * foundValue = *memoryPosition;
    do {
        oldValue = foundValue;
        newValue = (((SAPDB_Byte *)oldValue) + (scaling * deltaValue));
    } while( !RTESys_CmpxchgPointer(memoryPosition, oldValue, newValue, &foundValue) );
    return newValue;
}

/*=========================================================================================*/
/*! Atomic modification (generic function) */
/*=========================================================================================*/

/*! define */
#define AtomicOperation(name_, valueType_) \
    void RTESys_AtomicOperation##name_ ( valueType_ *           memoryPosition, \
                                         RTESys_AtomicOp##name_ operationFunction, \
                                         void *                 operationContext ) \
    { \
        valueType_ oldValue = 0; \
        valueType_ newValue = 0; \
        valueType_ foundValue = *memoryPosition; \
\
        do \
        { \
           oldValue = foundValue; \
           if ( !(*operationFunction)(oldValue, &newValue, operationContext) ) \
               break; \
        } while ( !RTESys_Cmpxchg##name_ (memoryPosition, oldValue, newValue, &foundValue) ); \
        return; \
    }

/*!
  @brief Atomic operation for SAPDB_Int8
  The operation given is executed atomically by calling function with given context until new value
  can be stored atomically.
  @param memoryPosition [in]
  @param operationFunction [in] calculates new value
  @param operationContext [in] context of this function
  @return newValue
 */
AtomicOperation(Int8,    SAPDB_Int8)
/*!
  @brief Atomic operation for SAPDB_UInt8
  The operation given is executed atomically by calling function with given context until new value
  can be stored atomically.
  @param memoryPosition [in]
  @param operationFunction [in] calculates new value
  @param operationContext [in] context of this function
  @return newValue
 */
AtomicOperation(UInt8,   SAPDB_UInt8)
/*!
  @brief Atomic operation for SAPDB_Int4
  The operation given is executed atomically by calling function with given context until new value
  can be stored atomically.
  @param memoryPosition [in]
  @param operationFunction [in] calculates new value
  @param operationContext [in] context of this function
  @return newValue
 */
AtomicOperation(Int4,    SAPDB_Int4)
/*!
  @brief Atomic operation for SAPDB_UInt4
  The operation given is executed atomically by calling function with given context until new value
  can be stored atomically.
  @param memoryPosition [in]
  @param operationFunction [in] calculates new value
  @param operationContext [in] context of this function
  @return newValue
 */
AtomicOperation(UInt4,   SAPDB_UInt4)
/*!
  @brief Atomic operation for SAPDB_UInt8
  The operation given is executed atomically by calling function with given context until new value
  can be stored atomically.
  @param memoryPosition [in]
  @param operationFunction [in] calculates new value
  @param operationContext [in] context of this function
  @return newValue
 */
AtomicOperation(Pointer, void *)

#ifndef KERNEL_LZU

#ifndef RTESYS_ATOMIC_OPERATION_POOL_SIZE
/*! define */
#define RTESYS_ATOMIC_OPERATION_POOL_SIZE 256
#endif /* RTESYS_ATOMIC_OPERATION_POOL_SIZE */
/*! global pool initialized flag */
static SAPDB_Bool  poolInitialized = 0;
/*! global lock pool hook */
static RTE_Lock  **poolLockArray = 0;

/*!
  @brief initialize pool

  Called while still not multithreaded!!!!!
 */
static void AtomicLockPoolInitialize()
{
    SAPDB_Byte *pLock = 0;
    SAPDB_UInt4 iLock = 0;
#if defined(PA20W) || defined(PA11)
    SAPDB_UInt4 lockSize = SAPDB_CACHELINE_SIZE;
#else
    SAPDB_UInt4 lockSize = sizeof(RTE_Lock);
#endif

    poolLockArray = (RTE_Lock **)malloc(RTESYS_ATOMIC_OPERATION_POOL_SIZE * sizeof(RTE_Lock *));

#if defined(WIN32)
    {
        SAPDB_Int4 systemPageMask = (RTESys_SystemPageSize() - 1);
        pLock = VirtualAlloc( 0,
                              (systemPageMask + (RTESYS_ATOMIC_OPERATION_POOL_SIZE * lockSize)) & (~systemPageMask),
                              MEM_COMMIT|MEM_TOP_DOWN,
                              PAGE_READWRITE );
    }
#else
    {
        SAPDB_Int4 systemPageMask = (RTESys_SystemPageSize() - 1);
        SAPDB_Byte *lockPoolPtr = (SAPDB_Byte *)malloc( systemPageMask + (RTESYS_ATOMIC_OPERATION_POOL_SIZE * lockSize) );
        pLock = (SAPDB_Byte *)0 + ( ((lockPoolPtr - (SAPDB_Byte *)0) + systemPageMask)&(~systemPageMask) );
    }
#endif
    for ( iLock = 0; iLock < RTESYS_ATOMIC_OPERATION_POOL_SIZE; iLock++ )
    {
        poolLockArray[iLock] = (RTE_Lock *)pLock;
        RTESys_InitLock(poolLockArray[iLock]);
        pLock += lockSize;
    }
}

/*!
  @brief lock memory address access using lock pool
  The locking uses a limited number of pool locks. The hashing of the address
  returns an index which must be used to release the lock
  @param memoryAddress [in] the memroy address to lock
  @return pool index
 */
static SAPDB_UInt4 AtomicLockPoolLock(void *memoryAddress)
{
    SAPDB_UInt4 poolIndex = 0;
    SAPDB_ULong byteOffset = ((SAPDB_Byte *)memoryAddress) - (SAPDB_Byte *)0;
    while ( 0 == (byteOffset & 1) )
    {
        byteOffset >>= 1;
    }

    if ( !poolInitialized )
    {
        AtomicLockPoolInitialize();
        poolInitialized = 1;
    }

    poolIndex = (SAPDB_UInt4)(byteOffset % RTESYS_ATOMIC_OPERATION_POOL_SIZE);

    while ( RTESys_TestAndLock(poolLockArray[poolIndex]) )
        ;

    return poolIndex;
}

/*!
  @brief unlock memory address access using lock pool
  The locking uses a limited number of pool locks. The hashing of the address
  returns an index which must be used to release the lock
  @param memoryAddress [in] the memroy address to lock
  @param poolIndex [in] index return by locking pool
  @return pool index
 */
static void AtomicLockPoolUnlock(void *memoryAddress, SAPDB_UInt4 poolIndex)
{
#ifdef SAPDB_SLOW
    SAPDB_UInt4 checkPoolIndex = 0;
    SAPDB_ULong byteOffset = ((SAPDB_Byte *)memoryAddress) - (SAPDB_Byte *)0;

    if ( !poolInitialized )
    {
        memoryAddress = (void *)1; *(SAPDB_Int4 *)memoryAddress = 1; /* die... */
    }

    while ( 0 == (byteOffset & 1) )
    {
        byteOffset >>= 1;
    }
    checkPoolIndex = (SAPDB_UInt4)(byteOffset % RTESYS_ATOMIC_OPERATION_POOL_SIZE);

    if ( poolIndex != checkPoolIndex )
    {
        memoryAddress = (void *)1; *(SAPDB_Int4 *)memoryAddress = 2; /* die... */
    }
#endif
    RTESys_Unlock(poolLockArray[poolIndex]);
}

#endif /* KERNEL_LZU */

static char ident_sr[] = "@(#)RTESys_AtomicOperation+nothread.c dated 2006-07-17 05:54:08 used 2008-05-09 02:06:44";
extern char *RTESys_AtomicOperation_nothread_force_data () { return( ident_sr ); }
static char ident_mf[] = "@(#)RTESys_AtomicOperation+nothread FAST I386 UNIX LINUX PTOC -REL30=1 -RELVER=R76 -LC_STATE=RAMP -COMPILER=UNDEF -MACHINE=UNDEF";
static char ident_cc[] = "@(#)RTESys_AtomicOperation+nothread cc -DREL30 -DLINUX -DI386 -DSAG -DSDB_RELEASE_070600 -I/home/gunter/SANDBOX/MaxDB/V76_00_32/MaxDB_DEV/usr/incl -I/home/gunter/SANDBOX/MaxDB/V76_00_32/MaxDB_DEV/sys/wrk/incl -I/home/gunter/SANDBOX/MaxDB/V76_00_32/MaxDB_DEV/sys/wrk/incl/SAPDB -DSYSV -D_SVID -I/usr/include/ncurses -w -Wall -D_FILE_OFFSET_BITS=64 -fPIC -DHAS_NPTL -fno-strict-aliasing -DSAPDB_FAST -O3 -march=pentium -mcpu=pentiumpro -I/home/gunter/SANDBOX/MaxDB/DevTool/incl ";
extern char *RTESys_AtomicOperation_nothread_force_whatinfo () { return( ident_cc ); }
