/*!
  \file    DBMSrvCmdEvent_EventList.hpp
  \author  MarcW
  \ingroup DBMServer commands
  \brief   list all events

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

#if !defined(DBMSrvCmdEvent_EventList_hpp)
#define DBMSrvCmdEvent_EventList_hpp

#include "DBM/Srv/Commands/DBMSrvCmd_EventAdministrationCommand.hpp"

/*! 
    @brief Class for handling the DBMServer command event_list
*/
class DBMSrvCmdEvent_EventList: public DBMSrvCmd_EventAdministrationCommand {
public:
    /*! @brief constructor */
    DBMSrvCmdEvent_EventList();

    /*! @brief destuctor */
    ~DBMSrvCmdEvent_EventList() {}

    /*! 
        @brief the actual implementation of the command execution

        @param vcontrol [INOUT] the dbmserver object
        @param command [IN] command line
        @param theReply [OUT] the object to write command response into
        @return execution result status
    */
    tcn00_Error runEventAdministrationCommand(
            VControlDataT * vcontrol,
            DBMSrvPar_Parameters* kernelParameters,
            CommandT      * command,
            DBMSrv_Reply& theReply);

private:
    static const char* const m_TextName;
    static const char* const m_TextValue;
    static const char* const m_TextPrio;
    static const char* const m_TextHelp;

    static const char* const m_LongHelp;
};

#endif //DBMSrvCmdEvent_EventList_hpp
