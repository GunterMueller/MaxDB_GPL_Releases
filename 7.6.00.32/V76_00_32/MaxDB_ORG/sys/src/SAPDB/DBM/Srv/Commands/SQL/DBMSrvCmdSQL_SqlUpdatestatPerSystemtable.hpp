/*!
    \file    DBMSrvCmdSQL_SqlUpdatestatPerSystemtable.hpp
    \author  MarcW
    \ingroup DBM Server commands
    \brief   update statistics per system table

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


*/

#if !defined(DBMSrvCmdSQL_SqlUpdatestatPerSystemtable_hpp)
#define DBMSrvCmdSQL_SqlUpdatestatPerSystemtable_hpp

#include "DBM/Srv/DBMSrv_Command.hpp"

// command name
#define DBMSrvCmdSQL_SqlUpdatestatPerSystemtable_CMD "sql_updatestat_per_systemtable"

class DBMSrvCmdSQL_SqlUpdatestatPerSystemtable: public DBMSrv_Command {
public:
    DBMSrvCmdSQL_SqlUpdatestatPerSystemtable();
    ~DBMSrvCmdSQL_SqlUpdatestatPerSystemtable() {};

    /*! @brief standard interface, called from command factory, calls updateSmart internally */
    tcn00_Error run(
            VControlDataT * vcontrol,
            CommandT      * command,
            DBMSrv_Reply& theReply);

    /*! @brief custom, this contains the real thing */
    bool updateSmart(
            VControlDataT* vcontrol,
            DBMSrvMsg_Error& errOut);

    bool updateSmartInBackground(
            VControlDataT* vcontrol,
            DBMSrvMsg_Error& errOut);

    static const char* getCommandName();
private:
    static const char* const m_LongHelp;
    static const char* const SQL_UPDATESTAT_SMART;
};
#endif //DBMSrvCmdSQL_SqlUpdatestatPerSystemtable_hpp
