/*!
  @file           veo953.c
  @author         JoergM
  @special area   application startup program
  @brief          other applicaione (xcontrol, xkernprot, ... )
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

#include "heo01.h"
#include "heo95.h"
#include "heo12_1.h"
#define XPARAM_NAMES_ONLY
#include "geo002.h"
#include "gsp09.h"

#include "RunTime/RTE_GetDBRootOfDB.h"
#include "RunTime/Configuration/RTEConf_ParameterAccess.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define KERNPROT_DBID        "-d"
#define KERNPROT_USE_FILE    "-f"
#define DIAGNOSE_KERNPROT    "KERNPROT"

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

typedef struct          xkernprot_args
                        {
                          char *dbname;
                          char *cp_tr_id  ;
                          char *cp_trace_pages ;
                          char *use_inp_file ;
                          tsp00_Pathc inp_file  ;
                          tsp00_Pathc outp_file ;
                          char *switches;
                        } XKERNRPOT_ARGS ;

/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

static int set_inp_outp_filename ( XKERNRPOT_ARGS *kernprot_rec ) ;

static int fill_kernprot_rec ( XKERNRPOT_ARGS *kernprot_rec,
   					           int             num_args,
					           char          **args );

/*===========================================================================*
 *  PUBLIC FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

#if defined (_WIN32)
  #define PATHVAR_DELIMITER ";"
  #define PATH_SEPERATOR   "\\"
#else
  #define PATHVAR_DELIMITER ":"
  #define PATH_SEPERATOR   "/"
#endif

/*-----------------------------------------------------------*/
int e953_call_python ( PROG_ID *python,
 					   int      num_args,
					   char   **args )
/*-----------------------------------------------------------*/
{
  int             call_ret = 1;
  tsp01_RteError  RteError;
  boolean         GetOk;
  tsp00_Pathc     x_pythonPath;
  tsp00_Pathc     InstPath;
  int             InstPathLen;
  char          * endPath;
  static const char    * PythonPathEnvVar = "PYTHONPATH";
  static const char    * PythonHomeEnvVar = "PYTHONHOME";
  const char    * OldPythonPath = getenv (PythonPathEnvVar);
  const char    * libPath1 = "lib" PATH_SEPERATOR "python2.3";
  int             newPathLen;
  char          * newPathBuf;
#if !defined (_WIN32)
  int             newHomeLen;
  char          * newHomeBuf;
#endif

  DBG0 (( "\ncall_python called with %d parameters\n", num_args ));

  GetOk = sqlGetMyModuleFilename (eo950_ArgV () [0], x_pythonPath,
      sizeof (x_pythonPath), &RteError);
  if (!GetOk) {
      perror ( "\n sqlGetMyModuleFilename error" );
      return call_ret;
  }
  sqlGetInstallationPathFromExecPath (x_pythonPath, InstPath, TERM_WITH_DELIMITER_EO01);
  InstPathLen = strlen (InstPath);
  endPath = InstPath + InstPathLen;
  /*
   * set python path
   */
  newPathLen = (int)(strlen (PythonPathEnvVar) + 1 + InstPathLen
    + strlen (libPath1));
  if (OldPythonPath != NULL) {
      newPathLen += (int)(1 + strlen (OldPythonPath));
  }
  newPathBuf = (char *)malloc (newPathLen + 1);
  if (newPathBuf == NULL) {
      perror ( "\nmalloc error" );
      return call_ret;
  }
  sprintf (newPathBuf, "%s=%s%s", PythonPathEnvVar, InstPath.asCharp(), libPath1);
  if (OldPythonPath != NULL) {
      strcat (newPathBuf, PATHVAR_DELIMITER);
      strcat (newPathBuf, OldPythonPath);
  }
  putenv (newPathBuf);
  /*
   * set PYTHONHOME
   */
#if !defined (_WIN32)
  newHomeLen = strlen (PythonHomeEnvVar) + 1 + strlen (InstPath);
  newHomeBuf = (char *)malloc (newHomeLen + 1);
  sprintf (newHomeBuf, "%s=%s", PythonHomeEnvVar, InstPath.asCharp());
  putenv (newHomeBuf);
#endif
  /*
   * set ld_load_path
   *    not necessary, python is linked statically on UNIX
   */
  /*
   * call python executable
   */
  if ( call_ret != NOTOK) {
      call_ret = e950_call_component ( python, num_args, args ) ;
  }
  return ( call_ret ) ;
}

