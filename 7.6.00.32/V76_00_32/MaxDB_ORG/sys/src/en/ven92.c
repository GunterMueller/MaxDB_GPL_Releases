/*!---------------------------------------------------------------------
  @file           ven92.c
  @author         JoergM
  @brief          UNIX main module for console (x_cons)
  @see            example.html ...

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


//
//   FreeBSD portions added by Kai Mosebach,
//   For more informations see : http://www.komadev.de/sapdb
//

\endif
---------------------------------------------------------------------*/




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include <ctype.h>
#include <time.h>

#include "hsp100.h"
#include "geo007_1.h"
#include "heo00x.h"
#include "geo002.h"
#include "gen003.h"
#include "heo46.h"
#include "heo52.h"
#include "heo92.h"
#include "geo50_0.h"
#include "geo001.h"
#include "geo57.h"
#include "hen40.h"
#include "gen41.h"
#include "gen38.h"
#include "RunTime/Console/RTECons_Main.h"
#include "RunTime/RTE_ConsoleStandardOutput.h"
#include "gen22.h"
#include "gen921.h"
#include "hsp77.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define MOD__ "ven92.c"

#if !OSF1
#undef		NULL
#endif
#include        <sys/param.h>
#ifndef		NULL
#define		NULL		0L
#endif

#define spri            (void) sprintf
#define pri             (void) printf

char  * USAGE = "usage: console dbname [command] \n" ;

#define	DEVTRACENAME	"sqldbtrace"

#define XCONS_INFO_LINE      "-------------------------------------------------------------------------------"

#define SIG_FORK_AND_EXIT SIGINT
#define SIG_STOP_DATABASE SIGTERM

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


#define         PRINT_LINES             16

#define RESET_TIME_DATES(tcb) \
        SAPDB_memset ( &(tcb->TaskStateStat), 0, sizeof ( teo52_TaskStateStatistic ) ) ;

#define MSGNO(_n) _n, ERR_TYPE, "CONSOLE "


/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/

extern	void				sqlclock ();
extern	void				sqlinit ();

extern  char                            * getenv ();
extern  long                            atol ();
extern	unsigned			sleep ();

#if ! ( defined(_IBMR2) || defined(OSF1) || defined(NMP) || defined(FREEBSD) )	/* already declared in include file */
#   if defined(HP9)
    extern  clock_t                         times ();
#   else
    extern  long                            times ();
#   endif
#endif  /* _IBMR2 */

/* global  void                            pri (char *,...); */

key_t                                   common_rte_key = ((key_t)0);
SQL_DBNAMEC                             dbname ;

/* local data */

int                                     e92_print_lines = 0;
int                                     e92_access_mode = 0;
                                        /* 0=none, 1=rdonly, 2=rdwr */
#ifdef	OLD
int                                     e92_kernel_active = 0;
                                        /* 0=not active, 1=active */
char                                    e92_unknown_type [ 8 ];
char                                    e92_unknown_state [ 14 ];
#endif	/*OLD*/
int                                     e92_shmid = 0;
int                                     e92_interval   = 0 ;
int                                     e92_repeat     = 1 ;
int                                     e92_repeat_inc = 1 ;
static	SIGNALFUNCTYPE                  (*e92_oldsigint ) ();
int                                     e92_break_flag = 0 ;
int                                     e92_show_long = 0 ;
struct  KERNEL_GLOBAL_SECTION *en92_NonRemappedKgs = 0;
unsigned long en92_SharedSegmentStart = 0;
unsigned long en92_SharedSegmentEnd = 0;
int  en92_ShmIsMemoryCopy = 0;
long en92_KgsRemappingDistance = 0;
long en92_NonRemappedKgsRemappingDistance = 0;
/* 0 pointer is still null pointer afterwards, all other are adjusted by remapping distance */
#define IS_POINTER_INSIDE_SHM_EN92(member_) \
   ( ( ((unsigned long)member_) >= en92_SharedSegmentStart) && (((unsigned long)member_) < en92_SharedSegmentEnd) )
#define ADJUST_SHM_POINTER_EN92(pointerType_, member_) \
if ( IS_POINTER_INSIDE_SHM_EN92(member_) ) member_ = (pointerType_)((unsigned long)(member_) + en92_KgsRemappingDistance)
#define MAP_TO_NON_REMAPPED_POINTER(pointerType_, member_) \
    ( (member_) != 0 ? (pointerType_)((unsigned long)(member_) - en92_NonRemappedKgsRemappingDistance) : (pointerType_)0 )


/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

static  int                             e92_command_driver ();
static  int                             e92_execute_command ();
static  int                             e92_help ();
static  int                             e92_dbmessages ();
static  int                             e92_getparam ();
static  int                             e92_putparam ();
static  int                             e92_dbshow ();
#if 0
static  int                             e92_time_measure ();
static  int                             e92_dbreset_cnt ();
#endif
static  int                             e92_dbcancel ();
static  int                             e92_dbcore ();
static  int                             e92_dbprofile ();
static  int                             e92_dbdebuglevel ();
static  int                             e92_dbkill ();
static  int                             e92_dbstop ();
static  int                             e92_dbstart ();
static  int                             e92_dbclear ();
static  int                             e92_shell ();
static  void                            e92_dbshow_aio       ();
static  void                            e92_dbshow_io        ();
static  void                            e92_dbshow_processes ();
static  void                            e92_dbshow_regions   ();
static  void                            e92_dbshow_rte       ();
static  void                            e92_dbshow_dev_io    ();
static  void                            e92_dbshow_queues    ();
static  void                            e92_dbshow_taskstack ();
static  void                            e92_dbshow_total_stat();
static  void                            e92_dbshow_task_stat ();
static  void                            e92_dbshow_task_cnt  ();
static  void                            e92_dbshow_task_reg  ();
static  void                            e92_dbshow_task_que  ();
static  void                            e92_dbshow_sleeptime ();
static  void                            e92_dbshow_sleepstat ();
static  void                            e92_dbshow_state     ();
static  void                            e92_dbshow_storage   ();
static  void                            e92_dbshow_versions  ();
static  void                            e92_dbshow_suspend   ();
static  void                            e92_dbshow_connections ();
static  int                             e92_map_shm ();
static  int                             e92_get_key_and_map ();
static  void                            e92_unmap_shm ();
static  int                             e92_get_interval ();
static  int                             e92_test_interval ();
static	SIGNALFUNCTYPE                  e92_catch_signal ();
static  int                             e92_get_show_long ();
#if 0
static	void				e92_dbreset_regions ();
static  void    			e92_dbreset_task_region_cnt () ;
static	void				e92_dbreset_task_cnt ();
static	void				e92_dbreset_sleeptime ();
static	void				e92_dbreset_iocnt ();
static	void				e92_dbreset_suspend ();
#endif
static	void				print_task_state_cnts ();
static	void				print_task_region_access ();
static	int				check_kernel ();
static	int				e92_error_io ();
static  int                 e92_prio_sv_w(int    argc ,
                                           char **argv ,
                                           int    argn ,
                                           int   *act_arg );
static  int                 e92_debugbreak(int    argc ,
                                           char **argv ,
                                           int    argn ,
                                           int   *act_arg );
static  int                 e92_dbtop ();
static	int				cmp_io ();
static	int				cmp_reg();
static	int				cmp_cmd();
static	int				cmp_vwait();
static	int				cmp_vsusp();
static	int				get_id();
static  int                             e92_create_shm_from_file();
static  int                             e92_check_ipcdb();

char *en92_peer_pid ( struct TASK_TYPE	*tcb, char *peer_pid_buffer);
char *en92_peer_node ( struct TASK_TYPE	*tcb );
int	en92_has_remote_pid ( struct TASK_TYPE	*tcb );
void en92_UnmapKgsNotRemapped();
int en92_MapKgsNotRemapped ( int shmid, int needWriteAccess );
/*!
  @brief Using the non remapped shared memory segment, create a copy of the shared segment given
  @param shmAddress [in] the original shared memory segment
  @return the pointer to the memory copy
 */
struct  KERNEL_GLOBAL_SECTION * en92_CreateKgsMemoryCopy(struct SHARED_SEGMENT const *shmAddress);

/*===========================================================================*
 *  PUBLIC FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

#undef MF__
#define MF__ MOD__"main"
int     c_main (
int                     argc ,
char                    ** argv )
{
    int                     argn , rc ;
    int                     act_arg ;

    sqlinit ( "console" , (char*)0 );

    /*
     * All output now redirected to standard output without buffering
     */
    (void) setbuf ( stdout , (char*)0 );
    (void) setbuf ( stderr , (char*)0 );
    close(2);
    dup(1);

    en22SelectChannelForMessageOutput(1, false);

    DBGIN;

    /* PTS1105151
     *  File permission mode is always specified absolutely
     */
    (void) umask ( 007 );

    argn = 1 ;
    if (argc <= 1 || argv [ argn ] [ 0 ] == '-' )
    {
      fputs ( XCONS_USAGE, stdout );
      return ( 1 );
    }

    if ( (int) strlen ( argv [ argn ] ) >= (int) sizeof(SQL_DBNAMEC) - 1 )
    {
        MSGCD(( MSGNO(11987), "dbname too long '%s' \n" , argv [ 1 ] ))
        (void) fputs ( USAGE , stderr );
        return ( 1 );
    }
    (void) strcpy ( (char *)dbname , argv [ argn ] );
    argn ++ ;
    DBG1 (( MF__,"dbname '%s' \n", dbname ))

    /*
     *  Compute the key for ipc resources from the database name
     */

    common_rte_key = sql41_get_server_key ( dbname );

    /*
     * KWe: remove empty arguments.    13.02.92
     */
    while ( argn < argc  && argv [ argn ] [ 0 ] == '\0' )
    {
        argn ++;
    }

    if ( argc <= argn )
    {
        DBG1 (( MF__,"calling command driver \n" ))
        rc = e92_command_driver ( );
    }
    else
    {
        e92_interval = 0;
        e92_repeat   = 1;
        do
        {
            DBG1 (( MF__,"executing immediate command \n" ))
            rc = e92_execute_command ( argc , argv , argn, &act_arg );
            if ( rc ) return ( rc );

            if ( act_arg ) (void) e92_get_interval ( argv, argc, act_arg );

            if ( ! ( e92_repeat -= e92_repeat_inc ) ) break;
            if ( e92_interval ) (void) sleep ( (unsigned) e92_interval );
            DBG1 (( MF__,"interval = %d; repeat '%d' \n",
                                      e92_interval, e92_repeat ))
        } while ( e92_repeat );
    }


    DBGOUT;
    return ( rc );
}

/*===========================================================================*
 *  LOCAL/PRIVATE/PROTECTED FUNCTIONS (CODE)                                 *
 *===========================================================================*/

static  int     e92_command_driver ( )
{
    int                     rc , argc , argn ;
    char                    * argv [ 256 / 4 ] ;
    char                    * p , line [ 256 ] ;
    int                     act_arg ;


    DBG1 (( MF__,"e92_cmddrv: called \n" ))

    argv[0] = (char *)"cons";
    argv[1] = (char *)dbname;

    for ( ; ; )
    {
	e92_show_long = 0 ;  /* MWa reset e92_show_long */
        e92_print_lines = PRINT_LINES ;

        SAPDB_memset ( line , 0 , sizeof(line) );
        (void) fputs ( "Command: " , stdout );
        (void) fflush ( stdout );
        p = fgets ( line, sizeof(line)-1, stdin );
        if ( p == 0 ) break ;
        DBG1 (( MF__,"e92_cmddrv: command line '%s' \n", line ))

        for ( argc = 2 ; argc < sizeof(argv)/sizeof(argv[0]) ; argc ++ )
        {
            argv [ argc ] = 0 ;
            for ( ; isspace ( * p ); p ++ );
            if ( * p == '\0' ) break ;
            argv [ argc ] = p ;
            for ( ; ! isspace ( * p ) && * p != '\0' ; p ++ );
            if ( * p == '\0' ) break ;
            * p = '\0' ;
            p ++ ;
        }
        if ( argv [ argc ] != 0 ) argc ++ ;
        argv [ argc ] = 0 ;
        argn = 2 ;
        if ( argc < 3 ) continue ;

        e92_interval = 0;
        e92_repeat   = 1;
	e92_repeat_inc = 1;
#if _IBMR2 || LINUX  || FREEBSD
    (void) siginterrupt ( SIG_FORK_AND_EXIT , true );
#endif
        e92_oldsigint = signal ( SIG_FORK_AND_EXIT, e92_catch_signal );
        do
        {
            DBG1 (( MF__,"e92_cmddrv: executing command \n" ))
            rc = e92_execute_command ( argc , argv , argn, &act_arg );
            if ( rc == - 1001 ) break ;
            if ( rc ) break;

            if ( act_arg ) (void) e92_get_interval ( argv, argc, act_arg );
            if ( ! ( e92_repeat -= e92_repeat_inc ) ) break;
            if ( e92_break_flag ) break;
            DBG1 (( MF__,"e92_cmddrv: before sleep' \n" ))
            if ( e92_interval ) (void) sleep ( (unsigned) e92_interval );
            DBG1 (( MF__,"e92_cmddrv: interval = %d; repeat '%d' \n",
                                      e92_interval, e92_repeat ))
        } while ( e92_repeat );
        (void) signal ( SIGINT, e92_oldsigint );
        if ( rc == - 1001 ) break ;
    }

    DBG1 (( MF__,"e92_cmddrv: returning \n" ))
    return ( 0 );
}


/*==========================================================================*/


static  int     e92_execute_command (
int                     argc ,
char                    ** argv ,
int                     argn ,
int                     *act_arg)
/*
 * KWe : act_arg is used for showing the caller, at which position
 *       the parameter count is placed.
 */
{
    int                     i , rc ;
    PROCESS_ID              taskid ;
    char                    * p , command [ 256 ] ;


    DBG1 (( MF__,"e92_execmd: called,    command '%s' \n", argv [ 0 ] ))

    *act_arg = 0;

    for ( i = 1 ; i < argc ; i ++ )
    {
        DBG1 (( MF__,"e92_execmd: called,    arg     '%s' \n", argv [ i ] ))
    }

    if ( argv [ argn ] [ 0 ] == '!' )
    {
        argv [ argn ] ++ ; /* skip '!' */
        if ( argv [ argn ] == '\0' ) argn ++ ; /* skip empty arg */
        DBG1 (( MF__,"e92_execmd: shell command '%s' \n", argv [ argn ] ))
        rc = e92_shell ( argc , argv , argn );
        return ( rc );
    }

    p = argv [ argn ] ;
    for ( i = 0 ; i < sizeof(command) && p [ i ] != '\0' ; i ++ )
    {
        /* Incompatibility in ctype of screenlib vs. libc */
        /* Incompatibility in toupper of berkeley vs. att */
        if ( islower ( p [ i ] ) )
                command [ i ] = toupper ( p [ i ] );
        else    command [ i ] = p [ i ] ;
    }
    if ( i < sizeof(command) )
    {
        command [ i ] = '\0' ;
    }
    else
    {
        MSGCD(( MSGNO(11987), "command name too long: '%s' \n" , p ))
        return ( 101 );
    }

    argn ++ ;
    DBG1 (( MF__,"e92_execmd: command name '%s' \n", command ))


    if ( e92_strcont ( "?"    , command , 1 ) ||
         e92_strcont ( "HELP" , command , 1 )    )
    {
        rc = e92_help ( );
        return ( rc );
    }
    if ( e92_strcont ( "START" , command , 3 ) )
    {
        rc = e92_dbstart ( );
        return ( rc );
    }
    if ( e92_strcont ( "QUIT" , command , 1 ) )
    {
        return ( - 1001 );
    }

    if ( e92_strcont ( "CR_SHM" , command , 6 ) )
    {
        rc = e92_create_shm_from_file ( argc , argv , argn, act_arg );
        return ( rc );
    }

    rc = e92_get_key_and_map ( );

/*PTS 1120283 --------------------------------------------------
 * Requests which can already be executed by the console thread.
 *
 * Requests will be parsed again by the called routine.
  --------------------------------------------------------------*/

    if ( e92_strcont ( "TRACE_FLUSH" , command , 5 )    ||
         e92_strcont ( "TRACELEVEL" , command , 6 )     ||  /* Setting trace level for XCons or Console Thread respectively (only for SLOW) */
         e92_strcont ( "LD_BAL_CHK" , command , 6 )     ||
         e92_strcont ( "DEBUGTASK" , command , 9 )      ||
         e92_strcont ( "TIME" , command , 2 )           ||
         e92_strcont ( "RESET" , command , 2 )          ||

         /* Requests only for test purposes */
         e92_strcont ( "TEST01" , command , 6 )         ||
         e92_strcont ( "TEST02" , command , 6 )         ||
         e92_strcont ( "TEST03" , command , 6 ))
    {
        if ( !RTECons_ProcessStandardRequest (argc ,argv) )
        {
            exit(1);
        }
        else
        {
            exit(0);
        }
    }


/* ----------------------------------------------------------- */

    if ( rc ) return ( rc );

    if ( e92_strcont ( "SHOW" , command , 1 ) )
    {
        rc = e92_dbshow ( argc , argv , argn, act_arg );
        return ( rc );
    }
    /* KWe
     * The following comands can execute only if the kernel is active
     */
    if ( ! e92_access_mode )
        {
        return ( 0 );
        }

    if ( e92_strcont ( "CLEAR" , command , 2 ) )
    {
        rc = e92_dbclear ( );

        *act_arg = argn;
        return ( rc );
    }
    if ( e92_strcont ( "STOP" , command , 3 ) )
    {
        rc = e92_dbstop ( );
        *act_arg = argn;

        return ( rc );
    }
    if ( e92_strcont ( "CANCEL" , command , 2 ) )
    {
        if ( argc <= argn )
        {
            MSGCD(( MSGNO(11987), "task index missing \n" ))
            return ( 101 );
        }
        p = argv [ argn ] ;
        if ( * p == 'T' || * p == 't' ) p ++ ;
        if ( * p == '\0' )
        {
            ++argn;
            if ( argc <= argn )
            {
                MSGCD(( MSGNO(11987), "task index missing \n" ))
                return ( 101 );
            }
            p = argv [ argn ] ;
        }
        taskid = atol ( p );
        argn ++ ;
        rc = e92_dbcancel ( taskid );
        *act_arg = argn;

        return ( rc );
    }
    if ( e92_strcont ( "CORE" , command , 2 ) )
    {
        if ( argc <= argn )
        {
            MSGCD(( MSGNO(11987), "index missing \n" ))
            return ( 101 );
        }
        p = argv [ argn ] ;
        i = 'T' ;
        switch ( * p )
        {
        case 'P' :
        case 'p' :
		i = 'P' ;
		/*FALLTHROUGH*/
        case 'T' :
        case 't' :
                p ++ ;
        }
        if ( * p == '\0' )
        {
            ++argn;
            if ( argc <= argn )
            {
                MSGCD(( MSGNO(11987), "index missing \n" ))
                return ( 101 );
            }
            p = argv [ argn ] ;
        }
        taskid = atol ( p );
        argn ++ ;
        rc = e92_dbcore ( i , taskid );
        *act_arg = argn;

        return ( rc );
    }
    if ( e92_strcont ( "PROFILE" , command , 2 ) )
    {
        if ( argc <= argn )
        {
            MSGCD(( MSGNO(11987), "thread index missing \n" ))
            return ( 101 );
        }
        p = argv [ argn ] ;
        if ( * p == 'P' || * p == 'p' ) p ++ ;
        if ( * p == '\0' )
        {
            ++argn;
            if ( argc <= argn )
            {
                MSGCD(( MSGNO(11987), "thread index missing \n" ))
                return ( 101 );
            }
            p = argv [ argn ] ;
        }
        taskid = atol ( p );
        argn ++ ;
        rc = e92_dbprofile ( taskid );
        *act_arg = argn;

        return ( rc );
    }

    if ( e92_strcont ( "DEBUGLEVEL" , command , 1 ) )
    {
        if ( argc <= argn )
        {
            MSGCD(( MSGNO(11987), "debuglevel missing \n" ))
            return ( 101 );
        }
        i = atoi ( argv [ argn ] );
        argn ++ ;
        rc = e92_dbdebuglevel ( i );
        *act_arg = argn;

        return ( rc );
    }

    if ( e92_strcont ( "KILL" , command , 1 ) )
    {
        if ( argc <= argn )
        {
            MSGCD(( MSGNO(11987), "task index missing \n" ))
            return ( 101 );
        }
        p = argv [ argn ] ;
        if ( * p == 'T' || * p == 't' ) p ++ ;
        if ( * p == '\0' )
        {
            ++argn;
            if ( argc <= argn )
            {
                MSGCD(( MSGNO(11987), "task index missing \n" ))
                return ( 101 );
            }
            p = argv [ argn ] ;
        }
        taskid = atol ( p );
        argn ++ ;
        rc = e92_dbkill ( taskid );
        *act_arg = argn;

        return ( rc );
    }
    if ( e92_strcont ( "MESSAGES" , command , 1 ) )
    {
        rc = e92_dbmessages ( );
        *act_arg = argn;
        return ( rc );
    }

    if ( e92_strcont ( "GETPARAM" , command , 6 ) )
    {
        rc = e92_getparam ( argc , argv , argn, act_arg );
        return ( rc );
    }
    if ( e92_strcont ( "PUTPARAM" , command , 6 ) )
    {
        rc = e92_putparam ( argc , argv , argn, act_arg );
        return ( rc );
    }
    if ( e92_strcont ( "TOP" , command , 2 ) )
    {
        rc = e92_dbtop ( argc , argv , argn, act_arg );
        return ( rc );
    }

#if 0
    if ( e92_strcont ( "TIME" , command , 2 ) )
    {
        rc = e92_time_measure ( argc , argv , argn, act_arg );
        return ( rc );
    }
    if ( e92_strcont ( "RESET" , command , 2 ) )
    {
        rc = e92_dbreset_cnt ( argc , argv , argn, act_arg );
        return ( rc );
    }
#endif
    if ( ! strcmp ( "ERRIOR" , command ) )
    {
        rc = e92_error_io ( sp2ioc_read , argc , argv , argn , act_arg );
        return ( 0 );
    }
    if ( ! strcmp ( "ERRIOW" , command ) )
    {
        rc = e92_error_io ( sp2ioc_write , argc , argv , argn , act_arg );
        return ( 0 );
    }
    if ( ! strcmp ( "PRIO_SV_W" , command ) )
    {
        rc = e92_prio_sv_w ( argc , argv , argn , act_arg );
        return ( 0 );
    }
    if ( e92_strcont ( XCONS_CMD_DEBUG_BREAK , command, XCONS_MIN_CMD_LEN_DEBUG_BREAK ) )
    {
        rc = e92_debugbreak ( argc , argv , argn , act_arg );
        return ( 0 );
    }

    MSGCD(( MSGNO(11987), "unknown command: '%s' \n" , command ))
    return ( 101 );
}


