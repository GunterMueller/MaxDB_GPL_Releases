/*!
  @file           Converter_IBackUp.hpp
  @author         TorstenS
  @author         AlexanderK
  @ingroup        Converter
  @brief          abstract back up interface class of the converter
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



#ifndef CONVERTER_IBACKUP_HPP
#define CONVERTER_IBACKUP_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 **/

#include "DataAccess/Data_Types.hpp"
#include "Converter/Converter_Types.hpp"


class Converter_LeafPage;
class Converter_BitMapPage;
class Converter_UsedPageNoContainer;
class IOMan_BlockAddress;
class RTETask_ITask;
class Msg_List;

/*!
  @interface    Converter_IBackUp
  @brief        Abstract back up interface class of the converter
 */

class Converter_IBackUp
{
public:
    /*!
       @brief   Returns the backup interface of the converter. This static
                function returns the interface of the converter which allows
                access to all methods defined in this interface class
       @return  (Converter_IBackUp) instance of the converter
     */

    static   Converter_IBackUp & Instance();

    /*!
       @brief   Starts a total backup (also called save data). All currently
                occupied blocks of the database are marked within the FBM to
                be part of the backup.
       @param   taskId [in] task id
       @return  Converter_ReturnCode
                - Converter_Ok
                - Converter_MemoryAllocationFailed (+errMsg)
     */

    virtual Converter_ReturnCode BeginSaveData(
        const RTETask_ITask &task,
        Msg_List            &errMsg ) = 0;

    /*!
       @brief   Starts a incremental data backup (also called save pages). All
                currently occupied blocks of the database which are marked as
                changed since the last successful complete data backup are 
                marked within the FBM to be part of the backup. A list of all 
                page numbers  used in the current data base is created. This
                list is used in case of a recovery to remove unsed pages.
       @param   taskId [in] task id
       @return  Converter_ReturnCode
                - Converter_Ok
                - Converter_MemoryAllocationFailed (+errMsg)
     */

    virtual Converter_ReturnCode BeginSavePages(
        const RTETask_ITask &task,
        Msg_List            &errMsg ) = 0;

    /*!
        @brief  Ends the backup of data or pages. The back up active flag in
                the converter is reset. If  the current back up  did not finish
                successfully all blocks still marked as to be written to the
                back up files are unmarked.
        @param   task                       [in] own task
        @param  bBackUpSuccessfullyFinished [in] indicates whether the current back
                                                 up finished successfully
        @return none
    */

    virtual void  EndSave(
        const RTETask_ITask &task,
        const SAPDB_Bool    bBackUpSuccessfullyFinished ) = 0;

    /*!
        @brief  Returns the number of of pages marked for back up
        @return none
    */

    virtual SAPDB_UInt GetNumPagesForBackUp() = 0 ;

    /*!
        @brief  Returns the number of converter pages required to address all
                data pages which belong to the current backup.
        @return SAPDB_UInt
    */

    virtual SAPDB_UInt GetNumConvPagesForBackUp() = 0;

    /*!
       @brief   Checks if the current back up finished successfully. Checks if
                all pages marked as to be included in the back up were fetched
                by the back up manager
       @param   task [in] own task
       @return  true if the back was successful else false
     */

    virtual SAPDB_Bool CheckBackUp( const RTETask_ITask &task ) const = 0;

    /*!
       @brief   Inform the converter that a given page was written into the backup file. 
                Removes the pageNo from the read page frame from the list of pageNo's generated
                at the beginning of the backup process. If the given pageNo is not member 
                of the backup pageNo list than an error occur.
       @param   task     [in] own task
       @param   msgList [out] for error tracking purpose
       @param   pageNo   [in] page number of the block which was mentioned above
       @param   addrMode [in] addressing mode (static resp. perm) of the block which was mentioned above
       @return  (bool) in case of false the msgList is filled
     */
  
    virtual bool
    ResetDataPageAfterBackUp(
        RTETask_ITask&              task,
        Msg_List&                   msgList,
        const Data_PageNo&          pageNo,
        const Data_AddressingMode&  addrMode ) = 0;

