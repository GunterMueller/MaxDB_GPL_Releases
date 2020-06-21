/*!
  @file           gos00.h
  @author         RaymondR
  @special area   
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


#ifndef GOS00_H
#define GOS00_H

#define NEEDED_WIN32_WINNT 0x0500

#if !defined(_WIN32_WINNT)
# define _WIN32_WINNT  NEEDED_WIN32_WINNT
#else
# if _WIN32_WINNT < NEEDED_WIN32_WINNT
#  undef _WIN32_WINNT
#  define _WIN32_WINNT  NEEDED_WIN32_WINNT
# endif
#endif

#include  "gsp00.h"
#include  "gsp100.h"
#include  "SAPDBCommon/SAPDB_string.h"
#include  "RunTime/RTE_Types.h"


#include <windows.h>
#include  <time.h>
#include  <ctype.h>
#include  <memory.h>
#include  <errno.h>
#include  <string.h>
#include  <stdlib.h>
#include  <signal.h>
#include  <stddef.h>
#include  <limits.h>
#include  <math.h>
#include  <stdio.h>
#include  <process.h>


#include  "geo60.h"

// ========================== RTE internal DEFINES ============================


#if defined(_WIN32) && defined(_ALPHA_)
  #define MEM_SYNC(_addr)                                                     \
            _addr = sql76_mem_sync ( _addr )
#else
  #define MEM_SYNC(_addr)                                                     \
            _addr
#endif

#if defined(_WIN32) && ( defined(_ALPHA_) || defined(_MIPS_) || defined(_PPC_) || defined (_WIN64) )
 #if defined ( NO_DIRTY_READ )
  #define INTERLOCK(addr)                                                     \
            InterlockedExchange(addr,1)
 #else
  #define INTERLOCK(addr)                                                     \
            ((LONG)*addr ? 1 : InterlockedExchange(addr,1))
 #endif
 #define CLEARLOCK(addr)                                                      \
           InterlockedExchange(addr,0)
 #define WAIT_UNTIL_ALONE(addr)                                               \
           while( INTERLOCK(addr) ) SLEEP( 0 )
#else
 #define INTERLOCK(addr)                                                      \
           sql76_interlock(addr)
 #define CLEARLOCK(addr)                                                      \
           sql76_clearlock(addr)
 #define WAIT_UNTIL_ALONE(addr)                                               \
           while( INTERLOCK(addr) ) SLEEP( 0 )
#endif


#define KERNEL_TYPE_FAST                 1
#define KERNEL_TYPE_SLOW                 2
#define KERNEL_TYPE_QUICK                3
#define KERNEL_TYPE_TEST                 4

//
// --- EVENT CATEGORIES of sqlmsg.dll
//
#define SQLMSG_CATEGORY_NONE              0
#define SQLMSG_CATEGORY_XSERVER           1
#define SQLMSG_CATEGORY_CLIENT            2
#define SQLMSG_CATEGORY_LAST              SQLMSG_CATEGORY_CLIENT

//
// --- EVENT CATEGORIES of the kernel.exe,quickknl.exe ...
//
#define NTKERN_CATEGORY_NONE              0
#define NTKERN_CATEGORY_FAST_SERVERDB     1
#define NTKERN_CATEGORY_QUICK_SERVERDB    2
#define NTKERN_CATEGORY_SLOW_SERVERDB     3
#define NTKERN_CATEGORY_FAST_GATEWAY      4
#define NTKERN_CATEGORY_SLOW_GATEWAY      5
#define NTKERN_CATEGORY_LAST              NTKERN_CATEGORY_SLOW_GATEWAY

#define  _System
#define  _Optlink
#if defined(__cplusplus)
  #define  _INLINE                        inline
#else
  #define  _INLINE                        __inline
#endif

#if !defined(INVALID_HANDLE_VALUE)
 #define INVALID_HANDLE_VALUE            (HANDLE)-1
#endif


#define KB                               * 1024              // 1 Kilobyte
#define MB                               * ( 1024 * 1024 )   // 1 Megabyte
#define GB                               * ( 1024 MB )       // 1 Gigabyte
#define MAX_ALLOC_POOL_SIZE              ( 20 MB )
#define ALIGNMENT_VALUE                  16
#define SQL_PACKET_ALIGNMENT             8
#define DEFAULT_TERMINATION_TIMEOUT      (60 * 20)   // -- Traceriter termination timeout value
#define MIN_TERMINATION_TIMEOUT          (60 * 5)


#define KBYTE_4                          4096
#define KBYTE_64                         65536

#define NOEABUF                          0L          // - used by 'DosOpen'
#define NUL                              '\0'
#define UNDEF                            -1
#define HT_SIZE                          8
#define HT                               '\t'
#define CR                               '\r'
#define LF                               '\n'

#define FILENAMESIZE                     64
#define MAXSTRINGLENGTH                  64
#define MAXEXIT                          10

#define STDIN                            0
#define STDOUT                           1
#define STDERR                           2

#define POST_SEM                         0
#define DO_NOT_POST_SEM                  1

//
// - device types  - 'sql44c_get_dev_type_by_filename'
//
#define DT_UNKNOWN                       0
#define DT_CON                           1
#define DT_CONOUT                        1   // - equal to DT_CON
#define DT_CONIN                         2
#define DT_PRN                           3
#define DT_LPTx                          4
#define DT_COMx                          5
#define DT_OTHER                         6
#define DT_TAPE                          7
#define DT_DRIVE                         8
#define DT_PIPE                          9
#define DT_NUL                           10


//
// --- diag/console options
//
#define MAX_MSG_LINE_LEN                 256

#define ENABLE_CONSOLE_OUTPUT            0x01
#define ENABLE_EVENT_LOG_OUTPUT          0x02
#define ENABLE_DIAGFILE_OUTPUT           0x04

//
// --- this types might be defined by geo200.h
//
#if !defined(IERR_TYPE) 
# define  IERR_TYPE                      0
#endif

#if !defined(ERR_TYPE) 
# define  ERR_TYPE                       1
#endif

#if !defined(WRN_TYPE) 
# define  WRN_TYPE                       2
#endif

#if !defined(INFO_TYPE) 
# define  INFO_TYPE                      3
#endif

//
// --- SERVERDB start/stop options
//
#define AUTOCLOSE                        0x0001
#define IGNORE_AUTORESTART               0x0002
#define FORCE_AUTORESTART                0x0004
#define LOW_PROCESS_PRIORITY             0x0008
#define MS_CLUSTER_SERVER_STARTUP        0x8000

//
// --- SERVERDB service options
//
#define CREATE_SAPDB_SERVICE             1UL
#define REMOVE_SAPDB_SERVICE             2UL
#define UPDATE_SAPDB_SERVICE             3UL


//
// --- virtual file options
//
#define HF_MIN                           1
#define MAGIC_REC_LEN                    8

#ifdef USER
    #define HF_MAX                       8
#else
    #define HF_MAX                       3
#endif

//
// --- File types
//
#define VFT_DISK                         0
#define VFT_CHARACTER_DEVICE             1
#define VFT_PIPE                         2

//
// --- File states
//
#define HFS_UNUSED                       1
#define HFS_RESERVED                     2
#define HFS_USED                         3
#define HFS_END                          4
#define HFS_ERROR                        5
#define HFS_EOF                          6

//
// --- File attributes
//
#define FILE_READONLY                    FILE_ATTRIBUTE_READONLY           
#define FILE_HIDDEN                      FILE_ATTRIBUTE_HIDDEN             
#define FILE_SYSTEM                      FILE_ATTRIBUTE_SYSTEM             
#define FILE_DIRECTORY                   FILE_ATTRIBUTE_DIRECTORY          
#define FILE_ARCHIVE                     FILE_ATTRIBUTE_ARCHIVE            
#define FILE_ENCRYPTED                   FILE_ATTRIBUTE_ENCRYPTED          
#define FILE_NORMAL                      FILE_ATTRIBUTE_NORMAL             
#define FILE_TEMPORARY                   FILE_ATTRIBUTE_TEMPORARY          
#define FILE_SPARSE_FILE                 FILE_ATTRIBUTE_SPARSE_FILE        
#define FILE_REPARSE_POINT               FILE_ATTRIBUTE_REPARSE_POINT      
#define FILE_COMPRESSED                  FILE_ATTRIBUTE_COMPRESSED         
#define FILE_OFFLINE                     FILE_ATTRIBUTE_OFFLINE            
#define FILE_NOT_CONTENT_INDEXED         FILE_ATTRIBUTE_NOT_CONTENT_INDEXED

//
// --- Tape drive features
//
#if defined(_WIN32)
 #define TAPE_COMPRESSION                0x00001
 #define TAPE_ECC                        0x00002
 #define TAPE_SET_COMPRESSION            0x00004
 #define TAPE_SET_ECC                    0x00008
 #define TAPE_REPORT_SMKS                0x00010
 #define TAPE_SET_BLOCK_SIZE             0x00020
 #define TAPE_LOCK_UNLOCK                0x00040
 #define TAPE_LOAD_UNLOAD                0x00080
 #define TAPE_WRITE_LONG_FMKS            0x00100
 #define TAPE_WRITE_SHORT_FMKS           0x00200
 #define TAPE_WRITE_FILEMARKS            0x00400
 #define TAPE_TENSION_TAPE               0x00800
 #define TAPE_WRITE_MARK_IMMED           0x01000
 #define TAPE_LOCK_UNLOCK_IMMED          0x02000
 #define TAPE_LOAD_UNLOAD_IMMED          0x04000
 #define TAPE_REWIND_IMMED               0x08000
 #define TAPE_TENSION_TAPE_IMMED         0x10000

 #define DEFAULT_TAPE_LOAD_RETRY_TIME    120     // -- retry time (s)

#endif

//
// --- sql43c_tcpip_init results
//
#define TCPIP_INIT_OK         0
#define TCPIP_INIT_NOTOK        1
#define TCPIP_INIT_WRONG_VERSION    2

//
// --- some object creation options
//
#define  CREATE_THRD_SUSPENDED         CREATE_SUSPENDED
#define  CREATE_EVENT_SEM_SHARED       1

//
// --- thread priority
//
#define SERVER_THRD_PRIO                THREAD_PRIORITY_HIGHEST
#define SERVER_LOOP_THRD_PRIO           THREAD_PRIORITY_HIGHEST
#define SERVER_REQUESTOR                THREAD_PRIORITY_HIGHEST

#define DEFAULT_UKT_THRD_PRIO           THREAD_PRIORITY_ABOVE_NORMAL
#define NETWORK_THRD_PRIO               THREAD_PRIORITY_ABOVE_NORMAL
#define CLOCK_THRD_PRIO                 THREAD_PRIORITY_NORMAL
#define REQUESTOR_THRD_PRIO             THREAD_PRIORITY_HIGHEST
#define DEV0_THRD_PRIO                  THREAD_PRIORITY_NORMAL
#define DEVI_THRD_PRIO                  THREAD_PRIORITY_HIGHEST
#define ASYNC0_THRD_PRIO                THREAD_PRIORITY_NORMAL
#define ASYNCI_THRD_PRIO                THREAD_PRIORITY_HIGHEST
#define DCOM0_THRD_PRIO                 THREAD_PRIORITY_NORMAL
#define DCOMI_THRD_PRIO                 THREAD_PRIORITY_NORMAL 
#define COORDINATOR_THRD_PRIO           THREAD_PRIORITY_NORMAL
#define TIMER_THRD_PRIO                 THREAD_PRIORITY_NORMAL
#define CONSOLE_THRD_PRIO               THREAD_PRIORITY_ABOVE_NORMAL
#define DEV_WORKER_THRD_PRIO            THREAD_PRIORITY_HIGHEST

//
// --- titles and program names
//
#define XSERV_TITLE                      _T("XServer")
#define XSTOP_TITLE                      _T("XStop")
#define XSTART_TITLE                     _T("XStart")
#define XSHOW_TITLE                      _T("XShow")
#define DB_TITLE                         _T("SAPDB")
#define SERVERDB_EVENT_SOURCE_NAME       _T("SAPDB:")
#define EVENT_SOURCE_NAME                _T("SAPDB")
#define KERNEL_APPLIC_NAME               _T("SAPDB - ")
#define KERNEL_APPLIC_CLASS              _T("SAPDB Kernel") // - OS2 PM-Console

//
// --- special Windows NT definitions (registry, service, ...)
//
#define SERVER_TYPE_SERVERDB             0 
#define SERVER_TYPE_GATEWAY              1
#define SERVER_TYPE_ADABAS_SERVERDB      2
#define SERVER_TYPE_ADABAS_GATEWAY       3
#define SERVER_TYPE_XSERVER              4

#define DATABASE_OPERATOR_GROUP          _T("SAPDB Operators")
#define L_DATABASE_OPERATOR_GROUP        L"SAPDB Operators"
#define DOMAIN_DATABASE_OPERATOR_GROUP   _T("Domain SAPDB Oper")
#define L_DOMAIN_DATABASE_OPERATOR_GROUP L"Domain SAPDB Oper"
#define L_DATABASE_OPERATOR_GROUP_DESC   L"Members can administer existing databases"

#define PRODUCT_GROUP_NAME               _T("SAP DBTech")
#define SYSTEM_GUEST_NAME                _T("GUEST")
#define DR_WATSON_EXENAME                _T("DRWTSN32")
#define ADABAS_SERVICE_ID                _T("ADABAS-")
#define ADABAS_SERVICE_ID_GW             _T("ADABAS GW-")
#define SERVICE_ID                       _T("SAP DBTech-")
#define SERVICE_DISPLAY_STR              _T("SAPDB:  ")
#define SERVICE_ID_GW                    _T("SAP DBTech GW-")
#define SERVICE_DISPLAY_STR_GW           _T("SAPDB Gateway:  ")
#define SERVICE_QUICK_EXT                _T(" (quick)")
#define SERVICE_SLOW_EXT                 _T(" (slow)")
#define SERVICE_TEST_EXT                 _T(" (test)")
#define SERVICE_ID_XSERVER               _T("XServer")
#define SERVICE_DISPLAY_STR_XSERVER      XSERV_TITLE

#define REG_SK_BASE                      _T("Software\\SAP")
#define REG_SK_APPLIC                    _T("SAP DBTech")
#define REG_SK_ADABAS_APPLIC             REG_SK_APPLIC
#define REG_SK_SERVICE_PARAM             _T("Parameters")
#define REG_SK_DLLS                      _T("DLLs")
#define REG_SK_SERVICE                   _T("SYSTEM\\CurrentControlSet\\Services")
#define REG_SK_EVENT_LOG                 _T("EventLog\\Application")
#define REG_SK_AEDEBUG                   _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\AeDebug")
#define REG_SK_PRODUCTOPTIONS            _T("System\\CurrentControlSet\\Control\\ProductOptions")
#define REG_SK_NT_CURRENT_VERSION        _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion")
#define REG_SK_NT_CLUSTER_SERVER_SERVICE _T("SYSTEM\\CurrentControlSet\\Services\\ClusSvc")
#define REG_SK_NT_CLUSTER_SERVER_HIVE    _T("Cluster")
#define REG_SK_NT_SYSTEM_ENVIRONMENT     _T("SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment")
#define REG_SK_NT_SYSTEM_POWER           _T("SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Power")

#define REG_VN_CLUSTER_SERVER_LOGSIZE    _T("ClusterLogSize")
#define REG_VN_SERVICE_IMAGE_PATH        _T("ImagePath")
#define REG_VN_SERVICE_PACK              _T("CSDVersion")
#define REG_VN_AEDEBUG                   _T("Debugger")
#define REG_VN_XUSER                     _T("UData")
#define REG_VN_DBROOT                    _T("DBRoot")
#define REG_VN_VERSION                   _T("Version")
#define REG_VN_VERSION_ID                _T("VersionID")
#define REG_VN_KEY_ID                    _T("KeyID")
#define REG_VN_KERNEL_VERSION            _T("KernelVersion")
#define REG_VN_MAX_CON                   _T("MaxConnections")
#define REG_VN_MSG_FILE                  _T("EventMessageFile")
#define REG_VN_EV_TYPES                  _T("TypesSupported")
#define REG_VN_CATEGORY_FILE             _T("CategoryMessageFile")
#define REG_VN_CATEGORY_COUNT            _T("CategoryCount")
#define REG_VN_DIAG_NAME                 _T("ApplDiagFileName")
#define REG_VN_DIAG_ENABLED              _T("ApplDiagEnabled")
#define REG_VN_CHAR_SET_ENABLED          _T("DBCharSetEnabled")
#define REG_VN_CHAR_SET_NAME             _T("DBCharSet")
#define REG_VN_TAPE_LOAD_RETRY_TIME      _T("MaxTapeLoadTime")
#define REG_VN_SQLOPT_ENABLED            _T("SQLOptEnabled")
#define REG_VN_SQLOPT                    _T("SQLOpt")
#define REG_VN_TAPE_COMPRESS             _T("TapeCompression")
#define REG_VN_TERMINATION_TIMEOUT       _T("TerminationTimeout")
#define REG_DBLOCALE_SQLOPT              _T("DBLocale")
#define REG_VN_TCPIP_DLL                 _T("TCPIP")
#define REG_VN_USER                      _T("User")
#define REG_VN_USE_XUDATA_POS            _T("UDataPos")
#define REG_VN_DEBUG_BREAK               _T("DebugBreak")
#define REG_VN_SERVICE_TYPE              _T("Type")
#define REG_VN_DEV_WORKERS               _T("IOWorkers")
#define REG_VN_CONCURRENT_DEV_WORKERS    _T("ConcurrentIOWorkers")
#define REG_VN_CONFIG_PATH               _T("ConfigPath")
#define REG_VN_WRK_PATH                  _T("WorkPath")
#define REG_VN_OBJECT_NAME               _T("ObjectName")
#define REG_VN_INDEPENDENT_PGM           _T("Programs")
#define REG_VN_INDEPENDENT_DATA          _T("Data")
#define REG_VN_SERVICE_PACK              _T("CSDVersion")
#define REG_VN_PRODUCT_SUITE             _T("ProductSuite")
#define REG_VN_ALWAYS_CHECK_CONN         _T("AlwaysCheckConn")
#define REG_VN_USE_INDEP_PATH            _T("UseIndependentPath")
#define REG_VN_DEBUG_LEVEL               _T("DbgLevel")
#define REG_VN_DIAGFILE_SIZE             _T("DiagFileSize")
#define REG_VN_NO_DNS_REVERSE_LOOKUP     _T("NoDNSReverseLookup")
#define REG_VN_AWE_RESERVED_MEMORY       _T("AWEReservedPhysMemory")
#define REG_VN_HEURISTICS                _T("Heuristics")
#define REG_VN_PAUSE_ENABLED             _T("PauseEnabled")
#define REG_VN_PAUSE                     _T("Pause")
#define REG_VN_PORTNO                    _T("PortNo")
#define REG_VN_NI_PORTNO                 _T("NIPortNo")
#define REG_VN_SSL_PORTNO                _T("SSLPortNo")

#define REG_MSG_FILE                     _T("sqlmsg.dll")

// --- microsoft product suites
#define PSUITE_TERMINAL_SERVER           _T("Terminal Server")

// --- service types
#define SV_TYPE_SAPDB                    0x18800000

// --- microsoft cluster server
#define REG_MSCS_PARAM_SERVERDB_OS00     _T("SERVERDB")
#define REG_MSCS_PARAM_AUTO_RES_SHT_OS00 _T("AutoRestartShutdown")
#define REG_MSCS_PARAM_REMARK_OS00       _T("Remark")
#define REG_MSCS_PARAM_LOG_VERBOSE_OS00  _T("LogVerbose")

#define RESOURCE_TYPE_SERVERDB_OS00      _T("SERVERDB (SAP DBTech)")
#define RESOURCE_TYPE_XSERVER_OS00       _T("XServer  (SAP DBTech)")
#define RESOURCE_TYPE_IP_ADDRESS_OS00    _T("IP Address")

#define GROUP_NAME_OS00                  _T("SAP DBTech")
#define RESOURCE_NAME_IP_ADDRESS_OS00    _T("SAP DBTech IP-Address")
#define RES_STRT_STOP_PEND_TIMEOUT_OS00  _T("PendingTimeout")

#define SAPDBCLUEX_DLL_NAME_OS00         _T("SAPDBMSCSEx.Dll")
#define SAPDBCLURES_DLL_NAME_OS00        _T("SAPDBMSCSRes.Dll")
#define SAPDBCLUMAN_DLL_NAME_OS00        _T("SAPDBMSCSMan.Dll")
#define CLUSTER_SERVER_API_DLL_OS00      _T("clusapi.dll")

//
// --- path and file names
//
#define DEFAULT_TCPIP_DLL                _T("sqltcp.dll")
#define CMD_FILENAME                     _T("cmd.exe")
#define CMD_COMMAND_PARAMETER            _T("/C")
#define USER_DIAGFILE_NAME               _T("appldiag")
#define FULL_USER_DIAGFILE_NAME          _T("%DBROOT%\\wrk\\appldiag")
#define BATCH_DIAGFILE_NAME              _T("batdiag")
#define FULL_BATCH_DIAGFILE_NAME         _T("%DBROOT%\\wrk\\batdiag")
#define KERN_ERR_FILE_NAME               _T("kernel.err")
#define SERV_ERR_FILE_NAME               _T("serv.err")
#define SERVER_TYPE_FILENAME             _T("config\\server.typ")
#define DB_XSERVER_FILE_NAME             _T("serv.exe")
#define DB_FAST_KERN_FILE_NAME           _T("pgm\\kernel.exe")
#define DB_QUICK_KERN_FILE_NAME          _T("pgm\\quickknl.exe")
#define DB_SLOW_KERN_FILE_NAME           _T("pgm\\slowknl.exe")
#define DB_KERN_ICON_NAME                _T("pgm\\kernel.ico")
#define DB_TEST_KERN_FILE_NAME           _T("pgm\\omststknl.exe")
#define DB_KERN_PATH_NAME                _T("pgm\\")
#define XSERV_DIAGNAME                   _T("xserver.prt")
#define XUSER_DEFAULT_PATH_NAME          _T("%HOMEDRIVE%%HOMEPATH%\\user.dat")

//
// --- environment variables
//
#define BATCH_DIAG_FILE_ENV_VAR          _T("BATCHDIAGFILE") // - SQLEXEC
#define DBTERM_ENV_VAR                   _T("DBTERM")
#define TAPE_COMPRESS_ENV_VAR            _T("TAPECOMPRESS")  // - 'OFF' - no comp.
#define SERVERDB_ENV_VAR                 _T("SERVERDB")
#define HOSTNAME_ENV_VAR                 _T("HOSTNAME")
#define DBROOT_ENV_VAR                   _T("DBROOT")
#define USERDIAG_ENV_VAR                 _T("DIAGFILE")      // - 'YES' diag. on
#define CHAR_SET_ENV_VAR                 _T("DBCHARSET")
#define DBLOCALE_ENV_VAR                 _T("DBLOCALE")
#define CLUSTERLOG_ENV_VAR               _T("CLUSTERLOG")


//
// --- named object identifier
//
#define SEM_XSTART                      _T("SAP DBTech-XSTART-")
#define SEM_XSTART_CRASH                _T("SAP DBTech-CRASH-")
#define SEM_XSTOP                       _T("SAP DBTech-XSTOP-")
#define SEM_XSERVER                     _T("SAP DBTech-XSERVER-")
#define SEM_COORD                       _T("SAP DBTech-COORD-")
#define SEM_UKT                         _T("SAP DBTech-UKT-")
#define SEM_USR                         _T("SAP DBTech-USR-")
//#define SHM_CONSREC                     _T("SAP DBTech-CONSREC-")
#define SHM_COMMSEG                     _T("ADABAS-COMMSEG-")
#define SHM_KSS                         _T("ADABAS-KSS-")
#define SHM_FCS                         _T("ADABAS-FCS-")
#define MAIL_REQ                        _T("\\\\.\\mailslot\\ADABAS\\REQ\\")
//#define MAIL_CONS                       _T("\\\\.\\mailslot\\SAP DBTech\\CONS\\")
#define XSERVER_OBJ_NAME                _T("XSERVER")

//
// --- additional error codes
//
#if defined(_WIN32)
 #define ERROR_DUPLICATE_NAME            ERROR_ALREADY_EXISTS
 #define ERROR_ALREADY_POSTED            NO_ERROR

 #if !defined(ERROR_TIMEOUT)
  #define ERROR_TIMEOUT                  1460
 #endif
#else
 #define ERROR_FILEMARK_DETECTED         ERROR_HANDLE_EOF
 #define ERROR_END_OF_MEDIA              ERROR_HANDLE_EOF
 #define ERROR_NO_DATA_DETECTED          ERROR_HANDLE_EOF
#endif

//
// --- socket - communication
//
#if defined (RTE_USE_SOCK)
#define INET_ADDR_SIZE                  4
#define SOCENOBUFS                      WSAENOBUFS
#define SOCEBADF                        WSAEBADF
#define SOCEADDRINUSE                   WSAEADDRINUSE
#define SOCENOTSOCK                     WSAENOTSOCK
#define SOCECONNREFUSED                 WSAECONNREFUSED
#define SOCEINTR                        WSAEINTR
#endif 
//
// --- exception handling
//
#define XCPT_ABORT_EXCEPTION            0xE000AFFE  // -- Exception number for 'abort'
#define XCPT_ABORT_EXCEPTION_MINI_DUMP  0xE000AAAA  // -- Exception number for 'mini dump'
#define XCPT_MICROSOFT_CPP_EXCEPTION    0xE06D7363  // -- Exception number for Microsoft C++ Exceptions


// ========================== RTE internal MACROS =============================

//
// --- memory allocation
//
#define  ALLOCA(_s)                                                           \
            _alloca(_s)


#if defined (KERNEL)
 #define  ALLOC_MEM(_p,_s)                                                     \
            sql57k_pmalloc(__LINE__,__FILE__,_p,_s)

 #define  FREE_MEM(_p)                                                         \
            sql57k_pfree(__LINE__,__FILE__,_p)
#else
 #define  ALLOC_MEM(_p,_s)                                                     \
            sql57c_malloc(__LINE__,__FILE__,_p,_s)

 #define  FREE_MEM(_p)                                                         \
            sql57c_free(__LINE__,__FILE__,_p)
#endif


#define  ALIGN(_val,_alignment)                                               \
           ((((_val) - 1) / (_alignment) + 1 ) * (_alignment))
#define  BLANKS_ONLY( _psz )                                                  \
           (strlen( _psz) == strspn( _psz, " " ))
#define  ARRAY_SIZE(_A)                                                       \
           (sizeof(_A) / sizeof((_A)[0]))


#define  THREADID                                                            \
            GetCurrentThreadId()

#define  GETPROCESSID(ppid)                                                  \
            (*ppid = GetCurrentProcessId())

#define  EXITTHREAD(_rc, _handle)                                             \
            { HANDLE _H = _handle;      /* use vaiable to suppress warning */ \
              if (_H != INVALID_HANDLE_VALUE)                                 \
              {                                                               \
                HANDLE h = _H;                                                \
                _H = INVALID_HANDLE_VALUE;                                    \
                CloseHandle(h);                                               \
              }                                                               \
              ExitThread( (UINT)_rc );                                        \
            }

