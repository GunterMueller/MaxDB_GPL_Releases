/*!
  @file     IOMan_Manager.cpp
  @author   TorstenS
  @ingroup  IOManagement
  @brief    This module is used to handle all volumes of the kernel.
            The IOMan_Manager class offers the possibility to manage 
            data and log volumes. This contains the facility to create,
            open, close, read and write from and to volumes.

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

#include "ggg00.h"   // PASCAL: GG_basic_constants_and_tyype
#include "ggg01.h" // e_invalid_oid
#include "hbd73.h"
#include "hkb57.h"
#include "hgg01_3.h"
#include "vsp002.h"
#include "heo58.h"   // vget_uniqe_id
#include "heo79_1.h"  // RTE: vxparam_save_good_config
#include "hbd01_1.h" // b01niltree_id
#include "hbd13_3.h" // bd13LoadNode


#include "FreeBlockManagement/FBM_Types.hpp"
#include "FreeBlockManagement/FBM_IManager.hpp"

#include "FileDirectory/FileDir_ISharedDirectory.hpp"

#include "Converter/Converter_IPageNoMapping.hpp"
#include "Converter/Converter_ISavePoint.hpp"
#include "Converter/Converter_IDropVolume.hpp"
#include "Converter/Converter_ICommon.hpp"
#include "Converter/Converter_Types.hpp"

#include "DataAccess/Data_PageNoList.hpp"

#include "IOManager/IOMan_MessageList.hpp"
#include "IOManager/IOMan_Manager.hpp"
#include "IOManager/IOMan_Messages.hpp"
#include "IOManager/IOMan_Migration.hpp"
#include "IOManager/IOMan_Types.hpp"
#include "IOManager/IOMan_ComplexClusterAddress.hpp"

#include "KernelCommon/Kernel_FileIO.hpp"
#include "KernelCommon/ParameterNames/KernelParam_IOManager.hpp"
#include "KernelCommon/ParameterNames/KernelParam_RunTime.h" // Instance_Type
#include "KernelCommon/Kernel_IAdminRestartShutdown.hpp"
#include "KernelCommon/Kernel_Trace.hpp"

#include "Logging/Log_Savepoint.hpp"

#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include "RunTime/RTE_Crash.hpp"
#include "RunTime/RTE_Message.hpp"
#include "RunTime/Configuration/RTEConf_ParameterAccess.hpp"
#include "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"

#include "ServerTasks/SrvTasks_JobDropVolume.hpp"
#include "ServerTasks/SrvTasks_JobList.hpp"
#include "ServerTasks/SrvTasks_WaitingTask.hpp"
#include "ServerTasks/SrvTasks_ICoordinator.hpp"

#include "SAPDBCommon/SAPDB_ToString.hpp"

#include "Trace/Trace_DevSpaceIOEntry.hpp"
#include "Transaction/Trans_Context.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/// count current io for read statistic
#define IOMAN_READ_IO   1
/// minimum number of data pages
#define IOMAN_MIN_REQUIRED_DATA_PAGES   2000
/// minimum number of data pages available after drop volume finished
#define IOMAN_DROP_MIN_REQUIRED_PAGES_ON_VOLUME   200

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/

IOMan_Manager*  IOMan_Manager::m_Instance = NULL;

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/


/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/

/*!
    @brief  This method is used to read the log mode information from
            the configuration file. 
    @return  (SAPDB_Bool) true, if the log is mirrored
 */

static
SAPDB_Bool IsMirroredLogConfigured()
{

    RTEConf_Parameter::String   paramValue;
    SAPDBErr_MessageList        errMsg;

    if( ! RTEConf_ParameterAccess::Instance()->GetString(
                KERNELPARAM_LOG_MIRRORED, paramValue, errMsg ))
    {
        RTE_Crash( errMsg );
    }

    if( 0 == strcmp( "NO", ( SAPDB_Char* ) paramValue ))
        return SAPDB_FALSE; // LOG_MIRRORED == NO
    else
        return SAPDB_TRUE; // LOG_MIRRORED == YES
}

/*---------------------------------------------------------------------------*/

/*!
    @brief  This method is used to get the database instance type from
            the configuration file. 
    @return  (SAPDB_Bool) true, if instance type is archive
 */

static
SAPDB_Bool IsArchiveInstanceConfigured()
{
    RTEConf_Parameter::String   paramValue;
    SAPDBErr_MessageList        errMsg;

    if( ! RTEConf_ParameterAccess::Instance()->GetString(
                KERNELPARAM_INSTANCE_TYPE, paramValue, errMsg ))
    {
        RTE_Crash( errMsg );
    }

    if( 0 == strcmp( "ARCHIVE", ( SAPDB_Char* ) paramValue ))
    {
        return SAPDB_TRUE; // INSTANCE_TYPE == ARCHIVE
    }
    else
    {
        return SAPDB_FALSE; // INSTANCE_TYPE != ARCHIVE
    }
}
/*---------------------------------------------------------------------------*/

/*!
    @brief  This method is used to get the maximum configured log and
            data volume count from the configuration file. 
    @param  paramName [in] wanted configuration parameter
    @return (SAPDB_UInt) maximum configured data volumes respl.
                         maximum configured log volumes.
 */

static SAPDB_UInt
GetIntConfiguredValue( const RTEConf_Parameter::Name paramName )
{
    RTEConf_Parameter::Integer  paramValue;
    SAPDBErr_MessageList        errMsg;

    if( ! RTEConf_ParameterAccess::Instance()->GetInteger(
                paramName, paramValue, errMsg ))
    {
        RTE_Crash( errMsg );
    }

    return(SAPDB_UInt) paramValue;
}

/*---------------------------------------------------------------------------*/

/*!
    @brief  This method is used to maintain the statistic counter 
            for physical data read i/o.
    @param  page [in] data page to be written.
    @return none
 */

static void
DataIOStatisticForPhysicalRead( const Data_BasePage   &page )
{
    const SAPDB_Bool1 bIsCatalog = page.GetPageMode().includes( pmCatalog_egg00 );
    const SAPDB_Bool1 bIsBlob    = ( pt2String_egg00 == page.ReadPageType2());

    bd73DataIOStatisticForPhysicalRead( page.GetLevel(),
                                        page.RecoveryMode().PageIsRecoverable(),
                                        bIsBlob, bIsCatalog );
}

/*---------------------------------------------------------------------------*/

/*!
    @brief  This method is used to maintain the statistic counter 
            for physical data write i/o.
    @param  page [in] data page to be written.
    @return none
 */

static void
DataIOStatisticForPhysicalWrite( const Data_BasePage   &page )
{
    const SAPDB_Bool1 bIsCatalog = page.GetPageMode().includes( pmCatalog_egg00 );
    const SAPDB_Bool1 bIsBlob    = ( pt2String_egg00 == page.ReadPageType2());

    bd73DataIOStatisticForPhysicalWrite( page.GetLevel(),
                                         page.RecoveryMode().PageIsRecoverable(),
                                         bIsBlob, bIsCatalog );
}


/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

IOMan_Manager::IOMan_Manager( SAPDBMem_IRawAllocator  &allocator )
        :
        m_Allocator( UTF8( "IOMan_Manager" ), allocator ),
        m_PageAllocator(),
        m_DataArea( m_PageAllocator, m_Allocator, ReadDataIOBlockCount() ),
        m_LogArea( m_PageAllocator, m_Allocator, IsMirroredLogConfigured(), ReadLogIOBlockCount() ),
        m_IsArchiveInstance( IsArchiveInstanceConfigured() ),
        m_AbortDropVolume(IOMan_Okay),
        m_DropVolumeNo()
{};

/*---------------------------------------------------------------------------*/

bool
IOMan_Manager::CreateInstance( RTETask_ITask &task )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_Manager::CreateInstance", IOMan_Trace, 5 );

    if( 0 == m_Instance )
    {
        m_Instance = new( RTEMem_Allocator::Instance() )
                     IOMan_Manager( RTEMem_Allocator::Instance() );

        if( 0 == m_Instance ){
            return false;
        }
        if( ! m_Instance->Initialize( task, ReadMaxDataVolumeCount(),
                                      ReadMaxLogVolumeCount()))
        {
            return false;
        }
    }
    return true;
}

/*---------------------------------------------------------------------------*/

bool
IOMan_Manager::Initialize(
    RTETask_ITask&      task,
    const SAPDB_UInt    maxDataVolumes,
    const SAPDB_UInt    maxLogVolumes)
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_Manager::Initialize", IOMan_Trace, 5 );

    if(! m_DataArea.Initialize( task, maxDataVolumes)){
        return false;
    }
    if(! m_LogArea.Initialize( task.ID(), maxLogVolumes)){
        return false;
    }
    return true;
}

/*---------------------------------------------------------------------------*/

void
IOMan_Manager::CloseAllLogVolumes( const tsp00_TaskId  taskId )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_Manager::CloseAllLogVolumes", IOMan_Trace, 5 );

    m_LogArea.CloseAll( taskId );
}

/*---------------------------------------------------------------------------*/

void
IOMan_Manager::CloseAllDataVolumes( const tsp00_TaskId  taskId )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_Manager::CloseAllDataVolumes", IOMan_Trace, 5 );

    RTETask_ITask&  task = *RTETask_ITask::ByTaskID( taskId );
    m_DataArea.CloseAll( task );
    FBM_IManager::Instance().Shutdown( taskId );
}

/*---------------------------------------------------------------------------*/

bool
IOMan_Manager::CreateAllLogVolumes( const tsp00_TaskId   taskId )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_Manager::CreateAllLogVolumes", IOMan_Trace, 5 );

    tsp00_Line          dbIdentifier;
    const SAPDB_Byte    *pDbIdentifier = reinterpret_cast<SAPDB_Byte*>( &dbIdentifier );

    vget_uniqe_id( dbIdentifier );

    return m_LogArea.CreateAll( taskId, pDbIdentifier );
}

/*---------------------------------------------------------------------------*/

