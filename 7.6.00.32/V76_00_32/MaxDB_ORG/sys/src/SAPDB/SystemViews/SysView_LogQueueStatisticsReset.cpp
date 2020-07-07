/****************************************************************************/
/*!

  @file        SysView_LogQueueStatistics.cpp

  -------------------------------------------------------------------------

  @author      MartinB

  @ingroup     SystemViews
  @brief       This module implements the "LogQueueStatistics" view class.

  @see        

*/
/*-------------------------------------------------------------------------

  copyright:    Copyright (c) 2002-2005 SAP AG


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




*****************************************************************************/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "ggg00.h"
#include "SAPDBCommon/SAPDB_string.h"
#include "SystemViews/SysView_Defines.hpp"
#include "SystemViews/SysView_ITableObj.hpp"
#include "SystemViews/SysView_LogQueueStatisticsReset.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "Logging/Log_Volume.hpp"
#include "Logging/Log_QueueStatistics.hpp"
#include "RunTime/RTE_KGS.hpp"


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
 *  STATIC/INLINE FUNCTIONS  (PROTOTYPES)                                     *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

void SysView_LogQueueStatisticsReset::Create()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_LogQueueStatisticsReset::Create", SysView_Trace, 5);

	m_Table->AppendCol (ITOCT_TIMESTAMP, SV_RESETDATE, 0);
	m_Table->AppendCol (ITOCT_FIXED ,SV_QUEUEID, 5);
	m_Table->AppendCol (ITOCT_FIXED ,SV_QUEUEMAXUSED, 5);
	m_Table->AppendCol (ITOCT_FIXED ,SV_QUEUEINSERTCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_QUEUEOVERFLOWCOUNT, 10);
	m_Table->AppendCol (ITOCT_FIXED ,SV_GROUPCOMMITCOUNT, 10);
	m_Table->AppendCol (ITOCT_FIXED ,SV_WAITCOUNT, 10);
	m_Table->AppendCol (ITOCT_FIXED ,SV_MAXWAITCOUNTPERPAGE, 10);
  	m_Table->AppendCol (ITOCT_FIXED ,SV_PHYSICALWRITECOUNT, 10);
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_LogQueueStatisticsReset::GetColCount()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_LogQueueStatisticsReset::GetColCount", SysView_Trace, 5);

    return SV_CC_LOGQUEUESTATISTICSRESET;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_LogQueueStatisticsReset::EstimateRows()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_LogQueueStatisticsReset::EstimateRows", SysView_Trace, 5);

    RTE_TaskId taskId = m_Context.GetTaskId();
    return Log_Volume::Instance().GetQueueCountFromInfoPage(taskId);
}

/*---------------------------------------------------------------------------*/

void SysView_LogQueueStatisticsReset::Execute()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_LogQueueStatisticsReset::Execute", SysView_Trace, 5);

    Log_QueueStatistics::Log_QueueStatisticValues statistics;
    SAPDB_UInt8                                   timeOfLastReset;

    // get the current table descriptor
    m_Table->GetCatalogTable();
    RTE_TaskId taskId = m_Context.GetTaskId();
    Log_Volume & log = Log_Volume::Instance();
    SAPDB_Int queueCount = log.GetQueueCountFromInfoPage(taskId);

    for (Log_QueueID queueIndex(0); queueIndex < queueCount; ++queueIndex)
    {
        Log_QueueStatisticManager.GetStatisticsReset (taskId, queueIndex, timeOfLastReset, statistics);

        SAPDB_UInt4 queueIndexUInt4 = queueIndex;
        AddColumn( timeOfLastReset, AsTimestamp);
        m_Table->MoveToCol( ITOVT_UINT4,  &queueIndexUInt4, 0);
        m_Table->MoveToCol( ITOVT_UINT4,  &statistics.queueMaxUsed, 0);
        m_Table->MoveToCol( ITOVT_UINT4,  &statistics.queueInsertCount, 0);
        m_Table->MoveToCol( ITOVT_UINT4,  &statistics.queueOverflowCount, 0);
        m_Table->MoveToCol( ITOVT_UINT4,  &statistics.groupCommitCount, 0);
        m_Table->MoveToCol( ITOVT_UINT4,  &statistics.waitCount, 0);
        m_Table->MoveToCol( ITOVT_UINT4,  &statistics.maxWaitCountPerPage, 0);
        m_Table->MoveToCol( ITOVT_UINT4,  &statistics.physWriteCount, 0);
    }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
