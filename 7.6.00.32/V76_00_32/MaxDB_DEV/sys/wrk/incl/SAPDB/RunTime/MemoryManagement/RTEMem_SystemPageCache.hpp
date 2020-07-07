/*!
  @file           RTEMem_SystemPageCache.hpp
  @author         JoergM
  @ingroup        Runtime
  @brief          Singleton for System Page allocation/buffering

  The cache uses two separated lists. Every memory block handled is either in the free block list or in the
  used block list.

  Free system memory pages are handled using block descriptors that are ordered by block size.
  For each block size group a descriptor exists, that links to the next size group.
  If a memory request comes in the corresponding 'size' descriptor is searched first and only if
  no such memory block exists and no bigger size memory block exists, the system memory
  pages are requested.

  The in used block list every memory block which was allocated and is still in use is found. This
  allows to inspect the memory block still in use, if a block is returned and thereby allows detection
  of block which are returned twice or which have never been allocated.

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

#ifndef RTEMEM_SYSTEMPAGECACHE_HPP
#define RTEMEM_SYSTEMPAGECACHE_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IBlockAllocator.hpp"
#include "SAPDBCommon/SAPDB_Singleton.hpp"
#include "RunTime/Synchronisation/RTESync_NamedSpinlock.hpp"
#include "RunTime/Synchronisation/RTESync_InterlockedCounter.hpp"

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
  @class    RTEMem_BlockDescriptor
  @brief    Element of a single linked list containing void * as payload

  A block descriptor describes a single allocated block as returned by Allocate()
  or a splinter of such a block.
  The chain elements build a single linked list. End of list is a NULL pointer
 */
class RTEMem_BlockDescriptor
{
public:
    /*!
      @brief constructor
      @param nextDescriptor [in] next element in chain
      @return 0 if no block currently managed, system page aligned block address otherwise
     */
    RTEMem_BlockDescriptor( RTEMem_BlockDescriptor *nextDescriptor )
        : m_NextDescriptor(nextDescriptor)
        , m_BlockAddress(0)
        , m_FreeBlockCount(0)
        , m_FirstSplinter(0)
        , m_NextSplinter(0)
    {}

    /*!
      @brief Reinitialize block descriptor content for reinsert into pool
      @param nextDescriptor [in] next element in chain
      @return 0 if no block currently managed, system page aligned block address otherwise
     */
    void MarkAsUnusedBlock( RTEMem_BlockDescriptor *nextDescriptor )
    {
        m_NextDescriptor = nextDescriptor;
        m_BlockAddress = 0;
        m_FreeBlockCount = 0;
        m_FirstSplinter = 0;
        m_NextSplinter = 0;
    }

    /*!
      @brief Initialize block to be used in used block chain

      While block is in used chain, its free block count is reset.
     */
    void MarkAsUsedBlock( )
    {
        m_FreeBlockCount = 0;
    }

    /*!
      @brief Initialize block to be used in free block chain

      While block is in free chain, its free block count is usable.
     */
    void MarkAsFreeBlock(SAPDB_ULong blockCount )
    {
        m_FreeBlockCount = blockCount;
    }

    /*!
      @brief Initialize block to be used in block dump chain

      @param nextDescriptor [in] the next block in block chain
      @param originalDescriptor [in] the descriptor copied from
      @param blockCount      [in] the block size to set
     */
    void MarkAsDumpBlock( RTEMem_BlockDescriptor *nextDescriptor,
                          RTEMem_BlockDescriptor *originalDescriptor,
                          SAPDB_ULong blockCount )
    {
        m_NextDescriptor = nextDescriptor;
        m_BlockAddress   = originalDescriptor->m_BlockAddress;
        m_FreeBlockCount = blockCount;
        m_FirstSplinter  = originalDescriptor->m_FirstSplinter;
        if ( m_FirstSplinter )
        {
            m_FirstSplinter = (RTEMem_BlockDescriptor *)m_FirstSplinter->m_BlockAddress;
        }
        m_NextSplinter   = originalDescriptor->m_NextSplinter;
        if ( m_NextSplinter )
        {
            m_NextSplinter = (RTEMem_BlockDescriptor *)m_NextSplinter->m_BlockAddress;
        }
    }

