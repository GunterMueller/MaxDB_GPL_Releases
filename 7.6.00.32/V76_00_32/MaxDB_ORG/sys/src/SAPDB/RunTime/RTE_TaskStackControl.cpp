/*!
  @file           RTE_TaskStackControl.cpp
  @author         JoergM
  @ingroup        Runtime
  @brief          UNIX Task Stack Handling

  Implements the task stack control for all user kernel tasks.

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
/*! @copydoc "RunTime/RTE_ITaskStackControl.hpp" */
#include "RunTime/RTE_ITaskStackControl.hpp"
#include "RunTime/MemoryManagement/RTEMem_RteAllocator.hpp"
#include "RunTime/RTE_Crash.hpp"
#include "RunTime/System/RTESys_AtomicOperation.hpp"
#include "RunTime/System/RTESys_MemoryBarrier.h"
#include "RunTime/System/RTESys_MemProtect.h"
#include "geo007_1.h" /* MSGD content... */
#include "geo50_0.h" /* kgs for XPARAM */
#include "geo002.h" /* XPARAM(showMaxStackUse/fUseProcessScope) */
#include "geo70k.h" /* NUM_GUARDPAGES_EO70 */
#include "heo07.h"
#include "RunTime/MemoryManagement/RTEMem_PseudoStackAllocator.hpp"

/*!
   @class RTE_TaskStackControl
   @brief local structure describing task chunk parameters
 */
class RTE_TaskStackControl : public RTE_ITaskStackControl
{
protected:
    /*!
       container for a mapping of a single task stack array
     */
    struct TaskStackMapping {
        struct TaskStackMapping *m_Next;     ///< next task stack in chain of task stack mappings
        SAPDB_Byte *m_StartOfFirstTaskStack; ///< address of first byte of first task stack
        SAPDB_Byte *m_EndOfLastTaskStack;    ///< address behind end of last task stack
        SAPDB_ULong m_SingleStackSize;       ///< Sizeof single task stack
        SAPDB_ULong m_SingleGapSize;         ///< Sizeof single gap between task stacks
        RTE_TaskId  m_FirstId;               ///< TaskId associated with first task stack
    };

public:
    /*!
      @brief get singleton instance
     */
    static RTE_TaskStackControl &Instance();

    /*!
      @brief Once called function to initialize task stack control
      @return specific type of task stack control created
     */
    virtual RTE_ITaskStackControl::TypeOfStackControl const GetTypeOfStackControl() const { return m_TypeOfStackControl; }

    /*!
      @brief Once called function to initialize task stack control

      @param numberOfTasks [in] number of tasks
      @param pFirst [in] pointer to first task control record
      @param pLast [in] pointer to last task control record
     */
    virtual void Initialize( SAPDB_Long numberOfTasks, 
                             struct TASK_TYPE *pFirst,
                             struct TASK_TYPE *pLast) = 0;

    /*!
      @brief Using current stack context return task control block of calling task or 0 if not inside task stack
      @return Task control block pointer of calling task or 0 if not on task stack
    */
    virtual struct TASK_TYPE * GetTaskControl() const
    {
        // atomic reading of top of queue
        TaskStackMapping *pMapping = m_FirstMapping;
        SAPDB_Byte *CompareAddress = (SAPDB_Byte *)&pMapping;
#ifdef DEBUG_RTE
char eBuffer[256];
#endif
        while ( pMapping )
        {
            if ( CompareAddress >= pMapping->m_StartOfFirstTaskStack
              && CompareAddress <  pMapping->m_EndOfLastTaskStack )
            {
                SAPDB_ULong totalStackSize = pMapping->m_SingleStackSize + pMapping->m_SingleGapSize;

                SAPDB_Long indexInStackArray = (CompareAddress - pMapping->m_StartOfFirstTaskStack)
                                             / totalStackSize;

                if (             0 == pMapping->m_SingleGapSize
                  || CompareAddress < ( pMapping->m_StartOfFirstTaskStack 
                                      + ( indexInStackArray * totalStackSize )
                                      + pMapping->m_SingleStackSize ) )
                {
                    struct TASK_TYPE * tcb = (KGS->pFirstTaskCtrl + pMapping->m_FirstId - 1 + indexInStackArray);
#ifdef DEBUG_RTE
SAPDB_sprintf(eBuffer, sizeof(eBuffer), "CompareAddress 0x%p Mapping 0x%p ---> Task%d", CompareAddress, pMapping, tcb->index);
MSGD(( INFO_TEST_WILDCARD, eBuffer ));
#endif
#ifdef SAPDB_SLOW
                    if ( CompareAddress >= tcb->StackChunkEnd )
                    {
                        RTE_Crash(SAPDBErr_MessageList()); // internal error : address not within task stack range
                    }
#endif /* SAPDB_SLOW */
                    return tcb;
                }
            }
            pMapping = pMapping->m_Next;
        }
#ifdef DEBUG_RTE
SAPDB_sprintf(eBuffer, sizeof(eBuffer), "CompareAddress 0x%p ---> NO MATCH!", CompareAddress );
MSGD(( ERR_TEST_WILDCARD, eBuffer ));
#endif
        return (struct TASK_TYPE *)0;
    }

