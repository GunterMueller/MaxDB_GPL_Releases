/*!
  \file    DBMSrvCmd_Quit.hpp
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

#if !defined(DBMSrvCmd_Quit_hpp)
#define DBMSrvCmd_Quit_hpp

#include "DBM/Srv/Commands/DBMSrvCmd_Exit.hpp"

/*! 
    @brief Class for handling the DBMServer command quit
*/
class DBMSrvCmd_Quit: public DBMSrvCmd_Exit {
public:
    /*! @brief constructor */
    DBMSrvCmd_Quit()
        : DBMSrvCmd_Exit() {
    m_longHelp=
        "@command quit You close the Database Manager CLI program."
        "@preconditions none"
        "@syntax quit"
        "@reply OK<NL>";
    }

    static const char * getCommandName() {return "quit";}
};

#endif //DBMSrvCmd_Quit_hpp
