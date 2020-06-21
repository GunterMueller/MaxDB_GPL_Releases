/*! 
  -----------------------------------------------------------------------------
  module: DBMCli_Common.hpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager Client Library
  description:  DBM-Server Session Class
  version:      7.3 and higher 
  -----------------------------------------------------------------------------
  Copyright (c) 2000-2005 SAP AG
  -----------------------------------------------------------------------------


    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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

#ifndef _DBMCLI_COMMON_HPP_
#define _DBMCLI_COMMON_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "Messages/Msg_List.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/

// stuff
#define DBMCLI_COMPID                   "DBMCli"
#define DBMCLI_NULLSTR                  '\0'

// dbmserver commands ...
#define DBMCLI_CMD_DBMVERSION           "dbm_version"

// ... inst_
#define DBMCLI_CMD_INSTENUM             "inst_enum"

// ... db_
#define DBMCLI_CMD_DBCREATE             "db_create"
#define DBMCLI_CMD_DBDROP               "db_drop"
#define DBMCLI_CMD_DBENUM               "db_enum"
#define DBMCLI_CMD_DBSTATE              "db_state"

#define DBMCLI_CMD_DBSTART              "db_start"
#define DBMCLI_CMD_DBSTOP               "db_stop" 

#define DBMCLI_CMD_DBWARM               "db_warm"
#define DBMCLI_CMD_DBCOLD               "db_cold"
#define DBMCLI_CMD_DBOFFLINE            "db_offline"
#define DBMCLI_CMD_DBRESTART            "db_restart"

#define DBMCLI_CMD_DBADDDEVICE          "db_adddevice"

#define DBMCLI_CMD_DBACTIVATE           "db_activate"
#define DBMCLI_CMD_DBACTIVATE_REQ       "db_activate_req"
#define DBMCLI_OPT_DBACTIVATE_RECOVER   "recover"

#define DBMCLI_OPT_KERNEL_FAST          "-fast"
#define DBMCLI_OPT_KERNEL_QUICK         "-quick"
#define DBMCLI_OPT_KERNEL_SLOW          "-slow"

#define DBMCLI_OPT_RESTART_UNTIL        "-u"
#define DBMCLI_OPT_SHUTDOWN_QUICK       "-immediate"

// ... sql_
#define DBMCLI_CMD_SQLCONNECT           "sql_connect"
#define DBMCLI_CMD_SQLEXECUTE           "sql_execute"
#define DBMCLI_CMD_SQLRELEASE           "sql_release"
#define DBMCLI_CMD_SQLUPDSTAT           "sql_updatestat"

// ... util_
#define DBMCLI_CMD_UTLCONNECT           "util_connect"
#define DBMCLI_CMD_UTLEXECUTE           "util_execute"
#define DBMCLI_CMD_UTLRELEASE           "util_release"
#define DBMCLI_CMD_UTILACTIVATE         "util_activate"

// ... info
#define DBMCLI_CMD_INFO                 "info"
#define DBMCLI_CMD_INFONEXT             "info_next"

#define DBMCLI_OPT_INFO_DEF             "-d"
#define DBMCLI_OPT_INFO_WHERE           "-w"

#define DBMCLI_OPT_INFO_STATE           "state"

// ... history
#define DBMCLI_CMD_HISTORYOPEN          "backup_history_open"
#define DBMCLI_CMD_HISTORYLIST          "backup_history_list"
#define DBMCLI_CMD_HISTORYNEXT          "backup_history_listnext"
#define DBMCLI_CMD_HISTORYCLOSE         "backup_history_close"
#define DBMCLI_CMD_HISTORYDATA          "backup_history_date"

#define DBMCLI_OPT_HISTORY_RESTORE      "-r"
#define DBMCLI_OPT_HISTORY_UNTIL        "-u"
#define DBMCLI_OPT_HISTORY_MEDIA        "-m"
#define DBMCLI_OPT_HISTORY_EXTERN       "-e"
#define DBMCLI_OPT_HISTORY_RESTALL      "*"
#define DBMCLI_OPT_HISTORY_RESTLAST     "LAST"
#define DBMCLI_OPT_HISTORY_RESTCONT     "CONTINUE"

// ... file
#define DBMCLI_CMD_FILEGETLIST          "file_getlist"
#define DBMCLI_CMD_FILEGETFIRST         "file_getfirst"
#define DBMCLI_CMD_FILEGETNEXT          "file_getnext"
#define DBMCLI_CMD_FILECLOSE            "file_close"
#define DBMCLI_CMD_DIAGLIST             "diag_histlist"
#define DBMCLI_CMD_DIAGPACK             "diag_pack"

// ... show
#define DBMCLI_CMD_SHOWLIST             "show_list"
#define DBMCLI_CMD_SHOW                 "show"
#define DBMCLI_CMD_SHOWNEXT             "show_next"

// ... param
#define DBMCLI_CMD_PARAM_INIT           "param_init"
#define DBMCLI_CMD_PARAM_COPY           "param_copy"

#define DBMCLI_CMD_PARAM_STARTSESSION   "param_startsession"
#define DBMCLI_CMD_PARAM_COMMITSESSION  "param_commitsession"
#define DBMCLI_CMD_PARAM_ABORTSESSION   "param_abortsession"

#define DBMCLI_CMD_PARAM_GETALL         "param_extgetall"
#define DBMCLI_CMD_PARAM_GETFULL        "param_getfull"
#define DBMCLI_CMD_PARAM_GETVALUE       "param_getvalue"
#define DBMCLI_CMD_PARAM_DIRECTGET      "param_directget"

#define DBMCLI_CMD_PARAM_PUT            "param_put"
#define DBMCLI_CMD_PARAM_PUTCONFIRM     "param_putconfirm"
#define DBMCLI_CMD_PARAM_CHECKALL       "param_checkall"

#define DBMCLI_CMD_PARAM_GETDEVSALL     "param_getdevsall"
#define DBMCLI_CMD_PARAM_GETDEVSPACE    "param_getdevspace"
#define DBMCLI_CMD_PARAM_ADDDEVSPACE    "param_adddevspace"
#define DBMCLI_CMD_PARAM_DELDEVSPACE    "param_deldevspace"

#define DBMCLI_OPT_PARAM_GROUP          "GROUP="
#define DBMCLI_OPT_PARAM_GENERAL        "GENERAL"
#define DBMCLI_OPT_PARAM_EXTENDED       "EXTENDED"
#define DBMCLI_OPT_PARAM_SUPPORT        "SUPPORT"

#define DBMCLI_CMD_PARAM_RMFILE         "param_rmfile"
#define DBMCLI_CMD_PARAM_RESTORE        "param_restore"

// ... medium

#define DBMCLI_CMD_MEDIUM_GET           "medium_get"
#define DBMCLI_CMD_MEDIUM_GETALL        "medium_getall"
#define DBMCLI_CMD_MEDIUM_PUT           "medium_put"
#define DBMCLI_CMD_MEDIUM_DELETE        "medium_delete"

// ... backup

#define DBMCLI_CMD_BACKUP_START         "backup_start"
#define DBMCLI_CMD_BACKUP_REPLACE       "backup_replace"
#define DBMCLI_CMD_BACKUP_IGNORE        "backup_ignore"
#define DBMCLI_CMD_BACKUP_CANCEL        "backup_cancel"
#define DBMCLI_CMD_BACKUP_STATE         "backup_state"

#define DBMCLI_CMD_BACKUP_START_REQ     "backup_req"
#define DBMCLI_CMD_BACKUP_REPLACE_REQ   "backup_replace_req"
#define DBMCLI_CMD_BACKUP_IGNORE_REQ    "backup_ignore_req"
#define DBMCLI_CMD_BACKUP_CANCEL_REQ    "backup_cancel_req"

#define DBMCLI_CMD_BACKUP_REPLY_AVAIL   "backup_reply_available"
#define DBMCLI_CMD_BACKUP_REPLY_RECEIVE "backup_reply_receive"

#define DBMCLI_OPT_BACKUP_RECOVERY      "RECOVERY"
#define DBMCLI_OPT_BACKUP_MIGRATION     "MIGRATION"

#define DBMCLI_OPT_BACKUP_DATA          "DATA"
#define DBMCLI_OPT_BACKUP_PAGES         "PAGES"
#define DBMCLI_OPT_BACKUP_LOG           "LOG"

// ... autolog

#define DBMCLI_CMD_AUTOLOG_SHOW         "autolog_show"
#define DBMCLI_CMD_AUTOLOG_ON           "autolog_on"
#define DBMCLI_CMD_AUTOLOG_OFF          "autolog_off"
#define DBMCLI_CMD_AUTOLOG_CANCEL       "autolog_cancel"

// ... recover 

#define DBMCLI_CMD_RECOVER_START         "recover_start"
#define DBMCLI_CMD_RECOVER_START_REQ     "recover_req"
#define DBMCLI_CMD_RECOVER_CHECK         "recover_check"
#define DBMCLI_CMD_RECOVER_CHECK_REQ     "recover_check_req"
#define DBMCLI_CMD_RECOVER_REPLACE       "recover_replace"
#define DBMCLI_CMD_RECOVER_REPLACE_REQ   "recover_replace_req"
#define DBMCLI_CMD_RECOVER_IGNORE        "recover_ignore"
#define DBMCLI_CMD_RECOVER_IGNORE_REQ    "recover_ignore_req"
#define DBMCLI_CMD_RECOVER_CANCEL        "recover_cancel"
#define DBMCLI_CMD_RECOVER_CANCEL_REQ    "recover_cancel_req"

#define DBMCLI_CMD_RECOVER_STATE         "recover_state"
#define DBMCLI_CMD_RECOVER_REPLY_AVAIL   DBMCLI_CMD_BACKUP_REPLY_AVAIL
#define DBMCLI_CMD_RECOVER_REPLY_RECEIVE DBMCLI_CMD_BACKUP_REPLY_RECEIVE

#define DBMCLI_CMD_RECOVER_CONFIG        "recover_config" 
#define DBMCLI_CMD_RECOVER_DEVSPACE      "recover_devspace"

#define DBMCLI_OPT_RECOVER_DATA           DBMCLI_OPT_BACKUP_DATA          
#define DBMCLI_OPT_RECOVER_PAGES          DBMCLI_OPT_BACKUP_PAGES         
#define DBMCLI_OPT_RECOVER_LOG            DBMCLI_OPT_BACKUP_LOG           
#define DBMCLI_OPT_RECOVER_UNTIL          "UNTIL"
#define DBMCLI_OPT_RECOVER_EBID           "ExternalBackupID"

//

#define DBMCLI_CMD_RESTARTINFO            "db_restartinfo"

//

#define DBMCLI_CMD_LOADSYSTAB           "load_systab"

// ... service
#define DBMCLI_CMD_SRVCONNECT           "service_connect"
#define DBMCLI_CMD_SRVRELEASE           "service_release"

// ... trace
#define DBMCLI_CMD_TRACE_PROTOPT        "trace_protopt"
#define DBMCLI_CMD_TRACE_PROT           "trace_prot"

#define DBMCLI_CMD_TRACE_CLEAR          "trace_clear"
#define DBMCLI_CMD_TRACE_FLUSH          "trace_flush"
#define DBMCLI_CMD_TRACE_OFF            "trace_off"
#define DBMCLI_CMD_TRACE_ON             "trace_on"
#define DBMCLI_CMD_TRACE_SHOW           "trace_show"

// ... user
#define DBMCLI_CMD_USER_GET             "user_get"
#define DBMCLI_CMD_USER_GETALL          "user_getall"
#define DBMCLI_CMD_USER_GETRIGHTS       "user_getrights"
#define DBMCLI_CMD_USER_PUT             "user_put"
#define DBMCLI_CMD_USER_CREATE          "user_create"
#define DBMCLI_CMD_USER_DELETE          "user_delete"

//

#define DBMCLI_VAL_OK                   "OK"
#define DBMCLI_VAL_ERR                  "ERR"

//

#define DBMCLI_VAL_END                  "END"
#define DBMCLI_VAL_CONTINUE             "CONTINUE"

//

#define DBMCLI_VAL_TRUE                 "TRUE"
#define DBMCLI_VAL_FALSE                "FALSE"
 
#define DBMCLI_VAL_YES                  "YES" 
#define DBMCLI_VAL_NO                   "NO" 

#define DBMCLI_VAL_ON                   "ON" 
#define DBMCLI_VAL_OFF                  "OFF" 

//

#define DBMCLI_SRV_RC_NOMEDIA           -24986
#define DBMCLI_SRV_RC_XPCHECK           -24973
#define DBMCLI_SRV_RC_SQL               -24988
#define DBMCLI_SRV_RC_NOREPLY           -24928
               
#define DBMCLI_DB_RC_INVFILE            -3004
#define DBMCLI_DB_RC_INVFILE2           -903

//

#define DBMCLI_CFGKEY_WEBDBMRECOVERY          "WEBDBMRECOVERY"
#define DBMCLI_CFGVAL_WEBDBMRECOVERY_INSTALL  "INSTALL"

// Parameters

#define PARAM_PARAM_CHANGED             "__PARAM_CHANGED___"
#define PARAM_LOG_MODE                  "LOG_MODE"

// string functions

#if defined (_WIN32)
  #define strncasecmp  _strnicmp
  #define strcasecmp   _stricmp
#else
  #include <strings.h>
#endif


// Define assertion macros for debug version
// 
// ASSERT_VALID( pObject ): Checks the class invariant of pObject by calling
//                          the AssertValid member function of pObject.
//                          It does nothing in Release-Versions.
//
// ASSERT( f ): Works like the assert makro. But if it was called from within
//              ASSERT_VALID it tells you on which line ASSERT_VALID was
//              called. It does nothing in Release-Versions.

#ifdef NDEBUG
  #define ASSERT( exp )
#else
  #define ASSERT( exp ) \
    assert( exp )
#endif

#endif // _DBMCLI_COMMON_HPP_


