/*!
  @file     Converter_MapSection.hpp
  @author   TorstenS
  @ingroup  Converter
  @brief    This calls is used to manage the concurrent access
            to the Converter_MapSectionData information.
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


#ifndef CONVERTER_MAP_SECTION_HPP
#define CONVERTER_MAP_SECTION_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 **/

#include "Converter/Converter_Types.hpp"
#include "Converter/Converter_MapSectionData.hpp"

#include "RunTime/Tasking/RTETask_ITask.hpp"


/*===========================================================================*
 *  FORWARD DECLARATION                                                      *
 **/

class Converter_Map;
class Data_PageRecoveryMode;

/*===========================================================================*
 *  DEFINES                                                                  *
 **/

/*!
  @class    Converter_MapSection
  @brief    This class is used to access the proper data of the
            corresponding section. This class implements a lock
            mechanism, which guarantees that all operation on the
            section specific part of the converter map are well
            synchronized. This class fits to the requirements of
            a temporary section. See also Converter_MapSectionData.
 */

class Converter_MapSection
{
public:

    /// iterator of unused entries
    typedef Converter_MapSectionData::MapEntryChain::Iterator     UnusedEntryIterator;

    /// iterator of partially used entries
    typedef Converter_MapSectionData::MapEntryChain::Iterator     PartiallyUsedEntryIterator;

    /// iterator of full entries
    typedef Converter_MapSectionData::MapEntryChain::Iterator     FullEntryUsedIterator;


    /* -----------------------------------------------------------------------*/
    /*! @name Common section administration                                   */
    /* -----------------------------------------------------------------------*/

    /*!
       @brief   Default constructor
       @return  none
    */

    Converter_MapSection()
    :
    m_pSectionData( NULL ),
    m_TaskId( RTE_UnknownTaskId ),
    m_bIsLocked( SAPDB_FALSE ),
    m_bUnlockWithCheck( SAPDB_TRUE )
    {}

    /*!
       @brief   Constructor. Is used to lock the section to allow
                synchronized concurrent access.
       @param   task             [in] own task
       @param   sectionData  [in|out] corresponding section information
                                      to be protected by this class
       @param   bUnlockWithCheck [in] execute check onto the section
                                      during object destruction. See
                                      also destructor description.
    */

    Converter_MapSection(
        const RTETask_ITask         &task,
        Converter_MapSectionData    &sectionData,
        const SAPDB_Bool            bUnlockWithCheck = SAPDB_TRUE )
    {
        m_bIsLocked = SAPDB_FALSE;

        Lock( task, sectionData, bUnlockWithCheck );
    }

    /*!
    @brief  Destructor. Removes section lock if needed. Dependend of
            the input parameter bUnlockWithCheck the correctness of
            the section object is validated during object destruction.
            In the fast system this check is executed only if the
            converter check level 5 is activated.
    */

    ~Converter_MapSection() {
        this->Unlock();
    }

    /*!
       @brief   Explicit locking of the section, if the default ctor
                was used before to create the object.
       @param   task             [in] own task
       @param   sectionData  [in|out] corresponding section information
                                      to be protected by this class
       @param   bUnlockWithCheck [in] execute check onto the section
                                      during object destruction. See
                                      also destructor description.
       @return  none
    */

    void Lock(
        const RTETask_ITask         &task,
        Converter_MapSectionData    &sectionData,
        const SAPDB_Bool            bUnlockWithCheck = SAPDB_TRUE )
    {
        m_TaskId           = task.ID();
        m_pSectionData     = &sectionData;
        m_bUnlockWithCheck = bUnlockWithCheck;

        Converter_MapSection::Lock();
    }

    /*!
       @brief   Explicit locking of the section. It's test by
                an assertion that the lock doesn't exist and
                the given taskId identifies the running task.
       @return  none
    */

    void Lock();