bool
IOMan_Manager::CreateAllDataVolumes( const tsp00_TaskId  taskId )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_Manager::CreateAllDataVolumes", IOMan_Trace, 5 );

    tsp00_Line          dbIdentifier;
    const SAPDB_Byte*   pDbIdentifier = reinterpret_cast<SAPDB_Byte*>( &dbIdentifier );

    vget_uniqe_id( dbIdentifier );

    RTETask_ITask&  task = *RTETask_ITask::ByTaskID( taskId );
    // Create, open and format all configured data volumes
    if(! m_DataArea.CreateAll( task, pDbIdentifier ))
        return false;

    // Check data volume size in regard to nonsense configuration
    if( IOMAN_MIN_REQUIRED_DATA_PAGES > m_DataArea.TotalDataPages() )
    {
        IOMan_MessageList errMsg( __CONTEXT__, IOMAN_MIN_DATA_PAGES,
                                  SAPDB_ToString( IOMAN_MIN_REQUIRED_DATA_PAGES, _T_d ),
                                  SAPDB_ToString( m_DataArea.TotalDataPages(), _T_d ));
        RTE_Crash( errMsg );
    }

    m_RestartPageAddress = IOMan_BlockAddress( m_DataArea.GetFirstConfiguredVolumeNo(), 1 );
    m_DataArea.Get( m_DataArea.GetFirstConfiguredVolumeNo()).SetRestartPageAddress( task, m_RestartPageAddress );

    const bool bInitial = true;
    return this->RestartAndInitializeFBM( task , bInitial );
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
IOMan_Manager::MigrateVolumes(
    RTETask_ITask&          task,
    SAPDBErr_MessageList&   messageList )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_Manager::MigrateVolumes", IOMan_Trace, 5 );

    {
        IOMan_MessageList infoMsg( __CONTEXT__, IOMAN_INFO_START_VOLUME_MIGRATION );
        RTE_Message( infoMsg );
    }

    IOMan_MessageList   msgList;
    IOMan_ReturnCode    retCode = m_DataArea.MigrateCapacity( task, msgList );

    if( IOMan_Okay == retCode ){
        retCode = m_LogArea.MigrateCapacity( task.ID(), msgList );
    }

    if( IOMan_Okay != retCode )
    {
        IOMan_MessageList infoMsg( __CONTEXT__, IOMAN_ERR_STOP_VOLUME_MIGRATION );
        RTE_Message( infoMsg );

        messageList.AppendNewMessage( msgList );

        return SAPDB_FALSE;
    }
    {
        // Try to remove old configuration files
        tsp00_ErrText       errText;
        tsp00_XpReturnCode  retCode;
        const SAPDB_Bool    bIsInRestartPhase = SAPDB_TRUE;

        vxparam_save_good_config( ! bIsInRestartPhase, errText, retCode  );

        if( xp_ok != retCode )
        {
            IOMan_MessageList errMsg( __CONTEXT__, IOMAN_WRN_SAVE_PARAM_FAILED,
                                      SAPDB_ToString( errText, errText.length(), errText.length()));
            RTE_Message( errMsg );
        }
    }
    {
        IOMan_MessageList infoMsg( __CONTEXT__, IOMAN_INFO_STOP_VOLUME_MIGRATION );
        RTE_Message( infoMsg );
    }

    return SAPDB_TRUE;
}

/*---------------------------------------------------------------------------*/

IOMan_BlockCount
IOMan_Manager::GetLogVolumeUsableSize( const IOMan_DeviceNo   devNo ) const
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_Manager::GetLogVolumeUsableSize", IOMan_Trace, 5 );

    if( 1 == devNo )
        return m_LogArea.Get( devNo ).GetUsableSize() -1; // log info page
    else
        return m_LogArea.Get( devNo ).GetUsableSize();
}

/*---------------------------------------------------------------------------*/

bool
IOMan_Manager::OpenAllLogVolumes(
    const tsp00_TaskId taskId,
    const SAPDB_Bool   bOpenForWrite )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_Manager::OpenLogArea", IOMan_Trace, 5 );

    if( m_LogArea.OpenAll( taskId, bOpenForWrite )){
        return true;
    }

    m_LogArea.CloseAll( taskId );
    return false;
}

/*---------------------------------------------------------------------------*/

bool
IOMan_Manager::OpenAllDataVolumes( const tsp00_TaskId taskId )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_Manager::OpenDataArea", IOMan_Trace, 5 );

    RTETask_ITask&  task = *RTETask_ITask::ByTaskID( taskId );
    if( m_DataArea.OpenAll( task ))
    {
        m_RestartPageAddress = this->ReadRestartPageAddress( task.ID());

        kb57RestoreRestartRecord( task.ID());

        SAPDBTRACE_WRITELN( IOMan_Trace, 5, "old cluster block count: " << kb57GetClusterBlockCount() );
        SAPDBTRACE_WRITELN( IOMan_Trace, 5, "cluster reserved space: " << kb57GetClusterReservedSpace() );

        // test if DATA_IO_BLOCK_COUNT has changed
        if (0 == kb57GetClusterBlockCount()) // initialize / migration from older release / no check!
        {
            SAPDBTRACE_WRITELN( IOMan_Trace, 5, "set new cluster block count: " << GetDataIOBlockCount() );
            kb57SetClusterBlockCount(GetDataIOBlockCount());
        }
        else
        {
            SAPDBTRACE_WRITELN( IOMan_Trace, 5, "keep old cluster block count: " << kb57GetClusterBlockCount() );
            if (kb57GetClusterBlockCount() != GetDataIOBlockCount()) // value has changed?
            {
                if (0 != kb57GetClusterReservedSpace())  // if we have allocated clusters, throw error!
                {
                    IOMan_MessageList errMsg( __CONTEXT__, IOMAN_INVALID_DATA_IO_BLOCK_COUNT,
                                              SAPDB_ToString(kb57GetClusterBlockCount(), _T_d ),
                                              SAPDB_ToString( GetDataIOBlockCount(), _T_d ));
                    RTE_Crash( errMsg );
                }
                else // no clusters allocate -> change config
                {
                    SAPDBTRACE_WRITELN( IOMan_Trace, 5, "set changed cluster block count: " << GetDataIOBlockCount() );
                    kb57SetClusterBlockCount(GetDataIOBlockCount());
                }
            }
        }

        if( this->RestartAndInitializeFBM( task )){
            return true;
        }
        else
        {
            if( IOMan_Check.ChecksLevel( 1 ))
            {
                IOMan_MessageList errMsg( __CONTEXT__, IOMAN_INVALID_DATA_IO_BLOCK_COUNT,
                                          SAPDB_ToString(kb57GetClusterBlockCount(), _T_d ),
                                          SAPDB_ToString( GetDataIOBlockCount(), _T_d ));
                RTE_Crash( errMsg );
            }
        }
    }
    m_DataArea.CloseAll( task );
    return false;
}

/*---------------------------------------------------------------------------*/

bool
IOMan_Manager::OpenOneDataVolume(
    const tsp00_TaskId    taskId,
    const IOMan_DeviceNo  devNo )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_Manager::OpenOneDataVolume", IOMan_Trace, 5 );

    RTETask_ITask&  task = *RTETask_ITask::ByTaskID( taskId );
    return IOMan_Okay == ( m_DataArea.Get( devNo ).Open( task ));
}

/*---------------------------------------------------------------------------*/
bool
IOMan_Manager::OpenOneLogVolume(
    const tsp00_TaskId    taskId,
    const IOMan_DeviceNo  devNo )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_Manager::OpenOneLogVolume", IOMan_Trace, 5 );

    return IOMan_Okay == ( m_LogArea.Get( devNo ).Open( taskId, 0, SAPDB_TRUE ));
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
IOMan_Manager::AddDataVolume(
    const tsp00_TaskId      taskId,
    tsp00_VFilename         &devName,
    const IOMan_BlockCount  devSize,
    const IOMan_DeviceNo    devNo )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_Manager::AddDataVolume", IOMan_Trace, 5 );

    RTETask_ITask&  task = *RTETask_ITask::ByTaskID( taskId );
    bool            retCode            = true;
    const bool      bDataAreaRestarted = m_DataArea.IsRestarted();

    if( ! bDataAreaRestarted ){
        retCode = m_DataArea.OpenAll( task );
    }
    if( retCode ){
        retCode = m_DataArea.AddVolume( task, ! bDataAreaRestarted, devName,
                                        devSize, devNo, m_RestartPageAddress );
    }
    if( ! bDataAreaRestarted ){
        m_DataArea.CloseAll( task );
    }
    return retCode ;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
IOMan_Manager::AddLogVolume(
    const tsp00_TaskId          taskId,
    tsp00_VFilename             &devName,
    tsp00_VFilename             &mirrDevName,
    const IOMan_BlockCount      devSize,
    const IOMan_DeviceNo        devNo,
    IOMan_LogPages              &pages,
    IOMan_BlockCount            &newLogSize )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_Manager::AddLogVolume", IOMan_Trace, 5 );

    bool retCode = true;
    const bool bLogAreaRestarted = m_LogArea.ConfiguredVolumes() == m_LogArea.UsedVolumes();


    if( ! bLogAreaRestarted )
        retCode = m_LogArea.OpenAll( taskId, SAPDB_TRUE );

    if( retCode )
    {
        retCode = m_LogArea.AddVolume( taskId, devName, mirrDevName,
                                       devSize, devNo, pages );

        if( retCode ){
            newLogSize = m_LogArea.TotalUsableLogPages();
        }
    }

    if( ! bLogAreaRestarted )
        m_LogArea.CloseAll( taskId ); // PTS 1112474 TS 2001-11-07

    return retCode;
}

/*---------------------------------------------------------------------------*/

