/*!
  @file    RTE_CommonInterfaceInstance.cpp
  @author  JoergM
  @ingroup Runtime
  @brief   Session-free kernel runtime interface for use in liboms.

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

#include "RunTime/RTE_CommonInterfaceInstance.hpp"
#include "RunTime/System/RTESys_ProcessID.hpp"
#include "RunTime/System/RTESys_Spinlock.h"
#include "RunTime/System/RTESys_Time.h"
#include "RunTime/System/RTESys_AtomicOperation.hpp"
#include "RunTime/System/RTESys_MemoryBarrier.h"
#include "RunTime/MemoryManagement/RTEMem_RteAllocator.hpp"
#include "Messages/Msg_Registry.hpp"
#include "RunTime/RTE_Message.hpp"
#include "RunTime/RTE_Crash.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

    /*
        @brief Register a message list into global array
        @return pointer to slot which must be used to deregister
     */
    Msg_RegistrySlot const *RTE_CommonInterfaceInstance::MsgListRegister(Msg_List const &msg)
    { return Msg_Registry::Instance().Register(msg); }

    /*
        @brief Deregister a message list from global array
        @param pSlot[in] pointer to slot where message is registered
        @param msg[in] message reference that is to be deregistered
        @return none
     */
    void RTE_CommonInterfaceInstance::MsgListDeregister(Msg_RegistrySlot const *&pSlot)
    { Msg_Registry::Instance().Deregister(pSlot); }
    
    /*
      @brief get the allocator to be used for all msg list allocation internally
      @return reference to message list emergency allocator
     */
    SAPDBMem_IRawAllocator &RTE_CommonInterfaceInstance::MsgListAllocator()
    { return Msg_Registry::Instance().Allocator(); }

    /*
    @brief   Returns the limitation of messages contained in a single list
    @param   none
    @return  Current limitation of contained messages ( 0 = unlimited )
    */
    SAPDB_UInt4 RTE_CommonInterfaceInstance::GetMessageLimit()
    { return Msg_Registry::Instance().GetMessageLimit(); }

    /*
    @brief   Sets the limitation of messages contained in a single list

    Limitation means the if number of messages that are contained in a message exceed
    the given number, only the topmost message can still be overloaded and all other
    message added only lead to an extra 'ignoring message' that shows the
    limitation and the number of ignored messages. Adding details is not updating main
    message, but only updates the 'ignoring message'.

    NOTE: This function is global to all Msg_Lists created. If called more than ones, the
    Msg_List always takes the maximum of the Msg_List contained messages and the actual 
    message limit value (regarding the top most and the 'ignoring message')

    The first time the message limit is exceeded due to an overload, the message
    to overload with replaces the first message and the second message is replaced by an
    SDBMSG_MESSAGES_IGNORED_MESSAGES message. See UpdateIgnoringMessage() for details.

    @param   messageLimit [in] new limitation of contained messages ( 0 = unlimited )
    @return  Former limitation of contained messages ( 0 = unlimited )
    */
    SAPDB_UInt4 RTE_CommonInterfaceInstance::SetMessageLimit(SAPDB_UInt4 messageLimit)
    { return Msg_Registry::Instance().SetMessageLimit(messageLimit); }

    /*
      @brief get the RTE allocator to be used for all runtime internal allocations
      @return reference to runtime owned allocator
     */
    SAPDBMem_IRawAllocator &RTE_CommonInterfaceInstance::Allocator()
    { return RTEMem_RteAllocator::Instance(); }

    /*
      @brief get current process id
      @return process id
     */
    SAPDB_UInt4 RTE_CommonInterfaceInstance::ProcessID()
    { return (SAPDB_UInt4)RTESys_ProcessID(); }
    /*
      @brief get optimal spinlock loop count
      @return number of hot spinlock loops
     */
    SAPDB_UInt4 RTE_CommonInterfaceInstance::GetLockLoopCount()
    { return RTESys_GetLockLoopCount(); }

    /*
      @brief set the task state to yield on spinlock, if not already set

      A task can indicate by this interface, that it is yielding to acquiere the given spinlock.
      This call is part of the interface, to allow liboms to indicate its spinlock yield entry.

      @param pLock [in] pointer to spinlock 
      @param beginning [in] flag to indicate yield begins or ends
      @return none
     */
    void RTE_CommonInterfaceInstance::SetTaskStateToYieldOnSpinlock(RTESync_Spinlock const *pLock, bool beginning)
    { return; }

     /*
      @brief fill protocol time stamp with current local time
      @param timeBuffer [out] buffer to be filled with local time stamp
      @return pointer to filled buffer
     */
    SAPDB_Char *RTE_CommonInterfaceInstance::FillProtocolTimeStamp(Timestamp & timeBuffer)
    {   return RTESys_FillProtocolTimeStamp(timeBuffer);    }

    /*
      @brief fill protocol time stamp with current GMT
      @param timeBuffer [out] buffer to be filled with GMT stamp
      @return pointer to filled buffer
     */
    SAPDB_Char *RTE_CommonInterfaceInstance::FillProtocolGMTTimeStamp(Timestamp & timeBuffer)
    { return RTESys_FillProtocolGMTTimeStamp(timeBuffer); }
    /*
      @brief get local date time
      @param now [out] current time
      @return none
     */
    void RTE_CommonInterfaceInstance::GetLocalDateTime(RTE_ISystem::DateTime &now)
    {   RTE_ISystem::GetLocalDateTime(now); }

    /*
      @brief protect given memory area
      @param pwAdr [in] start address (must be system page aligned)
      @param ulSize [in] number of bytes to protect
      @param type [in] protection type MEMORYPROTECTION_READWRITE,MEMORYPROTECTION_READ,MEMORYPROTECTION_NOACCESS
      @return 0 if call succeeded, system specific error code else
     */
    SAPDB_ULong RTE_CommonInterfaceInstance::MemProtect( void *pvAdr, SAPDB_ULong ulSize, SAPDB_Int type)
    { return RTESys_MemProtect(pvAdr, ulSize, type); }

    /*
      @brief give up time slice

      This call allows to dispatch next thread/process. Used if block i.e. by a spinlock.
     */
    void RTE_CommonInterfaceInstance::GiveUpTimeSlice()
    { RTESys_GiveUpTimeSlice(); }

    /*
      @brief yield while potentially inside task code

      NOTE: This call produces extra overhead to detect if runnning inside a task or runtime thread.
      If you already know you are running in a task, you can specify a task id. If you know you are not
      running in a task, better call GiveUpTimeSlice() directly !

      This call allows to dispatch next task/thread/process. Used if block i.e. by a spinlock
      param taskId [in] taskid currently running in ( optional, but performance relevant...)
     */
    void RTE_CommonInterfaceInstance::TaskYield(RTE_TaskId, SAPDB_Bool)
    { RTESys_GiveUpTimeSlice(); }

    /*
      @brief atomic read for 8 byte value
      
      This call allows to atomically read an 8 byte value (important for 32bit platforms...)
      @param memoryPosition [in] reference to memory position 
      @return read value
     */
    SAPDB_UInt8 RTE_CommonInterfaceInstance::AtomicRead(SAPDB_UInt8 & memoryPosition)
    { return RTESys_AtomicRead(memoryPosition); }

    /*
      @brief atomic write for 8 byte value
      
      This call allows to atomically write an 8 byte value (important for 32bit platforms...)
      @param memoryPosition [in] reference to memory position 
      @param value [in] value to write
      @return none
     */
    void        RTE_CommonInterfaceInstance::AtomicWrite(SAPDB_Int8 & memoryPosition, SAPDB_Int8 const newValue )
    { RTESys_AtomicWrite( memoryPosition, newValue ); }

    /*
      @brief atomic mopdify a value
      
      This call allows to atomically modify a value at given position

      @param memoryPosition [in] reference to memory position 
      @param deltaValue [in] the delta value to write
      @return the result value
     */
    SAPDB_Int4  RTE_CommonInterfaceInstance::AtomicModify( SAPDB_Int4 &     memoryPosition,
                                     SAPDB_Int4 const deltaValue )
    { return RTESys_AtomicModify( memoryPosition, deltaValue ); }
    /*
      @brief atomic mopdify a value
      
      This call allows to atomically modify a value at given position

      @param memoryPosition [in] reference to memory position 
      @param deltaValue [in] the delta value to write
      @return the result value
     */
    SAPDB_UInt4 RTE_CommonInterfaceInstance::AtomicModify( SAPDB_UInt4 &     memoryPosition,
                                     SAPDB_Int4 const deltaValue )
    { return RTESys_AtomicModify( memoryPosition, deltaValue ); }
    /*
      @brief atomic mopdify a value
      
      This call allows to atomically modify a value at given position

      @param memoryPosition [in] reference to memory position 
      @param deltaValue [in] the delta value to write
      @return the result value
     */
    SAPDB_Int8  RTE_CommonInterfaceInstance::AtomicModify( SAPDB_Int8 &     memoryPosition,
                                     SAPDB_Int8 const deltaValue )
    { return RTESys_AtomicModify( memoryPosition, deltaValue ); }
    /*
      @brief atomic mopdify a value
      
      This call allows to atomically modify a value at given position

      @param memoryPosition [in] reference to memory position 
      @param deltaValue [in] the delta value to write
      @return the result value
     */
    SAPDB_UInt8 RTE_CommonInterfaceInstance::AtomicModify( SAPDB_UInt8 &     memoryPosition,
                                     SAPDB_Int8 const deltaValue )
    { return RTESys_AtomicModify( memoryPosition, deltaValue ); }


    /*
      @brief atomic compare and exchage a value
      
      This call allows to atomically compare and exchange a value.
      This is simulated on platforms that do not provide such a call.
      
      @param memoryPosition [in] reference to memory position 
      @param expectedValue [in] the expected value
      @param newValue [in] the new value to set
      @param oldValue [out] the value found
      @return true if expected value was found, false if oldValue differs from expected value
     */
    SAPDB_Bool  RTE_CommonInterfaceInstance::AtomicCompareAndExchange( SAPDB_Int4 &     memoryPosition,
                                                  SAPDB_Int4 const expectedValue,
                                                  SAPDB_Int4 const newValue,
                                                  SAPDB_Int4 &     oldValue )
    { return RTESys_CompareAndExchange( memoryPosition, expectedValue, newValue, oldValue ); }
    /*
      @brief atomic compare and exchage a value
      
      This call allows to atomically compare and exchange a value.
      This is simulated on platforms that do not provide such a call.
      
      @param memoryPosition [in] reference to memory position 
      @param expectedValue [in] the expected value
      @param newValue [in] the new value to set
      @param oldValue [out] the value found
      @return true if expected value was found, false if oldValue differs from expected value
     */
    SAPDB_Bool  RTE_CommonInterfaceInstance::AtomicCompareAndExchange( SAPDB_UInt4 &     memoryPosition,
                                                  SAPDB_UInt4 const expectedValue,
                                                  SAPDB_UInt4 const newValue,
                                                  SAPDB_UInt4 &     oldValue )
    { return RTESys_CompareAndExchange( memoryPosition, expectedValue, newValue, oldValue ); }
    /*
      @brief atomic compare and exchage a value
      
      This call allows to atomically compare and exchange a value.
      This is simulated on platforms that do not provide such a call.
      
      @param memoryPosition [in] reference to memory position 
      @param expectedValue [in] the expected value
      @param newValue [in] the new value to set
      @param oldValue [out] the value found
      @return true if expected value was found, false if oldValue differs from expected value
     */
    SAPDB_Bool  RTE_CommonInterfaceInstance::AtomicCompareAndExchange( SAPDB_Int8 &     memoryPosition,
                                                  SAPDB_Int8 const expectedValue,
                                                  SAPDB_Int8 const newValue,
                                                  SAPDB_Int8 &     oldValue )
    { return RTESys_CompareAndExchange( memoryPosition, expectedValue, newValue, oldValue ); }
    /*
      @brief atomic compare and exchage a value
      
      This call allows to atomically compare and exchange a value.
      This is simulated on platforms that do not provide such a call.
      
      @param memoryPosition [in] reference to memory position 
      @param expectedValue [in] the expected value
      @param newValue [in] the new value to set
      @param oldValue [out] the value found
      @return true if expected value was found, false if oldValue differs from expected value
     */
    SAPDB_Bool  RTE_CommonInterfaceInstance::AtomicCompareAndExchange( SAPDB_UInt8 &     memoryPosition,
                                                  SAPDB_UInt8 const expectedValue,
                                                  SAPDB_UInt8 const newValue,
                                                  SAPDB_UInt8 &     oldValue )
    { return RTESys_CompareAndExchange( memoryPosition, expectedValue, newValue, oldValue ); }
    /*
      @brief atomic compare and exchage a value
      
      This call allows to atomically compare and exchange a value.
      This is simulated on platforms that do not provide such a call.
      
      @param pPointerPosition [in] reference to memory position 
      @param expectedValue [in] the expected value
      @param newValue [in] the new value to set
      @param pOldValue [out] the value found
      @return true if expected value was found, false if oldValue differs from expected value
     */
    SAPDB_Bool  RTE_CommonInterfaceInstance::AtomicCompareAndExchange( void * *     pPointerPosition,
                                                  void * const expectedValue,
                                                  void * const newValue,
                                                  void * *     pOldValue )
    { return RTESys_CompareAndExchange( *pPointerPosition, expectedValue, newValue, *pOldValue ); }
    /*
      @brief atomic compare and exchage a value
      
      This call allows to atomically compare and exchange a value.
      This is simulated on platforms that do not provide such a call.
      
      @param pPointerPosition [in] reference to memory position 
      @param expectedValue [in] the expected value
      @param newValue [in] the new value to set
      @param pOldValue [out] the value found
      @return true if expected value was found, false if oldValue differs from expected value
     */
    SAPDB_Bool  RTE_CommonInterfaceInstance::AtomicCompareAndExchange( void const * * pPointerPosition,
                                                  void const * const expectedValue,
                                                  void const * const newValue,
                                                  void const * *     pOldValue )
    {
        void **voidPosition = (void **)pPointerPosition;
        void *oldValueFound;
        if ( RTESys_CmpxchgPointer(*voidPosition, (void *)expectedValue, (void *)newValue, oldValueFound) )
        {
            *pOldValue = (void const *)oldValueFound; 
            return true;
        }
        return false;
    }

    /*
      @brief set a read memory barrier
     */
    void RTE_CommonInterfaceInstance::ReadMemoryBarrier()
    { RTESys_ReadMemoryBarrier(); }
    /*
      @brief set a write memory barrier
     */
    void RTE_CommonInterfaceInstance::WriteMemoryBarrier()
    { RTESys_WriteMemoryBarrier(); }
    /*
      @brief set a memory barrier
     */
    void RTE_CommonInterfaceInstance::MemoryBarrier()
    { RTESys_MemoryBarrier(); }

    /*
      @brief write a diagnostic message into knldiag
      @param msg [in] the message list to write
      @return none
     */
    void RTE_CommonInterfaceInstance::DiagMessage(Msg_List const &msg)
    { RTE_Message(msg); }

    void RTE_CommonInterfaceInstance::ForcedCrash(Msg_List const &msg)
    { RTE_Crash(msg); }
