/*!
  @file     IOMan_IDataIO.hpp
  @author   TorstenS
  @ingroup  IOManagement
  @brief    Interface for read and write access on the data volumes
  @see            

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



#ifndef IOMAN_IDATAIO_HPP
#define IOMAN_IDATAIO_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "DataAccess/Data_Types.hpp"
#include "DataAccess/Data_PageNoList.hpp" // no forward for typedef possible
#include "IOManager/IOMan_Types.hpp"
#include "IOManager/IOMan_DataPages.hpp" // no forward for typedef possible

class Data_BasePage;
class IOMan_AsynIOHandle;
class IOMan_BlockAddress;
class IOMan_ClusterAddress;
class IOMan_ComplexClusterAddress;
class IOMan_PackedBlockAddress;
class IOMan_ClusteredDataPages;
class Kernel_RestartPage;
class RTETask_ITask;

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
   @interface   IOMan_IDataIO
   @brief       This interface is used for read and write access on the data
                volumes.
 */

class IOMan_IDataIO
{
public:

    /*!
       @brief   This method returns the access to the IOMan_IDataIO interface
                of component IOManager. It is assumed that the IOManager singleton 
                is available! No CreateInstance call will be done.
       @return  Access to the IOMan_ILogIO interface
     */

    static IOMan_IDataIO& GetInstance();

    /*!
       @brief   This method is used to read one data page synchronous from a data 
                volume into the given memory. The caller is blocked as long as
                the read io is not completed The position of the page will be determined
                by the converter. The correctness of the read data page will be
                checked. In case of errors detected by the check method of the 
                page the read io will be repeated. If the error is still present 
                after three read operations, the provided buffer is dumped into 
                the rundirectory of the kernel. Each failed read operation is 
                logged within the knldiag and knldiag error file. In case of 
                fatal errors of the IO subsystem an emergency shutdown is executed.
       @param   taskId [in] Identification of the calling task.
       @param   page   [in] Handler for the wanted page.
       @param   pageNo [in] Id of the wanted page
       @return  (IOMan_ReturnCode)
                IOMan_Okay,
                IOMan_NoConverterEntry,
                IOMan_PageNoOutOfRange,
                IOMan_BadDataPage
     */

    virtual IOMan_ReturnCode ReadDataPage(
        const tsp00_TaskId  taskId,
        Data_BasePage       &page,
        const Data_PageNo   &pageNo ) = 0;

    /*!
        @brief  This method is used to read a couple of data pages synchronous from 
                a data volume into the given contiguous memory shunk. The requested
                data pages have to be stored contiguously in a data volume! The block 
                addresses must be given by the caller and each item of the pageNoList 
                must match to an item with the same index in the given page set and 
                given cluster address.
                The caller is blocked as long as the read io is not completed. 
                The correctness of the read data pages will be checked. In case of 
                errors detected by the check method of the pages the read io will be 
                repeated. If the error is still present after three read operations, 
                the filled buffer is dumped into the rundirectory of the kernel. Each 
                failed read operation is logged within the knldiag and knldiag error 
                file. In case of fatal errors of the IO subsystem an emergency shutdown 
                is executed.
        @param  task           [in] Identification of the calling task
        @param  pageNoList     [in] collection of requests page IDs
        @param  pages          [in] collection of requested pages, containing the recovery mode
        @param  cluster        [in] cluster block address
        @param  startIndex     [in] specifies at with index of the given page collection the
                                    relevant i/o pages begin. This is interesting if the given 
                                    page collection contains  more pages than the given cluster 
                                    address specifies.        
        @param  badDataPageNo [out] in case of a bad data page the pageNo of the corrupted
                                    page is returned
        @return (IOMan_ReturnCode)
                IOMan_Okay,
                IOMan_BadDataPage
    */
    virtual IOMan_ReturnCode ReadDataPages(
        RTETask_ITask&                      task,
        const Data_PageNoList&              pageNoList,
        IOMan_ClusteredDataPages&           pages,
        const IOMan_ComplexClusterAddress&  cluster,
        const SAPDB_UInt                    startIndex,
        Data_PageNo&                        badDataPageNo ) = 0;

