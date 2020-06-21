/*!
  @file           vos97c.c
  @author         RaymondR
  @brief          show
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




/*
 * INCLUDE FILES
 */
#include "gos00.h"
#include "heo46.h"
#include "gsp09.h"
#include "geo007_2.h"
#include "heo01.h"

#include "RunTime/RTE_GetDBRootOfDB.h"
#include "SAPDBCommon/SAPDB_string.h"
#include "SAPDBCommon/SAPDB_sprintf.h"

#if defined (CTRLCOMP)
# include "heo05.h"       /* nocheck */
# include "vsp004.h"      /* nocheck */
#endif


#include       <stdarg.h>

/*
 *  DEFINES
 */
#define MOD__  "VOS97CC : "
#define MF__   MOD__"UNDEFINED"


#define OPTION_STRING         "rvn:A"
#define SHOW_VERSION          1
#define SHOW_DBROOT           2
#define SHOW_ACCOUNT          3

/*
 *  MACROS
 */

#if defined (ENUMINST_ONLY)
/* uses stdmalloc, otherwise, most of RTE has to be included */
#undef ALLOC_MEM
#define ALLOC_MEM(target, size)    ((*(target) = malloc (size)) == NULL)
#undef FREE_MEM
#define FREE_MEM(ptr)              free (ptr);
#endif


/*
 *  LOCAL TYPE AND STRUCT DEFINITIONS
 */

typedef struct DBEnumT {
    BOOLEAN fullInfo;
    BOOLEAN isOpen;
    INT1    dbKind;
    INT1    filler;
} DBEnumT;

typedef struct InstallationEnumT {
    HKEY    keyHandle;
    DWORD   index;
    BOOLEAN isOpen;
    INT1    dbKind;
    INT2    filler;
} InstallationEnumT;

/*
 * EXTERNAL VARIABLES
 */
#if defined(_WIN32)
 extern int    sql80_OptInd;
 extern int    sql80_OptErr;
 extern char*  sql80_OptArg;
#endif


/*
 *  EXPORTED VARIABLES
 */


/*
 * LOCAL VARIABLES
 */

/*
 * LOCAL FUNCTION PROTOTYPES
 */

static APIRET sql97c_show_dbs_gws   ( PCHAR    pszNodeName, ULONG ulShow );
static APIRET sql97c_show_server    ( PCHAR    pszNodeName, ULONG ulShow );

#if defined (_WIN32)
 static PSZ    sql97c_get_db_gw_version    ( PSZ                   pszNodeName,
                                             ULONG                 ulServerType,
                                             PSZ                   pszServerDB,
                                             tsp9_release_version* binaryVersion );
 static PSZ    sql97c_get_server_version   ( PSZ                   pszNodeName );
 static PSZ    sql97c_get_DBROOT_of_server ( PSZ                   pszNodeName );
 static PSZ    sql97c_get_account_name     ( PSZ                   pszNodeName,
                                             ULONG                 ulServerType,
                                             PSZ                   pszServerDB );
 static VOID   sql97c_compress_path        ( PSZ                   pszCompPathName,
                                             PSZ                   pszPathName,
                                             ULONG                 ulCompPathLen );
 static BOOL   sql97c_newestDBRoot         ( PSZ                   pszDBRoot,
                                             PSZ                   pszWantedFile);

#endif

/*
 * ========================== GLOBAL FUNCTIONS ================================
 */

#if !defined (CTRLCOMP)

