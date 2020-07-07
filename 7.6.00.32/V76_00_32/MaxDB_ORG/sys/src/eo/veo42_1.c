/*!
  @file           veo42_1.c
  @author         DanielD
  @special area
  @brief          RTE - main module of control server
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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined (_WIN32)
#include <windows.h>
#include <direct.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h> /* for umask */
#include <signal.h>
#include <ucontext.h>
#endif

#define MOD__ "veo42c :"

#include "heo102.h"
#include "heo05.h"
#include "heo01.h"
#include "vsp009c.h"
#include "gsp00.h"

#if defined (_WIN32)    /*&if $OSSPEC = WIN32*/
#include  "gos00.h"     /* nocheck */
#include  "vsp002c.h"   /* nocheck */
#else                   /* &else */
#include  "RunTime/RTE_SdbStarter.h" /* nocheck */
#endif                  /*&endif*/

#define cserverNameC "dbmsrv                                                          "
static const char *dbrootKeyC = "DBROOT";
static const char *pathKeyC   = "PATH";
#if defined (_WIN32)
#define dirSepC '\\'
#define pathSepC ';'
static const char* eo42_subdirsC [] = {"bin", "pgm", NULL};
#else
#define dirSepC '/'
#define pathSepC ':'
static const char* eo42_subdirsC [] = {"bin", NULL};
#endif

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif /* TRUE */

#include "SAPDBCommon/SAPDB_string.h"

/**\
--------------------------------------- MF__ MOD__ (server)
\**/

static void
eo42_SetDBRoot (
   const char* dbroot,
   int dbrootLen)
{
#undef MF__
#define MF__ MOD__"eo42_SetDBRoot"
    char       *dbrootVar;
    int         dbrootKeyLen = (int)strlen (dbrootKeyC);

    dbrootVar = malloc (dbrootKeyLen + 1 + dbrootLen + 1); /* = and 0 */
	SAPDB_memcpy (dbrootVar, dbrootKeyC, dbrootKeyLen);
    dbrootVar [dbrootKeyLen] = '=';
    SAPDB_memcpy (dbrootVar + dbrootKeyLen + 1, dbroot, dbrootLen + 1);
    putenv (dbrootVar);
}

/*----------------------------------------*/

static void
eo42_ChangePathVar (
   const char* dbroot,
   int dbrootLen)
{
#undef MF__
#define MF__ MOD__"eo42_ChangePathVar"
    int         pathKeyLen   = (int)strlen (pathKeyC);
    const char *oldPathVar;
    int         oldPathLen;
    char       *pathVar;
    int         pathVarLen;
    char       *pathPos;
    int         i;
    int         subdirLen;
    tsp00_Pathc pathIndep;
    int         pathIndepLen = 0;
    tsp01_RteError RteError;

    oldPathVar = getenv (pathKeyC);
    if (oldPathVar == NULL)
        oldPathLen = 0;
    else
        oldPathLen = (int)strlen (oldPathVar);

    pathVarLen = pathKeyLen + 1 + oldPathLen + 1;
    for (i = 0; eo42_subdirsC [i] != NULL; ++i) {
        pathVarLen += dbrootLen + (int)strlen (eo42_subdirsC [i]) + 2;
            /* 1 for dirSep  and 1 for pathSep */
    }
    if (sqlGetIndependentProgramsPath ( pathIndep, 0, &RteError)) {
      pathIndepLen = (int) strlen(pathIndep);
      for (i = 0; eo42_subdirsC [i] != NULL; ++i) {
        pathVarLen +=  pathIndepLen + (int)strlen (eo42_subdirsC [i]) + 2;
      } /* end for */
    } /* end if */

    pathVar = malloc (pathVarLen);
    SAPDB_memcpy (pathVar, pathKeyC, pathKeyLen);
    pathVar [pathKeyLen] = '=';
    pathPos = pathVar + pathKeyLen + 1;

    /* add dependent path */
    for (i = 0; eo42_subdirsC [i] != NULL; ++i) {
        SAPDB_memcpy (pathPos, dbroot, dbrootLen);
        pathPos += dbrootLen;
        pathPos [0] = dirSepC;
        ++pathPos;
        subdirLen = (int)strlen (eo42_subdirsC [i]);
        SAPDB_memcpy (pathPos, eo42_subdirsC [i], subdirLen);
        pathPos += subdirLen;
        pathPos [0] = pathSepC;
        ++pathPos;
    }

    /* add independent path*/
    if (pathIndepLen >0) {
      for (i = 0; eo42_subdirsC [i] != NULL; ++i) {
          SAPDB_memcpy (pathPos, pathIndep, pathIndepLen);
          pathPos += pathIndepLen;
          pathPos [0] = dirSepC;
          ++pathPos;
          subdirLen = (int)strlen (eo42_subdirsC [i]);
          SAPDB_memcpy (pathPos, eo42_subdirsC [i], subdirLen);
          pathPos += subdirLen;
          pathPos [0] = pathSepC;
          ++pathPos;
      } /* end for */
    } /* end if */

    /* add old path */
    if (oldPathVar != NULL)
        SAPDB_memcpy (pathPos, oldPathVar, oldPathLen + 1);

    putenv (pathVar);
}

/*----------------------------------------*/

