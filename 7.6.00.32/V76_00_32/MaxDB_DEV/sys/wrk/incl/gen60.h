/*!
  @file           gen60.h
  @author         JoergM
  @special area   vopmsg
  @brief          Message Output
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



#ifndef GEN60_H
#define GEN60_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/RTE_Types.h"
#include <stdarg.h>

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externC void sql60kc_msg ( SAPDB_Byte prio ,
						   const SAPDB_Char *text,
						   SAPDB_Int   msg_type );

externC void sql60_write_to_util_diag ( const SAPDB_Char     *text ,
                                        SAPDB_Int      len );

externC void sql60_write_to_event_file ( const SAPDB_Char     *text ,
                                         SAPDB_Int      len );

externC void e60_deliver_msg (SAPDB_Char *msg, 
							  SAPDB_Int lgt );

externC SAPDB_Int  en60InitializeMessageOutput( /* void */ );

externC void sqldbgwrite ( const SAPDB_Char *text ,
                           ... );

externC void sqlerrwrite ( const SAPDB_Char *text ,
                           ... );

externC void sqlwngwrite ( const SAPDB_Char *text ,
                           ... );

externC SAPDB_Int  en60OpenEventFile ( RTE_Path VAR_ARRAY_REF Filename, SAPDB_Int4 MaxSize );

externC void en60MessageOutputCall( SAPDB_Int4         msgOutputMask,
                                    SAPDB_Int4         msgNumber, 
                                    SAPDB_Int4         msgType, 
                                    SAPDB_Char const * msgLabel, 
                                    SAPDB_Char const * msgFormat,
                                    va_list            msgArguments );

/*!
  @brief write the database stop timestamp into knldiag.err
 */
externC void RTE_WriteDatabaseStopTime();
#endif  /* GEN60_H */
