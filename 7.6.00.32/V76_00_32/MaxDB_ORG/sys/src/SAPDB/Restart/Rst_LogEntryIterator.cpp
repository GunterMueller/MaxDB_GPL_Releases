/*!
    @ingroup        Restart
    @file           Rst_LogEntryIterator.cpp
    @author         MartinB
    @brief          implementation for the reading of logEntries from tape and logVolume
                    for redo
*/
/*

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



 */

/*===========================================================================*
*  INCLUDES                                                                  *
*============================================================================*/

#include "hkb57_1.h" // k57restartrec
#include "Restart/Rst_LogEntryIterator.hpp"
#include "Logging/Log_Volume.hpp"
#include "RunTime/RTE_Message.hpp"
#include "Messages/Msg_List.hpp"


bool Rst_LogEntryIterator::Initialize (       tsp00_TaskId            taskId,
                                              SAPDBMem_IRawAllocator &redoReadTaskAllocator,
                                              boolean                *RTECancelFlag,
                                        const Log_IOSequenceNo        oldestKnownIOSeq,
                                              Msg_List               &errlist
                                       )
{
    SAPDBTRACE_METHOD_DEBUG ("Rst_LogEntryIterator::Initialize", LogTrans_Trace, 5);

    m_pAllocator = &redoReadTaskAllocator;

    Log_Volume              &log = Log_Volume::Instance();
    Log_IOSequenceNo         lastProcessedIOSequence;
    Log_IOSequenceNo         firstIOSeqOfLogVolume;
    Log_RawDeviceOffset      firstKnownOffset;
    bool                     incrementIOSeq = true;
    tsp00_TaskId             taskToBeResumedAfterInit = cgg_nil_pid;

    if ( Rst_RedoKindStandby == m_RedoKind )
        m_standbySynchronizer.InitialSuspend(taskId, taskToBeResumedAfterInit);
        
    m_taskId = taskId;
    m_RTECancelFlag = RTECancelFlag;
    if (   ((m_untilDate.IsValid() && m_untilTime.IsValid()) || (m_untilIOSequence.IsValid()))
        && (m_RedoKind != Rst_RedoKindRestoreLog))
    // PTS 1119223 mb 2002-12-10 only for RestartUntil the scan of the Log is done
    {
        log.ScanLogForFirstKnown(m_taskId);
    }

    log.GetFirstKnownBeforeRestoreLog(firstKnownOffset, firstIOSeqOfLogVolume);

    // first check for a iterator, which reads from a tape
    if ( m_RedoKind == Rst_RedoKindRestoreLog )
    {
        if (firstIOSeqOfLogVolume.IsValid())
        // there is something on the log-volume
        {
            if (!log.GetSavepointIOSeq().IsInRange(firstIOSeqOfLogVolume, log.GetLastKnownIOSeq()))
            // but the log-entries on the log-volume are not sufficient
            // => use tapes for recover-log
            {
                Log_IOSequenceNo         lastIOSeqFromTape;
                if (firstIOSeqOfLogVolume != MIN_IOSEQUENCE)
                {
                    lastIOSeqFromTape = firstIOSeqOfLogVolume - 1;
                }
                RTE_Message(Restart_Exception(__FILE__,__LINE__,
                                          RST_RECOV_TAPE,
                                          SAPDB_ToString(log.GetSavepointIOSeq(),_T_d),
                                          SAPDB_ToString(lastIOSeqFromTape,_T_d)));
                m_currentIter = log.GetTapeIterator(  *m_pAllocator,
                                                       m_taskId,
                                                       m_RTECancelFlag,
                                                       lastIOSeqFromTape,
                                                       m_untilDate,
                                                       m_untilTime,
                                                       m_untilIOSequence);
                m_currentIter->Initialize();
                log.SetToLastSavepointEntry (m_taskId, *m_currentIter);
            }
        }
        else
        // there is nothing on the log-volume
        // => recover from tape only
        {
            Log_IOSequenceNo         lastIOSeqFromTape; //invalid
            RTE_Message(Restart_Exception(__FILE__,__LINE__,
                                      RST_RECOV_TAPE,
                                      SAPDB_ToString(log.GetSavepointIOSeq(),_T_d),
                                      SAPDB_ToString(lastIOSeqFromTape,_T_d)));
            m_currentIter = log.GetTapeIterator(  *m_pAllocator,
                                                   m_taskId,
                                                   m_RTECancelFlag,
                                                   lastIOSeqFromTape,
                                                   m_untilDate,
                                                   m_untilTime,
                                                   m_untilIOSequence);
            m_currentIter->Initialize();
            log.SetToLastSavepointEntry (m_taskId, *m_currentIter);
        }
    }

    // if no tape-iterator has been created: create an iterator for
    // reading from the log-volume
        
    if (NULL == m_currentIter)
    {
        if (!log.LogIsEmpty()) 
        // Redo entries on the log-device only if it hasn't been read already and if there is something on it
        {
            m_currentIter = log.GetLogVolumeIterator (  *m_pAllocator,
                                                         taskId,
                                                         m_RTECancelFlag,
                                                         m_untilDate,
                                                         m_untilTime,
                                                         m_untilIOSequence,
                                                         m_standbySynchronizer);
            m_currentIter->Initialize();
            log.SetToLastSavepointEntry (m_taskId, *m_currentIter);

            if ( LogTrans_Trace.TracesLevel(7) )
                m_currentIter->WriteToTrace ("ReadIter");

            RTE_Message(Restart_Exception(__FILE__,__LINE__,
                                      RST_RECOV_LOG_VOL,
                                      SAPDB_ToString(m_currentIter->GetIOSequence() ,_T_d)));
        }
    }
    else if (m_RedoKind != Rst_RedoKindRestoreLog && log.LogIsEmpty())
    {
        // no log has been restored (neither from tape, nor from logVolume)
        // so the log-iosequences has to match the savepoint-iosequence
        // PTS 1116123 mb 2002-06-11
        m_lastProcessedIOSeq = k57restartrec->rstLastSavept_kb00().svpIOsequence_kb00;
    }
    
    // there may be a suspended SYNCHRONIZE, which has to be awaked after the initialization has finished
    if (cgg_nil_pid != taskToBeResumedAfterInit)
    {
        vresume(taskToBeResumedAfterInit);
    }
    return true;
}

