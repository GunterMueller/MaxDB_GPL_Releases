/*!========================================================================

  @file         RTEThread_ConsoleConnections.hpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Console connection object

                Handles all console connections

  @since        2005-01-10  13:40
  @sa           

  ==========================================================================

  \if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2004-2005 SAP AG

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


#ifndef RTETHREAD_CONSOLECONNECTIONS_HPP
#define RTETHREAD_CONSOLECONNECTIONS_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp" 
#include "RunTime/Threading/RTEThread_ConsoleWorkerBase.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/SAPDB_Singleton.hpp"
#include "RunTime/RTE_HandleManager.hpp"
#include "RunTime/RTE_MessageList.hpp"
#include "RunTime/Synchronisation/RTESync_InterlockedCounter.hpp"
#include "RunTime/Threading/RTEThread_ConsoleCommand.hpp"


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
  @class          RTEThread_ConsoleConnectionListItem
   @description    List item (connection slot) of the connection list


                Instances of that class administer a console connection and hold all
                data necessary for that. This includes a pointer to a suitable
                worker instance (request processing, data communication).

 */


class RTEThread_ConsoleConnections
{
public:
    /*!
      @description    Return reference to single instance of RTEThread_ConsoleConnections
      @return value   Reference to RTEThread_ConsoleConnections instance
    */
    static  RTEThread_ConsoleConnections&   Instance ();

    /*!
      @brief          Destructor
    */
    ~RTEThread_ConsoleConnections();

    /*!
      @description    Create a new connection to the client console.

      @param     command[in]             - Reference to request send by XCons 
                                           (-> Pipe -> Console Thread -> Command Queue -> Worker Thread)
      @param     threadIndex [in]        - Index of the current worker thread
      @param     messageList [in/out]    - Message list

      @return     RTE_CONS_NO_ERROR:      Connection was opend successfully
                  RTE_CONS_ERROR:         Open failed but the connection list is still consistent
                                          -> No reason for termination
                  RTE_CONS_FATAL_ERROR:   Inconsistency 
                                          -> Abort should be performed
    */
    SAPDB_UInt4     OpenConnection      ( RTEThread_ConsoleCommand                       &command,
                                          SAPDB_UInt4 const                              threadIndex,
                                          SAPDBErr_MessageList &                         messageList );


    /*!
      @description    Execute a console request

      @param     command[in]             - Reference to request send by XCons 
      @param     threadIndex [in]        - Index of the current worker thread
      @param     messageList [in/out]    - Message list

      @return     RTE_CONS_NO_ERROR:      Request was handled successfully
                  RTE_CONS_ERROR:         Request error
    */
    SAPDB_UInt4 RequestProcessing( RTEThread_ConsoleCommand    &command,
                                   SAPDB_UInt4 const           threadIndex,
                                   SAPDBErr_MessageList&       messageList );

   /*!
     @description    Close a connection to the client console.

     @param          command[in]          - Reference to request send by XCons 
     @param          threadIndex [in]     - Index of the current worker thread
     @param          messageList [in/out] - Message list

     @return value   RTE_CONS_NO_ERROR:   Release succeeded
                     RTE_CONS_ERROR:      Release failed
    */
    SAPDB_UInt4     CloseConnection   ( RTEThread_ConsoleCommand                       &command,
                                        SAPDB_UInt4 const                              threadIndex,
                                        SAPDBErr_MessageList &                         messageList );

    /*!
      @brief          Get number of open connections
      @return         Number of connections
      */
    SAPDB_UInt4    GetNumConnections    () const    { return m_NumConnections.GetValue(); }


    /*!
     @description    Determine existing connections which were not used for a special period
                     of time (MAX_CONSOLE_IDLE_TIME). Those connection are released.
    */
    void   ScanConnectionsForTimeout ();


private:
    /*!
      @brief          Create a console worker
      */
    SAPDB_UInt4 CreateWorker( RTEThread_ConsoleCommand     &command,
                              RTEThread_ConsoleWorkerBase* &pWorker,
                              SAPDBErr_MessageList&        messageList );

    /*!
      @brief          Release a connect worker
      */
    SAPDB_UInt4 ReleaseWorker ( RTEThread_ConsoleWorkerBase*   pWorker,
                                SAPDBErr_MessageList*          pMessageList = 0 );


    /*!
      @brief          Reject an open connection request
      */
    void        OpenReject ( RTEThread_ConsoleCommand       &command,
                             RTE_HandleManager::Hdl         hdl,
                             RTEThread_ConsoleWorkerBase*   pWorker,
                             SAPDBErr_MessageList&          sendMessageList );

    /*!
      @brief          Constructor

      The default constructor is privat to this class to avoid instantiation outside of the class.
      */
    RTEThread_ConsoleConnections();

    SAPDB_Singleton m_ThisIsASingleton; // prevents copy and assignment

    // Using a static reference would mean, that initialization function is called before main
    // This we have to prevent for the same reason we prevent static class instances
    // -> dynamic linked RunTime would not get initialized correctly on some OS...
    static RTEThread_ConsoleConnections *m_Instance;

private:
    RTE_HandleManager                       m_Connections;
    RTESync_InterlockedCounter<SAPDB_UInt4> m_NumConnections;

};


#endif  /* RTETHREAD_CONSOLECONNECTIONS_HPP */