    /*!
      @brief Initialize block as splitted
      If a block was found in the free list for a smaller block request, the unsplitted block is splitted.
      A possible 'free' memory request on the free list can use this information, to free only those blocks
      which are not splitted. Marking a splitted block marks both the block which has been splitted and the
      splinter. If a splinter is further splitted its first splinter pointer is not set otherwise it points
      to itself. The last splinters next splinter pointer is always set to null

      @param blockWhichHasBeenSplitted [in] the block which has been splitted
      @param splittedChunkAddress [in] the starting address of the splitted chunk
      @param splittedBlockCount [in] the block size of the spliter
      @return none
     */
    void MarkAsSplittedBlock( RTEMem_BlockDescriptor * blockWhichHasBeenSplitted,
                              void *splittedChunkAddress,
                              SAPDB_ULong splittedBlockCount )
    {
        if ( !blockWhichHasBeenSplitted->m_FirstSplinter )
        {
            blockWhichHasBeenSplitted->m_FirstSplinter = blockWhichHasBeenSplitted;
        }
        m_FirstSplinter    = blockWhichHasBeenSplitted->m_FirstSplinter;
        m_NextSplinter     = blockWhichHasBeenSplitted->m_NextSplinter;
        blockWhichHasBeenSplitted->m_NextSplinter = this;
        m_BlockAddress     = splittedChunkAddress;
        m_FreeBlockCount   = splittedBlockCount;
    }

    /*!
      @brief Initialize merged block to be reused in free block chain

      If splitting fails due to out of memory problems with descriptor pool, the 
      already splitted block is merged. The spliter pointer are reset and the new block
      size is set.

      @param newBlockCount  [in] the block size to set
     */
    void MarkAsMergedBlock(SAPDB_ULong newBlockCount)
    {
        m_FirstSplinter = 0;
        m_NextSplinter = 0;
        m_FreeBlockCount = newBlockCount;
    }

    /*!
      @brief Update merged block which may still be a splinter...

      The block is merged with the given splinter. This is simply done by 
      using the m_NextSplinter of the given splinter as new next splinter
      and setting newBlockCount as free block count.

      @param mergedBlockCount   [in] the block count of the merged splinter
      @param pSplinterToAdd [in] the splinter which is merged
     */
    void MarkAsBiggerSplinter( SAPDB_ULong mergedBlockCount,
                               RTEMem_BlockDescriptor *pSplinterToAdd)
    {
        m_FreeBlockCount = mergedBlockCount;
        m_NextSplinter   = pSplinterToAdd->m_NextSplinter;
    }

    /*!
      @brief get the next descriptor in the simple linked list
      @return 0 at end of list, address of descriptor else
     */
    RTEMem_BlockDescriptor * & GetNextDescriptor()
    {
        return m_NextDescriptor; 
    }

    /*!
      @brief set the next element in chain
      @param nextDescriptor [in] next element in chain
      @return none
     */
    void SetNextDescriptor(RTEMem_BlockDescriptor *nextDescriptor)
    {
        m_NextDescriptor = nextDescriptor;
    }

    /*!
      @brief get managed block address
      @return 0 if no block currently managed
     */
    void * GetBlockAddress()
    {
        return m_BlockAddress; 
    }

    /*!
      @brief set the address of the block managed
      @param blockAddress [in] the block address to set
      @return none
     */
    void SetBlockAddress(void *blockAddress)
    {
        m_BlockAddress = blockAddress;
    }

    /*!
      @brief return pointer to first splinter

      @return 0 if block is not part of a splitted block, pointer to first splinter else
     */
    RTEMem_BlockDescriptor * GetFirstSplinter()
    {
        return m_FirstSplinter;
    }
 
    /*!
      @brief get splinter specific information of block

      @param nextSplinter  [out] the next splinter of the splitted block
      @param blockCount    [out] the splinter block size if currently free
     */
    void GetSplitBlockInformation( RTEMem_BlockDescriptor * & nextSplinter,
                                   SAPDB_ULong              & blockCount )
    {
        nextSplinter = m_NextSplinter;
        blockCount   = m_FreeBlockCount;
    }

