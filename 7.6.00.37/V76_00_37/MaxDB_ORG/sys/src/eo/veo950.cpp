/*!
  @file           veo950.c
  @author         JoergM
  @special area   application startup program
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




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "hsp100.h"
#include "heo95.h"
#include "heo46.h"
#include "SAPDBCommon/SAPDB_string.h"
#ifdef _WIN32
#  include "gos00.h"    /* nocheck */
#else
#  include <sys/wait.h>
#endif
/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#ifdef NO_INLINES
# define _INLINE
#else
#ifndef _INLINE
# define _INLINE	__inline
#endif
#endif

#define ENV_DBROOT_ID "DBROOT"
#define ERR_EXAMINE_DBROOT_OR_FILENAME "can't get dbroot or filename from '%s'"

#ifdef OS2
#define OS_VERS "OS/2"
typedef  void   ( * SIGNALFUNCTYPE )( int SigType );
#define  _SIGNALFUNCTYPE        void
static _SIGNALFUNCTYPE catch_signal ( int sig );
#endif

#ifdef WIN32
#define OS_VERS "WIN32"
typedef  void   ( __cdecl * SIGNALFUNCTYPE )( int SigType );
#define  _SIGNALFUNCTYPE        void __cdecl
static _SIGNALFUNCTYPE catch_signal ( int sig );
#else
#define OS_VERS "UNIX"
#ifdef MX5
typedef  int    ( * SIGNALFUNCTYPE )( );
#define  _SIGNALFUNCTYPE        int
#else
typedef  void   (  * SIGNALFUNCTYPE )( int SigType );
#define  _SIGNALFUNCTYPE        void
#endif
#endif

#define PRINT_DBG_INFO     "D"
#define PRINT_VERSION_INFO "V"
#define CALL_DEBUGGER      "DBG"

/* --- ADIS 1000065 */

#ifdef WIN32
#define ENV_PATH_DELIMITER ';'
#else
#define ENV_PATH_DELIMITER ':'
#define KSH_ENV "ENV"
#endif


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/


/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/

static int            reset_terminal = FALSE ;
static char          *debugger_name = NULL ;
static int            eo950_argc = 0;
static const char **  eo950_argv = NULL;


/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

#ifndef WIN32
static char *en950GetModuleFilename ( char       *Filename,
                                      tsp00_Pathc  FullPath ) ;

static int exec_sql_prog            ( PROG_ID    *sel_component,
                                      char       *sql_prog,
                                      int         num_args,
                                      char      **args );

static int call_prog_win            ( char       *progname,
                                      int         num_params,
                                      char      **params ) ;

static void reset_term              () ;
#endif

static int e950_call_sql_prog       ( char       *sql_prog,
                                      int         num_prog_args,
                                      char      **prog_args ) ;

static int call_selected_func       ( PROG_ID    *sel_prog,
                                      int         num_args,
                                      char      **args );

static int show_hlp_file            ( char       *helpfile ) ;

static int search_prog              ( char       *prog_to_search,
                                      PROG_ID    *progs ,
                                      int         num_progs) ;

static int is_batchmode             ( int         num_args,
                                      char      **args ) ;

#ifdef _WIN32
static _SIGNALFUNCTYPE catch_signal ( int sig );
#else
extern "C" /*static*/ _SIGNALFUNCTYPE catch_signal ( int sig );
#endif

static int en950TestPathFunctions   ( char *DbRoot );

/*===========================================================================*
 *  PUBLIC FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

/*-----------------------------------------------------------*/
int eo950_ArgC ()
/*-----------------------------------------------------------*/
{
    return eo950_argc;
}

/*-----------------------------------------------------------*/
const char **  eo950_ArgV ()
/*-----------------------------------------------------------*/
{
    return eo950_argv;
}

/*-----------------------------------------------------------*/
extern "C" int en950ApplStartupProgram ( int    NumArgs,
                                         char **Args )
