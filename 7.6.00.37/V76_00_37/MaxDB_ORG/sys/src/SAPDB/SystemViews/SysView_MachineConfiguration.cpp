/****************************************************************************/
/*!

  @file        SysView_MachineConfiguration.cpp

  -------------------------------------------------------------------------

  @author      RobinW

  @ingroup     SystemViews
  @brief       This module implements the "MachineConfiguration" view class.

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
#include "SystemViews/SysView_MachineConfiguration.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "RunTime/System/RTESys_SystemInfo.h"
#include "RunTime/System/RTESys_MemoryInfo.hpp"

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

void SysView_MachineConfiguration::Create()
{
	m_Table->AppendCol (ITOCT_FIXED,  SV_CPUNUMBER, 5);
	m_Table->AppendCol (ITOCT_FIXED,  SV_PHYSICALCPUNUMBER, 5);
	m_Table->AppendCol (ITOCT_FIXED,  SV_PHYSICALMEMORYSIZE, 10);
	m_Table->AppendCol (ITOCT_FIXED,  SV_VIRTUALMEMORYSIZE, 10);
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_MachineConfiguration::GetColCount()
{
	return SV_CC_MACHINECONFIGURATION;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_MachineConfiguration::EstimateRows()
{
	return SV_ER_MACHINECONFIGURATION;
}

/*---------------------------------------------------------------------------*/

void SysView_MachineConfiguration::Execute()
{
    m_Table->GetCatalogTable();
    if (m_Context.IsOk())
    {
		SAPDB_Int4 NumberOfCPUs = RTESys_NumberOfCPU();
		SAPDB_Int4 NumberOfPhysicalCPUs = RTESys_NumberOfPhysicalCPUs();
		AddColumn( NumberOfCPUs );
		if( NumberOfCPUs == NumberOfPhysicalCPUs )
        	AddColumn();
		else
			AddColumn( NumberOfPhysicalCPUs );

        // the RTESys_MemoryInfo methods are called without calling update() before,
        // because update() would take a significant amount of time.
        // That means that the values are possibly not up-to-date.
        // Anyway, physical and virtual memory are not likely to change often..
        AddColumn( (SAPDB_UInt8)RTESys_MemoryInfo::GetPhysicalInstalled() );
        AddColumn( (SAPDB_UInt8)RTESys_MemoryInfo::GetVirtualInstalled() );
    }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
