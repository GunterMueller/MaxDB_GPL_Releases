
/*!**************************************************************************

  module       : KernelMem_DataCacheAllocator.hpp

  -------------------------------------------------------------------------

  responsible : ThomasA

  special area: MemoryManagement
  description : Defines an allocator for objects of a given type, which requires memory from the data cache
 
  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG




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


    
*****************************************************************************/


#ifndef KERNELMEM_DATACACHEALLOCATOR_HPP
#define KERNELMEM_DATACACHEALLOCATOR_HPP

#include "RunTime/RTE_CompilerFeatures.h"
#include "Messages/Msg_List.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "RunTime/MemoryManagement/RTEMem_AllocatorRegister.hpp"
#include "FrameControl/FrameCtrl_IBlockAllocator.hpp"
#include "RunTime/Synchronisation/RTESync_RWRegion.hpp"
#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include "SAPDBCommon/Messages/SDBMsg_KernelCommon.h"


// TODO remove
#define VERIFY 1

#if defined VERIFY
#include "KernelCommon/Kernel_VTrace.hpp"
#include "Container/Container_AVLTree.hpp"
#include "KernelCommon/Kernel_OpMsg.hpp"

externC void vabort(bool);

#define KERNELMEM_CHECKPAGE this->Check();
#define KERNELMEM_CHECK_USED_ENTRY(P,PAGE) PAGE->CheckUsedChunk(P);
#define KERNELMEM_CHECK_FREE_ENTRY(P,PAGE) PAGE->CheckFreeChunk(P);
#define KERNELMEM_INSERT_CHUNK(P) { Container_AVLTreeError rc; m_allChunks.Insert(P,rc); }
#define KERNELMEM_INSERT_PAGE(P) { Container_AVLTreeError rc; m_tree.Insert(P,rc); m_allTree.Insert(P,rc); }
#define KERNELMEM_DELETE_PAGE(P) { Allocator.DeletePage(P); }
#define KERNELMEM_CHECK_CONTROLLED_PAGE(P,PCHUNK) CheckControlledPage(P,PCHUNK);
#else
#define KERNELMEM_CHECKPAGE
#define KERNELMEM_CHECK_FREE_ENTRY(P,PAGE)
#define KERNELMEM_CHECK_USED_ENTRY(P,PAGE)
#define KERNELMEM_INSERT_CHUNK(P)
#define KERNELMEM_INSERT_PAGE(P) 
#define KERNELMEM_DELETE_PAGE(P)
#define KERNELMEM_CHECK_CONTROLLED_PAGE(P)
#endif

template <class T>
class KernelMem_DataCacheAllocator;

/*!
   @brief defines how a page lying in the data cache is partitioned into slots of type T 
 */
template <class T>
class KernelMem_Page
{
    struct Entry
    {
        Entry(SAPDB_Int2 Offset)
            : m_offset(Offset)
            , m_refCount(0)
            , m_free(true)
            , m_entry()
        {
        }
        void IncRefCount()
        {
            ++m_refCount;
        }
        int DecRefCount()
        {
            --m_refCount;
            return m_refCount;
        }
        bool IsFree() const
        {
            return m_free;
        }
        void SetFree()
        {
            m_free = true;
        }
        void SetUsed()
        {
            m_free = false;
        }
        SAPDB_Int2   m_offset;
        SAPDB_Byte   m_refCount;
        bool         m_free;
        T            m_entry;
    };
public :
    friend class KernelMem_DataCacheAllocator<T>;
private :
    /*!
       @brief requires a page from the data cache and initializes with empty empty
     */
    static KernelMem_Page* Create(int SizeOfT);
    /*!
       @brief returns the page to the data cache
     */
    void Delete();
    /*!
       @brief returns the size of an entry of typ T in a page  
     */
    static int EntrySize(int SizeOfT);
    /*!
       @brief returns the page containing the entry identified by a given entry address
     */
    static KernelMem_Page* GetContainingPage(T* p);
    /*!
       @brief returns the first free entry on the page.
     */
    T* GetFreeEntry();
    /*!
       @brief releases a given entry
     */
    void FreeEntry(T* pFreeEntry, KernelMem_DataCacheAllocator<T>& Allocator);
    /*!
       @brief returns the entry at the given offset
     */
    Entry* EntryAtOffset(int Offset)
    {
        return reinterpret_cast<Entry*>(reinterpret_cast<SAPDB_Byte*>(this) + Offset);
    }
    /*!
       @brief returns the page from the page list
     */
    void Unlink()
    {
        m_prev->m_next = m_next;
        m_next->m_prev = m_prev;
    }
#if defined(VERIFY)
    inline void CheckFreeChunk(Entry* pEntry);
    inline void CheckUsedChunk(Entry* pEntry);
    static bool IsFreeChunk (T* p)
    {
         Entry* pEntry = reinterpret_cast<Entry*>(reinterpret_cast<SAPDB_Byte*>(p) - RTE_OFFSET_OF(Entry, m_entry));
         return pEntry->IsFree();
    }
    void Check();
#endif
    /*!
       @brief constructor
     */
    KernelMem_Page(int SizeOfT, int PageSize);
    KernelMem_Page* m_prev;
    KernelMem_Page* m_next;
    SAPDB_UInt2     m_inUseCount;
    SAPDB_UInt2     m_pageSize;
    SAPDB_UInt2     m_firstFreeOnPage;
    SAPDB_UInt2     m_entrySize;
    Entry           m_entries; // actually this is an array
};