int main ( int argc, char  *argv[] )
  {
  #undef  MF__
  #define MF__ MOD__"main"
  APIRET   rc        = NO_ERROR;
  PCHAR    pszNodeName   = NULL;
  #if defined(_WIN32)
   INT     OptionChar;
   BOOL    fCommandError = FALSE;
   ULONG   ulShow    = 0;
  #endif

  DBGPAS;

  #if defined(_WIN32)
   while ( (OptionChar = sql80_GetOpt ( argc, argv, OPTION_STRING )) != -1 )
     {
     switch ( OptionChar )
       {
       case 'n' :
           pszNodeName   = sql80_OptArg;
           break;
       #if defined (_WIN32)
        case 'v' :
          if ( sql02_get_platform_id() == VER_PLATFORM_WIN32_NT )
            ulShow = SHOW_VERSION;
          else
            fCommandError = TRUE;
          break;
        case 'r' :
          if ( sql02_get_platform_id() == VER_PLATFORM_WIN32_NT )
            ulShow = SHOW_DBROOT;
          else
            fCommandError = TRUE;
          break;
        case 'A' :
          if ( sql02_get_platform_id() == VER_PLATFORM_WIN32_NT )
            ulShow = SHOW_ACCOUNT;
          else
            fCommandError = TRUE;
          break;
       #endif
       default :
           fCommandError = TRUE;
       }
     }

   if ( (argc - sql80_OptInd) || (fCommandError == TRUE) )
     {
     if ( sql02_get_platform_id() == VER_PLATFORM_WIN32_NT )
       printf (SHOW_USAGE);
     else
       printf (SHOW_USAGE_95);

     EXITPROCESS(0);
     }

   rc = sql97_update( pszNodeName );

   if ( rc != NO_ERROR )
     return ( rc );

  #endif


  rc = sql97c_show_dbs_gws( pszNodeName, ulShow );

  if ( rc == NO_ERROR )
    rc = sql97c_show_server( pszNodeName, ulShow );

  return ( rc );
  }

#else

/*------------------------------*/

#if !defined (ENUMINST_ONLY)

/*---------------------------------------------------------------------------*/

static SAPDB_Bool isRelativePath(SAPDB_Char *path)
{
#ifdef _WIN32
    return path[0]!='\\' && (path[1]!=':' || path[2]!='\\');
#else
    return path[0]!='/';
#endif
}

BOOL sql97cc_findControlServer ( PSZ            pszExecutable,// OUT
                                 PSZ            pszServerDB,  // IN
                                 PSZ            pszServerPgm, // IN
                                 PSZ            pszDBRoot,    // IN/OUT
                                 tsp00_ErrText  errText)      // OUT
{
#undef MF__
    #define MF__ MOD__"sql97cc_findControlServer"
    BOOLEAN found = FALSE;

    if ((pszServerDB != NULL) && (pszServerDB [0] != '\0'))
    {
        // first precedence: explicit dbname
        // PTS 1108841 
        found = RTE_GetDBRootOfDB( pszServerDB, pszDBRoot, sizeof(tsp00_Pathc)-strlen(pszDBRoot)) ;
        if ( found )
        {
            found = RTE_CheckIfInstallationRegistered(pszDBRoot);
        }
    }
    else if ((pszDBRoot != NULL) && (pszDBRoot [0] != '\0')) 
    {
#ifdef HAS_WORKING_INSTALLATION_DEREGISTRATION
        // second precedence: explicit dbroot
        pPlaceToBeFilled = pszDBRoot + u3RootLength;
        found = RTE_CheckIfInstallationRegistered(pszDBRoot);
#else
        found = true;
#endif
    }
    else 
    {
      // last chance: get newest installation
        found = sql97c_newestDBRoot (pszDBRoot, pszServerPgm);
    }
    if (found) 
    {
        pszExecutable[0] = 0;
        if(isRelativePath(pszDBRoot))
        {
            tsp01_RteError  rteError; 
            sqlGetPortableInstallationRootPath (    pszExecutable,
                                                    TERM_WITH_DELIMITER_EO01,
                                                    &rteError );
        }
        strcat (pszExecutable, pszDBRoot);
        strcat (pszExecutable, "\\pgm\\");
        strcat (pszExecutable, pszServerPgm);
        /* append .exe only if not already found */
        if ( strlen(pszServerPgm) <= strlen(".exe")
          || stricmp(pszServerPgm+strlen(pszServerPgm)-strlen(".exe"), ".exe") )
        {
            strcat (pszExecutable, ".exe");
        }

        /* check existence of file */
        if ( GetFileAttributes (pszExecutable) == 0xffffffff )
        {   
            SAPDB_sprintf(errText , sizeof(tsp00_ErrText), "%s executable not found",pszServerPgm);
            found = false;
        }
    }
    else
    {
        SAPDB_strncpy_zero_terminated(errText , "database instance not found", sizeof(tsp00_ErrText));
    }
    return found;
}

#endif
#endif


/*
 * ========================== LOCAL FUNCTIONS =================================
 */

#if !defined (ENUMINST_ONLY)

