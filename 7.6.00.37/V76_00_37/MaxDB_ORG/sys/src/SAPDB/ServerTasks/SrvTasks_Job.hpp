/*!
  @file           SrvTasks_Job.hpp
  @author         UweH
  @ingroup        ServerTasks
  @brief          This defines SrvTasks_Job

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
#ifndef SrvTasks_Job_HPP
#define SrvTasks_Job_HPP
//--------------------------------------------------------------------------------------
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/SAPDB_string.h"
#include "Container/Container_Vector.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "ServerTasks/SrvTasks_JobTypes.hpp"
#include "Transaction/Trans_Context.hpp"
//--------------------------------------------------------------------------------------

class SAPDBMem_IRawAllocator;
class SrvTasks_ITaskForJob;
class SrvTasks_WaitingTask;
class Kernel_VTrace;
class SAPDB_OStream;
//--------------------------------------------------------------------------------------
/// This implements one job to be executed by a servertask.
class SrvTasks_Job
{
public: // interface for users
    /// A user settable id number.
    typedef SAPDB_UInt2 Id;
    /// A Result value
    typedef SAPDB_UInt4 Result;
    /// standard result values
    enum StandardResults
    {
        ok = 0, error = 1
    };
    /// attributes forced by a job to be inherited by the executing server task
    class Attributes
    {
    public:
        /// Default values are assigned
        Attributes()
        :Id                    (0),
         IgnoreDBFullSecureLimit(false),
         KeepServerTaskReserved(false)
        {}
        /// Default values are assigned
        Attributes(SAPDB_Int4 newId)
        :Id                    (newId),
         IgnoreDBFullSecureLimit(false),
         KeepServerTaskReserved(false)
        {}
        /// This is an id which is used e.g. in the vtrace and other organizational manners
        SAPDB_Int4 Id;
        /// If this is true the server task uses a reserved pageNo pool, if the data base is nearly full
        bool       IgnoreDBFullSecureLimit;
        /// If this is set to true the server task will not be available for others when the job
        /// is finished.
        bool       KeepServerTaskReserved;
    };
    /*!
    @brief This creates and defines a new job which can be executed by a server task.
    @param description [in]  a string for any kind of nice output
    @param shortdescription [in]  a string for x_cons
    @param attributes  [in]  These attributes are inherited to the server task's session context
    @param waitingTask [in] optional waiting task context
    */
    SrvTasks_Job(const RTE_TaskDisplayName      description,
                 const RTE_TaskDisplayNameShort shortdescription,
                 const Attributes               attributes,
                 SrvTasks_WaitingTask         * waitingTask = 0 );
    /*!
    @brief This creates and defines a new job which can be executed by a server task.
    @param jobtype     [in]  this type identifies the servertask if it is of a known type
    @param attributes  [in]  These attributes are inherited to the server task's session context
    @param waitingTask [in]  optional waiting task context
    */
    SrvTasks_Job(SrvTasks_JobTypes              jobtype,
                 const Attributes               attributes,
                 SrvTasks_WaitingTask         * waitingTask = 0 );
	/// virtual destructor for use by inherited classes
    virtual ~SrvTasks_Job() {}
	/// result of ExecuteByServer
    enum ExecuteByServerResult
    {
        executeOK,
        executeError,
        executeNoServerAvailable
    };
    /*!
    @brief The given job is immediately executed by a servertask.
    @param  task        [in] This is used for synchronization
    @param  errlist     [out] hint in case of an error
    @param  waitingTask [in] if this pointer is assigned it overwrites a previously given value
                             The caller must wait externally !!!
    @return ExecuteByServerResult
    */
    ExecuteByServerResult ExecuteByServer ( RTETask_ITask        &task,
                                            Msg_List             &errlist,
                                            SrvTasks_WaitingTask *waitingTask = 0 );
	/*!
    @brief   This is used to execute a job directly. For example if no server task is available.
    @param   trans The current context.
    
    Results and ErrorList must be read via GetResult().
    */
    void ExecuteDirectly (Trans_Context &trans)
    {
        m_Result  = ExecuteIntern(trans);
        m_ErrList = trans.ErrorList();
    }
    /// The caller waits until the job is finished. The jobs result is returned.
    bool WaitForResult ( SrvTasks_WaitingTask &waitingTask,
                         Result               &jobResult,
                         Msg_List             &jobErrList);
    /// sets the waiting task context
    void SetWaitingTask(SrvTasks_WaitingTask &waitingTask)
    {
        m_WaitingTask = &waitingTask;
    }
    /// This is used to abort a running job.
    /// The CancelByte is set if the job is just be executed by a server task.
    /// The Job is not finished but suspended by the server tasking it is resumed.
    void Abort(RTETask_ITask &task);
    /// returns the job's id
    Id GetId()
    {
        return m_Attributes.Id;
    }
    /// Returns the attributes
    const Attributes& GetAttributes() const
    {
        return m_Attributes;
    }
    /// Sets Attributes
    void SetAttributes(const Attributes& attributes)
    {
        m_Attributes = attributes;
    }
    /// returns the current result code of the job
    /// The errlist is assigned the jobs errlist.
    /// The jobs state is set to confirmed.
    Result GetResult(Msg_List &errlist)
    {
        m_State = confirmed;
        if ( ! m_ErrList.IsEmpty() )
            errlist.AppendNewMessage(m_ErrList);
        return m_Result;
    }
    /// this defines possible job states
    enum State
    {
        prepared,  // initial state after creating the job.
        started,   // The job is send to the server task to be executed.
        executing, // The job is during execution by the server task.
        finished,  // The job is finished with executing.
        aborted,   // The job is finished but the abort flag is true.
        confirmed, // The result was read by anyone with GetResult()
        invalidState_DONT_USE
    };
    /// this is used to have a nice print out
    static const char * StateString[invalidState_DONT_USE];
    /// returns the execution jobs state
    State GetState() const
    {
        return m_State;
    }
    /// the jobs description is given
    void GetDescription( RTE_TaskDisplayName      description,
                         RTE_TaskDisplayNameShort shortDescription )
    {
        SAPDB_strncpy_zero_terminated ( description, m_Description, sizeof(RTE_TaskDisplayName) );
        SAPDB_strncpy_zero_terminated ( shortDescription, m_ShortDescription, sizeof(RTE_TaskDisplayNameShort) );
    }
    /// Appends some context information
    virtual void AppendContextInfo (SAPDB_Char  const * const  filename,
                                    SAPDB_UInt4 const          linenumber,
                                    Msg_List                  &errlist,
                                    SAPDB_Char  const * const  title = 0) const;
    /// This is called to get access to the local transaction context
    /// This may only be called if the job has a valid context set
    Trans_Context& GetTransContext();
