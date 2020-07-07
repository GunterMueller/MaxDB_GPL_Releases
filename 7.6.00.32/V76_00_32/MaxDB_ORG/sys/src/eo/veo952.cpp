/*!
  @file           veo952.cpp
  @author         JoergM
  @special area   application startup program
  @brief          rte programs (x_start, x_stop, ... )
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




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#define XPARAM_NAMES_ONLY
#include "geo002.h"
#include "heo95.h"
#include "heo11.h"
#include "gsp09.h"
#include "gip00.h"
#include "heo46.h"
#include "RunTime/Configuration/RTEConf_ParameterAccess.hpp"

#ifndef WIN32                           /*&if $OSSPEC = WIN32*/
#include "gen41.h"                      /* nocheck but_imkdep */
#include "RunTime/RTE_SdbStarter.h"     /* nocheck but_imkdep */
#endif                                  /*&endif*/

#include "RunTime/RTE_GetDBRootOfDB.h"

#include "SAPDBCommon/SAPDB_Names.h"
#include "SAPDBCommon/SAPDB_string.h"
#include "vsp003c.h"
extern void	sqloutwrite     ( SAPDB_Int4           msg_no ,
                              tsp3_msg_type        msg_type ,
                              const SAPDB_Char    *msg_label ,
                              const SAPDB_Char    *msg_format ,
                              ...                           );

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#ifdef WIN32 
#define SERVER_OPT_INSTALL   "install"
#define SERVER_OPT_REMOVE    "remove"
#endif

#define SERVER_OPT_START     "start"
#define SERVER_OPT_STOP      "stop"
#define SERVER_OPT_UNLISTEN  "unlisten"

#ifndef WIN32
#define INFO_TYPE 3
#define INFO_STARTING                10001,INFO_TYPE,"start   ","Starting server '%s', please wait ..."
#define INFO_GWSTARTING              10001,INFO_TYPE,"gwstart ","Starting gateway '%s', please wait ..."
#define INFO_STOP_CLEARING           10003,INFO_TYPE,"stop    ","Server '%s' did not run, clearing resources."
#define INFO_STOP_STOPPED            10008,INFO_TYPE,"stop    ","Server '%s' stopped."

int e952_call_clear ( PROG_ID *clear, 
					  int      num_args, 
					  char   **args );
static int ClearCalledFromStop = FALSE ;

#endif

#ifdef NO_INLINES
# define _INLINE
#else
# define _INLINE	__inline
#endif

#define MAX_START_PARAMS 10
#define START_OPT_SLOW     "slow"
#define START_OPT_QUICK    "quick"
#define START_OPT_FAST     "fast"
#define START_OPT_TEST     "test"
#define START_OPT_PROF     "P"
#define START_OPT_PROF_BUF "p"
#define START_OPT_DEBUG    "D"
#define START_OPT_IGN_REST "-i"
#define START_OPT_ADMIN_MODE "-admin"
#define START_GATEWAY      "-g"
#define START_OPT_LOW_PRIO "-l"
#define START_OPT_LOWER_PRIORITY "-lowprio"
#define STOP_OPT_IGN_SHUT  "-force"
#define STOP_OPT_DUMP      "-dump"
#define END_OF_PARSABLE_OPTIONS "--"

#define TYPE_FAST_KERNEL   0
#define TYPE_QUICK_KERNEL  1
#define TYPE_SLOW_KERNEL   2
#define TYPE_TEST_KERNEL   3

#define OPT_VERBOSE   "-v"
#define OPT_NODE     "-n"
#define OPT_CHECK_INTERVAL "-c"
#define OPT_DEBUG_LEVEL "-D"
#define OPT_NEW_DEBUG_LEVEL "-N"
#define OPT_PROTFILE_SIZE "-Z"
#define OPT_SERVICEPORT_NUMBER "-S"
#define OPT_SERVER_INSTALL     "-i"
#define OPT_SERVER_REMOVE      "-r"
#define OPT_SERVER_UPDATE      "-u"
#define OPT_SERVER_START       "-s"
#define OPT_SERVER_STOP        "-k"
#define OPT_SERVER_FASTMODE    "-F"

