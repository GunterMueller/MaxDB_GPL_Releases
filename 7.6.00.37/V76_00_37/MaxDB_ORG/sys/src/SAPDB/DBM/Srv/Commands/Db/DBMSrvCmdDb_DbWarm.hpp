/*!
  \file    DBMSrvCmdDb_DbOnline.hpp
  \author  MarcW
  \ingroup DBMServer commands
  \brief   obsolete versions of db_online

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

#if !defined(DBMSrvCmdDb_DbWarm_hpp)
#define DBMSrvCmdDb_DbWarm_hpp

#include "DBM/Srv/Commands/Db/DBMSrvCmdDb_DbOnline.hpp"

class DBMSrvCmdDb_DbWarm: public DBMSrvCmdDb_DbOnline {
public:
    DBMSrvCmdDb_DbWarm()
        : DBMSrvCmdDb_DbOnline(true) {  // this command is obsolete

        m_longHelp=
            "@command db_warm This is an obsolete version of command db_online."
            "@syntax db_warm (obsolete version of db_online)";
    }

    static const char * getCommandName() {return "db_warm";}
};

class DBMSrvCmdDb_Dbwarm: public DBMSrvCmdDb_DbOnline {
public:
    DBMSrvCmdDb_Dbwarm()
        : DBMSrvCmdDb_DbOnline(true) { // this command is obsolete

        m_longHelp=
            "@command dbwarm This is an obsolete version of command db_online."
            "@syntax dbwarm (obsolete version of db_online)";
    }

    static const char * getCommandName() {return "dbwarm";}
};

#endif //DBMSrvCmdDb_DbWarm_hpp
