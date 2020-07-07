/*!
  @file           RTEMem_PageAllocator.hpp
  @author         JoergM
  @ingroup        Runtime
  @brief          Singleton for I/O Page allocation

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
#ifndef RTEMEM_PAGEALLOCATOR_HPP
#define RTEMEM_PAGEALLOCATOR_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/SAPDB_Singleton.hpp"
#include "RunTime/Synchronisation/RTESync_NamedSpinlock.hpp"

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
  @class    RTEMem_PageAllocator
  @brief    Handle single I/O pages

  This allocator only allows to request and release single database pages pages. This enables a very simple
  freelist handling. The allocator is filled using an AddMemoryBlock memnber function, like SAPDBMem_PageAllocator.
  Since SAPDBMem_PageAllocator is not multi thread save its allocation and deallocation couldn't be used.
 */
class RTEMem_PageAllocator : public SAPDBMem_IRawAllocator
{
public:
    /*!
      @brief Initialize the RTEMem_PageAllocator instance

      @param dbPageSize [in] database page size in bytes
     */
    static void Initialize( SAPDB_Int4 dbPageSize );

    /*!
      @brief Return reference to single instance of RTEMem_PageAllocator

      The Initialize() function must have been called before.
      This is necessary since the database page size is only known by the caller
      of the Initialize() function
    
      @return Reference to SAPDBMem_PageAllocator instance
     */
    static RTEMem_PageAllocator & Instance( );

    /*!
       @brief   This method lets an memory block be controlled by the page
                allocator. The page allocator splits the memory block into
                smaller pieces of the page size.
       @param   Ptr [in] pointer to the given memory block
       @param   Len [in] number of bytes belonging to the given memory block
       @return  none
     */
    void AddMemoryBlock(void* Ptr, SAPDB_ULong Len);

    /*!
      @brief allocate a single database page

      The given number of bytes must match pagesize
    
      @return the pointer to the page or 0 if no memory available
     */
    void* Allocate(SAPDB_ULong ByteCount);

    /*!
      @brief allocate a single database page

      The given number of bytes must match pagesize
    
      @return the pointer to the page or 0 if no memory available
     */
    virtual void* Allocate(SAPDB_ULong ByteCount, const void* Hint)
    {
        return Allocate(ByteCount); 
    }

    /*!
      @brief deallocate a single database page

      @param pDatabasePage [in] the pointer to the page
     */
    virtual void Deallocate(void* pDatabasePage);

   /*!
      @brief  Outputs statistics about the underlying base allocator.
      Since the memory blocks are inserted from outside, but no allocated is used, 0 is returned.

      @param  CountAlloc [out] always 0
      @param  CountDealloc [out] always 0
    */
    virtual void GetBaseAllocatorCallStatistics(SAPDB_ULong &CountAlloc,
                                  SAPDB_ULong &CountDealloc) const
    {
        CountAlloc   = 0;
        CountDealloc = 0;
    }

    /*!
      @brief   Outputs statistics about the number of Allocate and Deallocate calls.
      @param          CountAlloc [out] The number of calls of method Allocate.
      @param          CountDealloc [out] The number of calls of method Deallocate.
      @return         none

    */
    virtual void GetCallStatistics(SAPDB_ULong &CountAlloc,
                                   SAPDB_ULong &CountDealloc) const
    {
        CountAlloc   = m_CountAlloc;
        CountDealloc = m_CountDealloc;
    }

    /*!
      @brief  Returns the number of failed alloc calls 
      @return number of failed allocs
    */
    virtual SAPDB_ULong GetFailedAllocCount() const
    {
        return m_CountError;
    }

    /*!
       @brief          Returns the number of free pages, that is the number of
                       subsequent sucessful calls to Allocate().
       @return         SAPDB_ULong
     */
    SAPDB_ULong GetFreePages() const
    {
        return m_FreePages; 
    }

#ifdef AIX
    /*!----------------------------------------------------------------------
    @brief Resets the m_Instance 
    -----------------------------------------------------------------------*/
    static void Reset()
    {
        m_Instance = 0;
    }
#endif

private:
    /*!
    @ brief defines a page header
    */
    struct Page {
        Page *NextPage;      //!< pointer to previous page
        Page *PreviousPage;  //!< pointer to next page
    };
    SAPDB_ULong m_PageSize;                     //!< The size of one page.
    SAPDB_ULong m_FreePages;                    //!< The number of free pages.
    Page *m_FirstFreePagePtr;                   //!< The address of the first free page in the chain.
    SAPDB_ULong m_CountAlloc;                   //!< number of Allocate calls.
    SAPDB_ULong m_CountDealloc;                 //!< number of Deallocate calls.     
    SAPDB_ULong m_CountError;                   //!< number of failed calls
    RTESync_NamedSpinlock m_freeBlockChainLock; //!< spinlock used to protect free block chain management

    /*!
      @brief constructor
      registers spinlock and initalize lokal statistic. Allocator is not operatable until 
      memory block habe been added.
     */
    RTEMem_PageAllocator(SAPDB_ULong pageSize)
    : m_PageSize(pageSize)
    , m_FreePages(0)
    , m_FirstFreePagePtr(0)
    , m_CountAlloc(0)
    , m_CountDealloc(0)
    , m_CountError(0)
    , m_freeBlockChainLock((const SAPDB_UTF8 *)"RTEMem_PageAllocator")
    {}

    /*!
    @brief empty destructor (memory was inserted from outside...)
    */
    virtual ~RTEMem_PageAllocator() {}

    SAPDB_Singleton m_ThisIsASingleton; ///< prevents copy and assignment

    static RTEMem_PageAllocator *m_Instance;
  // Using a static reference would mean, that initialization function is called before main
  // This we have to prevent for the same reason we prevent static class instances
  // -> dynamic linked RunTime would not get initialized correctly on some OS...
};

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/



#endif  /* RTEMEM_PAGEALLOCATOR_HPP */
