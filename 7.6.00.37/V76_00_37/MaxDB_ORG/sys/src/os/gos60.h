/*!
  @file           gos60.h
  @author         JoergM
  @brief          Setup of NT own message output
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


#ifndef GOS60_H
#define GOS60_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/SAPDB_Types.h"

#if defined(__cplusplus)

extern "C" {

#include <stdarg.h>

#endif
           
/*===========================================================================*
 *  GLOBAL FUNCTIONS                                                         *
 *===========================================================================*/

void os60MessageOutputCall( SAPDB_Int4         msgOutputMask,
                            SAPDB_Int4         msgNumber, 
                            SAPDB_Int4         msgType, 
                            SAPDB_Char const * msgLabel, 
                            SAPDB_Char const * msgFormat,
                            va_list            msgArguments );

/*!
  @brief write the database stop timestamp into knldiag.err
 */
void RTE_WriteDatabaseStopTime();
#if defined(__cplusplus)

} /* extern "C" */

#endif

#endif /* GOS60_H */