const IOMan_BlockAddress&
IOMan_Manager::GetRestartPageAddress( const tsp00_TaskId taskId )
{
    if( ! m_RestartPageAddress.IsValid())
    {
        const IOMan_DeviceNo firstVol = m_DataArea.GetFirstConfiguredVolumeNo();

        if (!OpenOneDataVolume( taskId, firstVol ))
        {
            return m_RestartPageAddress;
        }
        else
        {
            m_RestartPageAddress = m_DataArea.Get( firstVol ).GetRestartPageAddress();
            m_DataArea.Get( firstVol ).Close( taskId );

            if(
                (! m_RestartPageAddress.IsValid()) ||
                ( m_RestartPageAddress.GetDeviceNo() > m_DataArea.MaxVolumes()) ||
                ( m_RestartPageAddress.GetDeviceNo() < IOMan_DataArea::GetFirstDataVolumeNo())
            )
            {
                IOMan_MessageList errMsg( __CONTEXT__, IOMAN_BAD_RESTART_PAGE_ADDRESS,
                                          KERNEL_DATA_FILE, SAPDB_ToString( m_RestartPageAddress.GetDeviceNo(), _T_d ),
                                          SAPDB_ToString( m_RestartPageAddress.GetBlockNo(), _T_d ));
                RTE_Crash( errMsg );
            }
        }
    }
    return m_RestartPageAddress;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
IOMan_Manager::MoveRestartPage( const tsp00_TaskId  taskId )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_Manager::MoveRestartPage", IOMan_Trace, 5 );

    RTETask_ITask&           task = *RTETask_ITask::ByTaskID( taskId );
    const bool               bReqSequential = false;
    const IOMan_BlockAddress newBlock       = FBM_IManager::Instance().GetFreeBlock( task.ID() , bReqSequential );
    const IOMan_BlockAddress oldBlock       = m_RestartPageAddress;

    SAPDBTRACE_WRITELN( IOMan_Trace, 5, "New Address: " << newBlock.GetDeviceNo() << "/" << newBlock.GetBlockNo() );

    IOMan_MessageList infoMsg( __CONTEXT__, IOMAN_INFO_DROP_MOVE_RESTART_PAGE,
                               SAPDB_ToString( newBlock.GetDeviceNo(), _T_d ),
                               SAPDB_ToString( newBlock.GetBlockNo(), _T_d ));
    RTE_Message( infoMsg );

    m_RestartPageAddress = newBlock;
    k57save_restartrecord (taskId);

    m_DataArea.Get( m_DataArea.GetFirstConfiguredVolumeNo()).SetRestartPageAddress( task, m_RestartPageAddress );
    FBM_IManager::Instance().SetBlockStateToFreeAfterSVP( task.ID(), oldBlock );

    return true;
}

/*---------------------------------------------------------------------------*/

IOMan_ReturnCode
IOMan_Manager::DropDataVolume(
    Trans_Context           &trans,
    Msg_List                &msgList,
    const IOMan_DeviceNo    volNo )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_Manager::DropDataVolume", IOMan_Trace, 5 );

    SAPDBERR_ASSERT_STATE( !FBM_IManager::Instance().IsVolumeToDrop(volNo) );

    SAPDB_UInt4 maxUsableServerCount = 4;  // want to use 4 server tasks for dropping
    m_AbortDropVolume = IOMan_Okay;        // initialize

    if( ! IsDataVolumeOnline( volNo ))
    {
        msgList.AppendNewMessage( IOMan_MessageList( __CONTEXT__,
                                  IOMAN_INFO_DROP_VOLUME_IS_OFFLINE, SAPDB_ToString( volNo, _T_d )));
        return IOMan_DropNotPossible;
    }

    // we can't drop a single volume / the volume with the restart record
    if( 1 == ConfiguredDataVolumes())
    {
        msgList.AppendNewMessage( IOMan_MessageList( __CONTEXT__, IOMAN_INFO_ONLY_ONE_VOLUME ));
        return IOMan_DropNotPossible;
    }

    // check snapshot -> not possible to drop volumes with snapshots enabled!
    if( kb57IsSnapShotDefined())
    {
        msgList.AppendNewMessage( IOMan_MessageList( __CONTEXT__, IOMAN_INFO_DROP_NOT_WITH_SNAPSHOT ));
        return IOMan_DropNotPossible;
    }

    // check space on remaining volumes
    const IOMan_BlockCount newNumberOfFreeBlocks =
        FBM_IManager::Instance().GetNumberOfFreeBlocks() - GetDataVolumeSize( volNo );

    if( IOMAN_DROP_MIN_REQUIRED_PAGES_ON_VOLUME >= newNumberOfFreeBlocks )
    {
        msgList.AppendNewMessage( IOMan_MessageList( __CONTEXT__, IOMAN_INFO_REMAINNING_SPACE_TOO_SMALL ));
        return IOMan_DropNotPossible;
    }

    if( IOMAN_MIN_REQUIRED_DATA_PAGES > m_DataArea.TotalDataPages() -  GetDataVolumeSize( volNo ))
    {
        msgList.AppendNewMessage(
            IOMan_MessageList( __CONTEXT__, IOMAN_MIN_DATA_PAGES,
                               SAPDB_ToString( IOMAN_MIN_REQUIRED_DATA_PAGES, _T_d ),
                               SAPDB_ToString( m_DataArea.TotalDataPages() - GetDataVolumeSize( volNo ) , _T_d )));
        return IOMan_DropNotPossible;
    }

    m_DropVolumeNo = volNo;

    // force savepoint reason: dropVolume
    if ( Log_Savepoint::ok != Log_Savepoint::Instance().StartSavepointAndWait(
                trans.Task(), Log_SVPReasonStartDropVolume, msgList ))
    {
        m_DropVolumeNo = IOMan_DeviceNo(); // invalid
        msgList.AppendNewMessage( IOMan_MessageList( __CONTEXT__, IOMAN_ERR_START_SVP_FAILED_DROP));
        return IOMan_DropFailed;
    }

    // scan converter, read pages, mark as dirty
    SrvTasks_WaitingTask                     waitingTask (trans.Task());
    SrvTasks_JobList<SrvTasks_JobDropVolume> dropJobs    (m_Allocator);
    SrvTasks_JobDropVolume                   *newDropJob;

    Converter_PageNoIterator pageIter(volNo, trans.Task(), m_Allocator);

    IOMan_MessageList StartDropVolume(__CONTEXT__, IOMAN_INFO_START_DROP_VOLUME,
                                      SAPDB_ToString(volNo, _T_d));
    RTE_Message (StartDropVolume);

    for (SAPDB_UInt loop = 0; loop < maxUsableServerCount; loop++)
    {
        newDropJob = new (m_Allocator) SrvTasks_JobDropVolume ( waitingTask, pageIter);
        if ( 0 == newDropJob )
        {
            AbortDropDataVolume(trans.Task(), msgList, IOMan_NoMoreMemory);
            return IOMan_NoMoreMemory;
        }

        SrvTasks_Job::ExecuteByServerResult execResult = newDropJob->ExecuteByServer(trans.Task(), msgList, &waitingTask);
        if ( execResult == SrvTasks_Job::ok )
        {
            if (!dropJobs.InsertEnd(newDropJob))
            {
                destroy (newDropJob, m_Allocator);
                AbortDropDataVolume(trans.Task(), msgList, IOMan_DropFailed);
                return IOMan_DropFailed;
            }
        }
        else
        {
            destroy (newDropJob, m_Allocator);
            if (execResult != SrvTasks_Job::executeNoServerAvailable )
            {
                AbortDropDataVolume(trans.Task(), msgList, IOMan_DropFailed);
                return IOMan_DropFailed;
            }
        }
    }

    if (!dropJobs.IsEmpty())  // server tasks are running
    {
        bool                 isAborted = false;
        SrvTasks_Job::Result jobresult;
        dropJobs.WaitForAll(waitingTask, isAborted, jobresult, msgList);

        if ( isAborted || (m_AbortDropVolume != IOMan_Okay) )
        {
            AbortDropDataVolume(trans.Task(), msgList, m_AbortDropVolume);
            return m_AbortDropVolume;
        }
    }
    else // no server task available, do it now
    {
        if (!SrvTaskDropVolume( trans.Task(), trans.ErrorList(), pageIter))
        {
            AbortDropDataVolume(trans.Task(), msgList, m_AbortDropVolume);
            return m_AbortDropVolume;
        }
    }


    IOMan_MessageList StopDropVolume(__CONTEXT__, IOMAN_INFO_STOP_DROP_VOLUME,
                                     SAPDB_ToString(volNo, _T_d));
    RTE_Message (StopDropVolume);

    if (trans.Task().CancelPending())
    {
        AbortDropDataVolume(trans.Task(), msgList, m_AbortDropVolume);
        return m_AbortDropVolume;
    }

    // move savepoint code to 'RemoveDroppedDataVolume'
    // we have to wait for backup to finish

    return IOMan_Okay;
}

/*---------------------------------------------------------------------------*/

bool
IOMan_Manager::PrepareVolumeForDrop(
    const RTETask_ITask     &task,
    Msg_List                &msgList)
{
    // ioman/fbm: set volume to read only

    if( FBM_IManager::Instance().PrepareVolumeForDrop( task, msgList, m_DropVolumeNo ))
    {
        RTE_Message( IOMan_MessageList( __CONTEXT__,
                                        IOMAN_INFO_VOLUME_SET_TO_READ_ONLY,
                                        SAPDB_ToString( m_DropVolumeNo, _T_d )));
        return true;
    }

    msgList.AppendNewMessage( IOMan_MessageList( __CONTEXT__,
                              IOMAN_ERR_CANT_PREPARE_DROP_VOLUME,
                              SAPDB_ToString( m_DropVolumeNo, _T_d )));
    m_DropVolumeNo = IOMan_VolumeNo(); // invalid

    return false;
}

/*---------------------------------------------------------------------------*/

