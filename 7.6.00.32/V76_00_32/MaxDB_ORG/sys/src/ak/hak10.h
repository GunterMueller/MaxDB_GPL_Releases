/*!******************************************************************************

  @file         hak10.h

  ------------------------------------------------------------------------------

  @author       ThomasA

  @brief        Systeminfo_cache

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

#ifndef HAK10_H
#define HAK10_H


/*===========================================================================*
 *     RefType: Both                                                         *
 *===========================================================================*/

/*===========================================================================*
 *     MD5: '\x81\xd2\xbe\x3d\x70\xa2\x2b\xbb\x66\x7e\xe1\xa6\x10\x5f\x95\x68'
 *===========================================================================*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "ggg00.h"
#include "gak00.h"
#include "vak001.h"
#include "gak40.h"


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externPascal pasbool a10chk;

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10_add_repl_sysinfo (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak_sysbufferaddress  VAR_VALUE_REF  syspoint,
    pasbool                              add_sysinfo,
    tgg00_BasisError      VAR_VALUE_REF  b_err);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10alter_long_version (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak_baserecord        VAR_VALUE_REF  base_rec,
    integer                              old_long_cnt,
    tgg00_MessType_Param                 m_type,
    pasbool                              view_scan);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10ak_cache_init ();

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal integer a10BaseRecPersistentOffset ();

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal tsp00_Int4 a10cache_used ();

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10clean_cache (
    tak_all_command_glob  VAR_VALUE_REF  acv);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10cmd_rollback (
    tak_all_command_glob  VAR_VALUE_REF  acv);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10_copy_catalog_rec (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_SysInfoKey      VAR_VALUE_REF  old_key,
    pasbool                              del_old_rec,
    tgg00_SysInfoKey      VAR_VALUE_REF  new_key,
    tsp00_C2              VAR_ARRAY_REF  new_segment_id,
    pasbool                              add_new_rec,
    tgg00_BasisError      VAR_VALUE_REF  b_err);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10cpy_result (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_SysInfoKey      VAR_VALUE_REF  pars_syskey,
    tgg00_SysInfoKey      VAR_VALUE_REF  res_syskey,
    tsp00_Int4                           rescnt,
    tgg00_BasisError      VAR_VALUE_REF  b_err);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10CopyFromSharedSQL (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    const tak_systembuffer  VAR_VALUE_REF  Sysinfo);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10DeallocTemp (
    tak_all_command_glob  VAR_VALUE_REF  acv);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10DeallocAll (
    tak_all_command_glob  VAR_VALUE_REF  acv);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10Displace (
    tak_all_command_glob  VAR_VALUE_REF  acv);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10_del_tab_sysinfo (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_Surrogate       VAR_ARRAY_REF  tableid,
    tak_del_tab_qual      VAR_VALUE_REF  qual,
    pasbool                              temp_table,
    tgg00_BasisError      VAR_VALUE_REF  b_err);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10DeleteRollbackFiles (
    tak_all_command_glob  VAR_VALUE_REF  acv);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10del_user_sysinfo (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_Surrogate       VAR_ARRAY_REF  auth,
    tgg00_BasisError      VAR_VALUE_REF  b_err);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool a10akdump (
    tak_all_command_glob  VAR_VALUE_REF  acv);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10dump (
    tak_all_command_glob  VAR_VALUE_REF  acv);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10DisableTableAccess ();

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10EnableTableAccess ();

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool a10TableAccessAllowed ();

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10expand_rec (
    tgg00_Rec             VAR_VALUE_REF  rec_buf,
    tgg00_BasisError      VAR_VALUE_REF  b_err);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10dispose (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak_sysbufferaddress  VAR_VALUE_REF  p);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10find_result (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak_parskey           VAR_VALUE_REF  pars_key,
    tak_directory_state_Param                 dstate,
    tak_sysbufferaddress  VAR_VALUE_REF  syspoint,
    tgg00_BasisError      VAR_VALUE_REF  b_err);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10_fix_len_get_sysinfo (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_SysInfoKey      VAR_VALUE_REF  syskey,
    tak_directory_state_Param                 dstate,
    integer                              required_len,
    integer                              plus,
    tak_sysbufferaddress  VAR_VALUE_REF  syspoint,
    tgg00_BasisError      VAR_VALUE_REF  b_err);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal tsp00_Int4 a10GetCreateCommandId (
    tak_sysbufferaddress                 syspointer);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal integer a10GetRefCount (
    tak_sysbufferaddress                 syspoint);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal tak_sysbufferaddress a10IncopyTableInit (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    const tgg00_SysInfoKey  VAR_VALUE_REF  key,
    integer                              colCount);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10InsertIntoCatalog (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    const tgg00_Rec       VAR_VALUE_REF  rec,
    pasbool                              doInsert);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10flush (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_SysInfoKey      VAR_VALUE_REF  syskey,
    pasbool                              disable_cmd_rollback);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10FlushResnames (
    tak_all_command_glob  VAR_VALUE_REF  acv);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10free_cache (
    tak_all_command_glob  VAR_VALUE_REF  acv);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10GetStatistics (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    pasbool                              resetStatistics,
    tsp00_Int4            VAR_VALUE_REF  accessCount,
    tsp00_Int4            VAR_VALUE_REF  successful,
    integer               VAR_VALUE_REF  hitRate);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal integer a10hit_rate (
    tak_all_command_glob  VAR_VALUE_REF  acv);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10increment_ap_tree (
    tak_all_command_glob  VAR_VALUE_REF  acv);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10intermediate_mblock (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_MessBlock       VAR_VALUE_REF  mblock,
    tgg00_MessBlockPtr    VAR_VALUE_REF  mblock_ptr);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10inquire_resnames (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tsp00_Int4            VAR_VALUE_REF  cnt,
    tak_resn_rec_arr      VAR_ARRAY_REF  resname_arr);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10ins_or_upd (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak_sysbufferaddress  VAR_VALUE_REF  syspoint,
    tgg00_BasisError      VAR_VALUE_REF  b_err);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10InsertKeptRecord (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak_sysbufferaddress                 syspointer);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10invalidate_root (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_FileId          VAR_VALUE_REF  treeid);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool a10is_fixed (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak_sysbufferaddress                 p);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10lock_fixed (
    tak_all_command_glob  VAR_VALUE_REF  acv);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10look_for_gaps (
    tak_all_command_glob  VAR_VALUE_REF  acv);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10mark_updated (
    tak_sysbufferaddress                 p);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10MarkGlobalReferencesUnused (
    tak_all_command_glob  VAR_VALUE_REF  acv);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10Lock (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak_sysbufferaddress                 p);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10Unlock (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak_sysbufferaddress                 p);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10mblock_into_cache (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_SysInfoKey      VAR_VALUE_REF  syskey,
    tgg00_MessBlock       VAR_VALUE_REF  mblock,
    tak_directory_state_Param                 dstate,
    tak_sysbufferaddress  VAR_VALUE_REF  syspoint,
    tgg00_BasisError      VAR_VALUE_REF  b_err);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10MigrateBaseRecords (
    pasbool                              doMigrate);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10new (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tsp00_Int4                           obj_size,
    tak_sysbufferaddress  VAR_VALUE_REF  p);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10new_packet (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tsp00_Int4                           size,
    tsp00_MoveObjPtr      VAR_VALUE_REF  ptr);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10prefix_parsid_delete (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak_parskey           VAR_VALUE_REF  parsk,
    integer               VAR_VALUE_REF  del_cnt,
    integer                              prefix_length);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10ReadFromCatalog (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_Rec             VAR_VALUE_REF  row);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10refresh_cache (
    tak_all_command_glob  VAR_VALUE_REF  acv);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10release_table (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_Surrogate       VAR_ARRAY_REF  tableid);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10rel_sysinfo (
    tak_sysbufferaddress                 syspointer);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10remove_parsinfos (
    tak_all_command_glob  VAR_VALUE_REF  acv);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10shrink_syntax_tree (
    tak_all_command_glob  VAR_VALUE_REF  acv);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10table_cache_delete (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_Surrogate       VAR_ARRAY_REF  tableid);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10unlock_fixed (
    tak_all_command_glob  VAR_VALUE_REF  acv);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10add_sysinfo (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak_sysbufferaddress  VAR_VALUE_REF  syspoint,
    tgg00_BasisError      VAR_VALUE_REF  b_err);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10_all_release (
    tak_all_command_glob  VAR_VALUE_REF  acv);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10_cache_delete (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    pasbool                              is_rollback);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10del_sysinfo (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_SysInfoKey      VAR_VALUE_REF  syskey,
    tgg00_BasisError      VAR_VALUE_REF  b_err);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10get_sysinfo (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_SysInfoKey      VAR_VALUE_REF  syskey,
    tak_directory_state_Param                 dstate,
    tak_sysbufferaddress  VAR_VALUE_REF  syspoint,
    tgg00_BasisError      VAR_VALUE_REF  b_err);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10_init_cache (
    tak_all_command_glob  VAR_VALUE_REF  acv);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal tak_sysbufferaddress a10KeepRecord (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_SysInfoKey      VAR_VALUE_REF  syskey);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10_key_del (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_SysInfoKey      VAR_VALUE_REF  syskey);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10key_del (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak_sysbufferaddress  VAR_VALUE_REF  syspointer);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10_lock_sysinfo (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_SysInfoKey      VAR_VALUE_REF  syskey,
    tgg00_LockReqMode_Param                 lockm);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool a10NextDisplacedKey (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_SysInfoKey      VAR_VALUE_REF  syskey);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10next_sysinfo (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_SysInfoKey      VAR_VALUE_REF  syskey,
    integer                              stop_prefix,
    tak_directory_state_Param                 dstate,
    tsp00_C2              VAR_ARRAY_REF  rec_kind,
    tak_sysbufferaddress  VAR_VALUE_REF  syspoint,
    tgg00_BasisError      VAR_VALUE_REF  b_err);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10_nil_get_sysinfo (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_SysInfoKey      VAR_VALUE_REF  syskey,
    tak_directory_state_Param                 dstate,
    tsp00_Int4                           syslen,
    tak_sysbufferaddress  VAR_VALUE_REF  syspoint,
    tgg00_BasisError      VAR_VALUE_REF  b_err);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool a10NoTableInfoInCache (
    tak_all_command_glob  VAR_VALUE_REF  acv);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10_rel_sysinfo (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_SysInfoKey      VAR_VALUE_REF  syskey);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10repl_sysinfo (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak_sysbufferaddress  VAR_VALUE_REF  syspoint,
    tgg00_BasisError      VAR_VALUE_REF  b_err);

/* Don't edit this file by hand, it is generated by ptocProto.py */


externPascal void a10ResetStatistics (
    tak_all_command_glob  VAR_VALUE_REF  acv);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10SessionCacheInit (
    tak_all_command_glob  VAR_VALUE_REF  acv);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal tak00_colinfo_ptr a10SetColumnAndName (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak_baserecord        VAR_VALUE_REF  base_rec,
    integer                              columnIndex,
    integer                              columnNameLength);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10SetMigrationProtTabid (
    integer                              tableNo,
    tgg00_Surrogate       VAR_ARRAY_REF  Tabid);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10StoreAs (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_SysInfoKey                     syskey,
    tgg00_SysInfoKey      VAR_VALUE_REF  storeSyskey,
    tgg00_BasisError      VAR_VALUE_REF  e);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10_upd_key (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak_parskey           VAR_VALUE_REF  parsk,
    integer                              displacement,
    tgg00_BasisError      VAR_VALUE_REF  b_err);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10_version (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak_baserecord        VAR_VALUE_REF  base_rec,
    tgg00_MessType_Param                 m_type,
    pasbool                              view_scan);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a10parsinfos (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak40_show_glob       VAR_VALUE_REF  a41v);

/* Don't edit this file by hand, it is generated by ptocProto.py */


#endif
