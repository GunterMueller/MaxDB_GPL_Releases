/*!
  \file    DBMSrvCmdScd_Header.hpp
  \author  MarcW
  \ingroup scheduler commands 
  \brief   This contains all headers from the Scheduler directory to avoid 
           "to long description line" errors

    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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

#if !defined(DBMSrvCmdScd_Header_hpp)
#define DBMSrvCmdScd_Header_hpp

#include "DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerActivateJob.hpp"
#include "DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerDeactivateJob.hpp"
#include "DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerDeleteJob.hpp"
#include "DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerState.hpp"
#include "DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerListJobs.hpp"
#include "DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerCreateJob.hpp"
#include "DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerStart.hpp"
#include "DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerStop.hpp"
#include "DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerShowJoblog.hpp"
#include "DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerDeleteJoblog.hpp"

#endif // DBMSrvCmdScd_Header
