/*!
 * \file    OMS_AllocatorTracer.cpp
 * \author  ThomasA
 * \brief   implements tracer callback of SAPDBMem_RawAllocator
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


#include "Oms/OMS_AllocatorTracer.hpp"
#include "Oms/OMS_Globals.hpp"
#include "Oms/OMS_DbpError.hpp"


OMS_AllocatorTracer::OMS_AllocatorTracer( OMS_AllocatorTracer::ExceptionHandler handler)
: m_exceptionHandler(handler)
{
}

//-----------------------------------------------------------------------------

void OMS_AllocatorTracer::Dump (void* p, int size)
{
    OMS_Globals::GetKernelInterface()->DumpChunk (p, size);
}

//-----------------------------------------------------------------------------

void OMS_AllocatorTracer::Trace (const char* msg)
{
    OMS_Globals::GetKernelInterface()->ConsoleError(msg);
}

//-----------------------------------------------------------------------------

void OMS_AllocatorTracer::VTrace (const char* msg)
{
    OMS_Globals::GetKernelInterface()->VTrace(msg);
}

//-----------------------------------------------------------------------------

void OMS_AllocatorTracer::Exception(const char* msg)
{   
  switch(m_exceptionHandler){
      case OMS_AllocatorTracer::THROW_BAD_DEALLOC:
        // TODO, new exception class for internal liboms allocator errors
        OMS_Globals::m_globalsInstance->Throw(e_invalid_deallocate, msg, __MY_FILE__, __LINE__);
        break;
      case OMS_AllocatorTracer::KERNEL_CRASH:
        OMS_Globals::GetKernelInterface()->Crash(msg);
  }
}

////-----------------------------------------------------------------------------
//
//void OMS_AllocatorTracer::Throw(const char* msg)
//{
//    // TODO, new exception class for internal liboms allocator errors
//    OMS_Globals::m_globalsInstance->Throw(e_invalid_deallocate, msg, __MY_FILE__, __LINE__);
//}
//
////-----------------------------------------------------------------------------
//
//void OMS_AllocatorTracer::Crash(const char* msg)
//{
//    OMS_Globals::GetKernelInterface()->Crash(msg);
//}
