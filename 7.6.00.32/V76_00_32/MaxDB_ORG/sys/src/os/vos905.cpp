/*!
  @file           vos905.cpp
  @author         RaymondR
  @brief          sqlxconnect
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
#include "gos00.h"
#include "heo46.h"
#include "vsp004.h"
#include "gsp09.h"
#include "gos003.h"
#include "geo007_1.h"
#include "geo007_2.h"
#include "gos906.h"
#include "hsp77.h"

#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"

/*
 *  DEFINES
 */
#define MOD__  "VOS905C : "
#define MF__   MOD__"UNDEFINED"

#define LINGER_TIME            15
#define READY_MESSAGE_LEN       5

/*
 *  MACROS
 */


/*
 *  LOCAL TYPE AND STRUCT DEFINITIONS
 */


/*
 * EXTERNAL VARIABLES
 */


/*
 *  EXPORTED VARIABLES
 */


/*
 * LOCAL VARIABLES
 */

/*
 * LOCAL FUNCTION PROTOTYPES
 */

VOID sql03u_xconnect (PSZ                       pszServerNode,
                      PSZ                       pszServerDB,
                      PSZ                       pszServerDBRoot,
                      PSZ                       pszServerPgm,
                      TOSCM_COMMSTRUCTOR_FT    *fpCommstructor,
                      tsp00_Int4 &              ulMaxCmdDataLen,
                      PVOID                     pSQLPacketList[],
                      tsp00_Int4 &              ulClientRef,
                      ULONG &                   rc,
                      tsp00_ErrTextc &          errText);

static TOSCM_COMM_CLASS * sql905c_shmclient_constructor (
                    void             *commInfo,
                    tsp00_ErrTextc  VAR_ARRAY_REF    errtext,
                    tsp01_CommErr_Enum   *returncode);

/*
 * ========================== GLOBAL FUNCTIONS ================================
 */
VOID sqlx2connectc (PROCESS_ID                     pid,
                   PSZ                             servernode,
                   PSZ                             serverdb,
                   PATHNAME                        dbroot,
                   PATHNAME                        serverpgm,
                   INT4                            *reference,
                   INT4                            *sql_packet_size,
                   PVOID                           sql_packet_list[],
                   tsp00_ErrTextc  VAR_ARRAY_REF                  pErrText,
                   tsp01_CommErr                  *returncode )
  {
  #undef  MF__
  #define MF__ MOD__"sqlx2connectc"
  ULONG         rc;
  PATHNAME      locDbroot;

  sp77sprintf(locDbroot,sizeof(PATHNAME),"%s",dbroot);

  sql03u_xconnect (servernode, serverdb, locDbroot, serverpgm,
                   (TOSCM_COMMSTRUCTOR_FT *)sql905c_shmclient_constructor, *sql_packet_size,
                   sql_packet_list, *reference, rc, pErrText);

  returncode->becomes ((tsp01_CommErr_Enum) rc);
  }


/*----------------------------------------*/

VOID sqlx2connectp (PROCESS_ID                      pid,
                   SQL_NODEID                      servernode,
                   SQL_DBNAME                      serverdb,
                   PATHNAME                        dbroot,
                   PATHNAME                        serverpgm,
                   INT4                            *reference,
                   INT4                            *sql_packet_size,
                   PVOID                           sql_packet_list[],
                   tsp00_ErrText                   errtext,
                   tsp01_CommErr                  *returncode )
  {
  #undef  MF__
  #define MF__ MOD__"sqlx2connectp"
  ULONG         rc;
  SQL_NODEIDC     szServerNode;
  SQL_DBNAMEC     szServerDB;
  tsp00_ErrTextc  pErrText;

  DBGIN;

  // ---  convert pascal names to null terminated strings
  eo46PtoC ( szServerNode, (tsp00_Byte*)servernode.asCharp(),  sizeof(SQL_NODEID) );
  eo46PtoC ( szServerDB,   (tsp00_Byte*)serverdb.asCharp(),    sizeof(SQL_DBNAME) );


  // ---  If no node name is given, split database name into node/dbname parts.
  if ( szServerNode [ 0 ] == '\0' )
    sql17u_split_dbname ( szServerDB, szServerNode );

  DBG3 (( MF__, "pszServerNode '%s'", szServerNode ));
  DBG3 (( MF__, "pszServerDB   '%s'", szServerDB ));

  sql03u_xconnect (servernode, serverdb, dbroot, serverpgm,
                   sql905c_shmclient_constructor, *sql_packet_size,
                   sql_packet_list, *reference, rc, pErrText);
  returncode->becomes ((tsp01_CommErr_Enum) rc);
  if ( *returncode != commErrOk_esp01 )
      errtext.c2p (pErrText);
  }

