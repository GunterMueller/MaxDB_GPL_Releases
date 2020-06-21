/*!
    @file           IOMan_Volume.hpp
    @author         TorstenS
    @ingroup        IOManagement
    @brief          This module is used to handle volumes.

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


#ifndef IOMAN_VOLUME_HPP
#define IOMAN_VOLUME_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"   // PASCAL: SP_basic_constants_and_types
#include "vsp002.h"  // PASCAL: IO Types

#include "IOManager/IOMan_ClusterAddress.hpp"
#include "IOManager/IOMan_Types.hpp"
#include "IOManager/IOMan_InfoPage.hpp"
#include "IOManager/IOMan_KernelPages.hpp"
#include "KernelCommon/Kernel_Common.hpp"
#include "DataAccess/Data_PageFrame.hpp"
#include "RunTime/InputOutput/RTEIO_VolumeInfo.hpp"

class IOMan_MessageList;
class Msg_List;
class RTETask_ITask;

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/// Defines default number of pages written by one io
#define IOMAN_WRITE_ONE_BLOCK       1

/// Defines if an error message should be written into the knldiag
#define IOMAN_SUPPRESS_ERR_MSG      SAPDB_TRUE

/// Defines that the volume will be open in a ReadWrite mode
#define IOMAN_OPEN_FOR_WRITE        SAPDB_TRUE

/// Defines that the capacity of the volume could be differ by one page
#define IOMAN_LAZY_CAPACITY_CHECK   SAPDB_TRUE


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
   @class   IOMan_Volume
   @brief   This class is used to handle volumes.
            It offers an amount of common (!) methods to manage any type 
            of volume. This class is only the base of the proper volume
            class implementations like data, log or trace volume. Therefore
            this class contains only a protected constructor to force the
            programer for example to derive a data volume class from this 
            one.
 */

class IOMan_Volume
{
public:

    /* -----------------------------------------------------------------------*/
    /*! @name Administration */
    /*@(*/
    /* -----------------------------------------------------------------------*/

    /*!
       @brief   This method is used to bring the volume from open mode into 
                close mode, presumed it's in open mode. The volume is detached.
       @param   taskId [in] Identification of the calling task.
       @return  none
     */

    void Close( const tsp00_TaskId taskId );

    /*!
       @brief   This method is used to create the IOMan_InfoPage. After
                creation the volume is still in offline mode.
       @param   taskId        [in] Identification of the calling task.
       @param   pDBIdentifier [in] Unique data base identifier
       @return  (IOMan_ReturnCode) IOMan_Okay means create was successful
     */

    virtual IOMan_ReturnCode CreateInfoPage(
        const tsp00_TaskId   taskId,
        const SAPDB_Byte     *pDBIdentifier );

    /*!
    @brief   This method is used to create and format a volume.
            If the creation is finished successful the method provides true 
            else false. It is assumed and tested by an assertion that the 
            volume is in offline mode.
    @param   taskId        [in] Identification of the calling task.
    @param   pDBIdentifier [in] Unique data base identifier
    @return  (IOMan_ReturnCode) IOMan_Okay means create was successful
    */


    IOMan_ReturnCode Create(
        const tsp00_TaskId  taskId,
        const SAPDB_Byte    *pDBIdentifier );

    /*!
       @brief   This method is used to inform the RTE about the presence of another
                new volume. It's required and checked whether the given volume 
                is in mode offline. Furhtermore the added volume is in mode offline.
       @param   devName [in] name of the volume to be added
       @param   devSize [in] capacity of the volume to be added
       @return  true if add was successful
     */

    SAPDB_Bool Add(
        tsp00_VFilename         &devName,
        const IOMan_BlockCount  devSize ) const;

    /*!
       @brief   This method is used to inform the RTE about the removing of 
                the volume. It's required and checked whether the given 
                volume is in mode offline.
       @return  none
     */

    void Del() const;

    /*!
      @brief    SetNextLinkage()
                This method is used to write the logical device number of the
                next logical volume into the info page of the current volume.
      @param    taskId     [in] Identification of the calling task.
      @param    nextVolume [in] Volume handler
      @return   (SAPDB_Bool) linkage is created successfully
    */

