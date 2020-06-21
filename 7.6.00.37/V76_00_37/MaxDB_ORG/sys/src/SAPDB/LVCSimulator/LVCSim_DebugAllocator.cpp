/*!
\file       LVCSim_DebugAllocator.cpp
\brief      Debugging memory allocator.
\author     Roterring
*/
/*

    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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

#include "LVCSimulator/LVCSim_Internal.hpp"
#include "LVCSimulator/LVCSim_DebugAllocator.hpp"
#include "LVCSimulator/LVCSim_DebugCallback.hpp"
#include "MemoryDebugger/MemDbg_Interface.hpp"
#include "MemoryDebugger/MemDbg_Config.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"

#ifndef OSF1

/// Dump all alloc/deallocs in log
#undef ALLOC_DEBUG

/// Default configuration for debug memory allocator.
static MemDbg_Config *memdbgConfig = NULL;
static double        memdbgConfigSpace[sizeof(MemDbg_Config) / sizeof(double) + 1];

static int        dumpTraceMessages = 0;
static int				dumpAllocMessages = 1;

typedef tsp00_Uint4 uint32_t;

/*!
 * \brief Debug allocator implementation.
 *
 * \ingroup    lvcsimulator
 */
class LVCSim_DebugAllocatorImp : public LVCSim_DebugAllocator, MemDbg_Interface
{
public:
  LVCSim_DebugAllocatorImp() : MemDbg_Interface(*memdbgConfig, false)
  {
  }

  ~LVCSim_DebugAllocatorImp()
  {
    mdCleanup();
  }

  /*!
   * \brief Allocates contiguous memory for \c ByteCount bytes.
   *
   * The memory is aligned as indicated by the 'GetAlignmentSize' method.
   * The memory is not initialised.
   *
   * \param ByteCount The size in bytes of the memory to be allocated. This 
   *                  argument must be greater zero.
   * \param Hint      If possible, the allocator tries to reserve the memory
   *                  at the same position as given by \c Hint. This is useful
   *                  for resizing a memory block. If the reallocation was
   *                  succesfull, the return value and \c Hint are identical.
   *
   * \return A pointer to the allocated raw memory block of \c ByteCount
   *    bytes. If exception handling is disabled, a NULL pointer will be returned
   *    in case of insufficient memory.
   * \throw SAPDBMem_BadAllocException may be thrown in classes derived from
   *    SAPDBMem_IRawAllocator if the allocation fails.
   */
  virtual void* Allocate(SAPDB_ULong ByteCount, const void* Hint)
  {
    return mdAllocate(ByteCount);
  }

  /*!
   * \brief Allocates contiguous memory for \c ByteCount bytes.
   *
   * The memory is aligned as indicated by the 'GetAlignmentSize' method.
   * The memory is not initialised.
   *
   * \param ByteCount The size in bytes of the memory to be allocated. This 
   *                  argument must be greater zero.
   *
   * \return A pointer to the allocated raw memory block of \c ByteCount
   *    bytes. If exception handling is disabled, a NULL pointer will be returned
   *    in case of insufficient memory.
   * \throw SAPDBMem_BadAllocException may be thrown in classes derived from
   *    SAPDBMem_IRawAllocator if the allocation fails.
   */
  virtual void* Allocate(SAPDB_ULong ByteCount)
  {
    ++m_alloccnt;
    return mdAllocate(ByteCount);
  }

  /*!
   * \brief Frees the memory.
   * 
   * The memory must have been allocated by a previous call to Allocate. 
   * This is checked by assertions.
   *
   * \note An implementation of this method may not throw an exception.
   *
   * \param p block to free.
   */
  virtual void Deallocate(void* p)
  {
    ++m_dealloccnt;
    mdDeallocate(p);
  }

  virtual void* AllocateArray(SAPDB_ULong ByteCount)
  {
    return mdAllocate(ByteCount);
  }

  virtual void DeallocateArray(void* p)
  {
    mdDeallocate(p);
  }

  virtual SAPDB_ULong GetChunkSize(const void* p);

  virtual bool doEscape(LVCSim_DebugAllocatorEscapeEnum what);

  virtual void doBlockError(const char *msg, const void *blk);

  virtual void GetBaseAllocatorCallStatistics(SAPDB_ULong &CountAlloc,
    SAPDB_ULong &CountDealloc) const
  {
    GetCallStatistics(CountAlloc, CountDealloc);
  }

  virtual void GetCallStatistics(SAPDB_ULong &CountAlloc,
    SAPDB_ULong &CountDealloc) const
  {
    CountAlloc = m_alloccnt;
    CountDealloc = m_dealloccnt;
  }

  virtual void* mdAllocatePrim(size_t size);

  virtual void mdDeallocatePrim(void* ptr);

  virtual size_t mdGetBlockSize(const void *ptr)
  {
    return GetChunkSize(ptr);
  }

  virtual const char *mdGetName()
  {
    return "<global>";
  }

  SAPDB_ULong	m_alloccnt;
  SAPDB_ULong	m_dealloccnt;
};


/*-----------------------------------------------------*/

static double debug_allocator_space[sizeof(LVCSim_DebugAllocatorImp) / sizeof(double) + 1];
LVCSim_DebugAllocator *LVCSim_DebugAllocator::m_instance = NULL;

/*-----------------------------------------------------*/

