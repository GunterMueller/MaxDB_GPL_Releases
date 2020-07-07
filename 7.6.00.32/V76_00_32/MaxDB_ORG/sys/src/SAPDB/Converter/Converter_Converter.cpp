/*!
  @file           Converter_Converter.cpp
  @author         TorstenS
  @author         AlexanderK
  @ingroup        Converter
  @brief          Implementation of all converter methods
*/

/*
  ========== licence begin  GPL
  Copyright (c) 2001-2004 SAP AG

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
  ========== licence end
*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "hgg01.h"    // g01is_archive
#include "hgg08.h"    // g08convert0
#include "hbd20_6.h"  // bd20GetNumberOfChangedPages
#include "hkb57.h"    // kb57GetRestartRecordKernelVersion

#include "Converter/Converter_Converter.hpp"
#include "Converter/Converter_Dump.hpp"
#include "Converter/Converter_LeafPage.hpp"
#include "Converter/Converter_Exception.hpp"
#include "Converter/Converter_MapError.hpp"
#include "Converter/Converter_Messages.hpp"
#include "Converter/Converter_PageFrameAllocator.hpp"
#include "Converter/Converter_Pager.hpp"
#include "Converter/Converter_BlockAddressIterator.hpp"
#include "Converter/Converter_DBSpaceStatistic.hpp"
#include "FrameControl/FrameCtrl_ICommon.hpp"
#include "GarbageCollector/GC_IGCController.hpp"
#include "IOManager/IOMan_IDataInfo.hpp"
#include "IOManager/IOMan_IDataManager.hpp"
#include "Pager/Pager_IRestart.hpp"
#include "Pager/Pager_ISavepoint.hpp"
#include "Logging/Log_Types.hpp"
#include "KernelCommon/Kernel_IAdminRestartShutdown.hpp"
#include "KernelCommon/Kernel_IAdminConfig.hpp"
#include "KernelCommon/ParameterNames/KernelParam_Converter.hpp"
#include "KernelCommon/Kernel_Migration.hpp"
#include "KernelCommon/Kernel_Version.hpp"
#include "KernelCommon/Kernel_FileIO.hpp"
#include "KernelCommon/Kernel_Dump.hpp"
#include "KernelCommon/DatabaseEvents/KernelDBE_DBFull.hpp"
#include "Messages/Msg_List.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "RunTime/RTE_Message.hpp"
#include "RunTime/RTE_Crash.hpp"
#include "RunTime/Configuration/RTEConf_ParameterAccess.hpp"
#include "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp"
#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include "RunTime/System/RTESys_MicroTime.h"
#include "RunTime/RTE_KGS.hpp"
#include "Transaction/Trans_Context.hpp"
#include "SAPDBCommon/Messages/SDBMsg_Converter.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/// Defines security space for database full handling
#define CONV_NUM_SECURITY_PAGES 10

/// suspend reason for db full
#define CONV_SUSPEND_HANDLE_DB_FULL                 197

/// suspend reason for
#define CONV_SUSPEND_FREE_PAGENO_WAIT_DURING_SVP    236

/// suspend reason for
#define CONV_SUSPEND_EXPAND_WAIT_DURING_SVP         237

/// suspend reason for
#define CONV_SUSPEND_DROP_VOLUME_WAIT_DURING_SVP    239


/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/

/*!
    @brief  This method is used to get the configured value for a given
            integer configuration parameter from the configuration file.
    @return (SAPDB_UInt) configuration value
 */

static SAPDB_UInt
GetConfiguredValue( const RTEConf_Parameter::Name paramName )
{
    RTEConf_Parameter::Integer  paramValue;
    Msg_List                    errMsg;

    if( ! RTEConf_ParameterAccess::Instance()->GetInteger(
                paramName, paramValue, errMsg ))
    {
        RTE_Crash( errMsg );
    }
    return( ( SAPDB_UInt ) paramValue );
}

/*!
    @brief  This method is used to write the given message list
            into the vtrace and the knldiag.
    @param  msgList [in] given message list to be written
    @return none
 */

static void
WriteMessageList( const Msg_List  &msgList )
{
    RTE_Message( msgList );

    SAPDBTrace_Stream   traceStream( &Converter_Trace.GetTracePrinter() );
    Msg_List::LongTimeBuffer timeBuffer;
    traceStream << "*** " << msgList.DateTime( timeBuffer ) << " "
    << msgList.TypeAsString() << " "
    << msgList.ID() << " "
    << msgList.Component() << " " << NewLine
    << "*** " << msgList.Message() << NewLine;
}

/*!
    @brief  This method is used to write a given converter leaf page 
            into the rundirectory of the kernel. The name of the 
            external file is build among other things with the BasePageNo
            of the given page.
    @param  leafPage [in] converter leaf page to be dumped
    @return none
*/

static void
DumpConverterPage( Converter_LeafPage   &leafPage )
{
    Kernel_FileIO   dumpPage( KERNEL_CONV_FILE, KERNEL_COR_FILE_EXTENSION,
                              SAPDB_ToString( leafPage.ReadBasePageNo(), _T_d ));

    dumpPage.Write( leafPage );
    dumpPage.Close();
}

/*===========================================================================*
 *  INSTANCE METHODS OF THE INTERFACE CLASSES  OF THE CONVERTER CLASS        *
 *===========================================================================*/

Converter_IManager & Converter_IManager::Instance()
{
    return Converter_Converter::Instance();
}

/*---------------------------------------------------------------------------*/

Converter_IMigration & Converter_IMigration::Instance()
{
    return Converter_Converter::Instance();
}

/*---------------------------------------------------------------------------*/

Converter_IDiagnose & Converter_IDiagnose::Instance()
{
    return Converter_Converter::Instance();
}

/*---------------------------------------------------------------------------*/

Converter_IDump & Converter_IDump::Instance()
{
    return Converter_Converter::Instance();
}

/*---------------------------------------------------------------------------*/

Converter_IPageIO & Converter_IPageIO::Instance()
{
    return Converter_Converter::Instance();
}

/*---------------------------------------------------------------------------*/

Converter_ISavePoint & Converter_ISavePoint::Instance()
{
    return Converter_Converter::Instance();
}

/*---------------------------------------------------------------------------*/

Converter_IDropVolume & Converter_IDropVolume::Instance()
{
    return Converter_Converter::Instance();
}

/*---------------------------------------------------------------------------*/

Converter_IVerify & Converter_IVerify::Instance()
{
    return Converter_Converter::Instance();
}

/*---------------------------------------------------------------------------*/

Converter_ICommon & Converter_ICommon::Instance()
{
    return Converter_Converter::Instance();
}

/*---------------------------------------------------------------------------*/

Converter_IBackUp & Converter_IBackUp::Instance()
{
    return Converter_Converter::Instance();
}

/*---------------------------------------------------------------------------*/

Converter_IPageNoMapping & Converter_IPageNoMapping::Instance()
{
    return Converter_Converter::Instance();
}

/*---------------------------------------------------------------------------*/

Converter_IPageNoManager & Converter_IPageNoManager::Instance()
{
    return Converter_Converter::Instance();
}

/*---------------------------------------------------------------------------*/

Converter_IEvent & Converter_IEvent::Instance()
{
    return Converter_Converter::Instance();
}


/*===========================================================================*
 *  METHODS OF THE CONVERTER CLASS                                           *
 *===========================================================================*/

Converter_Converter* Converter_Converter::m_Instance = NULL;

/*---------------------------------------------------------------------------*/

Converter_Converter::Converter_Converter( SAPDBMem_IRawAllocator  &allocator )
        :
        m_Allocator( UTF8( "Converter_Manager" ), allocator),
        m_MapBlockingSize( 8 ),
        m_VolumeNoBitCount( GetConfiguredValue( UTF8( KERNELPARAM_VOLUMENO_BIT_COUNT ))),
        m_NumRegions( GetConfiguredValue( UTF8( KERNELPARAM_CONVERTER_REGIONS ))),
        m_BaseRegionId( g08convert0 ),
        m_PageSize( FrameCtrl_ICommon::GetInstance().GetPageSize()),
        m_TempMapControl( m_Allocator, Data_PageRecoveryMode( Data_Dynamic, Data_NotRecoverable )),
        m_PermMapControl( m_Allocator, Data_PageRecoveryMode( Data_Dynamic, Data_Recoverable )),
        m_PermStaticMapControl( m_Allocator, Data_PageRecoveryMode( Data_Static, Data_Recoverable )),
        m_Index (m_Allocator),
        m_Active( false ),
        m_InMaintenanceMode( false ),
        m_DBFullWaitQueue( CONV_SUSPEND_HANDLE_DB_FULL ),
        m_FreePageNoWaitQueue( CONV_SUSPEND_FREE_PAGENO_WAIT_DURING_SVP ),
        m_ExpandConverterWaitQueue( CONV_SUSPEND_EXPAND_WAIT_DURING_SVP ),
        m_DBUsageMonitor (sp31ei_db_filling_above_limit, sp31ei_db_filling_below_limit),
        m_VolumeList(NULL)
{
    // initialize static member of the converter page defining
    // the number of bits used to address a volume

    Converter_Page::SetDevNoBitCount( m_VolumeNoBitCount );

    // the exceeding of the following limits will always be written
    // into the knldiag. all values are defined in units of 0.01 %

    m_DBUsageMonitor.EnableEventing();
}

/*---------------------------------------------------------------------------*/

Converter_Converter&
Converter_Converter::CreateInstance ()
{
    SAPDBTRACE_ROUTINE_DEBUG( "Converter::CreateInstance", Converter_Trace, 5 );

    if( m_Instance == NULL )
    {
        m_Instance = new (RTEMem_Allocator::Instance())
                     Converter_Converter( RTEMem_Allocator::Instance() );

        if( NULL == m_Instance )
        {
            Converter_Exception errMsg( __CONTEXT__, CONVERTER_NO_MEMORY_TO_BUILD_CONVERTER );
            RTE_Crash( errMsg );
        }
    }
    return *m_Instance;
}
/*---------------------------------------------------------------------------*/

