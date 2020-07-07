/*!
  @file           vos906.c
  @author         RaymondR
  @brief          Control Server Communication
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
#define RTE_USE_SOCK

#include "gos00.h"
#include "heo46.h"
#include "vsp001c.h"
#include "gsp09.h"
#include "geo007_1.h"
#include "gos003.h"
#include "gos96.h"
#include "heo420.h"
#include "heo44.h"
#include "gos906.h"

#include <direct.h>

/*
 *  DEFINES
 */
#define MOD__  "VOS906C : "
#define MF__   MOD__"UNDEFINED"

/*
 *  MACROS
 */

/*
 *  LOCAL TYPE AND STRUCT DEFINITIONS
 */
typedef struct tos906c_ShmOptions 
{
  HANDLE      shmHandle;
  HANDLE      writeHandle;
  HANDLE      readHandle;
  char*       serverdb;
  char*       dbroot;
  RTE_OSPid   senderPID;
} tos906c_ShmOptions;


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
static void           sql906c_slotName ( char* target,
                                         DWORD value );

static DWORD WINAPI   sql906c_ShmWatcherThread ( void* slotParm );
static void           sql906c_pConnection ( HANDLE writeHandle );
static BOOL           sql906c_StartShmCancelThread ( teo906_Connection * pConnection );
static void           sql906c_StopShmCancelThread  ( teo906_Connection * pConnection );
static BOOL           sql906c_ParseShmOptions ( tos906c_ShmOptions *options,
                                                int                argc,
                                                char*              argv [] );
static void           sql906c_SendReadyMessage ( HANDLE writeHandle );

/*
 * ========================== GLOBAL FUNCTIONS ================================
 */

void sqlxlogon ( const char      *username,
                 const char      *passwd,
                 tsp9_rte_xerror *xerror )
{
#undef MF__
#define MF__ MOD__"sqlxlogon"
  
  HANDLE   logonToken;
  BOOL     ok = FALSE;
  char *   UserName;
  char *   DomainName;

  eo44initError (xerror);

  UserName = strrchr ( username, '/' );
  if ( UserName == NULL ) {
    UserName = strrchr ( username, '\\' );
  } // end if
  
  if ( UserName != NULL )
  {
    size_t DomainNameLen = UserName - username;
    
    DomainName = (char*)_alloca(DomainNameLen + 1);
    
    if ( DomainName != NULL )
    {
      strncpy ( DomainName, username, DomainNameLen  );
      DomainName[DomainNameLen] = '\0';
    }
    
    UserName += 1;
  }
  else
  {
    DomainName = ".";
    UserName   = (char *)username;
  }
  
  if (LogonUser ( UserName, 
                  DomainName, 
                  (char *)passwd,
                  LOGON32_LOGON_BATCH, 
                  LOGON32_PROVIDER_DEFAULT, 
                  &logonToken)) 
  {
    if (ImpersonateLoggedOnUser (logonToken)) 
      ok = TRUE;
  }
  
  if (!ok) 
  {
    int osError = GetLastError ();

    if ( ERROR_LOGON_TYPE_NOT_GRANTED == osError )
        eo44anyError( xerror, "Logon failure: the user has not been granted the 'logon as a batch job' right. Please contact your administrator."  );
    else
    {
      LPVOID lpMsgBuf;

      FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                     FORMAT_MESSAGE_FROM_SYSTEM | 
                     FORMAT_MESSAGE_IGNORE_INSERTS,
                     NULL,
                     osError,
                     MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                     (LPTSTR) &lpMsgBuf,
                     0,
                     NULL );

        eo44anyError (xerror, lpMsgBuf);
        LocalFree( lpMsgBuf );
    }

    xerror->xe_native_error = osError;
  }
}

/*----------------------------------------*/

void sqlxidlogon ( int              userid,
                   tsp9_rte_xerror *xerror )
{
#undef MF__
#define MF__ MOD__"sqlxidlogon"

    eo44initError (xerror);
    eo44anyError (xerror, "no logon by id possible");
}
  
/*----------------------------------------*/

BOOL sql906c_ItsAShmConnectRequest ( int                argc,
                                     char*              argv [] )
{
# undef MF__
# define MF__ MOD__"sql906c_ItsAShmConnectRequest"
  int i;
  
  for (i = 1; i < argc; ++i) 
  {
    if (argv[i][0] != '-') 
      break;

    if ( argv[i][1] == 'M' ) 
      return TRUE;
  }

  return FALSE;
}

