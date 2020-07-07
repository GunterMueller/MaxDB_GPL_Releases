/*!
  \file    DBMSrvProc_IOManagedProcess.hpp
  \author  MarcW
  \brief   Job declaration

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

#include "hni33.h"
#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_ErrorHandler.hpp"

#if !defined(DBMSrvProc_IOManagedProcess_hpp)
#define DBMSrvProc_IOManagedProcess_hpp

/*!
    @brief runs a dbm server command in a separate process

    The process' stdout is buffered in memory (digested into memory) and can be checked later.
    Communication to the process is done via pipes. This class is used from the class DBMSrvScd_Job
    in the command scheduler.
    @see DBMSrvScd_Job
*/
class DBMSrvProc_IOManagedProcess {
public:
    /*
        @brief constructor

        @param aDBName [IN] name of DB on which command is to be executed
        @param aUserID [IN] dbm user name
        @param aUserID [IN] dbm user's password
        @param aCommandLine [IN] dbm server command to be executed
    */
    DBMSrvProc_IOManagedProcess(
            const char* aDBName,
            const char* aUserID,
            const char* aPassword,
            const char* aCommandLine);

    /*! @brief destructor */
    ~DBMSrvProc_IOManagedProcess();


    /*!
        @brief get the pid of the dbmcli

        @return pid of the dbmcli
    */
    RTE_OSPid getClientPid() {return m_Process->getPid();}

    /*!
        @brief get the pid of the DBMServer

        @return pid of the DBMServer
    */
    RTE_OSPid getServerPid() {return m_ServerPid;}

    /*!
        @brief check if managed process is running

        @return true if managed process is running, false otherwise
    */
    bool isRunning() const;

    /*!
        @brief read next answer part from pipe

        Called from DBMSrvScd_Job in the main scheduling loop of the scheduler.
    */
    void digestNextCommandAnswerPart();

    /*!
        @brief get process' answer (as much of it as already available)

        Returned string is not 0-terminated. Memory must be freed by caller.
    */
    char* getLog( size_t& aLength ) const;

    char* getReply( size_t& aLength ) const;

    bool getReturnCode( int &returnCode ) const;

private:
    tni33_ABackgroundProcess* m_Process;
    tni34_APipe* m_Me2Process;
    tni34_APipe* m_Process2Me;
    char* m_AnswerBuffer;
    size_t m_CurrentWriteOffset;

    char* m_DBName;

    bool m_CommandAnswerComplete;
    char m_CmdBuffer[128];

    RTE_OSPid m_ServerPid;

    /*! @brief digest answer until time is up or end is reached */
    bool digestAnswer(const time_t allowedTime, const char* endMarker);
    void digestString(const char* aString);
    bool digestPrompt();

    void setServerPid();
    void logError(
        const DBMSrvMsg_Error & error);

    static int m_InstanceCount;
    static DBMSrvMsg_ErrorHandler m_ErrorHandler;

    static const char* const m_StartTemplate;
    static const char* const m_LogonTemplate;
    static const char* const m_CommandTemplate;
    static const char* const m_CommandDbmGetpid;
    static const char* const m_ExitCommand;
    static const char* const m_StartAnswerEndMarkerTemplate;
    static const char* const m_StandardAnswerEndMarker;
    
    static const char* const m_ProtocolSeparatorStart;
    static const char* const m_ProtocolSeparatorLogon;
    static const char* const m_ProtocolSeparatorExecute;
    static const char* const m_ProtocolSeparatorExit;
    
    static const int m_SleepTimeWhileDigesting; //milliseconds
    static const int m_SleepTimeWhileLogon; //milliseconds
    static const time_t m_DigestingTimePart; // seconds
    static const time_t m_DigestingTimeComplete; // seconds
    static const size_t m_AnswerSize; // bytes
};

#endif //DBMSrvProc_IOManagedProcess_hpp
