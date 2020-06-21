/*!
  @file           RTE_IniFileHandling.c
  @author         JoergM
  @brief          Registry for UNIX: Handling of '.ini' files on UNIX and registry on Windows
  @see            example.html ...


    Each .ini File consists of sections, which are identified by a section header.
    The section header consists of an name framed by brackets.
    Each entry in the section consists of a key an value separated by an equal sign.
    The section is seperated from the next section by an empty line.
    Example
    [Database]
    SUT72=/home/remuser/releases/V72/usr
    MUT7264=/home/remuser/releases/V7264/usr

    first created 1998-06-23 12:30

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


\endif
---------------------------------------------------------------------*/
#include "RunTime/RTE_IniFileHandling.h"
#include "SAPDBCommon/SAPDB_Names.h"
#include "SAPDBCommon/SAPDB_string.h"
#include "SAPDBCommon/SAPDB_sprintf.h"
#include "geo007_2.h"
#include "heo46.h"
#include "heo01.h"

#include <malloc.h>
/*
 * first check if any UNIX platform is defined
 */

#define SAPDB_UNLOCKED_ACCESS false
#define SAPDB_LOCKED_ACCESS   true

#if !(defined SUN || defined SOLARIS || defined AIX || defined OSF1 || defined NMP || defined HPUX || defined(LINUX))
#include <windows.h>
#include <shlobj.h>


#include "gos00.h"      /* nocheck */

/*! @brief get windows confile string */
static SAPDB_Int4 RTE_GetWindowsConfigString (
                        SAPDB_Char const *      accountName,
                        SAPDB_Bool              isUserSpecific,
                        SAPDB_Bool              wantDefaultString,
                        SAPDB_Char const *      szSection,
                        SAPDB_Char const *      szEntry, 
                        SAPDB_Char *            szString,
                        SAPDB_Int4 const        MaxStringLen,
                        tsp00_ErrTextc          ErrText,
                        RTE_IniFileResult *     pOk );
/*! @brief put windows confile string */
static SAPDB_Int4 RTE_PutWindowsConfigString (
                        SAPDB_Char const *    accountName,
                        SAPDB_Bool            isUserSpecific,
                        SAPDB_Bool            wantDefaultString,
                        const SAPDB_Char *    szSection,
                        const SAPDB_Char *    szEntry, 
                        const SAPDB_Char *    szString,
                        tsp00_ErrTextc        ErrText,
                        RTE_IniFileResult * pOk );
/*! @brief remove windows confile string */
static SAPDB_Int4 RTE_RemoveWindowsConfigString (
                        SAPDB_Char const *    accountName,
                        SAPDB_Bool            isUserSpecific,
                        SAPDB_Bool            wantDefaultString,
                        const SAPDB_Char *    szSection,
                        const SAPDB_Char *    szEntry, 
                        tsp00_ErrTextc        ErrText,
                        RTE_IniFileResult * pOk );
/*! @brief open windows confile string enumeration */
static RTE_RegistryHandle RTE_OpenWindowsConfigEnum (SAPDB_Char const * accountName,
                                                     SAPDB_Bool         isUserSpecific,
                                                     RTE_RegistryHandle hEnum,
                                                     SAPDB_Char const * szSection,
                                                     tsp00_ErrTextc     ErrText,
                                                     RTE_IniFileResult * pOk );
/*! @brief close windows confile string enumeration */
static SAPDB_Int4 RTE_CloseWindowsConfigEnum ( RTE_RegistryHandle hEnum,
                                               tsp00_ErrTextc     ErrText,
                                               RTE_IniFileResult * pOk );
/*! @brief next in windows confile string enumeration */
static SAPDB_Int4 RTE_NextWindowsConfigEnum( RTE_RegistryHandle hEnum,
                                             SAPDB_Char *szEntry,
                                             const SAPDB_Int4 MaxEntryLen,
                                             SAPDB_Char *szString,
                                             const SAPDB_Int4 MaxStringLen,
                                             tsp00_ErrTextc   ErrText,
                                             RTE_IniFileResult * pOk );

#define STRCASECMP stricmp
#include <io.h>

#else
#include <sys/utsname.h>
#if defined(SUN) || defined(SOLARIS)
#include <signal.h> /* for kill */
#define _POSIX_PTHREAD_SEMANTICS
#endif /* SUN || SOLARIS */
#include <pwd.h>
#include <grp.h>
#include <unistd.h>

/* for void *alloca(size_t) */
#ifdef AIX

# include       <malloc.h>

#else

# include       <alloca.h>

#endif

#define STRCASECMP strcasecmp
#define SLEEP(s) sleep ( s )

#endif /*--------- end of UNIX specifc part --------------*/

#ifdef SDB_MULTI_THREAD
#include "heo07.h" /* nocheck &variant -singlethread */
#endif /* SDB_MULTI_THREAD */ 
#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <ctype.h>
#include "RunTime/RTE_UNIXAuthenticate.h"
/*! found in Env{"ODBCINI"} oder in home directory */
#define UNIX_USER_ODBC_INI_FILE "/.odbc.ini"
/*! found in absolute path */
#define UNIX_GLOBAL_ODBC_INI_FILE "/etc/odbc.ini"
#define SDB_LOCK_FILE_SUFFIX "_lock"

/*
 *  declarations for config file processing
 */
#define LINE_SIZE_MAX 800

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*
 * This must be implemented as a Macro, since it uses alloca...
 */
#define READLINE_EN09(_Fd, _Buffer, _OldSize, _Size, _MoreData, _Result) \
  _OldSize = 0; \
  for ( _MoreData = true; _MoreData; ) \
  { \
    _Result = ReadLine ( _Fd, _Buffer+_OldSize, (_Size-_OldSize), &_MoreData ); \
    if ( _Result != 1 ) \
    { \
      break; \
    } \
\
    if ( _MoreData ) \
    { \
      SAPDB_Char *SecondBuffer; \
      _OldSize = _Size-1; \
      _Size += LINE_SIZE_MAX; \
\
      SecondBuffer = (SAPDB_Char *)alloca(_Size); \
      strcpy( SecondBuffer, _Buffer); \
      _Buffer = SecondBuffer; \
    } \
  }

/*
 * This must be implemented as a Macro, since it uses alloca...
 */
#define COPYLINE_EN09(_ppNextEntry, _Buffer, _OldSize, _Size, _MoreData, _Result) \
  _OldSize = 0; \
  for ( _MoreData = true; _MoreData; ) \
  { \
    _Result = CopyLine ( _ppNextEntry, _Buffer+_OldSize, (_Size-_OldSize), &_MoreData ); \
    if ( _Result != 1 ) \
    { \
      break; \
    } \
\
    if ( _MoreData ) \
    { \
      SAPDB_Char *SecondBuffer; \
      _OldSize = _Size-1; \
      _Size += LINE_SIZE_MAX; \
\
      SecondBuffer = (SAPDB_Char *)alloca(_Size); \
      strcpy( SecondBuffer, _Buffer); \
      _Buffer = SecondBuffer; \
    } \
  }

/*===========================================================================*
 *  LOCAL FUNCTIONS (PROTOTYPES)                                             *
 *===========================================================================*/

typedef struct _LockFile {
    SAPDB_Bool m_IsTemp;
    const SAPDB_Char *m_Path;
    SAPDB_Int4 Fd;
    SAPDB_Bool m_IsOpen;
    SAPDB_Bool m_IsLocked;
    SAPDB_Bool m_ReadOnly;
    RTE_LockFileContent m_LockEntry;
    SAPDB_Char const *m_LastError;
} RTE_LockFile;

static SAPDB_Char *CopySection(SAPDB_Int4 Fd);
static SAPDB_Int4 CopyLine (
                     SAPDB_Char **ppNextEntry,
                     SAPDB_Char * pBuffer,
                     SAPDB_Int4    BufSize,
                     SAPDB_Bool * pMoreData );
static SAPDB_Int4 ReadLine (SAPDB_Int4,
                            SAPDB_Char *,
                            SAPDB_Int4 /* PTS 1104679 */,
                            SAPDB_Bool * );
static SAPDB_Int4 FindSection (SAPDB_Int4,
                               const SAPDB_Char *);
static SAPDB_Int4 WriteSection (SAPDB_Int4,
                                const SAPDB_Char*,
                                SAPDB_Int4);
static SAPDB_Int4 WriteEntry (SAPDB_Int4,
                              const SAPDB_Char *,
                              const SAPDB_Char *);
static SAPDB_Int4 WriteLine (SAPDB_Int4,
                             const SAPDB_Char*);
static SAPDB_Int4 UpdateConfigString (SAPDB_Bool        wantLock,
                                      const SAPDB_Char *szPath, 
                                      const SAPDB_Char *szSection, 
                                      const SAPDB_Char *szEntry,
                                      const SAPDB_Char *szString,
                                      const SAPDB_Bool DeleteFlag,
                                      tsp00_ErrTextc   ErrText,
                                      RTE_IniFileResult *pOk);
static SAPDB_Int4 UpdateInstallationConfigString (const SAPDB_Char *szEntry,
                                                  const SAPDB_Char *szString,
                                                  const SAPDB_Bool DeleteFlag,
                                                  tsp00_ErrTextc   ErrText,
                                                  RTE_IniFileResult *pOk);
static SAPDB_Int4 GetConfigString ( SAPDB_Bool            wantLock,
                                    RTE_LockFile         *    pRegistryFile,
                                    const SAPDB_Char *    szPath,
                                    const SAPDB_Char *    szSection,
                                    const SAPDB_Char *    szEntry, 
                                    SAPDB_Char       *    szString,
                                    const SAPDB_Int4    MaxStringLen,
                                    tsp00_ErrTextc      ErrText,
                                    RTE_IniFileResult * pOk);
static SAPDB_Int4 RTE_NextUNIXConfigEnum( RTE_RegistryHandle hEnum,
                                          SAPDB_Char *szEntry,
                                          const SAPDB_Int4 MaxEntryLen,
                                          SAPDB_Char *szString,
                                          const SAPDB_Int4 MaxStringLen,
                                          tsp00_ErrTextc   ErrText,
                                          RTE_IniFileResult * pOk );
static SAPDB_Bool FoundMatchingEntry( SAPDB_Char const *buffer
                                    , SAPDB_Char const *szEntry );
static void RegistryFile_Init(RTE_LockFile *this,
                              SAPDB_Bool IsTemp,
                              SAPDB_Bool readOnly);
static SAPDB_Bool RegistryFile_Open(RTE_LockFile *this,
                                    const SAPDB_Char *Path);
static SAPDB_Bool RegistryFile_Create(RTE_LockFile *this,
                                      const SAPDB_Char *Path);
static SAPDB_Bool RegistryFile_Lock(RTE_LockFile *this);
static SAPDB_Bool RegistryFile_Unlock(RTE_LockFile *this);
static SAPDB_Bool RegistryFile_Reset(RTE_LockFile *this);
static void RegistryFile_Close(RTE_LockFile *this);
static RTE_RegistryHandle OpenConfigFileForEnumeration (RTE_RegistryHandle hEnum,
                                                        const SAPDB_Char *szPath,
                                                        SAPDB_Int4         readOnly,
                                                        const SAPDB_Char *szSection,
                                                        tsp00_ErrTextc    ErrText, 
                                                        RTE_IniFileResult *pOk);
static SAPDB_Char const *GetLastSystemErrorAsString();
static SAPDB_Bool ValidateConfigPath( SAPDB_Char const * szConfigPath,
                                     tsp00_ErrTextc      ErrText,
                                     RTE_IniFileResult *pOk);
static SAPDB_Bool myGetEnv  ( SAPDB_Char        *EnvName,
                              SAPDB_Char        *EnvValue,
                              SAPDB_Int4         MaxEnvValueLen);
static int my_save_chmod(SAPDB_Char const *path, SAPDB_Int4 newMode);
static SAPDB_Int4 RTE_RemoveUNIXConfigString (  SAPDB_Bool wantLock,
                                                const SAPDB_Char *szFile,
                                                const SAPDB_Char *szSection,
                                                const SAPDB_Char *szEntry, 
                                                tsp00_ErrTextc    ErrText,
                                                RTE_IniFileResult *pOk);

static SAPDB_Bool RTE_GetUserSpecificPath (SAPDB_Char const * const        homeSubdirectory,
                                           SAPDB_Bool                      roaming,
                                           RTE_Path                        ConfigPath,
                                           SAPDB_Bool                      TerminateWithDelimiter,
                                           tsp00_ErrTextc                  ErrText );

static RTE_LockFileContent RegistryFile_Nobody = { 0 };

static SAPDB_UInt RegistryFile_VerbosityLevel = SAPDB_VERBOSITY_LEVEL_SILENT; /*  + SAPDB_VERBOSITY_FLAG_ACTIVITY; */

/*---------------------------------*/

static SAPDB_Bool RegistryFile_TestVerbosityFlag(SAPDB_UInt flags)
{
    return ( (RegistryFile_VerbosityLevel&flags) == flags );
}

/*---------------------------------*/

static SAPDB_Bool RegistryFile_TestVerbosityLevel(SAPDB_UInt minLevel)
{
    return ( (RegistryFile_VerbosityLevel&SAPDB_VERBOSITY_LEVEL_MASK) >= minLevel );
}

/*---------------------------------*/

SAPDB_UInt RegistryFile_SetVerbosity(SAPDB_UInt level)
{
    SAPDB_UInt oldLevel = RegistryFile_VerbosityLevel;
    RegistryFile_VerbosityLevel = level;
    return oldLevel;
}

/*---------------------------------*/

SAPDB_UInt RegistryFile_GetVerbosity()
{
    return RegistryFile_VerbosityLevel;
}


static SAPDB_Bool TrimDelimiter(RTE_Path path,
                                SAPDB_Int4 forceDelimiter);

/*---------------------------------------------------------------------------*/

/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/

/*===========================================================================*
 *  GLOBAL FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

/*
  Function:     RTE_GetGlobalConfigString ()
  description:  Read Config Entry
  
    Gives back a string from ini-file 'szFile' from the position descripted by
    'szSection' and 'szEntry'. It fails if either the file, the section or the
    entry does not exist.
    
  arguments:
    szFile [in] Name of registry file
    szSection [in] Given section to read from
    szEntry [in] Given key of 'key=value' entry
    szString [out] Place to store the value of 'key=value' entry
    MaxStringLen [in] Maximum value buffer size
    ErrText [out] Place for descriptive error message
    Ok [out] Error return value

  return value: none, but err is updated to
    <UL>
        <LI>0: Write failed ( inspect errno )
        <LI>1: Write ok
    </UL>
 */

SAPDB_Int4 RTE_GetGlobalConfigString (
                        const SAPDB_Char *    szFile,
                        const SAPDB_Char *    szSection,
                        const SAPDB_Char *    szEntry, 
                        SAPDB_Char       *    szString,
                        const SAPDB_Int4       MaxStringLen,
                        tsp00_ErrTextc   ErrText,
                        RTE_IniFileResult * pOk )
{
#if defined(WIN32)
/*  
    if(!getenv("SDB_U3_ROOT"))  // useless??
    {
        return RTE_GetWindowsConfigString(NULL,
                                        false, 
                                        (SAPDB_Bool)(strcmp(szSection, SAPDB_RUNTIME_SECTION) == 0),
                                        szSection,
                                        szEntry,
                                        szString,
                                        MaxStringLen,
                                        ErrText,
                                        pOk );
    }
*/
#endif
{    SAPDB_Int4 result;
    RTE_LockFile RegistryFile;
    SAPDB_Int4 wantLock;
    SAPDB_Char *szPath;

    if ( !szFile || !szSection )
    {
        *pOk = SAPDB_INIFILE_RESULT_ERR_PARAM;
        strcpy ( (SAPDB_Char *)ErrText, "NULL pointer for file or section passed" );
        return 0;
    }

    wantLock = ( (0 == strcmp(szFile, SAPDB_INSTALLATIONS_INI_FILE))
              || (0 == strcmp(szFile, SAPDB_RUNTIMES_INI_FILE)) ? SAPDB_UNLOCKED_ACCESS : SAPDB_LOCKED_ACCESS );

    RegistryFile_Init(&RegistryFile, false, true);

    if ( *szFile == '/' )
    {
#ifndef _WIN32
        if ( !strncmp(szFile, SAPDB_OLD_GLOBAL_CONFIG_DIRECTORY "/", strlen(SAPDB_OLD_GLOBAL_CONFIG_DIRECTORY "/")) 
          ||  (getenv(SAPDB_OWN_GLOBAL_CONFIG_FILE) != 0 && !strcmp(szFile, getenv(SAPDB_OWN_GLOBAL_CONFIG_FILE)))
          || !strcmp(szFile, SAPDB_GLOBAL_CONFIG_FILE) )
        {
            szPath = (SAPDB_Char *)alloca(strlen(szFile)+1);
            strcpy (szPath, szFile);
        }
        else
#endif
        {
            *pOk = SAPDB_INIFILE_RESULT_ERR_PARAM;
            strcpy ( (SAPDB_Char *)ErrText, "Only relativ pathes allowed" );
            return 0;
        }
    }
    else
    {
        if ( !strcmp(SAPDB_ODBC_INI_FILE, szFile) )
        {
            szPath = (SAPDB_Char *)alloca(strlen(UNIX_GLOBAL_ODBC_INI_FILE)+1);
            strcpy(szPath, UNIX_GLOBAL_ODBC_INI_FILE);
        }
        else
        {
            RTE_Path indepConfigPath;
            if ( !RTE_GetCommonConfigPath(indepConfigPath, true, ErrText) )
            {
                *pOk = SAPDB_INIFILE_RESULT_ERR_PARAM;
                return 0;
            }
            szPath = (SAPDB_Char *)alloca(strlen(indepConfigPath)+1+strlen(szFile)+1);
            strcpy(szPath, indepConfigPath); strcat(szPath, szFile);
        }
    }

    result = GetConfigString ( wantLock,
                               &RegistryFile,
                               szPath,
                               szSection,
                               szEntry, 
                               szString,
                               MaxStringLen,
                               ErrText,
                               pOk);
    RegistryFile_Close(&RegistryFile);

    return result == 0 ? 0 : (SAPDB_Int4)strlen (szString);
}
}

/*
  Function:     RTE_GetInstallationConfigString ()
  description:  Read Config Entry from globals section
  
    Gives back a string from "Globals" section in ini-file '/etc/opt/sapdb' from the position
    descripted by 'szEntry'. It fails if either the file, the section or the entry does not exist.
    
  arguments:
    szEntry [in] Given key of 'key=value' entry
    szString [out] Place to store the value of 'key=value' entry
    MaxStringLen [in] Maximum value buffer size
    ErrText [out] Place for descriptive error message
    Ok [out] Error return value

  return value: none, but err is updated to
    <UL>
        <LI>0: Write failed ( inspect errno )
        <LI>1: Write ok
    </UL>
 */
SAPDB_Int4 RTE_GetInstallationConfigString (
                        const SAPDB_Char *    szEntry, 
                        SAPDB_Char       *    szString,
                        const SAPDB_Int4       MaxStringLen,
                        tsp00_ErrTextc   ErrText,
                        RTE_IniFileResult * pOk )
{
    RTE_LockFile RegistryFile;
    SAPDB_Int4 result;
    RTE_Path pathToOwnGlobalConfigFile;

#if defined(WIN32)
    tsp00_Pathc u3Root;
    tsp01_RteError  rteError; 
    if(!sqlGetPortableInstallationRootPath( u3Root,
                                        TERM_WITHOUT_DELIMITER_EO01,
                                        &rteError ))
    {
        return RTE_GetWindowsConfigString(NULL,
                                        false,
                                        false,
                                        SAPDB_GLOBALS_SECTION,
                                        szEntry,
                                        szString,
                                        MaxStringLen,
                                        ErrText,
                                        pOk );
    }
#endif
    if ( !myGetEnv(SAPDB_OWN_GLOBAL_CONFIG_FILE, pathToOwnGlobalConfigFile, sizeof(RTE_Path) ) )
    {
#ifdef _WIN32
        SAPDB_sprintf(&pathToOwnGlobalConfigFile[0], sizeof(RTE_Path),"%s%s",u3Root,SAPDB_GLOBAL_CONFIG_FILE);
#else
        SAPDB_strncpy_zero_terminated( &pathToOwnGlobalConfigFile[0], SAPDB_GLOBAL_CONFIG_FILE, sizeof(RTE_Path));
#endif
    }

    RegistryFile_Init(&RegistryFile, false, true);

    result = GetConfigString( SAPDB_UNLOCKED_ACCESS,
                              &RegistryFile,
                              &pathToOwnGlobalConfigFile[0],
                              SAPDB_GLOBALS_SECTION,
                              szEntry,
                              szString,
                              MaxStringLen,
                              ErrText,
                              pOk );
    RegistryFile_Close(&RegistryFile);

    return result == 0 ? 0 : (SAPDB_Int4)strlen (szString);
}

/*!
   @brief          Read User Specific Config Entry

   Gives back a string from ini-file 'szFile' from the position descripted by
   'szSection' and 'szEntry'. The ini-file is searched in '$HOME/.sapdb' first.
   If no entry is found there, it DOES NOT try again in '<indepData>/.sapdb'!
   It fails if either no file (config_error_open_chn09),
   the section and entry (SAPDB_INIFILE_RESULT_NO_ENTRY)
   does not exist. If it is not accessible another error code is returned. This allows
   to program a fallback lookup in other global files if wanted.
 */
SAPDB_Int4 RTE_GetUserConfigString (
    SAPDB_Char const * accountName,
    const SAPDB_Char * szFile,
    const SAPDB_Char * szSection,
    const SAPDB_Char * szEntry,
    SAPDB_Char       * szString,
    const SAPDB_Int4   MaxStringLen,
    tsp00_ErrTextc ErrText,
    RTE_IniFileResult       * pOk)
{
#if defined(WIN32)
    return RTE_GetWindowsConfigString (accountName,
                                           true,
                                           (SAPDB_Bool) (strcmp(szSection, SAPDB_RUNTIME_SECTION) == 0),
                                           szSection,
                                           szEntry,
                                           szString,
                                           MaxStringLen,
                                           ErrText,
                                           pOk);
#else
    SAPDB_Int4 result;
    RTE_LockFile RegistryFile;
    SAPDB_Char *szPath;
    RTE_Path configPath;

    if ( !szFile || !szSection )
    {
        *pOk = SAPDB_INIFILE_RESULT_ERR_PARAM;
        strcpy ( (SAPDB_Char *)ErrText, "NULL pointer for file or section passed" );
        return 0;
    }

    if ( *szFile == '/' )
    {
        *pOk = SAPDB_INIFILE_RESULT_ERR_PARAM;
        strcpy ( (SAPDB_Char *)ErrText, "Only relativ pathes allowed" );
        return 0;
    }

    if ( !strcmp(SAPDB_ODBC_INI_FILE, szFile) )
    {
        if ( myGetEnv("ODBCINI", configPath, sizeof(RTE_Path) ) )
        {
            szPath = (SAPDB_Char *)alloca(strlen(configPath)+strlen(UNIX_USER_ODBC_INI_FILE)+1);
            strcpy(szPath, configPath); strcat(szPath, UNIX_USER_ODBC_INI_FILE);
        }
        else
        {
            SAPDB_UInt4 neededSize = 0;
            SAPDB_Char  tmpBuffer[2];
            if ( !RTE_GetHomeDirectoryFromPasswdById( (SAPDB_Int4)geteuid ()
                                                    , tmpBuffer
                                                    , (SAPDB_UInt4)2
                                                    , &neededSize ) && 0 == neededSize )
            {
                strcpy((SAPDB_Char *)ErrText, "Found no home directory entry");
                *pOk = SAPDB_INIFILE_RESULT_ERR_PARAM;
                return 0;
            }
            szPath = (SAPDB_Char *)alloca(neededSize+strlen(UNIX_USER_ODBC_INI_FILE)+1);
            if ( !RTE_GetHomeDirectoryFromPasswdById( (SAPDB_Int4)geteuid ()
                                                    , szPath
                                                    , neededSize
                                                    , &neededSize ) )
            {
                strcpy((SAPDB_Char *)ErrText, "Found no home directory entry");
                *pOk = SAPDB_INIFILE_RESULT_ERR_PARAM;
                return 0;
            }
            strcat(szPath, UNIX_USER_ODBC_INI_FILE);
        }
    }
    else 
    {
        if ( !RTE_GetUserSpecificConfigPath(configPath, true, ErrText) )
        {
            *pOk = SAPDB_INIFILE_RESULT_ERR_OPEN;
            return 0;
        }

        szPath = (SAPDB_Char *)alloca(strlen(configPath)+strlen(szFile)+1);
        strcpy(szPath, configPath); strcat(szPath, szFile);
    }

    RegistryFile_Init(&RegistryFile, false, true);

    result = GetConfigString ( SAPDB_LOCKED_ACCESS,
                               &RegistryFile,
                               szPath,
                               szSection,
                               szEntry, 
                               szString, 
                               MaxStringLen,
                               ErrText,
                               pOk);

    RegistryFile_Close(&RegistryFile);

    return result;
#endif
}

/*
  Function:     RTE_GetConfigString ()
  description:  Read Config Entry
  
    Gives back a string from ini-file 'szFile' from the position descripted by
    'szSection' and 'szEntry'. It fails if either the file, the section or the
    entry does not exist. This routine first tries to read the 'User' config string
    and if it fails fallbacks using the global config string file.
    
  arguments:
    szFile [in] Name of registry file
    szSection [in] Given section to read from
    szEntry [in] Given key of 'key=value' entry
    szString [out] Place to store the value of 'key=value' entry
    MaxStringLen [in] Maximum value buffer size
    ErrText [out] Place for descriptive error message
    Ok [out] Error return value

  return value: none, but err is updated to
    <UL>
        <LI>0: Write failed ( inspect errno )
        <LI>1: Write ok
    </UL>
 */
