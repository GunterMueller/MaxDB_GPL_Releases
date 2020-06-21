/*!
  \file    DBMSrvCmd_Exit.hpp
  \author  MarcW
  \ingroup DBMServer commands
  \brief   exit the DBMServer

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

#if !defined(DBMSrvCmd_Exit_hpp)
#define DBMSrvCmd_Exit_hpp

#include "DBM/Srv/DBMSrv_Command.hpp"

/*! 
    @brief Class for handling the DBMServer command exit
*/
class DBMSrvCmd_Exit: public DBMSrv_Command {
public:
    /*! @brief constructor */
    DBMSrvCmd_Exit();

    virtual ~DBMSrvCmd_Exit() {}

    virtual bool isExitCommand() {return true;}

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

    static const char * getCommandName() {return "exit";}
};

#endif //DBMSrvCmd_Exit_hpp
