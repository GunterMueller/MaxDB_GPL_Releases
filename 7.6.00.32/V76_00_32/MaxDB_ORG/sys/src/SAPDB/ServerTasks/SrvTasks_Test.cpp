/*!
  @file        SrvTasks_Test.cpp
  @author      UweH
  @ingroup     ServerTasks
  @brief       This is for testing the server tasks.
  @since       2005-02-02

\if EMIT_LICENCE
  copyright:    Copyright (c) 2001-2005 SAP AG


    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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
#include "ServerTasks/SrvTasks_Test.hpp"
#include "Transaction/Trans_Context.hpp"
#include "ServerTasks/SrvTasks_ICoordinator.hpp"
#include "ServerTasks/SrvTasks_Job.hpp"
#include "ServerTasks/SrvTasks_JobList.hpp"
#include "ServerTasks/SrvTasks_WaitingTask.hpp"
#include "RunTime/Synchronisation/RTESync_Spinlock.hpp"
#include "SAPDBCommon/Messages/SDBMsg_SrvTasks.h"
#include "Logging/Log_Savepoint.hpp"
//--------------------------------------------------------------------------------------
/// test job
class SrvTasks_JobTest : public SrvTasks_Job
{
public:
	/// standard constructor
    SrvTasks_JobTest(SAPDB_Int4            id,
                     SrvTasks_WaitingTask &waitingTask)
    : SrvTasks_Job("SERVERTASKS TEST JOB............", "TESTJOB", Attributes(id), &waitingTask)
    {}
    /// This is doing nothing because we are not copying
    virtual void Deallocate ()
    {}
protected:
    /// This executes the job and must be overlayed
    virtual Result ExecuteIntern(Trans_Context &trans)
    {
        const Log_Savepoint::StartResult result =
            Log_Savepoint::Instance().StartSavepointAndWait ( trans.Task(),
                                                              Log_SVPReasonUserRequested,
                                                              trans.ErrorList());
        if ( result != Log_Savepoint::ok )
        {
            trans.ErrorList().AddDetail(SDBMSG_SRVTASKS_SELF_TEST_ERROR,
                                        Msg_Arg("ERRORTEXT","StartSavepointAndWait failed"),
                                        Msg_Arg("NO",       SAPDB_ToString(m_Attributes.Id)));
            return error;
        }
            
        RTE_Message(Msg_List(Msg_List::Info, SDBMSG_SRVTASKS_SELF_TEST_ERROR,
                            Msg_Arg("ERRORTEXT","Test Job executed"),
                            Msg_Arg("NO",       SAPDB_ToString(m_Attributes.Id))));
        // trans.Task().Sleep(1);
        return ok;
    }
    /// This is not allowed because we always want to wait.
    virtual bool CreateCopy ( SAPDBMem_IRawAllocator &allocator, 
                              Msg_List               &errlist,
                              SrvTasks_Job*          &jobCopy ) const
	{
        return false;
	}
};
/// test coordinator class
class SrvTasks_TestCoordinator
{
private:
    /// job list type
    typedef SrvTasks_JobList<SrvTasks_JobTest> JobListTest;

public:
    /// standard constructor
    SrvTasks_TestCoordinator(Trans_Context &trans)
    : m_Result          (true)
    , m_StartedServer   (0)
    , m_AvailableServer (0)
    , m_Trans           (trans)
    , m_WaitingTask     (trans.Task())
    , m_Jobs            (trans.Allocator())
    {}
    /// run the test
    bool Run (SAPDB_UInt testCount)
    {
        SrvTasks_ICoordinator &coord = SrvTasks_ICoordinator::Instance();
    
        coord.WriteToTrace("Servertasks before test");
        
        m_AvailableServer = coord.GetAvailableServerTasks();
        
        bool result = StartAndRestartJobs (testCount);
    
        if ( result )
        {
            JobListTest::Iterator    jobiter;
            
            while ( m_Jobs.WaitForAnyJobFinished(m_WaitingTask, jobiter) )
            {
                ServerStopped();
                result =  (*jobiter)->GetResult(m_Trans.ErrorList()) == SrvTasks_JobTest::ok;
                destroy(*jobiter, m_Trans.Allocator());
                m_Jobs.RemoveJob(jobiter);
            }
        }
        else
        {
            m_Jobs.AbortAll(m_WaitingTask);
        }

        if ( result && m_StartedServer > 0 )
        {
            m_Trans.ErrorList().AppendNewMessage(Msg_List(Msg_List::Error, SDBMSG_SRVTASKS_SELF_TEST_ERROR,
                                                          Msg_Arg("ERRORTEXT","There are servertasks started which were not received."),
                                                          Msg_Arg("NO",       SAPDB_ToString(m_StartedServer))));
            result = false;
        }

        if ( result && ! m_Jobs.IsEmpty() )
        {
            m_Trans.ErrorList().AppendNewMessage(Msg_List(Msg_List::Error, SDBMSG_SRVTASKS_SELF_TEST_ERROR,
                                                          Msg_Arg("ERRORTEXT","The JobList should be empty at the end, but is not."),
                                                          Msg_Arg("NO",       SAPDB_ToString(m_Jobs.GetSize()))));
            result = false;
        }
    
        coord.WriteToTrace("Servertasks after test");
    
        return result;
    }

private:

    bool StartAndRestartJobs(SAPDB_UInt testCount)
    {
        bool                                result = true;
        bool                                jobIsExecuted;
        JobListTest::Iterator               jobiter;
        SrvTasks_Job::ExecuteByServerResult executeResult;
         
        for ( SAPDB_UInt jobno = 0; jobno < testCount; ++jobno )
        {
            SrvTasks_JobTest* pNewJob = new(m_Trans.Allocator()) SrvTasks_JobTest(jobno, m_WaitingTask);
            if ( pNewJob == 0 )
            {
                m_Trans.ErrorList().AppendNewMessage(Msg_List(Msg_List::Error,SDBMSG_SRVTASKS_ALLOCATION_FAILED));
                result = false;
                break;
            }
            
            jobIsExecuted = false;

            while ( ! jobIsExecuted && result )
            {
                executeResult = pNewJob->ExecuteByServer(m_Trans.Task(), m_Trans.ErrorList(), &m_WaitingTask);
                switch ( executeResult )
                {
                    case SrvTasks_Job::executeOK:
                        if ( m_Jobs.InsertEnd(pNewJob) )
                        {
                            ServerStarted();
                            jobIsExecuted = true;
                            break;
                        }
                        destroy(pNewJob,m_Trans.Allocator());
                        m_Trans.ErrorList() = Msg_List(Msg_List::Error,SDBMSG_SRVTASKS_ALLOCATION_FAILED);
                        pNewJob->AppendContextInfo(__CONTEXT__,m_Trans.ErrorList(),"m_Jobs.InsertEnd(pNewJob) failed");
                        result = false;
                        break;
                    case SrvTasks_Job::executeNoServerAvailable:
                        m_Trans.ErrorList().ClearMessageList();
                        if ( ! m_Jobs.WaitForAnyJobFinished(m_WaitingTask, jobiter) )
                        {
                            result = false;
                        }
                        else
                        {
                            ServerStopped();
                            result =  (*jobiter)->GetResult(m_Trans.ErrorList()) == SrvTasks_Job::ok;
                            destroy(*jobiter, m_Trans.Allocator());
                            m_Jobs.RemoveJob(jobiter);
                        }
                        continue;
                    case SrvTasks_Job::executeError:
                        // go through
                    default:
                        m_Trans.ErrorList().
                        AppendNewMessage(Msg_List(Msg_List::Error, SDBMSG_SRVTASKS_SELF_TEST_ERROR,
                                                  Msg_Arg("ERRORTEXT","ExecuteByServer returns error"),
                                                  Msg_Arg("NO",       SAPDB_ToString(executeResult))));
                        destroy(pNewJob,m_Trans.Allocator());
                        result = false;
                        break;
                }
            }
        }
        return result;
    }
    
    void ServerStarted()
    {
        m_Spinlock.Lock();
        ++m_StartedServer;
        m_Spinlock.Unlock(); 
    }

    void ServerStopped()
    {
        m_Spinlock.Lock();
        --m_StartedServer;
        m_Spinlock.Unlock(); 
    }
    
private:

    SAPDB_UInt            m_StartedServer;
    SAPDB_UInt            m_AvailableServer;
    bool                  m_Result;
    Trans_Context        &m_Trans;
    SrvTasks_WaitingTask  m_WaitingTask;
    JobListTest           m_Jobs;
    RTESync_Spinlock      m_Spinlock;
};
//--------------------------------------------------------------------------------------
bool SrvTasks_Test (Trans_Context &trans)
{
    SrvTasks_TestCoordinator test (trans);
    return test.Run(1000);
}
