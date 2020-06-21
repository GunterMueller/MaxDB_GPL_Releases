/*!
  @file           Log_QueueStatistics.cpp
  @author         MartinB
  @ingroup        LogVolume
  @brief          Collects statistical data for the LogVolume

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

#include "Logging/Log_QueueStatistics.hpp"
#include "Logging/Log_Volume.hpp"
#include "Logging/Log_Queue.hpp"

/// the statistics-manger is declared here
Log_QueueStatistics Log_QueueStatisticManager;


void Log_QueueStatistics::GetStatistics (      tsp00_TaskId               taskid,
                                         const Log_QueueID                queueId,
                                               Log_QueueStatisticValues & statistics ) const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_QueueStatistics::GetLogStatistics", LogVolume_Trace, 5);


    Log_Volume & log (Log_Volume::Instance());
    Log_Queue  & logQueue (log.GetQueueForStatistic(queueId));

    // systemviews display sizes in 1KB-units => recalculate them
    const SAPDB_UInt pageSize = FrameCtrl_ICommon::GetInstance().GetPageSize();

    // statistics from the Log_Queue
    statistics.queueMaxUsed = (logQueue.GetMaxPagesUsed(Log_Queue::SystemViewLogQueue) * pageSize ) / 1024;
    statistics.queueInsertCount = logQueue.GetReserveCount(Log_Queue::SystemViewLogQueue);
    statistics.queueOverflowCount = logQueue.GetQueueFullSuspendCount(Log_Queue::SystemViewLogQueue);
    statistics.groupCommitCount = logQueue.GetGroupCommitCount(Log_Queue::SystemViewLogQueue);
    statistics.waitCount = logQueue.GetWaitCount(Log_Queue::SystemViewLogQueue);
    statistics.maxWaitCountPerPage = logQueue.GetMaxCountOfWaitingTasksPerPage(Log_Queue::SystemViewLogQueue);
    statistics.physWriteCount = logQueue.GetPhysWriteCount(Log_Queue::SystemViewLogQueue);
}   
void Log_QueueStatistics::GetStatisticsReset (      tsp00_TaskId                taskid,
                                              const Log_QueueID                 queueId,
                                                    SAPDB_UInt8               &timeOfLastReset,
                                                    Log_QueueStatisticValues  & statistics )
{
    SAPDBTRACE_METHOD_DEBUG ("Log_QueueStatistics::GetLogStatisticsReset", LogVolume_Trace, 5);


    Log_Volume & log (Log_Volume::Instance());
    Log_Queue  & logQueue (log.GetQueueForStatistic(queueId));

    // systemviews display sizes in 1KB-units => recalculate them
    const SAPDB_UInt pageSize = FrameCtrl_ICommon::GetInstance().GetPageSize();

    // statistics from the Log_Queue
    statistics.queueMaxUsed = (logQueue.GetMaxPagesUsed(Log_Queue::SystemViewLogQueueReset) * pageSize ) / 1024;
    statistics.queueInsertCount = logQueue.GetReserveCount(Log_Queue::SystemViewLogQueueReset);
    statistics.queueOverflowCount = logQueue.GetQueueFullSuspendCount(Log_Queue::SystemViewLogQueueReset);
    statistics.groupCommitCount = logQueue.GetGroupCommitCount(Log_Queue::SystemViewLogQueueReset);
    statistics.waitCount = logQueue.GetWaitCount(Log_Queue::SystemViewLogQueueReset);
    statistics.maxWaitCountPerPage = logQueue.GetMaxCountOfWaitingTasksPerPage(Log_Queue::SystemViewLogQueueReset);
    statistics.physWriteCount = logQueue.GetPhysWriteCount(Log_Queue::SystemViewLogQueueReset);
    timeOfLastReset = logQueue.GetTimeOfLastStatisticsReset();
}

void Log_QueueStatistics::ResetStatistics (tsp00_TaskId          taskid)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_QueueStatistics::ResetStatistics", LogVolume_Trace, 5);

    Log_Volume & log (Log_Volume::Instance());
    SAPDB_Int queueCount = log.GetNumQueuesAllocated();
    for (Log_QueueID queueId(0); queueId < queueCount; ++queueId)
    {
        Log_Queue  & logQueue (log.GetQueueForStatistic(queueId));
        logQueue.ResetStatistics(taskid, Log_Queue::ResetSystemView);
    }
}


