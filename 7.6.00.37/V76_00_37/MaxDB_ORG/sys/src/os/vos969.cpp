/*!
  @file           vos969.cpp
  @author         RaymondR
  @brief          START/STOP/INSTALL/REMOVE TCP/IP_DATABASE_SERVER
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




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#if defined CLUSTER_SERVER_VERSION
# include  "gos200.h" /* nocheck */
# include  "gos212.h" /* nocheck */
#endif

#include    "heo01.h"
#include    "hsp100.h"
#include    "gos969.h"
#include    "geo007_1.h"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define MOD__  "VOS969 : "
#define MF__   MOD__"UNDEFINED"

#define TIMEOUT                   120
#define START_SLEEP_TIME          15000
#define DIAGSIZE                  100                      // - 4KByte Pages

#if defined(DEBUG_RTE)
 #define STATIC
#else
 #define STATIC static
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



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

static LONG          os969CheckServiceParams  ( char *pszServerName );

/*===========================================================================*
 *  PUBLIC FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

externC
DWORD os969StartAnyServerService    ( char *pszNodeName,
                                       char *pszServerName )
   {
   #undef  MF__
   #define MF__ MOD__"os969StartAnyServerService"
   LONG                    rc                         = NO_ERROR;
   SC_HANDLE               schService                 = NULL;
   ULONG                   ulTimeout                  = TIMEOUT;
   SC_HANDLE               schSCManager;
   SERVICE_STATUS          ssSeviceStatus;

   DBGIN;

#if defined CLUSTER_SERVER_VERSION
   // --- try start XSERVER via cluster server
   rc = os212_OnlineXSERVERClusterResource ();

   if (( rc != ERROR_RESOURCE_NOT_FOUND ) &&
       ( rc != ERROR_MOD_NOT_FOUND ))
   {
     return rc;
   }
#endif

   sql60_open_event_log ( pszServerName, EVENT_SOURCE_NAME );

   schSCManager = OpenSCManager( pszNodeName, NULL, SC_MANAGER_CONNECT );

   if ( schSCManager != NULL )
     {
     schService = OpenService(schSCManager, pszServerName,
                              SERVICE_START | SERVICE_QUERY_STATUS);

     if (schService == NULL)
       rc = GetLastError();
     else
       {
       if ( pszNodeName == NULL )
         {
         rc = os969CheckServiceParams ( pszServerName);

         if ( rc != NO_ERROR )
           {
           CloseServiceHandle(schService);
           CloseServiceHandle(schSCManager);

           DBGOUT;
           return ( rc );
           }
        }

       if ( !StartService(schService, 0, NULL) )
         rc = GetLastError();
       else
         {
         do
           {
           SLEEP( 1000 );
           ulTimeout--;

           if ( !QueryServiceStatus( schService, &ssSeviceStatus ))
             rc = GetLastError();
           else if ( ssSeviceStatus.dwCurrentState == SERVICE_RUNNING )
             break;
           else if ( ssSeviceStatus.dwCurrentState == SERVICE_STOPPED )
             {
             MSGCD(( ERR_XSER_STOPPED_EV_LOG ));
             DBGOUT;
             return( (DWORD)-2 );
             }

           if ( ulTimeout == 0 )
             rc = ERROR_TIMEOUT;
           }
         while ( rc == NO_ERROR );
         }
       CloseServiceHandle(schService);
       }
     CloseServiceHandle(schSCManager);
     }
   else
     rc = GetLastError();

   switch (rc)
     {
     case NO_ERROR:                        break;
     case ERROR_FILE_NOT_FOUND:
     case ERROR_PATH_NOT_FOUND:            MSGALL (( INFO_SERVICE_NOT_INST_CORR ));
                                           break;
     case ERROR_SERVICE_DOES_NOT_EXIST:    MSGALL (( ERR_SERVICE_UNKNOWN, (char *)pszServerName ));
                                           break;
     case ERROR_SERVICE_ALREADY_RUNNING:   MSGCD (( ERR_XSER_ALREADY_RUNNING ));
                                           break;
     case RPC_S_SERVER_UNAVAILABLE:        MSGALL (( ERR_SERVER_NODE_UNKNOWN, (char *)pszNodeName ));
                                           break;
     case ERROR_ACCESS_DENIED:             MSGALL (( ERR_ACCESS_DENIED_ON_XX, (char *)pszServerName ));
                                           break;
     default:                              MSGALL (( ERR_XSER_CANNOT_START, rc ));
                                           break;
     }

   sql60_close_event_log ();

   DBGOUT;
   return ( rc );
   }

 /*------------------------------*/