/*==========================================================================*/


static  int     e92_help ( )

{
  /* PTS 1107597 */
 printf  (XCONS_USAGE);

#if 0
pri ( "TIME [ ENABLE | DISABLE ] enable or disable time measurement\n" );
pri ( "CANCEL   index       cancel the command executed by task <index>\n" );
pri ( "KILL     index       kill the session of task <index>\n" );
pri ( "SHOW [ LONG | COMPRESS ]\n" );
pri ( "         objects     show statistics/states about the following objects:\n" );
pri ( "                     ALL       STORAGE   VERSIONS    STATE      \n" );
pri ( "                     PROCESSES REGIONS   IO   AIO    RTE        \n" );
/* F.H. */
pri ( "                     QUEUES    TASKS     SLEEP       SUSPENDS   \n" );
pri ( "                     T_STAT    T_REG     T_QUEUE                \n" );
pri ( "                     T_CNT [ SV | US | DW | T<taskindex> ]      \n" );
pri ( "                     ACT   [ SV | US | DW ]          RUN        \n" );
/*
pri ( "MESSAGES             display database console messages\n" );
*/
pri ( "CORE     [T] index   produce core dump for task <index>\n" );
/*
pri ( "PROFILE  [P] index   produce profile of thread <index>\n" );
 */
pri ( "CORE     P tid       produce core dump for thread <tid>\n" );
pri ( "DEBUGLEV level       set the debuglevel for the kernel\n" );
pri ( "RESET    obj_cnt     resets counter about the following objects:\n" );
pri ( "                     IO   T_CNT   REGIONS    ( ALL )  \n" );
pri ( "                     incl. local counters of any task \n" );
pri ( "ERRIOR   devno       force error at next read to devno \n" );
pri ( "ERRIOW   devno       force error at next write to devno \n" );
pri ( "START                start database\n" );
pri ( "STOP                 stop  database\n" );
pri ( "CLEAR                clear database\n" );
pri ( "DEBUGBREAK SET pos   set debugbreak position\n" );
pri ( "DEBUGBREAK CLEAR     clear debugbreak position\n" );
pri ( "DEBUGBREAK SHOW      show current debugbreak position\n" );
pri ( "!        command     execute shell command\n" );
pri ( "QUIT                 exit console driver\n" );
#endif

return ( 0 );
}


/*==========================================================================*/


static  int     e92_dbmessages ( )


{
    DBG1 (( MF__,"e92_dbmessages: called \n" ))
    MSGCD(( MSGNO(11999), "messages not yet implemented \n" ))
    DBG1 (( MF__,"e92_dbmessages: returning \n" ))
    return ( 0 );
}

/*==========================================================================*/
#define SHM_RD_WR 2

#if 0
static  int     e92_time_measure (
int                     argc ,
char                    ** argv ,
int                     argn ,
int                     *actual_arg )
{
        int                     i ;
        char                    * p ;
        char                    param [ MAXVFNLENGTH ] ;


    DBG1 (( MF__,"e92_time_measure: called \n" ))

    if ( e92_map_shm ( SHM_RDONLY ) > 1 ) return ( 2 );
    DBG1 (( MF__,"e92_time_measure: kgs access mode %d \n", e92_access_mode ))

    *actual_arg      = argn;
    argn += e92_test_interval ( argv, argc, argn );
    if ( argn < argc )
    {
        for ( ; argn < argc ; argn ++, ( *actual_arg ) ++ )
        {
            p = argv [ argn ] ;
            for ( i = 0 ; i < MAXVFNLENGTH && p [ i ] != '\0' ; i ++ )
            {
                /* Incompatibility in ctype of screenlib vs. libc */
                if ( islower ( p [ i ] ) )
                        param [ i ] = p [ i ] - 'a' + 'A' ;
                else    param [ i ] = p [ i ] ;
            }
            if ( i < MAXVFNLENGTH )
            {
                param [ i ] = '\0' ;
            }
            else
            {
                MSGCD(( MSGNO(11987), "parameter too long: '%s' \n" , p ))
                return ( 101 );
            }

            if ( e92_strcont ( "ENABLE" , param , 1 ) )
            {
              if ( ! XPARAM(fTimeMeasure) )
              {
                  struct  TASK_TYPE * tcb ;
                  for ( tcb = kgs->pFirstTaskCtrl ; tcb <= kgs->pLastTaskCtrl ; tcb ++ )
                  {
                      struct TASK_TYPE *nonRemappedTcb = MAP_TO_NON_REMAPPED_POINTER(struct TASK_TYPE *, tcb);
                      nonRemappedTcb->TaskStatRecResetPending = EO52_RESET_TASK_PARTIAL_PENDING;
                  }
                  e92_dbreset_sleeptime();
                  e92_dbreset_iocnt();
                  en92_NonRemappedKgs->XParam->fTimeMeasure = TRUE ;
              }
            }
            else
            if ( e92_strcont ( "DISABLE"       , param , 1 ) )
            {
              en92_NonRemappedKgs->XParam->fTimeMeasure = FALSE ;
            }
            else
            {
                MSGCD(( MSGNO(11987), "unknown time object '%s' \n", param ))
                return ( 2 );
            }
        }
    }

    DBG1 (( MF__,"e92_time_measure: returning \n" ))
    return ( 0 );
}

/*==========================================================================*/

static  int     e92_dbreset_cnt (
int                     argc ,
char                    ** argv ,
int                     argn ,
int                     *actual_arg )
{
        int                     i , cnt ;
        char                    * p ;
        char                    param [ MAXVFNLENGTH ] ;

static  int                     r_io        = 0 ;
static  int                     r_regions   = 0 ;
static  int                     r_dispcnt   = 0 ;
static  int                     r_sleeptime = 0 ;
static  int                     r_suspend   = 0 ;

    DBG1 (( MF__,"e92_resetcnt: called \n" ))

    e92_unmap_shm ( ) ;
    if ( e92_map_shm ( SHM_RD_WR ) > 1 || e92_access_mode != SHM_RD_WR )
	  return ( 2 );
    DBG1 (( MF__,"e92_dbreset: kgs access mode %d \n", e92_access_mode ))

    *actual_arg      = argn;
    argn += e92_test_interval ( argv, argc, argn );
    if ( argn < argc )
    {
        for ( ; argn < argc ; argn ++, ( *actual_arg ) ++ )
        {
            p = argv [ argn ] ;
            for ( i = 0 ; i < MAXVFNLENGTH && p [ i ] != '\0' ; i ++ )
            {
                /* Incompatibility in ctype of screenlib vs. libc */
                if ( islower ( p [ i ] ) )
                        param [ i ] = p [ i ] - 'a' + 'A' ;
                else    param [ i ] = p [ i ] ;
            }
            if ( i < MAXVFNLENGTH )
            {
                param [ i ] = '\0' ;
            }
            else
            {
                MSGCD(( MSGNO(11987), "parameter too long: '%s' \n" , p ))
                return ( 101 );
            }

            cnt = 0 ;
            if ( e92_strcont ( "ALL" , param , 3 ) )
            {
	        r_regions = r_dispcnt  = r_io = r_sleeptime = r_suspend = 1 ;
	        cnt ++;
   	    }
            if ( e92_strcont ( "T_CNT"       , param , 1 ) )
            {
                r_dispcnt     = 1 ;
                cnt ++ ;
            }
            if ( e92_strcont ( "REGIONS"     , param , 1 ) )
            {
                r_regions   = 1 ;
                cnt ++ ;
            }
            if ( e92_strcont ( "IO"     , param , 1 ) )
            {
                r_io   = 1 ;
                cnt ++ ;
            }
            if ( e92_strcont ( "SUSP"     , param , 1 ) )
            {
                r_suspend   = 1 ;
                cnt ++ ;
            }
            if ( e92_strcont ( "SLEEP"     , param , 1 ) )
            {
                r_sleeptime   = 1 ;
                cnt ++ ;
            }
            if ( e92_test_interval ( argv, argc, argn ) )
            {
                *actual_arg = argn;
                break;
            }
            if ( cnt < 1 )
            {
                MSGCD(( MSGNO(11987), "unknown reset object '%s' \n", param ))
                return ( 2 );
            }
            if ( cnt > 1 )
            {
                MSGCD(( MSGNO(11987), "abbreviation error, reset object '%s' \n", param ))
                return ( 2 );
            }
        }
    }
    if ( r_regions  ) e92_dbreset_regions   () ;
    if ( r_dispcnt  ) e92_dbreset_task_cnt  () ;
    if ( r_io       ) e92_dbreset_iocnt     () ;
    if ( r_suspend  ) e92_dbreset_suspend   () ;
    if ( r_sleeptime) e92_dbreset_sleeptime () ;

    DBG1 (( MF__,"e92_resetcnt: returning \n" ))
    return ( 0 );
}

/*==========================================================================*/

static  void    e92_dbreset_regions ( )
{
int                             i ;
struct DDB4_SEMA                * nonRemapped_curr_sema ;
struct  TASK_TYPE               * tcb ;

DBG1 (( MF__,"e92_dbreset_regions: called \n" )) ;

for ( i = 0 ; i < XPARAM(ulNoOfRegions) ; i ++ )
  if ( (nonRemapped_curr_sema = MAP_TO_NON_REMAPPED_POINTER(struct DDB4_SEMA *, & kgs->semaphore [ i ])) != 0 )
  {
    nonRemapped_curr_sema->get_count =
    nonRemapped_curr_sema->wait_count =
    nonRemapped_curr_sema->begexcl_tas_count =
    nonRemapped_curr_sema->endexcl_tas_count =
    nonRemapped_curr_sema->collision_count = 0 ;
/*
    nonRemapped_curr_sema->total_sec= curr_sema->total_ms = 0 ;
*/
  }
e92_dbreset_task_region_cnt () ;

DBG1 (( MF__,"e92_reset_regions: returning \n" )) ;
return ;
}

/*==========================================================================*/

static  void    e92_dbreset_task_region_cnt ( )

{
    struct  TASK_TYPE               * tcb ;
    int                             i ;

    for ( tcb = kgs->pFirstTaskCtrl ; tcb <= kgs->pLastTaskCtrl ; tcb ++ )
    {
        SAPDB_Int4 *nonRemappedTcb_reg_excl_cnt = MAP_TO_NON_REMAPPED_POINTER(SAPDB_Int4 *, tcb->reg_excl_cnt);
        SAPDB_Int4 *nonRemappedTcb_reg_coll_cnt = MAP_TO_NON_REMAPPED_POINTER(SAPDB_Int4 *, tcb->reg_coll_cnt);
        for ( i = 0 ; i < XPARAM(ulNoOfRegions) ; i ++ )
        {
            nonRemappedTcb_reg_excl_cnt[i] = nonRemappedTcb_reg_coll_cnt[i] = 0 ;
        }
    }
    DBG1 (( MF__,"e92_reset_regions: returning \n" )) ;
    return ;
}
/*==========================================================================*/


static  void    e92_dbreset_task_cnt ( )
{
  struct  TASK_TYPE               * tcb ;

  DBG1 (( MF__,"e92_dbreset_task_cnt: called \n" )) ;

  for ( tcb = kgs->pFirstTaskCtrl ; tcb <= kgs->pLastTaskCtrl ; tcb ++ )
  {
      struct TASK_TYPE *nonRemappedTcb = MAP_TO_NON_REMAPPED_POINTER(struct TASK_TYPE *, tcb);
      nonRemappedTcb->TaskStatRecResetPending = EO52_RESET_TASK_FULL_PENDING;
  }

  e92_dbreset_task_region_cnt () ;

  DBG1 (( MF__,"e92_reset_task_cnt: returning \n" )) ;
}

/*==========================================================================*/

static  void    e92_dbreset_sleeptime ( )
{
  ten50_UKT_Control *ukt;
  ten50_UKT_Control *nonRemapped_ukt;

  DBG1 (( MF__,"e92_dbreset_sleeptime: called \n" )) ;

  for ( ukt = kgs->first_kp ; ukt <= kgs->last_kp ; ukt ++ )
  {
      nonRemapped_ukt = MAP_TO_NON_REMAPPED_POINTER(ten50_UKT_Control *, ukt);
      nonRemapped_ukt->SleepStatRecResetPending = true;
  }

  DBG1 (( MF__,"e92_reset_sleeptime: returning \n" )) ;
}

/*==========================================================================*/

static  void    e92_dbreset_suspend ( )
{
int i ;
DBG1 (( MF__,"e92_dbreset_suspend: called \n" ))

    for ( i = 0; i < MX_SUSPEND_REASONS_EO00; ++i )
        en92_NonRemappedKgs->SuspendReason[i] = 0;

DBG1 (( MF__,"e92_dbreset_suspend: returning \n" ))
}

/*==========================================================================*/

static  void    e92_dbreset_iocnt ( )
{
    ten50_UKT_Control * ukt;
    struct DEV_PCB    * dev_pcb ;
    int i, j;

    DBG1 (( MF__,"e92_dbreset_iocnt: called \n" ))

    for ( ukt = kgs->first_kp ; ukt <= kgs->last_kp ; ukt ++ )
    {
        if ( ukt )
        {
            struct UKP_IO_STRUCT *nonRemapped_UKPIO = MAP_TO_NON_REMAPPED_POINTER(struct UKP_IO_STRUCT *, ukt->pSingleIO);
            for ( i = 0 ; i < kgs-> max_ukpio_devs ; i++ )
            {
                nonRemapped_UKPIO[i].ReadSelfIo.IOStat.TimeStat.ulCount = nonRemapped_UKPIO[i].WriteSelfIo.IOStat.TimeStat.ulCount = 0 ;
            }
        }
    }

    for ( i = 0 ; i < kgs->maxdevspaces ; i ++ )
      for ( j = 0 ; j < kgs->devpsets ; j++ )
        {
            dev_pcb = kgs->first_dev + i * kgs->devpsets + j ;
            if ( dev_pcb && dev_pcb->state != KT_INITIALIZED )
            {
                struct DEV_PCB * nonRemapped_dev_pcb = MAP_TO_NON_REMAPPED_POINTER(struct DEV_PCB *, dev_pcb);
                nonRemapped_dev_pcb->DevIOStat.ulMaxIOQueLen = 0;
                nonRemapped_dev_pcb->DevIOStat.ResetPending = true;
            }
        }
    DBG1 (( MF__,"e92_reset_iocnt: returning \n" )) ;
}
#endif
/*==========================================================================*/

SAPDB_Bool e92_checkForRunningProcesses ()
{
    tsp00_TaskId   dbPid;
    if ( en41GetPIDFromFile(dbname, &dbPid) != 0 )
	{
        return ( false );
    }
    if ( dbPid == 0 )
    {
        return false;
    }
    if( kill ( (pid_t) dbPid , 0 )  != 0
     && errno == ESRCH )
    {
        return false;
    }
    return true;
}

/*==========================================================================*/

