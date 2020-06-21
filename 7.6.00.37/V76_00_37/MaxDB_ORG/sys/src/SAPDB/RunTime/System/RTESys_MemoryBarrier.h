/*!========================================================================

  @file         RTESys_MemoryBarrier.h
  @ingroup      Runtime
  @author       RaymondR

  @brief        Memory Ordering.

  The term memory ordering refers to the order in which the processor issues reads (loads) and
  writes (stores) through the system bus to system memory. The different CPU architectures
  supports several memory ordering models depending on the implementation of the architecture.
  For example, the Intel 386 processor enforces program ordering (generally referred to 
  as strong ordering), where reads and writes are issued on the system bus in the order 
  they occur in the instruction stream under all circumstances. To optimize instruction 
  execution, the newer CPU architectures supporting performance enhancing operations such 
  as allowing reads to pass buffered writes. The goal of any of these order variations is
  to increase instruction execution speeds, while maintaining memory coherency, even in 
  multiple-processor systems.

  The RTESys_MemoryBarrier, RTESys_WriteMemoryBarrier, and RTESys_ReadMemoryBarrier 
  functions provide a performance-efficient way of insuring load and store memory 
  ordering between routines that produce weakly-ordered results and routines that 
  consume that data.

  @since        2003-04-08  13:24
  @sa           

  ==========================================================================

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

  \endif
 ============================================================================*/

#ifndef RTESYS_MEMORYBARRIER_H
#define RTESYS_MEMORYBARRIER_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/RTE_CompilerFeatures.h" /* for externC and VAR_ARRAY_REF ... */
#include "RunTime/RTE_Configure.h" /* dep=RTE_Configure.pl */

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#if defined(HP_IA64)

# include <ia64/sys/inline.h>
# define RTESys_MemoryBarrier _Asm_mf
# define RTESys_ReadMemoryBarrier _Asm_mf
# define RTESys_WriteMemoryBarrier _Asm_mf

#elif defined(AIX) && defined(_IBMR2)

externC void RTESys_WriteMemoryBarrier(void); 
#  pragma mc_func RTESys_WriteMemoryBarrier { "7c2004ac" } /* lwsync */ 
#  pragma reg_killed_by RTESys_WriteMemoryBarrier 

externC void RTESys_ReadMemoryBarrier(void); 
#  pragma mc_func RTESys_ReadMemoryBarrier { "4c00012c" } /* isync */ 
#  pragma reg_killed_by RTESys_ReadMemoryBarrier 

externC void RTESys_MemoryBarrier(void); 
#  pragma mc_func   RTESys_MemoryBarrier { "7c0004ac" } /* sync */ 
#  pragma reg_killed_by RTESys_MemoryBarrier 

/* WIN && LINUX */
#elif defined( IA64 ) && defined( __INTEL_COMPILER )

#if defined(LINUX)
#include <ia64intrin.h>
#endif

# define RTESys_MemoryBarrier()      __mf()
# define RTESys_ReadMemoryBarrier()  __mf()
# define RTESys_WriteMemoryBarrier() __mf()

#elif defined(LINUX) && defined(IA64)

# define RTESys_MemoryBarrier()      __asm__ __volatile__("mf":::"memory")
# define RTESys_ReadMemoryBarrier()  __asm__ __volatile__("mf":::"memory")
# define RTESys_WriteMemoryBarrier() __asm__ __volatile__("mf":::"memory")

#elif defined(LINUX) && defined(X86_64)

/* from #include <asm-x86_64/system.h> mb(),rmb(),wmb() definitions */

# define RTESys_MemoryBarrier()      __asm__ __volatile__("mfence":::"memory")
# define RTESys_ReadMemoryBarrier()  __asm__ __volatile__("lfence":::"memory")
# define RTESys_WriteMemoryBarrier() __asm__ __volatile__("sfence":::"memory")

#elif defined(LINUX) && defined(SDBonPPC64)

# define RTESys_MemoryBarrier()      __asm__ __volatile__(  "sync":::"memory")
# define RTESys_ReadMemoryBarrier()  __asm__ __volatile__(" isync":::"memory")
# define RTESys_WriteMemoryBarrier() __asm__ __volatile__("lwsync":::"memory")

#elif defined(LINUX) && defined(S390X)

# define RTESys_MemoryBarrier()      __asm__ __volatile__("BCR 15,0":::"memory")
# define RTESys_ReadMemoryBarrier()  __asm__ __volatile__("BCR 15,0":::"memory")
# define RTESys_WriteMemoryBarrier() __asm__ __volatile__("BCR 15,0":::"memory")

