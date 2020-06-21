/*!
  @file           ven96.c
  @author         JoergM
  @brief          XServer main program
  @see

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


//
//   FreeBSD portions added by Kai Mosebach,
//   For more informations see : http://www.komadev.de/sapdb
//

\endif
*/



#define MOD__ "ven96.c:"
/*
* INCLUDE FILES
*/
#define VSP100_CPP 1 /* call Raymond if you want to fix this, the versionId type is defined as void otherwise */
#include "gen00.h"
#include "vsp001c.h"
#include "geo007_1.h"
#include "gen003.h"
#include "hsp100.h"
#include "gen32.h"
#include "gen33.h"
#include "hen40.h"
#include "gen41.h"
#include "gen42.h"
#include "gen43.h"
#include "heo46.h"
#include "gen46.h"
#include "heo420.h"
#if defined(_IBMR2) || defined(HP9)
# include "gen49.h"
#endif /* _IBMR2 || HP9 */
#include "gen904.h"
#include "hsp77.h"
#include "RunTime/Diagnose/RTEDiag_Appldiag.h"
#include <poll.h>
#include <sys/resource.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>
#include "RunTime/RTE_saveUNIXcalls.h"
#include "RunTime/RTE_SdbStarter.h"
#include "RunTime/RTE_IniFileHandling.h"
#include "SAPDBCommon/SAPDB_Names.h"
#include "geo908.h"
#include "heo12.h"
#include "RunTime/RTE_UNIXAuthenticate.h"
#include "RunTime/Security/RTESec_SSL.h"
#include "RunTime/Security/RTESec_SAPSSL.h"

#if defined(SUN) || defined(SOLARIS)
#define _POSIX_PTHREAD_SEMANTICS
#endif

/*
*  DEFINES
*/
#define    DBMSRV_REFERENCE    0x80000000

 /* PTS 1114659 */
#define SOCKET_DEAD_CHECK_TIMEOUT 10

#define MSG1(x_) if ( en96_debugLevel >= 1 ) MSGD(x_)
#define MSG2(x_) if ( en96_debugLevel >= 2 ) MSGD(x_)
#define MSG3(x_) if ( en96_debugLevel >= 3 ) MSGD(x_)
#define MSG4(x_) if ( en96_debugLevel >= 4 ) MSGD(x_)
#define MSG5(x_) if ( en96_debugLevel >= 5 ) MSGD(x_)
#define MSG6(x_) if ( en96_debugLevel >= 6 ) MSGD(x_)
#define MSG7(x_) if ( en96_debugLevel >= 7 ) MSGD(x_)
#define MSG8(x_) if ( en96_debugLevel >= 8 ) MSGD(x_)
#define MSG9(x_) if ( en96_debugLevel >= 9 ) MSGD(x_)

#define DEFAULT_XSERVER_PROT_SIZE (64*1024)

/* pathnames added to sql41_get_spool_dir() */
char  *         PROTFILE                = "xserver.prt";
char  *         UPDATEFIFOFILE          = "xserver_update";
char  *         PROTOCOLFIFOFILE        = "xserver_logger";
int             en96_debugLevel = 0;

#if defined(FREEBSD)
#define RLIMIT_AS RLIMIT_VMEM
#endif

#define XSERVER_STOP_REQUEST_REJECTED "Stop request rejected"

enum vserver_files { vs_protfile=0,
                     vs_updatepipe=1,
                     vs_loggerpipe=2,
                     vs_nitrace=3};

static char const *vs_filesuffixes[] = { ".prt", "_update", "_logger", ".nitrace" };

static SAPDB_Bool BuildVserverFilename( enum vserver_files which,
                                        SAPDB_Bool oldForm,
                                        SAPDB_Char const *portOrServiceSpecified,
                                        tsp00_Pathc pathToFill,
                                        tsp01_RteError *pRteError);
#if defined(SVR4) || defined(OSF1) || defined(LINUX) || defined(SOLARIS)
# include  <wait.h>
#endif

/* exported functions */

/* imported functions */

# ifdef DEBUG_DEBUG
static char hexbuffer[64] = { 0 };
# endif /* DEBUG_DEBUG */

/* local functions */

/*!
  @brief Stop the running server

  The job is to stop the running server and all its children.
 */
static  int                             en96_StopServer ();

/*!
  @brief Change debug level of the running server

  The job is to modify the debug level of the running server and all its subsequent forked children.
 */
static  int                             en96_ChangeDebugLevel ( int newDebugLevel );
static  int                             en96_GetServerAddress ();
static  int                             en96_ServerLoop ();
static  int                             en96_Connect ();
static  int                             en96_transfer ();
static  int                             e96_info_request ( rte_header    *header );
static  void                            e96_set_signals ( SIGNALFUNCTYPE    (*func)() );
static  int                             en96_deadSocketCheck(void *arg); /* PTS 1114659 */
static  SIGNALFUNCTYPE                  en96_deadCheck ( int sig ); /* PTS 1114659 */
static  SIGNALFUNCTYPE                  e96_catch_alarm ( int sig );
static  SIGNALFUNCTYPE                  e96_catch_signal ( int sig );

#define WAIT_FOR_CHILD
#ifdef WAIT_FOR_CHILD
 static  SIGNALFUNCTYPE                 e96_catch_sigcld ( int sig );
#endif

static  int                             e96_close_on_exec ( int fd );
static  int                             e96_SetSigAction(int sig, void (*handler)(int) );
static  int                             en96_ActionMatch(const char *action, char *argument);
static void                             en96_Usage(int longForm);

/* exported data */


/* imported data */

extern  int                             sql22_timestamp;
extern  char                            *sql22_file;
extern  int                             sql22_fd;
extern  int                             sql22_keep_open;

/* local data */

static  pid_t                           en96_masterPid = (pid_t)0;
static  pid_t                           en96_pid       = (pid_t)0;

typedef struct xserverConnection_
{
	char           *serviceName;
	unsigned short  port;
    struct sockaddr sa;
	int             sd;
} xserverConnection;

/*!
  @brief buffer used to transfer update command information
 */
struct updateCommandBuffer_ {
    int totalLength;
    int replyTag;
    int argCount;
    char commandString[4];
};

static xserverConnection en96_ListenSocketInfo = {
	SOCKET_SERVICE_SQL6_EO003,
	0
};
static  SAPDB_Int8 en96_AcceptCount = 0;
static  char en96_AcceptId[32] = { 0 };
static  connection_info                 *e96_cip;
static  connection_info                 e96_connection;
static  SQL_DBNAMEC                     e96_remote_db;

static  int                             e96_remote_ref;
static  int                             e96_remote_swap;
/* PTS 1109405 now global for PING request */
static  tsp00_Versionc                  en96_RTEVersionString;
static  void                            en96_ShowEnviron(int argc, char *argv[],
                                                         char *envp[],
                                                         SAPDB_Int4 callerUid,
                                                         SAPDB_Int4 callerGid);
static  char *                          en96_service_or_port_specified = 0;
static  int                             en96_takeoverSocket = UNDEF;
static  char                            en96_niserverPath[sizeof(tsp00_Pathc)+16 /* strlen("/niserver") */];
static  char                            en96_nisslserverPath[sizeof(tsp00_Pathc)+16 /* strlen("/nisslserver") */];
static  int                             en96_suppressAutomaticNIServerHandling = 0;
static  int                             en96_forceOnlyNIServerHandling = 0;
static  int                             en96_suppressAutomaticNISSLServerHandling = 0;
static  int                             en96_forceOnlyNISSLServerHandling = 0;

static  tsp00_Pathc                     en96_oldProtocolPipePath;
static  tsp00_Pathc                     en96_protocolPipePath;
static  int                             en96_protocolPipe = UNDEF;

static  tsp00_Pathc                     en96_oldUpdateFifoPath;
static  tsp00_Pathc                     en96_updateFifoPath;
static  int                             en96_updateFifo = UNDEF;

static  tsp00_Pathc                     en96_oldUpdateFifoReplyPath;
static  tsp00_Pathc                     en96_updateFifoReplyPath;
static  int                             en96_updateFifoReplyTag = 0;

static  tsp00_Pathc                     en96_oldProtfile;
static  tsp00_Pathc                     en96_protfile;

static  SAPDB_Char                      en96_processIdString[40];
static  int                             en96_fastModeSelected = 0; /* if selected DNS lookup is suppressed (default off) */
static  int                             en96_foundRunningVersion = false;
static  tsp100_VersionID                en96_runningVersionId;

/*!
  @brief create pathes for x_niserver and x_nisslserver calls
  @return none
 */
static  void                            en96_CreateHelpServerPathes();

#define NISERVER_SHOW_VERSION_EN96 1
#define NISERVER_START_EN96 2
#define NISERVER_UNLISTEN_EN96 3
#define NISERVER_STOP_EN96 4
#define NISSLSERVER_SHOW_VERSION_EN96 5
#define NISSLSERVER_START_EN96 6
#define NISSLSERVER_UNLISTEN_EN96 7
#define NISSLSERVER_STOP_EN96 8
/*!
  @brief call a help server (x_niserver or x_nisslserver) to do start,stop,unlisten or show version
  @param what [in] selected action
  @return exit code of help server or -1 on error
 */
static  int                             en96_CallHelpServer(int what);


/*!
  @brief create path to niserver, protocol file, protocol pipe and update pipe and change working directory
  @return 0 if c all succeeded, != 0 if call failed (error message already printed)
 */
static  int                             en96_CreatePathesAndChangeDirectory();

/*!
  @brief use ping request to get running x_servers version and compare against own version

  The routine is used either to check against own version or to verify update succeeded (pIsNewer = 0)
  @param pIsNewer [inout] return result of comparison (true if running version is newer)
  @param pIsAlreadyStarted [inout] filled with result of comparison (true if running version is identical)
  @return 0 if call succeeded, != 0 if call failed (error message already printed)
 */
static int en96_CheckRunningVersion( int *pIsNewer, int *pIsAlreadyStarted );

/*!
  @brief inform the running x_server via its update pipe and wait for its response

  A update reply pipe is created and its 'tag' is written into the update pipe together with the 'update command line'.
  As response the x_server process id followed by a single space is expected (i.e. '12345 ') in the update reply pipe.
  The process id is used to wait until the running x_server process terminates.

  @param argc [in] argument count
  @param argv [in] argument vector needed to create update command line
  @return 0 if call succeeded, != 0 if call failed (error message already printed)
 */
static int en96_TryToUpdate(int argc, char *argv[]);

/*!
  @brief write given buffer into update fifo
  @param commandBuffer [in] the command buffer to write
  @param totalLength [in] total length in bytes
  @return 0 if call succeeded, != 0 if call failed (error message already printed)
 */
static int en96_WriteCommandToUpdateFifo(char *commandBuffer, int totalLength);

/*!
  @brief write the update command line

  The update pipe is filled with the information needed to fork and exec the new x_server.
  The information consists of total length, reply fifo tag, DBROOT for new vserver, command line arguments.
  The latter arguments are separated by zeros.

  @param argc [in] argument count
  @param argv [in] argument vector needed to create update command line
  @return 0 if call succeeded, != 0 if call failed (error message already printed)
 */
static int en96_WriteUpdateCallToUpdateFifo(int argc, char *argv[]);

/*!
  @brief check the signaled input on update pipe

  The running x_server got input on the update pipe. The input is checked to be 'update command information' and as response
  the update reply pipe is opended and the own process id followed by a space is written to that pipe. The caller is expected
  to fork itself and exec the new image specified in the update command. After that, the main server loop is terminated.
  The listener socket is passed to the new x_server as command line option. The update socket descriptor is the global descriptor
  en96_updateFifo, which was opened in en96_StartToListen.

  @return 0 if no valid update request is received, != 0 if update request was received, process forked and response was written
 */
static int en96_CheckForUpdateRequest();

/*!
  @brief open and bind the socket to listen to
  @param isRunning [out] set != 0 if port is already in use
  @return 0 if call succeeded, != 0 if call failed
 */
static int en96_OpenListenSocket ( int *isRunning );

/*!
  @brief start to listen on socket

  @return 0 if call succeeded, != 0 if call failed
 */
static int en96_StartToListen ();

/*!
  @brief If given extract callerUid, callerGid and administrator information from given arguments. Else
         determine those informations from the process.
  @return 
 */

static void en96_GetCallerInformation(int *         argc,
                                      char *        argv[],
                                      SAPDB_Int4 *  callerUid,
                                      SAPDB_Int4 *  callerGid,
                                      SAPDB_Bool *  isAdministrator);


/*==========================================================================*/
/* x_server main                                                             */
/*==========================================================================*/


