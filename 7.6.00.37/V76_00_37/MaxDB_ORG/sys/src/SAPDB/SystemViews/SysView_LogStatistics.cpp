/****************************************************************************/
/*!

  @file        SysView_LogStatistics.cpp

  -------------------------------------------------------------------------

  @author      ElkeZ

  @ingroup     SystemViews
  @brief       This module implements the "LogStatistics" view class.

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
#include "SystemViews/SysView_LogStatistics.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "Logging/Log_Statistics.hpp"


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

void SysView_LogStatistics::Create()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_LogStatistics::Create", SysView_Trace, 5);
    

    m_Table->AppendCol (ITOCT_FIXED ,SV_USABLESIZE, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_USEDSIZE, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_USEDSIZEPERCENTAGE, 3);
	m_Table->AppendCol (ITOCT_FIXED ,SV_NOTSAVEDSIZE, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_NOTSAVEDPERCENTAGE, 3);
	m_Table->AppendCol (ITOCT_FIXED ,SV_SAVEPOINTCOUNT, 10);
	m_Table->AppendCol (ITOCT_FIXED ,SV_TRANSACTIONCOUNT, 10);
	m_Table->AppendCol (ITOCT_FIXED ,SV_WRITETRANSACTIONCOUNT, 10);
	m_Table->AppendCol (ITOCT_FIXED ,SV_MAXUSEDSESSIONID, 10);
	m_Table->AppendCol (ITOCT_FIXED ,SV_MAXUSEDTRANSACTIONNUMBER, 15);
	m_Table->AppendCol (ITOCT_FIXED ,SV_QUEUECOUNT, 5);
	m_Table->AppendCol (ITOCT_FIXED ,SV_QUEUESIZE, 5);
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_LogStatistics::GetColCount()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_LogStatistics::GetColCount", SysView_Trace, 5);

    return SV_CC_LOGSTATISTICS;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_LogStatistics::EstimateRows()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_LogStatistics::EstimateRows", SysView_Trace, 5);

    return SV_ER_LOGSTATISTICS;
}

/*---------------------------------------------------------------------------*/

void SysView_LogStatistics::Execute()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_LogStatistics::Execute", SysView_Trace, 5);

    Log_Statistics::Log_StatisticValues statistics;

    // get the current table descriptor
    m_Table->GetCatalogTable();
    RTE_TaskId taskId = m_Context.GetTaskId();

    Log_StatisticManager.GetStatistics (taskId, statistics);

    m_Table->MoveToCol( ITOVT_UINT4,  &statistics.usableSize, 0);
    m_Table->MoveToCol( ITOVT_UINT4,  &statistics.usedSize, 0);
    m_Table->MoveToCol( ITOVT_UINT4,  &statistics.usedPercentage, 0);
    m_Table->MoveToCol( ITOVT_UINT4,  &statistics.notSavedSize, 0);
    m_Table->MoveToCol( ITOVT_UINT4,  &statistics.notSavePercentage, 0);
    m_Table->MoveToCol( ITOVT_UINT4,  &statistics.savepointCount, 0);
    m_Table->MoveToCol( ITOVT_UINT8,  &statistics.selectTransactionCount, 0);
    m_Table->MoveToCol( ITOVT_UINT8,  &statistics.writeTransactionCount, 0);
    m_Table->MoveToCol( ITOVT_UINT4,  &statistics.maxUsedSessionId, 0);
    m_Table->MoveToCol( ITOVT_UINT8,  &statistics.maxUsedTransNo, 0);
    m_Table->MoveToCol( ITOVT_UINT4,  &statistics.queueCount, 0);
    m_Table->MoveToCol( ITOVT_UINT4,  &statistics.queueSize, 0);

}

/*---------------------------------------------------------------------------*/

SAPDB_Bool SysView_LogStatistics::ResetCounter()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_LogStatistics::ResetCounter", SysView_Trace, 5);

    RTE_TaskId taskId = m_Context.GetTaskId();
    Log_StatisticManager.ResetStatistics(taskId);
    return true;
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
