/*! @defgroup LogVolume      Access to LogVolumes            */
/*!
  @file           Log_Volume.hpp
  @author         UweH
  @author         MartinB
  @ingroup        LogVolume
  @brief          Manages the log volume with queue and writer

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
#ifndef LOG_VOLUME_HPP
#define LOG_VOLUME_HPP
#include "gsp00.h"   // tsp00_TaskId
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "Container/Container_Vector.hpp"
#include "KernelCommon/DatabaseEvents/KernelDBE_Event.hpp"
#include "Logging/Log_IDiagnose.hpp"
#include "Logging/Log_Exceptions.hpp"
#include "Logging/Log_Queue.hpp"
#include "Logging/Log_Writer.hpp"
#include "Logging/Log_RawDeviceIterator.hpp"
#include "Logging/Log_InfoPage.hpp"
#include "Logging/Log_VolumeIterator.hpp"
#include "Logging/Log_SeqLogDevReader.hpp"
#include "Logging/Log_SaveIterator.hpp"
#include "IOManager/IOMan_ILogIO.hpp"
class RTETask_ITask;
class Trans_Context;
/*!
   @brief Administrates a single log volume
*/

/// The first usable offset on the Log_Volume
#define LOG_BEGIN_OFFSET     (Log_DeviceBlockCount (0U))
/// position of log info page on raw log device
#define LOG_INFOPAGEOFFSET   (Log_DeviceBlockCount(0U))
/// number of log info blocks (pages) on a raw log device
#define LOG_INFOBLOCKCOUNT   (Log_DeviceBlockCount(1U))



/// class for accessing a Log_Volume
class Log_Volume : public Log_IDiagnose
{
public:
    /// vector of log queues
    typedef Log_Writer::QueueVector                             QueueVector;

    /// queue synchronization mode (indicates whether queue requires sync. regions)
    typedef Log_Queue::SynchronizationMode                      QueueSyncMode;

    /// i/o sequence number of log pages
    typedef Log_IOSequenceNo                                    IOSequenceNo;
    
    /// block iterator of raw log device
    typedef Log_RawDeviceIterator                               RawDeviceIterator;
    
    /// raw offset on log device
    typedef Log_RawDeviceOffset                                 RawDeviceOffset;
    
    /// number of blocks on a raw log device
    typedef Log_DeviceBlockCount                                DeviceBlockCount;
    
    /// allocator for queue entries
    typedef SAPDBMem_IRawAllocator                              Allocator;
    
    /// allocator for page frames
    typedef Log_FrameAllocator                                  FrameAllocator;
    
    /// identifier of kernel task
    typedef tsp00_TaskId                                        TaskID;
    
    /// identifies the kind of restart
    enum RestartType
    {
        RestartNormal,
        RestartForFormat,
        RestartForRestore,
        RestartForStandby
    };

    /// Create the singleton this must be called by the writer task
    static void CreateInstance(RTETask_ITask          &task,
                               SAPDBMem_IRawAllocator &allocator,
                               FrameAllocator&         framealloc,
                               QueueSyncMode           syncmode);
    /*!
        @brief Returns reference to archive log manager singleton
        This method works properly only after the singleton has been created with CreateInstance().
     */
    static Log_Volume& Instance() 
    {
        SAPDBERR_ASSERT_STATE(m_Instance != 0);
        return *m_Instance;
    }
    /*!
       @brief          Constructor
       @param          writertaskid [in] id of the associated writer task
       @param          alloc [in] raw allocator to be used
       @param          framealloc [in] raw allocator for allocating pageframes
       @param          syncmode [in] log queue synchronization mode
     */
    Log_Volume 
        (TaskID          writertaskid,
         Allocator&      alloc,
         FrameAllocator &framealloc,
         QueueSyncMode   syncmode)
        :
         m_Allocator      ( UTF8( "Log_Volume" ), alloc ), 
         m_FrameAlloc     (framealloc),
         m_WriterTaskSync (writertaskid),
         m_PageReader     (IOMan_ILogIO::GetInstance()),
         m_PageWriter     (IOMan_ILogIO::GetInstance()),
         m_LogQueues      (m_Allocator),
         m_LogWriter      (m_Allocator, m_LogQueues, IOMan_ILogIO::GetInstance(), m_WriterTaskSync),
         m_IOPages        (m_Allocator, UTF8( "Log_Volume::WriteLogPages" )),
         m_QueueSyncMode  (syncmode),
         m_FlushMode      (Log_DeviceMinimizeSpace), // PTS 1130336 mb 2004-06-29
         m_SaveIterator   (alloc, framealloc, m_PageReader),
         m_CurrentQueueCount (0),                // PTS 1135304 mb 2005-05-24
         m_NumQueuesActive(0)                        // PTS 1135304 mb 2005-05-24
    { }
    /*!
       @brief   Creates archive log for new database

       @li This method is called on 'init config'.
       @li It is assumed that this method is called only once within the lifetime of a database.
       @li The archive log device is initialized between the given limits; all log pages are cleared.
       @li The log info page is initialized with default values.
       @li Log queues and log writer are created.
     */
    Log_ErrorState CreateForNewDatabase ( TaskID taskid );