    /*!
      @brief The tasks may have the stack on stack of UKT thread. In that case the mapping information
      is known at a very late moment. This call defines the interface used in such a case.
      The call allows to associate a large number of task stack with a single call. This is used in a
      user task thread, where lots of identical tasks are added.

      If stack address space is not used for task stack, this call is not implemented.

      @param firstStack [in] address of first stack to map
      @param singleStackSize [in] size of a single stack size in bytes
      @param firstId [in] the first task id associated with the first task stack
      @param numberOfTasksToAdd [in] the number of tasks added with identical task stack size
      @return true if task stack mapping was added, false if not
    */
    virtual SAPDB_Bool AddTaskStackMapping( SAPDB_Byte *firstStack,
                                            SAPDB_ULong singleStackSize,
                                            RTE_TaskId  firstId,
                                            SAPDB_UInt4 numberOfTasksToAdd )
    {
        TaskStackMapping *pNewMapping = 
            (TaskStackMapping *)RTEMem_RteAllocator::Instance().Allocate(sizeof(TaskStackMapping));
        if ( !pNewMapping )
        {
            return false;
        }

        pNewMapping->m_StartOfFirstTaskStack = firstStack;
        pNewMapping->m_EndOfLastTaskStack = firstStack + (singleStackSize * numberOfTasksToAdd);
        pNewMapping->m_SingleStackSize = singleStackSize ;
        pNewMapping->m_SingleGapSize = 0;
        pNewMapping->m_FirstId = firstId;

        AddTaskStackMapping( *pNewMapping );
        return true;
    }

    /*!
      @brief The tasks may have the stack on stack of UKT thread. In that case the mapping information
      is known at a very late moment. This call defines the interface used in such a case.
      The call allows to associate a large number of task stack with a single call. This is used in a
      user task thread, where lots of identical tasks are added.

      If stack address space is not used for task stack, this call is not implemented.

      @param newMapping [in] structure filled with mapping
    */
    void  AddTaskStackMapping( TaskStackMapping &newMapping )
    {
        // atomic replacement of top of queue
        TaskStackMapping *pLastFirstMapping = m_FirstMapping;
        do {
            newMapping.m_Next = pLastFirstMapping;
    		RTESys_WriteMemoryBarrier();
        } while ( !RTESys_CompareAndExchange( m_FirstMapping, 
#ifdef OSF1
                                              pLastFirstMapping,
                                              &newMapping,
#else
                                              const_cast<TaskStackMapping * const>(pLastFirstMapping),
                                              const_cast<TaskStackMapping * const>(&newMapping),
#endif
                                              pLastFirstMapping ) );

#ifdef DEBUG_RTE
char eBuffer[256];
TaskStackMapping *pMapping = &newMapping;
SAPDB_sprintf(eBuffer, sizeof(eBuffer), "Added newMapping 0x%p Start 0x%p End 0x%p FirstId %d SingleStackSize %ld SingleGapSize %ld", 
            pMapping,
            pMapping->m_StartOfFirstTaskStack,
            pMapping->m_EndOfLastTaskStack,
            pMapping->m_FirstId,
            pMapping->m_SingleStackSize, 
            pMapping->m_SingleGapSize );
MSGD(( INFO_TEST_WILDCARD, eBuffer ));
#endif
    }

protected:
    /*!
      @brief Calculate the maximum task stack size for all tasks
  
      Loop over all tasks and get the maximum. Take into account that maximum cannot be smaller than minimum thread stack size
      @param numberOfTasks [in] number of tasks defined
      @param pFirst [in] first task control
      @param pLast [in] last task control
     */
    void CalculateMaxTaskStack( SAPDB_Long numberOfTasks,
                                struct TASK_TYPE *pFirst, 
                                struct TASK_TYPE *pLast);

    /*!
      @brief Try to allocate and guard specified memory range

      If allacation at fixed address fails, it is fatal. If protection fails, it is a warning only...

      @param startAddress [in] start address of memory area to protect
      @param guardSize [in] Size of memory area to protect
     */
    void TryToGuard(SAPDB_Byte *startAddress, SAPDB_Long guardSize);

protected:
    /*!
      @brief ctor
      @param typeOfStackControl [in] type of task stack control
     */
    RTE_TaskStackControl(RTE_ITaskStackControl::TypeOfStackControl const typeOfStackControl);

    /*!
      @brief reserver or allocate big memory chunk
      @param tryReserve [in] indicate to try to reserve chunk
     */
    void ReserveOrAllocateBigChunk(SAPDB_Bool tryReserve);

    /*!
      @brief report consumed memory
     */
    void ConsumedMemoryInfo();

    SAPDB_Byte *m_StartAddress;             /*!< start address of complete task stack chunk */
    SAPDB_Byte *m_StartOfFirstTaskStack;    /*!< start address of first task stack in chunk */
    SAPDB_Long  m_TotalSize;                /*!< total size of task stack chunk including all guard pages */
    SAPDB_Long  m_SingleAlignedStackSize;   /*!< aligned stack size of single task */
    SAPDB_Long  m_SingleGuardSize;          /*!< size of a single guard area */
    SAPDB_Long  m_SingleProtectedStackSize; /*!< size of a single stack area including guard pages */
    SAPDB_Long  m_NumberOfTasks;            /*!< Number of tasks contained in chunk */
    SAPDB_Long  m_consumedPhysicalMemory;   /*!< Number of bytes used for stack pages */
    SAPDB_Long  m_consumedGuardedMemory;    /*!< Number of bytes used for guard pages */
    SAPDB_ULong m_maxTaskStackSize;         /*!< Maximum task stack size */

protected:
    SAPDB_Bool  m_useReservedAreaAndGaps;   /*!< Flag to show mprotect calls are not needed */
private:
    TaskStackMapping            *m_FirstMapping; /*!< first mapping */
    SAPDBMem_SynchronizedPseudoAllocator  &m_PseudoAllocator; /*!< for allocator statistics systemview */
    RTE_ITaskStackControl::TypeOfStackControl const m_TypeOfStackControl; /*!< type of derived class */
public:
    static RTE_ITaskStackControl *m_Instance; ///< singleton pointer
}; /* endclass RTE_TaskStackControl */

RTE_ITaskStackControl * RTE_TaskStackControl::m_Instance = 0;

/*!
  @class RTE_LimitedMmapSectionsTaskStackControl
  @brief stack control for limited number of mmap sections
 */
