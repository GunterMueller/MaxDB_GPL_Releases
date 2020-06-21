/*!
  @file           heo01.h
  @author         JoergM
  @special area   general Path / Environment functions
  @brief          Interface for RTE functions, used by C/C++ modules ...
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


#ifndef HEO01_H
#define HEO01_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp01.h"

#if defined(_WIN32) /*------- _WIN32 -------------------*/

# if !defined(MAX_PATH)
#  include <windows.h>
# endif

#else  /*--------------------- UNIX --------------------*/

# if !defined(_POSIX_PATH_MAX)
#  undef  _POSIX_SOURCE
#  define _POSIX_SOURCE 1
#  include <limits.h>
# endif

#endif /*----------------- _WIN32/UNIX -----------------*/


/*===========================================================================*
 *  DEFINES / MACROS / TYPEDEFS                                              *
 *===========================================================================*/

#define TERM_WITHOUT_DELIMITER_EO01     0
#define TERM_WITH_DELIMITER_EO01        1

#if defined(_WIN32) /*------- _WIN32 -------------------*/

# define PATH_LEN_EO01	                  MAX_PATH
# define PATH_DELIMITER_EO01                '\\'
# define EO01_IS_ABSOLUT_PATH(_path) \
          (*_path == PATH_DELIMITER_EO01 || *_path == '%' || _path[1] == ':')

#else  /*--------------------- UNIX --------------------*/

# define PATH_LEN_EO01	                  _POSIX_PATH_MAX
# define PATH_DELIMITER_EO01                '/'
# define EO01_IS_ABSOLUT_PATH(_path) \
          (*_path == PATH_DELIMITER_EO01 || *_path == '$' )

#endif /*----------------- _WIN32/UNIX -----------------*/


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

/* ------------------------------------------------------------------
 * PUBLIC FUNCTION sqlGetUserSpecificWrkPath
 * ------------------------------------------------------------------
 * purpose:   gets the work path of the actual SAPDB user working directory
 *
 * If the environment variable "SDB_HOMEDIRECTORY" is found and a directory
 *     with such name exists is accessible, it is used. If it is not accessible,
 *     the environment variable is ignored.
 * Otherwise if a home directory exists the subdirectory ".sapdb" in the home
 *     directory is used as path. If such a subdirectory exists or can be
 *     created and is accessible, it is used.
 * NOTE If the directory does not exist this call will try to create it.
 *     On UNIX it does this with mode 0777, so use the current 'umask' setting
 *     if further restriction is wanted. Default should be umask 022.
 *     On Windows the CreateDirectory() with the default security descriptor
 *     NULL is used.
 * If still no valid directory is found and the program runs in an user environ-
 *     ment, the path is build by appending the username to the result of
 *     sqlGetIndependentWrkPath().
 *     If such a subdirectory exists or can be created (see above) and is
 *     accessible, it is used. If it is not, an error is returned.
 * Otherwise if not running in an user environment, the call falls back to
 * sqlGetIndependentServiceProtPath().
 *
 *   WrkPath                  - work path to the actual SAPDB for R/3 release
 *                              WrkPath is a C ( null terminated ) string
 *   TerminateWithDelimiter   - TERM_WITHOUT_DELIMITER_EO01
 *                            - TERM_WITH_DELIMITER_EO01
 *
 * return - pointer to WrkPath
 */
externC tsp00_Bool
sqlGetUserSpecificWrkPath     ( tsp00_Pathc VAR_ARRAY_REF    WrkPath,
                                int                          TerminateWithDelimiter,
                                tsp01_RteError              *RteError ) ;

/* ------------------------------------------------------------------
 * PUBLIC FUNCTION sqlGetWrkPath
 * ------------------------------------------------------------------
 * purpose:   gets the work path of the actual SAPDB for R/3 release
 *
 * parameters:
 *   WrkPath                  - work path to the actual SAPDB for R/3 release
 *                              WrkPath is a C ( null terminated ) string
 *   TerminateWithDelimiter   - TERM_WITHOUT_DELIMITER_EO01
 *                            - TERM_WITH_DELIMITER_EO01
 *
 * return - pointer to WrkPath
 */

