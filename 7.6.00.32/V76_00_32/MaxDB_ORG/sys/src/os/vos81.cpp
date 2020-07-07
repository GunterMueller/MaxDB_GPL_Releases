/*!
  @file           vos81.cpp
  @author         RaymondR
  @brief          COORDINATOR_thread
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

/*
 * INCLUDE FILES
 */
//#include           "gos00.h"
#include           <stdlib.h>
#include           "gos41.h"
#include           "RunTime/RTE_Types.h"
#include           "heo00.h"
#include           "heo46.h"
#include           "gos00k.h"
#include           "geo002.h"
#include           "gos003.h"
#include           "geo003.h"
#include           "hsp100.h"
#include           "geo007_1.h"
#include           "geo67.h"
#include           "gos44.h"
#include           "heo15.h"
#include           "geo43_0.h"
#include           "Trace/Trace_InfoPage.h"
#include           "RunTime/System/RTESys_SystemInfo.h"
#include           "RunTime/Diagnose/RTEDiag_ServerCrashHist.h"
#include           "RunTime/System/RTESys_NTLoadDLLFunc.h"
#include           "heo54k.h"
#include           "RunTime/System/RTESys_NTHighResTimer.h"
#include           "RunTime/RTE_KernelMain.h"
#include           "RunTime/Threading/RTEThread_Console.h"
#include           "RunTime/RTE_KSS.h"
#include           "hsp77.h"
#include           "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.h"
#include           "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp"
#include           "RunTime/Configuration/RTEConf_ParameterAccessSimple.h"
#include           "KernelCommon/ParameterNames/KernelParam_RunTime.h"
#include           "SAPDBCommon/SAPDB_Names.h"
#include           "RunTime/Threading/RTEThread_Requestor.hpp"
#include           "RunTime/Tasking/RTETask_WorkerPool.hpp"
#include           "RunTime/RTE_MessageList.hpp"
#include           "RunTime/RTE_Crash.hpp"
#include           "RunTime/Test/RTETst_Default.hpp"
#include           "KernelCommon/Kernel_ExtendedState.hpp"
#include           "RunTime/RTE_Version.h"

#if defined(_FASTCAP)
# include "fastcap.h"   /* nocheck */
#endif

#include <rtcapi.h>

/*
 *  DEFINES
 */
#define MOD__       "VOS81.C : "
#define MF__        MOD__"UNDEFINED"

#define MAX_KT_WAIT                   10000           // - wait time (msec)
#define HEURISTICS_HIBERNATION_POS    6
#define MAX_UKT_START_WAIT_TIME       60      // -- sleep time


/*
 *  authentication flags
 */
#define AFL_BASIC                             0x1
#define AFL_SCRAMMD5                          0x2

/*
 *  MACROS
 */


/*
 *  LOCAL TYPE AND STRUCT DEFINITIONS
 */


/*
 * EXTERNAL VARIABLES
 */
extern "C" HANDLE                   hModule = INVALID_HANDLE_VALUE;

/*
 *  EXPORTED VARIABLES
 */
KERNEL_GLOBAL_SEC            kgs;
KERNEL_SHARED_SEG            *pkss = NULL;
FLAG_COMM_SEG                *pfcs = NULL;


#define TC_EXIT             0

/*
 * LOCAL VARIABLES
 */


/*
 * LOCAL FUNCTION PROTOTYPES
 */
LONG  sql81k_get_serverdb_dbroot            ( PSZ   pszServerDB );
LONG  sql81k_startup                        ( PSZ   pszServerDB );
LONG  sql81k_create_kernel_threads          ( VOID );
LONG  sql81k_create_UKTs                    ( VOID );
LONG  sql81k_create_SAs                     ( );
LONG  sql81k_init_kernel_shared_segments    ( VOID );
LONG  sql81k_set_max_num_of_file_handles    ( VOID );
LONG  sql81k_set_rundir                     ( PSZ pszServerDB );
VOID  sql81k_death                          ( VOID );

LONG sql81k_create_io_compl_port            ( VOID );
LONG sql81k_init_thread_local_storage       ( VOID );
bool sql81k_continue_search_exception       ( VOID );
BOOL sql81k_get_debug_break_entry           ( PSZ   pszServerDB );
LONG sql81k_set_path_env                    ( VOID );

LONG sql81_InitEventing                     ( );
bool sql81k_always_check_conn               ( VOID );
LONG sql81k_open_diag_files                 ( PSZ pszServerDB);

int sql81k_handle_RTC_failure               ( int          errorType,
                                              const char * fileName,
                                              int          sourceLine,
                                              const char * moduleName,
                                              const char * formatString,
                                              ... );
void sql81k_handle_purecall					( void );

/*
 * ========================== GLOBAL FUNCTIONS ================================
 */
LONG sql81k_Coordinator ( PSZ              pszServerDB,
                          INT              argc,
                          PCHAR            argv[] )
{
#undef  MF__
#define MF__ MOD__"sql81k_Coordinator"
  LONG                  rc                        = NO_ERROR;
  PATHNAME              szModFileName;
  CHAR                  szTitle[40 + 1];
  C40C                  szEventSource;
  SQL_DBNAMEC           szTempServerDB;
  teo07_Thread          mainThread;
  tsp00_ErrTextc        threadErrtext;
  teo07_ThreadErr       threadOk;
  HEV                   hevTmpCoord = 0;

  DBGIN;


  __try   // --- add exit handler to a thread's chain of ex. handlers
  {
    __try
    {
#     if !defined (_WIN64)
      _RTC_SetErrorFunc(sql81k_handle_RTC_failure);
#     endif

#     if defined(_M_IX86)
	  // Set the function to be called when a pure virtual function is called
	  _set_purecall_handler( sql81k_handle_purecall );
#     endif

      if (pszServerDB == 0 || *pszServerDB == '\0')
      {
        MSGALL (( ERR_SERVERDB_NO_SET ));
        DBG1  (( MF__, "Database name must be set" ));
        DBGOUT;
        return( ERROR_INVALID_PARAMETER );
      }

      // --- Initialize the Thread Local Storage to make sure that calls to
      //  THIS_UKT_CTRL (macro) return 0 until the tasking is active
      if ((rc = sql81k_init_thread_local_storage ()) != NO_ERROR)
        return(rc);

      pszServerDB = CharUpper(pszServerDB);

      // --- Set 'ServerDB' and 'DBROOT' of the KGS.
      //     This must be done first!
      if ((rc = sql81k_get_serverdb_dbroot (pszServerDB)) != NO_ERROR)
          return(rc);

      // --- build event source name and title string
      //
      strcpy (szTempServerDB, pszServerDB );
      CharLower(szTempServerDB);
      *szTempServerDB = (CHAR)toupper(*szTempServerDB);

      rc = sql41c_open_event_sem ( &hevTmpCoord, SEM_COORD, kgs.szServerDB, ERROR_ACCESS_DENIED);

      if ((rc == NO_ERROR) || (rc == ERROR_ACCESS_DENIED))
      {
          MSGALL (( ERR_DATABASE_ALREADY_RUN, &kgs.szServerDB[0] ));
          sql41c_close_event_sem ( hevTmpCoord, "COORD" );
          return(rc);
      }

      if ( false == sql81k_PortableInstallationAcitve() )
      {
          strcpy ( szTitle, DB_TITLE);
          strcat ( szTitle, " - " );
          strcat ( szTitle, szTempServerDB );

          strcpy ( szEventSource, SERVERDB_EVENT_SOURCE_NAME );
          strcat ( szEventSource, szTempServerDB );

          // --- reopen event log
          rc = sql60_reopen_event_log ( szTitle, szEventSource );

          if ( rc != NO_ERROR )
              ABORT();
      }

      // --- set default drive and path to 'rundirectory'
      if ((rc = sql81k_set_rundir(pszServerDB)) != NO_ERROR)
          return(rc);

      // --- create security descriptors like 'kgs.AdminSA'
      sql81k_create_SAs();

      // --- Open diag files -- new style files are opened implicitly
#ifndef NO_OLD_DIAG_FILES
      sql81k_open_diag_files(pszServerDB);
#endif
      if ( RTE_KernelMain(argc, argv) )
      {
          return( ERROR_INVALID_PARAMETER );
      }

      // --- disable critical error message box popups
      //
      SetErrorMode( SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX );

      sqlinit_multithreading(&mainThread, threadErrtext, &threadOk);
      if ( threadOk != THR_OK_EO07 )
      {
    	MSGALL (( ERR_INIT_MULTITHREADING, &threadErrtext[0] )) ;
        DBG1  (( MF__, "Multithreading initialization failed" ));
        DBGOUT;
        return( ERROR_INVALID_PARAMETER );
      }

      if (GetModuleFileName( NULL, szModFileName, sizeof(szModFileName)))
      {
        if (strstr ( szModFileName, DB_FAST_KERN_FILE_NAME ))
          kgs.dwEventCategory = NTKERN_CATEGORY_FAST_SERVERDB;
        else if (strstr ( szModFileName, DB_QUICK_KERN_FILE_NAME ))
          kgs.dwEventCategory = NTKERN_CATEGORY_QUICK_SERVERDB;
        else if (strstr ( szModFileName, DB_SLOW_KERN_FILE_NAME ))
          kgs.dwEventCategory = NTKERN_CATEGORY_SLOW_SERVERDB;
        else
          kgs.dwEventCategory = NTKERN_CATEGORY_NONE;
      }
      else
        kgs.dwEventCategory = NTKERN_CATEGORY_NONE;

      if ( sql81k_get_debug_break_entry( pszServerDB ) == FALSE )
        kgs.ulStaticDebugBreakPos = 0;

      if ( kgs.ulStaticDebugBreakPos == 1 )
        DebugBreak();

      SetConsoleCtrlHandler(NULL, TRUE);

      if ( argc > 0 )
        kgs.ulStartupOptions = (ULONG)atol( argv[0]);
      else
      {
          if ( kgs.fRunningAsNTService == FALSE )
          {
              kgs.ulStartupOptions = IGNORE_AUTORESTART;
          }
          else
          {
              kgs.ulStartupOptions = FORCE_AUTORESTART;
          }
      }
      // --- Initalize function
      if ((rc = sql81k_startup ( pszServerDB )) != NO_ERROR)
      {
        *kgs.pDBState = SERVER_ABORT;

        // --- post XSTART semaphore
        if (pszServerDB)
          sql61k_post_xstart_sem ( pszServerDB, TRUE );

        // --- remove exception handler from a thread's chain of ex. handlers

        UPDATE_SERVICE_OR_CONSOLE_STATE(*kgs.pDBState);

        if ( kgs.fRunningAsNTService == FALSE )
          SLEEP ( CRASH_SLEEP_TIME * 1000 );

        return ( rc );
      }


      /* PTS 1103824
       *  Create the MessageBuffer Worker thread
       */
      eo15InitMessageBufferSpace( XPARAM(ulMaxMessageFiles),
                                  (teo15MessageBufferSpace *)kgs.MessageBufferSpace );

      /* PTS 1108768 Space for Tracebuffer used for InMemory tracing */
      Trace_InfoPageSetup();

      // --- Create threads (DEV0, Timer ...)
      if (sql81k_create_kernel_threads () != NO_ERROR)
        ABORT();

      // --- Create user kernel threads
      if (sql81k_create_UKTs () != NO_ERROR)
        ABORT();

      sql81k_death ();
    }
    __except( sql50k_excepthandler(GetExceptionInformation()) ) {;}
  }
  __finally { sql50k_exithandler(); }

  // - signal that the proccess/service is down!
  *kgs.pDBState = SERVER_STOPPED;
  UPDATE_SERVICE_OR_CONSOLE_STATE( *kgs.pDBState );

  DBGOUT;
  return ( NO_ERROR );
  }