    /*!
      @brief get free block information

      If a block was converted into a free block, the unsplittedBlockCount is the blocksize 
      and the block address is the block address which the unsplittedBlockAddress is still 0

      @param blockAddress [out] the block address set
      @param blockCount   [out] the block size
      @return true if block was splitted, false if block was never splitted.
     */
    SAPDB_Bool GetReleasableBlockInformation( void *      & blockAddress,
                                              SAPDB_ULong & blockCount )
    {
        if ( GetFirstSplinter() || (0 == m_FreeBlockCount) )
        {
            return false;
        }
        blockAddress = m_BlockAddress;
        blockCount   = m_FreeBlockCount;
        return true;
    }

    /*!
      @brief get dump block information

      @param blockAddress [out] the block address set
      @param blockCount   [out] the block size
      @param firstSplinter [out] the address of the first splinter
      @param nextSplinter  [out] the address of the next splinter
     */
    void GetDumpBlockInformation( void *                   & blockAddress,
                                  SAPDB_ULong              & blockCount,
                                  RTEMem_BlockDescriptor * & firstSplinter,
                                  RTEMem_BlockDescriptor * & nextSplinter )
    {
        blockAddress  = m_BlockAddress;
        blockCount    = m_FreeBlockCount;
        firstSplinter = m_FirstSplinter;
        nextSplinter  = m_NextSplinter;
    }

private:
    RTEMem_BlockDescriptor * m_NextDescriptor;       ///< address of next list element
    void                   * m_BlockAddress;         ///< address of block to manage
    RTEMem_BlockDescriptor * m_FirstSplinter;        ///< address of first splinter if splitted block
    RTEMem_BlockDescriptor * m_NextSplinter;         ///< address of next splinter if splitted block
    SAPDB_ULong              m_FreeBlockCount;       ///< set only if free block or dump block, otherwise 0
};

/*!
  @class    RTEMem_BlockChainHead
  @brief    Element of a single linked list containing RTEMem_BlockDescriptor lists as payload

  RTEMem_SystemPageCache internal block chain head pointing to single linked list of block descriptors
  for blocks of identical block size
 */
class RTEMem_BlockChainHead
{
private:
    RTEMem_BlockChainHead  * pNextChain;              ///< next bigger block size block chain
    RTEMem_BlockDescriptor * firstBlockDescriptor;   ///< first block descriptor in list
    SAPDB_ULong blockCount;                           ///< common block size of all blocks in this chain
friend class RTEMem_SystemPageCache;
};

/*!----------------------------------------------------------------------
  @class    RTEMem_SystemPageCache
  @brief    Singleton for System Page allocation/buffering

  This cache allows to reduce the number of system calls while allocating / deallocating system pages.
  It uses memory descriptors for each block of free pages. The memory descriptor blocks are separated.
  They have fixed size and are hold in a simple stack like freelist.
-----------------------------------------------------------------------*/
class RTEMem_SystemPageCache : public SAPDBMem_IBlockAllocator
{
public:
   /*!
    @brief Get the reference to single instance of RTEMem_SystemPageCache

    If necessary the constructor for the singleton is called

    @return Reference to RTEMem_SystemPageCache instance
   */
   static RTEMem_SystemPageCache & Instance();

   /*!----------------------------------------------------------------------
   @brief  Returns the number of blocks needed to allocate specifed number of bytes

   Since Allocate() needs the number of blocks instead of number 
   of bytes, this helper routine exists to calculate the right 
   argument. When used calling the Routine GetBlockSize() is not needed.

   @param NumberOfBytes [in] Number of bytes instead of number of blocks.

   @return [SAPDB_ULong] Number of blocks needed for a given number of bytes.
   -----------------------------------------------------------------------*/
   virtual SAPDB_ULong CalcBlockCount(SAPDB_ULong NumberOfBytes) const
      { return ( NumberOfBytes + m_pageSize - 1 ) / m_pageSize; }

   /*!----------------------------------------------------------------------
   @brief Returns the size in bytes of the blocks that can be allocated by this allocator.
   @return system page size in bytes
   -----------------------------------------------------------------------*/
   virtual SAPDB_ULong GetBlockSize() const { return m_pageSize; }

   /*!----------------------------------------------------------------------
   @brief Returns the alignment size (in bytes) of the blocks that can be allocated by this allocator.

   An implementation must ensure that the alignment size A and
   the block size B satisfy B = N * A, where N is a non-zero
   integer number.
   -----------------------------------------------------------------------*/
   virtual SAPDB_ULong GetAlignmentSize() const { return m_pageSize; }

