/*!
  @file           vos968.c
  @author         RaymondR
  @brief          TCP/IP_DATABASE_SERVER
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
#define RTE_USE_SOCK

#include		   "gos96.h"
#include		   "gos960.h"
#include		   "gos962.h"
#include		   "gos964.h"
#include           "hsp100.h"
#include		   "gos969.h"
#include           "heo01.h"
#include           "geo007_1.h"
#include           "geo007_2.h"
#include           "geo43_0.h"
#include           "heo670.h"
#include           "geo40.h"
#include           "SAPDBCommon/SAPDB_Names.h"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define MOD__  "VOS968  : "
#define MF__   MOD__"UNDEFINED"

#define TIMEOUT                   120
#define DIAGSIZE                  100                      // - 4KByte Pages

#define OPT_SHOW_USAGE            0x001
#define OPT_START_SERVER          0x002
#define OPT_STOP_SERVER           0x004
#define OPT_START_SERVICE         0x008
#define OPT_STOP_SERVICE          0x010
#define OPT_INSTALL_SERVICE       0x020
#define OPT_REMOVE_SERVICE        0x040
#define OPT_RUN                   0x080
#define OPT_UPDATE_SERVICE        0x100
#define OPT_SHOW_VERSION          0x200
#define OPT_FAST_MODE             0x400


#define OPTION_STRING            "XsirD:RkS:n:huVN:Fq"

#define ICON_SERV                 130
#define SM_TRAY_CALLBACK          WM_TIMER  // - do nothing


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

static int os968Main ( int argc, char  *argv[] ) ;
static void os968_ShowEnviron(char **envp);

/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/

extern int                       sql80_OptInd;
extern int                       sql80_OptErr;
extern char*                     sql80_OptArg;
extern HMODULE                   hModule;


/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/

GLOBAL_XSERVER_REC      gxr;

/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

BOOL __stdcall SetServiceBits( IN SERVICE_STATUS_HANDLE hServiceStatus,
                               IN DWORD                 dwServiceBits,
                               IN BOOL                  bSetBitsOn,
                               IN BOOL                  bUpdateImmediately );

static VOID _System os968Finish              ( ULONG  termcode );
static INT          os968Coordinator         ( VOID );
static INT          os968Server              ( VOID );

static BOOL WINAPI  os968ConsoleCtrlHandler  ( DWORD                  dwCtrlType );
static VOID WINAPI  os968ServiceCtrlHandler  ( DWORD                  dwCtrlCode );
       VOID         os968UpdateDebugLevelFromRegistry ( VOID );
static VOID WINAPI  os968Service             ( DWORD                    argc, 
                                               LPTSTR *                 argv );
static BOOL         os968DebuggerIsDrWatson  ( VOID );
static void         os968UpdateVersion       ( VOID );
static LONG         os968_OpenDiagfile       ( PSECURITY_ATTRIBUTES     pSA,
											   DWORD                    dwDiagFileSize );
static teo00_Bool   os968_SetSymbolPath      ();
static LONG         os968_SetWrkDir          ();
static teo00_Bool   os968_ParsePortArgs      ( char*                    arg, 
                                               unsigned short*          pPort,
                                               unsigned short*          pPortNI, 
                                               unsigned short*          pPortSSL );

static teo00_Bool os968_PortableInstallationAcitve();

static VOID os968_InitXserverSpecification   ( BOOL          fQuietMode,
                                               BOOL          fPortableInstallation,
                                               BOOL          fNTService,
                                               BOOL          fNoDNSReverseLookup,
                                               USHORT        usDebugLevel,
                                               USHORT        usServicePort,
                                               USHORT        usNIServicePort,
                                               USHORT        usNISSLServicePort );
static bool os968_BuildHostSpecificName      ( char *        buffer, 
                                               DWORD         bufferSize, 
                                               char const*   portNo, 
                                               char const*   prefix, 
                                               char const*   suffix );

/*===========================================================================*
 *  PUBLIC FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

int main ( int argc, char  *argv[] )
{
  return os968Main(argc, argv);
}


/*------------------------------*/

VOID  sql96c_abort ( VOID )
  {
  #undef  MF__
  #define MF__ MOD__"sql96c_abort"

  DBGPAS;

  /*
   * activide exception handler
   */
  RaiseException(XCPT_ABORT_EXCEPTION, 0, 0, NULL);

  // ---  should not come here
  EXITPROCESS( 1 );
  }

 /*------------------------------*/

 VOID sql96c_NewSrvState ( DWORD dwState,  ULONG ulExitCode )
   {
   #undef  MF__
   #define MF__ MOD__"sql96c_NewSrvState"
   LONG                 rc = NO_ERROR;

   DBGPAS;

   switch( dwState )
     {
     case XSERVER_INITIALIZE:
       gxr.ssStatus.dwCurrentState            = SERVICE_START_PENDING;
       gxr.ssStatus.dwServiceType             = SERVICE_WIN32_OWN_PROCESS;
       gxr.ssStatus.dwWin32ExitCode           = NO_ERROR;
       gxr.ssStatus.dwServiceSpecificExitCode = NO_ERROR;
       gxr.ssStatus.dwWaitHint                = 10000;
       gxr.ssStatus.dwCheckPoint              = 0;
       gxr.ssStatus.dwControlsAccepted        = SERVICE_ACCEPT_SHUTDOWN;
       return;

     case XSERVER_STARTING:
       gxr.ssStatus.dwCurrentState            = SERVICE_START_PENDING;
       gxr.ssStatus.dwWaitHint                = 30000;
       gxr.ssStatus.dwCheckPoint++;
       break;

     case XSERVER_RUNNING:
       gxr.ssStatus.dwControlsAccepted        = SERVICE_ACCEPT_STOP           |
                                                SERVICE_ACCEPT_SHUTDOWN;

       #if defined ( PAUSE_SERVICE_ALLOWED )
        gxr.ssStatus.dwControlsAccepted      |= SERVICE_ACCEPT_PAUSE_CONTINUE;
       #endif

       if ( gxr.fServicePaused )
         gxr.ssStatus.dwCurrentState          = SERVICE_PAUSED;
       else
         gxr.ssStatus.dwCurrentState          = SERVICE_RUNNING;

       gxr.ssStatus.dwWin32ExitCode           = NO_ERROR;
       gxr.ssStatus.dwServiceSpecificExitCode = NO_ERROR;
       gxr.ssStatus.dwWaitHint                = 0;
       gxr.ssStatus.dwCheckPoint              = 0;
       break;

     case XSERVER_STOPPED:
     case XSERVER_ABORT:
       gxr.ssStatus.dwControlsAccepted        = SERVICE_ACCEPT_STOP |
                                                SERVICE_ACCEPT_SHUTDOWN;
       gxr.ssStatus.dwCurrentState            = SERVICE_STOPPED;
       gxr.ssStatus.dwWaitHint                = 0;
       gxr.ssStatus.dwCheckPoint              = 0;


       if ( ulExitCode != NO_ERROR )
         {
         gxr.ssStatus.dwWin32ExitCode           = ERROR_SERVICE_SPECIFIC_ERROR;
         gxr.ssStatus.dwServiceSpecificExitCode = ulExitCode;
         }
        else
         {
         gxr.ssStatus.dwWin32ExitCode           = NO_ERROR;
         gxr.ssStatus.dwServiceSpecificExitCode = NO_ERROR;
         }
      break;

     default:
      gxr.ssStatus.dwCheckPoint++;
      break;
     }


   // Report the status of the service to the service control manager.
   if (!SetServiceStatus ( gxr.sshStatusHandle, &gxr.ssStatus ))
     {
     rc = GetLastError();
     MSGALL (( ERR_SET_SERVICE_STATUS, rc ));
     }

   return;
   }

/*------------------------------*/

