/*!========================================================================

  @ingroup      CKC
  @file         RTECKC_ClientCommunicationSegment.cpp
  @author       StefanP

  @brief        Communication Segment for the Client Kernel Communication

  @since        2003-11-03  13:21
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
#include    "RunTime/ClientKernelCommunication/RTECKC_CommunicationSegmentHeader.hpp"
#include    "RunTime/ClientKernelCommunication/RTECKC_ClientCommunicationSegment.hpp"
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

SAPDB_Bool  RTECKC_ClientCommunicationSegment::ReplyAvailable ()
const
{
    SAPDB_UInt4                                     idx;
    
    SAPDBTRACE_METHOD_DEBUG ("RTECKC_CommunicationSegment::ReplyAvailable", CKC_Trace, 9);

    //Dirty look for an available Reply Packet
    for (idx = 0; idx <= GetNumberPackets (); ++idx)
    {
        if (RTECKC_CommSegPacketInfo::Pending == 
            (GetPacketInfo () + idx)->GetConnectionState (RTECKC_CommSegPacketInfo::Remote))
        {
                break;
        }
    }

    if (idx == GetNumberPackets ())
    {
        return false;
    }

    return true;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4     RTECKC_ClientCommunicationSegment::GetActualTaskSchedulerSemaphoreIdx () 
const
{
    SAPDBTRACE_METHOD_DEBUG ("RTECKC_CommunicationSegment::GetActualTaskSchedulerSemaphoreIdx", CKC_Trace, 9);

    return GetLoadBalancingOption ()->GetTaskSchedulerSemaphoreIdx ();
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool      RTECKC_ClientCommunicationSegment::RemotePacketAvailable (SAPDB_Byte * &      pPacket) 
{
    RTECKC_CommSegPacketInfo::ConnectionStateType   remoteState;

    SAPDBTRACE_METHOD_DEBUG ("RTECKC_CommunicationSegment::RemoteSendPacket", CKC_Trace, 9);

    if (!GetNextReceivePacket ())
    {
        return false;
    }

    remoteState = (GetPacketInfo () + GetActualReceivePacket ())->GetConnectionState (RTECKC_CommSegPacketInfo::Remote);

    if (RTECKC_CommSegPacketInfo::Pending != remoteState)
    {
        return false;
    }

    pPacket = GetFirstPacket () + GetActualReceivePacket () * GetMPU ();

    (GetPacketInfo () + GetActualReceivePacket ())->SetConnectionState (RTECKC_CommSegPacketInfo::Own, 
        RTECKC_CommSegPacketInfo::Received);

    (GetPacketInfo () + GetActualReceivePacket ())->SetConnectionState (RTECKC_CommSegPacketInfo::Remote, 
        RTECKC_CommSegPacketInfo::Send);

    SetActualReceivePacket (RTECKC_UNDEF_COMMUNICATION_PACKET_IDX);

    return true;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4     RTECKC_ClientCommunicationSegment::GetCommSegHeaderSize    (SAPDB_UInt4     numPackets)
{
    SAPDBTRACE_ROUTINE_DEBUG ("RTECKC_CommunicationSegment::GetCommSegHeaderSize", 
        CKC_Trace, 9);

    return (RTECKC_CommSegHeader::GetCommSegHeaderBufferSize () + 
        RTECKC_CommSegHeaderCommonOptionPart::GetOptionPartBufferSize () + 
        RTECKC_LoadBalancingOption::GetLoadBalancingOptionBufferSize() +
        numPackets * RTECKC_CommSegPacketInfo::GetPacketInfoBufferSize ());
}