   /*!----------------------------------------------------------------------
   @brief          Outputs statistics about the number of Allocate and Deallocate calls of
                      the allocator to the underlying base allocator.
   @param          CountAlloc [out] The number of calls of method Allocate of the base allocator.
   @param          CountDealloc [out] The number of calls of method Deallocate of the base.
   @return         none
   -----------------------------------------------------------------------*/
   virtual void GetBaseAllocatorCallStatistics( SAPDB_ULong &CountAlloc,
                                                SAPDB_ULong &CountDealloc) const;

   /*!----------------------------------------------------------------------
   @brief          Outputs statistics about the number of Allocate and Deallocate calls of
                      the allocator.
   @param          CountAlloc [out] The number of calls of method Allocate of allocator.
   @param          CountDealloc [out] The number of calls of method Deallocate of allocator
   @return         none
   -----------------------------------------------------------------------*/
   virtual void GetCallStatistics( SAPDB_ULong &CountAlloc,
                                   SAPDB_ULong &CountDealloc) const;

   /*!----------------------------------------------------------------------
   @brief Outputs statistics about the memory usage of the allocator.
   @param BytesUsed [out] The number of bytes used by this allocater.
   @param MaxBytesUsed [out] The maximum number of bytes used by this allocator so far
   @param BytesControlled [out] The number of bytes controlled by this allocator.
   -----------------------------------------------------------------------*/
   virtual void CalcStatistics( SAPDB_ULong &BytesUsed,
                                SAPDB_ULong &MaxBytesUsed,
                                SAPDB_ULong &BytesControlled);

   /*!----------------------------------------------------------------------
   @brief  Allocates contiguous memory for 'count' blocks of memory.

   The implementation grants that a pointer returned will be an
   integral multiple of the alignment size.

   The memory is not initialised.

   @param BlockCount [in] The number of blocks to be allocated. This argument must be greater zero.
   @return [void*] A pointer to the allocated raw memory block of given number of
                 blocks. If exception handling is disabled, a NULL pointer will
                 be returned in case of insufficient memory.
   -----------------------------------------------------------------------*/
   virtual void* Allocate(SAPDB_ULong BlockCount)
   {
       return Allocate((void *)0, BlockCount, true); // return commited memory at any address
   }

   /*!
      @brief          Allocates contiguous memory for 'count' blocks of
                      memory.
                      An implementation grants that a pointer returned will be an
                      integral multiple of the alignment size. 
                      The memory is not initialised.
      @param          BlockCount [in] The number of blocks to be allocated. This argument must be greater zero.
      @param          messageList [out] filled if memory not available   
      @return         void*
                      A pointer to the allocated raw memory block of &lt;BlockCount> 
                      blocks. If exception handling is disabled, a NULL pointer will
                      be returned in case of insufficient memory.
      @exception      From 'SAPDBMem_IBlockAllocator' derived classes may 
                      throw a 'SAPDBMem_BadAllocException' if the allocation fails.

    */

  virtual void * Allocate(SAPDB_ULong BlockCount, Msg_List &messageList)
  {
      return Allocate((void *)0, BlockCount, true, messageList); // return commited memory at any address
  }

   /*!----------------------------------------------------------------------
   @brief  Allocates contiguous memory for 'count' blocks of memory with commit specified

   The implementation grants that a pointer returned will be an
   integral multiple of the alignment size.

   The memory is not initialised and not commited, if DoCommit is false

   @param FixedAddress [in] If not 0, only return memory at specifed address
   @param BlockCount [in] The number of blocks to be allocated. This argument must be greater zero.
   @param DoCommit [in] Flag to indicate memory must be committed
   @return [void*] A pointer to the allocated raw memory block of given number of
                 blocks. If exception handling is disabled, a NULL pointer will
                 be returned in case of insufficient memory.
   -----------------------------------------------------------------------*/
   void* Allocate(void *FixedAddress, SAPDB_ULong BlockCount, SAPDB_Bool DoCommit);

