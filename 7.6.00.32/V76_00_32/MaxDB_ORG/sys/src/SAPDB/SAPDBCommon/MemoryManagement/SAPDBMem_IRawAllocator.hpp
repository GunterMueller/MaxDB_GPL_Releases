/*!
  @defgroup       Memory management
*/

/*!
  @file           SAPDBMem_IRawAllocator.hpp
  @author         ThomasA
  @brief          definition of the interface of an raw allocator
 
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


#ifndef SAPDBMem_IRawAllocator_HPP
#define SAPDBMem_IRawAllocator_HPP

#include "SAPDBCommon/MemoryManagement/SAPDBMem_IAllocatorInfo.hpp"

/*!
  @interface      SAPDBMem_IRawAllocator
  @brief          Allocator interface for 'raw' memory.


                Classes implementing this interface provide methods for reserving
                and freeing 'raw' memory. The reserved memory always is suitably
                (platform dependent) aligned such that any object type can be put
                there. Implementations shall meet the requirements of the C++
                Standard, par.3.7.3 'Dynamic storage duration
                [basic.std.dynamic]'.
                Every class implementing this interface must assure, that the
                copy constructor and the assignment operator is inaccessible.

 */


class SAPDBMem_IRawAllocator : public SAPDBMem_IAllocatorInfo 
{
public: 
   /*!
      @brief         defines minimum alignment of memory blocks returned by an allocator.
   */

   struct AlignType {
      union { void* p; long l; double d; }; 
   };
   

   /*!
      @brief          Returns the alignment size in bytes of this allocator.
      @return         SAPDB_ULong

    */

   static SAPDB_ULong GetAlignmentSize()
      { return SAPDB_ALIGNMENT; }

   /*!
      @brief          Returns the value for an invalid size.
                      The is defined as 0xFFFF... .
      @return         SAPDB_ULong

    */

   static SAPDB_ULong GetInvalidSize()
      { return (((SAPDB_ULong)((SAPDB_Long)(-1)))); }

   /*!
      @brief          Returns the offset of &lt;Address> to the next address which is 
                      greater than &lt;Address> and is aligned.
      @return         SAPDB_ULong


                    This offset can be used, to get an aligned address with the 
                    formula 
                       NewAddress = Address + Offset;

    */

   static SAPDB_ULong CalcAlignmentOffset(void *Address)
   {
      return static_cast<SAPDB_ULong>(GetAlignmentSize()-
                                      (reinterpret_cast<SAPDB_Byte*>(Address)- 
                                       reinterpret_cast<SAPDB_Byte*>(0)) 
                                      % GetAlignmentSize());
   }

   /*!
      @brief          Checks, if the given address is aligned.
      @return         bool

    */

   static bool IsAligned(void *Address)
   {
      return CalcAlignmentOffset(Address) == GetAlignmentSize();
   }

   /*!
      @brief          Does nothing

    */

   virtual ~SAPDBMem_IRawAllocator() {}

   /*!
      @brief          Allocates contiguous memory for &lt;ByteCount> bytes.
              The memory is aligned as indicated by the 'GetAlignmentSize' 
              method.
              The memory is not initialised.
      @param          ByteCount [in] The size in bytes of the memory to be allocated. This 
                      argument must be greater zero.
      @return         void*
                      A pointer to the allocated raw memory block of &lt;ByteCount> 
                      bytes. If exception handling is disabled, a NULL pointer will
                      be re turned in case of insufficient memory.
      @exception      From 'SAPDBMem_IRawAllocator' derived classes may throw a
                      'SAPDBMem_BadAllocException' if the allocation fails.

    */

   virtual void* Allocate(SAPDB_ULong ByteCount) = 0;

    /*!
      @brief          Allocates contiguous memory for &lt;ByteCount> bytes.
              The memory is aligned as indicated by the 'GetAlignmentSize' 
              method.
              The memory is not initialised. 
              This method assures, that no exception will be thrown.
              The default implementation just call the Allocate method. Allocators, which may throw
              an exception there, must overwrite this implementation !
      @param          ByteCount [in] The size in bytes of the memory to be allocated. This 
                      argument must be greater zero.
      @return         void*
                      A pointer to the allocated raw memory block of &lt;ByteCount> 
                      bytes. If exception handling is disabled, a NULL pointer will
                      be re turned in case of insufficient memory.
      @exception      None

    */

   virtual void* AllocateThrowsNothing(SAPDB_ULong ByteCount)
   {
       return this->Allocate(ByteCount);
   }


