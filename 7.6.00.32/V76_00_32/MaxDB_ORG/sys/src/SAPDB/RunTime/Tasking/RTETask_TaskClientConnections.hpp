/*!========================================================================

  @file         RTETask_TaskClientConnections.hpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Task related client kernel connection informations

                Class which collects all connection related methodes and 
                data that belongs to a task that is doing client kernel 
                communication.
 
                This object belongs to a task object (RTETask_Task).
                
  @since        2004-10-12  10:53
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


#ifndef RTETASK_TASKCLIENTCONNECTIONS_HPP
#define RTETASK_TASKCLIENTCONNECTIONS_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include    "RunTime/Tasking/RTETask_CommonQueueElement.hpp"
#include    "RunTime/Tasking/RTETask_CommonQueue.hpp"
#include    "RunTime/RTE_Types.hpp"
#include    "RunTime/Tasking/RTETask_DoublyLinkedList.hpp"
#include    "RunTime/Synchronisation/RTESync_Spinlock.hpp"
#include    "RunTime/RTE_MessageList.hpp"
#include    "RunTime/RTE_KGS.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define SESSION_IDENTIFIER_LEN 20

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

struct RTETask_Task;
struct RTETask_TaskClientConnection;
class  RTETask_ITaskClientConnection;

/*!---------------------------------------------------------------------------
  @class   RTETask_TaskClientConnections
  @brief   Task related client kernel connection informations

           Class which collects all connection related methodes and 
           data that belongs to a task that is doing client kernel 
           communication.

           This object belongs to a task object (RTETask_Task).
 ----------------------------------------------------------------------------*/

class RTETask_TaskClientConnections
{
public:
    /*!
    @brief        Is the task connectable?
    @return       Returns true if the task is connectable
    */
    SAPDB_Bool IsConnectable()
    {
        return m_IsConnectable;
    }

   /*! 
    @brief  Waits for a client connection
            
            A call to 'AcceptClientConnect' of the RTETask_ITask' interface
            leads to a call of this methode. This methode is used to
            join incoming client connects. The call blocks until a 
            client connect will be joined to the tasks connect request. 
            The returned client connection object is used to handle
            the new created connection.
            
    @return Connection object of a single client connection
    */
    RTETask_ITaskClientConnection * const AcceptClientConnect();


   /*! 
    @brief  Get the session timeout value 
    */
    SAPDB_ULong  GetSessionTimeOut() const
    { 
        return m_SessionTimeOut;
    };

   /*! 
    @brief  The session timeout value 
    */
    void  SetSessionTimeOut( SAPDB_ULong sessionTimeOut ) 
    { 
        m_SessionTimeOut = sessionTimeOut;
    };

    /*!
     @brief   Enqueue element into the connection request queue
               
              The connection related data of a incomming connect request
              is stored in a common queue element and is enqueued in the
              external requests queue of a TaskScheduler. The TaskScheduler
              enqueues this request in this connection request queue if the 
              requested task not already waiting for incomming connects.
              If the task is waiting for connections the request is directly
              enqueued in the run queue.

    @param          queueElement  [in] - Common queue element containing the connect
                                         request information in its data part
    */
    void EnqueueIntoConnectionRequestQueue( RTETask_CommonQueueElement &queueElement )
    { 
        return m_ConnectionRequestQueue.Enqueue(queueElement);
    }

    /*!
     @brief   Wait for connection request
               
              The connection related data of a incomming connect request
              is stored in a common queue element and is enqueued in the
              external requests queue of a TaskScheduler. The TaskScheduler
              enqueues this request in this connection request queue if the 
              requested task not already waiting for incomming connects.
              If the task is waiting for connections the request is directly
              enqueued in the run queue. 
              This methode is dealing with both situations and returns an
              incomming connect request (common queue element).

    @param          queueElemType  [in] - the expected queue element for the
                                          connection

    @return   Pointer to common queue element containing the connect request data.
    */
    RTETask_CommonQueueElement* WaitForConnectRequest( RTETask_CommonQueueElement::ElementType  queueElemType );

   /*! 
    @brief  Create a new connection object
            
            Creates a new connection object which is used for a single
            client task communication.

    @param          messageList  [out] - Error messages
        
    @return         Null, on error
                    else  Connection object for client task communication
    */
    RTETask_TaskClientConnection * const CreateConnectionObject( SAPDBErr_MessageList &messageList );

   /*! 
    @brief  Release a connection object
            
            Removes a given connection object from the connection list and
            deletes the object.

    @param          pConnection  [in] - connection object that will be dropped
    */
    void ReleaseConnectionObject( RTETask_TaskClientConnection *  pConnection );