#define DONT_ASK_FOR_CREATION FALSE

/* Type of program to be started up */
#define TFLG_KERNEL	1
#define TFLG_GATEWAY 	2

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/
typedef struct          xstop_args
                        { char *dbname ;
                          char *ign_shut ;
                          char *dumpflag;
                          char *verbose;
                        } XSTOP_ARGS ;

/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

#ifndef WIN32
static int check_db_to_start         ( char      *dbname, 
							           char      *full_knl_path, 
 		   					           char      *knl_prog, 
                                       int        type_flag );

#endif

static int fill_stop_rec             ( XSTOP_ARGS *stop_rec, 
						               int         num_args, 
						               char **     args ) ;

static int e952_check_rundir ( char  *dbname, 
                              SAPDB_UTF8 *param_rundir );

static int rundir_exists             ( char       *dbname, 
						               SAPDB_UTF8 *rundir ) ;

/*===========================================================================*
 *  PUBLIC FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

/*-----------------------------------------------------------*/
int e952_call_stop ( PROG_ID *stop, 
					 int      num_args, 
					 char   **args )
/*-----------------------------------------------------------*/
{
  int call_ret = NOTOK ;
  XSTOP_ARGS args_rec ;
  if ( debug ) e950_print_params ( "call_stop", num_args, args ) ;

  if ( !fill_stop_rec ( &args_rec, num_args, args ) )
    e950_call_help (stop->progid, stop->prog_helpfiles ); 
  else
    {
      char **loc_params = (char **)malloc ( (num_args+1) * sizeof ( char *) ) ;
      int num_params = 0 ;
      if ( loc_params )
        { 
    {
        tsp00_Pathc     DBRootOfDB;

        if ( RTE_GetDBRootOfDB(args_rec.dbname, DBRootOfDB, sizeof(DBRootOfDB)) )
        {
            e950_PutEnvDBRoot(DBRootOfDB) ;
        }
        else
        {
            printf("Installationpath of serverdb <%s> not found!", args_rec.dbname);
            return NOTOK;
        }
    }
#ifdef WIN32
          loc_params[num_params] = "-d"            ; ++num_params ; 
#endif
          loc_params[num_params] = args_rec.dbname ; ++num_params ;

          if ( args_rec.ign_shut ) 
            { loc_params[num_params] = args_rec.ign_shut; ++num_params ; }

          if ( args_rec.dumpflag ) 
            { loc_params[num_params] = args_rec.dumpflag ; ++num_params ; }

          if ( args_rec.verbose ) 
            { loc_params[num_params] = args_rec.verbose ; ++num_params ; }

          call_ret = e950_call_component (stop, num_params, loc_params);
          free ( loc_params ) ;
        }
    }
  return ( call_ret ) ;
}

/*-----------------------------------------------------------*/
int e952_call_console (PROG_ID *console, 
					   int      num_args, 
					   char   **args )
/*-----------------------------------------------------------*/
{

    int call_ret = NOTOK;
    DBG0 (( "\ncall_console called with %d parameters\n", num_args ));
    if ( num_args )
    {
	    /* PTS 1108841 */
        tsp00_Pathc     DBRootOfDB;

        if ( RTE_GetDBRootOfDB(args[0], DBRootOfDB, sizeof(DBRootOfDB)) )
        {
            e950_PutEnvDBRoot(DBRootOfDB) ;
            call_ret = e950_call_component ( console, num_args, args ) ;
        }
        else
        {
            printf("Installationpath of serverdb <%s> not found!", args[0]);
        }
    }
    else
    {
        USAGE_CONS(console->progid);
#ifndef WIN32
        e950_search_and_call_sqlprog ( "x_show",  num_args, args ) ;
#endif
    }
  return ( call_ret ) ;
}

/*-----------------------------------------------------------*/
int e952_call_show (PROG_ID *show, 
					int      num_args, 
					char   **args )
