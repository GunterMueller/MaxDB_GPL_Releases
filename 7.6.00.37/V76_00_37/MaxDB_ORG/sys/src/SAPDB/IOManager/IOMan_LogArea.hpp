/*!
  @file           IOMan_LogArea.hpp
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



#ifndef IOMAN_LOG_CONTROL_HPP
#define IOMAN_LOG_CONTROL_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "Container/Container_Vector.hpp"
#include "IOManager/IOMan_BlockAddress.hpp"
#include "IOManager/IOMan_ClusterAddress.hpp"
#include "IOManager/IOMan_LogDevice.hpp"
#include "IOManager/IOMan_LogPages.hpp"
#include "IOManager/IOMan_LogDevices.hpp"
#include "IOManager/IOMan_PageFrameAllocator.hpp"
#include "IOManager/IOMan_Types.hpp"
#include "KernelCommon/Kernel_Common.hpp"
#include "Logging/Log_Types.hpp"

class IOMan_MessageList;
class SAPDBMem_IRawAllocator;

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
   @class   IOMan_LogArea
   @brief   This class is used to manage the log area.
 */


class IOMan_LogArea
{
public:

    /* -----------------------------------------------------------------------*/
    /*! @name Constructors and initialization */
     /*@{*/
    /* -----------------------------------------------------------------------*/

    /*!
       @brief   Default constructor. 
                Creates a manager for all log devices.
       @param   pageAllocator [in] Allocator for page frames
       @param   allocator [in] Allocator
       @param   bIsLogMirrored [in] Is the log mode dual configured
       @param   logBlockIoCount [in] cluster size in pages for log io
       @return  none
     */

    IOMan_LogArea(
        IOMan_PageFrameAllocator    &pageAllocator,
        SAPDBMem_IRawAllocator      &allocator,
        const SAPDB_Bool            bIsLogMirrored,
        const SAPDB_UInt            logBlockIoCount )
    :
    m_PageAllocator( pageAllocator ),
    m_Allocator( allocator ),
    m_BlockIoCount( logBlockIoCount ),
    m_bIsLogMirrored( bIsLogMirrored ),
    m_TotalLogPages( 0 ),
    m_ConfiguredDevices( 0 ),
    m_LastAccessedDevice( 0 ),
    m_UsedDevices( 0 ),
    m_LogDevices( m_Allocator, bIsLogMirrored )
    {}

    /*!
       @brief   This method is used to initialize all log devices.
       @param   taskId        [in] Identification of the calling task
       @param   maxLogDevices [in] Maximum number of addressable log devices.
       @return  in case of an error false is returned else true
     */

    bool Initialize(
        const tsp00_TaskId  taskId,
        const SAPDB_Int     maxLogDevices);

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! @name Log device access */
     /*@{*/
    /* -----------------------------------------------------------------------*/

     /*!
       @brief   This method is used to get access to a log device which is 
                identified by the given device number.
       @param   devNo [in] logical identifier of a log device
       @return  IOMan_ILogDevice
     */

    IOMan_ILogDevice& Get( const IOMan_DeviceNo devNo ){
        return( m_LogDevices.Get( devNo ));
    }

    const IOMan_ILogDevice & Get( const IOMan_DeviceNo devNo ) const{
        return( m_LogDevices.Get( devNo ));
    }

    /*!
       @brief   This method is used to transform the logical address given by 
                the component logging into a physical block address which consider
                the partition of the log in separate log devices.
       @param   offset [in] Logical offset within the log
       @return  IOMan_BlockAddress
     */

    IOMan_BlockAddress GetBlockAddress( const Log_NonCyclicRawDeviceOffset offset );

    /*!
       @brief   This method is used to transform the logical address given by 
                the component logging into a physical block address which consider
                the partition of the log in separate log devices.
                The returned cluster address contains the correspondimg block
                address and furthermore the block count which is possible reduced
                in comparison to the given BlockCount. In this case a second 
                request is needed to handle the rest of the proper log address.
       @param   offset [in] Logical offset within the log
       @param   blockCount [in] Number of blocks to read or write from log
       @return  IOMan_ClusterAddress
     */

    IOMan_ClusterAddress GetClusterAddress(
        const Log_NonCyclicRawDeviceOffset   offset,
              IOMan_BlockCount               blockCount );

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! @name Iterator access */
     /*@{*/
    /* -----------------------------------------------------------------------*/

    /*!
       @brief   This method returns the identifier of the first log device.
                It's not guaranteed that the returned device number 
                specifies a device which is online.
       @return  IOMan_DeviceNo
     */

    IOMan_DeviceNo GetFirstLogVolumeNo() const{
        return( m_LogDevices.FirstDeviceNo() );
    }

     /*!
       @brief   This method returns the next identifier of the log device
                following the given log device. It's not guaranteed that
                the returned device identifier specifies a device which
                is online.
       @param   devNo [in] Logical device identifier
       @return  IOMan_DeviceNo
     */

