/*!========================================================================

  @file         RTECKC_KernelCommSequenceCheck.cpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Kernel send and receive sequence check

                The client kernel communication allows more than
                one packet to be send to the database kernel. The 
                following class allows to check the packet sequence
                correctness.

  @since        2003-11-04  13:11
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



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include    <memory.h>

#include    "RunTime/ClientKernelCommunication/RTECKC_KernelCommSequenceCheck.hpp"
#include    "SAPDBCommon/SAPDB_Types.hpp"
#include    "SAPDBCommon/SAPDB_ToString.hpp"
#include    "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include    "RunTime/RTE_CKC_Messages.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

RTECKC_KernelCommSequenceCheck::~RTECKC_KernelCommSequenceCheck()
{
    destroyarray (m_pPacketReceived, m_NumPackets, RTEMem_Allocator::Instance());
}

/*---------------------------------------------------------------------------*/

RTECKC_KernelCommSequenceCheck::SyncRet
    RTECKC_KernelCommSequenceCheck::Set (SAPDB_Byte*                    pFirstPacket, 
                                         SAPDB_UInt4                    packetDistance,
                                         SAPDB_UInt4                    numPackets,
                                         SAPDBErr_MessageList &         messageList)
{
    m_pFirstPacket    = pFirstPacket;
    m_PacketDistance  = packetDistance;
    m_NumPackets      = numPackets;

    if ( 0 == m_pFirstPacket || 0 == m_PacketDistance || 0 == m_NumPackets )
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CKC_WRONG_SET_PARAMETER,
                                                         SAPDB_ToString (m_pFirstPacket),
                                                         SAPDB_ToString (m_PacketDistance),
                                                         SAPDB_ToString (m_NumPackets)); 
        return Error;
    }

    newarray ( m_pPacketReceived, m_NumPackets, RTEMem_Allocator::Instance() );

    if ( 0 == m_pPacketReceived )
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_SYS_ALLOC_ARRAY_INST, 
                                                         SAPDB_ToString (m_NumPackets),
                                                         "SAPDB_Bool");
        return Error;
    }

    memset ( m_pPacketReceived, 0, m_NumPackets * sizeof(m_pPacketReceived[0]));

    return NoError;
}

/*---------------------------------------------------------------------------*/

RTECKC_KernelCommSequenceCheck::SyncRet
   RTECKC_KernelCommSequenceCheck::EnterReceive( SAPDBErr_MessageList      &messageList )
{
    SyncRet  retVal;

    if ( true == m_SendOutstanding )
    {
        messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_CKC_SEND_OUTSTANDING );
        retVal  = SendOutstanding;
    }
    else
        retVal = NoError;

    return retVal;
}

/*---------------------------------------------------------------------------*/

RTECKC_KernelCommSequenceCheck::SyncRet
   RTECKC_KernelCommSequenceCheck::LeaveReceive(SAPDB_Byte*               packet,
                                                SAPDBErr_MessageList      &messageList )
{
    SyncRet  retVal;

    // calculte the slot for packet related send / receive syncronization
    SAPDB_UInt4 slot = CalcSlot( packet );

    if ( slot >= m_NumPackets ) 
    {
        messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_CKC_WRONG_PACKET_PTR, ToStr(packet)  );
        retVal      = WrongPacket;
    }
    else if ( true == m_pPacketReceived[slot] )
    {
        messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_CKC_PACKET_IN_USE, ToStr(packet)  );
        retVal     = PacketInUse;
    }
    else
    {
        m_pPacketReceived[slot] = true;
        m_SendOutstanding       = true;
        retVal                  = NoError;
    }
    return retVal;
}

/*---------------------------------------------------------------------------*/

RTECKC_KernelCommSequenceCheck::SyncRet
   RTECKC_KernelCommSequenceCheck::EnterSend( SAPDB_Byte const * const   packet,
                                              SAPDBErr_MessageList      &messageList )
{
    SyncRet  retVal;

    if ( false == m_SendOutstanding )
    {
        messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_CKC_NO_OUTSTANDING_PCKTS  );
        retVal      = NoOutstandingPackets;
    }
    else
    {
        SAPDB_UInt4 slot = CalcSlot( packet );

        if (( slot >= m_NumPackets ) || ( false == m_pPacketReceived[slot] ))
        {
            messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_CKC_WRONG_PACKET_PTR, ToStr(packet)  );
            retVal      = WrongPacket;
        }
        else
        {
            m_pPacketReceived[slot] = false;
            m_SendOutstanding       = false;
            retVal                  = NoError;

        }
    }
    return retVal;
}


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/