#ifdef STATIC_LINK
  #pragma handle(sql81k_kernel_main)
#endif

/*------------------------------*/

BOOL sql81k_wakeup_corrdinator ( VOID )
  {
  #undef  MF__
  #define MF__ MOD__"sql81k_wakeup_corrdinator"
  APIRET   rc = NO_ERROR;
  static BOOL         fCoordAwaked = FALSE;

  // --- wake up coordinator thread
  if (( kgs.Coord.ThrdCtrlHeader.hevSem != 0 )                    &&
      ( kgs.Coord.ThrdCtrlHeader.hevSem != INVALID_HANDLE_VALUE ) &&
      ( *kgs.pDBState                   != SERVER_STOPPED )       &&
      ( fCoordAwaked                    == FALSE ))
    {
    fCoordAwaked = TRUE;
    sql72k_wake_kt ( &kgs.Coord.ThrdCtrlHeader );

    return (TRUE);
    }

  return (FALSE);
  }

 /*------------------------------*/

BOOL sql81k_hibernation_enabled( VOID )
   {
   #undef  MF__
   #define MF__ MOD__"sql81k_hibernation_enabled"
   LONG             rc = NO_ERROR;
   REG_ENTRY_REC    RegistryEntries[1];
   char             *pBuffer;


   DBGIN;

   RegistryEntries[0].ulValueSize  = 1;
   RegistryEntries[0].pszValueName = REG_VN_HEURISTICS;
   RegistryEntries[0].ulValueType  = REG_BINARY;

   do
     {
     pBuffer                   = (char*)alloca(RegistryEntries[0].ulValueSize);
     RegistryEntries[0].pValue = pBuffer;

     rc = sql50_reg_get_values ( NULL, HKEY_LOCAL_MACHINE, REG_SK_NT_SYSTEM_POWER, 1, RegistryEntries );
     }
   while ( rc == ERROR_MORE_DATA );

   if ( rc == ERROR_FILE_NOT_FOUND )
     {
     DBGOUT;
     return( FALSE );
     }

   if ( rc == NO_ERROR )
     {
     if ( pBuffer[HEURISTICS_HIBERNATION_POS] == 0 )
       {
         DBGOUT;
         return( FALSE );
       }
     }

   DBGOUT;
   return( TRUE );
   }

/*------------------------------*/

BOOL sql81k_PortableInstallationAcitve()
  {
  #undef  MF__
  #define MF__ MOD__"sql81k_PortableInstallationAcitve"

  tsp01_RteError          RteError;
  tsp00_Pathc             Path;

  return sqlGetPortableInstallationRootPath(Path, TERM_WITHOUT_DELIMITER_EO01, &RteError );
  }


/*
 * ========================== LOCAL FUNCTIONS =================================
 */

static void sql81PrintStartUpInformation ()
  {
  #undef  MF__
  #define MF__ MOD__"sql81PrintStartUpInformation"
  SYSTEMTIME             DateTime;
  teo00_Int8             TotalPhys;
  teo00_Int8             AvailPhys;
  teo00_Int8             TotalVirtual;
  PSZ                    ProductSuite;
  ULONG                  majorVersion;
  SAPDB_Char             CPUType[64];

  MSGD (( INFO_VERSION, RTE_GetKernelVersion() ))
  MSGD (( INFO_VERSION, RTE_GetRTEVersion() ))

  MSGD (( INFO_DB_START_SERVERDB,   &kgs.szServerDB[0] ));
  /* PTS 1116151 */
  if ( XPARAM(szOfficialNode)[0] != 0 )
  {
      strncpy ( kgs.szServerNode, XPARAM(szOfficialNode), sizeof(tsp00_NodeId) );
      kgs.szServerNode[sizeof(tsp00_NodeId)] = 0;
      MSGCD (( INFO_USER_SPECIFIED_OFFICIAL_NODE , &kgs.szServerNode[0]));
  }
  else
  {
      MSGD (( INFO_DB_START_SERVERNODE, &kgs.szServerNode[0] ));
  }
  MSGD (( INFO_DB_START_PID,        kgs.KernelPid ));

  GetLocalTime(&DateTime);

  ProductSuite = sql02_get_product_suite() ;
  sql02_get_os_version (&majorVersion, NULL);

  if (majorVersion >= 0x0005)   /* PTS 1110642 */
  {
    RTE_NTMEMORYSTATUSEX    memoryStatus;

    memset (&memoryStatus, 0, sizeof(memoryStatus));
    memoryStatus.dwLength = sizeof(memoryStatus);
    RTESys_NTGlobalMemoryStatusEx(&memoryStatus);
    TotalPhys    = memoryStatus.ullTotalPhys;
    AvailPhys    = memoryStatus.ullAvailPhys;
    TotalVirtual = memoryStatus.ullTotalVirtual;
  }
  else
  {
    MEMORYSTATUS            memoryStatus;

    memset (&memoryStatus, 0, sizeof(memoryStatus));
    memoryStatus.dwLength = sizeof(memoryStatus);
    GlobalMemoryStatus(&memoryStatus);
    TotalPhys    = memoryStatus.dwTotalPhys;
    AvailPhys    = memoryStatus.dwAvailPhys;
    TotalVirtual = memoryStatus.dwTotalVirtual;
  }

  MSGD (( INFO_DB_START_DATE, DateTime.wYear, DateTime.wMonth,DateTime.wDay ));

  if ( kgs.pszUserName[0] != '\0' )    // - NT only
    MSGD (( INFO_DB_START_OWNER, &kgs.pszUserName[0] ));

  if ( ProductSuite )
    MSGD (( INFO_DB_START_PRODUCT_SUITE, ProductSuite));

  if( true == RTESys_GetCPUType(  CPUType, 64 ))
	MSGD (( INFO_DB_START_MACHINE, CPUType ));

  if ( kgs.ulNumOfProcessors != kgs.ulNumOfPhysicalProcessors )
    MSGD (( INFO_DB_START_NUM_LOGICAL_PROCESSORS, kgs.ulNumOfPhysicalProcessors, kgs.ulNumOfProcessors ))
  else
    MSGD (( INFO_DB_START_NUM_PROCESSORS, kgs.ulNumOfPhysicalProcessors ));
 

  MSGD (( INFO_DB_START_FIBER, XPARAM(fUseFibers) ? "YES" : "NO" ));
  MSGD (( INFO_DB_START_MAX_ADDRESS,           TotalVirtual >> 20 ));
  MSGD (( INFO_DB_START_PHYSICAL_MEMORY_TOTAL, TotalPhys    >> 20 ));
  MSGD (( INFO_DB_START_PHYSICAL_MEMORY_AVAIL, AvailPhys    >> 20 ));
  }