DWORD os968ExceptionHandler (LPEXCEPTION_POINTERS lpExcPtrs )
  {
  #undef  MF__
  #define MF__ MOD__"os968ExceptionHandler"
  INT                    i;
  PSZ                    pszPhysDiagFileName = NULL;
  PEXCEPTION_RECORD      pExceptionRecord    = lpExcPtrs->ExceptionRecord;

  DBGPAS;

  sql60_reset_crit_section();

  switch(pExceptionRecord->ExceptionCode)
    {
    case STATUS_IN_PAGE_ERROR            :
    case STATUS_NONCONTINUABLE_EXCEPTION :
    case STATUS_INVALID_DISPOSITION      :
    case STATUS_ACCESS_VIOLATION         :
    case STATUS_INTEGER_DIVIDE_BY_ZERO   :
    case STATUS_FLOAT_DIVIDE_BY_ZERO     :
    case STATUS_FLOAT_INVALID_OPERATION  :
    case STATUS_ILLEGAL_INSTRUCTION      :
    case STATUS_PRIVILEGED_INSTRUCTION   :
    case STATUS_INTEGER_OVERFLOW         :
    case STATUS_FLOAT_OVERFLOW           :
    case STATUS_FLOAT_UNDERFLOW          :
    case STATUS_FLOAT_DENORMAL_OPERAND   :
    case STATUS_FLOAT_INEXACT_RESULT     :
    case STATUS_FLOAT_STACK_CHECK        :
    case STATUS_DATATYPE_MISALIGNMENT    :
    case STATUS_ARRAY_BOUNDS_EXCEEDED    :
    case STATUS_STACK_OVERFLOW           :

      if ( gxr.dwState != XSERVER_ABORT )
        {
        for (i = pExceptionRecord->NumberParameters; i < EXCEPTION_MAXIMUM_PARAMETERS; i++)
          pExceptionRecord->ExceptionInformation[i] = 0;

        MSGCD ((ERR_XSER_EXCEPTION, pExceptionRecord->ExceptionCode, pExceptionRecord->ExceptionAddress,
                                    pExceptionRecord->ExceptionInformation[0], 
                                    pExceptionRecord->ExceptionInformation[1],
                                    pExceptionRecord->ExceptionInformation[2], 
                                    pExceptionRecord->ExceptionInformation[3]));
        }
      break;

    case XCPT_ABORT_EXCEPTION:
      break;

    case STATUS_CONTROL_C_EXIT:
       switch(pExceptionRecord->ExceptionInformation[0])
         {
         case CTRL_LOGOFF_EVENT   :
         case CTRL_CLOSE_EVENT    :
           if ( gxr.fNTService )
             {
             DBG3 ((MF__, "Exception Ignored!"));
             return(EXCEPTION_CONTINUE_EXECUTION);
             }
           MSGD (( INFO_XSER_EXCEP_SIG_KILL ));
           break;

         case CTRL_SHUTDOWN_EVENT :
           MSGD (( INFO_XSER_EXCEP_SIG_KILL ));
           break;
        case CTRL_C_EVENT       :
        case CTRL_BREAK_EVENT   :
         default                :
           if ( gxr.fNTService )
             {
             DBG3 ((MF__, "Exception Ignored!"));
             return(EXCEPTION_CONTINUE_EXECUTION);
             }
           break;
         }
      break;

    default:
      if ( pExceptionRecord->ExceptionFlags & EXCEPTION_NONCONTINUABLE )
        {
        for (i = pExceptionRecord->NumberParameters; i < EXCEPTION_MAXIMUM_PARAMETERS; i++)
          pExceptionRecord->ExceptionInformation[i] = 0;

        MSGCD ((ERR_XSER_EXCEPTION, pExceptionRecord->ExceptionCode, pExceptionRecord->ExceptionAddress,
                                    pExceptionRecord->ExceptionInformation[0], 
                                    pExceptionRecord->ExceptionInformation[1],
                                    pExceptionRecord->ExceptionInformation[2], 
                                    pExceptionRecord->ExceptionInformation[3]));
        break;
        }
      return(EXCEPTION_CONTINUE_SEARCH);
    }

  if ( pExceptionRecord->ExceptionCode == STATUS_CONTROL_C_EXIT ) // - termination by signal?
    return(EXCEPTION_CONTINUE_SEARCH);

  if ( gxr.dwState != XSERVER_ABORT )  // --- don't display abort
    {                                  //     information twice
    eo670TraceContext traceContext;

    gxr.dwState  = XSERVER_ABORT;

    eo670_CTraceStackInit();

    traceContext.threadHandle   = GetCurrentThread();
    traceContext.pThreadContext = lpExcPtrs->ContextRecord;

    eo670_CTraceContextStack( &traceContext );

    sql60_get_diag_filename ( &pszPhysDiagFileName );

    if ( pszPhysDiagFileName != NULL)
      {
      MSGCEL (( INFO_XSER_ABORTS_DIAG_INFO, pszPhysDiagFileName ));
      MSGD   (( INFO_XSER_ABORTS ));
      }
    else
      MSGALL (( INFO_XSER_ABORTS ));

    eo670_WriteMiniDump ( THREADID, lpExcPtrs, SAPDB_SRV_MINI_DUMP, true, false, &gxr.AdminSA );   // write full mini dump file

    if ( gxr.fNTService )
      sql96c_NewSrvState( gxr.dwState, gxr.ulCrashRc );
    }

  os968Finish (0);

  if ( gxr.fDebuggerIsDrWatson )
    {
    return(EXCEPTION_CONTINUE_SEARCH);  // - DrWatson will write a dump file!
    }

  EXITPROCESS( 1 );
  return(EXCEPTION_CONTINUE_SEARCH);
  }

/*===========================================================================*
 *  LOCAL/PRIVATE/PROTECTED FUNCTIONS (CODE)                                 *
 *===========================================================================*/

