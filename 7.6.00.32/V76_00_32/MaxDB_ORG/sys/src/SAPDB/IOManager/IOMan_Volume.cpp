/*!
    @file           IOMan_Volume.cpp
    @author         TorstenS
    @ingroup        IOManagement
    @brief          This module is used to handle volumes.
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

#include "hgg08.h"      // PASCAL: g08config
#include "heo54k.h"     // RTE   : Vattach/Vdetach/Vblockio/Vvectorio
#include "heo55k.h"     // RTE   : vbegexcl/vendexcl
#include "heo56.h"      // RTE   : Vsleep
#include "heo58.h"      // RTE   : vget_devsize/vget_devname/vnew_devspace/vrevoke_devspace

#include "IOManager/IOMan_Volume.hpp"
#include "IOManager/IOMan_MessageList.hpp"  // ++++
#include "IOManager/IOMan_Messages.hpp"     // ++++
#include "IOManager/IOMan_ReservedBlockAddress.hpp"
#include "FrameControl/FrameCtrl_ICommon.hpp"
#include "KernelCommon/Kernel_FileIO.hpp"
#include "KernelCommon/Kernel_IAdminRestartShutdown.hpp"
#include "RunTime/RTE_Crash.hpp"
#include "RunTime/RTE_Message.hpp"  
#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "SAPDBCommon/SAPDB_ToString.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp" 

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

/* -----------------------------------------------------------------------*/
/*!
   @class   IOMan_VolumeLock
   @brief   This class is used to synchronize concurrent access to the shared
            data of the IOMan_Volume. To synchronize the access the g08config
            region is used.
 */
/* -----------------------------------------------------------------------*/


class IOMan_VolumeLock
{
public:

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   The constructor is used to get the lock.
       @return  none
     */
    /* -----------------------------------------------------------------------*/

    IOMan_VolumeLock( const tsp00_TaskId taskId )
    :
    m_Owner( taskId )
    {
        vbegexcl( m_Owner, g08config );
    };

    /* -----------------------------------------------------------------------*/
    /*!
       @brief   The destructor will release the lock.
       @return  none
     */
    /* -----------------------------------------------------------------------*/

    ~IOMan_VolumeLock()
    {
        vendexcl( m_Owner, g08config );
    };


private:

    /* -----------------------------------------------------------------------*/
    /*!
    @brief: Contains the taskId of the lock holder.
    */
    /* -----------------------------------------------------------------------*/

    tsp00_TaskId    m_Owner;
};

/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

/* -----------------------------------------------------------------------*/
/*!
    @brief  This function is used cut trailing blanks of a given
            string. Additional the c termination token is added.
    @param  pDBIdentifier [in/out] pointer to a string of the length
                                   IOMAN_DB_IDENT_LENGTH
    @return  none
 */
/* -----------------------------------------------------------------------*/

void ReduceDbIdentifier( SAPDB_Char *pDBIdentifier )
{
    SAPDB_Int i = IOMAN_DB_IDENT_LENGTH -1;

    while( ' ' ==  *( pDBIdentifier + i ) && i > 0 )
        i -= 1;

    if( i < ( IOMAN_DB_IDENT_LENGTH -2 ))
        *( pDBIdentifier + i + 1 ) = '\0';
}

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

SAPDB_Bool
IOMan_Volume::Add(
    tsp00_VFilename         &devName,
    const IOMan_BlockCount  devSize ) const
{
    if( IsOnline() )
    {
        Msg_List msgList(
            Msg_List::Error,
            SDBMSG_IOMAN_ADD_VOLUME_IS_ONLINE,
            Msg_Arg( SDBMSGTAG_IOMAN_ADD_VOLUME_IS_ONLINE__VOLUME_TYPE, GetTypeName()),
            Msg_Arg( SDBMSGTAG_IOMAN_ADD_VOLUME_IS_ONLINE__VOLUME_ID, GetLogicalDevNo())
        );

        RTE_Message( msgList );
        msgList.ClearMessageList();
        return SAPDB_FALSE;
    }

    SAPDB_Bool1  bOkay = SAPDB_TRUE;
    Msg_List     errList;

    vnew_devspace( GetType(), GetLogicalDevNo(), devName, devSize, bOkay, errList );

    if( SAPDB_TRUE != bOkay )
    {
        RTE_Message( errList );
        IOMan_MessageList errMsg( __CONTEXT__, IOMAN_ERR_NEW_VOLUME_FAILED,
                                  GetTypeName(), SAPDB_ToString( GetLogicalDevNo(), _T_d ));
        RTE_Message( errMsg );
        return SAPDB_FALSE;
    }
    return SAPDB_TRUE;
}

/*---------------------------------------------------------------------------*/