    /*!
    @brief          Initialize object, set task this object belongs to

    @param          pTask        [in] - pointer to the task object.
    @param          isConnectable[in] - set task connectable state
    */
    void Initialize( RTETask_Task* pTask,
                     SAPDB_Bool    isConnectable );

    /*!
    @description    Update the clients task scheduler semaphore indexes

                    The load balancing mechanism allows the 
                    TaskScheduler to move a task to another 
                    TaskScheduler. Because the client needs the 
                    correct TaskScheduler semaphore to signal
                    the task a new packet arrival. The TaskScheduler
                    has to update clients task scheduler semaphore 
                    indexes after a task movement.

                    This methode is doing a unsynchronized connection list
                    access so it's only allowed to call from TaskScheduler
                    where the corresponding task is domiciled.
    */
    void UpdateClientsTaskSchedulerSemaphoreIndexes();

    /*!
    @description    Release all connections

        Methode steps through the connection list and and the connect 
        request queueand relases all open connections.
    */
    void ReleaseAllConnections();

   /*! 
    @brief  Returns a session URI string
            
            The session URI string determines the open
            sessions (connections) of a definite task.
            It's used for cancel and kill requests or 
            multiple connects to the same task. 
            To connect to the same task at least one 
            connection must be established. 
            The connect requests must contain the following 
            session URI string:

              "/database/<serverdb>/session/<sessionid>"

            Do not make any assumption to the sessionid content.

    @return         session URI string identifier
    */
    SAPDB_Char const * const SessionURIString() const
    {
        return m_SessionURIString;
    }

    /*!
    @brief          Parse session identifier

                    The session identifier determines the open
                    sessions (connections) of a definite task.
                    It's part of a session URI string which
                    has the following format:

                      "/database/<serverdb>/session/<sessionid>"

                    If the session identifier is valid this methode 
                    returns the identified task client connections.

    @param          sessionIdentifier  [in] - session identifier.
    @param          messageList       [out] - Error messages

    @return         Task client connections identified by the given
                    session identifier, null if the session identifier 
                    is not valid
    */
    static RTETask_TaskClientConnections *  const
          ParseSessionIdentifier( SAPDB_Char const * const sessionIdentifier,
                                  SAPDBErr_MessageList     &messageList );

    /*!
    @description    Remove connections where the client is lost.
    */
    static void  ReleaseOrphanedConnections();

    /*!
    @brief          Calculate a new session identfier of the session URI string.
    */
    void NewSessionURIString();

    /*!
    @brief          Constructor
    */
    RTETask_TaskClientConnections() : m_pTask(0),
                                      m_ConnectionRequestQueue( "Task connection request queue" ),
                                      m_SessionTimeOut(0),
                                      m_SessionIdentifierCounter(0),
                                      m_SessionURIString(0),
                                      m_KGS(RTE_KGS::Instance()),
                                      m_IsConnectable(false)
    {}

    /*!
    @brief          Destructor
    */
    ~RTETask_TaskClientConnections();

private:
    /*!
    @brief          Creates a new session URI string

                    This URI string can be used to identfy 
                    the connections of a task. And is used
                    for cancle and kill requests

                    The URI has the format:
                       "/database/<serverdb>/session/<sessionid>"

    @param          messageList       [out] - Error messages

    @return         true, if no error
    */
    SAPDB_Bool CreateSessionURIString( SAPDBErr_MessageList &messageList );

    /*!
    @brief          Creates a new connection identifier string in
                    'm_SessionIdentifier' 
    */
    void CreateNewSessionIdentifier();

private:
    // don't allow copies
    RTETask_TaskClientConnections( const RTETask_TaskClientConnections &c )
        : m_pTask( c.m_pTask ),
          m_ConnectionRequestQueue( "Task connection request queue" ),
          m_SessionTimeOut(0),
          m_SessionIdentifierCounter(0),
          m_SessionURIString(0),
          m_KGS(RTE_KGS::Instance()),
          m_IsConnectable(c.m_IsConnectable)
    {;}

private:
    SAPDB_UInt4                                                      m_SessionIdentifierCounter;
    SAPDB_Char                                                       m_SessionIdentifier[SESSION_IDENTIFIER_LEN + 1];
    RTETask_Task*                                                    m_pTask;
    SAPDB_ULong                                                      m_SessionTimeOut;
    RTETask_CommonQueue                                              m_ConnectionRequestQueue;
    RTETask_DoublyLinkedList<DummyLockedScopeDLList,DummyLockDLList> m_ConnectionList;
    SAPDB_Char*                                                      m_SessionURIString;
    RTE_KGS                                                          &m_KGS;            // Shortcut to kgs
    SAPDB_Bool                                                       m_IsConnectable;
};

#endif  /* RTETASK_TASKCLIENTCONNECTIONS_HPP */