#define  EXITPROCESS(_rc)                                                    \
            ExitProcess( (UINT)_rc );

#define  SLEEP(_msec)                                                        \
            SleepEx(_msec, FALSE)

#define  BEEP(_Freq, _Dur)                                                   \
            Beep(_Freq, _Dur)

#define  GETENV(_pKey, _pVal)                                                \
            (!(*_pVal = getenv(_pKey)) ? ERROR_ENVVAR_NOT_FOUND : NO_ERROR)

#define  DELETE_FILE(_file)                                                  \
            (DeleteFile(_file) ? NO_ERROR : GetLastError())

#define  MOVE_FILE(_NewFile, _OldFile)                                       \
            (MoveFile(_NewFile, _OldFile) ? NO_ERROR : GetLastError())

#define  CLOSE_FILE(_hdl)                                                    \
            (CloseHandle((HANDLE)_hdl) ? NO_ERROR : GetLastError())

#define  SET_FILE_PTR(_hdl, _size, _typ, _plpos, _phpos)                     \
            ((*_plpos=SetFilePointer((HANDLE)_hdl,_size,_phpos,_typ)) != -1 ? \
              NO_ERROR : GetLastError())

#define  WRITE_FILE(_hdl, _buf, _size, _written)                             \
            (WriteFile((HANDLE)_hdl, _buf, _size, _written, NULL) ?          \
              NO_ERROR : GetLastError())