/*-----------------------------------------------------------*/
{
  int rc;
  tsp00_Pathc              szDBRoot;
  tsp00_Pathc              szProgName ;
  char                   *ProgToCall;
  int                     FirstArgNum ;
  tsp01_RteError          RteError;

#ifdef DEBUG_RTE
    debug = TRUE ;
#endif

/* PTS 1108841 */

  eo950_argc = NumArgs;
  eo950_argv = (const char **)Args;
  if ( !sqlGetMyModuleFilename( Args[0], szProgName, sizeof(tsp00_Pathc), &RteError ) )
  {
    en950_PrintRteError ( "sqlGetMyModuleFilename failed.", &RteError ) ;
    return ( 1 );
  }
  DBG0 (( "sqlGetMyModuleFilename: %s\n",szProgName.asCharp() ));

  sqlGetDBrootFromExecPath ( szProgName, szDBRoot, TERM_WITHOUT_DELIMITER_EO01 ) ;

  if ( szDBRoot[0] == '\0' ||
       (ProgToCall = (char *) strrchr ( szProgName , PATH_DELIMITER_EO01 ) ) == NULL )
  {
    fprintf ( stderr, ERR_EXAMINE_DBROOT_OR_FILENAME, szProgName ) ;
    return ( 1 );
  }

  strcpy(szProgName, ProgToCall+1 ) ;

  if ( strchr ( szProgName, '.' ) )
    *(strchr ( szProgName, '.' )) = '\0' ;

  if ( NumArgs > 1 && !strcmp ( szProgName, "sql_run" ) )
  {
    FirstArgNum = 2 ;
    ProgToCall = Args[1] ;
  }
  else
  {
    FirstArgNum = 1 ;
    ProgToCall = szProgName ;
  }

  if ( NumArgs > FirstArgNum && IS_OPT_CHR (*(Args[FirstArgNum]) ) &&
       !strcmp ( &(Args[FirstArgNum][1]), PRINT_DBG_INFO ) )
  {
    debug = TRUE ;
    FirstArgNum++ ;
  }

  DBG0 (( "\nrunning in debug-mode ...\n" ));
  DBG0 (( "en950ApplStartupProgram: DBroot  : %s \n",szDBRoot.asCharp() ));
  DBG0 (( "en950ApplStartupProgram: Progname: %s \n",ProgToCall ));

  e950_PutEnvDBRoot( szDBRoot ); /* PTS 1108841*/

#ifdef TEST_ENV
  en950TestPathFunctions(szDBRoot);
#endif

  rc = e950_call_sql_prog ( ProgToCall , NumArgs-FirstArgNum, &Args[FirstArgNum] ) ;
  return (rc);
}

/* PTS 1108841 */
/*-----------------------------------------------------------*/
void e950_PutEnvDBRoot ( tsp00_Pathc DBRoot )
/*-----------------------------------------------------------*/
{
  static char szPutEnvDBRoot[sizeof(tsp00_Pathc) + sizeof(ENV_DBROOT_ID) + 2 ];
  sprintf(szPutEnvDBRoot, "%s=%s", ENV_DBROOT_ID, DBRoot.asCharp() );
  DBG0 (( "\nputenv(%s)\n", &szPutEnvDBRoot[0]));
  putenv(szPutEnvDBRoot) ;
}

/*-----------------------------------------------------------*/
int e950_search_and_call_sqlprog ( char  *sql_prog,
                                   int    num_params,
                                   char **params )
/*-----------------------------------------------------------*/
{
  int id = search_prog ( sql_prog, proglist, PROGLIST_SIZE ),
      ok = id != NOTOK, rc ;

  if ( !ok )
    { printf ("\nunknown progname %s \n", sql_prog ) ;
      rc = 1 ;
    }
  else
    rc = call_selected_func ( & proglist [ id ],num_params, params ) ;

  return ( rc ) ;
}

/*-----------------------------------------------------------*/
int e950_call_help ( char  *progname,
                     char **helpfiles )
/*-----------------------------------------------------------*/
{
  tsp00_Pathc full_helpfile ,
             helpfile_path ;
  int rc ;
  tsp01_RteError  RteError ;

  rc = sqlGetDbrootEnvPath ( helpfile_path, TERM_WITH_DELIMITER_EO01, &RteError ) ? 0 : NOTOK ;

  DBG0 (( "\ne950_call_help for %s called\n", progname ));
  if ( rc != NOTOK )
    { printf( SQL_HELP_HEADER, progname ) ;
      do
        { strcpy ( full_helpfile, helpfile_path  );
          strcat ( full_helpfile, *helpfiles );
          DBG0 (( "\nfull_helpfile_path = %s\n", full_helpfile.asCharp() )) ;
          show_hlp_file ( full_helpfile ) ;
        }
      while ( *(++helpfiles) ) ;
    }
  return ( rc ) ;
}

