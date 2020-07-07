/*!
  @file           SAPDBMem_RawAllocator.hpp
  @author         ThomasA
  @ingroup        Memory Mangement
  @brief          Implementation of a general purpose allocator.

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

#ifndef SAPDBMEM_RAWALLOCATOR_HPP
#define SAPDBMEM_RAWALLOCATOR_HPP

#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IBlockAllocator.hpp"
#include "RunTime/RTE_CompilerFeatures.h"
#include "RunTime/MemoryManagement/RTEMem_AllocatorRegister.hpp"
#include "ggg250.h"

class  RTESync_Spinlock;

#if defined (SAPDBMEM_RAWALLOCATOR_NAMESPACE)
namespace SAPDBMEM_RAWALLOCATOR_NAMESPACE
{
#endif

/*!
@brief defines the structur of a chunk provided from base allocator managed
       by SAPDBMem_Allocator
*/
struct SAPDBMem_RawChunkHeader
{
    /*!
    @brief contructor
    */
    SAPDBMem_RawChunkHeader(unsigned char* lwb, unsigned char* upb, SAPDB_UInt4 blockCount)
        : m_lwb(lwb)
        , m_upb(upb)
        , m_blockCount(blockCount)
    {
    }
    /*!
    @brief returns address of first chunk in base chunk
    */
    static void* FirstChunkInRawChunk(void* p); 
    
    unsigned char* m_lwb;        //!< start address of the chunk
    unsigned char* m_upb;        //!< end address of the chunk
    SAPDB_UInt4    m_blockCount; //!< size of the chunk in number of blocks
};


/*!
@brief raw chunks are managed by an AVL tree. This class defines the
       compare method used by the AVL tree.
*/
class SAPDBMem_RawChunkTreeComparator
{
public :
    /*!
    @brief compares the start addresses of two raw chunks
    @param t1 [in] reference to first chunk
    @param t2 [in] reference to second chunk
    @return -1 or 1
    */
    int Compare(const SAPDBMem_RawChunkHeader& t1, const SAPDBMem_RawChunkHeader& t2) 
    {
        if (t1.m_upb < t2.m_lwb)
        {
            return -1;
        }
        if (t2.m_upb < t1.m_lwb)
        {
            return 1;
        }
        return 0;
    }
};

/*!
@brief raw chunks are managed by an AVL tree. This class defines the
       allocator used by the AVL tree.
*/
class SAPDBMem_RawChunkTreeAllocator
{
public :
    typedef cgg250AvlNode<SAPDBMem_RawChunkHeader,SAPDBMem_RawChunkTreeComparator,SAPDBMem_RawChunkTreeAllocator> Node; //!< AVL node type
    void* allocate(size_t sz); //!< allocates sz bytes
    void  deallocate(void* p); //!< frees memory
    void  Set(void* p);        //!< stores a pointer in m_rawChunk, will be returned by next allocate call
private :
    void* m_rawChunk; //!< address of chunk next returned by allocate call
};

/*!
  @class          SAPDBMem_RawAllocator
  @brief          Implementation of the 'SAPDBMem_IRawAllocator'
                  interface.
                  This class provides an implementation of a heap based on the
                  ideas of the malloc/free/realloc written by Doug Lea.

                  According to Lea the algorithm is among the fastest while also being
                  among the most space-conserving, portable and tunable.
                  Consistent balance across these factors results in a good general-purpose 
                  allocator. For a high-level description, see
                  http://g.oswego.edu/dl/html/malloc.html
                  The following changes have been made:
                  - no requirements for addresses of chunks provided from raw allocator
                  - no special treatment of top chunk
                  - improved performance 
                  - 64 Bit Support
                  - use of a binary tree to manage free lists of big chunks. This reduces     insertion into free lists from O(n) to O(log2 n)
 */



#if defined(BIT64)
#define SAPDBMEM_ALLOCATOR_UNLIMITED 0xFFFFFFFFFFFFFFFFUL
#else
#define SAPDBMEM_ALLOCATOR_UNLIMITED 0xFFFFFFFF
#endif

#if ASSERT_STATE_OPTION == SAPDBERR_OFF
#undef DEBUG_BABDL
#else
#define DEBUG_BABDL
#endif

class  SAPDBMem_DelayedFreeHandler;
class  SAPDBMem_UsedChunkDirectory;
struct SAPDBMem_TreeNode;


class SAPDBMem_RawAllocator : public SAPDBMem_IRawAllocator
{
public:
  typedef SAPDB_ULong  SizeType;   //!< size type of the interface 
  typedef SAPDB_UInt4  ChunkSize;  //!< chunk size

  /*! defined wether memory is returned to base allocator */
  enum    FreeRawExtendsEnum { FREE_RAW_EXTENDS, NOT_FREE_RAW_EXTENDS, FREE_RAW_EXTENDS_EXCEPT_INITIAL };  

  /*! defines the bad alloc handler */
  enum    BadAllocHandlers   { NO_THROW_HANDLER, THROW_SAPDBMEM_EXECEPTION, THROW_STD_BAD_ALLOC };


