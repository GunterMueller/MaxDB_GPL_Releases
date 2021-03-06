/*!******************************************************************************

  @file         hak47.h

  ------------------------------------------------------------------------------

  @author       ThomasA

  @brief        Show_table 

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

#ifndef HAK47_H
#define HAK47_H


/*===========================================================================*
 *     RefType: Both                                                         *
 *===========================================================================*/

/*===========================================================================*
 *     MD5: '\xcb\xb3\xa0\x39\x27\x20\x54\x19\x9b\xf0\xb5\xfc\x71\xcb\x16\x92'
 *===========================================================================*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gak00.h"
#include "vak001.h"
#include "gak40.h"


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externPascal void a47range_default (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak40_show_glob       VAR_VALUE_REF  a41v,
    tak00_columninfo      VAR_VALUE_REF  col_info);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a47show_table (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak40_show_glob       VAR_VALUE_REF  a41v,
    integer                              kw_index);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a47table_kind (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak40_show_glob       VAR_VALUE_REF  a41v,
    tak_baserecord        VAR_VALUE_REF  base_rec);

/* Don't edit this file by hand, it is generated by ptocProto.py */


#endif

