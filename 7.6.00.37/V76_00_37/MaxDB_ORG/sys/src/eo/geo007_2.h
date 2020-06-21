/*!
  @file           geo007_2.h
  @author         JoergM,RaymondR,StefnaP,RobinW
  @special area   RTE User visible Strings
  @brief          Kernel, Common and User RTE show messages for UNIX and NT. 
                              To ease the typographic problem fixing and translation
                              all such strings are contained in this module.
                              NOTE: Messages are seperated from this and contained in geo007_1.h
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



#ifndef GEO007_2_H
#define GEO007_2_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/* =============================== ERROR MESSAGES ============================== */

#define ERRMSG_CANT_INSTALL_SIG_HANDLER     "could not install signal handler"
#define ERRMSG_SET_EXIT_HANDLER_FAILED      "set exit handler failed [%lu]"
#define ERRMSG_MISSING_DBROOT               "missing "DBROOT_ENV_VAR" "
#define ERRMSG_CANT_FREE_MEM                "could not free memory"
#define ERRMSG_ALLOC_MEMORY                 "could not allocate memory"
/*
 --- remote comm. ---
*/

#define ERRMSG_COM_DATABASE_NOT_STARTED     "database not running"
#define ERRMSG_COM_TO_MANY_DB_SESSIONS      "too many database sessions active"
#define ERRMSG_COM_SERVER_OR_DB_NOT_FOUND   "database or server not found"

#define ERRMSG_COM_CANT_WRITE_TO_PIPE       "could not write to pipe"
#define ERRMSG_COM_PACK_WRITE_SIZE_ERROR    "pipe write size error"
#define ERRMSG_COM_CONN_PACKET_READ_ERR     "connect packet read pipe error"
#define ERRMSG_COM_CANT_SND_TO_SOCKET       "could not send to socket"
#define ERRMSG_COM_CONN_PACKET_REC_ERR      "connect packet receive error"
#define ERRMSG_COM_PACK_SEND_SIZE_ERROR     "socket send size error"
#define ERRMSG_COM_CONN_SERV_REJECT         "server rejected connection"
#define ERRMSG_COM_CONN_PACKET_GARBLED      "connect packet garbled"
#define ERRMSG_COM_ILL_PACKET_SIZE          "illegal packet size"
#define ERRMSG_COM_SOCKET_SND_ERROR         "socket send error"
#define ERRMSG_COM_SOCKET_REC_ERROR         "socket receive error"
#define ERRMSG_COM_CONN_BROKEN              "connection broken"
#define ERRMSG_COM_CANT_SET_OBJ_SEC         "could not set object security"
#define ERRMSG_COM_CANT_GET_OBJ_SEC         "could not get object security"
#define ERRMSG_COM_SOCKET_SELECT_ERROR      "socket select error"
#define ERRMSG_COM_SOCKET_SET_SNDBUF_ERROR  "socket set send buf error"
#define ERRMSG_COM_SOCKET_SET_RCVBUF_ERROR  "socket set recv buf error"
#define ERRMSG_COM_BAD_RTE_RETURN_CODE      "unknown RTE return code received"
#define ERRMSG_COM_NOT_OK                   "communication broken [not ok]"
#define ERRMSG_COM_TIMEOUT                  "command timeout"
#define ERRMSG_COM_CRASH                    "connection aborted"
#define ERRMSG_COM_PACKET_LIMIT             "packet limit reached"
#define ERRMSG_COM_RELEASED                 "session released"
#define ERRMSG_COM_UNKNOWN_REQUEST          "unknown request"
#define ERRMSG_COM_SHUTDOWN                 "database shutdown"


/*===========================================================================*
 *                            C O M M U N I C A T I O N                      *
 *===========================================================================*/

#define ERRMSG_SERVERDB_NOT_STARTED         "SERVERDB not started"
#define ERRMSG_NETW_NOT_RESTARTED           "network not restarted"
#define ERRMSG_COM_SERVER_OR_DB_NOT_ACC     "XSERVER or SERVERDB not accessible"
#define ERRMSG_COM_CANT_CREATE_COM_SEM      "could not create comm. semaphore"
#define ERRMSG_COM_CANT_ALLOC_COM_SEG       "could not create comm. shared memory"
#define ERRMSG_COM_CANT_GIVE_COM_SEG        "could not share memory with kernel"
#define ERRMSG_COM_CANT_OPEN_UKT_SEM        "could not open kernel semaphore"
#define ERRMSG_COM_CANT_WRITE_COM_QUE       "could not write to comm. queue"
#define ERRMSG_CONN_TIMEOUT                 "connect timeout"
#define ERRMSG_COM_TIMEOUT                  "command timeout"
#define ERRMSG_REPLY_TIMEOUT                "reply timeout"
#define ERRMSG_COM_WAIT_COM_SEM             "communication semaphore error"
#define ERRMSG_COM_POST_UKT_SEM             "kernel semaphore error"
#define ERRMSG_COM_CONN_REFUSED             "connection refused"
#define ERRMSG_COM_CONN_BROKEN              "connection broken"
#define ERRMSG_COM_UNBALANCED_REQUEST_REPLY "unbalanced request/reply"
#define ERRMSG_COM_CANT_FREE_MEM            "could not free memory"
#define ERRMSG_COM_TO_MANY_SESSIONS         "too many sessions"
#define ERRMSG_COM_UTIL_ALREADY_CONN        "utility session is already in use"
#define ERRMSG_NO_TCPIP_INSTALLED           "no TCP/IP to connect remote server"
#define ERRMSG_WRONG_TCPIP_VERSION          "wrong DLL-Version, (check: SQLTCPx.DLL)"
#define ERRMSG_SERV_BY_NAME_ERROR           "unknown TCP/IP service"
#define ERRMSG_MISSING_TCP_SERVICE_ENTRY    "missing TCP/IP service entry: "
#define ERRMSG_CANT_CREATE_SOCKET           "could not create socket"
#define ERRMSG_INCORRECT_MAXSIZE            "got incorrect maxsize"
#define ERRMSG_CANT_SET_BUF_SIZE            "could not set socket buffer"
#define ERRMSG_OUT_OF_SOCK_BUF_MEM          "Out of socket buffer memory"
#define ERRMSG_GET_HOST_BY_NAME_ERR         "unknown host name"
#define ERRMSG_REQ_SOCKET_CONNECT           "could not connect to socket"
#define ERRMSG_COM_ILL_REFERENCE            "illegal reference"
#define ERRMSG_COM_CORRUPTED_INTERNAL_DATA  "internal: corrupted connection data"
#define ERRMSG_COM_APPLIC_FORKED            "application forked"
#define ERRMSG_COM_WRONG_CONN_STATE         "wrong connection state"
#define ERRMSG_COM_WRONG_PACKET_LEN         "wrong packet length"
#define ERRMSG_COM_WRONG_PACKET_ADDR        "packet address differs from SQLREQUEST"
#define ERRMSG_KEEP_ALIVE                   "error setting 'KEEPALIVE' socket option"
#define ERRMSG_LINGER                       "error setting 'LINGER' socket option"
#define ERRMSG_XSERVER_NOT_ACTIVE           "XSERVER might be inactive"
#define ERRMSG_CONNECT_TIMED_OUT            "connect request timed out"
#define ERRMSG_CREATING_REC_THREAD          "error creating 'ASYNC RECEIVE' thread"
#define ERRMSG_KILL_REC_THREAD              "could not kill 'ASYNC RECEIVE' thread"
#define ERRMSG_CREATING_REC_SEM             "error creating 'ASYNC RECEIVE' semaphore"
#define ERRMSG_CLOSE_REC_SEM                "error closing 'ASYNC RECEIVE' semaphore"
#define ERRMSG_COM_POST_ASYNC_SEM           "async receive semaphore error"
#define ERRMSG_COM_CLOSE_QUE_MAIL           "could not close queue/mailslot"
#define ERRMSG_COM_CLOSE_SERV_SEM           "could not close server semaphore"
#define ERRMSG_COM_CLOSE_CLIENT_SEM         "could not close client semaphore"
#define ERRMSG_INVALID_PACKET_SIZE          "invalid packet size"
#define ERRMSG_INVALID_MIN_REPL_SIZE        "invalid minimum reply data size"
#define ERRMSG_INVALID_MAX_DATA_LEN         "invalid maximum data length"
#define ERRMSG_INVALID_REPL_PACKET_SIZE     "invalid reply packet size"
#define ERRMSG_GET_OFFICIAL_NODE_NAME       "could not get node name"
#define ERRMSG_GET_HOST_NAME                "could not get host name"
#define ERRMSG_COM_CLOSE_SERV_PROCESS       "could not close server process handle"