SAPDB_Int4 RTE_GetConfigString (
                        const SAPDB_Char *    szFile,
                        const SAPDB_Char *    szSection,
                        const SAPDB_Char *    szEntry, 
                        SAPDB_Char       *    szString,
                        const SAPDB_Int4       MaxStringLen,
                        tsp00_ErrTextc   ErrText,
                        RTE_IniFileResult * pOk )
{
    SAPDB_Int4 result;
    /* Skip INSTALLATIONS and DATABASES search in user specific directory */
    if ( strcmp(szFile, SAPDB_INSTALLATIONS_INI_FILE)
      && strcmp(szFile, SAPDB_DATABASES_INI_FILE) )
    {
        result = RTE_GetUserConfigString( NULL,
                                          szFile,
                                          szSection,
                                          szEntry,
                                          szString,
                                          MaxStringLen,
                                          ErrText,
                                          pOk);
        if ( SAPDB_INIFILE_RESULT_ERR_OPEN != *pOk
          && SAPDB_INIFILE_RESULT_NO_ENTRY != *pOk )
        {
            return result;
        }
    }

    result = RTE_GetGlobalConfigString( szFile,
                                        szSection,
                                        szEntry,
                                        szString,
                                        MaxStringLen,
                                        ErrText,
                                        pOk);
    if ( SAPDB_INIFILE_RESULT_ERR_OPEN != *pOk
      && SAPDB_INIFILE_RESULT_NO_ENTRY != *pOk )
    {
        return result;
    }

#if defined(WIN32)
    return result;
#else
    {
        char *oldPath;
        if ( !strcmp(szFile, SAPDB_RUNTIMES_INI_FILE)
          || !strcmp(szFile, SAPDB_INSTALLATIONS_INI_FILE)
          || !strcmp(szFile, SAPDB_DATABASES_INI_FILE) )
        {
            oldPath = (char *)(SAPDB_OLD_GLOBAL_CONFIG_DIRECTORY "/" SAPDB_OLD_GLOBAL_COMMON_CONFIG_FILE);
        }
        else
        {
            oldPath = (char *)alloca(strlen(SAPDB_OLD_GLOBAL_CONFIG_DIRECTORY "/") + strlen(szFile) + 1);
            strcpy(oldPath, SAPDB_OLD_GLOBAL_CONFIG_DIRECTORY "/"); strcat(oldPath, szFile);
        }

        return RTE_GetGlobalConfigString( oldPath,
                                          szSection,
                                          szEntry,
                                          szString,
                                          MaxStringLen,
                                          ErrText,
                                          pOk);
    }
#endif
}
/*---------------------------------------------------------------------------*/
/*                           Put Config String                               */
/*---------------------------------------------------------------------------*/

/*
  Function:     RTE_PutConfigString ()
  description:  Write or update Registry Entry
  
   Inserts or replaces a string into the ini-file name 'szString'
   to a position descripted by 'section'-name and 'entry'-name.
 
   If the file does not exists, it will be created
   files are located in <sapdb_indep>/config.
   If <sapdb_indep>/config does not exists the function fails,
  
   This function needs write permission in '<sapdb_indep>/config'
   to create a temporary file, otherwise it fails 
    
  arguments:
    szFile [in] Name of registry file
    szSection [in] Section to write into
    szEntry [in] Given key of 'key=value' entry
    szString [in] Given value of 'key=value' entry
    ErrText [out] Place for descriptive error message
    Ok [out] Error return value

  return value: none, but err is updated to
    <UL>
        <LI>0: failed ( inspect errno )
        <LI>1: ok
    </UL>
 */
SAPDB_Int4 RTE_PutConfigString (
    const SAPDB_Char *szFile,
    const SAPDB_Char *szSection,
    const SAPDB_Char *szEntry, 
    const SAPDB_Char *szString,
    tsp00_ErrTextc ErrText,
    RTE_IniFileResult *pOk)
{
#if defined(WIN32)
    if ( strcmp(szFile, SAPDB_INSTALLATIONS_INI_FILE)
      && strcmp(szFile, SAPDB_DATABASES_INI_FILE) )
    {    
        return RTE_PutWindowsConfigString(NULL,
                                        false,
                                        (SAPDB_Bool)(strcmp(szSection, SAPDB_RUNTIME_SECTION) == 0),
                                        szSection,
                                        szEntry,
                                        szString,
                                        ErrText,
                                        pOk );
    }
#endif
  {
    SAPDB_Int4 result;
    SAPDB_Char *szPath;
    RTE_Path indepConfigPath;
    SAPDB_Bool lockedOnlyUpdate;

    if ( !szFile || !szSection )
    {
        *pOk = SAPDB_INIFILE_RESULT_ERR_PARAM;
        strcpy( (SAPDB_Char *)ErrText, "NULL pointer for file or section passed" );
        return 0;
    }

    if ( (getenv(SAPDB_OWN_GLOBAL_CONFIG_FILE) != 0 && !strcmp(szFile, getenv(SAPDB_OWN_GLOBAL_CONFIG_FILE)))
       ||!strcmp(szFile, SAPDB_GLOBAL_CONFIG_FILE) )
    {
        szPath = (SAPDB_Char *)alloca(strlen(szFile)+1);
        strcpy(szPath, szFile);
    }
    else if ( *szFile == '/' )
    {
        *pOk = SAPDB_INIFILE_RESULT_ERR_PARAM;
        strcpy ( (SAPDB_Char *)ErrText, "Only relativ pathes allowed" );
        return 0;
    }
    else
    {
        if ( !strcmp(SAPDB_ODBC_INI_FILE, szFile) )
        {
            szPath = (SAPDB_Char *)alloca(strlen(UNIX_GLOBAL_ODBC_INI_FILE)+1);
            strcpy(szPath, UNIX_GLOBAL_ODBC_INI_FILE);
        }
        else
        {
            if ( !RTE_GetCommonConfigPath(indepConfigPath, false, ErrText) )
            {
                *pOk = SAPDB_INIFILE_RESULT_ERR_PARAM;
                return 0;
            }
            /* If needed create directory for registry file */
            if ( !ValidateConfigPath(indepConfigPath, ErrText, pOk) )
            {
                /* Ok & ErrText already setup... */
                return 0;
            }

            szPath = (SAPDB_Char *)alloca(strlen(indepConfigPath)+1+strlen(szFile)+1);
#ifdef _WIN32
            strcpy(szPath, indepConfigPath); strcat(szPath, "\\"); strcat(szPath, szFile);
#else
            strcpy(szPath, indepConfigPath); strcat(szPath, "/"); strcat(szPath, szFile);
#endif
        }
    }


    /* Installations.ini and Runtimes.ini are handled by installer only, which runs always only in one incarnation */
    lockedOnlyUpdate = ( (0 == strcmp(szFile, SAPDB_INSTALLATIONS_INI_FILE))
                      || (0 == strcmp(szFile, SAPDB_RUNTIMES_INI_FILE)) ? SAPDB_UNLOCKED_ACCESS : SAPDB_LOCKED_ACCESS );

    
	if ( !lockedOnlyUpdate )
    {
#ifdef _WIN32
        if( INVALID_FILE_ATTRIBUTES != GetFileAttributes(szPath))
#else
        if ( 0 == access(szPath, F_OK|R_OK) )
#endif
        {
            if ( my_save_chmod(szPath, 0644) == -1 )
            {
                *pOk = SAPDB_INIFILE_RESULT_ACCESS_DENIED;
                strcpy ( (SAPDB_Char *)ErrText, "Failed to write enable" );
                return 0;
            }
        }
    }

	result = UpdateConfigString ( lockedOnlyUpdate,
                                  szPath,
                                  szSection,
                                  szEntry,
                                  szString,
                                  false,
                                  ErrText, 
                                  pOk );

	if ( !lockedOnlyUpdate )
    {
		(void)my_save_chmod(szPath, 0444); /* ignore error on resetting to read only mode */
	}

    return result;
  }
}

/*
  Function:     RTE_PutInstallationConfigString ()
  description:  Write or update global Registry Entry
  
   Inserts or replaces a string in the global ini-file.
   The entry named 'szString' is at a position descripted by 
   fixed section "Globals" and 'entry'-name.
 
   If the file does not exists, it will be created
   files are located in "/etc/opt/". If "/etc/opt/" does not exists the function fails,
  
   This function needs write permission in "/etc/opt/"
   to create a temporary file, otherwise it fails too.
    
  arguments:
    szEntry [in] Given key of 'key=value' entry
    szString [in] Given value of 'key=value' entry
    ErrText [out] Place for descriptive error message
    Ok [out] Error return value

  return value: none, but err is updated to
    <UL>
        <LI>0: failed ( inspect errno )
        <LI>1: ok
    </UL>
 */
SAPDB_Int4 RTE_PutInstallationConfigString (
    const SAPDB_Char *szEntry, 
    const SAPDB_Char *szString,
    tsp00_ErrTextc ErrText,
    RTE_IniFileResult *pOk)
{
#if defined(WIN32)
    return RTE_PutWindowsConfigString( NULL,
                                       false, 
                                       false,
                                       SAPDB_GLOBALS_SECTION,
                                       szEntry,
                                       szString,
                                       ErrText,
                                       pOk );
#else
    RTE_Path pathToOwnGlobalConfigFile;

    if ( !myGetEnv(SAPDB_OWN_GLOBAL_CONFIG_FILE, pathToOwnGlobalConfigFile, sizeof(RTE_Path) ) )
    {
        SAPDB_strncpy_zero_terminated( &pathToOwnGlobalConfigFile[0], SAPDB_GLOBAL_CONFIG_FILE, sizeof(RTE_Path));
    }

    return ( UpdateConfigString ( false,
                                  &pathToOwnGlobalConfigFile[0],
                                  SAPDB_GLOBALS_SECTION,
                                  szEntry, 
                                  szString, 
                                  false,
                                  ErrText,
                                  pOk) );
#endif
}

/*!
   @brief          Write or update user specific registry Entry
   @param          szFile [in] Name of registry file in $HOME/.sapdb
   @param          szSection [in] Section to write into
   @param          szEntry [in] Given key of 'key=value' entry
   @param          szString [in] Given value of 'key=value' entry
   @param          ErrText [out] Place for descriptive error message
   @param          Ok [out] Error return value  Possible values for Ok 
   &lt;UL>    
   &lt;LI>SAPDB_INIFILE_RESULT_ERR_KEY   bad key given (contains '=') or bad entry found (no '=')    
   &lt;LI>SAPDB_INIFILE_RESULT_ERR_TEMP  failed to create temp file or directory    
   &lt;LI>SAPDB_INIFILE_RESULT_ERR_OPEN  failed to open registry    
   &lt;LI>SAPDB_INIFILE_RESULT_ERR_LOCK  failed to lock registry    
   &lt;LI>SAPDB_INIFILE_RESULT_ERR_READ  failed to read registry    
   &lt;LI>SAPDB_INIFILE_RESULT_ERR_WRITE failed to write temp file    
   &lt;LI>SAPDB_INIFILE_RESULT_ERR_SEEK  failed to seek to begin of file    
   &lt;LI>SAPDB_INIFILE_RESULT_ERR_PARAM   NULL pointer given    
   &lt;LI>SAPDB_INIFILE_RESULT_OK  No error 
   &lt;/UL>
   @return         none, but err is updated to
  &lt;UL>
      &lt;LI>0: failed ( inspect Ok )
      &lt;LI>1: ok
  &lt;/UL>


    Inserts or replaces a string into the user specific ini-file name 'szString'
    to a position descripted by 'section'-name and 'entry'-name.
    If the file does not exists, it will be created
    Files are located in '$HOME/.sapdb'.
    If this directory does not exists, the directory 'ini' will be 
    created in '$HOME'
    If '$HOME' does not exists the function failed
    This function needs write permission in '$HOME/.sapdb'
    to create a temporary file, otherwise it fails 
 */
SAPDB_Int4 RTE_PutUserConfigString (
    SAPDB_Char const * accountName,
    const SAPDB_Char * szFile,
    const SAPDB_Char * szSection,
    const SAPDB_Char * szEntry,
    const SAPDB_Char * szString,
    tsp00_ErrTextc   ErrText,
    RTE_IniFileResult    * pOk)
{
#if defined(WIN32)
    return RTE_PutWindowsConfigString (accountName,
                                           true,
                                           (SAPDB_Bool)(strcmp(szSection, SAPDB_RUNTIME_SECTION) == 0),
                                           szSection,
                                           szEntry,
                                           szString,
                                           ErrText,
                                           pOk);

#else
    SAPDB_Char *szPath;
    RTE_Path userConfigPath;

    if ( !szFile || !szSection )
    {
        *pOk = SAPDB_INIFILE_RESULT_ERR_PARAM;
        strcpy( (SAPDB_Char *)ErrText, "NULL pointer for file or section passed" );
        return 0;
    }

    if ( *szFile == '/' )
    {
        *pOk = SAPDB_INIFILE_RESULT_ERR_PARAM;
        strcpy ( (SAPDB_Char *)ErrText, "Only relativ pathes allowed" );
        return 0;
    }

    if ( !strcmp(SAPDB_ODBC_INI_FILE, szFile) )
    {
        if ( myGetEnv("ODBCINI", userConfigPath, sizeof(RTE_Path) ) )
        {
            szPath = (SAPDB_Char *)alloca(strlen(userConfigPath)+strlen(UNIX_USER_ODBC_INI_FILE)+1);
            strcpy(szPath, userConfigPath); strcat(szPath, UNIX_USER_ODBC_INI_FILE);
        }
        else
        {
            SAPDB_UInt4 neededSize = 0;
            SAPDB_Char  tmpBuffer[2];
            if ( !RTE_GetHomeDirectoryFromPasswdById( (SAPDB_Int4)geteuid ()
                                                    , tmpBuffer
                                                    , sizeof(tmpBuffer)
                                                    , &neededSize ) && 0 == neededSize )
            {
                strcpy((SAPDB_Char *)ErrText, "Found no home directory entry");
                *pOk = SAPDB_INIFILE_RESULT_ERR_PARAM;
                return 0;
            }
            szPath = (SAPDB_Char *)alloca(neededSize+strlen(UNIX_USER_ODBC_INI_FILE)+1);
            if ( !RTE_GetHomeDirectoryFromPasswdById( (SAPDB_Int4)geteuid ()
                                                    , szPath
                                                    , neededSize
                                                    , &neededSize ) )
            {
                strcpy((SAPDB_Char *)ErrText, "Found no home directory entry");
                *pOk = SAPDB_INIFILE_RESULT_ERR_PARAM;
                return 0;
            }
            strcat(szPath, UNIX_USER_ODBC_INI_FILE);
        }
    }
    else 
    {
        if ( !RTE_GetUserSpecificConfigPath(userConfigPath, false, ErrText) )
        {
            *pOk = SAPDB_INIFILE_RESULT_ERR_OPEN;
            return 0;
        }

        /* If needed create directory for registry file */
        if ( !ValidateConfigPath(userConfigPath, ErrText, pOk) )
        {
            /* Ok & ErrText already setup... */
            return 0;
        }

        szPath = (SAPDB_Char *)alloca(strlen(userConfigPath)+1+strlen(szFile)+1);
        strcpy(szPath, userConfigPath); strcat(szPath, "/"); strcat(szPath, szFile);
    }

    return ( UpdateConfigString ( true,
                                  szPath,
                                  szSection,
                                  szEntry, 
                                  szString, 
                                  false,
                                  ErrText,
                                  pOk) );
#endif
}

/*---------------------------------------------------------------------------*/

void RTE_GetUserConfigFileLocker (
    const SAPDB_Char *szFile,
    SAPDB_Bool *pFileLocked,
    SAPDB_Bool *pIsLocalhost,
    RTE_LockFileContent *pContent,
    tsp00_ErrTextc ErrText,
    RTE_IniFileResult *pOk)
{
    SAPDB_Bool fileExists;
    if ( !szFile )
    {
        *pOk = SAPDB_INIFILE_RESULT_ERR_PARAM;
        strcpy( (SAPDB_Char *)ErrText, "NULL pointer for file passed" );
        return;
    }

    if ( !pFileLocked || !pIsLocalhost || !pContent || !pOk )
    {
        *pOk = SAPDB_INIFILE_RESULT_ERR_PARAM;
        strcpy( (SAPDB_Char *)ErrText, "NULL pointer for parameter passed" );
        return;
    }

    *pOk = SAPDB_INIFILE_RESULT_OK;    

    *pFileLocked = false;
    *pIsLocalhost = false;
    memset( pContent, 0, sizeof(RTE_LockFileContent));
    ErrText[0] = 0;
#if !defined(WIN32)
    {
        SAPDB_Char *szPath;
        RTE_Path userConfigPath;
        RTE_LockFile RegistryFile;

        if ( *szFile == '/' )
        {
            *pOk = SAPDB_INIFILE_RESULT_ERR_PARAM;
            strcpy ( (SAPDB_Char *)ErrText, "Only relativ pathes allowed" );
            return;
        }

        if ( !strcmp(SAPDB_ODBC_INI_FILE, szFile) )
        {
            if ( myGetEnv("ODBCINI", userConfigPath, sizeof(RTE_Path) ) )
            {
                szPath = (SAPDB_Char *)alloca(strlen(userConfigPath)+strlen(UNIX_USER_ODBC_INI_FILE)+strlen(SDB_LOCK_FILE_SUFFIX)+1);
                strcpy(szPath, userConfigPath); strcat(szPath, UNIX_USER_ODBC_INI_FILE); strcat(szPath, SDB_LOCK_FILE_SUFFIX);
            }
            else
            {
                SAPDB_UInt4 neededSize = 0;
                SAPDB_Char  tmpBuffer[2];
                if ( !RTE_GetHomeDirectoryFromPasswdById( (SAPDB_Int4)geteuid ()
                                                        , tmpBuffer
                                                        , (SAPDB_UInt4)2
                                                        , &neededSize ) && 0 == neededSize )
                {
                    strcpy((SAPDB_Char *)ErrText, "Found no home directory entry");
                    *pOk = SAPDB_INIFILE_RESULT_ERR_PARAM;
                    return;
                }
                szPath = (SAPDB_Char *)alloca(neededSize+strlen(UNIX_USER_ODBC_INI_FILE)+strlen(SDB_LOCK_FILE_SUFFIX)+1);
                if ( !RTE_GetHomeDirectoryFromPasswdById( (SAPDB_Int4)geteuid ()
                                                        , szPath
                                                        , neededSize
                                                        , &neededSize ) )
                {
                    strcpy((SAPDB_Char *)ErrText, "Found no home directory entry");
                    *pOk = SAPDB_INIFILE_RESULT_ERR_PARAM;
                    return;
                }
                strcat(szPath, UNIX_USER_ODBC_INI_FILE); strcat(szPath, SDB_LOCK_FILE_SUFFIX);
            }
        }
        else 
        {
            if ( !RTE_GetUserSpecificConfigPath(userConfigPath, false, ErrText) )
            {
                *pOk = SAPDB_INIFILE_RESULT_ERR_OPEN;
                return;
            }

            /* If needed create directory for registry file */
            if ( !ValidateConfigPath(userConfigPath, ErrText, pOk) )
            {
                /* Ok & ErrText already setup... */
                return;
            }

            szPath = (SAPDB_Char *)alloca(strlen(userConfigPath)+1+strlen(szFile)+strlen(SDB_LOCK_FILE_SUFFIX)+1);
            strcpy(szPath, userConfigPath); strcat(szPath, "/"); strcat(szPath, szFile); strcat(szPath, SDB_LOCK_FILE_SUFFIX);
        }

        RegistryFile_Init(&RegistryFile, false, true);

        if ( !RegistryFile_Open(&RegistryFile, szPath) )
        {
            SAPDB_Char const *errmsg;
            fileExists = false;
            switch( errno )
            {
            case ENOENT:
                return; /* no real error...file just not exists */
            case ESTALE:
                return; /* no real error...file just has been removed */
            default:
                break;
            }
            strcpy((SAPDB_Char *)ErrText, "Open lock:");
            strncpy( (SAPDB_Char *)ErrText, GetLastSystemErrorAsString(), sizeof(tsp00_ErrTextc)-1);
            *pOk = SAPDB_INIFILE_RESULT_ERR_OPEN;
            return;
        }

        {
            SAPDB_Int4 ReadResult = read(RegistryFile.Fd, pContent, sizeof(RTE_LockFileContent));

            if ( ReadResult != sizeof(RTE_LockFileContent) )
            {
                if ( ReadResult < 0 )
                {
                    strcpy((SAPDB_Char *)ErrText, "Read lock:");
                    strncpy( (SAPDB_Char *)ErrText, GetLastSystemErrorAsString(), sizeof(tsp00_ErrTextc)-1);
                    *pOk = SAPDB_INIFILE_RESULT_ERR_READ;
                    RegistryFile_Close(&RegistryFile);
                    return;
                }
            }
        }

        *pFileLocked = ( 0 != SAPDB_memcmp(pContent, &RegistryFile_Nobody, sizeof(RTE_LockFileContent)) );
        *pIsLocalhost = ( 0 == strcmp( &pContent->machineName[0]
                                     , &RegistryFile.m_LockEntry.machineName[0] ) );
        RegistryFile_Close(&RegistryFile);
    }
#endif
}

/*---------------------------------*/

volatile SAPDB_Bool  RTE_RetryWanted = true;
volatile SAPDB_UInt4 RTE_RetryPollLoops     = 100;
volatile SAPDB_UInt4 RTE_RetryLocalTimeout  = 0;
volatile SAPDB_UInt4 RTE_RetryRemoteTimeout = 300;

/*!
  @brief Set timeout and retry values for advisory lock

  Retry of advisory lock is only of importance for UNIX, where the registry is simulated by a file.
  This file can set in an NFS filesystem and therefore the advisory lock used to synchronize the
  access is not always available. The 'retry' of this locking is complete switchable by 'retryWanted'
  flag. If 'pollLoops' is larger then zero, the first retries do only give up time slice and retry.
  The advisory lock can be hold by a local process in which case the alive test for the process works fine
  and kept locked by a remote machine. It is recommended to keep the default for local timeout to '0' which means
  unlimited and for 'remote timeout' to a meaningfull value depending on the network in use. The defaults are
  '100 pollLoops', then unlimited number of 'one second sleep intervals' for local timeout (0) and a 5 minute 
  timeout for remote (300).

  If lock collision occured the registry function will return    
    SAPDB_INIFILE_RESULT_ERR_LOCK    failed to lock registry
  for all registry function that work on writable files.

  @param          retryWanted [in] new value for retry wanted flag
  @param          pollLoops [inout] input new value, output old value for 'poll loops'
  @param          localTimeout [inout] number of one second interval to wait in case of local lock collision
  @param          remoteTimeout [inout] number of one second interval to wait in case of remote lock collistion
  @return old value of 'retryWanted' flag.
 */
SAPDB_Bool RTE_SetRetryWanted( SAPDB_Bool newFlag
                             , SAPDB_UInt4 *pNewPollLoops
                             , SAPDB_UInt4 *pNewLocalTimeout
                             , SAPDB_UInt4 *pNewRemoteTimeout )
{
    SAPDB_Bool oldFlag = RTE_RetryWanted;
    RTE_RetryWanted = newFlag;

    if ( pNewPollLoops )
    {
        SAPDB_UInt4 oldPollLoops = RTE_RetryPollLoops;
        RTE_RetryPollLoops = *pNewPollLoops;
        *pNewPollLoops = oldPollLoops;
    }

    if ( pNewLocalTimeout )
    {
        SAPDB_UInt4 oldLocalTimeout = RTE_RetryLocalTimeout;
        RTE_RetryLocalTimeout = *pNewLocalTimeout;
        *pNewLocalTimeout = oldLocalTimeout;
    }

    if ( pNewRemoteTimeout )
    {
        SAPDB_UInt4 oldRemoteTimeout = RTE_RetryRemoteTimeout;
        RTE_RetryRemoteTimeout = *pNewRemoteTimeout;
        *pNewRemoteTimeout = oldRemoteTimeout;
    }

    return oldFlag;
}

/*---------------------------------------------------------------------------*/

/*
  Function:     RTE_RemoveConfigString ()
  description:  Remove entry from Registry
                
    Removes the entry specified by 'szSection' and 'szEntry' from config file 

    This function needs write permission in <indep_data>/config to create a temporary file, otherwise it fails .
    
  arguments:
    szFile [in] Name of registry file
    szSection [in] Section to write into
    szEntry [in] Given key of 'key=value' entry
    ErrText [out] Place for descriptive error message
    Ok [out] Error return value

  return value: none, but err is updated to
    <UL>
        <LI>0: failed ( inspect errno )
        <LI>1: ok
    </UL>
 */
SAPDB_Int4 RTE_RemoveConfigString (
    const SAPDB_Char *szFile,
    const SAPDB_Char *szSection,
    const SAPDB_Char *szEntry, 
    tsp00_ErrTextc ErrText,
    RTE_IniFileResult *pOk)
{
#if defined(WIN32)
    if ( strcmp(szFile, SAPDB_INSTALLATIONS_INI_FILE)
      && strcmp(szFile, SAPDB_DATABASES_INI_FILE) )
    {
        return RTE_RemoveWindowsConfigString( NULL,
                                            false,
                                            (SAPDB_Bool)(strcmp(szSection, SAPDB_RUNTIME_SECTION) == 0),
                                            szSection,
                                            szEntry,
                                            ErrText,
                                            pOk );
    }
#endif
  {
    SAPDB_Int4 firstResult;
    tsp00_ErrTextc firstErrText;
    RTE_IniFileResult firstOk;
    SAPDB_Int4 fallbackResult;
    tsp00_ErrTextc fallbackErrText;
    RTE_IniFileResult fallbackOk;
    SAPDB_Char *szPath;
    char *oldPath;
    SAPDB_Bool wantLock;

    if ( !szFile || !szSection )
    {
        *pOk = SAPDB_INIFILE_RESULT_ERR_PARAM;
        strcpy( (SAPDB_Char *)ErrText, "NULL pointer for file or section passed" );
        return 0;
    }

    if ( *szFile == '/' )
    {
        *pOk = SAPDB_INIFILE_RESULT_ERR_PARAM;
        strcpy ( (SAPDB_Char *)ErrText, "Only relativ pathes allowed" );
        return 0;
    }

    /* Installations.ini and Runtimes.ini are handled by installer only, which runs always only in one incarnation */
	if (( !strcmp(szFile, SAPDB_INSTALLATIONS_INI_FILE) ||
		  !strcmp(szFile, SAPDB_RUNTIMES_INI_FILE )))
	{
		RTE_Path indepConfigPath;

		if ( !RTE_GetCommonConfigPath(indepConfigPath, false, ErrText) )
		{
			*pOk = SAPDB_INIFILE_RESULT_ERR_PARAM;
			return 0;
		}

		szPath = (SAPDB_Char *)alloca(strlen(indepConfigPath)+1+strlen(szFile)+1);
		strcpy(szPath, indepConfigPath); strcat(szPath, "/"); strcat(szPath, szFile);

        wantLock = SAPDB_UNLOCKED_ACCESS;
	}
    else
    {
        wantLock = SAPDB_LOCKED_ACCESS;
    }

    firstOk = SAPDB_INIFILE_RESULT_OK;
    if ( SAPDB_UNLOCKED_ACCESS == wantLock )
    {
#ifndef _WIN32
        if ( 0 == access(szPath, F_OK|R_OK) )
#endif
        {
            if ( my_save_chmod(szPath, 0644) == -1 )
            {
                firstOk = SAPDB_INIFILE_RESULT_ACCESS_DENIED;
                strcpy ( (SAPDB_Char *)firstErrText, "Failed to write enable" );
            }
        }
    }

    if ( SAPDB_INIFILE_RESULT_OK == firstOk )
    {
        firstResult = RTE_RemoveUNIXConfigString ( wantLock,
                                                   szFile,
                                                   szSection,
                                                   szEntry, 
                                                   firstErrText,
                                                  &firstOk );

        if ( SAPDB_UNLOCKED_ACCESS == wantLock )
	    {
		    (void)my_save_chmod(szPath, 0444); /* ignore problem to reset to read only mode */
        }
    }


#ifndef _WIN32
    if ( !strcmp(szFile, SAPDB_RUNTIMES_INI_FILE)
        || !strcmp(szFile, SAPDB_INSTALLATIONS_INI_FILE)
        || !strcmp(szFile, SAPDB_DATABASES_INI_FILE) )
    {
        oldPath = (char *)(SAPDB_OLD_GLOBAL_CONFIG_DIRECTORY "/" SAPDB_OLD_GLOBAL_COMMON_CONFIG_FILE);
    }
    else
    {
        oldPath = (char *)alloca(strlen(SAPDB_OLD_GLOBAL_CONFIG_DIRECTORY "/") + strlen(szFile) + 1);
        strcpy(oldPath, SAPDB_OLD_GLOBAL_CONFIG_DIRECTORY "/"); strcat(oldPath, szFile);
    }

    fallbackResult = RTE_RemoveUNIXConfigString ( wantLock,
                                                  oldPath,
                                                  szSection,
                                                  szEntry, 
                                                  fallbackErrText,
                                                 &fallbackOk );
#endif

#ifdef _WIN32
    if ( SAPDB_INIFILE_RESULT_OK != firstOk )
    {
        SAPDB_memcpy(ErrText, firstErrText, sizeof(tsp00_ErrTextc));
    }
    *pOk = firstOk;
    return firstResult;
#else
    if ( SAPDB_INIFILE_RESULT_OK == firstOk )
    {
        *pOk = firstOk;
        return firstResult;
    }

    if ( SAPDB_INIFILE_RESULT_OK == fallbackOk )
    {
        *pOk = fallbackOk;
        return fallbackResult;
    }

    if ( SAPDB_INIFILE_RESULT_NO_ENTRY != firstOk )
    {
        *pOk = firstOk;
        SAPDB_memcpy(ErrText, firstErrText, sizeof(tsp00_ErrTextc));
        return firstResult;
    }

    *pOk = fallbackOk;
    SAPDB_memcpy(ErrText, fallbackErrText, sizeof(tsp00_ErrTextc));
    return fallbackResult;
#endif
  }
}

