/*!
  @file           hos63k.h
  @author         RaymondR
  @brief          kernel to kernel communication
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




#ifndef HOS63K_H
#define HOS63K_H

#include "hos33.h"

// +---------------------------------------------------------------+
// |   RECEIVER TASK COMMUNICATION CONTROL                         |
// +---------------------------------------------------------------+

typedef struct receiver_task_comm_ctrl
  {
  ULONG                           ulRecvSlot;
  ULONG                           ulUseCount;

  ULONG                           ulPathID;
  ULONG                           ulConnected;      // - connect time ( time in sec )
  ULONG                           ulCommState;
  ULONG                           ulGlobCommState;  // - this state can be modfied foreign threads


  HANDLE                          hClientProcess;   // - NT only, local client process handle
  HEV                             hevClientSem;     // - local client semaphore
  struct comm_seg_header_record   *pCSHeader;
  HANDLE                          hCS;              // - NT only, CommSeg handle
  HANDLE                          hPeerFCS;         // - NT local distrib. only
  PFLAG_COMM_SEG                  pPeerFCS;         // - local distrib. only

  ULONG                           ulMaxDataLen;
  ULONG                           ulMaxCmdDataLen;

  PULONG                          pulPeerCommFlag;  // - local distrib. only
  PID                             pidPeerPID;       // - process id of connected peer process
  SQL_NODEIDC                     szPeerNode;
  ULONG                           ulPeerRef;
  BOOL                            fValidPcktRecv;
  struct teo03_CommPacketRecord  *pAcknowledgePacket;
  struct teo03_CommPacketRecord  *pCommPacket;
  } RECEIVER_TASK_COMM_CTRL;

typedef RECEIVER_TASK_COMM_CTRL     *PRECEIVER_TASK_COMM_CTRL;


// +---------------------------------------------------------------+
// |   SENDER TASK COMMUNICATION CONTROL                           |
// +---------------------------------------------------------------+

typedef struct sender_task_comm_ctrl
  {
  ULONG                           ulSendSlot;
  ULONG                           ulUseCount;

  ULONG                           ulPathID;
  ULONG                           ulConnected;      // - connect time ( time in sec )
  ULONG                           ulCommState;
  ULONG                           ulGlobCommState;  // - this state can be modfied foreign threads

  ULONG                           ulPacketSize;
  ULONG                           ulMaxDataLen;
  ULONG                           ulMaxCmdDataLen;
  struct teo03_CommPacketRecord  *pCommPacket;

  SHM_CONNECT_INFO_REC            Shm;   // - vos33c, internal use only!
  } SENDER_TASK_COMM_CTRL;

typedef SENDER_TASK_COMM_CTRL       *PSENDER_TASK_COMM_CTRL;

#define SND_COMM_CTRL(_TaskCtrl) ( (PSENDER_TASK_COMM_CTRL )  (_TaskCtrl->pSenderCommCtrl) )
#define RCV_COMM_CTRL(_TaskCtrl) ( (PRECEIVER_TASK_COMM_CTRL )(_TaskCtrl->pReceiverCommCtrl) )

#endif