   /*!----------------------------------------------------------------------
   @brief  Allocates contiguous memory for 'count' blocks of memory with commit specified

   The implementation grants that a pointer returned will be an
   integral multiple of the alignment size.

   The memory is not initialised and not commited, if DoCommit is false

   @param FixedAddress [in] If not 0, only return memory at specifed address
   @param BlockCount [in] The number of blocks to be allocated. This argument must be greater zero.
   @param DoCommit [in] Flag to indicate memory must be committed
   @param messageList [out] message if out of memory
   @return [void*] A pointer to the allocated raw memory block of given number of
                 blocks. If exception handling is disabled, a NULL pointer will
                 be returned in case of insufficient memory.
   -----------------------------------------------------------------------*/
   void* Allocate(void *FixedAddress, SAPDB_ULong BlockCount, SAPDB_Bool DoCommit, Msg_List &messageList);

   /*!----------------------------------------------------------------------
   @brief Frees the memory.

    None of the objects in the memory block will be deinitialised
    by this call.  The size argument must match the value passed
    to 'allocate' to obtain this memory. The pointer argument
    must not be null.
    @param p [in] previously allocated block
    @param BlockCount [in] size of block in number of blocks
   -----------------------------------------------------------------------*/
   virtual void Deallocate(void* p, SAPDB_ULong BlockCount);

   /*!
     @brief Return identifier
     @return const SAPDB_UTF8 * "RTEMem_SystemPageCache"
    */

   virtual const SAPDB_UTF8 * GetIdentifier() const 
          { return (const SAPDB_UTF8 *)"SystemPageCache"; }

   /*!
      @brief          Returns the number of failed allocation requests
      @return         number of failed allocation requests
    */
   virtual SAPDB_ULong GetAllocFailedCount() const
       { return m_ErrorCount.GetValue(); }

   /*!
     @brief Release all free unsplitted blocks
     @return number of released pages
    */
   SAPDB_ULong ReleaseFreeBlocks();

   /*!
     @brief force dump of block chains into knldiag
     @param toConsole [in] optional flag to dump to console (default off)
    */
   void Dump(SAPDB_Bool toConsole=false);

   /*!----------------------------------------------------------------------
    @brief disables the system page cache

    This call enforces an write through mode, which effectivly disables the caching.
    Only already splitted blocks are kept under cache control, sice they cannot be returned
    to the system without prior merging.
    */
    void Disable()
    {
        m_isDisabled = 1;
        ReleaseFreeBlocks();
    }

   /*!----------------------------------------------------------------------
    @brief reenables the system page cache

    This call stops the write through mode.
    */
    void Enable()
    {
        m_isDisabled = 0;
    }

private:
    /*!
      lock selector enumeration
     */
    typedef enum { NoLockFreeChainHead, LockFreeChainHead, LockUsedChainHead } LockSelection;
    /*!----------------------------------------------------------------------
    @brief contructor
    */
    RTEMem_SystemPageCache();

   /*!----------------------------------------------------------------------
    @brief return a descriptor to the descriptor pool
    Call uses m_descriptorPoolLock to lock pool
    @param blockDescriptor [in] block descriptor to return
    */
    void LockedReturnDescriptorToPool(RTEMem_BlockDescriptor *blockDescriptor);

   /*!----------------------------------------------------------------------
    @brief add a complete linked list of descriptors to the descriptor pool
    Call uses m_descriptorPoolLock to lock pool
    @param firstDescriptor [in] first descriptor to add
    @param lastDescriptor [in] last block descriptor to add
    */
    void LockedAddDescriptorChainToPool( RTEMem_BlockDescriptor *firstDescriptor,
                                         RTEMem_BlockDescriptor *lastDescriptor );

   /*!----------------------------------------------------------------------
    @brief get a descriptor from the descriptor pool
    Call uses m_descriptorPoolLock to lock pool
    @param blockDescriptor [out] block descriptor returned
    @return true if descriptor returned, false if no more descriptor in pool
    */
    SAPDB_Bool LockedGetDescriptorFromPool(RTEMem_BlockDescriptor * &blockDescriptor);

   /*!----------------------------------------------------------------------
    @brief get a descriptor from the descriptor pool
    If needed this call allocates memory for the pool
    @return block descriptor returned or 0 if out of memory
    */
    RTEMem_BlockDescriptor * GetDescriptorFromPool();

