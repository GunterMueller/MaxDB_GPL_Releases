/*!
  @file           veo01.c
  @author         JoergM
  @special area   general Path / Environment functions
  @brief
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

#include <stdlib.h>
#if !defined(_WIN32)                     /*&if $OSSPEC not in [WIN32]*/
#include <unistd.h>
#include <errno.h>
#include <pwd.h>
#include "gsp09.h"                       /* nocheck but_imkdep */
#include "RunTime/RTE_saveUNIXcalls.h"   /* nocheck but_imkdep */
#endif                                   /*&endif*/
#include "hsp100.h"
#include "heo01.h"                 
#include "heo46.h"
#include "gip00.h"
#include "hsp77.h"
#include "SAPDBCommon/SAPDB_Names.h"
#include "SAPDBCommon/SAPDB_string.h"
#include "RunTime/RTE_IniFileHandling.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#ifdef NO_INLINES
# define _INLINE
#else
# define _INLINE	__inline
#endif

#define DBROOT_ENV_VAR_EO01                     "DBROOT"
#define INSTROOT_ENV_VAR_EO01                   "INSTROOT"
#define PORTABLEINSTALLATIONROOT_ENV_VAR_EO01   "SDB_PORTABLE_ROOT"

#define SUBPATH_INDEP_PGM_EO01    "pgm"
#define SUBPATH_INDEP_LIB_EO01    "lib"
#define SUBPATH_INDEP_BIN_EO01    "bin"
#define SUBPATH_INDEP_ENV_EO01    "env"
#define SUBPATH_INDEP_PROT_EO01   "prot"
#define SUBPATH_INDEP_CONFIG_EO01 "config"
#define SUBPATH_INDEP_WRK_EO01    "wrk"
#define SUBPATH_INDEP_TERMINFO_EO01 "terminfo"
#define SUBPATH_INDEP_MSG_EO01    "msg"

#define DBROOT_BIN_EO01     "bin"
#define DBROOT_ETC_EO01     "etc"
#define DBROOT_ENV_EO01     "env"
#define DBROOT_INCL_EO01    "incl"

#define SAPDB_NO_INDEPPATH_EO01 "SAPDB_NO_INDEPPATH"

/* PTS 1104841 */
/*********************/
#if defined(BIT64) && !defined(OSF1) && !defined(LINUX) && !defined(HP_IA64)
/*********************/

#ifdef WIN32
#define DBROOT_LIB_EO01     "lib\\lib64"
#else
#define DBROOT_LIB_EO01     "lib/lib64"
#endif

#else

#define DBROOT_LIB_EO01     "lib"
/*********************/
#endif  /* BIT64 */
/*********************/

#define DBROOT_MISC_EO01    "misc"
#define DBROOT_PGM_EO01     "pgm"
#define DBROOT_SAP_EO01     "sap"
#define DBROOT_SERVICE_EO01 "serv"
#define DBROOT_WRK_EO01     "wrk"

#define OLD_SERVICE_PARAMFILE_HEADER_EO01 "SVP"
#define OLD_VERSION_TRAILER_LEN_EO01      5

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

#define  GET_INDEP_PATH(_Key, _Path, _MaxPathLen, _RteError) eo01_getglobal( _Key, _Path, _MaxPathLen, _RteError)
/************/
#ifdef _WIN32
/************/
#define  SET_INDEP_PATH(_Key, _Path, _RteError)              os01_SetIndependentPath( _Key, _Path, _RteError)
/************/
#else
/************/
#define  SET_INDEP_PATH(_Key, _Path, _RteError)              eo01_putglobal( _Key, _Path, _RteError )
/************/
#endif
/************/

#if defined(SUN) && defined(BIT64)
#define LDLIBPATH64_SUN "LD_LIBRARY_PATH64"
#endif

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/


enum IndependentPathType
  {
    IndependentPrograms,
    IndependentData
  };

enum DbrootSubPath
  {
     DbrootBin,
     DbrootEtc,
     DbrootEnv,
     DbrootIncl,
     DbrootLib,
     DbrootMisc,
     DbrootPgm,
     DbrootSap,
     DbrootWrk
  } ;

/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL FUNCTIONS (PROTOTYPES)                                             *
 *===========================================================================*/
#if defined(_WIN32)
extern tsp00_Bool os01_GetIndependentPath ( PSZ             pszValueName,
                                            tsp00_Pathc     szPath,
                                            tsp01_RteError  *pRteError );

extern tsp00_Bool os01_SetIndependentPath ( PSZ             pszValueName,
                                            tsp00_Pathc     szPath,
                                            tsp01_RteError  *pRteError );
#else
static tsp00_Bool eo01_putglobal ( const char      *GlobalKey,
                                   char            *GlobalValue,
                                   tsp01_RteError  *RteError );

#endif /* WIN32 */
static tsp00_Bool eo01_getglobal ( const char      *GlobalKey,
                                   char            *GlobalValue,
                                   int              MaxValueLen,
                                   tsp01_RteError  *RteError );

static tsp00_Bool
eo01_GetIndependentSubPath    (  tsp00_Pathc         FullPath,
                                 tsp00_Pathc         SubPath,
                                 enum IndependentPathType PathType,
                                 int                 TerminateWithDelimiter,
                                 tsp01_RteError     *pRteError );

static tsp00_Bool
eo01_GetDbrootEnvVar           ( tsp00_Pathc         Dbroot,
                                 int                 TerminateWithDelimiter,
                                 tsp01_RteError     *pRteError );

static tsp00_Bool
eo01_GetPortableInstallationRootEnvVar (    tsp00_Pathc     PortableInstalaltionRoot,
                                            int             TerminateWithDelimiter,
                                            tsp01_RteError  *pRteError );

static char *eo01_CheckPathEndingForDelimiter ( tsp00_Pathc         Path,
                                                int                 TerminateWithDelimiter ) ;

static tsp00_Bool
eo01_IsIndependentPathAvailable( char           *Key ) ;

static tsp00_Bool
eo01_GetIndependentPath        ( char               *Key,
                                 tsp00_Pathc         Path,
                                 int                 TerminateWithDelimiter,
                                 tsp01_RteError     *pRteError );

static tsp00_Bool
eo01_SetIndependentPath        ( char               *Key,
                                 tsp00_Pathc         Path,
                                 tsp01_RteError     *pRteError ) ;

static tsp00_Bool
eo01_GetDbrootSubPath          ( tsp00_Pathc        Path,
                                 enum DbrootSubPath PathType,
                                 int                TerminateWithDelimiter,
                                 tsp01_RteError     *pRteError ) ;

static int en01_SearchSubPath(const char *GivenPath, const char *SubPath);

static SAPDB_Char *eo01_TrimLDLIBPATH(SAPDB_Char *currentLDLIBPATH);
/*===========================================================================*
 *  GLOBAL FUNCTIONS (CODE)                                                  *
 *===========================================================================*/


/*------------------------------*/

/*########################*/
#ifndef DBROOT_PATHES_ONLY
/*########################*/

/* PTS 1106596 */
tsp00_Bool
sqlIsIndependentDataPathInRegistry ()
{
  tsp01_RteError  RteError ;
  tsp00_Pathc     Path ;
  if(sqlGetPortableInstallationRootPath ( Path, false,&RteError ))
  {
      return true;
  }
  return GET_INDEP_PATH( SAPDB_INDEPDATA_KEY, Path, sizeof(tsp00_Pathc), &RteError );
}

/*------------------------------*/

tsp00_Bool
sqlIsIndependentProgramsPathInRegistry ()
{
  return ( eo01_IsIndependentPathAvailable( SAPDB_INDEPPROGRAMS_KEY ) ) ;
}