Converter_ReturnCode
Converter_Converter::RestartForRestore(
    const RTETask_ITask             &task,
    const Converter_Version         &ConverterVersion,
    const IOMan_PackedBlockAddress  &packedRootBlockAddr,
    const Converter_Version         &snapShotVersion,
    const IOMan_PackedBlockAddress  &packedSnapShotRootBlockAddr,
    const Kernel_DateTime           &snapShotCreationTimeStamp,
    const SAPDB_Bool                bLastSaveDataSuccessful,
    const Data_PageNo               &MaxDynamicPageNo,
    const Data_PageNo               &MaxStaticPageNo,
    const SAPDB_UInt                NumAllPages,
    const SAPDB_UInt                volumeNoBitCount )
{
    SAPDBTRACE_ROUTINE_DEBUG( "Converter::RestartForRestore", Converter_Trace, 5 );

    if( m_Active )
    {
        // Converter is still active, which could not happen, because
        // this should be synchcronized by the kernel administration class
        // but beware of ADMIN Select's!

        Converter_Exception errMsg( __CONTEXT__, CONVERTER_WRN_NOT_RESTARTABLE2 );
        RTE_Message( errMsg );

        return( Converter_IncompatibleMode );
    }

    const Converter_ReturnCode retCode = Restart( task, ConverterVersion,
                                         packedRootBlockAddr, snapShotVersion,
                                         packedSnapShotRootBlockAddr, snapShotCreationTimeStamp, false,
                                         bLastSaveDataSuccessful, MaxDynamicPageNo, MaxStaticPageNo,
                                         NumAllPages, volumeNoBitCount );
    if( Converter_Ok == retCode )
    {
        m_InMaintenanceMode  = true;
        m_RestorePagesActive = true;
    }
    return( retCode );
}

/*---------------------------------------------------------------------------*/

Converter_ReturnCode
Converter_Converter::Restart(
    const RTETask_ITask             &task,
    const Converter_Version         &ConverterVersion,
    const IOMan_PackedBlockAddress  &packedRootBlockAddr,
    const Converter_Version         &snapShotVersion,
    const IOMan_PackedBlockAddress  &packedSnapShotRootBlockAddr,
    const Kernel_DateTime           &snapShotCreationTimeStamp,
    const SAPDB_Bool                bIncConverterVersion,
    const SAPDB_Bool                bLastSaveDataSuccessful,
    const Data_PageNo               &MaxDynamicPageNo,
    const Data_PageNo               &MaxStaticPageNo,
    const SAPDB_UInt                NumAllPages,
    const SAPDB_UInt                volumeNoBitCount )
{
    SAPDBTRACE_ROUTINE_DEBUG( "Converter::Restart", Converter_Trace, 5 );

    if( m_Active )
    {
        if( m_InMaintenanceMode )
        {
            // Converter is in maintenance mode, for example check data, restore etc.
            // This could not happen, because it should be sychnronized by the
            // kernel administration class, but beware of ADMIN Select's!

            Converter_Exception errMsg( __CONTEXT__, CONVERTER_WRN_NOT_RESTARTABLE );
            RTE_Message( errMsg );
            return( Converter_IncompatibleMode );
        }
        return( Converter_Ok ); // is still active
    }

    SAPDBTRACE_WRITELN( Converter_Trace, 5, "ConverterVersion      : " << ConverterVersion );
    SAPDBTRACE_WRITELN( Converter_Trace, 5, "SnapShotVersion       : " << snapShotVersion );
    SAPDBTRACE_WRITELN( Converter_Trace, 7, "LastSaveDataSuccessful: " <<
                        ( bLastSaveDataSuccessful ? "yes" : "no" ));
    SAPDBTRACE_WRITELN( Converter_Trace, 7, "MaxDynamicPageNo      : " << MaxDynamicPageNo );
    SAPDBTRACE_WRITELN( Converter_Trace, 7, "MaxStaticPageNo       : " << MaxStaticPageNo );
    SAPDBTRACE_WRITELN( Converter_Trace, 7, "NumAllPages           : " << NumAllPages );


    CheckAndMigrateConfigurationParameter( volumeNoBitCount );

    if(
        ( ! m_TempMapControl.Initialize( task, NumAllPages,
                                         m_BaseRegionId, m_NumRegions, m_MapBlockingSize, m_PageSize ))
        ||
        ( ! m_PermMapControl.Initialize( task, SAPDB_MAX( MaxDynamicPageNo, Data_PageNo( NumAllPages )),
                                         m_BaseRegionId, m_NumRegions, m_MapBlockingSize, m_PageSize,
                                         bLastSaveDataSuccessful ))
        ||
        (! m_PermStaticMapControl.Initialize( task, SAPDB_MAX( MaxStaticPageNo, Data_PageNo( NumAllPages )),
                                              m_BaseRegionId, m_NumRegions, m_MapBlockingSize, m_PageSize,
                                              bLastSaveDataSuccessful ))
    )
    {
        Converter_Exception errMsg( __CONTEXT__, CONVERTER_MAP_INIT_FAILED1,
                                    SAPDB_ToString( NumAllPages,      _T_d ),
                                    SAPDB_ToString( MaxDynamicPageNo, _T_d ),
                                    SAPDB_ToString( MaxStaticPageNo,  _T_d ));
        RTE_Crash( errMsg );
    }

    InitializeBase( NumAllPages );

    m_Version                   = ConverterVersion;
    m_RootBlockAddress          = packedRootBlockAddr.Unpack( GetVolumeNoBitCount());
    m_SnapShotVersion           = snapShotVersion;
    m_SnapShotCreationTimeStamp = snapShotCreationTimeStamp;

    /* the parent index is created starting with the data stored on the ConverterRootBlock */
    Converter_Pager pager( task, m_SnapShotVersion );

    m_Index.Restart( m_RootBlockAddress, pager );

    /* create per device lists of all converter leaf nodes */
    VolumeList  volumeList( m_Allocator );

    m_Index.CreateInputQueues(volumeList, m_PageSize);

    m_VolumeList = &volumeList; // is used by the pager tasks

    /* read all converter leaf pages referenced by the converter tree */
    Pager_IRestart::GetInstance().ReadConverter( task.ID() );

    m_VolumeList = NULL;

    volumeList.Delete(); // free occupied memory

    SAPDBERR_ASSERT_STATE( m_Index.Verify()); // PTS 1113242 TS 2001-12-21

    // Caused by the FBM handling it is necessary that
    // the current/working converter is read first.

    if( m_SnapShotVersion.IsValid()){
        RestartSnapShot( task, packedSnapShotRootBlockAddr );
    }

    if( bIncConverterVersion )
        ++m_Version;
    else
        m_InMaintenanceMode = true;

    CheckDataAreaPeakFilling();

    SAPDBTRACE_WRITELN( Converter_Trace, 5, "PermPagesUsed: " << m_NumPermPagesUsed );
    SAPDBTRACE_WRITELN( Converter_Trace, 5, "UpdatedDynamicPages: " <<
                        m_PermMapControl.NumUpdatedPermPages( task ));
    SAPDBTRACE_WRITELN( Converter_Trace, 5, "UpdatedStaticPages: " <<
                        m_PermStaticMapControl.NumUpdatedPermPages( task ));
    return( Converter_Ok );
}

/*---------------------------------------------------------------------------*/

void
Converter_Converter::ReadPages(
    const RTETask_ITask &task,
    SAPDB_UInt          pagerIndex,
    SAPDB_UInt          &totalPages,
    SAPDB_UInt          &totalIO )
{

    SAPDBTRACE_ROUTINE_DEBUG( "Converter::ReadPages", Converter_Trace, 5 );

    SAPDBERR_ASSERT_STATE (NULL != m_VolumeList);

    Converter_Pager pager( task, m_SnapShotVersion );

    // loop over all converter leaf pages located in one data volume. This
    // will speed up the restart time of the converter. Check if pager tasks
    // is responsible for another data volume. This could happen, if more
    // data volumes are configured than pager tasks.

    while (pagerIndex < ((*m_VolumeList).GetSize())) // scan over data volumes
    {
        Converter_Index::BlockVector::Iterator        iter    = (*m_VolumeList)[pagerIndex].Begin();
        const Converter_Index::BlockVector::Iterator  iterEnd = (*m_VolumeList)[pagerIndex].End();

        for( ; iter < iterEnd; iter++ ) // read conv pages of one data volume
        {
            Converter_LeafPage  leafPage;

            pager.Read( iter->GetBlockAddress(), leafPage );

            totalPages++;
            totalIO++;

            const Data_PageRecoveryMode recMode  = leafPage.ReadPageRecoveryMode();
            const Data_AddressingMode   addrMode = recMode.GetAddressingMode();

            SAPDBERR_ASSERT_STATE( recMode.PageIsRecoverable());

            // write the address of the parent index page on the leaf page according the
            // iterator. This is needed for converter leaf page i/o to be able to update
            // the block address of the leaf in the corresponding converter index page.

            leafPage.WriteParentReference (iter->GetParentReference());

            // All reserved converter leaf page entries are set to free. Leaf page
            // will be embeded into converter.

            SAPDB_UInt numPermPagesUsed = 0;

            if( Data_Static == addrMode )
            {
                m_PermStaticMapControl.IntegratePage( task, leafPage, numPermPagesUsed );
            }
            else // Data_Dynamic
            {
                m_PermMapControl.IntegratePage( task, leafPage, numPermPagesUsed );
            }

            m_NumPermPagesUsed.Increment( numPermPagesUsed );

            // Mark all used blocks referenced by the current converter leaf pages as
            // occupied in the FBM

            Converter_BlockAddressIterator blockIter = leafPage.GetUsedBlockIterator();

            if( ! FBM_IManager::Instance().SetBlockStateToOccupied( task.ID(), blockIter ))
            {
                Converter_Exception errMsg( __CONTEXT__, CONVERTER_BAD_ENTRY_STATE,
                                            SAPDB_ToString( leafPage.GetPageNo( blockIter.GetPosition()), _T_d ),
                                            (( Data_Dynamic == addrMode ) ? "perm" : "static"));
                DumpConverterPage( leafPage );
                RTE_Crash( errMsg );
            }
            CheckFillingOfDataBase();
        }
        pagerIndex += g01maxdatawriter();
    }
}

/*---------------------------------------------------------------------------*/