/*-----------------------------------------------------------*/
int e950_call_component ( PROG_ID     *sel_component,
                          int          num_args,
                          char       **args )
/*-----------------------------------------------------------*/
{
  tsp00_Pathc full_progpath ;
  int rc ;
  SIGNALFUNCTYPE oldsigint;
  oldsigint = signal ( SIGINT , catch_signal );
  DBG0 (( "\ncomponent %s called\n", sel_component->progid ));
  if ( debug ) e950_print_params ( "e950_call_component", num_args, args ) ;

  if ( (rc = en950_GetProgramExecPath ( sel_component->ProgramLocation, full_progpath.asCharp() )  ) )
  {
      int pathLen = strlen(full_progpath);
      char *progname = sel_component->progname;
      while ( *progname )
      {
            /* 'console' was changed to 'cons' on UNIX. In order to guaranty that we find */
            /* the old 'console' we are searching first for 'cons' and afterwards         */
            /* trying 'console' if 'cons was not found.                                   */
            /* After i did this with strt.exe too, i put in this code for all OS          */
          sprintf ( full_progpath.asCharp(), "%*.*s%c%s" ,
                    pathLen, pathLen, full_progpath.asCharp(), PATH_DELIMITER_EO01, progname );
          if ( !e951_f_access (full_progpath, S_IFREG, FALSE) )
          {
              DBG0 (( "\ne950_call_component: progname = '%s' not found (ignored)\n", full_progpath ));
              progname += strlen(progname) + 1;
          }
          else
          {
              break;
          }
      }
#ifndef WIN32
      if ( sel_component->init_term && !is_batchmode ( num_args, args ) )
        { SIGNALFUNCTYPE oldsigquit;
          DBG0 (( "\ninit_term set and batchmode is false" ));
          if ( (char *)getenv ( KSH_ENV ) )
            { static char new_env_var[20] ;
              DBG0 (( "\nEnvironmentvariable %s has value '%s'\n" ,
                      KSH_ENV, getenv(KSH_ENV) ));
              DBG0 (( "\nresetting '%s' to avoid terminal problems!\n", KSH_ENV ));
              sprintf ( new_env_var, "%s= ", KSH_ENV ) ;
              putenv ( new_env_var ) ;
              DBG0 (( "\nEnvironmentvariable %s has now value '%s'\n" ,
                      KSH_ENV, getenv(KSH_ENV) ));
            }
          oldsigquit= signal ( SIGQUIT, catch_signal );
          if ( (char *)getenv ( TERM ) || (char *)getenv ( DBTERM ) )
            { 
              TERMIO_INFO   save_terminfo ;
              (void) GET_TERMIO ( 0 , &save_terminfo  );
              rc = exec_sql_prog ( sel_component,full_progpath, num_args, args);
              if ( sel_component->init_term != NO_TERM )
                (void) SET_TERMIO ( 0 , &save_terminfo  );
              if ( reset_terminal ) reset_term();
            }
          else
            { printf ( "\nset and export %s or %s\n", TERM, DBTERM ) ;
              rc = NOTOK ;
            }
          signal ( SIGQUIT, oldsigquit);
        }
      else
#endif /* WIN32 */
        rc = e950_call_prog ( full_progpath, num_args, args ) ;
    }
  else
    rc = NOTOK ;
  signal ( SIGINT , oldsigint );
  return ( rc ) ;
}

/*-----------------------------------------------------------*/
void en950_PrintRteError ( char           *HeaderInfo,
                           tsp01_RteError *RteError)
/*-----------------------------------------------------------*/
{
  if ( RteError->RteErrCode != RTE_NO_ERROR_SP01 ||
       RteError->OsErrCode  != OS_NO_ERROR_SP01 )
  {
    fprintf ( stderr, "\n%s\n" , HeaderInfo);
    fprintf ( stderr, "OS_ERROR  %d: %s\n" , RteError->OsErrCode, RteError->OsErrText);
    fprintf ( stderr, "RTE_ERROR %d: %s\n" , RteError->RteErrCode, RteError->RteErrText);
  }
  return ;
}