/*------------------------------*/

tsp00_Bool
sqlGetIndependentProgramsPath ( tsp00_Pathc     ProgramsPath,
                                int             TerminateWithDelimiter,
                                tsp01_RteError  *pRteError )
{
  return ( eo01_GetIndependentPath ( SAPDB_INDEPPROGRAMS_KEY, ProgramsPath,
                                     TerminateWithDelimiter, pRteError ) ) ;
}

/*------------------------------*/

tsp00_Bool sqlGetIndependentDataPath ( tsp00_Pathc     DataPath,
                                       int             TerminateWithDelimiter,
                                       tsp01_RteError  *pRteError )
{
  boolean Ok = false ;

  Ok = GET_INDEP_PATH( SAPDB_INDEPDATA_KEY, DataPath, sizeof(tsp00_Pathc), pRteError );
  if ( Ok )
    eo01_CheckPathEndingForDelimiter ( DataPath, TerminateWithDelimiter ) ;

  return ( Ok ) ;
}

/*------------------------------*/

tsp00_Bool sqlSetIndependentProgramsPath ( tsp00_Pathc     ProgramsPath,
                                           tsp01_RteError  *pRteError )
{
  return ( eo01_SetIndependentPath ( SAPDB_INDEPPROGRAMS_KEY, ProgramsPath, pRteError ) ) ;
}

/*------------------------------*/

tsp00_Bool sqlSetIndependentDataPath ( tsp00_Pathc     DataPath,
                                       tsp01_RteError  *pRteError )
{
  return ( eo01_SetIndependentPath ( SAPDB_INDEPDATA_KEY, DataPath, pRteError ) ) ;
}

/*------------------------------*/

tsp00_Bool sqlGetIndependentPgmPath ( tsp00_Pathc     PGMPath,
                                      int             TerminateWithDelimiter,
                                      tsp01_RteError  *pRteError )
{
  return ( eo01_GetIndependentSubPath ( PGMPath, SUBPATH_INDEP_PGM_EO01, IndependentPrograms,
                                        TerminateWithDelimiter, pRteError ) ) ;
}

/*------------------------------*/
tsp00_Bool sqlGetIndependentLibPath ( tsp00_Pathc     LIBPath,
                                      int             TerminateWithDelimiter,
                                      tsp01_RteError  *pRteError )
{
  return ( eo01_GetIndependentSubPath ( LIBPath, SUBPATH_INDEP_LIB_EO01, IndependentPrograms,
                                        TerminateWithDelimiter, pRteError ) ) ;
}

/*------------------------------*/

tsp00_Bool sqlGetIndependentBinPath ( tsp00_Pathc     BINPath,
                                      int             TerminateWithDelimiter,
                                      tsp01_RteError  *pRteError )
{
  return ( eo01_GetIndependentSubPath ( BINPath, SUBPATH_INDEP_BIN_EO01, IndependentPrograms,
                                        TerminateWithDelimiter, pRteError ) ) ;
}

/*------------------------------*/

tsp00_Bool sqlGetIndependentEnvPath ( tsp00_Pathc     ENVPath,
                                      int             TerminateWithDelimiter,
                                      tsp01_RteError  *pRteError )
{
  return ( eo01_GetIndependentSubPath ( ENVPath, SUBPATH_INDEP_ENV_EO01, IndependentPrograms,
                                        TerminateWithDelimiter, pRteError ) ) ;
}

/*------------------------------*/

tsp00_Bool sqlGetIndependentTerminfoPath ( tsp00_Pathc     TerminfoPath,
                                           int             TerminateWithDelimiter,
                                           tsp01_RteError  *pRteError )
{
  return ( eo01_GetIndependentSubPath ( TerminfoPath, SUBPATH_INDEP_TERMINFO_EO01, IndependentPrograms,
                                        TerminateWithDelimiter, pRteError ) ) ;
}

/*------------------------------*/

tsp00_Bool sqlGetIndependentMsgPath ( tsp00_Pathc     MSGPath,
                                      int             TerminateWithDelimiter,
                                      tsp01_RteError  *pRteError )
{
  return ( eo01_GetIndependentSubPath ( MSGPath, SUBPATH_INDEP_MSG_EO01, IndependentPrograms,
                                        TerminateWithDelimiter, pRteError ) ) ;
}

/*------------------------------*/
tsp00_Bool sqlGetIndependentProtPath( tsp00_Pathc     PROTPath,
                                      int             TerminateWithDelimiter,
                                      tsp01_RteError  *pRteError )
{
  tsp00_Bool Ok ;

/*!!!!!!!!!!!!!!!!!!!!!!!*/
#ifndef PROT_PATH_ACTIVE
/*!!!!!!!!!!!!!!!!!!!!!!!*/

/* "prot" files for precompiler / are stored in "wrk" */

  Ok = sqlGetIndependentWrkPath ( PROTPath, TerminateWithDelimiter, pRteError ) ;

/*!!!!!!!!!!!!!!!!!!!!!!!*/
#else
/*!!!!!!!!!!!!!!!!!!!!!!!*/

  Ok =  eo01_GetIndependentSubPath ( PROTPath, SUBPATH_INDEP_PROT_EO01, IndependentPrograms,
                                     TerminateWithDelimiter, pRteError )  ;
/*!!!!!!!!!!!!!!!!!!!!!!!*/
#endif
/*!!!!!!!!!!!!!!!!!!!!!!!*/

  return(Ok);
}

/*------------------------------*/
/* PTS 1108067 */
tsp00_Bool sqlGetIndependentServiceProtPath( tsp00_Pathc     PROTPath,
                                             int             TerminateWithDelimiter,
                                             tsp01_RteError  *pRteError )
{
  tsp00_Bool Ok ;
/* "prot" files for xserver / ni are stored in "wrk" (may be "prot" directory later) */

  Ok = sqlGetIndependentProtPath ( PROTPath, TerminateWithDelimiter, pRteError ) ;

/*==========*/
#ifdef WIN32
/*==========*/
  if ( !Ok )
  {
    /*
       If a NT service (serv.exe) is calling this routine und there is neither
       IndependentPath nor DBROOT set, we have to find out the "prot" directory
     */
    if ( GetModuleFileName(NULL, PROTPath, sizeof (tsp00_Pathc) ) )
    {
      char *Hptr = strrchr(PROTPath, '\\' );
      Ok = true ;
      if ( Hptr )
      {
        *Hptr = '\0' ;
        Hptr  = strrchr(PROTPath, '\\' ) ;
        if ( Hptr )
        {
          strcpy(Hptr+1,SUBPATH_INDEP_WRK_EO01 ) ;
          eo01_CheckPathEndingForDelimiter ( PROTPath, TerminateWithDelimiter ) ;
        }
      }
    }
    else
      eo46_set_rte_error (pRteError, GetLastError(), "GetModuleFileName", NULL);
  }
/*==========*/
#endif
/*==========*/

  return(Ok);
}

/*------------------------------*/

char *sqlGetConfigPath ( tsp00_Pathc ConfigPath,
                         int         TerminateWithDelimiter )
{
  tsp01_RteError  RteError ;
  eo46_rte_error_init ( &RteError ) ;

  sqlGetIndependentConfigPath ( ConfigPath, TerminateWithDelimiter, &RteError ) ;

  return(ConfigPath) ;
}


/*------------------------------*/


tsp00_Bool sqlGetIndependentConfigPath ( tsp00_Pathc     ConfigPath,
                                         int             TerminateWithDelimiter,
                                         tsp01_RteError  *pRteError )
{

  tsp00_Bool                Ok ;

  Ok = eo01_GetIndependentSubPath ( ConfigPath, SUBPATH_INDEP_CONFIG_EO01, IndependentData,
                                    TerminateWithDelimiter, pRteError ) ;
  return ( Ok ) ;
}

