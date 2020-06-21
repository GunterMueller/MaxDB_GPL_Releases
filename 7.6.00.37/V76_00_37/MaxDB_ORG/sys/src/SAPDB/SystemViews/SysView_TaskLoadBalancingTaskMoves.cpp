/*!========================================================================

  @file         SysView_TaskLoadBalancingTaskMoves.cpp
  @ingroup      
  @author       RaymondR

  @brief        This module implements the "TaskLoadBalancingTaskMoves" view class.


  @since        2004-06-01  18:11
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
#include "SystemViews/SysView_TaskLoadBalancingTaskMoves.hpp"
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


void SysView_TaskLoadBalancingTaskMoves::Create()
{
	m_Table->AppendCol (ITOCT_TIMESTAMP, SV_MOVETIME,                         0); // Column 1
	m_Table->AppendCol (ITOCT_FIXED,     SV_TASKID,                          10); // Column 2
    m_Table->AppendCol (ITOCT_FIXED,     SV_MAXTASKRUNNABLETIME,             20); // Column 3
    m_Table->AppendCol (ITOCT_FIXED,     SV_SOURCETASKGROUP,                 10); // Column 4
    m_Table->AppendCol (ITOCT_FIXED,     SV_SOURCEMAXTASKRUNNABLETIME,       20); // Column 5
    m_Table->AppendCol (ITOCT_FIXED,     SV_DESTINATIONTASKGROUP,            10); // Column 6
    m_Table->AppendCol (ITOCT_FIXED,     SV_DESTINATIONMAXTASKRUNNABLETIME,  20); // Column 7
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_TaskLoadBalancingTaskMoves::GetColCount()
{
	return SV_CC_TASKLOADBALANCINGTASKMOVES;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_TaskLoadBalancingTaskMoves::EstimateRows()
{
	return SV_ER_TASKLOADBALANCINGTASKMOVES;
}

/*---------------------------------------------------------------------------*/

void SysView_TaskLoadBalancingTaskMoves::Execute()
{
    m_Table->GetCatalogTable();

    if(m_Context.IsOk())
    {
       RTETask_LoadBalancingInfo   LoadBalancingInfo;

       LoadBalancingInfo.SnapShot();

       RTETask_LoadBalancingInfo::RTETask_MoveInfo const * pMoveInfo;

       pMoveInfo = LoadBalancingInfo.GetFirstTaskMoveInfo();

       while ( 0 != pMoveInfo )
       {
           // Column 1 SV_MOVETIME
           AddColumn(  pMoveInfo->GetMoveTime(),  AsTimestamp );

           // Column 2 SV_TASKID
           AddColumn( pMoveInfo->GetTaskIndex() );

           // Column 3 SV_MAXTASKRUNNABLETIME
           AddColumn( pMoveInfo->GetTaskMaxRunnableMicroSeconds() );

           // Column 4 SV_SOURCETASKGROUP
           AddColumn( pMoveInfo->GetSrcUKTIndex() );

           // Column 5 SV_SOURCEMAXTASKRUNNABLETIME
           AddColumn( pMoveInfo->GetSrcMaxRunnableMicroSeconds() );

           // Column 6 SV_DESTINATIONTASKGROUP
           AddColumn( pMoveInfo->GetDstUKTIndex() );

           // Column 7 SV_DESTINATIONMAXTASKRUNNABLETIME
           AddColumn( pMoveInfo->GetDstMaxRunnableMicroSeconds() );

           pMoveInfo = LoadBalancingInfo.GetNextTaskMoveInfo();
       }
    }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/