   /*!----------------------------------------------------------------------
    @brief get a descriptor from the descriptor pool
    If needed this call allocates memory for the pool
    @param messageList [out] filled if out of memory
    @return block descriptor returned or 0 if out of memory
    */
    RTEMem_BlockDescriptor * GetDescriptorFromPool(Msg_List &messageList);

   /*!----------------------------------------------------------------------
    @brief return a chain head to the chain head pool
    Call uses m_chainHeadPoolLock to lock pool
    @param chainHead [in] chain head to return
    */
    void LockedReturnChainHeadToPool(RTEMem_BlockChainHead *chainHead);

   /*!----------------------------------------------------------------------
    @brief add a complete linked list of chain heads to the chain head pool
    Call uses m_chainHeadPoolLock to lock pool
    @param firstChainHead [in] first chain head to add
    @param lastChainHead [in] last chain head to add
    */
    void LockedAddChainHeadChainToPool( RTEMem_BlockChainHead *firstChainHead,
                                        RTEMem_BlockChainHead *lastChainHead );

   /*!----------------------------------------------------------------------
    @brief get a chain head from the chain head pool
    Call uses m_chainHeadPoolLock to lock pool
    @param chainHead [out] chain head returned
    @return true if chain head returned, false if no more chain heads in pool
    */
    SAPDB_Bool LockedGetChainHeadFromPool(RTEMem_BlockChainHead * &chainHead);

   /*!----------------------------------------------------------------------
    @brief get a chain head from the chain head pool
    If needed this call allocates memory for the pool
    @return chain head returned or 0 if out of memory
    */
    RTEMem_BlockChainHead * GetChainHeadFromPool();

    /*!----------------------------------------------------------------------
    @brief get a free block descriptor for the given block size
    Call uses m_freeBlockChainLock to lock free block chain. The search starts with given block
    chain header. If necessary it continous search on bigger size chains and splits a block of 
    bigger size. The block descriptor returned is dequeued.
    @param blockCount [in] number of block requested
    @param blockChain [in] first block chain to start search.0
    @return block descriptor returned or 0 if no block in free list
    */
    RTEMem_BlockDescriptor * LockedDequeueFreeBlock( SAPDB_ULong       blockCount,
                                                     RTEMem_BlockChainHead  & blockChain );
    /*!----------------------------------------------------------------------
    @brief insert the remaining splinter and convert block into splinter
    @param freeChainHead [in] the free chain head were to enqueue the splitted block
    @param pBlockDescriptor [in] the block descriptor of the block splitted
    @param splittedBlockAddress [in] the start address of the splitted block
    @param splittedBlockCount [in] the count of the splitted block
    @param pSplittedBlockDescriptor [out] the descriptor of the remaining splinter
    */
    void ReinsertRemainingSplinter( RTEMem_BlockChainHead  & freeChainHead,
                                    RTEMem_BlockDescriptor * pBlockDescriptor,
                                    void                   * splittedBlockAddress,
                                    SAPDB_ULong              splittedBlockCount,
                                    RTEMem_BlockDescriptor * pSplittedBlockDescriptor);
    /*!----------------------------------------------------------------------
    @brief protected dequeue of a given block descriptor if found in given chain head
    Call uses m_freeBlockChainLock to lock free block chain. 
    @param blockChain [in] the block chain to search
    @param blockDescriptor [in] the block descriptor to dequeue
    @return true if found and dequeued, false else
    */
    SAPDB_Bool DequeueSpecifiedFreeBlockDescriptor( RTEMem_BlockChainHead  & blockChain,
                                                    RTEMem_BlockDescriptor * blockDescriptor );

    /*!----------------------------------------------------------------------
    @brief protected enqueue of a block descriptor in given chain head
    Call uses given lock to lock block chain. 
    @param firstDescriptor [in]  address of first descriptor in chain
    @param blockDescriptor [in]  block which will become next first block in chain
    */
    void EnqueueBlockDescriptor( RTEMem_BlockDescriptor * & firstDescriptor,
                                 RTEMem_BlockDescriptor *   blockDescriptor );