static  int     e92_dbshow (
int                     argc ,
char                    ** argv ,
int                     argn ,
int                     *actual_arg )
{
        int                     i , cnt ;
        char                    * p ;
        char                    param [ MAXVFNLENGTH ] ;
        int                     tid         = -1 ;

static  int                     s_io          = 1 ;
static  int                     s_aio         = 1 ;
static  int                     s_processes   = 1 ;
static  int                     s_state_type  = SHOW_ALL_EO92 ;
static  int                     s_task_type   = 0 ;
static  int                     s_runnable    = 0 ;
static  int                     s_rwregions   = 0 ;
static  int                     s_regions     = 0 ;
static  int                     s_rte         = 0 ;
static  int                     s_dev_io      = 0 ;
static  int                     s_storage     = 0 ;
static  int                     s_versions    = 1 ;
static  int                     s_queues      = 0 ;
static  int                     s_taskstack   = 0 ;
static  int                     s_total_stat  = 0 ;
static  int                     s_task_stat   = 0 ;
static  int                     s_task_cnt    = 0 ;
static  int                     s_task_reg    = 0 ;
static  int                     s_task_que    = 0 ;
static  int                     s_sleeptime   = 0 ;
static  int                     s_sleepstat   = 0 ;
static  int                     s_state       = 0 ;
static  int                     s_susp        = 0 ;
static  int                     s_connections = 0;
static  int                     s_task_move   = 0;
static  int                     s_move_info   = 0;
static  int                     s_all = 0;


    DBG1 (( MF__,"e92_dbshow: called \n" ))

/* PTS 1107597 */
    printf ( XCONS_SERVERDB, dbname );

    argn += e92_get_show_long ( argv, argc, argn );
    *actual_arg      = argn;
    /*
     * KWe : Here we must increment argn
     *       if there are numeric arguments at the actual position
     *       of the command line.
     */
    argn += e92_test_interval ( argv, argc, argn );
    if ( argn < argc )
    {
        s_io = s_aio= 0 ;
        s_processes = 0 ;
        s_state_type= SHOW_ALL_EO92 ;
        s_task_type = SHOW_TT_ALL_EO92 ;
        s_runnable  = 0 ;
        s_rwregions = 0 ;
        s_regions   = 0 ;
        s_rte       = 0 ;
        s_dev_io    = 0 ;
        s_queues    = 0 ;
        s_taskstack = 0 ;
        s_task_stat = s_total_stat = s_task_cnt = s_task_reg = s_task_que = 0 ;
        s_sleeptime = 0 ;
        s_sleepstat = 0 ;
        s_state     = 0 ;
        s_susp      = 0 ;
        s_storage   = 0 ;
        s_versions  = 0 ;
		s_connections = 0 ;
        s_task_move   = 0 ;
        s_move_info   = 0 ;
        s_all         = 0 ;

        for ( ; argn < argc ; argn ++, ( *actual_arg ) ++ )
        {
            p = argv [ argn ] ;
            for ( i = 0 ; i < MAXVFNLENGTH && p [ i ] != '\0' ; i ++ )
            {
                /* Incompatibility in ctype of screenlib vs. libc */
                if ( islower ( p [ i ] ) )
                        param [ i ] = p [ i ] - 'a' + 'A' ;
                else    param [ i ] = p [ i ] ;
            }
            if ( i < MAXVFNLENGTH )
            {
                param [ i ] = '\0' ;
            }
            else
            {
                MSGCD(( MSGNO(11987), "parameter too long: '%s' \n" , p ))
                return ( 101 );
            }

            cnt = 0 ;
            if ( e92_strcont ( "ALL" , param , 3 ) )
            {
                s_all         = 1;
                s_io = s_aio  = 1;
                s_processes   = 1;
                s_regions     = 1;
                s_rte         = 1;
                s_rwregions   = 1;
                s_dev_io      = 1;
                s_queues      = 1;
                s_taskstack   = 1;
                s_task_stat   = 1;
                s_task_que    = 1;
                s_total_stat  = 1;
                s_sleeptime   = 1;
                s_sleepstat   = 1;
                s_state       = 1;
                s_susp        = 1;
                s_storage     = 1;
                s_versions    = 1;
                s_connections = 1;
                s_move_info   = 1;
                s_task_move   = 1;
                cnt ++ ;
            }
            if ( e92_strcont ( "IO"          , param , 1 ) )
            {
                s_io        = 1 ;
                cnt ++ ;
            }
            if ( e92_strcont ( "AIO"         , param , 2 ) )
            {
                s_aio       = 1 ;
                cnt ++ ;
            }
            if ( e92_strcont ( "PROCESSES"   , param , 1 ) ||
                 e92_strcont ( "TASKS"       , param , 2) )
            {
                s_processes = 1 ;
                if ( argc > argn + 1 )
                  if (e92_get_tt_to_show (argv[argn + 1] , &s_task_type))
                    ( *actual_arg ) = ++argn ;
                cnt ++ ;
            }
            if ( e92_strcont ( "REGIONS"     , param , 1 ) )
            {
                s_regions   = 1 ;
                cnt ++ ;
            }
            if ( e92_strcont ( "RTE"         , param , 2 ) )
            {
                s_rte       = 1 ;
                cnt ++ ;
            }
            if ( e92_strcont ( "DEV_IO"      , param , 2 ) )
            {
                s_dev_io    = 1 ;
                cnt ++ ;
            }
            if ( e92_strcont ( "ACTIVE"   , param , 2 ) )
            {
                s_processes  = 1 ;
                s_state_type = SHOW_ACTIVE_EO92 ;

                if ( argc > argn + 1 )
                  if (e92_get_tt_to_show (argv[argn + 1] , &s_task_type))
                    ( *actual_arg ) = ++argn ;

                cnt ++ ;
            }
            if ( e92_strcont ( "RUNNABLE"   , param , 2 ) )
            {
                s_processes = 1 ;
                s_state_type= SHOW_RUNNABLE_EO92 ;

                if ( argc > argn + 1 )
                  if (e92_get_tt_to_show (argv[argn + 1] , &s_task_type))
                    ( *actual_arg ) = ++argn ;

                cnt ++ ;
            }
            if ( e92_strcont ( "QUEUES"      , param , 1 ) )
            {
                s_queues    = 1 ;
                cnt ++ ;
            }
            if ( e92_strcont ( "TSTACK"      , param , 1 ) )
            {
                s_taskstack = 1 ;
                cnt ++ ;
            }
#ifdef MUELL
            if ( tid == -1 )
              tid = get_id ( param ) ;
#endif
            if ( e92_strcont ( "T_QUEUE"      , param , 2 ) )
            {
                s_task_que      = 1 ;
                cnt ++ ;
            }
            if ( e92_strcont ( "T_STAT"       , param , 2 ) )
            {
                s_task_stat     = 1 ;
                s_task_que      = 1 ;
                cnt ++ ;
            }
            if ( e92_strcont ( "TOTAL"        , param , 2 ) )
            {
                s_total_stat    = 1 ;
                cnt ++ ;
            }
            if ( e92_strcont ( "T_CNT"        , param , 2 ) )
            {   int ext_param = argc > argn + 1 ;
                if ( ext_param )
                  { int ext_param = e92_get_tt_to_show (argv[argn + 1], &s_task_type);
                    if ( !ext_param )
                       ext_param = ( tid = get_id ( argv[argn + 1] ) ) > 0 ;

                  }
                 if ( ext_param )
                   ( *actual_arg ) = ++argn ;
                 else
                   s_task_type = SHOW_TT_ALL_EO92 ;
                cnt ++ ;
                s_task_cnt      = 1 ;
            }
            if ( e92_strcont ( "T_REG"        , param , 2 ) )
            {
                s_task_reg      = 1 ;
                cnt ++ ;
            }
            if ( e92_strcont ( "SLEEP"        , param , 3 ) )
            {
                s_sleeptime      = 1 ;
                cnt ++ ;
            }
            if ( e92_strcont ( "SL_STAT"      , param , 3 ) )
            {
                s_sleepstat      = 1 ;
                cnt ++ ;
            }
            if ( e92_strcont ( "STATE"       , param , 3 ) )
            {
                s_state     = 1 ;
                cnt ++ ;
            }
            if ( e92_strcont ( "STORAGE"     , param , 1 ) )
            {
                s_storage   = 1 ;
                cnt ++ ;
            }
            if ( e92_strcont ( "VERSIONS"    , param , 1 ) )
            {
                s_versions  = 1 ;
                cnt ++ ;
            }
            if ( e92_strcont ( "CONNECTIONS"    , param , 1 ) )
            {
                s_connections  = 1 ;
                cnt ++ ;
            }
            if ( e92_strcont ( "SUSP"    , param , 2 ) )
            {
                s_susp  = 1 ;
                cnt ++ ;
            }
            if ( e92_strcont ( "MOVEINFO"    , param , 6 ) )
            {
                s_move_info  = 1 ;
                cnt ++ ;
            }

            if ( e92_strcont ( "T_MOVE"    , param , 6 ) )
            {
                s_task_move  = 1 ;
                cnt ++ ;
            }
            if ( e92_strcont ( (char *) "RWREGIONS" , param , 2 ) )
            {
                if ( argc > argn + 1 )
                    if (0 == strcmp(argv[argn + 1] , "all"))
                        ( *actual_arg ) = ++argn ;
                    s_rwregions  = 1 ;
                    cnt ++ ;
            }
            /*
             * KWe : e92_interval returns the count of following parametes,
             *       which are digits ( only if there are one or two parameters.
             *       Bacause count (and repeat) only can be placed at the end
             *       of the parameter list, we can break now.
             */
            if ( e92_test_interval ( argv, argc, argn ) )
            {
                *actual_arg = argn;
                break;
/*                cnt ++; */
            }
            if ( cnt < 1 )
            {
                MSGCD(( MSGNO(11987), "unknown show object '%s' \n", param ))
                return ( 2 );
            }
            if ( cnt > 1 )
            {
                MSGCD(( MSGNO(11987), "abbreviation error, show object '%s' \n", param ))
                return ( 2 );
            }
        }
    }
    /* else use last show commands */

    if ( s_state     )
	{
	    if ( e92_map_shm ( SHM_RDONLY ) > 1 )
		{
			pri ( "The database state is OFFLINE\n" );
			return 0;
		}
		else
		{
            /* First check if there are running processes at all. If there are no running processes, the state is OFFLINE, */
            /* no matter what the kgs says. */
            if(!e92_checkForRunningProcesses())
            {
			    pri ( "The database state is OFFLINE\n" );
		    }
		    else
		    {
                e92_dbshow_state     () ;
            }
		}
	}
    else
	{
	    if ( e92_map_shm ( SHM_RDONLY ) > 1 )
		{
			pri ( "The database state is OFFLINE\n" );
			return ( 2 );
		}
	}

    if ( s_versions  ) e92_dbshow_versions  () ;
    if ( s_processes )
    {
        e92_dbshow_processes (s_state_type, s_task_type) ;
    }
    if ( s_rte       ) e92_dbshow_rte       () ;
    if ( s_dev_io    ) e92_dbshow_dev_io    () ;
    if ( s_io        ) e92_dbshow_io        () ;
    if ( s_aio       ) e92_dbshow_aio       () ;
    if ( s_sleeptime ) e92_dbshow_sleeptime () ;
    if ( s_sleepstat ) e92_dbshow_sleepstat () ;
    if ( s_task_stat ) e92_dbshow_task_stat (tid) ;
    if ( s_total_stat) e92_dbshow_total_stat() ;
    if ( s_task_que  ) e92_dbshow_task_que  (tid) ;
    if ( s_task_cnt  ) e92_dbshow_task_cnt  (tid,s_task_type) ;
    if ( s_task_reg  ) e92_dbshow_task_reg  () ;
    if ( s_queues    ) e92_dbshow_queues    () ;
    if ( s_taskstack ) e92_dbshow_taskstack () ;
    if ( s_susp      ) e92_dbshow_suspend   () ;
    if ( s_regions   ) e92_dbshow_regions   () ;
    if ( s_storage   ) e92_dbshow_storage   () ;
    if ( s_connections ) e92_dbshow_connections () ;

    /* every new console statement goes here.... */
    if (( s_task_move ) || ( s_move_info ) || ( s_rwregions   ))
    {
        if (s_all)
        {
            /* Force to not repeat the commands on the new console because this is done already here */
            RTECons_ProcessStandardRequest(4 ,argv);
        }
        else
        {
            RTECons_ProcessStandardRequest(argc ,argv);
            /* Avoid repetition because this is already done by the new console */
            *actual_arg = 0;
        }
    }

    DBG1 (( MF__,"e92_dbshow: returning \n" ))
    return ( 0 );
}

/*==========================================================================*/

#undef MF__
#define MF__ MOD__"e92_dbcancel"
static  int     e92_dbcancel (
PROCESS_ID              taskid )
{
    struct TASK_TYPE * nonRemapped_tcb ;

    DBGIN;

    if ( e92_map_shm ( SHM_RD_WR ) ) return ( 2 );
    DBG1 (( MF__,"kgs access mode %d \n", e92_access_mode ))

    if ( taskid < 1 || taskid > kgs-> ulNumOfTasks )
    {
        MSGCD(( MSGNO(11987), "task index out of range: %ld \n", (long) taskid ))
        MSGCD(( MSGNO(11987), "allowed range is 1 to %d \n", kgs-> ulNumOfTasks ))
        return ( 2 );
    }
    nonRemapped_tcb = MAP_TO_NON_REMAPPED_POINTER(struct TASK_TYPE *, kgs->pFirstTaskCtrl + taskid - 1);
    nonRemapped_tcb->rte_comm.to_cancel = 1 ;

    DBGOUT;
    return ( 0 );
}


/*==========================================================================*/

#undef MF__
#define MF__ MOD__"e92_dbcore"
static  int     e92_dbcore (
int type ,
PROCESS_ID                      id )
{
    int                     rc ;
    pid_t                   pid ;

    DBG1 (( MF__,"called,    type %c \n", type ))
    DBG1 (( MF__,"           id   %ld \n", id ))

    if ( e92_map_shm ( SHM_RD_WR ) ) return ( 2 );
    DBG1 (( MF__,"kgs access mode %d \n", e92_access_mode ))

    if ( type == 'T' )
    {
        if ( id < 1 || id > kgs-> ulNumOfTasks )
        {
            MSGCD(( MSGNO(11987), "task index out of range: %ld \n", id ))
            MSGCD(( MSGNO(11987), "allowed range is 1 to %d \n", kgs-> ulNumOfTasks ))
            return ( 2 );
        }
        en92_NonRemappedKgs->coretaskindex = id ;
        en92_NonRemappedKgs->corethreadindex = 0 ;
	}
    else
    {
        if ( id < 1 )
        {
            MSGCD(( MSGNO(11987), "thread index out of range: %ld \n", id ))
            MSGCD(( MSGNO(11987), "allowed range begins with 1\n" ))
            return ( 2 );
        }
        en92_NonRemappedKgs->coretaskindex = -1 ;
        en92_NonRemappedKgs->corethreadindex = id ;
	}

    pid = kgs-> pid;
    if ( pid > 1 )
    {
        MSGCD(( MSGNO(11987), "send SIG_FORK_AND_EXIT to pid %ld thread %d task %d\n",
                (long) pid, kgs->corethreadindex, kgs->coretaskindex ))
        rc = kill ( pid , SIG_FORK_AND_EXIT );
    }
    else
    {
        MSGCD(( MSGNO(11987), "won't kill pid %ld \n", (long) pid ))
    }

    if ( rc )
    {
        MSGCD(( MSGNO(11987), "cannot kill process pid %ld: %s \n",
					(long) pid , sqlerrs() ))
        return ( 2 );
    }

    DBGOUT;
    return ( 0 );
}

/*==========================================================================*/

#undef MF__
#define MF__ MOD__"e92_dbprofile"
static  int     e92_dbprofile (
PROCESS_ID id )
{
    int                     rc ;
    pid_t                   pid ;

    DBG1 (( MF__,"id   %ld \n", id ))

    if ( e92_map_shm ( SHM_RD_WR ) ) return ( 2 );
    DBG1 (( MF__,"kgs access mode %d \n", e92_access_mode ))

    en92_NonRemappedKgs->coretaskindex = 0 ;
    en92_NonRemappedKgs->corethreadindex = id ;

    pid = kgs-> pid;
    if ( pid > 1 )
    {
        MSGCD(( MSGNO(11987), "send PROFILE signal to pid %ld \n", (long) pid ))
        rc = kill ( pid , SIG_FORK_AND_EXIT );
    }
    else
    {
        MSGCD(( MSGNO(11987), "won't kill pid %ld \n", (long) pid ))
    }

    if ( rc )
    {
        MSGCD(( MSGNO(11987), "cannot kill process pid %ld: %s \n",
					(long) pid , sqlerrs() ))
        return ( 2 );
    }

    DBGOUT;
    return ( 0 );
}


/*==========================================================================*/


static  int     e92_dbdebuglevel (
int                             level )
{
    DBG1 (( MF__,"e92_dbdebuglevel: called \n" ))

    if ( e92_map_shm ( SHM_RD_WR ) ) return ( 2 );
    DBG1 (( MF__,"e92_dbdebuglevel: kgs access mode %d \n", e92_access_mode ))

    DBG1 (( MF__,"e92_dbdebuglevel: old debuglevel %d \n", kgs->debuglevel ))
    en92_NonRemappedKgs->debuglevel = level ;
    DBG1 (( MF__,"e92_dbdebuglevel: returning \n" ))
    return ( 0 );
}

/*==========================================================================*/
char	*en92_peer_pid (
struct TASK_TYPE	*tcb ,
char                *peer_pid_buffer)
{
    char			*ptr ;

    if ( ! tcb->connection )
    {
        return ( "-1" ) ;
    }

    if ( *(tcb->connection->ci_remote_pid) )
    {
    	ptr = (char *)tcb->connection->ci_remote_pid ;
    }
    else
    {
    	ptr = peer_pid_buffer;
	    (void) sp77sprintf ( ptr , 32, "%ld" , (long)
		    	tcb->connection->ci_peer_pid );
    }

    return ( ptr );
}

/*==========================================================================*/
char *en92_peer_node (
struct TASK_TYPE	*tcb )
{
    if ( tcb->connection )
      return ( (char *)tcb->connection->ci_peer_node );
    else
      return ( "" ) ;
}

/*==========================================================================*/
int	en92_has_remote_pid ( struct TASK_TYPE	*tcb )
{
    if ( tcb->connection
      && *(tcb->connection->ci_remote_pid) )
    	return ( TRUE );
    else
	    return ( FALSE );
}

/*==========================================================================*/
void	en92_kill_task (
struct TASK_TYPE	*tcb )
{
    int  rc ;
    union semun     arg ;
    struct TASK_TYPE * nonRemapped_tcb = MAP_TO_NON_REMAPPED_POINTER(struct TASK_TYPE *, tcb);
	if ( tcb->connection )
    {
        struct connection_info *nonRemapped_connection = MAP_TO_NON_REMAPPED_POINTER(struct connection_info *, tcb->connection);
        nonRemapped_connection->ci_state = CON_ABORTED ;
    }
    nonRemapped_tcb->rte_comm.to_cancel = 1 ;

    arg.val = 1 ;
    do {
      rc = semctl ( tcb->ukt->semid , 0 , SETVAL , SEMCTL_SETVAL(arg) );
    } while ( rc && errno == EINTR );
}

/*==========================================================================*/


static  int     e92_dbkill (
PROCESS_ID              pid )
{
    struct TASK_TYPE        * tcb ;

    DBG1 (( MF__,"e92_dbkill: called \n" ))

    if ( e92_map_shm ( SHM_RD_WR ) ) return ( 2 );
    DBG1 (( MF__,"e92_dbkill: kgs access mode %d \n", e92_access_mode ))

    if ( pid < 1 || pid > kgs-> ulNumOfTasks )
    {
        MSGCD(( MSGNO(11987), "task index out of range: %ld \n", (long) pid ))
        MSGCD(( MSGNO(11987), "allowed range is 1 to %d \n", kgs-> ulNumOfTasks ))
        return ( 2 );
    }
    tcb = kgs-> pFirstTaskCtrl + pid - 1 ;
    en92_kill_task ( tcb );

    DBG1 (( MF__,"e92_dbkill: returning \n" ))
    return ( 0 );
}


/*==========================================================================*/


static  int     e92_dbstop ( )


{
    int                     rc ;
    char                    line [ 256 ] ;


    DBG1 (( MF__,"e92_dbstop: called \n" ))

    spri ( line , "x_stop %s" , dbname );

    rc = system ( line );

    /* KWe
     * now, the kernel should not be active
     */
    e92_unmap_shm ( );

    DBG1 (( MF__,"e92_dbstop: returning \n" ))
    return ( rc );
}


/*==========================================================================*/


static  int     e92_dbstart ( )


{
    int                     rc ;
    char                    line [ 256 ] ;


    DBG1 (( MF__,"e92_dbstart: called \n" ))

    spri ( line , "x_start %s" , dbname );

    rc = system ( line );

    /* KWe
     * now we must
     *  - get the common_rte_key, and
     *  - map the shared memory segment
     */
    rc = e92_get_key_and_map ( );

    DBG1 (( MF__,"e92_dbstart: returning \n" ))
    return ( rc );
}


/*==========================================================================*/


static  int     e92_dbclear ( )


{
    int                     rc ;
    char                    line [ 256 ] ;


    DBG1 (( MF__,"e92_dbclear: called \n" ))

    spri ( line , "x_clear %s" , dbname );

    rc = system ( line );

    /* KWe
     * now, the kernel should not be active
     */
    e92_unmap_shm ( );

    DBG1 (( MF__,"e92_dbclear: returning \n" ))
    return ( rc );
}


/*==========================================================================*/


static  int     e92_shell (
int                     argc ,
char                    ** argv ,
int                     argn )
{
    int                     lgt , rc ;
    char                    line [ 256 ] ;


    DBG1 (( MF__,"e92_shell: called \n" ))

    lgt = 0 ;
    for ( ; argn < argc ; argn ++ )
    {
            spri ( line + lgt , "%s " , argv [ argn ] );
            lgt += strlen ( line + lgt );
    }
    if ( lgt == 0 ) return ( 0 );

    rc = system ( line );

    DBG1 (( MF__,"e92_shell: returning \n" ))
    return ( rc );
}


/*==========================================================================*/


static  void    e92_dbshow_io ()

{
ten50_UKT_Control *ukt;
int                             i , j;
SAPDB_UInt8                     rcnt , wcnt ;
SAPDB_UInt8                     rPageCnt , wPageCnt ;
SAPDB_UInt8                     tot_read = 0, tot_write = 0;
SAPDB_UInt8                     tot_rPageCnt = 0, tot_wPageCnt = 0;
struct DEV_PCB                  * dev_pcb ;

DBG1 (( MF__,"e92_dbshow_io: called \n" ))

/* PTS 1105128 */
pri ( "\n" );
pri ( "%-32.32s %3.3s %10.10s %10.10s %10.10s %10.10s\n",
        "Devspace", "No.", "Read(s)", "RPages", "Write(s)","WPages" );

for ( i = 0 ; i < kgs->maxdevspaces ; i ++ )
{
    rcnt = wcnt = 0 ;
    rPageCnt = wPageCnt = 0 ;

    for ( j = 0 ; j < kgs->devpsets ; j++ )
    {
        dev_pcb = kgs->first_dev + i * kgs->devpsets + j ;
        if ( dev_pcb->state != KT_INITIALIZED &&
             kgs->dev0.dev[i].devspace[0] != '\0' &&
             !dev_pcb ->DevIOStat.ResetPending )
        {
            rcnt += dev_pcb->DevIOStat.DevRead.IOStat.TimeStat.ulCount;
            wcnt += dev_pcb->DevIOStat.DevWrite.IOStat.TimeStat.ulCount;
            rPageCnt += dev_pcb->DevIOStat.DevRead.ulPageCount;
            wPageCnt += dev_pcb->DevIOStat.DevWrite.ulPageCount;
        }
    }

    for ( ukt = kgs->first_kp ; ukt <= kgs->last_kp ; ukt ++ )
    {
        if ( !ukt->pSingleIO[i].selfIOStatisticResetPending )
        {
            rcnt += ukt->pSingleIO[i].ReadSelfIo.IOStat.TimeStat.ulCount;
            wcnt += ukt->pSingleIO[i].WriteSelfIo.IOStat.TimeStat.ulCount;
            rPageCnt += ukt->pSingleIO[i].ReadSelfIo.ulPageCount;
            wPageCnt += ukt->pSingleIO[i].WriteSelfIo.ulPageCount;
        }
    }

    tot_read += rcnt; 
    tot_write += wcnt;
    tot_rPageCnt += rPageCnt; 
    tot_wPageCnt += wPageCnt;
    
    if ( rcnt || wcnt )
    {
        pri ( "%-32.32s %3d %10" SAPDB_UINT8_FORMAT " %10" SAPDB_UINT8_FORMAT " %10" SAPDB_UINT8_FORMAT " %10" SAPDB_UINT8_FORMAT "\n",
	      kgs->dev0.dev[i].devspace , i , rcnt , rPageCnt, wcnt, wPageCnt) ;
    }
}

	pri ( "%-37.37s%10" SAPDB_UINT8_FORMAT " %10" SAPDB_UINT8_FORMAT " %10" SAPDB_UINT8_FORMAT " %10" SAPDB_UINT8_FORMAT "\n",
              "total I/O", tot_read, tot_rPageCnt, tot_write, tot_wPageCnt) ;

DBG1 (( MF__,"e92_dbshow_io: returning \n" ))
}