protected: // interface for coordinator or task
    /// This executes the job. This must always be called by a servertask.
    void Execute ();
    /// This is called by the servertask before the job is executed
    virtual void BeginExecute (SrvTasks_ITaskForJob &context);
    /*!
    @brief  This methode must be implemented by the job designer
    @param  This is the job tasks or session context.
    @return Result
    
    This can be called by a servertask or a usertask.
    */
    virtual Result ExecuteIntern (Trans_Context &trans) = 0;
    /// This is called by the servertask after the job is executed
    virtual void EndExecute ();
    /// This creates a copy of the job (including all pointers)
    /// This is used by servertasks if nobody wants to wait for the jobs end
    virtual bool CreateCopy ( SAPDBMem_IRawAllocator &allocator, 
                              Msg_List               &errlist,
                              SrvTasks_Job*          &jobCopy ) const = 0;
    /// This deallocates all references which were made by CreateCopy()
    virtual void Deallocate () = 0;
    /// This indicates that the job is successfully sent to a servertask.
    void SetStarted(RTETask_ITask &usertask);
    /// returns a witing task pointer, it may be null
    /// This is used by the server task.
    SrvTasks_WaitingTask* GetWaitingTask() const
    {
        return m_WaitingTask;
    }
protected:
    /// The result of a job is understood by the caller.
    /// In most cases only 0=OK or 1=ERROR is used.
    Result                   m_Result;
    /// These attributes are inherited to the server task context
    Attributes               m_Attributes;
	/// This is the state of the job.
	State                    m_State;
    /// if the job type is well known the type is different from unspecified
    SrvTasks_JobTypes        m_JobType;
    /// a description of the jobs purpose
    RTE_TaskDisplayName      m_Description;
	/// a short description of the jobs purpose (for x_cons)
    RTE_TaskDisplayNameShort m_ShortDescription;
    /// This pointer is assigned if a task is waiting for the jobs end
    SrvTasks_WaitingTask   * m_WaitingTask;
	/// If the job is executed by a server task the pointer of the task is stored here
    SrvTasks_ITaskForJob   *m_ServerTaskContext;
private:
    /// If during the job, any error occured, this list might contain additional information.
    Msg_List                 m_ErrList;
private:
    /// Do not use Copy Ctor instead use CreateCopy()
    SrvTasks_Job(const SrvTasks_Job& other);

friend SAPDB_OStream & operator<< (SAPDB_OStream &os, const SrvTasks_Job &job);
/// The task is allowed to call Execute()
friend class SrvTasks_Task;
};
/// This is used to write a job to the knltrace.
SAPDB_OStream & operator<< (SAPDB_OStream &os, const SrvTasks_Job &job);
#endif // SrvTasks_Job_HPP
