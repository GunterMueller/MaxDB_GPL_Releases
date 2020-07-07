/*!
  @file           gen003.h
  @author         JoergM
  @special area   communication ...
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



#ifndef GEN003_H
#define GEN003_H


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

/*
 * For historical reasons we need a compatible long type for lokal communication
 * with 32Bit/64Bit Databases. Under OSF1 this was always a long (8 Byte) for others
 * it was alwas a 4Byte type. To Mix 32Bit Clients with 64Bit Databases, the new
 * type ten_com_long is introduced, that replaces all long in struct comseg_header
 * and struct big_comseg_header.
 */
#if OSF1
typedef long ten_com_long;
#else
typedef tsp00_Int4 ten_com_long;
#endif

#if defined( NMP )
#ifndef _TIMEVAL
#define _TIMEVAL
struct timeval {
        time_t  tv_sec;         /* seconds */
#if defined(__LP64__)
        int     tv_usec;        /* and microseconds */
#else
        long    tv_usec;        /* and microseconds */
#endif /* __LP64__ */
};
#endif /* _TIMEVAL */
#endif /* NMP */

#if SOCKETS || CMS
#  if  ULT || NMP || SUN || SOLARIS
#      include         <sys/uio.h>
#      include         <sys/socket.h>
#      include         <netdb.h>
#  else
#      include         <sys/socket.h>
#      include         <netdb.h>
#      if  SVR4 || _IBMR2
#           include         <sys/select.h>
#      endif
# endif
#endif  /*SOCKETS || CMS*/

#ifdef  CMS
#include        <cms_socket.h>
#endif  /*CMS*/

#include "gsp00.h"
#include "gsp01.h"
#include "geo003.h"
#include "geo40.h"
#include "gen00.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define         NODESEPARATORLIST       "!#:"
#define         MAXCONNECTIONS          200
#define         RETRYCOUNT              5
#define         RETRYTIME               3
#define         LOCL_CON_TMO            120 /* may use socket via NETWORK */
#define         LOCL_PKT_TMO            90  /* may use socket via NETWORK */
#define         SOCK_CON_TMO            120
#define         SOCK_PKT_TMO            90
#define         SOCK_CON_QUE_SIZE       5
#define         INET_ADDR_SIZE          4

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*
 *  Macros for debugging of Internet addresses
 */
#ifdef  DEBUG_RTE
extern  void        sqldbgaddress ();
#define DBGADR(x)   {sqldbgaddress x;}
#else
#define DBGADR(x)   {}
#endif

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/



/*
 *  RTE HEADER for communication packets.
 */
struct rte_header
{
    tsp00_Int4  rh_act_send_len ;   /* this packet segment's length */
    tsp00_Uint1 rh_protocol_id ;    /* network protocol for transport */
    tsp00_Uint1 rh_mess_class ;     /* type of message in packet */
    tsp00_Uint1 rh_rte_flags ;      /* type modifiers */
    tsp00_Uint1 rh_residual_packets ;   /* number of trailing packet segments */
    tsp00_Int4  rh_sender_ref ;     /* this packet's sender's reference */
    tsp00_Int4  rh_receiver_ref ;   /* this packet's receiver's reference */
    tsp00_Int2  rh_rte_return_code ;    /* error number for last operation */
    tsp00_Uint1 rh_new_swap_type ;
    tsp00_Uint1 rh_filler1 ;      /* reserved for future use */
    tsp00_Int4  rh_max_send_len ;   /* total packet length (all segments) */
    /*
     *  Should be 8-byte aligned to avoid alignment computations.
     *
     *  rh_act_send_len includes the rte_header.
     *  rh_max_send_len includes one rte_header regardless of the number
     *                  of segments. This means, that the sum of all
     *                  segment's rh_act_send_len is greater than
     *                  rh_max_send_len unless the number of segments is 1.
     */
};
typedef struct rte_header       rte_header ;

/*
 *  rte_return_code   <==>  same as on sqlaconnect..sqlarelease
 */

