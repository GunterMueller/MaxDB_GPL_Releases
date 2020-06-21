/*!
  @file           IOMan_Migration.cpp
  @author         TorstenS
  @ingroup        IOManagement
  @brief          This modul implements the inplace data base migration of
                  OLTP releases 7.2 and 7.3 to 7.4

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

#include "ggg00.h"
#include "vsp002.h"  // PASCAL: IO Types
#include "hkb57.h"   // PASCAL: kb57GetUSMRoot, kb57MigrateRestartRecord
#include "hgg08.h"   // PASCAL: g08config
#include "heo54k.h"  // RTE   : Vattach/Vdetach/Vblockio/Vvectorio
#include "heo58.h"   // RTE   : vget_devsize
#include "heo79_1.h" // RTE   : vxparam_save_good_config

#include "Converter/Converter_IManager.hpp"
#include "Converter/Converter_IMigration.hpp"
#include "Converter/Converter_IPageNoMapping.hpp"

#include "DataAccess/Data_BasePage.hpp"

#include "FreeBlockManagement/FBM_IManager.hpp"

#include "IOManager/IOMan_Exception.hpp"
#include "IOManager/IOMan_Messages.hpp"
#include "IOManager/IOMan_Migration.hpp"
#include "IOManager/IOMan_ReservedBlockAddress.hpp"
#include "IOManager/IOMan_DataArea.hpp"
#include "IOManager/IOMan_LogArea.hpp"
#include "IOManager/IOMan_Manager.hpp"

#include "KernelCommon/Kernel_FileIO.hpp"
#include "KernelCommon/Kernel_RestartPage.hpp"

#include "Logging/Log_Volume.hpp"
#include "Logging/Log_Types.hpp"

#include "RunTime/RTE_Crash.hpp"
#include "RunTime/RTE_Message.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"

#include "SAPDBCommon/SAPDB_ToString.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"

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

IOMan_Migration::IOMan_Migration(
    RTETask_ITask&              task,
    IOMan_PageFrameAllocator&   pageAllocator,
    SAPDBMem_IRawAllocator&     allocator )
        :
        m_Task( task ),
        m_PageAllocator(),
        m_Frame(),
        m_DataDevSpaces( allocator ),
        m_PageNoList( allocator ),
        m_BlockList( allocator ),
        m_UsedDataVolumes( 0 ),
        m_MaxDataPages( 0 ),
        m_ConverterVersion(),
        m_UsmRoot()
{
    m_SysDevSpace.pddLogicalDevNo  = 1;
    m_SysDevSpace.pddPhysicalDevNo = 0;
    m_SysDevSpace.pddCapacity      = 0;
    m_SysDevSpace.pddOnline        = false;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
IOMan_Migration::Initialize( const SAPDB_Int    maxDataDevSpaces )
{
    m_Frame = m_PageAllocator.New( m_Task.ID());

    if(! m_DataDevSpaces.Resize( maxDataDevSpaces ))
        return( false );

    DataDevSpaceIterator        iter    = m_DataDevSpaces.Begin();
    DataDevSpaceConstIterator   endIter = m_DataDevSpaces.End();

    for( ; iter != endIter; ++iter )
    {
        ( *iter ).pddLogicalDevNo  = m_DataDevSpaces.GetIndex( iter ) +1; // logical Id starts with 1
        ( *iter ).pddPhysicalDevNo = 0;
        ( *iter ).pddCapacity      = 0;
        ( *iter ).pddOnline        = false;
    }

    if( ! m_PageNoList.Reserve( maxDataDevSpaces + 1 )) // n * IOMan_InfoPage + RestartRecord
        return( false );

    if( ! m_BlockList.Reserve( maxDataDevSpaces + 1 )) // n * IOMan_InfoPage + RestartRecord
        return( false );

    return( true );
}

/*---------------------------------------------------------------------------*/

