/*!========================================================================

  @file         RTEThread_Requestor.hpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Handle incomming communication requests

                This thread class manages incomming communication 
                requests from clients application like connect, cancel
                and kill requests.
                
  @since        2003-11-06  15:58
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


#ifndef RTETHREAD_REQUESTOR_HPP
#define RTETHREAD_REQUESTOR_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include    "RunTime/RTE_KGS.hpp"
#include    "SAPDBCommon/SAPDB_Types.hpp"
#include    "RunTime/RTE_Thread_Messages.hpp"
#include    "RunTime/Threading/RTEThread_KernelThread.hpp"
#include    "RunTime/ClientKernelCommunication/RTECKC_RequestChannel.hpp"
#include    "SAPDBCommon/SAPDB_Singleton.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

class RTECKC_RequestChannel;
class RTECKC_Request;
class RTESync_BinarySemaphore;

/*!---------------------------------------------------------------------------
  @class   RTEThread_Requestor
  @brief   Handle incomming communication requests

           This thread class manages incomming communication 
           requests from client applications like connect, cancel
           and kill requests.
 ----------------------------------------------------------------------------*/

class RTEThread_Requestor : public RTEThread_KernelThread
{
public:
  /*!
    @description    Return reference to single instance of RTEThread_Requestor
    @return         Reference to RTEThread_Requestor instance
   */
   static RTEThread_Requestor & Instance( );

private:
    /*!
       @brief          Constructor
     */
    RTEThread_Requestor() : RTEThread_KernelThread("Requestor"),
                            m_KGS(RTE_KGS::Instance())  {}

    /*!
    @description    The created thread runs the code of the methode 'ThreadMain'
    */
    virtual SAPDB_Int4  KernelThreadMain();

    /*!
    @description    Create the requestors IPC resources used 
                    for incomming requests
    */
    void CreateIPCResources();

    /*!
    @description    Handle cancel request
    */
    void HandleCancelRequest( RTECKC_Request*     pRequest );

    /*!
    @description    Handle connect request
    */
    void HandleConnectRequest( RTECKC_Request*          pRequest );

    /*!
    @description    Send a established connection object to the corresponding
                    task.

                    A new established connection to a client application is 
                    send to the task that is waiting for a incomming connect
                    request.
                    
    @param          pConnection    [in] - Pointer to the connection object holding
                                          all information of a already established 
                                          client connection. The object is ready to
                                          use for task client/task communication. 
    @param          pQueueElement  [in] - Queue element to transport connect request
    */
    void SendConnectionToTask( RTETask_TaskClientConnection* pConnection,
                               RTETask_CommonQueueElement*   pQueueElement );

  /*!
    @description    Destroy instance pointer
    */
   virtual void  DestroyObject() { m_Instance = 0; };

private:
    RTE_KGS                    &m_KGS;              // Shortcut to kgs
    RTECKC_RequestChannel       m_requestChannel;

    SAPDB_Singleton             m_ThisIsASingleton; // prevents copy and assignment
    static RTEThread_Requestor *m_Instance;
};


#endif  /* RTETHREAD_REQUESTOR_HPP */
