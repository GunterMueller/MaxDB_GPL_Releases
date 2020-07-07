/*!
  @file           heo79.h
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



#ifndef HEO79_H
#define HEO79_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "heo11.h"

#include "heo79_1.h"

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

/* --- VEO79.C ------------------------------------------------------------- */


externC void    vxparam_open            (   XP_PHFILE           plXParamFd,
                                            tsp00_ErrText         errtext,
                                            tsp00_XpReturnCode *pucXParamReturnCode );

externC void    vxparam_close           (   XP_HFILE            lXParamFd,
                                            tsp00_ErrText         errtext,
                                            tsp00_XpReturnCode *pucXParamReturnCode );

externC void    vxparam_open_binary      ( tsp00_DbName       ServerDB,
                                           BOOLEAN            ReadOnly,
                                           XP_PHFILE          xp_fd ,
                                           tsp00_ErrText      errtext ,
                                           tsp00_XpReturnCode *xp_ret ) ;

externC void    vxparam_read_binary      ( XP_HFILE            xp_fd ,
                                           char               *buf ,
                                           tsp00_Int4            bytes_to_read ,
                                           tsp00_Int4           *bytes_read ,
                                           tsp00_ErrText         errtext ,
                                           tsp00_XpReturnCode *xp_ret );

externC void    vxparam_write_binary     ( XP_HFILE            xp_fd ,
                                           char               *buf ,
                                           tsp00_Int4            bytes_to_write ,
                                           tsp00_Int4           *bytes_written ,
                                           tsp00_ErrText         errtext ,
                                           tsp00_XpReturnCode *xp_ret );

externC void    vxparam_get_filesize    ( tsp00_Int4           *xp_fsize ,
                                          tsp00_ErrText         errtext ,
                                          tsp00_XpReturnCode *xp_ret );

#endif  /* HEO79_H */