/*-----------------------------------------------------------*/
int e953_call_perl ( PROG_ID *perl,
 					   int      num_args,
					   char   **args )
/*-----------------------------------------------------------*/
{
  int             call_ret = 1;
  tsp01_RteError  RteError;
  boolean         GetOk;
  tsp00_Pathc     x_perlPath;
  tsp00_Pathc     InstPath;
  int             InstPathLen;
  char          * endPath;
  static const char    * PerlPathEnvVar = "PERL5LIB";
  const char    * OldPerlPath = getenv (PerlPathEnvVar);
  const char    * libPath1 = "lib" PATH_SEPERATOR "perl";
  const char    * libPath2 = "env";
  int             newPathLen;
  char          * newPathBuf;
#if !defined (_WIN32)
  int             newHomeLen;
  char          * newHomeBuf;
#endif

  DBG0 (( "\ncall_perl called with %d parameters\n", num_args ));

  GetOk = sqlGetMyModuleFilename (eo950_ArgV () [0], x_perlPath,
      sizeof (x_perlPath), &RteError);
  if (!GetOk) {
      perror ( "\n sqlGetMyModuleFilename error" );
      return call_ret;
  }
  sqlGetInstallationPathFromExecPath (x_perlPath, InstPath, TERM_WITH_DELIMITER_EO01);
  InstPathLen = strlen (InstPath);
  endPath = InstPath + InstPathLen;
  /*
   * set PERL5LIB
   */
  newPathLen = (int)(strlen (PerlPathEnvVar) + 1 + InstPathLen
    + strlen (libPath1)) + 1 + InstPathLen + strlen (libPath2);
  if (OldPerlPath != NULL) {
      newPathLen += (int)(1 + strlen (OldPerlPath));
  }
  newPathBuf = (char *)malloc (newPathLen + 1);
  if (newPathBuf == NULL) {
      perror ( "\nmalloc error" );
      return call_ret;
  }
  sprintf (newPathBuf, "%s=%s%s", PerlPathEnvVar, InstPath.asCharp(), libPath1);
  strcat (newPathBuf, PATHVAR_DELIMITER);
  strcat (newPathBuf, InstPath);
  strcat (newPathBuf, libPath2);
  if (OldPerlPath != NULL) {
      strcat (newPathBuf, PATHVAR_DELIMITER);
      strcat (newPathBuf, OldPerlPath);
  }
  putenv (newPathBuf);
  /*
   * call python executable
   */
  if ( call_ret != NOTOK) {
      call_ret = e950_call_component ( perl, num_args, args ) ;
  }
  return ( call_ret ) ;
}

/* PTS 1108471 */
/*-----------------------------------------------------------*/
int e953_call_tracesort ( PROG_ID *tracesort,
 					      int      num_args,
						  char   **args )
/*-----------------------------------------------------------*/
{
  int call_ret =1, num_kernprot_args = 0 ;

  call_ret = e950_call_component ( tracesort, num_args, args ) ;
  return ( call_ret ) ;
}

/*-----------------------------------------------------------*/
int e953_call_kernprot ( PROG_ID *kernprot,
 					     int      num_args,
						 char   **args )