externC char *sqlGetWrkPath    ( tsp00_Pathc VAR_ARRAY_REF  WrkPath,
                                 int                        TerminateWithDelimiter ) ;

/* ------------------------------------------------------------------
 * PUBLIC FUNCTION sqlGetDBrootFromExecPath
 * ------------------------------------------------------------------
 * purpose:   extracts the dbroot path of the given executable
 *
 * parameters:
 *   ExecPath                 - full path of the executable
 *                              (argv[0] / GetModuleFileName ) /V62/usr/pgm/putparam
 *
 *   DBrootPath               - /V62/usr[/]
 *
 *   TerminateWithDelimiter   - TERM_WITHOUT_DELIMITER_EO01
 *                            - TERM_WITH_DELIMITER_EO01
 *
 * return - pointer to DBrootPath
 */

externC char *sqlGetDBrootFromExecPath        ( tsp00_Pathc VAR_ARRAY_REF ExecPath,
                                                tsp00_Pathc VAR_ARRAY_REF DBroot,
                                                int                  TerminateWithDelimiter ) ;
/* ------------------------------------------------------------------
 * PUBLIC FUNCTION sqlGetInstallationPathFromExecPath
 * ------------------------------------------------------------------
 * purpose:   extracts the installation path of the given executable
 *
 * parameters:
 *   ExecPath                 - full path of the executable
 *                              (argv[0] / GetModuleFileName ) /V62/usr/pgm/putparam
 *
 *   InstPath [OUT}           - <dependent>[/], <independent>[/]
 *
 *   TerminateWithDelimiter   - TERM_WITHOUT_DELIMITER_EO01
 *                            - TERM_WITH_DELIMITER_EO01
 *
 * return - pointer to InstPath
 */

externC char *sqlGetInstallationPathFromExecPath ( tsp00_Pathc VAR_ARRAY_REF ExecPath,
                                                   tsp00_Pathc VAR_ARRAY_REF InstPath,
                                                   int                  TerminateWithDelimiter ) ;
/* PTS 1108827 */
/* ------------------------------------------------------------------
 * PUBLIC FUNCTION sqlGetMyModuleFilename
 * ------------------------------------------------------------------
 * purpose:   get the full filename / path of the calling process
 *
 * parameters:
 *   Filename                 - Windows: unused
 *                              UNIX   : argv[0]
 *
 *   FullPath                 - returning path
 *   MaxPathLen               - bytes available in FullPath
 *
 *   RteError                 - RTE / OS errorcode and errortext
 *                              ( if function returned FALSE )
 *
 * return - true if success, false if not
 */

externC tsp00_Bool sqlGetMyModuleFilename ( const char      *Filename,
                                            char            *FullPath,
				 	              		    int              MaxPathLen,
									        tsp01_RteError  *RteError );

/* PTS 1106596 */
/* ------------------------------------------------------------------
 * PUBLIC FUNCTION sqlIsIndependentDataPathInRegistry
 * ------------------------------------------------------------------
 * purpose:   checks if the  release independent "Data" Path
 *            used for ( <IndependentPath>/config, <IndependentPath>/wrk )
 *            if found in the registry
 *
 *
 * return - TRUE : the key for rhe release independent "Data"  path is present in the registry
 *          FALSE: a path is NOT present in the registry for the specified key
 */
externC tsp00_Bool
sqlIsIndependentDataPathInRegistry () ;

/* ------------------------------------------------------------------
 * PUBLIC FUNCTION sqlIsIndependentProgramsPathInRegistry
 * ------------------------------------------------------------------
 * purpose:   checks if the  release independent "Programs" Path
 *            used for ( <IndependentPath>/config, <IndependentPath>/wrk )
 *            if found in the registry
 *
 *
 * return - TRUE : the key for rhe release independent "Programs"  path is present in the registry
 *          FALSE: a path is NOT present in the registry for the specified key
 */
