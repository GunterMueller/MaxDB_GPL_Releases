/*!
  @file           geo003.h
  @author         JoergM
  @special area   communication
  @brief          description ...
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



#ifndef GEO003_H
#define GEO003_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#if defined (_WIN32)

#define NEEDED_WIN32_WINNT 0x0500

#if !defined(_WIN32_WINNT)
# define _WIN32_WINNT  NEEDED_WIN32_WINNT
#else
# if _WIN32_WINNT < NEEDED_WIN32_WINNT
#  undef _WIN32_WINNT
#  define _WIN32_WINNT  NEEDED_WIN32_WINNT
# endif
#endif

#include <windows.h>
#endif

#include "gsp00.h"
#include "geo001.h"
#include "heo01.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*
 * --- socket - communication
 */
#define SOCKET_PROTOCOL_EO003                  "tcp"
#define SOCKET_SERVICE_SQL30_EO003             "sql30"
#define DEFAULT_PORT_SERVICE_SQL30_EO003            7200
#define SOCKET_SERVICE_SQL6_EO003              "sql6"
#define DEFAULT_PORT_SERVICE_SQL6_EO003             7210
#define SOCKET_SERVICE_NI72_EO003              "sapdbni72"
#define DEFAULT_PORT_SERVICE_NI72_EO003             7269
#define SOCKET_SERVICE_NISSL76_EO003           "sdbnissl76"
#define DEFAULT_PORT_SERVICE_NISSL76_EO003          7270
#define SOCKET_SERVICE_DBMSRV_EO003            "dbmsrv"


#define MAX_SQL_PACKETS_EO003         2
#define MAX_SQL_PACKETS               MAX_SQL_PACKETS_EO003

/*
 *  States for DB-Communication
 */
#define CON_UNUSED_EO003                      0
#define CON_INITIALIZED_EO003                 1
#define CON_CONNECTING_EO003	                2
#define CON_CONNECTED_EO003                   3
#define CON_REQUESTED_EO003                   4
#define CON_RECEIVED_EO003                    5
#define	CON_REPLIED_EO003                     6
#define CON_CANCELLED_EO003                   7
#define CON_RELEASED_EO003                    8
#define CON_KEEP_ALIVE_EO003                  9
#define	CON_TIMEDOUT_EO003                   11



/* for old code only */
#define	CON_UNUSED		      CON_UNUSED_EO003
#define CON_INITIALIZED     CON_INITIALIZED_EO003
#define	CON_CONNECTING		  CON_CONNECTING_EO003
#define	CON_ESTABLISHED		  CON_CONNECTED_EO003
#define CON_CONNECTED       CON_CONNECTED_EO003
#define CON_REQUESTED       CON_REQUESTED_EO003
#define CON_RECEIVED        CON_RECEIVED_EO003
#define	CON_REPLIED         CON_REPLIED_EO003
#define CON_RELEASED        CON_RELEASED_EO003
#define CON_CANCELLED       CON_CANCELLED_EO003
#define CON_ABORTED         CON_CANCELLED_EO003
#define	CON_TIMEDOUT        CON_TIMEDOUT_EO003

/*
 * used for db-manager only
 */
#define RSQL_CTRL_PACKET_SIZE_EO003    16384
#define RSQL_CTRL_MIN_REPLY_SIZE_EO003 100

/*
 * local protocol_id
 */
#define PROT_UNDEF_EO003                0
#define PROT_SHM_EO003                  1
#define PROT_BIGSHM_EO003               2
#define PROT_SOCKET_EO003               3
#define PROT_NI_EO003                   4
#define PROT_LOCAL_CONTROL_EO003        5
#define PROT_VMTDISPATCH_EO003          6
#define NPROTOCOLS_EO003                6
#define PROT_NISSL_EO003                7
#define PROT_SSL_EO003                  8


/*
 * remote protocol_id
 */
#define RSQL_RTE_PROT_LOCAL_EO003             0
#define RSQL_RTE_PROT_NI_EO003                1
#define RSQL_RTE_PROT_TCP_EO003               3

/*
 *  mess_class  ( old )
 */
