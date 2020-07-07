/*!
  @file           vos01.c
  @author         RaymondR
  @brief          Common Environment Functions
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



\endif
*/



//
//  INCLUDE FILES
//
#include "gos00.h"
#include "heo00.h" 
#include "heo46.h"
#include "heo01.h"
#include "heo46.h"
#include "geo007_1.h"


//
//  DEFINES
//

#define MOD__  "VOS01C : "
#define MF__   MOD__"UNDEFINED"


#define SAP_PATH                         "sap\\"
#define PGM_PATH                         "pgm\\"
#define ALTERNATE_SYMBOL_PATH            "symbols"
#define ALTERNATE_SYMBOL_PATH_ENV_VAR    "_NT_ALT_SYMBOL_PATH"

#if defined(OMSTST)
# define ALTERNATE_SYMBOL_PATH_OMSTST     "symbols\\test"
#endif

//
//  MACROS
//


//
//  LOCAL TYPE AND STRUCT DEFINITIONS
//


//
//  EXTERNAL VARIABLES
//


//
//  EXPORTED VARIABLES
//


//
//  LOCAL VARIABLES
//
static PSZ         pszDBRoot    = NULL;
static SQL_DBNAMEC szServerDB;
static tsp00_Name    szDBLang ;

//
//  LOCAL FUNCTION PROTOTYPES
//


//
// ========================== GLOBAL FUNCTIONS ================================
//


BOOL sql01c_get_serverdb ( PSZ *ppszServerDB  )
  {
  #undef  MF__
  #define MF__ MOD__"sql01c_get_serverdb"

  APIRET  rc          = NO_ERROR;
  PSZ     pszServerDB = NULL;


  DBGIN;

  if ( pszServerDB != NULL )
    {
    *ppszServerDB = szServerDB;

    DBGOUT;
    return ( TRUE );
    }
  else
    {
    rc = GETENV( SERVERDB_ENV_VAR, &pszServerDB );

    if ( (rc == NO_ERROR) && ( *pszServerDB ))
      {
      if ( !BLANKS_ONLY(pszServerDB))
        {
        strncpy ( szServerDB, pszServerDB, sizeof(szServerDB) - 1 );
        strupr  ( szServerDB );
        *ppszServerDB = szServerDB;

        DBGOUT;
        return ( TRUE );
        }
      }
   }

  *ppszServerDB = NULL;

  DBGOUT;
  return ( FALSE );
  }

/*------------------------------*/

BOOL sql01c_get_dbroot ( PSZ *ppszDBRoot  )
  {
  #undef  MF__
  #define MF__ MOD__"sql01c_get_dbroot"

  APIRET  rc = NO_ERROR;
  ULONG   ulDBRootLen;
  ULONG   i;


  DBGIN;

  if ( pszDBRoot != NULL )
    {
    *ppszDBRoot = pszDBRoot;

    DBGOUT;
    return ( TRUE );
    }
  else
    {
    rc = GETENV( DBROOT_ENV_VAR, &pszDBRoot );

    if ( (rc == NO_ERROR) && ( *pszDBRoot ))
      {
      if ( !BLANKS_ONLY(pszDBRoot) )
        {
        *ppszDBRoot = pszDBRoot;
        ulDBRootLen = (ULONG)strlen(pszDBRoot);

        for ( i = 0; i < ulDBRootLen; i++ )
          {
          if ( pszDBRoot[i] == '/' )
            pszDBRoot[i] = '\\';
          }

        if ( pszDBRoot[ulDBRootLen - 1] == '\\')
          {
          pszDBRoot[ulDBRootLen - 1] = '\0';
          }

        DBGOUT;
        return ( TRUE );
        }
      }
    }

  *ppszDBRoot = NULL;

  DBGOUT;
  return ( FALSE );
  }

/*------------------------------*/

