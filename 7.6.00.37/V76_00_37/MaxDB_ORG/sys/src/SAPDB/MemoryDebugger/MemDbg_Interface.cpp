/*!
 * \file    MemDbg_Interface.cpp
 * \author  IvanS
 * \brief   Memory debugger interface.
 */
/*
    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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

*/

#include "MemoryDebugger/MemDbg_Interface.hpp"
#include "MemoryDebugger/MemDbg_Config.hpp"
#include "MemoryDebugger/MemDbg_Callback.hpp"
#include "MemoryDebugger/MemDbg_Header.hpp"


#if defined(WIN32) && !defined(_WIN64)

#include "MemoryDebugger/MemDbg_Pure.h"
#include <windows.h>

/// Check for purify integration on Win32
bool MemDbg_Interface::purifyRunning = PurifyIsRunning() != 0;

#else   // WIN32

// no purify on Unix
bool MemDbg_Interface::purifyRunning = false;

#endif  // !WIN32


#include <string.h>

#if defined(WIN32)
// Windows
typedef unsigned __int64  uint64_t;
typedef unsigned int      uint32_t;
#elif defined(LINUX)
// Linux
typedef unsigned long long uint64_t;
typedef unsigned int       uint32_t;
#else
// Unix
typedef unsigned long     uint64_t;
typedef unsigned int      uint32_t;
#endif  // platform

#define INIT_BYTE       0xcd
#define FREE_BYTE       0xdb
#define FREE_WORD       0xdbdbdbdb

#define FENCE_BYTE      0xa5
#define FENCE_WORD      0xa5a5a5a5
#define FENCE_PATTERN   (((uint64_t) FENCE_WORD) | (((uint64_t) FENCE_WORD) << 32))
#define FREE_PATTERN    (((uint64_t) FREE_WORD) | (((uint64_t) FREE_WORD) << 32))

MemDbg_Callback *MemDbg_Interface::m_callback = NULL;
MemDbg_Lock *MemDbg_Interface::m_lock = NULL;

void *MemDbg_Interface::mdAbstractTable[] = {
  (void*)mdAbstractCall, (void*)mdAbstractCall, (void*)mdAbstractCall, (void*)mdAbstractCall,
  (void*)mdAbstractCall, (void*)mdAbstractCall, (void*)mdAbstractCall, (void*)mdAbstractCall,
  (void*)mdAbstractCall, (void*)mdAbstractCall, (void*)mdAbstractCall, (void*)mdAbstractCall,
  (void*)mdAbstractCall, (void*)mdAbstractCall, (void*)mdAbstractCall, (void*)mdAbstractCall,
  (void*)mdAbstractCall, (void*)mdAbstractCall, (void*)mdAbstractCall, (void*)mdAbstractCall,
  (void*)mdAbstractCall, (void*)mdAbstractCall, (void*)mdAbstractCall, (void*)mdAbstractCall,
  (void*)mdAbstractCall, (void*)mdAbstractCall, (void*)mdAbstractCall, (void*)mdAbstractCall,
  (void*)mdAbstractCall, (void*)mdAbstractCall, (void*)mdAbstractCall, (void*)mdAbstractCall,
  (void*)mdAbstractCall, (void*)mdAbstractCall, (void*)mdAbstractCall, (void*)mdAbstractCall,
  (void*)mdAbstractCall, (void*)mdAbstractCall, (void*)mdAbstractCall, (void*)mdAbstractCall,
  (void*)mdAbstractCall, (void*)mdAbstractCall, (void*)mdAbstractCall, (void*)mdAbstractCall,
  (void*)mdAbstractCall, (void*)mdAbstractCall, (void*)mdAbstractCall, (void*)mdAbstractCall,
  (void*)mdAbstractCall, (void*)mdAbstractCall, (void*)mdAbstractCall, (void*)mdAbstractCall,
  (void*)mdAbstractCall, (void*)mdAbstractCall, (void*)mdAbstractCall, (void*)mdAbstractCall,
  (void*)mdAbstractCall, (void*)mdAbstractCall, (void*)mdAbstractCall, (void*)mdAbstractCall,
  (void*)mdAbstractCall, (void*)mdAbstractCall, (void*)mdAbstractCall, (void*)mdAbstractCall,
  (void*)mdAbstractCall, (void*)mdAbstractCall, (void*)mdAbstractCall, (void*)mdAbstractCall,
  (void*)mdAbstractCall, (void*)mdAbstractCall, (void*)mdAbstractCall, (void*)mdAbstractCall
};