    /*!----------------------------------------------------------------------
    @brief protected enqueue of a block descriptor in given chain head
    Call uses given lock to lock block chain. 
    @param firstDescriptor [in]  address of first descriptor in chain
    @param blockDescriptor [in]  block which will become next first block in chain
    @param blockCounter    [out] block counter
    @param lock [in] lock used to protect chain
    */
    void LockedEnqueueBlockDescriptor( RTEMem_BlockDescriptor * & firstDescriptor,
                                       RTEMem_BlockDescriptor *   blockDescriptor,
                                       SAPDB_ULong              & blockCounter,
                                       RTESync_Spinlock  & lock );

    /*!----------------------------------------------------------------------
    @brief protected dequeue of a block address from used block chain head
    Call uses m_usedBlockChainLock to lock used block chain.
    @param firstDescriptor [in] address of first descriptor in chain
    @param blockAddress [in] block address to search
    @return block descriptor dequeued or 0 if no such block descriptor found
    */
    RTEMem_BlockDescriptor * LockedDequeueUsedBlockDescriptor( RTEMem_BlockDescriptor * & firstDescriptor,
                                                               void *                     blockAddress );

   /*!----------------------------------------------------------------------
    @brief lock protected search free block chain head
    If the corresponding block chain head is not found, the given pNewHead is added to the chain.
    
    @param blockCount [in] size of block requested in number of pages
    @param chainHead [in] free block or used block chain
    @param lock [in] the lock used to protect access to the chain
    @param pNewHead [in] already allocated space for new block chain head
    @param pResultHead [out] update the pointer to block descriptor to return
    @return true if block chain was added, false if already found (pNewHead can be released by caller)
    */
    SAPDB_Bool LockedFindBlockChainHead( SAPDB_ULong                blockCount,
                                         RTEMem_BlockChainHead *  & chainHead,
                                         RTESync_Spinlock         & lock,
                                         RTEMem_BlockChainHead *    pNewHead,
                                         RTEMem_BlockChainHead *  & pResultHead );

   /*!----------------------------------------------------------------------
    @brief lock protected search free block chain head
    If the corresponding block chain head is not found, false is returned instead of adding a new chain head
    
    @param blockCount [in] size of block requested in number of pages
    @param pChainHead [in] free block or used block chain
    @return true if block chain was found, false if not (no such head)
    */
    SAPDB_Bool SearchFreeBlockChainHead( SAPDB_ULong                blockCount,
                                         RTEMem_BlockChainHead *  & pChainHead );
   /*!----------------------------------------------------------------------
    @brief search free block chain head
    If no free block chain head is found, it is created. Always a reference to the right RTEMem_BlockChainHead
    is returned. This does not automatically return a free block...
    
    @param blockCount [in] size of block requested in number of pages
    @param foundHead [out] block chain for given block size found or created
    @param whichLock [in]  select which chain head list and if lock to use
    @return true if block chain head was found or created, false if out of memory
    */
    SAPDB_Bool FindBlockChainHead( SAPDB_ULong               blockCount,
                                   RTEMem_BlockChainHead * & foundHead,
                                   LockSelection             whichLock );

   /*!----------------------------------------------------------------------
    @brief search all free block chains for block that can be released
    
    This call searches for unsplitted blocks to return them to the system.
    The information how much splitted blocks are still hold in free list is returned too.

    @param releasedPages [out] number of released pages
    @param releasedBlocks [out] corresponding number of released blocks
    @param releasedPages [out] number of still holding  pages
    @param releasedBlocks [out] corresponding number of still holding blocks
    @return a list of the dequeued free blocks
    */
    RTEMem_BlockDescriptor * GetDequeuedFreeBlocks( SAPDB_ULong & releasedPages, 
                                                    SAPDB_ULong & releasedBlocks,
                                                    SAPDB_ULong & holdingPages,
                                                    SAPDB_ULong & holdingBlocks );

   /*!----------------------------------------------------------------------
    @brief fill dumpBlockChain with a copy of the current descriptors found in given chain
    
    This call parses all chain heads and all chains, but copies only used block descriptors
    as 'free block descriptors'.
    @param chainHead [in] the first chain head
    @param lock [in] the lock used to protect access to the chain
    @param dumpBlockChain [out] the output chain0
    @param chainHeads [out] the number of chain heads parsed
    @param blockDescriptors [out] the number of block descriptors parsed
    @return true if chain was retrieved, false if out of memory
    */
    SAPDB_Bool GetDumpBlockChain( RTEMem_BlockChainHead  * chainHead,
                                  RTESync_Spinlock         &lock,
                                  RTEMem_BlockDescriptor * &dumpBlockChain,
                                  SAPDB_ULong            & chainHeads,
                                  SAPDB_ULong            & blockDescriptors );

