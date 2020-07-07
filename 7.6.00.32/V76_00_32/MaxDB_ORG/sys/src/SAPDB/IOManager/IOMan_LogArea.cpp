/*!
  @file           IOMan_LogArea.cpp
  @author         TorstenS
  @ingroup        IOManagement
  @brief          This module is used to manage the log area.

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


#include "IOManager/IOMan_LogArea.hpp"
#include "IOManager/IOMan_MessageList.hpp"
#include "IOManager/IOMan_Messages.hpp"
#include "IOManager/IOMan_ReservedBlockAddress.hpp"

#include "Logging/Log_Page.hpp"

#include "RunTime/RTE_Crash.hpp"
#include "RunTime/RTE_Message.hpp"
#include "RunTime/InputOutput/RTEIO_VolumeInfo.hpp"

#include "SAPDBCommon/SAPDB_ToString.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"


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

SAPDB_Bool
IOMan_LogArea::AddVolume(
    const tsp00_TaskId          taskId,
    tsp00_VFilename             &devName,
    tsp00_VFilename             &mirrDevName,
    const IOMan_BlockCount      configuredSize,
    const IOMan_DeviceNo        devNo,
    IOMan_LogPages              &pages )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_LogArea::AddLogVolume", IOMan_Trace, 6 );

    SAPDBTRACE_WRITELN( IOMan_Trace, 6, "Volume: " << devName <<
                        " DevSize: " << configuredSize << " DevNo: " << devNo );

    if( ! m_LogDevices.IsDeviceNoValid( devNo ))
    {
        IOMan_MessageList errMsg( __CONTEXT__, IOMAN_ERR_LOG_DEVNO_OUT_OF_RANGE,
                                  SAPDB_ToString( devNo, _T_d ),
                                  SAPDB_ToString( m_LogDevices.MaxDevices(), _T_d ));
        RTE_Message( errMsg );
        return SAPDB_FALSE;
    }

    IOMan_ILogDevice  &currDevice  = Get( devNo );
    IOMan_ILogDevice  *pPrevDevice = NULL;
    IOMan_ILogDevice  *pNextDevice = NULL;

    // Add log device to the control structures of the RTE, because at the
    // time of kernel restart this device was not stored within data base
    // configuration file. The device was made persistent by the dbmsrv after
    // starting the db kernel, therefore vcold couldn't find the device!

    if(! currDevice.Add( devName, mirrDevName, configuredSize ))
        return SAPDB_FALSE;

    const SAPDB_Byte *pDBIdentifier = DetermineLinkage( currDevice, &pPrevDevice, &pNextDevice );

    if( NULL == pDBIdentifier )
        return SAPDB_FALSE;

    if( IOMan_Okay == currDevice.Create( taskId, pDBIdentifier ))
    {
        if( IOMan_Okay == currDevice.Open( taskId, m_TotalLogPages, SAPDB_TRUE ))
        {
            WriteLogPages( taskId, currDevice, pages );

            if( NULL != pPrevDevice )
                CreateLinkage( taskId, *pPrevDevice, currDevice );

            if( NULL != pNextDevice )
                CreateLinkage( taskId, currDevice, *pNextDevice );

            m_TotalLogPages += currDevice.GetUsableSize();
            m_UsedDevices++;
            m_ConfiguredDevices++;
            {
                IOMan_MessageList errMsg( __CONTEXT__, IOMAN_INFO_ADD_VOLUME,
                                          currDevice.GetTypeName(),
                                          SAPDB_ToString( currDevice.GetLogicalDevNo(), _T_d ),
                                          SAPDB_ToString( currDevice.GetConfiguredSize(), _T_d ));
                RTE_Message( errMsg );
            }
            return SAPDB_TRUE;
        }
    }
    currDevice.Close( taskId );
    currDevice.Del();
    return SAPDB_FALSE;
}

/*---------------------------------------------------------------------------*/

