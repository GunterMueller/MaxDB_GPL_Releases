/*



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

#include <WINDOWS.H>

#include <stdio.h>
#include <time.h>
#include <errno.h>

/* <-- Crash handling unix */
#ifndef WIN32
    #include <signal.h>
    #include <unistd.h>
    #include <stdlib.h>
    #if defined(LINUX)
        #include <linux/version.h>
    #endif
#endif
/* Crash handling unix --> */

#include <sapdbwa_define.h>

#include "SAPDB/WebAgent/HTTP/WAHTTP_Misc.h"
#include "SAPDB/WebAgent/HTTP/WAHTTP_Log.h"
#include "SAPDB/WebAgent/HTTP/WAHTTP_IniPar.h"

#include "hsp100.h"
#include "gsp05.h"

#include "heo670.h"

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

#define INI_LOGFILEPATH		"LogFilePath"
#define DEF_LOGFILE		"httpreq.log"
#define DEF_ERRFILE		"httperr.log"

#define LOG_FORMAT_LEAD		"%s: %s"
#define LOG_FORMAT_TRAIL	""
#define TRC_FORMAT		"%s: "
#define MALLOC_ERROR		"Malloc error"

typedef tsp00_Int4	My_FILE;
/*
static My_FILE	*LogFd = NULL;
static My_FILE	*ErrFd = NULL;
static Excl	ErrExcl;
static Excl	LogExcl;
static Excl	LogReqExcl;
*/
static sapdbwa_Bool LogReq = sapdbwa_True;
static int LogRes = FALSE;
static char LogFilePath[MAX_FILENAME_LEN+1] = "";
static char DefLogFilePath[1000] = "";
static int TraceLevel = 0;
static char StartTimeStr[11];

static DynStr	ReqLogStart = NULL,
		ResLogStart = NULL;
static DynStr	ReqLogEnd = NULL,
		ResLogEnd = NULL;

static DynStr	ReqLeader = NULL,
		ReqTrailer = NULL;
static DynStr	ResLeader = NULL,
		ResTrailer = NULL;

static struct st_wahttp_log_desc     myLogDesc;

void SetDefLogFilePath
(
 char	*path
)
{
 strcpymax(sizeof(DefLogFilePath), DefLogFilePath, path);
}

/*===========================================================================*/

WAHTTP_LogDesc  Log_GetDesc()
{

    return &myLogDesc;

}

/*===========================================================================*/

int InitLog()
{

    char		        pLogReq[100];
    char		        Ipar[100];
    time_t		        t;
    char		        tmps[100];
    tsp05_RteFileError	err;
    tsp00_Bool		    bOk;

    time(&t);
    strftime(StartTimeStr, sizeof(StartTimeStr), "%m%d%y%H%M", localtime(&t));

    initexcl(&myLogDesc.logExcl, 0, NULL);
    initexcl(&myLogDesc.errExcl, 0, NULL);
    /*
    initexcl(&LogReqExcl, 0, NULL);
    */

    strncpy( LogFilePath, GetLogDirectory(), MAX_FILENAME_LEN );
    LogFilePath[MAX_FILENAME_LEN] = '\0';

    LogReq = GetLogRequests();

    /* Is a log requested? */
    if (LogReq == sapdbwa_True) {
        if (strlen(LogFilePath) == 0)
    	    strcpymax(sizeof(LogFilePath), LogFilePath, DefLogFilePath);

        sp77sprintf( Ipar, sizeof(Ipar), "%s%c%s", LogFilePath, DIRPATH_SEP, DEF_LOGFILE);

        sqlfopenc (Ipar, sp5vf_binary, sp5vf_append, sp5bk_unbuffered, &myLogDesc.logFile, &err);
        if (err.sp5fe_result != vf_ok)
            myLogDesc.logFile = 0;

        Log("\n", -1);
    }

    if (strlen(LogFilePath) == 0)
        strcpymax(sizeof(LogFilePath), LogFilePath, DefLogFilePath);

    sp77sprintf( Ipar, sizeof(Ipar), "%s%c%s", LogFilePath, DIRPATH_SEP, DEF_ERRFILE);

    sqlfopenc (Ipar, sp5vf_binary, sp5vf_append, sp5bk_unbuffered, &myLogDesc.errFile, &err);
    if (err.sp5fe_result != vf_ok)
        myLogDesc.errFile = 0;

    sprintf(tmps, "Starting ErrLog (build %s)", s100buildnumber());
    Log_Error(tmps, -1);


    printf("Using (logfile:%s, tracelevel:%d)\n", Ipar, TraceLevel);

    LogRes = FALSE;

    return(TRUE);

}

/*===========================================================================*/