/*-----------------------------------------------------------*/
{
  int call_ret =1, num_kernprot_args = 0 ;
  XKERNRPOT_ARGS args_rec ;
  char *kernprot_args [ 10 ] ;

  DBG0 (( "\ncall_kernprot called with %d parameters\n", num_args ));

  if ( num_args == 0 )
  {
    num_kernprot_args = 2 ; /* -d KERNPROT  */
    kernprot_args[0]  = KERNPROT_DBID ;
    kernprot_args[1]  = DIAGNOSE_KERNPROT ;
  }
  else
    if ( !fill_kernprot_rec ( &args_rec, num_args, args ) )
    {
      e950_call_help (kernprot->progid, kernprot->prog_helpfiles );
      call_ret = NOTOK ;
    }
  else
    {
      if ( set_inp_outp_filename ( &args_rec ) )
      {
        if ( args_rec.inp_file[0] != '\0' )
        {
          if ( e951_f_access ( args_rec.inp_file, F_EX, TRUE ) )
          {
            /* PTS 1108471 / PTS 1113502
			if ( !args_rec.use_inp_file ) */
			{
     		  char *TraceSortArgs[2] ;
	    	  tsp00_Pathc SortTraceOutpFile ;

			  sprintf(SortTraceOutpFile.asCharp(), "%s.dat", args_rec.inp_file.asCharp() );
              TraceSortArgs[0] = args_rec.inp_file.asCharp() ;
              TraceSortArgs[1] = SortTraceOutpFile.asCharp() ;
              call_ret = e950_search_and_call_sqlprog ( "xtracesort", 2, TraceSortArgs ) ;
			  if ( call_ret != 0  )
     			return (NOTOK) ;
			  else
                 strcpy( args_rec.inp_file, SortTraceOutpFile);
			}

            num_kernprot_args += 4 ; /* -d KERNPROT outp inp */
            kernprot_args[0] = KERNPROT_DBID ;

            kernprot_args[1] = DIAGNOSE_KERNPROT ;
            kernprot_args[2] = args_rec.outp_file.asCharp() ;
            kernprot_args[3] = args_rec.inp_file.asCharp() ;

            if ( args_rec.switches )
            {
                num_kernprot_args++ ;  /* akbxmn */
                kernprot_args[4] = args_rec.switches ;
                printf ( "\nreading : %s"  , args_rec.inp_file.asCharp() ) ;
                printf ( "\ncreating: %s\n", args_rec.outp_file.asCharp() ) ;
            }
          }
          else
            call_ret = NOTOK ;
        }
      }
      else
        call_ret = NOTOK ;

    }

  if ( call_ret != NOTOK)
     call_ret = e950_call_component ( kernprot, num_kernprot_args,
                                      kernprot_args ) ;
  return ( call_ret ) ;
}

/* PTS 1108841 FH 10-Jan-2001                                                    */
/*-----------------------------------------------------------*/
int e953_call_PrecompiledTools  ( PROG_ID *PrecompiledTool,
        					      int      num_args,
		      	    			  char   **args )
/*-----------------------------------------------------------*/
{
  int call_ret = TRUE;
  tsp00_DbNamec  DefaultDBName ;
  tsp00_ErrTextc ErrTextc ;

  DBG0 (( "\ncall_PrecompiledTools called with %d parameters\n", num_args ));

  if ( GetDefaultDBName ( DefaultDBName, ErrTextc ) && *DefaultDBName != '\0' )
  {
    tsp00_Pathc   DBRootOfDB;
    DBG0(("DefaultDBName = %s\n", DefaultDBName.asCharp()))

	if ( RTE_GetDBRootOfDB(DefaultDBName, DBRootOfDB, sizeof(DBRootOfDB) ) )
    {
      DBG0(("DBROOT of %s is %s\n", DefaultDBName.asCharp(), DBRootOfDB.asCharp()));
      e950_PutEnvDBRoot ( DBRootOfDB ) ;
	}
    else
	{
      printf("Cant find installation path for <%s>! \n", DefaultDBName.asCharp());
      call_ret = NOTOK ;
	}
  }
  else
  {
    printf("Error during searching of the name of the database: ");
    printf("%s\n", *DefaultDBName != '\0' ? ErrTextc.asCharp() : "no database name found");
    call_ret = NOTOK ;
  }

  if ( call_ret != NOTOK)
     call_ret = e950_call_component ( PrecompiledTool, num_args, args ) ;

  return ( call_ret ) ;
}

