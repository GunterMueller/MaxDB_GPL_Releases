/*!
  @file     IOMan_LogDevice.hpp
  @author   TorstenS
  @ingroup  IOManagement
  @brief    Internal interface for log devices in particular 
            single and dual log devices.

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



#ifndef IOMAN_ILOG_DEVICE_HPP
#define IOMAN_ILOG_DEVICE_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"  

#include "IOManager/IOMan_ClusterAddress.hpp"
#include "IOManager/IOMan_InfoPage.hpp"
#include "IOManager/IOMan_KernelPages.hpp"
#include "IOManager/IOMan_LogVolume.hpp"
#include "IOManager/IOMan_Types.hpp"
#include "KernelCommon/Kernel_Common.hpp"
#include "DataAccess/Data_PageFrame.hpp"
#include "Logging/Log_Types.hpp"

class IOMan_MessageList;
class IOMan_IPages;

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
   @interface   IOMan_ILogDevice
   @brief       This class is used as a log device interface. It describes
                the properties of the log devices in particular the single
                and dual log device. A single log device consists of one
                log volume and a dual log device of two volumes.
 */

class IOMan_ILogDevice
{
public:

    /* -----------------------------------------------------------------------*/
    /*! @name Administration */
    /*@{*/
    /* -----------------------------------------------------------------------*/

    /*!
       @brief   This method is used to set the unique log device identifier
                and to give the I/O capable memory for the IOManInfoPage for
                each log volume.
       @param   devNo     [in] Logical device identifier
       @param   frame     [in] Frame for the IOManInfoPage of the log volume
       @param   mirrFrame [in] Frame for the IOManInfoPage of the mirrored log
                               volume (log mode dual only)
       @return         none
     */

    virtual void Initialize(
        const IOMan_DeviceNo            devNo,
        const Data_PageFrame   &frame,
        const Data_PageFrame   &mirrFrame ) = 0;

    /*!
       @brief   This method is used to bring the log device from open mode into 
                close mode, presumed it's in open mode. The device is detached.
       @param   taskId [in] Identification of the calling task.
       @return  none
     */

    virtual void Close( const tsp00_TaskId taskId ) = 0;

    /*!
       @brief   This method is used to write the IOMan_InfoPage to the log device.
       @param   taskId        [in] Identification of the calling task.
       @param   pDBIdentifier [in] Unique data base identifier
       @return  (IOMan_ReturnCode) IOMan_Okay means CreateInfoPage was successful
     */

    virtual IOMan_ReturnCode CreateInfoPage(
        const tsp00_TaskId   taskId,
        const SAPDB_Byte     *pDBIdentifier ) = 0;

    /*!
       @brief   This method is used to create and format a log device.
                If the creation is finished successful the method provides true 
                else false. It is assumed and tested by an assertion that the 
                device is in closed mode.
       @param   taskId        [in] Identification of the calling task.
       @param   pDBIdentifier [in] Unique data base identifier
       @return  (IOMan_ReturnCode) IOMan_Okay means create was successful
     */

    virtual IOMan_ReturnCode Create(
        const tsp00_TaskId  taskId,
        const SAPDB_Byte    *pDBIdentifier ) = 0;

    /*!
       @brief   This method is used to bring the log device from close mode into
                open mode, presumed it isn't in open mode already. The device 
                is attached.
       @param   taskId [in] Identification of the calling task.
       @param   logCapacity [in] Current size in pages of log devices opened so far.
       @param   bOpenForWrite [in] true means that the log volume is read- and
                                    writable; else it is only readable.
       @return  (IOMan_ReturnCode) IOMan_Okay means open was successful
     */

    virtual IOMan_ReturnCode Open(
        const tsp00_TaskId      taskId,
        const IOMan_BlockCount  logCapacity,
        const SAPDB_Bool        bOpenForWrite ) = 0;

    /*!
       @brief   This method is used to inform the RTE about the presence of another
                new log device. It's required and checked whether the given device 
                is in mode offline. Furhtermore the added device is in mode closed.
       @param   devName [in] name of the original device to be added
       @param   mirrDevName [in] name of the mirrored device (log mode dual only)
       @param   devSize [in] capacity of the device to be added
       @return  true if add was successful
     */

    virtual SAPDB_Bool Add(
        tsp00_VFilename         &devName,
        tsp00_VFilename         &mirrDevName,
        const IOMan_BlockCount  devSize ) const = 0;

