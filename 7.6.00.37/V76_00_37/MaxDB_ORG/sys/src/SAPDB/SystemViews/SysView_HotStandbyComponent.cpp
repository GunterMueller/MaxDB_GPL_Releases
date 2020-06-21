/****************************************************************************/
/*!

  @file        SysView_HotStandbyComponent.cpp

  -------------------------------------------------------------------------

  @author      JoergM

  @ingroup     SystemViews
  @brief       This module implements the "HotStandbyComponent" view class.

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
#include "SystemViews/SysView_HotStandbyComponent.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "RunTime/RTE_KGS.hpp"
#include "RunTime/HotStandbyStorage/RTEHSS_KernelInterface.hpp"
#include "RunTime/System/RTESys_Time.h"

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

void SysView_HotStandbyComponent::Create()
{
    // Column 1
	m_Table->AppendCol (ITOCT_CHAR,      SV_LOCALNODE,                  64);
    // Column 2
	m_Table->AppendCol (ITOCT_CHAR,      SV_MASTERLOCALNODE,            64);
    // Column 3
	m_Table->AppendCol (ITOCT_CHAR,      SV_STANDBYROLE,                 8);
    // Column 4
	m_Table->AppendCol (ITOCT_TIMESTAMP, SV_TAKEOVERDATE,                0);
    // Column 5
	m_Table->AppendCol (ITOCT_FIXED,     SV_DELAYTIME,                  10);
    // Column 6
    m_Table->AppendCol (ITOCT_TIMESTAMP, SV_STANDBYSTATEDATE,            0);
    // Column 7
	m_Table->AppendCol (ITOCT_FIXED,     SV_SYNCHRONIZERECEIVECOUNT,    10);
    // Column 8
	m_Table->AppendCol (ITOCT_FIXED,     SV_FIRSTNEEDEDLOGOFFSET,       10);
    // Column 9
	m_Table->AppendCol (ITOCT_FIXED,     SV_LASTVALIDLOGOFFSETRECEIVED, 10);
    // Column 10
	m_Table->AppendCol (ITOCT_TIMESTAMP, SV_INITSTANDBYDATE,             0);
    // Column 11
    m_Table->AppendCol (ITOCT_TIMESTAMP, SV_RESTARTSTANDBYDATE,          0);
    // Column 12
	m_Table->AppendCol (ITOCT_TIMESTAMP, SV_LASTSYNCHRONIZEDATE,         0);
    // Column 13
	m_Table->AppendCol (ITOCT_TIMESTAMP, SV_PREPARESTANDBYDATE,          0);
    // Column 14
	m_Table->AppendCol (ITOCT_TIMESTAMP, SV_REGISTERSTANDBYDATE,         0);
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_HotStandbyComponent::GetColCount()
{
	return SV_CC_HOTSTANDBYCOMPONENT;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_HotStandbyComponent::EstimateRows()
{
	return SV_ER_HOTSTANDBYCOMPONENT;
}

/*---------------------------------------------------------------------------*/

void SysView_HotStandbyComponent::Execute()
{
    m_Table->GetCatalogTable();

    if (m_Context.IsOk())
    {
        RTE_KGS::Statistic const &instanceStatistic = RTE_KGS::Instance().GetStatistic();
        RTEHSS_KernelInterface::Statistic const &hssStatistic = RTEHSS_KernelInterface::Instance().GetStatistic();

    // Column 1 SV_LOCALNODE
        AddColumn( (SAPDB_Char const *)&instanceStatistic.localNodeName[0]  );
    // Column 2 SV_MASTERLOCALNODE
        AddColumn( (SAPDB_Char const *)&hssStatistic.masterNodeName[0]      );
    // Column 3 SV_STANDBYROLE
        AddColumn(  RTEHSS_KernelInterface::Instance().GetOwnRoleAsString() );
    // Column 4 SV_TAKEOVERDATE
        AddColumn(  hssStatistic.timeOfTakeover,                AsTimestamp );
    // Column 5 SV_DELAYTIME
	    AddColumn(  hssStatistic.ownDelayTime                               );
    // Column 6 SV_STANDBYSTATEDATE
        AddColumn(  hssStatistic.timeOfStandbyState,            AsTimestamp );
    // Column 7  SV_SYNCHRONIZERECEIVECOUNT
	    AddColumn(  hssStatistic.synchronizeReceiveCount                    );
    // Column 8  SV_FIRSTNEEDEDLOGOFFSET
	    AddColumn(  hssStatistic.firstNeededLogOffsetReported               );
    // Column 9  SV_LASTVALIDLOGOFFSETRECEIVED
	    AddColumn(  hssStatistic.lastValidLogOffsetReceived                 );
    // Column 10 SV_INITSTANDBYDATE
        AddColumn(  hssStatistic.timeOfInitStandby,             AsTimestamp );
    // Column 11 SV_RESTARTSTANDBYDATE
        AddColumn(  hssStatistic.timeOfRestartStandby,          AsTimestamp );
    // Column 12 SV_LASTSYNCHRONIZEDATE
        AddColumn(  hssStatistic.timeOfSynchronize,             AsTimestamp );
    // Column 13 SV_PREPARESTANDBYDATE
        AddColumn(  hssStatistic.timeOfPrepareStandby,          AsTimestamp );
    // Column 14 == SV_CC_HOTSTANDBYCOMPONENT SV_REGISTERSTANDBYDATE
        AddColumn(  hssStatistic.timeOfRegisterStandby,         AsTimestamp );
    }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
