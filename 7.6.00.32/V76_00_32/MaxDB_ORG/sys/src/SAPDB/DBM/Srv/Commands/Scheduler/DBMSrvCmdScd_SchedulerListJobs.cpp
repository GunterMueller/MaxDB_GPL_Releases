/*!
  \file    DBMSrvCmdScd_SchedulerListJobs.cpp
  \author  MarcW
  \ingroup DBMServer commands
  \brief   list all scheduled jobs

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

#include "hcn50.h"

#include "DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerListJobs.hpp"
#include "DBM/Srv/Commands/Scheduler/DBMSrvCmdScd_SchedulerState.hpp"
#include "DBM/Srv/SharedMemory/DBMSrvShM_LineAccessorScheduler.hpp"

DBMSrvCmdScd_SchedulerListJobs::DBMSrvCmdScd_SchedulerListJobs()
: DBMSrvCmdScd_SchedulerCommand( DBMSrv_Command::KeySchedulerListJobs, DBMSrvCmdScd_SchedulerReply::getListExplanation()) {}

tcn00_Error DBMSrvCmdScd_SchedulerListJobs::runLockProtected(
    VControlDataT* vcontrol,
    CommandT* command,
    DBMSrvCmdScd_SchedulerReply& theSchedReply) {

    DBMSrvMsg_Error errOut;
    theSchedReply.startWithOK();

    // now send object to scheduler
    DBMSrvShM_LineAccessorScheduler laSch(errOut);
    if( !errOut.IsEmpty() )
        return theSchedReply.startWithMessageList(errOut);
    
    if( !(laSch.setInfoType(DBMSrvShM_LineAccessorScheduler::IT_RequestJobList, errOut) &&
          laSch.setUserName(getUser(), errOut) &&
          laSch.setRecipientScheduler(errOut)) )
        return theSchedReply.startWithMessageList(errOut);

    // start the scheduler if it's not running yet
    DBMSrvCmdScd_SchedulerState cmdGetState;
    bool schedulerUpInitialy(cmdGetState.isSchedulerUp(errOut));
    if( !errOut.IsEmpty() )
        return theSchedReply.startWithMessageList(errOut);

    if( !schedulerUpInitialy ) {
        tcn00_Error theRC = startScheduler(theSchedReply);
        if( theRC != OK_CN00 )
            return theRC;
    }

    // wait for answer from scheduler
    if( !laSch.clientWait(errOut) )
        return theSchedReply.startWithMessageList(errOut);

    // now there should be an answer
    DBMSrvShM_LineAccessorScheduler::InfoType infTy;
    char* pVarPart(NULL);
    if( !(laSch.getInfoType(infTy, errOut) && laSch.getVariablePart(pVarPart, errOut)) ) {
        if( pVarPart != NULL )
            delete pVarPart;
        return theSchedReply.startWithMessageList(errOut);
    }

    theSchedReply.appendSchedulerInfo(infTy, pVarPart);

    if( pVarPart != NULL )
        delete pVarPart;

    // we processed what came back, now clear shm line
    if( !laSch.clearContent(errOut) )
        return theSchedReply.startWithMessageList(errOut);

    // stop scheduler if it was not up initially
    if( !schedulerUpInitialy )
        stopScheduler(theSchedReply);

    // return the information we collected in the reply
    return theSchedReply.ReturnCode();
}