    /*!
       @brief  Restarts archive log for existing database
       @param  taskid [in] id of the calling task
       @param  restartType [in] selects the operation for which the DB will be restarted
       @param  errlist [out] more hints in case of an error
       @return Log_ErrorState

       @li This method is called on 'restart'.
       @li The actual log parameters are retrieved from the existing log info page.
       @li Log queues and log writer are allocated if necessary.
     */
    Log_ErrorState Restart ( TaskID       taskid,
                             RestartType  restartType,
                             Msg_List    &errlist);
    
    /// Restarts archive log in order to perform a 'save log' on cold database
    Log_ErrorState RestartForSaveLogCold ( TaskID     taskid,
                                           Msg_List  &errlist );
    /*!
    @brief        Clears existing log device between given limits
    @since        PTS 1111525 mb 2002-01-24

    @param        taskid                    [in] id of calling task
    @param        firstOffsetToClear        [in] lowest offset on the device
    @param        lastOffsetToClear         [in] highest offset on the device
    @param        nextIOSequenceOnLog       [in] the Log_IOSequenceNo, which will be the next one, written on the log
    @param        firstPageIsEmpty          [out] returns, whether the page on firstOffsetToClear is really empty
    */
    void ClearLogPartially  (const TaskID                         taskid,
                             const Log_RawDeviceOffset            firstOffsetToClear, 
                             const Log_RawDeviceOffset            lastOffsetToClear,
                             const Log_IOSequenceNo               nextIOSequenceOnLog,
                                   bool                          &firstPageIsEmpty);

    /*!
    @brief        Clears existing log device between given limits

    @param        taskid                     [in] id of calling task
    @param        lastKnownIOSeqAfterClear   [in] the new value of lastKnownIOSeq
    */
    void ClearLogComplete
        (const TaskID                         taskid,
         const Log_IOSequenceNo               lastKnownIOSeqAfterClear);

    /*!
    @brief        Clears existing log device between given limits
    @since        PTS 1111525 mb 2002-01-24
    */
    void ClearLogComplete
        (const TaskID                         taskid);
    /// returns true, if the log area is ready for writing
    static bool IsReadyForWriting()
    {
        return m_Instance != 0 && m_Instance->m_State.IsReadyForWriting();
    }
    /// Deletes log queues and log writer
    void Delete();
    /// Destructor
    ~Log_Volume()
    {
        Delete();
    }
    /*!
        @brief   called for setting the correct IOSeqence and WriteOffset after restart/restore log

        This must be used, to set the IOSequence correctly after a
        restart/restore, because at the time, when UpdateLogInfoPage
        is called, nothing is known about how many IOSequences are
        Redone. This is especially true for HotStandby.
     */
    void UpdateIOSequenceAndOffset (      TaskID                taskid, //PTS 1113550 mb 2002-05-15
                                          IOSequenceNo          lastProcessedIOSequence,
                                    const Log_RawDeviceIterator nextWritePosition);

    /// Returns the number of log pages that are not saved
    DeviceBlockCount GetNumberOfUnsavedPages() const;

    /// describes the state of a save-iterator
    enum InitLogSaveResult
    {
        Okay,
        NoLogToSave,
        HistoryIsLost,
        IncompleteSegment,
        InitializeFailed
    };

    // defines how much of the unsaved log-pages will be denoted by the 
    // newly constructed Log_SaveIterator (PTS 1128703 mb 2004-04-23)
    enum SaveRange
    {
        AllAtOnce,                     ///< saves all Log_Pages with one Log-SaveIterator
        AllIncompleteSegmentsAllowed,  ///< saves in Segments, the last may be incomplete
        OnlyCompleteLogSegment         ///< saves only complete segments
    };

    /// allocates an iterator for saving the log
    const InitLogSaveResult InitLogSaveIterator( const tsp00_TaskId            taskId,
                                                       SAPDBMem_IRawAllocator &allocator,
                                                 const SaveRange               rangeToBeSaved, // PTS 1128703  
                                                 const SAPDB_Bool              repeatable,                           
                                                 const Log_DeviceBlockCount    blockSize);

    /// returns the current iterator for saving log
    Log_SaveIterator & GetLogSaveIter() { return m_SaveIterator; };

    /*!
       @brief          returns a new backup-count for numbering the log_save-files
       @return         next unused log-count

       This function increments the counter in the log_info-page
       of the device and writes the log_info-page to disk
     */
    SAPDB_UInt4    GetNextBackupCount(tsp00_TaskId    TaskId);

