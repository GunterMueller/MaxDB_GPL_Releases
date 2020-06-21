/*!
  @file           DBMSrvKnl_GlobalSessions.hpp
  @author         Bernd Vorsprach - bernd.vorsprach@sap.com
  @brief          DBMServer to Kernel Communication - Specification

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
*/

#ifndef _DBMSrvKnl_GlobalSessions_HPP_
#define _DBMSrvKnl_GlobalSessions_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
#include "SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_Session.hpp"

/*! @brief Global sessions container
    This object stores all the global DBMServer session to the kernel. */
class DBMSrvKnl_GlobalSessions {
  public:
    typedef enum {
      SessionMinValue,
      SessionDatabase,
      SessionSQL,
      SessionInfo,
      SessionBackup,
      SessionEvent,
      SessionService,
      SessionMaxValue
    } SessionType;

    /*! @brief  Creates and stores a session with the kernel.
        @param  oType      [IN]  - type of requested session
        @param  szDatabase [IN]  - name of database
        @param  szUser     [IN]  - name of user
        @param  szPassword [IN]  - password of user
        @param  oError     [OUT] - message list object for error information
        @return Pointer to the session object, null pointer in case of errors */
    static DBMSrvKnl_Session * CreateSession
      ( SessionType       oType,
        const char      * szDatabase,
        const char      * szUser,
        const char      * szPassword,
        DBMSrvMsg_Error & oError );

    /*! @brief  Creates and stores a session with the kernel.
        @param  oType      [IN]  - type of requested session
        @param  szDatabase [IN]  - name of database
        @param  szConnect  [IN]  - connect data (user, user typeor nothing)
        @param  oError     [OUT] - message list object for error information
        @return Pointer to the session object, null pointer in case of errors */
    static DBMSrvKnl_Session * CreateSession
      ( SessionType       oType,
        const char      * szDatabase,
        const char      * szConnect,
        DBMSrvMsg_Error & oError );

    /*! @brief  Creates and stores a session with the kernel.
        @param  oType      [IN]  - type of requested session
        @param  szDatabase [IN]  - name of database
        @param  oError     [OUT] - message list object for error information
        @return Pointer to the session object, null pointer in case of errors */
    static DBMSrvKnl_Session * CreateSession
      ( SessionType       oType,
        const char      * szDatabase,
        DBMSrvMsg_Error & oError );

    /*! @brief  Gets an existing session object or creates a new one
        @param  oType      [IN]  - type of requested session
        @param  szDatabase [IN]  - name of database (will be ignored if sessions exists)
        @param  szUser     [IN]  - name of user (will be ignored if sessions exists)
        @param  szPassword [IN]  - password of user (will be ignored if sessions exists)
        @param  oError     [OUT] - message list object for error information
        @return Pointer to the session object, null pointer in case of errors */
    static DBMSrvKnl_Session * GetOrCreateSession
      ( SessionType       oType,
        const char      * szDatabase,
        const char      * szUser,
        const char      * szPassword,
        DBMSrvMsg_Error & oError );

    /*! @brief  Gets an existing session object or creates a new one
        @param  oType      [IN]  - type of requested session
        @param  szDatabase [IN]  - name of database (will be ignored if sessions exists)
        @param  szConnect  [IN]  - connect data (user, user typeor nothing)
                                   (will be ignored if sessions exists)
        @param  oError     [OUT] - message list object for error information
        @return Pointer to the session object, null pointer in case of errors */
    static DBMSrvKnl_Session * GetOrCreateSession
      ( SessionType       oType,
        const char      * szDatabase,
        const char      * szConnect,
        DBMSrvMsg_Error & oError );

    /*! @brief  Gets an existing session object or creates a new one
        @param  oType      [IN]  - type of requested session
        @param  szDatabase [IN]  - name of database (will be ignored if sessions exists)
        @param  oError     [OUT] - message list object for error information
        @return Pointer to the session object, null pointer in case of errors */
    static DBMSrvKnl_Session * GetOrCreateSession
      ( SessionType       oType,
        const char      * szDatabase,
        DBMSrvMsg_Error & oError );

    /*! @brief  Get an existing session object.
        @param  oType      [IN]  - type of requested session
        @return Pointer to the session object, or null if not exists */
    static DBMSrvKnl_Session * GetSession
      ( SessionType oType );

    /*! @brief  Release a session.
        @param  oType      [IN]  - type of requested session
        @param  oError     [OUT] - message list object for error information
        \param  keepSessionObject [IN] if true the session is released, but the corresponding object is not destroyed */
    static void ReleaseSession
        ( SessionType oType,
          DBMSrvMsg_Error & oError,
          bool              keepSessionObject=false);

    /*! @brief  Release all sessions.
    
        \param oError  [OUT] object containing error messages
        \param keepDBSessionObjectAndServiceSession [IN]  set to true if db and
                   service session object should be destroyed */
    static void ReleaseAll
        ( DBMSrvMsg_Error & oError,
          bool              keepDBSessionObjectAndServiceSession=false);

    class KernelSessionPointer {
      public:
        KernelSessionPointer() {m_pPointer = NULL;}
        DBMSrvKnl_Session * m_pPointer;
    }; // end class KernelSessionPointer

  private:
    static KernelSessionPointer m_oSessions[SessionMaxValue];

}; // end class DBMSrvKnl_GlobalSessions

#endif /* _DBMSrvKnl_GlobalSessions_HPP_ */

