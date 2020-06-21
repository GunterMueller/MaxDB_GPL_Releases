/*!
  \file    DBMSrvCmdMedium_MediumLabelOffline.hpp
  \author  TiloH
  \ingroup DBM Server commands
  \brief   definition of a classes handling DBM Server
           command medium_put

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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

#if !defined(DBMSRVCMDMEDIUM_MEDIUMLABELOFFLINE_HPP)
#define DBMSRVCMDMEDIUM_MEDIUMLABELOFFLINE_HPP

#include "DBM/Srv/DBMSrv_Command.hpp"

class DBMSrvCmdMedium_MediumLabelOffline : public DBMSrv_Command
{
  public:
    /*! constructor */
    DBMSrvCmdMedium_MediumLabelOffline();

    /*! \brief the virtual function implementing the medium_put command
        \param vcontrol [IN/OUT] vcontrol structure to use
        \param command  [IN]     the command to execute
        \param reply    [OUT]    the answer of the command
        \return OK_CN00 if successful, an error value other than OK_CN00
            otherwise */
    tcn00_Error run(VControlDataT * vcontrol,
                    CommandT      * command,
                    DBMSrv_Reply  & reply);

    /*! \brief get command name as string
        \return the name of the command as string */
    static inline const char * getCommandName(){return "medium_labeloffline";} //!<needed to be static to avoid object instantiation during search of fitting command class
};

#endif
