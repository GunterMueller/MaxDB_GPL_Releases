/*!******************************************************************************

  @file         hbd10.h

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

#ifndef HBD10_H
#define HBD10_H


/*===========================================================================*
 *     RefType: C++                                                          *
 *===========================================================================*/

/*===========================================================================*
 *     MD5: '\xff\x88\x22\x4d\xf7\x57\x26\x85\xd5\x10\x2b\xbb\x89\xd1\x6d\x87'
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

externPascal pasbool bd10IsConnectAllowed ();

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool bd10IsDbFull (
    tsp00_TaskId             TaskId);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal tsp00_Int4 bd10ConverterSize ();

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd10EndSave (
    tsp00_TaskId             TaskId,
    pasbool                  bBackUpSuccessful);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal tsp00_Int4 b10bup_data_page_cnt (
    tsp00_TaskId             pid);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal tsp00_Int4 bd10conv_page_cnt ();

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b10create_converter (
    tsp00_TaskId             TaskId,
    tsp00_Int4               NumAllPages);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b10create_converter_for_restore (
    tsp00_TaskId             TaskId,
    tsp00_Int4               ConverterVersion,
    tsp00_Int4               NumAllPages,
    pasbool                  LastSaveDataSuccessful,
    tkb00_ConfigRestartParam  &  RestartRecord);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd10dbspace_statistic (
    tgg00_TransContext    &  t,
    tgg00_DbSpaceInfo     &  info);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd10EndColdVerify (
    tsp00_TaskId             TaskId,
    tsp00_Int4            &  NumPermPages,
    tsp00_Int4            &  NumReleasedBlocks);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd10EndFailedColdVerify (
    tsp00_TaskId             TaskId);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd10FlushConverterSerial (
    tgg00_TransContext    &  Trans,
    tsp00_Int4            &  ConverterVersion,
    tsp00_PageNo          &  MaxStaticPageNo,
    tsp00_PageNo          &  MaxDynamicPageNo,
    tsp00_Int4            &  PackedConvRootBlockAddress);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd10BeginColdVerify (
    tgg00_TransContext    &  Trans);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b10rc_restart_converter (
    tsp00_TaskId             TaskId,
    tgg00_BasisError      &  TrError,
    tsp00_Int4               ConverterVersion,
    tsp00_Int4               NumAllPages,
    pasbool                  IncrementConvVersion,
    pasbool                  LastSaveDataSuccessful,
    tkb00_ConfigRestartParam  &  RestartParam);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b10restart_converter_for_restore (
    tgg00_TransContext    &  t,
    tsp00_Int4               ConverterVersion,
    tsp00_Int4               NumAllPages,
    pasbool                  LastSaveDataSuccessful,
    tkb00_ConfigRestartParam  &  RestartParam,
    tkb00_ConfigRestartParam  &  OldRestartParam);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd10ShutdownConverter (
    tsp00_TaskId             TaskId);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b10use_pno (
    tgg00_TransContext    &  t,
    tsp00_PageNo             data_pno);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal tsp00_Int4 bd10GetConverterVersion ();

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd10GetConverterBitMapPage (
    tsp00_TaskId             TaskId,
    tsp00_Page            &  Page,
    pasbool               &  bNoMorePages);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd10GetConverterArchivePage (
    tsp00_TaskId             TaskId,
    tsp00_Page            &  Page,
    pasbool               &  bNoMorePages);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd10RemoveConverterBitMapPages ();

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd10RemoveConverterArchivePages ();

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd10RestoreUsedPageNos (
    tsp00_TaskId             TaskId,
    tsp00_PageAddr           pPage);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd10RestoreConverter (
    tsp00_TaskId             TaskId,
    tsp00_PageAddr           pPage);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd10FinishRestoreUsedPageNos (
    tsp00_TaskId             TaskId);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd10CheckConverterPage (
    tsp00_Page            &  Page,
    pasbool               &  bPageIsDamaged);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd10CheckConverterBitMapPage (
    tsp00_Page            &  Page,
    pasbool               &  bPageIsDamaged);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd10GetConvEntry (
    tsp00_PageAddr           pPage,
    tsp00_Int4               EntryNo,
    pasbool                  IsLeaf,
    tsp00_Int4            &  DevNo,
    tsp00_Int4            &  BlockNo,
    pasbool               &  IsMarked,
    pasbool               &  IsPending);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd10GetPageHeader (
    tsp00_PageAddr           pPage,
    tsp00_Int4            &  Version,
    tsp00_Int4            &  DevNo,
    tsp00_Int4            &  BlockNo,
    tsp00_Int4            &  BaseNo,
    tsp00_Int4            &  TotalCnt,
    tsp00_Int4            &  ParentIndex,
    tsp00_Int4            &  PageEntryNo,
    pasbool               &  IsLeaf,
    pasbool               &  IsPerm,
    pasbool               &  IsDynamic);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool bd10CheckBackup (
    tsp00_TaskId             taskId);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal tsp00_Uint1 bd10GetVolumeNoBitCount ();

/* Don't edit this file by hand, it is generated by ptocProto.py */


#endif
