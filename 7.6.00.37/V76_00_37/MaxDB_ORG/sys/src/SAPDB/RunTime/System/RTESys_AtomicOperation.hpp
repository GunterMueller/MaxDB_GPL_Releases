/*!
  @file           RTESys_AtomicOperation.hpp
  @author         JoergM
  @ingroup        Runtime
  @brief          Atomic operation (C++ Interface) (cmpxchg, atomic_increment, atomic_decrement)

  This header files defines five function for atomic read, compare and exchange and atomic modifications.

  The functions are RTESys_AtomicRead, RTESys_AtomicWrite RTESys_CompareAndExchange, RTESys_AtomicModify
  and RTESys_AtomicOperation.

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
#ifndef RTESYS_ATOMICOPERATION_HPP
#define RTESYS_ATOMICOPERATION_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/System/RTESys_AtomicOperation.h"

//--------------------------------------------------------------------------------
// RTESys_AtomicRead
//--------------------------------------------------------------------------------

/*
  These functions have been introducded to allow consistent atomic access on various types

  With more widespread use of CompareAndExchange and to prevent otherwise needed MemoryBarriers
  the usage was extended to atomic access on shared data!
 */

/*!
   @brief Atomic read operation on 8byte aligned signed value

   @param memoryPosition [inout] the memory position to read
   @return the value at the given position
 */
inline SAPDB_Int8 RTESys_AtomicRead( SAPDB_Int8 & memoryPosition ) { return RTESys_AtomicReadInt8(memoryPosition); }
/*!
   @brief same of 8byte aligned unsigned value

   @param memoryPosition [inout] the memory position to read
   @return [SAPDB_UInt8] the value at the given position
 */
inline SAPDB_UInt8 RTESys_AtomicRead( SAPDB_UInt8 & memoryPosition ) { return RTESys_AtomicReadUInt8(memoryPosition); }
/*!
   @brief Atomic read operation on 4byte aligned signed value

   @param memoryPosition [inout] the memory position to read
   @return [SAPDB_Int4] the value at the given position
 */
inline SAPDB_Int4 RTESys_AtomicRead( SAPDB_Int4 & memoryPosition ) { return RTESys_AtomicReadInt4(memoryPosition); }
/*!
   @brief same of 4byte aligned unsigned value

   @param memoryPosition [inout] the memory position to read
   @return [SAPDB_UInt4] the value at the given position
 */
inline SAPDB_UInt4 RTESys_AtomicRead( SAPDB_UInt4 & memoryPosition ) { return RTESys_AtomicReadUInt4(memoryPosition); }
/*!
   @brief Atomic read operation of a pointer

   @param memoryPosition [inout] the memory position to read
   @return [void *] the pointer at the given position
 */
template <class PointerType>
inline PointerType * RTESys_AtomicRead( PointerType * &memoryPosition ) 
{ return (PointerType *)RTESys_AtomicReadPointer( *(void **)&memoryPosition ); }

//--------------------------------------------------------------------------------
// RTESys_AtomicWrite
//--------------------------------------------------------------------------------

/*!
   @brief Atomic write operation on 8byte aligned signed value

   @param memoryPosition [inout] the memory position to write to
   @param newValue [in] the new value for the given position
   @return none
 */
inline void RTESys_AtomicWrite( SAPDB_Int8 & memoryPosition, SAPDB_Int8 const newValue )
{ RTESys_AtomicWriteInt8(memoryPosition, newValue); }
/*!
   @brief same of 8byte aligned unsigned value

   @param memoryPosition [inout] the memory position to read
   @param newValue [in] the new value for the given position
   @return none
 */
inline void RTESys_AtomicWrite( SAPDB_UInt8 & memoryPosition, SAPDB_UInt8 const newValue )
{ RTESys_AtomicWriteUInt8(memoryPosition, newValue); }
/*!
   @brief Atomic write operation on 4byte aligned signed value

   @param memoryPosition [inout] the memory position to write to
   @param newValue [in] the new value for the given position
   @return none
 */