static APIRET sql97c_show_dbs_gws ( PSZ pszNodeName, ULONG ulShow )
  {
  #undef  MF__
  #define MF__ MOD__"sql97c_show_dbs_gws"
  APIRET                rc             = NO_ERROR;
  ULONG                 ulFound        = 0;
  BOOL                  fActive;
  SQL_DBNAMEC           szServerDB;
  PATHNAME              szDBRoot;
  PSZ                   pszVersion     = NULL;
  PSZ                   pszDBRoot      = NULL;
  ULONG                 ulServerType   = SERVER_TYPE_SERVERDB;
  PSZ                   pszAccountName = NULL;
  tsp9_release_version  BinaryVersion;

  for (;;)
    {
    ulFound = 0;

    rc = sql97_first_db_gw_state ( ulServerType, szServerDB, &fActive );

    if (( rc != ERROR_NO_MORE_FILES  ) && ( rc != NO_ERROR ))
      {
      DBGOUT;
      return ( rc );
      }


   switch(ulServerType)
     {
     case SERVER_TYPE_ADABAS_SERVERDB :    
       if ( rc == NO_ERROR ) printf ( XSHOW_ADABAS_SERVERDBS );
       break;
     case SERVER_TYPE_ADABAS_GATEWAY :
       if ( rc == NO_ERROR ) printf ( XSHOW_ADABAS_GATEWAYS );
       break;
     case SERVER_TYPE_GATEWAY :    
       if ( rc == NO_ERROR ) printf ( XSHOW_GATEWAYS );
       break;
     default:
       printf ( XSHOW_SERVERDBS );
       break;
     }

    // --- check SERVERDBs 
    while ( rc == NO_ERROR )
      {
      ulFound++;

      switch ( ulShow )
        {
        case SHOW_VERSION:
          #if defined (_WIN32)
          if ( sql02_get_platform_id() == VER_PLATFORM_WIN32_NT )
            pszVersion = sql97c_get_db_gw_version ( pszNodeName, ulServerType,
                                                    szServerDB, &BinaryVersion);
          #endif

          if ( pszVersion == NULL )
            pszVersion = "???";

          if ( fActive )
            printf ( XSHOW_SERVERDB_GW_VER_RUNNING, szServerDB, pszVersion );
          else
            printf ( XSHOW_SERVERDB_GW_VER_STOPPED, szServerDB, pszVersion );
          break;

        case SHOW_ACCOUNT:
          #if defined (_WIN32)
          if ( sql02_get_platform_id() == VER_PLATFORM_WIN32_NT )
            pszAccountName = sql97c_get_account_name ( pszNodeName, ulServerType, szServerDB );
          #endif

          if ( pszAccountName == NULL )
            pszAccountName = "???\'";
          else
            strcat ( pszAccountName, "'" );

          if ( fActive )
            printf ( XSHOW_SERVERDB_GW_ACC_RUNNING, szServerDB, pszAccountName );
          else
            printf ( XSHOW_SERVERDB_GW_ACC_STOPPED, szServerDB, pszAccountName );
          break;

        case SHOW_DBROOT:
          #if defined (_WIN32)
          if ( sql02_get_platform_id() == VER_PLATFORM_WIN32_NT )
            pszDBRoot  = RTE_getDBRootFromRegistry ( pszNodeName, ulServerType,
                                                      szServerDB );
            if ( pszDBRoot == NULL )
              pszDBRoot = "???\'";
            else
              {
              sql97c_compress_path ( szDBRoot, pszDBRoot, 40 );
              strcat ( szDBRoot, "'" );
              pszDBRoot = szDBRoot;
              }
          #else
          pszDBRoot = "???\'";
          #endif


          if ( fActive )
            printf ( XSHOW_SERVERDB_GW_DBR_RUNNING, szServerDB, pszDBRoot );
          else
            printf ( XSHOW_SERVERDB_GW_DBR_STOPPED, szServerDB, pszDBRoot );
          break;

        default:
          if ( fActive )
            printf ( XSHOW_SERVERDB_GW_RUNNING, szServerDB );
          else
            printf ( XSHOW_SERVERDB_GW_STOPPED, szServerDB );
          break;
        }

      rc = sql97_next_db_gw_state ( szServerDB, &fActive );

      if (( rc != ERROR_NO_MORE_FILES  ) && ( rc != NO_ERROR ))
        {
        DBGOUT;
        return ( rc );
        }
      }

    if (( ulFound == 0 ) && (ulServerType == SERVER_TYPE_SERVERDB ))
      printf ( XSHOW_NO_SERVERDBS_GATEWAYS );

    if ( sql02_get_platform_id() == VER_PLATFORM_WIN32_NT )
      {
      if      (ulServerType == SERVER_TYPE_SERVERDB )
        ulServerType = SERVER_TYPE_GATEWAY;
      else if (ulServerType == SERVER_TYPE_GATEWAY )
        ulServerType = SERVER_TYPE_ADABAS_SERVERDB;
      else if (ulServerType == SERVER_TYPE_ADABAS_SERVERDB )
        ulServerType = SERVER_TYPE_ADABAS_GATEWAY;
      else
        break;
      }
    else
      break;
    }


  DBGOUT;
  return ( NO_ERROR );
  }