externC tsp00_Bool
sqlIsIndependentProgramsPathInRegistry () ;


/* ------------------------------------------------------------------
 * PUBLIC FUNCTION sqlGetIndependentDataPath
 * ------------------------------------------------------------------
 * purpose:   gets the release independent "Data" Path
 *            used for ( <IndependentPath>/config, <IndependentPath>/wrk )
 *
 * parameters:
 *   DataPath                 - path to the "Data" directory
 *                              Path is a C ( null terminated ) string
 *   TerminateWithDelimiter   - TERM_WITHOUT_DELIMITER_EO01
 *                            - TERM_WITH_DELIMITER_EO01
 *   RteError                 - RTE / OS errorcode and errortext
 *                              ( if function returned FALSE )
 *
 * return - TRUE : success
 *          FALSE: failed, for more information see "RteError"
 */
externC tsp00_Bool
sqlGetIndependentDataPath     ( tsp00_Pathc VAR_ARRAY_REF    DataPath,
                                int                          TerminateWithDelimiter,
                                tsp01_RteError              *RteError ) ;


/* ------------------------------------------------------------------
 * PUBLIC FUNCTION sqlGetIndependentProgramsPath
 * ------------------------------------------------------------------
 * purpose:   gets the release independent "Programs" Path
 *            used for ( <IndependentPath>/config, <IndependentPath>/wrk )
 *
 * parameters:
 *   ProgramsPath             - path to the "Programs" directory
 *                              Path is a C ( null terminated ) string
 *   TerminateWithDelimiter   - TERM_WITHOUT_DELIMITER_EO01
 *                            - TERM_WITH_DELIMITER_EO01
 *   RteError                 - RTE / OS errorcode and errortext
 *                              ( if function returned FALSE )
 *
 * return - TRUE : success
 *          FALSE: failed, for more information see "RteError"
 */
externC tsp00_Bool
sqlGetIndependentProgramsPath ( tsp00_Pathc VAR_ARRAY_REF    DataPath,
                                int                          TerminateWithDelimiter,
                                tsp01_RteError              *RteError ) ;

/* ------------------------------------------------------------------
 * PUBLIC FUNCTION sqlSetIndependentDataPath
 * ------------------------------------------------------------------
 * purpose:   sets the release independent "Data" Path
 *            used for ( <IndependentPath>/config, <IndependentPath>/wrk )
 *
 * parameters:
 *   DataPath                 - path to the "Data" directory
 *                              Path is a C ( null terminated ) string
 *   RteError                 - RTE / OS errorcode and errortext
 *                              ( if function returned FALSE )
 *
 * return - TRUE : success
 *          FALSE: failed, for more information see "RteError"
 */
externC tsp00_Bool
sqlSetIndependentDataPath     ( tsp00_Pathc VAR_ARRAY_REF    DataPath,
                                tsp01_RteError              *RteError ) ;


/* ------------------------------------------------------------------
 * PUBLIC FUNCTION sqlSetIndependentProgramsPath
 * ------------------------------------------------------------------
 * purpose:   sets the release independent "Programs" Path
 *            used for ( <IndependentPath>/bin, <IndependentPath>/pgm )
 *
 * parameters:
 *   ProgramsPath             - path to the "Programs" directory
 *                              Path is a C ( null terminated ) string
 *   RteError                 - RTE / OS errorcode and errortext
 *                              ( if function returned FALSE )
 *
 * return - TRUE : success
 *          FALSE: failed, for more information see "RteError"
 */
externC tsp00_Bool
sqlSetIndependentProgramsPath ( tsp00_Pathc  VAR_ARRAY_REF   DataPath,
                                tsp01_RteError              *RteError ) ;

