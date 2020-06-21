/*!========================================================================

  @ingroup      CKC
  @file         RTECKC_ClientCommunicationSegment.hpp
  @author       StefanP

  @brief        Communication Segment for the local Client Kernel Communication
                Client Interface

  @since        2003-11-03  13:21
  @sa           


  =======================================================================

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
  ----------------------------------------------------------------------------*/


#ifndef RTECKC_CLIENTCOMMUNICATIONSEGMENT_HPP
#define RTECKC_CLIENTCOMMUNICATIONSEGMENT_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include    "RunTime/ClientKernelCommunication/RTECKC_CommunicationSegment.hpp"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"


extern SAPDBTrace_Topic CKC_Trace;///<CKC Trace

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


/*!
    @class	RTECKC_ClientCommunicationSegment          
    @brief  This class is derived from RTECKC_CommunicationSegment and inludes the 
			the client specific methods.
     		(Cf. RTECKC_CommunicationSegment.hpp for details about the communication
			segment).

*/


class RTECKC_ClientCommunicationSegment : public RTECKC_CommunicationSegment
{
public:

  /*!
     @brief			Constructor

     @param         mpu [in]				- Maximum packet unit - Maximum size (in bytes) of a packet (header + data)
     @param         numPacket [in]			- Number of packets to be used for the communication

  */									      

    RTECKC_ClientCommunicationSegment   (SAPDB_UInt4 const              mpu,
                                         SAPDB_UInt4 const              numPacket) 
        :   RTECKC_CommunicationSegment (RTECKC_Client, mpu, numPacket)
        {   
            SAPDBTRACE_METHOD_DEBUG ("RTECKC_ClientCommunicationSegment::RTECKC_ClientCommunicationSegment", 
                                     CKC_Trace, 9);
        }


  /*!
     @brief			Check if a reply on one (!) of the packets is available. (This is done by checking 
					if one of the packets has the server connect state 'Pending'.)

     @return        true:	Reply is available.
					false:	Reply is not available.

  */

    SAPDB_Bool      ReplyAvailable          () const;

  /*!
     @brief			Get the semaphore index of the task scheduler where the session actually runs in
					(due to load balancing the session (task) may move to another task scheduler
					during runtime).
					This index is used to find the name of the semaphore in the kernel shared 
					communication flag section (FLAG_COMM_SEG).

     @return		Index
  */

    SAPDB_UInt4     GetActualTaskSchedulerSemaphoreIdx         () const;

  /*!
     @brief			Get the pointer to the reply packet, if available.
					The packet is available if 
						1. A packet with the expected sequence number is available 
						2. The server connection state is 'Pending'			
					
					No checks concerning the communication state are made. This method is meant
					for reply packets holding error message lists and may be called after an
					call to 'Receive' or 'ClientConnectReceive' which returns with an return code
					indicating an error event in the communication state of the server (i.e. a return
					code other than WrongState, DataMissing, DataOutstanding or DataAvailable).

					If the packet could be received the connection state is updated accordingly.

     @param         pPacket [out]			- Pointer to the packet holding the data received  

     @return		true:	Packet is available.
					false:	Packet is not available.
  */

    SAPDB_Bool      RemotePacketAvailable   (SAPDB_Byte * &             pPacket);

  /*!
     @brief			Get size of communication segment header (excluding header of the packets)
                    
     @param         numPackets [in]			- Number of packets to be used for the communication

     @return        Size of the communication segment header

  */

    static  SAPDB_UInt4     GetCommSegHeaderSize    (SAPDB_UInt4     numPackets);

private:


#define SHM_COMMSEG_NAME_SKELETON       "SDBTech-Communication-Segment-" ///<Fixed part of the communication segment name

  /*!
     @brief			Copy Constructor - By declaring the copy constructor as private we prevent that
					he can be called from outside
  */

	inline  RTECKC_ClientCommunicationSegment   (const RTECKC_ClientCommunicationSegment & commSeg)
        : RTECKC_CommunicationSegment (RTECKC_Client)
 {;};

};

#endif  /* RTECKC_CLIENTCOMMUNICATIONSEGMENT_HPP */

