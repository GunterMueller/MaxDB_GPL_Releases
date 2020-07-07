/*!
  @file           vos50k.c
  @author         RaymondR
  @brief          kernel exit/exception handling
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
#include "geo002.h"
#include "geo007_1.h"
#include "gos00k.h"
#include "gos003.h"
#include "heo54k.h"
#include "hsp100.h"
#include "heo15.h"
#include "heo670.h"
#include "RunTime/RTE_KSS.h"
#include "RunTime/Configuration/RTEConf_ParameterAccessSimple.h"
#include "KernelCommon/ParameterNames/KernelParam_RunTime.h"
#include "RunTime/MemoryManagement/RTEMem_AWEAllocator.h"
#include "Messages/Msg_OutputKnlDiag.h"
#include "gos60.h"
#include "SAPDBCommon/SAPDB_Names.h"

#if defined(_FASTCAP)
# include "FastCap.h"  /* nocheck */
#endif

/*
 *  DEFINES
 */
#define MOD__  "VOS50KC : "
#define MF__   MOD__"UNDEFINED"


#if defined (ENABLE_SEH_DUMP) && defined (_M_IX86)
# define TRYLEVEL_NONE             0xFFFFFFFF
# define MAX_SCOPETABLE_ENTRIES    1024
# define MAX_SEH_FRAMES            128
#define  SEH_DIAG_FILE             _T("SEHDiag")
#define  ASS_JMP_RELATIVE          0xE9
#define  ASS_MOVE_EAX_MEM          0xB8

# define ENTER_CRIT_SEC()                                          \
            if (!CritSecInitialized) { CritSecInitialized = TRUE;  \
              InitializeCriticalSection(&CritSec); }               \
            EnterCriticalSection(&CritSec); CritSecCount++;

# define EXIT_CRIT_SEC()                                           \
            if (CritSecCount) { CritSecCount--;                    \
              LeaveCriticalSection(&CritSec); }
#endif

extern void Msg_RegistryDump();

/*
 *  MACROS
 */


/*
 *  LOCAL TYPE AND STRUCT DEFINITIONS
 */
#if defined (ENABLE_SEH_DUMP) && defined (_M_IX86)
 typedef struct SCOPETABLE_ENTRY 
 { 
   DWORD                             TryLevel; 
   FARPROC                           pFilterFunc; 
   FARPROC                           pHandlerFunc; 
 } SCOPETABLE_ENTRY; 

 typedef struct VC_EXCEPTION_REGISTRATION
 { 
   SCOPETABLE_ENTRY                  *pScopeTable; 
   DWORD                             TryLevel; 
   DWORD                             _ebp; 
 } VC_EXCEPTION_REGISTRATION; 

 typedef struct VCPP_EXCEPTION_REGISTRATION
 { 
   DWORD                             TryLevel; 
   DWORD                             _ebp; 
 } VCPP_EXCEPTION_REGISTRATION; 

 typedef struct EXCEPTION_REGISTRATION 
 { 
   struct EXCEPTION_REGISTRATION*    Prev; 
   FARPROC                           Handler; 

   union 
   {
     VC_EXCEPTION_REGISTRATION       C;
     VCPP_EXCEPTION_REGISTRATION     Cpp;
   };

 } EXCEPTION_REGISTRATION; 


  typedef struct ASS_5BYTE
  {
    unsigned char Code;
    unsigned long Value;
  } ASS_5BYTE;
#endif




/*
 *  EXPORTED VARIABLES
 */

#if defined (ENABLE_SEH_DUMP) && defined (_M_IX86)
 extern int _except_handler3(PEXCEPTION_RECORD, EXCEPTION_REGISTRATION *,
                             PCONTEXT, PEXCEPTION_RECORD); 
#endif

/*
 * LOCAL VARIABLES
 */
#if defined (ENABLE_SEH_DUMP) && defined (_M_IX86)
 static   FILE         *fpSEH = NULL;
#endif

/*
 * LOCAL FUNCTION PROTOTYPES
 */
static VOID  sql50k_exceptcleanup              ( DWORD                      threadId,
                                                 LPEXCEPTION_POINTERS       lpExcPtrs,
                                                 BOOL                       fMiniDump, 
                                                 BOOL                       fDumpFlag, 
                                                 BOOL                       fTraceFlag,
                                                 DWORD                      Tid );
static VOID  sql50k_exitcleanup                ();
static VOID  sql50k_abort                      ( BOOL                       fWriteMiniDump );
static VOID  sql50k_cleanup_communication_ports( VOID );
static VOID sql50k_kill_UKTs                   ( VOID );
static VOID  sql50k_queue_mailslot_wakeup_evt  ( PSZ                        pszMailSlotName,
                                                 PVOID                      pRequest,
                                                 ULONG                      ulRequestLen );

#if defined (ENABLE_SEH_DUMP) && defined (_M_IX86)
 static VOID sql50k_show_seh_frame             ( FILE                       *fpSEH,
                                                 EXCEPTION_REGISTRATION     *pExcRec );
#endif

static VOID  sql50k_dump_ak                     ( VOID ); // CR 1103950
static VOID  sql50k_write_mini_dump             ( DWORD                      threadId,
                                                  LPEXCEPTION_POINTERS       pExcPtrs );


/*
 * ========================== GLOBAL FUNCTIONS ================================
 */

VOID  _vabort ( BOOLEAN write_crash_dump, ULONG ulLine, PSZ pszModule )
{
#undef  MF__
#define MF__ MOD__"_vabort"
  
  DBGPAS;
  
  DBG1 ((MF__, "_vabort exception, %s: %d", pszModule, ulLine));
  MSGCD(( ERR_KERNEL_VABORT_EXEPT_CHK, pszModule, ulLine ));

  sql50k_abort( write_crash_dump );
}