class RTE_LimitedMmapSectionsTaskStackControl: public RTE_TaskStackControl
{
public:
    /*!
      @brief get singleton instance
     */
    RTE_LimitedMmapSectionsTaskStackControl();
    /*!
      @brief Update task control records
      
      For limited address space, the single chunk allocation is all we do
      so only an update of task control resords is needed
      @param pFirst [in] first task control
      @param pLast [in] last task control
     */
    void UpdateTaskControlRecords(struct TASK_TYPE *pFirst, struct TASK_TYPE *pLast);

    /*!
      @brief Once called function to initialize task stack control

      @param numberOfTasks [in] number of tasks
      @param pFirst [in] pointer to first task control record
      @param pLast [in] pointer to last task control record
     */
    virtual void Initialize( SAPDB_Long numberOfTasks,
                             struct TASK_TYPE *pFirst,
                             struct TASK_TYPE *pLast );
private:
    TaskStackMapping m_Mapping; ///< The only mapping needed mapping
}; /* endclass RTE_LimitedMmapSectionsTaskStackControl */


/*!
  @class RTE_LimitedAddressSpaceTaskStackControl
  @brief stack control for limited address space
 */
class RTE_LimitedAddressSpaceTaskStackControl: public RTE_TaskStackControl
{
public:
    /*!
      @brief ctor 
     */
    RTE_LimitedAddressSpaceTaskStackControl();
    /*!
      @brief Once called function to initialize task stack control

      @param numberOfTasks [in] number of tasks
      @param pFirst [in] pointer to first task control record
      @param pLast [in] pointer to last task control record
     */
    virtual void Initialize( SAPDB_Long numberOfTasks,
                             struct TASK_TYPE *pFirst,
                             struct TASK_TYPE *pLast );
private:
    /*!
      @brief Try to guard all pages of a task stack

        If 64Bit address room allows it, the guard pages in front of task stack area are setup here

      @param pFirst [in] first task control
      @param pLast [in] last task control
      @return The number of bytes consumed by guard pages
     */
    SAPDB_Long TryToGuardStartPages(struct TASK_TYPE *pFirst, struct TASK_TYPE *pLast);

    /*!
     @brief Allocate and protect task stacks and guardpages (guardpages only if needed) 

     Take into account, that two different sizes are used for user task stack and non user task stacks

     @param pFirst [in] first task control
     @param pLast [in] last task control
     */
    void AllocateAndProtectTaskStacks(struct TASK_TYPE *pFirst, struct TASK_TYPE *pLast);
    /*!
      @brief Calculate the maximum task stack size for non user tasks

      Loop over all tasks and get the maximum task stack for non user tasks.
      Count the number of these non user tasks too.
  
      @param pFirst [in] first task control
      @param pLast [in] last task control
      @return maximum task stack size for non user tasks in bytes
     */
    SAPDB_ULong CalculateMaxNonUserTaskStack( struct TASK_TYPE *pFirst,
                                              struct TASK_TYPE *pLast );
    SAPDB_Byte *m_StartOfFirstUserTaskStack;/* start address of first user task stack in chunk */
    SAPDB_Long  m_SingleAlignedNonUserStackSize; /* aligned stack size of single non user task */
    SAPDB_Long  m_SingleProtectedNonUserStackSize; /* size of a single stack area including guard pages */
    SAPDB_Long  m_NumberOfNonUserTasks;     /* Number of non user tasks contained in chunk */
    TaskStackMapping m_NonUserTaskMapping; ///< The mapping for non user tasks
    TaskStackMapping m_UserTaskMapping; ///< The mapping for user tasks
}; /* endclass RTE_LimitedAddressSpaceTaskStackControl */

/*!
  @class RTE_BigAddressSpaceTaskStackControl
  @brief stack control for big address space
 */
class RTE_BigAddressSpaceTaskStackControl: public RTE_TaskStackControl
{
public:
    /*!
      @brief ctor
     */
    RTE_BigAddressSpaceTaskStackControl();
    /*!
      @brief Once called function to initialize task stack control

      @param numberOfTasks [in] number of tasks
      @param pFirst [in] pointer to first task control record
      @param pLast [in] pointer to last task control record
     */
    virtual void Initialize( SAPDB_Long numberOfTasks,
                             struct TASK_TYPE *pFirst,
                             struct TASK_TYPE *pLast );
private:
    /*!
      @brief Try to guard all pages of a task stack

        If 64Bit address room allows it, the guard pages in front of task stack area are setup here

      @param pFirst [in] first task control
      @param pLast [in] last task control
      @return The number of bytes consumed by guard pages
     */
    SAPDB_Long TryToGuardStartPages(struct TASK_TYPE *pFirst, struct TASK_TYPE *pLast);

    /*!
       @brief Allocate and protect task stacks and guardpages (guardpages only if needed) 
       @param pFirst [in] first task control
       @param pLast [in] last task control
     */
    void AllocateAndProtectTaskStacks(struct TASK_TYPE *pFirst, struct TASK_TYPE *pLast);

    TaskStackMapping m_Mapping; ///< The only mapping needed mapping
}; /* endclass RTE_BigAddressSpaceTaskStackControl */

/*!
  @class RTE_StackAddressSpaceTaskStackControl
  @brief stack on stack allocation task stack control
 */
class RTE_StackAddressSpaceTaskStackControl: public RTE_TaskStackControl
{
public:
    /*!
      @brief ctor
     */
    RTE_StackAddressSpaceTaskStackControl();

    /*!
      @brief Once called function to initialize task stack control

      @param numberOfTasks [in] number of tasks
      @param pFirst [in] pointer to first task control record
      @param pLast [in] pointer to last task control record
     */
    virtual void Initialize( SAPDB_Long numberOfTasks,
                             struct TASK_TYPE * pFirst,
                             struct TASK_TYPE * pLast )
    {}
}; /* endclass RTE_StackAddressSpaceTaskStackControl */

//------------------

/*!
  @brief Access to task stack control instance
  This call uses kernel parameter set to select the right task stack control class
 */