/*!
   @brief          Remove User specific Entry from Registry

     Removes the entry specified by 'szSection' and 'szEntry' from config file 
     This function needs write permission in '$HOME/.sapdb'
     to create a temporary file, otherwise it fails .
 */

SAPDB_Int4 RTE_RemoveUserConfigString (
    SAPDB_Char const * accountName,
    const SAPDB_Char * szFile,
    const SAPDB_Char * szSection,
    const SAPDB_Char * szEntry,
    tsp00_ErrTextc ErrText,
    RTE_IniFileResult * pOk)
{
#if defined(WIN32)
    return RTE_RemoveWindowsConfigString(accountName,
                                             true,
                                             (SAPDB_Bool)(strcmp(szSection, SAPDB_RUNTIME_SECTION) == 0),
                                             szSection,
                                             szEntry,
                                             ErrText,
                                             pOk );
#else
    SAPDB_Char *szPath;
    RTE_Path userConfigPath;

    if ( !szFile || !szSection )
    {
        *pOk = SAPDB_INIFILE_RESULT_ERR_PARAM;
        strcpy( (SAPDB_Char *)ErrText, "NULL pointer for file or section passed" );
        return 0;
    }

    if ( *szFile == '/' )
    {
        *pOk = SAPDB_INIFILE_RESULT_ERR_PARAM;
        strcpy ( (SAPDB_Char *)ErrText, "Only relativ pathes allowed" );
        return 0;
    }

    if ( !strcmp(SAPDB_ODBC_INI_FILE, szFile) )
    {
        if ( myGetEnv("ODBCINI", userConfigPath, sizeof(RTE_Path) ) )
        {
            szPath = (SAPDB_Char *)alloca(strlen(userConfigPath)+strlen(UNIX_USER_ODBC_INI_FILE)+1);
            strcpy(szPath, userConfigPath); strcat(szPath, UNIX_USER_ODBC_INI_FILE);
        }
        else
        {
            SAPDB_UInt4 neededSize = 0;
            SAPDB_Char  tmpBuffer[2];
            if ( !RTE_GetHomeDirectoryFromPasswdById( (SAPDB_Int4)geteuid ()
                                                    , tmpBuffer
                                                    , (SAPDB_UInt4)2
                                                    , &neededSize ) && 0 == neededSize )
            {
                strcpy((SAPDB_Char *)ErrText, "Found no home directory entry");
                *pOk = SAPDB_INIFILE_RESULT_ERR_PARAM;
                return 0;
            }
            szPath = (SAPDB_Char *)alloca(neededSize+strlen(UNIX_USER_ODBC_INI_FILE)+1);
            if ( !RTE_GetHomeDirectoryFromPasswdById( (SAPDB_Int4)geteuid ()
                                                    , szPath
                                                    , neededSize
                                                    , &neededSize ) )
            {
                strcpy((SAPDB_Char *)ErrText, "Found no home directory entry");
                *pOk = SAPDB_INIFILE_RESULT_ERR_PARAM;
                return 0;
            }
            strcat(szPath, UNIX_USER_ODBC_INI_FILE);
        }
    }
    else 
    {
        if ( !RTE_GetUserSpecificConfigPath(userConfigPath, false, ErrText) )
        {
            *pOk = SAPDB_INIFILE_RESULT_ERR_OPEN;
            return 0;
        }

        /* If needed create directory for registry file */
        if ( !ValidateConfigPath(userConfigPath, ErrText, pOk) )
        {
            /* Ok & ErrText already setup... */
            return 0;
        }

        szPath = (SAPDB_Char *)alloca(strlen(userConfigPath)+1+strlen(szFile)+1);
        strcpy(szPath, userConfigPath); strcat(szPath, "/"); strcat(szPath, szFile);
    }

    return ( UpdateConfigString (
                    true,
                    szPath,
                    szSection,
                    szEntry,
                    NULL, 
                    true, 
                    ErrText,
                    pOk) );
#endif
}

/*---------------------------------------------------------------------------*/

/*
  Function:     RTE_OpenGlobalConfigEnum ()
  description:  Open registry for enumeration of a section
                
    Open the registery and position to the given section.
    It is expected that a loop over 'RTE_NextConfigEnum()' is used to retrieve the
    entries.

    Note: The returned handle has no state information. The complete Registry is locked.
    Therefore you must call RTE_CloseConfigEnum() after last usage, or the Registry is locked
    until end of process.
  
  arguments:
    szFile [in] Name of registry file
    szSection [in] Section to write into
    ErrText [out] Place for descriptive error message
    Ok [out] Error return value

  return value:
    <UL>
        handle  used in subsequent RTE_NextConfigEnum calls
    </UL>
 */
RTE_RegistryHandle RTE_OpenGlobalConfigEnum (
    const SAPDB_Char *szFile,
    const SAPDB_Char *szSection,
    tsp00_ErrTextc ErrText, 
    RTE_IniFileResult *pOk)
{
    SAPDB_Int4          Fd;
    SAPDB_Char *        szPath;
    RTE_RegistryHandle  hEnum;
    RTE_RegistryHandle  handle;
    SAPDB_Int4          readOnly;

    if ( !szFile || !szSection )
    {
        *pOk = SAPDB_INIFILE_RESULT_ERR_PARAM;
        strcpy( (SAPDB_Char *)ErrText, "OpenConfigEnum NULL pointer passed" );
        return 0;
    }

    hEnum = (RTE_RegistryHandle)malloc(sizeof(RTE_RegistryHandleStruct)
                                       +strlen(szSection)+1
                                       +strlen(szFile)+1);
    if ( !hEnum )
    {
        strcpy((char *)ErrText, "Out of memory");
        *pOk = SAPDB_INIFILE_RESULT_ERR_MEMORY;
        return 0;
    }

    hEnum->section = (SAPDB_Char *)(hEnum+1);
    strcpy(hEnum->section, szSection);
    hEnum->file = hEnum->section + strlen(szSection) + 1;
    strcpy(hEnum->file, szFile);
    hEnum->parseAll = 0;
    hEnum->buffer = NULL;

#if defined(WIN32)
    if ( strcmp(szFile, SAPDB_INSTALLATIONS_INI_FILE)
      && strcmp(szFile, SAPDB_DATABASES_INI_FILE) )
    {
        hEnum->location = RTE_GlobalRegistry;

        if ( *szFile == '/' )
        {
            *pOk = SAPDB_INIFILE_RESULT_ERR_PARAM;
            strcpy ( (SAPDB_Char *)ErrText, "Only relativ pathes allowed" );
            return 0;
        }

        handle = RTE_OpenWindowsConfigEnum (NULL,
                                        false,
                                        hEnum,
                                        szSection,
                                        ErrText,
                                        pOk );
        if (0 == handle)
        {
            free(hEnum);
        }

        return handle;
    }
    hEnum->location = RTE_GlobalRegistry;
#else
    if ( !strncmp(szFile, SAPDB_OLD_GLOBAL_CONFIG_DIRECTORY "/", strlen(SAPDB_OLD_GLOBAL_CONFIG_DIRECTORY "/")) )
    {
        hEnum->location = RTE_OldUNIXRegistry;
    }
    else
    {
        hEnum->location = RTE_GlobalRegistry;
    }

    if ( *szFile == '/' )
    {
        if ( !strncmp(szFile, SAPDB_OLD_GLOBAL_CONFIG_DIRECTORY "/", strlen(SAPDB_OLD_GLOBAL_CONFIG_DIRECTORY "/"))
          ||  (getenv(SAPDB_OWN_GLOBAL_CONFIG_FILE) != 0 && !strcmp(szFile, getenv(SAPDB_OWN_GLOBAL_CONFIG_FILE)))
          || !strcmp(szFile, SAPDB_GLOBAL_CONFIG_FILE) )
        {
            szPath = (SAPDB_Char *)alloca(strlen(szFile)+1);
            strcpy (szPath, szFile);
        }
        else
        {
            *pOk = SAPDB_INIFILE_RESULT_ERR_PARAM;
            strcpy ( (SAPDB_Char *)ErrText, "Only relativ pathes allowed" );
            return 0;
        }
    }
    else
#endif
    {
        RTE_Path indepConfigPath;
        if ( !RTE_GetCommonConfigPath(indepConfigPath, true, ErrText) )
        {
            *pOk = SAPDB_INIFILE_RESULT_ERR_PARAM;
            return 0;
        }
        szPath = (SAPDB_Char *)alloca(strlen(indepConfigPath)+strlen(szFile)+1);
        strcpy(szPath, indepConfigPath); strcat(szPath, szFile);
    }

    readOnly = (  !strcmp(szFile, SAPDB_INSTALLATIONS_INI_FILE)
              ||  !strcmp(szFile, SAPDB_RUNTIMES_INI_FILE) 
              ||  (getenv(SAPDB_OWN_GLOBAL_CONFIG_FILE) != 0 && !strcmp(szFile, getenv(SAPDB_OWN_GLOBAL_CONFIG_FILE)))
              ||  !strcmp(szFile, SAPDB_GLOBAL_CONFIG_FILE)
#ifndef _WIN32
              || !strncmp(szFile, SAPDB_OLD_GLOBAL_CONFIG_DIRECTORY "/", strlen(SAPDB_OLD_GLOBAL_CONFIG_DIRECTORY "/")) 
#endif
              );

    return OpenConfigFileForEnumeration(hEnum,
                                        szPath,
                                        readOnly,
                                        szSection,
                                        ErrText,
                                        pOk);
}

/*-----------------------------------*/

RTE_RegistryHandle RTE_OpenUserConfigEnum (
    SAPDB_Char const * accountName,
    const SAPDB_Char * szFile,
    const SAPDB_Char * szSection,
    tsp00_ErrTextc ErrText, 
    RTE_IniFileResult  * pOk)
{
    RTE_RegistryHandle hEnum;
    RTE_RegistryHandle handle;

    if ( !szFile || !szSection )
    {
        *pOk = SAPDB_INIFILE_RESULT_ERR_PARAM;
        strcpy( (SAPDB_Char *)ErrText, "NULL pointer passed for file or section" );
        return 0;
    }

    if ( *szFile == '/' )
    {
        *pOk = SAPDB_INIFILE_RESULT_ERR_PARAM;
        strcpy ( (SAPDB_Char *)ErrText, "Only relativ pathes allowed" );
        return 0;
    }

    hEnum = (RTE_RegistryHandle)malloc( sizeof(RTE_RegistryHandleStruct)
                                        +strlen(szSection)+1
                                        +strlen(szFile)+1);
    if ( !hEnum )
    {
        strcpy((char *)ErrText, "Out of memory");
        *pOk = SAPDB_INIFILE_RESULT_ERR_MEMORY;
        return 0;
    }

    hEnum->section = (SAPDB_Char *)(hEnum+1);
    strcpy(hEnum->section, szSection);
    hEnum->file = hEnum->section + strlen(szSection) + 1;
    strcpy(hEnum->file, szFile);
    hEnum->parseAll = 0;
    hEnum->buffer = NULL;

    hEnum->location = RTE_UserRegistry;

#if defined(WIN32)
    if ( strcmp(szFile, SAPDB_INSTALLATIONS_INI_FILE)
      && strcmp(szFile, SAPDB_DATABASES_INI_FILE) )
    {
        handle = RTE_OpenWindowsConfigEnum (accountName,
                                            true,
                                            hEnum,
                                            szSection,
                                            ErrText,
                                            pOk );
        if (0 == handle)
        {
            free(hEnum);
        }

        return handle;
    }
#endif
    {
        SAPDB_Char * szPath;
        RTE_Path userConfigPath;
        if ( !RTE_GetUserSpecificConfigPath(userConfigPath, true, ErrText) )
        {
            *pOk = SAPDB_INIFILE_RESULT_ERR_OPEN;
            strcpy( (SAPDB_Char *)ErrText, "user configuration path not accessible" );
            free(hEnum);
            return 0;
        }
        szPath = (SAPDB_Char *)alloca(strlen(userConfigPath)+strlen(szFile)+1);
        strcpy(szPath, userConfigPath); strcat(szPath, szFile);

        return OpenConfigFileForEnumeration(hEnum,
                                            szPath,
                                            true,
                                            szSection,
                                            ErrText,
                                            pOk);
    }
}

/*---------------------------------------------------------------------------*/

RTE_RegistryHandle RTE_OpenConfigEnum (
    const SAPDB_Char * szFile,
    const SAPDB_Char * szSection,
    tsp00_ErrTextc ErrText, 
    RTE_IniFileResult  * pOk)
{
    RTE_RegistryHandle hEnum;

    /* Skip INSTALLATIONS and DATABASES search in user specific directory */
    if ( strcmp(szFile, SAPDB_INSTALLATIONS_INI_FILE)
      && strcmp(szFile, SAPDB_DATABASES_INI_FILE) )
    {
        hEnum = RTE_OpenUserConfigEnum(NULL, szFile, szSection, ErrText, pOk);
        if ( SAPDB_INIFILE_RESULT_OK == *pOk )
        {
            hEnum->parseAll = 1;
            return hEnum;
        }
    }

    hEnum = RTE_OpenGlobalConfigEnum (szFile, szSection, ErrText, pOk);
#if !defined(WIN32)
    /* For UNIX another special old registry must be parsed if [Installations], [Databases] or [Runtime] section is searched */
    if ( SAPDB_INIFILE_RESULT_OK == *pOk  || '/' == *szFile )
    {
        hEnum->parseAll = 1;
        return hEnum;
    }

    {
        char *oldPath;
        if ( !strcmp(szFile, SAPDB_RUNTIMES_INI_FILE)
            || !strcmp(szFile, SAPDB_INSTALLATIONS_INI_FILE)
            || !strcmp(szFile, SAPDB_DATABASES_INI_FILE) )
        {
            oldPath = (char *)(SAPDB_OLD_GLOBAL_CONFIG_DIRECTORY "/" SAPDB_OLD_GLOBAL_COMMON_CONFIG_FILE);
        }
        else
        {
            oldPath = (char *)alloca(strlen(SAPDB_OLD_GLOBAL_CONFIG_DIRECTORY "/") + strlen(szFile) + 1);
            strcpy(oldPath, SAPDB_OLD_GLOBAL_CONFIG_DIRECTORY "/"); strcat(oldPath, szFile);
        }

        hEnum = RTE_OpenGlobalConfigEnum ( oldPath,
                                           szSection,
                                           ErrText,
                                           pOk );
    }
#endif

    return hEnum;
}

/*---------------------------------------------------------------------------*/

/*
  Function:     RTE_CloseConfigEnum ()
  description:  Closes registry opened by RTE_OpenConfigFile ()
                
    Close the registry. This invalidates the given handle.
    Subsequent calls to RTE_NextConfigEnum() must fail.
  
  arguments:
    hEnum [in] Handle of registry enumeration
    ErrText [out] Place for descriptive error message
    Ok [out] Error return value

  return value:
    <UL>
        <LI>0: failed ( inspect errno )
        <LI>1: ok
    </UL>
 */
SAPDB_Int4 RTE_CloseConfigEnum (
    RTE_RegistryHandle hEnum,
    tsp00_ErrTextc ErrText,
    RTE_IniFileResult * pOk)
{
    if ( hEnum )
    {
        void *buffer = hEnum->buffer;
#if defined(WIN32)
        if(!buffer)
        {
            SAPDB_Int4      rc;
            rc = RTE_CloseWindowsConfigEnum(hEnum, ErrText, pOk);
            free(hEnum);
            return rc;
        }
#endif

        hEnum->buffer = hEnum->nextEntry = 0;
        free(buffer);
        free(hEnum);

        ErrText[0] = '\0';
        *pOk = SAPDB_INIFILE_RESULT_OK;
        return 1;
    }
    else
    {
        /* nothing to do */
        *pOk = SAPDB_INIFILE_RESULT_OK;
        return 1;
    }
}

/*
  Function:     RTE_NextConfigEnum ()
  description:  Reads next entry from registry opened by RTE_OpenConfigFile ()
                
   Reads next 'szEntry' and 'szString' from section of config file opened by 
   RTE_OpenConfigEnum ().
   Gives back 'SAPDB_INIFILE_RESULT_EOF' on 'Ok' on end of section or end of file.
  
  arguments:
    hEnum [in] Handle of registry file
    szEntry [out] Place to store to key of 'key=value' entry
    MaxEntryLen [in] Maximum key buffer size
    szString [out] Place to store the value of 'key=value' entry
    MaxStringLen [in] Maximum value buffer size
    ErrText [out] Place for descriptive error message
    Ok [out] Error return value

  return value:
    <UL>
        <LI>0: failed ( inspect errno )
        <LI>1: ok
    </UL>
 */
SAPDB_Int4 RTE_NextConfigEnum (
    RTE_RegistryHandle hEnum,
    SAPDB_Char *szEntry,
    const SAPDB_Int4 MaxEntryLen,
    SAPDB_Char *szString,
    const SAPDB_Int4 MaxStringLen,
    RTE_RegistryLocation *pLocation,
    tsp00_ErrTextc ErrText,
    RTE_IniFileResult *pOk)
{
    *pLocation = hEnum->location;

#if defined(WIN32)
    if(!hEnum->buffer)
    {
        if ( RTE_NextWindowsConfigEnum( hEnum,
                                        szEntry,
                                        MaxEntryLen,
                                        szString,
                                        MaxStringLen,
                                        ErrText,
                                        pOk ) )
        {
            return 1;
        }

        if ( SAPDB_INIFILE_RESULT_EOF == *pOk
        && hEnum->parseAll 
        && hEnum->location == RTE_UserRegistry )
        {
            RTE_RegistryHandle nextEnum;
            *pOk = SAPDB_INIFILE_RESULT_OK;
            nextEnum = RTE_OpenGlobalConfigEnum ( hEnum->file,
                                                hEnum->section,
                                                ErrText, 
                                                pOk);

            if ( *pOk == SAPDB_INIFILE_RESULT_OK )
            {
                /* exchange search key */
                HKEY hkOld = hEnum->hkResult;
                hEnum->hkResult = nextEnum->hkResult;
                nextEnum->hkResult = hkOld;
                hEnum->Index = 0;

                /* copy new location */
                hEnum->location = nextEnum->location;
                /* close old key and free new space! */
                RTE_CloseConfigEnum(nextEnum, ErrText, pOk);

                return RTE_NextConfigEnum( hEnum,
                                        szEntry,
                                        MaxEntryLen,
                                        szString,
                                        MaxStringLen,
                                        pLocation,
                                        ErrText,
                                        pOk);
            }
        }
        return 0;
    }
#endif
    if ( RTE_NextUNIXConfigEnum( hEnum,
                                 szEntry,
                                 MaxEntryLen,
                                 szString,
                                 MaxStringLen,
                                 ErrText,
                                 pOk ) )
    {
        return 1;
    }

    if ( SAPDB_INIFILE_RESULT_EOF == *pOk
      && hEnum->parseAll 
      && hEnum->location != RTE_OldUNIXRegistry )
    {
        RTE_RegistryHandle nextEnum;

#ifndef _WIN32
        if ( hEnum->location == RTE_GlobalRegistry )
        {
            char *oldPath;
            if ( !strcmp(hEnum->file, SAPDB_RUNTIMES_INI_FILE)
                || !strcmp(hEnum->file, SAPDB_INSTALLATIONS_INI_FILE)
                || !strcmp(hEnum->file, SAPDB_DATABASES_INI_FILE) )
            {
                oldPath = (char *)(SAPDB_OLD_GLOBAL_CONFIG_DIRECTORY "/" SAPDB_OLD_GLOBAL_COMMON_CONFIG_FILE);
            }
            else
            {
                oldPath = (char *)alloca(strlen(SAPDB_OLD_GLOBAL_CONFIG_DIRECTORY "/") + strlen(hEnum->file) + 1);
                strcpy(oldPath, SAPDB_OLD_GLOBAL_CONFIG_DIRECTORY "/"); strcat(oldPath, hEnum->file);
            }

            nextEnum = RTE_OpenGlobalConfigEnum ( oldPath,
                                                  hEnum->section,
                                                  ErrText,
                                                  pOk );
        }
        else
#endif
        {
            nextEnum = RTE_OpenGlobalConfigEnum ( hEnum->file,
                                                  hEnum->section,
                                                  ErrText,
                                                  pOk);
        }

        if ( *pOk == SAPDB_INIFILE_RESULT_OK )
        {
            /* exchange buffer pointer and copy nextEntry pointer */
            SAPDB_Char *oldBuffer = hEnum->buffer;

            hEnum->buffer = nextEnum->buffer;
            hEnum->nextEntry = nextEnum->nextEntry;

            nextEnum->buffer = oldBuffer;

            /* copy new location */
            hEnum->location = nextEnum->location;

            /* close old key and free new space! */
            RTE_CloseConfigEnum(nextEnum, ErrText, pOk);

            return RTE_NextConfigEnum( hEnum,
                                       szEntry,
                                       MaxEntryLen,
                                       szString,
                                       MaxStringLen,
                                       pLocation,
                                       ErrText,
                                       pOk);
        }
        else
        {
            strcpy( ErrText, "End of Registry" );
            *pOk = SAPDB_INIFILE_RESULT_EOF;
        }
    }
    return 0;
}

/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/* Obsolet, use RTE_GetUserSpecificApplicationDataPath instead (see below)    */
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

/* ------------------------------------------------------------------
 * PUBLIC FUNCTION RTE_GetUserSpecificConfigPath 
 * ------------------------------------------------------------------
 * purpose:   Gets the user specific paths for configuration data 'Application Data'.
 *
 * If the environment variable "SDB_HOMEDIRECTORY" is found and a directory with such name exists 
 *     is accessible, it is used (UNIX only). If it is not accessible, the environment variable is ignored.
 * Otherwise if a home directory exists the subdirectory ".sdb/<machine>" in the home 
 *     directory is used as path for the 'Application Data' (UNIX only). 
 *     If such a subdirectory exists or can be created and is accessible, it is used.
 *     On Windows the "<IndependantDataPath>\wrk" is returned.
 * NOTE If the directory does not exist this call will try to create it.
 *     On UNIX it does this with mode 0777, so use the current 'umask' setting
 *     if further restriction is wanted. Default should be umask 022.
 *     On Windows the CreateDirectory() with the default security descriptor
 *     NULL is used.
 *
 *   ConfigPath               - work path to the actual SAPDB for R/3 release 
 *                              WrkPath is a C ( null terminated ) string
 *   TerminateWithDelimiter   - TERM_WITHOUT_DELIMITER_EO01 
 *                            - TERM_WITH_DELIMITER_EO01 
 *   ErrText                  - Error message if any
 * return - true if call succeeded, false else (ErrText filled)
 */
SAPDB_Bool RTE_GetUserSpecificConfigPath ( RTE_Path           ConfigPath,
                                           SAPDB_Bool         TerminateWithDelimiter,
                                           tsp00_ErrTextc     ErrText )
{
#if defined(WIN32)
  if (!RTE_GetCommonDataPath(ConfigPath, true, ErrText))
    return false;

  if(strlen(ConfigPath) + 3 > sizeof(RTE_Path)) {
    strcpy((SAPDB_Char *)ErrText, "Path to <indep data>/wrk too long");
    return false;        
  }
  strcat(ConfigPath, "wrk");
  if (!TrimDelimiter(ConfigPath, TerminateWithDelimiter)) {
    strcpy((SAPDB_Char *)ErrText, "Path to <indep data>/wrk too long");
    return false;        
  } 
  return true;
#else
  return RTE_GetUserSpecificApplicationDataPath (ConfigPath, false, TerminateWithDelimiter, ErrText);
#endif
}


/* ------------------------------------------------------------------
 * PUBLIC FUNCTION RTE_GetUserSpecificApplicationDataPath
 * ------------------------------------------------------------------
 * purpose:   Gets the user specific path for the local or global application data (e.g. configuration data).
 *
 * If the environment variable "SDB_HOMEDIRECTORY" for the local application data path or 
 *     "SDB_GLOBAL_APPLICATION_DATA" for global application data is found and a directory 
 *     with such name is accessible, it is used (UNIX only). 
 *     If it is not accessible, the call fails.
 * Otherwise if a home directory exists the subdirectory "$HOME/.sdb/<machine>" or "$HOME/.sdb" 
 *     on UNIX or "C:\Documents and Settings\<user id>\Local Settings\Application Data\sdb" or 
 *     "C:\Documents and Settings\<user id>\Application Data\sdb" on Windows is used as path 
 *     for the local or global 'Application Data' respectively. 
 * NOTE If the directory does not exist this call will try to create it.
 *     On UNIX it does this with mode 0777, so use the current 'umask' setting
 *     if further restriction is wanted. Default should be umask 022.
 *     On Windows the CreateDirectory() with the default security descriptor
 *     NULL is used.
 *
 *   applicationDataPath (out)      - Application Data path 
 *                                    applicationDataPath is a C ( null terminated ) string
 *   roaming (in)                   - true: Data path for roaming applications is wanted
 *                                  - false: Data path for local (nonroaming) applications is wanted
 *   TerminateWithDelimiter (in)    - TERM_WITHOUT_DELIMITER_EO01 
 *                                  - TERM_WITH_DELIMITER_EO01 
 *   ErrText (out)                  - Error message if any
 * return - true if call succeeded, false else (ErrText filled)
 */
