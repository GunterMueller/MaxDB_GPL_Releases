/*!---------------------------------------------------------------------
  @file           ven81.cpp
  @author         JoergM
  @brief          KERNEL main
  @see            example.html ...

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


//
//   FreeBSD portions added by Kai Mosebach,
//   For more informations see : http://www.komadev.de/sapdb
//

\endif
---------------------------------------------------------------------*/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

extern "C" { /* Needed for HPUX, otherwise getcontext is undefined reference */
#include <ucontext.h>       /* for getcontext() in context check */
};
#ifdef FREEBSD
#include <sys/signal.h>
#endif
#include "RunTime/RTE_Types.h"
#include "heo07.h"
#include "gen60.h"
#include "heo00.h"
#include "geo50_0.h"
#include "gen00.h"
#include "geo007_1.h"
#include "hen40.h" /* sqlerrs */
#include "gen41.h"
#include "gen43.h"
#include "gen45.h"
#include "hen50.h" /* vabort */
#include "gen52.h"
#include "gen54.h"
#include "geo67.h"
#include "gen70.h"
#include "gen71.h"
#include "gen72.h"
#include "gen74.h"
#include "gen75.h"
#include "gen81.h"
#include "gen83.h"
#include "gen84.h"
#include "gen85.h"
#include "gen88.h"
#include "hsp100.h"
#include "geo57.h"
#include "heo15.h"
#include "RunTime/Tasking/RTETask_WorkerPool.hpp"
#include "RunTime/Diagnose/RTEDiag_ServerCrashHist.h"
#include "RunTime/System/RTESys_Time.h"
#include "Trace/Trace_InfoPage.h"
#include "RunTime/RTE_Speed.h"
#include <sys/wait.h>
#include "RunTime/MemoryManagement/RTEMem_PageAllocator.h"
#include "gip00.h"
#include "liveCache/MemoryManagement/LVCMem_Allocator.h"
#include <sys/resource.h>   /* for getrlimit() */
#if defined(OSF1)
/* otherwise ds0124 complains... */
extern "C" int _Esetrlimit(int, const struct rlimit *);
#endif
#include <unistd.h>         /* for sysconf()   */
#include "RunTime/RTE_DiagPipe.h"
#include "RunTime/Threading/RTEThread_Console.h"
#include "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.h"
#include "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp"
#include "RunTime/Tasking/RTETask_LegacyTaskCtrl.h"
#include "heo670.h"
#include "SAPDBCommon/SAPDB_Names.h"
#include "RunTime/System/RTESys_SystemInfo.h"
#include "RunTime/RTE_UNIXAuthenticate.h"
#include "RunTime/RTE_saveUNIXcalls.h"
#include "SAPDBCommon/SAPDB_sprintf.h"
#include "gen60_1.h"
#include "heo69.h"
#include "heo92.h"
#include "RunTime/RTE_SpawnRestartShutdown.h"
#include "Messages/Msg_OutputKnlDiag.h"
#include "RunTime/RTE_Version.h"
#include "RunTime/Synchronisation/RTESync_SpinlockRegister.hpp"
#include "RunTime/Synchronisation/RTESync_SpinlockPoolRegister.hpp"
#include "RunTime/Diagnose/RTEDiag_SymbolResolutionLock.h"
#include "RunTime/System/RTESys_MicroTime.h"

#include "RunTime/Threading/RTEThread_IORedirection.hpp"

extern "C" void en88CreateAndPrepareTaskStacks();
extern "C" void en60_ReopenKnldiagFile(RTE_XPARAM_REC *XparamParameters);

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
#if defined(LINUX)
#define HAS_CLONE_PROCESSES_INSTEAD_OF_THREADS
#endif /* OSF1 */

#undef MOD__
#define MOD__ "ven81.c:"

#define TRACE_WRITER_TIMEOUT_EN81 60 /* seconds */

#define SIG_FORK_AND_EXIT SIGINT
#define SIG_STOP_DATABASE SIGTERM
#define EXIT_CODE_HUP_SIGNAL 42
#define EXIT_CODE_PANIC_SIGNAL 47
#define KERNEL_TERMINATED_BY_HUP_SIGNAL (-1)

#define DEAD_DETECTION_TIMEOUT_EN81 1 /* seconds */
#define MAX_UKT_START_WAIT_TIME     60  /* sleep time */

#define SAPDB_Debug_Env "SAPDB_Debug_Env" /* file used for dynamic debugging */

#ifdef FREEBSD
#define RLIMIT_AS RLIMIT_VMEM
#endif

#if defined(AIX)
/*
  Only this platform has a version of sigaltstack, that works fine in
  combination with stack overflow handling...
 */
#  define USE_SIGALTSTACK
#  define WANTED_SIGSTKSZ_EN81 (MINSIGSTKSZ+(64*1024))
#endif /* AIX */

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

typedef struct _en81_WorkerInfo
{
    const char       *name;
    void             (*work)(void);
    teo07_Thread     thread;
    teo07_ThreadId   tid;
    volatile int     running;
    int              loopForever;
    int              triggerSemaphore;
    int              doneSemaphore;
} ten81_WorkerInfo;

/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/

#if defined(OSF1)
extern unsigned long				    _ftext ;
extern unsigned long			        _etext ;
#elif defined(AIX)
extern  int                             _etext ;
#elif defined(SVR4) || defined(SOLARIS)
extern  int                             etext ;
extern  int                             _start ;
#elif defined(HPUX)
extern  int                             _etext ;
extern  int                             __text_start ;
#else
extern  int                             etext ;
#endif

extern  volatile int                    e60_dbgdbglvl ;
extern  volatile tsp00_Bool             en50NoCoreDumpWanted;

/* exported data */

tsp00_DbNamec                    en81_dbname ;
int		  				         en81_usable_fds = 0;
long                             en81_MemPageSize = 0L;
static int en81_OtherArgc = 0;
static char **en81_OtherArgv = 0;
static teo07_ThreadSemaphore 			en81_TimerSleepCondition = NULL;

volatile        int                     en81_neverStop = 1;
	/* PTS 1112857 */
volatile        int                     en81_KernelGotCrashSignal = 0;
/*
 * Workerthreads for Signalhandling
 */
static ten81_WorkerInfo                 en81_CrashWorker =
    { "CRASH", (void (*)())0, (teo07_Thread)0, (teo07_ThreadId)0, 0, 0, -1, -1 };
static ten81_WorkerInfo                 en81_StopWorker  =
    { "STOP", (void (*)())0, (teo07_Thread)0, (teo07_ThreadId)0, 0, 0, -1, -1 };
static ten81_WorkerInfo                 en81_UktDumpWorker  =
    { "DUMP", (void (*)())0, (teo07_Thread)0, (teo07_ThreadId)0, 0, 1, -1, -1 };

/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/

/*
 * It is a problem, if we crash during a 'malloc/free' after the mutex for malloc/free
 * was already acquired...
 * The localtime_r Routine is internally using a 'malloc/free' combination,
 * which is in such a case deadlocked... So we use an indication to prevent the message
 * output routine from generating a timestamp in that case... Better solution would be
 * to 'force' the internal mutex to be released, but how?
 */
static teo07_ThreadSemaphore            en81_TerminateCondition = NULL;
static teo07_ThreadSemaphore            en81_TerminateEndCondition = NULL;
static tsp00_Pathc                      en81_pathToExecutable;
static SAPDB_UInt4                      en81_StartupOptions = 0;

#ifdef USE_SIGALTSTACK
/*
   define alternate stack on heap ... (16*1024) is enough space for system page alignment
   on all known systems
 */
#define MAX_SYSTEM_PAGE_SIZE_OVER_ALL_PLATFORMS (16*1024)
static char en81_AlternateStackSpace[WANTED_SIGSTKSZ_EN81+MAX_SYSTEM_PAGE_SIZE_OVER_ALL_PLATFORMS] = { 0 };
#endif /* USE_SIGALTSTACK */

/*===========================================================================*
 *  LOCAL FUNCTIONS (PROTOTYPES)                                             *
 *===========================================================================*/

extern  void en81Abort();
static  void                            en81_resume ( struct TASK_TYPE * tcb );
static  int                             en81_Startup ( SAPDB_Char * currentVersion , char *envp[] );
static  int                             en81_CreateRteProcs ( /* void */ );
static  int                             en81_CreateUkts ( /* void */ );
static  int                             en81_CreateKtSemaphores ( /* void */ );
static  void                            en81_WaitForTermination ( /* void */ );
static  void                            en81_CrashAction(/* void */);
static  void                            en81_StopAction(/* void */);
static  void                            en81_UktDumpAction(/* void */);
static  int                             en81_GetDebugEnv(const char *Variable,
                                                         char *ResultBuffer,
                                                         const size_t MaxLength);
static const char *                     en81_SigName(int sigNumber);
static const char *                     en81_SigCode(int sigCode);
static int                              en81_ValidateNumberOfWantedDescriptors();
/* PTS 1105262 / PTS 1105263 */
extern "C" {
    typedef void (*signalHandler)(int, siginfo_t *,void *);
};

static int en81_SetSigAction(int sig, signalHandler handler);
static int en81_SetDefaultAction(int sig);
static int en81_SetIgnoreAction(int sig);
/*
 * If threading is used, it is still possible to support profiling snapshot output
 * by calling monitor() interface (XPG2, SVID2, SVID3). But for now it is
 * only a nice to have feature. LOW PRIORITY
 */

/*
 * If threading is used it is still a nice to have feature to produce a
 * internal task specific core dump, but since signals are not delivered
 * to a specific UKT but to THE process, it is needed to modify the control
 * interface to add this feature. EVEN LOWER PRIORITY
 */
extern "C" void en81_ForkAndExit( int sig, siginfo_t *sigInfo, void *context );
extern "C" void en81_StopDatabase( int sig, siginfo_t *sigInfo, void *context );
extern "C" void en81_CrashSignalHandler( int sig, siginfo_t *sigInfo, void *context );

static  void                            en81_InitFirstDescriptors( /* void */ );
static  void                            en81_SetSignalHandler( tsp00_Bool suppressCore );
static  int                             en81_FillServerDbName( char *dbname, const char *arg0 );
/*
 * Wrapper for catching C++ exceptions not handled
 */
extern "C" int                          en80RunKernel(struct ten50_KT_Control *pMainThread);
extern "C" void Msg_RegistryDump();
void                            en81_LastWords(int runKernelExitCode, int isKilled);
static  int                             en81_CheckTracewriterRunning(/* void */);
static  int                             en81_InitEventing( /* void */ );
static  void                            en81_SuppressCoreDump();
static  void                            en81_AllowCoreDump(tsp00_Bool *pSuppress);
static  int                             en81_InitializePageAllocator( RTE_XPARAM_REC *XParam );

/*
 * PTS 1116686
 */
static void en81WriteResourceLimitToString(SAPDB_UInt8 size,SAPDB_Char *value,SAPDB_Bool inMB,SAPDB_UInt8 unlimited);
static void en81OutputResourceLimits(RTE_XPARAM_REC *XparamParameters  , char *envp[]);

static void en81_CheckCoroutineUsage(RTE_XPARAM_REC *XparamParameters );

/*===========================================================================*
 *  GLOBAL FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

/*
  @brief Worker thread to replace watchdogs update task
  Used for SDB_WITH_PROFILER only to do the time stamp update
  @return given argument
 */
extern "C" void* en81_ClockMain(void *arg)
{
    while(KGS)
    {
        KGS->current_time = time((time_t *)0);
        sleep(1);
    }
    return arg;
}
/*
  @brief create a clock worker thread
  Its task is to update the global time stamp every second. This is needed for profiling only, since the watchdog thread
  does it itself otherwise

  Used for SDB_WITH_PROFILER only to do the time stamp update

  @return 0 if running, 1 if creation failed
 */
static int en81_CreateClockWorker()
{
    teo07_ThreadErr err;
	tsp00_ErrTextc  errtext;
    teo07_Thread     clockThread;

    /*
     *  Create WORKER thread
     */
    sqlbeginthread( KT_WORKER_STACK_SIZE,
                    en81_ClockMain,
                    0,
					THR_CREATE_SUSPENDED_EO07,
                    &clockThread,
                    errtext,
                    &err );
    if ( err == THR_OK_EO07 )
    {
        KGS->clockTid = sqlthreadid(clockThread);
    }
    else
    {
        MSGCD (( ERR_CREATE_THREAD , "CLOCK", errtext.asCharp() )) ;
        return ( 1 );
    }

    sqlresumethread( clockThread, errtext, &err );
    if ( err != THR_OK_EO07 )
    {
        MSGCD (( ERR_RESUME_THREAD, "CLOCK", errtext.asCharp() ));
        return ( 1 );
    }

    MSGD (( INFO_PROCESS_STARTED, "CLOCK" ));

    return ( 0 );
}

/*
 * en81_kernel_main
 *
 * The main routine (directly called by main(argc,argv)). Argument are
 * orginal command line arguments.
 *
 * argv[0] Programname
 * argv[1] "kernel"
 * argv[2] "-online" optional autorestart option (start dbmstart after reaching admin state)
 * argv[3] "-lowprio" optional starting in low priority mode
 * argv[4] "-Dl" optional debugging level (sets global variable e60_dbgdbglvl)
 *         l is a single digit [0-9].
 * additional arguments are currently ignored
 */