/*------------------------------*/

// F.H. 23.08.95 used in vsp26 only

VOID  sqlabort (VOID)
{
#undef  MF__
#define MF__ MOD__"sqlabort"
  DBGPAS;

  DBG3 ((MF__, "generate a ABORT_EXCEPTION!"));
  MSGCD(( ERR_KERNEL_VABORT_EXEPT ));

  RaiseException(XCPT_ABORT_EXCEPTION, 0, 0, NULL);
}

/*------------------------------*/


VOID vdump_rte ( VOID )
{
  char *RTEDumpName = XPARAM(szRTEDumpFilename) ;

  if ( *RTEDumpName != '\0' )
  {
      STARTUPINFO         StartupInfo;
      PROCESS_INFORMATION ProcessInfo;
      tsp01_RteError      RteError;
      tsp00_Pathc         szPgmPath;
      DWORD               rc = NO_ERROR;
      HANDLE              hFile   = INVALID_HANDLE_VALUE;
      char                szCmdBuf[sizeof(tsp00_Pathc) + sizeof(SQL_DBNAMEC) + 
                                   sizeof(VF_FILENAMEC) + 30 ];

      if ( sqlGetDbrootPgmPath (szPgmPath, TERM_WITH_DELIMITER_EO01, &RteError) )
      {
          kgs.ulDebugLevel = 3;

          hFile = CreateFile( RTEDumpName,
                              GENERIC_WRITE | GENERIC_READ, 
                              FILE_SHARE_READ | FILE_SHARE_WRITE, &kgs.FileSA,
                              CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL , NULL );

          if ( hFile == INVALID_HANDLE_VALUE )
              rc = GetLastError();
          else
          {
              GetStartupInfo (&StartupInfo);
              memset ( &ProcessInfo, 0, sizeof(ProcessInfo) );

              sprintf(szCmdBuf, "\"%scons.exe\" \"%s\" sh all", szPgmPath, kgs.szServerDB );

              SetHandleInformation(hFile, HANDLE_FLAG_INHERIT, TRUE);

              StartupInfo.lpTitle    = szCmdBuf;
              StartupInfo.dwFlags    = STARTF_USESTDHANDLES;
              StartupInfo.hStdOutput = hFile;
              StartupInfo.hStdError  = hFile;

              if ( false ==  CreateProcess ( 0, szCmdBuf, 0, 0, TRUE, CREATE_NO_WINDOW,
                                             0, 0, &StartupInfo, &ProcessInfo) )
              {
                  rc = GetLastError();
              }
              else
              {
                  WaitForSingleObject(ProcessInfo.hProcess, 60000);

                  CloseHandle( ProcessInfo.hProcess );
                  CloseHandle( ProcessInfo.hThread );
              }
              CloseHandle( hFile );
          }
      }

      if ( NO_ERROR != rc )
      {
          sprintf( szCmdBuf, "%scons.exe %s sh all > %s",
                   szPgmPath, kgs.szServerDB, RTEDumpName);
          system (szCmdBuf);
      }

  }
}

/*------------------------------*/

ULONG  sql50k_get_termination_timeout ( VOID )
{
#undef  MF__
#define MF__ MOD__"sql50k_get_termination_timeout"
  ULONG             ulTermTimeout = DEFAULT_TERMINATION_TIMEOUT;
  REG_ENTRY_REC     RegistryEntries[1];
  PATHNAME          szSubKey;
  LONG              rc;
  
  DBGIN;
  
  RegistryEntries[0].pszValueName = REG_VN_TERMINATION_TIMEOUT;
  RegistryEntries[0].pValue       = &ulTermTimeout;
  RegistryEntries[0].ulValueSize  = sizeof(DWORD);
  RegistryEntries[0].ulValueType  = REG_DWORD;
  
  strcpy ( szSubKey, kgs.szServiceName);
  strcat ( szSubKey, "\\"REG_SK_SERVICE_PARAM );
  
  rc = sql50_reg_get_service_values ( NULL, szSubKey, 1, RegistryEntries );
  
  if ((rc != NO_ERROR) || (ulTermTimeout < MIN_TERMINATION_TIMEOUT))
    ulTermTimeout = DEFAULT_TERMINATION_TIMEOUT;
  
  DBGOUT;
  return ulTermTimeout;
}

/*------------------------------*/

VOID  vabort ( BOOLEAN write_crash_dump )
{
#undef  MF__
#define MF__ MOD__"vabort"
  
  DBGPAS;
  
  DBG3 ((MF__, "vabort exception!"));
  MSGCD(( ERR_KERNEL_VABORT_EXEPT ));

  sql50k_abort( write_crash_dump );
}

/*------------------------------*/


VOID  sql50k_rte_abort ( BOOLEAN write_crash_dump, ULONG ulLine, PSZ pszModule )
{
#undef  MF__
#define MF__ MOD__"sql50k_rte_abort"
  
  DBGPAS;
  
#ifdef CHK_ABORT
  DBG3 ((MF__, "RTE abort exception, %s: %d"));
  MSGCD (( ERR_RTE_ABORT_EXEPT_CHK ));
#else
  DBG3 ((MF__, "RTE abort exception!"));
  MSGCD (( ERR_RTE_ABORT_EXEPT ));
#endif
  
  sql50k_abort( write_crash_dump );
}

/*------------------------------*/

VOID _System sql50k_exithandler ( VOID )
{
#undef  MF__
#define MF__ MOD__"sql50k_exithandler"

  sql50k_exitcleanup();
  
  return;
}

/*------------------------------*/