    /// This returns the distance in pages, which is used for checking for autosave
    DeviceBlockCount GetAutosaveDistance()
    {
        return m_autosaveDistance;    // PTS 1113361 mb 2002-01-11
    }

    /// sets/resest whether the log may overwritten without save_log
    bool SetAutomaticOverwrite (tsp00_TaskId taskid,
                                bool         enabled);

    /// returns the flag whether the log may overwritten without save_log
    bool IsAutomaticOverwrite (tsp00_TaskId taskid);

    /// result set for FreeLog-calls
    enum FreeLogResult
    {
        /// the operation succeded
        FreeLogOk,

        /// the IOSequence of the free command does not match to the last saved segment 
        IOSequenceDoesNotMatch,     

        /// open of log volume failed
        LogVolumeNotAccessible,
        
        /// no save log done before
        NoSaveLogExecutedBefore,
        
        /// no log freed due to repeatable-flage
        RepeatedLogSaveNeeded
    };
    
    /*!
    @brief frees the logPages after a save into a pipe

    frees the log which had been saved by the previous "save log into pipe".
    It is checked, that the given IOSequences matches the saved values from
    the previous save
    */
    FreeLogResult FreeLogForPipe(const tsp00_TaskId         taskid,
                                 const Log_IOSequenceNo     startIOSeq,
                                 const Log_IOSequenceNo     endIOSeq);

    /// This releases log pages, which are already backuped.
    FreeLogResult FreeLog (const tsp00_TaskId        taskid);

    /// This releases entries in a logpage up to a specified IOSequence 
    /// @attention This call is not synchronized with other accesses
    /// to m_bufferedPage or the logVolume.
    void TruncateLogPage (      TaskID               taskid,
                          const Log_IOSequenceNo     newLastIOSequence,
                          const Log_RawDeviceOffset  pageOffset);

    /// This releases entries in a logpage up to a specified IOSequence 
    /// @attention This call is not synchronized with other accesses
    /// to m_bufferedPage or the logVolume.
    void TruncateLogPage (      TaskID               taskid,
                          const Log_IOSequenceNo     newLastIOSequence,
                          const Log_RawDeviceOffset  pageOffset,
                                bool                &isEmpty);

public:
    /// Returns lowest offset on raw log device to be used for cyclical log write i/o
    /// Offset is located behind the leading log info page block(s) on the raw device.
    Log_RawDeviceOffset LogicalLogBeginOffset() const
    {
        return (RawDeviceOffset)(m_RawDeviceBegin.RawValue() + LOG_INFOBLOCKCOUNT);
    }

    /// Returns highest offset on raw log device to be used for cyclical log write i/o
    Log_RawDeviceOffset LogicalLogEndOffset() const
    {
        return m_RawDeviceEnd;
    }

    /// Returns offset of log info page on raw device
    RawDeviceOffset LogInfoPageOffset() const
    {
        return (RawDeviceOffset)(m_RawDeviceBegin.RawValue() + LOG_INFOPAGEOFFSET);
    }

    /// Returns the last (youngest) known IOSeq known on the LogDevice. This value
    /// is only extracted from the Log_InfoPage and therefore may not be exact.
    Log_IOSequenceNo GetLastKnownIOSeq() const
    {
        return  m_LogInfoPage.ReadLastKnownIOSequenceNo();
    }
    
    /// Returns the last (youngest) known Offset known on the LogDevice. This value
    /// is only extracted from the Log_InfoPage and therefore may not be exact.
    Log_RawDeviceOffset GetLastKnownOffset() const
    {
        return  m_LogInfoPage.ReadLastKnownOffset();
    }
    
    /// Returns the oldest known IOSeq known on the LogDevice. This value
    /// is only extracted from the Log_InfoPage and therefore may not be exact.
    Log_IOSequenceNo GetOldestKnownIOSeq() const
    {
        return  m_LogInfoPage.ReadOldestKnownIOSequenceNo();
    }
    
    /// returns true, if a subsequent ResoreveForEot will not block because of LogFull
    bool ReserveForEotWillNotBlock() const
    {
        return m_LogWriter.PagesAreFreeForWrite();
    }
    
    /// Checks whether no more log can be written (Not valid for utility)
    SAPDB_Bool LogIsFull ()
    {
        return m_LogWriter.LogIsFull()
               ||
               m_LogQueues[0]->IsLogFull();
    }

    /// Checks whether nothing was written to the log device since a clear log
    bool LogIsEmpty ()
    {
        SAPDBERR_ASSERT_STATE (m_LogInfoPage.IsAssigned());
        SAPDBERR_ASSERT_STATE (Log_DeviceStateClearing  != m_LogInfoPage.ReadDeviceState());
        SAPDBERR_ASSERT_STATE (Log_DeviceStatePartiallyClearing  != m_LogInfoPage.ReadDeviceState());

        return m_LogInfoPage.ReadLastKnownOffset().IsInvalid();  //PTS 1113550 mb 2002-05-2
    }

