/*!
  @file           RTE_DiagPipe.h
  @author         RobinW
  @brief          Kernel RunTime: Message Output
  @see            

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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

#ifndef RTE_DIAGPIPE_H
#define RTE_DIAGPIPE_H

/* during kernel startup, messages given by MSGD are also written to a pipe that is read by dbmcli */
/* this file implements on all platforms routines for the kernel to write to the pipe and routines */
/* for the client (dbmsrv/dbmcli) to read from this pipe. As the kernel routines code are called   */
/* from C code, they are implemented as global functions. The client routines are called from C++  */
/* code, so a class has been created for them */

/*!
   @brief           connect to the pipe that has been created by the dbm.
   @param           dbName [in] - Name of the current database

 */

externC void RTE_ConnectToDiagPipe(SAPDB_Char *dbName);

/*!
   @brief           write a message to the pipe if it's open.
   @param           message [in] - pointer to the message text
   @param           length [in] - length of the message text.

 */

externC void RTE_WriteToDiagPipe( SAPDB_Char *message, SAPDB_Int length );

/*!
   @brief           if the diag pipe is open, close it and invalidate the file descriptor.

 */

externC void RTE_CloseDiagPipe(void);

/*!
   @brief           give the state of the diag pipe
   @return value    true if the diag pipe is open, false if not

 */

externC SAPDB_Bool RTE_DiagPipeIsOpen(void);

#ifdef __cplusplus
#include "RunTime/RTE_DiagPipe.hpp"
#endif

#endif /* RTE_DIAGPIPE_H */

