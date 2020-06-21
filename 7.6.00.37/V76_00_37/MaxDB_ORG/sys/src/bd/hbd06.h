/*!******************************************************************************

  @file         hbd06.h

  ------------------------------------------------------------------------------

  @author       JuergenP

  @brief        error_text_handling

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

#ifndef HBD06_H
#define HBD06_H


/*===========================================================================*
 *     RefType: C++                                                          *
 *===========================================================================*/

/*===========================================================================*
 *     MD5: '\x98\x65\xe3\x15\xd0\xae\xe5\x10\xd4\x86\xf0\x11\xe9\xa8\x99\x3f'
 *===========================================================================*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "ggg00.h"
#include "ggg04.h"


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externPascal pasbool b06check_rec;

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b06check_mxbd (
    pasbool               &  ok);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b06check_vresume_cnt (
    tsp00_TaskId             pid);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b06create_errortext_file (
    tgg00_TransContext    &  t);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b06get_errtxt (
    tgg00_TransContext    &  t,
    integer               &  errlen,
    tgg04_ErrorText       &  etexttype,
    tgg00_BasisError      &  b_error,
    tsp00_C256            &  errtxt);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b06init_errtxt ();

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b06put_errtxt (
    tgg00_TransContext    &  t,
    tsp00_TaskId             taskId,
    integer                  errlen,
    tgg04_ErrorText_Param     etexttype,
    tgg00_BasisError         b_err_in,
    tsp00_C256            &  errtxt,
    tgg00_BasisError      &  b_err_out);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b06write_filename_and_root (
    tgg00_FileId          &  file_id);

/* Don't edit this file by hand, it is generated by ptocProto.py */


#if defined (SAPDB_SLOW)

externPascal void b06record_check (
    tgg00_Rec             &  rec,
    tgg00_FileId          &  fileId,
    tgg00_BasisError      &  b_err);

/* Don't edit this file by hand, it is generated by ptocProto.py */


#endif

externPascal void b06file_opmsg (
    tsp00_Int4               msg_no,
    tgg00_FileId          &  fileId);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void b06dump_bad_page (
    tsp00_TaskId             pid,
    char                     page_type_flag,
    const char*              file_ext,
    tsp00_Int4               bad_pno,
    void*                    buf_ptr,
    integer                  curr_buf_cnt);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void bd06CorruptedTreeHandling (
    tgg00_FileId          &  fileId,
    tsp00_Int4               msgNo,
    tgg00_BasisError         trError);

/* Don't edit this file by hand, it is generated by ptocProto.py */


#endif

