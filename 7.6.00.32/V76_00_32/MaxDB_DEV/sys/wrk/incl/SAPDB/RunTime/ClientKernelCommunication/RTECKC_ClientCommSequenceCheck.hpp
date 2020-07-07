/*!========================================================================

  @ingroup      CKC
  @file         RTECKC_ClientCommSequenceCheck.hpp
  @author       RaymondR

  @brief        Client send and receive sequence check  

                The client kernel communication allows more than
                one packet to be send to the database kernel. This
                means it is allowed to the client to send all available
                packets before a receive called to get the result. 

  @since        2003-10-27  15:48
  @sa           

  ==========================================================================

  \if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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
 ============================================================================*/


#ifndef RTECKC_CLIENTCOMMSEQUENCECHECK_HPP
#define RTECKC_CLIENTCOMMSEQUENCECHECK_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include    "SAPDBCommon/SAPDB_Types.hpp"
#include    "RunTime/Synchronisation/RTESync_Spinlock.hpp"
#include    "RunTime/RTE_MessageList.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


/*!---------------------------------------------------------------------------
  @class   RTECKC_ClientCommSequenceCheck
  @brief   Client send and receive sequence check.

           The client kernel communication allows more than
           one packet to be send to the database kernel. This
           means it is allowed to the client to send all available
           packets before a receive called to get the result. 

           This class is used to aware the sequence of the send
           and receive calls driven by the client application
           software. If the send/receive calls are out of sequence
           a bad retrun code and a error message will be the result.

           For example it is not allowed to:
                - call receive without any outstanding send call
                - call the send twice for the same packet
                - call the send with an invalid packet pointer

           But it's allowed to:
                - call send with different packet pointers before calling receive

           This class has some contrains to the packet buffer to be fulfilled.
           
           If there is more than one packet buffer these buffers must stick 
           together in a single buffer. This single buffer is separeted in multiple
           packet buffers each packet with the same distance to previous and 
           next one.

 ----------------------------------------------------------------------------*/

class RTECKC_ClientCommSequenceCheck
{
public:
    /*!  @brief   Possible methode return values */
    enum SyncRet  { NoError = 0,
                    Error,               // Error during initialization
                    WrongPacket,         // wrong packet pointer
                    PacketInUse,         // packet already in use
                    NoOutstandingPackets,// no packet outstanding EnterSend() must be called first
                    SendPending,         // EnterSend() already pending
                    ReceivePending,      // EnterReceive() already pending
                    ClosePending,        // EnterClose () already pending
                    NoReceivePending     // EnterReceive() must be call before LeaveReceive()
                  };

public:
    /*!
    @brief         Constructor

    Creates a client sequence check class
    */
    RTECKC_ClientCommSequenceCheck()  : m_Lock(0),
                                       m_pFirstPacket(0),
                                       m_PacketDistance(0),
                                       m_SendPending(false),
                                       m_ReceivePending(false),
                                       m_ClosePending(false),
                                       m_ReceivesOutstanding(0),
                                       m_NumPackets(0),
                                       m_pPacketUsed (NULL)
                                      {;}

    /*!
    @brief         Destructor

    Destroy the client sequence check class
    */
    ~RTECKC_ClientCommSequenceCheck();

    /*!
    @brief         Sets the sequence check parameter

    Sets the sequence check parameter which are used to determine if the 
    constrains are fulfilled.

    If there is more than one packet buffer these buffers must stick 
    together in a single buffer. This single buffer is separeted in multiple
    packet buffers each packet with the same distance to previous and 
    next one. The send packet pointer and receive packet pointer must be
    the same.

    @param         pFirstPacket   [in]  - The first of one ore more packet buffers
                                          sticking together in a single buffer each 
                                          with the same distance to the previous and 
                                          next packet.
                                          
    @param         packetDistance [in]  - The distance to prevoius and next packet if one.
    @param         numPackets     [in]  - Number of packets
    @param         messageList [in/out] - Message List

    @return        NoError or Error
    */
    SyncRet Set         (SAPDB_Byte*                    pFirstPacket, 
                         SAPDB_UInt4                    packetDistance,
                         SAPDB_UInt4                    numPackets,
                         SAPDBErr_MessageList &         messageList);