externC int en81_kernel_main ( int argc , char *argv[] , char *envp[] )
{
#undef  MF__
#define MF__ MOD__"en81_kernel_main"
    pid_t                       runKernelPid;
    int                         runKernelExitCode;
    int                         rc = 0;
    struct ten50_KT_Control     mainThread;
    teo07_ThreadErr             err;
	tsp00_ErrTextc              errtext;
    DBGIN;

    /* Save path to executable */
    /* PTS 1110474 */
    if ( argv[0][0] == '/' )
    {
        if ( strlen(argv[0]) > sizeof(tsp00_Path) )
        {
            SAPDB_memset(en81_pathToExecutable, 0, sizeof(tsp00_Pathc));
        }
        else
        {
            strcpy(en81_pathToExecutable, argv[0]);
        }
    }
    else
    {
        if ( getcwd((char *)en81_pathToExecutable, sizeof(tsp00_Path)) == 0 )
        {
            SAPDB_memset((char *)en81_pathToExecutable, 0, sizeof(tsp00_Pathc));
        }
        else
        {
            if ( (strlen((char *)en81_pathToExecutable) + strlen(argv[0]) + 2) > sizeof(tsp00_Pathc) )
            {
                SAPDB_memset((char *)en81_pathToExecutable, 0, sizeof(tsp00_Pathc));
            }
            else
            {
                strcat((char *)en81_pathToExecutable, "/");
                strcat((char *)en81_pathToExecutable, argv[0]);
            }
        }
    }

    /*
     * Become session leader [Create own processgroup (prevents against kill(0,SIGKILL))]
     */
    if ( RTE_save_getsid((pid_t)0) != RTE_save_getpid() )
    {
        setsid ();
    }

    /*
     * If OS should not support this call uses a system specific define
     * to get the Memory PageSize in bytes defined normaly as PAGESIZE or PAGSIZ...
     */
    en81_MemPageSize = sysconf(_SC_PAGESIZE);

    /*
     * Start in debugger if wanted and possible...
     */
    if ( en81_GetDebugEnv("SAPDB_DEBUG_KERNEL_MAIN", NULL, 0) )
	{
        (void)en81CallDebugger();
    }

    /*
     *  Verify the presence of all required spool directories,
     *  and try to create the missing ones.
     */
    if ( sql41_check_spool_dirs () != 0 )
    {
		MSGCD (( ERR_SPOOL_DIR_MISSING ));
		return ( 2 );
    }

    if ( strlen(argv[1]) >= sizeof(tsp00_DbName) )
    {
	    MSGCD (( ERR_SERVERDB_TOO_LONG, sizeof(tsp00_DbName) ));
        return 1;
    }

    { /* PTS 1125559 */
        int iChar;
        SAPDB_memset(&en81_dbname[0], 0, sizeof(en81_dbname));
        for ( iChar = 0; argv[1][iChar] != 0 && iChar < sizeof(tsp00_DbName); iChar++ )
        {
            en81_dbname[iChar] = toupper(argv[1][iChar]);
        }
    }

    if ( en41SetPPIDInFile( argv[1], (tsp00_TaskId)RTE_save_getpid() ) != 0 )
    {
        return 1;
    }

    /* PTS 1110621 */
    if ( en41SetDbSpeedInFile( en81_dbname, RTE_GetSpeed ()) != 0 )
    {
        return 1;
    }

    SAPDB_memset(&mainThread, 0, sizeof(struct ten50_KT_Control));

    sqlinit_multithreading( & mainThread.thread, errtext, &err);
    if ( err != THR_OK_EO07 )
    {
	MSGCD (( ERR_INIT_MULTITHREADING, &errtext[0] )) ;
	return 3;
    }

    // this thread redirects any output written to stdout and stderr into message 
    // lists written to the KnlMsg file
    RTEThread_IORedirection pipeReaderThread;

    Msg_IOutput::GetDiagnosticOutput();  // to enforce singleton initialization before starting other threads
    Msg_List messageList;
    if ( RTEThread_Thread::NoError !=  pipeReaderThread.Create( 0, messageList) )
    {
        Msg_List errList;
        Msg_IOutput::GetDiagnosticOutput().Output( messageList,errList );
    }

    mainThread.tid = sqlthreadid( mainThread.thread );
    mainThread.start_time = time ( (time_t*)0 );

	/*
	 *  Create the Thread Termination Condition Variable
	 * used by threads cleanup handler to notify COORDINATOR
     */
	if ( rc == 0 )
	{
        tsp00_ErrTextc dummy;
		sqlcreatesem(&en81_TerminateCondition, 0, dummy, &err );
		sqlcreatesem(&en81_TerminateEndCondition, 0, dummy, &err );
	}

    /*
     *  Read param file, open diag, and initialize kernel global section.
     *  Do this in a subroutine because it needs lots of stack.
     */
    if ( rc == 0 )
	{
		rc = en81_Startup ( RTE_GetRTEVersion()  , envp );
	}

    if ( rc != 0 )
	{
		return ( rc );
	}

#if defined(OSF1)
	if ( XPARAM(fUseAsyncIO) )
    {
	  if ( e54a_install_aio_signal_handler () )
      {
		MSGD (( INFO_UKT_ASYNCIO_SIGNAL ));
      }
	  else
      {
		MSGD (( ERR_UKT_ASYNCIO_SIGNAL, sqlerrs() ));
      }
    }
#endif

    /*
     *  Create Semaphores
     */
    rc = en81_CreateKtSemaphores ();

    /*
     *  Create Big Communication Segments
     */
    if ( rc == 0 )
    {
		rc = e74_create_big_comsegs ();
    }

    KGS->ulStartupOptions = en81_StartupOptions;

    if ( rc == 0 )
    {
#ifdef SDB_WITH_PROFILER
        if ( rc == 0 )
        {
            runKernelExitCode = en80RunKernel( &mainThread ); /* return code ignored for profiler... */
        }
        else
        {
            return rc;
        }
#else
        int waitpidErrno = 0;

        runKernelPid = fork();
        switch ( runKernelPid )
        {
        case 0:
            en81_SetSignalHandler( XPARAM(suppressCore) );
		    /* on OSF1 the kernel with overwritten operator new crashes because
             * in the startup phase the LIBC seems to allocate memory from
             * the not overwritten operator new but deletes it with overwritten
             * operator delete. Stopping with _exit avoids this problem
             */
#if defined(LINUX)
            RTESys_LinuxTimerLeapCheck( 0 == SAPDB_strcasecmp(XPARAM(szHiResTimerType).asCharp(), "GETTIMEOFDAY") );
#endif
            e52_init_time (); /* second initialization to become independend of watchdog process */
#if defined(HAS_CLONE_PROCESSES_INSTEAD_OF_THREADS)
            exit(en80RunKernel( &mainThread ));
#else
            _exit(en80RunKernel( &mainThread ));
#endif
            break;
        case -1:
            /* Fork failed */
		    MSGCD (( ERR_INIT_MULTITHREADING, "forking runKernel failed" ));
            return ( 1 );
        default:
            RTE_CloseDiagPipe();
            /* This is the watchdog that waits for kernel termination */
            break;
        }

        rc = 0;

		/* PTS 1113489
		  Now we wait until kernel terminated or SERVER_KILL state was established
		  for more than MAX_DEADCHECK_SLEEP_SECONDS ...
		  This will prevent deadlock situations where stack backtrace never ends or
		  signal handler do not return due to mixed up mutex...
		 */
#define MAX_DEADCHECK_SLEEP_SECONDS 600 /* after these minutes, we give up.... */

        runKernelExitCode = 0;
        do
        {
            rc = waitpid( runKernelPid, &runKernelExitCode, WNOHANG );
			if ( 0 == rc )
			{
				if ( 0 != KGS
                  && XPARAM(suppressCore)
			      && SERVER_KILL == KGS->state )
				{
					static SAPDB_Int4 deadCheck = 0;
					if ( deadCheck < MAX_DEADCHECK_SLEEP_SECONDS )
					{
						++deadCheck;
					}
					else
					{
						break;
					}
				}
				sleep(1); /* sleep for a second */
				continue;
			}
            else if ( -1 == rc )
            {
                waitpidErrno = errno;
            }
        } while ( ((rc == -1) && (errno == EINTR)) || (0 == rc) );

        /* PTS 1105262 / PTS 1105263 */
        en81_SetIgnoreAction( SIGHUP );

        /* Kill all remaining children of this process */
        kill( 0, SIGHUP );

        // if running kernel did not die until now, we kill it now
        if ( rc != runKernelPid )
        {
            kill( runKernelPid, SIGKILL );
        }

        /* Collect all children zombies, if any... */
        do {
            while(wait((int *)0) != -1);
        } while ( errno == EINTR );

        if ( KGS )
        {
#ifdef AIX
            RTESync_SpinlockRegister::Reset();
            RTEMem_AllocatorRegister::Reset();
            RTEConf_ParameterAccess::Reset();
            RTEMem_Allocator_Reset();
            RTEMem_RteAllocator_Reset();
            RTEMem_PageAllocator_Reset();
            RTEMem_BlockAllocator_Reset();
            RTEMem_RawAllocator_Reset();
            RTESync_SpinlockPoolRegister::Reset();
#endif

#ifdef NEW_DIAG_FILES
            Msg_BeginOfTerminationPhase();
#endif
#ifndef NO_OLD_DIAG_FILES
            en60_ReopenKnldiagFile( KGS->XParam );
#endif
        }

        if ( -1 == rc )
        {
            /* Posix says... [ECHILD] The calling process has no existing unwaited-for child processes.  */
            if ( ECHILD == waitpidErrno )
            {
                MSGD(( WRN_WAITPID_FOR_NO_CHILD ));
                runKernelExitCode = 0; /* be friendly to linux users... */
            }
            else
            {
                errno = waitpidErrno; /* for sqlerrs()... */
                MSGD(( ERR_WAITPID_ERROR, waitpidErrno, sqlerrs() ));
            }
        }
        else if ( 0 == rc )
        {
            MSGD(( ERR_WAITPID_TIMEOUT, MAX_DEADCHECK_SLEEP_SECONDS ));
        }
        else if ( rc != runKernelPid ) /* check for valid runKernelExitCode */
        {
            MSGD(( ERR_WAITPID_WRONG_PID, (long)rc, (long)runKernelPid ));
            runKernelExitCode = KERNEL_TERMINATED_BY_HUP_SIGNAL; /* mark as terminated by HUP signal ... */
        }

        en81_LastWords(runKernelExitCode, SERVER_KILL == KGS->state);
#endif /* SDB_WITH_PROFILER */
    }
    return 0;
}

/*
 * ===========================================================================
 */
#undef  MF__
#define MF__ MOD__"en81_NotifyCoordinator"
externC void en81_NotifyCoordinator( int tw_gone )
{
	if ( tw_gone )
    {
        MSGCD (( INFO_RELEASING_TRACEWRITER ));
        KGS->traceWriterGone = 1;
        sqlsignalsem(en81_TerminateEndCondition);
	}
    sqlsignalsem(en81_TerminateCondition);
}

/*
 * ===========================================================================
 */
static int en81_CheckTracewriterRunning()
{
    return ( KGS != NULL
          && KGS->traceflag != NULL
          && KGS->dumpflag != NULL
          && KGS->first_dev != NULL
          && KGS->first_dev->state != KT_INITIALIZED
          && KGS->traceWriterGone == 0 );
}

/*
 * ===========================================================================
 */
#undef  MF__
#define MF__ MOD__"en81_kill_database"
externC int en81_kill_database ( )
{
    int DontWaitForTracewriter = 1;

    DBGIN;

    if ( KGS != NULL )
    {
        /*
         *  If the server was in normal operation,
         *  try to activate the bufwriter to get dump and trace.
         *  When the bufwriter terminates, it's process exits.
         */
        if ( KGS->state != SERVER_STOP && KGS->state != SERVER_KILL )
        {
		    KGS->state = SERVER_KILL ;
		    MSGD (( INFO_CHANGE_STATE, "KILL", KGS->state ));
        }

        if ( KGS->mode == 0 )
        {
            KGS->mode = MODE_KILL ;
        }

        /*
         *  Set bufwriter's trace and dump flag.
         *  If traceflag or dumpflag are not yet set,
         *  return an error code (vdiaginitCalled = 1), which will break the
         *  coordinator loop.
         *  This can happen during the startup sequence only.
         */
        if ( KGS->traceflag != NULL
          && KGS->dumpflag != NULL )
        {
            DBG1 (( MF__,"setting trace and dump flags \n" ));
            * KGS->traceflag = 1 ;
            * KGS->dumpflag  = 1 ;

            /*
             *  Resume the bufwriter
             */
            /* check on trace dev, not on system dev - might already be detached */
            if ( KGS->first_dev != NULL )
            {
                if ( (KGS->first_dev)->state != KT_INITIALIZED )
                {
    	    	    DBG1 (( MF__,"resuming bufwriter \n" ));
	    	        en81_resume ( KGS->tw );
                    MSGCD ((INFO_RESUMED_TW)) ;
                    DontWaitForTracewriter = 0;
                }
                else
                {
        		    MSGD (( INFO_CHANGE_STATE, "TRACEDEVICE_NOT_INITIALIZED", 0 ));
                }
            }
            else
            {
    		    MSGD (( INFO_CHANGE_STATE, "NO_TRACEDEVICE", 0 ));
            }
        }
        else
        {
            if ( KGS->traceflag == NULL )
            {
    		    MSGD (( INFO_CHANGE_STATE, "NO_TRACEFLAG", 0 ));
            }
            if ( KGS->dumpflag == NULL )
            {
    		    MSGD (( INFO_CHANGE_STATE, "NO_DUMPFLAG", 0 ));
            }
            /*
             *  If traceflag or dumpflag could not be set,
             *  don't wait for the exit of the bufwriter.
             *  This can happen during the early start phase.
             */
        }
    }
    return DontWaitForTracewriter;
}

/* ----------- */
#undef MF__
#define MF__ MOD__"en81NotifyStarter"
void en81NotifyStarter()
{
    if ( KGS->ulStartupOptions & FORCE_AUTORESTART 
      && RTE_SpawnRestartShutdown( true ))
    {
        return;
    }

    if(RTE_DiagPipeIsOpen())
    {
        RTE_CloseDiagPipe();
        (void)en41RemovePipeFDFile(en81_dbname);
    }
}

/* -------------- */
#undef  MF__
#define MF__ MOD__"en81TimerThreadSleep"
externC void en81TimerThreadSleep(tsp00_Int4 SleepPeriod)
{
    teo07_ThreadErr Err;

    sqltimedwaitsem( en81_TimerSleepCondition,
                     SleepPeriod,
                     &Err );
	if ( Err != THR_OK_EO07
	  && Err != THR_TIMEOUT_EO07 )
	{
#define ERR_TIMER_BAD_WAITSEM 11036,ERR_TYPE,"timer   ","ABEND: strange result from sqltimewaitsem %d"
		MSGCD (( ERR_TIMER_BAD_WAITSEM, Err ))
		vabort ( WRITE_CORE );
		/* NOTREACHED */
	}

	if ( Err == THR_OK_EO07 )
	{
		DBG1 (( MF__,"AWOKE prematurly\n" ));
	}
	else
	{
		DBG1 (( MF__,"AWOKE by timeout\n" ));
	}
}

/* -------------- */
#undef  MF__
#define MF__ MOD__"en81TimerThreadWakeup"
externC void en81TimerThreadWakeup()
{
    sqlsignalsem( en81_TimerSleepCondition );
}

/*===========================================================================*
 *  LOCAL FUNCTIONS (CODE)                                                   *
 *===========================================================================*/

/*
 *  Issue start messages which are independend from XParam Record
 */
static void ShowXParamIndependendStatupInformation(SAPDB_Char * currentVersion, tsp00_NodeIdc dbnode)
{
    MSGALL (( INFO_VERSION , RTE_GetKernelVersion() ));
    MSGALL (( INFO_VERSION , RTE_GetRTEVersion() ));
#ifdef SDB_WITH_PROFILER
    MSGALL (( INFO_VERSION , "Compiled for Profiling!" ));
#endif /* SDB_WITH_PROFILER */

    MSGALL(( INFO_DB_START_SERVERDB, en81_dbname.asCharp() ));

    MSGALL(( INFO_DB_START_SERVERNODE, dbnode.asCharp() ));

    {
        time_t     time_now;
        struct tm  tm_now;
        struct tm *now;

        time_now = time((time_t *)0);
        now = en52Localtime_r(&time_now, &tm_now);
        if ( now )
        {
            MSGALL(( INFO_DB_START_DATE, now->tm_year + 1900, now->tm_mon + 1, now->tm_mday ));
        }
        else
        {
            MSGALL(( INFO_DB_START_DATE, 0, 0, 0 ));
        }
    }
    MSGALL(( INFO_DB_START_PID, RTE_save_getpid() ));

    {
        SAPDB_Char nameBuffer[256];
        SAPDB_UInt4 neededSize;
        if ( RTE_GetUsernameFromPasswdById( (SAPDB_Int4)geteuid()
                                          , nameBuffer
                                          , sizeof(nameBuffer)
                                          , neededSize) )
        {
            MSGALL(( INFO_DB_START_OWNER, &nameBuffer[0] ));
        }

        if ( RTE_GetGroupNameFromGroupById( (SAPDB_Int4)getegid()
                                          , nameBuffer
                                          , sizeof(nameBuffer)
                                          , neededSize) )
        {
            MSGALL(( INFO_DB_START_GROUP, &nameBuffer[0] ));
        }
    }

#ifdef BIT64
    MSGALL(( INFO_DB_START_VERSION, "64BIT Kernel" ));
#else
    MSGALL(( INFO_DB_START_VERSION, "32BIT Kernel" ));
#endif
    MSGALL(( INFO_DB_START_VERSION, currentVersion ));
    MSGALL(( INFO_DB_START_VERSION, RTE_GetSpeed() ));
    /*
        http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1127694
    */
    {
        SAPDB_Char cpuType[64];
        RTESys_GetCPUType(cpuType,64);
        MSGALL(( INFO_DB_START_MACHINE, cpuType ));
    }

	{
		SAPDB_UInt4 NumberOfProcessors = RTESys_NumberOfCPU( );
		SAPDB_UInt4 NumberOfPhysicalProcessors = RTESys_NumberOfPhysicalCPUs( );

		if( NumberOfPhysicalProcessors != NumberOfProcessors )
			MSGALL (( INFO_DB_START_NUM_LOGICAL_PROCESSORS, NumberOfPhysicalProcessors, NumberOfProcessors ))
		else
			MSGALL (( INFO_DB_START_NUM_PROCESSORS, NumberOfProcessors ));

	}

	{
        tsp00_Pathc currentDirectory;
        if ( getcwd((char *)currentDirectory, sizeof(tsp00_Path)) != 0 )
        {
            MSGALL(( INFO_DB_START_DBROOT, &currentDirectory[0] ));
        }
    }

    MSGALL(( INFO_DB_START_LIBPATH, LDLIBPATH_IP00, 
            getenv(LDLIBPATH_IP00) ? getenv(LDLIBPATH_IP00) : ""));
}

/*----------------------------------*/

