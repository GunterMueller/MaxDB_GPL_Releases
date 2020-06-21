/*!
  @file           SrvTasks_JobList.hpp
  @author         UweH
  @ingroup        ServerTasks
  @brief          This defines SrvTasks_JobList

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
#ifndef SrvTasks_JobList_HPP
#define SrvTasks_JobList_HPP
//--------------------------------------------------------------------------------------
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/Messages/SDBMsg_SrvTasks.h"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "RunTime/RTE_Message.hpp"
#include "Container/Container_Vector.hpp"
#include "ServerTasks/SrvTasks_Types.hpp"
#include "ServerTasks/SrvTasks_WaitingTask.hpp"
#include "ServerTasks/SrvTasks_Job.hpp"
#include "heo55k.h" // RTE   : vbegexcl,venexcl 
#include "hgg08.h"  // g08servertask
//--------------------------------------------------------------------------------------
class RTETask_ITask;
class SAPDBMem_IRawAllocator;
//--------------------------------------------------------------------------------------
/// A list of jobs to be executed.
template <class JOB>
class SrvTasks_JobList : public Container_Vector<JOB*>
{
public:
    /// ctor
    SrvTasks_JobList( SAPDBMem_IRawAllocator &allocator )
    : Container_Vector<JOB*>(allocator)
    {}
    /*!
    @brief  All jobs in the list are executed by servertasks
    @param  waitingTask   [in] This is your own task context extended to wait for jobs
    @param  anyJobAborted [out] this becomes true if any job was flagged as aborted
    @param  errlist       [out] hint in case of an error
    @return false on any error
    
    This is a convenience function, if you need to start a lot of jobs and want to wait
    for them all.
    */
    bool ExecuteAll ( SrvTasks_WaitingTask   &waitingTask,
                      bool                   &anyJobAborted,
                      Msg_List               &errlist );
    /// The Iterator points to a job which is finished if true is returned
    bool SearchForFinishedJob( SrvTasks_WaitingTask                      &waitingTask,
                               typename Container_Vector<JOB*>::Iterator &jobIter,
                               SAPDB_UInt                                &unfinishedJobs,
                               bool                                       notAllJobsMayBeStarted = false);
    /// returns true if all jobs are confirmed.
    bool AllConfirmed();
    /*!
    @param  waitingTask  [in]  the task context which is waiting
    @param  finishedJob  [out] The pointer to a finished job
    @param  notAllJobsMayBeStarted [in] This is true for backup jobs.
    @return true if a jobs result is returned
    @brief  This returns the result of any job which is finished
    */
    bool WaitForAnyJobFinished ( SrvTasks_WaitingTask &waitingTask,
                                 typename Container_Vector<JOB*>::Iterator             &finishedJob,
                                 bool                  notAllJobsMayBeStarted = false );
    /*!
    @param  waitingTask  [in]  the task context which is waiting
    @param  removejob
    @param  isAborted [out] true if any job was aborted
    @param  result    [out] last error result
    @param  errlist   [out] error messages
    @return the last result which is not OK
    @brief  This collects the results of all jobs.
    */
    void WaitForAll ( SrvTasks_WaitingTask &waitingTask,
                      bool                 &isAborted,
                      typename JOB::Result &result,
                      Msg_List             &errlist );
    /*!
    @brief This deallocates all jobs referenced by this list.
    This assumes that all inserted jobs objects are allocated by the allocator within this container.
    */
    void RemoveAllJobs();
    /*!
    @param  waitingTask  [in]  the task context which is waiting
    @brief  This sets the abort flag and waits for all jobs to be finished.
    */
    void AbortAll ( SrvTasks_WaitingTask &waitingTask );
    /// Writes the list to the knltrace
    void WriteToTrace (SAPDB_Char  const * const  title = 0) const;
    /// last printout of the list if trace level 7 is set
    ~SrvTasks_JobList()
    {
        if ( SrvTasks_Trace.TracesLevel(7) )
            WriteToTrace ("~SrvTasks_JobList");
        this->RemoveAllJobs();
    }
    /*!
    @brief removes a job from the list
    @param job [in/out] the reference to the job, normaly the job is finished
    The job object is deallocated and its destructor is called.
    */
    void RemoveJob(typename Container_Vector<JOB*>::Iterator &job);
};