IOMan_Migration::~IOMan_Migration()
{
    if( m_Frame.IsAssigned() )
        m_PageAllocator.Free( m_Task.ID(), m_Frame );

    m_DataDevSpaces.Delete();
    m_PageNoList.Delete();
    m_BlockList.Delete();
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
IOMan_Migration::IsMigrationNeeded()
{
    IOMan_BlockCount    capacity = 0;
    SAPDB_Bool1         bOkay    = SAPDB_TRUE;

    // If the system devspace is available a migration is needed
    vget_devsize( tsp2_dev_type::fromConst( sp2dt_sys ), 1, capacity, bOkay );
    return SAPDB_TRUE == bOkay;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
IOMan_Migration::ExecuteMigration(
    IOMan_DataArea  &dataArea,
    IOMan_LogArea   &logArea )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_Migration::ExecuteMigration", IOMan_Trace, 5 );

    SAPDBERR_ASSERT_STATE( IsMigrationNeeded() );

    IOMan_Exception errMsg( __CONTEXT__, IOMAN_INFO_DO_MIGRATION );
    RTE_Message( errMsg );

    OpenSystemAndDataDevSpaces();

    if( ! RestartFBM())
    {
        IOMan_Exception errMsg( __CONTEXT__, IOMAN_ERR_MIG_COULD_NOT_START_FBM );
        RTE_Crash( errMsg );
    }

    if( ! MigrateRestartRecord() )
    {
        CloseSystemAndDataDevSpaces();
        ShutdownFBM();
        return( false );
    }

    Converter_IManager::Instance().CreateForRestore( m_Task,
            m_ConverterVersion, false, m_MaxDataPages-1, 0, m_MaxDataPages);

    MigrateConverter();

    DisplaceDataPages();

    CloseSystemAndDataDevSpaces();

    // Note that the converter and FBM main structures are still active
    // This is necessary for the subsequent converter flush

    // THIS IS THE POINT OF NO RETURN -- IF AN ERROR OCCUR AN
    // INPLACE MIGRATION IS NOT POSSIBLE ANYMORE

    if( ! dataArea.CreateAll( m_Task, k57DBIdentifier() ))
    {
        IOMan_Exception errMsg( __CONTEXT__, IOMAN_ERR_MIG_DATA_FAILED );
        RTE_Crash( errMsg );
    }

    FlushAndShutdownConverter();

    k57save_restartrecord( m_Task.ID());

    dataArea.CloseAll( m_Task );

    ShutdownFBM();

    RemoveSystemDevspace();

    CreateAndFormatLog( logArea );

    {
        IOMan_Exception errMsg( __CONTEXT__, IOMAN_INFO_MIGRATION_FINISHED );
        RTE_Message( errMsg );
    }
    return( true );
}

/*----------------------------- Private methods -----------------------------*/

void
IOMan_Migration::OpenSystemAndDataDevSpaces()
{
    OpenDevSpace( m_SysDevSpace, tsp2_dev_type::fromConst( sp2dt_sys ));

    SAPDB_Bool1                 bOkay   = SAPDB_TRUE;
    DataDevSpaceIterator        iter    = m_DataDevSpaces.Begin();
    DataDevSpaceConstIterator   endIter = m_DataDevSpaces.End();

    for( ; iter != endIter; ++iter )
    {
        vget_devsize( sp2dt_data, ( *iter ).pddLogicalDevNo,( *iter ).pddCapacity, bOkay );

        if( SAPDB_TRUE != bOkay )
            break;

        OpenDevSpace( ( *iter ), tsp2_dev_type::fromConst( sp2dt_data ));

        m_UsedDataVolumes++;
        m_MaxDataPages += ( *iter ).pddCapacity -1; // -1 caused by the adaption of logical/physical volume size
    }
}

/*---------------------------------------------------------------------------*/

void
IOMan_Migration::CloseSystemAndDataDevSpaces()
{

    CloseDevSpace( m_SysDevSpace );

    DataDevSpaceIterator        iter    = m_DataDevSpaces.Begin();
    DataDevSpaceConstIterator   endIter = m_DataDevSpaces.End();

    for( ; iter != endIter; ++iter )
    {
        CloseDevSpace( ( *iter ) );
    }
    m_UsedDataVolumes = 0;
    m_MaxDataPages    = 0;
}

/*---------------------------------------------------------------------------*/

void
IOMan_Migration::OpenDevSpace(
    DevSpace                &devSpace,
    const tsp2_dev_type     type )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_Migration::OpenDevSpace", IOMan_Trace, 5 );

    SAPDB_Bool1     bOkay = SAPDB_TRUE;
    tsp00_ErrText   errText;

    vattach( devSpace.pddLogicalDevNo, type, devSpace.pddPhysicalDevNo, m_Task.ID(), errText, bOkay );

    if ( SAPDB_TRUE != bOkay )
    {
        IOMan_Exception errMsg( __CONTEXT__, IOMAN_ERR_MIG_ATTACH_FAILED,
                                SAPDB_ToString( devSpace.pddLogicalDevNo, _T_d ),
                                SAPDB_ToString( errText, errText.length(), errText.length()));

        RTE_Crash( errMsg );
    }

    devSpace.pddOnline = true;
}

