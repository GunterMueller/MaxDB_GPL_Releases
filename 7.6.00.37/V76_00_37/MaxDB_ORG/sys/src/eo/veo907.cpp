/*!
  @file           veo907.cpp
  @author         RaymondR
  @special area   communication
  @brief          Contains the DBM communication.
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

#include "geo42.h"
#include "geo44.h"
#include "geo45.h"
#include "geo907.h"
#include "geo570.h"


#if defined(_WIN32)
# include <crtdbg.h>
#else /* UNIX */
# include <assert.h>

# if defined(_DEBUG)
#  define _ASSERT      assert
# else
#  define _ASSERT(_assertion)
# endif
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

// --- VARPART Values ---
//
//  6                             7
//  1 2 3 4 5 6 7 8 9 A B C D E F 0 1 2 3 4 5 6 7 8 9 A
//  a b c d e f g h i j k l m n o p q r s t u v w x y z
//
// - Lower case characters are reserved for application specific
//   varpart values!
#define VARPART_VAL_DBROOT_EO44                    0x72   /* = r */
#define VARPART_VAL_SERVERDB_EO44                  0x65   /* = d */
#define VARPART_VAL_SENDERNODEID_EO44              0x6e   /* = n */
#define VARPART_VAL_SENDERPID_EO44                 0x70   /* = p */


// --- request/response types ( 129 - 255 ) reserved for application
//     specific connections like DBMServer... see geo44.h
#define REQ_RESP_TYPE_SET_DBMSERVER_PARAMS_REQUEST           129
#define REQ_RESP_TYPE_SET_DBMSERVER_PARAMS_RESPONSE          130
#define REQ_RESP_TYPE_DBMSERVER_AUTOLOGON_REQUEST            131
#define REQ_RESP_TYPE_DBMSERVER_AUTOLOGON_RESPONSE           132

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/
#if !defined (_WIN32)
static int eo907_GetPipeOwner (teo110_PipeHdl PipeHandle);
#endif

/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*                      class SetDBMServerParamsReq                          */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

/*!
  @class          SetDBMServerParamsReq
   @Description    Build up and/or extract a set DBMServer parameter request
              packet.


                SetDBMServerParamsReq is derived from
                teo42_RTEReqRespBase

 */

class SetDBMServerParamsReq : public teo42_RTEReqRespBase
{
public:
  struct  teo44_Params : public teo42_RTEReqRespBase::teo42_FixedPartParams
  {
    tsp00_DbNamec                   ServerDB;                     // - in/out
    tsp00_Pathc                     DBRoot;                       // - in/out
    tsp00_NodeIdc                   senderNodeId;
    SAPDB_Int8                      senderPID;
  };

public:
                      SetDBMServerParamsReq          () { eo44_InitParams();};
                      ~SetDBMServerParamsReq         () {;}

  teo44_Params&       eo44_Params                    () { return m_Params; };
  void                eo44_InitParams                ();

  static tsp00_Uint1  eo42_ReqRespType               () { return REQ_RESP_TYPE_SET_DBMSERVER_PARAMS_REQUEST; };

protected:
  virtual teo42_FixedPartParams& eo42_FixedPartParams() { return m_Params; };

  virtual bool        eo42_FillVarPart               ( teo42_RTEReqRespVarPart             &VarPart,
                                                       teo200_EventList * const            pEventList = NULL );

  virtual bool        eo42_ExtractVarPart            ( teo42_RTEReqRespVarPart const       &VarPart,
                                                       teo200_EventList * const            pEventList = NULL );
private:
  teo44_Params      m_Params;
};


/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*                       class SetDBMServerParamsResp                        */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

/*!
  @class          SetDBMServerParamsResp
   @Description    Build up and/or extract a set DBMServer parameter response
              packet.


                SetDBMServerParamsResp is derived from
                teo42_EmptyReqResp

 */

class SetDBMServerParamsResp : public teo42_EmptyReqResp
{
public:
  static tsp00_Uint1  eo42_ReqRespType  () { return REQ_RESP_TYPE_SET_DBMSERVER_PARAMS_RESPONSE; };
};


