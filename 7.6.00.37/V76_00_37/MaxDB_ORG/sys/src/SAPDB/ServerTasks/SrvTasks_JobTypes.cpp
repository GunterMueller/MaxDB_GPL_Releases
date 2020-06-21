  /*!
  @file           SrvTasks_JobTypes.cpp
  @author         UweH
  @ingroup        ServerTasks
  @brief          types of global known jobs for vtrace or rte console

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

#include "ServerTasks/SrvTasks_JobTypes.hpp"

//--------------------------------------------------------------------------------------

const RTE_TaskDisplayName SrvTasks_JobTypesString [] =
{
    "Unspecified",
    "Prefetch Blob",
    "Prefetch Select",
    "Prefetch Pages",
    "Prefetch Objetcs Coordinator",
    "Autosave Log Writer",
    "Autosave Log Reader",
    "Backup / Restore Medium Task",
    "Backup / Restore Volume Task",
    "Check Data",
    "Create Index",
    "Drop Auxiliary Files",
    "Prefetch Objects",
    "RedoLog Reader",
    "RedoLog Execution Task",
    "Standby Restart Coordinator",
    "Standby Synchronize Server",
    "Savepoint Coordinator",
    "Garbage Collector Task",
    "Join Index Select",
    "Update Statistics",
    "Drop Volume",
    "Update Counter"
    // !Keep in sync with SrvTasks_JobTypes and SrvTasks_JobTypesStringShort!
};

//--------------------------------------------------------------------------------------

const RTE_TaskDisplayNameShort SrvTasks_JobTypesStringShort [] =
{
    "Unspec ",
    "PrefBlb",
    "PrefSel",
    "PrefPag",
    "PrefOCo",
    "AutoLgW",
    "AutoLgR",
    "BUPmed ",
    "BUPvol ",
    "ChkData",
    "CrIndex",
    "DropAux",
    "PrefObj",
    "RedoRea",
    "RedoExe",
    "StdbyRC",
    "StdbySy",
    "Savepnt",
    "GarbCol",
    "JoinInS",
    "UpdStat",
    "DropVol",
    "UpdCnt"
    // !Keep in sync with SrvTasks_JobTypes and SrvTasks_JobTypesString!
};
