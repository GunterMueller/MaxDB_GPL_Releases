/*!
  @file           RTECons_Communication.hpp
  @author         StefanP
  @special area   Console (XCons)
  @brief          Communication
  @see            

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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



#ifndef RTECONS_COMMUNICATION_HPP
#define RTECONS_COMMUNICATION_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include    "RunTime/RTE_ConsoleDataCommunication.hpp"
#include    "RunTime/RTE_ConsoleRequestCommunication.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*===========================================================================*
 *  PROTOTYPS                                                                *
 *===========================================================================*/
/*!
  @class          RTECons_Communication
   @description    XCons request and data communication


                This class hides the request and data communication
                from the client (XCons) to the server (Console Thread). 
                It is derived from the more abstract communication classes 
                RTE_ConsoleRequestCommunication and RTE_ConsoleDataCommunication 
                respectively.
                The communication is initialized and the protocol handling for 
                sending the requests is also done by instances of this class.

 */

class RTECons_Communication:    public RTE_ConsoleDataCommunication, 
                                public RTE_ConsoleRequestCommunication
{

public:

    enum CommRC {NoError = 0,	///<No error 
                 Error,			///<Error (not specified)
                 Offline 	    ///<Database is offline
               };	
  /*!
     @description    Constructor
                Calls the constructor of RTE_ConsoleDataCommunication and
                RTE_ConsoleRequestCommunication and does some initialization.
     @param          szServerDB [in] - Name of the destination server DB
     @param          communicationType [in] - Type of communication
                                           (RTE_CONS_COM_STANDARD,...)

   */

    RTECons_Communication       (SAPDB_Char const * const   szServerDB,
                                 SAPDB_UInt4     const      communicationType);

  /*!
     @description    Destructor
                On WIN32: Free the security descriptor

   */

    ~RTECons_Communication      ();

  /*!
     @description    Initialize the request and data communication
                WIN32: Initialize the security attributes
     @param          messageList [in/out] - Message list
     @return value   NoError, Error, Offline

   */
 

    CommRC      Initialize      (SAPDBErr_MessageList &             messageList);


  /*!
     @description    Sending the open/connect request and the data to server
     @param          reqRespType [in] - OPEN_REQ, DATA_REQ, CLOSE_REQ
     @param          security [in] - true: Security attribute is set, false: else
     @param          pData [in] - Pointer to data to be send
     @param          size [in] - Size of data to be send
     @param          messageList [in/out] - Message list
     @return         RTE_CONS_NO_ERROR/RTE_CONS_ERROR/RTE_CONS_TIMEOUT/RTE_CONS_REMOTE_TIMEOUT
   */


    SAPDB_UInt4 SendRequestData (SAPDB_UInt4 const                  reqRespType,
                                 SAPDB_Bool const                   security,
                                 SAPDB_Byte const * const           pData,
                                 SAPDB_UInt4 const                  size,
                                 SAPDBErr_MessageList &             messageList);


  /*!
     @description    Set the connect handle in the data structure for the connect requests.
                This method can be called just after the connect data is replied by
                the server.
     @param          hConnect [in] - Connection handle returned by the server
     @return value   none

   */
 

    void    SetConnectHandle    (RTE_ConsoleHandle const            hConnect)
        {m_request.data.hConnect = hConnect;}

  /*!
     @description    Get the connection handle
     @return value   Connection handle

   */
 

    RTE_ConsoleHandle   GetConnectHandle    () const
        {return m_request.data.hConnect;}

  /*!
     @description    Get the Kernel version
     @return value   Kernel version

   */
 

    SAPDB_Char * const  GetKernelVersion    () const
        {return (SAPDB_Char *)m_KernelVersion;}

  /*!
     @description    Get the RTE version
     @return value   RTE version

   */
 

    SAPDB_Char * const   GetRTEVersion       () const
        {return (SAPDB_Char *)m_RTEVersion;}

  /*!
     @description    Generate a unique global connection counter


                  This counter is used as own reference for the data communication 
                  to the server side.
     @return value   

   */


    static  SAPDB_UInt4     GenerateConnectionCounter (void);    

private:
    SAPDB_Bool  CheckAccessRight    (SAPDBErr_MessageList&          messageList);


    static  SAPDB_UInt4                     m_ConnectionCounter;
    static  RTE_Lock                        m_ConnectionCounterSpinlock;

            RTE_DBName                      m_ServerDB;
            RTE_ConsoleRequest              m_request;
            RTEComm_PacketHeader            m_header;
            RTE_Version                     m_RTEVersion;
            RTE_Version                     m_KernelVersion;
            
            RTE_SECURITY_ATTRIBUTES         m_WorldSA;                      // NT Only
};

inline SAPDB_ToStringClass SAPDB_ToString(RTECons_Communication::CommRC     rcComm)
{
    switch (rcComm)
    {
    case RTECons_Communication::NoError: 
        return SAPDB_ToStringClass("NoError");
    case RTECons_Communication::Error: 
        return SAPDB_ToStringClass("Error");
    case RTECons_Communication::Offline: 
        return SAPDB_ToStringClass("Offline");
    default:                          
        break;
    }

    return SAPDB_ToStringClass((SAPDB_Int4)rcComm); 
}

 
#endif  /* RTECONS_COMMUNICATION_HPP */
