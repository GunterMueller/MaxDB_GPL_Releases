/*!******************************************************************************

  @file         hak36.h

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

#ifndef HAK36_H
#define HAK36_H


/*===========================================================================*
 *     RefType: Both                                                         *
 *===========================================================================*/

/*===========================================================================*
 *     MD5: '\x74\x2d\x0c\x51\x54\x6d\xfb\x49\x97\x85\xc7\x55\xcd\x31\xb5\xf4'
 *===========================================================================*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "ggg00.h"
#include "vak001.h"


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externPascal void a36after_systable_load (
    tak_all_command_glob  VAR_VALUE_REF  acv);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a36diag_outcopy (
    tak_all_command_glob  VAR_VALUE_REF  acv);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a36dg_tablename_get (
    tgg00_TransContext    VAR_VALUE_REF  t,
    tgg00_Surrogate       VAR_ARRAY_REF  tabid,
    tgg00_Surrogate       VAR_ARRAY_REF  owner_id,
    tsp00_KnlIdentifier   VAR_ARRAY_REF  tablename);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a36dg_username_get (
    tgg00_TransContext    VAR_VALUE_REF  t,
    tgg00_Surrogate       VAR_ARRAY_REF  user_id,
    tsp00_KnlIdentifier   VAR_ARRAY_REF  username);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a36get_catalog_record (
    tgg00_TransContext    VAR_VALUE_REF  t,
    tgg00_SysInfoKey      VAR_VALUE_REF  sysk,
    tak_systembuffer      VAR_VALUE_REF  sysbuf);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a36hex_surrogate (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_Surrogate       VAR_ARRAY_REF  surrogate);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a36restart_time (
    tsp00_Int4            VAR_VALUE_REF  date,
    tsp00_Int4            VAR_VALUE_REF  time);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool a36tree_in_memory (
    tgg00_FileId          VAR_VALUE_REF  tree_id);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a36_call_semantic (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_UtilCmdId       VAR_VALUE_REF  util_cmd_id);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a36devname (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    integer                              tree_index,
    tsp00_VFilename       VAR_ARRAY_REF  dname,
    integer                              dlen);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a36filename (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    integer                              tree_index,
    tsp00_VFilename       VAR_ARRAY_REF  fname,
    integer                              fnlen);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a36utilprot_needed (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    pasbool               VAR_VALUE_REF  prot_needed,
    pasbool               VAR_VALUE_REF  with_tapeinfo);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool a36filetype (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tsp00_Int2                           tree_index,
    tsp00_VfType          VAR_VALUE_REF  filetype);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a36_standby_semantic (
    tak_all_command_glob  VAR_VALUE_REF  acv);

/* Don't edit this file by hand, it is generated by ptocProto.py */


#endif

