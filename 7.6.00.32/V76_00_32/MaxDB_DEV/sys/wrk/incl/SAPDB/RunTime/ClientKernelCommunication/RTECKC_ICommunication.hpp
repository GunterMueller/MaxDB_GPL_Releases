/*!========================================================================

  @ingroup      CKC
  @file         RTECKC_ICommunication.hpp
  @author       StefanP

  @brief        Interface for the Client Kernel Communication. This Interface is
                only be used internally by the Client Runtime.
                Each kind of communication (NI, socket or local) has to implement
                the methods offered by this interface.
                (NI, socket, local)

  @since        2003-10-15  10:59
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


#ifndef RTECKC_ICOMMUNICATION_HPP
#define RTECKC_ICOMMUNICATION_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include    "RunTime/ClientKernelCommunication/RTECKC_ClientCommunication.hpp"
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


/*!
    @class	RTECKC_ICommunication     
    @brief  Interface class for the different kind of connection classes (local, TCP/IP, NI).  
*/

class RTECKC_ICommunication
{
public:
  /*!
     @brief     Destructor
  */

    virtual    ~RTECKC_ICommunication () {};    

  /*!
     @brief     Does some preparation for a following Close request).

     @param     messageList [out]		- Message list
  */

    virtual    RTECKC_ClientCommunication::CommRC    
                            PrepareClose    (SAPDBErr_MessageList &			messageList) = 0;

  /*!
     @brief		Close the local connection. All resources are destroyed.

     @param     messageList [out]		- Message list

     @return    true/false

  */
                                    
    virtual SAPDB_Bool      Close           (SAPDBErr_MessageList &          messageList) = 0;

  /*!
     @brief     Get pointer to a communication packet (data part)
	 @param     packetIdx [in]		- Index of the packet wanted (0 <= packetIndex < m_NumPacket)

     @return    Pointer to communication packet wanted or NULL when index is illegal 
                (out of range)
  */

    virtual SAPDB_Byte * const   GetDataPacket (SAPDB_UInt4 const          packetIdx) const = 0;

  /*!
     @brief     Send
                Send data to Kernel

     @param     pSendData [in]      - Pointer to the data to be send (the pointer must be
                                      received by 'GetDataPacket') 
     @param     sendLen [in]        - Data length
     @param     messageList [out]   - Message list
  */

    virtual RTECKC_ClientCommunication::CommRC  
                            Send            (SAPDB_Byte const * const       pSendData,
                                             SAPDB_UInt4 const              sendLen,
                                             SAPDBErr_MessageList &         messageList) = 0;

  /*!
     @brief     ReplyAvailable
                Determine if a reply is available in one of the communication packets

     @return    true/false
  */

    virtual SAPDB_Bool      ReplyAvailable  () const = 0;

  /*!
     @brief     Receive
                Receive data from the Kernel. It is ensured that we always receive the most 
                outstanding packet (First 'Send' - First 'Receive'). Insofar the 
                output parameter 'pReceiveData' may be useless for the caller.

     @param     pReceiveData [out]  - Pointer to the packet holding the data received
     @param     replyLen [out]      - Length of data received
     @param     messageList [out]   - Message list
  */

    virtual RTECKC_ClientCommunication::CommRC    
                            Receive         (SAPDB_Byte * &                 pReceiveData,
                                             SAPDB_UInt4 &                  replyLen,
                                             SAPDBErr_MessageList &         messageList) = 0;

  /*!
     @brief     CancelRequest
                Cancel the actual command of the Kernel session we are connected to. 
                Since this request is send asynchronously to the Kernel (Requestor)
                it is not guarantied which command is actually canceled. 
     @param     messageList [out]   - Message list
  */

    virtual RTECKC_ClientCommunication::CommRC    
                            CancelRequest   (SAPDBErr_MessageList &         messageList) = 0;

private:

};



#endif  /* RTECKC_ICOMMUNICATION_HPP */

