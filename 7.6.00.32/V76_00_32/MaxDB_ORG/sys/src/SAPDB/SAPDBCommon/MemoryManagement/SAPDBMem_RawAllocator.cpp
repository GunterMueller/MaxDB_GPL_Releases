/*!***************************************************************************
  module:       SAPDBMem_RawAllocator.cpp

  responsible:  ThomasA

  special area: Kernel

  description:  Implementation of a general purpose allocator.

  copyright:    Copyright (c) 1999-2005 SAP AG

  created:      Wed Jan 12 14:35:56  2000

  updated:      Tue Feb  8 10:49:05  2000



    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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

#include <stdlib.h> 
#include <string.h>
#include <new>

#if defined(KERNEL)
#include "Oms/OMS_DbpError.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_Exceptions.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp"
#include "RunTime/RTE_Crash.hpp"
#include "RunTime/MemoryManagement/RTEMem_AllocatorRegister.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_CallDirectory.hpp"
#include "KernelCommon/Kernel_OpMsg.hpp"
#include "heo52.h" // vtimestamp
#include "heo670.h"
#include "heo66.h" // vfopen etc
#endif
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "RunTime/Synchronisation/RTESync_Spinlock.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_RawAllocator.hpp"
#include "RunTime/RTE_IInterface.hpp"
#include "SAPDBCommon/SAPDB_OStream.hpp"
#include "geo573.h"
#include "hsp77.h"
#include "ggg01.h"
#include "hxt12.h"

#if defined(_REENTRANT) && defined(KERNEL)
/* for sqlgetthreadid() only...until RTEThread::Self is always usable... */
#include "heo07.h" /* nocheck */ 
#endif

#if defined(KERNEL)
extern SAPDBTrace_Topic AllocatorTrace;
#endif

#if defined (SAPDBMEM_RAWALLOCATOR_NAMESPACE)
using namespace SAPDBMEM_RAWALLOCATOR_NAMESPACE;
#endif

// a little bit odd, but here to avoid big changes
geo573_BadAllocHandler::Callback geo573_BadAllocHandler::m_cb = NULL;

/* Macros */

#if defined(WIN32) || defined(OSF1)
#define TRY try 
#define CATCH_ALL(X) catch(...)\
{\
    X;\
    throw;\
}
#define CATCH_DBP_ERROR catch (DbpError&) { throw; }
#else
#define TRY
#define CATCH_ALL(X)
#define CATCH_DBP_ERROR
#endif

// if RESERVE_PTR_SPACE is defined, the space needed for linking a chunk into
// its freelist is never part of the user visible part of a chunk. This yields
// in better stability against errors, but results in more memory requirement
#define RESERVE_PTR_SPACE

#if defined(RESERVE_PTR_SPACE)
#define SET_MIN_FREE_BLOCK_PATTERN(pChunk) \
if (!(m_checkFlags & FL_FILL_FREE_BLOCKS)) \
{ \
  *REINTERPRET_CAST(SAPDB_UInt4*, pChunk->ChunkMem()) = FREE_BLOCK_PATTERN;\
}
#else
SET_MIN_FREE_BLOCK_PATTERN(pChunk)
#endif

#if defined(SAPDB_SLOW) || defined(SAPDB_QUICK)
#define INITIAL_FLAGS  (FL_NO_MANS_LAND + FL_DOUBLE_BOOKKEEPING + FL_CHECK_TREE)
#else
#define INITIAL_FLAGS  (0)
#endif

#define BINBLOCKWIDTH          4   /* bins per block */
#define MAX_SMALLBIN_SIZE      1008
#define SMALLBIN_WIDTH         8
#define ALLOCATOR_TRACELEVEL   8
#define FREE_BLOCK_PATTERN     0xFDFDFDFD
#define NO_MANS_LAND_MASK      0xFEFEFEFE
#define CALL_BAD_ALLOC_HANDLER true
#define DO_GROW                true


#define LOCK_HEAP(LOCKED) if (!LOCKED) { m_lock->Lock(); LOCKED = true; }
#define UNLOCK_HEAP if (m_lock) m_lock->Unlock();

#if !defined(KERNEL)
#if defined(SAPDBERR_ASSERT_STATE)
#undef SAPDBERR_ASSERT_STATE
#endif
#define SAPDBERR_ASSERT_STATE(X) if (!(X)) throw -1;
#endif

#if defined(KERNEL) && defined(SAPDB_SLOW)
#define SAPDBMEM_WRITELN(X,Y,Z) SAPDBTRACE_WRITELN (X,Y,Z)
#else
#define SAPDBMEM_WRITELN(X,Y,Z) { if (m_traceEnabled) SAPDBMem_VTrace(m_tracer) << Z; }
#endif

#define SAPDBMEM_NEVER_RESET_LOCK_COUNTER false
class SAPDBMem_ScopeBlockedForMessageOutput
{
public:
    SAPDBMem_ScopeBlockedForMessageOutput(int &lockCounter, SAPDB_UInt4 &lockOwner, bool doResetInDtor=true)
        : m_DoResetInDtor(doResetInDtor)
        , m_LockCounter(lockCounter)
        , m_LockOwner(lockOwner)
    {
#if defined(_REENTRANT) && defined(KERNEL)
        m_LockOwner = (SAPDB_UInt4)sqlgetthreadid();
#else
        m_LockOwner = 0;
#endif
        ++m_LockCounter;
    }

    ~SAPDBMem_ScopeBlockedForMessageOutput()
    {
        if ( m_DoResetInDtor )
        {
            --m_LockCounter;
            m_LockOwner = 0;
        }
    }
private:
    bool m_DoResetInDtor;
    int &m_LockCounter;
    SAPDB_UInt4 &m_LockOwner;
};

//-----------------------------------------------------------------------------

class SAPDBMem_VTrace : public SAPDB_OStream
{
public:
    SAPDBMem_VTrace (SAPDBMem_RawAllocator::ITracerAndExceptionHandler* pTracer)
        : SAPDB_OStream (m_buf)
        , m_buf(pTracer)
    {
    }
    ~SAPDBMem_VTrace ()
    {
        this->Flush ();
    }
private:
    /*******************************************************************

      Class: LineBuffer

     ******************************************************************/
    class LineBuffer : public SAPDB_StreamBuffer
    {
    public:
        LineBuffer (SAPDBMem_RawAllocator::ITracerAndExceptionHandler* pTracer) 
            : SAPDB_StreamBuffer (m_line, m_line + sizeof(m_line) - 1)
            , m_pTracer(pTracer) 
        {}
        virtual void Overflow ()
        {
            if (m_pTracer)
            {
                const char * start = this->Begin ();
                const char * end   = this->Next ();
                int length = end - start;
                if (length > 0)
                {
                    m_line[length] = 0;
                    m_pTracer->VTrace(m_line);
                }
            }
        }
    private:
        SAPDBMem_RawAllocator::ITracerAndExceptionHandler* m_pTracer;
        char                                               m_line [133];
    };

    /* EndClass: LineBuffer */

private:
    LineBuffer m_buf;
};

//-----------------------------------------------------------------------------

// hash list containing information about used chunks, if
// flag FL_DOUBLE_BOOKKEEPING is active

#if defined (SAPDBMEM_RAWALLOCATOR_NAMESPACE)
namespace SAPDBMEM_RAWALLOCATOR_NAMESPACE
{
#endif
struct SAPDBMem_ChunkDirectoryEntry
{
    SAPDBMem_ChunkDirectoryEntry(SAPDBMem_ChunkDirectoryEntry* next, void* p, size_t size)
        : m_next(next)
        , m_pointer(p)
        , m_size(size)
    {
    }
    
    SAPDBMem_ChunkDirectoryEntry* m_next;
    void*                         m_pointer;
    size_t                        m_size;
};

typedef SAPDBMem_ChunkDirectoryEntry* SAPDBMem_ChunkDirectoryEntryPtr; 

class SAPDBMem_UsedChunkDirectory
{
public :
    SAPDBMem_UsedChunkDirectory()
        : m_entryCount(0)
        , m_headEntries(0)
        , m_free(0)
    {
        m_head        = (SAPDBMem_ChunkDirectoryEntryPtr*) malloc(sizeof(SAPDBMem_ChunkDirectoryEntryPtr) * 2013);
        if (m_head)
        {
            m_headEntries = 2013;
            for (int ix = 0; ix < m_headEntries; ++ix)
            {
                m_head[ix] = NULL;
            }
        }
    }

    ~SAPDBMem_UsedChunkDirectory()
    {
        free(m_head);
        while (m_free)
        {
            void* toFree = m_free;
            m_free       = m_free->m_next;
            free(toFree);
        }
    }

    bool Insert (void* p, size_t size)
    {
        if (!m_head) return false;
        Resize();
        int   slot = HashFunc(p);
        void* pEntry;
        if (0 != m_free)
        {
            pEntry = m_free;
            m_free = m_free->m_next;
        }
        else
        {
            pEntry = malloc(sizeof(SAPDBMem_ChunkDirectoryEntry));
            if (NULL == pEntry)
            {
                return false;
            }
        }
        m_head[slot] = new(pEntry) SAPDBMem_ChunkDirectoryEntry(m_head[slot], p, size);
        ++m_entryCount;
        return true;
    }
    int  Find (void* p) const
    {
        int slot = HashFunc(p);
        SAPDBMem_ChunkDirectoryEntryPtr pCurr = m_head[slot];
        while (pCurr)
        {
            if (p == pCurr->m_pointer)
            {
                return (int) pCurr->m_size;
            }
            pCurr = pCurr->m_next;
        }
        return 0;
    }
    bool Delete (void* p)
    {
        int slot                               = HashFunc(p);
        SAPDBMem_ChunkDirectoryEntryPtr  pCurr = m_head[slot];
        SAPDBMem_ChunkDirectoryEntryPtr* pPrev = &m_head[slot];
        while (pCurr)
        {
            if (p == pCurr->m_pointer)
            {
                *pPrev        = pCurr->m_next;
                pCurr->m_next = m_free; 
                m_free        = pCurr;
                --m_entryCount;
                return true;
            }
            pPrev = &pCurr->m_next;
            pCurr = pCurr->m_next;
        }
        return false;
    }
    void* operator new (size_t size)
    {
        return malloc(size);
    }
    void operator delete(void* p)
    {
        free(p);
    }
private :
    int HashFunc(void* p) const
    {
#if defined(BIT64)
        return (int) (((SAPDB_UInt8) p >> 3) % m_headEntries);
#else
        return ((SAPDB_UInt4) p >> 3) % m_headEntries;
#endif
    }

    void Resize()
    {
        if (m_entryCount > 3 * m_headEntries)
        {
            tsp00_Int4 currHeadEntries   = m_entryCount;
            bool isPrime                 = false;
            int  ix;
            while (!isPrime) {
                currHeadEntries++;
                isPrime = (1 == (currHeadEntries % 2)) ? true : false; 
                ix = 3;
                while (((ix * ix) <= currHeadEntries) && (isPrime)) {
                    isPrime = (0 == (currHeadEntries % ix)) ? false : true;
                    ix += 2;
                }
            }
            SAPDBMem_ChunkDirectoryEntryPtr* newHead;
            SAPDBMem_ChunkDirectoryEntryPtr* oldHead;
            newHead = (SAPDBMem_ChunkDirectoryEntryPtr*) malloc(sizeof(SAPDBMem_ChunkDirectoryEntryPtr) * currHeadEntries);
            if (NULL != newHead)
            {
                oldHead = m_head;
                m_head  = newHead;
                for (ix = 0; ix < currHeadEntries; ++ix)
                {
                    m_head[ix] = NULL;
                }
                int oldHeadEntries  = m_headEntries;
                m_headEntries       = currHeadEntries;
                for (ix = 0; ix < oldHeadEntries; ++ix)
                {
                    SAPDBMem_ChunkDirectoryEntryPtr pCurr = oldHead[ix];
                    while (pCurr)
                    {
                        SAPDBMem_ChunkDirectoryEntryPtr p = pCurr;
                        pCurr = pCurr->m_next;
                        p->m_next = m_free;
                        m_free    = p;
                        Insert(p->m_pointer, p->m_size);
                    }
                }
                free(oldHead);
            }
        }
    }
    int                              m_entryCount;
    int                              m_headEntries;
    SAPDBMem_ChunkDirectoryEntryPtr  m_free;
    SAPDBMem_ChunkDirectoryEntryPtr* m_head;
};

#if defined (SAPDBMEM_RAWALLOCATOR_NAMESPACE)
};
#endif

// end of hash list

/* size field is or'ed with PREV_INUSE when previous adjacent chunk in use */

#define PREV_INUSE 0x1 

/* size field is or'ed with RAW_CHUNK_MARK if the chunk is last of raw chunk, PTS 1109340 */

#define RAW_CHUNK_MARK  0x2

/* size field is or'ed with CALL_STACK_AVAILABLE if the chunk contains call stack information */

#define CALL_STACK_AVAILABLE 0x4 

#define SIZE_BITS 0xE0000007 

#define MAX_CALL_STACK_DEPTH 32

#define MALLOC_ALIGNMENT       (sizeof(SAPDBMem_ChunkSizeInfo))
#define MALLOC_ALIGN_MASK      (MALLOC_ALIGNMENT - 1)

#if defined(RESERVE_PTR_SPACE)
#define MINSIZE                (sizeof(SAPDBMem_RawAllocator::CChunk)+8)
#else
#define MINSIZE                (sizeof(SAPDBMem_RawAllocator::CChunk))
#endif

// structure of a node buffer containing nodes of big chunk freelist

struct SAPDBMem_RawAllocator::TNodeBuffer {
  TNodeBuffer*     m_next;
  SAPDB_UInt4      m_blockCount;
#if defined(BIT64)
  SAPDB_UInt4  m_filler;
#endif
  TNodeBuffer(SAPDB_UInt4 blockCount, TNodeBuffer* next)
    : m_next(next)
    , m_blockCount(blockCount)
    {}
};

class SAPDBMem_ChunkSizeInfo {
public :
  SAPDBMem_RawAllocator::ChunkSize   prev_size;      /* Size of previous chunk (if free). */
  SAPDBMem_RawAllocator::ChunkSize   size;           /* Size in bytes, including overhead. */
  SAPDBMem_ChunkSizeInfo() : prev_size(0), size(0) {}
};

/*!
  class: CChunk

  description: Defined the structure and methods of a chunk or memory of a SAPDBMem_RawAllocator

*/
class SAPDBMem_RawAllocator::CChunk : public SAPDBMem_ChunkSizeInfo
{
public :
  SAPDBMem_RawAllocator::CChunkPtr       next;
  SAPDBMem_RawAllocator::CChunkPtr       prev;      /* double links */

#ifdef DEBUG_BABDL
  INLINE void CheckAllocatedChunk(SAPDBMem_RawAllocator* heap, SAPDBMem_RawAllocator::ChunkSize size);
#endif
/*! 
    function: ClearPrevInUseAndGetChunkSize 

    description: returns the size of a chunk
*/
  INLINE SAPDBMem_RawAllocator::ChunkSize ClearPrevInUseAndGetChunkSize();
/*! 
    function: ChunkSize 

    description: returns the size of a chunk
*/
  INLINE SAPDBMem_RawAllocator::ChunkSize ChunkSize();
/*! 
    function: ChunkAtOffset

    description: returns a pointer to the chunk starting at offset bytes from the current chunk
*/
  INLINE SAPDBMem_RawAllocator::CChunkPtr ChunkAtOffset(SAPDBMem_RawAllocator::ChunkSize offset);
/*! 
    function: ChunkAtNegativeOffset

    description: returns a pointer to the chunk starting at -offset bytes from the current chunk
*/
  INLINE SAPDBMem_RawAllocator::CChunkPtr ChunkAtNegativeOffset(SAPDBMem_RawAllocator::ChunkSize offset);

/*! 
    function: ChunkMem

    description: returns a pointer to user part of the chunk
*/
  INLINE void* ChunkMem() const;
/*!
  function : First

  description : returns the first chunk of free list, used only if 'this' is a free list header
*/
  INLINE SAPDBMem_RawAllocator::CChunkPtr First() const;
  INLINE SAPDBMem_RawAllocator::CChunkPtr FirstInRawChunkPtr() const; /* PTS 1109340 */
/*!
  function    : GetAllocator
  description : returns the allocator containing the chunk
*/
  INLINE SAPDBMem_RawAllocator* GetAllocator() const;
/*!
  function : GetRawChunkPtr

  description : can only be applied to the first logical chunk in a raw chunk. Returns the address of that
                raw chunk.
*/
  INLINE SAPDBMem_RawChunkTreeAllocator::Node* GetRawChunkPtr();  /* PTS 1109340 */
/*!
  function : InUse

  description : returns true, if current chunk is allocated; false if chunk is free
*/
  INLINE bool InUse();

/*!
  function : IsCallStackAvailable()
  description : returns true, if current chunk contains call stack information; false otherwise
*/
  INLINE bool IsCallStackAvailable();
/*!
  function : IsRawChunkEndMark

  description : returns true, if current chunk last of a raw chunk
*/
  INLINE bool IsRawChunkEndMark();
/*!
  function : Last

  description : returns the last chunk of free list, used only if 'this' is a free list header
*/
  INLINE SAPDBMem_RawAllocator::CChunkPtr Last() const;
/*! 
  function : NextChunk

  description : returns a pointer to the next chunk of current chunk
*/
  INLINE SAPDBMem_RawAllocator::CChunkPtr NextChunk();

