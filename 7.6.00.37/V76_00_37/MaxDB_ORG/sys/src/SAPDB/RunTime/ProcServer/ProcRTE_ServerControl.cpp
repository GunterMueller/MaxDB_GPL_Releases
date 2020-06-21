/*!*********************************************************************

    module:       ProcRTE_ServerControl.cpp

    --------------------------------------------------------------------

    responsible:  DanielD

    special area:

    description:

    --------------------------------------------------------------------


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




***********************************************************************/

#include "RunTime/ProcServer/ProcRTE_ServerControl.hpp"

#include <stddef.h>
#include <stdlib.h>
#include <ctype.h>

#if defined (WIN32)

#include <windows.h>

#else

#include <errno.h>
#define GetLastError() errno

#endif

#include "hsp77.h"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Contracts.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp"
#include "RunTime/ProcServer/ProcRTE_Runtime.hpp"
#include "RunTime/ProcServer/ProcRTE_ServerSession.hpp"
#include "RunTime/ProcServer/ProcRTE_Messages.hpp"
#define KERNEL_LZU
#include "RunTime/Tasking/RTETask_Task.hpp"
#include "RunTime/Tasking/RTETask_ITaskClientConnection.hpp"
#undef KERNEL_LZU
#include "RunTime/UDEServer/RTEUDE_SocketIPC.hpp"

class ProcRTE_ServerControlPrivates {
public:
    RTEUDE_SocketIPC     adminChannel;
};

/*----------------------------------------*/

/*!

  @class ProcRTE_StartingServerControl
  @brief initiate communication by starting the server and
    waiting for an admin request

 */
class ProcRTE_StartingServerControl
: public ProcRTE_ServerControl
{
public:
    ProcRTE_StartingServerControl (const char * commandTemplate,
                                   SAPDB_Int4   templateLen);
    virtual bool connect (SAPDBErr_MessageList * messageList);
private:
    char commandTemplate [255];
    int  commandTemplateLen;
};

/*----------------------------------------*/

/*!

  @class ProcRTE_RemoteServerControl
  @brief initiate communication by connecting to a socket

 */
class ProcRTE_RemoteServerControl
: public ProcRTE_ServerControl
{
public:
    ProcRTE_RemoteServerControl (const char * commandTemplate,
                                 SAPDB_Int4   templateLen);
    virtual bool connect (SAPDBErr_MessageList * messageList);
private:
    int port;
    char hostname [100];
};

/*----------------------------------------*/

class BufWriter
{
public:
    BufWriter (char * buf_init) {
        this->buf = buf_init;
    }
    void add (const char * data, int len) {
        memcpy (this->buf, data, len);
        this->buf += len;
    }
    void add  (const char * data) {
        this->add (data, strlen (data));
    }
    void addToken (const char * data) {
        this->add (data);
        this->add (' ');
    }
    void add (char ch) {
        this->buf [0] = ch;
        ++this->buf;
    }
    void close () {
        this->add ('\0');
    }
private:
    char * buf;
};


/*----------------------------------------*/

ProcRTE_ServerControl * ProcRTE_ServerControl::
createServerControl (
    bool         connectToRunning,
    const char * commandTemplate,
    SAPDB_Int4   templateLen,
    SAPDBErr_MessageList * messageList)
{
    ProcRTE_ServerControl * result;
    bool ok;

    if (connectToRunning) {
        result = PROCRTE_RUNTIME_NEW (ProcRTE_RemoteServerControl) (commandTemplate, templateLen);
    }
    else {
        result = PROCRTE_RUNTIME_NEW (ProcRTE_StartingServerControl) (commandTemplate, templateLen);
    }
    SAPDBERR_MISSINGCHECK(result != NULL);
    ok = result->connect (messageList);
    if (ok) {
        /* send ping to check server is alive */
        ProcRTE_Runtime::log ("ping proc server");
        result->running = result->simpleCmd ("ping");
        if (!result->running) {
            *messageList = SAPDBErr_MessageList (PROCRTE_COMM_PING_FAILED);
        }
    }
    if (!ok) {
        result->~ProcRTE_ServerControl ();
        ProcRTE_Runtime::free (result);
        return NULL;
    }
    return result;
}

