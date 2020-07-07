/*!
  @file           RTEThread_ConsoleWorkerBase.hpp
  @author         StefanP
  @special area   Kernel Console Thread
  @brief          Base Class for the Request Processing
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



#ifndef RTETHREAD_CONSOLEWORKERBASE_HPP
#define RTETHREAD_CONSOLEWORKERBASE_HPP

#if (defined (LINUX) && !defined(HAS_NPTL) && !defined(HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE)) || defined (SUN) || defined(SOLARIS)
#define HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE
#endif

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include    "geo003.h"
#include    "heo07.h"
#include    "RunTime/RTE_ConsoleCommon.hpp"
#include    "RunTime/RTE_ConsoleStandardDataRequests.h"
#include    "RunTime/RTE_ConsoleDataCommunication.hpp"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include    "RunTime/System/RTESys_Time.h"



extern SAPDBTrace_Topic Console_Trace; ///< global console trace object

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
  @class          RTEThread_ConsoleWorkerBase
   @description    Abstract class for request processing


                This class delivers some interfaces for the request processing.
                A specific class for request processing can be derived from these
                abstract class by implementing theses interfaces.
                For each protocol on the data level a subclass has to derived from
                this class which implements the specific request processing.

 */


class RTEThread_ConsoleWorkerBase
{
public:
  /*!
     @description   Constructor
                    Calls the constructor of RTE_ConsoleDataCommunication and
                    initializes the connect and the last access time

     @param         openMode    [in] - RTE_CONSOLE_CLIENT/SERVER_MODE
   */

  
    RTEThread_ConsoleWorkerBase (SAPDB_UInt4 const              openMode )
    :   m_CommPort         (openMode),
        m_ConnectTime     (RTESys_Time ()), 
        m_Initialized     (false),
        m_RequestPending  (false),
        m_lastRequestTime (m_ConnectTime),
        m_RemotePID       (0)
        {
            SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleWorkerBase::RTEThread_ConsoleWorkerBase", 
                                     Console_Trace, 9);
            sqlcreatemutex(&m_LockMutex);
        }

  /*!
     @description    Destructor

   */


     virtual ~RTEThread_ConsoleWorkerBase () 
        {
            SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleWorkerBase::~RTEThread_ConsoleWorkerBase", 
                                     Console_Trace, 9);
            sqldestroymutex(&m_LockMutex);
         }

  /*!
     @description    Initialize the RTEThread_ConsoleWorkerBase instance.


                  By the initialization of the RTEThread_ConsoleWorkerBase the 
                  initialization of the data communication is taken place (attaching
                  semaphores and shared memory)
     @param          shmID       [in]     - ID for the shared communication segment (shared memory)
     @param          remotePID   [in]     - Process ID of the client
     @param          myRef       [in]     - Own reference (= connection slot number)
     @param          remoteRef   [in]     - Remote (XCons) reference
     @param          messageList [in/out] - Message list
     @return value   true/false

   */


    SAPDB_Bool          Initialize          (RTE_UniqueIdBuffer const           shmID,
                                             RTE_OSPid   const                  remotePID,
                                             SAPDB_UInt4 const                  myRef,
                                             SAPDB_UInt4 const                  remoteRef,
                                             SAPDBErr_MessageList &             messageList);

  /*!
     @description    Process a connect request

     @param          hConnect [in] - Unique handle of the created connection.
                                     To be send to the client for later connects.
     @param          messageList [in/out] - Message list
     @return         RTE_CONS_NO_ERROR/RTE_CONS_ERROR/RTE_CONS_TIMEOUT/RTE_CONS_REMOTE_TIMEOUT
   */


    virtual SAPDB_UInt4 ConnectRequest      ( RTE_ConsoleHandle const            hConnect,
                                              SAPDBErr_MessageList &             messageList )
    {
        return Connect( hConnect, messageList );
    }

  /*!
     @description    Reject a connect request
     @param          sendMessageList [in/out] - Message list to be send

   */
    virtual void        RejectConnectRequest ( SAPDBErr_MessageList&        sendMessageList )
    {
        return ConnectReject( sendMessageList );
    }

  /*!
     @description    Process all requests

                     This method processes all requests from the client side beside the connect
                     and disconnect.

     @param          messageList [in/out] - Message list
     @return         RTE_CONS_NO_ERROR/RTE_CONS_ERROR/RTE_CONS_TIMEOUT/RTE_CONS_REMOTE_TIMEOUT

   */
    virtual SAPDB_UInt4 ProcessRequest  ( SAPDBErr_MessageList &             messageList )
    {
        LockRequestProcessing();
        m_lastRequestTime = RTESys_Time ();
        m_RequestPending = true;

        SAPDB_UInt4 rc = RequestProcessing ( messageList );

        m_RequestPending = false;
        UnlockRequestProcessing();

        return rc;
    }


  /*!
     @description    Process a disconnect request

     @param       messageList [in/out] - Message list
     @return      RTE_CONS_NO_ERROR/RTE_CONS_ERROR/RTE_CONS_TIMEOUT/RTE_CONS_REMOTE_TIMEOUT
   */

    virtual SAPDB_UInt4 DisconnectRequest   (SAPDBErr_MessageList &             messageList)
    {
        LockRequestProcessing();
        SAPDB_UInt4 rc = Disconnect( messageList );
        UnlockRequestProcessing();

        return rc;
    }


  /*!
     @description    Returns the ID of the peer process
     @return value   see above
   */

    RTE_OSPid  GetRemotePID() { return m_RemotePID; }

  /*!
     @description    Returns the initial connect time (open connect time) 
                for this connection as seconds since 1.1.1990
     @return value   see above

   */


    SAPDB_UInt8 const   GetConsoleConnectTime       () const 
                            {return m_ConnectTime;}

  /*!
     @description    Returns the last connect (access) time for this connection 
                as seconds since 1.1.1990
     @return value   see above

   */

    SAPDB_UInt8         GetLastRequestProcessingTime    () const   
                            {return m_lastRequestTime;}

  /*!
     @description    Is the worker currently processing a request.
     @return value   True if the wroker is busy
   */
    SAPDB_Bool          IsRequestPendig () { return m_RequestPending; }

  /*!
     @description    Get the actual busy state
     @return value   Busy state (true/false)

   */
    SAPDB_Bool          IsInitialized    () const
                            {return m_Initialized;}