/*----------------------------------------*/


VOID sqlxconnectc (PROCESS_ID                      pid,
                   char                           *servernode,
                   char                           *serverdb,
                   PATHNAME                        dbroot,
                   INT4                            *reference,
                   INT4                            *sql_packet_size,
                   PVOID                           sql_packet_list[],
                   char                           *errtext,
                   tsp01_CommErr                  *returncode )
  {
  #undef  MF__
  #define MF__ MOD__"sqlxconnectc"
  sqlx2connectc (pid, servernode, serverdb, dbroot, NULL, reference,
                 sql_packet_size, sql_packet_list,
                 *(tsp00_ErrTextc*) errtext, returncode);
  }

/*----------------------------------------*/

VOID sqlxconnectp (PROCESS_ID                      pid,
                   SQL_NODEID                      servernode,
                   SQL_DBNAME                      serverdb,
                   PATHNAME                        dbroot,
                   PATHNAME                        serverpgm,
                   INT4                            *reference,
                   INT4                            *sql_packet_size,
                   PVOID                           sql_packet_list[],
                   tsp00_ErrText                   errtext,
                   tsp01_CommErr                  *returncode )
  {
  #undef  MF__
  #define MF__ MOD__"sqlxconnectp"
  sqlx2connectp (pid, servernode, serverdb, dbroot, serverpgm, reference,
                 sql_packet_size, sql_packet_list, errtext, returncode);
  }


/*
 * ========================== LOCAL FUNCTIONS ================================
 */

/*!

  @class t_shmclient_class

 */
class ShmClient
    : public TOSCM_COMM_CLASS
{
public:
    virtual tsp01_CommErr_Enum connect (
                               CONNECT_PARAM_REC       *pConnParam,
                               PSZ                      pszServerDB,
                               PSZ                      pszDBRoot,
                               PSZ                      pszServerPgm,
                               tsp00_ErrTextc  VAR_ARRAY_REF           errtext);

    virtual tsp01_CommErr_Enum release ();

    virtual tsp01_CommErr_Enum request (
                               void             *sql_packet,
                               tsp00_Int4          length,
                               tsp00_ErrTextc  VAR_ARRAY_REF      errtext);

    virtual tsp01_CommErr_Enum receive (
                               void            **sql_packet,
                               tsp00_Int4      VAR_VALUE_REF      length,
                               tsp00_ErrTextc  VAR_ARRAY_REF      errtext);

    virtual tsp01_CommErr_Enum replyAvailable (
                               tsp00_ErrTextc  VAR_ARRAY_REF      errtext);

    virtual tsp01_CommErr_Enum cancel (
                               tsp00_ErrTextc  VAR_ARRAY_REF     errtext);

private:
    tsp01_CommErr_Enum        allocMem (
                                CONNECT_PARAM_REC*pConnParam,
                                tsp00_ErrTextc  VAR_ARRAY_REF   errtext);
    BOOL                      startProcess (
                                PSZ              szExecutable,
                                PSZ              szDBRoot,
                                PSZ              szDBName,
                                HANDLE           shmHandle,
                                HANDLE           writePipe,
                                DWORD            pid);

    tsp01_CommErr_Enum        spawnServer (
                                CONNECT_PARAM_REC   *pConnParam,
                                const char          *exepath,
                                PSZ                  pszServerDB,
                                PSZ                  pszDBRoot,
                                PSZ                  pszServerPgm,
                                tsp00_ErrTextc  VAR_ARRAY_REF       errtext);

private:
    HANDLE               shmHandle;
    HANDLE               requestEvent;
    HANDLE               replyEvent;
    tos906c_shmServer   *server;
    char                *packetMem;
    HANDLE               serverProcess;
    DWORD                serverId;
};

/*----------------------------------------*/

__inline static tsp00_Longuint
sql905c_pointerDiff (
   void *p1,
   void *p2)
{
#undef MF__
#define MF__ MOD__"sql905c_shmclient_connect"
    return (char*) p1 - (char*) p2;
}

/*----------------------------------------*/

