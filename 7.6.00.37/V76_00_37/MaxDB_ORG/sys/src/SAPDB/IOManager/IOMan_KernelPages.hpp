/*!
  @file         IOMan_KernelPages.hpp
  @author       TorstenS
  @ingroup      IOManagement
  @brief        This module is used to define and implement an class which 
                handles a collection of pages, whereby all pages have he same 
                properties, i.e there are from the same page type.

                The class IOMan_IKernelPagesIterator defines and implements 
                an iterator on the above mentioned class, which offers the
                facility to access the stored pages in a common way as a
                IOMan_IPage. The iterator returns the stored pages in
                the temporal sequence of there storage( FIFO).
                The template class IOMan_Pages is used to store a concrete 
                collection of pages. This class is responsible to get the 
                page type individual memory for the page handler. The above
                mentioned class IOMan_KernelIPages deals with structures
                pointing into this class. Note that there is a very tight 
                connection between IOMan_KernelIPages and IOMan_Pages.
                The template class IOMan_PagesIterator defines and implements 
                an iterator on the above mentioned class, which offers the
                facility to access the stored pages in a concrete way for 
                example as data, log or converter pages.The iterator returns 
                the stored pages in the temporal sequence of there storage( FIFO).

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


#ifndef IOMAN_KERNELPAGES_HPP
#define IOMAN_KERNELPAGES_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"

#include "Container/Container_Vector.hpp"
#include "KernelCommon/Kernel_Common.hpp"
#include "IOManager/IOMan_IPage.hpp"
#include "DataAccess/Data_PageFrame.hpp"
#include "RunTime/MemoryManagement/RTEMem_AllocatorWrapper.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

// forward declaraion

class IOMan_IKernelPagesIterator;
template <class PAGE> class IOMan_PagesIterator;


/*!
  @interface    IOMan_IKernelPages
  @brief        This module is used to define and implement an class which 
                handles a collection of pages, whereby all pages have he same 
                properties, i.e there are from the same page type.
 */

class IOMan_IKernelPages
{
    friend class IOMan_IKernelPagesIterator;
public:

    /*! @name Common container access */
    /*@{*/

    /// defines the first item index in the page set.
    static SAPDB_UInt FirstIndex(){
        return 0;
    }
    /*!
       @brief   This method is used to clear the current page set, i.e the 
                internal counters of this class are reinitialized and the
                destructors of the stored pages are called! Furthermore no
                memory allocated before with the given allocator is released.
       @return  none
     */
    void Clear()
    {
        m_KernelPagePointers.Clear();
        m_FramePointers.Clear();
    }

    /*!
       @brief   This method is used to get a pointer onto a list of pointers
                containing the addresses of the page frames(!) and not the 
                page handler managed by this page set. if no index is given
                than the full list is returned else the index identifies the
                start offset of the returned pointer list. Note that index 
                value one specifies the second item within the page set!
       @param   index [in] identifies the n-th item; beginning with 0 
       @return  tsp00_PageAddr* of the first page frame.
     */

    tsp00_PageAddr* GetFrameList( const SAPDB_UInt index = IOMan_IKernelPages::FirstIndex())
    {
        SAPDBERR_ASSERT_STATE( index <= ( this->GetPageCount() -1 ));
        return reinterpret_cast< tsp00_PageAddr* >( &m_FramePointers[ index ] );
    }

    /*!
       @brief   This method is used to get the current number of stored pages.
       @return  Page count
     */
    SAPDB_UInt GetPageCount() const{
        return m_KernelPagePointers.GetSize();
    }

    /*!
       @brief   This method is used to check whether the page set is empty.
       @return  true if page set is empty; else false.
     */
    bool IsEmpty() const{
        return m_KernelPagePointers.IsEmpty();
    }

    /*!
       @brief   This method is used to check whether the page set is able to
                store another page.
       @return  true if the maximum capacity is reached; else false.
     */
    bool IsFull() const{
        return m_KernelPagePointers.IsFull();
    }