SAPDB_Bool RTE_GetUserSpecificApplicationDataPath   (RTE_Path             applicationDataPath,
                                                     SAPDB_Bool           roaming,
                                                     SAPDB_Bool           terminateWithDelimiter,
                                                     tsp00_ErrTextc       errTextc)
{
#if defined(WIN32)

    HRESULT     rc;
    int         folder;

    folder = roaming ? CSIDL_APPDATA : CSIDL_LOCAL_APPDATA;

    rc = SHGetFolderPath(NULL, CSIDL_FLAG_CREATE | folder, NULL, SHGFP_TYPE_CURRENT, applicationDataPath);
    if (S_OK != rc)
    {
        eo46BuildErrorStringRC (errTextc, ERRMSG_SYS_GET_APPL_DATA_PATH, rc);
        return false;
    }

    if (strlen (applicationDataPath) + strlen ("\\" SAPDB_HOME_SUBDIRECTORY) >= sizeof(RTE_Path))
    {
        eo46BuildErrorStringRC (errTextc, ERRMSG_SYS_APPL_DATA_PATH_LENGTH, 0);
        return false;
    }

    strcat (applicationDataPath, "\\" SAPDB_HOME_SUBDIRECTORY);

    if (!TrimDelimiter(applicationDataPath, terminateWithDelimiter)) 
    {
        eo46BuildErrorStringRC (errTextc, ERRMSG_SYS_APPL_DATA_PATH_LENGTH, 0);
        return false;        
    } 

    if (!CreateDirectory(applicationDataPath, 0 )) 
    {
        rc = GetLastError();

        if ( ERROR_ALREADY_EXISTS != rc )
        {
            eo46BuildErrorStringRC (errTextc, roaming ? ERRMSG_SYS_CREATE_USER_APPL_DATA_PATH 
                                                      : ERRMSG_SYS_CREATE_USER_LOCAL_APPL_DATA_PATH, rc );
            return false;
        }
    } 

    return true;
#else
    return RTE_GetUserSpecificPath (SAPDB_APPLICATION_DATA_SUBDIRECTORY, 
                                    roaming,
                                    applicationDataPath, 
                                    terminateWithDelimiter, 
                                    errTextc);
#endif
}

#if defined(WIN32)
/* ------------------------------------------------------------------
 * PUBLIC FUNCTION RTE_GetCommonApplicationDataPath (Windows only)
 * ------------------------------------------------------------------
 * purpose:   Gets the common path for the application data.
 *
 *     "C:\Documents and Settings\All Users\Application Data\sdb" on Windows is used as path 
 *     for the 'Application Data'. 
 *
 *     NOTE If the directory does not exist this call will try to create it.
 *
 *   applicationDataPath    (out)   - Application Data path 
 *                                    applicationDataPath is a C ( null terminated ) string
 *   terminateWithDelimiter (in)    - TERM_WITHOUT_DELIMITER_EO01 
 *                                  - TERM_WITH_DELIMITER_EO01 
 *   errText                (out)   - Error message if any
 * return - true if call succeeded, false else (ErrText filled)
 */
SAPDB_Bool RTE_GetCommonApplicationDataPath   ( RTE_Path             applicationDataPath,
                                                SAPDB_Bool           terminateWithDelimiter,
                                                tsp00_ErrTextc       errTextc )
{
    HRESULT     rc;
    int         folder = CSIDL_COMMON_APPDATA;

    rc = SHGetFolderPath( NULL, CSIDL_FLAG_CREATE | folder, NULL, SHGFP_TYPE_CURRENT, applicationDataPath );

    if ( S_OK != rc )
    {
        eo46BuildErrorStringRC (errTextc, ERRMSG_SYS_GET_COMMON_APPL_DATA_PATH, rc);
        return false;
    }

    if ( strlen (applicationDataPath) + strlen ("\\" SAPDB_HOME_SUBDIRECTORY) >= sizeof(RTE_Path) )
    {
        eo46BuildErrorStringRC (errTextc, ERRMSG_SYS_COMMON_APPL_DATA_PATH_LENGTH, 0);
        return false;
    }

    strcat (applicationDataPath, "\\" SAPDB_HOME_SUBDIRECTORY);

    if (!TrimDelimiter(applicationDataPath, terminateWithDelimiter)) 
    {
        eo46BuildErrorStringRC (errTextc, ERRMSG_SYS_COMMON_APPL_DATA_PATH_LENGTH, 0);
        return false;        
    } 

    if (!CreateDirectory(applicationDataPath, 0 )) 
    {
        rc = GetLastError();

        if ( ERROR_ALREADY_EXISTS != rc )
        {
            eo46BuildErrorStringRC (errTextc, ERRMSG_SYS_CREATE_COMMON_APPL_DATA_PATH, rc );
            return false;
        }
    } 
    return true;
}
#endif


/* ------------------------------------------------------------------
 * PUBLIC FUNCTION RTE_GetMyDocumentsPath
 * ------------------------------------------------------------------
 * purpose:   Gets the user specific path for application documents (e.g. import or export data).
 *
 * If the environment variable "SDB_MY_DOCUMENTS" is found and a directory with such name exists 
 *     is accessible, it is used (UNIX only). 
 *     If it is not accessible, the environment variable is ignored.
 * Otherwise if a home directory exists the subdirectory "$HOME/sdb/<machine>" on UNIX or 
 *     "C:\Documents and Settings\<user id>\My Documents\sdb" on Windows is used as path 
 *     for 'My Documents'. 
 *     If such a subdirectory exists or can be created and is accessible, it is used.
  * NOTE If the directory does not exist this call will try to create it (UNIX only).
 *     On UNIX it does this with mode 0777, so use the current 'umask' setting
 *     if further restriction is wanted. Default should be umask 022.
 *     On Windows the CreateDirectory() with the default security descriptor
 *     NULL is used.
 *
 *   myDocumentsPath (out)          - My Ducuments path 
 *                                    applicationDataPath is a C ( null terminated ) string
 *   TerminateWithDelimiter (in)    - TERM_WITHOUT_DELIMITER_EO01 
 *                                  - TERM_WITH_DELIMITER_EO01 
 *   ErrText (out)                  - Error message if any
 * return - true if call succeeded, false else (ErrText filled)
 */

SAPDB_Bool RTE_GetMyDocumentsPath                   (RTE_Path             myDocumentsPath,
                                                     SAPDB_Bool           terminateWithDelimiter,
                                                     tsp00_ErrTextc       errTextc)
{
#if defined(WIN32)

    HRESULT     rc;
    /*
        CAUTION
        Microsoft gives confusing desriptions about the determination of the physical 'My Documents' 
        folder.
        They say that from version 6.0 on of the shell32.dll the following should not work anymore.
        But the reality shows the opposite. Of course the future behavior is unsure. 
    */
    rc = SHGetFolderPath(NULL, CSIDL_FLAG_CREATE | CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, myDocumentsPath);
    if (S_OK != rc)
    {
        eo46BuildErrorStringRC (errTextc, ERRMSG_SYS_GET_MY_DOCUMENTS_PATH, rc);
        return false;
    }

    if (strlen (myDocumentsPath) + strlen ("\\" SAPDB_HOME_SUBDIRECTORY) >= sizeof(RTE_Path))
    {
        eo46BuildErrorStringRC (errTextc, ERRMSG_SYS_MY_DOCUMENTS_PATH_LENGTH, 0);
        return false;
    }

    strcat (myDocumentsPath, "\\" SAPDB_HOME_SUBDIRECTORY);

    if (!TrimDelimiter(myDocumentsPath, terminateWithDelimiter)) 
    {
        eo46BuildErrorStringRC (errTextc, ERRMSG_SYS_MY_DOCUMENTS_PATH_LENGTH, 0);
        return false;        
    } 

    if (!CreateDirectory(myDocumentsPath, 0 )) 
    {
        rc = GetLastError();

        if ( ERROR_ALREADY_EXISTS != rc )
        {
            eo46BuildErrorStringRC (errTextc, ERRMSG_SYS_CREATE_MY_DOCUMENT_PATH, rc);
            return false;
        }
    } 

    return true;
#else
    return RTE_GetUserSpecificPath (SAPDB_HOME_SUBDIRECTORY, 
                                    false,
                                    myDocumentsPath, 
                                    terminateWithDelimiter, 
                                    errTextc);
#endif
}

/*------------------------------------------------------------------------------------------*/

/*
  Get Common Independend Config Path
  @param ConfigPath [out] the path found as IndepData=path entry in /etc/opt/sapdb appened with /config
  @param TerminateWithDelimiter [in] forces to terminate path with path delimiter '/'
  @param ErrText [out] in case of problems filled with error text
  @return true if call succeeded, false if call failed
 */
SAPDB_Bool RTE_GetCommonConfigPath ( RTE_Path           ConfigPath,
                                     SAPDB_Bool         TerminateWithDelimiter,
                                     tsp00_ErrTextc ErrText )
{

    RTE_IniFileResult Ok;
    SAPDB_Int4 pathLen = 0;
#ifdef _WIN32
    tsp00_Pathc u3Root;
    tsp01_RteError  rteError; 
    if(!sqlGetPortableInstallationRootPath( u3Root,
                                        TERM_WITH_DELIMITER_EO01,
                                        &rteError ))
    {
        return false;
    }
    SAPDB_strncpy_zero_terminated(&ConfigPath[0],u3Root,sizeof(RTE_Path));
    pathLen = strlen(&ConfigPath[0]);
#endif
    pathLen += RTE_GetInstallationConfigString( SAPDB_INDEPDATA_KEY,
                                                          &ConfigPath[pathLen],
                                                          (const SAPDB_Int4)sizeof(RTE_Path)-pathLen,
                                                          ErrText,
                                                          &Ok );
    if ( Ok == SAPDB_INIFILE_RESULT_OK )
    {
        if ( !TrimDelimiter(&ConfigPath[0], true) )
        {
            strcpy((SAPDB_Char *)ErrText, "Independend Data Path too long");
            return false;
        }

        if ( (pathLen + strlen(SAPDB_CONFIG_SUBPATH)) > sizeof(RTE_Path) )
        {
            strcpy((SAPDB_Char *)ErrText, "Independend Config Path too long");
            return false;
        }

        strcat(&ConfigPath[0], SAPDB_CONFIG_SUBPATH);
        if ( !TrimDelimiter(&ConfigPath[0], TerminateWithDelimiter) )
        {
            strcpy((SAPDB_Char *)ErrText, "Independend Config Path just too long");
            return false;
        }

        return true;
    }
    else
    {
        return false;
    }
}

/*
  Get Common Independend Data Path
  @param ConfigPath [out] the path found as IndepData=path entry in /etc/opt/sapdb appened with /config
  @param TerminateWithDelimiter [in] forces to terminate path with path delimiter '/'
  @param ErrText [out] in case of problems filled with error text
  @return true if call succeeded, false if call failed
 */
SAPDB_Bool RTE_GetCommonDataPath ( RTE_Path           ConfigPath,
                                   SAPDB_Bool         TerminateWithDelimiter,
                                   tsp00_ErrTextc ErrText )
{
    RTE_IniFileResult Ok;
    SAPDB_Int4 pathLen = RTE_GetInstallationConfigString( SAPDB_INDEPDATA_KEY,
                                                          &ConfigPath[0],
                                                          (const SAPDB_Int4)sizeof(RTE_Path),
                                                          ErrText,
                                                          &Ok );
    if ( Ok == SAPDB_INIFILE_RESULT_OK )
    {
        if ( !TrimDelimiter(&ConfigPath[0], TerminateWithDelimiter) )
        {
            strcpy((SAPDB_Char *)ErrText, "Independend Data Path too long");
            return false;
        }

        return true;
    }
    return false;
}

/*---*/

SAPDB_Int4 RTE_GetSapdbOwner ( SAPDB_Char * szOwner,
                               const SAPDB_Int4       MaxStringLen,
                               tsp00_ErrTextc   ErrText,
                               RTE_IniFileResult * pOk )
{
    return RTE_GetInstallationConfigString (
                        SAPDB_SAPDBOWNER_KEY,
                        szOwner,
                        MaxStringLen,
                        ErrText,
                        pOk );
}

/*---*/

SAPDB_Int4 RTE_GetSapdbGroup ( SAPDB_Char * szOwner,
                               const SAPDB_Int4       MaxStringLen,
                               tsp00_ErrTextc   ErrText,
                               RTE_IniFileResult * pOk )
{
    return RTE_GetInstallationConfigString (
                        SAPDB_SAPDBGROUP_KEY,
                        szOwner,
                        MaxStringLen,
                        ErrText,
                        pOk );
}

#if !defined(WIN32)

#define SDB_FILE_ENTRY_LIMIT (1024*1024*16)
#define SDB_FILE_ENTRY_FIRST_SIZE 4096

/*-----------------------------------*/
/* UNIX passwd and group file access */
/*-----------------------------------*/

SAPDB_Bool RTE_GetUserIdFromPasswdByName( SAPDB_Char const * userName
                                        , SAPDB_Int4       *pUserId  )
{
    char *tmpBuffer;
    long tmpBufferLength = SDB_FILE_ENTRY_FIRST_SIZE;
    struct passwd  Entry;
    struct passwd *pwEntry = 0;

    do
    {
        tmpBuffer = (char *)malloc(tmpBufferLength);
        if ( !tmpBuffer )
        {
            break;
        }

        if ( getpwnam_r( userName
                       , &Entry
                       , tmpBuffer
                       , tmpBufferLength
                       , &pwEntry 
                       ) == 0 )
        {
            break;
        }

        free(tmpBuffer);
        tmpBufferLength *= 2;
        tmpBuffer = 0;
        pwEntry = 0;

    } while ( tmpBufferLength < SDB_FILE_ENTRY_LIMIT );

    if ( 0 != pwEntry )
    {
        *pUserId = (SAPDB_Int4)pwEntry->pw_uid;
    }

    free(tmpBuffer);

    return ( 0 != pwEntry );
}

/*--------*/

SAPDB_Bool RTE_GetSapdbOwnerUserId(SAPDB_Int4 *pUserId)
{
    static SAPDB_Int4 ownerUserId = -1;
    SAPDB_Char OwnernameBuffer[256];
    RTE_IniFileResult  ok;
    tsp00_ErrTextc errtext;

    if ( -1 == ownerUserId )
    {
        RTE_GetInstallationConfigString( SAPDB_SAPDBOWNER_KEY, 
                                         OwnernameBuffer, 
                                         (SAPDB_Int4)sizeof(OwnernameBuffer),
                                         errtext,
                                         &ok );
        if ( SAPDB_INIFILE_RESULT_OK != ok)
        {
            return false;
        }
        if ( !RTE_GetUserIdFromPasswdByName(&OwnernameBuffer[0], &ownerUserId) )
        {
            return false;
        }
    }
    *pUserId = ownerUserId;
    return true;
}

/*--------*/

SAPDB_Bool RTE_GetUserAndGroupIdFromPasswdByName( SAPDB_Char const * userName
                                                , SAPDB_Int4        *pUserId
                                                , SAPDB_Int4        *pGroupId )
{
    char *tmpBuffer;
    long tmpBufferLength = SDB_FILE_ENTRY_FIRST_SIZE;
    struct passwd  Entry;
    struct passwd *pwEntry = 0;

    do
    {
        tmpBuffer = (char *)malloc(tmpBufferLength);
        if ( !tmpBuffer )
        {
            break;
        }

        if ( getpwnam_r( userName
                       , &Entry
                       , tmpBuffer
                       , tmpBufferLength
                       , &pwEntry 
                       ) == 0 )
        {
            break;
        }

        free(tmpBuffer);
        tmpBufferLength *= 2;
        tmpBuffer = 0;
        pwEntry = 0;

    } while ( tmpBufferLength < SDB_FILE_ENTRY_LIMIT );

    if ( 0 != pwEntry )
    {
        *pUserId = (SAPDB_Int4)pwEntry->pw_uid;
        *pGroupId = (SAPDB_Int4)pwEntry->pw_gid;
    }

    free(tmpBuffer);

    return ( 0 != pwEntry );
}

/*--------*/

SAPDB_Bool RTE_GetUsernameFromPasswdById( SAPDB_Int4 const userId
                                        , SAPDB_Char  *    userNameBuffer
                                        , SAPDB_UInt4      userNameBufferSize
                                        , SAPDB_UInt4     *pNeededSize)
{
    char *tmpBuffer;
    long tmpBufferLength = SDB_FILE_ENTRY_FIRST_SIZE;
    struct passwd  Entry;
    struct passwd *pwEntry = 0;

    do
    {
        tmpBuffer = (char *)malloc(tmpBufferLength);
        if ( !tmpBuffer )
        {
            break;
        }

        if ( getpwuid_r( (uid_t)userId
                       , &Entry
                       , tmpBuffer
                       , tmpBufferLength
                       , &pwEntry 
                       ) == 0 )
        {
            break;
        }

        free(tmpBuffer);
        tmpBufferLength *= 2;
        tmpBuffer = 0;
        pwEntry = 0;

    } while ( tmpBufferLength < SDB_FILE_ENTRY_LIMIT );

    if ( 0 == pwEntry )
    {
        *pNeededSize = 0;
    }
    else
    {
        *pNeededSize = SAPDB_strlen(pwEntry->pw_name) + 1;
        if ( (*pNeededSize) > userNameBufferSize )
        {
            SAPDB_memcpy(userNameBuffer, pwEntry->pw_name, userNameBufferSize - 1);
            userNameBuffer[userNameBufferSize - 1] = 0;
            pwEntry = 0;
        }
        else
        {
            SAPDB_memcpy(userNameBuffer, pwEntry->pw_name, *pNeededSize);
        }
    }

    free(tmpBuffer);

    return ( 0 != pwEntry );
}

/*--------*/

SAPDB_Bool RTE_GetHomeDirectoryFromPasswdById( SAPDB_Int4 const userId
                                             , SAPDB_Char  *    pathBuffer
                                             , SAPDB_UInt4      pathBufferSize
                                             , SAPDB_UInt4     *pNeededSize)
{
    char *tmpBuffer;
    long tmpBufferLength = SDB_FILE_ENTRY_FIRST_SIZE;
    struct passwd  Entry;
    struct passwd *pwEntry = 0;

    do
    {
        tmpBuffer = (char *)malloc(tmpBufferLength);
        if ( !tmpBuffer )
        {
            break;
        }

        if ( getpwuid_r( (uid_t)userId
                       , &Entry
                       , tmpBuffer
                       , tmpBufferLength
                       , &pwEntry 
                       ) == 0 )
        {
            break;
        }

        free(tmpBuffer);
        tmpBufferLength *= 2;
        tmpBuffer = 0;
        pwEntry = 0;

    } while ( tmpBufferLength < SDB_FILE_ENTRY_LIMIT );

    if ( 0 == pwEntry )
    {
        *pNeededSize = 0;
    }
    else
    {
        *pNeededSize = SAPDB_strlen(pwEntry->pw_dir) + 1;
        if ( (*pNeededSize) > pathBufferSize )
        {
            SAPDB_memcpy(pathBuffer, pwEntry->pw_dir, pathBufferSize - 1);
            pathBuffer[pathBufferSize - 1] = 0;
            pwEntry = 0;
        }
        else
        {
            SAPDB_memcpy(pathBuffer, pwEntry->pw_dir, *pNeededSize);
        }
    }

    free(tmpBuffer);

    return ( 0 != pwEntry );
}

/*--------*/

SAPDB_Bool RTE_GetGroupIdFromGroupByName( SAPDB_Char const * groupName
                                        , SAPDB_Int4       * pGroupId )
{
    char *tmpBuffer;
    long tmpBufferLength = SDB_FILE_ENTRY_FIRST_SIZE;
    struct group  Entry;
    struct group *grEntry = 0;

    do
    {
        tmpBuffer = (char *)malloc(tmpBufferLength);
        if ( !tmpBuffer )
        {
            return false;
        }

        if ( getgrnam_r( groupName
                       , &Entry
                       , tmpBuffer
                       , tmpBufferLength
                       , &grEntry 
                       ) == 0 )
        {
            break;
        }
        free(tmpBuffer);
        tmpBufferLength *= 2;
        tmpBuffer = 0;
        grEntry = 0;

    } while ( tmpBufferLength < SDB_FILE_ENTRY_LIMIT );

    if ( 0 != grEntry )
    {
        *pGroupId = (SAPDB_Int4)grEntry->gr_gid;
    }

    free(tmpBuffer);

    return ( 0 != grEntry );
}

/*--------*/

SAPDB_Bool RTE_GetGroupNameFromGroupById( SAPDB_Int4 const groupId
                                                 , SAPDB_Char  *    groupNameBuffer
                                                 , SAPDB_UInt4      groupNameBufferSize
                                                 , SAPDB_UInt4     *pNeededSize)
{
    char *tmpBuffer;
    long tmpBufferLength = SDB_FILE_ENTRY_FIRST_SIZE;
    struct group  Entry;
    struct group *grEntry = 0;

    do 
    {
        tmpBuffer = (char *)malloc(tmpBufferLength);
        if ( !tmpBuffer )
        {
            return false;
        }

        if ( getgrgid_r( (gid_t)groupId
                       , &Entry
                       , tmpBuffer
                       , tmpBufferLength
                       , &grEntry 
                       ) == 0 )
        {
            break;
        }
        free(tmpBuffer);
        tmpBufferLength *= 2;
        tmpBuffer = 0;
        grEntry = 0;
    } while ( tmpBufferLength < SDB_FILE_ENTRY_LIMIT );

    if ( 0 == grEntry )
    {
        *pNeededSize = 0;
    }
    else
    {
        *pNeededSize = SAPDB_strlen(grEntry->gr_name) + 1;
        if ( (*pNeededSize) > groupNameBufferSize )
        {
            SAPDB_memcpy(groupNameBuffer, grEntry->gr_name, groupNameBufferSize - 1);
            groupNameBuffer[groupNameBufferSize - 1] = 0;
            grEntry = 0;
        }
        else
        {
            SAPDB_memcpy(groupNameBuffer, grEntry->gr_name, *pNeededSize);
        }
    }

    free(tmpBuffer);

    return ( 0 != grEntry );
}

/*--------*/

SAPDB_Bool RTE_SearchUserNameInGroupMemberList( SAPDB_Char const *userName
                                                         , SAPDB_Int4 primaryGroupId
                                                         , SAPDB_Char const *groupName
                                                         , SAPDB_Bool *pGroupFound )
{
    char *tmpBuffer;
    long tmpBufferLength = SDB_FILE_ENTRY_FIRST_SIZE;
    struct group  Entry;
    struct group *grEntry = 0;
    SAPDB_Int iCheck;
    SAPDB_Char **memberList;

    do
    {
        tmpBuffer = (char *)malloc(tmpBufferLength);
        if ( !tmpBuffer )
        {
            break;
        }

        if ( getgrnam_r( groupName
                       , &Entry
                       , tmpBuffer
                       , tmpBufferLength
                       , &grEntry 
                       ) == 0 )
        {
            break;
        }
        free(tmpBuffer);
        tmpBufferLength *= 2;
        tmpBuffer = 0;
        grEntry = 0;

    } while ( tmpBufferLength < SDB_FILE_ENTRY_LIMIT );

    *pGroupFound = ( 0 != grEntry );
    if ( !*pGroupFound )
    {
        if ( tmpBuffer )
        {
            free(tmpBuffer);
        }
        return false;
    }

    /* If primary group matches, user is member of that group */
    if ( primaryGroupId == grEntry->gr_gid )
    {
        free(tmpBuffer);
        return true;
    }

    /* otherwise he must be explicitly listed in member list */
    for ( memberList = grEntry->gr_mem; memberList && *memberList; ++memberList )
    {
        for ( iCheck = 0; userName[iCheck] != 0; ++iCheck )
        {
            if ( toupper(userName[iCheck]) != toupper((*memberList)[iCheck]) )
            {
                break;
            }
        }

        if ( toupper(userName[iCheck]) == toupper((*memberList)[iCheck]) )
        {
            free(tmpBuffer);

            return true; /* ok we found him */
        }

    }

    free(tmpBuffer);

    return false;
}

#endif

/*===========================================================================*
 *  LOCAL FUNCTIONS (CODE)                                                   *
 *===========================================================================*/

#if 1 //defined SUN || defined SOLARIS || defined AIX || defined OSF1 || defined NMP || defined HPUX || defined(LINUX)
/*----------------------------*/
/*  UNIX only static routines */
/*----------------------------*/

/*
  Function:     ReadLine ()
  description:  Read from given filedescriptor until EOL('\n') or EOF
  
    Puts the next line as a zero terminated string to szBuffer.
    Any nonprintable characters are ignored (this works on dos files too).
    If given buffer space is not sufficient, 'MoreData' returns true.

    An empty line read is returned as a Buffer with first character set to 0, MoreData is false.

    If MoreData flag is set, the outer function must loop to read the rest of the line. Since buffer
    is always 0 terminated, it must use 'BufSize-1' bytes of each call to ReadLine().

    Single character I/O is used since no buffering is used.

  arguments:
    Fd [in] Opened Filedescriptor
    pBuffer [out] Pointer to Buffer space
    BufSize[in] Size of Bufferspace
    MoreData [out] Flag to indicate more data follows

  return value:
    <UL>
        <LI>0: EOL or EOF already reached (Buffer not modified)
        <LI>-1: Read error
        <LI>1: Buffer content filled
    </UL>
 */
static SAPDB_Int4 ReadLine (
                     SAPDB_Int4    Fd,
                     SAPDB_Char * pBuffer,
                     SAPDB_Int4    BufSize,
                     SAPDB_Bool * pMoreData )
{
#undef MF__
#define MF__ MOD__"ReadLine"
  SAPDB_Char Ch;
  SAPDB_Int4 Val;
  SAPDB_Char *ptr;
  SAPDB_Int4 ReadLen ;
  SAPDB_Int4 ReadResult ;

  *pMoreData = true;
  ptr = pBuffer;
  for (ReadLen=0; ReadLen<(BufSize-1); ReadLen++ ) 
  {
    Val = read (Fd, &Ch, 1);
    if (Val == 0)
    {
      *pMoreData = false;
      if (ptr > pBuffer)
      {
        break;
      }
      else
      {
        return 0;
      }
    }

    if ( Val != 1 )
    {
      *pMoreData = false;
      *ptr = 0;
      return -1;
    }

    /* end of line */
    if (Ch == '\n')
    {
      *pMoreData = false;
      break;
    }
    /* ignore any other nonprintable character */
    if (Ch < ' ')
    {
      ReadLen--;
      continue;
    }
    *ptr++ = Ch;
  }
  *ptr = '\0';
  return 1;
}