externC
DWORD os969StopAnyServerService    ( char *pszNodeName,
                                       char *pszServerName )
   {
   #undef  MF__
   #define MF__ MOD__"os969StopAnyServerService"
   LONG                    rc                         = NO_ERROR;
   SC_HANDLE               schService                 = NULL;
   ULONG                   ulTimeout                  = TIMEOUT;
   SC_HANDLE               schSCManager;
   SERVICE_STATUS          ssSeviceStatus;

   DBGIN;

#if defined CLUSTER_SERVER_VERSION
   // --- try stop XSERVER via cluster server
   rc = os212_OfflineXSERVERClusterResource ();

   if (( rc != ERROR_RESOURCE_NOT_FOUND ) &&
       ( rc != ERROR_MOD_NOT_FOUND ))
   {
     return rc;
   }
#endif

   sql60_open_event_log ( pszServerName, EVENT_SOURCE_NAME );

   schSCManager = OpenSCManager( pszNodeName, NULL, SC_MANAGER_CONNECT );

   if ( schSCManager != NULL )
     {
     schService = OpenService(schSCManager, pszServerName,
                              SERVICE_STOP | SERVICE_QUERY_STATUS);

     if (schService == NULL)
       rc = GetLastError();
     else
       {
       if (!ControlService(schService, SERVICE_CONTROL_STOP, &ssSeviceStatus))
         rc = GetLastError();
       else
         {
         do
           {
           SLEEP( 1000 );
           ulTimeout--;

           if ( !QueryServiceStatus( schService, &ssSeviceStatus ))
             rc = GetLastError();
           else if ( ssSeviceStatus.dwCurrentState == SERVICE_STOPPED )
             break;

           if ( ulTimeout == 0 )
             rc = ERROR_TIMEOUT;
           }
         while ( rc == NO_ERROR );
         }
       CloseServiceHandle(schService);
       }
     CloseServiceHandle(schSCManager);
     }
   else
     rc = GetLastError();

   switch (rc)
     {
     case NO_ERROR:                     break;
     case ERROR_ACCESS_DENIED:          MSGALL (( ERR_ACCESS_DENIED_ON_XX, (char *)pszServerName ));
                                        break;
     case ERROR_SERVICE_DOES_NOT_EXIST: MSGALL (( ERR_SERVICE_UNKNOWN, (char *)pszServerName ));
                                        break;
     case RPC_S_SERVER_UNAVAILABLE :    MSGALL (( ERR_SERVER_NODE_UNKNOWN, (char *)pszNodeName ));
                                        break;
     case ERROR_SERVICE_NOT_ACTIVE :    MSGCD (( ERR_XSER_NOT_STARTED ));
                                        break;
     default:                           MSGALL (( ERR_XSER_CANNOT_STOP, rc ));
                                        break;
     }

   sql60_close_event_log ();

   DBGOUT;
   return ( rc );
   }

/*------------------------------*/