/* ------------------------------------------------------------------
 * PUBLIC FUNCTION sqlGetIndependentPgmPath
 * ------------------------------------------------------------------
 * purpose:   gets the release independent "pgm" path ( x????.exe, ...)
 *
 * parameters:
 *   PgmPath                  - path to the "pgm" directory
 *                              Path is a C ( null terminated ) string
 *   TerminateWithDelimiter   - TERM_WITHOUT_DELIMITER_EO01
 *                            - TERM_WITH_DELIMITER_EO01
 *   RteError                 - RTE / OS errorcode and errortext
 *                              ( if function returned FALSE )
 *
 * return - TRUE : success
 *          FALSE: failed, for more information see "RteError"
 */
externC tsp00_Bool
sqlGetIndependentPgmPath      ( tsp00_Pathc VAR_ARRAY_REF    PgmPath,
                                int                          TerminateWithDelimiter,
                                tsp01_RteError              *RteError ) ;



/* ------------------------------------------------------------------
 * PUBLIC FUNCTION sqlGetIndependentLibPath
 * ------------------------------------------------------------------
 * purpose:   gets the release independent "lib" path
 *
 * parameters:
 *   LIBPath                  - path to the "lib" directory
 *                              Path is a C ( null terminated ) string
 *   TerminateWithDelimiter   - TERM_WITHOUT_DELIMITER_EO01
 *                            - TERM_WITH_DELIMITER_EO01
 *   RteError                 - RTE / OS errorcode and errortext
 *                              ( if function returned FALSE )
 *
 * return - TRUE : success
 *          FALSE: failed, for more information see "RteError"
 */
externC tsp00_Bool
sqlGetIndependentLibPath	   ( tsp00_Pathc VAR_ARRAY_REF  LIBPath,
                                 int						TerminateWithDelimiter,
                                 tsp01_RteError			   *pRteError );

/* ------------------------------------------------------------------

 * PUBLIC FUNCTION sqlGetIndependentBinPath
 * ------------------------------------------------------------------
 * purpose:   gets the release independent "bin" path ( x????.exe, ...)
 *
 * parameters:
 *   BinPath                  - path to the "bin" directory
 *                              Path is a C ( null terminated ) string
 *   TerminateWithDelimiter   - TERM_WITHOUT_DELIMITER_EO01
 *                            - TERM_WITH_DELIMITER_EO01
 *   RteError                 - RTE / OS errorcode and errortext
 *                              ( if function returned FALSE )
 *
 * return - TRUE : success
 *          FALSE: failed, for more information see "RteError"
 */

externC tsp00_Bool
sqlGetIndependentBinPath      ( tsp00_Pathc VAR_ARRAY_REF    BinPath,
                                int                          TerminateWithDelimiter,
                                tsp01_RteError              *RteError ) ;

/* ------------------------------------------------------------------
 * PUBLIC FUNCTION sqlGetIndependentEnvPath
 * ------------------------------------------------------------------
 * purpose:   gets the release independent "env" path ( data, configuration, ...)
 *
 * parameters:
 *   EnvPath                  - path to the "env" directory
 *                              Path is a C ( null terminated ) string
 *   TerminateWithDelimiter   - TERM_WITHOUT_DELIMITER_EO01
 *                            - TERM_WITH_DELIMITER_EO01
 *   RteError                 - RTE / OS errorcode and errortext
 *                              ( if function returned FALSE )
 *
 * return - TRUE : success
 *          FALSE: failed, for more information see "RteError"
 */

externC tsp00_Bool
sqlGetIndependentEnvPath      ( tsp00_Pathc VAR_ARRAY_REF    EnvPath,
                                int                          TerminateWithDelimiter,
                                tsp01_RteError              *RteError ) ;

/* ------------------------------------------------------------------
 * PUBLIC FUNCTION sqlGetIndependentTerminfoPath
 * ------------------------------------------------------------------
 * purpose:   gets the release independent "terminfo" path used by
 *            applications using the virtual terminal (userx, diagnose. ...)
 *
 * parameters:
 *   TerminfoPath             - path to the "terminfo" directory
 *                              Path is a C ( null terminated ) string
 *   TerminateWithDelimiter   - TERM_WITHOUT_DELIMITER_EO01
 *                            - TERM_WITH_DELIMITER_EO01
 *   RteError                 - RTE / OS errorcode and errortext
 *                              ( if function returned FALSE )
 *
 * return - TRUE : success
 *          FALSE: failed, for more information see "RteError"
 */

