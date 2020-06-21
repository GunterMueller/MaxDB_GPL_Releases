/*!
  \file    DBMSrvCmdBHist_BackupHistoryAppend.hpp
  \author  TiloH
  \ingroup DBMServer commands
  \brief   declaration of a class handling DBM Server command
           backup_history_append

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

#if !defined(DBMSrvCmdBHist_BackupHistoryAppend_hpp)
#define DBMSrvCmdBHist_BackupHistoryAppend_hpp

#include "SAPDB/DBM/Srv/DBMSrv_Command.hpp"

/*! \brief A class for handling the DBMServer command backup_history_append
    
    Appends certain information to the external backup history */
class DBMSrvCmdBHist_BackupHistoryAppend: public DBMSrv_Command
{
  public:
    DBMSrvCmdBHist_BackupHistoryAppend();

    /*! the virtual function implementing the backup_history_append command in 7.6
        \param vcontrol    [INOUT] DBMServer session data
        \param command     [IN]    DBMServer command
        \param reply       [OUT]   reply buffer object
        \return error code specified in gcn00.h */
    tcn00_Error run(VControlDataT * vcontrol,
                    CommandT      * command,
                    DBMSrv_Reply  & reply);

    /*! the function implementing the backup_history_append command for 7.5.00
        \param vcontrol    [INOUT] DBMServer session data
        \param command     [IN]    DBMServer command
        \param replyData   [OUT]   reply buffer
        \param replyLen    [OUT]   lenght of reply
        \param replyLenMax [IN]    max. length of reply buffer
        \return error code specified in gcn00.h */
    static tcn00_Error runCommand(VControlDataT * vcontrol,
                                  CommandT      * command,
                                  char          * replyData,
                                  int           * replyLen,
                                  int             replyLenMax);
};

#endif
