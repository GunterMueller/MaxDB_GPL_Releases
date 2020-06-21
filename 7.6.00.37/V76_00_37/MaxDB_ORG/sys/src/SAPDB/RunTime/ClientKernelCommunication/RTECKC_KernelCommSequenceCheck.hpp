/*!========================================================================

  @file         RTECKC_KernelCommSequenceCheck.hpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Kernel send and receive sequence check

                The client kernel communication allows more than
                one packet to be send to the database kernel. The 
                following class allows to check the packet sequence
                correctness.

  @since        2003-10-27  17:37
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


#ifndef RTECKC_KERNELCOMMSEQUENCECHECK_HPP
#define RTECKC_KERNELCOMMSEQUENCECHECK_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include    "SAPDBCommon/SAPDB_Types.hpp"
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
  @class   RTECKC_KernelCommSequenceCheck
  @brief   Kernel send and receive sequence check.

           The client kernel communication allows more than
           one packet to be send to the database kernel. 

           This class is used to aware the sequence of the send
           and receive calls driven by the database kernel.
           If the send/receive calls are out of sequence
           a bad retrun code and a error message will be the result.

           For example it is not allowed to:
                - call send without a previous receive call
                - call the send twice for the same packet
                - call the send with an invalid packet pointer

           This class has some contrains to the packet buffer to be fulfilled.
           
           If there is more than one packet buffer these buffers must stick 
           together in a single buffer. This single buffer is separeted in multiple
           packet buffers each packet with the same distance to previous and 
           next one.

 ----------------------------------------------------------------------------*/

class RTECKC_KernelCommSequenceCheck
{
public:
    /*!  @brief   Possible methode return values */
    enum SyncRet  { NoError = 0,
                    Error,               // Error during initialization
                    WrongPacket,         // wrong packet pointer
                    PacketInUse,         // packet already in use
                    SendOutstanding,     // outstanding send operation
                    NoOutstandingPackets // no packet outstanding LeaveReceive() must be called first
                  };

public:
    /*!
    @brief         Constructor

    Creates a kernel sequence check class
    */
    RTECKC_KernelCommSequenceCheck() : m_pFirstPacket(0),
                                       m_PacketDistance(0),
                                       m_NumPackets(0),
                                       m_pPacketReceived(0),
                                       m_SendOutstanding(false)
    {}

    /*!
    @brief         Destructor

    Destroy the kernel sequence check class
    */
    ~RTECKC_KernelCommSequenceCheck();



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
    @brief         Check the receive operation.

    This methode has to be called as one of the first operation before
    the real receive code is performed.
 
    @param         messageList [out]    - Returns an error message if any

    @return       NoError, SendPending
    */
    SyncRet EnterReceive( SAPDBErr_MessageList      &messageList );

    /*!
    @brief         Signal and of receive operation.

    This methode has to be called at the end of the receive operation
    to signal that a packet has be arrived and must be replied by the next
    send operation.
 
    @param         packet      [in]     - Pointer to the received packet
    @param         messageList [out]    - Returns an error message if any

    @return       NoError, WrongPacket, PacketInUse
    */
    SyncRet LeaveReceive( SAPDB_Byte*               packet,
                          SAPDBErr_MessageList      &messageList );

    /*!
    @brief         Check the send operation.

    This methode has to be called as one of the first operation before
    the real send code is performed.
 
    @param         packet      [in]     - Pointer to the packet to be send
    @param         messageList [out]    - Returns an error message if any

    @return       NoError, WrongPacket
    */
    SyncRet EnterSend   ( SAPDB_Byte const * const      packet,
                          SAPDBErr_MessageList &        messageList );





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
    SAPDB_Byte*          m_pFirstPacket;                   // pointer to the first packet
    SAPDB_UInt4          m_PacketDistance;                 // distance of each packet
    SAPDB_UInt4          m_NumPackets;                     // number of currently used packets
    SAPDB_Bool *         m_pPacketReceived;                // packet informations, packet received
    SAPDB_Bool           m_SendOutstanding;                // packet send outstanding
};

#endif  /* RTECKC_KERNELCOMMSEQUENCECHECK_HPP */