/*------------------------------*/

char *sqlGetWrkPath ( tsp00_Pathc WrkPath,
                      int         TerminateWithDelimiter )
{

  tsp01_RteError  RteError ;
  eo46_rte_error_init ( &RteError ) ;

  sqlGetIndependentWrkPath ( WrkPath, TerminateWithDelimiter, &RteError ) ;

  return(WrkPath) ;

}

/*------------------------------*/

tsp00_Bool sqlGetIndependentWrkPath ( tsp00_Pathc     WrkPath,
                                      int             TerminateWithDelimiter,
                                      tsp01_RteError  *pRteError )
{
  return ( eo01_GetIndependentSubPath ( WrkPath, SUBPATH_INDEP_WRK_EO01, IndependentData,
                                        TerminateWithDelimiter, pRteError ) ) ;
}
 
/*------------------------------*/

externC tsp00_Bool
sqlUpdateNiLibPathEnvironment(SAPDB_Bool *    pEnvironmentAdjusted,
                              tsp01_RteError *pRteError)
{
/*
 *  It is necessary to add <IndepLibPath> to the LD_LIBRARY_PATH or PATH
 *  to guaranty that the UNIX or Windows respectively SAPNI shared object finds the right SAP objects (e.g. libsapu16.so)
 *  On WINDOWS it is necessary to add <IndepLibPath> the PATH so that the DLL's loaded by SAPNI (e.g. libsapu16v71.dll)
 *  can be found.
 */
    tsp00_Bool  result = false;

#ifdef WIN32
    SAPDB_Char *            oldPath;
    tsp00_Pathc             indepLibPath; 
    SAPDB_UInt4             envBufferLengthNew;
    SAPDB_Char *            pEnvBuffer;
    
    *pEnvironmentAdjusted = false;
    eo46_rte_error_init ( pRteError ) ;
    oldPath = getenv(LDLIBPATH_IP00);
    if (sqlGetIndependentLibPath(indepLibPath, 0, pRteError)) 
    {
        envBufferLengthNew = (SAPDB_UInt4)(strlen (LDLIBPATH_IP00) + 1 + strlen (oldPath) + 1 + strlen (indepLibPath) + 1);
        pEnvBuffer = (SAPDB_Char *)alloca (envBufferLengthNew);
        if (NULL != pEnvBuffer)
        {
            strcpy (pEnvBuffer, LDLIBPATH_IP00);
            strcat (pEnvBuffer, "=");
            strcat (pEnvBuffer, oldPath);
            strcat (pEnvBuffer, ";");
            strcat (pEnvBuffer, indepLibPath);
            if (putenv (pEnvBuffer) < 0)
            {
                eo46_set_rte_error (pRteError, 0, "Failed to putenv ", LDLIBPATH_IP00);
            }
            else
            {
                result = true;
                *pEnvironmentAdjusted = true;
            }
        }
        else
        {
            eo46_set_rte_error (pRteError, 0, "Failed to allocate memory for ", LDLIBPATH_IP00);
        }
    }
#else
    static char *envLDLIB = NULL;
    char *oldenvLDLIB = NULL;
    char *pureLDLIBPATH;
    char *currentLDLIBPATH;
    tsp00_Pathc LibPath;
    int         addLibPath = 0;

    *pEnvironmentAdjusted = false;
    eo46_rte_error_init ( pRteError ) ;
    pureLDLIBPATH = getenv(LDLIBPATH_IP00);

    currentLDLIBPATH = eo01_TrimLDLIBPATH(pureLDLIBPATH);

    if (sqlGetIndependentLibPath (LibPath, 0, pRteError))
    {
        /* If path is found set add...Path to 0 otherwise to 1! This allows to use it for calculation */
        addLibPath = ( en01_SearchSubPath(currentLDLIBPATH, LibPath) ? 0 : 1 );

        if ( addLibPath == 0
          && currentLDLIBPATH == pureLDLIBPATH )
        {
            /* Nothing to do. Environment already corrected */
            result = true;
        }
        else
        {
            oldenvLDLIB = envLDLIB;

            if ( currentLDLIBPATH != NULL )
            {
                /* Lib path is added if not already found */
                /* If either one or two path are added, for each path a colon is needed */
                /* This is handled by 'addLibPath'! */
                envLDLIB = (char *)malloc( strlen(LDLIBPATH_IP00 "=")
                    + addLibPath * ( strlen(LibPath) + strlen(":") )
                    + strlen(currentLDLIBPATH) + 1 );
            }
            else
            {
                /* Lib path is added if not already found */
                envLDLIB = (char *)malloc( strlen(LDLIBPATH_IP00 "=")
                    + strlen(LibPath) + strlen(":")
                    + 1 );
            }

            if ( envLDLIB == NULL )
            {
                eo46_set_rte_error (pRteError, 0, "Failed to allocate memory for ", LDLIBPATH_IP00);
            }
            else
            {
                strcpy(envLDLIB, LDLIBPATH_IP00 "=");

                /* Add LIB path always first */
                if ( addLibPath != 0 )
                {
                    strcat(envLDLIB, LibPath);
                }

                if ( currentLDLIBPATH != NULL ) /* Append former LDLIBPATH */
                {
                    if (addLibPath != 0)
                    {
                        strcat(envLDLIB, ":");
                    }

                    strcat(envLDLIB, currentLDLIBPATH);
                }

                if ( putenv(envLDLIB) < 0 )
                {
                    free(envLDLIB);
                    envLDLIB = oldenvLDLIB;
                    eo46_set_rte_error (pRteError, 0, "Failed to putenv ", LDLIBPATH_IP00);
                }
                else
                {
                    if ( oldenvLDLIB != NULL )
                    {
                        free(oldenvLDLIB); /* prevent some memory leak, accept that last malloc is not freed... */
                    }

                    result = true; /* everything fine */
                    *pEnvironmentAdjusted = true;
                }
            }
        }
    }
    else
    {
        eo46_set_rte_error (pRteError, 0, "Failed to get lib and sap path for", LDLIBPATH_IP00);
    }

#if defined(SUN) && defined(BIT64)

    if ( result )
    {
        char *pureLDLIB64PATH;
        pureLDLIB64PATH = getenv(LDLIBPATH64_SUN);

        /* If LD_LIBRARY_PATH64 is not set, we do not override it */
        if ( 0 != pureLDLIB64PATH )
        {
            static char *envLDLIB64 = NULL;
            char *oldenvLDLIB64 = NULL;
            char *currentLDLIB64PATH = pureLDLIB64PATH;
            tsp00_Pathc LibPath;
            int         addLibPath = 0;
            
            result = false;
            *pEnvironmentAdjusted = false;
            if (sqlGetIndependentLibPath (LibPath, 0, pRteError))
            {
                /* If path is found set add...Path to 0 otherwise to 1! This allows to use it for calculation */
                addLibPath = ( en01_SearchSubPath(currentLDLIB64PATH, LibPath) ? 0 : 1 );

                if ( addLibPath == 0
                  && currentLDLIB64PATH == pureLDLIB64PATH )
                {
                    /* Nothing to do. Environment already corrected */
                    result = true;
                }
                else
                {
                    oldenvLDLIB64 = envLDLIB64;

                    if ( currentLDLIB64PATH != NULL )
                    {
                        /* Lib path is added if not already found */
                        /* If either one or two path are added, for each path a colon is needed */
                        /* This is handled by 'addLibPath'! */
                        envLDLIB64 = (char *)malloc( strlen(LDLIBPATH64_SUN "=")
                            + addLibPath * ( strlen(LibPath) + strlen(":") )
                            + strlen(currentLDLIB64PATH) + 1 );
                    }
                    else
                    {
                        /* Lib path is added if not already found */
                        envLDLIB64 = (char *)malloc( strlen(LDLIBPATH64_SUN "=")
                            + strlen(LibPath) + strlen(":")
                            + 1 );
                    }

                    if ( envLDLIB64 == NULL )
                    {
                        eo46_set_rte_error (pRteError, 0, "Failed to allocate memory for ", LDLIBPATH64_SUN);
                    }
                    else
                    {
                        strcpy(envLDLIB64, LDLIBPATH64_SUN "=");

                        /* Add LIB path always first */
                        if ( addLibPath != 0 )
                        {
                            strcat(envLDLIB64, LibPath);
                        }

                        if ( currentLDLIB64PATH != NULL ) /* Append former LDLIBPATH */
                        {
                            if (addLibPath != 0)
                            {
                                strcat(envLDLIB64, ":");
                            }

                            strcat(envLDLIB64, currentLDLIB64PATH);
                        }

                        if ( putenv(envLDLIB64) < 0 )
                        {
                            free(envLDLIB64);
                            envLDLIB64 = oldenvLDLIB64;
                            eo46_set_rte_error (pRteError, 0, "Failed to putenv ", LDLIBPATH64_SUN);
                        }
                        else
                        {
                            if ( oldenvLDLIB64 != NULL )
                            {
                                free(oldenvLDLIB64); /* prevent some memory leak, accept that last malloc is not freed... */
                            }
                            result = true; /* everything fine */
                            *pEnvironmentAdjusted = true;
                        }
                    }
                }
            }
            else
            {
                eo46_set_rte_error (pRteError, 0, "Failed to get lib and sap path for", LDLIBPATH64_SUN);
            }
        }
    }
#endif
#endif /* else !WIN32 */
    return result;
}