/*---------------------------------------------------------------------------*/

void
IOMan_Migration::CloseDevSpace( DevSpace &devSpace )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_Migration::CloseDevSpace", IOMan_Trace, 5 );

    if( devSpace.pddOnline )
    {
        devSpace.pddOnline   = false;
        devSpace.pddCapacity = 0;

        vdetach( devSpace.pddPhysicalDevNo, m_Task.ID());

        devSpace.pddPhysicalDevNo = 0;
    }
}
/*---------------------------------------------------------------------------*/

bool
IOMan_Migration::RestartFBM()
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_Migration::RestartFBM", IOMan_Trace, 5 );

    const IOMan_ReservedBlockAddress  reservedBlocks;
    const IOMan_BlockAddress          restartPageBlock( 1,1 ); // fixed position
    const IOMan_DeviceNo              maxDataDevSpaces = m_DataDevSpaces.GetCapacity();

    if( FBM_Okay != FBM_IManager::Instance().Restart( m_Task.ID(), maxDataDevSpaces, reservedBlocks,
                                   IOMan_Manager::GetInstance().GetDataIOBlockCount(),
                                   IOMan_Manager::ReadDataVolumeGroups()))
    {
        return( false );
    }

    DataDevSpaceIterator        iter    = m_DataDevSpaces.Begin();
    DataDevSpaceConstIterator   endIter = m_DataDevSpaces.End();

    for( iter = m_DataDevSpaces.Begin(); iter != endIter; ++iter )
    {
        if( ( *iter ).pddOnline )
        {
            if( ! FBM_IManager::Instance().RegisterVolume( m_Task.ID(),
                    ( *iter ).pddLogicalDevNo, ( *iter ).pddCapacity -1 , // -1, capacity without rteInfoPage
                    RTE_VolumeAccessModeNormal, restartPageBlock, 0 ))
            {
                return( false );
            }
        }
    }
    return( true );
}

/*---------------------------------------------------------------------------*/

void
IOMan_Migration::ShutdownFBM()
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_Migration::ShutdownFBM", IOMan_Trace, 5 );

    FBM_IManager::Instance().Shutdown( m_Task.ID());
}

/*---------------------------------------------------------------------------*/

void
IOMan_Migration::FlushAndShutdownConverter()
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_Migration::FlushAndShutdownConverter", IOMan_Trace, 5 );

    Converter_Version         converterVersion;
    Data_PageNo               maxStaticPageNo;
    Data_PageNo               maxDynamicPageNo;
    IOMan_PackedBlockAddress  packedRootBlockAddress;

    Converter_IManager::Instance().FlushSerial( m_Task, converterVersion,
            maxStaticPageNo, maxDynamicPageNo,  packedRootBlockAddress );

    kb57SetRootBlockAddr( packedRootBlockAddress );
    kb57SetMaxDynamicPageNo( maxDynamicPageNo );
    kb57SetMaxStaticPageNo( maxStaticPageNo );

    Converter_IManager::Instance().Shutdown( m_Task );
}

/*---------------------------------------------------------------------------*/

void
IOMan_Migration::RemoveSystemDevspace() const
{

    tsp00_ErrText       errText;
    tsp00_XpReturnCode  retCode;

    if( SAPDB_TRUE == vxparam_delete_sysdev( errText ))
    {
        vxparam_save_good_config( true, errText, retCode  );
        if( xp_ok == retCode )
            return;
    }
    IOMan_Exception errMsg( __CONTEXT__, IOMAN_ERR_MIG_PARAM_FAILED,
                            SAPDB_ToString( errText, errText.length(), errText.length()));

    RTE_Crash( errMsg );
}

/*---------------------------------------------------------------------------*/

void
IOMan_Migration::CreateAndFormatLog( IOMan_LogArea   &logArea )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_Migration::CreateAndFormatLog", IOMan_Trace, 5 );

    if( ! logArea.CreateAll( m_Task.ID(), k57DBIdentifier() ))
    {
        IOMan_Exception errMsg( __CONTEXT__, IOMAN_ERR_MIG_LOG_FAILED );
        RTE_Crash( errMsg );
    }

    const IOMan_BlockCount  logSize = logArea.TotalUsableLogPages() -1;

    const Log_ErrorState retCode = Log_Volume::Instance().CreateForNewDatabase(m_Task.ID());

    if( Log_Ok != retCode )
    {
        Log_Volume::Instance().WriteDeviceParametersToTrace();

        IOMan_Exception errMsg( __CONTEXT__, IOMAN_ERR_MIG_LOG_FORMAT_FAILED,
                                Log_ErrorStateStrings[ retCode ] );

        RTE_Crash( errMsg );

    }
    Log_Volume::Instance().CloseLogArea( m_Task.ID());
}