/*-----------------------------------------------------------*/
{
  int call_ret = NOTOK;
  DBG0 (( "\ncall_show called with %d parameters\n", num_args ));

#ifdef WIN32

  call_ret = e950_call_component ( show, num_args, args ) ;

#else /* it's UNIX */

  if ( num_args == 0 ||
     ( num_args == 1 && !strcmp ( args[0], "-c" ) ) )
  {  
    char cmdline [ 1024 ] ;
    tsp00_Pathc   PgmPath ;

    if ( en952_GetDbrootPgmPath (PgmPath) )
    { 
      printf ("\nServerdbs started for following names:\n");

      /* PTS 1104922 */
      en41BuildXShowCommand(PgmPath, cmdline, 1024);

      DBG0 (("\nxshow cmd: %s\n", &cmdline[0] ));
      putenv("COLUMNS=200"); /* PTS 1103704 */
      call_ret = system ( cmdline ) ;
    }
    else
      call_ret = NOTOK ;
  }
  else
    USAGE_SHOW(show->progid)

#endif

  return ( call_ret ) ;
}


/*-----------------------------------------------------------*/
int e952_call_vserver ( PROG_ID *vserver,
  					    int      num_args, 
					    char   **args )
/*-----------------------------------------------------------*/
{
    int call_ret = TRUE ;
#if WIN32 

    int num_params;
    char **params;
    int i;
    SAPDB_Bool setStartFlag = true;

    DBG0 (( "\ncall_vserver called\n" ));

    num_params = 0;
    params = (char **)malloc((num_args+1) * sizeof(char *));
    if ( !params )
    {
        printf("Failed to allocate memory for parameter array\n");
    }

    for ( i=0; i < num_args && IS_OPT_CHR (*args[i]) && call_ret != NOTOK; i++ )
    {
        if (  args[i][1] == OPT_DEBUG_LEVEL[1] 
           || args[i][1] == OPT_SERVER_INSTALL[1]
           || args[i][1] == OPT_SERVER_REMOVE[1] 
           || args[i][1] == OPT_SERVER_UPDATE[1]
           || args[i][1] == OPT_SERVER_FASTMODE[1]
           || args[i][1] == OPT_SERVER_START[1]
           || args[i][1] == OPT_SERVER_STOP[1]
           || args[i][1] == OPT_NEW_DEBUG_LEVEL[1] ) 
        {
            setStartFlag = false;
        }
        if (  args[i][1] == OPT_SERVICEPORT_NUMBER[1] 
           || args[i][1] == OPT_NODE[1]
           || args[i][1] == OPT_DEBUG_LEVEL[1] 
           || args[i][1] == OPT_NEW_DEBUG_LEVEL[1]  ) /* PTS 1111208 */
		{
		    if ('\0' == args[i][2]) 
            {
                if (( (i + 1) >= num_args ) || IS_OPT_CHR (*args[i+1]))
                    call_ret = NOTOK ;
                else
                    params[num_params] = args[i++] ; num_params++ ;
            }
        }
        params[num_params] = args[i] ; num_params++ ;
    }
    
    if ( num_args > i && !strcmp ( args[i], SERVER_OPT_START ) )
    {
        params[num_params] = OPT_SERVER_START ; num_params++ ; 
    }
    else if ( num_args > i && !strcmp ( args[i], SERVER_OPT_STOP ) )
    {
        params[num_params] = OPT_SERVER_STOP ; num_params++ ; 
    }
    else if ( num_args > i && !strcmp ( args[i], SERVER_OPT_INSTALL ) )
    {
        params[num_params] = OPT_SERVER_INSTALL ; num_params++ ; 
    }
    else if ( num_args > i && !strcmp ( args[i], SERVER_OPT_REMOVE ) )
    {
        params[num_params] = OPT_SERVER_REMOVE ; num_params++ ; 
    }
    else if (( num_args == 0 ) || ( setStartFlag ))
    {
        params[num_params] = OPT_SERVER_START ; num_params++ ; 
    }
    else if ( i == 0 )
    {
        call_ret = NOTOK ; 
    }

    if ( call_ret != NOTOK )
    {
        if ( num_params + 2 == num_args && 
             IS_OPT_CHR ( *(args[i+1]) ) && 
             !strcmp ( &args[i+1][1], &OPT_NODE[1] ) )
        {
            params[num_params] = args[i+1] ; num_params++; 
            params[num_params] = args[i+2] ; num_params++; 
        }
        else if ( num_params < num_args )
        {
            call_ret = NOTOK ;
        }
    }
    if ( call_ret == NOTOK ) 
    {
        USAGE_SERVER(vserver->progid);
    }

    if ( call_ret != NOTOK )
    {
         call_ret = e950_call_component ( vserver, num_params, params ) ;
    }

#else
    /* UNIX vserver directly parses its arguments... */
    tsp01_RteError RteError;

    SAPDB_Int4             callerUid = -1;
    SAPDB_Int4             callerGid = -1;
    SAPDB_Bool             isAdministrator = false;
    RTE_Path               binPath ;
    SAPDB_Bool             dummy = false;

    int newArgc = num_args + 1; 
    char **newArgv;
    SAPDB_UInt4     idx;


    newArgv = (char **)alloca(sizeof (*newArgv) * newArgc);
    if ( !newArgv )
    {
        printf("Failed to allocate memory for parameter array\n");
        return NOTOK;
    }

    /* Add the own program path (/<IndepBin>/x_server) as first argument.   */
    /* This has to be done for the sdbstarter program call.                 */
    /* The own program path was eliminated from the given arguments before  */
    /* this function was called.                                            */
    /* May be we should extract the x_server startup program from the       */
    /* general sql_run mechanism.                                           */                   
    if (en950_GetProgramExecPath (vserver->callProgLocation, binPath))
    { 

        strcat (binPath, "/");
        strcat (binPath, vserver->progid);
        newArgv[0] = binPath + 0;
        for (idx = 0; idx < num_args; ++idx)
        {
            newArgv[idx + 1] = args[idx];
        }
    }
    else
    {
        printf("Failed to get execution path for %s\n", vserver->progid);
        return NOTOK;
    }

    /* Start using sdbstarter to initialize group list correctly */
    RTE_CallSdbStarter(newArgc, newArgv, callerUid, callerGid, isAdministrator);

/*
    if (sqlUpdateNiLibPathEnvironment(&dummy, &RteError))
	{
*/
         call_ret = e950_call_component (vserver, num_args, args);
/*
	}
	else
	{
	    printf("\nUpdate environment failed:%s\nVServer not executed!\n", RteError.RteErrText ) ;
	    call_ret = NOTOK;
	}
*/
#endif
    return ( call_ret ) ;
}

