/*!**********************************************************************

  module: RegUpd_Log.c

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: Registry

  description:  This component updates registry / ini file entries
                in a managed way.
                http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1122610

  see also:

  -----------------------------------------------------------------------

                         Copyright (c) 2002-2005 SAP AG







    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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





*/

/***********************************************************************

	Includes

 ***********************************************************************/

#include "SAPDB/RegistryUpdate/RegUpd_Log.h"

/***********************************************************************

	Defines

 ***********************************************************************/

#define WITH_TRACE              0
#if WITH_TRACE
    #define WriteTraceMethod(a) printf("\n\n%s\n",a)

    #define WriteTrace(a)       printf(a)
    #define WriteTrace1(a,b)    printf(a,b)
    #define WriteTrace2(a,b,c)  printf(a,b,c)
#else
    #define WriteTraceMethod(a)

    #define WriteTrace(a)
    #define WriteTrace1(a,b)
    #define WriteTrace2(a,b,c)
#endif

/*---------------------------------------------------------------------------*/

/* <-- Crash handling unix */
#ifndef WIN32

#if defined(AIX) || defined(OSF1)
/*
  Only these two platform have a version of sigaltstack, that works fine in
  combination with stack overflow handling...
 */
#  define USE_SIGALTSTACK
#  define WANTED_SIGSTKSZ_EN81 (MINSIGSTKSZ+(64*1024))
#endif /* AIX || OSF1 */

#ifdef USE_SIGALTSTACK
/* 
   define alternate stack on heap ... (16*1024) is enough space for system page alignment 
   on all known systems 
 */
#define MAX_SYSTEM_PAGE_SIZE_OVER_ALL_PLATFORMS (16*1024)
static char AlternateStackSpace[WANTED_SIGSTKSZ_EN81+MAX_SYSTEM_PAGE_SIZE_OVER_ALL_PLATFORMS] = { 0 };
#endif /* USE_SIGALTSTACK */

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

volatile        int                     KernelGotCrashSignal = 0;

#define EXIT_CODE_PANIC_SIGNAL  42

static int SetSigAction(int sig, void (*handler)(int, siginfo_t *, void *) );
static void CrashSignalHandler( int sig, siginfo_t *sigInfo, void *context );

#endif
/* Crash handling unix --> */

/*---------------------------------------------------------------------------*/

/***********************************************************************

	Structs & Types

 ***********************************************************************/

/*---------------------------------------------------------------------------*/

/***********************************************************************

	Prototypes

 ***********************************************************************/

void WriteLog( char const * const   text, 
               SAPDB_Bool           withTime );

void WriteException( char const * const text, 
                     void * const       pOutContext );

/***********************************************************************

	Implementation

 ***********************************************************************/

char    execDir[MAX_FILENAME_LEN+1] = "";

SAPDB_Bool GetExecDirectory( char           *cmdArg,
                             SAPDB_UInt2     size      )
{

	char		tmpPath[200];
	char		execPath[200];
	char		currentDirectory[MAX_FILENAME_LEN+1];
	char		*separator = NULL;

	/* Get current directory	*/
	currentDirectory[0] = '\0';
	if( !getcwd( currentDirectory, sizeof(currentDirectory) ) ) {
		separator = strrchr(cmdArg, DIRPATH_SEP);
		if (!separator)
			execPath[0] = '\0';
		else if (separator - cmdArg + 1 > sizeof(execPath)) {
			printf("%s:%s\nCurrent directory path too long!\n", APP_NAME, cmdArg);
			return SAPDB_FALSE;
		} else {
			strncpy(execPath, cmdArg, separator - cmdArg + 1);
			execPath[separator - cmdArg + 1] = '\0';
		}

		if (execPath[1] != ':' && execPath[0] != DIRPATH_SEP) {
			/* this is a relative path */
			sprintf(tmpPath, "%s%c%s", currentDirectory, DIRPATH_SEP, execPath);
		} else
			strcpy(tmpPath, execPath);

		strcpy( currentDirectory, tmpPath );
	}

	/* Set return value	*/
	strncpy( execDir, currentDirectory, size );
	execDir[MAX_FILENAME_LEN] = '\0';

	return SAPDB_TRUE;

}

/*===========================================================================*/

/*===========================================================================*/
/*=== Crash handling                                                         */
/*===========================================================================*/

#ifdef WIN32
    DWORD DumpException( LPEXCEPTION_POINTERS ec )
    {
        eo670TraceContext traceContext;

        WriteLog("EXECPTION OCCURED:", SAPDB_TRUE);

        traceContext.threadHandle   = GetCurrentThread();
        traceContext.pThreadContext = ec->ContextRecord;
        eo670_CTraceContextStackOCB(&traceContext, WriteException, NULL);

        return EXCEPTION_CONTINUE_SEARCH;
    }
#else
    void DumpException(void *context)
    {
        WriteLog("EXECPTION OCCURED:", SAPDB_TRUE);
        eo670_CTraceContextStackOCB(context, WriteException, NULL);
    }