#undef  MF__
#define MF__ MOD__"en81_Startup"
static  int     en81_Startup ( SAPDB_Char * currentVersion  , char *envp[] )
{
    tsp00_NodeIdc               dbnode ;
    int                         rc ;
    char                      * p ;
    RTE_XPARAM_REC              XparamParameters ;

    DBGIN;

    SAPDB_memset ( &XparamParameters ,0, sizeof( RTE_XPARAM_REC ) ) ;

    /*
     *  Get node name
     */
    rc = sql43_get_my_host_name ( dbnode , sizeof(tsp00_NodeId) );
    if ( rc )
    {
		MSGCD (( ERR_MISSING_HOSTNAME )) ;
		return ( 2 );
    }

    ShowXParamIndependendStatupInformation( currentVersion, dbnode);

#ifdef SOCKETS
    rc = sql43_get_official_node_name ( dbnode , dbnode ,
					sizeof(tsp00_NodeId) );
    if ( rc )
    {
		MSGCD (( ERR_MISSING_OFFICIAL_NAME )) ;
		return ( 2 );
    }
    DBG1 (( MF__,"official dbnode  '%s' \n", dbnode ));
#endif /* SOCKETS */

    /*
     *  Read parameter file
     */
    {
        Msg_List err;
        if(!RTEConf_ReadAndAnalyzeParam ( en81_dbname, &XparamParameters, err))
        {
            RTE_Message(err);
            return ( 2 ) ;
        }
    }

    /*
     *  Go to rundirectory
     */
    if ( XparamParameters.szRunDirectory [ 0 ] == '\0' )
    {
		MSGCD (( ERR_XP_RUNDIR_MISSING, PAN_RUNDIR ));
		return ( 2 );
    }

    {
        tsp00_Pathc currentDirectory;
        if ( getcwd((char *)currentDirectory, sizeof(tsp00_Path)) != 0
          || strcmp(currentDirectory, XparamParameters.szRunDirectory) != 0 )
        {
            if ( chdir ( XparamParameters.szRunDirectory ) )
            {
		        MSGCD (( ERR_WRONG_XP_RUNDIR, XparamParameters.szRunDirectory.asCharp(),
                                sqlerrs() )) ;
		        return ( 2 );
            }
        }
    }

    /*
     *  Get the number of usable file descriptors for this process
     */
    en81_usable_fds = sysconf ( _SC_OPEN_MAX );

    /*
     *  Open diag file and console terminals.
     *  Ignore errors relating to the terminals.
     *
     *  If the database was shutting down abnormally
     *  the last time, the diagnostic files are beeing
     *  saved to the history, too.
     */
    if ( e60_open_opmsgs( &XparamParameters ) & 0xf )
    {
		MSGCD (( ERR_CANT_OPEN_DIAGFILES ));
		return ( 2 );
    }
    /* If found, setup OfficialNode from parameter file */
    if ( XparamParameters.szOfficialNode[0] != '\0' )
    {
        strncpy(dbnode, XparamParameters.szOfficialNode, sizeof(tsp00_NodeId));
        dbnode[sizeof(tsp00_NodeId)] = 0;
                MSGCD (( INFO_USER_SPECIFIED_OFFICIAL_NODE , dbnode.asCharp() ));
    }
    else
    {
        MSGALL(( INFO_DB_START_SERVERNODE, dbnode.asCharp() ));
    }

    /*
     * PTS 1116686
     */
    en81OutputResourceLimits(&XparamParameters , envp );
    /*
     * PTS 1113405
     */
    if ( XparamParameters.suppressCore )
    {
        en81_SuppressCoreDump();
    }
    else
    {
        en81_AllowCoreDump(&XparamParameters.suppressCore);
    }

    /*
     * Setup SignalHandler for common signals
     */
    en81_SetSignalHandler( XparamParameters.suppressCore );

    /* Memory allocation limit etc... */
    RTEConf_HandleSpecialParametersWithMessageOutput();

    /*
     *  Check if coroutines are available and are to be used
     */

    en81_CheckCoroutineUsage(&XparamParameters );
    /*
     *  Initialize shared memory
     */

    rc = e70_init_kgs ( & XparamParameters );
    if ( rc ) return ( rc );

#if defined(LINUX)
    RTESys_LinuxTimerLeapCheck( 0 == SAPDB_strcasecmp(XparamParameters.szHiResTimerType.asCharp(), "GETTIMEOFDAY") );
#endif
    e52_init_time ();

    /* PTS 1115397
     * Check for working pipe check algorithm using existing FIFO directory
     */
    en54FindPipeCheckAlgorithm(en81_dbname);

    /*
        Validate number of available file descriptor
     */
    rc = en81_ValidateNumberOfWantedDescriptors();
    if ( rc != 0 )
    {
        return ( rc );
    }

    DBG1 (( MF__,"KGS 0x%lx\n", KGS ));

    KGS->parentPid = RTE_save_getpid();

    strncpy ( KGS->KernelVersion, (char *)RTE_GetKernelVersion(), sizeof(KGS->KernelVersion) );

    /*
     *  Get this rte's version
     */

    SAPDB_memcpy ( KGS->RTEVersion , currentVersion , strlen(currentVersion) );
    DBG1 (( MF__,"COORDINATOR: RTEVersion '%s' \n", KGS->RTEVersion ));
	sp100_GetVersionID( VersionIDType0_esp100, s100buildnumberEx, & KGS->RTEVersionID);

    /*
     *  Copy node name
     */

    (void) strcpy ( (char *)KGS->servernode , (const char *)dbnode );

    /*
     *  Get code size
     */

#if defined(OSF1)
    KGS->codesize = (tsp00_Int4) ( (long) & _etext - (long) & _ftext );
    DBG1 (( MF__,"& _ftext %p \n", & _ftext ));
    DBG1 (( MF__,"& _etext %p \n", & _etext ));
    DBG1 (( MF__,"KGS->codesize 0x%x \n", KGS->codesize ));
#elif defined(AIX)
#ifdef BIT64
    KGS->codesize = (tsp00_Int4) ( (long) & _etext - 0x100000000L );
#else
    KGS->codesize = (tsp00_Int4) ( (long) & _etext -  0x10000000L );
#endif
#elif defined(SVR4) || defined(SOLARIS)
    KGS->codesize = (tsp00_Int4) ( (long)& etext  - (long)& _start );
#elif defined(HPUX)
    KGS->codesize = (tsp00_Int4) ( (long)& _etext  - (long)& __text_start );
#else /* other OS */
    KGS->codesize = (tsp00_Int4) ( (long) & etext );
#endif
    DBG1 (( MF__,"KGS->codesize %d \n", KGS->codesize ));

    KGS->state = SERVER_STARTING ;

    DBGOUT;
    return ( 0 );
}

/*
 * ===========================================================================
 */
#undef  MF__
#define MF__ MOD__"en81_CreateWorkerSemaphores"
static  int en81_CreateWorkerSemaphores ( ten81_WorkerInfo *pWorker, int *pIdx )
{
    int rc;

    rc = sql41_create_sem ( KGS->ipcBaseKey , (*pIdx)++ , 0 , 0666 ,
					RTE_save_getuid() , (char *)"db" , KGS->serverdb );
	if ( rc < 0 )
	{
        DBG1(( MF__,"WORKER trigger semaphore creation failed:%s\n", sqlerrs() ));
		return ( rc );
	}
    pWorker->triggerSemaphore = rc;

	rc = sql41_create_sem ( KGS->ipcBaseKey , (*pIdx)++ , 0 , 0666 ,
					RTE_save_getuid() , (char *)"db" , KGS->serverdb );
	if ( rc < 0 )
	{
        DBG1(( MF__,"WORKER done semaphore creation failed:%s\n", sqlerrs() ));
		return ( rc );
	}
    pWorker->doneSemaphore = rc;

    return 0;
}

/*
 * ===========================================================================
 */
#undef  MF__
#define MF__ MOD__"en81_CreateKtSemaphores"
static  int en81_CreateKtSemaphores ( void )
{
    teo07_ThreadErr err;
    int			rc ;
    int			idx = 0 ;
    ten50_UKT_Control  *ukt ;


    DBGIN;

    DBG1(( MF__,"KGS->serverdb: %s\n", KGS->serverdb ));

    /*
     *  Create semaphores for timer thread
     */
    tsp00_ErrTextc dummy;
    sqlcreatesem( &en81_TimerSleepCondition, 0, dummy, &err ) ;
    if ( err != THR_OK_EO07 )
    {
        DBG1(( MF__,"TimerSleepCondition failed\n" ));
		return err;
    }
    DBG1(( MF__,"TimerSleepCondition created\n" ));

    /*
     *  Create semaphores for UKPS
     */
    for ( ukt = KGS->first_kp ; ukt <= KGS->last_kp ; ukt ++ )
    {
		rc = sql41_create_sem ( KGS->ipcBaseKey , idx ++ , 0 , 0666 ,
					RTE_save_getuid() , (char *)"db" , KGS->serverdb );
		if ( rc < 0 )
		{
            DBG1(( MF__,"UKT semaphore failed:%s\n", sqlerrs() ));
			return ( rc );
		}
		ukt->semid = rc ;
		DBG1(( MF__,"UKT   semid %d", rc ));
    }

    /*
     *  Create semaphore for DEV0
     */
    rc = sql41_create_sem ( KGS->ipcBaseKey , idx ++ , 0 , 0666 ,
				    RTE_save_getuid() , (char *)"db" , KGS->serverdb );
    if ( rc < 0 )
    {
        DBG1(( MF__,"DEV0 semaphore failed:%s\n", sqlerrs() ));
		return ( rc );
    }
    KGS->dev0.semid = rc ;
    DBG1(( MF__,"DEV0  semid %d", rc ));

    rc = en81_CreateWorkerSemaphores( &en81_CrashWorker, &idx);
    if ( rc != 0 )
    {
        return (rc);
    }

    rc = en81_CreateWorkerSemaphores( &en81_StopWorker, &idx);
    if ( rc != 0 )
    {
        return (rc);
    }

    rc = en81_CreateWorkerSemaphores( &en81_UktDumpWorker, &idx);
    if ( rc != 0 )
    {
        return (rc);
    }

    DBGOUT;
    return ( 0 );
}

/*
 * ===========================================================================
 */
#undef  MF__
#define MF__ MOD__"en81_TriggerWorker"
static void en81_TriggerWorker( ten81_WorkerInfo *pInfo )
{
    int           rc;
    struct sembuf own_sembuf ;

    if ( pInfo->running != 0 )
    {
        SAPDB_memcpy ( &own_sembuf , &semsnd , sizeof(struct sembuf) );
        do {
            rc = semop ( pInfo->triggerSemaphore, &own_sembuf, 1 );
        } while ( (rc == -1) && (errno == EINTR) );

        if ( rc == -1 )
        {
            en81Abort();
        }
    }
}

/* -------------- */
#undef  MF__
#define MF__ MOD__"en81_WaitForWorker"
static void en81_WaitForWorker( ten81_WorkerInfo *pInfo )
{
    int           rc;
    struct sembuf own_sembuf ;

    if ( pInfo->running != 0 )
    {
        SAPDB_memcpy ( &own_sembuf , &semrcv , sizeof(struct sembuf) );
        do {
            rc = semop( pInfo->doneSemaphore, &own_sembuf, 1);
        } while ( (rc == -1) && (errno == EINTR) );

        if ( rc == -1 )
        {
            en81Abort();
        }
    }
}

/*
 * ===========================================================================
 */
#undef  MF__
#define MF__ MOD__"en81_WorkerMain"
extern "C" void *en81_WorkerMain( void *info )
{
    int rc;
    struct sembuf                       own_sembuf ;
    ten81_WorkerInfo *pInfo = (ten81_WorkerInfo *)info;

    pInfo->running = 1;
    do
    {
        /*
         *  The main loop for the Worker thread
         */
        SAPDB_memcpy ( &own_sembuf , &semrcv , sizeof(struct sembuf) );
        do { /* wait until wakened */
            rc = semop ( pInfo->triggerSemaphore, &own_sembuf, 1 );
        } while ( (rc == -1) && (errno == EINTR) );

        if ( rc == -1 )
        {
            vabort(WRITE_CORE);
        }

        (*(pInfo->work))();

        SAPDB_memcpy ( &own_sembuf , &semsnd , sizeof(struct sembuf) );
        do {
            rc = semop ( pInfo->doneSemaphore, &own_sembuf, 1 );
        } while ( (rc == -1) && (errno == EINTR) );

        if ( rc == -1 )
        {
            vabort(WRITE_CORE);
        }
    } while ( pInfo->loopForever != 0 );

    /*
     * Worker will never return...
     */
    while( en81_neverStop )
    {
        sqlyieldthread();
    }
    return (void *)0;
}

/*
 * ===========================================================================
 */
#undef  MF__
#define MF__ MOD__"en81_CreateWorker"
static int en81_CreateWorker( ten81_WorkerInfo *pInfo, void (*work)() )
{
    teo07_ThreadErr err;
	tsp00_ErrTextc  errtext;

    pInfo->work = work;
    /*
     *  Create WORKER thread
     */
    sqlbeginthread( KT_WORKER_STACK_SIZE,
                    en81_WorkerMain,
                    pInfo,
					THR_CREATE_SUSPENDED_EO07,
                    &(pInfo->thread),
                    errtext,
                    &err );
    if ( err == THR_OK_EO07 )
    {
        pInfo->tid = sqlthreadid(pInfo->thread);
    }
    else
    {
        MSGCD (( ERR_CREATE_THREAD , pInfo->name, errtext.asCharp() )) ;
        return ( 1 );
    }

    sqlresumethread( pInfo->thread, errtext, &err );
    if ( err != THR_OK_EO07 )
    {
        MSGCD (( ERR_RESUME_THREAD, pInfo->name, errtext.asCharp() ));
        return ( 1 );
    }

    DBG1(( MF__,"%s thread created, tid %ld", pInfo->name, (long) pInfo->tid ));
    return ( 0 );
}

/*
 * ===========================================================================
 */
#undef  MF__
#define MF__ MOD__"en81_CreateRteProcs"
static  int     en81_CreateRteProcs ( void )
{
    int				rc ;
    teo07_ThreadErr err;
	tsp00_ErrTextc  errtext;

    DBGIN;


    /*
     *  Create CONSOLE thread
     */
    sqlbeginthread( KT_CONSOLE_STACK_SIZE,
				    RTEThread_Console,
				    NULL,
					THR_CREATE_SUSPENDED_EO07,
					& KGS->console.thread,
				    errtext,
					&err );
	if ( err == THR_OK_EO07 )
	{
		KGS->console.tid = sqlthreadid(KGS->console.thread);
	}
    else
    {
        MSGCD (( ERR_CREATE_THREAD , "CONSOLE", errtext.asCharp() )) ;
		return ( 3 );
    }
	sqlresumethread( KGS->console.thread, errtext, &err );
    if ( err != THR_OK_EO07 )
    {
        MSGCD (( ERR_RESUME_THREAD, "CONSOLE", errtext.asCharp() ));
    }
    DBG1(( MF__, "CONSOLE created,   tid %ld", (long) KGS->console.tid ));

    /*
     * Start the worker for signal handling
     */
    rc = en81_CreateWorker( &en81_CrashWorker, en81_CrashAction );
    if ( rc != 0 )
        return rc;
    rc = en81_CreateWorker( &en81_StopWorker, en81_StopAction );
    if ( rc != 0 )
        return rc;
    rc = en81_CreateWorker( &en81_UktDumpWorker, en81_UktDumpAction );
    if ( rc != 0 )
        return rc;

    /*
     *  Create TIMER process
     */
    sqlbeginthread( KT_TIMER_STACK_SIZE,
                    en83TimerThread,
                    NULL,
					THR_CREATE_SUSPENDED_EO07,
                    & KGS->timer.thread,
                    errtext,
                    &err );
    if ( err == THR_OK_EO07 )
    {
        KGS->timer.tid = sqlthreadid(KGS->timer.thread);
    }
    else
    {
        MSGCD (( ERR_CREATE_THREAD , "TIMER", errtext.asCharp() )) ;
        return ( 3 );
    }
	sqlresumethread( KGS->timer.thread, errtext, &err );
    if ( err != THR_OK_EO07 )
    {
        MSGCD (( ERR_RESUME_THREAD, "TIMER", errtext.asCharp() ));
    }

    DBG1(( MF__,"TIMER created,     tid %ld", (long) KGS->timer.tid ));

    /*
     *  Create REQUESTOR Process
     */
    sqlbeginthread( KT_REQUESTOR_STACK_SIZE,
                    en84_requestor,
                    NULL,
					THR_CREATE_SUSPENDED_EO07,
                    & KGS->requestor.thread,
                    errtext,
                    &err );
    if ( err == THR_OK_EO07 )
    {
        KGS->requestor.tid = sqlthreadid(KGS->requestor.thread);
    }
    else
    {
        MSGCD (( ERR_CREATE_THREAD , "REQUESTOR", errtext.asCharp() )) ;
        return ( 3 );
    }
	sqlresumethread( KGS->requestor.thread, errtext, &err );
    if ( err != THR_OK_EO07 )
    {
        MSGCD (( ERR_RESUME_THREAD, "REQUESTOR", errtext.asCharp() ));
    }

    DBG1(( MF__,"REQUESTOR created, tid %ld", (long) KGS->requestor.tid ));

    /*
     *  Create DEV0 and if wanted all DEVi threads
     */
    rc = en54CreateIOWorkerThreads();
    if ( rc != 0 )
        return rc;

    RTETask_CWrapperForCreateWorkerThread(KT_WORKER_STACK_SIZE);

    DBGOUT;
    return ( 0 );
}

#ifdef LINUX

#ifndef CPU_SETSIZE
/* number of bits in a cpu_set_t used in sched_set/getaffinity() despite the current manual pages say they use 'unsigned long *' ....*/
#define CPU_SETSIZE 1024
#endif
/* size of unsigned long in bits */
#define MASK_WORD_IN_BITS (sizeof(SAPDB_ULong)*8)

static SAPDB_Char   *RTE_AffinityStringCopy = 0;
static SAPDB_ULong **RTE_AffinityMaskArray  = 0;
static SAPDB_Char  **RTE_AffinitySubStringArray = 0;
static SAPDB_UInt4   RTE_AffinityArraySize = 0;
static SAPDB_UInt4   RTE_ProcessorsConfigured = 0;
static SAPDB_UInt4   RTE_MaskLengthInBytes = 0;

/*---*/

/*!
  @brief parse the first number in the given string and check if it is in range
  @param number_string [in] number string to parse
  @param processor_number [out] number found
  @return true valid number found, false invalid number
 */
SAPDB_Bool RTE_ParseProcessorNumber( SAPDB_Char const *number_string
                                   , SAPDB_UInt4 &processor_number )
{
    SAPDB_UInt4 number = 0;
    SAPDB_Char const *parse = number_string;

    while ( parse )
    {
        switch( *parse )
        {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                number *= 10;
                number += (*parse++) - '0';

                break;
            case '-':
                parse = 0;
                break;
            case ',':
                parse = 0;
                break;
            case 0:
                parse = 0;
                break;
            default:
                number = RTE_ProcessorsConfigured;
                break;
        }
    }

    if ( number >= RTE_ProcessorsConfigured )
    {
        return false;
    }

    processor_number = number;

    return true;
}

/*---*/