/*----------------------------------------*/

teo906_Connection* sql906c_shmConnect ( int                         argc,
                                        char*                       argv [],
                                        char const ** const         ppServerDB,
                                        char const ** const         ppDBRoot,
                                        SAPDB_Char * const          pSenderNodeId,
                                        RTE_OSPid *                 pSenderPID)
{
# undef MF__
# define MF__ MOD__"sql906c_shmConnect"

  teo906_Connection    *pConnection;
  tos906c_ShmOptions    Options;
  SAPDB_UInt4           bufferSize;

  if ( sql906c_ParseShmOptions ( &Options, argc, argv ) == FALSE )
    return NULL;

  pConnection = malloc (sizeof (teo906_Connection));

  if (pConnection == NULL) 
  {
    printf ("Out of memory error\n");
    return NULL;
  }

  pConnection->ShmComm = MapViewOfFile (Options.shmHandle, FILE_MAP_WRITE, 0, 0, 0);

  if ( pConnection->ShmComm == NULL )  
  {
    printf ("Error attaching SHM, rc = %d\n", GetLastError());
    return NULL;
  }

  // fill struct
  pConnection->ShmComm->shmHandle = Options.shmHandle;
  pConnection->ShmComm->packetMem = (char*) (pConnection->ShmComm + 1);
  pConnection->ShmComm->isAlive   = TRUE;

  pConnection->ItsAShmConnection  = TRUE;

  *ppServerDB = Options.serverdb;
  *ppDBRoot   = Options.dbroot;
  *pSenderPID = Options.senderPID;
  bufferSize = sizeof (tsp00_NodeIdc);
  if (!GetComputerNameEx (ComputerNameDnsFullyQualified, pSenderNodeId, &bufferSize))
  {
    pSenderNodeId[0]  = '\0'; 
  }

  if ( sql906c_StartShmCancelThread ( pConnection ) == FALSE )
    return NULL;

  /* write Ready message to pipe */
  sql906c_SendReadyMessage (Options.writeHandle);

  CloseHandle (Options.writeHandle);
  CloseHandle (Options.readHandle);

  return pConnection;
}


/*----------------------------------------*/

tsp01_CommErr_Enum sql906c_shmReceive ( teo906_Connection *   pConnection,
                                        char** const          RecvDataBuffer,
                                        int*                  RecvLen,
                                        char** const          SendDataBuffer,
                                        int* const            MaxSendLen )
{
# undef MF__
# define MF__ MOD__"sql906c_shmReceive"
  BOOL eventSignaled;
  
  sql906c_waitForEvent ( pConnection->ShmComm->requestEvent, 
                         pConnection->ShmComm->clientProcess,
                         &eventSignaled );
  if (!eventSignaled) 
    return commErrNotOk_esp01;

  // calculate
  pConnection->ShmComm->requestPacket = pConnection->ShmComm->packetMem + 
                                        pConnection->ShmComm->requestOffset;

  // pConnection->ShmComm->requestLen is set by client
  pConnection->ShmComm->replyOffset   = pConnection->ShmComm->requestOffset + 
                                        pConnection->ShmComm->requestLen; /* points
  to start of rte header */
  pConnection->ShmComm->replyPacket   = pConnection->ShmComm->packetMem + 
                                        pConnection->ShmComm->replyOffset +
                                        sizeof (RTE_HEADER_REC);                          /* points
  to start of actual data */
  *RecvDataBuffer                     = pConnection->ShmComm->requestPacket;
  *RecvLen                            = pConnection->ShmComm->requestLen;
  *SendDataBuffer                     = pConnection->ShmComm->replyPacket;
  *MaxSendLen                         =  csp9_ctrl_packet_size -  
                                         pConnection->ShmComm->requestLen -
                                         (3 * sizeof (RTE_HEADER_REC)) - 
                                         DBM_MIN_RESPONSE_PACKET_SIZE;
  return commErrOk_esp01;
}

/*----------------------------------------*/

tsp01_CommErr_Enum sql906c_shmSend ( teo906_Connection *   pConnection,
                                     int                   SendLen )
{
# undef MF__
# define MF__ MOD__"sql906c_shmSend"

  pConnection->ShmComm->replyLen = SendLen;
  pConnection->ShmComm->replyAvailable = TRUE;
  SetEvent (pConnection->ShmComm->replyEvent);

  return commErrOk_esp01;
}