void
IOMan_Volume::Del() const
{
    if( IsOnline() )
    {
        IOMan_MessageList errMsg( __CONTEXT__, IOMAN_ERR_VOLUME_IS_ONLINE,
                                  GetTypeName(), SAPDB_ToString( GetLogicalDevNo(), _T_d ));
        RTE_Crash( errMsg );
    }
    vrevoke_devspace( GetType(), GetLogicalDevNo() );
}

/*---------------------------------------------------------------------------*/

void
IOMan_Volume::Close( const tsp00_TaskId taskId )
{
    if (! IsOnline() )
        return;

    m_State    = IOMan_Offline;
    m_Capacity = 0;
    m_Mode     = RTE_VolumeAccessModeUnknown;

    vdetach( GetPhysicalDevNo(), taskId );

    m_PhysicalDevNo = 0;
}

/*---------------------------------------------------------------------------*/

IOMan_ReturnCode
IOMan_Volume::CreateInfoPage(
    const tsp00_TaskId   taskId,
    const SAPDB_Byte     *pDBIdentifier )
{
    // Check if volume is stored within configuration file.
    if( ! GetConfiguration( m_Capacity, m_BlockSize, m_Mode ))
        return IOMan_NotConfigured;

    if( ! Attach( taskId, ! IOMAN_SUPPRESS_ERR_MSG ))
        return IOMan_AttachFailed;

    m_InfoPage.InitializeFrame( GetLogicalDevNo(), GetConfiguredSize(),
                                GetBlockSize(), pDBIdentifier );

    // Write IOMan_InfoPage to volume, therefore the active state has to be set.
    m_State = IOMan_ReadWrite;

    WriteInfoPage( taskId );

    Close( taskId );

    return IOMan_Okay;
}

/*---------------------------------------------------------------------------*/

