/*!
\file       LVCSim_DebugCallback.cpp
\brief      Memory debugger callback.
\author     IvanS
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

#include "LVCSimulator/LVCSim_DebugCallback.hpp"
#include "MemoryDebugger/MemDbg_Interface.hpp"
#include "RunTime/Synchronisation/RTESync_Spinlock.hpp"
#include "heo670.h"
#include "hsp77.h"
#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#ifdef WIN32
#include <io.h>
#define write _write
#define open _open
#define O_WRONLY _O_WRONLY
#define O_CREAT _O_CREAT
#define O_TRUNC _O_TRUNC
#else
#include <sys/types.h>
#include <sys/stat.h>
#endif


/// Trace file descriptor.
static int traceFile = 1;

/*-----------------------------------------------------*/

static void writeMsg(const char *msg, int traceFile)
{
  write(traceFile, msg, strlen(msg));
}

/*-----------------------------------------------------*/

class LVCSim_DebugLock : public MemDbg_Lock, private RTESync_Spinlock {
public:
  LVCSim_DebugLock()
  {
  }

  ~LVCSim_DebugLock()
  {
  }

  virtual void lock()
  {
    Lock();
  }

  virtual void unlock()
  {
    Unlock();
  }

  void *operator new(size_t sz, void *p)
  {
    return p;
  }

  void operator delete(void *ptr, void *p)
  {
  }
};

/*-----------------------------------------------------*/

/// Default instance.
LVCSim_DebugCallback *LVCSim_DebugCallback::m_instance;
/// Error flag.
bool LVCSim_DebugCallback::m_hasErrors = false;

static double debug_callback_space[sizeof(LVCSim_DebugCallback) / sizeof(double) + 1];

LVCSim_DebugCallback &LVCSim_DebugCallback::getInstance()
{
  if (m_instance == NULL) {
    m_instance = new(debug_callback_space) LVCSim_DebugCallback();
  }
  return *m_instance;
}

/*-----------------------------------------------------*/

size_t LVCSim_DebugCallback::mdFillTrace(void **traceptr, size_t levels)
{
/*#if (defined(WIN32) && !defined(_WIN64))
  // for Win32, we have tracing directly in assembly (fast)
  return MemDbg_Callback::mdFillTrace(traceptr, levels);
#else*/
  // use standard tracing on other systems
  return eo670_FillCallArray(levels, traceptr);
//#endif
}

/*-----------------------------------------------------*/

void LVCSim_DebugCallback::dumpTrace(int id)
{
  void* trace[64];  // TODO: configurable?
  char buf[256];
  size_t count = mdGetTrace(id, trace, 64);
  if (count == 0) {
    return;
  }

  void *context = NULL;
  eo670_GetAddressInfoProlog(&context);
  bool checkLevels = true;
  for (size_t i = 0; i < count; ++i) {
    eo670AddressInfo aInfo;
    void *ptr = trace[i];
    if (eo670_GetAddressInfo(ptr, &aInfo, context)) {
      if (checkLevels) {
        const char *sym = aInfo.demangledSymbol;
        if (!sym || !*sym) {
          sym = aInfo.symbol;
          if (!sym) {
            sym = "unknown";
          }
        }
        if (strstr(sym, "LVCSim_DebugCallback") ||
          strstr(sym, "MemDbg_Callback") ||
          strstr(sym, "MemDbg_Interface") ||
          strstr(sym, "LVCSim_DebugAllocatorImp")) {
          // filter out
          continue;
        } else {
          checkLevels = false;
        }
      }
      sp77sprintf(buf, sizeof(buf),
        " - 0x%08p: %s+%d %s[%s:%d]\n", ptr, (aInfo.demangledSymbol && aInfo.demangledSymbol[0]) ?
        aInfo.demangledSymbol : (aInfo.symbol ? aInfo.symbol : "unknown"), aInfo.offsetToSymbol,
        aInfo.moduleName ? aInfo.moduleName : "unknown", aInfo.sourceFileName ? 
        aInfo.sourceFileName : "unknown", aInfo.lineNumber);
      eo670_FreeAddressInfo(&aInfo);
    } else {
      sp77sprintf(buf, sizeof(buf), " - 0x%08p: <unknown>\n", ptr);
    }
    write(traceFile, buf, strlen(buf));
  }
  eo670_GetAddressInfoEpilog(context);
}

/*-----------------------------------------------------*/

void LVCSim_DebugCallback::mdAllocatorMessage(const char *msg, MemDbg_Interface *allocator, const void *blk,
    size_t size, int allocTrace, int freeTrace, int currentTrace)
{
  m_hasErrors = true;
  if (traceFile == 1) {
    // TODO: make configurable
    int fd = open("mem_trace.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd > 0) {
      traceFile = fd;
    }
  }

  char buf[1024];
  size_t len = 0;
  len = sp77sprintf(buf, sizeof(buf), "%s", msg);
  if (allocator != NULL) {
    len += sp77sprintf(buf + len, sizeof(buf) - len, ", alloc %s (%p)",
      allocator->mdGetName(), allocator);
  }
  if (blk != NULL) {
    len += sp77sprintf(buf + len, sizeof(buf) - len, ", block %p(%dB)", blk, size);
  }
  writeMsg("#########################################################################\n", traceFile);
  write(traceFile, buf, len);
  write(traceFile, "\n", strlen("\n"));
  bool first = true;
  if (allocTrace) {
    writeMsg("Allocation trace:\n", traceFile);
    dumpTrace(allocTrace);
    first = false;
  }
  if (freeTrace) {
    if (first) {
      first = false;
    } else {
      writeMsg("-------------------------------------------------------------------------\n", traceFile);
    }
    writeMsg("Deallocation trace:\n", traceFile);
    dumpTrace(freeTrace);
  }
  if (currentTrace) {
    if (first) {
      first = false;
    } else {
      writeMsg("-------------------------------------------------------------------------\n", traceFile);
    }
    writeMsg("Current stack trace:\n", traceFile);
    dumpTrace(currentTrace);
  }
}

/*-----------------------------------------------------*/

void LVCSim_DebugCallback::mdAbort(const char *msg)
{
  mdAllocatorMessage(msg, NULL, NULL, 0, 0, 0, mdGetTraceID());
  abort();
}

/*-----------------------------------------------------*/

void LVCSim_DebugCallback::mdAbstractCall(const void *blk, const void *vtable, size_t size, int allocTrace, int freeTrace)
{
  char buf[64];
  sp77sprintf(buf, sizeof(buf), "Abstract call, VTable %p", vtable);
  mdAllocatorMessage(buf, NULL, blk, size, allocTrace, freeTrace, mdGetTraceID());
}

/*-----------------------------------------------------*/

MemDbg_Lock *LVCSim_DebugCallback::mdCreateLock()
{
  LVCSim_DebugLock *lock = reinterpret_cast<LVCSim_DebugLock*>(
    mdSystemAllocate(sizeof(LVCSim_DebugLock)));
  if (lock) {
    new(lock) LVCSim_DebugLock();
  }
  return lock;
}

/*-----------------------------------------------------*/

void LVCSim_DebugCallback::mdDestroyLock(MemDbg_Lock *lock)
{
  mdSystemFree(lock);
}

/*-----------------------------------------------------*/