tsp01_CommErr_Enum ShmClient::
allocMem (
   CONNECT_PARAM_REC*pConnParam,
   tsp00_ErrTextc  VAR_ARRAY_REF         errtext)
{
#undef MF__
#define MF__ MOD__"ShmClient::allocMem"
    ULONG               ulCnt;
    ULONG               shmSize;

    // alloc server struct and packets in shared memory
    shmSize = (sizeof (tos906c_shmServer) + SQL_PACKET_ALIGNMENT - 1)
            / SQL_PACKET_ALIGNMENT * SQL_PACKET_ALIGNMENT;
    pConnParam->ulPacketSize = csp9_ctrl_packet_size;
    pConnParam->ulMinReplySize = 100;
    pConnParam->ulMaxDataLen = pConnParam->ulPacketSize
        - (2 * RTE_HEADER_SIZE);
    shmSize += pConnParam->ulPacketCnt * pConnParam->ulPacketSize;
    this->shmHandle = CreateFileMapping ((HANDLE)-1, NULL,
            PAGE_READWRITE, 0, shmSize, NULL);
    if (this->shmHandle == NULL) {
        sql46c_build_error_string ( errtext,
                ERRMSG_COM_CANT_ALLOC_COM_SEG, GetLastError () );
        return commErrNotOk_esp01;
    }
    // fill all structures with this information
    this->server = (tos906c_shmServer*) MapViewOfFile (this->shmHandle,
            FILE_MAP_WRITE, 0, 0, 0);
    this->packetMem = (char*) (this->server + 1); // starts after server struct
    for ( ulCnt = 0; ulCnt < pConnParam->ulPacketCnt; ulCnt++ )
    {
      pConnParam->pCommPacketList[ulCnt] = (PCOMM_PACKET_REC)
              (this->packetMem + (ulCnt * pConnParam->ulPacketSize));
    }
    return commErrOk_esp01;
}

/*------------------------------*/

#if !defined DEBUGGER
#define DEBUGGER ""
#elif defined _WIN32
#undef DEBUGGER
#define DEBUGGER "devenv /debugexe "
#endif

BOOL ShmClient::
startProcess (
   PSZ              szExecutable,
   PSZ              szDBRoot,
   PSZ              szDBName,
   HANDLE           shmHandle,
   HANDLE           writePipe,
   DWORD            pid)
{
#undef  MF__
#define MF__ MOD__"ShmClient::startProcess"
  STARTUPINFO         startupInfo;
  PROCESS_INFORMATION procInfo;
  BOOL                success;
  char                cmdline [500];
  char                dbnameArg [100];

  GetStartupInfo (&startupInfo);
# if defined (_WIN64)
   sp77sprintf (cmdline,500, "%s%s -M %p -P 0,%p -R \"%s\"", DEBUGGER,
            szExecutable, shmHandle, writePipe, szDBRoot);
# else
   sp77sprintf (cmdline,500, "%s%s -M %x -P 0,%x -R \"%s\"", DEBUGGER,
            szExecutable, shmHandle, writePipe, szDBRoot);
# endif
  if ((szDBName != NULL) && (szDBName [0] != '\0'))
    {
    sp77sprintf (dbnameArg,100, " -d %s", szDBName);
    strcat (cmdline, dbnameArg);
    }

  if (RTE_UNDEF_OSPID != pid)
  {
    sp77sprintf (dbnameArg,100, " -p %d", pid);
    strcat (cmdline, dbnameArg);
  }

  startupInfo.dwFlags = STARTF_USESHOWWINDOW;
  startupInfo.wShowWindow = SW_HIDE;
  success = CreateProcess (0, cmdline, 0, 0, TRUE, 0,
      0, 0, &startupInfo, &procInfo);
  this->serverProcess = procInfo.hProcess;
  this->serverId = procInfo.dwProcessId;
  CloseHandle (procInfo.hThread);
  return success;
}

/*----------------------------------------*/

static HANDLE sql905c_changeHandle (
   HANDLE      *handlerAddr,
   int          access)
{
#undef MF__
#define MF__ MOD__"sql905c_shmclient_connect"
    HANDLE  currentProcess = GetCurrentProcess ();

    DuplicateHandle (currentProcess, *handlerAddr, currentProcess,
            handlerAddr, access, TRUE, DUPLICATE_CLOSE_SOURCE);
    return *handlerAddr;
}

/*----------------------------------------*/

