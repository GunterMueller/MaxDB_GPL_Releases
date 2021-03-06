/*!******************************************************************************

  @file         hak52.h

  ------------------------------------------------------------------------------

  @author       <unknown>

  @brief        <unknown topic>

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG
 
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

#ifndef HAK52_H
#define HAK52_H


/*===========================================================================*
 *     RefType: Both                                                         *
 *===========================================================================*/

/*===========================================================================*
 *     MD5: '\xba\x84\x51\x60\x51\x08\xbe\x31\x91\x18\x7b\x55\x8c\x44\xaa\x62'
 *===========================================================================*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "ggg00.h"
#include "vak001.h"


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externPascal void a52_alock_statement (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tsp00_Int2            VAR_VALUE_REF  put_node);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a52_aunlock_statement (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tsp00_Int2            VAR_VALUE_REF  put_node);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a52_acommit_rollback_statement (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tsp00_Int2            VAR_VALUE_REF  put_node,
    pasbool                              commit);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a52_aforce_statement (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tsp00_Int2            VAR_VALUE_REF  put_node);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a52_asavepoint (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tsp00_Int2            VAR_VALUE_REF  put_node);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a52_arelease_savepoint (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tsp00_Int2            VAR_VALUE_REF  put_node);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a52asubtrans_statement (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tsp00_Int2            VAR_VALUE_REF  put_node);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a52_call_semantik (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tsp00_Int2                           subproc);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a52InnerTransactionStatement (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tsp00_Int2            VAR_VALUE_REF  put_node);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a52_lock_statement (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tsp00_Int2                           start_node);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a52commit_rollback (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_MessType_Param                 m_type,
    pasbool                              n_rel,
    pasbool                              normal_release);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a52_ex_commit_rollback (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_MessType_Param                 m_type,
    pasbool                              n_rel,
    pasbool                              normal_release);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a52_rest_lock_statement (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak_syspointerarr     VAR_VALUE_REF  isparr,
    tak_ak_all_locks      VAR_VALUE_REF  back_lock,
    pasbool                              return_result,
    pasbool                              complicate);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a52free_caches (
    tak_all_command_glob  VAR_VALUE_REF  acv);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a52new_subtrans (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tsp00_KnlIdentifier   VAR_ARRAY_REF  subtrans_name);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a52InnerTransaction (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    integer                              subproc);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a52internal_subtrans (
    tak_all_command_glob  VAR_VALUE_REF  acv);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a52end_rollback_subtrans (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tsp00_KnlIdentifier   VAR_ARRAY_REF  subtrans_name,
    tgg00_MessType_Param                 m_type);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a52release_session (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    pasbool                              normal_release);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a52init_subtrans_of_new_trans (
    tak_all_command_glob  VAR_VALUE_REF  acv);

/* Don't edit this file by hand, it is generated by ptocProto.py */


#endif

