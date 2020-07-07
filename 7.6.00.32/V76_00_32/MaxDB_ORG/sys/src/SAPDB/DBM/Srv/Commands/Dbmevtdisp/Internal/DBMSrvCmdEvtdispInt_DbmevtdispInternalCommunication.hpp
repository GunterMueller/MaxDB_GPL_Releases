/*!
  \file    DBMSrvCmdEvtdispInt_DbmevtdispInternalCommunication.hpp
  \author  MarcW
  \ingroup internal DBMServer commands for event dispatcher
  \brief   

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


*/

#if !defined(DBMSrvCmdEvtdispInt_DbmevtdispInternalCommunication_hpp)
#define DBMSrvCmdEvtdispInt_DbmevtdispInternalCommunication_hpp

#include "DBM/Srv/DBMSrv_Command.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"

/*! 
    @brief Class for handling the DBMServer command dbm_getpid
*/
class DBMSrvCmdEvtdispInt_DbmevtdispInternalCommunication: public DBMSrv_Command {
public:
    /*! @brief constructor */
    DBMSrvCmdEvtdispInt_DbmevtdispInternalCommunication();

    /*! @brief destructor */
    ~DBMSrvCmdEvtdispInt_DbmevtdispInternalCommunication(){}

    /*! 
        @brief implementation of the command execution

        @param vcontrol [INOUT] the dbmserver object
        @param command [IN] command line
        @param theReply [OUT] the object to write command response into
        @return execution result status
    */
    tcn00_Error run(
            VControlDataT* vcontrol,
            CommandT* command,
            DBMSrv_Reply& theReply);

    static const char* getCommandName();

    static bool userHasPermissionToAccess( const char* lineOwner );

private:
    void listInstances(DBMSrvMsg_Error& outList, DBMSrv_Reply& theReply);
    void replyCommandToInstance( const int instanceID, DBMSrvMsg_Error& outList, DBMSrv_Reply& theReply);
    void stopInstance( const int instanceID, DBMSrvMsg_Error& outList );

    static const char* const m_LongHelp;
};

#endif //DBMSrvEvtdispInt_DbmevtdispInternalAdd_hpp
