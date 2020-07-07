/*!
  \file    DBMSrvCmd_Explain.hpp
  \author  TiloH
  \ingroup DBMServer commands
  \brief   definition of a class for the DBMServer command explain

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

#if !defined(DBMSrvCmd_Explain_hpp)
#define DBMSrvCmd_Explain_hpp


//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------

#include "DBM/Srv/DBMSrv_Command.hpp"

//-----------------------------------------------------------------------------
// class DBMSrvCmd_Explain
//-----------------------------------------------------------------------------

//! \brief a class handling the DBMServer command explain
class DBMSrvCmd_Explain : public DBMSrv_Command {
public:
    DBMSrvCmd_Explain();

    //! the function implementing the explain command
    tcn00_Error run(VControlDataT * vcontrol,
                    CommandT      * command,
                    DBMSrv_Reply& theReply);
};
#endif