    /*!
       @brief   Explicit unlocking of the section. If no lock
                exist nothing will be done. If the section object
                was created with the check parameter see destructor
                description for additional functionality executed by
                this method.
       @return  none
    */

    void Unlock();

    /*!
       @brief   Release/Reset map section data
       @return  none
    */

    void Shutdown(){
        m_pSectionData->Shutdown();
    }

    /* -----------------------------------------------------------------------*/
    /*! @name  Chain handling methods                                         */
    /* -----------------------------------------------------------------------*/

    /*!
       @brief   Remove given entry from partially filled chain
                and insert into full filled chain.
       @return  none
    */

    void RechainEntryFromPartiallyUsedToFull( const Converter_MapEntryNo entryNo )
    {
        m_pSectionData->m_PartiallyUsedEntryChain.Remove( entryNo );
        m_pSectionData->m_FullUsedEntryChain.InsertFront( entryNo );
    }

    /*!
       @brief   Remove given entry from unused chain
                and insert into partially filled chain.
       @return  none
    */

    void RechainEntryFromUnusedToPartiallyUsed( const Converter_MapEntryNo entryNo )
    {
        m_pSectionData->m_UnusedEntryChain.Remove( entryNo );
        m_pSectionData->m_PartiallyUsedEntryChain.InsertFront( entryNo );
    }

    /*!
       @brief   Remove given entry from partially used chain and
                insert into unused chain.
       @return  none
    */

    void RechainEntryFromPartiallyUsedToUnused( const Converter_MapEntryNo entryNo )
    {
        m_pSectionData->m_PartiallyUsedEntryChain.Remove( entryNo );
        m_pSectionData->m_UnusedEntryChain.InsertFront( entryNo );
    }

    /*!
       @brief   Remove given entry from full filled chain and
                insert into parially used chain.
       @return  none
    */

    void RechainEntryFromFullToPartiallyUsed( const Converter_MapEntryNo entryNo )
    {
        m_pSectionData->m_FullUsedEntryChain.Remove( entryNo );
        m_pSectionData->m_PartiallyUsedEntryChain.InsertFront( entryNo );
    }

    /*!
       @brief   Append given entry to the end of the unused chain.
       @return  none
    */

    void AppendToUnusedEntryChain( const Converter_MapEntryNo  entryNo ){
        m_pSectionData->m_UnusedEntryChain.InsertEnd( entryNo );
    }

    /* -----------------------------------------------------------------------*/
    /*! @name  Iterator for unused entry chain                                */
    /* -----------------------------------------------------------------------*/

    /*!
       @brief   Returns iterator at begin of chain of unused entries
       @return  (UnusedEntryIterator)
     */

    UnusedEntryIterator UnusedEntriesBegin(){
        return( m_pSectionData->m_UnusedEntryChain.Begin());
    }

    /*!
       @brief   Returns iterator behind end of chain of unused entries
       @return  (UnusedEntryIterator)
     */

    UnusedEntryIterator UnusedEntriesEnd(){
        return( m_pSectionData->m_UnusedEntryChain.End());
    }

    /* -----------------------------------------------------------------------*/
    /*! @name  Iterator for partially used entry chain                        */
    /* -----------------------------------------------------------------------*/

    /*!
       @brief   Returns iterator at begin of chain of partially used entries
       @return  (PartiallyUsedEntryIterator)
     */

    PartiallyUsedEntryIterator PartiallyUsedEntriesBegin(){
        return( m_pSectionData->m_PartiallyUsedEntryChain.Begin());
    }

    /*!
       @brief   Returns iterator behind end of chain of partially used entries
       @return  (PartiallyUsedEntryIterator)
     */

    PartiallyUsedEntryIterator PartiallyUsedEntriesEnd(){
        return( m_pSectionData->m_PartiallyUsedEntryChain.End());
    }

    /* -----------------------------------------------------------------------*/
    /*! @name  Iterator for full used entry chain                             */
    /* -----------------------------------------------------------------------*/

