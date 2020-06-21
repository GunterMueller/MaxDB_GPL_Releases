/*!
  @file           IOMan_DataVolume.hpp
  @author         TorstenS
  @ingroup        IOManagement
  @brief          This module is used to handle a data volume.
  @see            

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2000-2006 SAP AG

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



#ifndef IOMAN_DATA_VOLUME_HPP
#define IOMAN_DATA_VOLUME_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"   // PASCAL: SP_basic_constants_and_types
#include "vsp002.h"  // PASCAL: IO Types

#include "DataAccess/Data_PageFrame.hpp"
#include "IOManager/IOMan_Volume.hpp"
#include "IOManager/IOMan_KernelPages.hpp"
#include "IOManager/IOMan_BlockAddress.hpp"
#include "IOManager/IOMan_Types.hpp"
#include "IOManager/IOMan_IPage.hpp"
#include "KernelCommon/Kernel_Common.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"


class Msg_List;
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
  @class    IOMan_DataVolume
  @brief    This class is used to handle data volumes.

 */


class IOMan_DataVolume : public IOMan_Volume
{
public:

    /*! @name Constructors and initialization */
    /*@{*/

    /*!
    @brief  Default Constructor. This method is used to set the member data 
            to an initial state, in particular the type to data. Additional 
            the constructor of the derived IOMan_Volume class is called.
            Note that the logical devo won't be set.
    @return none

    */

    IOMan_DataVolume():IOMan_Volume( tsp2_dev_type::fromConst( sp2dt_data ))
    {}

    /*!
       @brief   This method is used to set the individual logical data volume
                identifier.
       @param   devNo [in] Logical volume identifier
       @param   frame [in] Frame for the IOManInfoPage of the data volume
       @return  none

     */

    void Initialize(
        const IOMan_DeviceNo    devNo,
        const Data_PageFrame    &frame )
    {
        IOMan_Volume::Initialize( devNo, frame );
    }

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! @name Administration */
    /*@{*/
    /* -----------------------------------------------------------------------*/

    /*!
       @brief   This method is used to bring a data volume from close mode into
                open mode. For more informations see IOMan_Volume::Open().
       @param   task[in] Identification of the calling task.
       @return  (IOMan_ReturnCode) IOMan_Okay means open was successful
     */
    IOMan_ReturnCode Open( RTETask_ITask&   task ){
        return( IOMan_Volume::Open( task.ID(), true, IOMAN_OPEN_FOR_WRITE ));
    }

    /*!
       @brief   This method is used to set the logical device number for 
                the next logical data volume to invalid. It is assumed that 
                the info page is accessable.
       @param   task [in] Identification of the calling task.
       @return  (bool) linkage was set to invalid successfully
    */
    bool SetNextLinkageToInvalid( RTETask_ITask&   task )
    {
        if( ! IOMan_Volume::IsWritable()){
            return false;
        }
        SAPDBTRACE_WRITELN( IOMan_Trace, 6, "Set next linkage of data volume "
                            << GetLogicalDevNo() << " to invalid "  );
        m_InfoPage.SetNextLogicalVolNoToInvalid();
        IOMan_Volume::WriteInfoPage( task.ID());
        return true;
    }

    /*!
       @brief   This method is used to set the logical device number for 
                the previous logical data volume to invalid. It is assumed that 
                the info page is accessable.
       @param   task [in] Identification of the calling task
       @return  (bool) linkage was set to invalid successfully
    */
    bool SetPrevLinkageToInvalid( RTETask_ITask&   task )
    {
        if( ! IOMan_Volume::IsWritable()){
            return false;
        }
        SAPDBTRACE_WRITELN( IOMan_Trace, 6, "Set previous linkage of volume "
                            << GetLogicalDevNo() << " to invalid "  );
        m_InfoPage.SetPrevLogicalVolNoToInvalid();
        IOMan_Volume::WriteInfoPage( task.ID());
        return true;
    }

    /*!
       @brief   This method is used to read the block address of the
                restart record from the info page. It is assumed that
                the data volume is online.
       @return  IOMan_BlockAddress
     */
    IOMan_BlockAddress GetRestartPageAddress() const
    {
        SAPDBERR_ASSERT_STATE( IsOnline());
        return m_InfoPage.GetRestartPageAddress();
    }