#ifdef WANT_SYSTEM_PAG_CACHE_DUMP
extern void RTEMem_DumpSystemPageCache();
#endif

DWORD sql50k_excepthandler ( LPEXCEPTION_POINTERS lpExcPtrs )
{
#undef  MF__
#define MF__ MOD__"sql50k_excepthandler"
  INT                      i;
  PSZ                      pszPhysDiagFileName;
  PEXCEPTION_RECORD        pExceptionRecord = lpExcPtrs->ExceptionRecord;
  DWORD                    Tid              = THREADID;
  PTASK_CTRL_REC           pCurrTask;
  PCHAR                    pStatusString;
  eo670TraceContext        traceContext;
  BOOL                     fWriteMiniDump   = TRUE;
  
  DBGPAS;
  
  sql60_reset_crit_section(); // --- crashing thread might be in a
                              //     crtical section
 
  Msg_BeginOfTerminationPhase();

    
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
    for (i = pExceptionRecord->NumberParameters; i < EXCEPTION_MAXIMUM_PARAMETERS; i++)
      pExceptionRecord->ExceptionInformation[i] = 0;
    
    switch(pExceptionRecord->ExceptionCode)
    {
    case STATUS_IN_PAGE_ERROR            :
        pStatusString = "In page error"; break;
    case STATUS_NONCONTINUABLE_EXCEPTION :
        pStatusString = "Noncontinuable exception"; break;
    case STATUS_INVALID_DISPOSITION      :
        pStatusString = "Invalid disposition"; break;
    case STATUS_ACCESS_VIOLATION         :
        pStatusString = "Access violation";  break;
    case STATUS_INTEGER_DIVIDE_BY_ZERO   :
        pStatusString = "Integer divide by zero"; break;
    case STATUS_FLOAT_DIVIDE_BY_ZERO     :
        pStatusString = "Float divide by zero"; break;
    case STATUS_FLOAT_INVALID_OPERATION  :
        pStatusString = "Float invalid operation"; break;
    case STATUS_ILLEGAL_INSTRUCTION      :
        pStatusString = "Illegal instruction"; break;
    case STATUS_PRIVILEGED_INSTRUCTION   :
        pStatusString = "Privileged instruction"; break;
    case STATUS_INTEGER_OVERFLOW         :
        pStatusString = "Integer overflow"; break;
    case STATUS_FLOAT_OVERFLOW           :
        pStatusString = "Floatoverflow"; break;
    case STATUS_FLOAT_UNDERFLOW          :
        pStatusString = "Float underflow"; break;
    case STATUS_FLOAT_DENORMAL_OPERAND   :
        pStatusString = "Float denormal operand"; break;
    case STATUS_FLOAT_INEXACT_RESULT     :
        pStatusString = "Inexact result"; break;
    case STATUS_FLOAT_STACK_CHECK        :
        pStatusString = "Float stack check"; break;
    case STATUS_DATATYPE_MISALIGNMENT    :
        pStatusString = "Datatype misalignment"; break;
    case STATUS_ARRAY_BOUNDS_EXCEEDED    :
        pStatusString = "Array bounds exceeded"; break;
    case STATUS_STACK_OVERFLOW           :
        pStatusString = "Stack overflow"; break;
    default:
        pStatusString = "Unknown";
    }
    
    if ( STATUS_ACCESS_VIOLATION == pExceptionRecord->ExceptionCode )
    {
       // EXCEPTION_ACCESS_VIOLATION: The first element of the 'ExceptionInformation' array 
       // contains a read-write flag that indicates the type of operation that caused the
       // access violation. If this value is zero, the thread attempted to read the
       // inaccessible data. If this value is 1, the thread attempted to write to an
       // inaccessible address. 
       // The second array element specifies the virtual address of the inaccessible data.
       MSGALL((ERR_EXCEPTION_DESC_ACCESS_VIOLATION, pStatusString,
               pExceptionRecord->ExceptionCode, pExceptionRecord->ExceptionAddress,
               1 == pExceptionRecord->ExceptionInformation[0] ? "write" : "read", 
               pExceptionRecord->ExceptionInformation[1] ));
    }
    else
    {
        MSGALL((ERR_EXCEPTION_DESC, pStatusString, 
                pExceptionRecord->ExceptionCode, pExceptionRecord->ExceptionAddress,
                pExceptionRecord->ExceptionInformation[0], pExceptionRecord->ExceptionInformation[1],
                pExceptionRecord->ExceptionInformation[2], pExceptionRecord->ExceptionInformation[3]));
    }

    break;

  case STATUS_GUARD_PAGE_VIOLATION     :

      MSGALL(( ERR_EXCEPT_GUARD_PAGE_VIOLATION, pExceptionRecord->ExceptionAddress ));

      pCurrTask = THIS_UKT_CTRL->pCTask;

      if ( pCurrTask  )
         MSGALL(( ERR_GUARD_EXCEPTION_INFO, pCurrTask->ulTaskIndex, pExceptionRecord->ExceptionInformation[1],
                  pCurrTask->Context.pStackBase, pCurrTask->Context.pStackLimit)) ;
    break;

  case STATUS_BREAKPOINT:
    MSGALL((ERR_DEBUG_BREAK, pExceptionRecord->ExceptionAddress ));
          eo670_CTraceStackInit();
          traceContext.threadHandle   = GetCurrentThread();
          traceContext.pThreadContext = lpExcPtrs->ContextRecord;
          eo670_CTraceContextStack( &traceContext );
    return(EXCEPTION_CONTINUE_SEARCH);
    break;
    
  case XCPT_MICROSOFT_CPP_EXCEPTION:
    MSGALL(( ERR_MICROSOFT_CPP_EXCEPTION, pExceptionRecord->ExceptionAddress,
             pExceptionRecord->ExceptionInformation[0], pExceptionRecord->ExceptionInformation[1],
             pExceptionRecord->ExceptionInformation[2], pExceptionRecord->ExceptionInformation[3] ));
    break;
    
  case XCPT_ABORT_EXCEPTION:
    fWriteMiniDump  = FALSE;
    break;

  case XCPT_ABORT_EXCEPTION_MINI_DUMP:
    break;
    
  case STATUS_CONTROL_C_EXIT:
    switch(pExceptionRecord->ExceptionInformation[0])
    {
    case CTRL_LOGOFF_EVENT   :
    case CTRL_CLOSE_EVENT    :
      
      if ( kgs.fRunningAsNTService )
      {
        DBG3 ((MF__, "Exception Ignored!"));
        return(EXCEPTION_CONTINUE_EXECUTION);
      }
      
      MSGALL(( INFO_EXCEPTION_SIG_KILL ));
      fWriteMiniDump  = FALSE;
      break;
      
    case CTRL_SHUTDOWN_EVENT :
      MSGALL(( INFO_EXCEPTION_SIG_KILL ));
      fWriteMiniDump  = FALSE;
      break;
      
    case CTRL_C_EVENT       :
    case CTRL_BREAK_EVENT   :
    default                 :
      DBG3 ((MF__, "Exception Ignored!"));
      return(EXCEPTION_CONTINUE_EXECUTION);
    }
    break;
    
  default:

    if ( pExceptionRecord->ExceptionFlags & EXCEPTION_NONCONTINUABLE )
      {
      for (i = pExceptionRecord->NumberParameters; i < EXCEPTION_MAXIMUM_PARAMETERS; i++)
        pExceptionRecord->ExceptionInformation[i] = 0;
      
      MSGALL((ERR_EXCEPTION, pExceptionRecord->ExceptionCode, pExceptionRecord->ExceptionAddress,
        pExceptionRecord->ExceptionInformation[0], pExceptionRecord->ExceptionInformation[1],
        pExceptionRecord->ExceptionInformation[2], pExceptionRecord->ExceptionInformation[3]));

      break;
      }

    return(EXCEPTION_CONTINUE_SEARCH);
    break;
    }

  if ( 0 == InterlockedCompareExchange( &kgs.CrashingTid, Tid, 0 ) ) // -- not already crashing?
    {
    if ( pExceptionRecord->ExceptionCode == STATUS_CONTROL_C_EXIT )
      {
      sql51k_auto_shutdown ();

      if ( *kgs.pDBState == SERVER_COLD )
        {
        MSGCD (( INFO_DB_STOPPED ));
        *kgs.pDBState = SERVER_STOP;
        }
      else
        {
        MSGALL(( WRN_DB_STOPPED_IN_WARM_MODE ));
        *kgs.pDBState = SERVER_KILL;
        }

      sql50k_exceptcleanup( Tid, lpExcPtrs, FALSE, FALSE, TRUE, Tid );
      }
    else
      {
      if( DuplicateHandle( GetCurrentProcess(), GetCurrentThread(),
                           GetCurrentProcess(), &kgs.CrashingThrdHandle,
                           DUPLICATE_SAME_ACCESS, TRUE,
                           DUPLICATE_SAME_ACCESS ) == 0 )
      {
          // -- we will wait for trace writer termination!
          if (  kgs.pTW )
             kgs.CrashingThrdHandle = kgs.pTW->pUKT->ThrdCtrlHeader.hThrd; 
          else
             kgs.CrashingThrdHandle = INVALID_HANDLE_VALUE;
      }

      /* if ulNumOfTasks is 0, tasking obviously has not been initialized. In this case 
         the following code itself would crash... */
      if( 0 == kgs.ulNumOfTasks ) 
      {
          *kgs.pDBState = SERVER_ABORT;
          eo670_CTraceStackInit();
          traceContext.threadHandle   = GetCurrentThread();
          traceContext.pThreadContext = lpExcPtrs->ContextRecord;
          eo670_CTraceContextStack( &traceContext );

          if ( fWriteMiniDump )
              sql50k_write_mini_dump( Tid, lpExcPtrs );
      }
      else
        {
          /* dump RTE must be called before the server abort state is set to avoid wrong task states ( PTS 1115012 )*/
          vdump_rte();
          *kgs.pDBState = SERVER_ABORT;

          sql60_get_diag_filename ( &pszPhysDiagFileName );

          if ( pszPhysDiagFileName != NULL)
          {
            MSGCEL (( ERR_KERNEL_ABORTS_DIAG_INFO, pszPhysDiagFileName ));
            MSGD   (( INFO_KERNEL_ABORTS ));
          }
          else
            MSGALL (( INFO_KERNEL_ABORTS ));

          /* --------------------------------------------------------------------------------- *
           * please do any further dump and cleanup operations inside the following function!  *
           * --------------------------------------------------------------------------------- */
          sql50k_exceptcleanup( Tid, lpExcPtrs, fWriteMiniDump, TRUE, TRUE, Tid );
        }
      }
    }
  else if ( kgs.CrashingTid != Tid )
    {
    // - another thread is chrashing. we will wait and see
    DWORD ulTerminationTimeOut = kgs.ulTerminationTimeOut * 2;
    WAIT_THREAD( kgs.CrashingThrdHandle, ulTerminationTimeOut );
    }

  /* PTS 1116365 */
  MSGD(( INFO_DIAG_HIST_FORCED ));
  sql50k_exitcleanup();

  // - signal that the proccess/service is down!
  *kgs.pDBState = SERVER_STOPPED;
  UPDATE_SERVICE_OR_CONSOLE_STATE( *kgs.pDBState );

  if ( kgs.fRunningAsNTService == FALSE )
  {
      // if we are running in console mode; delayed close of console window
      SleepEx ( CRASH_SLEEP_TIME * 1000, FALSE );
  }

  if (( kgs.fContinueSearchExcpt         == FALSE ) ||
      ( pExceptionRecord->ExceptionCode  == XCPT_ABORT_EXCEPTION )) // - 'abort' without crash dump
    {
    HANDLE   hProcess;

    if ( DuplicateHandle( GetCurrentProcess(), GetCurrentProcess(), GetCurrentProcess(),
                          &hProcess, DUPLICATE_SAME_ACCESS, TRUE, DUPLICATE_SAME_ACCESS ))
      TerminateProcess ( hProcess, pExceptionRecord->ExceptionCode );
    else
      ExitProcess ( 0 );
   }

  // - the standard exception handler is calling the
  //   default debugger ( registry: 'AeDebug' )
  return(EXCEPTION_CONTINUE_SEARCH); 
  }
  