externC
DWORD os969StartAnyServer           ( char *pszCmdLine, 
                                       char *pszServerName, 
                                       char *pszServerSemaName, 
                                       BOOL fSuppressActiveMsg )
  {
  #undef  MF__
  #define MF__ MOD__"os969StartAnyServer"
  LONG                    rc                     = NO_ERROR;
  HEV                     hevSemXserv            = 0;
  CHAR                    szCmdLine[512];
   static STARTUPINFO     StartupInfo            = {sizeof(STARTUPINFO)};
   PROCESS_INFORMATION    ProcessInfo;
   CHAR                   szCommandFile[MAX_PATH];

  DBGIN;

  if (strlen(pszCmdLine) + 3 >= sizeof(szCmdLine))
    {
    MSGCD (( ERR_CMD_LINE_TOO_LONG ));
    DBGOUT;
    return ( 1 );
    }

  strcpy(szCmdLine, pszCmdLine);
  strcat(szCmdLine, " -R");

  rc = sql41c_open_event_sem ( &hevSemXserv, pszServerSemaName, "", NO_ERROR);

  if (rc == NO_ERROR)
    {
    sql41c_close_event_sem ( hevSemXserv, pszServerName );

    if ( fSuppressActiveMsg == FALSE )
      MSGCD (( ERR_XSER_ALREADY_RUNNING ));

    DBGOUT;
    return( ERROR_ALREADY_EXISTS );
    }

   if (GetModuleFileName(NULL, szCommandFile, sizeof(szCommandFile)))
       pszCmdLine = szCommandFile;
   else
       pszCmdLine = NULL;

   StartupInfo.dwFlags     = STARTF_USESHOWWINDOW;
   StartupInfo.wShowWindow = SW_SHOWMINNOACTIVE;

   if (!CreateProcess(pszCmdLine,
                      szCmdLine,
                      NULL,
                      NULL,
                      FALSE,
                      CREATE_NO_WINDOW,
                      NULL,
                      NULL,
                      &StartupInfo,
                      &ProcessInfo))
     {
     if (!pszCmdLine)
       pszCmdLine = pszServerName;

     rc = GetLastError();

     MSGCD (( ERR_XSER_CANNOT_START, rc ));
     }

  if ( rc == NO_ERROR)
    {
    SLEEP ( START_SLEEP_TIME );

    // --- check if XSERVER is still active
    rc = sql41c_open_event_sem ( &hevSemXserv, pszServerSemaName, "",
                                ERROR_ACCESS_DENIED);

    if ( rc == ERROR_SEM_NOT_FOUND )
      MSGCD (( ERR_XSER_STOPPED_DIAG ))
    else if ( rc == NO_ERROR )
      sql41c_close_event_sem ( hevSemXserv, pszServerName );
    else
      rc = NO_ERROR;
    }


  DBGOUT;
  return ( rc );
  }

/*------------------------------*/

externC
DWORD os969StopAnyServer            ( char *pszServerName,
                                       char *pszServerSemName )
  {
  #undef  MF__
  #define MF__ MOD__"os969StopAnyServer"
  HEV                     hevSemXserv  = 0;   // --- should be zero
  DWORD                  rc           = NO_ERROR;

  DBGIN;

  rc = sql41c_open_event_sem ( &hevSemXserv, pszServerSemName, "",
                               ERROR_ACCESS_DENIED);

  if (rc == ERROR_ACCESS_DENIED)
    MSGCD(( ERR_ACCESS_DENIED_ON_XX, pszServerName ))
  else if (rc == ERROR_SEM_NOT_FOUND)
    MSGCD (( ERR_XSER_NOT_STARTED ))
  else if ( rc == NO_ERROR )
    {
    rc = sql41c_post_event_sem ( hevSemXserv, pszServerName );

    sql41c_close_event_sem ( hevSemXserv, pszServerName );
    }

  DBGOUT;
  return (rc);
  }

/*------------------------------*/

externC
DWORD os969UpdateAnyServerService   ( char      *pszNodeName, 
                                      char      *pszServerName, 
								      BOOL      fAutostart,
                                      BOOL      fNoDNSReverseLookup,
                                      DWORD     dwPortNo,
                                      DWORD     dwNIPortNo,
                                      DWORD     dwSSLPortNo )
   {
   #undef  MF__
   #define MF__ MOD__"os969UpdateAnyServerService"
   LONG                    rc            = NO_ERROR;
   SC_HANDLE               schService    = NULL;
   SC_HANDLE               schSCManager;

   sql60_open_event_log ( pszServerName, EVENT_SOURCE_NAME );

   schSCManager = OpenSCManager( pszNodeName, NULL, SC_MANAGER_ALL_ACCESS );

   if ( schSCManager != NULL )
     {
     if (schService = OpenService(schSCManager, pszServerName, SERVICE_START))
       {
       CloseServiceHandle(schService);
       CloseServiceHandle(schSCManager);

       rc = os969RemoveAnyXserverService ( pszNodeName, pszServerName );

       if ( rc == NO_ERROR )
        rc = os969InstallAnyXserverService ( pszNodeName, pszServerName, fAutostart, fNoDNSReverseLookup, dwPortNo, dwNIPortNo, dwSSLPortNo );

       sql60_close_event_log ();

       DBGOUT;
       return ( rc );

       }
     else if ( GetLastError() == ERROR_SERVICE_DOES_NOT_EXIST )
       rc = os969InstallAnyXserverService ( pszNodeName, pszServerName, fAutostart, fNoDNSReverseLookup, dwPortNo, dwNIPortNo, dwSSLPortNo );
     }
   else
     {
     rc = GetLastError();

     if (rc == ERROR_ACCESS_DENIED)
       MSGALL(( ERR_ACCESS_DENIED_ON_XX, (char *)pszServerName ))
     else if ( rc == RPC_S_SERVER_UNAVAILABLE )
       MSGALL(( ERR_SERVER_NODE_UNKNOWN, (char *)pszNodeName ))
     else
       MSGALL (( ERR_INSTALL_SERVICE, (char *)pszServerName, rc ))
     }

   sql60_close_event_log ();

   DBGOUT;
   return ( rc );
   }

 /*------------------------------*/