/*###########################*/
#endif /* DBROOT_PATHES_ONLY */
/*###########################*/

/*----------- PTS 1107355 ( used within vos78.cpp ) ----*/

tsp00_Bool sqlGetDbrootPath ( tsp00_Pathc     Path,
                              int             TerminateWithDelimiter,
                              tsp01_RteError  *pRteError )
{
  tsp01_RteError dummyRteError;
  SAPDB_Char *pSpaceForDBRoot = Path;
  tsp00_Pathc     PortablePath;
  tsp00_Pathc     DBRoot;

  if (eo01_GetDbrootEnvVar( DBRoot, TerminateWithDelimiter, pRteError )) {
    strcpy(Path, DBRoot);

    if(sqlGetPortableInstallationRootPath ( PortablePath, TERM_WITH_DELIMITER_EO01,&dummyRteError ))  {
#ifdef _WIN32
      if (strnicmp(DBRoot, PortablePath, strlen(PortablePath)-1) != 0) {
#else
      if (strncmp(DBRoot, PortablePath, strlen(PortablePath)-1) != 0) {
#endif
        strcpy(Path, PortablePath);
        strcat(Path, DBRoot);
      } /* end if */
    } /* end if */
  } else {
      return false;
  } /* end if */

  return true;
} /* end sqlGetDbrootPath */

/*------------------------------*/

tsp00_Bool sqlGetDbrootBinPath ( tsp00_Pathc     BinPath,
                                 int             TerminateWithDelimiter,
                                 tsp01_RteError  *pRteError )
{
  return ( eo01_GetDbrootSubPath ( BinPath, DbrootBin,
                                   TerminateWithDelimiter, pRteError ) ) ;
}

/*------------------------------*/

tsp00_Bool sqlGetDbrootEnvPath ( tsp00_Pathc     EnvPath,
                                 int             TerminateWithDelimiter,
                                 tsp01_RteError  *pRteError )
{
  return ( eo01_GetDbrootSubPath ( EnvPath, DbrootEnv,
                                   TerminateWithDelimiter, pRteError ) ) ;
}

/*------------------------------*/

tsp00_Bool sqlGetDbrootPgmPath ( tsp00_Pathc     PgmPath,
                                 int             TerminateWithDelimiter,
                                 tsp01_RteError  *pRteError )
{
  return ( eo01_GetDbrootSubPath ( PgmPath, DbrootPgm,
                                   TerminateWithDelimiter, pRteError ) ) ;
}

/*------------------------------*/

tsp00_Bool sqlGetDbrootWrkPath ( tsp00_Pathc     WrkPath,
                                 int             TerminateWithDelimiter,
                                 tsp01_RteError  *pRteError )
{
  return ( eo01_GetDbrootSubPath ( WrkPath, DbrootWrk,
                                   TerminateWithDelimiter, pRteError ) ) ;
}

/*------------------------------*/

tsp00_Bool sqlGetDbrootLibPath ( tsp00_Pathc     LibPath,
                                 int             TerminateWithDelimiter,
                                 tsp01_RteError  *pRteError )
{
  return ( eo01_GetDbrootSubPath ( LibPath, DbrootLib,
                                   TerminateWithDelimiter, pRteError ) ) ;
}

/*------------------------------*/
/* PTS 1105164 */
tsp00_Bool sqlGetDbrootSapPath ( tsp00_Pathc     SapPath,
                                 int             TerminateWithDelimiter,
                                 tsp01_RteError  *pRteError )
{
  return ( eo01_GetDbrootSubPath ( SapPath, DbrootSap,
                                   TerminateWithDelimiter, pRteError ) ) ;
}

/*------------------------------*/

tsp00_Bool sqlGetPortableInstallationRootPath ( tsp00_Pathc     Path,
                                                int             TerminateWithDelimiter,
                                                tsp01_RteError  *pRteError )
{
  return ( eo01_GetPortableInstallationRootEnvVar( Path, TerminateWithDelimiter, pRteError ) );
}

/*------------------------------*/

static SAPDB_Char *eo01_TrimLDLIBPATH(SAPDB_Char *currentLDLIBPATH)
{
/* on DEC if a setuid root program is forked, the LD_LIBRARY_PATH is decorated by a prepended
  frame of 0xFF effectiv user id 0xFF effectiv group id 0xFF . That must be removed here... */
#define DEC_SPECIAL_SETUID_INTRODUCER 0xFF

    SAPDB_Byte *p = (SAPDB_Byte *)currentLDLIBPATH;
    if ( p && DEC_SPECIAL_SETUID_INTRODUCER == *p )
    {
         ++p; /* skip first */
         while ( *p && *p != DEC_SPECIAL_SETUID_INTRODUCER )
              ++p; /* skip until second is detected */
         if ( *p == DEC_SPECIAL_SETUID_INTRODUCER )
            ++p; /* skip second */
         while ( *p && *p != DEC_SPECIAL_SETUID_INTRODUCER )
            ++p; /* skip until third is detected */
         if ( *p == DEC_SPECIAL_SETUID_INTRODUCER )
         {
            ++p;
            return (SAPDB_Char *)p;
         }
    }
    else
    {
        return currentLDLIBPATH;
    }
}

/*
 * function: sqlUpdateLibPathEnvironment
 * description:
 * Update environment of starting kernel or xregcomp i.e.. It updates the sharedc library paths
 * arguments: pRteError [out] the error return
 * return value: 0 if update failed (pRteError is filled), 1 if successfull
 */
tsp00_Bool sqlUpdateLibPathEnvironment(tsp01_RteError *pRteError)
{
/*
*  It is necessary to add '$DBROOT/lib' to LD_LIBRARY_PATH
*  Otherwise DCOM runs into severe problems.
*  It is necessary to add '$DBROOT/sap' to LD_LIBRARY_PATH
*  Otherwise DCOM specific dlls are not found
    */
#ifdef WIN32
    eo46_rte_error_init ( pRteError ) ;
    return 1;
#else
    static char *envLDLIB = NULL;
    char *oldenvLDLIB = NULL;
    char *pureLDLIBPATH;
    char *currentLDLIBPATH;
    tsp00_Pathc LibPath;
    tsp00_Pathc SapPath;
    int         addLibPath = 0;
    int         addSapPath = 0;
    tsp00_Bool  result = 0;

    eo46_rte_error_init ( pRteError ) ;

    pureLDLIBPATH = getenv(LDLIBPATH_IP00);

    /* Enforce to leave status as setuid root program! */
    if ( getuid() != 0 && geteuid() == 0 )
    {
        setuid(getuid());
    }

    currentLDLIBPATH = eo01_TrimLDLIBPATH(pureLDLIBPATH);

    if ( sqlGetDbrootLibPath(LibPath, 0, pRteError )
        &&  sqlGetDbrootSapPath(SapPath, 0, pRteError ) )
    {
        /* If path is found set add...Path to 0 otherwise to 1! This allows to use it for calculation */
        addLibPath = ( en01_SearchSubPath(currentLDLIBPATH, LibPath) ? 0 : 1 );
        addSapPath = ( en01_SearchSubPath(currentLDLIBPATH, SapPath) ? 0 : 1 );

        if ( addLibPath == 0
          && addSapPath == 0
          && currentLDLIBPATH == pureLDLIBPATH )
        {
            /* Nothing to do. Environment already corrected */
            result = 1;
        }
        else
        {
            oldenvLDLIB = envLDLIB;

            if ( currentLDLIBPATH != NULL )
            {
                /* Lib or Sap path are added if not already found (addLibSapPath == 0) */
                /* If either one or two path are added, for each path a colon is needed */
                /* This is handled by 'addLibPath' and 'addSapPath' ! */
                envLDLIB = (char *)malloc( strlen(LDLIBPATH_IP00 "=")
                    + addLibPath * ( strlen(LibPath) + strlen(":") )
                    + addSapPath * ( strlen(SapPath) + strlen(":") )
                    + strlen(currentLDLIBPATH) + 1 );
            }
            else
            {
                /* Lib or Sap path are added if not already found (addLibSapPath == 0) */
                /* +1 is needed for zero termination */
                envLDLIB = (char *)malloc( strlen(LDLIBPATH_IP00 "=")
                    + strlen(LibPath) + strlen(":")
                    + strlen(SapPath)
                    + 1 );
            }

            if ( envLDLIB == NULL )
            {
                eo46_set_rte_error (pRteError, 0, "Failed to allocate memory for ", LDLIBPATH_IP00);
            }
            else
            {
                strcpy(envLDLIB, LDLIBPATH_IP00 "=");

                /* Add LIB path always first */
                if ( addLibPath != 0 )
                {
                    strcat(envLDLIB, LibPath);
                    if ( addSapPath != 0 )
                    {
                        /* Add colon to separate SAP path from LIB path */
                        strcat(envLDLIB, ":");
                        strcat(envLDLIB, SapPath);
                    }
                }
                else
                {
                    if ( addSapPath != 0 )
                    {
                        strcat(envLDLIB, SapPath);
                    }
                }

                if ( currentLDLIBPATH != NULL ) /* Append former LDLIBPATH */
                {
                    if ( addLibPath != 0 || addSapPath != 0 )
                    {
                        strcat(envLDLIB, ":");
                    }
                    strcat(envLDLIB, currentLDLIBPATH);
                }

                if ( putenv(envLDLIB) < 0 )
                {
                    free(envLDLIB);
                    envLDLIB = oldenvLDLIB;
                    eo46_set_rte_error (pRteError, 0, "Failed to putenv ", LDLIBPATH_IP00);
                }
                else
                {
                    if ( oldenvLDLIB != NULL )
                    {
                        free(oldenvLDLIB); /* prevent some memory leak, accept that last malloc is not freed... */
                    }
                    result = 1; /* everything fine */
                }
            }
        }
    }
    else
    {
        eo46_set_rte_error (pRteError, 0, "Failed to get lib and sap path for", LDLIBPATH_IP00);
    }

#if defined(SUN) && defined(BIT64)

    if ( result )
    {
        char *pureLDLIB64PATH = getenv(LDLIBPATH64_SUN);

        /* If LD_LIBRARY_PATH64 is not set, we do not override it */
        if ( 0 != pureLDLIB64PATH )
        {
            static char *envLDLIB64 = NULL;
            char *oldenvLDLIB64 = NULL;
            char *currentLDLIB64PATH = pureLDLIB64PATH;
            tsp00_Pathc LibPath;
            tsp00_Pathc SapPath;
            int         addLibPath = 0;
            int         addSapPath = 0;

            result = 0;

            if ( sqlGetDbrootLibPath(LibPath, 0, pRteError )
             &&  sqlGetDbrootSapPath(SapPath, 0, pRteError ) )
            {
                /* If path is found set add...Path to 0 otherwise to 1! This allows to use it for calculation */
                addLibPath = ( en01_SearchSubPath(currentLDLIB64PATH, LibPath) ? 0 : 1 );
                addSapPath = ( en01_SearchSubPath(currentLDLIB64PATH, SapPath) ? 0 : 1 );

                if ( addLibPath == 0
                  && addSapPath == 0
                  && currentLDLIB64PATH == pureLDLIB64PATH )
                {
                    /* Nothing to do. Environment already corrected */
                    result = 1;
                }
                else
                {
                    oldenvLDLIB64 = envLDLIB64;

                    if ( currentLDLIB64PATH != NULL )
                    {
                        /* Lib or Sap path are added if not already found (addLibSapPath == 0) */
                        /* If either one or two path are added, for each path a colon is needed */
                        /* This is handled by 'addLibPath' and 'addSapPath' ! */
                        envLDLIB64 = (char *)malloc( strlen(LDLIBPATH64_SUN "=")
                            + addLibPath * ( strlen(LibPath) + strlen(":") )
                            + addSapPath * ( strlen(SapPath) + strlen(":") )
                            + strlen(currentLDLIB64PATH) + 1 );
                    }
                    else
                    {
                        /* Lib or Sap path are added if not already found (addLibSapPath == 0) */
                        /* +1 is needed for zero termination */
                        envLDLIB64 = (char *)malloc( strlen(LDLIBPATH64_SUN "=")
                            + strlen(LibPath) + strlen(":")
                            + strlen(SapPath)
                            + 1 );
                    }

                    if ( envLDLIB64 == NULL )
                    {
                        eo46_set_rte_error (pRteError, 0, "Failed to allocate memory for ", LDLIBPATH64_SUN);
                    }
                    else
                    {
                        strcpy(envLDLIB64, LDLIBPATH64_SUN "=");

                        /* Add LIB path always first */
                        if ( addLibPath != 0 )
                        {
                            strcat(envLDLIB64, LibPath);
                            if ( addSapPath != 0 )
                            {
                                /* Add colon to separate SAP path from LIB path */
                                strcat(envLDLIB64, ":");
                                strcat(envLDLIB64, SapPath);
                            }
                        }
                        else
                        {
                            if ( addSapPath != 0 )
                            {
                                strcat(envLDLIB64, SapPath);
                            }
                        }

                        if ( currentLDLIB64PATH != NULL ) /* Append former LDLIBPATH */
                        {
                            if ( addLibPath != 0 || addSapPath != 0 )
                            {
                                strcat(envLDLIB64, ":");
                            }
                            strcat(envLDLIB64, currentLDLIB64PATH);
                        }

                        if ( putenv(envLDLIB64) < 0 )
                        {
                            free(envLDLIB64);
                            envLDLIB64 = oldenvLDLIB64;
                            eo46_set_rte_error (pRteError, 0, "Failed to putenv ", LDLIBPATH64_SUN);
                        }
                        else
                        {
                            if ( oldenvLDLIB64 != NULL )
                            {
                                free(oldenvLDLIB64); /* prevent some memory leak, accept that last malloc is not freed... */
                            }
                            result = 1; /* everything fine */
                        }
                    }
                }
            }
            else
            {
                eo46_set_rte_error (pRteError, 0, "Failed to get lib and sap path for", LDLIBPATH64_SUN);
            }

        }
    }
#endif

    return result;
#endif /* else !WIN32 */
}

/*------------------------------*/

externC char *sqlGetDBrootFromExecPath ( tsp00_Pathc ExecPath,
                                         tsp00_Pathc Dbroot,
                                         int         TerminateWithDelimiter )
{
  return sqlGetInstallationPathFromExecPath (ExecPath, Dbroot, TerminateWithDelimiter);
}

/*------------------------------*/

externC char *sqlGetInstallationPathFromExecPath ( tsp00_Pathc ExecPath,
                                                   tsp00_Pathc InstPath,
                                                   int         TerminateWithDelimiter )
{
  char *Ptr = InstPath;

  strcpy ( InstPath, ExecPath ) ;

  if ( (Ptr = (char *) strrchr ( InstPath, PATH_DELIMITER_EO01 )) )
  {
    *Ptr  = '\0' ; /* split Filename from path */
     if ( (Ptr = (char *) strrchr ( InstPath , PATH_DELIMITER_EO01 )) )
       if ( !TerminateWithDelimiter )
         *Ptr  = '\0' ; /* split pgm | bin from path */
       else
         *(Ptr+1) = '\0' ;
  }

  if ( !Ptr ) {
    InstPath[0] = '\0';
  }

  return ( InstPath ) ;
}
/*********************/
#ifndef WIN32
/*********************/
/* PTS 1105226 / 1108841 */
/*-----------------------------------------------------------*/
static int eo01CheckFilenameForRelativePath ( const char  *RelFilename,
                                              char        *RelPath,
                                              char        *AbsPath,
           				    		          int          MaxAbsPathLen )
/*-----------------------------------------------------------*/
{
    int  Found = false ;
    const char *Ptr ;

    AbsPath[0] = '\0' ;

    if ( RelPath && *RelPath != '\0' && *RelPath != PATH_DELIMITER_EO01 )
      strcpy ( AbsPath, RelPath ) ;

    if ( strrchr(RelFilename, PATH_DELIMITER_EO01) ) /* relative path */
    {
      tsp00_Pathc HelpPath ;
      char *helpPtr ;
      /* try to find relative Path RelFilename */
	  strncpy( HelpPath, RelFilename, sizeof(tsp00_Pathc) ) ;
      helpPtr = strrchr(HelpPath, PATH_DELIMITER_EO01) ;
      *helpPtr = '\0' ;
      strcat ( AbsPath, HelpPath ) ;
      Ptr = (helpPtr+1);
    }
    else
      Ptr  = RelFilename;


    if ( AbsPath[0] != '\0' )
    {
      /* try to change to AbsPath */
      chdir ( AbsPath ) ;
      if ( getcwd ( AbsPath, MaxAbsPathLen ) != NULL )
      {
        int AbsPathLen = strlen(AbsPath);
		if ( AbsPathLen + 1 + strlen(Ptr) < MaxAbsPathLen )
		{
          if ( AbsPath [ AbsPathLen -1 ] != PATH_DELIMITER_EO01 )
		  {
            AbsPath [ AbsPathLen  ] = PATH_DELIMITER_EO01 ;
            AbsPath [ AbsPathLen+1] = '\0' ;
		  }
          /* I hope relative Path is AbsPath */
          strcat ( AbsPath, Ptr ) ;
          Found = access(AbsPath, X_OK) == 0 ;
		}
      }
    }
    return (Found) ;
}

#define ENV_PATH_DELIMITER_EO01 ':'
/*-----------------------------------------------------------*/
static char *eo01GetModuleFilename ( const char *Filename,
                                     char    *FullPath,
   				    		         int      MaxPathLen )
/*-----------------------------------------------------------*/
{
  int Found ;

  if ( (Found = *Filename == PATH_DELIMITER_EO01) )       /* absolut path */
  {
    /* absolut Path = Filename */
    strncpy( FullPath, Filename, MaxPathLen );
  }
  else
  {
    Found = eo01CheckFilenameForRelativePath (Filename, NULL, FullPath, MaxPathLen);

    if ( !Found )
    {
      char *Ptr ;
      int  FileNameLen = strlen(Filename) ;
      int  PathNameLen  ;
      char *Path = strdup( getenv("PATH")) ;

      /* checking PATH */
      if ( Path )
      {
        do
        {
          /* 1103637 */
          if ( (Ptr = strchr(Path, ENV_PATH_DELIMITER_EO01)) )
            *Ptr = '\0' ; /* seperate one dir, if Ptr == NULL handle last dir in path */

          PathNameLen = strlen ( Path ) ;
          if ( PathNameLen + FileNameLen < MaxPathLen )
          {
            if ( *Path == PATH_DELIMITER_EO01 ) /* absolut Path */
            {
              strcpy(FullPath, Path);
              if ( Path [ PathNameLen - 1 ] != PATH_DELIMITER_EO01 )      /* 1103637 */
              {
                  FullPath[PathNameLen] = PATH_DELIMITER_EO01;
                  FullPath[PathNameLen+1] = 0;
              }
              strcat(FullPath, Filename);
              /* checking File FullPath */
              Found = access(FullPath, X_OK) == 0 ;
            }
            else
              if ( PathNameLen == 0 )
                Found = eo01CheckFilenameForRelativePath (Filename, ".", FullPath, MaxPathLen ) ;
              else
                Found = eo01CheckFilenameForRelativePath (Filename, Path, FullPath, MaxPathLen ) ;

            if ( Ptr )
              Path = ++Ptr ;
          }
		  else
			  break ; /* not enough space in variable FullPath */
        }
        while ( !Found && Ptr ) ;
      }
    }
  }
  return( Found ? FullPath : NULL) ;
}
/*********************/
#endif
/*********************/

/*------------------------------*/
externC tsp00_Bool sqlGetMyModuleFilename ( const char      *Filename,
                                            char            *FullPath,
				 	              		    int              MaxPathLen,
									        tsp01_RteError  *pRteError )
{
  eo46_rte_error_init ( pRteError ) ;
  *FullPath = '\0' ;

/*********************/
#ifdef WIN32
/*********************/
  if (GetModuleFileName (NULL, FullPath, MaxPathLen) == 0)
    eo46_set_rte_error (pRteError, GetLastError(), "Error during sqlGetMyModuleFilename of ", Filename);
/*********************/
#else
/*********************/
    if ( eo01GetModuleFilename ( Filename, FullPath, MaxPathLen) == NULL )
      eo46_set_rte_error (pRteError, 0, "Error during sqlGetMyModuleFilename of ", Filename);
/*********************/
#endif
/*********************/

  return ( pRteError->RteErrCode == RTE_NO_ERROR_SP01 ) ;
}

/*------------------------------*/

externC tsp00_Bool sqlGetEnvironment  ( char        *EnvName,
                                char        *EnvValue,
                                int          MaxEnvValueLen)
{
  tsp00_Bool  Ok = MaxEnvValueLen > 0 ;

  if ( Ok )
  {
    char       *PtrToEnvValue = getenv(EnvName);

    Ok = PtrToEnvValue != NULL ;

    if ( Ok )
    {
      if ( (int)strlen(PtrToEnvValue) < MaxEnvValueLen)
        strcpy(EnvValue, PtrToEnvValue);
      else
      {
        strncpy(EnvValue, PtrToEnvValue, MaxEnvValueLen-1) ;
        EnvValue[MaxEnvValueLen-1] = '\0' ;
        Ok = false ;
      }
    }
    else
      *EnvValue = '\0' ;
  }
  return (Ok) ;
}

/*------------------------------*/

externC void sqlgetenv (tsp00_C64       envname,
       					tsp00_C64       envvalue,
   		            tsp00_BoolAddr  envfound)
{
  tsp00_C64c        envnameC, envvalueC ;

  eo46PtoC ( envnameC, envname, sizeof (envname) );

  *envfound = sqlGetEnvironment(envnameC, envvalueC, sizeof(tsp00_C64) ) ;

  if ( *envfound )
    eo46CtoP ( envvalue, envvalueC, sizeof (tsp00_C64) );

  return ;
}

/*------------------------------*/

externC void eo01_PutEnvVarDoNotUseIndepPath()
{
  static char EnvBuf[80] ;
  strcpy( EnvBuf, SAPDB_NO_INDEPPATH_EO01) ;
  strcat( EnvBuf, "=yes") ;
  putenv( EnvBuf);
}

/*------------------------------*/

externC void eo01_PutEnvVarDBRoot(tsp00_Pathc   dbroot)
{
  SAPDB_Char EnvBuf[sizeof(tsp00_Pathc) + sizeof (DBROOT_ENV_VAR_EO01) + 1];

  strcpy( EnvBuf, DBROOT_ENV_VAR_EO01) ;
  strcat( EnvBuf, "=");
  strcat( EnvBuf, dbroot);
  putenv( EnvBuf);
}

/*------------------------------*/

externC void eo01_RemoveEnvVarDoNotUseIndepPath()
{
  static char EnvBuf[80] ;
  strcpy( EnvBuf, SAPDB_NO_INDEPPATH_EO01) ;
  strcat( EnvBuf, "=") ;
  putenv( EnvBuf);
}

/*------------------------------*/

externC boolean eo01_IsEnvVarDoNotUseIndepPathSet()
{
  tsp00_Pathc     Path;

  return ( sqlGetEnvironment ( SAPDB_NO_INDEPPATH_EO01, Path, sizeof(tsp00_Pathc) ) &&
	      (*Path == 'y' || *Path == 'Y' ) ) ;
}

/*===========================================================================*
 *  LOCAL FUNCTIONS (CODE)                                                   *
 *===========================================================================*/

static boolean eo01_GetDbrootSubPath ( tsp00_Pathc        Path,
                                       enum DbrootSubPath PathType,
                                       int                TerminateWithDelimiter,
                                       tsp01_RteError     *pRteError )
{
  boolean Ok = false ;
  char   *SubPath ;

  SAPDB_Char *pSpaceForDBRoot = Path;

  Ok = sqlGetDbrootPath( pSpaceForDBRoot, TERM_WITH_DELIMITER_EO01, pRteError ) ;
  if ( Ok )
  {
    switch ( PathType )
    {
      case DbrootBin : SubPath = DBROOT_BIN_EO01 ; break ;
      case DbrootEtc : SubPath = DBROOT_ETC_EO01 ; break ;
      case DbrootEnv : SubPath = DBROOT_ENV_EO01 ; break ;
      case DbrootIncl: SubPath = DBROOT_INCL_EO01; break ;
      case DbrootLib : SubPath = DBROOT_LIB_EO01 ; break ;
      case DbrootMisc: SubPath = DBROOT_MISC_EO01; break ;
      case DbrootPgm : SubPath = DBROOT_PGM_EO01 ; break ;
      case DbrootSap : SubPath = DBROOT_SAP_EO01 ; break ;
      case DbrootWrk : SubPath = DBROOT_WRK_EO01 ; break ;
      default        : eo46_set_rte_error (pRteError, 0, "eo01_GetDbrootSubPath: bad SubPath", NULL);
                       Ok = false                ; break ;
    }
    if ( Ok )
    {
      strcat ( Path, SubPath ) ;
      eo01_CheckPathEndingForDelimiter ( Path, TerminateWithDelimiter ) ;
    }
  }

  return ( Ok ) ;
}

/*------------------------------*/

static tsp00_Bool
eo01_GetDbrootEnvVar ( tsp00_Pathc     Dbroot,
                       int             TerminateWithDelimiter,
                       tsp01_RteError  *pRteError )
{
  tsp00_Bool Ok = sqlGetEnvironment( DBROOT_ENV_VAR_EO01, Dbroot, sizeof(tsp00_Pathc)) ;

  if ( Ok )
    eo01_CheckPathEndingForDelimiter ( Dbroot, TerminateWithDelimiter ) ;
  else
    eo46_set_rte_error (pRteError, 0, "eo01_GetDbrootEnvVar: DBROOT not set", NULL);

  return ( Ok ) ;
}

/*------------------------------*/

static char *eo01_CheckPathEndingForDelimiter ( tsp00_Pathc     Path,
                                                int             TerminateWithDelimiter )
{
  int PathLen = (int)strlen(Path) ;
  if ( PathLen < sizeof(tsp00_Path) )
  {
    if (Path [ PathLen - 1 ] == PATH_DELIMITER_EO01 )
    {
      if ( !TerminateWithDelimiter )
        Path [ PathLen - 1 ] = '\0' ;
    }
    else
    {
      if ( TerminateWithDelimiter )
      {
        Path [ PathLen   ] = PATH_DELIMITER_EO01 ;
        Path [ PathLen+1 ] = '\0' ;
      }
    }
  }
  return ( Path ) ;
}

/*########################*/
#ifndef DBROOT_PATHES_ONLY
/*########################*/

/*------------------------------*/

static tsp00_Bool eo01_SetIndependentPath ( char           *Key,
                                            tsp00_Pathc     Path,
                                            tsp01_RteError  *pRteError )
{
  return SET_INDEP_PATH( Key, Path, pRteError );
}

/*------------------------------*/

static tsp00_Bool
eo01_GetIndependentPath ( char           *Key,
                          tsp00_Pathc     Path,
                          int             TerminateWithDelimiter,
                          tsp01_RteError  *pRteError )
{
  boolean Ok = false ;

  /* in case of development use DBROOT as independend path */

  /* PTS 1111406 */

  if (eo01_IsEnvVarDoNotUseIndepPathSet())
  {
      Ok = eo01_GetDbrootEnvVar( Path, TerminateWithDelimiter, pRteError);
  }
  else
  {
      Ok = GET_INDEP_PATH( Key, Path, sizeof(tsp00_Pathc), pRteError );
  }

#ifdef USE_GLOBAL_PATHES_ONLY
  /* !!!! If there are no IndependentPath available ( during development ) */
  /* !!!! use INSTROOT for this limited time */
  if ( !Ok )
  {
    eo46_rte_error_init ( pRteError ) ;
    if ( !(Ok = sqlGetEnvironment( INSTROOT_ENV_VAR_EO01, Path, sizeof(tsp00_Pathc))) )
      eo46_set_rte_error (pRteError, 0, "eo01_GetIndependentPath: INSTROOT not set", NULL);
  }
#endif

  if ( Ok )
    eo01_CheckPathEndingForDelimiter ( Path, TerminateWithDelimiter ) ;

  return ( Ok ) ;
}

/*------------------------------*/

static tsp00_Bool
eo01_IsIndependentPathAvailable ( char           *Key )
{
  boolean         Ok = false  ;
  tsp01_RteError  RteError ;
  tsp00_Pathc     Path ;

  if ( !eo01_IsEnvVarDoNotUseIndepPathSet() )
  {
    Ok = sqlGetPortableInstallationRootPath ( Path, false,&RteError );
    if ( !Ok )
    {
        Ok = GET_INDEP_PATH( Key, Path, sizeof(tsp00_Pathc), &RteError );
    }
  }

  return ( Ok ) ;
}

/*------------------------------*/

static tsp00_Bool
eo01_GetIndependentSubPath ( tsp00_Pathc     FullPath,
                             tsp00_Pathc     SubPath,
                             enum IndependentPathType PathType,
                             int             TerminateWithDelimiter,
                             tsp01_RteError  *pRteError )
{
  tsp00_Bool Ok ;

  switch ( PathType )
  {
    case IndependentPrograms :
         Ok = sqlGetIndependentProgramsPath ( FullPath, TERM_WITH_DELIMITER_EO01, pRteError ) ;
         break ;
    case IndependentData :
         Ok = sqlGetIndependentDataPath ( FullPath, TERM_WITH_DELIMITER_EO01, pRteError ) ;
         break ;
    default :
         eo46_set_rte_error (pRteError, 0, "eo01_GetIndependentSubPath: bad SubPath", NULL);
         Ok = false ;
         break ;
  }

  if ( Ok )
  {
    strcat ( FullPath, SubPath ) ;
    eo01_CheckPathEndingForDelimiter ( FullPath, TerminateWithDelimiter ) ;
  }
  else
    FullPath[0] = '\0' ;

  return ( Ok ) ;
}

/*------------------------------*/

static tsp00_Bool
eo01_GetIndependentDataSubPath ( tsp00_Pathc     FullPath,
                                 tsp00_Pathc     SubPath,
                                 int             TerminateWithDelimiter,
                                 tsp01_RteError  *pRteError )
{
  tsp00_Bool Ok = sqlGetIndependentDataPath ( FullPath, TERM_WITH_DELIMITER_EO01, pRteError ) ;

  if ( Ok )
  {
    strcat ( FullPath, SubPath ) ;
    eo01_CheckPathEndingForDelimiter ( FullPath, TerminateWithDelimiter ) ;
  }

  return ( Ok ) ;
}
/*###########################*/
#endif /* DBROOT_PATHES_ONLY */
/*###########################*/

/*
 * function: en01_SearchSubPath
 * description:
 * Given path is read from environment. It contains the complete path specification for
 * all shared library paths configurated. Each path element is separated by a colon ':'.
 * arguments: GivenPath [in] the complete path environment string
 *            SubPath [in] the path to search
 * return value: 0 if subpath was not found, 1 if found
 */
static int en01_SearchSubPath(const char *GivenPath, const char *SubPath)
{
  int SearchResult = 0;
  const char *SearchPath = GivenPath;

  if ( SearchPath != NULL )
  {
    /* Loop over each path element */
    while ( *SearchPath != '\0' )
    {
      while ( *SearchPath == ':' ) ++SearchPath;

      if ( !strncmp(SearchPath, SubPath, strlen(SubPath)) )
      {
        SearchResult = 1;
        break;
      }

      while ( *SearchPath != ':'
           && *SearchPath != '\0' ) ++SearchPath;
    }
  }

  return SearchResult;
}

#define NEW_ENV_DBNAME_ID "SERVERDB"

/*
 * function: sqlPutDbNameToEnv
 * description:
 * Write "SERVERDB=<dbname>" to environment
 * arguments: dbname [in] the database name
 * return value: false if update failed, true if successfull
 */
tsp00_Bool sqlPutDbNameToEnv ( tsp00_DbNamec dbname )
{
  static tsp00_Pathc env_dbname ;
  sp77sprintf ( env_dbname, sizeof(tsp00_Pathc), "%s=%s", NEW_ENV_DBNAME_ID , dbname ) ;
  return (putenv ( env_dbname ) == 0) ;
}

/*
 * function: sqlGetDbNameFromEnv
 * description:
 * Read the database name from the environment
 * return value: a pointer to the database name, NULL if none was found
 */
char *sqlGetDbNameFromEnv ( void )
{
  char *dbname ;
  dbname = (char *) getenv ( NEW_ENV_DBNAME_ID );
  return ( dbname ) ;
}

/*------------------------------*/

static tsp00_Bool
eo01_GetPortableInstallationRootEnvVar (    tsp00_Pathc     PortableInstalaltionRoot,
                                            int             TerminateWithDelimiter,
                                            tsp01_RteError  *pRteError )
{
  tsp00_Bool Ok = sqlGetEnvironment( PORTABLEINSTALLATIONROOT_ENV_VAR_EO01, PortableInstalaltionRoot, sizeof(tsp00_Pathc)) ;

  if ( Ok )
    eo01_CheckPathEndingForDelimiter ( PortableInstalaltionRoot, TerminateWithDelimiter ) ;
  else
    eo46_set_rte_error (pRteError, 0, "eo01_GetPortableInstallationRootEnvVar: environment variable not set", NULL);

  return ( Ok ) ;
}

/*########################*/
#ifndef DBROOT_PATHES_ONLY
/*########################*/
#ifndef WIN32
/**\
--------------------------------------- MF__ MOD__ (Installation Registry)
\**/
    #undef MF__
    #define MF__ MOD__"eo01_putglobal"
static tsp00_Bool eo01_putglobal ( const char      *GlobalKey,
                            char            *GlobalValue,
                            tsp01_RteError  *RteError )
{
    tsp00_Bool    ok;
    unsigned char rc;
    tsp00_ErrTextc errtext;

    int  umask_old ;
    umask_old = umask(022); /* if the global registration file is created, the write access is limited to owner */
    if ( !RTE_PutInstallationConfigString (GlobalKey, GlobalValue, errtext, &rc))
    {
        ok = false ;
        eo46_set_rte_error (RteError, 0, errtext, GlobalKey);
	}
    else
    {
        ok = true ;
    }
    umask(umask_old);
    return(ok);
}

/*----------------------------------------*/
#endif

    #undef MF__
    #define MF__ MOD__"eo01_getglobal"
static tsp00_Bool eo01_getglobal ( const char      *GlobalKey,
                                   char            *GlobalValue,
                                   int              MaxValueLen,
                                   tsp01_RteError  *RteError )
{
    tsp00_Bool    ok;
    unsigned char rc;
    tsp00_ErrTextc errtext;

#ifdef _WIN32
    tsp00_Pathc     szPath ;
    tsp01_RteError  dummyRteError;
    ok = sqlGetPortableInstallationRootPath ( szPath, TERM_WITH_DELIMITER_EO01,&dummyRteError );
    if ( !ok )
    {
        return os01_GetIndependentPath( (PSZ)GlobalKey ,GlobalValue, RteError);
    }
    SAPDB_strncpy_zero_terminated( GlobalValue,szPath,MaxValueLen);
    MaxValueLen -= strlen( GlobalValue );
    GlobalValue += strlen( GlobalValue );
#endif
    if (!RTE_GetInstallationConfigString (GlobalKey, GlobalValue, MaxValueLen,  errtext, &rc))
    {
        ok = false ;
        eo46_set_rte_error (RteError, 0, errtext, GlobalKey);
	}
    else
    {
        ok = true ;
    }
    return(ok);
}
/*########################*/
#endif /* DBROOT_PATHES_ONLY */
/*########################*/

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
