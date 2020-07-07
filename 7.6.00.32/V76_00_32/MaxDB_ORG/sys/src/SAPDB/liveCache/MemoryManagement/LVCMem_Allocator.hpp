/*!
@ defgroup OMSMemory memory management of the OMS 
*/

/*!
  @file           LVCMem_Allocator.hpp
  @author         ThomasA
  @brief          Common Allocator for small objects in OMS context (replaces new/delete)
                  This allocator wrapps internally to several SAPDBMem_RawAllocator's.
                  It is also a singleton.

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



#ifndef LVCMEM_ALLOCATOR_HPP
#define LVCMEM_ALLOCATOR_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_SynchronizedRawAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_CallDirectory.hpp"
#include "Container/Container_Vector.hpp"
#include "heo670.h"

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
   @brief    Basic synchronised allocator replacing global 'malloc'/'free'

 */

class LVCMem_Allocator: public SAPDBMem_IRawAllocator
{
public:
  struct ContentInfo
  {
    SAPDB_UInt4 m_ChunkCount;
    SAPDB_UInt4 m_Size;
    ContentInfo()
      : m_ChunkCount(0)
      , m_Size(0)
    {
    }
  };

  struct SymbolInfo 
  {
      SymbolInfo() {}
      char* m_pSymb;
      int   m_offset;
      void* m_pcAddr;
  };

  typedef Container_Vector<ContentInfo> ContentInfoVector;

  typedef Container_Vector<SymbolInfo> SymbolVector;

  class ContentIterator
  {
    friend class LVCMem_Allocator;
  public :
    operator bool() const;
    void operator++();
    void Current(SAPDB_UInt4& ChunkCount, SAPDB_UInt4& Size, int& CallStackId, SymbolVector& Symbols) const;
    ~ContentIterator();
    struct AddressInfo 
    {
      eo670AddressInfo m_addressInfo;
      AddressInfo();
      ~AddressInfo();
    };
    struct Members
    {
      int                                m_refCount;
      void*                              m_callInfoContext;
      SAPDBMem_CallDirectory::Iterator   m_callStackDirIter;
      ContentInfoVector                  m_vector;
      Container_Vector<AddressInfo>      m_CallAddressInfo;

      Members(LVCMem_Allocator&);
    };
  private :
    LVCMem_Allocator& m_allocator;
    Members*          m_members;
    ContentIterator(LVCMem_Allocator& allocator);
  };

  /*!
     @brief  Return reference to single instance of LVCMem_Allocator
     @return Reference to LVCMem_Allocator instance
   */

  static SAPDBMem_IRawAllocator & Instance( );

  /*!
     @brief  Return reference to single instance of the oms allocator
     @return reference to the oms allocator instance
   */

  static SAPDBMem_IRawAllocator& LVCOmsAllocatorInstance();

  /*!
     @brief    Allocates contiguous memory for 'count' bytes.
               The memory is suitably aligned for all types.
               The memory is not initialised.  
               std::bad_alloc is thrown on failure.
     @param    count [in] Number of Bytes to allocate
     @return   Position where memory is allocated
  */
  
  virtual void* Allocate(SAPDB_ULong count);

  /*!
     @brief    Allocates contiguous memory for 'count' bytes,
     @param    count [in] Number of Bytes to allocate
     @param    hint [in] Hint where to allocate memory, has no meaning here
     @return   Position where memory is allocated
   */

  virtual void* Allocate(SAPDB_ULong count, const void * hint);

  /*!
     @brief    inline part of the implementaion of the Allocate method(s)
     @param    count [in] Number of Bytes to allocate
     @param    isUsrRequest [in] if true, method has been by operator new or
               omsMalloc, i.e. usr statistics will be updated.
               Otherwise the request is treaded as system request.
     @return   Position where memory is allocated
   */

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

  virtual void* AllocateThrowsNothing(SAPDB_ULong ByteCount);

  inline void* AllocateImplementation(SAPDB_ULong count, bool isUsrRequest, bool ThrowBadAlloc = true); //<! Allocate implementation

  ContentIterator Begin();

