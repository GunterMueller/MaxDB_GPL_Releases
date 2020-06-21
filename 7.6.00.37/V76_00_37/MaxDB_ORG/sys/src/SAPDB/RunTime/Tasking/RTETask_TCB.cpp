/****************************************************************************

  module      : RTETask_TCB.cpp

  -------------------------------------------------------------------------

  responsible : RobinW

  special area: System Interface
  description : Interface class to hide system dependencys of task control block

  -------------------------------------------------------------------------

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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
#include "SAPDBCommon/SAPDB_Types.h"
#include "RunTime/Tasking/RTETask_TCB.hpp"
#include "RunTime/Tasking/RTETask_Yield.h"
#include "RunTime/System/RTESys_Spinlock.h"
#include "geo007_1.h"
#include "geo50_0.h"

#ifndef WIN32
#include "hen50.h" /* nocheck */
#endif

#include "heo51.h"
#include "heo56.h"

void RTETask_getTCBandUKTCB( RTE_TCB &TCB, RTE_UKT &UKTCB, RTE_TaskId &pid )
{
    if(RTE_UnknownTaskId != pid)
    {
        TASK_CTRL pTaskCtrl;
        PID_TCB(pid,pTaskCtrl);
        TCB     = pTaskCtrl;
        UKTCB   = TCB.ukt();
    }
    else
    {
        UKTCB   = THIS_UKT_CTRL;
        TCB     = UKTCB.activeTask();
        pid     = TCB.taskIndex();
    }
}


#ifdef WIN32
SAPDB_Long RTE_TCB::m_dummy;
#endif

/*!
  @brief Yield call inside kernel
  If kernel code runs outside of task group a thread rescheduling is executed.
  If kernel code runs inside a task group, it uses the dispatcher to find a new
  task, and a thread rescheduling is only executed, if a new tasks exists. Otherwise
  a switch to the new task is done.
 */
extern "C" void RTETask_Yield(SAPDB_Bool allowToMove)
{
    RTE_TaskId ownId;
    vgetpid( ownId );

    if ( RTE_UnknownTaskId != ownId )
    {
        TASK_CTRL ownTCB;
        PID_TCB(ownId, ownTCB);

        // Prevent task from being moved...
        if ( !allowToMove ) vmovelock( ownId );
          SAPDB_UInt8 taskSwitches = UKT(ownTCB)->taskSwitches;
          vsleep(ownId, 0);
        if ( !allowToMove ) vmoveunlock( ownId );

        /* if number of task switches did, not change, we can savely reschedule the thread... */
        if ( taskSwitches == UKT(ownTCB)->taskSwitches )
        {
            RTESys_GiveUpTimeSlice();
        }
    }
    else
    {
        RTESys_GiveUpTimeSlice();
    }
}