/*------------------------------*/

static LONG sql81k_startup ( PSZ   pszServerDB)
  {
  #undef  MF__
  #define MF__ MOD__"sql81k_startup"
  LONG                      rc          = NO_ERROR;
  HEV                       hevTmpCoord = 0;
  ULONG                     ulDiagOptions;
  SQL_NODEIDC               szNodeName;
  BOOL                      fOk;
  cTimerType                TimerType = cCPUTimer;
  #if defined (BELOW_NORMAL_PRIORITY_CLASS)
   ULONG                    MajorVersion;
  #endif

  // setup pointer to KGS structure
  RTE_KGS::Instance().SetKGS(&kgs);

/*
  // --- Set 'ServerDB' and 'DBROOT' of the KGS.
  //     This must be done first!
  if ((rc = sql81k_get_serverdb_dbroot (pszServerDB)) != NO_ERROR)
    return(rc);
*/
  if ((rc = sql81k_set_path_env ()) != NO_ERROR)
    return(rc);

  if ( kgs.ulStartupOptions & LOW_PROCESS_PRIORITY )
    {
    #if defined (BELOW_NORMAL_PRIORITY_CLASS)
     sql02_get_os_version ( &MajorVersion, NULL );

     if ( MajorVersion > 4 )
      SetPriorityClass( GetCurrentProcess(), BELOW_NORMAL_PRIORITY_CLASS );  // -- Windows 2000
     else
      SetPriorityClass( GetCurrentProcess(), IDLE_PRIORITY_CLASS );

    #else
     SetPriorityClass( GetCurrentProcess(), IDLE_PRIORITY_CLASS );
    #endif
    }

   rc = sql49c_get_user_info ( &kgs.pszUserName, NULL );

   if ( rc != NO_ERROR )
     kgs.pszUserName = "";
   else if ( kgs.fRunningAsNTService == TRUE )
     {
     sql49c_user_privilege_check ( &fOk );

     if ( !fOk )
       {
       MSGALL(( ERR_USER_MISSING_SERV_PRIV, &kgs.pszUserName[0] ));
       return(ERROR_ACCESS_DENIED);
       }
     }

  //
  // --- get node name
  //
  kgs.szServerNode[0] = '\0';

  rc = sql43_get_my_host_name ( kgs.szServerNode, sizeof(kgs.szServerNode) );

  if ( rc == NO_ERROR )
    {
    hModule = GetModuleHandle(NULL);   // - used in 'vos40lc'

    if ( sql43_tcpip_init () == TCPIP_INIT_OK )
      {
      rc = sql43_get_official_node_name ( kgs.szServerNode, szNodeName,
                                           sizeof(szNodeName));
      if ( rc != NO_ERROR )
        MSGCD(( WRN_OFF_NODE_NAME_NOT_FOUND, &kgs.szServerNode[0] ))
      else
        strcpy ( kgs.szServerNode, szNodeName );

      // --- we only need TCP/IP to get the node name
      sql43_tcpip_cleanup ();
      }
    }

  // --- After creating the security descriptors we are able to
  //     create COORDINATOR's semaphore
  rc = sql41c_create_event_sem ( &hevTmpCoord, SEM_COORD,
                                 kgs.szServerDB, 0, FALSE, &kgs.AdminSA );

  if (rc == ERROR_DUPLICATE_NAME)
    {
    MSGALL (( ERR_DATABASE_ALREADY_RUN, &kgs.szServerDB[0] ));
    DBG1  (( MF__, "Database '%s' already started", pszServerDB ));
    return(rc);
    }
  else if (rc != NO_ERROR)
    return(rc);

  // --- Read XPARAM values into the KGS-XParam area
  if ((rc = sql79k_read_params()) != NO_ERROR)
    return( rc );

  // --- init the High Resolution Timer
  switch ( XPARAM(szHiResTimerType[0]) )
    {
    case 'C':  // CPU
      TimerType = cCPUTimer;
      break;
    case 'P':  // PERFCNTR
      TimerType = cQueryPerfCntr;
      break;
    case 'T':  // TICKS
      TimerType = cTickCount;
      break;
    }

  // starting asynchronous time initialization
  RTESys_NTHRTimerInitializeAsync ( TimerType );

  // now we have the param values, initialize the KGS statistics
  RTE_KGS::Instance().InitStatistic();

  if ( kgs.fRunningAsNTService == TRUE )
    {
    ulDiagOptions = ENABLE_EVENT_LOG_OUTPUT |
                    ENABLE_DIAGFILE_OUTPUT;
    }
  else
    {
    ulDiagOptions = ENABLE_CONSOLE_OUTPUT |
                    ENABLE_DIAGFILE_OUTPUT;
    }

  if (rc != NO_ERROR)
    return( rc );

  // --- Make backup of diagnostic files if the database last time was shutting
  // --- down abnormally
  RTEDiag_MakeCrashHist (kgs.XParam, kgs.szServerDB);

  // --- Posting the flag for an abnormal database shutdown
  RTEDiag_SetCrashSem (CRASH_SEM_POST, kgs.szServerDB);
  kgs.dumpDiagFiles = false; //PTS 1115180

  // --- Open util diag files
  rc = sql60_open_util_diag_file ( XPARAM(szUtilityProtFile),
                                   XPARAM(ulUtilityProtSize), &kgs.FileSA );

  if (rc != NO_ERROR)
    return( rc );

  if (rc != NO_ERROR)
    return( rc );


  kgs.ulSystemPageSize          = RTESys_SystemPageSize();
  kgs.ulNumOfProcessors         = RTESys_NumberOfCPU();
  kgs.ulNumOfPhysicalProcessors = RTESys_NumberOfPhysicalCPUs();

  if ( false == XPARAM(fUseFibers) )
     XPARAM(uLoadBalancingCheck) = 0;

  sql81PrintStartUpInformation ();

  // Memory allocation limit etc...
  RTEConf_HandleSpecialParametersWithMessageOutput();

  // --- read the 'TaskCluster' and initialize control records
  if ((rc = sql70k_init_control_records ()) != NO_ERROR)
    return(rc);

  if ((rc = sql81k_init_kernel_shared_segments ()) != NO_ERROR)
    return(rc);

  rc = sql81_InitEventing();

  if (rc != NO_ERROR)
    return( rc );

  // --- After initializing the control records we are able to
  //     store COORDINATOR's semaphore
  kgs.Coord.ThrdCtrlHeader.hevSem = hevTmpCoord;

  kgs.fHibernationEnabled = sql81k_hibernation_enabled();

   // --- init the 'vclock' - timer
   sql52k_vclock_init ();

   if ( kgs.ulNumOfDevWorker )
       sql81k_create_io_compl_port();

   kgs.fContinueSearchExcpt    = sql81k_continue_search_exception();
   kgs.fAlwaysCheckConnections = sql81k_always_check_conn();

   /* Setup default RTE test and initialize RTE_TestParser singleton */
   SAPDBErr_MessageList      errList;
   RTETst_RegisterDefault(errList);

   RTEConf_Parameter::String valueString;

   if((RTEConf_ParameterAccess::Instance())->GetString( KERNELPARAM_UKT_CPU_REL, valueString, errList))
   {
      if ( 0 == strcmp("IDEAL_PROCESSOR", (char*)valueString ))
        kgs.UKTProcessorRelationship = UKT_CPU_RELATIONSHIP_IDEAL_PROCESSOR;
      else if ( 0 == strcmp("PROCESSOR_AFFINITY", (char*)valueString ))
        kgs.UKTProcessorRelationship = UKT_CPU_RELATIONSHIP_AFFINITY;
      else
        kgs.UKTProcessorRelationship = UKT_CPU_RELATIONSHIP_NONE;
   }
   else
      kgs.UKTProcessorRelationship = UKT_CPU_RELATIONSHIP_NONE;

  // wait for asynchronous time initialization
  RTESys_NTHRTimerInitializeAsyncWait();

  return (rc);
  }

/*------------------------------*/

