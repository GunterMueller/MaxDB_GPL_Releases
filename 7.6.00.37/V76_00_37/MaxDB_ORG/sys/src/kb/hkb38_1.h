/*!******************************************************************************

  @file         hkb38_1.h

  ------------------------------------------------------------------------------

  @author       UweH

  @brief        KB_headmaster

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

#ifndef HKB38_1_H
#define HKB38_1_H


/*===========================================================================*
 *     RefType: Both                                                         *
 *===========================================================================*/

/*===========================================================================*
 *     MD5: '\xb2\x9d\x82\x99\x99\x29\x05\xb2\xc2\x3c\x44\x69\x9c\x3b\x16\x5a'
 *===========================================================================*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "ggg00.h"


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externPascal void k38e_autosave_end (
    tgg00_TransContext    VAR_VALUE_REF  t,
    pasbool                              is_quick);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k38get_autosave_medianame (
    tsp00_TaskId                         TaskId,
    tsp00_C64             VAR_ARRAY_REF  media_name);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k38i_autosave_init (
    tgg00_MessBlock       VAR_VALUE_REF  m);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool k38is_on_autosave (
    tgg00_TransContext    VAR_VALUE_REF  t,
    pasbool                              in_region);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k38trigger_autosave (
    tgg00_TransContext    VAR_VALUE_REF  t,
    pasbool                              in_region);

/* Don't edit this file by hand, it is generated by ptocProto.py */


#endif

