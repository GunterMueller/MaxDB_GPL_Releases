/*!
  @file           vos01u.c
  @author         RaymondR
  @brief          Initialization (User Part)
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
// INCLUDE FILES
//
#include "gos00.h"
#include "geo00.h"
#include "heo46.h"
#include "geo007_1.h"
#include "geo007_2.h"
#include "geo572.h"
#include "heo01.h"
#include "gos60.h"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageOutput.h"
#include "RunTime/Security/Crypto/RTECrypto_EntropyPool.h"

//
//  DEFINES
//

#define MOD__  "VOS01UC : "
#define MF__   MOD__"UNDEFINED"


//
//  MACROS
//


//
//  LOCAL TYPE AND STRUCT DEFINITIONS
//


/*
 * EXTERNAL VARIABLES
 */


//
//  EXPORTED VARIABLES
//

PBOOLEAN    pbGlobalCancelAddress   = NULL;
BOOLEAN     bRuntimeComponent       = FALSE;
BOOL        fCannotStartThread      = FALSE;
HANDLE      hModule = INVALID_HANDLE_VALUE;



//
//  LOCAL VARIABLES
//

static BOOLEAN                   bInitialized    = FALSE;
static unsigned long             ulClientCount   = 0;
static UINT1                     fReturnValue    = 0;
static clock_t                   ulStartTicks    = 0;
static BOOLEAN                   bSetExitList    = TRUE;

#if defined(_WIN32)
 static volatile BOOL            fCtrl_C         = FALSE;
#else
 static SIGNALFUNCTYPE           FetchedSigInt   = SIG_DFL;
 static SIGNALFUNCTYPE           FetchedSigBrk   = SIG_DFL;
#endif


//
//  LOCAL FUNCTION PROTOTYPES
//
static VOID _System  sql01u_exit_handler     ( ULONG   ulTermCode );

#if defined(_WIN32)
 static BOOL          sql01u_start_test_thread ( VOID );
 static BOOL WINAPI   sql01u_catch_event       ( DWORD dwCtrlType );
 static LONG          sql01u_init_reg_values   ( VOID );
#else
 static VOID _CDECL   sql01u_catch_event       ( INT SigType );
#endif

//
// ========================== GLOBAL FUNCTIONS ================================
//
void    sqlSetModuleHandle (HANDLE       moduleHandle)
{
    hModule = moduleHandle;
}


VOID    sqlinit (   tsp00_CompName                  acComponent,
                    PBOOLEAN                        pbCancelAddress )
  {
  #undef  MF__
  #define MF__ MOD__"sqlinit"
  APIRET        rc;

  #if defined(_WIN32)
   static BOOL  fCtrlHandlerSet = FALSE;
  #endif


  DBGIN;

  eo572_CreateAllocPool ();

  ++ulClientCount;
  if (ulClientCount > 1)
    {
    // initialize RTE only once
    return;
    }

  if ( bInitialized )
    {
    DBG1  (( MF__, "sqlinit() called twice" ));
    MSGCD (( ERR_INIT_CALLED_TWICE ));

    DBGOUT;

    return;
    }

  SAPDBErr_SetMessageOutputCall( os60MessageOutputCall );

  sql01u_init_reg_values ();

  if ( *acComponent == 'R' )
    bRuntimeComponent = TRUE;  // --- it's a precompiled program

  if ( pbCancelAddress )
    {
    *pbCancelAddress      = FALSE;
    pbGlobalCancelAddress = pbCancelAddress;
    }



  #if defined(_WIN32)

   if ( fCtrlHandlerSet == FALSE ) 
     { 
     fCannotStartThread = !sql01u_start_test_thread();

     if ( fCannotStartThread == FALSE ) // - 16 Bit environment?
	   {
       if (SetConsoleCtrlHandler( sql01u_catch_event, TRUE ))
         fCtrlHandlerSet = TRUE;
	   }
     }    
  #else
   if ( FetchedSigInt != (SIGNALFUNCTYPE) sql01u_catch_event )
     FetchedSigInt = signal ( SIGINT, sql01u_catch_event );

   if ( FetchedSigBrk != (SIGNALFUNCTYPE) sql01u_catch_event )
     FetchedSigBrk = signal ( SIGBREAK, sql01u_catch_event );
  #endif


  if ( bSetExitList )
    {
    bSetExitList = FALSE;

    rc = sql02u_add_exit_list ( sql01u_exit_handler );

    if ( rc != NO_ERROR )
      {
      DBG1 (( MF__, ERRMSG_SET_EXIT_HANDLER_FAILED, rc ));
      printf ( ERRMSG_SET_EXIT_HANDLER_FAILED"\n", rc );

      ABORT();
      }
    }

  sql03u_init ();

  #if defined(WINTERM)
   win07p_init ( acComponent, pbCancelAddress );
  #endif

  initSystemEntropyPool( );
   
  bInitialized = TRUE;
  fReturnValue = 0;
  ulStartTicks = clock ();

  DBGOUT;
  return;
  }


