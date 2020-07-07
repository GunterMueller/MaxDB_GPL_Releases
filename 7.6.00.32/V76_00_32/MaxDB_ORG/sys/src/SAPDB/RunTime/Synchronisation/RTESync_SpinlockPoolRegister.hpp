/*!
  @file           RTESync_SpinlockPoolRegister.hpp
  @author         RobinW
  @ingroup        Runtime
  @brief          Register for Spinlock Pools


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
#ifndef RTESYNC_SPINLOCKPOOLREGISTER_HPP
#define RTESYNC_SPINLOCKPOOLREGISTER_HPP

#include "SAPDB/SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDB/RunTime/RTE_Types.hpp"
#include "SAPDB/RunTime/Synchronisation/RTESync_NamedSpinlock.hpp"

class RTESync_SpinlockPool;
class RTESync_BaseSpinlockPool;
class RTESync_RWRegionRegister;

/*! 
  @class RTESync_SpinlockPoolRegister

  @brief The spinlock pool register singleton
*/
class RTESync_SpinlockPoolRegister : public RTE_ItemRegister<RTESync_SpinlockPool>
{
public:
    /*!
      enumeration for selection of synchronized / unsynchronized access
     */
    enum AccessSynchronization
    {
        synchronized,   ///< statistic record filled synchronized
        unsynchronized  ///< statistic record filled unsynchronized
    };
    static const SAPDB_Int4 m_MaxIdentfierLength; ///< maximum identifier length

    //! the statistics info
    struct StatisticInfo
    {
        //! name of the spinlock pool
        SAPDB_UTF8  PoolName[40+4];
        //! number of spinlocks in the pool
        SAPDB_Int4  NumberOfSpinlocks;
        //! pointer to the RWRegions registered for this spinlock pool
        RTESync_RWRegionRegister *pRWRegionRegister;

        //! the maximum values 
        RTE_SpinlockStatistic Maximum;
        //! the minimum values 
        RTE_SpinlockStatistic Minimum;
        //! the average values 
        RTE_SpinlockStatistic Average;
    };

    /*!
        @brief Return reference to single instance of RTESync_SpinlockRegister

        The InitializeInstance function must have been called before.

        @return Reference to singleton instance
     */
    static RTESync_SpinlockPoolRegister & Instance( );
    /*!
       @brief Get a snapshot of all statistic informations
       @param pInfoArray [out] Pointer to array of StatisticInfo
       @param itemCount [out] number of items returned
       @param mode [in] select Normal or Resetable items
  
       @return True if info array fill, false if not not enough memory
     */
    SAPDB_Bool GetStatisticInfo(StatisticInfo * &pInfoArray, SAPDB_Int4 &itemCount, RTESync_Spinlock::Mode mode, AccessSynchronization access = synchronized );

    /*!
       @brief Free the snapshot of all statistic informations collected by GetStatisticInfo

       @param pInfoArray [in] Pointer to array of StatisticInfo
     */
    void FreeStatisticInfo(StatisticInfo *pInfoArray);

#ifdef AIX
    /*!
      @brief call
     */
    static void Reset(void)
    {
        m_Instance = NULL;
    }
#endif

    /*!
      @brief perform a reset: for all registered elements, set the values "at reset" to the current values
    */
    void ResetAllRegisteredElements(void);

    /*!
        @brief return the number of registered items
    */
    SAPDB_UInt4 GetCount(void)
    {
        return m_Count;
    }

private:
    RTESync_SpinlockPoolRegister(SAPDB_Bool makeBackupCopy):RTE_ItemRegister<RTESync_SpinlockPool>(makeBackupCopy) {}

    /*!
       @brief Fill the statistic informations
       
       Called by GetStatisticInfo, to call at least memory allocation outside of locked registration code

       @param pInfoArray [in] Pointer to array of StatisticInfo
       @param maxCount [out] maximum number of items returned
       @param itemCount [out] number of items returned
       @param mode [in] select Normal or Resetable items
     */
    SAPDB_Bool FillStatisticInfo(StatisticInfo * &pInfoArray, SAPDB_Int4 maxCount, SAPDB_Int4 &itemCount, RTESync_Spinlock::Mode mode, AccessSynchronization access = synchronized );