BOOL sql01c_get_dblang ( PSZ *ppszDBLang  )
  {
  #undef  MF__
  #define MF__ MOD__"sql01c_get_dblang"

  APIRET  rc          = NO_ERROR;
  PSZ     pszDBLang   = NULL;


  DBGIN;

  if ( pszDBLang != NULL )
    {
    *ppszDBLang = szDBLang;

    DBGOUT;
    return ( TRUE );
    }
  else
    {
    rc = GETENV( DBLOCALE_ENV_VAR, &pszDBLang );

    if ( rc == ERROR_ENVVAR_NOT_FOUND )
      rc = GETENV( "DBLANG", &pszDBLang ); // old DBLANG

    if ( (rc == NO_ERROR) && ( *pszDBLang ))
      {
      if ( !BLANKS_ONLY(pszDBLang))
        {
        strncpy ( szDBLang, pszDBLang, sizeof(szDBLang) - 1 );
        strupr  ( szDBLang );
        *ppszDBLang = szDBLang;

        DBGOUT;
        return ( TRUE );
        }
      }
    }

  *ppszDBLang = NULL;

  DBGOUT;
  return ( FALSE );
  }


/*------------------------------*/

BOOL sql01c_set_symbol_path ( PSZ szDBRoot )
  {
  #undef  MF__
  #define MF__ MOD__"sql01c_set_symbol_path"

  static char      szSymbolPathEnv[1024];
  PATHNAME         szTmpDBRoot;
  PSZ              *pszDBRoot ;
  ULONG            ulLen ;
  tsp00_Pathc      szSAPPath;
  tsp01_RteError   rteError;

  if ( szDBRoot )
    pszDBRoot = &szDBRoot ;
  else
    { pszDBRoot = (PSZ *)(&szTmpDBRoot) ;
      if ( !sql01c_get_dbroot ( pszDBRoot ) )
        return (FALSE) ;
    }
  if ( !sqlGetDbrootSapPath ( szSAPPath, TERM_WITH_DELIMITER_EO01, &rteError ))
    szSAPPath[0] = '\0';

  // --- set SYMBOL environment variable
  ulLen = (ULONG)(strlen(ALTERNATE_SYMBOL_PATH_ENV_VAR) +
                  1                                     +    // '='
                  strlen(*pszDBRoot)                    +
                  1                                     +    // '\\'
                  strlen(ALTERNATE_SYMBOL_PATH));

  if ( szSAPPath[0] != '\0' )
  {
    ulLen += (ULONG)(1                                   +    // ';'
                     strlen(szSAPPath));
  }

#  if defined(OMSTST)
    ulLen += (ULONG)(1                                     +    // ';'
                     strlen(*pszDBRoot)                    +
                     1                                     +    // '\\'
                     strlen(ALTERNATE_SYMBOL_PATH_OMSTST));
#  endif



  if ( ulLen < sizeof(szSymbolPathEnv) )
    {
    szSymbolPathEnv[sizeof(szSymbolPathEnv) - 1] = '\0';

    strcpy ( szSymbolPathEnv, ALTERNATE_SYMBOL_PATH_ENV_VAR );
    strcat ( szSymbolPathEnv, "=" );

#  if defined(OMSTST)
     strcat ( szSymbolPathEnv, *pszDBRoot );
     strcat ( szSymbolPathEnv, "\\" );
     strcat ( szSymbolPathEnv, ALTERNATE_SYMBOL_PATH_OMSTST );
     strcat ( szSymbolPathEnv, ";" );
#  endif

    strcat ( szSymbolPathEnv, *pszDBRoot );
    strcat ( szSymbolPathEnv, "\\" );
    strcat ( szSymbolPathEnv, ALTERNATE_SYMBOL_PATH );

    if ( szSAPPath[0] != '\0' )
    {
      int Len = (int)strlen(szSAPPath);
      if ( szSAPPath[Len-1] == '\\' )
        szSAPPath[Len-1] = '\0';
      strcat ( szSymbolPathEnv, ";" );
      strcat ( szSymbolPathEnv, szSAPPath );
    }

   _putenv ( szSymbolPathEnv );
   }

  DBGOUT;
  return ( TRUE );
  }

/*------------------------------*/

boolean os01_SetIndependentPath ( PSZ             pszValueName,
                                  tsp00_Pathc     pszPath,
                                  tsp01_RteError  *RteError )
  {
  #undef  MF__
  #define MF__ MOD__"os01_SetIndependentPath"

  #if defined(_WIN32)
   APIRET                    rc = NO_ERROR;
   REG_ENTRY_REC             RegistryEntries[1];
  #endif

  DBGIN;

  RegistryEntries[0].pszValueName = pszValueName ;
  RegistryEntries[0].pValue       = pszPath;
  RegistryEntries[0].ulValueSize  = (ULONG)strlen(pszPath);
  RegistryEntries[0].ulValueType  = REG_EXPAND_SZ;

  rc = sql50_reg_put_applic_values ( NULL, NULL, HKEY_LOCAL_MACHINE,
                                     NULL, NULL, NULL, 1, RegistryEntries );


  if (  rc != NO_ERROR )
    eo46_set_rte_error (RteError, rc, "Error during set registry key ", pszValueName);

  DBGOUT;
  return ( rc == NO_ERROR );
  }