/*-----------------------------------------------------*/

class MemDbg_PureInterface {
public:
  static void mdAbstractCall(void *arg)
  {
    MemDbg_Interface::mdAbstractCall(arg);
  }
};

/*-----------------------------------------------------*/

class MemDbg_BlockEntry {
public:
  MemDbg_BlockEntry(void *ptr, size_t sz, MemDbg_BlockEntry *next)
    : m_ptr(ptr), m_sz(sz), m_next(next)
  {
  }

  void *operator new(size_t sz, MemDbg_BlockEntry *be)
  {
    return be;
  }

  void operator delete(void *ptr, MemDbg_BlockEntry *be)
  {
  }

  void              *m_ptr;
  size_t            m_sz;
  MemDbg_BlockEntry *m_next;
};

/*-----------------------------------------------------*/

class MemDbg_BlockHash {
public:
  MemDbg_BlockHash()
    : m_count(0), m_limit(0), m_size(0), m_hash(NULL)
  {
  }

  ~MemDbg_BlockHash()
  {
    if (m_hash) {
      // drop all hash entries
      // TODO: move to leak detection routine
      for (size_t i = 0; i < m_size; ++i) {
        MemDbg_BlockEntry *e = m_hash[i];
        if (e) {
          do {
            MemDbg_BlockEntry *todelete = e;
            e = e->m_next;
            MemDbg_Interface::mdQueryCallback()->
              mdSystemFree(todelete);
          } while (e);
          m_hash[i] = NULL;
        }
      }
      MemDbg_Interface::mdQueryCallback()->
        mdSystemFree(m_hash);
      m_count = m_size = m_limit = 0;
      m_hash = NULL;
    }
  }

  bool insert(void *ptr, size_t sz)
  {
    if (m_count >= m_limit) {
      // needs resize
      size_t newsize;
      if (m_limit == 0) {
        newsize = 11;
      } else {
        newsize = 2 * m_size + 1;
      }

      MemDbg_BlockEntry **newentry = reinterpret_cast<MemDbg_BlockEntry**>(
        MemDbg_Interface::mdQueryCallback()->
        mdSystemAllocate(sizeof(MemDbg_BlockEntry) * newsize));
      if (newentry == NULL) {
        return false;
      }
      memset(newentry, 0, sizeof(MemDbg_BlockEntry) * newsize);

      // rehash
      for (size_t i = 0; i < m_size; ++i) {
        MemDbg_BlockEntry *e = m_hash[i];
        while (e != NULL) {
          MemDbg_BlockEntry *next = e->m_next;
          size_t bucket = hash(e->m_ptr, newsize);
          e->m_next = newentry[bucket];
          newentry[bucket] = e;
          e = next;
        }
      }

      m_size = newsize;
      m_limit = newsize * 7 / 10;
      MemDbg_Interface::mdQueryCallback()->mdSystemFree(m_hash);
      m_hash = newentry;
    }

    MemDbg_BlockEntry* &hd = head(ptr);

    MemDbg_BlockEntry *next = reinterpret_cast<MemDbg_BlockEntry*>(
      MemDbg_Interface::mdQueryCallback()->
      mdSystemAllocate(sizeof(MemDbg_BlockEntry)));
    if (next == NULL) {
      return false;
    }
    hd = new(next) MemDbg_BlockEntry(ptr, sz, hd);
    ++m_count;
    return true;
  }

  bool remove(void *ptr)
  {
    if (m_count == 0) {
      return false;
    }

    MemDbg_BlockEntry *last = NULL;
    MemDbg_BlockEntry *hd = head(ptr);

    while (hd) {
      if (hd->m_ptr == ptr) {
        // remove this one
        if (last) {
          last->m_next = hd->m_next;
        } else {
          head(ptr) = hd->m_next;
        }
        MemDbg_Interface::mdQueryCallback()->mdSystemFree(hd);
        --m_count;
        return true;
      }
      last = hd;
      hd = hd->m_next;
    }
    return false;
  }

  size_t getBucketCount()
  {
    return m_size;
  }

  MemDbg_BlockEntry *getBucketEntry(size_t idx)
  {
    return m_hash[idx];
  }

  void *operator new(size_t sz, MemDbg_BlockHash *bh)
  {
    return bh;
  }

  void operator delete(void *ptr, MemDbg_BlockHash *bh)
  {
  }

private:
  void *operator new(size_t sz)
  {
    return NULL;
  }

  void operator delete(void *ptr)
  {
  }