    IOMan_DeviceNo GetNextLogVolumeNo( const IOMan_DeviceNo devNo ) const{
        return( devNo + 1 );
    }

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! @name Administration */
     /*@{*/
    /* -----------------------------------------------------------------------*/

     /*!
       @brief   This method is used to detach all configured log devices, 
                which are in mode open.
       @param   taskId [in] Identification of the calling task.
       @return  none
     */

    void CloseAll( const tsp00_TaskId taskId );

    /*!
       @brief   This method is used to create all configured log devices,
                i.e. each log devices is generated and formated. At the end 
                all log devices are in mode open.
       @param   taskId [in] Identification of the calling task.
       @param   pDBIdentifier [in] Unique data base identifier
       @return  In case of an error false is returned else true.
     */

    SAPDB_Bool CreateAll(
        const tsp00_TaskId taskId,
        const SAPDB_Byte   *pDBIdentifier );

    /*!
       @brief   This method is used to attach all configured log devices,
                which are not still in mode open.
       @param   taskId        [in] Identification of the calling task.
       @param   bOpenForWrite [in] true means that the log area will be
                                   accessible for read and write calls;
                                   false means that only read accesses are allowed.
                                   This is used by the HotStandBy solution..
       @return  In case of an error false is returned else true.
     */

    SAPDB_Bool OpenAll( 
        const tsp00_TaskId taskId,
        const SAPDB_Bool   bOpenForWrite );

    /*!
       @brief   This method is used to add a new log devices to the 
                log in warm as in cold mode too. The proper activation 
                of the new device must be done by the Log_IOManager.
       @param   taskId         [in] Identification of the calling task.
       @param   devName        [in] Name of the new volume
       @param   mirrDevName    [in] Name of the mirrored volume (log mode dual only)
       @param   configuredSize [in] Size of the new device in pages
       @param   devNo          [in] Logical identifier of the device
       @param   pages          [in] (nil)-Log pages used for device formating
       @return  true means that add log device was successful
     */

    SAPDB_Bool AddVolume(
        const tsp00_TaskId          taskId,
        tsp00_VFilename             &devName,
        tsp00_VFilename             &mirrDevName,
        const IOMan_BlockCount      configuredSize,
        const IOMan_DeviceNo        devNo,
        IOMan_LogPages              &pages );

    /*!
       @brief   This method is used to adjust the capacity of the log 
                devices stored in the IOManInfoPage's.
       @param   taskId      [in] Identification of the calling task.
       @param   msgList [in|out] messageList
       @return  (IOMan_ReturnCode) 
     */

    IOMan_ReturnCode MigrateCapacity(
        const tsp00_TaskId  taskId,
        IOMan_MessageList   &msgList );

     /*!
       @brief   This method is used to repair a corrupted log volume by copy
                the intact log volume of the same log device onto the corrupted.
                If the given log volume isn't bad nothing happens. This
                feature is available in log mode dual only.
       @param   taskId [in] Identification of the calling task.
       @param   badDevName [in] Name of the bad log volume to be reintegrated
       @return  true means that reintegrate log volume was successful
     */

    SAPDB_Bool ReIntegrateBadLogVolume(
        const tsp00_TaskId  taskId,
        tsp00_VFilename     &badDevName );

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! @name Configuration and statistic information */
     /*@{*/
    /* -----------------------------------------------------------------------*/

    /*!
       @brief   This method is used to determine the name of a log volume
                which is marked as bad resp. not accessable, because it's
                removed. This method is only available in mode admin and
                only if log mode dual is configured.
       @param   taskId [in] Identification of the calling task.
       @param   badDevName [out] Name of the bad log volume
       @return  true means bad log volme found
     */

    SAPDB_Bool GetBadLogVolume(
        const tsp00_TaskId  taskId,
        tsp00_VFilename     &badDevName );

    /*!
       @brief   This method is used to determine the logical identifier for
                a given log devices name. It is not needed that the log 
                devices are in mode open. In case of a mirrored 
                log, it is returned whether the given log volume name identifies 
                the primary volume or the secondary of the log device.
       @param   devName     [in] Name of a log volume
       @param   devNo      [out] Logical volume identifier
       @param   bIsPrimary [out] Is primary log volume
       @return  true means devices was found; else false
     */

    SAPDB_Bool GetVolumeNo(
        tsp00_VFilename &devName,
        IOMan_DeviceNo  &devNo,
        SAPDB_Bool      &bIsPrimary );

    /*!
       @brief   This method returns the number of all configured log devices 
                This number is also available if not all log devices are 
                in mode open.
       @return  (SAPDB_Int)
     */

    SAPDB_Int ConfiguredVolumes() const{
        return( m_ConfiguredDevices );
    }

    /*!
       @brief   This method provides the maximum number of configurable 
                log volumes.
       @return  SAPDB_Int
     */

    virtual SAPDB_Int MaxConfigurableVolumes() const{
        return( m_LogDevices.MaxDevices() );
    }

    /*!
       @brief   This method provides whether the log area consists of mirrored
                log volumes.
       @return  SAPDB_Int
     */

    virtual SAPDB_Bool IsLogMirrored() const
    {
        return( m_bIsLogMirrored );
    }