#define WRNMSG_COM_NODE_PL_DBNAME_TOO_LONG  "SERVERNODE + SERVERDB too long"
#define ERRMSG_COM_SERVER_NODE_NAME_TOO_LONG "server node name too long"
#define ERRMSG_COM_SERVERDB_NAME_TOO_LONG   "serverDB name too long"
#define ERRMSG_COM_SERVERDBROOT_NAME_TOO_LONG "serverDB root name too long"
#define ERRMSG_COM_SERVERPGM_NAME_TOO_LONG  "server program name too long"
#define ERRMSG_COM_NO_MORE_MEMORY_FOR_URI   "no more memory to build temporary connection URI"

#define ERRMSG_ILL_SWAP_TYPE                  "illegal swap type"
#define ERRMSG_COM_ILL_MAXSEGMENTSIZE         "illegal max. segment size"
#define ERRMSG_COM_PACKET_GARBLED             "received a garbled packet"
#define ERRMSG_COM_CONN_CLOSED_BY_COM_PART    "connection closed by comm. partner"

#define ERRMSG_COM_TO_MANY_OPEN_SESSIONS      "too many SERVERDB sessions active"
#define ERRMSG_PROTOCOL_ERROR                 "protocol error"
#define ERRMSG_UNSUPPORTED_PROTOCOL           "unsupported protocol"
#define ERRMSG_COM_RTE_COMM_ERROR             "communication error"
#define ERRMSG_COM_COMMUNICATION_TIMEOUT      "communication timeout"
#define ERRMSG_COM_COMMSEG_HEADER_CORRUPT     "communication segment header corrupt"
#define ERRMSG_COM_CERT_BUFFER_TOO_SMALL      "buffer too small to hold certificate"

#define ERRMSG_NI_LIBRARY_NOT_LOADED          "NI: Library not loaded"
#define ERRMSG_NI_FUNCTION_NOT_LOADED         "NI: Function not loaded"
#define ERRMSG_NICOM_NOT_SUPPORTED            "SAP NI not supported"
#define ERRMSG_NICOM_INIT_ERROR               "NI: init failed"
#define ERRMSG_NICOM_FINISH_ERROR             "NI: finish failed"
#define ERRMSG_NICOM_LISTEN_ERROR             "NI: listen failed"
#define ERRMSG_NICOM_ACCEPT_ERROR             "NI: accept failed"
#define ERRMSG_NICOM_CONNECT_ERROR            "NI: connect failed"
#define ERRMSG_NICOM_PEEK_ERROR               "NI: peek error"
#define ERRMSG_NICOM_SOCKET_SND_ERROR         "ni-socket send error"
#define ERRMSG_NICOM_REC_ERROR                "ni receive error"

#define ERRMSG_SSL_FUNCTION_NOT_LOADED        "SSL: Function not loaded"
#define ERRMSG_SSL_MEM_LIBRARY_PATH           "SSL: No more memory for library path"  
#define ERRMSG_SSL_MEM_SECURITY_PATH          "SSL: No more memory for security path"  
#define ERRMSG_SSL_MEM_CERTIFICATE_NAME       "SSL: No more memory for certificate name"  
#define ERRMSG_SSL_MEM_LICENCE_NAME           "SSL: No more memory for licence name"
#define ERRMSG_SSL_INIT_ERROR                 "SSL: Init"
#define ERRMSG_SSL_SESSION_INIT_ERROR         "SSL: Init session"  
#define ERRMSG_SSL_DUP_SSL_HDL                "SSL: Dup NI handle"  
#define ERRMSG_SSL_CREDENTIAL_ERROR           "SSL: Credential"
#define ERRMSG_SSL_SET_NIHDL_ERROR            "SSL: Set NI handle"
#define ERRMSG_SSL_MEM_HOSTNAME_NAME          "SSL: No more memory for target hostname"
#define ERRMSG_SSL_SET_HOSTNAME               "SSL: Set hostname"
#define ERRMSG_SSL_SESSION_START              "SSL: Start failed"
#define ERRMSG_SSL_SEND                       "SSL: Send failed"
#define ERRMSG_SSL_RECEIVE                    "SSL: Receive failed"
#define ERRMSG_SSL_MEM_CERT_FILE_NAME         "SSL: No more memory for certificate file"  
#define ERRMSG_SSL_NO_CERT                    "SSL: No certificate given"  
#define ERRMSG_SSL_OPEN_CERT_FILE             "SSL: Open certificate file failed"
#define ERRMSG_SSL_WRITE_CERT_FILE            "SSL: Write certificate file failed"
#define ERRMSG_SSL_MEM_OPEN_FLAG              "SSL: No more memory for open flag"
#define ERRMSG_SSL_GET_PEER_INFO              "SSL: Get peer info"
#define ERRMSG_SSL_ALLOC                      "SSL: Memory allocation failed"  
#define ERRMSG_SSL_LIBRARY_NOT_FOUND          "SSL: Could not locate SSL library"  
#define ERRMSG_SSL_SECURITY_PATH_NOT_FOUND    "SSL: Could not locate security path"  
#define ERRMSG_SSL_LICENCE_FILE_NOT_FOUND     "SSL: Could not locate licence file"




/*
 --- XUSER ---
*/
#define ERRMSG_CANNOT_CREATE_XUSER_FILE     "could not create USER file"
#define ERRMSG_CANNOT_OPEN_XUSER_FILE       "could not open USER file"
#define ERRMSG_CANNOT_CLOSE_XUSER_FILE      "could not close USER file"
#define ERRMSG_CANNOT_READ_XUSER_FILE       "could not read USER file"
#define ERRMSG_CANNOT_WRITE_XUSER_FILE      "could not write to USER file"
#define ERRMSG_CANNOT_SEEK_XUSER_FILE       "could not seek on USER file"
#define ERRMSG_READ_ERROR_XUSER_FILE        "error while reading USER file"
#define ERRMSG_READ_ERROR_XUSER_PAGE        "error while reading USER page"
#define ERRMSG_READ_ERROR_XUSER_EUSER       "error while reading effective USER"
#define ERRMSG_WRITE_ERROR_XUSER_FILE       "error while writing to USER file"
#define ERRMSG_BLANK_USERKEY                "blank user key not allowed"
#define ERRMSG_NO_XUSER_ENTRY_FOUND         "the USERKEY is unknown"
#define ERRMSG_NO_XUSER_FILE                "could not find USER file"
#define ERRMSG_DIFFERENT_SIDS               "incorrect USER data owner"
#define ERRMSG_ILLEGAL_USER_ENTRY           "illegal USER entry"
#define ERRMSG_ILLEGAL_USER_INDEX           "illegal USER index"
#define ERRMSG_DELETE_XUSER_FILE            "could not delete USER file"
#define ERRMSG_GET_LOGON_USER_NAME          "could not get logon USER name"
#define ERRMSG_NO_SPACE_LEFT                "space for USER entries exhausted"
#define ERRMSG_READ_XUSER_REG_DATA          "could not read USER data"
#define ERRMSG_WRITE_XUSER_REG_DATA         "could not write USER data"
#define ERRMSG_REMOVE_XUSER_REG_DATA        "could not remove USER data"
#define ERRMSG_WRONG_XUSER_REG_DATA_LEN     "wrong USER data length"
#define ERRMSG_NO_GUEST_ACCESS              "missing permission to create USER data"
#define ERRMSG_DATA_NEWER_THAN_COMPONENT    "USER data newer than component"
#define ERRMSG_XUSER_NO_PERMISSION          "no permission to change USER data"

