/*!
  \file    DBMSrvCmdDbm_DbmExecuteServerprocess.hpp
  \author  MarcW
  \ingroup DBMServer commands
  \brief   declaration of class for handling DBMServer
           command dbm_execute_serverprocess

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

#include "DBM/Srv/DBMSrv_Command.hpp"
#include "DBM/Srv/Process/DBMSrvProc_ServerProcessCommandLineConverter.hpp"

#if !defined(DBMSrvCmdDbm_DbmExecuteServerprocess_hpp)
#define DBMSrvCmdDbm_DbmExecuteServerprocess_hpp

/*! 
    @brief command class for executing server processes

    To be used only in an "intermediate" DBM server, that behaves like a
    client. The DBM server, that serves the client request, instanciates this
    DBM server through the class DBMSrvProc_ServerProcess using the x-server. By
    doing so, the child processes, that are spawned from this command, behave like
    services on Windows systems. Only selected processes can be started. The executable
    is coded as a number, which is decoded in this class with the help of a
    DBMSrvProc_ServerProcessCommandLineConverter.

    If DBMSrvProc_ServerProcess detects that it has already been started via the x-server
    (remote communication to its client program), then this command is called directly
    within the same DBM server, no "intermediate" DBM server is created. In this case, not
    the run() method is called but the method runInProcess().

    @see DBMSrvProc_ServerProcessCommandLineConverter
    @see DBMSrvProc_ServerProcess
*/
class DBMSrvCmdDbm_DbmExecuteServerprocess: public DBMSrv_Command {
public:
    /*! @brief constructor */
    DBMSrvCmdDbm_DbmExecuteServerprocess();

    /*! 
        @brief the actual implementation of the command execution

        Since this command is not intended to be run by a user, there is no help.

        @param vcontrol [INOUT] the dbmserver object
        @param command [IN] command line
        @param theReply [OUT] the object to write command response into
        @return execution result status
    */
    tcn00_Error run(VControlDataT * vcontrol,
        CommandT      * command,
        DBMSrv_Reply& theReply);

    /*!
        @brief the "in process" run method

        This method is called from DBMSrvProc_ServerProcess, if the DBM server
        has a remote communication to its client.
        @param aCommandLine [IN] the command line to be executed.
        @param aMessageList [OUT] errors are reported using this list
        @see DBMSrvProc_ServerProcess
    */
    void runInProcess(const char* aCommandLine,
        Msg_List& aMessageList);
};
#endif //DBMSrvCmdDbm_DbmExecuteServerprocess_hpp
