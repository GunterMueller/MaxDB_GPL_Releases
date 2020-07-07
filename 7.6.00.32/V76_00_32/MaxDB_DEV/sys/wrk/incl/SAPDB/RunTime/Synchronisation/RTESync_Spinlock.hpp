/*!
  @file           RTESync_Spinlock.hpp
  @author         JoergM
  @ingroup        Runtime
  @brief          Spinlocks

  System Independend RTE_Spinlock and RTE_LockedScope Interface

  The RTE_Spinlock is a class that support Lock() and Unlock() of a spinlock.
  The RTE_LockedScope is a class that is used to automatically protect a scope.

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
#ifndef RTESYNC_SPINLOCK_HPP
#define RTESYNC_SPINLOCK_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/RTE_Types.hpp"
#include "RunTime/System/RTESys_Spinlock.h"
#if defined(TESTSIMUL_SMALL)
extern "C" SAPDB_Int4 RTESys_GetLockLoopCount();
#else
#include "RunTime/RTE_IInterface.hpp"
#endif
#include "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"
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
  @class       RTESync_Spinlock
  @brief       This class implements the OS dependend spinlock

  This class allows to Lock() and Unlock() a spinlock. The spinlock may be
  accessed via internal or external RTE_Lock. The constructor decides, if the
  Unlock() is called during destruction, or the spinlock is left in its last
  state. The RTESys_Spinlock implementation decides how to yield, when a spinlock
  cannot be locked on first attempt.
 */
