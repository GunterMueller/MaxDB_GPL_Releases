/*



    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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





*/

#define	SAPDB_LVCSIM_DEPRECATED

#include "LVCSimulator/LVCSim_Client.hpp"
#include "LVCSimulator/LVCSim_KernelInterface.hpp"
#include "RunTime/Synchronisation/RTESync_RWRegion.hpp"
#include "RunTime/Synchronisation/RTESync_Spinlock.hpp"


// Old simulator OMS compatibility

void OmsAbstractObject::omsRegClass(OmsHandle &h, ClassID guid,
	size_t psoSize, size_t objSize, const char*cls_name, ClassID base_class,
	const OmsAbstractObject*pobj, OmsSchemaHandle sh, OmsContainerNo cno)
{
	omsRegClass(h, guid, cls_name, psoSize, objSize, base_class, 
		(OmsAbstractObject*) pobj, sh, cno); 
}

void OmsAbstractObject::omsRegClassAndKey(OmsHandle&h, ClassID guid,
	size_t psoSize, size_t objSize, size_t keySize, const char*cls_name,
	ClassID base_class, const OmsAbstractObject*pobj, OmsSchemaHandle sh,
	OmsContainerNo cno)
{ 
	omsRegClassAndKey(h, guid, cls_name, psoSize, keySize, objSize, base_class,
		(OmsAbstractObject*) pobj, sh, cno, false); 
}

LVCSim_StreamIOBase::~LVCSim_StreamIOBase()
{
}

LVCSim_SqlHandle::LVCSim_SqlHandle()
{
}

LVCSim_SqlHandle::~LVCSim_SqlHandle()
{
}


// compatibility for R/W locks

#ifdef WIN32
externC SAPDB_Int4 RTESys_GetLockLoopCount()
{
  return 0;
}
#endif

/// Simulator R/W lock, semaphore-based (expensive).
class SimRWLock : public RTESync_IRWRegion {
public:
  SimRWLock(SAPDB_Int8 id)
    : m_last(RTE_UnknownTaskId), m_id(id), m_lockCnt(0),
      m_excWaiting(0), m_shrWaiting(0)
  {
    m_excSem = LVCSim::SemCreate();
    m_shrSem = LVCSim::SemCreate();
  }

  virtual void enter(bool exclusive,RTE_TaskId pid=RTE_UnknownTaskId)
  {
    enterPrim(exclusive, true);
  }

  virtual void enterInterruptible(bool exclusive,bool cancelable,SAPDB_ULong timeout,bool &hasBeenInterrupted,RTE_TaskId pid=RTE_UnknownTaskId)
  {
    enterPrim(exclusive, true);
    hasBeenInterrupted = false;
  }

  virtual bool tryEnter(bool exclusive,RTE_TaskId pid=RTE_UnknownTaskId)
  {
    return enterPrim(exclusive, false);
  }

  virtual void leave(bool exclusive,RTE_TaskId pid=RTE_UnknownTaskId)
  {
    RTESync_LockedScope lck(m_lock);

    if (m_lockCnt < 0) {
      if (!exclusive) {
        error("leave(!exclusive) called when locked exclusive");
      }
      m_lockCnt = 0;
      if (m_shrWaiting > 0) {
        // signal shared waiter(s)
        int i = m_shrWaiting;
        while (--i >= 0) {
          LVCSim::SemSignal(m_shrSem);
        }
      } else if (m_excWaiting) {
        // signal other exclusive waiter to continue
        LVCSim::SemSignal(m_excSem);
      }
    } else if (m_lockCnt > 0) {
      if (exclusive) {
        error("leave(exclusive) called when locked non-exclusive");
      }
      --m_lockCnt;
      if (m_excWaiting > 0) {
        // signal one exclusive waiter
        LVCSim::SemSignal(m_excSem);
      } else if (m_shrWaiting > 0) {
        // signal shared waiter(s)
        int i = m_shrWaiting;
        while (--i >= 0) {
          LVCSim::SemSignal(m_shrSem);
        }
      }
    } else {
      error("leave called when not locked");
    }
  }

  virtual void SwitchToNonExclusiveAccess(RTE_TaskId pid=RTE_UnknownTaskId)
  {
    RTESync_LockedScope lck(m_lock);

    if (m_lockCnt < 0) {
      m_lockCnt = 1;
      // notify all shared waiters
      int i = m_shrWaiting;
      while (--i >= 0) {
        LVCSim::SemSignal(m_shrSem);
      }
    } else {
      error("Cannot switch to non-exclusive when not locked exclusive");
    }
  }

