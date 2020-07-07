/*!
  @file     IOMan_DualLogDevice.hpp
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



#ifndef IOMAN_DUAL_LOG_DEVICE_HPP
#define IOMAN_DUAL_LOG_DEVICE_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"   // PASCAL: SP_basic_constants_and_types

#include "IOManager/IOMan_LogDevice.hpp"
#include "IOManager/IOMan_LogVolume.hpp"
#include "IOManager/IOMan_MirrorLogVolume.hpp"
#include "KernelCommon/Kernel_Common.hpp"

class IOMan_IPage;
class IOMan_MessageList;

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
  @class    IOMan_DualLogDevice
  @brief    This class implements the dual log device which is
            a variant of the pure virtual interface IOMan_LogDevice.
 */

class IOMan_DualLogDevice : public IOMan_ILogDevice
{
public:

    /* -----------------------------------------------------------------------*/
    /*! @name Constructors and initialization */
    /*@{*/
    /* -----------------------------------------------------------------------*/

    /*!
       @brief   Constructor of a dual log device, whichs contains of two
                log volumes.
       @return  none
     */

    IOMan_DualLogDevice();

    /*!
       @brief   This method is used to set the unique log device identifier
                and to give the I/O capable memory for the IOManInfoPage for
                each log volume.
       @param   devNo     [in] Logical device identifier
       @param   frame     [in] Frame for the IOManInfoPage of the log volume
       @param   mirrFrame [in] Frame for the IOManInfoPage of the mirrored log
                               volume.
       @return  none
     */

    virtual void Initialize(
        const IOMan_DeviceNo            devNo,
        const Data_PageFrame   &frame,
        const Data_PageFrame   &mirrFrame )
    {
        m_PrimaryLogVolume.Initialize( devNo, frame );
        m_SecondaryLogVolume.Initialize( devNo, mirrFrame );
    }

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! @name Administration */
    /*@{*/
    /* -----------------------------------------------------------------------*/

    /*!
       @brief   This method is used to bring the log device from open mode into 
                close mode, presumed it's in open mode. The device is detached.
       @param   taskId [in] Identification of the calling task.
       @return  none
     */

    virtual void Close( const tsp00_TaskId taskId )
    {
        m_PrimaryLogVolume.Close( taskId );
        m_SecondaryLogVolume.Close( taskId );
    }

    /*!
       @brief   This method is used to write the IOMan_InfoPage to this set of 
                log volumes.
       @param   taskId        [in] Identification of the calling task.
       @param   pDBIdentifier [in] Unique data base identifier
       @return  (IOMan_ReturnCode) IOMan_Okay means CreateInfoPage was successful
     */

    IOMan_ReturnCode CreateInfoPage(
        const tsp00_TaskId   taskId,
        const SAPDB_Byte     *pDBIdentifier )
    {
        m_PrimaryLogVolume.CreateInfoPage( taskId, pDBIdentifier );

        return m_SecondaryLogVolume.CreateInfoPage( taskId, pDBIdentifier );
    }

    /*!
       @brief   This method is used to create and format a log device.
                If the creation is finished successful the method provides true 
                else false. It is assumed and tested by an assertion that the 
                device is in closed mode. If both log volumes have different
                size the error IOMan_VolumeCapacityMismatch is set.
       @param   taskId        [in] Identification of the calling task.
       @param   pDBIdentifier [in] Unique data base identifier
       @return  (IOMan_ReturnCode) IOMan_Okay means create was successful
     */

    virtual IOMan_ReturnCode Create(
        const tsp00_TaskId  taskId,
        const SAPDB_Byte    *pDBIdentifier );

    /*!
       @brief   This method is used to bring the log device from close mode into
                open mode, presumed it isn't in open mode already. The device 
                is attached.
       @param   taskId        [in] Identification of the calling task.
       @param   logCapacity   [in] Current size in pages of log devices opened so far.
       @param   bOpenForWrite [in] true means that the log volume is read- and
                                    writable; else it is only readable.
       @return  (IOMan_ReturnCode) IOMan_Okay means open was successful
     */

    virtual IOMan_ReturnCode Open(
        const tsp00_TaskId      taskId,
        const IOMan_BlockCount  logCapacity,
        const SAPDB_Bool        bOpenForWrite );