    /*!
       @brief   This method is used to write the block address of the
                restart record into the info page. It is assumed that
                the data volume is online.
       @param   task [in] Identification of the calling task
       @param   restartPage [in] block address of restart page        
       @return  IOMan_BlockAddress
     */
    void SetRestartPageAddress(
        RTETask_ITask&              task,
        const IOMan_BlockAddress&   restartPage )
    {
        SAPDBERR_ASSERT_STATE( IsOnline());
        m_InfoPage.SetRestartPageAddress( restartPage );
        WriteInfoPage( task.ID());
    }

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! @name Input/Output */
    /*@{*/
    /* -----------------------------------------------------------------------*/

    /*!
       @brief   This method is used to read one page from a volume into the
                given memory. The correctness  of the read page will be checked
                as far as three times if an error has occured. Each error is
                loged in the knldiag file and the io buffer is dumped. In this
                case the method provides a false as return code. In case of fatal
                errors of the IO subsystem an emergency shutdown is executed.
       @param   task  [in] Identification of the calling task.
       @param   page  [in] Handler of the wanted Page.
       @param   block [in] Position of the wanted page within the volume.
       @return  true means page succesful read; else false for bad data page
     */
    bool ReadPage(
        RTETask_ITask&              task,
        IOMan_IPage&                page,
        const IOMan_BlockAddress&   block )
    {
        SAPDBERR_ASSERT_STATE( block.GetDeviceNo() == m_LogicalDevNo );
        return IOMan_Volume::Read( task.ID(), page, block.GetBlockNo());
    }

    /*!
        @brief  This method is used to read a couple of data pages synchronous 
                from a data volume into the given contiguous memory shunk.   
                The correctness  of the read pages will be checked as far as 
                three times if an error has occured. Each error is loged in the 
                knldiag file and the io buffer is dumped. In this case the method 
                provides a false as return code. The io check will be executed
                only for those pages which are marked as Used in the given complex
                cluster address. Therefore it will be possible to read a huge number
                of pages with one io and subsequent to filter the interesting pages from 
                this io buffer. In case of fatal errors of the IO subsystem an emergency 
                shutdown is executed.
        @param  task          [in] Identification of the calling task
        @param  msgList      [out] msgList for error diagnostic
        @param  pages         [in] Handler of the requested Pages
        @param  startIndex    [in] index within given page colection, which defines the
                                   first page to be read
        @param  cluster       [in] Position of the requested pages within the volume
        @param  badPageIndex [out] index of a bad page within given page collection
        @return  true means page succesful read; else false for bad data page
    */
    bool ReadPages(
        RTETask_ITask&                      task,
        Msg_List&                           msgList,
        IOMan_IKernelPages&                 pages,
        const SAPDB_UInt                    startIndex,
        const IOMan_ComplexClusterAddress&  cluster,
        SAPDB_UInt&                         badPageIndex );

    /*!
    @brief  This method is used to read a couple of data pages asynchronous via
            a special io channel from a data volume into the given contiguous 
            memory shunk. The correctness  of the read pages will be checked as 
            far as three times if an error has occured. Each error is loged in the 
            kernel protocol file and the io buffer is dumped. The io check will be 
            executed only for those pages which are marked as Used in the given complex
            cluster address. Therefore it will be possible to read a huge number
            of pages with one io and subsequent to filter the interesting pages from 
            this io buffer.
    @param  task          [in] task 
    @param  msgList      [out] msgList for error diagnostic
    @param  asynIOHandle  [in] identification of io channel
    @param  pages         [in] Handler of the requested Pages
    @param  startIndex    [in] index within given page colection, which defines the
                               first page to be read
    @param  cluster       [in] Position of the requested pages within the volume
    @return  (AsynReadPagesRc)
            - arpRcOkay
            - arpRcOffline       (requested volume is offline)
            - arpRcIoCheckFailed (checksum mismatch and msgList is filled)
            - arpRcSystemError   (msgList is filled)
    */
    enum AsynReadPagesRc{
        arpRcOkay          = IOMan_Okay,
        arpRcOffline       = IOMan_VolumeOffline,
        arpRcIoCheckFailed = IOMan_BadDataPage,
        arpRcSystemError   = IOMan_SystemError
    };
    AsynReadPagesRc AsynReadPages(
        RTETask_ITask&                      task,
        Msg_List&                           msgList,
        const SAPDB_Int4                    asynIOHandle,
        IOMan_IKernelPages&                 pages,
        const SAPDB_UInt                    startIndex,
        const IOMan_ComplexClusterAddress&  cluster );