#define  READ_FILE(_hdl, _buf, _size, _read)                                 \
            (ReadFile((HANDLE)_hdl, _buf, _size, _read, NULL) ?              \
              NO_ERROR : GetLastError())

#define  WRITE_FILE_OL(_hdl, _buf, _size, _written, _ol)                     \
            (WriteFile((HANDLE)_hdl, _buf, _size, _written, _ol) ?           \
              NO_ERROR : GetLastError())

#define  READ_FILE_OL(_hdl, _buf, _size, _read, _ol)                         \
            (ReadFile((HANDLE)_hdl, _buf, _size, _read, _ol) ?               \
              NO_ERROR : GetLastError())


#define  CREATE_THREAD(_pTid, _pHandle, _pFunc, _Arg, _Options, _StackSize )  \
            ((*_pHandle = CreateThread(NULL, _StackSize,                      \
                                       (LPTHREAD_START_ROUTINE)_pFunc,        \
                                       (LPVOID)_Arg, _Options, _pTid )) ?     \
               NO_ERROR : (*_pHandle = INVALID_HANDLE_VALUE, GetLastError()))

#define  RESUME_THREAD(_Tid, _Handle )                                       \
            ((ResumeThread(_Handle) != (DWORD)-1) ? NO_ERROR : GetLastError())