/*==========================================================================*/

static  void    e92_dbshow_aio ()

{

SAPDB_UInt8                     tot_read = 0, tot_write = 0;
struct DEV_PCB                  * dev_pcb ;
int                             devno ;
static int                      first = TRUE ;

for ( devno = kgs->maxdevspaces, dev_pcb = kgs->first_asdev;
      dev_pcb <= kgs->last_dev;
      dev_pcb ++, devno++ )
  { if ( dev_pcb->state != KT_INITIALIZED )
    { if ( first )
        {
          first = FALSE ;
          pri ( "\n" );
          pri ( "%6.6s  %-28.28s %5.5s %10.10s %10.10s %10.10s\n",
                  "Pid","Devspace", "Index", "Read(s)",  "Write(s)" , "Queue" );
        }
        pri ( "%6ld  %-28.28s %5d %10" SAPDB_UINT8_FORMAT " %10" SAPDB_UINT8_FORMAT " %6d (%d)\n",
               (long) dev_pcb->tid ,
               kgs->dev0.dev[devno].devspace ,
               devno ,
               dev_pcb->DevIOStat.ResetPending ? (SAPDB_UInt8)0 : dev_pcb ->DevIOStat.DevRead.IOStat.TimeStat.ulCount ,
               dev_pcb->DevIOStat.ResetPending ? (SAPDB_UInt8)0 : dev_pcb ->DevIOStat.DevWrite.IOStat.TimeStat.ulCount ,
               dev_pcb ->DevIOStat.ulIOQueLen ,
               dev_pcb ->DevIOStat.ulMaxIOQueLen ) ;
        if ( !dev_pcb->DevIOStat.ResetPending )
        {
            tot_read  += dev_pcb ->DevIOStat.DevRead.IOStat.TimeStat.ulCount ;
            tot_write += dev_pcb ->DevIOStat.DevWrite.IOStat.TimeStat.ulCount ;
        }
    }
  }

if ( tot_read || tot_write )
  pri ( "        %-23.23s %10" SAPDB_UINT8_FORMAT " %10" SAPDB_UINT8_FORMAT " %10" SAPDB_UINT8_FORMAT "\n",
        "total I/O", tot_read + tot_write, tot_read , tot_write ) ;
}

/*==========================================================================*/


static  void    e92_dbshow_processes (
int tasks_to_show ,
int ranking_type  )
{
	struct TASK_TYPE                * tcb ;
	time_t                          delta_time ;
	int                             rc, num_connect_wait_tasks = 0 ;
	static int                      first = TRUE ;
	char peer_pid_buffer[32];
	teo00_Uint1                     ShowId ;
	char                            *Hptr ;

    char                            szWaitingIndex[20];
    char                            szWaitingItem[20];
    char                            szApplPid[14];
    char                            szTimeOut[20];
    char                            szTid[12];

	DBG1 (( MF__,"e92_dbshow_processes: called \n" ))

	if ( first )
	{
		first = FALSE ;
		pri ( XCONS_SHOW_TASK_HEADER );
	}
    else
    {
        pri ( "\n" );
    }

	for ( tcb = kgs->pFirstTaskCtrl ; tcb <= kgs->pLastTaskCtrl ; tcb ++ )
	{
		if ( e92_show_long || IS_TASK_TO_SHOW_EO92 (tcb->type, 
                                                    tcb->state, 
                                                    tcb->state_reason,
                                                    tcb->TaskOMSStat.OMSCallActive, 
                                                    tcb->is_in_queue,
                                                    ranking_type, 
                                                    tasks_to_show))   /* PTS 1116933 */
		{
			sp77sprintf(szTid, sizeof(szTid), "%ld", (long) tcb->taskThreadId );
            sp77sprintf(szApplPid, sizeof(szApplPid), "%s%s",
                en92_peer_pid(tcb, peer_pid_buffer),
				en92_has_remote_pid(tcb) ? "*" : "" );

            szTimeOut[0] = 0;
            if ( tcb->state == TSK_VRECEIVE_EO00 )
            {
                long timeout = en921GetTaskCommandTimeOut( tcb->pTaskObject ) - kgs->current_time;
                if ( timeout > 32767 )
                {
                    strncpy (szTimeOut, XCONS_SHOW_TASK_NO_TIMEOUT, 10);
                }
                else if ( timeout < 0 )
                {
                    strcpy (szTimeOut, "0");
                }
                else
                {
                    sp77sprintf(szTimeOut, sizeof(szTimeOut), "%ld", timeout);
                }
            }
            else
            {
                sp77sprintf(szTimeOut, sizeof(szTimeOut), "%ld", tcb->ulDynPrio);
            }

            szWaitingIndex[0] = 0;
            if ( tcb->type == TT_US_EO00 && (tcb->state == TSK_VSUSPEND_EO00 || tcb->state == TSK_VWAIT_EO00 ) )
            {
                if ( tcb->lWaitForTask > 0 )
                {
                    sp77sprintf(szWaitingIndex, sizeof(szWaitingIndex), "%ld", (long)tcb->lWaitForTask );
                }
            } else if ( tcb->state == TSK_IO_READ_EO00 || tcb->state == TSK_IO_WRITE_EO00 )
            {
                sp77sprintf(szWaitingIndex, sizeof(szWaitingIndex), "%ld", (long)tcb->lWaitForDevno );
            }
            else if ( tcb->state == TSK_ENTERRWREGION_EO00 ) 
            {
                sp77sprintf(szWaitingIndex,sizeof(szWaitingIndex),"(%c)",
                    tcb->fTaskIsWaitingForExclusiveAccess ? 'w' : 'r');
            }
            else if ( tcb->state == TSK_YIELDING_ON_SPINLOCK_EO00 ) 
            {
                sp77sprintf(szWaitingIndex,sizeof(szWaitingIndex),"%s", &tcb->NameOfSpinlockYieldingFor[0]);
            }
            else
            {
                if ( tcb->sid > 0 )
                {
                    sp77sprintf(szWaitingIndex, sizeof(szWaitingIndex), "%ld", (long)tcb->sid );
                }
            }

                /* PTS 1115172 */
            szWaitingItem[0] = 0;
            if ( (tcb->type == TT_US_EO00 || tcb->type == TT_SV_EO00 || tcb->type == TT_GC_EO00) /* PTS 1130342 */
                && tcb->state == TSK_VSUSPEND_EO00 )
            {
                if ( tcb->lockedPageNo >= 0 && tcb->lockedPageNo != MAX_INT4_SP00 )
                {
                    sp77sprintf(szWaitingItem, sizeof(szWaitingItem), "%ld", (long)tcb->lockedPageNo );
                }
                else if ( tcb->lWaitForRoot >= 0 && tcb->lWaitForRoot != MAX_INT4_SP00 )
                {
                    sp77sprintf(szWaitingItem, sizeof(szWaitingItem), "%ld", (long)tcb->lWaitForRoot );
                }
            }
            else if ( tcb->state == TSK_ENTERRWREGION_EO00 ) 
            {
                sp77sprintf(szWaitingItem,sizeof(szWaitingItem),"%d",
                    tcb->sid);
            }
            else if ( tcb->state == TSK_VRECEIVE_EO00 )
            {
                sp77sprintf(szWaitingItem,sizeof(szWaitingItem),"%ld",
                    (long)(kgs->current_time - tcb->CmdWaitTime) );
            }

            pri ( XCONS_SHOW_TASK_INFO,
					tcb->index,
					tcb->ukt->index, /* PTS 1111201 */
                    szTid,
                    en921GetTaskDisplayName (tcb->pTaskObject, false),
                    szApplPid,
					tcb->is_in_queue == TASK_IS_IN_RUN_QUEUE_EO00 ? '*' : ' ' ,
					eo92GetTaskStateName (tcb->state, tcb->state_reason) ,
					tcb->TaskOMSStat.OMSCallActive ? '&' : ' ', /* CR 1106969 */
					tcb->rte_comm.to_cancel ? '!' : ' ' ,
					tcb->prio_flag ? '0' + tcb->prio_flag + tcb->save_prio : ' ' ,
					szTimeOut,
					tcb->begexcl_count ,
                    szWaitingIndex,
                    szWaitingItem,
          ( !tcb->ukt->SleepStatRecResetPending
          && tcb->ukt->UKTRunningStat.TimeStat.ulCount > 0 ) ? tcb->ukt->UKTRunningStat.TimeStat.ulCount : 0,
					(tcb->ukt->state == KT_SLEEPING) ? 's' : 'r' );

        }
		else
		  if ( tcb->type == TT_US_EO00 )
			num_connect_wait_tasks ++;

	}

	if ( tasks_to_show == SHOW_ALL_EO92 )
		if ( num_connect_wait_tasks )
			if ( num_connect_wait_tasks > 1 )
				pri ( "      %5d Tasks are in State 'Connect wait'\n",
							 num_connect_wait_tasks );
			else
				pri ( "      %5d Task is in State 'Connect wait'\n",
							 num_connect_wait_tasks );

	DBG1 (( MF__,"e92_dbshow_processes: returning \n" ))
}


/*==========================================================================*/


static  void    e92_dbshow_regions ( )

{
int                             i, rc;
int                             Exclusive = 0;
struct DDB4_SEMA                * curr_sema ;
struct N_QUEUE                  * lquu ;
float                           CollisionRate ;

DBG1 (( MF__,"e92_dbshow_regions: called \n" ))


pri ( "%5s  %-8.8s %10.10s     %10.10s %10.10s\n",
      "Index", "Region", "Get-Cnt", "avg_time", "tot_time");

pri ( "\n" );
pri ( "%5s  %-8.8s  %-5.5s %10.10s %10.10s %10.10s %8.8s %4.4s   %s\n",
      "Index", "Region", "Owner", "Get-Cnt", "Tas-Cnt","Coll.", "Waits",
      "Excl","Coll % " );

for ( i = 0 ; i < XPARAM(ulNoOfRegions) ; i ++ )
{
    curr_sema = & kgs->semaphore [ i ];
	CollisionRate =(!curr_sema->get_count || !curr_sema->collision_count) ? 0.00 :
		           (float)curr_sema->collision_count*100.0 /
             	   (float)curr_sema->get_count ;

#ifdef NMP
	Exclusive = curr_sema->semaliste.sem_exclusive[0].sl_lock ;
#else
# if SUN && SVR4 || SOLARIS
	Exclusive = 7777 ; /* mutex */
# else
	Exclusive = curr_sema->semaliste.sem_exclusive [0] ;
# endif
#endif

    if ( curr_sema->semaname [ 0 ] == '\0' ) break ;

    if ( curr_sema->owner )
    {
	pri ( "%5d  %-8.8s  T%-4d %10u %10u %10u %8d %4d %6.2f %%\n",
	    i + 1 ,
	    curr_sema->semaname ,
	    curr_sema->owner->index ,
	    curr_sema->get_count ,
	    curr_sema->begexcl_tas_count + curr_sema->endexcl_tas_count,
	    curr_sema->collision_count ,
	    curr_sema->wait_count ,
        Exclusive,
        CollisionRate ) ;
    }
    else
    {
	pri ( "%5d  %-8.8s  %-5.5s %10u %10u %10u %8d %4d %6.2f %%\n",
	    i + 1 ,
	    curr_sema->semaname ,
	    "" ,
	    curr_sema->get_count ,
	    curr_sema->begexcl_tas_count + curr_sema->endexcl_tas_count,
	    curr_sema->collision_count ,
	    curr_sema->wait_count ,
        Exclusive,
        CollisionRate ) ;
    }

    for ( lquu = curr_sema->semaliste.last ;
          lquu && lquu->taskid ;
          lquu = lquu->next )
    {
        pri ( "%44.44s waiting: T%-4d\n", "" , lquu->taskid->index );
    }
}

pri ( "\nAlone :\n" );
pri ( "Count : %d   \tCollisions : %d\n" ,
        kgs->alone_count , kgs->alone_collision ) ;
DBG1 (( MF__,"e92_dbshow_regions: returning \n" ))
}


/*==========================================================================*/


static  void    e92_dbshow_rte ( )

{
int                             i ;
int                             j,rc ;
struct DEV_PCB                  * dev_pcb ;
char                            * p ;
tsp00_C64c                      cluster ;
int                             uktIndex ;
ten50_UKT_Control               * ukt;
struct TASK_TYPE                * tcb;
struct DOUBLY_LINKED            * curr_quu;
char                              szThreadName[100];

DBG1 (( MF__,"e92_dbshow_rte: called \n" ))


if ( en92_ShmIsMemoryCopy )
{
    pri ( "\n" );
    pri( "KGS memory copy @%p, mapped @%p, original @%p\n",
        kgs,
        en92_NonRemappedKgs,
        en92_NonRemappedKgs->this_kgs
        );
}
pri ( "\n" );
pri ( "Special Threads:\n" );
pri ( "%6.6s %-11.11s\n",
	"UNIX", "Threads" );
pri ( "%6.6s %-11.11s\n",
	"tid", "name" );

pri ( "%6d %-11.11s\n", kgs->coord.tid     , "COORDINATOR" );
pri ( "%6d %-11.11s Now %lu = %s", kgs->clockTid, "CLOCK", (SAPDB_ULong)kgs->current_time, ctime(&kgs->current_time) );
pri ( "%6d %-11.11s\n", kgs->console.tid   , "CONSOLE    " );
if ( KGS-> timer.state == KT_RUNNING )
{
    pri ( "%6d %-11.11s did %lu sleeps. Now running\n", kgs->timer.tid     , "TIMER      ", KGS->timerSleepIntervals );
}
else
{
    pri ( "%6d %-11.11s did %lu sleeps. Now sleeping until %lu for %ld s\n", kgs->timer.tid     , "TIMER      "
        , KGS->timerSleepIntervals, KGS->timerCurrentWakeupTime, KGS->timerCurrentSleepInterval);

}
pri ( "%6d %-11.11s\n", kgs->requestor.tid , "REQUESTOR  " );
pri ( "%6d %-11.11s\n", kgs->dev0.tid      , "DEV0       " );
pri ( "\n" );
pri ( "User Kernel Threads:\n" );
pri ( "%6.6s %-7.7s  %10.10s %10.10s %7.7s  %6.6s  %5.5s   %s\n",
	"UNIX", "Thread", "Dispatch","TaskSwitch","Command","Active","Total","Task" );
pri ( "%6.6s %-7.7s  %10.10s %10.10s %7.7s  %6.6s  %5.5s   %s\n",
	"tid", "name", "counter", "counter", "counter", "Tasks"  , "Tasks" , "cluster" );


uktIndex = 1;
for ( ukt = kgs->first_kp ; ukt <= kgs->last_kp ; ukt ++ )
{
    char localOutputBuffer[sizeof(cluster) + 100];
    cluster [ 0 ] = '\0' ;
    i = eo92CreateTaskcluster ( ukt,
                                cluster, sizeof ( cluster ) ) ;

    /* PTS 1117471 %qd needed for disp_counter (, taskSwitches) and cmd_counter !!! */
    sp77sprintf(localOutputBuffer, sizeof(localOutputBuffer),
        "%6ld UKT%-3d %10qd %10qd %9qd  %6d  %5d   %s\n",
                (long) ukt->tid ,
                uktIndex ,
                ukt->disp_counter ,
                ukt->taskSwitches ,
                ukt->cmd_counter ,
                ukt->activeTasks ,
                i ,
                cluster );
    pri ( "%s\n", localOutputBuffer );
    uktIndex++;
}

pri ( "\n" );
pri ( "I/O via UKT and Device Processes:\n" );
pri ( "%6.6s %-6.6s %-28.28s %3.3s %10.10s %10.10s %8.8s\n",
    "UNIX", "Thread", "Devspace", "Dev",   "Read",  "Write" , " Queue");
pri ( "%6.6s %-6.6s %-26.26s %5.5s %10.10s %10.10s %4.4s %4.4s\n",
    "tid",  "name", "name",     "index", "count", "count", "len", "max");
/* borrow 2 chars from 'Dev name' for 'Dev Index' column */

uktIndex = 1;
for ( ukt = kgs->first_kp ; ukt <= kgs->last_kp ; ukt ++ )
{
    for ( i = 0 ; i < kgs-> max_ukpio_devs ; i++ )
    {
        if ( !ukt->pSingleIO[i].selfIOStatisticResetPending && (ukt->pSingleIO[i].ReadSelfIo.IOStat.TimeStat.ulCount || ukt->pSingleIO[i].WriteSelfIo.IOStat.TimeStat.ulCount) )
        {
	    pri ( "%6ld UKT%-3d %-28.28s %3d %10" SAPDB_UINT8_FORMAT " %10" SAPDB_UINT8_FORMAT "   -- (--)\n",
                (long) ukt->tid ,
                uktIndex,
                kgs->dev0.dev[i].devspace ,
                i ,
                ukt->pSingleIO[i].ReadSelfIo.IOStat.TimeStat.ulCount ,
                ukt->pSingleIO[i].WriteSelfIo.IOStat.TimeStat.ulCount ) ;
        }
    }
    uktIndex++;
}


for ( i = 0 ; i < kgs->maxdevspaces ; i ++ )
{
    for ( j = 0 ; j < kgs->devpsets ; j++ )
    {
        dev_pcb = kgs->first_dev + i * kgs->devpsets + j ;

        if ( dev_pcb->state != KT_INITIALIZED )
        {
	    pri ( "%6ld %-6.6s %-28.28s %3d %10" SAPDB_UINT8_FORMAT " %10" SAPDB_UINT8_FORMAT " %4d (%d)\n",
                (long) dev_pcb->tid ,
                "DEV" ,
                kgs->dev0.dev[i].devspace ,
                i ,
               dev_pcb->DevIOStat.ResetPending ? (SAPDB_UInt8)0 : dev_pcb ->DevIOStat.DevRead.IOStat.TimeStat.ulCount ,
               dev_pcb->DevIOStat.ResetPending ? (SAPDB_UInt8)0 : dev_pcb ->DevIOStat.DevWrite.IOStat.TimeStat.ulCount ,
                dev_pcb ->DevIOStat.ulIOQueLen ,
                dev_pcb ->DevIOStat.ulMaxIOQueLen ) ;
        }
    }
}

DBG1 (( MF__,"e92_dbshow_rte: returning \n" ))
}


/*==========================================================================*/

static  void    e92_dbshow_dev_io ( )

{
int                             i , j , got_sv , got_us ;
struct DEV_PCB                  * dev_pcb ;
char                            * p ;
tsp00_C64c                      cluster ;
int                             uktIndex ;
static int                      first = TRUE ;


DBG1 (( MF__,"e92_dbshow_dev_io: called \n" ))

for ( i = 0 ; i < kgs->maxdevspaces ; i ++ )
{
    for ( j = 0 ; j < kgs->devpsets ; j++ )
    {
        dev_pcb = kgs->first_dev + i * kgs->devpsets + j ;

        if ( dev_pcb->state != KT_INITIALIZED &&
            !dev_pcb->DevIOStat.ResetPending &&
            ( dev_pcb->DevIOStat.DevRead.IOStat.TimeStat.ulCount  > 0  ||
             dev_pcb->DevIOStat.DevWrite.IOStat.TimeStat.ulCount > 0 ) )
        {
            if ( first )
              { first = FALSE ;
                pri ( "\n" );
                pri ( "I/O via Device Processes:\n" );
                pri ( "%6.6s %-28.28s %10.10s %10.10s %10.10s %10.10s\n",
                    "UNIX", "Devspace", "Read ", "avg_read" , "Write", "avg_write");
                pri ( "%6.6s %-28.28s %10.10s %10.10s %10.10s %10.10s\n",
                    "tid",  "name    ", "count", "    time" , "count", "     time");
              }
            pri ( "%6ld %-28.28s %10" SAPDB_UINT8_FORMAT " %10.4f %10" SAPDB_UINT8_FORMAT " %10.4f\n",
                (long) dev_pcb->tid ,
                kgs->dev0.dev[i].devspace ,
                dev_pcb ->DevIOStat.DevRead.IOStat.TimeStat.ulCount ,
                AVG_TIME( dev_pcb->DevIOStat.DevRead.IOStat.TimeStat),
                dev_pcb ->DevIOStat.DevWrite.IOStat.TimeStat.ulCount ,
                AVG_TIME( dev_pcb->DevIOStat.DevWrite.IOStat.TimeStat) ) ;
        }
    }
}

DBG1 (( MF__,"e92_dbshow_dev_io: returning \n" ))

}