#if defined (HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE)
  /*!
     @description    Set unique index.  for the timeout list
     @return value   
  */

    void                SetThreadIndex (SAPDB_UInt4 const               index)
                            {m_CommPort.SetIndex (index);}
#endif

  /*!
     @description    Supplies the client reference (reference needed for the RTE Header)
     @return value   Client reference

   */

    SAPDB_UInt4 GetClientReference  () const    
        {return m_CommPort.GetRemoteRef ();}

  /*!
     @description    Supplies the server reference (reference needed for the RTE Header)
     @return value   Server reference

   */

    SAPDB_UInt4 GetServerReference  () const    
        {return m_CommPort.GetMyRef ();}

  /*!
     @description    Send the message list to the client if a error has occured during 
                processing a request.


                  The responds type 'ALL_REP' is intended to the situation where the
                  request is not known anymore (e.g. in case of a timeout).
                  Has to be implemented by a subclass (protocol specific).
     @param          respType [in] - responds type (DATA_REP, OPEN_REP, CLOSE_REP, ALL_REP)
     @param          errCode [in] - Error code (RTE_CONS_ERROR, RTE_CONS_FATAL_ERROR, RTE_CONS_TIMEOUT)
     @param          messageListToSend [in/out] - Message list to be send. If the function
                                             fails (rc = false) the derived error messages are
                                             added to this message list.
     @return value   true
                false:  see above

   */


    virtual SAPDB_Bool SendError           (SAPDB_UInt1 const                  respType,
                                            SAPDB_UInt4 const                  errCode,
                                            SAPDBErr_MessageList &             messageListToSend) = 0;


