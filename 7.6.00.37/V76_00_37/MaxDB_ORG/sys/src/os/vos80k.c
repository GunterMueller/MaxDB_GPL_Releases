/*!
  @file           vos80k.c
  @author         RaymondR
  @brief          kernel entry point
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
//#include "gos00.h"
#include "gos41.h"
#include "heo00.h"
#include "heo46.h"
#include "geo007_1.h"
#include "gos00k.h"
#include "gos60.h"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageOutput.h"
#include "RunTime/Tasking/RTETask_Task.h"
#include "RunTime/RTE_DiagPipe.h"
#include "hsp100.h" 
#include <WinCon.h>

// !!!!!!!!!!!!!!!!!!!!!! work around !!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//
// The prototype of 'SetServiceBits' was not defined correctly in
// 'lmservice.h' (Visual C++ 4.0).
//
//#include           <lmcons.h>
//#include           <lmservice.h>
//

BOOL __stdcall SetServiceBits( IN SERVICE_STATUS_HANDLE hServiceStatus,
                               IN DWORD                 dwServiceBits,
                               IN BOOL                  bSetBitsOn,
                               IN BOOL                  bUpdateImmediately );
// !!!!!!!!!!!!!!!!!!!!!! work around !!!!!!!!!!!!!!!!!!!!!!!!!!!!!

/*
 *  DEFINES
 */
#define MOD__  "VOS80KC : "
#define MF__   MOD__"UNDEFINED"


/*
 *  MACROS
 */


/*
 *  LOCAL TYPE AND STRUCT DEFINITIONS
 */


/*
 *  EXPORTED VARIABLES
 */


/*
 * LOCAL FUNCTION PROTOTYPES
 */

 VOID WINAPI   sql80k_ServiceMain     ( DWORD argc, LPTSTR *argv );

 BOOL         sql80k_pause_enabled    ( VOID );


/*
 * LOCAL VARIABLES
 */

 static CHAR                  szServiceName[60];
 static SERVICE_STATUS_HANDLE hServiceStatus;
 static SERVICE_STATUS        Status  = {
                                        SERVICE_WIN32_OWN_PROCESS,
                                        0,
                                        SERVICE_ACCEPT_SHUTDOWN,
                                        NO_ERROR, 0,
                                        0,
                                        0 };

 static SERVICE_TABLE_ENTRY DispTab[] = {
                                        { szServiceName,
                                          (LPSERVICE_MAIN_FUNCTION)
                                          sql80k_ServiceMain },
                                        { NULL, NULL }
                                        };