/*----------------------------------------*/

void ProcRTE_ServerControl::
stop ()
{
    if (this->running) {
        //
        this->running = false;
        this->simpleCmd ("stop");
        this->privates->adminChannel.close ();
    }
}

/*----------------------------------------*/

RTETask_ITaskClientConnection * ProcRTE_ServerControl::
createSession (
    SAPDBErr_MessageList * messageList)
{
    /*
     * send session command
     */
    char newSessionCmd [100];
    RTETask_Task * task = static_cast<RTETask_Task*>(RTETask_ITask::Self ());
    sp77sprintf (newSessionCmd, sizeof (newSessionCmd), "%s %s",
        "newsession", task->ClientConnections().SessionURIString());
    bool sendCommandOK = this->simpleCmd (newSessionCmd);
    if (!sendCommandOK) {
        *messageList = SAPDBErr_MessageList (PROCRTE_SEND_NEWSESSION_FAILED);
        return NULL;
    }
    /*
     * wait for connection
     */
    RTETask_ITaskClientConnection * result = task->AcceptClientConnect ();
    if (result == NULL) {
        *messageList = SAPDBErr_MessageList (PROCRTE_SEND_NEWSESSION_FAILED);
        return NULL;
    }
    /*
     * return
     */
    return result;
}

/*----------------------------------------*/

void ProcRTE_ServerControl::
releaseSession (
    RTETask_ITaskClientConnection * session)
{
    session->Release ();
}

/*----------------------------------------*/

bool ProcRTE_ServerControl::
isRunning () const
{
    return this->running;
}

/*----------------------------------------*/

ProcRTE_ServerControl::
ProcRTE_ServerControl ()
{
    this->running = false;
    this->privates = PROCRTE_RUNTIME_NEW (ProcRTE_ServerControlPrivates);
}

/*----------------------------------------*/

ProcRTE_ServerControl::
~ProcRTE_ServerControl ()
{
    this->stop ();
    ProcRTE_Runtime::free (this->privates);
}

/*----------------------------------------*/

bool ProcRTE_ServerControl::
tryReconnect (
    SAPDBErr_MessageList * messageList)
{
    this->privates->adminChannel.close ();
    return this->connect (messageList);
}

/*----------------------------------------*/

static bool
findVariable (
    const char * readpos,
    const char * endpos,
    const char ** varstart,
    int         * varlen)
{
    const char * match = (const char *) memchr (readpos, '$', endpos - readpos);
    if (match == NULL) {
        return false;
    }
    if (match [1] != '{') {
        return false;
    }
    const char * endmatch = (const char *) memchr (match + 2, '}', endpos - readpos - 2);
    if (endmatch == NULL) {
        return false;
    }
    *varstart = match;
    *varlen = endmatch - match + 1;
    return true;
}

/*----------------------------------------*/

static bool
isVariable (
    const char * varpos,
    int          varlen,
    const char * staticname,
    int          staticnameLen)
{
    /*
     * subtract ${}
     */
    varpos += 2;
    varlen -= 3;
    /*
     * compare
     */
    if ((varlen == staticnameLen) && (memcmp (varpos, staticname, varlen) == 0)) {
        return true;
    }
    else {
        return false;
    }
}

/*----------------------------------------*/

static char javaudeCommandLine [1024];
static int javaudeCommandLineLen = -1;

static void
fillJavaudeCommandLine (
    const char * dbname,
    const char * udeid)
{
    BufWriter writer (javaudeCommandLine);
    const char * dbroot = getenv ("DBROOT");
    SAPDBERR_MISSINGCHECK2 (dbroot != NULL, "DBROOT not set");
    writer.add ('"');
    writer.add (dbroot);
    writer.add ("/pgm/javaude-con");
#if defined (_WIN32)
    writer.add (".exe");
#endif
    writer.add ('"');
    writer.add (" -instroot ");
    writer.add ('"');
    writer.add (dbroot);
    writer.add ('"');
    writer.add (" -configfile ");
    writer.add ('"');
    writer.add (dbroot);
    writer.add ("/config/javaude.conf");
    writer.add ('"');
    writer.close ();
    javaudeCommandLineLen = strlen (javaudeCommandLine);
}