/*!
  @brief fill bits in processor mask

  Mask is not cleared by this routine!

  @param pmask_bits [inout] mask to fill
  @param first_processor [in] first processor to set
  @param last_processor [in] last processor to set
  @return true if range ok, false if nothing filled
 */
SAPDB_Bool RTE_FillBits( SAPDB_ULong *pmask_bits,
                         SAPDB_UInt4 first_processor,
                         SAPDB_UInt4 last_processor )
{
    SAPDB_UInt4 mask_word_index;
    SAPDB_UInt4 mask_index;
    SAPDB_UInt4 first_word_index = first_processor/MASK_WORD_IN_BITS;
    SAPDB_UInt4 last_word_index = last_processor/MASK_WORD_IN_BITS;
    SAPDB_UInt4 first_index;
    SAPDB_UInt4 last_index;

    if ( first_processor > last_processor )
    {
        return false;
    }

    for ( mask_word_index = first_word_index;
          mask_word_index <= last_word_index;
        ++mask_word_index )
    {
        if ( mask_word_index == first_word_index )
        {
            first_index = first_processor % MASK_WORD_IN_BITS;
        }
        else
        {
            first_index = 0;
        }

        if ( mask_word_index == last_word_index )
        {
             last_index = last_processor % MASK_WORD_IN_BITS;
        }
        else
        {
             last_index = MASK_WORD_IN_BITS - 1;
        }

        for ( mask_index = first_index;
              mask_index <= last_index;
            ++mask_index )
        {
            pmask_bits[mask_word_index] |= (1UL << mask_index);
        }
    }
}

/*---*/

SAPDB_Bool RTE_FillSingleMask(SAPDB_ULong *mask_bits, SAPDB_Char const *mask_string)
{
    SAPDB_Char const *first_pointer;
    SAPDB_Char const *last_pointer;
    SAPDB_Char const *next_range;

    for ( first_pointer = mask_string;
          first_pointer != 0;
          first_pointer = next_range + 1 )
    {
        SAPDB_UInt4 first_processor;
        SAPDB_UInt4 last_processor;

        if ( 0 == *first_pointer )
        {
            break;
        }
        next_range = strchr(first_pointer, ',');
        last_pointer = strchr(first_pointer, '-');

        if ( !last_pointer || (next_range && (last_pointer > next_range)) )
        {
             if ( !RTE_ParseProcessorNumber(first_pointer, first_processor) )
             {
                 MSGD (( 12627, WRN_TYPE,_T("TASKING "),_T("Parse error in processor mask %s bad first number: %s"), mask_string, first_pointer ));
                 return false;
             }

             last_processor = first_processor;
        }
        else
        {
             if ( last_pointer == first_pointer )
             {
                first_processor = 0;
             }
             else
             {
                if ( !RTE_ParseProcessorNumber(first_pointer, first_processor) )
                {
                    MSGD (( 12627, WRN_TYPE,_T("TASKING "),_T("Parse error in processor mask %s bad first number: %s"), mask_string, first_pointer ));
                    return false;
                }
             }

             ++last_pointer; /* skip '-' */
             if ( (last_pointer == next_range) || 0 == *last_pointer )
             {
                last_processor = RTE_ProcessorsConfigured;
             }
             else
             {
                if ( !RTE_ParseProcessorNumber(last_pointer, last_processor) )
                {
                    MSGD (( 12627, WRN_TYPE,_T("TASKING "),_T("Parse error in processor mask %s bad last number: %s"), mask_string, last_pointer ));
                    return false;
                }
             }
        }

        if ( !RTE_FillBits(mask_bits, first_processor, last_processor) )
        {
            MSGD (( 12627, WRN_TYPE,_T("TASKING "),_T("Parse error in processor mask %s bad range %u-%u"), mask_string, first_processor, last_processor ));
            return false;
        }

        if ( 0 == next_range )
        {
             break;
        }
    }
    return true;
}

/*---*/

void RTE_CountArraySize(SAPDB_Char const *mask_string)
{
#define EXPECT_FIRST_NUMBER    0x1
#define EXPECT_RANGE_DELIMITER 0x2
#define EXPECT_LIST_DELIMITER 0x2
#define EXPECT_SECOND_NUMBER   0x4
#define EXPECT_MASK_DELIMITER  0x8
    SAPDB_UInt4 arraySize = 0;
    SAPDB_Char const *pParse;
    SAPDB_Char *pCopy = RTE_AffinityStringCopy;
    SAPDB_Char currentChar;
    SAPDB_Int parseState = EXPECT_FIRST_NUMBER|EXPECT_RANGE_DELIMITER;

    RTE_AffinityArraySize = 0;

    for ( SAPDB_Char const *pParse = mask_string; pParse && *pParse; ++pParse )
    {
        currentChar = *pParse;

        switch ( currentChar )
        {
        case ' ':
        case '\t':
        case '\n':
            // white spaces silently ignored
            break;
        case '0':        case '1':        case '2':        case '3':        case '4':
        case '5':        case '6':        case '7':        case '8':        case '9':
            if ( parseState & EXPECT_FIRST_NUMBER )
            {
                if ( 0 == arraySize ) arraySize = 1;
                *pCopy++ = currentChar;
                parseState &= (~(EXPECT_SECOND_NUMBER));
                parseState |= (EXPECT_RANGE_DELIMITER|EXPECT_MASK_DELIMITER);
            }
            else if ( parseState & EXPECT_SECOND_NUMBER )
            {
                *pCopy++ = currentChar;
                parseState &= (~(EXPECT_RANGE_DELIMITER|EXPECT_FIRST_NUMBER));
                parseState |= EXPECT_MASK_DELIMITER;
            }
            else
            {
                MSGD (( 12627, ERR_TYPE,_T("TASKING "),_T("Parse error: Internal error? Number not expected at offset ? State 0x%x\n"),
                    pParse - mask_string, parseState ));
                return;
            }
            break;
        case '-':
            if ( parseState & EXPECT_RANGE_DELIMITER )
            {
                if ( 0 == arraySize ) arraySize = 1;
                *pCopy++ = currentChar;
                parseState &= (~(EXPECT_RANGE_DELIMITER|EXPECT_FIRST_NUMBER));
                parseState |= EXPECT_MASK_DELIMITER|EXPECT_SECOND_NUMBER;
            }
            else
            {
            MSGD (( 12627, WRN_TYPE,_T("TASKING "),_T("Parse error: Bad range delimiter '-' at offset %u\n"),
                currentChar, pParse - mask_string ));
            }
            break;
        case ',':
            if ( parseState & EXPECT_LIST_DELIMITER )
            {
                *pCopy++ = currentChar;
                parseState &= (~(EXPECT_LIST_DELIMITER|EXPECT_SECOND_NUMBER));
                parseState |= EXPECT_RANGE_DELIMITER|EXPECT_FIRST_NUMBER|EXPECT_MASK_DELIMITER;
            } // additional list delimiters silently ignored
            break;
        case ';':
            if ( parseState & EXPECT_MASK_DELIMITER )
            {
                ++arraySize;
                *pCopy++ = currentChar;
                parseState &= (~(EXPECT_MASK_DELIMITER|EXPECT_SECOND_NUMBER));
                parseState |= (EXPECT_FIRST_NUMBER|EXPECT_RANGE_DELIMITER);
            } // additional mask delimiters silently ignored
            break;
        default:
            MSGD (( 12627, WRN_TYPE,_T("TASKING "),_T("Parse error: Bad character '%c' at offset %u\n"),
                currentChar, pParse - mask_string ));
            return;
        }
    }
    *pCopy = 0;
    RTE_AffinityArraySize = arraySize;
    if ( 0 == arraySize )
    {
        MSGD (( 12627, WRN_TYPE,_T("TASKING "),_T("Parse error: Empty affinity string\n") ));
    }
}

/*!
  @brief fill bits in processor mask parsing mask string

  Mask is cleared by this routine! The mask string is a comma seperated list of processor
  ranges. Each range has a default first processor 0 and a default last processor (configured - 1).
  Valid range specification would be -13 (first 14 processors) or 1-3,7-9 or 1,2,3,7,8,9 or 2,5-

  @param mask_string [in] processor mask string to use
  @return true if range ok, false if nothing filled
 */
SAPDB_Bool RTE_FillUsingGivenMaskString( SAPDB_Char *mask_string)
{
    RTE_AffinityStringCopy = (SAPDB_Char *) RTEMem_RteAllocator::Instance().Allocate( strlen(mask_string) + 1);
    if ( !RTE_AffinityStringCopy )
    {
        MSGD (( 12627, WRN_TYPE,_T("TASKING "),_T("Could not allocate affinty string copy") ));
        return false;
    }

    RTE_CountArraySize(mask_string);
    if ( 0 == RTE_AffinityArraySize )
    {
        MSGD (( 12627, WRN_TYPE,_T("TASKING "),_T("Bad affinity mask string") ));
        RTEMem_RteAllocator::Instance().Deallocate( RTE_AffinityStringCopy );
        RTE_AffinityStringCopy = 0;
        return false;
    }

    RTE_AffinitySubStringArray = (SAPDB_Char **)RTEMem_RteAllocator::Instance().Allocate(RTE_AffinityArraySize * sizeof(SAPDB_Char *));
    if ( !RTE_AffinitySubStringArray )
    {
        MSGD (( 12627, WRN_TYPE,_T("TASKING "),_T("Could not allocate affinity sub string array for %u pointer"),
            RTE_AffinityArraySize ));
        RTEMem_RteAllocator::Instance().Deallocate( RTE_AffinityStringCopy );
        RTE_AffinityStringCopy = 0;
        RTE_AffinityArraySize = 0;
        return false;
    }   

    RTE_AffinityMaskArray = (SAPDB_ULong **)RTEMem_RteAllocator::Instance().Allocate(RTE_AffinityArraySize * sizeof(SAPDB_ULong *));
    if ( !RTE_AffinityMaskArray )
    {
        MSGD (( 12627, WRN_TYPE,_T("TASKING "),_T("Could not allocate affinty mask array for %u pointer"),
            RTE_AffinityArraySize ));
        RTEMem_RteAllocator::Instance().Deallocate( RTE_AffinitySubStringArray );
        RTE_AffinitySubStringArray = 0;
        RTEMem_RteAllocator::Instance().Deallocate( RTE_AffinityStringCopy );
        RTE_AffinityStringCopy = 0;
        RTE_AffinityArraySize = 0;
        return false;
    }   

    SAPDB_Byte *pMaskSpace = (SAPDB_Byte *)RTEMem_RteAllocator::Instance().Allocate(RTE_AffinityArraySize * RTE_MaskLengthInBytes);
    if ( !pMaskSpace )
    {
        MSGD (( 12627, WRN_TYPE,_T("TASKING "),_T("Could not allocate affinity sub string array for %u x %u bytes"),
            RTE_AffinityArraySize, RTE_MaskLengthInBytes ));
        RTEMem_RteAllocator::Instance().Deallocate( RTE_AffinityMaskArray );
        RTE_AffinityMaskArray = 0;
        RTEMem_RteAllocator::Instance().Deallocate( RTE_AffinitySubStringArray );
        RTE_AffinitySubStringArray = 0;
        RTEMem_RteAllocator::Instance().Deallocate( RTE_AffinityStringCopy );
        RTE_AffinityStringCopy = 0;
        RTE_AffinityArraySize = 0;
        return false;
    }   
    memset(pMaskSpace, 0, RTE_AffinityArraySize * RTE_MaskLengthInBytes);

    SAPDB_UInt4 iArray;
    SAPDB_Char *pCopy;
    for ( iArray = 0, pCopy = RTE_AffinityStringCopy; iArray < RTE_AffinityArraySize; ++iArray )
    {
        RTE_AffinitySubStringArray[iArray] = pCopy;
        while ( 0 != *pCopy ) // move to next mask and clear mask separator...
        {
            if ( *pCopy == ';' )
            {
                *pCopy = 0;
                ++pCopy;
                break;
            }
            else
            {
                ++pCopy;
            }
        }

        RTE_AffinityMaskArray[iArray] = (SAPDB_ULong *)(pMaskSpace + (iArray * RTE_MaskLengthInBytes));

        if ( !RTE_FillSingleMask(RTE_AffinityMaskArray[iArray], RTE_AffinitySubStringArray[iArray]) )
        {
            MSGD (( 12627, WRN_TYPE,_T("TASKING "),_T("Filling bits in sub mask %u (%s) failed"),
                iArray, RTE_AffinitySubStringArray[iArray] ));
            RTEMem_RteAllocator::Instance().Deallocate( pMaskSpace );
            RTEMem_RteAllocator::Instance().Deallocate( RTE_AffinityMaskArray );
            RTE_AffinityMaskArray = 0;
            RTEMem_RteAllocator::Instance().Deallocate( RTE_AffinitySubStringArray );
            RTE_AffinitySubStringArray = 0;
            RTEMem_RteAllocator::Instance().Deallocate( RTE_AffinityStringCopy );
            RTE_AffinityStringCopy = 0;
            RTE_AffinityArraySize = 0;
            return false;
        }
    }

    return true;
}

/*
 * ===========================================================================
 */
static void SetProcessorAffinity()
{
    // to setup masks, we need the number of processors configured
    RTE_ProcessorsConfigured = (SAPDB_UInt4)sysconf(_SC_NPROCESSORS_CONF);

    if ( RTE_ProcessorsConfigured <= 1 )
    {
        MSGD (( 12627, INFO_TYPE,_T("TASKING "),_T("No CPU binding, since only %d processor configured"),
            RTE_ProcessorsConfigured ));
        return; // on single process systems we have nothing to do with binding...
    }
    else
    {
        MSGD (( 12627, INFO_TYPE,_T("TASKING "),_T("Found %d processors configured"),
            RTE_ProcessorsConfigured ));
    }

    Msg_List errList;
    RTEConf_Parameter::String valueString;

    if((RTEConf_ParameterAccess::Instance())->GetString( (SAPDB_UTF8 *)"UKT_CPU_RELATIONSHIP", valueString, errList))
    {
        if ( 0 == strcmp("NONE", (char*)valueString )) 
        {
            MSGD (( 12627, INFO_TYPE,_T("TASKING "),_T("UKT_CPU_RELATIONSHIP set to NONE") ));
            return; // no binding wanted
        }
        else if ( !(char*)valueString  || !*(char*)valueString )
        {
            MSGD (( 12627, INFO_TYPE,_T("TASKING "),_T("No processor range list given") ));
            return;
        }
    }
    else
    {
        MSGD (( 12627, INFO_TYPE,_T("TASKING "),_T("UKT_CPU_RELATIONSHIP not set") ));
        return; // default: no binding wanted
    }

    RTE_MaskLengthInBytes = ( ( RTE_ProcessorsConfigured + CPU_SETSIZE - 1 )
                        / CPU_SETSIZE ) * (CPU_SETSIZE/8);

    if ( !RTE_FillUsingGivenMaskString((char*)valueString) )
    {
        MSGD (( 12627, WRN_TYPE,_T("TASKING "),_T("Could not fill affinty mask with given mask %s"),
            (char*)valueString ));
        return;
    }

    SAPDB_UInt4 nextMaskToUse = 0;
    ten50_UKT_Control * ukt;
    for ( ukt = KGS->first_kp ; ukt <= KGS->last_kp ; ukt ++ )
    {
        if ( ukt->numUserTasks > 0 )
        {
            if ( 0 != sched_setaffinity( (pid_t)ukt->tid, 
                                         (size_t)RTE_MaskLengthInBytes,
                                         (cpu_set_t *)RTE_AffinityMaskArray[nextMaskToUse]) )
            {
                MSGD (( 12627, WRN_TYPE,_T("TASKING "),_T("UKT%d (LWPID %u) set affinity to CPUs '%s' failed [%d]:%s"),
                    ukt->index, (SAPDB_UInt4)ukt->tid, RTE_AffinitySubStringArray[nextMaskToUse], errno, sqlerrs() ));
            }
            else
            {
                MSGD (( 12627, INFO_TYPE,_T("TASKING "),_T("UKT%d (LWPID %u) set affinity to CPU '%s'"),
                    ukt->index, (SAPDB_UInt4)ukt->tid, RTE_AffinitySubStringArray[nextMaskToUse] ));
            }
            nextMaskToUse = ((1+nextMaskToUse)%RTE_AffinityArraySize);
        }
    }

    return;
}

#endif

/*
 * ===========================================================================
 */
