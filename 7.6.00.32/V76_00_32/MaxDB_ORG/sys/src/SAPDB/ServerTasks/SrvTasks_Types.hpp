  /*!
  @file           SrvTasks_Types.hpp
  @author         UweH
  @ingroup        ServerTasks
  @brief          Base types of this component

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
#ifndef SRVTASKS_TYPES_HPP
#define SRVTASKS_TYPES_HPP

#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include "SAPDBCommon/Diagnostics/SAPDBDiag_CheckTopic.hpp"
//--------------------------------------------------------------------------------------
/// trace level for servertasks
extern SAPDBTrace_Topic     SrvTasks_Trace;
/// check level for servertasks
extern SAPDBDiag_CheckTopic SrvTasks_Check;
//--------------------------------------------------------------------------------------
class SrvTasks_ITask;
/// a requested server task context
struct SrvTasks_RequestedTask
{
    /// pointer to server task context
    SrvTasks_ITask* pTask;
    /// index within servertask manager
    SAPDB_UInt      taskindex;
    /// initializes all
    SrvTasks_RequestedTask()
    :pTask(0),taskindex(SAPDB_MAX_UINT4)
    {}
    /// assigns all
    SrvTasks_RequestedTask(SrvTasks_ITask* pTaskNew,
                           SAPDB_UInt      taskindexNew)
    :pTask(pTaskNew),taskindex(taskindexNew)
    {}
};
#endif // SRVTASKS_TYPES_HPP