//-----------------------------------------------------------------------------

/*!
   @brief implements an alloctor for objects of type T. The memory is allocated from the data cache.
          Sizeof(T) must fit into a page, i.e. must be smaller than about 8000 bytes.
          The objects managed by this allocator may have a different size than sizeof(T). This is
          useful, if only the prefix of T is known at compile time, but the rest becomes
          computed at execute time. However, the alignment is used from type T.
 */
template <class T>
class KernelMem_DataCacheAllocator : public SAPDBMem_IAllocatorInfo 
{
public :
    /*!
       @brief constructor. 
       @param SizeOfT [in] the wanted size for type T. Normally this is sizeof(T)
       @param Identifier [in] the name of the allocator. This is the name of the allocator in ALLOCATORSTATISTIC
       @param Ok [out] true, if the allocator could be constructed successfully; false otherwise
       @param MsgList [out], a description of the error, if Ok is false.
     */
    KernelMem_DataCacheAllocator(
        int                 SizeOfT,
        const SAPDB_UTF8*   Identifier,
        bool&               Ok, 
        Msg_List&           MsgList);
    /*!
       @brief destructor
     */
    ~KernelMem_DataCacheAllocator();
    /*!
       @brief provides memory for an instance of Type T
       @param TaskId [in] current task identification
       @return 0, if no more memory is available; !> 0 otherwise
     */
    T* Allocate(RTE_TaskId TaskId);
    /*!
       @brief returns memory
       @param TaskId [in] current task identification
       @p [in] the address to be released. Must be the result of an Allocate call.
     */
    void Deallocate(RTE_TaskId TaskId, T* p);
    // SAPDBMem_IAllocatorInfo interface
    virtual void CalcStatistics(SAPDB_ULong &BytesUsed,
                                SAPDB_ULong &MaxBytesUsed,
                                SAPDB_ULong &BytesControlled);
 
   virtual void GetBaseAllocatorCallStatistics(SAPDB_ULong &CountAlloc,
                                  SAPDB_ULong &CountDealloc) const;