  inline size_t hash(void *ptr, size_t size)
  {
    return reinterpret_cast<unsigned long>(ptr) % size;
  }

  MemDbg_BlockEntry* &head(void *ptr)
  {
    return m_hash[hash(ptr, m_size)];
  }

  size_t            m_count;    ///< Count of the items in hash.
  size_t            m_limit;    ///< Limit for growing up the hash.
  size_t            m_size;     ///< Size of the hash (bucket count)
  MemDbg_BlockEntry **m_hash;   ///< Hash heads.
};

/*-----------------------------------------------------*/

#if defined(WIN32) && !defined(_WIN64)

//extern "C" void *_set_purecall_handler(void *function);

// since set_purecall_handler is not available, must do some tricks
static void changeCall(void *fncptr, void *newfunc)
{
  char *data = (char*) fncptr;
  long addr = ((long) data) & ~4095;
  DWORD oldprot;
  VirtualProtect((LPVOID) addr, 4096, PAGE_EXECUTE_READWRITE, &oldprot);

  data[0] = (char) 0xE9;                              // long jmp relative
  long val = ((long) newfunc) - ((long) fncptr) - 5;  // delta
  memcpy(data + 1, &val, 4);

  VirtualProtect((LPVOID) addr, 4096, oldprot, NULL);
}

void MemDbg_Interface::mdRegisterPurecall(void *purecallfnc)
{
  //_set_purecall_handler(memdbg_purecall);
  changeCall(purecallfnc, MemDbg_Interface::mdAbstractCall);
}

#elif defined(HPUX) || defined(AIX)

extern "C" void __PureVirtualCalled(void *arg)
{
  MemDbg_PureInterface::mdAbstractCall(arg);
}

#elif defined(LINUX)

extern "C" void __pure_virtual(void *arg)
{
  MemDbg_PureInterface::mdAbstractCall(arg);
}

#elif defined(OSF1)

extern "C" void __pure_virtual_called(void *arg)
{
  MemDbg_PureInterface::mdAbstractCall(arg);
}

#elif defined(SUN) || defined(SOLARIS)

// two versions here, one for older, one for newer runtime library

extern "C" void _pure_error_(void *arg)
{
  MemDbg_PureInterface::mdAbstractCall(arg);
}

extern "C" void __1cG__CrunKpure_error6F_v_(void *arg)
{
  MemDbg_PureInterface::mdAbstractCall(arg);
}

#endif  // platform switch

/*-----------------------------------------------------*/

MemDbg_Interface::MemDbg_Interface(MemDbg_Config &config, bool hasIterator)
  : m_config(config), m_freesize(0), m_freestart(0), m_freeend(0),
    m_freelist(NULL), m_instLock(m_callback->mdCreateLock()),
    m_usedBlks(NULL), m_hasIter(hasIterator)
{
  if (m_instLock == NULL) {
    // use global lock
    m_instLock = m_lock;
    m_callback->mdAllocatorMessage("Cannot create instance lock, using global lock",
      this, NULL, 0, 0, 0, m_callback->mdGetTraceID());
  }
#if defined(WIN32) && !defined(_WIN64)
  // make sure purifyRunning is correctly set, even if not yet initialized
  purifyRunning = PurifyIsRunning() != 0;
  mdRegisterPurecall(_purecall);
#endif  // WIN32
}

/*-----------------------------------------------------*/

MemDbg_Interface::~MemDbg_Interface()
{
  if (m_usedBlks) {
    // drop used blocks
    m_usedBlks->~MemDbg_BlockHash();
    m_callback->mdSystemFree(m_usedBlks);
    m_usedBlks = NULL;
  }
  if (m_freelist) {
    // this is an error - forgotten cleanup
    m_callback->mdAllocatorMessage("Forgotten mdCleanup() in allocator destructor", this,
      NULL, 0, 0, 0, m_callback->mdGetTraceID());
  }
  m_callback->mdDestroyLock(m_instLock);
}

/*-----------------------------------------------------*/