/*
 * ========================== GLOBAL FUNCTIONS ================================
 */

 VOID sql80k_NewSrvState ( SAPDB_UInt4 DatabaseState )
   {
   #undef  MF__
   #define MF__ MOD__"sql80k_NewSrvState"

   DBGPAS;

   switch( DatabaseState )
     {
     case SERVER_STARTING:
       Status.dwCurrentState            = SERVICE_START_PENDING;
       Status.dwServiceType             = SERVICE_WIN32_OWN_PROCESS;
       Status.dwWin32ExitCode           = NO_ERROR;
       Status.dwServiceSpecificExitCode = NO_ERROR;
       Status.dwWaitHint                = 240000;
       Status.dwControlsAccepted        = SERVICE_ACCEPT_SHUTDOWN;
       Status.dwCheckPoint++;
       break;

     case SERVER_COLD:
     case SERVER_WARM:
     case SERVER_STANDBY_EO00:
       Status.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;

       if ( sql80k_pause_enabled() )
       {
           Status.dwControlsAccepted  |= SERVICE_ACCEPT_PAUSE_CONTINUE;

           if ( kgs.fServicePaused )
               Status.dwCurrentState = SERVICE_PAUSED;
           else
               Status.dwCurrentState = ( SERVER_COLD == DatabaseState && 
                                         kgs.ulStartupOptions & FORCE_AUTORESTART ) ? SERVICE_START_PENDING : SERVICE_RUNNING;
       }
       else
           Status.dwCurrentState = ( SERVER_COLD == DatabaseState &&  
                                     kgs.ulStartupOptions & FORCE_AUTORESTART ) ? SERVICE_START_PENDING : SERVICE_RUNNING;

       Status.dwWin32ExitCode           = NO_ERROR;
       Status.dwServiceSpecificExitCode = NO_ERROR;
       Status.dwWaitHint                = 0;
       break;

     case SERVER_STOPPED :
       Status.dwCurrentState            = SERVICE_STOPPED;
       Status.dwWin32ExitCode           = NO_ERROR;
       Status.dwServiceSpecificExitCode = NO_ERROR;
       Status.dwWaitHint                = 0;
       Status.dwCheckPoint              = 0;

     default:
       Status.dwControlsAccepted        = SERVICE_ACCEPT_STOP |
                                          SERVICE_ACCEPT_SHUTDOWN;
       Status.dwCurrentState            = SERVICE_STOP_PENDING;
       Status.dwWin32ExitCode           = NO_ERROR;
       Status.dwServiceSpecificExitCode = NO_ERROR;
       Status.dwWaitHint                = sql50k_get_termination_timeout();
       Status.dwCheckPoint++;
     }

   SetServiceStatus(hServiceStatus, &Status);

   return;
   }

 /*------------------------------*/

 VOID WINAPI sql80k_CtrlHandler (DWORD fdwCtrl)
   {
   #undef  MF__
   #define MF__ MOD__"sql80k_CtrlHandler"
   PUKT_CTRL_REC            pTmpUKTCtrl;

   DBGPAS;
   switch(fdwCtrl)
     {
     case SERVICE_CONTROL_STOP:         // --- Stop the service.
         Status.dwCurrentState     = SERVICE_STOP_PENDING;
         Status.dwWin32ExitCode    = NO_ERROR;
         Status.dwCheckPoint       = 0;
         Status.dwWaitHint         = 60000;
         Status.dwControlsAccepted = 0;

         sql41c_post_event_sem ( kgs.Coord.ThrdCtrlHeader.hevSem, "Coord" );
         break;

     case SERVICE_CONTROL_SHUTDOWN:
         Status.dwCurrentState     = SERVICE_STOP_PENDING;
         Status.dwWin32ExitCode    = NO_ERROR;
         Status.dwCheckPoint       = 0;
         Status.dwWaitHint         = 60000;
         Status.dwControlsAccepted = 0;
         kgs.fWinShutdownPending   = true;

         sql41c_post_event_sem ( kgs.Coord.ThrdCtrlHeader.hevSem, "Coord" );
         break;

     case SERVICE_CONTROL_PAUSE:
         kgs.fServicePaused        = TRUE;
         Status.dwCurrentState     = SERVICE_PAUSED;
         Status.dwWin32ExitCode    = NO_ERROR;
         Status.dwCheckPoint       = 0;
         Status.dwWaitHint         = 0;
         MSGALL(( INFO_KERNEL_PAUSED ))
         break;

     case SERVICE_CONTROL_CONTINUE:
         kgs.fServicePaused        = FALSE;
         Status.dwCurrentState     = SERVICE_RUNNING;
         Status.dwWin32ExitCode    = NO_ERROR;
         Status.dwCheckPoint       = 0;
         Status.dwWaitHint         = 0;
         MSGALL(( INFO_KERNEL_CONTINUED ))

        //  --- wake all user kernel threads
        for ( pTmpUKTCtrl =  kgs.pFirstUKTCtrl; pTmpUKTCtrl <= kgs.pLastUKTCtrl; pTmpUKTCtrl++ )
        {
            if ( pTmpUKTCtrl->ThrdCtrlHeader.hevSem != (HEV) INVALID_HANDLE_VALUE )
                sql72k_wake_ukt( pTmpUKTCtrl );
        }

         break;

     case SERVICE_CONTROL_INTERROGATE:  // --- Update the service status.
     default:
         break;
     }

   SetServiceStatus(hServiceStatus, &Status);

   return;
   }

 /*------------------------------*/