bool
IOMan_Manager::AbortDropDataVolume(
    RTETask_ITask           &task,
    Msg_List &msgList,
    const IOMan_ReturnCode reason)
{
    if (!m_DropVolumeNo.IsValid())
    {
        return false;  // gehen sie weiter, hier gibt es nichts zu sehen
    }


    // stop server tasks
    m_AbortDropVolume = reason;

    IOMan_DeviceNo volNo = m_DropVolumeNo;
    m_DropVolumeNo = IOMan_DeviceNo(); // invalid

    FBM_IManager::Instance().RestoreVolumeForDrop( task, msgList, volNo );

    if (m_AbortDropVolume == IOMan_Canceled)
    {
        msgList.AppendNewMessage( IOMan_MessageList( __CONTEXT__,
                                  IOMAN_INFO_CANCEL_DROP_VOLUME,
                                  SAPDB_ToString( volNo, _T_d )));
    }
    else
    {
        if (m_AbortDropVolume == IOMan_DropDBFull)
        {
            msgList.AppendNewMessage( IOMan_MessageList( __CONTEXT__,
                                      IOMAN_INFO_DBFULL_DROP_VOLUME,
                                      SAPDB_ToString( volNo, _T_d )));
        }
        else
        {
            msgList.AppendNewMessage( IOMan_MessageList( __CONTEXT__,
                                      IOMAN_ERR_DROP_FAILED,
                                      SAPDB_ToString( volNo, _T_d ),
                                      SAPDB_ToString( reason )));
        }
    }

    return true;
}

/*---------------------------------------------------------------------------*/

IOMan_ReturnCode
IOMan_Manager::RemoveDroppedDataVolume(
    RTETask_ITask           &task,
    Msg_List                &msgList,
    const IOMan_DeviceNo    volNo )
{
    SAPDBERR_ASSERT_STATE( FBM_IManager::Instance().IsVolumeToDrop(volNo) );

    // force savepoint reason: dropVolume
    if ( Log_Savepoint::ok != Log_Savepoint::Instance().StartSavepointAndWait(
                task, Log_SVPReasonDropVolume, msgList ))
    {
        m_DropVolumeNo = IOMan_DeviceNo(); // invalid
        msgList.AppendNewMessage( IOMan_MessageList( __CONTEXT__, IOMAN_ERR_START_SVP_FAILED_DROP));
        return IOMan_DropFailed;
    }

    m_DropVolumeNo = IOMan_DeviceNo(); // invalid

    if (m_AbortDropVolume == IOMan_Okay)
    {
        const IOMan_BlockCount numUsedBlocks = FBM_IManager::Instance().NumBlocksUsed( volNo );

        if( 1 != numUsedBlocks )
        {
            FBM_IManager::Instance().DumpUsedBlocks( volNo );

            msgList.AppendNewMessage( IOMan_MessageList( __CONTEXT__,
                                      IOMAN_ERR_DATA_VOLUME_NOT_EMPTY,
                                      SAPDB_ToString( volNo, _T_d ),
                                      SAPDB_ToString( numUsedBlocks, _T_d ) ));

            FBM_IManager::Instance().RestoreVolumeForDrop( task, msgList, volNo );
            return IOMan_DropFailed;
        }

        // write restartPageAdress to the first of the remaining data volumes
        if( volNo == m_DataArea.GetFirstConfiguredVolumeNo())
        {
            m_DataArea.Get(m_DataArea.GetConfiguredVolumeNo( 2 )).SetRestartPageAddress( task, m_RestartPageAddress); // new
            m_DataArea.Get( volNo ).SetRestartPageAddress( task, IOMan_BlockAddress(0,0)); // invalidate
        }


        m_DataArea.DropVolume( task, volNo );

        if ( ! FBM_IManager::Instance().DeRegisterVolume( task, msgList, volNo ))
        {
            FBM_IManager::Instance().RestoreVolumeForDrop( task, msgList, volNo );  // fixme: should crash!
            msgList.AppendNewMessage( IOMan_MessageList( __CONTEXT__, IOMAN_ERR_DEREGISTER_DROP_FAILED));
            return IOMan_DropFailed;
        }

        IOMan_MessageList infoMsg( __CONTEXT__, IOMAN_INFO_DROP_VOLUME_FINISHED,
                                   SAPDB_ToString( volNo, _T_d ));
        RTE_Message( infoMsg );

        return IOMan_Okay;
    }
    else
    {
        if (m_AbortDropVolume == IOMan_Canceled)
        {
            msgList.AppendNewMessage( IOMan_MessageList( __CONTEXT__,
                                      IOMAN_INFO_CANCEL_DROP_VOLUME,
                                      SAPDB_ToString( volNo, _T_d )));
        }
        if (m_AbortDropVolume == IOMan_DropDBFull)
        {
            msgList.AppendNewMessage( IOMan_MessageList( __CONTEXT__,
                                      IOMAN_INFO_DBFULL_DROP_VOLUME,
                                      SAPDB_ToString( volNo, _T_d )));
        }
        return m_AbortDropVolume;
    }
}

/*---------------------------------------------------------------------------*/

bool
IOMan_Manager::IsDataVolumeToDrop(
    const IOMan_DeviceNo    volNo ) const
{
    return FBM_IManager::Instance().IsVolumeToDrop(volNo);
}

/*---------------------------------------------------------------------------*/

bool
IOMan_Manager::IsRestartPageOnDropVolume() const
{
    return FBM_IManager::Instance().IsVolumeToDrop(m_RestartPageAddress.GetDeviceNo());
}

/*---------------------------------------------------------------------------*/

bool
IOMan_Manager::SrvTaskDropVolume(
    RTETask_ITask               &task,
    Msg_List                    &msgList,
    Converter_PageNoIterator    &pageIter)
{

    tgg00_FileId tempFileId = b01niltree_id;
    tgg00_FileId permFileId = b01niltree_id;
    tgg00_FileId permStaticFileId = b01niltree_id;

    tempFileId.fileTfn_gg00().becomes( tfnTemp_egg00 );
    tempFileId.fileType_gg00().clear();
    tempFileId.fileType_gg00().addElement (ftsTemp_egg00);

    permFileId.fileTfn_gg00().becomes( tfnTable_egg00 );
    permFileId.fileType_gg00().clear();
    permFileId.fileType_gg00().addElement (ftsPerm_egg00);

    permStaticFileId.fileTfn_gg00().becomes( tfnObj_egg00 );
    permStaticFileId.fileType_gg00().clear();
    permStaticFileId.fileType_gg00().addElement (ftsObject_egg00);

    Data_PageNo             pageNo;
    Data_PageRecoveryMode   recMode;
    tgg00_BasisError        baseError = e_ok; // because of bd13LoadNode

    while (pageIter.GetNextPageNo(pageNo, recMode, task) && (e_ok == baseError) && (m_AbortDropVolume == IOMan_Okay))
    {
        tgg00_RecoveryMode recoveryMode;

        if (recMode.UseStaticPageAddressing())
        {
            recoveryMode = tgg00_RecoveryMode::fromConst(rmStatic_egg00);
        }
        else if (recMode.PageIsRecoverable())
        {
            recoveryMode = tgg00_RecoveryMode::fromConst(rmPerm_egg00);
        }
        else
        {
            recoveryMode = tgg00_RecoveryMode::fromConst(rmTemp_egg00);
        }

        const SAPDB_Int maxRetries = 4;
        SAPDB_Int       retryCount = 0;
        bool            bDoRetry    = false;

        tgg00_PageType pageType = tgg00_PageType::fromConst( ptNil_egg00 );

        const boolean setChanged = true;
        const boolean isClustered = false;  // fixme, check for clustered area in FBM!

        do {
            switch (recoveryMode)
            {
            case rmTemp_egg00:
                bd13LoadNode (task.ID(), baseError, tempFileId, pageType, pageNo, recoveryMode, setChanged, isClustered);
                break;
            case rmPerm_egg00:
                bd13LoadNode (task.ID(), baseError, permFileId, pageType, pageNo, recoveryMode, setChanged, isClustered);
                break;
            case rmStatic_egg00:
                bd13LoadNode (task.ID(), baseError, permStaticFileId, pageType, pageNo, recoveryMode, setChanged, isClustered );
                break;
            default:
                RTE_Crash( SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                                              "IOMan_Manager::SrvTaskDropVolume: Invalid rec mode") );
                break;
            }

            switch (baseError)
            {
            case e_no_converter_entry: // continue
                baseError = e_ok;
                break;

            case e_illegal_page_no:
            case e_shutdown:
            case e_inconsistent_nodetype:
            case e_bad_datapage:
            case e_corrupted_datapage_from_io:
            case e_no_more_memory:
            case e_invalid_oid:
                break;

            case e_sysbuffer_overflow:
                retryCount++;
                task.Sleep( 3 );
                bDoRetry = true;
                break;
            }
        } while ((retryCount < maxRetries) && bDoRetry);

        if( e_ok == baseError && pageType == ptFileDir_egg00 ){
            FileDir_ISharedDirectory::Instance().MarkPageForWrite( task.ID(), pageNo);
        }

        if (task.CancelPending())
        {
            m_AbortDropVolume = IOMan_Canceled;
        }
    }
    if( e_ok == baseError ){
        return true;
    }

    IOMan_MessageList abortDropVolume(__CONTEXT__, IOMAN_ERR_ABORT_DROP_VOLUME,
                                      Kernel_BasisErrorToResultString( baseError ));

    msgList.AppendNewMessage( abortDropVolume );

    return false;
}

/*---------------------------------------------------------------------------*/

void
IOMan_Manager::ReadConverterPage(
    const tsp00_TaskId          taskId,
    const IOMan_BlockAddress    &block,
    Converter_Page              &page )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_Manager::ReadConverterPage", IOMan_Trace, 5 );

    SAPDBTRACE_WRITELN( IOMan_Trace, 5, "ConverterPage: " << block.GetDeviceNo() <<\
                        "/" << block.GetBlockNo() );

    RTETask_ITask&  task = *RTETask_ITask::ByTaskID( taskId );
    const SAPDB_Bool bOkay = m_DataArea.Get( block.GetDeviceNo() ).ReadPage( task, page, block );

    if( g01vtrace.vtrAll_gg00 || g01vtrace.vtrIoTrace_gg00 ){
        Trace_ReadConverterPageEntry( block, 0 );
    }

    if( SAPDB_TRUE == bOkay )
    {
        // Check given block address against the block address
        // within the obtained buffer
        const IOMan_BlockAddress  pageBlock = page.ReadBlockAddress();

        if(
            ( block.GetDeviceNo() == pageBlock.GetDeviceNo() ) &&
            ( block.GetBlockNo() == pageBlock.GetBlockNo() )
        )
        {
            bd73ConverterIOStatistic( IOMAN_READ_IO );
            return;
        }
    }

    // Abnormal termination
    IOMan_MessageList errMsg( __CONTEXT__, IOMAN_WRONG_CONV_PAGE,
                              KERNEL_DATA_FILE, SAPDB_ToString( block.GetDeviceNo(), _T_d ),
                              SAPDB_ToString( block.GetBlockNo(), _T_d ));

    Kernel_FileIO   DumpFile( KERNEL_DATA_FILE, KERNEL_BAD_FILE_EXTENSION,
                              SAPDB_ToString( block.GetDeviceNo(), _T_d ),
                              SAPDB_ToString( block.GetBlockNo(), _T_d ));
    DumpFile.Write( page );
    DumpFile.Close();

    RTE_Crash( errMsg );
}

