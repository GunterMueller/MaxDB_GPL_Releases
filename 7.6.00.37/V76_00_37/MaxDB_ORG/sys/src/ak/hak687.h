/*!******************************************************************************

  @file         hak687.h

  ------------------------------------------------------------------------------

  @author       GertG

  @brief        Join_aux_functions

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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

#ifndef HAK687_H
#define HAK687_H


/*===========================================================================*
 *     RefType: Both                                                         *
 *===========================================================================*/

/*===========================================================================*
 *     MD5: '\x99\x03\x8e\xa7\xb0\x9e\xe5\xce\x0f\x29\xfc\x99\xc0\x00\x58\x0f'
 *===========================================================================*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "ggg00.h"


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externPascal void a687_check_memory_usage (
    tgg00_TransContext    VAR_VALUE_REF  TransContext,
    tsp00_Int4                           global_added,
    tsp00_Longuint VAR_VALUE_REF                 usedbytes1);

/* Don't edit this file by hand, it is generated by ptocProto.py */


#endif

