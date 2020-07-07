/*!******************************************************************************

  @file         hkb560.h

  ------------------------------------------------------------------------------

  @author       <unknown>

  @brief        <unknown topic>

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG
 
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

#ifndef HKB560_H
#define HKB560_H


/*===========================================================================*
 *     RefType: Both                                                         *
 *===========================================================================*/

/*===========================================================================*
 *     MD5: '\xbc\x61\x7c\x93\x0b\x85\x86\x63\xe9\x96\x24\xc3\x37\x2f\x7a\xf3'
 *===========================================================================*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "ggg00.h"
#include "gkb00.h"


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externPascal void kb560GetArchiveLogDeviceInfo (
    tsp00_TaskId                         taskid,
    SAPDB_UInt4                          logno,
    pasbool               VAR_VALUE_REF  available,
    SAPDB_UInt4           VAR_VALUE_REF  deviceSize,
    SAPDB_UInt4           VAR_VALUE_REF  pagesUsed,
    SAPDB_UInt4           VAR_VALUE_REF  pagesNotSaved);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void kb560GetArchiveLogQueueInfo (
    tsp00_TaskId                         taskid,
    SAPDB_UInt4                          logno,
    pasbool               VAR_VALUE_REF  available,
    SAPDB_UInt4           VAR_VALUE_REF  queueSize,
    SAPDB_UInt4           VAR_VALUE_REF  maxPagesUsedInQueue,
    SAPDB_UInt4           VAR_VALUE_REF  countReserveRequests,
    SAPDB_UInt4           VAR_VALUE_REF  auxCountWaitForIO,
    SAPDB_UInt4           VAR_VALUE_REF  auxCountGroupCommit,
    SAPDB_UInt4           VAR_VALUE_REF  countWaitForQueueFree,
    SAPDB_UInt4           VAR_VALUE_REF  maxWaitForPageIO);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void kb560GetStatistics (
    tsp00_TaskId                         taskid,
    tkb00_LogDeviceInfo   VAR_VALUE_REF  LogDeviceInfo,
    tkb00_LogQueueInfo    VAR_VALUE_REF  LogQueueInfo);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void kb560ResetStatistics (
    tsp00_TaskId                         taskid);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void kb560GetLogDBIdent (
    tsp00_Line            VAR_ARRAY_REF  dbident);

/* Don't edit this file by hand, it is generated by ptocProto.py */

class Log_IOSequenceNo;

externPascal pasbool kb560GetLogInfo (
    tsp00_TaskId                         taskid,
    tsp00_Int4            VAR_VALUE_REF  date,
    tsp00_Int4            VAR_VALUE_REF  time,
    tsp00_Int4            VAR_VALUE_REF  flushmode,
    tsp00_Int4            VAR_VALUE_REF  devicestate,
    tsp00_Uint4           VAR_VALUE_REF  queuecount,
    tsp00_Uint4           VAR_VALUE_REF  cycleendoffset,
    tsp00_Uint4           VAR_VALUE_REF  oldestnotsavedoffset,
    Log_IOSequenceNo&                    oldestnotsavedsequence,
    tsp00_Uint4           VAR_VALUE_REF  lastknownoffset,
    Log_IOSequenceNo&                    lastknownsequence,
    tsp00_Uint4           VAR_VALUE_REF  backupcount,
    tsp00_Int4            VAR_VALUE_REF  untildate,
    tsp00_Int4            VAR_VALUE_REF  untiltime,
    tsp00_Uint4           VAR_VALUE_REF  untilclearloweroffset,
    tsp00_Uint4           VAR_VALUE_REF  untilclearupperoffset,
    Log_IOSequenceNo&                    untilsequenceafterredo,
    Log_IOSequenceNo&                    savepointsequence,
    tsp00_Uint4           VAR_VALUE_REF  savepointdevoffset,
    tsp00_Uint4           VAR_VALUE_REF  savepointoffset);

/* Don't edit this file by hand, it is generated by ptocProto.py */

class Log_Transaction;

externPascal void kb560DumpTransaction (
    Log_Transaction*                     pTrans,
    void*&                               pData,
    SAPDB_UInt4&                         dataLength);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool kb560VerifyOpenTransAndHistory (
    tgg00_TransContext    VAR_VALUE_REF  Trans,
    pasbool                              isCold);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void kb560CreateHistoryManager (
    tgg00_TransContext    VAR_VALUE_REF  Trans);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void kb560DropHistoryManager ();

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool kb560TransactionIsOpen (
    void*                                TransPtr);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void kb560InitSavepointParam (
    tkb00_SaveptParam     VAR_VALUE_REF  param);

/* Don't edit this file by hand, it is generated by ptocProto.py */

class Trans_Context;

