/*!
    @file       IOMan_SingleLogDevice.cpp
    @author     TorstenS
    @ingroup    IOManagement
    @brief      This module implements the single log device which is
                a variant of the pure virtual interface IOMan_LogDevice.
*/
/*
    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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

*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "IOManager/IOMan_Exception.hpp"
#include "IOManager/IOMan_Messages.hpp"
#include "IOManager/IOMan_SingleLogDevice.hpp"
#include "KernelCommon/Kernel_IAdminRestartShutdown.hpp"
#include "RunTime/RTE_Message.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

void
IOMan_SingleLogDevice::ReadPage(
    const tsp00_TaskId         taskId,
    IOMan_IPage               &page,
    const IOMan_BlockAddress   &block )
{
    if( ! m_LogVolume.ReadPage( taskId, page, block ))
    {
        IOMan_Exception errMsg( __CONTEXT__, IOMAN_EMERGENCY_MSG1 );
        RTE_Message( errMsg );

        Kernel_IAdminRestartShutdown::Instance().Offline();
    }
}

/*---------------------------------------------------------------------------*/

void
IOMan_SingleLogDevice::ReadPages(
    const tsp00_TaskId         taskId,
    IOMan_IKernelPages         &pages,
    const IOMan_ClusterAddress &cluster,
    const SAPDB_UInt           startIndex )
{
    if( ! m_LogVolume.ReadPages( taskId, pages, cluster, startIndex ))
    {
        IOMan_Exception errMsg( __CONTEXT__, IOMAN_EMERGENCY_MSG1 );
        RTE_Message( errMsg );

        Kernel_IAdminRestartShutdown::Instance().Offline();
    }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
