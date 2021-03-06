/*!******************************************************************************

  @file         hbd30.h

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

#ifndef HBD30_H
#define HBD30_H


/*===========================================================================*
 *     RefType: C++                                                          *
 *===========================================================================*/

/*===========================================================================*
 *     MD5: '\xe3\x26\x76\x08\xba\x33\x5e\xe7\x01\xb2\x06\x89\x53\x40\x7f\xcf'
 *===========================================================================*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "ggg00.h"
#include "gbd00.h"


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externPascal void b30cappend_to_tree (
    tgg00_Lkey            &  rk,
    tgg00_Rec             &  b,
    tgg00_FilePos         &  tree_pos,
    tbd_current_tree      &  current);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b30cget_result_leaf (
    tgg00_Lkey            &  rk,
    pasbool                  get_next,
    pasbool                  prepare_for_upd,
    tgg00_FilePos         &  tree_pos,
    tbd_node_ptrs         &  nptr,
    tbd_current_tree      &  current);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b30cnext_from_tree (
    tgg00_Lkey            &  rk,
    tgg00_BdSetResultRecord  &  set_result,
    tgg00_FilePos         &  tree_pos,
    void                  *  b,
    tbd_current_tree      &  current);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b30cprev_from_tree (
    tgg00_Lkey            &  rk,
    tgg00_BdSetResultRecord  &  set_result,
    tgg00_FilePos         &  tree_pos,
    void                  *  b,
    tbd_current_tree      &  current);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd30CheckDatabase (
    tgg00_TransContext    &  t,
    tsp00_Int4            &  bad_index_cnt,
    tsp00_Int4            &  total_bad_cnt,
    tgg00_BasisError      &  verify_error,
    pasbool                  bWithUpdate,
    pasbool                  bWithExtendedCheck,
    pasbool                  bWithIndexes,
    pasbool                  bWithDeleteUnreferencedBlobs,
    integer                  max_server);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b30fdir_to_treeid (
    tbd_fileinfo          &  fi,
    tgg00_FileId          &  file_id);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b30init_load (
    tsp00_PageNo          &  rightmost_leaf,
    tbd_current_tree      &  current);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b30load_table (
    tbd_node_ptrs         &  lnptr,
    tbd_node_ptrs         &  nptr,
    pasbool                  only_left,
    tbd_current_tree      &  current);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b30new_tree (
    tbd_fileinfo          &  fi,
    tbd_current_tree      &  current);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd30AddFileState (
    tbd_current_tree      &  current,
    tbd_file_state_set    &  fileState);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd30AddToTempTree (
    pasbool                  bCountDuplicates,
    tgg00_Rec             &  rec,
    tbd_current_tree      &  current);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd30AddToTree (
    tgg00_Lkey            &  recKey,
    tgg00_Rec             &  rec,
    tbd_current_tree      &  current);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd30AddToTreeWithSpecialKey (
    tgg00_Lkey            &  recKey,
    tgg00_Rec             &  rec,
    tbd_current_tree      &  current);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd30AddToTreeWithBeforeImage (
    tgg00_Lkey            &  recKey,
    tgg00_Rec             &  rec,
    tgg00_MessBlock       &  m,
    tsp00_Addr               pInsertCmd,
    tgg00_LockReqMode_Param     grantedLock,
    tbd_current_tree      &  current);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd30AddToTreeWithBeforeImageAndSpecialKey (
    tgg00_Lkey            &  recKey,
    tgg00_Rec             &  rec,
    tgg00_MessBlock       &  m,
    tsp00_Addr               pInsertCmd,
    tgg00_LockReqMode_Param     grantedLock,
    tbd_current_tree      &  current);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd30BadFile (
    tgg00_TransContext    &  trans,
    tgg00_FileId          &  fileId);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd30BuildCurrent (
    tgg00_TransContext    &  trans,
    tgg00_FileId          &  fileId,
    tbd_current_tree      &  current);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd30CheckTree (
    tgg00_TransContext    &  trans,
    tgg00_FileId          &  fileId,
    tbd00_TreeStatistic   &  treeStatistic,
    tsp00_Int2               longColCnt,
    tsp00_Int2               newLongColCnt,
    pasbool                  bWithExtendedCheck,
    pasbool                  bCheckLongExistence,
    pasbool                  bQuiet,
    pasbool                  bUpdateLeaves);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd30DelFromTree (
    tgg00_Lkey            &  recKey,
    tbd_current_tree      &  current);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd30DelGarbageFromTree (
    tgg00_Lkey            &  recKey,
    tbd_current_tree      &  current,
    tgg91_PageRef         &  wantedRef);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd30DelFromTreeWithBeforeImage (
    tgg00_Lkey            &  recKey,
    tgg00_Rec             &  oldRec,
    tgg00_MessBlock       &  m,
    tsp00_Addr               pDeleteCmd,
    tgg00_LockReqMode_Param     grantedLock,
    tbd_current_tree      &  current);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd30DropTree (
    tbd_current_tree      &  current,
    integer                  longColumnCount,
    pasbool                  bOnlyEmpty);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd30ExistRecord (
    tgg00_Lkey            &  recKey,
    tbd_current_tree      &  current);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd30FindLeaf (
    tgg00_Lkey            &  key,
    tbd_current_tree      &  current);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd30GetFromTree (
    tgg00_Lkey            &  recKey,
    tgg00_Rec             &  rec,
    tbd_current_tree      &  current,
    tgg00_LockReqMode_Param     wantedLock);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd30GetFromTreeWithTreepos (
    tgg00_Lkey            &  recKey,
    tgg00_Rec             &  rec,
    tgg00_FilePos         &  tree_pos,
    tbd_current_tree      &  current,
    tgg00_LockReqMode_Param     wantedLock);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd30GetNextLeafPage (
    tsp00_PageNo          &  nextLeafPage,
    tbd_node_ptrs         &  nptr,
    tbd_current_tree      &  current);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd30GetNextLong (
    tgg00_Lkey            &  recKey,
    integer                  numberOfLongs,
    integer               &  currentLong,
    tgg00_FileId          &  longFileId,
    tgg00_FilePos         &  filePos,
    tbd_current_tree      &  current);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd30GetTree (
    tgg00_TransContext    &  trans,
    tgg00_FileId          &  fileId,
    tbd_current_tree      &  current,
    tgg00_MessType_Param     messType,
    pasbool                  bLockTreeExcl,
    pasbool                  bSynchronizeTreeLock);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd30GetSubTree (
    tbd_current_tree      &  current,
    tsp00_PageNo             indexPageNo);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd30MarkDeletedFromTree (
    tgg00_Lkey            &  recKey,
    tbd_current_tree      &  current,
    pasbool                  markForDelete,
    tgg91_PageRef         &  beforeRef,
    tgg91_TransNo         &  updTrans,
    tgg00_RecPtr             pRec);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd30NextFromTree (
    tgg00_Lkey            &  recKey,
    pasbool                  bInclusive,
    pasbool                  bWithSQLLock,
    tgg00_Rec             &  rec,
    tbd_current_tree      &  current);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd30NewTree (
    tgg00_TransContext    &  trans,
    tgg00_FileId          &  fileId,
    tbd_current_tree      &  current,
    pasbool                  bLockTreeExcl,
    pasbool                  bSynchronizeTreeLock);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd30PrevFromTree (
    tgg00_Lkey            &  recKey,
    pasbool                  bInclusive,
    pasbool                  bWithSQLLock,
    tgg00_Rec             &  rec,
    tbd_current_tree      &  current);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd30ReleaseTree (
    tbd_current_tree      &  current);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd30ReleaseSubTree (
    tbd_current_tree      &  current);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd30ReplaceInTree (
    tgg00_Lkey            &  recKey,
    tgg00_Rec             &  rec,
    tbd_current_tree      &  current,
    tgg91_PageRef         &  beforeRef,
    tgg91_TransNo         &  updTrans);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd30ReplaceInTreeWithTreepos (
    tgg00_Rec             &  rec,
    tgg00_FilePos         &  tree_pos,
    tbd_current_tree      &  current);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd30ReplaceInTreeWithBeforeImage (
    tgg00_Lkey            &  recKey,
    tgg00_Rec             &  oldRec,
    tgg00_Rec             &  newRec,
    tgg00_MessBlock       &  m,
    tsp00_Addr               pUpdateCmd,
    tgg00_LockReqMode_Param     grantedLock,
    tbd_current_tree      &  current);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd30ResetBadFile (
    tgg00_TransContext    &  trans,
    tgg00_FileId          &  fileId);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd30SearchRecord (
    tbd_current_tree      &  current,
    tsp00_KeyPtr             recKey,
    tsp00_Int4               recKeyLen,
    pasbool                  bLockLeafExcl,
    tbd_node_ptrs         &  nptr,
    tsp00_Int4            &  recIndex,
    tbd_neighbors         &  neighbs,
    tbd_searchresult      &  searchResult);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd30SetReadOnly (
    tgg00_TransContext    &  trans,
    tgg00_FileId          &  fileId);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd30SetFileVersion (
    tbd_current_tree      &  current,
    tgg91_FileVersion        newfileVersion);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd30SubFileState (
    tbd_current_tree      &  current,
    tbd_file_state_set    &  fileState);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd30VerifyTree (
    tgg00_TransContext    &  trans,
    tgg00_FileId          &  fileId,
    tbd00_TreeStatistic   &  treeStatistic,
    pasbool                  bUpdateConverter,
    pasbool                  bWithExtendedCheck);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd30InitFileIdForBlobFileIds ();

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd30AddBlobIdFoundInFiledirectory (
    tgg00_TransContext    &  trans,
    tgg00_Surrogate       &  BlobId);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd30AddBlobIdFoundInTable (
    tgg00_TransContext    &  trans,
    tgg00_Surrogate       &  BlobId);

/* Don't edit this file by hand, it is generated by ptocProto.py */


#endif