static int os968Main ( int argc, char  *argv[] )
  {
  LONG                    rc                    = NO_ERROR;
  BOOL                    fCommandError         = FALSE;
  ULONG                   ulStrtOpt             = OPT_START_SERVER;
  INT                     OptionChar;
  CHAR                    szCmdLine[512];
  INT                     i;
  INT                     newDebugLevel         = -1;
  PCHAR                   pszNodeName           = NULL;
  BOOL                    fPortableInstallation = os968_PortableInstallationAcitve();
  BOOL                    fNTService            = FALSE;
  USHORT                  usDebugLevel          = 0;
  BOOL                    fNoDNSReverseLookup   = FALSE;
  USHORT                  usServicePort         = 0;
  USHORT                  usNIServicePort       = 0;
  USHORT                  usNISSLServicePort    = 0;
  BOOL                    fQuietMode            = FALSE;
  SERVICE_TABLE_ENTRY     dispatchTable[]       = { { "INIT",
                                                    (LPSERVICE_MAIN_FUNCTION)os968Service },
                                                    { NULL, NULL } };
  memset( &gxr, 0, sizeof(gxr) );

  if ( false == fPortableInstallation )
    {
    fNTService = TRUE;
    ulStrtOpt  = OPT_RUN;
    }

  while ( (OptionChar = sql80_GetOpt ( argc, argv, OPTION_STRING )) != -1 )
    {
    switch ( OptionChar )
      {
       case 'X' :  // Obsolet
               break;
       case 's' :  // --- hidden parameter
               if ( fNTService )
                 ulStrtOpt = OPT_START_SERVICE;
               break;
       case 'i' :  // --- hidden parameter
               if ( fNTService )
                 ulStrtOpt = OPT_INSTALL_SERVICE;
               else
                 fCommandError = TRUE;
               break;
       case 'u' : 
               if ( fNTService )
                 ulStrtOpt = OPT_UPDATE_SERVICE;
               else
                 fCommandError = TRUE;
               break;

       case 'r' :
               if ( fNTService )
                 ulStrtOpt = OPT_REMOVE_SERVICE;
               else
                 fCommandError = TRUE;
               break;
       case 'D' :
               if (( ulStrtOpt   != OPT_INSTALL_SERVICE ) &&
                   ( ulStrtOpt   != OPT_REMOVE_SERVICE  ) &&
                   ( pszNodeName == NULL  ))
                 {
                 fNTService   = FALSE;
                 usDebugLevel = sql80_OptArg[ 0 ] - '0';
                 }
               else
                 fCommandError = TRUE;
               break;
       case 'N' : /* PTS 1111208 */
               if (( ulStrtOpt   == OPT_INSTALL_SERVICE ) ||
                   ( ulStrtOpt   == OPT_REMOVE_SERVICE  ) ||
                   ( ulStrtOpt   == OPT_UPDATE_SERVICE  ) ) {
                 fCommandError = TRUE;
			   }
			   else
			   {
                 newDebugLevel = sql80_OptArg[ 0 ] - '0';
			   }
               break;
       case 'R' :  // --- hidden parameter
               if ( pszNodeName  == NULL  )
                 {
                 fNTService = FALSE;
                 ulStrtOpt      = OPT_RUN;
                 }
               else
                 fCommandError = TRUE;

               break;
       case 'F' :
               fNoDNSReverseLookup = TRUE;
               break;
       case 'n' :
               if ( fNTService )
                 {
                 pszNodeName  = sql80_OptArg;

                 if ( ulStrtOpt  == OPT_RUN )     // - this option is not
                   ulStrtOpt = OPT_START_SERVICE; //   allowed, assuming start
                 }                                //   request.
               else
                 fCommandError = TRUE;
               break;
       case 'k' :
               if ( fNTService )
                 ulStrtOpt = OPT_STOP_SERVICE;
               else
                 ulStrtOpt = OPT_STOP_SERVER;
               break;
      case 'h' :
              ulStrtOpt = OPT_SHOW_USAGE;
              break;

      case 'S' :
            if  ( false == os968_ParsePortArgs( sql80_OptArg, 
                                                &usServicePort,
                                                &usNIServicePort,
                                                &usNISSLServicePort ))
            {
                fCommandError = TRUE;
            }
              break;
      case 'q' :  // --- hidden parameter "quiet mode"
              fQuietMode = TRUE;
              break;
      case 'V' :  // --- CR 1104459
              ulStrtOpt = OPT_SHOW_VERSION ;
              break;
      default :
              fCommandError = TRUE;
      }
    }

    if ( fNTService )
    {
        if (( fNoDNSReverseLookup   == TRUE  ) &&
            (( ulStrtOpt            != OPT_INSTALL_SERVICE ) &&
             ( ulStrtOpt            != OPT_UPDATE_SERVICE  ))) 
        {
            ulStrtOpt  = OPT_UPDATE_SERVICE; // try update
        }
    }

  if ( (argc - sql80_OptInd) || (fCommandError == TRUE) )
    {
    printf ( XSERVER_NO_VALID_OPTION );

    DBGOUT;
    return ( 1 );
    }

  if ( ulStrtOpt == OPT_SHOW_USAGE )
    {
    printf ( XSERVER_USAGE_NT );

    DBGOUT;
    return ( 0 );
    }


  // --- CR 1104459
  if ( ulStrtOpt == OPT_SHOW_VERSION )
  {
    sp100_GetVersionString(COMP_NAME_RTE_NT_SP100, s100buildnumber, gxr.RTEVersionString);
    printf("%s\n",gxr.RTEVersionString);
    return ( 0 );
  }

  if ( newDebugLevel >= 0 )
  { /* PTS 1111208 */
	  tsp00_ErrTextc errTextC;
      (void)sql43_tcpip_init ();
	  if ( !pszNodeName ) pszNodeName = "localhost";
	  rc = os23NewDebugLevel(pszNodeName, newDebugLevel, errTextC);
	  if ( NO_ERROR == rc )
	  {
		  printf("New debug level %d\n", newDebugLevel );
	  }
	  else
	  {
		  printf("Failed to set new debug level:%s\n", &errTextC[0]);
	  }
	  DBGOUT;
	  return ( rc );
  }

  os968_InitXserverSpecification( fQuietMode,
                                  fPortableInstallation,
                                  fNTService,
                                  fNoDNSReverseLookup,
                                  usDebugLevel,
                                  usServicePort,
                                  usNIServicePort,
                                  usNISSLServicePort );

   //
   // --- NT Service
   //
   if ( gxr.fNTService )
     {
     if (( ulStrtOpt == OPT_INSTALL_SERVICE ) ||
         ( ulStrtOpt == OPT_UPDATE_SERVICE ))
       {
       if ( ulStrtOpt == OPT_INSTALL_SERVICE )
         rc = os969InstallAnyXserverService ( pszNodeName, gxr.ServerSpecification.ServerName,TRUE, gxr.fNoDNSReverseLookup,
                                              gxr.TCPIPCoord.usServicePort, gxr.NICoord.usServicePort, gxr.NISSLCoord.usServicePort );
       else
         rc = os969UpdateAnyServerService ( pszNodeName, gxr.ServerSpecification.ServerName, TRUE, gxr.fNoDNSReverseLookup,
                                            gxr.TCPIPCoord.usServicePort, gxr.NICoord.usServicePort, gxr.NISSLCoord.usServicePort );
       }
     else if ( ulStrtOpt == OPT_START_SERVICE )
       {
       rc = os969StartAnyServerService ( pszNodeName, gxr.ServerSpecification.ServerName );

       if ( rc == NO_ERROR )
         MSGCD (( INFO_XSER_STARTED ));
       }
     else if ( ulStrtOpt == OPT_STOP_SERVICE )
       rc = os969StopAnyServerService ( pszNodeName, gxr.ServerSpecification.ServerName );
     else if ( ulStrtOpt == OPT_REMOVE_SERVICE )
       rc = os969RemoveAnyXserverService ( pszNodeName, gxr.ServerSpecification.ServerName );
     else
       {
       sql60_open_event_log ( gxr.ServerSpecification.ServerName, EVENT_SOURCE_NAME );

       dispatchTable[0].lpServiceName = gxr.ServerSpecification.ServerName ;

       if (!StartServiceCtrlDispatcher(dispatchTable))
         {
         rc = GetLastError();

         MSGALL (( ERR_START_SERVICE, gxr.ServerSpecification.ServerName, rc ));

         if ( rc == ERROR_FAILED_SERVICE_CONTROLLER_CONNECT )
           {
           gxr.fNTService = FALSE;

           rc = os968Server ();
           }
         }
       }

     DBGOUT;
     return ( rc );
     }

  // --- Console application (SERVER)
  //
  if ( ulStrtOpt == OPT_START_SERVER )
    {
    szCmdLine[0] = '\0';

    for ( i = 1; i < argc; i++ )
      {
      strcat(szCmdLine, " ");
      strcat(szCmdLine, argv[i]);
      }

    rc = os969StartAnyServer ( szCmdLine, gxr.ServerSpecification.ServerName, gxr.ServerSpecification.ServerSemaName, fQuietMode );

    if ( rc == NO_ERROR )
      MSGCD (( INFO_XSER_STARTED ));
    }
  else if ( ulStrtOpt == OPT_STOP_SERVER )
    rc = os969StopAnyServer ( gxr.ServerSpecification.ServerName, gxr.ServerSpecification.ServerSemaName );
  else
    rc = os968Server ();

  DBGOUT;
  return ( rc );
  }

 /*------------------------------*/

 static HICON os968_get_icon_from_resource( UINT id )
   {
   #undef  MF__
   #define MF__ MOD__"os968_get_icon_from_resource"
   HICON                 hIcon = 0;
   HINSTANCE             hExe;           // handle to loaded .EXE file
   HRSRC                 hResource;      // handle for FindResource
   HRSRC                 hMem;           // handle for LoadResource
   BYTE                  *lpResource;    // address of resource data
   int                   nID;            // ID of resource that best fits current screen

   DBGIN;

   if ( (hExe = GetModuleHandle ( NULL ))                                   &&
       (hResource  = FindResource(hExe,MAKEINTRESOURCE(id),RT_GROUP_ICON)) &&
       (hMem       = LoadResource(hExe, hResource))                        &&
       (lpResource = (BYTE *)LockResource(hMem))                           &&
       (nID        = LookupIconIdFromDirectoryEx((PBYTE) lpResource, TRUE,
                                                 0, 0, LR_DEFAULTCOLOR))   &&
       (hResource  = FindResource(hExe, MAKEINTRESOURCE(nID),
                                  MAKEINTRESOURCE(RT_ICON)))               &&
       (hMem       = LoadResource(hExe, hResource))                        &&
       (lpResource = (BYTE *)LockResource(hMem))                                   &&
       (hIcon      = CreateIconFromResourceEx((PBYTE) lpResource,
                                               SizeofResource(hExe,hResource),
                                               TRUE, 0x00030000,
                                               0, 0, LR_DEFAULTCOLOR)))
    {
    DBGOUT
    return (hIcon);
    }


  DBGOUT
  return (0);
  }

 /*------------------------------*/

 static VOID os968_tray_message( HWND hWnd, UINT message)
   {
   #undef  MF__
   #define MF__ MOD__"os968_tray_message"
   NOTIFYICONDATA        tnd;
   static HICON          hIcon = 0;

   DBGIN;

   if ( hIcon == 0)
     hIcon = os968_get_icon_from_resource( ICON_SERV );

   wsprintf(tnd.szTip, XSERVER_ACTIVE, gxr.TCPIPCoord.usServicePort );

   switch (message)
     {
     case NIM_ADD:
       tnd.uFlags = NIF_MESSAGE|NIF_ICON|NIF_TIP;
       break;

     case NIM_MODIFY:
       tnd.uFlags = NIF_TIP|NIF_ICON;
       break;

     case NIM_DELETE:
       tnd.uFlags = 0;
       break;
     }

   if ( hIcon )
     {
     tnd.uID              = (UINT)ICON_SERV;
     tnd.cbSize           = sizeof(NOTIFYICONDATA);
     tnd.hWnd             = hWnd;
     tnd.uCallbackMessage = SM_TRAY_CALLBACK;
     tnd.hIcon            = hIcon;
     Shell_NotifyIcon(message, &tnd);
     }

   DBGOUT
   return;
   }

 /*------------------------------*/