#elif defined(SOLARIS) && defined(SDB_HAS_SYS_ATOMIC)

#include <sys/atomic.h>
# define RTESys_MemoryBarrier()      membar_producer(), membar_consumer()
# define RTESys_ReadMemoryBarrier()  membar_consumer()
# define RTESys_WriteMemoryBarrier() membar_producer()

#else
/*
  The macros above define all three types of memory barriers....
 */
#  if defined (_WIN64) || defined(PA11) || defined(PA20W) || defined(ALPHA) || (defined(LINUX) && defined(I386))
/*
  The other coding implements a single type of memory barrier...
 */
#     define RTESys_ReadMemoryBarrier      RTESys_MemoryBarrier
#     define RTESys_WriteMemoryBarrier     RTESys_MemoryBarrier
#  endif

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


/*!------------------------------------------------------------------
   @brief   'RTESys_WriteMemoryBarrier' performs a serializing operation
               on all store-to-memory instructions that were issued prior the 
               'RTESys_WriteMemoryBarrier' function. This serializing operation
               guarantees  that every store instruction that precedes in program
               order the 'RTESys_WriteMemoryBarrier' function is globally visible 
               before any store instruction that follows 'RTESys_WriteMemoryBarrier'
               is globally visible. 'RTESys_WriteMemoryBarrier' is ordered with 
               respect to store instructions but is not ordered with respect to load
               instructions.

               Weakly ordered memory types can be used to achieve higher processor
               performance through such techniques as out-of-order issue, 
               write-combining, and write-collapsing. The degree to which a consumer
               of data recognizes or knows that the data is weakly ordered varies 
               among applications and may be unknown to the producer of this data.
               'RTESys_WriteMemoryBarrier' provides a performance-efficient way of 
               insuring store ordering between routines that produce weakly-ordered
               results and routines that consume this data.
   
   @returns        none

 ------------------------------------------------------------------*/

externC  void RTESys_WriteMemoryBarrier ( void );

/*!------------------------------------------------------------------
   @brief   'RTESys_MemoryBarrier' performs a serializing operation 
              on all load-from-memory and store-to-memory instructions 
              that were issued prior the 'RTESys_MemoryBarrier' function. 
              This serializing operation guarantees that every load and 
              store instruction that precedes in program order the 
              'RTESys_MemoryBarrier' function is globally visible before 
              any load or store instruction that follows 'RTESys_MemoryBarrier'
              is globally visible. 'RTESys_MemoryBarrier' is ordered with respect 
              to all load and store instructions.

              Weakly ordered memory types can be used to achieve higher processor
              performance through such techniques as out-of-order issue, 
              speculative reads, write-combining, and write-collapsing.
              The degree to which a consumer of data recognizes or knows that the
              data is weakly ordered varies among applications and may be unknown to
              the producer of this data. 'RTESys_MemoryBarrier' provides a 
              performance-efficient way of ensuring load and store ordering between
              routines that produce weakly-ordered results and routines that consume 
              that data.
   
   @returns        none

 ------------------------------------------------------------------*/

externC  void RTESys_MemoryBarrier ( void );

/*!------------------------------------------------------------------
   @brief   'RTESys_ReadMemoryBarrier' performs a serializing operation
              on all load-from-memory instructions that were issued prior 
              the 'RTESys_ReadMemoryBarrier' function. This serializing 
              operation guarantees that every load instruction that
              precedes in program order the 'RTESys_ReadMemoryBarrier' function
              is globally visible before any load instruction that follows  
              'RTESys_ReadMemoryBarrier' is globally visible. 
              'RTESys_ReadMemoryBarrier' is ordered with respect to load instructions 
              but is not ordered with respect to store instructions.

              Weakly ordered memory types can be used to achieve higher processor 
              performance through such techniques as out-of-order issue and speculative
              reads. The degree to which a consumer of data recognizes or knows that
              the data is weakly ordered varies among applications and may be unknown to
              the producer of this data. 'RTESys_ReadMemoryBarrier' provides a 
              performance-efficient way of insuring load ordering between routines
              that produce weakly-ordered results and routines that consume that data.

   @returns        none

 ------------------------------------------------------------------*/

externC  void RTESys_ReadMemoryBarrier ( void );

#endif /* HP_IA64 uses compiler inlines... */

#endif  /* RTESYS_MEMORYBARRIER_H */
