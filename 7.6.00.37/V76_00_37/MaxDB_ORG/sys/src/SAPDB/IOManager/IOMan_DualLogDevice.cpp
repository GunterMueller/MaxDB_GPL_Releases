/*!
  @file     IOMan_DualLogDevice.cpp
  @author   TorstenS
  @ingroup  IOManagement
  @brief    This module implements the dual log device which is
            a variant of the pure virtual interface IOMan_LogDevice.

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

#include "vsp002.h"  // PASCAL: IO Types
#include "heo54k.h"  // RTE   : Vattach/Vdetach/Vblockio/Vvectorio

#include "IOManager/IOMan_Exception.hpp"
#include "IOManager/IOMan_Messages.hpp"
#include "IOManager/IOMan_DualLogDevice.hpp"
#include "KernelCommon/Kernel_FileIO.hpp"
#include "KernelCommon/Kernel_IAdminRestartShutdown.hpp"

#include "RunTime/RTE_Crash.hpp"
#include "RunTime/RTE_Message.hpp"
#include "SAPDBCommon/SAPDB_ToString.hpp"

#include "SAPDBCommon/Messages/SDBMsg_IOMan.h"


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

IOMan_DualLogDevice::IOMan_DualLogDevice()
        :
        m_PrimaryLogVolume( SAPDB_FALSE ),   // => primary volume
        m_SecondaryLogVolume( SAPDB_TRUE ) // => secondary volume
{};

/*---------------------------------------------------------------------------*/