int c_main_with_envp (
                      int  argc ,
                      char *argv[],
                      char *envp[] )
{
# undef  MF__
# define  MF__  MOD__"en96_main"
    tsp01_RteError         RteError;
    int                    lgt , tfd [ 2 ];
    char                   line [ 256 ];
    int                    fd;
    int                    argn , rc;
    RTE_FileOffset         en96_max_file_size = DEFAULT_XSERVER_PROT_SIZE;
    int                    killActionSelected = 0;
    int                    updateActionSelected = 0;
    int                    takeoverActionSelected = 0;
    int                    isInteractive = 0;
    int                    newDebugLevel = UNDEF;
    int                    showVersionInfoOnly = 0;
    int                    isRunning = 0; /* Set to true, if running server is found - checked in OpenListenSocket() */
    int                    isAlreadyStarted = 0; /* Set to true, if running server with identical version is found */
    SAPDB_Int4             callerUid = -1;
    SAPDB_Int4             callerGid = -1;
    SAPDB_Bool             isAdministrator = false;
    SAPDB_Bool             environmentAdjusted = false;

    /* Starting vserver by using the sdbstarter is not necessary here because the x_server was already started      */
    /* by the sdbstarter. So the only reason for calling 'RTE_CallSdbStarter' is to parse from the given arguments  */
    /* the callerUid, callerGid and the information if it was called by a member of the sdb administrator group.    */
    en96_GetCallerInformation (&argc, argv, &callerUid, &callerGid, &isAdministrator);

    /* Server input may be piped,               */
    /* so close stdin to free the process(es)   */
    /* that write to the pipe.                  */
    /* Reopen stdin, so file descriptors 0      */
    /* will not be used for network operations. */
    /* The above is important for both the      */
    /* starter and the background server.       */

    /* redirect stdin to the null device */
    (void) close ( 0 );
    (void) open ( "/dev/null", O_RDONLY , 0 );

    /* reenable redirection of output. This coding inform 'MSGD' output to write on stderr and keep it open */
    sql22_fd = 2;
    sql22_keep_open = 1;

    /*
     *  File permission mode is always specified absolutely
     *  within the server.
     */
    (void) umask ( 007 );

    sp100_GetVersionString(COMP_NAME_RTE_SP100, s100buildnumber, en96_RTEVersionString);

    /* parse arguments */
    if ( argc > 1 )
    {
        int o_char ;
        do {
            o_char = sqlgetopt ( argc, argv, "ac:ikFVWwXYyUD:N:S:Z:T:" );

            if ( UNDEF == o_char )
            {
                if ( optind < argc )
                {
                    o_char = 0;
                    if ( en96_ActionMatch("START", argv[optind]) )
                    {
                        /* default ignored */
                        optind++;
                    } else if ( en96_ActionMatch("STOP", argv[optind]) )
                    {
                        killActionSelected = 1;
                        optind++;
                    } else if ( en96_ActionMatch("UPDATE", argv[optind]) )
                    {
                        updateActionSelected = 1;
                        optind++;
                    }
                    else
                    {
                        printf( "Invalid argument %d '%s'\n", optind, argv[optind] );
                        en96_Usage(0);
                        return 1;
                    }
                }
            }
            else
            {

                switch ( o_char )
			    {
                case 'k' :
                    killActionSelected = 1;
                    break;

                case 'F' :
                    en96_fastModeSelected = 1;
                    break;

                case 'i' :
                    isInteractive = 1;
                    break;

                case 'V' :
                    showVersionInfoOnly = 1;
                    break;

                case 'X' : /* ignore old obsolete 'X' option which was used to suppress < 6.x connections */
                    break;

                case 'Y' :
                    en96_suppressAutomaticNIServerHandling = 1;
                    en96_suppressAutomaticNISSLServerHandling = 1;
                    break;

                case 'y':
                    en96_forceOnlyNIServerHandling = 1;
                    break;

                case 'W' :
                    en96_suppressAutomaticNISSLServerHandling = 1;
                    break;

                case 'w' :
                    en96_forceOnlyNISSLServerHandling = 1;
                    break;

                case 'D' :
                    if ( isdigit(optarg[0]) )
                    {
                        en96_debugLevel = optarg[0] - '0';
                    }
                    else
                    {
                        en96_debugLevel = 5;
                    }
                    break;

                case 'N' :
                    if ( isdigit(optarg[0]) )
                    {
                        newDebugLevel = optarg[0] - '0';
                    }
                    else
                    {
                        newDebugLevel = 5;
                    }
                    break;

                case 'S':
                    en96_service_or_port_specified = strdup(optarg);
                    break;

                case 'T':
                    if ( optarg )
                    {
                        en96_takeoverSocket = atoi ( optarg );
                        if ( 0 == en96_takeoverSocket )
                        {
                            en96_takeoverSocket = UNDEF;
                        }
                        else
                        {
                            if ( strchr(optarg, ',') )
                            {
                                en96_protocolPipe = atoi(strchr(optarg, ',')+1);
                                if ( 0 == en96_protocolPipe )
                                {
                                    en96_takeoverSocket = UNDEF;
                                    en96_protocolPipe   = UNDEF;
                                }

                            }
                        }
                    }
                    break;

                case 'Z':
                    en96_max_file_size = atoi ( optarg ) * 1024;
                    break;

                case 'U':
                    updateActionSelected = 1;
                    break;

                case 'H':
                case 'h':
                case '?':
                    en96_Usage(1);
                    return 1;

                default:
                    if ( 0 != o_char )
                    {
                        printf( "Invalid argument %d unknown option '%c'\n", optind, o_char );
                        en96_Usage(0);
                        return 1;
                    }
                    break;
                }
            }
        } while ( UNDEF != o_char );
    }

    /* Combination check... */
    if ( en96_forceOnlyNIServerHandling && en96_suppressAutomaticNIServerHandling )
    {
        printf( "You cannot enforce (-y) and suppress (-Y) niserver handling in one command.\n" );
        en96_Usage(0);
        return 1;
    }

    if ( en96_forceOnlyNISSLServerHandling && en96_suppressAutomaticNISSLServerHandling )
    {
        printf( "You cannot enforce (-w) and suppress (-W opr -Y) nisslserver handling in one command.\n" );
        en96_Usage(0);
        return 1;
    }

    if ( en96_forceOnlyNIServerHandling && en96_forceOnlyNISSLServerHandling )
    {
        printf( "You cannot enforce (-w) nisslserver or enforce (-y niserver handling in one command.\n" );
        en96_Usage(0);
        return 1;
    }

    /*
     *  Close all files except stdout, stderr and (if specifed) the socket to take over.
     */
    for ( fd = 3;fd < sysconf ( _SC_OPEN_MAX ); fd ++ )
    {
        if ( fd != en96_takeoverSocket
          && fd != en96_protocolPipe )
        {
            (void) close ( fd );
        }
    }


    /* Expand LibPath environment to find the shared objects needed by NI, UNICODE, and SSL */

    /* For normal vserver start this could also be done by the startup program x_server (as was done before).    */
    /* But during vserver update the new vserver is called by 'execv' from the running vserver.                  */
    /* But unfortunately the vserver from version <= 7.5 does not need those shared objects. Therefore the       */
    /* vserver from version <= 7.5 does not expand the LibPath environment. This was a problem when this vserver */
    /* calls a vserver from version > 7.5 because those vserver need an extension of the LibPath environment     */
    /* to find the shared objects mentioned above.                                                               */
    /* Since some UNIX platforms need to know the LibPath from the moment of starting the executable we call     */
    /* always 'execv' here to restart the process with the adjusted environment.                                 */
    if (sqlUpdateNiLibPathEnvironment(&environmentAdjusted, &RteError))
    {
        if (environmentAdjusted)
        {
            if ( -1 == RTE_save_execv(argv[0], argv))
            {
                fprintf(stderr, "Failed to execv %s: error code %d:%s\n", argv[0], errno, strerror(errno));
                return 2;
            }
        }
    }
    else
    {
        MSGD (( ERR_XSERVER_UPDATE_LIBPATH, RteError.RteErrText));
        return 2; 
    }


    /* en96_CallHelpServer depends on it... */
    en96_CreateHelpServerPathes();

    /* Show version action is handled immediatly */
    if ( showVersionInfoOnly )
    {
        if ( en96_forceOnlyNIServerHandling )
        {
            en96_CallHelpServer(NISERVER_SHOW_VERSION_EN96);
        }
        else if ( en96_forceOnlyNISSLServerHandling )
        {
            en96_CallHelpServer(NISSLSERVER_SHOW_VERSION_EN96);
        }
        else
        {
            printf("%s (vserver)\n" , en96_RTEVersionString );
            if ( !en96_suppressAutomaticNIServerHandling )
            {
                en96_CallHelpServer(NISERVER_SHOW_VERSION_EN96);
            }
            if ( !en96_suppressAutomaticNISSLServerHandling )
            {
                en96_CallHelpServer(NISSLSERVER_SHOW_VERSION_EN96);
            }
        }
        return ( 0 );
    }

    /* administrator check is handled immediatly */
    if ( killActionSelected || updateActionSelected )
    {
        if ( !isAdministrator )
        {
            MSGD (( ERR_XSERVER_NOT_ADMINISTRATOR ));
            return ( 2 );
        }
    }

    /* Create path names for executables , pipes and protocol files */
    if ( en96_CreatePathesAndChangeDirectory() )
    {
        return (2);
    }

    /* Change debug level action cannot be handled correctly */
    if ( newDebugLevel >= 0 )
    {
        if ( 0 != en96_ChangeDebugLevel ( newDebugLevel ) )
        {
            MSGD (( WRN_XSERVER_NO_DEBUGLEVEL_CHANGE ));
            return ( 2 );
        }
        MSGD(( INFO_XSERVER_NEW_DEBUG_LEVEL, newDebugLevel ));
        return ( 0 );
    }

    /* Force starting of NISERVER only */
    if ( en96_forceOnlyNIServerHandling )
    {
        if ( killActionSelected )
        {
            en96_CallHelpServer(NISERVER_STOP_EN96);
            return ( 0 );
        }

        rc = en96_CallHelpServer(NISERVER_START_EN96);
        if ( rc != 0 )
        {
            MSGD (( WRN_XSERVER_NI_INIT, "Start niserver failed" ));
        }
        return rc;
    }

    /* Force starting of NISSLSERVER only */
    if ( en96_forceOnlyNISSLServerHandling )
    {
        if ( killActionSelected )
        {
            en96_CallHelpServer(NISSLSERVER_STOP_EN96);
            return ( 0 );
        }

        rc = en96_CallHelpServer(NISSLSERVER_START_EN96);
        if ( rc != 0 )
        {
            MSGD (( WRN_XSERVER_NISSL_INIT, "Start nisslserver failed" ));
        }

        return rc;
    }

    e96_cip      = &e96_connection;

    if ( en96_OpenListenSocket ( &isRunning ) )
    {
        return (3);
    }

    if ( isRunning )
    {
        int isNewer = 0;

        if ( killActionSelected )
        {
            if ( 0 != en96_StopServer () )
            {
                MSGD (( WRN_XSERVER_NO_KILL ));
                return ( 2 );
            }
            return 0;
        }

        /* isNewer is not set if only the build number differs... */
        if ( en96_CheckRunningVersion( &isNewer, &isAlreadyStarted) )
        {
            return (3);
        }

        en96_foundRunningVersion = true;
        /* If a newer version is found, x_server is not allowed to start or update */
        if ( isNewer )
        {
            MSGD (( INFO_XSERVER_NEWER_VERSION1, en96_RTEVersionString ));
            MSGD (( INFO_XSERVER_NEWER_VERSION2 ));
            return (0);
        }
    }
    else if ( killActionSelected )
    {
        return ( 0 );
    }


    if ( isRunning )
    {
        if ( updateActionSelected )
        {
            /* Server is running, so try to update by allowing running server to terminate */
            if ( en96_TryToUpdate(argc, argv) )
            {
                return (2);
            }

            if ( en96_CheckRunningVersion( (int*)0, (int*)0) )
            {
                return (2);
            }
            return (0);
        }
        else
        {
            if ( isAlreadyStarted )
            {
                if ( en96_service_or_port_specified )
                {
                    MSGD (( INFO_XSERVER_ON_PORT_STARTED_VERSION , en96_service_or_port_specified, "already..." ));
                }
                else
                {
                    MSGD (( INFO_XSERVER_STARTED_VERSION , "already..." ));
                }
                return (0);
            }

            MSGD (( ERR_XSERVER_ALREADY_RUNNING ));
            return (4);
        }
    }
    else
    {
        unlink(en96_oldUpdateFifoPath);
        unlink(en96_oldProtocolPipePath);
        unlink(en96_oldProtfile);

#ifndef FREEBSD
        if ( UNDEF == mknod ( en96_protocolPipePath, S_IFIFO | 0660, 0 )
          && EEXIST != errno )
#else
        if ( mkfifo ( en96_protocolPipePath, 0660 ) )
#endif
        {
            MSGD (( ERR_XSERVER_CREATE_NAMED_PIPE, en96_protocolPipePath, sqlerrs() ));
            return (2);
        }

        if ( access(en96_protocolPipePath, R_OK|W_OK) != 0 )
        {
            MSGD (( ERR_XSERVER_CREATE_NAMED_PIPE, en96_protocolPipePath, sqlerrs() ));
            return (2);
        }

    }

    if ( en96_StartToListen() )
    {
        return (4);
    }

    if ( 0 != en96_CallHelpServer(NISERVER_START_EN96) )
    {
        MSGD (( WRN_XSERVER_NI_INIT, "Automatic niserver start failed" ));
    }

    if ( 0 != en96_CallHelpServer(NISSLSERVER_START_EN96) )
    {
        MSGD (( WRN_XSERVER_NISSL_INIT, "Automatic nisslserver start failed" ));
    }

    if ( isInteractive )
    {
        e96_set_signals ( SIG_DFL );
        if ( en96_service_or_port_specified )
        {
            MSGD (( INFO_XSERVER_ON_PORT_STARTED_VERSION , en96_service_or_port_specified, en96_RTEVersionString ));
        }
        else
        {
            MSGD (( INFO_XSERVER_STARTED_VERSION , en96_RTEVersionString ));
        }
        en96_ShowEnviron(argc, argv, envp, callerUid, callerGid);
    }
    else
    {
        e96_set_signals ( SIG_IGN );

        if ( UNDEF == en96_takeoverSocket )
        {
            rc = pipe ( tfd );
            if ( rc )
            {
                MSGD (( ERR_XSERVER_CREATE_PIPE, sqlerrs() ));
                return ( 1 );
            }
            /* create background server */
            rc = RTE_save_fork ();
            if ( UNDEF == rc )
            {
                MSGD (( ERR_XSERVER_OPEN_PROCESS , sqlerrs() ));
                return ( 1 );
            }
            if ( rc > 0 )
            {
                /* starter process exits when pipe becomes closed */
                (void) close ( tfd [ 1 ] ); /* don't write to pipe */
                for (;; )
                {
                    lgt = read ( tfd [ 0 ] , line , sizeof(line) );
                    if ( lgt <= 0 ) break;
                    (void) write ( 2, line , (unsigned) lgt );
                }
                return ( 0 );
            }

            /* background server */

            (void) close ( tfd[0] ); /* don't read from pipe */
            tfd[0] = UNDEF;

            /* Server output may be piped,                       */
            /* so close stdout/stderr to free the process(es)    */
            /* that read from the pipe(s).                       */
            /* Reopen stdout/stderr, so file descriptors 1 and 2 */
            /* will not be used for network operations.          */

            if ( en96_service_or_port_specified )
            {
                MSGD (( INFO_XSERVER_ON_PORT_STARTED_VERSION , en96_service_or_port_specified, en96_RTEVersionString ));
            }
            else
            {
                MSGD (( INFO_XSERVER_STARTED_VERSION , en96_RTEVersionString ));
            }

            /* redirect stdout to the pipe */
            (void) close ( 1 );
            (void) dup ( tfd[1] );

            /* redirect stderr to the pipe */
            (void) close ( 2 );
            (void) dup ( tfd[1] );

            en96_pid = getpid ();
            errno = 0;

            if ( setpgid (0, 0) != 0 ) /* PTS 1109220 */
            {
                MSGD (( ERR_XSERVER_PROCESS_GROUP, sqlerrs() ));
                return ( 3 );
            }
            else
            {
                en96_masterPid = en96_pid; /* PTS 1109220 */
            }

            MSG1 (( INFO_XSERVER_REDIRECT_OUTPUT, en96_protfile ));
        }
        else
        {
            en96_pid = getpid ();
            en96_masterPid = en96_pid;
        }

        if ( UNDEF == en96_protocolPipe )
        {
            (void) unlink ( en96_protfile );

            /* PTS 1109704 */
            fd  = open ( en96_protfile , O_WRONLY | O_CREAT | O_TRUNC , 0644 );
            if ( fd < 0 )
            {
                MSGD (( ERR_XSERVER_REDIRECT_OUTPUT, en96_protfile, sqlerrs() ));
                return ( 4 );
            }
        }
        else
        {
            fd = en96_protocolPipe;
        }

        dup2(fd, 1);
        dup2(fd, 2);

        if ( UNDEF == en96_takeoverSocket )
        {
            /* closing the pipe lets the starter exit */
            (void) close ( tfd[1] );
            tfd[1] = UNDEF;
        }

        sql22_file = en96_protocolPipePath;

        /* redirection of output to protocol pipe. This coding inform 'MSGD' output to write on protocol pipe
           including time stamp information and keep it open */
        sql22_timestamp = 1;
        sql22_keep_open = 1;
        sql22_fd = fd;

        if ( UNDEF == en96_protocolPipe )
        {
            fd = RTEDiag_XserverLogger(argv, fd, en96_max_file_size, en96_protocolPipePath);
            if ( UNDEF == fd ) /* PTS 1113240 */
            {
                MSGD (( ERR_XSERVER_REDIRECT_OUTPUT, en96_protfile, "logging process not started" ));
                return ( 4 );
            }
            en96_protocolPipe = fd;
        }

        dup2(fd, 1);
        dup2(fd, 2);
    }

    /* from this time on, any debug output will have a time stamp */
    sql22_timestamp = 1;

    if ( en96_service_or_port_specified )
    {
        MSGD (( INFO_XSERVER_ON_PORT_STARTED_VERSION , en96_service_or_port_specified, en96_RTEVersionString ));
    }
    else
    {
        MSGD (( INFO_XSERVER_STARTED_VERSION , en96_RTEVersionString ));
    }

    MSGD (( INFO_XSERVER_SERVICE_PORT , en96_ListenSocketInfo.port ));

    en96_ShowEnviron(argc, argv, envp, callerUid, callerGid);

    if (!en96_suppressAutomaticNISSLServerHandling)
    {   
        /* NI support is necessary for the SSL utilities. */
        /* The SSL utilities are needed to determine the server certificate */ 
        tsp00_ErrTextc  pErrText;
        tsp00_Pathc     niTraceFileName = "";

        if ( !BuildVserverFilename( vs_nitrace,
            false,
            en96_service_or_port_specified,
            niTraceFileName,
            &RteError) )
        {
            MSGD (( ERR_XSERVER_NI_TRACEFILE , RteError.RteErrText));
            niTraceFileName [0] = '\0';
        }

        if (commErrOk_esp01 == eo40NiInit(niTraceFileName, 0, pErrText))
        {
            /* Try to initialize the SSL utilities */
            if (commErrOk_esp01 != RTESec_SSLUtilInit (niTraceFileName, pErrText ))
            {
                MSGD (( WRN_XSERVER_NISSL_UTIL_INIT, pErrText));
            }
        }
        else
        {
            tsp00_ErrTextc  tmpErrText;
            eo40NiFinish (tmpErrText);
            MSGD (( WRN_XSERVER_NI_INIT, pErrText ));
        }
    }

    rc = en96_ServerLoop ( );

    if ( e96_cip->ci_sd > 2 )
    {
        (void) shutdown ( e96_cip->ci_sd , 2 );
        (void) close ( e96_cip->ci_sd );
    }
    (void) close ( en96_ListenSocketInfo.sd );
    en96_ListenSocketInfo.sd = UNDEF;

    return ( rc );
}

/*==========================================================================*/