    SAPDB_Bool SetNextLinkage(
        const tsp00_TaskId  taskId,
        const IOMan_Volume  &nextVolume);

    /*!
       @brief   This method is used to write the logical device number of 
                the previous logical volume into the info page of the
                current volume.
       @param   taskId     [in] Identification of the calling task.
       @param   prevVolume [in] Volume handler
       @return  (SAPDB_Bool) linkage is created successfully
    */

    SAPDB_Bool SetPrevLinkage(
        const tsp00_TaskId  taskId,
        const IOMan_Volume  &prevVolume );

    /*!
       @brief   This method is used to validate the linkage between the currrent
                and the given volume.
       @param   taskId [in] Identification of the calling task.
       @param   volume [in] volume handler
       @return  (SAPDB_Bool) true if linkage is valid; else false
    */

    SAPDB_Bool IsLinkageValid(
        const tsp00_TaskId  taskId,
        const IOMan_Volume  &volume ) const;

    /*!
       @brief   This method is used to validate if a logical predecessor 
                volume is definded.
       @return  (SAPDB_Bool) true if predecessor volume is definded
    */

    SAPDB_Bool IsPrevLinkageValid() const;

    /*!
       @brief   This method is used to validate if a logical successor 
                volume is definded.
       @return  (SAPDB_Bool) true if successor volume is definded
     */

    SAPDB_Bool IsNextLinkageValid() const;

    /*!
       @brief   This method is used to compare the DBIdentifier of the 
                currrent and the given volume. If the identifiers are equal 
                true is returned else false.
       @param   taskId [in] Identification of the calling task.
       @param   volume [in] Volume handler
       @return  (SAPDB_Bool) true if identifiers are equal; else false
    */

    SAPDB_Bool IsDBIdentifierValid(
        const tsp00_TaskId  taskId,
        const IOMan_Volume  &volume ) const
    {
        if(( ! IsOnline() ) || (! volume.IsOnline()))
            return false;

        return IsDBIdentifierValid( taskId, volume.GetDBIdentifier() );
    }

    /*!
       @brief   This method is used to compare the given DBIdentifier with 
                the DBIdentifier of the current volume. If the identifiers 
                are equal true is returned else false.
       @param   taskId [in] Identification of the calling task.
       @param   pDBIdentifier [in] DBIdentifier
       @return  (SAPDB_Bool) true if identifiers are equal; else false
     */

    SAPDB_Bool IsDBIdentifierValid(
        const tsp00_TaskId  taskId,
        const SAPDB_Byte    *pDBIdentifier ) const;

    /*!
       @brief   This method is used to remove the bad marker from the volume.
                A following open call won't break down caused by the bad flag.
       @param   taskId [in] Identification of the calling task.
       @return  (IOMan_ReturnCode) IOMan_Okay means unset was successful
     */

    IOMan_ReturnCode UnSetBad( const tsp00_TaskId   taskId );

    /*!
       @brief   This method is used to adjust the capacity value stored
                in the IOManInfoPage to the migrated value of the 
                configuration. After opening and updating the capacity 
                the volume is closed.
       @param   taskId      [in] Identification of the calling task.
       @param   msgList [in|out] messageList
       @return  (IOMan_ReturnCode) 
     */

    IOMan_ReturnCode MigrateCapacity(
        const tsp00_TaskId  taskId,
        IOMan_MessageList   &msgList );

    /* -----------------------------------------------------------------------*/
    /*@)*/
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! @name Common information */
    /*@(*/
    /* -----------------------------------------------------------------------*/

    /*!
       @brief   This method is used to get the size in bytes of one block
                managed by volumes. Note that all blocks within the same 
                volume have the same size.
       @return  Size of one block of the current volume in bytes
     */

    SAPDB_UInt4 GetBlockSize() const
    {
        return m_BlockSize;
    }

    /*!
       @brief   This method is used to get the configured volume capacity in
                blocks. This capacity is equal to corresponding configuration
                file entry and describes the physical size of the volume.
       @return  IOMan_BlockCount
     */

    IOMan_BlockCount GetConfiguredSize() const
    {
        if( 0 < m_Capacity )
            return( m_Capacity );

        IOMan_BlockCount    capacity;
        SAPDB_UInt4         blockSize;
        RTE_VolumeAccessMode accessMode;

        if (GetConfiguration( capacity, blockSize, accessMode ))
        {
            return capacity;
        }
        else;
        {
            return 0;
        }
    }