   virtual void GetCallStatistics(SAPDB_ULong &CountAlloc,
                                  SAPDB_ULong &CountDealloc) const;

private :
    friend class KernelMem_Page<T>;
    typedef KernelMem_Page<T>* PagePtr;
    void Link(PagePtr pPage);
    void SetAllocatorName(const SAPDB_UTF8 * Name);
    RTESync_IRWRegion*       m_pLock;                  //!< synchronisation by RWLock
    RTESync_SpinlockPool     m_spinlockPool;           //!< spinlock pool of size 1 for RWLock
    int                      m_sizeOfT;                //!< sizeof an entry of type T      
    SAPDB_UInt8              m_allocCnt;               //!< number of allocates
    SAPDB_UInt8              m_deallocCnt;             //!< number of deallocates
    SAPDB_UInt8              m_baseAllocCnt;           //!< number of page allocates
    SAPDB_UInt8              m_baseDeallocCnt;         //!< number of page deallocates
    SAPDB_UInt8              m_usedEntries;            //!< number of entries in use
    SAPDB_UInt8              m_maxUsedEntries;         //!< maximal number of entries in use
    SAPDB_UInt4              m_pageCnt;                //!< number of pages controlled by the allocator 
    RTEMem_AllocatorInfo     m_allocatorInfo;           //!< allocator statistics
    SAPDB_UTF8               m_name[40+1];              //!< name of the allocator
    PagePtr                  m_prev;                    //!< previous page having free entries
    PagePtr                  m_next;                    //!< next page having free entries
#if defined VERIFY
public :
    inline int Compare(const PagePtr& NodeKey, const PagePtr& SearchKey)
    {
        if (NodeKey < SearchKey) return -1;
        if (NodeKey > SearchKey) return 1;
        return 0;
    };
    typedef KernelMem_DataCacheAllocator<T> TheAllocator;

    typedef Container_AVLNode<PagePtr,TheAllocator> Node;

    typedef Container_AVLTree<Node, PagePtr,TheAllocator> Tree;

    void DeletePage(PagePtr p)
    {
        m_tree.Delete(p);
    }
    void CheckControlledPage(PagePtr p, T* pChunk)
    {
        if (!m_tree.Find(p))
        {
            {
                SAPDB_Int2* pOffset = reinterpret_cast<SAPDB_Int2*> (reinterpret_cast<SAPDB_Byte*>(pChunk) - 4);
                Kernel_VTrace() << "dealloc " << ToStr(pChunk) << " in page " << ToStr(p) << " offset " << *pOffset; 
            }
            if (KernelMem_Page<T>::IsFreeChunk(pChunk))
            {
                KernelMem_Crash ("releasing unused chunk");
            }
            if (!m_allChunks.Find((PagePtr) pChunk))
            {
                 KernelMem_Crash ("wrong chunk (never used)");
            }
            if (!m_allTree.Find(p))
            {
                KernelMem_Crash ("wrong page address (never used)");
            }
            else
            {
                KernelMem_Crash ("wrong page address");
            }
        }
    }
    static void KernelMem_Crash (const char* msg)
    {
        {
            Kernel_VTrace trace;
            trace << msg << NewLine;
            Kernel_OpInfo (csp3_n_memory) << msg;
        }
        vabort (true);
    }
private :
    Tree m_tree;
    Tree m_allTree;
    Tree m_allChunks;

#endif
};

//-----------------------------------------------------------------------------

template <class T>
KernelMem_Page<T>::KernelMem_Page(int SizeOfT, int PageSize)
: m_inUseCount(0)
, m_pageSize(PageSize)
, m_entrySize(0)
, m_entries(0)
{
    m_entrySize       = EntrySize(SizeOfT);
    int offset        = RTE_OFFSET_OF(KernelMem_Page<T>, m_entries);
    m_firstFreeOnPage = offset;
    m_inUseCount      = (PageSize - offset) / m_entrySize;
    Entry* pEntry = &m_entries;
    while (m_inUseCount > 0)
    {
        offset += m_entrySize;
        new ((void*) pEntry) Entry((1 == m_inUseCount) ? 0 : offset); 
        ++pEntry;
        --m_inUseCount;
    }
    KERNELMEM_CHECKPAGE
}

//-----------------------------------------------------------------------------

template <class T>
KernelMem_Page<T>* KernelMem_Page<T>::Create(int SizeOfT)
{
    SAPDBMem_IBlockAllocator& Allocator = FrameCtrl_IBlockAllocator::GetInstance();
    void* p = Allocator.Allocate(1);
    if (p)
    {
        return new(p) KernelMem_Page(SizeOfT, Allocator.GetBlockSize());
    }
    return 0;
}

//-----------------------------------------------------------------------------


template <class T>
void KernelMem_Page<T>::Delete()
{
    this->Unlink();
    SAPDBMem_IBlockAllocator& Allocator = FrameCtrl_IBlockAllocator::GetInstance();
    Allocator.Deallocate(this, 1);
}

//-----------------------------------------------------------------------------