#ifdef WIN32
/*-----------------------------------------------------------*/
int e950_call_prog ( char  *progname,
                     int    num_params,
                     char **params )
/*-----------------------------------------------------------*/
{
  int i/*, rc???, sql01c_set_symbol_path () */;
  DWORD rc;
  tsp01_RteError RteError ;
  char * CmdLineArgs;
  int CmdLineLen;
  STARTUPINFO StartupInfo ;
  PROCESS_INFORMATION Pi;

  DBG0 (( "\ne950_call_prog: progname = '%s'\n", progname ));

  eo46_rte_error_init( &RteError );

  rc = 1;
  /*
   * calculate length of command line
   */
  CmdLineLen = strlen (progname) + 2; /* two quotes */
  for ( i=0; i < num_params; ++i)
  {
      CmdLineLen += strlen (params [i]) + 3; /* blank + two quotes */
  }
  /*
   * build command line
   */
  CmdLineArgs = (char *)malloc (CmdLineLen+1);
  if (CmdLineArgs == NULL) {
      return 1;
  }
  strcpy( CmdLineArgs, "\"") ;
  strcat( CmdLineArgs, progname) ;
  strcat( CmdLineArgs, "\"") ;

  for ( i=0; i < num_params; i++ )
  {
    strcat(CmdLineArgs, " ") ;
    if ( strchr ( params[i], ' ' ) )
    {
      strcat( CmdLineArgs, "\"") ;
      strcat(CmdLineArgs, params[i]) ;
      strcat( CmdLineArgs, "\"") ;
    } else {
      strcat(CmdLineArgs, params[i]) ;
    }
  }

  /*
   * start program
   */
  GetStartupInfo( &StartupInfo ) ;

  sql01c_set_symbol_path ( NULL ) ;

  DBG0 (( "\ne950_call_prog: CmdLineArgs = '%s'\n", CmdLineArgs )) ;

  if ( !CreateProcess( progname, CmdLineArgs, NULL, NULL, TRUE, 0, NULL, NULL, &StartupInfo, &Pi) )
    eo46_set_rte_error (&RteError, GetLastError(), "Error during CreateProcess: ", progname);
  else
    if ( WaitForSingleObject( Pi.hProcess, INFINITE ) == WAIT_FAILED )
      eo46_set_rte_error (&RteError, GetLastError(), "Error during wait of process termination", NULL);
    else
      if ( !GetExitCodeProcess( Pi.hProcess, &rc ) )
        eo46_set_rte_error (&RteError, GetLastError(), "GetExitCodeProcess failed!", NULL);

  if ( RteError.OsErrCode != OS_NO_ERROR_SP01 )
  {
    fprintf ( stderr, "OS_ERROR  %d: %s\n" , RteError.OsErrCode, RteError.OsErrText);
    fprintf ( stderr, "RTE_ERROR %d: %s\n" , RteError.RteErrCode, RteError.RteErrText);
  }

  free (CmdLineArgs);
  DBG0 (( "\ne950_call_prog: returncode is %d\n", rc ));
  return (int) rc ;
}

#else /* it's UNIX */

extern "C" void en01CheckForDBUmask();

/*-----------------------------------------------------------*/
int e950_call_prog ( char  *progname,
                     int    num_params,
                     char **params )
