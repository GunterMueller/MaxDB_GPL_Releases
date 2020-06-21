/*!
  @file           SAPDBMem_IncrementalRawAllocator.hpp
  @author         ThomasA
  @ingroup        Memory Management
  @brief          Simple implementation of 'SAPDBMem_IRawAllocator' interface.

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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

#ifndef SAPDBMem_IncrementalRawAllocator_HPP
#define SAPDBMem_IncrementalRawAllocator_HPP


#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IncrementalBufferAllocator.hpp"


/*!
  @class   SAPDBMem_IncrementalRawAllocator
  @brief   Implementation of the 'SAPDBMem_IRawAllocator' interface.

           This class manages large memory blocks obtained from a raw 
           allocator and allocates smaller blocks inside sequentially in 
           an aligned way. It can only free the large memory block as a 
           whole.
*/

class SAPDBMem_IncrementalRawAllocator : public SAPDBMem_IRawAllocator
{
private:
    SAPDBMem_IRawAllocator& m_RawAllocator;     //!< The underlying raw allocator.
    SAPDB_ULong m_MinSize;                      //!< The minimum size for each large block.
                                                //!< from the underlying raw allocator
    void *m_FirstBlock;                         //!< The address of the first block in the chain.
    SAPDB_ULong m_ActualBlockSize;              //!< The size of the last block in the chain
    void *m_ActualBlock;                        //!< The address of the last block in the chain.
    SAPDB_ULong m_HistoryUsedSize;              //!< The total size of all large blocks allocated 
                                                //!< from the underlying raw allocator without the
                                                //!< size of the actual large block.
    SAPDB_ULong m_HistoryMaxUsedSize;           //!< maximum of m_HistoryUsedSize
    SAPDB_ULong m_CountAlloc;                   //!< number of calls of method Allocate
    SAPDB_ULong m_CountDealloc;                 //!< number of calls of method Deallocate
    /*! The incremental raw allocator uses an incremental buffer allocator, which
        manages the actual block.
    */
    SAPDBMem_IncrementalBufferAllocator m_BufferAllocator;

public:
    /*!
       @brief          Constructs an incremental raw allocator with memory blocks 
                       obtained from a raw allocator.
       @param          RawAlloc [in] The underlying raw allocator.
       @param          MinSize [in] The minimum and initial size of the managed blocks.


                        This allocator manages an unlimitid number of blocks obtained 
                        from the underlying raw allocator. Within these blocks the 
                        allocator allocates smaller blocks sequentially in an aligned 
                        way. 
                        The minimum size of each block is given at construct time.
                        The allocator starts with one block of the minimum size.
                        If the remaining space within the first block is not sufficient 
                        to fulfill the next Allocate() request, additional blocks are
                        requested fomr the underlying raw allocator.
                        The maximum size depends on the argument in the Allocate() 
                        request.

     */

    SAPDBMem_IncrementalRawAllocator(
        SAPDBMem_IRawAllocator& RawAlloc,
        SAPDB_ULong MinSize
    );

    /*!
       @brief destructor          
     */

    virtual ~SAPDBMem_IncrementalRawAllocator();

    /*!
       @brief  alloccates memory
       @see  SAPDBMem_IRawAllocator::Allocate
     */

    virtual void* Allocate(SAPDB_ULong ByteCount);

    /*!
       @brief  alloccates memory
       @see  SAPDBMem_IRawAllocator::Allocate
     */

    virtual void* Allocate(SAPDB_ULong ByteCount, const void* Hint);

    /*!
       @brief frees memory  
       @see  SAPDBMem_IRawAllocator::Deallocate
     */

    virtual void Deallocate(void* p) {}

    /*!
       @brief check allocator consistency
       @see   SAPDBMem_IRawAllocator::CheckConsistency
    */

    virtual bool CheckConsistency();

	/*!
    @brief  yields base allocator statistics
    @see    SAPDBMem_IAllocatorInfo::GetBaseAllocatorCallStatistics
    */

    virtual void GetBaseAllocatorCallStatistics(SAPDB_ULong &CountAlloc,
                                  SAPDB_ULong &CountDealloc) const;

    /*!
       @brief yields allocator statistics
       @see    SAPDBMem_IAllocatorInfo::CalcStatistics
    */

    virtual void CalcStatistics(SAPDB_ULong &BytesUsed,
                                SAPDB_ULong &MaxBytesUsed,
                                SAPDB_ULong &BytesControlled);
    
    /*!
       @brief  yields allocator call statistics
       @see    SAPDBMem_IAllocatorInfo::GetCallStatistics
     */

    virtual void GetCallStatistics(SAPDB_ULong &CountAlloc,
                                   SAPDB_ULong &CountDealloc) const;

    /*!
       @brief  Frees the total memory controlled by this allocator.
       @return none
     */

    void FreeAll();

private:
    /*!
    @brief Private copy constructor avoids copying.
    */
    SAPDBMem_IncrementalRawAllocator(const SAPDBMem_IncrementalRawAllocator &InitAlloc)
     : m_RawAllocator(InitAlloc.m_RawAllocator), 
        m_BufferAllocator(0, 0)
    {}

    /*!
    @brief Private assignment operator avoids copying.
    */
    SAPDBMem_IncrementalRawAllocator operator=(SAPDBMem_IncrementalRawAllocator &)
        { return *this; }

    /*!
    @brief Get a new block from the raw allocator and chain it.
    @param Size [in] The size of the new block.
    */
    
    void* GetNewBlock(SAPDB_ULong Size);

    /*!
    @brief Initialize the buffer allocator with a memory block.
    @param Block [in] The address of the new memory block.
    @param BlockSize [in] The size of the memory block.
    */
    void InitializeBufferAllocator(void *Block, SAPDB_ULong BlockSize);
};



#endif // SAPDBMem_IncrementalRawAllocator_HPP

