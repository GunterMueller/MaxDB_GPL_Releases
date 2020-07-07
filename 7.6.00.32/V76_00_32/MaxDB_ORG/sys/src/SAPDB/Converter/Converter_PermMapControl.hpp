/*!
  @file     Converter_PermMapControl.hpp
  @author   TorstenS
  @ingroup  Converter
  @brief    Manages the concurrent access to the structure mapping
            permanent dynamic pageNo's to their physical block address
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


#ifndef CONVERTER_PERM_MAPCONTROL_HPP
#define CONVERTER_PERM_MAPCONTROL_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 **/


#include "Converter/Converter_MapControl.hpp"
#include "Converter/Converter_UsedPageNoContainer.hpp"
#include "Converter/Converter_PageNoContainer.hpp"
#include "Converter/Converter_Map.hpp"
#include "Converter/Converter_PermMapSectionData.hpp"
#include "Converter/Converter_ArchiveContainer.hpp"

#include "Container/Container_Vector.hpp"

#include "KernelCommon/Kernel_Common.hpp"
#include "KernelCommon/Kernel_SynchronizedTaskWaitList.hpp"

#include "RunTime/Synchronisation/RTESync_InterlockedCounter.hpp"



/*===========================================================================*
 *  FORWARD DECLARATION                                                      *
 **/

class Converter_BitMapPage;
class Converter_PermMapSection;
class Converter_LeafPage;
class Converter_Index;
class Converter_Pager;
class Converter_Version;
class IOMan_BlockAddress;
class Data_PageRecoveryMode;
class Kernel_Dump;
class SAPDBMem_IRawAllocator;
class RTETask_ITask;

/*===========================================================================*
 *  DEFINES                                                                  *
 **/

/*!
  @class    Converter_PermMapControl
  @brief    Administration object to handle concurrent access to the converter
            map, which deals with permanent dynamic pages.
 */

class Converter_PermMapControl : public Converter_MapControl
{
public:

    /* -----------------------------------------------------------------------*/
    /*! @name Construction                                                    */
    /* -----------------------------------------------------------------------*/

    /*!
       @brief   Constructor
       @param   allocator [in|out] allocator for memory resources
       @param   recMode       [in] recovery mode of the map control
     */

    Converter_PermMapControl(
        SAPDBMem_IRawAllocator        &allocator,
        const Data_PageRecoveryMode   &recMode )
    :
    Converter_MapControl( allocator, recMode ),
    m_UsedPageNoList( allocator, recMode.GetAddressingMode(), 10 ),
    m_PageNoList( allocator ),
    m_SectionPermData( allocator ),
    m_ArchiveConverter( allocator, 10 ),
    m_NumChangedConverterPages( 0 )
    {}

    /* -----------------------------------------------------------------------*/
    /*! @name Manager                                                         */
    /* -----------------------------------------------------------------------*/
    /*!
       @brief   Sets all members of the map control according to the input parameters
       @param   task                [in] own task
       @param   maxDataPnoInMap     [in] maximum page number to manage by the converter
       @param   baseRegionId        [in] start region id of for the first section
       @param   numConverterRegions [in] the converter is devided into  numConverterRegions
                                         'little' converter each protected by an own region
                                         with BaseRegionId &lt;=RegionId&lt;BaseRegionId+
                                         NumConverterRegions. The more regions are supplied
                                         the better parallel accesses can be handled
       @param   clusterSize         [in] number of subsequent map entries building a lock set
       @param   converterPageSize   [in] total size of a converter page in byte
       @param   bLastSaveDataSuccessful [in] last save data finished successfully
       @return  (SAPDB_Bool) returns true if the initilization was successful
     */

    virtual SAPDB_Bool  Initialize(
        const RTETask_ITask  &task,
        const SAPDB_UInt     maxDataPnoInMap,
        const RTE_RegionId   &baseRegionId,
        const SAPDB_UInt     numConverterRegions,
        const SAPDB_UInt     clusterSize,
        const SAPDB_UInt     converterPageSize,
        const SAPDB_Bool     bLastSaveDataSuccessful );

