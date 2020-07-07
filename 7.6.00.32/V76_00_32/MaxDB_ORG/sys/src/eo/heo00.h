/*!
  @file           heo00.h
  @author         JoergM
  @special area   
  @brief          RTE - common_rte_include module
  @see            

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
*/



#ifndef HEO00_H
#define HEO00_H

#if defined (SAPDB_QUICK) || defined (SAPDB_SLOW)
#define DEVELOP_CHECKS TRUE
#endif

#include "gsp00.h"

/*
 *  Values for kernel task type
 */
/* --- now defined in geo00_2.h */

/***************************************************************/
#if defined(_WIN32) || defined(OS2) 
/***************************************************************/

//#define  SPRINTF                        wsprintf
#define  KERNEL_GLOBAL_DBNAME           kgs.szServerDB

/* PATH_DELIMITTER */
/* now defined in geo001.h */

/***************************************************************/
#else
/***************************************************************/

#define  SPRINTF                        sprintf
#define  KERNEL_GLOBAL_DBNAME           en81_dbname

extern  tsp00_DbNamec      KERNEL_GLOBAL_DBNAME ;

/* PATH_DELIMITTER */
/* --- now defined in geo001.h */

/* FREE_MEM, ALLOC_MEM */
/* now defined in geo570.h */

/***************************************************************/
#endif
/***************************************************************/



#endif  /* HEO00_H */