static  int     en96_StopServer ()
{
#undef  MF__
#define  MF__  MOD__"en96_StopServer"

    int        rc = 0;
    int        retry = 0;
    tsp00_ErrTextc pErrText;
    union {
	    rte_header *header;
    } u;
    char               packet [ RTE_CONPKT_SIZE ];
    int updateFifo;
    struct updateCommandBuffer_ stopCommand;

    DBGIN;

    if ( 0 != en96_GetServerAddress () )
    {
        return ( 2 );
    }

    if ( 0 != sql43_get_host_by_name ( "localhost",
                                       ((unsigned char *) (en96_ListenSocketInfo.sa.sa_data)) + 2 ,
                                       sizeof(en96_ListenSocketInfo.sa.sa_data) - 2 ) )
    {
        MSGD(( ERR_GET_HOST_BY_NAME, "localhost" ));
        return ( 2 );
    }

    /* create socket */
    en96_ListenSocketInfo.sd = socket ( AF_INET , SOCK_STREAM , 0 );

    if ( UNDEF == en96_ListenSocketInfo.sd )
    {
        rc = errno;
        DBG1 (( MF__,": listen socket open error, %s" , sqlerrs() ));
        return ( 2 );
    }
    MSG1(( INFO_XSERVER_OPEN_SOCKET, en96_ListenSocketInfo.sd ));

    rc = connect ( en96_ListenSocketInfo.sd , &(en96_ListenSocketInfo.sa), sizeof(en96_ListenSocketInfo.sa) );

    if ( UNDEF == rc )
    {
        if ( errno == ECONNREFUSED )
        {
            (void) close ( en96_ListenSocketInfo.sd );
            en96_ListenSocketInfo.sd = UNDEF;
            if ( en96_service_or_port_specified )
            {
                MSGD (( INFO_XSERVER_ON_PORT_NOT_RUNNING, en96_service_or_port_specified ));
            }
            else
            {
                MSGD (( INFO_XSERVER_NOT_RUNNING ));
            }
            return ( 0 );
        }
        MSGD(( ERR_REQ_SOCKET_CONNECT, errno ));
        (void) close ( en96_ListenSocketInfo.sd );
        en96_ListenSocketInfo.sd = UNDEF;
        return ( 2 );
    }

    /* try stop via special request */

    u.header = (rte_header*) packet;
    sql42_create_conpkt ( u.header , 0 ,
                      (tsp00_Int4) en96_ListenSocketInfo.port , (tsp00_Int4) en96_ListenSocketInfo.port ,
                      (int) en96_ListenSocketInfo.port , 0 , (long) RTE_CONPKT_SIZE ,
                      0L , 0L , 0L , "" , "" );
    rc = sql42_send_conpkt ( en96_ListenSocketInfo.sd , u.header , pErrText );
    if ( rc )
    {
        MSGD(( ERR_XSERVER_SEND_PACKET, en96_ListenSocketInfo.port, pErrText ));
    }
    else 
    {
        rc = read ( en96_ListenSocketInfo.sd , packet , RTE_CONPKT_SIZE );
        if ( UNDEF == rc )
        {
            MSGD(( ERR_XSERVER_RECV_PACKET, en96_ListenSocketInfo.port, sqlerrs() ));
            rc =  2;
        }
        else
        {
            if ( rc == 0 )
            {
                if ( en96_service_or_port_specified )
                {
                    MSGD (( INFO_XSERVER_ON_PORT_STOPPED , en96_service_or_port_specified ));
                }
                else
                {
                    MSGD (( INFO_XSERVER_STOPPED ));
                }
            }
            else
            {
                if ( rc < 0
                  || packet[0] != rc
                  || rc != (strlen(&packet[1])+2) )
                {
                    packet[0] = 0;
                }
                rc = 2;
            }
        }
    }

    (void) close ( en96_ListenSocketInfo.sd );
    en96_ListenSocketInfo.sd = UNDEF;

    if ( rc == 0 )
    {
        return ( 0 );
    }

    /* Stop request was recognized, but rejected! This indicates stop via update pipe is possible! */

    if ( packet[0] == (strlen(XSERVER_STOP_REQUEST_REJECTED)+2)
      && 0 == strcmp(&packet[1], XSERVER_STOP_REQUEST_REJECTED) )
    {
        packet[0] = 0;
        /* allowed to try to stop via update pipe */

        stopCommand.totalLength = sizeof(struct updateCommandBuffer_);
        memcpy(stopCommand.commandString, "STP", 4);
        if ( en96_WriteCommandToUpdateFifo((char *)&stopCommand, sizeof(struct updateCommandBuffer_)) != 0 )
        {
            return ( 2 );
        }

        while ( retry < 3 )
        {
            updateFifo = open(en96_updateFifoPath, O_WRONLY|O_NONBLOCK);
            /* running server will remove this pipe before exiting... */
            if ( UNDEF == updateFifo )
            {
                if ( en96_service_or_port_specified )
                {
                    MSGD (( INFO_XSERVER_ON_PORT_STOPPED , en96_service_or_port_specified ));
                }
                else
                {
                    MSGD (( INFO_XSERVER_STOPPED ));
                }
                return ( 0 );
            }
            else
            {
                close(updateFifo);
            }

            sleep(1 + retry);
            ++retry;
        }
    }

    if ( packet[0] != 0 )
    {
        MSGD(( ERR_XSERVER_SEND_PACKET, en96_ListenSocketInfo.port, &packet[1] ));
    }

    DBGOUT;
    return ( 1 );
}

/*==========================================================================*/

static  int     en96_ChangeDebugLevel ( int newDebugLevel )
{
#undef  MF__
#define  MF__  MOD__"en96_ChangeDebugLevel"
    struct updateCommandBuffer_ debugCommand;

    debugCommand.totalLength = sizeof(struct updateCommandBuffer_);
    debugCommand.replyTag = newDebugLevel;
    memcpy(debugCommand.commandString, "DBG", 4);
    if ( en96_WriteCommandToUpdateFifo((char *)&debugCommand, sizeof(struct updateCommandBuffer_)) != 0 )
    {
        return ( 2 );
    }
    return ( 0 );
}

/*==========================================================================*/

static  int     en96_GetServerAddress ()
{
# undef  MF__
# define  MF__  MOD__"en96_GetServerAddress"
  int                rc;
  char              *pService;

  DBGIN;

  if ( en96_service_or_port_specified )
  {
      if ( en96_service_or_port_specified[0] >= '0' && en96_service_or_port_specified[0] <= '9' )
      {
          en96_ListenSocketInfo.port = atoi(en96_service_or_port_specified);
          pService = 0;
      }
      else
      {
          pService = en96_service_or_port_specified;
      }
  }
  else
  {
      pService = en96_ListenSocketInfo.serviceName;
  }

  if ( pService )
  {
      /* get port number for remote sql service */
      rc = sql43_get_service_by_name ( pService, &(en96_ListenSocketInfo.port) );
      if ( rc != 0 )
      {
          MSGD (( ERR_XSERVER_GET_SERVICE , pService ));
          return ( 2 );
      }
  }

  MSG2 (( INFO_XSERVER_SERVICE_PORT , en96_ListenSocketInfo.port ));

  /* host address is implicit, so only address family and */
  /* port number are to be determined                     */

  SAPDB_memset ( &(en96_ListenSocketInfo.sa) , 0 , sizeof(en96_ListenSocketInfo.sa) );
  en96_ListenSocketInfo.sa.sa_family = AF_INET;
  en96_ListenSocketInfo.sa.sa_data[0] = ( ((en96_ListenSocketInfo.port) >> 8) & 0xFF ); /* high byte */
  en96_ListenSocketInfo.sa.sa_data[1] = ( ((en96_ListenSocketInfo.port)     ) & 0xFF ); /* low  byte */

  DBGOUT;

  return ( 0 );
}

/*==========================================================================*/
static  int     en96_OpenListenSocket ( int *isRunning )
{
# undef MF__
# define MF__ MOD__"en96_OpenListenSocket"
    int rc;
    DBGIN;

    if ( 0 != en96_GetServerAddress () )
    {
        return (1);
    }

    if ( UNDEF != en96_takeoverSocket )
    {
        en96_ListenSocketInfo.sd = en96_takeoverSocket;
        *isRunning = 0;
        return (0);
    }

    /* create server socket */
    en96_ListenSocketInfo.sd = socket ( AF_INET , SOCK_STREAM , 0 );
    if ( UNDEF == en96_ListenSocketInfo.sd )
    {
        MSGD (( ERR_XSERVER_LISTEN_SOCKET_OPEN, sqlerrs() ));
        return ( 1 );
    }
    MSG2 (( INFO_XSERVER_LISTEN_ON_SOCKET, en96_ListenSocketInfo.sd ));

    /*
     *  Set the reuse-address-after-close option for the listening socket.
     */
    rc = sql42_socket_live ( en96_ListenSocketInfo.sd );
    if ( rc != SP1CE_OK )
    {
	    MSGD(( ERR_KEEP_ALIVE, rc ));
        (void) close ( en96_ListenSocketInfo.sd );
        en96_ListenSocketInfo.sd = UNDEF;
        return ( 1 );
    }

    /* bind the socket to the server address */
    rc = bind ( en96_ListenSocketInfo.sd, &(en96_ListenSocketInfo.sa), sizeof(en96_ListenSocketInfo.sa) );
    if ( UNDEF == rc )
    {
        if ( EADDRINUSE == errno )
        {
            /* found out that server was already listening on this port */
            *isRunning = 1;
        }
        else
        {
            MSGD(( ERR_XSERVER_BIND_ERROR, sqlerrs() ));
            (void) close ( en96_ListenSocketInfo.sd );
            en96_ListenSocketInfo.sd = UNDEF;
            return ( 1 );
        }
    }

    return ( 0 );
}
/*==========================================================================*/

static int en96_StartToListen ()
{
    int rc;

#ifndef FREEBSD
    if ( mknod ( en96_updateFifoPath, S_IFIFO | 0660, 0 ) < 0
      && EEXIST != errno )
#else
    /* Forget mknod on FreeBSD, use mkfifo ! */
    if ( mkfifo (en96_updateFifoPath , 0660))
#endif
    {
        MSGD (( ERR_XSERVER_CREATE_NAMED_PIPE, en96_updateFifoPath, sqlerrs() ));
        return ( 1 );
    }

    en96_updateFifo = open(en96_updateFifoPath, O_RDWR|O_NONBLOCK);
    if ( UNDEF == en96_updateFifo )
    {
        MSGD (( ERR_XSERVER_OPEN_UPDATE_FIFO, en96_updateFifoPath, sqlerrs() ));
    }
    else
    {
        e96_close_on_exec(en96_updateFifo);
    }

    /* recycling already opened socket */
    if ( UNDEF == en96_takeoverSocket )
    {
# ifndef SOMAXCONN
/* Maximum number of outstanding connect requests */
#  define  SOMAXCONN  5
# endif
        rc = listen ( en96_ListenSocketInfo.sd, SOMAXCONN );
        if ( rc )
        {
            rc = errno;
            MSGD (( ERR_XSERVER_LISTEN_ERROR, sqlerrs() ));
            (void) close ( en96_ListenSocketInfo.sd );
            en96_ListenSocketInfo.sd = UNDEF;
            return ( 1 );
        }
        /*
         *  Set the reuse-address-after-close option for the listening socket.
         */
        rc = sql42_socket_live ( en96_ListenSocketInfo.sd );
        if ( rc != SP1CE_OK )
        {
	        MSGD(( ERR_KEEP_ALIVE, rc ));
            (void) close ( en96_ListenSocketInfo.sd );
            en96_ListenSocketInfo.sd = UNDEF;
            return ( 1 );
        }
    }

    MSG2 (( INFO_XSERVER_LISTEN_ON_SOCKET, en96_ListenSocketInfo.sd ));

    DBGOUT;
    return ( 0 );
}

/*==========================================================================*/

static void en96_ReleaseZombies( )
{
  int last_errno = errno;

  /* Let all children die, that are already zombies... */

  while ( UNDEF == waitpid( (pid_t)UNDEF, (int *)0, WNOHANG ) )
    if ( errno != EINTR )
      break;
    errno = last_errno;
}

/*==========================================================================*/

static  int     en96_ServerLoop ( )
{
# undef  MF__
# define  MF__  MOD__"en96_ServerLoop"
    int             rc;
#if defined(HPUX)
    int             lgt;
#endif
#if defined(OSF1)
    unsigned long   lgt;
#endif
#if defined(AIX) || defined(LINUX) || defined(FREEBSD) || defined(SUN) || defined(SOLARIS)
    socklen_t       lgt;
#endif
    pid_t           cldpid;
    int             errcnt = 0;
    char            *ptr;
    int             sd;
    int             sdcount;
    struct pollfd   listenFd[2];
    DBGIN;

    listenFd[0].fd     = en96_ListenSocketInfo.sd;
    listenFd[0].events = POLLIN;
    listenFd[1].fd     = en96_updateFifo;
    listenFd[1].events = POLLIN;

    for (;;)
    {
        listenFd[0].revents = 0;
        listenFd[1].revents = 0;
        DBG1 (( MF__,"poll() ... " ));

        rc = poll( listenFd, 2, UNDEF );
        DBG1 (( MF__,"poll() rc %d ", rc ));

        en96_ReleaseZombies( );

        /*
         *  Check for poll errors.
         */
        if ( UNDEF == rc )
        {
            if ( EINTR == errno )
            {
                MSG9 (( INFO_XSERVER_INTERRUPTED, "poll" ));
                continue;
            }
            MSGD(( ERR_POLL_SYSCALL, 1, UNDEF, sqlerrs() ));
            return ( 2 );
        }

        if ( listenFd[1].revents != 0 )
        {
             /* if an update request was received, this will not return */
             if ( en96_CheckForUpdateRequest() )
             {
                 e96_cip->ci_sd = 0; /* do not touch previous accepted socket */
                 break; /* leave the socket loop */
             }
             else
             {
                 continue;
             }
        }

        if ( listenFd[0].revents != 0 )
        {
            sd = en96_ListenSocketInfo.sd;
        }
        else
        {
            MSGD(( ERR_POLL_SYSCALL, 1, UNDEF, "wrong descriptor" ));
            return 2;
        }

        SAPDB_memset( &e96_connection , 0 , sizeof(connection_info) );

        lgt = sizeof(e96_cip->ci_peer_sock_addr);

        MSG6(( INFO_XSERVER_ACCEPTING ));

        e96_cip->ci_sd = accept ( sd , &e96_cip->ci_peer_sock_addr , &lgt );

        if ( UNDEF == e96_cip->ci_sd )
        {
            if ( EINTR == errno )
            continue;
            errcnt ++;

            MSGD(( ERR_XSERVER_ACCEPT_ERROR, sqlerrs() ));

            if ( (errcnt > 9) || (errno == EBADF) )
                return ( 2 );
            else
                continue;
        }
        errcnt = 0;

	    ++en96_AcceptCount;
        /* needed in several error outputs, not just for next output line... */
        sp77sprintf(en96_AcceptId, sizeof(en96_AcceptId), "%qd", en96_AcceptCount);

        MSG1(( INFO_XSERVER_ACCEPT_CONNECTION, e96_cip->ci_sd, en96_AcceptId ));

        if ( !en96_fastModeSelected )
        {
            /* get the remote node name */
            rc = sql43_get_host_by_address ( (unsigned char*)
                                             e96_cip->ci_peer_sock_addr.sa_data+2 ,
                                             (char *)e96_cip->ci_peer_node ,
                                             sizeof(e96_cip->ci_peer_node) );
        }
        else
        {
            rc = 1; /* DNS lookup suppressed */
        }

        if ( rc )
        {
            ptr = (char*) e96_cip->ci_peer_sock_addr.sa_data+2;

            sp77sprintf ( e96_cip->ci_peer_node , sizeof(e96_cip->ci_peer_node), "%d.%d.%d.%d" ,
                          ptr[0]&0xff , ptr[1]&0xff , ptr[2]&0xff , ptr[3]&0xff );
        }

        MSG1(( INFO_XSERVER_ACCEPT_CLIENT_NODE, en96_AcceptId, e96_cip->ci_peer_node ));

        /*
         *  Set the keep-alive option for the new connection.
         *  Set the reuse-address-after-close option for the new connection.
         */
        rc = sql42_socket_live ( e96_cip->ci_sd );

        if ( rc != SP1CE_OK )
        {
            return ( rc );
        }

        cldpid = RTE_save_fork ();

        if ( UNDEF == cldpid )
        {
            MSGD(( ERR_XSERVER_OPEN_PROCESS, sqlerrs() ));
        }
	    else if ( 0 == cldpid )
        {
            int fd;
            en96_pid = getpid ();
            (void) close ( en96_updateFifo );     /* child process does not react on update fifo */
            en96_updateFifo = UNDEF;
            (void) close ( en96_ListenSocketInfo.sd ); /* child process does not react on listener socket */
	        en96_ListenSocketInfo.sd = UNDEF;

            do
            {
                rc = en96_Connect ();
            } while (( rc                     == SP1CE_OK ) &&
                     ( e96_cip->ci_mess_class == RSQL_INFO_REQUEST_KEEP_ALIVE_EO003 ));

            if (    ( rc == SP1CE_OK )
               && ((e96_cip->ci_mess_class == RSQL_USER_CONN_REQUEST_EO003)
               ||  (e96_cip->ci_mess_class == RSQL_KERN_CONN_REQUEST_EO003)
               ||  (e96_cip->ci_mess_class == RSQL_CTRL_CONN_REQUEST_EO003)))
            {
                rc = en96_transfer ();
            }
            else
            {
                char packet [ RTE_CONPKT_SIZE ];
                SAPDB_memset(packet, 0, sizeof(RTE_CONPKT_SIZE) );
                if ( rc != SP1CE_OK )
                {
                    if ( rc == SP1CE_CRASH )
                    {
                        /* instead of a crash, we reject the request */
                        strncpy(&packet[1], XSERVER_STOP_REQUEST_REJECTED, RTE_CONPKT_SIZE-2);
                    }
                    else
                    {
                        strncpy(&packet[1], "Rejected bad connect packet", RTE_CONPKT_SIZE-2);
                    }
                    packet[0] = strlen(&packet[1]) + 2;
                    write(e96_cip->ci_sd, packet, packet[0]);
                }
            }

            /* the socket is still used by the cserver process */
            (void) shutdown ( e96_cip->ci_sd , 2 );
            (void) close ( e96_cip->ci_sd );

            MSG1(( INFO_EXIT_CODE, (long) en96_pid, rc ));
            exit ( rc );
        }
	    else
        {
            MSG1(( INFO_FORKED_CHILD, (long) cldpid ));
        }

        (void) close ( e96_cip->ci_sd );
        DBG1 (( MF__,"closed connection sd %d ", e96_cip->ci_sd ));
    }

    RTESec_SAPSSLUtilClose ();

    return 0;
}

