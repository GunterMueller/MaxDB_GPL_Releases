/*!
  @file           vos97.c
  @author         RaymondR
  @brief          get SERVERDB and XSERVER status
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
#include "heo01.h"
#include "geo007_1.h"   
#include "gos41.h"
#include "RunTime/Configuration/RTEConf_ParameterAccessSimple.h"
#include "gsp09.h"



/*
 *  DEFINES
 */
#define USE_REGISTRY

#define MOD__  "VOS97C : "
#define MF__   MOD__"UNDEFINED"

#define PARAM_SEARCH_KEY             "KERNELVERSION"
#define SEARCH_PATTERN               "*"
#define FILE_ATTRIBUTE               0
#define DEFAULT_SERVICE_BUF          16384


/*
 *  MACROS
 */


/*
 *  LOCAL TYPE AND STRUCT DEFINITIONS
 */

/*
 * EXTERNAL VARIABLES
 */


/*
 *  EXPORTED VARIABLES
 */


/*
 * LOCAL VARIABLES
 */

 static LPENUM_SERVICE_STATUS                lpessService           = NULL;
 static ULONG                                ulNumServices;
 static ULONG                                ulCurrService          = 0;
 static ULONG                                ulCurrServerType       = SERVER_TYPE_SERVERDB;
 static USHORT                               usCurrPort             = 0;

 static WIN32_FIND_DATA                      FindData;
 static HANDLE                               FindHandle             = INVALID_HANDLE_VALUE;
 static char                                 portBits[((ULONG)((USHORT)-1) + 1)/8];

#define portBits_ZERO()                      memset(portBits, 0, sizeof(portBits))
#define portBits_SET(_n)                     (portBits[(_n)/8] |= (1 << ((_n) % 8)))
#define portBits_ISSET(_n)                   (portBits[(_n)/8] & (1 << ((_n) % 8)))

/*
 * LOCAL FUNCTION PROTOTYPES
 */
static VOID sql97_check_param_file ( PSZ    pszServerDB,
                                     PBOOL  pfOk );

static void   sql97_reset_to_find_first_server_and_service ();

static APIRET sql97_get_service_state( ULONG     ulServerType,
                                       PSZ       pszServerExtension,
                                       ULONG     ulMaxExtension,
                                       PBOOL     pfActive );
static APIRET sql97_get_server_state ( ULONG     ulServerType,
                                       PSZ       pszServerExtension,
                                       ULONG     ulMaxExtension,
                                       PBOOL     pfActive );

static teo00_Bool sql97_PortableInstallationAcitve();

/*
 * ========================== GLOBAL FUNCTIONS ================================
 */



/*
  Function: os97_IsServiceRunning
  Description: Check if given service is running or start pending
  Arguments: hSCM [in] handle to service control manager
             serviceName [in] service name (not display name) of service to test
  Return value: TRUE if service is running or start pending, FALSE else
 */
static bool os97_IsServiceRunning(SC_HANDLE hSCM, 
                                  const char *serviceName)
{
    SERVICE_STATUS serviceStatus;
    SC_HANDLE hService;
    DWORD rc;
    bool result = false;

    hService = OpenService(hSCM, serviceName, SERVICE_QUERY_STATUS );
    if ( !hService )
    {
        rc = GetLastError();
        if ( rc != ERROR_SERVICE_DOES_NOT_EXIST )
        {
            MSGCD(( ERR_OPEN_SERVICE_MANAGER, rc ));
        }
    }
    else
    {
        if ( QueryServiceStatus( hService,         // handle of service
                                &serviceStatus  // address of service status structure
                               ) )
        {
            result = ( (serviceStatus.dwCurrentState == SERVICE_RUNNING) 
                    || (serviceStatus.dwCurrentState == SERVICE_START_PENDING) );  
        }
        if (!CloseServiceHandle(hService))
        {
            DWORD rc = GetLastError();
            MSGCD(( ERR_CLOSE_SERVICE_MANAGER, rc ));
        }
    }
    return result;
}