    /// returns the size of the archive log device
    DeviceBlockCount GetDeviceSize ()
    {
        return GetWriter().UserLogSize();  // PTS 1124876 UH 2003-10-23
    }

    /// returns true if Logwriting is currently suspended by user
    bool LogwriterIsSuspended ()
    {
        return GetWriter().IsSuspendedByUser();
    }

    /// returns statistic information about the device
    void GetDeviceUsage ( SAPDB_UInt &deviceSize,
                          SAPDB_UInt &pagesUsed,
                          SAPDB_UInt &pagesNotSaved,
                          SAPDB_UInt &pendingLogSize );

    /// returns the size of a log queue specified by configuration parameter
    static SAPDB_UInt QueueSize();
    
    /// sets the number log queues
    bool SetQueueCount(SAPDB_UInt queueCount, Msg_List & errlist);
    
    /// returns summarized statistics about all Log_Queue.
    /// This method is OBSOLETE and replaced by Log_Queue::Get*. This method is only used by
    /// the old fashioned systemview MONITOR_LOG.
    void GetQueueUsage ( SAPDB_UInt4    &queueSize,
                         SAPDB_UInt4    &maxPagesUsedInQueue,
                         SAPDB_UInt4    &countReserveRequests,
                         SAPDB_UInt4    &countWaitForIO,
                         SAPDB_UInt4    &countGroupCommit,
                         SAPDB_UInt4    &countWaitForQueueFree,
                         SAPDB_UInt4    &maxWaitForPageIO );
    
    /// resets the statistics for the LogQueue
    void ResetStatistics (tsp00_TaskId taskid);

    /// returns information relevant for the restart
    void GetRestartVersion ( tsp00_TaskId      taskid,
                             Log_IOSequenceNo &firstiosequence,
                             SAPDB_Char       *dbident,
                             SAPDB_UInt        dbidentSize,
                             RTE_Nodename     &masterNodeName );

    /// returns the db-identifier
    void GetLogDBIdent ( SAPDB_Char       *dbident );

    /// sets the database identifier
    void SetLogDBIdent ( tsp00_TaskId    taskid,
                         tsp00_Line      dbIdent);

    /// returns the NodeId of the master for the HotStandby
    bool GetMasterNodeName ( RTE_Nodename &masterNodename );

    /// sets the NodeId of the Master for HotStandby
    void SetMasterNodeName ( tsp00_TaskId        taskid,
                             const RTE_Nodename &masterNodename );
                           
    /// returns the position on the LogVolume, where the next page will be written.
    /// @attention For LogMode MINIMIZE_SPACE, the result may be not really exact,
    /// because it is determined at write-time, weather the page will be written
    /// again on the same position or not.
    Log_RawDeviceIterator GetWriterPosition() const;
    
    /// returns the state of the LogDevice
    const Log_DeviceState GetDeviceState () const;

    /// returns the current active flushmode of the logarea
    const Log_FlushMode GetFlushMode() const;

    /// sets the state of the device
    bool SetVolumeState (tsp00_TaskId    taskid,
                         Log_DeviceState state);

    /// sets/resets the flag in the Log_Infopage, whether the LogVolume is enabled
    bool SetLogWriting (tsp00_TaskId taskid,
                        bool         on);

    /// reads the number of LogQueues from the Log_InfoPage
    SAPDB_UInt2 GetQueueCountFromInfoPage (tsp00_TaskId taskid);
    
    /// returns the number of logging-queues which are actually allocated
    /// PTS 11335304 mb 2005-05-20
    SAPDB_UInt2 GetNumQueuesAllocated ();

    /// returns the flag in the Log_Infopage, whether the LogVolume is enabled
    bool IsDevspaceEnabled (tsp00_TaskId taskid);
    
    /// constant for writing readable code when using GetQueueForWriting.
    /// Currently only the savepoint uses a static determined queue
    #define LOG_USE_QUEUE_ZERO true

    /// Returns log queue instance that serves calling user task. This queue has to released
    /// by calling ReleaseQueue in order to allow a synamic change of the parameter LOG_QUEUE_COUNT
    void GetQueueForWriting(tsp00_TaskId  taskid,
                            Log_Queue *  &logQueue,
                            const bool    useQueueZero=false);
                            
    /// returns the currently number of active logging queues. This method uses a
    /// dirty access without synchronisation - thus the value may be outdated. 
    SAPDB_UInt4 GetNumQueuesActiveDirty() const { return m_NumQueuesActive; }
       
