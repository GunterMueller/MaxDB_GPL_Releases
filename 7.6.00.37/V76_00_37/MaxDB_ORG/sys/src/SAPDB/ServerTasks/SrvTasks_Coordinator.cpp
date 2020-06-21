/*!
  @file        SrvTasks_Coordinator.cpp
  @author      UweH
  @ingroup     ServerTasks
  @brief       This class coordinates all server tasks.
  @since       2004-06-08

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
#include "ServerTasks/SrvTasks_Coordinator.hpp"
#include "ServerTasks/SrvTasks_Task.hpp"
#include "ServerTasks/SrvTasks_Types.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDBCommon/Messages/SDBMsg_SrvTasks.h"
#include "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp"
#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include "RunTime/RTE_Crash.hpp"
#include "hkb38.h"  // k38GetMaxServerTasks
#include "hkb900.h" // kb900AvailableServerTasks
#include "heo55k.h" // RTE   : vbegexcl,venexcl 
#include "hgg08.h"  // g08server
#include "hgg01.h"  // g01maxuser
//--------------------------------------------------------------------------------------
SrvTasks_Coordinator* SrvTasks_Coordinator::m_Instance = 0;
// -----------------------------------------------------------------------------
/// This is used to read some parameters like MAXSERVERTASKS and RESERVEDSERVERTASKS
inline bool GetParameterValues( SAPDB_UInt4 &maxServerTasks,
                                SAPDB_UInt4 &maxReservedTasks,
                                Msg_List    &errlist)
{
    const SAPDB_UTF8 *         paramName1 = UTF8("MAXSERVERTASKS");
    const SAPDB_UTF8 *         paramName2 = UTF8("RESERVEDSERVERTASKS");
    RTEConf_Parameter::Integer value      = 0;
    
    if ( RTEConf_ParameterAccess::Instance() == 0 )
        return false;

    if ( ! RTEConf_ParameterAccess::Instance()->GetInteger(paramName1, value, errlist)
         ||
         value > SAPDB_MAX_UINT4 )
        return false;
    else
        maxServerTasks = static_cast<SAPDB_UInt4>(value);
    
    if ( ! RTEConf_ParameterAccess::Instance()->GetInteger(paramName2, value, errlist)
         ||
         value > SAPDB_MAX_UINT4 )
        return false;
    else
        maxReservedTasks = static_cast<SAPDB_UInt4>(value);

    return true;
}
//--------------------------------------------------------------------------------------
void SrvTasks_Coordinator::CreateInstance (SAPDBMem_IRawAllocator &allocator)
{
    SAPDBTRACE_ROUTINE_DEBUG ("SrvTasks_Coordinator::CreateInstance", SrvTasks_Trace, 5);
    if ( m_Instance == 0 )
    {
        SAPDBMEM_STATIC_RAW_ALLOCATE (Space, sizeof(SrvTasks_Coordinator));

        m_Instance = new (Space) SrvTasks_Coordinator(allocator);

        if ( m_Instance == 0 )
            RTE_Crash(Msg_List(Msg_List::Error,SDBMSG_SRVTASKS_NO_MORE_MEMORY_FOR_COORDINATOR));

        SAPDB_UInt4 maxServerTasks = 0;
        Msg_List    errlist;

        if ( ! GetParameterValues( maxServerTasks,
                                   m_Instance->m_NumberOfReservedTasks,
                                   errlist ) )
            RTE_Crash(errlist);

        m_Instance->m_Tasks.Initialize(maxServerTasks);

        RTE_Message (Msg_List(Msg_List::Info, SDBMSG_SRVTASKS_AVAILABLE_SERVERTASKS,
                     Msg_Arg("UINT4",SAPDB_ToString(m_Instance->MaxConfiguredServerTasks()))));
    }
}
//--------------------------------------------------------------------------------------
SrvTasks_Coordinator& SrvTasks_Coordinator::Instance()
{
    SAPDBERR_ASSERT_STATE(m_Instance != 0);
    return *m_Instance;
}
//--------------------------------------------------------------------------------------
SrvTasks_ICoordinator& SrvTasks_ICoordinator::Instance()
{
    return SrvTasks_Coordinator::Instance();
}
//--------------------------------------------------------------------------------------
void SrvTasks_Coordinator::AppendContextInfo (SAPDB_Char  const * const  filename,
                                              SAPDB_UInt4 const          linenumber,
                                              SAPDBErr_MessageList      &errlist,
                                              SAPDB_Char  const * const  title) const
{
    errlist.AddDetail(SDBMSG_SRVTASKS_SERVERTASKS_COORDINATOR_INFO,
                      Msg_Arg("MAX_SERVER_TASKS",SAPDB_ToString(MaxConfiguredServerTasks())),
                      Msg_Arg("SERVER_TASKS_USED",SAPDB_ToString(m_UsedServerTasks)));
    
    for ( SAPDB_UInt controlno = 0; controlno < MaxConfiguredServerTasks(); ++controlno )
    {
        errlist.AddDetail(SDBMSG_SRVTASKS_SERVERTASK_CONTROL_INFO,
                          Msg_Arg("SERVER_TASK_INDEX",SAPDB_ToString(controlno)),
                          Msg_Arg("SRVTASK_ASSIGNED",(m_Tasks[controlno].IsAssigned() ? "assigned" : "")),
                          Msg_Arg("SRVTASKS_RESERVED",(m_Tasks[controlno].IsReserved() ? "reserved" : "")));
        m_Tasks[controlno].AppendContextInfo(filename,linenumber,errlist);
    }
}
//--------------------------------------------------------------------------------------
void SrvTasks_Coordinator::ServerTaskControl::AppendContextInfo (SAPDB_Char  const * const  filename,
                                              SAPDB_UInt4 const          linenumber,
                                              SAPDBErr_MessageList      &errlist,
                                              SAPDB_Char  const * const  title) const
{
    if ( IsAssigned() )
        m_pTask->AppendContextInfo(filename,linenumber,errlist);
}
//--------------------------------------------------------------------------------------
/// This is used to write a servertask control structure to the knltrace.
SAPDB_OStream & operator<< (SAPDB_OStream &os, const SrvTasks_Coordinator::ServerTaskControl &control)
{
    os << "TaskControl: " << (control.m_Reserved ? "    RESERVED " : "NOT RESERVED ");
    if ( control.m_pTask != 0 )
        os << *(control.m_pTask) << "'" << control.m_Description << "'";
	else
        os << "NOT ASSIGNED";
	return os;
}
//--------------------------------------------------------------------------------------
void SrvTasks_Coordinator::WriteToTrace (SAPDB_Char  const * const  title) const
{
    Kernel_VTrace trace;
    
    if ( title != 0 )
        trace << title << FlushLine;

    trace << "SERVERTASK COORDINATOR: "
          << "used=" << m_UsedServerTasks << " "
          << "max="  << MaxConfiguredServerTasks()  << " "
          << FlushLine;

    for ( SAPDB_UInt controlno = 0; controlno < MaxConfiguredServerTasks(); ++controlno )
        trace << "SERVERTASKCONTROL #" << controlno << " " << m_Tasks[controlno] << FlushLine;
}
//--------------------------------------------------------------------------------------
/// This is a pascal wrapper
externPascal tsp00_Int4 kb900AvailableServerTasks ()
{
    return SrvTasks_Coordinator::Instance().AvailableServerTasks();
}
//--------------------------------------------------------------------------------------
bool SrvTasks_Coordinator::Register(SrvTasks_Task &srvtask,
                                    Msg_List      &errlist)
{
    SAPDBTRACE_ROUTINE_DEBUG ("SrvTasks_Coordinator::Register", SrvTasks_Trace, 5);
    vbegexcl (srvtask.GetContext().Task().ID(),g08server);
    for ( SAPDB_UInt taskindex = 0; taskindex < MaxConfiguredServerTasks(); ++taskindex )
    {
        if ( ! m_Tasks[taskindex].IsAssigned() )
        {
            m_Tasks[taskindex] = ServerTaskControl(srvtask);
            srvtask.SetIndex(taskindex);
            vendexcl (srvtask.GetContext().Task().ID(),g08server);
            return true;
        }
    }
    vendexcl (srvtask.GetContext().Task().ID(),g08server);
    errlist = Msg_List(Msg_List::Error,SDBMSG_SRVTASKS_NO_MORE_MEMORY_TO_REGISTER_TASK);
    return false;
}
//--------------------------------------------------------------------------------------
void SrvTasks_Coordinator::WaitUntilAllServertasksAssigned(RTETask_ITask &task) const
{
    while ( ! m_Tasks[MaxConfiguredServerTasks()-1].IsAssigned() )
        task.Sleep(1);
}

//--------------------------------------------------------------------------------------
SrvTasks_Coordinator::ReserveResult
SrvTasks_Coordinator::Reserve ( RTETask_ITask                  &task,
                                SrvTasks_RequestedTask         &servertask,
                                Msg_List                       &errlist,
                                const RTE_TaskDisplayName       description,
                                const RTE_TaskDisplayNameShort  shortdescription,
                                bool                            forSystemUsage )
{
    SAPDBTRACE_ROUTINE_DEBUG ("SrvTasks_Coordinator::Reserve", SrvTasks_Trace, 5);

    WaitUntilAllServertasksAssigned(task);
    
    if ( forSystemUsage )
        if ( MaxConfiguredServerTasks() == 0 )
            return reserveNoServerAvailable;
    else
        if ( MaxServerTasks() == 0 )
            return reserveNoServerAvailable;

    bool       isOK              = true;
    bool       useReleaseOnError = false;
    bool       endReached        = false;
    SAPDB_UInt taskindex         = forSystemUsage ? MaxConfiguredServerTasks()-1 : 0;

    vbegexcl (task.ID(),g08server);
    
    while ( ! endReached )
    {
        if ( ! m_Tasks[taskindex].IsReserved() )
        {
            if ( SrvTasks_Trace.TracesLevel(7) )
                Kernel_VTrace() << "RESERVE " << m_Tasks[taskindex];
            servertask = SrvTasks_RequestedTask (m_Tasks[taskindex].Reserve(description,shortdescription), taskindex);
            useReleaseOnError = true;
            if ( forSystemUsage )
                ++m_NumberOfUsedReservedTasks;
            else
                ++m_UsedServerTasks;
            break;
        }
        taskindex  = forSystemUsage ? taskindex-1 : taskindex+1;
        endReached = forSystemUsage ? taskindex == 0 : taskindex >= MaxServerTasks();
    }

    #ifdef SAPDB_SLOW
    if ( endReached )
        WriteToTrace("SrvTasks_Coordinator::Reserve => reserveNoServerAvailable");
    #endif

    vendexcl (task.ID(),g08server);

    return endReached ? reserveNoServerAvailable : reserveOK;
}
//--------------------------------------------------------------------------------------
bool SrvTasks_Coordinator::Release ( RTETask_ITask  &task,
                                     SAPDB_UInt      taskindex )
{
    SAPDBTRACE_ROUTINE_DEBUG ("SrvTasks_Coordinator::Release", SrvTasks_Trace, 5);
    bool isOK = true;
    vbegexcl (task.ID(),g08server);
    if ( m_Tasks[taskindex].IsReserved() )
    {
        if ( SrvTasks_Trace.TracesLevel(7) )
            Kernel_VTrace() << "RELEASE: " << m_Tasks[taskindex];
        m_Tasks[taskindex].Release();
        --m_UsedServerTasks;
    }
    else
        isOK = false;
    vendexcl (task.ID(),g08server);
    return isOK;
}
//--------------------------------------------------------------------------------------
void SrvTasks_Coordinator::SetTaskStarted ( RTETask_ITask           &task,
                                            SAPDB_UInt               taskindex,
                                            RTE_TaskDisplayName      description,
                                            RTE_TaskDisplayNameShort shortdescription )
{
    SAPDBTRACE_ROUTINE_DEBUG ("SrvTasks_Coordinator::SetTaskStarted", SrvTasks_Trace, 5);
    vbegexcl (task.ID(),g08server);
    if ( m_Tasks[taskindex].IsReserved() )
        task.SetDisplayName  (shortdescription, description);
    vendexcl (task.ID(),g08server);
}
//--------------------------------------------------------------------------------------
void SrvTasks_Coordinator::SetTaskStopped ( RTETask_ITask  &task,
                                            SAPDB_UInt      taskindex )
{
    SAPDBTRACE_ROUTINE_DEBUG ("SrvTasks_Coordinator::SetTaskStopped", SrvTasks_Trace, 5);
    vbegexcl (task.ID(),g08server);
    task.SetDisplayName(0,0);
    vendexcl (task.ID(),g08server);
}
//--------------------------------------------------------------------------------------
bool SrvTasks_Coordinator::GetInfo (RTETask_ITask   &task,
                                    SAPDB_UInt       controlNo,
                                    SAPDB_UInt      &servertaskid,
                                    bool            &isAssigned,
                                    bool            &isReserved,
                                    RTE_TaskDisplayName  description) const
{
    if ( controlNo >= MaxConfiguredServerTasks() )
        return false;

    vbegexcl (task.ID(),g08server);
    servertaskid = m_Tasks[controlNo].TaskId();
    isAssigned   = m_Tasks[controlNo].IsAssigned();
    isReserved   = m_Tasks[controlNo].IsReserved();
    RTE_TaskDisplayNameShort dummy;
    m_Tasks[controlNo].GetDescription(description,dummy);
    vendexcl (task.ID(),g08server);
    return true;
}