/*------------------------------*/

void sql50k_resume_tracewriter()
{
    DWORD   result;
    HANDLE  hThread = kgs.pTW->pUKT->ThrdCtrlHeader.hThrd;
    DWORD   terminationTimeOut;

    //
    // --- send a resume tracewriter request
    //
    RESUME_TRACEWRITER(kgs.pTW);

    //
    // --- wait for termination of the tracewriter thread
    //
    terminationTimeOut = sql50k_get_termination_timeout();
    terminationTimeOut = terminationTimeOut > 30 ? terminationTimeOut : 30; // set minimum value 30 seconds

    MSGD (( INFO_TW_TERM_TIMEOUT, terminationTimeOut ));

    kgs.pTW->TaskAlive = 0; // reset active flag. Will be set by a running tracewriter

    // wait for tracewriter thread termination
    while (( result = WaitForSingleObject(hThread, 1000 )) == WAIT_TIMEOUT  && terminationTimeOut )
    {
        if ( 0 != kgs.pTW->TaskAlive ) // Tracewriter still alive?
        {
            kgs.pTW->TaskAlive = 0;                                                 // Reset the active flag
            terminationTimeOut = sql50k_get_termination_timeout();                  // and reload the timeout value.
            terminationTimeOut = terminationTimeOut > 30 ? terminationTimeOut : 30; // Set minimum value 30 seconds.
        }
        else
            --terminationTimeOut;  // The tracewriter has not signaled alive state. Decrease timeout value.
    }

    // Unexpected wait error occured?
    if ( terminationTimeOut &&  WAIT_OBJECT_0 != result )
    {
        // Okay, we have to wait until the timeout elapses or the tracewriter keeps silent!
        do 
        {
            kgs.pTW->TaskAlive = 0;
            Sleep ( 60000 );  // wait a minute
            terminationTimeOut = terminationTimeOut > 60 ? terminationTimeOut - 60 : 0;
        } 
        while (  0 != kgs.pTW->TaskAlive && terminationTimeOut );
    }

    if ( WAIT_OBJECT_0 == result )  
    {   // The tracewriter has stop, invalidate its handle.
        CloseHandle( kgs.pTW->pUKT->ThrdCtrlHeader.hThrd );
        kgs.pTW->pUKT->ThrdCtrlHeader.hThrd = INVALID_HANDLE_VALUE;
    }

    return;
}