LVCSim_DebugAllocator &LVCSim_DebugAllocator::Instance()
{
  if (m_instance == NULL) {
    memdbgConfig = reinterpret_cast<MemDbg_Config*>(memdbgConfigSpace);

    new (memdbgConfig) MemDbg_Config(0xffffff & ~MemDbg_Config::FLG_QUICKFREECHECK, 10*1024*1024);
    //new (memdbgConfig) MemDbg_Config(0x0, 10*1024*1024);
    MemDbg_Interface::mdRegisterCallback(&LVCSim_DebugCallback::getInstance());
      m_instance = new(debug_allocator_space) LVCSim_DebugAllocatorImp();
  }
  return *m_instance;
}

/*-----------------------------------------------------*/

bool LVCSim_DebugAllocator::Escape(LVCSim_DebugAllocatorEscapeEnum what)
{
  switch (what) {
  case LVCSIM_DA_ON:
    memdbgConfig = reinterpret_cast<MemDbg_Config*>(memdbgConfigSpace);
    new (memdbgConfig) MemDbg_Config(0xffffff & ~MemDbg_Config::FLG_QUICKFREECHECK, 10*1024*1024);
    break;
  case LVCSIM_DA_OFF:
    memdbgConfig = reinterpret_cast<MemDbg_Config*>(memdbgConfigSpace);
    new (memdbgConfig) MemDbg_Config(0, 10*1024*1024);
    break;
  case LVCSIM_DA_ALLOCERR_ON:
    ++dumpAllocMessages;
    break;
  case LVCSIM_DA_ALLOCERR_OFF:
    --dumpAllocMessages;
    break;
  case LVCSIM_DA_SYMREFRESH:
    // ignore - not used anymore
    break;
  default:
    return Instance().doEscape(what);
  }
  return false;
}

/*-----------------------------------------------------*/
/*-----------------------------------------------------*/

SAPDB_ULong LVCSim_DebugAllocatorImp::GetChunkSize(const void* ptr)
{
  const char *p = reinterpret_cast<const char*>(ptr);
  size_t size = *reinterpret_cast<const uint32_t*>(p - 4);
  return size & ((1<<28) - 1);
}

/*-----------------------------------------------------*/

bool LVCSim_DebugAllocatorImp::doEscape(LVCSim_DebugAllocatorEscapeEnum what)
{
  switch (what) {
  case LVCSIM_DA_CHECK_LEAKS:
    return mdCheckLeaks();
    break;
  case LVCSIM_DA_STACKTRACE:
    // dump current stack trace
    mdQueryCallback()->mdAllocatorMessage("User-triggered trace", this, NULL, 0,
      0, 0, mdQueryCallback()->mdGetTraceID());
    break;
  case LVCSIM_DA_ENTER_NOLEAK:
    memdbgConfig->noLeaksOn();
    break;
  case LVCSIM_DA_EXIT_NOLEAK:
    memdbgConfig->noLeaksOff();
    break;
  }
  return false;
}

/*-----------------------------------------------------*/

void LVCSim_DebugAllocatorImp::doBlockError(const char *msg, const void *blk)
{
}

/*-----------------------------------------------------*/

void* LVCSim_DebugAllocatorImp::mdAllocatePrim(size_t size)
{
  /*
  Worst-case: allocation at 16B alignment:
  XXXX XXXX XXXX SSSS Block data
  Where high 4 bits of SSSS define delta from block beginning.
  */
  if (size >= 1 << 28) {
    // sorry, we allow maximum 256MB blocks
    return NULL;
  }
  char *p = reinterpret_cast<char*>(malloc(size + 16));
  if (p == NULL) {
    THROW_BAD_ALLOC_GEO573;
  }
  ptrdiff_t addr = reinterpret_cast<ptrdiff_t>(p);
  ptrdiff_t delta = 16 - (addr & 15);
  p += delta;
  *reinterpret_cast<uint32_t*>(p - 4) = (delta << 28) | size;
#ifdef ALLOC_DEBUG
  char buf[128];
  sprintf(buf, "Allocate delta %d", delta);
  mdQueryCallback()->mdAllocatorMessage(buf, this, p, size, 0, 0, 0);
#endif
  return p;
}

/*-----------------------------------------------------*/

void LVCSim_DebugAllocatorImp::mdDeallocatePrim(void* ptr)
{
  char *p = reinterpret_cast<char*>(ptr);
  size_t size = *reinterpret_cast<uint32_t*>(p - 4);
  size_t delta = size >> 28;
#ifdef ALLOC_DEBUG
  char buf[128];
  sprintf(buf, "Deallocate delta %d", delta);
  mdQueryCallback()->mdAllocatorMessage(buf, this, ptr, size & ((1 << 28) - 1), 0, 0, 0);
#endif
  if (delta == 0) {
    p -= 16;
  } else {
    p -= delta;
  }
  free(p);
}

/*-----------------------------------------------------*/













#else   // old stuff

#include "RunTime/Synchronisation/RTESync_Spinlock.hpp"
#include <stdlib.h>
#include <string.h>
#include "hsp77.h"
#include "geo573.h"
#include "heo670.h"

#if WIN32
#ifndef _DEBUG
#define	_DEBUG
#endif
#include <io.h>
#include <crtdbg.h>
#include <Dbghelp.h>
#else
#include <unistd.h>
#endif


#if defined(WIN32) && !defined(_WIN64)
// own symbol lookup
//#define USE_OWN_SYMLOOKUP
#undef USE_OWN_SYMLOOKUP
#endif

LVCSim_DebugAllocator	*LVCSim_DebugAllocator::m_instance = NULL;

bool                    __debug_lvcsim_throw_badalloc = false;

#define	FENCECNT	7		// size of pre-write fence
#define	FENCECHAR	0xa5	// fence char
#define	EMPTYCHAR	0xcd	// empty space filler
#define	MIN_FL		16		// minimum alloc unit
#define	MAX_FL		32768	// maximum alloc for "small" block - in freelists
#define	FL_COUNT	12		// freelist count

