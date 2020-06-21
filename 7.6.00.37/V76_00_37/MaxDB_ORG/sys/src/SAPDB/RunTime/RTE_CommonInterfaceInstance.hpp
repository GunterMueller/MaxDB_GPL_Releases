/*!
  @file    RTE_CommonInterfaceInstance.hpp
  @author  JoergM
  @ingroup Runtime
  @brief   Session-free kernel runtime interface implementation for use in liboms.

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

#ifndef RTE_COMMONINTERFACEINSTANCE_HPP
#define RTE_COMMONINTERFACEINSTANCE_HPP

#include "RunTime/RTE_IInterface.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
  @class  RTE_CommonInterfaceInstance
  @brief The exported common runtime interface
  
  This class contains all functions implementable identical for client and kernel
 */
class RTE_CommonInterfaceInstance : public RTE_IInterface
{
public:
    /*!
        @brief Register a message list into global array
        @return pointer to slot which must be used to deregister
     */
    virtual Msg_RegistrySlot const *MsgListRegister(Msg_List const &msg);

    /*!
        @brief Deregister a message list from global array
        @param pSlot[in] pointer to slot where message is registered
        @param msg[in] message reference that is to be deregistered
        @return none
     */
    virtual void MsgListDeregister(Msg_RegistrySlot const *&pSlot);
    
    /*!
      @brief get the allocator to be used for all msg list allocation internally
      @return reference to message list emergency allocator
     */
    virtual SAPDBMem_IRawAllocator &MsgListAllocator();

    /*!
    @brief   Returns the limitation of messages contained in a single list
    @param   none
    @return  Current limitation of contained messages ( 0 = unlimited )
    */
    virtual SAPDB_UInt4 GetMessageLimit();

    /*!
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
    virtual SAPDB_UInt4 SetMessageLimit(SAPDB_UInt4 messageLimit);

    /*!
      @brief get the RTE allocator to be used for all runtime internal allocations
      @return reference to runtime owned allocator
     */
    virtual SAPDBMem_IRawAllocator &Allocator();

    /*!
      @brief get current process id
      @return process id
     */
    virtual SAPDB_UInt4 ProcessID();
    /*!
      @brief get optimal spinlock loop count
      @return number of hot spinlock loops
     */
    virtual SAPDB_UInt4 GetLockLoopCount();

    /*!
      @brief set the task state to yield on spinlock, if not already set

      A task can indicate by this interface, that it is yielding to acquiere the given spinlock.
      This call is part of the interface, to allow liboms to indicate its spinlock yield entry.

      @param pLock [in] pointer to spinlock 
      @param beginning [in] flag to indicate yield begins or ends
      @return none
     */
    virtual void SetTaskStateToYieldOnSpinlock(RTESync_Spinlock const *pLock, bool beginning);

    /*!
      @brief fill protocol time stamp with current local time
      @param timeBuffer [out] buffer to be filled with local time stamp
      @return pointer to filled buffer
     */
    virtual SAPDB_Char *FillProtocolTimeStamp(Timestamp & timeBuffer);

    /*!
      @brief fill protocol time stamp with current GMT
      @param timeBuffer [out] buffer to be filled with GMT stamp
      @return pointer to filled buffer
     */
    virtual SAPDB_Char *FillProtocolGMTTimeStamp(Timestamp & timeBuffer);

    /*!
      @brief get local date time
      @param now [out] current time
      @return none
     */
    virtual void GetLocalDateTime(RTE_ISystem::DateTime &now);

    /*!
      @brief protect given memory area
      @param pwAdr [in] start address (must be system page aligned)
      @param ulSize [in] number of bytes to protect
      @param type [in] protection type MEMORYPROTECTION_READWRITE,MEMORYPROTECTION_READ,MEMORYPROTECTION_NOACCESS
      @return 0 if call succeeded, system specific error code else
     */
    virtual SAPDB_ULong MemProtect( void *pvAdr, SAPDB_ULong ulSize, SAPDB_Int type );