RTE_ITaskStackControl & RTE_ITaskStackControl::Instance()
{

    if ( KGS && XPARAM(fUseCoroutines) && XPARAM(fUseStackOnStack) )
    {
        SAPDBMEM_STATIC_RAW_ALLOCATE(Space, sizeof(RTE_StackAddressSpaceTaskStackControl));
        if ( !RTE_TaskStackControl::m_Instance )
        {
            RTE_TaskStackControl::m_Instance = new (Space) RTE_StackAddressSpaceTaskStackControl();
        }
    }
    else
    {
#if defined(HAS_VERY_LIMITED_VIRTUAL_ADDRESS_SPACE)
        SAPDBMEM_STATIC_RAW_ALLOCATE(Space, sizeof(RTE_LimitedAddressSpaceTaskStackControl));
        if ( !RTE_TaskStackControl::m_Instance )
        {
            RTE_TaskStackControl::m_Instance = new (Space) RTE_LimitedAddressSpaceTaskStackControl();
        }
#elif defined(HAS_LIMITED_NUMBER_OF_MMAP_SECTIONS)
        SAPDBMEM_STATIC_RAW_ALLOCATE(Space, sizeof(RTE_LimitedMmapSectionsTaskStackControl));
        if ( !RTE_TaskStackControl::m_Instance )
        {
            RTE_TaskStackControl::m_Instance = new (Space) RTE_LimitedMmapSectionsTaskStackControl();
        }
#else
        SAPDBMEM_STATIC_RAW_ALLOCATE(Space, sizeof(RTE_BigAddressSpaceTaskStackControl));
        if ( !RTE_TaskStackControl::m_Instance )
        {
            RTE_TaskStackControl::m_Instance = new (Space) RTE_BigAddressSpaceTaskStackControl();
        }
#endif
    }
    SAPDBERR_ASSERT_STATE(RTE_TaskStackControl::m_Instance != NULL);
    return *RTE_TaskStackControl::m_Instance;
}

//-----------------------------------------------------------------------------
//------------------ Contructors for singleton --------------------------------
//-----------------------------------------------------------------------------

RTE_TaskStackControl::RTE_TaskStackControl(RTE_ITaskStackControl::TypeOfStackControl const typeOfStackControl)
: m_StartAddress(0),
  m_StartOfFirstTaskStack(0),
  m_TotalSize(0),
  m_SingleAlignedStackSize(0),
  m_SingleGuardSize(0),
  m_SingleProtectedStackSize(0),
  m_NumberOfTasks(0),
  m_maxTaskStackSize(0),
  m_useReservedAreaAndGaps(false),
  m_FirstMapping(0),
  m_PseudoAllocator(RTEMem_PseudoStackAllocator::Instance()),
  m_TypeOfStackControl(typeOfStackControl)
{}

//--

RTE_StackAddressSpaceTaskStackControl::RTE_StackAddressSpaceTaskStackControl()
: RTE_TaskStackControl(RTE_ITaskStackControl::StackAddressSpace)
{}

//--

RTE_LimitedAddressSpaceTaskStackControl::RTE_LimitedAddressSpaceTaskStackControl()
: m_StartOfFirstUserTaskStack(0),
  m_SingleAlignedNonUserStackSize(0),
  m_SingleProtectedNonUserStackSize(0),
  m_NumberOfNonUserTasks(0),
  RTE_TaskStackControl(RTE_ITaskStackControl::LimitedAddressSpace)
{}

//--

RTE_LimitedMmapSectionsTaskStackControl::RTE_LimitedMmapSectionsTaskStackControl()
: RTE_TaskStackControl(RTE_ITaskStackControl::LimitedMmapSections)
{}

//--

RTE_BigAddressSpaceTaskStackControl::RTE_BigAddressSpaceTaskStackControl()
: RTE_TaskStackControl(RTE_ITaskStackControl::BigAddressSpace)
{}

//-----------------------------------------------------------------------------
//--------- Reserve or allocate the big chunk ---------------------------------
//-----------------------------------------------------------------------------

void RTE_TaskStackControl::CalculateMaxTaskStack(
    SAPDB_Long numberOfTasks,
    struct TASK_TYPE *pFirst,
    struct TASK_TYPE *pLast)
{
    m_NumberOfTasks = numberOfTasks;

    for ( struct TASK_TYPE *tcb = pFirst; tcb <= pLast; ++tcb )
    {
        if ( tcb->stack_size > m_maxTaskStackSize )
        {
            m_maxTaskStackSize = tcb->stack_size;
        }
    }

    if ( !XPARAM(fUseCoroutines) )
    {
        if ( m_maxTaskStackSize < sqlget_thread_min_stack() )
        {
            m_maxTaskStackSize = sqlget_thread_min_stack();
        }
    }

    MSGD (( INFO_MAX_TASK_STACK, m_maxTaskStackSize/1024 ));

    m_SingleAlignedStackSize   = RTE_ISystem::Instance().RoundUpToSystemPageSize(m_maxTaskStackSize);
} // RTE_TaskStackControl::CalculateMaxTaskStack

// -----------------------------------------------------

void RTE_TaskStackControl::TryToGuard(SAPDB_Byte *startAddress, SAPDB_Long guardSize)
{
#if !defined(USE_VMALLOC_NOT_MMAP) && defined(USE_FIXED_MMAP_ADDRESS)
    if ( RTE_ITaskStackControl::StackAddressSpace != m_TypeOfStackControl )
    {
        if ( !RTE_ISystem::Instance().UncachedAllocSystemPagesAtFixedAddress(startAddress, guardSize, true) )
        {
            MSGD(( ERR_REALLOCATING_TASK_STACK, startAddress, startAddress + guardSize - 1 ));
            RTE_Crash(SAPDBErr_MessageList());
        }
    }
#endif /* !USE_VMALLOC_NOT_MMAP && USE_FIXED_MMAP_ADDRESS */

    if ( RTE_ISystem::Instance().MemProtect( startAddress, guardSize, MEMORYPROTECTION_NOACCESS ) )
    {
        MSGD(( WRN_PROTECTING_TASK_STACK, startAddress, startAddress + guardSize - 1 ));
    }
} // RTE_TaskStackControl::TryToGuard

