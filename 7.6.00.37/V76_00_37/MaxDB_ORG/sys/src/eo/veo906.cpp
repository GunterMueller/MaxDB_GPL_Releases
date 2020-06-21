/*!
  @file           veo906.cpp
  @author         RaymondR
  @special area   DBMServer communication
  @brief          DBMServer communication
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




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp01.h"
#include "geo00_2.h"
#include "geo60.h"
#include "geo200.h"
#include "geo906.h"
#include "geo907.h"
#include "geo007_1.h"

#if defined (_WIN32)

# if defined ALIGN
#  undef ALIGN
# endif

# include <direct.h>
# include "gos00.h"   /* nocheck */
# include "gos906.h"  /* nocheck */

#else
# include  <signal.h>
# include  <stdlib.h>
#endif

#if defined (DUMP_PACKET_DATA)
# include <stdio.h>
# include <time.h>
# include <ctype.h>
#endif


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#if defined (DUMP_PACKET_DATA)
# define DUMP_FILE_NAME "serverpkts.txt"
#endif
/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

static void               eo906_PrintEvtLst   ( teo200_EventList      &EvtLst );

static teo906_Connection* eo906_Connect       ( int                   argc,
                                                char*                 argv [],
                                                char const ** const   ppServerDB,
                                                char const ** const   ppDBRoot,
                                                SAPDB_Char const ** const   ppSenderNodeId,
                                                RTE_OSPid &           senderPID,
                                                bool &                fIsLoggedOn,
                                                bool &                fCouldLogOn,
                                                tsp00_Int4 &          iCallerid,
                                                bool &                fLocalCommuncation);
static tsp01_CommErr_Enum eo906_Receive       ( teo906_Connection *   pConnection,
                                                char**                ppRecvDataBuffer,
                                                int*                  pRecvLen,
                                                char**                ppSendDataBuffer,
                                                int* const            pMaxSendLen );

static tsp01_CommErr_Enum eo906_Send          ( teo906_Connection *   pConnection,
                                                int const             SendLen );

static void eo906_Release                     ( teo906_Connection *   pConnection );

static bool eo906_StartCancelThreadOrProcess  ( teo906_Connection *   pConnection );
static bool eo906_StopCancelThreadOrProcess   ( teo906_Connection *   pConnection );


#if defined (_WIN32)
 static DWORD WINAPI eo906_WatcherThread      ( teo906_Connection *   pConnection );
#else
 static void         eo906_WatcherProcess     ( teo906_Connection *   pConnection );
#endif


#if defined (DUMP_PACKET_DATA)
 static void eo906_DumpPacketData             ( bool const            Recv,
                                                char const * const    PacketDataBuffer,
                                                int const             PacketDataLen );
#endif
/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