VOID os968UpdateDebugLevelFromRegistry( VOID )
{
    PATHNAME                szKey;
    REG_ENTRY_REC           RegistryEntries[1];
    DWORD                   dwDebugLevel;

    strcpy ( szKey, gxr.ServerSpecification.ServerName );
    strcat ( szKey, "\\"REG_SK_SERVICE_PARAM );
    RegistryEntries[0].pszValueName        = REG_VN_DEBUG_LEVEL;
    RegistryEntries[0].pValue              = &dwDebugLevel;
    RegistryEntries[0].ulValueSize         = sizeof(DWORD);
    RegistryEntries[0].ulValueType         = REG_DWORD;

    if (sql50_reg_get_service_values ( NULL, szKey, 1, RegistryEntries ) == NO_ERROR)
    {
  	    gxr.usDebugLevel = (SHORT)dwDebugLevel;
    }
}

 /*------------------------------*/

 static VOID WINAPI os968Service ( DWORD argc, LPTSTR *argv )
   {
   #undef  MF__
   #define MF__ MOD__"os968Service"
   LONG                    rc = NO_ERROR;
   PSZ                     pszUserName;
   BOOL                    fOk;
   SECURITY_ATTRIBUTES     SA;
   ACCESS_RIGHTS_REC       Access;
   REG_ENTRY_REC           RegistryEntries[1];
   DWORD                   dwValUseIndepPath = 0;
   PATHNAME                szKey;
   DWORD                   dwDiagFileSize       = DIAGSIZE;
   DWORD                   dwNoDNSReverseLookup = 0;
   DWORD                   dwPortNo             = 0;
   PSZ                     pSep; 

   DBGIN;

   __try
   {
    sql60_disable_default_device ();

    // --- initialize service control
    gxr.dwState  = XSERVER_INITIALIZE;
    sql96c_NewSrvState( gxr.dwState, NO_ERROR );

    strcpy( gxr.ServerSpecification.ServerName, argv[0] );
    pSep = strrchr( gxr.ServerSpecification.ServerName, SERVICE_PORT_SEP );

    if ( 0 != pSep )
        strcat( gxr.ServerSpecification.ServerSemaName, pSep + 1 );

    gxr.sshStatusHandle = RegisterServiceCtrlHandler( gxr.ServerSpecification.ServerName, os968ServiceCtrlHandler );

    if ( gxr.sshStatusHandle == 0 )
      {
      rc = GetLastError();
      MSGALL (( ERR_SET_SERVICE_STATUS, rc ));
      }
    else
      {
      SetServiceBits ( gxr.sshStatusHandle,
                       SV_TYPE_SAPDB, TRUE, TRUE );

      gxr.dwState  = XSERVER_STARTING;
      sql96c_NewSrvState( gxr.dwState, NO_ERROR );

      gxr.fDebuggerIsDrWatson = os968DebuggerIsDrWatson();

//    PTS 1107043
      strcpy ( szKey, gxr.ServerSpecification.ServerName );
      strcat ( szKey, "\\"REG_SK_SERVICE_PARAM );
      RegistryEntries[0].pszValueName        = REG_VN_USE_INDEP_PATH;
      RegistryEntries[0].pValue              = &dwValUseIndepPath;
      RegistryEntries[0].ulValueSize         = sizeof(DWORD);
      RegistryEntries[0].ulValueType         = REG_DWORD;

      if (sql50_reg_get_service_values ( NULL, szKey, 1, RegistryEntries ) == NO_ERROR )
      {
        if ( !dwValUseIndepPath ) 
        {
            tsp00_Pathc szDBRoot;

            eo01_PutEnvVarDoNotUseIndepPath();
            RegistryEntries[0].pszValueName        = REG_VN_DBROOT;
            RegistryEntries[0].pValue              = szDBRoot;
            RegistryEntries[0].ulValueSize         = sizeof(szDBRoot);
            RegistryEntries[0].ulValueType         = REG_SZ;
            if (NO_ERROR != sql50_reg_get_service_values ( NULL, szKey, 1, RegistryEntries ))
            {
                MSGALL (( WRN_XSER_CANNOT_READ_REG, REG_VN_DBROOT));
            }
            else
            {
                eo01_PutEnvVarDBRoot (szDBRoot);           
            }
        }
      }

      os968_SetWrkDir();

      strcpy ( szKey, gxr.ServerSpecification.ServerName );
      strcat ( szKey, "\\"REG_SK_SERVICE_PARAM );
      RegistryEntries[0].pszValueName        = REG_VN_NO_DNS_REVERSE_LOOKUP;
      RegistryEntries[0].pValue              = &dwNoDNSReverseLookup;
      RegistryEntries[0].ulValueSize         = sizeof(DWORD);
      RegistryEntries[0].ulValueType         = REG_DWORD;

      if (sql50_reg_get_service_values ( NULL, szKey, 1, RegistryEntries ) == NO_ERROR)
        {
        gxr.fNoDNSReverseLookup = dwNoDNSReverseLookup == 1;
        }

      os968UpdateDebugLevelFromRegistry();

      strcpy ( szKey, gxr.ServerSpecification.ServerName );
      strcat ( szKey, "\\"REG_SK_SERVICE_PARAM );
      RegistryEntries[0].pszValueName        = REG_VN_DIAGFILE_SIZE;
      RegistryEntries[0].pValue              = &dwDiagFileSize;
      RegistryEntries[0].ulValueSize         = sizeof(DWORD);
      RegistryEntries[0].ulValueType         = REG_DWORD;
      
      if (sql50_reg_get_service_values ( NULL, szKey, 1, RegistryEntries ) != NO_ERROR )
        dwDiagFileSize = DIAGSIZE * (gxr.usDebugLevel + 1);

      strcpy ( szKey, gxr.ServerSpecification.ServerName );
      strcat ( szKey, "\\"REG_SK_SERVICE_PARAM );
      RegistryEntries[0].pszValueName        = REG_VN_PORTNO;
      RegistryEntries[0].pValue              = &dwPortNo;
      RegistryEntries[0].ulValueSize         = sizeof(DWORD);
      RegistryEntries[0].ulValueType         = REG_DWORD;
      
      if ( (sql50_reg_get_service_values ( NULL, szKey, 1, RegistryEntries ) == NO_ERROR ) && ( 0xFFFF >= dwPortNo ))
          gxr.TCPIPCoord.usServicePort = (USHORT)dwPortNo;

      strcpy ( szKey, gxr.ServerSpecification.ServerName );
      strcat ( szKey, "\\"REG_SK_SERVICE_PARAM );
      RegistryEntries[0].pszValueName        = REG_VN_NI_PORTNO;
      RegistryEntries[0].pValue              = &dwPortNo;
      RegistryEntries[0].ulValueSize         = sizeof(DWORD);
      RegistryEntries[0].ulValueType         = REG_DWORD;
      
      if ( (sql50_reg_get_service_values ( NULL, szKey, 1, RegistryEntries ) == NO_ERROR ) && ( 0xFFFF >= dwPortNo ))
          gxr.NICoord.usServicePort = (USHORT)dwPortNo;

      strcpy ( szKey, gxr.ServerSpecification.ServerName );
      strcat ( szKey, "\\"REG_SK_SERVICE_PARAM );
      RegistryEntries[0].pszValueName        = REG_VN_SSL_PORTNO;
      RegistryEntries[0].pValue              = &dwPortNo;
      RegistryEntries[0].ulValueSize         = sizeof(DWORD);
      RegistryEntries[0].ulValueType         = REG_DWORD;
      
      if ( (sql50_reg_get_service_values ( NULL, szKey, 1, RegistryEntries ) == NO_ERROR ) && ( 0xFFFF >= dwPortNo ))
          gxr.NISSLCoord.usServicePort = (USHORT)dwPortNo;

      if ( !os968_SetSymbolPath() ) /* PTS 1104558 */
      {
        MSGALL (( WRN_XSER_CANT_SET_SYMBOL_PATH ));
      }

      rc = sql49c_get_user_info ( &pszUserName, NULL );

      if ( rc != NO_ERROR )
        pszUserName = "";

      sql49c_user_privilege_check ( &fOk );
 
      if ( !fOk )
        {
        MSGALL (( ERR_XSER_USER_MISSING_PRIV, pszUserName ));
        rc = ERROR_ACCESS_DENIED;
        }

      if ( rc == NO_ERROR )
        {
        SA.nLength              = sizeof(SECURITY_ATTRIBUTES);
        SA.bInheritHandle       = FALSE;
        SA.lpSecurityDescriptor = NULL;

        memset ( &Access, 0, sizeof (Access) );  // - this must be done first!
        Access.fDescOwnerIsAdmin       = FALSE;
        Access.fAddDefaultDACL         = FALSE;
        Access.User.ulAccess           = 0;
        Access.Creator.ulAccess        = SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;
        Access.Admin.ulAccess          = SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;
        Access.System.ulAccess         = SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;
        Access.SAPDBOp.ulAccess        = SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;
        Access.DomSAPDBOp.ulAccess     = SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;
        Access.World.ulAccess          = 0;
        Access.BackupOp.ulAccess       = GENERIC_READ;

        rc = sql49c_alloc_and_init_SD( &Access, &SA.lpSecurityDescriptor );
        }

      if ( rc == NO_ERROR )
        rc = os968_OpenDiagfile ( &SA, max( dwDiagFileSize, DIAGSIZE ) );


      if ( rc == NO_ERROR )
        rc = FREE_MEM(SA.lpSecurityDescriptor);

      if ( rc == NO_ERROR )
      {
          os968UpdateVersion();

          rc = os968Coordinator ();
      }
        
      if ( gxr.dwState != XSERVER_STOPPED )
        {
        gxr.dwState  = XSERVER_STOPPED;
        sql96c_NewSrvState( gxr.dwState, rc );
        }
      }
  } 
  __except( os968ExceptionHandler(GetExceptionInformation()) ){;}

   DBGOUT;
   exit ( rc );
   return;
   }