// -----------------------------------------------------------

void RTE_TaskStackControl::ReserveOrAllocateBigChunk(SAPDB_Bool tryReserve)
{
    if ( tryReserve )
    {
        // to be replaced by usage of RTEMem_ProtectedAllocator ????....
        m_StartAddress = (SAPDB_Byte *)RTE_ISystem::Instance().ReserveSystemPages(m_TotalSize);
        if ( m_StartAddress )
        {
            MSGD(( INFO_RESERVED_TASK_STACK, m_TotalSize/1024, m_StartAddress ));
        }
        else
        {
            MSGD(( INFO_SYSTEM_CHOOSEN_TASK_STACK ));
        }
    }
    else
    {
        m_StartAddress             = (SAPDB_Byte *)0; // Any address possible
        MSGD(( INFO_SYSTEM_CHOOSEN_TASK_STACK ));
    }

    if ( 0 == m_StartAddress )
    {
        m_StartAddress = (SAPDB_Byte *)RTE_ISystem::Instance().UncachedAllocSystemPagesAtFixedAddress((void *)0, m_TotalSize, true);
    }
    else
    {
        m_useReservedAreaAndGaps = true;
        MSGD(( INFO_ALLOCATING_TASK_STACK_GAPS ));
    }

    MSGD(( INFO_ALLOCATING_TASK_STACK1, m_NumberOfTasks ));
    MSGD(( INFO_ALLOCATING_TASK_STACK2, m_SingleAlignedStackSize/1024 ));
    MSGD(( INFO_ALLOCATING_TASK_STACK3, m_SingleGuardSize/1024 ));

    if ( !m_StartAddress )
    {
        MSGD(( ERR_ALLOCATING_TASK_STACK1 ));
        MSGD(( ERR_ALLOCATING_TASK_STACK2 ));
        RTE_Crash(SAPDBErr_MessageList());
    }
    else
    {
        m_StartOfFirstTaskStack = m_StartAddress + m_SingleGuardSize;
        MSGD(( INFO_ALLOCATING_TASK_STACK4,
               m_StartAddress,
               m_StartAddress + m_TotalSize - 1 ));
    }
} // RTE_TaskStackControl::ReserveOrAllocateBigChunk

// -------------------------------------------------------------

void RTE_LimitedMmapSectionsTaskStackControl::UpdateTaskControlRecords(
    struct TASK_TYPE *pFirst, 
    struct TASK_TYPE *pLast)
{
    //-----------------------------------------------------------------------------
    // Update individual task control record stack chunk information
    //-----------------------------------------------------------------------------

    SAPDB_Byte *stackStartAddress = m_StartOfFirstTaskStack;

    for ( struct TASK_TYPE * tcb = pFirst; tcb <= pLast; ++tcb )
    {
        SAPDB_Int4 alignedStackSize = RTE_ISystem::Instance().RoundUpToSystemPageSize(tcb->stack_size);
        if ( !XPARAM(fUseCoroutines) )
        {
            if ( alignedStackSize < sqlget_thread_min_stack() )
            {
                alignedStackSize = RTE_ISystem::Instance().RoundUpToSystemPageSize(sqlget_thread_min_stack());
            }
        }

        tcb->StackChunkStart = stackStartAddress;
        tcb->StackChunkEnd   = stackStartAddress + alignedStackSize;
        tcb->stack_size      = alignedStackSize;

        tcb->GuardPageBegin  = 0;
        tcb->GuardPageEnd    = 0;

        m_consumedPhysicalMemory += m_SingleProtectedStackSize;

        // now we can step to the next gap
        stackStartAddress        += m_SingleProtectedStackSize;
    }
} // RTE_LimitedMmapSectionsTaskStackControl::UpdateTaskControlRecords

// --------------------------------------------------------------------------

void RTE_BigAddressSpaceTaskStackControl::AllocateAndProtectTaskStacks(
    struct TASK_TYPE *pFirst,
    struct TASK_TYPE *pLast)
{
    if ( !m_useReservedAreaAndGaps )
    {
#if !defined(USE_VMALLOC_NOT_MMAP) && defined(USE_FIXED_MMAP_ADDRESS)
        // We got the big chunk, but now we free it again, to reduce the amount of memory used...
        if ( !RTE_ISystem::Instance().UncachedFreeSystemPages(m_StartAddress, m_TotalSize) )
        {
            MSGD(( ERR_FREE_BIG_TASK_STACK_CHUNK ));
            RTE_Crash(SAPDBErr_MessageList());
        }
#endif /* !USE_VMALLOC_NOT_MMAP && USE_FIXED_MMAP_ADDRESS */

        if ( NUM_GUARDPAGES_EO70 > 0 )
        {
            m_consumedGuardedMemory = TryToGuardStartPages(pFirst, pLast);
        }
    }

    //-----------------------------------------------------------------------------
    // Allocate the stack pages at fixed addresses and allocate guard pages if wanted and no reservation was possible
    //-----------------------------------------------------------------------------

    SAPDB_Byte *stackStartAddress = m_StartOfFirstTaskStack;

