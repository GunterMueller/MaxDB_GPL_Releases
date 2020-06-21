/*!
    @file           DBMCliEvtDisp_Dispatcher.hpp
    @author         MarcW
    @brief          Event Dispatcher  - declaration

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

#ifndef DBMCliEvtDisp_Dispatcher_HPP
#define DBMCliEvtDisp_Dispatcher_HPP

#include <stdio.h>
#include <signal.h>

#include "DBM/Cli/Eventing/Dispatcher/DBMCliEvtDisp_EventListener.hpp"
#include "DBM/Cli/Eventing/Dispatcher/DBMCliEvtDisp_Properties.hpp"
#include "DBM/Cli/Eventing/Dispatcher/DBMCliEvtDisp_SrvCommand.hpp"
#include "DBM/Cli/Eventing/Dispatcher/KeyMatch/DBMCliEvtDispKeyMatch_Expression.hpp"
#include "DBM/Cli/DBMCli_Stuff.hpp"
#include "DBM/Logging/DBMLog_Logger.hpp"

/*!
    @brief Event Dispatcher

    This Event Dispatcher class offers the possibility to connect to some MaxDB database and listen
    to its kernel events. By configuration, the dispatcher starts other programs (so called event
    handlers) and passes eventually required connect data and event data to them.

    The configuration is read from a configuration file. The activities of the dispatcher can be
    logged in a logfile.

    @see DBMCliEvtDisp_EventHandler
*/
class DBMCliEvtDisp_Dispatcher : public DBMCliEvtDisp_EventListener {

public:

    /*!
        @brief Constructor

        Constructor to which all runtime information can be passed. If not configuration file
        is passed, the configuration is first searched in the DB's rundirectory. If no configuration
        can be found there, <instroot>/env is seached for a configuration file If no log file is
        passed, a logfile in the DB's rundirectory will be written.

        @param aNode        [IN] DB host (connect data)
        @param aDb          [IN] DB name (connect data)
        @param aUserpw      [IN] user/pw combination (connect data)
        @param aCfgfile     [IN] path to configuration file
        @param aLogfile     [IN] path to log file
        @param oMessageList [OUT] message list
    */
    DBMCliEvtDisp_Dispatcher(
            bool internal,
            bool remotaAccess,
            const DBMCli_String& aNode,
            const DBMCli_String& aDb,
            const DBMCli_String& aUserpw,
            const DBMCli_String& aCfgfile,
            const DBMCli_String& aLogfile,
            Msg_List& outList );

    /*! @brief destructor */
    ~DBMCliEvtDisp_Dispatcher();


    int getInstanceID() const {return m_InstanceID;}
    bool isListening() const {return m_bRunning;}

    /*! @brief implements the event listening loop  */
    bool Run();

  /*!
      @brief called if an event is received

      Here, the actual dispatching takes place: The configuration is evaluated and for each handler
      found there, the variables from the configuration are replaced with their actual values.
      @param oEvent kernel event received from database
      @return indicator whether event listening should continue (always true here)
  */
  bool EventProc ( const DBMCliEvtCm_Event& oEvent );

private:

    DBMCliEvtDisp_SrvCommand::ID getCommandFromServer(Msg_List& outList);

    Tools_List<DBMCliEvtDispKeyMatch_Expression*>* m_DispatchingList;

    bool m_IsInternal;
    bool m_NotOnlineDispatched;

	/*!
		@brief jow to handle the problem, that there is no event task available
		
		After (and including) 7.6.14 the DBMServer sends the message -24755, this variable
		is set to true, in case the server version is 7.6.14 or higher. False otherwise.
	*/
	bool m_TasklimitMessageFromDBM;

    bool clearDispatchingList();
    bool clearRawList(Tools_List<Tools_Property*>* aRawList);
    bool fillDispatchingList(Tools_List<Tools_Property*>* aRawList);


    /*!
        @brief action to take after creating own events

        After creating own events, it can be necessary to either
        go on listeing immediately, stop listening or to wait until
        trying to go on listening, because there is no connection.
    */
    enum NextActionType {
        GoonListening, /*!< go on immediately */
        WaitListening, /*!< wait before listening */
        StopListening  /*!< stop listening completely */
    };

