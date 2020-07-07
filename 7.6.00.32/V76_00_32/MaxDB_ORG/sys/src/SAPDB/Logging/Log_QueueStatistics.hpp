/*!
  @file           Log_QueueStatistics.hpp
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
#ifndef LOG_QUEUESTATISTICS_HPP
#define LOG_QUEUESTATISTICS_HPP

#include "gsp00.h"
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "Logging/Log_Types.hpp"


/// defines access method for extracting statistical values related to the Log_Queue
class Log_QueueStatistics
{
public:
    /// structure for extracting statistical data of logging queues
    struct Log_QueueStatisticValues 
    {
        SAPDB_UInt4     queueMaxUsed;           ///< QUEUEMAXUSED, FIXED(5)
        SAPDB_UInt4     queueInsertCount;       ///< QUEUEINSERTCOUNT, FIXED(20)
        SAPDB_UInt4     queueOverflowCount;     ///< QUEUEOVERFLOWCOUNT, FIXED(10)
        SAPDB_UInt4     groupCommitCount;       ///< GROUPCOMMITCOUNT, FIXED(10)
        SAPDB_UInt4     waitCount;              ///< WAITCOUNT, FIXED(10)
        SAPDB_UInt4     maxWaitCountPerPage;    ///< MAXWAITCOUNTPERPAGE, FIXED(10)
        SAPDB_UInt4     physWriteCount;         ///< SV_PHYSICALWRITECOUNT, FIXED(10)
    };

    /// returns all information for one Log_Queue for SytemView_LogStatistics
    void GetStatistics (      tsp00_TaskId          taskid,
                        const Log_QueueID           queueId,
                              Log_QueueStatisticValues & statistics ) const;
    /// returns all statistical information for one Log_Queue since the last Reset-Call
    void GetStatisticsReset (      tsp00_TaskId               taskid,
                             const Log_QueueID                queueId,
                                   SAPDB_UInt8              & timeOfLastReset,
                                   Log_QueueStatisticValues & statistics );
    /// Resets the values for resetable statistical values for logging queues
    void ResetStatistics(tsp00_TaskId          taskid);

};
/// from external the access is done via this manager-instance
/// The instance itself is allocaled and initialized via Log_Volume
extern Log_QueueStatistics Log_QueueStatisticManager;
#endif

