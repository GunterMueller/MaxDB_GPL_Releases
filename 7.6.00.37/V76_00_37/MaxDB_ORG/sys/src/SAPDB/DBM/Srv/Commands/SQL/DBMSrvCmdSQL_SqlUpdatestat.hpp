/*!
    \file    DBMSrvCmdSQL_SqlUpdatestat.hpp
    \author  MarcW
    \ingroup DBM Server commands
    \brief   update statistics

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

#if !defined(DBMSrvCmdSQL_SqlUpdatestat_hpp)
#define DBMSrvCmdSQL_SqlUpdatestat_hpp

#include "DBM/Srv/DBMSrv_Command.hpp"

class DBMSrvCmdSQL_SqlUpdatestat: public DBMSrv_Command {
public:
    DBMSrvCmdSQL_SqlUpdatestat();
    ~DBMSrvCmdSQL_SqlUpdatestat() {};

    tcn00_Error run(
            VControlDataT * vcontrol,
            CommandT      * command,
            DBMSrv_Reply& theReply);

    static const char* getCommandName();
private:
    static const char* const m_LongHelp;
    static const char* const SQL_UPDATESTAT;
    static const char* const SQL_UPDATESTAT_ALL;
};
#endif //DBMSrvCmdSQL_SqlUpdatestat_hpp