/*------------------------------*/

#if defined (ENABLE_SEH_DUMP) && defined (_M_IX86)

 VOID  sql50_dump_SEH ( PSZ  pszFormatStr, ... )
 {
 #undef  MF__
 #define MF__ MOD__"sql50_dump_SEH"

   static CRITICAL_SECTION      CritSec;
   static BOOL                  CritSecInitialized = FALSE;
   static DWORD                 CritSecCount       = 0; 
   DWORD                        Frames             = 0;
   va_list                      args;
   EXCEPTION_REGISTRATION       *pExcRec; 
   TCHAR                        szHeader[MAX_DIAG_HEADER_LEN + 1];

   // --- Get a pointer to the head of the chain at FS:[0] 
   __asm mov eax, FS:[0] 
   __asm mov [pExcRec], EAX 

   va_start(args, pszFormatStr);
   
   ENTER_CRIT_SEC();

   if ( fpSEH == NULL )
     fpSEH = fopen ( SEH_DIAG_FILE, _T("w"));
   
   if ( fpSEH != NULL )
   {
     sql60_create_diag_header ( 0, 0, NULL, time_diag_header, szHeader );

     fprintf ( fpSEH, "\n- %s  ", szHeader );
     vfprintf ( fpSEH, pszFormatStr, args );

     __try 
     {
       while (( TRYLEVEL_NONE != (unsigned)pExcRec ) && ( ++Frames < MAX_SEH_FRAMES ))
       { 
         sql50k_show_seh_frame( fpSEH, pExcRec ); 
         pExcRec = (EXCEPTION_REGISTRATION *)(pExcRec->Prev); 
       }

       if ( Frames >= MAX_SEH_FRAMES )
         fprintf ( fpSEH, _T(" ... too many frames, Output trucated\n") );
     }
    __except( EXCEPTION_EXECUTE_HANDLER )
    {
      fprintf ( fpSEH, _T("Error evaluating exception structures.\n") );
    }
       

    fflush(fpSEH);
   }

   EXIT_CRIT_SEC();
   va_end(args);
 }
#endif

/*
 * ========================== LOCAL FUNCTIONS =================================
 */
extern void Msg_RegistryDump();