/*
 --- Security ---
*/
#define ERRMSG_SEC_GET_USER_SID             "could not determine user SID"


/*
 --- Registry ---
*/
#define ERRMSG_REG_WRITE_VALUE              "write registry value failed"    
#define ERRMSG_REG_REMOVE_KEY               "remove registry key failed"    
#define ERRMSG_REG_REMOVE_VALUE             "remove registry value failed"
#define ERRMSG_REG_ENUM_OPEN                "open registry for enumeration failed"
#define ERRMSG_REG_SET_SECURITY             "set security for registry key failed"

/*
 --- SQLPRINT ---
*/
#define ERRMSG_WRITE_SPOOLFILE              "could not write on spool file"
#define ERRMSG_READ_SPOOLFILE               "could not read on spool file"

/*
 --- SQLEXEC/SQLAT ---
*/
#define ERRMSG_SYNC_SAME                    "could not start of the same session"
#define ERRMSG_CMD_LENGTH                   "command length too long"
#define ERRMSG_SYNC_START                   "could not start of a sync session"
#define ERRMSG_START_SESSION                "could not start a new session"
#define ERRMSG_END_SESSION                  "could not end session"
#define ERRMSG_SYNC_END                     "undefined end of a session"
#define ERRMSG_CANNOT_EXECUTE_CMD           "could not execute command"
#define ERRMSG_WRONG_TIME_VALUE             "wrong time value"
#define ERRMSG_NO_CMD                       "no command to execute"
#define ERRMSG_SET_STD_HANDLE               "could not set standard handle"
#define ERRMSG_CREATE_PIPE                  "could not create 'BATCH' pipe"
#define ERRMSG_CREATE_THREAD                "could not create 'BATCH' thread"
#define ERRMSG_RESUMING_THREAD              "could not resume 'BATCH' thread"
#define ERRMSG_SCHEDULE_SERVICE_NOT_STARTED "service '%s' not started"
#define ERRMSG_STARTING_FUNC_FROM_DLL       "could start function from DLL"
#define ERRMSG_FUNCTION_NOT_AVAIL           "function not available"

/*
 --- VIRTUAL TERMINAL ---
*/
#define ERRMSG_VT_WRONG_WINDOW_SIZE         "the window size is to large"
#define ERRMSG_VT_GET_VIDEO_MODE            "could not examine Video mode, rc = %hu"
#define ERRMSG_VT_OPEN_KEYBOARD             "could not examine Keyboard, rc = %lu"
#define ERRMSG_VT_CANT_CHANGE_CURSOR        "could not change cursor type, rc = %hu"

/*
 --- VIRTUAL FILE ---
*/
#define ERRMSG_VF_ILL_HOST_FILE_NAME        "illegal host file name"
#define ERRMSG_VF_ILL_READ_FORMAT_COMB      "illegal read/format combination"
#define ERRMSG_VF_BAD_FILENO                "invalid file number"
#define ERRMSG_VF_WRONG_FILE_NAME           "wrong file name"
#define ERRMSG_VF_ILL_DIREC                 "illegal direction"
#define ERRMSG_VF_ILL_FILE_DESC             "illegal file description"
#define ERRMSG_VF_NON_CONST_REC_LEN         "non-constant record length"
#define ERRMSG_VF_SEEK_REACHED_EOF          "seeking overflows the eof"
#define ERRMSG_VF_FTYPE_NOT_TO_SEEK         "wrong file type to seek"
#define ERRMSG_VF_NO_REG_FILE               "could not delete non-regular files"
#define ERRMSG_VF_DELETE                    "could not delete file"
#define ERRMSG_VF_INVALID_TAPE_CONTENTS     "invalid/no tape contents"

/* --- common */
#define ERRMSG_VF_ALLOC_BUFFER_SPACE        "could not allocate buffer space"
#define ERRMSG_VF_FILE_EXIST                "file already exists"
#define ERRMSG_VF_FILE_SHARING_VIOLATION    "this file/tape/pipe is already in use"
#define ERRMSG_VF_FREE_BUFFER_SPACE         "could not free buffer space"
#define ERRMSG_VF_FREE_BUFPOOL_COUNT_DIFF   "free Buffer pool: block count error"
#define ERRMSG_VF_GET_FILE_STATUS           "error by get file status"
#define ERRMSG_VF_GET_FILE_TYPE             "error by get file type"
#define ERRMSG_VF_ILL_APPEND_DEVICE         "illegal device for direction append"
#define ERRMSG_VF_ILL_DIREC                 "illegal direction"
#define ERRMSG_VF_ILL_FILE_DESC             "illegal file description"
#define ERRMSG_VF_ILL_FILE_SPEC             "illegal file specification"
#define ERRMSG_VF_ILL_DATA_SIZE             "illegal data size"
#define ERRMSG_VF_ILL_NEXT_REC_LEN          "length of next record is out of range"
#define ERRMSG_VF_ILL_REC_LEN               "illegal record length"
#define ERRMSG_VF_ILL_RESOURCE              "illegal resource"
#define ERRMSG_VF_MAX_REC                   "record too big   (> max record length)"
#define ERRMSG_VF_MIN_REC                   "record too small (< min record length)"
#define ERRMSG_VF_NO_FREE_FILE_DESCRIPTOR   "No free file descriptor found"
#define ERRMSG_VF_OPEN                      "could not open file/tape/pipe"
#define ERRMSG_VF_READ                      "could not read file/tape/pipe"
#define ERRMSG_VF_SEEK                      "could not seek on file/tape/pipe"
#define ERRMSG_VF_WRITE                     "could not write to file/tape/pipe"
#define ERRMSG_VF_TAPE_END_OF_MEDIA         "end-of-tape marker reached"
#define ERRMSG_VF_TAPE_WRITE_MARK           "Could not write filemark or setmark"
#define ERRMSG_VF_TAPE_FILE_SET_MARK        "file or setmark reached"
#define ERRMSG_VF_TAPE_NOT_PATITIONED       "tape not partitioned"
#define ERRMSG_VF_TAPE_NO_MEDIA_IN_DRIVE    "missing tape media in drive"
#define ERRMSG_VF_TAPE_MEDIA_CHANGED        "tape media changed"
#define ERRMSG_VF_REWIND_TAPE               "could not rewind tape"
#define ERRMSG_VF_DEVICE_NOT_READY          "device not ready"
#define ERRMSG_VF_TAPE_PARAM                "could not get/set tape parameter"
#define ERRMSG_VF_TAPE_LOCK_UNLOCK          "could not lock/unlock tape"
#define ERRMSG_VF_TAPE_LOAD_UNLOAD          "could not load/unload tape"
#define ERRMSG_VF_TAPE_WRONG_PART_CNT       "more then one partition on the tape"
#define ERRMSG_VF_CLOSE                     "could not close file"
#define ERRMSG_VF_DELETE                    "could not delete file"
#define ERRMSG_VF_ACCESS_DENIED             "access denied"
#define ERRMSG_VF_DISK_FULL                 "not enough space on disk"
#define ERRMSG_VF_WRONG_PATH                "wrong path"
#define ERRMSG_VF_WRONG_FILE_DEVICE_NAME    "wrong file or device name"
#define ERRMSG_VF_MEDIA_WRITE_PROTECTED     "the media is write protected"

