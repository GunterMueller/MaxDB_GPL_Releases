/*!******************************************************************************

  @file         hbd17.h

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

#ifndef HBD17_H
#define HBD17_H


/*===========================================================================*
 *     RefType: C++                                                          *
 *===========================================================================*/

/*===========================================================================*
 *     MD5: '\xed\x89\xc4\xbb\x90\x73\x4e\xae\xe2\xab\xa8\x5f\xa6\x56\x55\x5b'
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

externPascal tgg00_FileId bd17BlobFdirFileId;

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b17add_fdir (
    tgg00_Filename        &  fn,
    tbd_fileinfo          &  fi,
    tgg00_TransContext    &  t);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b17add_blob_fdir_nolock (
    tbd_current_tree      &  current_fdir,
    tgg00_Filename        &  fn,
    tbd_fileinfo          &  fi,
    tgg00_TransContext    &  t);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b17del_fdir (
    tgg00_Filename        &  fn,
    pasbool                  shared_file,
    tgg00_TransContext    &  t);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b17fadd_file_state (
    tgg00_TransContext    &  t,
    tgg00_FileId          &  fileId,
    tbd_file_state_set    &  newState);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b17fsub_file_state (
    tgg00_TransContext    &  t,
    tgg00_FileId          &  fileId,
    tbd_file_state_set    &  subState);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b17next_del_fdir (
    tgg00_Filename        &  prefix,
    integer                  prefix_len,
    tgg00_Filename        &  fn,
    tbd_fileinfo          &  fi,
    tgg00_TransContext    &  t);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b17repl_fdir (
    tgg00_Filename        &  fn,
    tbd_fileinfo          &  fi,
    pasbool                  shared_file,
    tgg00_TransContext    &  t);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b17reset_read_only (
    tgg00_TransContext    &  t);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b17state_fdir (
    tgg00_Filename        &  fn,
    tbd_fileinfo          &  fi,
    pasbool                  shared_file,
    tgg00_TransContext    &  t);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd17AddBlobToFDir (
    tgg00_TransContext    &  trans,
    tgg00_Filename        &  fn,
    tbd_fileinfo          &  fi);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd17CreateBlobFDir (
    tgg00_TransContext    &  trans);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal tsp00_PageNo bd17GetBlobFdirRoot ();

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd17GetNextBlob (
    tgg00_TransContext    &  trans,
    tgg00_Filename        &  filename,
    tbd_fileinfo          &  fileInfo);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd17GetBlobFileInfo (
    tgg00_TransContext    &  trans,
    tgg00_FileId          &  fileId,
    tgg91_FileVersion     &  fileVersion,
    tbd_file_state_set    &  fileState);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd17MigrateFdir (
    tgg00_TransContext    &  t,
    tsp00_PageNo             oldFdirRootPNo);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd17RestartBlobFdir (
    tgg00_TransContext    &  t,
    tsp00_PageNo             blob_fdir_root);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd17RenameFile (
    tgg00_TransContext    &  trans,
    tgg00_FileId          &  srcFileId,
    tgg00_FileId          &  destFileId,
    tgg00_Surrogate       &  parent_Id);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd17VerifyBlobFdir (
    tgg00_TransContext    &  trans,
    pasbool                  bIsCold,
    pasbool                  bWithExtendedCheck);

/* Don't edit this file by hand, it is generated by ptocProto.py */


#endif

