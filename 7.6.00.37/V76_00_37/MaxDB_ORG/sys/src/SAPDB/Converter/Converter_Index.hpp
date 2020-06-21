/*!
  @file           Converter_Index.hpp
  @author         TorstenS
  @ingroup        Converter
  @brief          Parent index for management of converter index pages
*/

/*
  ========== licence begin  GPL
  Copyright (c) 2000-2004 SAP AG

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
*/

#ifndef CONVERTER_INDEX_HPP
#define CONVERTER_INDEX_HPP

#include "Container/Container_Vector.hpp"
#include "Container/Container_SortableVector.hpp"
#include "Converter/Converter_Chain.hpp"
#include "Converter/Converter_IndexEntry.hpp"
#include "Converter/Converter_LeafPage.hpp"
#include "Converter/Converter_LeafReference.hpp"

class Converter_Pager;
class Kernel_Dump;
class RTETask_ITask;

/// buffer which contains converter index entries (== the index column)
typedef Container_Vector<Converter_IndexEntry> Converter_IndexVector;


/*!
  @interface      Converter_IndexIterator
  @brief          Parent index for management of converter index pages
 */
class Converter_IndexIterator
{
    friend class Converter_Index;

public:

    /*!
       @brief          Constructor
       @param          indexiter [in] iterator at start position in index column
       @param          indexenditer [in] iterator behind end position in index column
       @return         none
     */
    Converter_IndexIterator
    (const Converter_IndexVector::Iterator  &indexiter,
     const Converter_IndexVector::Iterator  &indexenditer)
    :
    m_EntryIter    (indexiter),
    m_EntryEndIter (indexenditer),
    m_PageIter     ((*indexiter).GetPage().Begin()),
    m_PageEndIter  ((*indexiter).GetPage().End())
    {}

    /*!
       @brief          Compares with other iterator
       @param          iter [in] other
       @return         (SAPDB_Bool) true if unequal
     */
    SAPDB_Bool operator!=(const Converter_IndexIterator &iter) const
    {
        return (m_EntryIter != iter.m_EntryIter) ||
               (m_PageIter  != iter.m_PageIter);
    }

    /*!
       @brief          Prefix increment; advances iterator
       @return         (Converter_IndexIterator&amp;) this instance
     */
    Converter_IndexIterator& operator++()
    {
        SAPDBERR_ASSERT_STATE(CanAdvance());

        // jump to next converter entry on the current page
        ++m_PageIter;

        // check if we have reached index page end
        if( m_PageIter == m_PageEndIter )
        {
            // jump to next converter index page
            ++m_EntryIter;
            while(CanAdvance() && !((*m_EntryIter).PageIsAssigned())) {
                ++m_EntryIter;
            }

            // check if we have not reached last index entry
            if (CanAdvance())
            {
                // reset page iterators
                Converter_IndexPage &indexpage = (*m_EntryIter).GetPage();
                m_PageIter                     = indexpage.Begin();
                m_PageEndIter                  = indexpage.End();
            }
        }
        return *this;
    }

    /*!
       @brief          Checks whether iterator can advance with operator++()
       @return         (SAPDB_Bool) true if iter can advance
     */
    SAPDB_Bool CanAdvance() const
    {
        return m_EntryIter != m_EntryEndIter;
    }

    /*!
       @brief          Returns the block address the iterator points to
       @return         (IOMan_BlockAddress) actual block address
     */
    IOMan_BlockAddress GetBlockAddress() const
    {
        return m_PageIter.GetBlockAddress();
    }

    /*!
       @brief          Checks whether actual entry is in use
       @return         (SAPDB_Bool) true if entry is in use
     */
    SAPDB_Bool EntryInUse() const
    {
        return m_PageIter.EntryInUse();
    }

protected:

    /// iterator over all entries in one page
    Converter_IndexVector::Iterator m_EntryIter;

    /// iterator end
    Converter_IndexVector::Iterator m_EntryEndIter;

    /// iterator over all pages
    Converter_PageEntryIterator     m_PageIter;

    /// iterator end
    Converter_PageEntryIterator     m_PageEndIter;
};


/*!
   @interface      Converter_Index
   @brief          Converter parent index

   - Note, that the index tree consists always of two index an one leaf level
 */
class Converter_Index
{

public:

    /// a index entry object (== slot of the index column)
    typedef Converter_IndexEntry                            IndexEntry;

    /// any number of index entries
    typedef Converter_IndexEntryCount                       IndexEntryCount;