#define SQLOPT "SQLOPT"
#define MAX_SQLOPT_LEN 300
/*-----------------------------------------------------------*/
int e952_call_sqlfilter ( PROG_ID *sqlfilter, 
						  int      num_args, 
						  char   **args )
/*-----------------------------------------------------------*/
{
  int call_ret, num_params = num_args ;
  char **params = (char **)malloc ( (num_args) * sizeof ( char *) ) ;

  DBG0 (( "\ncall_sqlfilter called with %d parameters\n", num_args ));
  if ( params != (char **) NULL )
    { int i;
      char act_opt[30], opt [ 200 ] ;
      for ( i=0, num_params=0, opt[0] = '\0' ; i < num_args ; i++  )
        if ( !strcmp ( args[i], "-u" ) || !strcmp ( args[i], "-d" ) )
          { sprintf ( act_opt, " %s %s", args[i], args[i+1] ) ;
            strcat ( opt, act_opt ) ; i++ ;
          }
        else
        if ( !strcmp ( args[i], "-T" ) )
          strcat ( opt, " -X -F sql.pct" ) ; 
        else
          { params [ num_params ] = args [ i ] ; num_params++ ; }
       if ( *opt != '\0' )
         { 
           char env_opt[MAX_SQLOPT_LEN];
           static char new_sqlopt[MAX_SQLOPT_LEN] ;

           if ( sqlGetEnvironment(SQLOPT, env_opt, sizeof(env_opt)) ) 
           {
             DBG0 (("%s: %s\n", SQLOPT, &env_opt[0] ));
             sprintf(new_sqlopt, "%s=%s %s", SQLOPT, &opt[0], &env_opt[0] ) ;
           }
           else
           {
             DBG0 (("%s: %s\n", SQLOPT, "is not set" ));
             sprintf(new_sqlopt, "%s=%s", SQLOPT, &opt[0] ) ;
           }
           DBG0 (("\nsetting new environment for %s:\n", SQLOPT ));
           DBG0 (("new value is : %s\n", &new_sqlopt[0] ));
           putenv ( new_sqlopt ) ;
         }
     }
  call_ret = e950_call_component ( sqlfilter, num_params, params ) ;
  if ( params != (char **)NULL ) free ( params ) ;
  return ( call_ret ) ;
}

