  /*!
  @file           SrvTasks_JobTypes.hpp
  @author         UweH
  @ingroup        ServerTasks
  @brief          types of global known jobs for vtrace or rte console

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
#ifndef SRVTASKS_JOBTYPES_HPP
#define SRVTASKS_JOBTYPES_HPP

#include "RunTime/Tasking/RTETask_ITask.hpp"

/// this is a list of some common job types which should be displayed in a uniform way
enum SrvTasks_JobTypes
{
    jobUnspecified = 0,
    jobPrefetchBlob,
    jobPrefetchSelect,
    jobPrefetchPages,
    jobPrefetchObjetcsCoordinator,
    jobAutosaveCoordinator,
    jobAutosaveReader,
    jobBackupMediumTask,
    jobBackupVolumeTask,
    jobCheckData,
    jobCreateIndex,
    jobDropAuxfiles,
    jobPrefetchObjects,
    jobRedoLogReader,
    jobRedoLogExecutor,
    jobStandbyRestartCoordinator,
    jobStandbySynchronizeServer,
    jobSavepoint,
    jobGarbageCollector,
    jobJoinInvSelect,
    jobUpdateStat,
    jobDropVolume,
    jobUpdateCounter,
    jobLAST_DO_NOT_USE_OR_REMOVE
    // !Keep in sync with SrvTasks_JobTypesString and SrvTasks_JobTypesStringShort in SrvTasks_JobTypes.cpp!
};
/// string conversion of SrvTasks_JobTypes
extern const RTE_TaskDisplayName      SrvTasks_JobTypesString [jobLAST_DO_NOT_USE_OR_REMOVE];

/// short string conversion of SrvTasks_JobTypes
extern const RTE_TaskDisplayNameShort SrvTasks_JobTypesStringShort [jobLAST_DO_NOT_USE_OR_REMOVE];

#endif // SRVTASKS_JOBTYPES_HPP