    /*!
       @brief   This method is used to get the internal volume capacity in
                blocks, i.e. this is the configured size of the volume
                reduce by the RTE INFO PAGE.
       @return  IOMan_BlockCount
     */

    IOMan_BlockCount GetInternalSize() const
    {
        return( GetConfiguredSize() -1 );
    }

    /*!
       @brief   This method is used to get the usable volume capacity in
                blocks, i.e. this is the configured size of the volume
                reduce by the RTE INFO PAGE and the IOMAN INFO PAGE.
       @return  IOMan_BlockCount
     */

    IOMan_BlockCount GetUsableSize() const
    {
        return( GetConfiguredSize() -2 );
    }

    /*!
       @brief   This method is used to get the access mode of the volume. 
                Common volume modes are 'normal' and 'sequential'. The
                sequential mode is used only within an archive instance.
                A sequential volume will be writte top/down and it's
                not formated.
       @return  RTE_VolumeAccessMode
     */

    RTE_VolumeAccessMode GetAccessMode() const
    {
        return m_Mode;
    }

    /*!
       @brief   This method is used to get the unique identifier of the 
                volume.
       @return  (const SAPDB_Byte*) pointer to (not null terminated) string
     */

    const SAPDB_Byte* GetDBIdentifier() const
    {
        SAPDBERR_ASSERT_STATE( IsOnline() );

        return m_InfoPage.GetDBIdentifier();
    }

    /*!
       @brief   This method is used to get the logical identifier of the
                volume. All volumes of the same volume type are counted 
                begining by ONE(!).
       @return  logical volume id
     */

    IOMan_DeviceNo  GetLogicalDevNo() const
    {
        return m_LogicalDevNo;
    }

    /*!
       @brief   This method is used to read the logical device number of
                the volume which is the next in logical order. This number
                must be equal to the logical next volume within the data 
                base configuration file.
       @return  IOMan_DeviceNo
     */

    IOMan_DeviceNo GetNextLogicalDevNo() const
    {
        SAPDBERR_ASSERT_STATE( IsOnline() );

        return m_InfoPage.GetNextLogicalDevNo();
    }

    /*!
       @brief   This method is used to read the logical device number of
                the volume which is the predecessor in logical order. This 
                number must be equal to the logical predecessor volume within 
                the data base configuration file.
       @return  IOMan_DeviceNo
     */

    IOMan_DeviceNo GetPrevLogicalDevNo() const
    {
        SAPDBERR_ASSERT_STATE( IsOnline() );

        return m_InfoPage.GetPrevLogicalDevNo();
    }

    /*!
       @brief   This method is used to get the type of the current volume.
       @return  tsp2_dev_type
    */

    tsp2_dev_type GetType() const
    {
        return m_Type;
    }

    /*!
       @brief   This method is used to get the configured name of the volume.
                If no valid volume name is found false is returned and the
                incoming volume name is not touched. Else the configured 
                volume name is stored within the incoming parameter. Is is 
                assumed and tested by an assertion that the volume is in open 
                mode.
       @param   devName [out] volume name
       @return  true means success; else false
     */

    SAPDB_Bool GetName( tsp00_VFilename &devName ) const;

    /*!
       @brief   Same conditions as the method above but in this case the
                returned volume name is terminted by c termination code
       @param   devName [out] volume name
       @return  true means success; else false
     */

    SAPDB_Bool GetNameWithTermination( tsp00_VFilename &devName ) const;

    /*!
       @brief   This method is used to get the type in form of a string
                for example data or log of the current volume.
       @return  volume type
    */

    SAPDB_Char* GetTypeName() const
    {
        switch( m_Type )
        {
        case sp2dt_log:
            return ( SAPDB_Char* )"log";
        case sp2dt_mirr_log:
            return ( SAPDB_Char* )"mlog";
        case sp2dt_data:
            return ( SAPDB_Char* )"data";
        case sp2dt_trace:
            return ( SAPDB_Char* )"trace";
        default:
            return ( SAPDB_Char* )"UNDEF";
        }
    }