    /*!
       @brief   This method returns the sum of all pages storable within all 
                configured log devices. This  value is only available if all 
                log devices are in mode open.
       @return  (IOMan_BlockCount)
     */

    IOMan_BlockCount  TotalUsableLogPages() const{
        return( m_TotalLogPages );
    }

    /*!
       @brief   This method returns the number of log devices which are 
                currently in mode open. Usualy all configured log devices 
                are still in mode open.
       @return  (SAPDB_Int)
     */

    SAPDB_Int UsedVolumes() const{
        return( m_UsedDevices );
    }

    /*!
       @brief   This method checks the validity of the given log device number
       @param   devNo [in] Logical device identifier
       @return  SAPDB_Bool
     */

    SAPDB_Bool IsLogVolumeNoValid( const IOMan_DeviceNo devNo ) const{
        return( m_LogDevices.IsDeviceNoValid( devNo ));
    }

    /*!
       @brief   This method returns whether the given log device is in
                state online.
       @param   devNo [in] Logical device identifier
       @return  SAPDB_Bool
     */

    SAPDB_Bool IsLogVolumeOnline( const IOMan_DeviceNo devNo ) const
    {
        if( ! m_LogDevices.IsDeviceNoValid( devNo ) )
            return( false );

        return( Get( devNo ).IsOnline() );
    }
    
    /*!
        @brief Returns cluster size in pages used for log page io
        @return (SAPDB_UInt) value in pages
    */
    SAPDB_UInt GetIOBlockCount() const{
        return m_BlockIoCount;
    }
    
    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/

private:

    /*!
       @brief   format all configured log devices
       @param   taskId      [in] Identification of the calling task
       @param   msgList [in|out] messageList
       @return  SAPDB_Bool success
     */

    SAPDB_Bool FormatAll( 
        const tsp00_TaskId  taskId,
        IOMan_MessageList   &msgList );

    /*!
       @brief   This method is used to create a linkage between the given
                log devices. The link anchor is stored within the IOManInfo
                page of the volumes
       @param   prevDevice [in] logical previous log device
       @param   prevDevice [in] current log device
       @return  none
     */

    void CreateLinkage(
        const tsp00_TaskId  taskId,
        IOMan_ILogDevice    &prevDevice,
        IOMan_ILogDevice    &currDevice );

    /*!
       @brief   This method is used to the access to the predecessor and 
                successor of the given volume. If a no successor or predecessor
                is available NULL ist returned; else a pointer to the dbIdentifier 
                is returned.
       @param   currDevice  [in] current log device 
       @param   pPrevVolume [out] previous volume 
       @param   pNextVolume [out] next volume 
       @return  (SAPDB_Byte*) dbIdentifier
     */

    const SAPDB_Byte *DetermineLinkage(
        const IOMan_ILogDevice  &currDevice,
        IOMan_ILogDevice        **pPrevDevice,
        IOMan_ILogDevice        **pNextDevice );

    /*!
        @brief  This method is used to write a couple of log pages to the
                given log. If the write operation yields to an error the system will
                crash.
        @param  taksId [in] task identification
        @param  device [in] current log device
        @param  pages  [in] collection of log pages to be written
        @return none
     */

    void WriteLogPages(
        const tsp00_TaskId  taskId,
        IOMan_ILogDevice    &device,
        IOMan_LogPages      &pages );

    /// Copy and assignment operator are not supported

    /*!
       @brief   The assignment operator is not supported to avoid not allowed
                duplicates.
       @param   logControl [in] logControl 
       @return  logControl&
     */

    IOMan_LogArea& operator=( const IOMan_LogArea &logControl );

    /*!
       @brief   The copy operator is not supported to avoid not allowed
                duplicates.
       @param   logControl [in] logControl 
       @return  logControl&
     */

   IOMan_LogArea( const IOMan_LogArea &logControl );

private:

    // Allocator for page frames. The page frame size is definded by
    // the data base configuration file.
    IOMan_PageFrameAllocator&   m_PageAllocator;

    // Named allocator for non page allocation.
    SAPDBMem_IRawAllocator& m_Allocator;

    // Defines cluster size in pages used for log page io
    const SAPDB_UInt m_BlockIoCount;
    
    // specifies whether log mode dual is configured. The log mode
    // is set initialy during singleton creation
    const SAPDB_Bool  m_bIsLogMirrored;

    // Is the maximum number of pages storable in all log devices.
    IOMan_BlockCount m_TotalLogPages;

    // Is the number of configured log devices.
    SAPDB_UInt m_ConfiguredDevices;

    // Is the logical identifier for the log device which was involved
    // in the last read or write access. Probable this will be the
    // destination of the next operation too.
    IOMan_DeviceNo  m_LastAccessedDevice;

    // Is the current number of log devices which are in mode open.
    SAPDB_UInt  m_UsedDevices;

    // Collection of log devices managed by this class
    IOMan_LogDevices    m_LogDevices;
};


#endif  /* IOMAN_LOG_CONTROL_HPP */