/*------------------------------*/

static teo00_Bool os968_PortableInstallationAcitve()
 {
  #undef  MF__
  #define MF__ MOD__"os968_PortableInstallationAcitve"

  tsp01_RteError          RteError;
  tsp00_Pathc             Path;

  return sqlGetPortableInstallationRootPath(Path, TERM_WITHOUT_DELIMITER_EO01, &RteError );
 }

/*------------------------------*/

static teo00_Bool os968_SetSymbolPath()
 {
  #undef  MF__
  #define MF__ MOD__"os968_SetSymbolPath"

  teo00_Bool              Ok = true;
  tsp01_RteError          RteError ;
  tsp00_Pathc             IndepProgramsPath;

  if ( !sqlGetIndependentProgramsPath(IndepProgramsPath, TERM_WITHOUT_DELIMITER_EO01, &RteError ) )
  {
    if ( GetModuleFileName(NULL, IndepProgramsPath, sizeof (IndepProgramsPath) ) )
    {
      // IndepProgramsPath should be ...\pgm\serv.exe
      // SymbolPath        should be ...\symbols
      // sql01c_set_symbol_path needs the path BEFORE symbol ( ...\) 

      char *Hptr = strrchr(IndepProgramsPath, '\\' ); 
      if ( Hptr )
      {
        *Hptr = '\0' ;
        Hptr = strrchr(IndepProgramsPath, '\\' ) ;
      }

      if ( Hptr )
        *Hptr = '\0' ;
      else
        Ok = false ;
    }
    else
    {
      Ok = false ;
    }
  }
 
  if ( Ok ) 
  {
    DBG1  (( MF__, "set SYMBOL environment path to %s", IndepProgramsPath ));
    // --- set SYMBOL environment variable
    sql01c_set_symbol_path ( IndepProgramsPath ) ;
  }
  return(Ok);
 }

 /*------------------------------*/

 static LONG os968_SetWrkDir()
 {
  #undef  MF__
  #define MF__ MOD__"os968_SetWrkDir"

  LONG                    rc = NO_ERROR;
  tsp00_Pathc             runDir;
  tsp01_RteError          RteError ;

  runDir[0] = '\0' ;

  if ( !sqlGetIndependentServiceProtPath ( runDir, TERM_WITH_DELIMITER_EO01, &RteError ))
  {
    MSGALL (( ERR_XP_RUNDIR_MISSING, "Working directory" ));
    return -1;
  }

 if (!SetCurrentDirectory(runDir))
    {
    MSGALL (( ERR_WRONG_XP_RUNDIR, "Working directory", runDir ));
    return -1;
    }

  return 0;

 }

/*------------------------------*/

 static LONG os968_OpenDiagfile( PSECURITY_ATTRIBUTES     pSA,
                                 DWORD                    dwDiagFileSize )
 {
  #undef  MF__
  #define MF__ MOD__"os968_OpenDiagfile"

  LONG                    rc = NO_ERROR;
  tsp00_Pathc             DiagnoseFile ;
  tsp01_RteError          RteError ;
  char                    szPort[6];
  char                    xserverDiagName[64];

  DiagnoseFile[0] = '\0' ;

    if (( 0 != gxr.TCPIPCoord.usServicePort ) && ( gxr.TCPIPCoord.usServicePort != DEFAULT_PORT_SERVICE_SQL6_EO003 ))
        itoa( gxr.TCPIPCoord.usServicePort, szPort, 10);
    else
        szPort[0] = '\0';

    if ( !os968_BuildHostSpecificName(xserverDiagName, sizeof(xserverDiagName), szPort, "xserver", ".prt") )
    {
        strncpy( xserverDiagName, XSERV_DIAGNAME, sizeof(xserverDiagName) - 1 );
        xserverDiagName[sizeof(xserverDiagName) - 1] = '\0';
    }
    else
    {
        xserverDiagName[sizeof(xserverDiagName) - 1] = '\0';
    }

  if ( !sqlGetIndependentServiceProtPath ( DiagnoseFile, TERM_WITH_DELIMITER_EO01, &RteError )) /* PTS 1108067 */
  {
    MSGALL (( WRN_XSER_NO_PATH_FOR_PROTFILE_FOUND, xserverDiagName));
  }

  strcat ( DiagnoseFile, xserverDiagName );

  rc = sql60_open_diag_file ( DiagnoseFile, dwDiagFileSize, pSA);

  return ( rc ) ;

 }

 /*------------------------------*/

 static VOID WINAPI os968ServiceCtrlHandler ( DWORD dwCtrlCode )
   {
   #undef  MF__
   #define MF__ MOD__"os968ServiceCtrlHandler"
   LONG                    rc      = NO_ERROR;

   DBGIN;

   // Handle the requested control code.
   //
   switch(dwCtrlCode)
     {
     case SERVICE_CONTROL_STOP:         // --- Stop the service.
     case SERVICE_CONTROL_SHUTDOWN:
         gxr.ssStatus.dwCurrentState     = SERVICE_STOP_PENDING;
         gxr.ssStatus.dwWin32ExitCode    = NO_ERROR;
         gxr.ssStatus.dwWaitHint         = 30000;
         gxr.ssStatus.dwControlsAccepted = 0;
         gxr.ssStatus.dwCheckPoint       = 0;

         // Report the status of the service to the service control manager.
         if (!SetServiceStatus ( gxr.sshStatusHandle, &gxr.ssStatus ))
           {
           rc = GetLastError();
           MSGALL (( ERR_SET_SERVICE_STATUS, rc ));
           }

         os969StopAnyServer ( gxr.ServerSpecification.ServerName, gxr.ServerSpecification.ServerSemaName );

         return;

     case SERVICE_CONTROL_PAUSE:
         gxr.fServicePaused              = TRUE;
         gxr.ssStatus.dwCurrentState     = SERVICE_PAUSED;
         gxr.ssStatus.dwWin32ExitCode    = NO_ERROR;
         gxr.ssStatus.dwCheckPoint       = 0;
         gxr.ssStatus.dwWaitHint         = 0;
         break;

     case SERVICE_CONTROL_CONTINUE:
         gxr.fServicePaused              = FALSE;
         gxr.ssStatus.dwCurrentState     = SERVICE_RUNNING;
         gxr.ssStatus.dwWin32ExitCode    = NO_ERROR;
         gxr.ssStatus.dwCheckPoint       = 0;
         gxr.ssStatus.dwWaitHint         = 0;
         break;

     case SERVICE_CONTROL_INTERROGATE:  // --- Update the service status.
     default:
         break;
     }

   // Report the status of the service to the service control manager.
   if (!SetServiceStatus ( gxr.sshStatusHandle, &gxr.ssStatus ))
     {
     rc = GetLastError();
     MSGALL (( ERR_SET_SERVICE_STATUS, rc ));
     }

   DBGOUT;
   return;
   }

