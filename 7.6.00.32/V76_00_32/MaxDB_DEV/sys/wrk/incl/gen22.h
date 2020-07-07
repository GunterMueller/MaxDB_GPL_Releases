/*!
  @file           gen22.h
  @author         JoergM
  @special area   RunTime
  @brief          Setup of UNIX own message output
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


#ifndef GEN22_H
#define GEN22_H

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

void en22MessageOutputCall( SAPDB_Int4         msgOutputMask,
                            SAPDB_Int4         msgNumber, 
                            SAPDB_Int4         msgType, 
                            SAPDB_Char const * msgLabel, 
                            SAPDB_Char const * msgFormat,
                            va_list            msgArguments );

/*
  @brief select given channel instead of appldiag for output of diagnostic messages

  After this call all MSG macro outputs as well as RTE_Message() outputs are shown on channel
  instead of appldiag. This call is used i.e. in RTECons_UNIXConsole to redirect output to
  console (channel==1) after redirection of stderr and setting to unbuffered mode.
  It is issued as a very first action in main().

  NOTE: channel must be opened already!

  @param channel [in] file descriptor to be used for output
  @param withTimestamp [in] if true, produce timestamps for every output line
 */
void en22SelectChannelForMessageOutput(SAPDB_Int channel, SAPDB_Bool withTimstamp);

#if defined(__cplusplus)

} /* extern "C" */

#endif

#endif /* GEN22_H */
