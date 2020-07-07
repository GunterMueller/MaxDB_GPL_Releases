/*!******************************************************************************

  @file         hbd31.h

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

#ifndef HBD31_H
#define HBD31_H


/*===========================================================================*
 *     RefType: C++                                                          *
 *===========================================================================*/

/*===========================================================================*
 *     MD5: '\x73\x72\xf7\x68\xf5\x7b\xe9\xa7\x72\x14\xb3\x77\x23\x87\x1d\xca'
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

externPascal void b31add_to_leaf (
    tgg00_Rec             &  b,
    tbd_node_ptrs         &  nptr,
    integer                  index,
    tsp00_PageNo             left,
    tbd_current_tree      &  current);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd31AddToLeaf (
    tgg00_Rec             &  b,
    tbd_node_ptrs         &  nptr,
    integer                  index,
    tsp00_PageNo             left,
    tbd_current_tree      &  current,
    pasbool                  useBeforeRef,
    tgg91_PageRef         &  beforeRef);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd31AppendToLeaf (
    tgg00_Rec             &  b,
    tbd_node_ptrs         &  nptr,
    integer                  index,
    tsp00_PageNo             left,
    tbd_current_tree      &  current,
    pasbool                  useBeforeRef,
    tgg91_PageRef         &  beforeRef);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b31del_from_leaf (
    tbd_node_ptrs         &  nptr,
    integer                  index,
    tsp00_PageNo             left,
    tbd_current_tree      &  current);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b31get_from_leaf (
    tbd_node_ptrs         &  nptr,
    integer                  index,
    pasbool                  with_kb_lock,
    tgg00_Rec             &  b,
    tbd_current_tree      &  current);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b31next_search (
    tbd_node_ptrs         &  nptr,
    integer               &  index,
    tbd_current_tree      &  current,
    tbd_lru_info_Param       lruInfo);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b31nfill_recordbuffer (
    tbd_node_ptrs         &  nptr,
    tgg00_Lkey            &  test_key,
    tgg00_FilePos         &  tree_pos,
    tgg00_BdSetResultRecord  &  set_result,
    void                  *  resultBuffer,
    tbd_current_tree      &  current);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b31pfill_recordbuffer (
    tbd_node_ptrs         &  nptr,
    tgg00_Lkey            &  test_key,
    tgg00_FilePos         &  tree_pos,
    tgg00_BdSetResultRecord  &  set_result,
    void                  *  resultBuffer,
    tbd_current_tree      &  current);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b31pointer_list (
    tbd_nodeptr           &  nptr);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd31PrevSearch (
    tsp00_KeyPtr             recKey,
    tsp00_Int4               keyLen,
    tbd_node_ptrs         &  pNodes,
    integer               &  recIndex,
    tsp00_PageNo          &  prevLeafPage,
    tbd_current_tree      &  current,
    pasbool                  bGetLeafForUpdate);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b31repl_in_leaf (
    tgg00_Rec             &  b,
    tbd_node_ptrs         &  nptr,
    integer                  index,
    tsp00_PageNo             left,
    tbd_current_tree      &  current);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd31ReplaceInLeaf (
    tgg00_Rec             &  b,
    tbd_node_ptrs         &  nptr,
    integer                  index,
    tsp00_PageNo             left,
    tbd_current_tree      &  current,
    tgg91_PageRef         &  beforeRef,
    tgg91_TransNo         &  updTrans);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b31search_entry (
    tbd_current_tree      &  current,
    tsp00_Key             &  rk,
    tsp00_Int4               KeyLen,
    tbd_nodeptr           &  nptr,
    tsp00_Int4            &  index,
    tbd_searchresult      &  result);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b31sort_entries (
    tbd_nodeptr           &  nptr,
    tgg00_TransContextPtr     t);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b31t_append_to_temp_leaf (
    tgg00_Rec             &  b,
    tbd_node_ptrs         &  nptr,
    tgg00_FilePos         &  tree_pos,
    tbd_current_tree      &  current);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b31t_prev_temp_search (
    tbd_node_ptrs         &  nptr,
    integer               &  index,
    tbd_current_tree      &  current);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b31w_next_search (
    tbd_node_ptrs         &  nptr,
    integer               &  index,
    tbd_current_tree      &  current);

/* Don't edit this file by hand, it is generated by ptocProto.py */


#endif

