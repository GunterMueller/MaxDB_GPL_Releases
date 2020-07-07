/*!
 * \file    MemDbg_Callback.cpp
 * \author  IvanS
 * \brief   Memory debugger callback.
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

#include "MemoryDebugger/MemDbg_Callback.hpp"
#include "MemoryDebugger/MemDbg_Interface.hpp"

#include <string.h>
#include <stdlib.h>

#if defined( SUN ) || defined( SOLARIS )
#define TRACE_SKIP  2       // skip levels
#elif !defined( WIN32 )
#define TRACE_SKIP  3       // skip levels
#else
#define TRACE_SKIP  4       // skip levels
#endif

#define	TRACE_SIZE	30		// levels of backtrace
#define	TRACE_HASH	65536	// default # of hashlist headers & hash items
#define ITEM_STEP   65536 // how many items to allocate in one step
#define ITEM_HEADER 16384 // how many item block headers to allocate

typedef void  *voidPtr;

/*-----------------------------------------------------*/

/// Helper class - trace hash bucket.
class TraceHashBucket {
public:
  TraceHashBucket	*next;
  unsigned int    id;
  unsigned int  	key;
  int             reported_count;
  voidPtr		  	  trace[TRACE_SIZE];

  void init(unsigned int i, unsigned int k, voidPtr *t, TraceHashBucket *n)
  {
    id = i;
    next = n;
    key = k;
    reported_count = 0;
    memcpy(&trace[0], t, sizeof(trace));
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
    if (next) {
      count += next->resetReported();
    }
    reported_count = 0;
    return count;
  }
};

/*-----------------------------------------------------*/

// internal trace hash variables
static TraceHashBucket **traceHash = NULL;    // hash list buckets
static TraceHashBucket *traceItems = NULL;    // trace items
static TraceHashBucket **traceItemHdr = NULL; // trace item block headers
static size_t hashSize, hashLimit, hashMask;  // size, limit and mask
static size_t itemCount, itemPtr, itemSize;   // items in traceItems

/*-----------------------------------------------------*/

void *MemDbg_Callback::mdSystemAllocate(size_t size)
{
  return malloc(size);
}

/*-----------------------------------------------------*/

void MemDbg_Callback::mdSystemFree(void *ptr)
{
  return free(ptr);
}

/*-----------------------------------------------------*/

size_t MemDbg_Callback::implFillTrace(void **traceptr, size_t levels)
{
#if defined(WIN32) && !defined(_WIN64)
  register voidPtr *traceStart;
  __asm {
    mov traceStart, ebp
  }
  register voidPtr *lastLevel = (voidPtr*) 0xffffffff;
  register voidPtr *stop = traceptr + levels;
  register skip = TRACE_SKIP;
  while (traceptr < stop && traceStart) {
    register voidPtr *newLevel = (voidPtr*) traceStart[1];
    /*if (newLevel >= lastLevel) {
      // error in stack
      break;
    }*/
    lastLevel = newLevel;
    if (--skip <= 0) {
      // skipped enough levels
      *traceptr = lastLevel;
      ++traceptr;
    }
    traceStart = (voidPtr*) *traceStart;
  }
  return levels - (stop - traceptr);
#else   // WIN32
  return 0;
#endif  // !WIN32
}

/*-----------------------------------------------------*/

int MemDbg_Callback::implGetTraceID()
{
  voidPtr trace[TRACE_SIZE];
  memset(trace, 0, sizeof(trace));
  size_t level = mdFillTrace(trace, TRACE_SIZE);

  ptrdiff_t key = 0;
  while (level > 0) {
    key = (key >> 30) ^ (key << 1) ^ (reinterpret_cast<ptrdiff_t>(trace[--level]));
  }
  key &= 0x7fffffff;
  if (key == 0) key = 1;

  // register trace in hashlist
  MemDbg_LockedScope lck(MemDbg_Interface::m_lock);
  if (!traceHash) {
    traceHash = (TraceHashBucket**) mdSystemAllocate(sizeof(TraceHashBucket*) * TRACE_HASH);
    if (traceHash == NULL) {
      mdAbort("Cannot allocate memory for stack trace hash");
    }
    memset(traceHash, 0, sizeof(TraceHashBucket*) * TRACE_HASH);
    hashSize = TRACE_HASH;
    hashLimit = (TRACE_HASH * 7) / 10;
    hashMask = hashSize - 1;
    itemCount = 0;
    itemPtr = 0;
    itemSize = ITEM_STEP;

    // allocate item headers
    traceItemHdr = (TraceHashBucket**) mdSystemAllocate(sizeof(TraceHashBucket*) * ITEM_HEADER);
    if (traceItemHdr == NULL) {
      mdAbort("Cannot allocate memory for stack trace item headers");
    }
    memset(traceItemHdr, 0, sizeof(TraceHashBucket*) * ITEM_HEADER);

    // allocate current set
    traceItems = (TraceHashBucket*) mdSystemAllocate(sizeof(TraceHashBucket) * ITEM_STEP);
    if (traceItems == NULL) {
      mdAbort("Cannot allocate memory for stack trace items");
    }
    memset(traceItems, 0, sizeof(TraceHashBucket) * ITEM_STEP);
    traceItemHdr[0] = traceItems;
  }
  int bucket = key & hashMask;

  for (;;) {
    TraceHashBucket *b = traceHash[bucket];
    while (b) {
      if (b->key == key) {
        if (b->traceEqual(trace)) {
          return b->id;
        }
        key += TRACE_HASH;
        break;
      }
      b = b->next;
    }
    if (!b) {
      // no bucket with such key - add new
      if (hashSize >= hashLimit) {
        // try to allocate bigger hash
      }
      if (itemCount == itemSize) {
        // must allocate next item block
        size_t index = itemCount / ITEM_STEP;
        if (index == ITEM_HEADER) {
          mdAbort("Limit of possible stack traces overflow. Increase ITEM_HEADER or ITEM_STEP.");
        }
        traceItems = (TraceHashBucket*) mdSystemAllocate(sizeof(TraceHashBucket) * ITEM_STEP);
        if (traceItems == NULL) {
          mdAbort("Cannot allocate memory for stack trace items");
        }
        memset(traceItems, 0, sizeof(TraceHashBucket) * ITEM_STEP);
        traceItemHdr[index] = traceItems;
        itemPtr = 0;
        itemSize += ITEM_STEP;
      }
      TraceHashBucket *p = traceItems + (itemPtr++);
      ++itemCount;
      p->init(itemCount, key, trace, traceHash[bucket]);
      traceHash[bucket] = p;
      return itemCount;
    }
  }
}

/*-----------------------------------------------------*/

size_t MemDbg_Callback::implGetTrace(int id, void **data, size_t dataSize)
{
  memset(data, 0, sizeof(void*) * dataSize);
  if (id < 1 || ((size_t) id) > itemCount) {
    // out of range
    return 0;
  }

  --id;
  size_t index = id / ITEM_STEP;
  size_t ptr = id % ITEM_STEP;
  TraceHashBucket &p = traceItemHdr[index][ptr];
  for (int i = 0; i < TRACE_SIZE; ++i) {
    if (i == dataSize) {
      return dataSize;
    }
    if (p.trace[i] != NULL) {
      data[i] = p.trace[i];
    } else {
      return i;
    }
  }
  return TRACE_SIZE;
}

/*-----------------------------------------------------*/