    /*!
       @brief   Sets a block address in a converter entry. After a page was restored
                this command updates the converter entry, i.e the block address of
                the restored page is updated in the converter entry
       @param   task     [in] own task
       @param   msgList [out] message list for error diagnostic purpose
       @param   block    [in] new block address which hast to be inserted in
                              the converter entry of the page defined by PageNo
                              and PageRecoveryMode
       @param   pageNo   [in] page number for which the converter entry has to be updated
       @param   addrMode [in] addressing mode of the page for which the back up
                              flags have to be removed
       @return  (bool)
     */
    virtual bool SetBlockAddressOfRestoredPage(
        RTETask_ITask&              task,
        Msg_List&                   msgList,
        const IOMan_BlockAddress&   block,
        const Data_PageNo&          pageNo,
        const Data_AddressingMode&  addrMode ) = 0;

    /*!
       @brief   This method fills the given page frame with a the copy
                of a static converter leaf page. After the page frame
                is copied the original page frame is deallocated.
       @param   leafPage [out] leaf page to be filled
       @return  SAPDB_Bool
     */

    virtual SAPDB_Bool GetStaticArchiveLeafPage( Converter_LeafPage &leafPage ) = 0;

    /*!
       @brief   This method fills the given page frame with a the copy
                of a dynamic converter leaf page. After the page frame
                is copied the original page frame is deallocated.
       @param   leafPage [out] leaf page to be filled
       @return  SAPDB_Bool
     */

    virtual SAPDB_Bool GetDynamicArchiveLeafPage( Converter_LeafPage &leafPage ) = 0;

    /*!
       @brief   Clears the containers with copied converter leafpages. Removes
                all copied converter leafpages which belong to a backup
       @return  none
     */

    virtual void RemoveArchiveLeafPages() = 0;

    /*!
       @brief   This method is used to write the content of a static
                converter bit map page into a given page frame. After
                the page frame is copied the original page frame is deallocated.
                The copied page frame will be copied into the backup file.
       @param   bitMapPage [in|out] page to be copied into
       @return  (SAPDB_Bool) true if page is available; else false
     */

    virtual SAPDB_Bool GetStaticBitMapPage( Converter_BitMapPage   &bitMapPage ) = 0;

    /*!
       @brief   This method is used to write the content of a dynamic
                converter bit map page into a given page frame. After
                the page frame is copied the original page frame is deallocated.
                The copied page frame will be copied into the backup file.
       @param   bitMapPage [in|out] page to be copied into
       @return  (SAPDB_Bool) true if page is available; else false
     */

    virtual SAPDB_Bool GetDynamicBitMapPage( Converter_BitMapPage   &bitMapPage ) = 0;

    /*!
      @brief   This method is used to clean up the internal static and
               dynamic converter bit map vector, which is used for data
               backup's (save pages)
      @return  none
    */

    virtual void RemoveBitMapPages() = 0;

    /*!
       @brief   Starts recovery of used pagenos. This method must be called prior
                to the first call to RestoreUsedPageNos() to allocated the needed
                memory.
       @return  (SAPDB_Bool) true if successful
     */

    virtual SAPDB_Bool BeginRestoreUsedPageNos() = 0;

    /*!
        @brief  Releases page numbers which do not belong to a restored data base.
                All page number which are not marked as used at the UsedPageNoBitMapPage
                the corresponding converter entry is set to free and the block is released.
                All page numbers which are marked as used in the UsedPageNoBitMapPage but
                not in the converter the correspondung page entry is reserved. All
                converter pages which contain any used page number are marked as used
                (see also FreePageNosNotInConverterBitMapPages)
        @param  task                 [in] own task
        @param  UsedPageNoBitMapPage [in] page which stores the page numbers of
                                      used pages in a bit map
        @return none
    */

    virtual void RestoreUsedPageNos(
        const RTETask_ITask         &task,
        const Converter_BitMapPage  &UsedPageNoBitMapPage ) = 0;

    /*!
        @brief  integrate converter pages from backup into the converter.
        @param  task     [in] own task
        @param  leafPage [in] converter page from backup medium
        @return none
    */

    virtual void RestoreConverterPage(
        RTETask_ITask&      task,
        Converter_LeafPage& leafPage ) = 0;

    /*!
        @brief  Releases page numbers which do not belong to a restored data base.
                After all UsedPageNoBitMapPage of a back up are read this function
                scans the converter and removes all those converter leaf pages
                (and their entries) which are not marked as used. (see also FreeUnusedPageNo)
                This means all those converter entries which were neither marked
                as used nor as free in the UsedPageNoBitMapPage's (means not at
                all mentioned) are removed.
        @param  task [in] own task
        @return none
    */

    virtual void FinishRestoreUsedPageNos( const RTETask_ITask &task ) = 0;
};

#endif  /* CONVERTER_IBACKUP_HPP */