#if !defined (_WIN32)

/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*                      class AutoLogonReq                                   */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

/*!
  @class          AutoLogonReq
   @Description    Set id of calling user

 */

class AutoLogonReq : public teo42_EmptyReqResp
{
public:
  static tsp00_Uint1  eo42_ReqRespType               () { return REQ_RESP_TYPE_DBMSERVER_AUTOLOGON_REQUEST; };

};


/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*                       class AutoLogonResp                                 */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

/*!
  @class          AutoLogonResp
   @Description    Acknowledge the auto logon


                AutoLogonResp is derived from
                teo42_EmptyReqResp

 */

class AutoLogonResp : public teo42_EmptyReqResp
{
public:
  static tsp00_Uint1  eo42_ReqRespType  () { return REQ_RESP_TYPE_DBMSERVER_AUTOLOGON_RESPONSE; };
};


#endif

/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/



/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*                     class teo907_DBMClientCommunication                   */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

bool teo907_DBMClientCommunication::eo907_EstablishConnection( _TCHAR const * const        Executable,
                                                               tsp00_DbNamec const         &ServerDB,
                                                               tsp00_Pathc const           &DBRoot,
                                                               tsp00_NodeIdc const &       senderNodeId,
                                                               RTE_OSPid                   senderPID,
                                                               void *                      &SendPacketBuffer,
                                                               tsp00_Uint4                 &SendPacketDataBufferSize,
                                                               tsp00_Uint4                 &TransferBufferSize,
                                                               teo200_EventList * const    pEventList)
{
  ROUTINE_DBG_MEO60 (_T("teo907_DBMClientCommunication::eo907_EstablishConnection"));
  bool           Ok;
  tsp00_Uint4    PeerTransferBufferSize;
  tsp00_Uint4    PeerMinResponsePacketSize;

  Ok = teo111_Connect( Executable, pEventList );

  if ( Ok )
  {
    // --- build up a temporary transfer buffer for only one packet
    m_TransferBufferSize = eo45_MinPacketBufferSize();
    m_TransferBuffer     = ALLOCATE_EO570( m_TransferBufferSize, pEventList );

    if ( m_TransferBuffer == NULL )
      return false;

    Ok = eo45_QueryPeerBufferSizes( m_TransferBuffer,
                                    m_TransferBufferSize,
                                    PeerTransferBufferSize,
                                    PeerMinResponsePacketSize,
                                    pEventList );
    if ( Ok )
    {
      _ASSERT(PeerMinResponsePacketSize == 0);
      // --- Currently the minimum response packet size must be zero.
      //     The transfer buffer is equal to the packet buffer

      if ( PeerTransferBufferSize != m_TransferBufferSize )
      {
        // --- the peer buffer size is different realocate transfer buffer!
        DEALLOCATE_EO570 ( m_TransferBuffer, pEventList );

        if ( EVENTLIST_IS_EMPTY_EO200( pEventList ) == false )
        {
          m_TransferBufferSize = 0;
          m_TransferBuffer     = NULL;
          Ok                   = false;
        }
        else
        {
          m_TransferBufferSize = PeerTransferBufferSize;
          m_TransferBuffer     = ALLOCATE_EO570( m_TransferBufferSize, pEventList );
        }
      }

      if ( m_TransferBuffer != NULL )
      {
        m_MinResponsePacketSize  = PeerMinResponsePacketSize;
        TransferBufferSize       = m_TransferBufferSize;
        SendPacketBuffer         = m_TransferBuffer;
        SendPacketDataBufferSize = eo45_CalcPacketDataBufferSize(m_TransferBufferSize, 0 == m_MinResponsePacketSize ? 1 : 2 )
                                   - m_MinResponsePacketSize;
      }
      if ( Ok )
      {
        Ok = eo907_SetDBMServerParams(m_TransferBuffer, 
                                      m_TransferBufferSize,
                                      ServerDB, 
                                      DBRoot, 
                                      senderNodeId,
                                      senderPID,
                                      pEventList);
      }
    }
  }

  return Ok;
}