/*----------------------------------------*/

void sql906c_shmRelease ( teo906_Connection *   pConnection )
{
# undef MF__
# define MF__ MOD__"sql906c_shmRelease"

  HANDLE shmHandle = pConnection->ShmComm->shmHandle;

  sql906c_StopShmCancelThread( pConnection );

  pConnection->ShmComm->isAlive = FALSE;
  CloseHandle (pConnection->ShmComm->requestEvent);
  CloseHandle (pConnection->ShmComm->replyEvent);
  CloseHandle (pConnection->ShmComm->clientProcess);
  UnmapViewOfFile (pConnection->ShmComm);
  CloseHandle (shmHandle);
  free(pConnection);
}

/*----------------------------------------*/

tsp01_CommErr_Enum sql906c_shmSendCancel ( DWORD        processId,
                                           tsp00_ErrText  errtext)
{
#undef MF__
#define MF__ MOD__"sql906c_shmSendCancel"
  HANDLE      mailSlot;
  char        name [100];
  DWORD       bytesWritten;
  
  errtext [0] = '\0';
  // open mailslot
  sql906c_slotName (name, processId);

  mailSlot = CreateFile (name, GENERIC_WRITE, FILE_SHARE_READ, NULL,
                         OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

  if (mailSlot == INVALID_HANDLE_VALUE) 
  {
    strcpy (errtext, "Cannot connect to server");
    return commErrNotOk_esp01;
  }
  // send cancel
  WriteFile (mailSlot, CANCELSTRING_COS905C, CANCELSIZE_COS905C,
             &bytesWritten, NULL);
  // close mailslot
  CloseHandle (mailSlot);
  return commErrOk_esp01;
}

/*----------------------------------------*/

global BOOL sql906c_processLives ( HANDLE process )
{
# undef MF__
# define MF__ MOD__"sql905c_processLives"
  BOOL result;
  BOOL ok;
  DWORD exitCode;
  
  ok = GetExitCodeProcess (process, &exitCode);
  if (exitCode == STILL_ACTIVE)
    result = TRUE;
  else
    result = FALSE;
  return result;
}

/*----------------------------------------*/

void sql906c_waitForEvent ( HANDLE event,
                            HANDLE process,
                            BOOL  *ok )
{
# undef MF__
# define MF__ MOD__"sql906c_waitForEvent"
  BOOL        eventPending    = TRUE;
  BOOL        processAlive    = TRUE;
  DWORD       waitRC;
  DWORD       nMilli          = 10;
  
  *ok = FALSE;
  while (eventPending && processAlive) 
  {
    // wait 15 seconds
    waitRC = WaitForSingleObject (event, nMilli);

    switch (waitRC) 
    {
    case WAIT_TIMEOUT:
      processAlive = sql906c_processLives (process);
      break;
    case WAIT_OBJECT_0:
      eventPending = FALSE;
      *ok = TRUE;
      break;
    default:
      eventPending = FALSE;
    }
    nMilli = (nMilli < 15000) ? nMilli * 2 : nMilli;
  }
}
/*----------------------------------------*/

static void sql906c_slotName ( char* target,
                               DWORD value )
{
# undef MF__
# define MF__ MOD__"sql906c_slotName"
  static const char * hexMap = "0123456789ABCDEF";
  char              * pos;
  
  strcpy (target, "\\\\.\\mailslot\\sap\\dbControl_________");
  pos = target + strlen (target) - 1;
  while (value > 0) {
    *pos = hexMap [value % 16];
    value /= 16;
    --pos;
  }
}

/*----------------------------------------*/

static void sql906c_SendReadyMessage ( HANDLE writeHandle )
{
# undef MF__
# define MF__ MOD__"sql906c_SendReadyMessage"
  DWORD       bytesWritten;
  
  WriteFile (writeHandle, READY_MESSAGE, READY_MESSAGE_LEN, 
             &bytesWritten, NULL);
  CloseHandle (writeHandle);
}

/*----------------------------------------*/

static BOOL sql906c_StartShmCancelThread ( teo906_Connection * pConnection )
{
# undef MF__
# define MF__ MOD__"sql906c_StartShmCancelThread"
  char        name[100];
  HANDLE      mailSlot;
  
  sql906c_slotName (name, GetCurrentProcessId ());

  mailSlot = CreateMailslot (name, 0, MAILSLOT_WAIT_FOREVER, NULL);
  
  if ( mailSlot == INVALID_HANDLE_VALUE)
  {
    printf ("Error creating mailslot, rc = %d'\n", GetLastError());
    return FALSE;
  }

  pConnection->CancelThread = CreateThread ( 0, 0, sql906c_ShmWatcherThread, 
                                             (void*) mailSlot, 0, NULL );

  if ( pConnection->CancelThread == NULL )
  {
    printf ("Error creating 'sql906c_ShmwatcherThread, rc = %d'\n", GetLastError());
    return FALSE;
  }

  return TRUE;
}

/*----------------------------------------*/

static void sql906c_StopShmCancelThread ( teo906_Connection * pConnection )
{
# undef MF__
# define MF__ MOD__"sql906c_StopShmCancelThread"

  if ( pConnection->CancelThread != NULL )
    TerminateThread(pConnection->CancelThread, 1);

  return;
}

/*----------------------------------------*/

static DWORD WINAPI sql906c_ShmWatcherThread ( void* slotParm )
{
# undef MF__
# define MF__ MOD__"sql906c_watcherThread"
  HANDLE      mailSlot = (HANDLE) slotParm;
  BOOL        slotOK;
  DWORD       nextSize;
  DWORD       msgCount;
  char        mailBuf [200];
  DWORD       bytesRead;
  int         result = 0;
  
  for (;;)
  {
    // get mail
    slotOK = ReadFile (mailSlot, mailBuf, 0, &bytesRead, NULL);

    slotOK = GetMailslotInfo (mailSlot, NULL, &nextSize, &msgCount, NULL);

    if (!slotOK) 
    {
      result = 1;
      break;
    }
    // if cancel: raise ()
    // fprintf (stdout, "cserver: %d messages waiting\n", msgCount);
    if (msgCount > 0) 
    {
      if (nextSize > sizeof (mailBuf)) 
        nextSize = sizeof (mailBuf);

      ReadFile (mailSlot, mailBuf, nextSize, &bytesRead, NULL);

      if (memcmp (mailBuf, CANCELSTRING_COS905C, CANCELSIZE_COS905C) == 0) 
        GenerateConsoleCtrlEvent (CTRL_C_EVENT, 0);
    }
  }

  CloseHandle (mailSlot);
  return result;
}

/*----------------------------------------*/

static BOOL sql906c_ParseShmOptions ( tos906c_ShmOptions *pOptions,
                                      int                argc,
                                      char*              argv [] )
{
# undef MF__
# define MF__ MOD__"sql906c_ParseShmOptions"
  
  int       i;
  BOOL      ParamsOk = TRUE;

  pOptions->shmHandle   = INVALID_HANDLE_VALUE;
  pOptions->writeHandle = INVALID_HANDLE_VALUE;
  pOptions->serverdb    = NULL;
  pOptions->dbroot      = NULL;
  pOptions->senderPID   = RTE_UNDEF_OSPID;

  for (i = 1; i < argc ; ++i) 
  {
    if (argv[i][0] != '-') 
      break;

    switch (argv[i][1]) 
    {
    case 'M':
# if defined (_WIN64)
      sscanf ( argv [i +1], "%p", &pOptions->shmHandle );
# else
      sscanf ( argv [i +1], "%x", &pOptions->shmHandle );
# endif
      ++i;
      break;
    case 'P':
# if defined (_WIN64)
      sscanf ( argv [i + 1], "%p,%p", &pOptions->readHandle,
               &pOptions->writeHandle );
# else
      sscanf ( argv [i + 1], "%x,%x", &pOptions->readHandle,
               &pOptions->writeHandle );
# endif
      ++i;
      break;
    case 'd':
      pOptions->serverdb = argv [i + 1];
      ++i;
      break;
    case 'R':
      pOptions->dbroot = argv [i + 1];
      ++i;
      break;
    case 'p':
      sscanf (argv [i + 1], "%d", &pOptions->senderPID);
      ++i;
      break;
    case 'v':
      // verbose_MV = TRUE;
      break;
    default:
      ++i;
      // usage_MF ();
      break;
    }
  }

  if ( ( pOptions->writeHandle == INVALID_HANDLE_VALUE ) ||
       ( pOptions->shmHandle   == INVALID_HANDLE_VALUE ))
  {
    printf ( "Invalid arguments\n");
    ParamsOk = FALSE;
  }

  return ParamsOk;
}


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
