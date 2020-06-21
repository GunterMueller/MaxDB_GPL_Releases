/*! @ingroup   KernelAdministration
    @file      Kernel_IAdminInfo.hpp
    @author    UweH
    @brief     This interface provides methods to handle information about kernel administrative state.
*/
/*
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
#ifndef Kernel_IAdminInfo_H
#define Kernel_IAdminInfo_H

#include "gsp00.h"

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "Logging/Log_Types.hpp"

struct tgg00_TransContext;
struct tgg00_UtilCmdId;

/// interface to administrative information gathering
class Kernel_IAdminInfo
{
public:
    /// Access the interface.
    static Kernel_IAdminInfo& Instance();
    /// Returns true if kernel is in restart phase.
    virtual bool KernelStateIsRedoLogProcessing() = 0;
    /// Returns true if kernel is online.
    virtual bool KernelStateIsOnline() = 0;
    /// Returns true if kernel is in admin mode.
    virtual bool KernelStateIsAdmin() = 0;
    /// A new id for the protocols is generated.
    virtual void GetNewCommandId( tsp00_TaskId     taskid,
                                  tgg00_UtilCmdId &id ) = 0;
    /// This returns all values to decide before restart if a restart can be successfull.
    virtual void GetRestartVersion
                           (tgg00_TransContext &trans,
                            Log_IOSequenceNo   &firstUsedIOsequence,
                            Log_IOSequenceNo   &firstLogIOsequence,
                            SAPDB_Char         *dbidentRestartrecord,
                            SAPDB_UInt          dbidentRestartrecordSize,
                            SAPDB_Char         *dbidentLogInfo,
                            SAPDB_UInt          dbidentLogInfoSize,
                            RTE_Nodename       &masterNodeName,
                            bool               &logWritingEnabled,
                            bool               &logAutoOverwrite,
                            SAPDB_Int4         &queueCount,
                            bool               &restartable,
                            SAPDB_Int4         &converterVersion) = 0;
    /// If this database is a service instance for checking backups true is returned.
    virtual bool IsCheckBackupConfigured() = 0;
    /// If this returns false no transaction writes to the log volume.
    virtual bool IsLogWritingEnabled(tsp00_TaskId taskid) = 0;
    /// If this returns true, no log backups are needed.
    virtual bool IsLogAutoOverwriteEnabled(tsp00_TaskId taskid) = 0;
};
#endif  /* Kernel_IAdminInfo_H */
