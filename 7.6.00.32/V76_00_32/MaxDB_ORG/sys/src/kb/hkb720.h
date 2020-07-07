/*!******************************************************************************

  @file         hkb720.h

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

#ifndef HKB720_H
#define HKB720_H


/*===========================================================================*
 *     RefType: C++                                                          *
 *===========================================================================*/

/*===========================================================================*
 *     MD5: '\xbd\x5c\x2a\x34\x54\xa7\x8f\xb5\x3d\x43\x8c\xb4\xc9\xea\x50\x14'
 *===========================================================================*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "ggg00.h"
#include "ggg07.h"


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externPascal void k720finish_catalog_select (
    tgg00_MessBlock       &  m,
    tgg00_SelectFieldsParam  &  sel,
    tgg07_StrategyInfo    &  strat,
    tgg07_select_context  &  selrec,
    tgg07_get_param       &  getrec,
    tgg00_Rec             &  lbuf);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k720init_ref_statistic ();

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k720ref_statistic (
    tgg00_RefInfoIndex    &  arr_index,
    tgg07_StrategyInfo    &  strat);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k720diagmon_strat_put (
    tgg00_TransContext    &  trans,
    tgg07_StrategyInfo    &  gg_strategy);

/* Don't edit this file by hand, it is generated by ptocProto.py */


#if defined (trace)

externPascal void k720trace_selrec (
    tgg00_MessBlock       &  m,
    tgg07_select_param    &  selrec);

/* Don't edit this file by hand, it is generated by ptocProto.py */


#endif

externPascal void k720start_catalog_select (
    tgg00_MessBlock       &  m,
    tgg00_SelectFieldsParam  &  sel,
    tgg07_StrategyInfo    &  gg_strategy,
    tgg07_select_context  &  selrec,
    pasbool               &  finding_possible);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k720monitor (
    tgg00_TransContext    &  trans,
    tgg00_SelectFieldsParam  &  sel,
    tsp00_Int4               start_sec,
    tsp00_Int4               start_microsec,
    tsp00_Int4               start_phys_ios,
    tsp00_Int4               start_suspends,
    tsp00_Int4               start_waits,
    pasbool                  put_strat,
    tgg00_RefInfoIndex_Param     arr_index,
    pasbool                  strat_cnt);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k720show_monitor (
    tgg00_ReferenceInfo   &  info);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k720_initialize (
    tgg00_MessBlock       &  m,
    tgg07_select_param    &  selrec,
    tgg07_StrategyInfo    &  gg_strategy,
    pasbool               &  use_stopkey);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k720_select (
    tgg00_MessBlock       &  m,
    tgg07_get_param       &  getrec,
    tgg07_select_param    &  selrec);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k720_single_select (
    tgg00_MessBlock       &  m);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k720_test_subquery (
    tgg00_TransContext    &  trans,
    tgg00_DataPart        &  datapart,
    tsp00_Int4               datapartsize,
    tgg00_StackDesc       &  mdesc,
    tgg00_Rec             &  rec);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k720apply_aggr_func (
    tgg00_AggrDescrRec    &  funct_fields,
    tgg00_Lkey            &  listkey,
    void                  *  lbuf,
    pasbool                  one_list_only,
    tgg00_BasisError      &  e);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k720_maxresult_get (
    tsp00_Int4               strat_maxcnt,
    tsp00_Int4            &  maxresult);

/* Don't edit this file by hand, it is generated by ptocProto.py */


#endif

