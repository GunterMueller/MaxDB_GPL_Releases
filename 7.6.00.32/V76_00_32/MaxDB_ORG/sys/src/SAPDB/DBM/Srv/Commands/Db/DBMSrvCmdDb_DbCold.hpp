/*!
  \file    DBMSrvCmdDb_DbCold.hpp
  \author  MarcW
  \ingroup DBMServer commands
  \brief   obsolete versions o command db_admin

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


#if !defined(DBMSrvCmdDb_DbCold_hpp)
#define DBMSrvCmdDb_DbCold_hpp

#include "DBM/Srv/Commands/Db/DBMSrvCmdDb_DbAdmin.hpp"

class DBMSrvCmdDb_DbCold: public DBMSrvCmdDb_DbAdmin {
public:
    DBMSrvCmdDb_DbCold()
        : DBMSrvCmdDb_DbAdmin(true) { // this command is obsolete

        m_longHelp=
            "@command db_cold This is an obsolete version of command db_admin."
            "@syntax db_cold (obsolete version of db_admin)";
    }

    static const char * getCommandName() {return "db_cold";}
};

class DBMSrvCmdDb_Dbcold: public DBMSrvCmdDb_DbAdmin {
public:
    DBMSrvCmdDb_Dbcold()
        : DBMSrvCmdDb_DbAdmin(true) { // this command is obsolete

        m_longHelp=
            "@command dbcold This is an obsolete version of command db_admin."
            "@syntax dbcold (obsolete version of db_admin)";
    }

    static const char * getCommandName() {return "dbcold";}
};

#endif //DBMSrvCmdDb_DbCold_hpp