void MemDbg_Interface::mdAbstractCall(void *arg)
{
  // identify the block/object, if possible
  const void *thisptr = NULL, *vtableptr = NULL;
#if defined(WIN32) && !defined(_WIN64)
  // Win32 has this in ecx
  __asm {
    mov thisptr, ecx
  }
  __try {
    // get vtable, if possible
    vtableptr = *reinterpret_cast<const void* const*>(thisptr);
  } __except(1) {
    // ignore error, set block to NULL
    thisptr = NULL;
  }
#elif defined(HPUX) || defined(LINUX) || defined(OSF1) || defined(AIX) || defined(SUN) || defined(SOLARIS)
  thisptr = arg;
  // get VTable safely
  // actually there must be some vtable if we make it here over abstract call...
  if ((reinterpret_cast<ptrdiff_t>(thisptr) & 7) == 0) {
    // only if properly aligned (actually it must be always aligned)
    vtableptr = *reinterpret_cast<const void* const*>(thisptr);
  }
#else
  // don't know yet how to handle
#endif
  if (vtableptr == mdAbstractTable) {
    // it is our abstract table, sorry
    vtableptr = NULL;
  }
  int allocTrace = 0, freeTrace = 0;
  size_t size = 0;
  if (thisptr != NULL) {
    // try to read stacktraces
    if ((reinterpret_cast<ptrdiff_t>(thisptr) & 15) == 8) {
      // potential debug block header
      const MemDbg_Header *hdr = reinterpret_cast<const MemDbg_Header*>(
        reinterpret_cast<const char*>(thisptr) - 8);
      allocTrace = hdr->getAllocTrace();
      freeTrace = hdr->getFreeTrace();
      size = hdr->getSize(true);
    }
  }
  m_callback->mdAbstractCall(thisptr, vtableptr, size, allocTrace, freeTrace);
  m_callback->mdAbort("Pure virtual function called");
}

/*-----------------------------------------------------*/

void *MemDbg_Interface::mdAllocate(size_t size)
{
  if (purifyRunning) {
    // use system routines
    return m_callback->mdSystemAllocate(size);
  }

  if (m_callback == NULL) {
    // sorry, no callback yet, allocate normally
    void *ptr = mdAllocatePrim(size);
    if (reinterpret_cast<ptrdiff_t>(ptr) & 15) {
      // error - allocation not at 16B alignment
      mdDeallocatePrim(ptr);
      return mdBadAllocHandling();
    }
    return ptr;
  }

  MemDbg_Config localConfig(m_config);
  if (localConfig.needsHeader()) {
    // compute sizes
    size_t extra = localConfig.computeDbgSpace(size);
    size_t alignedSize = (size + 7) & ~7;
    if (alignedSize < 16) {
      alignedSize = 16;
    }

    // allocate underlying block
    char *ptr = reinterpret_cast<char*>(mdAllocatePrim(alignedSize + extra));
    if( ptr == NULL){
      m_callback->mdAllocatorMessage("Could not allocate memory",
      this, ptr, size, m_callback->mdGetTraceID(), 0, 0);
      return mdBadAllocHandling();
    }
    if (reinterpret_cast<ptrdiff_t>(ptr) & 15) {
      // error - allocation not at 16B alignment
      m_callback->mdAllocatorMessage("Underlying allocator returned memory NOT aligned at 16B",
        this, ptr, size, m_callback->mdGetTraceID(), 0, 0);
      mdDeallocatePrim(ptr);
      return mdBadAllocHandling();
    }


    MemDbg_Header *headerPtr = reinterpret_cast<MemDbg_Header*>(ptr);
    if (extra > 16) {
      // reserved extra word for size between two copies of the header
      headerPtr += 2;
    }
    headerPtr->setSize(size, true);

    if (localConfig.isFence()) {
      // store fence at end of the block
      register char *endFence = ptr + alignedSize + extra - 8;
      *reinterpret_cast<uint64_t*>(endFence) = FENCE_PATTERN;
      register size_t diff = alignedSize - size;
      if (diff) {
        // size not aligned, store also in block
        char *stop = endFence - diff;
        while (--endFence >= stop) {
          *endFence = (char) FENCE_BYTE;
        }
      }
      headerPtr->setFence();
    } else {
      headerPtr->clearFence();
    }

    // set up alloc trace
    if (localConfig.isNoLeak()) {
      // special constant for no-leak blocks
      headerPtr->setAllocTrace(MemDbg_Header::NOLEAK);
    } else {
      if (localConfig.isAllocTrace()) {
        // store allocating stacktrace
        headerPtr->setAllocTrace(m_callback->mdGetTraceID());
      } else {
        headerPtr->setAllocTrace(0);
      }
    }

    // set up doublekeeping, if needed
    if (localConfig.isDoublekeep()) {
      MemDbg_LockedScope lck(m_instLock);
      if (!m_usedBlks) {
        // create used blocks
        m_usedBlks = reinterpret_cast<MemDbg_BlockHash*>(
          m_callback->mdSystemAllocate(sizeof(MemDbg_BlockHash)));
        new(m_usedBlks) MemDbg_BlockHash();
        if (m_usedBlks) {
          if (m_usedBlks->insert(reinterpret_cast<char*>(headerPtr) + 8, size)) {
            headerPtr->setDoublekeep();
          } else {
            // ignore - cannot insert block
            headerPtr->clearDoublekeep();
          }
        } else {
          // ignore - cannot create used blocks
          headerPtr->clearDoublekeep();
        }
      } else {
        if (m_usedBlks->insert(reinterpret_cast<char*>(headerPtr) + 8, size)) {
          headerPtr->setDoublekeep();
        } else {
          // ignore - cannot insert block
          headerPtr->clearDoublekeep();
        }
      }
    } else {
      headerPtr->clearDoublekeep();
    }

    if (extra > 16) {
      // needs second header before size, copy it there
      headerPtr[-2] = *headerPtr;
    }

    ptr = reinterpret_cast<char*>(headerPtr) + 8;
    if (localConfig.isInitialize()) {
      // initialize block with pattern
      memset(ptr, INIT_BYTE, size);
    }
    return ptr;
  }

  // non-debug allocation
  void *ptr = mdAllocatePrim(size);
  if (reinterpret_cast<ptrdiff_t>(ptr) & 15) {
    // error - allocation not at 16B alignment
    m_callback->mdAllocatorMessage("Underlying allocator returned memory NOT aligned at 16B",
      this, ptr, size, m_callback->mdGetTraceID(), 0, 0);
    mdDeallocatePrim(ptr);
    return mdBadAllocHandling();
  }
  if (localConfig.isInitialize()) {
    // initialize block with pattern
    memset(ptr, INIT_BYTE, size);
  }
  return ptr;
}