externC
DWORD os969InstallAnyXserverService ( char      *pszNodeName, 
                                      char      *pszServerName, 
	                                  BOOL      fAutostart,
                                      BOOL      fNoDNSReverseLookup,
                                      DWORD     dwPortNo,
                                      DWORD     dwNIPortNo,
                                      DWORD     dwSSLPortNo )
   {
   #undef  MF__
   #define MF__ MOD__"os969InstallAnyXserverService"
   LONG                    rc            = NO_ERROR;
   SC_HANDLE               schService    = NULL;
   PSECURITY_DESCRIPTOR    pSD           = NULL;
   SC_HANDLE               schSCManager;
   REG_ENTRY_REC           RegistryEntries[13];
   PATHNAME                szKey;
   PATHNAME                szMsgAndCategoryFile;
   tsp100_VersionID0       RTEVersionID;
   tsp00_Versionc          RTEVersionString;
   tsp00_Pathc             szIndepPGMPath;
   tsp00_Pathc             dbrootPath;
   tsp00_Pathc             szCommandFile;
   tsp01_RteError          RteError ;
   DWORD                   dwValUseIndepPath;     /* PTS 1107043 */
   DWORD                   dwDebugLevel         = 0;
   DWORD                   dwDiagFileSize       = DIAGSIZE;
   DWORD                   dwNoDNSReverseLookup = fNoDNSReverseLookup ? 1 : 0;
   
   DBGIN;

#  if defined CLUSTER_SERVER_VERSION
    fAutostart = false;  // -- must be always false!!!
#  endif

   schSCManager = OpenSCManager( pszNodeName, NULL, SC_MANAGER_ALL_ACCESS );

   if ( schSCManager != NULL )
     {
     if ( sqlGetIndependentPgmPath ( szIndepPGMPath, TERM_WITH_DELIMITER_EO01, &RteError ) )
     {
       lstrcpy(szCommandFile, szIndepPGMPath);
       lstrcat(szCommandFile, DB_XSERVER_FILE_NAME);
     }
     else
     { // PTS 1107043
       MSGALL(( ERR_CANT_GET_PGM_PATH, (char *)DB_XSERVER_FILE_NAME))
       MSGALL(( ERR_RTE_ERROR, RteError.RteErrCode, (char *)RteError.RteErrText )) ;
       MSGALL(( ERR_OS_ERROR , RteError.OsErrCode,  (char *)RteError.OsErrText )) ;
	   return ( !NO_ERROR ) ;
     }

     schService = CreateService( schSCManager,
                                 pszServerName,
                                 pszServerName,
                                 SERVICE_ALL_ACCESS,
                                 SERVICE_WIN32_OWN_PROCESS,
                                 fAutostart ? SERVICE_AUTO_START : SERVICE_DEMAND_START,
                                 SERVICE_ERROR_NORMAL,
                                 szCommandFile,
                                 NULL,
                                 NULL,
                                 NULL,
                                 NULL,
                                 NULL);

     if (schService == NULL)
       rc = GetLastError();
     else
       {
       rc = sql49c_alloc_and_init_service_SD ( &pSD, schService );

       if ( ( rc == NO_ERROR ) && ( pSD != NULL ))
         {
         if (!SetServiceObjectSecurity( schService, DACL_SECURITY_INFORMATION, pSD ))
           {
           rc = GetLastError ();
           }
         }

       CloseServiceHandle(schService);

       if ( rc == NO_ERROR )
         {
             SAPDB_UInt4    regIndex = 0;

         // ---- insert version string and szIndepPGMPath entry into registry database
         if ( sql02_get_platform_id() == VER_PLATFORM_WIN32_NT )
           sp100_GetVersionString(COMP_NAME_RTE_NT_SP100, s100buildnumber, RTEVersionString);
         else
           sp100_GetVersionString(COMP_NAME_RTE_WIN9X_SP100, s100buildnumber, RTEVersionString );

         sp100_GetVersionID ( VersionIDType0_esp100, s100buildnumberEx, &RTEVersionID );

         strcpy ( szKey, pszServerName );
         strcat ( szKey, "\\"REG_SK_SERVICE_PARAM );

         RegistryEntries[regIndex].pszValueName = REG_VN_VERSION;
         RegistryEntries[regIndex].pValue       = RTEVersionString;
         RegistryEntries[regIndex].ulValueSize  = (ULONG)strlen(RTEVersionString);
         RegistryEntries[regIndex].ulValueType  = REG_SZ;

         regIndex++;

         RegistryEntries[regIndex].pszValueName = REG_VN_VERSION_ID;
         RegistryEntries[regIndex].pValue       = &RTEVersionID;
         RegistryEntries[regIndex].ulValueSize  = sizeof(RTEVersionID);
         RegistryEntries[regIndex].ulValueType  = REG_BINARY;

         regIndex++;

         dwValUseIndepPath = eo01_IsEnvVarDoNotUseIndepPathSet() ? 0 : 1 ;
         RegistryEntries[regIndex].pszValueName = REG_VN_USE_INDEP_PATH;
         RegistryEntries[regIndex].pValue       = &dwValUseIndepPath;
         RegistryEntries[regIndex].ulValueSize  = sizeof(DWORD);
         RegistryEntries[regIndex].ulValueType  = REG_DWORD;

         regIndex++;

         if (0 == dwValUseIndepPath)
         {
            if (!sqlGetDbrootPath (dbrootPath, 0, &RteError))
            {
                MSGALL(( ERR_XSERVER_MISSING_DBROOT));
                MSGALL(( ERR_RTE_ERROR, RteError.RteErrCode, (char *)RteError.RteErrText )) ;
                MSGALL(( ERR_OS_ERROR , RteError.OsErrCode,  (char *)RteError.OsErrText )) ;
	            return ( !NO_ERROR ) ;
            }
             
            RegistryEntries[regIndex].pszValueName = REG_VN_DBROOT;
            RegistryEntries[regIndex].pValue       = dbrootPath;
            RegistryEntries[regIndex].ulValueSize  = (ULONG)strlen(dbrootPath);
            RegistryEntries[regIndex].ulValueType  = REG_SZ;
            regIndex++;
         }

         RegistryEntries[regIndex].pszValueName = REG_VN_DEBUG_LEVEL;
         RegistryEntries[regIndex].pValue       = &dwDebugLevel;
         RegistryEntries[regIndex].ulValueSize  = sizeof(dwDebugLevel);
         RegistryEntries[regIndex].ulValueType  = REG_DWORD;

         regIndex++;

         RegistryEntries[regIndex].pszValueName = REG_VN_DIAGFILE_SIZE;
         RegistryEntries[regIndex].pValue       = &dwDiagFileSize;
         RegistryEntries[regIndex].ulValueSize  = sizeof(dwDiagFileSize);
         RegistryEntries[regIndex].ulValueType  = REG_DWORD;

         regIndex++;

         RegistryEntries[regIndex].pszValueName = REG_VN_NO_DNS_REVERSE_LOOKUP;
         RegistryEntries[regIndex].pValue       = &dwNoDNSReverseLookup;
         RegistryEntries[regIndex].ulValueSize  = sizeof(dwNoDNSReverseLookup);
         RegistryEntries[regIndex].ulValueType  = REG_DWORD;

         regIndex++;

         RegistryEntries[regIndex].pszValueName = REG_VN_PORTNO;
         RegistryEntries[regIndex].pValue       = &dwPortNo;
         RegistryEntries[regIndex].ulValueSize  = sizeof(dwPortNo);
         RegistryEntries[regIndex].ulValueType  = REG_DWORD;

         regIndex++;

         RegistryEntries[regIndex].pszValueName = REG_VN_NI_PORTNO;
         RegistryEntries[regIndex].pValue       = &dwNIPortNo;
         RegistryEntries[regIndex].ulValueSize  = sizeof(dwNIPortNo);
         RegistryEntries[regIndex].ulValueType  = REG_DWORD;

         regIndex++;

         RegistryEntries[regIndex].pszValueName = REG_VN_SSL_PORTNO;
         RegistryEntries[regIndex].pValue       = &dwSSLPortNo;
         RegistryEntries[regIndex].ulValueSize  = sizeof(dwSSLPortNo);
         RegistryEntries[regIndex].ulValueType  = REG_DWORD;

         rc = sql50_reg_put_service_values ( pszNodeName, szKey, regIndex + 1, RegistryEntries );

         if ( rc == NO_ERROR )
           {
           lstrcpy(szMsgAndCategoryFile, szIndepPGMPath);
           strcat (szMsgAndCategoryFile, REG_MSG_FILE );

           rc = sql60_install_event_source ( pszNodeName, 
                                             EVENT_SOURCE_NAME,
                                             szMsgAndCategoryFile,
                                             szMsgAndCategoryFile,
                                             SQLMSG_CATEGORY_LAST);
           }
         }
       }
     CloseServiceHandle(schSCManager);
     }
   else
     rc = GetLastError();

   if ( rc == ERROR_SERVICE_EXISTS )
     MSGCD(( INFO_SERVICE_ALREADY_INST, (char *)pszServerName ))
   else if (rc == ERROR_ACCESS_DENIED)
     MSGALL(( ERR_ACCESS_DENIED_ON_XX, (char *)pszServerName ))
   else if ( rc == RPC_S_SERVER_UNAVAILABLE )
     MSGALL(( ERR_SERVER_NODE_UNKNOWN, (char *)pszNodeName ))
   else if ( rc != NO_ERROR )
     MSGALL (( ERR_INSTALL_SERVICE, (char *)pszServerName, rc ))
   else
     MSGCD (( INFO_SERVICE_INSTALLED, (char *)pszServerName ));

   sql60_close_event_log ();

   DBGOUT;
   return ( rc );
   }

 /*------------------------------*/