extern "C" void sqlxservinit ( void**               ppConnection,
                               int                  argc,
                               char*                argv [],
                               char*                ServerDB,
                               char*                DBRoot,
                               SAPDB_Char *         senderNodeId,
                               RTE_OSPid &          senderPID,
                               BOOLEAN*             LoggedOnPtr,
                               BOOLEAN*             CouldLogOnPtr,
                               int*                 CallerIdPtr,
                               BOOLEAN*             isLocalCommuniationPtr,
                               tsp01_CommErr*       commErr )
{
  ROUTINE_DBG_MEO60 (_T("sqlxservinit"));
  char const *          pServerDB;
  char const *          pDBRoot;
  SAPDB_Char  * const   pLocalNodeId = senderNodeId;
  SAPDB_Char const *    pSenderNodeId;
  bool                  LoggedOn;
  bool                  CouldLogOn;
  tsp00_Int4            CallerId;
  bool                  IsLocalCommunication;

# if defined (_WIN32)
   /* disable message output */
   sql60_disable_GUI_console();
   sql60_disable_message_box ();
   sql60_disable_console ();
#endif

   DBRoot[0]        = '\0';
   ServerDB[0]      = '\0';
   senderNodeId[0]  = '\0'; 
   senderPID      = RTE_UNDEF_OSPID;
   commErr->becomes(commErrOk_esp01);

# if defined (_WIN32)

   *CouldLogOnPtr = FALSE;
   *CallerIdPtr = -1;
   *isLocalCommuniationPtr = FALSE;
   // use old shared memory communication
   //
   if ( sql906c_ItsAShmConnectRequest ( argc, argv ) == TRUE )
   {
     *ppConnection = (void*)sql906c_shmConnect ( argc, argv, &pServerDB, &pDBRoot, pLocalNodeId, senderPID);

     if ( *ppConnection != NULL )
     {
       *LoggedOnPtr = true;

       if ( pServerDB != NULL )
         strcpy ( ServerDB, pServerDB );

       if ( pDBRoot != NULL )
         strcpy ( DBRoot, pDBRoot );

       if ( DBRoot[0] != '\0' )
         chdir (DBRoot);

       if ( pLocalNodeId != NULL )
         strcpy (senderNodeId, pLocalNodeId);

       *isLocalCommuniationPtr = TRUE;
     }
     else
       commErr->becomes( commErrNotOk_esp01 );

     return;
   }
# endif

  *ppConnection = (void*)eo906_Connect ( argc,
                                         argv,
                                        &pServerDB,
                                        &pDBRoot,
                                        &pSenderNodeId,
                                         senderPID,
                                         LoggedOn,
                                         CouldLogOn,
                                         CallerId,
                                         IsLocalCommunication);

  if ( *ppConnection != NULL )
  {
    *LoggedOnPtr = LoggedOn;
    *CouldLogOnPtr = CouldLogOn;
    *CallerIdPtr = CallerId;
    *isLocalCommuniationPtr = IsLocalCommunication;

    if ( pServerDB != NULL )
      strcpy ( ServerDB, pServerDB );

    if ( pDBRoot != NULL )
      strcpy ( DBRoot, pDBRoot );

    if ( DBRoot[0] != '\0' )
      chdir (DBRoot);

    if ( pSenderNodeId != NULL )
    {
      strcpy (senderNodeId, pSenderNodeId);
    }
  }
  else
    commErr->becomes( commErrNotOk_esp01 );

  return;
}


/*---------------------------------------------------------------------------*/

extern "C" void sqlxservrecv( void*                 pConnection,
                              char**                RecvDataBuffer,
                              int*                  RecvLen,
                              char**                SendDataBuffer,
                              int*                  MaxSendLen,
                              tsp01_CommErr*        commErr )
{
  ROUTINE_DBG_MEO60 (_T("sqlxservrecv"));

# if defined (_WIN32)

  if ( ((teo906_Connection*)pConnection)->ItsAShmConnection == TRUE )
  {
    commErr->becomes( sql906c_shmReceive ( (teo906_Connection*)pConnection,
                                           RecvDataBuffer, RecvLen,
                                           SendDataBuffer, MaxSendLen ));
    return;
  }
# endif

  commErr->becomes( eo906_Receive( (teo906_Connection*)pConnection,
                                   RecvDataBuffer, RecvLen,
                                   SendDataBuffer, MaxSendLen ));
  return;
}

/*---------------------------------------------------------------------------*/

extern "C" void sqlxservsend( void*                pConnection,
                              int                  SendLen,
                              tsp01_CommErr*       commErr)
{
  ROUTINE_DBG_MEO60 (_T("sqlxservsend"));

# if defined (_WIN32)

  if ( ((teo906_Connection*)pConnection)->ItsAShmConnection == TRUE )
  {
    commErr->becomes( sql906c_shmSend ( (teo906_Connection*)pConnection, SendLen ));
    return;
  }
#endif

  commErr->becomes( eo906_Send ( (teo906_Connection*)pConnection, SendLen ));

  return;
}

/*---------------------------------------------------------------------------*/

extern "C" void sqlxservexit ( void*               pConnection )
{
  ROUTINE_DBG_MEO60 (_T("sqlxservexit"));

# if defined (_WIN32)

  if ( ((teo906_Connection*)pConnection)->ItsAShmConnection == TRUE )
  {
    sql906c_shmRelease ( (teo906_Connection*)pConnection );
    return;
  }
#endif
  eo906_Release( (teo906_Connection*)pConnection );
}

/*---------------------------------------------------------------------------*/