#if defined( SUN ) || defined( SOLARIS )
#define TRACE_SKIP  2       // skip levels
#elif !defined( WIN32 )
#define TRACE_SKIP  5       // skip levels 
#else
#define TRACE_SKIP  6       // skip levels
#endif

#define	TRACE_SIZE	30		// levels of backtrace
#define	TRACE_HASH	60013	// # of hashlist headers

#define MIN_FREE_COUNT  200 // keep min. 200 blocks in each freelist

/*
Freelists will be created for powers of two (16, 32, 64, 128, 256, 512, 1024, 2048,
4096, 8192, 16384, 32768), bigger blocks
will be allocated using malloc()
*/


static RTESync_Spinlock	*traceLock = NULL;
static FILE             *traceFile = NULL;
static int              dumpTraceMessages = 0;
static int				      dumpAllocMessages = 1;

class LVCSim_TraceLock {
public:
  LVCSim_TraceLock()
  {
    if (!traceLock) {
      traceLock = (RTESync_Spinlock*) malloc(sizeof(RTESync_Spinlock));
      traceLock = new(traceLock) RTESync_Spinlock();
    }
    traceLock->Lock();
  }
  ~LVCSim_TraceLock()
  {
    traceLock->Unlock();
  }
};

static void message(const char *fmt, ...) 
{
  char buf2[4096];
  va_list args;
  va_start(args, fmt);
  sp77vsprintf(buf2, (unsigned int) sizeof(buf2), fmt, args);
  va_end(args);
  if (dumpTraceMessages) {
#ifdef WIN32
    _write(2, buf2, (unsigned int) strlen(buf2));
#else
    write(2, buf2, strlen(buf2));
#endif
  }
  if (!traceFile) {
    // TODO: make configurable
    traceFile = fopen("mem_trace.txt", "w");
    if (!traceFile) return;
  }
  fputs(buf2, traceFile);
  fflush(traceFile);
}

typedef void *voidPtr;

class TraceHashBucket {
public:
  TraceHashBucket	*next;
  unsigned int	key;
  int             reported_count;
  voidPtr			trace[TRACE_SIZE];

  void init(unsigned int k, voidPtr *t, TraceHashBucket *n)
  {
    next = n;
    key = k;
    reported_count = 0;
    SAPDB_MemCopyNoCheck(&trace[0], t, sizeof(trace));
  }

  bool traceEqual(voidPtr *t)
  {
    for (int i = 0; i < TRACE_SIZE; ++i) {
      if (trace[i] != t[i]) {
        return false;
      }
      if (!trace[i]) {
        break;
      }
    }
    return true;
  }

  int resetReported()
  {
    int count = reported_count;
    if (next) count += next->resetReported();
    reported_count = 0;
    return count;
  }
};

static TraceHashBucket	**traceHash = NULL;     // hash list buckets
static bool				tracerInited = false;   // tracer is inited
static long             traceUsedBuckets = 0;   // count of used buckets in hash list
static long             traceCount = 0;         // count of traces in hash list
static long             traceSkips = 0;         // skips for duplicate keys

void *operator new(size_t size, RTESync_Spinlock *p)
{
  return p;
}

void operator delete(void *b, RTESync_Spinlock *p)
{
}

static void initTracer()
{
#ifdef USE_OWN_SYMLOOKUP
  if (tracerInited) {
    // free all old resources to force reload
    SymCleanup(GetCurrentProcess());
  }
  char spath[4096], *pp;
  spath[0] = 0;
  GetCurrentDirectory(sizeof(spath), spath);
  pp = spath + strlen(spath);
  *(pp++) = ';';
  GetModuleFileName(0, pp, sizeof(spath) - (pp - spath));
  pp += strlen(pp);
  while (*pp != '\\' && *pp != '/' && *pp != ':' && *pp != ';') {
    --pp;
  }
  *pp = 0;
  pp = getenv("LVCSIM_SYMPATH");
  if (pp) {
    strcat(spath, ";");
    strcpy(spath, pp);
  }
  pp = getenv("_NT_SYMBOL_PATH");
  if (pp) {
    strcat(spath, ";");
    strcat(spath, pp);
  }
  pp = getenv("_NT_ALTERNATE_SYMBOL_PATH");
  if (pp) {
    strcat(spath, ";");
    strcat(spath, pp);
  }
  pp = getenv("_NT_ALT_SYMBOL_PATH");
  if (pp) {
    strcat(spath, ";");
    strcat(spath, pp);
  }
  pp = getenv("SYSTEMROOT");
  if (pp) {
    strcat(spath, ";");
    strcat(spath, pp);
  }
  pp = getenv("PATH");
  if (pp) {
    strcat(spath, ";");
    strcat(spath, pp);
  }
  SymSetSearchPath(GetCurrentProcess(), spath);
  SymSetOptions((SymGetOptions() | SYMOPT_LOAD_LINES | SYMOPT_DEFERRED_LOADS) & ~SYMOPT_UNDNAME);
  if (!SymInitialize(GetCurrentProcess(), spath, TRUE))
  {
    // SymInitialize failed
    message("SymInitialize returned error : %d\n", GetLastError());
  }

  SymGetSearchPath(GetCurrentProcess(), spath, sizeof(spath));
  message("Symbol search path is: %s\n", spath);
#endif
  tracerInited = true;
}

