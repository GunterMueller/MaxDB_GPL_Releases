/*!
    @file       IOMan_DataArea.hpp
    @author     TorstenS
    @ingroup    IOManagement
    @brief      This module is used to manage the data area.

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


#ifndef IOMAN_DATACONTROL_HPP
#define IOMAN_DATACONTROL_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"

#include "Container/Container_Vector.hpp"
#include "IOManager/IOMan_BlockAddress.hpp"
#include "IOManager/IOMan_DataPages.hpp"
#include "IOManager/IOMan_DataVolume.hpp"
#include "IOManager/IOMan_PageFrameAllocator.hpp"
#include "IOManager/IOMan_Types.hpp"
#include "KernelCommon/Kernel_Common.hpp"
#include "RunTime/Synchronisation/RTESync_RWRegion.hpp"

class SAPDBMem_IRawAllocator;
class IOMan_AsynIOHandle;
class IOMan_MessageList;
class IOMan_ComplexClusterAddress;
class RTETask_ITask;
class Data_IOBuffer;
class Msg_List;
class FBM_IManager;

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
    @class  IOMan_DataArea
    @brief  This class is used to manage the data area. The data area the
            the collectio of all data volumes and used to store data pages.
 */

class IOMan_DataArea
{
public:

    /*!
       @brief   Defines abbreviation for a data volume collection
     */

    typedef Container_Vector<IOMan_DataVolume>  DataVolumes;

    /*!
       @brief   Defines abbreviation for a data volume collection iterator
     */

    typedef DataVolumes::Iterator               Iterator;

    /*!
      @brief   Defines abbreviation for a const data volume collection iterator
     */

    typedef DataVolumes::ConstIterator          ConstIterator;

    /* -----------------------------------------------------------------------*/
    /*! @name Constructors and initialization */
    /*@{*/
    /* -----------------------------------------------------------------------*/

    /*!
        @brief  Constructor
        @param  pageAllocator [in] Allocator for page frames
        @param  allocator     [in] Allocator
        @param  dataBlockIoCount [in] cluster size in pages for data io and fixed  
                                      cluster size of each volume 
        @return none
     */

