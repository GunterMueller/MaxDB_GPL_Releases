/*!*********************************************************************

    @file         RTEUDE_StarterRelay.cpp

    @author       DanielD

    @brief        A service to start UDE server

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2004-2005 SAP AG

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
***********************************************************************/

#include "RunTime/UDEServer/RTEUDE_StarterRelay.hpp"

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#if defined (_WIN32)
#include <windows.h>
#include <process.h>
#endif

#if ! defined (_WIN32)
#include <errno.h>
#endif

#include "gsp00.h"
#include "heo06.h"
#include "hsp77.h"
#include "RunTime/InputOutput/RTEIO_ShellUtil.hpp"
#include "RunTime/UDEServer/RTEUDE_IPC.hpp"
#include "RunTime/UDEServer/RTEUDE_CommandInterpreter.hpp"

/*----------------------------------------*/

class Log;
static int logLevel = 1;

#if defined (WIN32)
#define PATH_DELIMITER "\\"
#else
#define PATH_DELIMITER "/"
#define GetLastError() errno
#endif

/*----------------------------------------*/

class Log
{
public:
    Log () {
        this->outstream = fopen ("c:/SAPDevelop/V76/develop/testrelay.log", "w");
    };
    ~Log () {
        if (this->isValid ()) {
            fclose (this->outstream);
        }
    };
    bool isValid () const {
        return this->outstream != NULL;
    };
    void msg (const char * msg, int len = -1) {
        if (this->isValid ()) {
            if (len == -1) {
                len = strlen (msg);
            }
            fprintf (this->outstream, "%.*s\n", len, msg);
        }
    };
private:
    FILE * outstream;
};

/*----------------------------------------*/

class RelayContext
{
public:
    Log * log;
};


/*----------------------------------------*/

static bool
dirExists (
    const char * dirname)
{
    tsp05_RteFileInfo finfo;
    tsp05_RteFileError ferr;
    sqlfinfoc (dirname, &finfo, &ferr);
    if (ferr.sp5fe_result != vf_ok) {
        return false;
    }
    if (!finfo.sp5fi_exists) {
        return false;
    }
    if (finfo.sp5fi_media_kind != 7) {
        return false;
    }
    return true;
}

/*----------------------------------------*/

static bool
assureDirectory (
    const char * dirname)
{
    tsp01_RteError rteError;
    if (!sqlcreate_dirc ((char*) dirname, &rteError)) {
        return false;
    }
    return true;
}

/*----------------------------------------*/

static bool
isDotDir (
    const char * path)
{
    if (strcmp (path, ".") == 0) {
        return true;
    }
    if (strcmp (path, "..") == 0) {
        return true;
    }
    return false;
}

/*----------------------------------------*/

static bool
rmtree (
    const char * path)
{
    bool                ok = true;
    tsp00_C256          dirname;
    void              * dirHandle;
    tsp00_C256          currentName;
    tsp05_RteFileError  ferr;

    strcpy (dirname, path);
    sqlfopendirc (&dirname, &dirHandle, &currentName, &ferr);
    if (ferr.sp5fe_result != vf_ok) {
        return false;
    }
    do {
        if (isDotDir (currentName)) {
            sqlfreaddirc (dirHandle, &currentName, &ferr);
            continue;
        }
        /*
         * get file info
         */
        tsp00_C256  fullpath;
        sp77sprintf (fullpath, sizeof (fullpath), "%s"PATH_DELIMITER"%s",
            path, currentName.asCharp ());
        tsp05_RteFileInfo finfo;
        sqlfinfoc (fullpath, &finfo, &ferr);
        if (ferr.sp5fe_result != vf_ok) {
            ok = false;
            sqlfreaddirc (dirHandle, &currentName, &ferr);
            continue;
        }
        /*
         *
         */
        if (finfo.sp5fi_media_kind == 7) {
            ok = rmtree (fullpath) && ok;
        }
        else {
            sqlferasec (fullpath, &ferr);
            ok = ok && (ferr.sp5fe_result == vf_ok);
        }
        /*
         * get next entry
         */
        sqlfreaddirc (dirHandle, &currentName, &ferr);
    } while (ferr.sp5fe_result == vf_ok);
    sqlfclosedirc (dirHandle, &ferr);
    tsp01_RteError rteErr;
    sqlremove_empty_dirc (dirname, &rteErr);
    ok = ok && (rteErr.RteErrCode == RTE_NO_ERROR_SP01);
    return ok;
}

