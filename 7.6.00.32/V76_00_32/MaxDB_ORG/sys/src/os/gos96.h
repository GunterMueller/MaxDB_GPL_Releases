/*!
  @file           gos96.h
  @author         FrankH
  @brief          XSERVER
  @see            

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



#ifndef GOS96_H
#define GOS96_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "geo00.h"
#include "gos003.h"
#include "hos23.h"
#include "hos33.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*PRETTY*/
//
// +---------------------------------------------------------------+
// |   DEFINES AND MACROS                                          |
// +---------------------------------------------------------------+
//
#define MSG1(x) {if(gxr.usDebugLevel>=1) sql60_msg_con_diag x;}
#define MSG2(x) {if(gxr.usDebugLevel>=2) sql60_msg_con_diag x;}
#define MSG3(x) {if(gxr.usDebugLevel>=3) sql60_msg_con_diag x;}
#define MSG4(x) {if(gxr.usDebugLevel>=4) sql60_msg_con_diag x;}
#define MSG5(x) {if(gxr.usDebugLevel>=5) sql60_msg_con_diag x;}
#define MSG6(x) {if(gxr.usDebugLevel>=6) sql60_msg_con_diag x;}
#define MSG7(x) {if(gxr.usDebugLevel>=7) sql60_msg_con_diag x;}
#define MSG8(x) {if(gxr.usDebugLevel>=8) sql60_msg_con_diag x;}
#define MSG9(x) {if(gxr.usDebugLevel>=9) sql60_msg_con_diag x;}

#define THRD_INITIALIZED                  1
#define THRD_RUNNING                      2
#define THRD_TERMINATED                   3
#define THRD_SLEEPING                     4
#define THRD_COMMAND_WAIT                 5
#define THRD_COMMAND_SEND                 6
#define THRD_DATA_RECEIVE                 7
#define THRD_DATA_SEND                    8
#define THRD_WAIT_FOR_ACKN                9
#define THRD_SEND_ACKN                    10


#if defined (_WIN64)
#define SERVER_STACK_SIZE                 16384
#define TCPIP_COORD_STACK_SIZE            24576
#else
#define SERVER_STACK_SIZE                 8192
#define TCPIP_COORD_STACK_SIZE            8192
#endif

#define XSERVER_INITIALIZE                1
#define XSERVER_STARTING                  2
#define XSERVER_RUNNING                   3
#define XSERVER_STOPPED                   4
#define XSERVER_ABORT                     5

#define READY_MESSAGE                     "Ready"
#define READY_MESSAGE_LEN                 5

#define MAX_CONTROL_SERVER                20

#define SERVICE_PORT_SEP                  '-'

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

//
// +---------------------------------------------------------------+
// |   THREAD CONTROL HEADER                                       |
// +---------------------------------------------------------------+
//
// --- Header of the the thread control records

typedef struct thread_ctrl_header_record
  {
  TID                             Tid;
  HANDLE                          hThrd;
  LONG                            lThrdPrio;
  INT4                            ThrdState;
  tsp00_Longuint                  ulStartTime;
  HANDLE                          hComm;
  HEV                             hevSem;
  teo00_Longuint                  StackSize;
  } THREAD_CTRL_HEADER_REC;


//
// +---------------------------------------------------------------+
// |   TCPIP COORDINATOR THREAD CONTROL                            |
// +---------------------------------------------------------------+
//

typedef void _System (* tos96_SeverWorkerThreadFkt )( ULONG  ulConnInfoHandle );



typedef struct tcpip_coord_ctrl_record
  {
  THREAD_CTRL_HEADER_REC          ThrdCtrlHeader;
  teo00_Socket                    sd;
  PCHAR                           pServiceStr;
  USHORT                          usServicePort;
  tos96_SeverWorkerThreadFkt      pTCPIPWorkerThreadFkt;
  ULONG                           ulMaxSegmentSize;
  PVOID                           pHandles;
  } TCPIP_COORD_CTRL_REC;

//
// +---------------------------------------------------------------+
// |   TCPIP SERVER THREAD CONTROL                                 |
// +---------------------------------------------------------------+
//
typedef struct tcpip_serv_connect_info_record
  {
  THREAD_CTRL_HEADER_REC          ThrdCtrlHeader;

  ULONG                           ulConnInfoHandle;
  ULONG                           ulConnStatus;

  teo00_Socket                    sd;
  void                           *NiHandle;
  void                           *pSSLHandle;
  SQL_NODEIDC                     szSenderNodeID;

  BOOL                            fAcknowledge;
  ULONG                           ulSwapType;
  ULONG                           ulMaxSegmentSize;
  ULONG                           ulServiceType;
  ULONG                           ulServerRef;
  ULONG                           ulSenderRef;
  SQL_DBNAMEC                     szServerDB;
  ULONG                           ulMaxCmdDataLen;
  ULONG                           ulMinReplySize;
  PCOMM_PACKET_REC                pCommPacket;

  PSECURITY_ATTRIBUTES            pWorldSA;   // --- NT only
  PSECURITY_ATTRIBUTES            pLockSA;    // --- NT only

  SHM_CONNECT_INFO_REC            Shm;   // - vos33c, internal use only!
  void *                          pDBM;
  } TCPIP_SERV_CONNECT_INFO_REC;