static teo906_Connection* eo906_Connect ( int                   argc,
                                          char*                 argv [],
                                          char const ** const   ppServerDB,
                                          char const ** const   ppDBRoot,
                                          SAPDB_Char const ** const   ppSenderNodeId,
                                          RTE_OSPid &           senderPID,
                                          bool &                fIsLoggedOn,
                                          bool &                fCouldLogOn,
                                          tsp00_Int4 &          iCallerid,
                                          bool &                fLocalCommuncation)
{
  ROUTINE_DBG_MEO60 (_T("eo906_Connect"));

  teo906_Connection               *pConnection = NULL;
  teo907_DBMServerCommunication   *pCommObj    = NULL;
  bool                            Ok;

  try
  {
    pCommObj = new teo907_DBMServerCommunication( argc,argv,
                                                  DBM_TRANSFER_BUFFER_SIZE,
                                                  DBM_MIN_RESPONSE_PACKET_SIZE,
                                                  Ok );
    if ( pCommObj == NULL )
    {
      printf ("Out of memory error\n");
      return NULL;
    }
    if ( Ok == false )
      return NULL;

    pConnection = new teo906_Connection;

    if (pConnection == NULL)
    {
      delete pConnection;
      printf ("Out of memory error\n");
      return NULL;
    }

    pConnection->pCommObj = (void*)pCommObj;

    *ppDBRoot       = pCommObj->eo907_DBRootStr();
    *ppServerDB     = pCommObj->eo907_ServerDBStr();
    *ppSenderNodeId = pCommObj->eo907_SenderNodeIDStr();
    senderPID       = pCommObj->eo907_SenderPID();

    fIsLoggedOn = pCommObj->eo907_IsLoggedOn ();
    fCouldLogOn = pCommObj->eo907_CouldLogOn ();
    iCallerid   = pCommObj->eo907_GetCallingUser ();
    fLocalCommuncation = pCommObj->eo907_IsLocalCommunication ();
#   if !defined (NO_CANCEL)
     eo906_StartCancelThreadOrProcess( (teo906_Connection*) pConnection );
#   else
#    if defined (_WIN32)
      pConnection->ItsAShmConnection = false;
      pConnection->CancelThread      = NULL;
#    else
      pConnection->CancelProcess     = -1;
#    endif
#   endif
  }
  catch ( teo200_EventList EvtLst )
  {
    delete pConnection;
    delete pCommObj;
    pConnection = NULL;
    eo906_PrintEvtLst( EvtLst );
  }

  return pConnection;
}

/*---------------------------------------------------------------------------*/

static tsp01_CommErr_Enum eo906_Receive ( teo906_Connection *   pConnection,
                                          char**                ppRecvDataBuffer,
                                          int*                  pRecvLen,
                                          char**                ppSendDataBuffer,
                                          int* const            pMaxSendLen )
{
  ROUTINE_DBG_MEO60 (_T("eo906_Receive"));

  tsp01_CommErr_Enum              commErr;
  teo907_DBMServerCommunication   *pCommObj;
  void *                          pRecvDataBuffer;
  tsp00_Uint4                     DataLen;
  void *                          pSendDataBuffer;
  tsp00_Uint4                     SendDataBufferSize;

  try
  {
    pCommObj = (teo907_DBMServerCommunication*)pConnection->pCommObj;

    pCommObj->eo907_Receive ( pRecvDataBuffer, DataLen,
                              pSendDataBuffer, SendDataBufferSize );

    *ppRecvDataBuffer = (char*)pRecvDataBuffer;
    *pRecvLen         = (int)DataLen;
    *ppSendDataBuffer = (char*)pSendDataBuffer;
    *pMaxSendLen      = (int)SendDataBufferSize;

#   if defined (DUMP_PACKET_DATA)
     pConnection->pSendDataBuffer = *ppSendDataBuffer;
     eo906_DumpPacketData( true, *ppRecvDataBuffer, *pRecvLen );
#   endif

    if ( DataLen == 0 )
      commErr = commErrReleased_esp01;
    else
      commErr = commErrOk_esp01;
  }
  catch ( teo200_EventList EvtLst )
  {
    eo906_PrintEvtLst( EvtLst );
    commErr = commErrNotOk_esp01;
  }

  return commErr;
}

/*---------------------------------------------------------------------------*/

