/****************************************************************************/
/*!

  @file        SysView_LogInformation.cpp

  -------------------------------------------------------------------------

  @author      ElkeZ

  @ingroup     SystemViews
  @brief       This module implements the "LogInformation" view class.

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
#include "SystemViews/SysView_LogInformation.hpp"
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

void SysView_LogInformation::Create()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_LogInformation::Create", SysView_Trace, 5);

	m_Table->AppendCol (ITOCT_CHAR  ,SV_LOGVOLUMEENABLED, 3);
	m_Table->AppendCol (ITOCT_CHAR  ,SV_AUTOMATICOVERWRITE, 3);
	m_Table->AppendCol (ITOCT_CHAR  ,SV_LOGWRITERSUSPENDED, 3);
    m_Table->AppendCol (ITOCT_CHAR  ,SV_FLUSHMODE, 20);
	m_Table->AppendCol (ITOCT_CHAR  ,SV_DEVICESTATE, 20);
	m_Table->AppendCol (ITOCT_CHAR  ,SV_LOGFULL, 3);
  	m_Table->AppendCol (ITOCT_FIXED ,SV_PENDINGLOGSIZE, 20);

}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_LogInformation::GetColCount()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_LogInformation::GetColCount", SysView_Trace, 5);

    return SV_CC_LOGINFORMATION;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_LogInformation::EstimateRows()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_LogInformation::EstimateRows", SysView_Trace, 5);

    return SV_ER_LOGINFORMATION;
}

/*---------------------------------------------------------------------------*/

void SysView_LogInformation::Execute()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_LogInformation::Execute", SysView_Trace, 5);

    Log_Statistics::Log_StatisticValues statistics;

    // get the current table descriptor
    m_Table->GetCatalogTable();
    RTE_TaskId taskId = m_Context.GetTaskId();

    Log_StatisticManager.GetStatistics (taskId, statistics);

    m_Table->MoveToCol (ITOVT_YESNO,   &statistics.logVolumeEnabled, 0);
    m_Table->MoveToCol (ITOVT_YESNO,   &statistics.automaticOverwrite, 0);
    m_Table->MoveToCol (ITOVT_YESNO,   &statistics.logWriterSuspended, 0);
    m_Table->MoveToCol (ITOVT_CHARPTR, Log_SysviewFlushModeStrings[statistics.flushMode] ,
                                       SAPDB_strlen(Log_SysviewFlushModeStrings[statistics.flushMode]));
    m_Table->MoveToCol (ITOVT_CHARPTR, Log_SysviewDeviceStateStrings[statistics.deviceState] ,
                                       SAPDB_strlen(Log_SysviewDeviceStateStrings[statistics.deviceState]));
    m_Table->MoveToCol (ITOVT_YESNO,   &statistics.logIsFull, 0);
    m_Table->MoveToCol (ITOVT_UINT4,   &statistics.pendingLogSize, 0);

}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