externC
DWORD os969RemoveAnyXserverService  ( char *pszNodeName,
                                       char *pszServerName )
   {
   #undef  MF__
   #define MF__ MOD__"os969RemoveAnyXserverService"
   LONG                    rc                         = NO_ERROR;
   SC_HANDLE               schService                 = NULL;
   ULONG                   ulTimeout                  = TIMEOUT;
   SC_HANDLE               schSCManager;
   SERVICE_STATUS          ssSeviceStatus;

   DBGIN;

   sql60_open_event_log ( pszServerName, EVENT_SOURCE_NAME );

   schSCManager = OpenSCManager( pszNodeName, NULL, SC_MANAGER_CONNECT );

   if ( schSCManager != NULL )
     {
     schService = OpenService(schSCManager, pszServerName,
                              DELETE | SERVICE_STOP | SERVICE_QUERY_STATUS);

     if (schService == NULL)
       rc = GetLastError();
     else
       {
       ControlService(schService, SERVICE_CONTROL_STOP, &ssSeviceStatus);

       do
         {
         SLEEP( 1000 );
         ulTimeout--;

         if ( !QueryServiceStatus( schService, &ssSeviceStatus ))
           rc = GetLastError();
         else if ( ssSeviceStatus.dwCurrentState == SERVICE_STOPPED )
           break;

         if ( ulTimeout == 0 )
           rc = ERROR_TIMEOUT;
         }
       while ( rc == NO_ERROR );

       if ( !DeleteService(schService) )
         rc = GetLastError();

       CloseServiceHandle(schService);
       }
     CloseServiceHandle(schSCManager);
     }
   else
     rc = GetLastError();


   if ( rc != NO_ERROR )
     {
     if (rc == ERROR_ACCESS_DENIED)
       MSGALL(( ERR_ACCESS_DENIED_ON_XX, (char *)pszServerName ))
     else if ( rc == ERROR_SERVICE_DOES_NOT_EXIST )
       MSGCD (( ERR_SERVICE_UNKNOWN, (char *)pszServerName  ))
     else if ( rc == RPC_S_SERVER_UNAVAILABLE )
       MSGALL(( ERR_SERVER_NODE_UNKNOWN, (char *)pszNodeName ))
     else
       MSGALL(( ERR_REMOVE_SERVICE, (char *)pszServerName, rc ));
     }
   else
     MSGCD (( INFO_SERVICE_REMOVED, (char *)pszServerName ));

   sql60_close_event_log ();

   DBGOUT;
   return ( rc );
   }

