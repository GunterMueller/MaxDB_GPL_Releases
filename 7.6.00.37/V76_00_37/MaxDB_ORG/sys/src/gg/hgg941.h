/*!******************************************************************************

  @file         hgg941.h

  ------------------------------------------------------------------------------

  @author       UweH

  @brief        GG_allocator_interface

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

********************************************************************************/

#ifndef HGG941_H
#define HGG941_H


/*===========================================================================*
 *     RefType: Both                                                         *
 *===========================================================================*/

/*===========================================================================*
 *     MD5: '\x8f\x71\x6f\x34\xe9\x63\x1c\x01\x99\xd5\x43\x13\xcc\xe9\x1d\x60'
 *===========================================================================*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "ggg00.h"


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externPascal tsp00_Addr gg941Allocate (
    tgg00_TransContext    VAR_VALUE_REF  TransContext,
    integer                              wantedBytes);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal tsp00_Int4 gg941AllocatedByChunkProperty (
    tgg00_TransContext    VAR_VALUE_REF  TransContext,
    integer                              property);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal tsp00_Addr gg941ReAllocate (
    tgg00_TransContext    VAR_VALUE_REF  TransContext,
    integer                              wantedBytes,
    tsp00_Addr                           hint);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool gg941CheckConsistency (
    tgg00_TransContext    VAR_VALUE_REF  TransContext);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void gg941Deallocate (
    tgg00_TransContext    VAR_VALUE_REF  TransContext,
    tsp00_Addr            VAR_VALUE_REF  p);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void gg941CalcStatistics (
    tgg00_TransContext    VAR_VALUE_REF  TransContext,
    tsp00_Longuint VAR_VALUE_REF                 usedbytes,
    tsp00_Longuint VAR_VALUE_REF                 maxusedbytes,
    tsp00_Longuint VAR_VALUE_REF                 ctrlbytes);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void gg941SetChunkProperty (
    tsp00_Addr                           pChunk,
    integer                              property);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void Kernel_CheckSwitch (
    const char*                          TopicStr,
    integer                              TopicStrLen);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void Kernel_TraceSwitch (
    const char*                          TopicStr,
    integer                              TopicStrLen);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void Kernel_InitTrace ();

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool Kernel_DiagInfop (
    integer                              requested,
    tsp00_Identifier *                   kindOut,
    tsp00_Sname *                        topicKeyOut,
    integer               VAR_VALUE_REF  levelOut);

/* Don't edit this file by hand, it is generated by ptocProto.py */


#endif

