/*!
  @file           veo908.cpp
  @author         RaymondR
  @special area   XSERVER/DBMSERVER communication
  @brief          XSERVER/DBMSERVER communication
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
#include "geo007_1.h"
#include "geo907.h"
#include "geo908.h"
#include "geo573.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

// --- debug only
#define  INFO_USING_DBMSRV                          N(0),INFO_TYPE,_T("XSERVER "),_T("Using: '%s'")


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

static void eo908_DumpEvtLst ( teo200_EventList &EvtLst );





/*---------------------------------------------------------------------------*/

#if defined (_WIN32)
/* used in geo907.h */
#undef ALIGN 
#  include "gos00.h"  /* nocheck */
#else
#  include "gen904.h" /* nocheck */
#endif
/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/


extern "C" tsp01_CommErr_Enum eo908_Connect( char const * const        pServerPgm,
                                             char const * const        pDBRoot,
                                             char const * const        pServerDB,
                                             void **                   ppDBM,
                                             void **                   ppCommPacket,
                                             tsp00_Uint4 *             pMaxCmdDataLen,
                                             tsp00_Uint4 *             pTransferBufferSize,
                                             tsp00_Uint4 *             pServerRef,
                                             SAPDB_Char const * const  pSenderNodeId,
                                             RTE_OSPid                 senderPID)
{
  ROUTINE_DBG_MEO60 (_T("eo908_Connect"));
  tsp01_CommErr_Enum                commErr = commErrOk_esp01;
  teo907_DBMClientCommunication *   pDBMObj = NULL;
  void *                            SendPacketBuffer;
  tsp00_Uint4                       SendPacketDataBufferSize;
  tsp00_Uint4                       TransferBufferSize;
  tsp00_Pathc                       DBRoot;
  tsp00_Pathc                       ServerPgm;
  tsp00_NodeIdc                     senderNodeId;
  tsp00_Pathc                       Executable;
  tsp00_DbNamec                     ServerDB;

  try 
  {
    DBRoot.rawAssign(pDBRoot);
    ServerDB.rawAssign(pServerDB);
    ServerPgm.rawAssign(pServerPgm);
    senderNodeId.rawAssign(pSenderNodeId);

#   if defined (_WIN32)
    tsp00_ErrText errText;
    if ( !sql97cc_findControlServer ( Executable.asCharp(), 
                                       ServerDB.asCharp(), 
                                       ServerPgm.asCharp(), 
                                       DBRoot.asCharp(),
                                       errText))  
     {
       return commErrServerOrDBUnknown_esp01;
     }

     *pServerRef = GetCurrentThreadId();

#   else
     tsp00_ErrTextc errText;
     if ( !sql904_findControlServer (  Executable.asCharp(),
                                       Executable.size (),
                                       ServerDB.asCharp(),  
                                       ServerPgm.asCharp(), 
                                       DBRoot.asCharp(),
                                       DBRoot.size (),
                                       errText ) )
     {
       return commErrServerOrDBUnknown_esp01;
     }
     *pServerRef = getpid();
#   endif

#   if defined ( DEBUG_RTE )
      MSGCD(( INFO_USING_DBMSRV, Executable.asCharp() ));
#   endif

    // --- if the is no memory allocate throws an error!
    pDBMObj = new( teo907_DBMClientCommunication );

    if ( pDBMObj == NULL)
    {
      MSGCD(( ERR_OUT_OF_MEMORY ));
      return commErrNotOk_esp01;
    }

    pDBMObj->eo907_EstablishConnection (Executable, 
                                        ServerDB, 
                                        DBRoot,
                                        senderNodeId,
                                        senderPID,
                                        SendPacketBuffer, 
                                        SendPacketDataBufferSize,
                                        TransferBufferSize);
    *ppCommPacket         = SendPacketBuffer;
    *pMaxCmdDataLen       = SendPacketDataBufferSize;
    *pTransferBufferSize  = TransferBufferSize;
    *ppDBM                = pDBMObj;
  }
  catch ( teo200_EventList EvtLst )
  {
    delete pDBMObj;
    eo908_DumpEvtLst( EvtLst );
    commErr = commErrNotOk_esp01;
  }

  return commErr;
}
 