/*------------------------------*/

VOID  sqlfinish (   BOOLEAN bTerminate  )
  {
  #undef  MF__
  #define MF__ MOD__"sqlfinish"


  DBGIN;

  if (ulClientCount <= 0)
    {
    return;
    }
  --ulClientCount;
  if (ulClientCount > 0)
    {
    // run cleanup code only for last client
    return;
    }

  sql03u_finish ();

  bInitialized = FALSE;

  #if defined(WINTERM)
   win07p_finish ( bTerminate );
  #else

  if ( bTerminate )
    {
    DBGOUT;

    EXITPROCESS( (ULONG)fReturnValue );
    }
  #endif

  DBGOUT;
  return;
  }


/*------------------------------*/


VOID sqlos   (   PUINT1  pfOS    )
  {
  #undef  MF__
  #define MF__ MOD__"sqlos"


  DBGPAS;

  #if defined(_WIN32)
    *pfOS = OS_WIN32;
  #else
    *pfOS = OS_OS2;
  #endif
  return;
  }


/*------------------------------*/


VOID sqluid (   PROCESS_ID* pUserID     )
  {
  #undef  MF__
  #define MF__ MOD__"sqluid"

  DBGIN;

  GETPROCESSID(pUserID);

  DBGOUT;
  return;
  }


/*------------------------------*/

VOID  sqlresult (   UINT1   fResult     )
  {
  #undef  MF__
  #define MF__ MOD__"sqlresult"


  DBGPAS;

  fReturnValue = fResult;

  return;
  }


/*------------------------------*/

VOID    sqlclock    (   PINT4   pSeconds,
                        PINT4   pMilliSeconds   )
  {
  #undef  MF__
  #define MF__ MOD__"sqlclock"


  clock_t ulTicks;


  DBGIN;

  ulTicks = clock () - ulStartTicks;
  *pSeconds      = ulTicks / CLOCKS_PER_SEC;

  *pMilliSeconds = (ulTicks % CLOCKS_PER_SEC) * (1000000 / CLOCKS_PER_SEC);

  DBGOUT;
  return;
  }



/*------------------------------*/


VOID    sqldattime  (   char*   pDate,
                        char*   pTime     )
  {
  #undef  MF__
  #define MF__ MOD__"sqldattime"
  char*          pString;
  #if defined(_WIN32)
   SYSTEMTIME    LocalTime;
  #else
   DATETIME      DateTime;
  #endif

  DBGIN;

  memset ( pDate, '0', 8 );
  memset ( pTime, '0', 8 );

  #if defined(_WIN32)
   GetLocalTime(&LocalTime);

   pString = pDate;
   *( pString++ ) += LocalTime.wYear / 1000;
   *( pString++ ) += LocalTime.wYear / 100 % 10;
   *( pString++ ) += LocalTime.wYear / 10  % 10;
   *( pString++ ) += LocalTime.wYear       % 10;
   *( pString++ ) += LocalTime.wMonth / 10;
   *( pString++ ) += LocalTime.wMonth % 10;
   *( pString++ ) += LocalTime.wDay   / 10;
   *( pString++ ) += LocalTime.wDay   % 10;

   pString = pTime + 2;
   *( pString++ ) += LocalTime.wHour   / 10;
   *( pString++ ) += LocalTime.wHour   % 10;
   *( pString++ ) += LocalTime.wMinute / 10;
   *( pString++ ) += LocalTime.wMinute % 10;
   *( pString++ ) += LocalTime.wSecond / 10;
   *( pString++ ) += LocalTime.wSecond % 10;
  #else
   DosGetDateTime (&DateTime);

   pString         = pDate;
   *( pString++ ) += (CHAR)(DateTime.year / 1000);
   DateTime.year   = DateTime.year        % 1000;
   *( pString++ ) += (CHAR)(DateTime.year / 100);
   DateTime.year   = DateTime.year        % 100;
   *( pString++ ) += (CHAR)(DateTime.year / 10);
   *( pString++ ) += (CHAR)(DateTime.year % 10);
   *( pString++ ) += DateTime.month       / 10;
   *( pString++ ) += DateTime.month       % 10;
   *( pString++ ) += DateTime.day         / 10;
   *( pString++ ) += DateTime.day         % 10;

   pString         = pTime + 2;
   *( pString++ ) += DateTime.hours       / 10;
   *( pString++ ) += DateTime.hours       % 10;
   *( pString++ ) += DateTime.minutes     / 10;
   *( pString++ ) += DateTime.minutes     % 10;
   *( pString++ ) += DateTime.seconds     / 10;
   *( pString++ ) += DateTime.seconds     % 10;
  #endif

  DBG3((MF__, "Time = '%.8s'", pTime));
  DBGOUT;
  return;
  }