  /*!
  @ brief defines an interface for tracing and aborting used by SAPDBMem_Allocator
  */
  class ITracerAndExceptionHandler 
  {
  public :
      /*!
      @brief dumps a memory chunk
      */
      virtual void Dump (void* p, int size) = 0;
      /*!
      @brief traces a zero terminated character string
      */
      virtual void Trace    (const char*) = 0;
       /*!
      @brief traces a zero terminated character string into the vtrace
      */
      virtual void VTrace    (const char*) = 0;
      /*!
      @brief exception handler
      */
      virtual void Exception(const char*) = 0;
  };

  /*!
  @brief structure keeping track of heap occupancy
  */
  struct THeapInfo {
    SizeType    used;          //!< number of bytes occupied by blocks currently in use
    SizeType    maxused;       //!< maximum number of bytes occupied by blocks currently in use
    SizeType    allocated;     //!< number of bytes allocated via GetMem. The heap uses GetMem to allocate memory from a raw allocator.
    SAPDB_UInt4 extends;       //!< number of extends of size m_supplement required from raw allocator 
    SAPDB_ULong cntAlloc;      //!< number of calls of Alloccate method
    SAPDB_ULong cntDealloc;    //!< number of calls of Deallocate method
    SAPDB_UInt4 cntRawAlloc;   //!< number of Allocate calls to raw allocator
    SAPDB_UInt4 cntRawDealloc; //!< number of Deallocate calls to raw allocator
    /*!
    @brief constructor, sets all members to given values
    */
    THeapInfo() 
      : used(0)
      , maxused(0)
      , allocated(0)
      , extends(0)
      , cntAlloc(0)
      , cntDealloc(0)
      , cntRawAlloc(0)
      , cntRawDealloc(0) {}
  };

/*!
  @brief          defines an heap iterator. The iterator allows to seek out all chunks (free and used) of the heap.
*/
  class HeapIterator {
#if defined (SAPDBMEM_RAWALLOCATOR_NAMESPACE)
      friend class SAPDBMEM_RAWALLOCATOR_NAMESPACE::SAPDBMem_RawAllocator;
#else
      friend class SAPDBMem_RawAllocator;
#endif
  public :
      HeapIterator(SAPDBMem_RawAllocator*);     //!< constructor for iterator on given SAPDBMem_RawAllocator 
      HeapIterator(const HeapIterator& source); //!< copies an iterator
      ~HeapIterator();                          //!< destructor               
      operator bool() const;                    //!< true, if iterator didn't reached the end yet
      void operator++();                        //!< steps to the next memory chunk
      const void* operator()() const;           //!< returns the current memory chunk
  private :
      SAPDBMem_RawAllocator* m_pHeap; //!< pointer to the SAPDBMem_RawAllocator instance
  };


   /*!
      @brief          Constructs an allocator from the provided RawAllocator or BlockAllocator.
      @param          Identifier [in] the name of the allocator
      @param          RawAllocator [in] The underlying raw allocator  
      @param          FirstBlockSize [in] The size of the memory block in bytes that the RawAllocator starts with.
      @param          SupplementBlockSize [in] The size of the memory blocks in bytes that are allocated, 
                      if the free memory is not sufficient to fulfill the next allocation.
      @param          FreeRawExtends [in] If FREE_RAW_EXTENDS, memory provided by RawAllocator is given back, whenever possible.
                      if FREE_RAW_EXTENDS_BUT_NOT_INTIAL_SIZE behaves like FREE_RAW_EXTENDS except that the first required
                      memory chunk is never given back.
      @param          MaxSize [in] The maximal size of the total memory in bytes controlled by the RawAllocator.
      @param          DoRegister[in] if true, the allocator is registered into the kernel allocator registry
    */


  SAPDBMem_RawAllocator(
    const SAPDB_UTF8       *   Identifier,
    SAPDBMem_IRawAllocator &   RawAllocator,
    SAPDB_ULong                FirstBlockSize,
    SAPDB_ULong                SupplementBlockSize,
    FreeRawExtendsEnum         FreeRawExtends,
    SAPDB_ULong                MaxSize = SAPDBMEM_ALLOCATOR_UNLIMITED,
    bool                       DoRegister = true
    ); 


   /*!
      @brief          Constructs an allocator from the provided RawAllocator or BlockAllocator.
      @param          Identifier [in] the name of the allocator
      @param          RawAllocator [in] The underlying raw allocator  
      @param          lockObject [in] synchronisation object used by allocator synchronisation
      @param          FirstBlockSize [in] The size of the memory block in bytes that the RawAllocator starts with.
      @param          SupplementBlockSize [in] The size of the memory blocks in bytes that are allocated, 
                      if the free memory is not sufficient to fulfill the next allocation.
      @param          FreeRawExtends [in] If FREE_RAW_EXTENDS, memory provided by RawAllocator is given back, whenever possible.
                      if FREE_RAW_EXTENDS_BUT_NOT_INTIAL_SIZE behaves like FREE_RAW_EXTENDS except that the first required
                      memory chunk is never given back.
      @param          MaxSize [in] The maximal size of the total memory in bytes controlled by the RawAllocator.
      @param          DoRegister[in] if true, the allocator is registered into the kernel allocator registry
    */


