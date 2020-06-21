/*!
 * \file    LVC_ProcServerInterfaceInstance.cpp
 * \author  MarkusSi
 * \brief   Interface for communicating between ProcServer and kernel.
 */
/*

    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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



*/

#include "liveCache/LVC_ProcServerInterfaceInstance.hpp"

#define KERNEL_LZU
#include    "RunTime/Tasking/RTETask_ITask.hpp"
#include    "RunTime/Tasking/RTETask_ITaskClientConnection.hpp"
#include    "RunTime/RTE_Message.hpp"
#include    "RunTime/UDEServer/RTEUDE_ServerControl.hpp"
#include    "RunTime/Tasking/RTETask_Task.hpp"
#include    <string.h>


/*----------------------------------------------------------------------------*/

LVC_ProcServerInterfaceInstance::LVC_ProcServerInterfaceInstance(SAPDBMem_IRawAllocator *pAlloc) 
  : LVC_ProcServerInterface(pAlloc)
  , m_pConnection(NULL)
  , m_pBuffer(NULL)
{
  SAPDBErr_MessageList                    messageList;

  RTEUDE_ServerControl UDEServerControl;
  RTETask_ITask&       task = *RTETask_ITask::Self();

  // start Connection   
  RTEUDE_ServerControl::ConnRet    serverReturn;
  serverReturn = UDEServerControl.Start(task, "liveCacheProcServer.exe", messageList );
  if (serverReturn != RTEUDE_ServerControl::NoError ){
    RTE_Message ( messageList, MSG_DIAG );
    return;
  }
  
  m_pConnection        = UDEServerControl.AcceptClientConnect( task );
  m_maxSendDataLength  = m_pConnection->MaxSendDataLength();

  // Wait for reply from client and check whether connection is established
  SAPDB_UInt4 dataLen = 0;
  RTETask_ITaskClientConnection::ConnRet  taskReturn;
  taskReturn = m_pConnection->Receive( 0, m_pBuffer, dataLen, messageList );
  if (  RTETask_ITaskClientConnection::NoError != taskReturn 
     || strcmp(reinterpret_cast<SAPDB_Char*>(m_pBuffer), "OK") != 0){
      RTE_Message ( messageList, MSG_DIAG );
      m_pConnection->Release();
      m_pConnection = NULL;
  }
}

/*----------------------------------------------------------------------------*/

LVC_ProcServerInterfaceInstance::~LVC_ProcServerInterfaceInstance() 
{
  m_pBuffer = NULL;

  if (m_pConnection){
    m_pConnection->Release();
    m_pConnection = NULL;
  }
}

/*----------------------------------------------------------------------------*/

void LVC_ProcServerInterfaceInstance::getSendArea(void* &addr, size_t &maxSize)
{
  addr    = m_pBuffer;
  maxSize = m_maxSendDataLength;
}

/*----------------------------------------------------------------------------*/

void LVC_ProcServerInterfaceInstance::getSecondarySendArea(void* &addr, size_t &maxSize)
{
  // TODO: not implemented
}

/*----------------------------------------------------------------------------*/

void LVC_ProcServerInterfaceInstance::getReceiveArea(void* &addr, size_t &maxSize)
{
  addr    = m_pBuffer;
  maxSize = m_maxSendDataLength;
}

/*----------------------------------------------------------------------------*/

void LVC_ProcServerInterfaceInstance::sendData(size_t size)
{
  RTETask_ITaskClientConnection::ConnRet  ret;
  SAPDBErr_MessageList                    messageList;

  // Sending packet
  ret = m_pConnection->Send( m_pBuffer, size, messageList );

  if ( RTETask_ITaskClientConnection::NoError != ret ){
    RTE_Message ( messageList, MSG_DIAG );
    m_pConnection->Release();
    return;
  }
}

/*----------------------------------------------------------------------------*/

void LVC_ProcServerInterfaceInstance::receiveData()
{
  RTETask_ITaskClientConnection::ConnRet  ret;
  SAPDB_UInt4                             dataLen;
  SAPDBErr_MessageList                    messageList;

  // Blocking wait for a packet 
  ret = m_pConnection->Receive( 0, m_pBuffer, dataLen, messageList );

  if ( RTETask_ITaskClientConnection::NoError != ret ){
    RTE_Message ( messageList, MSG_DIAG );
    m_pConnection->Release();
  }
}

/*----------------------------------------------------------------------------*/

size_t LVC_ProcServerInterfaceInstance::getOffsetForAddress(void *addr)
{
  // TODO: not implemented
  return 0;
}

/*----------------------------------------------------------------------------*/

void* LVC_ProcServerInterfaceInstance::getAddressForOffset(size_t offs)
{
  // TODO: not implemented
  return NULL;
}

/*----------------------------------------------------------------------------*/