/*==========================================================================*/

static int en96_StopXserver()
{
	int rc;

    if ( en96_service_or_port_specified )
    {
        MSGD (( INFO_XSERVER_ON_PORT_STOPPED_VERSION , en96_service_or_port_specified, en96_RTEVersionString ));
    }
    else
    {
    	MSGD (( INFO_XSERVER_STOPPED_VERSION , en96_RTEVersionString ));
    }

    (void)close(en96_updateFifo);
    (void)unlink(en96_updateFifoPath);
    (void)unlink(en96_protocolPipePath);

    (void) e96_SetSigAction ( SIGTERM , SIG_IGN );
    rc = kill ( (pid_t) (- en96_masterPid) , SIGTERM );
    if ( UNDEF == rc )
	{
		MSGD(( ERR_XSERVER_KILL, en96_AcceptId, sqlerrs() ));
		return SP1CE_NOTOK;
    }

	return SP1CE_OK;
}

/*==========================================================================*/

static void en96_ShowRequest( tsp00_Int4 myref )
{
	const char *requestClass;
	const char *serviceClass;

	if ( en96_debugLevel >= 2 )
	{
		switch ( e96_cip->ci_mess_class )
		{
		case RSQL_NEW_DEBUG_LEVEL_REQUEST_EO003:
			requestClass = "NEW DEBUG LEVEL";
			break;

		case RSQL_INFO_REQUEST_EO003 :
			requestClass = "INFO";
			break;

		case RSQL_INFO_REQUEST_KEEP_ALIVE_EO003 :
			requestClass = "KEEP ALIVE";
			break;

		case RSQL_USER_CONN_REQUEST_EO003 :
			requestClass = "USER CONNECT";
			break;

		case RSQL_KERN_CONN_REQUEST_EO003 :
			requestClass = "KERNEL CONNECT";
			break;

		case RSQL_USER_CANCEL_REQUEST_EO003 :
			requestClass = "USER CANCEL";
			break;

		case RSQL_DUMP_REQUEST_EO003 :
			requestClass = "DUMP";
			break;

		case RSQL_PING_REQUEST_EO003:
			requestClass = "PING";
			break;

		case RSQL_CTRL_CONN_REQUEST_EO003 :
			requestClass = "CONTROL CONNECT";
			break;

        case RSQL_CTRL_CANCEL_REQUEST_EO003 :
			requestClass = "CONTROL CANCEL";
			break;

        default:
			requestClass = "UNKNOWN";
			break;
		}

		MSGD(( INFO_XSERVER_REQUEST,
			en96_AcceptId,
			"",
			requestClass,
			(int)e96_cip->ci_mess_class,
			(int)myref,
			(int)e96_remote_ref ));
	}

	if ( en96_debugLevel >= 3 )
	{
		switch ( e96_cip->ci_service )
		{
		case srvUser_esp01:
			serviceClass = "USER";
			break;
		case srvAsyncUser_esp01:
			serviceClass = "ASYNC USER";
			break;
		case srvUtility_esp01:
			serviceClass = "UTILITY";
			break;
		case srvDistribution_esp01:
			serviceClass = "DISTRIBUTION";
			break;
		case srvControl_esp01:
			serviceClass = "CONTROL";
			break;
		case srvEvent_esp01:
			serviceClass = "EVENT";
			break;
		case srvStream_esp01:
			serviceClass = "STREAM";
			break;
        case srvPing_esp01:
			serviceClass = "PING";
			break;
		default:
			serviceClass = "UNKNOWN";
			break;
		}

		MSGD(( INFO_XSERVER_REQUEST_DETAILS,
			en96_AcceptId,
			serviceClass,
			(int)e96_cip->ci_service,
			e96_cip->ci_peer_dbname,
			e96_remote_db[0] ? " Remote DB " : "",
			e96_remote_db[0] ? e96_remote_db : "",
			e96_cip->ci_remote_pid,
			e96_cip->ci_max_segment_size ));
	}
}

/*==========================================================================*/

static void en96_ShowReply( tsp00_Int4 replyClassId )
{
	const char *replyClass;

	switch ( replyClassId )
	{
	case RSQL_NEW_DEBUG_LEVEL_REPLY_EO003:
		replyClass = "NEW DEBUG LEVEL";
		break;

	case RSQL_INFO_REPLY_EO003 :
		replyClass = "INFO";
		break;

	case RSQL_USER_CONN_REPLY_EO003 :
		replyClass = "USER CONNECT";
		break;

    case RSQL_KERN_CONN_REPLY_EO003 :
		replyClass = "KERNEL CONNECT";
		break;

    case RSQL_PING_REPLY_EO003:
		replyClass = "PING";
		break;

    case RSQL_CTRL_CONN_REPLY_EO003 :
		replyClass = "CONTROL CONNECT";
		break;

	default:
		replyClass = "UNKNOWN";
		break;
	}

	MSGD(( INFO_XSERVER_REPLY,
		en96_AcceptId,
		replyClass,
		replyClassId ));
}

/*==========================================================================*/

static  int     en96_Connect ( )
{
# undef  MF__
# define  MF__  MOD__"en96_Connect"
    int               rc;
    int               lgt;
    int               retcod;
    int               replyclass;
    tsp00_Int4        myref;
    tsp00_Int4        e96_remote_ref;
    rte_header        *header;
    char              *ptr;
    tsp00_ErrTextc    pErrText;
    char              packet [ RTE_CONPKT_SIZE ];
    tsp00_Uint4       TransferBufferSize;
    tsp00_Uint4       MaxCmdDataLen;
    tsp00_Uint4       ServerRef;
    tsp00_Pathc       executable;
    tsp00_Pathc       dbroot;
    tsp00_Pathc       serverpgm;
    SAPDB_Byte *      pServerCert = NULL;
    SAPDB_UInt4       lenCert = 0;
    ULONG             commState;
    RTE_OSPid         clientPID = RTE_UNDEF_OSPID;


    e96_close_on_exec ( e96_cip->ci_sd );

    DBGIN;
    header     = (rte_header*) packet;

    e96_cip->ci_connect_time = time ( (time_t*)0 );
    e96_cip->ci_use_count ++;
    e96_cip->ci_state        = CON_CONNECTING;
    e96_cip->ci_protocol     = PROT_UNDEF_EO003;
    e96_cip->ci_packet_cnt   = 1;
    e96_cip->ci_my_pid       = en96_pid;
    e96_cip->ci_my_ref       = 1;
    e96_cip->encryption      = RTE_NO_ENCRYPTION;

    /* get request packet */

    (void) alarm ( SOCK_CON_TMO );
    rc = sql42_recv_conpkt ( e96_cip->ci_sd,
                             header,
                             &e96_cip->ci_state,
                             pErrText );
    (void) alarm ( 0 );

    if ( rc != SP1CE_OK )
    {
        MSGD(( ERR_XSERVER_CONN_REQ, pErrText ));
        return ( SP1CE_NOTOK );
    }

    sql42_extract_conpkt ( header , &e96_cip->ci_mess_class ,
                           &myref , &e96_remote_ref ,
                           &retcod , &e96_cip->ci_service ,
                           &e96_cip->ci_max_segment_size ,
                           &e96_cip->ci_max_data_size ,
                           &e96_cip->ci_packet_size ,
                           &e96_cip->ci_min_reply_size ,
                           &e96_remote_swap ,
                           (char *)e96_cip->ci_peer_dbname ,
                           (char *)e96_remote_db );

    sql42_get_string ( header , RSQL_OPT_REM_PID_EO003 ,
                       sizeof(e96_cip->ci_remote_pid) ,
                       (char *)e96_cip->ci_remote_pid );

#ifdef SIMULATE_STOP_ERROR
    retcod--;
#endif

    if ( (e96_cip->ci_mess_class     == 0)                        &&
         (myref                      == (tsp00_Int4) en96_ListenSocketInfo.port) &&
         (e96_remote_ref             == (tsp00_Int4) en96_ListenSocketInfo.port) &&
         (((SAPDB_UInt2)retcod)      == (tsp00_Int4) en96_ListenSocketInfo.port) &&
         (e96_cip->ci_peer_dbname[0] == '\0')                     &&
         (e96_remote_db[0]           == '\0'))
    {
        MSG1(( ERR_XSERVER_CONN_REQ, XSERVER_STOP_REQUEST_REJECTED ));
        return ( SP1CE_CRASH );
    }

    en96_ShowRequest(myref);

    switch ( e96_cip->ci_mess_class )
    {
        case RSQL_INFO_REQUEST_EO003 :
        case RSQL_INFO_REQUEST_KEEP_ALIVE_EO003 :
            replyclass = RSQL_INFO_REPLY_EO003;
            (void) alarm ( LOCL_CON_TMO );
            retcod = e96_info_request ( header );
            (void) alarm ( 0 );
            break;

        case RSQL_USER_CONN_REQUEST_EO003 :
        case RSQL_KERN_CONN_REQUEST_EO003 :
/* Connect req. received, SERVERDB: %s, SenderRef: %d, PacketSize: %d, MaxDataLen: %d") */
            if ( e96_cip->ci_mess_class == RSQL_USER_CONN_REQUEST_EO003 )
            {
              replyclass = RSQL_USER_CONN_REPLY_EO003;
            }
            else
            {
              replyclass = RSQL_KERN_CONN_REPLY_EO003;
            }

            (void) alarm ( LOCL_CON_TMO );
            retcod = sql33_connect ( e96_cip , pErrText,
                                     (struct connection_info*)0, 0 );
            (void) alarm ( 0 );

            (void)sql42_socket_buffer ( e96_cip->ci_sd ,
                                       &e96_cip->ci_max_segment_size );
            break;

        case RSQL_USER_CANCEL_REQUEST_EO003 :

            if (( e96_cip->pDBM == NULL ) && ( myref & DBMSRV_REFERENCE ) == 0 )
            {
                e96_cip->ci_peer_ref = myref;

                rc = sql33_cancel ( e96_cip , pErrText );
            }
            else
            {
                rc = 1;
            }
            return ( rc == 0 ? SP1CE_OK : SP1CE_NOTOK );

        case RSQL_CTRL_CANCEL_REQUEST_EO003 :
            if ( ( myref & DBMSRV_REFERENCE ) != 0 )
            {
                /* SIGKILL is very deadly... Better use SIGUSR2 + an SIGUSR2 signal handler enabled only while dbmsrv control connect .... */
                rc = kill( (pid_t)(((SAPDB_UInt4)myref) & (~DBMSRV_REFERENCE)), SIGKILL );
            }
            else
            {
                rc = 1;
            }
            return ( rc == 0 ? SP1CE_OK : SP1CE_NOTOK );

        case RSQL_DUMP_REQUEST_EO003 :
            if ( e96_cip->pDBM == NULL )
                rc = sql33_dump ( e96_cip , pErrText );
            return ( rc == 0 ? SP1CE_OK : SP1CE_NOTOK );

        case RSQL_NEW_DEBUG_LEVEL_REQUEST_EO003:
            MSG2(( INFO_XSERVER_NEW_DEBUG_LEVEL, en96_debugLevel ));
            replyclass = RSQL_NEW_DEBUG_LEVEL_REPLY_EO003 ;
            retcod = SP1CE_OK;
            break;

        case RSQL_PING_REQUEST_EO003:
            replyclass = RSQL_PING_REPLY_EO003 ;
            retcod = SP1CE_OK;
            break;

        case RSQL_CERT_REQUEST_EO003:
            replyclass = RSQL_CERT_REPLY_EO003 ;
            commState = RTESec_SAPSSLExportOwnCertificate (&pServerCert, &lenCert, pErrText); 
            retcod = (commState == commErrOk_esp01 ? SP1CE_OK : SP1CE_NOTOK);
            break;


        case RSQL_CTRL_CONN_REQUEST_EO003 :
            /* retrieve dbroot from packet */
            dbroot [0]   = '\0';
            sql42_get_string (header, RSQL_OPT_DBROOT_EO003, sizeof (dbroot),
					                    (char *)dbroot);
            serverpgm [0] = '\0';
            sql42_get_string (header, RSQL_OPT_SERVERPGM_EO003, sizeof (serverpgm),
						                   (char *)serverpgm);

            replyclass = RSQL_CTRL_CONN_REPLY_EO003;

            if ( serverpgm [0] == '\0' )
            {
                strcpy ((char *)serverpgm, "dbmsrv");
            }
            else if ( strcmp((char *)serverpgm, "dbmsrv")
                   && strcmp((char *)serverpgm, "lserver") )
            {
                retcod = SP1CE_NOTOK;
                break;
            }

            (void) alarm ( LOCL_CON_TMO );

            if (NULL != e96_cip->ci_remote_pid && '\0' != e96_cip->ci_remote_pid[0])
            {
                clientPID = (RTE_OSPid)atol(e96_cip->ci_remote_pid);
            }

            retcod = eo908_Connect( serverpgm,
                                    dbroot,
                                    e96_cip->ci_peer_dbname,
                                    &e96_cip->pDBM,
                                    (void**)&(e96_cip->ci_packet[0]),
                                    &MaxCmdDataLen,
                                    &TransferBufferSize,
                                    &ServerRef,
                                    e96_cip->ci_peer_node,
                                    clientPID);

            (void) alarm ( 0 );

            e96_cip->ci_peer_ref = ServerRef | DBMSRV_REFERENCE;

            (void)sql42_socket_buffer ( e96_cip->ci_sd ,
                                       &e96_cip->ci_max_segment_size );

            MSG6 (( INFO_XSERVER_SEGMENT_SIZES, e96_cip->ci_sd, e96_cip->ci_max_segment_size ));

            break;

        default :
            return ( SP1CE_NOTOK );
    }

    if (RSQL_CERT_REPLY_EO003 == replyclass)
    {
        retcod = sql42_SendCertificatePacket (e96_cip->ci_sd, 
                                              replyclass,
                                              e96_cip->ci_peer_ref, 
                                              e96_remote_ref,
                                              retcod, 
                                              pServerCert,
                                              lenCert,
                                              pErrText);

        RTESec_SAPSSLAuxFreeString (&pServerCert);      
    }
    else
    {
        /*
        *  Return UKP's reference as my reference for later use in cancel.
        */
        /*
        *  An INFO_REPLY can be sent as received from kernel.
        */
        if ( (retcod != SP1CE_OK) || (replyclass != RSQL_INFO_REPLY_EO003) )
        {
            sql42_create_conpkt ( header , replyclass ,
                e96_cip->ci_peer_ref , e96_remote_ref ,
                retcod , e96_cip->ci_service ,
                e96_cip->ci_max_segment_size ,
                e96_cip->ci_max_data_size , e96_cip->ci_packet_size ,
                e96_cip->ci_min_reply_size ,
                (char *)e96_cip->ci_peer_dbname ,
                (char *)e96_remote_db );
            /* PTS 1109405 */
            if ( RSQL_PING_REPLY_EO003 == replyclass )
            {
                tsp00_Versionc                  en96_PingReplyRTEVersionString;
                SAPDB_memcpy(en96_PingReplyRTEVersionString, en96_RTEVersionString, sizeof(tsp00_Versionc));
#ifdef HAS_CONVERT_TO_PING_REPLY_VERSION_STRING
                sp100_ConvertToPingReplyVersionString(en96_PingReplyRTEVersionString);
#endif
                sql42_put_string( header, 'V',  en96_PingReplyRTEVersionString );
            }
            else
            {
                sql42_put_string ( header, RSQL_OPT_AUTH_ALLOW_EO003, e96_cip->ci_authentication_allow );
            }
        }

        if ( en96_debugLevel >= 3 )
        {
            en96_ShowReply( replyclass );
        }

        rc = sql42_send_conpkt ( e96_cip->ci_sd , header , pErrText );
        if ( rc != SP1CE_OK )
        {
            MSGD(( ERR_XSERVER_SEND_PACKET, (int)e96_cip->ci_peer_ref, pErrText ));
            return ( SP1CE_NOTOK );
        }
    }

    DBGOUT;

    if ( retcod != SP1CE_OK )
    {
        return ( SP1CE_NOTOK );
    }
    return ( SP1CE_OK );
}

