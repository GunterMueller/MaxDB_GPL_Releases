/*!******************************************************************************

  @file         hak722.h

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

#ifndef HAK722_H
#define HAK722_H


/*===========================================================================*
 *     RefType: C++                                                          *
 *===========================================================================*/

/*===========================================================================*
 *     MD5: '\x7b\xda\xe4\xb9\x4f\xee\xbc\x43\x26\x9d\xc4\x52\xc0\x5d\x05\x52'
 *===========================================================================*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "ggg00.h"
#include "vak001.h"
#include "ggg07.h"
#include "gak71.h"
#include "gak40.h"


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externPascal void a722add_col_qual (
    tak_all_command_glob  &  acv,
    tak40_show_glob       &  a41v,
    tgg00_MessBlock       &  mblock,
    integer                  colno,
    tgg00_SysInfoKey      &  startkey,
    tgg00_SysInfoKey      &  stopkey,
    tak40_int2_arr_ptr       pos_info);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool a722test_col_qual (
    tak_all_command_glob  &  acv,
    tak40_show_glob       &  a41v,
    tak_oldidentifier     &  colname,
    char                     def_byte,
    tsp00_MoveObjPtr         value_ptr,
    integer                  value_len,
    tak40_int2_arr_ptr       pos_info);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a722update_strategy (
    tak_all_command_glob  &  acv,
    tgg07_StrCatalog      &  cat_strat);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a722strategy (
    tak_all_command_glob  &  acv,
    tak_dml_info          &  dmli,
    tak71_page_eval_rec   &  eval_info,
    tgg07_StrategyInfo    &  gg_strategy,
    tsp00_Int2            &  strat_len);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool a722full_one_level_id (
    tgg07_StrategyInfo    &  strat);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool a722full_two_level_id (
    tgg07_StrategyInfo    &  strat);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a722init_epos_reccol_relation (
    tak_all_command_glob  &  acv,
    tak40_int2_arr_ptr    &  pos_info);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a722finalize_epos_reccol_relation (
    tak_all_command_glob  &  acv,
    tak40_int2_arr_ptr    &  pos_info);

/* Don't edit this file by hand, it is generated by ptocProto.py */


#endif