template <class T>
int KernelMem_Page<T>::EntrySize(int SizeOfT)
{
    int offset = RTE_OFFSET_OF(Entry, m_entry);
    return (offset + SizeOfT + offset - 1) & ~(offset - 1);
}

//-----------------------------------------------------------------------------

template <class T>
KernelMem_Page<T>* KernelMem_Page<T>::GetContainingPage(T* p)
{
    Entry* pEntry = reinterpret_cast<Entry*>(reinterpret_cast<SAPDB_Byte*>(p) - RTE_OFFSET_OF(Entry, m_entry));
    SAPDB_Byte* pByte = reinterpret_cast<SAPDB_Byte*>(pEntry) - pEntry->m_offset;
    return reinterpret_cast<KernelMem_Page*>(pByte);
}

//-----------------------------------------------------------------------------

template <class T>
T* KernelMem_Page<T>::GetFreeEntry()
{
    Entry* pFree      = EntryAtOffset(m_firstFreeOnPage);
    {
        Kernel_VTrace() << "Alloc " << ToStr(&pFree->m_entry) << " from " << ToStr(this) << " offset " << m_firstFreeOnPage;
    }
    int firstFree     = m_firstFreeOnPage;
    m_firstFreeOnPage = pFree->m_offset; 
    pFree->m_offset   = firstFree; 
    ++m_inUseCount;
    if (0 == m_firstFreeOnPage)
    {
        // no more free entries on page, remove from free list
        this->Unlink();
    }
    pFree->SetUsed();
    KERNELMEM_CHECKPAGE
    return &pFree->m_entry;
}

//-----------------------------------------------------------------------------

template<class T>
void KernelMem_Page<T>::FreeEntry(T* pFreeEntry, KernelMem_DataCacheAllocator<T>& Allocator)
{
    if (1 == m_inUseCount)
    {
        // this has been the last used entry, return page to data cache
        --Allocator.m_pageCnt;
        ++Allocator.m_baseDeallocCnt;
        this->Delete();
        KERNELMEM_DELETE_PAGE(this);
    }
    else
    {
        if (0 == m_firstFreeOnPage)
        {
            // insert page in list of pages containing free entries
            Allocator.Link(this);
        }
        --m_inUseCount;
        Entry* pEntry = reinterpret_cast<Entry*>(reinterpret_cast<SAPDB_Byte*>(pFreeEntry) - RTE_OFFSET_OF(Entry, m_entry));
        KERNELMEM_CHECK_USED_ENTRY(pEntry, this);
        pEntry->SetFree();
        int offset = pEntry->m_offset;
        pEntry->m_offset  = m_firstFreeOnPage;
        m_firstFreeOnPage = offset;
        KERNELMEM_CHECKPAGE
    }
}

//-----------------------------------------------------------------------------

template <class T>
KernelMem_DataCacheAllocator<T>::KernelMem_DataCacheAllocator(int                 SizeOfT,
                                                              const SAPDB_UTF8*   Identifier,
                                                              bool&               Ok,
                                                              Msg_List&           MsgList)
: m_pLock(0)
, m_spinlockPool ((SAPDB_UTF8 *) "KernelMem_DataCacheAllocator", 1, RTEMem_Allocator::Instance())
, m_sizeOfT(SizeOfT)
, m_allocCnt(0)
, m_deallocCnt(0)
, m_baseAllocCnt(0)
, m_baseDeallocCnt(0)
, m_usedEntries(0)
, m_maxUsedEntries(0)
, m_pageCnt(0)
, m_allocatorInfo(Identifier)
, m_prev(reinterpret_cast<PagePtr>(&m_prev))
, m_next(reinterpret_cast<PagePtr>(&m_prev))
#if defined VERIFY
, m_tree()
, m_allTree()
, m_allChunks()
#endif
{
    SAPDBMem_IBlockAllocator& Allocator = FrameCtrl_IBlockAllocator::GetInstance();
    this->SetAllocatorName(Identifier);
    m_allocatorInfo = RTEMem_AllocatorInfo(&m_name[0], this, Allocator.GetIdentifier());
    RTEMem_AllocatorRegister::Instance().Register(m_allocatorInfo);
    if (sizeof(T) > Allocator.GetBlockSize())
    {
        MsgList.AppendNewMessage (Msg_List(
            Msg_List::Error,
            SDBMSG_KERNELCOMMON_TYPE_TOO_LONG,
            Msg_Arg("BYTE_SIZE", sizeof(T)),
            Msg_Arg("BYTES_USED", Allocator.GetBlockSize())));
        Ok = false;
        return;
    }
    Ok = true;
    m_pLock = RTESync_CreateRWRegion(1, m_spinlockPool, RTEMem_Allocator::Instance());
    if (0 == m_pLock)
    {
        MsgList.AppendNewMessage (Msg_List(
            Msg_List::Error,
            SDBMSG_KERNELCOMMON_NO_MEMORY,
            Msg_Arg("BYTES_NEEDED", RTESync_IRWRegion::sizeOf())));
        Ok = false;
    }
#if defined VERIFY
    m_tree.AdviseCompare(this);
    m_tree.AdviseAllocator(&RTEMem_Allocator::Instance());
    m_allTree.AdviseCompare(this);
    m_allTree.AdviseAllocator(&RTEMem_Allocator::Instance());
    m_allChunks.AdviseCompare(this);
    m_allChunks.AdviseAllocator(&RTEMem_Allocator::Instance());
#endif
}