static unsigned int getTrace(int skip)
{
  voidPtr traceData[TRACE_SIZE + TRACE_SKIP];
  voidPtr *trace = traceData;
  int level = LVCSim::GetStackTrace(trace, TRACE_SIZE, skip);
  if (level == 0) {
    return 0;
  }

  unsigned int key = 0, level2 = level;
  while (level2 > 0) {
    key = (key >> 30) ^ (key << 1) ^ *((unsigned int*) (trace + --level2));
  }
  key &= 0x7fffffff;
  if (key == 0) key = 1;

  // register trace in hashlist
  LVCSim_TraceLock lck;
  if (!traceHash) {
    traceHash = (TraceHashBucket**) malloc(sizeof(TraceHashBucket*) * TRACE_HASH);
    memset(traceHash, 0, sizeof(TraceHashBucket*) * TRACE_HASH);
  }
  int bucket = key % TRACE_HASH;

  for (;;) {
    TraceHashBucket *b = traceHash[bucket];
    if (!b) {
      traceUsedBuckets++;
    }
    while (b) {
      if (b->key == key) {
        if (b->traceEqual(trace)) {
          return key;
        }
        key += TRACE_HASH ^ *((unsigned int*) (trace + level2++));
        key &= 0x7fffffff;
        if (level2 >= (unsigned int) level) {
          level2 = 0;
        }
        bucket = key % TRACE_HASH;
        traceSkips++;
        break;
      }
      b = b->next;
    }
    if (!b) {
      // no bucket with such key - add new
      TraceHashBucket *p = (TraceHashBucket*) malloc(sizeof(TraceHashBucket));
      p->init(key, trace, traceHash[bucket]);
      traceHash[bucket] = p;
      traceCount++;
      return key;
    }
  }
}

static void printTrace(unsigned int key)
{
  TraceHashBucket *b = traceHash ? traceHash[key % TRACE_HASH] : NULL;
  while (b) {
    if (b->key == key) break;
    b = b->next;
  }
  LVCSim_TraceLock lck;
  if (!b) {
    message("Error: no such traceback key: %d\n", key);
    return;
  }
  if (!tracerInited) {
    initTracer();
  }
  int level = 0;
#ifndef USE_OWN_SYMLOOKUP
  void *context = NULL;
  eo670_GetAddressInfoProlog(&context);
#endif
  while (level < TRACE_SIZE && b->trace[level]) {
    void *ptr = b->trace[level++];
#ifdef USE_OWN_SYMLOOKUP
    DWORD disp;
    BYTE buffer[512];
    PIMAGEHLP_SYMBOL pSymbol = (PIMAGEHLP_SYMBOL )buffer;
    pSymbol->SizeOfStruct = sizeof(PIMAGEHLP_SYMBOL);
    pSymbol->MaxNameLength = sizeof(buffer) - sizeof(SYMBOL_INFO) - 1;

    if (SymGetSymFromAddr(GetCurrentProcess(), (DWORD) ptr, &disp, pSymbol)) {
      IMAGEHLP_LINE line;
      line.SizeOfStruct = sizeof(IMAGEHLP_LINE);
      if (SymGetLineFromAddr(GetCurrentProcess(), (DWORD) ptr, &disp, &line)) {
        message(" - 0x%08p: %s+%d [%s:%d]\n", ptr, pSymbol->Name, 
          (DWORD) ptr - line.Address, line.FileName, line.LineNumber);
      } else {
        message(" - 0x%08p: %s+%d\n", ptr, pSymbol->Name, disp);
      }
    } else {
      message(" - 0x%08p: <unknown>\n", ptr);
    }
#else
    eo670AddressInfo aInfo;
    if (eo670_GetAddressInfo(ptr, &aInfo, context)) {
      message(" - 0x%08p: %s+%d %s[%s:%d]\n", ptr, (aInfo.demangledSymbol && aInfo.demangledSymbol[0]) ?
        aInfo.demangledSymbol : (aInfo.symbol ? aInfo.symbol : "unknown"), aInfo.offsetToSymbol,
        aInfo.moduleName ? aInfo.moduleName : "unknown", aInfo.sourceFileName ? 
        aInfo.sourceFileName : "unknown", aInfo.lineNumber);
      eo670_FreeAddressInfo(&aInfo);
    } else {
      message(" - 0x%08p: <unknown>\n", ptr);
    }
#endif
  }
#ifndef USE_OWN_SYMLOOKUP
  eo670_GetAddressInfoEpilog(context);
#endif
}

static int incTraceReport(unsigned int key)
{
  TraceHashBucket *b = traceHash ? traceHash[key % TRACE_HASH] : NULL;
  while (b) {
    if (b->key == key) break;
    b = b->next;
  }
  if (!b) {
    LVCSim_TraceLock lck;
    message("Error: no such traceback key: %d\n", key);
    return 0;
  }
  return b->reported_count++;
}

static int resetTraceReport()
{
  int count = 0;
  if (traceHash) {
    for (int i = 0; i < TRACE_HASH; ++i) {
      if (traceHash[i]) {
        count += traceHash[i]->resetReported();
      }
    }
  }
  return count;
}

class LVCSim_AllocHeader {
public:
  LVCSim_AllocHeader	*prev;				// previous block
  LVCSim_AllocHeader	*next;				// next block
  unsigned int		allocTrKey;			// alloc stacktrace key
  unsigned int		deallocTrKey;		// dealloc stacktrace key
  SAPDB_ULong			blksize;			// real allocated block size
  char				flags;			    // bit 0: array flag, bit 1: no leak flag
  unsigned char		fence[FENCECNT];	// pre write fence

  // data follow
};