/*-----------------------------------------------------------*/
{
  int status, rc ;
  char **loc_params = (char **)malloc ( (num_params+3) * sizeof ( char *) ) ;
  char err_str [ 80 ] ;

  DBG0 (( "\ne950_call_prog: progname = %s with %d param(s)\n",
          progname, num_params ));

  if ( loc_params != (char **)NULL )
    { if ( debugger_name == CNULL )
        { loc_params[0] = progname ;
          SAPDB_memcpy ( &loc_params[1], params, (num_params) * sizeof ( char* ) ) ;
        }
      else
        { loc_params[1] = progname ;
          progname = loc_params[0] = debugger_name ;
          SAPDB_memcpy ( &loc_params[2], params, (num_params) * sizeof ( char* ) ) ;
          num_params++ ;
        }
      loc_params[num_params+1] = CNULL ;
      if ( debug )
        e950_print_params ( "e950_call_prog", num_params, loc_params+1 ) ;
    }
  else
    { perror ( "\nmalloc error" ) ; return ( NOTOK ) ; }

/* PTS 1106388 */
  en01CheckForDBUmask();

  switch ( fork() )
    { case -1 : perror ( "\nfork error" ); rc = NOTOK; break ;
      case  0 : rc = execvp ( progname, loc_params ) ;
                sprintf(err_str, "\nCan't exec program <%s> ", progname ) ;
                perror ( err_str ) ;
                rc = NOTOK;
                exit(1);
                break ;
      default : while ( (rc = wait ( &status )) < 0 && errno == EINTR ) ;
                if ( rc < 0 )
                  { perror ( "\nwait error" ); rc = NOTOK ; }
                else
                   rc = status ;
                break ;
    }

  free ( (char *)loc_params ) ;
  DBG0 (( "\ne950_call_prog: returncode is %d\n", rc ));
  return ( rc ) ;
}
#endif

/*-----------------------------------------------------------*/
void e950_print_params ( char *funcname,
                         int   num_params,
                         char **params )
/*-----------------------------------------------------------*/
{
  if ( debug )
    { int i;
      DBG0 (("\nprinting %d parameter for function %s",num_params, funcname));
      for ( i=0; i<num_params; i++)
        DBG0 (("\n%2d. %s", i+1, params[i] ));
      DBG0 (("\n"));
    }
  return ;
}
#if 0
externC tsp00_Bool
sqlPutDbNameToEnv ( tsp00_DbNamec VAR_ARRAY_REF dbname );
/*-----------------------------------------------------------*/
int e950_put_dbname_to_env ( char *dbname )
/*-----------------------------------------------------------*/
{
  int put_success ;
  if ( ! (put_success = (int) sqlPutDbNameToEnv((tsp00_DbNamec *)dbname) ))
    printf("\ncan't expand environment with <%s>\n", dbname ) ;
  DBG0 (( "\ne950_put_dbname_to_env: %s \n", dbname ));
  return ( put_success ) ;
}
#endif
/*-----------------------------------------------------------*/
boolean en950_GetProgramExecPath ( enum ProgramExecPathType   ProgramLocation,
                                   char                      *GetProgramExecPath )
/*-----------------------------------------------------------*/
{
  tsp01_RteError  RteError ;
  boolean         GetOk ;
  int             DelimiterEnding = TERM_WITHOUT_DELIMITER_EO01 ;
  tsp00_Pathc     GetProgramExecPathc;

  switch ( ProgramLocation )
  {
    case IndependentPGM :
            GetOk = sqlGetIndependentPgmPath ( GetProgramExecPathc,
                                               DelimiterEnding,
                                               &RteError ) ;
            break ;
    case IndependentBIN :
            GetOk = sqlGetIndependentBinPath ( GetProgramExecPathc,
                                               DelimiterEnding,
                                               &RteError ) ;
            break ;
    case DbrootPGM      :
            GetOk = sqlGetDbrootPgmPath( GetProgramExecPathc, DelimiterEnding, &RteError ) ;
            break ;

    case DbrootBIN      :
            GetOk = sqlGetDbrootBinPath( GetProgramExecPathc, DelimiterEnding, &RteError ) ;
            break ;

    case DbrootSAP      : /* PTS 1108841 */
            GetOk = sqlGetDbrootSapPath( GetProgramExecPathc, DelimiterEnding, &RteError ) ;
            break ;
    case NoExecPath     :
    default             :
            eo46_set_rte_error (&RteError, 0, "Program location not implemented!", "");
            GetOk = FALSE ;
  }

  if ( !GetOk )
  {
    *GetProgramExecPath = '\0' ;
    en950_PrintRteError ("en950_GetProgramExecPath failed:", &RteError) ;
  }
  strcpy( GetProgramExecPath, GetProgramExecPathc.asCharp());
  DBG0 (( "\nen950_GetProgramExecPath: %s \n", GetProgramExecPath ));
  return ( GetOk ) ;
}