//------------------------------------------------------------------------------

template <class T>
KernelMem_DataCacheAllocator<T>::~KernelMem_DataCacheAllocator()
{
    RTEMem_AllocatorRegister::Instance().Deregister(m_allocatorInfo);
    if (0 != m_pLock)
    {
        RTESync_DestroyRWRegion(m_pLock, RTEMem_Allocator::Instance());
    }
}

//------------------------------------------------------------------------------

template <class T>
T* KernelMem_DataCacheAllocator<T>::Allocate(RTE_TaskId TaskId)
{
    RTESync_RWLockedScope lockedScope(*m_pLock, true, TaskId);
    ++m_allocCnt;
    PagePtr pPage = m_next;
    if (reinterpret_cast<PagePtr>(&m_prev) == pPage)
    {
        pPage = KernelMem_Page<T>::Create(m_sizeOfT);  
        if (0 == pPage)
        {
            return 0;
        }
        KERNELMEM_INSERT_PAGE(pPage);
        ++m_baseAllocCnt;
        ++m_pageCnt;
        this->Link(pPage);
    }
    T* p = pPage->GetFreeEntry();
    if (p)
    {
        ++m_usedEntries;
        if (m_usedEntries > m_maxUsedEntries)
        {
            m_maxUsedEntries = m_usedEntries;
        }
    }
    KERNELMEM_INSERT_CHUNK(reinterpret_cast<PagePtr>(p))
   
    return p;
}

//-----------------------------------------------------------------------------

template <class T>
void KernelMem_DataCacheAllocator<T>::Deallocate(RTE_TaskId TaskId, T* p)
{
     PagePtr pPage = KernelMem_Page<T>::GetContainingPage(p);
     KERNELMEM_CHECK_CONTROLLED_PAGE(pPage, p);
     RTESync_RWLockedScope lockedScope(*m_pLock, true, TaskId);
     ++m_deallocCnt;
     --m_usedEntries;
     pPage->FreeEntry(p, *this);
}

//-----------------------------------------------------------------------------

template <class T>
void KernelMem_DataCacheAllocator<T>::Link(PagePtr pPage)
{
    pPage->m_next = m_next;
    pPage->m_prev = m_next->m_prev;
    m_next->m_prev = pPage;
    m_next = pPage;
}

//-----------------------------------------------------------------------------

template <class T>
void KernelMem_DataCacheAllocator<T>::SetAllocatorName(const SAPDB_UTF8 * Name)
    {
    RTESync_LockedScope lock(RTEMem_AllocatorRegister::Instance().Spinlock());
    strncpy((char *)&m_name[0], (const char *)Name, sizeof(m_name)-1);
    m_name[sizeof(m_name)-1] = 0;
}

//-----------------------------------------------------------------------------

