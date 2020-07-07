/*!******************************************************************************

  @file         hak01.h

  ------------------------------------------------------------------------------

  @author       <unknown>

  @brief        <unknown topic>

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

#ifndef HAK01_H
#define HAK01_H


/*===========================================================================*
 *     RefType: Both                                                         *
 *===========================================================================*/

/*===========================================================================*
 *     MD5: '\xde\x49\xfe\x31\x41\x80\x2e\xd7\x6a\x9a\x46\x8c\x59\xf0\xfa\x6f'
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

externPascal integer a01char_size;
externPascal integer a01identifier_size;
externPascal tgg00_SysInfoKey a01defaultkey;
externPascal tak_privilege a01emptypriv;
externPascal tak_columnset a01fullset;
externPascal tak_keywordtab a01kw;
externPascal tak_sqlm_charset a01letter_dig_under;
externPascal pasbool a01diag_monitor_on;
externPascal pasbool a01diag_moni_parseid;
externPascal pasbool a01diag_analyze_on;
externPascal pasbool a01diag_ana_coll_data;
externPascal tsp1_part_header a01nil_part_desc;
externPascal tak_queryrewrite_mode a01QueryRewriteMode;
externPascal pasbool a01sm_collect_data;
externPascal tgg00_SysInfoKey a01sysnullkey;
externPascal tak_charset a01digits;
externPascal tak_sqlm_charset a01_letters;
externPascal tak_sqlm_charset a01_first_id_char;
externPascal tsp00_KnlIdentifier a01controluser;
externPascal tsp00_KnlIdentifier a01_i_authorization;
externPascal tsp00_KnlIdentifier a01_i_dbarole;
externPascal tsp00_KnlIdentifier a01_i_catalog;
externPascal tsp00_KnlIdentifier a01_i_count;
externPascal tsp00_KnlIdentifier a01_i_current;
externPascal tsp00_KnlIdentifier a01_i_current_schema;
externPascal tsp00_KnlIdentifier a01_i_cursor;
externPascal tsp00_KnlIdentifier a01_i_errmsg;
externPascal tsp00_KnlIdentifier a01_i_temp;
externPascal tsp00_KnlIdentifier a01_i_domain;
externPascal tsp00_KnlIdentifier a01_i_sys_appl;
externPascal tsp00_KnlIdentifier a01_i_dual;
externPascal tsp00_KnlIdentifier a01_i_java;
externPascal tsp00_KnlIdentifier a01_i_main;
externPascal tsp00_KnlIdentifier a01_i_new;
externPascal tsp00_KnlIdentifier a01_i_old;
externPascal tsp00_KnlIdentifier a01_i_oms_container;
externPascal tsp00_KnlIdentifier a01_i_oms_schema;
externPascal tsp00_KnlIdentifier a01_i_oms_identifier;
externPascal tsp00_KnlIdentifier a01_i_postupgradecheck;
externPascal tsp00_KnlIdentifier a01_i_postupgradechecku;
externPascal tsp00_KnlIdentifier a01_i_rc;
externPascal tsp00_KnlIdentifier a01_i_sysddltrigger;
externPascal tsp00_KnlIdentifier a01_i_syscat1;
externPascal tsp00_KnlIdentifier a01_i_syscat2;
externPascal tsp00_KnlIdentifier a01_i_syschecktablelog;
externPascal tsp00_KnlIdentifier a01_i_sysmessages;
externPascal tsp00_KnlIdentifier a01_i_sysdialoglibrary;
externPascal tsp00_KnlIdentifier a01_i_sysmonitor;
externPascal tsp00_KnlIdentifier a01_i_sysmondata;
externPascal tsp00_KnlIdentifier a01_i_sysparseid;
externPascal tsp00_KnlIdentifier a01_i_syscmd_analyze;
externPascal tsp00_KnlIdentifier a01_i_sysdata_analyze;
externPascal tsp00_KnlIdentifier a01_i_sysproc;
externPascal tsp00_KnlIdentifier a01_i_syscursorname;
externPascal tsp00_KnlIdentifier a01_i_syserrormap;
externPascal tsp00_KnlIdentifier a01_i_sysstatistics;
externPascal tsp00_KnlIdentifier a01_i_sysupdstatlog;
externPascal tsp00_KnlIdentifier a01_i_sysupdstatwanted;
externPascal tsp00_KnlIdentifier a01_i_public;
externPascal tsp00_KnlIdentifier a01_i_queryrewriterules;
externPascal tsp00_KnlIdentifier a01_i_user;
externPascal tsp00_KnlIdentifier a01_i_usergroup;
externPascal tsp00_KnlIdentifier a01_i_sapr3;
externPascal tsp00_KnlIdentifier a01_i_sysdd;
externPascal tsp00_KnlIdentifier a01_i_sys;
externPascal tsp00_KnlIdentifier a01_i_sysinfo;
externPascal tsp00_KnlIdentifier a01_i_system;
externPascal tsp00_KnlIdentifier a01_i_sysdba;
externPascal tsp00_KnlIdentifier a01_i_table;
externPascal tsp00_KnlIdentifier a01_i_tablekey;
externPascal tsp00_KnlIdentifier a01_i_show;
externPascal tsp00_KnlIdentifier a01_i_ak_cache;
externPascal tsp00_KnlIdentifier a01_i_oldpacket;
externPascal tsp00_KnlIdentifier a01_i_expression;
externPascal tsp00_Int2 a01_l_expression;
externPascal tsp00_KnlIdentifier a01_i_rowno;
externPascal tsp00_KnlIdentifier a01_i_rest;
externPascal tsp00_KnlIdentifier a01_i_asterisk;
externPascal tsp00_KnlIdentifier a01_i_internal;
externPascal tsp00_KnlIdentifier a01_i_tmpresult;
externPascal tsp00_Identifier a01_i_b_identifier;
externPascal tsp00_KnlIdentifier a01_il_b_identifier;
externPascal tsp00_KnlIdentifier a01_into_res_name;
externPascal tsp00_KnlIdentifier a01_zero_res_name;
externPascal tsp00_KnlIdentifier a01_union_fill_resname;
externPascal char a01ht;
externPascal char a01lf;
externPascal char a01cr;
externPascal pasbool a01join_clust_read;

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool a01equal_char (
    void                  *  m,
    tsp00_Int4                           pos,
    char                                 cmp_char);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool a01is_whitespace_char (
    void                  *  m,
    tsp00_Int4                           pos);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a01_parser_init ();

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a01info_call_put (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak_procs_Param                      proc,
    tsp00_Int2                           subproc,
    tsp00_Int2            VAR_VALUE_REF  curr_n,
    tsp00_Int2            VAR_VALUE_REF  info_n);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool a01is_ascii_unicode (
    void                  *  m,
    integer                              pos);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool a01is_identifier (
    void                  *  identifier,
    integer                              len,
    tsp00_SqlMode_Param                  sqlmode);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a01_init_command (
    tak_all_command_glob  VAR_VALUE_REF  acv);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a01_force_symbol (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak_sc_symbol_Param                  expected_symbol,
    tsp00_Int2            VAR_VALUE_REF  node1,
    tsp00_Int2            VAR_VALUE_REF  node2);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a01_compact_cmd (
    tak_all_command_glob  VAR_VALUE_REF  acv);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a01_next_symbol (
    tak_all_command_glob  VAR_VALUE_REF  acv);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a01_is_end_symbol (
    tak_all_command_glob  VAR_VALUE_REF  acv);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a01_put_node (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tsp00_Int2            VAR_VALUE_REF  curr_n);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a01_put_same_node (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tsp00_Int2                           input_node,
    tsp00_Int2            VAR_VALUE_REF  curr_n);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a01_function_put (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tsp00_Int2            VAR_VALUE_REF  curr_n);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a01_dt_put_datatype_node (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tsp00_Int2            VAR_VALUE_REF  curr_n,
    tsp00_DataType_Param                 data_type,
    tsp00_Int2                           datalen,
    tsp00_Int2                           datafrac,
    integer                              inoutlen);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a01_call_put (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak_procs_Param                      proc,
    tsp00_Int2                           subproc,
    tsp00_Int2            VAR_VALUE_REF  curr_n);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a01_call_put_queryrewrite (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tsp00_Int2            VAR_VALUE_REF  curr_n);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a01_get_keyword (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    integer               VAR_VALUE_REF  index,
    pasbool               VAR_VALUE_REF  reserved);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a01_write_keywords ();

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a01_is_odbc_syntax (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak_odbc_partype      VAR_VALUE_REF  partype,
    tak_odbc_functiontype  VAR_VALUE_REF  functype);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal tsp00_MoveObjPtr a01node_get_ptr (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    integer                              tree_index);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool a01_odbc_end_ok (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak_odbc_partype_Param                 partype);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal tsp00_Int4 a01aligned_cmd_len (
    tsp00_Int4                           len);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a01ptr_put_node (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tsp00_MoveObjPtr                     ptr,
    tsp00_Int2            VAR_VALUE_REF  curr_n);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a01setl_identifier (
    tsp00_KnlIdentifier   VAR_ARRAY_REF  id,
    tsp00_KnlIdentifier   VAR_ARRAY_REF  set_identifier);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a01sets_identifier (
    tsp00_KnlIdentifier   VAR_ARRAY_REF  id,
    tak_oldidentifier     VAR_ARRAY_REF  set_identifier);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a01set32_identifier (
    tsp00_KnlIdentifier   VAR_ARRAY_REF  id,
    tsp00_Lname           VAR_ARRAY_REF  set_identifier);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal tsp00_Int2 a01swap_int2 (
    tsp00_Int2                           old);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a01swap_cmd_part (
    tsp00_SwapKind_Param                 client_swap,
    tsp1_part_header      VAR_VALUE_REF  cmd_part);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool a01eqkw (
    tak_keyword           VAR_ARRAY_REF  a,
    void                  *  b,
    tsp00_Int4                           b_pos,
    integer                              length);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool a01mandatory_keyword (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    integer                              required_keyword);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool a01_eqkey (
    tak_keyword           VAR_ARRAY_REF  a,
    tsp00_SqlMode_Param                  sqlmode,
    void                  *  b,
    tak_scanner_glob      VAR_VALUE_REF  scv);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a01_get_password (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tsp00_Int2            VAR_VALUE_REF  put_node,
    tsp00_Int2            VAR_VALUE_REF  last_node);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a01_character_set (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tsp00_Int2            VAR_VALUE_REF  put_node,
    tsp00_Int2            VAR_VALUE_REF  last_node);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a01blank_skip_node (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tsp00_Int4                           pos,
    tsp00_Int4                           len);

/* Don't edit this file by hand, it is generated by ptocProto.py */


#endif