externC tsp00_Bool
sqlGetIndependentTerminfoPath ( tsp00_Pathc VAR_ARRAY_REF    TerminfoPath,
                                int                          TerminateWithDelimiter,
                                tsp01_RteError              *RteError ) ;

/* ------------------------------------------------------------------
 * PUBLIC FUNCTION sqlGetIndependentMsgPath
 * ------------------------------------------------------------------
 * purpose:   gets the release independent "msg" path ( *.xml, ...)
 *
 * parameters:
 *   MsgPath                  - path to the "msg" directory
 *                              Path is a C ( null terminated ) string
 *   TerminateWithDelimiter   - TERM_WITHOUT_DELIMITER_EO01
 *                            - TERM_WITH_DELIMITER_EO01
 *   RteError                 - RTE / OS errorcode and errortext
 *                              ( if function returned FALSE )
 *
 * return - TRUE : success
 *          FALSE: failed, for more information see "RteError"
 */
externC tsp00_Bool
sqlGetIndependentMsgPath      ( tsp00_Pathc VAR_ARRAY_REF    MsgPath,
                                int                          TerminateWithDelimiter,
                                tsp01_RteError              *RteError ) ;

/* ------------------------------------------------------------------
 * PUBLIC FUNCTION sqlGetIndependentProtPath
 * ------------------------------------------------------------------
 * purpose:   gets the release independent "prot" path ( xserver.prt, precompiler)
 *
 * parameters:
 *   ProtPath                 - path to the "prot" directory
 *                              Path is a C ( null terminated ) string
 *   TerminateWithDelimiter   - TERM_WITHOUT_DELIMITER_EO01
 *                            - TERM_WITH_DELIMITER_EO01
 *   RteError                 - RTE / OS errorcode and errortext
 *                              ( if function returned FALSE )
 *
 * return - TRUE : success
 *          FALSE: failed, for more information see "RteError"
 */

externC tsp00_Bool
sqlGetIndependentProtPath     ( tsp00_Pathc VAR_ARRAY_REF    PROTPath,
                                int                          TerminateWithDelimiter,
                                tsp01_RteError              *RteError ) ;

/* PTS 1108067 */
/* ------------------------------------------------------------------
 * PUBLIC FUNCTION sqlGetIndependentServiceProtPath (Windows Only)
 * ------------------------------------------------------------------
 * purpose:   gets the release independent "prot" path ( xserver.prt, ...)
 *            if there is no independent path set, the directory relative
 *            to the directory of the calling process is used and the suffix
 *            "prot" or "wrk" is added.
 *
 * parameters:
 *   ProtPath                 - path to the "prot" directory
 *                              Path is a C ( null terminated ) string
 *   TerminateWithDelimiter   - TERM_WITHOUT_DELIMITER_EO01
 *                            - TERM_WITH_DELIMITER_EO01
 *   RteError                 - RTE / OS errorcode and errortext
 *                              ( if function returned FALSE )
 *
 * return - TRUE : success
 *          FALSE: failed, for more information see "RteError"
 */

externC tsp00_Bool
sqlGetIndependentServiceProtPath ( tsp00_Pathc VAR_ARRAY_REF    ProtPath,
                                   int                          TerminateWithDelimiter,
                                   tsp01_RteError              *RteError ) ;