  SAPDBMem_RawAllocator(
    const SAPDB_UTF8       *   Identifier,
    SAPDBMem_IRawAllocator &   RawAllocator,
    RTESync_Spinlock*          lockObject,
    SAPDB_ULong                FirstBlockSize,
    SAPDB_ULong                SupplementBlockSize,
    FreeRawExtendsEnum         FreeRawExtends,
    SAPDB_ULong                MaxSize = SAPDBMEM_ALLOCATOR_UNLIMITED,
    bool                       DoRegister = true
    ); 


  /*!
      @brief          Constructs an allocator from the provided RawAllocator or BlockAllocator.
      @param          Identifier [in] the name of the allocator
      @param          BlockAllocator [in] The underlying block allocator
      @param          FirstBlockSize [in] The size of the memory block in bytes that the RawAllocator starts with.
      @param          SupplementBlockSize [in] The size of the memory blocks in bytes that are allocated, 
                      if the free memory is not sufficient to fulfill the next allocation.
      @param          FreeRawExtends [in] If FREE_RAW_EXTENDS, memory provided by RawAllocator is given back, whenever possible.
                      if FREE_RAW_EXTENDS_BUT_NOT_INTIAL_SIZE behaves like FREE_RAW_EXTENDS except that the first required
                      memory chunk is never given back.
      @param          MaxSize [in] The maximal size of the total memory in bytes controlled by the RawAllocator.
  */

  SAPDBMem_RawAllocator(
    const SAPDB_UTF8       *   Identifier,
    SAPDBMem_IBlockAllocator & BlockAllocator,
    SAPDB_ULong                FirstBlockSize,
    SAPDB_ULong                SupplementBlockSize,
    FreeRawExtendsEnum         FreeRawExtends,
    SAPDB_ULong                MaxSize = SAPDBMEM_ALLOCATOR_UNLIMITED
    ); 
  
  /*!
      @brief          Constructs an allocator from the provided RawAllocator or BlockAllocator.
      @param          Identifier [in] the name of the allocator
      @param          BlockAllocator [in] The underlying block allocator
      @param          lockObject [in] synchronisation object used by allocator synchronisation
      @param          FirstBlockSize [in] The size of the memory block in bytes that the RawAllocator starts with.
      @param          SupplementBlockSize [in] The size of the memory blocks in bytes that are allocated, 
                      if the free memory is not sufficient to fulfill the next allocation.
      @param          FreeRawExtends [in] If FREE_RAW_EXTENDS, memory provided by RawAllocator is given back, whenever possible.
                      if FREE_RAW_EXTENDS_BUT_NOT_INTIAL_SIZE behaves like FREE_RAW_EXTENDS except that the first required
                      memory chunk is never given back.
      @param          MaxSize [in] The maximal size of the total memory in bytes controlled by the RawAllocator.
  */

  SAPDBMem_RawAllocator(
    const SAPDB_UTF8       *   Identifier,
    SAPDBMem_IBlockAllocator & BlockAllocator,
    RTESync_Spinlock*          lockObject,
    SAPDB_ULong                FirstBlockSize,
    SAPDB_ULong                SupplementBlockSize,
    FreeRawExtendsEnum         FreeRawExtends,
    SAPDB_ULong                MaxSize = SAPDBMEM_ALLOCATOR_UNLIMITED
    ); 

   /*!
      @brief  destructor, returns all memory to base allocator
    */

   virtual ~SAPDBMem_RawAllocator();
  
  /*!
     @brief allocates ByteCount bytes from allocator
     @see   SAPDBMem_IRawAllocator::Allocate
   */

  virtual void* Allocate(SAPDB_ULong ByteCount);

  /*!
     @brief allocates ByteCount bytes from allocator
     @see   SAPDBMem_IRawAllocator::Allocate
   */

  virtual void* Allocate(SAPDB_ULong ByteCount, const void* Hint);

   /*!
     @brief allocates ByteCount bytes from allocator
     @see   SAPDBMem_IRawAllocator::AllocateThrowNothing
   */

  virtual void* AllocateThrowsNothing(SAPDB_ULong ByteCount);

  /*!
     @brief allocates ByteCount bytes from allocator without growing
   */

  void* AllocateWithoutGrowth(SAPDB_ULong ByteCount);

  /*!
     @brief        tells the allocator that it will be used by the given task exclusively. This is used
                   for checking unsynchronized allocators against concurrent usage. 
     @param        TaskId [in] the task identification of the task that is going to use the allocator
   */

  virtual void AssignToTask(SAPDB_UInt4 TaskId) { m_taskId = TaskId; }

  /*!
     @brief checks allocator integrity
   */

  bool Check();

