/*!
  @file           RTEDiag_Appldiag.h
  @author         JoergM
  @ingroup        RunTime
  @brief          UNIX protocol file logging process spawning

  Used especially to start a logger process that allows several processes to log into a single log file

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
#ifndef RTEDIAG_APPLDIAG_H
#define RTEDIAG_APPLDIAG_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include "RunTime/RTE_Types.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/
/*!
  @brief Spawining logging process for appldiag
  @param argv [inout] argument vector used to manipulate argv[0]
  @param newTitle [in] the new title wanted
  @param outputFd [in] already opened output file
  @param useTimestamp [in] flag to indicate time stamps are wanted
  @param fileLimit [in] limitation of output file size in bytes
  @return -1 on error, file descriptor of new output descriptor
 */
externC SAPDB_Int4 RTEDiag_SpawnAppldiagLogger( 
	SAPDB_Char *argv[],	/* the original argv */
	SAPDB_Char *newTitle, /* the new argv[0] value */
    SAPDB_Int4 outputFd, /* copy of sql22_fd */
	SAPDB_Int4 useTimestamp, /* copy of sql22_time_stamp */
    RTE_FileOffset fileLimit); /* new parameter */

/*!
  @brief Spawining logging process for Xserver
  @param argv [inout] argument vector used to manipulate argv[0]
  @param outputFd [in] already opened output file
  @param fileLimit [in] limitation of output file size in bytes
  @param protocolPipeName [in] name of named pipe used to write to logging process
  @return -1 on error, file descriptor of new output descriptor
 */
externC SAPDB_Int4 RTEDiag_XserverLogger( 
	SAPDB_Char *argv[], /* the original argv */
    SAPDB_Int4 outputFd, /* copy of sql22_fd */
    RTE_FileOffset fileLimit, /* limitation of logfile size in bytes */
    SAPDB_Char const *protocolPipeName); /* named pipe name */

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/



#endif  /* RTEDIAG_APPLDIAG_H */
