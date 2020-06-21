/*!
  \file    DBMSrvCmdScd_SchedulerDeleteJoblog.cpp
  \author  MarcW
  \ingroup DBMServer commands
  \brief   command scheduler_delete_joblog

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

#include "DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerDeleteJoblog.hpp"

DBMSrvCmdScd_SchedulerDeleteJoblog::DBMSrvCmdScd_SchedulerDeleteJoblog()
: DBMSrv_Command( KeySchedulerDeleteJoblog, false )
{
    m_longHelp=
    "@command scheduler_delete_joblog delete the log of a job"
    "@preconditions You have the DBM operator authorizations SchedulerMgm or Scheduling."
    "@syntax scheduler_delete_joblog <job_id>"
    "@param <job_id> id of the job of which the log is to delete"
    "@reply OK<NL>"
        "und weiter weiﬂ ich noch nicht..."; 
}

tcn00_Error DBMSrvCmdScd_SchedulerDeleteJoblog::run(
    VControlDataT* vcontrol,
    CommandT* command,
    DBMSrv_Reply& theReply) {
    
    theReply.startWithOK();
    theReply.appendString("not yet implemented");
        
    return OK_CN00;
}