/*---------------------------------------------------------------------------*/

void
IOMan_Migration::ReadOldConverterPage(
    const Data_PageNo convPageNo,
    Converter_Page72  &converterPage )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_Migration::ReadOldConverterPage", IOMan_Trace, 5 );

    const SAPDB_Int4    SYS_DEV_BLOCK_CNT_BD01 = 8;
    const SAPDB_Int4    OF_CONV_0_BD15         = 3;
    const SAPDB_Int4    systemDevSpaceSlot     = convPageNo / SYS_DEV_BLOCK_CNT_BD01;
    IOMan_BlockNo       blockNo;

    if( 0 == ( m_ConverterVersion % 2 ))
        blockNo = systemDevSpaceSlot * SYS_DEV_BLOCK_CNT_BD01 +
                  OF_CONV_0_BD15 + convPageNo;
    else
        blockNo = systemDevSpaceSlot * SYS_DEV_BLOCK_CNT_BD01 +
                  OF_CONV_0_BD15 + convPageNo + SYS_DEV_BLOCK_CNT_BD01;

    ReadPageFromDevSpace( ( SAPDB_Char* )"System", m_SysDevSpace,
                          converterPage, convPageNo, blockNo );
}

/*---------------------------------------------------------------------------*/

void
IOMan_Migration::ReadPageFromDevSpace(
    SAPDB_Char             *devSpaceType,
    DevSpace                &devSpace,
    Kernel_Page72           &page,
    const Data_PageNo       pageNo,
    const IOMan_BlockNo     blockNo,
    const IOMan_BlockCount  blockCount )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_Migration::ReadPageFromSysDevSpace", IOMan_Trace, 5 );

    if(! devSpace.pddOnline )
    {
        IOMan_Exception errMsg( __CONTEXT__, IOMAN_WRN_VOLUME_OFFLINE,
                                devSpaceType, SAPDB_ToString( devSpace.pddLogicalDevNo, _T_d ));
        RTE_Crash( errMsg );
    }

    SAPDB_Bool1             bOkay = SAPDB_TRUE;
    tsp00_ErrText           errText;
    const SAPDB_Int4        maxRetries = 3;
    const SAPDB_Int4        sleepTime  = 2;
    const tsp00_PageAddr    pFrame = reinterpret_cast<tsp00_PageAddr> ( page.GetFramePtr() );

    for( SAPDB_UInt readCount = 1; maxRetries >= readCount; ++readCount )
    {
        vblockio( devSpace.pddPhysicalDevNo, blockNo,
                  tsp2_io_op_code::fromConst( sp2ioc_read ),
                  pFrame, blockCount, m_Task.ID(), errText, bOkay );

        if( SAPDB_TRUE != bOkay )
        {
            IOMan_Exception errMsg( __CONTEXT__, IOMAN_NOT_ACCESSIBLE,
                                    devSpaceType, SAPDB_ToString( devSpace.pddLogicalDevNo, _T_d ),
                                    SAPDB_ToString( blockNo, _T_d ),
                                    SAPDB_ToString( errText, errText.length(), errText.length()));
            RTE_Crash( errMsg );
        }

        if( page.CheckAfterRead() ) // major security check is alright
        {
            if( ! pageNo.IsValid())
                return; // not possible to check the page no => everything is okay

            if( pageNo == page.GetPageID())
                return; // page no check is possible => everthing is okay
        }

        IOMan_Exception errMsg( __CONTEXT__, IOMAN_BAD_PAGE,
                                devSpaceType, SAPDB_ToString( devSpace.pddLogicalDevNo, _T_d ),
                                SAPDB_ToString( blockNo, _T_d ));

        Kernel_FileIO   dumpFile( devSpaceType, KERNEL_BAD_FILE_EXTENSION,
                                  SAPDB_ToString( devSpace.pddLogicalDevNo, _T_d ),
                                  SAPDB_ToString( blockNo, _T_d ));

        dumpFile.Write( page );
        dumpFile.Close();

        RTE_Message( errMsg );
        m_Task.Sleep( sleepTime );
    }

    IOMan_Exception errMsg( __CONTEXT__, IOMAN_ERR_READ_PAGE_FAILED,
                            devSpaceType, SAPDB_ToString( devSpace.pddLogicalDevNo, _T_d ),
                            SAPDB_ToString( blockNo, _T_d ));
    RTE_Crash( errMsg );
}

