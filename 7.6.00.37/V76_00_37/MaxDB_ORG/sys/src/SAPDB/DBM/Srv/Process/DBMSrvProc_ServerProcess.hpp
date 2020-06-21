/*!
  \file    DBMSrvProc_ServerProcess.hpp
  \author  MarcW
  \brief   server process declaration

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

#include "DBM/Srv/Process/DBMSrvProc_ArgumentList.hpp"
#include "DBM/Srv/Process/DBMSrvProc_ServerProcessCommandLineConverter.hpp"

#if !defined(DBMSrvProc_ServerProcess_hpp)
#define DBMSrvProc_ServerProcess_hpp

/*!
    @brief a process that runs independend of a logged on user

    On unix systems this class does nothing special, since a spawned process runs until
    it is expicitly terminated. On windows systems, all processes started by an interactive
    user are terminated when he logs off. To overcome this, this class makes use of the
    fact that a child process of a service runs with the service user credentials, it is
    not terminated when the user logs off.

    To accomplish this, the command line is executed by a process that is a child
    process of a DBM server (an "intermediate" DBM server), that is started through the
    x-server (making the process a grand child of the NT service "x-server").

    There is a special dbm server command (dbm_execute_serverprocess) that is called. Only
    seletected executables, that are defined in a mapping, be be called.
    @see DBMSrvProc_ServerProcessCommandLineConverter
    @see DBMSrvCmd_DbmExecuteServerprocess
*/
class DBMSrvProc_ServerProcess {
public:
    /*!
        @brief constructor

        @param aProgramKey [IN] which progam to execute as server process
        @param aCommandLine [IN] parameters for the program
    */
    DBMSrvProc_ServerProcess(
        const DBMSrvProc_ServerProcessCommandLineConverter::ProgramKey aProgramKey,
        DBMSrvProc_ArgumentList& aArgList);

    /*! @brief destructor */
    ~DBMSrvProc_ServerProcess() {};

    /*!
        @brief check if process could be started

        This method does not check whether the process is still running. It is checked,
        if the information could be passed to the DBM server that should start
        the process.

        @return true if process could be started, false otherwise
    */
    bool started() const {return m_Started;}

    /*!
        @brief if the process could not be started, available error messages can be retrieved.

        Call th is method, if started() returns false.
        @see started()

        @param aMsgList [OUT] message list, to which the error messages, that we created
        during the creation of the server process, are added.
    */
    void getMessagesInto( Msg_List& aMsgList ) { aMsgList.Overrule(m_ErrorMessages);}

private:
    Msg_List m_ErrorMessages;
    bool m_Started;

    static const char* const m_LocalHostText;
};

#endif //DBMSrvProc_ServerProcess_hpp
