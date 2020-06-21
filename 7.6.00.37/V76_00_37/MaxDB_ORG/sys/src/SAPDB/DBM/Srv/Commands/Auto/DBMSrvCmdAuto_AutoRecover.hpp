/*!
    \file    DBMSrvCmdAuto_AutoRecover.hpp
    \author  TiloH
    \ingroup DBM Server commands
    \brief   automatic recovery

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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

#if !defined(DBMSRVCMDAUTO_AUTORECOVER_HPP)
#define DBMSRVCMDAUTO_AUTORECOVER_HPP


#include "DBM/Srv/BackupHistory/DBMSrvBHist_RestorePlanner.hpp"
#include "DBM/Srv/Infrastructure/Commands/DBMSrvInfrCmd_ClassicCommand.hpp"


/*! 
    @brief Class for handling the DBM Server command autorecover
*/
class DBMSrvCmdAuto_AutoRecover
    : public DBMSrvInfrCmd_ClassicCommand
{
  public:
    /* default constructor
    
       DBMSrvInfrCmd_ClassicCommand is used as base class for convinience
       \TODO avoid usage of DBMSrvInfrCmd_ClassicCommand */
    DBMSrvCmdAuto_AutoRecover():DBMSrvInfrCmd_ClassicCommand(DBMSrvBHist_RestorePlanner::AutomaticRecovery)
    {
        m_longHelp=
            "@command autorecover You start an automatic restore of the "
                "database. The backup history is used to determine the "
                "backups needed for the restore. File backups and backups from "
                "external backup tools are checked for availability."
            "@preconditions You have the server authorization Recovery. You "
                "have opened a utility session (see DBM command "
                "util_connect)."
            "@syntax autorecover [LOG] [UNTIL <date> <time>] [ ( KEY <key> "
                "LABEL <label> ) | ( IBID <internal_backup_ID> ) ]"
            "@param LOG Only log backups will be used for the restore."
            "@param \"UNTIL <date> <time>\" The database will be restored "
                "to its status at date <date> and time <time>. If this option "
                "is not specified, the command autorecover restores the "
                "database to the last database state available through backups "
                "and log volumes."
            "@param \"KEY <key> LABEL <label>\" The the backup identified by "
                "its internal key <key> and its label <label> will be used as "
                "first backup during the recovery. The internal key and the "
                "label of a backup can be found in the backup history."
            "@param \"IBID <internal_backup_ID>\" The backup identified by the "
                "internal backup ID <internal_backup_ID> will be used as first "
                "backup during the recovery. The internal backup ID of a "
                "backup with internal key <key> and label <label> equals "
                "the character string \"<key>|<label>\"."
            "@reply "
                "OK<NL>"
                "Return code             <return_code><NL>"
                "Date                    [<date]<NL>"
                "Time                    [<time>]<NL>"
                "Server                  [<server>]<NL>"
                "Database                [<database>]<NL>"
                "Kernel Version          [<kernel_version>]<NL>"
                "Pages Transferred       [<pages_transferred>]<NL>"
                "Pages Left              [<pages left>]<NL>"
                "Volumes                 [<number_volumes>]<NL>"
                "Medianame               [<media_name>]<NL>"
                "Location                [<location>]<NL>"
                "Error text              [<error_text>]<NL>"
                "Label                   [<label>]<NL>"
                "Is Consistent           [<is_consistent>]<NL>"
                "First LOG Page          [<number_first_log_page>]<NL>"
                "Last LOG Page           [<number_last_log_page>]<NL>"
                "DB Stamp 1 Date         [<time_stamp_1_date>]<NL>"
                "DB Stamp 1 Time         [<time_stamp_1_time>]<NL>"
                "DB Stamp 2 Date         [<time_stamp_2_date>]<NL>"
                "DB Stamp 2 Time         [<time_stamp_2_time>]<NL>"
                "Page Count              [<page_count>]<NL>"
                "Devices Used            [<devices_used>]<NL>"
                "Database ID             [<database_ID>]<NL>"
                "Max Used Data Page      [<max_used_data_page>]<NL>"

                "@replyvalue <return_code> 0 if recovery was successful, an error code unequal to 0 otherwise"
                "@replyvalue <date>        Date of restore of last restored backup"
                "@replyvalue <time>        Time of restore of last restored backup"
                "@replyvalue <server>      Name of the database server"
                "@replyvalue <database>    Name of the database instance"
                "@replyvalue <kernel_version> Kernel version of the database"
                "@replyvalue <pages_transferred> Number of pages transferred during restore of last restored backup"
                "@replyvalue <pages left>  Number of pages still to be transferred of last restored backup"
                "@replyvalue <number_volumes> Number of backup media used during restore of last restored backup"
                "@replyvalue <media_name>  Name of the backup medium used during restore of last restored backup"
                "@replyvalue <location>    File or device name used during restore of last restored backup"
                "@replyvalue <error_text>  Error message text"
                "@replyvalue <label>       Label of the last restored backup"
                "@replyvalue <is_consistent> Only for data backup: last restored backup was internally consistent"
                "@replyvalue <number_first_log_page> For data backup: first page of log backup to be read. For log backup: first page saved in log."
                "@replyvalue <number_last_log_page> Only for log backup: last page saved in log"
                "@replyvalue <time_stamp_1_date> Date of time stamp for first page of log backup"
                "@replyvalue <time_stamp_1_time> Time of time stamp for first page of log backup"
                "@replyvalue <time_stamp_2_date> Date of time stamp for last page of log backup"
                "@replyvalue <time_stamp_2_time> Time of time stamp for last page of log backup"
                "@replyvalue <page_count>  Total number of pages contained in the last restored backup"
                "@replyvalue <devices_used> Number of backup devices used during restore of last restored backup"
                "@replyvalue <database_ID> Database ID used to identify data and log backups that belong together"
                "@replyvalue <max_used_data_page> Maximum page number (indication of minimum database size when backup is imported)";
    }

    static inline const char * getCommandName(){return "autorecover";} //!<needed to avoid object instantiation during search of right command class
};
#endif