    /*! @brief the configuration */
    DBMCliEvtDisp_Properties dispatcherprop;
    /*! @brief user/pw combination */
    DBMCli_String m_userpw;
    /*! @brief the DB host name */
    DBMCli_String m_NodeName;
    /*! @brief index from DBMServer */
    int m_InstanceID;
    DBMCli_String m_ConfigFile;
    DBMCli_String m_LogFile;

    /*! @brief could configuration be read => can we really dispatch? */
    bool m_bRunning;

    /*!
        @brief event handlers to pass parameters to via pipe

        The last cell of this array must be filled with an empty string.
        The programs listed here will receive their parameters via pipe, so
        on unix platforms no user/pw is visible in process list.
    */
    static const DBMCli_String m_ViaPipe[5];

    bool executeStopCommand();

    /*!
        @brief call the passed command line

        If the event handler is contained in the list of handlers to be passed
        its parameters via pipe, this is done, otherwise parameters are passed
        on the command line.
        @parameter handler [IN] command line to be executed
        @see m_ViaPipe
    */
    void doCall(DBMCli_String& handler );

    /*!
        @brief replace a variable in a command line

        After the call, all variables in str will be replaced
        @param str     [IN/OUT] command line containing variables
        @param varName [IN] name of variable
        @param varVal  [IN] value to be put there instead
    */
    void replaceVar( DBMCli_String& str, const char* varName, const char* varVal);

    /*!
        @brief create own events

        checks the message list, that was generated and creates events.
        @return indicator for listening loop what to do next.
    */
    NextActionType HandleListenError();
    
    /*!
        @brief handles missed evenst

        Send a missing event event for event no. m_EvtCountKernel
        @see m_EvtCountKernel
    */
    void HandleMissedEvent();
    
    /*! @brief coutn of sefl-created events */
    SAPDB_Int4 m_EvtCountSelf;

    /*! @brief counter of received kernel events */
    SAPDB_Int4 m_EvtCountKernel;

    /*! @brief logger */
    DBMLog_Logger& m_Logger;

    /*! @brief variable for replacement in configuration file */
    static const char* const DBNAME;
    /*! @brief variable for replacement in configuration file */
    static const char* const DBMUSERPW;
    /*! @brief variable for replacement in configuration file */
    static const char* const SERVERNAME;
    /*! @brief variable for replacement in configuration file */
    static const char* const EVTALL;
    /*! @brief variable for replacement in configuration file */
    static const char* const EVTNAME;
    /*! @brief variable for replacement in configuration file */
    static const char* const EVTCOUNT;
    /*! @brief variable for replacement in configuration file */
    static const char* const EVTPRIO;
    /*! @brief variable for replacement in configuration file */
    static const char* const EVTVAL1;
    /*! @brief variable for replacement in configuration file */
    static const char* const EVTVAL2;
    /*! @brief variable for replacement in configuration file */
    static const char* const EVTDATE;
    /*! @brief variable for replacement in configuration file */
    static const char* const EVTTIME;
    /*! @brief variable for replacement in configuration file */
    static const char* const EVTDESC;
    /*! @brief variable for replacement in configuration file */
    static const char* const EVTTEXT;

    static const DBMCli_String EVT_NAME_SHUTDOWN;
    static const DBMCli_String EVT_NAME_DBCRASH;
    static const DBMCli_String EVT_NAME_UNKNOWNERR;
    static const DBMCli_String EVT_NAME_MISSEDEVENT;
    static const DBMCli_String EVT_NAME_DISPATCHERSTOP;
    static const DBMCli_String EVT_NAME_DISPATCHERSTART;
    static const DBMCli_String EVT_NAME_CLCONNECTIONLOST;
    static const DBMCli_String EVT_NAME_SRCONNECTIONLOST;
    static const DBMCli_String EVT_NAME_NOEVENTTASK;
    static const DBMCli_String EVT_TEXT_DBCRASH;
    static const DBMCli_String EVT_TEXT_UNKNOWNERR;
    static const DBMCli_String EVT_TEXT_MISSEDEVENT;
    static const DBMCli_String EVT_TEXT_DISPATCHERSTOP;
    static const DBMCli_String EVT_TEXT_DISPATCHERSTART;
    static const DBMCli_String EVT_TEXT_CLCONNECTIONLOST;
    static const DBMCli_String EVT_TEXT_SRCONNECTIONLOST;
    static const DBMCli_String EVT_TEXT_NOEVENTTASK;
};

#endif //DBMCliEvtDisp_Dispatcher_HPP