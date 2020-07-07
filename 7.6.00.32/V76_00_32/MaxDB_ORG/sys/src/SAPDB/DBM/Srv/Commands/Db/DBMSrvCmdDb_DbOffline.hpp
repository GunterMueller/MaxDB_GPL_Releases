/*!
  \file    DBMSrvCmdDb_DbOffline.hpp
  \author  MarcW
  \ingroup DBMServer commands
  \brief   declaration of class for handling DBMServer
           command db_offline

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

#if !defined(DBMSrvCmdDb_DbOffline_hpp)
#define DBMSrvCmdDb_DbOffline_hpp

#include "DBM/Srv/DBMSrv_Command.hpp"

/*! 
    @brief Class for handling the DBMServer command db_offline
*/
class DBMSrvCmdDb_DbOffline: public DBMSrv_Command {
public:
    /*! @brief constructor */
    DBMSrvCmdDb_DbOffline( bool obsolete = false );

    /*! 
        @brief the actual implementation of the command execution

        @param vcontrol [INOUT] the dbmserver object
        @param command [IN] command line
        @param theReply [OUT] the object to write command response into
        @return execution result status
    */
    tcn00_Error run(
//    tcn00_Error runUsingSharedMemory(
            VControlDataT * vcontrol,
            CommandT      * command,
            DBMSrv_Reply& theReply);

    static const char * getCommandName() {return "db_offline";}

/*
protected:
    virtual bool incrementShmCount() { return true; }
    virtual void decrementShmCount() {}
*/

};

// obsolete version

class DBMSrvCmdDb_Dboffline: public DBMSrvCmdDb_DbOffline {
public:
    DBMSrvCmdDb_Dboffline()
        : DBMSrvCmdDb_DbOffline(true) { // this command is obsolete

        m_longHelp=
            "@command dboffline This is an obsolete version of command db_offline."
            "@syntax dboffline (obsolete version of db_offline)";
    }

    static const char * getCommandName() {return "dboffline";}
};

#endif //DBMSrvCmdDb_DbOffline_hpp
