/*!
    @file     SAPDBAlgo_FNVHash.h
    @ingroup  SAPDBAlgo
    @author   DirkT
    @brief    C++ Implementation of the 32 bit FNV hash algorithm
    @see      http://www.isthe.com/chongo/tech/comp/fnv/

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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

#ifndef SAPDBALGO_FNVHASH_H
#define SAPDBALGO_FNVHASH_H

#include "SAPDBCommon/SAPDB_Types.hpp"

/// Initial value for 32bit hash hash generation
#define FNV_32_INIT ((SAPDB_UInt4)0x811c9dc5)
/// Prime number to be used for 32bit hash generation
#define FNV_32_PRIME ((SAPDB_UInt4)0x01000193)

/*!
    @brief  Calculates a 32bit hash value of a given memory segment
    @param  Buf  [in] Pointer to the memory buffer
    @param  Len  [in] Length of the memory buffer
    @param  HVal [in] Initial hashvalue
*/
inline void FNV32Hash(const void* Buf, size_t Len, SAPDB_UInt4& HVal)
{
    unsigned char* bp = (unsigned char *) Buf;  /* start of buffer      */
    unsigned char* be = bp + Len;               /* beyond end of buffer */

    while ( bp < be )
    {
	    // Calculate HVal *= FNV_32_PRIME; by bit shifting and addition
    	HVal += (HVal<<1) + (HVal<<4) + (HVal<<7) + (HVal<<8) + (HVal<<24);
	    HVal ^= (SAPDB_UInt4)*bp++;
    }
}

/*!
    @brief  Calculates a 32bit hash value of a given memory segment; slightly different algorithm than FNV32Hash
    @param  Buf  [in] Pointer to the memory buffer
    @param  Len  [in] Length of the memory buffer
    @param  HVal [in] Initial hashvalue
*/
inline void FNV32Hash2(const void* Buf, size_t Len, SAPDB_UInt4& HVal)
{
    unsigned char* bp = (unsigned char *) Buf;  /* start of buffer      */
    unsigned char* be = bp + Len;               /* beyond end of buffer */

    while ( bp < be )
    {
	    HVal ^= (SAPDB_UInt4)*bp++;
	    // Calculate HVal *= FNV_32_PRIME; by bit shifting and addition
    	HVal += (HVal<<1) + (HVal<<4) + (HVal<<7) + (HVal<<8) + (HVal<<24);
    }
}

#endif