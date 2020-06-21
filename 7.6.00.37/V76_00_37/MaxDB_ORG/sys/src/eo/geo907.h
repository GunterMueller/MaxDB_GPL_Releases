/*!
  @file           geo907.h
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



#ifndef GEO907_H
#define GEO907_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "geo111.h"
#include "geo42.h"
#include "geo45.h"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#if !defined ALIGNMENT_VALUE
# define ALIGNMENT_VALUE                  16
# define ALIGN(_Value, _Align)   ((_Value + _Align - 1) & ~(_Align - 1))
#endif

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*                     class teo907_DBMClientCommunication                    */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

class teo907_DBMClientCommunication : protected teo45_RTEProtocolBase,
                                      protected teo111_ParentComm
{
public:
                        teo907_DBMClientCommunication ()
                                                      : m_Connected(false),
                                                        m_TransferBufferSize(0),
                                                        m_TransferBuffer(NULL),
                                                        m_MinResponsePacketSize(0),
                                                        m_RecievePacketOffset(0)
                                                      {
                                                        eo45_SetProtocolID     ( DBManager_eeo42 );
                                                        eo45_SetProtocolVersion( 0 );
                                                        eo45_SetTransferUnit( *this );
                                                      };

  virtual               ~teo907_DBMClientCommunication();

  bool                  eo907_EstablishConnection     ( _TCHAR const * const        Executable,
                                                        tsp00_DbNamec const         &ServerDB,
                                                        tsp00_Pathc const &         DBRoot,
                                                        tsp00_NodeIdc const &       senderNodeId,
                                                        RTE_OSPid                   senderPID,
                                                        void *                      &SendPacketBuffer,
                                                        tsp00_Uint4                 &SendPacketDataBufferSize,
                                                        tsp00_Uint4                 &TransferBufferSize,
                                                        teo200_EventList * const    pEventList = NULL );

  bool                  eo907_SetBufferSize           ( tsp00_Uint4  const          TransferBufferSize,
                                                        teo200_EventList * const    pEventList = NULL );

  void * const          eo907_CalcPacketDataBuffer    ( void * const                PacketBuffer )
                                                      { return eo45_CalcPacketDataBuffer(PacketBuffer); };

  tsp00_Uint4           eo907_CalcPacketDataBufferSize( tsp00_Uint4                 PacketBufferSize )
                                                      { return eo45_CalcPacketDataBufferSize(PacketBufferSize); };

  inline bool           eo907_Send                    ( tsp00_Uint4  const          DataLen,
                                                        teo200_EventList * const    pEventList = NULL );

  inline bool           eo907_Receive                 ( void *                      &ReceivePacketBuffer,
                                                        tsp00_Uint4                 &DataLen,
                                                        teo200_EventList * const    pEventList = NULL );

  bool                  eo907_AsyncCancel             ( teo200_EventList * const    pEventList = NULL )
                                                      { return eo45_AsyncCancel( pEventList ); };

private:
  bool                  eo907_SetDBMServerParams      ( void * const                PacketBuffer,
                                                        tsp00_Uint4 const           PacketBufferSize,
                                                        tsp00_DbNamec const         &ServerDB,
                                                        tsp00_Pathc const           &DBRoot,
                                                        tsp00_NodeIdc const &       senderNodeId,
                                                        RTE_OSPid                   senderPID,
                                                        teo200_EventList * const    pEventList );
private:
  bool                      m_Connected;

  void *                    m_TransferBuffer;
  tsp00_Uint4               m_TransferBufferSize;
  tsp00_Uint4               m_MinResponsePacketSize;
  tsp00_Uint4               m_RecievePacketOffset;
};

/*---------------------------------------------------------------------------*/

