/*!========================================================================

  @ingroup      CKC
  @file         RTECKC_RequestChannel.hpp
  @author       StefanP

  @brief        Request Channel

                Implementation of the communication channel between client
                applications and the kernel requestor

  @since        2003-11-13  14:59
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


#ifndef RTECKC_REQUESTCHANNEL_HPP
#define RTECKC_REQUESTCHANNEL_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include    "RunTime/IPC/RTEIPC_Mailslot.hpp"
#include    "RunTime/ClientKernelCommunication/RTECKC_ClientCommunication.hpp"
#include    "RunTime/ClientKernelCommunication/RTECKC_Request.hpp"
#include    "RunTime/RTE_MessageList.hpp"



/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
 
#if defined (_WIN32)
#define RTECKC_REQUEST_CHANNEL_PATH                "\\\\.\\mailslot\\SDBTech\\ClientKernelCommunication\\"	///<path name of the request channel
#define RTECKC_REQUEST_CHANNEL_NAME                "MAILSLOT-CLIENT-KERNEL-COMMUNICATION-" ///<static part of the name of the request channel 
#else
#define RTECKC_REQUEST_CHANNEL_NAME                "CLIENT-KERNEL-COMMUNICATION" ///<static part of the name of the request channel
//#define RTECKC_REQUEST_CHANNEL_PERMISSION          (S_IRUSR | S_IWUSR | S_IWGRP | S_IWOTH )
#endif


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


/*!
    @class          RTECKC_RequestChannel
    @brief			This class implements a request channel between client applications and the 
					kernel (requestor thread) for reading and writing requests
                    of type RTECKC_Request. The kernel creates the request channel 
					(open mode RTE_OpenNew) and the client application has to attach that channel 
					(open mode RTE_OpenExisting).
					Internally a mailslot is used as communication object.
*/


class RTECKC_RequestChannel
{
public:

  /*!
     @brief			Constructor
  */	

	RTECKC_RequestChannel			() {};

  /*!
     @brief			Destructor
  */

    ~RTECKC_RequestChannel			();

  /*!
     @brief    Open the request channel (open the mailslot)
                     
     @param          openMode [in]          - RTE_OpenExisting or RTE_OpenNew
     @param          serverDB [in]          - Name of the server DB used to build the name of the 
											  request channel (this is the variable part of the name)
     @param          secAttribute [in]      - Input depends on the open mode:
											  RTE_OpenNew:		Security attribute to be used for creating the 
																request channel (cf. RTESys_SecurityAttributes.hpp) 
											  RTE_OpenExisting: RTESys_SecurityAttributes::Instance().GetNoSA ()
     @param          messageList [out]		- Message list

     @return   NoError, Error, ResourceMissing (RTE_OpenExisting only)  
  */

    RTEIPC_CommunicationObject::CommObjRC      
                    Open            (RTE_OpenMode                   openMode,
                                     SAPDB_Char *                   serverDB,
                                     RTE_SECURITY_ATTRIBUTES  &     secAttribute,   
                                     SAPDBErr_MessageList &         messageList);

  /*!
     @brief    Close request channel (close mailslot)

     @param          messageList [out]		- Message list
                     
     @return   true/false
  */

    SAPDB_Bool      Close           (SAPDBErr_MessageList &         messageList);


  /*!
     @brief    Read request from the request channel
                     
     @param          request     [out]      - Request to be read
     @param          messageList [out]		- Message list

     @return   true/false
  */

    SAPDB_Bool      Read            (RTECKC_Request &               request,
                                     SAPDBErr_MessageList &         messageList);

  /*!
     @brief    Write request to request channel

     @param          request     [in]       - Request to be written
     @param          messageList [out]		- Message list

     @return        NoError, Error, ResourceMissing
  */

    RTEIPC_CommunicationObject::CommObjRC
                    Write           (RTECKC_Request const &         request,
                                     SAPDBErr_MessageList &         messageList);

  /*!
     @brief    Determine the maximum allowed length of a request. (Requests of this length 
			   are guarantied to be written atomically). 
  */

    SAPDB_UInt4     GetMaxRequestLength ()
        const
        {
            return (static_cast<SAPDB_UInt4> (m_Mailslot.GetMaxAllowedObjectLength ()));
        }

private:
    // It is not allowed to call the Copy Constructor from outside
    inline   RTECKC_RequestChannel  (const RTECKC_RequestChannel & reqChannel) {};


    RTEIPC_Mailslot                 m_Mailslot;
};



#endif  /* RTECKC_REQUESTCHANNEL_HPP */