    /*@}*/

protected:

    /*! @name Protected constructors and initialization */
    /*@{*/

    /*!
       @brief   The internal member data are initialized, but no memory is
                allocated with the given allocator. This will be done with the
                method Initialize.
       @param   allocator [in] allocator
       @return  none
     */
    IOMan_IKernelPages( SAPDBMem_IRawAllocator& allocator )
    :
    m_KernelPagePointers( allocator ),
    m_FramePointers( allocator )
    {}

    /*!
       @brief   This method is used to initialize the page set, i.e to get
                the memory for pages and the frame pointer list via the 
                given allocator.
       @param   maxPages [in] Capacity of the page set
       @return  true if the initialization is succesful; else false
     */
    bool Initialize( const SAPDB_Int2  maxPages )
    {
        if(  m_KernelPagePointers.Reserve( maxPages )){
            if(  m_FramePointers.Reserve( maxPages )){
                return true;
            }
        }
        this->Delete();
        return( false );
    }
    /*!
       @brief   The destructor will release all memory allocated before with
                the given allocator and will call the destructors of the 
                pages stored within the page set.
       @return  none.
     */
    ~IOMan_IKernelPages(){
        this->Delete();
    }

    /*!
       @brief   This method is used to delete all internal structures and
                to deallocate the memory.
       @return  none
     */
    void Delete()
    {
        m_KernelPagePointers.Delete();
        m_FramePointers.Delete();
    }

    /*@}*/

    /*! @name Common access */
    /*@{*/

    /*!
       @brief   This method is used to insert another page into the page set.
                The insertion sequence is the sequence in which the pages are 
                stored within the collection and therefore the sequence in which 
                an iterator operates on this set. The caller has to ensure that 
                the container is not full. Note that at least the destructor 
                of this class or the clear method will call the destructor for 
                each stored page.
       @param   page       [in] Page quantified for I/O management
        @return  none
     */
    void Append( IOMan_IPage&    page )
    {
        SAPDBERR_ASSERT_STATE( ! this->IsFull() );

        m_KernelPagePointers.InsertEnd( &page );
        m_FramePointers.InsertEnd( page.GetFramePtr());
    }

    /*@}*/

private:

    // Collection of page pointer of one page type.
    Container_Vector< IOMan_IPage* >  m_KernelPagePointers;

    // Collection of the pure page frame pointer of the pages
    // handled by m_PageList. Each item of the m_FramePointers
    // corresponds with an item in the m_PageList collection.
    Container_Vector< Data_PageFrame::Pointer >   m_FramePointers;
};

/*!
   @interface   IOMan_IKernelPagesIterator
   @brief       The class IOMan_IKernelPagesIterator defines and implements 
                an iterator on the above mentioned class, which offers the
                facility to access the stored pages in a common way as a
                IOMan_IPage. The iterator returns the stored pages in the 
                temporal sequence of there storage( FIFO).
 */

class IOMan_IKernelPagesIterator
{
public:

    /*! @name Constructors and initialization */
    /*@{*/

    /*!
       @brief   Constructor
       @param   pages [in] Collection of pages
       @return  none
     */
    IOMan_IKernelPagesIterator( IOMan_IKernelPages& pages )
    :
    m_Pages( pages ),
    m_Index( IOMan_IKernelPages::FirstIndex())
    {}

    /*@}*/

    /*! @name Iterator access */
    /*@{*/
    /*!
       @brief   This method is used to make the first page (index = 0) within 
                the set to the current page for the iterator. If an index is 
                given this index identifies the page which will be the first 
                one. There is no check whether the page is valid.
       @param   index [in] Offset of the first page. Default is zero.
       @return  none
     */
    void Begin( const SAPDB_UInt  index = IOMan_IKernelPages::FirstIndex()){
        m_Index = index;
    }

