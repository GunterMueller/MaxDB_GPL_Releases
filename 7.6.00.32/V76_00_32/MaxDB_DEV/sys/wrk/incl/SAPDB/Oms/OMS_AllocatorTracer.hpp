/*!
 * \file    OMS_AllocatorTracer.hpp
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

#ifndef __OMS_ALLOCATORTRACE_HPP
#define __OMS_ALLOCATORTRACE_HPP

#include "Oms/OMS_RawAllocator.hpp"

/*!
   @brief implements the SAPDBMem_RawAllocator::ITracerAndExceptionHandler interface
 */
class OMS_AllocatorTracer : public  SAPDBMem_RawAllocator::ITracerAndExceptionHandler 
{
public :
    enum ExceptionHandler {THROW_BAD_DEALLOC, KERNEL_CRASH};
    OMS_AllocatorTracer( OMS_AllocatorTracer::ExceptionHandler handler);
    virtual void Dump (void* p, int size);
    virtual void Trace (const char* msg);
    virtual void VTrace (const char* msg);
    virtual void Exception(const char* msg);
private :
    OMS_AllocatorTracer::ExceptionHandler m_exceptionHandler;
};

#endif