void Log( char	text[],
          int	lg )
{

    char			    tdbuffer[100];
    char			    wbuffer[100];
    time_t			    t;
    size_t			    tmplg = 0;
    size_t			    ulg = 0;
    tsp00_Bool		    bOk;
    tsp05_RteFileError	ferror;

    /* Is a log requested? */
    if (LogReq == sapdbwa_False) {
        return;
    }

    if (!myLogDesc.logFile)
        return;

    if (lg < 0)
        ulg = strlen(text);
    else
        ulg = lg;

    begexcl(myLogDesc.logExcl);

    time(&t);
    strftime(tdbuffer, sizeof(tdbuffer), "[%d/%b/%Y:%H:%M:%S]",
    localtime(&t));
    if (myLogDesc.logFile) {
        sp77sprintf( wbuffer, sizeof(wbuffer), LOG_FORMAT_LEAD, tdbuffer, "");
        sqlfwritec (myLogDesc.logFile, wbuffer, strlen(wbuffer), &ferror);
    }

    if (myLogDesc.logFile)
        sqlfwritec (myLogDesc.logFile, text, ulg, &ferror);

    if (myLogDesc.logFile) {
        sqlfwritec (myLogDesc.logFile, LOG_FORMAT_TRAIL, sizeof(LOG_FORMAT_TRAIL)-1, &ferror);
#ifdef WIN32
        sqlfwritec (myLogDesc.logFile, "\r\n", sizeof("\r\n")-1, &ferror);
#else
        sqlfwritec (myLogDesc.logFile, "\n", sizeof("\n")-1, &ferror);
#endif
    }

    endexcl(myLogDesc.logExcl);

}

/*===========================================================================*/

void Log_WriteError( char	    *text,
                     int	     length,
                     char       *file,
                     const int   line )
{

    char			    tdbuffer[100];
    char			    wbuffer[100];
    char                errText[1024] = "";
    time_t			    t;
    size_t			    tmplg = 0;
    size_t			    ulg = 0;
    tsp00_Bool		    bOk;
    tsp05_RteFileError	ferror;

    if (!myLogDesc.errFile)
        return;

    begexcl(myLogDesc.errExcl);

    time(&t);
    strftime(tdbuffer, sizeof(tdbuffer), "[%d/%b/%Y:%H:%M:%S]", localtime(&t));
    if (myLogDesc.errFile) {
        sp77sprintf( wbuffer, sizeof(wbuffer), LOG_FORMAT_LEAD, tdbuffer, "");
        sqlfwritec (myLogDesc.errFile, wbuffer, strlen(wbuffer), &ferror);
    }

    if (myLogDesc.errFile) {
#ifdef WAHTTP_COMPREHENSIVE_ERROR_DESCRIPTION
    	sp77sprintf(errText, 1024, "%s[%s: %d]", text, file, line);
#else
    	sp77sprintf(errText, 1024, "%s", text);
#endif

        ulg = strlen(errText);
        sqlfwritec (myLogDesc.errFile, errText, ulg, &ferror);
    }

    if (myLogDesc.errFile) {
        sqlfwritec (myLogDesc.errFile, LOG_FORMAT_TRAIL, sizeof(LOG_FORMAT_TRAIL)-1, &ferror);
#ifdef WIN32
        sqlfwritec (myLogDesc.errFile, "\r\n", sizeof("\r\n")-1, &ferror);
#else
        sqlfwritec (myLogDesc.errFile, "\n", sizeof("\n")-1, &ferror);
#endif
    }

    endexcl(myLogDesc.errExcl);

}

/*===========================================================================*/

void Log_WriteJournal( int   socket,
                       char *text,
                       int   length )
{

    char			    tdbuffer[100];
    char			    wbuffer[100];
    char                errText[1024] = "";
    time_t			    t;
    size_t			    tmplg = 0;
    size_t			    ulg = 0;
    tsp00_Bool		    bOk;
    tsp05_RteFileError	ferror;

    if (!myLogDesc.errFile)
        return;

    begexcl(myLogDesc.errExcl);

    time(&t);
    strftime(tdbuffer, sizeof(tdbuffer), "[%d/%b/%Y:%H:%M:%S]", localtime(&t));
    if (myLogDesc.errFile) {
        sp77sprintf( wbuffer, sizeof(wbuffer), LOG_FORMAT_LEAD, tdbuffer, "");
        sqlfwritec (myLogDesc.errFile, wbuffer, strlen(wbuffer), &ferror);
    }

    if (myLogDesc.errFile) {
    	sp77sprintf(errText, 1024, "[%d]%s", socket, text);

        ulg = strlen(errText);
        sqlfwritec (myLogDesc.errFile, errText, ulg, &ferror);
    }

    if (myLogDesc.errFile) {
        sqlfwritec (myLogDesc.errFile, LOG_FORMAT_TRAIL, sizeof(LOG_FORMAT_TRAIL)-1, &ferror);
#ifdef WIN32
        sqlfwritec (myLogDesc.errFile, "\r\n", sizeof("\r\n")-1, &ferror);
#else
        sqlfwritec (myLogDesc.errFile, "\n", sizeof("\n")-1, &ferror);
#endif
    }

    endexcl(myLogDesc.errExcl);

}