   /*!
      @brief checks allocator integrity
      @see SAPDBMem_IRawAllocator::CheckConsistency
   */
  
   virtual bool CheckConsistency(); 

   /*!
      @brief          Checks that allows message output routines to ignore this allocator
      
      The message output routine needs memory. If a problem occurs inside a locked section of the
      allocator it would lead to deadlock if the message output would try to use it to show the
      message. Therefore the message output routine uses this function to decide to switch to its 
      own 'Emergency allocator'...

      @return         bool (if true only message output code will change its behaviour...)

      The default implementation does always indicates usabilaty for message output

    */
   virtual bool DoNotUseForMessageOutputAllocation() const { return m_BlockedForMessageOutput > 0; }

   /*!
      @brief if compiled multithreaded this returns the id of the thread that currently locks the internal structures 
      @return blocking thread id
    */
   SAPDB_UInt4 BlockingThreadId() const { return m_BlockingThreadId; }

   /*!
      @brief yieds allocator statistics
      @see SAPDBMem_IAllocatorInfo::CalcStatistics
    */
  
   virtual void CalcStatistics(SAPDB_ULong &BytesUsed,
                               SAPDB_ULong &MaxBytesUsed,
                               SAPDB_ULong &BytesControlled);

   /*!
      @brief yieds allocator statistics
    */
   void CalcStatisticsConst(SAPDB_ULong &BytesUsed,
                            SAPDB_ULong &MaxBytesUsed,
                            SAPDB_ULong &BytesControlled) const;
   /*!
      @brief computes the maximal allocatable size of a chunk under the assumption, that
             the base allocator may yield at most m_supplement bytes
    */
   SAPDB_ULong ComputeAndSetMaxAllocatableSize();
   /*!
      @brief defines a pattern that will be placed to the left of each allocated chunk
      @param Pattern [in] the pattern having the size of a pointer
    */
   void DefineLeftFencePattern (void* Pattern);
   /*!
      @brief normally in the kernel context information is written into the knldiag file, if the
             allocator runs out of memory. After a call of DisableOpMessages disables the behavior.  
   */

   void DisableOpMessages();

   /*!
      @brief yields statistics about calls to base allocator
    */

   virtual void GetBaseAllocatorCallStatistics(SAPDB_ULong &CountAlloc,
                                  SAPDB_ULong &CountDealloc) const;

    /*!
       @brief yields statistics about Allocate and Deallocate calls
    */


   virtual void GetCallStatistics(SAPDB_ULong &CountAlloc,
                                  SAPDB_ULong &CountDealloc) const;

   /*!
      @brief returns the size of a chunk
   */

   virtual SAPDB_ULong GetChunkSize(const void* p);

   /*!
      @brief returns the calculated chunk size for requested ByteCount bytes
   */

   virtual ChunkSize CalcChunkSize(SAPDB_ULong ByteCount);

   /*!
      @brief returns the calculated chunk header size for given HeapCheckLevel and CallStackLevel
   */

   static SAPDB_ULong CalcChunkHeaderSize(SAPDB_UInt4 checkFlags, bool monitorCallStackLevel);

   /*!
      @brief returns the calculated maximum RawChunk size for a given user chunk of size ByteCount bytes 
   */

   static SAPDB_ULong CalcMaxRawChunkSize(SAPDB_ULong ByteCount);

   /*!
   @brief returns the maximal size that may be requestet via the Allocate method
   */
   virtual SAPDB_ULong GetMaxAllocatableSize() const;
  /*!
     @brief deallocates (frees) a chunk previously returned by an Allocate call.
     @param p [in] the address of the chunk.
  */

  virtual void Deallocate(void* p);
  
  /*!
     @brief starts tracing
  */

  void EnableTrace();

  /*!
     @brief          See 'SAPDBMem_IRawAllocator'
  */

  virtual void FreeAll();

  /*!
     @brief          for a given memory chunk provided from a SAPDBMem_RawAllocator instance
                     a pointer to that instance is returned.
     @param          p [in] pointer to a chunk of memory provided by an Allocate call of a 
                     SAPDBMem_RawAllocator instance.
     @return         pointer to the SAPDBMem_RawAllocator instance which provided the memory
  */

  static SAPDBMem_RawAllocator* GetAllocator(const void* p);

  /*!
     @brief          yields information about a memory chunk
     @param          p [in] the address of the chunk. Must be the result of an Allocate call of the current instance.
     @param          inUse [out] true if the chunk is marked as used; otherwise false
     @param          ChunkSize [out] the size of the chunk in bytes
     @param          CallStackId [out] the identification of the back trace allocating the chunk
     @return         none
  */

  void GetChunkInfo(const void* p, bool& inUse, SAPDB_ULong& ChunkSize, SAPDB_ULong& CallStackId) const;

  /*!
    @brief returns the current call stack level (monitoring of chunk owners)
   */

  int GetCallStackLevel() const { return m_monitorCallStackLevel; }

  /*!
     @brief          Returns the number of errors found and corrected by the allocator
     @return         integer, number of error found
  */
  