/*---------------------------------------------------------------------------*/

extern "C" tsp01_CommErr_Enum eo908_Request( void *                      pDBM,
                                             tsp00_Uint4  const          DataLen )
{
  ROUTINE_DBG_MEO60 (_T("eo908_Request"));
  tsp01_CommErr_Enum                commErr = commErrOk_esp01;
  teo907_DBMClientCommunication *   pDBMObj = NULL;

  try 
  {
    pDBMObj = (teo907_DBMClientCommunication*) pDBM;

    pDBMObj->eo907_Send( DataLen );
  }
  catch ( teo200_EventList EvtLst )
  {
    commErr = commErrNotOk_esp01;
    eo908_DumpEvtLst( EvtLst );
  }

  return commErr;
}

/*---------------------------------------------------------------------------*/

extern "C" tsp01_CommErr_Enum eo908_Receive( void *                      pDBM,
                                             void ** const               ppReceivePacketBuffer,
                                             tsp00_Uint4 * const         pDataLen )
{
  ROUTINE_DBG_MEO60 (_T("eo908_Receive"));
  tsp01_CommErr_Enum                commErr;
  teo907_DBMClientCommunication *   pDBMObj = NULL;

  void *                      pReceivePacketBuffer;
  tsp00_Uint4                 DataLen = 0;

  try 
  {
    pDBMObj = (teo907_DBMClientCommunication*) pDBM;

    pDBMObj->eo907_Receive( pReceivePacketBuffer, DataLen );

    *ppReceivePacketBuffer = pReceivePacketBuffer;
    *pDataLen              = DataLen;

    if ( DataLen == 0)
      commErr = commErrReleased_esp01;
    else
      commErr = commErrOk_esp01;
  }
  catch ( teo200_EventList EvtLst )
  {
    commErr = commErrNotOk_esp01;
    eo908_DumpEvtLst( EvtLst );
  }

  return commErr;
}

/*---------------------------------------------------------------------------*/

extern "C" tsp01_CommErr_Enum eo908_Cancel( void *                      pDBM )
{
  ROUTINE_DBG_MEO60 (_T("eo908_Cancel"));
  tsp01_CommErr_Enum                commErr = commErrOk_esp01;
  teo907_DBMClientCommunication *   pDBMObj = NULL;

  try 
  {
    pDBMObj = (teo907_DBMClientCommunication*) pDBM;

    pDBMObj->eo907_AsyncCancel();
  }
  catch ( teo200_EventList EvtLst )
  {
    commErr = commErrNotOk_esp01;
    eo908_DumpEvtLst( EvtLst );
  }

  return commErr;
}

/*---------------------------------------------------------------------------*/

extern "C" tsp01_CommErr_Enum eo908_Release( void *                      pDBM )
{
  ROUTINE_DBG_MEO60 (_T("eo908_Release"));
  tsp01_CommErr_Enum                commErr = commErrOk_esp01;

  try 
  {
    delete (teo907_DBMClientCommunication*) pDBM;
  }
  catch ( teo200_EventList EvtLst )
  {
    commErr = commErrNotOk_esp01;
    eo908_DumpEvtLst( EvtLst );
  }

  return commErr;
}

/*---------------------------------------------------------------------------*/

static void eo908_DumpEvtLst ( teo200_EventList &EvtLst )
{
  ROUTINE_DBG_MEO60 (_T("eo908_DumpEvtLst"));

  teo200_EventList *pTmpEvtLst = &EvtLst;

  if ( *pTmpEvtLst != false )
  {
    do
    {
      MSGCD_EVTLST( (*pTmpEvtLst) )
      pTmpEvtLst = pTmpEvtLst->eo200_NextEvent();
    }
    while ( pTmpEvtLst != NULL );
  }
}


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/