  INLINE SAPDBMem_RawAllocator::CChunkPtr NextChunk(
    SAPDBMem_RawAllocator::ChunkSize);
    /*!
  function : NextFreeList

  description : returns a pointer to the next free list. Only if current chunk is a free list header.
*/
  INLINE SAPDBMem_RawAllocator::CChunkPtr NextFreeList();
/*!
  function PrevChunk

  description : returns a pointer to the next chunk of current chunk
*/
  INLINE SAPDBMem_RawAllocator::CChunkPtr PrevChunk();
/*!
  function : PrevFreeList

  description : returns a pointer to the prev free list. Only if current chunk is a free list header.
*/
  INLINE SAPDBMem_RawAllocator::CChunkPtr PrevFreeList();
/*!
  function : PrevInUse

  description : returns true, if predecessor of current chunk is in use
*/
  INLINE bool PrevInUse();
/*!
  function : SetAllocator

  description : writes the pointer of the allocator managing the chunk into chunk header
*/
  INLINE void SetAllocator(SAPDBMem_RawAllocator*);
/*!
  function : SetHead

  description : writes size of chunk into chunk header
*/
  INLINE void SetHead(SAPDBMem_RawAllocator::ChunkSize sz);
  /*!
  function : SetHeadAndFoot

  description : writes size of chunk into chunk header
*/
  INLINE void SetHeadAndFoot(SAPDBMem_RawAllocator::ChunkSize sz);

  INLINE bool InUseBitAtOffset(SAPDBMem_RawAllocator::ChunkSize offset);

  INLINE void SetCallStackAvailable();

  INLINE void SetInUseAtOffset(SAPDBMem_RawAllocator::ChunkSize offset);
  
/*!
  function : Unlink

  description : removes current chunk from free list
*/
  INLINE void Unlink(SAPDBMem_RawAllocator&);
/*
  function : Mem2Chunk

  description : for a given pointer to the user part of a chunk the pointer to the chunk is returned
*/
  INLINE static SAPDBMem_RawAllocator::CChunkPtr Mem2Chunk(void *mem);
};

//-----------------------------------------------------------------------------
#if defined (SAPDBMEM_RAWALLOCATOR_NAMESPACE)
namespace SAPDBMEM_RAWALLOCATOR_NAMESPACE
{
#endif
class SAPDBMem_DelayedFreeHandler
{
public :
    SAPDBMem_DelayedFreeHandler (SAPDBMem_RawAllocator& Allocator)
        : m_allocator(Allocator)
        , m_entryCount(0)
        , m_currIndex(0)
    {
    }
    void Insert (SAPDBMem_RawAllocator::CChunkPtr p)
    {
        if (m_entryCount == MAX_DELAYED)
        {
           
            m_allocator.CheckFreeChunk (m_freeArray[m_currIndex]);
            if (m_allocator.m_checkFlags & SAPDBMem_RawAllocator::FL_NO_MANS_LAND)
            {
                SAPDB_UInt4* pI4  = REINTERPRET_CAST(SAPDB_UInt4*, p->NextChunk());
                if (p->IsCallStackAvailable())
                {
                    --pI4;
                }
                *pI4 = NO_MANS_LAND_MASK;
            }
            m_allocator.Deallocate(m_freeArray[m_currIndex]->ChunkMem());
        }
        else
        {
            ++m_entryCount;
        }
        m_freeArray[m_currIndex] = p;
        ++m_currIndex;
        if (MAX_DELAYED == m_currIndex)
        {
            m_currIndex = 0;
        }
    }
private :
    enum { MAX_DELAYED = 512 };
    SAPDBMem_RawAllocator&           m_allocator;
    int                              m_entryCount;
    int                              m_currIndex;
    SAPDBMem_RawAllocator::CChunkPtr m_freeArray[MAX_DELAYED];
};
#if defined (SAPDBMEM_RAWALLOCATOR_NAMESPACE)
};
#endif

/*===========================================================================*
*  IMPLEMENTATION OF SAPDBMem_RawAllocator::CChunk methods                                                                *
*===========================================================================*/

INLINE SAPDBMem_RawAllocator::ChunkSize SAPDBMem_RawAllocator::CChunk::ClearPrevInUseAndGetChunkSize() {
  size &= ~(PREV_INUSE);
  return ChunkSize();
}

INLINE SAPDBMem_RawAllocator::ChunkSize SAPDBMem_RawAllocator::CChunk::ChunkSize() {
  return (size & ~(SIZE_BITS));
}

INLINE SAPDBMem_RawAllocator::CChunkPtr SAPDBMem_RawAllocator::CChunk::ChunkAtOffset(SAPDBMem_RawAllocator::ChunkSize offset) {
  return ((SAPDBMem_RawAllocator::CChunkPtr)(((char*)(this)) + (offset)));
}

INLINE SAPDBMem_RawAllocator::CChunkPtr SAPDBMem_RawAllocator::CChunk::ChunkAtNegativeOffset(SAPDBMem_RawAllocator::ChunkSize offset) {
  return ((SAPDBMem_RawAllocator::CChunkPtr)(((char*)(this)) - (offset)));
}

INLINE SAPDBMem_RawAllocator::CChunkPtr SAPDBMem_RawAllocator::CChunk::First() const {
  return next;
}

/* PTS 1109340 */
INLINE SAPDBMem_RawAllocator::CChunkPtr SAPDBMem_RawAllocator::CChunk::FirstInRawChunkPtr() const {
  return (SAPDBMem_RawAllocator::CChunkPtr) (((char*) this) - prev_size);
}


/* PTS 1109340 */
INLINE SAPDBMem_RawChunkTreeAllocator::Node* SAPDBMem_RawAllocator::CChunk::GetRawChunkPtr() {
  return REINTERPRET_CAST(SAPDBMem_RawChunkTreeAllocator::Node*,
    (REINTERPRET_CAST(char*, this) - SAPDBMem_RawAllocator::RawChunkHeaderSize())); 
}

INLINE SAPDBMem_RawAllocator* SAPDBMem_RawAllocator::CChunk::GetAllocator() const
{
    return REINTERPRET_CAST(SAPDBMem_RawAllocator*, next); 
}

INLINE bool SAPDBMem_RawAllocator::CChunk::InUse() {
  return ((((SAPDBMem_RawAllocator::CChunkPtr)(((char*)(this))+(ChunkSize())))->size) & PREV_INUSE);
}

INLINE bool SAPDBMem_RawAllocator::CChunk::IsCallStackAvailable() {
  return ((size & CALL_STACK_AVAILABLE) > 0);
}

/* PTS 1109340 */
INLINE bool SAPDBMem_RawAllocator::CChunk::IsRawChunkEndMark() {
  return ((size & RAW_CHUNK_MARK) > 0);
}

INLINE SAPDBMem_RawAllocator::CChunkPtr SAPDBMem_RawAllocator::CChunk::Last() const {
  return prev;
}

INLINE SAPDBMem_RawAllocator::CChunkPtr SAPDBMem_RawAllocator::CChunk::NextChunk() {
  return ((SAPDBMem_RawAllocator::CChunkPtr)( ((char*)(this)) + (ChunkSize()) ));
}

INLINE SAPDBMem_RawAllocator::CChunkPtr SAPDBMem_RawAllocator::CChunk::NextChunk(SAPDBMem_RawAllocator::ChunkSize sz) {
  return ((SAPDBMem_RawAllocator::CChunkPtr)( ((char*)(this)) + sz ) );
}

INLINE SAPDBMem_RawAllocator::CChunkPtr SAPDBMem_RawAllocator::CChunk::NextFreeList() {
  return REINTERPRET_CAST (SAPDBMem_RawAllocator::CChunkPtr,
    REINTERPRET_CAST(char*, this) + 2 * sizeof(SAPDBMem_RawAllocator::CChunkPtr));
}

INLINE SAPDBMem_RawAllocator::CChunkPtr SAPDBMem_RawAllocator::CChunk::PrevChunk() {
  return ((SAPDBMem_RawAllocator::CChunkPtr)( ((char*)(this)) - (prev_size) ));
}

INLINE SAPDBMem_RawAllocator::CChunkPtr SAPDBMem_RawAllocator::CChunk::PrevFreeList() {
  return REINTERPRET_CAST (SAPDBMem_RawAllocator::CChunkPtr,
    REINTERPRET_CAST(char*, this) - 2 * sizeof(SAPDBMem_RawAllocator::CChunkPtr));
}

INLINE bool SAPDBMem_RawAllocator::CChunk::PrevInUse() {
  return (this->size & PREV_INUSE);
}

INLINE void SAPDBMem_RawAllocator::CChunk::SetAllocator(SAPDBMem_RawAllocator* pAlloc)
{
    next = REINTERPRET_CAST(CChunkPtr, pAlloc); 
    prev = REINTERPRET_CAST(CChunkPtr, pAlloc->m_leftFencePattern);
}

INLINE void SAPDBMem_RawAllocator::CChunk::SetHead(SAPDBMem_RawAllocator::ChunkSize sz) {
  //    SAPDBERR_ASSERT_STATE(sz >= 8);
  size = sz;
}

INLINE void SAPDBMem_RawAllocator::CChunk::SetHeadAndFoot(SAPDBMem_RawAllocator::ChunkSize sz) {
  this->size = sz | PREV_INUSE;
  ((SAPDBMem_RawAllocator::CChunkPtr)((char*)(this) + (sz)))->prev_size = sz;
}

INLINE bool SAPDBMem_RawAllocator::CChunk::InUseBitAtOffset(SAPDBMem_RawAllocator::ChunkSize offset) {
  return (((SAPDBMem_RawAllocator::CChunkPtr)(((char*)(this)) + (offset)))->size & PREV_INUSE);
}

INLINE void SAPDBMem_RawAllocator::CChunk::SetInUseAtOffset(SAPDBMem_RawAllocator::ChunkSize offset) {
  (((SAPDBMem_RawAllocator::CChunkPtr)(((char*)(this)) + offset))->size |= PREV_INUSE);
}

INLINE void SAPDBMem_RawAllocator::CChunk::SetCallStackAvailable()
{
  this->size |= CALL_STACK_AVAILABLE;
}

/*-------------------------------------------------------------------*/

INLINE void* SAPDBMem_RawAllocator::AllocateResult(CChunk* pChunk, int monitorCallStackLevel) 
{
   
    if (m_checkFlags & FL_DOUBLE_BOOKKEEPING)
    {
        if (!m_usedChunks->Insert(pChunk->ChunkMem(), pChunk->ChunkSize()))
        {
            // not enough memory for double bookkeeping, turn it off
            Trace ("double bookkeeping turned off"); 
            m_checkFlags -= FL_DOUBLE_BOOKKEEPING;
            delete m_usedChunks;
            m_usedChunks = 0;
        }
    }
    UNLOCK_HEAP;
#if defined(KERNEL)
    if (monitorCallStackLevel > 0)
    {
        void* callArray[MAX_CALL_STACK_DEPTH];
        int filledLevel    = eo670_FillCallArray(monitorCallStackLevel, &callArray[0]);
        SAPDB_UInt4* pInt4 = REINTERPRET_CAST(SAPDB_UInt4*, pChunk->NextChunk());
        *pInt4 = SAPDBMem_CallDirectory::Instance().GetCallId(filledLevel, &callArray[0]);
        pChunk->SetCallStackAvailable();
    }
#endif
    if (m_checkFlags > 0)
    {
        CheckUsage();
        if (m_checkFlags & FL_NO_MANS_LAND)
        {
            SAPDB_UInt4* pI4  = REINTERPRET_CAST(SAPDB_UInt4*, pChunk->NextChunk());
            if (pChunk->IsCallStackAvailable())
            {
                --pI4;
            }
            *pI4 = NO_MANS_LAND_MASK;
        }
    }
    pChunk->SetAllocator(this);
    void* p = pChunk->ChunkMem();

    SAPDBMEM_WRITELN (AllocatorTrace, ALLOCATOR_TRACELEVEL, 
        "At " << ToStr(p) << " " << pChunk->ChunkSize() << " bytes");

    return p;
}

/*-------------------------------------------------------------------*/

INLINE void* SAPDBMem_RawAllocator::CChunk::ChunkMem() const
{
#if defined(RESERVE_PTR_SPACE)
    return ((void*)((char*)(this) + sizeof(CChunk)));
#else
    return ((void*)((char*)(this) + sizeof(SAPDBMem_ChunkSizeInfo)));
#endif
}

/*-------------------------------------------------------------------*/

INLINE SAPDBMem_RawAllocator::CChunkPtr SAPDBMem_RawAllocator::CChunk::Mem2Chunk(void *mem) 
{
#if defined(RESERVE_PTR_SPACE)
  return ((SAPDBMem_RawAllocator::CChunkPtr)((char*)(mem) - sizeof(CChunk)));
#else
  return ((SAPDBMem_RawAllocator::CChunkPtr)((char*)(mem) - sizeof(SAPDBMem_ChunkSizeInfo)));
#endif
}

#ifdef DEBUG_BABDL

INLINE void SAPDBMem_RawAllocator::CChunk::CheckAllocatedChunk(SAPDBMem_RawAllocator* heap, SAPDBMem_RawAllocator::ChunkSize CheckSize) {
  SAPDBMem_RawAllocator::ChunkSize sz = CheckSize & ~PREV_INUSE;
  long room = sz - CheckSize;

  // heap->CheckInUseChunk(this); TODO

  /* Legal size ... */
  SAPDBERR_ASSERT_STATE((long)sz >= (long)MINSIZE);
  SAPDBERR_ASSERT_STATE((sz & MALLOC_ALIGN_MASK) == 0);
  SAPDBERR_ASSERT_STATE(room >= 0);
  SAPDBERR_ASSERT_STATE(room < (long)MINSIZE);
  /* ... and alignment */
  //SAPDBERR_ASSERT_STATE(aligned_OK(chunk2mem(p)));
  /* ... and was allocated at front of an available chunk */
  SAPDBERR_ASSERT_STATE(this->PrevInUse());
  SAPDB_UInt4* pInt4 = REINTERPRET_CAST(SAPDB_UInt4*, this->NextChunk());
  SAPDBERR_ASSERT_STATE((*pInt4) == (SAPDB_UInt4) NO_MANS_LAND_MASK);
}

#endif

#if defined (SAPDBMEM_RAWALLOCATOR_NAMESPACE)
namespace SAPDBMEM_RAWALLOCATOR_NAMESPACE
{
#endif
struct TRawChunkFoot {
  TRawChunkFoot(SAPDBMem_RawAllocator::CChunkPtr pRawChunk) /* PTS 1109340 */ 
  {
    REINTERPRET_CAST(SAPDBMem_RawAllocator::CChunkPtr, &m_firstChunk )->SetHead(sizeof(m_firstChunk));
    REINTERPRET_CAST(SAPDBMem_RawAllocator::CChunkPtr, &m_secondChunk)->SetHead(sizeof(m_secondChunk) | PREV_INUSE | RAW_CHUNK_MARK);
    m_secondChunk.prev_size = 
      (REINTERPRET_CAST(char*, &m_secondChunk) - REINTERPRET_CAST(char*, pRawChunk));
  }
  SAPDBMem_ChunkSizeInfo m_firstChunk;
  SAPDBMem_ChunkSizeInfo m_secondChunk;
};

// Node definition of the tree of free big chunks
// caution : it must be possible to interpret the node as as free chunk, i.e.
//           next and prev must have the same offset as in struct CChunk

struct SAPDBMem_TreeNode
{
  SAPDBMem_TreeNode*  m_left;
#if !defined(BIT64)
  SAPDBMem_TreeNode*  m_right;
#endif
  SAPDBMem_RawAllocator::CChunkPtr next;
  SAPDBMem_RawAllocator::CChunkPtr prev;
#if defined(BIT64)
  SAPDBMem_TreeNode*  m_right;
#endif
  SAPDBMem_TreeNode** m_parent;
  size_t              m_size;
};
#if defined (SAPDBMEM_RAWALLOCATOR_NAMESPACE)
};
#endif

/*-------------------------------------------------------------------*/

// binary tree holding big free chunks. Each node of the tree
// is the header of a double chained list containing chunks of the
// same size. The tree is ordered by chunk sizes.