    for ( struct TASK_TYPE *tcb = pFirst; tcb <= pLast; ++tcb )
    {
        SAPDB_Int4 alignedStackSize = RTE_ISystem::Instance().RoundUpToSystemPageSize(tcb->stack_size);
        if ( !XPARAM(fUseCoroutines) )
        {
            if ( alignedStackSize < sqlget_thread_min_stack() )
            {
                alignedStackSize = RTE_ISystem::Instance().RoundUpToSystemPageSize(sqlget_thread_min_stack());
            }
        }

        tcb->StackChunkStart = stackStartAddress;
        tcb->StackChunkEnd   = stackStartAddress + alignedStackSize;
        tcb->stack_size      = alignedStackSize;

#if !defined(USE_VMALLOC_NOT_MMAP) && defined(USE_FIXED_MMAP_ADDRESS)
        if ( !RTE_ISystem::Instance().UncachedAllocSystemPagesAtFixedAddress(stackStartAddress, alignedStackSize, true) )
        {
            MSGD(( ERR_REALLOCATING_TASK_STACK, 
                   stackStartAddress, 
                   stackStartAddress + alignedStackSize - 1 ));
            RTE_Crash(SAPDBErr_MessageList());
        }
#endif /* !USE_VMALLOC_NOT_MMAP && USE_FIXED_MMAP_ADDRESS */

        // check for extra guard page needed
        if ( (NUM_GUARDPAGES_EO70 > 0) 
          && (alignedStackSize < m_maxTaskStackSize) )
        {
            // the stack chunk is smaller than maximum
            // the gap is unprotected! So we have to add an extra guard page
            // if the gap is smaller than the wanted guard page size, the 
            // smaller gap is completely protected and the adjacing guard
            // page guaranties that the guard page size is sufficient
            SAPDB_Long extraGuardSize = m_maxTaskStackSize - alignedStackSize;
            
            if ( extraGuardSize > m_SingleGuardSize )
            {
                extraGuardSize = m_SingleGuardSize;
            }

            tcb->GuardPageBegin = stackStartAddress + alignedStackSize;

            if ( extraGuardSize < m_SingleGuardSize )
            {
                tcb->GuardPageEnd   = tcb->GuardPageBegin 
                                    + m_maxTaskStackSize - alignedStackSize // see comment above
                                    + m_SingleGuardSize;
            }
            else
            {
                tcb->GuardPageEnd   = tcb->GuardPageBegin + m_SingleGuardSize;
            }

            if ( !m_useReservedAreaAndGaps )
            {
                TryToGuard( m_StartOfFirstTaskStack 
                          + m_SingleProtectedStackSize * (tcb->index-1)
                          + alignedStackSize,
                            extraGuardSize);
                m_consumedGuardedMemory += extraGuardSize;
            }
        }

        m_consumedPhysicalMemory += alignedStackSize;

        // now we can step to the next gap
        stackStartAddress += m_SingleProtectedStackSize;
    }
} // BigAddressSpaceTaskStackControl::AllocateAndProtectTaskStacks

// --------------------------------------------------------------------------

void RTE_LimitedAddressSpaceTaskStackControl::AllocateAndProtectTaskStacks(
    struct TASK_TYPE *pFirst,
    struct TASK_TYPE *pLast)
{
    if ( !m_useReservedAreaAndGaps )
    {
#ifndef USE_VMALLOC_NOT_MMAP
        // We got the big chunk, but now we free it again, to reduce the amount of memory used...
        if ( !RTE_ISystem::Instance().UncachedFreeSystemPages(m_StartAddress, m_TotalSize) )
        {
            MSGD(( ERR_FREE_BIG_TASK_STACK_CHUNK ));
            RTE_Crash(SAPDBErr_MessageList());
        }
#endif /* USE_VMALLOC_NOT_MMAP */

        if ( NUM_GUARDPAGES_EO70 > 0 )
        {
            m_consumedGuardedMemory = TryToGuardStartPages(pFirst, pLast);
        }
    }

    //-----------------------------------------------------------------------------
    // Allocate the stack pages at fixed addresses and allocate guard pages if wanted and no reservation was possible
    //-----------------------------------------------------------------------------

    SAPDB_Byte *stackStartAddress = m_StartOfFirstTaskStack;

    for ( struct TASK_TYPE *tcb = pFirst; tcb <= pLast; ++tcb )
    {
        SAPDB_Int4 alignedStackSize = RTE_ISystem::Instance().RoundUpToSystemPageSize(tcb->stack_size);
        if ( !XPARAM(fUseCoroutines) )
        {
            if ( alignedStackSize < sqlget_thread_min_stack() )
            {
                alignedStackSize = RTE_ISystem::Instance().RoundUpToSystemPageSize(sqlget_thread_min_stack());
            }
        }

        tcb->StackChunkStart = stackStartAddress;
        tcb->StackChunkEnd   = stackStartAddress + alignedStackSize;
        tcb->stack_size      = alignedStackSize;

#if !defined(USE_VMALLOC_NOT_MMAP) && defined(USE_FIXED_MMAP_ADDRESS)
        if ( !RTE_ISystem::Instance().UncachedAllocSystemPagesAtFixedAddress(stackStartAddress, alignedStackSize, true) )
        {
            MSGD(( ERR_REALLOCATING_TASK_STACK, 
                   stackStartAddress, 
                   stackStartAddress + alignedStackSize - 1 ));
            RTE_Crash(SAPDBErr_MessageList());
        }
#endif /* !USE_VMALLOC_NOT_MMAP && USE_FIXED_MMAP_ADDRESS */

        // non user max task stack size and user max task stack size differ
        SAPDB_Long taskTypeMaxStackSize;

        if ( tcb->type != TT_US_EO00
          && tcb->type != TT_UT_EO00 )
        {
            taskTypeMaxStackSize = m_SingleAlignedNonUserStackSize;
        }
        else
        {
            taskTypeMaxStackSize = m_SingleAlignedStackSize;
        }

        // check for extra guard page needed
        if ( (NUM_GUARDPAGES_EO70 > 0) 
          && (alignedStackSize < taskTypeMaxStackSize) )
        {
            // the stack chunk is smaller than maximum
            // the gap is unprotected! So we have to add an extra guard page
            // if the gap is smaller than the wanted guard page size, the 
            // smaller gap is completely protected and the adjacing guard
            // page guaranties that the guard page size is sufficient
            SAPDB_Long extraGuardSize;
            
            extraGuardSize = taskTypeMaxStackSize - alignedStackSize;
            
            if ( extraGuardSize > m_SingleGuardSize )
            {
                extraGuardSize = m_SingleGuardSize;
            }

            tcb->GuardPageBegin = stackStartAddress + alignedStackSize;

            if ( extraGuardSize < m_SingleGuardSize )
            {
                tcb->GuardPageEnd   = tcb->GuardPageBegin 
                                    + taskTypeMaxStackSize - alignedStackSize // see comment above
                                    + m_SingleGuardSize;
            }
            else
            {
                tcb->GuardPageEnd   = tcb->GuardPageBegin + m_SingleGuardSize;
            }

            if ( !m_useReservedAreaAndGaps )
            {
                TryToGuard( stackStartAddress + alignedStackSize,
                            extraGuardSize);
                m_consumedGuardedMemory += extraGuardSize;
            }
        }

        m_consumedPhysicalMemory += alignedStackSize;

        // now we can step to the next gap
        if ( tcb->type != TT_US_EO00
          && tcb->type != TT_UT_EO00 )
        {
            stackStartAddress += m_SingleProtectedNonUserStackSize;
        }
        else
        {
            stackStartAddress += m_SingleProtectedStackSize;
        }

    }
} // RTE_LimitedAddressSpaceTaskStackControl::AllocateAndProtectTaskStacks