/*==========================================================================*/

static int en96_transfer ( )
{
# undef  MF__
# define  MF__  MOD__"en96_transfer"
  int             rc;
  int             lgt;
  long            cache_lgt;
  char            *cache_buf;
  tsp00_ErrTextc  pErrText;
  rte_header      *header;

  DBGIN;

    cache_lgt = 0;
    cache_buf = malloc ( e96_cip->ci_packet_size );

    if ( ! cache_buf )
    {
      MSGD(( ERR_NOT_ENOUGH_MEMORY, "packet", (e96_cip->ci_packet_size+1023)/1024 ));
      return ( SP1CE_NOTOK );
    }
  e96_cip->ci_request = e96_cip->ci_packet[0];

  MSG4(( INFO_XSERVER_ENTER_DATA_TRANS, en96_AcceptId ));

  (void) e96_SetSigAction ( SIGALRM , e96_catch_alarm );

  for (;; )
  {
      header = e96_cip->ci_request;

      rc = sql42_recv_packet ( e96_cip->ci_sd , header ,
                               e96_remote_swap ,
                               e96_cip->ci_max_data_size + RTE_HEADER_SIZE ,
                               pErrText,
                               cache_buf , &cache_lgt );
      if ( rc != SP1CE_OK )
      {
        if ( rc != SP1CE_RELEASED )
        {
            MSGD(( ERR_XSERVER_RECV_PACKET, e96_cip->ci_sd, pErrText ));
        }
        break;
      }
      if ( ! header->rh_max_send_len )
      {
        MSGD(( ERR_XSERVER_RECV_PACKET, e96_cip->ci_sd, pErrText ));
        rc = SP1CE_RELEASED;
        break;
      }

      lgt = header->rh_max_send_len;

      MSG7(( INFO_XSERVER_READ_REQUEST_PACKET, lgt, e96_cip->ci_sd ));

      if ( lgt < RTE_HEADER_SIZE )
      {
        MSGD (( ERR_XSERVER_SURPRISING_HEADER, lgt, (int)RTE_HEADER_SIZE ));
        rc = SP1CE_NOTOK;
        break;
      }

      if ( en96_debugLevel >= 9 )
      {
         sql46_dump_packet ( header );
#     ifdef FROMUSER
       sql46_dump ( "FROMUSER------>", 16 );
       sql46_dump ( (char*) header , lgt );
       sql46_dump ( "<---------------", 18 - (lgt + 13)%16 );
       sql46_dump ( "--FROMUSER", 11 );
#     endif
      }

      if (   (header->rh_mess_class != RSQL_USER_DATA_REQUEST_EO003)
          && (header->rh_mess_class != RSQL_KERN_DATA_REQUEST_EO003))
      {
        DBG1 (( MF__,"mess_class %d ", header->rh_mess_class ));

        if (   (header->rh_mess_class == RSQL_USER_RELEASE_REQUEST_EO003)
            || (header->rh_mess_class == RSQL_KERN_RELEASE_REQUEST_EO003))
        {
          rc = SP1CE_RELEASED;
          break;
        }
      }

      header->rh_act_send_len = lgt;
      header->rh_sender_ref   = e96_cip->ci_my_ref;
      header->rh_receiver_ref = e96_cip->ci_peer_ref;
      e96_cip->ci_request_lgt = lgt - RTE_HEADER_SIZE;

      if (0 == e96_cip->ci_min_reply_size)
      {
        e96_cip->ci_reply       = header;
        e96_cip->ci_reply_size  = e96_cip->ci_max_data_size;
      }
      else
      {
        e96_cip->ci_reply       = (rte_header*) ((char*) header + lgt);
        e96_cip->ci_reply_size  = e96_cip->ci_max_data_size - e96_cip->ci_request_lgt;
      }

    if ( e96_cip->pDBM != NULL )
    {
      tsp00_Uint4         ResDataLen;

      rc = eo908_Request( e96_cip->pDBM, e96_cip->ci_request_lgt );

      if ( rc != commErrOk_esp01 )
      {
        if ( rc == commErrReleased_esp01 )
          rc = commErrOk_esp01;
        break;
      }
      else if ( ! e96_cip->ci_request_lgt )
        break;

      rc = eo908_Receive( e96_cip->pDBM, (void**)&e96_cip->ci_reply,
                          &ResDataLen );

      if ( rc != commErrOk_esp01 )
        break;

      e96_cip->ci_reply_size = ResDataLen;
    }
    else
    {
        if ( e96_cip->ci_service == srvPing_esp01 )
        {
            SAPDB_UInt1     hops;

            if ( eo420ExtractPingPacket( (char*)(header + 1), e96_cip->ci_request_lgt, &hops, 0, 0 ) )
            {
                if ( hops-- == 0 )
                {   /* No more hops! */
                    lgt =  eo420FillPingPacket ( (char*) (e96_cip->ci_reply + 1),
                                                 e96_cip->ci_reply_size,
                                                 0, /* hops equal to 0 means that the final destination was not reached */
                                                 "XServer",
                                                 en96_RTEVersionString, 0 );

                    header                      = e96_cip->ci_reply;
                    header->rh_act_send_len     = lgt + RTE_HEADER_SIZE ;
                    header->rh_max_send_len     = lgt + RTE_HEADER_SIZE ;
                    header->rh_protocol_id      = RSQL_RTE_PROT_LOCAL_EO003 ;
                    header->rh_mess_class       = RSQL_USER_DATA_REPLY_EO003 ;
                    header->rh_rte_flags        = RSQL_NORMAL_EO003 ;
                    header->rh_residual_packets = 0 ;
                    header->rh_sender_ref       = e96_cip->ci_peer_ref;
                    header->rh_receiver_ref     = e96_remote_ref;
                    header->rh_rte_return_code  = 0 ;
                    header->rh_new_swap_type    = sql42_new_swap_type();
                    header->rh_filler1          = 0 ;
                    lgt                         = header->rh_max_send_len;

                    if ( en96_debugLevel >= 9 )
                    {
                        sql46_dump_packet ( header );
                        sql46_dump ( "TOUSER-------->", 16 );
                        sql46_dump ( (char*) header , lgt );
                        sql46_dump ( "<---------------", 18 - (lgt + 13)%16 );
                        sql46_dump ( "----TOUSER", 11 );
                    }

                    MSG7(( INFO_XSERVER_SERVDB_DATA_SEND, lgt, e96_cip->ci_peer_ref, e96_remote_ref ));

                    rc = sql42_send_packet ( e96_cip->ci_sd , header ,
                                             e96_cip->ci_max_segment_size , pErrText );
                    if ( rc != SP1CE_OK )
                    {
                        MSGD (( ERR_XSERVER_SEND_PACKET, e96_cip->ci_sd, pErrText ));
                        break;
                    }
                    continue;
                }
            }
            else
            {
                rc = SP1CE_NOTOK;
                break;
            }
        }

        rc = sql33_request ( e96_cip , pErrText );
        if ( rc != SP1CE_OK )
        {
            break;
        }
        MSG8 (( INFO_XSERVER_REQUEST_RETURNED, e96_cip->ci_sd, e96_cip->ci_state ));

        /* PTS 1114659 */
        (void) e96_SetSigAction ( SIGALRM , en96_deadCheck );
        (void) alarm ( SOCKET_DEAD_CHECK_TIMEOUT );

        rc = en33CheckedReceive ( en96_deadSocketCheck, e96_cip, e96_cip , pErrText );
        if ( rc != SP1CE_OK )
        {
            break;
        }

        (void) alarm ( 0 );
        (void) e96_SetSigAction ( SIGALRM , e96_catch_alarm );
        /* PTS 1114659 */
    }


      header                  = e96_cip->ci_reply;
      header->rh_sender_ref   = e96_cip->ci_peer_ref;
      header->rh_receiver_ref = e96_remote_ref;
      lgt                     = header->rh_max_send_len;
      if ( en96_debugLevel >= 9 )
      {
       sql46_dump_packet ( header );
       sql46_dump ( "TOUSER-------->", 16 );
       sql46_dump ( (char*) header , lgt );
       sql46_dump ( "<---------------", 18 - (lgt + 13)%16 );
       sql46_dump ( "----TOUSER", 11 );
	  }

      MSG7(( INFO_XSERVER_SERVDB_DATA_SEND, lgt, e96_cip->ci_peer_ref, e96_remote_ref ));

      rc = sql42_send_packet ( e96_cip->ci_sd , header ,
                               e96_cip->ci_max_segment_size , pErrText );
      if ( rc != SP1CE_OK )
      {
        MSGD (( ERR_XSERVER_SEND_PACKET, e96_cip->ci_sd, pErrText ));
        break;
      }

    if ( rc != SP1CE_OK )
    {
      break;
    }
  }

  (void) alarm ( 0 );

  MSG4(( INFO_XSERVER_END_OF_DATA_TRANS, en96_AcceptId ));

  if ( e96_cip->pDBM != NULL )
  {
    eo908_Release ( e96_cip->pDBM );
    e96_cip->pDBM = NULL;
  }
  else
  {
      (void) sql33_release ( e96_cip , pErrText, (connection_info*)0, 0 );
  }

    sql42_send_control ( e96_cip->ci_sd , RSQL_USER_RELEASE_REQUEST_EO003 ,
                         e96_cip->ci_peer_ref , e96_remote_ref , rc );

    DBGOUT;
    free ( cache_buf );
    return ( rc );
}

/*==========================================================================*/

static  int  e96_info_request ( rte_header    *header )
{
# undef  MF__
# define  MF__  MOD__"e96_info_request"
  int             rc;
  int             kernel_fd;
  int             reply_fd;
  uid_t           uid;
  tsp00_ErrTextc  pErrText;
  VF_FILENAMEC    file;
  struct sockaddr sa;
  tsp00_DbNamec   peer_dbname;
  int             idx;


  DBG1 (( MF__,"local dbname '%s' service %d ",
          e96_cip->ci_peer_dbname , e96_cip->ci_service ));


  strcpy (peer_dbname, e96_cip->ci_peer_dbname);

  rc = sql32_open_kernel_fifo ( peer_dbname , &kernel_fd ,
                                &uid , pErrText );
  if ( rc != SP1CE_OK )
  {
    for (idx = 0; e96_cip->ci_peer_dbname[idx] != 0; ++idx)
    {
      if (islower (e96_cip->ci_peer_dbname[idx]))
      {
        peer_dbname[idx] = toupper (e96_cip->ci_peer_dbname[idx]);
      }
    }

    rc = sql32_open_kernel_fifo ( peer_dbname , &kernel_fd ,
                                  &uid , pErrText );
    if (rc != SP1CE_OK)
    {
      DBG1 (( MF__,"cannot open knl fifo: %s", pErrText ));
      return ( SP1CE_NOTOK );
    }
  }

  DBG1 (( MF__,"opened kernel fifo, fd %d ", kernel_fd ));

  /*
  *  The reply fifo must be open before REQUESTOR writes to it.
  *  Otherwise REQUESTOR might be fast enough to close the reply fifo
  *  before the application had it open.
  *  The last close on a fifo clears its contents.
  */
  rc = sql32_open_reply_fifo ( peer_dbname,
                               en96_pid ,
                               (tsp00_Int4) 1 ,
                               (char *)file ,
                               &reply_fd ,
                               pErrText );
  if ( rc != SP1CE_OK )
  {
    DBG1 (( MF__,"cannot open reply fifo: %s", pErrText ));
    (void) close ( kernel_fd );
    return ( SP1CE_NOTOK );
  }

  /*
  *  An INFO_REQUEST can be delivered as received from application.
  *  Node name and reply fifo name must be added.
  */
  sql42_put_string ( header , RSQL_OPT_NODE_EO003 , (char *)e96_cip->ci_peer_node );
  sql42_put_string ( header , RSQL_OPT_FIFO_EO003 , (char *)file );

  rc = sql42_send_conpkt ( kernel_fd , header , pErrText );

  if ( rc != SP1CE_OK )
  {
    DBG1 (( MF__,"cannot write to kernel fifo: %s ", pErrText ));
    (void) close ( kernel_fd );
    (void) close ( reply_fd );
    (void) unlink ( (char *)file );
    return ( SP1CE_NOTOK );
  }
  (void) close ( kernel_fd );

  DBG1 (( MF__,"closed fd %d ", kernel_fd ));
  DBG1 (( MF__,"sent  info   request to '%s' ", e96_cip->ci_peer_dbname ));

  rc = sql42_recv_conpkt ( reply_fd , header , &e96_cip->ci_state , pErrText );

  if ( rc != SP1CE_OK )
  {
    DBG1 (( MF__,": %s ", pErrText ));
    (void) close ( reply_fd );
    unlink ( (char *)file );
    return ( SP1CE_NOTOK );
  }
  DBG1 (( MF__,"received reply from fd %d lgt %d ",
          reply_fd , header->rh_max_send_len ));

  (void) close ( reply_fd );
  unlink ( (char *)file );
  DBG1 (( MF__,"closed/unlinked '%s', fd %d ", file , reply_fd ));

  if ( header->rh_mess_class != RSQL_INFO_REPLY_EO003 )
  {
    DBG1 (( MF__,": got illegal reply packet, messclass %d ",
            header->rh_mess_class ));
    return ( SP1CE_NOTOK );
  }

  /*
  *  If there is no port number in the info reply,
  *  add the own port number to the info reply.
  */
  SAPDB_memcpy ( &sa , &en96_ListenSocketInfo.sa , sizeof(sa) );

  sql42_get_port_no ( header , &sa );

  if ( ! memcmp ( &sa , &en96_ListenSocketInfo.sa , sizeof(sa) ) )
  {
    DBG1 (( MF__,"putting own port no " ));
    sql42_put_port_no ( header , &en96_ListenSocketInfo.sa );
  }
  DBG1 (( MF__,"returning %d ", SP1CE_OK ));
  return ( SP1CE_OK );
}

/*==========================================================================*/

static  void    e96_set_signals ( SIGNALFUNCTYPE    (*func)() )
{
# undef  MF__
# define  MF__  MOD__"e96_setsig"
  (void) e96_SetSigAction ( SIGHUP  , func             );
  (void) e96_SetSigAction ( SIGINT  , func             );
  (void) e96_SetSigAction ( SIGALRM , e96_catch_signal );
  (void) e96_SetSigAction ( SIGTERM , e96_catch_signal );
  (void) e96_SetSigAction ( SIGPIPE , SIG_IGN          );

# ifdef WAIT_FOR_CHILD
   (void) e96_SetSigAction ( SIGCLD  , e96_catch_sigcld );
# else
   (void) e96_SetSigAction ( SIGCLD  , SIG_IGN          );
# endif
}

/*==========================================================================*/

/* This callback is called whenever the semaphore operation is interrupted */
      /* PTS 1114659 */