inline void RTESys_AtomicWrite( SAPDB_Int4 & memoryPosition, SAPDB_Int4 const newValue )
{ RTESys_AtomicWriteInt4(memoryPosition, newValue); }
/*!
   @brief same of 4byte aligned unsigned value

   @param memoryPosition [inout] the memory position to read
   @param newValue [in] the new value for the given position
   @return none
 */
inline void RTESys_AtomicWrite( SAPDB_UInt4 & memoryPosition, SAPDB_UInt4 const newValue )
{ RTESys_AtomicWriteUInt4(memoryPosition, newValue); }
/*!
   @brief Atomic write operation of a pointer

   @param memoryPosition [inout] the memory position to read
   @param newPointer [in] the new pounter for the given position
   @return none
 */
template <class PointerType>
inline void RTESys_AtomicWrite( PointerType * &memoryPosition, PointerType * const newPointer ) 
{ RTESys_AtomicWritePointer( *(void **)&memoryPosition, (void*)newPointer ); }

#if !defined(BIT64)
/*!
   @brief Atomic write operation on 4byte aligned unsigned value

   @param memoryPosition [in] the memory position to write
   @param newValue [in] the new value for the given position
 */
inline void RTESys_AtomicWrite( SAPDB_Long & memoryPosition, SAPDB_Long const newValue )
{ RTESys_AtomicWriteInt4( *(SAPDB_Int4 *)&memoryPosition, (SAPDB_Int4)newValue); }
/*!
   @brief Atomic write operation on 4byte aligned unsigned value

   @param memoryPosition [in] the memory position to write
   @param newValue [in] the new value for the given position
 */
inline void RTESys_AtomicWrite( SAPDB_ULong & memoryPosition, SAPDB_ULong const newValue )
{ RTESys_AtomicWriteUInt4( *(SAPDB_UInt4 *)&memoryPosition, (SAPDB_UInt4)newValue); }
#endif

//--------------------------------------------------------------------------------
// RTESys_CompareAndExchange
//--------------------------------------------------------------------------------

/*! 
   @brief Atomic CompareAndExchange value

   NOTE: since normaly some modification have been made on a structure, these calls IMPLICITLY enforces
   memory barriers, so that all updates are visible.

   If the old value is no longer matching, the function does not modify the memory
   and returns false. Otherwise the new value is set and true is returned.

  @param memoryPosition [in] the memory position to exchange
  @param expectedValue [in] the expected old value
  @param newValue [in] the wanted new value
  @param oldValue [out] the compared old value
  @return [SAPDB_Bool] true if newValue replaced oldValue, false if expectedValue was no longer found */
inline SAPDB_Bool RTESys_CompareAndExchange( SAPDB_Int4 &     memoryPosition,
                                             SAPDB_Int4 const expectedValue,
                                             SAPDB_Int4 const newValue,
                                             SAPDB_Int4 &     oldValue )
{ return RTESys_CmpxchgInt4(memoryPosition, expectedValue, newValue, oldValue); }

/*! @brief Atomic CompareAndExchange value
    @param memoryPosition [in] the memory position to exchange
    @param expectedValue [in] the expected old value
    @param newValue [in] the wanted new value
    @param oldValue [out] the compared old value
    @return [SAPDB_Bool] true if newValue replaced oldValue, false if expectedValue was no longer found */
inline SAPDB_Bool RTESys_CompareAndExchange( SAPDB_Int8 &     memoryPosition,
                                             SAPDB_Int8 const expectedValue,
                                             SAPDB_Int8 const newValue,
                                             SAPDB_Int8 &     oldValue )
{ return RTESys_CmpxchgInt8(memoryPosition, expectedValue, newValue, oldValue); }

/*! @brief Atomic CompareAndExchange value
    @param memoryPosition [in] the memory position to exchange
    @param expectedValue [in] the expected old valueq
    @param newValue [in] the wanted new value
    @param oldValue [out] the compared old value
    @return [SAPDB_Bool] true if newValue replaced oldValue, false if expectedValue was no longer found */