    /*!
       @brief   This method is used to inform the RTE about the removing of 
                the log device. It's required and checked whether the given 
                device is in mode offline.
       @return  none
     */

    virtual void Del() const = 0;

    /*!
       @brief   This method is used to adjust the capacity of the log 
                device stored in the IOManInfoPage's.
       @param   taskId      [in] Identification of the calling task.
       @param   msgList [in|out] messageList
       @return  (IOMan_ReturnCode) 
     */

    virtual IOMan_ReturnCode MigrateCapacity(
        const tsp00_TaskId  taskId,
        IOMan_MessageList   &msgList ) = 0;

    /*!
       @brief   This method is used to write the logical device number of 
                the next logical device into the info page of the
                current device.
       @param   taskId     [in] Identification of the calling task.
       @param   nextDevice [in] device handler
       @return  (SAPDB_Bool) linkage is created successfully
     */

    virtual SAPDB_Bool SetNextLinkage(
        const tsp00_TaskId      taskId,
        const IOMan_ILogDevice  &nextDevice) = 0;

    /*!
       @brief   This method is used to write the logical device number of 
                the previous logical device into the info page of the
                current device.
       @param   taskId     [in] Identification of the calling task.
       @param   prevDevice [in] device handler
       @return  (SAPDB_Bool) linkage is created successfully
     */

    virtual SAPDB_Bool SetPrevLinkage(
        const tsp00_TaskId      taskId,
        const IOMan_ILogDevice  &prevDevice ) = 0;

    /*!
       @brief   This method is used to validate the linkage between the currrent
                and the given log device.
       @param   taskId [in] Identification of the calling task.
       @param   device [in] device handler
       @return  (SAPDB_Bool) true if linkage is valid; else false
     */

    virtual SAPDB_Bool IsLinkageValid(
        const tsp00_TaskId      taskId,
        const IOMan_ILogDevice  &device ) const = 0;

    /*!
       @brief   This method is used to validate if a logical predecessor 
                device is definded.
       @return  (SAPDB_Bool) true if predecessor device is definded
     */

    virtual SAPDB_Bool IsPrevLinkageValid() const = 0;

    /*!
       @brief   This method is used to validate if a logical successor 
                device is definded.
       @return  (SAPDB_Bool) true if successor device is definded
     */

    virtual SAPDB_Bool IsNextLinkageValid() const = 0;

    /*!
       @brief   This method is used to compare the DBIdentifier of the 
                currrent and the given device. If the identifiers are equal 
                true is returned else false.
       @param   taskId [in] Identification of the calling task.
       @param   device [in] device handler
       @return  (SAPDB_Bool) true if identifiers are equal; else false
     */

    virtual SAPDB_Bool IsDBIdentifierValid(
        const tsp00_TaskId      taskId,
        const IOMan_ILogDevice  &device ) const = 0;

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/
    /* -----------------------------------------------------------------------*/
    /*! @name Common information */
    /*@{*/
    /* -----------------------------------------------------------------------*/

    /*!
       @brief   This method is used to get the configured device capacity in
                blocks. This capacity is equal to corresponding configuration
                file entry and describes the physical size of the device.
       @return  IOMan_BlockCount
     */

    virtual IOMan_BlockCount GetConfiguredSize() const = 0;

    /*!
       @brief   This method is used to get the internal device capacity in
                blocks, i.e. this is the configured size of the device
                reduce by the RTE INFO PAGE.
       @return  IOMan_BlockCount
     */

    virtual IOMan_BlockCount GetInternalSize() const = 0;

    /*!
       @brief   This method is used to get the usable device capacity in
                blocks, i.e. this is the configured size of the device
                reduce by the RTE INFO PAGE and the IOMAN INFO PAGE.
       @return  IOMan_BlockCount
     */

    virtual IOMan_BlockCount GetUsableSize() const = 0;

    /*!
       @brief   This method is used to get the unique identifier of the device.
       @return  (const SAPDB_Byte*) pointer to (not null terminated) string
     */

    virtual const SAPDB_Byte* GetDBIdentifier() const = 0;

    /*!
       @brief   This method is used to get the logical identifier of the
                device. All devices of the same device type are counted 
                begining by ONE(!).
       @return  logical device id

     */