/*------------------------------*/

static APIRET sql97c_show_server ( PSZ    pszNodeName, ULONG ulShow )
  {
  #undef  MF__
  #define MF__ MOD__"sql97c_show_server"
  APIRET    rc     = NO_ERROR;
  PSZ       pszVersion = NULL;
  PSZ       pszDBRoot  = NULL;
  PATHNAME  szDBRoot;
  BOOL      fActive;
  BOOL      fNoService;
  BOOL      fNoServiceInfoWritten = true;
  CHAR      szPort[10];

  rc = sql97_first_xserver_state ( szPort, sizeof(szPort), &fNoService, &fActive );

  if ( NO_ERROR != rc )
    {
    if ( ERROR_NO_MORE_FILES == rc )
      printf ( XSHOW_XSERVER_NOT_INSTALLED );

    DBGOUT;
    return ( rc );
    }

  if ( false == fNoService )
     printf ( XSHOW_XSERVER_SERVICES );
  
  do
    {
    if (( fNoServiceInfoWritten ) && fNoService )
      {
      fNoServiceInfoWritten = false;
      printf ( XSHOW_XSERVER_IN_USER_SESSION );
      }

    switch ( ulShow )
      {
      case SHOW_VERSION:
        if ( sql02_get_platform_id() == VER_PLATFORM_WIN32_NT )
          pszVersion = sql97c_get_server_version ( pszNodeName );

        if ( pszVersion == NULL )
          pszVersion = "???";

        if ( fActive )
          printf ( XSHOW_XSERVER_VER_ACTIVE, szPort, pszVersion );
        else
          printf ( XSHOW_XSERVER_VER_NOT_ACTIVE, szPort, pszVersion );
        break;

      case SHOW_DBROOT:
        if ( sql02_get_platform_id() == VER_PLATFORM_WIN32_NT )
          pszDBRoot  = sql97c_get_DBROOT_of_server ( pszNodeName );

          if ( pszDBRoot == NULL )
            pszDBRoot = "???\'";
          else
            {
            sql97c_compress_path ( szDBRoot, pszDBRoot, 40 );
            strcat ( szDBRoot, "'" );
            pszDBRoot = szDBRoot;
            }
        if ( fActive )
           printf ( XSHOW_XSERVER_DBR_ACTIVE, szPort, pszDBRoot );
        else
          printf ( XSHOW_XSERVER_DBR_NOT_ACTIVE, szPort, pszDBRoot );
        break;

      default:
        if ( fActive )
          printf ( XSHOW_XSERVER_ACTIVE, szPort );
        else
          printf ( XSHOW_XSERVER_NOT_ACTIVE, szPort );
        break;
      }

      rc = sql97_next_xserver_state ( szPort, sizeof(szPort), &fNoService, &fActive );
    } 
  while ( NO_ERROR == rc );

  DBGOUT;
  return ( rc );
  }

/*------------------------------*/

