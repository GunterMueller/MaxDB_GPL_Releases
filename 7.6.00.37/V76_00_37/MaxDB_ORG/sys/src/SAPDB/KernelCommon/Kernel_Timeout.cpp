/*!
  @file         Kernel_Timeout.cpp
  @author       UweH
  @brief        This implements the class Kernel_Timeout which is the realization of the timeout task.
  @since        2004-11-03
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
//-----------------------------------------------------------------------------
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "KernelCommon/Kernel_Timeout.hpp"
#include "ggg00.h"   // tgg00_TransContext
#include "heo52.h"   // vclock
#include "ggg92.h"   // for hkb51.h (tgg92_KernelOid)
#include "hkb51.h"   // k51clear_locklist
#include "hak341.h"  // ak341OmsInspectTimeout
#include "hbd91.h"   // bd91CheckRetentionTime
#include "Logging/Log_Volume.hpp"
#include "ServerTasks/SrvTasks_JobDropAuxFiles.hpp"            
#include "SQLManager/Catalog/Catalog_Cache.hpp"
#include "Transaction/Trans_Context.hpp"
#include "Converter/Converter_ICommon.hpp"
//-----------------------------------------------------------------------------
void Kernel_Timeout::Run(Trans_Context &context)
{
    SAPDB_Int4          timeoutInterval = cgg04_lc_timeout_interval;
    SAPDB_Int4          timeoutValue = timeoutInterval;
    SAPDB_Int4          startTime;
    SAPDB_Int4          stopTime;
    SAPDB_Int4          microsec;
 
    while ( 1 )
    {
        context.Task().Sleep(timeoutValue);
        vclock (&startTime, &microsec);
        // -------------------------------------
        if (  Kernel_IAdminInfo::Instance().KernelStateIsOnline() )
        {
            context.TimeoutTaskInit();
            k51clear_locklist (context.OldTrans());
            context.TimeoutTaskInit();
            ak341OmsInspectTimeout(); // PTS 1110287
            context.TimeoutTaskInit();
            if ( Log_Volume::IsReadyForWriting() )
                Log_Volume::Instance().CheckForLogSaveAndSavepoint(context);
            context.TimeoutTaskInit();
            (void)Kernel_IAdminHotStandby::Instance().TriggerSynchronize (context.OldTrans());
            context.TimeoutTaskInit();
            SrvTasks_JobDropAuxFiles::CheckAndStartDropServer(context.Task().ID());
            context.TimeoutTaskInit();
            Catalog_Cache::Shrink(context.Task().ID());
        }
        context.TimeoutTaskInit();
        bd91CheckRetentionTime(context.Task().ID());
        context.TimeoutTaskInit();
        Converter_ICommon::Instance().CheckAndStartSavepoint(context.Task(), context.ErrorList());
        // -------------------------------------
        vclock (&stopTime, &microsec);
        if ( timeoutInterval < stopTime - startTime )
            timeoutValue = 0; // reschedule
        else
            timeoutValue = timeoutInterval - (stopTime - startTime);
    }
}
