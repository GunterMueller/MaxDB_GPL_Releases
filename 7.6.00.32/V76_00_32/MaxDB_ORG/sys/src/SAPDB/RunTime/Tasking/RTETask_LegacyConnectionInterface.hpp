   /*!========================================================================
   
     @file         RTETask_LegacyConnectionInterface.hpp
     @ingroup      Runtime
     @author       StefanP
   
     @brief        
   
                   
   
     @since        2005-11-03  16:16
     @sa           
   
     ==========================================================================
   
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
    ============================================================================*/
   
   
#ifndef RTETASK_LEGACYCONNECTIONINTERFACE_HPP
#define RTETASK_LEGACYCONNECTIONINTERFACE_HPP
   
   /*===========================================================================*
    *  INCLUDES                                                                 *
    *===========================================================================*/
   
#include    "RunTime/Tasking/RTETask_ITaskClientConnection.hpp"
   
   
   /*===========================================================================*
    *  DEFINES                                                                  *
    *===========================================================================*/
   
   
   /*===========================================================================*
    *  MACROS                                                                   *
    *===========================================================================*/
   
   
   /*===========================================================================*
    *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
    *===========================================================================*/
   
/*!
  @brief get task encryption type
  @param taskId [in] task id
  @return task encryption type (UnknownEncryption, NoEncryption, NISSL)
 */
RTETask_ITaskClientConnection::EncryptionType     RTETask_EncryptionType  (RTE_TaskId      taskId);
   
  
/*!
  @brief get task connection type
  @param taskId [in] task id
  @return task connection type (UnknownConnection, Utility, Ping, Event, OLTP, LiveCache)
 */
RTETask_ITaskClientConnection::ConnectionType     RTETask_ConnectionType  (RTE_TaskId      taskId);
   
#endif  /* RTETASK_LEGACYCONNECTIONINTERFACE_HPP */