  virtual SAPDB_ULong GetErrorCount() const;

   /*!
      @brief          Returns the number of failed allocation attemps
                      The derived class is responsible to implement this feature (but must not do so...)
      @return         integer
    */

  virtual SAPDB_ULong GetAllocFailedCount() const { return m_failedAllocCount; }

  /*!
     @brief          returns the minimal size of the first chunk allocated from the underlying allocator
     @return         size in Bytes
  */

  SAPDB_UInt4 GetFirstChunkParameter() const;

  /*!
     @brief          returns the allocator info of allocator (name, pointer to raw allocator interface,
                     name of underlying allocator.
     @return         reference to the allocator instance
   */

  RTEMem_AllocatorInfo& GetAllocatorInfo() 
  {
      return m_AllocatorInfo;
  }

  /*!
     @brief          returns the properties of a memory chunk. These properties have been
                     set by a previous call of SetChunkProperties.
     @param          p [in] pointer to a memory chunk returned by an SAPDBMem_RawAllocator::Allocate call
     @return values  the properties of the chunk (<= 7)
   */

  static int GetChunkProperties(const void* p);

  /*!
     @brief          returns the name of the allocator.
     @return         pointer to the allocator name
   */


  virtual const SAPDB_UTF8 * GetIdentifier() const;
  
 /*!
    @brief          yields information about heap occupancy
    @return         none
  */
  
  void GetInfo(THeapInfo& info) const;

  /*!
     @brief returns true if write protection is enables, false otherwise 
   */

  bool IsWriteProtected() const; 

  /*!
     @brief has same semantics as C runtime realloc 
   */

  void* Reallocate (SAPDB_ULong ByteCount, void* hint);

 /*!
    @brief          stores a pointer to an allocator into a memory chunk. The memory chunk
                    must have been provided by a call of SAPDBMem_RawAllocator::Allocate.
    @param          p [in] pointer to the memory chunk
    @param          pAllocator [in] pointer to an allocator
    @return         none
  */

  static void SetAllocator(void* p, SAPDBMem_IRawAllocator* pAllocator);

  /*!
     @brief enables the handler to be used when no more memory is available
     @param handler identifies the handler to be used 
           (NO_THROW_HANDLER,THROW_SAPDBMEM_EXECEPTION,THROW_STD_BAD_ALLOC)
  */

  void SetBadAllocHandler(BadAllocHandlers handler);

  /*!
     @brief set the name of the allocator
     @param Name the allocator name
  */

  void SetIdentifier(const SAPDB_UTF8 * Name);

  /*!
     @brief sets the maximal size of the total memory in bytes controlled by the RawAllocator.
            Has no meaning, if the current size is already greater than NewLimit.
     @param NewLimit the new size in bytes
  */

  void SetLimit(SAPDB_ULong NewLimit);

  /*!
     @brief sets the size of memory required from the base allocator
  */

  void SetSupplementChunkParameter(SAPDB_UInt4);

  /*!
     @brief sets the tracer to be used for tracing and exception handling
     @param pTracer [in] a pointer to an instance implementing the trace interface.
  */

  void SetTracer(ITracerAndExceptionHandler* pTracer) { m_tracer = pTracer; } 


  /*!
     @brief  enables or disables call stack monitoring. If call stack monitoring is
             enabled, each memory chunk contains information about the call stack of
             the memory request.
     @param level [in] call stack depth, 0 disables call stack monitoring
   */

  void SetCallStackMonitoring (int level);

  /*!
     @brief  each memory chunk managed by a SAPDBMem_RawAllocator instance may
             hold up to 3 bits of property information, that can be set by the
             holder of the chunk. The chunk properties have no meaning for
             the SAPDBMem_RawAllocator, but only for the chunk owner.
             The properties of a chunk may be requested via GetChunkProperties
     @param  p [in] pointer to a memory chunk returned by an SAPDBMem_RawAllocator::Allocate call
     @param  properties [in] integer with the chunk properties, must be &lt;= 7
   */

  static void SetChunkProperties(const void* p, int properties);

  /*!
     @brief enables all check levels of the allocator.
            caution : has no meaning, if the allocator holds any memory
  */

  void SetFullChecking ();

  /*!
     @brief enables all check levels of the allocator according to parameter HEAP_CHECK_LEVEL.
  */

  void SetCheckLevel();
  /*!
     @brief set the check level to the given value.
  */

  void SetCheckLevel(SAPDB_UInt4 CheckLevel);
  /*!
     @brief yields an iterator over all memory chunks of the allocator.
  */

  HeapIterator Begin();

  /*!
     @brief enables read and write access to data managed by the allocator 
  */

  bool Unprotect();

  /*!
     @brief disables any changes in memory managed by the allocator. This is possible only
            if the allocator is based on a block alloctor. 
     @return true if successful, otherwise false
  */
  bool WriteProtect();

  /* forward declarations of following classes must be public due to */
  /* aix compiler                                                    */
  class   CChunk;
  typedef CChunk* CChunkPtr; //!< a pointer to a chunk

protected:

/*! define inline */

#define INLINE inline 
  
