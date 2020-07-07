/*!
  @file     IOMan_Manager.hpp
  @author   TorstenS
  @ingroup  IOManagement
  @brief    This module is used to handle all volumes of the kernel.
            The IOMan_Manager class offers the possibility to manage 
            data and log volumes. This contains the facility to create,
            open, close, read and write from and to volumes.

\if EMIT_LICENCE
\endif
\if EMIT_LICENCE


    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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



#ifndef IOMAN_MANAGER_HPP
#define IOMAN_MANAGER_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"   // PASCAL: SP_basic_constants_and_types

// Interfaces
#include "IOManager/IOMan_IConverterIO.hpp"
#include "IOManager/IOMan_IDataInfo.hpp"
#include "IOManager/IOMan_IDataIO.hpp"
#include "IOManager/IOMan_IDataManager.hpp"
#include "IOManager/IOMan_IDiagnose.hpp"
#include "IOManager/IOMan_ILogInfo.hpp"
#include "IOManager/IOMan_ILogManager.hpp"
#include "IOManager/IOMan_ILogIO.hpp"
#include "IOManager/IOMan_IMigration.hpp"

#include "Converter/Converter_Page.hpp"
#include "DataAccess/Data_BasePage.hpp"
#include "DataAccess/Data_Types.hpp"
#include "IOManager/IOMan_BlockAddress.hpp"
#include "IOManager/IOMan_ClusterAddress.hpp"
#include "IOManager/IOMan_DataArea.hpp"
#include "IOManager/IOMan_ConverterPages.hpp"
#include "IOManager/IOMan_DataPages.hpp"
#include "IOManager/IOMan_LogArea.hpp"
#include "IOManager/IOMan_LogPages.hpp"
#include "IOManager/IOMan_PageFrameAllocator.hpp"
#include "IOManager/IOMan_ReservedBlockAddress.hpp"
#include "IOManager/IOMan_Types.hpp"
#include "KernelCommon/Kernel_Common.hpp"
#include "KernelCommon/Kernel_RestartPage.hpp"
#include "Logging/Log_ClusterAddress.hpp"
#include "Logging/Log_InfoPage.hpp"
#include "Logging/Log_Page.hpp"
#include "RunTime/MemoryManagement/RTEMem_AllocatorWrapper.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"

class SAPDBErr_MessageList;
class IOMan_ComplexClusterAddress;

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
   @class   IOMan_Manager
   @brief   This class is used to handle all volumes of the kernel.
 */

