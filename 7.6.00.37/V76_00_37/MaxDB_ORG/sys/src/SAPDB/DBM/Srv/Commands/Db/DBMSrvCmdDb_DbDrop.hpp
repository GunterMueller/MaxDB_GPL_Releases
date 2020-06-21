/*!
  \file    DBMSrvCmdDb_DbDrop.hpp
  \author  MarcW
  \ingroup DBMServer commands
  \brief   declaration of class for handling DBMServer
           command db_online

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

#if !defined(DBMSrvCmdDb_DbDrop_hpp)
#define DBMSrvCmdDb_DbDrop_hpp

#include "DBM/Srv/DBMSrv_Command.hpp"

/*! 
    @brief Class for handling the DBMServer command add_volume
*/
class DBMSrvCmdDb_DbDrop: public DBMSrv_Command {
public:
    /*! @brief constructor , obsolete flag for derived old commands */
    DBMSrvCmdDb_DbDrop();

    virtual ~DBMSrvCmdDb_DbDrop() {}

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


tcn00_Error doDrop(
        tsp00_DbNamec         szDbName,
        const tsp00_Pathc     szDbRoot,
        const bool            bWithFiles,
        const bool            bDeleteLog,
        DBMSrv_Reply&         reply,
        bool                  bServiceDB);

    static const char* getCommandName() {return "db_drop";}

private:
    /*! 
        @brief set the dropped flag of the DBMServer
    */
    void setToDropped();

    /*! 
        @brief check for other dbmservers
    */
    bool noOtherServersUp(const char* dbName, DBMSrvMsg_Error & errOut);

    /*! 
        @brief check for other dbmservers
    */
    bool waitForOtherServers(const char* dbName, DBMSrvMsg_Error & errOut);

    static const char* WITHOUTFILES;
};

#endif //DBMSrvCmdDb_DbDrop_hpp