INLINE void SAPDBMem_RawAllocator::InsertFreeBigChunk(CChunkPtr p)
{
    // insert a free chunk into the tree
    NodePtr* pPrev = &m_root;
    NodePtr pCurr;
    while (0 != (*pPrev))
    {
        pCurr = *pPrev;
        if (p->ChunkSize() == pCurr->m_size)
        {
            // node for this size already in tree, append chunk to 
            // list
            CChunkPtr prv = pCurr->prev;
            p->prev       = prv;
            p->next       = prv->next;
            pCurr->prev   = p;
            prv->next     = p;
            return;
        }
        else // keep on searching
        {
            if (p->ChunkSize() < pCurr->m_size)
            {
                pPrev = &pCurr->m_left;
            }
            else
            {
                pPrev = &pCurr->m_right;
            }
        }
    }
    // node of this size not in tree, insert node with list
    // containing chunk
    if (0 == m_firstFreeNode)
    {
        this->NewNodeBuffer();
        if (0 == m_firstFreeNode)
        {
            // emergency code, if no more memory is available
            this->InsertChunkIntoSortedList(p);
            return;
        }
    }
    // return first node from free list
    NodePtr pNode   = m_firstFreeNode;
    m_firstFreeNode = m_firstFreeNode->m_right;
    pNode->m_left   = 0;
    pNode->m_right  = 0;
    pNode->m_parent = pPrev;
    pNode->next     = p;
    pNode->prev     = p;
    pNode->m_size   = p->ChunkSize();
    p->next         = REINTERPRET_CAST(CChunkPtr, pNode);
    p->prev         = REINTERPRET_CAST(CChunkPtr, pNode);
    *pPrev          = pNode;
    if (FL_CHECK_TREE & m_checkFlags)
    {
        this->TreeCheck();
    }
}

/*-------------------------------------------------------------------*/
    
void SAPDBMem_RawAllocator::InsertChunkIntoSortedList(CChunkPtr pChunk)
{
    CChunkPtr prv = REINTERPRET_CAST(CChunkPtr, &m_root);                                                         
    CChunkPtr nxt = m_next;
    SAPDBMem_RawAllocator::ChunkSize ChunkSize = pChunk->ChunkSize();
    while (nxt != prv && ChunkSize < nxt->ChunkSize()) 
    {
        nxt = nxt->next;
    }
    prv          = nxt->prev;                                                            
    pChunk->prev = prv;                                                               
    pChunk->next = nxt;
    prv->next    = pChunk;
    nxt->prev    = pChunk;
}

/*-------------------------------------------------------------------*/

void SAPDBMem_RawAllocator::SubTreeCheck(NodePtr p, NodePtr* followed)
{
    if (0 == p)
    {
        return;
    }
    if (followed != p->m_parent)
    {
        this->Trace("invalid parent pointer %p, expected %p", followed, p->m_parent);
        this->Crash("invalid parent");
    }
    CChunkPtr pList = p->next;
    while (pList != REINTERPRET_CAST(CChunkPtr, p))
    {
        const bool abort = true;
        if ((0 == pList) || (!this->CheckPointer(pList, !abort)))
        {
            this->Trace("list pointer out of range %p", pList);
            this->Crash("list pointer out of range");
        }
        if (pList->ChunkSize() != p->m_size)
        {
            this->Trace("wrong chunk size in list, %d, expected %d", pList->ChunkSize(), p->m_size);
            this->Crash("wrong chunk size");
        }
        pList = pList->next;
    }
    SubTreeCheck(p->m_left,  &p->m_left);
    SubTreeCheck(p->m_right, &p->m_right);
}

/*-------------------------------------------------------------------*/

void SAPDBMem_RawAllocator::ThrowException()
{
    if (m_tracer)
    {
        m_tracer->Exception("SAPDBMem_RawAllocator::Exception");
    }
}

/*-------------------------------------------------------------------*/

void  SAPDBMem_RawAllocator::Trace(const char* fmt, ...)
{
    va_list args;
    va_start (args, fmt);
    char buffer[256];

    sp77vsprintf ( buffer, sizeof(buffer), fmt, args );
#if defined(KERNEL)
    if (!m_tracer)
    {
        Kernel_OpError (csp3_no_more_memory, csp3_n_memory) << buffer;
    }
    else
#endif
    if (m_tracer)
    {

        m_tracer->Trace(buffer);
    }
    va_end (args);
}

/*-------------------------------------------------------------------*/

void  SAPDBMem_RawAllocator::TraceRawChunks()
{
    cgg250AvlTree<SAPDBMem_RawChunkHeader,
        SAPDBMem_RawChunkTreeComparator,
        SAPDBMem_RawChunkTreeAllocator>::Iterator rawIter = m_rawChunkTree.First();
    while (rawIter)
    {
        const SAPDBMem_RawChunkHeader* rch = rawIter();
        this->Trace ("Raw Chunk from %p to %p, size %d", rch->m_lwb, rch->m_upb, rch->m_upb - rch->m_lwb);
        ++rawIter;
    }
}

/*-------------------------------------------------------------------*/

void SAPDBMem_RawAllocator::TraceSubTree(NodePtr p)
{
    if (NULL == p)
    {
        return;
    }
    TraceSubTree(p->m_left);
    int cnt = 0;
    CChunkPtr pList = p->next;
    while (pList != REINTERPRET_CAST(CChunkPtr, p))
    {
        ++cnt;
        pList = pList->next;
    }
    if (cnt > 0)
    {
#if defined(KERNEL)
         Kernel_OpInfo (csp3_n_memory) << "free blocks size " << p->m_size << " : " << cnt;
#endif
    }
    TraceSubTree(p->m_right);

}

/*-------------------------------------------------------------------*/

void SAPDBMem_RawAllocator::TraceTree()
{
  TraceSubTree(m_root);
}

/*-------------------------------------------------------------------*/

void SAPDBMem_RawAllocator::TreeCheck()
{
    SubTreeCheck(m_root, &m_root);
}

/*-------------------------------------------------------------------*/

INLINE bool SAPDBMem_RawAllocator::TreeFindFreeChunk(CChunkPtr p)
{
    SAPDBMem_RawAllocator::ChunkSize size  = p->ChunkSize();
    NodePtr pCurr = m_root;
    // binary search for best fitting free list
    while (true)
    {
        if (0 == pCurr)
        {
            return false;
        }
        if (pCurr->m_size == size)
        {
            CChunkPtr pList = pCurr->next;
            while (pList != REINTERPRET_CAST(CChunkPtr, pCurr))
            {
                if (pList == p)
                {
                    return true;
                }
                pList = pList->next;
            }
        }
        else
        {
            if (pCurr->m_size < size)
            {
                pCurr = pCurr->m_right;
            }
            else
            {
                pCurr = pCurr->m_left;
            }
        }
    }
}

/*-------------------------------------------------------------------*/

INLINE void SAPDBMem_RawAllocator::TreeRemoveNode(NodePtr pNode)
{
    if (pNode == REINTERPRET_CAST(NodePtr, &m_root))
    {
        // don't delete the root
        return;
    }
    SAPDBERR_ASSERT_STATE(*pNode->m_parent == pNode);
    
    if (0 == pNode->m_left)
    {
        // left subtree empty
        if (0 == pNode->m_right)
        {
            // left and right subtree empty
            *pNode->m_parent = 0;
        }
        else
        {
            *pNode->m_parent = pNode->m_right;
            pNode->m_right->m_parent = pNode->m_parent;
        }
    }
    else
    {
        if (0 == pNode->m_right)
        {
            *pNode->m_parent = pNode->m_left;
            pNode->m_left->m_parent = pNode->m_parent;
        }
        else
        {
            // left and right subtree exist, substitute pNode by smallest
            // node of right subtree
            NodePtr  pSmallest = pNode->m_right;
            while (pSmallest->m_left != 0)
            {
                pSmallest = pSmallest->m_left;
            }
            if (0 != pSmallest->m_right)
            {
                *pSmallest->m_parent         = pSmallest->m_right;
                pSmallest->m_right->m_parent = pSmallest->m_parent;
            }
            else
            {
                *pSmallest->m_parent = 0;
            }
            pSmallest->m_left    = pNode->m_left;
            pSmallest->m_right   = pNode->m_right;
            pSmallest->m_parent  = pNode->m_parent;
            *pSmallest->m_parent = pSmallest;
            if (0 != pSmallest->m_left)
            {
                pSmallest->m_left->m_parent = &pSmallest->m_left;
            }
            if (0 != pSmallest->m_right)
            {
                pSmallest->m_right->m_parent = &pSmallest->m_right;
            }
        }
    }
    // insert node into free list
    pNode->m_right  = m_firstFreeNode;
    m_firstFreeNode = pNode;
    if (FL_CHECK_TREE & m_checkFlags)
    {
        this->TreeCheck();
    }
}

/*-------------------------------------------------------------------*/

INLINE void SAPDBMem_RawAllocator::RemoveFreeBigChunk(CChunkPtr p)
{
    if (p->next == p->prev)
    {
        // list becomes empty, remove from tree
        this->TreeRemoveNode(REINTERPRET_CAST(NodePtr, p->prev));
    }
    else
    {
        // remove chunk from list
        p->next->prev = p->prev;
        p->prev->next = p->next;
    }
}

/*-------------------------------------------------------------------*/

class SAPDBMem_SyncObject
{
public :
  SAPDBMem_SyncObject(RTESync_Spinlock* pSpinLock)
    : m_pSpinLock(pSpinLock)
  {
    if (0 != m_pSpinLock)
    {
      m_pSpinLock->Lock();
    }
  }
  void Unlock()
  {
      if (0 != m_pSpinLock)
      {
          m_pSpinLock->Unlock();
          m_pSpinLock = 0;
      }
  }
  ~SAPDBMem_SyncObject()
  {
    if (0 != m_pSpinLock)
    {
      m_pSpinLock->Unlock();
    }
  }
private :
  RTESync_Spinlock* m_pSpinLock;
};

/*-------------------------------------------------------------------*/

INLINE void SAPDBMem_RawAllocator::CChunk::Unlink(SAPDBMem_RawAllocator& allocator)
{
  if (ChunkSize() < MAX_SMALLBIN_SIZE) 
  { 
    SAPDBMem_RawAllocator::CChunkPtr pPrev = prev;
    SAPDBMem_RawAllocator::CChunkPtr pNext = next;  
    pPrev->next = pNext;
    pNext->prev = pPrev;
  }
  else
  {
    allocator.RemoveFreeBigChunk(this);
  }
}

/*-------------------------------------------------------------------*/

SAPDBMem_RawAllocator::CFreeChunkLists::CFreeChunkLists() {
  int ix;
  int jx = 0;
  
  for (ix = 0; ix < 128; ix++) {
    m_freeLists[jx].m_next = operator[](ix);
    m_freeLists[jx].m_prev = operator[](ix);
    ++jx;
  }
}

/*-------------------------------------------------------------------*/

INLINE SAPDBMem_RawAllocator::CChunkPtr SAPDBMem_RawAllocator::CFreeChunkLists::operator[] (int ix) {
  return REINTERPRET_CAST(SAPDBMem_RawAllocator::CChunkPtr,
    REINTERPRET_CAST(unsigned char*, &m_freeLists[ix]) - sizeof(SAPDBMem_ChunkSizeInfo));
}

/*-------------------------------------------------------------------*/

INLINE int SAPDBMem_RawAllocator::CFreeChunkLists::GetSmallFreeListIndex(SAPDBMem_RawAllocator::ChunkSize size) {
  return ((SAPDB_UInt4) size) >> 3;
}

/*-------------------------------------------------------------------*/

SAPDBMem_RawAllocator::SAPDBMem_RawAllocator
(
    const SAPDB_UTF8       *   Identifier,
    SAPDBMem_IRawAllocator &   RawAllocator,
    SAPDB_ULong                FirstBlockSize,
    SAPDB_ULong                SupplementBlockSize,
    FreeRawExtendsEnum         FreeRawExtends,
    SAPDB_ULong                MaxSize,
    bool                       DoRegister
 )
 : m_this(0)
 , m_BlockAllocator(NULL)
 , m_RawAllocator(&RawAllocator)
 , m_tracer(NULL)
 , m_delayedFreeHandler(NULL)
 , m_leftFencePattern(NULL)
 , m_firstSize(FirstBlockSize) 
 , m_supplement(SupplementBlockSize) 
 , m_maxHeap(MaxSize) 
 , m_freeLists()
 , m_binBlocks(0)
 , m_checkFlags(GetCheckLevel())
 , m_maxChunkSize(0xFFFFFFFF)
 , m_heapInfo()
 , m_checkHeap(false)
 , m_freeRawExtends(FreeRawExtends)
 , m_opMessages(true)
 , m_writeAllocateFailedOpMsg(true)
 , m_writeProtected(false)
 , m_doDeregister(DoRegister)
 , m_monitorCallStackLevel(0)
 , m_failedAllocCount(0)
 , m_errorCount(0)
 , m_taskId(RTE_UnknownTaskId)
 , m_badAllocFunc(SAPDBMem_RawAllocator::InitBadAllocHandler())
 , m_lock(0)
 , m_AllocatorInfo(Identifier)
 , m_usedChunks(NULL)
 , m_root(0)
 , m_next(REINTERPRET_CAST(CChunkPtr, &m_root))
 , m_prev(REINTERPRET_CAST(CChunkPtr, &m_root))
 , m_firstFreeNode(0)
 , m_freeNodeBuffer(0)
 , m_rawChunkTree(&m_rawChunkTreeComparator, &m_rawTreeAllocator)
 , m_BlockedForMessageOutput(0)
 , m_BlockingThreadId(0)
 , m_traceEnabled(false)
{
    if (m_checkFlags & FL_DOUBLE_BOOKKEEPING)
    {
        m_usedChunks = new SAPDBMem_UsedChunkDirectory();
        if (!m_usedChunks)
        {
            m_checkFlags -= FL_DOUBLE_BOOKKEEPING;
        }
    }
    this->CheckConstraints();
    SetIdentifier(Identifier);
    m_AllocatorInfo = RTEMem_AllocatorInfo(&m_Name[0], this, RawAllocator.GetIdentifier());

#if defined(KERNEL)
    m_taskId = EvalCurrentTaskId();
    if (DoRegister)
    {
        RTEMem_AllocatorRegister::Instance().Register(m_AllocatorInfo);
    }
#endif    
}

/*-------------------------------------------------------------------*/

SAPDBMem_RawAllocator::SAPDBMem_RawAllocator
(
    const SAPDB_UTF8       *   Identifier,
    SAPDBMem_IRawAllocator &   RawAllocator,
    RTESync_Spinlock*          lockObject,
    SAPDB_ULong                FirstBlockSize,
    SAPDB_ULong                SupplementBlockSize,
    FreeRawExtendsEnum         FreeRawExtends,
    SAPDB_ULong                MaxSize,
    bool                       DoRegister
 )
 : m_this(0)
 , m_BlockAllocator(NULL)
 , m_RawAllocator(&RawAllocator)
 , m_tracer(NULL)
 , m_delayedFreeHandler(NULL)
 , m_leftFencePattern(NULL)
 , m_firstSize(FirstBlockSize) 
 , m_supplement(SupplementBlockSize) 
 , m_maxHeap(MaxSize) 
 , m_freeLists()
 , m_binBlocks(0)
 , m_checkFlags(GetCheckLevel())
 , m_maxChunkSize(0xFFFFFFFF)
 , m_heapInfo()
 , m_checkHeap(false)
 , m_freeRawExtends(FreeRawExtends)
 , m_opMessages(true)
 , m_writeAllocateFailedOpMsg(true)
 , m_writeProtected(false)
 , m_doDeregister(DoRegister)
 , m_monitorCallStackLevel(0)
 , m_failedAllocCount(0)
 , m_errorCount(0)
 , m_taskId(RTE_UnknownTaskId)
 , m_badAllocFunc(SAPDBMem_RawAllocator::InitBadAllocHandler())
 , m_lock(lockObject)
 , m_AllocatorInfo(Identifier)
 , m_usedChunks(NULL)
 , m_root(0)
 , m_next(REINTERPRET_CAST(CChunkPtr, &m_root))
 , m_prev(REINTERPRET_CAST(CChunkPtr, &m_root))
 , m_firstFreeNode(0)
 , m_freeNodeBuffer(0)
 , m_rawChunkTree(&m_rawChunkTreeComparator, &m_rawTreeAllocator)
 , m_BlockedForMessageOutput(0)
 , m_BlockingThreadId(0)
 , m_traceEnabled(false)
{
    if (m_checkFlags & FL_DOUBLE_BOOKKEEPING)
    {
        m_usedChunks = new SAPDBMem_UsedChunkDirectory();
        if (!m_usedChunks)
        {
            m_checkFlags -= FL_DOUBLE_BOOKKEEPING;
        }
    }
    this->CheckConstraints();
    SetIdentifier(Identifier);
    m_AllocatorInfo = RTEMem_AllocatorInfo(&m_Name[0], this, RawAllocator.GetIdentifier());

#if defined(KERNEL)
    if (0 == lockObject)
    {
        m_taskId = EvalCurrentTaskId();
    }
    if (DoRegister)
    {
        RTEMem_AllocatorRegister::Instance().Register(m_AllocatorInfo);
    }
#endif    
}