#define RSQL_RTE_REQUEST_CONN_EO003           1
#define RSQL_RTE_REPLY_CONN_EO003             2
#define RSQL_RTE_REQUEST_USER_EO003           3
#define RSQL_RTE_REPLY_USER_EO003             4
#define RSQL_RTE_OPEN_SESSION_EO003           16
#define RSQL_RTE_CLOSE_SESSION_EO003          17
#define RSQL_RTE_SEND_DATA_EO003              18
#define RSQL_RTE_OPEN_SESSION_REPLY_EO003     19
#define RSQL_RTE_CANCEL_EO003                 31
#define RSQL_RTE_DUMP_EO003                   32
#define RSQL_RTE_ADDRESS_REQUEST_EO003        41
#define RSQL_RTE_ADDRESS_REPLY_EO003          42

/*
 *  mess_class  ( new )
 */
#define RSQL_INFO_REQUEST_KEEP_ALIVE_EO003    49 /*change from 50 to 49 due to PTS 1140311*/
/*old #define RSQL_INFO_REQUEST_KEEP_ALIVE_EO003    50      <<<--- do not reuse 50 or old kernels will crash... */
#define RSQL_INFO_REQUEST_EO003               51
#define RSQL_INFO_REPLY_EO003                 52

#define RSQL_PING_REQUEST_EO003               53
#define RSQL_PING_REPLY_EO003                 54

#define RSQL_NEW_DEBUG_LEVEL_REQUEST_EO003    55
#define RSQL_NEW_DEBUG_LEVEL_REPLY_EO003      56

#define RSQL_MIGRATE_REQUEST_EO003            57
#define RSQL_MIGRATE_REPLY_EO003              58

/* !!! dbname in varpart only tsp00_C8 !!! */
#define RSQL_USER_CONN_REQUEST_EO003          61

#define RSQL_USER_CONN_REPLY_EO003            62
#define RSQL_USER_DATA_REQUEST_EO003          63
#define RSQL_USER_DATA_REPLY_EO003            64
#define RSQL_USER_CANCEL_REQUEST_EO003        65
#define RSQL_USER_RELEASE_REQUEST_EO003       66

#define RSQL_KERN_CONN_REQUEST_EO003          71
#define RSQL_KERN_CONN_REPLY_EO003            72
#define RSQL_KERN_DATA_REQUEST_EO003          73
#define RSQL_KERN_DATA_REPLY_EO003            74
#define RSQL_KERN_RELEASE_REQUEST_EO003       76

#define RSQL_DUMP_REQUEST_EO003               81

#define RSQL_CTRL_CONN_REQUEST_EO003		  91
#define RSQL_CTRL_CONN_REPLY_EO003	    	  92
#define RSQL_CTRL_CANCEL_REQUEST_EO003	      93

#define RSQL_CERT_REQUEST_EO003               94
#define RSQL_CERT_REPLY_EO003                 95

/* !!! dbname in varpart only tsp00_DbName !!! */
#define RSQL_NEW_USER_CONN_REQUEST_EO003      100

/*
 *  rte flags
 */
#define RSQL_NORMAL_EO003                     0
#define RSQL_COMMIT_EO003                     1

/*
 *  ostype
 */
#define RSQL_MVSBATCH_OS_EO003                0
#define RSQL_MVSCICS_OS_EO003                 1
#define RSQL_MVSTSO_OS_EO003                  2
#define RSQL_VSEBATCH_OS_EO003                3
#define RSQL_VSECICS_OS_EO003                 4
#define RSQL_VMSP_OS_EO003                    5
#define RSQL_VMCICS_OS_EO003                  6
#define RSQL_VMS_OS_EO003                     7
#define RSQL_MSDOS_OS_EO003                   8
#define RSQL_OS2_OS_EO003                     9
#define RSQL_BS2000_OS_EO003                  10
#define RSQL_UNIX_OS_EO003                    11
#define RSQL_SINIX_OS_EO003                   12
#define RSQL_WIN32_EO003                      13

#if defined (_WIN32)
 #define RSQL_OS_TYPE_EO003   RSQL_OS2_OS_EO003
#else
 #define RSQL_OS_TYPE_EO003   RSQL_WIN32_EO003
#endif


/*
 *  local request types
 */
#define SQL_RTE_CONNECT_EO003                 1
#define SQL_RTE_CANCEL_EO003                  2

#define SQL_RTE_DUMP_EO003                    15   /* -- max.= 15 */

/*===========================================================================*/

