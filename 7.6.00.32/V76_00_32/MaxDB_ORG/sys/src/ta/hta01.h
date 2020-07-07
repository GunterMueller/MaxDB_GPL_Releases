/*!******************************************************************************

  @file         hta01.h

  ------------------------------------------------------------------------------

  @author       ElkeZ

  @brief        Test_Procedures

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

#ifndef HTA01_H
#define HTA01_H


/*===========================================================================*
 *     RefType: C++                                                          *
 *===========================================================================*/

/*===========================================================================*
 *     MD5: '\xe9\x9b\x46\x87\x3f\xa5\xe1\xe8\x27\xe2\x7b\x36\x62\x91\xf9\xc1'
 *===========================================================================*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "ggg00.h"


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externPascal void ta01dump_state_set (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tgg00_DumpStateSet    &  s);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void cccinit (
    tsp00_Ccc             &  ccv);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void cccoutp (
    tsp00_C20             &  n,
    tsp00_Ccc                ccv);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void cccprint (
    tsp00_Trace           &  tt);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01addr (
    tgg00_Debug_Param        debug,
    const char*              nam,
    const void*              bufaddr);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01addr_1 (
    tgg00_Debug_Param        debug,
    const char*              nam,
    void*                    bufaddr);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01addr_2 (
    tgg00_Debug_Param        debug,
    const char*              nam,
    void*                    bufaddr);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01addr_3 (
    tgg00_Debug_Param        debug,
    const char*              nam,
    void*                    bufaddr);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01addr2 (
    tgg00_Debug_Param        debug,
    const char*              nam1,
    void*                    bufaddr1,
    const char*              nam2,
    void*                    bufaddr2);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01aptree (
    tgg00_Debug_Param        debug,
    void*                    ap_tree,
    integer                  node_cnt,
    tsp00_Int2               hint_node);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01basis_error (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tgg00_BasisError         b_err);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01bduse (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tgg00_BdUseSet        &  s);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01bool (
    tgg00_Debug_Param        debug,
    const char*              nam,
    pasbool                  curr_bool);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01bool2 (
    tgg00_Debug_Param        debug,
    const char*              nam1,
    pasbool                  bool1,
    const char*              nam2,
    pasbool                  bool2);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01buf (
    tgg00_Debug_Param        debug,
    void*                    buf,
    integer                  startpos,
    integer                  endpos);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01buf1 (
    tgg00_Debug_Param        debug,
    void*                    buf,
    integer                  startpos,
    integer                  endpos);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01char (
    tgg00_Debug_Param        debug,
    const char*              nam,
    char                     c);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01counter (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tsp00_8ByteCounter       counter);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01cpp_trace (
    const char*              proc_name,
    pasbool                  is_start);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01c2int (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tsp00_C2              &  int2);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01c4int (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tsp00_C4              &  int4);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01c18 (
    tgg00_Debug_Param        debug,
    const char*              msg);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01c32 (
    tgg00_Debug_Param        debug,
    const char*              msg);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01c64 (
    tgg00_Debug_Param        debug,
    const char*              msg);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01c30 (
    tgg00_Debug_Param        debug,
    const char*              msg);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01c40 (
    tgg00_Debug_Param        debug,
    const char*              msg);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01devname (
    tgg00_Debug_Param        debug,
    const char*              nam,
    const char*              d);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01filename (
    tgg00_Debug_Param        debug,
    const char*              fn);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01filetype (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tgg00_FiletypeSet     &  s);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void ta01FileVers (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tgg91_FileVersion     &  FileVers);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01handling (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tgg00_HandlingSet     &  s);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01identifier (
    tgg00_Debug_Param        debug,
    const char*              identifier);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01int4 (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tsp00_Int4               int4);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01int4_addr (
    tgg00_Debug_Param        debug,
    const char*              nam1,
    tsp00_Int4               int4,
    const char*              nam2,
    void*                    bufaddr);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool t01istrace (
    tgg00_Debug_Param        debug,
    tgg91_SessionNo          session);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01key (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tgg00_Lkey            &  k);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01lkey (
    tgg00_Debug_Param        debug,
    tgg00_Lkey            &  k);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01lidentifier (
    tgg00_Debug_Param        debug,
    const char*              identifier);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01line (
    tgg00_Debug_Param        debug,
    const char*              ln);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01bufline (
    tgg00_Debug_Param        debug,
    const char*              ln,
    integer                  len);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01lmulti_switch (
    tsp00_C20             &  s20,
    tsp00_C20             &  s201,
    tsp00_C16             &  on_text,
    tsp00_C16             &  off_text,
    integer                  on_count);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01localstate (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tgg00_TransState      &  s);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01lock (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tgg00_Lock            &  lock);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01lockmode (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tgg00_LockReqMode_Param     lock_mode);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01lockstate (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tgg00_LockReqState    &  lock_state);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01long_qual (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tgg00_LongQual        &  long_qual);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01longdescriptor (
    tgg00_Debug_Param        debug,
    tsp00_LongDescriptor     long_desc);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01ldb_longdescblock (
    tgg00_Debug_Param        debug,
    tsp00_LongDescBlock      long_desc);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01mblock_header (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tgg00_MessBufHeader   &  mbuf_header,
    tgg00_TransContextPtr     trans_ptr);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01messblock (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tgg00_MessBlock       &  m);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01messmaxstack (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tgg00_MessType_Param     mess_type,
    tsp00_Int4               int4);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01messtype (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tgg00_MessType_Param     mess_type);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01mess2type (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tgg00_MessType2_Param     mess2_type);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01minbuf (
    pasbool                  min_wanted);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01moveobj (
    tgg00_Debug_Param        debug,
    const void*              moveobj,
    tsp00_Int4               startpos,
    tsp00_Int4               endpos);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01msgcheck (
    const char*              msg,
    pasbool                  check_condition,
    tsp00_Int4               bad_int);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01multiswitch (
    const char*              s20,
    const char*              s201);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01mxerrmsg (
    const char*              nam,
    tsp00_Int4               int_wrong,
    tsp00_Int4               int_ok);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01name (
    tgg00_Debug_Param        debug,
    const char*              nam);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01name1 (
    tgg00_Debug_Param        debug,
    const char*              nam,
    const char*              n);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01op (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tgg00_StackOpType_Param     op);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01op_func (
    tgg00_Debug_Param        debug,
    tsp00_Sname           &  nam,
    tgg00_StackOpFunc_Param     op);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01opmsg (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tsp00_Opmsg           &  msg);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01page (
    tgg00_Debug_Param        debug,
    void*                    buf,
    tsp00_Int4               startpos,
    tsp00_Int4               endpos);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01page_ref (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tgg91_PageRef         &  PageRef);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01page_type (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tgg00_PageType_Param     pagetype,
    tgg00_PageType2_Param     pagetype2);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01p2int4 (
    tgg00_Debug_Param        debug,
    const char*              nam_1,
    tsp00_Int4               int_1,
    const char*              nam_2,
    tsp00_Int4               int_2);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01p4int4 (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tsp00_Int4               int_1,
    tsp00_Int4               int_2,
    tsp00_Int4               int_3,
    tsp00_Int4               int_4);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01stqual (
    tgg00_Debug_Param        debug,
    tgg00_QualBuf         &  qual,
    tgg00_StackListPtr       stack_addr);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01qual (
    tgg00_Debug_Param        debug,
    tgg00_QualBuf         &  part1);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01qual1 (
    tgg00_Debug_Param        debug,
    tsp00_Sname           &  nam,
    tgg00_QualBuf         &  part1);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01real (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tsp00_Longreal           re,
    integer                  digits);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01separator (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tgg00_RecPtr             separator);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void ta01Session (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tgg91_SessionNo       &  session);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void ta01SessionTrans (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tgg91_SessionNo       &  session,
    tgg91_TransNo         &  trans,
    tgg91_TransNo         &  write_trans,
    tgg00_SubtransNo      &  subtrans);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01setmaxbuflength (
    tsp00_Int4               max_len);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01SetSingleUser ();

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01sinit ();

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01site (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tgg00_ServerdbNo      &  site);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01sname (
    tgg00_Debug_Param        debug,
    const char*              nam);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01stackentry (
    tgg00_Debug_Param        debug,
    tgg00_StackEntry      &  st,
    integer                  entry_index);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01statistics (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tgg00_SampleInfo      &  info);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01stdesc (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tgg00_StackDesc       &  stack_desc);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01stackdesc (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tgg00_StackListPtr       stack_addr,
    tgg00_StackDesc       &  stack_desc);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01str30 (
    tgg00_Debug_Param        debug,
    const char*              str30);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void ta01Subtrans (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tgg00_SubtransNo      &  SubtransNo);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01surrogate (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tgg00_Surrogate       &  tabid);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01RestoreSwitchAdjustments ();

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01SaveSwitchAdjustments (
    pasbool                  turnOff);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01tabid (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tgg00_Surrogate       &  tabid);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool t01trace (
    tgg00_Debug_Param        debug);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01trans (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tgg00_TransContext    &  t);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void ta01TransNo (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tgg91_TransNo         &  TransNo);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01treeid (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tgg00_FileId          &  treeid);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01valueto_c64 (
    tsp00_C64             &  s,
    integer                  pos,
    tsp00_Int2               val2,
    tsp00_Int4               val4,
    pasbool                  is_int2);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01vffn (
    tgg00_Debug_Param        debug,
    const char*              nam,
    const char*              fn);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01vftype (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tsp00_VfType_Param       ft);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01warningset (
    tgg00_Debug_Param        debug,
    const char*              nam,
    tsp00_WarningSet      &  warn);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t01write_line (
    const char*              ln);

/* Don't edit this file by hand, it is generated by ptocProto.py */


#endif