    /*!
    @brief         Check the send operation.

    This methode has to be called as one of the first operation before
    the real send code is performed.
 
    @param         packet      [in]     - Pointer to the packet to be send
    @param         messageList [out]    - Returns an error message if any

    @return       NoError, WrongPacket, PacketInUse, SendPending, ClosePending
    */
    SyncRet EnterSend   ( SAPDB_Byte const * const      packet,
                          SAPDBErr_MessageList &        messageList );



    /*!
    @brief         Signal that we leaved the send call.
    */
    void LeaveSend      () { m_SendPending = false; }

    /*!
    @brief         Check the receive operation.

    This methode has to be called as one of the first operation before
    the real receive code is performed.
 
    @param         messageList [out]    - Returns an error message if any

    @return       NoError, NoOutstandingPackets, ReceivePending, ClosePending
    */
    SyncRet EnterReceive( SAPDBErr_MessageList      &messageList );

    /*!
    @brief         Signal and of receive operation.

    This methode has to be called at the end of the receive operation
    to signal that a packet has be arrived and can be used by the next
    send operation.
 
    @param         packet      [in]     - Pointer to the received packet
    @param         messageList [out]    - Returns an error message if any

    @return       NoError, WrongPacket, NoReceivePending, NoOutstandingPackets
    */
    SyncRet LeaveReceive( SAPDB_Byte*               packet,
                          SAPDBErr_MessageList      &messageList );

    /*!
    @brief         Check the close operation

    This methode has to be called as one of the first operation before
    the real close code is performed.
    'ReceivePending' is returned as hint for the caller of this method that a pending
    receive exists. This is means no error. 
 
    @param         messageList [out]            - Returns an error message if any

    @return       NoError, ClosePending, ReceivePending
    */

    SyncRet EnterClose (SAPDBErr_MessageList &      messageList);

/*!
    @brief         Check the cancel operation

    This methode has to be called as one of the first operation before
    the real cancel code is performed. It is checked if a close operation
    is already pending.
 
    @param         messageList [out]            - Returns an error message if any

    @return       NoError, ClosePending
    */

    SyncRet EnterCancel (SAPDBErr_MessageList &      messageList);


private:
    /*! @brief    calcualte the slot of a given packet pointer */
    SAPDB_UInt4  CalcSlot ( SAPDB_Byte const * const packet ) 
    { 
        SAPDB_Long packetDiff = (SAPDB_Long)(packet - m_pFirstPacket);

        if (( packetDiff < 0 ) || ( 0 != (packetDiff % m_PacketDistance )))
          return (SAPDB_UInt4) -1;    // set maximum slot value

        return (SAPDB_UInt4)(packetDiff / m_PacketDistance);
    }

private:
    RTESync_Spinlock     m_Lock;                           // used for locked scopes

    SAPDB_Byte*          m_pFirstPacket;                   // pointer to the first packet
    SAPDB_UInt4          m_PacketDistance;                 // distance of each packet
    SAPDB_UInt4          m_NumPackets;                     // number of currently used packets

    SAPDB_Bool           m_SendPending;                    // signals that a Send is pending (EnterSend called)
    SAPDB_Bool           m_ReceivePending;                 // signals that a receive is pending (EnterReceive called)
    SAPDB_Bool           m_ClosePending;                   // signals that a close is pending (EnterClose called)
    SAPDB_UInt4          m_ReceivesOutstanding;            // number of outstanding receives
    SAPDB_Bool *         m_pPacketUsed;                    // packet informations, packet in use 
};

#endif  /* RTECKC_CLIENTCOMMSEQUENCECHECK_HPP */