/*-----------------------------------------------------------*/
int e952_call_backup( PROG_ID *backup,
					  int      num_args,
					  char   **args )
/*-----------------------------------------------------------*/
{
  int call_ret = NOTOK;

 /* PTS 1108797 */
  if ( num_args >= 2 && strcmp(args[0],"-d") == 0 )
  {
    tsp00_Pathc   DBRootOfDB;
	if ( RTE_GetDBRootOfDB(args[1], DBRootOfDB, sizeof(DBRootOfDB)) )
	{
	  e950_PutEnvDBRoot(DBRootOfDB) ;
      call_ret = e950_call_component ( backup, num_args, args ) ;
	}
  }
  else
  {
    /* search in SQLOPT / XUSER (like R/3) for the default DB */
	call_ret = e953_call_PrecompiledTools ( backup, num_args, args ) ;
  }
  return ( call_ret ) ;
}

/*-----------------------------------------------------------*/
int eo952CallPing     ( PROG_ID *Ping,
  					    int      num_args, 
					    char   **args )
/*-----------------------------------------------------------*/
{
    int call_ret = NOTOK ;
	tsp01_RteError RteError;
    SAPDB_Bool      dummy = false;


    DBG0 (( "\ncall_ping called\n" ));


	if (sqlUpdateNiLibPathEnvironment(dummy, &RteError))
	{
		call_ret = e950_call_component ( Ping, num_args, args ) ;
	}
	else
	{
		printf("\nUpdate environment failed:%s\nPing not executed!\n", RteError.RteErrText ) ;
		call_ret = NOTOK;
	}
    
    return ( call_ret ) ;
}

/* ###################################################### */
#ifndef WIN32
/* ###################################################### */
/*-----------------------------------------------------------*/
int en952_GetDbrootPgmPath ( char *DbrootPgmPath )
/*-----------------------------------------------------------*/
{
  int             rc ;
  tsp01_RteError  RteError ;

  eo46_rte_error_init ( &RteError ) ;
  tsp00_C256c DbrootPgmPathc;
  DbrootPgmPathc.rawAssign( DbrootPgmPath );
  
  sqlGetDbrootPgmPath( DbrootPgmPathc, TERM_WITH_DELIMITER_EO01, &RteError ) ;

  if ( !(rc = RteError.RteErrCode == RTE_NO_ERROR_SP01) ) 
    en950_PrintRteError ( "sqlGetDbrootPgmPath failed:", &RteError ) ;

  return (rc) ;
}

#define MSG_O(x)   { sqloutwrite x ; }
/*-----------------------------------------------------------*/
int e952_call_clear ( PROG_ID *clear, 
					  int      num_args, 
					  char   **args )
