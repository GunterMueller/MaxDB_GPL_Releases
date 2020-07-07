/*!
  @file           SAPDBMem_IncrementalBufferAllocator.hpp
  @author         Stefan Baier
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


#ifndef SAPDBMem_IncrementalBufferAllocator_HPP
#define SAPDBMem_IncrementalBufferAllocator_HPP


#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"


/*!
   @class          SAPDBMem_IncrementalBufferAllocator
   @brief          Simple implementation of the 'SAPDBMem_IRawAllocator'
                   interface.

                   This class manages a (large) memory block and allocates smaller
                   blocks inside in an aligned way.  It can only free the large
                   memory block as a whole.
                   The user of this allocator has to take care about the memory
                   that comes with the arguments in the constructor.

 */


class SAPDBMem_IncrementalBufferAllocator
    : public SAPDBMem_IRawAllocator
{
    friend class SAPDBMem_IncrementalRawAllocator;

protected:
    SAPDB_Byte* m_Memory;       //!< Address of the memory block managed by this allocator
    SAPDB_ULong m_Size;         //!< Length of the memory block in bytes
    SAPDB_ULong m_MaxUsedSize;  //!< Max used size
    SAPDB_Byte* m_FreeMemory;   //!< Address of the free memory block
                                //!< This address is always in the range m_Memory...m_Memory+m_Capacity-1
    SAPDB_ULong m_FreeSize;     //!< Length of the free memory in bytes
    SAPDB_Byte* m_LastPointer;  //!< Hint for reallocating memory
    SAPDB_ULong m_LastSize;     //!< Hint for reallocating memory
    SAPDB_ULong m_TotalAllocatedSize;    //!< The total size of the memory allocated from the user.
    SAPDB_ULong m_CountAlloc;            //!< number of calls of method Allocate
    SAPDB_ULong m_CountDealloc;          //!< number of calls of method Deallocate
public:
    /*!
       @brief          Constructs an raw allocator with an initial memory block.
       @param          Address [in] The address of the memory block.Address must be aligned.
       @param          Length [in] The length of the memory block.
       @param          Align [in] If true, the first allocated chunk will be aligned to
                                  the SAPDBMem_IRawAllocator alignment. If false, the caller is
                                  responsible for correct alignment.
     */

    SAPDBMem_IncrementalBufferAllocator(void* Address, SAPDB_ULong Length, bool Align = true);
    
    /*!
       @brief  destructor, does nothing.
    */

    virtual ~SAPDBMem_IncrementalBufferAllocator() {}

    /*!
       @brief Checks, if the allocator manages a valid memory block.
       @return bool
    */

    bool IsAssigned()
        { return ((m_Memory != 0) && (m_Size != 0)); }

    /*!
       @see SAPDBMem_IRawAllocator::Allocate
     */

    virtual void* Allocate(SAPDB_ULong ByteCount) ;

    /*!
       @see SAPDBMem_IRawAllocator::Allocate
     */
    
    virtual void* Allocate(SAPDB_ULong ByteCount, const void* Hint);

    /*!
       @see SAPDBMem_IRawAllocator::Deallocate
     */

    virtual void Deallocate(void* Ptr);

    /*!
       @see SAPDBMem_IRawAllocator::CheckConsistency
    */

    virtual bool CheckConsistency();

    /*!
       @see SAPDBMem_IRawAllocator::CalcStatistics
     */

    virtual void CalcStatistics(SAPDB_ULong &BytesUsed,
                                SAPDB_ULong &MaxBytesUsed,
                                SAPDB_ULong &BytesControlled);

    /*!
       @brief   Frees the total memory controlled by this allocator.
       @return  none
     */

    void FreeAll();

    /*!
       @see SAPDBMem_IRawAllocator::GetBaseAllocatorCallStatistics
    */


    virtual void GetBaseAllocatorCallStatistics(SAPDB_ULong &CountAlloc,
                                  SAPDB_ULong &CountDealloc) const;

    /*!
       @see SAPDBMem_IRawAllocator::GetCallStatistics
     */

 
    virtual void GetCallStatistics(SAPDB_ULong &CountAlloc,
                                   SAPDB_ULong &CountDealloc) const;

    /*!
       @brief  Returns the size of bytes, that can be allocated with the
               next call to Allocate().

       @return SAPDB_ULong
     */

    SAPDB_ULong GetCapacity()
        { return m_FreeSize; }


    /*!
       @brief          Returns the number of errors found and corrected by the allocator.
                       This allocator is not able to provide the information, i.e. returns 0
       @return         0
     */

  
private:
    /*!
    @brief Private copy constructor avoids copying.
    */
    SAPDBMem_IncrementalBufferAllocator(const SAPDBMem_IncrementalBufferAllocator &)
        {}

    /*!
    @brief private assignment operator avoids copying.
    */
    SAPDBMem_IncrementalBufferAllocator operator=(SAPDBMem_IncrementalBufferAllocator &)
        { return *this; }

    /*!
    @brief Initialize all members.
    */
    void Initialize(void* Address, SAPDB_ULong Length, bool Align = true);

};


#endif // SAPDBMem_IncrementalBufferAllocator_HPP

