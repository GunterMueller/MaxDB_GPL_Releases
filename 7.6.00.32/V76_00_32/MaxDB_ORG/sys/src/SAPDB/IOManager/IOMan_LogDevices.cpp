/*!
  @file     IOMan_LogDevices.cpp
  @author   TorstenS
  @ingroup  IOManagement
  @brief    This module is used to handle a collecton of log devices
            in particular single and dual logs.

\if EMIT_LICENCE

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



\endif
*/




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "IOManager/IOMan_LogDevices.hpp"
#include "IOManager/IOMan_SingleLogDevice.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"

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
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

SAPDB_Bool
IOMan_LogDevices::Initialize(
    const tsp00_TaskId  taskId,
    const SAPDB_Int     maxLogVolumes )
{
    // Create anchor list for single or dual log devices
    if(! m_LogDevices.Resize( maxLogVolumes ))
        return( SAPDB_FALSE );

    // Link proper devices with anchor list
    if( m_bIsLogMirrored )
    {
        const SAPDB_UInt4 wantedSpace = maxLogVolumes * sizeof( IOMan_DualLogDevice );

        // Allocate memory for the dual log device handler
        IOMan_DualLogDevice *pDualDevices = reinterpret_cast< IOMan_DualLogDevice* >
                ( m_LogDevices.GetAllocator().Allocate( wantedSpace ));

        if( NULL == pDualDevices )
            return( SAPDB_FALSE );

        // Initialize dual log device handler with default constructor via placement new
        newarray( pDualDevices, maxLogVolumes, pDualDevices );

        // Link dual log device handler with anchor vector 
        for( Iterator iter = m_LogDevices.Begin(); iter != m_LogDevices.End();
                ++iter, ++pDualDevices )
        {
            *iter = pDualDevices;
        }
    }
    else
    {
        const SAPDB_UInt4 wantedSpace = maxLogVolumes * sizeof( IOMan_SingleLogDevice );

        // Allocate memory for the single log device handler
        IOMan_SingleLogDevice *pSingleDevices = reinterpret_cast< IOMan_SingleLogDevice* >
                ( m_LogDevices.GetAllocator().Allocate( wantedSpace ));

        if( NULL == pSingleDevices )
            return( SAPDB_FALSE );

        // Initialize single log device handler with default constructor via placement new
        newarray( pSingleDevices, maxLogVolumes, pSingleDevices );

         // Link single log device handler with anchor vector 
        for( Iterator iter = m_LogDevices.Begin(); iter != m_LogDevices.End();
                ++iter, ++pSingleDevices )
        {
            *iter = pSingleDevices;
        }
    }
    return( SAPDB_TRUE);
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
