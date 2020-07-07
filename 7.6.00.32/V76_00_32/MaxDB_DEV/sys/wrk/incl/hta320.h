/*!******************************************************************************

  @file         hta320.h

  ------------------------------------------------------------------------------

  @author       

  @brief        TA_driver_procedures

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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

#ifndef HTA320_H
#define HTA320_H


/*===========================================================================*
 *     RefType: Both                                                         *
 *===========================================================================*/

/*===========================================================================*
 *     MD5: '\x04\x9a\xb0\x94\x3b\x7d\x10\x72\x4d\xd1\xf1\xfb\xd5\x55\x2f\x04'
 *===========================================================================*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "gut00.h"
#include "vsp001.h"
#include "gta00.h"


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externPascal void t320sequential_program (
    tta_user_proc_globals  VAR_VALUE_REF  upg,
    tut_terminal          VAR_VALUE_REF  ut_term,
    pasbool                              is_util_cmd,
    tsp00_Line            VAR_ARRAY_REF  util_cmd_line);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t320readterminal (
    tta_user_proc_globals  VAR_VALUE_REF  upg,
    tsp00_DataLine        VAR_VALUE_REF  lineinfo,
    pasbool               VAR_VALUE_REF  last_line);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t320dbcall (
    tta_user_proc_globals  VAR_VALUE_REF  upg,
    tsp1_packet_ptr                      snd_packet,
    tsp1_packet_ptr       VAR_VALUE_REF  rcv_packet,
    tsp00_Int2            VAR_VALUE_REF  returncode,
    tsp00_C12             VAR_ARRAY_REF  parseid,
    tsp00_Int2                           rescount);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t320drop_parseid (
    tta_user_proc_globals  VAR_VALUE_REF  upg,
    tsp1_packet_ptr                      snd_packet,
    tsp00_C12             VAR_ARRAY_REF  parseid);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t320errmsg (
    tta_user_proc_globals  VAR_VALUE_REF  upg,
    tsp1_segment          VAR_VALUE_REF  ret_segm);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t320string_to_line (
    tsp00_C40             VAR_ARRAY_REF  comment,
    tsp00_Line            VAR_ARRAY_REF  commentline);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t320finish_part (
    tta_user_proc_globals  VAR_VALUE_REF  upg,
    tsp1_packet_ptr                      packet_ptr,
    tsp1_part             VAR_VALUE_REF  part);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t320unicode2ascii (
    tta_user_proc_globals  VAR_VALUE_REF  upg,
    tsp00_Int4                           buf_size,
    tsp00_MoveObjPtr                     buf_ptr,
    tsp00_Int4            VAR_VALUE_REF  buf_len);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t320buildbuffer (
    tsp00_DataLine        VAR_VALUE_REF  line1,
    tsp1_segment          VAR_VALUE_REF  cmd_segm);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t320resultcount_part (
    tta_user_proc_globals  VAR_VALUE_REF  upg,
    tsp1_packet_ptr                      snd_packet,
    tsp1_segment          VAR_VALUE_REF  snd_segm,
    tsp00_Int2                           result);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t320getcommand (
    tsp00_DataLine        VAR_VALUE_REF  in_line,
    tsp00_Sname           VAR_ARRAY_REF  vstring);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t320GetVal (
    tta_user_proc_globals  VAR_VALUE_REF  upg,
    tsp1_packet_ptr                      snd_packet,
    tsp00_LongDescriptor  VAR_VALUE_REF  long_desc,
    tsp1_part_ptr         VAR_VALUE_REF  pData);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t320display_enter_command (
    tta_user_proc_globals  VAR_VALUE_REF  upg,
    tsp00_Int2                           returncode);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t320prot (
    tta_user_proc_globals  VAR_VALUE_REF  upg,
    integer                              len);

/* Don't edit this file by hand, it is generated by ptocProto.py */


externPascal void t320parseid_part (
    tta_user_proc_globals  VAR_VALUE_REF  upg,
    tsp1_packet_ptr                      snd_packet,
    tsp1_segment          VAR_VALUE_REF  snd_segm,
    tsp00_C12             VAR_ARRAY_REF  parsid);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t320dbs_send (
    tta_user_proc_globals  VAR_VALUE_REF  upg,
    tsp1_packet_ptr                      snd_packet,
    tsp1_packet_ptr       VAR_VALUE_REF  rcv_packet,
    tsp00_Int2            VAR_VALUE_REF  returncode);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void t320_to_upper (
    tsp00_Line            VAR_ARRAY_REF  line,
    integer                              length);

/* Don't edit this file by hand, it is generated by ptocProto.py */


#endif