#define  WAIT_THREAD(_Handle, _Sec)                                          \
            while((WaitForSingleObject(_Handle, 1000) == WAIT_TIMEOUT) &&    \
                  (_Sec > 0)) { _Sec--; }

#define  KILL_THREAD( _Tid, _Handle )                                        \
            (TerminateThread(_Handle, 1) ?                                   \
                            (CloseHandle(_Handle), NO_ERROR) : GetLastError())

#define  SET_THREAD_PRIO( _Prio, _Tid, _Handle )                             \
            (SetThreadPriority(_Handle, _Prio ) ? NO_ERROR : GetLastError())

//
// --- ABORT
//
#if defined (SERVER)
 #define  ABORT_NO_CRASH_DUMP()                                               \
            sql96c_abort ();

 #define  ABORT()                                                             \
            sql96c_abort ();
#elif defined (USER)
 #define  ABORT_NO_CRASH_DUMP()                                               \
            sqlabort()

 #define  ABORT()                                                             \
            sqlabort()
#elif defined (KERNEL)
 #define  ABORT_NO_CRASH_DUMP()                                               \
            sql50k_rte_abort( FALSE, __LINE__, __FILE__ )

 #define  ABORT()                                                             \
            sql50k_rte_abort( TRUE, __LINE__, __FILE__ )
