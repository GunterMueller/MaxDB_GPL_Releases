/* @lastChanged: "1998-05-07  18:52"
 *
 * @filename:    hcn31.h
 * @purpose:     "backup management"
 * @release:     7.1.0.0
 * @see:         "-.-"
 *
 * ==================================================================
 *
 * responsible:   MartinR
 *
 * created:       1998-03-18 by MartinR
 *
 * purpose:       basic module for backup
 *                               
 * ==================================================================



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




 */

#ifndef _HCN31_H_
#define _HCN31_H_

/* ------------------------------------------------------------------
 * includes
 * ------------------------------------------------------------------
 */
#include "gcn00.h"
#include "gcn003.h"
#include "DBM/Srv/DBMSrv_Reply.hpp"
#include "DBM/Srv/KernelAccess/DBMSrvKnl_Session.hpp"

/* ------------------------------------------------------------------
 * specification public functions
 * ------------------------------------------------------------------
 */
tcn00_Error cn31BackupReplyAvailable (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn31BackupReplyReceive (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn31BackupCommandRequest (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn31BackupCommand (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn31BackupSaveRequest (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn31BackupStage (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn31BackupStageRepeat (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn31BackupSave (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn31BackupSaveReplaceRequest (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn31BackupSaveReplace (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn31BackupSaveIgnoreRequest (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn31BackupSaveIgnore (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn31BackupSaveCancelRequest (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn31BackupSaveCancel (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn31BackupSaveState (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn31BackupRestoreRequest (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn31BackupRestoreRequest (
      VControlDataT     * vcontrol,
      char              * replyData,
      int               * replyLen,
      int                 replyLenMax,
      const char        * MediumName,
      const char        * BackupType,
      const char        * ExternalBackupID,
      tcn003_MediaGroup * MediaGroup,
      tcn003_Medium     * Medium,
      tcn003_Media      * Media,
      int               & MediaUsedByBackupSession,
      const char        * UntilDate,
      const char        * UntilTime,
      const char        * LabelToCheck);

tcn00_Error cn31BackupRestore (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn31BackupRestoreCheckRequest (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn31BackupRestoreCheck (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn31BackupRestoreCreateRequest (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn31BackupRestoreCreate (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn31BackupRestoreConfig (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn31BackupRestoreReplaceRequest (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn31BackupRestoreReplaceRequest (
      VControlDataT     * vcontrol,
      char              * replyData,
      int               * replyLen,
      int                 replyLenMax,
      const char        * MediumName,
      const char        * ExternalBackupID,
      tcn003_Medium     * pMedium,
      tcn003_MediaGroup * MediaGroup,
      tcn003_Media      * Media,
      int               & MediaUsedByBackupSession);

tcn00_Error cn31BackupRestoreReplace (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn31BackupRestoreIgnoreRequest (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn31BackupRestoreIgnore (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn31BackupRestoreCancelRequest (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn31BackupRestoreCancel (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn31BackupRestoreState (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn31BackupRestoreStateCheck (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn31BackupRestartInfo (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn31BackupLabel (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn31BackupRestoreDevspace (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn31AutosaveOn (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn31AutosaveOn (
      VControlDataT * vcontrol,
      const char    * szMediaName,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn31AutosaveEnd (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn31AutosaveEnd (
      VControlDataT * vcontrol,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn31AutosaveCancel (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

tcn00_Error cn31AutosaveShow (
      VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax );

/* ------------------------------------------------------------------
 * specification internal functions
 * ------------------------------------------------------------------
 */

tcn00_Error cn31BackupRestartInfo(
      VControlDataT            * vcontrol,
      char                     * replyData,
      int                      * replyLen,
      int                        replyLenMax,
      tcn003_BackupRestartInfo * pRestartInfo);

enum cn31BackupSessionType
{
    ForBackup_cn31,
    ForRestore_cn31,
    ForCheck_cn31
};

tcn00_Error cn31BackupSession (
    VControlDataT            * vcontrol,
    DBMSrv_Reply             & reply,
    tcn003_BackupSession    ** pBackupSession,
    cn31BackupSessionType      sessionFor);

tcn00_Error cn31getExistingBackupAndKernelSession (
    VControlDataT            * vcontrol,
    DBMSrv_Reply             & reply,
    tcn003_BackupSession    *& backupSession,
    DBMSrvKnl_Session       *& kernelSession,
    cn31BackupSessionType      sessionFor);

tcn00_Error cn31BackupStageSession(
    VControlDataT         * vcontrol,
    DBMSrv_Reply          & reply,
    tcn003_BackupSession ** pBackupSession);

void cn31FreeBackupSession(
      VControlDataT     * vcontrol);

void cn31FreeBackupStageSession(VControlDataT * vcontrol);

tcn00_Error cn31_BuildSaveIgnoreCommand(const char * szBackupType,
                                        char       * szCommand);

tcn00_Error cn31_BuildRestoreIgnoreCommand(const char * szBackupType,
                                           char       * szCommand);

int cn31BackupResultReceive
  ( DBMSrvKnl_Session   & oSession,
    bool                & bBackupRc,
    tcn003_BackupResult & oResult,
    DBMSrvMsg_Error     & oError );

void cn31_InitResult (
    tcn003_BackupResult * pResult );

void cn31_PrintBackupAnswer (
    char                * replyData,
    tcn003_BackupResult * tResult,
    bool                  bState);

#endif /* _HCN31_H_ */

