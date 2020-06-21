/*!
  @defgroup  ServerTasks The component ServerTasks implements the internal kernel tasks.
*/
/*!
  @file           SrvTasks_Coordinator.hpp
  @author         UweH
  @ingroup        ServerTasks
  @brief          This class coordinates all server tasks.

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
#ifndef SrvTasks_Coordinator_HPP
#define SrvTasks_Coordinator_HPP
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/SAPDB_Singleton.hpp"
#include "SAPDBCommon/SAPDB_string.h"
#include "Container/Container_Vector.hpp"
#include "ServerTasks/SrvTasks_ICoordinator.hpp"
#include "ServerTasks/SrvTasks_Task.hpp"
#include "Transaction/Trans_Context.hpp"

class SAPDBMem_IRawAllocator;
class Msg_Message;
class Kernel_VTrace;
class SAPDB_OStream;

/// The coordinator of all server tasks
class SrvTasks_Coordinator : public SAPDB_Singleton, public SrvTasks_ICoordinator
{
public: /// user interface
    virtual ReserveResult Reserve ( RTETask_ITask                  &task,
                                    SrvTasks_RequestedTask         &servertask,
                                    Msg_List                       &errlist,
                                    const RTE_TaskDisplayName       description,
                                    const RTE_TaskDisplayNameShort  shortdescription,
                                    bool                            forSystemUsage = false );
    virtual SAPDB_UInt4 GetMaxServerTasks() const
    {
        return MaxServerTasks();
    }
    virtual SAPDB_UInt4 GetAvailableServerTasks() const
    {
        return AvailableServerTasks();
    }
    virtual bool GetInfo (RTETask_ITask   &task,
                          SAPDB_UInt       controlNo,
                          SAPDB_UInt      &servertaskid,
                          bool            &isAssigned,
                          bool            &isReserved,
                          RTE_TaskDisplayName  description) const;
    virtual void AppendContextInfo (SAPDB_Char  const * const  filename,
                                    SAPDB_UInt4 const          linenumber,
                                    SAPDBErr_MessageList      &errlist,
                                    SAPDB_Char  const * const  title = 0) const;
    virtual void WriteToTrace (SAPDB_Char  const * const  title = 0) const;
public: /// internal interface
    /// Access for interfaces
    static SrvTasks_Coordinator& Instance();
    /// Creates the singleton and may be called only once.
    static void CreateInstance ( SAPDBMem_IRawAllocator &allocator );
    /// This registers a new server task context to the coordinator
    bool Register( SrvTasks_Task &srvtask,
                   Msg_List      &errlist );
    /// returns an allocator which must be used if memory is needed by user tasks
    /// but is released by servertasks automatically
    /// for example if the caller does no want to wait for the job end
    SAPDBMem_IRawAllocator& Allocator()
    {
        return m_Allocator;
    }
    /// This releases the servertask identified by the given index.
    /// This is only called by the servertask itself.
    bool Release ( RTETask_ITask  &task,
                   SAPDB_UInt      taskindex );
    /// This marks a servertask as starting a job
    /// This is only called by the servertask itself and sets the string for the console
    void SetTaskStarted ( RTETask_ITask           &task,
                          SAPDB_UInt               taskindex,
                          RTE_TaskDisplayName      description,
                          RTE_TaskDisplayNameShort shortdescription );
    /// This marks a servertask as a free server task
    /// This is only called by the servertask itself and resets the string for the console
    void SetTaskStopped ( RTETask_ITask  &task,
                          SAPDB_UInt      taskindex );
	/// returns the number of maximum available servertasks
    SAPDB_UInt MaxServerTasks() const
    {
        return m_Tasks.GetCapacity() - m_NumberOfReservedTasks;
    }
    /// returns currently available serertasks
    SAPDB_UInt AvailableServerTasks() const
    {
        return MaxServerTasks() - m_UsedServerTasks;
    }
private:
    /// hidden constructor
    SrvTasks_Coordinator(SAPDBMem_IRawAllocator &allocator)
    : m_Allocator                (allocator)
    , m_Tasks                    (allocator)
    , m_UsedServerTasks          (0)
    , m_NumberOfReservedTasks    (0)
    , m_NumberOfUsedReservedTasks(0)
    {}
	/// returns the number of maximum configured servertasks
    SAPDB_UInt MaxConfiguredServerTasks() const
    {
        return m_Tasks.GetCapacity();
    }
    /// Wait until all server tasks have registered themself
    void WaitUntilAllServertasksAssigned(RTETask_ITask &task) const;
private: /// members
    /// This instance pointer of the singleton
    static SrvTasks_Coordinator *m_Instance;
public:
    /// this control manages one servertask
    class ServerTaskControl
    {
    public:
        /// default ctor only for array allocation
        ServerTaskControl()
        : m_pTask (0)
        {
            Release();
        }
        /// assignment ctor
        ServerTaskControl(SrvTasks_Task &srvTask)
        : m_pTask (&srvTask)
        {
            Release();
        }
        /// returns true if a server task is assigned
        bool IsAssigned() const
        {
            return m_pTask != 0;
        }
        /// returns true if the server task is in use
        bool IsReserved() const
        {
            return m_Reserved;
        }
        /// returns true if the server task is in use
        SAPDB_UInt TaskId() const
        {
            return m_pTask != 0 ? m_pTask->GetContext().Task().ID() : 0;
        }
        /// returns true if the server task is in use
        void GetDescription(RTE_TaskDisplayName      description,
                            RTE_TaskDisplayNameShort shortdescription) const
        
        {
            SAPDB_strcpy ( description,      m_Description );
            SAPDB_strcpy ( shortdescription, m_ShortDescription );
        }
        /// This returns a server task and marks it as reserved.
        /// If 0 is returned no servertask is available.
        SrvTasks_Task* Reserve(const RTE_TaskDisplayName      description,
                               const RTE_TaskDisplayNameShort shortdescription)
        {
            if ( m_Reserved )
                return 0;
            m_Reserved = true;
            SAPDB_strcpy ( m_Description, description );
            SAPDB_strcpy ( m_ShortDescription, shortdescription );
            return m_pTask;
        }
        /// This releases a server task and unmarks it as reserved. false is returned on error.
        void Release()
        {
            m_Reserved            = false;
            m_Description[0]      = 0;
            m_ShortDescription[0] = 0;
        }
        /// write some trace
        void WriteToTrace (Kernel_VTrace &trace) const;
        /// print some information
        void AppendContextInfo (SAPDB_Char  const * const  filename,
                                SAPDB_UInt4 const          linenumber,
                                SAPDBErr_MessageList      &errlist,
                                SAPDB_Char  const * const  title = 0) const;
    private:
        /// indicates if the server task was reserved
        bool                      m_Reserved;
        SrvTasks_Task            *m_pTask;
        RTE_TaskDisplayName       m_Description;
        RTE_TaskDisplayNameShort  m_ShortDescription;
    friend SAPDB_OStream & operator<< (SAPDB_OStream &os, const SrvTasks_Coordinator::ServerTaskControl &control);
    };
    /// list of server task controls
    typedef Container_Vector<ServerTaskControl> TaskList;
private:
    /// the global allocator used for allocating jobs too
    SAPDBMem_IRawAllocator &m_Allocator;
    /// the list of all registered server tasks
    TaskList m_Tasks;
    /// usage counter for servertasks
    SAPDB_UInt m_UsedServerTasks;
    /// the number of reserved system servertasks
    SAPDB_UInt m_NumberOfReservedTasks;
    /// the number of used reserved system servertasks
    SAPDB_UInt m_NumberOfUsedReservedTasks;
};
#endif // SrvTasks_Coordinator_HPP
