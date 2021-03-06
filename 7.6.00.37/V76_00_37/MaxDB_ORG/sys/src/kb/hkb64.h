/*!******************************************************************************

  @file         hkb64.h

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

#ifndef HKB64_H
#define HKB64_H


/*===========================================================================*
 *     RefType: C++                                                          *
 *===========================================================================*/

/*===========================================================================*
 *     MD5: '\xc6\x87\xf4\x65\x08\xe9\x72\x00\x61\x88\xaf\x56\x5c\x1c\x5a\x76'
 *===========================================================================*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "ggg00.h"


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externPascal void k64begin_load (
    tgg00_MessBlock       &  m);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k64build_aux_fn_prefix (
    tgg91_TransNo         &  transid,
    pasbool                  is_perm,
    tgg00_FileId          &  aux_id,
    integer               &  prefix_len);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k64change_filetype (
    tgg00_MessBlock       &  m);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k64cmd_drop (
    tgg00_MessBlock       &  m);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k64create_bd_file (
    tgg00_TransContext    &  t,
    tgg00_FileId          &  tree_id,
    tgg00_MessType_Param     mess_type,
    tgg00_MessType2_Param     mess2_type,
    tgg00_ExtendedFiletypeSet  &  eFiletypeSet,
    tsp00_Int2               str_col_cnt);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k64CreateTable (
    tgg00_TransContext    &  t,
    tgg00_FileId          &  fileId,
    tgg00_MessType2_Param     mess2_type,
    tgg00_ExtendedFiletypeSet  &  eFiletypeSet,
    tsp00_Int2               str_col_cnt,
    pasbool                  createShortColFile);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k64drop_tree (
    tgg00_TransContext    &  t,
    tgg00_FileId          &  file_id);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k64drop_lob_tree (
    tgg00_TransContext    &  t,
    const tgg00_Surrogate&     tableid,
    tgg00_FileId          &  file_id);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k64end_load (
    tgg00_MessBlock       &  m);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k64rename_lob_file (
    tgg00_TransContext    &  t,
    const tgg00_Surrogate&     tableid,
    tgg00_FileId          &  source_id,
    tgg00_FileId          &  target_id);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k64set_file_version (
    tgg00_MessBlock       &  m);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k64set_treeleafnodes (
    tgg00_TransContext    &  t,
    tgg00_FileId          &  fileId);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k64short_col_file_create (
    tgg00_TransContext    &  t,
    tgg00_FileId          &  tree_id);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k64short_col_file_drop (
    tgg00_TransContext    &  t,
    tgg00_FileId          &  tree_id);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k64table_create (
    tgg00_MessBlock       &  m);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k64temp_file_handling (
    tgg00_MessBlock       &  m);

/* Don't edit this file by hand, it is generated by ptocProto.py */


#endif

