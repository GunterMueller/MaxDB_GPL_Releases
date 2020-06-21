/*!
  @file           KernelMem_RawAllocatorTracer.cpp
  @author         ThomasA
  @ingroup        Memory Mangement
  @brief          Implementations a tracer for SAPDBMem_RawAllocator

\if EMIT_LICENCE

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



\endif
*/

#include "heo52.h" // vtimestamp
#include "heo66.h" // vfopen etc

#include "KernelCommon/MemoryManagement/KernelMem_RawAllocatorTracer.hpp"
#include "RunTime/RTE_Message.hpp"
#include "KernelCommon/Kernel_OpMsg.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_Exceptions.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"

class KernelMem_Tracer : public SAPDBMem_RawAllocator::ITracerAndExceptionHandler 
{
public :
    virtual void Dump (void* p, int size)
    {
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
    }
    virtual void Trace (const char* msg)
    {
        Kernel_OpError (csp3_no_more_memory, csp3_n_memory) << msg; 
    }
    virtual void VTrace (const char* msg)
    {
        Kernel_VTrace() << msg;
    }
    virtual void Exception(const char* msg)
    {
        SAPDBMem_ConsistencyException message(__CONTEXT__, msg); 
        RTE_Crash(message); 
    }
};

static KernelMem_Tracer theTrace; 

SAPDBMem_RawAllocator::ITracerAndExceptionHandler& KernelMem_RawAllocatorTracer::GetTracer()
{
     return theTrace;
}