static  int     en81_CreateUkts ( void )
{
#undef  MF__
#define MF__ MOD__"en81_CreateUkts"
    ten50_UKT_Control * ukt;
    struct  TASK_TYPE * tcb;
    tsp00_ErrTextc      errtext;
    teo07_ThreadErr     err;
    int                 waitTime;

    DBGIN;

    if ( XPARAM(fUseCoroutines) )
    {
        XPARAM(uLoadBalancingCheck) = 0;

        for ( tcb = KGS->pFirstTaskCtrl; tcb <= KGS->pLastTaskCtrl; ++tcb )
        {
            if ( !en88InitCoroutineTask(tcb) )
            {
                MSGD(( ERR_STARTING_TASK_THREADS1 ));
                MSGD(( ERR_STARTING_TASK_THREADS2 ));
            }
        }

        for ( ukt = KGS->first_kp ; ukt <= KGS->last_kp ; ukt ++ )
        {
            /*
            *  Create UKT thread
            */
            if ( XPARAM(fUseStackOnStack) )
            {
                sqlbeginthread( UKT_BASE_STACK_SIZE + ukt->stacksize,
                                en88RunCoroutineUKT,
                                ukt,
					            THR_CREATE_SUSPENDED_EO07,
                                & ukt->thread,
                                errtext,
                                &err );
            }
            else
            {
                sqlbeginthread( UKT_BASE_STACK_SIZE,
                                en88RunCoroutineUKT,
                                ukt,
					            THR_CREATE_SUSPENDED_EO07,
                                & ukt->thread,
                                errtext,
                                &err );
            }
            if ( err == THR_OK_EO07 )
            {
                ukt->tid = sqlthreadid(ukt->thread);
            }
            else
            {
                char uktName[40];
                sp77sprintf(uktName, sizeof(uktName), "UKT%d", ukt->index);
                MSGCD (( ERR_CREATE_THREAD , uktName, errtext.asCharp() )) ;
                return ( 3 );
            }

		    DBG1(( MF__,"UKT%d created, tid %ld", ukt->index, (long)ukt->tid ));

            if ( KGS->ti->ukt != ukt )
            {
                sqlresumethread( ukt->thread, errtext, &err );

                if ( err != THR_OK_EO07 )
                {
                    MSGCD (( ERR_RESUME_THREAD, ukt->index, errtext.asCharp() ));
                    return ( 3 );
                }
                for ( waitTime = MAX_UKT_START_WAIT_TIME;
                    (!ukt->started) && waitTime;
                    --waitTime )
                {
                    sleep(1); // sleep a second
                }
            }
        }
        /* resume ukt containing timer task */
        sqlresumethread( KGS->ti->ukt->thread, errtext, &err );

        if ( err != THR_OK_EO07 )
        {
            MSGCD (( ERR_RESUME_THREAD, ukt->index, errtext.asCharp() ));
            return ( 3 );
        }
    }
    else
    {
        for ( tcb = KGS->pFirstTaskCtrl; tcb <= KGS->pLastTaskCtrl; ++tcb )
        {
            if ( !en88_InitDynPthreadTasks(tcb) )
            {
                MSGD(( ERR_STARTING_TASK_THREADS1 ));
                MSGD(( ERR_STARTING_TASK_THREADS2 ));
                vabort(false);
            }
        }

        for ( ukt = KGS->first_kp ; ukt <= KGS->last_kp ; ukt ++ )
        {
            /* resume the first task thread inside each UKT */
            if ( KGS->ti->ukt != ukt )
            {
		        sqlresumethread( ukt->pFirstTaskCtrl->taskThread, errtext, &err );

                if ( err != THR_OK_EO07 )
                {
                    MSGCD (( ERR_RESUME_THREAD, ukt->index, errtext.asCharp() ));
                    return ( 3 );
                }

                for ( waitTime = MAX_UKT_START_WAIT_TIME;
                    (ukt->state != KT_SLEEPING) && waitTime;
                    --waitTime )
                {
                    sleep(1); // sleep a second
                }
            }

		    DBG1(( MF__,"UKT%d created, tid %ld", ukt->index, (long)ukt->tid ));

	    } /* for ukt <= last_kp */

        /* resume ukt containing timer task */
        sqlresumethread( KGS->ti->taskThread, errtext, &err );

        if ( err != THR_OK_EO07 )
        {
            MSGCD (( ERR_RESUME_THREAD, ukt->index, errtext.asCharp() ));
            return ( 3 );
        }
    }

#ifdef LINUX
    SetProcessorAffinity();
#endif
    DBGOUT;
    return ( 0 );
}

/*
 * ===========================================================================
 */
#undef  MF__
#define MF__ MOD__"en81_WaitForTermination"
static  void    en81_WaitForTermination ( void )
{
	DBGIN;

    do { /* If a Crash Signal was received, this will never terminate.... */
        sqlwaitsem(en81_TerminateCondition);
    } while ( RTESys_IsInCrashSignalHandler() != 0 );

#ifndef SDB_WITH_PROFILER
    /*
	 *  At this point we know, that a child terminated.
	 *  If the server was in normal operation,
	 *  then the death of a child cannot be accepted.
     */
    en81FinalAction();
#endif

    DBGOUT;
}

/*
 * ===========================================================================
 */
#undef  MF__
#define MF__ MOD__"en81_resume"
static void en81_resume ( struct TASK_TYPE * tcb )
{
    struct DOUBLY_LINKED            * lquu ;

	DBGIN;

    /*
     *  don't use vresume: it can be executed from UKPs only
     */
    lquu = GetTaskDLQElement1( tcb );
    lquu->req_type = REQ_VRESUME ;

    /*
     *  Enter queue element in IOC queue
     *  this is the only enque-procedure that may be used by other
     *  processes.......................
     */
    en71_iocEnquAndWake ( tcb->ukt, tcb , lquu );

	DBGOUT;
}

/*
 * ===========================================================================
 */
#undef MF__
#define MF__ MOD__"en81_StopDatabase"
extern "C" void en81_StopDatabase( int sig, siginfo_t *sigInfo, void *context )
{
    en81_TriggerWorker( &en81_StopWorker );
}

/*
 * ===========================================================================
 */
#undef MF__
#define MF__ MOD__"en81_StopAction"
static void en81_StopAction()
{
    teo07_ThreadErr err;
	ten50_UKT_Control *ukt ;

    MSGCD (( INFO_CAUGHT_STOP_SIGNAL )) ;

    if ( KGS != NULL )
	{
        if ( KGS->state == SERVER_ONLINE_EO00
          && KGS->gracefully 
          && RTE_SpawnRestartShutdown(false) )
        {
            return;
        }

        KGS->state = SERVER_STOP ;
		MSGD (( INFO_CHANGE_STATE, "STOP", KGS->state ));

		DBG1 (( MF__,"setting trace flag \n" ));
		if ( KGS->traceflag ) * KGS->traceflag = 1 ;
        if ( KGS->wantDump
          && KGS->dumpflag )
        {
            * KGS->dumpflag = 1;
    		DBG1 (( MF__,"setting dump flag \n" ));

            Msg_RegistryDump();
        }

        /*
		 *  Resume the bufwriter
		 */
		DBG1 (( MF__,"resuming bufwriter \n" ));
		en81_resume ( KGS->tw );
		MSGCD (( INFO_RESUMED_TW ));

        en81_NotifyCoordinator(0);
    }
    else
    {
        /* Very early stopped... */
        exit(0);
    }
}

/*
 * ===========================================================================
 */
#undef MF__
#define MF__ MOD__"en81Abort"
void en81Abort()
{
    (void) en81_SetDefaultAction( SIGIOT );
    (void) en81_SetDefaultAction( SIGBUS  );
    (void) en81_SetDefaultAction( SIGSEGV );
    (void) raise ( SIGIOT  );
    (void) raise ( SIGBUS  );
    (void) raise ( SIGSEGV );
    /* Should not be reached... */
    while( en81_neverStop )
    {
       pause();
    }
}

/*--------------------*/

#undef MF__
#define MF__ MOD__"en81_ForkAndExit"
/*ARGSUSED*/
extern "C" void en81_ForkAndExit( int sig, siginfo_t *sigInfo, void *context )
{
    /* Signal is redistributed to target thread */
    if ( en81_UktDumpWorker.running != 0 )
    {
        (void)en81_SetIgnoreAction( sig ) ;
        /*
         * Next signal will be send to the thread...
         * Yes I know there is a critical section, but this is for debugging only...
         */
        en81_TriggerWorker( & en81_UktDumpWorker );
        en81_WaitForWorker( & en81_UktDumpWorker );
    }
}

/*--------------------*/
/* Executed in special worker thread en81_UktDumpWorker!!! Not in signal handler !!! */
/* Otherwise the fork() would be able to block the complete process due to mutex usage. */
/* Before signal handler triggers this action, the KGS->coretaskindex is set (x_cons) */
/* Possible values: -1 dont care */
/*                   0 do profiling exit(1) */
/*                   1-ulNumOfTasks dont care */
/* Othervalues: do not do anything... */
/* The former meaning lost its substance, since now all tasks are threads and are dumped */
/* in there current state and at their current position */
#undef MF__
#define MF__ MOD__"en81_UktDumpAction"
static void en81_UktDumpAction()
{
    tsp00_Int4          taskIndex ;

    taskIndex = KGS->coretaskindex ;

    if ( taskIndex >= -1 /* -1 is legal !!! */
      && taskIndex <= KGS->ulNumOfTasks )
    {
	    pid_t childPid ;
        KGS->coretaskindex = 0 ;
        KGS->corethreadindex = 0 ;

		/*
		 *  Fork a son process.
		 */
		childPid = fork ();
	    if ( childPid )
		{
			/* FATHER */
		
			if ( childPid == (pid_t) -1 )
			{
	            MSGD (( ERR_FORK_ERR , sqlerrs() )) ;
			}
			else
			{
				/*
				 * The process waits for the child to die.
				 * While it waits, the context of its tasks
				 * cannot be changed.
				 */
				pid_t returnedChild;
				do {
					/* I nice side effect is that we 'eliminate' zombies of vexec() */
					returnedChild = wait( (int *)0 );
					if ( returnedChild != childPid )
					{
						/* some other thread could have snapped away that child */
						/* so we have to check whether we missed it... */
						returnedChild = waitpid( childPid, (int *)0, WNOHANG );
						/* returns 0 if childPid still exists, but is still running... */
					}
				} while ( returnedChild != (pid_t)-1
					  &&  returnedChild != childPid );
			}
			/* FATHER */
	        (void) en81_SetSigAction( SIG_FORK_AND_EXIT , en81_ForkAndExit );
			return ;
		}
	
		/* SON */
		if ( taskIndex == 0 )
		{
			/* force profiling output (monitor.out) */
			exit ( 1 );
		}
	
		/* dump process as is (don't change to a task) */
		/* With pthread used for task all tasks are automatically visible */
        en81_AllowCoreDump((tsp00_Bool *)0);
		en81Abort();
    }
}

/*--------------------*/

#undef MF__
#define MF__ MOD__"en81_dump_process_core"
void en81_dump_process_core()
{
    pid_t childPid ;

    childPid = fork ();
    if ( childPid )
    {
		/* FATHER */
		
		if ( childPid == (pid_t) -1 )
		{
            MSGD (( ERR_FORK_ERR , sqlerrs() )) ;
		}
		else
		{
            /*
             * The process waits for the child to die.
			 * While it waits, the context of its tasks
			 * cannot be changed.
		     */
            pid_t returnedChild;
            do {
                /* I nice side effect is that we 'eliminate' zombies of vexec() */
                returnedChild = wait( (int *)0 );
                if ( returnedChild != childPid )
                {
                    /* some other thread could have snapped away that child */
                    /* so we have to check whether we missed it... */
                    returnedChild = waitpid( childPid, (int *)0, WNOHANG );
                    /* returns 0 if childPid still exists, but is still running... */
                }
            } while ( returnedChild != (pid_t)-1
                  &&  returnedChild != childPid );
		}
		
		/* FATHER */
		return ;
    }
	
    /* SON */
    MSGALL (( INFO_COREHANDER_SON_FORCES_CORE )) ;
    en81_AllowCoreDump((tsp00_Bool *)0);
    en81Abort();
}

/*
 * ===========================================================================
 */

#if defined(LINUX)
#  include <linux/version.h>
#endif

/*
  LINUX_VERSION_CODE 132099 maybe to high, but the definite number is not known to me....

  This code is able to produce a stack backtrace even on these old LINUX machines we are currently
  using (Kernel 2.2.11-sap2_serial_sysrq). We have another test machine with a Kernel 2.4.4.
  From this newer machine i took the LINUX_VERSION_CODE. The reason for this effort is that
  even the gdb dumps a core on the old linux machine while trying to load our kernel....
  I took the inline assembler to get the current frame pointer and analyzed the stack frame
  with a memory dump (optimized and debug code). This way i found the offset for the 'sigcontext'
  and put it into the magic number below... I am not happy with this, but it is an old linux
  kernel anyhow... 6.2.2002 jrg
 */
#if defined(LINUX) && LINUX_VERSION_CODE < 132099
#  define LINUX_WITHOUT_SIGINFO
#else
#  undef  LINUX_WITHOUT_SIGINFO
#endif

extern "C" void en81_CrashSignalHandler( int sig, siginfo_t *sigInfo, void *context )
{
#undef MF__
#define MF__ MOD__"en81_CrashSignalHandler"

#if defined(LINUX) && defined(SDBonPPC64)
    /*
      LinuxOnPower needs a fixup of the signal frame context!
     */
    register unsigned long ** stack_pointer asm("r1");

    ucontext_t *pContext = (ucontext_t *)context;
    if ( pContext 
      && stack_pointer
      && *stack_pointer
      && **stack_pointer == 0 )
    {
        **stack_pointer = (unsigned long)pContext->uc_mcontext.regs->gpr[PT_R1];
    }
#endif

#ifdef LINUX_WITHOUT_SIGINFO
    /* in the old linux kernel 2.2.11 context was not usable at all.... */
    asm( "movl %%ebp,%0" : "=r" (context) : /* no input operand */ );
#endif /* LINUX_WITHOUT_SIGINFO */

    /* PTS 1112857 */
    if ( 2 <= en81_KernelGotCrashSignal )
	{
#if defined(HAS_CLONE_PROCESSES_INSTEAD_OF_THREADS)
        exit(EXIT_CODE_PANIC_SIGNAL);
#else
        _exit(EXIT_CODE_PANIC_SIGNAL);
#endif
	}
	++en81_KernelGotCrashSignal;
    if ( KGS )
    {
        KGS->gotCrashSignal = en81_KernelGotCrashSignal;
    }
    (void) en81_SetDefaultAction( sig     );
    (void) en81_SetDefaultAction( SIGIOT  );
    (void) en81_SetDefaultAction( SIGBUS  );
    (void) en81_SetDefaultAction( SIGSEGV );

    if ( KGS ) KGS->state = SERVER_KILL;

    RTESys_SetInCrashSignalHandler();

    MSGALL (( INFO_CAUGHT_ANY_SIGNAL, sig, en81_SigName(sig) )) ;
    MSGALL (( ERR_COREHANDER_ABORT, sig )) ;

    eo670_CTraceStackInit();
    eo670_CTraceContextStack(context);

    if ( sigInfo )
    {
        MSGALL (( N(330),ERR_TYPE,_T("COREHAND"),_T("dump of siginfo content:") ));
        MSGALL (( N(330),ERR_TYPE,_T("COREHAND"),_T(" signal     %u (%s)"), sigInfo->si_signo, en81_SigName(sigInfo->si_signo) ));
        MSGALL (( N(330),ERR_TYPE,_T("COREHAND"),_T(" code       %u (%s)"), sigInfo->si_code, en81_SigCode(sigInfo->si_code) ));
        MSGALL (( N(330),ERR_TYPE,_T("COREHAND"),_T(" errno      %u"), sigInfo->si_errno ));
        MSGALL (( N(330),ERR_TYPE,_T("COREHAND"),_T(" value(int) %u"), (unsigned long)sigInfo->si_value.sival_int ));
        MSGALL (( N(330),ERR_TYPE,_T("COREHAND"),_T(" value(ptr) @%p"), (unsigned long)sigInfo->si_value.sival_ptr ));
        MSGALL (( N(330),ERR_TYPE,_T("COREHAND"),_T(" pid        %lu"), (unsigned long)sigInfo->si_pid ));
        MSGALL (( N(330),ERR_TYPE,_T("COREHAND"),_T(" addr       @%p"), sigInfo->si_addr ));
    }

    if ( KGS ) LVCMem_SignalAnalysis();

    if ( KGS ) en81ShowMaxStackUse();

    if ( en50NoCoreDumpWanted == false )
    {
        /*
         * Terminated by Exception
         *
         * Do a core dump using default signal handler...
         *
         * NOTE: vabort handles its own 'core dump' fork.
         * Before it does this, it sets 'en50NoCoreDumpWanted' to true
         * This is done here too, to prevent overwriting first core...
         */
        en50NoCoreDumpWanted = true;
    }
    else
    {
#if defined(HAS_CLONE_PROCESSES_INSTEAD_OF_THREADS)
        exit(EXIT_CODE_PANIC_SIGNAL); /* Linux clone process would die very lonely otherwise... */
#else
        _exit(EXIT_CODE_PANIC_SIGNAL);
#endif
    }
}

/*------------------*/

#undef MF__
#define MF__ MOD__"en81FinalAction"
externC void en81FinalAction()
{
    en81_TriggerWorker( & en81_CrashWorker );
    en81_WaitForWorker( & en81_CrashWorker );
}

/*------------------*/

/*
 *  This function is used in emergency cases, to resume tracewriter while the
 *  process is still alive and wait for its termination.
 */
