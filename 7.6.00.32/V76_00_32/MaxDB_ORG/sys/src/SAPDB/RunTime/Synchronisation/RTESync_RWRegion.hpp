/*!
  @file           RTESync_RWRegion.hpp
  @author         RobinW
  @ingroup        Runtime
  @brief          RWRegions

  A realization of critical regions that support exclusive ("Write") and non-exclusive ("Read") access an the usage
  of a pool of spinlocks instead of using one spinlock for each region.

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
#ifndef RTESYNC_RWREGION_HPP
#define RTESYNC_RWREGION_HPP

#include "SAPDB/SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDB/RunTime/RTE_Types.hpp"
#include "SAPDB/RunTime/Synchronisation/RTESync_NamedSpinlock.hpp"

#include "SAPDB/RunTime/Synchronisation/RTESync_SpinlockPool.hpp"

class RTETask_ITask;

/*!
   @class RTESync_IRWRegion
   @brief a critical region that can be accessed exclusively (if the data is only read) or non-exclusively (if the data is 
          modified). Non-exclusive access can be granted to several tasks in parallel, Exclusive acess is only granted
          to one task at a time.
          To reduce memory consumption, not every region has its own spinlock, but a reference to a spinlock pool
          is given to the region from which one spinlock is taken.

          This is only an interface. Instances of this class cannot be created directly. Use the factory function
          RTESync_CreateRWRegion() to create a new instance of a class with this interface.
 */
class RTESync_IRWRegion
{
public:
    /*!
      There might be 
    */
    enum flavours {WithSemalist,WithoutSemalist,WithSemalistNoStatistics};
    /*!
      @brief                 enter the region either exclusively or non-exclusively

      @param exclusive  [in] true if exclusive access is desired, false if non-exclusive access is desired
      @param pid        [in] the current task id. As there are means to calculate the current task id, it can be left out.
                             But the calculation is quite expensive, so it is a good idea to fill it in.
      @return none
     */
    virtual void enter(bool exclusive,RTE_TaskId pid=RTE_UnknownTaskId)=0;

    /*!
      @brief                 enter the region either exclusively or non-exclusively

      @param exclusive  [in] true if exclusive access is desired, false if non-exclusive access is desired
      @param pTaskObject[in] the current task object. As there are means to calculate the current task object, it can be 
                             zero. But the calculation is quite expensive, so it is a good idea to fill it in.
      @return none
     */
    virtual void enter(SAPDB_Bool exclusive,RTETask_ITask *pTaskObject)=0;

    /*!
      @brief                            enter the region either exclusively or non-exclusively

      @param exclusive          [in]    true if exclusive access is desired, false if non-exclusive access is desired
      @param hasBeenCanceled    [out]   true if the wait has been canceled. Do not enter the critical section in this case!!!
      @param pid                [in]    the current task id. As there are means to calculate the current task id, it can be left out.
                                        But the calculation is quite expensive, so it is a good idea to fill it in.
      @return none
     */
    virtual void enterInterruptible(bool exclusive,bool cancelable,SAPDB_ULong timeout,bool &hasBeenInterrupted,RTE_TaskId pid=RTE_UnknownTaskId)=0;

    /*!
      @brief                            enter the region either exclusively or non-exclusively
                                        the blocking wait may be canceled from outside!

      @param exclusive          [in]    true if exclusive access is desired, false if non-exclusive access is desired
      @param hasBeenCanceled    [out]   true if the wait has been canceled. Do not enter the critical section in this case!!!
      @param pTaskObject        [in]    the current task object. As there are means to calculate the current task object, it can be 
                                        zero. But the calculation is quite expensive, so it is a good idea to fill it in.
      @return none
     */
    virtual void enterInterruptible(bool exclusive,bool cancelable,SAPDB_ULong timeout,bool &hasBeenInterrupted,RTETask_ITask *pTaskObject)=0;

    /*!
      @brief                 try to enter the region, do not block if entering is not possible because of a collision

      @param exclusive  [in] true if exclusive access is desired, false if non-exclusive access is desired
      @param pid        [in] the current task id. As there are means to calculate the current task id, it can be left out.
                             But the calculation is quite expensive, so it is a good idea to fill it in.
      @return [bool]         true if the region could be entered, false if not.
     */
    virtual bool tryEnter(bool exclusive,RTE_TaskId pid=RTE_UnknownTaskId)=0;

    /*!
      @brief                 try to enter the region, do not block if entering is not possible because of a collision

      @param exclusive  [in] true if exclusive access is desired, false if non-exclusive access is desired
      @param pTaskObject[in] the current task object. As there are means to calculate the current task object, it can be 
                             zero. But the calculation is quite expensive, so it is a good idea to fill it in.
      @return [bool]         true if the region could be entered, false if not.
     */
    virtual bool tryEnter(bool exclusive,RTETask_ITask *pTaskObject)=0;

    /*!
      @brief                 leave a region.

      @param exclusive  [in] the exclusive value enter()/tryEnter() had been called with
      @param pid        [in] the current task id. As there are means to calculate the current task id, it can be left out.
                             But the calculation is quite expensive, so it is a good idea to fill it in.
      @return none
     */
    virtual void leave(bool exclusive,RTE_TaskId pid=RTE_UnknownTaskId)=0;