tsp01_CommErr_Enum ShmClient::
spawnServer (
   CONNECT_PARAM_REC   *pConnParam,
   const char          *exepath,
   PSZ                  pszServerDB,
   PSZ                  pszDBRoot,
   PSZ                  pszServerPgm,
   tsp00_ErrTextc  VAR_ARRAY_REF       errtext)
{
#undef MF__
#define MF__ MOD__"ShmClient::spawnServer"
  HANDLE        parentRead;
  HANDLE        childWrite;
  BOOLEAN       ok;
  char          readbuf [READY_MESSAGE_LEN];
  DWORD         bytesRead;
  PATHNAME      executable;
  SECURITY_ATTRIBUTES secAttr;
  tsp00_ErrText errtextP;

  secAttr.nLength = sizeof (secAttr);
  secAttr.lpSecurityDescriptor = NULL;
  secAttr.bInheritHandle = TRUE;
  // look for server
  ok = sql97cc_findControlServer (executable, pszServerDB,
                                  pszServerPgm, pszDBRoot, errtextP);
  if (!ok)
    {
    errtext.p2c(errtextP);
    return commErrNotOk_esp01;
    }
  // create low level pipes
  ok = CreatePipe (&parentRead, &childWrite, &secAttr, 0);
  if (!ok)
    {
    sp77sprintf(errtext, sizeof(tsp00_ErrText),"Error creating pipes");
    return commErrNotOk_esp01;
    }
  // start server
  ok = this->startProcess (executable, pszDBRoot,
      pConnParam->pszServerDB, this->shmHandle, childWrite, pConnParam->pidClientPID);
  CloseHandle (childWrite);
  if (!ok)
    {
    sp77sprintf(errtext,sizeof(tsp00_ErrText),"Error starting %s",pszServerPgm);
    CloseHandle (parentRead);
    return commErrNotOk_esp01;
    }
  // receive ready message from server
  ok = ReadFile (parentRead, readbuf, READY_MESSAGE_LEN, &bytesRead, NULL);
  CloseHandle (parentRead);
  if (!ok || (bytesRead != READY_MESSAGE_LEN))
    {
    sp77sprintf(errtext,sizeof(tsp00_ErrText),"%s died",pszServerPgm);
    return commErrNotOk_esp01;
    }
  return commErrOk_esp01;
}

/*----------------------------------------*/

tsp01_CommErr_Enum ShmClient::
connect (
    CONNECT_PARAM_REC*pConnParam,
    PSZ               pszServerDB,
    PSZ               pszDBRoot,
    PSZ               pszServerPgm,
    tsp00_ErrTextc  VAR_ARRAY_REF    errtext)
{
#undef MF__
#define MF__ MOD__"ShmClient::connect"
    LONG                rc = NO_ERROR;
    PATHNAME            exepath;
    tsp01_CommErr_Enum      result;

    result = this->allocMem (pConnParam, errtext);
    if (result != commErrOk_esp01)
        return result;
    this->server->isAlive = FALSE;
    this->requestEvent = CreateEvent (NULL, FALSE, FALSE, NULL);
    this->replyEvent = CreateEvent (NULL, FALSE, FALSE, NULL);
    this->server->clientProcess = GetCurrentProcess ();
    sql905c_changeHandle (&this->server->clientProcess, PROCESS_QUERY_INFORMATION);
    if ((this->requestEvent == NULL)
            || (this->replyEvent == NULL))
    {
        sp77sprintf(errtext,sizeof(tsp00_ErrText),"Creation of events failed");
        return commErrNotOk_esp01;
    }
    sql905c_changeHandle (&this->requestEvent, EVENT_ALL_ACCESS);
    sql905c_changeHandle (&this->replyEvent, EVENT_ALL_ACCESS);
    this->server->requestEvent = this->requestEvent;
    this->server->replyEvent = this->replyEvent;
    sql905c_changeHandle (&this->shmHandle, FILE_MAP_ALL_ACCESS);
    result = this->spawnServer (pConnParam, exepath,
            pszServerDB, pszDBRoot, pszServerPgm, errtext);
    return result;
}

/*----------------------------------------*/

