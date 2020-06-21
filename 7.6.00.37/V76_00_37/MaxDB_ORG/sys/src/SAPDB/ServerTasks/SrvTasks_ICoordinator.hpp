/*!
  @file           SrvTasks_ICoordinator.hpp
  @author         UweH
  @ingroup        ServerTasks
  @brief          This defines the interface a job can call

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
#ifndef SrvTasks_ICoordinator_HPP
#define SrvTasks_ICoordinator_HPP

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "ServerTasks/SrvTasks_ITask.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"

class  RTETask_ITask;
class  SrvTasks_WaitingTask;
struct SrvTasks_RequestedTask;
/*!
@interface SrvTasks_ICoordinator
@brief     This is used to reserve server tasks for jobs and to read statistic information.
*/
class SrvTasks_ICoordinator
{
public: 
    /// Access for interfaces
    static SrvTasks_ICoordinator& Instance();
    /// result of Reserve()
    enum ReserveResult
    {
        reserveOK,
        reserveNotAllRequested,
        reserveNoServerAvailable,
        reserveFailed
    };
	/*!
    @brief   This reserves one server task.
    @param   task [in] task context
    @param   servertask [out] returned reserved servertask
    @param   errlist [out] hint in case of an error
    @param   description  [in] usage information, used in systemviews
    @param   shortdescription  [in] usage information, used by x_cons
    @param   forSystemUsage [in] this is only used internally for special server tasks
    @return  ReserveResult
    The Size of the list gives the wanted amount of server tasks.
    The Length gives the amount of server tasks which are granted.
    */
    virtual ReserveResult Reserve ( RTETask_ITask                 &task,
                                    SrvTasks_RequestedTask        &servertask,
                                    Msg_List                      &errlist,
                                    const RTE_TaskDisplayName      description,
                                    const RTE_TaskDisplayNameShort shortdescription,
                                    bool                           forSystemUsage = false ) = 0;
    /// returns the number of maximum available servertasks
    virtual SAPDB_UInt4 GetMaxServerTasks() const = 0;
    /// returns currently available serertasks
    virtual SAPDB_UInt GetAvailableServerTasks() const = 0;
    /// get information of a servertask, returns false if no data available
    virtual bool GetInfo (RTETask_ITask       &task,
                          SAPDB_UInt           controlNo,
                          SAPDB_UInt          &servertaskid,
                          bool                &isAssigned,
                          bool                &isReserved,
                          RTE_TaskDisplayName  description) const = 0;
    /// Appends some context information
    virtual void AppendContextInfo (SAPDB_Char  const * const  filename,
                                    SAPDB_UInt4 const          linenumber,
                                    SAPDBErr_MessageList      &errlist,
                                    SAPDB_Char  const * const  title = 0) const = 0;
    /// Writze context information to knltrace
    virtual void WriteToTrace (SAPDB_Char  const * const  title = 0) const = 0;
};
#endif // SrvTasks_ICoordinator_HPP