/*
 *  REMOTE SQL connect message.
 */
struct rte_connect_packet
{
    tsp00_C2             cp_mess_code ;      /* character code; swapping */
    tsp00_Int2           cp_connect_length ; /* this packet's length (no rte_hdr) */
    tsp00_Uint1          cp_service_type ;   /* service to connect */
    tsp00_Uint1          cp_os_type ;        /* sender's operating system */
    tsp00_Uint1          cp_filler1 ;        /* alignment filler */
    tsp00_Uint1          cp_filler2 ;        /* alignment filler */
    tsp00_Int4           cp_max_segment_size ;   /* max supported packet segment size */
    tsp00_Int4           cp_max_data_size ;  /* max total data size incl min reply */
                                             /* sql_packet_size - sizes of headers */
    tsp00_Int4           cp_packet_size ;    /* x_param: total packet size */
    tsp00_Int4           cp_min_reply_size ; /* x_param: minimum size for reply */

#ifdef DO_NOT_USE_THIS_IT_IS_CORRECT_BUT_INKOMPATIBLE
    tsp00_DbName         cp_server_db ;
    tsp00_DbName         cp_client_db ;
#else
    tsp00_C8             cp_server_db;   /* distribution: requesting dbname */
    tsp00_C8             cp_client_db ;  /* dbname to connect to */
#endif
    tsp00_C256           cp_var_part ;   /* optional parameters */
};
typedef struct rte_connect_packet   rte_connect_packet ;
/* The following sizes include the RTE_HEADER ! */
#define RTE_CONPKT_SIZE     (RTE_HEADER_SIZE + sizeof(rte_connect_packet))
#define RTE_CONPKT_MINSIZ   (RTE_CONPKT_SIZE - sizeof(tsp00_C256))


/*
 *             Communication Segment
 *
 *      +----------------------------+
 *      | comseg_header              | ----+
 *      +----------------------------+     |
 *      | offset 1                   | <---+
 *      | offset 2                   | ----+
 *      +----------------------------+     |
 *      | rte_header + sql_packet 1  | <---+
 *      +----------------------------+     |
 *      | rte_header + sql_packet 2  | <---+
 *      +----------------------------+
 *
 *  Offsets are always in number of bytes from the start of a
 *  shared memory segment.
 *  cs_list_offset:
 *      small comseg: The number of bytes from start of comseg_header
 *                    to list of packet offsets.
 *      big comseg:   The number of bytes from start of big_comseg_header
 *                    to list of packet offsets.
 *  packet offset lists:
 *      small comseg: The offset for each packet from start of
 *                    comseg_header to rte_header.
 *      big_comseg:   The offset for each packet from start of
 *                    big_comseg_header to rte_header.
 *
 *  +-----------------------------------------------------+
 *  |         client_flag , server_flag                   |
 *  |-----------------------------------------------------|
 *  |                  -------                            |
 *  |                   0 , 2                             |
 *  |  srv=2;cli=0     -------                     cli=1  |
 *  |      sqlreceive /       \ sqlrequest,sqlrelease     |
 *  |               /           \                         |
 *  |           -------       -------                     |
 *  |            2 , 1         1 , 2                      |
 *  |           -------       -------                     |
 *  |               \           /                         |
 *  |          vreply \       / vreceive                  |
 *  |  srv=1           -------               cli=2;srv=0  |
 *  |                   2 , 0                             |
 *  |                  -------                            |
 *  +-----------------------------------------------------+
 *
 */

#define     COMSEG_VERSION      1

struct comseg_header
{
    ten_com_long   cs_size ;       /* small comseg: total shm size */
    int            cs_version ;        /* structure layout version number */

    int            cs_packet_cnt ;     /* packet count for this connection */
    ten_com_long   cs_packet_size ;    /* size of each packet */
    ten_com_long   cs_list_offset ;    /* offset from start of shm to list */
    int            cs_current_packet ; /* packet index for current req/reply */