    /*!
       @brief   This method is used to inform the RTE about the presence of another
                new log device. It's required and checked whether the given device 
                is in mode offline. Furhtermore the added device is in mode closed.
       @param   devName [in] name of the original volume to be added
       @param   mirrDevName [in] name of the mirrored volume to be added
       @param   devSize [in] capacity of the device to be added
       @return  (SAPDB_Bool) true if add was successful
     */

    virtual SAPDB_Bool Add(
        tsp00_VFilename         &devName,
        tsp00_VFilename         &mirrDevName,
        const IOMan_BlockCount  devSize ) const
    {
        if( ! m_PrimaryLogVolume.Add( devName, devSize ))
            return false ;

        return m_SecondaryLogVolume.Add( mirrDevName, devSize );
    }

    /*!
       @brief   This method is used to inform the RTE about the removing of 
                the log device. It's required and checked whether the given 
                device is in mode offline.
       @return  none
     */

    virtual void Del() const
    {
        m_PrimaryLogVolume.Del();
        m_SecondaryLogVolume.Del();
    }

    /*!
       @brief   This method is used to adjust the capacity of the log 
                device stored in the IOManInfoPage's.
       @param   taskId      [in] Identification of the calling task.
       @param   msgList [in|out] messageList
       @return  (IOMan_ReturnCode) 
     */

    virtual IOMan_ReturnCode MigrateCapacity(
        const tsp00_TaskId  taskId,
        IOMan_MessageList   &msgList )
    {
        const IOMan_ReturnCode retCode = m_PrimaryLogVolume.MigrateCapacity( taskId, msgList );

         if( IOMan_Okay != retCode )
            return retCode;

        return m_SecondaryLogVolume.MigrateCapacity( taskId, msgList );
    }

    /*!
       @brief   This method is used to repair a corrupted log volume by copy
                the intact log volume of the same log device onto the corrupted.
       @param   taskId [in] Identification of the calling task.
       @param   badDevName [in] Name of the bad log volume to be reintegrated
       @return  true means that reintegrate log volume was successful
     */

    SAPDB_Bool ReIntegrateBadLogVolume(
        const tsp00_TaskId  taskId,
        tsp00_VFilename     &badDevName );

    /*!
        @brief  This method is used to write the logical device number of 
                the next logical device into the info page of the
                current device.
        @param  taskId     [in] Identification of the calling task.
        @param  nextDevice [in] device handler
        @return (SAPDB_Bool) linkage is created successfully
     */

    virtual SAPDB_Bool SetNextLinkage(
        const tsp00_TaskId      taskId,
        const IOMan_ILogDevice  &nextDevice )
    {
        if( ! m_PrimaryLogVolume.SetNextLinkage( taskId, nextDevice.GetPrimaryVolume()))
            return SAPDB_FALSE;

        return m_SecondaryLogVolume.SetNextLinkage( taskId, nextDevice.GetSecondaryVolume());
    }

    /*!
        @brief  This method is used to write the logical device number of 
                the previous logical device into the info page of the
                current device.
        @param  taskId     [in] Identification of the calling task.
        @param  prevDevice [in] device handler
        @return (SAPDB_Bool) linkage is created successfully
     */

    virtual SAPDB_Bool SetPrevLinkage(
        const tsp00_TaskId      taskId,
        const IOMan_ILogDevice  &prevDevice )
    {
        if( ! m_PrimaryLogVolume.SetPrevLinkage( taskId, prevDevice.GetPrimaryVolume()))
            return SAPDB_FALSE;

        return m_SecondaryLogVolume.SetPrevLinkage( taskId, prevDevice.GetSecondaryVolume());
    }

    /*!
       @brief   This method is used to validate the linkage between the currrent
                and the given log device.
       @param   taskId [in] Identification of the calling task.
       @param   device [in] device handler
       @return  (SAPDB_Bool) true if linkage is valid; else false
     */

    virtual SAPDB_Bool IsLinkageValid(
        const tsp00_TaskId      taskId,
        const IOMan_ILogDevice  &device ) const
    {
        if( ! m_PrimaryLogVolume.IsLinkageValid( taskId, device.GetPrimaryVolume()))
            return SAPDB_FALSE;

        return m_SecondaryLogVolume.IsLinkageValid( taskId, device.GetSecondaryVolume());
    }