/*===========================================================================*/

void SetTraceLevel( int	Level )
{

    TraceLevel = Level;

}

/*===========================================================================*/

void Trace( int	    Level,
            char   *Txt,
            size_t	lg )
{

    if (Level > TraceLevel)
        return;

    Log_Error(Txt, lg);

}

/*===========================================================================*/

void ExitLog( void )
{

    tsp05_RteFileError	ferror;

    /* Is a log requested? */
    if (LogReq == sapdbwa_False) {
        return;
    }

    Log("Terminating Logging", -1);
    begexcl(myLogDesc.logExcl);

    /*
    exitexcl(LogReqExcl);
    */
    if (myLogDesc.logFile)
        sqlfclosec (myLogDesc.logFile, sp5vf_close_normal, &ferror);

    myLogDesc.logFile = 0;

    if (myLogDesc.errFile)
        sqlfclosec (myLogDesc.errFile, sp5vf_close_normal, &ferror);

    myLogDesc.errFile = 0;
    endexcl(myLogDesc.logExcl);
    endexcl(myLogDesc.errExcl);

    exitexcl(myLogDesc.logExcl);

}

/*===========================================================================*/

void Log_WriteSyscallError( char	   *txt,
                            int         err,
                            char       *file,
                            const int   line )
{
    char	     tmp[1000];
    const char	*etxt = "";
    size_t	     lg = 0;

#ifdef WIN32

    LPVOID  lpMsgBuf;
    DWORD   iets = WSAGetLastError();
    char	tmptxt[MAX_ERROR_TEXT_LEN+1];

    if (!iets || iets == 10053 || iets == 10054) { /* PTS: */
        return;
    }

    GetLastErrorText(tmptxt, MAX_ERROR_TEXT_LEN);

    if (strlen(tmptxt) > 0 ) {
        lg = sp77sprintf(tmp, 1000, "%s, (GetLastError() is: %s)", txt, tmptxt);
    } else {
        lg = sp77sprintf(tmp, 1000, "%s", txt);
    }

    lg = strlen(tmp);

    Log_WriteError(tmp, lg, file, line);

#else

#if SUN || SOLARIS || HPUX || AIX || LINUX
    etxt = strerror(errno);
    if (!etxt) {
        etxt = "";
    }
#else
    if (errno >= 0 && errno < sys_nerr) {
        etxt = sys_errlist[errno];
    }
#endif

    lg = sprintf(tmp, "%s, %s", txt, etxt);
    Log_WriteError(tmp, lg, file, line);

#endif

}

/*===========================================================================*/
/*=== Crash handling                                                         */
/*===========================================================================*/

void Log_Exception( char    *text )
{

    char                errText[1024] = "";
    size_t			    ulg = 0;
    tsp05_RteFileError	ferror;

    if (!myLogDesc.errFile)
        return;

    begexcl(myLogDesc.errExcl);

    sp77sprintf(errText, 1024, "%s", text);

    ulg = strlen(errText);
    sqlfwritec (myLogDesc.errFile, errText, ulg, &ferror);

#ifdef WIN32
        sqlfwritec (myLogDesc.errFile, "\r\n", sizeof("\r\n")-1, &ferror);
#else
        sqlfwritec (myLogDesc.errFile, "\n", sizeof("\n")-1, &ferror);
#endif

    endexcl(myLogDesc.errExcl);

}

/*===========================================================================*/

void WriteException( char const * const str, 
                     void * const       pOutContext )
{

    Log_Exception((char*)str);

}

/*===========================================================================*/

#ifdef WIN32
    DWORD Log_WriteException( LPEXCEPTION_POINTERS ec )
    {
        eo670TraceContext traceContext;

        Log_Error("EXECPTION OCCURED:", 0);

        traceContext.threadHandle   = GetCurrentThread();
        traceContext.pThreadContext = ec->ContextRecord;
        eo670_CTraceContextStackOCB(&traceContext, WriteException, NULL);

        return EXCEPTION_CONTINUE_SEARCH;
    }
#else
    void Log_WriteException(void *context)
    {
        Log_Error("EXECPTION OCCURED:", 0);
        eo670_CTraceContextStackOCB(context, WriteException, NULL);
    }

/*===========================================================================*/

void Log_SetSignalHandler()
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

    Log_WriteException(context);
}

#endif

/*===========================================================================*/