/*===========================================================================*
 *  LOCAL/PRIVATE/PROTECTED FUNCTIONS (CODE)                                 *
 *===========================================================================*/


/*-----------------------------------------------------------*/

#ifdef TEST_ENV

#define PUT_RTE_ERROR\
  if ( !ok ) \
    { fprintf ( stderr, "OS_ERROR  %d: %s\n" , RteError.OsErrCode, RteError.OsErrText);\
      fprintf ( stderr, "RTE_ERROR %d: %s\n" , RteError.RteErrCode, RteError.RteErrText); }

/*-----------------------------------------------------------*/
static int en950TestPathFunctions ( char *DbRoot )
/*-----------------------------------------------------------*/
{
  tsp00_Pathc              szProgName ;
  tsp01_RteError RteError ;
  boolean ok ;

#ifdef SET_INDEP
  ok = sqlSetIndependentProgramsPath(DbRoot,&RteError);
  fprintf ( stderr, "sqlSetIndependentProgramsPath: ok=%d\n", ok );
  PUT_RTE_ERROR ;
#endif

  ok = sqlGetIndependentProgramsPath(szProgName,0,&RteError);
  fprintf ( stderr, "sqlGetIndependentProgramsPath: %s, ok=%d\n", szProgName,ok );
  PUT_RTE_ERROR ;

  if ( !sqlIsIndependentProgramsPathInRegistry() )
    fprintf ( stderr, "!!! IndependentProgramsPath NOT FOUND in registry !!! using DBROOT\n");

#ifdef SET_INDEP
  ok = sqlSetIndependentDataPath(DbRoot,&RteError);
  fprintf ( stderr, "sqlSetIndependentDataPath: ok=%d\n", ok );
  PUT_RTE_ERROR ;
#endif

  ok = sqlGetIndependentDataPath(szProgName,1,&RteError);
  fprintf ( stderr, "sqlGetIndependentDataPath: %s, ok=%d\n", szProgName,ok );
  PUT_RTE_ERROR ;

  if ( !sqlIsIndependentDataPathInRegistry() )
    fprintf ( stderr, "!!! IndependentDataPath NOT FOUND in registry !!!, using DBROOT  !!!\n");

  ok = sqlGetIndependentPgmPath(szProgName,1,&RteError);
  fprintf ( stderr, "sqlGetIndependentPgmPath: %s, ok=%d\n", szProgName,ok );
  PUT_RTE_ERROR ;

  ok = sqlGetIndependentBinPath(szProgName,1,&RteError);
  fprintf ( stderr, "sqlGetIndependentBinPath: %s, ok=%d\n", szProgName,ok );
  PUT_RTE_ERROR ;

  ok = sqlGetIndependentConfigPath(szProgName,1,&RteError);
  fprintf ( stderr, "sqlGetIndependentConfigPath: %s, ok=%d\n", szProgName,ok );
  PUT_RTE_ERROR ;

  ok = sqlGetIndependentWrkPath(szProgName,1,&RteError);
  fprintf ( stderr, "sqlGetIndependentWrkPath: %s, ok=%d\n", szProgName,ok );
  PUT_RTE_ERROR ;
}
#endif

/*-----------------------------------------------------------*/
static int e950_call_sql_prog ( char  *sql_prog,
                                int    num_prog_args,
                                char **prog_args )
/*-----------------------------------------------------------*/
{
  int rc ;
  char **args = prog_args ;
  char                   *pszCompName;
  tsp00_Versionc         RTEVersionString;

  if ( num_prog_args > 0 && IS_OPT_CHR (*(prog_args[0]) ) )
  {
    if ( !strcmp ( &prog_args[0][1], PRINT_VERSION_INFO ) || debug )
    {
#       if _WIN32 || OS2
//          int  sql02_get_platform_id();
          if ( sql02_get_platform_id() == VER_PLATFORM_WIN32_NT )
	        pszCompName = COMP_NAME_RTE_NT_SP100;
          else
	        pszCompName = COMP_NAME_RTE_WIN9X_SP100;
#        else
            pszCompName = COMP_NAME_RTE_SP100;
#        endif

      sp100_GetVersionString(pszCompName, s100buildnumber, RTEVersionString);
      printf("\n%s (Application startup program)\n", RTEVersionString );
    }

    if ( num_prog_args > 1 && IS_OPT_CHR (*(args[0]) ) &&
         !strcmp ( &args[0][1], CALL_DEBUGGER ) )
    {
      debugger_name = args[1] ;
      num_prog_args -= 2 ;
      args = &(args[2]) ;
      DBG0 (( "\nusing debugger %s \n",debugger_name ));
    }
  }

  if ( debug ) e950_print_params ( "call_sql_prog", num_prog_args, args ) ;

  rc = e950_search_and_call_sqlprog ( sql_prog, num_prog_args, args ) ;

  return ( rc & 0xff ? rc & 0xff : rc >> 8 ) ;
}


