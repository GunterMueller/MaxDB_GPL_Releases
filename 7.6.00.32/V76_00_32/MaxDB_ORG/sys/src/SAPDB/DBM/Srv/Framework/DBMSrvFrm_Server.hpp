/*!
  \file    DBMSrvFrm_Server.hpp
  \author  MarcW
  \ingroup DBMServer framework
  \brief   the representation of the real world's server

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

#if !defined(DBMSrvFrm_Server_hpp)
#define DBMSrvFrm_Server_hpp

#include "DBM/Srv/Framework/DBMSrvFrm_ExceptionHandler.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"
#include "RunTime/System/RTESys_Time.h"

/*! 
    @brief representation of the real world's server

    Of this class, only one instance should exist (per process). This instance should be the only
    static variable in the entire process. It lives on the stack.
*/
class DBMSrvFrm_Server {

public:
    static DBMSrvFrm_Server& Instance() {
        return m_Instance;
    }

    // no need to report errors
    bool start(bool dbmcliMode);
    
    // report errors to return them to client
    bool stop( DBMSrvMsg_Error& errOut );

#if defined( _WIN32 )
    int handleException( unsigned int exCode, struct _EXCEPTION_POINTERS *ep ) {
        return m_ExceptionHandler.dumpInformation(exCode, ep);
    }
#else
    void handleException( uint signalCode ) {
        m_ExceptionHandler.dumpInformation(signalCode);
    }
#endif

    /*! @brief handle error in communication with client */
    void handleCommunicationError(tsp01_CommErr errorCode);

    // place here getter/setter methods of global pieces of information
    // * database name
    // * current user
    // * an object that contains all open sessions to kernel
    // * ...

private:
    DBMSrvFrm_Server();
    ~DBMSrvFrm_Server() {}

    // this method takes care that this is not logged more than once
    void logConnect();
    void logDisconnect();

    RTESys_Timestamp m_CtrlservopenTime;


    // required for access to logConnect from vcn10, legacy
    friend void cn10_LogError( const DBMSrvMsg_Error& listToLog );
    friend void cn10_LogCommand( CommandT   * command );

    static DBMSrvFrm_Server m_Instance;

    bool m_Started;

    DBMSrvFrm_ExceptionHandler m_ExceptionHandler;

public:
    // legacy
    DBMSrvMsg_Error m_CtrlservopenError;
    bool m_LoggedConnect;

};

#endif //DBMSrvFrm_Server_hpp
