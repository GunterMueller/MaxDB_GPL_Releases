/****************************************************************************/
/*!

  @file        SysView_Instance.cpp

  -------------------------------------------------------------------------

  @author      JoergM

  @ingroup     SystemViews
  @brief       This module implements the "Instance" view class.

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
#include "SystemViews/SysView_Instance.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "RunTime/RTE_KGS.hpp"
#include "RunTime/System/RTESys_Time.h"

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

void SysView_Instance::Create()
{
	m_Table->AppendCol (ITOCT_CHAR,  SV_NODE, 64);
	m_Table->AppendCol (ITOCT_CHAR,  SV_LOCALNODE, 64);
	m_Table->AppendCol (ITOCT_TIMESTAMP, SV_ADMINSTATEDATE, 0);
	m_Table->AppendCol (ITOCT_TIMESTAMP, SV_ONLINESTATEDATE, 0);
	m_Table->AppendCol (ITOCT_FIXED, SV_KERNELPROCESSID, 20);
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_Instance::GetColCount()
{
	return SV_CC_INSTANCE;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_Instance::EstimateRows()
{
	return SV_ER_INSTANCE;
}

/*---------------------------------------------------------------------------*/

void SysView_Instance::Execute()
{
    m_Table->GetCatalogTable();

    if (m_Context.IsOk())
    {
        RTE_KGS::Statistic const &instanceStatistic = RTE_KGS::Instance().GetStatistic();

        AddColumn( (SAPDB_Char const *)&instanceStatistic.officialNodeName[0] );
        AddColumn( (SAPDB_Char const *)&instanceStatistic.localNodeName[0]    );
        AddColumn( instanceStatistic.timeOfAdminState, AsTimestamp            );
        AddColumn( instanceStatistic.timeOfOnlineState, AsTimestamp           );
        AddColumn( instanceStatistic.processId                                );
    }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
