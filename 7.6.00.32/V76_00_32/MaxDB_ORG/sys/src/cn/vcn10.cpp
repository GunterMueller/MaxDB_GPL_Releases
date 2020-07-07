/*!
  -----------------------------------------------------------------------------
  module: vcn10.cpp
  -----------------------------------------------------------------------------

  responsible:  BerndV

  special area: DBMServer

  description:  "main" file for DBMServer

                implementing ctrlservopen()
                             ctrlservclose()
                             ctrlservcommand()

  -----------------------------------------------------------------------------


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



  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
/***** Memory Debug BEGIN *****/
#ifdef _WIN32
//  #define DBMMEMDEBUG        1
#endif
#ifdef DBMMEMDEBUG
  #pragma message ("Memory Debug ON")
  #define _CRTDBG_MAP_ALLOC 1
  #define _DEBUG            1
#endif
/***** Memory Debug END *****/

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "vsp001.h"
#include "vsp009c.h"

#include "hsp100.h"
#include "heo02.h"
#include "heo06.h"

#include "hcn10.h"
#include "hcn20.h"
#include "hcn30.h"
#include "hcn31.h"
#include "hcn32.h"
#include "hcn40.h"
#include "hcn41.h"
#include "hcn42.h"
#include "hcn43.h"
#include "hcn45.h"
#include "hcn46.h"
#include "hcn50.h"
#include "hcn51.h"
#include "hcn80.h"
#include "hcn81.h"
#include "hcn83.h"
#include "hcn90.h"

#include "DBM/Srv/Framework/DBMSrvFrm_Server.hpp"
#include "DBM/Srv/HSS/DBMSrvHSS_Nodes.hpp"
#include "DBM/Srv/SharedMemory/DBMSrvShM_ShmAdmin.hpp"
#include "DBM/Srv/SharedMemory/DBMSrvShM_ShmMigrator.hpp"
#include "DBM/Logging/DBMLog_Logger.hpp"
#include "DBM/Logging/DBMLog_LogFile.hpp"
#include "DBM/Logging/DBMLog_SynchronizedLogFile.hpp"
#include "DBM/Srv/Infrastructure/Reply/DBMSrvInfrReply_Reply2MsgList.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_RTEError.hpp"

#include "DBM/Srv/BackupHistory/DBMSrvBHist_HistorySession.hpp"
#include "DBM/Srv/BackupHistory/DBMSrvBHist_RestorePlanner.hpp"

#include "DBM/Srv/Commands/Medium/DBMSrvCmdMedium_MediumDelete.hpp"
#include "DBM/Srv/Commands/Medium/DBMSrvCmdMedium_MediumPut.hpp"

#include "DBM/Srv/Infrastructure/Commands/DBMSrvInfrCmd_CommandFactory.hpp"
#include "SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_GlobalSessions.hpp"

/***** Memory Debug BEGIN *****/
#ifdef DBMMEMDEBUG
  #include <crtdbg.h>
  _CrtMemState s1, s2, s3;
  HANDLE hFile;
#endif
/***** Memory Debug END *****/

/*!
  -----------------------------------------------------------------------------
  Chapter: Private types
  -----------------------------------------------------------------------------
 */

/*! Declaration: DBMServer  obsolete command structure */
typedef struct DBMServerObsoleteCommand {
  const char               * pOldName;
  const char               * pNewName;
} DBMServerObsoleteCommand;
/*! EndDeclaration: DBMServer obsolete command structure */

/*!
  -----------------------------------------------------------------------------
  EndChapter: Private types
  -----------------------------------------------------------------------------
 */

/*!
  -----------------------------------------------------------------------------
  Chapter: Private macros
  -----------------------------------------------------------------------------
 */

/*! Declaration: DBMServer command protocol values */
#define CMD_PROT_NO            (-1)
#define CMD_PROT_ALL           (-2)
#define CMD_PROT_NOERR         (-3)
#define CMD_PROT_YES           (0)
/*! EndDeclaration: DBMServer command protocol values */

/*! Declaration: DBName  values */
#define DBNAME_YES      1
#define DBNAME_NO       2
#define DBNAME_EGAL     3
/*! EndDeclaration: DBMServer command protocol values */

#ifndef DBROOT_ENV_VAR
  #define DBROOT_ENV_VAR         "DBROOT"
#endif

const int CommandT::InvalidIndex = -1;

/*!
  -----------------------------------------------------------------------------
  EndChapter: Private macros
  -----------------------------------------------------------------------------
 */

/*!
  -----------------------------------------------------------------------------
  Chapter: Private functions
  -----------------------------------------------------------------------------
 */

/*!
  -------------------------------------------------------------------------
  function:     cn10_AnalyzeCommand
  -------------------------------------------------------------------------
  description:  DBMServer command analyzer

                This function searches in an array for the properties
                of the specified DBMServer command.

  arguments:    szData     [INOUT] - buffer with the command and parameters
                nDataLen   [IN]    - the name of the database
                DBMCommand [INOUT] - structur for the analyzed command

  -------------------------------------------------------------------------
*/
static void cn10_AnalyzeCommand
      ( const char  * szData,
        const int      nDataLen,
        CommandT     & DBMCommand );

/*!
  -------------------------------------------------------------------------
  function:     cn10_HSSExecute
  -------------------------------------------------------------------------
  description:  Hot Standby Execute

                Here will be executed the hss_execute

  arguments:    vcontrol    [INOUT]  - DBMServer session data
                command     [IN]     - DBMServer command
                replyData   [OUT]    - reply buffer
                replyLen    [OUT]    - lenght of reply
                replyLenMax [IN]     - max. length of reply buffer

  return value: error code specified in gcn00c.h

  -------------------------------------------------------------------------
*/
static tcn00_Error cn10_HSSExecute (
    VControlDataT * vcontrol,
    CommandT      * command,
    char          * replyData,
    int           * replyLen,
    int             replyLenMax);

/*!
  -------------------------------------------------------------------------
  function:     cn10_LogCommand
  -------------------------------------------------------------------------
  description:  Command log

                This functions logs the specified command.

  arguments:    command     [IN]     - DBMServer command

  -------------------------------------------------------------------------
*/
void cn10_LogCommand (
    CommandT      * command );

/*!
  -------------------------------------------------------------------------
  function:     cn10_LogError
  -------------------------------------------------------------------------
  description:  Error log

                This functions logs the specified error list.

  arguments:    listToLog   [IN]     - error list to be logged

  -------------------------------------------------------------------------
*/
void cn10_LogError (
      const DBMSrvMsg_Error& listToLog );

/*!
  -----------------------------------------------------------------------------
  EndChapter: Private functions
  -----------------------------------------------------------------------------
 */
static DBMServerObsoleteCommand obsoletesC[] = {
  {"autosave_on"       , "autolog_on"},
  {NULL                , NULL        }
};