#else
 #define  ABORT_NO_CRASH_DUMP()                                               \
            EXITPROCESS( (ULONG)-1)

 #define  ABORT()                                                             \
            EXITPROCESS( (ULONG)-1)
#endif

#define RTE_ASSERT(_as, _err)                                                 \
            if ( _as ) {                                                      \
              MSGCD(_err);                                                    \
              ABORT(); }



// ========================== RTE internal TYPES ==============================

// --- Initialization ---------------------------------------------------------

#define OS_OS2                  4
#define OS_WIN32                3

typedef CHAR                          C40C[41] ;
typedef CHAR                          tsp2_devnamec [ ALIGN_4BYTE_EO00(sizeof(tsp00_VFilename)+1) ];
typedef CHAR                          C8C[9] ;
typedef CHAR                          C256[256] ;

typedef tsp00_C64                       C64 ;
typedef CHAR                          C64C [ sizeof ( tsp00_C64 ) + 1 ] ;

typedef CHAR                          VF_FILENAMEC [ sizeof ( tsp00_VFilename ) + 1 ] ;
typedef CHAR                          tsp_knl_identifierC [ sizeof ( tsp00_KnlIdentifier ) + 1 ];
typedef tsp00_NodeId                  SQL_NODEID;
typedef tsp00_DbName                  SQL_DBNAME;
typedef CHAR                          SQL_NODEIDC[ sizeof(tsp00_NodeId) + 1 ] ;
typedef CHAR                          SQL_DBNAMEC[ sizeof(tsp00_DbName) + 1 ] ;
typedef tsp00_TaskId                  PROCESS_ID ;
typedef tsp00_RegionId                REGION_ID;
typedef tsp00_RteCommRec              RTE_COMM_REC ;
typedef tsp00_RteCommRec              *PRTE_COMM_REC;
typedef tsp00_Page                    *BUFFERADDRESS;                
typedef tsp00_ErrText                 ERRORTEXT;                

#define MAXERRORTEXT                  ( sizeof ( tsp00_ErrText ) )
#define MAXBUFFERLENGTH               mxsp_buf
#define MX_DBNAME                     ( sizeof ( tsp00_DbName ) )
#define MX_NODEID                     ( sizeof ( tsp00_NodeId ) )
#define MX_LINE                       ( sizeof ( tsp00_Line ) )


#define MAX_DIAG_HEADER_LEN           80

typedef unsigned char           enum1;
typedef unsigned                CINT;
typedef char                    INT1;
typedef char*                   PINT1;         
typedef unsigned char           UINT1;
typedef unsigned char*          PUINT1;
typedef short                   INT2;
typedef short*                  PINT2;          
typedef unsigned short          UINT2;
typedef tsp00_Int4                INT4;
typedef INT4*                   PINT4;
typedef unsigned long           UINT4;
typedef float                   FLOAT4;
typedef double                  FLOAT8;
typedef FLOAT8                  tsp00_Longreal ;

#define FILL(d,s,l)     (void) memset ( (char*)(d) ,   (int)(s) , (CINT)(l) )


typedef DWORD                         APIRET;
typedef PVOID                         *PPVOID;
typedef DWORD                         TID, *PTID;
typedef DWORD                         PID, *PPID;
typedef HANDLE                        HEV, *PHEV;
typedef VOID                          ( __cdecl * SIGNALFUNCTYPE )( INT SigType );
typedef VOID _System                  (*PFNEXITLIST)(ULONG);
typedef CHAR                          IPC_BASE_OBJ_NAME[70];
typedef IPC_BASE_OBJ_NAME             *PIPC_BASE_OBJ_NAME;
typedef EXCEPTION_RECORD              EXCEPTION_REC;
typedef EXCEPTION_RECORD              EXCEPTION_REPORT_REC;
typedef CHAR                          PATHNAME[MAX_PATH + 1];
typedef CHAR                          SERVICE_DISPLAY_NAME[MAX_PATH + 1];
typedef DWORD                         QWORD[2];

typedef LONG                           ( *PMSGFUNCTYPE )( char * MessageString );
typedef EXCEPTION_REC                  *PEXCEPTION_REC;
typedef EXCEPTION_REPORT_REC           *PEXCEPTION_REPORT_REC;
typedef ULONG                          COMMHANDLE;
typedef COMMHANDLE                     *PCOMMHANDLE;


//
// --- kernel shared section
//
/*
    -> Moved to RTE_KSS.h
*/

/*
 *  - Structure of the kernel communication segment
 *
 *       +------------------------------------+
 *    +--|     flag_comm_seg_header           |--+
 *    |  |                                    |----+
 *    |  +------------------------------------+  | |
 *    +->|     CommFlag 0                     |  | |
 *       |        .     .                     |  | |
 *       |        .     .                     |  | |
 *       |     CommFlag n                     |  | |
 *       +------------------------------------+  | |
 *       |     SemaFlag 0                     |<-+ |
 *       |        .     .                     |    |
 *       |        .     .                     |    |
 *       |     SemaFlag m                     |    |
 *       +------------------------------------+    |
 *       |     SemaName 0                     |<---+ 
 *       |        .     .                     |    
 *       |     SemaName m                     |    
 *       +------------------------------------+    
 */
typedef struct flag_comm_seg
  {
  ULONG                             ulPacketSize;
  ULONG                             ulMinReplySize;

  ULONG                             ulNumOfCommFlags; // - Flags are behind
  ULONG                             ulCommFlagOffset; //   this segment!
  ULONG                             ulNumOfSemaphores;// - Flags are behind
  ULONG                             ulSemaFlagOffset; //   this segment!
  ULONG                             ulSemaNameOffset; // - Names are behind this segment!
  BOOL                              fSemDemandOpen;
  CHAR                              szSemPath[ 256 ];
  CHAR                              AuthenticationAllow[ 256 ];
  } FLAG_COMM_SEG;


typedef  FLAG_COMM_SEG             *PFLAG_COMM_SEG;

//
// --- Windows NT / registry handling
//
typedef struct reg_entry_record
  {
  char *                           pszValueName;
  PVOID                            pValue;
  ULONG                            ulValueSize;
  ULONG                            ulValueType;
  ULONG                            rc;
  } REG_ENTRY_REC;

typedef REG_ENTRY_REC      *PREG_ENTRY_REC;

typedef struct user_access_rights_record
  {
  ULONG                  ulAccess;
  ULONG                  ulAceFlags;
  ULONG                  ulInheritAccess;
  ULONG                  ulInheritAceFlags;
  } USER_ACCESS_RIGHTS_REC;

typedef USER_ACCESS_RIGHTS_REC  *PUSER_ACCESS_RIGHTS_REC;

typedef struct access_rights_record
  {
  BOOL                   fDescOwnerIsAdmin;
  BOOL                   fAddDefaultDACL;
  SAPDB_Bool             ignoreOwnUser;
  SAPDB_Char *           pUseOtherUser;

  USER_ACCESS_RIGHTS_REC User;
  USER_ACCESS_RIGHTS_REC OtherUser;
  USER_ACCESS_RIGHTS_REC Creator;
  USER_ACCESS_RIGHTS_REC Admin;
  USER_ACCESS_RIGHTS_REC System;
  USER_ACCESS_RIGHTS_REC SAPDBOp;
  USER_ACCESS_RIGHTS_REC DomSAPDBOp;
  USER_ACCESS_RIGHTS_REC World;
  USER_ACCESS_RIGHTS_REC BackupOp;
  } ACCESS_RIGHTS_REC;