inline SAPDB_Bool RTESys_CompareAndExchange( SAPDB_UInt4 &     memoryPosition,
                                             SAPDB_UInt4 const expectedValue,
                                             SAPDB_UInt4 const newValue,
                                             SAPDB_UInt4 &     oldValue )
{ return RTESys_CmpxchgUInt4(memoryPosition, expectedValue, newValue, oldValue); }
/*! @brief Atomic CompareAndExchange value
    @param memoryPosition [in] the memory position to exchange
    @param expectedValue [in] the expected old value
    @param newValue [in] the wanted new value
    @param oldValue [out] the compared old value
    @return [SAPDB_Bool] true if newValue replaced oldValue, false if expectedValue was no longer found */
inline SAPDB_Bool RTESys_CompareAndExchange( SAPDB_UInt8 &     memoryPosition,
                                             SAPDB_UInt8 const expectedValue,
                                             SAPDB_UInt8 const newValue,
                                             SAPDB_UInt8 &     oldValue )
{ return RTESys_CmpxchgUInt8(memoryPosition, expectedValue, newValue, oldValue); }

/*! @brief Atomic CompareAndExchange value
    @param pointerPosition [in] the memory position to modify
    @param expectedValue [in] the expected old value
    @param newValue [in] the wanted new value
    @param oldValue [out] the compared old value
    @return [SAPDB_Bool] true if newValue replaced oldValue, false if expectedValue was no longer found */
template <class PointerType>
inline SAPDB_Bool RTESys_CompareAndExchange( PointerType * &     pointerPosition,
                                             PointerType * const expectedValue,
                                             PointerType * const newValue,
                                             PointerType * &     oldValue )
{
    void **voidPosition = (void **)&pointerPosition;
    void *oldValueFound;
    SAPDB_Bool result = RTESys_CmpxchgPointer(*voidPosition, expectedValue, newValue, oldValueFound);
    oldValue = (PointerType *)oldValueFound; 
    return result;
}

#if !defined(BIT64)
/*! @brief Atomic CompareAndExchange value
    @param memoryPosition [in] the memory position to exchange
    @param expectedValue [in] the expected old value
    @param newValue [in] the wanted new value
    @param oldValue [out] the compared old value
    @return [SAPDB_Bool] true if newValue replaced oldValue, false if expectedValue was no longer found */
inline SAPDB_Bool RTESys_CompareAndExchange( SAPDB_Long &     memoryPosition,
                                             SAPDB_Long const expectedValue,
                                             SAPDB_Long const newValue,
                                             SAPDB_Long &     oldValue )
{
    SAPDB_Int4 &castedMemoryPosition = *(SAPDB_Int4 *)&memoryPosition;
    SAPDB_Int4 &castedOldValue       = *(SAPDB_Int4 *)&oldValue;
    return RTESys_CmpxchgInt4(castedMemoryPosition, expectedValue, newValue, castedOldValue); 
}

/*! @brief Atomic CompareAndExchange value
    @param memoryPosition [in] the memory position to exchange
    @param expectedValue [in] the expected old value
    @param newValue [in] the wanted new value
    @param oldValue [out] the compared old value
    @return [SAPDB_Bool] true if newValue replaced oldValue, false if expectedValue was no longer found */
inline SAPDB_Bool RTESys_CompareAndExchange( SAPDB_ULong &     memoryPosition,
                                             SAPDB_ULong const expectedValue,
                                             SAPDB_ULong const newValue,
                                             SAPDB_ULong &     oldValue )
{
    SAPDB_UInt4 &castedMemoryPosition = *(SAPDB_UInt4 *)&memoryPosition;
    SAPDB_UInt4 &castedOldValue       = *(SAPDB_UInt4 *)&oldValue;
    return RTESys_CmpxchgUInt4(castedMemoryPosition, expectedValue, newValue, castedOldValue); 
}