    IOMan_DataArea(
        IOMan_PageFrameAllocator&    pageAllocator,
        SAPDBMem_IRawAllocator&      allocator,
        const SAPDB_UInt             dataBlockIoCount )
    :
    m_PageAllocator( pageAllocator ),
    m_Allocator( allocator ),
    m_BlockIoCount( dataBlockIoCount ),
    m_UsedVolumes( 0 ),
    m_ConfiguredVolumes( 0 ),
    m_TotalDataPages( 0 ),
    m_SpinLockPool((SAPDB_UTF8*)"IOMan_DataArea::m_LinkageSpinLock", 1),
    m_pRwLock( 0 ),
    m_Volumes( m_Allocator )
    {
        m_pRwLock = RTESync_CreateRWRegion( 1, m_SpinLockPool, m_Allocator );

        if ( 0 == m_pRwLock )
            RTE_Crash( SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                                          "Could not create system key cache resources"));
    }

    ~IOMan_DataArea()
    {
        if( m_pRwLock ){
            RTESync_DestroyRWRegion( m_pRwLock, m_Allocator );
        }
    }

    /*!
        @brief  This method is used to initialize the data control structures,
                i.e. to get the memory via the allocator.
        @param  task           [in] Identification of the calling task
        @param  maxDataVolumes [in] Maximun number of data volumes.
        @return true if the initialization is succesful; else false
     */

    bool Initialize(
        RTETask_ITask&      task,
        const SAPDB_Int     maxDataVolumes);

    /*!
        @brief  find the first configured data volume
        @return volume number of the first configured data volume
     */
    IOMan_DeviceNo GetFirstConfiguredVolumeNo() const{
        return GetConfiguredVolumeNo( 1 );
    }

    /*!
        @brief  find the n-th configured data volume
        @return volume number of the n-th configured data volume
     */
    IOMan_DeviceNo GetConfiguredVolumeNo( const SAPDB_Int pos ) const;

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! @name Volume administration */
    /*@{*/
    /* -----------------------------------------------------------------------*/

    /*!
        @brief  This method is used close all data volumes which are still
                in mode open.
        @param  task [in] Identification of the calling task
        @return none
     */

    void CloseAll( RTETask_ITask&   task );

    /*!
        @brief  This method is used to create, format and open all data 
                volumes being defined within the data base configuration file.
        @param  task [in] Identification of the calling task
        @param  pDBIdentifier [in] Unique data base identifier
        @return true means successful execution
     */

    bool CreateAll(
        RTETask_ITask&      task,
        const SAPDB_Byte*   pDBIdentifier );

    /*!
       @brief   This method is used to open all data volumes being configued
                in the data base configuration file.
       @param   task [in] Identification of the calling task
       @return  true means successful execution
     */


    bool OpenAll( RTETask_ITask&  task );

    /*!
       @brief   This method is used to add a new data volume to the data area
       @param   task             [in] Identification of the calling task.
       @param   bAddVolumeOnly   [in] Do not adjust FBM and Converter
       @param   devName          [in] Name of the new volume
       @param   configuredSize   [in] Size of the new volume in pages
       @param   devNo            [in] Logical indentifier of the volume
       @param   restartPageBlock [in] block address of restart page
       @return  true means that add data volume was successful
     */

    bool AddVolume(
        RTETask_ITask&              task,
        const bool            bAddVolumeOnly,
        tsp00_VFilename&            devName,
        const IOMan_BlockCount      configuredSize,
        const IOMan_DeviceNo        devNo,
        const IOMan_BlockAddress&   restartPageBlock );

    bool DropVolume(
        RTETask_ITask&          task,
        const IOMan_DeviceNo    devNo );

    /*!
       @brief   This method is used to adjust the capacity of the data 
                volumes stored in the IOManInfoPage's.
       @param   task        [in] Identification of the calling task.
       @param   msgList [in|out] messageList
       @return  (IOMan_ReturnCode) 
     */
    IOMan_ReturnCode MigrateCapacity(
        RTETask_ITask&      task,
        IOMan_MessageList&  msgList );

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! @name Volume access                                                   */
    /*@{*/
    /* -----------------------------------------------------------------------*/

    /*!
        @brief  This method returns a volume handler for the given logical
                identifier. There is not check whether the given logical
                is valid.
        @param  devNo [in] Logical volume identifier
        @return (IOMan_DataVolume)
     */

    IOMan_DataVolume& Get( const IOMan_DeviceNo devNo ){
        return( m_Volumes[ devNo - IOMan_DataArea::GetFirstDataVolumeNo() ] );
    }

    const IOMan_DataVolume& Get( const IOMan_DeviceNo devNo ) const{
        return( m_Volumes[ devNo - IOMan_DataArea::GetFirstDataVolumeNo() ] );
    }

    /*!
       @brief  This method is used to read data pages for backup. See detailed 
               description in IOMan_IDataIO::ReadDataPagesForBackup
       @param   task                [in] task 
       @param   msgList            [out] for error tracking purpose
       @param   asynIOHandle        [in] Handle for the asynchronous io
       @param   pages               [in] Handler of the pages to be read
       @param   blocksToBeBackuped [out] number of pages to be written into backup medium
       @param   bDoExtendedCheck    [in] do extended consistency check on read pages
       @param   bIsClustered       [out] pages belongs to the clustered area
       @return (bool) true means read was successfull
    */
    bool ReadPagesForBackup(
        RTETask_ITask&              task,
        Msg_List&                   msgList,
        IOMan_DataPages&            pages,
        const IOMan_AsynIOHandle&   asynIOHandle,
        IOMan_BlockCount&           blocksToBeBackuped,
        const bool                  bDoExtendedCheck,
        bool&                       bIsClustered );

    /*!
    @brief  This method is used to write data pages during recovery. See detailed 
           description in IOMan_IDataIO::WriteDataPagesForRestore
    @param   task                [in] task
    @param   msgList            [out] for error tracking purpose
    @param   asynIOHandle        [in] Handle for the asynchronous io
    @param   pages               [in] Handler of the pages to be written
    @param   bDoExtendedCheck    [in] do extended consistency check on given pages
    @param   bIsClustered        [in] pages belongs to the clustered area
    @return (bool) true means write was successfull
    */
    bool WritePagesForRestore(
        RTETask_ITask&              task,
        Msg_List&                   msgList,
        IOMan_DataPages&            pages,
        const IOMan_AsynIOHandle&   asynIOHandle,
        const bool                  bDoExtendedCheck,
        const bool                  bIsClustered );

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/
    /* -----------------------------------------------------------------------*/
    /*! @name Iterator access */
    /*@{*/
    /* -----------------------------------------------------------------------*/

    /*!
       @brief   This method is used to set an iterator to the first data
                volume. There is no check whether the first volume is valid.
       @return  Iterator/ConstIterator
     */

    Iterator Begin(){
        return( m_Volumes.Begin() );
    }

    ConstIterator Begin() const{
        return( m_Volumes.Begin() );
    }

    /*!
       @brief   This method is used to get an iterator pointing behind the 
                last valid data volume. Crazy isnt't it?
       @return  Iterator/ConstIterator
     */

    Iterator End(){
        return( m_Volumes.End() );
    }

    ConstIterator End() const{
        return( m_Volumes.End() );
    }

    /*!
       @brief   This method returns the identifier of the first data volume.
                It's not guaranteed that the returned volume identifier 
                specifies a volume which is online.
       @return  IOMan_DeviceNo
     */

    static IOMan_DeviceNo GetFirstDataVolumeNo(){
        return( 1 ); // Volume counting starts with one
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
        return( devNo + 1 );
    }

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! @name Configuration and statistic information */
    /*@{*/
    /* -----------------------------------------------------------------------*/

    /*!
       @brief   This method returns the number of data volumes configured 
                within the data base configuration file, i.e the number of 
                data volumes corresponding to a entry within the configuration 
                file. This number is also available if not all data volumes 
                are in mode open.
       @return  (SAPDB_Int)
     */

    SAPDB_Int ConfiguredVolumes() const{
        return( m_ConfiguredVolumes );
    }

    /*!
       @brief   This method is used to determine the logical identifier for
                a given data volume name. It is not needed that the data 
                volumes are in mode open.
       @param   devName [in] Name of the searched volume
       @param   devNo [out] Logical identifier of the volume
       @return  true means volume was found; else false
     */


    bool GetVolumeNo(
        tsp00_VFilename&    devName,
        IOMan_DeviceNo&     devNo ) const;

    /*!
       @brief   This method returns the maximun number of configurable data 
                volumes. This value is stored within the data base configuration
                file.
       @return  (SAPDB_Int) Maximun number of configurable data volumes.
     */

    SAPDB_Int MaxVolumes() const{
        return( m_Volumes.GetCapacity() );
    }

    /*!
       @brief   This method returns the sum of all pages storable within all 
                configured data volumes. This value doesn't include the pages
                needed for administrativ purpose, i.e IOMan_InfoPage.
                This value is only available if all
                data volumes are in mode open.
       @return  IOMan_BlockCount
     */

    IOMan_BlockCount  TotalUsableDataPages() const{
        return( m_TotalDataPages );
    }

    /*!
       @brief   This method returns the sum of all pages storable within all 
                configured data volumes. This value includes all pages of
                all data volumes without taking account of pages reserved 
                for administrative purposes. This value is only available if 
                all data volumes are in mode open.
       @return  SAPDB_Int4
     */

    SAPDB_Int4 TotalDataPages() const{
        return( TotalUsableDataPages() + ( UsedVolumes() * 2)); // 2 => RTE + IOManInfoPage
    }

    /*!
       @brief   This method returns the number of data volumes which are 
                currently in mode open. Usualy all configured data volumes 
                are still in mode open.
       @return  (SAPDB_Int)
     */


    SAPDB_Int UsedVolumes() const{
        return( m_UsedVolumes );
    }

    /*!
       @brief   This method checks the validity of the given data volume id.
       @param   devNo [in] Logical volume identifier
       @return  bool
     */

    bool IsDataVolumeNoValid( const IOMan_DeviceNo devNo ) const
    {
        if(( IOMan_DataArea::GetFirstDataVolumeNo() > devNo ) || ( devNo > MaxVolumes()))
            return( false );
        else
            return( true );
    }

    /*!
       @brief   This method returns whether the given data volume is in
                state online.
       @param   devNo [in] Logical volume identifier
       @return  bool
     */

    bool IsDataVolumeOnline( const IOMan_DeviceNo devNo ) const
    {
        if( ! IsDataVolumeNoValid( devNo ) )
            return( false );

        return( Get( devNo ).IsOnline() );
    }

    /*!
       @brief   This method returns whether the data area is fully restarted
       @return  bool
     */

    bool IsRestarted() const{
        return (m_UsedVolumes == m_ConfiguredVolumes);
    }
    /*!
        @brief Returns cluster size of data area in pages used for io
        @return (SAPDB_UInt) value in pages
    */
    SAPDB_UInt GetIOBlockCount() const{
        return m_BlockIoCount;
    }

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/