/*------------------------------*/

VOID  sqlsleep ( INT2 limit )
  {
  #undef  MF__
  #define MF__ MOD__"sqlsleep"
  #if defined(_WIN32)
   int   i = 0;
  #endif

  DBGIN;

  DBG3((MF__, "limit = '%d'", limit));

  #if defined(_WIN32)
   fCtrl_C = FALSE;

   while (( i++ < limit ) && ( fCtrl_C == FALSE ))
     {
     SLEEP (1000);
     }
  #else
   SLEEP ( 1000 * limit );
  #endif

  DBGOUT;
  return;
  }

//
// ========================== LOCAL FUNCTIONS =================================
//

#if defined(_WIN32)

 static BOOL WINAPI sql01u_catch_event ( DWORD dwCtrlType )
   {
   #undef  MF__
   #define MF__ MOD__"sql01u_catch_event"

   DBGPAS;

   switch ( dwCtrlType )
     {
     case CTRL_C_EVENT:
       fCtrl_C = TRUE;  // --- used by sqlsleep()

       if ( pbGlobalCancelAddress )
         *pbGlobalCancelAddress = TRUE;

       if (( bRuntimeComponent     == TRUE ) &&
           ( pbGlobalCancelAddress == NULL ))
         {
         break;
         }

       return (TRUE);
       break;

     case CTRL_BREAK_EVENT:

       #if !defined(WINDOWS)
        sql04u_vt_crash_cleanup();
       #endif
       sql03u_finish ();
       break;

     case CTRL_CLOSE_EVENT:
     case CTRL_SHUTDOWN_EVENT:
       sql03u_finish ();
       break;
     }

   return (FALSE);
   }


/*------------------------------*/

#else

 static VOID _CDECL sql01u_catch_event ( INT SigType )
   {
   #undef  MF__
   #define MF__ MOD__"sql01u_catch_event"

   DBGPAS;
   signal ( SigType, SIG_IGN );

   switch ( SigType )
     {
     case SIGINT:

       if ( pbGlobalCancelAddress )
         *pbGlobalCancelAddress = TRUE;

       if ( FetchedSigInt != SIG_DFL &&
            FetchedSigInt != SIG_IGN &&
            FetchedSigInt != SIG_ERR &&
            FetchedSigInt != (SIGNALFUNCTYPE) sql01u_catch_event )
         {
         FetchedSigInt ( SigType );
         }
       else if (( bRuntimeComponent     == TRUE ) &&
                ( pbGlobalCancelAddress == NULL ))
         {
         EXITPROCESS ( 5 );
         }
       break;

     case SIGBREAK:

       if ( FetchedSigBrk != SIG_DFL &&
            FetchedSigBrk != SIG_IGN &&
            FetchedSigBrk != SIG_ERR &&
            FetchedSigBrk != (SIGNALFUNCTYPE) sql01u_catch_event )
         {
         FetchedSigBrk ( SigType );
         }
       else
         {
         sql04u_vt_crash_cleanup();
         sql03u_finish ();

         EXITPROCESS ( 0xFADE );
         }
       break;
     }

   signal ( SigType, sql01u_catch_event );

   return;
   }

#endif

/*------------------------------*/

static VOID _System sql01u_exit_handler ( ULONG   ulTermCode )
  {
  #undef  MF__
  #define MF__ MOD__"sql01u_exit_handler"
  DBGPAS;

  DBG3 (( MF__, "ulTermCode = %d", ulTermCode ));

  sql02u_next_exit_func ();
  }

/*------------------------------*/

