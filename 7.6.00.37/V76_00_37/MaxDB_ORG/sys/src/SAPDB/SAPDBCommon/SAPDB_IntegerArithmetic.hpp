/*!***************************************************************************
  module:       SAPDB_IntegerArithmetic.hpp

  responsible:  StefanB

  special area: Kernel

  description:  Some functions for integer arithmetic. 

  copyright:    Copyright (c) 2000-2005 SAP AG

  created:      Tue May 09 17:10:00  2000

  updated:      Tue May 09 17:10:00  2000



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




*****************************************************************************/

#ifndef SAPDB_INTEGERARITHMETIC_HPP
#define SAPDB_INTEGERARITHMETIC_HPP

#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"


/*!***************************************************************************
function: SAPDB_Log2Floor
description: Largest integer not larger than log2 of argument. 
*****************************************************************************/

template <class UIntType>
UIntType SAPDB_Log2Floor(UIntType a)
{
   SAPDBERR_ASSERT_ARGUMENT(a > 0);

   UIntType b = 2;
   UIntType log2Floor = 1;
   while (b <= a) {
      b = b << 1;
      ++ log2Floor;
   };
   return (log2Floor - 1);
}

/*!***************************************************************************
function: SAPDB_Log2Ceil
description: Least integer not less than log2 of argument. 
*****************************************************************************/

template <class UIntType>
UIntType SAPDB_Log2Ceil(UIntType a)
{
   SAPDBERR_ASSERT_ARGUMENT(a > 0);

   UIntType b = 1;
   UIntType log2Ceil = 0;
   while (b < a) {
      b = b << 1;
      ++ log2Ceil;
   };
   return log2Ceil;
}

/*!***************************************************************************
function: SAPDB_MaxValue
description: Maximum value that can be held in the integer type. 
*****************************************************************************/

template <class IntType>
IntType SAPDB_MaxValue(IntType)
{
   IntType x = 127;
   while ((x << 1) > x) {
       x <<= 1;
       x += 1;
   }
   return x;

/*
    // Bug: The algorithm below does not work with signed types!
   IntType x = ~(static_cast<IntType>(0));
   // If <IntType> is signed, remove sign bit
   if (x < 0) {
      x = static_cast<IntType>(static_cast<SAPDB_UInt8>(x) >> 1);
   }
   return x;
*/
}

/*!***************************************************************************
function: SAPDB_BitCount
description: Number of bits of the integer type. 
*****************************************************************************/

template <class UIntType>
UIntType SAPDB_BitCount(UIntType)
{
   UIntType bitCount = 0;
   UIntType m = SAPDB_MaxValue(m);
   while (m != 0) {
      m <<= 1;
      ++ bitCount;
   }
   return ( bitCount );
}

/*!***************************************************************************
function: SAPDB_DivideCeil
description: Integer division rounded up.
*****************************************************************************/

template <class UIntType>
UIntType SAPDB_DivideCeil(UIntType x, UIntType y)
{
   SAPDBERR_ASSERT_ARGUMENT(x > 0);
   SAPDBERR_ASSERT_ARGUMENT(y > 0);

   return (( x + y - 1) / y);
}

/*!***************************************************************************
function: SAPDB_RoundUp
description: Minimum multiple of the factor <tt>f</tt> that is not less than the 
             argument <tt>x</tt>. 
*****************************************************************************/

template <class UIntType>
inline UIntType SAPDB_RoundUp(UIntType x, UIntType f)
{
   SAPDBERR_ASSERT_ARGUMENT(x > 0);
   SAPDBERR_ASSERT_ARGUMENT(f > 0);

   return (( x + f -1) / f) * f;
}

/*!***************************************************************************
function: SAPDB_RoundDown
description: Maximum multiple of the factor <tt>f</tt> that is not larger 
             than the argument <tt>x</tt>.
*****************************************************************************/

template <class UIntType>
inline UIntType SAPDB_RoundDown(UIntType x, UIntType f)
{
   SAPDBERR_ASSERT_ARGUMENT(x > 0);
   SAPDBERR_ASSERT_ARGUMENT(f > 0);

   return (x / f) * f;
}

/*!***************************************************************************
function: SAPDB_ComplementOfModulus
description: The complement of the modulus. 
     ´       The complement of X modulo Y is ( (Y - X % Y) % Y ).
*****************************************************************************/

/* Verstehe ich nicht

template <class UIntType>
inline UIntType SAPDB_ComplementOfModulus(UIntType x, UIntType y)
{
   SAPDBERR_ASSERT_ARGUMENT(x > 0);
   SAPDBERR_ASSERT_ARGUMENT(y > 0);

   return (( x % y == 0 ) ? 0 : ( y - x % y ));
}
*/  


#endif // SAPDB_INTEGERARITHMETIC_HPP