void
Converter_Converter::ReadSnapShotPages(
    const RTETask_ITask &task,
    SAPDB_UInt          pagerIndex,
    SAPDB_UInt          &totalPages,
    SAPDB_UInt          &totalIO )
{
    SAPDBTRACE_ROUTINE_DEBUG( "Converter::ReadSnapShotPages", Converter_Trace, 5 );

    SAPDBERR_ASSERT_STATE (NULL != m_VolumeList);

    // check if Pager has something to do ..
    if ((*m_VolumeList).GetSize() <= pagerIndex){
        return;
    }

    Converter_PageFrameAllocator    frameAllocator( task );
    Converter_SnapShotPager         pager( task, m_SnapShotVersion, m_Index );
    Converter_LeafPage              leafPage;

    leafPage.SetFrame( frameAllocator.New());

    // loop over all converter leaf pages located in one data volume. This
    // will speed up the restart time of the converter. Check if pager tasks
    // is responsible for another data volume. This could happen, if more
    // data volumes are configured than pager tasks.

    while (pagerIndex < ((*m_VolumeList).GetSize())) // scan over data volumes
    {
        Converter_Index::BlockVector::Iterator        iter    = (*m_VolumeList)[pagerIndex].Begin();
        const Converter_Index::BlockVector::Iterator  iterEnd = (*m_VolumeList)[pagerIndex].End();

        for( ; iter < iterEnd; iter++ ) // read conv pages of one data volume
        {
            pager.Read( iter->GetBlockAddress(), leafPage );

            totalPages++;
            totalIO++;

            const Data_PageRecoveryMode recMode  = leafPage.ReadPageRecoveryMode();
            const Data_AddressingMode   addrMode = recMode.GetAddressingMode();

            SAPDBERR_ASSERT_STATE( recMode.PageIsRecoverable());

            // mark all used blocks referenced by the current snapshot converter
            // leaf page as occupied in the FBM. Note that the corresponding FBM
            // block could be already set to occupied!

            Converter_PageEntryIterator         pageEntryIter    = leafPage.Begin();
            const Converter_PageEntryIterator   pageEntryEndIter = leafPage.End();

            for( ; pageEntryIter != pageEntryEndIter; ++pageEntryIter )
            {
                if( ! pageEntryIter.EntryInUse())
                    continue;

                if( pageEntryIter.GetPackedBlockAddress().IsReserved())
                    continue;


                const Data_PageNo        pageNo   = leafPage.GetPageNo( pageEntryIter.GetPosition());
                const IOMan_BlockAddress newBlock = pageEntryIter.GetBlockAddress(); // from snapShot
                IOMan_BlockAddress       block;

                const Converter_ReturnCode retCode = GetPositionForReading( task, pageNo, recMode, block );

                SAPDBERR_ASSERT_STATE( newBlock.IsValid());

                IncrementSnapShotSize();

                if(( Converter_Ok == retCode ) && ( block == newBlock ))
                {
                    SAPDBTRACE_WRITELN( Converter_Trace, 7, "Snapshot: " << ((Data_Dynamic == addrMode) ?
                                        "Permanent" : "Static") << " pageNo: " << pageNo << " on block: " <<
                                        block.GetVolumeNo() << "/" << block.GetBlockNo() << " not marked as used ");

                    continue; // given page(block) hasn't changed since snapshot creation
                }

                SAPDBTRACE_WRITELN( Converter_Trace, 7, "Snapshot: " << ((Data_Dynamic == addrMode) ?
                                    "Permanent" : "Static") << " pageNo: " << pageNo << " on block: " <<
                                    newBlock.GetVolumeNo() << "/" << newBlock.GetBlockNo() << " marked as used ");

                if( FBM_IManager::Instance().SetBlockStateToOccupied( task.ID(), newBlock ))
                {
                    m_NumPermPagesUsed.Increment();
                    continue;
                }
                Converter_Exception errMsg( __CONTEXT__, CONVERTER_BAD_ENTRY_STATE,
                                            SAPDB_ToString( pageNo, _T_d ),
                                            (( Data_Dynamic == addrMode ) ?"perm" : "static"));
                DumpConverterPage( leafPage );
                RTE_Crash( errMsg );
            }
        }
        pagerIndex += g01maxdatawriter();
    }
    if( leafPage.IsAssigned()){
        frameAllocator.Free( leafPage.GetFrame());
    }
}

/*---------------------------------------------------------------------------*/

void
Converter_Converter::Create(
    const RTETask_ITask &task,
    const SAPDB_UInt    numAllPages )
{
    SAPDBTRACE_ROUTINE_DEBUG( "Converter::Create", Converter_Trace, 5 );

    SAPDBERR_ASSERT_ARGUMENT ( m_Version.IsInvalid() );

    if ( m_Version.IsValid() )
        return;

    if(
        ( ! m_TempMapControl.Initialize( task, numAllPages,
                                         m_BaseRegionId, m_NumRegions, m_MapBlockingSize, m_PageSize ))
        ||
        ( ! m_PermMapControl.Initialize( task, numAllPages,
                                         m_BaseRegionId, m_NumRegions, m_MapBlockingSize, m_PageSize, true ))
        ||
        (! m_PermStaticMapControl.Initialize( task, numAllPages,
                                              m_BaseRegionId, m_NumRegions, m_MapBlockingSize, m_PageSize, true ))
    )
    {
        Converter_Exception errMsg( __CONTEXT__, CONVERTER_MAP_INIT_FAILED2,
                                    SAPDB_ToString( numAllPages, _T_d ));
        RTE_Crash( errMsg );
    }

    InitializeBase( numAllPages );
    m_Version.SetToInitialVersion(); // corresponds to version 1

    m_Index.Create();
}

/*---------------------------------------------------------------------------*/

void
Converter_Converter::CreateForRestore(
    const RTETask_ITask     &task,
    const Converter_Version &converterVersion,
    const SAPDB_Bool        bLastSaveDataSuccessful,
    const Data_PageNo       &maxDynamicPageNo,
    const Data_PageNo       &maxStaticPageNo,
    const SAPDB_UInt        numAllPages )
{
    SAPDBTRACE_ROUTINE_DEBUG( "Converter::CreateForRestore", Converter_Trace, 5 );

    if(
        ( ! m_TempMapControl.Initialize( task, numAllPages,
                                         m_BaseRegionId, m_NumRegions, m_MapBlockingSize, m_PageSize ))
        ||
        ( ! m_PermMapControl.Initialize( task, SAPDB_MAX( maxDynamicPageNo, Data_PageNo( numAllPages )),
                                         m_BaseRegionId, m_NumRegions, m_MapBlockingSize, m_PageSize,
                                         bLastSaveDataSuccessful ))
        ||
        (! m_PermStaticMapControl.Initialize( task, SAPDB_MAX( maxStaticPageNo, Data_PageNo( numAllPages )),
                                              m_BaseRegionId, m_NumRegions, m_MapBlockingSize, m_PageSize,
                                              bLastSaveDataSuccessful ))
    )
    {
        Converter_Exception errMsg( __CONTEXT__, CONVERTER_MAP_INIT_FAILED1,
                                    SAPDB_ToString( numAllPages,      _T_d),
                                    SAPDB_ToString( maxDynamicPageNo, _T_d),
                                    SAPDB_ToString( maxStaticPageNo,  _T_d));
        RTE_Crash( errMsg );
    }

    InitializeBase( numAllPages );

    m_InMaintenanceMode      = true;
    m_RestoreDataActive      = true;
    m_Version                = converterVersion;
    m_Index.Create();
}

/*---------------------------------------------------------------------------*/

void
Converter_Converter::Shutdown( const RTETask_ITask &task )
{
    SAPDBTRACE_ROUTINE_DEBUG( "Converter::Shutdown", Converter_Trace, 5 );

    if( ! m_Active )
        return;

    m_NumPermPagesUsed           = 0;
    m_NumTempPagesUsed           = 0;
    m_NumPermConvPages           = 0;
    m_NumTempConvPages           = 0;
    m_NumSnapShotPages           = 0;
    m_TemporaryPeakFilling       = 0;
    m_FrozenTemporaryPeakFilling = 0;
    m_PermanentPeakFilling       = 0;
    m_FrozenPermanentPeakFilling = 0;
    m_NumAllPages                = 0;
    m_SecuritySpaceLimit         = 0;

    m_Active               = false;
    m_InMaintenanceMode    = false;
    m_SavePagesIsActiv     = false;
    m_SaveDataIsActiv      = false;
    m_RestoreDataActive    = false;
    m_RestorePagesActive   = false;
    m_ColdVerifyIsActive   = false;
    m_Version.Invalidate();
    m_SnapShotVersion.Invalidate();
    m_SnapShotCreationTimeStamp.Invalidate();

    /* resume all tasks waiting for a converter resource */
    m_DBFullWaitQueue.ResumeAllWaitingTasks( task.ID());
    m_FreePageNoWaitQueue.ResumeAllWaitingTasks( task.ID());

    m_TempMapControl.Shutdown( task );
    m_PermMapControl.Shutdown( task );
    m_PermStaticMapControl.Shutdown( task );

    m_Index.Shutdown();
}

/*---------------------------------------------------------------------------*/

void
Converter_Converter::FlushParallel (
    const RTETask_ITask         &task,
    Converter_Version           &ConverterVersion,
    Data_PageNo                 &MaxStaticPageNo,
    Data_PageNo                 &MaxDynamicPageNo,
    IOMan_PackedBlockAddress    &packedRootBlockAddr,
    SAPDB_UInt                  &NumPagesTotal,
    SAPDB_UInt                  &NumIOTotal)
{
    SAPDBTRACE_ROUTINE_DEBUG( "Converter::FlushParallel", Converter_Trace, 5 );

    SAPDB_UInt  NumPages = 0;
    SAPDB_UInt  NumIO    = 0;

    if( ! m_Active )
    {
        Converter_Exception errMsg( __CONTEXT__, CONVERTER_INFO_SHUTDOWN );
        RTE_Message( errMsg );

        Kernel_IAdminRestartShutdown::Instance().Offline();
    }

    // MapControls dealing with permanent pageId's remembers parentReferences
    // of empty converter leaf pages between converter flushes, now it's time
    // to drop these pages from the index

    m_PermMapControl.RemoveEmptyLeaves( task, m_SnapShotVersion, m_Index );
    m_PermStaticMapControl.RemoveEmptyLeaves( task, m_SnapShotVersion, m_Index );

    Pager_ISavepoint::GetInstance().FlushConverterParallel( task.ID(), NumPages, NumIO );

    NumPagesTotal += NumPages;
    NumIOTotal    += NumIO;

    Converter_Pager pager ( task, m_SnapShotVersion );

    MaxStaticPageNo    = m_MaxUsedStaticPageNo;
    MaxDynamicPageNo   = m_MaxUsedDynamicPageNo;

    m_RootBlockAddress = m_Index.Flush( pager, NumPages, NumIO );

    NumPagesTotal += NumPages;
    NumIOTotal    += NumIO;

    packedRootBlockAddr = m_RootBlockAddress.Pack( GetVolumeNoBitCount());;
    ConverterVersion    = m_Version;

    // Converter leaf pages are set to changed if a permanent data page is
    // removed or the block address is updated. Both operations are not
    // possible during the converter flush (savepoint) and therefore the
    // number of changed converter leaves has to be zero at the end of the
    // savepoint.

    SAPDBERR_ASSERT_STATE( 0 == GetNumberOfChangedConvPages());
}

/*---------------------------------------------------------------------------*/

