/*!******************************************************************************

  @file         hak686.h

  ------------------------------------------------------------------------------

  @author       GertG

  @brief        Join_execution

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

#ifndef HAK686_H
#define HAK686_H


/*===========================================================================*
 *     RefType: Both                                                         *
 *===========================================================================*/

/*===========================================================================*
 *     MD5: '\x91\x21\xae\xdb\x70\x5f\x5d\xb0\xb8\xc4\xc0\xfc\xad\xd9\xec\x30'
 *===========================================================================*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "ggg00.h"
#include "vak001.h"
#include "gak68.h"


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externPascal void a686_create_join_result (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak_dml_info          VAR_VALUE_REF  dmli,
    const tak68_sequence  VAR_ARRAY_REF  sequence,
    const tak_parskey     VAR_VALUE_REF  parsk,
    const tak68_joinview_rec  VAR_VALUE_REF  jvrec,
    const pasbool                        use_old_rescnt,
    const pasbool                        del_parsinfos,
    tgg00_BasisError      VAR_VALUE_REF  b_err);

/* Don't edit this file by hand, it is generated by ptocProto.py */


#endif