static VOID  sql50k_exceptcleanup ( DWORD                      threadId,
                                    LPEXCEPTION_POINTERS       lpExcPtrs, 
                                    BOOL                       fMiniDump,
                                    BOOL                       fDumpFlag, 
                                    BOOL                       fTraceFlag,
                                    DWORD                      Tid )
  {
#undef  MF__
#define MF__ MOD__"sql50k_exceptcleanup"
  BOOL                     fExitProccess    = FALSE;
  PEXCEPTION_RECORD        pExceptionRecord = lpExcPtrs->ExceptionRecord;
  PUKT_CTRL_REC            pTmpUKTCtrl;

  DBGPAS;

  __try
    {
    /* --------------------------------------------------------------------------------*
     *   all cleanup and dump operations bevor resuming tracewrite should be done here *
     *---------------------------------------------------------------------------------*/

    // for Robin.. a good place to switch to TerminiationPhase when Msg_IOutput::GetDiagnosticOutput() is made available...
    // ...well - if this was a c++ - file...
    /*
        {
        Msg_List tmpMsgList;
        if ( !Msg_IOutput::GetDiagnosticOutput().BeginOfTerminationPhase(tmpMsgList) )
        {
        Msg_List dummyTmpMsgList;
        Msg_IOutput::GetEmergencyOutput().Output(tmpMsgList, dummyTmpMsgList);
        }
        }
    */

    if ( fDumpFlag )
    {   
       eo670TraceContext   traceContext;

       eo670_CTraceStackInit();
       traceContext.threadHandle   = GetCurrentThread();
       traceContext.pThreadContext = lpExcPtrs->ContextRecord;
       eo670_CTraceContextStack( &traceContext );

       if ( fMiniDump )
          sql50k_write_mini_dump( threadId, lpExcPtrs );

#      ifdef WANT_SYSTEM_PAG_CACHE_DUMP
        RTEMem_DumpSystemPageCache();
#      endif

       Msg_RegistryDump();

       if ( pExceptionRecord->ExceptionCode != STATUS_GUARD_PAGE_VIOLATION ) // PTS 1106437
       {
           sql50k_dump_ak();   // CR 1103950
       }
    }  /* end of dump trace */


    /* -------------------------------------------------------------------------*
     *   REMOVE UKTs, tasks threads                                             *
     *--------------------------------------------------------------------------*/

    //  --- wake all user kernel threads
    for ( pTmpUKTCtrl =  kgs.pFirstUKTCtrl; pTmpUKTCtrl <= kgs.pLastUKTCtrl; pTmpUKTCtrl++ )
      {
      if ( pTmpUKTCtrl->ThrdCtrlHeader.Tid == Tid )
        sql55k_free_regions ( pTmpUKTCtrl );
      else
        {
        if ( pTmpUKTCtrl->ThrdCtrlHeader.hevSem != (HEV) INVALID_HANDLE_VALUE )
          sql72k_wake_ukt( pTmpUKTCtrl );
        }
      }

    sql54k_dev0_devi_crash_cleanup ( Tid );

    // --- is it posible to resume the tracewriter?
    if (( kgs.pfDump                        != NULL )            &&
        ( kgs.pfTraceFlag                   != NULL )            &&
        ( kgs.pTW->TaskState                != TSK_INITIALIZED_EO00 ) &&
        ( kgs.pTW->pUKT->ThrdCtrlHeader.Tid != Tid )             &&
        ( kgs.Dev0.lTraceDevspaceNo         != UNDEF ))
      {
      // --- set trace and dump flag
      //
      *kgs.pfDump       = fDumpFlag;
      *kgs.pfTraceFlag  = fTraceFlag;

      //
      // --- send a resume tracewriter request
      //
      sql50k_resume_tracewriter();
      }
    }
  __except( EXCEPTION_EXECUTE_HANDLER ) 
    {
      MSGD (( 19999, 1, "        ", "---> Exception cleanup aborted! <---" ));
    }
  
  return;
}

/*------------------------------*/