    /*!
       @brief   This method is used to report whether the current page 
                pointed by the iterator is valid.
       @return  true means that the current page is not valid and that the end
                of the page set is reached; else false
     */
    bool End() const{
        return( m_Index >= m_Pages.GetPageCount());
    }
    
    /*!
        @brief This method returns the index that belongs to the interator
        @return (SAPDB_UInt)
    */
    SAPDB_UInt GetIndex() const{
        return m_Index;
    }
    /*!
       @brief   This method is used to set the iterator to the next page within
                the page collection.
       @return  none
     */
    void operator++(){
        ++m_Index;
    }

    /*!
       @brief   This method is used to get access to the page pointed by the
                iterator.
       @return  (IOMan_IPage&amp;)
     */
    IOMan_IPage& operator*(){
        return( *m_Pages.m_KernelPagePointers[ m_Index ] );
    }

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/

private:

    // Reference of the page collection this iterator works on.
    IOMan_IKernelPages& m_Pages;

    // Index within the page collection administrated by the class IOMan_IKernelPages
    SAPDB_UInt  m_Index;
};

/*!
  @class    IOMan_Pages
  @brief    The template class IOMan_Pages is used to store a concrete 
            collection of pages. This class is responsible to get the 
            page type individual memory for the page handler. The above
            mentioned class IOMan_KernelIPages deals with structures
            pointing into this class. Note that there is a very tight 
            connection between IOMan_KernelIPages and IOMan_Pages.
 */

template <class PAGE>
class IOMan_Pages : public IOMan_IKernelPages
{
    /// Define PageIterator as a friend
    friend class IOMan_PagesIterator<PAGE>;

public:

    /*! @name Constructors and initialization */
    /*@{*/

    /*!
       @brief   The internal member data are initialized, but no memory is
                allocated with the given allocator. This will be done with the
                method Initialize.
       @param   allocator  [in] Allocator
       @param   identifier [in] Identifier for the given allocator to support
                                memory allocation monitoring
       @return  none
     */
    IOMan_Pages(
        SAPDBMem_IRawAllocator& allocator,
        SAPDB_UTF8*             identifier )
    :
    m_Allocator( identifier, allocator ),
    IOMan_IKernelPages( m_Allocator ),
    m_Pages( m_Allocator ){}

    /*!
       @brief   This method is used to initialize the page set, i.e to get
                the memory for pages and the frame pointer list via the 
                given allocator.
       @param   maxPages [in] Capacity of the page set
       @return  true if the initialization is succesful; else false
     */
    bool Initialize( const SAPDB_Int2  maxPages )
    {
        if( ! m_Pages.Reserve( maxPages )){
            return( false );
        }
        if( ! IOMan_IKernelPages::Initialize( maxPages ))
        {
            Delete();
            return( false );
        }
        return( true );
    }

    /*!
       @brief   The destructor will release all memory allocated before with
                the given allocator and will call the destructors of the 
                pages stored within the page set.
       @return  none.
     */
    ~IOMan_Pages(){
        this->Delete();
    }

    /*!
       @brief   This method is used to delete all internal structures and
                to deallocate the memory.
       @return  none
     */
    void Delete()
    {
        IOMan_IKernelPages::Delete();
        m_Pages.Delete();
    }

    /*!
          @brief   This method is used to clear the current page set, i.e the 
                   internal counters of this class are reinitialized and the
                   destructors of the stored pages are called! Furthermore no
                   memory allocated before with the given allocator is released.
          @return  none
        */
    void Clear()
    {
        m_Pages.Clear();
        IOMan_IKernelPages::Clear();
    }

    /*@}*/

    /*! @name Common Acccess */
    /*@{*/