  /*!
     @brief   Frees the memory.
 
              The pointer must have been obtained by a previous call to
              'Allocate'.
     @param   p [in] Pointer to memory to deallocate
  */

   virtual void Deallocate(void * p);

  /*!
     @brief   returns the size of a chunk
     @param   the address of the chunk
     @return  the chunk size
  */

   virtual SAPDB_ULong GetChunkSize(const void* p);

  /*!
     @brief    Returns the name of this allocator
     @return value   "LVCMem_Allocator"
   */
   
   virtual const SAPDB_UTF8 * GetIdentifier() const 
      { return (const SAPDB_UTF8 *)"LVCMem_Allocator"; }

  /*!
     @brief    Outputs statistics about the memory usage of the allocator.
     @param    BytesUsed [out] The number of bytes used by this allocater.
     @param    BytesControlled [out] The number of bytes controlled by this allocator.
     @return   none
   */
 
    virtual void CalcStatistics( SAPDB_ULong &BytesUsed,
                                 SAPDB_ULong &MaxBytesUsed,
                                 SAPDB_ULong &BytesControlled);


  /*!
     @brief   Outputs statistics about the number of Allocate and Deallocate calls of
              the allocator to the underlying base allocator.
     @param   CountAlloc [out] The number of calls of method Allocate of the base allocator.
     @param   CountDealloc [out] The number of calls of method Deallocate of the base.
     @return  none
   */

   virtual void GetBaseAllocatorCallStatistics(SAPDB_ULong &CountAlloc,
                                  SAPDB_ULong &CountDealloc) const;

  /*!
    @brief returns the current call stack level (monitoring of chunk owners)
   */

  int GetCallStackLevel() const;

   /*!
      @brief   Outputs statistics about the number of Allocate and Deallocate calls.
      @param   CountAlloc [out] The number of calls of method Allocate.
      @param   CountDealloc [out] The number of calls of method Deallocate.
      @return  none
    */

    virtual void GetCallStatistics(SAPDB_ULong &CountAlloc,
                                  SAPDB_ULong &CountDealloc) const;

   /*!
      @brief  Returns the number of errors found and corrected by the allocator

      @return Number of errors
    */
   virtual SAPDB_ULong GetErrorCount() const;

   /*!
      @brief    Outputs statistics about the emergency allocator
      @param    BytesUsed [out] The number of bytes used by the emergency allocater.
      @param    BytesControlled [out] The number of bytes controlled by the emergency allocator.
      @param    MaxBytesUsed [out] max(BytesUsed)
      @return   none
    */

    void GetEmergencyAllocatorInfo(SAPDB_ULong &BytesUsed,
                                   SAPDB_ULong &BytesControlled,
                                   SAPDB_ULong &MaxBytesUsed) const; 

   /*!
      @brief     Creates the LVCMem_Allocator. Prior to this call the
                 allocator consists of a single SAPDBMem_RawAllocator.
      @param     subHeapCount [in] number of SAPDBMem_RawAllocator's controlled by this
                 allocator. Using several allocators reduces collisions on
                 multi cpu plattforms.
      @param     RawChunkSize [in] if one of the SAPDBMem_RawAllocator's needs memory
                 from the underlying block alloctor, at least RawChunkSize * 1024
                 bytes are required.
      @param     limitKB [in] maximal amount of memory (in KB) provided by the underlying block allocator
      @param     isLiveCache [in] true, if allocator is running in liveCache environment,
                 false otherwise (oltp).
      @return    none
    */

    void Create(int subHeapCount, SAPDB_ULong RawChunkSize, SAPDB_ULong limitKB, bool isLiveCache);

  /*!
     @brief      increments the counter for operation delete by one.
     @param      p [in] pointer to memory to be deallocated
     @return     none
   */

    inline void IncrementDelete(void* p);

  /*!
     @brief      increments the counter for operation new by one.
     @param      p [in] pointer to allocated memory
     @return     none
   */

    inline void IncrementNew(void* p);