static  int en96_deadSocketCheck(void *arg)
{
    struct pollfd socketFd[1];
    int pollRc;
    connection_info *pInfo = (connection_info *)arg;

    if ( pInfo != e96_cip )
    {
        return 1;
    }

    socketFd[0].fd = e96_cip->ci_sd;
    socketFd[0].events = POLLIN;
    socketFd[0].revents = 0;

    pollRc = poll(socketFd, 1, 0);

    /* This signal handler is only active while waiting on reply from kernel */
    /* If data is available then, the ping-pong protocol is broken, or the    */
    /* socket is closed both leading to drop of this vserver                  */
    if ( ((UNDEF == pollRc) && (errno != EAGAIN))
      || ((    1 == pollRc) && (socketFd[0].revents & (POLLHUP|POLLERR|POLLIN) )) )
    {
        MSGD (( INFO_XSERVER_CONN_LOST ));
        return 1;
    }

    (void) e96_SetSigAction ( SIGALRM , en96_deadCheck );
    (void) alarm ( SOCKET_DEAD_CHECK_TIMEOUT );

    return 0;
}

/*----------------------------------------------------------*/

static  SIGNALFUNCTYPE  en96_deadCheck (int             sig)
{
# undef  MF__
# define  MF__  MOD__"en96_deadCheck"
}

/*----------------------------------------------------------*/

static  SIGNALFUNCTYPE  e96_catch_alarm (int             sig)
{
# undef  MF__
# define  MF__  MOD__"e96_catch_alarm"
  int                           state;
  tsp00_Int4                      srvref;
  tsp00_Int4                      cliref;
  pid_t                         clipid;
  struct comseg_header          *comseg;
  char                          buffer [ 256 ];


  /* PTS 1104895 */
  if ( e96_cip->pDBM != NULL )
  {
      /* MSGD (( INFO_XSERVER_CONN_LOST )); */
  }
  else
  {
    comseg = e96_cip->ci_comseg;

    /* PTS 1104895 */
    /* DBG1 (( MF__,"comseg 0x%08lx ", (long) comseg ));
    */
    if ( ! comseg )
    {
      /* MSGD (( INFO_XSERVER_CONN_LOST )); */
      exit ( 2 );
    }

    state  = comseg->cs_server_state;
    srvref = comseg->cs_server_ref;
    cliref = comseg->cs_client_ref;
    clipid = comseg->cs_client_pid;

    /* PTS 1104895 */
    /*  DBG1 (( MF__,"state %d sref %d(%d) cref %d cpid %ld ",
        state , (int) srvref , (int) e96_cip->ci_peer_ref ,
        (int) cliref , (long) clipid ));
    */
    if ((clipid != en96_pid) || (cliref != e96_cip->ci_my_ref))
    {
      /* PTS 1104895 */
      /*      DBG1 (( MF__,"assuming timeout " ));
      */
      state = SP1CE_TIMEOUT;/* comseg given away, assuming timeout */
    }
    if ((state != SP1CE_OK) || (srvref != e96_cip->ci_peer_ref))
    {
      /* MSGD (( INFO_XSERVER_CONN_LOST )); */
      sql42_send_control ( e96_cip->ci_sd ,
                           RSQL_USER_RELEASE_REQUEST_EO003 ,
                           e96_cip->ci_peer_ref , e96_remote_ref ,
                           state );
      /* PTS 1104895 */
      /*      DBG1 (( MF__,"waiting for application to close " ));
      */
      while ( recv ( e96_cip->ci_sd , buffer , sizeof(buffer) , 0 ) > 0 )
      {
        /* consume packets until EOF */
      }
      /* PTS 1104895 */
      /*      DBG1 (( MF__,"closing and exiting " ));
      */
      (void) shutdown ( e96_cip->ci_sd , 2 );
      (void) close ( e96_cip->ci_sd );
      exit ( 2 );
    }
  }
}

/*==========================================================================*/

static  SIGNALFUNCTYPE  e96_catch_signal ( int      sig )
{
#undef  MF__
#define  MF__  MOD__"e96_catchsig"
  int        rc;

  /*  DBG1 (( MF__,"caught signal %d ", sig )); */

  switch ( sig )
  {
  case SIGHUP :
  case SIGINT :
  case SIGTERM :
    /*    MSGD (( INFO_VSERVER_STOPPED ));*/
    if ( UNDEF != e96_cip->ci_sd )
    {
        (void) shutdown ( e96_cip->ci_sd , 2 );
        (void) close ( e96_cip->ci_sd );
    }

    if ( (en96_pid == en96_masterPid) && (en96_masterPid > 1) )
    {
        (void) close ( en96_ListenSocketInfo.sd );
	    en96_ListenSocketInfo.sd = UNDEF;
        rc = kill ( (pid_t) (- en96_masterPid) , SIGTERM );

        if ( UNDEF == rc )
            DBG1 (( MF__,"group kill error, %s", sqlerrs() ));
    }
    exit ( sig );
    break;
  case SIGALRM :
    e96_cip->ci_state = CON_TIMEDOUT;
    break;
  }
}

/*==========================================================================*/

#ifdef WAIT_FOR_CHILD
static  SIGNALFUNCTYPE  e96_catch_sigcld ( int sig)
{
# undef  MF__
# define  MF__  MOD__"e96_catchcld"

  en96_ReleaseZombies( );
}
#endif

/*==========================================================================*/

static int e96_close_on_exec ( int fd )
{
  int fd_flag;
  int rc;

  rc = fcntl(fd, F_GETFD, &fd_flag);

  if ( rc == 0 )
    rc = fcntl(fd, F_SETFD, fd_flag|FD_CLOEXEC);

  return rc;
}


/*------------------------------*/

static int e96_SetSigAction(int sig, void (*handler)(int) )
{
  struct sigaction sa;

  sa.sa_handler = handler;
  sigemptyset( &sa.sa_mask );
  sa.sa_flags = 0;
  return sigaction( sig, &sa, NULL);
}

/*-------------------------------*/

#define SHOW_AS_NUMBER_EN96 0
#define SHOW_AS_KBYTE_EN96  1

static void en96_ShowResourceLimit(int what, const char *info, int asKb)
{
    struct rlimit currentlimit;

    if ( getrlimit(what, &currentlimit) != 0 )
    {
        MSGD(( ERR_GET_RESOURCE_FAILED, info, sqlerrs() ));
        return;
    }
    if(RLIM_INFINITY == currentlimit.rlim_cur)
    {
        MSGD(( INFO_RESOURCE_UNLIMITED, info ));
    }
    else
    {
        if ( SHOW_AS_NUMBER_EN96 == asKb )
        {
            MSGD(( INFO_RESOURCE_LIMIT, info, (long)currentlimit.rlim_cur ));
        }
        else
        {
            MSGD(( INFO_RESOURCE_LIMIT_KB, info, (long)(currentlimit.rlim_cur/1024) ));
        }
    }
}

/*-------------------------------*/

static void en96_ShowEnviron(int argc, char *argv[], char *envp[], SAPDB_Int4 callerUid, SAPDB_Int4 callerGid)
{
    char **pEnv;
    int    iArg;

    MSGD(( INFO_ARGUMENT_DUMP_START ));
    for ( iArg = 1; iArg < argc; iArg++ )
    {
        MSGD(( INFO_ARGUMENT_VALUE_DUMP, iArg, argv[iArg] ));
    }
    MSGD(( INFO_ARGUMENT_DUMP_COMPLETED ));

    MSGD(( INFO_ULIMITS_DUMP_START ));

    MSGD(( INFO_STARTER_USER_GROUP_ID, callerUid, callerGid ));
    MSGD(( INFO_CURRENT_USER_ID, (SAPDB_Int4)getuid(), (SAPDB_Int4)geteuid() ));
    MSGD(( INFO_CURRENT_GROUP_ID, (SAPDB_Int4)getgid(), (SAPDB_Int4)getegid() ));
    en96_ShowResourceLimit(RLIMIT_CPU,      "cpu time",             SHOW_AS_NUMBER_EN96);
#if defined(RLIMIT_NPROC)
    en96_ShowResourceLimit(RLIMIT_NPROC,    "number of processes",  SHOW_AS_NUMBER_EN96);
#endif /* RLIMIT_NPROC */
    en96_ShowResourceLimit(RLIMIT_NOFILE,   "number of open files", SHOW_AS_NUMBER_EN96);
    en96_ShowResourceLimit(RLIMIT_CORE,     "core size",            SHOW_AS_KBYTE_EN96);
    en96_ShowResourceLimit(RLIMIT_FSIZE,    "file size",            SHOW_AS_KBYTE_EN96);
    en96_ShowResourceLimit(RLIMIT_DATA,     "heap memory size",     SHOW_AS_KBYTE_EN96);
    en96_ShowResourceLimit(RLIMIT_STACK,    "stack memory size",    SHOW_AS_KBYTE_EN96);
#if defined(RLIMIT_MEMLOCK)
    en96_ShowResourceLimit(RLIMIT_MEMLOCK,  "lockable memory size", SHOW_AS_KBYTE_EN96);
#endif /* RLIMIT_MEMLOCK */
    en96_ShowResourceLimit(RLIMIT_AS,       "virtual memory size",  SHOW_AS_KBYTE_EN96);
#if defined(RLIMIT_RSS)
    en96_ShowResourceLimit(RLIMIT_RSS,      "resident set size",    SHOW_AS_KBYTE_EN96);
#endif /* RLIMIT_RSS */

    MSGD(( INFO_ULIMITS_DUMP_COMPLETED ));

    /*------------------------------------*/

    MSGD(( INFO_ENVIRONMENT_DUMP_START ));
    for ( pEnv = envp; pEnv && *pEnv; ++pEnv )
    {
#define MAX_ENV_PART 40
	    char  EnvBuffer[MAX_ENV_PART+2];
	    char  *pEnvPart;
        int   len;

	    for ( len  = (int)strlen(*pEnv), pEnvPart = *pEnv;
              len  > MAX_ENV_PART;
              len -= MAX_ENV_PART,       pEnvPart += MAX_ENV_PART )
	    {
            SAPDB_memset(EnvBuffer, 0, MAX_ENV_PART+2);
	        if ( pEnvPart == *pEnv )
            {
   	            SAPDB_memcpy(EnvBuffer, pEnvPart, MAX_ENV_PART);
            }
            else
            {
                EnvBuffer[0] = ' '; /* add a space for each followup line */
       	        SAPDB_memcpy(&EnvBuffer[1], pEnvPart, MAX_ENV_PART);
            }
            MSGD(( INFO_ENVIRONMENT_VARIABLE, EnvBuffer ))
	    }

        if ( pEnvPart == *pEnv )
        {
            MSGD(( INFO_ENVIRONMENT_VARIABLE, pEnvPart ))
        }
        else if ( len > 0 )
        {
            EnvBuffer[0] = ' '; /* add a space for each followup line */
			SAPDB_memcpy(&EnvBuffer[1], pEnvPart, len+1);
            MSGD(( INFO_ENVIRONMENT_VARIABLE, EnvBuffer ))
        }
    }
    MSGD(( INFO_ENVIRONMENT_DUMP_COMPLETED ));

    /*------------------------------------*/

    if ( en96_fastModeSelected )
    {
        MSGD(( INFO_XSERVER_SUPPRESS_DNS_LOOKUP ));
    }
}

/*---------------------------------------------------------*/

static void en96_Usage(int longForm)
{
    printf("Syntax: x_server [{<option>}] [ start | stop | update ] [{<option>}]\n");
    printf("Usage: start or stop the communication server for SAPDB. Use -h for option description\n");
    if ( longForm )
    {
        printf("  start (default) start the server\n");
        printf("  stop            stop the server and all its clients\n");
        printf("  update          close listening socket only, but keep established connections untouched\n");
        printf("Options:\n");
        printf("-i           Interactive mode (run in foreground)\n\n");
        printf("-k           like 'stop' action\n");
        printf("-U           like 'update' action\n");
        printf("-F           fast mode (suppress DNS lookup)\n");
        printf("-D level     Set debug level\n");
        printf("-N level     Set new debug level while x_server is running\n");
        printf("-S service   Specify alternative service port (service name or port number).\n");
        printf("-Z size      Set size of xserver.prt file in KBytes (Default %dKByte)\n", DEFAULT_XSERVER_PROT_SIZE/1024);
        printf("-V           Show version and exit\n");
    }
}

/*---------------------------------------------------------*/

static int en96_ActionMatch(const char *action, char *argument)
{
    const char *pMatch = action;
          char *pArg   = argument;
    while ( *pMatch )
    {
        if ( *pMatch == toupper(*pArg) )
        {
            ++pArg;
            ++pMatch;
        }
        else
        {
            return 0;
        }
    }
    return 1;
}

/*----------------------------------------------------------------*/

static int en96_CallHelpServer(int what)
{
    int rc = 0;
    struct stat sbuf;
    char *pathToServer;

    switch(what) {
    case NISERVER_SHOW_VERSION_EN96:
    case NISERVER_START_EN96:
    case NISERVER_UNLISTEN_EN96:
    case NISERVER_STOP_EN96:
        if ( en96_suppressAutomaticNIServerHandling )
        {
            return 0;
        }
        pathToServer = en96_niserverPath;
        break;
    case NISSLSERVER_SHOW_VERSION_EN96:
    case NISSLSERVER_START_EN96:
    case NISSLSERVER_UNLISTEN_EN96:
    case NISSLSERVER_STOP_EN96:
        if ( en96_suppressAutomaticNISSLServerHandling )
        {
            return 0;
        }
        pathToServer = en96_nisslserverPath;
        break;
    default:
        return 0;
    }

    if ( stat(pathToServer, &sbuf) == 0 )
    {
        SAPDB_Int4 exitStatus;
        SAPDB_Int4 child;

        child = RTE_save_fork();
        switch ( child )
        {
        case 0:
            switch(what) {
            case NISERVER_SHOW_VERSION_EN96:
            case NISSLSERVER_SHOW_VERSION_EN96:
                execl(pathToServer, pathToServer, "-V", 0);
                break;

            case NISERVER_START_EN96:
            case NISSLSERVER_START_EN96:
                if ( en96_service_or_port_specified )
                {
                    execl(pathToServer, pathToServer, "-S", en96_service_or_port_specified, 0);
                }
                else
                {
                    execl(pathToServer, pathToServer, 0);
                }
                break;

            case NISERVER_UNLISTEN_EN96:
            case NISSLSERVER_UNLISTEN_EN96:
                if ( en96_service_or_port_specified )
                {
                    execl(pathToServer, pathToServer, "-S", en96_service_or_port_specified, "-u", 0);
                }
                else
                {
                    execl(pathToServer, pathToServer, "-u", 0);
                }
                break;

            case NISERVER_STOP_EN96:
            case NISSLSERVER_STOP_EN96:
                if ( en96_service_or_port_specified )
                {
                    execl(pathToServer, pathToServer, "-S", en96_service_or_port_specified, "-k", 0);
                }
                else
                {
                    execl(pathToServer, pathToServer, "-k", 0);
                }
                break;

            default:
                break;
            }
            break;

        case -1:
            if ( pathToServer == en96_niserverPath )
            {
                MSGD(( WRN_XSERVER_NI_INIT, "Failed to fork niserver" ));
                MSGD(( WRN_XSERVER_NI_INIT, pathToServer ));
                MSGD(( WRN_XSERVER_NI_INIT, sqlerrs() ));
            }
            else
            {
                MSGD(( WRN_XSERVER_NISSL_INIT, "Failed to fork nisslserver" ));
                MSGD(( WRN_XSERVER_NISSL_INIT, pathToServer ));
                MSGD(( WRN_XSERVER_NISSL_INIT, sqlerrs() ));
            }
            rc = 8;
            break;

        default:
            if ( RTE_save_waitpid(child, &exitStatus, 0) != child )
            {
                if ( pathToServer == en96_niserverPath )
                {
                    MSGD(( WRN_XSERVER_NI_INIT, "Failed to wait on niserver" ));
                    MSGD(( WRN_XSERVER_NI_INIT, pathToServer ));
                    if ( en96_service_or_port_specified )
                    {
                        MSGD(( WRN_XSERVER_NI_INIT, en96_service_or_port_specified ));
                    }
                    MSGD(( WRN_XSERVER_NI_INIT, sqlerrs() ));
                }
                else
                {
                    MSGD(( WRN_XSERVER_NISSL_INIT, "Failed to wait on nisslserver" ));
                    MSGD(( WRN_XSERVER_NISSL_INIT, pathToServer ));
                    if ( en96_service_or_port_specified )
                    {
                        MSGD(( WRN_XSERVER_NISSL_INIT, en96_service_or_port_specified ));
                    }
                    MSGD(( WRN_XSERVER_NISSL_INIT, sqlerrs() ));
                }
                rc = 9;
            }
            else
            {
                if ( WIFEXITED(exitStatus) )
                {
                    rc = WEXITSTATUS(exitStatus);
                }
                else
                {
                    char hexExitStatus[44];
                    sp77sprintf(hexExitStatus, sizeof(hexExitStatus), "Exitcode 0x%x", exitStatus);

                    if ( pathToServer == en96_niserverPath )
                    {
                        MSGD(( WRN_XSERVER_NI_INIT, "Bad exit status of niserver" ));
                        MSGD(( WRN_XSERVER_NI_INIT, pathToServer ));
                        if ( en96_service_or_port_specified )
                        {
                            MSGD(( WRN_XSERVER_NI_INIT, en96_service_or_port_specified ));
                        }
                        MSGD(( WRN_XSERVER_NI_INIT, hexExitStatus ));
                    }
                    else
                    {
                        MSGD(( WRN_XSERVER_NISSL_INIT, "Bad exit status of nisslserver" ));
                        MSGD(( WRN_XSERVER_NISSL_INIT, pathToServer ));
                        if ( en96_service_or_port_specified )
                        {
                            MSGD(( WRN_XSERVER_NISSL_INIT, en96_service_or_port_specified ));
                        }
                        MSGD(( WRN_XSERVER_NISSL_INIT, hexExitStatus ));
                    }
                    rc = 10;
                }
            }
        }
    }
    else
    {
        if ( pathToServer == en96_niserverPath )
        {
            MSGD(( WRN_XSERVER_NI_INIT, "Failed stat on niserver" ));
            MSGD(( WRN_XSERVER_NI_INIT, pathToServer ));
            MSGD(( WRN_XSERVER_NI_INIT, sqlerrs() ));
        }
        else
        {
            MSGD(( WRN_XSERVER_NISSL_INIT, "Failed stat on nisslserver" ));
            MSGD(( WRN_XSERVER_NISSL_INIT, pathToServer ));
            MSGD(( WRN_XSERVER_NISSL_INIT, sqlerrs() ));
        }
        rc = 11;
    }
    return rc;
}