/*---------------------------------------------------------------------------*/

IOMan_ReturnCode
IOMan_Manager::ReadDataPage(
    const tsp00_TaskId  taskId,
    Data_BasePage       &page,
    const Data_PageNo   &pageNo )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_Manager::ReadDataPage", IOMan_Trace, 5 );

    RTETask_ITask&  task = *RTETask_ITask::ByTaskID( taskId );

    IOMan_BlockAddress          block;
    const Converter_ReturnCode  retCode = Converter_IPageNoMapping::Instance().GetPositionForReading(
                                              task, pageNo, page.RecoveryMode(), block );
    if( Converter_Ok != retCode )
    {
        if( Converter_PageNoOutOfRange == retCode )
            return IOMan_PageNoOutOfRange;
        else
            return IOMan_NoConverterEntry;
    }

    SAPDBERR_ASSERT_STATE( block.IsValid() );

    const SAPDB_Int  cMaxRetries = IOMan_Volume::MaxReadRetries();
    SAPDB_Int        retryCount  = 0;
    do
    {
        if( ! m_DataArea.Get( block.GetVolumeNo()).ReadPage( task, page, block	))
        {
            RTE_Message( IOMan_MessageList( __CONTEXT__, IOMAN_BAD_DATA_PAGE,
                                            SAPDB_ToString( pageNo, _T_d ),           // requested pageNo
                                            SAPDB_ToString( page.RecoveryMode()),     // requested recMode
                                            SAPDB_ToString( page.PageNo(), _T_d )));  // read pageNo
            return IOMan_BadDataPage;
        }

        DataIOStatisticForPhysicalRead( page );

        if( g01vtrace.vtrAll_gg00 || g01vtrace.vtrIoTrace_gg00	){
            Trace_ReadDataPageEntry( page.ReadPageType2(), page.PageNo(), block	);
        }
        //	Check given	pno	against	the	pno	within the obtained	buffer
        if(	pageNo == page.PageNo()){
            return IOMan_Okay;
        }
        { // page is correct, but it is not the requested one!
            IOMan_MessageList errMsg( __CONTEXT__, IOMAN_WRONG_PAGENO2,
                                      SAPDB_ToString( pageNo, _T_d ),          // requested pageNo
                                      SAPDB_ToString( page.RecoveryMode()),    // requested recMode
                                      SAPDB_ToString( page.PageNo(), _T_d ));  // read pageNo

            errMsg.AppendNewMessage( IOMan_MessageList( __CONTEXT__, IOMAN_WRONG_PAGENO,
                                     SAPDB_ToString( block.GetVolumeNo(), _T_d	),
                                     SAPDB_ToString( block.GetBlockNo(), _T_d )));
            RTE_Message( errMsg	);

            Kernel_FileIO	dumpFile( KERNEL_DATA_FILE,	KERNEL_BAD_FILE_EXTENSION,
                                    SAPDB_ToString( pageNo, _T_d ));
            dumpFile.Write(	page );
        }
        retryCount+=1;
        task.Sleep( IOMan_Volume::RetrySleepTime());
    }while( retryCount < cMaxRetries );

    return IOMan_BadDataPage;
}

/*---------------------------------------------------------------------------*/

IOMan_ReturnCode
IOMan_Manager::ReadDataPages(
    RTETask_ITask&                      task,
    const Data_PageNoList&              pageNoList,
    IOMan_ClusteredDataPages&           pages,
    const IOMan_ComplexClusterAddress&  cluster,
    const SAPDB_UInt                    startIndex,
    Data_PageNo&                        badDataPageNo )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_Manager::ReadDataPages", IOMan_Trace, 5 );

    SAPDBERR_ASSERT_STATE( startIndex + SAPDB_UInt( cluster.GetBlockCount()) <= pages.GetPageCount());
    SAPDBERR_ASSERT_STATE( pageNoList.GetSize() == pages.GetPageCount());
    SAPDBERR_ASSERT_STATE( 0 < cluster.GetBlockCount());

    const SAPDB_UInt  cMaxRetries  = IOMan_Volume::MaxReadRetries();
    SAPDB_UInt        retryCount   = 0;
    SAPDB_UInt        badPageIndex = 0;
    bool              bBadPage     = false;

    Msg_List msgList; // TODO torsten

    do
    {
        bBadPage = false;
        if( ! m_DataArea.Get( cluster.GetVolumeNo()).ReadPages( task, msgList, pages, startIndex, cluster, badPageIndex ))
        {
            badDataPageNo = pageNoList[ badPageIndex ];
            const Data_BasePage&  badPage = pages.Get( badPageIndex );
            RTE_Message( IOMan_MessageList( __CONTEXT__, IOMAN_BAD_DATA_PAGE,
                                            SAPDB_ToString( pageNoList[ badPageIndex ], _T_d ), // requested pageNo
                                            SAPDB_ToString( badPage.RecoveryMode()),            // requested recMode
                                            SAPDB_ToString( badPage.PageNo(), _T_d )));         // read pageNo
            return IOMan_BadDataPage;
        }
        IOMan_ClusterAddress::BlockIndex    blockIndex  = 0;
        SAPDB_UInt                          pageNoIndex = startIndex;
        IOMan_DataPagesIterator             iter( pages );
        SAPDB_UInt                          traceNo  = ( cluster.GetBlockCount() > 1 )? 1:0;

        for( blockIndex  = 0, iter.Begin( startIndex ); ! iter.End(), blockIndex < cluster.GetBlockCount();
                ++iter, ++blockIndex, ++pageNoIndex, ++traceNo )
        {
            if( ! cluster.IsBlockInUse( blockIndex )){
                continue; // clustered read with 'empty' pages for better performance
            }
            SAPDBERR_ASSERT_STATE(( *iter ).IsAssigned());
            DataIOStatisticForPhysicalRead( *iter );

            if( g01vtrace.vtrAll_gg00 || g01vtrace.vtrIoTrace_gg00	){
                Trace_ReadDataPageEntry(( *iter ).ReadPageType2(), ( *iter ).PageNo(),
                                        cluster.GetBlockAddress( blockIndex ), traceNo );
            }
            //	Check given	pno	against	the	pno	within the obtained	buffer
            const Data_PageNo&    requestedPageNo = pageNoList[ pageNoIndex ];
            if( requestedPageNo != ( *iter ).PageNo())
            {
                // page is correct, but it is not the requested one!
                IOMan_MessageList errMsg( __CONTEXT__, IOMAN_WRONG_PAGENO2,
                                          SAPDB_ToString( requestedPageNo, _T_d ),     // requested pageNo
                                          SAPDB_ToString( ( *iter ).RecoveryMode()),   // requested recMode
                                          SAPDB_ToString( ( *iter ).PageNo(), _T_d )); // read pageNo

                errMsg.AppendNewMessage( IOMan_MessageList( __CONTEXT__, IOMAN_WRONG_PAGENO,
                                         SAPDB_ToString( cluster.GetVolumeNo(), _T_d ),
                                         SAPDB_ToString( cluster.GetBlockNo( blockIndex ), _T_d )));
                RTE_Message( errMsg	);

                Kernel_FileIO	dumpFile( KERNEL_DATA_FILE,	KERNEL_BAD_FILE_EXTENSION,
                                        SAPDB_ToString( requestedPageNo, _T_d ));
                dumpFile.Write(	*iter );
                bBadPage = true;
                break;
            }
        }
        if( ! bBadPage ){
            return IOMan_Okay;
        }
        retryCount+=1;
        task.Sleep( IOMan_Volume::RetrySleepTime());
    }
    while( retryCount < cMaxRetries );

    return IOMan_BadDataPage;
}

/*---------------------------------------------------------------------------*/

bool
IOMan_Manager::ReadDataPageDirect(
    const tsp00_TaskId         taskId,
    Data_BasePage              &page,
    const IOMan_BlockAddress   &block )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_Manager::ReadDataPageDirect", IOMan_Trace, 5 );

    SAPDBTRACE_WRITELN( IOMan_Trace, 5, "BlockAddress: " <<
                        block.GetDeviceNo() << "/" << block.GetBlockNo());
    RTETask_ITask&  task = *RTETask_ITask::ByTaskID( taskId );
    if(! m_DataArea.Get( block.GetDeviceNo() ).ReadPage( task, page, block )){
        return false;
    }
    if( g01vtrace.vtrAll_gg00 || g01vtrace.vtrIoTrace_gg00 ){
        Trace_ReadDataPageEntry( page.ReadPageType2(), page.PageNo(), block );
    }
    return true;
}

/*---------------------------------------------------------------------------*/

void
IOMan_Manager::ReadLogInfoPage(
    const tsp00_TaskId        taskId,
    Log_InfoPage              &page,
    const Log_ClusterAddress  &address )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_Manager::ReadLogInfoPage", IOMan_Trace, 5 );

    SAPDBTRACE_WRITELN( IOMan_Trace, 5,
                        " LogOffset: " << address.GetOffset() <<
                        " BlockCount: " << address.GetBlockCount() );

    SAPDBERR_ASSERT_STATE( 1 == address.GetBlockCount() );
    SAPDBERR_ASSERT_STATE( address.GetOffset().IsValid() );

    IOMan_BlockAddress  block = m_LogArea.GetBlockAddress( address.GetOffset().RawValue() );

    m_LogArea.Get( block.GetDeviceNo() ).ReadPage( taskId, page, block );

    if( g01vtrace.vtrAll_gg00 || g01vtrace.vtrIoTrace_gg00 ){
        Trace_ReadLogPageEntry( pt2LogInfo_egg00, page.ReadPageId(), block );
    }
}