/* ------------------------------------------------------------------
 * PUBLIC FUNCTION sqlGetIndependentWrkPath
 * ------------------------------------------------------------------
 * purpose:   gets the release independent "wrk" path ( appldiag, ...)
 *
 * parameters:
 *   ConfigPath               - path to the "wrk" directory
 *                              Path is a C ( null terminated ) string
 *   TerminateWithDelimiter   - TERM_WITHOUT_DELIMITER_EO01
 *                            - TERM_WITH_DELIMITER_EO01
 *   RteError                 - RTE / OS errorcode and errortext
 *                              ( if function returned FALSE )
 *
 * return - TRUE : success
 *          FALSE: failed, for more information see "RteError"
 */

externC tsp00_Bool
sqlGetIndependentWrkPath      ( tsp00_Pathc VAR_ARRAY_REF    WrkPath,
                                int                          TerminateWithDelimiter,
                                tsp01_RteError              *RteError ) ;

/* Old style, use "sqlGetIndependentWrkPath" */

externC char *sqlGetWrkPath    ( tsp00_Pathc VAR_ARRAY_REF WrkPath,
                                 int                       TerminateWithDelimiter ) ;

/* ------------------------------------------------------------------
 * PUBLIC FUNCTION sqlGetIndependentConfigPath
 * ------------------------------------------------------------------
 * purpose:   gets the release independent "config" path ( paramfiles, ... )
 *
 * parameters:
 *   ConfigPath               - path to the "config" directory
 *                              Path is a C ( null terminated ) string
 *   TerminateWithDelimiter   - TERM_WITHOUT_DELIMITER_EO01
 *                            - TERM_WITH_DELIMITER_EO01
 *   RteError                 - RTE / OS errorcode and errortext
 *                              ( if function returned FALSE )
 *
 * return - TRUE : success
 *          FALSE: failed, for more information see "RteError"
 */

externC tsp00_Bool
sqlGetIndependentConfigPath   ( tsp00_Pathc  VAR_ARRAY_REF   ConfigPath,
                                int                          TerminateWithDelimiter,
                                tsp01_RteError              *RteError ) ;
/* Old style, use "sqlGetIndependentConfigPath" */

externC char *sqlGetConfigPath ( tsp00_Pathc VAR_ARRAY_REF ConfigPath,
                                 int                       TerminateWithDelimiter ) ;


/*!
   @brief          Update LIBPATH environment for NI
   @param          pEnvironmentAdjusted [out] true: Environment was adjusted
   @param          RteError [out] the error return
   @return         false if update failed (RteError is filled), true if successfull

   Used for starting the niserver so that it is guarantied that the shared objects needed for 
   NI comunication are found (e.g. libsapu16.so).
 
   It updates the shared library paths environment by prefix our libary path (<IndepPgmPath>/lib).
 */

externC tsp00_Bool
sqlUpdateNiLibPathEnvironment(SAPDB_Bool VAR_VALUE_REF        pEnvironmentAdjusted,
                              tsp01_RteError *                pRteError);

/* ------------------------------------------------------------------
 * PUBLIC FUNCTION sqlGetDbrootPath (use within vos78.cpp PTS 1107355)
 * ------------------------------------------------------------------
 * purpose:   gets the DBROOT path
 *
 * parameters:
 *   EnvPath                  - path DBROOT directory
 *                              Path is a C ( null terminated ) string
 *   TerminateWithDelimiter   - TERM_WITHOUT_DELIMITER_EO01
 *                            - TERM_WITH_DELIMITER_EO01
 *   RteError                 - RTE / OS errorcode and errortext
 *                              ( if function returned FALSE )
 *
 * return - TRUE : success
 *          FALSE: failed, for more information see "RteError"
 */

externC tsp00_Bool
sqlGetDbrootPath ( tsp00_Pathc  VAR_ARRAY_REF   Path,
                   int                          TerminateWithDelimiter,
                   tsp01_RteError              *RteError ) ;

