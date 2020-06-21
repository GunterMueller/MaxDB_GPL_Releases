/*!========================================================================

  @ingroup      IPC
  @file         RTEIPC_Mailslot.hpp
  @author       StefanP

  @brief        Primitives for Mailslot Handling

  @since        2003-11-13  15:16
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


#ifndef RTEIPC_MAILSLOT_HPP
#define RTEIPC_MAILSLOT_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include    "RunTime/IPC/RTEIPC_CommunicationObject.hpp"
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
    @class          RTEIPC_Mailslot
    @brief			Basic class for Mailslot handling
                    
                    This class is derived from the abstract communication class RTEIPC_CommunicationObject.
					The virtual methods of RTEIPC_CommunicationObject are implemented as well as a method for
					opening a mailslot.
                                        
                    It is always ensured that one reads a full message of the wanted size. Therefore especially 
                    on UNIX the risk that one reads some garbage which someone has erroneously written to the 
                    mailslot (file) is minimized considerable.  
                    
                    (On NT the mailslot is realized by given API functions. On UNIX the consistency is
                    guarantied by adding a kind of bit stuffing method to the standard FIFO functionality of
                    the system)

                    UNIX only: It has to be taken notice of the maximum allowed message length (a method for 
                               the length is offered (see below)). Messages which exceed this length are 
                               refused.
*/


class   RTEIPC_Mailslot : public RTEIPC_CommunicationObject
{
public:


  /*!
     @brief			Constructor
     @return        none

  */
    
    RTEIPC_Mailslot                 ();

  /*!
     @brief			Destructor
     @return        none
  */

    virtual ~RTEIPC_Mailslot     ();

  /*!
     @brief			Open or create a mailslot object (depends on the given open mode)

     @param         mailslotName [in]      - Reference of the full name of the mailslot
     @param         openMode [in]          - RTE_OpenExisting or RTE_OpenNew
     @param         secAttribute [in]      - Reference of the security attribute for creating the mailslot.
  										     Is not relevant for opening. In that case 
													RTESys_SecurityAttributes::Instance().GetNoSA () can be 
											 given.	
											 !!!Caution: For UNIX the security attribute (file permission) has to be given
														 absolut, i.e. an existing umask is not considered.
     @param         messageList [out]	   - Message list

     @return        NoError, Error, ResourceMissing

  */

    CommObjRC    Open            (RTE_Path const &                  mailslotName,
                                  RTE_OpenMode const                openMode,
                                  RTE_SECURITY_ATTRIBUTES &         secAttribute,   
                                  SAPDBErr_MessageList &            messageList);
            


  /*!
     @brief			Close the mailslot object

     @param         messageList [out]		- Message list

     @return        true/false

  */

    virtual SAPDB_Bool    Close           (SAPDBErr_MessageList &      messageList);


  /*!
     @brief			Write message to mailslot

					The bytes to be written should not exceed the maximum allowed length given by 
					GetMaxAllowedObjectLength.

     @param         pMessage [in]           - Pointer to the message
     @param         bytesToWrite [in]       - Number of bytes to be written
     @param         bytesWritten [out]      - Number of bytes written
     @param         messageList [out]		- Message list

     @return        NoError, Error, ResourceMissing

  */

    virtual CommObjRC     Write   (SAPDB_Byte const * const    pMessage,
                                   SAPDB_UInt4 &               bytesToWrite,
                                   SAPDB_UInt4 &               bytesWritten,
                                   SAPDBErr_MessageList &      messageList);

  /*!
     @brief			Read message from mailslot

                    The call is blocking. That means if no message is actually in the mailslot
                    the call listens at the mailslot until a message is available.

     @param         pMessage [out]          - Pointer to the message
     @param         bytesRead [out]         - Number of bytes read
     @param         messageList [out]		- Message list

     @return        NoError, Error

  */

    virtual CommObjRC     Read    (SAPDB_Byte * const          pMessage,
                                   SAPDB_UInt4 &               bytesRead,
                                   SAPDBErr_MessageList &      messageList);

  /*!
     @brief			Determine the maximum allowed length of a single message (buffer) written/read to/from
					the mailslot. 

					May be called after opening the mailslot.

     @return		Length
  */

    virtual SAPDB_UInt8      GetMaxAllowedObjectLength ()
        const
        {
            return m_MaxMessageLength;
        }


private:
  /*!
     @brief			Increment given value by one as long as the value is smaller than SAPDB_MAX_UINT4.
	 				Otherwise the value is set to zero again.
  */
    #define     MAX_MESSAGE_LENGTH        4096 ///<NT only: Maximum allowed message length 

  /*!
     @brief			Prevent calling the copy constructor from outside
  */

	inline   RTEIPC_Mailslot  (const RTEIPC_Mailslot & mailslot) {;};

#if !defined (_WIN32)
  /*!
     @brief			Determine if the communication object named by 'm_MailslotName' is a FIFO
     @return        NoError, Error, ResourceMissing
  */

    CommObjRC           IsFIFO      (SAPDBErr_MessageList &         messageList);
#endif

    RTE_FileHandle      m_Handle;
    RTE_Path            m_MailslotName;
    RTE_OpenMode        m_OpenMode;
    SAPDB_UInt4         m_MaxMessageLength;

#if !defined(_WIN32)
    SAPDB_Byte *        m_pBuffer;
    SAPDB_UInt8         m_BufferSize;
    SAPDB_UInt4         m_BytesInBuffer;
#endif

};


#endif  /* RTEIPC_MAILSLOT_HPP */