static LONG sql81k_create_io_compl_port ( VOID )
  {
  #undef  MF__
  #define MF__ MOD__"sql81k_create_io_compl_port"
  LONG              rc = NO_ERROR;
  ULONG             ulNumOfConcurrentDevWorkers = 0;
  PATHNAME          szSubKey;
  REG_ENTRY_REC     RegistryEntries[1];

  DBGIN;

  if ( kgs.fRunningAsNTService == TRUE )
    {
    strcpy ( szSubKey, kgs.szServiceName);
    strcat ( szSubKey, "\\"REG_SK_SERVICE_PARAM );

    RegistryEntries[0].pszValueName = REG_VN_CONCURRENT_DEV_WORKERS;
    RegistryEntries[0].pValue       = &ulNumOfConcurrentDevWorkers;
    RegistryEntries[0].ulValueSize  = sizeof(DWORD);
    RegistryEntries[0].ulValueType  = REG_DWORD;

    rc = sql50_reg_get_service_values ( NULL, szSubKey, 1, RegistryEntries );

    if (rc != NO_ERROR)
      ulNumOfConcurrentDevWorkers = max(kgs.ulNumOfDevWorker/2, 1);
    else
      ulNumOfConcurrentDevWorkers = min(ulNumOfConcurrentDevWorkers,
                                        kgs.ulNumOfDevWorker);
    }
  else
    ulNumOfConcurrentDevWorkers = max(kgs.ulNumOfDevWorker/2, 1);

  kgs.hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE,
                                               NULL, 0,
                                               ulNumOfConcurrentDevWorkers );
  if ( kgs.hCompletionPort == NULL )
    {
    rc = GetLastError();
    MSGD (( ERR_CREATE_IO_COMPL_PORT, rc ));
    return (rc);
    }

   kgs.ulNumOfConcurrentDevWorkers = ulNumOfConcurrentDevWorkers;

  DBGOUT;
  return(rc);
  }


/*------------------------------*/

static LONG sql81k_get_serverdb_dbroot ( PSZ pszServerDB )
  {
  #undef  MF__
  #define MF__ MOD__"sql81k_get_serverdb_dbroot"
  LONG              rc = NO_ERROR;
  PSZ               pszDBRoot;
  PATHNAME         szSubKey;
  static PATHNAME  szDBRootEnv;
  REG_ENTRY_REC    RegistryEntries[1];
  DWORD            ValUseIndepPath ; /* PTS 1107043 */

  DBGIN;

  if (strlen (pszServerDB) >= sizeof (kgs.szServerDB))
    {
    MSGALL (( ERR_SERVERDB_TOO_LONG, sizeof (kgs.szServerDB) - 1 ));
    DBG1  (( MF__, "Database name too long, allowed are %u characters",
             sizeof (kgs.szServerDB) - 1 ));
    DBGOUT;
    return( -1 );
    }

  // --- set the 'ServerDB'
  strcpy (kgs.szServerDB, pszServerDB);

  // ---  get DBROOT from Environment
  if ( kgs.fRunningAsNTService == TRUE )
  {
    strcpy ( szSubKey, kgs.szServiceName);
    strcat ( szSubKey, "\\"REG_SK_SERVICE_PARAM );

    RegistryEntries[0].pszValueName = REG_VN_DBROOT;
    RegistryEntries[0].pValue       = kgs.szDBRoot;
    RegistryEntries[0].ulValueSize  = sizeof(kgs.szDBRoot);
    RegistryEntries[0].ulValueType  = REG_SZ;

    rc = sql50_reg_get_service_values ( NULL, szSubKey, 1, RegistryEntries );

    if ((rc != NO_ERROR) || ( !kgs.szDBRoot[0] ))
    {
        MSGALL (( ERR_DBROOT_NOT_SET ));
        DBG1  (( MF__, "DBROOT must be set" ));

        DBGOUT;
        return( ERROR_ENVVAR_NOT_FOUND );
    }

    // --- set DBROOT environment variable
    szDBRootEnv[sizeof(szDBRootEnv) - 1] = '\0';
    strcpy ( szDBRootEnv, DBROOT_ENV_VAR );
    strcat ( szDBRootEnv, "=" );
    strncat( szDBRootEnv, kgs.szDBRoot,
        sizeof(szDBRootEnv) - strlen(szDBRootEnv) - 1 );
    _putenv ( szDBRootEnv );

    /* PTS 1107043 */
    RegistryEntries[0].pszValueName        = REG_VN_USE_INDEP_PATH;
    RegistryEntries[0].pValue              = &ValUseIndepPath;
    RegistryEntries[0].ulValueSize         = sizeof(DWORD);
    RegistryEntries[0].ulValueType         = REG_DWORD;

    if  (sql50_reg_get_service_values ( NULL, szSubKey, 1, RegistryEntries ) == NO_ERROR)
        if (!ValUseIndepPath )
            eo01_PutEnvVarDoNotUseIndepPath();
        else
            eo01_RemoveEnvVarDoNotUseIndepPath();
    }
  else
  {
      if ( sql01c_get_dbroot (&pszDBRoot) == FALSE )
      {
          MSGALL (( ERR_DBROOT_NOT_SET ));
          DBG1  (( MF__, "DBROOT must be set" ));

          DBGOUT;
          return( ERROR_ENVVAR_NOT_FOUND );
      }
      else
          strncpy ( kgs.szDBRoot, pszDBRoot, sizeof(kgs.szDBRoot));
  }

  // --- set SYMBOL environment variable
  sql01c_set_symbol_path ( kgs.szDBRoot ) ;

  DBG3 ((MF__, "Database name: %s", kgs.szServerDB));
  DBG3 ((MF__, "DBRoot:        %s", kgs.szDBRoot));

  DBGOUT;
  return(rc);
  }

/*------------------------------*/

static LONG  sql81k_set_path_env (VOID)
  {
  #undef  MF__
  #define MF__ MOD__"sql81k_set_path_env"
  LONG             rc    = NO_ERROR;
  DWORD            dwLen = 0;
  PSZ              pszPath;
  PSZ              pszNewPath;
  PATHNAME         szPgmPath;
  PATHNAME         szSAPPath;

  rc = GETENV( "PATH", &pszPath );

  if ( rc != NO_ERROR )
    pszPath = "";

  sql01c_build_pgm_path ( kgs.szDBRoot, szPgmPath );
  sql01c_build_sap_path ( kgs.szDBRoot, szSAPPath);

  dwLen = (DWORD)(strlen("PATH=") + strlen(pszPath) + strlen(szPgmPath) + strlen(szSAPPath) + 4);

  if ( (rc = ALLOC_MEM ((PPVOID)&(pszNewPath), dwLen )) != NO_ERROR)
    {
    DBGOUT;
    return ( rc );
    }

  // --- set PATH environment variable
  strcpy ( pszNewPath, "PATH=" );
  strcat ( pszNewPath, szPgmPath );

  dwLen = (DWORD)strlen(pszNewPath);
  if ( pszNewPath[dwLen - 1] == '\\' )
  {
    pszNewPath[dwLen - 1]     = '\0';
    dwLen-- ;
  }

  pszNewPath[dwLen]      = ';';
  pszNewPath[dwLen + 1]  = '\0';

  strcat ( pszNewPath, szSAPPath );

  dwLen = (DWORD)strlen(pszNewPath);
  if ( pszNewPath[dwLen - 1] == '\\' )
  {
    pszNewPath[dwLen - 1]     = '\0';
    dwLen-- ;
  }

  pszNewPath[dwLen]      = ';';
  pszNewPath[dwLen + 1]  = '\0';

  strcat ( pszNewPath, pszPath );

  _putenv ( pszNewPath );

  DBGOUT;
  return (rc);
  }


/*------------------------------*/

static LONG  sql81k_set_rundir (PSZ pszServerDB)
  {
  #undef  MF__
  #define MF__ MOD__"sql81k_set_rundir"
  LONG                                 rc = NO_ERROR;
  PATHNAME                             szRunDir;

  // --- get the run directory from the parameter file
  RTEConf_Parameter parameterSet( pszServerDB );
  Msg_List err;
  SAPDB_UTF8        stringValue[RTECONF_MAXSTRINGLENGTH+1];
  if( !parameterSet.GetRundirectoryAbsolutePath( stringValue,err ))
  {
    {
    MSGALL (( ERR_XP_RUNDIR_MISSING, PAN_RUNDIR ));
    DBG1  (( MF__, "%s missing", PAN_RUNDIR ));
    return (-1);
    }
  }
  tsp00_C256c  szRunDirectoryParam;
  szRunDirectoryParam.rawAssign((const char *) &stringValue[0]);

  // --- substitute logical path name parts
  sql44c_subst_log_parts ( szRunDir, szRunDirectoryParam );

  if (!SetCurrentDirectory(szRunDir))
    {
    MSGALL (( ERR_WRONG_XP_RUNDIR, PAN_RUNDIR, &szRunDirectoryParam[0] ));
    DBG1  (( MF__, "Could not change directory - %s: %s", PAN_RUNDIR,
             szRunDirectoryParam ));
    return (-1);
    }

  return (rc);
  }

/*------------------------------*/