static cn10DBMServerCommand namesC[] = {
// name                        Logon  Sys    Static DBNameState   Min  Max  Protocol
//                             Function
//                             Short Help
//                             command key
//                             HSS    DBMRights
//! \see hcn10.h
{"apropos"                    ,false, false, false, DBNAME_EGAL,   1,   1, CMD_PROT_NO
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyApropos
                              ,true  ,DBMMaskNothing_CN50 },
{"archive_stage"              ,true , false, false, DBNAME_YES ,   2,   6, CMD_PROT_ALL
                              ,cn31BackupStage
                              ,"<medium> <stage> [verify|noverify] [remove|keep]\n                         [FileNumberList <list>]"
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"archive_stage_repeat"       ,true , false, false, DBNAME_YES ,   1,   3, CMD_PROT_ALL
                              ,cn31BackupStageRepeat
                              ,"<medium> [verify|noverify] [remove|keep]"
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"auto_extend"                ,true , false, false, DBNAME_YES ,   1,   2, CMD_PROT_ALL
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyAutoExtend
                              ,true , DBMRightAccessUtility_CN50 | DBMRightParamFull_CN50},
{"auto_update_statistics"     ,true , false, false, DBNAME_YES ,   1,   1, CMD_PROT_ALL
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyAutoUpdateStatistics
                              ,true , DBMRightAccessSQL_CN50},
{"autolog_cancel"             ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_YES
                              ,cn31AutosaveCancel
                              ,"                                                  "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"autosave_cancel"            ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_YES
                              ,cn31AutosaveCancel
 /*** obsolete ***/           ,"(obsolete version of autolog_cancel)              "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"autolog_off"                ,true , false, false, DBNAME_YES ,   0 ,   0, CMD_PROT_YES
                              ,cn31AutosaveEnd
                              ,"                                                  "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"autosave_off"               ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_YES
                              ,cn31AutosaveEnd
 /*** obsolete ***/           ,"(obsolete version of autolog_off)                 "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"autolog_on"                 ,true , false, false, DBNAME_YES ,   0,   1, CMD_PROT_YES
                              ,cn31AutosaveOn
                              ,"[<medium>]                                        "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"autosave_on"                ,true , false, false, DBNAME_YES ,   0,   1, CMD_PROT_YES
                              ,cn31AutosaveOn
 /*** obsolete ***/           ,"(obsolete version of autolog_on)                  "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"autolog_show"               ,true , false, false, DBNAME_YES ,   0 ,   0, CMD_PROT_NO
                              ,cn31AutosaveShow
                              ,"                                                  "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 | DBMRightDBInfoRead_CN50 },
{"autorecover"                ,true , false, false, DBNAME_YES ,   0,   8, CMD_PROT_ALL
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyUsingCommandFactory
                              ,true , DBMRightBackupRestore_CN50 },
{"autosave_show"              ,true , false, false, DBNAME_YES ,   0 ,   0, CMD_PROT_NO
                              ,cn31AutosaveShow
 /*** obsolete ***/           ,"(obsolete version of autolog_show)                "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 | DBMRightDBInfoRead_CN50 },
{""                           ,true , false, false, DBNAME_EGAL ,   0,   0, CMD_PROT_NO
                              ,NULL
                              ,"                                                  "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,false, DBMMaskNothing_CN50 },
{"backup_cancel"              ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_ALL
                              ,cn31BackupSaveCancel
                              ,"                            (needs backup session)"
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"backup_save_cancel"         ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_ALL
                              ,cn31BackupSaveCancel
 /*** obsolete ***/           ,"(obsolete version of autolog_on)                  "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"backup_cancel_req"          ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_ALL
                              ,cn31BackupSaveCancelRequest
                              ,NULL
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"backup_save_cancel_req"     ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_ALL
                              ,cn31BackupSaveCancelRequest
 /*** obsolete ***/           ,NULL
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"backup_command"             ,true , false, false, DBNAME_YES ,   1,   1, CMD_PROT_ALL
                              ,cn31BackupCommand
                              ,NULL
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"backup_command_req"         ,true , false, false, DBNAME_YES ,   1,   1, CMD_PROT_ALL
                              ,cn31BackupCommandRequest
                              ,NULL
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"backup_ext_ids_forget"      ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_ALL
                              ,cn32ForgetEBIDs
                              ,"                                                  "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 | DBMRightDBInfoRead_CN50 },
{"backup_ext_ids_get"         ,true , false, false, DBNAME_YES ,   1,   3, CMD_PROT_ALL
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyBackupExtIdsGet
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 | DBMRightDBInfoRead_CN50 },
{"backup_ext_ids_getfromtool" ,true , false, false, DBNAME_YES ,   1,   3, CMD_PROT_ALL
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyBackupExtIdsGetFromTool
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 | DBMRightDBInfoRead_CN50 },
{"backup_ext_ids_list"        ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_ALL
                              ,cn32ListNextEBIDs
                              ,"                                                  "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 | DBMRightDBInfoRead_CN50 },
{"backup_ext_ids_listnext"    ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_ALL
                              ,cn32ListNextEBIDs
                              ,"                                                  "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 | DBMRightDBInfoRead_CN50 },

{"backup_history_append"      ,true , false, false, DBNAME_YES ,   1,   256, CMD_PROT_ALL
                              ,0
                              ,0 //hidden command
                              ,DBMSrv_Command::KeyBackupHistoryAppend
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },

{"backup_history_close"       ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_NO
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyBackupHistoryClose
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 | DBMRightDBInfoRead_CN50 },
{"backup_history_date"        ,true , false, false, DBNAME_YES ,   0,   0,CMD_PROT_NO
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyBackupHistoryDate
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 | DBMRightDBInfoRead_CN50 },
{"backup_history_list"        ,true , false, false, DBNAME_YES ,   0,  10,CMD_PROT_NO
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyBackupHistoryList
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 | DBMRightDBInfoRead_CN50 },
{"backup_history_listnext"    ,true , false, false, DBNAME_YES ,   0,   0,CMD_PROT_NO
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyBackupHistoryListNext
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 | DBMRightDBInfoRead_CN50 },
{"backup_history_open"        ,true , false, false, DBNAME_YES ,   0,   1, CMD_PROT_NO
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyBackupHistoryOpen
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 | DBMRightDBInfoRead_CN50 },
{"backup_ignore"              ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_ALL
                              ,cn31BackupSaveIgnore
                              ,"                            (needs backup session)"
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"backup_save_ignore"         ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_ALL
                              ,cn31BackupSaveIgnore
 /*** obsolete ***/           ,"(obsolete version of backup_ignore)               "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"backup_ignore_req"          ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_ALL
                              ,cn31BackupSaveIgnoreRequest
                              ,NULL
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"backup_save_ignore_req"     ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_ALL
                              ,cn31BackupSaveIgnoreRequest
 /*** obsolete ***/           ,NULL
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"backup_replace"             ,true , false, false, DBNAME_YES ,   1,   2, CMD_PROT_ALL
                              ,cn31BackupSaveReplace
                              ,"<medium> [<loc>]            (needs backup session)"
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"backup_save_replace"        ,true , false, false, DBNAME_YES ,   1,   2, CMD_PROT_ALL
                              ,cn31BackupSaveReplace
 /*** obsolete ***/           ,"(obsolete version of backup_replace)              "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"backup_replace_req"         ,true , false, false, DBNAME_YES ,   1,   2, CMD_PROT_ALL
                              ,cn31BackupSaveReplaceRequest
                              ,NULL
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"backup_save_replace_req"    ,true , false, false, DBNAME_YES ,   1,   2, CMD_PROT_ALL
                              ,cn31BackupSaveReplaceRequest
 /*** obsolete ***/           ,NULL
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"backup_reply_available"     ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_NO
                              ,cn31BackupReplyAvailable
                              ,NULL
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"backup_reply_receive"       ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_ALL
                              ,cn31BackupReplyReceive
                              ,NULL
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"backup_req"                 ,true , false, false, DBNAME_YES ,   1,   3, CMD_PROT_ALL
                              ,cn31BackupSaveRequest
                              ,NULL
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"backup_save_req"            ,true , false, false, DBNAME_YES ,   1,   3, CMD_PROT_ALL
                              ,cn31BackupSaveRequest
 /*** obsolete ***/           ,NULL
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"backup_stage"               ,true , false, false, DBNAME_YES ,   2,   6, CMD_PROT_ALL
                              ,cn31BackupStage
                              ,"(obsolete version of archive_stage)     "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"backup_stage_repeat"        ,true , false, false, DBNAME_YES ,   1,   3, CMD_PROT_ALL
                              ,cn31BackupStageRepeat
                              ,"(obsolete version of archive_stage_repeat)     "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"backup_start"               ,true , false, false, DBNAME_YES ,   1,   4, CMD_PROT_ALL
                              ,cn31BackupSave
                              ,"<medium> [RECOVERY|MIGRATION] [DATA|PAGES|LOG]    \n                         [AUTOIGNORE]                  (needs util session)"
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"backup_save"                ,true , false, false, DBNAME_YES ,   1,   4, CMD_PROT_ALL
                              ,cn31BackupSave
 /*** obsolete ***/           ,"(obsolete version of backup_start)                "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"backup_state"               ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_NO
                              ,cn31BackupSaveState
                              ,"                            (needs backup session)"
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"backup_save_state"          ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_NO
                              ,cn31BackupSaveState
 /*** obsolete ***/           ,"(obsolete version of backup_state)                "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"bye"                        ,false, false, false, DBNAME_EGAL,   0,   0, CMD_PROT_NO
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyUsingCommandFactory
                              ,true  ,DBMMaskNothing_CN50 },
{""                           ,true , false, false, DBNAME_EGAL,   0,   0, CMD_PROT_NO
                              ,NULL
                              ,"                                                  "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true  ,DBMMaskNothing_CN50 },
{"db_activate"                ,true , false, false, DBNAME_YES ,   0,   9, CMD_PROT_YES
                              ,cn80DBActivate
                              ,"<sysdba>,<pwd> | RECOVER <recoveroptions>         "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightAccessUtility_CN50 },
{"db_activate_req"            ,true , false, false, DBNAME_YES ,   0,   9, CMD_PROT_YES
                              ,cn80DBActivateRequest
                              ,0
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightAccessUtility_CN50 },
{"db_addvolume"               ,true , false, false, DBNAME_YES ,   1,   6, CMD_PROT_ALL
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyUsingCommandFactory
                              ,true , DBMRightAccessUtility_CN50 | DBMRightParamFull_CN50},
{"db_adddevspace"             ,true , false, false, DBNAME_YES ,   4,   6, CMD_PROT_ALL
                              ,cn40DbAddDevice
 /*** obsolete ***/           ,"(obsolete version of db_addvolume)                "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightAccessUtility_CN50 | DBMRightParamFull_CN50},
{"db_adddevice"               ,true , false, false, DBNAME_YES ,   4,   6, CMD_PROT_ALL
                              ,cn40DbAddDevice
 /*** obsolete ***/           ,"(obsolete version of db_addvolume)                "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightAccessUtility_CN50 | DBMRightParamFull_CN50},
{"db_admin"                   ,true , false, false, DBNAME_YES ,   0,   2, CMD_PROT_ALL
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyUsingCommandFactory
                              ,true , DBMRightDBStop_CN50 },
{"db_clear"                   ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_ALL
                              ,cn40DBClear
                              ,"                                    (needs dbname)"
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightDBStop_CN50 },
{"db_cold"                    ,true , false, false, DBNAME_YES ,   0,   2, CMD_PROT_ALL
                              ,0
/*** obsolete ***/            ,0
                              ,DBMSrv_Command::KeyUsingCommandFactory
                              ,true , DBMRightDBStop_CN50 },
{"dbcold"                     ,true , false, false, DBNAME_YES ,   0,   1, CMD_PROT_ALL
                              ,0
 /*** obsolete ***/           ,0
                              ,DBMSrv_Command::KeyUsingCommandFactory
                              ,true , DBMRightDBStop_CN50 },
{"db_connect"                 ,true , false, false, DBNAME_YES ,   0,   2, CMD_PROT_NO
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyUsingCommandFactory
                              ,true , DBMRightAccessSQL_CN50 | DBMRightAccessUtility_CN50 | DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50},
{"db_cons"                    ,true , false, false, DBNAME_YES ,   1,  -1, CMD_PROT_NO
                              ,cn40DBConsole
                              ,"<console command>                                 "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightDBStop_CN50 },
{"db_create"                  ,false, false, false, DBNAME_NO  ,   2,   7, 1
                              ,cn40DBCreate
                              ,"[-a] [-u] [-g <supportgroup>] <dbname> <usr>,<pwd> [<osusr>,<pwd>]"
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true  ,DBMMaskNothing_CN50 },
{"db_deletevolume"            ,true , false, false, DBNAME_YES ,   0,   3, CMD_PROT_ALL
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyUsingCommandFactory
                              ,true , DBMRightAccessUtility_CN50 | DBMRightParamFull_CN50},
{"db_drop"                    ,true , false, true , DBNAME_YES ,   0,   1, CMD_PROT_NO
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyUsingCommandFactory
                              ,true , DBMRightInstallMgm_CN50 },
{"db_enum"                    ,false, false, false, DBNAME_EGAL,   0,   1, CMD_PROT_NO
                              ,cn40DBEnum
                              ,"[-s]                                              "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true  ,DBMMaskNothing_CN50 },
{"enum_dbs"                   ,false, false, false, DBNAME_EGAL,   0,   1, CMD_PROT_NO
                              ,cn40DBEnum
 /*** obsolete ***/           ,"(obsolete version of db_enum)                     "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true  ,DBMMaskNothing_CN50 },
{"db_execute"                 ,true , false, false, DBNAME_YES ,   0,  -1, 2
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyUsingCommandFactory
                              ,true , DBMRightAccessSQL_CN50 | DBMRightAccessUtility_CN50 },
{"db_executenice"             ,true , false, false, DBNAME_YES ,   0,  -1 ,          2
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyUsingCommandFactory
                              ,true , DBMRightAccessSQL_CN50 | DBMRightAccessUtility_CN50 },
{"db_fetch"                   ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_NOERR
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyUsingCommandFactory
                              ,true , DBMRightAccessSQL_CN50 | DBMRightAccessUtility_CN50 },
{"db_fetchnice"               ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_NOERR
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyUsingCommandFactory
                              ,true , DBMRightAccessSQL_CN50 | DBMRightAccessUtility_CN50 },
{"db_migratecatalog"          ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_ALL
                              ,cn45MigrateCatalog
                              ,"                                                  "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightAccessUtility_CN50},
{"db_offline"                 ,true , false, false, DBNAME_YES ,   0,   2, CMD_PROT_YES
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyUsingCommandFactory
                              ,true , DBMRightDBStop_CN50 },
{"dboffline"                  ,true , false, false, DBNAME_YES ,   0,   2, CMD_PROT_ALL
                              ,0
 /*** obsolete ***/           ,0
                              ,DBMSrv_Command::KeyUsingCommandFactory
                              ,true , DBMRightDBStop_CN50 },
{"db_online"                  ,true , false, false, DBNAME_YES ,   0,   4, CMD_PROT_ALL
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyUsingCommandFactory
                              ,true , DBMRightDBStart_CN50 },
{"db_reg"                     ,true , false, false, DBNAME_EGAL,   0,  11, 1
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyUsingCommandFactory
                              ,true , DBMRightInstallMgm_CN50 },
{"db_register"                ,true , false, false, DBNAME_EGAL,   0,  11, 1
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyUsingCommandFactory
                              ,true , DBMRightInstallMgm_CN50 },
{"db_reinstall"               ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_ALL
                              ,cn45Reinstall
                              ,"                                                  "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightAccessUtility_CN50},
{"db_release"                 ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_NO
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyUsingCommandFactory
                              ,true , DBMRightAccessSQL_CN50 | DBMRightAccessUtility_CN50 | DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50},
{"db_unreg"                   ,true , false, false, DBNAME_YES ,   0,   0, 1
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyUsingCommandFactory
                              ,true , DBMRightInstallMgm_CN50 },
{"db_warm"                    ,true , false, false, DBNAME_YES ,   0,   4, CMD_PROT_ALL
                              ,0
 /*** obsolete ***/           ,0
                              ,DBMSrv_Command::KeyUsingCommandFactory
                              ,true , DBMRightDBStart_CN50 },
{"dbwarm"                     ,true , false, false, DBNAME_YES ,   0,   4, CMD_PROT_ALL
                              ,0
 /*** obsolete ***/           ,0
                              ,DBMSrv_Command::KeyUsingCommandFactory
                              ,true , DBMRightDBStart_CN50 },
{"db_restart"                 ,true , false, false, DBNAME_YES ,   0,   6, CMD_PROT_ALL
                              ,cn40DBRestart
                              ,"[(-f|-s|-t)] [-i] [-d] [-u <yyyymmdd> <hhmmss>]   "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightDBStop_CN50 },
{"dbrestart"                  ,true , false, false, DBNAME_YES ,   0,   6, CMD_PROT_ALL
                              ,cn40DBRestart
 /*** obsolete ***/           ,"(obsolete version of db_restart)                   "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightDBStop_CN50 },
{"db_restartinfo"             ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_NO
                              ,cn31BackupRestartInfo
                              ,"                              (needs util session)"
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 },
{"backup_restart_info"        ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_NO
                              ,cn31BackupRestartInfo
 /*** obsolete ***/           ,"(obsolete version of db_restartinfo)              "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 },
{"db_speed"                   ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_NO
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyDbSpeed
                              ,true , DBMRightDBInfoRead_CN50 },
{"db_standby"                 ,true , false, false, DBNAME_YES ,   1,   1, CMD_PROT_ALL
                              ,cn46DBStandby
                              ,"<node>                                            "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightDBStart_CN50 },
{"db_start"                   ,true , false, false, DBNAME_YES ,   0,   1, CMD_PROT_ALL
                              ,cn40DBStart
                              ,"[(-fast|-slow|-test)]               (needs dbname)"
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightDBStart_CN50 },
{"start"                      ,true , false, false, DBNAME_YES ,   0,   1, CMD_PROT_ALL
                              ,cn40DBStart
 /*** obsolete ***/           ,"(obsolete version of db_start)                     "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightDBStart_CN50 },
{"db_state"                   ,true , false, false, DBNAME_YES ,   0,   1, CMD_PROT_NO
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyDbState
                              ,true , DBMRightDBInfoRead_CN50 },
{"db_stop"                    ,true , false, false, DBNAME_YES ,   0,   1, CMD_PROT_ALL
                              ,cn40DBStop
                              ,"[-dump]                             (needs dbname)"
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightDBStop_CN50 },
{"stop"                       ,true , false, false, DBNAME_YES ,   0,   1, CMD_PROT_ALL
                              ,cn40DBStop
 /*** obsolete ***/           ,"(obsolete version of db_stop)                      "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightDBStop_CN50 },
{"dban_delete"                ,true , false, false, DBNAME_YES ,   1,   1, CMD_PROT_NO
                              ,cn43DBanalyzerDelete
                              ,"                                                  "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightDBInfoRead_CN50 },
{"dban_start"                 ,true , false, false, DBNAME_YES ,   0,  -1, CMD_PROT_YES
                              ,cn43DBanalyzerStart
                              ,"                                                  "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightDBInfoRead_CN50 },
{"dban_state"                 ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_NOERR
                              ,cn43DBanalyzerState
                              ,"                                                  "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightDBInfoRead_CN50 },
{"dban_stop"                  ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_YES
                              ,cn43DBanalyzerStop
                              ,"                                                  "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightDBInfoRead_CN50 },
{"dbm_configget"              ,true , false, false, DBNAME_YES ,   0,   2, CMD_PROT_NO
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyUsingCommandFactory
                              ,true , DBMRightInstallMgm_CN50 },
{"dbm_configset"              ,true , false, false, DBNAME_YES ,   1,   3, 1
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyUsingCommandFactory
                              ,true , DBMRightInstallMgm_CN50 },
{"dbm_getpath"                ,false, false, true , DBNAME_EGAL,   1,   1, CMD_PROT_NO
                              ,cn40DBMGetPath
                              ,"[IndepDataPath|IndepProgPath]                     "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true  ,DBMMaskNothing_CN50 },
{"dbm_getpid"                 ,false , false, false, DBNAME_EGAL,   0,   0, CMD_PROT_NOERR
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyDbmGetpid
                              ,true  ,DBMMaskNothing_CN50 },
{"dbm_getmemoryusage"         ,false , false, false, DBNAME_EGAL,   0,   0, CMD_PROT_NOERR
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyUsingCommandFactory
                              ,true  ,DBMMaskNothing_CN50 },
{"dbm_next"                   ,false, false, false, DBNAME_EGAL ,  0 ,   0, CMD_PROT_NO , cn40DBMNext
                              ,NULL
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true  ,DBMMaskNothing_CN50 },
{"dbm_setpath"                ,false, false, true , DBNAME_EGAL,   2,   2, CMD_PROT_NO
                              ,cn40DBMSetPath
                              ,"[IndepDataPath|IndepProgPath] <path>              "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true  ,DBMMaskNothing_CN50 },
{"dbm_shm_info"               ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_NOERR
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyDbmShmInfo
                              ,true , DBMRightShMAdmin_CN50 },
{"dbm_shm_reset"              ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_NOERR
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyDbmShmReset
                              ,true , DBMRightShMAdmin_CN50 },
{"dbm_shm_unlock"             ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_NOERR
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyDbmShmUnlock
                              ,true , DBMRightShMAdmin_CN50 },
{"dbm_version"                ,false, false, true , DBNAME_EGAL,   0,   1, CMD_PROT_NO
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyDbmVersion
                              ,true  ,DBMMaskNothing_CN50 },
{"dbm_execute_serverprocess"  ,true, false, false , DBNAME_YES,   1,   64, CMD_PROT_NOERR
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyDbmExecuteServerprocess
                              ,true  ,DBMMaskNothing_CN50 },
{"dbm_list_memoryerror"       ,false, false, false , DBNAME_EGAL,  0,    0, CMD_PROT_NOERR
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyDbmListMemoryerror
                              ,true  ,DBMMaskNothing_CN50 },
{"dbmevtdisp_internal_add"    ,true, false, false , DBNAME_YES,    6,   6, CMD_PROT_NOERR
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyUsingCommandFactory
                              ,true  ,DBMMaskNothing_CN50 },
{"dbmevtdisp_internal_communication",true, false, false , DBNAME_YES,    1,  10, CMD_PROT_NOERR
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyUsingCommandFactory
                              ,true  ,DBMMaskNothing_CN50 },
{"dbmevtdisp_internal_delete" ,true, false, false , DBNAME_YES,    1,  1, CMD_PROT_NOERR
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyUsingCommandFactory
                              ,true  ,DBMMaskNothing_CN50 },
{"diag_histlist"              ,true , false, false, DBNAME_YES ,   0,   1, CMD_PROT_NO
                              ,cn42DiagHistList
                              ,"                                    (needs dbname)"
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightDBFileRead_CN50 },
{"diag_pack"                  ,true , false, false, DBNAME_YES ,   0,   3, CMD_PROT_NO
                              ,cn42DiagPack
                              ,"<content spec>                      (needs dbname)"
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightDBFileRead_CN50 },
{""                           ,true , false, false, DBNAME_EGAL,   0,   0, CMD_PROT_NO
                              ,NULL
                              ,"                                                  "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true  ,DBMMaskNothing_CN50 },
{"event_available"            ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_NOERR
                              ,NULL
                              ,NULL
                              ,DBMSrv_Command::KeyEventAvailable
                              ,true , DBMRightDBInfoRead_CN50 },
{"event_delete"               ,true , false, false, DBNAME_YES ,   1,   2, CMD_PROT_ALL
                              ,NULL
                              ,NULL
                              ,DBMSrv_Command::KeyEventDelete
                              ,true , DBMRightDBInfoRead_CN50 },
{"event_list"                 ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_NO
                              ,NULL
                              ,NULL
                              ,DBMSrv_Command::KeyEventList
                              ,true , DBMRightDBInfoRead_CN50 },
{"event_list_categories"      ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_NO
                              ,NULL
                              ,NULL
                              ,DBMSrv_Command::KeyEventListCategories
                              ,true , DBMRightDBInfoRead_CN50 },
{"event_receive"              ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_NOERR
                              ,NULL
                              ,NULL
                              ,DBMSrv_Command::KeyEventReceive
                              ,true , DBMRightDBInfoRead_CN50 },
{"event_release"              ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_NO
                              ,NULL
                              ,NULL
                              ,DBMSrv_Command::KeyEventRelease
                              ,true , DBMRightDBInfoRead_CN50 },
{"event_set"                  ,true , false, false, DBNAME_YES ,   2,   3, CMD_PROT_ALL
                              ,NULL
                              ,NULL
                              ,DBMSrv_Command::KeyEventSet
                              ,true , DBMRightDBInfoRead_CN50 },
{"event_wait"                 ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_NO
                              ,NULL
                              ,NULL
                              ,DBMSrv_Command::KeyEventWait
                              ,true , DBMRightDBInfoRead_CN50 },
{"exec_lcinit"                ,true , false, false, DBNAME_YES ,   0,  -1, CMD_PROT_YES
                              ,cn40ExecLcInit
                              ,"                                                  "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightSystemCmd_CN50 },
{"exec_nipingsrv"             ,true , false, false, DBNAME_YES ,   0,   1, CMD_PROT_NO
                              ,cn40ExecNiPingSrv
                              ,"[<idletime>]                                      "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightSystemCmd_CN50 },
{"exec_sdbinfo"               ,false, false, false, DBNAME_EGAL,   0,  -1, CMD_PROT_NO
                              ,cn40ExecSDBInfo
                              ,"[<parameters>]                                    "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true  ,DBMMaskNothing_CN50 },
{"exec_xpu"                   ,true , false, false, DBNAME_YES ,   0,  -1, CMD_PROT_YES
                              ,cn40ExecXPU
                              ,"                                                  "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightAccessSQL_CN50 },
{"exit"                       ,false, false, false, DBNAME_EGAL,   0,   0, CMD_PROT_NO
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyUsingCommandFactory
                              ,true  ,DBMMaskNothing_CN50 },
{"explain"                    ,false, false, false, DBNAME_EGAL,   1,   4, CMD_PROT_NO
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyExplain
                              ,true  ,DBMMaskNothing_CN50 },
{""                           ,true , false, false, DBNAME_EGAL,   0,   0, CMD_PROT_NO
                              ,NULL
                              ,"                                                  "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true  ,DBMMaskNothing_CN50 },
{"file_backup"                ,true , false, false, DBNAME_YES ,   1,   1, CMD_PROT_NO
                              ,cn42FileBackup
                              ,"<fileid>                            (needs dbname)"
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightDBFileRead_CN50 },
{"file_getfirst"              ,true , false, false, DBNAME_YES ,   1,   3, CMD_PROT_NO
                              ,cn42FileGet
                              ,"<fileid> [LINE=<n>|DATE=<yyyymmdd>] (needs dbname)"
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightDBFileRead_CN50 },
{"file_getlist"               ,true , false, false, DBNAME_YES ,   0,   1, CMD_PROT_NO
                              ,cn42FileGetList
                              ,"[<showlevel>]                       (needs dbname)"
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightDBFileRead_CN50 },
{"file_getnext"               ,true , false, false, DBNAME_YES ,   2,   2, CMD_PROT_NO
                              ,cn42FileGetNext
                              ,"<fileid> <file handle>              (needs dbname)"
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightDBFileRead_CN50 },
{"file_operation"             ,true , false, false, DBNAME_YES ,   2,   3, CMD_PROT_ALL
                              ,cn42FileOperation
                              ,"<fileid> OP=<operid> [DATE=<date>|LINE=<line>]    "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightDBFileRead_CN50 },
{"file_restore"               ,true , false, false, DBNAME_YES ,   1,   1, CMD_PROT_NO
                              ,cn42FileRestore
                              ,"<fileid>                            (needs dbname)"
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightDBFileRead_CN50 },
{"get_default"                ,false, false, false, DBNAME_EGAL,   2,   2, CMD_PROT_NO
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyGetDefault
                              ,true  ,DBMMaskNothing_CN50 },
{""                           ,true , false, false, DBNAME_EGAL,   0,   0, CMD_PROT_NO
                              ,NULL
                              ,"                                                  "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true  ,DBMMaskNothing_CN50 },
{"help"                       ,false, false, false, DBNAME_EGAL,   0,   2, CMD_PROT_NO
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyHelp
                              ,true  ,DBMMaskNothing_CN50 },
{"hss_addstandby"             ,true , false, false, DBNAME_YES ,   2,   4, 1
                              ,cn46AddStandby
                              ,"<node> login=<osusr>,<pwd> [path=<instroot>     \n                         delay=<delay>]"
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,false,DBMRightInstallMgm_CN50 },
{"hss_copyfile"               ,true, false, true , DBNAME_YES,     0,   2, CMD_PROT_ALL
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyHssCopyFile
                              ,true  ,DBMRightInstallMgm_CN50 },
{"hss_enable"                 ,true , false, false, DBNAME_YES ,   0,   2, CMD_PROT_ALL
                              ,cn46Enable
                              ,"[node=<officialnode>] [lib=<hsslibrary>]          "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,false,DBMRightInstallMgm_CN50 },
{"hss_execute"                ,false, false, false, DBNAME_YES ,   2,  -1, 2
                              ,cn10_HSSExecute
                              ,"<node> <dbmcommand> [<parameters>]                "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,false  ,DBMMaskNothing_CN50 },
{"hss_getnodes"               ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_NO
                              ,cn46GetNodes
                              ,"                                                  "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightDBInfoRead_CN50  },
{"hss_removestandby"          ,true , false, false, DBNAME_YES ,   1,   1, 1
                              ,cn46RemoveStandby
                              ,"<node>                                            "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,false,DBMRightInstallMgm_CN50 },
{""                           ,true , false, false, DBNAME_EGAL,   0,   0, CMD_PROT_NO
                              ,NULL
                              ,"                                                  "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true  ,DBMMaskNothing_CN50 },
{"info"                       ,true , false, false, DBNAME_YES ,   1,  -1, CMD_PROT_NO
                              ,cn81Info
                              ,"<name>                                            "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightDBInfoRead_CN50 },
{"info_next"                  ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_NO
                              ,cn81InfoNext
                              ,"                                                  "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightDBInfoRead_CN50 },
{"inst_enum"                  ,false, false, false, DBNAME_EGAL,   0,   1, CMD_PROT_NO
                              ,cn40InstEnum
                              ,"[-k]                                              "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true  ,DBMMaskNothing_CN50 },
{"installation_enum"          ,false, false, false, DBNAME_EGAL,   0,   1, CMD_PROT_NO
                              ,cn40InstEnum
 /*** obsolete ***/           ,"(obsolete version of inst_enum)                   "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true  ,DBMMaskNothing_CN50 },
{"enum_installations"         ,false, false, false, DBNAME_EGAL,   0,   1, CMD_PROT_NO
                              ,cn40InstEnum
 /*** obsolete ***/           ,"(obsolete version of inst_enum)                   "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true  ,DBMMaskNothing_CN50 },
{"inst_info"                  ,false, false, true , DBNAME_EGAL,   0,   2, CMD_PROT_NO
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyInstInfo
                              ,true  ,DBMMaskNothing_CN50 },
{"inst_reg"                   ,false, true , false, DBNAME_NO,     0,   5, CMD_PROT_ALL
                              ,cn40InstReg
                              ,"[<dbroot>] [[-v]  <version>] [-k <key>]           "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true  ,DBMMaskNothing_CN50 },
{"installation_register"      ,false, true , false, DBNAME_NO,     0,   5, CMD_PROT_ALL
                              ,cn40InstReg
 /*** obsolete ***/           ,"(obsolete version of inst_reg)                    "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true  ,DBMMaskNothing_CN50 },
{"register_installation"      ,false, true , false, DBNAME_NO,     0,   5, CMD_PROT_ALL
                              ,cn40InstReg
 /*** obsolete ***/           ,"(obsolete version of inst_reg)                    "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true  ,DBMMaskNothing_CN50 },
{"inst_unreg"                 ,false, true , false, DBNAME_NO,     0,   1, CMD_PROT_ALL
                              ,cn40InstUnreg
                              ,"[<dbroot>]                                        "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true  ,DBMMaskNothing_CN50 },
{"installation_unregister"    ,false, true , false, DBNAME_NO,     0,   1, CMD_PROT_ALL
                              ,cn40InstUnreg
 /*** obsolete ***/           ,"(obsolete version of inst_unreg)                  "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true  ,DBMMaskNothing_CN50 },
{"unregister_installation"    ,false, true , false, DBNAME_NO,     0,   1, CMD_PROT_ALL
                              ,cn40InstUnreg
 /*** obsolete ***/           ,"(obsolete version of inst_unreg)                  "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true  ,DBMMaskNothing_CN50 },
{""                           ,true , false, false, DBNAME_EGAL,   0,   0, CMD_PROT_NO
                              ,NULL
                              ,"                                                  "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true  ,DBMMaskNothing_CN50 },
{"load_lcapps"                ,true , false, false, DBNAME_YES ,   0,   1, CMD_PROT_YES
                              ,cn40LoadLcApps
                              ,"[<sapuser>,<pw>]                    (needs dbname)"
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightExecLoad_CN50 | DBMRightLoadSysTab_CN50},
{"load_r3tab"                 ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_YES
                              ,cn40LoadR3Tab
                              ,"                                    (needs dbname)"
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightLoadSysTab_CN50 | DBMRightExecLoad_CN50},
{"load_systab"                ,true , false, false, DBNAME_YES ,   0,   4, CMD_PROT_YES
                              ,cn40LoadSysTab
                              ,"[-u <user>,<pwd>]                   (needs dbname)"
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightLoadSysTab_CN50 | DBMRightExecLoad_CN50},
{"load_tutorial"              ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_NO
                              ,cn40LoadTutorial
                              ,"                                    (needs dbname)"
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightExecLoad_CN50 | DBMRightLoadSysTab_CN50},
{""                           ,true , false, false, DBNAME_EGAL,   0,   0, CMD_PROT_NO
                              ,NULL
                              ,"                                                  "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true  ,DBMMaskNothing_CN50 },
{"medium_date"                ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_NO
                              ,cn30BackupMediaDate
                              ,"                                                  "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 | DBMRightDBInfoRead_CN50 },
{"backup_media_date"          ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_NO
                              ,cn30BackupMediaDate
 /*** obsolete ***/           ,"(obsolete version of medium_date)                 "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 | DBMRightDBInfoRead_CN50 },
{"medium_delete"              ,true , false, false, DBNAME_YES ,   1,   1, CMD_PROT_ALL
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyMediumDelete
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"medium_delete_nodistribute" ,true , false, false, DBNAME_YES ,   1,   1, CMD_PROT_ALL
                              ,0 
                              ,0
                              ,DBMSrv_Command::KeyMediumDeleteNoDistribute
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"backup_media_delete"        ,true , false, false, DBNAME_YES ,   1,   1, CMD_PROT_ALL
                              ,DBMSrvCmdMedium_MediumDelete::runCommand
 /*** obsolete ***/           ,"(obsolete version of medium_delete)               "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"medium_get"                 ,true , false, false, DBNAME_YES ,   1,   1, CMD_PROT_NO
                              ,cn30BackupMediaGet
                              ,"<name>                                            "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 | DBMRightDBInfoRead_CN50 },
{"backup_media_get"           ,true , false, false, DBNAME_YES ,   1,   1, CMD_PROT_NO
                              ,cn30BackupMediaGet
 /*** obsolete ***/           ,"(obsolete version of medium_get)                  "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 | DBMRightDBInfoRead_CN50 },
{"medium_getall"              ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_NO
                              ,cn30BackupMediaGetAll
                              ,"                                                  "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 | DBMRightDBInfoRead_CN50 },
{"backup_media_getall"        ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_NO
                              ,cn30BackupMediaGetAll
 /*** obsolete ***/           ,"(obsolete version of medium_getall)               "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 | DBMRightDBInfoRead_CN50 },
{"medium_label"               ,true , false, false, DBNAME_YES ,   1,   2, CMD_PROT_NO
                              ,cn31BackupLabel
                              ,"<medium> [<nnn>]              (needs util session)"
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"backup_label"               ,true , false, false, DBNAME_YES ,   1,   2, CMD_PROT_NO
                              ,cn31BackupLabel
 /*** obsolete ***/           ,"(obsolete version of medium_label)                "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"medium_labeloffline"        ,true , true, false, DBNAME_EGAL ,   1,   5, CMD_PROT_NO
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyUsingCommandFactory
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"backup_labeloffline"        ,true , false, false, DBNAME_YES ,   1,   2, CMD_PROT_NO
                              ,0
 /*** obsolete ***/           ,0
                              ,DBMSrv_Command::KeyUsingCommandFactory
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"medium_put"                 ,true , false, false, DBNAME_YES ,   2,  10, CMD_PROT_ALL
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyMediumPut
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"medium_put_nodistribute"    ,true , false, false, DBNAME_YES ,   2,  10, CMD_PROT_ALL
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyMediumPutNoDistribute
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"backup_media_put"           ,true , false, false, DBNAME_YES ,   2,  10, CMD_PROT_ALL
                              ,DBMSrvCmdMedium_MediumPut::runCommand
 /*** obsolete ***/           ,"(obsolete version of medium_put)                  "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"medium_migrate"             ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_NO
                              ,cn30BackupMediaMigrate
                              ,NULL
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 | DBMRightDBInfoRead_CN50 },
{""                           ,true , false, false, DBNAME_EGAL,   0,   0, CMD_PROT_NO
                              ,NULL
                              ,"                                                  "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true  ,DBMMaskNothing_CN50 },
{"package_info"               ,false, false, false, DBNAME_EGAL,   1,   1, CMD_PROT_NO
                              ,cn40ExecSDBInst
                              ,"<directory>                                         "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true  ,DBMMaskNothing_CN50 },
{"param_abortsession"         ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_NO
                              ,cn20ParamAbortSession
                              ,"                             (needs param session)"
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightParamFull_CN50 | DBMRightParamCheckWrite_CN50 | DBMRightParamRead_CN50 },
{"param_addvolume"            ,true , false, false, DBNAME_YES ,   4,   5, CMD_PROT_ALL
                              ,cn20ParamAddDevSpace
                              ,"<num> <mod> <nam> <typ> <siz>       (needs dbname)"
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightParamFull_CN50 | DBMRightParamCheckWrite_CN50 },
{"param_adddevspace"          ,true , false, false, DBNAME_YES ,   4,   5, CMD_PROT_ALL
                              ,cn20ParamAddDevSpace
 /*** obsolete ***/           ,"(obsolete version of param_addvolume)             "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightParamFull_CN50 | DBMRightParamCheckWrite_CN50 },
{"param_checkall"             ,true , false, false, DBNAME_YES ,   0,   1, CMD_PROT_ALL
                              ,cn20ParamCheckAll
                              ,"[<mode>]                            (needs dbname)"
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightParamFull_CN50 | DBMRightParamCheckWrite_CN50 },
{"param_commitsession"        ,true , false, false, DBNAME_YES ,   0,   1, CMD_PROT_ALL
                              ,cn20ParamCommitSession
                              ,"                             (needs param session)"
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightParamFull_CN50 | DBMRightParamCheckWrite_CN50 },
{"param_copy"                 ,true , false, false, DBNAME_YES ,   1,   1, CMD_PROT_NO
                              ,cn20ParamCopy
                              ,"<sourcedb>                   (needs param session)"
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightInstallMgm_CN50 },
{"param_delvolume"            ,true , false, false, DBNAME_YES ,   2,   2, CMD_PROT_ALL
                              ,cn20ParamDelDevSpace
                              ,NULL
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightParamFull_CN50 | DBMRightParamCheckWrite_CN50 },
{"param_deldevspace"          ,true , false, false, DBNAME_YES ,   2,   2, CMD_PROT_ALL
                              ,cn20ParamDelDevSpace
                              ,NULL
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightParamFull_CN50 | DBMRightParamCheckWrite_CN50 },
{"param_directdel"            ,true , false, false, DBNAME_YES ,   1,   2, CMD_PROT_ALL
                              ,cn20ParamDirectDel
                              ,"<keyname>                                         "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightParamFull_CN50 },
{"param_directget"            ,true , false, false, DBNAME_YES ,   1,   1, CMD_PROT_NO
                              ,cn20ParamDirectGet
                              ,"<keyname>                                         "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightParamFull_CN50 | DBMRightParamRead_CN50 },
{"param_directgetall"         ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_NO
                              ,cn20ParamDirectGetAll
                              ,"                                                  "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightParamFull_CN50 | DBMRightParamRead_CN50 },
{"param_directgetallnext"     ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_NO
                              ,cn20ParamDirectGetAllNext
                              ,"                                                  "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightParamFull_CN50 | DBMRightParamRead_CN50 },
{"param_directput"            ,true , false, false, DBNAME_YES ,   2,   3, CMD_PROT_ALL
                              ,cn20ParamDirectPut
                              ,"<keyname> <value> [<comment>]                     "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightParamFull_CN50 },
{"param_extget"               ,true , false, false, DBNAME_YES ,   1,   1, CMD_PROT_NO
                              ,cn20ParamExtGet
                              ,"<keyname>                                         "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightParamFull_CN50 | DBMRightParamCheckWrite_CN50 | DBMRightParamRead_CN50 },
{"param_extgetall"            ,true , false, false, DBNAME_YES ,   0,  -1, CMD_PROT_NO
                              ,cn20ParamExtGetAll
                              ,"[<property>=<value>]                              "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightParamFull_CN50 | DBMRightParamCheckWrite_CN50 | DBMRightParamRead_CN50 },
{"param_extgetallnext"        ,true , false, false, DBNAME_YES ,   0,  -1, CMD_PROT_NO
                              ,cn20ParamExtGetAllNext
                              ,"[<property>=<value>]                              "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightParamFull_CN50 | DBMRightParamCheckWrite_CN50 | DBMRightParamRead_CN50 },
{"param_getdefault"           ,true , false, false, DBNAME_YES ,   1,   1, CMD_PROT_NO
                              ,cn20ParamGetDefault
                              ,"<keyname>                                         "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightParamFull_CN50 | DBMRightParamCheckWrite_CN50 | DBMRightParamRead_CN50 },
{"param_getexplain"           ,true , false, false, DBNAME_YES ,   1,   1, CMD_PROT_NO
                              ,cn20ParamGetExplain
                              ,"<keyname>                                         "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightParamFull_CN50 | DBMRightParamCheckWrite_CN50 | DBMRightParamRead_CN50 },
{"param_getfull"              ,true , false, false, DBNAME_YES ,   1,   1, CMD_PROT_NO
                              ,cn20ParamGetFull
                              ,"<keyname>                                         "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightParamFull_CN50 | DBMRightParamCheckWrite_CN50 | DBMRightParamRead_CN50 },
{"param_gethelp"              ,true , false, false, DBNAME_YES ,   1,   1, CMD_PROT_NO
                              ,cn20ParamGetHelp
                              ,"<keyname>                                         "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightParamFull_CN50 | DBMRightParamCheckWrite_CN50 | DBMRightParamRead_CN50 },
{"param_gethistory"           ,true , false, false, DBNAME_YES ,   0,   5, CMD_PROT_NO
                              ,cn20ParamHistory
                              ,"                                                  "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightParamFull_CN50 | DBMRightParamCheckWrite_CN50 | DBMRightParamRead_CN50 },
{"param_gethistorynext"       ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_NO
                              ,cn20ParamHistoryNext
                              ,"                                                  "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightParamFull_CN50 | DBMRightParamCheckWrite_CN50 | DBMRightParamRead_CN50 },
{"param_getproperties"        ,true , false, false, DBNAME_YES ,   1,   1, CMD_PROT_NO
                              ,cn20ParamGetProperties
                              ,"<keyname>                                         "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightParamFull_CN50 | DBMRightParamCheckWrite_CN50 | DBMRightParamRead_CN50 },
{"param_gettype"              ,true , false, false, DBNAME_YES ,   1,   1, CMD_PROT_NO
                              ,cn20ParamGetType
                              ,"<keyname>                                         "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightParamFull_CN50 | DBMRightParamCheckWrite_CN50 | DBMRightParamRead_CN50 },
{"param_getvalue"             ,true , false, false, DBNAME_YES ,   1,   1, CMD_PROT_NO
                              ,cn20ParamGetValue
                              ,"<keyname>                                         "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightParamFull_CN50 | DBMRightParamCheckWrite_CN50 | DBMRightParamRead_CN50 },
{"param_getvolsall"           ,true , false, false, DBNAME_YES ,   0,   1, CMD_PROT_NO
                              ,cn20ParamGetDevSpaceAll
                              ,"[<mod>]                                           "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightParamFull_CN50 | DBMRightParamCheckWrite_CN50 | DBMRightParamRead_CN50 },
{"param_getdevsall"           ,true , false, false, DBNAME_YES ,   0,   1, CMD_PROT_NO
                              ,cn20ParamGetDevSpaceAll
 /*** obsolete ***/           ,"(obsolete version of param_getvolsall)            "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightParamFull_CN50 | DBMRightParamCheckWrite_CN50 | DBMRightParamRead_CN50 },
{"param_getvolume"            ,true , false, false, DBNAME_YES ,   2,   2, CMD_PROT_NO
                              ,cn20ParamGetDevSpace
                              ,"<num> <mod>                                       "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightParamFull_CN50 | DBMRightParamCheckWrite_CN50 | DBMRightParamRead_CN50 },
{"param_getdevspace"          ,true , false, false, DBNAME_YES ,   2,   2, CMD_PROT_NO
                              ,cn20ParamGetDevSpace
 /*** obsolete ***/           ,"(obsolete version of param_getvolume)             "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightParamFull_CN50 | DBMRightParamCheckWrite_CN50 | DBMRightParamRead_CN50 },
{"param_init"                 ,true , false, false, DBNAME_YES ,   0,   1, CMD_PROT_NO
                              ,cn20ParamInit
                              ,"[<instance type>]            (needs param session)"
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightInstallMgm_CN50 },
{"param_put"                  ,true , false, false, DBNAME_YES ,   2,   5, CMD_PROT_ALL
                              ,cn20ParamPut
                              ,"[-running] [-permanent] <keyname> <value> [<comment>]"
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightParamFull_CN50 | DBMRightParamCheckWrite_CN50 },
{"param_restore"              ,true , false, false, DBNAME_YES ,   1,   1, CMD_PROT_ALL
                              ,cn20ParamRestore
                              ,"<version number>                    (needs dbname)"
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightParamFull_CN50 | DBMRightParamCheckWrite_CN50 },
{"param_rmfile"               ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_ALL
                              ,cn20ParamRmFile
                              ,"                                    (needs dbname)"
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightInstallMgm_CN50 },
{"param_startsession"         ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_NO
                              ,cn20ParamStartSession
                              ,"                                    (needs dbname)"
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightParamFull_CN50 | DBMRightParamCheckWrite_CN50 | DBMRightParamRead_CN50 },
{"param_versions"             ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_NO
                              ,cn20ParamVersions
                              ,"                                    (needs dbname)"
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightParamFull_CN50 | DBMRightParamCheckWrite_CN50 | DBMRightParamRead_CN50 },
{""                           ,true , false, false, DBNAME_EGAL,   0,   0, CMD_PROT_NO
                              ,NULL
                              ,"                                                  "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true  ,DBMMaskNothing_CN50 },
{"quit"                       ,false, false, false, DBNAME_EGAL,   0,   0, CMD_PROT_NO
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyUsingCommandFactory
                              ,true  ,DBMMaskNothing_CN50 },
{""                           ,true , false, false, DBNAME_EGAL,   0,   0, CMD_PROT_NO
                              ,NULL
                              ,"                                                  "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true ,DBMMaskNothing_CN50 },
{"recover_req"                ,true , false, false, DBNAME_YES ,   1,   8, CMD_PROT_ALL
                              ,cn31BackupRestoreRequest
                              ,NULL
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 },
{"backup_restore_req"         ,true , false, false, DBNAME_YES ,   1,   8, CMD_PROT_ALL
                              ,cn31BackupRestoreRequest
 /*** obsolete ***/           ,NULL
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 },
{"recover_cancel"             ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_ALL
                              ,cn31BackupRestoreCancel
                              ,"                            (needs backup session)"
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 },
{"backup_restore_cancel"      ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_ALL
                              ,cn31BackupRestoreCancel
 /*** obsolete ***/           ,"(obsolete version of recover_cancel)              "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 },
{"recover_cancel_req"         ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_ALL
                              ,cn31BackupRestoreCancelRequest
                              ,NULL
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 },
{"backup_restore_cancel_req"  ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_ALL
                              ,cn31BackupRestoreCancelRequest
 /*** obsolete ***/           ,NULL
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 },
{"recover_check"              ,true , false, false, DBNAME_YES ,   1,   10, CMD_PROT_ALL
                              ,cn31BackupRestoreCheck
                              ,"<medium> <savetype> [ExternalBackupID <ext_backup_ID>]\n                         [<nnn>] [UNTIL <date> <time>] [LABEL <label>]        \n                                                    (needs service session)"
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"backup_restore_check"       ,true , false, false, DBNAME_YES ,   1,   8, CMD_PROT_ALL
                              ,cn31BackupRestoreCheck
 /*** obsolete ***/           ,"(obsolete version of recover_check)               "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"recover_check_req"          ,true , false, false, DBNAME_YES ,   1,   8, CMD_PROT_ALL
                              ,cn31BackupRestoreCheckRequest
                              ,NULL
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"backup_restore_check_req"   ,true , false, false, DBNAME_YES ,   1,   8, CMD_PROT_ALL
                              ,cn31BackupRestoreCheckRequest
 /*** obsolete ***/           ,NULL
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"recover_config"             ,true , false, false, DBNAME_YES ,   1,   1, CMD_PROT_ALL
                              ,cn31BackupRestoreConfig
                              ,"<medium>                   (needs service session)"
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 },
{"backup_restore_config"      ,true , false, false, DBNAME_YES ,   1,   1, CMD_PROT_ALL
                              ,cn31BackupRestoreConfig
 /*** obsolete ***/           ,"(obsolete version of recover_config)              "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 },
{"recover_ignore"             ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_ALL
                              ,cn31BackupRestoreIgnore
                              ,"                            (needs backup session)"
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"backup_restore_ignore"      ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_ALL
                              ,cn31BackupRestoreIgnore
 /*** obsolete ***/           ,"(obsolete version of recover_ignore)              "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"recover_ignore_req"         ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_ALL
                              ,cn31BackupRestoreIgnoreRequest
                              ,NULL
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"backup_restore_ignore_req"  ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_ALL
                              ,cn31BackupRestoreIgnoreRequest
 /*** obsolete ***/           ,NULL
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"recover_replace"            ,true , false, false, DBNAME_YES ,   1,   4, CMD_PROT_ALL
                              ,cn31BackupRestoreReplace
                              ,"<medium> [ExternalBackupID <ext_backup_ID>]\n                         [<loc> [<nnn>]]    (needs backup session)"
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"backup_restore_replace"     ,true , false, false, DBNAME_YES ,   1,   4, CMD_PROT_ALL
                              ,cn31BackupRestoreReplace
 /*** obsolete ***/           ,"(obsolete version of recover_replace)             "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"recover_replace_req"        ,true , false, false, DBNAME_YES ,   1,   3, CMD_PROT_ALL
                              ,cn31BackupRestoreReplaceRequest
                              ,NULL
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"backup_restore_replace_req" ,true , false, false, DBNAME_YES ,   1,   3, CMD_PROT_ALL
                              ,cn31BackupRestoreReplaceRequest
 /*** obsolete ***/           ,NULL
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"recover_start"              ,true , false, false, DBNAME_YES ,   1,   11, CMD_PROT_ALL
                              ,cn31BackupRestore
                              ,"<medium> <savetype> [ExternalBackupID <ext_backup_ID>]\n                         [<nnn>] [UNTIL <date> <time>] [AUTOIGNORE]\n                         [LABEL <label>]               (needs util session)"
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 },
{"backup_restore"             ,true , false, false, DBNAME_YES ,   1,   11, CMD_PROT_ALL
                              ,cn31BackupRestore
 /*** obsolete ***/           ,"(obsolete version of recover_start)               "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 },
{"recover_state"              ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_NO
                              ,cn31BackupRestoreState
                              ,"                            (needs backup session)"
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 },
{"backup_restore_state"       ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_NO
                              ,cn31BackupRestoreState
 /*** obsolete ***/           ,"(obsolete version of revocer_state)               "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 },
{"recover_state_check"        ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_NO
                              ,cn31BackupRestoreStateCheck
                              ,"                           (needs backup session)"
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"backup_restore_state_check" ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_NO
                              ,cn31BackupRestoreStateCheck
 /*** obsolete ***/           ,"(obsolete version of recover_state_check)         "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"recover_volume"             ,true , false, false, DBNAME_YES ,   1,   1, CMD_PROT_ALL
                              ,cn31BackupRestoreDevspace
                              ,"<devicename>                  (needs util session)"
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 },
{"recover_devspace"           ,true , false, false, DBNAME_YES ,   1,   1, CMD_PROT_ALL
                              ,cn31BackupRestoreDevspace
 /*** obsolete ***/           ,"(obsolete version of recover_volume)              "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 },
{"backup_restore_devspace"    ,true , false, false, DBNAME_YES ,   1,   1, CMD_PROT_ALL
                              ,cn31BackupRestoreDevspace
 /*** obsolete ***/           ,"(obsolete version of recover_volume)              "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 },
{"release"                    ,false, false, false, DBNAME_EGAL,   0,   0, CMD_PROT_NO
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyUsingCommandFactory
                              ,true  ,DBMMaskNothing_CN50 },
{""                           ,true , false, false, DBNAME_EGAL,   0,   0, CMD_PROT_NO
                              ,NULL
                              ,"                                                  "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true  ,DBMMaskNothing_CN50 },
{"scheduler_activate_job"     ,true , false, false, DBNAME_YES ,   1,   1, CMD_PROT_ALL
                              ,0
                              ,0
                              ,DBMSrv_Command::KeySchedulerActivateJob
                              ,true , DBMRightSCDUse_CN50 | DBMRightSCDAdmin_CN50 },
{"scheduler_create_job"       ,true , false, false, DBNAME_YES ,   2,   3, CMD_PROT_ALL
                              ,0
                              ,0
                              ,DBMSrv_Command::KeySchedulerCreateJob
                              ,true , DBMRightSCDUse_CN50 | DBMRightSCDAdmin_CN50 },
{"scheduler_deactivate_job"   ,true , false, false, DBNAME_YES ,   1,   1, CMD_PROT_ALL
                              ,0
                              ,0
                              ,DBMSrv_Command::KeySchedulerDeactivateJob
                              ,true , DBMRightSCDUse_CN50 | DBMRightSCDAdmin_CN50 },
{"scheduler_delete_job"       ,true , false, false, DBNAME_YES ,   1,   1, CMD_PROT_ALL
                              ,0
                              ,0
                              ,DBMSrv_Command::KeySchedulerDeleteJob
                              ,true , DBMRightSCDUse_CN50 | DBMRightSCDAdmin_CN50 },
{"scheduler_list_jobs"        ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_ALL
                              ,0
                              ,0
                              ,DBMSrv_Command::KeySchedulerListJobs
                              ,true , DBMRightSCDUse_CN50 | DBMRightSCDAdmin_CN50 },
/*
{"scheduler_show_joblog"      ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_ALL
                              ,0
                              ,0
                              ,DBMSrv_Command::KeySchedulerShowJoblog
                              ,true , DBMRightSCDUse_CN50 | DBMRightSCDAdmin_CN50 },
{"scheduler_delete_joblog"    ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_ALL
                              ,0
                              ,0
                              ,DBMSrv_Command::KeySchedulerDeleteJoblog
                              ,true , DBMRightSCDUse_CN50 | DBMRightSCDAdmin_CN50 },
*/
{"scheduler_start"            ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_ALL
                              ,0
                              ,0
                              ,DBMSrv_Command::KeySchedulerStart
                              ,true , DBMRightSCDAdmin_CN50 },
{"scheduler_state"            ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_ALL
                              ,0
                              ,0
                              ,DBMSrv_Command::KeySchedulerState
                              ,true , DBMRightSCDUse_CN50 | DBMRightSCDAdmin_CN50 },
{"scheduler_stop"             ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_ALL
                              ,0
                              ,0
                              ,DBMSrv_Command::KeySchedulerStop
                              ,true , DBMRightSCDAdmin_CN50 },
{"service_connect"            ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_NO
                              ,cn80ServiceConnect
                              ,"                                    (needs dbname)"
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"service_release"            ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_NO
                              ,cn80ServiceRelease
                              ,"                                    (needs dbname)"
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50 },
{"show"                       ,true , false, false, DBNAME_YES ,   1,  -1, CMD_PROT_NO
                              ,cn40ShowSomething
                              ,"<SHOW command>                                    "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightDBInfoRead_CN50 },
{"show_list"                  ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_NO
                              ,cn40ShowList
                              ,"                                                  "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightDBInfoRead_CN50 },
{"show_next"                  ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_NO
                              ,cn40ShowNext
                              ,"                                                  "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightDBInfoRead_CN50 },
{"sql_connect"                ,true , false, false, DBNAME_YES ,   0,   1, CMD_PROT_NO
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyUsingCommandFactory
                              ,true , DBMRightAccessSQL_CN50 | DBMRightDBInfoRead_CN50 },
{"sql_execute"                ,true , false, false, DBNAME_YES ,   1,  -1, CMD_PROT_NOERR
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyUsingCommandFactory
                              ,true , DBMRightAccessSQL_CN50 },
{"sql_executenice"            ,true , false, false, DBNAME_YES ,   1,  -1, CMD_PROT_NOERR
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyUsingCommandFactory
                              ,true , DBMRightAccessSQL_CN50 },
{"sql_fetch"                  ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_NO
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyUsingCommandFactory
                              ,true , DBMRightAccessSQL_CN50 },
{"sql_fetchnice"              ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_NO
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyUsingCommandFactory
                              ,true , DBMRightAccessSQL_CN50 },
{"sql_info"                   ,true , false, false, DBNAME_YES ,   1,  -1, CMD_PROT_NO
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyUsingCommandFactory
                              ,true , DBMRightAccessSQL_CN50 },
{"sql_recreateindex"          ,true , false, false, DBNAME_YES ,   0,   1, CMD_PROT_ALL
                              ,cn83RecreateIndex
                              ,"\"<scheme>\".\"<table>\".\"<index>\"              "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightAccessSQL_CN50 },
{"sql_release"                ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_NO
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyUsingCommandFactory
                              ,true , DBMRightAccessSQL_CN50 | DBMRightDBInfoRead_CN50 },
{"sql_updatestat"             ,true , false, false, DBNAME_YES ,   0,  -1, CMD_PROT_ALL
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyUsingCommandFactory
                              ,true , DBMRightAccessSQL_CN50 },
{"sql_updatestat_per_systemtable",true , false, false, DBNAME_YES ,   0,  0, CMD_PROT_ALL
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyUsingCommandFactory
                              ,true , DBMRightAccessSQL_CN50 },
{""                           ,true , false, false, DBNAME_EGAL,   0,   0, CMD_PROT_NO
                              ,NULL
                              ,"                                                  "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true  ,DBMMaskNothing_CN50 },
{"test"                       ,false, false, false, DBNAME_EGAL,   0,  -1, CMD_PROT_ALL
                              ,cn40Test
                              ,NULL
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true  ,DBMMaskNothing_CN50 },
{"trace_clear"                ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_YES
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyTraceClear
                              ,true , DBMRightDBInfoRead_CN50 | DBMRightDBFileRead_CN50 },
{"trace_flush"                ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_YES
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyTraceFlush
                              ,true , DBMRightDBInfoRead_CN50 | DBMRightDBFileRead_CN50 },
{"trace_off"                  ,true , false, false, DBNAME_YES ,   1,  -1, CMD_PROT_YES
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyTraceOff
                              ,true , DBMRightDBInfoRead_CN50 | DBMRightDBFileRead_CN50 },
{"trace_on"                   ,true , false, false, DBNAME_YES ,   1,  -1, CMD_PROT_YES
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyTraceOn
                              ,true , DBMRightDBInfoRead_CN50 | DBMRightDBFileRead_CN50 },
{"trace_prot"                 ,true , false, false, DBNAME_YES ,   1,   1, CMD_PROT_YES
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyTraceProt
                              ,true , DBMRightDBInfoRead_CN50 | DBMRightDBFileRead_CN50 },
{"trace_protopt"              ,false, false, false, DBNAME_EGAL,   0,   0, CMD_PROT_NO
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyTraceProtOpt
                              ,true , DBMRightDBInfoRead_CN50 | DBMRightDBFileRead_CN50 },
{"trace_show"                 ,true , false, false, DBNAME_YES ,   0,   1, CMD_PROT_NO
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyTraceShow
                              ,true , DBMRightDBInfoRead_CN50 | DBMRightDBFileRead_CN50 },
{""                           ,true , false, false, DBNAME_EGAL,   0,   0, CMD_PROT_NO
                              ,NULL
                              ,"                                                  "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true  ,DBMMaskNothing_CN50 },
{"user_create"                ,true , false, false, DBNAME_YES ,   1,   2, CMD_PROT_YES
                              ,cn41UserCreate
                              ,"<username>,<pwd> [<templateuser>]   (needs dbname)"
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightUserMgm_CN50 },
{"user_delete"                ,true , false, false, DBNAME_YES ,   1,   1, CMD_PROT_ALL
                              ,cn41UserDelete
                              ,"<username>                          (needs dbname)"
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightUserMgm_CN50 },
{"user_get"                   ,true , false, false, DBNAME_YES ,   1,   1, CMD_PROT_NO
                              ,cn41UserGet
                              ,"<username>                          (needs dbname)"
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightDBInfoRead_CN50 },
{"user_getall"                ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_NO
                              ,cn41UserGetAll
                              ,"                                    (needs dbname)"
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightDBInfoRead_CN50 },
{"user_getchallenge"          ,false, false, false, DBNAME_YES ,   1,  -1, CMD_PROT_NO
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyUsingCommandFactory
                              ,true  ,DBMMaskNothing_CN50 },
{"user_getrights"             ,true , false, false, DBNAME_YES ,   2,   2, CMD_PROT_NO
                              ,cn41UserGetRights
                              ,"<username> SERVERRIGHTS|GUIRIGHTS   (needs dbname)"
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightDBInfoRead_CN50 },
{"user_logon"                 ,false, false, false, DBNAME_YES ,   1,   1, CMD_PROT_NO
                              ,cn41UserLogon
                              ,"<username>,<pwd>                    (needs dbname)"
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true  ,DBMMaskNothing_CN50 },
{"user_put"                   ,true , false, false, DBNAME_YES ,   2,  -1, 1
                              ,cn41UserPut
                              ,"<username> <property>=<value> ...   (needs dbname)"
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightUserMgm_CN50 },
{"user_rename"                ,true , false, false, DBNAME_YES ,   2,  2, 1
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyUsingCommandFactory
                              ,true , DBMRightUserMgm_CN50 },
{"user_response"              ,false, false, false, DBNAME_YES ,   1,   1, CMD_PROT_NO
                              ,0
                              ,0
                              ,DBMSrv_Command::KeyUsingCommandFactory
                              ,true  ,DBMMaskNothing_CN50 },
{"user_sap"                   ,false, false, false, DBNAME_YES ,   1,   1, CMD_PROT_YES
                              ,cn41UserSap
                              ,NULL
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightUserMgm_CN50 },
{"user_sysdba"                ,false, false, false, DBNAME_YES ,   1,   1, CMD_PROT_YES
                              ,cn41UserSysdba
                              ,NULL
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightUserMgm_CN50 },
{"user_system"                ,false, false, false, DBNAME_EGAL,   1,   1, CMD_PROT_NO
                              ,cn41UserSystem
                              ,NULL
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true  ,DBMMaskNothing_CN50 },
{"util_connect"               ,true , false, false, DBNAME_YES ,   0,   1, CMD_PROT_NO
                              ,cn80UtilConnect
 /*** obsolete ***/           ,"(obsolete version of db_connect)                  "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightAccessUtility_CN50 | DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50},
{"util_execute"               ,true , false, false, DBNAME_YES ,   0,  -1, CMD_PROT_ALL
                              ,cn80UtilExecute
 /*** obsolete ***/           ,"(obsolete version of db_executenice)              "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightAccessUtility_CN50 },
{"util_release"               ,true , false, false, DBNAME_YES ,   0,   0, CMD_PROT_NO
                              ,cn80UtilRelease
 /*** obsolete ***/           ,"(obsolete version of db_release)                  "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true , DBMRightAccessUtility_CN50 | DBMRightBackupRestore_CN50 | DBMRightBackupSave_CN50},
{""                           ,true , false, false, DBNAME_EGAL,   0,   0, CMD_PROT_NO
                              ,NULL
                              ,"                                                  "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true  ,DBMMaskNothing_CN50 },
{"version"                    ,false, false, true , DBNAME_EGAL,   0,   0, CMD_PROT_NO
                              ,cn40GetVersion
                              ,"                                                  "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true  ,DBMMaskNothing_CN50 },
{NULL                         ,true , false, false, DBNAME_EGAL,   0,   0, CMD_PROT_NO
                              ,NULL
                              ,"                                                  "
                              ,DBMSrv_Command::KeyUnknownCommand
                              ,true  ,DBMMaskNothing_CN50 }
};

