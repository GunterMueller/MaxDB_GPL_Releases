/*!******************************************************************************

  @file         hbd21.h

  ------------------------------------------------------------------------------

  @author       JuergenP

  @brief        BD-tdata-cache

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

#ifndef HBD21_H
#define HBD21_H


/*===========================================================================*
 *     RefType: C++                                                          *
 *===========================================================================*/

/*===========================================================================*
 *     MD5: '\x8a\xf9\x91\xbc\x81\x30\x7e\x94\x09\xe4\x1d\x03\xf4\x9b\x92\x6b'
 *===========================================================================*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "ggg00.h"


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externPascal void b21init_temp_cache (
    tsp00_TaskId             pid,
    pasbool                  allocate_monitor,
    pasbool                  is_control_session,
    tgg00_TempDataCachePtr  &  temp_cache_ptr,
    tgg00_BasisError      &  err);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b21m_cmdid (
    tgg00_TempDataCachePtr     temp_cache_ptr,
    tgg00_Surrogate       &  cmdid);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b21m_copy_result (
    tgg00_TempDataCachePtr     temp_cache_ptr,
    tsp00_C3              &  copy_result);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b21m_date_time (
    tsp00_Date            &  curr_date,
    tsp00_Time            &  curr_time);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b21m_init_monitor_rec ();

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool b21m_is_select (
    tgg00_TempDataCachePtr     temp_cache_ptr);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal tsp00_Int4 b21m_fetch_calls (
    tgg00_TempDataCachePtr     temp_cache_ptr);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal tsp00_Int4 b21m_kb_calls (
    tgg00_TempDataCachePtr     temp_cache_ptr);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal tsp00_Int4 b21m_microsec (
    tgg00_TempDataCachePtr     temp_cache_ptr);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal tsp00_Int4 b21m_obj_hist_read_count (
    tgg00_TempDataCachePtr     temp_cache_ptr);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b21m_parse_again (
    tgg00_TempDataCachePtr     temp_cache_ptr,
    tsp00_C3              &  parse_again);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b21m_parseid (
    tgg00_TempDataCachePtr     temp_cache_ptr,
    tsp00_C12             &  parseid,
    tsp00_C12             &  masterparseid);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal tsp00_Int4 b21m_phys_ios (
    tgg00_TempDataCachePtr     temp_cache_ptr);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b21m_reset_monitor (
    tgg00_TempDataCachePtr     temp_cache_ptr,
    pasbool                  reset_sum);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b21m_roots (
    tgg00_TempDataCachePtr     temp_cache_ptr,
    tsp00_Int4            &  cnt,
    tgg00_RootListPtr     &  rootlist_ptr);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal tsp00_Int4 b21m_rows_fetched (
    tgg00_TempDataCachePtr     temp_cache_ptr);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal tsp00_Int4 b21m_rows_qual (
    tgg00_TempDataCachePtr     temp_cache_ptr);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal tsp00_Int4 b21m_rows_read (
    tgg00_TempDataCachePtr     temp_cache_ptr);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal tsp00_Int4 b21m_sec (
    tgg00_TempDataCachePtr     temp_cache_ptr);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b21m_strat (
    tgg00_TempDataCachePtr     temp_cache_ptr,
    tsp00_C40             &  strategy);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal tsp00_Int4 b21m_suspends (
    tgg00_TempDataCachePtr     temp_cache_ptr);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b21m_termid (
    tgg00_TempDataCachePtr     temp_cache_ptr,
    tsp00_TermId          &  termid);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b21m_username (
    tgg00_TempDataCachePtr     temp_cache_ptr,
    tsp00_KnlIdentifier   &  username);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b21m_values (
    tgg00_TempDataCachePtr     temp_cache_ptr,
    tsp00_MoveObjPtr      &  values_addr,
    tsp00_Int4            &  values_len);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal tsp00_Int4 b21m_virt_reads (
    tgg00_TempDataCachePtr     temp_cache_ptr);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal tsp00_Int4 b21m_waits (
    tgg00_TempDataCachePtr     temp_cache_ptr);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b21mp_cmdid_put (
    tgg00_TempDataCachePtr     temp_cache_ptr,
    tgg00_Surrogate       &  cmdid);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b21mp_copy_result_put (
    tgg00_TempDataCachePtr     temp_cache_ptr,
    tsp00_C3              &  copy_result);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b21mp_fetch_calls_put (
    tgg00_TempDataCachePtr     temp_cache_ptr);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b21mp_is_select (
    tgg00_TempDataCachePtr     temp_cache_ptr,
    pasbool                  is_select);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b21mp_kb_calls_put (
    tgg00_TempDataCachePtr     temp_cache_ptr);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b21mp_microsec_put (
    tgg00_TempDataCachePtr     temp_cache_ptr,
    tsp00_Int4               microsec);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b21mp_new_sum_put (
    tgg00_TempDataCachePtr     temp_cache_ptr);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b21mp_obj_hist_read_count_put (
    tgg00_TempDataCachePtr     temp_cache_ptr,
    tsp00_Int4               read_count);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b21mp_parse_again_put (
    tgg00_TempDataCachePtr     temp_cache_ptr,
    tsp00_C3              &  parse_again);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b21mp_parseid_put (
    tgg00_TempDataCachePtr     temp_cache_ptr,
    tsp00_C12             &  parseid,
    tsp00_C12             &  masterparseid);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b21mp_phys_ios_put (
    tgg00_TempDataCachePtr     temp_cache_ptr,
    tsp00_Int4               phys_ios);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b21mp_root_put (
    tgg00_TempDataCachePtr     temp_cache_ptr,
    tsp00_PageNo             root);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b21mp_rows_fetched_put (
    tgg00_TempDataCachePtr     temp_cache_ptr,
    tsp00_Int4               rows_fetched);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b21mp_rows_qual_put (
    tgg00_TempDataCachePtr     temp_cache_ptr,
    tsp00_Int4               rows_qual);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b21mp_rows_read_put (
    tgg00_TempDataCachePtr     temp_cache_ptr,
    tsp00_Int4               rows_read);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b21mp_sec_put (
    tgg00_TempDataCachePtr     temp_cache_ptr,
    tsp00_Int4               sec);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b21mp_strat_put (
    tgg00_TempDataCachePtr     temp_cache_ptr,
    tsp00_C40             &  strategy,
    tsp00_Int4               len);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b21mp_suspends_put (
    tgg00_TempDataCachePtr     temp_cache_ptr,
    tsp00_Int4               suspends);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b21mp_termid_put (
    tgg00_TempDataCachePtr     temp_cache_ptr,
    tsp00_TermId          &  termid);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b21mp_username_put (
    tgg00_TempDataCachePtr     temp_cache_ptr,
    tsp00_KnlIdentifier   &  username);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b21mp_value_put (
    tgg00_TempDataCachePtr     temp_cache_ptr,
    tsp00_MoveObjPtr         curr_value_addr,
    tsp00_Int4               curr_value_len);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b21mp_virt_reads_put (
    tgg00_TempDataCachePtr     temp_cache_ptr,
    tsp00_Int4               virt_reads);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b21mp_waits_put (
    tgg00_TempDataCachePtr     temp_cache_ptr,
    tsp00_Int4               waits);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b21temp_cache_release (
    tsp00_TaskId             pid,
    tgg00_TempDataCachePtr     temp_cache_ptr);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal tsp00_Int4 b21tmp_cache_size (
    tgg00_TempDataCachePtr     temp_cache_ptr);

/* Don't edit this file by hand, it is generated by ptocProto.py */


#endif

