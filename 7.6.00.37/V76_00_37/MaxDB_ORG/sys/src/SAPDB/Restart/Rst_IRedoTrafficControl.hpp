/*!
    @ingroup        Restart
    @file           Rst_IRedoTrafficControl.hpp
    @author         UweH
    @author         JuergenA
    @brief          defines an interface class of a singleton to synchronize the parallel redo
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
#ifndef RST_IREDO_TRAFFIC_CONTROL_H
#define RST_IREDO_TRAFFIC_CONTROL_H

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "Logging/Log_Types.hpp"
#include "Logging/Log_Transaction.hpp"
#include "Restart/Rst_Types.hpp"
#include "gsp00.h"
#include "ggg00.h"
/*!
   @interface Rst_IRedoTrafficControl
   @brief     interface class of a singleton to synchronize the parallel redo
 */
class Rst_IRedoTrafficControl
{
public:
    /// This is called by the redo reader to prevent that all transentries become used
    /// The caller waits if g01maxuser jobs were already inserted to the redo list.
    virtual bool CheckJobCount (tsp00_TaskId          taskid,
                                SAPDBErr_MessageList &errlist) = 0; // UH 2005-01-03
    /// This is called by the RedoReader if no more transentries are available
    /// The caller is suspended until one RedoTask is finished.
    virtual bool WaitForFinishedRedoTransaction (tsp00_TaskId          taskid,
                                                 SAPDBErr_MessageList &errlist) = 0;
    /*!
        @brief          inserts a job. This method is provided for the log reader task.
        @param          LogReaderTaskId        [in] 
        @param          TransIndex             [in] 
        @param          EndOfTransIoSeq        [in] 
        @param          UndoOrRedo             [in] 
        @param          oldestKnownEOTSequence [in] lower bound of processed IOSequences. Needed for comparison of IOSequences
        @param          checkAscendingSequence [in]
        @param          Transaction            [in] 
        @param          errlist                [out] 
        @return         IsOk
     */
    virtual bool InsertJob (tsp00_TaskId          LogReaderTaskId,
                            tgg00_TransIndex      TransIndex,
                            Log_IOSequenceNo      EndOfTransIoSeq,
                            Log_ActionType        UndoOrRedo,
                            Log_IOSequenceNo      oldestKnownEOTSequence, // PTS 1124684 UH 2003-10-16
                            bool                  checkAscendingSequence, // PTS 1113641 UH 2002-01-25
                            Log_Transaction      &Transaction,
                            SAPDBErr_MessageList &errlist)          = 0;
	/// Executes vacant jobs. This method is provided for redo tasks.
    virtual bool ExecuteJobs (tsp00_TaskId          taskid,
                              tgg00_TransContext   &oldcontext,
                              SAPDBErr_MessageList &errlist) = 0;
    /// result for CheckIoSeq
    enum CheckIoSeqResult // PTS ? 2005-01-03
    {
        CheckIoSeqOk,
        CheckIoSeqRedoIsAborted,
        CheckIoSeqError
    };
    /*!
        @brief  a redo task checks if preceding transactions are closed
        @param  RedoTaskId [in] 
        @param  WaitContext [in/out] 
        @param  IoSeq [in] sequence of the log entry handled by the redo task
        @param  waitInserted [out] if true the caller has to wait
        @param  errlist [out] error messages
        @return CheckIoSeqResult
     */
    virtual CheckIoSeqResult CheckIoSeq (tsp00_TaskId          RedoTaskId,
                                         tgg00_WaitContext    &WaitContext,
                                         Log_IOSequenceNo      IoSeq,
                                         bool                 &waitInserted,
                                         SAPDBErr_MessageList &errlist) = 0;
	/// A redo task waits until preceding transactions are closed.
    /// If true is returned the redo is aborted.
    virtual bool WaitForIoSeq (tsp00_TaskId  RedoTaskId) = 0;
    /// This is used to wait until the whole list is empty
    virtual void WaitForAllRedoTasks ( tsp00_TaskId RedoTaskId ) = 0;
	/// The log reader task sets the state: end of log read
    virtual void SetEndOfLogRead (tsp00_TaskId LogReaderTaskId) = 0;
    /// The log reader calls this after recreation of all open transactions from last savepoint
    virtual void EnableRedoProcessing (tsp00_TaskId LogReaderTaskId) = 0;
	/// writes some basic information into the trace 
	virtual void WriteToTrace (const char *title = 0) const = 0;
    /// write some additional information into the given errlist
    virtual void AppendContextInfo (SAPDB_Char  const * const  filename,
                                    SAPDB_UInt4 const          linenumber,
                                    SAPDBErr_MessageList      &errlist,
                                    SAPDB_Char  const * const  title = 0) const = 0;
	/// This returns the youngest iosequence inserted to the redo list
	virtual Log_IOSequenceNo GetLastIOSequence (tsp00_TaskId taskid) const = 0;
    /// Abort all redo tasks if waiting for something then wake them up
    virtual bool Abort (tsp00_TaskId taskid) = 0;
    /// returns true if redo is aborted
    virtual bool IsAborted () const = 0;
    /// maximum number of transactions which are configure
    virtual SAPDB_UInt GetMaximumNumberOfTransactions() const = 0; // PTS 1134103 UH 2005-03-07 new
    /// a list of transno's is returned, which is currently known by redo traffic control
	virtual bool GetTransactions (tsp00_TaskId            taskid,
                                  Rst_Transactions       &listOfTransToSkip,
                                  Msg_List               &errlist) const = 0; // PTS 1134103 UH 2005-03-07 new
};
#endif  /* RST_IREDO_TRAFFIC_CONTROL_H */