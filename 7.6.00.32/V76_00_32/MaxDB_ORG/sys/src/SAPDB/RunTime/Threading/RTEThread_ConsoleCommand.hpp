/*!========================================================================

  @file         RTEThread_ConsoleCommand.hpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Command element used for communication between 
                console thread and worker threads

  @since        2004-12-16  14:39
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


#ifndef RTETHREAD_CONSOLECOMMAND_HPP
#define RTETHREAD_CONSOLECOMMAND_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/RTE_Types.h"
#include "RunTime/RTE_ConsoleCommon.hpp"
#include "RunTime/Container/RTECont_DoublyLinkedListElement.hpp"

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
  @class   RTEThread_ConsoleCommand
  @brief   Console command

           Class is the container for the console command to be handled by
           the console worker thread.
 ----------------------------------------------------------------------------*/

class RTEThread_ConsoleCommand : protected RTECont_DoublyLinkedListElement
{
public:
    /*!
       @brief          Constructor
       @param          reqType     [in] - Request type like 'open request', 'close request' etc.
       @param          senderRef   [in] - Sender specific identifier
       @param          receiverRef [in] - Identifier of the receiver 'kernel'
       @param          data        [in] - Data container
     */
    RTEThread_ConsoleCommand ( SAPDB_UInt1                 reqType,
                               SAPDB_UInt4                 senderRef,
                               SAPDB_UInt4                 receiverRef,
                               RTE_ConsoleRequestData      &data )
                               : m_ReqType(reqType),
                                 m_SenderRef(senderRef),
                                 m_ReceiverRef(receiverRef),
                                 m_Data(data) {}

    /*!
       @brief          Constructor
     */
    RTEThread_ConsoleCommand () : m_ReqType(0),
                                  m_SenderRef(0),
                                  m_ReceiverRef(0) {}

    /*!
       @brief          Get request type
       @return value   request type
    */
    SAPDB_UInt1                 ReqType()     { return m_ReqType; }

    /*!
       @brief          Get sender specific identifier
       @return value   Sender specific identifier
    */
    SAPDB_UInt4                 SenderRef()   { return m_SenderRef; }

    /*!
       @brief          Get Identifier of the receiver 'kernel'
       @return value   Identifier of the receiver 'kernel'
    */
    SAPDB_UInt4                 ReceiverRef() { return m_ReceiverRef; }

    /*!
       @brief          Get access to the data container
       @return value   Data container
    */
    RTE_ConsoleRequestData&     Data()        { return m_Data; }


private:
    friend class RTEThread_ConsoleWorkerThread;

    SAPDB_UInt1                 m_ReqType;
    SAPDB_UInt4                 m_SenderRef;
    SAPDB_UInt4                 m_ReceiverRef;
    RTE_ConsoleRequestData      m_Data;
};


#endif  /* RTETHREAD_CONSOLECOMMAND_HPP */