/*------------------------------*/

boolean os01_GetIndependentPath ( PSZ             pszValueName,
                                  tsp00_Pathc     szPath,
                                  tsp01_RteError  *RteError )
  {
  #undef  MF__
  #define MF__ MOD__"os01_GetIndependentPath"

  APIRET                    rc = NO_ERROR;
  REG_ENTRY_REC             RegistryEntries[1];
  boolean Ok;
  tsp01_RteError dummyRteError;

  DBGIN;

  Ok = sqlGetPortableInstallationRootPath ( szPath, TERM_WITH_DELIMITER_EO01,&dummyRteError );
  if ( Ok )
      return ( Ok );

  RegistryEntries[0].pszValueName = pszValueName;
  RegistryEntries[0].pValue       = szPath;
  RegistryEntries[0].ulValueSize  = sizeof(tsp00_Pathc);
  RegistryEntries[0].ulValueType  = REG_EXPAND_SZ;

  rc = sql50_reg_get_applic_values ( NULL, HKEY_LOCAL_MACHINE,
                                     NULL, 1, RegistryEntries );

  if ( rc != NO_ERROR )
  {
    eo46_set_rte_error (RteError, rc, "Error during registry access to key ", pszValueName);
    szPath[0] = '\0' ;
  }

  DBGOUT;
  return ( rc == NO_ERROR );
  }

/*------------------------------*/

VOID sql01c_build_sap_path ( PSZ pszDBRoot, PSZ pszSAPPath )
  {

  #undef  MF__
  #define MF__ MOD__"sql01c_build_sap_path"

  DBGIN;

  strcpy (pszSAPPath, pszDBRoot);
  strcat (pszSAPPath, "\\"SAP_PATH);

  DBGOUT;
  }

/*------------------------------*/

VOID sql01c_build_pgm_path ( PSZ pszDBRoot, PSZ pszPgmPath )
  {

  #undef  MF__
  #define MF__ MOD__"sql01c_build_pgm_path"

  DBGIN;

  strcpy (pszPgmPath, pszDBRoot);
  strcat (pszPgmPath, "\\"PGM_PATH);

  DBGOUT;
  }


/* PTS 1113170 */
/*!
   @brief          This funtions determines if the running program was compiled as a 32 bit application and is
              now running on a 64 bit machine (Itanium).
   @return         True:   32 bit application on 64 bit (Itanium).
              False:  Else.

 */

extern SAPDB_Bool sql01c_Is32BitApplOn64Bit ()
{

    #undef  MF__
    #define MF__ MOD__"sql01c_Is32BitApplOnItanium"

    static  SAPDB_Bool      firstRun = true;
    static  SAPDB_Bool      is32BitApplOnItanium = false; 
    static  SAPDB_Char      envVariable[] = "ProgramFiles";

    DBGIN;

    
#if defined (_WIN32) && !defined (_WIN64)
    /* We have to detect on which platform we are running */
    if (firstRun)
    {
        SAPDB_Char  progdir[80];
        DWORD       len;

        len = GetEnvironmentVariable (envVariable, progdir, sizeof (progdir));
        if (len > 0 && len < sizeof (progdir))
        {
            if (NULL != strstr(progdir,"(x86)"))
            {
                is32BitApplOnItanium = true;
            }
        }
        else if (len == 0)
        {
            MSGD ((INFO_SYS_ENV_VARIABLE_NOT_FOUND, envVariable));
            MSGD ((INFO_SYS_ASSUME_32_BIT_PLATFORM));  
        }
        else
        {
            MSGD ((INFO_SYS_BUFFER_TO_SMALL, envVariable));
            MSGD ((INFO_SYS_ASSUME_32_BIT_PLATFORM));
        }

        firstRun = false;
    }
#endif
  
    return (is32BitApplOnItanium); 


    DBGOUT;
}


//
// ========================== LOCAL FUNCTIONS =================================
//


//
// =============================== END ========================================
//