static void
eo42_ChangeEnvironment (
   const char* dbroot)
{
#undef MF__
#define MF__ MOD__"eo42_ChangeEnvironment"
    int         dbrootLen;

    dbrootLen = (int)strlen (dbroot);
    eo42_SetDBRoot (dbroot, dbrootLen);
    eo42_ChangePathVar (dbroot, dbrootLen);

    { /* begin block */
      tsp00_Pathc    Path;
      tsp01_RteError RteError;

      if (sqlGetDbrootPath (Path, TERM_WITHOUT_DELIMITER_EO01, &RteError)) {
          chdir(Path);
      } /* end if */
    } /* end block */
}

/*----------------------------------------*/
#if defined( _WIN32 )
static int
eo42_ExceptionHandlerProxy (
    unsigned int exCode,
    struct _EXCEPTION_POINTERS *ep) {

    return ctrlservExceptionHandler( exCode, ep );
}
#else
static void
eo42_ExceptionHandlerProxy( int signalCode ) {

    ctrlservExceptionHandler( signalCode );
}
#endif

/*----------------------------------------*/

int
c_main (
    int argc,
    char* argv [])
{
#undef MF__
#define MF__ MOD__"main"
    void        *csRTE;
    tsp00_DbNamec dbname;
    tsp00_Pathc   dbroot;
    tsp00_NodeIdc senderNodeId;
    RTE_OSPid     senderPID;
    tsp01_CommErr commErr;
    void        *vcontrolData = NULL;
    int          rc;
    char        *requestData;
    int          requestLen;
    char        *replyData;
    int          replyLenMax;
    int          replyLen;
    BOOLEAN      connected  = TRUE;
    BOOLEAN      cancelByte = FALSE;
    BOOLEAN      loggedOn   = FALSE;
    BOOLEAN      couldLogOn = FALSE;
    int          callerId   = -1;
    BOOLEAN      isLocalCommunication = FALSE;
    SAPDB_Int8 senderPIDOSIndep = 0;

#if !defined (_WIN32)
    SAPDB_Int4   realCallerUid;
    SAPDB_Int4   realCallerGid;
    SAPDB_Bool   isAdministrator;

    RTE_CallSdbStarter(&argc, &argv[0], &realCallerUid, &realCallerGid, &isAdministrator);
#endif

    sqlinit ((unsigned char *)cserverNameC, &cancelByte);
#if !defined (_WIN32)
    /*
       Ignore user umask setting for dbmsrv and only create files that are user and group writable
     */
    umask(007);
#endif
    sqlxservinit (&csRTE, argc, argv, dbname, dbroot, senderNodeId, &senderPID,
        &loggedOn, &couldLogOn, &callerId, &isLocalCommunication, &commErr);


#if !defined (_WIN32)
    /* Using RTE_CallSdbStarter always produces 'sdbowner' as caller id. So use realCallerId saved before... */
    if ( couldLogOn && isAdministrator)
    {
        loggedOn = TRUE;
        callerId = realCallerUid;
    }
#endif
    if (commErr == va_ok) {
#if defined (_WIN32)
        __try {
#else
        signal( SIGILL, eo42_ExceptionHandlerProxy );
        signal( SIGTRAP, eo42_ExceptionHandlerProxy );
        signal( SIGABRT, eo42_ExceptionHandlerProxy );
        signal( SIGFPE, eo42_ExceptionHandlerProxy );
        signal( SIGBUS, eo42_ExceptionHandlerProxy );
        signal( SIGSEGV, eo42_ExceptionHandlerProxy );
        signal( SIGXCPU, eo42_ExceptionHandlerProxy );
        #if defined(AIX)
        signal ( SIGIOT, eo42_ExceptionHandlerProxy );
        #endif
        #if !defined(LINUX) && !defined(FREEBSD)
        signal ( SIGEMT, eo42_ExceptionHandlerProxy );
        signal ( SIGSYS, eo42_ExceptionHandlerProxy );
        #endif
#endif
        eo42_ChangeEnvironment (dbroot);
        ctrlservopen (
                &vcontrolData,
                dbname,
                dbroot,
                &cancelByte,
                loggedOn,
                couldLogOn,
                callerId,
                isLocalCommunication,
                senderNodeId,
                (SAPDB_Int8)senderPID);
        commErr = (vcontrolData != NULL) ? va_ok : va_notok;

        while (connected && (commErr == va_ok)) {
            requestData = NULL;
            requestLen  = undef_ceo00;
            replyData   = NULL;
            replyLenMax = undef_ceo00;
            replyLen    = 0;
            sqlxservrecv (csRTE, &requestData, &requestLen,
                          &replyData, &replyLenMax, &commErr);
            if (commErr != va_ok) {
                break;
            }
            rc =  ctrlservcommand (vcontrolData, requestData, requestLen,
                                   replyData, &replyLen, replyLenMax);
            if ( replyLen )
                sqlxservsend (csRTE, replyLen, &commErr);

            if ((rc == csp9_ctrlcmd_err) || (rc == csp9_ctrl_release)) {
                connected = FALSE;
            }
        }

        /* communication error, let implementation do something... */
        if( commErr != va_ok )
            ctrlservHandleCommError( callerId, connected, cancelByte, commErr);
        
        ctrlservclose (vcontrolData);

#if defined (_WIN32)
        }
        __except (eo42_ExceptionHandlerProxy(GetExceptionCode(), GetExceptionInformation())) {
        }
#endif
        sqlxservexit (csRTE);
    }
    sqlfinish (FALSE);
    return (commErr);
}
