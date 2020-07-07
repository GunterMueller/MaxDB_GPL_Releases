/*!========================================================================

  @file         SysView_TaskLoadBalancingInformation.cpp
  @ingroup      
  @author       RaymondR

  @brief        This module implements the "TaskLoadBalancingInformation" view class.

  @since        2004-06-14  10:17
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
#include "SystemViews/SysView_TaskLoadBalancingInformation.hpp"
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


void SysView_TaskLoadBalancingInformation::Create()
{
	m_Table->AppendCol (ITOCT_FIXED,     SV_MEASUREMENTINTERVALTIME,   10);  // Column 1
    m_Table->AppendCol (ITOCT_TIMESTAMP, SV_INTERVALSTARTTIME,          0);  // Column 2
    m_Table->AppendCol (ITOCT_FIXED,     SV_MEASUREMENTINTERVALS,      10);  // Column 3
	m_Table->AppendCol (ITOCT_FIXED,     SV_VALUEEQUALITY,              3);  // Column 4
    m_Table->AppendCol (ITOCT_FIXED,     SV_MINDIFFERENCE,              3);  // Column 5
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_TaskLoadBalancingInformation::GetColCount()
{
	return SV_CC_TASKLOADBALANCINGINFORMATION;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_TaskLoadBalancingInformation::EstimateRows()
{
	return SV_ER_TASKLOADBALANCINGINFORMATION;
}

/*---------------------------------------------------------------------------*/

void SysView_TaskLoadBalancingInformation::Execute()
{
    m_Table->GetCatalogTable();

    if(m_Context.IsOk())
    {
       RTETask_LoadBalancingInfo   LoadBalancingInfo;

       LoadBalancingInfo.SnapShot();

       // Column 1 SV_MEASUREMENTINTERVALTIME
       AddColumn(  LoadBalancingInfo.CheckIntervalTime() );

       // Column 2 SV_INTERVALSTARTTIME
       AddColumn( LoadBalancingInfo.TaskMoveIntervalStartTime(), AsTimestamp );

       // Column 3 SV_MEASUREMENTINTERVALS
       AddColumn( LoadBalancingInfo.TaskMoveMeasIntervals() );

       // Column 4 SV_VALUEEQUALITY
       AddColumn( LoadBalancingInfo.ValueEquality() );

       // Column 5 SV_MINDIFFERENCE
       AddColumn( LoadBalancingInfo.MinimumDiff() );
    }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
