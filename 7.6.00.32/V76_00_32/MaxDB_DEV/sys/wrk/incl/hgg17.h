/*!******************************************************************************

  @file         hgg17.h

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

#ifndef HGG17_H
#define HGG17_H


/*===========================================================================*
 *     RefType: C++                                                          *
 *===========================================================================*/

/*===========================================================================*
 *     MD5: '\x59\x60\xb1\x51\x69\x3c\x3f\x04\x64\x23\x41\x14\x92\x5a\x37\x38'
 *===========================================================================*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "ggg00.h"


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externPascal void g17addr_to_line (
    void*                    any_addr,
    integer               &  ln_len,
    char*                    ln);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void g17basis_err_to_line (
    tgg00_BasisError         b_err,
    integer               &  ln_len,
    char*                    ln);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void g17bd_use_set_to_line (
    tgg00_BdUseSet        &  bd_use,
    integer               &  ln_len,
    char*                    ln);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void g17bool_to_line (
    tsp00_Name            &  nam,
    integer                  bool_int,
    pasbool                  with_delimiter,
    integer               &  ln_len,
    char*                    ln);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void g17counter_to_line (
    tsp00_8ByteCounter       counter,
    integer               &  ln_len,
    char*                    ln);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void g17cpp_str_to_line (
    char*                    str,
    integer               &  ln_len,
    char*                    ln);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void g17c2int_to_line (
    tsp00_C2              &  intval,
    integer               &  ln_len,
    char*                    ln);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void g17c4int_to_line (
    tsp00_C4              &  intval,
    integer               &  ln_len,
    char*                    ln);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void g17date_to_line (
    char*                    date,
    integer               &  ln_len,
    char*                    ln);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void g17dump_state_set (
    tgg00_DumpStateSet    &  dump_state,
    integer               &  ln_len,
    char*                    ln);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void g17filename (
    char*                    nam,
    char*                    fn,
    integer                  line_indent,
    char*                    first_ln,
    char*                    second_ln,
    char*                    third_ln,
    pasbool               &  is_third_ln);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void g17fileno_to_line (
    tgg00_Surrogate       &  fileNo,
    integer               &  ln_len,
    char*                    ln);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void g17filetype_to_line (
    tgg00_FiletypeSet     &  filetype,
    pasbool                  is_edit_mode,
    integer               &  ln_len,
    char*                    ln);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void g17efiletype_to_line (
    tgg00_ExtendedFiletypeSet  &  efiletype,
    pasbool                  is_edit_mode,
    integer               &  ln_len,
    tsp00_Line            &  ln);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void g17handling_set_to_line (
    tgg00_HandlingSet     &  handling,
    integer               &  ln_len,
    char*                    ln);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void g17hexto_line (
    char                     c,
    integer               &  ln_len,
    char*                    ln);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void g17identifier_to_line (
    tsp00_KnlIdentifier   &  id,
    integer               &  ln_len,
    char*                    ln);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void g17intdate_time (
    tsp00_Int4               int_date,
    tsp00_Int4               int_time,
    char*                    str_date,
    char*                    str_time);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void g17int4to_line (
    tsp00_Int4               intval,
    pasbool                  with_zero,
    integer                  int_len,
    integer                  ln_pos,
    char*                    ln);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void g17lname_to_line (
    tsp00_Lname           &  n,
    integer               &  ln_len,
    tsp00_Line            &  ln);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void g17lockmode_to_line (
    tgg00_LockReqMode_Param     lock_mode,
    integer               &  ln_len,
    char*                    ln);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void g17lockstate_to_line (
    tgg00_LockReqState    &  lock_state,
    integer               &  ln_len,
    char*                    ln);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void g17longreal_to_line (
    tsp00_Longreal           re,
    integer                  digits,
    integer                  pos,
    char*                    ln);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void g17messtype_to_line (
    tgg00_MessType_Param     mess_type,
    integer               &  ln_len,
    char*                    ln);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void g17mess2type_to_line (
    tgg00_MessType2_Param     mess2_type,
    integer               &  ln_len,
    char*                    ln);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void g17nameto_line (
    const char*              n,
    integer               &  ln_len,
    char*                    ln);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void g17obj_state_to_line (
    tgg00_ObjState_Param     obj_state,
    integer               &  ln_len,
    char*                    ln);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void g17obj_transinfo_to_line (
    tgg00_ObjTransInfo       transinfo,
    integer               &  ln_len,
    char*                    ln);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void g17onestackentry (
    tgg00_StackEntry      &  st,
    integer                  entry_index,
    char*                    ln,
    pasbool               &  is_second_ln,
    char*                    second_ln);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void g17page_ref_to_line (
    tgg91_PageRef         &  page_ref,
    integer               &  ln_len,
    char*                    ln);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void g17pagecheck_to_line (
    tgg00_PageCheckType_Param     pagecheck,
    integer               &  ln_len,
    char*                    ln);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void g17pagemode_to_line (
    tgg00_PageMode        &  pagemode,
    pasbool                  is_edit_mode,
    integer               &  ln_len,
    char*                    ln);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void g17pagetype_to_line (
    tgg00_PageType_Param     pagetype,
    integer               &  ln_len,
    char*                    ln);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void g17pagetype2_to_line (
    tgg00_PageType2_Param     pagetype2,
    integer               &  ln_len,
    char*                    ln);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void g17paramspec_to_line (
    tsp00_DataType_Param     data_type,
    tsp00_Int2               length,
    tsp00_Uint1              frac,
    integer               &  ln_len,
    char*                    ln);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void g17parsidinfo_to_line (
    char                     info,
    integer               &  ln_len,
    char*                    ln);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool g17printable_char (
    char                     c);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void gg17SessionTransToLine (
    tgg91_SessionNo       &  session,
    tgg91_TransNo         &  transid,
    tgg91_TransNo         &  writetransid,
    tgg00_SubtransNo      &  subtrans,
    integer               &  ln_len,
    char*                    ln);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void g17siteto_line (
    tgg00_ServerdbNo      &  site,
    integer               &  ln_len,
    char*                    ln);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void g17sname_to_line (
    tsp00_Sname           &  n,
    integer               &  ln_len,
    char*                    ln);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void g17sp1function_to_line (
    tsp00_Int2               function_code,
    integer               &  ln_len,
    char*                    ln);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void g17st1op (
    tgg00_StackOpType_Param     op,
    integer               &  len,
    char*                    msg);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void g17st2op_func (
    tgg00_StackOpFunc_Param     op,
    integer               &  len,
    char*                    msg);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void g17st3op_output (
    tgg00_StackOpOutput_Param     op,
    integer               &  len,
    char*                    msg);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void g17st4op_build_in (
    tgg00_StackOpBuildIn_Param     op,
    integer               &  len,
    char*                    msg);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void g17surrogate_to_line (
    tgg00_Surrogate       &  surrogate,
    integer               &  ln_len,
    char*                    ln);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void g17tfntype_to_line (
    tgg00_Tfn_Param          tfn,
    integer               &  ln_len,
    char*                    ln);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void g17time_to_line (
    tsp00_Time            &  time,
    integer               &  ln_len,
    char*                    ln);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void g17trans_state_to_line (
    tgg00_TransState      &  trans_state,
    integer               &  ln_len,
    char*                    ln);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void g17trimint4_to_line (
    tsp00_Int4               intval,
    integer               &  ln_len,
    char*                    ln);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void g17ttfntemp_to_line (
    tgg00_TfnTemp_Param      ttfn,
    integer               &  ln_len,
    char*                    ln);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void g17vftype (
    tsp00_VfType_Param       ft,
    tsp00_Int4            &  ln_len,
    char*                    ln);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void g17warning_item_to_line (
    tsp00_Warnings_Param     warning_item,
    integer               &  ln_len,
    char*                    ln);

/* Don't edit this file by hand, it is generated by ptocProto.py */


#endif

