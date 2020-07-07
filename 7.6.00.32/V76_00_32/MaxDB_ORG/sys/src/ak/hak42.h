/*!******************************************************************************

  @file         hak42.h

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

#ifndef HAK42_H
#define HAK42_H


/*===========================================================================*
 *     RefType: Both                                                         *
 *===========================================================================*/

/*===========================================================================*
 *     MD5: '\x13\x23\x7b\x5f\x67\x10\xe4\x19\xf8\xc0\x31\x8f\xd6\xd8\x5c\x35'
 *===========================================================================*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "ggg00.h"
#include "vak001.h"
#include "gak40.h"


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/


#if defined (SAPDB_SLOW)

externPascal pasbool a42check_time;

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a42_init_time_array ();

/* Don't edit this file by hand, it is generated by ptocProto.py */


#endif

externPascal void a42_check_database (
    tak_all_command_glob  VAR_VALUE_REF  acv);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a42caches_statistics (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak40_show_glob       VAR_VALUE_REF  a41v);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a42_get_pagecount (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak_sysbufferaddress                 base_ptr,
    tsp00_Int4            VAR_VALUE_REF  pagecount);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a42execute (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak40_show_glob       VAR_VALUE_REF  a41v);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a42get_tablename (
    tgg00_TransContext    VAR_VALUE_REF  t,
    tgg00_Surrogate       VAR_ARRAY_REF  tabid,
    tsp00_KnlIdentifier   VAR_ARRAY_REF  authid,
    tsp00_KnlIdentifier   VAR_ARRAY_REF  tablen,
    tgg00_BasisError      VAR_VALUE_REF  b_err);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a42index_inf_to_messbuf (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_MessBlock       VAR_VALUE_REF  mblock,
    tak40_show_glob       VAR_VALUE_REF  a41v,
    tsp00_KnlIdentifier   VAR_ARRAY_REF  indexn,
    pasbool               VAR_VALUE_REF  selectivity,
    pasbool                              diagnose_index);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a42lock_req_mode (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak40_show_glob       VAR_VALUE_REF  a41v,
    tgg00_LockReqMode_Param                 lock_mode);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a42lock_req_state (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak40_show_glob       VAR_VALUE_REF  a41v,
    tgg00_LockReqState    VAR_ARRAY_REF  lock_state);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a42_monitor_init (
    tsp00_TaskId                         process_id);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a42move_trans_no (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak40_show_glob       VAR_VALUE_REF  a41v,
    tgg91_TransNo         VAR_VALUE_REF  trans);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a42name_and_val_statistic (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak40_show_glob       VAR_VALUE_REF  a41v,
    tak_keyword           VAR_ARRAY_REF  vkw,
    tsp00_Int4                           value,
    tsp00_Sname           VAR_ARRAY_REF  nam);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a42new_lock_req_mode (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak40_show_glob       VAR_VALUE_REF  a41v,
    tgg00_LockReqMode_Param                 lock_mode);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a42new_lock_req_state (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak40_show_glob       VAR_VALUE_REF  a41v,
    tgg00_LockReqState    VAR_ARRAY_REF  lock_state);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a42_start_semantic (
    tak_all_command_glob  VAR_VALUE_REF  acv);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a42table_statistics (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak40_show_glob       VAR_VALUE_REF  a41v,
    pasbool                              pages_only);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a42iindex_statistics (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak40_show_glob       VAR_VALUE_REF  a41v);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a42put_running_cmds (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tsp00_C12             VAR_ARRAY_REF  parsid);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool a42getparseid (
    tgg00_TransContext    VAR_VALUE_REF  t,
    tsp00_TaskId          VAR_VALUE_REF  pid,
    tsp00_C12             VAR_ARRAY_REF  parsid,
    tgg00_BasisError      VAR_VALUE_REF  b_err);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a42reset_running_cmds (
    tak_all_command_glob  VAR_VALUE_REF  acv);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a42bool_to_intern_state (
    tak_acv_address                      acv_ptr,
    tak40_show_glob_ptr                  a41v_ptr,
    tsp00_C20             VAR_ARRAY_REF  component,
    tsp00_C40             VAR_ARRAY_REF  description,
    pasbool                              bool_val);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a42date_to_intern_state (
    tak_acv_address                      acv_ptr,
    tak40_show_glob_ptr                  a41v_ptr,
    tsp00_C20             VAR_ARRAY_REF  component,
    tsp00_C40             VAR_ARRAY_REF  description,
    tsp00_Int4                           int_date);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a42int4_to_intern_state (
    tak_acv_address                      acv_ptr,
    tak40_show_glob_ptr                  a41v_ptr,
    tsp00_C20             VAR_ARRAY_REF  component,
    tsp00_C40             VAR_ARRAY_REF  description,
    tsp00_Int4                           int_val);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a42session_to_intern_state (
    tak_acv_address                      acv_ptr,
    tak40_show_glob_ptr                  a41v_ptr,
    tsp00_C20             VAR_ARRAY_REF  component,
    tsp00_C40             VAR_ARRAY_REF  description,
    tgg91_SessionNo                      session);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a42str_to_intern_state (
    tak_acv_address                      acv_ptr,
    tak40_show_glob_ptr                  a41v_ptr,
    tsp00_C20             VAR_ARRAY_REF  component,
    tsp00_C40             VAR_ARRAY_REF  description,
    tsp00_MoveObjPtr                     str_ptr,
    integer                              str_len);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a42surrogate_to_intern_state (
    tak_acv_address                      acv_ptr,
    tak40_show_glob_ptr                  a41v_ptr,
    tsp00_C20             VAR_ARRAY_REF  component,
    tsp00_C40             VAR_ARRAY_REF  description,
    tgg00_Surrogate       VAR_ARRAY_REF  surrogate);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a42time_to_intern_state (
    tak_acv_address                      acv_ptr,
    tak40_show_glob_ptr                  a41v_ptr,
    tsp00_C20             VAR_ARRAY_REF  component,
    tsp00_C40             VAR_ARRAY_REF  description,
    tsp00_Int4                           int_time);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a42trans_to_intern_state (
    tak_acv_address                      acv_ptr,
    tak40_show_glob_ptr                  a41v_ptr,
    tsp00_C20             VAR_ARRAY_REF  component,
    tsp00_C40             VAR_ARRAY_REF  description,
    tgg91_TransNo                        trans);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a42uint1_to_intern_state (
    tak_acv_address                      acv_ptr,
    tak40_show_glob_ptr                  a41v_ptr,
    tsp00_C20             VAR_ARRAY_REF  component,
    tsp00_C40             VAR_ARRAY_REF  description,
    tsp00_Int2                           int_val);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a42uint2_to_intern_state (
    tak_acv_address                      acv_ptr,
    tak40_show_glob_ptr                  a41v_ptr,
    tsp00_C20             VAR_ARRAY_REF  component,
    tsp00_C40             VAR_ARRAY_REF  description,
    tsp00_Int4                           int_val);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a42uint4_to_intern_state (
    tak_acv_address                      acv_ptr,
    tak40_show_glob_ptr                  a41v_ptr,
    tsp00_C20             VAR_ARRAY_REF  component,
    tsp00_C40             VAR_ARRAY_REF  description,
    tsp00_8ByteCounter                   int_val);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a42UserStatisticsCallback (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak40_show_glob       VAR_VALUE_REF  a41v,
    tak_sysbufferaddress                 pUser);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a42ResetOmsMonitor (
    tak_all_command_glob  VAR_VALUE_REF  acv);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a42ref_stat_real (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak40_show_glob       VAR_VALUE_REF  a41v,
    tsp00_C40             VAR_ARRAY_REF  textpar,
    tsp00_Longreal                       lreal);

/* Don't edit this file by hand, it is generated by ptocProto.py */


#endif