#define RSQL_OPT_PORT_NO_EO003           0x50   /* = P */
#define RSQL_OPT_REM_PID_EO003           0x49   /* = I */
#define RSQL_OPT_ACKNOWLEDGE_EO003       0x52   /* = R */
#define RSQL_OPT_NODE_EO003              0x3E   /* = N */
#define RSQL_OPT_AUTH_ALLOW_EO003        0x61   /* = a string */
#define RSQL_OPT_DBROOT_EO003            0x64   /* = d */
#define RSQL_OPT_SERVERPGM_EO003         0x70   /* = p */
#define RSQL_OPT_OMIT_REPLY_PART_EO003   0x72   /* = r int1 */
#define	RSQL_OPT_VERSION_EO003	         0x56	/* = V PTS 1105295 */

#define	RSQL_OPT_ALTER_SERV_SEM_EO003	 0x41	/* = A int1,     local only */
#define	RSQL_OPT_PKTCNT_EO003	    	 0x43	/* = C int4,     local only */
#define	RSQL_OPT_PID_EO003	    	     0x44	/* = D int4,     local only */
#define	RSQL_OPT_SEMID_EO003		     0x53	/* = S int4,     local only */
#define	RSQL_OPT_SHMID_EO003		     0x4D	/* = M int4,     local only */
#define	RSQL_OPT_FIFO_EO003            	 0x46	/* = F string,   local only */
#define	RSQL_OPT_OFFSET_EO003	     	 0x4F	/* = O long,     local only */
#define	RSQL_OPT_ENCRYPTION_EO003    	 0x45	/* = E long,     local only */



/*
 *  Encryption types
 */
//!!!DO NOT FORGET TO EDIT ALSO ENUMERATION IN RTETASK_ITASKCLIENTCONNECTION.HPP
#define RTE_NO_ENCRYPTION               0x01
#define RTE_NI_SSL_ENCRYPTION           0x02



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

typedef struct teo003_RteHeaderRecord
  {
  tsp00_Int4                            ActSendLen; 
  tsp00_Uint1                           ProtocolID;
  tsp00_Uint1                           MessClass;
  tsp00_Uint1                           RTEFlags;
  tsp00_Uint1                           ResidualPackets;
  tsp00_Int4                            SenderRef;
  tsp00_Int4                            ReceiverRef;
  tsp00_Int2                            RTEReturnCode;
  tsp00_Uint1                           NewSwapType;
  tsp00_Uint1                           Filler1;
  tsp00_Int4                            MaxSendLen;
    /*
     *  Should be 8-byte aligned to avoid alignment computations.
     *
     *  ActSendLen      includes the rte_header.
     *  MaxSendLen      includes one rte_header regardless of the number
     *                  of segments. This means, that the sum of all
     *                  segment's rh_act_send_len is greater than
     *                  rh_max_send_len unless the number of segments is 1.
     */
  } teo003_RteHeaderRecord; 

#define RTE_HEADER_SIZE_EO003      (sizeof(teo003_RteHeaderRecord))
#define RTE_HEADER_SIZE            RTE_HEADER_SIZE_EO003

typedef struct teo003_CommPacketRecord
  {
  teo003_RteHeaderRecord                  RTEHeader;
  char                                   pDataPart[1];
  } teo003_CommPacketRecord;

typedef struct teo003_RteConnectPacketRec
  {
  tsp00_C2                              sMessCode;
  tsp00_Int2                            ConnectLength;
  tsp00_Uint1                           ServiceType;
  tsp00_Uint1                           OSType;
  tsp00_Uint1                           Filler1;
  tsp00_Uint1                           Filler2;
  tsp00_Int4                            MaxSegmentSize;
  tsp00_Int4                            MaxDataLen;
  tsp00_Int4                            PacketSize;
  tsp00_Int4                            MinReplySize;
#ifdef DO_NOT_USE_THIS_IT_IS_CORRECT_BUT_INKOMPATIBLE
  tsp00_DbName                          ReceiverServerDB;
  tsp00_DbName                          SenderServerDB;
#else
  tsp00_C8                              ReceiverServerDB;
  tsp00_C8                              SenderServerDB;
#endif
  tsp00_C256                            VarPart;
  } teo003_RteConnectPacketRec;



typedef struct RTESec_CertificateReplyPacket
  {
    SAPDB_UInt1                         swapType;
    SAPDB_Byte                          pCertificate[1];
  } RTESec_CertificateReplyPacket;

