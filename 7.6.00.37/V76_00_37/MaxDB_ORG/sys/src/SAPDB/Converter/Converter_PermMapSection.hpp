/*!
  @file     Converter_PermMapSection.hpp
  @author   TorstenS
  @ingroup  Converter
  @brief    This calls is used to manage the concurrent access
            to the Converter_PermMapSectionData information.
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


#ifndef CONVERTER_PERM_MAP_SECTION_HPP
#define CONVERTER_PERM_MAP_SECTION_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 **/

#include "Converter/Converter_Types.hpp"
#include "Converter/Converter_Chain.hpp"
#include "Converter/Converter_MapSection.hpp"
#include "Converter/Converter_MapSectionData.hpp"
#include "Converter/Converter_PermMapSectionData.hpp"
#include "Container/Container_Vector.hpp"

#include "RunTime/Tasking/RTETask_ITask.hpp"


/*===========================================================================*
 *  FORWARD DECLARATION                                                      *
 **/

class Converter_FreePageList;
class Converter_ParentReference;
class Converter_MapEntry;


/*===========================================================================*
 *  DEFINES                                                                  *
 **/


/*!
  @class    Converter_PermMapSection
  @brief    This class is a extension of the base class Converter_MapSection
            which is now able to manage the permanent part of the converter map.
            See also Converter_PermMapSectionData.
 */

class Converter_PermMapSection : public Converter_MapSection
{
public:

    /// iterator of free page no list
    typedef Converter_FreePageList::Iterator     FreePageListIterator;

    /// an object containing info about whether a leaf page is changed
    typedef Converter_PermMapSectionData::ChangedInfo   ChangedInfo;

/*!
  @class    ChangedEntryIterator
  @brief    This class implements an iterator operating on the changed entries
            of the persistent map. Changed entries have to be flushed into
            the data area at each savepoint
 */

class ChangedEntryIterator :
                public Converter_ChainIteratorTPL< Container_Vector<ChangedInfo> , ChangedInfo>
    {
    public:
        /// my chain iterator
        typedef Converter_ChainIteratorTPL< Container_Vector<ChangedInfo> , ChangedInfo> Parent;

        /*!
           @brief   constructor
           @param   changedChainIter [in] iterator onto the list of changed entries
           @param   map              [in] converter map
        */

        ChangedEntryIterator(
            const Parent    &changedChainIter,
            Converter_Map   &map );

        /*!
           @brief   Returns the map entry, where the iterator actualy points to.
           @return  Converter_MapEntry&
        */

        Converter_MapEntry& operator*() const;

    private:
        /// pointer to entries
        Converter_MapSectionData::MapVector *m_pEntryVector;
    };

public:

    /*!
       @brief   Default constructor
       @return  none
    */

    Converter_PermMapSection()
    :
    Converter_MapSection(),
    m_pSectionPermData( NULL )
    {}

    /*!
       @brief   Constructor. Is used to lock the section to allow
                synchronized concurrent access.
       @param   task                [in] own task
       @param   sectionData     [in|out] corresponding section information
                                         to be protected by this class
       @param   sectionPermData [in|out] corresponding perm section information
                                         to be protected by this class
       @param   bUnlockWithCheck    [in] execute check onto the section
                                         during object destruction. See
                                         also destructor description.
    */

    Converter_PermMapSection(
        const RTETask_ITask          &task,
        Converter_MapSectionData     &sectionData,
        Converter_PermMapSectionData &sectionPermData,
        const SAPDB_Bool             bUnlockWithCheck = SAPDB_TRUE )
    :
    Converter_MapSection( task, sectionData, bUnlockWithCheck ),
    m_pSectionPermData( &sectionPermData )
    {}

    /*!
       @brief   Explicit locking of the section, if the default ctor
                was used before to create the object.
       @param   task                [in] own task
       @param   sectionData     [in|out] corresponding section information
                                         to be protected by this class
       @param   sectionPermData [in|out] corresponding perm section information
                                         to be protected by this class
       @param   bUnlockWithCheck    [in] execute check onto the section
                                         during object destruction. See
                                         also destructor description.
       @return  none
    */

    void Lock(
        const RTETask_ITask          &task,
        Converter_MapSectionData     &sectionData,
        Converter_PermMapSectionData &sectionPermData,
        const SAPDB_Bool             bUnlockWithCheck = SAPDB_TRUE )
    {
        Converter_MapSection::Lock( task, sectionData, bUnlockWithCheck );
        m_pSectionPermData = &sectionPermData;
    }

    /*!
       @brief   Explicit locking of the section. It's test by
                an assertion that the lock doesn't exist and
                the given taskId identifies the running task.
       @return  none
    */

    void Lock(){
        Converter_MapSection::Lock();
    }

    /*!
        @brief  Shutdown is used to release the allocated memory
        @return none
    */

    void Shutdown(){
        m_pSectionData->Shutdown();
        m_pSectionPermData->Shutdown();
    }

    /*!
       @brief   This method is used to inform the section about the begin
                of a savepoint.
       @return  none
    */

