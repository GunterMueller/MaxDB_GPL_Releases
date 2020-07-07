/*!
  @file           RTESync_InterlockedCounter.hpp
  @author         JoergM
  @ingroup        Runtime
  @brief          InterlockedCounter

    The interlocked counter handles itsa locking on its own
    to ease implementation of shared counter object.

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
#ifndef RTESYNC_INTERLOCKEDCOUNTER_HPP
#define RTESYNC_INTERLOCKEDCOUNTER_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "RunTime/Synchronisation/RTESync_Spinlock.hpp"

#if defined(_WIN32)
#include <windows.h>
#endif /* WIN32 */
/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
  @class RTESync_InterlockedCounter
  @brief Template class for Interlocked Counter (with Sample Code)

  This class allows to instantiate counter variables that are atomically
  incrementable, decrementable. Getting and setting a value is supported
  too as well as a copy of one interlocked counter variable into a second.

  Only assignment operators are defined. Other Operators are not defined!

  NOTE: On WIN32 the RTESync_InterlockedCounter<long> is the fastest

  Example:

  RTESync_InterlockedCounter<SAPDB_Int8> Counter;

  if ( Counter != 0 ) vabort("Should be 0");

  // if ( Counter != 0 ) will work, since their is a cast operator defined that
  // allows the compiler to convert Counter into an SAPDB_Int8 which can be used
  // in the comparison. Counter.GetValue() is an alternative writing for it, but
  // longer...

  Counter.SetValue(4);
  Counter.Increment(); // implizit argument 'increment by 1'
  Counter.Increment(4);

  // loop should loop 9 times...
  for ( SAPDB_Int8 i = Counter; i > 0; i-- )
  {
    Counter.Decrement(2);
    Counter.Decrement(); // implizit argument 'decrement by 1'
  }

  if ( Counter != -18 ) vabort("9-27 should be -18");
 */
template <class T_CounterType>
class RTESync_InterlockedCounter
{
public:
  /*!
   @brief Template Constructor

    The constructor allocates an RTE_Spinlock and sets the initial value
    @param InitialValue [in] optional intial value
   */
  explicit RTESync_InterlockedCounter(T_CounterType InitialValue=0)
    : m_Value(InitialValue)
  {
  }

  /*!
    @brief Template Copy constructor

    The copy is independend interlocked from the first counter. Therefore a new
    SpinlockHook must be created.
    @param CounterToCopy [in] counter object to copy
   */
  RTESync_InterlockedCounter(const RTESync_InterlockedCounter<T_CounterType> &CounterToCopy)
    : m_Value(CounterToCopy.GetValue())
  {
  }

  /*!
    @brief Get the current counter value
    @return current counter value
   */
  T_CounterType GetValue() const
  {
    T_CounterType CurrentValue;
    {
      RTESync_LockedScope Lock(m_Spinlock);
      CurrentValue = m_Value;
    }
    return CurrentValue;
  }

  /*!
    @brief Set the current counter value
    @param NewValue [in] new counter value
   */
  void SetValue(const T_CounterType NewValue)
  {
    RTESync_LockedScope Lock(m_Spinlock);
    m_Value = NewValue;
  }

  /*!
    @brief Increment the counter (Default amount is 1)

    The pointer to old value can be used in case InterlockedCounter is used as a unique number generator

    @param Amount [in] the amount to add
    @param pOldValue [out] pointer to the value before adding Amount
    @return result of increment
   */
  T_CounterType Increment(const T_CounterType Amount=1, T_CounterType *pOldValue=0)
  {
    T_CounterType CurrentValue;
    RTESync_LockedScope Lock(m_Spinlock);
    {
        if ( pOldValue ) *pOldValue = m_Value;
        m_Value += Amount;
        CurrentValue = m_Value;
    }
    return CurrentValue;
  }

  /*!
    @brief Decrement the counter (Default amount is 1)
    @param Amount [in] the amount to add
    @return result of decrement
   */
  T_CounterType Decrement(const T_CounterType Amount=1)
  {
    T_CounterType CurrentValue;
    RTESync_LockedScope Lock(m_Spinlock);
    {
        m_Value -= Amount;
        CurrentValue = m_Value;
    }
    return CurrentValue;
  }

  /*!
    @brief Assignment operator sets new counter value
    @param NewValue [in] the new value of the counter
   */
  RTESync_InterlockedCounter<T_CounterType> & operator=(const T_CounterType NewValue)
  {
    {
      RTESync_LockedScope Lock(m_Spinlock);
      m_Value = NewValue;
    }
    return *this;
  }