/*----------------------------------------*/

static const char * portName = "port";
static int portNameLen = 4;
static const char * javaudeName = "javaude";
static int javaudeNameLen = 7;

typedef struct VariableT {
    const char * name;
    const char * value;
    int          valueLen;
    int          startpos;
    int          endpos;
} VariableT;


static const char*
buildCommandLine (
    const char * commandTemplate,
    int          templateLen,
    int          portNumber)
{
    const char * templateEnd = commandTemplate + templateLen;
    const char * readpos = commandTemplate;
    static const char * startude = "startude";
    char         portBuffer [20];
    int          portBufferLen;
    bool         portFound = false;
    bool         javaudeFound = false;
    const char * dbname = "DUMMYDB";
    const char * udeid = "SQLJ";
    int          outputLength = strlen (startude) + 1 + strlen (dbname) + 1
                        + strlen (udeid) + 1;

    while (isspace (templateEnd [-1]) && (templateEnd > commandTemplate)) {
        --templateEnd;
    }
    sp77sprintf (portBuffer, sizeof (portBuffer), "%d", portNumber);
    portBufferLen = strlen (portBuffer);
    /*
     * find variables of the form ${name}
     */
    const char * varpos;
    int          varlen;
    while (findVariable (readpos, templateEnd, &varpos, &varlen)) {
        outputLength += varpos - readpos;
        if (isVariable (varpos, varlen, portName, portNameLen)) {
            outputLength += portBufferLen;
            portFound = true;
        }
        else if (isVariable (varpos, varlen, javaudeName, javaudeNameLen)) {
            if (javaudeCommandLineLen == -1) {
                fillJavaudeCommandLine (dbname, udeid);
            }
            outputLength += javaudeCommandLineLen;
            javaudeFound = true;
        }
        else {
            outputLength += varlen;
        }
        readpos = varpos + varlen;
    }
    outputLength += templateEnd - readpos;
    if (! portFound) {
        outputLength += 1 + portBufferLen;
    }
    /*
     * allocate result and copy strings
     */
    char * result = (char *) ProcRTE_Runtime::alloc (outputLength + 1);
    BufWriter writer (result);
    writer.addToken (startude);
    writer.addToken (dbname);
    writer.addToken (udeid);
    int plainlen;
    readpos = commandTemplate;
    while (findVariable (readpos, templateEnd, &varpos, &varlen)) {
        /*
         * copy unchanged data
         */
        plainlen = varpos - readpos;
        if (plainlen > 0) {
            writer.add (readpos, plainlen);
        }
        /*
         * copy variable values
         */
        if (isVariable (varpos, varlen, portName, portNameLen)) {
            writer.add (portBuffer, portBufferLen);
        }
        else if (isVariable (varpos, varlen, javaudeName, javaudeNameLen)) {
            writer.add (javaudeCommandLine, javaudeCommandLineLen);
        }
        else {
            writer.add (varpos, varlen);
        }
        readpos = varpos + varlen;
    }
    /*
     * copy plain text after last variable
     */
    plainlen = templateEnd - readpos;
    if (plainlen > 0) {
        writer.add (readpos, plainlen);
    }
    /*
     * add port at end
     */
    if (! portFound) {
        writer.add (' ');
        writer.add (portBuffer, portBufferLen);
    }
    /*
     * close result
     */
    writer.close ();
    return result;
}

/*----------------------------------------*/

static const char * const remotePatternC = "sdb:remoteude://";
static int remotePatternLen = -1;