    pid_t          cs_client_pid ;     /* client's pid */
    pid_t          cs_server_pid ;     /* server's pid */
    tsp00_Int4     cs_client_ref ;     /* client's reference number */
    tsp00_Int4     cs_server_ref ;     /* server's reference number */
    int            cs_client_state ;   /* client's communication status */
    int            cs_server_state ;   /* server's communication status */
    int            cs_client_flag ;    /* client's packet availability flag */
    int            cs_server_flag ;    /* server's packet availability flag */

    int            cs_shmid ;      /* this shm's shmid */
    int            cs_client_semid ;   /* client's semid */
    int            cs_server_semid ;   /* server's semid */

    /*
     *  for big comseg
     */
    COMSEG_LOCK cs_exclusive ;      /* lock here for write access */
};
typedef struct comseg_header        comseg_header ;

/*
 *  Structure of the BIG communication segment,
 *  which is shared between the application process and the UKP.
 *
 *      +-------------------------------+
 *      | big_comseg_header             | --------+
 *      +-------------------------------+         |
 *      +-------------------------------+ <-------+
 *      | comseg_header            Tn   | --------+
 *      +-------------------------------+         |
 *      | comseg_header            Tn+1 | ------+ |
 *      +-------------------------------+       | |
 *      | comseg_header            ...  |       | |
 *      +-------------------------------+       | |
 *      | comseg_header            Tn+m | ----+ | |
 *      +-------------------------------+     | | |
 *      +-------------------------------+     | | |
 *      | offset 1                      | <---|-|-+
 *      | offset 2                 Tn   | ----|-|-------+
 *      +-------------------------------+     | |       |
 *      | offset 1                      | <---|-+       |
 *      | offset 2                 Tn+1 | ----|-------+ |
 *      +-------------------------------+     |       | |
 *      | offset                   ...  |     |       | |
 *      | offset                   ...  |     |       | |
 *      +-------------------------------+     |       | |
 *      | offset 1                      | <---+       | |
 *      | offset 2                 Tn+m | ----------+ | |
 *      +-------------------------------+           | | |
 *      +-------------------------------+           | | |
 *      | rte_header + sql_packet  Tn   | <---------|-|-+
 *      +-------------------------------+           | |
 *      | rte_header + sql_packet  Tn+1 | <---------|-+
 *      +-------------------------------+           |
 *      | rte_header + sql_packet  ...  |           |
 *      +-------------------------------+           |
 *      | rte_header + sql_packet  Tn+m | <---------+
 *      +-------------------------------+
 *      | rte_header + sql_packet  ...  | (for clients needing two packets)
 *      +-------------------------------+
 *
 */

#define     BIG_COMSEG_VERSION  1

struct big_comseg_header
{
    ten_com_long    bc_size ;       /* total shared memory segment size */
    int             bc_version ;        /* structure layout version number */

    int             bc_comseg_cnt ;     /* Number of comseg_headers included */
    int             bc_packet_cnt ;     /* Number of packets included */
    ten_com_long    bc_comseg_size ;    /* size of each comseg_header */
    ten_com_long    bc_packet_size ;    /* size of each packet */
    ten_com_long    bc_comseg_offset ;  /* offset of first comseg_header */
    ten_com_long    bc_list_offset ;    /* offset of packet offset list */
    ten_com_long    bc_packet_offset ;  /* offset of first packet */

    /*
     *  Set by UKP before leaving dispatcher, cleared before sleeping.
     */
    int             bc_kp_running ;

    /*
     *  Set by every peer, after any request is ready.
     */
    int             bc_common_flag ;
    /*
     *  Needed for 64bit pointer padding...
     */
    int             padding ;
};
typedef struct big_comseg_header    big_comseg_header ;

/*
 *
 */

typedef int pipe_handle_t;

struct pipe_handles {
    pipe_handle_t writeHandle;
    pipe_handle_t readHandle;
};

struct pipe_connection_info {
    struct pipe_handles data;
    struct pipe_handles async;
};

/*
 *  Connection information.
 *  This is used by applications, server, network processes,
 *  tasks: utility, user.
 */