    /// releases a Log_Queue which has been get by GetQueueForWriting (PTS 1135304 mb 2005-05-24)
    void ReleaseQueue (tsp00_TaskId  taskid,
                       Log_Queue* &logQueue);
   
    /// Returns log queue instance with the specific Log_QueueID
    Log_Queue& GetQueueForStatistic(const Log_QueueID queueId);

    /// Returns log writer instance
    Log_Writer& GetWriter()
    {
        return m_LogWriter;
    }

    /// Returns id of associated writer task
    TaskID GetWriterTaskID() const
    {
        return m_WriterTaskSync.GetTaskID();
    }

    /// collects some information about the LogVoulme. 
    /// returns false, if logging is not initialized
    bool GetLogInfo ( tsp00_TaskId              taskid,
                      Kernel_Date              &date,
                      Kernel_Time              &time,
                      Log_FlushMode            &flushmode,
                      Log_DeviceState          &devicestate,
                      SAPDB_UInt2              &queuecount,
                      Log_RawDeviceOffset      &cycleendoffset,
                      Log_RawDeviceOffset      &oldestnotsavedoffset,
                      Log_IOSequenceNo         &oldestnotsavedsequence,
                      Log_RawDeviceOffset      &lastknownoffset,
                      Log_IOSequenceNo         &lastknownsequence,
                      SAPDB_UInt4              &backupcount,
                      Kernel_Date              &untildate,
                      Kernel_Time              &untiltime,
                      Log_RawDeviceOffset      &untilclearloweroffset,
                      Log_RawDeviceOffset      &untilclearupperoffset,
                      Log_IOSequenceNo         &untilsequenceafterredo,
                      Log_IOSequenceNo         &savepointsequence,
                      Log_RawDeviceOffset      &savepointdevoffset,
                      Data_PageFrameOffsetType &savepointoffset );

    /*!
       @brief          This offers an iterator, to access afterimages.
       @param          alloc [in] Allocator for dynamic memory allocation
       @param          taskId [in] current taskId
       @param          RTECancelFlag [in] flag from the TransContext
       @param          untilDate [in] Date for Restore/Recover-Until
       @param          untilTime [in] Time for Restore/Recover-Until
       @param          untilIOSequence [in] stop-iosequence for Restore/Recover-Until
       @param          standbySynchronizer [in] Reference to the synchronization object for standby-instances
       @return         Log_VolumeIterator
     */
    Log_VolumeIterator* GetLogVolumeIterator (
                SAPDBMem_IRawAllocator    &alloc,
                tsp00_TaskId               taskId,
                boolean                    *RTECancelFlag,
                Kernel_Date               &untilDate,         // PTS 1111525 mb 2001-01-15
                Kernel_Time               &untilTime,         // PTS 1111525 mb 2001-01-15
                Log_IOSequenceNo          &untilIOSequence,   // PTS 1121539 mb 2003-02-06
                Rst_StandbySynchronizer   &standbySynchronizer)
    const;
                                                  
    /*!
       @brief          This offers an iterator, to access afterimages.
       @param          alloc [in] Allocator for memory allocation
       @param          taskId [in] current taskid
       @param          RTECancelFlag [in] flag which indicates a cancel of this task
       @param          lastKnownOnTape [in] Last IOSequence to be read from tape
       @param          untilDate [in]
       @param          untilTime [in]
       @param          untilIOSequence [in] stop-iosequence for Restore/Recover-Until
       @return         Log_VolumeIterator

     */

    Log_VolumeIterator* GetTapeIterator (
                SAPDBMem_IRawAllocator     &alloc,
                tsp00_TaskId                taskId,
                boolean                    *RTECancelFlag,
                Log_IOSequenceNo            lastKnownOnTape,
                Kernel_Date                &untilDate,         // PTS 1111525 mb 2001-01-15
                Kernel_Time                &untilTime,         // PTS 1111525 mb 2001-01-15
                Log_IOSequenceNo           &untilIOSequence)   // PTS 1121539 mb 2003-02-06
    const;

    /// The iterator is set to the last savepoint entry of the restart record.
    void SetToLastSavepointEntry (tsp00_TaskId  taskId,Log_VolumeIterator &iter);

    /// reads the iosequence of the last savepoint from the restartrecord
    Log_IOSequenceNo  GetSavepointIOSeq();

    /// The iterator is set to the begin of the page denoted by offset
    void SetToBeginOfPage (Log_VolumeIterator    &iter,
                           Log_RawDeviceOffset        offset);

    /// The given bytes from the restartrecord are copied into the
    /// infopage. The loginfo page is written to disk. The version is set
    /// if the corresponding flag is true (i.e. for a restart-savepoint)
    void WriteSavepointValuesAndFlushInfoPage (TaskID                   taskid,
                                               const bool               isRestartSavepoint, // PTS 1137938 mb 2005-09-19
                                               tsp00_Version            kernelVersion,
                                               const SAPDBFields_Field& rrpart);