/*
  Function:     CopyLine ()
  description:  Copy from given buffer until EOL('\n') or '\0'
  
    Puts the next line as a zero terminated string to szBuffer.
    Any nonprintable characters are ignored (this works on dos files too).
    If given buffer space is not sufficient, 'MoreData' returns true.

    An empty line read is returned as a Buffer with first character set to 0, MoreData is false.

    If MoreData flag is set, the outer function must loop to read the rest of the line. Since buffer
    is always 0 terminated, it must use 'BufSize-1' bytes of each call to ReadLine().

  arguments:
    ppNextEntry [inout] pointer to next entry in buffer to copy from (updated)
    pBuffer   [out] Pointer to Buffer space
    BufSize   [in]  Size of Bufferspace
    MoreData  [out] Flag to indicate more data follows

  return value:
    <UL>
        <LI>0: EOL or '\0' already reached (Buffer not modified)
        <LI>-1: Read error
        <LI>1: Buffer content filled
    </UL>
 */
static SAPDB_Int4 CopyLine (
                     SAPDB_Char **ppNextEntry,
                     SAPDB_Char * pBuffer,
                     SAPDB_Int4    BufSize,
                     SAPDB_Bool * pMoreData )
{
#undef MF__
#define MF__ MOD__"CopyLine"
  SAPDB_Char Ch;
  SAPDB_Char *ptr;
  SAPDB_Int4 ReadLen ;
  SAPDB_Int4 ReadResult ;

  *pMoreData = true;
  ptr = pBuffer;
  for (ReadLen=0; ReadLen<(BufSize-1); ReadLen++ ) 
  {
    Ch = **ppNextEntry;
    if ( '\0' == Ch)
    {
      *pMoreData = false;
      if (ptr > pBuffer)
      {
        break;
      }
      else
      {
        return 0;
      }
    }

    /* if not already at end of buffer, adjust next entry pointer */
    ++(*ppNextEntry);

    /* end of line */
    if ( '\n' == Ch )
    {
      *pMoreData = false;
      break;
    }
    /* ignore any other nonprintable character */
    if (Ch < ' ')
    {
      ReadLen--;
      continue;
    }
    *ptr++ = Ch;
  }
  *ptr = '\0';
  return 1;
}

/*---------------------------------------------------------------------------*/

static SAPDB_Char *CopySection(SAPDB_Int4 Fd)
{
    SAPDB_Char *buffer;
    SAPDB_Char *readBuffer;
    SAPDB_Int8  bufferSize;
    SAPDB_Int8  bytesCopied;
    SAPDB_Int8  currentPosition;
    SAPDB_Int8  endPosition;
    SAPDB_Int8  tmpPosition;

    do {
        currentPosition = (SAPDB_Int8)lseek(Fd, (off_t)0, SEEK_CUR );
    } while ( currentPosition == (SAPDB_Int8)-1 && errno == EINTR);
    if ( (SAPDB_Int8)-1 == currentPosition ) return 0;

    do {
        endPosition = (SAPDB_Int8)lseek(Fd, (off_t)0, SEEK_END);
    } while ( endPosition == (SAPDB_Int8)-1 && errno == EINTR);
    if ( (SAPDB_Int8)-1 == endPosition ) return 0;

    do {
        tmpPosition = (SAPDB_Int8)lseek(Fd, (off_t)currentPosition, SEEK_SET);
    } while ( tmpPosition == (SAPDB_Int8)-1 && errno == EINTR);
    if ( currentPosition != tmpPosition ) return 0;

    bufferSize = (endPosition - currentPosition) + 2; /* delta + null byte */
    if ( bufferSize < 1 || (bufferSize != ((SAPDB_Int4)bufferSize)) )
    {   /* strange buffer size... */
        return 0;
    }

    buffer = (SAPDB_Char *)calloc(1, bufferSize);
    readBuffer = buffer;
    
    do {
        bytesCopied = (SAPDB_Int8)read(Fd, readBuffer, bufferSize - 1);
        if ( bytesCopied > 0 )
        {
            bufferSize -= bytesCopied;
            readBuffer += bytesCopied;
        }
    } while ( ( (bytesCopied > 0) && (bufferSize > 1) )
           || ( bytesCopied < 0 && errno == EINTR ) );

    if ( bytesCopied < 0 )
    {
        free(buffer);
        return 0;
    }

    return buffer;
}

/*---------------------------------------------------------------------------*/

/*
  Function:     FindSection ()
  description:  Read from given filedescriptor until given section is reached
  
   Next SAPDB_Char to read is the first SAPDB_Char of the first line of this section

  arguments:
    Fd [in] Opened Filedescriptor
    szSection [in] Given Section Identifier

  return value:
    <UL>
        <LI>0: EOF reached (Section not found)
        <LI>1: Section found
        <LI>-1: Read error
    </UL>
 */
static SAPDB_Int4 FindSection (
                        SAPDB_Int4          Fd,
                        const SAPDB_Char * szSection)
{
#undef MF__
#define MF__ MOD__"FindSection"
  SAPDB_Char *szTempSection = (SAPDB_Char *)alloca(strlen(szSection) + strlen("[]") + 1);
  
  SAPDB_Char *Buffer = (SAPDB_Char *)alloca(LINE_SIZE_MAX);
  SAPDB_Int4   BufferSize = LINE_SIZE_MAX;
  SAPDB_Int4   OldBufferSize = 0;
  SAPDB_Bool  MoreData;
  SAPDB_Int4   ReadResult;

#ifdef _WIN32
  if( 0 == strcmp(szSection,SAPDB_GLOBALS_SECTION) )    // on windows there is a difference between globals section in registry and globals section in files...
  {
    szSection = SAPDB_GLOBALS_SECTION_IN_FILE;
  }
#endif
  strcpy (szTempSection, "[");
  strcat (szTempSection, szSection);
  strcat (szTempSection, "]");

  do 
  {
    READLINE_EN09(Fd, Buffer, OldBufferSize, BufferSize, MoreData, ReadResult);
    if ( ReadResult != 1 )
    {
        return ReadResult;
    }
  } while ( STRCASECMP (Buffer, szTempSection) );
  return 1;
}

/*---------------------------------------------------------------------------*/

/*
  Function:     WriteSection ()
  description:  Write a section header
  
   To seperate each section, the section header can be prepended by a newline.

   "[szSection]\n"   without NewLineFlag

   or

   "\n[szSection]\n" with NewLineFlag

  arguments:
    Fd [in] Opened Filedescriptor
    szSection [in] Given Section Identifier
    NewLineFlag [in] Flag to indicate additional Newline prepended

  return value:
    <UL>
        <LI>0: Write failed ( inspect errno )
        <LI>1: Write ok
    </UL>
 */
static SAPDB_Int4 WriteSection (
                         SAPDB_Int4 Fd,
                         const SAPDB_Char *szSection,
                         SAPDB_Int4 NewLineFlag)
{
#undef MF__
#define MF__ MOD__"WriteSection"
  SAPDB_Char *szTempSection;
  SAPDB_Int4 cbLen, rc;

  szTempSection = (SAPDB_Char *)alloca(strlen(szSection)+strlen("\n[]\n") + 1);
  if (NewLineFlag == 0) 
  {
    strcpy (szTempSection, "[");
  }
  else 
  {
    strcpy (szTempSection, "\n[");
  }
  strcat (szTempSection, szSection);
  strcat (szTempSection, "]\n");
  cbLen = strlen (szTempSection); 

  rc = write (Fd, szTempSection, cbLen);
  if (cbLen != rc) 
  {
    return 0;
  }
  return 1;
}

/*---------------------------------------------------------------------------*/

/*
  Function:     WriteEntry ()
  description:  Write a entry line
  
    Write a line formed as 

    "szEntry=szString\n"

  arguments:
    Fd [in] Opened Filedescriptor
    szEntry [in] Given entry key
    szString [in] Given entry value
    
  return value:
    <UL>
        <LI>0: Write failed ( inspect errno )
        <LI>1: Write ok
    </UL>
 */
#undef MF__
#define MF__ MOD__"WriteEntry"
static SAPDB_Int4 WriteEntry (
                       SAPDB_Int4 Fd,
                       const SAPDB_Char *szEntry,
                       const SAPDB_Char *szString)
{
  SAPDB_Char *szTempEntry;
  SAPDB_Int4 cbLen, rc;

  szTempEntry = (SAPDB_Char *)alloca(strlen(szEntry) + strlen("=\n") + strlen(szString) + 1);
  strcpy (szTempEntry, szEntry);
  strcat (szTempEntry, "=");
  strcat (szTempEntry, szString);
  strcat (szTempEntry, "\n");
  
  cbLen = strlen (szTempEntry); 
  rc = write (Fd, szTempEntry, cbLen);
  if ( cbLen != rc) 
  {
    return 0;
  }
  return 1;
}

/*---------------------------------------------------------------------------*/

/*
  Function:     WriteLine ()
  description:  Write a line formed read by ReadLine
  
    Used to copy Registry content into a temporary file. Need to add a newline on each line.
    
  arguments:
    Fd [in] Opened Filedescriptor
    szString [in] Given line
    
  return value:
    <UL>
        <LI>0: Write failed ( inspect errno )
        <LI>1: Write ok
    </UL>
 */
#undef MF__
#define MF__ MOD__"WriteLine"
static SAPDB_Int4 WriteLine (
                      SAPDB_Int4 Fd,
                      const SAPDB_Char *szString)
{
  SAPDB_Char *szLine;
  SAPDB_Int4 cbLen, rc;

  szLine = (SAPDB_Char *)alloca(strlen(szString) + 2);
  strcpy (szLine, szString);
  strcat (szLine, "\n");
  cbLen = strlen (szLine);
  rc = write (Fd, szLine, cbLen);
  if (cbLen != rc)
  {
    return 0;
  }
  return 1;
}

/*---------------------------------------------------------------------------*/

/*
  Function:     ValidateConfigPath ()
  description:  Open the given path to the config files

    First create directory used for registry file.

  arguments:
    szConfigPath [in] Complete path to configuration directory
    ErrText [out] Errormessage if something went wrong
    Ok [out] Error enumeration

  return value:
    <UL>
        <LI>true:  Open ok,
        <LI>false: Open failed (Ok and ErrText are set). 
    </UL>
 */
