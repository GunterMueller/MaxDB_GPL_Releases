/*!******************************************************************************

  @file         hbd60.h

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

#ifndef HBD60_H
#define HBD60_H


/*===========================================================================*
 *     RefType: Both                                                         *
 *===========================================================================*/

/*===========================================================================*
 *     MD5: '\x7e\x7a\x4f\x37\xa5\xc8\x46\x39\xdd\x6d\xe3\x84\xb1\xfa\x70\x25'
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

class Trans_Context;

externPascal void bd60ContPutLeaves (
    Trans_Context         VAR_VALUE_REF  trans,
    tbd_current_tree      VAR_VALUE_REF  current,
    tbd_node_ptrs         VAR_VALUE_REF  nptr,
    const tsp00_Int4                     pos,
    tsp00_Int4                           length,
    const tsp00_Byte* const                 b,
    const tsp00_Int4                     bufferSize,
    bool&                                firstcall,
    const bool                           extended,
    tsp00_Int4            VAR_VALUE_REF  filelen);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd60Extend (
    Trans_Context         VAR_VALUE_REF  trans,
    tbd_current_tree      VAR_VALUE_REF  current,
    tbd_node_ptrs         VAR_VALUE_REF  nptr,
    const tsp00_Int4                     pos,
    tsp00_Int4                           length,
    const bool                           use_buf,
    const char                           fill_char,
    const tsp00_Int4                     buf_size,
    const tsp00_Byte*                    buf_addr,
    tsp00_Int4                           bufpos);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd60Get (
    Trans_Context         VAR_VALUE_REF  trans,
    tbd_current_tree      VAR_VALUE_REF  current,
    tbd_node_ptrs         VAR_VALUE_REF  nptr,
    tsp00_Int4                           pos,
    tsp00_Int4                           length,
    tsp00_Int4                           buf_size,
    tsp00_Byte* const                    buf_addr,
    tsp00_Int4                           bufpos);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd60PutIntoLeaves (
    Trans_Context         VAR_VALUE_REF  trans,
    tbd_current_tree      VAR_VALUE_REF  current,
    tbd_node_ptrs         VAR_VALUE_REF  nptr,
    tsp00_Int4                           pos,
    tsp00_Int4                           length,
    const bool                           use_buf,
    const char                           fill_char,
    const tsp00_Int4                     buf_size,
    const tsp00_Byte* const                 buf_addr,
    tsp00_Int4                           bufpos,
    const bool                           extended);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd60NewLob (
    tbd_current_tree      VAR_VALUE_REF  current);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd60Read (
    Trans_Context         VAR_VALUE_REF  trans,
    const tbd_node* const                 nptr,
    tsp00_Int4                           pos,
    const tsp00_Int4                     length,
    const tsp00_Int4                     buf_size,
    tsp00_Byte* const                    buf_addr,
    const tsp00_Int4                     buf_pos);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd60Drop (
    Trans_Context         VAR_VALUE_REF  trans,
    tbd_current_tree      VAR_VALUE_REF  current);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd60ExtendRoot (
    Trans_Context         VAR_VALUE_REF  trans,
    tbd_current_tree      VAR_VALUE_REF  current,
    tbd_node_ptrs         VAR_VALUE_REF  nptr,
    tsp00_Int4                           pos,
    tsp00_Int4                           length,
    const bool                           use_buf,
    const char                           fill_char,
    const tsp00_Int4                     buf_size,
    const tsp00_Byte* const                 buf_addr,
    tsp00_Int4                           bufpos);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd60PutIntoRoot (
    Trans_Context         VAR_VALUE_REF  trans,
    tbd_current_tree      VAR_VALUE_REF  current,
    tbd_node_ptrs         VAR_VALUE_REF  nptr,
    tsp00_Int4                           pos,
    tsp00_Int4                           length,
    const bool                           use_buf,
    const char                           fill_char,
    const tsp00_Int4                     buf_size,
    const tsp00_Byte* const                 buf_addr,
    const tsp00_Int4                     bufpos,
    const bool                           extended);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd60CheckLob (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    tgg00_FileId          VAR_VALUE_REF  fileId,
    const tgg00_Surrogate&                 parentTabId,
    const bool                           bModifyLobCounter,
    const bool                           bUpdateLeaves);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd60GetNextByteLeafPage (
    tsp00_PageNo          VAR_VALUE_REF  nextLeafPage,
    tbd_node_ptrs         VAR_VALUE_REF  nptr,
    tbd_current_tree      VAR_VALUE_REF  current);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd60VerifyLob (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    tgg00_FileId          VAR_VALUE_REF  fileId,
    const bool                           bUpdateConverter,
    tsp00_Int4            VAR_VALUE_REF  numPages);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd60Extract (
    Trans_Context         VAR_VALUE_REF  trans,
    tbd_current_tree      VAR_VALUE_REF  current,
    tsp00_VFilename       VAR_ARRAY_REF  hostFileName);

/* Don't edit this file by hand, it is generated by ptocProto.py */


#endif