#undef MF__
#define MF__ MOD__"en81_CrashAction"
static void en81_CrashAction()
{
    teo07_ThreadErr ok;
    SAPDB_Int4      LastTWAlive;

	if ( KGS->state != SERVER_STOP
      && KGS->state != SERVER_KILL )
	{
		KGS->state = SERVER_KILL ;
		MSGD (( INFO_CHANGE_STATE, "KILL", KGS->state ));

		if ( en81_kill_database () != 0 )
        {
            return; /* Trace device not initialized. No need to wait for trace writer */
        }
	}

	/*
	 *  If the thread with the tracewriter terminated,
	 *  there is no use to wait for more processes.
	 */
    if ( en81_CheckTracewriterRunning() != 0 )
    {
    	MSGD (( INFO_CHANGE_STATE, "TRACE_WRITER_WAIT", KGS->state ));
    }

    while ( en81_CheckTracewriterRunning() != 0 )
    {
        LastTWAlive = KGS->tw->TaskAlive;

        DBG1 (( MF__,"COORDINATOR: waiting for thread termination\n" ));

        sqltimedwaitsem(en81_TerminateEndCondition, TRACE_WRITER_TIMEOUT_EN81, &ok );

        if ( (ok == THR_TIMEOUT_EO07) && (LastTWAlive == KGS->tw->TaskAlive) )
        {
    		MSGD (( INFO_CHANGE_STATE, "TRACE_WRITER_TIMEOUT", KGS->state ));
    		DBG1 (( MF__,"COORDINATOR: Timeout (%d seconds) waiting for trace writer\n",
                        TRACE_WRITER_TIMEOUT_EN81 ));
            break;
        }
    }
}

#undef MF__
#define MF__ MOD__"en81_HupSignaled"
extern "C" void en81_HupSignaled( int sig, siginfo_t *sigInfo, void *context )
{
#if defined(HAS_CLONE_PROCESSES_INSTEAD_OF_THREADS)
    exit(EXIT_CODE_HUP_SIGNAL);
#else
    _exit(EXIT_CODE_HUP_SIGNAL);
#endif
}

#undef MF__
#define MF__ MOD__"en81_SetSignalHandler"
static void en81_SetSignalHandler( tsp00_Bool suppressCore )
{
#ifdef USE_SIGALTSTACK
    stack_t newStack;
    newStack.ss_sp = ((char *)0) + ( ((&en81_AlternateStackSpace[0]
                                   - ((char *)0))+en81_MemPageSize-1 ) & ~(en81_MemPageSize-1) );
    newStack.ss_size = WANTED_SIGSTKSZ_EN81;
    newStack.ss_flags = 0;

    if ( sigaltstack(&newStack, 0) == -1 )
    {
       perror("sigaltstack failed:");
       exit(1);
    }
#endif /* USE_SIGALTSTACK */

    /*
     *  Initalize signal handling.
     *  These should not stop the kernel.
     */
    (void) en81_SetSigAction( SIGHUP  , en81_HupSignaled);
    (void) en81_SetIgnoreAction( SIGINT );
    (void) en81_SetIgnoreAction ( SIGCONT );
#if !defined(LINUX) && !defined(FREEBSD)
    (void) en81_SetIgnoreAction ( SIGSYS );
#endif /* LINUX */
    (void) en81_SetIgnoreAction ( SIGPIPE );
#ifdef SIGWINCH
    (void) en81_SetIgnoreAction ( SIGWINCH );
#endif /* SIGWINCH */
    (void) en81_SetIgnoreAction ( SIGXFSZ );

    /*
     *  SIGCLD is required for the wait system call in wait_for_termination.
     */
    (void) en81_SetDefaultAction ( SIGCLD );

    (void) en81_SetSigAction ( SIG_STOP_DATABASE , en81_StopDatabase );

    (void) en81_SetSigAction ( SIG_FORK_AND_EXIT , en81_ForkAndExit );

#ifndef CRASH_IMMEDIATLY
    if ( suppressCore )
    {
        /*
         *  These should crash the kernel.
         */
        (void) en81_SetSigAction ( SIGILL  , en81_CrashSignalHandler );
        (void) en81_SetSigAction ( SIGTRAP , en81_CrashSignalHandler );
        (void) en81_SetSigAction ( SIGABRT , en81_CrashSignalHandler );
        (void) en81_SetSigAction ( SIGFPE  , en81_CrashSignalHandler );
        (void) en81_SetSigAction ( SIGBUS  , en81_CrashSignalHandler );
        (void) en81_SetSigAction ( SIGSEGV , en81_CrashSignalHandler );
        (void) en81_SetSigAction ( SIGXCPU , en81_CrashSignalHandler );
#if defined(AIX)
/*AIX*/   (void) en81_SetSigAction ( SIGIOT  , en81_CrashSignalHandler );
#endif
#if !defined(LINUX) && !defined(FREEBSD)
/*!LINUX*/ (void) en81_SetSigAction ( SIGEMT  , en81_CrashSignalHandler );
/*!LINUX*/ (void) en81_SetSigAction ( SIGSYS  , en81_CrashSignalHandler );
#endif
    }
#endif /* !CRASH_IMMEDIATLY */

}

/*-----*/

void en81ShowMaxStackUse()
{
    if ( XPARAM(showMaxStackUse) )
    {
        struct TASK_TYPE *tcb;

        for ( tcb = KGS->pFirstTaskCtrl ; tcb <= KGS->pLastTaskCtrl; tcb++ )
        {
            vsShowMaxStackUse( tcb->index, eo92GetTaskTypeName(tcb->type) );
        }
    }
}

/*-----*/

#undef MF__
#define MF__ MOD__"en81_InitFirstDescriptors"
static void en81_InitFirstDescriptors(void)
{
    int devNullFd = 0;
    do
    {
        devNullFd = open("/dev/null", O_WRONLY);
    } while ( devNullFd >= 0 && devNullFd <= 2 );
    if ( devNullFd > 2 ) close(devNullFd);
}

#undef MF__
#define MF__ MOD__"en81RunKernel"
externC int en81RunKernel(struct ten50_KT_Control *pMainThread)
{
    int rc;

    DBGIN;

    rc = en81_CreateClockWorker();
    if ( rc != 0 )
    {
        return rc;
    }

    // the kernel process needs an own thread for I/O redirection: the thread in the 
    // watchdog process would be able to handle that, but there would be no synchro-
    // nization between the messages generated by this thread and the ones generated
    // in the kernel process
    RTEThread_IORedirection pipeReaderThread;

    Msg_IOutput::GetDiagnosticOutput();  // to enforce singleton initialization before starting other threads
    Msg_List messageList;
    if ( RTEThread_Thread::NoError !=  pipeReaderThread.Create( 0, messageList) )
    {
        Msg_List errList;
        Msg_IOutput::GetDiagnosticOutput().Output( messageList,errList );
    }
 
    MSGALL(( INFO_DB_START_SERVERDB, "+++++++++++++++++++++++++++++++++++++++++++++++++" ));
    MSGALL(( INFO_DB_START_SERVERDB, "+++++++++++++++++++ Kernel Forked +++++++++++++++" ));
    MSGALL(( INFO_DB_START_SERVERDB, en81_dbname.asCharp() ));
    MSGALL(( INFO_DB_START_PID, RTE_save_getpid() ));
    MSGALL(( INFO_DB_START_SERVERDB, "+++++++++++++++++++++++++++++++++++++++++++++++++" ));

    /*
     * Start in debugger if wanted and possible...
     */
    if ( en81_GetDebugEnv("SAPDB_DEBUG_RUN_KERNEL", NULL, 0) )
	{
        (void)en81CallDebugger();
    }

    rc = en81_InitEventing();
    if ( rc != 0 )
    {
        return ( rc );
    }

    KGS->traceWriterGone = 0;

    KGS->pid = RTE_save_getpid();
    if ( en41SetPIDInFile( en81_dbname, KGS->pid ) != 0 )
    {
        return 1;
    }

    if ( !RTEDiag_CreatedRegisteredSymbolResolutionLock() )
    {
        MSGD(( ERR_CANT_REGISTER_SYMBOL_RESOLUTION_LOCK ));
    }

    /*
     * Initialize I/O Page allocator using XParam values
     */
    rc = en81_InitializePageAllocator(KGS->XParam);
    if ( rc != 0 )
    {
        return rc;
    }

    /*
     * Prepare and allocate task stacks
     */
    en88CreateAndPrepareTaskStacks();

	/*
	 *  COORDINATOR process is already running. Initialize its data.
	 */
	KGS->coord.start_time = pMainThread->start_time;
	KGS->coord.thread = pMainThread->thread;
	KGS->coord.tid = pMainThread->tid;

    DBG1 (( MF__,"COORDINATOR: starting with tid %ld \n", (long)KGS->coord.tid ));

    /* PTS 1103824
     *  Create the MessageBuffer Worker thread
     */
    eo15InitMessageBufferSpace( XPARAM(ulMaxMessageFiles),
                                    (teo15MessageBufferSpace *)KGS->MessageBufferSpace );

    /* PTS 1108768 Space for Tracebuffer used for InMemory tracing */
    Trace_InfoPageSetup();

    rc = en81_CreateRteProcs ();

    /*
     *  Create ukp processes
     */
    if ( ! rc )
    {
		rc = en81_CreateUkts ( );
    }

    /*
     *  Wait for termination of the database
     */
    if ( ! rc )
    {
		en81_WaitForTermination ( );
    }

    /* If enabled show max stack usage */
    en81ShowMaxStackUse();

    if ( KGS->state != SERVER_STOP && KGS->state != SERVER_KILL )
    {
        MSGALL (( ERR_RTE_ABORT_EXEPT ));
    }

    /*
     *  Issue stop message
     */
    if ( KGS->state != SERVER_STOP )
    {
        MSGALL (( INFO_KERNEL_ABORTS ));
		rc = 2 ;
    }
    else
    {
        KGS->traceNeedsSaving = 0;

        MSGALL (( INFO_DB_STOPPED ));

		rc = 0 ;
    }

	DBG1 (( MF__,"returning %d \n", rc ));
    DBGOUT;

    return ( rc );
}

/*
 * Supplying a 'SAPDB_DEBUG_CMD' will have the effect, that a call to this
 * function leads in executing a command prepared using this environment value
 * as format string for sprintf() with argument current pid (returned by RTE_save_getpid)
 *
 * i.e.: on AIX   SAPDB_DEBUG_CMD ="xterm -e dbx -a %d"
 *       on HP                     "xterm -e dde -ui line -attach %d path_to_exe"
 *       on SUN                    "xterm -e dbx path_to_exe %d"
 *       on SOLARIS                "xterm -e dbx path_to_exe %d"
 *       on DEC                    "xterm -e ladebug -pid %d path_to_exe"
 *                              or "xterm -e dbx path_to_exe %d"
 *       on SNI                    "xterm -e dbx %d"
 *       on LINUX                  "xterm -e gdb path_to_exe %d"
 *       on FREEBSD                "xterm -e gdb path_to_exe %d"
 *
 * This function is i.e. called inside of vabort()...
 */
#undef MF__
#define MF__ MOD__"en81CallDebugger"
externC int en81CallDebugger()
{
#define DEBUG_COMMAND_BUFFER_MXEN81 (300)

static char  DebugCall[DEBUG_COMMAND_BUFFER_MXEN81+12+sizeof(tsp00_Pathc)];
static char  DebugCmd[DEBUG_COMMAND_BUFFER_MXEN81];

    /*
     * Check Environment and check length of string found (need space for pid too)!
     */
    if ( en81_GetDebugEnv("SAPDB_DEBUG_CMD", DebugCmd, DEBUG_COMMAND_BUFFER_MXEN81 ) )
    {
        int i;
        int pidFound = 0;
        int pathFound = 0;    /* PTS 1110474 */

        for ( i = 0; i < strlen(DebugCmd); i++ )
        {
            if ( DebugCmd[i] == '%' )
            {
                if ( DebugCmd[i+1] == 'd' )
                {
                    if ( pidFound != 0 )
                    {
                        /* only one process id allowed */
                        return 0;
                    }
                    pidFound = i+1;
                } else if ( DebugCmd[i+1] == 's' )
                {
                    if ( pathFound != 0 )
                    {
                        /* only one path specifier allowed */
                        return 0;
                    }
                    pathFound = i+1;
                } else if ( DebugCmd[i+1] != '%' )
                {
                    /* Illegal format identifier */
                    return 0;
                }
            }
        }

        if ( pathFound != 0 && 0 == en81_pathToExecutable[0] )
        {
            /* Path was not stored, but given... */
            return 0;
        }

        if ( 0 != pidFound && 0 != pathFound )
        {
            if ( pidFound > pathFound )
            {
                sp77sprintf( DebugCall, sizeof(DebugCall), DebugCmd, &en81_pathToExecutable[0], RTE_save_getpid() );
            }
            else
            {
                sp77sprintf( DebugCall, sizeof(DebugCall), DebugCmd, RTE_save_getpid(), &en81_pathToExecutable[0] );
            }
        } else if ( 0 != pidFound )
        {
            sp77sprintf( DebugCall, sizeof(DebugCall), DebugCmd, RTE_save_getpid() );
        } else if ( 0 != pathFound )
        {
            sp77sprintf( DebugCall, sizeof(DebugCall), DebugCmd, &en81_pathToExecutable[0] );
        } else
        {
            strcpy( DebugCall, DebugCmd );
        }

        system(DebugCall);
        /* stop at least the current thread... Debugger should have taken over then... */
        return 1;
    }
    else
    {
        return 0;
    }
}

/*---------------------------------------------------------------------------*/

#undef MF__
#define MF__ MOD__"en81_SigName"
static const char *en81_SigName(int sigCode)
{
    switch( sigCode )
    {
    case SIGHUP: /* 1 */
        return "SIGHUP";
    case SIGINT: /* 2 */
        return "SIGINT";
    case SIGQUIT: /* 3 */
        return "SIGQUIT";
    case SIGILL: /* 4 */
        return "SIGILL";
    case SIGTRAP: /* 5 */
        return "SIGTRAP";
    case SIGABRT: /* 6 */
        return "SIGABRT";
    case SIGBUS: /* 7 */
        return "SIGBUS";
    case SIGFPE: /* 8 */
        return "SIGFPE";
    case SIGKILL: /* 9 */
        return "SIGKILL";
    case SIGUSR1: /* 10 */
        return "SIGUSR1";
    case SIGSEGV: /* 11 */
        return "SIGSEGV";
    case SIGUSR2: /* 12 */
        return "SIGUSR2";
    case SIGPIPE: /* 13 */
        return "SIGPIPE";
    case SIGALRM: /* 14 */
        return "SIGALRM";
    case SIGTERM: /* 15 */
        return "SIGTERM";
    case SIGCLD: /* 16 */
        return "SIGCLD";
    case SIGCONT: /* 17 */
        return "SIGCONT";
    case SIGSTOP: /* 18 */
        return "SIGSTOP";

    case SIGXCPU: /* 24 */
        return "SIGXCPU";
    case SIGXFSZ: /* 25 */
        return "SIGXFSZ";

#ifdef SIGWINCH
    case SIGWINCH: /* 28 */
        return "SIGWINCH";
#endif /* SIGWINCH */

#if !defined(LINUX) && !defined(FREEBSD)
    case SIGSYS:
        return "SIGSYS";
    case SIGEMT:
        return "SIGEMT";
#endif /* !LINUX */

    default:
        break;
    }
    return "SIGUNKNOWN";
}

/*---------------------------------------------------------------------------*/

#undef MF__
#define MF__ MOD__"en81_SigCode"
static const char *en81_SigCode(int sigCode)
{
    switch( sigCode )
    {
#if defined(ILL_ILLOPC)
    case ILL_ILLOPC:
        return "ILL_ILLOPC:illegal opcode";
#endif
#if defined(ILL_ILLOPN)
    case ILL_ILLOPN:
        return "ILL_ILLOPN:illegal operand";
#endif
#if defined(ILL_ILLADR)
    case ILL_ILLADR:
        return "ILL_ILLADR:illegal address";
#endif
#if defined(ILL_ILLTRP)
    case ILL_ILLTRP:
        return "ILL_ILLTRP:illegal trap";
#endif
#if defined(ILL_PRVOPC)
    case ILL_PRVOPC:
        return "ILL_PRVOPC:priveleged opcode";
#endif
#if defined(ILL_PRVREG)
    case ILL_PRVREG:
        return "ILL_PRVREG:priveleged register";
#endif
#if defined(ILL_COPROC)
    case ILL_COPROC:
        return "ILL_COPROC:coprocessor error";
#endif
#if defined(ILL_BADSTK)
    case ILL_BADSTK:
        return "ILL_BADSTK:internal stack error";
#endif
    default:
        break;
    }
    return "unknown";
}

/*---------------------------------------------------------------------------*/