extern "C" APIRET sql81k_create_kernel_thread( SAPDB_Char const      *threadName
                                             , TID                   *pTid
                                             , HANDLE                *phThrd
                                             , LPVOID                 threadMain
                                             , LPVOID                 threadArg
                                             , SIZE_T                 threadStackSize
                                             , SAPDB_Bool             unconditional )
{
    APIRET rc = NO_ERROR;

    SAPDB_ULong SystemSize = ((threadStackSize + (1024*1024) - 1)&(~((1024*1024) - 1)));

    if ( !RTE_PseudoAllocStack(SystemSize, 0) )
    {
        rc = ERROR_NOT_ENOUGH_MEMORY;
        MSGD ((ERR_CREATING_THREAD, threadName, rc));
        if ( unconditional )
        {
            ABORT();
        }
        return rc;
    }

    rc = CREATE_THREAD( pTid,
                        phThrd,
                        (LPTHREAD_START_ROUTINE)threadMain,
                        threadArg,
                        CREATE_THRD_SUSPENDED,
                        threadStackSize);
    if (rc != NO_ERROR)
    {
        MSGD ((ERR_CREATING_THREAD, threadName, rc));
        DBG1 ((MF__, "Cannot create thread: '%s', rc = %d", threadName, rc))
        if ( unconditional )
        {
            ABORT();
        }
    }
    else
    {
        rc = RESUME_THREAD(*pTid, *phThrd );

        if (rc != NO_ERROR)
        {
            MSGD ((ERR_RESUMING_THREAD, threadName, rc));
            DBG1 ((MF__, "Cannot resume thread: '%s', rc = %d", threadName, rc))
            if ( unconditional )
            {
                ABORT();
            }
        }
    }

    if( rc != NO_ERROR )
    {
        RTE_PseudoFreeStack(SystemSize, 0);
    }

    return rc;
}

/*------------------------------*/

extern "C" LPVOID sql81k_create_kernel_fiber( SIZE_T                fiberStackSize
                                            , LPVOID                fiberMain
                                            , LPVOID                fiberArg )
{
    SAPDB_ULong SystemSize = ((fiberStackSize + (1024*1024) - 1)&(~((1024*1024) - 1)));

    if ( !RTE_PseudoAllocStack(SystemSize, 0) )
    {
        return 0;
    }

    LPVOID result = CreateFiber( fiberStackSize, (LPFIBER_START_ROUTINE)fiberMain, fiberArg );

    if( result == NULL)
    {
        RTE_PseudoFreeStack(SystemSize, 0);
    }

    return result;
}

/*------------------------------*/

externC VOID           sql81k_delete_kernel_fiber       ( SIZE_T                fiberStackSize
                                                        , LPVOID                fiberHandle )
{
    SAPDB_ULong SystemSize = ((fiberStackSize + (1024*1024) - 1)&(~((1024*1024) - 1)));
    RTE_PseudoFreeStack(SystemSize, 0);
    DeleteFiber( fiberHandle );
}

/*------------------------------*/

LONG  sql81k_create_kernel_threads          ( VOID );

static LONG sql81k_create_kernel_threads ( VOID )
  {
  #undef  MF__
  #define MF__ MOD__"sql81k_create_kernel_threads"
  APIRET                rc         = NO_ERROR;
  ULONG                 ulWaitTime = MAX_KT_WAIT;
  PDEV_WORKER_CTRL_REC  pDevWorkerCtrl;

  DBGIN;

  //
  // --- Clock  ( should be the first thread )
  //
  sql81k_create_kernel_thread( "Clock"
                             , &kgs.Clock.ThrdCtrlHeader.Tid
                             , &kgs.Clock.ThrdCtrlHeader.hThrd
                             , sql85k_clock
                             , 0 // no argument
                             , KT_CLOCK_STACK_SIZE
                             , true );

  //
  // --- Timer
  //
  sql81k_create_kernel_thread( "Timer"
                             , &kgs.Timer.ThrdCtrlHeader.Tid
                             , &kgs.Timer.ThrdCtrlHeader.hThrd
                             , sql83k_timer
                             , 0 // no argument
                             , KT_TIMER_STACK_SIZE
                             , true );

  //
  // --- Dev0
  //
  sql81k_create_kernel_thread( "Dev0"
                             , &kgs.Dev0.ThrdCtrlHeader.Tid
                             , &kgs.Dev0.ThrdCtrlHeader.hThrd
                             , sql54k_dev0
                             , 0 // no argument
                             , KT_DEV0_STACK_SIZE
                             , true );

  //
  // --- Start DEV_WORKER's
  //
  for ( pDevWorkerCtrl = kgs.pFirstDevWorkerCtrl;
        pDevWorkerCtrl <= kgs.pLastDevWorkerCtrl &&  pDevWorkerCtrl != NULL;
        pDevWorkerCtrl++)
    {
      sql81k_create_kernel_thread( "DevWorker"
                                 , &pDevWorkerCtrl->ThrdCtrlHeader.Tid
                                 , &pDevWorkerCtrl->ThrdCtrlHeader.hThrd
                                 , sql54k_dev_worker
                                 , pDevWorkerCtrl
                                 , KT_DEV_WORKER_STACK_SIZE
                                 , true );
    }

  //
  // --- ASYNC0
  //
  sql81k_create_kernel_thread( "Async0"
                             , &kgs.Async0.ThrdCtrlHeader.Tid
                             , &kgs.Async0.ThrdCtrlHeader.hThrd
                             , sql65k_async0
                             , 0 // no argument
                             , KT_ASYNC0_STACK_SIZE
                             , true );

  //
  // --- Console
  //
  sql81k_create_kernel_thread( "Console"
                             , &kgs.Console.ThrdCtrlHeader.Tid
                             , &kgs.Console.ThrdCtrlHeader.hThrd
                             , RTEThread_Console
                             , 0 // no argument
                             , KT_CONSOLE_STACK_SIZE
                             , true );

  //
  // --- Old Requestor
  //
  sql81k_create_kernel_thread( "Requestor"
                             , &kgs.Requestor.ThrdCtrlHeader.Tid
                             , &kgs.Requestor.ThrdCtrlHeader.hThrd
                             , sql84k_requestor
                             , 0 // no argument
                             , KT_REQUESTOR_STACK_SIZE
                             , true );

  RTEThread_Requestor&  requestor = RTEThread_Requestor::Instance();
  SAPDBErr_MessageList  messageList;

  if ( requestor.Create( RTEThread_Requestor::Normal, 32768,
                         kgs.Requestor.ThrdCtrlHeader.lThrdPrio,
                         messageList ) != RTEThread_Requestor::NoError )
  {
      RTE_Crash( messageList );
  }


  while (( kgs.Clock.ThrdCtrlHeader.ThrdState      == KT_INITIALIZED ) ||
         ( kgs.Timer.ThrdCtrlHeader.ThrdState      == KT_INITIALIZED ) ||
         ( kgs.Dev0.ThrdCtrlHeader.ThrdState       == KT_INITIALIZED ) ||
         ( kgs.Async0.ThrdCtrlHeader.ThrdState     == KT_INITIALIZED ) ||
         ( kgs.Console.ThrdCtrlHeader.ThrdState    == KT_INITIALIZED ) ||
         ( kgs.Requestor.ThrdCtrlHeader.ThrdState  == KT_INITIALIZED ) ||
         ( requestor.ThreadHasStarted()            == false ))
    {
    SLEEP( 100 );
    ulWaitTime -= 100;

    if ( ulWaitTime == 0 )
      {
      MSGD ((INFO_KT_INIT_TIMEOUT));
      DBG1 ((MF__, "Kernel threads initialization timeout"));
      break;
      }
    }

  /* Create worker thread for tasks */
  RTETask_CWrapperForCreateWorkerThread(KT_DEV_WORKER_STACK_SIZE);

  DBGOUT;
  return ( rc );
  }

/*------------------------------*/

static LONG  sql81k_create_UKTs ( VOID )
  {
  #undef  MF__
  #define MF__ MOD__"sql81k_create_UKTs"
  APIRET             rc = NO_ERROR;
  PUKT_CTRL_REC      pCurrUKTCtrl;
  DWORD              dwThreadStackSize;
  ULONG              ulWaitTime;

  DBGIN;

  for (pCurrUKTCtrl = kgs.pFirstUKTCtrl; pCurrUKTCtrl <= kgs.pLastUKTCtrl; pCurrUKTCtrl++)
    {
    if (kgs.pTI->pUKT == pCurrUKTCtrl)
      continue;

    dwThreadStackSize = (DWORD)kgs.ulUKTBaseStackSize;
    sql81k_create_kernel_thread( "UKT"
                               , &pCurrUKTCtrl->ThrdCtrlHeader.Tid
                               , &pCurrUKTCtrl->ThrdCtrlHeader.hThrd
                               , sql88k_ukt
                               , pCurrUKTCtrl
                               , dwThreadStackSize
                               , true );
    }

  // --- Wait until the UKTs are up and running
  for (pCurrUKTCtrl = kgs.pFirstUKTCtrl; pCurrUKTCtrl <= kgs.pLastUKTCtrl; pCurrUKTCtrl++)
    {
    if (kgs.pTI->pUKT == pCurrUKTCtrl)
      continue;

      for ( ulWaitTime = MAX_UKT_START_WAIT_TIME * 10;
          (pCurrUKTCtrl->ThrdCtrlHeader.ThrdState == UKT_INITIALIZED) && ulWaitTime;
          --ulWaitTime )
      {
          SLEEP( 1000 / 10 ); // -- sleep a moment
      }
    }

  dwThreadStackSize = (DWORD)kgs.ulUKTBaseStackSize;
    sql81k_create_kernel_thread( "UKT"
                               , &kgs.pTI->pUKT->ThrdCtrlHeader.Tid
                               , &kgs.pTI->pUKT->ThrdCtrlHeader.hThrd
                               , sql88k_ukt
                               , kgs.pTI->pUKT
                               , dwThreadStackSize
                               , true );
  DBGOUT;
  return ( rc );
  }