typedef struct teo003_ConPktParamRecord
  {
  tsp00_Uint                      ulMessClass;
  tsp00_Uint                      ulSenderRef;
  tsp00_Uint                      ulReceiverRef;
  tsp00_Uint                      ulServiceType;
  tsp00_Uint                      ulMaxSegmentSize;
  tsp00_Uint                      ulPacketSize;
  tsp00_Uint                      ulMaxDataLen;
  tsp00_Uint                      ulMinReplySize;
  tsp00_Uint                      ulSwapType;
  tsp00_DbNamec                   szSenderServerDB;
  tsp00_DbNamec                   szReceiverServerDB;
  teo001_Pid                      pidSenderPID;
  tsp00_Uint2                     usServicePort;
  tsp00_Uint                      ulCommState;
  tsp00_Bool                      fAcknowledge;
  tsp00_Bool                      fOmitReplyPart;
  tsp00_Pathc                     szServerDBRoot;
  tsp00_Pathc                     szServerPgm;
  char                            authenticationAllow[256];
  tsp00_Pathc                     szVersion;          /* PTS 1105295 */
  tsp00_Pathc                     szOptPeerNode;      /* UNIX local connect only */
  tsp00_Pathc                     szOptFiFo;          /* UNIX local connect only */
  tsp00_Int4                      SemId;              /* UNIX local connect only */
  tsp00_Int4                      ShmId;              /* UNIX local connect only */
  tsp00_Int4                      PacketCnt;          /* UNIX local connect only */
  tsp00_Uint                      BigComsegOffset;    /* UNIX local connect only */
  teo001_Pid                      LocalApplPID;       /* UNIX local connect only */
  } teo003_ConPktParamRecord;

typedef struct connect_param_record
  {
  /* - input - */
  char                             *pszServerDB;
  char                             *pszServerNode;
  char                             *pszServerDBRoot;  /* control server only */
  char                             *pszServerPgm;     /* control server only */
  tsp00_Uint                        ulServiceType;
  tsp00_Uint                        ulPacketCnt;
  tsp00_Uint                        ulClientRef;
  teo001_Pid                        pidClientPID;
  char                             *pszClientNode;    /* comm. xserver/kernel only */
  tsp00_Bool                        isSSL;                     /* SSL connection */
  tsp00_Bool                        isSSLoverSAPRouterString;  /* SSL connection over SAPRouter string */
  tsp00_Bool                        ignoreHostnameInServerCert;

  /* - input/output - */
  tsp00_Uint                        ulPacketSize;
  tsp00_Uint                        ulMaxDataLen;
  tsp00_Bool                        fAcknowledge;

  /* - output - */
  tsp00_Uint                        ulServerRef;
  tsp00_Uint                        ulMinReplySize;
  struct teo003_CommPacketRecord    *pCommPacketList[MAX_SQL_PACKETS];
  char                              authenticationAllow[256];
  SAPDB_UInt1                       encryption;
/*==========*/
#ifdef _WIN32
/*==========*/
  tsp00_Uint                        ulConnTimeout;    /* shm comm. only */
  PSECURITY_ATTRIBUTES              pWorldSA;         /* shm comm. only */
  PSECURITY_ATTRIBUTES              pLockSA;          /* shm comm. only */
  tsp00_Uint                        ulCommFlagNo;     /* shm comm. only */
/*==========*/
#endif
/*==========*/
  } teo003_ConnectParamRecord;

typedef struct ping_record
  {
    tsp00_Uint1                     RecordVersion;
    tsp00_Uint1                     Hops;
    tsp00_Uint2                     DataOffset;
    tsp00_Uint4                     DataLen;
  } teo003_PingRecord;

/* USED FOR CONTABILITY TO OLD STYLE SOURCES ONLY */
#define CONPKT_PARAM_REC          teo003_ConPktParamRecord
typedef teo003_ConPktParamRecord  *PCONPKT_PARAM_REC;
/* ============================================== */

typedef tsp00_Uint       
       (* SEND_PKT_FUNC_PTR_EO003  ) (void                        *Handle, 
                                      char                        *Buffer,
                                      tsp00_Int4                   SendLen,
                                      tsp00_ErrTextc VAR_ARRAY_REF ErrText);

typedef tsp00_Uint       
        (* RECV_PKT_FUNC_PTR_EO003  )(void                         *Handle,
                                      char                         *Data,
                                      tsp00_Uint                    ulMaxDataLen,
                                      tsp00_Uint                   *BytesReceived,
                                      tsp00_ErrTextc VAR_ARRAY_REF  ErrText);

/*
 * --- unions for mapping checks
 */
union map_check_i2_b2
  {
  tsp00_Int2                            int2;
  tsp00_C2                              c2;
  };

union map_check_i4_b4
  {
  tsp00_Int4                            int4;
  tsp00_C4                              c4;
  };

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/


/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/



#endif  /* GEO03_H */