#undef MF__
#define MF__ MOD__"en81_LastWords"
void en81_LastWords(int runKernelExitCode, int isKilled)
{
    int lastFd;

    (void)en41RemovePipeFDFile(en81_dbname);

    /* Reopen message file for Kernel ExitCode Information */
    if ( 0 == runKernelExitCode && !isKilled )
    {
        MSGD(( INFO_KERNEL_EXIT_NORMAL ));
    }
#ifndef SDB_WITH_PROFILER
    else if ( 0 == runKernelExitCode && isKilled )
    {
        MSGD(( ERR_KERNEL_EXITED_BY_SIGNAL, 0, "Killed after timeout with state SERVER_KILL"));
    }
    else if ( KERNEL_TERMINATED_BY_HUP_SIGNAL == runKernelExitCode )
    {
        MSGD (( ERR_KERNEL_EXITED_BY_HUP ));
    }
    else
    {
        /* If Kernel failed, look for an existing rtedump.
         * If nothing or an old version found, dump it from shared memory segment
         * In case of crash the dump may not have been written... */
        en54DumpNewRteDumpOnly();

#ifndef WCOREDUMP
#define WCOREDUMP(e_) (((e_) & 0200) == 0200)
#endif
        /* Show exit code with/without core */
        if ( WCOREDUMP(runKernelExitCode) )
        {
            system( "chmod g+r core*" ); /* make core readable for sdba group */
            MSGD(( ERR_KERNEL_EXIT_WITH_CORE, runKernelExitCode ));
        }
        else
        {
            MSGD(( ERR_KERNEL_EXIT_WITHOUT_CORE, runKernelExitCode ));
        }

        /* Show signal, if signal was received */
        if ( WIFSIGNALED(runKernelExitCode) )
        {
            int sigCode = WTERMSIG(runKernelExitCode);

            if ( KGS )
            {
                if ( !XPARAM(suppressCore) )
                {
                    MSGALL (( ERR_KERNEL_STACK_BACKTRACE_SUPPRESSED )) ;
                }
                else if ( 0 == KGS->gotCrashSignal )
                { /* signal handler not suppressed, but signal handler not called... */
                    MSGD(( ERR_KERNEL_INDICATES_STACK_OVERFLOW ));                
                }
            }

            MSGD(( ERR_KERNEL_EXITED_BY_SIGNAL, sigCode, en81_SigName(sigCode) ));
        }

        if ( WIFEXITED(runKernelExitCode) )
        {
            if ( EXIT_CODE_HUP_SIGNAL == WEXITSTATUS(runKernelExitCode) )
            {
                MSGD (( ERR_KERNEL_EXITED_BY_HUP ));
            }
            else if ( EXIT_CODE_PANIC_SIGNAL == WEXITSTATUS(runKernelExitCode) )
            {
                MSGD (( ERR_KERNEL_EXITED_BY_PANIC ));
            }
            else
            {
                MSGD (( ERR_KERNEL_EXIT_CODE, WEXITSTATUS(runKernelExitCode) ));
            }
        }
    }
#else /* SDB_WITH_PROFILER */
    else
    {
        MSGD (( ERR_KERNEL_EXIT_CODE, runKernelExitCode ));
    }
#endif /* SDB_WITH_PROFILER */

    if ( KGS )
    {
        /* --- Reset of Crash-Semaphore in case of a normal shutdown */
        /* PTS 1115180, 1116362 */
        if (!KGS->dumpDiagFiles && !isKilled )
        {
            RTEDiag_SetCrashSem (CRASH_SEM_RESET, KGS->serverdb);
        }
        else /* 1116365 */
        {
            MSGD(( INFO_DIAG_HIST_FORCED ));
        }


        /* PTS 1108768 */
        /* If tracewriter was not able to write its last pages, dump them out */
        if ( KGS->traceNeedsSaving )
        {
            /*
             *  Trace pages may have to be dumped in 'knltrace'.
             */
            tsp00_Int4 PageSize;
            char *kernelTraceName;
            int   kernelTraceFd;

            MSGD (( INFO_TRACEWRITER_EMERGENCY ));

            kernelTraceName = XPARAM(szTraceDevspace);

            PageSize = XPARAM(ulDBPageSize);

            kernelTraceFd = open(kernelTraceName, O_RDWR);
            if ( kernelTraceFd < 0 )
            {
                MSGD (( ERR_OPEN, kernelTraceName, errno ));
            }
            else
            {
                /* Skip Page0 */
              if ( lseek(kernelTraceFd, PageSize, SEEK_SET) != PageSize )
              {
                MSGD (( ERR_OPEN, kernelTraceName, errno ));
              }
              else
              {
                /* Dumping Tracebuffer as one */
                while ( write(kernelTraceFd, KGS->traceBufferSpace, KGS->totalTraceBufferSizeInPages * PageSize) == -1 )
                   if ( errno != EINTR ) break;
              }
              close(kernelTraceFd);
            }
        }

        /* PTS 1103824
         * Complete any not yet completed COM Message files
         */
        eo15CompleteMessageFiles( XPARAM(ulMaxMessageFiles),
                                        (teo15MessageBufferSpace *)KGS->MessageBufferSpace );

	    /* PTS 1113008
	      Since kernel does no longer inform the clients, they are informed now....
	     */
        en75FinalReleaseConnections(isKilled ? 1 : runKernelExitCode);

#ifndef SDB_WITH_PROFILER
        /*
         *  The database is down, kill the remnants.
         */
        sql45_killall ( KGS );
#endif /* SDB_WITH_PROFILER */

        MSGD (( INFO_CHANGE_STATE, "OFFLINE ", KGS->state ));
    }
    else
    {
        MSGD (( INFO_CHANGE_STATE, "OFFLINE ", 0 ));
    }
#ifndef NO_OLD_DIAG_FILES
    RTE_WriteDatabaseStopTime();
#endif
#ifdef NEW_DIAG_FILES
    Msg_WriteStopMessage();
#endif
}

/*------------------------------*/