/*----------------------------------------*/

static bool
readInt (
    RTEUDE_TokenIterator & tokeni,
    int            * resultParm)
{
    int result = 0;
    const char * token;
    int tokenLen;

    if (!tokeni.next (token, tokenLen)) {
        return false;
    }
    if (tokenLen > 5) {
        return false;
    }

    for (int i = 0; i < tokenLen; ++i) {
        char digit = token [i];
        if ((digit < '0') || ('9' < digit)) {
            return false;
        }
        result = result * 10 + (digit - '0');
    }
    *resultParm = result;
    return true;
}

/*----------------------------------------*/

static bool
readName (
    RTEUDE_TokenIterator & tokeni,
    char           * name,
    int              namesize)  // excludes trailing \0
{
    const char * token;
    int tokenLen;

    if (!tokeni.next (token, tokenLen)) {
        return false;
    }
    if (tokenLen > namesize) {
        return false;
    }

    memcpy (name, token, tokenLen);
    name [tokenLen] = '\0';
    return true;
}

/*----------------------------------------*/

static bool
readDbname (
    RTEUDE_TokenIterator & tokeni,
    tsp00_DbNamec  * dbname)
{
    return readName (tokeni, dbname->asCharp (), DBNAME_MXSP00);
}

/*----------------------------------------*/

static bool
readUdeId (
    RTEUDE_TokenIterator & tokeni,
    tsp00_Identifierc * name)
{
    return readName (tokeni, name->asCharp (), sizeof (tsp00_Identifier));
}

/*----------------------------------------*/

static bool
readUdePath (
    RTEUDE_TokenIterator & tokeni,
    char           * path,
    bool             withDbname,
    bool             withUdeid)
{
    strcpy (path, "tmpdirs");
    if (!withDbname) {
        return true;
    }
    /*
     * get dbname
     */
    tsp00_DbNamec dbname;
    if (!readDbname (tokeni, &dbname)) {
        return false;
    }
    strcat (path, PATH_DELIMITER);
    strcat (path, dbname);
    if (!withUdeid) {
        return true;
    }
    /*
     * get ude id
     */
    tsp00_Identifierc udeId;
    if (!readUdeId (tokeni, &udeId)) {
        return false;
    }
    strcat (path, PATH_DELIMITER);
    strcat (path, udeId);
    /*
     *
     */
    return true;
}

/*----------------------------------------*/
/*
 *  log (.*)
 */
static bool
logCmd (
    void                 * context,
    RTEUDE_TokenIterator & tokeni,
    RTEUDE_CommandReply  & reply)
{
    RelayContext* relay = (RelayContext *) context;
    relay->log->msg (tokeni.current (), tokeni.length ());
    return true;
}

/*----------------------------------------*/

/*
 * loglevel ([0-9]+)
 */
static bool
loglevelCmd (
    void                 * context,
    RTEUDE_TokenIterator & tokeni,
    RTEUDE_CommandReply  & reply)
{
    int newlevel;

    if (!readInt (tokeni, &newlevel)) {
        return false;
    }
    logLevel = newlevel;
    return true;
}

/*----------------------------------------*/

/*
 * stop
 */
static bool
stopCmd (
    void                 * context,
    RTEUDE_TokenIterator & tokeni,
    RTEUDE_CommandReply  & reply)
{
    RelayContext* relay = (RelayContext *) context;
    reply.stopLoop = true;
    if (logLevel > 0) {
        relay->log->msg ("stop request");
    }
    return true;
}

