/*!******************************************************************************

  @file         hak28.h

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

#ifndef HAK28_H
#define HAK28_H


/*===========================================================================*
 *     RefType: Both                                                         *
 *===========================================================================*/

/*===========================================================================*
 *     MD5: '\x74\xcd\x6f\x0c\xbd\xe7\xe5\x8a\xfc\x31\x25\x73\x3f\x85\xe2\x28'
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

externPascal tgg00_FileId a28updstatwanted_tree;

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a28assign_col_info (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_SysInfoKey      VAR_VALUE_REF  syskey,
    tak_sysbufferaddress  VAR_VALUE_REF  stat_buf,
    integer                              colno,
    tsp00_Int4                           dist_values,
    tsp00_Int4                           pages,
    tsp00_Int4                           avg_lst_len);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a28column_statistics (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak_sysbufferaddress                 base_ptr,
    tak00_columninfo      VAR_VALUE_REF  col_info,
    pasbool                              infer_stats,
    tak_column_statistics  VAR_VALUE_REF  statistics,
    pasbool               VAR_VALUE_REF  stat_rec_found);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a28info (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak_sysbufferaddress                 base_ptr,
    tsp00_Int4            VAR_VALUE_REF  pages,
    tsp00_Int4            VAR_VALUE_REF  rows,
    tsp00_Int4            VAR_VALUE_REF  date,
    tsp00_Int4            VAR_VALUE_REF  time);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a28new_statistic_info (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak_sysbufferaddress                 base_ptr);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a28lock_table_for_updstat (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_Surrogate       VAR_ARRAY_REF  table_surr,
    tgg00_BasisError      VAR_VALUE_REF  b_err);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a28lock_stat_catalog_rec (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak_sysbufferaddress                 base_ptr,
    pasbool                              lockExclusive,
    tak_sysbufferaddress  VAR_VALUE_REF  stat_buf,
    tgg00_BasisError      VAR_VALUE_REF  b_err);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool a28nextUpdStatWantedEntry (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak40_show_glob       VAR_VALUE_REF  a41v,
    tgg00_Lkey            VAR_VALUE_REF  key);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal tsp00_Int4 a28prim_pages (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak_baserecord        VAR_VALUE_REF  baserec);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal tsp00_Int4 a28primrows (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak_baserecord        VAR_VALUE_REF  baserec);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a28set_col_info (
    tak_statisticsinfo    VAR_VALUE_REF  stat_rec,
    integer                              colno,
    tsp00_Int4                           dist_values,
    tsp00_Int4                           pages,
    tsp00_Int4                           avg_lst_len);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a28sys_upd_statistics (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_FileId          VAR_VALUE_REF  tree,
    tgg00_MessType_Param                 mtype,
    tgg00_MessType2_Param                 m2type,
    tsp00_Int4                           known_pages,
    tsp00_Int4                           found_pages);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a28add_table_upd_statistics (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tsp00_Addr                           updStatObj,
    pasbool                              bForceUpdStat,
    tsp00_Int4                           sample_rows,
    integer                              sample_pct);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a28table_upd_statistics (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    pasbool                              bForceUpdStat,
    tsp00_Int4                           sample_rows,
    integer                              sample_pct,
    tsp00_Int4                           page_cnt,
    tsp00_Int4                           rec_cnt,
    tsp00_Int4                           sec_cnt,
    integer                              colno,
    pasbool                              add,
    pasbool                              commit);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a28TransferStatistics (
    tgg00_TransContext    VAR_VALUE_REF  t,
    tak_baserecord        VAR_VALUE_REF  base_rec);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool a28TreeStoredInVar (
    tgg00_FileId          VAR_VALUE_REF  tree);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a28update_statistics (
    tak_all_command_glob  VAR_VALUE_REF  acv);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a28rows_and_colstat (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak_sysbufferaddress                 base_ptr,
    tak00_columninfo      VAR_VALUE_REF  col_info,
    tak_column_statistics  VAR_VALUE_REF  statistics,
    tsp00_Int4            VAR_VALUE_REF  prim_rows,
    pasbool               VAR_VALUE_REF  no_cstats);

/* Don't edit this file by hand, it is generated by ptocProto.py */


#endif