  /* the RTESync_IRWRegion interface now knows methods which take an RTETask_ITask * instead */
  /* of the task id. As both task id and task object can be ignored here, the new methods are*/
  /* just redirected to the old ones */
  virtual void enter(bool exclusive,RTETask_ITask *pTaskObject)
  {
    enter(exclusive);
  }

  virtual void enterInterruptible(bool exclusive,bool cancelable,SAPDB_ULong timeout,bool &hasBeenInterrupted,RTETask_ITask *pTaskObject)
  {
    enterInterruptible(exclusive,cancelable,timeout,hasBeenInterrupted);
  }

  virtual bool tryEnter(bool exclusive,RTETask_ITask *pTaskObject)
  {
    return tryEnter(exclusive);
  }

  virtual void leave(bool exclusive,RTETask_ITask *pTaskObject)
  {
    leave(exclusive);
  }

  virtual void SwitchToNonExclusiveAccess(RTETask_ITask *pTaskObject)
  {
    SwitchToNonExclusiveAccess();
  }

  virtual SAPDB_Int8 getId(void)
  {
    return m_id;
  }

  virtual RTE_TaskId getLastEntered(void)
  {
    return m_last;
  }

  virtual SAPDB_UInt4 GetCollisionCounter(void)
  {
      return 0;
  }

  virtual SAPDB_UInt4 GetExclusiveAccessCounter(void)
  {
      return 0;
  }

  virtual SAPDB_UInt4 GetNonExclusiveAccessCounter(void)
  {
      return 0;
  }

  virtual SAPDB_UInt4 GetWaitCounter(void)
  {
      return 0;
  }

  virtual SAPDB_UInt4 GetTasCounter(void)
  {
      return 0;
  }

  virtual void Reset(void)
  {
  }

  RTESync_SpinlockPool *GetSpinlockPool(void)
  {
    return NULL;
  }

private:
  bool enterPrim(bool exclusive, bool wait)
  {
    {
      RTESync_LockedScope lck(m_lock);

      if (exclusive) {
        // exclusive
        if (m_lockCnt == 0 && m_excWaiting == 0 && m_shrWaiting == 0) {
          m_lockCnt = -1;
          return true;
        }
        if (!wait) {
          return false;
        }
        ++m_excWaiting;
      } else {
        // shared
        if (m_lockCnt >= 0 && m_excWaiting == 0) {
          ++m_lockCnt;
          return true;
        }
        if (!wait) {
          return false;
        }
        ++m_shrWaiting;
      }
    }

  retry:
    // must wait
    LVCSim::SemWait(exclusive ? m_excSem : m_shrSem);

    // got signal, so now it must go through
    RTESync_LockedScope lck(m_lock);
    if (exclusive) {
      if (m_lockCnt != 0) {
        //error("invalid lock state when locking exclusive");
        goto retry;
      }
      // TODO: assert m_lockCnt == 0
      m_lockCnt = -1;
      --m_excWaiting;
    } else {
      if (m_lockCnt < 0) {
        //error("invalid lock state when locking non-exclusive");
        goto retry;
      }
      ++m_lockCnt;
      --m_shrWaiting;
    }

    return true;
  }

  void error(const char *msg)
  {
    throw DbpError(-28999, msg);
  }

  RTE_TaskId        m_last;
  SAPDB_Int8        m_id;
  RTESync_Spinlock  m_lock;
  int               m_lockCnt;
  LVCSim_Semaphore  *m_shrSem;
  LVCSim_Semaphore  *m_excSem;
  int               m_excWaiting;
  int               m_shrWaiting;
};

RTESync_IRWRegion *RTESync_CreateRWRegion(SAPDB_Int8 id,RTESync_SpinlockPool &pool, SAPDBMem_IRawAllocator &allocator,RTESync_IRWRegion::flavours flavour)
{
  void *space = allocator.Allocate(sizeof(SimRWLock));
  if (space == NULL) {
    return NULL;
  }
  return new(space) SimRWLock(id);
}


// needed for interfacing with dispatcher
extern "C" unsigned char g01unicode = true;

// needed for tracing messages
extern "C" void g01optextmsg (
    int                      msg_prio,
    int                      msg_type,
    int                      msg_no,
    const char*              msg_label,
    const char*              msg_text)
{
  LVCSim_KernelInterface::Instance()->ConsoleMessage(msg_text);
}

// AIX compatibility

#ifdef AIX

extern "C" {
extern void vabort(bool x);
};

extern void vabort(bool x)
{
}

#endif