typedef TCPIP_SERV_CONNECT_INFO_REC *PTCPIP_SERV_CONNECT_INFO_REC;


//
// +---------------------------------------------------------------+
// |   SERVER THREAD CONTROL                                       |
// +---------------------------------------------------------------+
//
typedef struct serv_connect_info_record
  {
  THREAD_CTRL_HEADER_REC          ThrdCtrlHeader;

  ULONG                           ulConnInfoHandle;
  ULONG                           ulConnStatus;
  BOOL                            fAcknowledge;

  HANDLE                          hCS;            // NT only
  struct comm_seg_header_record   *pCSHeader;
  HANDLE                          hPeerFCS;       // NT local distrib. only
  PFLAG_COMM_SEG                  pPeerFCS;       // local distrib. only
  HANDLE                          hClientProcess; // NT only
  HEV                             hevClientSem;

  PULONG                          pulPeerCommFlag;
  TCPIP_CONNECT_INFO_REC          TCPIP; // - vos23c, internal use only!
  } SERV_CONNECT_INFO_REC;

typedef SERV_CONNECT_INFO_REC *PSERV_CONNECT_INFO_REC;


/************************ SAP NI (network Interface) ****************************/
//
// +---------------------------------------------------------------+
// |   NI COORDINATOR THREAD CONTROL                               |
// +---------------------------------------------------------------+
//

typedef struct ni_coord_ctrl_record
  {
  THREAD_CTRL_HEADER_REC          ThrdCtrlHeader;
  void                           *NiHandle;
  USHORT                          usServicePort;
  ULONG                           ulMaxSegmentSize;
  PVOID                           pHandles;
  } NI_COORD_CTRL_REC;

/************************ SAP NI (network Interface) ****************************/

//
// +---------------------------------------------------------------+
// |   GLOABAL XSERVER RECORD                                      |
// +---------------------------------------------------------------+
//

typedef struct any_server_id_rec
  {
    char                          ServerName[256];
    char                          ServerSemaName[256];
  }  ANY_SERVER_ID_REC;

typedef struct global_xserver_record
  {
  BOOL                           fQuietMode;
  BOOL                           fNTService;
  BOOL                           fPortableInstallation;
  BOOL                           fServicePaused;
  BOOL                           fDebuggerIsDrWatson;
  BOOL                           fNoDNSReverseLookup;
  USHORT                         usDebugLevel;
  ULONG                          ulCrashRc;
  HEV                            hevSemXserv;
  PID                            pid;
  ANY_SERVER_ID_REC              ServerSpecification ;
  tsp00_Versionc                 RTEVersionString;

  SECURITY_ATTRIBUTES            WorldSA;   // --- NT only
  SECURITY_ATTRIBUTES            LockSA;    // --- NT only
  SECURITY_ATTRIBUTES            AdminSA;   // --- NT only

  SERVICE_STATUS_HANDLE          sshStatusHandle;
  SERVICE_STATUS                 ssStatus;
  DWORD                          dwState;

  TCPIP_COORD_CTRL_REC           TCPIPCoord;
  NI_COORD_CTRL_REC              NICoord;
  NI_COORD_CTRL_REC              NISSLCoord; // Support for SSL over NI
  char                           *ControlServer[MAX_CONTROL_SERVER];
  }  GLOBAL_XSERVER_REC;

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/


void 
os96InitConPktParamRec  ( teo003_ConPktParamRecord *pConPktParamRec );

void 
os96InitLocalConnectParamRec (teo003_ConnectParamRecord    *pLocConnParam,
                              teo003_ConPktParamRecord     *pConPktParamRec,
                              TCPIP_SERV_CONNECT_INFO_REC  *pConnInfo ) ;

tsp00_Uint     
os96OpenListeningSocket ( char           *pServiceName,
                          tsp00_Uint2    *pServicePort,
                          teo00_Socket   *pSd,
                          tsp00_Uint     *pMaxSegmentSize );

tsp00_Uint    
os96SetSockBufSizes     ( teo00_Socket    sd,
                          tsp00_Uint      MinSegmentSize,
                          tsp00_Uint      *pMaxSegmentSize );

tsp00_Int    
os96StartTCPIPCoord     ( TCPIP_COORD_CTRL_REC *pTCPIPCoord ) ;

DWORD        os968ExceptionHandler     ( LPEXCEPTION_POINTERS lpExcPtrs );
VOID         sql96c_NewSrvState        ( DWORD                dwState,
                                         ULONG                ulExitCode );

/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/


#endif  /* GOS96_H */