/*-----------------------------------------------------*/

void MemDbg_Interface::mdDeallocate(void *ptr)
{
  if (ptr == NULL) {
    // nothing to do
    return;
  }
  if (purifyRunning) {
    // use system routines
    return m_callback->mdSystemFree(ptr);
  }

  if ((reinterpret_cast<ptrdiff_t>(ptr) & 15) == 0) {
    // ordinary block, just free it up
    mdDeallocatePrim(ptr);
    return;
  }

  if ((reinterpret_cast<ptrdiff_t>(ptr) & 7) != 0) {
    // invalid pointer, not alligned
    m_callback->mdAllocatorMessage("mdDeallocate not alligned pointer", this,
          ptr, 0, 0, 0, m_callback->mdGetTraceID());
    return;
  }

  char *pptr = reinterpret_cast<char*>(ptr);
  char *realBlock = pptr - 8;
  MemDbg_Header *header = reinterpret_cast<MemDbg_Header*>(realBlock);
  size_t size = header->getSize(true);
  size_t alignedSize = (size + 7) & ~7;
  if (alignedSize < 16) {
    alignedSize = 16;
  }
  if (size > MemDbg_Header::MAX_SIZE) {
    // subtract space needed for the size
    realBlock -= 16;
  }

  if (header->isDoubleKeep()) {
    // check double-keeping hashlist
    MemDbg_LockedScope lck(m_instLock);
    if (m_usedBlks && m_usedBlks->remove(ptr)) {
      // ok
    } else {
      // error
      m_callback->mdNotInUseFree(this, ptr, size,
        header->getAllocTrace(), header->getFreeTrace());
      return;
    }
  }
  if (!mdInUse(realBlock)) {
    // already freed
    m_callback->mdNotInUseFree(this, ptr, size,
      header->getAllocTrace(), header->getFreeTrace());
    return;
  }

  if (header->isFence()) {
    // check postfence
    register char *endFence = pptr + alignedSize;
    if (*reinterpret_cast<uint64_t*>(endFence) != FENCE_PATTERN) {
      // error
      m_callback->mdPostFenceOverwritten(this, pptr, size,
        header->getAllocTrace(), header->getFreeTrace());
    } else {
      // check unaligned bytes
      register size_t diff = alignedSize - size;
      if (diff) {
        // size not aligned, store also in block
        char *stop = endFence - diff;
        while (--endFence >= stop) {
          if (*endFence != (char) FENCE_BYTE) {
            // error
            m_callback->mdPostFenceOverwritten(this, pptr, size,
              header->getAllocTrace(), header->getFreeTrace());
            break;
          }
        }
      }
    }
  }

  // fill in free trace, if requested
  if (m_config.isFreeTrace() && m_callback != NULL) {
    header->setFreeTrace(m_callback->mdGetTraceID());
  } else {
    header->setFreeTrace(0);
  }

  if (m_config.isDelayFreeCheck() || m_config.isFreeCheck()) {
    // prepare the block - fill in with free pattern (incl. fence, if any)
    register int words = alignedSize >> 3;
    register char *dptr = pptr;
    if (header->isFence()) {
      ++words;
    }
    if (m_config.isQuickFreeCheck()) {
      header->setQuickCheck();
      // adjust word count and start address
      if (words >= 3) {
        words = 2;
      } else {
        --words;
      }
      dptr += 8;
    } else {
      header->clearQuickCheck();
    }
    // initialize free space
    while (--words >= 0) {
      *reinterpret_cast<uint64_t*>(dptr) = FREE_PATTERN;
      dptr += 8;
    }
    header->setFreeCheck();
  } else {
    header->clearFreeCheck();
  }

  // put in the abstract VTable
  *reinterpret_cast<void**>(ptr) = mdAbstractTable;

  if (m_config.isDelayFree()) {
    // manage for later deallocation
    manageDelayFree(ptr);
  } else {
    // normal deallocate
    mdDeallocatePrim(realBlock);
  }
}