BOOL  sql80k_pause_enabled ( VOID )
  {
  #undef  MF__
  #define MF__ MOD__"sql80k_pause_enabled"
  DWORD             PauseEnabled = 0;
  REG_ENTRY_REC     RegistryEntries[1];
  PATHNAME          szSubKey;
  LONG              rc;
  
  DBGIN;
  
  RegistryEntries[0].pszValueName = REG_VN_PAUSE_ENABLED;
  RegistryEntries[0].pValue       = &PauseEnabled;
  RegistryEntries[0].ulValueSize  = sizeof(DWORD);
  RegistryEntries[0].ulValueType  = REG_DWORD;
  
  rc = sql50_reg_get_applic_values ( NULL, HKEY_LOCAL_MACHINE,
                                     NULL, 1, RegistryEntries );


  if (( NO_ERROR == rc ) && ( 0 != PauseEnabled ))
  {
      DWORD  Pause = 0;

      RegistryEntries[0].pszValueName = REG_VN_PAUSE;
      RegistryEntries[0].pValue       = &Pause;
      RegistryEntries[0].ulValueSize  = sizeof(DWORD);
      RegistryEntries[0].ulValueType  = REG_DWORD;

      strcpy ( szSubKey, kgs.szServiceName);
      strcat ( szSubKey, "\\"REG_SK_SERVICE_PARAM );

      rc = sql50_reg_get_service_values ( NULL, szSubKey, 1, RegistryEntries );

      if ( NO_ERROR == rc )
        kgs.fServicePaused = 0 != Pause;

      // reset value
      Pause = 0;
      sql50_reg_put_service_values ( NULL, szSubKey, 1, RegistryEntries );
  }
  
  DBGOUT;
  return 0 != PauseEnabled;
  }


 /*------------------------------*/

 VOID WINAPI sql80k_ServiceMain (DWORD argc, LPTSTR *argv)
   {
   #undef  MF__
   #define MF__ MOD__"sql80k_ServiceMain"
   PCHAR                pszService;
   PCHAR                pszServerDB = NULL;
   DWORD                Result      = NO_ERROR;
   SERVICE_STATUS       TmpStatus;
   DBGIN;

   sql60_disable_default_device ();

   kgs.fWinShutdownPending = false;

   if (hServiceStatus = RegisterServiceCtrlHandler
                        (szServiceName,(LPHANDLER_FUNCTION)sql80k_CtrlHandler))
     {
     SetServiceBits ( hServiceStatus, SV_TYPE_SAPDB, TRUE, TRUE );

     // --- set initial service state...
     sql80k_NewSrvState( *kgs.pDBState );

     // --- save service name
     strcpy( kgs.szServiceName, argv[0] );

     if ( !strncmp( argv[0], SERVICE_ID_GW, sizeof(SERVICE_ID_GW) - 1 ))
      kgs.fIsGateway = TRUE;
     else
      kgs.fIsGateway = FALSE;

     // --- extract the database name from the service display name (in argv[0])
     for (pszService = argv[0]; *pszService && *pszService!='-'; ++pszService);

     // --- and now, start the kernel
     if ( *pszService )
       {
       argc--;
       argv++;

       pszServerDB = pszService + 1;

       pszService = strstr ( pszServerDB, SERVICE_QUICK_EXT );

       if ( pszService == NULL )
         pszService = strstr ( pszServerDB, SERVICE_SLOW_EXT );

       if ( pszService == NULL )
         pszService = strstr ( pszServerDB, SERVICE_TEST_EXT );

       if ( pszService )
        *pszService = '\0';

       RTE_ConnectToDiagPipe(pszServerDB);

       if (Result = sql81k_Coordinator( pszServerDB, argc, argv ))
         {
         Status.dwWin32ExitCode           = ERROR_SERVICE_SPECIFIC_ERROR;
         Status.dwServiceSpecificExitCode = Result;
         }
       }

     TmpStatus = Status;
     TmpStatus.dwCurrentState = SERVICE_STOPPED;
     SetServiceStatus(hServiceStatus, &TmpStatus);
     }
   else
     {
     MSGALL((ERR_CANT_START_DISPATCHER));
     }

   EXITPROCESS( Result );
   return;
   }