    static RTESync_SpinlockPoolRegister *m_Instance;
};

/*!
  @class RTESync_SpinlockPoolReader
  @brief Helper class to iterate through registered spinlock pools
 */
class RTESync_SpinlockPoolReader
{

public:

    /* -----------------------------------------------------------------------*/
    /*! chapter: Constructors and initialization                              */
    /* -----------------------------------------------------------------------*/

    /*!
      @brief ctor
     */
    RTESync_SpinlockPoolReader (void);
    
    /*!
      @brief dtor
     */
    ~RTESync_SpinlockPoolReader (void);

    /* -----------------------------------------------------------------------*/
    /*! endchapter: Constructors and initialization                           */
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! chapter: Iteration                                                    */
    /* -----------------------------------------------------------------------*/

    /*!
      @brief reset current to first item

      Use 'Next' member function to retrieve the first item

      @param mode [in] select Normal or Resetable items
      @return true if any items, false if empty list
     */
    SAPDB_Bool First (
        RTESync_Spinlock::Mode mode = RTESync_Spinlock::normal, 
        RTESync_SpinlockPoolRegister::AccessSynchronization access = RTESync_SpinlockPoolRegister::synchronized );

    /*!
      @brief set current to next item
      @return true if more items, false if at end of list
     */
    SAPDB_Bool Next (void);

    /* -----------------------------------------------------------------------*/
    /*! endchapter: Iteration                                                 */
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! chapter: Data Access                                                  */
    /* -----------------------------------------------------------------------*/

    /*!
      @brief return name of spinlock
      @param name [in] buffer for name
      @return true if name returned, false if no spinlocks to iterate
     */
    SAPDB_Bool Name (SAPDB_UTF8 name[]);                                        ///< name of the current item

    /*!
      @brief call
      @param elements [out]
      @return value
     */
    SAPDB_Bool NumberOfElements (SAPDB_Int4 &elements);                         ///< number of elements of the current item

    /*!
      @brief call
      @param maxLocks [out]
      @return value
     */
    SAPDB_Bool MaxLocks (SAPDB_UInt8 &maxLocks);                                ///< maximum of the lock counts within the current item

    /*!
      @brief call
      @param minLocks [out]
      @return value
     */
    SAPDB_Bool MinLocks (SAPDB_UInt8 &minLocks);                                ///< minimum of the lock counts within the current item

    /*!
      @brief call
      @param avgLocks [out]
      @return value
     */
    SAPDB_Bool AvgLocks (SAPDB_UInt8 &avgLocks);                                ///< average of the lock counts withinthe current item

    /*!
      @brief call
      @param maxCollisions [out]
      @return value
     */
    SAPDB_Bool MaxCollisions (SAPDB_UInt8 &maxCollisions);                      ///< maximum of the collision counts within the current item

    /*!
      @brief call
      @param minCollisions [out]
      @return value
     */
    SAPDB_Bool MinCollisions (SAPDB_UInt8 &minCollisions);                      ///< minimum of the collision counts within the current item

    /*!
      @brief call
      @param avgCollisions [out]
      @return value
     */
    SAPDB_Bool AvgCollisions (SAPDB_UInt8 &avgCollisions);                      ///< average of the collision counts within the current item

    /*!
      @brief call
      @param maxTotalSpinLoops [out]
      @return value
     */
    SAPDB_Bool MaxTotalSpinLoops (SAPDB_UInt8 &maxTotalSpinLoops);              ///< maximum of the total spin loops within the current item

    /*!
      @brief call
      @param minTotalSpinLoops [out]
      @return value
     */
    SAPDB_Bool MinTotalSpinLoops (SAPDB_UInt8 &minTotalSpinLoops);              ///< minimum of the total spin loops within the current item

