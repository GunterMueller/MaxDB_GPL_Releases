/*!
  \file    DBMSrvCmdMedium_MediumDelete.hpp
  \author  TiloH
  \ingroup DBMServer commands
  \brief   definition of a classes handling DBMServer
           command medium_delete

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

#include "DBM/Srv/DBMSrv_Command.hpp"

class DBMSrvCmdMedium_MediumDelete : public DBMSrv_Command
{
  public:
    DBMSrvCmdMedium_MediumDelete();

    /*! \brief the virtual function implementing the medium_delete command */
    tcn00_Error run(VControlDataT * vcontrol,
                    CommandT      * command,
                    DBMSrv_Reply  & theReply);

    /*! \brief static function providing the run function of medium_delete for vcn10.cpp
        \TODO should be eliminated together with vcn10.cpp in the long run */
    static tcn00_Error runCommand
        (VControlDataT * vcontrol,
         CommandT      * command,
         char          * replyData,
         int           * replyLen,
         int             replyLenMax);

    static const char * const m_LongHelp; //!< long help string, used in vcn10.cpp
};