    /*!
      @brief                 leave a region.

      @param exclusive  [in] the exclusive value enter()/tryEnter() had been called with
      @param pTaskObject[in] the current task object. As there are means to calculate the current task object, it can be 
                             zero. But the calculation is quite expensive, so it is a good idea to fill it in.
      @return none
     */
    virtual void leave(bool exclusive,RTETask_ITask *pTaskObject)=0;

    /*!
      @brief returns the size of a RWRegion instance in bytes.
      @brief the size in bytes
     */
    static SAPDB_UInt4 sizeOf(flavours flavour = WithSemalist);

    /*!
      @brief                 change the access level of an already entered region from exclusive access to non-exclusive access.
                             To leave the region, call leave() with exclusive=false!

      @param exclusive  [in] the exclusive value enter()/tryEnter() had been called
      @param pid        [in] the current task id. As there are means to calculate the current task id, it can be left out.
                             But the calculation is quite expensive, so it is a good idea to fill it in.
      @return none
     */
    virtual void SwitchToNonExclusiveAccess(RTE_TaskId pid=RTE_UnknownTaskId)=0;

    /*!
      @brief                 change the access level of an already entered region from exclusive access to non-exclusive access.
                             To leave the region, call leave() with exclusive=false!

      @param exclusive  [in] the exclusive value enter()/tryEnter() had been called
      @param pTaskObject[in] the current task object. As there are means to calculate the current task object, it can be 
                             zero. But the calculation is quite expensive, so it is a good idea to fill it in.
      @return none
     */
    virtual void SwitchToNonExclusiveAccess(RTETask_ITask *pTaskObject)=0;

    /*!
      @brief get the id of the region. Usable for sanity checks.

      @param none
      @return [SAPDB_Int8] the id of the region
     */
    virtual SAPDB_Int8 getId(void)=0;

    /*!
      @brief get the id of the task that was the last to enter the region. Usable for sanity checks.

      @param none
      @return [RTE_TaskId] the id of the task.
     */
    virtual RTE_TaskId getLastEntered(void)=0;

    /*!
      @brief get collision count
      @return number of collisions on this region
     */
    virtual SAPDB_UInt4 GetCollisionCounter(void)=0;

    /*!
      @brief get exclusive access count
      @return number of exclusive lock operations with this region
     */
    virtual SAPDB_UInt4 GetExclusiveAccessCounter(void)=0;

    /*!
      @brief get non exclusive access count
      @return number of non exclusive lock operations with this region
     */
    virtual SAPDB_UInt4 GetNonExclusiveAccessCounter(void)=0;

    /*!
      @brief get wait count
      @return number of wait operations with this region
     */
    virtual SAPDB_UInt4 GetWaitCounter(void)=0;

    /*!
      @brief get test and set count
      @return number of test and set operations with this region
     */
    virtual SAPDB_UInt4 GetTasCounter(void)=0;

    /*!
      @brief get the spinlock pool the rwregion uses
     */
    virtual RTESync_SpinlockPool *GetSpinlockPool(void)=0;

    /*!
      @brief reset resetting RWRegion statistic
     */
    virtual void Reset(void)=0;
};

/*!
  @brief            factory function that returns an instance of a class accessible by the interface RTESync_IRWRegion

  @param id         [in] the id of the region
  @param pool       [in] the spinlock pool from which the spinlocks are taken
  @param allocator  [in] the allocator from which the memory for the region is taken
  @param flavour    [in] the flavour. If left out, a default is chosen. Currently there is only one really sensible flavour
                         (the WithSemalist one), so the parameter can be left out in most cases-
  @return [RTESync_IRWRegion *] pointer to a newly created RWRegion. Destroy it using RTESync_DestroyRWRegion() 
                                when it is no longer needed
*/
RTESync_IRWRegion *RTESync_CreateRWRegion(SAPDB_Int8 id,RTESync_SpinlockPool &pool, SAPDBMem_IRawAllocator &allocator,RTESync_IRWRegion::flavours flavour = RTESync_IRWRegion::WithSemalist);

/*!
  @brief            function that that destroys an instance of a class created by RTESync_CreateRWRegion()

  @param region     [in] a pointer to the region to be destroyed
  @param allocator  [in] the allocator to which the memory for the region is returned
*/
void RTESync_DestroyRWRegion(RTESync_IRWRegion *region,SAPDBMem_IRawAllocator &allocator);

/*!
   @brief locks on a RW-Region in the scope of a variable of the class. 
 */
class RTESync_RWLockedScope
{
public :
    /*!
       @brief constructor, locks share or exclusive
     */
    RTESync_RWLockedScope (RTESync_IRWRegion& RWRegion, bool ExclusiveScope, RTE_TaskId tid = RTE_UnknownTaskId)
        : m_rwRegion(RWRegion)
        , m_tid(tid)
        , m_exclusive(ExclusiveScope)
    {
        RWRegion.enter(ExclusiveScope, tid);
    }
    /*!
       @brief destructor, releases lock
     */
    ~RTESync_RWLockedScope ()
    {
       m_rwRegion.leave (m_exclusive, m_tid);
    }
private :
    RTESync_IRWRegion& m_rwRegion;
    RTE_TaskId         m_tid;
    bool               m_exclusive;
};


RTESync_IRWRegionReader *RTESync_GetRWRegionReader(RTESync_RWRegionRegister &rWRegionRegister,SAPDBMem_IRawAllocator &allocator);

#endif // RTESYNC_RWREGION_HPP
