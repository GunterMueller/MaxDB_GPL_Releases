  /*!***************************************************************************

  module:       SAPDB_BitOperations.hpp

  ------------------------------------------------------------------------------

  responsible:  TillL

  special area: SAPDB Common

  description:  Collection of useful inline functions which operate on bits

  see also:     

  ------------------------------------------------------------------------------

  copyright:            Copyright (c) 2000-2005 SAP AG




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


#ifndef SAPDB_BITOPERATIONS_HPP
#define SAPDB_BITOPERATIONS_HPP


#include "SAPDBCommon/SAPDB_Types.hpp"


/*!-------------------------------------------------------------------------
function:     SAPDB_ReplaceBitRasterValue()
description:  Replaces the contents of a continuous bitsequence in <input>
              and returns the result.

              - <class T> is any unsigned integer type (Byte, UInt2, UInt4, UInt8)
              - The bits represented by <raster> are contiguous
              - The contents of these bits in <input> is replaced by <value>
              - Example (Byte):
                input            = 001 110 01

                raster           = 00000 111, bits must be aligned to the right
                value            = 00000 100
                shift            = 2          

                raster  << shift = 000 111 00, i.e., the part to be replaced in input is "110"
                value   << shift = 000 100 00

                result           = 001 100 01

arguments:    input  [in]  source value
              raster [in]  sequence of n '1'-s aligned to the right (== bit raster)
              value  [in]  sequence of n bits aligned to the right
              shift  [in]  count of bits by which <raster> and <value> are to be shifted 
                           to the left 
return value: (T) updated value
--------------------------------------------------------------------------*/
template <class T>
inline T SAPDB_ReplaceBitRasterValue(T input, T raster, SAPDB_UInt shift, T value)
{
    return (input & ~(raster << shift)) + ((value & raster) << shift);
}


/*!-------------------------------------------------------------------------
function:     SAPDB_GetBitRasterValue()
description:  Returns the value of a continuous bitsequence in <input> value

              - <class T> is any unsigned integer type (Byte, UInt2, UInt4, UInt8)
              - The bits represented by <raster> are contiguous
              - The value of these bits in <input> is returned
              - Example (Byte):
                input            = 001 110 01

                raster           = 00000 111, bits must be aligned to the right
                shift            = 2
                raster  << shift = 000 111 00, i.e., the part to be returned from input is "110"

                result           = 00000 110

arguments:    input  [in]  source value
              raster [in]  sequence of n '1'-s aligned to the right (== bit raster)
              shift  [in]  count of bits by which <raster> is to be shifted 
                           to the left 
return value: (T) value of the bits masked by raster 
--------------------------------------------------------------------------*/
template <class T>
inline T SAPDB_GetBitRasterValue(T input, T raster, SAPDB_UInt shift)
{
    return (input & (raster << shift)) >> shift;
}


/*!-------------------------------------------------------------------------
function:     SAPDB_Div8()
description:  Performs integer division by 8 on input value
              <T> is any unsigned integer type (Byte, UInt2, UInt4, UInt8)
arguments:    input [in]    input value
return value: (T) result of division
--------------------------------------------------------------------------*/
template <class T>
inline T SAPDB_Div8(T input)
{
    return input >> 3;
}

/*!-------------------------------------------------------------------------
function:     SAPDB_Mod8()
description:  Calculates input modulo 8
              <T> is any unsigned integer type (Byte, UInt2, UInt4, UInt8)
arguments:    input [in]    input value
return value: (SAPDB_Byte) result of modulo
--------------------------------------------------------------------------*/
template <class T>
inline SAPDB_Byte SAPDB_Mod8(T input)
{
    return static_cast<SAPDB_Byte>(input & static_cast<T>(0x07));
}




#endif // SAPDB_BITOPERATIONS_HPP