/*-----------------------------------------------------------*/
{
  int call_ret = NOTOK;

  DBG0 (( "\ncall_clear called with %d parameters\n", num_args ));
  if ( num_args == 1 )
    {   
      tsp00_DbNamec DbNamec;
      DbNamec.rawAssign( args[0] );
      if ( sqlXParamCheckExists ( DbNamec ) )
        { tsp00_Pathc pgm_path , full_progpath ;
          if ( en950_GetProgramExecPath ( clear->ProgramLocation, pgm_path ) )
            {
              if ( !ClearCalledFromStop )
              {
                  sprintf ( full_progpath, "%s/%s", pgm_path, clear->progname );
                  call_ret = e950_call_prog ( full_progpath, num_args, args ) ;
              }
              sprintf ( full_progpath.asCharp(), "%s/%s", pgm_path.asCharp(), &clear->progname [ strlen ( clear->progname )+1] );
              call_ret = e950_call_prog ( full_progpath, num_args, args ) ;
              sprintf ( full_progpath.asCharp(), "%s/%s%s", pgm_path.asCharp(), SAPDB_DATABASE_DIRECTORY_PREFIX, args[0] ) ;
              DBG0 (( "\ntry to remove <%s>\n", full_progpath.asCharp() ));
              unlink ( full_progpath ) ;
              sprintf ( full_progpath.asCharp(), "%s/%s%s", pgm_path.asCharp(), "gw:", args[0] ) ;
              DBG0 (( "\ntry to remove <%s>\n", full_progpath.asCharp() ));
              unlink ( full_progpath ) ;

              {
                  tsp00_Pathc spoolPath;
                  sql41_get_spool_dir(spoolPath);

                  sprintf ( full_progpath.asCharp(), "%s%s/%s", spoolPath.asCharp(), SAPDB_SPOOL_DIAG_DIRECTORY, args[0] ) ;
              }
              DBG0 (( "\ntry to remove <%s>\n", full_progpath.asCharp() ));
              unlink ( full_progpath ) ;
            }
          else
            { printf( "\n$DBROOT is not set!\n" ) ;
              call_ret = NOTOK ;
            }
        }
      else
        printf("\nparam file <%s> not found!\nclear not executed!\n", args[0] ) ;
    }
  else
    { call_ret = NOTOK ;
      USAGE_CLEAR(clear->progid)
    }
  return ( call_ret ) ;
}

/*-----------------------------------------------------------*/
/* PTS 1108781 / 1108471 / 1108798 */
int e952_call_regcomp ( PROG_ID *regcomp,
  					    int      num_args, 
					    char   **args )
/*-----------------------------------------------------------*/
{
    int call_ret = NOTOK;
    tsp01_RteError RteError;

    if ( sqlUpdateLibPathEnvironment(&RteError) )
    {
        call_ret = e950_call_component ( regcomp, num_args, args ) ;
    }
    else
    {
        printf("\nUpdate environment failed:%s\nregcomp not executed!\n", RteError.RteErrText ) ;
        call_ret = NOTOK;
    }

    return ( call_ret ) ;
}
/* ###################################################### */
#endif
/* ###################################################### */

/*-----------------------------------------------------------*/
int e952_call_start ( PROG_ID *start, 
					  int      num_args, 
					  char   **args )