/*----------------------------------------*/

/*
 * start <dbname> <ude-id> <cmdline>
 */
static bool
startudeCmd (
    void                 * context,
    RTEUDE_TokenIterator & tokeni,
    RTEUDE_CommandReply  & reply)
{
    char fulldir [256];
    if (!readUdePath (tokeni, fulldir, true, true)) {
        return false;
    }
    if (!assureDirectory (fulldir)) {
        return reply.error ("failed to create Directory");
    }
    /*
     * cd to directory and start process
     */
    tokeni.skipSpace ();
#if defined (_WIN32)
    STARTUPINFO startUpInfo;
    PROCESS_INFORMATION procInfo;

    GetStartupInfo (&startUpInfo);
    int spawnOK = CreateProcess (NULL, (LPSTR) tokeni.current (),
        0, // processSecurity,
        0, // threadSecurity,
        FALSE,
        0,
        0, // environment
        fulldir, // currDir
        &startUpInfo,
        &procInfo);
    int spawnRC;
    if (!spawnOK) {
        spawnRC = GetLastError ();
    }
#else
#endif
    return true;
}

/*----------------------------------------*/

/*
 * clearude <ude-id>
 */
static bool
clearudeCmd (
    void                 * context,
    RTEUDE_TokenIterator & tokeni,
    RTEUDE_CommandReply  & reply)
{
    char fulldir [256];
    if (!readUdePath (tokeni, fulldir, true, true)) {
        return false;
    }
    Msg_List msgList;
    if (!RTEIO_ShellUtil::RemoveTree (fulldir, msgList)) {
        return reply.error ("clearude failed");
    }
    return true;
}

/*----------------------------------------*/

/*
 * cleardb <dbname>
 */
static bool
cleardbCmd (
    void                 * context,
    RTEUDE_TokenIterator & tokeni,
    RTEUDE_CommandReply  & reply)
{
    char fulldir [256];
    if (!readUdePath (tokeni, fulldir, true, false)) {
        return false;
    }
    Msg_List msgList;
    if (!RTEIO_ShellUtil::RemoveTree (fulldir, msgList)) {
        return reply.error ("cleardb failed");
    }
    return true;
}

/*----------------------------------------*/

/*
 * clearall
 */
static bool
clearallCmd (
    void                 * context,
    RTEUDE_TokenIterator & tokeni,
    RTEUDE_CommandReply  & reply)
{
    char fulldir [256];
    if (!readUdePath (tokeni, fulldir, false, false)) {
        return false;
    }
    Msg_List msgList;
    if (!RTEIO_ShellUtil::RemoveTree (fulldir, msgList)) {
        return reply.error ("clearall failed");
    }
    return true;
}

/*----------------------------------------*/

static RTEUDE_CommandLookup  lookupTable []  = {
    {"log",         0, logCmd},
    {"loglevel",    0, loglevelCmd},
    {"stop",        0, stopCmd},
    {"startude",    0, startudeCmd},
    {"clearude",    0, clearudeCmd},
    {"cleardb",     0, cleardbCmd},
    {"clearall",    0, clearallCmd},
    {NULL, 0, NULL}
};

/*----------------------------------------*/

bool RTEUDE_StarterRelay::
serve (
    RTEUDE_IPC & ipc,
    char       * errormessage)
{
    /*
     * open log
     */
    Log log;
    if (!log.isValid ()) {
        strcpy (errormessage, "failed to open log");
        return false;
    }
    log.msg ("started uderelay");
    if (!ipc.isValid ()) {
        char buf [30];
        sprintf (buf, "pipe error %d", GetLastError ());
        log.msg (buf);
    }
    else {
        log.msg ("pipe is open");
    }
    /*
     * handle requests
     */
    RelayContext context;
    context.log = &log;
    RTEUDE_CommandInterpreter interpreter (lookupTable);
    interpreter.run (&context, &ipc);
    /*
     * cleanup
     */
    return true;
}

/*----------------------------------------*/