const cn10DBMServerCommand * DBMServerCommandsArray_cn10=namesC;

/*!
  -----------------------------------------------------------------------------
  Chapter: static variables
  -----------------------------------------------------------------------------
 */
static VControlDataT TheVControl;

 /*!
  -----------------------------------------------------------------------------
  EndChapter: static variables
  -----------------------------------------------------------------------------
 */

/*!
  -----------------------------------------------------------------------------
  Chapter: Public functions
  -----------------------------------------------------------------------------
 */

#if defined( _WIN32 )
int ctrlservExceptionHandler(
        unsigned int exCode,
        struct _EXCEPTION_POINTERS *ep) {
    return DBMSrvFrm_Server::Instance().handleException( exCode, ep );
}
#else
void ctrlservExceptionHandler(
        int signalCode ) {
    DBMSrvFrm_Server::Instance().handleException( signalCode );
}
#endif

/*!
    @brief called by DBMServer framework in case a communication error occurs

    This funtion handles the error in logging it...
*/
void ctrlservHandleCommError(
        int callerId,
        tsp00_Bool connected,
        tsp00_Bool cancelByte,
        tsp01_CommErr commErr) {
    DBMSrvFrm_Server::Instance().handleCommunicationError( commErr );
}

/*!
  -------------------------------------------------------------------------
  function:     ctrlservopen
  -------------------------------------------------------------------------
  description:  DBMServer startup

                This function initializes the DBMServer session data. The
                function will be called by the DBMServer framework at the
                startup of the DBMServer.

  arguments:    vcontrolData   [OUT]  - pointer for DBMServer session data
                dbname         [IN]   - name of database
                dbroot         [IN]   - dbroot path
                pfCancelFlag   [IN]   - the cancel flag
                bVerifiedLocalCommunication [IN]   - the 'administrator' local communication flag
                couldLogOn     [IN]   - could call sqlxidlogon (callerId) (obsolete)
                callerId       [IN]   - user id of calling process (obsolete)
                isLocalCommunication        [IN]   - true unless xserver is involved

  -------------------------------------------------------------------------
*/
void ctrlservopen (
            void      ** vcontrolData,
            const char * dbname,
            const char * dbroot,
            tsp00_Bool * pfCancelFlag,
            tsp00_Bool   bVerifiedLocalCommunication,
            tsp00_Bool,
            int,
            tsp00_Bool  isLocalCommunication,
            const char* clientMachineID,
            SAPDB_Int8  clientPIDonClientMachine)
{
  // all code from this method should be moved to DBMSrvFrm_Server's start method

  TheVControl.start();
  Tools_DynamicUTF8String dbNameString(dbname==NULL?"":dbname); // for upper case conversion
  TheVControl.m_ClientMachineID = clientMachineID;
  TheVControl.m_ClientPIDonClientMachine = clientPIDonClientMachine;
  TheVControl.bIsSAPDBAdmi = (bVerifiedLocalCommunication != false);
  TheVControl.bIsLocalCommunication = (isLocalCommunication != 0);
  TheVControl.bIsDropped = false;
  TheVControl.active          = true;
  TheVControl.dbname.rawAssign(dbNameString.ToUpper().CharPtr());
  TheVControl.dbroot.rawAssign(dbroot==NULL?"":dbroot);
  TheVControl.pHSSNodes       = new DBMSrvHSS_Nodes;    //this does not too much
  TheVControl.pfCancelFlag    = (bool *) pfCancelFlag;
  TheVControl.pCurrentUser    = new cn50DBMUser;        //this does not too much
  cn00DBMServerData::setVcontrol(&TheVControl);
  *vcontrolData = &TheVControl;

#if defined DBMCLI
  if( DBMSrvFrm_Server::Instance().start(true) ) {
#else
  if( DBMSrvFrm_Server::Instance().start(false) ) {
#endif

    // initialize logging
    DBMLog_Logger::getInstance().setDBMComponent("DBMSrv");
    tsp00_Pathc   szLogFilePath;
    DBMLog_LogFile* normalFile(NULL);
    DBMLog_LogFile* panicFile(NULL);
        
    // before doing anything else, prepare to log into <independent data>/wrk with regular files (no shared memory)
    // this preparation must not require shared memory!
    DBMSrvMsg_Error logFilePathError;
    tsp00_DbNamec emptyDbName;
    emptyDbName.Init();
    if( cn42GetFileName(emptyDbName, FGET_DBMSRV_CN42, szLogFilePath, logFilePathError) ) {
        // we could find the path
        normalFile = new DBMLog_LogFile(szLogFilePath.asCharp(), false);
        panicFile = new DBMLog_LogFile(szLogFilePath.asCharp(), true);
        if( normalFile == NULL || panicFile == NULL ) {
            DBMSrvFrm_Server::Instance().m_CtrlservopenError.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_MEM));
            DBMSrvFrm_Server::Instance().m_CtrlservopenError.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_NODBMLOGGER));
        }
        if( normalFile != NULL )
            DBMLog_Logger::getInstance().addFile(normalFile);
        if( panicFile != NULL )
            DBMLog_Logger::getInstance().addFile(panicFile);
        normalFile = NULL;
        panicFile = NULL;
    }
    else {
        // error calculating path
        DBMSrvFrm_Server::Instance().m_CtrlservopenError.Overrule(logFilePathError);
        DBMSrvFrm_Server::Instance().m_CtrlservopenError.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_NODBMLOGGER));
    }

    // initialize shared memory
    bool shmUsable(false);
    DBMSrvMsg_Error shmInitError;
    if( !dbNameString.Empty() )
        shmUsable = DBMSrvShM_ShmAdmin::TheAdmin().initialize(dbNameString.ToUpper().CharPtr(), shmInitError);

    bool runDirExists(false);
    if( !dbNameString.Empty() && shmUsable ) {
        // find database's rundirectory (for logging)
        tcn002_XpValueString runDir;
        runDir.Init();
        DBMSrvMsg_Error oParamGetMsg;
        if( !cn20XParamGetValue(TheVControl.dbname, PAN_RUNDIR, runDir, oParamGetMsg) && oParamGetMsg!=DBMSrvMsg_Error::DBMSrv_XPNOTFOUND )
            DBMSrvFrm_Server::Instance().m_CtrlservopenError.Overrule(oParamGetMsg);
        else {
            tsp05_RteFileInfo runDirInfo;
            tsp05_RteFileError runDirErr;
            sqlfinfoc (runDir.asCharp(), &runDirInfo, &runDirErr);
            if( runDirErr.sp5fe_result == vf_ok ) {
                runDirExists = (runDirInfo.sp5fi_exists != (pasbool) 0);
                // we have a rundirectory, now we can delete the "old" shared memory files
                if( runDirExists )
                    DBMSrvShM_ShmMigrator::deleteShmFromRundirectory(runDir.asCharp());
            }
            else
                DBMSrvFrm_Server::Instance().m_CtrlservopenError.Overrule(DBMSrvMsg_RTEError(runDirErr));
        }
    }

    // prepare logging
    logFilePathError.ClearMessageList();
    if( runDirExists ) {
        // a synched file for normal logging
        // a direct file for panic logging
        if( cn42GetFileName(TheVControl.dbname, FGET_DBMPRT_CN42, szLogFilePath, logFilePathError) ) {
            // we could find logfile path
            if( shmUsable )
                normalFile = new DBMLog_SynchronizedLogFile(szLogFilePath.asCharp());
            else
                normalFile = new DBMLog_LogFile(szLogFilePath.asCharp(), false);
            panicFile = new DBMLog_LogFile(szLogFilePath.asCharp(), true);
        }
        else {
            // error finding logfile
            DBMSrvFrm_Server::Instance().m_CtrlservopenError.Overrule(logFilePathError);
            DBMSrvFrm_Server::Instance().m_CtrlservopenError.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_NODBMLOGGER));
        }
    }
    else if( !dbNameString.Empty() ) {
        // we have no rundirectory but a database name
        // a synched file for normal logging
        // a direct file for panic logging
        if( cn42GetFileName(TheVControl.dbname, FGET_DBMSRV_CN42, szLogFilePath, logFilePathError) ) {
            // we could find logfile path
            if( shmUsable )
                normalFile = new DBMLog_SynchronizedLogFile(szLogFilePath.asCharp());
            else
                normalFile = new DBMLog_LogFile(szLogFilePath.asCharp(), false);
            panicFile = new DBMLog_LogFile(szLogFilePath.asCharp(), true);
        }
        else {
            // error finding logfile
            DBMSrvFrm_Server::Instance().m_CtrlservopenError.Overrule(logFilePathError);
            DBMSrvFrm_Server::Instance().m_CtrlservopenError.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_NODBMLOGGER));
        }
    }
    else {
        // no rundirectory, no database name
        // keep default behavior
    }
        
    // now we know what files to log to (if different from default)
    if(  normalFile != NULL && panicFile != NULL ) {
        DBMLog_Logger::getInstance().freeResources();
        DBMLog_Logger::getInstance().addFile(normalFile);
        DBMLog_Logger::getInstance().addFile(panicFile);
    }

    if( !shmUsable && !dbNameString.Empty() )
        // shared memory is not usable, but should be
        DBMSrvFrm_Server::Instance().m_CtrlservopenError.Overrule(shmInitError);
    
    //set DBROOT environment variable (PTS: 1107753)
    if (TheVControl.dbroot.length() > 0) {
        static char szEnvVar[512];
        SAPDB_strcpy(szEnvVar, DBROOT_ENV_VAR);
        strcat(szEnvVar, "=");
        strcat(szEnvVar, TheVControl.dbroot);
        putenv(szEnvVar);
    }
  } 

  if( !DBMSrvFrm_Server::Instance().m_CtrlservopenError.IsEmpty() )
    DBMSrvFrm_Server::Instance().m_CtrlservopenError.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_DBMSRV_NOSTART));
} /* end ctrlservopen */