#if defined(_WIN32)

 static LONG sql01u_init_reg_values ( VOID )
   {
   #undef  MF__
   #define MF__ MOD__"sql01u_init_reg_values"
   APIRET        rc           = NO_ERROR;
   BOOL          fEnabled     = FALSE;
   DWORD         DbgLevel     = 0;
   ULONG         ulValues     = 0;
   REG_ENTRY_REC RegistryEntries[7];
   PATHNAME      szKey;
   tsp00_Pathc     FullDiagFileName ;
   tsp01_RteError  RteError ;

   DBGPAS;

   strcpy ( szKey, REG_SK_BASE );
   strcat ( szKey, "\\" );
   strcat ( szKey, REG_SK_APPLIC );
   rc = sql50_reg_key_exists ( NULL, HKEY_CURRENT_USER, szKey );

   if ((  rc == ERROR_FILE_NOT_FOUND ) || (  rc == ERROR_PATH_NOT_FOUND ))
     {
     RegistryEntries[ulValues].pszValueName = REG_VN_CHAR_SET_ENABLED;
     RegistryEntries[ulValues].pValue       = &fEnabled;
     RegistryEntries[ulValues].ulValueSize  = sizeof(DWORD);
     RegistryEntries[ulValues].ulValueType  = REG_DWORD;
     ulValues++;

     RegistryEntries[ulValues].pszValueName = REG_VN_CHAR_SET_NAME;
     RegistryEntries[ulValues].pValue       = "IBM437_GER";
     RegistryEntries[ulValues].ulValueSize  = (ULONG)strlen( "IBM437_GER" );
     RegistryEntries[ulValues].ulValueType  = REG_SZ;
     ulValues++;

     RegistryEntries[ulValues].pszValueName = REG_VN_SQLOPT_ENABLED;
     RegistryEntries[ulValues].pValue       = &fEnabled;
     RegistryEntries[ulValues].ulValueSize  = sizeof(DWORD);
     RegistryEntries[ulValues].ulValueType  = REG_DWORD;
     ulValues++;

     RegistryEntries[ulValues].pszValueName = REG_VN_SQLOPT;
     RegistryEntries[ulValues].pValue       = "";
     RegistryEntries[ulValues].ulValueSize  = 0;
     RegistryEntries[ulValues].ulValueType  = REG_SZ;
     ulValues++;

     RegistryEntries[ulValues].pszValueName = REG_VN_DIAG_ENABLED;
     RegistryEntries[ulValues].pValue       = &fEnabled;
     RegistryEntries[ulValues].ulValueSize  = sizeof(DWORD);
     RegistryEntries[ulValues].ulValueType  = REG_DWORD;
     ulValues++;

     /* PTS 1106789*/
     if ( sqlGetIndependentWrkPath ( FullDiagFileName, TERM_WITH_DELIMITER_EO01, &RteError ) )
       strcat ( FullDiagFileName, USER_DIAGFILE_NAME ) ;
     else
       strcpy ( FullDiagFileName, FULL_USER_DIAGFILE_NAME ) ;
       
     RegistryEntries[ulValues].pszValueName = REG_VN_DIAG_NAME;
     RegistryEntries[ulValues].pValue       = FullDiagFileName;
     RegistryEntries[ulValues].ulValueSize  = (ULONG)strlen(FullDiagFileName);
     RegistryEntries[ulValues].ulValueType  = REG_SZ;
     ulValues++;

     RegistryEntries[ulValues].pszValueName = REG_VN_DEBUG_LEVEL;
     RegistryEntries[ulValues].pValue       = &DbgLevel;
     RegistryEntries[ulValues].ulValueSize  = sizeof(DWORD);
     RegistryEntries[ulValues].ulValueType  = REG_DWORD;
     ulValues++;

     // --- check array size
     RTE_ASSERT(sizeof (RegistryEntries)/sizeof(RegistryEntries[0]) < ulValues,
                (ERR_ASSERT, MF__, __LINE__));

     rc = sql50_reg_put_applic_values ( NULL, NULL, HKEY_CURRENT_USER, "", "", NULL,
                                        ulValues, RegistryEntries );

     if ( rc != NO_ERROR )
       {
       MSGCD (( ERR_WRITE_CHAR_SET_OPT_VALS, rc ));
       }
     }

   return ( rc );
   }

/*------------------------------*/

 VOID _System  sql01u_dummy ( VOID )
   {
   DBGPAS;
   }

/*------------------------------*/

 static BOOL sql01u_start_test_thread ( VOID )
   {
   #undef  MF__
   #define MF__ MOD__"sql01u_start_test_thread"
   HANDLE                        hThread;
   DWORD                         dwThreadId;
   APIRET                        rc;

   DBGPAS;

   rc = CREATE_THREAD(&dwThreadId,
                      &hThread,
                      sql01u_dummy,
                      NULL, 0, 4096 );

   if ( rc != NO_ERROR )
     return ( FALSE );

   CloseHandle(hThread);

   return ( TRUE );
   }

#endif

//
// =============================== END ========================================
//