/*-----------------------------------------------------*/

bool MemDbg_Interface::mdCleanup(bool checkLeaks)
{
  mdFreeDelayed(true);
  if (checkLeaks) {
    return mdCheckLeaks();
  } else {
    return false;
  }
}

/*-----------------------------------------------------*/

void MemDbg_Interface::mdFreeDelayed(bool freeFreelist)
{
  if (m_freelist) {
    MemDbg_LockedScope lck(m_instLock);

    // free up all items in the freelist
    while (m_freestart != m_freeend) {
      freeLastDelayFree();
    }
    if (freeFreelist) {
      m_callback->mdSystemFree(m_freelist);
      m_freelist = NULL;
    }
  }
}

/*-----------------------------------------------------*/

bool MemDbg_Interface::mdCheckLeaks()
{
  bool found = false;
  mdFreeDelayed();
  if (m_hasIter) {
    size_t size;
    const void *ptr = mdGetNextBlock(NULL, size);
    ptrdiff_t val = reinterpret_cast<ptrdiff_t>(ptr);
    if (val & 15) {
      m_callback->mdAllocatorMessage("mdCheckLeaks encountered wrong addr in iteration", this,
        ptr, 0, 0, 0, m_callback->mdGetTraceID());
      return true;
    }
    while (ptr) {
      // dump out this block
      MemDbg_Header *header;
      void *realBlock;
      decodeAddr(const_cast<void*>(ptr), size, header, realBlock);

      if (header != NULL) {
        // is debug block
        if (header->getAllocTrace() != MemDbg_Header::NOLEAK) {
          mdReportLeak(header + 1);
          found = true;
        }
      } else {
        // is normal block
        mdReportLeak(realBlock);
        found = true;
      }

      ptr = mdGetNextBlock(ptr, size);
      ptrdiff_t val = reinterpret_cast<ptrdiff_t>(ptr);
      if (val & 15) {
        m_callback->mdAllocatorMessage("mdCheckLeaks encountered wrong addr in iteration", this,
          ptr, 0, 0, 0, m_callback->mdGetTraceID());
        return true;
      }
    }
  } else if (m_usedBlks) {
    // iterate over all blocks in hash
    size_t buckets = m_usedBlks->getBucketCount();
    for (size_t ptr = 0; ptr < buckets; ++ptr) {
      MemDbg_BlockEntry *e = m_usedBlks->getBucketEntry(ptr);
      while (e != NULL) {
        const MemDbg_Header *header = reinterpret_cast<const MemDbg_Header*>(e->m_ptr) - 1;
        MemDbg_BlockEntry *next = e->m_next;
        int allocTrace = header->getAllocTrace();
        if (allocTrace != MemDbg_Header::NOLEAK) {
          mdReportLeak(e->m_ptr);
          found = true;
        }
        e = next;
      }
    }
  }
  return found;
}

/*-----------------------------------------------------*/