IOMan_ReturnCode
IOMan_DualLogDevice::Create(
    const tsp00_TaskId  taskId,
    const SAPDB_Byte    *pDBIdentifier )
{
    IOMan_ReturnCode retCode = m_PrimaryLogVolume.Create( taskId, pDBIdentifier );

    if( IOMan_Okay != retCode )
        return( retCode );

    retCode = m_SecondaryLogVolume.Create( taskId, pDBIdentifier );

    if( IOMan_Okay != retCode )
        return( retCode );

    if( m_PrimaryLogVolume.GetConfiguredSize() != m_SecondaryLogVolume.GetConfiguredSize() )
    {
        IOMan_Exception errMsg( __CONTEXT__, IOMAN_ERR_CAPACITY_MISMATCH,
                                SAPDB_ToString( m_PrimaryLogVolume.GetLogicalDevNo(), _T_d ),
                                SAPDB_ToString( m_PrimaryLogVolume.GetConfiguredSize(), _T_d ),
                                SAPDB_ToString( m_SecondaryLogVolume.GetConfiguredSize(), _T_d ));
        RTE_Message( errMsg );

        return( IOMan_VolumeCapacityMismatch );
    }
    return( IOMan_Okay );
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
IOMan_DualLogDevice::GetBadLogVolume(
    const tsp00_TaskId  taskId,
    tsp00_VFilename     &badDevName )
{
    SAPDB_Bool  bFoundBadVolume = SAPDB_TRUE;

    SAPDBERR_ASSERT_STATE( m_PrimaryLogVolume.IsConfigured() == m_SecondaryLogVolume.IsConfigured() );

    if( ! m_PrimaryLogVolume.IsConfigured() )
        return( SAPDB_FALSE );

    if( IOMan_Okay != m_PrimaryLogVolume.OpenForReIntegration( taskId ))
    {
        m_PrimaryLogVolume.GetName( badDevName );
    }
    else if( IOMan_Okay != m_SecondaryLogVolume.OpenForReIntegration( taskId ))
    {
        m_SecondaryLogVolume.GetName( badDevName );
    }
    else
        bFoundBadVolume = SAPDB_FALSE;

    Close( taskId );
    return( bFoundBadVolume );
}

/*---------------------------------------------------------------------------*/

IOMan_ReturnCode
IOMan_DualLogDevice::Open(
    const tsp00_TaskId      taskId,
    const IOMan_BlockCount  logCapacity,
    const SAPDB_Bool        bOpenForWrite )
{
    IOMan_ReturnCode retCodePrimary   = m_PrimaryLogVolume  .Open( taskId, logCapacity, bOpenForWrite );
    IOMan_ReturnCode retCodeSecondary = m_SecondaryLogVolume.Open( taskId, logCapacity, bOpenForWrite );
    IOMan_ReturnCode retCode = IOMan_Okay;

    if(( IOMan_Okay == retCodePrimary ) && ( IOMan_Okay == retCodeSecondary )) // both are OK?
    {
        retCode = IOMan_Okay;
    }
    else
    {
        // primary is not configured but secondary is -> bad config
        if((( IOMan_NotConfigured == retCodePrimary ) && ( IOMan_NotConfigured != retCodeSecondary )) ||
          (( IOMan_NotConfigured != retCodePrimary ) && ( IOMan_NotConfigured == retCodeSecondary )))
        {

            SAPDB_Int4 volId = m_PrimaryLogVolume.GetLogicalDevNo();

            RTE_Message(Msg_List(
                Msg_List::Error,
                SDBMSG_IOMAN_LOGDEVICE_CONFIGURATION_CORRUPTED,
                Msg_Arg(SDBMSGTAG_IOMAN_LOGDEVICE_CONFIGURATION_CORRUPTED__VOLUME_ID,volId)));

            retCode = IOMan_ConfigurationCorrupted;
        }
        else 
        {
            // return something != IOMan_Okay 
            retCode = (IOMan_Okay == retCodePrimary) ? retCodeSecondary : retCodePrimary;
        }
    }

    if (retCode == IOMan_Okay)
    {
        if( m_PrimaryLogVolume.IsDBIdentifierValid( taskId, m_SecondaryLogVolume ))
        {
            return IOMan_Okay;
        }
        else
        {
            return IOMan_ConfigurationCorrupted;
        }
    }
    else
    {
        return retCode;
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
IOMan_DualLogDevice::ReIntegrateBadLogVolume(
    const tsp00_TaskId  taskId,
    tsp00_VFilename     &badDevName )
{
    const IOMan_BlockCount  logCapacity = 0; // current log capacity
    tsp00_VFilename         srcName;
    tsp00_VFilename         destName;

    m_PrimaryLogVolume.GetName( srcName );

    const SAPDB_Bool bSecondaryVolumeIsBad =
        ( 0 == memcmp( badDevName, srcName, sizeof( badDevName ))) ? SAPDB_FALSE : SAPDB_TRUE;

    IOMan_MirrorLogVolume &srcVolume  = bSecondaryVolumeIsBad ? m_PrimaryLogVolume : m_SecondaryLogVolume;
    IOMan_MirrorLogVolume &destVolume = bSecondaryVolumeIsBad ? m_SecondaryLogVolume : m_PrimaryLogVolume;

    if( IOMan_Okay != srcVolume.Open( taskId, logCapacity, IOMAN_OPEN_FOR_WRITE ))
        return( SAPDB_FALSE ); // source volume has a problem

    switch( destVolume.OpenForReIntegration( taskId ) )
    {
    case IOMan_Okay : // => bad volume isn't bad
        {
            destVolume.GetNameWithTermination( destName );
            destVolume.Close( taskId );
            srcVolume.Close( taskId );

            IOMan_Exception errMsg( __CONTEXT__, IOMAN_INFO_VOLUME_NOT_BAD, destName );
            RTE_Message( errMsg );

            return( SAPDB_TRUE );
        }
    case IOMan_AttachFailed: // volume is not accessable => no SAP DB Device available
        {
            if( IOMan_Okay == destVolume.Create( taskId, srcVolume.GetDBIdentifier()))
            {
                if( IOMan_Okay == destVolume.Open( taskId, logCapacity, IOMAN_OPEN_FOR_WRITE ))
                    break;
            }
            srcVolume.Close( taskId );
            return( SAPDB_FALSE ); // destination volume has still a problem
        }
    case IOMan_BadInfoPage:
    case IOMan_ConfigurationCorrupted:
    case IOMan_BadVolume:
        break; // we can manage these errors
    case IOMan_ReadError: // new problem => handle via case-default!
    default:
        {
            srcVolume.Close( taskId );
            return( SAPDB_FALSE ); // destination volume has an unmanageable problem
        }
    }

    // create info page again and set bad flag as long as reintegration is executed
    destVolume.PrepareForReIntergration( taskId, srcVolume.GetDBIdentifier());

    // re-link the log volumes
    if( srcVolume.IsPrevLinkageValid() )
    {
        if( !destVolume.SetPredecessor( taskId, srcVolume.GetPrevLogicalDevNo() ))
            return( SAPDB_FALSE );
    }
    if( srcVolume.IsNextLinkageValid() )
    {
        if( ! destVolume.SetSuccessor( taskId, srcVolume.GetNextLogicalDevNo() ))
            return( SAPDB_FALSE );
    }

    // Copy source log volume to destination (formaly bad) volume
    srcVolume.GetNameWithTermination( srcName );
    destVolume.GetNameWithTermination( destName );

    IOMan_Exception errMsg( __CONTEXT__, IOMAN_INFO_START_REINTEGRATION,
                            SAPDB_ToString( srcVolume.GetLogicalDevNo(), _T_d ),
                            srcName, destName );
    RTE_Message( errMsg );

    SAPDB_Bool1         bOkay = SAPDB_TRUE;
    tsp00_ErrText       errText;
    const IOMan_BlockNo startBlock = 1; // do not copy IOManInfoPage to conserve bad flag

    vcopyvolume ( srcVolume.GetPhysicalDevNoOfMirroredLog(),
                  destVolume.GetPhysicalDevNoOfMirroredLog(),
                  startBlock, srcVolume.GetUsableSize(),
                  taskId, errText, bOkay );

    if( SAPDB_TRUE == bOkay )
    {
        // remove bad flag from destination volume
        destVolume.EnableAfterReIntegration( taskId );

        IOMan_Exception errMsg( __CONTEXT__, IOMAN_INFO_STOP_REINTEGRATION );
        RTE_Message( errMsg );
    }
    else
    {
        IOMan_Exception errMsg( __CONTEXT__, IOMAN_ERR_STOP_REINTEGRATION,
                                SAPDB_ToString( errText, errText.length(), errText.length()));
        RTE_Message( errMsg );
    }

    srcVolume.Close( taskId );
    destVolume.Close( taskId );

    return( SAPDB_TRUE == bOkay );
}

/*---------------------------------------------------------------------------*/

void
IOMan_DualLogDevice::ReadPage(
    const tsp00_TaskId         taskId,
    IOMan_IPage               &page,
    const IOMan_BlockAddress   &block )
{
    if( m_PrimaryLogVolume.ReadPage( taskId, page, block ))
        return;

    if( m_PrimaryLogVolume.IsWritable() )
    {
        // In case of a bad log page the log volume is set to bad
        m_PrimaryLogVolume.SetMirroredLogToBad( taskId, block.GetBlockNo() );
    }
    Kernel_IAdminRestartShutdown::Instance().Offline();
}

/*---------------------------------------------------------------------------*/

void
IOMan_DualLogDevice::ReadPages(
    const tsp00_TaskId         taskId,
    IOMan_IKernelPages         &pages,
    const IOMan_ClusterAddress &cluster,
    const SAPDB_UInt           startIndex )
{
    if( m_PrimaryLogVolume.ReadPages( taskId, pages, cluster, startIndex ))
        return;

    if( m_PrimaryLogVolume.IsWritable() )
    {
        // In case of a bad log page the log volume is set to bad
        m_PrimaryLogVolume.SetMirroredLogToBad( taskId, cluster.GetBlockNo( startIndex ));
    }
    Kernel_IAdminRestartShutdown::Instance().Offline();
}

/*---------------------------------------------------------------------------*/

void
IOMan_DualLogDevice::WritePage(
    const tsp00_TaskId          taskId,
    IOMan_IPage                &page,
    const IOMan_BlockAddress    &block )
{
    SAPDBERR_ASSERT_STATE( block.GetDeviceNo() == GetLogicalDevNo() );

    if( ! IsWritable() )
    {
        IOMan_Exception errMsg1( __CONTEXT__, IOMAN_ERR_NOT_WRITEABLE, GetTypeName(),
                                 SAPDB_ToString( GetLogicalDevNo(), _T_d ),
                                 m_PrimaryLogVolume.GetStateName());
        IOMan_Exception errMsg2( __CONTEXT__, IOMAN_EMERGENCY_MSG1 );

        errMsg1.AppendNewMessage( errMsg2 );

        RTE_Crash( errMsg1 );
    }

    if(! page.PrepareForWrite() )
    {
        IOMan_Exception errMsg( __CONTEXT__, IOMAN_BAD_OUTPUT_PAGE, GetTypeName(),
                                SAPDB_ToString( GetLogicalDevNo(), _T_d ),
                                SAPDB_ToString( block.GetBlockNo(), _T_d ));

        Kernel_FileIO   dumpFile( GetTypeName(), KERNEL_BAD_FILE_EXTENSION,
                                  SAPDB_ToString( GetLogicalDevNo(), _T_d ),
                                  SAPDB_ToString( block.GetBlockNo(), _T_d ));

        dumpFile.Write( page );
        dumpFile.Close();

        RTE_Crash( errMsg );
    }

    tsp00_PageAddr pFrame = reinterpret_cast< tsp00_PageAddr >( page.GetFramePtr() );

    if(! WriteVectorSimple( taskId, &pFrame, block.GetBlockNo(), IOMAN_WRITE_ONE_BLOCK ))
    {
        IOMan_Exception errMsg( __CONTEXT__, IOMAN_EMERGENCY_MSG1 );
        RTE_Message( errMsg );

        Kernel_IAdminRestartShutdown::Instance().Offline();
    }
}

/*---------------------------------------------------------------------------*/

void
IOMan_DualLogDevice::WritePages(
    const tsp00_TaskId          taskId,
    IOMan_IKernelPages          &pages,
    const IOMan_ClusterAddress  &cluster,
    const SAPDB_UInt            startIndex )
{
    SAPDBERR_ASSERT_STATE( cluster.GetDeviceNo() == GetLogicalDevNo() );

    if( ! IsWritable() )
    {
        IOMan_Exception errMsg1( __CONTEXT__, IOMAN_ERR_NOT_WRITEABLE, GetTypeName(),
                                 SAPDB_ToString( GetLogicalDevNo(), _T_d ),
                                 m_PrimaryLogVolume.GetStateName());
        IOMan_Exception errMsg2( __CONTEXT__, IOMAN_EMERGENCY_MSG1 );

        errMsg1.AppendNewMessage( errMsg2 );

        RTE_Crash( errMsg1 );
    }

    IOMan_ClusterAddress::BlockIndex index = 0;
    IOMan_IKernelPagesIterator       iter( pages );

    for( iter.Begin( startIndex );
            !iter.End()  && index < cluster.GetBlockCount();
            ++iter, ++index )
    {
        if(! ( *iter ).PrepareForWrite() )
        {
            IOMan_Exception errMsg( __CONTEXT__, IOMAN_BAD_OUTPUT_PAGE, GetTypeName(),
                                    SAPDB_ToString( GetLogicalDevNo(), _T_d ),
                                    SAPDB_ToString( cluster.GetBlockNo( index ), _T_d ));

            Kernel_FileIO   dumpFile( GetTypeName(), KERNEL_BAD_FILE_EXTENSION,
                                      SAPDB_ToString( GetLogicalDevNo(), _T_d ),
                                      SAPDB_ToString( cluster.GetBlockNo( index ), _T_d ));
            dumpFile.Write( *iter );
            dumpFile.Close();

            RTE_Crash( errMsg );
        }
    }
    if(! WriteVectorSimple( taskId, pages.GetFrameList( startIndex ),
                            cluster.GetBlockNo( 0 ), cluster.GetBlockCount() ))
    {
        IOMan_Exception errMsg( __CONTEXT__, IOMAN_EMERGENCY_MSG1 );
        RTE_Message( errMsg );

        Kernel_IAdminRestartShutdown::Instance().Offline();
    }
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

SAPDB_Bool
IOMan_DualLogDevice::WriteVectorSimple(
    const tsp00_TaskId		taskId,
    tsp00_PageAddr          *pFrame,
    const IOMan_BlockNo     blockNo,
    const IOMan_BlockCount  blockCount )
{
    SAPDBERR_ASSERT_STATE( m_PrimaryLogVolume.IsBlockNoValid( blockNo ));
    SAPDBERR_ASSERT_STATE( m_PrimaryLogVolume.IsBlockNoValid( blockNo ));
    SAPDBERR_ASSERT_STATE( m_SecondaryLogVolume.IsBlockNoValid( blockNo + blockCount - 1 ));
    SAPDBERR_ASSERT_STATE( m_SecondaryLogVolume.IsBlockNoValid( blockNo + blockCount - 1 ));

    SAPDB_Bool1     bOkay1 = SAPDB_TRUE;
    SAPDB_Bool1     bOkay2 = SAPDB_TRUE;
    tsp00_ErrText   errText1;
    tsp00_ErrText   errText2;

    vdualvectorio( m_PrimaryLogVolume.GetPhysicalDevNoOfMirroredLog(),
                   m_SecondaryLogVolume.GetPhysicalDevNoOfMirroredLog(),
                   blockNo, pFrame, blockCount, taskId,
                   errText1, bOkay1, errText1, bOkay2 );

    if( SAPDB_TRUE != bOkay1 )
    {
        IOMan_Exception errMsg( __CONTEXT__, IOMAN_NOT_ACCESSIBLE, GetTypeName(),
                                SAPDB_ToString( m_PrimaryLogVolume.GetLogicalDevNo(), _T_d ),
                                SAPDB_ToString( blockNo, _T_d ),
                                SAPDB_ToString( errText1, errText1.length(), errText1.length()));
        RTE_Message( errMsg );
    }
    if( SAPDB_TRUE != bOkay2 )
    {
        IOMan_Exception errMsg( __CONTEXT__, IOMAN_NOT_ACCESSIBLE, GetTypeName(),
                                SAPDB_ToString( m_SecondaryLogVolume.GetLogicalDevNo(), _T_d ),
                                SAPDB_ToString( blockNo, _T_d ),
                                SAPDB_ToString( errText2, errText2.length(), errText2.length()));
        RTE_Message( errMsg );
    }
    return( SAPDB_TRUE == bOkay1 && SAPDB_TRUE == bOkay2);
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