static tsp01_CommErr_Enum eo906_Send ( teo906_Connection *   pConnection,
                                       int const             SendLen )
{
  ROUTINE_DBG_MEO60 (_T("eo906_Send"));

  tsp01_CommErr_Enum              commErr;
  teo907_DBMServerCommunication   *pCommObj;

  try
  {
    pCommObj = (teo907_DBMServerCommunication*)pConnection->pCommObj;

#   if defined (DUMP_PACKET_DATA)
     eo906_DumpPacketData( false, pConnection->pSendDataBuffer, SendLen );
#   endif

    pCommObj->eo907_Send ( SendLen );

    commErr = commErrOk_esp01;
  }
  catch ( teo200_EventList EvtLst )
  {
    eo906_PrintEvtLst( EvtLst );
    commErr = commErrNotOk_esp01;
  }

  return commErr;
}

/*---------------------------------------------------------------------------*/

static void eo906_Release( teo906_Connection *   pConnection )
{
  ROUTINE_DBG_MEO60 (_T("eo906_Release"));

  try
  {
    if ( pConnection != NULL )
    {
      eo906_StopCancelThreadOrProcess( pConnection );

      delete (teo907_DBMServerCommunication*) pConnection->pCommObj;
      delete pConnection;
    }
  }
  catch ( teo200_EventList EvtLst )
  {
    eo906_PrintEvtLst( EvtLst );
  }

  return;
}

/*----------------------------------------*/

static bool eo906_StartCancelThreadOrProcess ( teo906_Connection * pConnection )
{
  ROUTINE_DBG_MEO60 (_T("eo906_StartCancelThreadOrProcess"));

#if defined (_WIN32)
  pConnection->CancelThread = CreateThread ( 0, 0, (LPTHREAD_START_ROUTINE)eo906_WatcherThread,
                                             (void*) pConnection, 0, NULL );
  if ( pConnection->CancelThread == NULL )
  {
    throw teo200_EventList ( FUNCTION_NAME_MEO60, ERR_CREATING_THREAD,
                             "eo906_WatcherThread", GetLastError() );
  }
#else
  pConnection->CancelProcess = fork();

  if ( pConnection->CancelProcess == 0 )
  {
    eo906_WatcherProcess( pConnection );
  }
  else if ( pConnection->CancelProcess == -1 )
  {
    throw teo200_EventList ( FUNCTION_NAME_MEO60, ERR_FORK_ERR,
                             "eo906_WatcherProcess" );
  }
#endif

  return true;
}

/*----------------------------------------*/

static bool eo906_StopCancelThreadOrProcess (  teo906_Connection * pConnection )
{
  ROUTINE_DBG_MEO60 (_T("eo906_StopCancelThreadOrProcess"));

#if defined (_WIN32)
  if ( pConnection->CancelThread != NULL )
    TerminateThread(pConnection->CancelThread, 1);
#else
  if ( pConnection->CancelProcess > 0 )
    kill( pConnection->CancelProcess, SIGKILL );
#endif

  return true;
}

/*----------------------------------------*/

#if defined (_WIN32)
 static DWORD WINAPI eo906_WatcherThread ( teo906_Connection * pConnection )
 {
   ROUTINE_DBG_MEO60 (_T("eo906_WatcherThread"));

   teo907_DBMServerCommunication   *pCommObj;

   pCommObj = (teo907_DBMServerCommunication*)pConnection->pCommObj;

   try
   {
     pCommObj->eo907_AsyncWaitForCancel();
     GenerateConsoleCtrlEvent (CTRL_C_EVENT, 0);
   }
   catch ( teo200_EventList EvtLst )
   { ; }

   return 0;
 }

/*----------------------------------------*/
#else
/*----------------------------------------*/

 static void eo906_WatcherProcess( teo906_Connection * pConnection )
 {
   ROUTINE_DBG_MEO60 (_T("eo906_WatcherProcess"));

   teo907_DBMServerCommunication   *pCommObj;

   pCommObj = (teo907_DBMServerCommunication*)pConnection->pCommObj;

   try
   {
     pCommObj->eo907_CloseData();       // --- close data pipes because of
                                        //     no data communication is needed.
     pCommObj->eo907_AsyncWaitForCancel();
   }
   catch ( teo200_EventList EvtLst )
   { ; }

   kill( getppid(), SIGKILL );

   exit (0);
 }
#endif

/*---------------------------------------------------------------------------*/