    /*!
       @brief   All resources held by members of the map control are returned.
       @param   task [in] own task
       @return  none
    */

    virtual void  Shutdown( const RTETask_ITask  &task);

    /*!
       @brief   Integrate a LeafPage into the Map, i.e. all reserved entries are reset
                to free all chains connecting pages as well as page entries are updated,
                statistic data are refreshed too
       @param   task              [in] own task
       @param   leafPage          [in] Page to integrate into the map
       @param   numPermPagesUsed [out] Number of used entries (no reserved) on this page
       @return  none
     */

    void IntegratePage(
        const RTETask_ITask &task,
        Converter_LeafPage  &leafPage,
        SAPDB_UInt         &numPermPagesUsed );

    /*!
       @brief   Remove empty LeavePages from Converter_Index
       @param   task            [in] own task
       @param   snapShotVersion [in] converter version of the snapshot
       @param   index           [in] Converter_Index
       @return  none
     */

    void RemoveEmptyLeaves(
        const RTETask_ITask     &task,
        const Converter_Version &snapShotVersion,
        Converter_Index         &index );

    /*!
       @brief   Writes all changed converter leaf pages referenced by the map to the data area.
                For each  converter leaf page written to a data volume the corresponding
                i/o address in the parent index is updated.
       @param   task   [in] own task
       @param   index  [in] each time a page is written to a device its new block address has
                            to be updated in the index of all converter leaf pages
       @param   pager  [in] handler used for converter leaf page i/o
       @return  (Data_PageNo) maximum data page number referenced by the map
     */

    Data_PageNo  Flush(
        const RTETask_ITask &task,
        Converter_Index     &index,
        Converter_Pager     &pager );

    /*!
       @brief   Writes all changed converter leaf pages in one section referenced by the map
                to a data device. For each  converter leaf page written to a device the
                corresponding i/o address in the parent index is updated.
       @param   task       [in]  own task
       @param   index      [in]  each time a page is written to a device its new block address has
                                 to be updated in the index of all converter leaf pages
       @param   pager      [in]  handler used for converter leaf page i/o
       @param   sectionNo  [in]  section number
       @param   totalPages [out] number of changed pages
       @param   totalIO    [out] number of io's made to flush the pages
       @return  none
     */

    void FlushSection (
        const RTETask_ITask &task,
        Converter_Index     &index,
        Converter_Pager     &pager,
        const SAPDB_UInt    sectionNo,
        SAPDB_UInt          &totalPages,
        SAPDB_UInt          &totalIO )
    {
        Data_PageNo dummyCount = 0;
        this->FlushSection( task, index, pager, sectionNo, totalPages, totalIO, dummyCount );
    }

    /*!
       @brief   Expand the map control to be able to handle NewNumAllPages more.
                Before the map is expanded all map sections are locked exclusively
                and it is assured that no savepoint is active
       @param   task            [in]     own task
       @param   newNumAllPages  [in]     after this command succeded the map control
                                         should manage the minimum number of NewNumAllPages
                                         pages
       @param   waitContext     [in|out] used to wait for other tasks
       @param   expandWaitQueue [in|out] queue to handle wait situations caused by
                the savepoint
       @return  (Converter_ReturnCode)
               Converter_Ok                     : Expand finished successfully
               Converter_SavePointActive        : Expand failed due to an active savepoint
               Converter_MemoryAllocationFailed : Expand failed due to a lack of memory
     */

    Converter_ReturnCode  Expand(
        const RTETask_ITask                             &task,
        const SAPDB_UInt                                newNumAllPages,
        Kernel_SynchronizedTaskWaitList::WaitContext    &waitContext,
        Kernel_SynchronizedTaskWaitList                 &expandWaitQueue );

    /* -----------------------------------------------------------------------*/
    /*! @name Back Up                                                         */
    /* -----------------------------------------------------------------------*/