/*---------------------------------------------------------------------------*/

bool teo907_DBMClientCommunication::eo907_SetBufferSize( tsp00_Uint4  const          TransferBufferSize,
                                                         teo200_EventList * const    pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo907_DBMClientCommunication::eo907_SetBufferSize"));

  bool Ok;

  if ( m_TransferBufferSize != TransferBufferSize )
  {
    m_TransferBufferSize = ALIGN( TransferBufferSize - ALIGNMENT_VALUE + 1,
                                  ALIGNMENT_VALUE );

    Ok = eo45_SetPeerBufferSizes( m_TransferBuffer, m_TransferBufferSize,
                                  m_TransferBufferSize, pEventList );
  }

  return Ok;
}

/*---------------------------------------------------------------------------*/

bool teo907_DBMClientCommunication::eo907_SetDBMServerParams ( void * const                PacketBuffer,
                                                               tsp00_Uint4 const           PacketBufferSize,
                                                               tsp00_DbNamec const         &ServerDB,
                                                               tsp00_Pathc const           &DBRoot,
                                                               tsp00_NodeIdc const &       senderNodeId,
                                                               RTE_OSPid                   senderPID,
                                                               teo200_EventList * const    pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo907_DBMClientCommunication::eo907_SetDBMServerParams"));

  tsp00_Uint4                 ReqDataLen;
  tsp00_Uint4                 RespDataLen;
  tsp00_Uint1                 RespType;
  bool                        Ok;
  teo41_Swap                  Swap;
  void *                      PacketDataBuffer;
  tsp00_Uint4                 PacketDataBufferSize;

  PacketDataBuffer     = eo45_CalcPacketDataBuffer( PacketBuffer );
  PacketDataBufferSize = eo45_CalcPacketDataBufferSize( PacketBufferSize );

  SetDBMServerParamsReq     SetDBMServerParams;

  // --- set DBRoot and ServerDB
  SetDBMServerParams.eo44_Params().ServerDB     = ServerDB;
  SetDBMServerParams.eo44_Params().DBRoot       = DBRoot;
  SetDBMServerParams.eo44_Params().senderNodeId = senderNodeId;
  //This cast was necessary because of a compatibility problem between a 7.6 XServer and a 
  //7.7 DBMServer or a 7.7 XServer and a 7.6 DBMServer
  SetDBMServerParams.eo44_Params().senderPID    = (SAPDB_Int8)senderPID;


  Ok = SetDBMServerParams.eo42_FillPacketData ( PacketDataBuffer, PacketDataBufferSize,
                                                ReqDataLen, pEventList );

  if ( Ok )
  { // --- send request and receive response!
    Ok = eo45_XferPacket ( PacketBuffer, PacketBufferSize,
                           ReqDataLen, SetDBMServerParams.eo42_ReqRespType(),
                           RespDataLen, RespType, Swap, pEventList );
  }

  if (( Ok ) && ( RespType != SetDBMServerParamsResp::eo42_ReqRespType() ))
  {
    Ok = eo45_HandleUnknownResponses ( PacketDataBuffer, RespDataLen, RespType,
                                       teo44_SetTransferBufferSizeResp::eo42_ReqRespType(),
                                       Swap, NULL,  pEventList ) ;
  }

  return Ok;
}

/*---------------------------------------------------------------------------*/

teo907_DBMClientCommunication::~teo907_DBMClientCommunication ()
{
  ROUTINE_DBG_MEO60 (_T("teo907_DBMClientCommunication::~teo907_DBMClientCommunication"));
  teo200_EventList     TmpEventList;

  if ( m_TransferBuffer != NULL )
  {
    // - implicit disconnect from peer!
    eo45_DisconnectFromPeer( m_TransferBuffer, m_TransferBufferSize, &TmpEventList );

    DEALLOCATE_EO570 ( m_TransferBuffer, &TmpEventList );
  }
}


/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*                     class teo907_DBMServerCommunication                    */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

