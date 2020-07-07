/*!
  @file           gos003.h
  @author         FrankH
  @brief
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



#ifndef GOS003_H
#define GOS003_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "gsp01.h"
#include "gos00.h"
#include "geo003.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define NODESEPARATORLIST               "!#:"
#define MAXPACKET                       100


#define MAXCONNECTIONS                  2000
#define MIN_COMSEG_SIZE                 8192
#define RETRYCOUNT                      3
#define LOCAL_CONN_RETRY_SLEEP_TIME     3            // - sec
#define LOCAL_CONN_TIMEOUT              120          // - sec
#define SOCKET_CONN_TIMEOUT             180          // - sec
#define MIN_COMMAND_TIMEOUT             30           // - sec
#define MIN_UNREL_CONN_TEST             120          // - sec

#define CS_IN_USE                       0x1
#define CS_CONN_ESTABLISHED             0x2
#define CS_CONN_RELEASED                0x4


/*
 *  local request types
 */
#define SQL_RTE_WAKEUP                  0
#define SQL_RTE_CONNECT                 1
#define SQL_RTE_CANCEL                  2

#define SQL_RTE_DUMP                    15   // -- max.= 15

/*
 *  communication segment version
 */
#define COMM_SEG_VERSION_WITH_SEMNAME_REPLY     1
#define COMM_SEG_VERSION_LOAD_BALANCING         2   //Semaphore names are transfered in the Flag Communication Segment 
#define COMM_SEG_VERSION_WITH_LONG_APPL_PID     3   //Client PID is transfered as 8 byte value. This is necessary for the
                                                    //communication between XServer and Kernel because the XServer has to
                                                    //receives the client PID as 8 Byte value in case of 
                                                    //Client Kernel Communication between UNIX and Windows (pid_t <-> DWORD)
#define COMM_SEG_VERSION_WITH_ENCRYPTION_INFO   4   //
                                                    //
                                                    //
                                                    //

//#define COMM_SEG_VERSION                        COMM_SEG_VERSION_WITH_LONG_APPL_PID
#define COMM_SEG_VERSION                        COMM_SEG_VERSION_WITH_ENCRYPTION_INFO


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


typedef tsp00_C2       C2;
typedef tsp00_C4       C4;
typedef tsp00_C8       C8;

//
// -- RTE HEADER for communication packets.
//
typedef teo003_RteHeaderRecord        RTE_HEADER_REC;
typedef teo003_RteHeaderRecord       *PRTE_HEADER_REC;


typedef teo003_CommPacketRecord      COMM_PACKET_REC  ;
typedef teo003_CommPacketRecord      *PCOMM_PACKET_REC;

// +---------------------------------------------------------------+
// |   USER TASK COMMUNICATION CONTROL                             |
// +---------------------------------------------------------------+

typedef struct user_task_comm_ctrl
  {
  tsp00_Longuint                  ulConnected;          // - task connected to a user ( time in sec )
  ULONG                           ulCommState;
  ULONG                           ulGlobCommState;      // - this state can be modfied by foreign threads

  tsp00_Longuint                  ulCmdWaitTime;

  HANDLE                          hLocalClientProcess;  // - local client process handle
  PID                             pidLocalClientPID;
  HANDLE                          hClientSem;
  struct comm_seg_header_record   *pCSHeader;
  HANDLE                          hCS;                  // - CommSeg handle

  PID                             pidClientPID;         // - is equal to 'hLocalClientProcess' on none remote connects
  SQL_NODEIDC                     szClientNode;
  ULONG                           ulClientRef;
  ULONG                           ulMaxDataLen;
  ULONG                           ulMaxCmdDataLen;
  ULONG                           ulLastRequestSize;    // for dump, since reply header overwrites request header...
  struct teo003_CommPacketRecord  *pReplyCommPacket;
  ULONG                           ulPacketCnt;
  struct teo003_CommPacketRecord  *pCommPacketList[MAX_SQL_PACKETS];
  ULONG                           ulServiceType;
  SAPDB_UInt1                     encryption;           // - encryption used for the session (SSL) 
  } USER_TASK_COMM_CTRL, tos00k_UserTaskCommCtrl;

typedef USER_TASK_COMM_CTRL       *PUSER_TASK_COMM_CTRL;

/*
 * - Structure of the remote communication record
 *
 *    Remote connect:
 *
 *      +------------------------------------+
 *      |     rte_header_record              |
 *      +------------------------------------+
 *      |     rte_connect_packet_record      |
 *      +------------------------------------+
 *
 *    Remote transfer:
 *
 *      +------------------------------------+
 *      |     comm_packet_record             |
 *      +---------------------------\--------+
 *                                   \
 *                                  +-\----------------------------------+
 *                                  |  \  rte_header_record +            |
 *                                  |     sql_packet (data)              |
 *                                  +------------------------------------+
 *
 *
 */

// --- Record is used for REMOTE connects only

typedef teo003_RteConnectPacketRec    RTE_CONNECT_PACKET_REC ;
typedef teo003_RteConnectPacketRec   *PRTE_CONNECT_PACKET_REC;