/*-----------------------------------------------------------*/
static int search_prog ( char    *prog_to_search,
                         PROG_ID *progs ,
                         int      num_progs)
/*-----------------------------------------------------------*/
{
  char *prog_to_call, *conv_progname ;
  int found_id, found ;

  for ( prog_to_call = conv_progname = (char *) strdup ( prog_to_search ) ;
        *conv_progname ; conv_progname++ )
          *conv_progname = (char )tolower (*conv_progname);

  DBG0 (( "\nsearching for program %s, listlen = %d \n",
            prog_to_call, num_progs ));

  for ( found_id = 0, found = FALSE; found_id < num_progs &&
       !( found = !strcmp ( prog_to_call, progs [ found_id ].progid ) ||
                ( progs [ found_id ].alter_progid != CNULL &&
                  !strcmp ( prog_to_call, progs [ found_id ].alter_progid )
                )
        );
       found_id++ )
    DBG0 (( "\ncmp <%s> with <%s>\n", prog_to_call,
                                      progs [ found_id ].progid));

  return ( found ? found_id : NOTOK ) ;
}

/*-----------------------------------------------------------*/
static int call_selected_func ( PROG_ID *sel_prog,
                                int      num_args,
                                char    **args )
/*-----------------------------------------------------------*/
{
  int prog_ret = 0 ;
  if ( num_args > 0 && IS_OPT_CHR ( *(args[0]) ) &&
       ( !strcmp ( &args[0][1], HELP_ID ) ||
         !strcmp ( &args[0][1], ALTERNATE_HELP_ID ) ) &&
       sel_prog->prog_helpfiles )
    prog_ret = e950_call_help ( sel_prog->progid, sel_prog->prog_helpfiles ) ;
  else
    prog_ret = (*sel_prog->call_func) ( sel_prog, num_args, args ) ;

  if ( prog_ret != 0 && prog_ret != NOTOK )
    DBG0 (( "\nProgram exits with status 0x%04x \n",
                                   (unsigned short) prog_ret )) ;
  return ( prog_ret ) ;
}

/*-----------------------------------------------------------*/
static int show_hlp_file ( char *helpfile )
/*-----------------------------------------------------------*/
{
  FILE *stream = fopen ( helpfile, "r" ) ;
  int show_ok = stream != (FILE *) NULL ;

  if ( show_ok )
    { char line[ MAX_HLP_LINE_LEN + 1 ] ;
      do
        if ( (show_ok = fgets ( line, MAX_HLP_LINE_LEN , stream ) != CNULL) )
          printf ("%s", line ) ;
      while ( show_ok ) ;
      fclose ( stream ) ;
    }
  else
    { perror ( "open error" ) ;
      printf ( "missing helpfile  >%s<", helpfile ) ;
    }

  return ( show_ok ) ;
}

/*-----------------------------------------------------------*/
/*static */_SIGNALFUNCTYPE catch_signal ( int sig )
/*-----------------------------------------------------------*/
{

  signal ( sig, SIG_IGN);
  reset_terminal = TRUE ;

  return ;
}

#ifndef WIN32
/*-----------------------------------------------------------*/
static int exec_sql_prog ( PROG_ID *sel_component,
                           char    *sql_prog,
                           int      num_args,
                           char   **args )