/*------------------------------*/

static INT os968Server ( VOID )
  {
  #undef  MF__
  #define MF__ MOD__"os968Server"
  LONG                    rc          = NO_ERROR;
  HEV                     hevSemXserv = 0;
  CHAR                    szOldTitle[100];
  CHAR                    szTitle[sizeof(szOldTitle)];
  CHAR                    szNewTitle[sizeof(szOldTitle)];
  ULONG                   ulTimeOut   = 1000;
  HWND                    hwndConsole = NULL;

  DBGIN;
  wsprintf( szNewTitle,"%s (%xh)", gxr.ServerSpecification.ServerName,
            GetCurrentProcessId() );

  GetConsoleTitle ( szOldTitle, sizeof(szOldTitle));
  SetConsoleTitle( szNewTitle );

  do
    {
    GetConsoleTitle ( szTitle, sizeof(szTitle));
    SLEEP(10);
    }
  while ( strcmp (szTitle, szNewTitle) );

  hwndConsole = FindWindow(NULL, szNewTitle);

  if ( hwndConsole )
    {
    if (( sql02_get_platform_id() != VER_PLATFORM_WIN32_NT ) &&
        ( !gxr.usDebugLevel ))
      {
      ShowWindow(hwndConsole, SW_HIDE);
      SendMessage(hwndConsole, WM_SYSCOMMAND, SC_MINIMIZE | HTCLIENT, 0L);
      }
    os968_tray_message(hwndConsole, NIM_ADD);
    }

  SetConsoleTitle( szOldTitle );

  rc = sql41c_open_event_sem ( &hevSemXserv, gxr.ServerSpecification.ServerSemaName, "", NO_ERROR);

  if (rc == NO_ERROR)
    {
    sql41c_close_event_sem ( hevSemXserv, gxr.ServerSpecification.ServerName );

    #if defined(_WIN32)
      if ( hwndConsole )
        os968_tray_message(hwndConsole, NIM_DELETE);
    #endif

    MSGCD (( ERR_XSER_ALREADY_RUNNING ));

    DBGOUT;
    return( 0 );
    }

  rc = os968_OpenDiagfile ( NULL, DIAGSIZE * (gxr.usDebugLevel + 1) );

  if ( rc != NO_ERROR )
    {
    if ( hwndConsole )
      os968_tray_message(hwndConsole, NIM_DELETE);

    DBGOUT;
    return ( rc );
    }

   // --- now running as server process, change the caption bar...
   SetConsoleTitle (gxr.ServerSpecification.ServerName);

   if (!SetConsoleCtrlHandler( os968ConsoleCtrlHandler, TRUE ))
     {
     rc = GetLastError ( );
     if ( hwndConsole )
       os968_tray_message(hwndConsole, NIM_DELETE);

     DBGOUT;
     return ( rc );
     }

    rc = os968Coordinator ();

   if ( hwndConsole )
     os968_tray_message(hwndConsole, NIM_DELETE);

  DBGOUT;
  return( rc );
  }

/*------------------------------*/

STATIC INT os968Coordinator ( VOID )
  {
  #undef  MF__
  #define MF__ MOD__"os968Coordinator"
  LONG                    rc        = NO_ERROR;
  ULONG                   ulResult;
  #if defined(_WIN32)
   ACCESS_RIGHTS_REC      Access;
   PSZ                    pszUserName;
  #endif

  DBGIN;

  GETPROCESSID(&gxr.pid);

  gxr.hevSemXserv = INVALID_HANDLE_VALUE;

   rc = sql49c_get_user_info ( &pszUserName, NULL );

   if ( rc != NO_ERROR )
     pszUserName = "";

   rc = sql49c_set_sync_access ();

   if ( rc != NO_ERROR )
     {
     DBGOUT;
     return( rc );
     }

   gxr.WorldSA.nLength         = sizeof(gxr.WorldSA);
   gxr.WorldSA.bInheritHandle  = FALSE;

   memset ( &Access, 0, sizeof (Access) );   // - this must be done first!
   Access.World.ulAccess       = SPECIFIC_RIGHTS_ALL |
                                 STANDARD_RIGHTS_ALL;

   rc = sql49c_alloc_and_init_SD( &Access, &gxr.WorldSA.lpSecurityDescriptor );

   if ( rc != NO_ERROR )
     {
     DBGOUT;
     return( rc );
     }

   gxr.LockSA.nLength         = sizeof(gxr.LockSA);
   gxr.LockSA.bInheritHandle  = FALSE;

   memset ( &Access, 0, sizeof (Access) );  // - this must be done first!
   rc = sql49c_alloc_and_init_SD( &Access, &gxr.LockSA.lpSecurityDescriptor );

   if ( rc != NO_ERROR )
     {
     DBGOUT;
     return( rc );
     }

   gxr.AdminSA.nLength         = sizeof(gxr.AdminSA);
   gxr.AdminSA.bInheritHandle  = FALSE;

   memset ( &Access, 0, sizeof (Access) );  // - this must be done first!
   Access.Creator.ulAccess        = SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;
   Access.Admin.ulAccess          = SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;
   Access.System.ulAccess         = SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;
   Access.SAPDBOp.ulAccess        = SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;
   Access.DomSAPDBOp.ulAccess     = SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;

   rc = sql49c_alloc_and_init_SD( &Access, &gxr.AdminSA.lpSecurityDescriptor );

   if ( rc != NO_ERROR )
     {
     DBGOUT;
     return( rc );
     }

  rc = sql41c_create_event_sem ( &gxr.hevSemXserv, gxr.ServerSpecification.ServerSemaName, "",
                                 0, FALSE, &gxr.AdminSA );

  if (rc == ERROR_DUPLICATE_NAME)
    {
    if ( gxr.fQuietMode == FALSE )
      MSGALL(( ERR_XSER_ALREADY_RUNNING ));

    DBGOUT;
    return( 0 );
    }
  else if (rc != NO_ERROR)
    {
    DBGOUT;
    return( rc );
    }

  __try
  {
    hModule = GetModuleHandle(NULL);   // - used in 'vos40lc'
	ulResult = sql43_tcpip_init ();

    if ( ulResult != TCPIP_INIT_OK )
      {
      if ( ulResult == TCPIP_INIT_WRONG_VERSION )
        MSGALL (( ERR_WRONG_TCPIP_VERSION ))
      else
        MSGALL (( ERR_XSER_CANNOT_INIT_TCPIP ))

      DBGOUT;
      return ( 1 );
      }

    // --- Write a message to the event log (Version included)
    //     and start the diagnostic file write cycle
    MSGALL (( INFO_XSER_STARTED_VERSION, gxr.RTEVersionString ));

    os968_ShowEnviron(_environ);

    // --- get port number for remote sql service
    if ( 0 == gxr.TCPIPCoord.usServicePort )
    {
        rc = sql43_get_service_by_name ( SOCKET_SERVICE_SQL6_EO003, &gxr.TCPIPCoord.usServicePort );

        if ( rc != NO_ERROR )
        {
            gxr.TCPIPCoord.usServicePort = DEFAULT_PORT_SERVICE_SQL6_EO003;
        }
        else
        {
            MSG6 (( ERR_SERV_BY_NAME_ERROR, SOCKET_SERVICE_SQL6_EO003 ));
        }
    }

    MSGD (( INFO_XSER_SERV_PORT, gxr.TCPIPCoord.usServicePort ))

    rc = os96StartTCPIPCoord ( &gxr.TCPIPCoord ) ;

    if ( false == gxr.fPortableInstallation ) // If it's a portable installation we don't need NI and SSL 
    {
        if ( rc == NO_ERROR )
        {
            rc = os962InitializeNISupport();

            if ( rc == NO_ERROR )
            {
                if ( 0 == gxr.NICoord.usServicePort )
                {
                    rc = sql43_get_service_by_name ( SOCKET_SERVICE_NI72_EO003, &gxr.NICoord.usServicePort );

                    if ( rc != NO_ERROR )
                    {
                        gxr.NICoord.usServicePort    = DEFAULT_PORT_SERVICE_NI72_EO003;
                    }
                    else
                    {
                        MSG6 (( ERR_SERV_BY_NAME_ERROR, SOCKET_SERVICE_NI72_EO003 ));
                    }
                }

                MSGD (( INFO_NI_SERVICE_PORT, gxr.NICoord.usServicePort ))

                rc = os962StartNICoord ( &gxr.NICoord ) ; // failure to start NI coordinator only reported

                if ( NO_ERROR != rc )
                {
                    MSGD (( ERR_XSER_CREATING_THREAD, "NICOORDINATOR", rc));
                }
                else
                {
                    MSGD (( INFO_NI_SERV_STARTED ));
                }

                // current NISSL needs successfull os962InitializeNISupport() since it shares NITRACEFILE !
                rc = os964InitializeNISSLSupport();

                if ( rc == NO_ERROR )
                {
                    if ( 0 == &gxr.NISSLCoord.usServicePort )
                    {
                        rc = sql43_get_service_by_name ( SOCKET_SERVICE_NISSL76_EO003, &gxr.NISSLCoord.usServicePort );

                        if ( rc != NO_ERROR )
                        {
                            gxr.NISSLCoord.usServicePort = DEFAULT_PORT_SERVICE_NISSL76_EO003;
                        }
                        else
                        {
                            MSG6 (( ERR_SERV_BY_NAME_ERROR, SOCKET_SERVICE_NISSL76_EO003 ));
                        }
                    }
                    MSGD (( INFO_NISSL_SERVICE_PORT, gxr.NISSLCoord.usServicePort ))

                    rc = os964StartNISSLCoord ( &gxr.NISSLCoord ) ;

                    if ( NO_ERROR != rc ) // failure to start NISSL coordinator only reported
                    {
                        MSGD (( ERR_XSER_CREATING_THREAD, "NISSLCOORDINATOR", rc));
                    }
                    else
                    {
                        MSGD (( INFO_NISSL_SERV_STARTED ));
                    }
                }
            }

            rc = NO_ERROR; // accept if NI support is not working...

        }
    }

    if (( gxr.fNTService ) && ( rc == NO_ERROR ))
       {
       gxr.dwState = XSERVER_RUNNING;
       sql96c_NewSrvState( gxr.dwState, NO_ERROR );
       }

     sql60_strt_msg_wrt_cycle ( );

    if ( rc == NO_ERROR )
      rc = sql41c_wait_event_sem ( gxr.hevSemXserv, (ULONG)UNDEF, gxr.ServerSpecification.ServerName );
    }
  __finally
     {
     os968Finish(0);
     }

    if ( gxr.fNTService )
      {
      gxr.dwState = XSERVER_STOPPED;
      sql96c_NewSrvState( gxr.dwState, NO_ERROR );
      }

  DBGOUT;
  return ( rc );
  }