    /*!
      @brief give up time slice

      This call allows to dispatch next thread/process. Used if block i.e. by a spinlock.
     */
    virtual void GiveUpTimeSlice();

    /*!
      @brief yield while potentially inside task code

      NOTE: This call produces extra overhead to detect if runnning inside a task or runtime thread.
      If you already know you are running in a task, you can specify a task id. If you know you are not
      running in a task, better call GiveUpTimeSlice() directly !

      This call allows to dispatch next task/thread/process. Used if block i.e. by a spinlock
      param taskId [in] taskid currently running in ( optional, but performance relevant...)
     */
    virtual void TaskYield(RTE_TaskId taskId=0, SAPDB_Bool allowLoadBalancing = false );

    /*!
      @brief atomic read for 8 byte value
      
      This call allows to atomically read an 8 byte value (important for 32bit platforms...)
      @param memoryPosition [in] reference to memory position 
      @return read value
     */
    virtual SAPDB_UInt8 AtomicRead(SAPDB_UInt8 & memoryPosition);

    /*!
      @brief atomic write for 8 byte value
      
      This call allows to atomically write an 8 byte value (important for 32bit platforms...)
      @param memoryPosition [in] reference to memory position 
      @param value [in] value to write
      @return none
     */
    virtual void        AtomicWrite(SAPDB_Int8 & memoryPosition, SAPDB_Int8 const newValue );

    /*!
      @brief atomic mopdify a value
      
      This call allows to atomically modify a value at given position

      @param memoryPosition [in] reference to memory position 
      @param deltaValue [in] the delta value to write
      @return the result value
     */
    virtual SAPDB_Int4  AtomicModify( SAPDB_Int4 &     memoryPosition,
                                     SAPDB_Int4 const deltaValue );
    /*!
      @brief atomic mopdify a value
      
      This call allows to atomically modify a value at given position

      @param memoryPosition [in] reference to memory position 
      @param deltaValue [in] the delta value to write
      @return the result value
     */
    virtual SAPDB_UInt4 AtomicModify( SAPDB_UInt4 &     memoryPosition,
                                     SAPDB_Int4 const deltaValue );
    /*!
      @brief atomic mopdify a value
      
      This call allows to atomically modify a value at given position

      @param memoryPosition [in] reference to memory position 
      @param deltaValue [in] the delta value to write
      @return the result value
     */
    virtual SAPDB_Int8  AtomicModify( SAPDB_Int8 &     memoryPosition,
                                     SAPDB_Int8 const deltaValue );
    /*!
      @brief atomic mopdify a value
      
      This call allows to atomically modify a value at given position

      @param memoryPosition [in] reference to memory position 
      @param deltaValue [in] the delta value to write
      @return the result value
     */
    virtual SAPDB_UInt8 AtomicModify( SAPDB_UInt8 &     memoryPosition,
                                     SAPDB_Int8 const deltaValue );