// -------------------------------------------------------------

void RTE_TaskStackControl::ConsumedMemoryInfo()
{
    MSGD(( INFO_ALLOCATING_TASK_STACK5, m_consumedPhysicalMemory/1024 ));
    MSGD(( INFO_ALLOCATING_TASK_STACK6, m_consumedGuardedMemory/1024 ));
    m_PseudoAllocator.AddBaseChunk(m_consumedPhysicalMemory);
    m_PseudoAllocator.AddChunk(m_consumedPhysicalMemory-m_consumedGuardedMemory);
} // RTE_TaskStackControl::ConsumedMemoryInfo()

// -------------------------------------------------------------

SAPDB_Long RTE_BigAddressSpaceTaskStackControl::TryToGuardStartPages(
   struct TASK_TYPE *pFirst,
   struct TASK_TYPE *pLast)
{
    SAPDB_Byte *guardStartAddress = m_StartAddress;

    TryToGuard( guardStartAddress, m_SingleGuardSize);

    for ( struct TASK_TYPE *tcb = pFirst; tcb <= pLast; ++tcb )
    {
        guardStartAddress += m_SingleProtectedStackSize;

        tcb->GuardPageBegin = guardStartAddress;
        tcb->GuardPageEnd   = guardStartAddress + m_SingleGuardSize;

        TryToGuard( guardStartAddress, m_SingleGuardSize);
    }

    return m_SingleGuardSize * (1 + m_NumberOfTasks);
} // RTE_BigAddressSpaceTaskStackControl::TryToGuardStartPages

// -------------------------------------------------------------

SAPDB_Long RTE_LimitedAddressSpaceTaskStackControl::TryToGuardStartPages(
   struct TASK_TYPE *pFirst,
   struct TASK_TYPE *pLast)
{
    SAPDB_Byte *guardStartAddress = m_StartAddress;

    TryToGuard( guardStartAddress, m_SingleGuardSize);

    for ( struct TASK_TYPE *tcb = pFirst; tcb <= pLast; ++tcb )
    {
        if ( tcb->type != TT_US_EO00
          && tcb->type != TT_UT_EO00 )
        {
            guardStartAddress += m_SingleProtectedNonUserStackSize;
        }
        else
        {
            guardStartAddress += m_SingleProtectedStackSize;
        }

        tcb->GuardPageBegin = guardStartAddress;
        tcb->GuardPageEnd   = guardStartAddress + m_SingleGuardSize;

        TryToGuard( guardStartAddress, m_SingleGuardSize);
    }

    return m_SingleGuardSize * (1 + m_NumberOfTasks);
} // RTE_LimitedAddressSpaceTaskStackControl::TryToGuardStartPages

// -----------------------------------------------------

SAPDB_ULong RTE_LimitedAddressSpaceTaskStackControl::CalculateMaxNonUserTaskStack(
 struct TASK_TYPE *pFirst,
 struct TASK_TYPE *pLast )
{
    struct  TASK_TYPE * tcb;
    SAPDB_ULong         maxNonUserTaskStackSize = 0;

    m_NumberOfNonUserTasks = 0;

    for ( tcb = pFirst; tcb <= pLast; ++tcb )
    {
        if ( tcb->type != TT_US_EO00
          && tcb->type != TT_UT_EO00 )
        {
            ++m_NumberOfNonUserTasks;     /* Number of non user tasks contained in chunk */

            if ( tcb->stack_size > maxNonUserTaskStackSize )
            {
                maxNonUserTaskStackSize = tcb->stack_size;
            }
        }
    }

    if ( !XPARAM(fUseCoroutines) )
    {
        if ( maxNonUserTaskStackSize < sqlget_thread_min_stack() )
        {
            maxNonUserTaskStackSize = sqlget_thread_min_stack();
        }
    }

    MSGD (( INFO_MAX_NON_USER_TASK_STACK, maxNonUserTaskStackSize/1024 ));

    return maxNonUserTaskStackSize;
} // RTE_LimitedAddressSpaceTaskStackControl::CalculateMaxNonUserTaskStack()

// -----------------------------------------------------
// RTE_TaskStackControl::Initialize implementations
// -----------------------------------------------------

void RTE_LimitedMmapSectionsTaskStackControl::Initialize(
    SAPDB_Long numberOfTasks, 
    struct TASK_TYPE *pFirst, 
    struct TASK_TYPE *pLast)
{
    CalculateMaxTaskStack(numberOfTasks, pFirst, pLast);