   // PTS 1129397 
   /*! protected methods to set the heapInfo statistics */

   INLINE void IncrementUsed(ChunkSize increment)         //!< increments the used memory counter
   {
     m_heapInfo.used += increment;
     if (m_heapInfo.used > m_heapInfo.maxused)
       m_heapInfo.maxused = m_heapInfo.used;
   }
   INLINE void DecrementUsed(ChunkSize decrement)         //!< decrements the used memory counter
   {
     m_heapInfo.used -= decrement;
   }
   INLINE void IncrementAllocated(ChunkSize increment)    //!< increments the allocated memory counter
   {
      m_heapInfo.allocated += increment;
   }   
   INLINE void DecrementAllocated(ChunkSize decrement)    //!< decrements the allocated memory counter
   {
     m_heapInfo.allocated -= decrement;
   }
   INLINE void IncrementAllocCnt()    //!< increments the allocation counter
   {
     ++m_heapInfo.cntAlloc;
   }
   INLINE void IncrementDeallocCnt()  //!< increments the deallocation counter
   {
     ++m_heapInfo.cntDealloc;
   }

private:
    /*! defines allocator check levels */ 
    enum checkFlags { 
        FL_NO_MANS_LAND       = 1,
        FL_FILL_FREE_BLOCKS   = 2,
        FL_DOUBLE_BOOKKEEPING = 4,
        FL_CHECK_TREE         = 8,
        FL_USAGE              = 16,
        FL_DELAYED_FREE       = 32,
        FL_ALL                = FL_NO_MANS_LAND|FL_FILL_FREE_BLOCKS|FL_DOUBLE_BOOKKEEPING|FL_CHECK_TREE|FL_USAGE|FL_DELAYED_FREE
    };

  /*!
     @brief Private copy constructor avoids copying.
  */
  SAPDBMem_RawAllocator(const SAPDBMem_RawAllocator &InitAlloc);
   
  /*!
     @brief Private assignment operator avoids copying.
  */
  SAPDBMem_RawAllocator operator=(SAPDBMem_RawAllocator &);

  struct  TNodeBuffer;

  /*!
  @brief manages free chunks
  */
  class CFreeChunkLists 
  {
    enum { NO_OF_FREELISTS = 128 };
    
    /*!
    @brief structure of a free list header. This is the header of a double chained list
    */
    struct FreeListHeader {
      CChunkPtr m_next; //!< next free chunk
      CChunkPtr m_prev; //!< previous free chunk
    };
    
    FreeListHeader m_freeLists[NO_OF_FREELISTS + 2]; //!< spave for 128 free lists
    public :
      CFreeChunkLists(); //!< constructor
      INLINE CChunkPtr operator[] (int ix); //!< yields ix-th free kist header
      INLINE static int GetSmallFreeListIndex(ChunkSize size); //!< returns the index of the free list of chunks of size 'size' 
  };

  typedef SAPDBMem_TreeNode* NodePtr; //!< Node of the tree managing big free chunks

  typedef void* (BadAllocFunc)(SAPDB_ULong); //!< signature of a bad alloc handler

  /*!
     @brief contains the current state of an iterator
  */

  struct IteratorState
  {
      SAPDBMem_RawAllocator*                   m_pHeap;     //!< pointer the the allocator to be iterated
      void*                                    m_currBlock; //!< current raw block
      void*                                    m_currChunk; //!< current chunk
      int                                      m_refCnt;    //!< number of references to this iterator state
      cgg250AvlTree<SAPDBMem_RawChunkHeader,SAPDBMem_RawChunkTreeComparator,SAPDBMem_RawChunkTreeAllocator>::Iterator m_rawIter; //!< iterator over raw chunks
  };

