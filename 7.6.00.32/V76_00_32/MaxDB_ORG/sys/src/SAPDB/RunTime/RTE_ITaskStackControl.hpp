/*!
  @file           RTE_ITaskStackControl.hpp
  @author         JoergM
  @ingroup        Runtime
  @brief          UNIX task stack interface

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
#ifndef RTE_ITASKSTACKCONTROL_HPP
#define RTE_ITASKSTACKCONTROL_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/RTE_Types.hpp"
#include "SAPDBCommon/SAPDB_Singleton.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
    @class  RTE_ITaskStackControl
    @brief  Singleton Interface class to access and create task stacks

     This pure virtual class abstracts from task stack creation details
 */
class RTE_ITaskStackControl: public SAPDB_Singleton
{
public:
    /*!
      @brief singleton access function
     */
    static RTE_ITaskStackControl &Instance();

    /*!
      @brief stack control is always derived. This enum indicates which kind of stack control is selected

      There are several possible implementation variations:

      0) The thread model does not allow coroutine stacks outside the original UKT stack. This model
         is needed to support the 'Linuxthread' implementation that are using a user space manager thread
         that uses current stack pointer to identify the thread itself. Other implementation that also use
         the current stack pointer to identify the current thread would also need 'StackAddressSpace'

      1a) The kernel task stacks occupy a single contigeous section in memory which is not separated by guard pages or gaps.
         Each task stack size is system page aligned. This model is used for 'LimitedAddressSpace'
         (default model for 32Bit maschines)

      1b) The kernel task stacks occupy a single contigous section in memory which but are separated by guard pages or gaps.
         Each task stack size is system page aligned. This model is used for 'LimitedAddressSpace'

      2) The kernel task stacks occupy a single contigous section in memory which is not separated by guard pages or gaps
         but each task stack separated by the maximum task stack size of all task stacks 'LimitedMmapSections'

      3) Each kernel task stack gets its own mmap section, protected by gaps and with a fixed size spacing allowing gaps
         of unmapped pages in between (default model for 64Bit maschines) 'BigAddressSpace'.

      The fixed separation allows quiet fast calculation of task id using current task stack addresses. To allow this in model
      1a) and 1b) only two kind of task stacks are allowed: UserTaskStacks and NonUserTaskStacks. In model 2) and 3) the maximum
      task stack which is allowed for all tasks is used as spacing distance. In model 2) and 3) the task stacks are allowed
      to grow up beginning with a defined minimum up to a defined maximum, while in model 1a) and 1b) the task stack are allocated
      once and forever at start.
     */
    enum TypeOfStackControl {
        StackAddressSpace = 0   /* Task are coroutines using stack of UKT thread */
      , LimitedAddressSpace = 1 /* Task stack address space is limited (fallback to alloced chunk) */
      , LimitedMmapSections = 2 /* Task address must use single mmap section */
      , BigAddressSpace = 3     /* Task address can use reserved individual separated mmap sections */
    };

    /*!
      @brief Once called function to initialize task stack control
      @param numberOfTasks [in] number of tasks
      @param pFirst [in] pointer to first task control record
      @param pLast [in] pointer to last task control record
      @return none
     */
    virtual void Initialize( SAPDB_Long numberOfTasks, 
                             struct TASK_TYPE *pFirst,
                             struct TASK_TYPE *pLast) = 0;

    /*!
      @brief Return the system specific task stack control type
      @return specific type of task stack control created
     */
    virtual RTE_ITaskStackControl::TypeOfStackControl const GetTypeOfStackControl() const = 0;

    /*!
      @brief Using current stack context return task control block of calling task or 0 if not inside task stack
      @return Task control block pointer of calling task or 0 if not on task stack
    */
    virtual struct TASK_TYPE *GetTaskControl() const = 0;

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
                                            SAPDB_UInt4 numberOfTasksToAdd ) = 0;


}; /* endclass RTE_ITaskStackControl */

#endif  /* RTE_ITASKSTACKCONTROL_HPP */