    /*!
       @brief   Marks all occupied blocks in the FBM for backup and updates
                the flags in the converter which display whether a page has to
                be included in a incremental backup or not
       @param   task                  [in] own task
       @param   errMsg               [out] detailed information about error state
       @param   numPermConvPages      [in] Number of converter leaf pages required to
                                           manage all permanent data pages of this map control
       @param   numPermPagesForBackUp [in] number of permanent pages marked to be included
                                           in this back up (save data)
       @return  Converter_ReturnCode
                - Converter_MemoryAllocationFailed (+errMsg)
                - Converter_Ok
     */

    Converter_ReturnCode MarkPermPagesForSaveData(
        const RTETask_ITask &task,
        Msg_List            &errMsg,
        SAPDB_UInt          &numPermConvPages,
        SAPDB_UInt          &numPermPagesForBackUp );

    /*!
       @brief   Marks all occupied blocks which have been changed since the last save 
                data in the FBM for back up
       @param   task                    [in] own task
       @param   errMsg                 [out] detailed information about error state
       @param   numPermConvPages        [in] Number of converter leaf pages required to manage all
                                             permanent data pages of this map control
       @param   numPermPagesForBackUp   [in] number of permanent pages marked to be included
                                             in this back up (save data)
       @param   converterVersion        [in] current converter version
       @return  Converter_ReturnCode
                - Converter_MemoryAllocationFailed (+errMsg)
                - Converter_Ok
     */

    Converter_ReturnCode MarkPermPagesForSavePages(
        const RTETask_ITask     &task,
        Msg_List                &errMsg,
        SAPDB_UInt              &numPermConvPages,
        SAPDB_UInt              &numPermPagesForBackUp,
        const Converter_Version &converterVersion );


    void MarkPermPagesChanged( const RTETask_ITask  &task );

    /*!
       @brief   This method is used to write the content of a converter bit map
                page into a given page frame. After the page frame is copied
                the original page frame is deallocated. The copied page frame
                wil be copied into the backup file.
       @param   bitMapPage [in|out] page to be copied into
       @return  (SAPDB_Bool) true if page is available; else false
     */

    SAPDB_Bool GetBitMapPage( Converter_BitMapPage   &bitMapPage );

    /*!
       @brief   This method is used to clean up the internal converter bit map
                vector, which is used for data backup's (save pages)
       @return  none
     */

    void RemoveBitMapPages();

    Converter_ReturnCode ExtractArchiveConverterForSaveData( const RTETask_ITask  &task );

    SAPDB_Bool GetArchiveLeafPage( Converter_LeafPage   &leafPage );

    /*!
       @brief   This method is used to clean up the internal archive converter
                leaf page vector, which is used for data backup's (save data)
                in an archive instance.
        @return none
     */

    void RemoveArchiveLeafPages();

    /*!
       @brief   Allocates the memory required for the later use of the container
       @param   task    [in]  own task
       @param   errMsg [out]  detailed info about error state
       @return  Converter_ReturnCode
                - Converter_MemoryAllocationFailed (+ errMsg)
                - Converter_Ok
     */

    Converter_ReturnCode InitializePageNoList(
        const RTETask_ITask  &task,
        Msg_List             &errMsg );

    /*!
       @brief   Removes all page numbers from the container and returns
                all resources held by the container
       @return  none
     */

    void ClearPageNoList(){
        m_PageNoList.Delete();
    }

    /*!
       @brief   Checks if there are any page numbers left for back up
       @return  (SAPDB_Bool) true if there are no more page numbers stored
                in the back up page number list of this map control
     */

    SAPDB_Bool  PageNoListIsEmpty() const{
        return m_PageNoList.IsEmpty();
    }

    /*!
       @brief   Removes a page no from the PageNoContainer. This list is 
                protected by the corresponding  section.
       @param   task   [in] own task
       @param   pageNo [in] page number to remove
       @return  (bool) true if pageNo could be removed; else false
     */