static VOID sql50k_exitcleanup ()
{
#undef  MF__
#define MF__ MOD__"sql50k_exitcleanup"

  static BOOL                    fWorkDone = FALSE;
  tsp00_Versionc                 RTEVersionString;
  COMM_REQUEST_REC               CommRequest;  // - CommRequest os used to wakeup the 
                                               //   requestor thread during process stop phase. 
                                               //   No initialization  necessary!!
  
  DBGPAS;
  
  __try
    {
#     if defined(_FASTCAP)
	   CAPStop(); // stop profiling
	   SLEEP(3000);
#     endif

    if ( fWorkDone == FALSE )
      {
      fWorkDone = TRUE;

      // --- set the RTE version string
      if ( sql02_get_platform_id() == VER_PLATFORM_WIN32_NT )
        sp100_GetVersionString(COMP_NAME_RTE_NT_SP100, s100buildnumber, RTEVersionString);
      else
        sp100_GetVersionString(COMP_NAME_RTE_WIN9X_SP100, s100buildnumber, RTEVersionString);
      
      
      if (( pkss != NULL ) && ( pkss->KernelVersion[0] != '\0' ))
        {
        MSGD   (( INFO_VERSION, pkss->KernelVersion ));
        MSGD   (( INFO_DB_TERM, kgs.szServerDB ));
        MSGCEL (( INFO_DB_TERM, kgs.szServerDB ));
        }
      else
        {
        MSGD   (( INFO_VERSION, kgs.szServerDB ));
        MSGD   (( INFO_DB_TERM, RTEVersionString ));
        MSGCEL (( INFO_DB_TERM, kgs.szServerDB ));
        }
      
      // --- signal that kernel is dying
      memset ( &CommRequest, 0, sizeof (CommRequest));
      CommRequest.ulReqType = SQL_RTE_WAKEUP;
      sql50k_queue_mailslot_wakeup_evt( MAIL_REQ,  &CommRequest, sizeof (CommRequest) );
      SLEEP(1000); // -- wait a moment until the mailslot has received the message

      sql50k_kill_UKTs();
      sql50k_cleanup_communication_ports ();

      if ( RTEMem_AWEAvailable() )
        RTEMem_AWEReleaseInstance (); // all UKTs are killed we can stop AWE!

      /* PTS 1103824 */
      if ( kgs.XParam )
      {
          eo15CompleteMessageFiles(XPARAM(ulMaxMessageFiles), kgs.MessageBufferSpace);
      }

      sql60_close_diag_file ();
      sql60_close_util_diag_file ();
      sql60_close_event_log();
#ifndef NO_OLD_DIAG_FILES
      RTE_WriteDatabaseStopTime();
#endif
#ifdef NEW_DIAG_FILES
      Msg_WriteStopMessage();
#endif
      sql60_close_error_diag_file();
      sql60_close_event_file();

#     if defined (ENABLE_SEH_DUMP) && defined (_M_IX86)
       if ( fpSEH != NULL ) 
         fclose ( fpSEH );
#     endif

      // --- try to post XSTART/XSTOP semaphore
      sql61k_post_xstart_sem ( kgs.szServerDB, TRUE );
      sql61k_post_xstop_sem  ( kgs.szServerDB );
      }
    }
  __except( EXCEPTION_EXECUTE_HANDLER ) 
    {
        MSGD (( 19999, 1, "        ", "---> Exit cleanup aborted! <---" ));
#ifndef NO_OLD_DIAG_FILES
        RTE_WriteDatabaseStopTime();
#endif
#ifdef NEW_DIAG_FILES
        Msg_WriteStopMessage();
#endif
    }

#if defined(_FASTCAP)
 exit(0); // we need an exit to flush the buffers!
#endif

  return;
}

/*------------------------------*/

static VOID  sql50k_abort ( BOOL fWriteMiniDump )
{
#undef  MF__
#define MF__ MOD__"sql50k_abort"

  DBGPAS;

  /*
   * activide exception handler
   */
  if ( fWriteMiniDump )
    RaiseException(XCPT_ABORT_EXCEPTION_MINI_DUMP, 0, 0, NULL);
  else
    RaiseException(XCPT_ABORT_EXCEPTION, 0, 0, NULL);
  
  // ---  should not come here
  EXITPROCESS( 1 );
}

/*------------------------------*/

static VOID  sql50k_queue_mailslot_wakeup_evt ( PSZ    pszMailSlotName,
                                               PVOID  pRequest,
                                               ULONG  ulRequestLen )
{
#undef  MF__
#define MF__ MOD__"sql50k_queue_mailslot_wakeup_evt"
  HANDLE            hHandle;
  PID               pid;
  APIRET            rc;
  
  DBGPAS;
  
  rc = sql41c_open_queue_mailslot ( &pid, &hHandle, pszMailSlotName,
                                    kgs.szServerDB );
  if ( rc == NO_ERROR )
  {
    sql41c_write_queue_mailslot( hHandle, pRequest,
                                 ulRequestLen, 0, NULL );
    sql41c_close_queue_mailslot ( hHandle );
  }
  
  return;
}

/*------------------------------*/

static VOID sql50k_cleanup_communication_ports ()
{
#undef  MF__
#define MF__ MOD__"sql50k_cleanup_communication_ports"
  PTASK_CTRL_REC           pCurrTask;
  
  DBGPAS;
  
  //
  //  --- clean up all communication ports
  //
  for (pCurrTask = kgs.pFirstTaskCtrl;
       ( pCurrTask != NULL ) && (pCurrTask <= kgs.pLastTaskCtrl);
       pCurrTask++)
  {
    // --- cleanup communication
    sql53k_comm_termination_cleanup ( pCurrTask );
  }
  
  return;
}

/*------------------------------*/

static VOID sql50k_kill_UKTs ()
{
#undef  MF__
#define MF__ MOD__"sql50k_kill_UKTs"
  PUKT_CTRL_REC            pTmpUKTCtrl;
  PUKT_CTRL_REC            pUKTCtrl = THIS_UKT_CTRL;
  SAPDB_Bool               fUKTFound = false;
  DWORD                    result;


  for ( pTmpUKTCtrl =  kgs.pFirstUKTCtrl;
        pTmpUKTCtrl <= kgs.pLastUKTCtrl;
        pTmpUKTCtrl++ )
  {
    if (( pTmpUKTCtrl                           != NULL )                       &&
        ( pTmpUKTCtrl->ThrdCtrlHeader.hevSem    != (HEV) INVALID_HANDLE_VALUE ) &&
        ( pTmpUKTCtrl->ThrdCtrlHeader.ThrdState != UKT_TERMINATED )             &&
        ( pTmpUKTCtrl->ThrdCtrlHeader.hThrd     != INVALID_HANDLE_VALUE )       &&
        ( pUKTCtrl                              != pTmpUKTCtrl )) // - be sure, it's not me!
    {
      DWORD ulTerminationTimeOut = 120;

      while (( WAIT_TIMEOUT == ( result = WaitForSingleObject( pTmpUKTCtrl->ThrdCtrlHeader.hThrd, 0 ))) &&
             ( ulTerminationTimeOut-- ))
      {
         SLEEP(1000); 
      }

      if ( WAIT_TIMEOUT == result )  
          TerminateThread(pTmpUKTCtrl->ThrdCtrlHeader.hThrd, 1);

      CloseHandle(pTmpUKTCtrl->ThrdCtrlHeader.hThrd);
      pTmpUKTCtrl->ThrdCtrlHeader.hThrd = INVALID_HANDLE_VALUE;
    }
  }
  return;
}