    virtual IOMan_DeviceNo  GetLogicalDevNo() const = 0;

    /*!
       @brief   This method is used to get the configured name of the volume.
                If no valid volume name is found false is returned and the
                incoming volume name is not touched. Else the configured 
                volume name is stored within the incoming parameter. Is is 
                assumed and tested by an assertion that the device is in open 
                mode. If log mode dual is defined the parameter bIsMirror
                defines whether name of the original or the mirrored volume 
                is requested. If no log mode dual is configured this parameter 
                will beignored.
       @param   devName  [out] device name
       @param   bIsMirror [in] name belongs to the mirror
       @return  true means success; else false
     */

    virtual SAPDB_Bool GetName(
        tsp00_VFilename     &devName,
        const SAPDB_Bool    bIsMirror = false ) const = 0;

    /*!
       @brief   This method is used to get the type for example data or log
                of the current device.
       @return  device type with null termination
     */

    virtual SAPDB_Char* GetTypeName() const = 0;

    /*!
       @brief   This method is used to read the logical device number of
                the log device which is the next in logical order. This number
                must be equal to the logical next log device within the data 
                base configuration file.
       @return  IOMan_DeviceNo
     */

    virtual IOMan_DeviceNo GetNextLogicalDevNo() const = 0;

    /*!
       @brief   This method is used to read the logical device number of
                the log device which is the predecessor in logical order. 
                This number must be equal to the logical predecessor log 
                device within the data base configuration file.
       @return  IOMan_DeviceNo
     */

    virtual IOMan_DeviceNo GetPrevLogicalDevNo() const = 0;

    /*!
       @brief   This method is used to check whether the current log device 
                exists, i.e. is stored in the data base configuration file.
       @return  true means is known as a valid log device; else false
     */

    virtual SAPDB_Bool IsConfigured() const = 0;

    /*!
       @brief   This method is used to determine whether the log device 
                is in open mode.
       @return  true means is in open mode; else false
     */

    virtual SAPDB_Bool IsOnline() const = 0;

    /*!
       @brief   This method provides the first logical block number of the
                log device. For example if a log consist of two 
                log devices with the capacity of 100 pages at each case. 
                The FirstOffset of the first device is zero and of the 
                second is 100.
       @return  Log_NonCyclicRawDeviceOffset
     */

    virtual Log_NonCyclicRawDeviceOffset FirstOffset() const = 0;

    /*!
       @brief   This method provides the last logical block number of the
                log device. For example if a log consist of two 
                log devices with the capacity of 100 pages at each case. 
                The LastOffset of the first device is 99 and of the 
                second is 199.
       @return  Log_NonCyclicRawDeviceOffset
     */

    virtual Log_NonCyclicRawDeviceOffset LastOffset() const = 0;

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! @name Input/Output */
    /*@{*/
    /* -----------------------------------------------------------------------*/

    /*!
       @brief   This method is used to read one log page from a log device 
                into the given memory. The correctness  of the read page will 
                be checked as far as three times if an error has occured. Each 
                error is loged in the knldiag file and the io buffer is dumped.
                In any case of an error for example checksum or I/O subsystem
                errors an emergency shutdown is executed.
       @param   taskId [in] Identification of the calling task.
       @param   page   [in] Handler of the wanted Page.
       @param   block  [in] Position of the wanted page within the device.
       @return  none
     */

    virtual void ReadPage(
        const tsp00_TaskId         taskId,
        IOMan_IPage               &page,
        const IOMan_BlockAddress   &block ) = 0;

    /*!
       @brief   This method is used to read a couple of log pages from a log 
                device into the given memory. The correctness  of the read 
                pages will be checked as far as three times if an error has 
                occured. Each error is loged in the knldiag file and the io 
                buffer is dumped.In any case of an error for example checksum 
                or I/O subsystem errors an emergency shutdown is executed.
       @param   taskId     [in] Identification of the calling task.
       @param   pages      [in] Handler of the pages to be read
       @param   cluster    [in] Address of the pages to be read
       @param   startIndex [in] Specifies the first page in the page collection 
                                which is relevant for the following I/O. Usually
                                the startIndex is zero. Only in case of read
                                orders splited over more than one log device
                                the startIndex is greater than zero.
       @return  none
     */

    virtual void ReadPages(
        const tsp00_TaskId         taskId,
        IOMan_IKernelPages         &pages,
        const IOMan_ClusterAddress &cluster,
        const SAPDB_UInt           startIndex ) = 0;