class IOMan_Manager : public IOMan_IConverterIO,
            public IOMan_IDataInfo,
            public IOMan_IDataIO,
            public IOMan_IDataManager,
            public IOMan_IDiagnose,
            public IOMan_ILogInfo,
            public IOMan_ILogIO,
            public IOMan_ILogManager,
            public IOMan_IMigration
{
    friend class IOMan_IConverterIO;
    friend class IOMan_IDataInfo;
    friend class IOMan_IDataIO;
    friend class IOMan_IDataManager;
    friend class IOMan_IDiagnose;
    friend class IOMan_ILogInfo;
    friend class IOMan_ILogIO;
    friend class IOMan_ILogManager;
    friend class IOMan_IMigration;

public:

    /*!
       @brief   This method returns the access to the component IOManager.
                It is assumed that the IOManager singleton is available! 
                No CreateInstance call will be done.
       @return  Access to the IOMan_Manager singleton
     */

    static IOMan_Manager& GetInstance(){ return *m_Instance; }

    /*!
       @brief   This method creates the singleton IOMananger.It is assumed 
                that this method will be callled only one time by the timeout 
                task and therefore no synchonization mechanism is used.
       @param   task [in] Identification of the calling task.
       @return  true means instance could be created; else false
     */

    static bool CreateInstance( RTETask_ITask &task );

    SAPDB_Bool AddDataVolume(
        const tsp00_TaskId      taskId,
        tsp00_VFilename         &devName,
        const IOMan_BlockCount  devSize,
        const IOMan_DeviceNo    devNo );

    SAPDB_Bool AddLogVolume(
        const tsp00_TaskId          taskId,
        tsp00_VFilename             &devName,
        tsp00_VFilename             &mirrDevName,
        const IOMan_BlockCount      devSize,
        const IOMan_DeviceNo        devNo,
        IOMan_LogPages              &pages,
        IOMan_BlockCount            &newLogSize );

    IOMan_ReturnCode DropDataVolume(
        Trans_Context           &trans,
        Msg_List                &msgList,
        const IOMan_DeviceNo    volNo );

    IOMan_ReturnCode RemoveDroppedDataVolume(
        RTETask_ITask           &task,
        Msg_List                &msgList,
        const IOMan_DeviceNo    volNo );

    /*!
        \brief check if data volume is to drop
        \returns true if data volume is to be dropped
    */
    bool IsDataVolumeToDrop(
        const IOMan_DeviceNo    volNo ) const;

    bool PrepareVolumeForDrop(
        const RTETask_ITask     &task,
        Msg_List                &msgList);

    bool AbortDropDataVolume(
        RTETask_ITask           &task,
        Msg_List                &msgList,
        const IOMan_ReturnCode  reason);

    /*!
       @brief          read all pages on one volume into the data cache
       @param          task    [in]       task 
       @param          msgList [in|out] message list
       @param          volno   [in] volume number to be dropped
       @return         bool ( true, if successful; else false)
     */
    bool SrvTaskDropVolume(
        RTETask_ITask       &task,
        Msg_List            &msgList,
        Converter_PageNoIterator &pageIter);

    SAPDB_Bool AddDataVolumePossible() const{
        return m_DataArea.ConfiguredVolumes() <  m_DataArea.MaxVolumes();
    }

    const IOMan_BlockAddress& GetRestartPageAddress( const tsp00_TaskId taskId );

    bool IsRestartPageOnDropVolume() const;

    SAPDB_Bool MoveRestartPage( const tsp00_TaskId  taskId );

    void CloseAllLogVolumes( const tsp00_TaskId  taskId );

    void CloseAllDataVolumes( const tsp00_TaskId  taskId );

    SAPDB_Int ConfiguredDataVolumes() const{
        return m_DataArea.ConfiguredVolumes();
    }

    SAPDB_Int MaxConfigurableDataVolumes() const{
        return m_DataArea.MaxVolumes();
    }

    SAPDB_Bool IsConfigured(IOMan_DeviceNo volNo) const{
        return m_DataArea.Get( volNo ).IsConfigured();
    }

    SAPDB_Int ConfiguredLogVolumes() const{
        return m_LogArea.ConfiguredVolumes();
    }

    SAPDB_Int MaxConfigurableLogVolumes() const{
        return m_LogArea.MaxConfigurableVolumes();
    }

    SAPDB_Bool IsLogMirrored() const{
        return m_LogArea.IsLogMirrored();
    }

    bool GetDataVolumeNo(
        tsp00_VFilename   &devName,
        IOMan_DeviceNo    &devNo ) const{
        return m_DataArea.GetVolumeNo( devName, devNo );
    }

    bool GetDataVolumeName(
        IOMan_DeviceNo    devNo,
        tsp00_VFilename   &devName ) const{
        return m_DataArea.Get( devNo ).GetName( devName );
    }

    SAPDB_Bool GetBadLogVolume(
        const tsp00_TaskId  taskId,
        tsp00_VFilename     &badDevName ){
        return m_LogArea.GetBadLogVolume( taskId, badDevName );
    }

    bool GetLogVolumeNo(
        tsp00_VFilename     &devName,
        IOMan_DeviceNo      &devNo,
        SAPDB_Bool          &bIsPrimary ){
        return m_LogArea.GetVolumeNo( devName, devNo, bIsPrimary );
    }

    bool GetLogVolumeName(
        const IOMan_DeviceNo        devNo,
        tsp00_VFilename             &devName ) const
    {
        return m_LogArea.Get( devNo ).GetName( devName );
    }

    bool GetMirrLogVolumeName(
        const IOMan_DeviceNo        devNo,
        tsp00_VFilename             &devName ) const
    {
        return( m_LogArea.Get( devNo ).GetName( devName, true ));
    }

    bool CreateAllLogVolumes( const tsp00_TaskId  taskId );

    bool CreateAllDataVolumes( const tsp00_TaskId  taskId );

    SAPDB_Bool MigrateVolumes(
        RTETask_ITask&          task,
        SAPDBErr_MessageList&   msgList );

    IOMan_BlockCount GetDataVolumeSize( const IOMan_DeviceNo volNo ) const{
        return m_DataArea.Get( volNo ).GetConfiguredSize();
    }

    IOMan_BlockCount GetDataVolumeUsableSize( const IOMan_DeviceNo volNo ) const{
        return m_DataArea.Get( volNo ).GetUsableSize();
    }

    IOMan_BlockCount GetLogVolumeSize( const IOMan_DeviceNo volNo ) const{
        return m_LogArea.Get( volNo ).GetConfiguredSize();
    }

    IOMan_BlockCount GetLogVolumeUsableSize( const IOMan_DeviceNo devNo ) const;

    bool OpenAllLogVolumes(
        const tsp00_TaskId  taskId,
        const SAPDB_Bool    bOpenForWrite = SAPDB_TRUE );

    bool OpenAllDataVolumes( const tsp00_TaskId   taskId );

    bool OpenOneDataVolume(
        const tsp00_TaskId    taskId,
        const IOMan_DeviceNo  devNo );

    bool OpenOneLogVolume(
        const tsp00_TaskId    taskId,
        const IOMan_DeviceNo  devNo );

    void ReadConverterPage(
        const tsp00_TaskId          taskId,
        const IOMan_BlockAddress    &block,
        Converter_Page              &page );

    IOMan_ReturnCode ReadDataPage(
        const tsp00_TaskId  taskId,
        Data_BasePage       &page,
        const Data_PageNo   &pageNo );

    IOMan_ReturnCode ReadDataPages(
        RTETask_ITask&                      task,
        const Data_PageNoList&              pageNoList,
        IOMan_ClusteredDataPages&           pages,
        const IOMan_ComplexClusterAddress&  cluster,
        const SAPDB_UInt                    startIndex,
        Data_PageNo&                        badDataPageNo );

    bool ReadDataPagesForBackup(
        RTETask_ITask&              task,
        Msg_List&                   msgList,
        const IOMan_AsynIOHandle&   asynIOHandle,
        IOMan_DataPages&            pages,
        IOMan_BlockCount&           blocksToBeBackuped,
        bool                        bDoExtendedCheck,
        bool&                       bIsClustered );

    bool ReadDataPageDirect(
        const tsp00_TaskId          taskId,
        Data_BasePage               &page,
        const IOMan_BlockAddress    &block );

    void ReadLogInfoPage(
        const tsp00_TaskId        taskId,
        Log_InfoPage              &page,
        const Log_ClusterAddress  &address );

    void ReadLogPage(
        const tsp00_TaskId        taskId,
        Log_Page                  &page,
        const Log_ClusterAddress  &address );

    void ReadLogPageWithoutCheck(
        const tsp00_TaskId        taskId,
        Log_Page                  &page,
        const Log_ClusterAddress  &address );

    void ReadLogPages(
        const tsp00_TaskId          taskId,
        IOMan_LogPages              &pages,
        const Log_ClusterAddress    &address );


    void ReadRestartPage(
        const tsp00_TaskId  taskId,
        Kernel_RestartPage  &page );

    void ReadSnapShotRestartPage(
        const tsp00_TaskId          taskId,
        Kernel_RestartPage          &page,
        const IOMan_BlockAddress    &block );

    SAPDB_Int4 TotalUsableDataPages() const{
        return m_DataArea.TotalUsableDataPages();
    }

    SAPDB_Int4 TotalDataPages() const{
        return m_DataArea.TotalDataPages();
    }

    SAPDB_Int4  TotalUsableLogPages() const{
        return m_LogArea.TotalUsableLogPages();
    }

    SAPDB_Int UsedDataVolumes() const{
        return m_DataArea.UsedVolumes();
    }

    void WriteConverterPage(
        const tsp00_TaskId  taskId,
        Converter_Page      &page );

    void WriteDataPage(
        const tsp00_TaskId  taskId,
        Data_BasePage&      page );

    void WriteDataPages(
        const tsp00_TaskId          taskId,
        IOMan_DataPages&            pages,
        const IOMan_ClusterAddress& cluster,
        const SAPDB_UInt            startIndex = 0 );

    bool WriteDataPagesForRestore(
        RTETask_ITask&              task,
        Msg_List&                   msgList,
        const IOMan_AsynIOHandle&   asynIOHandle,
        IOMan_DataPages&            pages,
        const bool                  bDoExtendedCheck,
        const bool                  bIsClustered );

    void WriteLogInfoPage(
        const tsp00_TaskId        taskId,
        Log_InfoPage              &page,
        const Log_ClusterAddress  &address );

    void WriteLogPage(
        const tsp00_TaskId        taskId,
        Log_Page                  &page,
        const Log_ClusterAddress  &address );

    void WriteLogPages(
        const tsp00_TaskId        taskId,
        IOMan_LogPages            &pages,
        const Log_ClusterAddress  &address );

    void WriteRestartPage(
        const tsp00_TaskId    taskId,
        Kernel_RestartPage    &page );

    void WriteSnapShotRestartPage(
        const tsp00_TaskId  taskId,
        Kernel_RestartPage  &page,
        IOMan_BlockAddress  &block );

    bool ReadPageDirectFromDataVolume(
        const tsp00_TaskId          taskId,
        const IOMan_BlockAddress    &block,
        const tsp00_PageAddr        pPage );

    bool WritePageDirectToDataVolume(
        const tsp00_TaskId          taskId,
        const IOMan_BlockAddress    &block,
        const tsp00_PageAddr        pPage );

    bool ReadPageDirectFromLogVolume(
        const tsp00_TaskId          taskId,
        const IOMan_BlockAddress    &block,
        const tsp00_PageAddr        pPage,
        const SAPDB_Bool            bFromPrimary );

    bool WritePageDirectToLogVolume(
        const tsp00_TaskId          taskId,
        const IOMan_BlockAddress    &block,
        const tsp00_PageAddr        pPage,
        const SAPDB_Bool            bToPrimary );

    IOMan_ReturnCode MigrateDataBase( RTETask_ITask&    task );


    /*!
       @brief   This method returns the identifier of the first data volume.
                It's not guaranteed that the returned volume identifier 
                specifies a volume which is online.
       @return  IOMan_DeviceNo
     */

    IOMan_DeviceNo GetFirstDataVolumeNo() const{
        return m_DataArea.GetFirstDataVolumeNo();
    }

    /*!
       @brief   This method returns the volume number of the first configured data volume.
       @return  IOMan_DeviceNo
     */
    IOMan_DeviceNo GetFirstConfiguredDataVolumeNo() const{
        return m_DataArea.GetFirstConfiguredVolumeNo();
    }

    /*!
       @brief   This method returns the volume number of the n-th configured data volume.
       @param   pos [in] 
       @return  IOMan_DeviceNo
     */
    IOMan_DeviceNo GetConfiguredDataVolumeNo( const SAPDB_Int pos ) const{
        return m_DataArea.GetConfiguredVolumeNo( pos );
    }


    /*!
       @brief   This method returns the next identifier of the data volume
                following the given data volume. It's not guaranteed that
                the returned volume identifier specifies a volume which
                is online.
       @param   devNo [in] Logical volume identifier
       @return  IOMan_DeviceNo
     */

    IOMan_DeviceNo GetNextDataVolumeNo( const IOMan_DeviceNo devNo ) const{
        return m_DataArea.GetNextDataVolumeNo( devNo );
    }

    /*!
       @brief   This method checks the validity of the given data volume 
                identifier.
       @param   devNo [in] Logical volume identifier
       @return  SAPDB_Bool
     */

    SAPDB_Bool IsDataVolumeNoValid( const IOMan_DeviceNo devNo ) const{
        return m_DataArea.IsDataVolumeNoValid( devNo );
    }

    /*!
       @brief   This method returns whether the given data volume is in
                state online.
       @param   devNo [in] Logical volume identifier
       @return  SAPDB_Bool
     */

    SAPDB_Bool IsDataVolumeOnline( const IOMan_DeviceNo devNo ) const{
        return m_DataArea.IsDataVolumeOnline( devNo );
    }

    /*!
       @brief   This method returns the identifier of the first log volume.
                It's not guaranteed that the returned volume identifier 
                specifies a volume which is online.
       @return  IOMan_DeviceNo
     */

    IOMan_DeviceNo GetFirstLogVolumeNo() const{
        return m_LogArea.GetFirstLogVolumeNo();
    }

    /*!
       @brief   This method returns the next identifier of the log volume
                following the given data volume. It's not guaranteed that
                the returned volume identifier specifies a volume which
                is online.
       @param   devNo [in] Logical volume identifier
       @return  IOMan_DeviceNo
     */

    IOMan_DeviceNo GetNextLogVolumeNo( const IOMan_DeviceNo devNo ) const    {
        return m_LogArea.GetNextLogVolumeNo( devNo );
    }

    /*!
       @brief   This method checks the validity of the given log volume 
                identifier.
       @param   devNo [in] Logical volume identifier
       @return  SAPDB_Bool
     */

    SAPDB_Bool IsLogVolumeNoValid( const IOMan_DeviceNo devNo ) const{
        return m_LogArea.IsLogVolumeNoValid( devNo );
    }

    /*!
       @brief   This method returns whether the given log volume is in
                state online.
       @param   devNo [in] Logical volume identifier
       @return  SAPDB_Bool
     */

    SAPDB_Bool IsLogVolumeOnline( const IOMan_DeviceNo devNo ) const{
        return m_LogArea.IsLogVolumeOnline( devNo );
    }

    SAPDB_Bool ReIntegrateBadLogVolume(
        const tsp00_TaskId  taskId,
        tsp00_VFilename     &badDevName ){
        return m_LogArea.ReIntegrateBadLogVolume( taskId, badDevName );
    }

    SAPDB_UInt GetDataIOBlockCount()const {
        SAPDBERR_ASSERT_STATE( m_DataArea.GetIOBlockCount() != 0)
        return m_DataArea.GetIOBlockCount();
    }
    // read configuration value DATA_IO_BLOCK_COUNT from RTE which might be slow
    static SAPDB_UInt ReadDataIOBlockCount();

    SAPDB_UInt GetLogIOBlockCount()const{
        SAPDBERR_ASSERT_STATE( m_LogArea.GetIOBlockCount() != 0)
        return m_LogArea.GetIOBlockCount();
    }
    // read configuration value LOG_IO_BLOCK_COUNT from RTE which might be slow
    static SAPDB_UInt ReadLogIOBlockCount();

    /// read configuration value DATA_VOLUME_GROUPS from RTE which might be slow
    static SAPDB_UInt ReadDataVolumeGroups();

private:

    // Copy and assignment operator are not supported

    IOMan_Manager& operator=( const IOMan_Manager &Manager );

    IOMan_Manager( const IOMan_Manager &Manager );

    IOMan_Manager( SAPDBMem_IRawAllocator  &allocator );

    bool Initialize(
        RTETask_ITask&      task,
        const SAPDB_UInt    maxDataVolumes,
        const SAPDB_UInt    maxLogVolumes);

    bool RestartAndInitializeFBM(
        RTETask_ITask       &task,
        const bool initial = false );

    IOMan_BlockAddress ReadRestartPageAddress( const tsp00_TaskId   taskId );

    // read configuration value MAXDATAVOLUMES from RTE which might be slow
    static SAPDB_UInt ReadMaxDataVolumeCount();

    // read configuration value MAXLOGVOLUMES from RTE which might be slow
    static SAPDB_UInt ReadMaxLogVolumeCount();

private:

    /*-------------------------------------------------------------------------
    declaration: m_Instance
    description: Pointer to the IOMananger singleton.
    --------------------------------------------------------------------------*/

    static IOMan_Manager    *m_Instance;

    /*-------------------------------------------------------------------------
    declaration: m_Allocator
    description: Base allocator used for all dependent memory allocation of
                 this class. Note that this member data has to be the first 
                 member data defined in this class, because the sequence
                 of the execution of the initialization lists depends on it!
    --------------------------------------------------------------------------*/

    RTEMem_AllocatorWrapper  m_Allocator;

    /*-------------------------------------------------------------------------
    declaration: m_PageAllocator
    description: Allocator for page frames. The page frame size is definded by
                 the data base configuration file. Note that this member data 
                 has to be the second member data defined in this class, because 
                 the sequence of the execution of the initialization lists 
                 depends on it!
    --------------------------------------------------------------------------*/

    IOMan_PageFrameAllocator    m_PageAllocator;

    /*-------------------------------------------------------------------------
    declaration: m_DataArea
    description: Manager istance for all data volumes.
    --------------------------------------------------------------------------*/

    IOMan_DataArea   m_DataArea;

    /*-------------------------------------------------------------------------
    declaration: m_LogArea
    description: Manager instance for all log volumes
    --------------------------------------------------------------------------*/

    IOMan_LogArea m_LogArea;

    /*-------------------------------------------------------------------------
    declaration: m_RestartPageAddress
    description: Address of the restart page
    --------------------------------------------------------------------------*/

    IOMan_BlockAddress  m_RestartPageAddress;

    /*-------------------------------------------------------------------------
    declaration: m_IsArchiveInstance
    description: Is true if the instance type of the data base is configured
                 to ARCHIVE. (SAP BW with LEGATO storage)
    --------------------------------------------------------------------------*/

    const SAPDB_Bool m_IsArchiveInstance;

    /*-------------------------------------------------------------------------
    declaration: m_AbortDropVolume
    description: reason is set, when database reaches DBFull or task is canceled
    --------------------------------------------------------------------------*/
    IOMan_ReturnCode m_AbortDropVolume;

    /*-------------------------------------------------------------------------
    declaration: m_DropVolumeNo
    description: number of the volume to drop
    --------------------------------------------------------------------------*/
    IOMan_DeviceNo m_DropVolumeNo;

};

#endif  /* IOMAN_MANAGER_HPP */