    bool RemovePageNoFromPageNoList(
        const RTETask_ITask &task,
        const Data_PageNo   pageNo )
    {
        SAPDBTRACE_WRITELN( Converter_Trace, 7, "Remove " << SAPDB_ToString( GetRecoveryMode()) <<
                            " pageNo: " << pageNo << " from PnoList" );

        const SAPDB_UInt sectionNo = CalculateSectionNo( pageNo );
        Converter_PermMapSection section( task, m_SectionData[ sectionNo ], m_SectionPermData[ sectionNo ]);
        return m_PageNoList.Remove( pageNo );
    }

    /*!
       @brief   Updates the IO-address in a converter entry. After a page was restored
                this command updates the converter entry, i.e the block address of the
                restored page is updated in the converter entry and the old blockaddress
                is set to free
       @param   task                    [in] own task
       @param   pageNo                  [in] page number for which the converter entry
                                             has to be updated
       @param   snapShotVersion         [in] converter version of the snapshot
       @param   blockAddress            [in] new block address to be inserted
       @param   bRestoreDataActive      [in] restore data is running
       @return  (bool)
     */

    bool  SetBlockAddressOfRestoredPage(
        const RTETask_ITask&        task,
        Msg_List&                   msgList,
        const Data_PageNo           pageNo,
        const Converter_Version&    snapShotVersion,
        const IOMan_BlockAddress&   blockAddress,
        SAPDB_Bool                  bRestoreDataActive );

    /*!
       @brief   Starts recovery of used pagenos. This method must be called prior to
                the first call of RestoreUsedPageNos(), cause it's responsible for
                the memory allocation.
       @return  (SAPDB_Bool) true if successful
     */

    SAPDB_Bool BeginRestoreUsedPageNos(){
        return m_Map.BeginRestoreUsedPageNos();
    }

    /*!
       @brief   Releases page numbers which do not belong to a restored data base.
                For all page number which are not marked as used at the usedPageNoBitMapPage
                the corresponding converter entry is set to free and the block is released
                For all page numbers which are marked as used in the UsedPageNoBitMapPage but
                not in the converter the correspondung page entry is reserved.
                All converter pages which contain any used page number are marked as used
                (see also FreePageNosNotInConverterBitMapPages)
       @param   task                 [in] own task
       @param   usedPageNoBitMapPage [in] page which stores the page numbers of used
                                          pages in a bit map
       @param   snapShotVersion      [in] converter version of the snapshot
       @param   diffNumUsedPages    [out] number of pages by which the total number
                                          of used pages was increased and decreased
                                          respectively applying this restore function
       @return  none
     */

    void RestoreUsedPageNos(
        const RTETask_ITask         &task,
        const Converter_BitMapPage  &usedPageNoBitMapPage,
        const Converter_Version     &snapShotVersion,
        SAPDB_Int                   &diffNumUsedPages );

    /*!
       @brief   Releases page numbers which do not belong to a restored data base.
                After all UsedPageNoBitMapPage of a back up are read this function
                scans the converter and removes all those converter leaf pages (and
                their entries) which are not marked as used. (see also RestoreUsedPageNos)
                This means all those converter entries which were neither marked as used
                nor as free in the  UsedPageNoBitMapPage's  (means not at all mentioned)
                are removed
       @param   task              [in] own task
       @param   snapShotVersion   [in] converter version of the snapshot
       @param   diffNumUsedPages [out] number of pages by which the total number of used
                                       pages was increased and decreased respectively
                                       applying this restore function
       @return  none
     */

    void FinishRestoreUsedPageNos(
        const RTETask_ITask     &task,
        const Converter_Version &snapShotVersion,
        SAPDB_Int               &diffNumUsedPages );

    /*!
       @brief   Inform map control about successfuly complete data backup.
                This is needed for maintanace of updated perm pages used
                by the incremental data backup
       @param   task [in] own task
       @return  none
     */

    void SaveDataFinishedSuccessfuly( const RTETask_ITask  &task );