    /// index of a certain index entry
    typedef Converter_IndexEntryNo                          IndexEntryNo;

    /// a converter leaf page handler object
    typedef Converter_LeafPage                              LeafPage;

    /// a converter index page handler object
    typedef Converter_IndexPage                             IndexPage;

    /// any number of page entries on a converter page
    typedef Converter_PageEntryCount                        PageEntryCount;

    /// index of a certain page entry on a converter leaf page
    typedef Converter_PageEntryNo                           PageEntryNo;

    /// standard iterator for the whole index
    typedef Converter_IndexIterator                         IndexIterator;

private:


    /// iterator for converter pages
    typedef Converter_PageEntryIterator                     PageEntryIterator;

    /// doubly linked chain of map entries
    typedef Converter_Chain
    <Converter_IndexVector, Converter_IndexEntry>   EntryChain;

public:


    /// a converter index column iterator
    typedef Converter_IndexVector::Iterator                 EntryIterator;


    /// iterator of partially used entries
    typedef EntryChain::Iterator                            PartiallyUsedEntryIterator;


    /// resource type used for dynamic memory allocation
    typedef Converter_IndexVector::Allocator                Allocator;

    /// queue of converter block numbers
    typedef Container_SortableVector<Converter_LeafReference> BlockVector;


    /* -----------------------------------------------------------------------*/

    /*! @name Constructors and initialization */

    /*!
       @brief          Default constructor
       @param          alloc [in] allocator for dynamic index resources
       @return         none
     */
    Converter_Index
    (Allocator&                     alloc)
    :
    m_EntryVector                  (alloc),
    m_PartiallyUsedEntryChain      (m_EntryVector),
    m_FullEntryChain               (m_EntryVector),
    m_EmptyEntryChain              (m_EntryVector),
    m_Allocator                    (alloc)
    {}

    /*!
       @brief          Builds the initial index consisting of an empty root page
       @return         none
     */
    void Create();

    /*!
       @brief          Builds up the index
       @param          rootaddr     [in] block address of the converter root
       @param          pager    [in|out] used for converter page i/o
       @return         none

       - The index is initialized and all index pages are loaded into memory.
     */
    void Restart(
        const IOMan_BlockAddress   &rootaddr,
        Converter_Pager            &pager );

    /*!
       @brief          create sorted queues of converter pages, one queue per volume
       @param          volumeList [in,out] list of converter page queues
       @param          ConverterPageSize [in] size of one ConverterPage
       @return         none
     */
    void CreateInputQueues(
        Container_Vector<BlockVector> &volumeList,
        const SAPDB_UInt               ConverterPageSize);

    /*!
       @brief          Deletes the index and frees all pages
       @return         none

       - The index is deleted; memory is deallocated,
     */
    void Shutdown();

    /* -----------------------------------------------------------------------*/

    /*! @name Miscellaneous */

    /*!
       @brief          Flushes all index pages to device
       @param          pager    [in|out] used for converter page i/o
       @param          NumPages    [out] number of pages written
       @param          NumIO       [out] number of clustered IO transactions
       @return         (IOMan_BlockAddress) address of the index root page

       - This method is used at the end of a savepoint.
     */
    IOMan_BlockAddress Flush(
        Converter_Pager       &pager,
        SAPDB_UInt            &NumPages,
        SAPDB_UInt            &NumIO );

    /*!
       @brief          Dumps the index entries into the kernel dump file
       @param          dump [in/out] kernel dump file
       @return         none
     */
    void Dump( Kernel_Dump  &dump );

    /*!
       @brief          Checks the structure of the parent index, including the
                       free counter and the parent index chains.
       @return         (SAPDB_Bool) true is paranet index is okay else false
     */
    SAPDB_Bool Verify() const; // PTS 1113242 TS 2001-12-21

    /*!
        @brief  Checks if the given block address is already part of
                the converter index. If so true is returned else false.
        @param  block  [in] block address to be checked
        @return (SAPDB_Bool)
    */

    SAPDB_Bool IsMember( const IOMan_BlockAddress   &block );


    /* -----------------------------------------------------------------------*/

    /*! @name Index access */

    /*!
       @brief          Provides capacity to hold at least the requested number of
                       index entries
       @param          size [in] requested index capacity
       @return         (SAPDB_Bool) true if successful

       - This method may require reallocation of the index.
     */
    SAPDB_Bool Reserve
    (IndexEntryCount                size)
    {
        return m_EntryVector.Resize(size);
    }