    void SetSavepointActive(){
        SAPDBERR_ASSERT_STATE( SAPDB_FALSE == m_pSectionPermData->m_SavepointIsActive );
        m_pSectionPermData->m_SavepointIsActive = SAPDB_TRUE;
    }

    /*!
       @brief   This method is used to inform the section about the end
                of a savepoint.
       @return  none
    */

    void ResetSavepointActive(){
        SAPDBERR_ASSERT_STATE( SAPDB_TRUE == m_pSectionPermData->m_SavepointIsActive );
        m_pSectionPermData->m_SavepointIsActive = SAPDB_FALSE;
    }

    /*!
       @brief   Returns true if a savepoint is active.
       @return  (SAPDB_Bool)
    */

    SAPDB_Bool IsSavepointActive(){
        return( SAPDB_TRUE == m_pSectionPermData->m_SavepointIsActive );
    }
    /*!
       @brief   This method is used to inform the section about the 
                successful end of the last complete data backup.
       @return  none
    */

    void SetStateOfLastSaveDataToSuccessful(){
        m_pSectionPermData->m_LastSaveDataSuccessful = SAPDB_TRUE;
    }

    /*!
       @brief   This method is used to inform the section about the 
                unsuccessful end of the last complete data backup.
       @return  none
    */

    void SetStateOfLastSaveDataToUnSuccessful(){
        m_pSectionPermData->m_LastSaveDataSuccessful = SAPDB_FALSE;
    }

    /*!
       @brief   Returns true if last complete save data was successful
       @return  (SAPDB_Bool)
    */

    SAPDB_Bool IsLastSaveDataSuccessful(){
        return( SAPDB_TRUE == m_pSectionPermData->m_LastSaveDataSuccessful );
    }

    /*!
       @brief   Increment the number of pending i/o orders. For further
                information see description of Converter_PermMapSectionData.
       @return  none
    */

    void IncrementPendingIO(){
        m_pSectionPermData->m_PendingIO += 1;
    }

    /*!
       @brief   Decrement the number of pending i/o orders. For further
                information see description of Converter_PermMapSectionData.
       @return  none
    */

    void DecrementPendingIO(){
        m_pSectionPermData->m_PendingIO -= 1;
    }

    /*!
       @brief   Returns the number of updated perm pages. For further information
                see description of Converter_PermMapSectionData.
       @return  SAPDB_UInt
    */

    SAPDB_UInt GetNumUpdatedPages() const
    {
        return 
            m_pSectionPermData->m_NumUpdatedPermPages 
            +
            m_pSectionPermData->m_NumPendingUpdatedPermPages;
    }

    /*!
       @brief   Freeze number of updated perm pages. 
       @return  none
    */

    void FreezeNumUpdatedPages() 
    {
         m_pSectionPermData->m_NumPendingUpdatedPermPages 
             += m_pSectionPermData->m_NumUpdatedPermPages;

         m_pSectionPermData->m_NumUpdatedPermPages = 0;
    }

    /*!
       @brief   Clear frozen number of updated perm pages.
       @return  none
    */

    void ClearFrozenNumUpdatedPages(){
        m_pSectionPermData->m_NumPendingUpdatedPermPages = 0;
    }

    /*!
       @brief   ReInitialize update counters after a restart
       @param   bPageIsUpdated         [in] is page marked as updated
       @param   bPageHasPendingUpdate  [in] is page marked as a pending update, 
                                            because of a failed complete backup
       @return  none
    */

    void ReInitialzeUpdatePages(
        const bool bPageIsUpdated,
        const bool bPageHasPendingUpdate )
    {
        if( bPageIsUpdated ){
            m_pSectionPermData->m_NumUpdatedPermPages +=1;
        }
        else if( ! this->IsLastSaveDataSuccessful() && bPageHasPendingUpdate ){
            m_pSectionPermData->m_NumPendingUpdatedPermPages +=1;
        }
    }

    /*!
       @brief   Increment the number of updated perm pages by one. 
       @param   bPageIsUpdated         [in] is page marked as updated
       @param   bPageHasPendingUpdate  [in] is page marked as a pending update, 
                                            because of a failed complete backup
       @return  none
    */

    void IncrementUpdatedPages( 
        const bool bPageIsUpdated,
        const bool bPageHasPendingUpdate )
    {
        // Do not increase m_NumUpdatedPermPages if page is already counted
        // as updated or already marked as pending and the last complete 
        // backup was not successful. In the last case the page is already
        // counted by m_NumPendingUpdatedPermPages

        if( bPageIsUpdated )
            return;

        if( this->IsLastSaveDataSuccessful() || ! bPageHasPendingUpdate )
            m_pSectionPermData->m_NumUpdatedPermPages +=1;
    }

    /*!
       @brief   Decrement the number of updated perm pages by one.
       @param   bPageIsUpdated        [in] is page marked as updated
       @param   bPageHasPendingUpdate [in] is page marked as a pending update, 
                                           because of a failed complete backup
       @return  none
    */

