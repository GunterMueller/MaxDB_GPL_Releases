/*!******************************************************************************

  @file         hak725.h

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

#ifndef HAK725_H
#define HAK725_H


/*===========================================================================*
 *     RefType: Both                                                         *
 *===========================================================================*/

/*===========================================================================*
 *     MD5: '\x27\xa1\x98\xeb\x38\x0f\xbf\xf2\xeb\xa6\xab\xbb\xca\x0a\xbf\xcd'
 *===========================================================================*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "ggg00.h"
#include "vak001.h"
#include "ggg07.h"
#include "gak70.h"
#include "gak71.h"


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externPascal void a725gg_strategy (
    tgg00_Debug_Param                    debug,
    tgg07_StrategyInfo    VAR_VALUE_REF  strat);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a725output_colposarr (
    tgg00_Debug_Param                    debug,
    tsp00_Sname           VAR_ARRAY_REF  nam,
    tgg07_ColPosArr       VAR_ARRAY_REF  keypos);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a725output_invstrat (
    tgg00_Debug_Param                    debug,
    tsp00_Sname           VAR_ARRAY_REF  nam,
    tgg07_StrInvInRange   VAR_VALUE_REF  inv_strat);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a725output_keystrat (
    tgg00_Debug_Param                    debug,
    tsp00_Sname           VAR_ARRAY_REF  nam,
    tgg07_StrKeyInRange   VAR_VALUE_REF  key_strat);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a725output_access_info (
    tgg00_Debug_Param                    debug,
    tsp00_Sname           VAR_ARRAY_REF  nam,
    tak70_strategy_record  VAR_VALUE_REF  access_info);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a725L1_terms_output (
    tgg00_Debug_Param                    debug,
    tak70_term            VAR_VALUE_REF  L1_terms);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a725output_involved_cols (
    tgg00_Debug_Param                    debug,
    tsp00_Sname           VAR_ARRAY_REF  nam,
    tak70_involved_columns                 ic_info);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a725output_eval (
    tgg00_Debug_Param                    debug,
    tsp00_Sname           VAR_ARRAY_REF  nam,
    tak71_page_eval_rec   VAR_VALUE_REF  eval_stat);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a725output_one_strat (
    tgg00_Debug_Param                    debug,
    tak70_one_strat       VAR_VALUE_REF  one_strat);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a725output_gg_strat (
    tgg00_Debug_Param                    debug,
    const char*                          nam,
    tgg07_StrategyInfo    VAR_VALUE_REF  gg_strat);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a725output_colposarr_ex (
    tgg00_Debug_Param                    debug,
    tsp00_Sname           VAR_ARRAY_REF  nam,
    tgg07_ColPosArr       VAR_ARRAY_REF  keypos,
    tsp00_Int2                           length);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a725output_invposarr (
    tgg00_Debug_Param                    debug,
    tsp00_Sname           VAR_ARRAY_REF  nam,
    tak70_Invposarr       VAR_ARRAY_REF  invpos);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a725output_end_strat (
    tgg00_Debug_Param                    debug,
    tsp00_Sname           VAR_ARRAY_REF  nam,
    tgg07_StrategyInfo    VAR_VALUE_REF  gg_strategy,
    tak71_page_eval_rec   VAR_VALUE_REF  eval_info,
    tsp00_Int2            VAR_VALUE_REF  StratInfo_len);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a725output_query_prop (
    tgg00_Debug_Param                    debug,
    tak70_query_properties                 qprop);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a725output_sequence_prop (
    tgg00_Debug_Param                    debug,
    tak70_sequence_properties  VAR_ARRAY_REF  sprop);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a725output_more_strat (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_Debug_Param                    debug,
    tgg07_StrategyInfo    VAR_VALUE_REF  gg_strategy,
    tak70_all_strat       VAR_VALUE_REF  all_strats);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a725output_explain (
    tgg00_Debug_Param                    debug,
    tak71_explain_rec     VAR_VALUE_REF  expl);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a725output_col_cluster (
    tgg00_Debug_Param                    debug,
    tsp00_Sname           VAR_ARRAY_REF  nam,
    tak70_sort_col_arr    VAR_ARRAY_REF  col_cluster,
    tsp00_Int2                           array_len);

/* Don't edit this file by hand, it is generated by ptocProto.py */


#endif