    /*!
       @brief   This method is used to get the current access permission
       @return  SAPDB_Char* with null termination
     */

    SAPDB_Char* GetStateName() const
    {
        switch( m_State )
        {
        case IOMan_Offline:
            return ( SAPDB_Char* )"offline";
        case IOMan_Read:
            return ( SAPDB_Char* )"read";
        case IOMan_ReadWrite:
            return ( SAPDB_Char* )"readWrite";
        default:
            return ( SAPDB_Char* )"UNDEF";
        }
    }

    /*!
       @brief   This method is used to check whether the current volume 
                exists, i.e. is stored in the data base configuration file.
       @return  true means is known as a valid volume; else false
     */

    SAPDB_Bool IsConfigured() const;

    /*!
       @brief   This method is used to determine whether the volume is in open
                mode. It's not said whether the volume is in state IOMan_Read 
                or IOMan_ReadWrite state.
       @return  true means the state is IOMan_Read or IOMan_ReadWrite
     */

    SAPDB_Bool IsOnline() const
    {
        return( (IOMan_ReadWrite == m_State ) || ( IOMan_Read == m_State ));
    }


    /*!
       @brief   This method is used to determine whether the volume is in
                open mode and writable.
       @return  true means is writable
     */

    SAPDB_Bool IsWritable() const
    {
        return( IOMan_ReadWrite == m_State );
    }

    /* -----------------------------------------------------------------------*/
    /*@)*/
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! @name Input/Output */
    /*@(*/
    /* -----------------------------------------------------------------------*/

    ///  Gives the number of retry attemps if an read call is failed
    static SAPDB_UInt MaxReadRetries(){
        return 3;
    }
    /// Defines the sleep time in seconds, before a read retry is executed
    static SAPDB_UInt RetrySleepTime(){
        return 2;
    }
    /*!
       @brief   This method encapsulate the ReadSimple method. Additional it
                offers the possibility to read a page three times if the check
                method of the page detect an error. Each error is loged in the 
                knldiag file and the io buffer is dumped. In this case the 
                method provides a false as return code. In case of fatal
                errors of the IO subsystem an emergency shutdown is executed!!
       @param   taskId [in] Identification of the calling task.
       @param   page [in] Handler of the wanted Page.
       @param   blockNo [in] Position of the wanted page within the volume.
       @return  true if the read operation is successful; else false.
     */

    SAPDB_Bool Read(
        const tsp00_TaskId  taskId,
        IOMan_IPage        &page,
        const IOMan_BlockNo blockNo );

    /*!
       @brief   This method is used to read a page (default) from a volume 
                into the given memory. The correctness  of the read page won't
                be checked. In case of a fatal errror during the read operation
                caused by the IO subsystem the error is logged in the knldiag 
                file and the return code is false.
       @param   taskId [in] Identification of the calling task.
       @param   pFrame [in] Memory where the read page will be stored.
       @param   blockNo [in] Position of the wanted page within the volume.
       @param   blockCount [in] Number of pages to be read in one step
                                Default is one page
       @return  true if the read operation is successful; else false.
     */

    SAPDB_Bool ReadSimple(
        const tsp00_TaskId      taskId,
        const tsp00_PageAddr    pFrame,
        const IOMan_BlockNo     blockNo,
        const IOMan_BlockCount  blockCount = 1 );

    /*!
       @brief   This method encapsulate the WriteSimple method. Additional it
                offers the possibility to check the page before it is written 
                into the volume. If during the check an error occurs an 
                emergency shutdown is executed. For more informations see 
                WriteSimple.
       @param   taskId [in] Identification of the calling task.
       @param   page [in] Handler for the page to be written
       @param   blockNo [in] Position for the page within the volume.
       @return  none
     */

    void Write(
        const tsp00_TaskId  taskId,
        IOMan_IPage        &page,
        const IOMan_BlockNo blockNo );

    /*!
       @brief   This method is used to write a page onto a volume 
                The correctness  of the page won't be  checked. In case of a 
                fatal errror during the write operation caused by the IO subsystem
                the error is logged in the knldiag file and the return code is false.
       @param   taskId  [in] Identification of the calling task.
       @param   pFrame  [in] Memory of the writeable pages.
       @param   blockNo [in] Position for the page within the volume.
       @return  true if the write operation is successful; else false.
     */

