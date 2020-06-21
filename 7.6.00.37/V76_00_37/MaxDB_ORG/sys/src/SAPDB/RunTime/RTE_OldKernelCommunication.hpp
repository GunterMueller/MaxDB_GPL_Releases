/*!
  @file    RTE_OldKernelCommunication.hpp
  @author  JoergM
  @ingroup Runtime
  @brief   C++ header for prototypes of old kernel communication interface

  Routine defined here will get removed as soon as new interface are available.

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

#ifndef RTE_OLDKERNELCOMMUNICATION_HPP
#define RTE_OLDKERNELCOMMUNICATION_HPP

#include "gsp00.h"
#include "gsp01.h"
#include "RunTime/RTE_Types.hpp"

/*!
  @brief allows a task to wait for an external connect request

  The maximum data size is either set to
    XPARAM(ulPacketSize) - sizeof(comseg_header) - 8 (alignment) - 2 * sizeof(rte_header)
  or to the client given value, if value is smaller but not zero

  @param taskId [in] the own task id 
  @param max_data_len [out] maximum data size that client and kernel agreed upon
  @param returncode [out] possible communication error
 */
extern "C" void
vconnect (
    RTE_TaskId      taskId ,
    SAPDB_Int4    & max_data_len ,
    tsp01_CommErr & returncode 
);
                            
/*!
  @brief allows a task to release the connection

  @param taskId [in] the own task id 
 */
extern "C" void
vrelease (
    RTE_TaskId taskId 
);

/*!
  @brief receive next packet from open connection
  
  This call blocks blocks until the next packet has been received.
  Dont exceed the retruned maximum reply size!
  
  @param taskId [in] the own task id 
  @param timeout [in] the timeout in seconds
  @param rcv_packet_ptr [out] the pointer to the receive packet
  @param rcv_length [out] the length of the received packet in bytes
  @param reply_packet_ptr [out] the pointer to the reply packet
  @param reply_size [out] the maximum reply size in bytes
  @param returncode [out] possible communication error
 */
extern "C" void 
vreceive (
    RTE_TaskId      taskId ,
    SAPDB_Int4      timeout ,
    void        * & rcv_packet_ptr ,
    SAPDB_Int4    & rcv_length ,
    void        * & reply_packet_ptr ,
    SAPDB_Int4    & reply_size ,
    tsp01_CommErr & returncode
);

/*!
  @brief sent reply packet over open connection

  This call blocks does not block until the reply packet has been sent. Dont change content of reply packet
  before next vreceive() call...
  
  @param taskId [in] the own task id 
  @param reply_packet_ptr [out] the pointer to the reply packet
  @param reply_length [out] the actual length of the reply in bytes
  @param returncode [out] possible communication error
 */
extern "C" void 
vreply (
    RTE_TaskId      taskId ,
    void          * reply_packet_ptr ,
    SAPDB_Int4      reply_length ,
    tsp01_CommErr & returncode 
);

/*!
  @brief get application node name and process id for open connection

  @param taskId [in] the own task id 
  @param appl_node [out] the application nodename
  @param appl_pid [out] the application process id
  @param ok [out] true if connected and information valid, false else
 */
extern "C" void 
vgetapplpid (
    RTE_TaskId     taskId ,
    tsp00_NodeId & appl_node ,
    SAPDB_Int4   & appl_pid ,
    SAPDB_Bool   & ok 
);

/*!
  @brief get number of conencted user tasks

  @return number of connected used tasks
 */
extern "C" SAPDB_Int4
vconnectedusertasks ();


#endif  /* RTE_OLDKERNELCOMMUNICATION_HPP */
