/*!
  @file           RTESync_NamedSpinlock.hpp
  @author         JoergM
  @ingroup        Runtime
  @brief          Spinlocks


  System Independend registerable RTE_Spinlock and RTE_LockedScope Interface

  The RTESync_Spinlock is a class that support Lock() and Unlock() of a spinlock.

  The RTESync_NamedSpinlock adds the support for statistic and registration in 
  the RTESync_SpinlockRegister::Instance().

  A spinlock is a special synchronisation object used to synchronize threads
  or processes. A spinlock can have only to states - locked or unlocked. 
  If it is already locked, a thread or processes, that tries to
  lock it again will spin (or poll) until the locking thread or process
  relaeses the hook. Therefore it is important to use this construct only
  in situtation, where the 'locker' is able to guarantee a short time for
  locking.

  The spinlock resource needs a synchronisation object in memory called a
  'RTE_Lock'. Size of this object may vary on different operating systems,
  since it is highly processor dependend. The RTE_Lock may already be
  created by another process or thread. To access such an external lock,
  the consturctor can be provided with an optionaly external lock pointer.

  The statistic information of registered spinlocks are found in system table
  SYSDD.SPINLOCKS.

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
*/
/*!***************************************************************************

  Module      : RTESync_NamedSpinlock.hpp

  -------------------------------------------------------------------------

  Responsible : Joerg Mensing - D027923

  Special area: Spinlocks
  Description : System Independend registerable RTE_Spinlock and RTE_LockedScope Interface

  The RTESync_Spinlock is a class that support Lock() and Unlock() of a spinlock.

  The RTESync_NamedSpinlock adds the support for statistic and registration in 
  the RTESync_SpinlockRegister::Instance().

  last changed: 2000-10-23  12:23
  first created:2000-02-29  9:27

  -------------------------------------------------------------------------

  copyright:    Copyright by SAP AG, 2000


    ========== licence begin LGPL
    Copyright (c) 2000-2005 SAP AG

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    ========== licence end

*****************************************************************************/


#ifndef RTESYNC_NAMEDSPINLOCK_HPP
#define RTESYNC_NAMEDSPINLOCK_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "RunTime/Synchronisation/RTESync_Spinlock.hpp"
#include "RunTime/Synchronisation/RTESync_SpinlockRegister.hpp"
#include <string.h>
#include <memory.h>

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!---------------------------------------------------------------------------
  @class  RTESync_NamedSpinlock
  @brief  This class implements the registered OS dependend spinlock hook

  This class allows to Lock() and Unlock() a spinlock. The spinlock may be
  accessed via internal or external RTE_Lock. The constructor decides, if the
  Unlock() is called during destruction, or the spinlock is left in its last
  state. The RTE_ISystem implementation decides how to yield, when a spinlock
  cannot be locked on first attempt.
 */
class RTESync_NamedSpinlock : public RTESync_Spinlock
{
public:
  /*!
    @brief Construct a spinlock

    The constructor has several options. The RTE_Lock can be internal or external.
    An external RTE_Lock is i.e. usefull to synchronize via an external provided
    shared memory segment that already contains an RTE_Lock. If external, it may
    not be usefull to initialze the RTE_Lock into a defined state. Therefore this
    is optional. For the same reason the RTE_Lock may not be necessarily unlock 
    during destruction of this object. So this is also optional.

    If the spinlock is internal, it cannot be exported!

    If an array of spinlocks is needed, it can only have default behaviour. An array
    of external spinlocks is not supported. If you need such a thing use a loop
    over individual RTESync_SpinlockHock constructors...

    NOTE This class does not support Copy ctor. It makes no sense at all for internal
    spinlock and is not really needed for external spinlocks. So it is forbidden now.
    
    @param Name         [in] The name of the spinlock
    @param ExternalHook [in] If non NULL used instead of internal storage
    @param DoInitialize [in] If set this flags forces initialization of RTE_Lock
    @param DoUnlock     [in] If set this flags forces unlock of RTE_Lock in destructor
   */
  RTESync_NamedSpinlock( const SAPDB_UTF8 *Name=(const SAPDB_UTF8 *)"NoName",
                         RTE_Lock * ExternalHook = (RTE_Lock *)0,
                         bool DoInitialize=true,
                         bool DoUnlock=true)
    : RTESync_Spinlock(ExternalHook, DoInitialize, DoUnlock),
      m_SpinlockInfo(Name,0)
  {
      SetIdentifier(Name);
      RTESync_Spinlock::SetName(&m_Name[0]);
      memset(&m_Statistic, 0, sizeof(m_Statistic));
      SetStatisticRecord(&m_Statistic);

      memset(&m_StatisticAtReset, 0, sizeof(m_StatisticAtReset));
      SetStatisticRecord(&m_StatisticAtReset,RTESync_Spinlock::reset);

      // m_Name and this is now known and RTESync_SpinlockInfo is of fixed size...
      new ( ((void *)&m_SpinlockInfo) ) RTESync_SpinlockInfo ( (const SAPDB_UTF8 *)&m_Name[0], this );

      RTESync_SpinlockRegister::Instance().Register(m_SpinlockInfo);
  }

  /*!
    @brief Destructor should unlock the Spinlock space if indicated by flag m_DoUnlock
   */
  ~RTESync_NamedSpinlock()
  {
      RTESync_SpinlockRegister::Instance().Deregister(m_SpinlockInfo);
  }

  /*!
    @brief Set the identifier of the spinlock

    Needed to make name unique, since SpinLock arrays may be created using a default name.
    If the name is not unique, identifying the spinlock becomes difficult...

    NOTE: This routine works only since the SpinlockRegister stored a pointer to m_Name and
    not the content of m_Name!!! To make sure name is not 'mixed up' register is locked during
    copy of name.

    @param Name [in] hopefully unique name for a spinlock
   */
  void SetIdentifier(const SAPDB_UTF8 *Name) 
  {
      RTESync_LockedScope lock(RTESync_SpinlockRegister::Instance().Spinlock());

      strncpy((char *)&m_Name[0], (const char *)Name, sizeof(m_Name)-1);
      m_Name[sizeof(m_Name)-1] = 0;
  }

  /*!
    @brief  Get the identifier of the spinlock
   */
  const SAPDB_UTF8 *GetIdentifier() const { return &m_Name[0]; }

private:
  RTESync_SpinlockInfo  m_SpinlockInfo; ///< The spinlock info registration record

  SAPDB_UTF8            m_Name[40+1];   ///< The copied spinlock name
  RTE_SpinlockStatistic m_Statistic;    ///< Spinloop statistic record (update done without extra spinlock...)
  RTE_SpinlockStatistic m_StatisticAtReset;    ///< Spinloop statistic record (update done without extra spinlock...)
};

#endif  /* RTESYNC_NAMEDSPINLOCK_HPP */