class RTESync_Spinlock
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
    
    @param ExternalHook [in] If non NULL used instead of internal storage
    @param DoInitialize [in] If set this flags forces initialization of RTE_Lock
    @param DoUnlock     [in] If set this flags forces unlock of RTE_Lock in destructor
   */
  RTESync_Spinlock(RTE_Lock * ExternalHook = (RTE_Lock *)0,
                       bool DoInitialize=true,
                       bool DoUnlock=true)
    : m_DoUnlock(DoUnlock), m_Status(0), m_StatusAtReset(0), m_SpinlockName(0)
  {
    m_Spinlock = ( ExternalHook ? ExternalHook : &m_SpinlockSpace );

    if ( DoInitialize )
    {
      RTESys_InitLock(m_Spinlock);
    }
  }

  /*!
    @brief Destructor should unlock the Spinlock space if indicated by flag m_DoUnlock
   */
  ~RTESync_Spinlock()
  {
    if ( m_DoUnlock ) RTESys_Unlock(m_Spinlock);
  }

  /*!
    @brief set name to given constant identifier

    NOTE: Name is not copied! Make sure it is not volatile...
    The RTESync_NamedSpinlock i.e. uses a name buffer inside the derived object...
   */
  void SetName(const SAPDB_UTF8 *newName) { m_SpinlockName = newName; }

  /*!
    @brief get name given
    @return given name or 0 if not named
   */
  SAPDB_UTF8 const *GetName() const { return m_SpinlockName; }

  /*!
    Statistic reconrd selection enumeration
   */
  enum Mode
  {
      normal, ///< statistic record filled normally
      reset   ///< statistic record filled until last reset statistics...
  };
  
  /*!
    @brief Retrieve the statistic record pointer for this spinlock

    NOTE that this routine does not lock the spinlock. This should be done
    from outside if necessary !!!!

    Return value: current statistic record pointer ( NULL if currently no statistics )
   */
  RTE_SpinlockStatistic * GetStatisticRecord(Mode mode = normal) const
  {
    if(reset == mode)
        return m_StatusAtReset;
    else
        return m_Status;
  }

  /*!
    @brief Set the statistic record pointer for this spinlock

    This call is needed to enable statistics. The newStatisticRecord is filled. The space
    should be on heap or shared memory...

    NOTE that this routine does not lock the spinlock. This should be done
    from outside if necessary !!!!

    @param  newStatisticRecord [in] New statistic record (or NULL if no statistic wanted)
    @param  mode [in] select the statistic record to replace
    @return [RTE_SpinlockStatistic *] replaced statistic record pointer
   */
  RTE_SpinlockStatistic * SetStatisticRecord( RTE_SpinlockStatistic * newStatisticRecord, Mode mode = normal )
  {
      RTE_SpinlockStatistic * currentStatisticRecord;
      if( normal == mode )
      {
        currentStatisticRecord = m_Status;

        m_Status = newStatisticRecord;
      }
      else
      {
        currentStatisticRecord = m_StatusAtReset;

        m_StatusAtReset = newStatisticRecord;
      }

      return currentStatisticRecord;
  }


  /*!
   @brief Try to lock the spinlock
   @return  [SAPDB_Bool] true if lock was not aquiered, false if locked now
   */
  SAPDB_Bool TryLock()
  {
      RTE_SpinlockStatistic * currentStatisticRecord = m_Status;
      if ( currentStatisticRecord )
      {
          return RTESys_TestAndLockWithStatistic( m_Spinlock,
                                                  &currentStatisticRecord->collisions,
                                                  &currentStatisticRecord->locks );
      }
      else
      {
          return RTESys_TestAndLock(m_Spinlock);
      }
  }

  /*!
   @brief Lock the Spinlock space unconditional

   After return from this call, the spinlock space is locked. If already
   locked a apropriate yield function is called in a tight loop.
   
   @param SpinLoops [in] Number of loops to take before each GiveUpTimeSlice() call
   */
  void Lock(int SpinLoops=0)
  {
    if ( SpinLoops == 0 )
    {
        /* get a system dependend resonable default */
#if defined(TESTSIMUL_SMALL)
        SpinLoops = RTESys_GetLockLoopCount();
#else
        SpinLoops = RTE_IInterface::Instance().GetLockLoopCount();
#endif
    }

    RTE_SpinlockStatistic * currentStatisticRecord = m_Status;
    if ( currentStatisticRecord )
    {
        /* PTS 1108524 */
        if ( RTESys_TestAndLock(m_Spinlock) )
        {
            SAPDB_Int4 currentLoops = 0;
            SAPDB_Int4 i;
            /* Loop before sleeping */
            for ( i = 1; i <= SpinLoops; i++ )
            {
                if ( !RTESys_TestAndLock(m_Spinlock) )
                {
                    /* Now in locked state... */
                    currentStatisticRecord->currentLoops = 0;

                    if ( i > currentStatisticRecord->maxSpinLoops )
                    {
                        currentStatisticRecord->maxSpinLoops = i;
                    }
                    currentStatisticRecord->totalSpinLoops += i;
                    ++currentStatisticRecord->collisions;
					++currentStatisticRecord->locks;			/* PTS 1111825 */
                    return;
                }
                else
                {
                    ++currentLoops;
                    /* dirty write, if more then one Spinner. But at least each */
                    /* Spinner writes its correct value... */
                    currentStatisticRecord->currentLoops = currentLoops;
                }
            }

            /* SpinLoops did not help us, so we do YieldLoops now */

#if defined(TESTSIMUL_SMALL)
            do {
                RTESys_GiveUpTimeSlice();
                ++currentLoops;
                /* dirty write, if more then one Spinner. But at least each */
                /* Spinner writes its correct value... */
                currentStatisticRecord->currentLoops = currentLoops;
            } while ( RTESys_TestAndLock(m_Spinlock) );
#else
            RTE_IInterface::Instance().SetTaskStateToYieldOnSpinlock(this, true);

            do {
                RTE_IInterface::Instance().TaskYield();
                ++currentLoops;
                /* dirty write, if more then one Spinner. But at least each */
                /* Spinner writes its correct value... */
                currentStatisticRecord->currentLoops = currentLoops;
            } while ( RTESys_TestAndLock(m_Spinlock) );

            RTE_IInterface::Instance().SetTaskStateToYieldOnSpinlock(this, false);
#endif

            currentStatisticRecord->currentLoops = 0;
 
            if ( (currentLoops - SpinLoops) > currentStatisticRecord->maxYieldLoops )
            {
                currentStatisticRecord->maxYieldLoops = currentLoops - SpinLoops;
            }

            if ( SpinLoops > currentStatisticRecord->maxSpinLoops )
            {
                currentStatisticRecord->maxSpinLoops = SpinLoops;
            }

            currentStatisticRecord->totalYieldLoops += currentLoops - SpinLoops;       /* PTS 1111825 */

            /* Now in locked state... */
            ++currentStatisticRecord->collisions;
        }

        ++currentStatisticRecord->locks;
    }
    else
    {
        /* No Statistic record... */

        if ( RTESys_TestAndLock(m_Spinlock) )
        {
          // Loop before sleeping
          for ( int i = 0; i < SpinLoops; i++ )
          {
            if ( !RTESys_TestAndLock(m_Spinlock) )
            {
                /* Now in locked state... */
              return;
            }
          }

          /* SpinLoops did not help us, so we do YieldLoops now */
#if defined(TESTSIMUL_SMALL)
          do {
              RTESys_GiveUpTimeSlice();
          } while ( RTESys_TestAndLock(m_Spinlock) );
#else
          RTE_IInterface::Instance().SetTaskStateToYieldOnSpinlock(this, true);

          do {
              RTE_IInterface::Instance().TaskYield();
          } while ( RTESys_TestAndLock(m_Spinlock) );

          RTE_IInterface::Instance().SetTaskStateToYieldOnSpinlock(this, false);
#endif
        }
    }
  }

  /*!
    @brief Unlocks the Spinlock space unconditional
   */
  void Unlock() { RTESys_Unlock(m_Spinlock); }
  
  /*!
    @brief Reset internal resetable statistic 
   */
  void Reset(void)
  {
      if( m_Status && m_StatusAtReset )
      {
          SAPDB_MemCopyNoCheck( m_StatusAtReset , m_Status , sizeof(RTE_SpinlockStatistic));
      }
  }

