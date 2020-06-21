/*!
  @file           heo60_1.h
  @author         JoergM
  @special area   Wrapper for sprintf
  @brief          Wrapper for ANSI C 'sprintf' function

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



#ifndef HEO60_1_H
#define HEO60_1_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

/*!
   @brief          Wrapper for ANSI C function 'sprintf'
   @param          pszBuffer [out] pointer to the message buffer
   @param          pszFormat [in] pointer to the message format string
   @param          ... [in] variable number of arguments depending on format
   @return         none


     In multi threaded environment direct use of 'sprintf' is forbidden. This wrapper
     is used instead. The ANSI specification of 'sprintf' should be used.
     NOTE: Message buffer MUST contain enough space or system may get corrupted.
           There is no way to check size of buffer...

 */

externC void 
vksprintf(       tsp00_Char * pszBuffer,
		   const tsp00_Char * pszFormat,
		                      ...);
/* implementation found in vos60c and ven60c */

#endif  /* HEO60_1_H */