/*-------------------------------------------------------------------*/

SAPDBMem_RawAllocator::SAPDBMem_RawAllocator
(
 const SAPDB_UTF8         * Identifier,
 SAPDBMem_IBlockAllocator & BlockAllocator, 
 RTESync_Spinlock*          lockObject,
 SAPDB_ULong                FirstBlockSize,
 SAPDB_ULong                SupplementBlockSize,
 FreeRawExtendsEnum         FreeRawExtends,
 SAPDB_ULong                MaxSize
 )  
 : m_this(0)
 , m_BlockAllocator(&BlockAllocator)
 , m_RawAllocator(NULL)
 , m_tracer(NULL)
 , m_delayedFreeHandler(NULL)
 , m_leftFencePattern(NULL)
 , m_firstSize(FirstBlockSize) 
 , m_supplement(SupplementBlockSize) 
 , m_maxHeap(MaxSize) 
 , m_freeLists() 
 , m_binBlocks(0)
 , m_checkFlags(GetCheckLevel())
 , m_maxChunkSize(0xFFFFFFFF)
 , m_heapInfo()
 , m_checkHeap(false)
 , m_freeRawExtends(FreeRawExtends)
 , m_opMessages(true)
 , m_writeAllocateFailedOpMsg(true)
 , m_writeProtected(false)
 , m_doDeregister(true)
 , m_monitorCallStackLevel(0)
 , m_failedAllocCount(0)
 , m_errorCount(0)
 , m_taskId(RTE_UnknownTaskId)
 , m_badAllocFunc(SAPDBMem_RawAllocator::InitBadAllocHandler())
 , m_lock(lockObject)
 , m_AllocatorInfo(Identifier)
 , m_usedChunks(NULL)
 , m_root(0)
 , m_next(REINTERPRET_CAST(CChunkPtr, &m_root))
 , m_prev(REINTERPRET_CAST(CChunkPtr, &m_root))
 , m_firstFreeNode(0)
 , m_freeNodeBuffer(0)
 , m_rawChunkTree(&m_rawChunkTreeComparator, &m_rawTreeAllocator)
 , m_BlockedForMessageOutput(0)
 , m_BlockingThreadId(0)
 , m_traceEnabled(false)
{
  this->CheckConstraints();
  SetIdentifier(Identifier);
  m_AllocatorInfo = RTEMem_AllocatorInfo(&m_Name[0], this, BlockAllocator.GetIdentifier());
  if (m_checkFlags & FL_DOUBLE_BOOKKEEPING)
  {
      m_usedChunks = new SAPDBMem_UsedChunkDirectory();
      if (!m_usedChunks)
      {
          m_checkFlags -= FL_DOUBLE_BOOKKEEPING;
      }
  }
#if defined(KERNEL)
  if (0 == lockObject)
  {
      m_taskId = EvalCurrentTaskId();
  }
  RTEMem_AllocatorRegister::Instance().Register(m_AllocatorInfo);
#endif
}

/*-------------------------------------------------------------------*/

SAPDBMem_RawAllocator::SAPDBMem_RawAllocator
(
 const SAPDB_UTF8         * Identifier,
 SAPDBMem_IBlockAllocator & BlockAllocator, 
 SAPDB_ULong                FirstBlockSize,
 SAPDB_ULong                SupplementBlockSize,
 FreeRawExtendsEnum         FreeRawExtends,
 SAPDB_ULong                MaxSize
 )  
 : m_this(0)
 , m_BlockAllocator(&BlockAllocator)
 , m_RawAllocator(NULL)
 , m_tracer(NULL)
 , m_delayedFreeHandler(NULL)
 , m_leftFencePattern(NULL)
 , m_firstSize(FirstBlockSize) 
 , m_supplement(SupplementBlockSize) 
 , m_maxHeap(MaxSize) 
 , m_freeLists() 
 , m_binBlocks(0)
 , m_checkFlags(GetCheckLevel())
 , m_maxChunkSize(0xFFFFFFFF)
 , m_heapInfo()
 , m_checkHeap(false)
 , m_freeRawExtends(FreeRawExtends)
 , m_opMessages(true)
 , m_writeAllocateFailedOpMsg(true)
 , m_writeProtected(false)
 , m_doDeregister(true)
 , m_monitorCallStackLevel(0)
 , m_failedAllocCount(0)
 , m_errorCount(0)
 , m_taskId(RTE_UnknownTaskId)
 , m_badAllocFunc(SAPDBMem_RawAllocator::InitBadAllocHandler())
 , m_lock(0)
 , m_AllocatorInfo(Identifier)
 , m_usedChunks(NULL)
 , m_root(0)
 , m_next(REINTERPRET_CAST(CChunkPtr, &m_root))
 , m_prev(REINTERPRET_CAST(CChunkPtr, &m_root))
 , m_firstFreeNode(0)
 , m_freeNodeBuffer(0)
 , m_rawChunkTree(&m_rawChunkTreeComparator, &m_rawTreeAllocator)
 , m_BlockedForMessageOutput(0)
 , m_BlockingThreadId(0)
 , m_traceEnabled(false)
{
  this->CheckConstraints();
#if defined(KERNEL)
  m_taskId = EvalCurrentTaskId();
  SetIdentifier(Identifier);
  m_AllocatorInfo = RTEMem_AllocatorInfo( &m_Name[0], this, BlockAllocator.GetIdentifier());
  RTEMem_AllocatorRegister::Instance().Register(m_AllocatorInfo);
#endif
  if (m_checkFlags & FL_DOUBLE_BOOKKEEPING)
  {
      m_usedChunks = new SAPDBMem_UsedChunkDirectory();
      if (!m_usedChunks)
      {
          m_checkFlags -= FL_DOUBLE_BOOKKEEPING;
      }
  }
}

/*-------------------------------------------------------------------*/

INLINE SAPDBMem_RawAllocator::ChunkSize SAPDBMem_RawAllocator::AlignedSize(SAPDBMem_RawAllocator::ChunkSize req) {
  return (((req + (sizeof(SAPDBMem_RawAllocator::ChunkSize) + MALLOC_ALIGN_MASK)) < 
    (long)(MINSIZE + MALLOC_ALIGN_MASK)) ? MINSIZE : 
  (((req) + (sizeof(SAPDBMem_RawAllocator::ChunkSize) + MALLOC_ALIGN_MASK)) & ~(MALLOC_ALIGN_MASK)));
}

/*-------------------------------------------------------------------*/

INLINE void SAPDBMem_RawAllocator::FillFreeChunk(void* p, int offset, int size, tsp00_Uint4 pattern)
{
    if (m_checkFlags & FL_FILL_FREE_BLOCKS)
    {
        register int  loop = size / sizeof(unsigned int);
        register unsigned int* pInt = REINTERPRET_CAST(unsigned int*, p) + offset;
        while (loop-- > 0)
        {
            *pInt++ = pattern;
        }
    }
}

/*-------------------------------------------------------------------*/

void SAPDBMem_RawAllocator::EnableTrace()
{
    m_traceEnabled = true;
}

/*-------------------------------------------------------------------*/

void SAPDBMem_RawAllocator::FreeAll()
{
    this->Destructor();
    if (m_BlockAllocator)
    {
        new(this) SAPDBMem_RawAllocator(this->GetIdentifier(), 
            *m_BlockAllocator, 
            m_lock, m_firstSize, m_supplement, m_freeRawExtends, m_maxHeap);
    }
    else
    {
        new(this) SAPDBMem_RawAllocator(this->GetIdentifier(),
            *m_RawAllocator, m_firstSize, m_supplement, m_freeRawExtends, m_maxHeap);
    }
}

/*-------------------------------------------------------------------*/

#if defined(WIN32) || defined(OSF1)
void SAPDBMem_RawAllocator::ExceptionDeallocate(void* p)
{
    this->AnalyzeAllocator(p);
    if (m_tracer)
    {
        m_tracer->Exception("ExceptionDeallocate");
    }
}

/*-------------------------------------------------------------------*/

void SAPDBMem_RawAllocator::ExceptionAllocate()
{
}

#endif

/*-------------------------------------------------------------------*/

void SAPDBMem_RawAllocator::FreeRawChunk(SAPDBMem_RawAllocator::CChunkPtr endChunk, SAPDBMem_RawAllocator::CChunkPtr p) /* PTS 1109340 */
{
    SAPDBMem_RawChunkTreeAllocator::Node* pNode = p->GetRawChunkPtr();
    if (m_rawChunkTree.Find(*pNode->GetKey()))
    {
        m_rawChunkTree.Delete(*pNode->GetKey());
        --m_heapInfo.extends;
        m_heapInfo.allocated -= (this->RawChunkHeaderSize() + endChunk->prev_size + sizeof(SAPDBMem_ChunkSizeInfo));
        this->DeallocateRaw(pNode, pNode->GetKey()->m_blockCount);
        ++m_heapInfo.cntRawDealloc;
    }
}

/*-------------------------------------------------------------------*/

INLINE void SAPDBMem_RawAllocator::FrontLink(SAPDBMem_RawAllocator::CChunkPtr pChunk, SAPDBMem_RawAllocator::ChunkSize chunkSize)                                          \
{        
  SET_MIN_FREE_BLOCK_PATTERN(pChunk);
  if (chunkSize < MAX_SMALLBIN_SIZE) 
  { 
    SAPDBMem_RawAllocator::CChunkPtr prv;
    SAPDBMem_RawAllocator::CChunkPtr nxt;
    int ix = m_freeLists.GetSmallFreeListIndex(chunkSize);                                                  
    MarkBinBlock(ix);                                                       
    prv =        m_freeLists[ix];                                                         
    nxt          = prv->next; 
    pChunk->prev = prv;                                                               
    pChunk->next = nxt;
    prv->next    = pChunk;
    nxt->prev    = pChunk;
  }                                                                           
  else     
  {                                                           
    this->InsertFreeBigChunk(pChunk);                                                       
  }  
}

/*-------------------------------------------------------------------*/

SAPDBMem_RawAllocator* SAPDBMem_RawAllocator::GetAllocator(const void* p)
{

   CChunk* pChunk = SAPDBMem_RawAllocator::CChunk::Mem2Chunk(const_cast<void*>(p));
   return REINTERPRET_CAST(SAPDBMem_RawAllocator*, pChunk->next); 
}

/*-------------------------------------------------------------------*/

INLINE SAPDB_UInt4 SAPDBMem_RawAllocator::Idx2BinBlock(int ix) {
  return ((SAPDB_UInt4) 1 << (ix / BINBLOCKWIDTH));
}

/*-------------------------------------------------------------------*/

bool SAPDBMem_RawAllocator::IsWriteProtected() const
{
    return m_writeProtected;
}

/*-------------------------------------------------------------------*/

SAPDB_UInt4 SAPDBMem_RawAllocator::GetCheckLevel()
{
#if defined(KERNEL)
    if (RTEConf_ParameterAccess::InstanceExists())
    {
        RTEConf_Parameter::Integer value;
        SAPDBErr_MessageList       errorList;
        if (RTEConf_ParameterAccess::Instance()->GetInteger((unsigned char*) "HEAP_CHECK_LEVEL", value, errorList))
        {
            return (SAPDB_UInt4) value;
        }
        else
        {
            // there's any problem reading parameter, run without any checks
            return 0;
        }
    }
#endif
    return INITIAL_FLAGS;
}

/*-------------------------------------------------------------------*/

SAPDBMem_RawAllocator::BadAllocFunc* SAPDBMem_RawAllocator::InitBadAllocHandler()
{
#ifdef SAPDB_COMPILE_WITHOUT_EXCEPTION_HANDLING
  return BadAllocNoThrow;
#else
  return BadAllocThrowStdBadAlloc;
#endif
}

/*-------------------------------------------------------------------*/

INLINE bool SAPDBMem_RawAllocator::IsSmallRequest (SAPDBMem_RawAllocator::ChunkSize req) {
  return (req < MAX_SMALLBIN_SIZE - SMALLBIN_WIDTH);
}

/*-------------------------------------------------------------------*/

INLINE void SAPDBMem_RawAllocator::MarkBinBlock(int ix) {
  m_binBlocks |= Idx2BinBlock(ix);
}

/*-------------------------------------------------------------------*/

void* SAPDBMem_RawAllocator::BadAllocNoThrow(SAPDB_ULong ByteCount)
{
  return (void*) 0;
}

/*-------------------------------------------------------------------*/

void* SAPDBMem_RawAllocator::BadAllocThrowSAPDBMem_BadAlloc(SAPDB_ULong ByteCount)
{
#if defined(KERNEL)
  throw SAPDBMem_BadAllocException(__CONTEXT__, ByteCount);
#else
  THROW_BAD_ALLOC_GEO573;
#endif
  return 0;
}

/*-------------------------------------------------------------------*/

void*  SAPDBMem_RawAllocator::BadAllocThrowStdBadAlloc(SAPDB_ULong ByteCount)
{
    // unnecessary comparison to avoid compiler warnings    
    if (ByteCount > 0)
    {
        THROW_BAD_ALLOC_GEO573;
    }
    return 0;
}

/*-------------------------------------------------------------------*/

void SAPDBMem_RawAllocator::CheckConstraints() 
{
    m_this = this;
    if (m_firstSize > SAPDB_MAX_UINT4) {
        m_firstSize = SAPDB_MAX_UINT4;
    }
    if (m_supplement > SAPDB_MAX_UINT4) {
        m_supplement = SAPDB_MAX_UINT4;
    }
    if (m_maxHeap < m_firstSize) {
        m_maxHeap = m_firstSize;
    }
}

/*-------------------------------------------------------------------*/

INLINE bool SAPDBMem_RawAllocator::CheckFreeChunk (CChunkPtr pChunk)
{
    if (m_checkFlags & FL_FILL_FREE_BLOCKS)
    {
        tsp00_Uint4* pInt = REINTERPRET_CAST(tsp00_Uint4*, 
            (REINTERPRET_CAST(char*, pChunk) + sizeof(CChunk)));
        int kx = (pChunk->ChunkSize() - sizeof(CChunk) - sizeof(tsp00_Uint4)) / sizeof(tsp00_Uint4);
        while (kx-- > 0)
        {
            if ( *pInt++ != FREE_BLOCK_PATTERN)
            {
                SAPDBMem_ScopeBlockedForMessageOutput blockScope(m_BlockedForMessageOutput, m_BlockingThreadId, SAPDBMEM_NEVER_RESET_LOCK_COUNTER);
                this->Trace("wrong free block pattern detected %p", pChunk);
                this->Trace("this  : %s", (char*) this->GetIdentifier());
                this->DumpChunk (pChunk, pChunk->ChunkSize());
                this->DumpRawChunk (pChunk);
                this->Dump();
                this->Crash("wrong free block pattern");
            }
        }
    }
#if defined(RESERVE_PTR_SPACE)
    else
    {
       if (FREE_BLOCK_PATTERN != *REINTERPRET_CAST(tsp00_Uint4*, pChunk->ChunkMem()))
       {
           ++m_errorCount;
		  SAPDBMem_ScopeBlockedForMessageOutput blockScope(m_BlockedForMessageOutput, m_BlockingThreadId);
          {
           this->Trace("wrong free block pattern detected %p", pChunk);
           this->Trace("this  : %s", (char*) this->GetIdentifier());
           //this->DumpChunk (pChunk,  pChunk->ChunkSize());
           this->DumpRawChunk (pChunk);
           //this->Dump();
#if defined(SAPDB_SLOW) || defined(SAPDB_QUICK)
           this->Crash("wrong free block pattern detected");
#endif
           pChunk->Unlink(*this);
           pChunk->SetInUseAtOffset(pChunk->ChunkSize());
           if (m_checkFlags & FL_DOUBLE_BOOKKEEPING)
           {
               if (!m_usedChunks->Insert(pChunk->ChunkMem(), pChunk->ChunkSize()))
               {
                   // not enough memory for double bookkeeping, turn it off
                   Trace ("double bookkeeping turned off"); 
                   m_checkFlags -= FL_DOUBLE_BOOKKEEPING;
                   delete m_usedChunks;
                   m_usedChunks = 0;
               }
           }
          }
           return false;
       }
    }
#endif
    return true;
}

/*-------------------------------------------------------------------*/