void
Converter_Converter::WritePages(
    const RTETask_ITask &task,
    const SAPDB_UInt    iSection,
    SAPDB_UInt          &totalPages,
    SAPDB_UInt          &totalIO )
{
    totalPages = 0;
    totalIO    = 0;

    Converter_Pager pager( task, m_SnapShotVersion );

    m_PermMapControl.FlushSection( task, m_Index, pager, iSection, totalPages, totalIO );
    m_PermStaticMapControl.FlushSection( task, m_Index, pager, iSection, totalPages, totalIO );
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
Converter_Converter::FlushSerial(
    const RTETask_ITask         &task,
    Converter_Version           &ConverterVersion,
    Data_PageNo                 &MaxStaticPageNo,
    Data_PageNo                 &MaxDynamicPageNo,
    IOMan_PackedBlockAddress    &packedRootBlockAddr )
{
    SAPDBTRACE_ROUTINE_DEBUG( "Converter::FlushSerial", Converter_Trace, 5 );

    SAPDB_UInt  NumPages = 0;
    SAPDB_UInt  NumIO    = 0;

    if( ! m_Active )
        return( SAPDB_FALSE );

    // MapControls dealing with permanent pageId's remembers parentReferences
    // of empty converter leaf pages between converter flushes, now it's time
    // to drop these pages from the index

    m_PermMapControl.RemoveEmptyLeaves( task, m_SnapShotVersion, m_Index );
    m_PermStaticMapControl.RemoveEmptyLeaves( task, m_SnapShotVersion, m_Index );

    Converter_Pager pager ( task, m_SnapShotVersion );

    MaxStaticPageNo     = m_PermStaticMapControl.Flush( task, m_Index, pager );
    MaxDynamicPageNo    = m_PermMapControl.Flush( task, m_Index, pager );

    m_RootBlockAddress  = m_Index.Flush( pager, NumPages, NumIO);

    packedRootBlockAddr = m_RootBlockAddress.Pack( GetVolumeNoBitCount());;
    ConverterVersion    = m_Version;

    return( SAPDB_TRUE );
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
Converter_Converter::Expand(
    const RTETask_ITask &task,
    const SAPDB_UInt    numPagesAdded )
{
    SAPDBTRACE_ROUTINE_DEBUG( "Converter::Expand", Converter_Trace, 5 );

    if( ! m_Active )
        return( SAPDB_FALSE );

    const SAPDB_UInt availablePages    = MaxStaticPageNo();
    SAPDB_UInt       immediateGrowSize = 0;

    if( m_NumAllPages < availablePages )
    {
        // Increase usable page count (m_NumAllPages) up to the current
        // maximum of the map's without increasing the maps. This could be
        // useful if the data base is full and add data volume collides with
        // the savepoint,i.e. expand map is not possible!

        immediateGrowSize =  ( numPagesAdded < ( availablePages - m_NumAllPages ))
                             ? numPagesAdded : ( availablePages - m_NumAllPages );

        AdjustDataBaseSize( task, immediateGrowSize );
    }
    const SAPDB_UInt remainingGrowSize = numPagesAdded - immediateGrowSize;

    if( 0 < remainingGrowSize )
    { // Grow maps if realy needed, i.e. the current map is too small

        if(
            ( ExpandMap( task, Data_PageRecoveryMode( Data_Dynamic, Data_NotRecoverable ), remainingGrowSize ))
            &&
            ( ExpandMap( task, Data_PageRecoveryMode( Data_Dynamic, Data_Recoverable ), remainingGrowSize ))
            &&
            ( ExpandMap( task, Data_PageRecoveryMode( Data_Static, Data_Recoverable ), remainingGrowSize ))
        )
        {
            AdjustDataBaseSize( task, remainingGrowSize );
        }
        else
        {
            Converter_Exception msgList( __CONTEXT__, CONVERTER_WRN_EXPAND_UNSUCCESSFUL );
            RTE_Message( msgList );
            return( SAPDB_FALSE );
        }
    }
    Converter_Exception msgList( __CONTEXT__, CONVERTER_INFO_EXPAND_SUCCESSFUL );
    RTE_Message( msgList );
    return( SAPDB_TRUE );
}

SAPDB_Bool
Converter_Converter::Shrink (
    const RTETask_ITask &task,
    const SAPDB_UInt    numPagesRemoved )
{
    SAPDBTRACE_ROUTINE_DEBUG( "Converter::Shrink", Converter_Trace, 5 );

    if( ! m_Active )
        return SAPDB_FALSE;

    AdjustDataBaseSize( task, -1 * (SAPDB_Int)numPagesRemoved);

    return SAPDB_TRUE;
}


/*---------------------------------------------------------------------------*/

void
Converter_Converter::PrepareForDropVolume( const RTETask_ITask &task )
{
    // mark all converter pages as changed

    m_PermStaticMapControl.MarkPermPagesChanged( task );
    m_PermMapControl.MarkPermPagesChanged( task );
}

/*---------------------------------------------------------------------------*/

Converter_ReturnCode
Converter_Converter::BeginSaveData(
    const RTETask_ITask &task,
    Msg_List            &errMsg )
{
    SAPDBTRACE_ROUTINE_DEBUG( "Converter::BeginSaveData", Converter_Trace, 5 );

    SAPDBERR_ASSERT_STATE (! BackUpIsActive());
    SAPDBERR_ASSERT_STATE (false == m_ColdVerifyIsActive);
    SAPDBERR_ASSERT_STATE (0     == m_NumStatConvPagesForBackUp);
    SAPDBERR_ASSERT_STATE (0     == m_NumDynConvPagesForBackUp);
    SAPDBERR_ASSERT_STATE (0     == m_NumPagesForBackUp);

    m_SaveDataIsActiv = true;

    SAPDB_UInt              numPagesForBackUp = 0;
    Converter_ReturnCode    retCode;

    retCode = m_PermStaticMapControl.MarkPermPagesForSaveData( task, errMsg,
              m_NumStatConvPagesForBackUp, numPagesForBackUp );

    if( Converter_Ok != retCode )
        return retCode;

    m_NumPagesForBackUp = numPagesForBackUp;

    retCode = m_PermMapControl.MarkPermPagesForSaveData( task, errMsg,
              m_NumDynConvPagesForBackUp, numPagesForBackUp );

    if( Converter_Ok != retCode )
        return retCode;

    m_NumPagesForBackUp += numPagesForBackUp;

    if (g01is_archive())
    {
        m_PermStaticMapControl.ExtractArchiveConverterForSaveData( task );

        m_PermMapControl.ExtractArchiveConverterForSaveData( task );
    }

    FBM_IManager::Instance().BeginReadingBlocksMarkedForBackUp( task.ID());
    return retCode;
}

/*---------------------------------------------------------------------------*/

Converter_ReturnCode
Converter_Converter::BeginSavePages(
    const RTETask_ITask &task,
    Msg_List            &errMsg )
{
    SAPDBTRACE_ROUTINE_DEBUG( "Converter::BeginSavePages", Converter_Trace, 5 );

    SAPDBERR_ASSERT_STATE (! BackUpIsActive());
    SAPDBERR_ASSERT_STATE (false == m_ColdVerifyIsActive);
    SAPDBERR_ASSERT_STATE (0     == m_NumStatConvPagesForBackUp);
    SAPDBERR_ASSERT_STATE (0     == m_NumDynConvPagesForBackUp);
    SAPDBERR_ASSERT_STATE (0     == m_NumPagesForBackUp);

    m_SavePagesIsActiv = true;

    SAPDB_UInt              numPagesForBackUp = 0;
    Converter_ReturnCode    retCode;

    retCode = m_PermStaticMapControl.MarkPermPagesForSavePages( task, errMsg,
              m_NumStatConvPagesForBackUp, numPagesForBackUp, Version());

    if( Converter_Ok != retCode )
        return retCode;

    m_NumPagesForBackUp = numPagesForBackUp;

    retCode = m_PermMapControl.MarkPermPagesForSavePages( task, errMsg,
              m_NumDynConvPagesForBackUp, numPagesForBackUp, Version());

    if( Converter_Ok != retCode )
        return retCode;

    m_NumPagesForBackUp += numPagesForBackUp;

    FBM_IManager::Instance().BeginReadingBlocksMarkedForBackUp( task.ID());
    return retCode;
}

/*---------------------------------------------------------------------------*/

bool
Converter_Converter::InternResetDataPageAfterBackUp(
    RTETask_ITask&              task,
    Msg_List&                   msgList,
    Converter_PermMapControl&   mapControl,
    const Data_PageNo&          pageNo )
{
    SAPDBTRACE_ROUTINE_DEBUG( "Converter::ResetDataPageAfterBackUp", Converter_Trace, 5 );

    if( pageNo > mapControl.MaxPageNo())
    {
        msgList = Msg_List(
                      Msg_List::Error, SDBMSG_CONVERTER_PAGENO_INVALID,
                      Msg_Arg(SDBMSGTAG_CONVERTER_PAGENO_INVALID__PAGENO, pageNo ),
                      Msg_Arg(SDBMSGTAG_CONVERTER_PAGENO_INVALID__PERSISTENT_TYPE,
                              SAPDB_ToString( mapControl.GetRecoveryMode())),
                      Msg_Arg(SDBMSGTAG_CONVERTER_PAGENO_INVALID__PAGENO_1,
                              mapControl.MaxPageNo()));
        return false;
    }
    if( ! mapControl.RemovePageNoFromPageNoList( task, pageNo ))
    {
        msgList = Msg_List(
                      Msg_List::Error, SDBMSG_CONVERTER_PAGENO_NOT_IN_BACKUP,
                      Msg_Arg(SDBMSGTAG_CONVERTER_PAGENO_NOT_IN_BACKUP__PAGENO, pageNo ),
                      Msg_Arg(SDBMSGTAG_CONVERTER_PAGENO_NOT_IN_BACKUP__PERSISTENT_TYPE,
                              SAPDB_ToString( mapControl.GetRecoveryMode())));
        return false;
    }
    return true;
}

/*---------------------------------------------------------------------------*/

void
Converter_Converter::EndSave(
    const RTETask_ITask &task,
    const SAPDB_Bool    bBackUpSuccessfullyFinished )
{
    SAPDBTRACE_ROUTINE_DEBUG( "Converter::EndSave", Converter_Trace, 5 );

    if( ! m_Active )
        return;

    m_NumDynConvPagesForBackUp  = 0;
    m_NumStatConvPagesForBackUp = 0;
    m_NumPagesForBackUp         = 0;

    if( m_SaveDataIsActiv )
    {
        m_SaveDataIsActiv = false;

        if (! bBackUpSuccessfullyFinished )
        {
            m_PermMapControl.SaveDataFinishedUnSuccessfuly( task );
            m_PermStaticMapControl.SaveDataFinishedUnSuccessfuly( task );

        }
        else
        {
            m_PermMapControl.SaveDataFinishedSuccessfuly( task );
            m_PermStaticMapControl.SaveDataFinishedSuccessfuly( task );
        }
    }
    else
    {
        m_SavePagesIsActiv = false;
        m_PermMapControl.RemoveBitMapPages();
        m_PermStaticMapControl.RemoveBitMapPages();
    }

    /* remove all flags and clear all lists which indicate */
    /* that a page is marked for the current back up       */
    if ( !bBackUpSuccessfullyFinished){
        FBM_IManager::Instance().RestoreAllBlockStatesMarkedForBackup( task.ID());
    }
    else
    {
        SAPDBERR_ASSERT_STATE( CheckBackUp( task ));
    }

    m_PermMapControl.ClearPageNoList();
    m_PermStaticMapControl.ClearPageNoList();
}

/*---------------------------------------------------------------------------*/

void
Converter_Converter::RestoreConverterPage(
    RTETask_ITask&      task,
    Converter_LeafPage& leafPage )
{
    Converter_PageEntryIterator       start = leafPage.Begin();
    const Converter_PageEntryIterator end   = leafPage.End();

    const Data_AddressingMode addrMode = leafPage.ReadPageRecoveryMode().GetAddressingMode();
    Msg_List    msgList;
    while( start != end )
    {
        if (start.EntryInUse())
        {
            const Data_PageNo        pageNo       = leafPage.GetPageNo( start.GetPosition());
            const IOMan_BlockAddress blockAddress = start.GetBlockAddress();
    
            if( ! this->SetBlockAddressOfRestoredPage( task, msgList, blockAddress, pageNo, addrMode )){
                RTE_Crash( msgList ); // temp solution => add MsgList to parameter list 
            }
        }
        ++start;
    }
}

/*---------------------------------------------------------------------------*/

void
Converter_Converter::EndSavepoint(
    const RTETask_ITask &task,
    Converter_Version   &converterVersion )
{
    SAPDBTRACE_ROUTINE_DEBUG( "Converter::EndSavepoint", Converter_Trace, 5 );

    ++m_Version;

    converterVersion = m_Version;

    m_PermMapControl.ReSetSavepointActiveFlag( task );
    m_PermStaticMapControl.ReSetSavepointActiveFlag( task );

    CheckDataAreaPeakFilling();
}

/*---------------------------------------------------------------------------*/

void
Converter_Converter::FreePageNosAfterSavepoint( const RTETask_ITask &task )
{
    SAPDBTRACE_ROUTINE_DEBUG( "Converter::FreePageNosAfterSavepoint", Converter_Trace, 5 );

    // release all PageNos stored in the FreePageNoList
    m_PermMapControl.FreePageNosAfterSavepoint( task, m_SnapShotVersion );
    m_PermStaticMapControl.FreePageNosAfterSavepoint( task, m_SnapShotVersion );

    // resume all tasks which were suspended during a savepoint since there
    // was no more space to insert the pageno to release into a freepagenolist
    if( ! m_FreePageNoWaitQueue.IsEmpty() )
        m_FreePageNoWaitQueue.ResumeAllWaitingTasks( task.ID());

    // resume tasks which want to expand the converter and collided with the savepoint
    if( ! m_ExpandConverterWaitQueue.IsEmpty() )
        m_ExpandConverterWaitQueue.ResumeAllWaitingTasks( task.ID());

    // resume all tasks suspended because no more pageNo were available
    if( ! m_DBFullWaitQueue.IsEmpty() )
        m_DBFullWaitQueue.ResumeAllWaitingTasks( task.ID());

    // At this time all relevant FBM blocks are set to free and therefore the
    // database filling could be changed although no pages are requested or released.
    CheckFillingOfDataBase();
}

/*---------------------------------------------------------------------------*/

void
Converter_Converter::GetPositionForWriting(
    const RTETask_ITask         &task,
    const Data_PageNo           &pageNo,
    const Data_PageRecoveryMode &pageRecoveryMode,
    const Converter_Version     &pageConverterVersion,
    const SAPDB_Bool            bReqSequential,
    IOMan_BlockAddress          &block )
{
    SAPDBTRACE_ROUTINE_DEBUG( "Converter::GetPositionForWriting", Converter_Trace, 5 );


    if( ! m_Active )
    {
        Converter_Exception errMsg( __CONTEXT__, CONVERTER_INFO_SHUTDOWN );
        RTE_Message( errMsg );

        Kernel_IAdminRestartShutdown::Instance().Offline();
    }

    const SAPDB_Bool     bIsPerm = pageRecoveryMode.PageIsRecoverable();
    Converter_ReturnCode retCode;

    if( bIsPerm )
    {
        if( pageRecoveryMode.UseDynamicPageAddressing() )
            retCode = m_PermMapControl.GetNewPositionForWrite( task, pageNo,
                      pageConverterVersion, m_SnapShotVersion, bReqSequential,
                      block );
        else
            retCode = m_PermStaticMapControl.GetNewPositionForWrite( task, pageNo,
                      pageConverterVersion, m_SnapShotVersion, bReqSequential,
                      block );
    }
    else // temp
    {
        retCode = m_TempMapControl.GetNewPositionForWrite( task, pageNo, bReqSequential, block );
    }

    if( Converter_Ok == retCode )
    {
        // At this time all relevant FBM blocks are set to free and therefore the
        // database filling could be changed although no pages are requested or released.
        CheckFillingOfDataBase();
        return;
    }

    Converter_MapError mapError( __CONTEXT__ , pageRecoveryMode, pageNo, retCode );
    RTE_Crash( mapError );
}

/*---------------------------------------------------------------------------*/

Converter_ReturnCode
Converter_Converter::GetPositionForReading(
    const RTETask_ITask         &task,
    const Data_PageNo           &pageNo,
    const Data_PageRecoveryMode &pageRecoveryMode,
    IOMan_BlockAddress          &block )
{
    SAPDBTRACE_ROUTINE_DEBUG( "Converter::GetPositionForReading", Converter_Trace, 5 );

    if( ! m_Active )
    {
        Converter_Exception errMsg( __CONTEXT__, CONVERTER_INFO_SHUTDOWN );
        RTE_Message( errMsg );

        Kernel_IAdminRestartShutdown::Instance().Offline();
    }

    if( pageRecoveryMode.PageIsRecoverable() )
    {
        if( pageRecoveryMode.UseDynamicPageAddressing() )
            return( m_PermMapControl.GetBlockAddress( task, pageNo, block ));
        else
            return( m_PermStaticMapControl.GetBlockAddress( task, pageNo, block ));
    }
    else
        return( m_TempMapControl.GetBlockAddress ( task, pageNo, block ));
}

/*---------------------------------------------------------------------------*/

void
Converter_Converter::SetPositionForWriting(
    const RTETask_ITask         &task,
    const Converter_Version     &pageConverterVersion,
    const Data_PageNo           &pageNo,
    const Data_PageRecoveryMode &pageRecoveryMode,
    const IOMan_BlockAddress    &block )
{
    SAPDBTRACE_ROUTINE_DEBUG( "Converter::SetPositionForWriting", Converter_Trace, 5 );

    if( ! m_Active )
    {
        Converter_Exception errMsg( __CONTEXT__, CONVERTER_INFO_SHUTDOWN );
        RTE_Message( errMsg );

        Kernel_IAdminRestartShutdown::Instance().Offline();
    }

    if( pageRecoveryMode.PageIsRecoverable() )
    {
        if( pageRecoveryMode.UseDynamicPageAddressing() )
            m_PermMapControl.SetPositionForWriting( task, m_Version, pageConverterVersion,
                                                    m_SnapShotVersion, pageNo, block );
        else
            m_PermStaticMapControl.SetPositionForWriting( task, m_Version, pageConverterVersion,
                    m_SnapShotVersion, pageNo, block );
    }
    else
        m_TempMapControl.SetPositionForWriting ( task, pageConverterVersion, pageNo, block );

    // At this time all relevant FBM blocks are set to free and therefore the
    // database filling could be changed although no pages are requested or released
    CheckFillingOfDataBase();
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
Converter_Converter::RequestSpecificiedNewStaticPageNo(
    const RTETask_ITask &task,
    const Data_PageNo   &pageNo )
{
    SAPDBTRACE_ROUTINE_DEBUG( "Converter::RequestSpecificiedNewStaticPageNo", Converter_Trace, 5 );

    // PTS 1116298 UH 2002-06-21
    // Added the wait interface and the loop because RequestSpecifiedNewPageNo()
    // has to wait for a savepoint.
    // It may happen that there exists a pending Free for the requested pageno.

    SAPDB_Bool  bWaitUntilSavepointFinished = SAPDB_FALSE;
    Kernel_SynchronizedTaskWaitList::WaitContext    waitContext;

    do
    {
        if( ! m_Active )
        {
            Converter_Exception errMsg( __CONTEXT__, CONVERTER_INFO_SHUTDOWN );
            RTE_Message( errMsg );

            Kernel_IAdminRestartShutdown::Instance().Offline();
        }

        if( ! m_PermStaticMapControl.RequestSpecifiedNewPageNo( task, pageNo,
                waitContext, m_FreePageNoWaitQueue, bWaitUntilSavepointFinished ))
        {
            // means that the page no is in use
            SAPDBTRACE_WRITELN( Converter_Trace, 5, "PageNo: " <<  pageNo << "not available" );
            return( SAPDB_FALSE );
        }

        if( bWaitUntilSavepointFinished )
            m_FreePageNoWaitQueue.Wait( task.ID());
    }
    while( bWaitUntilSavepointFinished );

    m_NumPermPagesUsed.Increment();
    CheckFillingOfDataBase ();

    SAPDBTRACE_WRITELN( Converter_Trace, 7, "PermPagesUsed: " << m_NumPermPagesUsed );
    return( SAPDB_TRUE );
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
Converter_Converter::FreeTempPageNo(
    const RTETask_ITask &task,
    const Data_PageNo   &pageNo )
{
    SAPDBTRACE_ROUTINE_DEBUG( "Converter::FreeTempPageNo", Converter_Trace, 5 );

    SAPDBTRACE_WRITELN( Converter_Trace, 5, "Free temp PageNo: " << pageNo );

    if( ! m_Active )
        return( SAPDB_FALSE );

    m_TempMapControl.FreePageNo( task, pageNo );

    m_NumTempPagesUsed.Decrement();
    SAPDBTRACE_WRITELN( Converter_Trace, 7, "TempPagesUsed: " << m_NumTempPagesUsed );

    CheckFillingOfDataBase();

    if( ! m_DBFullWaitQueue.IsEmpty() ){
        m_DBFullWaitQueue.ResumeAllWaitingTasks( task.ID());
    }
    return( SAPDB_TRUE );
}

/*---------------------------------------------------------------------------*/

Converter_ReturnCode
Converter_Converter::HandleDBFull(
    Trans_Context   &trans,
    SAPDB_UInt      numDataPagesRequested )
{
    SAPDBTRACE_ROUTINE_DEBUG( "Converter::HandleDBFull", Converter_Trace, 6 );

    numDataPagesRequested += CONV_NUM_SECURITY_PAGES; // add some pages for security reasons
    RTETask_ITask   &task = trans.Task();

    while( m_Active )
    {
        const SAPDB_Int4 numChangedPages  = NumChangedPages();
        const SAPDB_Bool pageNosExhausted = ! IsSpaceAvailable( numDataPagesRequested );

        if( ! pageNosExhausted )
            return Converter_Ok; // comman way to leave this method

        // ============ DB Full handling ============

        if( ! SavepointNeeded( numChangedPages ))
            GC_IGCController::Instance().TriggerGarbageCollection(  task.ID(), true );

        if( trans.HasSavepointSynchronizationLocks())
            return Converter_Ok;

        const SAPDB_Bool bAddVolumeIsPossible = IOMan_IDataInfo::GetInstance().AddDataVolumePossible();

        if
        (
            (
                (
                    (SAPDB_UInt( trans.TempPageCount()) > m_NumTempPagesUsed/20) &&
                    (m_NumTempPagesUsed                 > m_NumAllPages/100    ) &&
                    (SAPDB_UInt( trans.TempPageCount()) > 100                  )
                )
                ||
                ( ! bAddVolumeIsPossible )
                ||
                ( trans.IsDbFullNoWait())
            )
            &&
            ( ! trans.IsRollback())
        )
        {
            if( ! bAddVolumeIsPossible )
            {
                Converter_Exception msgList (__CONTEXT__, CONVERTER_WRN_NO_ADD_DATA_DEVICE,
                                             SAPDB_ToString( task.ID(), _T_d ));
                WriteMessageList( msgList );
                msgList.ClearMessageList();
            }
            else if( trans.IsDbFullNoWait())
            {
                Converter_Exception msgList (__CONTEXT__, CONVERTER_WRN_NO_WAIT,
                                             SAPDB_ToString( task.ID(), _T_d ));
                WriteMessageList( msgList );
                msgList.ClearMessageList();
            }
            else
            {
                Converter_Exception msgList(__CONTEXT__,CONVERTER_WRN_FORCE_ROLLBACK_DUE_TO_DB_FULL,
                                            SAPDB_ToString( task.ID(), _T_d ),
                                            SAPDB_ToString( m_NumAllPages, _T_d ),
                                            SAPDB_ToString( m_NumTempPagesUsed, _T_d ),
                                            SAPDB_ToString( trans.TempPageCount(), _T_d ));
                WriteMessageList( msgList );
                msgList.ClearMessageList();
            }
            return Converter_TempSpaceExhausted; // force rollback but do not suspend task
        }
        {
            Converter_Exception msgList( __CONTEXT__,CONVERTER_WRN_SUSPEND_TASK_DUE_TO_DB_FULL,
                                         SAPDB_ToString( task.ID(), _T_d ),
                                         SAPDB_ToString( GetNumberOfUsedPages(), _T_d ),
                                         SAPDB_ToString( numDataPagesRequested, _T_d ),
                                         SAPDB_ToString( GetNumberOfAllPages(), _T_d ));
            WriteMessageList( msgList );
            msgList.ClearMessageList();
        }

        m_DBFullWaitQueue.InsertTaskAndWait( task.ID());

        if(( task.CancelPending() ) && ( ! trans.IsRollback()))
        {
            Converter_Exception msgList( __CONTEXT__,CONVERTER_INFO_CANCELLED_DURING_DB_FULL,
                                         SAPDB_ToString( task.ID(), _T_d ));
            WriteMessageList( msgList );
            msgList.ClearMessageList();

            return Converter_ExecutionCanceled;
        }
    }
    if( ! m_Active )
    {
        Converter_Exception errMsg( __CONTEXT__, CONVERTER_INFO_SHUTDOWN );
        RTE_Message( errMsg );
        errMsg.ClearMessageList();

        Kernel_IAdminRestartShutdown::Instance().Offline();
    }

    return Converter_Ok;
}

/*---------------------------------------------------------------------------*/

Converter_ReturnCode
Converter_Converter::BeginCheckData(
    const RTETask_ITask &task,
    Msg_List            &errMsg )
{
    SAPDBTRACE_ROUTINE_DEBUG( "Converter::BeginCheckData", Converter_Trace, 5 );

    SAPDBERR_ASSERT_STATE (! BackUpIsActive());
    SAPDBERR_ASSERT_STATE (false == m_ColdVerifyIsActive);
    SAPDBERR_ASSERT_STATE (0     == m_NumStatConvPagesForBackUp);
    SAPDBERR_ASSERT_STATE (0     == m_NumDynConvPagesForBackUp);
    SAPDBERR_ASSERT_STATE (0     == m_NumPagesForBackUp);

    m_InMaintenanceMode  = true;
    m_ColdVerifyIsActive = true;

    Converter_ReturnCode retCode;

    retCode = m_PermMapControl.InitializePageNoList( task, errMsg );

    if( Converter_Ok == retCode )
        retCode = m_PermStaticMapControl.InitializePageNoList( task, errMsg );

    if( Converter_Ok != retCode )
        errMsg.Overrule( Msg_List( Msg_List::Error,
                                   SDBMSG_CONVERTER_ALLOCATION_OF_USED_PNOLIST_FAILED ));

    return retCode;
}

/*---------------------------------------------------------------------------*/

void
Converter_Converter::EndCheckData(
    const RTETask_ITask &task,
    Data_PageCount      &NumStaticPermDataPages,
    Data_PageCount      &NumDynamicPermDataPages,
    Data_PageCount      &NumPermConvPages,
    Data_PageCount      &NumReleaseBlocks )
{
    SAPDBTRACE_ROUTINE_DEBUG( "Converter::EndCheckData", Converter_Trace, 5 );

    SAPDBERR_ASSERT_STATE (! BackUpIsActive());
    SAPDBERR_ASSERT_STATE (true == m_ColdVerifyIsActive);
    SAPDBERR_ASSERT_STATE (0     == m_NumStatConvPagesForBackUp);
    SAPDBERR_ASSERT_STATE (0     == m_NumDynConvPagesForBackUp);
    SAPDBERR_ASSERT_STATE (0     == m_NumPagesForBackUp);

    SAPDB_UInt NumPermStaticConvPages   = 0;
    SAPDB_UInt NumPermDynamicConvPages  = 0;
    SAPDB_UInt NumStaticTempDataPages   = 0;
    SAPDB_UInt NumDynamictempDataPages  = 0;
    SAPDB_UInt numStaticReleasedBlocks  = 0;
    SAPDB_UInt numDynamicReleasedBlocks = 0;

    m_PermStaticMapControl.ReleaseUnusedPageNos( task, m_SnapShotVersion,
            NumStaticPermDataPages, NumPermStaticConvPages, numStaticReleasedBlocks );

    m_PermMapControl.ReleaseUnusedPageNos(       task, m_SnapShotVersion,
            NumDynamicPermDataPages, NumPermDynamicConvPages, numDynamicReleasedBlocks );

    NumPermConvPages = NumPermStaticConvPages + NumPermDynamicConvPages;
    NumReleaseBlocks = numStaticReleasedBlocks + numDynamicReleasedBlocks;

    m_NumPermPagesUsed = NumStaticPermDataPages + NumDynamicPermDataPages;

    m_PermStaticMapControl.ClearPageNoList();
    m_PermMapControl.ClearPageNoList();

    m_ColdVerifyIsActive = false;
    m_InMaintenanceMode  = false;
}

/*---------------------------------------------------------------------------*/

void
Converter_Converter::EndFailedCheckData ( const RTETask_ITask &task )
{
    SAPDBTRACE_ROUTINE_DEBUG( "Converter::EndFailedCheckData", Converter_Trace, 5 );

    SAPDBERR_ASSERT_STATE (! BackUpIsActive());
    SAPDBERR_ASSERT_STATE (true == m_ColdVerifyIsActive);
    SAPDBERR_ASSERT_STATE (0     == m_NumStatConvPagesForBackUp);
    SAPDBERR_ASSERT_STATE (0     == m_NumDynConvPagesForBackUp);
    SAPDBERR_ASSERT_STATE (0     == m_NumPagesForBackUp);

    m_PermStaticMapControl.EndFailedCheckData( task );
    m_PermMapControl.EndFailedCheckData( task );

    m_ColdVerifyIsActive = false;
}

/*---------------------------------------------------------------------------*/

void
Converter_Converter::GetDbSpaceStatistics(
    const RTETask_ITask         &task,
    Converter_DBSpaceStatistic  &dbSpaceInfo )
{
    SAPDBTRACE_ROUTINE_DEBUG( "Converter::GetDbSpaceStatistics", Converter_Trace, 5 );

    dbSpaceInfo.SetTotalPageCount( IOMan_IDataInfo::GetInstance().TotalDataPages());
    dbSpaceInfo.SetDataPermPageCount( IOMan_IDataInfo::GetInstance().TotalDataPages());
    dbSpaceInfo.SetAllPermPageCount( m_NumPermPagesUsed + m_NumPermConvPages );
    dbSpaceInfo.SetTempPageCount( m_NumTempPagesUsed );
    dbSpaceInfo.SetAllUsedPageCount( GetNumberOfUsedPages());
    dbSpaceInfo.SetModifiedPageCount( UpdatedPagesSinceLastSaveData( task ));
    dbSpaceInfo.SetMaxPermPagePeak( GetPeakForPermPages());
    dbSpaceInfo.SetUsedFBMBlocks( FBM_IManager::Instance().GetNumberOfUsedBlocks());
    dbSpaceInfo.CalculatePercentValues();
}

/*---------------------------------------------------------------------------*/

void
Converter_Converter::ResetSystemViewDataStatistics()
{
    m_FrozenTemporaryPeakFilling = m_NumTempPagesUsed;
    m_FrozenPermanentPeakFilling = m_NumPermPagesUsed;
    m_ResetTimeStamp             = RTESys_MicroSecondTime();
}

/*---------------------------------------------------------------------------*/

bool
Converter_Converter::IsDBFull() const
{
    if( ! m_Active )
        return false;

    if( ! m_DBFullWaitQueue.IsEmpty())
        return( true ); // task is suspended because of db full

    if( FBM_IManager::Instance().IsDBFull())
        return( true ); // task is suspended because of db full

    const SAPDB_UInt numDataPagesRequested = CONV_NUM_SECURITY_PAGES * 2;// see HandledbFull

    // PTS 1124331 TS 2003-09-23
    if( ! IsSpaceAvailable( numDataPagesRequested ))
        return( true ); // pageNos exhausted

    if( ! FBM_IManager::Instance().IsSpaceAvailable( NumChangedPages() + numDataPagesRequested ))
        return( true ); // blocks in FBM exhausted

    return( false );
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
Converter_Converter::SecuritySpaceReached() const
{
    // PTS 1124331 TS 2003-09-23

    if( ! IsSpaceAvailable( CONV_NUM_SECURITY_PAGES ))
        return( SAPDB_TRUE );

    if( ! FBM_IManager::Instance().IsSpaceAvailable( NumChangedPages()))
        return( SAPDB_TRUE );

    return( SAPDB_FALSE );
}

/*---------------------------------------------------------------------------*/

Converter_GarbageCollectionReturnCode
Converter_Converter::GarbageCollectionNeeded() const // PTS 1116157 FF 2002-06-14
{
    if( ! SavepointIsPossible() )
        return( Converter_GCNotWanted );

    const SAPDB_Int4 numChangedPages = NumChangedPages();

    if( SavepointNeeded( numChangedPages )) // Savepoint will come immediately started by TimeOut Task
        return( Converter_GCNotWanted );

    // data base is filled up to 90 percent inclusive the changed pages of the data cache

    if( FBM_IManager::Instance().GarbageCollectionNeeded( numChangedPages ))
        return( Converter_GCNeeded );
    else
        return( Converter_GCNotNeeded );
}

/*---------------------------------------------------------------------------*/

// called by timeout task
bool
Converter_Converter::CheckAndStartSavepoint(
    RTETask_ITask   &task,
    Msg_List        &msgList ) const
{
    SAPDBTRACE_ROUTINE_DEBUG( "Converter::CheckAndStartSavepoint", Converter_Trace, 8 );

    if( IsDBFull())
    {
        KernelDBE_DBFull    dbFullEvent;
        dbFullEvent.SendEvent();

        IOMan_IDataManager::GetInstance().AbortDropDataVolume(task, msgList, IOMan_DropDBFull);
    }

    const SAPDB_Int4 numChangedPages = NumChangedPages();

    if( ! SavepointNeeded( numChangedPages ))
        return true;

    { // write info message into knldiag
        Converter_Exception infoMsg(
            __CONTEXT__, CONVERTER_SAVEPOINT_NEEDED,
            SAPDB_ToString( numChangedPages, _T_d ),
            SAPDB_ToString( FBM_IManager::Instance().GetNumberOfUsedBlocks(), _T_d ),
            SAPDB_ToString( FBM_IManager::Instance().GetNumberOfFreeBlocks(), _T_d ));

        RTE_Message( infoMsg );
        infoMsg.ClearMessageList();
    }

    if ( Log_Savepoint::ok ==
            Log_Savepoint::Instance().StartSavepoint( task, Log_SVPReasonConverter, msgList ))
        return true;
    else
        return false;
}

/*---------------------------------------------------------------------------*/

bool
Converter_Converter::SavepointNeeded( const SAPDB_Int4 numChangedPages ) const
{
    if( ! SavepointIsPossible() )
        return( false );

    if( FBM_IManager::Instance().SavepointNeeded() ) // more blocks in state free after SVP than free
        return( true );

    if( numChangedPages > (( FBM_IManager::Instance().GetNumberOfFreeBlocks()) / 3 ))
        return( true );

    return( false );
}

/*---------------------------------------------------------------------------*/

void
Converter_Converter::Dump(
    const RTETask_ITask &task,
    Kernel_Dump         &Dump,
    const SAPDB_Bool    bEnterRegion )
{
    struct ConverterManager   CM;

    CM.dmpVersion                         = m_Version;
    CM.dmpNumPermPagesUsed                = m_NumPermPagesUsed;
    CM.dmpNumTempPagesUsed                = m_NumTempPagesUsed;
    CM.dmpNumAllPages                     = m_NumAllPages;
    CM.dmpDeltaSnapShotPages              = m_NumSnapShotPages;
    CM.dmpNumPagesForBackup               = m_NumPagesForBackUp;
    CM.dmpNumStatConvPagesForBackup       = m_NumStatConvPagesForBackUp;
    CM.dmpNumDynConvPagesForBackup        = m_NumDynConvPagesForBackUp;
    CM.dmpNumPermConvPages                = m_NumPermConvPages;
    CM.dmpNumTempConvPages                = m_NumTempConvPages;
    CM.dmpMapBlockingSize                 = m_MapBlockingSize;
    CM.dmpConverterIsActive               = m_Active;
    CM.dmpSavePagesIsActive               = m_SavePagesIsActiv;
    CM.dmpSaveDataIsActive                = m_SaveDataIsActiv;
    CM.dmpRestoreDataIsActive             = m_RestoreDataActive;
    CM.dmpRestorePagesIsActive            = m_RestorePagesActive;
    CM.dmpColdVerifyIsActive              = m_ColdVerifyIsActive;

    if( m_RootBlockAddress.IsValid() )
    {
        CM.dmpRootDevNo   = m_RootBlockAddress.GetVolumeNo();
        CM.dmpRootBlockNo = m_RootBlockAddress.GetBlockNo();
    }
    else
    {
        CM.dmpRootDevNo   = IOMan_VolumeNo();
        CM.dmpRootBlockNo = IOMan_BlockNo();
    }

    Dump.InsertEntry( Kernel_Dump::DmpConverterManager, Kernel_DumpPage::Entry( &CM, sizeof( CM )));

    m_Index.Dump( Dump );

    m_TempMapControl.Dump( task, Dump, bEnterRegion );
    m_PermMapControl.Dump( task, Dump, bEnterRegion );
    m_PermStaticMapControl.Dump( task, Dump, bEnterRegion );
}

/*---------------------------------------------------------------------------*/

void
Converter_Converter::SetBlockAddressForMigratedPage (
    const RTETask_ITask&        task,
    const IOMan_BlockAddress    &block,
    const Data_PageNo           &pageNo,
    const SAPDB_Bool            bSetToOccupied )
{
    SAPDBTRACE_ROUTINE_DEBUG( "Converter::SetBlockAddressForMigratedPage", Converter_Trace, 5 );

    SAPDBERR_ASSERT_STATE( m_RestoreDataActive );

    // At this time the corresponding converter page entry is invalid (initialized),
    // because the converter is build up the current migration step.
    Msg_List msgList;
    if( ! m_PermMapControl.SetBlockAddressOfRestoredPage( task, msgList, pageNo,
            m_SnapShotVersion, block, m_RestoreDataActive ))
    {
        RTE_Crash( msgList ); // temp solution => add MsgList to parameter list
    }

    m_NumPermPagesUsed.Increment();

    CheckFillingOfDataBase();

    if( SAPDB_TRUE != bSetToOccupied )
        return;

    // After updating the converter page entry, the block is set to occupied within
    // the FBM. Note that the given block must not specify one of the reserved blocks
    // in the data volumes.

    if( ! FBM_IManager::Instance().SetBlockStateToOccupied( task.ID(), block ))
    {
        Converter_Exception errMsg( __CONTEXT__, CONVERTER_BAD_ENTRY_STATE,
                                    SAPDB_ToString( pageNo, _T_d ), "perm");
        RTE_Crash( errMsg );
    }
}

/*---------------------------------------------------------------------------*/

IOMan_BlockAddress
Converter_Converter::DetermineAndSetBlockAddressForMigratedPage(
    const RTETask_ITask &task,
    const Data_PageNo   &pageNo )
{
    const SAPDB_Bool         bSetToOccupied = SAPDB_FALSE;
    const SAPDB_Bool         bReqSequential = SAPDB_FALSE;
    const IOMan_BlockAddress newBlock =
        FBM_IManager::Instance().GetFreeBlock( task.ID() , bReqSequential );

    // Note that the newBlock is set to occupied by the GetFreeBlock
    // method and therefore not set again to occupupied by the subsequent call.

    SetBlockAddressForMigratedPage( task, newBlock, pageNo, bSetToOccupied );

    return( newBlock );
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
Converter_Converter::DeleteBlockAddress(
    const RTETask_ITask         &task,
    const Data_PageNo           &pageNo,
    const Data_PageRecoveryMode &recMode )
{
    SAPDBERR_ASSERT_STATE( m_Active );

    IOMan_BlockAddress          block;
    Converter_ReturnCode        retCode;
    const Data_AddressingMode   addrMode = recMode.GetAddressingMode();

    if( ! recMode.PageIsRecoverable())
        return( SAPDB_TRUE );

    if( Data_Dynamic == addrMode ){
        retCode = m_PermMapControl.GetBlockAddress( task, pageNo, block );
    }
    else if( Data_Static == addrMode ){
        retCode = m_PermStaticMapControl.GetBlockAddress( task, pageNo, block );
    }
    else{
        retCode = Converter_NoConverterEntry;
    }

    if( Converter_Ok != retCode )
        return( SAPDB_FALSE );

    Converter_Version  pageConverterVersion; // dummy version, not used in this case

    pageConverterVersion.SetToInitialVersion(); // set to a defined value, which will not influence the execution

    if( Data_Dynamic == addrMode )
        return( FreePermPageNo( task, pageNo, pageConverterVersion ));
    else
        return( FreeStaticPageNo( task, pageNo, pageConverterVersion ));
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
Converter_Converter::GetBlockAddress(
    const RTETask_ITask         &task,
    const Data_PageNo           &pageNo,
    const Data_PageRecoveryMode &recMode,
    IOMan_BlockAddress          &block )
{
    SAPDBERR_ASSERT_STATE( m_Active );

    Converter_ReturnCode        retCode;
    const Data_AddressingMode   addrMode = recMode.GetAddressingMode();

    if( Data_Dynamic == addrMode )
    {
        if( recMode.PageIsRecoverable())
            retCode = m_PermMapControl.GetBlockAddress( task, pageNo, block );
        else
            retCode = m_TempMapControl.GetBlockAddress( task, pageNo, block );
    }
    else if( Data_Static == addrMode ){
        retCode = m_PermStaticMapControl.GetBlockAddress( task, pageNo, block );
    }
    else{
        retCode = Converter_NoConverterEntry;
    }

    if( Converter_Ok == retCode )
        return( SAPDB_TRUE );

    block.Invalidate();

    return( SAPDB_FALSE );
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
Converter_Converter::SetNewBlockAddress(
    const RTETask_ITask         &task,
    const Data_PageNo           &pageNo,
    const Data_PageRecoveryMode &recMode,
    const IOMan_BlockAddress    &blockAddr )
{
    SAPDBERR_ASSERT_STATE( m_Active );

    if( ! recMode.PageIsRecoverable()){
        return true;
    }
    Msg_List msgList;
    bool retCode = false;
    if( recMode.UseStaticPageAddressing())
    {
        retCode = m_PermStaticMapControl.SetBlockAddressOfRestoredPage(
            task, msgList, pageNo, m_SnapShotVersion, blockAddr, m_RestoreDataActive );
    }
    else
    {
        retCode = m_PermMapControl.SetBlockAddressOfRestoredPage(
            task, msgList, pageNo, m_SnapShotVersion, blockAddr, m_RestoreDataActive );
    }
    if( 0 == retCode ){
        RTE_Message( msgList ); // temp solution adjust parameter list
        msgList.Clear();
    }
    return retCode;
}

//----------------------------------------------------------------------------
//  private methods
//----------------------------------------------------------------------------

void
Converter_Converter::InitializeBase( const SAPDB_UInt NumAllPages )
{
    m_SnapShotVersion.Invalidate();
    m_SnapShotCreationTimeStamp.Invalidate();

    m_ResetTimeStamp = RTE_KGS::Instance().GetStatistic().timeOfAdminState;

    m_NumPermPagesUsed                 = 0;
    m_NumTempPagesUsed                 = 0;
    m_NumPagesForBackUp                = 0;
    m_NumStatConvPagesForBackUp        = 0;
    m_NumDynConvPagesForBackUp         = 0;
    m_NumPermConvPages                 = 0;
    m_NumTempConvPages                 = 0;
    m_NumSnapShotPages                 = 0;
    m_TemporaryPeakFilling             = 0;
    m_FrozenTemporaryPeakFilling       = 0;
    m_PermanentPeakFilling             = 0;
    m_MaxUsedDynamicPageNo             = 0;
    m_MaxUsedStaticPageNo              = 0;
    m_NumAllPages                      = NumAllPages;
    SetSecuritySpaceLimit();
    m_Active                           = true;
    m_InMaintenanceMode                = false;
    m_SavePagesIsActiv                 = false;
    m_SaveDataIsActiv                  = false;
    m_RestoreDataActive                = false;
    m_RestorePagesActive               = false;
    m_ColdVerifyIsActive               = false;
    m_DBUsageMonitor.Rescale (m_NumAllPages);
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
Converter_Converter::ExpandMap(
    const RTETask_ITask          &task,
    const Data_PageRecoveryMode  &recoveryMode,
    const SAPDB_UInt             numPagesAdded )
{
    Kernel_SynchronizedTaskWaitList::WaitContext    waitContext;
    Converter_ReturnCode    retCode;
    const SAPDB_UInt        newAllPages = m_NumAllPages + numPagesAdded;

    do
    {
        if( recoveryMode.PageIsRecoverable() )
        {
            if( recoveryMode.UseDynamicPageAddressing() ){
                retCode = m_PermMapControl.Expand( task, newAllPages,
                                                   waitContext, m_ExpandConverterWaitQueue );
            }
            else{
                retCode = m_PermStaticMapControl.Expand( task, newAllPages,
                          waitContext, m_ExpandConverterWaitQueue );
            }
        }
        else{
            retCode = m_TempMapControl.Expand( task, newAllPages );
        }


        switch( retCode )
        {
        case Converter_Ok:
            return( SAPDB_TRUE );
        case Converter_SavePointActive:
            m_ExpandConverterWaitQueue.Wait( task.ID());
            break;
        default:
            {
                Converter_Exception ExpandFailed(__CONTEXT__,CONVERTER_EXPAND_FAILED,
                                                 SAPDB_ToString( m_NumAllPages, _T_d) ,
                                                 SAPDB_ToString( newAllPages, _T_d ));
                RTE_Message( ExpandFailed );
            }
            return( SAPDB_FALSE );
        }
    }
    while( m_Active );

    return( SAPDB_FALSE );
}

/*---------------------------------------------------------------------------*/

void
Converter_Converter::RequestNewPageNo(
    const RTETask_ITask     &task,
    const bool              bIgnoreSecureLimit,
    Converter_MapControl    &mapControl,
    Data_PageNo             &newPageNo )
{
    const SAPDB_UInt    numDataPagesRequested = 1;

    while( m_Active )
    {
        const SAPDB_UInt upperLimit = bIgnoreSecureLimit ? m_NumAllPages : m_SecuritySpaceLimit;

        if(( GetNumberOfUsedPages() + numDataPagesRequested ) < upperLimit )
        {
            if( mapControl.RequestNewPageNo( task, newPageNo ))
            {
                CheckFillingOfDataBase();
                SAPDBTRACE_WRITELN( Converter_Trace, 5, "New PageNo: " << newPageNo );
                return;
            }
        }
        {
            Converter_Exception msgList( __CONTEXT__,CONVERTER_WRN_SUSPEND_TASK_DUE_TO_DB_FULL,
                                         SAPDB_ToString( task.ID(), _T_d ),
                                         SAPDB_ToString( GetNumberOfUsedPages(), _T_d ),
                                         SAPDB_ToString( numDataPagesRequested, _T_d ),
                                         SAPDB_ToString( GetNumberOfAllPages(), _T_d ));
            WriteMessageList( msgList );
        }
        m_DBFullWaitQueue.InsertTaskAndWait( task.ID());
    }

    if( ! m_Active )
    {
        Converter_Exception msgList( __CONTEXT__, CONVERTER_INFO_SHUTDOWN );
        RTE_Message( msgList );

        Kernel_IAdminRestartShutdown::Instance().Offline();
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
Converter_Converter::FreePermPageNo(
    const RTETask_ITask         &task,
    Converter_PermMapControl    &mapControl,
    const Data_PageNo           pageNo,
    const Converter_Version     &pageConverterVersion )
{
    SAPDBTRACE_ROUTINE_DEBUG( "Converter::FreePermPageNo", Converter_Trace, 5 );

    SAPDBTRACE_WRITELN( Converter_Trace, 5, "Free perm PageNo: " << pageNo );

    if( ! m_Active ){
        return( SAPDB_FALSE );
    }

    SAPDB_Bool  bWaitUntilSavepointFinished  = false;
    SAPDB_Bool  bOldBlockIsUsableImmediately = false;
    SAPDB_Bool  bDoDecrementPermUseCount     = true;

    do
    {
        Kernel_SynchronizedTaskWaitList::WaitContext    waitContext;

        mapControl.FreePageNo( task, pageConverterVersion, m_SnapShotVersion, pageNo,
                               waitContext, m_FreePageNoWaitQueue, bWaitUntilSavepointFinished,
                               bOldBlockIsUsableImmediately, bDoDecrementPermUseCount );

        if( bWaitUntilSavepointFinished )
        {
            m_FreePageNoWaitQueue.Wait( task.ID());

            if( ! m_Active ){
                return( SAPDB_FALSE );
            }
        }
    }
    while( bWaitUntilSavepointFinished );

    if( bDoDecrementPermUseCount )
        m_NumPermPagesUsed.Decrement();

    SAPDBTRACE_WRITELN( Converter_Trace, 7, "PermPagesUsed: " << m_NumPermPagesUsed );

    CheckFillingOfDataBase();

    if(( bOldBlockIsUsableImmediately ) && ( ! m_DBFullWaitQueue.IsEmpty() )){
        m_DBFullWaitQueue.ResumeAllWaitingTasks( task.ID());
    }
    return( SAPDB_TRUE );
}

/*---------------------------------------------------------------------------*/

void
Converter_Converter::RestartSnapShot(
    const RTETask_ITask             &task,
    const IOMan_PackedBlockAddress  &packedSnapShotRootBlock )
{
    SAPDBTRACE_ROUTINE_DEBUG( "Converter::RestartSnapShot", Converter_Trace, 6 );

    SAPDBERR_ASSERT_STATE( NULL == m_VolumeList );

    const SAPDB_Bool            bReadSnapShot     = SAPDB_TRUE;
    const IOMan_BlockAddress    snapShotRootBlock = packedSnapShotRootBlock.Unpack( GetVolumeNoBitCount());
    Converter_SnapShotPager     pager( task, m_SnapShotVersion, m_Index );
    Converter_Index             snapShotIndex( m_Allocator);
    VolumeList                  volumeList( m_Allocator );

    SAPDBERR_ASSERT_STATE( snapShotRootBlock.IsValid());

    snapShotIndex.Restart( snapShotRootBlock, pager );

    SAPDBERR_ASSERT_STATE( snapShotIndex.Verify());

    snapShotIndex.CreateInputQueues( volumeList, m_PageSize );

    m_VolumeList = &volumeList; // is used by the pager tasks

    Pager_IRestart::GetInstance().ReadConverter( task.ID(), bReadSnapShot );

    m_VolumeList = NULL;

    volumeList.Delete(); // free occupied memory

    snapShotIndex.Shutdown();
}

/*---------------------------------------------------------------------------*/

SAPDB_Int4
Converter_Converter::NumChangedPages() const
{
    return( bd20GetNumberOfChangedPages() + GetNumberOfChangedConvPages());
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
Converter_Converter::IsLeafPageMember(
    const RTETask_ITask       &task,
    const Converter_LeafPage  &page )
{
    SAPDBTRACE_ROUTINE_DEBUG( "Converter::IsLeafPageMember", Converter_Trace, 7 );


    const Data_PageRecoveryMode recMode    = page.ReadPageRecoveryMode();
    const Data_PageNo           basePageNo = page.ReadBasePageNo();
    const IOMan_BlockAddress    block      = page.ReadBlockAddress();

    if( recMode.UseDynamicPageAddressing())
        return( m_PermMapControl.IsMember( task, basePageNo, block ));

    if( recMode.UseStaticPageAddressing())
        return(m_PermStaticMapControl.IsMember( task, basePageNo, block ));

    return( SAPDB_FALSE );
}

/*---------------------------------------------------------------------------*/

void
Converter_Converter::CheckAndMigrateConfigurationParameter(
    const SAPDB_UInt volumeNoBitCount ) const
{
    SAPDBTRACE_ROUTINE_DEBUG( "Converter::CheckAndMigrate", Converter_Trace, 7 );

    SAPDBTRACE_WRITELN( Converter_Trace, 7, "given volumeNoBitCount: " << volumeNoBitCount );
    SAPDBTRACE_WRITELN( Converter_Trace, 7, "configured volumeNoBitCount: " << GetVolumeNoBitCount());


    if(( 0 != volumeNoBitCount ) && ( volumeNoBitCount != GetVolumeNoBitCount()))
    {
        // configuration parameter VOLUMENO_BIT_COUNT is different
        // between configuration file and restart record

        Converter_Exception errMsg( __CONTEXT__, CONVERTER_ERR_BIT_COUNT_MISSMATCH,
                                    SAPDB_ToString( volumeNoBitCount, _T_d ),
                                    SAPDB_ToString( GetVolumeNoBitCount(),  _T_d ));
        RTE_Crash( errMsg );
    }

    Kernel_Version::VersionString   kernelVersion;
    kb57GetRestartRecordKernelVersion( kernelVersion );

    Kernel_Migration& migration = Kernel_IAdminConfig::Instance().MigrationHandler();
    Msg_List          errMsg;
    if ( ! migration.CheckCompatibility( Kernel_Migration::converter, kernelVersion, errMsg ))
    {
        errMsg.Overrule(Converter_Exception( __CONTEXT__, CONVERTER_ERR_MIGRATION_CHECK ));
        RTE_Crash( errMsg );
    }

    const SAPDB_Char* changeTitle = "CONVERTER VOLUME NO BIT COUNT";

    if ( migration.SearchForChangeInfo( changeTitle ).IsEnabled() )
    {
        kb57SetVolumeNoBitCount( GetVolumeNoBitCount());
        migration.CollectMigrationInfo (Kernel_Migration::converter, changeTitle);
    }
}

/*---------------------------------------------------------------------------*/

void
Converter_Converter::AdjustDataBaseSize(
    const RTETask_ITask &task,
    const SAPDB_Int    numPagesAdded )
{
    m_NumAllPages +=numPagesAdded;
    m_DBUsageMonitor.Rescale ( m_NumAllPages );
    SetSecuritySpaceLimit();
    if (numPagesAdded > 0)
    {
        m_DBFullWaitQueue.ResumeAllWaitingTasks( task.ID());
    }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