extern "C" void Msg_RegistryDump();

/*------------------------------*/

VOID sql81k_death ( VOID )
  {
  #undef  MF__
  #define MF__ MOD__"sql81k_death"
  PUKT_CTRL_REC              pTmpUKTCtrl;
  APIRET                     rc;

  DBGIN;

  // --- Wait until resumed by XSTOP
  kgs.Coord.ThrdCtrlHeader.ThrdState   = KT_SLEEPING;

  rc = sql41c_wait_event_sem (kgs.Coord.ThrdCtrlHeader.hevSem, (ULONG) -1,
                              "Coord");

  kgs.Coord.ThrdCtrlHeader.ThrdState   = KT_RUNNING;

  if (rc != NO_ERROR )
    ABORT();


  if ( *kgs.pDBState == SERVER_ABORT )
    {
        MSGD(( INFO_DIAG_HIST_FORCED ));
        SLEEP ( MIN_TERMINATION_TIMEOUT * 1000 );
        DBGOUT;
        return;
    }

  if ( *kgs.pDBState == SERVER_WARM )
  {
      sql51k_auto_shutdown ();
  }

  if (( *kgs.pDBState == SERVER_COLD ) ||
      ( *kgs.pDBState == SERVER_STARTING ))
    {
    MSGCD  (( INFO_DB_STOPPED ));
    DBG1  (( MF__, "Database is being stopped" ));
    *kgs.pDBState = SERVER_STOP;
    UPDATE_SERVICE_OR_CONSOLE_STATE(*kgs.pDBState);
    }
  else if (( *kgs.pDBState == SERVER_WARM ) ||
           ( *kgs.pDBState == SERVER_STANDBY_EO00 ))
    {
    MSGALL (( WRN_DB_STOPPED_IN_WARM_MODE ));
    DBG1  ((MF__, "Database is being stopped in WARM mode"));
    *kgs.pDBState = SERVER_KILL;
    UPDATE_SERVICE_OR_CONSOLE_STATE(*kgs.pDBState);
    }

  if ( kgs.pfDump != NULL )
    {

    if (( pkss->fDump       == TRUE )                  ||
        ( *kgs.pDBState     == SERVER_SHUTDOWNREINIT ) ||
        ( *kgs.pDBState     == SERVER_SHUTDOWNKILL ))
      {
      *kgs.pfDump  = TRUE;
      }
    }

  if ( kgs.pfTraceFlag != NULL )
    *kgs.pfTraceFlag   = TRUE;

  //
  //  --- wake all user kernel threads
  //
  for ( pTmpUKTCtrl =  kgs.pFirstUKTCtrl;
        pTmpUKTCtrl <= kgs.pLastUKTCtrl;
        pTmpUKTCtrl++ )
    {
    if ( pTmpUKTCtrl->ThrdCtrlHeader.hevSem != (HEV) INVALID_HANDLE_VALUE )
      sql72k_wake_ukt( pTmpUKTCtrl );
    }

  /*
   *  If the trace flag or dump flag could not be set, or the
   *  tracewriter task is not active don't wake up the tracewriter.
   *  This can happen during the early start phase.
   */
  if (( kgs.pfDump         != NULL )            &&
      ( kgs.pfTraceFlag    != NULL )            &&
      ( kgs.pTW->TaskState != TSK_INITIALIZED_EO00 ))
    {
        if ( *kgs.pfDump )
        {
            Msg_RegistryDump();
        }

        sql50k_resume_tracewriter();

        if ( *kgs.pfDump == TRUE )
          vdump_rte();
    }

  // --- Reset of Crash-Semaphore in case of a normal shutdown
  // PTS 1115180

  if (!kgs.dumpDiagFiles)
  {

     RTEDiag_SetCrashSem (CRASH_SEM_RESET, kgs.szServerDB);
  }
  else /* PTS 1116365 */
  { 
     MSGD(( INFO_DIAG_HIST_FORCED ));
  }

  DBGOUT;
  return;
  }

/*------------------------------*/

static LONG sql81k_init_kernel_shared_segments ( VOID )
  {
  #undef  MF__
  #define MF__ MOD__"sql81k_init_kernel_shared_segments"
  LONG                   rc;
  HANDLE                 hShrdMem;
  ULONG                  ulNumOfSemaphores;
  ULONG                  ulAuthenticationFlags;

  DBGIN;

  SAPDBErr_MessageList      errList;
  RTEConf_Parameter::String valueString;

  if( RTEConf_ParameterAccess::Instance()->GetString( KERNELPARAM_AUTHENTICATION_ALLOW, valueString, errList ))
  {
      if ( 0 == strcmp("BASIC", (char*)valueString ))
        ulAuthenticationFlags = AFL_BASIC;
      else if ( 0 == strcmp("SCRAMMD5", (char*)valueString ))
        ulAuthenticationFlags = AFL_SCRAMMD5;
      else
        ulAuthenticationFlags = AFL_SCRAMMD5 | AFL_BASIC;
  }
  else
      ulAuthenticationFlags = AFL_SCRAMMD5 | AFL_BASIC;

  if ( RTEConf_ParameterAccess::Instance()->GetString( KERNELPARAM_AUTHENTICATION_DENY,  valueString, errList) )
  {
      if ( 0 == strcmp("BASIC", (char*)valueString ))
        ulAuthenticationFlags &= ~AFL_BASIC;
      else if ( 0 == strcmp("SCRAMMD5", (char*)valueString ))
        ulAuthenticationFlags &= ~AFL_SCRAMMD5;
  }


  //
  // --- create kernel shared section
  //
  kgs.ulKSSSize = sizeof (KERNEL_SHARED_SEG);
  rc = sql41c_create_shrd_mem ((PPVOID)&pkss, kgs.ulKSSSize,
                               SHM_KSS, kgs.szServerDB,
                               &kgs.WorldSA,
                               &hShrdMem);

  if (rc != NO_ERROR)
    return(rc);

  memset ( pkss, 0, kgs.ulKSSSize );
  pkss->kernelStateSize = sizeof(Kernel_SharedState);
  Kernel_ExtendedState::pPendingUpdateCounter   = &pkss->pendingUpdateCounter;
  Kernel_ExtendedState::pCompletedUpdateCounter = &pkss->completedUpdateCounter;
  Kernel_ExtendedState::pSharedState            = &pkss->kernelState;

  sp100_GetVersionID ( VersionIDType2_esp100, s100buildnumberEx, &pkss->RTEVersionID );

  SAPDB_strncpy_zero_terminated ( pkss->RTEVersion, RTE_GetRTEVersion(), sizeof(pkss->RTEVersion) );

  SAPDB_strncpy_zero_terminated ( pkss->KernelVersion, RTE_GetKernelVersion(), sizeof(pkss->KernelVersion) );

  if ( kgs.fRunningAsNTService == TRUE )
  {
       //
       // --- update the KERNEL-VERSION (KernelVersion) only!!!!
       //     The RTE-Version (Version) is needed for installation checks.
       //
       REG_ENTRY_REC       RegistryEntries[1];
       PATHNAME            szSubKey;
       strcpy ( szSubKey, kgs.szServiceName);
       strcat ( szSubKey, "\\"REG_SK_SERVICE_PARAM );
       RegistryEntries[0].pszValueName = REG_VN_KERNEL_VERSION;
       RegistryEntries[0].pValue       = pkss->KernelVersion;
       RegistryEntries[0].ulValueSize  = (ULONG)strlen(pkss->KernelVersion);
       RegistryEntries[0].ulValueType  = REG_SZ;

       sql50_reg_put_service_values ( NULL, szSubKey, 1, RegistryEntries );
   }

  pkss->fDump                                = false;
  pkss->gracefully                           = true;
  pkss->KernelPid                            = kgs.KernelPid;
  pkss->fIsGateway                           = kgs.fIsGateway;
  pkss->dBState                              = kgs.tmpDBState;
  kgs.pDBState                               = &(pkss->dBState);

  //
  // --- create kernel shared communication flag section
  //
  ulNumOfSemaphores = kgs.ulNumOfUKTs;
  kgs.ulFCSSize     = sizeof (FLAG_COMM_SEG)                 +
                      (kgs.ulNumOfCommFlags * sizeof(ULONG)) +
                      (ulNumOfSemaphores    * sizeof(ULONG)) +
                      (ulNumOfSemaphores    * sizeof(IPC_BASE_OBJ_NAME));

  rc = sql41c_create_shrd_mem ((PPVOID)&pfcs, kgs.ulFCSSize,
                               SHM_FCS, kgs.szServerDB,
                               &kgs.WorldSA,
                               &hShrdMem);

  if (rc != NO_ERROR)
    return( rc );

  memset ( pfcs, 0, kgs.ulFCSSize );

  pfcs->ulPacketSize      = XPARAM(ulPacketSize);
  pfcs->ulMinReplySize    = XPARAM(ulMinReplySize);

  pfcs->ulNumOfCommFlags  = kgs.ulNumOfCommFlags;
  pfcs->ulCommFlagOffset  = sizeof (FLAG_COMM_SEG);
  pfcs->ulNumOfSemaphores = ulNumOfSemaphores;
  pfcs->ulSemaFlagOffset  = pfcs->ulCommFlagOffset +
                            (kgs.ulNumOfCommFlags * sizeof(ULONG));
  pfcs->ulSemaNameOffset  = pfcs->ulSemaFlagOffset +
                            (ulNumOfSemaphores * sizeof(ULONG));

  strcpy ( pfcs->szSemPath, SEM_UKT );
  pfcs->fSemDemandOpen = TRUE;

  pfcs->AuthenticationAllow[0] = '\0';

  if ( (ulAuthenticationFlags & ( AFL_SCRAMMD5 | AFL_BASIC )) == ( AFL_SCRAMMD5 | AFL_BASIC ) )
      strcat( pfcs->AuthenticationAllow, "BASIC,SCRAMMD5" );
  else if ( ulAuthenticationFlags & AFL_BASIC )
      strcat( pfcs->AuthenticationAllow, "BASIC" );
  else if ( ulAuthenticationFlags & AFL_SCRAMMD5 )
      strcat( pfcs->AuthenticationAllow, "SCRAMMD5" );

  kgs.pulCommFlags = (PULONG)((PCHAR)pfcs + pfcs->ulCommFlagOffset);
  kgs.pulSemaFlags = (PULONG)((PCHAR)pfcs + pfcs->ulSemaFlagOffset);
  kgs.pSemaNames   = (PIPC_BASE_OBJ_NAME)((PCHAR)pfcs + pfcs->ulSemaNameOffset);

  DBGOUT;
  return ( rc );
  }


