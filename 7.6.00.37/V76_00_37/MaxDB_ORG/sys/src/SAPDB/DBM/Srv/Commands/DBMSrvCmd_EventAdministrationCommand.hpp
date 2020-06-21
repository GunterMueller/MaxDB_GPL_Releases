/*!
  \file    DBMSrvCmd_EventAdministrationCommand.hpp
  \author  MarcW
  \ingroup DBMServer commands
  \brief   base class for event administration commands

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

#if !defined(DBMSrvCmd_EventAdministrationCommand_hpp)
#define DBMSrvCmd_EventAdministrationCommand_hpp

#include "DBM/Srv/Parameters/DBMSrvPar_Parameters.hpp"
#include "SAPDBCommon/DatabaseEvents/SAPDBDBE_KernelParameterNames.hpp"
#include "DBM/Srv/Commands/DBMSrvCmd_EventAdministrationConstants.hpp"
#include "DBM/Srv/DBMSrv_Command.hpp"

class DBMSrvCmd_EventAdministrationCommand:
        public DBMSrvCmd_EventAdministrationConstants,
        public DBMSrv_Command {
public:
    /*! @brief constructor for subclasses */
    DBMSrvCmd_EventAdministrationCommand(
            CommandKey commandKey,
            bool isObsolete,
            const char* aLongHelp,
            bool isHidden = false);

    /*! @brief required for subclassing */
    virtual ~DBMSrvCmd_EventAdministrationCommand() {}

    /*! 
        @brief the actual implementation of the command execution

        @param vcontrol [INOUT] the dbmserver object
        @param command [IN] command line
        @param theReply [OUT] the object to write command response into
        @return execution result status
    */
    tcn00_Error run(
            VControlDataT * vcontrol,
            CommandT      * command,
            DBMSrv_Reply& theReply);

protected:
    virtual tcn00_Error runEventAdministrationCommand(
            VControlDataT * vcontrol,
            DBMSrvPar_Parameters* kernelParameters,
            CommandT      * command,
            DBMSrv_Reply& theReply) = 0;
};

#endif //DBMSrvCmd_EventAdministrationCommand
