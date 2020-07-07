/*!******************************************************************************

  @file         hkb52_1.h

  ------------------------------------------------------------------------------

  @author       UweH

  @brief        KB_show_lock

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

#ifndef HKB52_1_H
#define HKB52_1_H


/*===========================================================================*
 *     RefType: Both                                                         *
 *===========================================================================*/

/*===========================================================================*
 *     MD5: '\xf4\xad\x71\x0c\x58\x8d\x8b\x82\x83\x55\xf5\xb8\x65\x12\xe6\xd6'
 *===========================================================================*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "ggg00.h"


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externPascal void k52dump_locklist (
    tsp00_TaskId                         TaskId,
    pasbool                              EnterRegions,
    tgg00_VfFileref       VAR_VALUE_REF  HostFile,
    tsp00_Page            VAR_ARRAY_REF  buf,
    tsp00_Int4            VAR_VALUE_REF  OutPno,
    integer               VAR_VALUE_REF  OutPos,
    tsp00_VfReturn        VAR_VALUE_REF  HostErr,
    tsp00_ErrText         VAR_ARRAY_REF  ErrText);

/* Don't edit this file by hand, it is generated by ptocProto.py */


#endif