/*------------------------------*/

 INT APIENTRY WinMain ( HINSTANCE hInstance,
                        HINSTANCE hPrevInstance,
                        LPSTR     lpszCmdLine,
                        INT       nCmdShow )
   {
   #undef  MF__
   #define MF__ MOD__"WinMain"
   APIRET       rc = NO_ERROR;
   PSZ          pszBegin;
   PSZ          pszEnd;
   PROG_ARGS    Args;

   SAPDBErr_SetMessageOutputCall(os60MessageOutputCall);

   Args.ulCount = 0;
   pszBegin     = lpszCmdLine;

   do
   {
       for ( ; *pszBegin == ' '; pszBegin++ ){;}
       Args.ppszVal[Args.ulCount] = pszBegin;
       Args.ulCount++;

       pszEnd = strchr ( pszBegin, ' ' );

       if ( pszEnd != NULL )
       {
           *pszEnd  = '\0';
           pszBegin = pszEnd + 1;
       }
   }
   while (( pszEnd != NULL ) && ( Args.ulCount < MAX_PROG_ARGS ));

   if ( ! strcmp("-V", Args.ppszVal[0] ) )
   {
       tsp00_Versionc currentVersion;
       char buffer[sizeof(tsp00_Versionc)+20];

      // --- set the RTE version string
      sp100_GetVersionString(COMP_NAME_RTE_NT_SP100, s100buildnumber, &currentVersion[0]);

#ifdef BIT64
      strcpy(buffer, "64BIT Kernel : ");
#else
      strcpy(buffer, "32BIT Kernel : ");
#endif
      strcat( buffer, &currentVersion[0] );

      sql60_enable_message_box();
      sql60_msg_box( 0, INFO_TYPE, "", buffer );
      return 0;
   }

   // --- init the kernel global section
   memset ( &kgs, 0, sizeof (KERNEL_GLOBAL_SEC));

   kgs.ulCurrTime          = time (NULL);
   kgs.ulDatabaseStartTime = kgs.ulCurrTime;
   kgs.tmpDBState          = SERVER_STARTING;
   kgs.pDBState            = &kgs.tmpDBState;
   kgs.pKernelEntryFunc    = RTETask_TaskMain;
   GETPROCESSID(&kgs.KernelPid);

    strcpy ( szServiceName, DB_TITLE );
    strcat ( szServiceName, " : 0x" );
    _ultoa ( kgs.KernelPid, &(szServiceName[strlen(szServiceName)]), 16 );

    if ( false == sql81k_PortableInstallationAcitve() )
      {
      kgs.fRunningAsNTService = TRUE;

      sql60_open_event_log ( szServiceName, EVENT_SOURCE_NAME );

      if (!StartServiceCtrlDispatcher(DispTab) &&
          Status.dwCurrentState != SERVICE_STOPPED)
        {
        rc = GetLastError();

        if (rc != ERROR_FAILED_SERVICE_CONTROLLER_CONNECT )
          {
          MSGALL((ERR_CANT_START_DISPATCHER));
          return  rc;
          }
        }
      else
        {
        return NO_ERROR;
        }
      }

   //
   // --- we are not running as service, start a foreground window 
   //
   kgs.fRunningAsNTService = FALSE;
   sql60_disable_default_device ();

   rc = sql80kn_CreateWindow ( hInstance, hPrevInstance, &Args, nCmdShow );

   return  ( rc );
   }

/*
 * ========================== LOCAL FUNCTIONS =================================
 */

