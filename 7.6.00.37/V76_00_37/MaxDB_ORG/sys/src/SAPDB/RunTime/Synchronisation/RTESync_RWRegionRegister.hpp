/*!
  @file           RTESync_RWRegionRegister.hpp
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
#ifndef RTESYNC_RWREGIONREGISTER_HPP
#define RTESYNC_RWREGIONREGISTER_HPP


class RTESync_IRWRegion;
class RTESync_WaitQueueForRWRegion;

/*!
  @class RTESync_IRWRegionReader
  @brief Helper class to iterate through registered RWRegions
 */
class RTESync_IRWRegionReader
{

public:
    //! the mode of the items that are retrieved by the reader: the values since start or values at last reset
    enum Mode
    {
        normal, //! retrieve values since start
        reset   //! retrieve values at last reset
    };

    //! select the RWRegions the reader retrieves information about
    enum Selection
    {
        all,            //! all registered RWRegions
        usedOnesOnly    //! the used RWRegions only
    };

    /* -----------------------------------------------------------------------*/
    /*! chapter: Iteration                                                    */
    /* -----------------------------------------------------------------------*/

    /*!
      @brief reset current to first item

      Use 'Next' member function to retrieve the first item

      @return true if any items, false if empty list
     */
    virtual SAPDB_Bool First (Mode mode = normal,Selection  selection = all)=0;

    /*!
      @brief set current to next item
      @return true if more items, false if at end of list
     */
    virtual SAPDB_Bool Next (void)=0;

    /* -----------------------------------------------------------------------*/
    /*! endchapter: Iteration                                                 */
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! chapter: Data Access                                                  */
    /* -----------------------------------------------------------------------*/

    //! the name of the spinlock pool of the current item
    virtual SAPDB_Bool SpinlockPoolName         (SAPDB_UTF8 name[])=0;
    //! the address of the current item
    virtual SAPDB_Bool Address                  (SAPDB_UInt8 &address)=0;
    //! the index of the current item
    virtual SAPDB_Bool Index                    (SAPDB_UInt8 &index)=0;
    //! the collision counter of the current item
    virtual SAPDB_Bool CollisionCount           (SAPDB_UInt8 &collisioncount)=0;
    //! the wait counter of the current item
    virtual SAPDB_Bool WaitCount                (SAPDB_UInt8 &waitcount)=0;
    //! the spinlock collision counter of the current item
    virtual SAPDB_Bool SpinLockCollisionCount   (SAPDB_UInt8 &tascount)=0;
    //! the exclusive access counter of the current item
    virtual SAPDB_Bool ExclusiveAccessCount     (SAPDB_UInt8 &exclusiveaccesscount)=0;
    //! the non-exclusive acces counter of the current item
    virtual SAPDB_Bool NonExclusiveAccessCount  (SAPDB_UInt8 &nonexclusiveaccesscount)=0;
    //! the last entered task of the current item
    virtual SAPDB_Bool LastEnteredTask          (SAPDB_Int4 &taskId)=0;
    //! the current access counter of the current item
    virtual SAPDB_Bool CurrentAccesses          (SAPDB_Int4 &currentAccesses)=0;
    //! of the current item
    virtual SAPDB_Bool GetNextWaitingTask       (SAPDB_Int4 &taskId)=0;

    /* -----------------------------------------------------------------------*/
    /*! endchapter: Data Access                                               */
    /* -----------------------------------------------------------------------*/
};


/*! 
  @class RTESync_RWRegionRegister

  @brief register for RWRegions
*/
class RTESync_RWRegionRegister : public RTE_ItemRegister<RTESync_IRWRegion>
{
public:
    //! the statistic info
    struct StatisticInfo
    {
        //! the name of the spinlock pool
        SAPDB_UTF8  PoolName[40+1];             
        //! the address
        SAPDB_ULong Address;
        //! the collision counter
        SAPDB_UInt4 CollisionCount;
        //! the wait counter
        SAPDB_UInt4 WaitCount;
        //! the spinlock collision counter
        SAPDB_ULong TasCount;
        //! the exclusive access counter
        SAPDB_UInt4 ExclusiveAccessCount;
        //! the non-exclusive acces counter
        SAPDB_UInt4 NonExclusiveAccessCount;
        //! the last entered task
        RTE_TaskId  TaskId;
        //! the current access counter
        SAPDB_Int4  CurrentAccesses;
        //! the index
        SAPDB_Int8  Index;
        //! the index of the first waiting task
        SAPDB_UInt  indexOfFirstWaitingTask;
        //! the number of waiting tasks
        SAPDB_UInt  numberOfWaitingTasks;
    };

    /*!
       Function: GetStatisticInfo
       Description: Get a snapshot of all statistic informations
       Arguments: pInfoArray[out] Pointer to array of StatisticInfo

       Return value: True if info array fill, false if not not enough memory
     */
    SAPDB_Bool GetStatisticInfo(StatisticInfo * &pInfoArray, RTE_TaskId * &pWaitingTasks, SAPDB_Int4 &itemCount,RTESync_IRWRegionReader::Mode mode,RTESync_IRWRegionReader::Selection  selection);

    /*!
       Function: FreeStatisticInfo
       Description: Free the snapshot of all statistic informations collected by GetStatisticInfo

       Arguments: pInfoArray[in] Pointer to array of StatisticInfo
     */
    void FreeStatisticInfo(StatisticInfo *pInfoArray,RTE_TaskId *pWaitingTasks);

    /*!
        @brief return the number of registered items
    */
    SAPDB_UInt4 GetCount(void)
    {
        return m_Count;
    }

    /*!
      @brief constructor
    */
    RTESync_RWRegionRegister(SAPDB_Bool makeBackupCopy)
        : RTE_ItemRegister<RTESync_IRWRegion>(makeBackupCopy)
    {}

    /*!
      @brief perform a reset: for all registered elements, set the values "at reset" to the current values
    */
    void ResetAllRegisteredElements(void);

    /*!
       Function: FillStatisticInfo
       Description: Fill the statistic informations
       
       Called by GetStatisticInfo, to call at least memory allocation outside of locked registration code

       Arguments: pInfoArray[in] Pointer to array of StatisticInfo
     */
private:
    SAPDB_Bool FillStatisticInfo(StatisticInfo * &pInfoArray, RTE_TaskId * pWaitingTasks, SAPDB_Int4 maxCount, SAPDB_Int4 &itemCount,RTESync_IRWRegionReader::Mode mode,RTESync_IRWRegionReader::Selection  selection);
};
#endif // RTESYNC_RWREGIONREGISTER_HPP