void
IOMan_LogArea::CloseAll( const tsp00_TaskId taskId )
{
    IOMan_LogDeviceIterator iter( m_LogDevices );

    for( iter.Begin(); !iter.End(); ++iter )
        ( *iter ).Close( taskId );

    m_TotalLogPages = 0;
    m_UsedDevices   = 0;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
IOMan_LogArea::CreateAll(
    const tsp00_TaskId taskId,
    const SAPDB_Byte   *pDBIdentifier )
{
    IOMan_MessageList msgList;

    if( 0 != m_TotalLogPages )
    {
        IOMan_MessageList errMsg( __CONTEXT__, IOMAN_ERR_LOG_AREA_ALREADY_ONLINE );
        RTE_Crash( errMsg );
    }

    if( ! FormatAll( taskId, msgList ))
    {
        msgList.AppendNewMessage( IOMan_MessageList( __CONTEXT__, IOMAN_ERR_ALL_FORMAT, "log" ));
        RTE_Message( msgList );
        return SAPDB_FALSE;
    }

    const SAPDB_Bool bOpenForWrite = SAPDB_TRUE;

    IOMan_LogDeviceIterator iter( m_LogDevices );
    IOMan_LogDeviceIterator prevIter( m_LogDevices );

    for( iter.Begin(), prevIter.End(); !iter.End(); ++iter )
    {
        const IOMan_ReturnCode retCode = ( *iter ).CreateInfoPage( taskId, pDBIdentifier );

        if( IOMan_Okay == retCode )
        {
            if( IOMan_Okay != ((*iter ).Open( taskId, m_TotalLogPages, bOpenForWrite )))
                return SAPDB_FALSE;
            else
            {
                if( ! prevIter.End() )
                    CreateLinkage( taskId, *prevIter, *iter );

                prevIter         = iter;
                m_TotalLogPages += ( *iter ).GetUsableSize();
                m_UsedDevices ++;
            }
        }
        else if( IOMan_NotConfigured == retCode )
            continue;
        else
            return SAPDB_FALSE;
    }
    SAPDBERR_ASSERT_STATE( m_ConfiguredDevices == m_UsedDevices );
    SAPDBERR_ASSERT_RANGE( m_UsedDevices, SAPDB_UInt( IOMan_LogDevices::FirstDeviceNo()),
                           m_LogDevices.MaxDevices() );

    return SAPDB_TRUE;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
IOMan_LogArea::OpenAll(
    const tsp00_TaskId taskId,
    const SAPDB_Bool   bOpenForWrite )
{

    IOMan_BlockCount        totalLogPages = 0;
    SAPDB_UInt              usedDevices   = 0;
    IOMan_LogDeviceIterator iter( m_LogDevices );
    IOMan_LogDeviceIterator prevIter( m_LogDevices );

    for( iter.Begin(), prevIter.End(); !iter.End(); ++iter )
    {
        const IOMan_ReturnCode retCode = ( *iter ).Open( taskId, totalLogPages, bOpenForWrite );

        if( IOMan_Okay == retCode )
        {
            if( prevIter.End() )
            {
                // iter points to the first device within configuration file
                if( ( *iter ).IsPrevLinkageValid())
                {
                    IOMan_MessageList errMsg( __CONTEXT__, IOMAN_ERR_MISSING_PREDECESSOR,
                                              ( *iter ).GetTypeName(),
                                              SAPDB_ToString( ( *iter ).GetLogicalDevNo(), _T_d ),
                                              SAPDB_ToString( ( *iter ).GetPrevLogicalDevNo(), _T_d ));
                    RTE_Message( errMsg );
                    return SAPDB_FALSE;
                }
            }
            else
            {
                if( !( *prevIter ).IsLinkageValid( taskId, *iter ))
                    return SAPDB_FALSE;

                if( !( *prevIter ).IsDBIdentifierValid( taskId, *iter ))
                    return SAPDB_FALSE;
            }
            prevIter       = iter;
            totalLogPages += ( *iter ).GetUsableSize();
            usedDevices++;
        }
        else if( IOMan_NotConfigured == retCode )
            continue;
        else
            return SAPDB_FALSE;
    }
    if( ( *prevIter ).IsNextLinkageValid())
    {
        IOMan_MessageList errMsg( __CONTEXT__, IOMAN_ERR_MISSING_SUCCESSOR,
                                  ( *prevIter ).GetTypeName(),
                                  SAPDB_ToString( ( *prevIter ).GetLogicalDevNo(), _T_d ),
                                  SAPDB_ToString( ( *prevIter ).GetNextLogicalDevNo(), _T_d ));
        RTE_Message( errMsg );

        return SAPDB_FALSE;
    }

    SAPDBERR_ASSERT_STATE( m_ConfiguredDevices == usedDevices );
    SAPDBERR_ASSERT_RANGE( usedDevices, SAPDB_UInt( IOMan_LogDevices::FirstDeviceNo()),
                           m_LogDevices.MaxDevices() );

    // Set the log area capacity after opening all devices.
    m_TotalLogPages = totalLogPages;
    m_UsedDevices   = usedDevices;

    return SAPDB_TRUE;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
IOMan_LogArea::GetVolumeNo(
    tsp00_VFilename &devName,
    IOMan_DeviceNo  &devNo,
    SAPDB_Bool      &bIsPrimary)
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_LogArea::GetVolumeNo", IOMan_Trace, 6 );

    tsp00_VFilename         auxName;
    IOMan_LogDeviceIterator iter( m_LogDevices );

    bIsPrimary = SAPDB_TRUE;

    for( iter.Begin(); !iter.End(); ++iter )
    {
        if(( *iter ).GetName( auxName ))
        {
            if( 0 == memcmp( devName, auxName, sizeof( devName )))
            {
                devNo = (*iter).GetLogicalDevNo();
                return SAPDB_TRUE;
            }
        }
        if( SAPDB_TRUE == m_bIsLogMirrored )
        {
            if(( *iter ).GetName( auxName, m_bIsLogMirrored ))
            {
                if( 0 == memcmp( devName, auxName, sizeof( devName )))
                {
                    devNo = (*iter).GetLogicalDevNo();
                    bIsPrimary = SAPDB_FALSE;
                    return( SAPDB_TRUE );
                }
            }
        }
    }
    return SAPDB_FALSE;
}
/*---------------------------------------------------------------------------*/

IOMan_ReturnCode
IOMan_LogArea::MigrateCapacity(
    const tsp00_TaskId  taskId,
    IOMan_MessageList   &msgList )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_LogArea::MigrateCapacity", IOMan_Trace, 6 );

    IOMan_LogDeviceIterator iter( m_LogDevices );

    for( iter.Begin(); ! iter.End(); ++iter )
    {
        const IOMan_ReturnCode retCode = ( *iter ).MigrateCapacity( taskId, msgList );

        if(( IOMan_Okay == retCode ) || ( IOMan_NotConfigured == retCode ))
            continue;
        else
            return retCode;
    }
    return IOMan_Okay;
}

/*---------------------------------------------------------------------------*/

bool
IOMan_LogArea::Initialize(
    const tsp00_TaskId  taskId,
    const SAPDB_Int     maxLogDevices)
{
    if( ! m_LogDevices.Initialize( taskId, maxLogDevices )){
        return false;
    }
    IOMan_VolumeNo  devNo  = IOMan_LogDevices::FirstDeviceNo();
    IOMan_LogDeviceIterator iter( m_LogDevices );

    for( iter.Begin(); !iter.End(); ++iter )
    {
        Data_PageFrame mirrFrame;
        Data_PageFrame frame = m_PageAllocator.New( taskId );

        if( ! frame.IsAssigned()){
            return false;
        }
        if( m_bIsLogMirrored )
        {
            mirrFrame = m_PageAllocator.New( taskId );

            if( ! mirrFrame.IsAssigned()){
                return false;
            }
        }
        ( *iter ).Initialize( devNo, frame, mirrFrame );

        // Determine the number of configured devices at this point
        // because some compontents are interessted in this information
        // before the corresponding devices are started.
        if( ( *iter ).IsConfigured()){
            ++m_ConfiguredDevices;
        }
        ++devNo;
    }
    m_LastAccessedDevice = IOMan_LogDevices::FirstDeviceNo();
    return true;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
IOMan_LogArea::GetBadLogVolume(
    const tsp00_TaskId  taskId,
    tsp00_VFilename     &badDevName )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_LogArea::GetBadLogVolume", IOMan_Trace, 6 );

    SAPDBERR_ASSERT_STATE( m_bIsLogMirrored );

    if( ! m_bIsLogMirrored )
        return SAPDB_FALSE;

    IOMan_LogDeviceIterator iter( m_LogDevices );

    for( iter.Begin(); !iter.End(); ++iter )
    {
        IOMan_DualLogDevice &dualDevice = iter.GetDualDevice();

        if( dualDevice.GetBadLogVolume( taskId, badDevName ))
            return SAPDB_TRUE;
    }
    return SAPDB_FALSE;
}

/*---------------------------------------------------------------------------*/

IOMan_BlockAddress
IOMan_LogArea::GetBlockAddress( const Log_NonCyclicRawDeviceOffset offset )
{

    if( offset > SAPDB_UInt4( m_TotalLogPages ))
    {
        IOMan_MessageList errMsg( __CONTEXT__, IOMAN_ERR_LOG_OFFSET_OUT_OF_RANGE,
                                  SAPDB_ToString( offset, _T_d ),
                                  SAPDB_ToString( m_TotalLogPages, _T_d ));
        RTE_Crash( errMsg );
    }

    SAPDBERR_ASSERT_STATE( SAPDB_UInt4( m_LastAccessedDevice ) <= m_UsedDevices );

    IOMan_DeviceNo      currentAccessedDevice = m_LastAccessedDevice;
    IOMan_BlockAddress  block;
    IOMan_BlockNo       blockNo;

    // Determine suitable log device
    if( offset >= Get( currentAccessedDevice ).FirstOffset() )
    {
        while( offset > Get( currentAccessedDevice ).LastOffset() )
            ++currentAccessedDevice;
    }
    else
    {
        while( offset < Get( currentAccessedDevice ).FirstOffset() )
            --currentAccessedDevice;
    }
    // Determine offset within log device
    blockNo = offset - Get( currentAccessedDevice ).FirstOffset() + 1; // +1 because of the IOMan_InfoPage
    block.SetAddress( currentAccessedDevice, blockNo );

    SAPDBERR_ASSERT_STATE( SAPDB_UInt4( currentAccessedDevice ) <= m_UsedDevices );
    SAPDBERR_ASSERT_STATE( blockNo < Get( currentAccessedDevice ).GetInternalSize());

    m_LastAccessedDevice = currentAccessedDevice;
    return block;
}

/*---------------------------------------------------------------------------*/

IOMan_ClusterAddress
IOMan_LogArea::GetClusterAddress(
    const Log_NonCyclicRawDeviceOffset   offset,
    IOMan_BlockCount               blockCount )
{
    if( offset + blockCount - 1 > SAPDB_UInt4( m_TotalLogPages ))
    {
        IOMan_MessageList errMsg( __CONTEXT__, IOMAN_ERR_LOG_OFFSET_OUT_OF_RANGE_1,
                                  SAPDB_ToString( offset, _T_d ),
                                  SAPDB_ToString( blockCount, _T_d ),
                                  SAPDB_ToString( m_TotalLogPages, _T_d ));
        RTE_Crash( errMsg );
    }

    IOMan_BlockAddress block = GetBlockAddress( offset );

    if( block.GetBlockNo() + blockCount - 1
            >
            Get( block.GetDeviceNo() ).GetUsableSize() ) // PTS 1110785 TS 2001-07-04
    {
        blockCount = Get( block.GetDeviceNo() ).GetUsableSize() - // PTS 1110785 TS 2001-07-04
                     block.GetBlockNo() + 1;
    }
    IOMan_ClusterAddress cluster( block, blockCount );
    return cluster;
}
/*---------------------------------------------------------------------------*/

SAPDB_Bool
IOMan_LogArea::ReIntegrateBadLogVolume(
    const tsp00_TaskId  taskId,
    tsp00_VFilename     &badDevName )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_LogArea::ReIntegrateBadLogVolume", IOMan_Trace, 6 );

    SAPDBERR_ASSERT_STATE( m_bIsLogMirrored );

    if( ! m_bIsLogMirrored )
        return SAPDB_FALSE;

    IOMan_DeviceNo  devNo;
    SAPDB_Bool      bDummyPrimary;

    if( ! GetVolumeNo( badDevName, devNo, bDummyPrimary ))
    {
        SAPDB_Char volName[ sizeof( tsp00_VFilename ) ];
        SAPDB_MemCopyNoCheck( volName, badDevName, sizeof( tsp00_VFilename ));

        SAPDB_Int i = sizeof( tsp00_VFilename ) -1;

        while( ' ' ==  volName[ i ] && i > 0 )
            i -= 1;

        if( i < ( sizeof( tsp00_VFilename ) -2 ))
            volName[ i + 1] = '\0';

        IOMan_MessageList errMsg( __CONTEXT__, IOMAN_INFO_BAD_VOLUME_NOT_FOUND, volName );
        RTE_Message( errMsg );

        return SAPDB_FALSE;
    }

    IOMan_DualLogDevice &dualDevice = m_LogDevices.GetDualDevice( devNo );

    return dualDevice.ReIntegrateBadLogVolume( taskId, badDevName );
}