//--------------------------------------------------------

/*
  Function: os97_IsServiceRunning
  Description: Check if given service is running or start pending
  Arguments: hSCM [in] handle to service control manager
             serviceName [in] service name (not display name) of service to test
  Return value: TRUE if service is running or start pending, FALSE else
 */
tsp9_pgm_kind sqlDBSpeed( const tsp00_DbNamec szDbName )
{
    #undef MF__
    #define MF__ MOD__"sqlDBSpeed"
    SC_HANDLE hSCM;
    char serviceName[256];
    tsp9_pgm_kind runningKind = csp9_any_pgm;

    DBGIN;

    hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_QUERY_LOCK_STATUS);
    if ( !hSCM )
    {
        DWORD rc = GetLastError();
        if ( rc != ERROR_SERVICE_DOES_NOT_EXIST )
        {
            MSGCD(( ERR_OPEN_SERVICE_MANAGER, rc ));
        }
    }
    else
    {
        sprintf(serviceName, "%s%s", SERVICE_ID, szDbName);
        if ( os97_IsServiceRunning( hSCM, serviceName ) )
        {
            runningKind = csp9_fast_pgm;
        }
        else
        {
            sprintf(serviceName, "%s%s%s", SERVICE_ID, szDbName, SERVICE_SLOW_EXT);
            if ( os97_IsServiceRunning( hSCM, serviceName ) )
            {
                runningKind = csp9_slow_pgm;
            }
            else
            {
                sprintf(serviceName, "%s%s%s", SERVICE_ID, szDbName, SERVICE_QUICK_EXT);
                if ( os97_IsServiceRunning( hSCM, serviceName ) )
                {
                    runningKind = csp9_quick_pgm;
                }
                else
                {
                    sprintf(serviceName, "%s%s%s", SERVICE_ID, szDbName, SERVICE_TEST_EXT);
                    if ( os97_IsServiceRunning( hSCM, serviceName ) )
                    {
                        runningKind = csp9_test_pgm;
                    }
                }
            }
        }

        if (!CloseServiceHandle(hSCM))
        {
            DWORD rc = GetLastError();
            MSGCD(( ERR_CLOSE_SERVICE_MANAGER, rc ));
        }
    }

    DBGOUT;

    return runningKind;
}

//--------------------------------------------------------

 APIRET sql97_update ( PSZ    pszNodeName )
   {
   #undef  MF__
   #define MF__ MOD__"sql97_update"
   APIRET                 rc               = NO_ERROR;
   ULONG                  ulSize           = DEFAULT_SERVICE_BUF;
   ULONG                  ulRemaining      = 0;
   ULONG                  ulResumeHandle;
   SC_HANDLE              hScM;

   DBGIN;

   // --- free old buffer
   if ( lpessService != NULL )
     {
     rc = FREE_MEM ( lpessService );

     lpessService = NULL;

     if ( rc != NO_ERROR )
       {
       DBGOUT;
       return( rc );
       }
     }

   for ( ;; )
     {
     if (!(hScM = OpenSCManager( pszNodeName, NULL, SC_MANAGER_ENUMERATE_SERVICE)))
       {
       rc = GetLastError();

       switch ( rc )
         {
         case ERROR_ACCESS_DENIED:      MSGCD (( ERR_ACCESS_DENIED ));
                                        break;
         case RPC_S_SERVER_UNAVAILABLE: MSGCD (( ERR_SERVER_NODE_UNKNOWN, pszNodeName ));
                                        break;
         default:                       MSGCD(( ERR_OPEN_SERVICE_MANAGER, rc ));
                                        break;
         }
       DBGOUT;
       return ( rc );
       }

     // --- allocate space for the result buffer
     rc = ALLOC_MEM ( &lpessService, ulSize );

     if ( rc != NO_ERROR )
       {
       lpessService = NULL;
       DBGOUT;
       return( rc );
       }

     ulCurrService    = (ULONG)UNDEF;
     ulNumServices    = 0;
     ulResumeHandle   = 0;

     // --- now we ask for the service entries
     if (!EnumServicesStatus( hScM, SERVICE_WIN32,
                              SERVICE_ACTIVE | SERVICE_INACTIVE,
                              lpessService, ulSize,
                              &ulRemaining, &ulNumServices,
                              &ulResumeHandle ))
       {
       rc = GetLastError ();

       if (( rc != NO_ERROR ) && ( rc != ERROR_MORE_DATA ))
         {
         FREE_MEM ( lpessService );
         lpessService = NULL;
         DBGOUT;
         return ( rc );
         }

       rc = FREE_MEM ( lpessService );

       lpessService = NULL;

       if ( rc != NO_ERROR )
         {
         DBGOUT;
         return( rc );
         }

       if (!CloseServiceHandle(hScM))
         {
         rc = GetLastError();
         MSGCD(( ERR_CLOSE_SERVICE_MANAGER, rc ));

         DBGOUT;
         return ( rc );
         }

       ulSize = ulSize + ulRemaining;
       }
     else
       break;
     }

    if (!CloseServiceHandle(hScM) && rc == NO_ERROR)
      {
      rc = GetLastError();
      MSGCD(( ERR_CLOSE_SERVICE_MANAGER, rc ));

      DBGOUT;
      return ( rc );
      }

   DBGOUT;
   return ( rc );
   }