    /*!
    @brief  This method is used to write a couple of data pages from a contiguous 
            memory shunk asynchronous via a special io channel into a data volume.  
            The correctness  of the given pages will be checked. Each error is loged 
            in the kernel protocol file and the io buffer is dumped. The io check 
            will be executed only for those pages which are marked as Used in the 
            given complex cluster address. Therefore it will be possible to write 
            a huge number of pages with one io whereby some pages in the given io 
            buffer will not be checked.
    @param  task          [in] task 
    @param  msgList      [out] msgList for error diagnostic
    @param  asynIOHandle  [in] identification of io channel
    @param  pages         [in] Handler of the requested Pages
    @param  startIndex    [in] index within given page colection, which defines the
                               first page to be read
    @param  cluster       [in] Position of the requested pages within the volume
    @param  badPageIndex [out] index of a bad page within given page collection 
    @return  (AsynWritePagesRc)
            - awpRcOkay
            - awpRcOffline       (requested volume is offline)
            - awpRcIoCheckFailed (checksum mismatch and msgList is filled)
            - awpRcSystemError   (msgList is filled)
    */
    enum AsynWritePagesRc{
        awpRcOkay          = IOMan_Okay,
        awpRcOffline       = IOMan_VolumeOffline,
        awpRcIoCheckFailed = IOMan_BadDataPage,
        awpRcSystemError   = IOMan_SystemError
    };
    IOMan_DataVolume::AsynWritePagesRc
    AsynWritePages(
        RTETask_ITask&                      task,
        Msg_List&                           msgList,
        const SAPDB_Int4                    asynIOHandle,
        IOMan_IKernelPages&                 pages,
        const SAPDB_UInt                    startIndex,
        const IOMan_ComplexClusterAddress&  cluster,
        SAPDB_UInt&                         badPageIndex );

    /*!
       @brief   This method is used to write one page from given memory into
                a volume. Before the proper io is executed the correctness of  
                the given page will be checked. Each error is loged in the 
                knldiag file and the io buffer is dumped. In any case of error 
                an emergency shutdown is executed.
       @param   task  [in] Identification of the calling task.
       @param   page  [in] Handler of the wanted Page.
       @param   block [in] Position of the wanted page within the volume.
       @return  none
     */
    void WritePage(
        RTETask_ITask&              task,
        IOMan_IPage&                page,
        const IOMan_BlockAddress&   block )
    {
        SAPDBERR_ASSERT_STATE( block.GetDeviceNo() == m_LogicalDevNo );
        IOMan_Volume::Write( task.ID(), page, block.GetBlockNo());
    }

    /*!
       @brief   This method is used to write pages from given memory into
                a volume. Before the proper io is executed the correctness of  
                the given page will be checked. Each error is loged in the 
                knldiag file and the io buffer is dumped. In any case of error 
                an emergency shutdown is executed.
       @param   task    [in] Identification of the calling task.
       @param   pages   [in] Handler of the pages to be written
       @param   cluster [in] Destination address of the pages to be written
       @return  none
     */
    void WritePages(
        RTETask_ITask&              task,
        IOMan_IKernelPages&         pages,
        const IOMan_ClusterAddress& cluster,
        const SAPDB_UInt            startPageIndex = IOMan_IKernelPages::FirstIndex())
    {
        SAPDBERR_ASSERT_STATE( cluster.GetDeviceNo() == m_LogicalDevNo );
        IOMan_Volume::WriteVector( task.ID(), pages, cluster, startPageIndex );
    }

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/

private:

    //The assignment operator is not supported to avoid not allowed duplicates
    IOMan_DataVolume& operator=( const IOMan_DataVolume &volume );

    // copy constructor is needed for newarray operation during initialization
    //IOMan_DataVolume( const IOMan_DataVolume &volume );

private:

    bool ExecuteIOCheck(
        RTETask_ITask&                      task,
        Msg_List&                           msgList,
        IOMan_IKernelPages&                 pages,
        const SAPDB_UInt                    startIndex,
        const IOMan_ComplexClusterAddress&  cluster,
        SAPDB_UInt&                         badPageIndex,
        SAPDB_UInt&                         badBlockIndex );
};

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/

#endif  /* IOMAN_DATA_VOLUME_HPP */

