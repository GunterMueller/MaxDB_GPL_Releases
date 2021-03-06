/*!******************************************************************************

  @file         hak102.h

  ------------------------------------------------------------------------------

  @author       RaymondR

  @brief        AKDump

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

#ifndef HAK102_H
#define HAK102_H


/*===========================================================================*
 *     RefType: Both                                                         *
 *===========================================================================*/

/*===========================================================================*
 *     MD5: '\xbd\xd0\xbd\x9d\x73\x67\x5b\x81\xfd\x4f\x88\x96\x00\x50\x76\xda'
 *===========================================================================*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "vak001.h"


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externPascal pasbool a102akdump (
    tak_all_command_glob  VAR_VALUE_REF  acv);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool a102IsCacheOk (
    tak_all_command_glob  VAR_VALUE_REF  acv);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool a102PtrOK (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak_cache_dir_pointer                 p);

/* Don't edit this file by hand, it is generated by ptocProto.py */


#endif