/*------------------------------*/

APIRET sql97_first_db_gw_state ( ULONG     ulServerType,
                                 PSZ       pszServerDB,
                                 PBOOL     pfActive )
  {
  #undef  MF__
  #define MF__ MOD__"sql97_first_db_gw_state"
  APIRET          rc                = NO_ERROR;

  ulCurrServerType = ulServerType;

  sql97_reset_to_find_first_server_and_service ();

  if ( false == sql97_PortableInstallationAcitve() )
  {
      if ( NO_ERROR == (rc = sql97_get_service_state ( ulCurrServerType, pszServerDB, MX_DBNAME, pfActive )) )
      {
          DBGOUT;
          return( rc );
      }

      DBGOUT;
      return ( rc );
  }
  else if ( SERVER_TYPE_SERVERDB == ulCurrServerType )
  {
      rc = sql97_get_server_state ( ulCurrServerType, pszServerDB, MX_DBNAME, pfActive );
  }
  else
      rc = ERROR_NO_MORE_FILES;


  DBGOUT;
  return ( rc );
  }

/*------------------------------*/

APIRET sql97_next_db_gw_state ( PSZ       pszServerDB,
                                PBOOL     pfActive )
  {
  #undef  MF__
  #define MF__ MOD__"sql97_next_db_gw_state"
  APIRET    rc        = NO_ERROR;

  if ( false == sql97_PortableInstallationAcitve() )
  {
      if ( lpessService == NULL )
      {
          DBGOUT;
          return ( ERROR_NO_MORE_FILES );
      }

      if ( NO_ERROR == (rc = sql97_get_service_state ( ulCurrServerType, pszServerDB, MX_DBNAME, pfActive )))
      {
          DBGOUT;
          return( rc );
      }

      DBGOUT;
      return ( rc );
  }
  else if ( SERVER_TYPE_SERVERDB == ulCurrServerType )
  {
      rc = sql97_get_server_state ( ulCurrServerType, pszServerDB, MX_DBNAME, pfActive );
  }
  else
      rc = ERROR_NO_MORE_FILES;


  DBGOUT;
  return ( rc );
  }

/*------------------------------*/