static SAPDB_Bool ValidateConfigPath( SAPDB_Char const * szConfigPath,
                                      tsp00_ErrTextc ErrText,
                                      RTE_IniFileResult *pOk)
{
    struct stat StatBuff;
    SAPDB_Int4  umask_old ;
    SAPDB_Bool result = true;

    umask_old = umask(0);

    if (stat (szConfigPath, &StatBuff) != 0)
    {
        if ( mkdir (szConfigPath, 0711) != 0 )
        {
            *pOk = SAPDB_INIFILE_RESULT_ERR_TEMP;
            strcpy( (SAPDB_Char *)ErrText, "Mkdir(Registry):" );
            strncat( (SAPDB_Char *)ErrText, GetLastSystemErrorAsString(), sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
            result = false;
        }
    }

    umask(umask_old);

    return result;
}

/*---------------------------------------------------------------------------*/

/*
  Function:     OpenTempConfigFile ()
  description:  Open the temporary config file

    First create directory used for registry file. Then try to create the temporary file.

  arguments:
    szTempPath [in] Complete path to temporary configuration file
    ErrText [out] Errormessage if something went wrong
    Ok [out] Error enumeration

  return value:
    <UL>
        <LI>true:  Open ok,
        <LI>false: Open failed (Ok and ErrText are set). 
    </UL>
 */
#undef MF__
#define MF__ MOD__"OpenTempConfigFile"
static SAPDB_Bool OpenTempConfigFile(RTE_LockFile *pTempFile,
                                     SAPDB_Char *szTempPath,
                                     SAPDB_Char const *szPath)
{
    if ( !pTempFile->m_IsOpen )
    {
        int tmpFd;

        SAPDB_Int4  umask_old ;

        /* Create and fill temp lock file with machine name and process and thread id */
        sprintf(szTempPath, "%s.XXXXXX", szPath );

        umask_old = umask(022);

        /* PTS 1100015 */
#ifdef _WIN32
        tmpFd = open(mktemp (szTempPath),O_RDWR|O_CREAT | O_BINARY);
#else
        tmpFd = mkstemp (szTempPath);
#endif
        umask(umask_old);

        if ( tmpFd < 0 )
        {
            return false;
        }
        pTempFile->m_Path = szTempPath;
        /* To obtain an exclusive lock file must be opened read/write */
        pTempFile->Fd = tmpFd;
        pTempFile->m_IsOpen = true;
    }

    return true;
}

/*---------------------------------------------------------------------------*/

/*
  Function:     CopyBackTempFile ()
  description:  Copy Temp file Back to Original

    A simple rename() would create a critical section since unlocking of registry 
    would take place before rename() could start. So we have to copy content...
  
  arguments:
    TempFile [in] RTE_LockFile of Tempfile
    RegistryFile [in] RTE_LockFile of Registryfile
    ErrText [out] Errormessage if something went wrong
    Ok [out] Error enumeration

  return value:
    <UL>
        <LI>0: Write failed ( inspect Ok and ErrText )
        <LI>1: Write ok
    </UL>
 */
static SAPDB_Int4 CopyBackTempFile(  RTE_LockFile *pTempFile,
                              RTE_LockFile *pRegistryFile,
                              tsp00_ErrTextc ErrText,
                              RTE_IniFileResult *pOk)
{
    SAPDB_Int4  result = 0;
    SAPDB_Char  Buffer[4096];
    SAPDB_Int4  BytesRead = 0;
    SAPDB_Int4  BytesWritten = 0;
    SAPDB_Int4  TotalSize = 0;

    while ( 0 == result )
    {
        if ( !RegistryFile_Reset(pTempFile) )
        {
            *pOk = SAPDB_INIFILE_RESULT_ERR_SEEK;
            strcpy( (SAPDB_Char *)ErrText, "Seek(TempRegistry):" );
            strncat( (SAPDB_Char *)ErrText, GetLastSystemErrorAsString(), sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
            break;
        }

        if ( !RegistryFile_Reset(pRegistryFile) )
        {
            *pOk = SAPDB_INIFILE_RESULT_ERR_SEEK;
            strcpy( (SAPDB_Char *)ErrText, "Seek(Registry):" );
            strncat( (SAPDB_Char *)ErrText, GetLastSystemErrorAsString(), sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
            break;
        }

        while ( (BytesRead = read(pTempFile->Fd, Buffer, 4096)) > 0 )
        {
            size_t BytesOffset;

            for ( BytesOffset = 0; 
                  BytesOffset < BytesRead; 
                  BytesOffset += BytesWritten, TotalSize += BytesWritten )
            {
                BytesWritten = write(pRegistryFile->Fd, Buffer+BytesOffset, BytesRead-BytesOffset);
                if ( (BytesWritten < 0) 
                  && (errno != EINTR) )
                {
                    break;
                }
            }
        }

        if ( BytesRead < 0 )
        {
            *pOk = SAPDB_INIFILE_RESULT_ERR_READ;
            strcpy( (SAPDB_Char *)ErrText, "Read(TempRegistry):" );
            strncat( (SAPDB_Char *)ErrText, GetLastSystemErrorAsString(), sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
            break;
        }

        if ( BytesWritten < 0 )
        {
            *pOk = SAPDB_INIFILE_RESULT_ERR_WRITE;
            strcpy( (SAPDB_Char *)ErrText, "Writeback(Registry):" );
            strncat( (SAPDB_Char *)ErrText, GetLastSystemErrorAsString(), sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
            break;
        }

#ifndef _WIN32  //TODO!!
        ftruncate( pRegistryFile->Fd, TotalSize );
#else
        _chsize( pRegistryFile->Fd, TotalSize );
#endif
        ErrText[0] = '\0';
        *pOk = SAPDB_INIFILE_RESULT_OK;
        result = 1;
    }

    return result;
}

/*
   UpdateConfigString
 */
static SAPDB_Int4 UpdateConfigString (
                                      SAPDB_Bool        wantLock,
                                      const SAPDB_Char *szPath,
                                      const SAPDB_Char *szSection,
                                      const SAPDB_Char *szEntry, 
                                      const SAPDB_Char *szString,
                                      const SAPDB_Bool DeleteFlag,
                                      tsp00_ErrTextc ErrText,
                                      RTE_IniFileResult *pOk)
{
    SAPDB_Int4   BufferSize = LINE_SIZE_MAX;
    SAPDB_Int4   OldBufferSize = 0;
    SAPDB_Char *Buffer = (SAPDB_Char *)alloca(BufferSize);
    SAPDB_Bool  MoreData;
    RTE_LockFile RegistryFile;
    RTE_LockFile TempFile;
    SAPDB_Char *szTempPath;
    SAPDB_Int4 result = 0;
    
    if ( !szEntry )
    {
        *pOk = SAPDB_INIFILE_RESULT_ERR_PARAM;
        strcpy( (SAPDB_Char *)ErrText, "NULL pointer for key passed" );
        return 0;
    }
    
    if ( !szString && !DeleteFlag )
    {
        *pOk = SAPDB_INIFILE_RESULT_ERR_PARAM;
        strcpy( (SAPDB_Char *)ErrText, "NULL pointer for string passed" );
        return 0;
    }
    
    /* Validate given szEntry */
    if (strchr (szEntry, '=') != 0) 
    {
        *pOk = SAPDB_INIFILE_RESULT_ERR_KEY;
        strcpy( (SAPDB_Char *)ErrText, "Found '=' in key for Registry:" );
        strncat( (SAPDB_Char *)ErrText, szEntry, sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
        return 0;
    }

    RegistryFile_Init(&RegistryFile, false, false);
    RegistryFile_Init(&TempFile, true, false);
    
    while ( 0 == result )
    { 
        *pOk = SAPDB_INIFILE_RESULT_OK;
    
        szTempPath = (SAPDB_Char *)alloca(strlen(szPath) + strlen(".XXXXXX") + 1);

        /* Open temporary registry file */
        if ( !OpenTempConfigFile(&TempFile, szTempPath, szPath) )
        {
            *pOk = SAPDB_INIFILE_RESULT_ERR_TEMP;
            strcpy( (SAPDB_Char *)ErrText, "Open(TempRegistry):" );
            strncat( (SAPDB_Char *)ErrText, GetLastSystemErrorAsString(), sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
            break;
        }
    
        /* Open Registry (create if does not exist */
        if ( !RegistryFile_Open (&RegistryFile, szPath ) )
        {
            if ( errno != ENOENT )
            {
                *pOk = SAPDB_INIFILE_RESULT_ERR_OPEN;
                strcpy( (SAPDB_Char *)ErrText, "Open(Registry):" );
                strncat( (SAPDB_Char *)ErrText, GetLastSystemErrorAsString(), sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
                break;
            }

            if ( DeleteFlag )
            {
                /* nothing to delete */ 
                *pOk = SAPDB_INIFILE_RESULT_NO_ENTRY;
                strcpy( (SAPDB_Char *)ErrText, "Registry empty" );
                break;
            }

            /* Registry.ini-file did not exist before... */
        
            if ( !RegistryFile_Create(&RegistryFile, szPath) )
            {
                *pOk = SAPDB_INIFILE_RESULT_ERR_OPEN;
                strcpy( (SAPDB_Char *)ErrText, "Create(Empty Registry):" );
                strncat( (SAPDB_Char *)ErrText, GetLastSystemErrorAsString(), sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
                break;
            }

            RegistryFile_Close(&RegistryFile);

            /* Open Registry must work now */
            if ( !RegistryFile_Open (&RegistryFile, szPath ) )
            {
                *pOk = SAPDB_INIFILE_RESULT_ERR_OPEN;
                strcpy( (SAPDB_Char *)ErrText, "Open(Registry):" );
                strncat( (SAPDB_Char *)ErrText, GetLastSystemErrorAsString(), sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
                break;
            }
        }
    
        /*-------------------------------------
        * Registry File exists and is opened lock it now
        */
        if ( wantLock && !RegistryFile_Lock(&RegistryFile) )
        {
            *pOk = SAPDB_INIFILE_RESULT_ERR_LOCK;
            strcpy( (SAPDB_Char *)ErrText, "Lock(Registry):" );
            strncat( (SAPDB_Char *)ErrText, RegistryFile.m_LastError, sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
            break;
        }
    
        /*------------------------------------------------
        Copy each line until searched section is found 
        If EOF is reached before, append new Section.
        */
        {
            SAPDB_Int4 ReadResult = 0;
            SAPDB_Char *szTempSection = (SAPDB_Char *)alloca( strlen("[]") + strlen(szSection) + 1 );
            /*  ini-file already exists */
            strcpy (szTempSection, "[");
            strcat (szTempSection, szSection);
            strcat (szTempSection, "]");
        
            while ( 0 == result )
            {
                READLINE_EN09(RegistryFile.Fd, Buffer, OldBufferSize, BufferSize, MoreData, ReadResult);
                if ( ReadResult == -1 )
                {
                    *pOk = SAPDB_INIFILE_RESULT_ERR_READ;
                    strcpy( (SAPDB_Char *)ErrText, "Read head(Registry):" );
                    strncat( (SAPDB_Char *)ErrText, GetLastSystemErrorAsString(), sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
                    break;
                }
            
                if ( ReadResult == 0 )
                {
                    /* EOF reached before section was found */
                    if ( DeleteFlag )
                    {
                        /* nothing to delete */ 
                        *pOk = SAPDB_INIFILE_RESULT_NO_ENTRY;
                        strcpy( (SAPDB_Char *)ErrText, "No Registry section '" );
                        strncat( (SAPDB_Char *)ErrText, szSection, sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
                        strncat( (SAPDB_Char *)ErrText, "'", sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
                        break;
                    }
                
                    /* no such section found, append it */
                    if (WriteSection (TempFile.Fd, szSection, 1) == 0) 
                    {
                        *pOk = SAPDB_INIFILE_RESULT_ERR_WRITE;
                        strcpy( (SAPDB_Char *)ErrText, "Write(Registry) section '" );
                        strncat( (SAPDB_Char *)ErrText, szSection, sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
                        strncat( (SAPDB_Char *)ErrText, "':", sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
                        strncat( (SAPDB_Char *)ErrText, GetLastSystemErrorAsString(), sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
                        break;
                    }
                
                    if (WriteEntry (TempFile.Fd, szEntry, szString) == 0) 
                    {
                        *pOk = SAPDB_INIFILE_RESULT_ERR_WRITE;
                        strcpy( (SAPDB_Char *)ErrText, "Write(Registry) entry '" );
                        strncat( (SAPDB_Char *)ErrText, szEntry, sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
                        strncat( (SAPDB_Char *)ErrText, "':", sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
                        strncat( (SAPDB_Char *)ErrText, GetLastSystemErrorAsString(), sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
                        break;
                    }
                
                    result = 2;
                    break;
                }
                
                if ( WriteLine (TempFile.Fd, Buffer) == 0 )
                {
                    *pOk = SAPDB_INIFILE_RESULT_ERR_WRITE;
                    strcpy( (SAPDB_Char *)ErrText, "Copy write(Registry):" );
                    strncat( (SAPDB_Char *)ErrText, GetLastSystemErrorAsString(), sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
                    break;
                }
            
                if ( STRCASECMP (Buffer, szTempSection) == 0 )
                {
                    result = 1;
                }
            }
        
            if ( 0 == result )
            {
                break;
            }

            if ( 2 == result )
            {
                /* entry already found ... prematurely at end of file... completed */
                break;
            }

            /*--------------------------------------------------
            searched section found,
            copy each line until searched entry key is found 
            */
            result = 0;

            while ( 0 == result )
            {
                READLINE_EN09(RegistryFile.Fd, Buffer, OldBufferSize, BufferSize, MoreData, ReadResult);
                if ( ReadResult == -1 )
                {
                    *pOk = SAPDB_INIFILE_RESULT_ERR_READ;
                    strcpy( (SAPDB_Char *)ErrText, "Read(Registry) entries:" );
                    strncat( (SAPDB_Char *)ErrText, GetLastSystemErrorAsString(), sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
                    break;
                }
            
                if ( ReadResult == 0 )
                {
                    /* end of file, no such entry found */
                    if ( DeleteFlag )
                    {
                        /* nothing to delete */ 
                        *pOk = SAPDB_INIFILE_RESULT_NO_ENTRY;
                        strcpy( (SAPDB_Char *)ErrText, "No Registry entry '" );
                        strncat( (SAPDB_Char *)ErrText, szEntry, sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
                        strncat( (SAPDB_Char *)ErrText, "'", sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
                        break;
                    }
                
                    /* append entry */
                    if (WriteEntry (TempFile.Fd, szEntry, szString) == 0) 
                    {
                        *pOk = SAPDB_INIFILE_RESULT_ERR_WRITE;
                        strcpy( (SAPDB_Char *)ErrText, "Append(Registry) entry '" );
                        strncat( (SAPDB_Char *)ErrText, szEntry, sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
                        strncat( (SAPDB_Char *)ErrText, "':", sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
                        strncat( (SAPDB_Char *)ErrText, GetLastSystemErrorAsString(), sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
                        break;
                    }
                
                    result = 2;
                    break;
                }
            
                {
                    SAPDB_Char *EqualSign = strchr(Buffer, '=');
                
                    if ( EqualSign != NULL
                      && FoundMatchingEntry(Buffer, szEntry) )
                    {
                        /* entry found */ 
                        if ( !DeleteFlag ) 
                        {
                            /* replace it */
                            if (WriteEntry (TempFile.Fd, szEntry, szString) == 0) 
                            {
                                *pOk = SAPDB_INIFILE_RESULT_ERR_WRITE;
                                strcpy( (SAPDB_Char *)ErrText, "Replace(Registry) entry '" );
                                strncat( (SAPDB_Char *)ErrText, szEntry, sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
                                strncat( (SAPDB_Char *)ErrText, "':", sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
                                strncat( (SAPDB_Char *)ErrText, GetLastSystemErrorAsString(), sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
                                break;
                            }
                        }
                        result = 1;
                        break;
                   }
                
                   if ( Buffer[0] == '[' 
                     || Buffer[0] == '\0' )
                   {
                       if ( DeleteFlag )
                       {
                           /* nothing to delete */ 
                           *pOk = SAPDB_INIFILE_RESULT_NO_ENTRY;
                           strcpy( (SAPDB_Char *)ErrText, "No Registry entry '" );
                           strncat( (SAPDB_Char *)ErrText, szEntry, sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
                           strncat( (SAPDB_Char *)ErrText, "'", sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
                           break;
                       }
                       else
                       {
                           /* insert it */
                           if (WriteEntry (TempFile.Fd, szEntry, szString) == 0) 
                           {
                               *pOk = SAPDB_INIFILE_RESULT_ERR_WRITE;
                               strcpy( (SAPDB_Char *)ErrText, "Insert(Registry) entry '" );
                               strncat( (SAPDB_Char *)ErrText, szEntry, sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
                               strncat( (SAPDB_Char *)ErrText, "':", sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
                               strncat( (SAPDB_Char *)ErrText, GetLastSystemErrorAsString(), sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
                               break;
                           }
                       }
                    
                       if ( Buffer[0] == '[' )
                       {
                           if ( WriteLine (TempFile.Fd, "") == 0 )
                           {
                               *pOk = SAPDB_INIFILE_RESULT_ERR_WRITE;
                               strcpy( (SAPDB_Char *)ErrText, "Insert(Registry) empty line:" );
                               strncat( (SAPDB_Char *)ErrText, GetLastSystemErrorAsString(), sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
                               break;
                           }
                       }
                    
                       if ( WriteLine (TempFile.Fd, Buffer) == 0 )
                       {
                           *pOk = SAPDB_INIFILE_RESULT_ERR_WRITE;
                           strcpy( (SAPDB_Char *)ErrText, "Insert(Registry) next section:" );
                           strncat( (SAPDB_Char *)ErrText, GetLastSystemErrorAsString(), sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
                           break;
                       }
                    
                       result = 1;
                       break;
                   }
                   else
                   {
                       if ( !EqualSign )
                       {
                           *pOk = SAPDB_INIFILE_RESULT_ERR_KEY;
                           strcpy( (SAPDB_Char *)ErrText, "No '=' found in:" );
                           strncat( (SAPDB_Char *)ErrText, Buffer, sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
                           break;
                       }
                   }
               }
            
               if (WriteLine (TempFile.Fd, Buffer) == 0)
               {
                   *pOk = SAPDB_INIFILE_RESULT_ERR_WRITE;
                   strcpy( (SAPDB_Char *)ErrText, "Copy entrys(Registry):" );
                   strncat( (SAPDB_Char *)ErrText, GetLastSystemErrorAsString(), sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
                   break;
               }
           }

           if ( 0 == result )
           {
               break;
           }

           if ( 2 == result )
           {
               /* entry already found ... prematurely at end of file... completed */
               break;
           }

           do
           {
               READLINE_EN09(RegistryFile.Fd, Buffer, OldBufferSize, BufferSize, MoreData, ReadResult);
               if ( ReadResult == -1 )
               {
                   *pOk = SAPDB_INIFILE_RESULT_ERR_READ;
                   strcpy( (SAPDB_Char *)ErrText, "Read rest(Registry):" );
                   strncat( (SAPDB_Char *)ErrText, GetLastSystemErrorAsString(), sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
                   break;
               }
               if ( ReadResult == 1 )
               {
                   if ( WriteLine (TempFile.Fd, Buffer) == 0 )
                   {
                       *pOk = SAPDB_INIFILE_RESULT_ERR_WRITE;
                       strcpy( (SAPDB_Char *)ErrText, "Copy rest(Registry):" );
                       strncat( (SAPDB_Char *)ErrText, GetLastSystemErrorAsString(), sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
                       break;
                   }
               }
           } while( ReadResult != 0 );

           if ( ReadResult != 0 )
           {
               break;
           }
        }

        result = 1;
    }

    if ( 0 != result )
    {
        result = CopyBackTempFile(&TempFile, &RegistryFile, ErrText, pOk);
    }

    RegistryFile_Close(&RegistryFile);
    RegistryFile_Close(&TempFile);
    return result;
}

/*
   RemoveConfigString
 */
static SAPDB_Int4 RTE_RemoveUNIXConfigString (
    SAPDB_Bool wantLock,
    const SAPDB_Char *szFile,
    const SAPDB_Char *szSection,
    const SAPDB_Char *szEntry, 
    tsp00_ErrTextc ErrText,
    RTE_IniFileResult *pOk)
{
    SAPDB_Char *szPath;
    RTE_Path indepConfigPath;

    if ( (getenv(SAPDB_OWN_GLOBAL_CONFIG_FILE) != 0 && !strcmp(szFile, getenv(SAPDB_OWN_GLOBAL_CONFIG_FILE)))
      || !strcmp(szFile, SAPDB_GLOBAL_CONFIG_FILE) 
      || *szFile == '/' )
    {
        szPath = (SAPDB_Char *)alloca(strlen(szFile)+1);
        strcpy(szPath, szFile);
    }
    else
    {
        if ( !strcmp(SAPDB_ODBC_INI_FILE, szFile) )
        {
            szPath = (SAPDB_Char *)alloca(strlen(UNIX_GLOBAL_ODBC_INI_FILE)+1);
            strcpy(szPath, UNIX_GLOBAL_ODBC_INI_FILE);
        }
        else
        {
            if ( !RTE_GetCommonConfigPath(indepConfigPath, false, ErrText) )
            {
                *pOk = SAPDB_INIFILE_RESULT_ERR_PARAM;
                return 0;
            }
            /* If needed create directory for registry file */
            if ( !ValidateConfigPath(indepConfigPath, ErrText, pOk) )
            {
                /* Ok & ErrText already setup... */
                return 0;
            }

            szPath = (SAPDB_Char *)alloca(strlen(indepConfigPath)+1+strlen(szFile)+1);
            strcpy(szPath, indepConfigPath); 
#ifndef _WIN32
            strcat(szPath, "/"); 
#else
            strcat(szPath, "\\"); 
#endif
            strcat(szPath, szFile);
        }
    }

    return ( UpdateConfigString (
                    wantLock,
                    szPath,
                    szSection,
                    szEntry,
                    NULL, 
                    true, 
                    ErrText,
                    pOk) );
}
/*---------------------------------------------------------------------------*/

/*
  Function:     UpdateConfigString ()
  description:  Write or delete a complete config string
  
    Used to write a new entry into a config file if DeleteFlag == 0 or
    to delete an entry from config file if DeleteFlag != 0.
    
  arguments:
    szFile [in] Name of the configuration file
    szSection [in] Given section
    szEntry [in] Given entry key in 'key=value' string
    szString [in] Given entry value in 'key=value' string
    DeleteFlag [in] Flag to indicate delete instead of write
    ErrText [out] Errormessage if something went wrong
    Ok [out] Error enumeration

  return value:
    <UL>
        <LI>0: Write failed ( inspect ErrText )
        <LI>1: Write ok
    </UL>
 */
static SAPDB_Int4 UpdateInstallationConfigString (
                              const SAPDB_Char *szEntry, 
                              const SAPDB_Char *szString,
                              const SAPDB_Bool DeleteFlag,
                              tsp00_ErrTextc ErrText,
                              RTE_IniFileResult *pOk)
{
    RTE_Path pathToOwnGlobalConfigFile;

    if ( !myGetEnv(SAPDB_OWN_GLOBAL_CONFIG_FILE, pathToOwnGlobalConfigFile, sizeof(RTE_Path) ) )
    {
        SAPDB_strncpy_zero_terminated( &pathToOwnGlobalConfigFile[0], SAPDB_GLOBAL_CONFIG_FILE, sizeof(RTE_Path));
    }

    return UpdateConfigString ( SAPDB_UNLOCKED_ACCESS,
                                &pathToOwnGlobalConfigFile[0],
                                SAPDB_GLOBALS_SECTION,
                                szEntry, 
                                szString,
                                DeleteFlag,
                                ErrText,
                                pOk);
}

/*---------------------------------*/

/*
  buffer contains a line beginning with <whiteSpace>*szEntry<whiteSpace>*=

  This call returns if a matiching entry is found
 */
static SAPDB_Bool FoundMatchingEntry( SAPDB_Char const *buffer
                                    , SAPDB_Char const *szEntry )
{
    SAPDB_Char const *bufferPtr = buffer;
    SAPDB_Int iChar;

    while ( 0 != *bufferPtr 
         && isspace(*bufferPtr) )
        ++bufferPtr;

    for ( iChar = 0; '=' != *bufferPtr 
                    && 0 != *bufferPtr
                    && 0 != szEntry[iChar]; ++iChar, ++bufferPtr )
    {
        if ( toupper(*bufferPtr) != toupper(szEntry[iChar]) )
        {
            return false;
        }
    }

    if ( 0 == szEntry[iChar] )
    {
        while (   0 != *bufferPtr 
             && '=' != *bufferPtr
             && isspace(*bufferPtr) )
            ++bufferPtr;
    }

    return ( 0 == szEntry[iChar] && '=' == *bufferPtr );
}

/*---------------------------------*/

static SAPDB_Char *SkipWhiteSpaces(SAPDB_Char *buffer)
{
    SAPDB_Char *bufferPtr = buffer;

    while (   0 != *bufferPtr 
         && isspace(*bufferPtr) )
        ++bufferPtr;
    return bufferPtr;
}

/*---------------------------------*/

static SAPDB_Int4 GetConfigString (
                        SAPDB_Bool      wantLock,
                        RTE_LockFile       *pRegistryFile,
                        const SAPDB_Char *    szPath,
                        const SAPDB_Char *    szSection,
                        const SAPDB_Char *    szEntry, 
                        SAPDB_Char       *    szString,
                        const SAPDB_Int4       MaxStringLen,
                        tsp00_ErrTextc   ErrText,
                        RTE_IniFileResult * pOk)
{  
    SAPDB_Char * pVal;
    SAPDB_Char * ptr;
    SAPDB_Int4    ReadResult;

    if ( !szEntry )
    {
        *pOk = SAPDB_INIFILE_RESULT_ERR_PARAM;
        strcpy ( (SAPDB_Char *)ErrText, "NULL pointer for key passed" );
        return 0;
    }
    
    if ( !szString )
    {
        *pOk = SAPDB_INIFILE_RESULT_ERR_PARAM;
        strcpy ( (SAPDB_Char *)ErrText, "NULL pointer for value passed" );
        return 0;
    }
    
    if ( *szEntry == 0 )
    {
        *pOk = SAPDB_INIFILE_RESULT_ERR_PARAM;
        strcpy ( (SAPDB_Char *)ErrText, "empty key passed" );
        return 0;
    }

    *szString = 0;

    if ( !RegistryFile_Open (pRegistryFile, szPath) )
    {
        *pOk = SAPDB_INIFILE_RESULT_ERR_OPEN;
        strcpy ( (SAPDB_Char *)ErrText, "Open Registry:" );
        strncat ( (SAPDB_Char *)ErrText, GetLastSystemErrorAsString(), sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
        return 0;
    }

    if ( wantLock && !RegistryFile_Lock(pRegistryFile) )
    {
        *pOk = SAPDB_INIFILE_RESULT_ERR_LOCK;
        strcpy ( (SAPDB_Char *)ErrText, "Lock(Registry):" );
        strncat ( (SAPDB_Char *)ErrText, pRegistryFile->m_LastError, sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
        return 0;
    }

    ReadResult = FindSection (pRegistryFile->Fd, szSection);
    if ( ReadResult == -1 )
    {
        szString[0] = '\0';
        *pOk = SAPDB_INIFILE_RESULT_ERR_READ;
        strcpy( (SAPDB_Char *)ErrText, "Read section(Registry) '" );
        strncat( (SAPDB_Char *)ErrText, szSection, sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
        strncat( (SAPDB_Char *)ErrText, "':", sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
        strncat( (SAPDB_Char *)ErrText, GetLastSystemErrorAsString(), sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
        return 0;
    }

    if ( ReadResult == 0) 
    {
        szString[0] = '\0';
        *pOk = SAPDB_INIFILE_RESULT_NO_ENTRY;
        strcpy( (SAPDB_Char *)ErrText, "Section not in Registry:" );
        strncat( (SAPDB_Char *)ErrText, szSection, sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
        return 0;
    }
    {
        SAPDB_Bool FoundEntry = false;

        SAPDB_Char *Buffer = (SAPDB_Char *)alloca(LINE_SIZE_MAX);
        SAPDB_Int4   OldBufferSize = 0;
        SAPDB_Int4   BufferSize = LINE_SIZE_MAX;
        SAPDB_Bool  MoreData;

        do
        {
            READLINE_EN09(pRegistryFile->Fd, Buffer, OldBufferSize, BufferSize, MoreData, ReadResult);
            if ( ReadResult == -1 )
            {
                szString[0] = '\0';
                *pOk = SAPDB_INIFILE_RESULT_ERR_READ;
                strcpy( (SAPDB_Char *)ErrText, "Read problem(Registry):" );
                strncat( (SAPDB_Char *)ErrText, GetLastSystemErrorAsString(), sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
                return 0;
            }

            if ( ReadResult == 0 
              || Buffer[0] == '\0'
              || Buffer[0] == '[' )
            {
                szString[0] = '\0';
                *pOk = SAPDB_INIFILE_RESULT_NO_ENTRY;
                strcpy( (SAPDB_Char *)ErrText, "Entry not in Registry:" );
                strncat( (SAPDB_Char *)ErrText, szEntry, sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
                return 0;
            }

            /* PTS 1105650 */
            pVal = strchr (Buffer, '=');

        } while ( pVal == NULL 
             || !FoundMatchingEntry(&Buffer[0], szEntry) );
                         
        /* PTS 1105122 */
        if ( pVal != NULL )
        {
             pVal = SkipWhiteSpaces(pVal+1);
             strncpy(szString, pVal, MaxStringLen-1);
             szString[MaxStringLen-1] = 0;

             if ( strlen(pVal) >= MaxStringLen )
             {
                 *pOk = SAPDB_INIFILE_RESULT_TRUNCATED;
                 sprintf( (SAPDB_Char *)ErrText, "Value [%d/%d] truncated:", MaxStringLen, strlen(pVal)+1 );
                 strncat( (SAPDB_Char *)ErrText, Buffer, sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
             }
             else
             {
                 *pOk = SAPDB_INIFILE_RESULT_OK;
                 ErrText[0] = '\0';
             }
        }
        else
        {
             *pOk = SAPDB_INIFILE_RESULT_ERR_KEY;
             strcpy( (SAPDB_Char *)ErrText, "Bad entry:");
             strncat( (SAPDB_Char *)ErrText, Buffer, sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
             return 0;
        }
    }
    return (strlen (szString));
}

/*----------------------------------------------------------------------*/
static SAPDB_Int4 RTE_NextUNIXConfigEnum( RTE_RegistryHandle hEnum,
                                          SAPDB_Char *szEntry,
                                          const SAPDB_Int4 MaxEntryLen,
                                          SAPDB_Char *szString,
                                          const SAPDB_Int4 MaxStringLen,
                                          tsp00_ErrTextc ErrText,
                                          RTE_IniFileResult * pOk )
{

    SAPDB_Int4 i, j, State;
    SAPDB_Char  *nextEntry = hEnum->nextEntry;
    SAPDB_Char  *Buffer = (SAPDB_Char *)alloca(LINE_SIZE_MAX);
    SAPDB_Int4   OldBufferSize = 0;
    SAPDB_Int4   BufferSize = LINE_SIZE_MAX;
    SAPDB_Bool   MoreData;
    SAPDB_Int4   ReadResult;

    if ( szEntry != NULL ) *szEntry = 0;
    if ( szString != NULL ) *szString = 0;

    for (;;) 
    {
        COPYLINE_EN09(&nextEntry, Buffer, OldBufferSize, BufferSize, MoreData, ReadResult);
        hEnum->nextEntry = nextEntry;

        if ( ReadResult == 0 )
        {
          strcpy( ErrText, "End of Registry" );
          *pOk = SAPDB_INIFILE_RESULT_EOF;
          return 0;
        }

        if (Buffer[0] == '[')
        {
          strcpy( ErrText, "End of Section" );
          *pOk = SAPDB_INIFILE_RESULT_EOF;
          return 0;
        }

        /* skip empty lines */
        if (Buffer[0] != '\0')
            break;
    }

    {
    SAPDB_Char *EqualSign;

    EqualSign = strchr(Buffer, '=');
    if ( !EqualSign )
    {
      *pOk = SAPDB_INIFILE_RESULT_ERR_KEY;
      strcpy( (SAPDB_Char *)ErrText, "No '=' found in:" );
      strncat( (SAPDB_Char *)ErrText, Buffer, sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
      return 0;
    }

    *EqualSign = '\0';

    if ( szEntry != NULL )
    {
      SAPDB_strncpy_zero_terminated(szEntry, Buffer, MaxEntryLen);

      if ( strlen(Buffer) >= MaxEntryLen )
      {
        *pOk = SAPDB_INIFILE_RESULT_TRUNCATED;
        sprintf( (SAPDB_Char *)ErrText, "Key [%d/%d] truncated:", MaxEntryLen, strlen(Buffer)+1 );
        strncat( (SAPDB_Char *)ErrText, Buffer, sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
        return 0;
      }
    }

    if ( szString != NULL )
    {
      SAPDB_strncpy_zero_terminated(szString, EqualSign+1, MaxStringLen);

      if ( strlen(EqualSign+1) >= MaxStringLen )
      {
        *pOk = SAPDB_INIFILE_RESULT_TRUNCATED;
        sprintf( (SAPDB_Char *)ErrText, "Value [%d/%d] truncated:", MaxStringLen, strlen(EqualSign+1)+1 );
        strncat( (SAPDB_Char *)ErrText, Buffer, sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
        return 0;
      }
    }
    }

    ErrText[0] = '\0';
    *pOk = SAPDB_INIFILE_RESULT_OK;
    return 1;
}

/*
  Class:     RTE_LockFile
  description:  Helper class to make sure file lock is removed and file is closed after return

    RTE_LockFile object is created without any file handle attached.
    After call to Open() the file handle is stored internally.
    The member function 'Lock()' is called afterwards.
    Destructor will remove the lock automatically if function returns and close the File.

  arguments: none
  return value: none
 */
static void RegistryFile_Init(RTE_LockFile *this, SAPDB_Bool IsTemp, SAPDB_Bool readOnly)
{
#ifdef _WIN32
  int size;
#else
  struct utsname unam;
#endif
  this->m_IsTemp = IsTemp;
  this->m_Path   = "";
  this->Fd     = -1;
  this->m_IsOpen = false;
  this->m_IsLocked = false;
  this->m_ReadOnly = readOnly;
  this->m_LockEntry.processId = (SAPDB_UInt4)getpid();
#ifdef _WIN32
  size = sizeof(this->m_LockEntry.machineName);
  GetComputerName( &this->m_LockEntry.machineName[0], &size );
#else
  memset(&unam, 0, sizeof(struct utsname));
  (void)uname ( &unam );
  SAPDB_strncpy_zero_terminated(&this->m_LockEntry.machineName[0], &unam.nodename[0], sizeof(this->m_LockEntry.machineName));
#endif
  this->m_LastError = "";
}

/*---------------------------------*/

static SAPDB_Bool RegistryFile_Open(RTE_LockFile *this, const SAPDB_Char *Path)
{
  if ( !this->m_IsOpen )
  {
    this->m_Path = Path;
    /* To obtain an exclusive lock file must be opened read/write */
#ifdef _WIN32        
    this->Fd = open(Path, (this->m_ReadOnly ? O_RDONLY : O_RDWR) | O_BINARY);
#else
    this->Fd = open(Path, this->m_ReadOnly ? O_RDONLY : O_RDWR);
#endif
    this->m_IsOpen = ( this->Fd != -1 );
  }
  return this->m_IsOpen;
}

/*---------------------------------*/

static SAPDB_Bool RegistryFile_Create(RTE_LockFile *this, const SAPDB_Char *Path)
{
    if ( !this->m_IsOpen )
    {
        this->m_Path = Path;
#ifdef _WIN32        
        this->Fd = open(Path, O_RDWR|O_CREAT | O_BINARY, 0666); /* external umask is used to limitate creation mode */
#else
        this->Fd = open(Path, O_RDWR|O_CREAT, 0666); /* external umask is used to limitate creation mode */
#endif
        this->m_IsOpen = ( this->Fd != -1 );
    }
    return this->m_IsOpen;
}

/*---------------------------------*/

#ifdef SDB_MULTI_THREAD

static teo07_Mutex RTE_AloneMutex = 0;

static void RTE_WaitUntilAloneForLock(SAPDB_Char const *pathToLock)
{
    if ( 0 == RTE_AloneMutex )
    {
        teo07_Mutex aloneMutex;

        sqlcreatemutex(&aloneMutex);

        RTE_AloneMutex = aloneMutex;
    
        sqlbeginmutex(&aloneMutex);

        if ( aloneMutex != RTE_AloneMutex )
        {
            sqlendmutex(&aloneMutex);
            sqldestroymutex(&aloneMutex);
            sqlbeginmutex(&RTE_AloneMutex);
        }
    }
    else
    {
        sqlbeginmutex(&RTE_AloneMutex);
    }
}


/*---------------------------------*/

static void RTE_AllowNextLocker(SAPDB_Char const *pathToLock)
{
    sqlendmutex(&RTE_AloneMutex);
}
#endif /* SDB_MULTI_THREAD */

static int threadId = 0;
/*---------------------------------*/

static SAPDB_Bool CheckForRetry(int lastErrno)
{
    switch( lastErrno )
    {
    case EINTR:
/*    case ESTALE: */
        return true;
    default:
        return false;
    }
}

/*---------*/

/*
  Old locker entry found only if no process id set or machine name matches and process no longer running
 */
static SAPDB_Bool RegistryFile_OldLocker(RTE_LockFile *this, RTE_LockFileContent *pEntry, bool *pIsRemoteLocked)
{
    if ( pEntry->processId == 0 )
    {
        if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_ALL_INFOS) )
        {
            printf("Found zeroed process id\n");
        }
        return true;
    }

    if ( 0 != SAPDB_memcmp( &pEntry->machineName[0], &this->m_LockEntry.machineName[0], sizeof(pEntry->machineName) ) )
    {
        if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_WARNING_AND_ERROR_ONLY) )
        {
            printf("Cannot decide for %u on machine %s own machine %s\n"
                , pEntry->processId
                , &pEntry->machineName[0]
                , &this->m_LockEntry.machineName[0] );
        }
        *pIsRemoteLocked = true;
        return false;
    }

#ifndef _WIN32
    if ( kill( (pid_t)pEntry->processId, 0) == 0
      || ESRCH != errno )
    {
        if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_WARNING_AND_ERROR_ONLY) )
        {
            printf("Found active process %u on own machine %s\n"
                , pEntry->processId
                , &pEntry->machineName[0] );
        }
        if ( (pid_t)pEntry->processId == getpid() )
        {
            if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_WARNING_AND_ERROR_ONLY) )
            {
                printf("Found myself\n");
            }
            return true;
        }
        return false;
    }
#endif
    if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_WARNING_AND_ERROR_ONLY) )
    {
        printf("Found inactive process %u on own machine %s\n"
            , pEntry->processId
            , &pEntry->machineName[0] );
    }
    return true;
}

/*---------*/

/* open/create lock file for r+w white external umask ignored */

static int open_unprotected_lockfile(char *file)
{
    int openResult;
#ifndef _WIN32
    mode_t oldMask;
#endif

    do {
#ifdef _WIN32        
        openResult = open(file, O_RDWR | O_BINARY);
#else
        openResult = open(file, O_RDWR);
#endif
        if ( openResult >= 0 )
        {
            return openResult;
        }
    } while ( errno == EINTR );

    do {
#ifdef _WIN32
        openResult = open(file, O_RDWR|O_CREAT | O_BINARY, 0666);
#else
        openResult = open(file, O_RDWR|O_CREAT, 0666);
#endif
        if ( openResult >= 0 )
        {
            if ( my_save_chmod(file, 0666) < 0 )
            {
                int lastErrno = errno;
                close(openResult);
                errno = lastErrno;
                openResult = -1;
            }
            return openResult;
        }
    } while ( errno == EINTR );
    return openResult;
}

/*---------*/

static SAPDB_Bool RegistryFile_SimulateGotLock(RTE_LockFile *this, SAPDB_Bool *pCanRetry, SAPDB_Bool *pIsRemoteLocked)
{
    SAPDB_Bool gotLock = false;
    SAPDB_Bool keepTrying = true;
    int lockFd;
    RTE_LockFileContent existingEntries[2];
    SAPDB_Char *lockFilePath;

#ifndef _WIN32
    if ( access(this->m_Path, W_OK) != 0 )
    {
        if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_ALL_INFOS) )
        {
            printf("No write access, no lock needed\n");
        }
        this->m_IsLocked = false;
        return true; /* No lock needed, since file is readonly anyhow */
    }
#endif

    lockFilePath = (SAPDB_Char *)alloca(strlen(this->m_Path)+strlen(SDB_LOCK_FILE_SUFFIX)+1);
    sprintf(lockFilePath, "%s" SDB_LOCK_FILE_SUFFIX, this->m_Path);

    while ( !gotLock )
    {
#ifdef _WIN32  //TODO: switch to RTESys_IO!
        int readResult;
#else
        ssize_t readResult;
#endif
        /* 0) first open file */
        lockFd = open_unprotected_lockfile(lockFilePath); /* external umask ignored */
        if ( lockFd < 0 )
        {
            *pCanRetry = CheckForRetry(errno);
            this->m_LastError = GetLastSystemErrorAsString();
            if ( !*pCanRetry || RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_ERROR_ONLY) )
            {
                if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_FATAL_ONLY) )
                {
                    printf("first open lock %s failed:%s\n"
                        , lockFilePath
                        , this->m_LastError);
                }
            }
            break;
        }

        /* 1) first try to read one or two entries */
        readResult = read(lockFd, &existingEntries[0], 2 * sizeof(RTE_LockFileContent));
        if ( 0 == readResult )
        {
            *pIsRemoteLocked = false;
            if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_ALL_INFOS) )
            {
                printf("first read lock %s empty\n", lockFilePath);
            }
        }
        else if ( sizeof(RTE_LockFileContent) == readResult )
        {
            *pIsRemoteLocked = false;
            if ( 0 == SAPDB_memcmp( &existingEntries[0], &RegistryFile_Nobody, sizeof(RTE_LockFileContent)) )
            {
                if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_ALL_INFOS) )
                {
                    printf("first read lock %s found it unlocked\n", lockFilePath);
                }
            }
            else
            {
                if ( RegistryFile_OldLocker(this, &existingEntries[0], pIsRemoteLocked) )
                {
                    if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_WARNING_AND_ERROR_ONLY) )
                    {
                        printf("first read lock %s had old entry (ignored)\n", lockFilePath);
                    }
                }
                else
                {
                    if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_WARNING_AND_ERROR_ONLY) )
                    {
                        printf("first read lock %s has active entry\n", lockFilePath);
                    }
                    break;
                }
            }
        }
        else if ( (2*sizeof(RTE_LockFileContent)) == readResult )
        {
            *pIsRemoteLocked = false;
            if ( RegistryFile_OldLocker(this, &existingEntries[0], pIsRemoteLocked) )
            {
                if ( RegistryFile_OldLocker(this, &existingEntries[1], pIsRemoteLocked) )
                {
                    if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_WARNING_AND_ERROR_ONLY) )
                    {
                        printf("first read lock %s had two old entries (ignored)\n", lockFilePath);
                    }
                }
                else
                {
                    if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_WARNING_AND_ERROR_ONLY) )
                    {
                        printf("first read lock %s has at active second entry\n", lockFilePath);
                    }
                    break;
                }
            }
            else
            {
                if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_WARNING_AND_ERROR_ONLY) )
                {
                    printf("first read lock %s has at active first entry\n", lockFilePath);
                }

                if ( RegistryFile_OldLocker(this, &existingEntries[1], pIsRemoteLocked) )
                {
                    if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_WARNING_AND_ERROR_ONLY) )
                    {
                        printf("first read lock %s had second old entry\n", lockFilePath);
                    }
                }
                else
                {
                    if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_WARNING_AND_ERROR_ONLY) )
                    {
                        printf("first read lock %s has at active second entry\n", lockFilePath);
                    }
                    break;
                }
                break;
            }
        }
        else
        {
            *pCanRetry = CheckForRetry(errno);
            this->m_LastError = GetLastSystemErrorAsString();
            if ( !*pCanRetry || RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_ERROR_ONLY) )
            {
                if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_FATAL_ONLY) )
                {
                    printf("first read lock %s entries failed %ld:%s\n"
                        , lockFilePath
                        , (long)readResult
                        , this->m_LastError );
                }
            }
            break;
        }

        /* 2) first write own info at 1st position */
#ifdef _WIN32  //TODO!!!
        lseek(lockFd, (off_t)0, SEEK_SET);
        if ( write(lockFd, &this->m_LockEntry, sizeof(RTE_LockFileContent))
            !=  sizeof(RTE_LockFileContent) )
#else
        if ( pwrite(lockFd, &this->m_LockEntry, sizeof(RTE_LockFileContent), (off_t)0)
            !=  sizeof(RTE_LockFileContent) )
#endif
        {
            *pCanRetry = CheckForRetry(errno);
            this->m_LastError = GetLastSystemErrorAsString();
            if ( !*pCanRetry || RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_ERROR_ONLY) )
            {
                if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_FATAL_ONLY) )
                {
                    printf("write first slot in %s failed:%s\n"
                        , lockFilePath
                        , this->m_LastError);
                }
            }
            break;
        }
        /* 3) first close */
        if ( 0 != close(lockFd) )
        {
            lockFd = -1;
            *pCanRetry = CheckForRetry(errno);
            this->m_LastError = GetLastSystemErrorAsString();
            if ( !*pCanRetry || RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_ERROR_ONLY) )
            {
                if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_FATAL_ONLY) )
                {
                    printf("first closing lock %s failed:%s\n"
                        , lockFilePath
                        , this->m_LastError);
                }
            }
            break;
        }

        /* 4) second open */
        lockFd = open_unprotected_lockfile(lockFilePath); /* external umask ignored */
        if ( lockFd < 0 )
        {
            *pCanRetry = CheckForRetry(errno);
            this->m_LastError = GetLastSystemErrorAsString();
            if ( !*pCanRetry || RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_ERROR_ONLY) )
            {
                if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_FATAL_ONLY) )
                {
                    printf("second open lock %s failed:%s\n"
                        , lockFilePath
                        , this->m_LastError);
                }
            }
            break;
        }

        /* 5) second try to read one or two entries */
        readResult = read(lockFd, &existingEntries[0], 2 * sizeof(RTE_LockFileContent));
        if ( sizeof(RTE_LockFileContent) == readResult )
        {
            *pIsRemoteLocked = false;
            if ( 0 == SAPDB_memcmp( &existingEntries[0], &this->m_LockEntry, sizeof(RTE_LockFileContent)) )
            {
                if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_ALL_INFOS) )
                {
                    printf("second read lock %s succeeded\n", lockFilePath);
                }
            }
            else
            {
                if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_ALL_INFOS) )
                {
                    printf("second read lock %s found different entry\n", lockFilePath);
                }

                if ( RegistryFile_OldLocker(this, &existingEntries[0], pIsRemoteLocked) )
                {
                    if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_ALL_INFOS) )
                    {
                        printf("entry overwritten by old locker\n");
                    }
                }
                else
                {
                    if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_ALL_INFOS) )
                    {
                        printf("entry overwritten by active locker\n");
                    }
                }
                break;
            }
        }
        else if ( 0 == readResult )
        {
            this->m_LastError = "file became empty";
            *pIsRemoteLocked = false;
            if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_ALL_INFOS) )
            {
                printf("second read lock %s file now empty\n", lockFilePath);
            }
            break;
        }
        else if ( (2*sizeof(RTE_LockFileContent)) == readResult )
        {
            *pIsRemoteLocked = false;
            if ( 0 == SAPDB_memcmp( &existingEntries[0], &this->m_LockEntry, sizeof(RTE_LockFileContent)) )
            {
                if ( RegistryFile_OldLocker(this, &existingEntries[1], pIsRemoteLocked) )
                {
                    if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_ALL_INFOS) )
                    {
                        printf("second read lock %s succeeded\n", lockFilePath);
                    }
                }
                else
                {
                    if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_ALL_INFOS) )
                    {
                        printf("second read lock second entry overwritten by active locker\n");
                    }
                }
            }
            else
            {
                if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_ALL_INFOS) )
                {
                    printf("second read lock %s found two entries\n", lockFilePath);
                }

                if ( RegistryFile_OldLocker(this, &existingEntries[0], pIsRemoteLocked) )
                {
                    if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_ALL_INFOS) )
                    {
                        printf("First entry overwritten by old locker\n");
                    }

                    if ( RegistryFile_OldLocker(this, &existingEntries[1], pIsRemoteLocked) )
                    {
                        if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_ALL_INFOS) )
                        {
                            printf("second entry overwritten by old locker\n");
                        }
                    }
                    else
                    {
                        if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_ALL_INFOS) )
                        {
                            printf("second entry overwritten by active locker\n");
                        }
                    }
                }
                else
                {
                    if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_ALL_INFOS) )
                    {
                        printf("first entry overwritten by active locker\n");
                    }

                    if ( RegistryFile_OldLocker(this, &existingEntries[1], pIsRemoteLocked) )
                    {
                        if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_ALL_INFOS) )
                        {
                            printf("second entry overwritten by old locker\n");
                        }
                    }
                    else
                    {
                        if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_ALL_INFOS) )
                        {
                            printf("second entry overwritten by active locker\n");
                        }
                    }
                }
                break;
            }
        }
        else
        {
            *pCanRetry = CheckForRetry(errno);
            this->m_LastError = GetLastSystemErrorAsString();
            if ( !*pCanRetry || RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_ERROR_ONLY) )
            {
                if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_FATAL_ONLY) )
                {
                    printf("second read lock %s entries failed %ld:%s\n"
                        , lockFilePath
                        , (long)readResult
                        , this->m_LastError );
                }
            }
            break;
        }

        /* 6) second write does write second entry */