    /// The parameters describing the log device are written to the trace.
    void WriteDeviceParametersToTrace ( );
    
    /// This write the internal info page to the devspace.  Date and time are updated.
    void FlushLogInfoPage (tsp00_TaskId taskid);

    /// This is used by the writer to update the log info page.
    void UpdateInfoPageFromWriter (tsp00_TaskId        writertaskid,
                                   Log_IOSequenceNo    lastKnownSequence,
                                   Log_RawDeviceOffset lastKnownOffset);
    
    /// This is used by the writer if the first offset of the added log device is to be written.
    void ChangeEndOffsetAfterAddDevSpace (tsp00_TaskId        writertaskid,
                                          Log_RawDeviceOffset newEndOffset);
    
    /// Sets the value of the smallest IOSequence/Offset on the LogVolume by
    /// scanning the LogDevice and stores it for later retrieval by
    /// GetFirstKnownBeforeRestoreLog. 
    /// @attention  Must not be called after Restart
    void ScanLogForFirstKnown (TaskID           taskid);

    /// searches the log_volume for the given IOSequence and returns the offset,
    /// on which this Log_IOSequenceNo resides (PTS 1129184)
    Log_RawDeviceOffset SearchSavepointOffset(      TaskID              taskid,
                                              const Log_IOSequenceNo    savepointIOSequence);

    /// This method is returns the values, which had been set by ScanLogForFirstKnown
    void GetFirstKnownBeforeRestoreLog (RawDeviceOffset     &firstKnownOffset,
                                        IOSequenceNo        &firstKnownIOSeq) 
    {
        firstKnownOffset = m_FirstKnownOffset;
        firstKnownIOSeq  = m_FirstKnownIOSeq;
    }

    /// This method (used by ADD LOG VOLUME) sets a new size of the LogVolume
    void SetNewDeviceEndOffset ( SAPDB_UInt4 newEndOffset );

    ///  This returns if log volumes are attached.
    bool IsLogAreaAccessible()
    {
        return m_State.IsLogAreaAccessible();
    }

    ///  This opens all log volumes. false is returnd on error.
    bool OpenLogArea ( TaskID taskid,
                       bool   forWrite = true);

    ///  This closes the log device.
    void CloseLogArea (TaskID taskid);
    
    ///  This opens the log device and prepares the InfoPage for reading.
    bool PrepareLogInfoPageForRead (TaskID taskid);

    ///  This method is called by the timeout task.
    void CheckForLogSaveAndSavepoint (Trans_Context &trans);

    ///  The logwriter is suspended until Resume() is called.
    ///  The returned iosequence is the last used iosequence.
    ///  @return Returns false if this is called more then by one task.
    ///  @since  PTS 1114053 UH 2002-10-22 new
    bool SuspendAndGetLastWrittenIOSequence ( TaskID            taskid,
                                              Log_IOSequenceNo &iosequence );

    ///  The logwriter is resumed after suspended by SuspendAndGetLastWrittenIOSequence.
    ///  @since PTS 1114053 UH 2002-10-22 new
    void ResumeByUser ();
    
    /// This causes the internal loginfo page be re-read from log volume.
    /// This is used by standby's which want to TakeOver
    void ReadLogInfoPageFromLogVolume ( TaskID taskid );

    /// adds a new event for the log usage
    bool AddLogFillingEvent (const SAPDB_Int              threshold,
                             const tsp31_event_prio_Param priority)
    {
		// PTS 1124876 UH 2003-10-23 new
        return m_LogWriter.AddLogFillingEvent (threshold, priority);
    }

    /// deletes a event for the log usage
    bool DelLogFillingEvent (const SAPDB_Int threshold)
    {
		// PTS 1124876 UH 2003-10-23 new
        return m_LogWriter.DelLogFillingEvent (threshold);
    }
    
    /// checks all pages of a logbackup by using k39locate_logentries_in_page 
    /// and performing appropriate checks
    SAPDB_Bool CheckLogBackup(      TaskID            taskId,
                                    pasbool          &rteCancelFlag,
                              const Log_IOSequenceNo  firstIOSeq,
                              const Log_IOSequenceNo  lastIOSeq);
private:

    /*!
    @brief        Clears existing log device between given limits
    @since        PTS 1111525 mb 2002-01-29 new
    @param        taskid                [in]  id of calling task
    @param        firstOffsetToClear    [in]  offset of first page to be cleared
    @param        lastOffsetToClear     [in]  offset behind last page to be cleared 
    @param        lastKnownOffset       [in]  new value for lastKnownOffset in the LogInfoPage
    @param        oldestNotSavedOffset  [in]  new value for oldestNotSaved in the LogInfoPage
    @param        oldestNotSavedIOSeq   [in]  new value for oldestNotSavedIOSeq in the LogInfoPage
    @param        stateBeforeFormat     [in]  the state, which will be set during formatLog
    @param        stateAfterFormat      [in]  the state, which will be set during formatLog
    @param        doFormatLog           [in]  format log volume with empty pages
    */
    void ClearLogIntern (                      // PTS 1111525 mb 2002-01-29 new
        const TaskID                          taskid,
        const Log_RawDeviceOffset             firstOffsetToClear, 
        const Log_RawDeviceOffset             lastOffsetToClear,
        const Log_RawDeviceOffset             lastKnownOffset,
        const Log_RawDeviceOffset             oldestNotSavedOffset,
        const Log_IOSequenceNo                oldestNotSavedIOSeq,
        const Log_DeviceState                 stateBeforeFormat,
        const Log_DeviceState                 stateAfterFormat,
        const SAPDB_Bool                      doFormatLog);
        
    /// repairs a LogPage, if it is the youngest page on the logVolume
    /// @param taskid               id of the current task (needed for IO)
    /// @param lastKnownQueueSeqNo  the up to now greatest read Queue-SequenceNumber. It is
    ///                             used for checking the consistency
    /// @param iter                 An iterator, which points to the current read position
    /// @param corruptPage          Reference to the already read page, which is faulty
    /// @param lastKnownSeqNo       Reference to the up to now youngest IOSequenceNumber
    /// @param lastKnownOffset      Reference to the offset of the up to now youngest found page
    /// @param errlist              additional error messages
    /// @return false on any error
    bool RepairYoungestPage( 
             TaskID                         taskid,
       const Log_IOSequenceNo               lastKnownQueueSeqNo,
             RawDeviceIterator              iter,
             Log_Page                      &corruptPage,
             Log_IOSequenceNo              &lastKnownSeqNo,
             Log_RawDeviceOffset           &lastKnownOffset,
             Msg_List                      &errlist); // UH 2005-10-18 added errlist and return bool

    ///  Determines restart parameters for the log writer by scanning 
    ///  the log device and the log info page.
    Log_ErrorState UpdateLogInfoPage (TaskID     taskid,
                                      Msg_List  &errlist);

    ///  checks the Restartrecord for consistency with the actual log
    Log_ErrorState CheckLogAndDataCompatibility (TaskID     taskid,
                                                 Msg_List  &errlist);

    ///  Returns true, if the given ioseqno is found on the page found
    ///  on the given offset.
    SAPDB_Bool CheckOffsetAndIOsequence (TaskID           taskid,
                                         RawDeviceOffset  offset,
                                         IOSequenceNo     seqno,
                                         Msg_List        &errlist);
    /// allocates the pageframe for the loginfopage
    SAPDB_Bool AllocateLogInfoPage();
    
    ///  Allocates the log info page frame only and reads the page from the device
    SAPDB_Bool AllocateAndReadInfoPage( tsp00_TaskId taskid,
                                        bool         readPage );

    /// allocates everything needed except the Log_Queues
    SAPDB_Bool AllocatePageFrames( SAPDB_UInt   ioblockcount );

    ///  Allocates page frames for info page and page buffer. Returns true if allocated
    SAPDB_Bool AllocateAllAndReadInfoPage( tsp00_TaskId taskid,
                                           SAPDB_UInt   ioblockCount,
                                           SAPDB_UInt   queueSize,
                                           SAPDB_UInt   queueCount );

    ///  Creates info page with default values
    void CreateInfoPage( tsp00_TaskId taskid,
                         SAPDB_UInt   queuecount );

    /*!
    @brief        Allocates and initializes multiple log queues. The number of Log_Queues is determined
                  by Log_InfoPage. The QueueSize is given by the parameter LOG_IO_QUEUE.
    @return       (SAPDB_Bool) true if successful
    */
    SAPDB_Bool CreateLogQueues ( SAPDB_UInt queuecount,
                                 SAPDB_UInt queuesize );

    /*!
    @brief        Initializes log writer
    @param        offset         [in] to start writing
    @param        seqno          [in] to use for next log page
    @param        ioblockcount   [in] max. count of i/o blocks for multiple write i/o
    @return       (SAPDB_Bool) true if successful
    */
    SAPDB_Bool InitLogWriter ( RawDeviceOffset offset,
                               IOSequenceNo    seqno,
                               SAPDB_UInt      ioblockcount);

    ///  Initializes log writer 
    void ResetLogWriter (TaskID          taskid);

    ///  Clears existing log device between given limits
    ///  (beginoffs may be greater than endoffs)
    void FormatLog
        (const TaskID                         taskid,
         const RawDeviceOffset                beginOffset,
         const RawDeviceOffset                endOffset);

