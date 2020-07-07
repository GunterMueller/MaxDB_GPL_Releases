/*!
  @file           RTESys_AtomicOperation.h
  @author         JoergM
  @ingroup        Runtime
  @brief          Atomic operation (cmpxchg, atomic_increment, atomic_decrement)

  This header files defines a lot of function for atomic read, compare and exchange and atomic modifications.

  For SAPDB_Int4, SAPDB_Int8, SAPDB_UInt4, SAPDB_UInt8 and any pointer individual function are exposed to
  prevent casting operations for C users... 
  
  C++ users should use the extra header file RTESys_AtomicOperation.hpp instead.

  The atomicity of all these calls is possible either if the machine specific implementation has the according
  atomic operations or the memory position is protected with the same spinlock. To decrease the number of 
  collisions, the spinlocks used are contained in a spinlock pool, which may be defined elsewhere...

  NOTE FOR THE FINETUNER: If a platform implementation allows to use a machine specific 'cmpxchg' assembly coding,
  the default increment/decrement functions are therefore implemented using 'cmpxchg' too.
  Otherwise a very severe debugging problem occures :-( Everybody synchronizes, but the spinlock does not protect
  from cmpxchg...

  To handle this, just make sure that first a 'cmpxchg' with implicit memory barrier is found and
  only after that the atomic increment/decrement calls are replaced.

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
#ifndef RTESYS_ATOMICOPERATION_H
#define RTESYS_ATOMICOPERATION_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/RTE_CompilerFeatures.h"
#include "RunTime/RTE_Configure.h" /* dep=RTE_Configure.pl */
#include "SAPDBCommon/SAPDB_Types.h"


#if defined(WIN32) || defined(AIX) || defined(OSF1) || (defined(LINUX) && (defined(I386) ||  defined(IA64) || defined(X86_64))) || defined(SUN) || defined(HP_IA64) || (defined(SOLARIS) && defined(SDB_HAS_SYS_ATOMIC) && defined(BIT64))

/* The coding does not need to simulate compare and exchange with spinlock */
#define RTESYS_HAS_NATIVE_ATOMIC_OPERATION

#else

/* The coding must simulate compare and exchange with spinlock */
#undef  RTESYS_HAS_NATIVE_ATOMIC_OPERATION

#endif

#if !defined(BIT64) && defined(RTESYS_HAS_NATIVE_ATOMIC_OPERATION)

#define RTESYS_HAS_ATOMIC_CAS2_OPERATION

#endif

#if defined(RTESYS_HAS_ATOMIC_CAS2_OPERATION)
/*!
   @brief Atomic CompareAndExchange value for two consecutive SAPDB_ULong

   This structure is used in RTESys_CompareAndExchangeTwoULong() function
 */
struct RTESys_DoubleULongValue
{
    SAPDB_ULong first;  ///< first value
    SAPDB_ULong second; ///< second value
};
#endif