/*------------------------------*/


STATIC BOOL WINAPI os968ConsoleCtrlHandler ( DWORD dwCtrlType )
  {
  #undef  MF__
  #define MF__ MOD__"os968ConsoleCtrlHandler"

  os969StopAnyServer ( gxr.ServerSpecification.ServerName, gxr.ServerSpecification.ServerSemaName );
  SLEEP ( 2000 );

  DBGPAS;
  exit(0);
  return (FALSE);
  }

/*------------------------------*/

STATIC VOID _System os968Finish (ULONG  termcode)
  {
  #undef  MF__
  #define MF__ MOD__"os968Finish"

  tsp00_ErrTextc                  pErrText;

  DBGPAS;

  if ( gxr.hevSemXserv != INVALID_HANDLE_VALUE )
  {
    sql41c_close_event_sem ( gxr.hevSemXserv, gxr.ServerSpecification.ServerName );
    gxr.hevSemXserv = INVALID_HANDLE_VALUE;
  }

  if ( gxr.NICoord.ThrdCtrlHeader.Tid != UNDEF )
  {
    os962NIFinish ( );
  }

  if ( gxr.NISSLCoord.ThrdCtrlHeader.Tid != UNDEF )
  {
    os964NISSLFinish ( );
  }

  if (gxr.NISSLCoord.ThrdCtrlHeader.Tid != UNDEF || gxr.NICoord.ThrdCtrlHeader.Tid != UNDEF)
  {
    eo40NiFinish (pErrText);
  }

  os960Finish (&gxr.TCPIPCoord);

  MSGALL (( INFO_XSER_STOPPED_VERSION, gxr.RTEVersionString ));
  sql60_close_diag_file ();

  return;
  }


/*------------------------------*/

static BOOL os968DebuggerIsDrWatson( VOID )
  {
  #undef  MF__
  #define MF__ MOD__"os968DebuggerIsDrWatson"
  LONG             rc = NO_ERROR;
  PATHNAME         szDebuggerEnty;
  REG_ENTRY_REC    RegistryEntries[1];

  DBGIN;

  if ( sql02_get_platform_id () == VER_PLATFORM_WIN32_NT )
    {
    RegistryEntries[0].pszValueName = REG_VN_AEDEBUG;
    RegistryEntries[0].pValue       = szDebuggerEnty;
    RegistryEntries[0].ulValueSize  = sizeof(szDebuggerEnty);
    RegistryEntries[0].ulValueType  = REG_SZ;

    rc = sql50_reg_get_values ( NULL, HKEY_LOCAL_MACHINE, REG_SK_AEDEBUG, 1, RegistryEntries );

    if (rc != NO_ERROR)
      {
      DBGOUT;
      return( FALSE );
      }

    strupr ( szDebuggerEnty );

    if ( strstr ( szDebuggerEnty, DR_WATSON_EXENAME ) != NULL )
      {
      DBGOUT;
      return( TRUE );
      }
    }

  DBGOUT;
  return( FALSE );
  }

/*------------------------------*/

static void os968UpdateVersion( VOID )
  {
  #undef  MF__
  #define MF__ MOD__"os968UpdateVersion"
  REG_ENTRY_REC           RegistryEntries[2];
  tsp100_VersionID0       RTEVersionID;
  tsp00_Versionc          RTEVersionString;
  PATHNAME                szKey;

  DBGIN;

  if ( sql02_get_platform_id () == VER_PLATFORM_WIN32_NT )
    {
    // ---- insert version string entry into registry database
    if ( sql02_get_platform_id() == VER_PLATFORM_WIN32_NT )
      sp100_GetVersionString(COMP_NAME_RTE_NT_SP100, s100buildnumber, RTEVersionString);
    else
      sp100_GetVersionString(COMP_NAME_RTE_WIN9X_SP100, s100buildnumber, RTEVersionString );
    
    sp100_GetVersionID ( VersionIDType0_esp100, s100buildnumberEx, &RTEVersionID );
    
    RegistryEntries[0].pszValueName = REG_VN_VERSION;
    RegistryEntries[0].pValue       = RTEVersionString;
    RegistryEntries[0].ulValueSize  = (ULONG)strlen(RTEVersionString);
    RegistryEntries[0].ulValueType  = REG_SZ;
    
    RegistryEntries[1].pszValueName = REG_VN_VERSION_ID;
    RegistryEntries[1].pValue       = &RTEVersionID;
    RegistryEntries[1].ulValueSize  = sizeof(RTEVersionID);
    RegistryEntries[1].ulValueType  = REG_BINARY;


    strcpy ( szKey, gxr.ServerSpecification.ServerName );
    strcat ( szKey, "\\"REG_SK_SERVICE_PARAM );
    
    sql50_reg_put_service_values ( NULL, szKey, 2, RegistryEntries );
    }

  DBGOUT;
  return;
  }

