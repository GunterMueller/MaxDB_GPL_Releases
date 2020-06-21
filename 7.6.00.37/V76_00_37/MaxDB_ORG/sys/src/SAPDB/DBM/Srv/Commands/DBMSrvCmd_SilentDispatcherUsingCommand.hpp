/*!
  \file    DBMSrvCmd_SilentDispatcherUsingCommand.hpp
  \author  MarcW
  \ingroup DBMServer commands
  \brief   base class for commands, that use the silent event dispatcher

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

#if !defined(DBMSrvCmd_SilentDispatcherUsingCommand_hpp)
#define DBMSrvCmd_SilentDispatcherUsingCommand_hpp

#include "DBM/Cli/Eventing/Dispatcher/DBMCliEvtDisp_Properties.hpp"
#include "DBM/Srv/DBMSrv_Command.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"
#include "DBM/Srv/SharedMemory/DBMSrvShM_LineAccessorResourceLock.hpp"
#include "ToolsCommon/Tools_DynamicUTF8String.hpp"
#include "vsp0031.h"

/*! 
    @brief base class for commands, that use the silent event dispatcher
*/
class DBMSrvCmd_SilentDispatcherUsingCommand: public DBMSrv_Command {
public:
    /*! @brief constructor */
    DBMSrvCmd_SilentDispatcherUsingCommand(CommandKey commandKey, const char * longHelp=0 );

    /*! @brief destructor */
    virtual ~DBMSrvCmd_SilentDispatcherUsingCommand() {};

    /*! 
        @brief implementation of the command execution

        @param vcontrol [INOUT] the dbmserver object
        @param command [IN] command line
        @param theReply [OUT] the object to write command response into
        @return execution result status
    */
    tcn00_Error run(VControlDataT* vcontrol,
        CommandT* command,
        DBMSrv_Reply& theReply);

    /* @brief check DBM server configuration, if parameter for starting event dispatcher is set */
    bool isConfiguredToRun(DBMSrvMsg_Error& errOut);

    static const char* const StopAndDelete;
    static const char* const StopAll;
    static const char* const Start;

protected:

    enum ConfigurationEntries {
        AUTOEXTEND,
        UPDATESTATISTICS
    };

    virtual tcn00_Error runUnProtectedBefore(
        VControlDataT* vcontrol,
        CommandT* command,
        DBMSrvMsg_Error& errOut) {

        m_UnprotectedBeforeOK = true;
        return OK_CN00;
    }

    /*!
        @brief to be overridden by subclasses

        Default implenmetation just starts the event dispatcher, if the DBM
        configuration requires this.
    */
    virtual tcn00_Error runLockProtected(
        VControlDataT* vcontrol,
        CommandT* command,
        DBMSrv_Reply& theReply);

    virtual tcn00_Error runUnProtectedAfter(
        VControlDataT* vcontrol,
        CommandT* command,
        DBMSrvMsg_Error& errOut) {
            
        m_UnprotectedAfterOK = true;
        return OK_CN00;
    }

    bool m_UnprotectedBeforeOK;
    bool m_UnprotectedAfterOK;
    bool m_ExecuteUnprotectedAfter;

    void getEventHandler(
        const Tools_DynamicUTF8String& aEventName,
        Tools_DynamicUTF8String& outKey,
        Tools_DynamicUTF8String& outValue);

    bool isDispatcherUp(DBMSrvMsg_Error& errOut);
    bool startDispatcher(DBMSrvMsg_Error& theErrList);
    bool stopDispatcher(DBMSrvMsg_Error& theErrList);

    tcn00_Error getEventCategoryNameFromID(
        const tsp31_event_ident_Enum eventID,
        DBMSrv_Reply& theReply,
        Tools_DynamicUTF8String& eventCategoryName );

    DBMCliEvtDisp_Properties m_SilentDispatcherProps;
private:
    bool migrateDispatcherConfiguration(
            VControlDataT* vcontrol,
            DBMSrv_Reply& theReply,
            DBMSrvMsg_Error& errOut);

    bool deleteConfigurationFile(DBMSrvMsg_Error& errOut);

    tsp00_Pathc m_Configfile;
};
#endif //DBMSrvCmd_SilentDispatcherUsingCommand