static const char *
findRemoteURI (
    const char           * commandTemplate,
    int                    templateLen,
    bool                   fullURI)
{
    if (remotePatternLen == -1) {
        remotePatternLen = strlen (remotePatternC);
    }
    const char * current = commandTemplate;
    while (isspace (*current)) {
        ++current;
    }
    if (!fullURI) {
        return current;
    }
    if (templateLen - (current - commandTemplate) < remotePatternLen) {
        return NULL;
    }
    if (memcmp (current, remotePatternC, remotePatternLen) == 0) {
        return current;
    }
    return NULL;
}

/*----------------------------------------*/

/*
    Parse connect information from commandTemplate

    fullURI: sdb:remoteude://[<host>:]<port>
    !fullURI: [<host>:]<port>
 */

static bool
connectToRemoteServer (
    const char           * commandTemplate,
    int                    templateLen,
    RTEUDE_SocketIPC     * adminChannel,
    SAPDBErr_MessageList * messageList,
    bool                   fullURI)
{
    const char * uri = findRemoteURI (commandTemplate, templateLen, fullURI);
    const char * uriEnd = commandTemplate + templateLen;
    /*
     * read host
     */
    const char * hoststart = uri + (fullURI ? remotePatternLen : 0);
    const char * colonPos = (const char *) memchr (hoststart, ':', uriEnd - hoststart);
    const char * hostend;
    if (colonPos == NULL) {
        hostend = hoststart;
    }
    else {
        hostend = colonPos;
    }
    char hostname [100];
    int hostnameLen = hostend - hoststart;
    if (hostnameLen + 1 > sizeof (hostname)) {
        *messageList = SAPDBErr_MessageList (PROCRTE_COMM_URI_INVALID_HOSTNAME);
        return false;

    }
    if (hostnameLen > 0) {
        memcpy (hostname, hoststart, hostnameLen);
    }
    hostname [hostnameLen] = '\0';
    /*
     * read port
     */
    int port = 0;
    const char * portString;
    if (colonPos == NULL) {
        portString = hostend;
    }
    else {
        portString = colonPos + 1;
    }
    while ((portString < uriEnd) && (isdigit (*portString))) {
        port = port * 10 + (*portString - '0');
        ++portString;
    }
    if (port == 0) {
        *messageList = SAPDBErr_MessageList (PROCRTE_COMM_URI_PORT_IS_MISSING);
        return false;
    }
    /*
     * connect
     */
    RTEUDE_ClientSocket client;
    if (!client.connect (port, hostname)) {
        *messageList = SAPDBErr_MessageList (PROCRTE_COMM_REMOTE_UDE_CONNECT_FAILED);
        return false;
    }
    *adminChannel = client;
    client.detach ();
    /*
     *
     */
    return true;
}

/*----------------------------------------*/

static bool
startUdeServer (
    const char           * commandTemplate,
    int                    templateLen,
    RTEUDE_SocketIPC     * adminChannel,
    SAPDBErr_MessageList * messageList)
{
    bool ok;
    /*
     * start socket server
     */
    RTEUDE_SocketServer socketServer (ProcRTE_Runtime::allocator ());
    ok = socketServer.listenOnDynamicPort ();
    if (!ok) {
        *messageList = SAPDBErr_MessageList (PROCRTE_COMM_LISTEN_FAILED);
        return false;
    }
    /*
     * expand port variable in command line
     */
    const char * commandLine = buildCommandLine (commandTemplate,
        templateLen, socketServer.getPort ());
    if (commandLine == NULL) {
        *messageList = SAPDBErr_MessageList (PROCRTE_OUT_OF_MEMORY, "?");
        return false;
    }
    /*
     * send command line to relay
     */
    RTEUDE_ClientSocket relayClient (7211, NULL);
    if (!relayClient.isValid ()
            || !relayClient.send (commandLine, strlen (commandLine)))
    {
        ProcRTE_Runtime::free ((void*) commandLine);
        ProcRTE_Runtime::logError ("send to relay failed");
        *messageList = SAPDBErr_MessageList (PROCRTE_COMM_RELAY_MESSAGE_SEND_FAILED);
        return false;
    }
    relayClient.close ();
    ProcRTE_Runtime::free ((void*) commandLine);
    /*
     * complete admin handshake
     */
    bool acceptOK = socketServer.accept (adminChannel);
    if (!acceptOK) {
        *messageList = SAPDBErr_MessageList (PROCRTE_COMM_ACCEPT_FAILED);
        return false;
    }
    return true;
}

