/*!******************************************************************************

  @file         hta01_3.h

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

#ifndef HTA01_3_H
#define HTA01_3_H


/*===========================================================================*
 *     RefType: C++                                                          *
 *===========================================================================*/

/*===========================================================================*
 *     MD5: '\x4c\x84\xbe\xc0\xf8\xc3\x16\x17\xc7\x4e\x8b\x5c\x7a\x29\x65\xb2'
 *===========================================================================*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "ggg00.h"
#include "ggg04.h"
#include "gak00.h"
#include "vak001.h"
#include "ggg07.h"


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externPascal void t01strategy (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tgg07_StrategyInfo    &  strategy);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01strat_enum (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tgg07_StratEnum_Param     strat);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01showkind (
    tgg00_Debug_Param        debug,
    tsp00_Sname           &  nam,
    tgg00_ShowKind_Param     show_kind);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01qual_kind (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tgg00_QualKind_Param     qual_kind);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01subq_kind (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tgg07_SubqKind_Param     subq_kind);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01distinct_kind (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tgg04_Distinct_Param     dist_kind);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01execution_kind (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tak_execution_kind_Param     ex_kind);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01comp_result (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tsp00_LcompResult_Param     comp_res);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01command_kind (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tak_commandkind_Param     command_kind);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01corr_type (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tak_corr_type_Param      corr_type);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01recursive_state (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tak_recursive_state_Param     recursive_state);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01columnset (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tak_columnset         &  columnset);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01knl_identifier (
    tgg00_Debug_Param        debug,
    const char*              nam,
    const char*              identifier);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01optimize_info (
    tgg00_Debug_Param        debug,
    tgg00_StackDesc       &  stack_desc,
    tsp00_MoveObjPtr         data_addr);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01catalog_entrytype (
    tgg00_Debug_Param        debug,
    tsp00_Char               etype);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01set_bool_debug_or_trace ();

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01disable_debug_or_trace ();

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01fp_kind (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tak_fp_kind_type_ptr     fp_kind);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01p2bool (
    tgg00_Debug_Param        debug,
    const char*              nam_1,
    pasbool                  bool1,
    const char*              nam_2,
    pasbool                  bool2);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01fetch_desc (
    tgg00_Debug_Param        debug,
    tsp00_Sname           &  nam,
    tgg00_FetchDesc       &  mf_desc);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01dist_optim (
    tgg00_Debug_Param        debug,
    tsp00_Sname           &  nam,
    tsp00_Int2               dist_optim);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01init_line (
    tsp00_Line            &  ln,
    integer               &  ln_len,
    tsp00_Sname           &  nam);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01p2real (
    tgg00_Debug_Param        debug,
    tsp00_Sname           &  nam_1,
    tsp00_Longreal           real_1,
    tsp00_Sname           &  nam_2,
    tsp00_Longreal           real_2,
    integer                  digits);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01trace_symbol (
    tgg00_Debug_Param        debug,
    tsp00_Sname           &  name,
    tak_sc_symbol_Param      symb);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01trace_kw (
    tgg00_Debug_Param        debug,
    tsp00_Sname           &  name,
    tak_keyword           &  kw);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01Int4 (
    tgg00_Debug_Param        debug,
    tsp00_Name            &  nam,
    tsp00_Int4               int4);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01line_c60 (
    tgg00_Debug_Param        debug,
    const char*              ln_p1);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01selfields (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tgg00_SelectFieldsParam     sfp);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01rowno (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tsp00_Int4               rowno);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01refinfoindex (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tgg00_RefInfoIndex_Param     idx);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01strat_access_mode (
    tgg00_Debug_Param        debug,
    tsp00_Sname           &  nam,
    tgg07_StratAccessModSet  &  access);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01colpropset (
    tgg00_Debug_Param        debug,
    tsp00_Sname           &  nam,
    tak00_colpropset      &  s);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01union_kind (
    tgg00_Debug_Param        debug,
    tsp00_Sname           &  nam,
    tak_union_kind_Param     ukind,
    pasbool                  uis_all);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01ttfntemp (
    tgg00_Debug_Param        debug,
    tsp00_Sname           &  nam,
    tgg00_TfnTemp_Param      ttfn);

/* Don't edit this file by hand, it is generated by ptocProto.py */


#endif

