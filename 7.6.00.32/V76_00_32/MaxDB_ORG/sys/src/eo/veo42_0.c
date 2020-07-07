/*!
  @file           veo42_0.c
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
#include "SAPDBCommon/SAPDB_string.h"

#if defined (_WIN32)
#include <windows.h>
#include <direct.h>
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
    pathVar = malloc (pathVarLen);
    SAPDB_memcpy (pathVar, pathKeyC, pathKeyLen);
    pathVar [pathKeyLen] = '=';
    pathPos = pathVar + pathKeyLen + 1;
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
    chdir (dbroot);
}

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
    BOOLEAN      isLocalCommuniation = FALSE;

    sqlinit ((unsigned char *)cserverNameC, &cancelByte);
#if !defined (_WIN32)
    /*
       Ignore user umask setting for dbmsrv and only create files that are user and group writable
     */
    umask(007);
#endif
    sqlxservinit (&csRTE, argc, argv, dbname, dbroot, senderNodeId, &senderPID,
        &loggedOn, &couldLogOn, &callerId, &isLocalCommuniation, &commErr);
    if (commErr == va_ok) {
        eo42_ChangeEnvironment (dbroot);
        ctrlservopen (&vcontrolData, dbname, dbroot, &cancelByte,
                loggedOn, couldLogOn, callerId, isLocalCommuniation, senderNodeId, (SAPDB_Int8)senderPID);
        commErr = (vcontrolData != NULL) ? va_ok : va_notok;
#if defined (_WIN32)
        __try {
#endif
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

            cancelByte = FALSE;     /* Reset CancelByte */
        }
#if defined (_WIN32)
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
        }
#endif
        ctrlservclose (vcontrolData);
        sqlxservexit (csRTE);
    }
    sqlfinish (FALSE);
    return (commErr);
}