teo907_DBMServerCommunication::
  teo907_DBMServerCommunication ( int                         argc,
                                  TCHAR**                     argv,
                                  tsp00_Uint4 const           TransferBufferSize,
                                  tsp00_Uint4 const           MinResponsePacketSize,
                                  bool                        &Ok,
                                  teo200_EventList * const    pEventList )
                             : m_TransferBufferSize(0),
                               m_TransferBuffer(NULL),
                               m_MinResponsePacketSize(0),
                               m_SendPacketOffset(0),
                               m_IsLoggedOn(false),
                               m_CouldLogOn(false),
                               m_CallingUser (-1),
                               m_IsLocalCommunication(false),
                               teo111_ChildComm (argc, argv, Ok)
{
  ROUTINE_DBG_MEO60 (_T("teo907_DBMServerCommunication::teo907_DBMServerCommunication"));

  if ( Ok )
  {
    m_DBRoot[0]         = '\0';
    m_ServerDB[0]       = '\0';
    m_SenderNodeId[0]   = '\0';
    m_SenderPID         = RTE_UNDEF_OSPID;

    eo45_SetProtocolID   ( DBManager_eeo42 );
    eo45_SetProtocolVersion ( 0 );
    eo45_SetTransferUnit( *this );

    // --- build up a transfer buffer for only one packet
    m_MinResponsePacketSize = ALIGN( MinResponsePacketSize, ALIGNMENT_VALUE );
    m_TransferBufferSize    = ALIGN( TransferBufferSize, ALIGNMENT_VALUE );
    m_TransferBuffer        = ALLOCATE_EO570( m_TransferBufferSize, pEventList );

    if ( m_TransferBuffer != NULL )
    {
      tsp00_Uint1       ReqType;
      teo41_Swap        Swap;
      tsp00_Uint4       DataLen;

      // --- no we are waiting for set DBServer parameter request!
      do
      {
        Ok = eo45_ReceivePacket( m_TransferBuffer, m_TransferBufferSize,
                                 DataLen, ReqType, Swap, pEventList );
        if ( Ok )
        {
          if ( ReqType == SetDBMServerParamsReq::eo42_ReqRespType() )
          {
            Ok = eo45_HandleSetParamsRequest( m_TransferBuffer, m_TransferBufferSize,
                                              DataLen, Swap, pEventList );
          }
#if !defined (_WIN32)
          else if ( ReqType == AutoLogonReq::eo42_ReqRespType() )
          {
            Ok = eo45_HandleAutoLogonRequest( m_TransferBuffer, m_TransferBufferSize,
                                              DataLen, Swap, pEventList );
            m_IsLocalCommunication = true;
          }
#endif
          else
          {
            // --- handle none set DBMServer params requests!
            Ok = eo45_HandleRequests( m_TransferBuffer, m_TransferBufferSize,
                                      DataLen, ReqType, Swap, pEventList );
          }
        }
      } while ( Ok && ReqType != SetDBMServerParamsReq::eo42_ReqRespType() );
    }
    else
      Ok = false;
  }
}

/*---------------------------------------------------------------------------*/

bool teo907_DBMServerCommunication::
       eo45_HandleSetParamsRequest( void * const              PacketBuffer,
                                    tsp00_Uint4 const         PacketBufferSize,
                                    tsp00_Uint4 const         ReqDataLen,
                                    teo41_Swap const          &Swap,
                                    teo200_EventList * const  pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo907_DBMServerCommunication::eo45_HandleSetParamsRequest"));

  bool Ok;

  tsp00_Uint4                 RespDataLen;
  void *                      PacketDataBuffer;
  tsp00_Uint4                 PacketDataBufferSize;
  SetDBMServerParamsReq       SetDBMServerParams;
  teo200_EventList            EventList;

  PacketDataBuffer     = eo45_CalcPacketDataBuffer( PacketBuffer );
  PacketDataBufferSize = eo45_CalcPacketDataBufferSize( PacketBufferSize );

  // --- swap and extract the request
  Ok = SetDBMServerParams.eo42_SwapAndExtractPacketData( PacketDataBuffer, ReqDataLen,
                                                         Swap, &EventList );
  if ( Ok )
  {
    m_DBRoot   = SetDBMServerParams.eo44_Params().DBRoot;
    m_ServerDB = SetDBMServerParams.eo44_Params().ServerDB;
    m_SenderNodeId = SetDBMServerParams.eo44_Params().senderNodeId;
    //This cast was necessary because of a compatibility problem between a 7.6 XServer and a 
    //7.7 DBMServer
    m_SenderPID = (RTE_OSPid)SetDBMServerParams.eo44_Params().senderPID;

    SetDBMServerParamsResp    SetDBMServerParams;

    Ok = SetDBMServerParams.eo42_FillPacketData( PacketDataBuffer, PacketDataBufferSize,
                                                 RespDataLen, pEventList );
    if ( Ok )
      Ok = eo45_SendPacket( PacketBuffer, RespDataLen,
                            SetDBMServerParamsResp::eo42_ReqRespType(),
                            pEventList );
  }
  else
  {
    eo45_SendRTEErrorResp( PacketBuffer, PacketBufferSize, EventList );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EventList );
  }

  return Ok;
};