/*===========================================================================*
 *  LOCAL/PRIVATE/PROTECTED FUNCTIONS (CODE)                                 *
 *===========================================================================*/

 static LONG os969CheckServiceParams ( char *pszServerName )
   {
   #undef  MF__
   #define MF__ MOD__"os969CheckServiceParams"
   PATHNAME                szSubKey;
   REG_ENTRY_REC           RegistryEntries[2];
   LONG                    rc  = NO_ERROR;
   tsp100_VersionID0       RTEVersionID;
   tsp00_Versionc          RTEVersionString;
   tsp00_Versionc          CurrRTEVersionString;
   tsp100_VersionResult    Result;

   DBGIN;

   strcpy ( szSubKey, pszServerName );
   strcat ( szSubKey, "\\"REG_SK_SERVICE_PARAM );

   RegistryEntries[0].pszValueName = REG_VN_VERSION;
   RegistryEntries[0].pValue       = RTEVersionString;
   RegistryEntries[0].ulValueSize  = sizeof(RTEVersionString);
   RegistryEntries[0].ulValueType  = REG_SZ;
   RegistryEntries[0].rc           = NO_ERROR;

   RegistryEntries[1].pszValueName = REG_VN_VERSION_ID;
   RegistryEntries[1].pValue       = &RTEVersionID;
   RegistryEntries[1].ulValueSize  = sizeof(RTEVersionID);
   RegistryEntries[1].ulValueType  = REG_BINARY;
   RegistryEntries[1].rc           = NO_ERROR;

   sql50_reg_get_service_values ( NULL, szSubKey, 2, RegistryEntries );

   if ( RegistryEntries[0].rc != NO_ERROR )
     {
     strcpy ( RTEVersionString, "???" );
     MSGALL (( WRN_XSER_CANNOT_READ_REG, REG_VN_VERSION ))
     DBGOUT;
     return ( NO_ERROR );
     }

   if ( RegistryEntries[1].rc != NO_ERROR )  // - no version id enty found?
     {
     MSGALL (( WRN_XSER_CANNOT_READ_REG, REG_VN_VERSION_ID ));
     DBGOUT;
     return ( NO_ERROR );
     }

   Result = sp100_CompareVersionIDs(&RTEVersionID);

   if ( Result == IsOlder_esp100 )
     {
     // --- set the RTE version string
     if ( sql02_get_platform_id() == VER_PLATFORM_WIN32_NT )
       sp100_GetVersionString(COMP_NAME_RTE_NT_SP100, s100buildnumber, CurrRTEVersionString);
     else
       sp100_GetVersionString(COMP_NAME_RTE_WIN9X_SP100, s100buildnumber, CurrRTEVersionString );

     MSGALL (( ERR_XSER_WRONG_VERSION, (char *)RTEVersionString, (char *)CurrRTEVersionString ))
     DBGOUT;
     return ( !NO_ERROR );
     }
/* PTS 1104558 */
#ifdef CHECK_DBROOT
   else
   {
     PSZ                     pszDBRoot;
     if ( sql01c_get_dbroot (&pszDBRoot) == FALSE )
       pszDBRoot = NULL;

     if (( pszDBRoot != NULL) && ( Result == IsEqual_esp100 ))
     {
       PATHNAME                szDBRoot;
       RegistryEntries[0].pszValueName = REG_VN_DBROOT;
       RegistryEntries[0].pValue       = szDBRoot;
       RegistryEntries[0].ulValueSize  = sizeof(szDBRoot);
       RegistryEntries[0].ulValueType  = REG_SZ;

       rc = sql50_reg_get_service_values ( NULL, szSubKey, 1, RegistryEntries );

       if ( rc != NO_ERROR )
       {
         MSGALL (( WRN_XSER_CANNOT_READ_REG, REG_VN_DBROOT ));
       }
       else if ( strcmp ( szDBRoot, pszDBRoot ))
       {
       MSGALL (( WRN_XSER_WRONG_DBROOT, (char *)szDBRoot ));
       }
     }
   }
#endif

   DBGOUT;
   return ( NO_ERROR );
   }



/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