#define ERRMSG_VF_CREATE_DIR                "could not create dir:"
#define ERRMSG_VF_REMOVE_DIR                "could not remove dir:"
#define ERRMSG_VF_CHANGE_DIR                "could not change dir:"
#define ERRMSG_VF_GETCWD                    "could not get curr. dir:"

/*
 --- SQLSETUSEROPTIONS ---
*/
#define ERRMSG_WRITE_USER_DIAG_OPT_VALS     "Could not write diagfile option values"
#define ERRMSG_READ_USER_DIAG_OPT_VALS      "Could not read diagfile option values"
#define ERRMSG_WRITE_CHAR_SET_OPT_VALS      "Could not write CHARACTERSET option values"
#define ERRMSG_READ_CHAR_SET_OPT_VALS       "Could not read CHARACTERSET option values"
#define ERRMSG_WRITE_SQLOPT_OPT_VALS        "Could not write SQLOPT option values"
#define ERRMSG_READ_SQLOPT_OPT_VALS         "Could not read SQLOPT option values"

/*
 --- SQLARG3 ---
*/
#define ERRMSG_NO_VALID_OPTION              "no valid option"
#define ERRMSG_NO_COMP_SPECIFIED            "no component specified"
#define ERRMSG_WRONG_OPTION_SEQUENCE        "wrong option sequence"
#define ERRMSG_MISSING_QUOTE                "missing quote"
#define ERRMSG_ILLEGAL_ARGUMENT             "illegal argument string"
#define ERRMSG_ILLEGAL_DIALOG_OBJ           "illegal component object"
#define ERRMSG_DOUBLE_SEPERATOR             "illegal seperator sequence"


/*
 --- XPARAM ---
*/
#define ERRMSG_PATH_TOO_LONG_FOR_XPF        "path too long for PARAM file"
#define ERRMSG_UNDEFINED_OPEN_MODE          "undefined open mode of PARAM file"
#define ERRMSG_ACCESS_XPARAM_FILE           "cannot access PARAM file"
#define ERRMSG_XP_FILE_READONLY             "put failed: PARAM file opened READONLY"
#define ERRMSG_BLANK_XP_KEY                 "blank keys not allowed"
#define ERRMSG_KEY_NOT_FOUND                "key not found"
#define ERRMSG_CANNOT_WRITE_XP_KEY          "cannot write key"
#define ERRMSG_CANNOT_WRITE_XP_VALUE        "cannot write value"
#define ERRMSG_CANNOT_WRITE_XP_FILE         "cannot write to PARAM file"
#define ERRMSG_WRITE_ERROR                  "write error"
#define ERRMSG_CANNOT_READ_XP_KEY           "cannot read key"
#define ERRMSG_CANNOT_READ_XP_VALUE         "cannot read value"
#define ERRMSG_READ_ERROR                   "read error"
#define ERRMSG_CANNOT_FIND_START_POINT      "cannot find start point"
#define ERRMSG_CANNOT_FIND_PREV_ENTRY       "cannot find previous entry"
#define ERRMSG_CANNOT_FIND_ACT_ENTRY        "cannot find actual entry"
#define ERRMSG_EOF_REACHED                  "end of file reached"
#define ERRMSG_PARAM_UPDATE_NOT_ALLOWED     "parameter update not allowed"
#define ERRMSG_BAD_FORMAT                   "bad format"

/*
 --- SERVICE ---
*/
#define SERVICE_USAGE                       "usage: service [[-g|-G] | [-d <..>][-r][-n <..>][-R <..>][-u][-q|-s|-t][-a]]\r\n"\
                                            "\n"                                                                \
                                            "     -d <serverdb>           - SERVERDB name       \n"             \
                                            "     -n <servernode>         - server node name    \n"             \
                                            "     -R <dbroot>             - DBROOT              \n"             \
                                            "     -G                      - install SDB operator group\n"       \
                                            "     -g                      - delete SDB operator group\n"        \
                                            "     -q                      - quick kernel        \n"             \
                                            "     -s                      - slow kernel         \n"             \
                                            "     -t                      - OMS test kernel     \n"             \
											"     -A <account,password>   - use this account    \n"             \
                                            "     -a                      - set autostart option\n"             \
											"     -P <parameter,value>    - alter parameter     \n"             \
                                            "     -r                      - removes service control entry\n"    \
                                            "     -u                      - update service control entry\n"

/*
 --- SHOW ---
*/
#if defined(_WIN32)
 #define SHOW_USAGE_95                      "usage: show [-n <servernode>]\r\n"            \
                                            "\n"                                           \
                                            "     -n <servernode> - server node name    \n"

 #define SHOW_USAGE                         "usage: show [-n <servernode>][-v|-r]\r\n"        \
                                            "\n"                                              \
                                            "     -n <servernode> - server node name    \n"   \
                                            "     -v              - show SERVERDB-Versions\n" \
                                            "     -r              - show DBROOTs\n"           \
											"     -A              - show service accounts\n"          
#else
 #define SHOW_USAGE                         "usage: show\r\n"
#endif

/*
 --- XSTART ---
*/
#if defined(_WIN32)
 #define XSTART_USAGE_NT                    "usage: strt [-d <..>] [-slow|-quick|-n <servernode>]\r\n"\
                                            "\n"                                            \
                                            "     -d <serverdb>    - SERVERDB name                 \n"\
                                            "     -n <servernode>  - server node name              \n"\
                                            "     -i               - no automatic restart          \n"\
                                            "     -g               - start gateway                 \n"\
                                            "     -l               - start with kernel low priority\n"\
                                            "     -slow            - start slow  kernel            \n"\
                                            "     -quick           - start quick kernel            \n"

 #define XSTART_USAGE                       "usage: strt [-d <..>] [-slow|-quick]                  \r\n"\
                                            "\n"                                                       \
                                            "     -d <serverdb>    - SERVERDB name                 \n"\
                                            "     -i               - no automatic restart          \n"\
                                            "     -l               - start with kernel low priority\n"\
                                            "     -slow            - start slow  kernel            \n"\
                                            "     -quick           - start quick kernel            \n"
#else
 #define XSTART_USAGE                       "usage: strt [-d <..>] [-slow|-quick] [-a]\r\n" \
                                            "\n"                                            \
                                            "     -d <serverdb>    - SERVERDB name       \n"\
                                            "     -i               - no automatic restart\n"\
                                            "     -slow            - start slow  kernel   \n"\
                                            "     -quick           - start quick kernel   \n"\
                                            "     -a               - close window automatically\n"
#endif

/*
 --- XSTOP ---
*/
#if defined(_WIN32)
 #define XSTOP_USAGE_NT                     "usage: stp -d <..> [-p|-n <servernode>]\r\n"\
                                            "\n"                                         \
                                            "     -d <serverdb>      - SERVERDB name    \n"\
                                            "     -n <servernode>    - server node name \n"\
                                            "     -p                 - dump             \n"
 #define XSTOP_USAGE                        "usage: stp -d <..> [-p]\r\n"\
                                            "\n"                                           \
                                            "     -d <serverdb>      - SERVERDB name    \n"\
                                            "     -p                 - dump             \n"