/*------------------------------*/

 static bool sql81k_continue_search_exception( VOID )
   {
   #undef  MF__
   #define MF__ MOD__"sql81k_continue_search_exception"
   LONG             rc = NO_ERROR;
   PATHNAME         szDebuggerEntry;
   PATHNAME         szDBDebuggerEntry;
   REG_ENTRY_REC    RegistryEntries[1];
   PATHNAME         szSubKey;


   DBGIN;

   if ( sql02_get_platform_id () == VER_PLATFORM_WIN32_NT )
     {
     RegistryEntries[0].pszValueName = REG_VN_AEDEBUG;
     RegistryEntries[0].pValue       = szDebuggerEntry;
     RegistryEntries[0].ulValueSize  = sizeof(szDebuggerEntry);
     RegistryEntries[0].ulValueType  = REG_SZ;

     rc = sql50_reg_get_values ( NULL, HKEY_LOCAL_MACHINE, REG_SK_AEDEBUG, 1, RegistryEntries );

     if (rc != NO_ERROR)
       {
       DBGOUT;
       return false;
       }

     strupr ( szDebuggerEntry );

     RegistryEntries[0].pszValueName = REG_VN_AEDEBUG;
     RegistryEntries[0].pValue       = szDBDebuggerEntry;
     RegistryEntries[0].ulValueSize  = sizeof(szDBDebuggerEntry);
     RegistryEntries[0].ulValueType  = REG_SZ;

     strcpy ( szSubKey, kgs.szServiceName);
     strcat ( szSubKey, "\\"REG_SK_SERVICE_PARAM );

     rc = sql50_reg_get_service_values ( NULL, szSubKey, 1, RegistryEntries );

     if ( rc == NO_ERROR )
       {
       strupr ( szDBDebuggerEntry );

       if ( strstr ( szDebuggerEntry, szDBDebuggerEntry ) != NULL )
         {
         DBGOUT;
         return true;
         }
       }

     if ( strstr ( szDebuggerEntry, DR_WATSON_EXENAME ) != NULL )
       {
       DBGOUT;
       return true;
       }
     }


   DBGOUT;
   return false;
   }

 /*------------------------------*/

 static BOOL sql81k_get_debug_break_entry( PSZ pszServerDB )
   {
   #undef  MF__
   #define MF__ MOD__"sql81k_get_debug_break_entry"
   LONG             rc = NO_ERROR;
   PATHNAME         szSubKey;
   REG_ENTRY_REC    RegistryEntries[1];

   DBGIN;

   strcpy ( szSubKey, kgs.szServiceName);

   RegistryEntries[0].pszValueName = REG_VN_SERVICE_TYPE;
   RegistryEntries[0].pValue       = &kgs.ulServiceType;
   RegistryEntries[0].ulValueSize  = sizeof(kgs.ulServiceType);
   RegistryEntries[0].ulValueType  = REG_DWORD;

   rc = sql50_reg_get_service_values ( NULL, szSubKey, 1, RegistryEntries );

   if (rc != NO_ERROR)
     {
     DBGOUT;
     return( FALSE );
     }

   strcpy ( szSubKey, kgs.szServiceName);
   strcat ( szSubKey, "\\"REG_SK_SERVICE_PARAM );

   RegistryEntries[0].pszValueName = REG_VN_DEBUG_BREAK;
   RegistryEntries[0].pValue       = &kgs.ulStaticDebugBreakPos;
   RegistryEntries[0].ulValueSize  = sizeof(kgs.ulStaticDebugBreakPos);
   RegistryEntries[0].ulValueType  = REG_DWORD;

   rc = sql50_reg_get_service_values ( NULL, szSubKey, 1, RegistryEntries );

   if (rc != NO_ERROR)
     {
     DBGOUT;
     return( FALSE );
     }

   //
   // --- check service type
   //
   if (( !(kgs.ulServiceType & SERVICE_INTERACTIVE_PROCESS) ) && (kgs.ulStaticDebugBreakPos != 0))
     {
     MSGALL (( WRN_NO_INTERACT_WITH_DESCTOP ));
     DBGOUT;
     return( FALSE );
     }

   DBGOUT;
   return( TRUE );
   }

 /*------------------------------*/

 static bool sql81k_always_check_conn( )
   {
   #undef  MF__
   #define MF__ MOD__"sql81k_always_check_conn"
   LONG             rc = NO_ERROR;
   PATHNAME         szSubKey;
   REG_ENTRY_REC    RegistryEntries[1];
   DWORD            AlwaysCheckConn = 0;

   DBGIN;

   strcpy ( szSubKey, kgs.szServiceName);

   RegistryEntries[0].pszValueName = REG_VN_ALWAYS_CHECK_CONN;
   RegistryEntries[0].pValue       = &AlwaysCheckConn;
   RegistryEntries[0].ulValueSize  = sizeof(AlwaysCheckConn);
   RegistryEntries[0].ulValueType  = REG_DWORD;

   rc = sql50_reg_get_service_values ( NULL, szSubKey, 1, RegistryEntries );

   if (rc != NO_ERROR)
     {
     DBGOUT;
     return( false );
     }


   DBGOUT;
   return( (bool)(AlwaysCheckConn != 0) );
   }

 /*------------------------------*/

 static LONG  sql81k_create_SAs ( )
   {
   #undef  MF__
   #define MF__ MOD__"sql81k_create_SAs"
   APIRET                 rc = NO_ERROR;
   ACCESS_RIGHTS_REC      Access;

   DBGIN;

   kgs.AdminSA.nLength         = sizeof(kgs.AdminSA);
   kgs.AdminSA.bInheritHandle  = FALSE;

   memset ( &Access, 0, sizeof (Access) );  // - this must be done first!
   Access.User.ulAccess           = SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;
   Access.Creator.ulAccess        = SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;
   Access.Admin.ulAccess          = SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;
   Access.System.ulAccess         = SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;
   Access.SAPDBOp.ulAccess        = SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;
   Access.DomSAPDBOp.ulAccess     = SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;

   rc = sql49c_alloc_and_init_SD( &Access, &kgs.AdminSA.lpSecurityDescriptor );

   if ( rc != NO_ERROR )
     {
     DBGOUT;
     DBG1  (( MF__, "Could not get admin security descriptor, rc = %d ", rc ));
     return( rc );
     }

   kgs.WorldSA.nLength         = sizeof(kgs.WorldSA);
   kgs.WorldSA.bInheritHandle  = FALSE;

   memset ( &Access, 0, sizeof (Access) );   // - this must be done first!
   Access.World.ulAccess       = SPECIFIC_RIGHTS_ALL |
                                 STANDARD_RIGHTS_ALL;

   rc = sql49c_alloc_and_init_SD( &Access, &kgs.WorldSA.lpSecurityDescriptor );

   if ( rc != NO_ERROR )
     {
     DBG1  (( MF__, "Could not get world security descriptor, rc = %d ", rc ));
     DBGOUT;
     return( rc );
     }

   kgs.LockSA.nLength          = sizeof(kgs.LockSA);
   kgs.LockSA.bInheritHandle   = FALSE;

   memset ( &Access, 0, sizeof (Access) );  // - this must be done first!
   rc = sql49c_alloc_and_init_SD( &Access, &kgs.LockSA.lpSecurityDescriptor );

   if ( rc != NO_ERROR )
     {
     DBG1  (( MF__, "Could not get lock security descriptor, rc = %d ", rc ));
     DBGOUT;
     return( rc );
     }



   kgs.FileSA.nLength              = sizeof(SECURITY_ATTRIBUTES);
   kgs.FileSA.bInheritHandle       = FALSE;
   kgs.FileSA.lpSecurityDescriptor = NULL;

   memset ( &Access, 0, sizeof (Access) );  // - this must be done first!
   Access.fDescOwnerIsAdmin       = FALSE;
   Access.fAddDefaultDACL         = FALSE;
   Access.User.ulAccess           = SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;
   Access.Creator.ulAccess        = SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;
   Access.Admin.ulAccess          = SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;
   Access.System.ulAccess         = SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;
   Access.SAPDBOp.ulAccess        = SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;
   Access.DomSAPDBOp.ulAccess     = SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;
   Access.World.ulAccess          = 0;
   Access.BackupOp.ulAccess       = GENERIC_READ;

   rc = sql49c_alloc_and_init_SD(&Access, &kgs.FileSA.lpSecurityDescriptor);

   if ( rc != NO_ERROR )
     {
     DBG1  (( MF__, "Could not get diagfile security descriptor, rc = %d ", rc ));
     DBGOUT;
     return( rc );
     }

   DBGOUT;
   return ( rc );
   }