    /*!
    @brief   Inform map control about unsuccessfuly complete data backup.
             This is needed for maintanace of updated perm pages used
             by the incremental data backup
    @param   task [in] own task
    @return  none
    */

    void SaveDataFinishedUnSuccessfuly( const RTETask_ITask  &task );

    /* -----------------------------------------------------------------------*/
    /*! @name Savepoint                                                       */
    /* -----------------------------------------------------------------------*/

    /*!
       @brief   Switches the flag in the converter  for 'save point active' on.
                As long as the 'save point active' flag is set the converter cannot
                be expanded and released page numbers are not removed in the converter
                map.(see method FreePageNosAfterSavepoint)
       @param   task [in] own task
       @return  (Data_PageNo) Maximum Used Page No
     */

    Data_PageNo SetSavepointActiveFlag( const RTETask_ITask  &task );

    /*!
       @brief   Switches the flag in the converter for 'save point active' off
       @param   task [in] own task
       @return  none
     */

    void ReSetSavepointActiveFlag( const RTETask_ITask  &task );

    /*!
       @brief   Release all PageNos stored in the FreePageNoList and free
                all blocks marked as freeAfterSavepoint
       @param   task                    [in] own task
       @param   snapShotVersion         [in] converter version of the snapshot

       @return  none
     */

    void FreePageNosAfterSavepoint(
        const RTETask_ITask     &task,
        const Converter_Version &snapShotVersion );


    /* -----------------------------------------------------------------------*/
    /*! @name PageNo Mapping on block addresses                               */
    /* -----------------------------------------------------------------------*/

    /*!
       @brief   Get the current block address of of a page. If no block address
                is available, an invalid block address will be returned. Before
                the block address is searched, the free page no list is checked,
                whether the wanted page no is already remove, but caused by a
                running savepoint the corresponding converter entry is not updated
                yet.
       @param   task   [in] own task
       @param   pageNo [in] given page no the block address is requested for
       @param   block  [out] requested block address
       @return  (Converter_ReturnCode)
                Converter_Ok,
                Converter_NoConverterEntry,
                Converter_PageNoOutOfRange,
                Converter_InvalidBlockAddressFound
     */

    Converter_ReturnCode GetBlockAddress(
        const RTETask_ITask &task,
        const Data_PageNo   pageNo,
        IOMan_BlockAddress  &block );

    /*!
       @brief   Get an i/o adress of an empty block to write a permanent page to
                a data volume.
       @param   task                    [in] own task
       @param   pageNo                  [in] wanted page
       @param   pageConverterVersion    [in] converter version of the page to write
       @param   snapShotVersion         [in] converter version of the snapshot
       @param   bReqSequential          [in] request access mode of volume
       @param   newBlock               [out] new block address
       @return  (Converter_ReturnCode)
     */
    Converter_ReturnCode GetNewPositionForWrite(
        const RTETask_ITask      &task,
        const Data_PageNo         pageNo,
        const Converter_Version  &pageConverterVersion,
        const Converter_Version  &snapShotVersion,
        const SAPDB_Bool          bReqSequential,
        IOMan_BlockAddress       &newBlock );

    /*!
       @brief   Updates the block address of a permanent page. The old block
                address is set to Free or FreeAfterSavepoint.
       @param   task                 [in] own task
       @param   converterVersion     [in] current converter version
       @param   pageConverterVersion [in] version of the converter when the
                                          page was written the last time
       @param   snapShotVersion      [in] converter version of the snapshot
       @param   pageNo               [in] page number for which the I/o-address hast to
                                          be updated
       @param   block                [in] new block address which is to insert
       @return  none
     */

    void SetPositionForWriting(
        const RTETask_ITask         &task,
        const Converter_Version     &converterVersion,
        const Converter_Version     &pageConverterVersion,
        const Converter_Version     &snapShotVersion,
        const Data_PageNo           pageNo,
        const IOMan_BlockAddress    &block );

