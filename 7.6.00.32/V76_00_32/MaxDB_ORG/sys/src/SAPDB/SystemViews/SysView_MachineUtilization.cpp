/****************************************************************************/
/*!

  @file        SysView_MachineUtilization.cpp

  -------------------------------------------------------------------------

  @author      RobinW

  @ingroup     SystemViews
  @brief       This module implements the "MachineULtilization" view class.

  @see        

*/
/*-------------------------------------------------------------------------

  copyright:    Copyright (c) 2003-2005 SAP AG


    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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
#include "SystemViews/SysView_MachineUtilization.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "RunTime/System/RTESys_SystemInfo.h"
#include "RunTime/System/RTESys_MemoryInfo.hpp"
#include "SAPDBCommon/SAPDB_sprintf.h"

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

void SysView_MachineUtilization::Create()
{
	m_Table->AppendCol (ITOCT_FIXED,  SV_PHYSICALMEMORYUSABLE, 10);
	m_Table->AppendCol (ITOCT_FIXED,  SV_VIRTUALMEMORYUSABLE,  10);
    m_Table->AppendCol (ITOCT_FIXED,  SV_PROCESSUSEDUSERTIME,  10);
    m_Table->AppendCol (ITOCT_FIXED,  SV_PROCESSUSEDSYSTEMTIME,10);
    m_Table->AppendCol (ITOCT_FIXED,  SV_MACHINEUSEDUSERTIME,  10);
    m_Table->AppendCol (ITOCT_FIXED,  SV_MACHINEUSEDSYSTEMTIME,10);
    m_Table->AppendCol (ITOCT_FIXED,  SV_MACHINEIDLETIME,  10);
    m_Table->AppendCol (ITOCT_CHAR,   SV_CPULOAD,12);
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_MachineUtilization::GetColCount()
{
	return SV_CC_MACHINEUTILIZATION;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_MachineUtilization::EstimateRows()
{
	return SV_ER_MACHINEUTILIZATION;
}

/*---------------------------------------------------------------------------*/

void SysView_MachineUtilization::Execute()
{
    m_Table->GetCatalogTable();
    if (m_Context.IsOk())
    {
        // call update() first so that the value to be read are up-to-date
        // Although update() may take a significant amoutn of time this is
        // necessary, because the available amount of physical and virtual 
        // memory is quite volatile.
        RTESys_MemoryInfo::update();
        AddColumn( (SAPDB_UInt8)RTESys_MemoryInfo::GetPhysicalAvailable() );
        AddColumn( (SAPDB_UInt8)RTESys_MemoryInfo::GetVirtualAvailable() );

        SAPDB_UInt8 secondsUser,secondsSystem,secondsIdle;
        RTESys_GetProcessTimes(&secondsUser,&secondsSystem);
        AddColumn( secondsUser );
        AddColumn( secondsSystem );

        RTESys_GetMachineTimes(&secondsUser,&secondsSystem,&secondsIdle);
        AddColumn( secondsUser );
        AddColumn( secondsSystem );
        AddColumn( secondsIdle );

        SAPDB_Char cpuLoadString[12];

        SAPDB_Real8 cpuLoad = RTESys_GetCPULoad();
        if(cpuLoad < 0.0)
            SAPDB_sprintf(cpuLoadString,12,"       ?????");
        else
            SAPDB_sprintf(cpuLoadString,12,"%12f",cpuLoad );
        AddColumn( cpuLoadString );
    }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