#undef MF__
#define MF__ MOD__"en81_InitEventing"
static int en81_InitEventing()
{
    int rc = NO_ERROR_EO001;
    tsp00_Int4 spaceNeeded = 0;

    if ( (XPARAM(szEventFile)[0] != '\0')
      && (XPARAM(ulEventSize) > 0) )
    {
        RTE_Path pathToEventFile;

        sp77sprintf(&pathToEventFile[0], sizeof(RTE_Path), "%s", XPARAM(szEventFile).asCharp() );
        MSGD (( INFO_EVENT_FILE, &pathToEventFile[0],
                                 XPARAM(ulEventSize) ));
        rc = en60OpenEventFile( pathToEventFile,
                                XPARAM(ulEventSize) );
        if (rc != NO_ERROR_EO001)
        {
            MSGD (( ERR_CANT_OPEN_FILE, XPARAM(szEventFile).asCharp(), rc ));
            return( rc );
        }
    }
    else
    {
        MSGD (( INFO_EVENT_NO_FILE, XPARAM(szEventFile).asCharp(), XPARAM(ulEventSize) ));
    }

    eo67InitializeEventInfo( );

    KGS->Eventing = NULL;
    spaceNeeded = eo67SpaceNeeded( XPARAM(ulMaxEventTasks), XPARAM(ulMaxEvents) );
    if ( spaceNeeded > 0 )
    {
        MSGD (( INFO_EVENT_RINGBUFFER, XPARAM(ulMaxEventTasks), XPARAM(ulMaxEvents) ));
        if ( (rc = ALLOC_MEM_EO57 (&KGS->Eventing, spaceNeeded )) != NO_ERROR_EO001)
        {
            DBGOUT;
            return ( rc );
        }

        SAPDB_memset(KGS->Eventing, 0, spaceNeeded);

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

    return( NO_ERROR_EO001 );
}

/*------------------------------*/

/*
 * File to allow dynamically specified debug information
 */
#undef MF__
#define MF__ MOD__"en81_GetDebugEnv"
static int en81_GetDebugEnv(const char *Variable, char *ResultBuffer, const size_t MaxLength)
{
    int CheckIndex;
    int Found;
    int Fd;
#define FILE_CONTENT_MXEN81 8192
    char FileContent[FILE_CONTENT_MXEN81];
    char *VariablePtr;
    int FileSize;
    size_t CheckLength;

    memset( &FileContent[0], 0, FILE_CONTENT_MXEN81);

    /* Variable must be set */
    if ( Variable == NULL )
    {
        return 0;
    }

    /* Variable must be non empty string */
    CheckLength = strlen(Variable);
    if ( CheckLength == 0 )
    {
        return 0;
    }

    /* File must exist */
    Fd = open( SAPDB_Debug_Env, O_RDONLY);
    if ( Fd < 0 )
    {
        return 0;
    }

    /* Filesize is strongly limited */
    FileSize = read(Fd, FileContent, FILE_CONTENT_MXEN81-1);
    if ( FileSize < 0 )
    {
        close(Fd);
        return 0;
    }

    VariablePtr = FileContent;

    /* Search Variable in Memory buffer. Set Found if Found */
    for ( Found = 0; *VariablePtr != '\0' && Found == 0; )
    {
            /* Skip Whitespace */
        while ( *VariablePtr == '\t'
             || *VariablePtr == ' '
             || *VariablePtr == '\n' )
             ++VariablePtr;

            /* Skip Comment */
        if ( *VariablePtr == '#' )
        {
            while ( *VariablePtr != '\0'
                 && *VariablePtr != '\n' )
                ++VariablePtr;
            continue;
        }

            /* Comparison ignoring case */
        for ( CheckIndex = 0; CheckIndex < CheckLength; CheckIndex++ )
        {
            if ( toupper(Variable[CheckIndex]) != VariablePtr[CheckIndex] )
                break;
        }

        if ( CheckIndex == CheckLength )
        {
            Found = 1;
            /* Allow ResultBuffer not to be specified ... */
            if ( ResultBuffer != NULL
              && MaxLength > 0 )
            {
                VariablePtr += CheckIndex;

                    /* Skip Whitespace */
                while ( *VariablePtr != '\0'
                     && *VariablePtr != '\n'
                     && ( *VariablePtr == '\t' || *VariablePtr == ' ' ) )
                        ++VariablePtr;

                    /* Allow equal sign
                       and do NOT SKIP whitespace after equal sign.
                       This allows to define string beginning with white spaces
                     */
                if ( *VariablePtr == '=' )
                {
                     ++VariablePtr;
                }

                for ( CheckIndex = 0; *VariablePtr != '\n'
                                   && CheckIndex < MaxLength; CheckIndex++ )
                {
                        /* Copy DOES NOT ignore case */
                        ResultBuffer[CheckIndex] = *VariablePtr++;
                }
                ResultBuffer[CheckIndex] = 0;
            }
        }
        else
        {
            /* Skip this Variable */
            while ( *VariablePtr != '\0'
                 && *VariablePtr != '\n' )
                ++VariablePtr;
            continue;
        }
    }

    close(Fd);
    return Found;
}

/*------------------------------*/
/* PTS 1105262 / PTS 1105263 */
static int en81_SetSigAction(int sig, signalHandler handler)
{
  struct sigaction sa;

  sa.sa_sigaction = handler;
  sigemptyset( &sa.sa_mask );
#ifdef USE_SIGALTSTACK
  sa.sa_flags = SA_SIGINFO|SA_ONSTACK;
#else
  sa.sa_flags = SA_SIGINFO;
#endif
  return sigaction( sig, &sa, NULL);
}

static int en81_SetDefaultAction(int sig)
{
  struct sigaction sa;

  sa.sa_handler = SIG_DFL;
  sigemptyset( &sa.sa_mask );
  sa.sa_flags = 0;
  return sigaction( sig, &sa, NULL);
}

static int en81_SetIgnoreAction(int sig)
{
  struct sigaction sa;

  sa.sa_handler = SIG_IGN;
  sigemptyset( &sa.sa_mask );
  sa.sa_flags = 0;
  return sigaction( sig, &sa, NULL);
}
/*------------------------------*/
/* PTS 1113405 */
static void en81_SuppressCoreDump()
{
    struct rlimit limit;
    int settingOk = 0;

    en50NoCoreDumpWanted = true;
    if ( getrlimit(RLIMIT_CORE, &limit) < 0 )
    {
        MSGD(( ERR_GETRLIMIT_SYSCALL, "CORE", sqlerrs() ));
    }
    else
    {
        if ( limit.rlim_max != RLIM_INFINITY )
        {
            MSGD(( WRN_USER_LIMITED_CORE_DUMP ));
        }

        limit.rlim_cur = 0;
        if ( setrlimit(RLIMIT_CORE, &limit) < 0 )
        {
            MSGD(( ERR_SETRLIMIT_SYSCALL, "CORE", sqlerrs() ));
        }
        else
        {
            settingOk = 1;
        }
    }

    if ( !settingOk )
    {
        MSGD(( WRN_CANT_SUPPRESS_CORE_DUMP ));
    }
    else
    {
        MSGD(( INFO_CORE_DUMP_SUPPRESSED ));
    }
}

/*
  Function: en81_AllowCoreDump
  Description: Allowing core dump creation by setting softlimit to hard limit

  If hard limit is not set to infinity, core dump is still limited to user
  enforced limit
 */
static void en81_AllowCoreDump(tsp00_Bool *pSuppress)
{
    struct rlimit limit;
    int settingOk = 0;

    if ( getrlimit(RLIMIT_CORE, &limit) < 0 )
    {
        MSGD(( ERR_GETRLIMIT_SYSCALL, "CORE", sqlerrs() ));
    }
    else
    {
        if ( limit.rlim_max != RLIM_INFINITY )
        {
            MSGD(( WRN_USER_LIMITED_CORE_DUMP ));
        }
        else
        {
            if ( limit.rlim_cur != limit.rlim_max )
            {
                limit.rlim_cur = limit.rlim_max;

                if ( setrlimit(RLIMIT_CORE, &limit) < 0 )
                {
                    MSGD(( ERR_SETRLIMIT_SYSCALL, "CORE", sqlerrs() ));
                }
                else
                {
                    settingOk = 1;
                }
            }
            else
            {
                settingOk = 1;
            }
        }
    }

    if ( !settingOk )
    {
        MSGD(( WRN_CANT_ALLOW_CORE_DUMP ));
        if ( pSuppress )
        {
            *pSuppress = true;
        }
        en50NoCoreDumpWanted = true;
    }
    else
    {
        MSGD(( INFO_CORE_DUMP_NOT_SUPPRESSED ));
        en50NoCoreDumpWanted = false;
    }
}

/*---------------------------------------------------------------------------
  Function: en81_InitializePageAllocator
  Description: Allocate I/O Pages according to XParam values

  XParam->ulDBPageSize (_PAGE_SIZE) specifies database page size

  XParam->cacheSize (CACHE_SIZE) if > 0 only value used
   else
  XParam->lDataCachePages (DATA_CACHE) specifies total number of data cache pages
  XParam->lConvCachePages (CONVERTER_CACHE) specifies total number of converter cache pages

  Arguments: XParam [in] pointer to parameter structure
  Return value: 0 if sucessfully initialized
                2 if adding failed
 */
static int en81_InitializePageAllocator(RTE_XPARAM_REC *XParam)
{
    SAPDB_UInt4 wantedPages;

    wantedPages = XPARAM(cacheSize);

    /* New allocation with single cache */
    if ( !RTEMem_InitializePageAllocator(XPARAM(ulDBPageSize), wantedPages) )
    {
      MSGD (( ERR_NOT_ENOUGH_MEMORY, "I/O buffer cache", (wantedPages * XPARAM(ulDBPageSize)) / 1024 ))
      return ( 2 );
    }

    MSGD (( INFO_INITSHM_MEM_ALLOCATED, wantedPages, "I/O buffer cache" ))

    return 0;
}

/*
 * PTS 1116686
 */

/* If size equals unlimited, write "unlimited" to valueString, else write size to valueString.  */
/* If inMB is true, divide size by (1024*1024) before and add "MB" to valueString               */
static void en81WriteResourceLimitToString(SAPDB_UInt8 size,SAPDB_Char *valueString,SAPDB_Bool inMB,SAPDB_UInt8 unlimited)
{
    if(unlimited == size)
    {
        sp77sprintf(valueString,256,"unlimited");
    }
    else
    {
        if(inMB)
        {
            sp77sprintf(valueString,256,"%ld MB",size / (1024*1024));
        }
        else
        {
            sp77sprintf(valueString,256,"%ld",size);
        }
    }
}

/*
   @brief Show key=value strings sorted in lexical order
   @param envp [in] key=value array 
 */
static void ShowSortedEnvironmentStringValuePairs(char *envp[])
{
    MSGD(( INFO_ENVIRONMENT_DUMP_START ));

    char *lastShown = 0;
    char *pEnvToShow = 0;
    
    pEnvToShow = ( 0 != envp ? envp[0] : 0 );

    while ( pEnvToShow )
    {
        if ( !lastShown )
        {
            /* search smallest first */
            for ( char **ppEnv = &envp[1]; *ppEnv; ++ppEnv )
            {
                for ( int iEqual = 0;
                      0 != pEnvToShow[iEqual]
                   && 0 != (*ppEnv)[iEqual];
                      ++iEqual )
                {
                    if ( '=' == pEnvToShow[iEqual] )
                    {
                        break; /* pEnvToShow name is shorter -> show first */
                    }
                    if ( '=' == (*ppEnv)[iEqual] )
                    {
                        pEnvToShow = *ppEnv; /* found shorter name in list -> show first */
                        break;
                    }
                    if ( toupper((*ppEnv)[iEqual])  < toupper(pEnvToShow[iEqual]) )
                    {
                        pEnvToShow = *ppEnv; /* found name lexicographic smaller -> show first */
                        break;
                    }
                    else if ( toupper((*ppEnv)[iEqual]) > toupper(pEnvToShow[iEqual]) )
                    {
                        /* found name already lexicographic smaller -> show first */
                        break;
                    }
                }
            }
        }
        else
        {
            pEnvToShow = 0;

            for ( char **ppEnv = envp; *ppEnv; ++ppEnv )
            {
                if ( lastShown != *ppEnv )
                {
                    int iEqual;
                    bool isCandidate = false;

                    /* if *ppEnv is smaller than lastShown, it is already shown */
                    for ( iEqual = 0; 0 != (*ppEnv)[iEqual]; ++iEqual )
                    {
                        if (  '=' == (*ppEnv)[iEqual]
                           || toupper((*ppEnv)[iEqual] < lastShown[iEqual]) )
                        {
                            /* found shorter name in list -> already shown */
                            break;
                        }
                        else if ( '=' == lastShown[iEqual]
                               || toupper((*ppEnv)[iEqual] > lastShown[iEqual]) )
                        {
                            isCandidate = true;
                            break;
                        }
                    }

                    if ( isCandidate )
                    {
                        /* if *ppEnv is larger than lastShown */
                        for ( int iEqual = 0; 0 != (*ppEnv)[iEqual]; ++iEqual )
                        {
                            if ( 0 == pEnvToShow )
                            {
                                pEnvToShow = *ppEnv; /* new candidate choosen */
                                break;
                            }

                            if ( '=' == (*ppEnv)[iEqual] 
                              || toupper((*ppEnv)[iEqual] < pEnvToShow[iEqual]) )
                            {
                                /* found shorter name in list -> new candidate */
                                pEnvToShow = *ppEnv;
                                break;
                            }

                            if ( '=' == pEnvToShow[iEqual] 
                                 || toupper((*ppEnv)[iEqual] > pEnvToShow[iEqual]) )
                            {
                                /* candidate already smaller */
                                break;
                            }
                        }
                    }
                }
            }
        }

        if ( 0 != pEnvToShow )
        {
#define MAX_ENV_PART 114
            char  EnvBuffer[MAX_ENV_PART+2];
            char  const *pEnvPart;
            int   partLen = 0;

            for ( int iLen = 0; partLen >= 0; ++iLen )
            {
                switch( pEnvToShow[iLen] )
                {
                case '\n':
                    if ( partLen >= (MAX_ENV_PART-1) )
                    {
                        EnvBuffer[partLen] = 0;
                        MSGD(( INFO_ENVIRONMENT_VARIABLE, &EnvBuffer[0] ))
                        EnvBuffer[0] = ' ';
                        partLen = 1;
                    }
                    /* substitute '\n' by '\\' 'n' */
                    EnvBuffer[partLen] = '\\';
                    ++partLen;
                    EnvBuffer[partLen] = 'n';
                    ++partLen;
                    break;
                case 0:
                    EnvBuffer[partLen] = 0;
                    MSGD(( INFO_ENVIRONMENT_VARIABLE, &EnvBuffer[0] ))
                    partLen = -1; /* end of outer loop condition */
                    break;
                default:
                    if ( partLen >= MAX_ENV_PART )
                    {
                        EnvBuffer[MAX_ENV_PART] = 0;
                        MSGD(( INFO_ENVIRONMENT_VARIABLE, &EnvBuffer[0] ))
                        EnvBuffer[0] = ' ';
                        partLen = 1;
                    }
                    EnvBuffer[partLen] = pEnvToShow[iLen];
                    ++partLen;
                    break;
                }

            }

            lastShown = pEnvToShow;
        }
    }

    MSGD(( INFO_ENVIRONMENT_DUMP_COMPLETED ));
}

/* Give a little information about the currently configured resource limits. */
/* Compare the amount of memory needed by the data cache and oms heap to the */
/* available heap and virtual address space and give a warning if the memory */
/* size needed exceeds one of them. */
static void en81OutputResourceLimits(RTE_XPARAM_REC *XparamParameters  , char *envp[] )
{
    SAPDB_UInt8 memoryNeeded;
    SAPDB_UInt8 virtualAddressSpace;
    SAPDB_UInt8 heapAvailable;
    SAPDB_Char value[256];
    struct rlimit limit;

    getrlimit(RLIMIT_CPU,&limit);
    en81WriteResourceLimitToString((SAPDB_UInt8)limit.rlim_cur,value,false,RLIM_INFINITY);
    MSGD ((INFO_MAX_CPU_TIME,value));
#ifdef RLIMIT_NPROC
    getrlimit(RLIMIT_NPROC,&limit);
    en81WriteResourceLimitToString((SAPDB_UInt8)limit.rlim_cur,value,false,RLIM_INFINITY);
    MSGD ((INFO_MAX_NUMBER_OF_PROCESSES,value));
#endif
    getrlimit(RLIMIT_CORE,&limit);
    en81WriteResourceLimitToString((SAPDB_UInt8)limit.rlim_cur,value,true,RLIM_INFINITY);
    MSGD ((INFO_MAX_CORE_SIZE,value));

    getrlimit(RLIMIT_NOFILE,&limit);
    en81WriteResourceLimitToString((SAPDB_UInt8)limit.rlim_cur,value,false,RLIM_INFINITY);
    MSGD ((INFO_MAX_OPEN_FILES,value));
#ifdef RLIMIT_FSIZE
/*
http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1125631
*/
    getrlimit(RLIMIT_FSIZE,&limit);
    en81WriteResourceLimitToString((SAPDB_UInt8)limit.rlim_cur,value,true,RLIM_INFINITY);
    MSGD ((INFO_MAX_FILESIZE,value));
    /* if the file size is limited, check the configured volume sizes against this limit */
    if(RLIM_INFINITY != limit.rlim_cur)
    {   /* check all volume types, avoiding a dirty '++' on an enum */
        if(!(  RTEConf_CheckVolumeSize(RTE_VolumeTypeSystem,XparamParameters,limit.rlim_cur)
            && RTEConf_CheckVolumeSize(RTE_VolumeTypeLog,XparamParameters,limit.rlim_cur)
            && RTEConf_CheckVolumeSize(RTE_VolumeTypeMirrorLog,XparamParameters,limit.rlim_cur)
            && RTEConf_CheckVolumeSize(RTE_VolumeTypeData,XparamParameters,limit.rlim_cur)
            && RTEConf_CheckVolumeSize(RTE_VolumeTypeTrace,XparamParameters,limit.rlim_cur) ))
        {
            vabort(false);
        }
    }
#endif
    en81WriteResourceLimitToString(sysconf(_SC_THREAD_THREADS_MAX),value,false,RLIM_INFINITY);
    MSGD ((INFO_MAX_THREADS,value));

    getrlimit(RLIMIT_STACK,&limit);
    en81WriteResourceLimitToString((SAPDB_UInt8)limit.rlim_cur,value,true,RLIM_INFINITY);
    MSGD ((INFO_STACK_SIZE,value));

#ifdef RLIMIT_MEMLOCK
    getrlimit(RLIMIT_MEMLOCK,&limit);
    en81WriteResourceLimitToString((SAPDB_UInt8)limit.rlim_cur,value,true,RLIM_INFINITY);
    MSGD ((INFO_LOCKABLE_MEMORY_SIZE,value));
#endif
    getrlimit(RLIMIT_AS,&limit);
    virtualAddressSpace = limit.rlim_cur;
    en81WriteResourceLimitToString(virtualAddressSpace,value,true,RLIM_INFINITY);
    MSGD ((INFO_MAX_VIRT_MEM_SIZE,value));
#ifdef RLIMIT_RSS
    getrlimit(RLIMIT_RSS,&limit);
    en81WriteResourceLimitToString((SAPDB_UInt8)limit.rlim_cur,value,true,RLIM_INFINITY);
    MSGD ((INFO_RESIDENT_SET_SIZE,value));
#endif
    getrlimit(RLIMIT_DATA,&limit);
    heapAvailable = limit.rlim_cur;
    en81WriteResourceLimitToString(heapAvailable,value,true,RLIM_INFINITY);
    MSGD ((INFO_MAX_HEAP_SIZE,value));

    memoryNeeded = (SAPDB_UInt8)(XparamParameters->cacheSize*XparamParameters->ulDBPageSize + XparamParameters->ulOMSHeapLimit);

    if(virtualAddressSpace != RLIM_INFINITY && (memoryNeeded > virtualAddressSpace))
        MSGD ((WRN_NOT_ENOUGH_VIRT_MEMORY));

    if(heapAvailable != RLIM_INFINITY && (memoryNeeded > heapAvailable))
        MSGD ((WRN_NOT_ENOUGH_HEAP));

    /*------------------------------------*/

    ShowSortedEnvironmentStringValuePairs(envp);
}

/*---------------------------------------*/

int en81ParseCommandLine(int argc, char *argv[])
{
    if ( argc == 1 )
    {
        printf("Missing database name\n");
        printf("Syntax: %s dbname\n", argv[0]);
        return 1;
    }

    if ( (argc == 2)
      && (argv[1][0] == '-')
      && (toupper(argv[1][1]) == 'V') )
    {
#ifdef SDB_WITH_PROFILER
#ifdef BIT64
        printf("64BIT Kernel : (profiling) %s\n", RTE_GetKernelVersion() );
#else
        printf("32BIT Kernel : (profiling) %s\n", RTE_GetKernelVersion() );
#endif /* BIT64 */
#else
#ifdef BIT64
        printf("64BIT Kernel : %s\n", RTE_GetKernelVersion() );
#else
        printf("32BIT Kernel : %s\n", RTE_GetKernelVersion() );
#endif /* BIT64 */
#endif /* SDB_WITH_PROFILER */
        return 1;
    }

    if ( strlen(argv[1]) >= sizeof(tsp00_DbName) )
    {
        printf("Database name '%s' to long (maximum %d character allowed)\n",
            argv[1], sizeof(tsp00_DbName) );
        return 1;
    }

    strcpy( (char *)&en81_dbname[0], argv[1] );

    en81_OtherArgc = argc - 2;
    en81_OtherArgv = &argv[2];

    if ( argc > 2 )
    {
        if ( !strcmp(argv[2], "-online") )
        {
            en81_StartupOptions |= FORCE_AUTORESTART;
        }
        else if ( !strcmp(argv[2], "-lowprio") )
        {
            en81_StartupOptions |= LOW_PROCESS_PRIORITY;
        }
		else if ( argv[2][0] == '-'
               && argv[2][1] == 'D'
               && argv[2][2] >= '0'
               && argv[2][2] <= '9' )
		{
    		e60_dbgdbglvl = argv[2][2] - '0' ;
		}
    }

    if ( argc > 3 )
    {
        if ( !strcmp(argv[3], "-lowprio") )
        {
            en81_StartupOptions |= LOW_PROCESS_PRIORITY;
        }
		else if ( argv[3][0] == '-'
               && argv[3][1] == 'D'
               && argv[3][2] >= '0'
               && argv[3][2] <= '9' )
		{
    		e60_dbgdbglvl = argv[3][2] - '0' ;
		}
    }

    if ( argc > 4 )
    {
        if ( argv[4][0] == '-'
          && argv[4][1] == 'D'
          && argv[4][2] >= '0'
          && argv[4][2] <= '9' )
		{
    		e60_dbgdbglvl = argv[4][2] - '0' ;
		}
    }

    return 0;
}

static void en81_CheckCoroutineUsage(RTE_XPARAM_REC *XparamParameters )
{
    if ( XparamParameters->fUseCoroutines )
    {
        MSGD(( INFO_USE_COROUTINE_FOR_TASK ));
        if ( XparamParameters->fUseStackOnStack )
        {
            MSGD(( INFO_USE_STACK_ON_STACK ));
        }
        if ( XparamParameters->fUseUcontext )
        {
            /* first check if getcontext() works (there are linuxes where it doesn't) */
            ucontext_t testContext;
            if( -1 == getcontext(&testContext) )
            {
                MSGD(( ERR_COROUTINES_NOT_AVAILABLE ));
                XparamParameters->fUseCoroutines = false;
                MSGD(( INFO_USE_THREADS_FOR_TASK ));
            }
            else
            {
                MSGD(( INFO_USE_UCONTEXT ));
            }
        }
        else
        {
            MSGD(( INFO_USE_OWN_CONTEXT ));
        }
    }
    else
    {
        MSGD(( INFO_USE_THREADS_FOR_TASK ));
    }
}

/*--------------------------------------------------------*/
/* ffs... */
void en81GetCommandLineOptions(int *pArgc, char ***pArgv)
{
    *pArgc = en81_OtherArgc;
    *pArgv = en81_OtherArgv;
}

/*
  @brief Try to open as much file descriptors as needed due to parameters given
  The number of file scriptors needed is
     (_IO_PROCS_PER_DEV + 1 + NUMBER_OF_UKTS) * (MAXDATAVOLUMES + MAXLOGVOLUMES + 2) 
    + MAXBACKUPVOLUMES 
    + _MAX_MESSAGE_FILES (Default for LVC = 64 )
    + 1(knldiag)
    + 1(dbm.utl)
    + 1(dbm.knl)
    + 1(dbm.mdf)
    + 1(xparam) 
    + 1(knldump)
    + 1(knldiag.evt) (optional)
    + 1(requestor_pipe)
    + 1(console_pipe)
    + 1(connection_reply_pipe)
    + 1(proc_map_file)

  @return 0 on success, 1 on failure
 */
static int en81_ValidateNumberOfWantedDescriptors()
{
    int iFile;
    int nextFd;
    int fileDescriptorsNeeded = 
        (KGS->maxdevspaces * KGS->devpsets) 
       + KGS->maxdevspaces 
       + KGS->maxtapes
       + XPARAM(ulMaxMessageFiles)
       + 12;
    int selfIoDescriptorsWanted = 
        ( XPARAM(fAllowSingleIO) ? (
         KGS->ulNumOfUKTs 
         * ( XPARAM(fUseAsyncIO) ? ((KGS->maxdevspaces  * 2) + KGS->maxtapes): KGS->maxdevspaces ) ) : 0 );

    int *fileFdArray = (int *)malloc((fileDescriptorsNeeded + selfIoDescriptorsWanted)*sizeof(int));

    if ( !fileFdArray )
    {
        MSGD(( 11000, WRN_TYPE, "INITIAL ", "Out of memory for wanted %d descriptors", fileDescriptorsNeeded + selfIoDescriptorsWanted));
        return 1;
    }

    for ( iFile = 0; iFile < fileDescriptorsNeeded; iFile++ )
    {
        nextFd = fileFdArray[iFile] = open("/dev/null", O_WRONLY);
        if ( nextFd < 0 )
        {
            nextFd = iFile;
            while ( iFile > 0 )
            {
                --iFile;
                close(fileFdArray[iFile]);
            }

            free(fileFdArray);
            if ( XPARAM(preallocateIOWorker) )
            {
                MSGD(( 11000, ERR_TYPE, "INITIAL ", "Configured %d files. Failed to open %d needed files.", fileDescriptorsNeeded + selfIoDescriptorsWanted, fileDescriptorsNeeded ));
                MSGD(( 11000, ERR_TYPE, "INITIAL ", "Limit reached at file %d! Check user and system limits...",  nextFd));
                MSGD(( 11000, WRN_TYPE, "INITIAL ", "Database must not start since PREALLOCATE_IOWORKER is set to YES"));
                return 1;
            }

            MSGD(( 11000, WRN_TYPE, "INITIAL ", "Configured %d files. Failed to open %d needed files.", fileDescriptorsNeeded + selfIoDescriptorsWanted, fileDescriptorsNeeded ));
            MSGD(( 11000, WRN_TYPE, "INITIAL ", "Limit reached at file %d! Check user and system limits...",  nextFd));
            MSGD(( 11000, WRN_TYPE, "INITIAL ", "Database tries to start, but can run into resource problem later..."));
            return 0;
        }
    }    

    for ( ; iFile < (fileDescriptorsNeeded + selfIoDescriptorsWanted); iFile++ )
    {
        nextFd = fileFdArray[iFile] = open("/dev/null", O_WRONLY);
        if ( nextFd < 0 )
        {
            nextFd = iFile;
            while ( iFile > 0 )
            {
                --iFile;
                close(fileFdArray[iFile]);
            }
            free(fileFdArray);

            if ( XPARAM(preallocateIOWorker) )
            {
                MSGD(( 11000, ERR_TYPE, "INITIAL ", "Configured %d files. Failed to open %d self io files.", fileDescriptorsNeeded + selfIoDescriptorsWanted, selfIoDescriptorsWanted ));
                MSGD(( 11000, ERR_TYPE, "INITIAL ", "Limit reached at file %d! Check user and system limits...",  nextFd));
                MSGD(( 11000, WRN_TYPE, "INITIAL ", "Database must not start since PREALLOCATE_IOWORKER is set to YES"));
                return 1;
            }

            MSGD(( 11000, WRN_TYPE, "INITIAL ", "Configured %d files. Failed to open optional %d self io files.", fileDescriptorsNeeded + selfIoDescriptorsWanted, selfIoDescriptorsWanted ));
            MSGD(( 11000, WRN_TYPE, "INITIAL ", "Limit reached at file %d! Check user and system limits...",  nextFd));
            MSGD(( 11000, WRN_TYPE, "INITIAL ", "Database tries to start now, but can run into resource problem later..."));
            return 0;
        }
    }

    while ( iFile > 0 )
    {
        --iFile;
        close(fileFdArray[iFile]);
    }
    free(fileFdArray);

    MSGD(( 11000, INFO_TYPE, "INITIAL ", "Verified that open of needed %d file descriptors is possible", fileDescriptorsNeeded + selfIoDescriptorsWanted));

    return 0;
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
