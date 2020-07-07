/*!
  \file    DBMSrvCmd_GetDefault.hpp
  \author  TiloH
  \ingroup DBMServer commands
  \brief   definition of a class for the DBMServer command getdefault

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2004-2005 SAP AG

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

#if !defined(DBMSrvCmd_GetDefault_hpp)
#define DBMSrvCmd_GetDefault_hpp


#include "DBM/Srv/DBMSrv_Command.hpp"


//! \brief a class handling the DBMServer command default
class DBMSrvCmd_GetDefault : public DBMSrv_Command
{
  public:
    DBMSrvCmd_GetDefault();

    //! the function implementing the default command
    tcn00_Error run(VControlDataT * vcontrol,
                    CommandT      * command,
                    DBMSrv_Reply  & theReply);
};
#endif