private:

    // read a couple of pages for data backup inclusive io check and
    // remove them from the converter resp. FBM backup structure
    bool AsynReadPages(
        RTETask_ITask&                      task,
        Msg_List&                           msgList,
        IOMan_DataPages&                    pages,
        const SAPDB_UInt                    startIndex,
        const IOMan_AsynIOHandle&           asynIOHandle,
        const IOMan_ComplexClusterAddress&  cluster,
        const bool                          bDoExtendedCheck );

    bool AsynWritePages(
        RTETask_ITask&              task,
        Msg_List&                   msgList,
        FBM_IManager&               fbm,
        IOMan_DataPages&            pages,
        const SAPDB_UInt            startIndex,
        const IOMan_AsynIOHandle&   asynIOHandle,
        const IOMan_ClusterAddress& cluster,
        const bool                  bIsClustered,
        const bool                  bDoExtendedCheck );

    IOMan_BlockCount DetermineUsageCount(
        const IOMan_DataPages&  pages );

    bool RemoveFromBackUpList(
        RTETask_ITask&                      task,
        Msg_List&                           msgList,
        IOMan_DataPages&                    pages,
        const SAPDB_UInt                    startIndex,
        const IOMan_ComplexClusterAddress&  cluster,
        SAPDB_UInt&                         badBlockIndex );

    void RemoveUnusedPages(
        IOMan_DataPages&                    pages,
        const SAPDB_UInt                    startIndex,
        const IOMan_ComplexClusterAddress&  cluster );

    // format all configured data volumes
    bool FormatAll(
        RTETask_ITask&      task,
        IOMan_MessageList&  msgList );

    void CreateLinkage(
        RTETask_ITask&      task,
        IOMan_DataVolume&   prevVolume,
        IOMan_DataVolume&   volume );

    void DropLinkage(
        RTETask_ITask&      task,
        IOMan_DataVolume&   volume );

    // This method is used to the access to the predecessor and
    // successor of the given volume. If a no successor or predecessor
    // is available NULL ist returned; else a pointer to the dbIdentifier
    // is returned.
    const SAPDB_Byte *DetermineLinkage(
        const IOMan_DataVolume& volume,
        IOMan_DataVolume**      pPrevVolume,
        IOMan_DataVolume**      pNextVolume );

