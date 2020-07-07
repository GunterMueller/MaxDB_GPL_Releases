/*!
  @file           RTEThread_ConsoleRequestor.hpp
  @author         StefanP
  @special area   Request Communication XCons - Console Thread
  @brief          Singleton on Kernel side
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



#ifndef RTETHREAD_CONSOLEREQUESTOR_HPP
#define RTETHREAD_CONSOLEREQUESTOR_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include    "RunTime/RTE_ConsoleRequestCommunication.hpp"
#include    "RunTime/Threading/RTEThread_ConsoleCommand.hpp"

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
  @class          RTEThread_ConsoleRequestor
   @description    Request communication (Singleton)


                This class is derived from 'RTE_ConsoleRequestCommunication'
                as a protocol layer for the console requests send from XCons (client)
                to the console thread (server).
                The protocol handling is based on the RTE header. 

 */


class RTEThread_ConsoleRequestor : private RTE_ConsoleRequestCommunication
{

public:

  /*!
     @description    Returns reference to single instance of RTEThread_ConsoleRequestor
     @return value   Reference to RTEThread_ConsoleRequestor instance

   */

    static  RTEThread_ConsoleRequestor    &   Instance ();

  /*!
     @description    Destructor
     @return value   none

   */


    ~RTEThread_ConsoleRequestor     ()  {;}


  /*!
     @description    Read a request from FIFO.


                  The call is blocking. That means it wait until a request is available.
     @param          request [out] - Reference to a Console Request
     @param          messageList [in/out] - Message list
     @return value   true
                false: Error on getting request or protocol error

   */


    SAPDB_Bool  ReadCommand         (RTEThread_ConsoleCommand &             command,
                                     SAPDBErr_MessageList &                 messageList);

private:

/*!
   @brief          Default constructor
   @param          pszServerDB [in] - Name of the DB

The default constructor is privat to this class to avoid instantiation outside of the class.

 */

    RTEThread_ConsoleRequestor      (SAPDB_Char const * const               pszServerDB) 
        :  RTE_ConsoleRequestCommunication (pszServerDB, RTE_CONSOLE_SERVER_MODE),
           m_header (&m_request.header) {;}


    static  RTEThread_ConsoleRequestor         *m_Instance;

            RTE_ConsoleRequest                  m_request;
            RTEComm_PacketHeader                m_header;
};



#endif  /* RTETHREAD_CONSOLEREQUESTOR_HPP */