    /*!
       @brief    Allocates contiguous memory for 'count' bytes from the oms allocator.
                 The memory is suitably aligned for all types.
                 The memory is not initialised.  
                 std::bad_alloc is thrown on failure.
       @param          count [in] Number of Bytes to allocate
       @return   Position where memory is allocated
     */

    inline void* OmsAllocate(SAPDB_ULong count);

    /*!
       @brief    Frees the memory from oms allocator.
                 The pointer must have been obtained by a previous call to
                 'Allocate'.
       @param    p [in] Pointer to memory to deallocate
     */
  
    inline void OmsDeallocate(void * p);

   /*!
      @brief      enables or disables allocator monitoring
      @param      enable [in] if true, monitoring is enables, otherwise monitoring id disabled.
      @return     none
    */

    inline void SetMonitoring(bool enable);

  /*!
     @brief       returns a reference to the LVCMem_Allocator instance
                  needed for implementation of operator new/delete
     @return      reference to LVCMem_Allocator instance
   */

    inline static LVCMem_Allocator& LVCAllocatorInstance();

    /*!
       @brief     inline implementation of LVCMem_Allocator.Deallocate
                  needed for implementation of operator new/delete
       @param     p [in] Pointer to memory to deallocate
       @param     isUsrRequest [in] if true, method has been called by operator delete or
                  omsFree, i.e. usr statistics will be updated.
                  Otherwise the request is treaded as system request.
       @return    none
     */

    inline void DeallocateImplementation(void* p, bool isUsrRequest);

    /*!
       @brief     forces the alloctor to ask no longer for the client of a memory request
       @return    none
     */

    void DisableClientInquiry();

   /*!
      @brief      enables or disables call stack monitoring. If call stack monitoring is
                  enabled, each memory chunk contains information about the call stack of
                  the memory request.
      @param      int [in] monitored call stack depth, 0 disables call stack monitoring
    */

   void SetCallStackMonitoring (int level);

   void SetLimit(SAPDB_ULong limit);

   /*!
      @brief      analyses the LVCMem_Allocator in case of a signal. If the allocator
                  is in an inconsistent state, errors are written into knldiag.
                  The method is called by the signal handle after having written
                  the stack back trace.
    */

   void SignalAnalysis();

   friend class LVCMem_Allocator::ContentIterator;
private:
  enum { MAX_SUB_HEAPS = 128 };
  // private allocator only used once during initialization
  LVCMem_Allocator();
 
  void* BadAllocHandling(
      int         allocatorIdx, 
      SAPDB_ULong count, 
      bool        badAllocRequired,
      bool        throwBadAlloc); //!< handles out of memory, tries to free memory 
  static void CreateInstance();                 //!< creates the singleton instance
  void DeallocateInEmergencyAllocator(void* p); //!< deallocates a chunk in emergency allocator
  int  FirstAllocatorIndex() const
  {
      return 0;
  }
  inline SAPDBMem_SynchronizedRawAllocator& GetSubAllocator(int idx) const
  {
      return *m_SubAllocators[idx];
  }
  
  /* data members */
  static LVCMem_Allocator*           m_Instance;            //!< pointer to the singleton instance
  static SAPDBMem_IRawAllocator*     m_OmsInstance;         //!< pointer to OMS allocator
  bool                               m_Monitoring;          //!< true, if monitoring is enabled
  bool                               m_askForClient;        //!< true, if acv can be used for subheap dispatching
  SAPDB_ULong                        m_CountNew;            //!< counts calls of operator new
  SAPDB_ULong                        m_CountDelete;         //!< counts calls of oerator delete
  int                                m_SubAllocatorCount;   //!< number of sub allocators
  SAPDBMem_SynchronizedRawAllocator  m_firstAllocator;      //!< at least one allocator is present always
  SAPDBMem_SynchronizedRawAllocator* m_SubAllocators[MAX_SUB_HEAPS]; //!< pointers to the subheaps

  // Using a static reference would mean, that initialization function is called before main
  // This we have to prevent for the same reason we prevent static class instances
  // -> dynamic linked RunTime would not get initialized correctly on some OS...
}; 