/* ------------------------------------------------------------------
 * PUBLIC FUNCTION sqlGetDbrootEnvPath
 * ------------------------------------------------------------------
 * purpose:   gets the DBROOT dependent "env" path ( helpfiles, ... )
 *
 * parameters:
 *   EnvPath                  - path to the "env" directory
 *                              Path is a C ( null terminated ) string
 *   TerminateWithDelimiter   - TERM_WITHOUT_DELIMITER_EO01
 *                            - TERM_WITH_DELIMITER_EO01
 *   RteError                 - RTE / OS errorcode and errortext
 *                              ( if function returned FALSE )
 *
 * return - TRUE : success
 *          FALSE: failed, for more information see "RteError"
 */

externC tsp00_Bool
sqlGetDbrootEnvPath ( tsp00_Pathc  VAR_ARRAY_REF   EnvPath,
                      int                          TerminateWithDelimiter,
                      tsp01_RteError              *RteError ) ;


/* ------------------------------------------------------------------
 * PUBLIC FUNCTION sqlGetDbrootBinPath
 * ------------------------------------------------------------------
 * purpose:   gets the DBROOT dependent "bin" path ( helpfiles, ... )
 *
 * parameters:
 *   BinPath                  - path to the "bin" directory
 *                              Path is a C ( null terminated ) string
 *   TerminateWithDelimiter   - TERM_WITHOUT_DELIMITER_EO01
 *                            - TERM_WITH_DELIMITER_EO01
 *   RteError                 - RTE / OS errorcode and errortext
 *                              ( if function returned FALSE )
 *
 * return - TRUE : success
 *          FALSE: failed, for more information see "RteError"
 */

externC tsp00_Bool
sqlGetDbrootBinPath ( tsp00_Pathc  VAR_ARRAY_REF   BinPath,
                      int                          TerminateWithDelimiter,
                      tsp01_RteError              *RteError ) ;


/* ------------------------------------------------------------------
 * PUBLIC FUNCTION sqlGetDbrootPgmPath
 * ------------------------------------------------------------------
 * purpose:   gets the DBROOT dependent "pgm" path ( helpfiles, ... )
 *
 * parameters:
 *   PgmPath                  - path to the "pgm" directory
 *                              Path is a C ( null terminated ) string
 *   TerminateWithDelimiter   - TERM_WITHOUT_DELIMITER_EO01
 *                            - TERM_WITH_DELIMITER_EO01
 *   RteError                 - RTE / OS errorcode and errortext
 *                              ( if function returned FALSE )
 *
 * return - TRUE : success
 *          FALSE: failed, for more information see "RteError"
 */

externC tsp00_Bool
sqlGetDbrootPgmPath ( tsp00_Pathc VAR_ARRAY_REF    PgmPath,
                      int                          TerminateWithDelimiter,
                      tsp01_RteError               *RteError ) ;

/* ------------------------------------------------------------------
 * PUBLIC FUNCTION sqlGetDbrootWrkPath
 * ------------------------------------------------------------------
 * purpose:   gets the DBROOT dependent "wrk" path ( protfiles, ... )
 *
 * parameters:
 *   wrkPath                  - path to the "wrk" directory
 *                              Path is a C ( null terminated ) string
 *   TerminateWithDelimiter   - TERM_WITHOUT_DELIMITER_EO01
 *                            - TERM_WITH_DELIMITER_EO01
 *   RteError                 - RTE / OS errorcode and errortext
 *                              ( if function returned FALSE )
 *
 * return - TRUE : success
 *          FALSE: failed, for more information see "RteError"
 */

externC tsp00_Bool
sqlGetDbrootWrkPath ( tsp00_Pathc VAR_ARRAY_REF    WrkPath,
                      int                          TerminateWithDelimiter,
                      tsp01_RteError              *RteError ) ;

/* ------------------------------------------------------------------
 * PUBLIC FUNCTION sqlGetDbrootLibPath
 * ------------------------------------------------------------------
 * purpose:   gets the DBROOT dependent "lib" path ( libraries, shared libraries... )
 *
 * parameters:
 *   libPath                  - path to the "lib" directory
 *                              Path is a C ( null terminated ) string
 *   TerminateWithDelimiter   - TERM_WITHOUT_DELIMITER_EO01
 *                            - TERM_WITH_DELIMITER_EO01
 *   RteError                 - RTE / OS errorcode and errortext
 *                              ( if function returned FALSE )
 *
 * return - TRUE : success
 *          FALSE: failed, for more information see "RteError"
 */