APIRET  sql97_first_xserver_state ( char *    pszPort,
                                    ULONG     ulPortBufSize,
                                    PBOOL     pfNoService,
                                    PBOOL     pfActive )
  {
  #undef  MF__
  #define MF__ MOD__"sql97_first_xserver_state"
  APIRET    rc;

  *pfActive     = FALSE;
  *pfNoService  = FALSE;
  portBits_ZERO();

  sql97_reset_to_find_first_server_and_service ();

  if ( NO_ERROR == ( rc = sql97_get_service_state ( SERVER_TYPE_XSERVER, pszPort, ulPortBufSize-1, pfActive )) )
  {
    USHORT usPort = 0;

    if ( '\0' != pszPort[0] )
      usPort = atoi(pszPort+1);

    portBits_SET(usPort);
    DBGOUT;
    return rc;
  }

 *pfNoService = TRUE;

  rc = sql97_get_server_state ( SERVER_TYPE_XSERVER, pszPort, ulPortBufSize-1, pfActive );

  DBGOUT;
  return rc;
  }

/*------------------------------*/

APIRET   sql97_next_xserver_state ( char *    pszPort,
                                    ULONG     ulPortBufSize,
                                    PBOOL     pfNoService,
                                    PBOOL     pfActive )
  {
  #undef  MF__
  #define MF__ MOD__"sql97_next_xserver_state"
  APIRET    rc;

 *pfActive     = FALSE;
 *pfNoService  = FALSE;

  if ( NO_ERROR == ( rc = sql97_get_service_state ( SERVER_TYPE_XSERVER, pszPort, ulPortBufSize-1, pfActive )) )
    {
    USHORT usPort = 0;

    if ( '\0' != pszPort[0] )
      usPort = atoi(pszPort+1);

    portBits_SET(usPort);
    DBGOUT;
    return rc;
    }

 *pfNoService = TRUE;

  rc = sql97_get_server_state ( SERVER_TYPE_XSERVER, pszPort, ulPortBufSize-1, pfActive );

  DBGOUT;
  return rc;
  }

/*
 * ========================== LOCAL FUNCTIONS =================================
 */

static VOID sql97_check_param_file ( PSZ   pszServerDB,
                                     PBOOL pfValidParamFile )
  {
  #undef  MF__
  #define MF__ MOD__"sql97_check_param_file"
  tsp00_C256c              XpValueC ;
  tsp00_ErrText ErrText;

  DBGPAS;

  // --- display no error messages

  sql60_disable_message_output ();

  *pfValidParamFile = RTEConf_ParamaterAccessGetParam ( pszServerDB, PARAM_SEARCH_KEY, XpValueC, ErrText );

  sql60_enable_message_output();
  }

/*------------------------------*/

static void sql97_reset_to_find_first_server_and_service ()
{
    ulCurrService  = 0;
    if ( INVALID_HANDLE_VALUE != FindHandle )
    {
        FindClose(FindHandle);
        FindHandle = INVALID_HANDLE_VALUE;
    }
    usCurrPort    = 0;
}

/*------------------------------*/