    /*!
       @brief   This method is used to insert another page into the page set.
                The insertion sequence is the sequence in which the pages are 
                stored within the collection and therefore the sequence in which 
                an iterator operates on this set. The caller has to ensure that 
                the container is not full. Note that at least the destructor 
                of this class or the clear method will call the destructor for 
                each stored page.
       @param   page [in] Page quantified for I/O management
       @return  none
     */
    void Append( PAGE&   page )
    {
        SAPDBERR_ASSERT_STATE( ! this->IsFull());

        m_Pages.InsertEnd( page );

        // Danger: Give the copied page handler to this method and not the
        // given page handler!
        IOMan_IKernelPages::Append( *m_Pages.Position( m_Pages.GetSize() -1) );
    }

    /*!
        @brief  This method is used to get access the page which is identified by
                the given page collection index
        @return (PAGE&)
    */
    PAGE& Get( const SAPDB_UInt  index ){
        return m_Pages[ index ];
    }

    const PAGE& Get( const SAPDB_UInt  index ) const{
        return m_Pages[ index ];
    }
    
    /// returns the number of configured blocksize
    SAPDB_UInt BlockSize() const
    {
        return m_Pages.GetCapacity();
    }

    /*@}*/

protected:

    // abbreviation for page collection
    typedef Container_Vector< PAGE > Pages;
    
    // Base allocator used for all dependent memory allocation of
    // this template class. Note that this member data has to be the
    // first member data defined in this class, because the sequence
    // of the execution of the initialization lists depends on it!
    RTEMem_AllocatorWrapper  m_Allocator;

    //Collection of pages of one page type.
    Pages  m_Pages;
};

/*!
   @class   IOMan_PagesIterator
   @brief   The template class IOMan_PagesIterator defines and implements 
            an iterator on the above mentioned class, which offers the
            facility to access the stored pages in a concrete way for 
            example as data, log or converter pages. The iterator returns 
            the stored pages in the temporal sequence of there storage( FIFO).
 */

template <class PAGE>
class IOMan_PagesIterator
{
public:

    /*! @name Constructors and initialization */
    /*@{*/
    /*!
       @brief   Constructor
       @param   pages [in] Collection of pages
       @return  none
     */
    IOMan_PagesIterator( IOMan_Pages<PAGE>& pages )
    :
    m_Pages( pages ),
    m_Index(IOMan_IKernelPages::FirstIndex())
    {}
    /*@}*/
    /*! @name Iterator access */
    /*@{*/

    /*!
       @brief   This method is used to make the first page (index = 0) within 
                the set to the current page for the iterator. If an index is 
                given this index identifies the page which will be the first 
                one. There is no check whether the page is valid.
       @param   index [in] Offset of the first page. Default is zero.
       @return  none
     */
    virtual void Begin( const SAPDB_UInt  index = IOMan_IKernelPages::FirstIndex()){
        m_Index = index;
    }

    /*!
       @brief   This method is used to report whether the current page 
                pointed by the iterator is valid.
       @return  true means that the current page is not valid and that the end
                of the page set is reached; else false
     */

    virtual SAPDB_Bool End() const{
        return( m_Index >= m_Pages.GetPageCount());
    }

    /*!
       @brief   This method is used to set the iterator to the next page within
                the page collection.
       @return  none
     */
    virtual void operator++(){
        ++m_Index;
    }

    /*!
       @brief   This method is used to get access to the page pointed by the iterator.
       @return  (PAGE&)
     */
    PAGE& operator*() const{
        return( m_Pages.m_Pages[ m_Index ] );
    }

    /*!
        @brief This method returns the index that belongs to the interator
        @return (SAPDB_UInt)
    */
    SAPDB_UInt GetIndex() const{
        return m_Index;
    }
    
    /*@}*/

private:

    // Reference of the page collection this iterator works on.
    IOMan_Pages<PAGE>&  m_Pages;

    // Index within the page collection administrated by the class IOMan_Pages
    SAPDB_UInt  m_Index;
};

#endif  /* IOMAN_KERNELPAGES_HPP */