    /*!
       @brief   This method is used to read a couple of data pages synchronous from 
                a data volume into the given contiguous memory shunk by using a
                separate io thread insulated from the common kernel io threads.
                The caller is blocked as long as the read io is not completed. 
                The caller has to provide the memory which is needed to read the
                pages. The pages will be determined by this method and the backup 
                membership will be checked as well as the correctness concerning the
                io check. Additional an exetended concistency check could be execute
                which corresponds to the extended option of "CheckData".
                Clustered pages will be returned in one cluster inclusive the not used
                blocks in the cluster. These blocks will be set to 0 (at least the
                page type). Non clustered pages will be read in one or more io calls
                and possible gaps will be closed by moving pages from right to left so
                that the value 'blocksToBeBackuped' specifies how may pages are 
                backup relevant.
       @param   task                [in] Identification of the calling task.
       @param   msgList            [out] for error tracking purpose
       @param   asynIOHandle        [in] Handle for the asynchronous io
       @param   pages               [in] Handler of the pages to be read
       @param   blocksToBeBackuped [out] number of pages to be written into backup medium
       @param   bDoExtendedCheck    [in] do extended consistency check on read pages
       @param   bIsClustered       [out] pages belongs to the clustered area
       @return  true means read was successfull
     */
    virtual bool  ReadDataPagesForBackup(
        RTETask_ITask&              task,
        Msg_List&                   msgList,
        const IOMan_AsynIOHandle&   asynIOHandle,
        IOMan_DataPages&            pages,
        IOMan_BlockCount&           blocksToBeBackuped,
        bool                        bDoExtendedCheck,
        bool&                       bIsClustered ) = 0;

    /*!
       @brief   This method is used to read one data page synchronous from a data 
                volume into the given memory. The caller is blocked as long as
                the read io is not completed. The position of the wanted page is
                given as a block address and therefore not searched in the component
                converter is neccessary. The validity of the given block address 
                is check on an abstract level, i.e. logical device number and block
                number are in a valid range. The correctness of the read data page 
                will be checked. In case of errors detected by the check method of 
                the page the read io will be repeated. If the error is still present 
                after three read operations, the provided buffer is dumped into 
                the rundirectory of the kernel. Each failed read operation is 
                logged within the knldiag and knldiag error file. In case of 
                fatal errors of the IO subsystem an emergency shutdown is executed.
       @param   taskId [in] Identification of the calling task.
       @param   page   [in] Handler for the wanted page.
       @param   block  [in] Position of the wanted page at the data volume
       @return  true means page succesful read; else false. 
                In case of a bad page the error is set to e_bad_datapage.
     */

    virtual bool ReadDataPageDirect(
        const tsp00_TaskId          taskId,
        Data_BasePage               &page,
        const IOMan_BlockAddress    &block ) = 0;

    /*!
       @brief   This method is used to read the restart page synchronous from 
                a defined position of an data volume. The caller is blocked 
                as long as the read io  is not completed. The correctness of 
                the read restart page will be checked. In case of errors detected 
                by the check method of the page the read io will be repeated. 
                If the error is still present  after three read operations, 
                the provided buffer is dumped into the rundirectory of the kernel. 
                Each failed read operation is logged within the knldiag and 
                knldiag error file. In any case of errors an emergency shutdown 
                is executed.
       @param   taskId [in] Identification of the calling task.
       @param   page   [in] Handler for the wanted page.
       @return  none
     */

    virtual void ReadRestartPage(
        const tsp00_TaskId  taskId,
        Kernel_RestartPage  &page ) = 0;

    /*!
       @brief   This method is used to read the restart page synchronous from 
                a given position on a data volume. The caller is blocked 
                as long as the read io  is not completed. The correctness of 
                the read restart page will be checked. In case of errors detected 
                by the check method of the page the read io will be repeated. 
                If the error is still present  after three read operations, 
                the provided buffer is dumped into the rundirectory of the kernel. 
                Each failed read operation is logged within the knldiag and 
                knldiag error file. In any case of errors an emergency shutdown 
                is executed.
       @param   taskId [in] Identification of the calling task.
       @param   page   [in] Handler for the wanted page.
       @param   block  [in] new block address where the given restart page is stored.
       @return  none
     */

    virtual void ReadSnapShotRestartPage(
        const tsp00_TaskId          taskId,
        Kernel_RestartPage          &page,
        const IOMan_BlockAddress    &block ) = 0;

    /*!
       @brief   This method is used to write one data page on one data volume.
                For more details see WriteDataPages! The difference to the
                WriteDataPages call is, that the caller hasn't reserved a block 
                address a prior. Therefore in the following first the componente 
                converter specialy free block management has to contacted to get 
                a block address. Note that this will increase the time the data 
                page is locked in the data cache!
       @param   taskId [in] Identification of the calling task.
       @param   page   [in] Handler of the given pages
       @return  none
     */

