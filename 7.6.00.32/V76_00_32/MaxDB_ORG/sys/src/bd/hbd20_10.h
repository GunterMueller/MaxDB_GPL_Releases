/*!******************************************************************************

  @file         hbd20_10.h

  ------------------------------------------------------------------------------

  @author       <unknown>

  @brief        <unknown topic>

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

********************************************************************************/

#ifndef HBD20_10_H
#define HBD20_10_H


/*===========================================================================*
 *     RefType: C++                                                          *
 *===========================================================================*/

/*===========================================================================*
 *     MD5: '\x45\x9f\xc5\xf4\xb0\x3a\x19\x57\x51\x6d\x40\x9c\x00\x99\x76\x4a'
 *===========================================================================*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externPascal void bd20SystemViewDataCache (
    tsp00_Int4            &  usableSize,
    tsp00_Int4            &  usedSize,
    tsp00_Int4            &  maxUsedSize,
    tsp00_Int4            &  omsDataPageCnt,
    tsp00_Int4            &  omsUnloadedVersPageCnt,
    tsp00_Int4            &  omsHistoryPageCnt,
    tsp00_Int4            &  sqlDataPageCnt,
    tsp00_Int4            &  changedPageCnt,
    tsp00_Int4            &  pendingSyncSvpCnt);

/* Don't edit this file by hand, it is generated by ptocProto.py */


#endif