externPascal void kb560StartSavepointAndWait (
    Trans_Context&                       Trans,
    tgg00_MessType2_Param                 MessType2);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void kb560StartSavepoint (
    Trans_Context&                       Trans,
    tgg00_MessType2_Param                 MessType2);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void kb560SavepointCoordinator (
    Trans_Context&                       Trans);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void kb560LogWriter ();

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void kb560AllocateLogVolumeInstance ();

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void kb560CreateLogTransaction (
    tgg00_TransContext    VAR_VALUE_REF  Trans);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void kb560DeleteLogTransaction (
    tgg00_TransContext    VAR_VALUE_REF  Trans);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void kb560CommitLogTransaction (
    tgg00_TransContext    VAR_VALUE_REF  Trans);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void kb560RollbackLogTransAction (
    tgg00_TransContext    VAR_VALUE_REF  Trans,
    tgg00_MessType2_Param                 RollbackKind,
    tgg00_SubtransNo                     SubTransNo);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void kb560BeginSubtransLogTransaction (
    tgg00_TransContext    VAR_VALUE_REF  Trans);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void kb560EndSubtransLogTransaction (
    tgg00_TransContext    VAR_VALUE_REF  Trans);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void kb560GetFirstKnownIOSeqBeforeRestoreLog (
    Log_IOSequenceNo&                    FirstIOSeq,
    pasbool               VAR_VALUE_REF  firstIOSeqIsValid);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool kb560LogIsFull ();

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k560check_logsave_allowed (
    tsp00_TaskId                         TaskId,
    tgg00_BasisError      VAR_VALUE_REF  BasisError);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal tsp00_Int4 kb560GetLogDeviceSize ();

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void kb560SetDeviceStateOkay (
    tsp00_TaskId                         taskid,
    tgg00_BasisError      VAR_VALUE_REF  TrError);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void kb560GetHistFileInfo (
    tgg00_TransContext    VAR_VALUE_REF  Trans,
    tsp00_Int4            VAR_VALUE_REF  HistFileNo,
    tgg91_TransNo         VAR_VALUE_REF  FirstTransId,
    pasbool               VAR_VALUE_REF  FirstTransIsUsed,
    tgg91_TransNo         VAR_VALUE_REF  LastTransId,
    pasbool               VAR_VALUE_REF  LastTransIsUsed,
    tsp00_Int4            VAR_VALUE_REF  PageCount,
    pasbool               VAR_VALUE_REF  bFileIsEmpty);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void kb560GetLogTransactionInfo (
    Log_Transaction*                     ptrans,
    tsp00_PageNo          VAR_VALUE_REF  undoroot,
    tsp00_PageNo          VAR_VALUE_REF  undolast,
    tsp00_Int4            VAR_VALUE_REF  undopagecount,
    tsp00_Int4            VAR_VALUE_REF  lastundosequence);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void kb560InvalidateIOSeq (
    Log_IOSequenceNo&                    ioSeq);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool kb560IsValidIOSeq (
    const Log_IOSequenceNo&                 ioSeq);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool kb560IsSuccessor (
    Log_IOSequenceNo&                    predecessor,
    Log_IOSequenceNo&                    successor);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal tgg00_BasisError kb560ConvertRollbackResultToBasisError (
    tsp00_Int4                           result);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal tgg00_BasisError kb560ConvertWriteRedoEntryResultToBasisError (
    tsp00_Int4                           result);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool k560IsSavepointAllowed ();

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void kb560GetSavepointStatistics (
    tsp00_Int4            VAR_VALUE_REF  lastDurationSec,
    tsp00_Int4            VAR_VALUE_REF  lastDurationMicroSec,
    tsp00_Int4            VAR_VALUE_REF  lastWrittenPages,
    tsp00_Int4            VAR_VALUE_REF  lastIOcount,
    tsp00_Int4            VAR_VALUE_REF  avgWrittenPages,
    tsp00_Int4            VAR_VALUE_REF  avgIOcount);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void kb560GetSavepointReasonString (
    tsp00_Uint1                          reasonid,
    tsp00_C40             VAR_ARRAY_REF  resonstring);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void kb560InsertToWaitForPrepare (
    tsp00_TaskId                         taskid,
    tgg00_WaitContext     VAR_VALUE_REF  waitcontext);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal tsp00_Uint4 kb560LogQueueSize ();

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void kb560SetTraceMaxBufferLength (
    tsp00_Int4                           length);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void kb560GetSys1CatalogTreeId (
    tgg00_FileId          VAR_VALUE_REF  treeId);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void kb560GetSys2CatalogTreeId (
    tgg00_FileId          VAR_VALUE_REF  treeId);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void kb560GetSys1CatalogTabId (
    tgg00_Surrogate       VAR_ARRAY_REF  tabId);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void kb560GetSys2CatalogTabId (
    tgg00_Surrogate       VAR_ARRAY_REF  tabId);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool kb560IsSys1CatalogTable (
    tgg00_Surrogate       VAR_ARRAY_REF  tabId);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool kb560IsSys2CatalogTable (
    tgg00_Surrogate       VAR_ARRAY_REF  tabId);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool kb560IsCatalogTable (
    tgg00_Surrogate       VAR_ARRAY_REF  tabId);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void kb560SetSys1CatalogTreeId (
    tgg00_FileId          VAR_VALUE_REF  treeId);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void kb560SetSys2CatalogTreeId (
    tgg00_FileId          VAR_VALUE_REF  treeId);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void kb560DetermineLocalRedoBufferSizeForTransactions ();

/* Don't edit this file by hand, it is generated by ptocProto.py */


#endif