/*===========================================================================*
 *  PRIVATE METHODS                                                          *
 *===========================================================================*/

SAPDB_Bool
IOMan_LogArea::FormatAll(
    const tsp00_TaskId  taskId,
    IOMan_MessageList     &msgList )
{

    Data_PageFrame frame = m_PageAllocator.New( taskId );

    if(! frame.IsAssigned()){
        return SAPDB_FALSE;
    }

    Log_Page dummyLogPage( frame, Log_QueueID(), Log_IOSequenceNo());

    RTEIO_VolumeInfoVector infoVector( m_Allocator );

    // double number of volumes if we have mirrored logs
    if( m_bIsLogMirrored )
    {
        if( ! infoVector.Reserve( m_ConfiguredDevices * 2 )){
            return SAPDB_FALSE;
        }
    }
    else
    {
        if( ! infoVector.Reserve( m_ConfiguredDevices )){
            return SAPDB_FALSE;
        }
    }

    IOMan_LogDeviceIterator iter( m_LogDevices );

    // fill vector with volume info
    for( iter.Begin(); ! iter.End(); ++iter )
    {
        if ((*iter).IsConfigured())
        {
            infoVector.InsertEnd ( new ( m_Allocator )
                                   RTEIO_LogVolumeInfo((*iter).GetLogicalDevNo(),
                                                       (*iter).GetInternalSize(),
                                                       dummyLogPage.GetFramePtr()));
            if( m_bIsLogMirrored )
            {
                infoVector.InsertEnd ( new ( m_Allocator )
                                       RTEIO_MirrorLogVolumeInfo((*iter).GetLogicalDevNo(),
                                                                 (*iter).GetInternalSize(),
                                                                 dummyLogPage.GetFramePtr()));
            }
        }
    }

    // format all volumes
    if ( ! RTEIO_FormatVolumeVector( taskId, infoVector, msgList) )
    {
        return SAPDB_FALSE;
    }

    return SAPDB_TRUE;
}