/*!
  -------------------------------------------------------------------------
  function:     ctrlserverclose
  -------------------------------------------------------------------------
  description:  DBMServer shutdown

                This function cleans up the DBMServer session data. The
                function will be called by the DBMServer framework at the
                shutdown of the DBMServer.

  arguments:    vcontrolData   [INOUT]  - pointer for DBMServer session data

  -------------------------------------------------------------------------
*/
void ctrlservclose (
      void * vcontrolData)
{
    DBMSrvMsg_Error loggedInside;
    DBMSrvFrm_Server::Instance().stop(loggedInside);  
} // end ctrlserverclose

/*!
  -------------------------------------------------------------------------
  function:     ctrlservcommand
  -------------------------------------------------------------------------
  description:  DBMServer command

                This function executes one DBMServer command. The
                function will be called by the DBMServer framework.

  arguments:    vcontrolData   [INOUT]  - pointer for DBMServer session data
                requestData    [IN]     - pointer to the request buffer
                requestLen     [IN]     - length of request buffer
                replyData      [INOUT]  - pointer to the reply buffer
                replyLen       [OUT]    - length of replied data buffer
                replyLenMax    [IN]     - max. length of reply buffer

  return value: csp9_ctrl_release for stopping the DBMServer, otherwise
                csp9_ctrlcmd_ok

  -------------------------------------------------------------------------
*/
int ctrlservcommand (
      void   * vcontrolData,
      char   * requestData,
      int      requestLen,
      char   * replyData,
      int    * replyLen,
      int      replyLenMax)
{
  FUNCTION_DBG_MCN00_1("vcn10::ctrlservcommand");

  *replyLen = 0;

  VControlDataT      * vcontrol  = (VControlDataT*) vcontrolData;
  CommandT             command;
  tcn00_Error          nCmdRc    = OK_CN00;
  bool serverMustExit(false);

  // analyze request command
  while(requestLen > 0 && requestData[requestLen-1] == ' ' ) // remove padding
    --requestLen;
  requestData[requestLen] = CHAR_STRINGTERM_CN90;

  replyLenMax = replyLenMax - requestLen - 200;

  //create a reply object and initialize it to a zero terminated string
  DBMSrv_Reply reply(replyData, replyLen, replyLenMax);
  reply.startWithString("");

  cn10_AnalyzeCommand (requestData, requestLen, command);

  if (command.nIndex != CommandT::InvalidIndex) {

    // log command
//    namesC[command.nIndex].nTokens2Log = CMD_PROT_ALL;
    if ((namesC[command.nIndex].nTokens2Log != CMD_PROT_NO   ) &&
        (namesC[command.nIndex].nTokens2Log != CMD_PROT_NOERR)    ) {
      cn10_LogCommand(&command);
    } // end if

    if (namesC[command.nIndex].name == NULL) {
      char         szToken[PARAMETER_MAXLEN_CN90];

      cn90GetToken(requestData, szToken, 1, PARAMETER_MAXLEN_CN90);

      nCmdRc=reply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_COMMAND, Msg_Arg("COMMAND_NAME", szToken)));
    } else if  (namesC[command.nIndex].nDBNameState == DBNAME_YES &&
                vcontrol->dbname[0] == CHAR_STRINGTERM_CN90 ) {
      // dbname required
      nCmdRc = cn90AnswerIError(replyData, replyLen, ERR_NODBNAME_CN00);
    } else if  (namesC[command.nIndex].nDBNameState == DBNAME_NO  &&
                vcontrol->dbname[0] != CHAR_STRINGTERM_CN90 ) {
      // no dbname
      nCmdRc = cn90AnswerIError(replyData, replyLen, ERR_DBNAME_CN00);
    } else if ((vcontrol->dbname[0] != CHAR_STRINGTERM_CN90) && namesC[command.nIndex].bNeedDBMLogon && !vcontrol->pCurrentUser->isLoggedOn()) {
      // Logon check
      nCmdRc = cn90AnswerIError(replyData, replyLen, ERR_NEEDLOGON_CN00);
    } else if  ((vcontrol->dbname[0] == CHAR_STRINGTERM_CN90) && namesC[command.nIndex].bNeedSAPDBAdmi  && !vcontrol->bIsSAPDBAdmi) {
      // system logon
      nCmdRc = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_NEEDADMI));
    } else if  ((vcontrol->dbname[0] != CHAR_STRINGTERM_CN90) && !vcontrol->pCurrentUser->isServerRight(namesC[command.nIndex].nRights)) {
      // check rights
      nCmdRc = cn90AnswerIError(replyData, replyLen, ERR_MISSRIGHT_CN00);
    } else if  (namesC[command.nIndex].nMinParam > 0 &&
                !cn90GetToken(command.args, NULL, namesC[command.nIndex].nMinParam, -1)) {
      // min param count
      nCmdRc = cn90AnswerIError(replyData, replyLen, ERR_TOOFEWPARAM_CN00);
    } else if  (namesC[command.nIndex].nMaxParam > -1 && cn90GetToken(command.args, NULL, namesC[command.nIndex].nMaxParam + 1, -1)) {
      // max param count
      nCmdRc = cn90AnswerIError(replyData, replyLen, ERR_TOOMANYPARAM_CN00);
    }
    else
    {
        //if we reach this point, we would like to call the function representing the DBM command
        //we have to watch out for classic commands with a cnFunction, for commands implemented
        //by a command object found via a command key and for commands implemented by command
        //objects, that are created by the command factory

        // remove old next command
        if (namesC[command.nIndex].cnFunction != cn40DBMNext)
        {
            vcontrol->szNextCommand.rawAssign("");
            vcontrol->nNextCommandSkip = 0;
        }

        //do we have a classic command function?
        if ( namesC[command.nIndex].cnFunction != 0 )
        {
            // call function pointer only if database was not dropped before
            if( vcontrol->bIsDropped )
            {
                // for all commands except the exit command, return an error and do nothing
                // dbname exists in vcontrol, otherwise we would not be here
                // the exit commands are implemented as factory commands, so we can't have one here
                nCmdRc = reply.startWithMessageList(
                    DBMSrvMsg_Error(SDBMSG_DBMSRV_DBISDROPPED, Msg_Arg("DBMSRV_COMP", (const char*) vcontrol->dbname)));
            }
            else //call the function
                nCmdRc = namesC[command.nIndex].cnFunction (vcontrol, &command, replyData, replyLen, replyLenMax);
        }
        else if(namesC[command.nIndex].cmdKey != DBMSrv_Command::KeyUnknownCommand)
        {
            DBMSrv_Command * pCommand=0;
            DBMSrvMsg_Error  message;

            nCmdRc=DBMSrvInfrCmd_CommandFactory::getCommand(
                    namesC[command.nIndex].name,
                    namesC[command.nIndex].cmdKey,
                    pCommand,
                    reply,
                    message);

            if(0!=pCommand)
            {
                if( vcontrol->bIsDropped && !pCommand->isExitCommand() ) {
                    // after the drop command, the client might send an exit command which should not fail
                    nCmdRc = reply.startWithMessageList(DBMSrvMsg_Error(
                        SDBMSG_DBMSRV_DBISDROPPED,
                        Msg_Arg("DBMSRV_COMP", (const char*) vcontrol->dbname)));
                }
                else {
                    nCmdRc = pCommand->run(vcontrol, &command, reply);
                    // we ran an exit command, so we exit, be it successful or not
                    serverMustExit = pCommand->isExitCommand();
                }
                delete pCommand;
            }
        }
        else
            nCmdRc=reply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_COMMAND, Msg_Arg("COMMAND_NAME", namesC[command.nIndex].name)));

        //fill reply buffer at all costs, otherwise we run into a buggy feature of our framework veo42_1.c (waits for reply without telling the client an answer before)
        //do not change nCmdRc, as it reflects the answer of the command
        //note, that an empty reply means that the command itself was known (DBMSrv_COMMAND does not leave an empty reply buffer)
        if(0==reply.getCurrentLength())
        {
            if(OK_CN00==nCmdRc)
                reply.startWithOK();
            else
                reply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_ERR));
        }
    } // end if

    // in case of error log command always
    if ((namesC[command.nIndex].nTokens2Log == CMD_PROT_NO     ) &&
        (nCmdRc                             != OK_CN00         ) &&
        (nCmdRc                             != ERR_NOREPLY_CN00) &&
        (vcontrol->active                                 )    ) {
        // errors from the exit command are not logged here (they are logged inside DBMSrvFrm_Server)
      cn10_LogCommand(&command);
    } // end if

  } else {
    DBMSrv_Reply reply(replyData, replyLen, replyLenMax);
    char         szToken[PARAMETER_MAXLEN_CN90];

    cn90GetToken(requestData, szToken, 1, PARAMETER_MAXLEN_CN90);

    nCmdRc=reply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_COMMAND, Msg_Arg("COMMAND_NAME", szToken)));
  } // end if

  // in case of error log answer
  if ((nCmdRc                             != OK_CN00         ) &&
      (nCmdRc                             != ERR_NOREPLY_CN00) &&
      (namesC[command.nIndex].nTokens2Log != CMD_PROT_NOERR  )    ) {
    
    DBMSrvMsg_Error outList;
    DBMSrvInfrReply_Reply2MsgList::transform(reply, outList); // if transformation fails, this
                                                              // is noted in errOut
    cn10_LogError( outList );
  } // end if

  return serverMustExit ? csp9_ctrl_release : csp9_ctrlcmd_ok;
} // end ctrlservcommand