private:
  RTESync_Spinlock( const RTESync_Spinlock & ) {} ///< do not allow a copy constructor

  RTE_Lock  m_SpinlockSpace; ///< Spinlock space if no external space given
  RTE_Lock *m_Spinlock; ///< Pointer to used spinlock space
  bool      m_DoUnlock; ///< Flag for destructor
  RTE_SpinlockStatistic *m_Status; ///< Spinloop statistic record
  RTE_SpinlockStatistic *m_StatusAtReset; ///< Spinloop statistic record at last reset
  const SAPDB_UTF8 *m_SpinlockName; ///< Spinlock name (not necessarily unique)
};


/*!
  @class RTESync_LockedScope
  @brief Class for spinlocked scope for automatic lock/unlock of a scope
  
  This class allows to synchronize access to regions of code that are not 
  allowed to be entered by several threads of context at a time. 

  NOTE This interface does busy waiting until lock is granted! It is more
  than other classes only to be used to protect SHORT sections of code or
  otherwise PERFORMANCE WILL DROP DRAMATICALLY.

  VERY IMPORTANT!!! This Interface is not reentrant! If you create a second
  RTESync_LockedScope on the same RTESync_Spinlock you produce a non detectable
  deadlock!!! The RTESync_LockedScope does not provide any information who did the
  lock, therefore it cannot check on reentrancy.

  If an RTESync_LockedScope is created on the heap, it locks the associated region
  until deleted. Placement new / calling of destructor can replace 'Lock' and 
  'Unlock'. But multithreaded access is not synchronized in such a case! If such
  a heap class is used, it should be synchronized from outside.

  @code

  // Primitive StackObject for stacking SAPDB_Int4 ...
  class RTE_StackObject {
  public:
    RTE_StackObject(SAPDB_Int4 val=0) : m_val(val), m_Next(0) {}

    SAPDB_Int4 GetVal()               { return m_val; }
    SAPDB_Int4 SetVal(SAPDB_Int4 val) { m_val = val; }

    RTE_StackObject * GetNext()                         { return m_Next; }
    void              SetNext(RTE_StackObject *theNext) { m_Next = theNext; }

  private:
    SAPDB_Int4 m_val;
    RTE_StackObject *m_Next;
  };

  // Sample code for implementation of simple stack

  // TRICK
  // Using RTESync_Spinlock as base class allows to Lock and Unlock a RTE_Stack!

  class RTE_Stack .. public RTESync_Spinlock {

  private:
    RTE_StackObject *m_TopOfStack;

  public:

    RTE_Stack().. m_TopOfStack(0) { EndOfStack = (RTE_StackObject *)-1; }

    ~RTE_Stack() { } // The spinlock is destroyed automatically

    void Push(RTE_StackObject & Object) {

      // The whole member function scope is locked
      RTESync_LockedScope Lock(*this);

      Object.SetNext(m_TopOfStack);
      m_TopOfStack = & Object;
    }

    RTE_StackObject & Pop() {

      RTE_StackObject * ReturnObject;

      { // Only this scope is locked

        RTESync_LockedScope Lock(*this);
  
        ReturnObject = m_TopOfStack;

        if ( m_TopOfStack )
        {
          m_TopOfStack = m_TopOfStack->GetNext();
        }
      } // after this scope unlocked action can continue

      return ( ReturnObject ? *EndOfStack .. *ReturnObject );
    }

    RTE_StackObject *EndOfStack;

  };

  @endcode
*/
class RTESync_LockedScope
{
public:
/*!
  @brief Constructor locks the spinlock.
  @param Hook [in] The Spinlock hook used
  @param SpinLoops [in] Number of Spinlock loops before Yield() call is done
 */
  RTESync_LockedScope(RTESync_Spinlock & Hook, int SpinLoops=0) : m_Hook(Hook)
  {
    m_Hook.Lock(SpinLoops);
  }

/*! 
  @brief Destructor gives the lock free
 */
  ~RTESync_LockedScope()
  {
    m_Hook.Unlock(); 
  }

private:
  RTESync_Spinlock & m_Hook; ///< The reference to the spinlock object
};

#endif  /* RTESYNC_SPINLOCK_HPP */