inline bool teo907_DBMClientCommunication::eo907_Send ( tsp00_Uint4  const          DataLen,
                                                        teo200_EventList * const    pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo907_DBMClientCommunication::eo907_Send"));

  bool            Ok;

  // --- check the data length
  if ( DataLen > eo45_CalcPacketDataBufferSize(m_TransferBufferSize - m_MinResponsePacketSize) )
  {
    teo200_EventList EvtLst( FUNCTION_NAME_MEO60, ERR_COMM_REQ_ILL_DATA_LEN, DataLen,
                             eo45_CalcPacketDataBufferSize(m_TransferBufferSize-m_MinResponsePacketSize));
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  Ok = eo45_SendRawDataPacket( m_TransferBuffer, DataLen, pEventList );

  // --- align the packet offset because each packet must be at least 16 byte aligned
  m_RecievePacketOffset = ALIGN( eo45_CalcPacketLength (DataLen), ALIGNMENT_VALUE );

  return Ok;
}

/*---------------------------------------------------------------------------*/

inline bool teo907_DBMClientCommunication::eo907_Receive ( void *                      &ReceivePacketBuffer,
                                                           tsp00_Uint4                 &DataLen,
                                                           teo200_EventList * const    pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo907_DBMClientCommunication::eo907_Receive"));

  ReceivePacketBuffer = (void*)((char*)m_TransferBuffer + m_RecievePacketOffset);

  return eo45_ReceiveRawDataPacket( ReceivePacketBuffer,
                                    m_TransferBufferSize - m_RecievePacketOffset,
                                    DataLen, pEventList );
}


/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/*                     class teo907_DBMServerCommunication                   */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

class teo907_DBMServerCommunication : protected teo45_RTEProtocolBase,
                                      protected teo111_ChildComm
{
public:
                        teo907_DBMServerCommunication ( int                         argc,
                                                        TCHAR**                     argv,
                                                        tsp00_Uint4 const           TransferBufferSize,
                                                        tsp00_Uint4 const           MinResponsePacketSize,
                                                        bool                        &Ok,
                                                        teo200_EventList * const    pEventList = NULL );
  virtual               ~teo907_DBMServerCommunication();

  inline bool           eo907_Send                    ( tsp00_Uint4  const          DataLen,
                                                        teo200_EventList * const    pEventList = NULL );

  inline bool           eo907_Receive                 ( void *                      &RecvDataBuffer,
                                                        tsp00_Uint4                 &DataLen,
                                                        void *                      &SendDataBuffer,
                                                        tsp00_Uint4                 &SendDataBufferSize,
                                                        teo200_EventList * const    pEventList = NULL );
  bool                  eo907_AsyncWaitForCancel      ( teo200_EventList * const    pEventList = NULL )
                                                      { return eo45_AsyncWaitForCancel( pEventList ); };

  void                  eo907_CloseData               ()
                                                      { eo110_CloseData(); };

  tsp00_DbNamec const & eo907_ServerDB                () const { return m_ServerDB; };
  char const * const    eo907_ServerDBStr             () const { return m_ServerDB.asCharp(); };
  tsp00_Pathc const &   eo907_DBRoot                  () const { return m_DBRoot; };
  char const * const    eo907_DBRootStr               () const { return m_DBRoot.asCharp(); };
  tsp00_NodeIdc const &   eo907_SenderNodeID          () const { return m_SenderNodeId; };
  SAPDB_Char const * const eo907_SenderNodeIDStr      () const { return m_SenderNodeId.asCharp(); };
  RTE_OSPid             eo907_SenderPID               () const { return m_SenderPID; };
  bool                  eo907_IsLoggedOn              () const { return m_IsLoggedOn; };
  bool                  eo907_CouldLogOn              () const { return m_CouldLogOn; };
  tsp00_Int4            eo907_GetCallingUser          () const { return m_CallingUser; }
  bool                  eo907_IsLocalCommunication    () const { return m_IsLocalCommunication; }

protected:
  virtual bool          eo45_QueryBufferSizeReqSupp   () const { return true; };
  virtual bool          eo45_OnQueryBufferSize        ( tsp00_Uint4  const          MaxTransferBufferSize,
                                                        tsp00_Uint4                 &TransferBufferSize,
                                                        tsp00_Uint4                 &MinResponsePacketSize,
                                                        teo200_EventList * const    pEventList = NULL );

  virtual bool          eo45_SetBufferSizeReqSupp     () const { return true; };
  virtual bool          eo45_OnSetBufferSize          ( tsp00_Uint4 const           TransferBufferSize,
                                                        teo200_EventList * const    pEventList = NULL );

private:
  virtual bool          eo45_HandleSetParamsRequest    ( void * const               PacketBuffer,
                                                         tsp00_Uint4 const          PacketBufferSize,
                                                         tsp00_Uint4 const          ReqDataLen,
                                                         teo41_Swap const           &Swap,
                                                         teo200_EventList * const   pEventList = NULL );
#if !defined (_WIN32)
  virtual bool          eo45_HandleAutoLogonRequest    ( void * const               PacketBuffer,
                                                         tsp00_Uint4 const          PacketBufferSize,
                                                         tsp00_Uint4 const          ReqDataLen,
                                                         teo41_Swap const           &Swap,
                                                         teo200_EventList * const   pEventList = NULL );
#endif
private:
  void *                    m_TransferBuffer;
  tsp00_Uint4               m_TransferBufferSize;
  tsp00_Uint4               m_MinResponsePacketSize;
  tsp00_Uint4               m_SendPacketOffset;

  tsp00_Pathc               m_DBRoot;
  tsp00_DbNamec             m_ServerDB;
  tsp00_NodeIdc             m_SenderNodeId;
  RTE_OSPid                 m_SenderPID;
  bool                      m_IsLoggedOn;
  bool                      m_CouldLogOn;
  tsp00_Int4                m_CallingUser;
  bool                      m_IsLocalCommunication;
};

/*---------------------------------------------------------------------------*/

inline bool teo907_DBMServerCommunication::eo907_Send ( tsp00_Uint4  const          DataLen,
                                                        teo200_EventList * const    pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo907_DBMClientCommunication::eo907_Send"));

  // --- check the data length
  if ( DataLen > eo45_CalcPacketDataBufferSize(m_TransferBufferSize - m_SendPacketOffset))
  {
    teo200_EventList EvtLst( FUNCTION_NAME_MEO60, ERR_COMM_REQ_ILL_DATA_LEN, DataLen,
                             eo45_CalcPacketDataBufferSize(m_TransferBufferSize-m_SendPacketOffset) );
    COPY_OR_THROW_EVENTLIST_EO200( pEventList, EvtLst );
    return false;
  }

  return  eo45_SendRawDataPacket( (void*)((char*)m_TransferBuffer + m_SendPacketOffset),
                                  DataLen, pEventList );
}

/*---------------------------------------------------------------------------*/

bool teo907_DBMServerCommunication::eo907_Receive( void *                      &ReceiveDataBuffer,
                                                   tsp00_Uint4                 &DataLen,
                                                   void *                      &SendDataBuffer,
                                                   tsp00_Uint4                 &SendDataBufferSize,
                                                   teo200_EventList * const    pEventList )
{
  ROUTINE_DBG_MEO60 (_T("teo907_DBMServerCommunication::eo907_ReceiveData"));

  bool           Ok;

  Ok = eo45_ReceiveRawDataPacket( m_TransferBuffer,
                                  m_TransferBufferSize - m_MinResponsePacketSize,
                                  DataLen,pEventList );

  // --- align the packet offset because each packet must be at least 16 byte aligned
  m_SendPacketOffset  = ALIGN( eo45_CalcPacketLength (DataLen), ALIGNMENT_VALUE );

  ReceiveDataBuffer   = eo45_CalcPacketDataBuffer( m_TransferBuffer );
  SendDataBuffer      = eo45_CalcPacketDataBuffer( (void*)((char*)m_TransferBuffer + m_SendPacketOffset));
  SendDataBufferSize  = eo45_CalcPacketDataBufferSize( m_TransferBufferSize - m_SendPacketOffset);


  /* 
   *  -- reduce send buffer size --
   *    It's a workaround, because the old client interface is ignoring the maximum reply packet size value.
   */
  tsp00_Uint4 MaxReceiveDataBufferSize = eo45_CalcPacketDataBufferSize(m_TransferBufferSize, 0 == m_MinResponsePacketSize ? 1 : 2)
                                         - m_MinResponsePacketSize;;
  if ( SendDataBufferSize > MaxReceiveDataBufferSize )
      SendDataBufferSize = MaxReceiveDataBufferSize;


  return Ok;
}



/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/


/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/



#endif  /* GEO907_H */
