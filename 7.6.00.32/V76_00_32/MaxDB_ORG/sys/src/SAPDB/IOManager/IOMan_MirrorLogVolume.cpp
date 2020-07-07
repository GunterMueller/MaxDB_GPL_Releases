/*!
  @file     IOMan_MirrorLogVolume.cpp
  @author   TorstenS
  @ingroup  IOManagement
  @brief    This module is used to handle a mirror log volume.
            Mirrored log volumes are available in log mode dual only.
            Note that both volumes of a log device in log mode dual
            are mirrored log volumes! In contrast to this the class
            IOMan_LogVolume is exclusive available in non dual log modes.
  @see      IOMan_LogVolume, IOMan_SingleLogDevice and IOMan_DualLogDevice

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

#include "IOManager/IOMan_MirrorLogVolume.hpp"
#include "IOManager/IOMan_ReservedBlockAddress.hpp"
#include "IOManager/IOMan_MessageList.hpp"
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
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

void
IOMan_MirrorLogVolume::EnableAfterReIntegration( const tsp00_TaskId  taskId )
{
    SAPDBERR_ASSERT_STATE( IsOnline() );

    m_InfoPage.SetBadBlockNo( 0 );

    WriteInfoPage( taskId );
}

/*---------------------------------------------------------------------------*/

IOMan_ReturnCode
IOMan_MirrorLogVolume::OpenForReIntegration( const tsp00_TaskId taskId )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_MirrorLogVolume::OpenForReIntegration", IOMan_Trace, 6 );

    if( IOMan_Okay != OpenSimple( taskId, IOMAN_SUPPRESS_ERR_MSG, IOMAN_OPEN_FOR_WRITE ))
        return( IOMan_AttachFailed ); // => volume is offline

    const IOMan_BlockNo blockNo = IOMan_ReservedBlockAddress::GetInfoPageBlockNo( m_LogicalDevNo );

    if(! ReadSimple( taskId, tsp00_PageAddr( m_InfoPage.GetFramePtr() ), blockNo ))
    {
        return( IOMan_ReadError ); // => volume is online
    }
    else if( ! m_InfoPage.CheckAfterRead())
    {
        return( IOMan_BadInfoPage ); // => volume is online
    }

    IOMan_MessageList   msgList;
    if( ! IsVolumeConfigurationValid( ! IOMAN_LAZY_CAPACITY_CHECK, msgList ))
    {
        RTE_Message( msgList );
        return( IOMan_ConfigurationCorrupted );
    }
    if( IsBad() )
    {
        return( IOMan_BadVolume ); // => volume is online
    }
    return( IOMan_Okay ); // => volume is online and okay
}

/*---------------------------------------------------------------------------*/

void
IOMan_MirrorLogVolume::PrepareForReIntergration(
    const tsp00_TaskId  taskId,
    const SAPDB_Byte    *pDBIdentifier )
{
    SAPDBERR_ASSERT_STATE( IsOnline() );

    m_InfoPage.InitializeFrame( m_LogicalDevNo, m_Capacity, m_BlockSize, pDBIdentifier );
    m_InfoPage.SetBadBlockNo( 1 );

    WriteInfoPage( taskId );
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
IOMan_MirrorLogVolume::SetSuccessor(
    const tsp00_TaskId      taskId,
    const IOMan_DeviceNo    nextDevNo )
{
    if( ! IsWritable() )
        return( false );

    SAPDBTRACE_WRITELN( IOMan_Trace, 6, "Link source devNo " << m_LogicalDevNo <<
                        " with next destination devNo " << nextDevNo );

    m_InfoPage.SetNextLogicalVolNo( nextDevNo );

    WriteInfoPage( taskId );

    return( true );
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool 
IOMan_MirrorLogVolume::SetPredecessor(
    const tsp00_TaskId      taskId,
    const IOMan_DeviceNo    prevDevNo )
{
    if( ! IsWritable() )
        return( false );

    SAPDBTRACE_WRITELN( IOMan_Trace, 6, "Link source devNo " << m_LogicalDevNo <<
                        " with prev destination devNo " << prevDevNo );

    m_InfoPage.SetPrevLogicalVolNo( prevDevNo );

    WriteInfoPage( taskId );

    return( true );
}


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
