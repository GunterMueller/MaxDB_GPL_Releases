/*!
  \file    DBMSrvCmd_BackupHistory.hpp
  \author  TiloH
  \ingroup DBMServer commands
  \brief   declaration of classes handling DBMServer
           backup history commands

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

#include "DBM/Srv/DBMSrv_Command.hpp"

#if !defined(DBMSrvCmd_BackupHistory_hpp)
#define DBMSrvCmd_BackupHistory_hpp


//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

/*! \brief A class for handling the DBMServer command backup_history_open
    
    The runCommand (re)creates the global backup history session object,
    reads the backup history files and stores the data into the new global
    backup history session object */
class DBMSrvCmd_BackupHistoryOpen : public DBMSrv_Command
{
  public:
    DBMSrvCmd_BackupHistoryOpen();

    //! the function implementing the backup_history_open command
    tcn00_Error run(VControlDataT * vcontrol,
        CommandT      * command,
        DBMSrv_Reply& theReply);

    static const char * const m_LongHelp;
};


/*! \brief A class for handling the DBMServer command backup_history_list
    
    The runCommand lists the first lines of the backup history from the
    global backup history session, that fit into the reply buffer */
class DBMSrvCmd_BackupHistoryList : public DBMSrv_Command
{
  public:
    DBMSrvCmd_BackupHistoryList();

    //! the function implementing the backup_history_list command
    tcn00_Error run(VControlDataT * vcontrol,
                    CommandT      * command,
                    DBMSrv_Reply& theReply);

    static const char * const m_LongHelp;
};


/*! \brief A class for handling the DBMServer command backup_history_listnext
    
    The runCommand lists the next lines of the backup history from the
    global backup history session, that fit into the new reply buffer */
class DBMSrvCmd_BackupHistoryListNext : public DBMSrv_Command
{
  public:
    DBMSrvCmd_BackupHistoryListNext();

    //! the function implementing the backup_history_listnext command
    tcn00_Error run(VControlDataT * vcontrol,
        CommandT      * command,
        DBMSrv_Reply& theReply);

    static const char * const m_LongHelp;
};


/*! \brief A class for handling the DBMServer command backup_history_close
    
    The runCommand deletes the global backup history session */
class DBMSrvCmd_BackupHistoryClose : public DBMSrv_Command
{
  public:
    DBMSrvCmd_BackupHistoryClose();

    //! the function implementing the backup_history_close command
    tcn00_Error run(VControlDataT * vcontrol,
        CommandT      * command,
        DBMSrv_Reply& theReply);

    static const char * const m_LongHelp;
};


/*! \brief A class for handling the DBMServer command backup_history_date
    
    The runCommand writes the date of the backup history file (dbm.knl) into the reply */
class DBMSrvCmd_BackupHistoryDate : public DBMSrv_Command
{
  public:
    DBMSrvCmd_BackupHistoryDate();

    //! the function implementing the backup_history_date command
    tcn00_Error run(VControlDataT * vcontrol,
        CommandT      * command,
        DBMSrv_Reply& theReply);

    static const char * const m_LongHelp;
};

#endif
