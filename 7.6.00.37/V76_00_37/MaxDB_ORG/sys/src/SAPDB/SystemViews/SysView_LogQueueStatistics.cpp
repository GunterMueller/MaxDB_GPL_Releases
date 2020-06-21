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
#include "SystemViews/SysView_LogQueueStatistics.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "Logging/Log_Volume.hpp"
#include "Logging/Log_QueueStatistics.hpp"


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

void SysView_LogQueueStatistics::Create()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_LogQueueStatistics::Create", SysView_Trace, 5);

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

SAPDB_Int SysView_LogQueueStatistics::GetColCount()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_LogQueueStatistics::GetColCount", SysView_Trace, 5);

    return SV_CC_LOGQUEUESTATISTICS;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_LogQueueStatistics::EstimateRows()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_LogQueueStatistics::EstimateRows", SysView_Trace, 5);

    RTE_TaskId taskId = m_Context.GetTaskId();
    return Log_Volume::Instance().GetQueueCountFromInfoPage(taskId);
}

/*---------------------------------------------------------------------------*/

void SysView_LogQueueStatistics::Execute()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_LogQueueStatistics::Execute", SysView_Trace, 5);

    Log_QueueStatistics::Log_QueueStatisticValues statistics;

    // get the current table descriptor
    m_Table->GetCatalogTable();
    RTE_TaskId taskId = m_Context.GetTaskId();
    Log_Volume & log = Log_Volume::Instance();
    SAPDB_Int queueCount = log.GetQueueCountFromInfoPage(taskId);

    for (Log_QueueID queueIndex(0); queueIndex < queueCount; ++queueIndex)
    {
        Log_QueueStatisticManager.GetStatistics (taskId, queueIndex, statistics);

        SAPDB_UInt4 queueIndexUInt4 = queueIndex;
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

SAPDB_Bool    SysView_LogQueueStatistics::ResetCounter (void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_LogQueueStatistics::ResetCounter", SysView_Trace, 5);

    RTE_TaskId taskId = m_Context.GetTaskId();
    Log_QueueStatisticManager.ResetStatistics(taskId);
    return true;
}
/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