/*---------------------------------------------------------------------------*/

void IOMan_Manager::ReadLogPageWithoutCheck(
    const tsp00_TaskId        taskId,
    Log_Page                  &page,
    const Log_ClusterAddress  &address )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_Manager::ReadLogPageWithoutCheck", IOMan_Trace, 5 );

    SAPDBTRACE_WRITELN( IOMan_Trace, 5,
                        " LogOffset: " << address.GetOffset() <<
                        " BlockCount: " << address.GetBlockCount() );

    SAPDBERR_ASSERT_STATE( 1 == address.GetBlockCount() );
    SAPDBERR_ASSERT_STATE( address.GetOffset().IsValid() );

    const IOMan_BlockAddress  block = m_LogArea.GetBlockAddress( address.GetOffset().RawValue() );

    if(! m_LogArea.Get( block.GetDeviceNo() ).ReadSimple( taskId,
            tsp00_PageAddr( page.GetFramePtr()), block.GetBlockNo(), true ))
    {
        Kernel_FileIO   dumpFile( m_LogArea.Get( block.GetDeviceNo() ).GetTypeName(),
                                  KERNEL_BAD_FILE_EXTENSION,
                                  SAPDB_ToString( block.GetDeviceNo(), _T_d ),
                                  SAPDB_ToString( block.GetBlockNo(), _T_d ));
        dumpFile.Write( page );
        dumpFile.Close();

        Kernel_IAdminRestartShutdown::Instance().Offline();
    }

    bd73LogIOStatistic( IOMAN_READ_IO );

    if( g01vtrace.vtrAll_gg00 || g01vtrace.vtrIoTrace_gg00 ){
        Trace_ReadLogPageEntry( pt2Nil_egg00, page.ReadLastWriterIOSequenceNo(), block );
    }
}

/*---------------------------------------------------------------------------*/

void
IOMan_Manager::ReadLogPage(
    const tsp00_TaskId       taskId,
    Log_Page                 &page,
    const Log_ClusterAddress &address )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_Manager::ReadLogPage", IOMan_Trace, 5 );

    SAPDBTRACE_WRITELN( IOMan_Trace, 5,
                        " LogOffset: " << address.GetOffset() <<
                        " BlockCount: " << address.GetBlockCount() );

    SAPDBERR_ASSERT_STATE( 1 == address.GetBlockCount() );
    SAPDBERR_ASSERT_STATE( address.GetOffset().IsValid() );

    IOMan_BlockAddress  block = m_LogArea.GetBlockAddress( address.GetOffset().RawValue() );

    m_LogArea.Get( block.GetDeviceNo() ).ReadPage( taskId, page, block );

    bd73LogIOStatistic( IOMAN_READ_IO );

    if( g01vtrace.vtrAll_gg00 || g01vtrace.vtrIoTrace_gg00 ){
        Trace_ReadLogPageEntry( pt2Nil_egg00, page.ReadLastWriterIOSequenceNo(), block );
    }
}

/*---------------------------------------------------------------------------*/

void
IOMan_Manager::ReadLogPages(
    const tsp00_TaskId          taskId,
    IOMan_LogPages              &pages,
    const Log_ClusterAddress    &address )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_Manager::ReadLogPages", IOMan_Trace, 5 );

    SAPDBTRACE_WRITELN( IOMan_Trace, 5,
                        " LogOffset: " << address.GetOffset() <<
                        " BlockCount: " << address.GetBlockCount() );

    SAPDBERR_ASSERT_STATE( pages.GetPageCount() >= address.GetBlockCount());
    SAPDBERR_ASSERT_STATE( 0 < pages.GetPageCount() );
    SAPDBERR_ASSERT_STATE( address.GetOffset().IsValid() );

    Log_DeviceBlockCount pagesToRead = address.GetBlockCount();
    SAPDB_UInt           startIndex  = 0;
    IOMan_ClusterAddress cluster;

    do
    {
        cluster = m_LogArea.GetClusterAddress( address.GetOffset().RawValue() + startIndex, pagesToRead );  //PTS 1125634
        m_LogArea.Get( cluster.GetDeviceNo() ).ReadPages( taskId, pages, cluster, startIndex );

        if( g01vtrace.vtrAll_gg00 || g01vtrace.vtrIoTrace_gg00 )
        {
            SAPDB_UInt                       traceNo  = ( cluster.GetBlockCount() > 1 )? 1:0;
            IOMan_ClusterAddress::BlockIndex index    = 0;
            IOMan_LogPagesIterator           iter( pages );

            for( iter.Begin( startIndex ); !iter.End() && index < cluster.GetBlockCount();
                    ++iter, ++index, ++traceNo )
            {
                Trace_ReadLogPageEntry( pt2Nil_egg00, ( *iter ).ReadLastWriterIOSequenceNo(),
                                        cluster.GetBlockAddress( index ), traceNo );
            }
        }
        pagesToRead -= cluster.GetBlockCount();
        startIndex  += cluster.GetBlockCount();
        bd73LogIOStatistic( IOMAN_READ_IO );
    }
    while( 0 < pagesToRead );
}

/*---------------------------------------------------------------------------*/

void
IOMan_Manager::ReadRestartPage(
    const tsp00_TaskId   taskId,
    Kernel_RestartPage   &page )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_Manager::ReadRestartPage", IOMan_Trace, 5 );

    RTETask_ITask&  task = *RTETask_ITask::ByTaskID( taskId );
    if( ! m_DataArea.Get( m_RestartPageAddress.GetDeviceNo()).ReadPage( task, page, m_RestartPageAddress ))
    {
        IOMan_MessageList ErrMsg( __CONTEXT__, IOMAN_BAD_RESTART_PAGE,
                                  KERNEL_DATA_FILE, SAPDB_ToString( m_RestartPageAddress.GetDeviceNo(), _T_d ),
                                  SAPDB_ToString( m_RestartPageAddress.GetBlockNo(), _T_d ));

        Kernel_FileIO   DumpFile( KERNEL_DATA_FILE, KERNEL_BAD_FILE_EXTENSION,
                                  SAPDB_ToString( m_RestartPageAddress.GetDeviceNo(), _T_d ),
                                  SAPDB_ToString( m_RestartPageAddress.GetBlockNo(), _T_d ));

        DumpFile.Write( page );
        DumpFile.Close();

        RTE_Crash( ErrMsg );
    }

    if( g01vtrace.vtrAll_gg00 || g01vtrace.vtrIoTrace_gg00 ){
        Trace_ReadDataPageEntry( page.ReadPageType2(), page.GetPageID(), m_RestartPageAddress );
    }
}

/*---------------------------------------------------------------------------*/

void
IOMan_Manager::ReadSnapShotRestartPage(
    const tsp00_TaskId          taskId,
    Kernel_RestartPage          &page,
    const IOMan_BlockAddress    &block )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_Manager::ReadSnapShotRestartPage", IOMan_Trace, 5 );

    RTETask_ITask&  task = *RTETask_ITask::ByTaskID( taskId );
    if(! m_DataArea.Get( block.GetDeviceNo()).ReadPage( task, page, block ))
    {
        IOMan_MessageList ErrMsg( __CONTEXT__, IOMAN_BAD_RESTART_PAGE,
                                  KERNEL_DATA_FILE, SAPDB_ToString( block.GetDeviceNo(), _T_d ),
                                  SAPDB_ToString( block.GetBlockNo(), _T_d ));

        Kernel_FileIO   DumpFile( KERNEL_DATA_FILE, KERNEL_BAD_FILE_EXTENSION,
                                  SAPDB_ToString( block.GetDeviceNo(), _T_d ),
                                  SAPDB_ToString( block.GetBlockNo(), _T_d ));

        DumpFile.Write( page );
        DumpFile.Close();

        RTE_Crash( ErrMsg );
    }

    if( g01vtrace.vtrAll_gg00 || g01vtrace.vtrIoTrace_gg00 ){
        Trace_ReadDataPageEntry( page.ReadPageType2(), page.GetPageID(), block );
    }
}

/*---------------------------------------------------------------------------*/

void
IOMan_Manager::WriteConverterPage(
    const tsp00_TaskId  taskId,
    Converter_Page      &page )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_Manager::WriteConverterPage", IOMan_Trace, 5 );

    RTETask_ITask&           task = *RTETask_ITask::ByTaskID( taskId );
    const bool               bReqSequential = false;
    const IOMan_BlockAddress block  = FBM_IManager::Instance().GetFreeBlock( task.ID() , bReqSequential);

    SAPDBTRACE_WRITELN( IOMan_Trace, 5, "New Address: " << block.GetDeviceNo() << "/" << block.GetBlockNo() );

    page.WriteBlockAddress( block );
    page.WriteVersion(Converter_ICommon::Instance().Version());

    m_DataArea.Get( block.GetDeviceNo() ).WritePage( task, page, block );

    bd73ConverterIOStatistic( ! IOMAN_READ_IO );

    if( g01vtrace.vtrAll_gg00 || g01vtrace.vtrIoTrace_gg00 ){
        Trace_WriteConverterPageEntry( block, 0 );
    }
}

/*---------------------------------------------------------------------------*/