/*!
  -----------------------------------------------------------------------------
  EndChapter: Public functions
  -----------------------------------------------------------------------------
 */

/*
  -----------------------------------------------------------------------------
  IMPLEMENTATION PRIVATE FUNCTION cn10_LogCommand
  -----------------------------------------------------------------------------
*/
void cn10_LogCommand
      ( CommandT   * command )
{
  size_t nPosition =  0;
  size_t nMaxLen   =  0;
  size_t argLen    =  strlen(command->args);
  int    nToken    =  0;

  if (namesC[command->nIndex].nTokens2Log == CMD_PROT_ALL) {
    nPosition = argLen + 1;
  } else {
    while (nPosition < argLen && nToken < namesC[command->nIndex].nTokens2Log) {
      ++nPosition;
      if (isspace(command->args[nPosition])     != 0 &&
          isspace(command->args[nPosition - 1]) == 0    ) {
        ++nToken;
      } // end if
    } // end while
  } // end if

  // Log Command
  Tools_DynamicUTF8String outLine(_Tools_UTF8Ptr(command->args));
  outLine = outLine.SubStr((Tools_DynamicUTF8String::BasisElementIndex) 0,
                           (Tools_DynamicUTF8String::ElementCount) nPosition);
  if (nPosition < argLen) {
  	outLine.Append("...");
  } // end if
  Msg_List commandList( Msg_List::Info, SDBMSG_DBMSRV_COMMAND_RECEIVED,
     Msg_Arg(SDBMSGTAG_DBMSRV_COMMAND_RECEIVED__DBMSRV_CMDOPTION,
       outLine.CharPtr()),
     Msg_Arg(SDBMSGTAG_DBMSRV_COMMAND_RECEIVED__DBMSRV_CMDNAME,
       (namesC[command->nIndex].name == NULL)? "" : namesC[command->nIndex].name));

  // log who called us
  DBMSrvFrm_Server::Instance().logConnect();

  DBMLog_Logger::getInstance().addMessageList(commandList);
} // end cn10_LogCommand

