/**************************************************************************/
/*!

  @file         Kernel_Task.hpp
  @author       UweH
  @brief        This implements the class Kernel_Task which encapsulates the RTETask_ITask
  @since        2004-11-24
*/
/*!
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
#ifndef KERNEL_TASK_HPP
#define KERNEL_TASK_HPP
//-----------------------------------------------------------------------------
#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "SAPDBCommon/Messages/SDBMsg_KernelCommon.h"
#include "KernelCommon/Kernel_Types.hpp"
#include "Transaction/Trans_Context.hpp"
//-----------------------------------------------------------------------------
class SQLMan_Context;
class Kernel_Task;
//-----------------------------------------------------------------------------
/// You can find the implementation in Kernel_Initialization.cpp
extern Kernel_Task Kernel_TaskLocalStorage;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/// This class is a high level interface to the kernel task context
class Kernel_Task
{
public:
    /// initialize all slots
    Kernel_Task()
    : m_Initialized(false)
    {
        for ( SAPDB_UInt slot = 0; slot < SlotTypes_LAST_DO_NOT_USE; ++slot )
            m_Slots[slot] = SAPDB_MAX_UINT4;
    }
    /// returns instance pointer
    static Kernel_Task& Instance()
    {
        return Kernel_TaskLocalStorage;
    }
    /// must be called by Kernel_Initialization, otherwise false is returned
    bool Initialize (RTETask_ITask &timeouttask)
    {
        RTETask_ITask &task = *RTETask_ITask::Self();
        if ( RTETask_ITask::Timer != task.Type() )
            return false;
        m_Slots[TransContextSlot ] = task.AllocateLocalStorage();
        m_Slots[SQLManContextSlot] = task.AllocateLocalStorage();
        m_Initialized = true;
        return true;
    }
    /// set the transcontext in the task local storage
    bool SetTransContext (RTETask_ITask &task,
                          Trans_Context &trans)
	{
        if ( m_Initialized )
            return task.SetLocalStorageValue( m_Slots[TransContextSlot],  &trans );
        else
            return false;
	}
    /// set the transcontext in the task local storage
    Trans_Context* GetTransContext (RTETask_ITask &task)
	{
        if ( m_Initialized )
        {
            #ifdef SAPDB_SLOW
            if ( Common_Check.ChecksLevel(7) )
            {
                Trans_Context* checktrans = reinterpret_cast<Trans_Context*>(task.GetLocalStorageValue( m_Slots[TransContextSlot] ));
                if ( task.ID() != checktrans->Task().ID() )
                    RTE_Crash( Msg_List( Msg_List::Error, SDBMSG_KERNELCOMMON_TASK_LOCAL_STORAGE_CHECK,
                                         Msg_Arg("TASK_ID", SAPDB_ToString(checktrans->Task().ID())),
                                         Msg_Arg("VALUE",   SAPDB_ToString(task.ID())) ) );
            }
            #endif
            return reinterpret_cast<Trans_Context*>(task.GetLocalStorageValue( m_Slots[TransContextSlot] ));
                
        }
        else
            return 0;
	}
    /// delete slot for transcontext
    bool DelTransContext (RTETask_ITask &task)
	{
        if ( m_Initialized )
            return task.SetLocalStorageValue( m_Slots[TransContextSlot],  0 );
        else
            return false;
	}
    /// set the sqlcontext in the task local storage
    bool SetSQLManContext (RTETask_ITask  &task,
                           SQLMan_Context &sqlcontext)
	{
        if ( m_Initialized )
            return task.SetLocalStorageValue( m_Slots[SQLManContextSlot],  &sqlcontext );
        else
            return false;
	}
    /// set the sqlcontext in the task local storage
    SQLMan_Context* GetSQLManContext (RTETask_ITask &task)
	{
        if ( m_Initialized )
        {
            #ifdef SAPDB_SLOW
            if ( Common_Check.ChecksLevel(7) )
            {
                Trans_Context* checktrans = reinterpret_cast<Trans_Context*>(task.GetLocalStorageValue( m_Slots[TransContextSlot] ));
                if ( task.ID() != checktrans->Task().ID() )
                    RTE_Crash( Msg_List( Msg_List::Error, SDBMSG_KERNELCOMMON_TASK_LOCAL_STORAGE_CHECK,
                                         Msg_Arg("TASK_ID", SAPDB_ToString(checktrans->Task().ID())),
                                         Msg_Arg("VALUE",   SAPDB_ToString(task.ID())) ) );
            }
            #endif
            return reinterpret_cast<SQLMan_Context*>(task.GetLocalStorageValue( m_Slots[SQLManContextSlot] ));
        }
        else
            return 0;
	}
    /// delete slot for sqlcontext
    bool DelSQLManContext (RTETask_ITask &task)
	{
        if ( m_Initialized )
            return task.SetLocalStorageValue( m_Slots[SQLManContextSlot],  0 );
        else
            return false;
	}
private:
    /// slot types
    enum SlotTypes
    {
        TransContextSlot  = 0,
        SQLManContextSlot = 1,
        SlotTypes_LAST_DO_NOT_USE = 2
    };
    SAPDB_UInt4 m_Slots[SlotTypes_LAST_DO_NOT_USE];
    bool        m_Initialized;
};
#endif /* KERNEL_TASK_HPP */