bool MemDbg_Interface::decodeAddr(void *ptr, size_t size, MemDbg_Header* &header, void* &realBlock)
{
  ptrdiff_t val = reinterpret_cast<ptrdiff_t>(ptr);

  if ((val & 15) == 8) {

    // debug block
    header = reinterpret_cast<MemDbg_Header*>(ptr) - 1;
    if (header->getSize(true) > MemDbg_Header::MAX_SIZE) {
      // big-size block
      realBlock = header - 2;
    } else {
      // normal block
      realBlock = header;
    }
    return true;

  } else if ((val & 15) == 0) {

    // don't know, ptr points to allocated block
    realBlock = ptr;
    if (ptr == NULL) {
      header = NULL;
      return true;
    }
    header = reinterpret_cast<MemDbg_Header*>(ptr);
    size_t blksize = header->getSize(false);
    if (size == NULL) {
      size = mdGetBlockSize(ptr);
    }
    if (blksize + 32 > size && blksize + 8 <= size) {
      // it is properly sized, have header
      if (blksize > MemDbg_Header::MAX_SIZE) {
        header += 2;
      }
      return true;
    }
    // it's not a debug block
    header = NULL;
    return true;

  } else {

    // error in address
    m_callback->mdAllocatorMessage("decodeAddr encountered wrong addr in iteration", this,
      ptr, 0, 0, 0, m_callback->mdGetTraceID());
    header = NULL;
    realBlock = NULL;
    return false;
  }
}

/*-----------------------------------------------------*/

bool MemDbg_Interface::decodeAddr(const void *ptr, size_t size, const MemDbg_Header* &header, const void* &realBlock)
{
  ptrdiff_t val = reinterpret_cast<ptrdiff_t>(ptr);

  if ((val & 15) == 8) {

    // debug block
    header = reinterpret_cast<const MemDbg_Header*>(ptr) - 1;
    if (header->getSize(true) > MemDbg_Header::MAX_SIZE) {
      // big-size block
      realBlock = header - 2;
    } else {
      // normal block
      realBlock = header;
    }
    return true;

  } else if ((val & 15) == 0) {

    // don't know, ptr points to allocated block
    realBlock = ptr;
    if (ptr == NULL) {
      header = NULL;
      return true;
    }
    header = reinterpret_cast<const MemDbg_Header*>(ptr);
    size_t blksize = header->getSize(false);
    if (size == NULL) {
      size = mdGetBlockSize(ptr);
    }
    if (blksize + 32 > size && blksize + 8 <= size) {
      // it is properly sized, have header
      if (blksize > MemDbg_Header::MAX_SIZE) {
        header += 2;
      }
      return true;
    }
    // it's not a debug block
    header = NULL;
    return true;

  } else {

    // error in address
    m_callback->mdAllocatorMessage("decodeAddr encountered wrong addr in iteration", this,
      ptr, 0, 0, 0, m_callback->mdGetTraceID());
    header = NULL;
    realBlock = NULL;
    return false;
  }
}

/*-----------------------------------------------------*/

void MemDbg_Interface::mdReportLeak(void *ptr)
{
  MemDbg_Header *header;
  void *realBlock;
  if (!decodeAddr(ptr, 0, header, realBlock)) {
    return;
  }

  if (header) {
    if (header->getAllocTrace() != MemDbg_Header::NOLEAK) {
      // only if not marked as bad
      header->setFreeTrace(0);  // prevent displaying (previous) free traces
      mdReportBlock(ptr, "Memory leak", false);
      mdMarkBadBlock(ptr);
    }
  } else {
    // it is not a debug block
    mdReportBlock(ptr, "Memory leak", false);
    mdMarkBadBlock(ptr);
  }
}

/*-----------------------------------------------------*/

void MemDbg_Interface::mdReportBlock(void *ptr, const char *message, bool curStack)
{
  MemDbg_Header *header;
  void *realBlock;
  if (!decodeAddr(ptr, 0, header, realBlock)) {
    return;
  }

  if (header) {
    // debug block, have maybe traces and such
    m_callback->mdAllocatorMessage(message, this, header + 1, header->getSize(true),
      header->getAllocTrace(), header->getFreeTrace(),
      curStack ? m_callback->mdGetTraceID() : 0);
  } else {
    // normal block, no traces
    size_t size;
    if (ptr) {
      size = mdGetBlockSize(ptr);
    } else {
      size = 0;
    }
    m_callback->mdAllocatorMessage(message, this, ptr, size,
      0, 0, curStack ? m_callback->mdGetTraceID() : 0);
  }
}

/*-----------------------------------------------------*/

void MemDbg_Interface::mdMarkBadBlock(void *ptr)
{
  MemDbg_Header *header;
  void *realBlock;
  if (!decodeAddr(ptr, 0, header, realBlock)) {
    return;
  }

  if (header) {
    header->setAllocTrace(MemDbg_Header::NOLEAK);
  }
  if (realBlock) {
    mdMarkBadBlockPrim(realBlock);
  }
}