   /*!----------------------------------------------------------------------
    @brief merge given blockDesriptor with all found adjacent blocks in free block chains
    
    After this call all merged blocks are returned and the already dequeued blockDescriptor
    is containing the new block size. If free splinters in front of the block are found, the
    block start address is modified.

    @param pBlockDescriptor [inout] the already dequeued block descriptor that request the merge operation
    @param blockCount [inout] the block count. Will be updated on return if merge happened
    @param pDequeuedBlockDescriptorChain [out] head of chain of block descriptors to be returned to pool
    @return the merged block count
    */
    SAPDB_ULong MergeWithFreeBlocks( RTEMem_BlockDescriptor * &pBlockDescriptor,
                                     SAPDB_ULong               blockCount,
                                     RTEMem_BlockDescriptor * &pDequeuedBlockDescriptorChain);

   /*!----------------------------------------------------------------------
    @brief dump a snapshot of current statistic information
    @param toConsole [in] flag to redirect dump to console
    */   
    void DumpStatistic(SAPDB_Bool toConsole);

    SAPDB_ULong m_pageSize;             ///< system memory page size
    SAPDB_Singleton m_ThisIsASingleton; ///< prevents copy and assignment

    RTEMem_BlockDescriptor *m_descriptorPool;     ///< pool of free block descriptors
    RTESync_NamedSpinlock m_descriptorPoolLock;   ///< spinlock used to protect pool of free descriptors
    RTEMem_BlockChainHead *m_chainHeadPool;              ///< pool of free chain heads
    RTESync_NamedSpinlock m_chainHeadPoolLock;    ///< spinlock used to protect pool of free chain heads
    RTEMem_BlockChainHead *m_freeBlockChainHead;         ///< head of free block lists
    RTESync_NamedSpinlock m_freeBlockChainLock;   ///< spinlock used to protect free block chain management
    RTEMem_BlockChainHead *m_usedBlockChainHead;         ///< head of used block lists
    RTESync_NamedSpinlock m_usedBlockChainLock;   ///< spinlock used to protect used block chain management
    RTEMem_BlockChainHead *m_emergencyFreeBlockChainHead; ///< head of emergency free block list

    RTESync_InterlockedCounter<SAPDB_ULong> m_BytesUsed;       ///< number of bytes in used blocks
    RTESync_InterlockedCounter<SAPDB_ULong> m_MaxBytesUsed;    ///< maximum number of bytes ever in used blocks
    RTESync_InterlockedCounter<SAPDB_ULong> m_BytesControlled; ///< number of bytes controlled
    RTESync_InterlockedCounter<SAPDB_ULong> m_CountBaseAlloc;  ///< number of Allocate calls of system pages
    RTESync_InterlockedCounter<SAPDB_ULong> m_CountBaseDealloc;///< number of Deallocated calls of system pages
    RTESync_InterlockedCounter<SAPDB_ULong> m_CountAlloc;      ///< number of Allocate calls
    RTESync_InterlockedCounter<SAPDB_ULong> m_CountDealloc;    ///< number of Deallocated calls
    RTESync_InterlockedCounter<SAPDB_ULong> m_ErrorCount;      ///< number of failed allocation calls
    SAPDB_Int4                              m_isDisabled;      ///< flag to indicate cache is disabled
    SAPDB_ULong                             m_FreeBlocks;      ///< number of free blocks
    SAPDB_ULong                             m_SplittedBlocks;  ///< number of splitted blocks
    SAPDB_ULong                             m_SplinterBlocks;  ///< number of splinter blocks
    SAPDB_ULong                             m_FreeSplinterBlocks; ///< number of splinter blocks in free list
    SAPDB_ULong                             m_UsedBlocks;      ///< number of used blocks
    // Using a static reference would mean, that initialization function is called before main
    // This we have to prevent for the same reason we prevent static class instances
    // -> dynamic linked RunTime would not get initialized correctly on some OS...
    static RTEMem_SystemPageCache *m_Instance;
};

#endif  /* RTEMEM_SYSTEMPAGECACHE_HPP */