/*-----------------------------------------------------------*/
{
  char *params[MAX_START_PARAMS+3], *serverdb = NULL; 
  int i, num_params=0, call_ret = TRUE;
  DBG0 (( "\ncall_start called with %d parameters\n", num_args ));

  for ( i=0 ; i<num_args && 
              call_ret != NOTOK && num_params < MAX_START_PARAMS ; i++ )
    if ( IS_OPT_CHR(*args[i]) )
      { if ( !strcmp ( &args[i][1], START_OPT_FAST ) ) ;
        else
        if ( !strcmp ( &args[i][1], START_OPT_SLOW ) ||
             !strcmp ( &args[i][1], START_OPT_QUICK) )
          { params[num_params] = args[i] ; num_params++ ; }
        else
        if ( !strcmp ( &args[i][1], &START_OPT_LOW_PRIO[1]) )
          { params[num_params] = args[i] ; num_params++ ; }
        else
        if ( !strcmp ( &args[i][1], &START_OPT_LOWER_PRIORITY[1]) )
          { params[num_params] = START_OPT_LOW_PRIO ; num_params++ ; }
        else
        if ( !strcmp ( &args[i][1], &START_OPT_ADMIN_MODE[1]) )
          { params[num_params] = args[i] ; num_params++ ; }
        else
        if ( !strcmp ( &args[i][1], &OPT_VERBOSE[1]) )
        { params[num_params] = args[i] ; num_params++ ; }
        else
        if ( !strcmp ( &args[i][1], &START_OPT_IGN_REST[1]) )
          { params[num_params] = args[i] ; num_params++ ; }
        else
        if ( !strcmp ( &args[i][1], &END_OF_PARSABLE_OPTIONS[1]) )
        {
          while ( i < num_args && num_params < MAX_START_PARAMS )
          {
              params[num_params] = args[i] ; num_params++ ; 
              ++i;
          }
          break;
        }
        else
          call_ret = NOTOK ;
        }
      else
      if ( !serverdb ) 
        { params[num_params] = "-d"    ; num_params++ ;
          params[num_params] = args[i] ; num_params++ ;
          serverdb = args[i] ;
        }
      else
        call_ret = NOTOK ;

  if ( call_ret == NOTOK || !serverdb )
    { call_ret = NOTOK ;
      e950_call_help (start->progid, start->prog_helpfiles ); 
    }
  else
    { SAPDB_UTF8 rundir[RTECONF_MAXSTRINGLENGTH+1] ;
    {
        tsp00_Pathc     DBRootOfDB;

        if ( RTE_GetDBRootOfDB(serverdb, DBRootOfDB, sizeof(DBRootOfDB)) )
        {
            e950_PutEnvDBRoot(DBRootOfDB) ;
        }
        else
        {
            printf("Installationpath of serverdb <%s> not found!", serverdb);
            return NOTOK;
        }
    }
        call_ret = !rundir_exists ( serverdb , &rundir[0] ) ? NOTOK :
                  e950_call_component ( start, num_params, params ) ; 
    }
  return ( call_ret ) ;
}

/*-----------------------------------------------------------*/
int e952_call_gwstart ( PROG_ID *start, 
					    int      num_args,
						char   **args )
/*-----------------------------------------------------------*/
{
  char **loc_params = (char **)malloc ( (num_args+1) * sizeof ( char *) ) ;
  int num_params = num_args+1 ;
  DBG0 (( "\ncall_gwstart called with %d parameters\n", num_args ));
  if ( loc_params )
    { SAPDB_memcpy( loc_params, args, num_args * sizeof ( char *) ) ;
      loc_params[num_args] = START_GATEWAY ;
      e952_call_start ( start, num_params, loc_params) ;
      free ( loc_params ) ;
    }
  return(TRUE);
}

/*===========================================================================*
 *  LOCAL/PRIVATE/PROTECTED FUNCTIONS (CODE)                                 *
 *===========================================================================*/


/*-----------------------------------------------------------*/
static int fill_stop_rec ( XSTOP_ARGS *stop_rec, 
						   int         num_args, 
						   char **     args )
/*-----------------------------------------------------------*/
{
  int fill_ok = num_args > 0 , act_arg = 0 ;
  SAPDB_memset ( stop_rec, '\0', sizeof ( XSTOP_ARGS ) ) ;
  while ( num_args && fill_ok )
    if ( IS_OPT_CHR ( *(args[act_arg]) ) )
    {
          if ( !SAPDB_strcasecmp(args[act_arg], STOP_OPT_IGN_SHUT) )
          {
              stop_rec->ign_shut = STOP_OPT_IGN_SHUT; act_arg++; --num_args ;
          }
          else if ( !SAPDB_strcasecmp(args[act_arg], STOP_OPT_DUMP) )
          {
              stop_rec->dumpflag = STOP_OPT_DUMP; act_arg++; --num_args ;
          }
          else if ( !SAPDB_strcasecmp(args[act_arg], OPT_VERBOSE) )
          {
              stop_rec->verbose = OPT_VERBOSE; act_arg++; --num_args ;
          }
          else
          {
              fill_ok = false;
          }
    }
    else
    {
      if ( !stop_rec->dbname ) 
      {
          stop_rec->dbname = args[act_arg] ; act_arg++; --num_args ;
      }
      else if ( !SAPDB_strcasecmp( args[act_arg], &STOP_OPT_IGN_SHUT[1] ) )
      {
          stop_rec->ign_shut = STOP_OPT_IGN_SHUT; act_arg++; --num_args ;
      }
      else if ( !SAPDB_strcasecmp( args[act_arg], &STOP_OPT_DUMP[1] ) )
      {
          stop_rec->dumpflag = STOP_OPT_DUMP; act_arg++; --num_args ;
      }
      else
      {
          fill_ok = false ;
      }
    }
  return ( fill_ok ) ;
}