class LVCSim_AllocFreelist {
public:
  LVCSim_AllocHeader	*first;				// first allocated block
  LVCSim_AllocHeader	*last;				// last allocated block
  LVCSim_AllocHeader	*free, *free_last;	// first and last free block

  SAPDB_ULong			blksize;			// maximum block size
  SAPDB_ULong         freeCounter;
  int                 leakCounter;        // leak prevention counter

  RTESync_Spinlock	lock;

  LVCSim_AllocFreelist() : first(NULL), last(NULL), free(NULL), 
    free_last(NULL), blksize(0), freeCounter(0), leakCounter(0)
  {
  }

  void setSize(SAPDB_ULong size) { blksize = size; }

  void checkRead(const void *ptr, const void *blk);
  bool checkReadNoErr(void *ptr, void *blk);
  bool checkFreeBlock(LVCSim_AllocHeader *blk);	// check block before free; unlink
  bool checkChainedBlock(LVCSim_AllocHeader *blk);// check block before alloc

  bool checkFences(LVCSim_AllocHeader *blk);
  bool checkLeaks(bool leaks);
  bool escape(LVCSim_DebugAllocatorEscapeEnum what); // mark all memory leaks as OK, etc.

  void registerAlloc(LVCSim_AllocHeader *newblk);	// register malloc()ed block
  void unregisterAlloc(LVCSim_AllocHeader *blk);	// unregister malloc()ed block

  void *allocate(SAPDB_ULong size, bool isArray);	// allocate new block
  void deallocate(LVCSim_AllocHeader *hdr);		// deallocate a block

  static void error(const LVCSim_AllocHeader *blk, const char *msg, ...)
  {
    char buf[4096];
    va_list arglist;
    va_start(arglist, msg);
    sp77vsprintf(buf, sizeof(buf), msg, arglist);
    va_end(arglist);

    ++dumpTraceMessages;
    if (blk) {
      {
        LVCSim_TraceLock lck;
        message("#########################################################################\n");
        message("HEAP ERR [0x%08p]: %s\n", blk, buf);
      }
      if (blk->allocTrKey) {
        {
          LVCSim_TraceLock lck;
          message("Trying to dump allocating stack trace for block 0x%08p:\n", blk + 1);
        }
        printTrace(blk->allocTrKey);
      }
      if (blk->deallocTrKey) {
        {
          LVCSim_TraceLock lck;
          message("-------------------------------------------------------------------------\n");
          message("Trying to dump deallocating stack trace for block 0x%08p:\n", blk + 1);
        }
        printTrace(blk->deallocTrKey);
      }
      LVCSim_TraceLock lck;
      message("-------------------------------------------------------------------------\n");
    } else {
      LVCSim_TraceLock lck;
      message("#########################################################################\n");
      message("HEAP ERR: %s\n", buf);
      message("-------------------------------------------------------------------------\n");
    }
    --dumpTraceMessages;
#ifdef WIN32
    _CrtDbgBreak();
#endif
  }
};


void LVCSim_AllocFreelist::checkRead(const void *ptr, const void *blk)
{
  try {
    int data = *((const int*) ptr);
  } catch (...) {
    error(NULL, "Cannot read 0x%08p, block: 0x%08p", ptr, blk);
  }
}

bool LVCSim_AllocFreelist::checkReadNoErr(void *ptr, void *blk)
{
  bool res = true;
  try {
    int data = *((int*) ptr);
  } catch (...) {
    error(NULL, "Cannot read 0x%08p, block: 0x%08p", ptr, blk);
    res = false;
  }
  return res;
}

bool LVCSim_AllocFreelist::checkFreeBlock(LVCSim_AllocHeader *blk)
{
  // check fences
  unsigned char *p = blk->fence;
  bool err = false;
  if (*p == EMPTYCHAR) {
    error(blk, "Empty char in pre-fence, double-freed block 0x%08p?", blk + 1);
    err = true;
  }
  SAPDB_ULong cnt = FENCECNT, i;
  for (i = 0; i < FENCECNT; ++i) {
    if (*p != FENCECHAR) {
      error(blk, "Pre-fence in block 0x%08p damaged at 0x%08p", blk + 1, p);
      err = true;
    }
    ++p;
  }
  p = reinterpret_cast<unsigned char*>(blk + 1) + blk->blksize;
  if (blksize > blk->blksize) cnt += blksize - blk->blksize;
  for (i = 0; i < cnt; ++i) {
    if (*p != FENCECHAR) {
      error(blk, "Post-fence in block 0x%08p damaged at 0x%08p or size invalid (%lu)", 
        blk + 1, p, blk->blksize);
      err = true;
    }
    ++p;
  }

  // unlink it from allocated list
  blk->deallocTrKey = getTrace(TRACE_SKIP);
  if (blk->prev) {
    checkRead(blk->prev, blk);
    if (blk->prev->next != blk) {
      error(blk, "Prev block 0x%08p next 0x%08p does not match block 0x%08p", 
        blk->prev, blk->prev->next, blk + 1);
      err = true;
    }
    blk->prev->next = blk->next;
  } else {
    if (first != blk) {
      error(blk, "First block 0x%08p does not match first pointer 0x%08p", blk + 1, first);
      err = true;
    }
    first = blk->next;
  }
  if (blk->next) {
    checkRead(blk->next, blk);
    if (blk->next->prev != blk) {
      error(blk, "Next block 0x%08p prev 0x%08p does not match block 0x%08p", 
        blk->next, blk->next->prev, blk + 1);
      err = true;
    }
    blk->next->prev = blk->prev;
  } else {
    if (last != blk) {
      error(blk, "Last block 0x%08p does not match last pointer 0x%08p", 
        blk + 1, last);
      err = true;
    }
    last = blk->prev;
  }
  return err;
}