#if defined(RTESYS_HAS_ATOMIC_CAS2_OPERATION)
/*! 
    @brief Atomic CompareAndExchange of two SAPDB_ULong values

    @param pointerPosition [in] the memory position to modify
    @param expectedValue [in] the expected old value
    @param newValue [in] the wanted new value
    @param oldValue [out] the compared old value
    @return [SAPDB_Bool] true if newValue replaced oldValue, false if expectedValue was no longer found 
 */
inline SAPDB_Bool RTESys_CompareAndExchangeTwoULong( RTESys_DoubleULongValue       & exchangePosition,
                                              RTESys_DoubleULongValue const & expectedValues,
                                              RTESys_DoubleULongValue const & newValues,
                                              RTESys_DoubleULongValue       & oldValues )
{
    SAPDB_UInt8       *pPosition  = (SAPDB_UInt8 *)&exchangePosition;
    SAPDB_UInt8 const *pExpected  = (SAPDB_UInt8 *)&expectedValues;
    SAPDB_UInt8 const *pNewValues = (SAPDB_UInt8 *)&newValues;
    SAPDB_UInt8       *pOldValues = (SAPDB_UInt8 *)&oldValues;

    return RTESys_CmpxchgUInt8( *pPosition
                              , *pExpected
                              , *pNewValues
                              , *pOldValues );
}
#endif

#endif /* !BIT64 */


//--------------------------------------------------------------------------------
// RTESys_AtomicModify
//--------------------------------------------------------------------------------

/*!
   @brief Atomic modification operation

   The value at the given position is modified by adding the delta value atomically.

       memoryPosition = memoryPostion + deltaValue

   This allows to implement AtomicIncrement/AtomicDecrement

   @param memoryPosition [inout] the memory position to exchange
   @param deltaValue [in] the delta to add
   @return resulting value
 */
inline SAPDB_Int4 RTESys_AtomicModify( SAPDB_Int4 &     memoryPosition,
                                 SAPDB_Int4 const deltaValue )
{ return RTESys_AtomicModifyInt4(memoryPosition, deltaValue); }
/*! @brief Atomic modification operation
    @param memoryPosition [in] the memory position to exchange
    @param deltaValue [in] the delta to add
    @return resulting value
 */
inline SAPDB_Int8 RTESys_AtomicModify( SAPDB_Int8 &     memoryPosition,
                                 SAPDB_Int8 const deltaValue )
{ return RTESys_AtomicModifyInt8(memoryPosition, deltaValue); }
/*! @brief Atomic modification operation
    @param memoryPosition [in] the memory position to exchange
    @param deltaValue [in] the delta to add
    @return resulting value
 */
inline SAPDB_UInt4 RTESys_AtomicModify( SAPDB_UInt4 &    memoryPosition,
                                 SAPDB_Int4 const deltaValue )
{ return RTESys_AtomicModifyUInt4(memoryPosition, deltaValue); }
/*! @brief Atomic modification operation
    @param memoryPosition [in] the memory position to exchange
    @param deltaValue [in] the delta to add
    @return resulting value
 */
inline SAPDB_UInt8 RTESys_AtomicModify( SAPDB_UInt8 &    memoryPosition,
                                 SAPDB_Int8 const deltaValue )
{ return RTESys_AtomicModifyUInt8(memoryPosition, deltaValue); }

/*! @brief Atomic modification operation
    @param pointerPosition [in] the memory position to modify
    @param scaling [in] the size of the pointed object
    @param deltaValue [in] the value to add
    @return resulting value
 */
inline void * RTESys_AtomicModify( void *          & pointerPosition,
                                 SAPDB_ULong const scaling,
                                 SAPDB_Long const  deltaValue )
{ return RTESys_AtomicModifyPointer(pointerPosition, scaling, deltaValue); }

/*! @brief Atomic modification operation
    This works as normal pointer arithmetic does
    @param pointerPosition [in] the memory position to modify
    @param deltaValue [in] the value to add
    @return resulting value
 */
