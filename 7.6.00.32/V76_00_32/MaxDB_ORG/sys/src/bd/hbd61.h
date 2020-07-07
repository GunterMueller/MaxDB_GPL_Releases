/*!******************************************************************************

  @file         hbd61.h

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

#ifndef HBD61_H
#define HBD61_H


/*===========================================================================*
 *     RefType: Both                                                         *
 *===========================================================================*/

/*===========================================================================*
 *     MD5: '\x94\x1c\x20\x74\x05\xb4\xfb\xaa\x24\x59\x32\xc8\x9c\xba\xb2\x95'
 *===========================================================================*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "gbd00.h"


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

class Trans_Context;
class Converter_PageCollector;
#include "DataAccess/Data_PageFrame.hpp"

externPascal void bd61AddIndex (
    Trans_Context         VAR_VALUE_REF  trans,
    tbd_current_tree      VAR_VALUE_REF  current,
    tbd_node_ptrs         VAR_VALUE_REF  pCurrRoots,
    const tsp00_PageNo                   currPageNo,
    const tsp00_PageNo                   newPageNo1,
    const tsp00_PageNo                   newPageNo2,
    const tsp00_Int2                     insertLevel);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd61ChangeContent (
    Trans_Context         VAR_VALUE_REF  trans,
    tbd_node_ptrs         VAR_VALUE_REF  nptr,
    tbd_node_ptrs         VAR_VALUE_REF  rootptr);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd61FirstLoadStep (
    Trans_Context         VAR_VALUE_REF  trans,
    tbd_current_tree      VAR_VALUE_REF  current,
    tbd_node_ptrs         VAR_VALUE_REF  leftNode,
    tbd_node_ptrs         VAR_VALUE_REF  rightNode);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd61FreeGarbage (
    Trans_Context         VAR_VALUE_REF  trans,
    tbd_current_tree      VAR_VALUE_REF  current,
    const tsp00_PageNo                   pageNo,
    const bool                           bTwice);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd61SearchLeaf (
    Trans_Context         VAR_VALUE_REF  trans,
    tbd_current_tree      VAR_VALUE_REF  current,
    tbd_node_ptrs         VAR_VALUE_REF  pNode,
    const tsp00_Int4                     pos,
    tsp00_Int4            VAR_VALUE_REF  leafPos);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd61SearchLeafWithClusteredRead (
    Trans_Context         VAR_VALUE_REF  trans,
    tbd_current_tree      VAR_VALUE_REF  current,
    tbd_node_ptrs         VAR_VALUE_REF  pNode,
    Converter_PageCollector  VAR_VALUE_REF  collector,
    Data_PageFrame        VAR_VALUE_REF  ioBuffer,
    const tsp00_Int4                     maxPagesToRead,
    const tsp00_Int4                     pos,
    tsp00_Int4            VAR_VALUE_REF  leafPos);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd61SearchLeafForUpdate (
    Trans_Context         VAR_VALUE_REF  trans,
    tbd_current_tree      VAR_VALUE_REF  current,
    tbd_node_ptrs         VAR_VALUE_REF  pNode,
    const tsp00_Int4                     pos,
    tsp00_Int4            VAR_VALUE_REF  leafPos,
    bool VAR_VALUE_REF                   bPrevLastPageExist);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd61Trunc (
    Trans_Context         VAR_VALUE_REF  trans,
    tbd_current_tree      VAR_VALUE_REF  current,
    tbd_node_ptrs         VAR_VALUE_REF  pNode,
    tsp00_Int4                           newFileLength,
    const bool                           bDestroy);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd61TruncRoot (
    tbd_current_tree      VAR_VALUE_REF  current,
    tbd_node_ptrs         VAR_VALUE_REF  pNode,
    const tsp00_Int4                     newFileLength);

/* Don't edit this file by hand, it is generated by ptocProto.py */


#endif