protected:

  /*!
     @description    Process a connect request


                  Has to be implemented by a subclass (protocol specific).
     @param          hConnect [in] - Unique handle of the created connection.
                                       To be send to the client for later connects.
     @param          messageList [in/out] - Message list
     @return         RTE_CONS_NO_ERROR/RTE_CONS_ERROR/RTE_CONS_TIMEOUT/RTE_CONS_REMOTE_TIMEOUT


   */


    virtual SAPDB_UInt4 Connect             (RTE_ConsoleHandle const            hConnect,
                                             SAPDBErr_MessageList &             messageList) = 0;


  /*!
     @description    Reject a connect request
     @param          sendMessageList [in/out] - Message list to be send

   */
    virtual void        ConnectReject       ( SAPDBErr_MessageList&        sendMessageList ) = 0;

  /*!
     @description    Main method for all requests


                  This method processes all requests from the client side beside the connect
                  and disconnect.
                  Has to be implemented by a subclass (protocol specific).
     @param       messageList [in/out] - Message list
     @return      RTE_CONS_NO_ERROR/RTE_CONS_ERROR/RTE_CONS_TIMEOUT/RTE_CONS_REMOTE_TIMEOUT

   */


    virtual SAPDB_UInt4 RequestProcessing   (SAPDBErr_MessageList &             messageList) = 0; 


  /*!
     @description    Process a disconnect request


                  Has to be implemented by a subclass (protocol specific).
     @param       messageList [in/out] - Message list
     @return      RTE_CONS_NO_ERROR/RTE_CONS_ERROR/RTE_CONS_TIMEOUT/RTE_CONS_REMOTE_TIMEOUT

   */

    virtual SAPDB_UInt4 Disconnect          (SAPDBErr_MessageList &             messageList) = 0;


  /*!
     @description    Wait for a confirmation to the release
     @return         RTE_CONS_NO_ERROR/RTE_CONS_ERROR/RTE_CONS_TIMEOUT
   */
        
    SAPDB_UInt4     WaitForReleaseConfirmation      (SAPDBErr_MessageList &         messageList) const
        {return m_CommPort.WaitForConfirmation (messageList);}

  /*!
     @description    Receive a confirmation from the client side that the release 
                     informations from the server side were received.
                     (This is done for synchronisation reasons because the server is releasing
                     the semaphore and shared memory resources.)
     @return value   true/false

   */

//    SAPDB_Bool          ReceiveReleaseConfirmation      ();


  /*!
     @description    Sending data to the remote side.


                  SendReplyData
                  The functions handles all the work for data transport by using the RTE Header.
                  If it necessary the data are send in several packages.
                  SendReplyDataNext
                  has to be used to send subsequent data after 'SendReplyData'. This may be necessary 
                  if we would need very large buffer to hold all the data at once.
     @param          pData [in] - Data to be send
     @param          size [in] - Size of data in bytes
     @param          respType [in] - Responds type (OPEN_REP, DATA_REP, CLOSE_REP, ALL_REP)
     @param          messageList [in/out] - Message list
     @return value   RTE_CONS_NO_ERROR/RTE_CONS_ERROR/RTE_CONS_TIMEOUT

   */

    SAPDB_UInt4 SendReplyData       (SAPDB_Byte const * const       pData,
                                     SAPDB_UInt4 const              size,
                                     SAPDB_UInt1 const              respType,
                                     SAPDBErr_MessageList &         messageList)
        {return m_CommPort.SendData (pData, size, respType, messageList);}


    SAPDB_UInt4 SendReplyDataNext   (SAPDB_Byte const * const       pData,
                                     SAPDB_UInt4 const              size,
                                     SAPDB_UInt1 const              respType,
                                     SAPDBErr_MessageList &         messageList)
        {return m_CommPort.SendDataNext (pData, size, respType, messageList);}