    /*!
       @brief   This method is used to validate if a logical predecessor 
                device is definded.
       @return  (SAPDB_Bool) true if predecessor device is definded
     */

    virtual SAPDB_Bool IsPrevLinkageValid() const
    {
        if( ! m_PrimaryLogVolume.IsPrevLinkageValid())
            return SAPDB_FALSE;

        return m_SecondaryLogVolume.IsPrevLinkageValid();
    }

    /*!
       @brief   This method is used to validate if a logical successor 
                device is definded.
       @return  (SAPDB_Bool) true if successor device is definded
     */

    virtual SAPDB_Bool IsNextLinkageValid() const
    {
        if( ! m_PrimaryLogVolume.IsNextLinkageValid())
            return SAPDB_FALSE;

        return m_SecondaryLogVolume.IsNextLinkageValid();
    }

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
        const IOMan_ILogDevice  &device ) const
    {
        return m_PrimaryLogVolume.IsDBIdentifierValid( taskId, device.GetPrimaryVolume());
    }

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/
    /* -----------------------------------------------------------------------*/
    /*! @name Common information                                              */
    /*@{*/
    /* -----------------------------------------------------------------------*/

    /*!
       @brief   This method is used to determine the name of a log volume
                which is marked as bad resp. not accessable, because it's
                removed.
       @param   taskId [in] Identification of the calling task.
       @param   badDevName [out] Name of the bad log volume
       @return  true means bad log volme found
     */

    SAPDB_Bool GetBadLogVolume(
        const tsp00_TaskId  taskId,
        tsp00_VFilename     &badDevName );

    /*!
        @brief   This method is used to get the configured device capacity in
                 blocks. This capacity is equal to corresponding configuration
                 file entry and describes the physical size of the device.
        @return  IOMan_BlockCount
      */

    virtual IOMan_BlockCount GetConfiguredSize() const
    {
        SAPDBERR_ASSERT_STATE( m_PrimaryLogVolume.GetConfiguredSize() ==
                               m_SecondaryLogVolume.GetConfiguredSize());

        return m_PrimaryLogVolume.GetConfiguredSize();
    }

    /*!
       @brief   This method is used to get the internal device capacity in
                blocks, i.e. this is the configured size of the device
                reduce by the RTE INFO PAGE.
       @return  IOMan_BlockCount
     */

    virtual IOMan_BlockCount GetInternalSize() const
    {
        SAPDBERR_ASSERT_STATE( m_PrimaryLogVolume.GetConfiguredSize() ==
                               m_SecondaryLogVolume.GetConfiguredSize());

        return m_PrimaryLogVolume.GetInternalSize();
    }

    /*!
       @brief   This method is used to get the usable device capacity in
                blocks, i.e. this is the configured size of the device
                reduce by the RTE INFO PAGE and the IOMAN INFO PAGE.
       @return  IOMan_BlockCount
     */

    virtual IOMan_BlockCount GetUsableSize() const
    {
        SAPDBERR_ASSERT_STATE( m_PrimaryLogVolume.GetConfiguredSize() ==
                               m_SecondaryLogVolume.GetConfiguredSize());

        return m_PrimaryLogVolume.GetUsableSize();
    }

    /*!
       @brief   This method is used to get the unique identifier of the 
                device.
       @return  (const SAPDB_Byte*) pointer to (not null terminated) string
     */

    virtual const SAPDB_Byte* GetDBIdentifier() const
    {
        return m_PrimaryLogVolume.GetDBIdentifier();
    }

    /*!
       @brief   This method is used to get the logical identifier of the
                device. All devices of the same device type are counted 
                begining by ONE(!).
       @return  logical device id
     */

    virtual IOMan_DeviceNo  GetLogicalDevNo() const
    {
        SAPDBERR_ASSERT_STATE( m_PrimaryLogVolume.GetLogicalDevNo() ==
                               m_SecondaryLogVolume.GetLogicalDevNo());

        return m_PrimaryLogVolume.GetLogicalDevNo();
    }

    /*!
       @brief   This method is used to get the configured name of the volume.
                If no valid volume name is found false is returned and the
                incoming volume name is not touched. Else the configured 
                volume name is stored within the incoming parameter. Is is 
                assumed and tested by an assertion that the device is in open 
                mode. The parameter bIsMirror defines whether name of the 
                original or the mirrored volume is requested.
       @param   devName [out] device name
       @param   bIsMirror [in] name belongs to the mirror
       @return  true means success; else false
     */

    virtual SAPDB_Bool GetName(
        tsp00_VFilename     &devName,
        const SAPDB_Bool    bIsMirror = false ) const
    {
        if( bIsMirror )
            return m_SecondaryLogVolume.GetName( devName );
        else
            return m_PrimaryLogVolume.GetName( devName );
    }

    /*!
       @brief   This method is used to get the type of the log device.
       @return  (SAPDB_Char*) device type
     */

    virtual SAPDB_Char* GetTypeName() const
    {
        return ( SAPDB_Char* )"DualLog";
    }

    /*!
       @brief   This method is used to read the logical device number of
                the log device which is the next in logical order. This number
                must be equal to the logical next log device within the data 
                base configuration file.
       @return  IOMan_DeviceNo
     */

    virtual IOMan_DeviceNo GetNextLogicalDevNo() const
    {
        return m_PrimaryLogVolume.GetNextLogicalDevNo();
    }

    /*!
       @brief   This method is used to read the logical device number of
                the log device which is the predecessor in logical order. 
                This number must be equal to the logical predecessor log 
                device within the data base configuration file.
       @return  IOMan_DeviceNo
     */

    virtual IOMan_DeviceNo GetPrevLogicalDevNo() const
    {
        return m_PrimaryLogVolume.GetPrevLogicalDevNo();
    }

    /*!
       @brief   This method is used to check whether the current log device 
                exists, i.e. is stored in the data base configuration file.
       @return  true means is known as a valid log device; else false
     */

    virtual SAPDB_Bool IsConfigured() const
    {
        if( ! m_PrimaryLogVolume.IsConfigured())
            return SAPDB_FALSE;
        else
            return m_SecondaryLogVolume.IsConfigured();
    }

    /*!
       @brief   This method is used to determine whether the log device 
                is in open mode.
       @return  true means is in open mode; else false
     */

    virtual SAPDB_Bool IsOnline() const
    {
        if( ! m_PrimaryLogVolume.IsOnline())
            return SAPDB_FALSE;
        else
            return m_SecondaryLogVolume.IsOnline();
    }

    /*!
       @brief   This method provides the first logical block number of the
                log device. For example if a log consist of two 
                log devices with the capacity of 100 pages at each case. 
                The FirstOffset of the first device is zero and of the 
                second is 100.
       @return  Log_NonCyclicRawDeviceOffset
     */

    virtual Log_NonCyclicRawDeviceOffset FirstOffset() const
    {
        SAPDBERR_ASSERT_STATE( m_PrimaryLogVolume.FirstOffset() == m_SecondaryLogVolume.FirstOffset());

        return m_PrimaryLogVolume.FirstOffset();
    }

    /*!
      @brief   This method provides the last logical block number of the
               log device. For example if a log consist of two 
               log devices with the capacity of 100 pages at each case. 
               The LastOffset of the first device is 99 and of the 
               second is 199.
      @return  Log_NonCyclicRawDeviceOffset
    */

    virtual Log_NonCyclicRawDeviceOffset LastOffset() const
    {
        SAPDBERR_ASSERT_STATE( m_PrimaryLogVolume.LastOffset() == m_SecondaryLogVolume.LastOffset());

        return m_PrimaryLogVolume.LastOffset();
    }

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! @name Input/Output                                                    */
    /*@{*/
    /* -----------------------------------------------------------------------*/

    /*!
       @brief   This method is used to read one log page from a log device 
                into the given memory. The correctness  of the read page will 
                be checked as far as three times if an error has occured. Each 
                error is loged in the knldiag file and the io buffer is dumped.
                In any case of an error for example checksum or I/O subsystem
                errors an emergency shutdown is executed. The device is marked 
                as bad.
       @param   taskId [in] Identification of the calling task.
       @param   page [in] Handler of the wanted Page.
       @param   block [in] Position of the wanted page within the device.
       @return  none
     */

    virtual void ReadPage(
        const tsp00_TaskId         taskId,
        IOMan_IPage               &page,
        const IOMan_BlockAddress   &block );

    /*!
       @brief   This method is used to read a couple of log pages from a log 
                device into the given memory. The correctness  of the read 
                pages will be checked as far as three times if an error has 
                occured. Each error is loged in the knldiag file and the io 
                buffer is dumped.In any case of an error for example checksum 
                or I/O subsystem errors an emergency shutdown is executed.
                The device is marked as bad.
       @param   taskId [in] Identification of the calling task.
       @param   pages [in] Handler of the pages to be read
       @param   cluster [in] Address of the pages to be read
       @param   startIndex [in] Specifies the first page in the page collection 
                                which is relevant for the following I/O. Usually
                               the startIndex is zero. Only in case of read
                               orders splited over more than one log device
                               the startIndex is greater than zero.
       @return   none
     */

    virtual void ReadPages(
        const tsp00_TaskId         taskId,
        IOMan_IKernelPages         &pages,
        const IOMan_ClusterAddress &cluster,
        const SAPDB_UInt           startIndex );

    /*!
       @brief   This method is used to read a page (default) from a device 
                into the given memory. The correctness  of the read page won't
                be checked. In case of a fatal errror during the read operation
                caused by the IO subsystem the error is logged in the knldiag 
                file and the return code is false.
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
        const IOMan_BlockCount  blockCount = 1 )
    {
        if( bFromPrimary )
            return( m_PrimaryLogVolume.ReadSimple( taskId, pFrame, blockNo, blockCount ));
        else
            return( m_SecondaryLogVolume.ReadSimple( taskId, pFrame, blockNo, blockCount ));
    }

    /*!
       @brief   This method is used to write one log page from given memory into
                a log device. Before the proper io is executed the correctness
                of the given page will be checked. Each error is loged in the 
                knldiag file and the io buffer is dumped. In any case of error 
                an emergency shutdown is executed.
       @param   taskId [in] Identification of the calling task.
       @param   page [in] Handler of the wanted Page.
       @param   block [in] Position of the wanted page within the device.
       @return  none
     */

    virtual void WritePage(
        const tsp00_TaskId          taskId,
        IOMan_IPage                &page,
        const IOMan_BlockAddress    &block );

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
        const SAPDB_UInt            startIndex = IOMan_IKernelPages::FirstIndex());

    /*!
       @brief   This method is used to write a page onto a device 
                The correctness  of the page won't be  checked. In case of a 
                fatal errror during the write operation caused by the IO subsystem
                the error is logged in the knldiag file and the return code is 
                false. 
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
        const SAPDB_Bool        bToPrimary )
    {
        if( bToPrimary )
            return( m_PrimaryLogVolume.WriteSimple( taskId, pFrame, blockNo ));
        else
            return( m_SecondaryLogVolume.WriteSimple( taskId, pFrame, blockNo ));
    }

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! @name Volume access                                                   */
    /*@{*/
    /* -----------------------------------------------------------------------*/

    /*!
       @brief   This method is used to get access to primary log volume.
       @return  none
     */

    virtual IOMan_LogVolume&  GetPrimaryVolume()
    {
        return m_PrimaryLogVolume;
    }

    virtual const IOMan_LogVolume&  GetPrimaryVolume() const
    {
        return m_PrimaryLogVolume;
    }

    /*!
       @brief   This method is used to get access to secondary log volume.
       @return  none
     */

    virtual IOMan_LogVolume&  GetSecondaryVolume()
    {
        return m_SecondaryLogVolume;
    }

    virtual const IOMan_LogVolume&  GetSecondaryVolume() const
    {
        return m_SecondaryLogVolume;
    }

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/

private:

    SAPDB_Bool IsWritable() const
    {
        if( ! m_PrimaryLogVolume.IsWritable())
            return SAPDB_FALSE;
        else
            return m_SecondaryLogVolume.IsWritable();
    }

    SAPDB_Bool WriteVectorSimple(
        const tsp00_TaskId      taskId,
        tsp00_PageAddr         *pFrame,
        const IOMan_BlockNo     blockNo,
        const IOMan_BlockCount  blockCount );

private:

    /*!
        @brief  This is the primary log volume of the log device.
     */

    IOMan_MirrorLogVolume   m_PrimaryLogVolume;

    /*!
        @brief  This is the secondary log volume of the log device.
     */

    IOMan_MirrorLogVolume   m_SecondaryLogVolume;
};


#endif  /* IOMAN_DUAL_LOG_DEVICE_HPP */