    virtual void WriteDataPage(
        const tsp00_TaskId  taskId,
        Data_BasePage&      page ) = 0;

    /*!
       @brief   This method is used to write a number of data pages synchronous
                into one data volume. The caller is blocked as long as the write 
                io is not completed. The maximum number of writable data pages is
                limited by a database configuration parameter. Additional to the
                writeable data pages the caller has to provide a cluster address,
                which specifies the write destination on a data volume. Before 
                the proper write is executed the correctness of the given pages 
                will be checked. In case of an error the corrupted page is dumped 
                into the kernel rundirectory and an emergency shutdown is executed.
                In the other case the data page is prepared for the io, i.e. the 
                converter version is updated and the checksum is calculated. 
                Additional the corresponding converter entry is updated and the proper
                write operation is executed. Note that the writeable data pages not 
                necessarily stored in a contiguous memory block, because this method
                uses scatter/gather io. The caller has to insure that the given 
                cluster has sufficient capacity to store all given data pages within
                the data volume. Pages without an corresponding block address won't
                be written.
       @param   taskId     [in] Identification of the calling task.
       @param   pages      [in] Handler of the given pages
       @param   cluster    [in] Write destination of the data pages in the data volume.
                                The caller has to insure that the given block addresses 
                                are available.
       @param   startIndex [in] specifies at with index of the given page collection the
                                relevant i/o pages begin (default value zero). This
                                is interesting if the given page collection contains 
                                more pages than the given cluster address specifies.
       @return  none
     */

    virtual void WriteDataPages(
        const tsp00_TaskId          taskId,
        IOMan_DataPages&            pages,
        const IOMan_ClusterAddress& cluster,
        const SAPDB_UInt            startIndex = 0 ) = 0;

    /*!
      @brief   This method is used to write a contiguous memory shunk of data pages 
               during a restore via a separate io thread insulated from the common 
               kernel io threads. The caller is blocked as long as the read io is 
               not completed. The correctness of the pages will be check by the common
               io check. Additional an extended concistency check could be execute
               which corresponds to the extended option of "CheckData".
               Clustered pages will stored as one cluster in the data area. 
               Padding pages in the given clustered will be ignored during the io check.
               Padding pages at the end of an unclustered page collection will be 
               removed.
      @param   task               [in] Identification of the calling task.
      @param   msgList           [out] for error tracking purpose
      @param   asynIOHandle       [in] Handle for the asynchronous io
      @param   pages              [in] Handler of the pages to be read
      @param   bDoExtendedCheck   [in] do extended consistency check on read pages
      @param   bIsClustered       [in] pages belongs to the clustered area
      @return  true means write was successfull
    */
    virtual bool WriteDataPagesForRestore(
        RTETask_ITask&              task,
        Msg_List&                   msgList,
        const IOMan_AsynIOHandle&   asynIOHandle,
        IOMan_DataPages&            pages,
        const bool                  bDoExtendedCheck,
        const bool                  bIsClustered ) = 0;

    /*!
       @brief   This method is used to write the restart page synchronous onto 
                a fixed position of a data volume Before the proper write is 
                executed the correctness of the  given page will be checked. In 
                case of an error the corrupted page is dumped into the kernel 
                rundirectory and an emergency shutdown is executed. In the other 
                case the restart page is prepared for the io, for example the 
                checksum is calculated. In any case of errors an emergency 
                shutdown is executed.
       @param   taskId [in] Identification of the calling task.
       @param   page   [in] Handler of the given restart page
       @return  none
     */

    virtual void WriteRestartPage(
        const tsp00_TaskId  taskId,
        Kernel_RestartPage& page ) = 0;

    /*!
       @brief   This method is used to write the restart page synchronous onto 
                a new position of a data volume.  Before the proper write is 
                executed the correctness of the given page will be checked. In 
                any case of an error the corrupted page is dumped into the kernel 
                rundirectory and an emergency shutdown is executed. In the other 
                case the restart page is prepared for the io, for example the 
                checksum is calculated. 
       @param   taskId  [in] Identification of the calling task.
       @param   page    [in] Handler of the given restart page
       @param   block  [out] new block address where the given restart page is stored.
       @return  one
     */

    virtual void WriteSnapShotRestartPage(
        const tsp00_TaskId  taskId,
        Kernel_RestartPage  &page,
        IOMan_BlockAddress  &block ) = 0;

};

#endif  /* IOMAN_IDATAIO_HPP */