typedef ACCESS_RIGHTS_REC  *PACCESS_RIGHTS_REC;

typedef enum 
  {
  no_diag_header,
  big_diag_header,
  big_user_diag_header,
  small_diag_header,
  event_log_header,
  time_diag_header,
  } DIAG_HEADER_TYPE;


// ========================== RTE internal PROTOTYPES =========================

#ifdef __cplusplus
extern "C" {
#endif
// --- VOS01U ----------------------------------------------------------------
void sqlSetModuleHandle (HANDLE  moduleHandle);

// --- VOS01C ----------------------------------------------------------------

BOOL            sql01c_get_dbroot         ( char *         *ppszDBRoot );
BOOL            sql01c_get_serverdb       ( char *         *ppszServerDB );
BOOL            sql01c_get_dblang         ( char *         *ppszDBLang );
VOID            sql01c_build_pgm_path     ( char *         pszDBRoot ,
                                            char *         pszPgmPath );
VOID            sql01c_build_config_path  ( char *         pszDBRoot ,
                                            char *         pszConfigPath );
VOID            sql01c_get_config_path    ( char *         *ppszConfigPath );
LONG            sql01c_set_config_path    ( char *         pszConfigPath );
VOID            sql01c_get_wrk_path       ( char *         *ppszWrkPath );
LONG            sql01c_set_wrk_path       ( char *         pszWrkPath );
BOOL            sql01c_set_symbol_path    ( char *         pszDBRoot );
VOID            sql01c_build_sap_path     ( char *         pszDBRoot, 
                                            char *         pszSAPPath );
/* PTS 1113170 */
/*!
   @brief          This funtions determines if the running program was compiled as a 32 bit application and is
              now running on a 64 bit machine (Itanium).
   @return         True:   32 bit application on 64 bit (Itanium).
              False:  Else.

 */

SAPDB_Bool      sql01c_Is32BitApplOn64Bit ( void );


// --- VOS02C ----------------------------------------------------------------

VOID            sql02_get_os_version           ( PULONG          pulMajorVersion,
                                                 PULONG          pulMinorVersion );
// sql02_get_platform_id:
//        Identifies the operating system platform. This member can be one of the following values.
//        This member can be a combination of the following values.  
//
//        ReturnValue                    Platform 
//        ----------------------------------------------------------------
//        VER_PLATFORM_WIN32s            Win32s on Windows 3.1.  
//        VER_PLATFORM_WIN32_WINDOWS     Win32 on Windows 95 or Windows 98.  
//        VER_PLATFORM_WIN32_NT          Win32 on Windows NT. 
//
ULONG           sql02_get_platform_id          ( void );

// sql02_get_service_pack:
//        Identifies the version number of the latest Service Pack installed on the system. 
//        If no Service Pack has been installed, the value is zero. 
//
//        ReturnValue: Version number of the latest Service Pack
//
DWORD           sql02_get_service_pack         ( void );


// sql02_get_os_suite_mask:
//        A set of bit flags that identify the product suites available on the system. 
//        This member can be a combination of the following currently known values. 
//        ( For detailed information view Microsoft Developer Network CD, 
//          subject OSVERSIONINFOEX )
//
//        ReturnValue                         Meaning
//        ----------------------------------------------------------------
//        VER_SUITE_SMALLBUSINESS             Microsoft® Small Business Server is installed.  
//        VER_SUITE_ENTERPRISE                Windows NT® Server Enterprise Edition is installed. 
//        VER_SUITE_BACKOFFICE                Microsoft® BackOffice® components are installed. 
//        VER_SUITE_COMMUNICATIONS  
//        VER_SUITE_TERMINAL                  Windows®-based Terminal Server is installed. 
//        VER_SUITE_SMALLBUSINESS_RESTRICTED  Microsoft® Small Business Server is installed with 
//                                            the restrictive client license in force. 
//
#if defined (OSVERSIONINFOEX)
 WORD           sql02_get_os_suite_mask        ( VOID );
#endif
char *          sql02_get_product_suite        ( VOID );

BOOL            sql02_validate_product_suite   ( char * szSuiteName );

BOOL            sql02_release_resources        ( VOID );

// --- VOS03C ----------------------------------------------------------------

APIRET          sql03c_add_SAPDB_operators_group ( VOID );
APIRET          sql03c_del_SAPDB_operators_group ( VOID );
APIRET          sql03c_get_domain_name           ( char * pszDomainName );
APIRET          sql03c_group_or_user_avail       ( char * pszNodeName,
                                                   char * pszGroupOrUserName );
APIRET          sql03c_group_or_user_on_DC_avail ( char * pszGroupOrUserName );

// --- VOS09C ----------------------------------------------------------------

BOOL            sql09c_init_handle_dir   ( PVOID               *ppHandles,
                                           ULONG               ulMaxHandles );
BOOL            sql09c_release_handle_dir( PVOID               pHandles );

BOOL            sql09c_reserve_handle    ( PVOID               pHandles,
                                           ULONG VAR_VALUE_REF pulHandle );
BOOL            sql09c_get_handle_data   ( PVOID               pHandles,
                                           ULONG               ulHandle,
                                           PVOID               *ppData,
                                           PULONG              pulHType,
                                           PULONG              pulID );
BOOL            sql09c_put_handle_data   ( PVOID               pHandles,
                                           ULONG               ulHandle,
                                           PVOID               pData,
                                           ULONG               ulHType,
                                           ULONG               ulID );
BOOL            sql09c_find_next_handle  ( PVOID               pHandles,
                                           PULONG              pulHandle );
BOOL            sql09c_free_handle       ( PVOID               pHandles,
                                           ULONG               ulHandle );
BOOL            sql09c_find_handle_by_id ( PVOID               pHandles,
                                           ULONG               ulID,
                                           PULONG              pulHandle );
BOOL            sql09c_lock_handle       ( PVOID               pHandles,
                                           ULONG               ulHandle );
BOOL            sql09c_unlock_handle     ( PVOID               pHandles,
                                           ULONG               ulHandle );

// --- VOS40UC - ( Abort and Exit Functions ) ---------------------------------

VOID    sqlabort    (   VOID    );


// --- VOS40C.C/VOS40L.C ------------------------------------------------------

BOOL            sql40c_free_dll             ( VOID );

ULONG           sql40c_get_dll_version      ( tsp100_VersionID1 *pVersion );

INT             sql40c_sock_cleanup         ( VOID );

INT             sql40c_getservbyname        ( PCHAR           pszService,
                                              PUSHORT         pusServicePort );
INT             sql40c_gethostbyname        ( char *          pszHostName,
                                              PUCHAR          pucAddr,
                                              PINT            pAddrLen );
INT             sql40c_gethostbyaddr        ( char *          pszHostName,
                                              PUCHAR          pucAddr,
                                              LONG            lAddrLen );
INT             sql40c_sock_init            ( VOID );

INT             sql40c_get_sock_version     ( UCHAR           *pcMajorSockVersion,
                                              UCHAR           *pcMinorSockVersion );
INT             sql40c_select               ( PINT            pSDCount,
                                              PVOID           pReadSDMask,
                                              PVOID           pWriteSDMask,
                                              PVOID           pExceptSDMask,
                                              USHORT          usTimeOut );
INT             sql40c_is_inet_addr_valid   ( PCHAR           Addr );

LONG            sql40c_set_tcpip_dll_path   ( char *          pszTCPIPDLLName );
ULONG           sql40c_get_tcpip_dll_path   ( char *          *ppszTCPIPDLLName );

#if defined (RTE_USE_SOCK)
INT             sql40c_stream_socket        ( teo00_Socket    *psd );
INT             sql40c_stream_socket2       ( teo00_Socket    *psd,
                                              DWORD           flags);
INT             sql40c_socket_close         ( teo00_Socket    sd );
INT             sql40c_accept_conn_request  ( teo00_Socket    sd,
                                              teo00_Socket    *psdClient,
                                              PVOID           pSockAddr,
                                              PINT            pLen );
INT             sql40c_keep_alive           ( teo00_Socket    sd );
INT             sql40c_linger               ( teo00_Socket    sd,
                                              BOOL            fLinger,
                                              INT             iSeconds );
INT             sql40c_bind                 ( teo00_Socket    sd,
                                              USHORT          usServicePort );

INT             sql40c_bind_on_port_and_inet_addr ( teo00_Socket  sd, 
                                                    USHORT        usServicePort,
                                                    PCHAR         pszInetAddr );

INT             sql40c_listen               ( teo00_Socket    sd,
                                              INT             Count );
INT             sql40c_set_send_buffer_size ( teo00_Socket    sd,
                                              PINT            pMaxSize,
                                              INT             MinSize );
INT             sql40c_set_recv_buffer_size ( teo00_Socket    sd,
                                              PINT            pMaxSize,
                                              INT             MinSize );
INT             sql40c_connect              ( teo00_Socket    sd,
                                              USHORT          usServicePort,
                                              PUCHAR          pucName,
                                              INT             NameLen );
INT             sql40c_send_packet          ( teo00_Socket    sd,
                                              PCHAR           pPacket,
                                              INT             SendLen,
                                              PINT            pBytesSend );
INT             sql40c_recv_packet          ( teo00_Socket    sd,
                                              PCHAR           pPacket,
                                              INT             ReceiveLen,
                                              PINT            pBytesReceived );
INT             sql40c_get_serviceport      ( teo00_Socket    sd,
                                              PUSHORT         pusServicePort );

#endif


// --- VOS49C.C --------------------------------------------------------------
 LONG            sql49c_user_privilege_check     ( BOOL                  *pfIsOk );

 LONG            sql49c_get_token_information    ( PTOKEN_USER           *ppTU,
                                                   PTOKEN_GROUPS         *ppTG,
                                                   PTOKEN_DEFAULT_DACL   *ppTDacl );
 LONG            sql49c_set_sync_access          ( VOID );
 LONG            sql49c_get_kernel_obj_SD        ( PSECURITY_DESCRIPTOR  *ppSD,
                                                   HANDLE                hObject );
 LONG            sql49c_set_kernel_obj_sec       ( PSECURITY_DESCRIPTOR  pSD,
                                                   HANDLE                hObject );
 LONG            sql49c_alloc_and_init_SD        ( PACCESS_RIGHTS_REC    pAccess,
                                                   PSECURITY_DESCRIPTOR  *ppSD );
 LONG            sql49c_alloc_and_init_service_SD( PSECURITY_DESCRIPTOR  *ppSD,
                                                   SC_HANDLE             schService );
 LONG            sql49c_get_user_info            ( char *                *ppszLogonName,
                                                   char *                *ppszRefDomain );
 LONG            sql49c_user_is_admin            ( BOOL                  *pfIsAdmin );
 LONG            sql49c_get_user_textual_SID     ( SAPDB_Char *          accountName,
                                                   PSZ *                 ppszTextualSID);
 LONG            sql49c_set_file_security        ( char *                pszFileName,
                                                   PSECURITY_DESCRIPTOR  pSD );
 LONG            sql49c_set_service_logon_right  ( char *                pszNodeName,
                                                   char *                pszAccountName );

// --- VOS50C.C --------------------------------------------------------------
 LONG sql50_reg_del_service_value  ( char *                    pszNodeName,
                                     char *                    pszSubKey,
                                     char *                    pszValueName );
 LONG sql50_reg_del_service_key    ( char *                    pszNodeName,
                                     char *                    pszSubKey );
 LONG sql50_reg_get_service_values ( char *                    pszNodeName,
                                     char *                    pszSubKey,
                                     ULONG                     ulNumOfEntries,
                                     PREG_ENTRY_REC            pEntyArray );
 LONG sql50_reg_put_service_values ( char *                    pszNodeName,
                                     char *                    pszSubKey,
                                     ULONG                     ulNumOfEntries,
                                     PREG_ENTRY_REC            pEntyArray );
 LONG  sql50_reg_applic_key_exists ( char *                    pszNodeName,
                                     HKEY                      hKey,
                                     char *                    pszSubKey );
 LONG sql50_reg_del_applic_value   ( char *                    pszNodeName,
                                     HKEY                      hKey,
                                     char *                    pszSubKey,
                                     char *                    pszValueName );
 LONG sql50_reg_del_applic_key     ( char *                    pszNodeName,
                                     HKEY                      hKey,
                                     char *                    pszSubKey );
 LONG sql50_RegistryDeleteApplSupKey (HKEY                     hKey,
                                     SAPDB_Char *              pszSubKey);
 LONG sql50_reg_get_applic_values  ( char *                    pszNodeName,
                                     HKEY                      hKey,
                                     char *                    pszSubKey,
                                     ULONG                     ulNumOfEntries,
                                     PREG_ENTRY_REC            pEntyArray );
 LONG sql50_reg_get_oldapplic_values(char *                    pszNodeName,
                                     HKEY                      hKey,
                                     char *                    pszSubKey,
                                     ULONG                     ulNumOfEntries,
                                     PREG_ENTRY_REC            pEntyArray );
 LONG sql50_reg_put_applic_values  ( char *                    pszNodeName,
                                     SAPDB_Char *              accountName,
                                     HKEY                      hKey,
                                     char *                    pszSubKey,
                                     char *                    pszPrivateKey,
                                     SAPDB_Char *              subSubKey,
                                     ULONG                     ulNumOfEntries,
                                     PREG_ENTRY_REC            pEntyArray );
 RTE_SystemRc sql50_SetKeySecurity  (SAPDB_Char *              accountName,
                                     HKEY                      hKey,
                                     SAPDB_Char *              subKey, 
                                     SAPDB_Char *              privateKey,
                                     SAPDB_Char *              subSubKey);
 LONG sql50_reg_key_exists         ( char *                    pszNodeName,
                                     HKEY                      hKey,
                                     char *                    pszSubKey );
 LONG sql50_reg_del_value          ( char *                    pszNodeName,
                                     HKEY                      hKey,
                                     char *                    pszSubKey,
                                     char *                    pszValueName );

 LONG sql50_reg_del_key            ( char *                    pszNodeName,
                                     HKEY                      hKey,
                                     char *                    pszSubKey );
 LONG sql50_reg_get_values         ( char *                    pszNodeName,
                                     HKEY                      hKey,
                                     char *                    pszSubKey,
                                     ULONG                     ulEntries,
                                     PREG_ENTRY_REC            pEntyArray );
 LONG sql50_reg_put_values         ( char *                    pszNodeName,
                                     HKEY                      hKey,
                                     char *                    pszSubKey,
                                     ULONG                     ulEntries,
                                     PREG_ENTRY_REC            pEntyArray,
                                     PSECURITY_ATTRIBUTES      pSA );
 LONG sql50_create_reg_key         ( char *                    pszNodeName,
                                     HKEY                      hKey,
                                     char *                    pszSubKey,
                                     PSECURITY_ATTRIBUTES      pSA );
 LONG    sql50_RegistryEnumOpen    ( HKEY                      hKey,
                                     SAPDB_Char *              pSubKey,
                                     PHKEY                     phKeyEnum);
 LONG    sql50_RegistryClose       ( HKEY                      hKey);
 LONG    sql50_RegistryEnumNext    ( HKEY                      hKey,
                                     DWORD *                   pIndex,
                                     SAPDB_Char *              entry,
                                     SAPDB_Int4                maxEntryLength,
                                     SAPDB_Char *              string,
                                     SAPDB_Int4                maxStringLength);

// --- VOS57C.C --------------------------------------------------------------

APIRET  sql57c_malloc                        ( ULONG      ulLine,
                                               char *     pszModuleName,
                                               PPVOID     ppvMemBlock,
                                               ULONG      ulSize      );
APIRET  sql57c_free                          ( ULONG      ulLine,
                                               char *     pszModuleName,
                                               PVOID      pvMemBlock  );
APIRET  sql57c_realloc                       ( ULONG      ulLine,
                                               char *     pszModuleName,
                                               PPVOID     ppvMemBlock,
                                               ULONG      ulSize      );
VOID    sql57c_heap_destroy                  ( VOID );



// --- VOS60C.C --------------------------------------------------------------

LONG          sql60_install_event_source   ( char *          pszNodeName,
                                             char *          pszEventLogSource,
                                             char *          pszEventFile,
                                             char *          pszCategoryFile,
                                             ULONG           ulCategories);
LONG          sql60_remove_event_source    ( char *          pszNodeName,
                                             char *          pszEventLogSource );
LONG          sql60_open_event_log         ( char *          pszEventLogTitle,
                                             char *          pszEventLogSource );
LONG          sql60_reopen_event_log       ( char *          pszEventLogTitle,
                                             char *          pszEventLogSource );
VOID          sql60_get_diag_filename      ( char *          *ppszPhysDiagFileName );
VOID          sql60_enable_GUI_console     ( PMSGFUNCTYPE pMessageFunc );
VOID          sql60_enable_default_device  ( VOID );
VOID          sql60_enable_console         ( VOID );
VOID          sql60_enable_message_box     ( VOID );
VOID          sql60_enable_message_output  ( VOID );
VOID          sql60_close_event_log        ( VOID );
VOID          sql60_close_diag_file        ( VOID );
VOID          sql60_close_error_diag_file  ( VOID );
VOID          sql60_disable_GUI_console    ( VOID );
VOID          sql60_disable_message_box    ( VOID );
VOID          sql60_disable_console        ( VOID );
VOID          sql60_disable_default_device ( VOID );
VOID          sql60_disable_message_output ( VOID );

VOID _System  sql60_int_err_msg_prio7      ( const char * pszFormatStr,
                                             ... );
VOID          sql60_strt_msg_wrt_cycle     ( VOID );
VOID _System  sql60_msg_prio_x             ( ULONG        ulPrio,
                                             LONG         lMsgID,
                                             ULONG        ulEventType,
                                             const char * pszLabel,
                                             const char * pszFormatStr,
                                             ... );

VOID          sql60_reset_crit_section     ( VOID );
VOID          sql60_release_resources      ( VOID );

LONG          sql60_open_diag_file         ( char *               pszDiagFileName,
                                             ULONG                ulDiagSize,
                                             PSECURITY_ATTRIBUTES pSA );
LONG          sql60_open_error_diag_file   ( char *               pszDiagFileName,
                                             PSECURITY_ATTRIBUTES pSA );

LONG          sql60_open_util_diag_file    ( char *               pszUtilDiagFileName,
                                             ULONG                ulDiagSize,
                                             PSECURITY_ATTRIBUTES pSA );

LONG          sql60_write_to_util_diag     ( const char *         pszMsg,
                                             ULONG                ulMsgLen );
VOID          sql60_close_util_diag_file   ( VOID );

LONG          sql60_open_event_file        ( char *               pszEventFileName,
                                             ULONG                ulDiagSize,
                                             PSECURITY_ATTRIBUTES pSA );

LONG          sql60_write_to_event_file    ( const char *         pszMsg,
                                             ULONG                ulMsgLen );

VOID          sql60_close_event_file       ( VOID );

VOID          sql60_create_diag_header     ( LONG                 lMsgID,
                                             ULONG                ulEventType,
                                             const char *         pszLabel,
                                             DIAG_HEADER_TYPE     HeaderType,
                                             char *               pszOutBuffer ); // - use 'MAX_DIAG_HEADER_LEN'
                                                                                  //   as minimum buffer size


// --- VOS76C -----------------------------------------------------------------

BOOL _Optlink sql76_interlock              ( LONG                 *plLockAddr );
VOID _Optlink sql76_clearlock              ( LONG                 *plLockAddr );

#if defined(_WIN32) && defined(_ALPHA_)
 PVOID        sql76_mem_sync               ( PVOID                pArg );
#endif

// --- VOS80C - ( getopt ) ----------------------------------------------------

INT             sql80_GetOpt               ( int          nArgc,
                                             char**       apszArgv,
                                             char*        pszOptString  );

// --- VOS90C ----------------------------------------------------------------

APIRET          sql90_start_db_gw_service    ( char *     pszNodeName,
                                               char *     pszServerDB,
                                               ULONG      ulOptions,
                                               ULONG      ulKernelType );
APIRET          sql90_stop_db_gw_service     ( char *     pszNodeName,
                                               char *     pszServerDB );
APIRET          sql90_start_serverdb         ( char *     pszServerDB,
                                               char *     pszDBRoot,
                                               ULONG      ulOptions,
                                               ULONG      ulKernelType );
APIRET          sql90_stop_serverdb          ( char *     pszServerDB,
                                               BOOL       fDump,
                                               BOOL       gracefully );


// --- VOS96C ----------------------------------------------------------------

// see "hos96.h"

// --- VOS96CC ----------------------------------------------------------------

VOID            sql96c_abort ( VOID );

// --- VOS97C ----------------------------------------------------------------

APIRET           sql97_update              ( char *       pszNodeName );

APIRET           sql97_first_db_gw_state   ( ULONG        ulServerType,
                                             char *       pszServerDB,
                                             PBOOL        pfActive );

APIRET           sql97_next_db_gw_state    ( char *       pszServerDB,
                                             PBOOL        pfActive );

APIRET           sql97_first_xserver_state ( char *       pszPort,
                                             ULONG        ulPortBufSize,
                                             PBOOL        pfNoService,
                                             PBOOL        pfActive );

APIRET           sql97_next_xserver_state  ( char *       pszPort,
                                             ULONG        ulPortBufSize,
                                             PBOOL        pfNoService,
                                             PBOOL        pfActive );

// --- VOS97CC ---------------------------------------------------------------

BOOL             sql97cc_findControlServer ( char *                         pszExecutable,// OUT
                                             char *                         pszServerDB,  // IN
                                             char *                         pszServerPgm, // IN
                                             char *                         pszDBRoot,    // IN/OUT
                                             tsp00_ErrText VAR_ARRAY_REF    errText);     // OUT
#if defined (USER)
// ===================== USER PROTOTYPES ======================================

// --- VWS07PC - ( Windows Terminal Initialization ) --------------------------

VOID           win07p_init        ( tsp00_CompName           acComponent,
                                    PBOOLEAN                 pbCancelAddress );

VOID           win07p_finish      ( BOOLEAN                  bTerminate     );

VOID           win07p_abort       ( VOID );


// --- VOS02UC - ( exit handling ) --------------------------------------------

APIRET         sql02u_add_exit_list  ( PFNEXITLIST pExitFunc );
APIRET         sql02u_next_exit_func ( VOID );
VOID           sql02u_run_exit_func  ( VOID );


// --- VOS03UC - ( Communication ) --------------------------------------------

VOID           sql03u_init        ( VOID );

VOID           sql03u_finish      ( VOID );


// --- VOS04UC - ( Virtual Terminal ) -----------------------------------------

VOID           sql04u_vt_crash_cleanup ( VOID );

// --- VOS17UC ----------------------------------------------------------------

VOID            sql17u_split_dbname( SQL_DBNAMEC             acServerDB,
                                     SQL_NODEIDC             acServerNode   );
#endif
#ifdef __cplusplus
}
#endif
#endif