#if defined(__cplusplus)
extern "C" {
#endif

/*===========================================================================*
 *  TYPEDEFS                                                                 *
 *===========================================================================*/
/*! 
   AtomicOperation function receives the oldValue read atomically from position and
   now can return the newValue using the passed operationContext. The operationContext
   may contain additional output values. If the newValue has to written back atomically
   the function must return true. The write back is done with a cmpxchg call, which 
   allows to repeat the function call if the memory position was modified in the mean time.
   Complete Prototype

    SAPDB_Bool RTESys_AtomicOp...(type_ const oldValue, - in
                                  type_ *     newValue, - out 
                                  void  *     context)  - in

   See description of RTESys_AtomicOperationInt4 for an example.
 */
typedef SAPDB_Bool (*RTESys_AtomicOpInt4)(SAPDB_Int4 const, SAPDB_Int4 VAR_VALUE_REF, void *);
/*! AtomicOperation function see above */
typedef SAPDB_Bool (*RTESys_AtomicOpInt8)(SAPDB_Int8 const, SAPDB_Int8 VAR_VALUE_REF, void *);
/*! AtomicOperation function */
typedef SAPDB_Bool (*RTESys_AtomicOpUInt4)(SAPDB_UInt4 const, SAPDB_UInt4 VAR_VALUE_REF, void *);
/*! AtomicOperation function */
typedef SAPDB_Bool (*RTESys_AtomicOpUInt8)(SAPDB_UInt8 const, SAPDB_UInt8 VAR_VALUE_REF, void *);
/*! AtomicOperation function */
typedef SAPDB_Bool (*RTESys_AtomicOpPointer)(void * const, void * VAR_VALUE_REF, void *);

/*=========================================================================================*/
/*! Atomic read values */
/*=========================================================================================*/

/*
  These functions have been introducded to allow consistent atomic access on various types

  With more widespread use of CompareAndExchange and to prevent otherwise needed MemoryBarriers
  the usage was extended to atomic access on shared data!
 */

/*!
   @brief Atomic read operation on 8byte aligned signed value

   @param memoryPosition [inout] the memory position to read
   @return [SAPDB_Int8] the value at the given position
 */
SAPDB_Int8 RTESys_AtomicReadInt8( SAPDB_Int8 VAR_VALUE_REF memoryPosition );
/*!
   @brief same of 8byte aligned unsigned value

   @param memoryPosition [inout] the memory position to read
   @return [SAPDB_UInt8] the value at the given position
 */
SAPDB_UInt8 RTESys_AtomicReadUInt8( SAPDB_UInt8 VAR_VALUE_REF memoryPosition );
/*!
   @brief Atomic read operation on 4byte aligned signed value

   @param memoryPosition [inout] the memory position to read
   @return [SAPDB_Int4] the value at the given position
 */
SAPDB_Int4 RTESys_AtomicReadInt4( SAPDB_Int4 VAR_VALUE_REF memoryPosition );
/*!
   @brief same of 4byte aligned unsigned value

   @param memoryPosition [inout] the memory position to read
   @return [SAPDB_UInt4] the value at the given position
 */
SAPDB_UInt4 RTESys_AtomicReadUInt4( SAPDB_UInt4 VAR_VALUE_REF memoryPosition );
/*!
   @brief Atomic read operation of a pointer

   @param memoryPosition [inout] the memory position to read
   @return [void *] the pointer at the given position
 */
void * RTESys_AtomicReadPointer( void * VAR_VALUE_REF memoryPosition );

/*=========================================================================================*/
/*! Atomic write values */
/*=========================================================================================*/

/*
  These functions have been introducded to allow consistent atomic access on various types

  With more widespread use of CompareAndExchange and to prevent otherwise needed MemoryBarriers
  the usage was extended to atomic access on shared data!
 */

/*!
   @brief Atomic write operation on 8byte aligned signed value

   @param memoryPosition [inout] the memory position to write to
   @param newValue [in] the new value for the given position
   @return none
 */
void RTESys_AtomicWriteInt8( SAPDB_Int8 VAR_VALUE_REF memoryPosition, SAPDB_Int8 const newValue );
/*!
   @brief same of 8byte aligned unsigned value

   @param memoryPosition [inout] the memory position to read
   @param newValue [in] the new value for the given position
   @return none
 */
void RTESys_AtomicWriteUInt8( SAPDB_UInt8 VAR_VALUE_REF memoryPosition, SAPDB_UInt8 const newValue );
/*!
   @brief Atomic write operation on 4byte aligned signed value

   @param memoryPosition [inout] the memory position to write to
   @param newValue [in] the new value for the given position
   @return none
 */
void RTESys_AtomicWriteInt4( SAPDB_Int4 VAR_VALUE_REF memoryPosition, SAPDB_Int4 const newValue );
/*!
   @brief same of 4byte aligned unsigned value

   @param memoryPosition [inout] the memory position to read
   @param newValue [in] the new value for the given position
   @return none
 */
void RTESys_AtomicWriteUInt4( SAPDB_UInt4 VAR_VALUE_REF memoryPosition, SAPDB_UInt4 const newValue );
/*!
   @brief Atomic write operation of a pointer

   @param memoryPosition [inout] the memory position to read
   @param newPointer [in] the new pounter for the given position
   @return none
 */
void RTESys_AtomicWritePointer( void * VAR_VALUE_REF memoryPosition, void * const newPointer );

/*=========================================================================================*/
/*! Compare and Exchange

   NOTE: since normaly some modification have been made on a structure, these calls IMPLICITLY enforces
   memory barriers, so that all updates are visible.
 */
/*=========================================================================================*/

/*!
   @brief Atomic Compare & Exchange operation on 4/8 byte signed/unsigned integer or pointer

   If the old value is no longer matching, the function does not modify the memory
   and returns false. Otherwise the new value is set and true is returned.

   @param memoryPosition [inout] the memory position to exchange
   @param expectedValue [in] the expected old value
   @param newValue [in] the wanted new value
   @param oldValue [out] the compared old value
   @return [SAPDB_Bool] true if newValue replaced oldValue, false if expectedValue was no longer found
 */
SAPDB_Bool RTESys_CmpxchgInt4( SAPDB_Int4 VAR_VALUE_REF memoryPosition,
                               SAPDB_Int4 const         expectedValue,
                               SAPDB_Int4 const         newValue,
                               SAPDB_Int4 VAR_VALUE_REF oldValue );
/*! @brief same for SAPDB_Int8
    @param memoryPosition [inout] the memory position to exchange
    @param expectedValue [in] the expected old value
    @param newValue [in] the wanted new value
    @param oldValue [out] the compared old value
    @return [SAPDB_Bool] true if newValue replaced oldValue, false if expectedValue was no longer found */
SAPDB_Bool RTESys_CmpxchgInt8( SAPDB_Int8 VAR_VALUE_REF memoryPosition,
                               SAPDB_Int8 const         expectedValue,
                               SAPDB_Int8 const         newValue,
                               SAPDB_Int8 VAR_VALUE_REF oldValue );
/*! @brief same for SAPDB_UInt4
    @param memoryPosition [inout] the memory position to exchange
    @param expectedValue [in] the expected old value
    @param newValue [in] the wanted new value
    @param oldValue [out] the compared old value
    @return [SAPDB_Bool] true if newValue replaced oldValue, false if expectedValue was no longer found */
SAPDB_Bool RTESys_CmpxchgUInt4( SAPDB_UInt4 VAR_VALUE_REF memoryPosition,
                                SAPDB_UInt4 const         expectedValue,
                                SAPDB_UInt4 const         newValue,
                                SAPDB_UInt4 VAR_VALUE_REF oldValue );
/*! @brief same for SAPDB_UInt8
    @param memoryPosition [inout] the memory position to exchange
    @param expectedValue [in] the expected old value
    @param newValue [in] the wanted new value
    @param oldValue [out] the compared old value
    @return [SAPDB_Bool] true if newValue replaced oldValue, false if expectedValue was no longer found */
SAPDB_Bool RTESys_CmpxchgUInt8( SAPDB_UInt8 VAR_VALUE_REF memoryPosition,
                                SAPDB_UInt8 const         expectedValue,
                                SAPDB_UInt8 const         newValue,
                                SAPDB_UInt8 VAR_VALUE_REF oldValue );
/*!
   @brief Atomic Compare & Exchange operation on 4/8 byte signed/unsigned integer or pointer

   If the old value is no longer found, the function does not modify the memory
   and returns false. Otherwise the new value is set and true is returned.

   @param pointerPosition [inout] the memory position to exchange pointer
   @param expectedValue [in] the expected old value
   @param newValue [in] the wanted new value
   @param oldValue [out] the compared old value
   @return [SAPDB_Bool] true if newValue replaced oldValue, false if expectedValue was no longer found */
SAPDB_Bool RTESys_CmpxchgPointer( void * VAR_VALUE_REF pointerPosition,
                                  void * const         expectedValue,
                                  void * const         newValue,
                                  void * VAR_VALUE_REF oldValue );

/*=========================================================================================*/
/*! Atomic modification (increment/decrement) */
/*=========================================================================================*/

/*!
   @brief Atomic modification operation on 4/8 byte signed/unsigned integer

   The value at the given position is modified by adding the delta value atomically.

       memoryPosition = memoryPostion + deltaValue

   This allows to implement AtomicIncrement/AtomicDecrement

   @param memoryPosition [inout] the memory position to exchange
   @param deltaValue [in] the delta to add
   @return resulting value
 */
SAPDB_Int4 RTESys_AtomicModifyInt4( SAPDB_Int4 VAR_VALUE_REF memoryPosition,
                              SAPDB_Int4 const         deltaValue );
/*! @brief same for SAPDB_Int8
    @param memoryPosition [inout] the memory position to modify
    @param deltaValue [in] the value to add
    @return resulting value */
SAPDB_Int8 RTESys_AtomicModifyInt8( SAPDB_Int8 VAR_VALUE_REF memoryPosition,
                              SAPDB_Int8 const         deltaValue );
/*! @brief same for SAPDB_UInt4
    @param memoryPosition [inout] the memory position to modify
    @param deltaValue [in] the value to add
    @return resulting value */
SAPDB_UInt4 RTESys_AtomicModifyUInt4( SAPDB_UInt4 VAR_VALUE_REF memoryPosition,
                               SAPDB_Int4 const          deltaValue );
/*! @brief same for SAPDB_UInt8
    @param memoryPosition [inout] the memory position to modify
    @param deltaValue [in] the value to add
    @return resulting value */
SAPDB_UInt8 RTESys_AtomicModifyUInt8( SAPDB_UInt8 VAR_VALUE_REF memoryPosition,
                               SAPDB_Int8 const          deltaValue );
/*!
    @brief Atomic pointer modification 
 
    The delta value is multiplied with the scaling factor. This emulates pointer arithmetic

        SAPDB_Int4 *pInt;
        pInt += 7 
    
    would be replaced by
    
      RTESys_AtomicModifyPointer(pInt, sizeof(SAPDB_Int4), 7);

    @param pointerPosition [inout] the memory position to modify
    @param scaling [in] the size of the pointed object
    @param deltaValue [in] the value to add
    @return resulting pointer
 */
void * RTESys_AtomicModifyPointer( void *       VAR_VALUE_REF pointerPosition,
                                 SAPDB_ULong const          scaling,
                                 SAPDB_Long const           deltaValue );

/*=========================================================================================*/
/*! Atomic modification (generic function) */
/*=========================================================================================*/

/*!
   @brief Atomic operation on given memory position

   This functions only make sure the atomicity of the read/modify/write cycle on the given memory
   position. This does not make the functions supplied thread save... If the memory position is modified
   very frequently, this function will loop, but since atomicread() and cmpxchg() are used the actual
   spinning time will be short.

   The value at the given position is retrieved, the function is applied and the result
   is written back if the function returns true.
   
   The function is getting three arguments, old_value, reference to new_value and supplied context.
   
   Samples are C_Style coding, since functions are extern "C"...
   
     AtomicAdd(ValueType oldValue, ValueType * newValue, void *context)
     {
         struct AtomicAddContext_ {
             ValueType deltaValue;
         } *atomicAddContext = (struct AtomicAddContext_ *)context;
         *newValue = oldValue + atomicAddContext->deltaValue;
         return true;
     }

     AtomicFactorizeWithLimit(ValueType oldValue, ValueType * newValue, void *context)
     {
         struct AtomicFactorizeWithLimitContext_ {
             ValueType factorValue;
             ValueType limitValue;
             ValueType oldValue;
         } *atomicFactorizeWithLimitContext = (struct AtomicFactorizeWithLimitContext_ *)context;

         atomicFactorizeWithLimitContext->oldValue = oldValue;
         *newValue = oldValue * atomicFactorizeWithLimitContext->factorValue;
         return ( *newValue < atomicFactorizeWithLimitContext->limitValue );
     }

  The function does the modification in the following way:

  void
  RTESys_AtomicOperationValueType( ValueType *memoryPosition,
                                   SAPDB_Bool (*operationFunction)(ValueType const, ValueType *, void *),
                                   void *operationContext )
  {
    ValueType oldValue = AtomicRead(memoryPosition);
    do
    {
       ValueType expectedValue = oldValue;
       ValueType newValue;
       if ( !(*operationFunction)(oldValue, &newValue, operationContext) )
           return;
    } while ( CompareAndExchange(memoryPosition, expectedValue, newValue, &oldValue) );
    return;
  }

   @param memoryPosition [inout] the memory position to modify
   @param operationFunction [in] the operation function
   @param operationContext [in] the operation context
   @return none
 */
void RTESys_AtomicOperationInt4( SAPDB_Int4 VAR_VALUE_REF memoryPosition,
                                 RTESys_AtomicOpInt4 operationFunction,
                                 void *operationContext );

/*! @brief same for SAPDB_Int8
    @param memoryPosition [inout] the memory position to modify
    @param operationFunction [in] the operation function
    @param operationContext [in] the operation context
    @return none */
void RTESys_AtomicOperationInt8( SAPDB_Int8 VAR_VALUE_REF memoryPosition,
                                 RTESys_AtomicOpInt8 operationFunction,
                                 void *operationContext );

/*! @brief same for SAPDB_UInt4
    @param memoryPosition [inout] the memory position to modify
    @param operationFunction [in] the operation function
    @param operationContext [in] the operation context
    @return none */
void RTESys_AtomicOperationUInt4( SAPDB_UInt4 VAR_VALUE_REF memoryPosition,
                                  RTESys_AtomicOpUInt4 operationFunction,
                                  void *operationContext );

/*! @brief same for SAPDB_UInt8
    @param memoryPosition [inout] the memory position to modify
    @param operationFunction [in] the operation function
    @param operationContext [in] the operation context
    @return none */
void RTESys_AtomicOperationUInt8( SAPDB_UInt8 VAR_VALUE_REF memoryPosition,
                                  RTESys_AtomicOpUInt8 operationFunction,
                                  void *operationContext );
/*! @brief same for void pointer
    @param pointerPosition [inout] the memory position to modify pointer
    @param operationFunction [in] the operation function
    @param operationContext [in] the operation context
    @return none */
void RTESys_AtomicOperationPointer( void * VAR_VALUE_REF pointerPosition,
                                    RTESys_AtomicOpPointer operationFunction,
                                    void *operationContext );
#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif  /* RTESYS_ATOMICOPERATION_H */