struct connection_info
{
    time_t                           ci_connect_time ;       /* time reserved for connect */
    int                              ci_state ;              /* connection state */
    int                              ci_use_count ;          /* incremented on each try */
    int                              ci_service ;            /* connected service */
                                                             /* KNL: UT/US or DISTRIB */
    int                              ci_protocol ;           /* connect method used */
                                                             /* KNL: SHM or BIGSHM */
    int                              ci_packet_cnt ;         /* number of packets used */
    long                             ci_packet_size ;        /* size of each packet */
    long                             ci_min_reply_size ;     /* min siz reserved for reply */
    long                             ci_max_data_size ;      /* max request+reply size */
    long                             ci_max_request_size;    /* max request size */

    pid_t                            ci_my_pid ;             /* own pid */
    pid_t                            ci_peer_pid ;           /* peer's pid (local) */
    tsp00_Int4                       ci_my_ref ;             /* own reference number */
    tsp00_Int4                       ci_peer_ref ;           /* peer's reference number */

    int                              ci_my_semid ;           /* own semid */
    int                              ci_peer_semid ;         /* peer's semid */
                                                             /* KNL: -2 for NETWORK */
    int                              ci_shmid ;              /* (big) comseg's shmid */

    int                              ci_peer_swap_type ;     /* remote: peer's swap type */
    long                             ci_max_segment_size;    /* remote: max segment size */
    int                              ci_sd ;                 /* remote: socket descriptor */
#ifdef SOCKETS
    struct sockaddr                  ci_peer_sock_addr ;     /* remote: peer's socket addr */
#endif
    tsp00_DbNamec                    ci_peer_dbname ;        /* remote/distrib: peer dbnam */
    tsp00_NodeIdc                    ci_peer_node ;          /* remote: peer's node name */
    tsp00_NodeIdc                    ci_remote_pid ;         /* remote: peer's process id */
    tsp00_NodeIdc                    ci_own_node ;           /* own node name */

    struct big_comseg_header         *ci_big_comseg ;        /* big comseg addr */
    long                             ci_big_offset ;         /* comseg_hdr offset */
    long                             ci_big_size ;           /* big comseg size */
    struct comseg_header             *ci_comseg ;            /* comseg_header addr */
    rte_header                       *ci_compack ;           /* comseg req addr */
    rte_header                       *ci_packet [ MAX_SQL_PACKETS ]; /* pkt addrs */
    rte_header                       *ci_request ;           /* curr request addr */
    rte_header                       *ci_reply ;             /* curr reply addr */
    void                             *ci_packetData [ MAX_SQL_PACKETS ];
    void                             *ci_requestData;
    void                             *ci_replyData;
    long                             ci_request_lgt ;        /* curr request lgt */
    long                             ci_reply_size ;         /* curr max reply siz */
    int                              ci_current_packet ;     /* curr packet index */
    char                             *ci_packet_buffer ;
    char                             *ci_cache_buf ;
    long                             ci_cache_lgt ;
    int                              ci_pipe_fd ;            /*KNL*/
    int                              ci_mess_class ;         /*KNL*/
    long                             ci_locbigoffset ;       /*KNL*/
    struct comseg_header             *ci_loccomseg ;         /*KNL*/
    struct connection_info           *ci_peer_connection;    /*KNL*/
    struct TASK_TYPE                 *ci_tcb ;               /*KNL*/
    int                              ci_readpipe_fd ;        /* PROT_LOCAL_CONTROL */
    int                              ci_writepipe_fd ;       /* PROT_LOCAL_CONTROL */
    char                             *ci_dbroot ;            /* used only during connect */
    char                             *ci_serverpgm;          /* used only during connect */
    struct connection_VMT            *ci_vmt ;               /* PIPE and FUTURE */
    struct teo40_NiConnectInfoRecord NI ;                    /* Communication via SAP NetworkInterface */
    void                             *pDBM;                  /* DBM Server communication  */
    SAPDB_Int4                        BytesReceived;         /* Number of bytes received since connection established */
    SAPDB_Int4                        BytesSend;             /* Number of bytes send since connection established */
    struct pipe_connection_info       ci_pipe_info;          /* local DBM/Repman server communication */
    SAPDB_UInt1                       ci_alter_server_sem;   /* KNL, client allows server semid changes */
    SAPDB_UInt4                       ci_dummy_comm_flag;    /* Used as dummy for old communication */
    char                              ci_authentication_allow[256];/* authentication list, e.g. SCRAMMD5, BASIC (old Auth.)  */
    SAPDB_UInt1                       encryption;
    SAPDB_UInt1                       connectType;           /* type of connection (OLTP, LVC) */
    RTE_OSPid                         kernelWatchDogPID;     /* PID of the kernel watchdog process */
    RTE_OSPid                         kernelPID;             /* PID of the kernel process */
    SAPDB_Bool                        kernelDied;            /* Kernel process has terminated */
};
typedef struct connection_info      connection_info ;