/*---------------------------------------------------------------------------*/
#if !defined (_WIN32)
bool teo907_DBMServerCommunication::
       eo45_HandleAutoLogonRequest( void * const              PacketBuffer,
                                    tsp00_Uint4 const         PacketBufferSize,
                                    tsp00_Uint4 const         ReqDataLen,
                                    teo41_Swap const          &Swap,
                                    teo200_EventList * const  pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo907_DBMServerCommunication::eo45_HandleAutoLogonRequest"));

  bool Ok;

  tsp00_Uint4                 RespDataLen;
  void *                      PacketDataBuffer;
  tsp00_Uint4                 PacketDataBufferSize;
  AutoLogonReq                AutoLogon;
  teo200_EventList            EventList;
  tsp00_Int4                  UserID;

  PacketDataBuffer     = eo45_CalcPacketDataBuffer( PacketBuffer );
  PacketDataBufferSize = eo45_CalcPacketDataBufferSize( PacketBufferSize );

  // --- swap and extract the request
  Ok = AutoLogon.eo42_SwapAndExtractPacketData( PacketDataBuffer, ReqDataLen,
                                                         Swap, &EventList );
  if ( Ok )
  {
      /* with the security concept, the server is no longer installed as setuid root, but
         as setuid sdbown setgid sdbadm. The former check for EffectiveUser == RootUser
         is no longer needed. Instead only the  CallingUser is retrieved and the 'CouldLogOn'
         is always set for UNIX. The check if the caller has administrator rights is done 'above'
         i.e. in ctrlservopen()...
       */
      m_IsLoggedOn = false; /* enforce check of CallingUser... */
      m_CouldLogOn = true;  /* allows the check ... */
      m_CallingUser = (tsp00_Int4)getuid();

    /*
     * send (empty) reply
     */
    AutoLogonResp    reply;

    Ok = reply.eo42_FillPacketData( PacketDataBuffer, PacketDataBufferSize,
                                                 RespDataLen, pEventList );
    if ( Ok )
      Ok = eo45_SendPacket( PacketBuffer, RespDataLen,
                            AutoLogonResp::eo42_ReqRespType(),
                            pEventList );
  }
  else
  {
    eo45_SendRTEErrorResp( PacketBuffer, PacketBufferSize, EventList );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EventList );
  }

  return Ok;
};
#endif

/*---------------------------------------------------------------------------*/

bool teo907_DBMServerCommunication::
       eo45_OnQueryBufferSize ( tsp00_Uint4  const          MaxTransferBufferSize,
                                tsp00_Uint4                 &TransferBufferSize,
                                tsp00_Uint4                 &MinResponsePacketSize,
                                teo200_EventList * const    pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo907_DBMServerCommunication::eo45_OnQueryBufferSize"));

  TransferBufferSize    = m_TransferBufferSize;
  MinResponsePacketSize = m_MinResponsePacketSize;

  return true;
}

/*---------------------------------------------------------------------------*/