INLINE void SAPDBMem_RawAllocator::CheckNoMansLand(CChunkPtr pChunk)
{
    if (!(m_checkFlags & FL_NO_MANS_LAND))
    {
        return;
    }
    bool ok;
    SAPDB_UInt4* pInt4 = REINTERPRET_CAST(SAPDB_UInt4*, pChunk->NextChunk());
    if (pChunk->IsCallStackAvailable())
    {
        --pInt4;
    }
    ok = ((*pInt4) == (SAPDB_UInt4) NO_MANS_LAND_MASK);
    if (!ok)
    {    
        SAPDBMem_ScopeBlockedForMessageOutput blockScope(m_BlockedForMessageOutput, m_BlockingThreadId, SAPDBMEM_NEVER_RESET_LOCK_COUNTER);
        //this->DumpChunk (pChunk, pChunk->ChunkSize());
        this->Trace("no mans land flag overwritten %p", pChunk);
        this->DumpRawChunk (pChunk);
        this->Crash("no_mans_land");
    }
//    if (pChunk->PrevInUse() && (0 == m_monitorCallStackLevel))
//    {
//        pInt4 = REINTERPRET_CAST(SAPDB_UInt4*, pChunk);
//        if ((*pInt4) != (SAPDB_UInt4) NO_MANS_LAND_MASK)
//        {
//            this->Trace("no mans land of previous block destroyed");
//            TODO find previous chunk
//            SAPDBMEM_CRASH("no_mans_land");
//        }
//    }
}

/*-------------------------------------------------------------------*/

INLINE RTE_TaskId SAPDBMem_RawAllocator::EvalCurrentTaskId()
{
#if defined(KERNEL)
    RTETask_ITask* pTask = RTETask_ITask::Self();
    if (pTask)
    {
        return pTask->ID();
    }
    else
#endif
    { 
        return RTE_UnknownTaskId;
    }
}

/*-------------------------------------------------------------------*/

bool SAPDBMem_RawAllocator::CheckPointer(void* p, bool abort)
{
    if (NULL == p)
    {
        return true;
    }
#if defined(BIT64)
#else
    SAPDB_UInt4 uInt = *REINTERPRET_CAST(SAPDB_UInt4*, &p);
    if (0 != (uInt & 0x7))
    {   // pointer not 8 byte aligned
        return false;
    }
#endif
    bool pointerInAnyRegion = (0 != m_rawChunkTree.Find(SAPDBMem_RawChunkHeader(
        REINTERPRET_CAST(unsigned char*, p),
        REINTERPRET_CAST(unsigned char*, p),
        0)));
    if (!pointerInAnyRegion)
    {
        if (abort)
        {
           SAPDBMem_ScopeBlockedForMessageOutput blockScope(m_BlockedForMessageOutput, m_BlockingThreadId);
           {
            this->Trace("pointer out of allocator : %p", p);
            this->TraceRawChunks();
            this->ThrowException();
           }
        }
        SAPDBERR_ASSERT_STATE (pointerInAnyRegion);
        return false;
    }
    return true;
}

/*-------------------------------------------------------------------*/

void SAPDBMem_RawAllocator::Crash(const char* msg)
{
    if (m_tracer) 
    {
        m_tracer->Exception(msg); 
    }
    else { 
#if defined(KERNEL)
        SAPDBMem_ScopeBlockedForMessageOutput blockScope(m_BlockedForMessageOutput, m_BlockingThreadId, SAPDBMEM_NEVER_RESET_LOCK_COUNTER);
        { Kernel_OpError (csp3_no_more_memory, csp3_n_memory) << msg; }    
        Msg_List messageList;
        RTE_Crash(messageList);
#else
        throw -1; // TODO
#endif
    }
}

/*-------------------------------------------------------------------*/

INLINE void SAPDBMem_RawAllocator::CheckUsage()
{
#if defined(KERNEL)
    if (m_checkFlags & FL_USAGE)
    {
        if ((m_taskId != RTE_UnknownTaskId) && (m_taskId != EvalCurrentTaskId()))
        {
            SAPDBMem_ScopeBlockedForMessageOutput blockScope(m_BlockedForMessageOutput, m_BlockingThreadId, SAPDBMEM_NEVER_RESET_LOCK_COUNTER);
            this->Trace("wrong task using %s, task %d calling, task %d expected", &m_Name[0], EvalCurrentTaskId(), m_taskId);
            this->Crash("wrong task");
        }
    }
#endif
}

/*-------------------------------------------------------------------*/

SAPDBMem_RawAllocator::~SAPDBMem_RawAllocator() 
{
#if defined(KERNEL)
    if (m_doDeregister)
    {
        RTEMem_AllocatorRegister::Instance().Deregister(m_AllocatorInfo);
    }
#endif
    this->Destructor();
}

/*-------------------------------------------------------------------*/

//int SAPDBMem_RawAllocator::GetId(void* p)
//{
//  SAPDBMem_RawAllocator::CChunkPtr pChunk = SAPDBMem_RawAllocator::CChunk::Mem2Chunk(p);
//  return pChunk->size >> 29;
//}

/*-------------------------------------------------------------------*/

const SAPDB_UTF8 * SAPDBMem_RawAllocator::GetIdentifier() const 
{
  return m_AllocatorInfo.GetIdentifier(); 
}

/*-------------------------------------------------------------------*/

void SAPDBMem_RawAllocator::GetInfo(SAPDBMem_RawAllocator::THeapInfo& heapInfo) const 
{
  heapInfo = m_heapInfo;
}

/*-------------------------------------------------------------------*/

SAPDB_UInt4 SAPDBMem_RawAllocator::GetFirstChunkParameter() const
{
  return m_firstSize;
}

/*-------------------------------------------------------------------*/

void SAPDBMem_RawAllocator::NewNodeBuffer()
{
    void* p;
    int   sizeInBytes;

    if (m_BlockAllocator)
    {
        sizeInBytes = m_BlockAllocator->GetBlockSize();
        p           = m_BlockAllocator->Allocate(1);
    }
    else
    {
        sizeInBytes = sizeof(TNodeBuffer) + 16 * sizeof(SAPDBMem_TreeNode);
        p           = m_RawAllocator->AllocateThrowsNothing (sizeInBytes);
    }
   
    if (NULL == p)
    {
        return;
    }
    m_freeNodeBuffer = new(p) SAPDBMem_RawAllocator::TNodeBuffer(1, m_freeNodeBuffer);
    m_firstFreeNode  = REINTERPRET_CAST(NodePtr, REINTERPRET_CAST(char*, p) + sizeof(TNodeBuffer));
    sizeInBytes      = sizeInBytes - sizeof(TNodeBuffer);
    NodePtr pNode    = m_firstFreeNode;
    int nodeCount    = sizeInBytes / sizeof(SAPDBMem_TreeNode);
    while (nodeCount-- > 1)
    {
        NodePtr  currNode = pNode;
        ++pNode;
        currNode->m_right = pNode;
    }
    pNode->m_right = 0;
}

/*-------------------------------------------------------------------*/

void SAPDBMem_RawAllocator::SetSupplementChunkParameter(SAPDB_UInt4 supplement)
{
  m_supplement = supplement;
}

/*-------------------------------------------------------------------*/

INLINE void* SAPDBMem_RawAllocator::AllocateImplementation (SAPDB_ULong ByteCount, bool CallBadAllocHandler, bool DoGrow) 
{
#ifdef SAPDBMem_USE_SYSTEM_ALLOC
  void *p = malloc(ByteCount);
  if (!p)
    return (*m_badAllocFunc)(ByteCount);  
  return p;
#else
  int           monitorCallStackLevel = 0;
  CChunkPtr     victim;          /* inspected/selected chunk */
  CChunkPtr     remainder;       /* remainder of a chunk split */
  ChunkSize     victim_size;     /* its size */
  int           idx;             /* index for free list traversal */
  CChunkPtr     bin;             /* associated bin */
  ChunkSize     remainder_size;  /* its size */
  SAPDB_ULong   block;           /* block traverser bit */
  int           startidx;        /* first bin of a traversed block */
  int           extends = 0;
  CChunkPtr     q;               /* misc temp */
  bool          heapLocked = (0 == m_lock);
  
  SAPDB_ULong request = ByteCount; 
  ChunkSize calcChunkSize = CalcChunkSize(request);

  if (request > calcChunkSize)
  {
      SAPDBMem_ScopeBlockedForMessageOutput blockScope(m_BlockedForMessageOutput, m_BlockingThreadId);
      this->Trace("Calculated chunk size smaller than requested size %p", this);
      return (*m_badAllocFunc)(ByteCount);
  }

#if defined(RESERVE_PTR_SPACE)
  ByteCount += 2 * sizeof(void*);
#endif
#if defined(OSF1)
  ByteCount += sizeof(SAPDB_UInt4);     /* avoid cache line problems */
#else
  if (m_checkFlags > 0)
  {
      if (this != m_this)
      {
          SAPDBMem_ScopeBlockedForMessageOutput blockScope(m_BlockedForMessageOutput, m_BlockingThreadId);
          this->Trace("this does not identify an allocator %p", this);
          return (*m_badAllocFunc)(ByteCount); 
      }
      if (m_checkFlags & FL_NO_MANS_LAND)
      {
          ByteCount += sizeof(SAPDB_UInt4); /* reserve space for no man's land */
      }
  }
#endif
  if (m_monitorCallStackLevel > 0)
  {
    monitorCallStackLevel = m_monitorCallStackLevel;
    ByteCount += sizeof(SAPDB_UInt4); /* reserve space for call back info */
  }
  ChunkSize nb = AlignedSize(ByteCount);

  if( nb != calcChunkSize)
  {
    this->Crash("wrong calculated chunk size");
  }

  ++m_heapInfo.cntAlloc;
  /* Check for exact match in a free list */
  do
  {
      if (IsSmallRequest(nb)) 
      {
          /* Faster version for small requests                           */
          /* No traversal or size check necessary for small free lists.  */
          idx = m_freeLists.GetSmallFreeListIndex(nb);
          q      = m_freeLists[idx];
          LOCK_HEAP(heapLocked);
          victim = q->Last();
          // check exact matching free list first 
          if (victim == q)
          {
              // Also scan the next one, since it would have a remainder < MINSIZE 
              q      = q->NextFreeList();
              victim = q->Last();
          }
          if (victim != q) 
          {
              // this free list satisfies the request
              if (!this->CheckFreeChunk(victim))
              {
                  continue;
              }
              victim_size   = victim->ChunkSize();
              this->IncrementUsed(victim_size);
              victim->Unlink(*this);
              victim->SetInUseAtOffset(victim_size);
              return AllocateResult(victim, monitorCallStackLevel);
          }
          idx += 2; /* Set for bin scan below. We've already scanned 2 free lists. */
try_again :
          if ( (block = Idx2BinBlock(idx)) <= m_binBlocks)  
          {
              
              /* Get to the first marked block */
              if ( (block & m_binBlocks) == 0) 
              {
                  /* force to an even block boundary */
                  idx = (idx & ~(BINBLOCKWIDTH - 1)) + BINBLOCKWIDTH;
                  block <<= 1;
                  while ((block & m_binBlocks) == 0)
                  {
                      idx += BINBLOCKWIDTH;
                      block <<= 1;
                  }
              }
              
              /* For each possibly nonempty block ... */
              for (;;)  
              {
                  startidx = idx;          /* (track incomplete blocks) */
                  q = bin = m_freeLists[idx];
                  
                  /* For each bin in this block ... */
                  do
                  {
                      victim = bin->Last();
                      if (victim != bin)
                      {
                          if (!this->CheckFreeChunk(victim))
                          {
                              // the corrupted block has already been removed from free chain
                              goto try_again;
                          }
                          victim_size = victim->ChunkSize();
                          remainder_size = victim_size - nb;
                          if (remainder_size >= MINSIZE) /* split */
                          {
                              remainder = victim->ChunkAtOffset(nb);
                              victim->SetHead(nb | PREV_INUSE);
                              victim->Unlink(*this);
                              remainder->SetHeadAndFoot(remainder_size);
                              FrontLink(remainder, remainder_size);
                              this->IncrementUsed(nb);
                              return AllocateResult(victim, monitorCallStackLevel); 
                          }
                          else {
                              victim->SetInUseAtOffset(victim_size);
                              victim->Unlink(*this);
                              this->IncrementUsed(victim_size);
                              return AllocateResult(victim, monitorCallStackLevel);
                          }
                      }
                      bin = bin->NextFreeList();
                  }
                  while ((++idx & (BINBLOCKWIDTH - 1)) != 0);
                  
                  /* Clear out the block bit. */
                  
                  do   /* Possibly backtrack to try to clear a partial block */
                  {
                      if ((startidx & (BINBLOCKWIDTH - 1)) == 0)
                      {
                          m_binBlocks &= ~block;
                          break;
                      }
                      --startidx;
                      q = q->PrevFreeList();
                  } while (q->First() == q);
                  
                  /* Get to the next possibly nonempty block */
                  
                  if ( (block <<= 1) <= m_binBlocks && (block != 0) ) 
                  {
                      while ((block & m_binBlocks) == 0)
                      {
                          idx += BINBLOCKWIDTH;
                          block <<= 1;
                      }
                  }
                  else
                      break;
              }
          }
      }
      else
      {
          // large request
          LOCK_HEAP(heapLocked);
      }
      NodePtr pCurr           = m_root;
      NodePtr pLastGreater    = 0;
      size_t  lastVisitedSize = 0;
      // binary search for best fitting free list
      while (true)
      {
          if (0 == pCurr)
          {
              break;
          }
          victim_size = pCurr->m_size;       
          // search best free list
          if (victim_size < nb)
          {
              lastVisitedSize = victim_size;
              pCurr           = pCurr->m_right;
          }
          else
          {
              if (victim_size > nb)
              {
                  pLastGreater = pCurr;
                  pCurr        = pCurr->m_left;
              }
              else
              {
                  // exact match, remove and return oldest chunk from list
                  victim = pCurr->next;
                  this->RemoveFreeBigChunk(victim);
                  victim->SetInUseAtOffset(nb);
                  this->IncrementUsed(nb);
                  return AllocateResult(victim, monitorCallStackLevel);
              }
          }
      }
      if (0 != pLastGreater)
      { 
          // remove and return oldest chunk from list
          victim         = pLastGreater->next;
          victim_size    = victim->ChunkSize();
          remainder_size = victim_size - nb;
          if (!this->CheckFreeChunk (victim))
          {
              ++m_failedAllocCount;
              UNLOCK_HEAP;
              return (*m_badAllocFunc)(ByteCount);  
          }
          if (victim->next == victim->prev)
          {
              // list becomes empty
              if (remainder_size > lastVisitedSize)
              {
                  // node keeps position in tree, don't remove
                  if (remainder_size >= MAX_SMALLBIN_SIZE) /* split */
                  {
                      pLastGreater->m_size = remainder_size;
                      remainder            = victim->ChunkAtOffset(nb);
                      remainder->SetHeadAndFoot(remainder_size);
                      SET_MIN_FREE_BLOCK_PATTERN(remainder);
                      pLastGreater->next  = remainder;
                      pLastGreater->prev  = remainder;
                      remainder->next     = REINTERPRET_CAST(CChunkPtr, pLastGreater);
                      remainder->prev     = REINTERPRET_CAST(CChunkPtr, pLastGreater);
                      victim->SetHead(nb | PREV_INUSE);
                      /* TODO try to use end of free chunk -> no changes of pointers needed
                      victim->SetHead(remainder_size | (victim->size & SIZE_BITS));
                      victim = victim->ChunkAtOffset(remainder_size);
                      victim->SetHead(nb);
                      */
                      victim->SetInUseAtOffset(nb);
                      this->IncrementUsed(nb);
                      return AllocateResult(victim, monitorCallStackLevel);
                  }
              }
              this->RemoveFreeBigChunk(victim);
          }
          else
          {
              // remove from free list, list does not become empty
              victim->next->prev = victim->prev;
              victim->prev->next = victim->next;
          }
      }
      else
      {
          // no chunk found yet, try to use emergency list
          victim = 0;
          for (CChunkPtr pVictim = m_prev; pVictim != REINTERPRET_CAST(CChunkPtr, &m_root); pVictim = pVictim->prev)
          {
              if (pVictim->ChunkSize() >= nb)
              {
                  // remove from list
                  pVictim->next->prev = pVictim->prev;
                  pVictim->prev->next = pVictim->next;
                  // use chunk
                  victim         = pVictim;
                  victim_size    = victim->ChunkSize();
                  remainder_size = victim_size - nb;
                  break;
              }
          }
          if (0 == victim)
          {
              goto extend;
          }
      }
      if (remainder_size >= MINSIZE) /* split */
      {
          remainder = victim->ChunkAtOffset(nb);
          victim->SetHead(nb | PREV_INUSE);
          remainder->SetHeadAndFoot(remainder_size);
          FrontLink(remainder, remainder_size);
          this->IncrementUsed(nb);
          return AllocateResult(victim, monitorCallStackLevel); 
      }
      else {
          victim->SetInUseAtOffset(victim_size);
          this->IncrementUsed(victim_size);
          return AllocateResult(victim, monitorCallStackLevel);
      }
extend :
      /* Try to extend */
      if ( DoGrow          &&
          ( 0 == extends ) && 
          ((m_supplement > 0) || (0 == m_heapInfo.extends)) 
          )
      {
          ++extends;
         SAPDBMem_ScopeBlockedForMessageOutput blockScope(m_BlockedForMessageOutput, m_BlockingThreadId);
         {
          if (MallocExtend(nb)) {
              continue;
          }
         }
      }
      ++m_failedAllocCount;
      UNLOCK_HEAP;
      return (CallBadAllocHandler) ? (*m_badAllocFunc)(ByteCount) : 0;
  }
  while(1);
#endif
}