/*
 * connection_VMT
 *
 * extends connection_info into a C++-like class with
 * dynamic method dispatch
 * This allows for a more dynamic linking of protocols
 * (The dbm server requires specific routines and
 * other applications shouldn't have to include this code)
 */

typedef int connect_FT (struct connection_info* conninfo, tsp00_ErrTextc VAR_ARRAY_REF pErrText);
typedef int clear_FT (struct connection_info* conninfo, tsp00_ErrTextc VAR_ARRAY_REF pErrText);
typedef int release_FT (struct connection_info* conninfo, tsp00_ErrTextc VAR_ARRAY_REF pErrText);
typedef int request_FT (struct connection_info* conninfo, tsp00_ErrTextc VAR_ARRAY_REF pErrText);
typedef int replyavailable_FT (struct connection_info* conninfo, tsp00_ErrTextc VAR_ARRAY_REF pErrText);
typedef int receive_FT (struct connection_info* conninfo, tsp00_ErrTextc VAR_ARRAY_REF pErrText);
typedef int cancel_FT (struct connection_info* conninfo, tsp00_ErrTextc VAR_ARRAY_REF pErrText);
typedef int dump_FT (struct connection_info* conninfo, tsp00_ErrTextc VAR_ARRAY_REF pErrText);

typedef struct connection_VMT {
    char              *classname;
    connect_FT        *connect;
    clear_FT          *clear;
    release_FT        *release;
    request_FT        *request;
    replyavailable_FT *replyavailable;
    receive_FT        *receive;
    cancel_FT         *cancel;
    dump_FT           *dump;
} connection_VMT;

#define CEN_CONTROL_HELLO "Control started"

/*
 *  Structures of the BIG communication method.
 *  The REQUESTOR maintains the comseg_list, which holds information
 *  that is necessary for the client's connect handling.
 *  Each UKP has a big communication segment, holding the communication
 *  data for each of its tasks.
 *
 *           read   +----------------------------+
 *        +<------- | comseg_list                | <---- REQUESTOR
 *        |         +----------------------------+
 *      Client
 *        |         +----------------------------+
 *        +<------> | big_comseg                 | <---> UKP
 *      read/write  +----------------------------+
 *                                                       ...
 *                  +----------------------------+
 *            <---> | big_comseg                 | <---> UKP
 *                  +----------------------------+
 *
 *  The comseg_list has an entry for each task of the serverdb.
 *
 *      +----------------------------+
 *      | comseg_list_header         | ----+
 *      +----------------------------+     |
 *  | offset 1                   | <---+
 *  | offset 2                   |
 *  | offset ...                 | ----+
 *      +----------------------------+     |
 *      | comseg_list_entry      T1  | <---+
 *      +----------------------------+     |
 *      | comseg_list_entry      T2  | <---+
 *      +----------------------------+     |
 *      | comseg_list_entry      ... | <---+
 *      +----------------------------+
 *
 */


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/


/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/



#endif  /* GEN003_H */