bool teo907_DBMServerCommunication::
       eo45_OnSetBufferSize ( tsp00_Uint4 const           TransferBufferSize,
                              teo200_EventList * const    pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo907_DBMServerCommunication::eo45_OnSetBufferSize"));

  if ( m_TransferBufferSize < TransferBufferSize )
  {
    teo200_EventList EvtLst( FUNCTION_NAME_MEO60, ERR_PROTOCOL, "eo45_OnSetBufferSize",
                             TransferBufferSize );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  m_TransferBufferSize =  TransferBufferSize;

  return true;
}

/*---------------------------------------------------------------------------*/

teo907_DBMServerCommunication::
  ~teo907_DBMServerCommunication ()
{
  ROUTINE_DBG_MEO60 (_T("teo907_DBMServerCommunication::~teo907_DBMServerCommunication"));
  teo200_EventList     TmpEventList;

  if ( m_TransferBuffer != NULL )
  {
    // - implicit disconnect from peer!
    eo45_DisconnectFromPeer( m_TransferBuffer, m_TransferBufferSize, &TmpEventList );

    DEALLOCATE_EO570 ( m_TransferBuffer, &TmpEventList );
  }
}

/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*                         class SetDBMServerParamsReq                       */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/


void  SetDBMServerParamsReq::eo44_InitParams ()
{
  ROUTINE_DBG_MEO60 (_T("SetDBMServerParamsReq::eo44_InitParams"));

  eo42_InitFixedPartParams();

  m_Params.ServerDB[0]      = '\0';
  m_Params.DBRoot[0]        = '\0';
  m_Params.senderNodeId[0]  = '\0';
  m_Params.senderPID        = RTE_UNDEF_OSPID;
}

/*---------------------------------------------------------------------------*/

bool SetDBMServerParamsReq::eo42_FillVarPart( teo42_RTEReqRespVarPart  &VarPart,
                                              teo200_EventList * const pEventList )
{
  ROUTINE_DBG_MEO60 (_T("SetDBMServerParamsReq::eo42_FillVarPart"));

  bool Ok = VarPart.eo42_PutValue( VARPART_VAL_DBROOT_EO44,
                                   m_Params.DBRoot.asCharp(),
                                   pEventList );
  if ( Ok )
    Ok = VarPart.eo42_PutValue( VARPART_VAL_SERVERDB_EO44,
                                m_Params.ServerDB.asCharp(),
                                pEventList );

  if ( Ok )
    Ok = VarPart.eo42_PutValue( VARPART_VAL_SENDERNODEID_EO44,
                                m_Params.senderNodeId.asCharp(),
                                pEventList );

  if ( Ok )
    Ok = VarPart.eo42_PutValue( VARPART_VAL_SENDERPID_EO44,
                                m_Params.senderPID,
                                pEventList );

  return Ok;
}

/*---------------------------------------------------------------------------*/

bool  SetDBMServerParamsReq::eo42_ExtractVarPart( teo42_RTEReqRespVarPart const  &VarPart,
                                                  teo200_EventList * const       pEventList )
{
  ROUTINE_DBG_MEO60 (_T("SetDBMServerParamsReq::eo42_ExtractVarPart"));

  bool Ok = VarPart.eo42_GetValue( VARPART_VAL_DBROOT_EO44,
                                   m_Params.DBRoot.size(),
                                   m_Params.DBRoot.asCharp(),
                                   true, pEventList );
  if ( Ok )
    Ok = VarPart.eo42_GetValue( VARPART_VAL_SERVERDB_EO44,
                                m_Params.ServerDB.size(),
                                m_Params.ServerDB.asCharp(),
                                true, pEventList );

  if ( Ok )
    Ok = VarPart.eo42_GetValue( VARPART_VAL_SENDERNODEID_EO44,
                                m_Params.senderNodeId.size(),
                                m_Params.senderNodeId.asCharp(),
                                true, pEventList );

  if ( Ok )
    Ok = VarPart.eo42_GetValue( VARPART_VAL_SENDERPID_EO44,
                                m_Params.senderPID,
                                true, pEventList );

  return Ok;
}

/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/