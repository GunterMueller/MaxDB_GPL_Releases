/* @lastChanged: "1998-04-17  17:29"

 * @filename:   heo60.h
 * @purpose:    "vmessage"
 * @release:    7.1.0.0
 * @see:        ""
 *
 * @Copyright (c) 1997-2005 SAP AG"


\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 1997-2005 SAP AG

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

*/

#ifndef HEO60_H
#define HEO60_H

#include "gsp00.h"
#include "gsp03.h"
#include "vsp0031.h"
#include "heo60_1.h" /* vksprintf()... To be deleted if all use heo60_1.h to get it */

externC void 	               vinsert_event    ( tsp31_event_description VAR_VALUE_REF Event );

externC void	               vmessage         ( tsp3_priority_Param   msg_prio ,
                                                  tsp3_msg_type_Param   msg_type ,
                                                  tsp00_Int4              msg_no ,
                                                  tsp00_C8 VAR_ARRAY_REF  msg_label ,
                                                  tsp00_C40 VAR_ARRAY_REF msg_txt ) ;

externC void	               vutilprot        ( char                 *msg_txt,
                                                  tsp00_Int4             msg_len) ;

externC void	               vtime_in_seconds ( tsp00_Int4 VAR_VALUE_REF time_in_sec );

#endif