externC tsp00_Bool
sqlGetDbrootLibPath ( tsp00_Pathc VAR_ARRAY_REF    LibPath,
                      int                          TerminateWithDelimiter,
                      tsp01_RteError              *RteError ) ;

/* ------------------------------------------------------------------
 * PUBLIC FUNCTION sqlGetDbrootSapPath
 * ------------------------------------------------------------------
 * purpose:   gets the DBROOT dependent "sap" path ( libraries, shared libraries... )
 *
 * parameters:
 *   SapPath                  - path to the "sap" directory
 *                              Path is a C ( null terminated ) string
 *   TerminateWithDelimiter   - TERM_WITHOUT_DELIMITER_EO01
 *                            - TERM_WITH_DELIMITER_EO01
 *   RteError                 - RTE / OS errorcode and errortext
 *                              ( if function returned FALSE )
 *
 * return - TRUE : success
 *          FALSE: failed, for more information see "RteError"
 */

externC tsp00_Bool
sqlGetDbrootSapPath ( tsp00_Pathc VAR_ARRAY_REF    SapPath,
                      int                          TerminateWithDelimiter,
                      tsp01_RteError              *RteError ) ;

/* ------------------------------------------------------------------
 * PUBLIC FUNCTION sqlGetPortableInstallationRootPath 
 * ------------------------------------------------------------------
 * purpose:   gets the root path for a portable installation
 *
 * parameters:
 *   SapPath                  - path to the portable installation directory
 *                              Path is a C ( null terminated ) string
 *   TerminateWithDelimiter   - TERM_WITHOUT_DELIMITER_EO01
 *                            - TERM_WITH_DELIMITER_EO01
 *   RteError                 - RTE / OS errorcode and errortext
 *                              ( if function returned FALSE )
 *
 * return - TRUE : success
 *          FALSE: failed, for more information see "RteError"
 */

externC tsp00_Bool 
sqlGetPortableInstallationRootPath (    tsp00_Pathc VAR_ARRAY_REF   Path,
                                        int                         TerminateWithDelimiter,
                                        tsp01_RteError             *pRteError );
externC tsp00_Bool
sqlGetEnvironment               ( char         *EnvName,
					                char	       *EnvValue,
					                int           MaxEnvValueLen);

externC void
sqlgetenv               (tsp00_C64      envname,
					               tsp00_C64      envvalue,
					               tsp00_BoolAddr envfound);

externC boolean eo01_IsEnvVarDoNotUseIndepPathSet();
externC void    eo01_PutEnvVarDoNotUseIndepPath() ;
externC void    eo01_PutEnvVarDBRoot(tsp00_Pathc   dbroot);
externC void    eo01_RemoveEnvVarDoNotUseIndepPath() ;

/*!
   @brief          Update LIBPATH environment
   @param          RteError [out] the error return
   @return         false if update failed (RteError is filled), true if successfull

   Used for starting kernel or regcomp in an environment that allows to find all SAPDB specific DLLs
   It updates the sharedc library paths environment

 */

externC tsp00_Bool
sqlUpdateLibPathEnvironment(tsp01_RteError *                RteError);

/*
 * function: sqlPutDbNameToEnv
 * description:
 * Write "SERVERDB=<dbname>" to environment
 * arguments: dbname [in] the database name
 * return value: false if update failed, true if successfull
 */
externC tsp00_Bool
sqlPutDbNameToEnv ( tsp00_DbNamec VAR_ARRAY_REF dbname );

/*
 * function: sqlGetDbNameFromEnv
 * description:
 * Read the database name from the environment
 * return value: a pointer to the database name, NULL if none was found
 */
externC char *
sqlGetDbNameFromEnv ( void );

#endif /*HEO01_H*/