template <class PointerType>
void * RTESys_AtomicModify( PointerType * &   pointerPosition,
                            SAPDB_Long const  deltaValue )
{
    void **voidPosition = (void **)&pointerPosition;
    return RTESys_AtomicModifyPointer(*voidPosition, sizeof(PointerType), deltaValue);
}

#if !defined(BIT64)

/*!
   @brief Atomic modification operation

   The value at the given position is modified by adding the delta value atomically.

       memoryPosition = memoryPostion + deltaValue

   This allows to implement AtomicIncrement/AtomicDecrement

   @param memoryPosition [inout] the memory position to exchange
   @param deltaValue [in] the delta to add
   @return resulting value
 */
inline SAPDB_Long RTESys_AtomicModify( SAPDB_Long & memoryPosition
                                     , SAPDB_Long const deltaValue )
{
    SAPDB_Int4 &castedMemoryPosition = *(SAPDB_Int4 *)&memoryPosition;
    return RTESys_AtomicModifyInt4(castedMemoryPosition, deltaValue);
}

/*!
   @brief Atomic modification operation

   The value at the given position is modified by adding the delta value atomically.

       memoryPosition = memoryPostion + deltaValue

   This allows to implement AtomicIncrement/AtomicDecrement

   @param memoryPosition [inout] the memory position to exchange
   @param deltaValue [in] the delta to add
   @return resulting value
 */
inline SAPDB_ULong RTESys_AtomicModify( SAPDB_ULong & memoryPosition
                                     , SAPDB_Long const deltaValue )
{
    SAPDB_UInt4 &castedMemoryPosition = *(SAPDB_UInt4 *)&memoryPosition;
    return RTESys_AtomicModifyUInt4(castedMemoryPosition, deltaValue);
}
#endif /* !BIT64 */

//--------------------------------------------------------------------------------
// RTESys_AtomicOperation
//--------------------------------------------------------------------------------

/*!
   @brief Atomic operation on given memory position

   This functions only make sure the atomicity of the read/modify/write cycle on the given memory
   position. This does not make the functions supplied thread save... If the memory position is modified
   very frequently, this function will loop, but since atomicread() and cmpxchg() are used the actual
   spinning time will be short.

   The value at the given position is retrieved, the function is applied and the result
   is written back if the function returns true.
   
   The function is getting three arguments, old_value, reference to new_value and supplied context.
   
     AtomicAdd(ValueType oldValue, ValueType & newValue, void *context)
     {
         struct AtomicAddContext_ {
             ValueType deltaValue;
         } *atomicAddContext = (struct AtomicAddContext_ *)context;
         newValue = oldValue + atomicAddContext->deltaValue;
         return true;
     }

     AtomicFactorizeWithLimit(ValueType oldValue, ValueType & newValue, void *context)
     {
         struct AtomicFactorizeWithLimitContext_ {
             ValueType factorValue;
             ValueType limitValue;
             ValueType oldValue;
         } *atomicFactorizeWithLimitContext = (struct AtomicFactorizeWithLimitContext_ *)context;

         atomicFactorizeWithLimitContext->oldValue = oldValue;
         newValue = oldValue * atomicFactorizeWithLimitContext->factorValue;
         return ( newValue < atomicFactorizeWithLimitContext->limitValue );
     }

  The function does the modification in the following way:

  void
  RTESys_AtomicOperationValueType( ValueType & memoryPosition,
                                   SAPDB_Bool (*operationFunction)(ValueType const, ValueType &, void *),
                                   void *operationContext )
  {
    ValueType oldValue = RTESys_AtomicRead(memoryPosition);
    do
    {
       ValueType expectedValue = oldValue;
       ValueType newValue;
       if ( !(*operationFunction)(oldValue, newValue, operationContext) )
           return;
    } while ( RTESys_CompareAndExchange(memoryPosition, expectedValue, newValue, oldValue) );
    return;
  }

   @param memoryPosition [inout] the memory position to modify
   @param operationFunction [in] the operation function
   @param operationContext [in] the operation context
   @return none
 */