    /*!
       @brief   Releases a permanent page number. Frees the converter slot and updates
                the appropriate free chains and counters. The corresponding block is set
                to freeAfterSavepoint. If the converter slot is already empty an
                emergency shutdown is executed.
       @param   task                    [in] own task
       @param   pageConverterVersion    [in] converter version of the page to be freed
       @param   snapShotVersion         [in] converter version of the snapshot
       @param   pageNo                  [in] page number to release
       @param   waitContext             [in] wait item
       @param   freePageNoWaitQueue [in|out] queue to handle wait situations caused by the
                                              savepoint
       @param   bWaitUntilSavepointFinished  [out] true if the action could not be performed
                                                   due to a currently active savepoint
       @param   bOldBlockIsUsableImmediately [out] true if block is set to free and usable
                                                   during the current savepoint cycle by another page
       @param   bDoDecrementPermUseCount [out] true if page number is not part of the snapshot
                                               and therefore the perm used statistic count could
                                               be decreased.
       @return  none
     */

    void FreePageNo(
        const RTETask_ITask                             &task,
        const Converter_Version                         &pageConverterVersion,
        const Converter_Version                         &snapShotVersion,
        const Data_PageNo                               pageNo,
        Kernel_SynchronizedTaskWaitList::WaitContext    &waitContext,
        Kernel_SynchronizedTaskWaitList                 &freePageNoWaitQueue,
        SAPDB_Bool                                      &bWaitUntilSavepointFinished,
        SAPDB_Bool                                      &bOldBlockIsUsableImmediately,
        SAPDB_Bool                                      &bDoDecrementPermUseCount );

    /* -----------------------------------------------------------------------*/
    /*! @name ColdVerify/Check data with update                               */
    /* -----------------------------------------------------------------------*/

    /*!
       @brief   Marks a page in the back up page number container as  used
       @param   task   [in] own task
       @param   pageNo [in] page number to be marked as used
       @return  none
     */
    void MarkPageNoAsUsed(
        const RTETask_ITask &task,
        const Data_PageNo   pageNo );

    /*!
       @brief   Releases all perm pages which are not marked as used in the
                backup page no container
       @param   task                    [in] own task
       @param   snapShotVersion         [in] converter version of the snapshot
       @param   numPermDataPages       [out] number of permanent data pages used
       @param   numPermConPages        [out] number of converter pages used for permanent data pages
       @param   numReleasesBlocks      [out] number of released blocks, which were not referenced during cold verify
       @return  none
     */
    void ReleaseUnusedPageNos (
        const RTETask_ITask     &task,
        const Converter_Version &snapShotVersion,
        SAPDB_UInt              &numPermDataPages,
        SAPDB_UInt              &numPermConPages,
        SAPDB_UInt              &numReleasesBlocks );

    /*!
       @brief   Finishes a failed converter verification. The converter is
                restored to the state it had before the verification was
                started. i.e. the back up page no lists which were created
                so far are destroyed and the blocks marked in the FBM are
                retored
       @param   task [in] own task
       @return  none
     */

    void EndFailedCheckData( const RTETask_ITask  &task );

    /* -----------------------------------------------------------------------*/
    /*! @name Common                                                          */
    /* -----------------------------------------------------------------------*/

    /*!
       @brief   Returns the number of all permanent pages changed since
                the last save data
       @param   task [in] own task
       @return  (Data_PageCount)
     */
    Data_PageCount NumUpdatedPermPages( const RTETask_ITask  &task );

    /*!
       @brief   Returns the number of converter pages marked as changed.
                This method is implemented access optimized.
       @return  (Data_PageCount)
     */

    Data_PageCount GetNumberOfChangedConvPages() const{
        return( m_NumChangedConverterPages );
    }

    /* -----------------------------------------------------------------------*/
    /*! @name Dump                                                            */
    /* -----------------------------------------------------------------------*/