#else
 #define XSTOP_USAGE                        "usage: stp -d <..> [-p]\r\n"\
                                            "\n"                                           \
                                            "     -d <serverdb>      - SERVERDB name    \n"\
                                            "     -p                 - dump             \n"

#endif
#define XSTOP_WRONG_VERS_KERNEL             "       Kernel  RTE Version :  %s\n"
#define XSTOP_WRONG_VERS_XSTOP              "       XSTOP   RTE Version :  %s\n"

/*
 --- GETPARAM ---
*/
#define GETPARAM_USAGE                      "usage: getparam <ServerDB> <ParamName> \r\n"
#define GETPARAM_ERR_TEXT                   "getparam : %s\n"

/*
 --- PUTPARAM ---
*/
#define PUTPARAM_USAGE                      "usage: %s <ServerDB> <ParamName> <ParamValue>\r\n"
#define RENPARAM_USAGE                      "usage: %s <ServerDB> <OldParamName> <NewParamName>\r\n"
#define PUTPARAM_ERR_TEXT                   "%s : %-18s: %s\n"


/*
 --- DBPARAMS ---
*/
#define DBPARAMS_USAGE                      "usage: dbparams" \
                                            "[ DROP TABLE ]\r\n"
#define DBPARAMS_CMD_DROP_TABLE             "DROP"
#define DBPARAMS_PAR_DROP_TABLE             "TABLE"

/*
 --- DBTIMES ---
*/
#define DBTIMES_USAGE                       "usage: dbtimes" \
                                            "[ DROP TABLE ]\r\n"
#define DBTIMES_CMD_DROP_TABLE              "DROP"
#define DBTIMES_PAR_DROP_TABLE              "TABLE"

/*
 --- XSERVER ---
*/
#define XSERVER_NO_VALID_OPTION   "No valid option! Use -h to get help.\r\n"
#define XSERVER_ACTIVE            "SDB Remote Server is active (Port:%d)"


#if defined(_WIN32) && defined(RUN_AS_SERVICE)
 #define XSERVER_USAGE_NT                                                    \
   "usage: serv [-D<l>|-i [-F]|-u [-F]|-r|-s|-k][-n <servernode>]\n"         \
   "\n"                                                                      \
   "     -D <l>          -  debug level ( l = 0 .. 9 )                    \n"\
   "                        1       error messages, rejected connections  \n"\
   "                        2       connections                           \n"\
   "                        3       start/stop data transfer              \n"\
   "                        4       start/stop server threads             \n"\
   "                        8       internal control flow                 \n"\
   "     -i              -  install as service                            \n"\
   "     -u              -  update service (remove/install)               \n"\
   "     -F              -  fast mode (suppress DNS lookup)               \n"\
   "     -n <servernode> -  server node name                              \n"\
   "     -r              -  remove service from service table             \n"\
   "     -s              -  start                                         \n"\
   "     -k              -  kill (stop) service                           \n"

#else
 #define XSERVER_USAGE                                                      \
   "usage: serv [-D<l>] [-k]\n"                                             \
   "\n"                                                                     \
   "     -D <l>          - debug level ( l = 0 .. 9 )                    \n"\
   "                       0       error messages, rejected connections  \n"\
   "                       1       all connections                       \n"\
   "                       2       data transfer                         \n"\
   "                       7       data                                  \n"\
   "                       8       internal control flow                 \n"\
   "     -k              - kill a running xserver                        \n"
#endif


/*
 --- XSHOW ---
*/
#define XSHOW_ADABAS_GATEWAYS              "\nADABAS for R/3 - Gateways: \n"
#define XSHOW_ADABAS_SERVERDBS             "\nADABAS for R/3 - SERVERDBs: \n"
#define XSHOW_GATEWAYS                     "\nGateways: \n"
#define XSHOW_SERVERDBS                    "\nSERVERDBs: \n"
#define XSHOW_SERVERDB_GW_RUNNING          "  - %-18.18s  [Running]\n"
#define XSHOW_SERVERDB_GW_STOPPED          "  - %-18.18s  [Stopped]\n"
#define XSHOW_SERVERDB_GW_VER_RUNNING      "  - %-18.18s '%-40.40s'  [Running]\n"
#define XSHOW_SERVERDB_GW_VER_STOPPED      "  - %-18.18s '%-40.40s'  [Stopped]\n"
#define XSHOW_SERVERDB_GW_ACC_RUNNING      "  - %-18.18s '%-40.41s  [Running]\n"
#define XSHOW_SERVERDB_GW_ACC_STOPPED      "  - %-18.18s '%-40.41s  [Stopped]\n"
#define XSHOW_SERVERDB_GW_DBR_RUNNING      "  - %-18.18s '%-41.41s  [Running]\n"
#define XSHOW_SERVERDB_GW_DBR_STOPPED      "  - %-18.18s '%-41.41s  [Stopped]\n"
#define XSHOW_NO_SERVERDBS_GATEWAYS        "  [ N O N E ]\n"
#define XSHOW_XSERVER_SERVICES             "\nServices:\n"
#define XSHOW_XSERVER_IN_USER_SESSION      "\nIn current session:\n"
#define XSHOW_XSERVER_NOT_INSTALLED        "\nXSERVER is not installed.\n"
#define XSHOW_XSERVER_ACTIVE               "  XSERVER%-5.5s          [Running]\n"
#define XSHOW_XSERVER_NOT_ACTIVE           "  XSERVER%-5.5s          [Stopped]\n"
#define XSHOW_XSERVER_VER_ACTIVE           "  XSERVER%-5.5s  '%-40.40s'         [Running]\n"
#define XSHOW_XSERVER_VER_NOT_ACTIVE       "  XSERVER%-5.5s  '%-40.40s'         [Stopped]\n"
#define XSHOW_XSERVER_DBR_ACTIVE           "  XSERVER%-5.5s  '%-41.41s         [Running]\n"
#define XSHOW_XSERVER_DBR_NOT_ACTIVE       "  XSERVER%-5.5s  '%-41.41s         [Stopped]\n"



/*
 --- DIAGNOSE / MESSAGES OUTPUT ---
*/
#define LAST_CHANCE_INFO         "Last Chance Information"
#define ERR_LONG_DESCRIPTION     "ERROR"
#define IERR_LONG_DESCRIPTION    "INTERNAL ERROR"
#define INFO_LONG_DESCRIPTION    "INFO"
#define WRN_LONG_DESCRIPTION     "WARNING"

#define BIG_HEADER_INFO_LINE     "--------------------------------------------------------------------------------\r\n"\
                                 "Date       Time       TID(hex) Typ MsgID Label    Message-Text\r\n"\
                                 "--------------------------------------------------------------------------------\r\n"
#define TIME_HEADER_INFO_LINE    "--------------------------------------------------------------------------------\r\n"\
                                 "Date       Time     Message-Text\r\n"\
                                 "--------------------------------------------------------------------------------\r\n"

#define EVENT_FILE_INFO_LINE     "--------------------------------------------------------------------------------\r\n"\
                                 "Date     Time     _________# Id P ____Value1 ____Value2 Message-Text\r\n"\
                                 "--------------------------------------------------------------------------------\r\n"

#define ERR_FILE_OPEN_LINE       "%19.19s                               --- Starting ---\r\n"
#define ERR_FILE_STARTING_TEXT   "--- Starting GMT %s %s"
#define ERR_FILE_STARTING_LINE   "%19.19s                               "ERR_FILE_STARTING_TEXT"\r\n"
#define ERR_FILE_STOPPING_TEXT   "___ Stopping GMT %s %s"
#define ERR_FILE_STOPPING_LINE   "%19.19s                               "ERR_FILE_STOPPING_TEXT"\r\n"
#define BIG_HEADER_FORMAT_STR    "%19.19s %10.10s %3.3s %5d %-8.8s "
#define TIME_HEADER_FORMAT_STR   "%04d-%02d-%02d %02d:%02d:%02d"
#define SMALL_HEADER_FORMAT_STR  "%5d %s: "
#define E_LOG_HEADER_FORMAT_STR  ":\r\n - %s   TID: %s  PID: %s  MsgID: %d\r\n  "