static APIRET sql97_get_server_state ( ULONG     ulServerType,
                                       PSZ       pszServerExtension,
                                       ULONG     ulMaxExtension,
                                       PBOOL     pfActive )
{
  PATHNAME        szSearchName;
  PSZ             pszFileName;
  tsp00_Pathc     pszConfigPath;
  BOOL            fValidParamFile;
  HEV             hevSem    = 0;
  APIRET          rc;
  tsp01_RteError  RteError ;

  if ( ulServerType == SERVER_TYPE_XSERVER )
  {
      if (( 0 == usCurrPort ) && ( 0 == portBits_ISSET(0) ))
      {
          // --- check SERVER
          rc = sql41c_open_event_sem ( &hevSem, SEM_XSERVER, "", ERROR_ACCESS_DENIED);
          pszServerExtension[0] = '\0';

          if  ( rc == ERROR_ACCESS_DENIED )
              rc = NO_ERROR;

          ++usCurrPort;
      }
      else
          rc = ERROR_SEM_NOT_FOUND;

      if  ( rc == ERROR_SEM_NOT_FOUND )
      {
          PATHNAME        szSearchName;

          do
          {
              strcpy (szSearchName, SEM_XSERVER );
              strcat (szSearchName, "-" );
              itoa ( usCurrPort, szSearchName + sizeof(SEM_XSERVER) - 1, 10 );

              // --- check SERVER
              rc = sql41c_open_event_sem ( &hevSem, szSearchName, "", ERROR_ACCESS_DENIED);

              if  (( (rc == ERROR_ACCESS_DENIED) || (rc == NO_ERROR)) && ( 0 == portBits_ISSET(usCurrPort) ))
              {
                  strncpy( pszServerExtension, szSearchName + sizeof(SEM_XSERVER) - 2, ulMaxExtension - 1 );
                  pszServerExtension[ulMaxExtension - 1] = '\0';
                  usCurrPort++;
                  break;
              }
          }
          while ( ++usCurrPort );
      }

      if (rc == ERROR_ACCESS_DENIED)
      {
          *pfActive = TRUE;
          rc        = NO_ERROR;
      }
      else if ( rc == ERROR_SEM_NOT_FOUND )
      {
          *pfActive = FALSE;
          rc        = ERROR_NO_MORE_FILES;
      }
      else if (rc == NO_ERROR)
      {
          sql41c_close_event_sem ( hevSem, XSERV_TITLE );
          *pfActive = TRUE;
      }
      else
          *pfActive = FALSE;
  }
  else
  {
      *pfActive = FALSE;
      rc        = NO_ERROR;

      if ( INVALID_HANDLE_VALUE == FindHandle )
      {
          pszFileName = FindData.cFileName;


          /* PTS 1107043 */
          if ( !sqlGetIndependentConfigPath ( pszConfigPath, TERM_WITH_DELIMITER_EO01, &RteError ) )
          {
              MSGCD(( ERR_CANT_GET_CONFIG_PATH ));
              MSGCD(( ERR_RTE_ERROR, RteError.RteErrCode, RteError.RteErrText ));
              MSGCD(( ERR_OS_ERROR , RteError.OsErrCode,  RteError.OsErrText ));
              return ( !NO_ERROR ) ;
          }

          strcpy (szSearchName, pszConfigPath ) ;
          strcat (szSearchName, SEARCH_PATTERN);


          FindHandle = FindFirstFile(szSearchName, &FindData);

          if (FindHandle == INVALID_HANDLE_VALUE)
          {
              rc = GetLastError();

              if ( rc == ERROR_FILE_NOT_FOUND )
                  rc = ERROR_NO_MORE_FILES;
          }
          else
              CharUpper(FindData.cFileName);

          if ( rc == ERROR_PATH_NOT_FOUND )
          {
              MSGCD((ERR_WRONG_DBROOT_PATH, pszConfigPath));
              return ( rc );
          }
          else if ( rc == NO_ERROR )
          {
              do
              {
                  if ( strrchr ( pszFileName, '.' ) != NULL )
                      fValidParamFile = FALSE;
                  else
                      sql97_check_param_file ( pszFileName, &fValidParamFile );

                  if ( fValidParamFile == FALSE )
                  {
                      if (!FindNextFile(FindHandle, &FindData))
                      {
                          rc = GetLastError();
                          FindClose(FindHandle);
                          FindHandle = INVALID_HANDLE_VALUE;
                      }
                      else
                          CharUpper(FindData.cFileName);
                  }
              }
              while (( fValidParamFile == FALSE ) && ( rc == NO_ERROR ));


              if (( fValidParamFile == TRUE ) && ( rc == NO_ERROR ))
              {
                  pszServerExtension[ulMaxExtension] = '\0';
                  strncpy ( pszServerExtension, pszFileName, ulMaxExtension );

                  rc = sql41c_open_event_sem ( &hevSem, SEM_COORD,pszFileName, ERROR_ACCESS_DENIED);

                  if ((rc == NO_ERROR) || (rc == ERROR_ACCESS_DENIED))
                  {
                      *pfActive = TRUE;

                      if (rc == NO_ERROR)
                          sql41c_close_event_sem ( hevSem, "COORD" );
                      rc = NO_ERROR;
                  }
                  else if ( rc == ERROR_SEM_NOT_FOUND )
                      rc = NO_ERROR;
              }
          }
      }
      else
      {
          pszFileName = FindData.cFileName;

          do
          {
              if (!FindNextFile(FindHandle, &FindData))
              {
                  rc = GetLastError();
                  FindClose(FindHandle);
                  FindHandle = INVALID_HANDLE_VALUE;
              }

              if ( rc == NO_ERROR )
              {
                  if ( strrchr ( pszFileName, '.' ) != NULL )
                      fValidParamFile = FALSE;
                  else
                      sql97_check_param_file ( pszFileName, &fValidParamFile );
              }
          }
          while (( fValidParamFile == FALSE ) && ( rc == NO_ERROR ));


          if (( fValidParamFile == TRUE ) && ( rc == NO_ERROR ))
          {
              pszServerExtension[ulMaxExtension] = '\0';
              strncpy ( pszServerExtension, pszFileName, ulMaxExtension );

              rc = sql41c_open_event_sem ( &hevSem, SEM_COORD, pszFileName, ERROR_ACCESS_DENIED);

              if ((rc == NO_ERROR) || (rc == ERROR_ACCESS_DENIED))
              {
                  *pfActive = TRUE;

                  if (rc == NO_ERROR)
                      sql41c_close_event_sem ( hevSem, "COORD" );
                  rc = NO_ERROR;
              }
              else if ( rc == ERROR_SEM_NOT_FOUND )
                  rc = NO_ERROR;
          }
      }
  }
  return rc;
}