bool LVCSim_AllocFreelist::checkFences(LVCSim_AllocHeader *blk)
{
  unsigned char *p = blk->fence;
  SAPDB_ULong cnt = FENCECNT, i;
  bool err = false;
  for (i = 0; i < FENCECNT; ++i) {
    if (*p != FENCECHAR) {
      error(blk, "Pre-fence in block 0x%08p damaged at 0x%08p", blk + 1, p);
      err = true;
    }
    ++p;
  }
  p = reinterpret_cast<unsigned char*>(blk + 1) + blk->blksize;
  if (blksize > blk->blksize) cnt += blksize - blk->blksize;
  for (i = 0; i < cnt; ++i) {
    if (*p != FENCECHAR) {
      error(blk, "Post-fence in block 0x%08p damaged at 0x%08p or size invalid (%lu)", 
        blk + 1, p, blk->blksize);
      err = true;
    }
    ++p;
  }
  return err;
}

bool LVCSim_AllocFreelist::checkLeaks(bool leaks)
{
  // dump memory leaks (allocated blocks)
  LVCSim_AllocHeader *blk = first;
  int displayed = 0;
  bool err = false;
  if (leaks)
    while (blk) {
      if (!checkReadNoErr(blk, blk)) {
        error(NULL, "cannot read block at 0x%08p", blk);
        err = true;
        break;
      }
      if (checkFences(blk)) err = true;
      if (!(blk->flags & 2)) {
        if (incTraceReport(blk->allocTrKey) == 0) {
          {
            LVCSim_TraceLock lck;
            message("-------------------------------------------------------------------------\n");
            message("Trying to dump allocating stack trace for leaked %dB "
              "block 0x%08p:\n", blk->blksize, blk + 1);
          }
          printTrace(blk->allocTrKey);
          ++displayed;
          err = true;
        }
      }
      blk = blk->next;
    }
    int total = resetTraceReport();
    if (total != displayed) {
      LVCSim_TraceLock lck;
      message("-------------------------------------------------------------------------\n");
      message("Displayed %d of %d warnings for block size %d\n", 
        displayed, total, blksize);
    }

    // check free blocks for overwrites - simulate allocate & deallocate from freelist
    SAPDB_ULong cnt = freeCounter;
    blk = free;
    while (blk && cnt--) {
      if (!checkReadNoErr(blk, blk)) {
        error(NULL, "cannot read block at 0x%08p", blk);
        err = true;
        break;
      }
      if (checkChainedBlock(blk)) err = true;
      blk = blk->next;
    }
    if (cnt != 0 || blk) {
      error(NULL, "invalid block count in free chain");
      err = true;
    }
    return err;
}

bool LVCSim_AllocFreelist::escape(LVCSim_DebugAllocatorEscapeEnum what)
{
  // dump memory leaks (allocated blocks)
  switch (what) {
  case LVCSIM_DA_CHECK_LEAKS:
    return checkLeaks(true);
  case LVCSIM_DA_CHECK_MEMORY:
    return checkLeaks(false);
  case LVCSIM_DA_ENTER_NOLEAK:
    if (leakCounter++ > 0) return false;
    break;
  case LVCSIM_DA_EXIT_NOLEAK:
    if (--leakCounter > 0) return false;
    break;
  case LVCSIM_DA_MESSAGES_ON:
    ++dumpTraceMessages;
    return false;
  case LVCSIM_DA_MESSAGES_OFF:
    --dumpTraceMessages;
    return false;
  }
  LVCSim_AllocHeader *blk = first;
  while (blk) {
    if (!checkReadNoErr(blk, blk)) {
      error(NULL, "cannot read block at 0x%08p", blk);
      break;
    }
    switch (what) {
    case LVCSIM_DA_CLEAR_LEAKS:
      blk->flags |= 2;
      blk = blk->next;
      break;
    case LVCSIM_DA_ENTER_NOLEAK:
      blk->flags |= 4;
      blk = blk->next;
      break;
    case LVCSIM_DA_EXIT_NOLEAK:
      if (blk->flags & 4) {
        blk->flags &= ~4;
      } else {
        blk->flags |= 2;
      }
      blk = blk->next;
      break;
    }
  }
  return false;
}

void LVCSim_AllocFreelist::registerAlloc(LVCSim_AllocHeader *newblk)
{
  // set up the pointers in new malloc()ed block
  memset(newblk->fence, FENCECHAR, FENCECNT);
  memset(newblk + 1, EMPTYCHAR, newblk->blksize);
  int cnt = FENCECNT;
  if (blksize > newblk->blksize) cnt += blksize - newblk->blksize;
  memset(reinterpret_cast<char*>(newblk + 1) + newblk->blksize, FENCECHAR, cnt);
  newblk->prev = NULL;
  newblk->next = first;
  newblk->allocTrKey = getTrace(TRACE_SKIP);
  if (first) {
    checkRead(first, NULL);
    if (first->prev != NULL) {
      error(first->prev, "first->prev != NULL (0x%08p)", first->prev);
    }
    first->prev = newblk;
  } else {
    if (last != NULL) {
      error(last, "last != NULL in empty (0x%08p)", last);
    }
    last = newblk;
  }
  first = newblk;
}

void LVCSim_AllocFreelist::unregisterAlloc(LVCSim_AllocHeader *blk)
{
  checkFreeBlock(blk);
}