IOMan_ReturnCode
IOMan_Volume::Create(
    const tsp00_TaskId   taskId,
    const SAPDB_Byte     *pDBIdentifier )
{
    SAPDBERR_ASSERT_STATE( ! IsOnline() );

    IOMan_MessageList msgList;

    const IOMan_ReturnCode retCode = Format( taskId, msgList );

    if( IOMan_Okay != retCode )
    {
        RTE_Message( msgList );

        return retCode;
    }
    return CreateInfoPage( taskId, pDBIdentifier );
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
IOMan_Volume::GetName( tsp00_VFilename &DevName ) const
{
    SAPDB_Bool1  bOkay = SAPDB_TRUE;

    vget_devname (GetType(), GetLogicalDevNo(), DevName, bOkay);

    return SAPDB_TRUE == bOkay;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
IOMan_Volume::GetNameWithTermination( tsp00_VFilename &devName ) const
{
    if( ! GetName( devName ))
        return SAPDB_FALSE;

    SAPDB_Int i = sizeof( tsp00_VFilename ) -1;

    while( ' ' ==  devName[ i ]  && i > 0 )
        i -= 1;

    if( i < ( sizeof( tsp00_VFilename ) -2 ))
        devName[ i + 1 ] = '\0';

    return SAPDB_TRUE;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
IOMan_Volume::IsConfigured() const
{
    IOMan_BlockCount        capacity  = 0;
    SAPDB_UInt4             blockSize = 0;
    RTE_VolumeAccessMode    accessMode = RTE_VolumeAccessModeUnknown;

    return GetConfiguration( capacity, blockSize, accessMode );
}

/*---------------------------------------------------------------------------*/

IOMan_ReturnCode
IOMan_Volume::Open(
    const tsp00_TaskId taskId,
    const SAPDB_Bool   bCheckBadFlag,
    const SAPDB_Bool   bOpenForWrite )
{
    IOMan_MessageList msgList;

    const IOMan_ReturnCode retCode =
        OpenSimple( taskId, ! IOMAN_SUPPRESS_ERR_MSG, bOpenForWrite );

    if( IOMan_Okay != retCode )
        return retCode;

    if( ! Read( taskId, m_InfoPage,
                IOMan_ReservedBlockAddress::GetInfoPageBlockNo( GetLogicalDevNo()))
      )
    {
        IOMan_MessageList errMsg( __CONTEXT__, IOMAN_READ_INFO_PAGE_FAILED,
                                  GetTypeName(), SAPDB_ToString( GetLogicalDevNo(), _T_d ));
        RTE_Message( errMsg);

        Kernel_IAdminRestartShutdown::Instance().Offline();
    }

    if( ! IsVolumeConfigurationValid( ! IOMAN_LAZY_CAPACITY_CHECK, msgList ))
    {
        RTE_Message( msgList );
        Close ( taskId );
        return IOMan_ConfigurationCorrupted;
    }

    if( IOMan_Okay != retCode )
        return retCode;

    // Check the correctness of the volume in consideration of the bad flag.
    if( bCheckBadFlag && IsBad() )
    {
        IOMan_MessageList errMsg( __CONTEXT__, IOMAN_INFO_VOLUME_MARKED_AS_BAD,
                                  GetTypeName(), SAPDB_ToString( GetLogicalDevNo(), _T_d ));
        RTE_Message( errMsg );

        Close( taskId );
        return IOMan_BadVolume;
    }
    return IOMan_Okay;
}
/*---------------------------------------------------------------------------*/

SAPDB_Bool
IOMan_Volume::SetNextLinkage(
    const tsp00_TaskId  taskId,
    const IOMan_Volume  &nextVolume )
{
    if( ! IsWritable() )
        return SAPDB_FALSE;

    SAPDBTRACE_WRITELN( IOMan_Trace, 6, "Link source volume " << GetLogicalDevNo() <<
                        " with next volume " << nextVolume.GetLogicalDevNo() );

    m_InfoPage.SetNextLogicalVolNo( nextVolume.GetLogicalDevNo() );

    WriteInfoPage( taskId );

    return SAPDB_TRUE;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool IOMan_Volume::SetPrevLinkage(
    const tsp00_TaskId  taskId,
    const IOMan_Volume  &prevVolume )
{
    if( ! IsWritable() )
        return SAPDB_FALSE;

    SAPDBTRACE_WRITELN( IOMan_Trace, 6, "Link source volume " << GetLogicalDevNo() <<
                        " with previous volume " << prevVolume.GetLogicalDevNo() );

    m_InfoPage.SetPrevLogicalVolNo( prevVolume.GetLogicalDevNo() );

    WriteInfoPage( taskId );

    return SAPDB_TRUE;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
IOMan_Volume::IsLinkageValid(
    const tsp00_TaskId      taskId,
    const IOMan_Volume    &volume ) const
{
    if( ! IsOnline() )
        return SAPDB_FALSE;

    SAPDBTRACE_WRITELN( IOMan_Trace, 6, "Validate link between source devNo " <<
                        GetLogicalDevNo() << " and destination devNo " <<
                        volume.GetLogicalDevNo() );

    if( GetNextLogicalDevNo() == volume.GetLogicalDevNo() )
    {
        if( GetLogicalDevNo() == volume.GetPrevLogicalDevNo() )
            return SAPDB_TRUE;
    }

    IOMan_MessageList errMsg( __CONTEXT__, IOMAN_CORRUPTED_CONFIGURATION,
                              GetTypeName(), SAPDB_ToString( GetLogicalDevNo(), _T_d ),
                              SAPDB_ToString( volume.GetLogicalDevNo(), _T_d ));
    RTE_Message( errMsg );
    return SAPDB_FALSE;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
IOMan_Volume::IsPrevLinkageValid() const
{
    if( ! IsOnline() )
        return SAPDB_FALSE;

    if( m_InfoPage.IsPrevLogicalDevNoValid() )
        return SAPDB_TRUE;

    return SAPDB_FALSE;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
IOMan_Volume::IsNextLinkageValid() const
{
    if( ! IsOnline() )
        return SAPDB_FALSE;

    if( m_InfoPage.IsNextLogicalDevNoValid() )
        return SAPDB_TRUE;

    return SAPDB_FALSE;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
IOMan_Volume::IsDBIdentifierValid(
    const tsp00_TaskId  taskId,
    const SAPDB_Byte    *pDBIdentifier ) const
{
    if( ! IsOnline() )
        return SAPDB_FALSE;

    if( 0 == memcmp( m_InfoPage.GetDBIdentifier(), pDBIdentifier,
                     m_InfoPage.GetDBIdentifierSize()))
        return SAPDB_TRUE;

    SAPDB_Char dbIdent1[ IOMAN_DB_IDENT_LENGTH ];
    SAPDB_Char dbIdent2[ IOMAN_DB_IDENT_LENGTH ];

    SAPDB_MemCopyNoCheck( dbIdent1, m_InfoPage.GetDBIdentifier(), IOMAN_DB_IDENT_LENGTH);
    SAPDB_MemCopyNoCheck( dbIdent2, pDBIdentifier, IOMAN_DB_IDENT_LENGTH);

    ReduceDbIdentifier( dbIdent1 );
    ReduceDbIdentifier( dbIdent2 );

    IOMan_MessageList errMsg( __CONTEXT__, IOMAN_DB_IDENTIFIER_MISSMATCH,
                              dbIdent1, GetTypeName(), SAPDB_ToString( GetLogicalDevNo(), _T_d ),
                              dbIdent2);

    RTE_Message( errMsg );
    return SAPDB_FALSE;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
IOMan_Volume::Read(
    const tsp00_TaskId    taskId,
    IOMan_IPage          &page,
    const IOMan_BlockNo   blockNo )
{
    for( SAPDB_UInt readCount = 1; IOMan_Volume::MaxReadRetries() >= readCount; ++readCount )
    {
        if(! ReadSimple( taskId, tsp00_PageAddr( page.GetFramePtr() ), blockNo ))
        {
            IOMan_MessageList errMsg( __CONTEXT__, IOMAN_EMERGENCY_MSG1 );
            RTE_Message( errMsg );

            Kernel_IAdminRestartShutdown::Instance().Offline();
        }

        if( page.CheckAfterRead() )
            return SAPDB_TRUE;

        IOMan_MessageList errMsg( __CONTEXT__, IOMAN_BAD_PAGE,
                                  GetTypeName(), SAPDB_ToString( GetLogicalDevNo(), _T_d ),
                                  SAPDB_ToString( blockNo, _T_d ));

        Kernel_FileIO   dumpFile( GetTypeName(), KERNEL_BAD_FILE_EXTENSION,
                                  SAPDB_ToString( GetLogicalDevNo(), _T_d ), SAPDB_ToString( blockNo, _T_d ));

        dumpFile.Write( page );
        dumpFile.Close();

        RTE_Message( errMsg );

        vsleep( taskId, IOMan_Volume::RetrySleepTime());
    }
    return SAPDB_FALSE; // abnormal termination => bad page
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
IOMan_Volume::ReadSimple(
    const tsp00_TaskId      taskId,
    const tsp00_PageAddr    pFrame,
    const IOMan_BlockNo     blockNo,
    const IOMan_BlockCount  blockCount )
{

    if( ! IsOnline() )
    {
        IOMan_MessageList errMsg( __CONTEXT__, IOMAN_WRN_VOLUME_OFFLINE,
                                  GetTypeName(), SAPDB_ToString( GetLogicalDevNo(), _T_d ));
        RTE_Message( errMsg );
        return SAPDB_FALSE;
    }

    SAPDBERR_ASSERT_STATE( IsBlockNoMapable( blockNo ));

    SAPDB_Bool1     bOkay = SAPDB_TRUE;
    tsp00_ErrText   errText;

    vblockio( GetPhysicalDevNo(), blockNo, tsp2_io_op_code::fromConst( sp2ioc_read ),
              pFrame, blockCount, taskId, errText, bOkay );

    if( SAPDB_TRUE != bOkay )
    {
        Msg_List msgList(
            Msg_List::Error, SDBMSG_IOMAN_NOT_ACCESSIBLE,
            Msg_Arg(SDBMSGTAG_IOMAN_NOT_ACCESSIBLE__VOLUME_TYPE, GetTypeName()),
            Msg_Arg(SDBMSGTAG_IOMAN_NOT_ACCESSIBLE__VOLUME_ID,   GetLogicalDevNo()),
            Msg_Arg(SDBMSGTAG_IOMAN_NOT_ACCESSIBLE__BLOCK_NO,    blockNo ),
            Msg_Arg(SDBMSGTAG_IOMAN_NOT_ACCESSIBLE__NUMBER,      blockCount ),
            Msg_Arg(SDBMSGTAG_IOMAN_NOT_ACCESSIBLE__ERRORTEXT,
                    SAPDB_ToString( errText, errText.length(), errText.length())),
            Msg_OptArg(SDBMSGTAG_IOMAN_NOT_ACCESSIBLE__NUMBER1, GetPhysicalDevNo()));

        RTE_Message( msgList );
    }
    return SAPDB_TRUE == bOkay;
}

/*---------------------------------------------------------------------------*/

void
IOMan_Volume::SetBad(
    const tsp00_TaskId   taskId,
    const IOMan_BlockNo  blockNo )
{
    IOMan_VolumeLock  lock( taskId ); // synchronize concurrent access to info page

    if ((! IsWritable()) || (! IsBlockNoMapable( blockNo )))
        return;

    // don't update info page if the error is caused by the info page!
    if( blockNo == IOMan_ReservedBlockAddress::GetInfoPageBlockNo( GetLogicalDevNo() ))
        return;

    if( IsBad() )
        return;

    m_InfoPage.SetBadBlockNo( blockNo );
    WriteInfoPage( taskId );

    IOMan_MessageList ErrMsg( __CONTEXT__, IOMAN_ERR_BAD_VOLUME,
                              GetTypeName(), SAPDB_ToString( GetLogicalDevNo(), _T_d ));
    RTE_Message( ErrMsg );

    Close( taskId );
}

/*---------------------------------------------------------------------------*/

IOMan_ReturnCode
IOMan_Volume::UnSetBad( const tsp00_TaskId  taskId )
{
    IOMan_VolumeLock  lock( taskId ); // synchronize concurrent access to info page

    if( IsWritable() )
        return IOMan_Okay; // an online volume is not bad

    // do not check bad volume flag during open
    const SAPDB_Bool    bCheckBadFlag = SAPDB_TRUE;
    const SAPDB_Bool    bOpenForWrite = SAPDB_TRUE;

    // don't check bad volume flag
    const IOMan_ReturnCode retCode = Open( taskId, ! bCheckBadFlag, bOpenForWrite );

    if( IOMan_Okay != retCode )
        return retCode;

    if( IsBad() )
    {
        m_InfoPage.SetBadBlockNo( 0 );
        m_InfoPage.IncrementBadResetCount();
        WriteInfoPage( taskId );

        IOMan_MessageList ErrMsg( __CONTEXT__, IOMAN_INFO_UNSET_BAD_VOLUME,
                                  GetTypeName(), SAPDB_ToString( GetLogicalDevNo(), _T_d ));
        RTE_Message( ErrMsg );
    }
    Close( taskId );

    return IOMan_Okay;
}

/*---------------------------------------------------------------------------*/

IOMan_ReturnCode
IOMan_Volume::MigrateCapacity(
    const tsp00_TaskId  taskId,
    IOMan_MessageList   &msgList )
{
    const IOMan_ReturnCode retCode =
        OpenSimple( taskId, ! IOMAN_SUPPRESS_ERR_MSG, IOMAN_OPEN_FOR_WRITE );

    if( IOMan_Okay != retCode )
        return retCode;

    if( ! Read( taskId, m_InfoPage,
                IOMan_ReservedBlockAddress::GetInfoPageBlockNo( GetLogicalDevNo()))
      )
    {
        IOMan_MessageList errMsg( __CONTEXT__, IOMAN_READ_INFO_PAGE_FAILED,
                                  GetTypeName(), SAPDB_ToString( GetLogicalDevNo(), _T_d ));
        RTE_Message( errMsg );
        msgList.AppendNewMessage( errMsg );

        Close ( taskId );
        return IOMan_BadInfoPage;
    }

    if( ! IsVolumeConfigurationValid( IOMAN_LAZY_CAPACITY_CHECK, msgList ))
    {
        RTE_Message( msgList );
        Close ( taskId );
        return IOMan_ConfigurationCorrupted;
    }

    if( m_InfoPage.GetCapacity() != GetConfiguredSize() )
    {
        m_InfoPage.SetCapacity( GetConfiguredSize());
        WriteInfoPage( taskId );
    }

    Close( taskId );

    return IOMan_Okay;
}

/*---------------------------------------------------------------------------*/

void
IOMan_Volume::Write(
    const tsp00_TaskId    taskId,
    IOMan_IPage          &page,
    const IOMan_BlockNo   blockNo )
{
    if(! page.PrepareForWrite() )
    {
        IOMan_MessageList errMsg( __CONTEXT__, IOMAN_BAD_OUTPUT_PAGE,
                                  GetTypeName(), SAPDB_ToString( GetLogicalDevNo(), _T_d ),
                                  SAPDB_ToString( blockNo, _T_d ));

        Kernel_FileIO   dumpFile( GetTypeName(), KERNEL_BAD_FILE_EXTENSION,
                                  SAPDB_ToString( GetLogicalDevNo(), _T_d ), SAPDB_ToString( blockNo, _T_d ));

        dumpFile.Write( page );
        dumpFile.Close();

        RTE_Crash( errMsg );
    }
    if( ! WriteSimple( taskId, tsp00_PageAddr( page.GetFramePtr()), blockNo ))
    {
        IOMan_MessageList errMsg( __CONTEXT__, IOMAN_EMERGENCY_MSG1 );
        RTE_Message( errMsg );

        Kernel_IAdminRestartShutdown::Instance().Offline();
    }

    if( IOMan_Check.ChecksLevel( 6 ))
    {
        if( page.CheckAfterRead() )
            return;

        IOMan_MessageList errMsg( __CONTEXT__, IOMAN_BAD_PAGE,
                                  GetTypeName(), SAPDB_ToString( GetLogicalDevNo(), _T_d ),
                                  SAPDB_ToString( blockNo, _T_d ));

        Kernel_FileIO   dumpFile( GetTypeName(), KERNEL_BAD_FILE_EXTENSION,
                                  SAPDB_ToString( GetLogicalDevNo(), _T_d ), SAPDB_ToString( blockNo, _T_d ));

        dumpFile.Write( page );
        dumpFile.Close();

        RTE_Crash( errMsg );
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
IOMan_Volume::WriteSimple(
    const tsp00_TaskId      taskId,
    const tsp00_PageAddr    pFrame,
    const IOMan_BlockNo     blockNo )
{
    if( ! IsWritable() )
    {
        IOMan_MessageList errMsg( __CONTEXT__, IOMAN_ERR_NOT_WRITEABLE, GetTypeName(),
                                  SAPDB_ToString( GetLogicalDevNo(), _T_d ), GetStateName());
        RTE_Message( errMsg );
        return SAPDB_FALSE;
    }

    SAPDBERR_ASSERT_STATE( IsBlockNoMapable( blockNo ));

    SAPDB_Bool1     bOkay = SAPDB_TRUE;
    tsp00_ErrText   errText;

    vblockio( GetPhysicalDevNo(), blockNo, tsp2_io_op_code::fromConst( sp2ioc_write ),
              pFrame, IOMAN_WRITE_ONE_BLOCK, taskId, errText, bOkay );

    if( SAPDB_TRUE != bOkay )
    {
        Msg_List msgList(
            Msg_List::Error, SDBMSG_IOMAN_NOT_ACCESSIBLE,
            Msg_Arg(SDBMSGTAG_IOMAN_NOT_ACCESSIBLE__VOLUME_TYPE, GetTypeName()),
            Msg_Arg(SDBMSGTAG_IOMAN_NOT_ACCESSIBLE__VOLUME_ID,   GetLogicalDevNo()),
            Msg_Arg(SDBMSGTAG_IOMAN_NOT_ACCESSIBLE__BLOCK_NO,    blockNo ),
            Msg_Arg(SDBMSGTAG_IOMAN_NOT_ACCESSIBLE__NUMBER,      1 ),
            Msg_Arg(SDBMSGTAG_IOMAN_NOT_ACCESSIBLE__ERRORTEXT,
                    SAPDB_ToString( errText, errText.length(), errText.length())),
            Msg_OptArg(SDBMSGTAG_IOMAN_NOT_ACCESSIBLE__NUMBER1, GetPhysicalDevNo()));

        RTE_Message( msgList );
    }
    return SAPDB_TRUE == bOkay;
}

/*---------------------------------------------------------------------------*/

void
IOMan_Volume::WriteVector(
    const tsp00_TaskId          taskId,
    IOMan_IKernelPages          &pages,
    const IOMan_ClusterAddress  &cluster,
    const SAPDB_UInt            startIndex)
{
    IOMan_ClusterAddress::BlockIndex index = 0;
    IOMan_IKernelPagesIterator       iter( pages );

    for( iter.Begin( startIndex ); // PTS 1110708 TS 2001-06-27
            !iter.End()  && index < cluster.GetBlockCount();
            ++iter, ++index )
    {
        if(! ( *iter ).PrepareForWrite() )
        {
            IOMan_MessageList errMsg( __CONTEXT__, IOMAN_BAD_OUTPUT_PAGE,
                                      GetTypeName(), SAPDB_ToString( GetLogicalDevNo(), _T_d ),
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
        IOMan_MessageList ErrMsg( __CONTEXT__, IOMAN_EMERGENCY_MSG1 );
        RTE_Message( ErrMsg );

        Kernel_IAdminRestartShutdown::Instance().Offline();
    }

    if( IOMan_Check.ChecksLevel( 6 ))
    {
        IOMan_ClusterAddress::BlockIndex index = 0;
        IOMan_IKernelPagesIterator       iter( pages );

        for( iter.Begin( startIndex );
                !iter.End()  && index < cluster.GetBlockCount();
                ++iter, ++index )
        {
            if( !( *iter ).CheckAfterRead() )
            {

                IOMan_MessageList errMsg( __CONTEXT__, IOMAN_BAD_OUTPUT_PAGE,
                                          GetTypeName(), SAPDB_ToString( GetLogicalDevNo(), _T_d ),
                                          SAPDB_ToString( cluster.GetBlockNo( index ), _T_d ));

                Kernel_FileIO   dumpFile( GetTypeName(), KERNEL_BAD_FILE_EXTENSION,
                                          SAPDB_ToString( GetLogicalDevNo(), _T_d ),
                                          SAPDB_ToString( cluster.GetBlockNo( index ), _T_d ));

                dumpFile.Write( *iter );
                dumpFile.Close();

                RTE_Crash( errMsg );
            }
        }
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
IOMan_Volume::WriteVectorSimple(
    const tsp00_TaskId      taskId,
    tsp00_PageAddr          *pFrame,
    const IOMan_BlockNo     blockNo,
    const IOMan_BlockCount  blockCount )
{
    if( ! IsWritable() )
    {
        IOMan_MessageList errMsg( __CONTEXT__, IOMAN_ERR_NOT_WRITEABLE, GetTypeName(),
                                  SAPDB_ToString( GetLogicalDevNo(), _T_d ), GetStateName());
        RTE_Message( errMsg );
        return SAPDB_FALSE;
    }

    SAPDBERR_ASSERT_STATE( IsBlockNoMapable( blockNo ));
    SAPDBERR_ASSERT_STATE( IsBlockNoMapable( blockNo + blockCount - 1 ));

    SAPDB_Bool1     bOkay = SAPDB_TRUE;
    tsp00_ErrText   errText;

    vvectorio( GetPhysicalDevNo(), blockNo, pFrame, blockCount, taskId, errText, bOkay );

    if( SAPDB_TRUE != bOkay )
    {
        Msg_List msgList(
            Msg_List::Error, SDBMSG_IOMAN_NOT_ACCESSIBLE,
            Msg_Arg(SDBMSGTAG_IOMAN_NOT_ACCESSIBLE__VOLUME_TYPE, GetTypeName()),
            Msg_Arg(SDBMSGTAG_IOMAN_NOT_ACCESSIBLE__VOLUME_ID,   GetLogicalDevNo()),
            Msg_Arg(SDBMSGTAG_IOMAN_NOT_ACCESSIBLE__BLOCK_NO,    blockNo ),
            Msg_Arg(SDBMSGTAG_IOMAN_NOT_ACCESSIBLE__NUMBER,      blockCount ),
            Msg_Arg(SDBMSGTAG_IOMAN_NOT_ACCESSIBLE__ERRORTEXT,
                    SAPDB_ToString( errText, errText.length(), errText.length())),
            Msg_OptArg(SDBMSGTAG_IOMAN_NOT_ACCESSIBLE__NUMBER1, GetPhysicalDevNo()));

        RTE_Message( msgList );
    }
    return SAPDB_TRUE == bOkay;
}

/*---------------------------------------------------------------------------*/

IOMan_Volume::AsynReadWriteRc
IOMan_Volume::AsynReadWrite(
    RTETask_ITask&              task,
    Msg_List&                   msgList,
    const SAPDB_Int4            asynIOHandle,
    IOMan_IKernelPages          &pages,
    const SAPDB_UInt            startIndex,
    const IOMan_ClusterAddress  &cluster )
{
    if( ! IsOnline()){
        return arwRcOffline;
    }
    SAPDBERR_ASSERT_STATE( IsBlockNoMapable( cluster.GetBlockNo( 0 ) ));
    SAPDBERR_ASSERT_STATE( IsBlockNoMapable( cluster.GetBlockNo( 0 ) + cluster.GetBlockCount() -1 ));

    tsp00_VaReturn  asynError;
    SAPDB_Bool1     bOkay = SAPDB_TRUE;
    SAPDB_Int4      ioId  = 8888;
    SAPDB_Int4      maxBlockCount;
    tsp00_ErrText   errText;

    vasynio( asynIOHandle, ioId, cluster.GetBlockNo( 0 ), cluster.GetBlockCount(),
             *pages.GetFrameList( startIndex ), asynError, errText );

    if( va_ok == asynError ){
        vasynwait( asynIOHandle, ioId, maxBlockCount, asynError, errText );
    }
    SAPDBERR_ASSERT_STATE( 8888 == ioId );
    if( va_ok == asynError ){
        return arwRcOkay;
    }
    msgList = Msg_List(
                  Msg_List::Error, SDBMSG_IOMAN_NOT_ACCESSIBLE,
                  Msg_Arg(SDBMSGTAG_IOMAN_NOT_ACCESSIBLE__VOLUME_TYPE, GetTypeName()),
                  Msg_Arg(SDBMSGTAG_IOMAN_NOT_ACCESSIBLE__VOLUME_ID,   GetLogicalDevNo()),
                  Msg_Arg(SDBMSGTAG_IOMAN_NOT_ACCESSIBLE__BLOCK_NO,    cluster.GetBlockNo( 0 )),
                  Msg_Arg(SDBMSGTAG_IOMAN_NOT_ACCESSIBLE__NUMBER,      cluster.GetBlockCount()),
                  Msg_Arg(SDBMSGTAG_IOMAN_NOT_ACCESSIBLE__ERRORTEXT,
                          SAPDB_ToString( errText, errText.length(), errText.length())),
                  Msg_OptArg(SDBMSGTAG_IOMAN_NOT_ACCESSIBLE__NUMBER1, asynIOHandle ));
    return arwRcSystemError;
}

/*===========================================================================*
 *  PRIVATE METHODS                                                          *
 *===========================================================================*/

IOMan_ReturnCode
IOMan_Volume::Format(
    const tsp00_TaskId  taskId,
    IOMan_MessageList   &msgList )
{
    SAPDBERR_ASSERT_STATE( ! IsOnline() );

    // Check if volume is stored within configuration file.
    if( ! GetConfiguration( m_Capacity, m_BlockSize, m_Mode ))
        return IOMan_NotConfigured;

    RTEIO_VolumeInfo dataVol( GetLogicalDevNo(), GetType(), GetInternalSize());

    if( ! RTEIO_FormatSingleVolume( taskId, dataVol, msgList ))
    {
        IOMan_MessageList errMsg( __CONTEXT__, IOMAN_ERR_SINGLE_FORMAT, GetTypeName(),
                                  SAPDB_ToString( GetLogicalDevNo(), _T_d ));

        msgList.AppendNewMessage( errMsg );
        return IOMan_FormatFailed;
    }
    return IOMan_Okay;
}

/*---------------------------------------------------------------------------*/

IOMan_ReturnCode
IOMan_Volume::OpenSimple(
    const tsp00_TaskId taskId,
    const SAPDB_Bool   bSuppressErrMsg,
    const SAPDB_Bool   bOpenForWrite )
{
    IOMan_BlockCount    capacity = 0;

    if( IsOnline() )
        return IOMan_Okay;

    if( ! GetConfiguration( capacity, m_BlockSize, m_Mode ))
        return IOMan_NotConfigured;

    if( ! Attach( taskId, bSuppressErrMsg ))
        return IOMan_AttachFailed;

    // Read IOMan_InfoPage from wanted volume, therefore the
    // active state an the capacity has to be set.
    if( bOpenForWrite )
        m_State = IOMan_ReadWrite;
    else
        m_State = IOMan_Read;
    m_Capacity = capacity;

    return IOMan_Okay;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
IOMan_Volume::Attach(
    const tsp00_TaskId taskId,
    const SAPDB_Bool   bSuppressErrMsg )
{
    SAPDB_Bool1     bOkay = SAPDB_TRUE;
    tsp00_ErrText   errText;

    vattach( GetLogicalDevNo(), GetType(), m_PhysicalDevNo, taskId, errText, bOkay );

    if(( SAPDB_TRUE != bOkay ) && ( ! bSuppressErrMsg ))
    {
        IOMan_MessageList errMsg( __CONTEXT__, IOMAN_ATTACH_FAILED, GetTypeName(),
                                  SAPDB_ToString( GetLogicalDevNo(), _T_d ),
                                  SAPDB_ToString( errText, errText.length(), errText.length()));
        RTE_Message( errMsg );
    }
    return SAPDB_TRUE == bOkay;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
IOMan_Volume::GetConfiguration(
    IOMan_BlockCount    &capacity,
    SAPDB_UInt4         &blockSize,
    RTE_VolumeAccessMode &accessMode ) const
{
    SAPDB_Bool1  bOkay = SAPDB_TRUE;

    // for future use, if data volumes have different page sizes
    // and the size will be stored within the configuration file
    blockSize = FrameCtrl_ICommon::GetInstance().GetPageSize();

    vget_devsize( GetType(), GetLogicalDevNo(), capacity, bOkay );

    accessMode = RTEIO_VolumeInfo( GetLogicalDevNo(), GetType()).GetAccessMode();

    return SAPDB_TRUE == bOkay;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
IOMan_Volume::IsVolumeConfigurationValid(
    const SAPDB_Bool    bDoLazyCapacityCheck,
    IOMan_MessageList   &msgList ) const
{
    if( GetLogicalDevNo() != m_InfoPage.GetLogicalDevNo() )
    {
        IOMan_MessageList errMsg( __CONTEXT__, IOMAN_DEVNO_MISSMATCH,
                                  GetTypeName(), SAPDB_ToString( GetLogicalDevNo(), _T_d ),
                                  SAPDB_ToString( m_InfoPage.GetLogicalDevNo(), _T_d ));
        msgList.AppendNewMessage( errMsg );
        return SAPDB_FALSE;
    }

    if( GetBlockSize() != m_InfoPage.GetBlockSize() )
    {
        IOMan_MessageList errMsg( __CONTEXT__, IOMAN_BLOCKSIZE_MISSMATCH,
                                  GetTypeName(), SAPDB_ToString( GetLogicalDevNo(), _T_d ),
                                  SAPDB_ToString( GetBlockSize(), _T_d ),
                                  SAPDB_ToString( m_InfoPage.GetBlockSize(), _T_d ));
        msgList.AppendNewMessage( errMsg );
        return SAPDB_FALSE;
    }

    if( GetConfiguredSize() == m_InfoPage.GetCapacity() )
        return SAPDB_TRUE;

    if( bDoLazyCapacityCheck ){                            // difference is caused by the physical
        if( GetInternalSize() == m_InfoPage.GetCapacity()) // resp. configured volume size migration
            return SAPDB_TRUE;
    }

    IOMan_MessageList errMsg( __CONTEXT__, IOMAN_CAPACITY_MISSMATCH,
                              GetTypeName(), SAPDB_ToString( GetLogicalDevNo(), _T_d ),
                              SAPDB_ToString( GetConfiguredSize(), _T_d ),
                              SAPDB_ToString( m_InfoPage.GetCapacity(), _T_d ));

    msgList.AppendNewMessage( errMsg );
    return SAPDB_FALSE;
}

/*---------------------------------------------------------------------------*/

void
IOMan_Volume::WriteInfoPage( const tsp00_TaskId  taskId )
{
    Write( taskId, m_InfoPage,
           IOMan_ReservedBlockAddress::GetInfoPageBlockNo( GetLogicalDevNo()));
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