#ifdef _WIN32
        lseek(lockFd, (off_t)sizeof(RTE_LockFileContent), SEEK_SET);
        if ( write(lockFd, &this->m_LockEntry, sizeof(RTE_LockFileContent))
            !=  sizeof(RTE_LockFileContent) )
#else
        if ( pwrite(lockFd, &this->m_LockEntry, sizeof(RTE_LockFileContent), (off_t)sizeof(RTE_LockFileContent))
            !=  sizeof(RTE_LockFileContent) )
#endif
        {
            *pCanRetry = CheckForRetry(errno);
            this->m_LastError = GetLastSystemErrorAsString();
            if ( !*pCanRetry || RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_ERROR_ONLY) )
            {
                if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_FATAL_ONLY) )
                {
                    printf("write second slot in %s failed:%s\n"
                        , lockFilePath
                        , this->m_LastError);
                }
            }
            break;
        }
        /* 7) second close */
        if ( 0 != close(lockFd) )
        {
            *pCanRetry = CheckForRetry(errno);
            lockFd = -1;
            this->m_LastError = GetLastSystemErrorAsString();
            if ( !*pCanRetry || RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_ERROR_ONLY) )
            {
                if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_FATAL_ONLY) )
                {
                    printf("second closing lock %s failed:%s\n"
                        , lockFilePath
                        , this->m_LastError);
                }
            }
            break;
        }

        /* 8) third open */
        lockFd = open_unprotected_lockfile(lockFilePath); /* external umask ignored */
        if ( lockFd < 0 )
        {
            *pCanRetry = CheckForRetry(errno);
            this->m_LastError = GetLastSystemErrorAsString();
            if ( !*pCanRetry || RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_ERROR_ONLY) )
            {
                if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_FATAL_ONLY) )
                {
                    printf("third open lock %s failed:%s\n"
                        , lockFilePath
                        , this->m_LastError);
                }
            }
            break;
        }

        /* 9) third read one or two entries */
        readResult = read(lockFd, &existingEntries[0], 2 * sizeof(RTE_LockFileContent));
        if ( (2*sizeof(RTE_LockFileContent)) == readResult )
        {
            *pIsRemoteLocked = false;
            if ( 0 == SAPDB_memcmp(&existingEntries[0], &this->m_LockEntry, sizeof(RTE_LockFileContent))
              || 0 == SAPDB_memcmp(&existingEntries[0], &RegistryFile_Nobody, sizeof(RTE_LockFileContent)) )
            {
                if ( 0 == SAPDB_memcmp(&existingEntries[1], &this->m_LockEntry, sizeof(RTE_LockFileContent)) )
                {
                    if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_ALL_INFOS) )
                    {
                        printf("both lock entries in %s ok\n", lockFilePath);
                    }
                }
                else
                {
                    this->m_LastError = "second entry overwritten";
                    if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_WARNING_AND_ERROR_ONLY) )
                    {
                        printf("third read lock %s found overwritten second entry\n", lockFilePath);
                    }
                    if ( RegistryFile_OldLocker(this, &existingEntries[1], pIsRemoteLocked) )
                    {
                        if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_WARNING_AND_ERROR_ONLY) )
                        {
                            printf("overwritten by old locker\n", lockFilePath);
                        }
                    }
                    else
                    {
                        if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_WARNING_AND_ERROR_ONLY) )
                        {
                            printf("overwritten by active locker\n", lockFilePath);
                        }
                        /* 10) invalidate first entry to prevent deadlock */
#ifdef _WIN32
                        lseek(lockFd, (off_t)0, SEEK_SET);
                        if ( write(lockFd, &RegistryFile_Nobody, sizeof(RTE_LockFileContent)) != sizeof(RTE_LockFileContent) )
#else
                        if ( pwrite(lockFd, &RegistryFile_Nobody, sizeof(RTE_LockFileContent), (off_t)0) != sizeof(RTE_LockFileContent) )
#endif
                        {
                            *pCanRetry = CheckForRetry(errno);
                            this->m_LastError = GetLastSystemErrorAsString();
                            if ( !*pCanRetry || RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_ERROR_ONLY) )
                            {
                                if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_FATAL_ONLY) )
                                {
                                    printf("failed to invaldiate first slot in %s:%s\n"
                                        , lockFilePath
                                        , this->m_LastError);
                                }
                            }
                        }
                    }
                    break;
                }
            }
            else
            {
                this->m_LastError = "first entry overwritten";
                if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_WARNING_AND_ERROR_ONLY) )
                {
                    printf("third read lock %s found overwritten first entry\n", lockFilePath);
                }

                if ( RegistryFile_OldLocker(this, &existingEntries[0], pIsRemoteLocked) )
                {
                    if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_WARNING_AND_ERROR_ONLY) )
                    {
                        printf("first entry overwritten by old locker\n");
                    }
                }
                else
                {
                    if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_WARNING_AND_ERROR_ONLY) )
                    {
                        printf("first entry overwritten by active locker\n");
                    }
                }

                if ( 0 == SAPDB_memcmp(&existingEntries[1], &this->m_LockEntry, sizeof(RTE_LockFileContent)) )
                {
                    if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_WARNING_AND_ERROR_ONLY) )
                    {
                        printf("second entry ok\n");
                    }
                }
                else
                {
                    if ( RegistryFile_OldLocker(this, &existingEntries[1], pIsRemoteLocked) )
                    {
                        if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_WARNING_AND_ERROR_ONLY) )
                        {
                            printf("second entry overwritten by old locker\n");
                        }
                    }
                    else
                    {
                        if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_WARNING_AND_ERROR_ONLY) )
                        {
                            printf("second entry overwritten by active locker\n");
                        }
                    }
                    /* 10) invalidate first entry to prevent deadlock */
#ifdef _WIN32
                    lseek(lockFd,SEEK_SET,(off_t)0);
                    if ( write(lockFd, &RegistryFile_Nobody, sizeof(RTE_LockFileContent)) != sizeof(RTE_LockFileContent) )
#else
                    if ( pwrite(lockFd, &RegistryFile_Nobody, sizeof(RTE_LockFileContent), (off_t)0) != sizeof(RTE_LockFileContent) )
#endif
                    {
                        *pCanRetry = CheckForRetry(errno);
                        this->m_LastError = GetLastSystemErrorAsString();
                        if ( !*pCanRetry || RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_ERROR_ONLY) )
                        {
                            if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_FATAL_ONLY) )
                            {
                                printf("failed to invaldiate first slot in %s:%s\n"
                                    , lockFilePath
                                    , this->m_LastError);
                            }
                        }
                    }
                }
                break; /* force retry is collision was detected on first entry... */
            }
        }
        else if ( sizeof(RTE_LockFileContent) == readResult )
        {
            *pIsRemoteLocked = false;
            this->m_LastError = "second entry truncated";
            if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_WARNING_AND_ERROR_ONLY) )
            {
                printf("third read lock %s found only one entry\n", lockFilePath);
            }

            if ( 0 == SAPDB_memcmp( &existingEntries[0], &this->m_LockEntry, sizeof(RTE_LockFileContent)) )
            {
                if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_WARNING_AND_ERROR_ONLY) )
                {
                    printf("third read found first entry ok but second missing\n");
                }
            }
            else
            {
                if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_WARNING_AND_ERROR_ONLY) )
                {
                    printf("third read lock %s found different entry\n", lockFilePath);
                }
                if ( RegistryFile_OldLocker(this, &existingEntries[0], pIsRemoteLocked) )
                {
                    if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_WARNING_AND_ERROR_ONLY) )
                    {
                        printf("entry overwritten by old locker\n");
                    }
                }
                else
                {
                    if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_WARNING_AND_ERROR_ONLY) )
                    {
                        printf("entry overwritten by active locker\n");
                    }
                }
            }
            break;
        }
        else if ( 0 == readResult )
        {
            *pIsRemoteLocked = false;
            this->m_LastError = "file became empty";
            if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_WARNING_AND_ERROR_ONLY) )
            {
                printf("third read lock %s file now empty\n", lockFilePath);
            }
            break;
        }
        else
        {
            *pCanRetry = CheckForRetry(errno);
            this->m_LastError = GetLastSystemErrorAsString();
            if ( !*pCanRetry || RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_ERROR_ONLY) )
            {
                if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_FATAL_ONLY) )
                {
                    printf("third read lock %s entries failed %ld:%s\n"
                        , lockFilePath
                        , (long)readResult
                        , this->m_LastError );
                }
            }
            break;
        }

        gotLock = true;
    }

    /* close if read reported other locker or is done */
    if ( lockFd >= 0 
      && 0 != close(lockFd) )
    {
        *pCanRetry = CheckForRetry(errno);
        lockFd = -1;
        if ( !*pCanRetry || RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_ERROR_ONLY) )
        {
            if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_FATAL_ONLY) )
            {
                printf("final closing lock %s failed:%s\n"
                    , lockFilePath
                    , GetLastSystemErrorAsString());
            }
        }
        gotLock = false;
    }
    else if ( lockFd >= 0 )
    {
        if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_ALL_INFOS) )
        {
            printf("closing lock %s succeeded\n"
                , lockFilePath );
        }
    }

    this->m_IsLocked = gotLock;
    return gotLock;
}

/*---------------------------------*/

static SAPDB_Bool RegistryFile_Lock(RTE_LockFile *this)
{
    if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_ALL_INFOS) )
    {
        printf("RegistryFile_Lock %s entered\n", this->m_Path);
    }
    this->m_LastError = "";
    if ( !this->m_IsLocked )
    {
        int localRetries = 0;
        int remoteRetries = 0;
        int pollRetries = 0;
        SAPDB_Bool isRemoteLocked = false;

#ifdef SDB_MULTI_THREAD
        RTE_WaitUntilAloneForLock(this->m_Path);
        if ( threadId != 0 )
        {
            if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_FATAL_ONLY) )
            {
                printf("thread %d NOT alone for %s\n", sqlgetthreadid(), this->m_Path);
            }
        }
        threadId = sqlgetthreadid();
#endif /* SDB_MULTI_THREAD */

        do {
            SAPDB_Bool canRetry = true;
            if ( RegistryFile_SimulateGotLock(this, &canRetry, &isRemoteLocked) )
            {
                if ( localRetries > 0 || remoteRetries > 0 || pollRetries > 0 )
                {
                    if ( RegistryFile_TestVerbosityFlag(SAPDB_VERBOSITY_FLAG_ACTIVITY) )
                    {
                        putchar('l');
                    }

                    if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_WARNING_AND_ERROR_ONLY) )
                    {
                        printf("RegistryFile_Lock %s succeeded after %d local retries %d remote retries %d poll retries\n"
                            , this->m_Path
                            , localRetries
                            , remoteRetries
                            , pollRetries );
                    }
                }
                else
                {
                    if ( RegistryFile_TestVerbosityFlag(SAPDB_VERBOSITY_FLAG_ACTIVITY) )
                    {
                        putchar('L');
                    }
                }

                if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_ALL_INFOS) )
                {
                    printf("RegistryFile_Lock %s succeeded\n", this->m_Path);
                }
                return true;
            }

            if ( !RTE_RetryWanted
              || !canRetry )
            {
                break;
            }

            if ( isRemoteLocked )
            {
                if ( RTE_RetryRemoteTimeout > 0 )
                {
                    if ( remoteRetries++ < RTE_RetryRemoteTimeout )
                    {
                        if ( RegistryFile_TestVerbosityFlag(SAPDB_VERBOSITY_FLAG_ACTIVITY) )
                        {
                            putchar('R');
                        }
                        SLEEP(1);
                    }
                    else
                    {
                        if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_ALL_INFOS) )
                        {
                            printf("Last lock error:%s\n", this->m_LastError);
                        }
                        this->m_LastError = "Too many local retries";
                        break;
                    }
                }
            }
            else if ( (RTE_RetryLocalTimeout+RTE_RetryPollLoops) > 0 )
            {
                remoteRetries = 0;
                if ( pollRetries++ < RTE_RetryPollLoops )
                {
                    if ( RegistryFile_TestVerbosityFlag(SAPDB_VERBOSITY_FLAG_ACTIVITY) )
                    {
                        putchar('s');
                    }
                    SLEEP(0);
                }
                else
                {
                    pollRetries = 0;
                    if ( RTE_RetryLocalTimeout > 0 )
                    {
                        if ( localRetries++ < RTE_RetryLocalTimeout )
                        {
                            if ( RegistryFile_TestVerbosityFlag(SAPDB_VERBOSITY_FLAG_ACTIVITY) )
                            {
                                putchar('S');
                            }
                            SLEEP(1);
                        }
                        else
                        {
                            if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_ALL_INFOS) )
                            {
                                printf("Last lock error:%s\n", this->m_LastError);
                            }
                            this->m_LastError = "Too many remote retries";
                            break;
                        }
                    }
                }
            }
        } while ( RTE_RetryWanted );

#ifdef SDB_MULTI_THREAD
        /* In case no locking is needed we have to release the mutex... */
        if ( !this->m_IsLocked )
        {
            if ( sqlgetthreadid() != threadId )
            {
                if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_FATAL_ONLY) )
                {
                    printf("Thread %d ??? alone for %s\n", sqlgetthreadid(), this->m_Path);
                }
            }
            threadId = 0;
            RTE_AllowNextLocker(this->m_Path);
        }
#endif /* SDB_MULTI_THREAD */

    }

    if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_ALL_INFOS) )
    {
        printf("RegistryFile_Lock %s failed\n", this->m_Path);
    }
    return false;
}

/*---------------------------------*/

static void RegistryFile_SimulateUnlock(RTE_LockFile *this)
{
    SAPDB_Char *lockFilePath = (SAPDB_Char *)alloca(strlen(this->m_Path)+strlen(SDB_LOCK_FILE_SUFFIX)+1);
    RTE_LockFileContent existingEntries[2];
#ifdef _WIN32
    int readResult;
#else
    ssize_t readResult;
#endif
    int lockFd;
    bool isRemoteLocked = false;

    sprintf(lockFilePath, "%s" SDB_LOCK_FILE_SUFFIX, this->m_Path);
#ifdef _WIN32        
    lockFd = open(lockFilePath, O_RDWR | O_BINARY);
#else
    lockFd = open(lockFilePath, O_RDWR);
#endif
    if ( lockFd < 0 )
    {
        if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_FATAL_ONLY) )
        {
            printf("open lock %s for unlock failed:%s\n", lockFilePath, GetLastSystemErrorAsString());
        }
    }
    else
    {

        readResult = read(lockFd, &existingEntries[0], 2 * sizeof(RTE_LockFileContent));
        if ( (2*sizeof(RTE_LockFileContent)) == readResult )
        {
            if ( 0 == SAPDB_memcmp(&existingEntries[1], &this->m_LockEntry, sizeof(RTE_LockFileContent)) )
            {
                if ( 0 == SAPDB_memcmp(&existingEntries[0], &this->m_LockEntry, sizeof(RTE_LockFileContent)) )
                {
                    if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_ALL_INFOS) )
                    {
                        printf("found normal locked file\n");
                    }
                }
                else
                {
                    /* if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_WARNING_AND_ERROR_ONLY) ) */
                    if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_FATAL_ONLY) ) /* XXX */
                    {
                        printf("first entry in lock %s overwritten\n", lockFilePath);
                        if ( RegistryFile_OldLocker(this, &existingEntries[1], &isRemoteLocked) )
                        {
                            printf("first entry overwritten by old locker\n");
                        }
                        else
                        {
                            printf("first entry overwritten by active %s locker\n", isRemoteLocked ? "remote" : "local");
                        }
                    }
                }
            }
            else
            {
                if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_FATAL_ONLY) )
                {
                    printf("lock brocken: second entry in lock %s overwritten\n", lockFilePath);
                    if ( RegistryFile_OldLocker(this, &existingEntries[1], &isRemoteLocked) )
                    {
                        printf("second entry overwritten by old locker\n", lockFilePath);
                    }
                    else
                    {
                        printf("second entry overwritten by active %s locker\n", isRemoteLocked ? "remote" : "local");
                    }
                }
            }
        }
        else if ( sizeof(RTE_LockFileContent) == readResult )
        {
            if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_FATAL_ONLY) )
            {
                printf("lock brocken: unlock read lock %s found only one entry\n", lockFilePath);
                if ( 0 == SAPDB_memcmp( &existingEntries[0], &this->m_LockEntry, sizeof(RTE_LockFileContent)) )
                {
                    printf("unlock read found first entry ok but second missing\n");
                }
                else
                {
                    printf("unlock read lock %s found different entry\n", lockFilePath);
                    if ( RegistryFile_OldLocker(this, &existingEntries[0], &isRemoteLocked) )
                    {
                        printf("entry overwritten by old locker\n");
                    }
                    else
                    {
                        printf("entry overwritten by active %s locker\n", isRemoteLocked ? "remote" : "local");
                    }
                }
            }
        }
        else if ( 0 == readResult )
        {
            if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_FATAL_ONLY) )
            {
                printf("lock brocken: unlock read lock %s file now empty\n", lockFilePath);
            }
        }
        else
        {
            if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_FATAL_ONLY) )
            {
                printf("lock brocken: third read lock %s entries failed %ld:%s\n"
                    , lockFilePath
                    , (long)readResult
                    , this->m_LastError );
            }
        }

#ifdef _WIN32  //TODO!!
        lseek(lockFd, SEEK_SET, (off_t)0);
        if ( sizeof(RegistryFile_Nobody) != write(lockFd, &RegistryFile_Nobody, sizeof(RegistryFile_Nobody)) )
#else
        if ( sizeof(RegistryFile_Nobody) != pwrite(lockFd, &RegistryFile_Nobody, sizeof(RegistryFile_Nobody), (off_t)0) )
#endif
        {
            if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_FATAL_ONLY) )
            {
                printf("mark lock %s as unused failed:%s\n", lockFilePath, GetLastSystemErrorAsString());
            }
        }
        else
        {
            if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_ALL_INFOS) )
            {
                printf("marked lock %s as unused\n", lockFilePath);
            }
        }
#ifndef _WIN32
        if ( 0 != ftruncate(lockFd, sizeof(RegistryFile_Nobody)) )
#else
        if ( 0 != _chsize(lockFd, sizeof(RegistryFile_Nobody)) )
#endif
        {
            if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_FATAL_ONLY) )
            {
                printf("truncate lock %s failed:%s\n", lockFilePath, GetLastSystemErrorAsString());
            }
        }
        if ( 0 != close(lockFd) )
        {
            lockFd = -1;
            if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_FATAL_ONLY) )
            {
                printf("close lock %s after marked as unused failed:%s\n", lockFilePath, GetLastSystemErrorAsString());
            }
        }
    }
    this->m_IsLocked = false;
}

/*---------------------------------*/

static SAPDB_Bool RegistryFile_Unlock(RTE_LockFile *this)
{
    if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_ALL_INFOS) )
    {
        printf("RegistryFile_Unlock %s entered\n", this->m_Path);
    }
    if ( this->m_IsLocked )
    {
        RegistryFile_SimulateUnlock(this);

#ifdef SDB_MULTI_THREAD
        if ( sqlgetthreadid() != threadId )
        {
            if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_FATAL_ONLY) )
            {
                printf("Thread %d ??? alone for %s\n", sqlgetthreadid(), this->m_Path);
            }
        }
        threadId = 0;
        RTE_AllowNextLocker(this->m_Path);
#endif /* SDB_MULTI_THREAD */
    }
    if ( RegistryFile_TestVerbosityLevel(SAPDB_VERBOSITY_LEVEL_ALL_INFOS) )
    {
        printf("RegistryFile_Unlock %s done\n", this->m_Path);
    }
    return true;
}

/*---------------------------------*/