    /*!
        @brief  Returns iterator at begin of chain of full used entries
        @return (FullEntryUsedIterator)
    */

    FullEntryUsedIterator FullUsedEntriesBegin(){
        return( m_pSectionData->m_FullUsedEntryChain.Begin());
    }

    /*!
       @brief   Returns iterator behind end of chain of full used entries
       @return  (FullEntryUsedIterator)
     */

    FullEntryUsedIterator FullUsedEntriesEnd(){
        return( m_pSectionData->m_FullUsedEntryChain.End());
    }

    /* -----------------------------------------------------------------------*/
    /*! @name  Miscellaneous                                                  */
    /* -----------------------------------------------------------------------*/

    /*!
       @brief   Increment the number of used data pages by the given pageCount.
                Default value for the pageCount is one.
                For further information see description of Converter_MapSectionData.
       @param   pageCount [in] number of used pages. Default is one.
       @return  none
    */

    void IncrementNumUsedDataPages( const SAPDB_UInt pageCount = 1 ){
        m_pSectionData->m_NumUsedDataPages += pageCount;
    }

    /*!
        @brief  Decrement the number of used data pages by the given pageCount.
                Default value for the pageCount is one.
                For further information see description of Converter_MapSectionData.
        @param  pageCount [in] number of used pages. Default is one.
        @return none
    */

    void DecrementNumUsedDataPages( const SAPDB_UInt pageCount = 1 ){
        m_pSectionData->m_NumUsedDataPages -= pageCount;
    }

    /*!
       @brief   Returns the number of used data pages. For further information
                see description of Converter_MapSectionData.
       @return  SAPDB_UInt
    */

    SAPDB_UInt GetNumUsedDataPages() const{
        return( m_pSectionData->m_NumUsedDataPages );
    }

    /*!
       @brief   Adjust max used entry index, if the given entry index
                is higher than the old maximum.
       @param   entryNo [in] map entry index
       @return  none
    */

    void CheckIncMaxUsedEntryNo( const Converter_MapEntryNo  entryNo )
    {
        if( entryNo > m_pSectionData->m_MaxUsedEntryNo )
            m_pSectionData->m_MaxUsedEntryNo = entryNo;
    }

    /*!
       @brief   Adjust max used entry index, if the given entry index
                was the highest entry index then the next lower entry
                index will be determined.
       @param   entryNo [in] map entry index
       @return  none
    */

    void CheckDecMaxDataPageNo( const Converter_MapEntryNo entryNo );

    /*!
       @brief   Returns true if the unused entry chain is empty
       @return  (SAPDB_Bool)
    */

    SAPDB_Bool IsUnusedEntryChainEmpty() const{
        return( m_pSectionData->m_UnusedEntryChain.IsEmpty());
    }

    /*!
      @brief    Returns the recovery mode of the data pages
                managed by this section
      @return  (Data_PageRecoveryMode&)
    */

    const Data_PageRecoveryMode& GetRecoveryMode() const{
        return( *( m_pSectionData->m_pRecoveryMode ));
    }

    /*!
      @brief    Returns the sectionId of this section.
      @return  (SAPDB_UInt)
    */

    SAPDB_UInt GetSectionNo() const{
        return( m_pSectionData->m_SectionNo );
    }

private:

#   ifdef SAPDB_QUICK

    const RTE_TaskId  GetTaskId() const{
        return RTETask_ITask::Self()->ID();
    }

#   endif


protected:

    /// pointer to corresponding map section data

    Converter_MapSectionData    *m_pSectionData;

    /// Identification of the task holding the section object

    RTE_TaskId  m_TaskId;

    /// state of lock

    SAPDB_Bool  m_bIsLocked;

    /// enable check executed during object destruction
    /// or unlock operation

    SAPDB_Bool  m_bUnlockWithCheck;
};

#endif  /* CONVERTER_MAP_SECTION_HPP */