/*------------------------------*/

static APIRET sql97_get_service_state ( ULONG     ulServerType,
                                        PSZ       pszServerExtension,
                                        ULONG     ulMaxExtension,
                                        PBOOL     pfActive )
   {
   #undef  MF__
   #define MF__ MOD__"sql97_get_service_state"
   PSZ    pszServiceID;

   DBGIN;

   if ( ulCurrService >= ulNumServices )
     {
     DBGOUT;
     return ( ERROR_NO_MORE_FILES );
     }

   switch(ulServerType)
     {
     case SERVER_TYPE_ADABAS_SERVERDB :
       pszServiceID = ADABAS_SERVICE_ID;
       break;
     case SERVER_TYPE_ADABAS_GATEWAY :
       pszServiceID = ADABAS_SERVICE_ID_GW;
       break;
     case SERVER_TYPE_GATEWAY :
       pszServiceID = SERVICE_ID_GW;
       break;
     case SERVER_TYPE_XSERVER:
       pszServiceID = SERVICE_ID_XSERVER;
       break;
     default:
       pszServiceID = SERVICE_ID;
       break;
     }

   for (; ulCurrService < ulNumServices; ulCurrService++ )
     {
     if (! strncmp( lpessService[ulCurrService].lpServiceName, pszServiceID,
                    strlen(pszServiceID)))
       {
       int len = strlen(pszServiceID);
       pszServerExtension[0] = '\0';

       if ( '\0' != lpessService[ulCurrService].lpServiceName[len] )
       {
           strncpy( pszServerExtension, lpessService[ulCurrService].lpServiceName + len, ulMaxExtension );
           pszServerExtension[ulMaxExtension - 1] = '\0';
       }

       switch ( lpessService[ulCurrService].ServiceStatus.dwCurrentState )
         {
         case SERVICE_START_PENDING:
         case SERVICE_RUNNING:
           *pfActive   = TRUE;
           break;
         default:
           *pfActive   = FALSE;
           break;
         }

       ulCurrService++;

       DBGOUT;
       return ( NO_ERROR );
       }
     }

   DBGOUT;
   return ( ERROR_NO_MORE_FILES );
   }

/*------------------------------*/

static teo00_Bool sql97_PortableInstallationAcitve()
 {
  #undef  MF__
  #define MF__ MOD__"sql97_PortableInstallationAcitve"

  tsp01_RteError          RteError;
  tsp00_Pathc             Path;

  return sqlGetPortableInstallationRootPath(Path, TERM_WITHOUT_DELIMITER_EO01, &RteError );
 }

/*
 * =============================== END ========================================
 */