#if defined (_WIN32)

 static PSZ sql97c_get_account_name ( PSZ                pszNodeName,
                                      ULONG              ulServerType,
                                      PSZ                pszServerDB )
   { 
   #undef  MF__
   #define MF__ MOD__"sql97c_get_account_name"
   REG_ENTRY_REC       RegistryEntries[1];
   CHAR                szService[MX_DBNAME + 80];
   PATHNAME            szSubKey;
   CHAR                szAccountName[80];
   PSZ                 pszAccountName;
  

   DBGIN;

   szAccountName[0] ='\0';

   switch(ulServerType)
     {
     case SERVER_TYPE_ADABAS_SERVERDB :
       strcpy ( szService, ADABAS_SERVICE_ID);
       break;
     case SERVER_TYPE_ADABAS_GATEWAY :
       strcpy ( szService, ADABAS_SERVICE_ID_GW);
       break;
     case SERVER_TYPE_GATEWAY :
       strcpy ( szService, SERVICE_ID_GW);
       break;
     default:
       strcpy ( szService, SERVICE_ID);
       break;
     }

   strcat ( szService, pszServerDB);
   CharUpperBuff(szService, (DWORD)strlen(szService));
   strcpy ( szSubKey,  szService );
  
   RegistryEntries[0].pszValueName = REG_VN_OBJECT_NAME;
   RegistryEntries[0].pValue       = szAccountName;
   RegistryEntries[0].ulValueSize  = sizeof(szAccountName) - 1;
   RegistryEntries[0].ulValueType  = REG_SZ;

   sql50_reg_get_service_values ( pszNodeName, szSubKey, 1, RegistryEntries );

   if ( RegistryEntries[0].rc == NO_ERROR )
     {
     pszAccountName = (PSZ)RegistryEntries[0].pValue;

     if (( pszAccountName[0] == '.' ) && ( pszAccountName[1] == '\\' ))
       pszAccountName += 2;

     DBGOUT;
     return pszAccountName;
     }


   DBGOUT;
   return NULL;
   }