bool LVCSim_AllocFreelist::checkChainedBlock(LVCSim_AllocHeader *blk)
{
  bool err = false;
  if (blk->blksize != blksize) {
    error(blk, "Invalid size %lu (should be %lu) in free block 0x%08p", 
      blk->blksize, blksize, blk + 1);
    err = true;
  }

  // check if the block is filled with emptychar
  unsigned char *p = blk->fence;
  for (SAPDB_ULong i = 0; i < blksize + 2 * FENCECNT; ++i) {
    if (*p != EMPTYCHAR) {
      error(blk, "Freed block 0x%08p damaged at 0x%08p", blk + 1, p);
      err = true;
    }
    ++p;
  }
  return err;
}

void *LVCSim_AllocFreelist::allocate(SAPDB_ULong size, bool isArray)
{
  LVCSim_AllocHeader *blk;

  if (free && freeCounter > MIN_FREE_COUNT) {
    // get block from freelist
    checkRead(free, NULL);
    blk = free;
    free = blk->next;
    if (free) {
      free->prev = NULL;
    } else {
      free_last = NULL;
    }
    --freeCounter;

    // check header
    if (blk->prev != NULL) {
      error(blk, "Invalid prev in freelist: 0x%08p, block: 0x%08p", blk->prev, blk + 1);
    }
    checkChainedBlock(blk);
  } else {
    // allocate new block
    blk = reinterpret_cast<LVCSim_AllocHeader*>(malloc(blksize + 
      sizeof(LVCSim_AllocHeader) + FENCECNT));
  }

  blk->blksize = size;
  blk->flags = isArray ? 1 : 0;
  registerAlloc(blk);

  return blk + 1;
}

void LVCSim_AllocFreelist::deallocate(LVCSim_AllocHeader *blk)
{
  checkFreeBlock(blk);

  // add to free chain
  memset(blk->fence, EMPTYCHAR, blksize + 2 * FENCECNT);
  blk->blksize = blksize;
  blk->next = NULL;
  blk->prev = free_last;
  if (free_last) {
    free_last->next = blk;
  } else {
    free = blk;
  }
  free_last = blk;
  ++freeCounter;
}

/*!
 * \brief Debug allocator implementation.
 *
 * \ingroup    lvcsimulator
 */
class LVCSim_DebugAllocatorImp : public LVCSim_DebugAllocator 
{
public:
  LVCSim_DebugAllocatorImp();
  ~LVCSim_DebugAllocatorImp();

  /*!
   * \brief Allocates contiguous memory for \c ByteCount bytes.
   *
   * The memory is aligned as indicated by the 'GetAlignmentSize' method.
   * The memory is not initialised.
   *
   * \param ByteCount The size in bytes of the memory to be allocated. This 
   *                  argument must be greater zero.
   * \param Hint      If possible, the allocator tries to reserve the memory
   *                  at the same position as given by \c Hint. This is useful
   *                  for resizing a memory block. If the reallocation was
   *                  succesfull, the return value and \c Hint are identical.
   *
   * \return A pointer to the allocated raw memory block of \c ByteCount
   *    bytes. If exception handling is disabled, a NULL pointer will be returned
   *    in case of insufficient memory.
   * \throw SAPDBMem_BadAllocException may be thrown in classes derived from
   *    SAPDBMem_IRawAllocator if the allocation fails.
   */
  virtual void* Allocate(SAPDB_ULong ByteCount, const void* Hint);

  /*!
   * \brief Allocates contiguous memory for \c ByteCount bytes.
   *
   * The memory is aligned as indicated by the 'GetAlignmentSize' method.
   * The memory is not initialised.
   *
   * \param ByteCount The size in bytes of the memory to be allocated. This 
   *                  argument must be greater zero.
   *
   * \return A pointer to the allocated raw memory block of \c ByteCount
   *    bytes. If exception handling is disabled, a NULL pointer will be returned
   *    in case of insufficient memory.
   * \throw SAPDBMem_BadAllocException may be thrown in classes derived from
   *    SAPDBMem_IRawAllocator if the allocation fails.
   */
  virtual void* Allocate(SAPDB_ULong ByteCount);

  /*!
   * \brief Frees the memory.
   * 
   * The memory must have been allocated by a previous call to Allocate. 
   * This is checked by assertions.
   *
   * \note An implementation of this method may not throw an exception.
   *
   * \param p block to free.
   */
  virtual void Deallocate(void* p);

  virtual void* AllocateArray(SAPDB_ULong ByteCount);
  virtual void DeallocateArray(void* p);

  virtual SAPDB_ULong GetChunkSize(const void* p);

  virtual bool doEscape(LVCSim_DebugAllocatorEscapeEnum what);
  virtual void doBlockError(const char *msg, const void *blk);

  virtual void GetBaseAllocatorCallStatistics(SAPDB_ULong &CountAlloc,
    SAPDB_ULong &CountDealloc) const
  {
    GetCallStatistics(CountAlloc, CountDealloc);
  }

  virtual void GetCallStatistics(SAPDB_ULong &CountAlloc,
    SAPDB_ULong &CountDealloc) const
  {
    CountAlloc = m_alloccnt;
    CountDealloc = m_dealloccnt;
  }

  void* AllocatePrim(SAPDB_ULong ByteCount, bool isArray);
  void DeallocatePrim(void* p, bool isArray);

  SAPDB_ULong	m_alloccnt;
  SAPDB_ULong	m_dealloccnt;

  LVCSim_AllocFreelist	bigfreelist;
  LVCSim_AllocFreelist	freelist[FL_COUNT];
};


static double debug_allocator_space[sizeof(LVCSim_DebugAllocatorImp) / sizeof(double) + 1];