    SAPDB_Bool WriteSimple(
        const tsp00_TaskId      taskId,
        const tsp00_PageAddr    pFrame,
        const IOMan_BlockNo     blockNo );

    /*!
       @brief   This method is used to write a number of pages onto a volume 
                Additional it offers the possibility to check the page before 
                it is written into the volume. In case of a fatal errror 
                during the write operation caused by the IO subsystem the 
                error is logged in the knldiag file and the return code is false.
       @param   taskId [in] Identification of the calling task.
       @param   pages [in] Handler of the pages to be written
       @param   cluster [in] Destination address of the pages to be written
       @param   startIndex [in] Specifies the first page in the page collection
                                which is relevant for the following I/O. Default
                                the startIndex is zero. Only in case of write                               
                                orders splited over more than one volume                               
                                the startIndex is greater than zero.
       @return  none
     */

    void WriteVector(
        const tsp00_TaskId          taskId,
        IOMan_IKernelPages          &pages,
        const IOMan_ClusterAddress  &cluster,
        const SAPDB_UInt            startIndex = IOMan_IKernelPages::FirstIndex());

    /*!
       @brief   This method is used to read or write via a separate io thread
                insulated from the common kernel io threads. The correctness  
                of the page won't be  checked.
       @param   task         [in] Identification of the calling task.
       @param   msgList     [out] msgList for error diagnostic
       @param   asynIOHandle [in] Handle for the asynchronous io
       @param   pages        [in] Handler of the pages to be written
       @param   startIndex   [in] specifies the first page in the given page collection
       @param   cluster      [in] Destination address of the pages to be written
       @return  (AsynReadWriteRc)
                - arwRcOkay
                - arwRcOffline      (requested volume is offline)
                - arwRcSystemError  (msgList is filled)
     */
    enum AsynReadWriteRc{
        arwRcOkay        = IOMan_Okay,
        arwRcOffline     = IOMan_VolumeOffline,
        arwRcSystemError = IOMan_SystemError
    };
    AsynReadWriteRc AsynReadWrite(
        RTETask_ITask&              task,
        Msg_List&                   msgList,
        const SAPDB_Int4            asynIOHandle,
        IOMan_IKernelPages          &pages,
        const SAPDB_UInt            startIndex,
        const IOMan_ClusterAddress  &cluster );

    /* -----------------------------------------------------------------------*/
    /*@)*/
    /* -----------------------------------------------------------------------*/

protected:

    /* -----------------------------------------------------------------------*/
    /*! @name Constructors and initialization */
    /*@(*/
    /* -----------------------------------------------------------------------*/

    /*!
       @brief   Constructor. This method is used to set the member data to an 
                initial state. The volume type and the logical identifier is 
                set to the given value and at this point non changeable. The 
                methods Create and Open are used to activate the volume. This 
                contructor is protected to force the programer to use the derived 
                data or log volume class constructor based on this one.
       @param   volumeType [in] Type of the volume
       @return  none
     */

    IOMan_Volume( const tsp2_dev_type &volumeType )
    :
    m_State( IOMan_Offline ),
    m_Type( volumeType ),
    m_Mode( RTE_VolumeAccessModeUnknown ),
    m_Capacity( 0 ),
    m_LogicalDevNo( 0 ),
    m_PhysicalDevNo( 0 ),
    m_BlockSize( 0 ),
    m_InfoPage()
    {}

    /*!
       @brief   This method is used to set the individual logical volume
                identifier.
       @param   devNo [in] Logical volume identifier
       @param   frame [in] Frame for the IOManInfoPage of the volume
       @return  none
     */

    void Initialize(
        const IOMan_DeviceNo            devNo,
        const Data_PageFrame   &frame )
    {
        m_LogicalDevNo = devNo;
        m_InfoPage.SetFrame( frame );
    }

    /* -----------------------------------------------------------------------*/
    /*@)*/
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! @name Administration */
    /*@(*/