/*!
   @description    Getting data from the remote side.


                  GetRequestData
                  This function has always to be called first to receive data from the 
                  remote side send by 'SendReplyData'. If the send data do not fit in the buffer 
                  consecutive data can be received by calling
                  GetRequestDataContinue
                  This function can be called as often as necessary to get all the data
                  which were send by the remote side.
                  GetRequestDataNext
                  has to be called if the remote side was sending further data by calling 
                  'SendReplyDataNext'. As 'GetData' this function has to be called only once 
                  followed by subsequent calls to 'GetRequestDataContinue' (if necessary).
   @param          pBuffer [in] - Buffer for the data to be read
   @param          expectedType [in] - expected request/ respond type (OPEN_REQ, OPEN_REP,...)
   @param          bufferSize [in] - Size of buffer in bytes
   @param          numBytesRead [out] - number of bytes read
   @param          numBytesTotal [out] - total number of bytes send by the remote side
   @param          messageList [in/out] - Message list
   @return         RTE_CONS_NO_ERROR/RTE_CONS_ERROR/RTE_CONS_TIMEOUT


 */


    SAPDB_UInt4 GetRequestData      (SAPDB_Byte * const             pBuffer,
                                     SAPDB_UInt1 const              expectedType,
                                     SAPDB_UInt4 const              bufferSize,
                                     SAPDB_UInt4 &                  numBytesRead,
                                     SAPDB_UInt4 &                  numBytesTotal,
                                     SAPDBErr_MessageList &         messageList)
        {return m_CommPort.GetData (pBuffer, expectedType, bufferSize, numBytesRead, 
                                     numBytesTotal, messageList);}
  

    SAPDB_UInt4 GetRequestDataNext  (SAPDB_Byte * const             pBuffer,
                                     SAPDB_UInt1 const              expectedType,
                                     SAPDB_UInt4 const              bufferSize,
                                     SAPDB_UInt4 &                  numBytesRead,
                                     SAPDB_UInt4 &                  numBytesTotal,
                                     SAPDBErr_MessageList &         messageList)
        {return m_CommPort.GetDataNext (pBuffer, expectedType, bufferSize, numBytesRead, 
                                        numBytesTotal, messageList);}


    SAPDB_UInt4 GetRequestDataContinue (SAPDB_Byte * const             pBuffer,
                                        SAPDB_UInt1 const              expectedType,
                                        SAPDB_UInt4 const              bufferSize,
                                        SAPDB_UInt4 &                  numBytesRead,
                                        SAPDB_UInt4 &                  numBytesTotal,
                                        SAPDBErr_MessageList &         messageList)
        {return m_CommPort.GetDataContinue (pBuffer, expectedType, bufferSize, numBytesRead, 
                                            numBytesTotal, messageList);}



  /*!
     @description    Lock the console worker.
   */
   inline  void        LockRequestProcessing();

  /*!
     @description    Unlock the console worker.
   */
   inline  void        UnlockRequestProcessing();

private:
    RTE_ConsoleDataCommunication        m_CommPort;
    SAPDB_UInt8     const               m_ConnectTime;
    SAPDB_UInt8                         m_lastRequestTime;
    SAPDB_Bool                          m_Initialized;
    SAPDB_Bool                          m_RequestPending;
    SAPDB_UInt4                         m_TimeOutIndex;
    RTE_OSPid                           m_RemotePID;
    teo07_Mutex                         m_LockMutex;
};

/*---------------------------------------------------------------------------*/

inline  void    RTEThread_ConsoleWorkerBase::LockRequestProcessing (void)
{
    SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleWorkerBase::LockRequestProcessing", 
                             Console_Trace, 9);
    sqlbeginmutex(&m_LockMutex);
}

/*---------------------------------------------------------------------------*/

inline  void    RTEThread_ConsoleWorkerBase::UnlockRequestProcessing (void)
{
    SAPDBTRACE_METHOD_DEBUG ("RTEThread_ConsoleWorkerBase::UnlockRequestProcessing", 
                             Console_Trace, 9);
    sqlendmutex(&m_LockMutex);
}


#endif  /* RTETHREAD_CONSOLEWORKERBASE_HPP */
