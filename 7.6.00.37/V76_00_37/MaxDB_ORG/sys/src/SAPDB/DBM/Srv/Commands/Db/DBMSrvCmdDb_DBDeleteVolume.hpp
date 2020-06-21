/*!
    \file    DBMSrvCmdDb_DBDeleteVolume.hpp
    \author  TiloH
    \ingroup DBM Server commands, section DB
    \brief   declaration of class for handling DBM Server command
             db_deletevolume

    \if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2005 SAP AG

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


#if !defined(DBMSRVCMDDB_DBDELETEVOLUME_HPP)
#define DBMSRVCMDDB_DBDELETEVOLUME_HPP

#include "gcn00.h"
#include "DBM/Srv/DBMSrv_Command.hpp"

/*! \brief class for handling the DBM Server command db_deletevolume */
class DBMSrvCmdDb_DBDeleteVolume : public DBMSrv_Command
{
  public:
    /*! \brief constructor */
    DBMSrvCmdDb_DBDeleteVolume();

    /*! \brief actual implementation of the command

        \param vcontrol [IN/OUT] dbm server object for global data
        \param command  [IN]     command line parameters
        \param reply    [OUT]    object managing the reply buffer, command
            writes its response into this buffer
        \return DBM Server return code resulting from commands execution */
    tcn00_Error run(VControlDataT * vcontrol,
        CommandT      * command,
        DBMSrv_Reply& theReply);

    /*! \brief function defining the name of the command
        \return "db_deletevolume" */
    static const char * getCommandName() {return "db_deletevolume";}
};

#endif