/*----------------------------------------------------------------*/

static void en96_CreateHelpServerPathes()
{
    tsp01_RteError RteError;

    SAPDB_memset(en96_niserverPath, 0, sizeof(en96_niserverPath));

    if ( !sqlGetIndependentPgmPath( en96_niserverPath, TERM_WITH_DELIMITER_EO01, &RteError ) )
    {
        strcpy(en96_niserverPath, "./");
    }

    strcat(en96_niserverPath, "niserver");

    SAPDB_memset(en96_nisslserverPath, 0, sizeof(en96_nisslserverPath));

    if ( !sqlGetIndependentPgmPath( en96_nisslserverPath, TERM_WITH_DELIMITER_EO01, &RteError ) )
    {
        strcpy(en96_nisslserverPath, "./");
    }

    strcat(en96_nisslserverPath, "nisslserver");
}

/*----------------------------------------------------------------*/

static int en96_CreatePathesAndChangeDirectory()
{
    int            rc;
    tsp00_Pathc    protocolDirectory;
    char          *protocolDirOwner = 0;
    tsp01_RteError RteError;

    if ( !BuildVserverFilename( vs_updatepipe,
                                true,
                                en96_service_or_port_specified,
                                en96_oldUpdateFifoPath,
                                &RteError ) )
    {
        MSGD (( ERR_XSERVER_PATH_TOO_LONG , "old update fifo path" ));
        return ( 2 );
    }

    if ( !BuildVserverFilename( vs_updatepipe,
                                false,
                                en96_service_or_port_specified,
                                en96_updateFifoPath,
                                &RteError ) )
    {
        MSGD (( ERR_XSERVER_PATH_TOO_LONG , "update fifo path" ));
        return ( 2 );
    }

    sp77sprintf(en96_processIdString, sizeof(en96_processIdString), "%d ", (int)getpid());

    if ( ( strlen(en96_oldUpdateFifoPath) + strlen(en96_processIdString) ) < sizeof(tsp00_Pathc) )
    {
        sql41_get_fifo_dir(en96_oldUpdateFifoReplyPath);
        (void) strcat ( en96_oldUpdateFifoReplyPath , UPDATEFIFOFILE );
        (void) strncat ( en96_oldUpdateFifoReplyPath , en96_processIdString, strlen(en96_processIdString) - 1 ); /* do not use the space */
        if ( en96_service_or_port_specified )
        {
            (void) strcat ( en96_oldUpdateFifoReplyPath , "_" );
            (void) strcat ( en96_oldUpdateFifoReplyPath , en96_service_or_port_specified );
        }
    }
    else
    {
        MSGD (( ERR_XSERVER_PATH_TOO_LONG , "old update reply fifo" ));
        return ( 2 );
    }

    if ( ( strlen(en96_updateFifoPath) + strlen(en96_processIdString) ) < sizeof(tsp00_Pathc) )
    {
        strcpy(en96_updateFifoReplyPath, en96_updateFifoPath);
        strcat(en96_updateFifoReplyPath, "_");
        (void) strncat ( en96_updateFifoReplyPath , en96_processIdString, strlen(en96_processIdString) - 1 ); /* do not use the space */
    }
    else
    {
        MSGD (( ERR_XSERVER_PATH_TOO_LONG , "update reply fifo" ));
        return ( 2 );
    }

    en96_updateFifoReplyTag = (int)getpid();

    if ( !BuildVserverFilename(vs_loggerpipe,
                              true,
                              en96_service_or_port_specified,
                              en96_oldProtocolPipePath,
                              &RteError) )
    {
        MSGD (( ERR_XSERVER_PATH_TOO_LONG , "old protocol pipe" ));
        return ( 2 );
    }

    if ( !BuildVserverFilename(vs_loggerpipe,
                              false,
                              en96_service_or_port_specified,
                              en96_protocolPipePath,
                              &RteError) )
    {
        MSGD (( ERR_XSERVER_PATH_TOO_LONG , "protocol pipe" ));
        return ( 2 );
    }

    if ( sqlGetIndependentProtPath ( protocolDirectory, TERM_WITH_DELIMITER_EO01, &RteError ) )
    {
        struct stat protocolDirStat;

        if ( 0 == stat(protocolDirectory, &protocolDirStat) )
        {
            SAPDB_Char nameBuffer[256];
            SAPDB_UInt4 neededSize;

            if ( RTE_GetUsernameFromPasswdById( (SAPDB_Int4)protocolDirStat.st_uid
                                              , nameBuffer
                                              , sizeof(nameBuffer)
                                              , &neededSize) )
            {
                protocolDirOwner = strdup(nameBuffer);
            }
        }

        if ( !protocolDirOwner )
        {
            protocolDirOwner = malloc(strlen("owner of ") + strlen(protocolDirectory) + 1);
            if ( protocolDirOwner )
            {
                strcpy(protocolDirOwner, "owner of ");
                strcat(protocolDirOwner, protocolDirectory);
            }
        }

        if ( !protocolDirOwner )
        {
            protocolDirOwner = (char *)"owner of SAPDB installation";
        }

        if ( access( protocolDirectory, W_OK|X_OK ) < 0 )
        {
             MSGD (( ERR_XSERVER_REDIRECT_OUTPUT , protocolDirectory, sqlerrs() ));
             MSGD (( INFO_XSERVER_WRONG_USER, protocolDirOwner ));
             return ( 2 );
        }

        rc = chdir ( protocolDirectory );
        if ( UNDEF == rc )
        {
            MSGD (( ERR_XSERVER_CHDIR , protocolDirectory , sqlerrs() ));
            MSGD (( INFO_XSERVER_WRONG_USER, protocolDirOwner ));
            return ( 2 );
        }

        DBG1 (( MF__,"changed directory to '%s' ", protocolDirectory ));
        if ( !BuildVserverFilename( vs_protfile,
                                    true,
                                    en96_service_or_port_specified,
                                    en96_oldProtfile,
                                    &RteError) )
        {
            MSGD (( ERR_XSER_CHDIR , protocolDirectory , RteError.RteErrText ));
            return ( 2 );
        }

        if ( !BuildVserverFilename( vs_protfile,
                                    false,
                                    en96_service_or_port_specified,
                                    en96_protfile,
                                    &RteError) )
        {
            MSGD (( ERR_XSER_CHDIR , protocolDirectory , RteError.RteErrText ));
            return ( 2 );
        }

        DBG1 (( MF__,"en96_protfile '%s' ", en96_protfile ));
    }
    else
    {
        MSGD (( ERR_XSERVER_CHDIR , "IndependendProtPath?" , RteError.RteErrText ));
        return ( 2 );
    }

    return (0);
}

/*--------------------------------------------------------*/

static int en96_CheckRunningVersion( int *pIsNewer, int *pIsAlreadyStarted )
{
    int isAlreadyStarted;
    char serverNode[sizeof(tsp00_Pathc)];
    tsp01_CommErr_Enum pingResult;
    tsp00_Versionc runningVersion;
    tsp00_ErrTextc pingErrText;

    if ( en96_service_or_port_specified )
    {
        strcpy( serverNode, "localhost:" );
        strcat( serverNode, en96_service_or_port_specified );
    }
    else
    {
        strcpy( serverNode, "localhost" );
    }

    pingResult = en42pingRequest( serverNode,
                                  runningVersion,
                                  NULL,
                                  0,
                                  false,
                                  pingErrText );

    if ( SP1CE_OK != pingResult )
    {
        MSGD (( ERR_XSERVER_PING, pingErrText ));
        return (3);
    }

#ifdef HAS_CONVERT_TO_PING_REPLY_VERSION_STRING
    sp100_ConvertToVersionString(runningVersion);
#endif
    isAlreadyStarted = ( 0 == strcmp(runningVersion, en96_RTEVersionString) );

    if ( pIsAlreadyStarted )
    {
        *pIsAlreadyStarted = isAlreadyStarted;
    }

    if ( 0 == pIsNewer )
    {
        if ( isAlreadyStarted )
        {
            MSGD (( INFO_XSERVER_UPDATE_VERSION , runningVersion ));
            return (0);
        }
        else
        {
            MSGD (( ERR_XSERVER_UPDATE_VERSION , runningVersion, en96_RTEVersionString ));
            return (1);
        }
    }
    else
    {
        MSGD (( INFO_XSERVER_RUNNING_VERSION , runningVersion ));
    }

    /* The version string defined VERSION_STR_SP100 "%-9.9s %-8.8s Build %-3.3s-%-11.11s" is expected to be used */
    if ( 0 != strcmp(VERSION_STR_SP100, "%-9.9s %-8.8s Build %-3.3s-%-11.11s")
      || ' ' != runningVersion[9]
      || 0 != strncmp(&runningVersion[18], " Build ", strlen(" Build "))
      || !isdigit(runningVersion[25])
      || !isdigit(runningVersion[26])
      || !isdigit(runningVersion[27])
      || '-' != runningVersion[28] )
    {
        MSGD (( ERR_XSERVER_VERSION_STRING_CONVERSION, "expected format %-9.9s %-8.8s Build %-3.3s-%-11.11s" ));
        return (4);
    }
    else
    {
        char *startPosition = &runningVersion[10];
/*
        int build = ((runningVersion[25] - '0') * 100)
                  + ((runningVersion[26] - '0') * 10)
                  + ((runningVersion[27] - '0'));
 */
        en96_runningVersionId.ID0.MajorVersion_sp100 = (unsigned char)strtoul(startPosition, 0, 10);
        if ( 0 == en96_runningVersionId.ID0.MajorVersion_sp100 )
        {
            MSGD (( ERR_XSERVER_VERSION_STRING_CONVERSION, "major release number cannot be zero" ));
            return (4);
        }

        startPosition = strchr(startPosition, '.');
        if ( startPosition )
        {
            ++startPosition;
        }
        else
        {
            MSGD (( ERR_XSERVER_VERSION_STRING_CONVERSION, "missing minor release number" ));
            return (4);
        }
        en96_runningVersionId.ID0.MinorVersion_sp100 = (unsigned char)strtoul(startPosition, 0, 10);

        startPosition = strchr(startPosition, '.');
        if ( startPosition )
        {
            ++startPosition;
        }
        else
        {
            MSGD (( ERR_XSERVER_VERSION_STRING_CONVERSION, "missing correction level" ));
            return (4);
        }
        en96_runningVersionId.ID0.CorrLevel_sp100    = (unsigned char)strtoul(startPosition, 0, 10);

        /* if own version is older, the running version is newer */
        *pIsNewer = ( IsNewer_esp100 == sp100_CompareVersionIDs(&en96_runningVersionId) );
    }
    return (0);
}

/*--------------------------------------------------*/

static int en96_TryToUpdate(int argc, char *argv[])
{
    int foundOldPathnames = false;
    int  rc;
    int  bytesRead;
    SAPDB_Char runningProcessIdString[40];
    int  updateFifoReplyChannel;

    (void)unlink(en96_oldUpdateFifoReplyPath);
    (void)unlink(en96_updateFifoReplyPath);

    if ( access(en96_protfile, W_OK) == 0
      && access(en96_protocolPipePath, W_OK) == 0
      && access(en96_updateFifoPath, W_OK) == 0 )
    {
#ifndef FREEBSD
        if ( UNDEF == mknod ( en96_updateFifoReplyPath, S_IFIFO | 0660, 0 ) )
#else
        if ( mkfifo ( en96_updateFifoReplyPath, 0660 ) )
#endif
        {
            MSGD (( ERR_XSERVER_CREATE_NAMED_PIPE, en96_updateFifoReplyPath, sqlerrs() ));
            return (1);
        }
        updateFifoReplyChannel = open(en96_updateFifoReplyPath, O_RDONLY|O_NONBLOCK);
    }
    else if ( access(en96_oldProtfile, W_OK) == 0
           && access(en96_oldProtocolPipePath, W_OK) == 0
           && access(en96_oldUpdateFifoPath, W_OK) == 0 )
    {
        /* If old pathes still exist try to rename them first, then do the update */
        foundOldPathnames = true;
        if ( 0 != rename(en96_oldProtocolPipePath, en96_protocolPipePath) )
        {
            MSGD (( ERR_XSERVER_RENAME_PATH, en96_oldProtocolPipePath, en96_protocolPipePath, sqlerrs() ));
            return (1);
        }

        if ( 0 != rename(en96_oldProtfile, en96_protfile) )
        {
            MSGD (( ERR_XSERVER_RENAME_PATH, en96_oldProtfile, en96_protfile, sqlerrs() ));
            /* try to revert to original names again... */
            (void)rename(en96_protocolPipePath, en96_oldProtocolPipePath);
            return (1);
        }

        if ( 0 != rename(en96_oldUpdateFifoPath, en96_updateFifoPath) )
        {
            MSGD (( ERR_XSERVER_RENAME_PATH, en96_oldUpdateFifoPath, en96_updateFifoPath, sqlerrs() ));
            /* try to revert to original names again... */
            (void)rename(en96_protfile, en96_oldProtfile);
            (void)rename(en96_protocolPipePath, en96_oldProtocolPipePath);
            return (1);
        }

        if ( UNDEF == mknod ( en96_oldUpdateFifoReplyPath, S_IFIFO | 0660, 0 ) )
        {
            MSGD (( ERR_XSERVER_CREATE_NAMED_PIPE, en96_oldUpdateFifoReplyPath, sqlerrs() ));
            /* try to revert to original names again... */
            (void)rename(en96_protfile, en96_oldProtfile);
            (void)rename(en96_protocolPipePath, en96_oldProtocolPipePath);
            (void)rename(en96_updateFifoPath, en96_oldUpdateFifoPath);
            return (1);
        }
        updateFifoReplyChannel = open(en96_oldUpdateFifoReplyPath, O_RDONLY|O_NONBLOCK);
    }
    else
    {
        MSGD (( ERR_XSERVER_CREATE_NAMED_PIPE, en96_updateFifoReplyPath, "Access problems to files or pipes" ));
        return (1);
    }

    if ( UNDEF == updateFifoReplyChannel )
    {
        MSGD (( ERR_XSERVER_OPEN_UPDATE_FIFO, en96_updateFifoReplyPath, sqlerrs() ));
        if ( foundOldPathnames )
        {
            /* try to revert to original names again... */
            (void)rename(en96_protfile, en96_oldProtfile);
            (void)rename(en96_protocolPipePath, en96_oldProtocolPipePath);
            (void)rename(en96_updateFifoPath, en96_oldUpdateFifoPath);
        }
        return (1);
    }

    if ( 0 != en96_WriteUpdateCallToUpdateFifo(argc, argv) )
    {
        close(updateFifoReplyChannel);
        MSGD (( ERR_XSERVER_WRITE_UPDATE_FIFO, en96_updateFifoPath, sqlerrs() ));
        if ( foundOldPathnames )
        {
            /* try to revert to original names again... */
            (void)rename(en96_protfile, en96_oldProtfile);
            (void)rename(en96_protocolPipePath, en96_oldProtocolPipePath);
            (void)rename(en96_updateFifoPath, en96_oldUpdateFifoPath);
        }
        return (1);
    }

    SAPDB_memset(runningProcessIdString, 0, sizeof(runningProcessIdString));
    bytesRead = 0;

    do
    {
        SAPDB_Char *firstSpace;
        rc = read(updateFifoReplyChannel, &runningProcessIdString[bytesRead], sizeof(runningProcessIdString)-bytesRead-1);
        if ( rc > 0 )
        {
            bytesRead += rc;
        }
        firstSpace = strchr(runningProcessIdString, ' ');
        if ( firstSpace )
        {
            *firstSpace = 0;
            break;
        }
    } while ( rc >= 0 || (rc < 0 && EAGAIN == errno) );

    if ( rc < 0 )
    {
        MSGD (( ERR_XSERVER_READ_UPDATE_REPLY, en96_updateFifoReplyPath, runningProcessIdString, sqlerrs() ));
    }

    close(updateFifoReplyChannel);

    if ( foundOldPathnames )
    {
        (void)unlink(en96_oldUpdateFifoReplyPath);
    }
    else
    {
        (void)unlink(en96_updateFifoReplyPath);
    }

    if ( rc < 0 && foundOldPathnames )
    {
        /* try to revert to original names again... */
        (void)rename(en96_protfile, en96_oldProtfile);
        (void)rename(en96_protocolPipePath, en96_oldProtocolPipePath);
        (void)rename(en96_updateFifoPath, en96_oldUpdateFifoPath);
        return (1);
    }

    return (0);
}

