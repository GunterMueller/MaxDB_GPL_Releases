/*!******************************************************************************

  @file         hgg20.h

  ------------------------------------------------------------------------------

  @author       HolgerB

  @brief        Unicode-Utilities

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

#ifndef HGG20_H
#define HGG20_H


/*===========================================================================*
 *     RefType: Both                                                         *
 *===========================================================================*/

/*===========================================================================*
 *     MD5: '\x98\x2b\x64\x2f\xc0\xe8\x50\xa2\x34\x9a\x64\x8d\xcc\xa0\x90\x07'
 *===========================================================================*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "vsp008.h"


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externPascal void g20get_uni_key (
    tsp00_KnlIdentifier   VAR_ARRAY_REF  setname,
    tsp00_CodeType        VAR_VALUE_REF  messcode,
    tsp00_Uint1           VAR_VALUE_REF  codewidth,
    tsp8_uni_error        VAR_VALUE_REF  rc);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void g20get_uni_set (
    tsp00_CodeType                       messcode,
    tsp00_Uint1           VAR_VALUE_REF  codewidth,
    tsp00_KnlIdentifier   VAR_ARRAY_REF  setname);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void g20unifill (
    tsp00_Int4                           size,
    void*                                m,
    tsp00_Int4                           pos,
    tsp00_Int4                           len,
    const char*                          filluchar);

/* Don't edit this file by hand, it is generated by ptocProto.py */


#endif