/*------------------------------*/

 static PSZ sql97c_get_db_gw_version ( PSZ                   pszNodeName,
                                       ULONG                 ulServerType,
                                       PSZ                   pszServerDB,
                                       tsp9_release_version* binaryVersion)
   {
   #undef  MF__
   #define MF__ MOD__"sql97c_get_db_gw_version"
   CHAR            szService[MX_DBNAME + 80];
   PATHNAME        szSubKey;
   REG_ENTRY_REC   RegistryEntries[3];
   static C40C     szRTEVersion;
   static C40C     szKernelVersion;
   LONG            rc = NO_ERROR;

   DBGIN;

   switch(ulServerType)
     {
     case SERVER_TYPE_ADABAS_SERVERDB :
       strcpy ( szService, ADABAS_SERVICE_ID);
       break;
     case SERVER_TYPE_ADABAS_GATEWAY :
       strcpy ( szService, ADABAS_SERVICE_ID_GW);
       break;
     case SERVER_TYPE_GATEWAY :
       strcpy ( szService, SERVICE_ID_GW);
       break;
     default:
       strcpy ( szService, SERVICE_ID);
       break;
     }

   strcat ( szService, pszServerDB);
   CharUpperBuff(szService, (DWORD)strlen(szService));
   strcpy ( szSubKey,  szService );
   strcat ( szSubKey, "\\"REG_SK_SERVICE_PARAM );

   RegistryEntries[0].pszValueName = REG_VN_KERNEL_VERSION;
   RegistryEntries[0].pValue       = szKernelVersion;
   RegistryEntries[0].ulValueSize  = sizeof(szKernelVersion);
   RegistryEntries[0].ulValueType  = REG_SZ;
   RegistryEntries[0].rc           = NO_ERROR;

   RegistryEntries[1].pszValueName = REG_VN_VERSION;
   RegistryEntries[1].pValue       = szRTEVersion;
   RegistryEntries[1].ulValueSize  = sizeof(szRTEVersion);
   RegistryEntries[1].ulValueType  = REG_SZ;
   RegistryEntries[1].rc           = NO_ERROR;

   RegistryEntries[2].pszValueName = REG_VN_VERSION_ID;
   RegistryEntries[2].pValue       = binaryVersion;
   RegistryEntries[2].ulValueSize  = sizeof(*binaryVersion);
   RegistryEntries[2].ulValueType  = REG_BINARY;
   RegistryEntries[2].rc           = NO_ERROR;

   sql50_reg_get_service_values ( pszNodeName, szSubKey, 3, RegistryEntries );

   /* PTS 1107094 
      compute version if REG_VN_VERSION_ID does not exist.
      (versions before 6.2) */
   if ( RegistryEntries[2].rc != NO_ERROR )
     {
     C40C     szVersion = "";

     if ( RegistryEntries[0].rc == NO_ERROR )
       {
       strcpy(szVersion, szKernelVersion);
       }
     else if ( RegistryEntries[1].rc == NO_ERROR )
       {
       strcpy(szVersion, szRTEVersion);
       }

     if (strlen(szVersion) > 0) 
       {
       int nIndex = 0;
       int nBegin = 0;
       
       for (nIndex = 0; nIndex < csp9_version_digits; ++nIndex) binaryVersion->no[nIndex] = 0;

       nIndex = 0;

       while (szVersion[nIndex] != ' ' && szVersion[nIndex] != '\0') ++nIndex;
       while (szVersion[nIndex] == ' ' && szVersion[nIndex] != '\0') ++nIndex;
       nBegin = nIndex;
       while (szVersion[nIndex] != ' ' && szVersion[nIndex] != '.' && szVersion[nIndex] != '\0') ++nIndex;
       if (szVersion[nIndex] != '\0') 
         {
         szVersion[nIndex] = '\0';
         binaryVersion->no[0] = atoi(&szVersion[nBegin]);
         ++nIndex;
         nBegin = nIndex;
         while (szVersion[nIndex] != ' ' && szVersion[nIndex] != '.' && szVersion[nIndex] != '\0') ++nIndex;
         if (szVersion[nIndex] != '\0') 
           {
           szVersion[nIndex] = '\0';
           binaryVersion->no[1] = atoi(&szVersion[nBegin]);
           ++nIndex;
           nBegin = nIndex;
           while (szVersion[nIndex] != ' ' && szVersion[nIndex] != '.' && szVersion[nIndex] != '\0') ++nIndex;
           szVersion[nIndex] = '\0';
           binaryVersion->no[2] = atoi(&szVersion[nBegin]);
           }
         }

       sql50_reg_put_service_values ( pszNodeName, szSubKey, 1, &RegistryEntries[2] );

       }
     }
   /* END PTS 1107094 */

   if ( RegistryEntries[0].rc == NO_ERROR )
     {
     DBGOUT;
     return (PSZ)RegistryEntries[0].pValue;
     }

   if ( RegistryEntries[1].rc == NO_ERROR )
     {
     DBGOUT;
     return (PSZ)RegistryEntries[1].pValue;
     }

   DBGOUT;
   return NULL;
   }

 /*------------------------------*/

 static PSZ sql97c_get_server_version ( PSZ  pszNodeName )
   {
   #undef  MF__
   #define MF__ MOD__"sql97c_get_server_version"
   PATHNAME        szSubKey;
   REG_ENTRY_REC   RegistryEntries[1];
   static C40C     szRTEVersion;
   LONG            rc = NO_ERROR;

   DBGIN;

   strcpy ( szSubKey, XSERV_TITLE );
   strcat ( szSubKey, "\\"REG_SK_SERVICE_PARAM );

   RegistryEntries[0].pszValueName = REG_VN_VERSION;
   RegistryEntries[0].pValue       = szRTEVersion;
   RegistryEntries[0].ulValueSize  = sizeof(szRTEVersion);
   RegistryEntries[0].ulValueType  = REG_SZ;
   RegistryEntries[0].rc           = NO_ERROR;


   sql50_reg_get_service_values ( pszNodeName, szSubKey, 1, RegistryEntries );

   if ( RegistryEntries[0].rc == NO_ERROR )
     {
     DBGOUT;
     return (PSZ)RegistryEntries[0].pValue;
     }

   DBGOUT;
   return NULL;
   }

 /*------------------------------*/

 static PSZ sql97c_get_DBROOT_of_server ( PSZ  pszNodeName )
   {
   #undef  MF__
   #define MF__ MOD__"sql97c_get_DBROOT_of_server"
   PATHNAME        szSubKey;
   REG_ENTRY_REC   RegistryEntries[1];
   static PATHNAME szDBRoot;
   LONG            rc = NO_ERROR;

   DBGIN;

   strcpy ( szSubKey, XSERV_TITLE );
   strcat ( szSubKey, "\\"REG_SK_SERVICE_PARAM );

   RegistryEntries[0].pszValueName = REG_VN_DBROOT;
   RegistryEntries[0].pValue       = szDBRoot;
   RegistryEntries[0].ulValueSize  = sizeof(szDBRoot);
   RegistryEntries[0].ulValueType  = REG_SZ;
   RegistryEntries[0].rc           = NO_ERROR;

   sql50_reg_get_service_values ( pszNodeName, szSubKey, 1, RegistryEntries );

   if ( RegistryEntries[0].rc == NO_ERROR )
     {
     DBGOUT;
     return (PSZ)RegistryEntries[0].pValue;
     }

   DBGOUT;
   return NULL;
   }

 /*------------------------------*/

 static VOID sql97c_compress_path ( PSZ    pszCompPathName,
                                    PSZ    pszPathName,
                                    ULONG  ulCompPathLen )
   {
   #undef  MF__
   #define MF__ MOD__"sql97c_compress_path"
   LONG       lPathLen;
   LONG       lSplitPos;
   LONG       lRest;

   DBGPAS;

   if ( ulCompPathLen < 4 )
     {
     pszCompPathName[0] = '\0';
     return;
     }

   lPathLen = (LONG)strlen ( pszPathName );

   if ( lPathLen > (INT)ulCompPathLen )
     {
     lSplitPos = (ulCompPathLen / 2) - 2;
     lRest     = ulCompPathLen - lSplitPos - 4;

     strncpy ( pszCompPathName, pszPathName, lSplitPos );
     pszCompPathName [ lSplitPos ] = '\0';
     strcat ( pszCompPathName, "...." );
     strcat ( pszCompPathName, pszPathName + lPathLen - lRest );
     }
   else
     {
     strcpy ( pszCompPathName, pszPathName );
     }
  return;
  }


 /*------------------------------*/
