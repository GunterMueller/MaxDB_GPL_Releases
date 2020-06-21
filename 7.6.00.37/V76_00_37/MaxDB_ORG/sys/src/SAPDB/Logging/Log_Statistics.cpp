/*!
  @file           Log_Statistics.cpp
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
#include "Logging/Log_Statistics.hpp"
#include "Logging/Log_Savepoint.hpp"
#include "Logging/Log_Volume.hpp"
#include "RunTime/System/RTESys_MicroTime.h"
#include "hkb57.h" // GetWriteTransNo
#include "hkb51.h" // GetSelectTransNo

/// deklaration of the manager
Log_Statistics Log_StatisticManager;

void Log_Statistics::GetStatistics (     tsp00_TaskId          taskid,
                                         Log_StatisticValues & statistics ) const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Statistics::GetStatistics", LogVolume_Trace, 5);
    
    Log_Volume & log = Log_Volume::Instance();

    statistics.logVolumeEnabled   = log.IsDevspaceEnabled(taskid);
    statistics.logWriterSuspended = log.LogwriterIsSuspended();
    statistics.automaticOverwrite = log.IsAutomaticOverwrite(taskid);

    log.GetDeviceUsage (statistics.usableSize,
                        statistics.usedSize,
                        statistics.notSavedSize,
                        statistics.pendingLogSize );

    statistics.usedPercentage = (statistics.usedSize * 100) / statistics.usableSize;
    statistics.notSavePercentage = (statistics.notSavedSize * 100) / statistics.usableSize;

    // systemviews display sizes in 1KB-units => recalculate them
    const SAPDB_UInt pageSize = FrameCtrl_ICommon::GetInstance().GetPageSize();
    statistics.usableSize   = (statistics.usableSize   * pageSize) / 1024;
    statistics.usedSize     = (statistics.usedSize     * pageSize) / 1024;
    statistics.notSavedSize = (statistics.notSavedSize * pageSize) / 1024;
    
    statistics.logIsFull = log.LogIsFull();
    statistics.savepointCount = Log_Savepoint::Instance().GetSavepointCount();

    tgg91_TransNo tmpTransNo;

    kb51GetLastSelectTransNo(taskid, tmpTransNo);
    statistics.selectTransactionCount = tmpTransNo.gg91GetUInt8();

    SAPDB_UInt8 currTransNo;
    kb57GetLastWriteTransNo(taskid, tmpTransNo);
    currTransNo = tmpTransNo.gg91GetUInt8();
    SAPDB_RangeIterator<SAPDB_UInt8, SAPDB_UInt8>
                        transIter(  0,
                                    currTransNo,
                                    tgg91_TransNo::gg91MaxTransNo().gg91GetUInt8());
    statistics.writeTransactionCount = transIter.GetDistanceFrom(restartWriteTransNo);

    tgg91_SessionNo sessionNo;
    kb57GetLastSessionNo(taskid, sessionNo );
    statistics.maxUsedSessionId = sessionNo.gg90GetInt4();
    statistics.maxUsedTransNo = currTransNo;

    statistics.queueCount = log.GetNumQueuesAllocated();
    statistics.queueSize = (log.QueueSize() * pageSize ) / 1024;
    statistics.flushMode = log.GetFlushMode();
    statistics.deviceState = log.GetDeviceState();
}
    
void Log_Statistics::GetStatisticsReset (tsp00_TaskId                taskid,
                                         Log_StatisticResetValues  & statistics )
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Statistics::GetStatisticsReset", LogVolume_Trace, 5);

    SAPDB_UInt4 currentValue;

    currentValue = Log_Savepoint::Instance().GetSavepointCount();
    statistics.savepointCount = currentValue - m_baseValues.savepointCount;

    tgg91_TransNo tmpTransNo;

    kb51GetLastSelectTransNo(taskid, tmpTransNo);
    SAPDB_RangeIterator<SAPDB_UInt8, SAPDB_UInt8>
                        transIter(  0,
                                    tmpTransNo.gg91GetUInt8(),
                                    tgg91_TransNo::gg91MaxTransNo().gg91GetUInt8());
    statistics.selectTransactionCount = transIter.GetDistanceFrom(m_baseValues.selectTransactionCount);

    kb57GetLastWriteTransNo(taskid, tmpTransNo);
    transIter.SetPosition( tmpTransNo.gg91GetUInt8() );
    statistics.writeTransactionCount = transIter.GetDistanceFrom(m_baseValues.writeTransactionCount);
    statistics.timeOfLastReset = m_baseValues.timeOfLastReset;
}

void Log_Statistics::Initialize(tsp00_TaskId     taskid)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Statistics::Initialize", LogVolume_Trace, 5);

    m_baseValues.savepointCount = Log_Savepoint::Instance().GetSavepointCount();

    tgg91_TransNo transNo;
    kb57GetLastWriteTransNo(taskid, transNo);
    m_baseValues.writeTransactionCount = transNo.gg91GetUInt8();
    ++m_baseValues.writeTransactionCount;

    kb51GetLastSelectTransNo(taskid, transNo);
    m_baseValues.selectTransactionCount = transNo.gg91GetUInt8();

    restartWriteTransNo = m_baseValues.writeTransactionCount;

    m_baseValues.timeOfLastReset = RTESys_MicroSecondTime();
}

void Log_Statistics::ResetStatistics (tsp00_TaskId          taskid)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_Statistics::ResetStatistics", LogVolume_Trace, 5);

    tgg91_TransNo currTransNo;
    Log_Volume & log (Log_Volume::Instance());

    m_baseValues.savepointCount = Log_Savepoint::Instance().GetSavepointCount();

    kb51GetLastSelectTransNo(taskid, currTransNo);
    m_baseValues.selectTransactionCount = currTransNo.gg91GetUInt8();

    kb57GetLastWriteTransNo(taskid, currTransNo);
    m_baseValues.writeTransactionCount = currTransNo.gg91GetUInt8();

    m_baseValues.timeOfLastReset = RTESys_MicroSecondTime();
}