static void eo906_PrintEvtLst ( teo200_EventList &EvtLst )
{
  ROUTINE_DBG_MEO60 (_T("eo906_PrintEvtLst"));

  teo200_EventList *pTmpEvtLst = &EvtLst;

  if ( *pTmpEvtLst == false )
    printf ( _T("Throw error, event list is empty\n"));
  else
  {
    do
    {
      printf ( "%5d %s %s: %s - (%s)\n",
               pTmpEvtLst->eo200_EventID(),
               pTmpEvtLst->eo200_EventDateTime(true),
               pTmpEvtLst->eo200_EventType(true),
               pTmpEvtLst->eo200_EventMsg(),
               pTmpEvtLst->eo200_FunctionName() );

      pTmpEvtLst = pTmpEvtLst->eo200_NextEvent();
    }
    while ( pTmpEvtLst != NULL );
  }
}

/*---------------------------------------------------------------------------*/


#if defined (DUMP_PACKET_DATA)
 static void eo906_DumpPacketData( bool const                 Recv,
                                   char const * const         PacketDataBuffer,
                                   int const                  PacketDataLen )
 {
   FILE                        *fp;
   int                         i, j, l;
   int                         LLen      = 16;
   char                        Line[512];
   char                        cTmp;
   static char const * const   HexASCIIMap = "0123456789ABCDEF";
   struct tm                   *TmBuf;
   time_t                      Clock;
   static  tsp00_Pathc         DumpFile;
   tsp01_RteError              RteError;

   if ( DumpFile[0] == '\0' )
   {
     sqlGetIndependentProtPath( DumpFile, TERM_WITH_DELIMITER_EO01, &RteError );
     strcat ( DumpFile, DUMP_FILE_NAME );
   }

   fp = fopen ( DumpFile, "a");

   if ( fp != NULL )
   {
     Clock = time ( (time_t*) 0 );
 #   if defined (_WIN32)
      TmBuf = localtime(&Clock);
 #   else
      struct tm   Tm_Buf;
      TmBuf = localtime_r(&Clock, &Tm_Buf );
 #   endif

     if ( Recv )
       fprintf ( fp, "\nRECV  (%02d-%02d %02d:%02d:%02d):\n 0x0000  ",
                 TmBuf->tm_mon+1, TmBuf->tm_mday,
                 TmBuf->tm_hour , TmBuf->tm_min ,
                 TmBuf->tm_sec );
     else
       fprintf ( fp, "\nSEND (%02d-%02d %02d:%02d:%02d):\n 0x0000  ",
                 TmBuf->tm_mon+1, TmBuf->tm_mday,
                 TmBuf->tm_hour , TmBuf->tm_min ,
                 TmBuf->tm_sec );

     for ( i = 0, l = 0; i < PacketDataLen; i++ )
     {
       Line[l++] = HexASCIIMap[(unsigned char)(PacketDataBuffer[i] >> 4) & 0x0F];
       Line[l++] = HexASCIIMap[(unsigned char) PacketDataBuffer[i]       & 0x0F];
       Line[l++] = ' ';

       if (( i != 0 ) && !((i + 1) % LLen) )
       {
         Line[l++] = ' ';
         Line[l++] = ' ';

         for ( j = 1; j <= LLen; j++ )
         {
           cTmp = PacketDataBuffer[i - LLen + j];
           (isprint(cTmp) != 0) ? Line[l++] = cTmp : Line[l++] = '.';
         }
         Line[l] = '\0';
         l       = 0;
         if ( i < PacketDataLen )
           fprintf ( fp, "%s\n 0x%04X  ", Line, i + 1 );
         else
           fprintf ( fp, "%s\n", Line );
       }
     }

     if (i % LLen)
     {
       for ( j = i % LLen; j < LLen; j++ )
       {
         Line[l++] = ' ';
         Line[l++] = ' ';
         Line[l++] = ' ';
       }
       Line[l++] = ' ';
       Line[l++] = ' ';

       for ( j = 0; j < (i % LLen); j++ )
       {
         cTmp = PacketDataBuffer[i - (i % LLen) + j];
         (isprint(cTmp) != 0) ? Line[l++] = cTmp : Line[l++] = '.';
       }
       Line[l++] = '\0';
       fprintf ( fp, "%s\n", Line);
     }
     fclose ( fp );
   }
   return;
 }
#endif

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/