/*-------------------------------------------------------------------*/

void* SAPDBMem_RawAllocator::Allocate (SAPDB_ULong ByteCount)
{
    return AllocateImplementation (ByteCount, CALL_BAD_ALLOC_HANDLER, DO_GROW);
}

/*-------------------------------------------------------------------*/

void* SAPDBMem_RawAllocator::Allocate(SAPDB_ULong ByteCount, const void* Hint)
{
    return AllocateImplementation (ByteCount, CALL_BAD_ALLOC_HANDLER, DO_GROW);
}

/*-------------------------------------------------------------------*/

void* SAPDBMem_RawAllocator::AllocateThrowsNothing(SAPDB_ULong ByteCount)
{
    return AllocateImplementation (ByteCount, !CALL_BAD_ALLOC_HANDLER, DO_GROW);
}

/*-------------------------------------------------------------------*/

void* SAPDBMem_RawAllocator::AllocateWithoutGrowth(SAPDB_ULong ByteCount)
{
    return AllocateImplementation (ByteCount, CALL_BAD_ALLOC_HANDLER, !DO_GROW);
}

/*-------------------------------------------------------------------*/

bool SAPDBMem_RawAllocator::AnalyzeAllocator(void* p)
{
   bool  error      = false;
   SAPDBMem_ScopeBlockedForMessageOutput blockScope(m_BlockedForMessageOutput, m_BlockingThreadId);
   {
    this->Trace("analysing %s", &m_Name[0]);
    const bool abort = true;

    if (p)
    {
        unsigned char* pChunk = REINTERPRET_CAST(unsigned char*, p);
        TRY {
            if (!this->CheckPointer(p, !abort))
            {
                error = true;
                this->Trace("analyzed pointer out of range %p", p);
            }
        }
        CATCH_ALL(this->Trace("crashed while checking pointer %p", p);)
    }
    SAPDB_UInt4 heapUsed = 0;
    // run over allocator
    TRY
    {
        unsigned int freeChunks = 0;
        unsigned int usedChunks = 0;
        CChunkPtr pPrevChunk    = NULL;
        SAPDB_UInt8 maxChunks   = m_heapInfo.allocated / MINSIZE;
        for (HeapIterator iter = Begin(); iter; ++iter) {
            if (0 == maxChunks)
            {
                error = true;
                this->Trace("loop assumed while traversing heap");
                break;
            }
            CChunkPtr p = CChunk::Mem2Chunk(CONST_CAST(void*, iter()));
            if (p == pPrevChunk)
            {
                error = true;
                this->Trace("loop detected while traversing heap");
                break;
            }
            pPrevChunk = p;
            if (!this->CheckPointer(p, !abort))
            {
                error = true;
                this->Trace("out of range pointer %p found while traversing heap", p);
                break;
            }
            if (p->InUse()) {
                if (m_checkFlags & FL_DOUBLE_BOOKKEEPING)
                {
                    if (0 == m_usedChunks->Find(p->ChunkMem()))
                    {
                        this->Trace("block marked as used but not in dir %p", p);
                        error = true;
                        break;
                    }
                }
                if (m_checkFlags & FL_NO_MANS_LAND)
                {
                    SAPDB_UInt4* pInt4 = REINTERPRET_CAST(SAPDB_UInt4*, p->NextChunk());
                    if ((*pInt4) != (SAPDB_UInt4) NO_MANS_LAND_MASK)
                    {
                        this->Trace("no mans land flag overwritten %p, size =", p, p->ChunkSize());
                        this->DumpRawChunk(p);
                        //this->DumpChunk(p, p->ChunkSize());
                    }
                    SAPDBERR_ASSERT_STATE((*pInt4) == (SAPDB_UInt4) NO_MANS_LAND_MASK);
                }
                ++usedChunks;
            }
            else {
                ++freeChunks;
                CheckFreeChunk(p);
                if (p->ChunkSize() >= MAX_SMALLBIN_SIZE)
                {
                    if (!this->TreeFindFreeChunk(p))
                    {
                        error = true;
                        this->Trace("free chunk not found in free list %p", p);
                        this->Trace("chunk size is %d bytes", (int) p->ChunkSize());
                        break;
                    }
                }
            }
            --maxChunks;
        }
        this->Trace("%analysed %u used chunks, %u free chunks", usedChunks, freeChunks);
    }
    CATCH_ALL(this->Trace("crashed while traversing allocator");)

    // analyze free lists of small chunks
    int idx  = m_freeLists.GetSmallFreeListIndex(AlignedSize(1));
    TRY
    {
        while (idx < 128)
        {
            CChunkPtr prv = m_freeLists[idx];
            CChunkPtr nxt = prv->next;
            SAPDB_UInt8 maxChunks = m_heapInfo.allocated / MINSIZE;
            while (nxt != prv) 
            {
                if (0 == maxChunks)
                {
                    error = true;
                    this->Trace("loop assumed in free list");
                    break;
                }
                if (!this->CheckPointer(nxt, !abort))
                {
                    error = true;
                    this->Trace("corrupted nxt pointer %p in free list %d", nxt, idx);
                    break;
                }
                nxt  = nxt->next;
                if (nxt == prv)
                {
                    break;
                }
                if (!this->CheckPointer(nxt, !abort))
                {
                    error = true;
                    this->Trace("corrupted nxt pointer %p in free list %d", nxt, idx);
                    break;
                }
                if (nxt->InUse())
                {
                    error = true;
                    this->Trace("Used chunk found in free list(%d) : %p", idx, nxt);
                    break;
                }
                --maxChunks;
            }
            ++idx;
        }
    }
    CATCH_ALL(this->Trace("crashed while checking small free list %d", idx);)
 
    if (error)
    {
        this->TraceRawChunks();
    }
    else
    {
        this->Trace("no problems detected in %s", &m_Name[0]);
    }

   }

    return error;
}

/*-------------------------------------------------------------------*/

INLINE void SAPDBMem_RawAllocator::Assert(bool condition)
{
#ifdef DEBUG_BABDL
  SAPDBERR_ASSERT_STATE(condition);
#else
  if (!condition) 
  {
#if defined(KERNEL)
   SAPDBMem_ScopeBlockedForMessageOutput blockScope(m_BlockedForMessageOutput, m_BlockingThreadId);
   {
    Kernel_OpError (csp3_no_more_memory, csp3_n_memory) << "Bad Dealloc Exception thrown" << (char*) this->GetIdentifier();
    eo670_CTraceStack();
    throw DbpError (e_invalid_deallocate, "bad deallocation");
   }
#else
    THROW_BAD_ALLOC_GEO573;
#endif
  }
#endif
}

/*-------------------------------------------------------------------*/

bool SAPDBMem_RawAllocator::MallocExtend(SAPDBMem_RawAllocator::ChunkSize required) {
  CChunkPtr  p;
  ChunkSize sz = (0 == m_heapInfo.extends) ? m_firstSize : m_supplement;
  if (required + this->RawChunkHeaderSize() + 2 * sizeof(ChunkSize) + sizeof(TRawChunkFoot) > sz) 
  {
    sz = required +  this->RawChunkHeaderSize() + sizeof(TRawChunkFoot);
  }
  SAPDB_ULong blockCount = 0;
  if (m_BlockAllocator)
  {
      blockCount = m_BlockAllocator->CalcBlockCount(sz);
      sz         = blockCount * m_BlockAllocator->GetBlockSize();
  }
  if ((sz <= m_maxHeap) && (m_heapInfo.allocated <= (m_maxHeap - sz)))
  {
      if (m_BlockAllocator)
      {
          p = reinterpret_cast<SAPDBMem_RawAllocator::CChunkPtr>(m_BlockAllocator->Allocate(blockCount));
      }
      else
      {
          p = reinterpret_cast<SAPDBMem_RawAllocator::CChunkPtr>(m_RawAllocator->Allocate(sz));
      }
  }
  else 
  {
    p = NULL;
  }
  if (p) {
    m_writeAllocateFailedOpMsg = true;
    this->FillFreeChunk(p, 0, sz, FREE_BLOCK_PATTERN);
    ++m_heapInfo.cntRawAlloc;
    ++m_heapInfo.extends;
    m_heapInfo.allocated += sz;
    unsigned char* pChar  = REINTERPRET_CAST(unsigned char*, p);
    m_rawTreeAllocator.Set(p);
    int rc = m_rawChunkTree.Insert(SAPDBMem_RawChunkHeader(pChar, pChar + sz - 1, blockCount));
    if (0 != rc)
    {
        SAPDBMem_ScopeBlockedForMessageOutput blockScope(m_BlockedForMessageOutput, m_BlockingThreadId, SAPDBMEM_NEVER_RESET_LOCK_COUNTER);
        this->Trace ("couldn't insert  : %p, %p", pChar, (pChar + sz));
        cgg250AvlTree<SAPDBMem_RawChunkHeader,
        SAPDBMem_RawChunkTreeComparator,
        SAPDBMem_RawChunkTreeAllocator>::Iterator rawIter = m_rawChunkTree.First();
        while (rawIter)
        {
          const SAPDBMem_RawChunkHeader* rch = rawIter();
          this->Trace ("Raw Chunk from %p to %p", rch->m_lwb, rch->m_upb);
          ++rawIter;
        }
        this->Crash("insert into raw chunk tree failed");
    }
    sz -= this->RawChunkHeaderSize();
    p = p->ChunkAtOffset(this->RawChunkHeaderSize());
    char* q = (char*) p + sz - sizeof(TRawChunkFoot);
    TRawChunkFoot* pRawFoot = new(q) TRawChunkFoot(p); /* PTS 1109340 */
    sz -= sizeof(TRawChunkFoot);
    p->SetHeadAndFoot(sz);
    FrontLink(p, sz);
    return true;
  }
#if defined(KERNEL)
  if (m_opMessages)
  {
    if( m_writeAllocateFailedOpMsg){
      this->Trace ("%s  : could not allocate memory", this->GetIdentifier());
      this->Trace ("required   : %d", required);
      this->Trace ("allocated  : %d", m_heapInfo.allocated);
      this->Trace ("supplement : %d", sz);
      this->Trace ("limit      : %d", m_maxHeap == SAPDBMEM_ALLOCATOR_UNLIMITED ? -1 : m_maxHeap);
      m_writeAllocateFailedOpMsg = false;
    }
      int idx  = m_freeLists.GetSmallFreeListIndex(AlignedSize(1));
      // trace information about free blocks into knldiag
      while (idx < 128)
      {
          int       cnt = 0;
          CChunkPtr prv = m_freeLists[idx];
          CChunkPtr nxt = prv->next;
          while (nxt != prv) 
          {
              ++cnt;
              nxt  = nxt->next;
          }
          if (cnt > 0)
          {
              this->Trace ("free blocks size %d : %d", (idx << 3), cnt);
          }
          ++idx;
      }
      this->TraceTree();
  }
#endif
  return false;
}

/*-------------------------------------------------------------------*/

bool SAPDBMem_RawAllocator::CheckConsistency()
{
  return !AnalyzeAllocator(NULL);
}

/*-------------------------------------------------------------------*/

void SAPDBMem_RawAllocator::GetChunkInfo(const void* p, bool& inUse, SAPDB_ULong& ChunkSize, SAPDB_ULong& CallStackId) const
{
  if (0 == p)
  {
    CallStackId = 0; 
    inUse       = false;
    ChunkSize   = 0;

  }
  else
  {
    CChunk* pChunk = CChunk::Mem2Chunk(CONST_CAST(void*,p));
    inUse = pChunk->InUse();
    if (inUse && pChunk->IsCallStackAvailable())
    {
      CallStackId = *REINTERPRET_CAST(SAPDB_UInt4*, CChunk::Mem2Chunk(CONST_CAST(void*,p))->NextChunk());
    }
    else
    {
      CallStackId = 0;
    }
    ChunkSize   = CChunk::Mem2Chunk(CONST_CAST(void*,p))->ChunkSize();
  }
}

/*-------------------------------------------------------------------*/

SAPDB_ULong SAPDBMem_RawAllocator::GetChunkSize(const void* p)
{
  if (0 == p)
  {
    return 0;
  }
  else
  {
#ifdef SAPDBMem_USE_SYSTEM_ALLOC
    // TODO: what to return?
    return 1;
#else
    return CChunk::Mem2Chunk(CONST_CAST(void*,p))->ChunkSize();
#endif
  }
}

/*-------------------------------------------------------------------*/

SAPDB_ULong SAPDBMem_RawAllocator::GetMaxAllocatableSize() const
{
    return m_maxChunkSize;
}

/*-------------------------------------------------------------------*/


void SAPDBMem_RawAllocator::CalcStatistics(SAPDB_ULong &BytesUsed,
                                           SAPDB_ULong &MaxBytesUsed,
                                           SAPDB_ULong &BytesControlled)
{ 
    BytesUsed       = m_heapInfo.used;
    MaxBytesUsed    = m_heapInfo.maxused;
    BytesControlled = m_heapInfo.allocated;
}

/*-------------------------------------------------------------------*/

void SAPDBMem_RawAllocator::CalcStatisticsConst(SAPDB_ULong &BytesUsed,
                                                SAPDB_ULong &MaxBytesUsed,
                                                SAPDB_ULong &BytesControlled) const
{
    BytesUsed       = m_heapInfo.used;
    MaxBytesUsed    = m_heapInfo.maxused;
    BytesControlled = m_heapInfo.allocated;
}

/*-------------------------------------------------------------------*/

SAPDB_ULong SAPDBMem_RawAllocator::ComputeAndSetMaxAllocatableSize()
{
    m_maxChunkSize = m_supplement - this->RawChunkHeaderSize() - sizeof(TRawChunkFoot) - sizeof(SAPDBMem_RawAllocator::ChunkSize) - 2 * sizeof(void*);
#if defined(OSF1)
    m_maxChunkSize -= sizeof(SAPDB_UInt4);     /* avoid cache line problems */
#else
    if (m_checkFlags & FL_NO_MANS_LAND)
    {
        m_maxChunkSize -= sizeof(SAPDB_UInt4); /* space for no man's land */
    }
#endif
    return m_maxChunkSize;
}

/*-------------------------------------------------------------------*/

SAPDBMem_RawAllocator::ChunkSize SAPDBMem_RawAllocator::CalcChunkSize(SAPDB_ULong ByteCount)
{
  return ((ByteCount < 8) ? MINSIZE :
    (((ByteCount) + CalcChunkHeaderSize(m_checkFlags, m_monitorCallStackLevel>1) + MALLOC_ALIGN_MASK) & ~(MALLOC_ALIGN_MASK)));
}

/*-------------------------------------------------------------------*/

SAPDB_ULong SAPDBMem_RawAllocator::CalcChunkHeaderSize(SAPDB_UInt4 checkFlags, bool monitorCallStackLevel)
{
  SAPDB_ULong headerSize = sizeof(SAPDBMem_RawAllocator::ChunkSize);
#if defined(RESERVE_PTR_SPACE)
  headerSize += 2 * sizeof(void*);
#endif
#if defined(OSF1)
  headerSize += sizeof(SAPDB_UInt4);     /* avoid cache line problems */
#else
  if (checkFlags > 0)
  {
      if (checkFlags & FL_NO_MANS_LAND)
      {
          headerSize += sizeof(SAPDB_UInt4); /* reserve space for no man's land */
      }
  }
#endif
  if (monitorCallStackLevel)
  {
    headerSize += sizeof(SAPDB_UInt4); /* reserve space for call back info */
  }
  return headerSize;
}

/*-------------------------------------------------------------------*/

SAPDB_ULong SAPDBMem_RawAllocator::CalcMaxRawChunkSize(SAPDB_ULong ByteCount)
{
  return (((ByteCount < 8) ? MINSIZE : (((ByteCount) + CalcChunkHeaderSize(/*maximum HeapCheckLevel*/FL_ALL, /*monitorCallStackLevel*/true) + MALLOC_ALIGN_MASK) & ~(MALLOC_ALIGN_MASK))) 
           +  SAPDBMem_RawAllocator::RawChunkHeaderSize() + 2 * sizeof(ChunkSize) + sizeof(TRawChunkFoot));
}