/*-----------------------------------------------------------*/
{
  int exec_ret ;
#if HPUX
  if ( (char *)getenv ( "DISPLAY" ) != CNULL )
    { exec_ret = call_prog_win ( sql_prog, num_args, args ) ;
      sel_component->init_term = NO_TERM ;
    }
  else
#endif
    exec_ret = e950_call_prog ( sql_prog, num_args, args ) ;
  return ( exec_ret ) ;
}

#define DAP4X "dap4x"
#define DPTG  "dptg"
#define BA    "ba"
/*-----------------------------------------------------------*/
static void reset_term ()
/*-----------------------------------------------------------*/
{
  char *term_var = (char *)getenv ( TERM ) ;

  DBG0 (( "\nterminal must be reseted!\n"  ));
  if ( term_var != CNULL &&
       ( !strncmp ( term_var, DAP4X, strlen ( DAP4X ) ) ||
         !strncmp ( term_var, DPTG , strlen ( DPTG  ) ) ||
         !strncmp ( term_var, BA   , strlen ( BA    ) ) ) )
    printf ("\033[=K0\033[=M\033[H\033[J\033[=S\033[?1l\033[?7h");

  printf ( "\033[m\033[H\033[J" ) ;
  return ;
}

#if HPUX
#define X_GEN_WIN "x_genwin"
#define ENV_PROG_ID "PROG"
/*-----------------------------------------------------------*/
static int call_prog_win ( char  *progname,
                           int    num_params,
                           char **params )
/*-----------------------------------------------------------*/
{
  tsp00_Pathc env_prog ;
  int rc ;
  DBG0 (( "\ncall_prog_win called ...\n"  ));
  if ( debug ) e950_print_params ( "call_prog_win", num_params, params ) ;
  sprintf ( env_prog, "%s=%s", ENV_PROG_ID,
                      debugger_name ? debugger_name : progname ) ;
  if ( (int )putenv ( env_prog ) )
    { rc = NOTOK ;
      printf("\ncan't expand environment with <%s>\n", env_prog ) ;
    }
  else
    { char            cmdline[1000] ;
      tsp00_Pathc      BinPath ;
	  tsp01_RteError  RteError ;

      if ( (rc = en950_GetProgramExecPath ( IndependentBIN, BinPath ) ) ) /* PTS 1108036 */
        { int i ;
          if ( debugger_name == CNULL )
            sprintf ( cmdline, "%s%c%s", BinPath, PATH_DELIMITER_EO01, X_GEN_WIN );
          else
            sprintf ( cmdline, "%s%c%s %s",
                      BinPath, PATH_DELIMITER_EO01, X_GEN_WIN, progname );
          for ( i=0; i<num_params; i++ )
            { strcat ( cmdline, " " ) ; strcat ( cmdline, params[i] ) ; }
          DBG0 (( "\ncall_prog_win called cmd:\n%s\n", &cmdline[0]  ));
          rc = system ( cmdline ) ;
        }
      else
        rc = NOTOK ;
    }
  return ( rc ) ;
}

#endif    /* HPUX */

#endif    /* !WIN32 */

#define BATCH_ID_LCHR "b"
#define BATCH_ID_UCHR "B"
#define BATCH_ID_LSTR "batch"
#define BATCH_ID_USTR "BATCH"
#define BATCH_ID_IMPORT  "i"
#define BATCH_ID_VERSION "V"
/*-----------------------------------------------------------*/
static int is_batchmode ( int    num_args,
                          char **args )
/*-----------------------------------------------------------*/
{
  int i, is_batch = FALSE ;
  for ( i=0; i<num_args && !is_batch ; i++ )
    is_batch = ( IS_OPT_CHR ( *(args[i]) ) &&
                 ( !strcmp ( &args[i][1], BATCH_ID_VERSION ) ||
                   !strcmp ( &args[i][1], BATCH_ID_IMPORT  ) ||
                   !strcmp ( &args[i][1], BATCH_ID_LCHR ) ||
                   !strcmp ( &args[i][1], BATCH_ID_UCHR ) ||
                   !strcmp ( &args[i][1], BATCH_ID_LSTR )
                 )
               ) || !strcmp ( args[i] , BATCH_ID_USTR ) ;

  DBG0 (( "\nbatchmode is %s \n", is_batch ? "TRUE" : "FALSE" ));
  return ( is_batch ) ;
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/


