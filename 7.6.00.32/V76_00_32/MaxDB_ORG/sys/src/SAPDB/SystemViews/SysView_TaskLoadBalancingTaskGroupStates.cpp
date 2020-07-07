/*!========================================================================

  @file         SysView_TaskLoadBalancingTaskGroupStates.cpp
  @ingroup      
  @author       RaymondR

  @brief        This module implements the "TaskLoadBalancingTaskGroupStates" view class.


  @since        2004-06-01  18:21
  @sa           

  ==========================================================================

  \if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2004-2005 SAP AG

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
 ============================================================================*/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "ggg00.h"
#include "SystemViews/SysView_Defines.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "SystemViews/SysView_ITableObj.hpp"
#include "SystemViews/SysView_TaskLoadBalancingTaskGroupStates.hpp"
#include "RunTime/Tasking/RTETask_LoadBalancingInfo.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/


void SysView_TaskLoadBalancingTaskGroupStates::Create()
{
	m_Table->AppendCol (ITOCT_FIXED,     SV_TASKGROUPID,                     10); // Column 1
	m_Table->AppendCol (ITOCT_FIXED,     SV_MOVETASKTOTASKGROUPID,           10); // Column 2
    m_Table->AppendCol (ITOCT_FIXED,     SV_MOVEABLETASKCOUNT,               10); // Column 3
    m_Table->AppendCol (ITOCT_TIMESTAMP, SV_INTERVALSTARTTIME,                0); // Column 4
    m_Table->AppendCol (ITOCT_FIXED,     SV_TASKGROUPSLEEPTIME,              20); // Column 5
    m_Table->AppendCol (ITOCT_FIXED,     SV_TOTALTASKRUNNABLETIME,           20); // Column 6
    m_Table->AppendCol (ITOCT_FIXED,     SV_MAXTASKRUNNABLETIME,             20); // Column 7
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_TaskLoadBalancingTaskGroupStates::GetColCount()
{
	return SV_CC_TASKLOADBALANCINGTASKGROUPSTATES;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_TaskLoadBalancingTaskGroupStates::EstimateRows()
{
	return SV_ER_TASKLOADBALANCINGTASKGROUPSTATES;
}

/*---------------------------------------------------------------------------*/

void SysView_TaskLoadBalancingTaskGroupStates::Execute()
{
    m_Table->GetCatalogTable();

    if(m_Context.IsOk())
    {
       RTETask_LoadBalancingInfo   LoadBalancingInfo;

       LoadBalancingInfo.SnapShot();

       RTETask_LoadBalancingInfo::RTETask_UKTMoveInfo const * pMoveInfo;

       pMoveInfo = LoadBalancingInfo.GetFirstUKTMoveInfo();

       while ( 0 != pMoveInfo )
       {
           // Column 1 SV_TASKGROUPID
           AddColumn(  pMoveInfo->GetUKTIndex() );

           // Column 2 SV_MOVETASKTOTASKGROUPID
           AddColumn( pMoveInfo->GetMoveTaskToUKTIndex() );

           // Column 3 SV_MOVEABLETASKCOUNT
           AddColumn( pMoveInfo->GetNumOfMovableTasks() );

           // Column 4 SV_INTERVALSTARTTIME
           AddColumn( pMoveInfo->GetTaskMoveIntervalStartTime(), AsTimestamp );

           // Column 5 SV_TASKGROUPSLEEPTIME
           AddColumn( pMoveInfo->GetUKTSleepTimeMicroSeconds() );

           // Column 6 SV_TOTALTASKRUNNABLETIME
           AddColumn( pMoveInfo->GetRunnableMicroSeconds() );

           // Column 7 SV_MAXTASKRUNNABLETIME
           AddColumn( pMoveInfo->GetMaxRunnableMicroSeconds() );

           pMoveInfo = LoadBalancingInfo.GetNextUKTMoveInfo ();
       }
    }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/