inline void RTESys_AtomicOperation( SAPDB_Int4 & memoryPosition,
                                    SAPDB_Bool (*operationFunction)(SAPDB_Int4 const, SAPDB_Int4 &, void *),
                                    void *operationContext )
{ RTESys_AtomicOperationInt4(memoryPosition, (RTESys_AtomicOpInt4)operationFunction, operationContext); }
/*! @brief Atomic operation on given memory position
    @param memoryPosition [in] the memory position to modify
    @param operationFunction [in] the operation function
    @param operationContext [in] the operation context
    @return none */
inline void RTESys_AtomicOperation( SAPDB_Int8 & memoryPosition,
                                    SAPDB_Bool (*operationFunction)(SAPDB_Int8 const, SAPDB_Int8 &, void *),
                                    void *operationContext )
{ RTESys_AtomicOperationInt8(memoryPosition, (RTESys_AtomicOpInt8)operationFunction, operationContext); }
/*! @brief Atomic operation on given memory position
    @param memoryPosition [in] the memory position to modify
    @param operationFunction [in] the operation function
    @param operationContext [in] the operation context
    @return none */
inline void RTESys_AtomicOperation( SAPDB_UInt4 & memoryPosition,
                                    SAPDB_Bool (*operationFunction)(SAPDB_UInt4 const, SAPDB_UInt4 &, void *),
                                    void *operationContext )
{ RTESys_AtomicOperationUInt4(memoryPosition, (RTESys_AtomicOpUInt4)operationFunction, operationContext); }
/*! @brief Atomic operation on given memory position
    @param memoryPosition [in] the memory position to modify
    @param operationFunction [in] the operation function
    @param operationContext [in] the operation context
    @return none */
inline void RTESys_AtomicOperation( SAPDB_UInt8 & memoryPosition,
                                    SAPDB_Bool (*operationFunction)(SAPDB_UInt8 const, SAPDB_UInt8 &, void *),
                                    void *operationContext )
{ RTESys_AtomicOperationUInt8(memoryPosition, (RTESys_AtomicOpUInt8)operationFunction, operationContext); }
/*! @brief Atomic operation on given memory position
    @param pointerPosition [in] the memory position to modify pointer
    @param operationFunction [in] the operation function
    @param operationContext [in] the operation context
    @return none */
inline void RTESys_AtomicOperation( void * & pointerPosition,
                                    SAPDB_Bool (*operationFunction)( void * const, void * &, void *),
                                    void *operationContext )
{ RTESys_AtomicOperationPointer(pointerPosition, (RTESys_AtomicOpPointer)operationFunction, operationContext); }

#if !defined(BIT64)

/*! @brief Atomic operation on given memory position
    @param memoryPosition [in] the memory position to modify
    @param operationFunction [in] the operation function
    @param operationContext [in] the operation context
    @return none */
inline void RTESys_AtomicOperation( SAPDB_Long & memoryPosition,
                                    SAPDB_Bool (*operationFunction)(SAPDB_Long const, SAPDB_Long &, void *),
                                    void *operationContext )
{ SAPDB_Int4 &castedMemoryPosition = *(SAPDB_Int4 *)&memoryPosition;
  RTESys_AtomicOperationInt4(castedMemoryPosition, (RTESys_AtomicOpInt4)operationFunction, operationContext); }

/*! @brief Atomic operation on given memory position
    @param memoryPosition [in] the memory position to modify
    @param operationFunction [in] the operation function
    @param operationContext [in] the operation context
    @return none */
inline void RTESys_AtomicOperation( SAPDB_ULong & memoryPosition,
                                    SAPDB_Bool (*operationFunction)(SAPDB_ULong const, SAPDB_ULong &, void *),
                                    void *operationContext )
{ SAPDB_UInt4 &castedMemoryPosition = *(SAPDB_UInt4 *)&memoryPosition;
  RTESys_AtomicOperationUInt4(castedMemoryPosition, (RTESys_AtomicOpUInt4)operationFunction, operationContext); }
#endif /* !BIT64 */

#endif  /* RTESYS_ATOMICOPERATION_HPP */
