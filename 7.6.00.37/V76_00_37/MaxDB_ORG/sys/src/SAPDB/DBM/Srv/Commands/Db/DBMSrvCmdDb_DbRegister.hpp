/*!
  \file    DBMSrvCmdDb_DbRegsiter.hpp
  \author  BerndV
  \ingroup DBMServer commands
  \brief   obsolete versions db_register of db_reg

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

#if !defined(DBMSrvCmdDb_DbRegister_hpp)
#define DBMSrvCmdDb_DbRegister_hpp

#include "DBM/Srv/Commands/Db/DBMSrvCmdDb_DbReg.hpp"

class DBMSrvCmdDb_DbRegister: public DBMSrvCmdDb_DbReg {
public:
    DBMSrvCmdDb_DbRegister() : DBMSrvCmdDb_DbReg(true)  // this command is obsolete
    { 
        m_longHelp=
            "@command db_regsiter This is an obsolete version of command db_reg."
            "@syntax db_register (obsolete version of db_reg)";
    } // end DBMSrvCmdDb_DbRegister

    static const char * getCommandName() {return "db_register";}
};

#endif //DBMSrvCmdDb_DbWarm_hpp
