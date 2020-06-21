/****************************************************************************/
/*!

  @file        SysView_LogStatisticsReset.cpp

  -------------------------------------------------------------------------

  @author      ElkeZ

  @ingroup     SystemViews
  @brief       This module implements the "LogStatisticsReset" view class.

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
#include "SystemViews/SysView_Defines.hpp"
#include "SystemViews/SysView_ITableObj.hpp"
#include "SystemViews/SysView_LogStatisticsReset.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "Logging/Log_Statistics.hpp"
#include "Logging/Log_Volume.hpp"
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

void SysView_LogStatisticsReset::Create()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_LogStatisticsReset::Create", SysView_Trace, 5);

	m_Table->AppendCol (ITOCT_TIMESTAMP, SV_RESETDATE, 0);
	m_Table->AppendCol (ITOCT_FIXED ,SV_SAVEPOINTCOUNT, 10);
	m_Table->AppendCol (ITOCT_FIXED ,SV_TRANSACTIONCOUNT, 10);
	m_Table->AppendCol (ITOCT_FIXED ,SV_WRITETRANSACTIONCOUNT, 10);
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_LogStatisticsReset::GetColCount()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_LogStatisticsReset::GetColCount", SysView_Trace, 5);

    return SV_CC_LOGSTATISTICSRESET;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_LogStatisticsReset::EstimateRows()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_LogStatisticsReset::EstimateRows", SysView_Trace, 5);

    return SV_ER_LOGSTATISTICSRESET;
}

/*---------------------------------------------------------------------------*/

void SysView_LogStatisticsReset::Execute()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_LogStatisticsReset::Execute", SysView_Trace, 5);

    Log_Statistics::Log_StatisticResetValues statistics;

    // get the current table descriptor
    m_Table->GetCatalogTable();
    RTE_TaskId taskId = m_Context.GetTaskId();

    Log_StatisticManager.GetStatisticsReset (taskId, statistics);

    AddColumn( statistics.timeOfLastReset, AsTimestamp );
    m_Table->MoveToCol( ITOVT_UINT4,  &statistics.savepointCount, 0);
    m_Table->MoveToCol( ITOVT_UINT8,  &statistics.selectTransactionCount, 0);
    m_Table->MoveToCol( ITOVT_UINT8,  &statistics.writeTransactionCount, 0);
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