  /*!
    @brief Assignment operator sets new counter value from other counter
    @param CounterToAssign [in] the counter object to assign
   */
  RTESync_InterlockedCounter<T_CounterType> & operator=(const RTESync_InterlockedCounter<T_CounterType> &CounterToAssign)
  {
    if ( &CounterToAssign != this ) // GetValue would deadlock otherwise
    {
      RTESync_LockedScope Lock(m_Spinlock);
      m_Value = CounterToAssign.GetValue();
    }
    return *this;
  }

  /*!
    @brief This typecast operator allows to use the value of the counter
    
    The value of a RTESync_InterlockedCounter class can be used directly in comparisons
    etc due to this typecast operator. Look for example at the following statement
    
      if ( Counter != 7 )
      
    Instead of casting the value '7' into a RTESync_InterlockedCounter<> instance, the 
    Counter is casted into its type and then the comparison can be done. This is much cheaper.
   */
  operator T_CounterType() const
  {
    return m_Value;
  }

  /*!
    @brief Destructor of counter
   */
  ~RTESync_InterlockedCounter()
  {
  }

private:
  T_CounterType            m_Value;    ///< The counter value
  mutable RTESync_Spinlock m_Spinlock; ///< The spinlock used to protect the counter
};

//------------------------------------------------------------
// Template Specialization
//------------------------------------------------------------

#if defined(_WIN32)
// Explicit spezialisation for long type
template<> class RTESync_InterlockedCounter<long>
{
public:
  /*!
    @brief Constructor

    The constructor allocates an SpinlockHook and sets the initial value
    @param InitialialValue [in] optional intial value
   */
  RTESync_InterlockedCounter(long InitialValue=0)
  {
    m_Value = InitialValue;
  }

  /*!
    @brief Copy constructor

    The copy is independend interlocked from the first counter. Therefore a new
    SpinlockHook must be created.
    @param CounterToCopy [in] counter object to copy
   */
  RTESync_InterlockedCounter(const RTESync_InterlockedCounter<long> &CounterToCopy)
  {
    m_Value = CounterToCopy.m_Value;
  }

  /*!
    @brief Get the current counter value
    @return current counter value
   */
  long GetValue() const
  {
    return m_Value;
  }

  /*!
    @brief Set the current counter value
    @param NewValue [in] new counter value
   */
  void SetValue(const long NewValue)
  {
    InterlockedExchange(&m_Value, NewValue);
  }

  /*!
    @brief Increment the counter (Default amount is 1)
    @param Amount [in] the amount to add
    @param pOldValue [out] pointer to the value before adding Amount
    @return result of increment
  */
  long Increment(const long Amount=1, long *pOldValue=0)
  {
    long previousValue = InterlockedExchangeAdd(&m_Value, Amount);
    if ( pOldValue ) *pOldValue = previousValue;
    return previousValue + Amount;
  }

  /*!
    @brief Decrement the counter (Default amount is 1)
    @param Amount [in] the amount to add
    @return result of decrement
   */
  long Decrement(const long Amount=1)
  {
    long previousValue = InterlockedExchangeAdd(&m_Value, -Amount);
    return previousValue - Amount;
  }

  /*!
    @brief Assignment operator sets new counter value
    @param NewValue [in] the new value of the counter
   */
  RTESync_InterlockedCounter<long> &operator =(const long NewValue)
  {
    InterlockedExchange(&m_Value, NewValue);
    return *this;
  }

  /*!
    @brief Assignment operator sets new counter value from other counter
    @param CounterToAssign [in] the counter object to assign
   */
  long &operator =(const RTESync_InterlockedCounter<long> &CounterToAssign)
  {
    InterlockedExchange(&m_Value, CounterToAssign.GetValue());
    return m_Value;
  }

  /*!
    @brief This typecast operator allows to use the value of the counter
    
    The value of a RTESync_InterlockedCounter class can be used directly in comparisons
    etc due to this typecast operator. Look for example at the following statement
    
      if ( Counter != 7 )
      
    Instead of casting the value '7' into a RTESync_InterlockedCounter<> instance, the 
    Counter is casted into its type and then the comparison can be done. This is much cheaper.
   */
  operator long() const
  {
    return m_Value;
  }

  /*!
    @brief Destructor of counter
   */
  ~RTESync_InterlockedCounter()
  {}

private:
  long m_Value; ///< The counter value
};
#endif /* WIN32 */

/*!
  endclass: RTESync_InterlockedCounter
 */
#endif  /* RTESYNC_INTERLOCKEDCOUNTER_HPP */
