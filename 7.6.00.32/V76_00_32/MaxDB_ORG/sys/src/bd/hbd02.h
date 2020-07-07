/*!******************************************************************************

  @file         hbd02.h

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

#ifndef HBD02_H
#define HBD02_H


/*===========================================================================*
 *     RefType: C++                                                          *
 *===========================================================================*/

/*===========================================================================*
 *     MD5: '\xd1\x87\x57\xd3\x8c\x32\x7e\xd2\xa9\xd8\x36\xb0\xa4\x4e\xde\x90'
 *===========================================================================*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "ggg00.h"


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externPascal void b02add_record (
    tgg00_TransContext    &  t,
    tgg00_FileId          &  file_id,
    const tgg00_Rec       &  b);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd02AddRecordWithSpecialKey (
    tgg00_TransContext    &  trans,
    tgg00_FileId          &  fileId,
    tgg00_Rec             &  rec);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b02calculate_page_count (
    tgg00_TransContext    &  t,
    tgg00_FileId          &  file_id,
    tgg00_Lkey            &  startkey,
    tgg00_Lkey            &  stopkey,
    tsp00_Int4            &  page_count);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b02del_record (
    tgg00_TransContext    &  t,
    tgg00_FileId          &  file_id,
    const tgg00_Lkey      &  rk);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b02DelGarbage (
    tgg00_TransContext    &  t,
    tgg00_FileId          &  file_id,
    const tgg00_Lkey      &  rk,
    const tgg91_PageRef   &  wantedRef);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b02eval_page_count (
    tgg00_TransContext    &  t,
    tgg00_FileId          &  file_id,
    tgg00_Lkey            &  startkey,
    tgg00_Lkey            &  stopkey,
    pasbool                  count_records,
    tsp00_Int4            &  page_count,
    tsp00_Int4            &  all_page_cnt,
    tsp00_Int4            &  record_cnt);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b02exists_record (
    tgg00_TransContext    &  t,
    tgg00_FileId          &  file_id,
    tgg00_Lkey            &  rk);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b02find_leaf (
    tgg00_TransContext    &  t,
    tgg00_FileId          &  file_id,
    tgg00_Lkey            &  key);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b02get_lock_record (
    tgg00_TransContext    &  t,
    tgg00_FileId          &  file_id,
    tgg00_Lkey            &  rk,
    tgg00_Rec             &  b);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b02get_record (
    tgg00_TransContext    &  t,
    tgg00_FileId          &  file_id,
    const tgg00_Lkey      &  rk,
    tgg00_Rec             &  b);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b02kb_del_rec (
    tgg00_MessBlock       &  m,
    tsp00_Addr               pDeleteCmd,
    tgg00_Lkey            &  rk,
    tgg00_Rec             &  old_recbuf,
    tgg00_LockReqMode_Param     granted_lock);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b02kb_ins_record (
    tgg00_MessBlock       &  m,
    tsp00_Addr               pInsertCmd,
    tgg00_Rec             &  b,
    tgg00_LockReqMode_Param     granted_lock);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b02kb_select_rec (
    tgg00_TransContext    &  t,
    tgg00_FileId          &  file_id,
    tsp00_KeyPtr             RecKey,
    tsp00_Int2            &  RecKeyLen,
    tsp00_KeyPtr             StopKey,
    tsp00_Int4               StopKeyLen,
    tsp00_Int4               recbuf_size,
    tsp00_MoveObjPtr         recbuf_ptr,
    pasbool                  ignore_vwait,
    tgg00_SelectFieldsParam  &  sel,
    tgg00_StackDesc       &  stack_desc,
    pasbool               &  unqualified,
    tgg00_LockReqMode     &  granted_lock);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b02kb_upd_rec (
    tgg00_MessBlock       &  m,
    tsp00_Addr               pUpdateCmd,
    tgg00_Lkey            &  rk,
    tgg00_Rec             &  old_recbuf,
    tgg00_Rec             &  new_recbuf,
    tgg00_LockReqMode_Param     granted_lock);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b02MarkRecordDeleted (
    tgg00_TransContext    &  t,
    tgg00_FileId          &  file_id,
    pasbool                  isDelete,
    const tgg91_PageRef   &  beforeRef,
    const tgg91_TransNo   &  updTrans,
    const tgg00_Lkey      &  rk,
    tgg00_RecPtr             pRec);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b02next_record (
    tgg00_TransContext    &  t,
    tgg00_FileId          &  file_id,
    tgg00_Lkey            &  rk,
    pasbool                  inclusive,
    tgg00_Rec             &  b);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b02prefetch (
    tgg00_TransContext    &  t,
    tgg00_FileId          &  file_id,
    tsp00_PageNo             pno);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b02prev_record (
    tgg00_TransContext    &  t,
    tgg00_FileId          &  file_id,
    tgg00_Lkey            &  rk,
    pasbool                  inclusive,
    tgg00_Rec             &  b);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b02repl_record (
    tgg00_TransContext    &  t,
    tgg00_FileId          &  file_id,
    const tgg00_Rec       &  b);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b02ReplaceRecord (
    tgg00_TransContext    &  t,
    tgg00_FileId          &  file_id,
    const tgg00_Rec       &  b,
    pasbool                  useTransInfo,
    const tgg91_PageRef   &  beforeRef,
    const tgg91_TransNo   &  updTrans);

/* Don't edit this file by hand, it is generated by ptocProto.py */


#endif
