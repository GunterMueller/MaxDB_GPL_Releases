/*!
  @file     IOMan_LogVolume.cpp
  @author   TorstenS
  @ingroup  IOManagement
  @brief    This module is used to handle a log volume.

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

#include "heo56.h"   // RTE   : Vsleep

#include "IOManager/IOMan_LogVolume.hpp"
#include "KernelCommon/Kernel_FileIO.hpp"
#include "KernelCommon/Kernel_IAdminRestartShutdown.hpp"
#include "IOManager/IOMan_Exception.hpp"
#include "IOManager/IOMan_Messages.hpp"
#include "RunTime/RTE_Crash.hpp"
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



/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

IOMan_ReturnCode
IOMan_LogVolume::Open(
    const tsp00_TaskId      taskId,
    const IOMan_BlockCount  logCapacity,
    const bool              bOpenForWrite )
{
    const IOMan_ReturnCode retCode =
        IOMan_Volume::Open( taskId, true, bOpenForWrite );

    if( IOMan_Okay == retCode )
    {
        m_FirstOffset = logCapacity;
        m_LastOffset  = m_FirstOffset +  GetUsableSize() -1;
    }
    return( retCode );
}

/*---------------------------------------------------------------------------*/


bool
IOMan_LogVolume::ReadPages(
    const tsp00_TaskId          taskId,
    IOMan_IKernelPages&         pages,
    const IOMan_ClusterAddress& cluster,
    const SAPDB_UInt            startIndex )
{
    SAPDBERR_ASSERT_STATE( cluster.GetDeviceNo() == GetLogicalDevNo());

    IOMan_IKernelPagesIterator          iter( pages );
    IOMan_ClusterAddress::BlockIndex    index = 0;

    #   ifdef SAPDB_FAST
        const bool bDoCheck = IOMan_Check.ChecksLevel( 5 );
    #   else
        const bool bDoCheck = true;
    #   endif
    if( bDoCheck ) // TODO
    {
        // Danger, because it's assumed that the given page frames are
        // in strictly ascending order within the memory!
        const SAPDB_UInt            nextIndex = IOMan_IKernelPages::FirstIndex()+ 1;
        IOMan_IKernelPagesIterator  prevIter( pages );

        for( prevIter.Begin(), iter.Begin( nextIndex ); !iter.End(); ++prevIter, ++iter )
        {
            if( ( *prevIter ).GetFramePtr() + m_BlockSize != ( *iter ).GetFramePtr() )
            {
                RTE_Crash( SAPDBErr_Exception(__FILE__, __LINE__,
                                              SAPDBERR_ASSERT_STATE_FAILED,
                                              "IOMan_LogVolume::ReadPages") );
            }
        }
    }

    for( SAPDB_UInt readCount = 1; IOMan_Volume::MaxReadRetries() >= readCount; ++readCount )
    {
        if(! ReadSimple( taskId, *( pages.GetFrameList( startIndex )),
                         cluster.GetBlockNo( 0 ),
                         cluster.GetBlockCount() ))
        {
            IOMan_Exception errMsg( __CONTEXT__, IOMAN_EMERGENCY_MSG1 );
            RTE_Message( errMsg );

            Kernel_IAdminRestartShutdown::Instance().Offline();
        }

        for( index = 0, iter.Begin( startIndex ); index < cluster.GetBlockCount(); ++iter, ++index )
        {
            if( ( *iter ).CheckAfterRead()){
                continue;
            }
            IOMan_Exception errMsg( __CONTEXT__, IOMAN_BAD_PAGE,
                                      GetTypeName(), SAPDB_ToString( GetLogicalDevNo(), _T_d ),
                                      SAPDB_ToString( cluster.GetBlockNo( index ), _T_d ));

            Kernel_FileIO   dumpFile( GetTypeName(), KERNEL_BAD_FILE_EXTENSION,
                                      SAPDB_ToString( GetLogicalDevNo(), _T_d ),
                                      SAPDB_ToString( cluster.GetBlockNo( index ), _T_d ));
            dumpFile.Write( *iter );
            dumpFile.Close();

            RTE_Message( errMsg );

            vsleep( taskId, IOMan_Volume::RetrySleepTime());
            break;
        }
        if( index >= cluster.GetBlockCount()){
            return true;     // usual case
        }
        //else read the page collection again, because an error has occured
    }
    return false;
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