LVCSim_DebugAllocator &LVCSim_DebugAllocator::Instance()
{
  if (m_instance == NULL) 
    m_instance = new(debug_allocator_space) LVCSim_DebugAllocatorImp();
  return *m_instance;
}


LVCSim_DebugAllocatorImp::LVCSim_DebugAllocatorImp()
: m_alloccnt(0), m_dealloccnt(0)
{
  SAPDB_ULong size = MIN_FL;
  for (int i = 0; i < FL_COUNT; ++i) {
    freelist[i].setSize(size);
    size <<= 1;
  }
}

LVCSim_DebugAllocatorImp::~LVCSim_DebugAllocatorImp()
{
}



SAPDB_ULong LVCSim_DebugAllocatorImp::GetChunkSize(const void* p)
{
  const LVCSim_AllocHeader *tmp = reinterpret_cast<const LVCSim_AllocHeader*>(p) - 1;
  return tmp->blksize & ~0xe0000000;
}

void* LVCSim_DebugAllocatorImp::AllocatePrim(SAPDB_ULong ByteCount, bool isArray)
{
  if (__debug_lvcsim_throw_badalloc) {
    //throw std::bad_alloc;
    THROW_BAD_ALLOC_GEO573;
  }

  if (ByteCount == 0) return NULL;

  ++m_alloccnt;
  if (ByteCount > MAX_FL) {
    // allocate using malloc()
    char *data = (char*) malloc(sizeof(LVCSim_AllocHeader) + FENCECNT + ByteCount);
    LVCSim_AllocHeader *tmp = reinterpret_cast<LVCSim_AllocHeader*>(data);
    tmp->blksize = ByteCount;
    tmp->flags = isArray ? 1 : 0;
    RTESync_LockedScope lck(bigfreelist.lock);
    bigfreelist.registerAlloc(tmp);
    return tmp + 1;
  } else {
    // allocate from freelist
    SAPDB_ULong space = MIN_FL, index = 0;
    while (ByteCount > space) {
      space <<= 1;
      ++index;
    }
    RTESync_LockedScope lck(freelist[index].lock);
    return freelist[index].allocate(ByteCount, isArray);
  }
}

void LVCSim_DebugAllocatorImp::DeallocatePrim(void* p, bool isArray)
{
  if (!p) return;

  bigfreelist.checkRead(p, p);
  LVCSim_AllocHeader *tmp = reinterpret_cast<LVCSim_AllocHeader*>(p) - 1;
  if (((tmp->flags & 1) && !isArray) || (!(tmp->flags & 1) && isArray)) {
    bigfreelist.error(tmp, "Array mismatch: block 0x%08p: %s, delete: %s", p,
      (tmp->flags & 1) ? "array" : "scalar", isArray ? "array" : "scalar");
  }

  if (tmp->blksize > MAX_FL) {
    // deallocate using free()
    RTESync_LockedScope lck(bigfreelist.lock);
    bigfreelist.unregisterAlloc(tmp);
    free(tmp);
  } else {
    // deallocate into freelist
    SAPDB_ULong space = MIN_FL, index = 0;
    while (tmp->blksize > space) {
      space <<= 1;
      ++index;
    }
    RTESync_LockedScope lck(freelist[index].lock);
    freelist[index].deallocate(tmp);
  }
  ++m_dealloccnt;
}

void* LVCSim_DebugAllocatorImp::Allocate(SAPDB_ULong ByteCount)
{
  return AllocatePrim(ByteCount, false);
}

void* LVCSim_DebugAllocatorImp::Allocate(SAPDB_ULong ByteCount, const void* Hint)
{
  return AllocatePrim(ByteCount, false);
}

void LVCSim_DebugAllocatorImp::Deallocate(void* p)
{
  DeallocatePrim(p, false);
}

void* LVCSim_DebugAllocatorImp::AllocateArray(SAPDB_ULong ByteCount)
{
  return AllocatePrim(ByteCount, true);
}

void LVCSim_DebugAllocatorImp::DeallocateArray(void* p)
{
  DeallocatePrim(p, true);
}

bool LVCSim_DebugAllocatorImp::doEscape(LVCSim_DebugAllocatorEscapeEnum what)
{
  bool err = false;
  for (int i = 0; i < FL_COUNT; ++i) {
    RTESync_LockedScope lck(freelist[i].lock);
    if (freelist[i].escape(what)) err = true;
  }
  RTESync_LockedScope lck(bigfreelist.lock);
  if (bigfreelist.escape(what)) err = true;
  return err;
}

void LVCSim_DebugAllocatorImp::doBlockError(const char *msg, const void *blk)
{
  if (dumpAllocMessages) {
    bigfreelist.checkRead(blk, blk);
    const LVCSim_AllocHeader *tmp = reinterpret_cast<const LVCSim_AllocHeader*>(blk) - 1;
    LVCSim_AllocFreelist::error(tmp, "Allocator Error: %s", msg);
  }
}

bool LVCSim_DebugAllocator::Escape(LVCSim_DebugAllocatorEscapeEnum what)
{
  switch (what) {
  case LVCSIM_DA_ALLOCERR_ON:
    ++dumpAllocMessages;
    break;
  case LVCSIM_DA_ALLOCERR_OFF:
    --dumpAllocMessages;
    break;
  case LVCSIM_DA_SYMREFRESH:
    initTracer();
    break;
  case LVCSIM_DA_STACKTRACE:
    // dump stack trace
    {
      LVCSim_TraceLock lck;
      message("User-triggered stack trace dump\n");
    }
    printTrace(getTrace(0));
    break;
  default:
    return Instance().doEscape(what);
  }
  return false;
}

#endif  // old stuff