/*
  -----------------------------------------------------------------------------
  IMPLEMENTATION PRIVATE FUNCTION cn10_LogError
  -----------------------------------------------------------------------------
*/
void cn10_LogError
      ( const DBMSrvMsg_Error& listToLog )
{
    // a little bit dirty: do not log "row not found" errors!
    bool noLogEntry(false);  
    if( listToLog.ID() == DBMSrvMsg_Error::get_tcn00_Error(DBMSrvMsg_Error::DBMSrv_SQL) ) {
        const char* sqlReturnCode = (const char*)listToLog.ArgValue(SDBMSGTAG_DBMSRV_SQL__RETURNCODE);
        noLogEntry = (sqlReturnCode != NULL && atoi(sqlReturnCode) == 100); // row not found
    }
    if( !noLogEntry ) {
      // log who called us  
      DBMSrvFrm_Server::Instance().logConnect();
      
      DBMLog_Logger::getInstance().addMessageList(listToLog);
    }
} // end cn10_LogError

/*
  -----------------------------------------------------------------------------
  IMPLEMENTATION PRIVATE FUNCTION cn10_AnalyzeCommand
  -----------------------------------------------------------------------------
*/
static void cn10_AnalyzeCommand
      ( const char  *  szData,
        const int      nDataLen,
        CommandT     & DBMCommand )
{
  int            nIndex   = 0;
  const char  *  pCommand = szData;
  bool handlingOpenError(!DBMSrvFrm_Server::Instance().m_CtrlservopenError.IsEmpty());

  if( handlingOpenError )
      pCommand = "exit";

  while (isspace(*pCommand) && ((pCommand - szData) < nDataLen)) {
    ++pCommand;
  } // end while

  for (nIndex = 0;namesC[nIndex].name != NULL; ++nIndex) {
    if (cn90CheckKeyword(pCommand, namesC[nIndex].name, false)) {
      DBMCommand.nIndex = nIndex;
      if( !handlingOpenError ) {
        // fill CommandT
        while (!isspace(*pCommand) && ((pCommand - szData) < nDataLen)) {
            ++pCommand;
        } // end while
        // skip white spaces
        while (isspace(*pCommand) && ((pCommand - szData) < nDataLen)) {
            ++pCommand;
        } // end while
        DBMCommand.setArgs (pCommand);
      }
      else
        DBMCommand.setArgs (""); // no arguments at exit command
      break;
    } // end if
  } // end for

} // end cn10_AnalyzeCommand

