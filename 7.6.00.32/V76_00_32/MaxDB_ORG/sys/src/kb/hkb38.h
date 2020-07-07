/*!******************************************************************************

  @file         hkb38.h

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

#ifndef HKB38_H
#define HKB38_H


/*===========================================================================*
 *     RefType: Both                                                         *
 *===========================================================================*/

/*===========================================================================*
 *     MD5: '\x68\x7c\xb2\x39\xc5\x39\x8d\x07\x7a\x3c\xf5\xec\x63\x5b\x79\x6d'
 *===========================================================================*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "ggg00.h"


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externPascal void k38allocate (
    tsp00_TaskId                         pid,
    tgg00_BasisError      VAR_VALUE_REF  b_err);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k38dump_headmaster (
    tgg00_VfFileref       VAR_VALUE_REF  hostfile,
    tsp00_Page            VAR_ARRAY_REF  buf,
    tsp00_Int4            VAR_VALUE_REF  out_pno,
    integer               VAR_VALUE_REF  out_pos,
    tsp00_VfReturn        VAR_VALUE_REF  host_err,
    tsp00_ErrText         VAR_ARRAY_REF  errtext);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal tsp00_Int4 k38GetMaxServerTasks ();

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal tsp00_TaskId k38GetTaskIdOfCurrentBackup (
    tsp00_TaskId                         taskid);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k38get_state (
    tsp00_Int4                           buf_size,
    tsp00_Int4            VAR_VALUE_REF  buf_len,
    void                  *  buffer);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k38headmaster (
    tgg00_MessBlock       VAR_VALUE_REF  m);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k38history_error_write (
    tsp00_TaskId                         taskId,
    tgg00_UtilCmdId       VAR_VALUE_REF  utilcmd_id,
    tgg00_MessType_Param                 save_type1,
    tgg00_MessType2_Param                 save_type2,
    pasbool                              is_cold,
    tsp00_Int4                           backup_no,
    tgg00_BasisError                     err_reason);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k38history_lost_write (
    tsp00_TaskId                         taskId);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool k38is_backup_running ();

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k38init_restore (
    tgg00_MessBlock       VAR_VALUE_REF  m,
    pasbool                              is_check_save);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k38init_save (
    tgg00_MessBlock       VAR_VALUE_REF  m,
    pasbool                              is_cold,
    pasbool                              complete_log,
    pasbool                              destructive_tape_check);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k38ReserveBackupServerTasks (
    tgg00_TransContext    VAR_VALUE_REF  trans);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k38restore_config (
    tgg00_MessBlock       VAR_VALUE_REF  mblock);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k38set_not_generated ();

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k38sh_autosave_show (
    tgg00_MessBlock       VAR_VALUE_REF  m);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k38st_autosave_start (
    tgg00_TransContext    VAR_VALUE_REF  trans);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k38UtilIdGet (
    tgg00_UtilCmdId       VAR_VALUE_REF  UtilIdStr);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k38UtilIdSaveLineNo (
    tgg00_UtilCmdId       VAR_VALUE_REF  UtilId);

/* Don't edit this file by hand, it is generated by ptocProto.py */


#endif
