/*!
  @file           heo79_1.h
  @author         JoergM
  @special area   LZU / Kernel
  @brief          kernel xparam access routines
  @see            example.html ...

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
*/



#ifndef HEO79_1_H
#define HEO79_1_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

/* --- VEO79.C ------------------------------------------------------------- */


externC void    vxparam_save_good_config (
    tsp00_Bool                          is_restart,
    tsp00_ErrText      VAR_ARRAY_REF    errtext,
    tsp00_XpReturnCode VAR_VALUE_REF    pucXParamReturnCode ) ;

externC tsp00_Bool  vxparam_delete_sysdev (
    tsp00_ErrText      VAR_ARRAY_REF     errtext );


#endif  /* HEO79_1_H */