    m_SingleGuardSize          = 0;
    m_SingleProtectedStackSize = m_SingleAlignedStackSize;
    m_TotalSize                = m_NumberOfTasks * m_SingleProtectedStackSize; 

    ReserveOrAllocateBigChunk(false); // false means do not try to reserve

    UpdateTaskControlRecords(pFirst, pLast);

    m_Mapping.m_Next = 0;
    m_Mapping.m_StartOfFirstTaskStack = m_StartOfFirstTaskStack;
    m_Mapping.m_EndOfLastTaskStack = m_StartOfFirstTaskStack + m_TotalSize;
    m_Mapping.m_SingleStackSize = m_SingleAlignedStackSize;
    m_Mapping.m_SingleGapSize = 0;
    m_Mapping.m_FirstId = 1;

    AddTaskStackMapping( m_Mapping );

    ConsumedMemoryInfo();

} // RTE_LimitedMmapSectionsTaskStackControl::Initialize()

// -------------------------------------------------------------

void RTE_LimitedAddressSpaceTaskStackControl::Initialize(
    SAPDB_Long numberOfTasks, 
    struct TASK_TYPE *pFirst, 
    struct TASK_TYPE *pLast)
{
    m_SingleAlignedNonUserStackSize = 
        RTE_ISystem::Instance().RoundUpToSystemPageSize( CalculateMaxNonUserTaskStack(pFirst, pLast) );
    CalculateMaxTaskStack(numberOfTasks, pFirst, pLast);

    m_SingleGuardSize                 = RTE_ISystem::Instance().GetSystemPageSize() * NUM_GUARDPAGES_EO70;
    m_SingleProtectedStackSize        = m_SingleAlignedStackSize        + m_SingleGuardSize;
    m_SingleProtectedNonUserStackSize = m_SingleAlignedNonUserStackSize + m_SingleGuardSize;
    // calculate max task stack for non user tasks and count the number of non user tasks in the system
    m_TotalSize = ( m_NumberOfNonUserTasks * m_SingleProtectedNonUserStackSize )
                + ( (m_NumberOfTasks - m_NumberOfNonUserTasks) * m_SingleProtectedStackSize );

    ReserveOrAllocateBigChunk(true); // true means try to reserve, which on LINUX will always fail...

    AllocateAndProtectTaskStacks( pFirst, pLast );

    m_StartOfFirstUserTaskStack = m_StartOfFirstTaskStack
                                + (m_NumberOfNonUserTasks * m_SingleProtectedNonUserStackSize);

    m_NonUserTaskMapping.m_Next = 0;
    m_NonUserTaskMapping.m_StartOfFirstTaskStack = m_StartOfFirstTaskStack;
    // protected size times non user tasks minus gap at end
    m_NonUserTaskMapping.m_EndOfLastTaskStack = m_StartOfFirstTaskStack 
                                    + ( m_SingleProtectedNonUserStackSize * m_NumberOfNonUserTasks )
                                    - m_SingleGuardSize;
    m_NonUserTaskMapping.m_SingleStackSize = m_SingleAlignedNonUserStackSize;
    m_NonUserTaskMapping.m_SingleGapSize = m_SingleGuardSize;
    m_NonUserTaskMapping.m_FirstId = 1;

    AddTaskStackMapping( m_NonUserTaskMapping );

    m_UserTaskMapping.m_Next = 0;
    m_UserTaskMapping.m_StartOfFirstTaskStack = m_StartOfFirstUserTaskStack;
    // protected size times user tasks minus gap at end
    m_UserTaskMapping.m_EndOfLastTaskStack = m_StartOfFirstUserTaskStack
                                    + ( m_SingleProtectedStackSize * (m_NumberOfTasks - m_NumberOfNonUserTasks) )
                                    - m_SingleGuardSize;
    m_UserTaskMapping.m_SingleStackSize = m_SingleAlignedStackSize;
    m_UserTaskMapping.m_SingleGapSize = m_SingleGuardSize;
    m_UserTaskMapping.m_FirstId = m_NumberOfNonUserTasks + 1;

    AddTaskStackMapping( m_UserTaskMapping );

    ConsumedMemoryInfo();

} // RTE_LimitedAddressSpaceTaskStackControl::Initialize

// ------------------------------------------------------

void RTE_BigAddressSpaceTaskStackControl::Initialize(
    SAPDB_Long numberOfTasks, 
    struct TASK_TYPE *pFirst, 
    struct TASK_TYPE *pLast)
{
    CalculateMaxTaskStack(numberOfTasks, pFirst, pLast);

    m_SingleGuardSize          = RTE_ISystem::Instance().GetSystemPageSize() * NUM_GUARDPAGES_EO70;
    m_SingleProtectedStackSize = m_SingleAlignedStackSize + m_SingleGuardSize;
    m_TotalSize                = m_SingleGuardSize        + ( m_NumberOfTasks * m_SingleProtectedStackSize ); 

    ReserveOrAllocateBigChunk(true); // true means try to reserve

    AllocateAndProtectTaskStacks( pFirst, pLast );

    m_Mapping.m_Next = 0;
    m_Mapping.m_StartOfFirstTaskStack = m_StartOfFirstTaskStack;
    // total size minus gap at front and gap at end
    m_Mapping.m_EndOfLastTaskStack = m_StartOfFirstTaskStack + m_TotalSize - ( 2 * m_SingleGuardSize );
    m_Mapping.m_SingleStackSize = m_SingleAlignedStackSize;
    m_Mapping.m_SingleGapSize = m_SingleGuardSize;
    m_Mapping.m_FirstId = 1;

    AddTaskStackMapping( m_Mapping );

    ConsumedMemoryInfo();

} // RTE_BigAddressSpaceTaskStackControl::Initialize