/*===========================================================================*/

    void SetSignalHandler()
    {
    #ifdef USE_SIGALTSTACK
        stack_t newStack;
        long MemPageSize = sysconf(_SC_PAGESIZE);

        newStack.ss_sp = ((char *)0) + ( ((&AlternateStackSpace[0]
                                    - ((char *)0))+MemPageSize-1 ) & ~(MemPageSize-1) );
        newStack.ss_size = WANTED_SIGSTKSZ_EN81;
        newStack.ss_flags = 0;

        if ( sigaltstack(&newStack, 0) == -1 )
        {
        perror("sigaltstack failed:");
        exit(1);
        }
    #endif /* USE_SIGALTSTACK */

        /*
        *  These should crash the kernel.
        */
        (void) SetSigAction ( SIGILL  , CrashSignalHandler );
        (void) SetSigAction ( SIGTRAP , CrashSignalHandler );
        (void) SetSigAction ( SIGABRT , CrashSignalHandler );
        (void) SetSigAction ( SIGFPE  , CrashSignalHandler );
        (void) SetSigAction ( SIGBUS  , CrashSignalHandler );
        (void) SetSigAction ( SIGSEGV , CrashSignalHandler );
        (void) SetSigAction ( SIGXCPU , CrashSignalHandler );
    #if defined(AIX)
    /*AIX*/   (void) SetSigAction ( SIGIOT  , CrashSignalHandler );
    #endif
    #if !defined(LINUX)
    /*!LINUX*/ (void) SetSigAction ( SIGEMT  , CrashSignalHandler );
    /*!LINUX*/ (void) SetSigAction ( SIGSYS  , CrashSignalHandler );
    #endif
    }

/*===========================================================================*/

    static int SetSigAction(int sig, void (*handler)(int, siginfo_t *, void *) )
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

/*===========================================================================*/

    static int SetDefaultAction(int sig)
    {
    struct sigaction sa;

    sa.sa_handler = SIG_DFL;
    sigemptyset( &sa.sa_mask );
    sa.sa_flags = 0;
    return sigaction( sig, &sa, NULL);
    }

/*===========================================================================*/

    static int SetIgnoreAction(int sig)
    {
    struct sigaction sa;

    sa.sa_handler = SIG_IGN;
    sigemptyset( &sa.sa_mask );
    sa.sa_flags = 0;
    return sigaction( sig, &sa, NULL);
    }

/*===========================================================================*/

    static void CrashSignalHandler( int sig, siginfo_t *sigInfo, void *context )
    {
    #ifdef LINUX_WITHOUT_SIGINFO
        /* in the old linux kernel 2.2.11 context was not usable at all.... */
        asm( "movl %%ebp,%0" : "=r" (context) : /* no input operand */ );
    #endif /* LINUX_WITHOUT_SIGINFO */

        if ( 2 <= KernelGotCrashSignal )
        {
            exit(EXIT_CODE_PANIC_SIGNAL);
        }
        ++KernelGotCrashSignal;

        (void) SetDefaultAction( sig     );
        (void) SetDefaultAction( SIGIOT  );
        (void) SetDefaultAction( SIGBUS  );
        (void) SetDefaultAction( SIGSEGV );

        DumpException(context);
    }

#endif

/*===========================================================================*/

void WriteException( char const * const text, 
                     void * const       pOutContext )
{

    WriteLog(text, SAPDB_FALSE);

}

/*===========================================================================*/

void WriteLog( char const * const   text, 
               SAPDB_Bool           withTime )
{

    char		        Ipar[100];
    char			    tdbuffer[100];
    char			    wbuffer[100];
    char                errText[1024] = "";
    time_t			    t;
    size_t			    ulg = 0;
    tsp05_RteFileError	ferror;
    int                 logFile;

    sp77sprintf(Ipar, sizeof(Ipar), "%s%c%s", execDir, DIRPATH_SEP, "regupd.log");
    sqlfopenc(Ipar, sp5vf_binary, sp5vf_append, sp5bk_unbuffered, &logFile, &ferror);
    if (ferror.sp5fe_result != vf_ok)
        logFile = 0;

    if (withTime == SAPDB_TRUE) {
        time(&t);
        strftime(tdbuffer, sizeof(tdbuffer), "[%d/%b/%Y:%H:%M:%S]", localtime(&t));
        sp77sprintf( wbuffer, sizeof(wbuffer), "%s: %s", tdbuffer, "");
        sqlfwritec (logFile, wbuffer, strlen(wbuffer), &ferror);
    }

    sp77sprintf(errText, 1024, "%s", text);
    ulg = strlen(errText);
    sqlfwritec (logFile, errText, ulg, &ferror);

#ifdef WIN32
        sqlfwritec (logFile, "\r\n", sizeof("\r\n")-1, &ferror);
#else
        sqlfwritec (logFile, "\n", sizeof("\n")-1, &ferror);
#endif

    sqlfclosec (logFile, sp5vf_close_normal, &ferror);

}


/***********************************************************************

	End

 ***********************************************************************/