    /*!
       @brief   This method is used to bring the volume from close mode into
                open mode, presumed it isn't in open mode already. For security
                reasons the IOManInfoPage is checked against the configuration.
       @param   taskId          [in] Identification of the calling task.
       @param   bCheckBadFlag   [in] check whether the volume is set to bad and
                                     therefore an open is not allowed.
       @param   bOpenForWrite   [in] open volume for read and write access
       @return  (IOMan_ReturnCode) IOMan_Okay means open was successful
     */

    IOMan_ReturnCode Open(
        const tsp00_TaskId  taskId,
        const SAPDB_Bool    bCheckBadFlag,
        const SAPDB_Bool    bOpenForWrite );

    /*!
       @brief   This method is used to bring the volume from close mode into
                open mode, presumed it isn't in open mode already. Note that
                no security check is made and the IOMan_InfoPage is NOT read!
       @param   taskId          [in] Identification of the calling task.
       @param   bSuppressErrMsg [in] no message into knldiag if attach failed
       @param   bOpenForWrite   [in] open volume for read and write access
       @return  (IOMan_ReturnCode) IOMan_Okay means open was successful
     */

    IOMan_ReturnCode OpenSimple(
        const tsp00_TaskId taskId,
        const SAPDB_Bool   bSuppressErrMsg,
        const SAPDB_Bool   bOpenForWrite );

    /*!
       @brief   This method is used to mark an open volume as bad. If the 
                given block number is valid it is written into the zero page 
                of the volume. Note that the method won't close the volume. 
                Each following open of the volume will cause an error.
       @param   taskId [in] Identification of the calling task.
       @param   blockNo [in] BlockNo to be inserted into the zero page.
       @return  none
     */

    void SetBad(
        const tsp00_TaskId  taskId,
        const IOMan_BlockNo blockNo );

    /* -----------------------------------------------------------------------*/
    /*@)*/
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! @name Input/Output */
    /*@(*/
    /* -----------------------------------------------------------------------*/

    /*!
        @brief  WriteInfoPage()  
                This method is used to write the IOMan_InfoPage to the 
                predefined position of within the current volume. In case
                of an error the system aborts.
        @param  taskId [in] Identification of the calling task.
        @return none
    */

    void WriteInfoPage( const tsp00_TaskId  taskId );

    /* -----------------------------------------------------------------------*/
    /*@)*/
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! @name Common information */
    /*@(*/
    /* -----------------------------------------------------------------------*/

    /*!
       @brief   This method is used to get the physical identifier of the
                volume. This is needed by the RTE Interface to identify a 
                volume.
       @return  physical volume id
     */

    SAPDB_Int4 GetPhysicalDevNo() const
    {
        return m_PhysicalDevNo;
    }

    /*!
       @brief   This method is used to determine if the volume is set to bad.
       @return  true means is bad; else false
     */

    SAPDB_Bool IsBad() const
    {
        return 0 != m_InfoPage.GetBadBlockNo();
    }

    /*!
       @brief   This method is used to determine whether a given block number
                is addressable via the volume. i.e. the block number is in
                the valid number range of the volume.
       @param   blockNo [in] BlockNo to be examined
       @return  true means is in volume scope; else false
     */

    SAPDB_Bool IsBlockNoMapable( const IOMan_BlockNo blockNo ) const
    {
        return ( 0 <= blockNo ) && ( blockNo < m_Capacity );
    }

    /*!
       @brief   This method is used to check the correctness of the 
                IOMan_InfoPage in relation the the configuration file.
       @param   bDoLazyCapacityCheck [in] if true the capacity of the volume
                                          and of the configuration could differ
                                          by one page; else it has to be equal.
       @param   msgList          [in|out] messageList
       @return  true if no error is detected; else false
     */

    SAPDB_Bool IsVolumeConfigurationValid(
        const SAPDB_Bool    bDoLazyCapacityCheck,
        IOMan_MessageList   &msgList ) const;

    /* -----------------------------------------------------------------------*/
    /*@)*/
    /* -----------------------------------------------------------------------*/

private:

    /*--------------------------------------------------------------------------
    @brief  This method is used to format the current volume.
    @param  taskId      [in] Identification of the calling task.
    @param  msgList [in|out] messageList
    @return IOMan_ReturnCode
    --------------------------------------------------------------------------*/

    IOMan_ReturnCode Format(
        const tsp00_TaskId   taskId,
        IOMan_MessageList    &msgList );

