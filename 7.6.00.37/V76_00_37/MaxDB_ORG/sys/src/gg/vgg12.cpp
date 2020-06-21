/****************************************************************************

  module      : vgg12.c

  -------------------------------------------------------------------------

  responsible : GertG

  special area: 
  description : SET implementation with allocator


  last changed: 2004-01-18  12:09
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2004-2005 SAP AG



    ========== licence begin  GPL
    Copyright (c) 2004-2005 SAP AG

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



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Streaming.hpp"
#include "gsp00.h"
#include "gsp00_2.h"
#include "ggg00.h"
#include "hgg941.h"
#include "hgg12.h"

extern SAPDBTrace_Topic AllocatorTrace;

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/



/*===========================================================================*
 *  PUBLIC FUNCTIONS (CODE)                                                  *
 *===========================================================================*/


externPascal tsp00_Addr gg12InitBitArray(
       tgg00_TransContext &TransContext, 
       tsp00_Int4         bits)
{

    int _byte_cnt = (bits + SAPDB_BITS_PER_BYTE - 1) / SAPDB_BITS_PER_BYTE * sizeof(tsp00_Uint1);
    tsp00_Addr set = 0;
    set = gg941Allocate( TransContext, _byte_cnt );
    SAPDBTRACE_WRITELN( AllocatorTrace, 1, "alloc BitArray: " << (void*)set );
    if ( set ) memset( set, 0, _byte_cnt );
    return set;
}

/*--------------------------------------------------------------------------*/

externPascal pasbool gg12GetBit( 
       tsp00_Addr set, 
       tsp00_Int4 number)
{
    set += number / SAPDB_BITS_PER_BYTE;
    return (*set & (1 << (number % SAPDB_BITS_PER_BYTE))) != 0;    /* 0 or 1   */
}

/*--------------------------------------------------------------------------*/

externPascal void gg12SetBit( 
        tsp00_Addr set, 
        tsp00_Int4 number, 
        pasbool value)
{
    set += number / SAPDB_BITS_PER_BYTE;
    if (value)
        *set |= 1 << (number % SAPDB_BITS_PER_BYTE);    /* set bit  */
    else    
        *set &= ~(1 << (number % SAPDB_BITS_PER_BYTE)); /* clear bit*/
}

/*--------------------------------------------------------------------------*/

externPascal void gg12FlipBit(
        tsp00_Addr set, 
        tsp00_Int4 number)
{
    set += number / SAPDB_BITS_PER_BYTE;
    *set ^= 1 << (number % SAPDB_BITS_PER_BYTE); /* flip bit */
}

/*--------------------------------------------------------------------------*/

externPascal void gg12ClearBitArray( 
        tsp00_Addr set, 
        tsp00_Int4 bits )
{
    memset( set, 0, (bits + SAPDB_BITS_PER_BYTE - 1) / SAPDB_BITS_PER_BYTE * sizeof(tsp00_Uint1) );
}

/*--------------------------------------------------------------------------*/

externPascal void gg12FinalizeBitArray(
       tgg00_TransContext &TransContext, 
       tsp00_Addr         &set)
{
    SAPDBTRACE_WRITELN( AllocatorTrace, 1, " dealloc BitArray: " << (void*)set );
    gg941Deallocate( TransContext, set );
}