template <class T>
void KernelMem_DataCacheAllocator<T>::CalcStatistics(SAPDB_ULong &BytesUsed,
                                SAPDB_ULong &MaxBytesUsed,
                                SAPDB_ULong &BytesControlled)
{
    BytesUsed       = (SAPDB_ULong) m_usedEntries * KernelMem_Page<T>::EntrySize(m_sizeOfT);
    MaxBytesUsed    = (SAPDB_ULong) m_maxUsedEntries * KernelMem_Page<T>::EntrySize(m_sizeOfT);;
    BytesControlled = m_pageCnt * FrameCtrl_IBlockAllocator::GetInstance().GetBlockSize();
}

//-----------------------------------------------------------------------------

template <class T>
void KernelMem_DataCacheAllocator<T>::GetBaseAllocatorCallStatistics(SAPDB_ULong &CountAlloc,
                                  SAPDB_ULong &CountDealloc) const
{
    CountAlloc   = (SAPDB_ULong) m_baseAllocCnt;
    CountDealloc = (SAPDB_ULong) m_baseDeallocCnt;
}

//-----------------------------------------------------------------------------

template <class T>
void KernelMem_DataCacheAllocator<T>::GetCallStatistics(SAPDB_ULong &CountAlloc,
                                  SAPDB_ULong &CountDealloc) const
{
    CountAlloc   = (SAPDB_ULong) m_allocCnt;
    CountDealloc = (SAPDB_ULong) m_deallocCnt;
}

//-----------------------------------------------------------------------------

#if defined(VERIFY)

template<class T>
inline void KernelMem_Page<T>::CheckFreeChunk(Entry* pEntry)
{
    if (!pEntry->IsFree()) KernelMem_DataCacheAllocator<T>::KernelMem_Crash ("Free chunk not having free flag");
    int offset = reinterpret_cast<SAPDB_Byte*>(pEntry) - reinterpret_cast<SAPDB_Byte*>(this);  
    if (pEntry->m_offset == offset)  KernelMem_DataCacheAllocator<T>::KernelMem_Crash ("Free chunk having wrong offset");
}
  
//-----------------------------------------------------------------------------   

template<class T>
inline void KernelMem_Page<T>::CheckUsedChunk(Entry* pEntry)
{
    if (pEntry->IsFree()) KernelMem_DataCacheAllocator<T>::KernelMem_Crash ("Used chunk having free flag");
    int offset = reinterpret_cast<SAPDB_Byte*>(pEntry) - reinterpret_cast<SAPDB_Byte*>(this);  
    if (pEntry->m_offset != offset)  KernelMem_DataCacheAllocator<T>::KernelMem_Crash ("Used chunk having wrong offset");
}
  
//-----------------------------------------------------------------------------   

template<class T>
void KernelMem_Page<T>::Check()
{
    int minOffset = RTE_OFFSET_OF(KernelMem_Page<T>, m_entries);
    int maxOnPage = (m_pageSize - minOffset) / m_entrySize;
    if (m_inUseCount > maxOnPage)
    {
        KernelMem_DataCacheAllocator<T>::KernelMem_Crash("isUseCount too big");
    }
    int offset  = RTE_OFFSET_OF(KernelMem_Page<T>, m_entries);
    int freeCnt = 0;
    for (int ix = 0; ix < maxOnPage; ++ix)
    {
         Entry* pEntry = EntryAtOffset(offset);
         if (pEntry->IsFree())
         {
             ++freeCnt;
             KERNELMEM_CHECK_FREE_ENTRY(pEntry,this)
         }
         else
         {
             KERNELMEM_CHECK_USED_ENTRY(pEntry,this)
         }
         offset += m_entrySize;
    }
    int curr = m_firstFreeOnPage; 
    int cnt  = 0;
    while ((curr != 0) && (cnt < maxOnPage))
    {
        if (curr < minOffset || curr > m_pageSize)
        {
             KernelMem_DataCacheAllocator<T>::KernelMem_Crash("offset out of range");
        }
        ++cnt;
        Entry* pFree = EntryAtOffset(curr);
        curr = pFree->m_offset;
    }
    if (cnt > maxOnPage)
    {
        KernelMem_DataCacheAllocator<T>::KernelMem_Crash("too many entries on page");
    }
    else
    {
        if (cnt != freeCnt)
        {
            KernelMem_DataCacheAllocator<T>::KernelMem_Crash("wrong free count");
        }
    }
}
#endif

#endif