    /*!
      @brief atomic compare and exchage a value
      
      This call allows to atomically compare and exchange a value.
      This is simulated on platforms that do not provide such a call.
      
      @param memoryPosition [in] reference to memory position 
      @param expectedValue [in] the expected value
      @param newValue [in] the new value to set
      @param oldValue [out] the value found
      @return true if expected value was found, false if oldValue differs from expected value
     */
    virtual SAPDB_Bool  AtomicCompareAndExchange( SAPDB_Int4 &     memoryPosition,
                                                  SAPDB_Int4 const expectedValue,
                                                  SAPDB_Int4 const newValue,
                                                  SAPDB_Int4 &     oldValue );
    /*!
      @brief atomic compare and exchage a value
      
      This call allows to atomically compare and exchange a value.
      This is simulated on platforms that do not provide such a call.
      
      @param memoryPosition [in] reference to memory position 
      @param expectedValue [in] the expected value
      @param newValue [in] the new value to set
      @param oldValue [out] the value found
      @return true if expected value was found, false if oldValue differs from expected value
     */
    virtual SAPDB_Bool  AtomicCompareAndExchange( SAPDB_UInt4 &     memoryPosition,
                                                  SAPDB_UInt4 const expectedValue,
                                                  SAPDB_UInt4 const newValue,
                                                  SAPDB_UInt4 &     oldValue );
    /*!
      @brief atomic compare and exchage a value
      
      This call allows to atomically compare and exchange a value.
      This is simulated on platforms that do not provide such a call.
      
      @param memoryPosition [in] reference to memory position 
      @param expectedValue [in] the expected value
      @param newValue [in] the new value to set
      @param oldValue [out] the value found
      @return true if expected value was found, false if oldValue differs from expected value
     */
    virtual SAPDB_Bool  AtomicCompareAndExchange( SAPDB_Int8 &     memoryPosition,
                                                  SAPDB_Int8 const expectedValue,
                                                  SAPDB_Int8 const newValue,
                                                  SAPDB_Int8 &     oldValue );
    /*!
      @brief atomic compare and exchage a value
      
      This call allows to atomically compare and exchange a value.
      This is simulated on platforms that do not provide such a call.
      
      @param memoryPosition [in] reference to memory position 
      @param expectedValue [in] the expected value
      @param newValue [in] the new value to set
      @param oldValue [out] the value found
      @return true if expected value was found, false if oldValue differs from expected value
     */
    virtual SAPDB_Bool  AtomicCompareAndExchange( SAPDB_UInt8 &     memoryPosition,
                                                  SAPDB_UInt8 const expectedValue,
                                                  SAPDB_UInt8 const newValue,
                                                  SAPDB_UInt8 &     oldValue );
    /*!
      @brief atomic compare and exchage a value
      
      This call allows to atomically compare and exchange a value.
      This is simulated on platforms that do not provide such a call.
      
      @param pPointerPosition [in] reference to memory position 
      @param expectedValue [in] the expected value
      @param newValue [in] the new value to set
      @param pOldValue [out] the value found
      @return true if expected value was found, false if oldValue differs from expected value
     */
    virtual SAPDB_Bool  AtomicCompareAndExchange( void * *     pPointerPosition,
                                                  void * const expectedValue,
                                                  void * const newValue,
                                                  void * *     pOldValue );
    /*!
      @brief atomic compare and exchage a value
      
      This call allows to atomically compare and exchange a value.
      This is simulated on platforms that do not provide such a call.
      
      @param pPointerPosition [in] reference to memory position 
      @param expectedValue [in] the expected value
      @param newValue [in] the new value to set
      @param pOldValue [out] the value found
      @return true if expected value was found, false if oldValue differs from expected value
     */
    virtual SAPDB_Bool  AtomicCompareAndExchange( void const * *     pPointerPosition,
                                                  void const * const expectedValue,
                                                  void const * const newValue,
                                                  void const * *     pOldValue );

    /*!
      @brief set a read memory barrier
     */
    virtual void ReadMemoryBarrier();
    /*!
      @brief set a write memory barrier
     */
    virtual void WriteMemoryBarrier();
    /*!
      @brief set a memory barrier
     */
    virtual void MemoryBarrier();

    /*!
      @brief get a procedure address from given shared library handle
     */
    virtual RTE_SharedLibProcAddress GetProcAddress( RTE_SharedLibHandle hLib,
                                                     SAPDB_Char const *szSqlFunction, 
                                                     SAPDB_Char *pErrText,
                                                     SAPDB_UInt4 lenErrTextInBytes) = 0;

    /*!
      @brief write a diagnostic message into knldiag
      @param msg [in] the message list to write
      @return none
     */
    virtual void DiagMessage(Msg_List const &msg);

    /*!
      @brief force an crash in emergency situation
      @param msg [in] the message list containing the famous last words...
      @return none and will never return anyhow...
     */
    virtual void ForcedCrash(Msg_List const &msg);
};

/*!
  endclass:       RTE_KernelInterfaceInstance
*/

#endif  /* RTE_COMMONINTERFACEINSTANCE_HPP */