static SAPDB_Bool RegistryFile_Reset(RTE_LockFile *this)
{
    if ( !this->m_IsOpen
      || lseek(this->Fd, (off_t)0, SEEK_SET) == -1 ) /* CTS 1108386 */
    {
        return false;
    }
    return true;
}

/*---------------------------------*/

static void RegistryFile_Close(RTE_LockFile *this)
{
    if ( this->m_IsOpen )
    {
        if ( this->m_IsLocked )
        {
            RegistryFile_Unlock(this);
        }            
        close(this->Fd);
        this->m_IsOpen = false;
    }

    if ( this->m_IsTemp )
    {
#ifdef _WIN32
        my_save_chmod(this->m_Path, 0644);  /* */
#endif
        unlink(this->m_Path);
    }
}

/*------------------------------------*/

static RTE_RegistryHandle OpenConfigFileForEnumeration (
    RTE_RegistryHandle hEnum,
    const SAPDB_Char  *szPath,
    SAPDB_Int4         readOnly,
    const SAPDB_Char  *szSection,
    tsp00_ErrTextc     ErrText, 
    RTE_IniFileResult *pOk )
{
    SAPDB_Bool      gotResultHandle = false;
    SAPDB_Int4      ReadResult;
    RTE_LockFile        RegistryFile;

    RegistryFile_Init(&RegistryFile, false, true);

    while ( !gotResultHandle )
    {
        if ( !RegistryFile_Open(&RegistryFile, szPath) )
        {
            *pOk = SAPDB_INIFILE_RESULT_ERR_OPEN;
            strcpy( (SAPDB_Char *)ErrText, "Open Registry:" );
            strncat( (SAPDB_Char *)ErrText, GetLastSystemErrorAsString(), sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
            break;
        }

        if ( !RegistryFile_Lock(&RegistryFile) )
        {
            *pOk = SAPDB_INIFILE_RESULT_ERR_LOCK;
            strcpy( (SAPDB_Char *)ErrText, "Lock Registry:" );
            strncat( (SAPDB_Char *)ErrText, RegistryFile.m_LastError, sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
            break;
        }

        ReadResult = FindSection (RegistryFile.Fd, szSection);
        if ( ReadResult == -1 )
        {
            *pOk = SAPDB_INIFILE_RESULT_ERR_READ;
            strcpy( (SAPDB_Char *)ErrText, "Read Registry:");
            strncat( (SAPDB_Char *)ErrText, GetLastSystemErrorAsString(), sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
            break;
        }
        if ( ReadResult == 0) 
        {
            *pOk = SAPDB_INIFILE_RESULT_NO_ENTRY;
            strcpy( (SAPDB_Char *)ErrText, "Can't find Registry section:" );
            strncat( (SAPDB_Char *)ErrText, szSection, sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
            break;
        }

        ErrText[0] = '\0';
        *pOk = SAPDB_INIFILE_RESULT_OK;

        hEnum->buffer = CopySection(RegistryFile.Fd);
        if ( !hEnum->buffer )
        {
            strcpy( (SAPDB_Char *)ErrText, "Memory copy of section [" );
            strncat( (SAPDB_Char *)ErrText, szSection, sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
            strncat( (SAPDB_Char *)ErrText, "]", sizeof(tsp00_ErrTextc) - strlen((SAPDB_Char *)ErrText) - 1 );
            *pOk = SAPDB_INIFILE_RESULT_ERR_MEMORY;
            break;
        }

        gotResultHandle = true;
    }

    RegistryFile_Close(&RegistryFile);

    if ( !gotResultHandle )
    {
        free(hEnum);
        return 0;
    }

    hEnum->nextEntry = hEnum->buffer;
    return hEnum;
}

/*---------------------------------------------------*/

static SAPDB_Char const *GetLastSystemErrorAsString()
{
	/* imported data */
	if ( errno == 0 )
	{
		return ( "NO ERROR(0)" );
	}
#if defined(SUN) || defined(SOLARIS) || defined(HPUX) || defined(LINUX) || defined(FREEBSD)
	{
		SAPDB_Char const *ErrorMessage = (SAPDB_Char const *)strerror(errno);
		if ( ErrorMessage == NULL )
		{
			ErrorMessage = "errno unknown";
		}
		return (SAPDB_Char *)ErrorMessage;
	}
#else
    if ( (errno < 1) || (errno > sys_nerr) )
	{
		return ( "ERRNO NOT IN sys_errlist[]" );
	}
	return ( (SAPDB_Char *)sys_errlist [ errno ] );
#endif
}

/*-------------------*/

static SAPDB_Bool myGetEnv  ( SAPDB_Char        *EnvName,
                              SAPDB_Char        *EnvValue,
                              SAPDB_Int4          MaxEnvValueLen)
{
  SAPDB_Bool  Ok = MaxEnvValueLen > 0 ;
  
  if ( Ok ) 
  {
    SAPDB_Char       *PtrToEnvValue = getenv(EnvName);
    
    Ok = PtrToEnvValue != NULL ;
    
    if ( Ok )
    {
      if ( (SAPDB_Int4)strlen(PtrToEnvValue) < MaxEnvValueLen)
        strcpy(EnvValue, PtrToEnvValue);
      else
      {
        SAPDB_strncpy_zero_terminated(EnvValue, PtrToEnvValue, MaxEnvValueLen);
        Ok = false ;
      }
    }
    else
      *EnvValue = '\0' ;
  }
  return (Ok) ;
}

/*--------------------*/

static int my_save_chmod(SAPDB_Char const *path, SAPDB_Int4 newMode)
{
   int result;
   do {
        result = chmod(path, newMode);
   } while ( -1 == result && errno == EINTR );
   return result;
}


#if !defined(WIN32)
/* ------------------------------------------------------------------
 * STATIC FUNCTION RTE_GetUserSpecificPath
 * ------------------------------------------------------------------
 * purpose:   Gets the user specific paths for configuration data 'Application Data' or documents 
 * 'My Documents' respectively.
 *
 * If the environment variable "SDB_HOMEDIRECTORY" for local 'Application Data',
 *     "SDB_GLOBAL_APPLICATION_DATA" for global 'Application Data' or "SDB_MY_DOCUMENTS" 
 *     for 'My Documents' is found and a directory with such name is accessible, it is used. 
 *     If it is not accessible, the call fails.
 * Otherwise if a the subdirectory ".sdb", ".sdb/<machine>" or "sdb/<machine>" in the home  
 *     directory is accessable it is used as path for the global or local 'Application Data' 
 *     or 'My Documents' folder respectively. 
  * NOTE If the directory does not exist this call will try to create it.
 *     On UNIX it does this with mode 0777, so use the current 'umask' setting
 *     if further restriction is wanted. Default should be umask 022.
 *     On Windows the CreateDirectory() with the default security descriptor
 *     NULL is used.
 *
 *   homeSubdirectory (in)      - SAPDB_APPLICATION_DATA_SUBDIRECTORY or
 *                                SAPDB_HOME_SUBDIRECTORY
 *   roaming (in)               - true: Data path for roaming applications is wanted
 *                              - false: Data path for local (nonroaming) applications is wanted
 *   ConfigPath (out)           - Path of the home directory requested
 *                                WrkPath is a C ( null terminated ) string
 *   TerminateWithDelimiter (in)- TERM_WITHOUT_DELIMITER_EO01 
 *                                TERM_WITH_DELIMITER_EO01 
 *   ErrText (out)              - Error message if any
 * return - true if call succeeded, false else (ErrText filled)
 */
static SAPDB_Bool RTE_GetUserSpecificPath (SAPDB_Char const * const        homeSubdirectory,
                                           SAPDB_Bool                      roaming,
                                           RTE_Path                        ConfigPath,
                                           SAPDB_Bool                      TerminateWithDelimiter,
                                           tsp00_ErrTextc                  ErrText )
{
    SAPDB_Char *    pPath = &ConfigPath[0];
    RTE_Path        envName;


    SAPDB_memset(pPath, 0, sizeof(RTE_Path));
    SAPDB_memset(ErrText, 0, sizeof(tsp00_ErrTextc));

    if (NULL == homeSubdirectory || '\0' == homeSubdirectory[0])
    {
        eo46BuildErrorStringRC (ErrText, ERRMSG_SYS_HOME_SUBDIRECTORY_UNKNOWN, 0);
        return false;
    }

    if (0 == strcmp (homeSubdirectory, SAPDB_APPLICATION_DATA_SUBDIRECTORY))
    {
        if (roaming)
        {
            strcpy (envName, SAPDB_GLOBAL_APPLICATION_DATA_PATH);
        }
        else
        {
            strcpy (envName, SAPDB_LOCAL_APPLICATION_DATA_PATH);
        }
    }
    else if (0 == strcmp (homeSubdirectory, SAPDB_HOME_SUBDIRECTORY))
    {
        strcpy (envName, SAPDB_MY_DOCUMENTS_PATH);
    }
    else
    {
        eo46BuildErrorStringRC (ErrText, ERRMSG_SYS_HOME_SUBDIRECTORY_UNKNOWN, 0);
        return false;
    }


    if ( !myGetEnv(envName, &ConfigPath[0], sizeof(RTE_Path) ) )
    {
        SAPDB_UInt4 neededSize = 0;
        SAPDB_Char  tmpBuffer[2];
        SAPDB_UInt4 lengthForNodeName = 0;
        struct utsname unam;

        if (!roaming)
        {
            if ( uname ( &unam ) < 0 )
            {
	            strcpy(ErrText, "Cant get hostname by uname");
	            return false;
            }

            lengthForNodeName = strlen ( unam.nodename ) + strlen("/");
        }

        if ( lengthForNodeName + (strlen(homeSubdirectory) + 2*strlen("/")) >= sizeof(RTE_Path))
        {
            strcpy(ErrText, "Hostname too long");
            return false;
        }

        if ( !RTE_GetHomeDirectoryFromPasswdById( (SAPDB_Int4)geteuid ()
                                                , pPath
                                                , sizeof(RTE_Path) - ( strlen(homeSubdirectory) + lengthForNodeName + 2 * strlen("/") )
                                                , &neededSize ) )
        {
            if ( 0 == neededSize )
            {
                strcpy(ErrText, "Found no home directory entry");
            }
            else
            {
                strcpy(ErrText, "Path to sdb subdirectory too long");
            }
            return false;
        }

        strcat( pPath, "/");
        strcat( pPath, homeSubdirectory );


        if ( 0 != access(pPath, R_OK|W_OK) )
        {
            /* Use external umask if you want to restrict this... */
            if ( 0 != mkdir(pPath, 0777)  )
            {
                strcpy(ErrText, "Cannot create sdb user subdirectory"); /* size of string no longer exceeds size of ErrText... */
                return false;
            }
        }

        if (0 != lengthForNodeName)
        {
            strcat( pPath, "/" );
            strcat( pPath, unam.nodename );

            if ( 0 != access(pPath, R_OK|W_OK) )
            {
                /* Use external umask if you want to restrict this... */
                if ( 0 != mkdir(pPath, 0777)  )
                {
                    strcpy(ErrText, "Cannot create sdb machine subdirectory"); /* size of string no longer exceeds size of ErrText... */
                    return false;
                }
            }
        }
    }

    if ( 0 != access(pPath, R_OK|W_OK) )
    {
        strcpy(ErrText, "Failed to access directory");
        return false;
    }

    return TrimDelimiter(pPath, TerminateWithDelimiter);
}
#endif

/*-----------------------------------*/
/*  End of UNIX only static routines */
/*-----------------------------------*/

#endif

/*-------------------------*/
/*  Common static routines */
/*-------------------------*/

static SAPDB_Bool TrimDelimiter(RTE_Path path,
                                SAPDB_Int4 forceDelimiter)
{
#ifdef _WIN32
    char pathsep = '\\';   
#else
    char pathsep = '/';   
#endif
    SAPDB_Int4 pathLen = strlen(path);
    SAPDB_Int4 hasDelimiter = ( path[pathLen - 1] == pathsep );

    if ( forceDelimiter )
    {
        if ( hasDelimiter )
        {
            while ( pathLen >= 2 
               && path[pathLen - 2] == pathsep )
            {
                --pathLen;
                path[pathLen] = 0;
            }
        }
        else
        {
            if ( pathLen >= sizeof(RTE_Path) )
            {
                return false;
            }
            path[pathLen] = pathsep;
            path[pathLen+1] = 0;
        }
    }
    else
    {
        if ( hasDelimiter )
        {
            while ( pathLen >= 1 
               && path[pathLen - 1] == pathsep )
            {
                --pathLen;
                path[pathLen] = 0;
            }
        }
    }

    return true;
}

/*--------------------------------*/
/*  End of common static routines */
/*--------------------------------*/

#if defined(WIN32)

/*-----------------------------*/
/*  WIN32 only static routines */
/*-----------------------------*/
static SAPDB_Int4 RTE_GetWindowsConfigString (
                        SAPDB_Char const *      accountName,
                        SAPDB_Bool              isUserSpecific,
                        SAPDB_Bool              wantDefaultString,
                        SAPDB_Char const *      szSection,
                        SAPDB_Char const *      szEntry, 
                        SAPDB_Char *            szString,
                        SAPDB_Int4 const        MaxStringLen,
                        tsp00_ErrTextc      ErrText,
                        RTE_IniFileResult *     pOk )
{

    RTE_SystemRc        rc = RTE_SYSTEMRC_NO_ERROR;
    SAPDB_Int4          result;
    REG_ENTRY_REC       RegistryEntries[1];
    SAPDB_Char *        pTextualSID = NULL;
    SAPDB_Char *        szSubKey;
    SAPDB_Char const *  szValue;
    SAPDB_UInt4         lenSubKey;


    if (!szSection )
    {
        *pOk = SAPDB_INIFILE_RESULT_ERR_PARAM;
        strcpy ( (SAPDB_Char *)ErrText, "NULL pointer for section passed" );
        return 0;
    }

    if ( MaxStringLen > 0 && !szString )
    {
        *pOk = SAPDB_INIFILE_RESULT_ERR_PARAM;
        strcpy ( (SAPDB_Char *)ErrText, "NULL pointer for string passed" );
        return 0;
    }

    if ( MaxStringLen > 0 )
    {
        *szString = 0; /* If an empty string is found, the RegQueryValueEx does not return zero termination... */
    }

    lenSubKey = strlen(szSection) + 1;
    if (isUserSpecific)
    {
        rc = sql49c_get_user_textual_SID ((SAPDB_Char *)accountName, &pTextualSID);
        if (RTE_SYSTEMRC_NO_ERROR != rc)
        {
            eo46BuildErrorStringRC ( ErrText, ERRMSG_SEC_GET_USER_SID, rc);
            *pOk = SAPDB_INIFILE_RESULT_ERR_GET_SID;
            return 0;
        }

        lenSubKey += (strlen (pTextualSID) + 1);
    }


    if ( wantDefaultString )
    {
        /* value is not stored as as string value, but use the default string of the key.
           This is the only way to enumerate without broken fingers */
        lenSubKey += (strlen(szEntry) + 1);

        szSubKey = (SAPDB_Char *)alloca(lenSubKey);
        strcpy(szSubKey, szSection);
        strcat(szSubKey, "\\");
        if (isUserSpecific)
        {
            strcat(szSubKey, pTextualSID);
            strcat(szSubKey, "\\");
        }

        strcat(szSubKey, szEntry);
        szValue = "";
    }
    else
    {

        szSubKey = (SAPDB_Char *)alloca(lenSubKey);
        strcpy ( szSubKey, szSection );
        if (isUserSpecific)
        {
            strcat(szSubKey, "\\");
            strcat(szSubKey, pTextualSID);
        }

        szValue = szEntry;
    }

    if (isUserSpecific)
    {
        FREE_MEM ( pTextualSID );
    }


    RegistryEntries[0].pszValueName = (SAPDB_Char *)szValue;
    RegistryEntries[0].pValue       = szString;
    RegistryEntries[0].ulValueSize  = MaxStringLen;
    RegistryEntries[0].ulValueType  = REG_EXPAND_SZ;

    rc = sql50_reg_get_applic_values ( NULL, HKEY_LOCAL_MACHINE, szSubKey, 1,
        RegistryEntries );

    if (RTE_SYSTEMRC_NO_ERROR == rc)
    {
        *pOk = SAPDB_INIFILE_RESULT_OK;
        result = strlen(szString);
    }
    else
    {
        strcpy((SAPDB_Char *)ErrText, "Can't read registry key");
        *pOk = SAPDB_INIFILE_RESULT_ERR_READ;
        result = 0;
    }

    return result;
}

/*---------------------------------------------------*/
static SAPDB_Int4 RTE_PutWindowsConfigString (
                        SAPDB_Char const *      accountName,
                        SAPDB_Bool              isUserSpecific,
                        SAPDB_Bool              wantDefaultString,
                        SAPDB_Char const *      szSection,
                        SAPDB_Char const *      szEntry, 
                        SAPDB_Char const *      szString,
                        tsp00_ErrTextc      ErrText,
                        RTE_IniFileResult *     pOk )
{
    RTE_SystemRc        rc = RTE_SYSTEMRC_NO_ERROR;
    REG_ENTRY_REC       RegistryEntries[1];
    SAPDB_Char *        pTextualSID = NULL;
    SAPDB_Char *        pSubSubKey = NULL;
    SAPDB_Int4          ret = 1;


    *pOk = SAPDB_INIFILE_RESULT_OK;

    if (isUserSpecific)
    {
        rc = sql49c_get_user_textual_SID ((SAPDB_Char *)accountName, &pTextualSID);
        if (RTE_SYSTEMRC_NO_ERROR != rc)
        {
            eo46BuildErrorStringRC ( ErrText, ERRMSG_SEC_GET_USER_SID, rc);
            *pOk = SAPDB_INIFILE_RESULT_ERR_GET_SID;
            return 0;
        }
    }

    if ( wantDefaultString )
    {
        /* value is not stored as as string value, but use the default string of the key.
           This is the only way to enumerate without broken fingers */
        RegistryEntries[0].pszValueName = NULL;
        pSubSubKey = (SAPDB_Char *)szEntry;
    }
    else
    {
        RegistryEntries[0].pszValueName = (SAPDB_Char *)szEntry;
    }

    RegistryEntries[0].pValue       = (SAPDB_Char *)szString;
    RegistryEntries[0].ulValueSize  = (ULONG) strlen(szString);
    RegistryEntries[0].ulValueType  = REG_EXPAND_SZ;

    rc = sql50_reg_put_applic_values (NULL, (SAPDB_Char *)accountName, HKEY_LOCAL_MACHINE,
            (SAPDB_Char *)szSection, pTextualSID, pSubSubKey,  
            1, RegistryEntries );
    if (RTE_SYSTEMRC_NO_ERROR != rc)
    {
        eo46BuildErrorStringRC ( ErrText, ERRMSG_REG_WRITE_VALUE, rc);
        *pOk = SAPDB_INIFILE_RESULT_ERR_WRITE;
        ret = 0;
    }
    else if (RTE_SYSTEMRC_NO_ERROR != sql50_SetKeySecurity ((SAPDB_Char *)accountName,
                                                              HKEY_LOCAL_MACHINE,
                                                              (SAPDB_Char *)szSection, 
                                                              pTextualSID,
                                                              pSubSubKey))
    {
        eo46BuildErrorStringRC ( ErrText, ERRMSG_REG_SET_SECURITY, rc);
        *pOk = SAPDB_INIFILE_RESULT_SET_SECURITY_FAILED;
        ret = 0;
    }

    if (isUserSpecific)
    {
        FREE_MEM ( pTextualSID );
    }

    return ret;
}

/*---------------------------------------------------*/
static SAPDB_Int4 RTE_RemoveWindowsConfigString (
                        SAPDB_Char const *    accountName,
                        SAPDB_Bool            isUserSpecific,
                        SAPDB_Bool            wantDefaultString,
                        const SAPDB_Char *    szSection,
                        const SAPDB_Char *    szEntry, 
                        tsp00_ErrTextc    ErrText,
                        RTE_IniFileResult *   pOk )
{
    RTE_SystemRc        rc = RTE_SYSTEMRC_NO_ERROR;
    SAPDB_Char *        pTextualSID = NULL;
    SAPDB_Char *        szSubKey;
    SAPDB_UInt4         lenSubKey;


    *pOk = SAPDB_INIFILE_RESULT_OK;

    lenSubKey = strlen(szSection) + 1;
    if (isUserSpecific)
    {
        rc = sql49c_get_user_textual_SID ((SAPDB_Char *)accountName, &pTextualSID);

        if (RTE_SYSTEMRC_NO_ERROR != rc)
        {
            eo46BuildErrorStringRC ( ErrText, ERRMSG_SEC_GET_USER_SID, rc);
            *pOk = SAPDB_INIFILE_RESULT_ERR_GET_SID;
            return 0;
        }

        lenSubKey += (strlen (pTextualSID) + 1);
    }

    if ( wantDefaultString )
    {
        lenSubKey += (strlen (szEntry) + 1);
    }


    szSubKey = (SAPDB_Char *)alloca(lenSubKey);
    strcpy ( szSubKey, szSection );
    if (isUserSpecific)
    {
        strcat ( szSubKey, "\\" );
        strcat ( szSubKey, pTextualSID );
        FREE_MEM (pTextualSID);
    }

    if ( wantDefaultString )
    {
        strcat ( szSubKey, "\\" );
        strcat ( szSubKey, szEntry );
        rc = sql50_reg_del_applic_key (NULL, 
            HKEY_LOCAL_MACHINE, 
            szSubKey);
        if (RTE_SYSTEMRC_NO_ERROR != rc)
        {
            eo46BuildErrorStringRC (ErrText, ERRMSG_REG_REMOVE_KEY, rc);
        }
    }
    else
    {
        rc = sql50_reg_del_applic_value (NULL, 
            HKEY_LOCAL_MACHINE,
            szSubKey, 
            (SAPDB_Char *)szEntry);
        if (RTE_SYSTEMRC_NO_ERROR != rc)
        {
            eo46BuildErrorStringRC (ErrText, ERRMSG_REG_REMOVE_VALUE, rc);
        }
    }

    if (RTE_SYSTEMRC_NO_ERROR != rc)
    {
        *pOk = SAPDB_INIFILE_RESULT_ERR_REMOVE;
        return 0;
    }

    return 1;

}

/*---------------------------------------------------*/
static RTE_RegistryHandle RTE_OpenWindowsConfigEnum (SAPDB_Char const * accountName,
                                                     SAPDB_Bool         isUserSpecific,
                                                     RTE_RegistryHandle hEnum,
                                                     SAPDB_Char const * szSection,
                                                     tsp00_ErrTextc ErrText,
                                                     RTE_IniFileResult * pOk )
{

    RTE_SystemRc        rc = RTE_SYSTEMRC_NO_ERROR;
    SAPDB_Char *        pTextualSID;
    SAPDB_Char *        szSubKey;
    SAPDB_UInt4         lenSubKey;


    *pOk = SAPDB_INIFILE_RESULT_OK;
    lenSubKey = strlen(szSection) + 1;
    hEnum->Index = 0;

    if (isUserSpecific)
    {
        rc = sql49c_get_user_textual_SID ((SAPDB_Char *)accountName, &pTextualSID);
        if (RTE_SYSTEMRC_NO_ERROR != rc)
        {
            eo46BuildErrorStringRC ( ErrText, ERRMSG_SEC_GET_USER_SID, rc);
            *pOk = SAPDB_INIFILE_RESULT_ERR_GET_SID;
            return 0;
        }

        lenSubKey += (strlen (pTextualSID) + 1);
    }

    szSubKey = (SAPDB_Char *)alloca(lenSubKey);
    strcpy ( szSubKey, szSection );

    if (isUserSpecific)
    {
        strcat ( szSubKey, "\\" );
        strcat ( szSubKey, pTextualSID );
        FREE_MEM (pTextualSID);
    }

    rc = sql50_RegistryEnumOpen (HKEY_LOCAL_MACHINE,
                                 szSubKey,
                                &hEnum->hkResult);
    if (RTE_SYSTEMRC_NO_ERROR != rc)
    {
        eo46BuildErrorStringRC ( ErrText, ERRMSG_REG_ENUM_OPEN, rc);
        *pOk = SAPDB_INIFILE_RESULT_ERR_ENUM_OPEN;
        return 0;
    }

    return hEnum;
}

/*---------------------------------------------------*/
static SAPDB_Int4 RTE_CloseWindowsConfigEnum ( RTE_RegistryHandle hEnum,
                                               tsp00_ErrTextc   ErrText,
                                               RTE_IniFileResult * pOk )
{
    RTE_SystemRc        rc = RTE_SYSTEMRC_NO_ERROR;

    rc = sql50_RegistryClose (hEnum->hkResult);
    if (RTE_SYSTEMRC_NO_ERROR == rc)
    {
        *pOk = SAPDB_INIFILE_RESULT_OK;
        return 1;
    }

    strcpy((SAPDB_Char *)ErrText, "Can't close registry");
    *pOk = SAPDB_INIFILE_RESULT_ERR_CLOSE;
    return 0;
}
/*---------------------------------------------------*/
static SAPDB_Int4 RTE_NextWindowsConfigEnum( RTE_RegistryHandle hEnum,
                                             SAPDB_Char *szEntry,
                                             const SAPDB_Int4 MaxEntryLen,
                                             SAPDB_Char *szString,
                                             const SAPDB_Int4 MaxStringLen,
                                             tsp00_ErrTextc ErrText,
                                             RTE_IniFileResult * pOk )
{
    RTE_SystemRc        rc = RTE_SYSTEMRC_NO_ERROR;

    rc = sql50_RegistryEnumNext (hEnum->hkResult, &hEnum->Index, szEntry, MaxEntryLen, szString, MaxStringLen);
    if (RTE_SYSTEMRC_NO_ERROR == rc)
    {
        *pOk = SAPDB_INIFILE_RESULT_OK;
        return 1;
    }
    else if ( rc == ERROR_NO_MORE_ITEMS )
    {
        strcpy( ErrText, "End of Registry" );
        *pOk = SAPDB_INIFILE_RESULT_EOF;      
        return 0;
    }
    else if (rc == ERROR_FILE_NOT_FOUND)
    {
        strcpy( ErrText, "Found user entry" );
        *pOk = SAPDB_INIFILE_RESULT_FILE_NOT_FOUND;      
        return 0;
    }
    else if (rc == ERROR_ACCESS_DENIED)
    {
        strcpy( ErrText, "Found foreign user entry" );
        *pOk = SAPDB_INIFILE_RESULT_ACCESS_DENIED;      
        return 0;
    }

    strcpy((SAPDB_Char *)ErrText, "Cannot read from registry");
    *pOk = SAPDB_INIFILE_RESULT_ERR_READ;
    return 0;
}
/*------------------------------------*/
/*  End of WIN32 only static routines */
/*------------------------------------*/

#endif
/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