    void DecrementUpdatedPages(
        const bool bPageIsUpdated,
        const bool bPageHasPendingUpdate )
    {
        if( bPageIsUpdated )
        {
            if( this->IsLastSaveDataSuccessful() || ! bPageHasPendingUpdate )
                m_pSectionPermData->m_NumUpdatedPermPages -=1;
            else
                m_pSectionPermData->m_NumPendingUpdatedPermPages -=1;
            return;
        }
        if( ! this->IsLastSaveDataSuccessful() && bPageHasPendingUpdate )
             m_pSectionPermData->m_NumPendingUpdatedPermPages -=1;
    }

    /*!
       @brief   Returns the maximum used data pageno in the specified section.
                Note that the returned page is not exact the highest used
                page no.
       @return  (Data_PageNo) unused page count
     */

    Data_PageNo GetMaxUsedPageNo() const;

    /*!
        @brief  Returns a reference to a list of parent refences,
                which contains the addresses of converter leaves
                to be removed from the converter index.
        @return (Container_Vector<Converter_ParentReference>&)
    */

    Container_Vector<Converter_ParentReference>&  GetParentReferences(){
        return( m_pSectionPermData->m_ParentRefs );
    }

    /*!
        @brief  Returns a reference to the list of free page no's. These
                page no's couldn't be freed immediately, because a savepoint
                was running.
        @return (onverter_FreePageList&)
    */

    Converter_FreePageList& GetFreeList(){
        return( m_pSectionPermData->m_FreePageNoList );
    }

    /*!
         @brief     Integrate a given converter leaf page into the converter map.
                    This will be doe during each restart.
         @param     usedEntryCount [in] number of used entries
         @param     freeEntryCount [in] number of free entries
         @return    none
     */

    void IntegratePage(
        const Converter_MapEntryNo  entryNo,
        const SAPDB_UInt            usedEntryCount,
        const SAPDB_UInt            freeEntryCount )
    {
        m_pSectionData->m_UnusedEntryChain.Remove( entryNo );

        IncrementNumUsedDataPages( usedEntryCount );

        if( freeEntryCount > 0 )
            m_pSectionData->m_PartiallyUsedEntryChain.InsertFront( entryNo );
        else
            m_pSectionData->m_FullUsedEntryChain.InsertFront( entryNo );

        CheckIncMaxUsedEntryNo( entryNo );
    }

    /*!
        @brief  Returns the number of used converter leaf pages in the map.
        @return (SAPDB_Int) page count
    */

    SAPDB_Int GetPageCount() const
    {
        return( m_pSectionData->m_FullUsedEntryChain.GetNodeCount() +
                m_pSectionData->m_PartiallyUsedEntryChain.GetNodeCount());
    }

    /* -----------------------------------------------------------------------*/
    /*! @name Changed entry handling  and iterators                           */
    /* -----------------------------------------------------------------------*/
    /*!
       @brief   Returns iterator pointing to the first changed entry in the map
       @return  (ChangedEntryIterator)
     */

    ChangedEntryIterator ChangedEntriesBegin(){
        return( ChangedEntryIterator( m_pSectionPermData->m_ChangedChain.Begin(),
            *( m_pSectionData->m_pMap )));
    }

    /*!
       @brief   Returns iterator behind the last changed entry in the map
       @return  (ChangedEntryIterator)
    */

    ChangedEntryIterator ChangedEntriesEnd(){
        return( ChangedEntryIterator( m_pSectionPermData->m_ChangedChain.End(),
            *( m_pSectionData->m_pMap )));
    }

    /*!
       @brief   The map entry identified by the given map entry number
                is set to unchanged and the counter for the changed
                pages is reduced. The next changed entry is returned.
       @param   entryNo [in] map entry index
       @return  ChangedEntryIterator
    */

    ChangedEntryIterator MarkPageAsUnchangedAndGoToNextChangedEntry(
        const Converter_MapEntryNo  entryNo );

    /*!
       @brief   If the map entry identified by the given map entry number
                is changed, it will be set to unchanged and the counter
                for the changed pages is reduced.
       @param   entryNo [in] map entry index
       @return  none
    */

    void MarkPageAsUnChanged( const Converter_MapEntryNo  entryNo );

    /*!
       @brief   If the map entry identified by the given map entry number
                is not changed, it will be set to changed and the counter
                for the changed pages will be increased.
       @param   entryNo [in] map entry index
       @return  none
    */

    void MarkPageAsChanged( const Converter_MapEntryNo  entryNo );

    /* -----------------------------------------------------------------------*/
    /*! @name  Iterator for free page no list                                 */
    /* -----------------------------------------------------------------------*/

    /*!
       @brief   Returns iterator at begin of free page no list.
       @return  (FreePageListIterator)
     */

    FreePageListIterator FreePageNoListBegin(){
        return( m_pSectionPermData->m_FreePageNoList.Begin());
    }

    /*!
       @brief   Returns iterator behind of free page no list.
       @return  (FreePageListIterator)
     */

    FreePageListIterator FreePageNoListEnd(){
        return( m_pSectionPermData->m_FreePageNoList.End());
    }

private:

    /// pointer to corresponding recoverable map section data

    Converter_PermMapSectionData    *m_pSectionPermData;
};

#endif  /* CONVERTER_PERM_MAP_SECTION_HPP */