#if defined (CTRLCOMP)

static BOOL sql97c_newerInstallation ( tsp9_rte_installationinfo* currentRead,
                       tsp9_rte_installationinfo* currentNewest)
  {
# undef MF__
# define MF__ MOD__"sql97c_newerInstallation"
  int i;
  
  for (i = 0; i < csp9_version_digits; ++i) 
    {
    if (currentRead->version.no [i] > currentNewest->version.no [i]) 
      {
      return TRUE;
      }
    if (currentRead->version.no [i] < currentNewest->version.no [i]) 
      {
      return FALSE;
      }
    }
  return FALSE;
  }

 /*------------------------------*/

static BOOL sql97c_newestDBRoot ( PSZ  pszDBRoot,       // OUT parameter
                                  PSZ pszWantedFile)    // IN  parameter
{
# undef MF__
# define MF__ MOD__"sql97c_newestDBRoot"
    void*             handle;
    tsp9_rte_installationinfo releaseInfo;
    tsp9_rte_xerror       xerror;
    tsp9_rte_installationinfo choosenInstallation;
    BOOL              result;
    PATHNAME          testPath;

    choosenInstallation.version.no [0] = csp9_invalid_version;
    sqlxopen_installation_enum (&handle, &xerror);
    while (xerror.xe_result == csp9_xrte_ok) 
    {
        sqlxnext_installation (handle, &releaseInfo, &xerror);
        if (sql97c_newerInstallation (&releaseInfo, &choosenInstallation)) 
        {
            testPath[0]=0;
            if(isRelativePath(releaseInfo.dbroot))
            {
                tsp01_RteError  rteError; 
                sqlGetPortableInstallationRootPath ( testPath,TERM_WITH_DELIMITER_EO01,&rteError );
            }
#if 0
            SAPDB_Char *pU3Root = getenv("SDB_U3_ROOT");
            if(pU3Root && isRelativePath(releaseInfo.dbroot))
            {
                SAPDB_UInt u3RootLength = strlen(pU3Root);
                SAPDB_sprintf(testPath,sizeof(tsp00_Pathc),"%s%s",pU3Root,'\\' == pU3Root[u3RootLength-1] ? "" : "\\");
            }            
#endif
            strcat (testPath, releaseInfo.dbroot);
            strcat (testPath, "\\pgm\\");
            strcat (testPath, pszWantedFile);

            /* append .exe only if not already found */
            if ( strlen(pszWantedFile) <= strlen(".exe")
              || stricmp(pszWantedFile+strlen(pszWantedFile)-strlen(".exe"), ".exe") )
            {
                strcat (testPath, ".exe");
            }

            if (GetFileAttributes (testPath) != 0xffffffff) 
            {
                choosenInstallation = releaseInfo;
            }
        }
    }

    sqlxclose_installation_enum (handle);

    if (choosenInstallation.version.no [0] != csp9_invalid_version) 
    {
        strcpy (pszDBRoot, choosenInstallation.dbroot);
        result = TRUE;
    }
    else 
    {
        pszDBRoot [0] = '\0';
        result = FALSE;
    }
    return result;
}

 /*------------------------------*/

#endif

#endif

#endif

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
