/*!******************************************************************************

  @file         hkb50.h

  ------------------------------------------------------------------------------

  @author       UweH

  @brief        KB_trans_state

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

#ifndef HKB50_H
#define HKB50_H


/*===========================================================================*
 *     RefType: C++                                                          *
 *===========================================================================*/

/*===========================================================================*
 *     MD5: '\xe6\x5b\xe2\xaf\x96\xc2\xde\x17\xed\xb4\x96\xc9\x09\x74\x7a\x89'
 *===========================================================================*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "ggg00.h"
#include "gkb05.h"
#include "ggg92.h"


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externPascal void k50aux_fn_no (
    tgg00_TransIndex         TransIndex,
    pasbool                  IsPerm,
    tsp00_Int4            &  AuxiliaryFnNo);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void kb50CheckActiveTrans (
    tsp00_TaskId             TaskId,
    tgg91_TransNo         &  WriteTransId,
    tgg00_TransIndex         TransIndex,
    pasbool               &  IsActive);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k50check_excl_user (
    tgg00_TransContext    &  Trans,
    tgg00_UserId          &  UserId);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void kb50CheckObjSelfLocked (
    tgg00_TransContext    &  Trans,
    tgg92_KernelOid&         Oid,
    tgg00_ObjTransInfo    &  ObjTransInfo,
    pasbool                  bIsShareLocked);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void kb50EnterTransRegion (
    tsp00_TaskId             taskid,
    tgg00_TransIndex         transindex);

/* Don't edit this file by hand, it is generated by ptocProto.py */

class FileDir_IDirectory;

externPascal void kb50GetLocalFileDir (
    tgg00_TransIndex         TransIndex,
    FileDir_IDirectory*&     filedir);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void kb50LeaveTransRegion (
    tsp00_TaskId             taskid,
    tgg00_TransIndex         transindex);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k50eot_check (
    tgg00_TransContext    &  t,
    tgg00_MessType_Param     MessType,
    tgg00_MessType2_Param     MessType2);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k50eot_new_trans (
    tgg00_TransContext    &  t);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k50excl_scan (
    tsp00_TaskId             TaskId,
    pasbool               &  IsEotExcl);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k50get_temp_log (
    tgg00_TransIndex         TransIndex,
    tsp00_PageNo          &  LogFileRoot,
    tsp00_Int4            &  LogCnt,
    tsp00_Int4            &  FunctRollb,
    pasbool               &  AuxFileExists);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal tsp00_Addr k50GetLogTransaction (
    tgg00_TransIndex         TransIndex);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void kb50LockObj (
    tgg00_TransContext    &  Trans,
    tsp00_C8              &  ObjFileNo,
    tgg92_KernelOid&         Oid,
    tgg00_ObjTransInfo    &  ObjTransInfo,
    pasbool                  ignShrLckCsView);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void kb50LockShareObj (
    tgg00_TransContext    &  Trans,
    tsp00_C8              &  ObjFileNo,
    tgg92_KernelOid&         Oid,
    tgg00_ObjTransInfo    &  ObjTransInfo);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k50my_state (
    tgg00_TransIndex         TransIndex,
    pasbool               &  IsEotExcl,
    pasbool               &  AuxPermExists,
    tsp00_TaskId          &  TransTaskId);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k50new_write_trans (
    tgg00_TransContext    &  Trans);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k50not_connected (
    tgg00_MessBlock       &  m);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void kb50ObjConsistentCheck (
    tgg00_TransContext    &  Trans,
    tgg91_TransNo         &  UpdTransId,
    pasbool               &  bConsistent,
    pasbool               &  bCancelled);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool k50oms_have_obj_locks (
    tgg00_TransIndex         TransIndex);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k50put_temp_log (
    tgg00_TransIndex         TransIndex,
    tgg00_TransState      &  TransState,
    tsp00_PageNo             LogFileRoot,
    tsp00_Int4               LogCnt,
    pasbool                  AuxFileExists);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k50RegisterTransaction (
    tgg00_TransIndex         TransIndex,
    void*                    LocalTransContext);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k50SatisfyNextWaitingObjRequest (
    tkb05_ObjCollGlob     &  ObjCollGlob,
    tkb05_ObjCollisionEntry  &  CollEntry,
    tgg91_TransNo         &  UpdTransId,
    pasbool                  bOnlyIfShareReq);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k50set_redo_process_id (
    tsp00_TaskId             TaskId,
    tgg00_TransIndex         TransIndex);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k50StartDbProcedure (
    tgg00_TransContext    &  t);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k50transindex_check (
    tgg00_TransContext    &  t);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void kb50UnlockObj (
    tsp00_TaskId             TaskId,
    tgg91_TransNo         &  WriteTransId,
    tgg00_TransIndex         TransIndex,
    tgg92_KernelOid&         Oid,
    tgg00_ObjTransInfo    &  ObjTransInfo);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void kb50UnlockShareObj (
    tsp00_TaskId             TaskId,
    tgg91_TransNo         &  WriteTransId,
    tgg00_TransIndex         TransIndex,
    tgg92_KernelOid&         Oid,
    tgg00_ObjTransInfo    &  ObjTransInfo);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void kb50WaitAndRetryObjLock (
    tgg00_TransContext    &  Trans,
    tgg00_MessType_Param     MessType,
    tgg00_MessType2_Param     MessType2);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void kb50XxxObjTrace (
    tgg00_TransContext    &  Trans,
    const char*              msg,
    tgg92_KernelOid&         oid,
    tgg91_TransNo         &  UpdTransId,
    tgg91_PageRef         &  BeforeRef,
    tgg00_ObjState_Param     ObjState);

/* Don't edit this file by hand, it is generated by ptocProto.py */


#endif