void
IOMan_Manager::WriteDataPage(
    const tsp00_TaskId   taskId,
    Data_BasePage        &page )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_Manager::WriteDataPage", IOMan_Trace, 5 );

    RTETask_ITask&  task = *RTETask_ITask::ByTaskID( taskId );
    const bool      bReqSequential =
        m_IsArchiveInstance && ( Data_VolumeSequential == page.VolumeMode());

    // Update the corresponding  converter entries, before the
    // converter version on the data pages will be changed because
    // the converter needs the old converter version to make decisions
    // about updating the old blockaddress.

    IOMan_BlockAddress block;

    Converter_IPageNoMapping::Instance().GetPositionForWriting( task,
            page.PageNo(), page.RecoveryMode(), page.ConverterVersion(),
            bReqSequential, block );

    // Store current converter version within the page image to note
    // the savepoint version when the page was written the last time
    // onto the data volume.
    page.SetConverterVersion( Converter_ICommon::Instance().Version ());

    m_DataArea.Get( block.GetDeviceNo() ).WritePage( task, page, block );
    DataIOStatisticForPhysicalWrite( page );

    if( g01vtrace.vtrAll_gg00 || g01vtrace.vtrIoTrace_gg00 ){
        Trace_WriteDataPageEntry( page.ReadPageType2(), page.PageNo(), block );
    }
}

/*---------------------------------------------------------------------------*/

void
IOMan_Manager::WriteDataPages(
    const tsp00_TaskId          taskId,
    IOMan_DataPages&            pages,
    const IOMan_ClusterAddress& cluster,
    const SAPDB_UInt            startIndex )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_Manager::WriteDataPages", IOMan_Trace, 5 );

    SAPDBERR_ASSERT_STATE( startIndex + SAPDB_UInt( cluster.GetBlockCount()) <= pages.GetPageCount());

    // Update the corresponding  converter entries, before the
    // converter version on the data pages will be changed because
    // the converter needs the old converter version to make decisions
    // about updating the old blockaddres.

    RTETask_ITask&    task = *RTETask_ITask::ByTaskID( taskId );

    IOMan_ClusterAddress::BlockIndex    index = 0;
    IOMan_DataPagesIterator             iter( pages );

    for( iter.Begin( startIndex ); !iter.End() && ( index < cluster.GetBlockCount());
            ++iter, ++index )
    {
        Converter_IPageNoMapping::Instance().SetPositionForWriting (
            task, ( *iter ).ConverterVersion(), ( *iter ).PageNo(),
            ( *iter ).RecoveryMode(), cluster.GetBlockAddress( index ));

        // Store current converter version within the page image to note
        // the savepoint version when the page was written the last time
        // onto the data volume.
        ( *iter ).SetConverterVersion( Converter_ICommon::Instance().Version ());

        DataIOStatisticForPhysicalWrite( *iter );
    }
    m_DataArea.Get( cluster.GetVolumeNo() ).WritePages( task, pages, cluster, startIndex );

    if( g01vtrace.vtrAll_gg00 || g01vtrace.vtrIoTrace_gg00 )
    {
        SAPDB_UInt  traceNo  = ( cluster.GetBlockCount() > 1 )? 1:0;
        IOMan_ClusterAddress::BlockIndex  index = 0;

        for (iter.Begin( startIndex ); !iter.End()&&( index < cluster.GetBlockCount());
                ++iter, ++index, ++traceNo )
        {
            Trace_WriteDataPageEntry( ( *iter ).ReadPageType2(),
                                      ( *iter ).PageNo(), cluster.GetBlockAddress( index ), traceNo );
        }
    }
}

/*---------------------------------------------------------------------------*/

void
IOMan_Manager::WriteLogInfoPage(
    const tsp00_TaskId        taskId,
    Log_InfoPage              &page,
    const Log_ClusterAddress  &address )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_Manager::WriteLogInfoPage", IOMan_Trace, 5 );

    SAPDBTRACE_WRITELN( IOMan_Trace, 5,
                        " LogOffset: " << address.GetOffset() <<
                        " BlockCount: " << address.GetBlockCount() );

    SAPDBERR_ASSERT_STATE( 1 == address.GetBlockCount() );
    SAPDBERR_ASSERT_STATE( address.GetOffset().IsValid() );

    IOMan_BlockAddress  block = m_LogArea.GetBlockAddress( address.GetOffset().RawValue() );

    m_LogArea.Get( block.GetDeviceNo() ).WritePage( taskId, page, block );

    if( g01vtrace.vtrAll_gg00 || g01vtrace.vtrIoTrace_gg00 ){
        Trace_WriteLogPageEntry( pt2LogInfo_egg00, page.ReadPageId(), block );
    }
    bd73LogIOStatistic( ! IOMAN_READ_IO );
}

/*---------------------------------------------------------------------------*/

void
IOMan_Manager::WriteLogPage(
    const tsp00_TaskId        taskId,
    Log_Page                  &page,
    const Log_ClusterAddress  &address )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_Manager::WriteLogPage", IOMan_Trace, 5 );

    SAPDBTRACE_WRITELN( IOMan_Trace, 5,
                        " LogOffset: " << address.GetOffset() <<
                        " BlockCount: " << address.GetBlockCount() );

    SAPDBERR_ASSERT_STATE( 1 == address.GetBlockCount() );
    SAPDBERR_ASSERT_STATE( address.GetOffset().IsValid() );

    IOMan_BlockAddress  block = m_LogArea.GetBlockAddress( address.GetOffset().RawValue() );

    m_LogArea.Get( block.GetDeviceNo() ).WritePage( taskId, page, block );

    if( g01vtrace.vtrAll_gg00 || g01vtrace.vtrIoTrace_gg00 ){
        Trace_WriteLogPageEntry( pt2Nil_egg00, page.ReadLastWriterIOSequenceNo(), block );
    }
    bd73LogIOStatistic( ! IOMAN_READ_IO );
}

/*---------------------------------------------------------------------------*/

void
IOMan_Manager::WriteLogPages(
    const tsp00_TaskId        taskId,
    IOMan_LogPages            &pages,
    const Log_ClusterAddress  &address )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_Manager::WriteLogPages", IOMan_Trace, 5 );

    SAPDBTRACE_WRITELN( IOMan_Trace, 5,
                        " LogOffset: " << address.GetOffset() <<
                        " BlockCount: " << address.GetBlockCount() );

    SAPDBERR_ASSERT_STATE( pages.GetPageCount() == address.GetBlockCount() );
    SAPDBERR_ASSERT_STATE( 0 < pages.GetPageCount() );
    SAPDBERR_ASSERT_STATE( address.GetOffset().IsValid() );

    Log_DeviceBlockCount pagesToWrite = address.GetBlockCount();
    SAPDB_UInt           startOffset  = 0;
    IOMan_ClusterAddress cluster;

    do
    {
        cluster = m_LogArea.GetClusterAddress( address.GetOffset().RawValue() + startOffset, pagesToWrite ); //PTS 1125634
        m_LogArea.Get( cluster.GetDeviceNo() ).WritePages( taskId, pages, cluster, startOffset );

        if( g01vtrace.vtrAll_gg00 || g01vtrace.vtrIoTrace_gg00 )
        {
            SAPDB_UInt                       traceNo  = ( cluster.GetBlockCount() > 1 )? 1:0;
            IOMan_ClusterAddress::BlockIndex index    = 0;
            IOMan_LogPagesIterator           iter( pages );

            for( iter.Begin( startOffset );
                    !iter.End() && index < cluster.GetBlockCount();
                    ++iter, ++index, ++traceNo )
            {
                Trace_WriteLogPageEntry( pt2Nil_egg00, ( *iter ).ReadLastWriterIOSequenceNo(),
                                         cluster.GetBlockAddress( index ), traceNo );
            }
        }
        pagesToWrite -= cluster.GetBlockCount();
        startOffset  += cluster.GetBlockCount();
        bd73LogIOStatistic( ! IOMAN_READ_IO );
    }
    while( 0 < pagesToWrite );
}

/*---------------------------------------------------------------------------*/

void
IOMan_Manager::WriteRestartPage(
    const tsp00_TaskId  taskId,
    Kernel_RestartPage  &page )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_Manager::WriteRestartPage", IOMan_Trace, 5 );

    RTETask_ITask&  task = *RTETask_ITask::ByTaskID( taskId );
    m_DataArea.Get( m_RestartPageAddress.GetDeviceNo()).WritePage( task,
            page, m_RestartPageAddress );

    if( g01vtrace.vtrAll_gg00 || g01vtrace.vtrIoTrace_gg00 ){
        Trace_WriteDataPageEntry( page.ReadPageType2(), page.GetPageID(), m_RestartPageAddress );
    }
}

/*---------------------------------------------------------------------------*/

void
IOMan_Manager::WriteSnapShotRestartPage(
    const tsp00_TaskId  taskId,
    Kernel_RestartPage  &page,
    IOMan_BlockAddress  &block )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_Manager::WriteSnapShotRestartPage", IOMan_Trace, 5 );

    RTETask_ITask&  task = *RTETask_ITask::ByTaskID( taskId );
    const bool      bReqSequential = false;

    block = FBM_IManager::Instance().GetFreeBlock( task.ID() , bReqSequential );

    SAPDBTRACE_WRITELN( IOMan_Trace, 5, "New Address: " << block.GetDeviceNo() << "/" << block.GetBlockNo() );

    m_DataArea.Get( block.GetDeviceNo() ).WritePage( task, page, block );

    if( g01vtrace.vtrAll_gg00 || g01vtrace.vtrIoTrace_gg00 ){
        Trace_WriteDataPageEntry( page.ReadPageType2(), page.GetPageID(), block );
    }
}

/*---------------------------------------------------------------------------*/

bool
IOMan_Manager::ReadPageDirectFromDataVolume(
    const tsp00_TaskId          taskId,
    const IOMan_BlockAddress    &block,
    const tsp00_PageAddr        pPage )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_Manager::ReadPageDirectFromDataVolume", IOMan_Trace, 5 );

    SAPDBTRACE_WRITELN( IOMan_Trace, 5, "BlockAddress: " <<
                        block.GetDeviceNo() << "/" << block.GetBlockNo());

    return m_DataArea.Get( block.GetDeviceNo() ).ReadSimple(
               taskId, pPage, block.GetBlockNo() );
}

/*---------------------------------------------------------------------------*/

