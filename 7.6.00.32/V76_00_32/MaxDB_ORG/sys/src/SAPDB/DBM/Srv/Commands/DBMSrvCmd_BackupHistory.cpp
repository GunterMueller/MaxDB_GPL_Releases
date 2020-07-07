/*!
  \file    DBMSrvCmd_BackupHistory.cpp
  \author  TiloH
  \ingroup DBMServer commands
  \brief   definition of a classes handling DBMServer
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

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------

#include "SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_HistorySession.hpp"
#include "SAPDB/DBM/Srv/Commands/DBMSrvCmd_BackupHistory.hpp"


//-----------------------------------------------------------------------------
// members of class DBMSrvCmd_BackupHistoryOpen
//-----------------------------------------------------------------------------

DBMSrvCmd_BackupHistoryOpen::DBMSrvCmd_BackupHistoryOpen()
    : DBMSrv_Command( DBMSrv_Command::KeyBackupHistoryOpen, false, DBMSrvCmd_BackupHistoryOpen::m_LongHelp) {}

const char * const DBMSrvCmd_BackupHistoryOpen::m_LongHelp=
    "@command backup_history_open With this command you fetch the current backup history into memory. "
        "The backup history is also updated with the current information from the previously utilized backup tools. "
        "To view the content, choose backup_history_list (displaying the backup history) or backup_history_listnext "
        "(scrolling through the backup history)."
    "@preconditions You have the DBM operator authorization Backup or DBInfoRead."
    "@syntax backup_history_open [-e]"
    "@param -e DBMServer also determines the status of the availability of the backups in previously used external backup tools."
    "@reply OK<NL>";

tcn00_Error DBMSrvCmd_BackupHistoryOpen::run(VControlDataT * vcontrol,
                                            CommandT      * command,
                                            DBMSrv_Reply& Reply) {

    tcn00_Error                  rc=OK_CN00; 
    DBMSrvBHist_HistorySession * HistorySession=0;

    rc=DBMSrvBHist_HistorySession::getGlobalSession(vcontrol, Reply, HistorySession);

    if(OK_CN00==rc)
    {
        DBMSrvBHist_HistorySession::freeGlobalSession(vcontrol);
        HistorySession=0; // (*pHistorySession) was freed one line above, so don't reference to it anymore
    }

    rc=DBMSrvBHist_HistorySession::allocateGlobalSession(vcontrol, Reply, HistorySession);

    if(OK_CN00==rc)
        rc=HistorySession->open(vcontrol, command, Reply);

    if(OK_CN00!=rc)
        DBMSrvBHist_HistorySession::freeGlobalSession(vcontrol);

    return rc;
}


//-----------------------------------------------------------------------------
// members of class DBMSrvCmd_BackupHistoryList
//-----------------------------------------------------------------------------

DBMSrvCmd_BackupHistoryList::DBMSrvCmd_BackupHistoryList()
    : DBMSrv_Command( DBMSrv_Command::KeyBackupHistoryList, false, DBMSrvCmd_BackupHistoryList::m_LongHelp ) {}

const char * const DBMSrvCmd_BackupHistoryList::m_LongHelp=
    "@command backup_history_list The command backup_history_list displays the content of the backup history. "
        "You can restrict or extend what is displayed using options. If you have not fetched the backup history "
        "with backup_history_open before, this is done omplicitly."
    "@preconditions You have the DBM operator authorization Backup or DBInfoRead. "
    "@syntax backup_history_list [-c <columns>] [-k <key>] [-l <label>] [-a <action>] [-r <restore_flag>] [-u <yyyymmddHHMMSS>] [-m] [-e] [-Inverted]"
    "@param \"-c <columns>\" With option -c <columns> you can restrict the number of columns to be displayed. "
        "If several columns are specified, you separate them by commas. "
        "The keywords for the individual columns are:"
        "@value KEY    \"SysKey\" The database key of the backup"
        "@value LABEL  \"Label\" The label of the backup"
        "@value ACTION \"Action\" The action of the backup"
        "@value STAMP1 \"\" The first time stamp of the backup"
        "@value STAMP2 \"\" The second time stamp of the backup"
        "@value START  \"Beginning\" The starting time of the backup"
        "@value STOP   \"End\" The end time of the backup"
        "@value FIRSTLOG \"\" The first log-IO-sequence number of the backup"
        "@value LASTLOG  \"\" The last log-IO-sequence number of the backup"
        "@value LOG    \"Log Required\" The column specifying if log backups are needed after restoring the backup"
        "@value MEDIA  \"Medium\" The medium name used during backup"
        "@value PAGES  \"Size\" The size of the backup in pages"
        "@value VOLUMES \"Volumes\" The number of parts of that backup"
        "@value RC     \"Result Code\" The database returncode of the backup"
        "@value ERROR  \"Result Text\" The error text of a backup"
    "@param \"-k <key>\" Only the line of the backup history that contains the key specified under <key> is displayed. "
        "This option cannot be used with the -r option."
    "@param \"-l <label>\" Only the lines of the backup history that contain the specified backup ID are displayed. "
        "This option cannot be used with the -r option."
    "@param \"-a <action>\" Only the lines of the backup history that correspond to the specified backup type are output."
        "This option cannot be used with the -r option."
    "@param \"-r <restore_flag>\" Only backups needed for a restore are displayed. The following values can be specified for "
        "<restore_flag>:"
        "@value * \"\" All data backups that are required to recover the database instance are displayed. If the log volume "
            "is intact, only the data backups that match the available log are displayed."
        "@value LAST \"\"Beginning with the last complete data backup, all backups that are required to recover the database "
            "instance are displayed."
        "@value <key> \"\" Beginning with the complete data backup specified in <key>, all backups that are required to recover "
            "the database instance are displayed."
    "@param \"-u <yyyymmddHHMMSS>\" Only those lines in the backup history that contain data that was saved in the database "
        "instance before the specified time <yyyymmddHHMMSS> are displayed. This option cannot be used with the -r option."
    "@param -m For each line that relates to a backup, information is supplied on the backup medium used."
    "@param -e For each line that relates to a backup, information is supplied on the relevant external backup ID."
    "@reply OK<NL>END|CONTINUE<NL><history_line><NL>[<media_line>]<NL>[<external_backup_id-line>]<NL>"
        "<history_line><NL>[<media_line>]<NL>[<external_backup_id-line>]<NL>..."
        "@replyvalue END The contents of the backup history have been displayed in full."
        "@replyvalue CONTINUE The backup history contains further entries that were not displayed due to the limited storage "
            "available for replies. You can display this data by using the backup_history_listnext command or you can close the backup history."
        "@replyvalue <history_line> Information on the completed backups. The individual columns are separated by |. A separate line is displayed for each backup."
        "@replyvalue <media_line>   Information on the medium used for the backup. The line is prefixed with an M: which, in turn, is followed by columns separated by |."
        "@replyvalue <external_backup_id_line> If the backup was created using an external backup tool, information on the external backup ID "
            "is provided here. You have to specify this information when restoring a database instance from an external backup tool."
            "The line is prefixed with E: which, in turn, is followed by columns separated with |.";

tcn00_Error DBMSrvCmd_BackupHistoryList::run(VControlDataT * vcontrol,
                                            CommandT      * command,
                                            DBMSrv_Reply& Reply) {

    tcn00_Error                  rc=OK_CN00;
    DBMSrvBHist_HistorySession * HistorySession=0;

    // get the global history session
    rc=DBMSrvBHist_HistorySession::getGlobalSession(vcontrol, Reply, HistorySession);

    //if there is no global history session, create one
    if(ERR_NOHISTORYSESSION_CN00==rc)
    {
        DBMSrvCmd_BackupHistoryOpen openCommand;

        rc=openCommand.run(vcontrol, command, Reply);

        if(OK_CN00==rc)
            rc=DBMSrvBHist_HistorySession::getGlobalSession(vcontrol, Reply, HistorySession);
    }

    if(OK_CN00==rc)
        rc=HistorySession->list(vcontrol, command, Reply);

    return rc;
}


//-----------------------------------------------------------------------------
// members of class DBMSrvCmd_BackupHistoryListNext
//-----------------------------------------------------------------------------
DBMSrvCmd_BackupHistoryListNext::DBMSrvCmd_BackupHistoryListNext()
    : DBMSrv_Command( DBMSrv_Command::KeyBackupHistoryListNext, false, DBMSrvCmd_BackupHistoryListNext::m_LongHelp ) {}

const char * const DBMSrvCmd_BackupHistoryListNext::m_LongHelp=
    "@command backup_history_listnext "
        "With backup_history_listnext you can display more of the backup history "
        "after you have started displaying it with the command backup_history_list. "
        "Options you specified for displaying the backup history remain active. "
        "If backup_history_list was not called before, backup_history_listnext starts "
        "displaying the backup history like a backup_history_list without any options."
    "@preconditions "
        "You have the DBM operator authorization Backup or DBInfoRead. "
        "You have opened a backup history session by backup_history_open or backup_history_list."
    "@syntax backup_history_listnext"
    "@reply "
        "OK<NL>"
    	"END|CONTINUE<NL>"
        "<history_line><NL>"
        "[<media_line>]<NL>"
        "[<external_backup_id_line>]<NL>"
        "<history_line><NL>"
        "[<media_line>]<NL>"
        "[<external_backup_id_line>]<NL>"
        "...."
        "@replyvalue END      The contents of the backup history have been transferred in full."
        "@replyvalue CONTINUE The backup history contains further entries that were not transferred "
            "due to the limited storage available for replies. You can display this data by entering "
            "the above command." 
        "@replyvalue <history_line> Information on the completed backups. The individual columns are "
            "separated by |. A separate line is displayed for each backup."
	    "@replyvalue <media_line> Information on the backup medium used for the backup. The line is prefixed "
            "with an M: which, in turn, is followed by columns separated by |."
        "@replyvalue <external_backup_id_line> If the backup was created using an external backup tool, "
            "information on the external backup ID is provided here. You have to specify the external backup ID "
            "when restoring a backup from an external backup tool. "
            "The line is prefixed with E: which, in turn, is followed by columns separated with |.";

tcn00_Error DBMSrvCmd_BackupHistoryListNext::run(VControlDataT * vcontrol,
                                                CommandT      * command,
                                                DBMSrv_Reply& Reply) {

    tcn00_Error                  rc=OK_CN00;
    DBMSrvBHist_HistorySession * HistorySession=0;

    // get the history session
    rc=DBMSrvBHist_HistorySession::getGlobalSession(vcontrol, Reply, HistorySession);

    if(OK_CN00==rc)
        rc=HistorySession->listNext(vcontrol, command, Reply);

    return rc;
}


//-----------------------------------------------------------------------------
// members of class DBMSrvCmd_BackupHistoryClose
//-----------------------------------------------------------------------------

DBMSrvCmd_BackupHistoryClose::DBMSrvCmd_BackupHistoryClose()
    : DBMSrv_Command( DBMSrv_Command::KeyBackupHistoryClose, false, DBMSrvCmd_BackupHistoryClose::m_LongHelp ) {}

const char * const DBMSrvCmd_BackupHistoryClose::m_LongHelp=
    "@command backup_history_close With backup_history_close you close the backup history session."
    "@preconditions "
        "You have the DBM operator authorization Backup or DBInfoRead. "
        "Needs a backup history session created by backup_history_open, or "
        "backup_history_list."
    "@syntax backup_history_close"
    "@reply OK<NL>";

tcn00_Error DBMSrvCmd_BackupHistoryClose::run(VControlDataT * vcontrol,
                                                     CommandT      * command,
                                                     DBMSrv_Reply& Reply ) {

    tcn00_Error                  rc=OK_CN00;
    DBMSrvBHist_HistorySession * HistorySession=0;

    rc=DBMSrvBHist_HistorySession::getGlobalSession(vcontrol, Reply, HistorySession);

    if(OK_CN00==rc)
    {
        HistorySession->close(vcontrol, command, Reply);
        DBMSrvBHist_HistorySession::freeGlobalSession(vcontrol);
    }

    return rc;
}


//-----------------------------------------------------------------------------
// members of class DBMSrvCmd_BackupHistoryDate
//-----------------------------------------------------------------------------
DBMSrvCmd_BackupHistoryDate::DBMSrvCmd_BackupHistoryDate()
    : DBMSrv_Command( DBMSrv_Command::KeyBackupHistoryDate, false, DBMSrvCmd_BackupHistoryDate::m_LongHelp ) {}

const char * const DBMSrvCmd_BackupHistoryDate::m_LongHelp=
    "@command backup_history_date With backup_history_date you can determine the date and time on "
        "which the backup history file was last changed."
    "@preconditions You have the DBM operator authorization Backup or DBInfoRead."
    "@syntax backup_history_date"
    "@reply OK<NL><yyyy><mm><dd><HH><MM><SS><NL>"
        "@replyvalue <yyyy> a four digit year number."
        "@replyvalue <mm> the month as two digit number."
        "@replyvalue <dd> the day as two digit number."
        "@replyvalue <HH> the hour as two digit number."
        "@replyvalue <MM> the minute as two digit number."
        "@replyvalue <SS> the second as two digit number.";

tcn00_Error DBMSrvCmd_BackupHistoryDate::run(VControlDataT * vcontrol,
                                                    CommandT      * command,
                                                    DBMSrv_Reply& Reply ) {

    DBMSrvBHist_HistorySession TempHistory;

    return TempHistory.date(vcontrol, command, Reply);
}
