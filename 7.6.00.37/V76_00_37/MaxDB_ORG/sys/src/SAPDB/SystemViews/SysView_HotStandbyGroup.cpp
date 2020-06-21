/****************************************************************************/
/*!

  @file        SysView_HotStandbyGroup.cpp

  -------------------------------------------------------------------------

  @author      JoergM

  @ingroup     SystemViews
  @brief       This module implements the "HotStandbyGroup" view class.

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
#include "SystemViews/SysView_HotStandbyGroup.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "RunTime/HotStandbyStorage/RTEHSS_StandbyNodeList.hpp"
#include "RunTime/System/RTESys_Time.h"

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

void SysView_HotStandbyGroup::Create()
{
    // Column 1
    m_Table->AppendCol (ITOCT_CHAR,      SV_LOCALNODE,                  64);
    // Column 2
    m_Table->AppendCol (ITOCT_TIMESTAMP, SV_ADDNODEDATE,                 0);
    // Column 3
    m_Table->AppendCol (ITOCT_TIMESTAMP, SV_REMOVENODEDATE,              0);
    // Column 4
    m_Table->AppendCol (ITOCT_CHAR,      SV_STATE,                      20);
    // Column 5
    m_Table->AppendCol (ITOCT_TIMESTAMP, SV_PREPARESTANDBYDATE,          0);
    // Column 6
    m_Table->AppendCol (ITOCT_TIMESTAMP, SV_REGISTERSTANDBYDATE,         0);
    // Column 7
    m_Table->AppendCol (ITOCT_TIMESTAMP, SV_SYNCHRONIZEDATE,             0);
    // Column 8
    m_Table->AppendCol (ITOCT_TIMESTAMP, SV_SUCCESSFULSYNCHRONIZEDATE,   0);
    // Column 9
    m_Table->AppendCol (ITOCT_FIXED,     SV_LASTVALIDLOGOFFSETSENT,     10);
    // Column 10
    m_Table->AppendCol (ITOCT_FIXED,     SV_FIRSTNEEDEDLOGOFFSET,       10);
    // Column 11
    m_Table->AppendCol (ITOCT_FIXED,     SV_SYNCHRONIZESENDCOUNT,       10);
    // Column 12 == SV_CC_HOTSTANDBYGROUP
    m_Table->AppendCol (ITOCT_FIXED,     SV_SYNCHRONIZERETRYCOUNT,      10);
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_HotStandbyGroup::GetColCount()
{
	return SV_CC_HOTSTANDBYGROUP;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_HotStandbyGroup::EstimateRows()
{
	return SV_ER_HOTSTANDBYGROUP;
}

/*---------------------------------------------------------------------------*/

void SysView_HotStandbyGroup::Execute()
{
    m_Table->GetCatalogTable();

    if (m_Context.IsOk())
    {
        RTEHSS_StandbyNodeList::Statistic const *nodeStatistic;

        for ( SAPDB_Int4 nodeIndex = 0;
              0 != ( nodeStatistic = RTEHSS_StandbyNodeList::Instance().GetStatistic(nodeIndex));
              ++nodeIndex )
        {
    // Column 1 SV_LOCALNODE
            AddColumn( (SAPDB_Char const *)&(nodeStatistic->nodeName[0])            );
    // Column 2 SV_ADDNODEDATE
            AddColumn(  nodeStatistic->timeOfAddNode,                   AsTimestamp );
    // Column 3 SV_REMOVENODEDATE
            AddColumn(  nodeStatistic->timeOfRemoveNode,                AsTimestamp );
    // Column 4 SV_STATE
            AddColumn(  RTEHSS_StandbyNodeList::Instance().GetNodeStateAsString(nodeStatistic->nodeState) );
    // Column 5 SV_PREPARESTANDBYDATE
            AddColumn(  nodeStatistic->timeOfPrepareStandby,            AsTimestamp );
    // Column 6 SV_REGISTERSTANDBYDATE
            AddColumn(  nodeStatistic->timeOfRegisterStandby,           AsTimestamp );
    // Column 7 SV_SYNCHRONIZEDATE
            AddColumn(  nodeStatistic->timeOfSynchronize,               AsTimestamp );
    // Column 8 SV_SUCCESSFULSYNCHRONIZEDATE
            AddColumn(  nodeStatistic->timeOfLastSuccessfulSynchronize, AsTimestamp );
    // Column 9 SV_LASTVALIDLOGOFFSETSENT
            AddColumn(  nodeStatistic->lastValidLogOffsetSent                       );
    // Column 10 SV_FIRSTNEEDEDLOGOFFSET
            AddColumn(  nodeStatistic->lastSavepointOffset                          );
    // Column 11 SV_SYNCHRONIZESENDCOUNT
	        AddColumn(  nodeStatistic->synchronizeSendCount                         );
    // Column 12 (== SV_CC_HOTSTANDBYGROUP) SV_SYNCHRONIZERETRYCOUNT
	        AddColumn(  nodeStatistic->synchronizeRetryCount                        );
        }
    }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