    /*!
      @brief call
      @param avgTotalSpinLoops [out]
      @return value
     */
    SAPDB_Bool AvgTotalSpinLoops (SAPDB_UInt8 &avgTotalSpinLoops);              ///< average of the total spin loops within the current item

    /*!
      @brief call
      @param maxTotalYieldLoops [out]
      @return value
     */
    SAPDB_Bool MaxTotalYieldLoops (SAPDB_UInt8 &maxTotalYieldLoops);            ///< maximum of the total yield loops within the current item

    /*!
      @brief call
      @param minTotalYieldLoops [out]
      @return value
     */
    SAPDB_Bool MinTotalYieldLoops (SAPDB_UInt8 &minTotalYieldLoops);            ///< minimum of the total yield loops within the current item

    /*!
      @brief call
      @param avgTotalYieldLoops [out]
      @return value
     */
    SAPDB_Bool AvgTotalYieldLoops (SAPDB_UInt8 &avgTotalYieldLoops);            ///< average of the total yield loops within the current item

    /*!
      @brief call
      @param maxMaxSpinLoops [out]
      @return value
     */
    SAPDB_Bool MaxMaxSpinLoops (SAPDB_UInt8 &maxMaxSpinLoops);                  ///< maximum of the maximum spin loops within the current item

    /*!
      @brief call
      @param minMaxSpinLoops [out]
      @return value
     */
    SAPDB_Bool MinMaxSpinLoops (SAPDB_UInt8 &minMaxSpinLoops);                  ///< minimum of the maximum spin loops within the current item

    /*!
      @brief call
      @param avgMaxSpinLoops [out]
      @return value
     */
    SAPDB_Bool AvgMaxSpinLoops (SAPDB_UInt8 &avgMaxSpinLoops);                  ///< average of the maximum spin loops within the current item

    /*!
      @brief call
      @param maxMaxYieldLoops [out]
      @return value
     */
    SAPDB_Bool MaxMaxYieldLoops (SAPDB_UInt8 &maxMaxYieldLoops);                ///< maximum of the maximum yield loops within the current item

    /*!
      @brief call
      @param minMaxYieldLoops [out]
      @return value
     */
    SAPDB_Bool MinMaxYieldLoops (SAPDB_UInt8 &minMaxYieldLoops);                ///< minimum of the maximum yield loops within the current item

    /*!
      @brief call
      @param avgMaxYieldLoops [out]
      @return value
     */
    SAPDB_Bool AvgMaxYieldLoops (SAPDB_UInt8 &avgMaxYieldLoops);                ///< average of the maximum yield loops within the current item

    /*!
      @brief call
      @param maxCurrentLoops [out]
      @return value
     */
    SAPDB_Bool MaxCurrentLoops (SAPDB_UInt8 &maxCurrentLoops);                  ///< maximum of the current loops within the current item

    /*!
      @brief call
      @param minCurrentLoops [out]
      @return value
     */
    SAPDB_Bool MinCurrentLoops (SAPDB_UInt8 &minCurrentLoops);                  ///< minimum of the current loops within the current item

    /*!
      @brief call
      @param avgCurrentLoops [out]
      @return value
     */
    SAPDB_Bool AvgCurrentLoops (SAPDB_UInt8 &avgCurrentLoops);                  ///< average of the current loops within the current item

    /*!
      @brief call
      @param pRWRegionRegister [out]
      @return value
     */
    SAPDB_Bool RWRegionRegister(RTESync_RWRegionRegister *&pRWRegionRegister);  ///< pointer to RWRegion register of the current item

    /* -----------------------------------------------------------------------*/
    /*! endchapter: Data Access                                               */
    /* -----------------------------------------------------------------------*/


private:

    void FreeArray (void);

    RTESync_SpinlockPoolRegister::StatisticInfo *m_pInfoArray;
    SAPDB_Int4                                   m_Elements;
    SAPDB_Int4                                   m_Current;
    
};
#endif // RTESYNC_SPINLOCKPOOLREGISTER_HPP