//--------------------------------------------------------------------------------------
template <class JOB> inline
bool SrvTasks_JobList<JOB>::ExecuteAll ( SrvTasks_WaitingTask   &waitingTask,
                                         bool                   &anyJobAborted,
                                         Msg_List               &errlist )
{
    SAPDBTRACE_METHOD_DEBUG ("SrvTasks_JobList::ExecuteAll", SrvTasks_Trace, 5);

    bool       isOK;
    typename Container_Vector<JOB*>::Iterator   iter;

    for ( iter = this->Begin(); iter != this->End(); ++iter )
    {
        if ( *iter == 0 )
        {
            isOK    = false;
            errlist = Msg_List(Msg_List::Error,SDBMSG_SRVTASKS_INTERNAL_ERROR,
                               Msg_Arg("DESCRIPTION","One of the jobs in the joblist is 0."));
            break;
        }
        
        switch ( (*iter)->ExecuteByServer(waitingTask.Task(),errlist,&waitingTask) )
        {
            case SrvTasks_Job::executeOK:
                isOK = true;
                break;
            case SrvTasks_Job::executeNoServerAvailable:
            case SrvTasks_Job::executeError:
                WriteToTrace("ExecuteByServer failed");
                isOK = false;
                break;
            default:
                isOK = false;
                break;
        }

        if ( ! isOK )
            break;
    }

    if ( ! isOK )
    {
        AbortAll(waitingTask); 
        anyJobAborted = true;
    }
    else
    {
        SrvTasks_Job::Result result = SrvTasks_Job::ok;
        WaitForAll(waitingTask, anyJobAborted, result, errlist);
        isOK = result == SrvTasks_Job::ok;
    }
    return isOK;
}
//--------------------------------------------------------------------------------------
template <class JOB> inline
bool SrvTasks_JobList<JOB>::AllConfirmed()
{
    typename Container_Vector<JOB*>::Iterator iter;
    for ( iter = this->Begin(); iter != this->End(); ++iter )
        if ( *iter == 0 || (*iter)->GetState() != SrvTasks_Job::confirmed )
            return false;
	return true;
}
//--------------------------------------------------------------------------------------
template <class JOB> bool inline
SrvTasks_JobList<JOB>::SearchForFinishedJob(SrvTasks_WaitingTask                      &waitingTask,
                                            typename Container_Vector<JOB*>::Iterator &jobIter,
                                            SAPDB_UInt                                &unfinishedJobs,
                                            bool                                       notAllJobsMayBeStarted)
{
    const bool writeTrace  = SrvTasks_Trace.TracesLevel(7);
    
    unfinishedJobs = 0;
    
    for ( jobIter = this->Begin(); jobIter != this->End(); ++jobIter )
    {
        if ( 0 == *jobIter )
        {
            if (writeTrace)
                Kernel_VTrace() << "WaitForAnyJobFinished: 0 pointer ignored";
        }
        else
        {
            typename JOB::State jobstate = (*jobIter)->GetState();

            switch ( jobstate )
            {
                case SrvTasks_Job::prepared:
                    if ( notAllJobsMayBeStarted )
                    {
                        if (writeTrace)
                            Kernel_VTrace() << "WaitForAnyJobFinished: ignored: " << **jobIter;
                        break;
                    }
                    // else NO break;
                case SrvTasks_Job::started:
                case SrvTasks_Job::executing:
                {
                    ++unfinishedJobs;
                    break;
                }
                case SrvTasks_Job::finished:
                case SrvTasks_Job::aborted:
                {
                    if (writeTrace)
                        Kernel_VTrace() << "WaitForAnyJobFinished: FOUND: " << **jobIter;
                    return true;
                }
                case SrvTasks_Job::confirmed:
                    if (writeTrace)
                        Kernel_VTrace() << "WaitForAnyJobFinished: ignored: " << **jobIter;
                    break;
                default:
                {
                    Kernel_VTrace trace;
                    trace << "UNEXPECTED JOB STATE: " << jobstate << FlushLine;
                    this->WriteToTrace ("unexpected job state - JobList:");
                    RTE_Crash(Msg_List(Msg_List::Error,SDBMSG_SRVTASKS_JOB_HAS_UNEXPECTED_STATE,
                              Msg_Arg("JOB_STATE",SAPDB_ToString(jobstate))));
                }
            }
        }
    }
	return false;
}
//--------------------------------------------------------------------------------------
template <class JOB> inline
bool SrvTasks_JobList<JOB>::WaitForAnyJobFinished ( SrvTasks_WaitingTask &waitingTask,
                                                    typename Container_Vector<JOB*>::Iterator             &finishedJob,
                                                    bool                  notAllJobsMayBeStarted )
{
    SAPDBTRACE_METHOD_DEBUG ("SrvTasks_JobList::WaitForAnyJobFinished", SrvTasks_Trace, 5);

    const bool writeTrace = SrvTasks_Trace.TracesLevel(7);

    if ( this->IsEmpty() )
    {
        if (writeTrace)
            Kernel_VTrace() << "WaitForAnyJobFinished: empty";
        return false;
    }

	vbegexcl(waitingTask.Task().ID(),g08servertask);

    SAPDB_UInt unfinishedJobs = 0;

    do
    {
        if ( SearchForFinishedJob(waitingTask, finishedJob, unfinishedJobs, notAllJobsMayBeStarted) )
        {
            vendexcl(waitingTask.Task().ID(),g08servertask);
            return true;
        }
        else
        {
            if ( unfinishedJobs > 0 )
            {
                waitingTask.SetWaitFlag();
                vendexcl(waitingTask.Task().ID(),g08servertask);
                waitingTask.Wait();
                vbegexcl(waitingTask.Task().ID(),g08servertask);
            }
        }
    }
    while ( unfinishedJobs > 0 );

    vendexcl(waitingTask.Task().ID(),g08servertask);
    
    if ( ! AllConfirmed() )
    {
        WriteToTrace("Nothing found but NOT all confirmed");
        RTE_Crash(Msg_List(Msg_List::Error,SDBMSG_SRVTASKS_WAIT_BUT_ALL_JOBS_ARE_CONFIRMED));
    }
    return false; // no more jobs to wait for
}
//--------------------------------------------------------------------------------------
template <class JOB> inline
void SrvTasks_JobList<JOB>::WaitForAll ( SrvTasks_WaitingTask &waitingTask,
                                         bool                 &isAborted,
                                         typename JOB::Result &result,
                                         Msg_List             &errlist )
{
    SAPDBTRACE_METHOD_DEBUG ("SrvTasks_JobList::WaitForAll", SrvTasks_Trace, 5);
    result = JOB::ok;

    typename Container_Vector<JOB*>::Iterator    finishedJob;
    Msg_List    auxErrList;
    typename JOB::Result auxResult;
    isAborted = false;

    while ( WaitForAnyJobFinished (waitingTask,finishedJob) )
    {
        isAborted = isAborted || (*finishedJob)->GetState() == JOB::aborted;
        auxResult = (*finishedJob)->GetResult(auxErrList);
        if ( auxResult != JOB::ok)
        {
            errlist.AppendNewMessage(auxErrList);
            if ( JOB::ok == result )
                result = auxResult;
        }
    }
}
//--------------------------------------------------------------------------------------
template <class JOB> inline
void SrvTasks_JobList<JOB>::AbortAll ( SrvTasks_WaitingTask &waitingTask )
{
    SAPDBTRACE_METHOD_DEBUG ("SrvTasks_JobList::AbortAll", SrvTasks_Trace, 5);
    typename Container_Vector<JOB*>::Iterator   iter;
    for ( iter = this->Begin(); iter != this->End(); ++iter )
        if ( *iter != 0 )
            (*iter)->Abort(waitingTask.Task());

    bool                 isAborted;
    typename JOB::Result result;
    Msg_List             errlist;

    WaitForAll (waitingTask,isAborted,result,errlist);
    if ( result != JOB::ok )
        RTE_Message(errlist);
}
//--------------------------------------------------------------------------------------
template <class JOB> inline
void SrvTasks_JobList<JOB>::RemoveAllJobs ()
{
    SAPDBTRACE_METHOD_DEBUG ("SrvTasks_JobList::RemoveAllJobs", SrvTasks_Trace, 5);
    typename Container_Vector<JOB*>::Iterator iter;
    for ( iter = this->Begin(); iter != this->End(); ++iter )
        if ( *iter != 0 )
            destroy(*iter, this->GetAllocator());
	this->Delete();
}
//--------------------------------------------------------------------------------------
template <class JOB> inline
void SrvTasks_JobList<JOB>::WriteToTrace (SAPDB_Char const * const  title) const
{
    Kernel_VTrace trace;
    if ( title != 0 )
        trace << title;
	else
        trace << "JobList:";
	trace << ", elements=" << this->GetSize() << FlushLine;
    int           jobCount;
    typename Container_Vector<JOB*>::ConstIterator iter;
    for ( iter = this->Begin(), jobCount = 0; iter != this->End(); ++iter, ++jobCount )
    {
        trace << "Job #" << jobCount << " ";
        if (*iter != 0)
            trace << **iter;
        else
            trace << "JobPointer is NULL";
        trace << FlushLine;
    }
}
//--------------------------------------------------------------------------------------
template <class JOB> inline
void SrvTasks_JobList<JOB>::RemoveJob(typename Container_Vector<JOB*>::Iterator &job)
{
    destroy(*job, this->GetAllocator());
    Delete(job);
}

#endif // SrvTasks_JobList_HPP