#define BEGIN_OF_DIAG_WRITE_CYCLE  "\r\n============================================ begin of write cycle ==================\r\n\r\n\r\n"

#define CURR_DIAG_WRITE_POS            "-------------------------------------------- current write position ----------------"
#define END_OF_DIAG_WRITE_CYCLE    "\r\n===========================================  end of write cycle  ==================="

/* --- KERNEL VEN35KC 'errortab' messages ----------------------------------------------------------------------------------------- */

#define ERRMSG_DCOM_CREATE_DCOMI                "could not create DCOMi-thread"
#define ERRMSG_DCOM_RESUME_DCOMI                "could not resume DCOMi-thread"
#define ERRMSG_DCOM_COINITIALIZE_FAILED         "CoInitialize failed"
#define ERRMSG_DCOMI_CLOSE_EVENT_SEM            "could not close DCOMi event semaphore"
#define ERRMSG_DCOM_MAX_DCOMIS_REACHED          "max DCOMis reached"
#define ERRMSG_DCOM_GET_DCOMI_CTRL              "could not get DCOMi-Ctrl"
#define ERRMSG_DCOM_DEBUG_NOT_ENABLED           "DCOM-Debug not enabled"
#define ERRMSG_DCOMI_NO_RUNNING                 "DCOMi not running"
#define ERRMSG_DCOM_REM_UKT_DCOMi_NOT_ALLOWED   "remove of UKT related DCOMi-thread not allowed"

/* --- KERNEL VEN54KC 'errortab' messages ----------------------------------------------------------------------------------------- */

#define ERRMSG_DEVIO_DEVCREA                    "could not create volume"
#define ERRMSG_DEVIO_MAXDEVNO                   "maximum attached volumes reached"
#define ERRMSG_DEVIO_ALRATT                     "volume already attached"
#define ERRMSG_DEVIO_DEVICREA                   "could not create parallel IO thread"
#define ERRMSG_DEVIO_WRITE                      "could not write on volume"
#define ERRMSG_DEVIO_READ                       "could not read from volume"
#define ERRMSG_DEVIO_SEEK                       "could not seek on volume"
#define ERRMSG_DEVIO_INVPNO                     "invalid pageno, no io performed"
#define ERRMSG_DEVIO_LOCK                       "could not lock volume"
#define ERRMSG_DEVIO_DEV_POS                    "wrong volume position"
#define ERRMSG_DEVIO_SEEK_POS                   "unexpected volume position"
#define ERRMSG_DEVIO_NOT_ATT                    "volume not attached"
#define ERRMSG_DEVIO_DEVSPACE_ON_DIR            "no volumes on directory"
#define ERRMSG_DEVIO_DEVSPACE_ON_SYS            "no volumes on system file"
#define ERRMSG_DEVIO_DEVSPACE_ON_RD_ONLY        "no volumes on read-only file"
#define ERRMSG_DEVIO_DEVSPACE_COMPRESSED        "no volumes on compressed file"
#define ERRMSG_DEVIO_ERROR_ON_DEVSPACE          "error on volume"
#define ERRMSG_DEVIO_OPEN_DEVSPACE              "could not open volume"
#define ERRMSG_DEVIO_ACCESS_DENIED              "access on volume denied"
#define ERRMSG_DEVIO_LOCK_DEVSPACE              "could not lock volume"
#define ERRMSG_DEVIO_WRONG_PATH                 "wrong path specified"
#define ERRMSG_DEVIO_GET_FILE_INFO              "could not query file info"
#define ERRMSG_DEVIO_UNLOCK_DEVSPACE            "could not unlock volume"
#define ERRMSG_DEVIO_CREATE_DEVSPACE            "could not create volume"
#define ERRMSG_DEVIO_DURING_Q_FILE_SIZE         "query file-info for volume"
#define ERRMSG_DEVIO_CLOSE_DEVSPACE             "could not close the volume"
#define ERRMSG_DEVIO_SET_DEVSPACE_OWNER         "could not set volume owner"
#define ERRMSG_DEVIO_WRITE_ON_DEVSPACE          "could not write on volume"
#define ERRMSG_DEVIO_READ_FROM_DEVSPACE         "could not read from volume"
#define ERRMSG_DEVIO_SINGLEIO_DEVSP_UNDEF       "read/write on undefined volume"
#define ERRMSG_DEVIO_SEEK_ON_DEVSPACE           "could not seek on volume"
#define ERRMSG_DEVIO_DISK_FULL                  "not enough space on disk"
#define ERRMSG_DEVIO_INVALID_DEVNO              "invalid despace no."
#define ERRMSG_DEVIO_DIF_SIZE_OF_MIR            "different sizes of mirrored volumes"
#define ERRMSG_DEVIO_DEV0_NOT_ACTIVE            "main I/O thread not active"
#define ERRMSG_DEVIO_DEVSPACE_LOCKED            "volume locked by another process"
#define ERRMSG_DEVIO_INV_DEVSPACE_SIZE          "invalid volume size"
#define ERRMSG_DEVIO_CANT_CHECK_DRIVE_CONT      "error while checking drive contents"
#define ERRMSG_DEVIO_DEV_DRIVE_NOT_EMPTY        "drive must be empty to initialize"
#define ERRMSG_DEVIO_CANT_REDET_MEDIA           "could not redetermine media"
#define ERRMSG_DEVIO_DEV_POS                    "wrong volume position"
#define ERRMSG_DEVIO_SEEK_POS                   "unexpected volume position"
#define ERRMSG_DEVIO_WRONG_FILE_ATTRIB          "wrong file atrribute"
#define ERRMSG_DEVIO_DEVICREA_TIMEOUT           "parallel IO thread creation timeout"
#define ERRMSG_DEVIO_EVAL_DEVSPACE              "error evaluating volume"
#define ERRMSG_DEVIO_CREATE_COMPL_PORT          "error creating io completion port"
#define ERRMSG_DEVIO_USER_REQ_ERROR             "user requested error"
#define ERRMSG_DEVIO_ALLOC_MEM                  "could not allocate memory"


/* --- KERNEL VEN59KC error messages ---------------------------------------------------------------------------------------------- */

#define ERRMSG_EXEC_COMMAND                     "could not execute command"
#define ERRMSG_CMD_FILE_NOT_FOUND               "CMD.EXE not found"

/* --- KERNEL VEN65KC 'errortab' messages ----------------------------------------------------------------------------------------- */

