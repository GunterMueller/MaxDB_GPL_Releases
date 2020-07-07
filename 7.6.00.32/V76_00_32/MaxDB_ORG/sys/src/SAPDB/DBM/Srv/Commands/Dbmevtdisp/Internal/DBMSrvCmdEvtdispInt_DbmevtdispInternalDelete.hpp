/*!
  \file    DBMSrvCmdEvtdispInt_DbmevtdispInternalDelete.hpp
  \author  MarcW
  \ingroup internal DBMServer commands for event dispatcher
  \brief   

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

#if !defined(DBMSrvCmdEvtdispInt_DbmevtdispInternalDelete_hpp)
#define DBMSrvCmdEvtdispInt_DbmevtdispInternalDelete_hpp

#include "DBM/Srv/DBMSrv_Command.hpp"

/*! 
    @brief class for handling the DBMServer command dbmevtdisp_internal_delete

    This command is either called from a event dispatcher that was cancelled (can be remote)
    or locally for administration.
*/
class DBMSrvCmdEvtdispInt_DbmevtdispInternalDelete: public DBMSrv_Command {
public:
    /*! @brief constructor */
    DBMSrvCmdEvtdispInt_DbmevtdispInternalDelete();

    /*! @brief destructor */
    ~DBMSrvCmdEvtdispInt_DbmevtdispInternalDelete(){}

    /*! 
        @brief implementation of the command execution

        @param vcontrol [INOUT] the dbmserver object
        @param command [IN] command line
        @param theReply [OUT] the object to write command response into
        @return execution result status
    */
    tcn00_Error run(
            VControlDataT* vcontrol,
            CommandT* command,
            DBMSrv_Reply& theReply);

    static const char* getCommandName();

private:
    static const char* const m_LongHelp;
};

#endif //DBMSrvCmdEvtdispInt_DbmevtdispInternalDelete_hpp