/*-------------------------------------------------------------------*/

void SAPDBMem_RawAllocator::DefineLeftFencePattern (void* Pattern)
{
    m_leftFencePattern = Pattern;
}

/*-------------------------------------------------------------------*/

void SAPDBMem_RawAllocator::DisableOpMessages()
{
    m_opMessages = false;
}

/*-------------------------------------------------------------------*/

void SAPDBMem_RawAllocator::GetBaseAllocatorCallStatistics(SAPDB_ULong &CountAlloc,
                                  SAPDB_ULong &CountDealloc) const
{
    CountAlloc   = m_heapInfo.cntRawAlloc;
    CountDealloc = m_heapInfo.cntRawDealloc;
}

/*-------------------------------------------------------------------*/

void SAPDBMem_RawAllocator::GetCallStatistics
(
 SAPDB_ULong &CountAlloc,
 SAPDB_ULong &CountDealloc
 ) const
{
    CountAlloc   = m_heapInfo.cntAlloc;
    CountDealloc = m_heapInfo.cntDealloc;
}

/*-------------------------------------------------------------------*/

void SAPDBMem_RawAllocator::Deallocate(void* mem)
{
#ifdef SAPDBMem_USE_SYSTEM_ALLOC
  free(mem);
#else
  CChunkPtr p;               /* chunk corresponding to mem */
  ChunkSize sz;              /* its size */
  CChunkPtr next;            /* next contiguous chunk */
  ChunkSize nextsz;          /* its size */
  ChunkSize prevsz;          /* size of previous contiguous chunk */

  SAPDBMEM_WRITELN (AllocatorTrace, ALLOCATOR_TRACELEVEL, "Deallocate " << ToStr(mem));

  if (NULL == mem)  /* free(0) has no effect */
      return;
  
  TRY 
  {
      ++m_heapInfo.cntDealloc;
      p = SAPDBMem_RawAllocator::CChunk::Mem2Chunk(mem);
      if (p->GetAllocator() != this)
      {
          bool abort = true;
          SAPDBMem_ScopeBlockedForMessageOutput blockScope(m_BlockedForMessageOutput, m_BlockingThreadId, SAPDBMEM_NEVER_RESET_LOCK_COUNTER);
          // releasing chunk in wrong allocator or header overwritten 
          if( this->CheckPointer(p,!abort)){
              this->Trace ("chunk allocator pointer in header overwritten %p", p);
              this->Trace ("this  : %s", this->GetIdentifier());
              this->DumpRawChunk(p);
              this->Crash("chunk header has been overwritten");
          }
          else{
              // chunk is not part of raw chunks, access allocator pointer in chunk header
              this->Trace ("releasing chunk in wrong allocator %p", p);
              this->Trace ("this  : %s", this->GetIdentifier());
              this->Trace ("chunk : %s", p->GetAllocator()->GetIdentifier());
              if( p->GetAllocator()->CheckPointer(p, !abort)){
                  // chunk is part of oter alloctor raw chunks, dump this raw chunk
                  p->GetAllocator()->DumpRawChunk(p);
              }
              else{
                  // chunk is not part of other allocator raw chunks, try to dump chunk directly
                  this->DumpChunk (p, p->ChunkSize());
              }
              this->Crash("wrong allocator");
          }            
          
      }
      SAPDBMem_SyncObject sync (m_lock);
      sz = p->ChunkSize();
      if (m_checkFlags > 0)
      {
          if (this != m_this)
          {
              SAPDBMem_ScopeBlockedForMessageOutput blockScope(m_BlockedForMessageOutput, m_BlockingThreadId);
              this->Trace("this does not identify an allocator %p", this);
              this->Assert(false); // bad dealloc exception
          }
          this->CheckUsage();
          this->CheckPointer(mem);
          this->CheckNoMansLand (p);
          this->FillFreeChunk (mem, 0, sz - sizeof(CChunk), FREE_BLOCK_PATTERN);
          if ((m_checkFlags & FL_DELAYED_FREE) && this->DelayedFreeSucceeded(p))
          {
              return;
          }
          if (m_checkFlags & FL_DOUBLE_BOOKKEEPING)
          {
              if (!m_usedChunks->Delete(mem))
              {
                  bool abort = true;
                  SAPDBMem_ScopeBlockedForMessageOutput blockScope(m_BlockedForMessageOutput, m_BlockingThreadId, SAPDBMEM_NEVER_RESET_LOCK_COUNTER);
                  this->Trace("releasing not in use pointer %p", mem);
                  this->Trace ("this  : %s", this->GetIdentifier());
                  if (p->InUse())
                  {
                      this->Trace("pointer marked as used in heap !");
                  }
                  if(this->CheckPointer(p, !abort)){
                      this->DumpRawChunk(p);
                  }
                  else{
                      this->Trace("pointer not part of raw chunks !");
                      this->DumpChunk(p, p->ChunkSize());
                  }
                  this->Crash("releasing not in use pointer");                  
              }
          }
      }
      if (!p->InUse())
      {
          SAPDBMem_ScopeBlockedForMessageOutput blockScope(m_BlockedForMessageOutput, m_BlockingThreadId);
#if defined(KERNEL)
          
          {
          Kernel_OpError (csp3_no_more_memory, csp3_n_memory) << "deallocating not in use pointer " << ToStr(mem);
          Kernel_OpError (csp3_no_more_memory, csp3_n_memory) << "this  : " << (char*) this->GetIdentifier();
          this->Dump();
          sync.Unlock();
          }
#else
          bool abort = true;
          this->Trace("deallocating not in use pointer %p", mem);
          this->Trace ("this  : %s", this->GetIdentifier());
          if(this->CheckPointer(p, !abort))
          {
              this->DumpRawChunk(p);
          }
          else
          {
              this->Trace("pointer not part of raw chunks !");
          }
#endif
          this->Assert(p->InUse());
      }

      this->DecrementUsed(sz);
      next    = p->NextChunk();
      nextsz  = next->ClearPrevInUseAndGetChunkSize();
      if (!(p->PrevInUse()))                    /* consolidate backward */
      {
          prevsz          = p->prev_size;
          CChunkPtr pPrev = p->ChunkAtNegativeOffset(prevsz);
          if (this->CheckFreeChunk(pPrev))
          {
              this->FillFreeChunk(p, -1, sizeof(CChunk) + sizeof(tsp00_Uint4), FREE_BLOCK_PATTERN);
              p   = pPrev; 
              sz += prevsz;
              p->Unlink(*this);
          }
      }
      
      if ((!(next->InUseBitAtOffset(nextsz))) && (this->CheckFreeChunk(next)))  /* consolidate forward */
      {
          sz += nextsz;
          next->Unlink(*this);
          this->FillFreeChunk (next, 0, sizeof(CChunk), FREE_BLOCK_PATTERN);
          next = next->NextChunk(nextsz);
      }
      if (NOT_FREE_RAW_EXTENDS != m_freeRawExtends) /* PTS 1109340 */
      {
          SAPDBMem_RawAllocator::CChunkPtr nextNext = next->NextChunk();
          
          if ((nextNext->IsRawChunkEndMark()) && (p == nextNext->FirstInRawChunkPtr())) 
          {
              if ((m_heapInfo.extends > 1) || (FREE_RAW_EXTENDS == m_freeRawExtends)) // PTS 1123833
              {
                  this->FreeRawChunk(nextNext, p);
                  return;
              }
          }
      }
      p->SetHeadAndFoot(sz);
      FrontLink(p, sz);
  }
#if defined(KERNEL)
  CATCH_DBP_ERROR
#endif
  CATCH_ALL(ExceptionDeallocate(mem))
#endif
}

/*-------------------------------------------------------------------*/

int SAPDBMem_RawAllocator::GetChunkProperties(const void* p)
{
    CChunk* pChunk = SAPDBMem_RawAllocator::CChunk::Mem2Chunk(CONST_CAST(void*, p));
    return (pChunk->size >> 29);      
}

/*-------------------------------------------------------------------*/

SAPDB_ULong SAPDBMem_RawAllocator::GetErrorCount() const
{
    return m_errorCount;
}

/*-------------------------------------------------------------------*/

void* SAPDBMem_RawAllocator::Reallocate (SAPDB_ULong ByteCount, void* pHint)
{
    if (0 == pHint)
    {
        return this->Allocate(ByteCount);
    }
    else
    {
        if (0 == ByteCount)
        {
            this->Deallocate(pHint);
            return 0;
        }
        
        if (m_checkFlags & FL_NO_MANS_LAND)
        {
            ByteCount += sizeof(SAPDB_UInt4); /* reserve space for no man's land */
        }
        
        CChunkPtr pHintChunk  = SAPDBMem_RawAllocator::CChunk::Mem2Chunk(pHint);
        ChunkSize contentSize = pHintChunk->ChunkSize() + sizeof(SAPDBMem_RawAllocator::ChunkSize)-
#if defined(RESERVE_PTR_SPACE)
            sizeof(CChunk);
#else
            sizeof(SAPDBMem_ChunkSizeInfo);
#endif
        if (contentSize < ByteCount)
        {
            ChunkSize appendSize = ByteCount - contentSize;
            CChunkPtr pNext = pHintChunk->NextChunk();
            {
                SAPDBMem_SyncObject sync (m_lock);
                if (false) // (!pNext->InUse()) 
                {
                    ChunkSize nextSize = pNext->ChunkSize();
                    if (appendSize <= nextSize)
                    {
                        pNext->Unlink(*this);
                        int remainder_size = nextSize - appendSize;
                        if (remainder_size >= MINSIZE) /* split */
                        {
                            CChunkPtr pRemainder = pNext->ChunkAtOffset(appendSize);
                            pRemainder->SetHeadAndFoot(remainder_size);
                            FrontLink(pRemainder, remainder_size);
                            this->IncrementUsed(appendSize);
                        }
                        return pHint;
                    }
                }
            }
            if (m_checkFlags & FL_NO_MANS_LAND)
            {
                ByteCount -= sizeof(SAPDB_UInt4); /* space for no man's land considered by Allocate method */
            }

            void* p = this->Allocate(ByteCount);
            if (p)
            {
                SAPDB_MemCopyNoCheck (p, pHint, contentSize);
            }
            this->Deallocate(pHint);
            return p;
        }
        else
        {
            return pHint;
        }
    }
}

/*-------------------------------------------------------------------*/

void SAPDBMem_RawAllocator::SetAllocator(void* p, SAPDBMem_IRawAllocator* pAllocator)
{
   CChunk* pChunk = SAPDBMem_RawAllocator::CChunk::Mem2Chunk(p);
   pChunk->next = REINTERPRET_CAST(CChunkPtr, pAllocator);
   pChunk->prev = REINTERPRET_CAST(CChunkPtr, REINTERPRET_CAST(SAPDBMem_RawAllocator*, pAllocator)->m_leftFencePattern);
}

/*-------------------------------------------------------------------*/

void SAPDBMem_RawAllocator::SetBadAllocHandler(BadAllocHandlers handler)
{
  switch (handler)
  {
  case NO_THROW_HANDLER :
    m_badAllocFunc = BadAllocNoThrow;
    break;
  case THROW_SAPDBMEM_EXECEPTION :
    m_badAllocFunc = BadAllocThrowSAPDBMem_BadAlloc;
    break;
  case THROW_STD_BAD_ALLOC :
    m_badAllocFunc = BadAllocThrowStdBadAlloc;
  }
}

/*-------------------------------------------------------------------*/

void SAPDBMem_RawAllocator::SetCallStackMonitoring (int level)
{
  if (level > MAX_CALL_STACK_DEPTH)
  {
    level = MAX_CALL_STACK_DEPTH;
  }
  m_monitorCallStackLevel = level;
}

/*-------------------------------------------------------------------*/

void SAPDBMem_RawAllocator::SetChunkProperties(const void* p, int properties)
{
    CChunk* pChunk = SAPDBMem_RawAllocator::CChunk::Mem2Chunk(CONST_CAST(void*, p));
    pChunk->size = (pChunk->size & ~0xe0000000) | (properties << 29);
}

/*-------------------------------------------------------------------*/

void SAPDBMem_RawAllocator::SetFullChecking ()
{
    if (0 == m_heapInfo.extends)
    {
        m_checkFlags = (FL_NO_MANS_LAND + FL_FILL_FREE_BLOCKS + FL_DOUBLE_BOOKKEEPING + FL_CHECK_TREE);
        if (!m_usedChunks)
        {
            m_usedChunks = new SAPDBMem_UsedChunkDirectory();
        }
    }
}

/*-------------------------------------------------------------------*/

void SAPDBMem_RawAllocator::SetCheckLevel()
{
#if defined(KERNEL)
    RTEConf_Parameter::Integer value;
    SAPDBErr_MessageList       errorList;
    if (RTEConf_ParameterAccess::Instance()->GetInteger((unsigned char*) "HEAP_CHECK_LEVEL", value, errorList))
    {
        m_checkFlags &= value;
    }
    else
    {
        // there's any problem reading parameter, run without any checks
        m_checkFlags = 0;
    }
    if (!(m_checkFlags & FL_DOUBLE_BOOKKEEPING))
    {
        delete m_usedChunks;
        m_usedChunks = 0;
    }
#endif
}

/*-------------------------------------------------------------------*/

void SAPDBMem_RawAllocator::SetCheckLevel(SAPDB_UInt4 CheckLevel)
{
   m_checkFlags = CheckLevel;
   if ((m_checkFlags & FL_DOUBLE_BOOKKEEPING) && (0 == m_heapInfo.extends))
   {
       if (!m_usedChunks)
       {
           m_usedChunks = new SAPDBMem_UsedChunkDirectory();
           if (!m_usedChunks)
           {
                m_checkFlags -= FL_DOUBLE_BOOKKEEPING;
           }
       }
   }
   if (!(m_checkFlags & FL_DOUBLE_BOOKKEEPING))
   {
       delete m_usedChunks;
       m_usedChunks = 0;
   }
}

/*-------------------------------------------------------------------*/

void SAPDBMem_RawAllocator::SetIdentifier(const SAPDB_UTF8 * name)
{
#if defined(KERNEL)

    RTESync_LockedScope lock(RTEMem_AllocatorRegister::Instance().Spinlock());

#endif
    strncpy((char *)&m_Name[0], (const char *)name, sizeof(m_Name)-1);
    m_Name[sizeof(m_Name)-1] = 0;
}

/*-------------------------------------------------------------------*/


/*
  These routines make a number of assertions about the states
  of data structures that should be true at all times. If any
  are not true, it's very likely that a user program has somehow
  trashed memory. (It's also possible that there is a coding error
  in malloc. In which case, please report it!)
*/

#ifdef DEBUG_BABDL

void SAPDBMem_RawAllocator::CheckChunk(SAPDBMem_RawAllocator::CChunkPtr p) 
{ 
  SAPDBMem_RawAllocator::ChunkSize sz = p->ChunkSize() & ~PREV_INUSE;
  // Check for legal pointer address 
  CheckPointer(p);
}

/*-------------------------------------------------------------------*/

void SAPDBMem_RawAllocator::CheckFreeChunkDbg(SAPDBMem_RawAllocator::CChunkPtr p)
{ 
  SAPDBMem_RawAllocator::ChunkSize sz = p->ChunkSize() & ~PREV_INUSE;
  SAPDBMem_RawAllocator::CChunkPtr next   = p->ChunkAtOffset(sz);

  CheckChunk(p);

  /* Check whether it claims to be free ... */
  SAPDBERR_ASSERT_STATE(!p->InUse());

  /* Unless a special marker, must have OK fields */
  if ((long)sz >= (long)MINSIZE)
  {
    SAPDBERR_ASSERT_STATE((sz & MALLOC_ALIGN_MASK) == 0);
//    SAPDBERR_ASSERT_STATE(aligned_OK(chunk2mem(p)));
    /* ... matching footer field */
    SAPDBERR_ASSERT_STATE(next->prev_size == sz);
    /* ... and is fully consolidated */
    SAPDBERR_ASSERT_STATE(p->PrevInUse());
    SAPDBERR_ASSERT_STATE (next->InUse());
    
  }
  else /* markers are always of size SIZE_SZ */
    SAPDBERR_ASSERT_STATE(sz == sizeof(SAPDBMem_RawAllocator::ChunkSize)); 
}

/*-------------------------------------------------------------------*/

