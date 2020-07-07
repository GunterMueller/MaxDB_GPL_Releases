/*!
  @file           OMS_LockObject.hpp
  @author         ThomasA
  @brief          defines OmsLockObject

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

#ifndef __OMS_LOCKOBJECT_HPP
#define __OMS_LOCKOBJECT_HPP

#include "Oms/OMS_AFX.h"
#include "livecachetypes.h" // nocheck

class  OmsHandle;
class  OMS_Session;
class  OMS_SessionLockObject;

/*---------------------------------------------------------------------------------*/
/// Provides a synchronisation mechanism , which supports share and exclusive locks
/*!
** These object provide a synchronisation mechanism, which supports share and exclusive 
** locks which are valid until transaction end. The maximal wait time may be defined 
** which is controlled by the timeout process.
**
** \see \ref oms_OmsLockObjectAPI
*/
class AFX_EXT_CLASS OmsLockObject {
  friend class OmsHandle;
  friend class OMS_Session;
  friend class OMS_SessionLockObjects;
public:
  /*---------------------------------------------------------------------------------*/
  /// construtor, stores reference to OmsHandle in instance
  OmsLockObject(OmsHandle&);
  
  /*---------------------------------------------------------------------------------*/
  /// destructor, releases the lock 
  ~OmsLockObject();

  /*---------------------------------------------------------------------------------*/
  /// generates a lock identification (OmsLockHandle)
  /*!
  ** Assigns the instance a handle allocated by the liveCache. The handle corresponds to 
  ** exactly one critical section. To synchronize several liveCache tasks in this section, each 
  ** task must instantiate an instance of OmsLockObject that has the same handle. This is 
  ** made possible by a second variant of omsEnable, which enables handles to be assigned 
  ** explicitly.
  */
  void  omsEnable ();

  /*---------------------------------------------------------------------------------*/
  /// sets a given OmsLockHandle provided by the caller
  /*!
  ** Assigns a handle to the instance. This handle normally originates from an instance of 
  ** OmsLockObject of a second liveCache task. Several tasks can exchange a handle in this 
  ** way, thereby securing a critical section. The handle can be exchanged using a global 
  ** variable anchored in the component, for example.
  */
  void  omsEnable (const OmsLockHandle&);

  /*---------------------------------------------------------------------------------*/
  /// returns the lock identification (OmsLockHandle) of the instance
  const OmsLockHandle& omsGetLockHandle() const;

  /*---------------------------------------------------------------------------------*/
  /// Provides shared access to the critical section.
  /*!
  ** Provides shared access to the critical section. The instance is then in the ShareLocked 
  ** state. At any point in time, any number of OmsLockObjects of different liveCache tasks 
  ** can be in the ShareLocked state; in other words, different tasks in this state can access the 
  ** critical section at the same time. If an exclusive lock is active, the call is delayed 
  ** until exlusive lock is released or a timeout occurs.
  */
  void  omsLockShare();

  /*---------------------------------------------------------------------------------*/
  /// Provides exclusive access to the critical section.
  /*!
  ** Provides exclusive access to the critical section. The instance is then in the 
  ** ExclusiveLocked state. If there are concurrent tasks in the critical section when the call is 
  ** carried out, the maximum wait time is the time specified in the Lock Request Timeout 
  ** parameter. 
  */
  void  omsLockExclusive();

  /*---------------------------------------------------------------------------------*/
  /// releases the lock.
  /*!
  ** Releases the critical section. All tasks waiting for this event are waked up and 
  ** continue execution. omsUnlock can also be executed without errors if omsLockShare 
  ** or omsLockExclusive has not been invoked previously. If omsUnlock is not invoked 
  ** explicitly from the application, it is executed implicitly by OMS at the end of 
  ** the transaction. 
  ** Once omsUnlock has been executed, the instance is in the NotLocked state.
  */
  void  omsUnlock();

  /*---------------------------------------------------------------------------------*/
  /// Checks whether the instance holds a lock.
  /*!
  ** Provides true if the instance is in the ShareLocked or ExclusiveLocked state; 
  ** otherwise false
  */
  bool  omsIsLocked() const;

  /*---------------------------------------------------------------------------------*/
  /// Checks whether the instance holds a share lock.
  /*!
  ** Provides true if the instance is in the ShareLocked state; 
  ** otherwise false
  */
  bool  omsShareLocked() const;

  /*---------------------------------------------------------------------------------*/
  /// Checks whether the instance holds a exclusive lock.
  /*!
  ** Provides true if the instance is in the ExclusiveLocked state; 
  ** otherwise false
  */
  bool  omsExclusiveLocked() const;

private:
  friend class OMS_SessionLockObjects;
  void  Invalidate();

  OmsHandle*            m_omsHandle;  //!< pointer to OmsHandle
  OmsLockHandle         m_lockHandle; //!< the identification of the lock
  OMS_SessionLockObject *m_pLock;     //!< pointer to implementation part, see OmsLockObject.cpp 
};

#endif  // __OMS_LOCKOBJECT_HPP