/*==========================================================================*/


static  void    e92_dbshow_queues ( )

{
int                             ix , first , outside ;
struct  IOC_QUEUE               * ioc ;
struct  IOR_QUEUE               * ioq ;
struct  XXX_QUEUE_HEAD          * lxx ;
struct  XXX_QUEUE               * lxxx ;
struct  TASK_TYPE               * tcb ;
int                             print_first; /* KWe */
ten50_UKT_Control               * ukt;


DBG1 (( MF__,"e92_dbshow_queues: called \n" ))


pri ( "\nQueues:\n" );
for ( ukt = kgs->first_kp ; ukt <= kgs->last_kp ; ukt ++ )
{
    pri ( "UKTx tid %6ld  \n", (long) ukt->tid );

    print_first = 1;
    for ( ix = kgs-> ulNumOfUKTs - 1 ; ix >= 0 ; ix -- )
    {
        if ( print_first )
        {
            pri ( "    xxx_queues\n" );
            print_first = 0;
        }
        lxx = ukt->xxx [ ix ];
        if ( lxx == (struct XXX_QUEUE_HEAD *) 0 ) continue ;
        pri ( "        from UKT tid %ld / overflows: %d \n",
                (long) kgs->first_kp[ix].tid , lxx->num_overflows );
        for ( lxxx = lxx->xx_work , first = TRUE , outside = FALSE ;
              (lxxx != lxx->xx_work) || first ;
              lxxx = lxxx->next , first = FALSE )
        {
            if ( lxxx == lxx->xx_request ) outside = TRUE ;
            if ( ! lxxx->task )
            {
                if ( ! outside ) pri ( "            NULL\n" );
                continue ;
            }
            if ( outside ) pri ( "            OUTSIDE:\n" );
              pri ( "            task T%d, from T%d, function 0x%08lx (%d)\n",
                                 lxxx->task->index , lxxx->FromTask->index,
                                 lxxx->to_execute , lxxx->func_param ) ;
        }
    }
/* KWe     pri ( "    ioc_queue\n" ); */
    print_first = 1;
    for ( ioc = ukt->ioc_queue.last , first = TRUE , outside = FALSE ;
          (ioc != ukt->ioc_queue.last) || first ;
          ioc = ioc->next , first = FALSE )
    {
        struct DOUBLY_LINKED *Req;
        struct TASK_TYPE     *pTask;
        if ( print_first )
        {
            pri ( "    ioc_queue\n" );
            print_first = 0;
        }
        if ( ioc == ukt->ioc_queue.first ) outside = TRUE ;

        Req = ioc->request;

        if ( 0 == Req || 0 == (pTask = Req->taskid) )
        {
            if ( ! outside ) pri ( "        NULL\n" );
            continue ;
        }

        if ( outside ) pri ( "        OUTSIDE:\n" );
        pri ( "        request for task T%-5d '%s'\n",
              pTask->index,
 	          e38_req_name ( Req->req_type ) );
    }

/* KWe     pri ( "    com_queue\n" ); */
    en921ShowCommqueueRemapped( ukt->pTaskSchedulerObject
                              , en92_ShmIsMemoryCopy
                              , ((SAPDB_Byte const *)0) +  en92_SharedSegmentStart
                              , ((SAPDB_Byte const *)0) +  en92_SharedSegmentEnd
                              , ((SAPDB_Byte       *)0) + (en92_SharedSegmentStart + en92_KgsRemappingDistance)
                              );

    print_first = 1;
    for ( ix=0; ix< ukt->RunQueueLen ; ix++ )
      { if ( print_first )
        {
            pri ( "    run_queue: %d\n", ukt->RunQueueLen);
            print_first = 0;
        }
      if ( (tcb = ukt->RunQueue[ix].Task) )
        { char szFromTask[20] ;
          struct DOUBLY_LINKED *Req      = ukt->RunQueue[ix].Req  ;
          struct TASK_TYPE     *FromTask = ukt->RunQueue[ix].FromTask ;
          if ( FromTask )
            sprintf ( szFromTask, "(T%d)", FromTask->index ) ;
          else
            szFromTask[0] = '\0' ;
          if ( Req )
            pri ( "        task T%d (%s), dyn_prio %d, %s %s\n",
                    tcb->index, eo92GetTaskStateName (tcb->state,0), tcb->ulDynPrio,
	  	    e38_req_name ( Req->req_type ), szFromTask );
          else
            pri ( "        task T%d (%s), dyn_prio %d %s\n",
                    tcb->index, eo92GetTaskStateName (tcb->state,0), tcb->ulDynPrio,
	  	    szFromTask );
        }
    }
}

DBG1 (( MF__,"e92_dbshow_queues: returning \n" ))
}


/*==========================================================================*/


static  void    e92_dbshow_taskstack ( )

{
struct  TASK_TYPE               * tcb ;


DBG1 (( MF__,"e92_dbshow_taskstack: called \n" ))

pri ( "\nTask Stacks:\n" );
for ( tcb = kgs->pFirstTaskCtrl ; tcb <= kgs->pLastTaskCtrl ; tcb ++ )
{
    pri ( "task T%d\n", tcb->index );
	pri ( "    type         %s         ", en921GetTaskDisplayName (tcb->pTaskObject, true));
	pri ( "    state        %s\n",        eo92GetTaskStateName ( tcb->state,0 ) );
    pri ( "    stack start  0x%08lx       ", tcb->StackChunkStart );
    pri ( "    stack end    0x%08lx\n",      tcb->StackChunkEnd );
    pri ( "    stack size   0x%08lx (%ld)\n", tcb->stack_size , tcb->stack_size );
    pri ( "    guard begin  0x%08lx       ", tcb->GuardPageBegin );
    pri ( "    guard end    0x%08lx\n",      tcb->GuardPageEnd );

    /* PTS 1110953 */
    pri ( "    stack actual 0x%08lx       ", tcb->pCurrStackPointer );
    pri ( "    stack maxuse 0x%08lx\n",      tcb->maxStackPointer );
    pri ( "    command max  %8ld          ", (long)tcb->commandMaxStack );
    pri ( "    total max  %8ld\n",           (long)tcb->totalMaxStack );
}

DBG1 (( MF__,"e92_dbshow_taskstack: returning \n" ))
}

/*==========================================================================*/

/*================================================*/
static  void    e92_dbshow_total_stat ( )
/*================================================*/
{
  struct  TASK_TYPE               * tcb ;
  int i ;
  unsigned long tot_commands =0L, tot_disp_calls = 0L,
                tot_vwait    =0L, tot_vsleep     = 0L, tot_vsusp    = 0L,
                tot_prio     =0L, tot_prio_f_oth = 0L,
                tot_self_io  =0L, tot_dev_io     = 0L,
                tot_usr_read =0L, tot_usr_write  = 0L,
                tot_self_susp_cnt    = 0L,
		tot_endexcl_tascnt = 0L, tot_reg_wait_cnt  = 0L,
                tot_reg_acc  =0L, tot_begexcl_tascnt = 0L, tot_reg_coll = 0L;

  for ( tcb = kgs->pFirstTaskCtrl ; tcb <= kgs->pLastTaskCtrl ; tcb ++ )
    { tot_disp_calls+= tcb->disp_counter    ;
      tot_commands  += tcb->QueStat.Resetable.ulCOMQueCount;
      tot_vwait     += tcb->TaskStateStat.VwaitStat.TimeStat.ulCount;
      tot_vsleep    += tcb->TaskStateStat.VsleepStat.TimeStat.ulCount;
      tot_vsusp     += tcb->TaskStateStat.VsuspStat.TimeStat.ulCount;
      tot_prio      += tcb->PrioStat.Resetable.ulTotalCount ;
      tot_prio_f_oth+= tcb->PrioStat.Resetable.ulOtherUkt ;
      tot_self_susp_cnt    += tcb->self_susp_cnt    ;
      tot_self_io   += tcb->TaskStateStat.SelfIOReadStat.IOStat.TimeStat.ulCount +
                       tcb->TaskStateStat.SelfIOWriteStat.IOStat.TimeStat.ulCount;
      tot_dev_io    += tcb->TaskStateStat.DevIOReadStat.IOStat.TimeStat.ulCount +
                       tcb->TaskStateStat.DevIOWriteStat.IOStat.TimeStat.ulCount;
      if ( tcb->type == TT_US_EO00 )
        { tot_usr_read  += tcb->TaskStateStat.SelfIOReadStat.IOStat.TimeStat.ulCount +
                           tcb->TaskStateStat.DevIOReadStat.IOStat.TimeStat.ulCount ;
          tot_usr_write += tcb->TaskStateStat.SelfIOWriteStat.IOStat.TimeStat.ulCount +
                           tcb->TaskStateStat.DevIOWriteStat.IOStat.TimeStat.ulCount ;
        }
    }

  for ( i = 0 ; i < XPARAM(ulNoOfRegions) ; i ++ )
   {
     tot_reg_acc            += kgs->semaphore [ i ].get_count ;
     tot_begexcl_tascnt     += kgs->semaphore [ i ].begexcl_tas_count ;
     tot_endexcl_tascnt     += kgs->semaphore [ i ].endexcl_tas_count ;
     tot_reg_wait_cnt       += kgs->semaphore [ i ].wait_count ;
     tot_reg_coll           += kgs->semaphore [ i ].collision_count ;
   }
    pri ( "\nTotal counter statistics:\n") ;
    pri ( "=========================\n\n") ;
    pri ( "%16s %-10u  %30s %-10u \n",
           "total_disp_calls    :", tot_disp_calls,
           "total_commands      :", tot_commands);

    pri ( "%16s %-10u  %30s %-10u\n%16s %-10u \n",
           "total_state_vwait   :", tot_vwait ,
           "total_state_vsuspend:", tot_vsusp ,
           "total_state_vsleep  :", tot_vsleep );

    if ( tot_prio || tot_prio_f_oth )
    pri ( "%16s %-10u  %30s %-10u \n",
           "total_prio          :", tot_prio ,
           "total_prio_from_oth :", tot_prio_f_oth );

    pri ( "%16s %-10u  %30s %-10u\n",
           "total_user_read     :", tot_usr_read ,
           "total_user_write    :", tot_usr_write );

    pri ( "%16s %-10u  %30s %-10u\n",
           "total_self_io       :", tot_self_io ,
           "total_dev_io        :", tot_dev_io  );

    pri ( "%16s %-10u  %30s %-10u\n%16s %-10u  %30s %-10u\n",
           "total_reg_access    :", tot_reg_acc ,
           "begexcl_tas_coll.   :", tot_begexcl_tascnt ,
           "total_reg_collision :", tot_reg_coll ,
           "endexcl_tas_coll.   :", tot_endexcl_tascnt ) ;

    pri ( "%16s %-10u  %30s %-10u\n",
           "total_reg_wait      :", tot_reg_wait_cnt ,
           "total_self_susp_cnt :", tot_self_susp_cnt );

    pri ( XCONS_SHOW_TOTAL_SYMBOL_RESOLUTION_LOCKS,
           XCONS_SHOW_TOTAL_SYMBOL_RESOLUTION_LOCK_CNT, kgs->symbolResolutionLockCount );

  return ;
}

/*================================================*/
static  void    e92_dbshow_task_stat ( int tid )
/*================================================*/
{
  struct  TASK_TYPE               * tcb = kgs->pFirstTaskCtrl ;

  if ( tid > 0 && tid <= kgs->pLastTaskCtrl->index )
    {
      print_task_state_cnts ( &tcb[tid-1] ) ;
      print_task_region_access ( &tcb[tid-1], FALSE ) ;
     }
  else
  for ( ; tcb <= kgs->pLastTaskCtrl ; tcb ++ )
    if ( tcb->disp_counter )
      {
        print_task_state_cnts ( tcb ) ;
        print_task_region_access ( tcb, FALSE ) ;
      }
}

/*================================================*/
static  void    e92_dbshow_task_cnt  (
int tid ,
int task_type )
{
  struct  TASK_TYPE               * tcb = kgs->pFirstTaskCtrl ;

  if ( tid > 0 && tid <= kgs->pLastTaskCtrl->index )
      print_task_state_cnts ( & tcb [ tid - 1 ] ) ;
  else
  for ( ; tcb <= kgs->pLastTaskCtrl ; tcb ++ )
    if ( (task_type == SHOW_TT_ALL_EO92 || tcb->type == task_type ) &&
          tcb->disp_counter )
      print_task_state_cnts ( tcb ) ;
}

/*=============================================================*/
static  void    print_task_prio_stat  (
struct  TASK_TYPE               * tcb )
{
  pri ( "%16s %-8d     %40s %-8d \n",
       "prio_total    :", tcb->PrioStat.Resetable.ulTotalCount ,
       "prio_from_oth :", tcb->PrioStat.Resetable.ulOtherUkt );

  return ;
}

/*================================================*/
static void print_task_state_cnts (
struct  TASK_TYPE               * tcb )
{
  int i=0;
  int j=0;
  char headline[80] ;
  char peer_pid_buffer[32];
  teo52_TaskStateStatistic *pTaskStateStat = &tcb->TaskStateStat ;

  
  (void) strcpy ( headline, XCONS_INFO_LINE );
  spri ( &headline[20], "  T%-4d %s  ( pid = %6s ) ", tcb->index,
                           en921GetTaskDisplayName (tcb->pTaskObject, true),
                           en92_peer_pid(tcb, peer_pid_buffer) );
  (void) strncat ( headline, XCONS_INFO_LINE, 78-strlen(headline) ) ;
  pri ( "\n\n%s\n", headline );

  if ( en92_has_remote_pid(tcb) )
   pri ( XCONS_SHOW_TASK_COUNT_REM_NODE_AND_PID ,
                 XCONS_SHOW_TASK_COUNT_REM_NODE ,
                 en92_peer_node(tcb) ,
                 XCONS_SHOW_TASK_COUNT_REM_PID  ,
                 en92_peer_pid(tcb, peer_pid_buffer) );

  eo92PrintTaskOMSStatistics( &tcb->TaskOMSStat ) ;


  pri ( XCONS_SHOW_TASK_COUNT_DISP_COMMAND_CNT    ,
                XCONS_SHOW_TASK_COUNT_KT_DISP_CALL_CNT,
                tcb->disp_counter,
                XCONS_SHOW_TASK_COUNT_KT_COMMAND_CNT,
                tcb->QueStat.Resetable.ulCOMQueCount);

  pri ( XCONS_SHOW_TASK_COUNT_EXCL_SELF_SUSP_CNT  ,
                XCONS_SHOW_TASK_COUNT_KT_EXCLUSIVE_CNT ,
                tcb->ulBegExclCalls,
                XCONS_SHOW_TASK_COUNT_KT_SELF_SUSP_CNT ,
                tcb->self_susp_cnt );

  if ( tcb->totalResumerCount > 0 )
  {
      int j0 = MAX_RESUMER_PIDS;
      j = tcb->totalResumerCount;

      pri ( XCONS_SHOW_TASK_COUNT_RESUME_CNT,
          (int)tcb->resume_count, (int)tcb->totalResumerCount);
      while( (j-- > 0) && (j0-- > 0) )
      {
          pri ( " T%d", (int)tcb->resumerPid[j%MAX_RESUMER_PIDS]);
      }
      pri ( " ]\n" );
  }

  eo92PrintTaskCounts( (tsp00_Int4 ) XPARAM(fTimeMeasure), pTaskStateStat ) ;

  if ( tcb->TaskStateStat.ulDevQueueLen[0] > 0 )
     pri ( "%16s %-8d %17s %-8d %17s %-u\n",
        "dev_que_len_0 :" , tcb->TaskStateStat.ulDevQueueLen[0],
        "dev_que_len_1 :" , tcb->TaskStateStat.ulDevQueueLen[1],
        "dev_que_len>1 :" , tcb->TaskStateStat.ulDevQueueLen[2]);

  if ( tcb->PrioStat.Resetable.ulTotalCount )
    { void    print_task_prio_stat  () ;
      print_task_prio_stat  ( tcb ) ;
    }

  pri ( "%s\n", XCONS_INFO_LINE ) ;
}

/*================================================*/
static  void    e92_dbshow_task_reg  ()
/*================================================*/
{
  struct  TASK_TYPE               * tcb ;

  for ( tcb = kgs->pFirstTaskCtrl ; tcb <= kgs->pLastTaskCtrl ; tcb ++ )
     print_task_region_access( tcb, TRUE ) ;
}

/*=============================================================*/
static  void    print_task_region_access  (
struct  TASK_TYPE               * tcb ,
int pri_headline )
{
  struct  DDB4_SEMA               * curr_sema ;
  int     i , first = TRUE;
  SAPDB_Char const *              displayName;


  displayName = en921GetTaskDisplayName (tcb->pTaskObject, true);

  for ( i = 0 ; i < XPARAM(ulNoOfRegions) ; i ++ )
    { curr_sema = & kgs->semaphore [ i ];
      if ( curr_sema->semaname [ 0 ] == '\0' ) break ;
      if ( curr_sema->get_count != 0 && tcb->reg_excl_cnt[i] != 0 )
        {
          if ( first )
            { first = FALSE ;
              if ( pri_headline )
                pri ( "\n\n%s\n T%-4d  %s     ( region-access )\n%s\n", XCONS_INFO_LINE,
                     tcb->index, 
                     displayName,
                     XCONS_INFO_LINE ) ;
              pri ( "%3s  %-8.8s  %s    %s\n%3s  %8s  %s    %s\n",
                    "Inx", "Region  ", "           Get-Counts         ",
                                     "      Collision-Counts    ",
                    "   ", "      ", "    Total /     Task   / percent",
                                        " Total /   Task   / percent");
            }
          pri ( "%3d  %-8.8s %10u / %10u /%6.2f %%  %8d / %8d /%6.2f %%\n",
                 i + 1 , curr_sema->semaname , curr_sema->get_count ,
                 tcb->reg_excl_cnt[i] ,
                 (!curr_sema->get_count || !tcb->reg_excl_cnt[i]) ? 0.00 :
                     (float)tcb->reg_excl_cnt[i]*100.0 /
                     (float)curr_sema->get_count,
                 curr_sema->collision_count , tcb->reg_coll_cnt[i] ,
                 (!tcb->reg_excl_cnt[i] || !tcb->reg_coll_cnt[i]) ? 0.00 :
                     (float)tcb->reg_coll_cnt[i]*100.0 /
                     (float)tcb->reg_excl_cnt[i]
               );
        }
    }
}

/*================================================*/
static  void    e92_dbshow_task_que  (
int tid )
{
  int     first = TRUE;
  struct  TASK_TYPE               *tcb, * f_tcb, *l_tcb ;

  if ( tid >= kgs->pFirstTaskCtrl->index && tid <= kgs->pLastTaskCtrl->index )
    f_tcb = l_tcb = & kgs->pFirstTaskCtrl[ tid-1 ] ;
  else
    f_tcb = kgs->pFirstTaskCtrl, l_tcb = kgs->pLastTaskCtrl ;

  for ( tcb = f_tcb ; tcb <= l_tcb ; tcb ++ )
    { teo52_TaskQueueStatistic *q_stat = & ( tcb->QueStat ) ;
      if ( q_stat->Resetable.ulUToUQueCount || q_stat->Resetable.ulUKTQueCount ||
           q_stat->Resetable.ulCOMQueCount )
        {
          if ( first )
            { first = FALSE ;
              pri ("\n%s\n%4s %8s  %9s  %9s\n%s\n",
                   XCONS_INFO_LINE, "Task ", "U2U", "UKT", "COM", XCONS_INFO_LINE);
            }
          pri ("T%-3d%10u %10u %10u\n",
               tcb->index, q_stat->Resetable.ulUToUQueCount    , q_stat->Resetable.ulUKTQueCount ,
                           q_stat->Resetable.ulCOMQueCount ) ;
        }
    }
}

#define TICKS_TO_TIME(_ticks, _sec, _msec) \
  _sec  = ( (unsigned )_ticks / CLOCKS_PER_SEC ) ;    \
  _msec = (((unsigned )_ticks % CLOCKS_PER_SEC) * 1000 / CLOCKS_PER_SEC ) ;