/*
  -----------------------------------------------------------------------------
  IMPLEMENTATION PRIVATE FUNCTION cn10_HSSExecute
  -----------------------------------------------------------------------------
*/
static tcn00_Error cn10_HSSExecute (
    VControlDataT * vcontrol,
    CommandT      * command,
    char          * replyData,
    int           * replyLen,
    int             replyLenMax)
{
  FUNCTION_DBG_MCN00_1("vcn10.cpp::cn10_HSSExecute");

  tcn00_Error nFuncReturn = OK_CN00;
  char        szNode   [PARAMETER_MAXLEN_CN90];
  char        szCommand[PARAMETER_MAXLEN_CN90];
  int         nIndex      = 0;

  // extract node
  cn90GetToken(command->args, szNode,   1, PARAMETER_MAXLEN_CN90);
  cn90GetToken(command->args, szCommand, 2, PARAMETER_MAXLEN_CN90);

  // search command
  while (namesC[nIndex].name != NULL) {
    if ( stricmp(szCommand, namesC[nIndex].name) == 0) {
      break;
    } // end if
    ++nIndex;
  } // end while

  if (namesC[nIndex].name == NULL) {
     DBMSrv_Reply reply(replyData, replyLen, replyLenMax);

     nFuncReturn=reply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_COMMAND, Msg_Arg("COMMAND_NAME", szCommand)));
  } // end if

  if (nFuncReturn == OK_CN00) {
    if (!namesC[nIndex].bHSS) {
     DBMSrvMsg_Error aEvent(SDBMSG_DBMSRV_NOHSS, Msg_Arg(SDBMSGTAG_DBMSRV_NOHSS__DBMSRV_COMP,szCommand));
     nFuncReturn = cn90AnswerEvent(replyData, replyLen, replyLenMax, aEvent);
    } // end if
  } // end if

  if (nFuncReturn == OK_CN00) {
    Tools_DynamicUTF8String sAnswer;

    DBMSrvMsg_Error oMsg;
    if (vcontrol->pHSSNodes->Execute(szNode, strstr(command->args, szCommand), sAnswer, oMsg)) {
      sprintf(replyData, "%.*s", replyLenMax, sAnswer.CharPtr());
      *replyLen = (int) strlen (replyData);
    } else {
      if ((((int) oMsg.ID()) != DBMSrvMsg_Error::get_tcn00_Error(DBMSrvMsg_Error::DBMSrv_HSSNODEUNKNOWN)) &&
          (((int) oMsg.ID()) != DBMSrvMsg_Error::get_tcn00_Error(DBMSrvMsg_Error::DBMSrv_HSS           ))    ) {
        sprintf(replyData, "%.*s", replyLenMax, sAnswer.CharPtr());
        *replyLen = (int) strlen (replyData);
      } else {
        nFuncReturn = DBMSrv_Reply(replyData, replyLen, replyLenMax).startWithMessageList(oMsg);
      } // end if
    } // end if
  } // end if

  return nFuncReturn;
} // end cn10_HSSExecute

/*
  -----------------------------------------------------------------------------
  IMPLEMENTATION PUBLIC FUNCTION cn10CheckStatic
  -----------------------------------------------------------------------------
*/
bool cn10CheckStatic
   ( const char * pCommand,
     bool       & bStaticAvailable,
     bool       & bNeedsDatabase,
     bool       & bNeedsUser )
{
  CommandT oCommand;
  cn10_AnalyzeCommand (pCommand, (int) strlen(pCommand), oCommand);

  if (oCommand.nIndex == CommandT::InvalidIndex) {
    return false;
  } // end if

  bStaticAvailable = namesC[oCommand.nIndex].bStaticAvailable;
  bNeedsUser       = namesC[oCommand.nIndex].bNeedDBMLogon;
  bNeedsDatabase   = (namesC[oCommand.nIndex].nDBNameState == DBNAME_YES);

  return true;
} // end cn10CheckStatic
