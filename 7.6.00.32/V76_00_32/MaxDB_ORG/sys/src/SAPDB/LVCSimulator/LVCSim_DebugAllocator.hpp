/*!***************************************************************************

  module      : LVCSim_DebugAllocator.hpp

  -------------------------------------------------------------------------

  responsible : IvanS

  special area: liveCache Simulator
  description : debugging memory allocator

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2002-2005 SAP AG




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




*****************************************************************************/


#ifndef LVCSIM_DEBUGALLOCATOR_HPP
#define LVCSIM_DEBUGALLOCATOR_HPP

#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"

#ifndef LVCSIM_DEBUG_ALLOC_ESCAPE_ENUM
#define LVCSIM_DEBUG_ALLOC_ESCAPE_ENUM
enum LVCSim_DebugAllocatorEscapeEnum {
    LVCSIM_DA_ON            // turn on memory debugger 
    LVCSIM_DA_OFF           // turn off memory debugger
    LVCSIM_DA_CLEAR_LEAKS,  // mark all memory blocks as no leaks
    LVCSIM_DA_ENTER_NOLEAK, // mark all block allocated between this
    LVCSIM_DA_EXIT_NOLEAK,  // and this as no leaks
    LVCSIM_DA_CHECK_LEAKS,  // check for leaks and dump leaks into trace/on screen
    LVCSIM_DA_CHECK_MEMORY, // check memory consistency
    LVCSIM_DA_MESSAGES_ON,  // turn on memory messages on-screen (e.g., leaks; can be nested)
    LVCSIM_DA_MESSAGES_OFF, // turn off memory messages on-screen
	LVCSIM_DA_ALLOCERR_ON,	// turn on allocator error messages
	LVCSIM_DA_ALLOCERR_OFF,	// turn off allocator error messages
    LVCSIM_DA_SYMREFRESH,   // refresh symbol table for stack traces (from LVCSIM_SYMPATH and PATH)
    LVCSIM_DA_STACKTRACE    // dump stack trace to the dump file
};
#endif

#ifndef LVCSIM_HEAP_CHECK_LEVEL_ENUM
#define LVCSIM_HEAP_CHECK_LEVEL_ENUM
enum LVCSim_HEAP_CHECK_LEVEL {
    LVCSIM_HEAP_FENCES = 1,	      // places a pattern after euch used chunk and checks this pattern, when the chunk is freed
    LVCSIM_HEAP_FREEPATTERN = 2,	// fills each freed chunk with a pattern
    LVCSIM_HEAP_DOUBLEBOOK = 4,	  // double bookkeeping of used chunks
    LVCSIM_HEAP_BINTREE = 8,	    // check of binary tree of big free chunks
    LVCSIM_HEAP_EXCLUSIVE = 16, 	//checks exclusive usage of unsnchronized heaps
    LVCSIM_HEAP_DELAYEDFREE = 32, // delayed free of freed chunks
    LVCSIM_HEAP_DISABLESTACK = 64	// use default context allocator for object frames instead of stack allocator
};
#endif

class LVCSim_DebugAllocator : public SAPDBMem_IRawAllocator
{
public:
	static LVCSim_DebugAllocator &Instance();

	virtual void* AllocateArray(SAPDB_ULong ByteCount) = 0;
	virtual void DeallocateArray(void* p) = 0;

  static bool Escape(LVCSim_DebugAllocatorEscapeEnum what);

protected:
	friend	class LVCSim_KernelInterface;

    virtual bool doEscape(LVCSim_DebugAllocatorEscapeEnum what) = 0;
	virtual void doBlockError(const char *msg, const void *blk) = 0;
private:
	static LVCSim_DebugAllocator	*m_instance;
};

#endif // LVCSIM_DEBUGALLOCATOR_HPP