    /*!
       @brief          Updates index data on given converter leaf page
       @param          task [in] own task
       @param          page [in] converter leaf page handler
       @return         none
     */
    void IntegratePage(
        const RTETask_ITask  &task,
        LeafPage             &page );

    /*!
       @brief          returns the parent reference associated with the given iterator
       @param          IndexIterator [in] iterator pointing to a particular entry in on a index page
       @return         Converter_Page:ParentReference : parent reference associated with the given iterator
     */
    Converter_ParentReference GetParentReference(
        const Converter_IndexIterator &IndexIterator ) const
    {
        return (Converter_ParentReference (m_EntryVector.GetIndex(IndexIterator.m_EntryIter),
                                           IndexIterator.m_PageIter.GetPosition()));
    }

    /*!
       @brief          remove all references to empty leafPages from index
       @param          task            [in] own task
       @param          snapShotVersion [in] converter version of the snapshot
       @param          parentRefs  [in|out] list of references to empty leafPages in one mapSection
       @return         none
     */
    void RemoveEmptyLeaves(
        const RTETask_ITask                         &task,
        const Converter_Version                     &snapShotVersion,
        Container_Vector<Converter_ParentReference> &parentRefs);

    /* -----------------------------------------------------------------------*/


    /*! @name Iterators */

    /*!
       @brief          Returns iterator at first level1 index page entry
       @return         (IndexIterator)

       - In contrast to other iterators, this iterator has a
         method 'CanAdvance()' to determine whether the iterator
         has reached the last page entry.
     */
    IndexIterator GetIterator()
    {
        return IndexIterator(Level1Begin(), Level1End());
    }

    /* -----------------------------------------------------------------------*/

private:

    /*-------------------------------------------------------------------------
    function:     RequestFreeParentReference()
    description:  Reserves a free parent reference on an index page

                  - An unused (free) entry is reserved and returned.
                  - If necessary, a new index page is allocated by 'frameresource'
    arguments:    frameresource [in] resource of page frames
    return value: (Converter_ParentReference) reserved parent reference
    --------------------------------------------------------------------------*/
    Converter_ParentReference RequestFreeParentReference(
       const RTETask_ITask  &task );

    /*-------------------------------------------------------------------------
    function:     Level1Begin()
    description:  Returns iterator at first level1 index entry
    arguments:    none
    return value: (EntryIterator)
    --------------------------------------------------------------------------*/
    EntryIterator Level1Begin()
    {
        return m_EntryVector.Position(Level1StartEntryNo());
    }

    /*-------------------------------------------------------------------------
    function:     Level1End()
    description:  Returns iterator behind last level1 index entry
    arguments:    none
    return value: (EntryIterator)
    --------------------------------------------------------------------------*/
    EntryIterator Level1End()
    {
        return m_EntryVector.End();
    }


    void SetRootPage(IndexPage &page);

    IndexEntryNo InsertPage (IndexPage  &page);

    static PageEntryNo RootEntryNo()
    {
        return 0;
    }

    static PageEntryNo Level1StartEntryNo()
    {
        return 1;
    }

    IndexEntry& GetRootEntry()
    {
        return m_EntryVector[RootEntryNo()];
    }

    IndexEntry& GetEntry
    (IndexEntryNo                   entryno)
    {
        return m_EntryVector[entryno];
    }


    /*--------------------------------------------------------------------------
    declaration : m_EntryVector
    description:  array of entries
    --------------------------------------------------------------------------*/
    Converter_IndexVector           m_EntryVector;
    /*--------------------------------------------------------------------------
    declaration:  m_PartiallyUsedEntryChain
    description:  doubly linked chains of partially used index entries
    --------------------------------------------------------------------------*/
    EntryChain                      m_PartiallyUsedEntryChain;
    /*--------------------------------------------------------------------------
    declaration:  m_FullEntryChain
    description:  doubly linked chains of completely filled index entries
    --------------------------------------------------------------------------*/
    EntryChain                      m_FullEntryChain;
    /*--------------------------------------------------------------------------
    declaration:  m_EmptyEntryChain
    description:  doubly linked chains of empty index entries
    --------------------------------------------------------------------------*/
    EntryChain                      m_EmptyEntryChain;

    /*--------------------------------------------------------------------------
    declaration : m_Allocator
    description:  allocator for IOQueues
    --------------------------------------------------------------------------*/
    Allocator&               m_Allocator;

};


#endif // CONVERTER_INDEX_HPP
