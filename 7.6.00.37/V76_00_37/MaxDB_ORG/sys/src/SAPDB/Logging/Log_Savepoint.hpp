/*!
  @file    Log_Savepoint.hpp
  @ingroup Logging
  @author  UweH
  @brief   The savepoint manager.

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
#ifndef LOG_SAVEPOINT_H
#define LOG_SAVEPOINT_H

#include "gsp00.h" // tsp00_TaskId
#include "ggg00.h" // tgg00_TransContext
#include "gkb00.h" // tkb00_SaveptParam
#include "SAPDBCommon/SAPDB_Singleton.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "KernelCommon/Kernel_DateTime.hpp"
#include "KernelCommon/Kernel_TaskWaitList.hpp"
#include "Logging/Log_Types.hpp"
#include "Converter/Converter_Version.hpp"
#include "IOManager/IOMan_BlockAddress.hpp"
#include "DataAccess/Data_Types.hpp"
#include "ServerTasks/SrvTasks_Types.hpp"

class SAPDBMem_IRawAllocator;
class RTETask_ITask;
class Trans_Context;

/*!
   @class Log_Savepoint
   @brief This is the savepoint manager. It is a Singleton.
 */
class Log_Savepoint : public SAPDB_Singleton
{
public:
    /// Access for interfaces
    static Log_Savepoint& Instance();
    /// Creates the singleton and may be called only once.
    static void CreateInstance ( SAPDBMem_IRawAllocator &allocator );
    /// This is used to reserve a servertask for savepoints
    static bool ReserveServerTask (RTETask_ITask &task,
                                   Msg_List      &errlist);
public:
    /// result of both start methodes
    enum StartResult
    {
        ok,
        systemNotReady,
        logIsFull,
        notAllowed,
        serverStartFailed,
        executionError
    };
    /*!
        @brief This starts a savepoint if none is running.
        If a savepoint is already running, the caller waits for the next savepoint
        which is executed automatically.
     */
    StartResult StartSavepointAndWait (RTETask_ITask       &task,
                                       Log_SavepointReason  reason,
                                       Msg_List            &errlist);
	/// If somebody is waiting for a savepoint, a savepoint is started, if no savepoint is active.
    StartResult StartSavepoint (RTETask_ITask       &task,
                                Log_SavepointReason  reason,
                                Msg_List            &errlist);
    /*!
        @brief returns true, if the given reason can only be triggered at one time.
        This reasons normally cause special functions during the savepoint.
        Examples are DataBackup or Restart.
     */
    bool IsDBManagerReason (Log_SavepointReason reason);
	/// returns true, if a savepoint is running.
    bool IsActive () const
    {
        // without region
        return m_IsActive;
    }
	/// This is the main servertask method.
    void Coordinator (tgg00_TransContext &Trans);
	/// Verifies opentrans file in restartrecord
    static bool Verify (tgg00_TransContext   &trans,
                        bool                  isCold,
                        SAPDBErr_MessageList &errlist);
	/// returns the last used savepoint id
    static Log_EntrySequence GetLastSavepointId();
	/// in online mode this will usually be Log_Savepoint but not during redo.
    static Log_ActionType GetLastSavepointEntryType();
    /// returns the timestamp from the logentry procssed during last savepoint
    static void GetStartEntryTimestamp( Kernel_Date &date,
                                        Kernel_Time &time );
	/// returns the last start entry iosequence
    static Log_IOSequenceNo GetLastSavepointIOSequence();
	/// returns the last start offset
    static Log_RawDeviceOffset GetLastSavepointOffset();
    /// some statistic values are offered
    void GetSavepointStatistics (SAPDB_Int4 &lastDurationSec,
                                 SAPDB_Int4 &lastDurationMicroSec,
                                 SAPDB_Int4 &lastWrittenPages,
                                 SAPDB_Int4 &lastIOcount,
                                 SAPDB_Int4 &avgWrittenPages,
                                 SAPDB_Int4 &avgIOcount)
    {
        // PTS 1119276 UH 2002-12-10 added
        lastWrittenPages     = m_Statistics.last.pagesWritten;
        lastIOcount          = m_Statistics.last.iocount;
        lastDurationSec      = m_Statistics.last.durationSec;
        lastDurationMicroSec = m_Statistics.last.durationMicroSec;
        m_Statistics.GetAverage(avgWrittenPages,avgIOcount);
    }
    /// returns the number of savepoints written since the last restart
    SAPDB_UInt GetSavepointCount()
    {
        return m_Statistics.count;
    }
    /// This is used for tasks which want to access a page for
    /// structureChange and have to wait for the running savepoint.
    void InsertToWaitForPrepare (RTETask_ITask     &task,
                                 tgg00_WaitContext &waitcontext);
    /// This can be used to prevent starting of a new savepoint. If false is returned the call failed.
    bool DenySavepoint (RTETask_ITask &task);
    /// This can be used to allow starting savepoints.
    void PermitSavepoint (RTETask_ITask &task);
    /// returns true if savepoint is allowed and restartrecord may be written
    bool IsSavepointAllowed() const;
    /// the string for a savepoint reason
    typedef SAPDB_Char SavepointReasonString [15+1];
    /// the string for a savepoint reason
    typedef SAPDB_Char LogentryTypeString [20+1];
    /// the db ident string
    typedef SAPDB_Char DatabaseIdentifier [sizeof(tsp00_Line)+1];
    /// the version string
    typedef SAPDB_Char DatabaseVersion [sizeof(tsp00_Version)+1];
    /// This defines the information needed by the systemview RESTARTINFORMATION
    struct RestartInformation
    {
        SAPDB_UInt4           badIndexCount;            ///< number of bad indexes - what does this mean ?
        SAPDB_UInt4           previousConverterVersion; ///< previous converter version
        SAPDB_UInt4           lastSavepointId;          ///< each savepoint has a number
        SavepointReasonString lastSavepointReason;      ///< the reason why the last savepoint was written
        SAPDB_UInt4           lastSavepointDate;        ///< Date when last savepoint was written
        SAPDB_UInt4           lastSavepointTime;        ///< Time when last savepoint was written
        SAPDB_UInt4           openTransactionCount;     ///< number of open transactions during last savepoint
        SAPDB_UInt4           openTransactionFileRoot;  ///< root page number of open trans file
        SAPDB_UInt4           historyDirectoryFileRoot; ///< root page number of history directory file
        SAPDB_UInt4           fileDirectoryRoot;        ///< root page number of filedirectory
        SAPDB_UInt4           longFileDirectoryRoot;    ///< root page number of long filedirectory
        SAPDB_UInt4           restartIOSequence;        ///< iosequence of the logentry where the redo reader must start
        SAPDB_UInt4           restartLogVolumeOffset;   ///< offset on log volume of the logentry where the redo reader must start
        SAPDB_UInt4           restartLogEntryOffset;    ///< offset in log page of the logentry where the redo reader must start
        LogentryTypeString    restartLogEntryType;      ///< entry type of the logentry where the redo reader must start
        SAPDB_UInt4           restartEntryDate;         ///< Date when last log entry was written
        SAPDB_UInt4           restartEntryTime;         ///< Date when last log entry was written
        DatabaseIdentifier    databaseIdentifier;       ///< DB Ident
        DatabaseVersion       currentSoftwareVersion;   ///< database release version of the last savepoint (40)
        DatabaseVersion       installationSoftwareVersion; ///< database release version of the createion of the current database (4)
    };
    /// returns infomation for the sstem view RESTARTINFORMATION
    static void GetRestartInformation ( RTETask_ITask      &task,
                                        RestartInformation &info );
private:
    /// exclusive requestor information
    struct ExclusiveRequestor
    {
        tsp00_TaskId         taskid;           ///< the task which waits for a special savepoint
        Log_SavepointReason  reason;           ///< the reason why the special savepoint was requested
        Msg_List            *pErrorList;       ///< If this is set the savepoints errorlist is append to the given.
        StartResult         *pResult;          ///< registered to transport any savepoint error
        void Clear ()
        {
            taskid     = cgg_nil_pid;
            reason     = Log_SVPReasonUnknown;
            pErrorList = 0;
            pResult    = 0;
        }
        /// initializes all values
        ExclusiveRequestor()
        {
            Clear();
        }
        bool IsAssigned() const
        {
            return taskid != cgg_nil_pid;
        }        
        bool Assign (const tsp00_TaskId         newTaskid,
                     const Log_SavepointReason  newReason,
                           Msg_List            &waiterErrList,
                           StartResult         &result)
        {
            if ( IsAssigned() )
                return false;
            taskid     = newTaskid;
            reason     = newReason;
            pErrorList = &waiterErrList;
            pResult    = &result;
            return true;
        }
    };
	/// this starts a servertask as a coordinator.
    bool StartCoordinator (RTETask_ITask &task,
                           Msg_List      &errlist);
    /// used for synchronization to access the restartrecord
    static void BeginRegion (RTETask_ITask &task);
    /// used for synchronization to access the restartrecord
    static void EndRegion (RTETask_ITask &task);
    /// During a savepoint the next tasks which want a savepoint are waiting for the next.
    void SwitchWaitList()
    {
        m_CurrentWaitList = (m_CurrentWaitList == 0) ? 1 : 0;
    }
	/// returns current waitlist to use.
    Kernel_TaskWaitList& CurrentWaitList()
    {
        return m_Waitlist [m_CurrentWaitList];
    }
	/// returns current waitlist to use.
    Kernel_TaskWaitList& WaitListForCurrentSavepoint()
    {
        return m_Waitlist [m_CurrentWaitList==0?1:0];
    }
	/// This writes the savepoint entry into all queues if no redo.
    bool DetermineRedoStartPositions ( Trans_Context     &trans,
                                       tkb00_SaveptParam &saveptParam);
	/// This creates a chain file, which contains entries for all open transactions.
    bool CreateOpenTransFile (Trans_Context      &trans,
                              tkb00_SaveptParam  &saveptParam,
                              bool                bIsRedoSavepoint);
	/// the loginfo pages are updated and restartrecord and loginfo is flushed.
    void FlushRestartRecordAndLogInfoPages  (Trans_Context             &trans,
                                             tkb00_SaveptParam          &saveptParam,
                                             Converter_Version          &converterVersion,
                                             IOMan_PackedBlockAddress   &converterRootBlockAddress,
                                             Data_PageNo                &maxDynamicPageNo,
                                             Data_PageNo                &maxStaticPageNo);
	/// FBM blocks being in state free after savepoint.
    void FlushCompleted (const RTETask_ITask     &task,
                         const Converter_Version &converterVersion);
	/// This stops all file accesses.
    void ShutdownFilesystem (tgg00_TransContext &coordinatorTrans);
    /// write some global info to the trace
    void WriteToTrace (char const * const title = 0);
    /// last words to exclusive user waiting for the savepoint
    void InternalErrorHandling(ExclusiveRequestor &exlusiveRequestor,
                               Msg_List           &errlist);
public:
    /// collected statistic data
    struct Statistic // PTS 1119276 UH 2002-12-10 added
    {
        SAPDB_UInt durationSec;             ///< duration in seconds
        SAPDB_UInt durationMicroSec;        ///< microsecond-part of duration
        SAPDB_UInt pagesWritten;            ///< number of pages written
        SAPDB_UInt iocount;                 ///< number of I/O-operations
        /// default ctor with zero-value
        Statistic()
        {
            durationSec      = 0;
            durationMicroSec = 0;
            pagesWritten     = 0;
            iocount          = 0;
        }
        /// constructor with all used values for a sample
        Statistic(SAPDB_UInt newDurationSec,
                  SAPDB_UInt newDurationMicroSec,
                  SAPDB_UInt newPagesWritten,
                  SAPDB_UInt newIocount)
        {
            durationSec      = newDurationSec;
            durationMicroSec = newDurationMicroSec;
            pagesWritten     = newPagesWritten;
            iocount          = newIocount;
        }
        /// adds two statistic samples in order to gain an total sum of statistic values
        Statistic& operator += ( const Statistic &other )
        {
            this->durationSec      += other.durationSec;
            this->durationMicroSec += other.durationMicroSec;
            this->durationSec      += this->durationMicroSec / 1000000;
            this->durationMicroSec  = this->durationMicroSec % 1000000;
            this->pagesWritten     += other.pagesWritten;
            this->iocount          += other.iocount;
            return *this;
        }
    };
    /// concrete summary of statistic samples
    struct Statistics // PTS 1119276 UH 2002-12-10 added
    {
        SAPDB_UInt count;           ///< number of summated statistic samples
        Statistic  last;            ///< value of the last sample
        Statistic  total;           ///< summation of all samples
        //-------------------------------------
        //-------------------------------------
        Statistics() { count = 0; }
        /// adds a set of statistic values to the current statistic state
        void Add(Statistic newValue)
        {
            ++count;
            last  =  newValue;
            total += last;
        }
        /// returns the average of all summated values
        void GetAverage(SAPDB_Int4 &pagesWritten,
                        SAPDB_Int4 &iocount)
        {
            pagesWritten = total.pagesWritten / count;
            iocount      = total.iocount      / count;
        }
    };
private:
    /// hidden default constructor resets all to nil
    Log_Savepoint(SAPDBMem_IRawAllocator &allocator)
    : m_Allocator        (allocator)
    , m_IsActive         (false)
    , m_CurrentWaitList  (0)
    , m_LogWriteEnabled  (false)
    , m_SavepointAllowed(true)
    , m_TaskWaitingUntilServertaskIsAvailable(0)
    , m_openTransfileCreated(false)
    {}
    /// This instance pointer of the singleton
    static Log_Savepoint *m_Instance;
    /// the global allocator which can be used by this object
    SAPDBMem_IRawAllocator &m_Allocator;
private:
	/// It is true, if a savepoint is running or will be running.
    bool                m_IsActive;
	/// Valid values are 0 and 1.
    SAPDB_UInt          m_CurrentWaitList;
    /// This is for tasks waiting for a normal savepoint be finished.
    /// There are two lists. One can be filled with requests and the other one
    /// can be used, if a savepoint is already running to wait for the next savepoint.
    Kernel_TaskWaitList    m_Waitlist [2];
    /// This is used for a dedicated savepoint
    ExclusiveRequestor m_ExclusiveRequestor;
    /*!
        @brief If this is false the next savepoint will be a ReDo-Savepoint
        Which means, that no log is written, but only the restartRecord is changed                   
        This member is initialized with false and can only be set
        to true (only one restart is possible during kernel-lifetime)
     */
    bool         m_LogWriteEnabled; // PTS 1115875 mb 2002-05-28
    /// collection point for statistics
    Statistics   m_Statistics; // PTS 1119276 UH 2002-12-10 added
    /// tasks waiting for end of prepare phase
    Kernel_TaskWaitList    m_WaitingForPrepare;
    /// This flag indicates that no savepoint can be written.
    bool m_SavepointAllowed; // PTS 1121281 UH 2003-03-24
    /// This is the server task which is permanently reserved for the savepoint coordinator
    SrvTasks_RequestedTask m_ServerTask;
    /// If this task pointer is not null a task is waiting until the savepoint servertask is initialized
    RTETask_ITask *m_TaskWaitingUntilServertaskIsAvailable;
    /// Flag which is used by CreateOpenTransFile to determine whether the current read opentransfile
    /// may be in an old layout
    bool           m_openTransfileCreated;
};
#endif  /* LOG_SAVEPOINT_H */
