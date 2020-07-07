/*!
  @file    RTESync_SpinlockRegister.hpp
  @author  JoergM
  @ingroup Common
  @brief   Allocator registration

  This class implements a singleton, that allows to register spinlocks.
  This allows to collect the statistic information for all registered
  spinlocks.

  last changed: 2005-08-25  15:07
  see also    :

  -------------------------------------------------------------------------

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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

*****************************************************************************/
#ifndef RTESYNC_SPINLOCKREGISTER_HPP
#define RTESYNC_SPINLOCKREGISTER_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

class RTESync_NamedSpinlock;
#include "RunTime/RTE_ItemRegister.hpp"

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*! 
  @class RTESync_SpinlockRegister

  @brief The spinlock register singleton
  
  The spinlock register allows individual spinlocks to Register()/Deregister()
  themselfs. The RTESync_NamedSpinlock is able to register itself during construction
  using a Register() call in their ctor and a Deregister() call in their dtor. 
  The registration takes place automatically during construction, no extra call is needed.

  To obtain a list of all static records of all currently registered spinlocks, a single
  call to GetStatisticInfo() is used. This call will lock the register during collecting
  information, providing a consistent snapshot. After the information is no longer needed
  it must be freed with a call to FreeStatisticInfo().

  The Register() and Deregister() functions use the same internal Spinlock as the GetStatisticInfo().
  
  Since the Deregister() function is called in the dtor only, the spinlock instances will never
  be detructed while the lock in the RTESync_SpinlockRegister is hold.

  Example:

  \#include "Runtime/Synchronisation/RTESync_SpinlockRegister.hpp"

  void SaveSpinlockInfo()
  {
      RTESync_SpinlockRegister & register = RTESync_SpinlockRegister::Instance() ;

      RTESync_SpinlockRegister::StatisticInfo * pInfo ;
      SAPDB_Int4 itemCount;

      if ( register.GetStatisticInfo(pInfo, itemCount) )
      {
          for ( SAPDB_Int4 iFirst; iFirst < itemCount; ++iFirst )
          {
              InsertIntoAllocatorSystemTable( pInfo[iItem].Name,
                                              usedCount,
                                              allocCount,
                                              info->GetBaseIdentifier() );
          }
      }

      register.Unlock();
  }

 */
class RTESync_SpinlockRegister : public RTE_ItemRegister<RTESync_NamedSpinlock>
{
public:
    static const SAPDB_Int4 m_MaxIdentfierLength;  //!< identifier maximum string length */

    /*!
      spinlock statistic recode
     */
    struct StatisticInfo
    {
        RTE_SpinlockStatistic StatisticRecord;    /*!< statistic values */
        SAPDB_UTF8            SpinlockName[40+4]; /*!< space for copy of spinlock identifier */
    };

    /*!
        @brief Return reference to single instance of RTESync_SpinlockRegister

        The InitializeInstance function must have been called before.

        @return Reference to singleton instance
     */
    static RTESync_SpinlockRegister & Instance( );

    /*!
       @brief Get a snapshot of all statistic informations
       @param pInfoArray [out] Pointer to array of StatisticInfo
       @param itemCount [out] number of items
       @param mode [in] RTESync_Spinlock::normal (default) or RTESync_Spinlock::reset for access to resettable statistic

       @return True if info array fill, false if not not enough memory
     */
    SAPDB_Bool GetStatisticInfo(StatisticInfo * &pInfoArray, SAPDB_Int4 &itemCount, RTESync_Spinlock::Mode mode = RTESync_Spinlock::normal );

    /*!
       @brief Free the snapshot of all statistic informations collected by GetStatisticInfo

       @param pInfoArray [in] Pointer to array of StatisticInfo
     */
    void FreeStatisticInfo(StatisticInfo *pInfoArray);

#ifdef AIX
    /*!
       @brief reset the register
       needed for watchdog process after kernel stopped
     */
    static void Reset(void)
    {
        m_Instance = NULL;
    }
#endif

    /*!
       @brief reset all registered elements
       needed for implementation of systemview SPINLOCKSTATISTICS
     */
    void ResetAllRegisteredElements(void);

private:
    /*!
       @brief private constructor
       @param makeBackupCopy [in] select to create a backup copy for slowknl sanity check
     */
    RTESync_SpinlockRegister(SAPDB_Bool makeBackupCopy):RTE_ItemRegister<RTESync_NamedSpinlock>(makeBackupCopy) {}

    /*!
       @brief Fill the statistic informations
       
       Called by GetStatisticInfo, to call at least memory allocation outside of locked registration code

       @param pInfoArray [out] Pointer to array of StatisticInfo
       @param itemCount [out] number of items
       @param mode [in] RTESync_Spinlock::normal (default) or RTESync_Spinlock::reset for access to resettable statistic

       @return True if info array fill, false if not not enough memory
     */
    SAPDB_Bool FillStatisticInfo(StatisticInfo * &pInfoArray, SAPDB_Int4 maxCount, SAPDB_Int4 &itemCount, RTESync_Spinlock::Mode mode );

    static RTESync_SpinlockRegister *m_Instance; //!< the singleton hook
}; /* endclass RTESync_SpinlockRegister */

/*! registration item type */
typedef RTE_ItemRegister<RTESync_NamedSpinlock>::Info RTESync_SpinlockInfo;

#endif  /* RTESYNC_SPINLOCKREGISTER_HPP */