/*================================================*/
static  void    e92_dbshow_sleeptime ( )
/*================================================*/
{
    ten50_UKT_Control *ukt;
    int first = 1;
    int uktIndex = 1;
    char IdleTimeBuf[20];             /* ddd:hh:mm:ss: */
    char IOWaitTimeBuf[20];           /* ddd:hh:mm:ss: */

    if ( XPARAM(fTimeMeasure) )
    {
      SAPDB_UInt8 actMicroSeconds;

      for ( ukt = kgs->first_kp ; ukt <= kgs->last_kp ; ukt ++, uktIndex++ )
      {
          if ( !ukt->SleepStatRecResetPending
            && ukt->RunningStateEnterMicroSeconds > 0
            && ukt->UKTRunningStat.TimeStat.ulCount > 0 )
          {
#ifdef HAS_CONSOLE_PIPE
              actMicroSeconds = ToBeReplaced(); /* XXXX to be replaced with value returned ....  */
          /* If Console Pipe is used, local time measurement is no longer allowed */
#else
              struct timeval tv ;
              (void) gettimeofday ( &tv , (void *)0 );

            /* Rescale using stored system time value */
              actMicroSeconds = (tv.tv_sec * ((SAPDB_UInt8)1000000) + tv.tv_usec)
                              - ukt->SysIdleTimeMeasureBeginMicroSeconds
                              + ukt->IdleTimeMeasureBeginMicroseconds;
#endif
              if ( actMicroSeconds > ukt->IdleTimeMeasureBeginMicroseconds )
              {
                  SAPDB_UInt8 totalMeasuredMicroseconds = actMicroSeconds - ukt->IdleTimeMeasureBeginMicroseconds;
                  SAPDB_UInt8 IdleMicroSeconds = totalMeasuredMicroseconds
                                               - ukt->UKTRunningStat.TimeStat.ulAbsMicroSeconds
                                               - ukt->UKTIOWaitStat.TimeStat.ulAbsMicroSeconds;
                  /* This correction is not possible if no console pipe is used */
                  if ( KT_RUNNING == ukt->state )
                  {
                      IdleMicroSeconds -= (actMicroSeconds - ukt->RunningStateEnterMicroSeconds);
                  }

                  eo92_SecToDDHHMMSS( IdleMicroSeconds/1000000, IdleTimeBuf, sizeof(IdleTimeBuf) ) ;

                  eo92_SecToDDHHMMSS( ukt->UKTIOWaitStat.TimeStat.ulAbsMicroSeconds/1000000, IOWaitTimeBuf, sizeof(IOWaitTimeBuf) ) ;

                  if ( first )
                  {
                      first = 0;
                      printf ( XCONS_SHOW_UKT_SLEEP_STAT_HEADER ) ;
                  }

	              printf( XCONS_SHOW_UKT_SLEEP_STAT_INFO ,
                          uktIndex ,
                          ukt->UKTRunningStat.TimeStat.ulCount,
                          IdleTimeBuf,
                          ((double)IdleMicroSeconds) * 100. / (double)totalMeasuredMicroseconds,
                          ukt->UKTIOWaitStat.TimeStat.ulCount,
                          IOWaitTimeBuf,
                          ((double)ukt->UKTIOWaitStat.TimeStat.ulAbsMicroSeconds) * 100. / (double)totalMeasuredMicroseconds,
                          ukt->RunQueueLen );
              }
          }
      }
    }

    if ( !first ) pri ( "%s\n", XCONS_INFO_LINE );
}

/*================================================*/
static  void    e92_dbshow_sleepstat ( )
/*================================================*/
{
  ten50_UKT_Control *ukt;
  int first = TRUE, uktIndex = 1;
  for ( ukt = kgs->first_kp ; ukt <= kgs->last_kp ; ukt ++, uktIndex++ )
    if ( ukt->ulTotPrioWhenSleep  || ukt->ulTotVsuspWhenSleep ||
         ukt->ulTotVwaitWhenSleep )
      { if ( first )
          { first = FALSE ;
            pri ( "\n\n%s\n PID             %8s      %8s   %8s    %8s\n%s\n",
                   XCONS_INFO_LINE, "sleep_cnt", "sleep_prio",
                          "sleep_susp" ,"sleep_wait", XCONS_INFO_LINE);
          }
        pri ( "%-6ld UKT%-3d       %6" SAPDB_UINT8_FORMAT "        %8u     %8u      %8u\n",
             (long) ukt->tid ,uktIndex, 
          ( !ukt->SleepStatRecResetPending
          && ukt->UKTRunningStat.TimeStat.ulCount > 0 ) ? ukt->UKTRunningStat.TimeStat.ulCount : (SAPDB_UInt8)0,
              ukt->ulTotPrioWhenSleep, ukt->ulTotVsuspWhenSleep ,
              ukt->ulTotVwaitWhenSleep );
      }

   if ( !first ) pri ( "%s\n", XCONS_INFO_LINE );
}

/*================================================*/
static  void    e92_dbshow_suspend ( )
/*================================================*/

{
int i , tot_susp ;
DBG1 (( MF__,"e92_dbshow_suspend: called \n" ))

pri ( "\nList of suspend-reasons:\n") ;
pri ( "========================\n") ;
for ( i = 0 , tot_susp = 0 ; i < MX_SUSPEND_REASONS_EO00 ; i++ )
  tot_susp +=  kgs-> SuspendReason[i] ;

pri ( "\nTotal Suspends: %d\n\n", tot_susp ) ;

for ( i = 0; i < MX_SUSPEND_REASONS_EO00 ; i++ )
    if ( kgs-> SuspendReason[i] )
      {
        pri ( "%s : %7d ( %6.2f%% ) %s\n",
                     SuspendReasonText[i] . szShortReason ,
                     kgs-> SuspendReason[i] ,
                     (double) kgs-> SuspendReason[i] * 100.0 /
                     (double) tot_susp ,
                     SuspendReasonText[i] . szLongReason ) ;
      }

DBG1 (( MF__,"e92_dbshow_suspend: returning \n" ))
}


/*==========================================================================*/


static  void    e92_dbshow_state ( )

{
DBG1 (( MF__,"e92_dbshow_state: called \n" ))

pri ( "The database state is %s\n", e38_dbstate_name ( kgs->state ) );

DBG1 (( MF__,"e92_dbshow_state: returning \n" ))
}


/*==========================================================================*/


static  void    e92_dbshow_storage ()

{
ten50_UKT_Control *ukt;
tsp00_Int4                        stacksize ,
                                DynDataUnused, DynDataUsed ,
                                DynPoolUnused, DynPoolUsed ;
teo00_Longuint rc = NO_ERROR_EO001;

DBG1 (( MF__,"e92_dbshow_storage: called \n" ))

stacksize = 0 ;
for ( ukt = kgs-> first_kp ; ukt <= kgs-> last_kp ; ukt ++ )
{
    stacksize += ukt->stacksize ;
}
DynDataUnused = (tsp00_Int4) ( kgs-> dyndata_top - kgs-> dyndata_ptr ) ;
DynDataUsed   = (tsp00_Int4) ( kgs-> dyndata_ptr - kgs-> dyndata_bot ) ;
DynPoolUnused = (tsp00_Int4) ( kgs-> dynpool_top - kgs-> dynpool_ptr ) ;
DynPoolUsed   = (tsp00_Int4) ( kgs-> dynpool_ptr - kgs-> dynpool_bot ) ;

pri ( "\n" );
pri ( "Size   of  code            %8d KB\n",    kgs-> codesize / 1024 );
pri ( "Size   of  task stacks     %8d KB\n",    stacksize    / 1024 );
pri ( "Size   of  shared data     %8d KB\n",    kgs-> seg_size / 1024 );
pri ( "Used       shared dyn pool %8d KB\n",    DynPoolUsed  / 1024 );
pri ( "Unused     shared dyn pool %8d KB\n",    DynPoolUnused/ 1024 );
pri ( "Used       shared dyn data %8d Pages\n", DynDataUsed );
pri ( "Unused     shared dyn data %8d Pages\n", DynDataUnused );
pri ( "Number of  existing tasks  %8d\n",       kgs-> ulNumOfTasks );

DBG1 (( MF__,"e92_dbshow_storage: returning \n" ))
}


/*==========================================================================*/


static  void    e92_dbshow_versions ()

{
DBG1 (( MF__,"e92_dbshow_versions: called \n" ))


pri ( "Kernel Version  '%s'\n", kgs->KernelVersion );
pri ( "RTE    Version  '%s'\n", kgs->RTEVersion );

DBG1 (( MF__,"e92_dbshow_versions: returning \n" ))
}


/*==========================================================================*/

static  int     e92_map_shm ( int shmflag )
{
    int                             rc ;

    DBG1 (( MF__,"called,    shmflag %#o \n", shmflag ))

    if ( e92_access_mode ) /* already attached ? */
    {
        DBG1 (( MF__,"already attached, access mode %d \n", e92_access_mode ))
        if ( e92_access_mode == 1 && shmflag != SHM_RDONLY )
        {
		    MSGCD(( MSGNO(11111), "Permission denied \n" ))
            return ( 2 );
        }
        return ( check_kernel() );
    }

    /*
     *  Get shmid
     */
    e92_shmid = shmget ( common_rte_key , 0 , 0 );
    if ( e92_shmid < 0 )
    {
        DBG1 (( MF__,"shmget returned %d \n", e92_shmid ));
        DBG1 (( MF__,"error, %s\n", sqlerrs() ));
        if ( errno == ENOENT )
        {
	        MSGCD(( MSGNO(11107), "Kernel Shared Memory Segment not found \n" ));
	        MSGCD(( MSGNO(11109), "Database not running '%s' \n", dbname ));
            return ( 2 );
        }
        if ( errno == EPERM || errno == EACCES )
        {
	        MSGCD(( MSGNO(11110), "Cannot attach to Kernel Shared Memory Segment: \n" ));
	        MSGCD(( MSGNO(11111), "Permission denied \n" ));
            return ( 2 );
        }
        MSGCD(( MSGNO(11201), "kernel shared segment shmget error, %s\n", sqlerrs() ));
        return ( 2 );
    }
    DBG1 (( MF__,"e92_shmid %d \n", e92_shmid ))

    /*
     *  if the flag parameter to 'map_kgs' is -1,
     *  it tries to get write permission, else to get read permission
     */
    e92_access_mode = en92_MapKgsNotRemapped( e92_shmid, shmflag != SHM_RDONLY );

    if ( !e92_access_mode )
    {
        MSGCD(( MSGNO(11201), "kernel shared segment map error, %s\n", sqlerrs() ));
        return ( 2 );
    }

    DBG1 (( MF__,"attached at address 0x%08lx \n", (long) kgs ))

    if ( IsEqual_esp100 != sp100_CompareVersionIDs ( &kgs->RTEVersionID ) )
    {
		tsp00_Versionc currentVersion;
        extern char *XConsBuildNumber(void);
		
		sp100_GetVersionString( COMP_NAME_RTE_SP100,
								XConsBuildNumber,
								&currentVersion[0]);

	    MSGALL(( MSGNO(11112), "Incompatible version of running kernel and console!\n" ));
	    MSGALL(( MSGNO(11112), "Running kernel-version is: %s\n", kgs->RTEVersion ));
		MSGALL(( MSGNO(11112), "Actual console-version is: %s\n", currentVersion ));
	    return ( 2 );
	}

    if ( check_kernel() )
    {
        MSGCD(( MSGNO(11201), "Kernel mapping lost\n" ));
        return ( 1 );
    }

    DBG1 (( MF__,"returning, COORDINATOR exists \n" ))
    return ( 0 );
}

/*==========================================================================*/

#undef MF__
#define MF__ MOD__"e92_get_key_and_map"
static int e92_get_key_and_map ( )
{
    int         rc;

    /* KWe
     * Here, we
     * - get the common_rte_key, and
     * - map the shared memory segment.
     */

    DBGIN;

    if ( common_rte_key < 0 )
        common_rte_key = sql41_get_server_key ( dbname );

    if ( common_rte_key < 0 )
    {
        /*
         * The kernel is not active
         */
        if ( ENOENT != errno )
        {
            /*
             * The error is not "No such file or directory"
             */
		    MSGCD(( MSGNO(11108), "console: getserverid error, %s\n", sqlerrs() ));
            return ( 2 );
        }
        else
		{
			return ( 0 );
		}
    }

    e92_unmap_shm ( ) ;

    DBG1 (( MF__,"common_rte_key 0x%08lx \n", (long) common_rte_key ));

    rc = 0;

    if ( common_rte_key > 0 )
        rc = e92_map_shm ( SHM_RDONLY );

    if ( rc )
    {
		MSGCD(( MSGNO(11108), "console: kernel shared segment attach error %d key 0x%08lx\n",
                    rc, (long) common_rte_key ));
    }
    DBG1 (( MF__,"kgs access mode %d \n", e92_access_mode ));
    DBGOUT;
    return ( rc );
}

/*==========================================================================*/

#undef MF__
#define MF__ MOD__"e92_unmap_shm"
static void e92_unmap_shm ( )
{
    DBGIN;

    /* KWe
     * We can only detach the shm, if the e92_access_mode == 1 or 2
     */
    if ( e92_access_mode )
    {
        e92_access_mode = 0;
        en92_UnmapKgsNotRemapped () ;
    }

    DBGOUT;
}

/*
 *  ==========================================================================
 */

static int e92_get_show_long (
char            **argv ,
int             argc ,
int             argn )
{
    char        *p ;
    int         i;
    char        param [ MAXVFNLENGTH ] ;

    /* KWe
     * looks for a 'l' or a 's' at the actual command line position.
     * Returns 1, if there is a 'l' or a 's'.
     * Returns 0, if there is not a 'l' or a 's'.
     */
    DBG1 (( MF__,"e92_get_shlo: calld argc '%d' argn '%d'\n", argc, argn ))

    SAPDB_memset ( param, '\0', MAXVFNLENGTH );
    if ( argc > argn )
    {
        DBG1 (( MF__,"e92_get_shlo: argv[%d]='%s' \n", argn, argv[argn] ))
        p = argv [ argn ];
        for ( i = 0 ; i < MAXVFNLENGTH && p [ i ] != '\0' ; i ++ )
        {
            /* Incompatibility in ctype of screenlib vs. libc */
            if ( islower ( p [ i ] ) )
                    param [ i ] = p [ i ] - 'a' + 'A' ;
            else    param [ i ] = p [ i ] ;
        }
        DBG1 (( MF__,"e92_get_shlo: param = %s\n", param ))
        if ( e92_strcont ( "LONG" , param , 1 ) )
        {
            e92_show_long = 1;
            DBG1 (( MF__,"e92_get_shlo: show_long = %d\n", e92_show_long ))
            return ( 1 );
        }
        if ( e92_strcont ( "COMPRESS" , param , 1 ) )
        {
            e92_show_long = 0;
            DBG1 (( MF__,"e92_get_shlo: show_long = %d\n", e92_show_long ))
            return ( 1 );
        }
    }
    return ( 0 );
}

/*
 *  ==========================================================================
 */

static int e92_get_interval (
char            **argv ,
int             argc ,
int             argn )
{
    char        *p ;
    int         rc;

    /* KWe
     * looks at the actual command line position, if there are one
     * or two numeric parameters.
     * Returns the number of numeric paramters.
     * e92_interval were set, if the first parameter is numeric
     * e92_repeat   were set, if the second parameter is numeric
     */
    DBG1 (( MF__,"e92_get_interv: calld argc '%d' argn '%d'\n", argc, argn ))
    rc = 0;

    if ( e92_interval ) return ( 0 );

    if ( argc > argn )
    {
        DBG1 (( MF__,"e92_get_interv: argv[%d]='%s' \n", argn, argv[argn] ))
        p = argv [ argn ];
        if ( * p != '\0' && isdigit ( *p ) )
        {
            e92_interval = atoi ( p );
            e92_repeat_inc = 0;
            DBG1 (( MF__,"e92_get_interv: interval = %d\n", e92_interval ))
            rc ++;
        }
        else
        {
            DBG1 (( MF__,"e92_get_interv: interval '%c' must be numeric\n", *p ))
            return ( 0 );
        }
    }
    argn ++;
    if ( argc > argn )
    {
        DBG1 (( MF__,"e92_get_interv: argv[%d]='%s' \n", argn, argv[argn] ))
        p = argv [ argn ];
        if ( * p != '\0' && isdigit ( *p ) )
        {
            e92_repeat = atoi ( p );
            e92_repeat_inc = 1;
            DBG1 (( MF__,"e92_get_interv: repeat   = %d\n", e92_repeat ))
            rc ++;
        }
        else
        {
            DBG1 (( MF__,"e92_get_interv: repeat   '%c' must be numeric\n", *p ))
        }
    }
    return ( rc );
}

/*
 *  ==========================================================================
 */

static int e92_test_interval (
char            **argv ,
int             argc ,
int             argn )
{
    char        *p ;
    int         rc;

    /* KWe
     * looks at the actual command line position, if there are one
     * or two numeric parameters.
     * Returns the number of numeric paramters.
     * Returns 0, if more than 2 parameters are given.
     * Returns 0, if 2 parameters are given, and one of them isn'n
     *            numeric.
     */
    DBG1 (( MF__,"e92_test_interv: calld argc '%d' argn '%d'\n", argc, argn ))
    rc = 0;

    if ( argc > argn )
    {
        DBG1 (( MF__,"e92_test_interv: argv[%d]='%s' \n", argn, argv[argn] ))
        p = argv [ argn ];
        if ( * p == '\0' )
        {
            DBG1 (( MF__,"e92_test_interv: argv[%d] is empty \n", argn ))
            return ( 0 );
        }
        if ( ! isdigit ( *p ) )
        {
            DBG1 (( MF__,"e92_test_interv: argv[%d] is not numeric \n", argn ))
            return ( 0 );
        }
        else rc ++;
    }
    argn ++;
    if ( argc > argn )
    {
        DBG1 (( MF__,"e92_test_interv: argv[%d]='%s' \n", argn, argv[argn] ))
        p = argv [ argn ];
        if ( * p == '\0' )
        {
            DBG1 (( MF__,"e92_test_interv: argv[%d] is empty \n", argn ))
            return ( 0 );
        }
        if ( ! isdigit ( *p ) )
        {
            DBG1 (( MF__,"e92_test_interv: argv[%d] is not numeric \n", argn ))
            return ( 0 );
        }
        else rc ++;
    }
    argn ++;
    if ( argc > argn )
    {
        DBG1 (( MF__,"e92_test_interv: too many arguments: %d \n", argn ))
        return ( 0 );
    }
    return ( rc );
}

/*
 *  ==========================================================================
 */

static SIGNALFUNCTYPE   e92_catch_signal (
int             sig)
{
    DBG1 (( MF__,"e92_catch_sig: caught signal %d \n", sig ))
    (void) signal ( sig, SIG_IGN );

    switch ( sig )
    {
    case SIGINT :
                e92_repeat     = 0 ;
                (void) signal ( sig, e92_oldsigint );
                break;
    default     :
                DBG1 (( MF__,"e92_catch_sig: undefined signal %d \n", sig ))
                break;
    }
}

/*
 *  ==========================================================================
 */

static	int     check_kernel ( )

{
    int                         rc ;


    if ( kgs == (struct KERNEL_GLOBAL_SECTION *) 0 ||
         kgs == (struct KERNEL_GLOBAL_SECTION *) -1 )
    {
        return ( 1 );
    }
    rc = kill ( kgs->pid , 0 );
    if ( rc && errno == ESRCH )
    {
    	MSGCD(( MSGNO(11105), "WARNING: kernel not running '%s' \n", dbname ));
	    MSGCD(( MSGNO(11105), "WARNING: examining OLD DATA \n" ));
        return ( 0 );
    }
    return ( 0 );
}

/*
 *  ==========================================================================
 */

/*ARGSUSED*/
/*VARARGS1*/
/* void    pri ( char *text , ... )
{
        va_list                 args ;
        char                    line [ 256 ];


        va_start ( args, text );
        (void) vsprintf ( line , text , args );
        va_end ( args );
        (void) fputs ( line , stdout );
        (void) fflush ( stdout );
} */

/*
 * ===========================================================================
 */

static int region_to_sort = 0;

/*================================================*/
static void print_task_cnts (
struct TASK_TYPE *tcb )
/*================================================*/
{
  char buf[30] ;
  char peer_pid_buffer[32];
  char *nod = en92_peer_node ( tcb );

  if ( en92_has_remote_pid(tcb) )
      (void) sprintf(buf, "T%4d%7.7s:%6s",
            tcb->index,
            strlen ( nod ) > 7  ? nod + strlen ( nod ) - 7 : nod ,
            en92_peer_pid(tcb, peer_pid_buffer) ) ;
  else
    (void) sprintf(buf, "T%-4d      %8s",tcb->index, en92_peer_pid(tcb, peer_pid_buffer) );

  if ( tcb->disp_counter > 0 )
    { pri ("%s     %8u    %8d   %8" SAPDB_UINT8_FORMAT "     %8" SAPDB_UINT8_FORMAT " %8" SAPDB_UINT8_FORMAT "\n",
            buf, tcb->QueStat.Resetable.ulCOMQueCount,
            tcb->reg_excl_cnt[region_to_sort],
            tcb->TaskStateStat.SelfIOReadStat.IOStat.TimeStat.ulCount +
            tcb->TaskStateStat.DevIOReadStat.IOStat.TimeStat.ulCount  +
            tcb->TaskStateStat.SelfIOWriteStat.IOStat.TimeStat.ulCount+
            tcb->TaskStateStat.DevIOWriteStat.IOStat.TimeStat.ulCount ,
            tcb->TaskStateStat.VsuspStat.TimeStat.ulCount,
            tcb->TaskStateStat.VwaitStat.TimeStat.ulCount) ;
    }
}