#define ERRMSG_ASYNCIO_SURPRISING_ERROR_CODE    "surprising error code"
#define ERRMSG_ASYNCIO_ALRATT                   "device already attached"
#define ERRMSG_ASYNCIO_MAXDEVICES               "maximum attached devices reached"
#define ERRMSG_ASYNCIO_ASYNCI_CREATE            "could not create async IO thread"
#define ERRMSG_ASYNCIO_ASYNC0_NOT_ACTIVE        "main async I/O thread not active"
#define ERRMSG_ASYNCIO_ACCESS_DENIED            "access denied"
#define ERRMSG_ASYNCIO_WRONG_PATH               "wrong path specified"
#define ERRMSG_ASYNCIO_GET_FILE_INFO            "could not query file info"
#define ERRMSG_ASYNCIO_IO_ON_DIR                "file I/O on directoriy not allowed"
#define ERRMSG_ASYNCIO_IO_ON_SYS                "file I/O on system file not allowed"
#define ERRMSG_ASYNCIO_OPEN_FILE_DEV_PIPE       "could not open file/device/pipe"
#define ERRMSG_ASYNCIO_WRONG_DEVICE_TYPE        "wrong device type"
#define ERRMSG_ASYNCIO_CLOSE_FILE_DEV_PIPE      "could not close file/device/pipe"
#define ERRMSG_ASYNCIO_CLOSE_EVENT_SEM          "could not close semaphore"
#define ERRMSG_ASYNCIO_ASYNCI_REMOVE            "could not remove async IO thread"
#define ERRMSG_ASYNCIO_FILE_DEV_PIPE_NOT_OPEN   "file/device/pipe not open"
#define ERRMSG_ASYNCIO_WRONG_DEVICENO           "wrong deviceno"
#define ERRMSG_ASYNCIO_WAIT_FIRST               "call wait first"
#define ERRMSG_ASYNCIO_NO_REQ_ACTIVE            "no I/O requests active"
#define ERRMSG_ASYNCIO_CANT_SEEK_FILE_DEV_PIPE  "could not seek on file/device/pipe"
#define ERRMSG_ASYNCIO_CANT_READ_FILE_DEV_PIPE  "could not read from file/device/pipe"
#define ERRMSG_ASYNCIO_CANT_WRITE_FILE_DEV_PIPE "could not write to file/device/pipe"
#define ERRMSG_ASYNCIO_END_OF_FILE_DEV_PIPE     "end of file"
#define ERRMSG_ASYNCIO_ALLOC_MEM                "could not allocate mameory"
#define ERRMSG_ASYNCIO_FREE_MEM                 "could not free memory"
#define ERRMSG_ASYNCIO_WRONG_BLOCK_SIZE         "wrong block size specified"
#define ERRMSG_ASYNCIO_WRONG_FILE_ATTRIB        "wrong file atrribute"
#define ERRMSG_ASYNCIO_MEDIA_WRITE_PROTECTED    "the media is write protected"
#define ERRMSG_ASYNCIO_DEVICE_NOT_READY         "device not ready"
#define ERRMSG_ASYNCIO_DEVICE_BUSY              "device busy"
#define ERRMSG_ASYNCIO_END_OF_MEDIA             "end of media reached"
#define ERRMSG_ASYNCIO_FILE_SET_MARK            "filemark or setmark was reached"
#define ERRMSG_ASYNCIO_MEDIA_NOT_PATITIONED     "no partition information found"
#define ERRMSG_ASYNCIO_NO_MEDIA_IN_DEVICE       "no media in device"
#define ERRMSG_ASYNCIO_MEDIA_CHANGED            "media changed"
#define ERRMSG_ASYNCIO_BAD_LENGTH_READ_WRITTEN  "bad length of data written/read"
#define ERRMSG_ASYNCIO_GET_TAPE_DRIVE_INFO      "could not get drive info"
#define ERRMSG_ASYNCIO_GET_TAPE_MEDIA_INFO      "could not get media info"
#define ERRMSG_ASYNCIO_SET_TAPE_DRIVE_INFO      "could not set drive info"
#define ERRMSG_ASYNCIO_SET_TAPE_MEDIA_INFO      "could not set media info"
#define ERRMSG_ASYNCIO_REWIND_TAPE              "could not rewind tape"
#define ERRMSG_ASYNCIO_LOCK_UNLOCK_TAPE         "could not lock/unlock tape"
#define ERRMSG_ASYNCIO_WRITE_TAPE_MARK          "could not write tape mark"
#define ERRMSG_ASYNCIO_CRC_ERROR                "CRC error, data/media might be damaged"
#define ERRMSG_ASYNCIO_SHARING_VIOLATION        "file is already used by another process"
#define ERRMSG_ASYNCIO_WRONG_FILE_DEVICE_NAME   "invalid file or device name"
#define ERRMSG_ASYNCIO_UNRECOGNIZED_MEDIA       "unrecognized media"
#define ERRMSG_ASYNCIO_TENSION_TAPE             "could not tension tape"
#define ERRMSG_ASYNCIO_DISK_FULL                "not enough space on the disk."
#define ERRMSG_ASYNCIO_LOAD_UNLOAD_TAPE         "could not load/unload tape"
#define ERRMSG_ASYNCIO_ILL_BLOCK_COUNT          "blockcount mismatch"
#define ERRMSG_ASYNCIO_IO_READ_ONLY             "cannot write on read-only file"
#define ERRMSG_ASYNCIO_PIPE_CONN_CANCELLED      "pipe operation cancelled"
#define ERRMSG_ASYNCIO_NO_DATA_DETECTED         "no more data on media"
#define ERRMSG_ASYNCIO_OPERATION_CANCELED       "operation was canceled by user"  // --- AIDS 1008631
#define ERRMSG_ASYNCIO_INVALID_BLOCKSIZE        "invalid tape blocksize"

/* --- Virtual file - general error messages -------------------------------------------------------------------------------------- */

#define ERRMSG_VF_ILL_HOST_FILE_NAME            "illegal host file name"
#define ERRMSG_VF_ILL_READ_FORMAT_COMB          "illegal read/format combination"
#define ERRMSG_VF_WRONG_FILE_NAME               "wrong file name"
#define ERRMSG_VF_ILL_DIREC                     "illegal direction"
#define ERRMSG_VF_BAD_FILENO                    "invalid file number"
#define ERRMSG_VF_ILL_FILE_DESC                 "illegal file description"
#define ERRMSG_VF_NON_CONST_REC_LEN             "non-constant record length"

/* --- Win95/NT Console ----------------------------------------------------------------------------------------------------------- */

#define KERNEL_HINT_SERVERDB_STARTING           "SERVERDB '%s' is starting"
#define KERNEL_HINT_STARTING                    "SERVERDB is starting"
#define KERNEL_HINT_SERVERDB_COLD               "SERVERDB '%s' state is ADMIN - %d user(s) connected"
#define KERNEL_HINT_COLD                        "SERVERDB is in ADMIN state - %d user(s) connected"
#define KERNEL_HINT_SERVERDB_WARM               "SERVERDB '%s' is ONLINE - %d user(s) connected"
#define KERNEL_HINT_WARM                        "SERVERDB is ONLINE - %d user(s) connected"
#define KERNEL_HINT_SERVERDB                    "SERVERDB '%s' - %d user(s) connected"

/* --- OS/2 Console --------------------------------------------------------------------------------------------------------------- */

#define KERNEL_WARNING                          "Warning"
#define KERNEL_INFORMATION                      "Information"
#define KERNEL_QUESTION                         "Question"
#define KERNEL_CONSOLE                          " Console "
#define KERNEL_DIAG                             " Diagnostic file "
#define KERNEL_FONT_TITLE                       "Font"
#define KERNEL_DB_STATE_WARM                    "ONLINE"
#define KERNEL_DB_STATE_COLD                    "ADMIN"
#define KERNEL_DB_STATE_STOPPED                 "STOPPED"
#define KERNEL_XSERVER_NOT_ACTIVE               "Remote SQL Server"
#define KERNEL_XSERVER_ACTIVE                   "Remote SQL Server"
#define KERNEL_STRING_NOT_FOUND                 "String not found!"
#define KERNEL_DELETE_CONSOLE                   "Do you want to delete the console contents ?\n\n"
#define KERNEL_CANCEL_EXIT_WARM_CONSOLE         "To stop the SERVERDB it must be in ADMIN mode.\n\n"
#define KERNEL_EXIT_WARM_CONSOLE                "The SERVERDB is ONLINE. Do you really want to stop the SERVERDB?\n\n"
#define KERNEL_EXIT_CONSOLE                     "Do you want to stop the SERVERDB?\n\n"
#define KERNEL_ISO_FONT_FAILED                  "The font selected is not compliant with the"     \
                                                " ISO 9241 standard on the IBM 9515, 9517, or"    \
                                                " 9518 displays.\n\n"
