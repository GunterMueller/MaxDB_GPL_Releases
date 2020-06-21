/*!
  @file           RTE_DiagPipe.hpp
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

#ifndef RTE_DIAGPIPE_HPP
#define RTE_DIAGPIPE_HPP

#include "RunTime/System/RTESys_IO.h"
class SAPDBErr_MessageList;

/*!
  @class  RTE_ClientDiagPipe
  @brief class for handling the diag pipe 

  During kernel startup, messages given by MSGD are also written to a pipe that is read by dbmcli
  this file implements on all platforms routines for the kernel to write to the pipe and routines
  for the client (dbmsrv/dbmcli) to read from this pipe. As the kernel routines code are called 
  from C code, they are implemented as global functions. The client routines are called from C++ 
  code, so a class has been created for them
 */
class RTE_ClientDiagPipe
{
public:
    /*!
       @brief           constructor.

     ----------------------------------------------------------------*/

    RTE_ClientDiagPipe();

    /*!
       @brief           destructor.

       closes all open pipes

     ----------------------------------------------------------------*/

    ~RTE_ClientDiagPipe();
    /*!
       @brief           create the diag pipe.
       @param           dbName [in] - Name of the current database

     */

    SAPDB_Bool create(SAPDB_Char *dbName);

    /*!
       @brief           read a message from the pipe.
       @param           message [out] - the message if one could be read
       @Return value    true if a message could be read, false if not

     */

    SAPDB_Bool readMessage(SAPDBErr_MessageList &message);

    /*!
       @brief           close the handle for the client side of the pipe if it is open.

     */

    void closeClientSide();

#ifdef _WIN32
#else
    /*!
       @brief   close the handle for the kernel side of the pipe if it is open.
                on unix systems, the client creates file descriptors for both sides 
                of the pipe and passes one to the kernel child process. This one must 
                be closed by the client to be able to be noticed when it is closed by 
                the kernel side. So the client is given a method to close the kernel 
                side of the pipe.

     */

    void closeKernelSide();

    /*!
       @brief   wait until the pipe has been closed by the kernel.
                On unix systens, a special "in-between-"process is created when the kernel 
                is being started. This process has to wait until the kernel signnalizes 
                admin state by closing the pipe, but he must not read from the pipe to 
                avoid stealing messages from the dbm process. So the "in-between-"process 
                has to use this method to be able to waít until the pipe is being closed.

       @Return value    false if an error occurred in the system call, true if no error
     */
    SAPDB_Bool waitForClose();


    /*!
       @brief   check if a given handle belongs to the diag pipe

     */

    SAPDB_Bool HandleBelongsToDiagPipe(RTE_FileHandle handle);
#endif

private:
#ifdef _WIN32
    HANDLE m_pipeHandle;
#else
    int m_pipeHandle[2];
#endif
};

#endif /* RTE_DIAGPIPE_HPP */