   /*!
      @brief          works like realloc
      @param          ByteCount [in] The size in bytes of the memory to be allocated. This 
                      argument must be greater zero.
      @param          Hint [in] If possible, the allocator tries to reserve the memory at
                      the same position as given by Hint. This is useful for
                      resizing a memory block. If the reallocation was succesfull
                      the return value and Hint are identical.
      @return         void*
                      A pointer to the allocated raw memory block of &lt;ByteCount> 
                      bytes. If exception handling is disabled, a NULL pointer will
                      be re turned in case of insufficient memory.          
   */

   virtual void* Allocate(SAPDB_ULong ByteCount, const void* Hint) = 0;

    /*!
      @brief        tells the allocator that it will be used by the given task exclusively. This is used
                    for checking unsynchronized allocators against concurrent usage. 
      @param        TaskId [in] the task identification of the task that is going to use the allocator
     */

   virtual void AssignToTask(SAPDB_UInt4 TaskId) {}

   /*!
      @brief          Frees the memory behind &lt;p>.
      @param          p [in] 
      @return         none


                    The memory must have been allocated by a previous call to
                    'Allocate'. This is checked by assertions.
                    !!! An implementation of this method may not throw an 
                    exception !!!

    */

   virtual void Deallocate(void* p) = 0;

   /*!
      @brief          Checks the integrity of the allocator.
      @return         bool (only in the non-exception case)
      @exception      SAPDBMem_ConsistencyException 


                    This function checks the inner state of the allocator 
                    concerning consistency. Therefore, the total memory controlled 
                    by this allocator is checked.
                    If an inconsistency is detected, an excpetion is thrown.
                    The default implementation does nothing.

    */

   virtual bool CheckConsistency() { return true; }

   /*!
      @brief          Checks that allows message output routines to ignore this allocator
      
      The message output routine needs memory. If a problem occurs inside a locked section of the
      allocator it would lead to deadlock if the message output would try to use it to show the
      message. Therefore the message output routine uses this function to decide to switch to its 
      own 'Emergency allocator'...

      @return         bool (if true only message output code will change its behaviour...)

      The default implementation does always indicates usabilaty for message output

    */

   virtual bool DoNotUseForMessageOutputAllocation() const { return false; }

   /*!
      @brief          returns the size af a chunk.
      @param          p [int] The address of the chunk. This must be the result of
                      a previous call of the Allocate method of the allocator.
                      The default implementation returns GetInvalidSize().
      @return         the internal size of the chunk in bytes. The size may differ
              from the required size of the corresponding Allocate call.

    */


   virtual SAPDB_ULong GetChunkSize(const void* p)
   {
       return GetInvalidSize();
   }

   /*!
      @brief returns the maximal size that may be requestet via the Allocate method
    */
   virtual SAPDB_ULong GetMaxAllocatableSize() const
   {
       return 0xFFFFFFFF;
   }
   /*!
      @brief          Compares two raw allocator instances.
      @param          Other [in] The other instance to compare with.
      @return         bool


                    Two instances of raw allocators are equal, if they lie at 
                    the same memory position.

    */

   bool operator ==(SAPDBMem_IRawAllocator& Other)
      { return (this == &Other); }

   /*!
      @brief          Returns the opposite of operator ==.
      @return         bool

    */

   bool operator !=(SAPDBMem_IRawAllocator& Other)
      { return !(operator==(Other)); }
};




/*!
   @brief          Macro used to allocate static space aligned properly
   @param          _name [in] Name of variable used i.e. in a later placement new
   @param          _size_in_bytes [in] Number of bytes needed
   @code           
    
    
    SAPDBMEM_STATIC_RAW_ALLOCATE(Space, sizeof(RTEMem_RawAllocator));
    
    m_Instance = new (Space) RTEMem_RawAllocator();
    @endcode        

                If a modules needs space even before any allocator is available, 
                this macro should be used, so space is always properly aligned. 
                There is no corresponding free.

 */


#define SAPDBMEM_STATIC_RAW_ALLOCATE(_name, _size_in_bytes)                  \
   static SAPDBMem_IRawAllocator::AlignType                                  \
       _name[((_size_in_bytes)+sizeof(SAPDBMem_IRawAllocator::AlignType)-1)/ \
              sizeof(SAPDBMem_IRawAllocator::AlignType)]





#endif // SAPDBMem_IRawAllocator_HPP