/*-----------------------------------------------------*/

void MemDbg_Interface::manageDelayFree(void *ptr)
{
  char *pptr = reinterpret_cast<char*>(ptr);
  char *realBlock = pptr - 8;
  MemDbg_Header *header = reinterpret_cast<MemDbg_Header*>(realBlock);
  size_t size = header->getSize(true);
  size_t alignedSize = (size + 7) & ~7;

  MemDbg_LockedScope lck(m_instLock);

  if (m_freelist == NULL) {
    m_freelist = (void**) m_callback->mdSystemAllocate(sizeof(void*) * MD_MAX_DELAYED_COUNT);
    if (m_freelist == NULL) {
      // deallocate immediately, cannot get memory for blocks
      if (size > MemDbg_Header::MAX_SIZE) {
        // subtract space needed for the size
        realBlock -= 16;
      }
      mdDeallocatePrim(realBlock);
      return;
    }
  }

  // we have the freelist, add new block there, if possible
  while (m_freesize + alignedSize >= m_config.getMaxFreeSize() && m_freeend != m_freestart) {
    // deallocate the last one
    freeLastDelayFree();
  }
  size_t nextidx = m_freeend + 1;
  if (nextidx == MD_MAX_DELAYED_COUNT) {
    nextidx = 0;
  }
  if (nextidx == m_freestart) {
    // no space in freelist, deallocate oldest
    freeLastDelayFree();
  }

  // put the block in the list
  m_freelist[m_freeend] = ptr;
  m_freeend = nextidx;
}

/*-----------------------------------------------------*/

void MemDbg_Interface::freeLastDelayFree()
{
  void *ptr = m_freelist[m_freestart++];
  if (m_freestart == MD_MAX_DELAYED_COUNT) {
    m_freestart = 0;
  }

  char *pptr = reinterpret_cast<char*>(ptr);
  char *realBlock = pptr - 8;
  MemDbg_Header *header = reinterpret_cast<MemDbg_Header*>(realBlock);
  size_t size = header->getSize(true);
  size_t alignedSize = (size + 7) & ~7;
  if (size > MemDbg_Header::MAX_SIZE) {
    // subtract space needed for the size
    realBlock -= 16;
  }

  // check for overwrites in free space
  for (;;) {
    if (*reinterpret_cast<void**>(ptr) != mdAbstractTable) {
      // found error in data
      m_callback->mdFreeDataOverwritten(this, ptr, size,
        header->getAllocTrace(), header->getFreeTrace());
      break;
    }
    // put free pattern back
    *reinterpret_cast<void**>(ptr) = reinterpret_cast<void*>(FREE_PATTERN);

    register int words = alignedSize >> 3;
    register char *dptr = pptr;
    if (header->isFence()) {
      ++words;
    }
    if (header->isQuickCheck()) {
      // adjust word count and start address
      if (words >= 3) {
        words = 2;
      } else {
        --words;
      }
      dptr += 8;
    }
    // check free space
    while (--words >= 0) {
      if (*reinterpret_cast<uint64_t*>(dptr) != FREE_PATTERN) {
        // found error in data
        m_callback->mdFreeDataOverwritten(this, ptr, size,
          header->getAllocTrace(), header->getFreeTrace());
        break;
      }
      dptr += 8;
    }
    break;
  }

  // is OK, deallocate
  mdDeallocatePrim(realBlock);
}

/*-----------------------------------------------------*/

void MemDbg_Interface::mdCheckFree(const void *ptr, size_t size, size_t skipBytes)
{
  // TODO
}

/*-----------------------------------------------------*/

const void *MemDbg_Interface::mdGetNextBlock(const void *last, size_t &size)
{
  size = 0;
  return NULL;
}

/*-----------------------------------------------------*/

void *MemDbg_Interface::mdBadAllocHandling()
{
  return NULL;
}

/*-----------------------------------------------------*/

MemDbg_Callback *MemDbg_Interface::mdRegisterCallback(MemDbg_Callback *cb)
{
  if (m_callback != NULL) {
    return m_callback;
  }
  m_callback = cb;

  // create synchronization lock
  m_lock = cb->mdCreateLock();
  if (m_lock == NULL) {
    cb->mdAbort("Cannot create lock when registering the callback");
  }

  return NULL;
}

/*-----------------------------------------------------*/