/*---------------------------------------------*/

static int en96_WriteCommandToUpdateFifo(char *commandBuffer, int totalLength)
{
    int  updateFifo;
    int  bytesWritten;
    int  writeResult;

    updateFifo = open(en96_updateFifoPath, O_WRONLY|O_NONBLOCK);
    if ( UNDEF == updateFifo )
    {
        MSGD (( ERR_XSERVER_OPEN_UPDATE_FIFO, en96_updateFifoPath, sqlerrs() ));
        return (1);
    }

    for ( bytesWritten = 0; bytesWritten < totalLength; bytesWritten += writeResult )
    {
        writeResult = write( updateFifo, commandBuffer + bytesWritten, totalLength - bytesWritten);

        if ( writeResult < 0 && EAGAIN == errno )
        {
            writeResult = 0;
        }

        if ( writeResult < 0 )
        {
            return (1);
        }
    }

    close(updateFifo);
    return (0);
}

/*---------------------------------------------*/

static int en96_WriteUpdateCallToUpdateFifo(int argc, char *argv[])
{
    char *newDBROOT;
    int commandLength = 0;
    int iArg;
    struct updateCommandBuffer_ *commandBuffer;
    char *commandString;

    /* First string is new DBROOT */
    newDBROOT = getenv("DBROOT");
    if ( !newDBROOT )
    {
        MSGD (( ERR_XSERVER_MISSING_DBROOT ));
        return (1);
    }

    commandLength = strlen("DBROOT=")+strlen(newDBROOT)+1;

    /* Skip 'UPDATE' options, but keep all other options */
    for ( iArg = 0; iArg < argc; iArg++ )
    {
        if ( 0 == en96_ActionMatch("UPDATE", argv[iArg])
          && 0 != strcmp(argv[iArg], "-U") )
        {
            commandLength += strlen(argv[iArg])+1;
        }
    }

    commandBuffer = (struct updateCommandBuffer_ *)malloc( (3*sizeof(int))+commandLength);
    commandBuffer->totalLength = (3*sizeof(int))+commandLength;
    commandBuffer->replyTag    = en96_updateFifoReplyTag;
    commandBuffer->argCount    = 0;
    commandString = commandBuffer->commandString;

    strcpy(commandString, "DBROOT=");
    commandString += strlen("DBROOT=");
    strcpy(commandString, newDBROOT);
    commandString += strlen(newDBROOT);
    *commandString++ = 0;

    for ( iArg = 0; iArg < argc; iArg++ )
    {
        if ( 0 == en96_ActionMatch("UPDATE", argv[iArg])
          && 0 != strcmp(argv[iArg], "-U") )
        {
            strcpy(commandString, argv[iArg]);
            commandString += strlen(argv[iArg]);
            *commandString++ = 0;
            ++(commandBuffer->argCount);
        }
    }

    return en96_WriteCommandToUpdateFifo( (char *)commandBuffer, commandBuffer->totalLength);
}

/*---------------------------------------------*/

static int en96_CheckForUpdateRequest()
{
    SAPDB_Char *ownDBROOT = 0;
    int  ioResult;
    char buffer = 0;
    SAPDB_Char updatingProcessIdString[40];
    SAPDB_Char takeoverSocket[20];
    int  bytesRead;
    int  bytesWritten;
    int  replyChannel;
    struct updateCommandBuffer_ *commandBuffer;
    union {
        SAPDB_Int4 totalLength;
        SAPDB_Char totalLengthAsByteStream[4];
    } lengthInput;
    pid_t updateChild;
    SAPDB_Byte    *inputBuffer;
    int  iArg;
    SAPDB_Char    **newArgv;
    SAPDB_Char    *newDBROOT;
    SAPDB_Char    *commandString;
    char *myDBROOT = getenv("DBROOT");

    if ( !myDBROOT )
    {
        MSGD (( ERR_XSERVER_MISSING_DBROOT ));
        return (0);
    }
    ownDBROOT = (SAPDB_Char *)malloc(strlen("DBROOT=") + strlen(myDBROOT) + 1);
    if ( !ownDBROOT )
    {
        MSGD (( ERR_XSERVER_READ_UPDATE_FIFO, "", "malloc for DBROOT envronment failed"));
        return (0);
    }
    strcpy(ownDBROOT, "DBROOT=");
    strcat(ownDBROOT, myDBROOT);

    /* read input length and allocate input structure */
    bytesRead = 0;
    do
    {
        ioResult = read(en96_updateFifo, &lengthInput.totalLengthAsByteStream[bytesRead], sizeof(SAPDB_Int4)-bytesRead);
        if ( ioResult > 0 )
        {
            bytesRead += ioResult;
        }
    } while ( bytesRead != sizeof(SAPDB_Int4) && (ioResult >= 0 || (ioResult < 0 && EAGAIN == errno)) );

    if ( bytesRead != sizeof(SAPDB_Int4) )
    {
        MSGD (( ERR_XSERVER_READ_UPDATE_FIFO, en96_updateFifoReplyPath, sqlerrs() ));
        return (0);
    }

    commandBuffer = (struct updateCommandBuffer_ *)malloc(lengthInput.totalLength);
    if ( !commandBuffer )
    {
        MSGD (( ERR_XSERVER_READ_UPDATE_FIFO, en96_updateFifoReplyPath, "malloc for update command buffer failed"));
        return (0);
    }

    /* read remaining input content */
    commandBuffer->totalLength = lengthInput.totalLength;
    inputBuffer = (SAPDB_Byte *)&commandBuffer->replyTag;

    bytesRead = 0;
    do
    {
        ioResult = read(en96_updateFifo, &inputBuffer[bytesRead], lengthInput.totalLength-sizeof(SAPDB_Int4)-bytesRead);
        if ( ioResult > 0 )
        {
            bytesRead += ioResult;
        }
    } while ( bytesRead != lengthInput.totalLength-sizeof(SAPDB_Int4) && (ioResult >= 0 || (ioResult < 0 && EAGAIN == errno)) );

    if ( bytesRead != lengthInput.totalLength-sizeof(SAPDB_Int4) )
    {
        MSGD (( ERR_XSERVER_READ_UPDATE_FIFO, en96_updateFifoReplyPath, sqlerrs() ));
        free(commandBuffer);
        return (0);
    }

    /* Parse received input stream */
    commandString = commandBuffer->commandString;

    /* execute stop command via update pipe */
    if ( lengthInput.totalLength == sizeof(struct updateCommandBuffer_) )
    {
        if ( !memcmp("STP", commandString, 4) )
        {
            free(commandBuffer);
            en96_CallHelpServer(NISERVER_STOP_EN96);
            en96_CallHelpServer(NISSLSERVER_STOP_EN96);
		    en96_StopXserver();
            exit(0);
        }

        if ( !memcmp("DBG", commandString, 4) )
        {
            MSG1(( INFO_XSERVER_NEW_DEBUG_LEVEL, en96_debugLevel ));
            en96_debugLevel = commandBuffer->replyTag;
            MSGD(( INFO_XSERVER_NEW_DEBUG_LEVEL, en96_debugLevel ));
            free(commandBuffer);
            return (0);
        }
    }

    /* first is new process DBROOT */
    newDBROOT = commandString;
    commandString += strlen(commandString)+1;

    sp77sprintf(updatingProcessIdString, sizeof(updatingProcessIdString), "%d", commandBuffer->replyTag);

    if ( (  strlen(en96_updateFifoReplyPath)
          + strlen("_")
          + strlen(updatingProcessIdString)
         ) >= sizeof(tsp00_Pathc) )
    {
        MSGD (( INFO_XSERVER_UPDATE_REQUEST, updatingProcessIdString ));
        MSGD (( ERR_XSERVER_PATH_TOO_LONG , en96_updateFifoReplyPath ));
        free(commandBuffer);
        return (0);
    }

    strcpy( en96_updateFifoReplyPath, en96_updateFifoPath);
    strcat( en96_updateFifoReplyPath, "_");
    strcat( en96_updateFifoReplyPath , updatingProcessIdString);

    replyChannel = open(en96_updateFifoReplyPath, O_WRONLY|O_NONBLOCK);
    if ( UNDEF == replyChannel )
    {
        MSGD (( ERR_XSERVER_OPEN_UPDATE_FIFO, en96_updateFifoReplyPath, sqlerrs() ));
        free(commandBuffer);
        return (0);
    }
    sp77sprintf(en96_processIdString, sizeof(en96_processIdString), "%d ", (int)getpid());

    for ( bytesWritten = 0; bytesWritten < (int)strlen(en96_processIdString); bytesWritten += ioResult )
    {
        ioResult = write( replyChannel,
                    &en96_processIdString[bytesWritten],
                    ((int)strlen(en96_processIdString))-bytesWritten);

        if ( ioResult < 0 && EAGAIN == errno )
        {
            ioResult = 0;
        }

        if ( ioResult < 0 )
        {
            MSGD (( ERR_XSERVER_WRITE_UPDATE_FIFO, en96_updateFifoReplyPath, sqlerrs() ));
            close(replyChannel);
            free(commandBuffer);
            return (0);
        }
    }
    close(replyChannel);

    /* next is path to vserver */
    if ( access( commandString, X_OK ) != 0 )
    {
        MSGD(( ERR_XSERVER_UPDATE_PROCESS, commandString, sqlerrs() ));
        free(commandBuffer);
        return (0);
    }

    /* stop ni-server for update */
	en96_CallHelpServer(NISERVER_STOP_EN96);
	en96_CallHelpServer(NISSLSERVER_STOP_EN96);

    /* build argument vector for execv */
    newArgv = (char **)malloc(commandBuffer->argCount+3);
    for ( iArg = 0; iArg < commandBuffer->argCount; ++iArg )
    {
        newArgv[iArg] = commandString;
        commandString += strlen(commandString)+1;
    }
    newArgv[iArg++] = "-T";
    sp77sprintf(takeoverSocket, sizeof(takeoverSocket), "%d,%d", en96_ListenSocketInfo.sd, en96_protocolPipe);
    newArgv[iArg++] = takeoverSocket;
    newArgv[iArg] = 0;

    putenv(newDBROOT);
    (void)execv(newArgv[0], newArgv);
    MSGD(( ERR_XSERVER_OPEN_PROCESS, sqlerrs() ));
    putenv(ownDBROOT);
    free(newArgv);
    free(commandBuffer);
    return (0);
}

/*---------------------------------------------*/

static SAPDB_Bool BuildVserverFilename( enum vserver_files which,
                                        SAPDB_Bool oldForm,
                                        SAPDB_Char const *portOrServiceSpecified,
                                        tsp00_Pathc pathToFill,
                                        tsp01_RteError *pRteError)
{
    static char myHostname[64] = { 0 };

    switch( which )
    {
    case vs_updatepipe:
    case vs_loggerpipe:
        sql41_get_fifo_dir(pathToFill);
        break;
    default:
        /* Since DBROOT differs in development environments, older running vservers cannot get an update... */
        if ( oldForm
          && en96_foundRunningVersion
          && (
                 en96_runningVersionId.ID0.MajorVersion_sp100 < 7
            ||
               ( en96_runningVersionId.ID0.MajorVersion_sp100 == 7
              && en96_runningVersionId.ID0.MinorVersion_sp100 <= 4 )
             )
          && eo01_IsEnvVarDoNotUseIndepPathSet() )
        {
            /* in our development environment update is not supportable due to different DBROOTs... */
            eo46_set_rte_error ( pRteError, undef_ceo00, "Update not support for SAPDB_NO_INDEPPATH=YES + Release <= 7.4", NULL ) ;
            return false;
        }

        if ( !sqlGetIndependentProtPath ( pathToFill, TERM_WITH_DELIMITER_EO01, pRteError ) )
        {
            return false;
        }
        break;
    }

    if ( oldForm )
    {
        if ( ( strlen(pathToFill)
             + strlen("xserver")
             + strlen(vs_filesuffixes[(int)which])
             + (portOrServiceSpecified ? strlen(portOrServiceSpecified) + 1 : 0)
             ) >= sizeof(tsp00_Pathc) )
        {
            eo46_set_rte_error ( pRteError, undef_ceo00, "Path too long to file name:", pathToFill ) ;
            return false;
        }

        strcat ( pathToFill, "xserver");
        strcat ( pathToFill, vs_filesuffixes[(int)which]);
        if ( portOrServiceSpecified ) strcat( pathToFill , "_" );
        if ( portOrServiceSpecified ) strcat( pathToFill , portOrServiceSpecified );

        return true;
    }

    if ( !myHostname[0] )
    {
        if ( sql43_get_my_host_name(myHostname, sizeof(myHostname)) == -1 )
        {
            eo46_set_rte_error ( pRteError, undef_ceo00, "Failed to get local maschine name", NULL ) ;
            return false;
        }
    }

    if ( ( strlen(pathToFill)
         + strlen("xserver_")
         + strlen(myHostname)
         + strlen(vs_filesuffixes[(int)which])
         + (portOrServiceSpecified ? strlen(portOrServiceSpecified) + 1 : 0)
         ) >= sizeof(tsp00_Pathc) )
    {
        eo46_set_rte_error ( pRteError, undef_ceo00, "Path too long to file name:", pathToFill ) ;
        return false;
    }

    strcat ( pathToFill, "xserver_" );
    strcat ( pathToFill, myHostname);
    strcat ( pathToFill, vs_filesuffixes[(int)which]);
    if ( portOrServiceSpecified ) strcat( pathToFill, "_" );
    if ( portOrServiceSpecified ) strcat( pathToFill, portOrServiceSpecified );

    return true;
}

/*---------------------------------------------*/

static void en96_GetCallerInformation(int  *        pArgc,
                                      char *        argv[],
                                      SAPDB_Int4 *  pCallerUid,
                                      SAPDB_Int4 *  pCallerGid,
                                      SAPDB_Bool *  pIsAdministrator)
{
    *pIsAdministrator = true;
    *pCallerUid = (SAPDB_Int4)RTE_save_getuid();
    *pCallerGid = (SAPDB_Int4)RTE_save_getgid();

    if ( *pArgc > 4 && !strcmp(argv[1], "-" SDBSTARTER_PROGRAM) )
    {
        int iArg;

        *pCallerUid = (SAPDB_Int4)strtol(argv[2], 0, 10);
        *pCallerGid = (SAPDB_Int4)strtol(argv[3], 0, 10);
        *pIsAdministrator = ( 'A' == *argv[4] );

        for ( iArg = 5; iArg < *pArgc; iArg++ )
        {
            argv[iArg-4] = argv[iArg];
        }
        argv[iArg-4] = 0;
        *pArgc -= 4;
        return;
    }

    /* In Berlin development make environment a remuser is not necessarily an SDB administrator */
    if ( eo01_IsEnvVarDoNotUseIndepPathSet() )
    {
        *pIsAdministrator = RTE_IsSdbAdministrator (*pCallerUid, *pCallerGid);
    }

    return;
}