private:

    /// Copy and assignment operator are not supported

    /*!
       @brief   The assignment operator is not supported to avoid not allowed
                duplicates.
       @param   dataControl [in] dataControl 
       @return  dataControl&
     */

    IOMan_DataArea& operator=( const IOMan_DataArea&    dataControl );

    /*!
       @brief   The copy operator is not supported to avoid not allowed
                duplicates.
       @param   dataControl [in] dataControl 
       @return  dataControl&
     */

    IOMan_DataArea( const IOMan_DataArea&   dataControl );

private:


    // Allocator for page frames. The page frame size is defined by
    // the data base configuration file
    IOMan_PageFrameAllocator&   m_PageAllocator;


    // Named allocator for non page allocation
    SAPDBMem_IRawAllocator& m_Allocator;

    // Defines the blocking size of the data area which is used for io
    const SAPDB_UInt m_BlockIoCount;

    // The number of data volumes which are currently in mode
    // open. Usualy all configured data volumes are still in mode open
    SAPDB_Int   m_UsedVolumes;

    // The number of data volumes configured within the data base configuration
    // file, i.e the number of data volumes corresponding to a entry within the
    // configuration file. This number is also available if not all data volumes
    // are in mode open
    SAPDB_Int4  m_ConfiguredVolumes;

    // The sum of all pages storable within all configured data volumes. This
    // value is only available if all data volumes are in mode open
    IOMan_BlockCount  m_TotalDataPages;

    // Collection of data volumes.
    DataVolumes   m_Volumes;

    // spinLockPool for rwLocks
    RTESync_SpinlockPool    m_SpinLockPool;

    // reader/writer lock
    RTESync_IRWRegion*  m_pRwLock;

    class LockedScope
    {
    public:
        LockedScope( RTESync_IRWRegion& rwRegion ): m_RWRegion( rwRegion )
        { m_RWRegion.enter( true );}

        ~LockedScope(){ m_RWRegion.leave( true );}

    private :
        RTESync_IRWRegion& m_RWRegion;
    };

};

#endif  /* IOMAN_DATACONTROL_HPP */