/*------------------------------*/

static LONG sql81k_init_thread_local_storage ( VOID )
  {
  #undef  MF__
  #define MF__ MOD__"sql81k_init_thread_local_storage"
  APIRET  rc = NO_ERROR;

  DBGPAS;

  // --- initlialize "Thread Local Storage" indexes
  if ((kgs.TlsUKTCtrlIdx = TlsAlloc()) == (DWORD)-1)
    {
    rc = GetLastError();
    MSGALL (( ERR_CANT_ALLOC_TLS, rc ));
    DBG1  (( MF__, "Cannot allocate thread local storage, rc = %u", rc ));
    }

  return ( rc );
  }

/*------------------------------*/

static LONG sql81_InitEventing()
{
    LONG rc = NO_ERROR;
    tsp00_Int4 spaceNeeded = 0;

    if ( XPARAM(szEventFile)[0] != '\0' && XPARAM(ulEventSize) > 0 )
    {
        MSGD (( INFO_EVENT_FILE, &XPARAM(szEventFile)[0], XPARAM(ulEventSize) ));
        rc = sql60_open_event_file( XPARAM(szEventFile),
                                    XPARAM(ulEventSize),
                                    &kgs.FileSA );
        if (rc != NO_ERROR)
        {
            MSGD (( ERR_CANT_OPEN_FILE, &XPARAM(szEventFile)[0], rc ));
            return( rc );
        }
    }
    else
    {
        MSGD (( INFO_EVENT_NO_FILE, &XPARAM(szEventFile)[0], XPARAM(ulEventSize) ));
    }

    eo67InitializeEventInfo( );

    kgs.Eventing = NULL;
    spaceNeeded = eo67SpaceNeeded( XPARAM(ulMaxEventTasks), XPARAM(ulMaxEvents) );
    if ( spaceNeeded > 0 )
    {
        MSGD (( INFO_EVENT_RINGBUFFER, XPARAM(ulMaxEvents), XPARAM(ulMaxEventTasks) ));
        if ( (rc = ALLOC_MEM (&kgs.Eventing, spaceNeeded )) != NO_ERROR)
        {
            DBGOUT;
            return ( rc );
        }

        memset(kgs.Eventing, 0, spaceNeeded);

        if ( !eo67Initialize( XPARAM(ulMaxEventTasks), XPARAM(ulMaxEvents) ) )
        {
            MSGD (( ERR_EVENTING_INITIALIZE ));
            DBGOUT;
            return (-1);
        }

    }
    else
    {
        MSGD (( INFO_EVENT_NO_RINGBUFFER, XPARAM(ulMaxEventTasks), XPARAM(ulMaxEvents) ));
    }

    return( NO_ERROR );
}

/*------------------------------*/

static LONG sql81k_open_diag_files(PSZ pszServerDB)
{
  ULONG           ulDiagSize;
  tsp00_C256c     szDiagSize;
  tsp00_ErrText   errtext;
  LONG            rc            = NO_ERROR;
  tsp00_DbNamec   DBNamec;
  tsp00_C256c     DiagSizec;

  DBNamec.rawAssign(pszServerDB);
  DiagSizec.rawAssign(PAN_DIAG_SIZE);

  if(!RTEConf_ParamaterAccessGetParam(DBNamec, DiagSizec,szDiagSize, errtext))
    return -1;
  ulDiagSize = atoi(szDiagSize);

  rc = sql60_open_diag_file ( SAPDB_DIAG_FILE_NAME, ulDiagSize, &kgs.FileSA );
  if(0 != rc)
    return rc;

  rc = sql60_open_error_diag_file( SAPDB_ERROR_FILE_NAME, &kgs.FileSA );

  return rc;
}

/*------------------------------*/

#if !defined (_WIN64)
 /*!
   @brief    sql81k_handle_RTC_failure

             The runtime checking handler function is called when NOT running under 
             debugger control else it's ignored. 

   @param    errorType   [in] - The type of error as reported through _RTC_SetErrorType.
                                Note that this function don't use this parameter.
   @param    sourceLine  [in] - The source file name where the error occurred.
   @param    moduleName  [in] - The module where the error occurred.

   @param    formatString[in] - The printf-style format string for the variable parameters.
                                We are using our own format, only needed for 'va_start'.
   @param    ...         [in] - The variable argument list.  There are only two values
                                passed here.  The first is the RTC Error number.
                                  1 = /RTCc
                                  2 = /RTCs
                                  3 = /RTCu
                                The second is the additional description of the error.
                                This is only important for /RTCs and /RTCu as those show
                                the variable where the problem happened.

   @return      TRUE  - Call a 'DebugBreak' after this function returns.
                FALSE - Continue execution.

*/

#if defined(_M_IX86)
  // Turn off run time checks for this function so don't go recursive on itsself.
# pragma runtime_checks("", off)
#endif

/*------------------------------*/

int sql81k_handle_RTC_failure ( int          errorType,
                                const char * fileName,
                                int          sourceLine,
                                const char * moduleName,
                                const char * formatString,
                                ... )
{
    char const *      errorDesc;
    char              displayStr[1024];
    char*             errorVariableDesc;
    _RTC_ErrorNumber  rtcErrNum;
    va_list           varList;

    // We need a default string for fileName and moduleName.
    if ( 0 == fileName )   fileName   = "Unknown";
    if ( 0 == moduleName ) moduleName = "Unknown";

    // Load the arguments
    va_start ( varList , formatString );
    rtcErrNum         = va_arg( varList, _RTC_ErrorNumber ); // number of the error that occured.
    errorVariableDesc = va_arg( varList, char * );          // additional description of the error.
    va_end ( varList );

    // Get the error description text based off the error number.
    errorDesc = _RTC_GetErrDesc ( rtcErrNum );

    if ( 1 != rtcErrNum ) // If it's any error other than /RTCc, we can include a variable information!
        _stprintf ( displayStr, "%.256s, %.128s In line %d of file '%.256s' (%.256s)",
                    errorDesc,errorVariableDesc,sourceLine,fileName,moduleName );
    else
        _stprintf ( displayStr, "%.256s In line %d of file '%.256s'  (%.256s)",
                    errorDesc,sourceLine,fileName,moduleName );

    MSGD (( 19999, 1, "RTCERROR", displayStr ));

    return FALSE;
}
#if defined(_M_IX86)
# pragma runtime_checks("", restore)
#endif

#endif

/*------------------------------*/

#if defined(_M_IX86)
 void sql81k_handle_purecall( void )
 {
    MSGD (( 19999, 1, "RTCERROR", "A pure virtual function is called"  ));
    ABORT();
 }
#endif

/*
 * =============================== END ========================================
 */