void SAPDBMem_RawAllocator::CheckInUseChunk(SAPDBMem_RawAllocator::CChunkPtr p) 
{ 
  SAPDBMem_RawAllocator::CChunkPtr next = p->NextChunk();
  CheckChunk(p);

  /* Check whether it claims to be in use ... */
  SAPDBERR_ASSERT_STATE(p->InUse());

  /* ... and is surrounded by OK chunks.
    Since more things can be checked with free chunks than inuse ones,
    if an inuse chunk borders them and debug is on, it's worth doing them.
  */
  if (!p->PrevInUse()) 
  {
    SAPDBMem_RawAllocator::CChunkPtr prv = p->PrevChunk();
    SAPDBERR_ASSERT_STATE(prv->NextChunk() == p);
    CheckFreeChunk(prv);
  }
  if (!next->InUse()) {
    CheckFreeChunk(next);
  }
}

/*-------------------------------------------------------------------*/

void SAPDBMem_RawAllocator::CheckMallocedChunk(SAPDBMem_RawAllocator::CChunkPtr p, SAPDBMem_RawAllocator::ChunkSize s) 
{
  SAPDBMem_RawAllocator::ChunkSize sz = p->size & ~PREV_INUSE;
  long room = sz - s;

  CheckInUseChunk(p);

  /* Legal size ... */
  SAPDBERR_ASSERT_STATE((long)sz >= (long)MINSIZE);
  SAPDBERR_ASSERT_STATE((sz & MALLOC_ALIGN_MASK) == 0);
  SAPDBERR_ASSERT_STATE(room >= 0);
  SAPDBERR_ASSERT_STATE(room < (long)MINSIZE);

  /* ... and alignment */
  //SAPDBERR_ASSERT_STATE(aligned_OK(p->Chunk2Mem()));


  /* ... and was allocated at front of an available chunk */
  SAPDBERR_ASSERT_STATE(p->PrevInUse());

}

#endif
/*-------------------------------------------------------------------*/

bool SAPDBMem_RawAllocator::Protect(SAPDB_Int mode)
{
    SAPDB_ULong error = RTE_SYSTEMRC_NO_ERROR;
    if (m_BlockAllocator)
    {
        for (cgg250AvlTree<SAPDBMem_RawChunkHeader,SAPDBMem_RawChunkTreeComparator, SAPDBMem_RawChunkTreeAllocator>::Iterator iter =
            m_rawChunkTree.First(); iter; ++iter)
        {
            SAPDBMem_RawChunkHeader* p = CONST_CAST(SAPDBMem_RawChunkHeader*, iter());
            error = RTE_IInterface::Instance().MemProtect ( p, p->m_blockCount * m_BlockAllocator->GetBlockSize(), mode);
            if (RTE_SYSTEMRC_NO_ERROR != error)
            {
                break;
            }
        }
    }
    return (RTE_SYSTEMRC_NO_ERROR == error);
}

/*-------------------------------------------------------------------*/

bool SAPDBMem_RawAllocator::Unprotect()
{
    if (this->Protect (MEMORYPROTECTION_READWRITE))
    {
        m_writeProtected = false;
    }
    return (!m_writeProtected); 
}

/*-------------------------------------------------------------------*/

bool SAPDBMem_RawAllocator::WriteProtect()
{
    m_writeProtected = this->Protect (MEMORYPROTECTION_READONLY);
    return m_writeProtected;
}

/*-------------------------------------------------------------------*/

void* SAPDBMem_RawChunkHeader::FirstChunkInRawChunk(void* p)
{
    return REINTERPRET_CAST(void*,
        (REINTERPRET_CAST(unsigned char*, p) + SAPDBMem_RawAllocator::RawChunkHeaderSize())); 
}

/*-------------------------------------------------------------------*/
/* topic : heap iterator                                             */
/*-------------------------------------------------------------------*/



/*-------------------------------------------------------------------*/

SAPDBMem_RawAllocator::HeapIterator SAPDBMem_RawAllocator::Begin()
{
  HeapIterator iter(this);
  if (this->m_lock)
  {
    this->m_lock->Lock();
  }
  m_iterState.m_pHeap     = this;
  m_iterState.m_refCnt    = 1;
  m_iterState.m_rawIter   = m_rawChunkTree.First();
  if (m_iterState.m_rawIter)
  { 
      m_iterState.m_currChunk = SAPDBMem_RawChunkHeader::FirstChunkInRawChunk(CONST_CAST(void*,
          REINTERPRET_CAST(const void*, m_iterState.m_rawIter())));
      this->CheckPointer(m_iterState.m_currChunk);
  }
  else
  {
      m_iterState.m_currChunk = 0;
  }
  return iter;
}

/*-------------------------------------------------------------------*/

void SAPDBMem_RawAllocator::DeallocateRaw(void* p, SAPDB_ULong BlockCount)
{
    if (m_BlockAllocator)
    {
        m_BlockAllocator->Deallocate(p, BlockCount);
    }
    else
    {
        m_RawAllocator->Deallocate(p);
    }
}

/*-------------------------------------------------------------------*/

bool SAPDBMem_RawAllocator::DelayedFreeSucceeded(CChunkPtr p)
{
    if (!m_delayedFreeHandler)
    {
        m_delayedFreeHandler = new SAPDBMem_DelayedFreeHandler(*this);
        if (!m_delayedFreeHandler)
        {
            return false;
        }
    }
    SET_MIN_FREE_BLOCK_PATTERN(p);
    // avoid delayed free for deallocates from delayed free list
    m_checkFlags &= ~((SAPDB_UInt4)FL_DELAYED_FREE);
    m_delayedFreeHandler->Insert(p);
    m_checkFlags |= FL_DELAYED_FREE;
    return true;
}

/*-------------------------------------------------------------------*/

void SAPDBMem_RawAllocator::Destructor() 
{
    // append raw chunks to node buffer list, neccessary to be able
    // to delete raw chunk tree
    for (cgg250AvlTree<SAPDBMem_RawChunkHeader,SAPDBMem_RawChunkTreeComparator, SAPDBMem_RawChunkTreeAllocator>::Iterator iter =
        m_rawChunkTree.First(); iter; ++iter)
    {
        SAPDBMem_RawChunkHeader* p = CONST_CAST(SAPDBMem_RawChunkHeader*, iter());
        m_freeNodeBuffer = new(p) SAPDBMem_RawAllocator::TNodeBuffer(p->m_blockCount, m_freeNodeBuffer);
    }
    // free raw chunk tree now, chunks are saved in node buffer list
    // caution : deleting the tree has no effect to the raw chunks
    // because deallocate does nothing !
    m_rawChunkTree.DeleteAll();
    // free all raw chunks now
    TNodeBuffer* toFree;
    while (NULL != m_freeNodeBuffer) 
    {
        toFree           = m_freeNodeBuffer;
        m_freeNodeBuffer = m_freeNodeBuffer->m_next;
        this->DeallocateRaw(toFree, toFree->m_blockCount);
    }
    m_freeNodeBuffer = 0;
    if (m_usedChunks)
    {
        delete m_usedChunks;
    }
    if (m_delayedFreeHandler)
    {
        delete m_delayedFreeHandler;
    }
    this->SetIdentifier ((SAPDB_UTF8 *) "DESTRUCTED");
    m_this = NULL;
}

/*-------------------------------------------------------------------*/

void  SAPDBMem_RawAllocator::Dump()
{
    cgg250AvlTree<SAPDBMem_RawChunkHeader,
        SAPDBMem_RawChunkTreeComparator,
        SAPDBMem_RawChunkTreeAllocator>::Iterator rawIter = m_rawChunkTree.First();
    while (rawIter)
    {
       SAPDBMem_ScopeBlockedForMessageOutput blockScope(m_BlockedForMessageOutput, m_BlockingThreadId);
       {
        const SAPDBMem_RawChunkHeader* rch = rawIter();
        this->Trace ("Raw Chunk from %p to %p, size %d", rch->m_lwb, rch->m_upb, rch->m_upb - rch->m_lwb);
        this->DumpChunk(rch->m_lwb, rch->m_upb - rch->m_lwb);
       }
        ++rawIter;
    }
}

/*-------------------------------------------------------------------*/

void  SAPDBMem_RawAllocator::DumpRawChunk(void *p)
{
    const SAPDBMem_RawChunkHeader* rch = m_rawChunkTree.Find(SAPDBMem_RawChunkHeader(
        REINTERPRET_CAST(unsigned char*, p),
        REINTERPRET_CAST(unsigned char*, p),
        0));
    if( rch){
        //this->Trace ("Chunk : %p", p);
        this->DumpChunk(rch->m_lwb, rch->m_upb - rch->m_lwb);
    }
}

/*-------------------------------------------------------------------*/

void SAPDBMem_RawAllocator::DumpChunk(void* p, int size)
{
    if (m_tracer)
    {
        m_tracer->Dump (p, size);
        return;
    }
#if defined(KERNEL)
    tsp00_VFilename fn;
    SAPDB_MemFillNoCheck (&fn[0], ' ', sizeof(fn));
    SAPDB_MemCopyNoCheck (&fn[0], "Heap                    .dmp", 28);
    tsp00_Timestamp ts;
    vtimestamp (ts);
    SAPDB_MemCopyNoCheck (&fn[4], &ts[0], sizeof(ts));
    tsp00_Int4    fileNo;
    tsp00_VfReturn e;
    tsp00_ErrText   errText;
    vfopen (fn, &fileNo, e, errText);
    fn[28] = 0;
    Kernel_OpError (csp3_no_more_memory, csp3_n_memory) << "dumping chunk "
        << ToStr(p) << ", size is " << size  << " into " << (char*) &fn[0];
    if (vf_ok == e)
    {
        unsigned char* iter = REINTERPRET_CAST(unsigned char*, p);
        tsp00_Buf Buf;
        while ((size > 0) && (vf_ok == e))
        {
            int cpySize = (size > sizeof(Buf)) ? sizeof(Buf) : size;
            SAPDB_MemFillNoCheck (&Buf[0], 0, sizeof(Buf));
            SAPDB_MemCopyNoCheck (&Buf[0], iter, cpySize);
            vfwrite (fileNo, Buf, e, errText);
            size -= sizeof(Buf);
            iter += sizeof(Buf);
        }
        if (vf_ok != e)
        {
            Kernel_OpError (csp3_no_more_memory, csp3_n_memory) << "couldn't write to dump file";
        }
        vfclose(fileNo, e, errText);
    }
    else
    {
        Kernel_OpError (csp3_no_more_memory, csp3_n_memory) << "could not open dump file " << (char*) &fn[0];
    }
#endif
}

/*-------------------------------------------------------------------*/

void SAPDBMem_RawAllocator::SetLimit(SAPDB_ULong NewLimit)
{
  if (NewLimit > m_heapInfo.allocated) 
  {
    m_maxHeap = NewLimit;
  }
}

/*-------------------------------------------------------------------*/

SAPDBMem_RawAllocator::HeapIterator::HeapIterator(SAPDBMem_RawAllocator* pHeap)
: m_pHeap(pHeap) 
{
}

/*-------------------------------------------------------------------*/

SAPDBMem_RawAllocator::HeapIterator::HeapIterator(const SAPDBMem_RawAllocator::HeapIterator& source)
: m_pHeap(source.m_pHeap) 
{
    ++m_pHeap->m_iterState.m_refCnt;
} 

/*-------------------------------------------------------------------*/

SAPDBMem_RawAllocator::HeapIterator::~HeapIterator()
{
    --m_pHeap->m_iterState.m_refCnt;
    if (0 == m_pHeap->m_iterState.m_refCnt)
    {
        if (m_pHeap->m_lock)
        {
            m_pHeap->m_lock->Unlock();
        }
    }
}

/*-------------------------------------------------------------------*/

SAPDBMem_RawAllocator::HeapIterator::operator bool() const {
  return (m_pHeap->m_iterState.m_currChunk != NULL);
}

/*-------------------------------------------------------------------*/

void SAPDBMem_RawAllocator::HeapIterator::operator++() {

  m_pHeap->m_iterState.m_currChunk = reinterpret_cast<SAPDBMem_RawAllocator::CChunkPtr>(m_pHeap->m_iterState.m_currChunk)->NextChunk();
  if (reinterpret_cast<SAPDBMem_RawAllocator::CChunkPtr>(m_pHeap->m_iterState.m_currChunk)->ChunkSize() == sizeof(SAPDBMem_ChunkSizeInfo)) 
  {
      // end of raw chunk reached, goto next raw chunk
      ++m_pHeap->m_iterState.m_rawIter;
      if (m_pHeap->m_iterState.m_rawIter)
      {
          m_pHeap->m_iterState.m_currChunk = SAPDBMem_RawChunkHeader::FirstChunkInRawChunk(CONST_CAST(void*,
              REINTERPRET_CAST(const void*, m_pHeap->m_iterState.m_rawIter())));
          
      }
      else
      {
          m_pHeap->m_iterState.m_currChunk = NULL;
      }
  }
  m_pHeap->CheckPointer(m_pHeap->m_iterState.m_currChunk);
}

/*-------------------------------------------------------------------*/

const void* SAPDBMem_RawAllocator::HeapIterator::operator()() const 
{

    return REINTERPRET_CAST(SAPDBMem_RawAllocator::CChunk*, m_pHeap->m_iterState.m_currChunk)->ChunkMem();
}

/*-------------------------------------------------------------------*/

void* SAPDBMem_RawChunkTreeAllocator::allocate(size_t sz)
{
    return m_rawChunk;
}

/*-------------------------------------------------------------------*/

void  SAPDBMem_RawChunkTreeAllocator::deallocate(void* p)
{
}

/*-------------------------------------------------------------------*/

void  SAPDBMem_RawChunkTreeAllocator::Set(void* p)
{
    m_rawChunk = p;
}

/*-------------------------------------------------------------------*/
/* pascal interface for diagnose tool                                */
/*-------------------------------------------------------------------*/

#if !defined(KERNEL)

static SAPDBMem_RawChunkHeader*         m_diagRawChunk;
static SAPDBMem_RawAllocator::CChunkPtr m_prevChunk;

externPascal tgg00_VoidPtr x12FirstChunk (
    tgg00_VoidPtr                        pRawChunk)
{
    m_diagRawChunk = reinterpret_cast<SAPDBMem_RawChunkHeader*> (pRawChunk);
    m_prevChunk    = 0; 
    return SAPDBMem_RawChunkHeader::FirstChunkInRawChunk(pRawChunk);
}

/*-------------------------------------------------------------------*/

externPascal tgg00_VoidPtr x12NextChunk (
    tgg00_VoidPtr                        pCurrChunk)
{
    if (reinterpret_cast<SAPDBMem_RawAllocator::CChunkPtr>(pCurrChunk)->ChunkSize() == sizeof(SAPDBMem_ChunkSizeInfo))
    {
        return 0;
    }
    m_prevChunk = reinterpret_cast<SAPDBMem_RawAllocator::CChunkPtr>(pCurrChunk);
    return m_prevChunk->NextChunk();
}

/*-------------------------------------------------------------------*/

externPascal void x12ChunkInfo (
    tgg00_VoidPtr                        pCurrChunk,
    tsp00_Int4            VAR_VALUE_REF  offset,
    tsp00_Int4            VAR_VALUE_REF  chunkSize,
    pasbool               VAR_VALUE_REF  inUse,
    pasbool               VAR_VALUE_REF  ok,
    tgg00_VoidPtr         VAR_VALUE_REF  prev,
    tgg00_VoidPtr         VAR_VALUE_REF  next,
    tsp00_C40             VAR_ARRAY_REF  errorText)
{
    ok                = true;
    SAPDB_Byte* pByte = reinterpret_cast<SAPDB_Byte*>(pCurrChunk);
    if ((pByte < (SAPDB_Byte*) m_diagRawChunk) || (pByte > (SAPDB_Byte*) m_diagRawChunk + (m_diagRawChunk->m_upb - m_diagRawChunk->m_lwb)))
    {
        strcpy ((char*) &errorText[0], "chunk out of range");
        ok = false;
    }
    if (ok)
    {
        SAPDBMem_RawAllocator::CChunkPtr p = reinterpret_cast<SAPDBMem_RawAllocator::CChunkPtr>(pCurrChunk);
        offset    = (char*) p - (char*) m_diagRawChunk;
        chunkSize = (tsp00_Int4) p->ChunkSize();
        inUse     = p->InUse();
        prev      = p->prev;
        next      = p->next;
        if (!inUse)
        {
            if (FREE_BLOCK_PATTERN != *REINTERPRET_CAST(tsp00_Uint4*, p->ChunkMem()))
            {
                ok = false;
                strcpy ((char*) &errorText[0], "invalid free block pattern");
            }
            if (ok && m_prevChunk)
            {
                if (!m_prevChunk->InUse())
                {
                    ok = false;
                    strcpy ((char*) &errorText[0], "not merged free chunks");
                }
            }
        }
    }
    if (!ok)
    {
        int len = (int) strlen((char*) &errorText[0]);
        SAPDB_MemFillNoCheck (&errorText[len], ' ', sizeof(errorText) - len); 
    }
}
#endif
