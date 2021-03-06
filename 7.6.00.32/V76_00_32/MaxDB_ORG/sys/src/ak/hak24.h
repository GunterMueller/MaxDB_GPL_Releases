/*!******************************************************************************

  @file         hak24.h

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

#ifndef HAK24_H
#define HAK24_H


/*===========================================================================*
 *     RefType: Both                                                         *
 *===========================================================================*/

/*===========================================================================*
 *     MD5: '\x45\x7c\x14\x05\x72\x89\xb3\x64\x15\xfd\xc7\x1e\x03\xdb\x6f\x16'
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

externPascal void a24assign_indexname (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak_sysbufferaddress                 ibuf,
    integer                              index,
    tsp00_KnlIdentifier   VAR_ARRAY_REF  indexname);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a24init_index_scan (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    const tgg00_Surrogate  VAR_ARRAY_REF  tabid,
    tak_index_scan_record  VAR_VALUE_REF  index_scan_rec);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a24get_indexname (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak_sysbufferaddress                 indexbuf,
    integer                              index,
    tsp00_KnlIdentifier   VAR_ARRAY_REF  index_name);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a24find_indexname (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tgg00_Surrogate       VAR_ARRAY_REF  tabid,
    tsp00_KnlIdentifier   VAR_ARRAY_REF  indexname,
    tak_index_scan_record  VAR_VALUE_REF  index_scan_rec);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a24fnd_indexno (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    const tgg00_Surrogate  VAR_ARRAY_REF  tabid,
    integer                              indexno,
    tak_index_scan_record  VAR_VALUE_REF  index_scan_rec);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a24finish_index_scan (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak_index_scan_record  VAR_VALUE_REF  index_scan_rec);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal integer a24FunctionParameterCount (
    tak_multindex         VAR_VALUE_REF  index_def);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal integer a24IndexFieldCount (
    tak_multindex         VAR_VALUE_REF  index_def);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool a24IndexMatch (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak_multindex         VAR_VALUE_REF  index_def,
    integer                              stackStart,
    integer                              stackEnd);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool a24IsFunctionBasedIndex (
    tak_multindex         VAR_VALUE_REF  index_def);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool a24LookForFunctionBasedIndex (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    integer                              stackStart,
    integer                              stackEnd,
    integer               VAR_VALUE_REF  stackNext);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a24MigrateUnnamedIndex (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak_sysbufferaddress                 pBase,
    tgg00_StackOpType_Param                 opType,
    tak00_colinfo_ptr                    colptr,
    tsp00_Int2            VAR_VALUE_REF  newindexno);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool a24next_named_index (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak_index_scan_record  VAR_VALUE_REF  index_scan_rec);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a24_call_semantic (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    pasbool                              ignore_errors);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a24drop_multiple_index (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak_viewscan_par      VAR_VALUE_REF  viewscanpar,
    pasbool                              dropFile,
    integer                              indexname_errpos,
    pasbool                              do_a38_input);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a24RecreateBadIndexes (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    pasbool                              SysCatalogOnly);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a24send_index_command_to_kb (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak_viewscan_par      VAR_VALUE_REF  viewscanpar,
    integer                              index_no,
    tgg00_MessType_Param                 m_type,
    tgg00_MessType2_Param                 mm_type);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a24unique_spec (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    integer                              unique_node);

/* Don't edit this file by hand, it is generated by ptocProto.py */


#endif