/*------------------------------*/

static VOID  sql50k_dump_ak ( VOID )
{
  __try
    {
    if ( THIS_UKT_CTRL != NULL )  // - am i a UKT?
      {
      PTASK_CTRL_REC  pCurrTask = THIS_UKT_CTRL->pCTask;

      if ( pCurrTask             && 
           pCurrTask->AcvPointer &&
           pCurrTask->AkDumpProc &&
           XPARAM(fAKDumpAllowed) )
        {
          MSGCD(( INFO_CALLING_AK_DUMP, pCurrTask->ulTaskIndex ));
          pCurrTask->AkDumpProc(pCurrTask->AcvPointer) ;
        }
      }
    }
  __except( EXCEPTION_EXECUTE_HANDLER ) 
    {
      MSGD (( 19999, 1, "CRASH   ", "---> Dump AK aborted! <---" ));
    }

  return;
}

/*------------------------------*/

static VOID  sql50k_write_mini_dump ( DWORD                      threadId,
                                      LPEXCEPTION_POINTERS       pExcPtrs )
{
    tsp00_C256c   XpValueC;
    tsp00_ErrText ErrText;
    static BOOL   fAlreadyWritten = false;

    if ( false == fAlreadyWritten )
    {
        fAlreadyWritten = true;
        if ( RTEConf_ParamaterAccessGetParam ( kgs.szServerDB, KERNELPARAM_MINI_DUMP, XpValueC, ErrText ) )
        {
            if ( 0 == strcmp("FULL", (char*)XpValueC ))
                eo670_WriteMiniDump ( threadId, pExcPtrs, SAPDB_KNL_MINI_DUMP, true, false, &kgs.AdminSA );   // write full mini dump file
            else if ( 0 == strcmp("NORMAL", (char*)XpValueC ))
                eo670_WriteMiniDump ( threadId, pExcPtrs, SAPDB_KNL_MINI_DUMP, false, false, &kgs.AdminSA );  // write normal mini dump file
        }
    }
}
/*------------------------------*/

#if defined (ENABLE_SEH_DUMP) && defined (_M_IX86)
 static VOID sql50k_show_seh_frame ( FILE                        *fpSEH,
                                     EXCEPTION_REGISTRATION      *pExcRec ) 
 {
 #undef  MF__
 #define MF__ MOD__"sql50k_show_seh_frame"

   SCOPETABLE_ENTRY   *pScopeTableEntry;
   TCHAR              TmpString[30];
   DWORD              i, Start, Cnt;
 

   if ( pExcRec->Cpp.TryLevel < MAX_SCOPETABLE_ENTRIES )
   {
     //  try/catch block
     //
     ASS_5BYTE  *pAss;
     FARPROC    pHandler;

     pAss = ((ASS_5BYTE*)pExcRec->Handler) + 1;

     if ( pAss.Code == ASS_JMP_RELATIVE )
     {
       pAss.Value = 
     }


     if ( (void*)_except_handler3 == (void*)pExcRec->Handler )
       sprintf( TmpString, _T("%s"), _T("_except_handler3") );
     else
       sprintf( TmpString, _T("%08X"), pExcRec->Handler );

     fprintf ( fpSEH, _T("\n  FS:[0]: '%08X'  CppTryLevel: '%d'  Handler: '%s'  _ebp: '%08X'\n"),
               pExcRec, pExcRec->Cpp.TryLevel + 1, TmpString, pExcRec->Cpp._ebp ); 
   }
   else
   {
     //  __try/__except block
     //
     if ( (void*)_except_handler3 == (void*)pExcRec->Handler )
       sprintf( TmpString, _T("%s"), _T("_except_handler3") );
     else
       sprintf( TmpString, _T("%08X"), pExcRec->Handler );

     fprintf ( fpSEH, _T("\n  FS:[0]: '%08X'  Scopetable: '%08X'  Handler: '%s'  _ebp: '%08X'\n"),
               pExcRec, pExcRec->C.pScopeTable, TmpString, pExcRec->C._ebp ); 
     
     if ( pExcRec->C.TryLevel > MAX_SCOPETABLE_ENTRIES )
       pExcRec->C.TryLevel = MAX_SCOPETABLE_ENTRIES;
     
     for ( i = 0, pScopeTableEntry = pExcRec->C.pScopeTable;
           i <= pExcRec->C.TryLevel; 
           i++ ) 
     {
       // --- find next 'TRYLEVEL_NONE' entry
       for ( Start = i;
             (i + 1) <= pExcRec->C.TryLevel &&
             pScopeTableEntry[i + 1].TryLevel != TRYLEVEL_NONE;
             i++ ) { ; }
       
       for ( Cnt = 0; Cnt <= i - Start; Cnt++ )
       {
         if ( pScopeTableEntry[i - Cnt].TryLevel == TRYLEVEL_NONE )
           sprintf( TmpString, _T("%s"), _T("NONE") );
         else
           sprintf( TmpString, _T("%08X"), pScopeTableEntry[i - Cnt].TryLevel );
         
         fprintf ( fpSEH, _T("   %d. __except: %08X {}: %08X  -  TryLevel: %s\n"), Cnt + 1,
                   pScopeTableEntry[i - Cnt].pFilterFunc, 
                   pScopeTableEntry[i - Cnt].pHandlerFunc,
                   TmpString ); 
       }

       if ( pExcRec->C.TryLevel == MAX_SCOPETABLE_ENTRIES )
         fprintf ( fpSEH, _T(" ... too many scapetable entries, Output trucated\n") );
     }
   }

 }  
#endif

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
