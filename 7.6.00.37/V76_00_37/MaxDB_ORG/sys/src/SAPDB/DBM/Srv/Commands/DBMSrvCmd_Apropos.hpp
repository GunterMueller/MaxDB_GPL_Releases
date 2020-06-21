/*!
  \file    DBMSrvCmd_Apropos.hpp
  \author  TiloH
  \ingroup DBMServer commands
  \brief   declaration of a class for handling DBMServer
           command apropos

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

#if !defined(DBMSrvCmd_Apropos_hpp)
#define DBMSrvCmd_Apropos_hpp


#include "gcn00.h"
#include "DBM/Srv/DBMSrv_Command.hpp"


//! \brief Class for handling the DBMServer command apropos
class DBMSrvCmd_Apropos : public DBMSrv_Command
{
  public:
    DBMSrvCmd_Apropos();

    /*! \brief implementation of the DBM Server command apropos

        \param  vcontrol [IN/OUT] global DBM Server data object
        \param  command  [IN]     command line
        \param  reply    [OUT]    object to write command response into
        \return execution result status
    */
    tcn00_Error run(
        VControlDataT * vcontrol,
        CommandT      * command,
        DBMSrv_Reply  & reply);
};

#endif
