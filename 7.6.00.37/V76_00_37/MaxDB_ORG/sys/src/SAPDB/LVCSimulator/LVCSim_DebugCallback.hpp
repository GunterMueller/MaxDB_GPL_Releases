/*!
\file       LVCSim_DebugCallback.hpp
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

#ifndef __LVCSIM_DEBUGCALLBACK_HPP
#define __LVCSIM_DEBUGCALLBACK_HPP

#include "MemoryDebugger/MemDbg_Callback.hpp"

class LVCSim_DebugCallback : public MemDbg_Callback {
public:
  virtual size_t mdFillTrace(void **traceptr, size_t levels);

  virtual void mdAllocatorMessage(const char *msg, MemDbg_Interface *allocator, const void *blk,
    size_t size, int allocTrace, int freeTrace, int currentTrace);

  virtual void mdAbort(const char *msg);

  virtual void mdAbstractCall(const void *blk, const void *vtable, size_t size, int allocTrace, int freeTrace);

  virtual MemDbg_Lock *mdCreateLock();

  virtual void mdDestroyLock(MemDbg_Lock *lock);

  static LVCSim_DebugCallback &getInstance();

  static inline bool hasErrors()
  {
    return m_hasErrors;
  }

private:
  void *operator new(size_t sz, void *p)
  {
    return p;
  }

  void operator delete(void *ptr, void *p)
  {
  }

  void dumpTrace(int id);

  static LVCSim_DebugCallback *m_instance;
  static bool m_hasErrors;
};

#endif  // __LVCSIM_DEBUGCALLBACK_HPP