/*------------------------------*/

static bool os968_BuildHostSpecificName      ( char *        buffer, 
                                               DWORD         bufferSize, 
                                               char const*   portNo, 
                                               char const*   prefix, 
                                               char const*   suffix )
{
    char myHostname[64];
    DWORD lNameLen = sizeof(myHostname);

    if ( !GetComputerName(myHostname, &lNameLen) )
    {
        return false;
    }

    if ( ((DWORD)(strlen(prefix) + strlen("_") +  strlen(myHostname) + 
          strlen("_") + strlen(portNo) + strlen(suffix) )) >= bufferSize )
    {
        return false;
    }

    strcpy(buffer, prefix);
    strcat(buffer, "_");
    strcat(buffer, &myHostname[0]);

    if ( '\0' != portNo[0] )
    {
        strcat(buffer, "_");
        strcat(buffer, portNo);
    }

    strcat(buffer, suffix);
    return true;
}

/*------------------------------*/

static VOID os968_InitXserverSpecification ( BOOL    fQuietMode,
                                             BOOL    fPortableInstallation,
                                             BOOL    fNTService,
                                             BOOL    fNoDNSReverseLookup,
                                             USHORT  usDebugLevel,
                                             USHORT  usServicePort,
                                             USHORT  usNIServicePort,
                                             USHORT  usNISSLServicePort )
{
    sp100_GetVersionString(COMP_NAME_RTE_NT_SP100, s100buildnumber, gxr.RTEVersionString);

    gxr.TCPIPCoord.sd             = INVALID_SOCKET;
    gxr.fQuietMode                = fQuietMode;
    gxr.fPortableInstallation     = fPortableInstallation;
    gxr.fNTService                = fNTService;
    gxr.usDebugLevel              = usDebugLevel;
    gxr.fNoDNSReverseLookup       = fNoDNSReverseLookup;
    gxr.TCPIPCoord.usServicePort  = usServicePort;
    gxr.NICoord.usServicePort     = usNIServicePort;
    gxr.NISSLCoord.usServicePort  = usNISSLServicePort;

    strncpy( gxr.ServerSpecification.ServerName, XSERV_TITLE, sizeof(gxr.ServerSpecification.ServerName) - 1 );
    gxr.ServerSpecification.ServerName[sizeof(gxr.ServerSpecification.ServerName) - 1] = '\0';

    strncpy( gxr.ServerSpecification.ServerSemaName, SEM_XSERVER, sizeof(gxr.ServerSpecification.ServerSemaName) - 1 );
    gxr.ServerSpecification.ServerSemaName[sizeof(gxr.ServerSpecification.ServerSemaName) - 1] = '\0';

    gxr.TCPIPCoord.ThrdCtrlHeader.StackSize  = TCPIP_COORD_STACK_SIZE ;
    gxr.TCPIPCoord.pServiceStr               = SOCKET_SERVICE_SQL6_EO003 ;
    gxr.TCPIPCoord.pTCPIPWorkerThreadFkt     = os960TCPIP_ServerThread ;

    if (( 0 != gxr.TCPIPCoord.usServicePort ) && ( gxr.TCPIPCoord.usServicePort != DEFAULT_PORT_SERVICE_SQL6_EO003 ))
    {
        char szPort[6];

        itoa( gxr.TCPIPCoord.usServicePort, szPort, 10);

        if ( strlen (gxr.ServerSpecification.ServerName) + 6 < sizeof(gxr.ServerSpecification.ServerName) - 1)
        {
            int  len = strlen(gxr.ServerSpecification.ServerName);

            gxr.ServerSpecification.ServerName[len]     = SERVICE_PORT_SEP;
            gxr.ServerSpecification.ServerName[len + 1] = '\0';
            strcat( gxr.ServerSpecification.ServerName, szPort );
        }
        if ( strlen (gxr.ServerSpecification.ServerSemaName) + 5 < sizeof(gxr.ServerSpecification.ServerSemaName) - 1)
        {
            strcat( gxr.ServerSpecification.ServerSemaName, szPort );
        }
    }

}

/*-------------------------------------------------*/

static void os968_ShowEnviron(char **envp)
{
    char **pEnv;
    MSGD(( INFO_ENVIRONMENT_DUMP_START ));
    for ( pEnv = envp; pEnv && *pEnv; ++pEnv )
    {
#define MAX_ENV_PART 80
	    char  EnvBuffer[MAX_ENV_PART+2];
	    char  *pEnvPart;
        int   len;

	    for ( len  = (int)strlen(*pEnv), pEnvPart = *pEnv;
              len  > MAX_ENV_PART;
              len -= MAX_ENV_PART,       pEnvPart += MAX_ENV_PART )
	    {
            memset(EnvBuffer, 0, MAX_ENV_PART+2);
	        if ( pEnvPart == *pEnv )
            {
   	            SAPDB_memcpy(EnvBuffer, pEnvPart, MAX_ENV_PART);
            }
            else
            {
                EnvBuffer[0] = ' '; /* add a space for each followup line */
       	        SAPDB_memcpy(&EnvBuffer[1], pEnvPart, MAX_ENV_PART);
            }
            MSGD(( INFO_ENVIRONMENT_VARIABLE, EnvBuffer ))
	    }

        if ( pEnvPart == *pEnv )
        {
            MSGD(( INFO_ENVIRONMENT_VARIABLE, pEnvPart ))
        }
        else
        {
            EnvBuffer[0] = ' '; /* add a space for each followup line */
       	    SAPDB_memcpy(&EnvBuffer[1], pEnvPart, len+1);
            MSGD(( INFO_ENVIRONMENT_VARIABLE, EnvBuffer ))
        }
    }
    MSGD(( INFO_ENVIRONMENT_DUMP_COMPLETED ));
}

/*-------------------------------------------------*/

static char* os968_ParsePortArg( char* arg, unsigned short* pPort, char* pPortTypeSpecifier )
{
    char  buffer[10];
    long  port;
    char* src      = arg;
    char* dest     = buffer;
    char* destEnd  = buffer + sizeof(buffer) - 2;

    *pPortTypeSpecifier = '\0';

    // Copy digit characters
    while ((0 != isdigit(*src)) && ( dest != destEnd ))
    {
        *dest++ = *src++;
    };
    *dest = '\0';

    // No digit character found?
    if ( dest == buffer )
        src = 0; // Parse error, unexpected character
    else
    {   // End of string reached?
        if ( '\0' == *src ) 
        {
            if ( 0xFFFF >= (port = atol( buffer )) ) // Convert port number string to long integer
                *pPort = (unsigned short)port;
            else
                src = 0; // Parse error, port out of range
        }
        else
        {
            if (( 'N' == *src ) || ( 'n' == *src ) || ( 'E' == *src ) || ( 'e' == *src ))
                *pPortTypeSpecifier = *src++; // Port type specifier found

            if ( 0xFFFF >= (port = atol( buffer )) ) // Convert port number string to long integer
            {
                *pPort = (unsigned short)port;

                if ( ',' == *src ) 
                    ++src;   // Step over separator
                else if ( '\0' != *src )
                    src = 0; // Parse error, unexpected character
            }
            else
                src = 0; // Parse error, port out of range

        }
    }
    return src;
}

/*-------------------------------------------------*/

static teo00_Bool os968_ParsePortArgs( char* arg, unsigned short* pPort, unsigned short* pPortNI, unsigned short* pPortSSL )
{
    char            portTypeSpecifier;
    unsigned short  port = 0;
    teo00_Bool      ok    = true;

    *pPortSSL = 0;
    *pPortNI  = 0;
    *pPort    = 0;

    do 
    {
        arg = os968_ParsePortArg( arg, &port, &portTypeSpecifier );

        if ( 0 == arg ) // Parse error?
            ok = false;
        else
        {
            // Assign port number
            switch ( portTypeSpecifier )
            {
            case 'E':
            case 'e':

                if ( *pPortSSL ) // Port set twice?
                    ok = false;
                else
                    *pPortSSL = port;
                break;
            case 'N':
            case 'n':
                if ( *pPortNI ) // Port set twice?
                    ok = false;
                else
                    *pPortNI = port;
                break;
            case '\0':
                if ( *pPort ) // Port set twice?
                    ok = false;
                else
                    *pPort = port;
                break;
            }
        }
    } while ( ok && ( '\0' != arg[0] ));

    return ok;
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