/*
 * ===========================================================================
 */

static	int	e92_error_io (
int			direction ,
int			argc ,
char		**argv ,
int			argn ,
int			*act_arg )
{
    int				devno ;
    char			*ptr ;
    struct DEV_PCB		*devpcb ;


    if ( argc <= argn )
    {
	MSGCD(( MSGNO(11987), "devno missing \n" ))
	return ( 101 );
    }
    ptr = argv[argn];
    argn ++ ;
    *act_arg = argn ;
    devno = atoi ( ptr );
    devpcb = kgs-> first_dev + devno * kgs-> devpsets ;
    devpcb->set_io_error = 0x100 | direction ; /* DEV_READ is zero */
    return ( 0 );
}

/*
 * ===========================================================================
 */

static	int	e92_prio_sv_w (
int			argc ,
char			**argv ,
int			argn ,
int			*act_arg )
{
    int				prio_val ;
    char			*ptr ;
    struct  TASK_TYPE           *tcb ;


    if ( argc <= argn )
    {
	MSGCD(( MSGNO(11987), "prio value missing \n" ))
	return ( 101 );
    }
    ptr = argv[argn];
    argn ++ ;
    *act_arg = argn ;
    prio_val = atoi ( ptr );
    for ( tcb = kgs->pFirstTaskCtrl ; tcb <= kgs->pLastTaskCtrl ; tcb ++ )
      if ( tcb->type == TT_SV_EO00 && tcb->TaskStateStat.AsynIOWriteStat.IOStat.TimeStat.ulCount )
        tcb->prio_flag = prio_val ;
    return ( 0 );
}

/*
 * ===========================================================================
 */

static	int	e92_debugbreak (
int			argc ,
char			**argv ,
int			argn ,
int			*act_arg )
{
    int				   break_val ;
#define SUB_COMMAND_MAX 79
    char			   SubCommand[SUB_COMMAND_MAX+1];
    char			  *ptr ;
    struct  TASK_TYPE *tcb ;
    int                i;

    if ( e92_map_shm ( SHM_RD_WR ) ) return ( 2 );

    if ( argc <= argn )
    {
    	MSGCD(( MSGNO(11987), "missing SET or CLEAR\n" ))
	    return ( 101 );
    }
    ptr = argv[argn];

    for ( i = 0; ptr[i] != 0 && i < SUB_COMMAND_MAX; i++ )
    {
        SubCommand[i] = toupper(ptr[i]);
    }
    SubCommand[i] = 0;
    ptr = SubCommand;

    argn ++ ;
    *act_arg = argn ;

    if ( e92_strcont( XCONS_CMD_SET, ptr, XCONS_MIN_CMD_LEN_SET ) )
    {
        if ( argc <= argn )
        {
    	    MSGCD(( MSGNO(11987), "value missing\n" ))
    	    return ( 101 );
        }

        ptr = argv[argn];
        argn ++ ;
        *act_arg = argn ;

        break_val = atoi ( ptr );
        if ( break_val >= 10000 )
        {
            en92_NonRemappedKgs->ulSingleDebugBreakPos = (unsigned int)break_val;
            pri("DEBUGBREAK position set to %d\n", break_val );
        }
        else
        {
            pri("DEBUGBREAK position %d ignored ( < 10000 reserved )\n", break_val );
        }
    }
    else if ( e92_strcont( XCONS_CMD_CLEAR, ptr, XCONS_MIN_CMD_LEN_CLEAR ))
    {
        en92_NonRemappedKgs->ulSingleDebugBreakPos = 0;
    }
    else if ( e92_strcont( XCONS_CMD_SHOW, ptr, XCONS_MIN_CMD_LEN_CLEAR ))
    {
        if ( 0 == en92_NonRemappedKgs->ulSingleDebugBreakPos )
        {
            pri("No DEBUGBREAK position set\n");
        }
        else
        {
            pri("DEBUGBREAK position set to %d\n", en92_NonRemappedKgs->ulSingleDebugBreakPos );
        }
    }
    else
    {
    	MSGCD(( MSGNO(11987), "only SET or CLEAR allowed\n" ))
    	return ( 101 );
    }
    return ( 0 );
}

/*================================================*/
static  int     e92_dbtop (
int                     argc ,
char                    ** argv ,
int                     argn ,
int                     *actual_arg )
{
    int i = 0 , cnt;
    struct TASK_TYPE **tcb_ptr ;
    static int (* cmp_fkt)() = NULL ;
    char                    * p , param [ MAXVFNLENGTH ] ;

    DBG1 (( MF__,"e92_dbtop: called \n" ))

    region_to_sort = -1 ;
    if ( e92_map_shm ( SHM_RDONLY ) > 1 ) return ( 2 );
    DBG1 (( MF__,"e92_dbtop: kgs access mode %d \n", e92_access_mode ))

    argn += e92_get_show_long ( argv, argc, argn );
    *actual_arg      = argn;
    argn += e92_test_interval ( argv, argc, argn );
    if ( argn < argc )
    {
        for ( ; argn < argc ; argn ++, ( *actual_arg ) ++ )
        {
            p = argv [ argn ] ;
            for ( i = 0 ; i < MAXVFNLENGTH && p [ i ] != '\0' ; i ++ )
            {
                /* Incompatibility in ctype of screenlib vs. libc */
                if ( islower ( p [ i ] ) )
                        param [ i ] = p [ i ] - 'a' + 'A' ;
                else    param [ i ] = p [ i ] ;
            }
            if ( i < MAXVFNLENGTH )
            {
                param [ i ] = '\0' ;
            }
            else
            {
                MSGCD(( MSGNO(11987), "parameter too long: '%s' \n" , p ))
                return ( 101 );
            }

            cnt = 0 ;

            if (  region_to_sort  == -1 )
              region_to_sort = get_id ( param ) ;
            if ( e92_strcont ( "CMD"          , param , 2 ) )
            { cmp_fkt=cmp_cmd ;  cnt ++ ; }
            else
            if ( e92_strcont ( "VSUSPEND"          , param , 2 ) )
            { cmp_fkt=cmp_vsusp ;  cnt ++ ; }
            else
            if ( e92_strcont ( "VWAIT"          , param , 2 ) )
            { cmp_fkt=cmp_vwait ;  cnt ++ ; }
            else
            if ( e92_strcont ( "IO"          , param , 2 ) )
            { cmp_fkt=cmp_io ;  cnt ++ ; }
            else
            if ( e92_strcont ( "REGIONS"     , param , 2 ) )
              { cmp_fkt=cmp_reg ;
                cnt ++ ;
                if ( region_to_sort > 0 ) region_to_sort -- ;
                else                      region_to_sort = 6 ;
              }
            /*
             * KWe : e92_interval returns the count of following parametes,
             *       which are digits ( only if there are one or two parameters.
             *       Bacause count (and repeat) only can be placed at the end
             *       of the parameter list, we can break now.
             */
            if ( e92_test_interval ( argv, argc, argn ) )
            {
                *actual_arg = argn;
                break;
/*                cnt ++; */
            }
            if ( cnt < 1 )
            {
                MSGCD(( MSGNO(11987), "unknown show object '%s' \n", param ))
                return ( 2 );
            }
            if ( cnt > 1 )
            {
                MSGCD(( MSGNO(11987), "abbreviation error, show object '%s' \n", param ))
                return ( 2 );
            }
        }
    }

  if ( cmp_fkt == NULL )
    {
       MSGCD(( MSGNO(11987), "functionpointer 'cmp_fkt' not set! \n" ));
       return ( 2 );
    }

  if ( ALLOC_MEM_EO57( (void **)&tcb_ptr, kgs->ulNumOfTasks * sizeof ( struct TASK_TYPE *) )
        != NO_ERROR_EO001 )
  {
      struct TASK_TYPE *tcb ;
      for ( i=0, tcb = kgs->pFirstTaskCtrl ; tcb <= kgs->pLastTaskCtrl ; tcb ++, i++ )
        tcb_ptr [ i ] = tcb ;
      qsort ( tcb_ptr, kgs->ulNumOfTasks ,
              sizeof ( struct TASK_TYPE *), cmp_fkt ) ;
      pri ("\n%s\n%5s      %8s     %8s    %8s   %8s      %8s %8s\n%s\n",
		XCONS_INFO_LINE, "Task ", "APPL-PID", "COMMANDS",
                kgs->semaphore [ region_to_sort ].semaname , "I/O",
                "SUSPENDS","WAITS", XCONS_INFO_LINE);
      for ( i=0; i<20 && i < kgs->ulNumOfTasks; i++ )
        print_task_cnts ( tcb_ptr[i] ) ;
    }

    DBG1 (( MF__,"e92_dbshow: returning \n" ))
    return ( 0 );
}

/*================================================*/
static int cmp_reg (
struct TASK_TYPE **tcb1 ,
struct TASK_TYPE **tcb2 )
{
  return ( (*tcb1)->reg_excl_cnt[region_to_sort] >
           (*tcb2)->reg_excl_cnt[region_to_sort] ? -1 : 1 ) ;
}

/*================================================*/
static int cmp_io (
struct TASK_TYPE **tcb1 ,
struct TASK_TYPE **tcb2 )
{
  return ( (*tcb1)->TaskStateStat.SelfIOReadStat.IOStat.TimeStat.ulCount +
           (*tcb1)->TaskStateStat.DevIOReadStat.IOStat.TimeStat.ulCount  +
           (*tcb1)->TaskStateStat.SelfIOWriteStat.IOStat.TimeStat.ulCount+
           (*tcb1)->TaskStateStat.DevIOWriteStat.IOStat.TimeStat.ulCount
           >
           (*tcb2)->TaskStateStat.SelfIOReadStat.IOStat.TimeStat.ulCount +
           (*tcb2)->TaskStateStat.DevIOReadStat.IOStat.TimeStat.ulCount  +
           (*tcb2)->TaskStateStat.SelfIOWriteStat.IOStat.TimeStat.ulCount+
           (*tcb2)->TaskStateStat.DevIOWriteStat.IOStat.TimeStat.ulCount ? -1 : 1 ) ;
}

/*================================================*/
static int cmp_cmd (
struct TASK_TYPE **tcb1 ,
struct TASK_TYPE **tcb2 )
{
  return ( (*tcb1)->QueStat.Resetable.ulCOMQueCount >
           (*tcb2)->QueStat.Resetable.ulCOMQueCount ? -1 : 1 ) ;
}

/*================================================*/
static int cmp_vwait (
struct TASK_TYPE **tcb1 ,
struct TASK_TYPE **tcb2 )
{
  return ( (*tcb1)->TaskStateStat.VwaitStat.TimeStat.ulCount >
           (*tcb2)->TaskStateStat.VwaitStat.TimeStat.ulCount ? -1 : 1 ) ;
}

/*================================================*/
static int cmp_vsusp (
struct TASK_TYPE **tcb1 ,
struct TASK_TYPE **tcb2 )
{
  return ( (*tcb1)->TaskStateStat.VsuspStat.TimeStat.ulCount >
           (*tcb2)->TaskStateStat.VsuspStat.TimeStat.ulCount ? -1 : 1 ) ;
}

/*================================================*/
static int get_id (
char *str )
{
  char *str_id = strrchr ( str, '_' ) ;
  int id ;

  if ( str_id == NULL )
    if ( *str == 'T' || *str == 't' )
      str_id = str ;

  if ( str_id != NULL )
    { if ( (id = atoi ( str_id+1 ) ) > 0 )
        /* *str_id = '\0' */;
    }
  else
    id = 0 ;
  return ( id ) ;
}


/*================================================*/
static  int     e92_putparam (
int                     argc ,
char                    ** argv ,
int                     argn ,
int                     *actual_arg )
{
  if ( argc - argn != 2 )
    {
      MSGCD(( MSGNO(11987), "missing paramater! \n" ));
    }
  else
    { if ( e92_set_new_param ( kgs-> XParam, argv[argn] , argv[argn+1] ) )
        e92_print_param ( kgs-> XParam, argv[argn] ? argv[argn] : "" ) ;
    }
  return (0) ;
}

/*================================================*/
static  int     e92_getparam (
int                     argc ,
char                    ** argv ,
int                     argn ,
int                     *actual_arg )
{
  e92_print_param ( kgs-> XParam, argv[argn] ? argv[argn] : "" ) ;
  return (0) ;
}

/*================================================*/
static  int     e92_create_shm_from_file (
int                     argc ,
char                    ** argv ,
int                     argn ,
int                     *actual_arg )
{
  if ( argc - argn > 1 )
    {
      MSGCD(( MSGNO(11987), "invalid paramater! \n" ));
      return (1) ;
    }
  else
    { struct stat buf ;
      *actual_arg = argn ;
      if ( stat ( argv[argn], &buf ) == -1 )
        { MSGCD(( MSGNO(11987), "cannot stat file '%s': %s \n", argv[argn],
                           sqlerrs() )) ;
          return (1) ;
        }
       else
        if ( e92_check_ipcdb (dbname) )
        { int shmid = sql41_create_shm ( common_rte_key , 0 , 0 , buf.st_size,
					    0664 , getuid() , "db" , dbname );
          if ( shmid < 0 )
            { MSGCD(( MSGNO(11987), "cannot create kernel shared section: %s\n",
                             sqlerrs() )) ;
              return (1) ;
            }
          else
            { struct SHARED_SEGMENT *seg =
              (struct SHARED_SEGMENT *) sql41_attach_shm ( shmid , (char*) 0 );
              if ( seg == 0 )
                { MSGCD(( MSGNO(11987), "cannot attach kernel shared section: %s\n" ,
                                 sqlerrs() )) ;
                  return (1) ;
                }
              else
                { FILE *stream = fopen (  argv[argn], "r" ) ;
                  int bytes_read = fread ( (char*)seg, 1, buf.st_size , stream);
                  if ( bytes_read != buf.st_size )
                    { MSGCD(( MSGNO(11987), "only %d bytes of %d read: %s\n" ,
                                       bytes_read, buf.st_size, sqlerrs() )) ;
                      return (1) ;
                    }
                  else
                    { struct SHARED_SEGMENT *naddr = seg->this_seg ;
                      pri ( "\n%d bytes loaded to shared memory \n" ,
                            bytes_read ) ;
                      if ( naddr != seg )
                        {
                          if ( shmdt ( (char *) seg ) >= 0 )
                            { seg = (struct SHARED_SEGMENT *)
                                    sql41_attach_shm ( shmid , (char*) naddr );
                            }
                        }
                      if ( naddr == seg )
                        seg->kgs->kgsshmid[1] = -1;  /* only first part !! */
                      else
                        MSGCD(( MSGNO(11987), "can't mark second kgsshmid\n" )) ;
                    }
                  (void) fclose ( stream ) ;
                }
            }
        }
   }
 return (0) ;
}

/*================================================*/
static int e92_check_ipcdb (
char *dbname )
{
  char mkdir_ipcdb[300], *ipcdb ;
  int rc ;
  struct stat buf ;

  strcpy ( mkdir_ipcdb, "mkdir " ) ;
  ipcdb = mkdir_ipcdb + strlen ( mkdir_ipcdb );
  sql41_get_ipc_dir ( ipcdb );
  (void) sprintf ( ipcdb + strlen ( ipcdb ), "db:%s", dbname );
  DBG1 (( MF__,"e92_check_ipcdb: %s\n", ipcdb )) ;

  if ( stat ( ipcdb, &buf ) == -1 )
    { rc = system ( mkdir_ipcdb ) ;
      DBG1 (( MF__,"e92_check_ipcdb: ret = %d\n", rc ))
      common_rte_key = sql41_get_server_key ( dbname );
    }
  else
    { rc = 0 ;
    }

  return ( rc == 0 ) ;
}

/*==========================================================================*/
/* PTS 1114129 */
static  void    e92_dbshow_connections ( )
{
#undef MF__
#define MF__ MOD__ "e92_dbshow_connections"
	int     gotConnectedTask = 0;
	struct  TASK_TYPE               * tcb ;
	struct connection_info          * pCi;
#define MAX_NAME_BUFFER_SIZE 128
	char buffer[MAX_NAME_BUFFER_SIZE];
    
	DBGIN;

#  ifndef	RUN_AS_SYSMON


	for ( tcb = kgs->pFirstTaskCtrl ; tcb <= kgs->pLastTaskCtrl ; tcb ++ )
	{
		pCi = tcb->connection;
		if ( pCi != 0 && tcb->state != TSK_INACTIVE_EO00 && tcb->state != TSK_CONNECTWAIT_EO00 )
		{
			gotConnectedTask = 1;
			pri ( XCONS_FORMAT_SHOW_CONNECTION_TASK, tcb->index, 
                                                     tcb->ukt->index, 
                                                     en921GetTaskDisplayName (tcb->pTaskObject, true));
			if ( pCi->ci_connect_time == 0 )
			{
			    pri ( XCONS_FORMAT_SHOW_CONNECTION_STATE_NOT_CONNECTED, eo92GetConnectStateName( pCi->ci_state, buffer, MAX_NAME_BUFFER_SIZE ) );
			}
			else
			{
				struct tm *pTime = localtime( &pCi->ci_connect_time );
				if ( pTime )
				{
					pri ( XCONS_FORMAT_SHOW_CONNECTION_STATE_TIME, eo92GetConnectStateName( pCi->ci_state, buffer, MAX_NAME_BUFFER_SIZE ), asctime( pTime ) );
				}
				else
				{
					pri ( XCONS_FORMAT_SHOW_CONNECTION_STATE_DUMPTIME, eo92GetConnectStateName( pCi->ci_state, buffer, MAX_NAME_BUFFER_SIZE ), pCi->ci_connect_time );
				}
			}
			if ( 0 == pCi->ci_remote_pid[0] || ' ' == pCi->ci_remote_pid[0] )
			{
				pri ( XCONS_FORMAT_SHOW_CONNECTION_LOCAL, (int)pCi->ci_peer_pid);
			}
			else
			{
				pri ( XCONS_FORMAT_SHOW_CONNECTION_REMOTE, (int)pCi->ci_peer_pid, pCi->ci_remote_pid, pCi->ci_peer_node);
			}

			if ( e92_show_long )
			{
	       	    pri ( "    connect service   %s\n", eo92GetConnectServiceName( pCi->ci_service, buffer, MAX_NAME_BUFFER_SIZE ) );
                pri ( "    use_count         %d\n", pCi->ci_use_count );
				pri ( "    protocol          %s\n", eo92GetConnectProtocolName( pCi->ci_protocol, buffer, MAX_NAME_BUFFER_SIZE ) );
				pri ( "    packet count      %d\n",  pCi->ci_packet_cnt );
				pri ( "    packet size       %ld\n", pCi->ci_packet_size );
				pri ( "    min reply size    %ld\n", pCi->ci_min_reply_size );
				pri ( "    cur reply size    %ld\n", pCi->ci_reply_size );
				pri ( "    max request size  %ld\n", pCi->ci_max_request_size );
				pri ( "    cur request size  %ld\n", pCi->ci_request_lgt );
				pri ( "    max data size     %ld\n", pCi->ci_max_data_size );
				pri ( "    kernel reference  %d\n",  pCi->ci_my_ref );
				pri ( "    kernel sem id     %d\n",  tcb->ukt->semid );
				pri ( "    peer reference    %d\n",  pCi->ci_peer_ref );
				pri ( "    peer sem id       %d\n",  pCi->ci_peer_semid );

				pri ( "    comseg offset     %ld\n", pCi->ci_big_offset );
				pri ( "    comseg size       %ld\n", pCi->ci_big_size );
			}
		}
	}

	if ( !gotConnectedTask )
	{
		pri ( XCONS_FORMAT_SHOW_CONNECTION_NO_TASK_CONNECTED );
	}
#  endif	/*RUN_AS_SYSMON*/

    DBGOUT;
}
/*-------------------------------------*/
struct  KERNEL_GLOBAL_SECTION * en92_CreateShmMemoryCopy(struct SHARED_SEGMENT const *shmAddress)
{
    unsigned long KgsOffset = ALIGN_EO00(sizeof(struct SHARED_SEGMENT), sizeof(void *));
    struct DOUBLY_LINKED *quu;
    struct ten50_UKT_Control * ukt;
    struct DEV_PCB * pcb;
    struct TASK_TYPE * tcb;
    struct  XXX_QUEUE_HEAD *xxx;
    struct XXX_QUEUE *xxxq;
    int iItem = 0;
    char *shmCopyBase = 0;
    SAPDB_Int4 total_xxx_count;