tsp01_CommErr_Enum ShmClient::
release ()
{
#undef MF__
#define MF__ MOD__"ShmClient::release"
    DWORD               waitRC;
    tsp01_CommErr_Enum       result = commErrOk_esp01;
    BOOL                terminateOK;

    waitRC = WaitForSingleObject (this->serverProcess, 5000);

    if (waitRC == WAIT_TIMEOUT) {
        terminateOK = TerminateProcess (this->serverProcess, 100);
        if (terminateOK) {
            waitRC = WaitForSingleObject (this->serverProcess, 2000);
        }
        else {
            result = commErrNotOk_esp01;
        }
    }
    CloseHandle (this->serverProcess);
    if (this->requestEvent != NULL) {
        CloseHandle (this->requestEvent);
    }
    if (this->replyEvent != NULL) {
        CloseHandle (this->replyEvent);
    }
    UnmapViewOfFile (this->server);
    CloseHandle (this->shmHandle);
    FREE_MEM (this);
    return result;
}

/*----------------------------------------*/

tsp01_CommErr_Enum ShmClient::
request (
    void             *sql_packet,
    tsp00_Int4          length,
    tsp00_ErrTextc  VAR_ARRAY_REF      errtext)
{
#undef MF__
#define MF__ MOD__"ShmClient::request"

    if (!this->server->isAlive) {
        sp77sprintf(errtext,sizeof(tsp00_ErrText),"Connection broken");
        return commErrNotOk_esp01;
    }
    this->server->requestOffset =
            (int)sql905c_pointerDiff (sql_packet, this->packetMem);
    this->server->requestLen = length;
    this->server->replyAvailable = FALSE;
    SetEvent (this->requestEvent);
    if (!this->server->isAlive) {
        sp77sprintf(errtext,sizeof(tsp00_ErrText),"Connection broken");
        return commErrNotOk_esp01;
    } else {
        errtext [0] = '\0';
        return commErrOk_esp01;
    }
}

/*----------------------------------------*/

tsp01_CommErr_Enum ShmClient::
receive (
    void **                             sql_packet,
    tsp00_Int4         VAR_VALUE_REF    length,
    tsp00_ErrTextc  VAR_ARRAY_REF       errtext)
{
#undef MF__
#define MF__ MOD__"ShmClient::receive"
    BOOL               eventSignaled;

    if (!this->server->isAlive) {
        sp77sprintf(errtext,sizeof(tsp00_ErrText),"Connection broken");
        return commErrNotOk_esp01;
    }
    sql906c_waitForEvent (this->replyEvent, this->serverProcess,
            &eventSignaled);
    if (!eventSignaled) {
        sp77sprintf(errtext,sizeof(tsp00_ErrText),"Connection broken");
        return commErrNotOk_esp01;
    }
    *sql_packet = this->packetMem + this->server->replyOffset;
    length = this->server->replyLen;
    errtext [0] = '\0';
    return commErrOk_esp01;
}

/*----------------------------------------*/

tsp01_CommErr_Enum ShmClient::
replyAvailable (
    tsp00_ErrTextc  VAR_ARRAY_REF    errtext)
{
#undef MF__
#define MF__ MOD__"ShmClient::replyAvailable"

    errtext [0] = '\0';
    if (this->server->replyAvailable)
        return commErrOk_esp01;
    else if (!this->server->isAlive)
        return commErrOk_esp01;
    else
        return commErrWouldBlock_esp01;
}

/*----------------------------------------*/

tsp01_CommErr_Enum ShmClient::
cancel (
    tsp00_ErrTextc  VAR_ARRAY_REF    errtext)
{
#undef MF__
#define MF__ MOD__"ShmClient::cancel"
    tsp00_ErrText errtextPascal;
    tsp01_CommErr_Enum result =
        sql906c_shmSendCancel (this->serverId, errtextPascal);
    if (result != commErrOk_esp01) {
        errtext.p2c (errtextPascal);
    }
    return result;
}

/*----------------------------------------*/

TOSCM_COMM_CLASS *
sql905c_shmclient_constructor (
    void             *commInfo,
    tsp00_ErrTextc  VAR_ARRAY_REF    errtext,
    tsp01_CommErr_Enum   *returncode)
{
#undef MF__
#define MF__ MOD__"sql905c_shmclient_constructor"
    ShmClient  *self;
    APIRET              rc;

    // alloc self
    rc = ALLOC_MEM ((PPVOID)&self, sizeof(ShmClient) );
    if( rc != NO_ERROR ) {
        sql46c_build_error_string ( errtext, ERRMSG_ALLOC_MEMORY, rc );
        *returncode = commErrNotOk_esp01;
        return NULL;
    }
    memset (self, '\0', sizeof (ShmClient));
    new (self) ShmClient;
    *returncode = commErrOk_esp01;
    return self;
}

/*
 * =============================== END ========================================
 */
