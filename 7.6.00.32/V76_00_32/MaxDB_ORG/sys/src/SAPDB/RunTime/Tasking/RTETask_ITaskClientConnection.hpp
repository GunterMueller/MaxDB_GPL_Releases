/*!========================================================================

  @file         RTETask_ITaskClientConnection.hpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Single client kernel (task) connection

                This class conatains all information of a 
                single client kernel connection. 
                Each task connected to a client application(s)
                has at least one connection object in  
                it's task client connection list. 
                A new connection to a client leads to new
                object in this list. If a connection is 
                dropped its object is removed from list.

  @since        2005-11-04  14:09
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


#ifndef RTETASK_ITASKCLIENTCONNECTION_HPP
#define RTETASK_ITASKCLIENTCONNECTION_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include    "geo003.h"
#include    "SAPDBCommon/SAPDB_Types.hpp"
#include    "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp"


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
  @class   RTETask_ITaskClientConnection
  @brief   Single client kernel (task) connection.

           This class conatains all information of a 
           single client kernel connection. 
           Each task connected to a client application(s)
           has at least one connection object in  
           it's task client connection list. 
           A new connection to a client leads to new
           object in this list. If a connection is 
           released its object is removed from list.

           A task that wants to wait for a new connection 
           has to call the 'ClientConnect' methode of the 
           'RTETask_Task' object. This methode returns a 
           reference to an object with the following interface
           class.

           To leave this connection and drop this object call 
           the 'Release' methode.

 ----------------------------------------------------------------------------*/

class RTETask_ITaskClientConnection
{
public:

    /*!  @brief   Type of connection */
    enum ConnectionType {UnknownConnection = 0,
                         Utility,
                         Ping,
                         Event,
                         OLTP,
                         LiveCache
                        };

    /*!  @brief   Type of encryption */
    enum EncryptionType {UnknownEncryption = 0,
                         NoEncryption = RTE_NO_ENCRYPTION,
                         NISSL = RTE_NI_SSL_ENCRYPTION
                        };


    /*!  @brief   Possible return values */
    enum ConnRet  { NoError = 0,
                    Error,               // Common error occured
                    ConnectionClosed,    // Connection closed by communication partner
                    SessionTimeout,      // Session receive timed out
                    SequenceError        // Wrong sequence of the send and receive calls
                                         //   For example it is not allowed to:
                                         //    - call send without a previous receive call
                                         //    - call the send twice for the same packet
                                         //    - call the send with an invalid packet pointer
                  };

    const SAPDB_UInt4 NoSessionTimeout;  // Use this value for 'Receive' calls if no
                                         // no session timeout is required.

public:
    /*!
    @description    Get the maximum allowed send data length.
                    
                    After receiving data via 'Receive', a pointer
                    to a transfer buffer has been returned holding 
                    the incomming data. This buffer has to be 
                    reused to reply date to the client via 'Send'.
                    The maximum allowed data that can be written
                    into this buffer is returned by 'MaxSendDataLength'.

                    WARNING:
                     Writting more than this lenght of data 
                     into the buffer might lead to a fatal
                     program error.

    @return     maximum data length that is allowed to put into 
                the transfer buffer. 
    */
    virtual SAPDB_UInt4  MaxSendDataLength() = 0;


    /*!
    @description    Receive data from the client application.
                    
                    After receiving data via 'Receive', a pointer
                    to a transfer buffer (pData) has been returned 
                    holding the incomming data. If the given timeout
                    value has expired before any data has been 
                    received, this methode returns with the error 
                    code 'SessionTimeout'.

    @param          timeOut    [in]  - Session timeout in seconds.
                                       If no data is received during 
                                       this period of time, the methode
                                       returns with the error code 
                                       'SessionTimeout'. Use the
                                       constant 'NoSessionTimeout' as
                                       input if no session timeout is 
                                       required.

    @param          pData      [out] - Pointer to the received data.
                                       This pointer has to be used for 
                                       subsequent 'Send' operation. 
                                       The methode 'MaxSendDataLength'
                                       allows to determine the maximum
                                       send data length.

    @param          dataLen    [out] - Received data length.
    @param          messageList[out] - Error messages

    @return     NoError, if no error is occured.
    */
    virtual ConnRet Receive( SAPDB_UInt4          timeOut, 
                             SAPDB_Byte * &       pData,
                             SAPDB_UInt4 &        dataLen,
                             SAPDBErr_MessageList &messageList ) = 0;

    /*!
    @description    Send data to the client application.
                    
                    After receiving data via 'Receive', a pointer
                    to a transfer buffer (pData) has been returned 
                    holding the incomming data.  This buffer has to be 
                    reused to reply date to the client via 'Send'.
                    The maximum allowed data that can be written
                    into this buffer is returned by 'MaxSendDataLength'.


    @param          pData       [in] - Pointer to the send data.
                                       This pointer must be the same 
                                       pointer obtained by the prior 
                                       'Receive' call.
                                       The methode 'MaxSendDataLength'
                                       should be called to determine
                                       the maximum data length allowed
                                       to store into this buffer. 

    @param          dataLen     [in] - Received data length.
    @param          messageList[out] - Error messages

    @return     NoError, if no error is occured.
    */
    virtual ConnRet Send( SAPDB_Byte * const   pData,
                          SAPDB_UInt4 const    dataLen,
                          SAPDBErr_MessageList &messageList ) = 0;


    /*!
    @description    Close the current connection and destroy 
                    this object. Any further access to this object
                    is a fatal error!!!!!
    */
    virtual void Release()  = 0;


    /*!
    @description    Constructor
    */
    RTETask_ITaskClientConnection() : NoSessionTimeout(0) {}

    /*!
    @description    Destructor
    */
    virtual ~RTETask_ITaskClientConnection() {}
};

/*!
  @brief SAPDB_ToString ctor for conversion of RTETask_ITaskClientConnection::ConnectionType
  @param taskType [in] the connection type to convert into string
  @return SAPDB_ToString class object for output
 */
inline SAPDB_ToStringClass SAPDB_ToString(RTETask_ITaskClientConnection::ConnectionType connectType)
{
    switch (connectType)
    {
    case RTETask_ITaskClientConnection::OLTP             : return SAPDB_ToString("OLTP");
    case RTETask_ITaskClientConnection::LiveCache        : return SAPDB_ToString("LVC");
    default:                break;
    }

    return                                        SAPDB_ToString("Unknown");
}

/*!
  @brief SAPDB_ToString ctor for conversion of RTETask_ITaskClientConnection::EncryptionType
  @param taskType [in] the connection type to convert into string
  @return SAPDB_ToString class object for output
 */
inline SAPDB_ToStringClass SAPDB_ToString(RTETask_ITaskClientConnection::EncryptionType encryptType)
{
    switch (encryptType)
    {
    case RTETask_ITaskClientConnection::NoEncryption     : return SAPDB_ToString("NO");
    case RTETask_ITaskClientConnection::NISSL            : return SAPDB_ToString("SSL");
    default:                break;
    }

    return                                        SAPDB_ToString("Unknown");
}
#endif  /* RTETASK_ITASKCLIENTCONNECTION_HPP */