  INLINE static ChunkSize AlignedSize (ChunkSize req);                               //!< returns the aligned size of a given size
  INLINE void*            AllocateImplementation(SAPDB_ULong ByteCount, bool CallBadAllocHandler, bool DoGrow); //!< implementation of a memory allocation
  INLINE void*            AllocateResult(CChunk* pChunk, int monitorCallStackLevel); //!< returns the result of an Allocate call
         bool             AnalyzeAllocator(void* p);                                 //!< checks the consistence of the allocator
  INLINE void             Assert(bool condtion);                                     //!< asserts a condition
  static void*            BadAllocNoThrow(SAPDB_ULong);                              //!< handler for bad alloc without throw (returns null)
  static void*            BadAllocThrowSAPDBMem_BadAlloc(SAPDB_ULong);               //!< handler for bad alloc throwing SAPDBMem_Exception
  static void*            BadAllocThrowStdBadAlloc(SAPDB_ULong);                     //!< handler for bad alloc throwing std::bad_alloc
         void             CheckConstraints();                                        //!< checks the allocator constraints, check in the course of construction 
  INLINE bool             CheckFreeChunk (CChunkPtr pChunk);                         //!< checks state of a free chunk
  INLINE void             CheckNoMansLand(CChunkPtr pChunk);                         //!< checks the no mans land pattern  
         bool             CheckPointer(void* p, bool abort = true);                  //!< verifies a pointer, must lie in any raw chunk 
  INLINE void             CheckUsage();                                              //!< checks if the caller if the allocators owner
         void             Crash(const char*);                                                   //!< crashed the kernel
         void             DeallocateRaw(void* p, SAPDB_ULong BlockCount);            //!< returns memory to the base allocator
         bool             DelayedFreeSucceeded(CChunkPtr p);                         //!< inserts a pointer into the list of delayed freed chunks
         void             Destructor();                                              //!< destructs the allocator
         void             Dump();                                                    //!< dumps the allocator using m_tracer
         void             DumpRawChunk(void* p);                                     //!< dumps a specific raw chunk using m_tracer
         void             DumpChunk(void* p, int length);                            //!< dumps a chunk using m_tracer
  INLINE static RTE_TaskId EvalCurrentTaskId();                                      //!< determines the current task id
#if defined(WIN32) || defined(OSF1)
         void             ExceptionDeallocate(void* p);                              //!< exception handler
         void             ExceptionAllocate();                                       //!< exception handler
#endif
  INLINE void             FillFreeChunk (void* p, int offset, int size, SAPDB_UInt4 pattern); //!< fille a chunk with given pattern
         void             FreeRawChunk(SAPDBMem_RawAllocator::CChunkPtr endChunk, SAPDBMem_RawAllocator::CChunkPtr p); //!< free a raw chunk
  INLINE void             FrontLink(CChunkPtr pChunk, ChunkSize sz); //!< inserts a chunk into free list
  static SAPDB_UInt4      GetCheckLevel();                           //!< return the current check level
  INLINE IteratorState&   GetIteratorState();                        //!< returns the current iterator state
  INLINE SAPDB_UInt4      Idx2BinBlock(int);                         //!< index
  static BadAllocFunc*    InitBadAllocHandler();                     //!< returns the initial bad alloc handler
         bool             MallocExtend(ChunkSize required);          //!< required memory from the base allocator
         void             NewNodeBuffer();                           //!< requests a new buffer for nodes of tree managing big chunks
  INLINE void             MarkBinBlock(int);                         //!< marks a bin block as containing free blocks
  static bool             IsSmallRequest (ChunkSize req);            //!< returns true, if req identifies a small request
  INLINE void             InsertFreeBigChunk(CChunkPtr p);           //!< inserts a big free cunks into the tree managing big chunks
         void             InsertChunkIntoSortedList(CChunkPtr pChunk); //!< inserts a chunk into a sorted list ordered by chunk size
         bool             Protect(SAPDB_Int mode);                     //!< support for memory write protection
  static int              RawChunkHeaderSize()                         
         {
             return ((sizeof(SAPDBMem_RawChunkTreeAllocator::Node) + sizeof(void*) -1) & ~(sizeof(void*)-1)); 
  }                                                                   //!< returns the size of a base chunk 

  INLINE void             RemoveFreeBigChunk(CChunkPtr p);            //!< removes a chunk from the tree managing big chunks
         void             SubTreeCheck(NodePtr p, NodePtr* followed); //!< checks a subtree of the tree managing big chunks for consistency
         void             ThrowException();                           //!< throws an exception
         void             TreeCheck();                                //!< checks a tree of the tree managing big chunks for consistency
         void             Trace(const char* msg, ...);                //!< writes a message into trace
         void             TraceRawChunks();                           //!< writes information of all base chunks into trace
         void             TraceSubTree(NodePtr p);                    //!< writes information of big free chunks into knldiag
         void             TraceTree();                                //!< writes information of big free chunks into knldiag
  INLINE bool             TreeFindFreeChunk(CChunkPtr p);             //!< searches a chunk in the tree managing big chunks
  INLINE void             TreeRemoveNode(NodePtr pNode);              //!< removes a node from the tree managing big chunks 
#ifdef DEBUG_BABDL
  void CheckInUseChunk(CChunkPtr);
  void CheckMallocedChunk(CChunkPtr p, ChunkSize s);
  void CheckChunk(CChunkPtr);
  void CheckFreeChunkDbg(CChunkPtr);
#endif