    /*!
       @brief   This method is used to read a page (default) from a device 
                into the given memory. The correctness  of the read page won't
                be checked. In case of a fatal errror during the read operation
                caused by the IO subsystem the error is logged in the knldiag 
                file and the return code is false. The device is marked as bad.
       @param   taskId       [in] Identification of the calling task.
       @param   pFrame       [in] Memory where the read page will be stored.
       @param   blockNo      [in] Position of the wanted page within the device.
       @param   bFromPrimary [in] Read from primary log volume of the device
       @param   blockCount   [in] Number of pages to be read in one step
                                Default is one page
       @return  true if the read operation is successful; else false.
     */

    virtual SAPDB_Bool ReadSimple(
        const tsp00_TaskId      taskId,
        const tsp00_PageAddr    pFrame,
        const IOMan_BlockNo     blockNo,
        const SAPDB_Bool        bFromPrimary,
        const IOMan_BlockCount  blockCount = 1 ) = 0;

    /*!
       @brief   This method is used to write one log page from given memory into
                a log device. Before the proper io is executed the correctness
                of the given page will be checked. Each error is loged in the 
                knldiag file and the io buffer is dumped. In any case of error 
                an emergency shutdown is executed.
       @param   taskId [in] Identification of the calling task.
       @param   page   [in] Handler of the wanted Page.
       @param   block  [in] Position of the wanted page within the device.
       @return  none
     */

    virtual void WritePage(
        const tsp00_TaskId          taskId,
        IOMan_IPage                &page,
        const IOMan_BlockAddress    &block ) = 0;

    /*!
       @brief   This method is used to write log pages from given memory into
                a log device. Before the proper io is executed the correctness of  
                the given page will be checked. Each error is loged in the 
                knldiag file and the io buffer is dumped. In any case of error 
                an emergency shutdown is executed.
       @param   taskId     [in] Identification of the calling task.
       @param   pages      [in] Handler of the pages to be written
       @param   cluster    [in] Destination address of the pages to be written
       @param   startIndex [in] Specifies the first page in the page collection 
                                which is relevant for the following I/O. Usually
                                the startIndex is zero. Only in case of write
                                orders splited over more than one log device
                                the startIndex is greater than zero.
       @return  none
     */

    virtual void WritePages(
        const tsp00_TaskId          taskId,
        IOMan_IKernelPages          &pages,
        const IOMan_ClusterAddress  &cluster,
        const SAPDB_UInt            startIndex = IOMan_IKernelPages::FirstIndex()) = 0;

    /*!
       @brief   This method is used to write a page onto a device 
                The correctness  of the page won't be  checked. In case of a 
                fatal errror during the write operation caused by the IO subsystem
                the error is logged in the knldiag file and the return code is 
                false. The device is marked as bad.
       @param   taskId     [in] Identification of the calling task.
       @param   pFrame     [in] Pointer to the frame
       @param   blockNo    [in] Position for the page within the device.
       @param   bToPrimary [in] Write to primary log volume of the device
       @return  true if the write operation is successful; else false.
     */

    virtual SAPDB_Bool WriteSimple(
        const tsp00_TaskId      taskId,
        const tsp00_PageAddr    pFrame,
        const IOMan_BlockNo     blockNo,
        const SAPDB_Bool        bToPrimary ) = 0;

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! @name Volume access */
    /*@{*/
    /* -----------------------------------------------------------------------*/

    /*!
       @brief   In case of a single log device this method is used to
                access the log volume. In case of a dual log device the
                access to the primary log volume is returned.
       @return  none
     */

    virtual IOMan_LogVolume&  GetPrimaryVolume() = 0;

    virtual const IOMan_LogVolume&  GetPrimaryVolume() const = 0;

    /*!
       @brief   In case of a single log device this method is used to
                access the log volume as GetPrimaryVolume In case of a 
                dual log device the access to the secondary log volume 
                is returned.
       @return  none
     */

    virtual IOMan_LogVolume&  GetSecondaryVolume() = 0;

    virtual const IOMan_LogVolume&  GetSecondaryVolume() const = 0;

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/

protected:

    // Do not create a pure object of this
    IOMan_ILogDevice(){}
};

#endif  /* IOMAN_ILOG_DEVICE_HPP */