typedef teo003_ConnectParamRecord    CONNECT_PARAM_REC ;
typedef teo003_ConnectParamRecord   *PCONNECT_PARAM_REC;

//
// ---  Local connect request (via message queue to REQUESTOR thread)
//
//
#if defined(_WIN32)

typedef struct comm_request_record
  {
  ULONG                           ulReqType;
  ULONG                           ulServerRef;
  ULONG                           ulCSSize;
  IPC_BASE_OBJ_NAME               szClientObjName;
  } COMM_REQUEST_REC;

#else

typedef struct comm_request_record  // - must not be greater than 32 Bit
  {
  INT                             bf28ServerRef: 28,
                                  bf4ReqType   : 4;
  } COMM_REQUEST_REC;

#endif

/*
 *  - Structure of the communication segment
 *
 *       +------------------------------------+
 *    +--|     comm_seg_header_record         |
 *    |  +------------------------------------+
 *    +->|     comm_seg_offset_record         |--+
 *       +------------------------------------+  |
 *       | 1.  comm_packet_record             |<-+
 *       |     (request)                      |  |
 *       |  --------------------------------  |  |
 *       |     comm_packet_record             |  |
 *       |     (receive)                      |  |
 *       +------------------------------------+  |
 *       .                                    .  .
 *       .                                    .  .
 *       +------------------------------------+  |
 *       | n.  comm_packet_record             |<-+
 *       |     (request)         \            |
 *       |  ----------------------\---------  |
 *       |     comm_packet_record  \          |
 *       |     (receive)            \         |
 *       +---------------------------\--------+
 *                                    \
 *      n <= MAX_SQL_PACKETS           \
 *                                +-----+------------------------------+
 *                                |     rte_header_record +            |
 *                                |     sql_packet (data)              |
 *                                +------------------------------------+
 *
 *
 *    The usage of ulClientFlag and ulServerFlag is as follows:
 *
 *
 *                             c s
 *                     init->  -+-
 *                             0 0
 *                                \
 *                          sqlaconnect,vconnect
 *                                /
 *                 c s         c s
 *                 -+-         -+-
 *                 0 2         0 2
 *                /   \       /
 *      sqlareceive   sqlarequest,sqlarelease
 *              /       \   /
 *           c s         c s
 *           -+-         -+-          0: no event
 *           2 1         1 2          1: event pending
 *              \       /             2: destination has received the event
 *           vreply   vreceive
 *                \   /
 *                 c s
 *                 -+-
 *                 2 0
 *
 *
 *
 *    if the communication is crashing:
 *
 *      kernel:   c s         client:   c s
 *                -+-                   -+-
 *                ? 1                   1 ?
 *
 *
 */

// --- local communication segment header

typedef struct comm_seg_header_record
  {
  ULONG                           ulCSSize;
  INT                             iCSVersion;
  ULONG                           ulPacketCnt;
  ULONG                           ulPacketSize;
  ULONG                           ulMaxDataLen;
  ULONG                           ulMinReplySize;
  ULONG                           ulOffsetRecord;
  ULONG                           ulCurrentPacket;
  PID                             pidClientOrPeerPID;
  PID                             pidServerPID;
  ULONG                           ulClientOrPeerRef;
  ULONG                           ulServerRef;
  ULONG                           ulClientCommState;
  ULONG                           ulServerCommState;
  ULONG                           ulClientFlag;
  ULONG                           ulServerFlag;
  ULONG                           ulServiceType;

  // -- operating system specific part
  ULONG                           ulCommFlagNo;
  ULONG                           ulSemaNo;

  SQL_NODEIDC                     szClientOrPeerNode; // comm. xserver/kernel only

  PID                             pidLocalPID;
  CHAR                            szSemPath[ 40 ];
  IPC_BASE_OBJ_NAME               szSemName;


  SQL_DBNAMEC                     szClientOrPeerServerDB; // distrib. only
  SQL_NODEIDC                     szServerNode;           // distrib. only
  SQL_DBNAMEC                     szServerDB;             // distrib. only
  SAPDB_Int8                      applPID;                // PID of the client application 
                                                          // (== pidLocalPID: Local communication
                                                          //  != pidLocalPID: remote communication )
  SAPDB_UInt1                     encryption;               
  } COMM_SEG_HEADER_REC;

typedef COMM_SEG_HEADER_REC       *PCOMM_SEG_HEADER_REC;

#define COMM_SEG_HEADER_SIZE  (sizeof(COMM_SEG_HEADER_REC))

// --- offsets to the communication packets
typedef struct comm_seg_offset_record
  {
  ULONG                           ulOffsetCnt;
  ULONG                           ulOffset[MAX_SQL_PACKETS];
  } COMM_SEG_OFFSET_REC;

typedef COMM_SEG_OFFSET_REC       *PCOMM_SEG_OFFSET_REC;

#define COMM_OFFSET_LIST_SIZE     (sizeof(COMM_SEG_OFFSET_REC))


//
// --- OLD RTE HEADER  and SQL packets.
//

