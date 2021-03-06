/*!******************************************************************************

  @file         hak43.h

  ------------------------------------------------------------------------------

  @author       ThomasA

  @brief        Show_user_priv

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

#ifndef HAK43_H
#define HAK43_H


/*===========================================================================*
 *     RefType: Both                                                         *
 *===========================================================================*/

/*===========================================================================*
 *     MD5: '\x79\x5e\x15\x34\x30\xce\x85\x7e\x76\x45\xa6\x01\x7c\x4b\x2a\xbe'
 *===========================================================================*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "vak001.h"
#include "gak40.h"


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externPascal void a43get_granted_priv (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak40_show_glob       VAR_VALUE_REF  a41v,
    tsp00_KnlIdentifier   VAR_ARRAY_REF  owner,
    tsp00_KnlIdentifier   VAR_ARRAY_REF  tablename);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a43execute (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak40_show_glob       VAR_VALUE_REF  a41v);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void a43AllRolesCallback (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tak40_show_glob       VAR_VALUE_REF  a41v,
    tak_sysbufferaddress                 pRole);

/* Don't edit this file by hand, it is generated by ptocProto.py */


#endif