/*---------------------------------------------------------------------------*/

void
IOMan_Migration::WritePageToDevSpace(
    SAPDB_Char             *devSpaceType,
    DevSpace                &devSpace,
    Kernel_Page72           &page,
    const Data_PageNo       pageNo,
    const IOMan_BlockNo     blockNo,
    const IOMan_BlockCount  blockCount )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_Migration::WritePageToDevSpace", IOMan_Trace, 5 );

    if(! devSpace.pddOnline )
    {
        IOMan_Exception errMsg( __CONTEXT__, IOMAN_WRN_VOLUME_OFFLINE,
                                devSpaceType, SAPDB_ToString( devSpace.pddLogicalDevNo, _T_d ));
        RTE_Crash( errMsg );
    }

    if(! page.PrepareForWrite() )
    {
        IOMan_Exception errMsg( __CONTEXT__, IOMAN_BAD_OUTPUT_PAGE,
                                devSpaceType, SAPDB_ToString( devSpace.pddLogicalDevNo, _T_d ),
                                SAPDB_ToString( blockNo, _T_d ));

        Kernel_FileIO   dumpFile( devSpaceType, KERNEL_BAD_FILE_EXTENSION,
                                  SAPDB_ToString( devSpace.pddLogicalDevNo, _T_d ),
                                  SAPDB_ToString( blockNo, _T_d ));

        dumpFile.Write( page );
        dumpFile.Close();

        RTE_Crash( errMsg );
    }
    {
        SAPDB_Bool1             bOkay = SAPDB_TRUE;
        tsp00_ErrText           errText;
        const tsp00_PageAddr    pFrame = reinterpret_cast<tsp00_PageAddr> ( page.GetFramePtr() );

        vblockio( devSpace.pddPhysicalDevNo, blockNo, tsp2_io_op_code::fromConst( sp2ioc_write ),
                  pFrame, blockCount, m_Task.ID(), errText, bOkay );

        if( SAPDB_TRUE != bOkay )
        {
            IOMan_Exception errMsg( __CONTEXT__, IOMAN_NOT_ACCESSIBLE,
                                    devSpaceType, SAPDB_ToString( devSpace.pddLogicalDevNo, _T_d ),
                                    SAPDB_ToString( blockNo, _T_d ),
                                    SAPDB_ToString( errText, errText.length(), errText.length()));
            RTE_Crash( errMsg );
        }
    }

}

/*---------------------------------------------------------------------------*/

void
IOMan_Migration::DisplaceDataPages()
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_Migration::DisplaceDataPages", IOMan_Trace, 5 );


    BlockListConstIterator  blockIter     = m_BlockList.Begin();
    PageNoListConstIterator pageNoIter    = m_PageNoList.Begin();
    PageNoListConstIterator pageNoEndIter = m_PageNoList.End();

    for( ; pageNoIter != pageNoEndIter; ++pageNoIter, ++blockIter )
    {
        const Data_PageNo           pageNo        = *pageNoIter;
        const IOMan_BlockAddress    oldBlock      = *blockIter;
        const SAPDB_UInt4           devSpaceIndex = oldBlock.GetDeviceNo() -1;
        Data_BasePage               page( m_Frame );


        SAPDBERR_ASSERT_STATE( oldBlock.GetDeviceNo() ==
                               m_DataDevSpaces[ devSpaceIndex ].pddLogicalDevNo  );

        ReadPageFromDevSpace( ( SAPDB_Char* )"Data", m_DataDevSpaces[ devSpaceIndex ],
                              page, pageNo, oldBlock.GetBlockNo() );

        const IOMan_BlockAddress block =
            Converter_IMigration::Instance().DetermineAndSetBlockAddressForMigratedPage(
                m_Task, pageNo );

        WritePageToDevSpace( ( SAPDB_Char* )"Data",
                             m_DataDevSpaces[ block.GetDeviceNo()-1 ],
                             page, pageNo, block.GetBlockNo() );
    }
    m_PageNoList.Clear();
    m_BlockList.Clear();
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
IOMan_Migration::MigrateRestartRecord()
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_Migration::MigrateRestartRecord", IOMan_Trace, 5 );

    const SAPDB_Int4    restartRecordOffset = 2;
    Data_PageNo         restartPageNo;
    Kernel_RestartPage  restartPage( m_Frame );

    ReadPageFromDevSpace( ( SAPDB_Char* )"System", m_SysDevSpace, restartPage,
                          restartPageNo, restartRecordOffset );

    const tsp00_PageAddr pOldRestartRec = reinterpret_cast<tsp00_PageAddr> ( restartPage.GetFramePtr() );
    const SAPDB_UInt4 maxDataPageNo     = kb57GetMaxDataPageNo( pOldRestartRec );
    m_ConverterVersion                  = kb57GetOldConverterVersion( pOldRestartRec );
    m_UsmRoot                           = kb57GetUSMRoot( pOldRestartRec );

    if( ! kb57IsMigrationFlagSet( pOldRestartRec ))
    {
        IOMan_Exception errMsg( __CONTEXT__, IOMAN_ERR_NOT_PREPARED_FOR_MIG );

        RTE_Message( errMsg );
        return( false );
    }

    if( maxDataPageNo != m_MaxDataPages - 1 )
    {
        IOMan_Exception errMsg( __CONTEXT__, IOMAN_ERR_ILLEGAL_DATA_AREA_SIZE,
                                SAPDB_ToString( maxDataPageNo, _T_d ),
                                SAPDB_ToString( m_MaxDataPages -1, _T_d ));
        RTE_Crash( errMsg );
    }

    if( ! kb57MigrateRestartRecord( pOldRestartRec ))
    {
        IOMan_Exception errMsg( __CONTEXT__, IOMAN_ERR_MIG_RESTARTREC_FAILED );

        RTE_Crash( errMsg );
    }

    kb57RestoreRestartRecordAfterMigration( pOldRestartRec ); // write into k57restartrec
    return( true );
}