/*---------------------------------------------------------------------------*/

void
IOMan_LogArea::CreateLinkage(
    const tsp00_TaskId  taskId,
    IOMan_ILogDevice    &prevDevice,
    IOMan_ILogDevice    &currDevice )
{
    if( prevDevice.SetNextLinkage( taskId, currDevice ))
        if( currDevice.SetPrevLinkage( taskId, prevDevice ))
            return;

    IOMan_MessageList errMsg( __CONTEXT__, IOMAN_ERR_CREATE_LINKAGE,
                              currDevice.GetTypeName(),
                              SAPDB_ToString( prevDevice.GetLogicalDevNo(), _T_d ),
                              SAPDB_ToString( currDevice.GetLogicalDevNo(), _T_d ));
    RTE_Crash( errMsg );
}

/*---------------------------------------------------------------------------*/

const SAPDB_Byte*
IOMan_LogArea::DetermineLinkage(
    const IOMan_ILogDevice  &currDevice,
    IOMan_ILogDevice        **pPrevDevice,
    IOMan_ILogDevice        **pNextDevice )
{
    IOMan_LogDeviceIterator iter( m_LogDevices );

    for( iter.Begin(); !iter.End(); ++iter )
    {
        if(( *iter ).IsOnline())
        {
            if(( *iter ).GetLogicalDevNo() < currDevice.GetLogicalDevNo())
                *pPrevDevice = &( *iter );
            else
                if(( *iter ).GetLogicalDevNo() > currDevice.GetLogicalDevNo())
                {
                    *pNextDevice = &( *iter );
                    break;
                }
        }
    }
    if( NULL != *pPrevDevice )
        return( **pPrevDevice ).GetDBIdentifier();
    else if( NULL != *pNextDevice )
        return( **pNextDevice ).GetDBIdentifier();
    else
        return NULL;
}

/*---------------------------------------------------------------------------*/

void
IOMan_LogArea::WriteLogPages(
    const tsp00_TaskId  taskId,
    IOMan_ILogDevice    &device,
    IOMan_LogPages      &pages )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_LogArea::WriteLogPages", IOMan_Trace, 6 );

    IOMan_BlockNo blockNo =
        IOMan_ReservedBlockAddress::GetInfoPageBlockNo( device.GetLogicalDevNo()) + 1;

    const IOMan_BlockCount  capacity   = device.GetUsableSize();
    const IOMan_BlockCount  blockCount = pages.GetPageCount();
    const IOMan_DeviceNo    devNo      = device.GetLogicalDevNo();

    IOMan_BlockCount        pagesToWrite;
    IOMan_BlockAddress      block;
    IOMan_ClusterAddress    cluster;

    while( blockNo <= capacity )
    {
        pagesToWrite =
            ( blockNo + blockCount - 1 <= capacity ) ? blockCount : capacity - blockNo + 1;

        block.SetAddress( devNo, blockNo );
        cluster.SetCluster( block, pagesToWrite );

        device.WritePages( taskId, pages, cluster );

        blockNo += pagesToWrite;
    }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