    ///  Clears existing log device between given limits
    ///  @attention beginoffs must not be greater than endoffs
    void FormatLogLinear
        (const TaskID                         taskid,
         const RawDeviceOffset                beginoffs,
         const RawDeviceOffset                endoffs);

    ///  Resets the LogFull state of the log_queues if the log is no longer full
    void ClearLogFullQueueState(TaskID      taskid);

    void ReadLogPageUnchecked(const tsp00_TaskId          taskid,
                                    Log_Page            & logPage,
                              const Log_RawDeviceOffset   offset);


    /*! @name Implementation of the Interface Log_IDiagnose  */
    /*@{*/
        virtual bool AllocateLogPageframe(Log_Page   & logPage);
        virtual void ReleaseLogPageframe(Log_Page    & logPage);
        virtual void ReadLogPage(const tsp00_TaskId          taskid,
                                       Log_Page            & logPage,
                                 const Log_RawDeviceOffset   offset);
        virtual void WriteLogPage(const tsp00_TaskId          taskid,
                                        Log_Page            & logPage,
                                  const Log_RawDeviceOffset   offset);
    /*@}*/


private:
    /// pointer to singleton instance 
    static Log_Volume*                                  m_Instance;

    /// raw allocator 
    RTEMem_AllocatorWrapper                             m_Allocator;
    
    ///  allocator for page frames
    FrameAllocator&                                     m_FrameAlloc;
    
    /// log writer task synchronization object
    Log_WriterTaskSynchronizer                          m_WriterTaskSync;
    
    /// circular list of log queues of this device
    QueueVector                                         m_LogQueues;
    
    /// log queue synchronization mode
    QueueSyncMode                                       m_QueueSyncMode;
    
    /// maximum number of concurrently usable logqueues (set online using LOG_QUEUE_COUNT)
    /// PTS 1135304 mb 2005-05-20
    SAPDB_UInt4                                         m_CurrentQueueCount;

    /// Number of Log_Queue 's on which transactions are active
    /// PTS 1135304 mb 2005-05-20
    SAPDB_UInt4                                         m_NumQueuesActive;

    /// spinlock for coordinating the access to m_NumQueuesActive
    mutable RTESync_Spinlock                            m_QueueCoordSpinlock;

    /// log writer flush mode
    Log_FlushMode                                       m_FlushMode;
    
    /// log writer instance of this log device
    Log_Writer                                          m_LogWriter;
    
    /// reader for raw log device
    IOMan_ILogIO&                                       m_PageReader;
    
    /// writer for raw log device
    IOMan_ILogIO&                                       m_PageWriter;
    
    /// log infopage handler
    Log_InfoPage                                        m_LogInfoPage;

    /// description: buffer for one log page
    Log_Page                                            m_BufferedPage;

    /// description: collection of page frames used for write i/o requests
    IOMan_LogPages                                      m_IOPages;

    /// description: offset on raw device where log begins
    Log_RawDeviceOffset                                     m_RawDeviceBegin;

    /// description: offset on raw device where log ends
    Log_RawDeviceOffset                                     m_RawDeviceEnd;

    /// defines the possible states of the log volume.
    class State
    {
    public:
        State()
        : m_LogAreaAccessible(false),
          m_LogInfoPageReadable(false),
          m_LogIsReadyForWriting(false)
        {}
        void SetLogAreaAccessible()
            { m_LogAreaAccessible = true; }
        void SetLogAreaNotAccessible()
            { m_LogAreaAccessible = false; }
        void SetLogInfoPageReadable()
            { m_LogInfoPageReadable = true; }
        void SetReadyForWriting()
            { m_LogIsReadyForWriting = true; }
        bool IsLogAreaAccessible() const
            { return m_LogAreaAccessible; }
        bool IsLogInfoPageReadable() const
            { return m_LogInfoPageReadable; }
        bool IsReadyForWriting() const
        {
            return m_LogAreaAccessible     &&
                   m_LogInfoPageReadable   &&
                   m_LogIsReadyForWriting;
        }
    private:
        bool m_LogAreaAccessible;
        bool m_LogInfoPageReadable;
        bool m_LogIsReadyForWriting;
    };
    
    /// This state indicates various states of the log volume

    State                                               m_State;
    
    /// holds the offset of the first known page
                  /// ONLY VALID DURING RESTART
    RawDeviceOffset                                     m_FirstKnownOffset;

    /// holds the IOSequence of the first known page
                  /// ONLY VALID DURING RESTART
    IOSequenceNo                                        m_FirstKnownIOSeq;
    
    /// holds the size of a saved LogSegment in pages (PTS 1113361 mb 2002-01-11)
    DeviceBlockCount                                    m_autosaveDistance;
    
    /// the iterator, which is used to read the logpages to be saved
    Log_SaveIterator                                    m_SaveIterator;
};


#endif  // LOG_VOLUME_HPP
