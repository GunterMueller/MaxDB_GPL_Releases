/*!******************************************************************************

  @file         hbd13.h

  ------------------------------------------------------------------------------

  @author       JuergenP

  @brief        nodehandling

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

#ifndef HBD13_H
#define HBD13_H


/*===========================================================================*
 *     RefType: C++                                                          *
 *===========================================================================*/

/*===========================================================================*
 *     MD5: '\x8a\x9a\x08\x01\x9c\x4d\x1a\x56\x57\x32\xda\x9f\x9b\x59\x2e\xc7'
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

externPascal void bd13CheckNode (
    tsp00_TaskId             taskId,
    tgg00_BasisError      &  trError,
    tbd_node_ptrs         &  pNodes,
    tgg00_FileId          &  fileId,
    pasbool                  bExtendedCheck);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd13FreePageNo (
    tsp00_PageNo             pageNo,
    tgg00_ConverterVersion     pageConverterVersion,
    tsp00_Int2               level,
    tbd_current_tree      &  current);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd13FreeNewRoot (
    tbd_current_tree      &  current);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd13FreeTempPageNo (
    tsp00_PageNo             pageNo,
    tsp00_Int2               level,
    tbd_current_tree      &  current);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b13free_node (
    tbd_node_ptrs         &  Nptrs,
    tbd_current_tree      &  Current);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd13FreePnosAfterUnlock (
    tbd_current_tree      &  Current);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd13GetNode (
    tbd_current_tree      &  Current,
    tsp00_PageNo             Pno,
    tbd00_PageLockMode_Param     PageLockMode,
    tbd_node_request_Param     NodeReq,
    tbd_node_ptrs         &  Nptrs);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b13init_default_data_page (
    tgg00_FileId          &  FileId,
    tsp00_Int2               Level,
    tsp00_PageNo             NewPno,
    tbd_node_ptrs         &  Nptrs);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b13new_node (
    tsp00_Int2               LevelNo,
    tbd_node_ptrs         &  Nptrs,
    tbd_current_tree      &  Current);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b13new_object_node (
    tbd_node_ptrs         &  Nptrs,
    tbd_current_tree      &  Current);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b13new_root (
    tbd_node_ptrs         &  Nptrs,
    tbd_current_tree      &  Current);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b13r_release_node (
    tbd_node_ptrs         &  Nptrs,
    tbd_current_tree      &  Current,
    tbd_lru_info_Param       LruInfo);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b13w_release_node (
    tbd_node_ptrs         &  Nptrs,
    tbd_current_tree      &  Current);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd13WriteExclusiveLockedRoot (
    tgg00_TransContext    &  Trans,
    tgg00_FileId          &  FileId,
    tbd_node_ptrs         &  RootNptrs);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd13RReleaseRoot (
    tbd_current_tree      &  current);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd13WReleaseRoot (
    tbd_current_tree      &  current);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd13RReleaseSubRoot (
    tbd_current_tree      &  current);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd13WReleaseSubRoot (
    tbd_current_tree      &  current);

/* Don't edit this file by hand, it is generated by ptocProto.py */


#endif
