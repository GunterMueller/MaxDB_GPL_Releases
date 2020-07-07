/*!
  @file           Log_Statistics.hpp
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
#ifndef LOG_STATISTICS_HPP
#define LOG_STATISTICS_HPP

#include "Logging/Log_Types.hpp"

/// defines access method for extracting statistical values related to the Log_Volume
class Log_Statistics
{
public:
    /// CTOR : reads all initial values from diverse components
    Log_Statistics()
        : restartWriteTransNo(0) {};

    /// After the restart some initial values, which are not initialized with ZERO every restart,
    /// have to be saved
    void Initialize( tsp00_TaskId   taskid );
    
    /// structure for extracting statistical data for the log area
    struct Log_StatisticValues 
    {
        SAPDB_Bool      logVolumeEnabled;       ///< SV_LOGVOLUMEENABLED, CHAR(3)
        SAPDB_Bool      logWriterSuspended;     ///< ???
        SAPDB_Bool      automaticOverwrite;     ///< ???
        SAPDB_UInt4     pendingLogSize;         ///< ???
        SAPDB_UInt4     usableSize;             ///< SV_USABLESIZE, FIXED(20)
        SAPDB_UInt4     usedSize;               ///< SV_USEDSIZE, FIXED(20)
        SAPDB_UInt4     usedPercentage;         ///< SV_USEDSIZEPERCENTAGE, FIXED(3)
        SAPDB_UInt4     notSavedSize;           ///< SV_NOTSAVEDSIZE, FIXED(20)
        SAPDB_UInt4     notSavePercentage;      ///< NOTSAVEDPERCENTAGE, FIXED(3)
        SAPDB_Bool      logIsFull;              ///< LOGFULL, CHAR(3)
        SAPDB_UInt4     savepointCount;         ///< SAVEPOINTCOUNT, FIXED(10)
        SAPDB_UInt8     selectTransactionCount; ///< TRANSACTIONCOUNT, FIXED(10)
        SAPDB_UInt8     writeTransactionCount;  ///< WRITETRANSACTIONCOUNT, FIXED(10)
        SAPDB_UInt4     maxUsedSessionId;       ///< MAXUSEDSESSIONID, FIXED(10)
        SAPDB_UInt8     maxUsedTransNo;         ///< MAXUSEDTRANSACTIONNUMBER, FIXED(15)
        SAPDB_UInt4     queueCount;             ///< QUEUECOUNT, FIXED(5)
        SAPDB_UInt4     queueSize;              ///< QUEUESIZE, FIXED(5)

        Log_FlushMode   flushMode;              ///< FLUSHMODE, CHAR(20)
        Log_DeviceState deviceState;            ///< DEVICESTATE, CHAR(20)
    } ;
    
    /// This struct is used by Log_Statistics and Log_StatisticsReset in order to
    /// avoid doubled counters (one for Log_Statistics and one for Log_StatisticsReset).
    /// Instead the counter counts only since the last RESET call. For the total
    /// sum since restart these base-values has to be added.
    struct Log_StatisticResetValues
    {
        SAPDB_UInt4     savepointCount;         ///< SAVEPOINTCOUNT, FIXED(10)
        SAPDB_UInt8     selectTransactionCount; ///< TRANSACTIONCOUNT, FIXED(10)
        SAPDB_UInt8     writeTransactionCount;  ///< WRITETRANSACTIONCOUNT, FIXED(10)
        SAPDB_UInt8     timeOfLastReset;        ///< RESETDATE, FIXED(10)
        
        /// CTOR
        Log_StatisticResetValues()
        :
        savepointCount        (0),
        selectTransactionCount(0),
        writeTransactionCount (0),
        timeOfLastReset       (0)
        {}
    };

    /// returns all information for one Log_Queue for SytemView_LogStatistics
    void GetStatistics (      tsp00_TaskId          taskid,
                              Log_StatisticValues & statistics ) const;
    /// returns all statistical information for one Log_Queue since the last Reset-Call
    void GetStatisticsReset (      tsp00_TaskId               taskid,
                                   Log_StatisticResetValues & statistics );
    /// Resets the values for resetable statistical values
    void ResetStatistics(tsp00_TaskId          taskid);


private:

    /// base values for calculating the total of some counters since the las
    /// restart
    Log_StatisticResetValues             m_baseValues;

    /// special basevalue, which is needed for calculating the WriteTransNo,
    /// because this value is not resetted by a restart
    SAPDB_UInt8                          restartWriteTransNo;
};
/// from external the access is done via this manager-instance
extern Log_Statistics Log_StatisticManager;
#endif