/*---------------------------------------------------------------------------*/

void
IOMan_Migration::MigrateConverter()
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_Migration::MigrateConverter", IOMan_Trace, 5 );

    const SAPDB_UInt            reportCount   = 5000;
    const SAPDB_Int4            maxConvPageNo = ( m_MaxDataPages -1 )/Converter_Page72::GetEntryCount();
    SAPDB_UInt4                 numDataPages  = 0;

    Converter_Page72            page( m_Frame );
    IOMan_ReservedBlockAddress  reservedBlocks;
    const IOMan_BlockAddress    restartPageBlock( 1,1 ); // fixed position

    for( Data_PageNo convPageNo = 0; convPageNo <= maxConvPageNo; convPageNo++ )
    {
        ReadOldConverterPage( convPageNo, page );

        const Converter_Page72EntryIterator pageEntryIterEnd = page.End();

        for( Converter_Page72EntryIterator pageEntryIter = page.Begin();
                pageEntryIter != pageEntryIterEnd; ++pageEntryIter )
        {
            if( pageEntryIter.IsPermanentUsed())
            {
                const Data_PageNo   dataPageNo = pageEntryIter.GetDataPageNo();

                if( m_UsmRoot == dataPageNo )
                    continue; // no USM available in 7.4

                const IOMan_BlockAddress block = pageEntryIter.GetBlockAddress();

                SAPDBERR_ASSERT_STATE( block.IsValid() );

                ++numDataPages;

                if( reservedBlocks.IsMember( block ) || restartPageBlock == block )
                {
                    // store the page id, because the page has to be displaced
                    // before the restart record or IOMan_Info page will be created
                    m_PageNoList.InsertEnd( dataPageNo );
                    m_BlockList.InsertEnd( block );
                }
                else
                {   // Only insert page id's into converter and FBM, which are
                    // not stored in the reserved area of a data devspace
                    Converter_IMigration::Instance().SetBlockAddressForMigratedPage(
                        m_Task, block, dataPageNo );
                }
            }
        }
        if(( 0 == ( convPageNo % reportCount )) && ( 0 < convPageNo )) // Report the state of migration
        {
            IOMan_Exception errMsg( __CONTEXT__, IOMAN_INFO_CONV_PAGES_MIGRATED,
                                    SAPDB_ToString( convPageNo, _T_d ), SAPDB_ToString( numDataPages, _T_d ));

            RTE_Message( errMsg );
        }

    }
    // ready message
    {
        IOMan_Exception errMsg( __CONTEXT__, IOMAN_INFO_CONV_PAGES_MIGRATED,
                                SAPDB_ToString( maxConvPageNo, _T_d ), SAPDB_ToString( numDataPages, _T_d ));

        RTE_Message( errMsg );
    }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