void Rst_LogEntryIterator::DeleteIter()
{
    SAPDBTRACE_METHOD_DEBUG ("Rst_LogEntryIterator::DeleteIter", LogTrans_Trace, 5);


    if (NULL != m_currentIter)
    {
        m_finalState =  m_currentIter->GetState();
        m_currentIter->Delete();
        if ( m_pAllocator != 0 )
            destroy(m_currentIter,*m_pAllocator);
    }
}

Rst_LogEntryIterator& Rst_LogEntryIterator::operator ++ ()
{
    m_lastProcessedIOSeq = m_currentIter->GetIOSequence();
    m_lastReadOffset = m_currentIter->GetPosition(); 
    
    ++(*m_currentIter);

    switch ( m_currentIter->GetState() )
    {
        case Log_VolumeIterator::Iter_ok:
            break;

        case Log_VolumeIterator::Iter_untilReached:
            // until reached by checking the timestamp on the LogPage
            // checking the timestamp of the commit-logentries is done in the RedoReadTask
            break;

        case Log_VolumeIterator::Iter_allRead:
            if ( Log_VolumeIterator::TapeIterator == m_currentIter->GetSourceType() )
            {
                m_currentIter->Delete();
                destroy(m_currentIter,*m_pAllocator);

                Log_RawDeviceOffset  firstKnownOffset;
                Log_IOSequenceNo     firstIOSeqOfLogVolume;
                Log_Volume          &log = Log_Volume::Instance();

                log.GetFirstKnownBeforeRestoreLog(firstKnownOffset, firstIOSeqOfLogVolume);
                if (firstIOSeqOfLogVolume.IsValid())
                {
                    m_currentIter = log.GetLogVolumeIterator (*m_pAllocator,
                                                               m_taskId,
                                                               m_RTECancelFlag,
                                                               m_untilDate,
                                                               m_untilTime,
                                                               m_untilIOSequence,
                                                               m_standbySynchronizer);
                    m_currentIter->Initialize();
                    log.SetToBeginOfPage (*m_currentIter, firstKnownOffset);
                }
            }
            break;
    }
    return *this;
}

bool Rst_LogEntryIterator::UntilDateIsReached(Kernel_Date readDate, Kernel_Time readTime)
{
    SAPDBTRACE_METHOD_DEBUG ("Rst_LogEntryIterator::UntilDateIsReached", LogTrans_Trace, 5);

    Log_IOSequenceNo currentIOSeq = m_currentIter->GetIOSequence();
    if (m_currentIter->UntilDateIsReached(readDate, readTime))
    {
        if (currentIOSeq == m_lastProcessedIOSeq)
        // multiple LogEntries had been written with the current IOSequence
        {
            // this IOSequence is not really the last complete processed
            // ==> adjust it to the real meant value
            --m_lastProcessedIOSeq;
        }
        return true;
    }
    else
    {
        return false;
    }
}    

void Rst_LogEntryIterator::ResumeReaderForRestart()
{
    SAPDBTRACE_METHOD_DEBUG ("Rst_LogEntryIterator::ResumeReaderForRestart", LogTrans_Trace, 5);
    
    m_standbySynchronizer.ResumeReaderForRestart();
}

void Rst_LogEntryIterator::SuspendRedoReader(tsp00_TaskId     taskId)
{
    SAPDBTRACE_METHOD_DEBUG ("Rst_LogEntryIterator::SuspendRedoReader", LogTrans_Trace, 5);
    
    m_standbySynchronizer.SuspendRedoReader(taskId);
}

void Rst_LogEntryIterator::SetLastRedoReadOffset(Log_RawDeviceOffset lastRedoOffset,
                                                 tsp00_TaskId     taskId)
{
    SAPDBTRACE_METHOD_DEBUG ("Rst_LogEntryIterator::SetLastRedoReadOffset", LogTrans_Trace, 5);

    m_standbySynchronizer.SetLastValidOffsetAndResume(lastRedoOffset, taskId);
}