  SAPDBMem_RawAllocator*       m_this;                    //!< this
  SAPDBMem_IBlockAllocator*    m_BlockAllocator;          //!< underlying block allocator, may be NULL
  SAPDBMem_IRawAllocator*      m_RawAllocator;            //!< underlying raw allocator, may be NULL
  ITracerAndExceptionHandler*  m_tracer;                  //!< pointer to an instance providing tracer inderface
  SAPDBMem_DelayedFreeHandler* m_delayedFreeHandler;      //!< handler for delayed free chunks
  void*                        m_leftFencePattern;        //!< the pattern placed in front of each used chunk
  SizeType                     m_firstSize;               //!< size of the first base chunk 
  SizeType                     m_supplement;              //!< size of base chunk required from base allocator
  SizeType                     m_maxHeap;                 //!< maximal memory (in bytes) required from base allocator
  SAPDB_UInt4                  m_binBlocks;               //!< index of free lists containing free chunks
  SAPDB_UInt4                  m_checkFlags;              //!< current allocator check levels
  SAPDB_UInt4                  m_maxChunkSize;            //!< maximal chunk size
  THeapInfo                    m_heapInfo;                //!< allocator statistics
  bool                         m_checkHeap;               //!< check heap if true 
  FreeRawExtendsEnum           m_freeRawExtends;          //!< return free base chunks to base allocator ?
  bool                         m_opMessages;              //!< if true, op messages are written to the knldiag in error conditions
  bool                         m_writeAllocateFailedOpMsg;//!< if true, the allocate failed op message is written to the knldiag
  bool                         m_writeProtected;          //!< true if allocator is write protected
  bool                         m_doDeregister;            //!< true, if allocator has to be deregistered at destruction
  bool                         m_traceEnabled;            //!< true, if tracing is active 
  int                          m_BlockedForMessageOutput; //!< true, if allocator is just doing message output while in locked scope to show emergency condition 
  SAPDB_UInt4                  m_BlockingThreadId;        //!< the blocking thread id
  int                          m_monitorCallStackLevel;   //!< depth of back trace
  SAPDB_ULong                  m_failedAllocCount;        //!< numbers of error detected
  int                          m_errorCount;              //!< numbers of error detected
  RTE_TaskId                   m_taskId;                  //!< task identification of the task using the allocator
  CFreeChunkLists              m_freeLists;               //!< free lists of small chunks
  BadAllocFunc*                m_badAllocFunc;            //!< current bad alloc handler
  RTESync_Spinlock*            m_lock;                    //!< synchronisation object, may be NULL
  RTEMem_AllocatorInfo         m_AllocatorInfo;           //!< allocator statistics
  SAPDB_UTF8                   m_Name[40+1];              //!< name of the allocator
  IteratorState                m_iterState;               //!< current iterator state
#if defined (SAPDBMEM_RAWALLOCATOR_NAMESPACE)
  SAPDBMEM_RAWALLOCATOR_NAMESPACE::SAPDBMem_UsedChunkDirectory* m_usedChunks;             //!< used chunk directory, only if double bookkeeping is enabled
#else
  SAPDBMem_UsedChunkDirectory* m_usedChunks;             //!< used chunk directory, only if double bookkeeping is enabled
#endif
  // following members define the root of a binary tree holding big free chunks
  // caution : it must be possible to interpret the tree instance as a chunk !
  NodePtr                    m_root;                     //!< root to the tree managing big chunks
#if !defined(BIT64)
  void*                      m_filler;                   //!< filler to ensure chunk layout
#endif
  CChunkPtr                  m_next;                     //!< next to ensure chunk layout
  CChunkPtr                  m_prev;                     //!< prev to ensure chunk layout
  NodePtr                    m_firstFreeNode;            //!< pointer to first free node of tree managing big chunks
  // end caution
  TNodeBuffer*                    m_freeNodeBuffer;      //!< pointer to buffer providing nodes of tree managing big chunks
  SAPDBMem_RawChunkTreeAllocator  m_rawTreeAllocator;    //!< allocator for tree managing big chunks
  SAPDBMem_RawChunkTreeComparator m_rawChunkTreeComparator;  //!< comparator for tree managing big chunks
  cgg250AvlTree<SAPDBMem_RawChunkHeader,SAPDBMem_RawChunkTreeComparator,SAPDBMem_RawChunkTreeAllocator> m_rawChunkTree; //!< tree managing big free chunks
#if defined (SAPDBMEM_RAWALLOCATOR_NAMESPACE)
  friend class SAPDBMEM_RAWALLOCATOR_NAMESPACE::SAPDBMem_RawAllocator::HeapIterator;
  friend class SAPDBMEM_RAWALLOCATOR_NAMESPACE::SAPDBMem_RawAllocator::CFreeChunkLists;
  friend class SAPDBMEM_RAWALLOCATOR_NAMESPACE::SAPDBMem_RawAllocator::CChunk; //!< forward declaration of CChunk, defined in implementation
  friend struct SAPDBMEM_RAWALLOCATOR_NAMESPACE::SAPDBMem_RawChunkHeader;
  friend class  SAPDBMEM_RAWALLOCATOR_NAMESPACE::SAPDBMem_DelayedFreeHandler;
#else
  friend class  SAPDBMem_RawAllocator::HeapIterator;
  friend class  SAPDBMem_RawAllocator::CFreeChunkLists;
  friend class  SAPDBMem_RawAllocator::CChunk; //!< forward declaration of CChunk, defined in implementation
  friend struct SAPDBMem_RawChunkHeader;
  friend class  SAPDBMem_DelayedFreeHandler;

#endif
};

#if defined (SAPDBMEM_RAWALLOCATOR_NAMESPACE)
}; // namespace
#endif

#endif