#define KERNEL_ISO_FONT_NOT_TESTED              "The font selected is not tested for compliance"  \
                                                " with the ISO 9241 standard.\n\n"


/* --- Kernel Communication ------------------------------------------------------------------------------------------------------ */

#define ERRMSG_NETW_NOT_RESTARTED               "network not restarted"
#define ERRMSG_NOT_YET_IMPLEMENTED              "not yet implemented"
#define ERRMSG_NETW_SHUTDOWN                    "network shutdown"
#define ERRMSG_REQUEST_TIMEOUT                  "request timeout"
#define ERRMSG_RECEIVE_TIMEOUT                  "receive timeout"
#define ERRMSG_SERVERDB_SHUTDOWN                "SERVERDB shutdown"
#define ERRMSG_ILLEGAL_PATHID                   "illegal pathid"
#define ERRMSG_PATH_NOT_CONNECTED               "path not connected"
#define ERRMSG_PEER_SEM_ERROR                   "peer semaphore error"
#define ERRMSG_TOO_MANY_SESSIONS                "too many sessions"
#define ERRMSG_WRONG_PACKET_LEN                 "wrong packet length"
#define ERRMSG_WRONG_PACKET_REQ                 "wrong packet requested"
#define ERRMSG_PROTOCOL_ERROR                   "protocol error"
#define ERRMSG_CONNECTION_BROKEN                "connection broken"
#define ERRMSG_CONNECTION_CLOSED_BY_C_PARTNER   "connection closed by comm. partner"

/* --- Multithreading API ------------------------- */

#define ERRMSG_STACKSIZE                        "stack size is to small."
#define ERRMSG_RESOURCE_LIMIT                   "resource limit."
#define ERRMSG_PROCADDRESS_NULL                 "address of thread proc is NULL."
#define ERRMSG_UNKNOWN_THREADID                 "thread id cannot be found."
#define ERRMSG_INVALID_SIGNAL                   "invalid signal number."
#define ERRMSG_DEADLOCK                         "failed to prevent a deadlock."
#define ERRMSG_INVALID_TLSKEY                   "invalid local storage key."
#define ERRMSG_TLSKEY_BUSY                      "local storage key is busy."




/* --- Microsoft Cluster Server ------------------- */

#define ERRMSG_CS_OPEN_REG                      _T("[SAP DBTech] Unable to open Parameters key. Error: %1!u!.")
#define ERRMSG_CS_ALLOC_RESOURCE                _T("[SAP DBTech] Unable to allocate resource entry structure. Error: %1!u!.")
#define ERRMSG_CS_NONEXIST_REQUEST              _T("[SAP DBTech] Request for a nonexistent resource id %1!u!.")
#define ERRMSG_CS_SANITY_CHECK_FAILED           _T("[SAP DBTech] Resource sanity check failed! resource id: %1!u!.")
#define ERRMSG_CS_START_WORKER_THRD             _T("[SAP DBTech] Online: Unable to start thread, Status %1!u!.")
#define ERRMSG_CS_ERR_READ_PROPERTY             _T("[SAP DBTech] Unable to read the '%1' property. Error: %2!u!.")
#define ERRMSG_CS_STARTING_RESOURCE             _T("[SAP DBTech] Error %1!u! bringing resource '%2' online.")
#define ERRMSG_CS_STOPPING_RESOURCE             _T("[SAP DBTech] Error %1!u! bringing resource '%2' offline.")


#define INFOMSG_CS_CLOSE_REQUEST                _T("[SAP DBTech] Close request.")
#define INFOMSG_CS_ONLINE_REQUEST               _T("[SAP DBTech] Online request.")
#define INFOMSG_CS_OFFLINE_REQUEST              _T("[SAP DBTech] Offline request.")
#define INFOMSG_CS_TERMINATE_REQUEST            _T("[SAP DBTech] Terminate request.")
#define INFOMSG_CS_RES_CTRL_REQUEST             _T("[SAP DBTech] Resource control request.")
#define INFOMSG_CS_IO_PENDING                   _T("[SAP DBTech] IO pending.")


#define OPTION_HELP_STR                         _T("help")
#define OPTION_ERROR_MESSAGES_TO_DBOX_STR       _T("send error messages to dialog box")
#define OPTION_VERBOSE_ERROR_MESSAGES_STR       _T("print verbose error messages")
#define OPTION_MOVE_DRIVE_TO_GROUP_STR          _T("move drive to SDB group")
#define OPTION_ONLINE_GROUP_STR                 _T("bring SDB group online")
#define OPTION_OFFLINE_GROUP_STR                _T("bring SDB group offline")
#define OPTION_BUILDUP_GROUP_STR                _T("build up SDB group")
#define OPTION_REMOVE_SAPDB_RESOURCES_STR       _T("remove all SDB resources")
#define OPTION_CREATE_RESOURCE_TYPES_STR        _T("create SDB resource types")
#define OPTION_REMOVE_RESOURCE_TYPES_STR        _T("remove SDB resource types")
#define OPTION_ONLINE_XSERVER_RESOURCE_STR      _T("bring XSERVER online")
#define OPTION_OFFLINE_XSERVER_RESOURCE_STR     _T("bring XSERVER offline")
#define OPTION_ONLINE_SERVERDB_RESOURCE_STR     _T("bring SERVERDB online")
#define OPTION_OFFLINE_SERVERDB_RESOURCE_STR    _T("bring SERVERDB offline")
#define OPTION_ADD_SERVERDB_RESOURCE_STR        _T("add SERVERDB resource")
#define OPTION_REMOVE_SERVERD_BRESOURCE_STR     _T("remove SERVERDB resource")
#define OPTION_ENABLE_CLUSTERLOG_STR            _T("enable cluster log")
#define OPTION_DISABLE_CLUSTERLOG_STR           _T("disable cluster log")
#define OPTION_REGISTER_DEBUGEX_STR             _T("register debugex")
#define OPTION_UNREGISTER_DEBUGEX_STR           _T("unregister debugex")


/* --- System Information ------------------- */
#define ERRMSG_SYS_CREATE_COMMON_APPL_DATA_PATH "create common data path failed"
#define ERRMSG_SYS_GET_COMMON_APPL_DATA_PATH    "determine common data path failed"
#define ERRMSG_SYS_COMMON_APPL_DATA_PATH_LENGTH "common data path to long"
#define ERRMSG_SYS_GET_APPL_DATA_PATH           "determine user data path failed"
#define ERRMSG_SYS_APPL_DATA_PATH_LENGTH        "user data path to long"
#define ERRMSG_SYS_GET_MY_DOCUMENTS_PATH        "determine user work path failed"
#define ERRMSG_SYS_MY_DOCUMENTS_PATH_LENGTH     "user work path to long"
#define ERRMSG_SYS_HOME_SUBDIRECTORY_UNKNOWN    "unknown home subdirectory"
#define ERRMSG_SYS_CREATE_MY_DOCUMENT_PATH      "create my document path"    
#define ERRMSG_SYS_CREATE_USER_APPL_DATA_PATH   "create user application path"    
#define ERRMSG_SYS_CREATE_USER_LOCAL_APPL_DATA_PATH "create user local appl path"
#endif  /* GEO007_2_H */