/*--------------------------------------------------------------------------------*/

#include "liveCache/MemoryManagement/LVCMem_Wrapper.hpp"

inline void* LVCMem_Allocator::AllocateImplementation(SAPDB_ULong count, bool isUsrRequest, bool ThrowBadAlloc)
{
#ifdef SAPDBMem_USE_SYSTEM_ALLOC
  void *p = malloc(count);
  if (!p) 
    p = this->BadAllocHandling(0, count, false, ThrowBadAlloc);
  return p;
#else
  const bool cBadAllocRequired = true;

  void* p;
  void* pContext  = (m_askForClient) ? LVCMem_Wrapper::GetContext() : NULL;
  int AllocatorId = LVCMem_Wrapper::GetUktId(pContext) % m_SubAllocatorCount;
  
#if defined (OMSTST)
  p = (void*) 0;
  bool badAllocRequired = LVCMem_Wrapper::BadAllocRequired(pContext);
  if (badAllocRequired)
  {
    this->BadAllocHandling(0, count, cBadAllocRequired, ThrowBadAlloc);
  }
#endif
  p = this->GetSubAllocator(AllocatorId).Allocate(count);
  if ((0 == p) && (1 < m_SubAllocatorCount))
  {
    int slot = AllocatorId;
    int cnt  = 1;
    do
    {
      ++cnt;
      ++slot;
      if (slot >= m_SubAllocatorCount)
      {
        slot = 0;
      }
      p = this->GetSubAllocator(slot).Allocate(count);
    }
    while ((0 == p) && (cnt != m_SubAllocatorCount));
  }
  if (0 == p)
  {
    p = this->BadAllocHandling(AllocatorId, count, !cBadAllocRequired, ThrowBadAlloc);
  }
  SAPDB_ULong chunkSize = this->GetChunkSize(p);
  LVCMem_Wrapper::MonitorAlloc (pContext, chunkSize, isUsrRequest);
  return p;
#endif
}

/*--------------------------------------------------------------------------------*/

inline void LVCMem_Allocator::DeallocateImplementation(void* p, bool isUsrRequest)
{
#ifdef SAPDBMem_USE_SYSTEM_ALLOC
    if (p) free(p);
#else
    if (0 == p)
    {
        return;
    }
    if (m_askForClient)
    {
        LVCMem_Wrapper::MonitorDealloc (this->GetChunkSize(p), isUsrRequest);
    }
    SAPDBMem_SynchronizedRawAllocator::GetAllocator(p)->Deallocate(p);
#endif
}

/*--------------------------------------------------------------------------------*/

inline void LVCMem_Allocator::IncrementDelete(void* p)
{
    ++m_CountDelete;
}

/*--------------------------------------------------------------------------------*/

inline void LVCMem_Allocator::IncrementNew(void* p)
{
    ++m_CountNew;
}

/*--------------------------------------------------------------------------------*/

inline LVCMem_Allocator& LVCMem_Allocator::LVCAllocatorInstance()
{
  if (0 == m_Instance)
  {
    LVCMem_Allocator::CreateInstance();
  }
  return *m_Instance;
}

/*--------------------------------------------------------------------------------*/

inline void* LVCMem_Allocator::OmsAllocate(SAPDB_ULong count)
{
   const bool cIsUserRequest = true;
   return AllocateImplementation (count, !cIsUserRequest);
}

/*--------------------------------------------------------------------------------*/

inline void LVCMem_Allocator::OmsDeallocate(void * p)
{
    const bool cIsUsrRequest = true;
    DeallocateImplementation (p, !cIsUsrRequest);
}

/*--------------------------------------------------------------------------------*/

inline void LVCMem_Allocator::SetMonitoring(bool enable)
{
	m_Monitoring = enable;
}

#define TheOmsRawAllocator LVCMem_Allocator::Instance()
#define TheLiveCacheAllocator LVCMem_Allocator::LVCAllocatorInstance() 
#define TheLiveCacheOmsAllocator LVCMem_Allocator::LVCOmsAllocatorInstance()

#endif  /* LVCMem_Allocator_HPP */
