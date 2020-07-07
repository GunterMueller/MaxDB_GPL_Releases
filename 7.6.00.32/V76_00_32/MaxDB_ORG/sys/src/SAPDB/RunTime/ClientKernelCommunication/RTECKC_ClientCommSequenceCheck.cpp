/*!========================================================================

  @ingroup      CKC
  @file         RTECKC_ClientCommSequenceCheck.cpp
  @author       RaymondR
  @brief        Client send and receive sequence check  

                The client kernel communication allows more than
                one packet to be send to the database kernel. This
                means it is allowed to the client to send all available
                packets before a receive called to get the result. 

  @since        2006-02-23  14:01
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

#include    "RunTime/ClientKernelCommunication/RTECKC_ClientCommSequenceCheck.hpp"
#include    "SAPDBCommon/SAPDB_Types.hpp"
#include    "RunTime/Synchronisation/RTESync_Spinlock.hpp"
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
RTECKC_ClientCommSequenceCheck::~RTECKC_ClientCommSequenceCheck()
{
    destroyarray (m_pPacketUsed, m_NumPackets, RTEMem_Allocator::Instance());
}

/*---------------------------------------------------------------------------*/

RTECKC_ClientCommSequenceCheck::SyncRet
    RTECKC_ClientCommSequenceCheck::Set (SAPDB_Byte*                    pFirstPacket, 
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

    newarray (m_pPacketUsed, m_NumPackets, RTEMem_Allocator::Instance());

    if ( 0 == m_pPacketUsed )
    {
        messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_SYS_ALLOC_ARRAY_INST, 
                                                         SAPDB_ToString (m_NumPackets),
                                                         "SAPDB_Bool");
        return Error;
    }

    memset ( m_pPacketUsed, 0, m_NumPackets * sizeof(m_pPacketUsed[0]));

    return NoError;
}

/*---------------------------------------------------------------------------*/

RTECKC_ClientCommSequenceCheck::SyncRet
   RTECKC_ClientCommSequenceCheck::EnterSend( SAPDB_Byte const * const   packet,
                                              SAPDBErr_MessageList      &messageList )
{
    SyncRet  retVal;

    SAPDB_UInt4 slot = CalcSlot( packet );

    if ( slot >= m_NumPackets ) 
    {
        messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_CKC_WRONG_PACKET_PTR, ToStr(packet)  );
        retVal      = WrongPacket;
    }
    else
    {
        m_Lock.Lock(); 

        if (true == m_ClosePending)
        {   //Close is already pending
            m_Lock.Unlock(); // do a unlock before creating message list
            messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEINFO_CKC_CLOSE_PENDING);
            retVal      = ClosePending;
        }
        else
            if ( true == m_pPacketUsed[slot] )
        {
            m_Lock.Unlock(); // do a unlock before creating message list
            messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_CKC_PACKET_IN_USE, ToStr(packet)  );
            retVal     = PacketInUse;
        }
        else
            if ( true == m_SendPending )
        {
            m_Lock.Unlock(); // do a unlock before creating message list
            messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_CKC_SEND_PENDING  );
            retVal      = SendPending;
        }
        else
        {
            ++m_ReceivesOutstanding;
            m_pPacketUsed[slot] = true;
            m_SendPending      = true;
            m_Lock.Unlock(); // do a unlock before creating message list
            retVal             = NoError;
        }
    }
    return retVal;
}

/*---------------------------------------------------------------------------*/

RTECKC_ClientCommSequenceCheck::SyncRet
   RTECKC_ClientCommSequenceCheck::EnterReceive( SAPDBErr_MessageList      &messageList )
{
    SyncRet  retVal;

    m_Lock.Lock(); // do a unlock before creating message list

    if (true == m_ClosePending)
    {   //Close is already pending
        m_Lock.Unlock(); // do a unlock before creating message list
        messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEINFO_CKC_CLOSE_PENDING);
        retVal      = ClosePending;
    }
    else // any receives outstanding?
        if ( 0 == m_ReceivesOutstanding )
    {
        m_Lock.Unlock(); // do a unlock before creating message list
        messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_CKC_NO_OUTSTANDING_PCKTS  );
        retVal      = NoOutstandingPackets;
    }
    else // anybody else already waiting? 
        if ( true == m_ReceivePending )
    {
        m_Lock.Unlock(); // do a unlock before creating message list
        messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_CKC_RECEIVE_ALREADY_PENDING  );
        retVal      = ReceivePending;
    }
    else
    {   // signal that we are waiting for a receive
        m_ReceivePending = true;
        m_Lock.Unlock(); // do a unlock before creating message list
        retVal           = NoError;
    }

    return retVal;
}

/*---------------------------------------------------------------------------*/

RTECKC_ClientCommSequenceCheck::SyncRet
   RTECKC_ClientCommSequenceCheck::LeaveReceive(SAPDB_Byte*               packet,
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
    else if ( false == m_ReceivePending )
    {
        messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_CKC_LEAVE_WITHOUT_ENTER_RECV, ToStr(packet)  );
        retVal      = NoReceivePending;
    }
    else
    {
        m_Lock.Lock(); // do a unlock before creating message list

        if ( 0 == m_ReceivesOutstanding )
        {
        m_Lock.Unlock(); // do a unlock before creating message list
        messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_CKC_NO_OUTSTANDING_PCKTS  );
        retVal      = NoOutstandingPackets;
        }
        else if ( false == m_pPacketUsed[slot] )
        {
            m_Lock.Unlock(); // do a unlock before creating message list
            messageList = SAPDBErr_MessageList( RTE_CONTEXT, RTEERR_CKC_WRONG_PACKET_PTR, ToStr(packet)  );
            retVal      = WrongPacket;
        }
        else
        {
            --m_ReceivesOutstanding;
            m_pPacketUsed[slot] = false;
            m_ReceivePending   = false;
            m_Lock.Unlock(); // do a unlock before creating message list
            retVal             = NoError;
        }
    }
    return retVal;
}

/*---------------------------------------------------------------------------*/

RTECKC_ClientCommSequenceCheck::SyncRet
   RTECKC_ClientCommSequenceCheck::EnterClose(SAPDBErr_MessageList &    messageList)
{
    SyncRet  retVal = NoError;

    m_Lock.Lock();

    if (true == m_ClosePending)
    {   //Close is already pending
        m_Lock.Unlock(); // do a unlock before creating message list
        messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEINFO_CKC_CLOSE_PENDING);
        retVal      = ClosePending;
    }
    else
    {   //signal close pending
        m_ClosePending = true;
        m_Lock.Unlock();
        if (true == m_ReceivePending)
        {
            retVal = ReceivePending;
        }
    }

    return retVal;
}

/*---------------------------------------------------------------------------*/

RTECKC_ClientCommSequenceCheck::SyncRet
   RTECKC_ClientCommSequenceCheck::EnterCancel(SAPDBErr_MessageList &    messageList)
{
    SyncRet  retVal = NoError;

    if (true == m_ClosePending)
    {   //Close is already pending
        messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEINFO_CKC_CLOSE_PENDING);
        retVal      = ClosePending;
    }

    return retVal;
}
/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/