/*===========================================================================*
 *  LOCAL/PRIVATE/PROTECTED FUNCTIONS (CODE)                                 *
 *===========================================================================*/


/*-----------------------------------------------------------*/
static int fill_kernprot_rec ( XKERNRPOT_ARGS *kernprot_rec,
   					           int             num_args,
					           char          **args )
/*-----------------------------------------------------------*/
{
  int fill_ok = TRUE , act_arg = 0 ;
  memset ( kernprot_rec, '\0', sizeof ( XKERNRPOT_ARGS ) ) ;
  while ( num_args && fill_ok )
    if ( IS_OPT_CHR ( *(args[act_arg]) ) )
    {
      if ( !strcmp ( &args[act_arg][1], &KERNPROT_DBID[1] ) )
      {
        if ( (fill_ok = num_args > 1) )
        {
          kernprot_rec->dbname = args[act_arg+1] ;
          num_args -= 2, act_arg+=2 ;
          DBG0(("fill_kernprot_rec: dbname: %s\n", kernprot_rec->dbname )) ;
        }
      }
      else
          if ( !strcmp ( &args[act_arg][1], &KERNPROT_USE_FILE[1] ) )
          {
            if ( (fill_ok = num_args > 1) )
            {
              kernprot_rec->use_inp_file = args[act_arg+1] ;
              num_args -= 2, act_arg+=2 ;
              DBG0(("fill_kernprot_rec: use file %s \n",
                     kernprot_rec->use_inp_file )) ;
            }
          }
          else
          {
            fill_ok = FALSE ;
            DBG0(("fill_kernprot_rec: unknown option: %s\n", args[act_arg] ));
          }
    }
    else
      if ( ! kernprot_rec->switches )
      {
        kernprot_rec->switches = args[act_arg] ;
        num_args-- ; act_arg++ ;
        DBG0(("kernprot_rec: switches: %s\n",kernprot_rec->switches ));
      }
      else
      {
        fill_ok = FALSE ;
        DBG0(("fill_kernrpot_rec: unknown option: %s\n", args[act_arg] ));
      }
  return ( fill_ok ) ;
}

/*-----------------------------------------------------------*/
static int set_inp_outp_filename ( XKERNRPOT_ARGS *kernprot_rec )
/*-----------------------------------------------------------*/
{
  int   set_ok = TRUE ;

  if ( kernprot_rec->dbname )
  { char *dummy_tr;
  if ( e951_getparam ( kernprot_rec->dbname ,
    PAN_KERNELTRACEFILE, &dummy_tr) == CNULL )
    set_ok = FALSE ;
  else
    if ( EO01_IS_ABSOLUT_PATH(dummy_tr) )
      strcpy (kernprot_rec->inp_file, dummy_tr ) ;
    else
    {
      RTEConf_Parameter parameterSet(kernprot_rec->dbname);
      Msg_List err;
      SAPDB_UTF8 dummy_rundir[RTECONF_MAXSTRINGLENGTH+1];
      if (! parameterSet.GetRundirectoryAbsolutePath( &dummy_rundir[0], err ))
        set_ok = FALSE ;
      else
      {
        set_ok = strlen((SAPDB_Char *)dummy_rundir) + strlen(dummy_tr) + 1 <
          sizeof (kernprot_rec->inp_file) ;
        if ( set_ok )
          sprintf(kernprot_rec->inp_file, "%s%c%s",
                  dummy_rundir, PATH_DELIMITER_EO01, dummy_tr ) ;
        else
          printf("set_inp_filename: path to long (%s%c%s)\n",
                  dummy_rundir, PATH_DELIMITER_EO01, dummy_tr ) ;
      }
    }
    if ( set_ok )
      sprintf(kernprot_rec->outp_file,"%s.prt", kernprot_rec->dbname) ;
  }
  else
    if ( kernprot_rec->use_inp_file )
    {
      strcpy( kernprot_rec->inp_file, kernprot_rec->use_inp_file );
      sprintf(kernprot_rec->outp_file,"%s.prt", kernprot_rec->use_inp_file);
    }

  return ( set_ok ) ;
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