/*-----------------------------------------------------------*/
static int e952_check_rundir ( char  *dbname, 
                               SAPDB_UTF8 *param_rundir )
/*-----------------------------------------------------------*/
{
  int check_ok ;

  DBG0 (( "e951_check_rundir for db <%s>\n", dbname ));
  RTEConf_Parameter parameterSet( dbname );
  Msg_List err;
  check_ok = parameterSet.GetRundirectoryAbsolutePath( &param_rundir[0], err );
  if ( check_ok )
    check_ok = e951_f_access ( (char *)param_rundir, S_IFDIR, FALSE ) ;

  return ( check_ok ) ;
}

/*-----------------------------------------------------------*/
static int rundir_exists ( char  *dbname, 
						   SAPDB_UTF8 *rundir )
/*-----------------------------------------------------------*/
{
  int Ok  ;

  tsp00_DbNamec DbNamec;
  DbNamec.rawAssign(dbname);

  Ok = sqlXParamCheckExists ( DbNamec ) ;

  if ( ! Ok  )
    printf("\nCannot read param-file for serverdb <%s>\n", dbname);
  else
    if ( !(Ok = e952_check_rundir ( dbname, rundir)) )
      { printf("\nRun-directory %s is missing ", rundir ) ;
        printf("\nPlease create it and start again\n");
      }
  return ( Ok ) ;
}

/* ###################################################### */
#ifndef WIN32
/* ###################################################### */

/*-----------------------------------------------------------*/
static int check_db_to_start ( char *dbname, 
							   char *full_knl_path, 
							   char *knl_prog, 
                               int   type_flag )
/*-----------------------------------------------------------*/
{
  int check_ok ;
  tsp00_Pathc ipcdb , ipcus , dbrootPgm ;
  DBG0 (( "check_db_to_start: serverdb <%s> ...\n", dbname ));

  if ( ( check_ok = en952_GetDbrootPgmPath ( dbrootPgm ) ) )
  {
      SAPDB_UTF8 rundir[RTECONF_MAXSTRINGLENGTH+1];
      /* ignore current umask setting for directory creation */
      unsigned short saved_umask = umask(0) ;

      strcpy ( full_knl_path, dbrootPgm ) ;
      strcat ( full_knl_path, knl_prog ) ;
      {
          tsp00_Pathc spoolPath;
          sql41_get_spool_dir(spoolPath);

          sprintf ( ipcdb, "%s%s/%s%s", &spoolPath[0], SAPDB_SPOOL_IPC_DIRECTORY, SAPDB_DATABASE_DIRECTORY_PREFIX, dbname ) ;
          sprintf ( ipcus, "%s%s/%s%s", &spoolPath[0], SAPDB_SPOOL_IPC_DIRECTORY, SAPDB_USER_DIRECTORY_PREFIX, dbname ) ;
      }

      if ( ( check_ok = rundir_exists ( dbname, &rundir[0] ) ) )
      {
          if ( ( check_ok = e951_f_access ( (char *)rundir, F_RWX, FALSE ) ) )
          {
              check_ok = e951_create_path ( ipcdb, 0770 ) && /* database tag files only for owner and group */
                     e951_create_path ( ipcus, 0777 ) && /* user tag files for everybody... */
                     umask ( saved_umask ) == 0 ;
          }
          else
          {
              printf("\nYou don't have read+write+execute permission on the run-directory\n%s", rundir) ;
              printf("\nPlease check the mode and start again\n");
          }
      }
  }
  else
  {
      printf("\nDBROOT must be set!\n");
  }
  return ( check_ok ) ;
}


/* ###################################################### */
#endif
/* ###################################################### */

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
