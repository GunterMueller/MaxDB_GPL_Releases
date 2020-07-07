/*!
    @ingroup   KernelAdministration
    @file      Kernel_IAdminBackupRecovery.hpp
    @author    UweH
    @brief     This interface provides methods to handle backup and recovery for log and data.

\if EMIT_LICENCE
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

\endif
*/
#ifndef Kernel_IAdminBackupRecovery_H
#define Kernel_IAdminBackupRecovery_H

#include "gsp00.h"
#include "ggg00.h"

#include "KernelCommon/Kernel_DateTime.hpp"
#include "Restart/Rst_Types.hpp"

struct tgg00_TransContext;
struct tgg00_MessBlock;
class SAPDBErr_MessageList;

/// Backup Recovery Interface
class Kernel_IAdminBackupRecovery
{
public:
    /// Access the interface
    static Kernel_IAdminBackupRecovery& Instance();
	/// For backup of data this tells if is incremental or complete.
    enum DataBackupType  {backupComplete, backupIncrementell};
    /// This starts complete or incremental data backup.
    virtual void BackupData (tgg00_TransContext &trans,
                             DataBackupType      backupType,
                             tgg00_MessBlock    &backupDescription) = 0;
    /// This starts complete or incremental data restore.
    virtual void RestoreData (tgg00_TransContext &trans,
                              tgg00_MessBlock    &backupDescription) = 0;
    /// This starts the log backup.
    virtual void BackupLog (tgg00_TransContext &trans,
                            tgg00_MessBlock    &backupDescription) = 0;
    /// result for RestoreLog()
    enum RestoreLogResult
    {
        RestoreLogOk,
        RestoreLogNewMediumRequired,
        RestoreLogWrongRole,
        RestoreLogNotAllowed,
        RestoreLogRestartInternFailed,
        RestoreLogInitError,
        RestoreLogError,
        RestoreLogFatalError
    };
    /// After log was restored the kernel is restarted to online mode.
    /// This is a restart with log entries from backup.
    virtual RestoreLogResult RestoreLog (tsp00_TaskId          taskid,
                                         tgg00_TransContext   &oldcontext,
                                         tgg00_MessBlock      &restoreDescription,
                                         SAPDBErr_MessageList &errlist) = 0;
    /// This is used for REPLACE, IGNORE and CANCEL commands
    virtual void BackupRestoreHandling (tgg00_TransContext &trans,
                                        tgg00_MessBlock    &handlingDescription) = 0;
	/// result for RedoLog()
    enum RedoLogResult
    {
        RedoLogOk,
        RedoLogNotAllowed,
        RedoLogUntilTimeInvalid,
        RedoLogUntilIOSequenceInvalid,
        RedoLogRedoLogFailed,
        RedoLogRedoLogCancelled,
        RedoLogReOpenLogForWritingFailed
    };
    /// Returns false on error. This is used externally by the backup or hot stand by server.
    virtual RedoLogResult RedoLog (tsp00_TaskId          taskid,
                                   tgg00_TransContext   &oldcontext,
                                   Rst_RedoKind          redokind,
                                   Log_RawDeviceOffset   lastRedoOffset,
                                   Kernel_Date           untildate,
                                   Kernel_Time           untiltime,
                                   Log_IOSequenceNo      untilIOSequence,
                                   SAPDBErr_MessageList &errlist) = 0;
    /// returns true if redo was aborted by any redo execution task.
    virtual bool IsRedoLogAborted (tsp00_TaskId taskid) = 0;
    /// This call aborts a running redo. If no redo is running it is ignored.
    virtual void AbortRedoLogByRestoreComponent (tsp00_TaskId     taskid,
                                                 tgg00_BasisError abortReason) = 0;
    /// This checks if the log state "HistoryLost" is set and if yes then creates a new DBIdent.
    virtual void SetNewDBIdentifierIfHistoryLost(tsp00_TaskId taskid) = 0;
    /// after this is called no backup and restore commands are allowed
    virtual bool DenyBackupAndRestore() = 0;
    /// after this is called backup and restore commands are allowed again
    virtual void PermitBackupAndRestore() = 0;
};
#endif  /* Kernel_IAdminBackupRecovery_H */