/*----------------------------------------*/

bool ProcRTE_ServerControl::
simpleCmd (
    const SAPDB_Char * cmdString)
{
    return this->simpleCmd (cmdString, strlen (cmdString));
}

/*----------------------------------------*/

bool ProcRTE_ServerControl::
simpleCmd (
    const SAPDB_Char * cmdString,
    int                cmdlen)
{
    char resultbuf [20];
    int  resultLen;
    bool callOK;

    callOK = this->privates->adminChannel.call (cmdString, cmdlen,
        resultbuf, sizeof (resultbuf), resultLen);
    if (!callOK) {
        SAPDBErr_MessageList messageList;
        if (this->tryReconnect (&messageList)) {
            callOK = callOK = this->privates->adminChannel.call (cmdString,
                cmdlen, resultbuf, sizeof (resultbuf), resultLen);
        }
    }
    if (!callOK) {
        return false;
    }
    return strncmp (resultbuf, "OK", 2) == 0;
}

/*----------------------------------------*/

ProcRTE_StartingServerControl::
ProcRTE_StartingServerControl (
    const char * commandTemplate_init,
    SAPDB_Int4   templateLen_init)
{
    if (templateLen_init > sizeof (this->commandTemplate)) {
        return;
    }
    memcpy (this->commandTemplate, commandTemplate_init, templateLen_init);
    this->commandTemplateLen = templateLen_init;
}

/*----------------------------------------*/

bool ProcRTE_StartingServerControl::
connect (
    SAPDBErr_MessageList * messageList)
{
    return startUdeServer (this->commandTemplate, this->commandTemplateLen,
        &this->privates->adminChannel, messageList);
}

/*----------------------------------------*/


ProcRTE_RemoteServerControl::
ProcRTE_RemoteServerControl (
    const char * commandTemplate,
    SAPDB_Int4   templateLen)
{
    this->port = -1;
    this->hostname [0] = '\0';
    bool fullURI = false;
    const char * uri = findRemoteURI (commandTemplate, templateLen, fullURI);
    const char * uriEnd = commandTemplate + templateLen;
    /*
     * read host
     */
    const char * hoststart = uri + (fullURI ? remotePatternLen : 0);
    const char * colonPos = (const char *) memchr (hoststart, ':', uriEnd - hoststart);
    const char * hostend;
    if (colonPos == NULL) {
        hostend = hoststart;
    }
    else {
        hostend = colonPos;
    }
    int hostnameLen = hostend - hoststart;
    if (hostnameLen + 1 > sizeof (hostname)) {
        return;
    }
    if (hostnameLen > 0) {
        memcpy (this->hostname, hoststart, hostnameLen);
    }
    this->hostname [hostnameLen] = '\0';
    /*
     * read port
     */
    this->port = 0;
    const char * portString;
    if (colonPos == NULL) {
        portString = hostend;
    }
    else {
        portString = colonPos + 1;
    }
    while ((portString < uriEnd) && (isdigit (*portString))) {
        this->port = this->port * 10 + (*portString - '0');
        ++portString;
    }
    if (port == 0) {
        port = -1;
    }
}

/*----------------------------------------*/

bool ProcRTE_RemoteServerControl::
connect (
    SAPDBErr_MessageList * messageList)
{
    if (this->port < 0) {
        *messageList = SAPDBErr_MessageList (PROCRTE_COMM_URI_INVALID_HOSTNAME);
        return false;
    }
    RTEUDE_ClientSocket client;
    if (!client.connect (port, hostname)) {
        *messageList = SAPDBErr_MessageList (PROCRTE_COMM_REMOTE_UDE_CONNECT_FAILED);
        return false;
    }
    this->privates->adminChannel = client;
    client.detach ();
    return true;
}

