/*!******************************************************************************

  @file         hak12.h

  ------------------------------------------------------------------------------

  @author       ThomasA

  @brief        AK_Domain

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

#ifndef HAK12_H
#define HAK12_H


/*===========================================================================*
 *     RefType: Both                                                         *
 *===========================================================================*/

/*===========================================================================*
 *     MD5: '\x44\x82\x5e\xbf\x2a\xff\x20\xfb\xef\x94\x6e\x30\x18\x12\x97\xa2'
 *===========================================================================*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "ggg00.h"
#include "gak00.h"
#include "vak001.h"


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externPascal void a12create_dbproc (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    integer                              start_node,
    tgg00_Surrogate       VAR_ARRAY_REF  dbproc_id);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a12_call_semantic (
    tak_all_command_glob  VAR_VALUE_REF  acv);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool a12DBFuncExistsNoPrivCheck (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tsp00_KnlIdentifier   VAR_ARRAY_REF  schema,
    tsp00_KnlIdentifier   VAR_ARRAY_REF  dbfunc_name,
    tak_directory_state_Param                 dstate,
    tak_sysbufferaddress  VAR_VALUE_REF  method_buf);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a12CreateFileInfoRecord (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_Surrogate       VAR_ARRAY_REF  dbproc_id,
    tak_method_filename   VAR_ARRAY_REF  libraryPathBuffer,
    tsp00_C32             VAR_ARRAY_REF  MD5_footprint);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a12drop_domain (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak_sysbufferaddress                 domain_ref);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool a12dbfunc_exist (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tsp00_KnlIdentifier   VAR_ARRAY_REF  schema,
    tsp00_KnlIdentifier   VAR_ARRAY_REF  dbfunc_name,
    tak_directory_state_Param                 dstate,
    tak_sysbufferaddress  VAR_VALUE_REF  method_buf);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool a12dbproc_exist (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tsp00_KnlIdentifier   VAR_ARRAY_REF  schema,
    tsp00_KnlIdentifier   VAR_ARRAY_REF  dbproc,
    tak_directory_state_Param                 dstate,
    tak_sysbufferaddress  VAR_VALUE_REF  method_buf);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a12describe_param (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak_sysbufferaddress                 method_buf,
    integer                              param_no,
    tak00_scolinf         VAR_VALUE_REF  colinf);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool a12exist_type (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    integer               VAR_VALUE_REF  tree_node,
    tsp00_KnlIdentifier   VAR_ARRAY_REF  owner,
    tsp00_KnlIdentifier   VAR_ARRAY_REF  type_name,
    tak_sysbufferaddress  VAR_VALUE_REF  type_buf);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a12FindParameter (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_Surrogate       VAR_ARRAY_REF  schemaId,
    tsp00_KnlIdentifier   VAR_ARRAY_REF  dbproc,
    integer                              paramNo,
    tak_param_info_ptr    VAR_VALUE_REF  pInfo);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal integer a12EvalDataLength (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak_sysbufferaddress                 p);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a12get_domain (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tsp00_KnlIdentifier   VAR_ARRAY_REF  owner,
    tsp00_KnlIdentifier   VAR_ARRAY_REF  domain_name,
    integer                              ti,
    tak_sysbufferaddress  VAR_VALUE_REF  domain_ref,
    tak_sysbufferaddress  VAR_VALUE_REF  domain_def);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a12InitParamInfoRec (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_Surrogate       VAR_ARRAY_REF  funcId,
    integer                              paramCnt,
    tak_sysbufferaddress  VAR_VALUE_REF  p);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a12LoadFileInfo (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_Surrogate       VAR_ARRAY_REF  dbproc_id,
    tsp00_Addr            VAR_VALUE_REF  pFileName,
    tsp00_C32             VAR_ARRAY_REF  MD5_footprint);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool a12method_exist (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_Surrogate       VAR_ARRAY_REF  type_id,
    tsp00_KnlIdentifier   VAR_ARRAY_REF  method_name,
    tak_directory_state_Param                 dstate,
    tak_sysbufferaddress  VAR_VALUE_REF  method_buf);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a12NewMethod (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_Surrogate       VAR_ARRAY_REF  SchemaId,
    tsp00_KnlIdentifier   VAR_ARRAY_REF  DBFuncName,
    tsp00_KnlIdentifier   VAR_ARRAY_REF  MethodName,
    tsp00_Int4                           ErrorPos,
    pasbool               VAR_VALUE_REF  IsReplace,
    tsp00_DbObjectType_Param                 MethodType,
    tsp00_C64             VAR_ARRAY_REF  ProgId,
    tsp00_KnlIdentifier   VAR_ARRAY_REF  language,
    tak_sysbufferaddress  VAR_VALUE_REF  methodbuf);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a12output_parameter (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak_sysbufferaddress                 method_buf,
    integer               VAR_VALUE_REF  inout_idx,
    tak00_scolinf         VAR_VALUE_REF  colinf);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a12read_domain_ref (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_Surrogate       VAR_ARRAY_REF  owner_id,
    tsp00_KnlIdentifier   VAR_ARRAY_REF  domain_name,
    tak_sysbufferaddress  VAR_VALUE_REF  domain_ref);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a12reference (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_Surrogate       VAR_ARRAY_REF  dbproc_surrogate,
    tsp00_KnlIdentifier   VAR_ARRAY_REF  schema,
    tsp00_KnlIdentifier   VAR_ARRAY_REF  dbproc_name);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a12StoreParamInfo (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak_sysbufferaddress                 p,
    tak00_scolinf         VAR_VALUE_REF  param_info);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool a12SystemTriggerExists (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tsp00_KnlIdentifier   VAR_ARRAY_REF  schema,
    tsp00_KnlIdentifier   VAR_ARRAY_REF  dbproc,
    tak_directory_state_Param                 dstate,
    tak_sysbufferaddress  VAR_VALUE_REF  method_buf);

/* Don't edit this file by hand, it is generated by ptocProto.py */


#endif