#define OLD_RTE_HEADER_PART                                \
        INT2                            ActSendLen;        \
        INT2                            MaxSendLen;        \
        INT1                            ProtocolID;        \
        INT1                            MessClass;         \
        INT1                            RTEFlags;          \
        INT1                            ResidualPackets;   \
        INT4                            SenderRefID;       \
        INT4                            ReceiverRefID;     \
        INT2                            RTEReturnCode;     \
        INT2                            Filler;


typedef struct old_rte_header_record
  {
  OLD_RTE_HEADER_PART
  } OLD_RTE_HEADER_REC;

typedef OLD_RTE_HEADER_REC       *POLD_RTE_HEADER_REC;

#define OLD_RTE_HEADER_SIZE     (sizeof(OLD_RTE_HEADER_REC))


typedef struct old_sql_packet_record
  {
  OLD_RTE_HEADER_PART

  C2                              mess_code;
  INT2                            mess_type;
  C8                              senderid;
  INT2                            part1_length;
  INT2                            part2_length;
  INT2                            return_code;
  INT2                            error_code;
  char                            var_part [ 8240 ];
  } OLD_SQL_PACKET_REC;

typedef OLD_SQL_PACKET_REC      *POLD_SQL_PACKET_REC;

typedef struct old_shm_info_record
  {
  SQL_DBNAMEC                     szServerDB;
  ULONG                           ulKernelRef;
  ULONG                           ulApplicRef;
  tsp01_Service                   ServiceType;

  HANDLE                          hCommHandle;
  HANDLE                          hevApplicSem;
  HANDLE                          hevUKTSem;
  struct old_comm_segment_record  *pCommSeg;
  HANDLE                          hCommSeg;      // - NT
  IPC_BASE_OBJ_NAME               szUKTSemName;  // - NT
  } OLD_SHM_INFO_REC;

typedef OLD_SHM_INFO_REC          *POLD_SHM_INFO_REC;

#if defined (__cplusplus)

class TOSCM_COMM_CLASS
{
public:
    virtual tsp01_CommErr_Enum connect (
                               CONNECT_PARAM_REC       *pConnParam,
                               PSZ                      pszServerDB,
                               PSZ                      pszDBRoot,
                               PSZ                      pszServerPgm,
                               tsp00_ErrTextc  VAR_ARRAY_REF           errtext)
                                = 0;

    virtual tsp01_CommErr_Enum release () = 0;

    virtual tsp01_CommErr_Enum request (
                               void *                           sql_packet,
                               tsp00_Int4                       length,
                               tsp00_ErrTextc  VAR_ARRAY_REF    errtext)
                                = 0;

    virtual tsp01_CommErr_Enum receive (
                               void **                          sql_packet,
                               tsp00_Int4         VAR_VALUE_REF length,
                               tsp00_ErrTextc  VAR_ARRAY_REF    errtext)
                                = 0;

    virtual tsp01_CommErr_Enum replyAvailable (
                               tsp00_ErrTextc  VAR_ARRAY_REF    errtext)
                                = 0;

    virtual tsp01_CommErr_Enum cancel (
                               tsp00_ErrTextc  VAR_ARRAY_REF    errtext)
                                = 0;
};

typedef TOSCM_COMM_CLASS *TOSCM_COMMSTRUCTOR_FT (
                               void *                           commInfo,
                               tsp00_ErrTextc  VAR_ARRAY_REF    errtext,
                               tsp01_CommErr_Enum *                  returncode);


#endif

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/


/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/

//
// --- connection parameter  ( sql33c_connect, sql23c_connect )
//
// --- VOS53KC ----------------------------------------------------------------

externC VOID           vconnect          ( PROCESS_ID                   pid,
                                           tsp00_Int4 VAR_VALUE_REF     max_data_len,
                                           tsp01_CommErr VAR_VALUE_REF  returncode );


externC tsp00_Int4     vcmd_wait_time    ( PROCESS_ID      pid );

externC VOID           vreceive          ( PROCESS_ID                   pid,
                                           tsp00_Int4                   timeout,
                                           char                      ** rcv_packet_ptr,
                                           tsp00_Int4  VAR_VALUE_REF    rcv_packet_length,
                                           char                      ** rpl_packet_ptr,
                                           tsp00_Int4 VAR_VALUE_REF     rpl_packet_size,
                                           tsp01_CommErr VAR_VALUE_REF  returncode );

externC VOID           vreply            ( PROCESS_ID                   pid,
                                           char                         *res_packet_ptr,
                                           tsp00_Int4                   res_packet_length,
                                           tsp01_CommErr VAR_VALUE_REF  returncode );

externC VOID           vrelease          ( PROCESS_ID                   pid );


externC tsp00_Int4     vconnectedusertasks ();

externC VOID	       vgetapplpid       ( PROCESS_ID                   tid ,
                                           tsp00_NodeId VAR_ARRAY_REF   appl_node ,
                                           tsp00_Int4   VAR_VALUE_REF   appl_pid ,
                                           BOOLEAN      VAR_VALUE_REF   ok );

#endif  /* GOS01_H */