    /*--------------------------------------------------------------------------
    @brief  This method is used to attach the current volume.
    @param  taskId          [in] Identification of the calling task.
    @param  bSuppressErrMsg [in] if set to true no error message
                                 is written into knldiag in case
                                 of an open error. (Default is false)
    @return true means attach was successful; else false
    --------------------------------------------------------------------------*/

    SAPDB_Bool Attach(
        const tsp00_TaskId taskId,
        const SAPDB_Bool   bSuppressErrMsg = false );

    /*--------------------------------------------------------------------------
    @brief  his method is used to get the configuration information
            of the current volume from the data base configuration 
            file. If no configuration is available the return value 
            is false.
    @param  capacity  [out] volume size in pages
    @param  blockSize [out] Page size 
    @return true means is known as a valid volume; else false
    --------------------------------------------------------------------------*/

    SAPDB_Bool GetConfiguration(
        IOMan_BlockCount    &capacity,
        SAPDB_UInt4         &blockSize,
        RTE_VolumeAccessMode &accessMode) const;

    /*--------------------------------------------------------------------------
    @brief  This method is used to write a number of pages onto a volume 
            The correctness  of the page won't be  checked. In case of a 
            fatal errror during the write operation caused by the IO subsystem
            the error is logged in the knldiag file and the return code is 
            false. The volume is marked as bad.
    @param  taskId     [in] Identification of the calling task.
    @param  pFrame     [in] Memory of the writeable pages.
    @param  blockNo    [in] Position for the first page within the volume.
    @param  blockCount [in] Number of pages to write
    @return true if the write operation is successful; else false.
    --------------------------------------------------------------------------*/

    SAPDB_Bool WriteVectorSimple(
        const tsp00_TaskId      taskId,
        tsp00_PageAddr          *pFrame,
        const IOMan_BlockNo     blockNo,
        const IOMan_BlockCount  blockCount );

private:

    // Assignment operator is not supported

    IOMan_Volume& operator=( const IOMan_Volume &volume );

    // copy constructor is needed for newarray operation during initialization
    //IOMan_Volume( const IOMan_Volume &volume );

private:

    /* -----------------------------------------------------------------------*/
    /*!
        @brief  Contains the states of a volume, See m_State
     */
    /* -----------------------------------------------------------------------*/

    typedef enum
    {
        IOMan_Offline = 0,
        IOMan_Read,
        IOMan_ReadWrite
    } IOMan_VolumeState;

protected:

    /*!
        @brief  Indicates whether the volume is in open mode. An open volume
                could be in state IOMan_Read or IOMan_ReadWrite, whereby the
                last one is the most common state. The IOMan_Read one is used
                for the slave instance in a HotStandBy solution.
     */

    IOMan_VolumeState   m_State;

    /*!
        @brief  Determines the type of the volume. Common volume types are 
                data and log.
     */

    const tsp2_dev_type m_Type;

    /*!
        @brief  Determines the access mode of the volume. Common volume modes are 
                'normal' and 'sequential'.
     */

    RTE_VolumeAccessMode m_Mode;


    /*!
        @brief  Is the maximum number of blocks storable in the volume. The 
                block size is defined by the member data m_BlockSize.
     */

    IOMan_BlockCount m_Capacity;

    /*!
        @brief  Is the logical identifier for an volume. The m_LogicalDevNo is
                a 'virtual' identifier for a volume. All volumes of the same
                volume type are counted begining by ONE(!).
     */

    IOMan_DeviceNo  m_LogicalDevNo;

    /*!
        @brief  Is the physical identifier for an volume. The m_PhysicalDevNo is
                needed by the RTE Interface to identify a volume.
     */

    SAPDB_Int4  m_PhysicalDevNo;

    /*!
        @brief  Is the size in bytes of one block within the volume. The default
                configured block size is 8192 bytes.
     */

    SAPDB_UInt4 m_BlockSize;

    /*!
        @brief  Is a page handler for the IOManInfoPage which is stored at the
                begining of each volume. It contains administrative informations
                hich has to be check at each restart of the io management.
     */

    IOMan_InfoPage  m_InfoPage;
};


#endif  /* IOMAN_VOLUME_HPP */

