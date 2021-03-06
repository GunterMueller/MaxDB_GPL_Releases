/*!******************************************************************************

  @file         hak93.h

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

#ifndef HAK93_H
#define HAK93_H


/*===========================================================================*
 *     RefType: Both                                                         *
 *===========================================================================*/

/*===========================================================================*
 *     MD5: '\x0e\xa6\x42\x7b\x8f\x02\x24\x32\x9e\x21\x7a\xc7\xdb\x96\xcb\x4c'
 *===========================================================================*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "vsp001.h"
#include "ggg00.h"
#include "vak001.h"


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externPascal void a93_user_commands (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    pasbool               VAR_VALUE_REF  alloc_ok,
    pasbool               VAR_VALUE_REF  do_release);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a93packet_vtrace (
    tgg00_TransContext    VAR_VALUE_REF  t,
    tgg00_VtraceType_Param                 trace_object,
    tsp1_packet_ptr                      packet_ptr);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a93request (
    tak_acv_address                      acv_addr,
    tsp1_packet_ptr                      sql_packet_ptr);

/* Don't edit this file by hand, it is generated by ptocProto.py */


externPascal void a93swap_to_application (
    tsp1_packet_ptr       VAR_VALUE_REF  packet,
    tsp00_CodeType                       a_mess_code,
    tsp00_SwapKind_Param                 a_mess_swap,
    tak_param_list                       a_parameter_spec,
    pasbool                              a_variable_input);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a93swap_from_application (
    tak_all_command_glob  VAR_VALUE_REF  acv);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a93swap_packet_header (
    tsp1_packet_header    VAR_VALUE_REF  packet_header,
    pasbool                              kernel_is_dest,
    tsp00_SwapKind_Param                 kernel_swap,
    tsp00_SwapKind_Param                 host_swap);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a93_implicit_commands (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tsp1_utility_startup_Param                 utility_startup);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a93reply (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_BasisError                     b_err);

/* Don't edit this file by hand, it is generated by ptocProto.py */


#endif