bool
IOMan_Manager::WritePageDirectToDataVolume(
    const tsp00_TaskId          taskId,
    const IOMan_BlockAddress    &block,
    const tsp00_PageAddr        pPage )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_Manager::WritePageDirectToDataVolume", IOMan_Trace, 5 );

    SAPDBTRACE_WRITELN( IOMan_Trace, 5, "BlockAddress: " <<
                        block.GetDeviceNo() << "/" << block.GetBlockNo());

    return m_DataArea.Get( block.GetDeviceNo() ).WriteSimple(
               taskId, pPage, block.GetBlockNo() );
}

/*---------------------------------------------------------------------------*/

bool
IOMan_Manager::WritePageDirectToLogVolume(
    const tsp00_TaskId          taskId,
    const IOMan_BlockAddress    &block,
    const tsp00_PageAddr        pPage,
    const SAPDB_Bool            bToPrimary )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_Manager::WritePageDirectToLogVolume", IOMan_Trace, 5 );

    SAPDBTRACE_WRITELN( IOMan_Trace, 5, "BlockAddress: " <<
                        block.GetDeviceNo() << "/" << block.GetBlockNo());

    return m_LogArea.Get( block.GetDeviceNo() ).WriteSimple(
               taskId, pPage, block.GetBlockNo(), bToPrimary );
}

/*---------------------------------------------------------------------------*/

bool
IOMan_Manager::ReadPageDirectFromLogVolume(
    const tsp00_TaskId          taskId,
    const IOMan_BlockAddress    &block,
    const tsp00_PageAddr        pPage,
    const SAPDB_Bool            bFromPrimary )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_Manager::ReadPageDirectFromLogVolume", IOMan_Trace, 5 );

    SAPDBTRACE_WRITELN( IOMan_Trace, 5, "BlockAddress: " <<
                        block.GetDeviceNo() << "/" << block.GetBlockNo());

    return m_LogArea.Get( block.GetDeviceNo() ).ReadSimple(
               taskId, pPage, block.GetBlockNo(), bFromPrimary );
}

/*---------------------------------------------------------------------------*/

bool
IOMan_Manager::ReadDataPagesForBackup(
    RTETask_ITask&              task,
    Msg_List&                   msgList,
    const IOMan_AsynIOHandle&   asynIOHandle,
    IOMan_DataPages&            pages,
    IOMan_BlockCount&           blocksToBeBackuped,
    const bool                  bDoExtendedCheck,
    bool&                       bIsClustered )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_Manager::ReadDataPagesForBackup", IOMan_Trace, 5 );

    return m_DataArea.ReadPagesForBackup( task, msgList, pages, asynIOHandle,
                                          blocksToBeBackuped, bDoExtendedCheck, bIsClustered );
}

/*---------------------------------------------------------------------------*/

bool
IOMan_Manager::WriteDataPagesForRestore(
    RTETask_ITask&              task,
    Msg_List&                   msgList,
    const IOMan_AsynIOHandle&   asynIOHandle,
    IOMan_DataPages&            pages,
    const bool                  bDoExtendedCheck,
    const bool                  bIsClustered )
{
    SAPDBTRACE_ROUTINE_DEBUG( "IOMan_Manager::WriteDataPagesForRestore", IOMan_Trace, 5 );

    return m_DataArea.WritePagesForRestore( task, msgList, pages, asynIOHandle,
                                            bDoExtendedCheck, bIsClustered );
}

/*---------------------------------------------------------------------------*/


IOMan_ReturnCode
IOMan_Manager::MigrateDataBase( RTETask_ITask&    task )
{
    if( IOMan_Migration::IsMigrationNeeded() )
    {
        IOMan_Migration migManager( task, m_PageAllocator, m_Allocator );

        if( ! migManager.Initialize( m_DataArea.MaxVolumes())){
            return IOMan_NoMoreMemory;
        }
        m_RestartPageAddress = IOMan_BlockAddress( 1, 1 ); // fixed position

        if( ! migManager.ExecuteMigration( m_DataArea, m_LogArea ))
        {
            // only one error could occur.
            return IOMan_NotPreparedForMigration;
        }
    }
    return IOMan_Okay;
}

/*===========================================================================*
 *  PRIVATE METHODS                                                          *
 *===========================================================================*/

SAPDB_UInt
IOMan_Manager::ReadDataVolumeGroups(){
    return GetIntConfiguredValue( KERNELPARAM_DATA_VOLUME_GROUPS );
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt
IOMan_Manager::ReadMaxDataVolumeCount(){
    return GetIntConfiguredValue( KERNELPARAM_MAXDATAVOLUMES );
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt
IOMan_Manager::ReadMaxLogVolumeCount(){
    return GetIntConfiguredValue( KERNELPARAM_MAXLOGVOLUMES );
}
/*---------------------------------------------------------------------------*/

SAPDB_UInt
IOMan_Manager::ReadLogIOBlockCount(){
    return GetIntConfiguredValue( KERNELPARAM_LOG_IO_BLOCK_COUNT );
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt
IOMan_Manager::ReadDataIOBlockCount(){
    return GetIntConfiguredValue( KERNELPARAM_DATA_IO_BLOCK_COUNT );
}

/*---------------------------------------------------------------------------*/

bool
IOMan_Manager::RestartAndInitializeFBM(
    RTETask_ITask   &task,
    const bool      bInitial )
{
    const IOMan_ReservedBlockAddress reservedBlocks;

    const FBM_ReturnCode retCode = FBM_IManager::Instance().Restart( task.ID(),
                                   m_DataArea.MaxVolumes(), reservedBlocks,
                                   GetDataIOBlockCount(), ReadDataVolumeGroups());

    if( retCode == FBM_AlreadyStarted ){
        return true; // do not add data volumes again
    }
    else if( retCode != FBM_Okay ){
        return false;
    }

    // FBM_Okay => add data volumes

    for( IOMan_DeviceNo volNo = 1; volNo <= m_DataArea.MaxVolumes(); ++volNo )
    {
        if( m_DataArea.Get( volNo ).IsConfigured())
        {
            if( ! FBM_IManager::Instance().RegisterVolume( task.ID(), volNo,
                    m_DataArea.Get( volNo ).GetInternalSize(),
                    m_DataArea.Get( volNo ).GetAccessMode(),
                    m_RestartPageAddress,
                    0))
            {
                return false;
            }
        }
    }
    if( !bInitial ){
        return FBM_IManager::Instance().ReserveClusterSpace( kb57GetClusterReservedSpace());
    }
    else {
        return FBM_IManager::Instance().ReserveClusterSpace( 0 );
    }

}
/*---------------------------------------------------------------------------*/

IOMan_BlockAddress
IOMan_Manager::ReadRestartPageAddress( const tsp00_TaskId   taskId )
{
    bool                closeVolumeAfterRead = false;
    IOMan_BlockAddress  restartPageAddress;

    RTETask_ITask&       task     = *RTETask_ITask::ByTaskID( taskId );
    const IOMan_DeviceNo firstVol = m_DataArea.GetFirstConfiguredVolumeNo();

    if( ! m_DataArea.IsDataVolumeOnline( firstVol ))
    {
        OpenOneDataVolume( taskId, firstVol );
        closeVolumeAfterRead = true;
    }

    restartPageAddress = m_DataArea.Get( firstVol ).GetRestartPageAddress();

    if( closeVolumeAfterRead ){
        m_DataArea.Get( firstVol ).Close( taskId );
    }

    if( restartPageAddress.IsValid() && ( restartPageAddress == IOMan_BlockAddress( 0,0 )))
    {   // migration from volumes without RestartInfo in Infopage

        restartPageAddress = IOMan_BlockAddress( m_DataArea.GetFirstConfiguredVolumeNo(), 1 );
        m_DataArea.Get( m_DataArea.GetFirstConfiguredVolumeNo()).SetRestartPageAddress( task, restartPageAddress );
    }
    else
    {
        if (!(restartPageAddress.IsValid() && m_DataArea.IsDataVolumeNoValid( restartPageAddress.GetDeviceNo())))
        {
            IOMan_MessageList ErrMsg( __CONTEXT__, IOMAN_BAD_RESTART_PAGE_ADDRESS,
                                      KERNEL_DATA_FILE, SAPDB_ToString( restartPageAddress.GetDeviceNo(), _T_d ),
                                      SAPDB_ToString( restartPageAddress.GetBlockNo(), _T_d ));
            RTE_Crash( ErrMsg );
        }
    }
    return restartPageAddress;
}


/*---------------------------------------------------------------------------*/
/*-------------- Implementation of Interface Instance methods ---------------*/
/*---------------------------------------------------------------------------*/

IOMan_IDataInfo&
IOMan_IDataInfo::GetInstance()
{
    return IOMan_Manager::GetInstance();
}

/*---------------------------------------------------------------------------*/

IOMan_IDataIO&
IOMan_IDataIO::GetInstance()
{
    return IOMan_Manager::GetInstance();
}

/*---------------------------------------------------------------------------*/

IOMan_IDataManager&
IOMan_IDataManager::GetInstance()
{
    return IOMan_Manager::GetInstance();
}

/*---------------------------------------------------------------------------*/

IOMan_IDiagnose&
IOMan_IDiagnose::GetInstance()
{
    return IOMan_Manager::GetInstance();
}

/*---------------------------------------------------------------------------*/

IOMan_ILogInfo&
IOMan_ILogInfo::GetInstance()
{
    return IOMan_Manager::GetInstance();
}

/*---------------------------------------------------------------------------*/

IOMan_ILogIO&
IOMan_ILogIO::GetInstance()
{
    return IOMan_Manager::GetInstance();
}

/*---------------------------------------------------------------------------*/

IOMan_ILogManager&
IOMan_ILogManager::GetInstance()
{
    return IOMan_Manager::GetInstance();
}

/*---------------------------------------------------------------------------*/

IOMan_IConverterIO&
IOMan_IConverterIO::GetInstance()
{
    return IOMan_Manager::GetInstance();
}

/*---------------------------------------------------------------------------*/

IOMan_IMigration&
IOMan_IMigration::GetInstance()
{
    return IOMan_Manager::GetInstance();
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