    /*!
       @brief   Dumps MapControl and the Map into the kernel dump file
       @param   task         [in] own task
       @param   dump     [in|out] kernel dump file
       @param   bEnterRegion [in] use the converter regions
       @return  none
     */

    virtual void Dump(
        const RTETask_ITask &task,
        Kernel_Dump         &dump,
        const SAPDB_Bool    bEnterRegion );

    /* -----------------------------------------------------------------------*/
    /*! @name SnapShot                                                        */
    /* -----------------------------------------------------------------------*/

    /*!
      @brief   Checks if the converter leaf page specified by the given
               basePageNo, has the same block address as the given block
               address.
      @param   task       [in] own task
      @param   basePageNo [in] data base page number
      @param   block      [in] block address of the converter leaf page
      @return  SAPDB_Bool
    */

    SAPDB_Bool  IsMember(
        const RTETask_ITask         &task,
        const Data_PageNo           basePageNo,
        const IOMan_BlockAddress    &block );

protected:

    /* -----------------------------------------------------------------------*/
    /*! @name protected methods                                               */
    /* -----------------------------------------------------------------------*/

    /// see public method FlushSection

    void FlushSection(
        const RTETask_ITask &task,
        Converter_Index     &index,
        Converter_Pager     &pager,
        const SAPDB_UInt    sectionNo,
        SAPDB_UInt          &totalPages,
        SAPDB_UInt          &totalIO,
        Data_PageNo         &maxUsedPageNoInSection );

    /// virtual is needed. because of re-implementation in StaticMapControl!
    virtual void WriteChangedConverterPage(
        const RTETask_ITask      &task,
        Converter_PermMapSection &section,
        Converter_LeafPage       &leafPage,
        Converter_Pager          &pager,
        SAPDB_UInt               &totalPages,
        SAPDB_UInt               &totalIO );

    /// see public method MarkPermPagesForSaveData

    void MarkPermPagesForSaveData(
        const RTETask_ITask         &task,
        Converter_PermMapSection    &section,
        Converter_LeafPage          &leafPage,
        SAPDB_UInt                  &numPermPagesForBackUp );

    /// see public method MarkPermPagesForSavePages

    void MarkPermPagesForSavePages(
        const RTETask_ITask    		&task,
        Converter_PermMapSection	&section,
        Converter_LeafPage      	&leafPage,
        SAPDB_UInt              	&numPermPagesForBackUp,
        const Converter_Version 	&converterVersion );

    /// see public method ReleaseUnusedPageNos

    void ReleaseUnusedPageNosOfOneConverterPage(
        Converter_PermMapSection &section,
        const RTETask_ITask      &task,
        const Converter_Version  &snapShotVersion,
        Converter_LeafPage       &leafPage,
        SAPDB_UInt               &numUsedDataPages,
        SAPDB_UInt               &numReleasesBlocks );

protected:

    /// Is used during save pages to store all page no's used at this
    /// time. This information will be written into the hostfile and
    /// re-used during the restore pages to determine the pages which
    /// could be removed.

    Converter_UsedPageNoContainer   m_UsedPageNoList;

    /// Is used during save data and save pages to store all page no's
    /// mark for backup. During backup the page no are remove from
    /// this list. At the end of the savepoint this list has to be empty!
    /// Also used during Check Data With Update to mark all used page
    /// numbers.

    Converter_PageNoContainer   m_PageNoList;

    /// list of section item containing the section specific informations

    Container_Vector<Converter_PermMapSectionData>  m_SectionPermData;

    //// number of changed permanent converter pages. Must be optimized for high read
    ///  access because this one is used during every call ofConverter::HandleDbFull

    RTESync_InterlockedCounter<SAPDB_UInt>  m_NumChangedConverterPages;

    /// copy of Converter for Archive backup

    Converter_ArchiveContainer  m_ArchiveConverter;

    /// lock for container operations (m_ArchiveConverter and m_UsedPageNoList)

    RTESync_Spinlock    m_ContainerLock;

};

#endif //CONVERTER_PERM_MAPCONTROL_HPP