    shmCopyBase = (char *)malloc(shmAddress->seg_size);
    en92_ShmIsMemoryCopy = (shmCopyBase != 0);

    if ( !shmCopyBase )
    {
        /* XXX */
        return 0;
    }
    en92_SharedSegmentStart = (unsigned long)shmAddress->this_seg;
    en92_SharedSegmentEnd = en92_SharedSegmentStart + shmAddress->seg_size;

    en92_NonRemappedKgs = (struct KERNEL_GLOBAL_SECTION *)(((char *)shmAddress) + KgsOffset);
    en92_KgsRemappingDistance = (((char *)shmCopyBase) - ((char *)en92_SharedSegmentStart));
    en92_NonRemappedKgsRemappingDistance = (((char *)shmCopyBase) - ((char *)shmAddress));

    /* Copy the complete kgs */
    SAPDB_memcpy(shmCopyBase, (char *)shmAddress, shmAddress->seg_size);

    /* adjust pointer in KERNEL_GLOBAL_SECTION */
    kgs = (struct  KERNEL_GLOBAL_SECTION *)(((char *)shmCopyBase) + KgsOffset);
    ADJUST_SHM_POINTER_EN92(struct  KERNEL_GLOBAL_SECTION *, kgs->this_kgs);
    ADJUST_SHM_POINTER_EN92(struct  SHARED_SEGMENT *, kgs->seg);
    ADJUST_SHM_POINTER_EN92(SAPDB_Int4 *, kgs->kgsshmid);
    ADJUST_SHM_POINTER_EN92(struct  ten50_UKT_Control *, kgs->first_kp);
    ADJUST_SHM_POINTER_EN92(struct  ten50_UKT_Control *, kgs->last_kp);
    ADJUST_SHM_POINTER_EN92(struct  DEV_PCB *, kgs->first_dev);
    ADJUST_SHM_POINTER_EN92(struct  DEV_PCB *, kgs->first_asdev);
    ADJUST_SHM_POINTER_EN92(struct  DEV_PCB *, kgs->last_dev);
    ADJUST_SHM_POINTER_EN92(struct  TASK_TYPE *, kgs->pFirstTaskCtrl);
    ADJUST_SHM_POINTER_EN92(struct  TASK_TYPE *, kgs->pLastTaskCtrl);
    ADJUST_SHM_POINTER_EN92(struct  TASK_TYPE *, kgs->ti);
    ADJUST_SHM_POINTER_EN92(struct  TASK_TYPE *, kgs->al);
    ADJUST_SHM_POINTER_EN92(struct  TASK_TYPE *, kgs->tw);
    ADJUST_SHM_POINTER_EN92(struct  TASK_TYPE *, kgs->cs);
    ADJUST_SHM_POINTER_EN92(struct  TASK_TYPE *, kgs->ut);
    ADJUST_SHM_POINTER_EN92(struct  TASK_TYPE *, kgs->first_datawriter);
    ADJUST_SHM_POINTER_EN92(struct  TASK_TYPE *, kgs->first_event_task);
    ADJUST_SHM_POINTER_EN92(struct  TASK_TYPE *, kgs->first_backup_task);
    ADJUST_SHM_POINTER_EN92(struct  TASK_TYPE *, kgs->first_garbage_collector);
    ADJUST_SHM_POINTER_EN92(struct  TASK_TYPE *, kgs->first_objectlog_writer);
    ADJUST_SHM_POINTER_EN92(struct  TASK_TYPE *, kgs->first_server);
    ADJUST_SHM_POINTER_EN92(struct  TASK_TYPE *, kgs->first_user);
    ADJUST_SHM_POINTER_EN92(struct  XXX_QUEUE_HEAD *, kgs->first_xxx);
    ADJUST_SHM_POINTER_EN92(struct  XXX_QUEUE_HEAD *, kgs->last_xxx);
    ADJUST_SHM_POINTER_EN92(struct  DDB4_SEMA *, kgs->semaphore);
    kgs->dumpflag = 0;  /* points outside shared memory... */
    kgs->traceflag = 0; /* points outside shared memory... */
    ADJUST_SHM_POINTER_EN92(UCHAR *, kgs->dynpool_bot);
    ADJUST_SHM_POINTER_EN92(UCHAR *, kgs->dynpool_ptr);
    ADJUST_SHM_POINTER_EN92(UCHAR *, kgs->dynpool_top);
    ADJUST_SHM_POINTER_EN92(tsp00_PageAddr, kgs->datacache_bot);
    ADJUST_SHM_POINTER_EN92(tsp00_PageAddr, kgs->datacache_ptr);
    ADJUST_SHM_POINTER_EN92(tsp00_PageAddr, kgs->datacache_top);
    ADJUST_SHM_POINTER_EN92(tsp00_PageAddr, kgs->convcache_bot);
    ADJUST_SHM_POINTER_EN92(tsp00_PageAddr, kgs->convcache_ptr);
    ADJUST_SHM_POINTER_EN92(tsp00_PageAddr, kgs->convcache_top);
    ADJUST_SHM_POINTER_EN92(tsp00_PageAddr, kgs->dyndata_bot);
    ADJUST_SHM_POINTER_EN92(tsp00_PageAddr, kgs->dyndata_ptr);
    ADJUST_SHM_POINTER_EN92(tsp00_PageAddr, kgs->dyndata_top);
    ADJUST_SHM_POINTER_EN92(struct connection_info *, kgs->connection);
    ADJUST_SHM_POINTER_EN92(void *, kgs->alone_thread);
    ADJUST_SHM_POINTER_EN92(struct rte_xparam_record *, kgs->XParam);
    ADJUST_SHM_POINTER_EN92(void *, kgs->Eventing);
    ADJUST_SHM_POINTER_EN92(void *, kgs->MessageBufferSpace);
    /* coord, console, timer, requestor */
    ADJUST_SHM_POINTER_EN92(void *, kgs->coord.work.first);
    ADJUST_SHM_POINTER_EN92(void *, kgs->coord.work.last);
    ADJUST_SHM_POINTER_EN92(void *, kgs->console.work.first);
    ADJUST_SHM_POINTER_EN92(void *, kgs->console.work.last);
    ADJUST_SHM_POINTER_EN92(void *, kgs->timer.work.first);
    ADJUST_SHM_POINTER_EN92(void *, kgs->timer.work.last);
    ADJUST_SHM_POINTER_EN92(void *, kgs->requestor.work.first);
    ADJUST_SHM_POINTER_EN92(void *, kgs->requestor.work.last);

    /* dev0 */
    ADJUST_SHM_POINTER_EN92(volatile struct IOC_QUEUE *, kgs->dev0.io_work);
    ADJUST_SHM_POINTER_EN92(struct dev_info *, kgs->dev0.dev);
    ADJUST_SHM_POINTER_EN92(struct IOC_QUEUE *, kgs->dev0.ios);
    ADJUST_SHM_POINTER_EN92(volatile struct IOC_QUEUE *, kgs->dev0.io_request);

    ADJUST_SHM_POINTER_EN92(tsp00_PageAddr, kgs->traceBufferSpace);

    /* adjust pointer to kgs and pointer to segment */
    kgs->seg->kgs = kgs;
    kgs->seg->this_seg = kgs->seg;

    for ( iItem = 0; iItem < NUM_DEVSP_TYPES; iItem++ )
    {
        SAPDB_Int4 NumDevs = kgs->XParam->DevspDescr[iItem].ulNumDevs ;
        if ( NumDevs > 0 )
        {
            ADJUST_SHM_POINTER_EN92(DEV_DESCR *, kgs->XParam->DevspDescr[iItem].Devspaces);
        }
    }

    for ( ukt = kgs->first_kp ; ukt && ukt <= kgs->last_kp ; ukt ++ )
    {
        ADJUST_SHM_POINTER_EN92(struct connection_info *, ukt->connection);
        ADJUST_SHM_POINTER_EN92(struct TASK_TYPE *, ukt->curr_task);
        ADJUST_SHM_POINTER_EN92(struct DOUBLY_LINKED *, ukt->curr_quu);
        ADJUST_SHM_POINTER_EN92(struct TASK_TYPE *, ukt->pFirstTaskCtrl);
        ADJUST_SHM_POINTER_EN92(struct IOC_QUEUE *, ukt->ioc_queue.first);
        ADJUST_SHM_POINTER_EN92(struct IOC_QUEUE *, ukt->ioc_queue.last);
        ADJUST_SHM_POINTER_EN92(struct IOC_QUEUE *, ukt->iocs);
        for ( iItem = 0; ukt->iocs && iItem < (kgs->ulNumOfTasks + (kgs->maxdevspaces + kgs->maxtapes) * kgs->max_asyn_request); ++iItem )
        {
            ADJUST_SHM_POINTER_EN92(struct IOC_QUEUE *, ukt->iocs[iItem].next);
            ADJUST_SHM_POINTER_EN92(struct IOC_QUEUE *, ukt->iocs[iItem].previous);
            ADJUST_SHM_POINTER_EN92(struct DOUBLY_LINKED *, ukt->iocs[iItem].request);
        }
        ADJUST_SHM_POINTER_EN92(struct IOR_QUEUE *, ukt->aioc_queue.first);
        ADJUST_SHM_POINTER_EN92(struct IOR_QUEUE *, ukt->aioc_queue.last);
        ADJUST_SHM_POINTER_EN92(struct IOR_QUEUE *, ukt->aiocs);
        for ( iItem = 0; ukt->aiocs && iItem < kgs->ulNumOfTasks; ++iItem )
        {
            ADJUST_SHM_POINTER_EN92(struct IOR_QUEUE *, ukt->aiocs[iItem].next);
            ADJUST_SHM_POINTER_EN92(struct IOR_QUEUE *, ukt->aiocs[iItem].previous);
            ADJUST_SHM_POINTER_EN92(struct DOUBLY_LINKED *, ukt->aiocs[iItem].request);
        }
        ADJUST_SHM_POINTER_EN92(struct RUN_QUEUE *, ukt->RunQueue);
        for ( iItem = 0; ukt->RunQueue && iItem < (2 * kgs->ulNumOfTasks); ++iItem )
        {
            ADJUST_SHM_POINTER_EN92(struct TASK_TYPE *, ukt->RunQueue[iItem].Task);
            ADJUST_SHM_POINTER_EN92(struct DOUBLY_LINKED *, ukt->RunQueue[iItem].Req);
            ADJUST_SHM_POINTER_EN92(struct TASK_TYPE *, ukt->RunQueue[iItem].FromTask);
        }
        ADJUST_SHM_POINTER_EN92(struct UKP_IO_STRUCT *, ukt->pSingleIO);
        ADJUST_SHM_POINTER_EN92(struct XXX_QUEUE_HEAD **, ukt->xxx);
        for ( iItem = 0; iItem < kgs->ulNumOfUKTs; ++iItem )
        {
            ADJUST_SHM_POINTER_EN92(struct XXX_QUEUE_HEAD *, ukt->xxx[iItem]);
        }
        ADJUST_SHM_POINTER_EN92(struct TASK_TYPE *, ukt->last_com_task);
        ADJUST_SHM_POINTER_EN92(struct ten50_UKT_Control *, ukt->moveTaskToUKT);
        ADJUST_SHM_POINTER_EN92(tsp00_PageAddr, ukt->traceBuffer);
        ADJUST_SHM_POINTER_EN92(struct RTE_CoroutineContext *, ukt->removeTaskContext);
        ADJUST_SHM_POINTER_EN92(struct RTETask_TaskScheduler*, ukt->pTaskSchedulerObject);
        ADJUST_SHM_POINTER_EN92(struct RTETask_LegacyUKTCtrl*, ukt->pLegacyUKTCtrlObject);
    }

    for ( pcb = kgs->first_dev ; pcb && pcb <= kgs->last_dev ; pcb ++ )
    {
        ADJUST_SHM_POINTER_EN92(struct ten50_UKT_Control *, pcb->ukt);
        ADJUST_SHM_POINTER_EN92(struct DOUBLY_LINKED *, pcb->request);
        ADJUST_SHM_POINTER_EN92(volatile struct IOC_QUEUE *, pcb->io_work);
        ADJUST_SHM_POINTER_EN92(volatile struct IOC_QUEUE *, pcb->io_request);
        ADJUST_SHM_POINTER_EN92(struct IOC_QUEUE *, pcb->ios);
    }

    for ( tcb = kgs->pFirstTaskCtrl ; tcb && tcb <= kgs->pLastTaskCtrl ; tcb ++ )
    {
        ADJUST_SHM_POINTER_EN92(struct TASK_TYPE *, tcb->this_tcb);
        ADJUST_SHM_POINTER_EN92(struct TASK_TYPE *, tcb->prev_task);
        ADJUST_SHM_POINTER_EN92(struct TASK_TYPE *, tcb->next_task);
        ADJUST_SHM_POINTER_EN92(struct ten50_UKT_Control *, tcb->ukt);
        ADJUST_SHM_POINTER_EN92(struct ten50_UKT_Control *, tcb->bigComSegBelongsToUKT);
        ADJUST_SHM_POINTER_EN92(struct connection_info *, tcb->connection);
        ADJUST_SHM_POINTER_EN92(SAPDB_Int4 *, tcb->reg_excl_cnt);
        ADJUST_SHM_POINTER_EN92(SAPDB_Int4 *, tcb->reg_coll_cnt);
        ADJUST_SHM_POINTER_EN92(char *, tcb->DevNameWaitingFor);
        ADJUST_SHM_POINTER_EN92(struct RTETask_CommonQueueElement *, tcb->asyn_requests);
        ADJUST_SHM_POINTER_EN92(void *, tcb->aio_op);
        ADJUST_SHM_POINTER_EN92(void *, tcb->pTrackBuffer);
        ADJUST_SHM_POINTER_EN92(struct RTE_CoroutineContext *, tcb->taskContext);
        ADJUST_SHM_POINTER_EN92(struct TASK_TYPE *, tcb->nextTaskInWaitQueue);
        ADJUST_SHM_POINTER_EN92(struct RTETask_Task *, tcb->pTaskObject);
        ADJUST_SHM_POINTER_EN92(struct RTETask_LegacyTaskCtrl *, tcb->pLegacyTaskCtrlObject);
    }

    for ( xxx = kgs->first_xxx; xxx && xxx <= kgs->last_xxx; ++xxx )
    {
        ADJUST_SHM_POINTER_EN92(struct XXX_QUEUE *, xxx->xx_work);
        ADJUST_SHM_POINTER_EN92(struct XXX_QUEUE *, xxx->element);
        ADJUST_SHM_POINTER_EN92(struct XXX_QUEUE *, xxx->xx_request);
    }

    xxxq = (struct XXX_QUEUE *)(kgs->last_xxx+1);
    total_xxx_count = (((SAPDB_Byte *)&kgs->semaphore[0]) - ((SAPDB_Byte *)xxxq))/sizeof(struct XXX_QUEUE);

    for ( iItem = 0; iItem < total_xxx_count; ++iItem )
    {
        ADJUST_SHM_POINTER_EN92(struct XXX_QUEUE *, xxxq->next);
        ADJUST_SHM_POINTER_EN92(struct TASK_TYPE *, xxxq->task);
        ADJUST_SHM_POINTER_EN92(struct TASK_TYPE *, xxxq->FromTask);
        ++xxxq;
    }

    for ( iItem = 0; iItem < kgs->XParam->ulNoOfRegions; ++iItem )
    {
        SAPDB_Int iListItem;
        ADJUST_SHM_POINTER_EN92(volatile struct TASK_TYPE *, kgs->semaphore[iItem].owner);
        ADJUST_SHM_POINTER_EN92(struct N_QUEUE *, kgs->semaphore[iItem].semaliste.first);
        ADJUST_SHM_POINTER_EN92(struct N_QUEUE *, kgs->semaphore[iItem].semaliste.last);
        ADJUST_SHM_POINTER_EN92(struct N_QUEUE *, kgs->semaphore[iItem].semaliste.sem);
        if ( kgs->semaphore[iItem].semaliste.sem )
        {
            for ( iListItem = 0; iListItem < kgs->ulNumOfTasks; ++iListItem )
            {
                ADJUST_SHM_POINTER_EN92(struct N_QUEUE *, kgs->semaphore[iItem].semaliste.sem[iListItem].next);
                ADJUST_SHM_POINTER_EN92(struct TASK_TYPE *, kgs->semaphore[iItem].semaliste.sem[iListItem].taskid);
            }
        }
    }

    return kgs;
}

/*-----------------------------------*/

#define ERR_MAP_KGS_SHMAT              11208,ERR_TYPE,"KGS     ","Cannot attach to Kernel Shared Memory Segment: %s"
#define ERR_MAP_KGS_SHMAT_RDONLY_PERM  11110,ERR_TYPE,"KGS     ","Cannot attach to Kernel Shared Memory Segment (RO): %s"
#define ERR_UNMAP_KGS_SHMDT            11210,ERR_TYPE,"KGS     ","Kernel Shared Memory Segment shmdt error, %s"

int en92_MapKgsNotRemapped ( int shmid, int needWriteAccess )
{
    int accessMode = 0;
    struct SHARED_SEGMENT *shmAddress;
    struct SHARED_SEGMENT *shmCopyAddress;
    /* three runs
        1. use wantedAddress 0 + SHM_RDWR if fails retry with SHM_RDONLY
          if success and original address matches -> done
          if failure -> mapping error -> return 0
        2. retry with wantedAddress = KGS segment address of kernel
          if success -> done
          else reset wantedAddress to 0
        3. retry again with wanted address 0
     */
    char *wantedAddress;
    int retryCounter;
    for ( wantedAddress = 0, retryCounter = 0; retryCounter < 3; ++retryCounter )
    {
        shmAddress = (struct SHARED_SEGMENT *) shmat ( e92_shmid , wantedAddress, SHM_RD_WR );
	    DBG1 (( MF__,"shmat  ShmAddr    0x%lx \n", ShmAddr ));
        if ( shmAddress == (struct SHARED_SEGMENT *) UNDEF )
        {
		    if ( (errno == EPERM || errno == EACCES)
              && !needWriteAccess )
            {
                shmAddress = (struct SHARED_SEGMENT *) shmat ( e92_shmid , wantedAddress, SHM_RDONLY );
                if ( shmAddress == (struct SHARED_SEGMENT *) UNDEF )
                {
                    if ( !wantedAddress )
                    {
                        MSGCD (( ERR_MAP_KGS_SHMAT_RDONLY_PERM, sqlerrs() ));
                        return 0;
                    }
                }
                else
                {
                    accessMode = 1;
                }
            }
		    else if ( !wantedAddress )
            {
			    MSGCD (( ERR_MAP_KGS_SHMAT, sqlerrs() ));
                return 0;
            }
        }
        else
        {
            accessMode = 2;
        }

        switch( retryCounter )
        {
        case 0:
            if ( shmAddress->this_seg == shmAddress )
            {
                retryCounter = 3; /* original address always welcome... */
            }
            else
            {
                /* prepare retry with original address */
                wantedAddress = (char *)(shmAddress->this_seg);
                shmdt( (char *)shmAddress ); /* detach unwanted address */
            }
            break;
        case 1:
            if ( shmAddress == (struct SHARED_SEGMENT *) UNDEF )
            {
                /* prepare retry with any address */
                wantedAddress = 0;
            }
            else
            {
                retryCounter = 3; /* accept whatever we got... */
            }
            break;
        default:
            /* no further retry */
            break;
        }

    }

    if ( shmAddress->this_seg == shmAddress )
    {
        en92_NonRemappedKgs = shmAddress->kgs;
        kgs = en92_NonRemappedKgs;
        en92_ShmIsMemoryCopy = false;
        en92_KgsRemappingDistance = 0;
    }
    else
    {
        kgs = en92_CreateShmMemoryCopy(shmAddress);
        if ( !kgs )
        {
            en92_UnmapKgsNotRemapped();
            accessMode = 0;
        }
    }

    DBGOUT;

    return accessMode;
}

/*-------------------------------------*/

void en92_UnmapKgsNotRemapped()
{
#undef MF__
#define MF__ MOD__"en92_UnmapKgsNotRemapped"
    DBGIN;

    if ( en92_ShmIsMemoryCopy )
    {
        free(kgs->seg);
    }

    if ( en92_NonRemappedKgs )
    {
        unsigned long KgsOffset = ALIGN_EO00(sizeof(struct SHARED_SEGMENT), sizeof(void *));
	    if ( shmdt ( ((char *)en92_NonRemappedKgs) - KgsOffset ) < 0 )
        {
            MSGD (( ERR_UNMAP_KGS_SHMDT, sqlerrs() ));
        }
    }

    en92_NonRemappedKgs = kgs = 0;
    en92_KgsRemappingDistance = 0;
    en92_ShmIsMemoryCopy = 0;

    DBGOUT;
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
