/****************************************************************************

  module      : vgg941.cpp

  -------------------------------------------------------------------------

  author      : JuergenA
  responsible : UweH

  special area:
  description : GG_allocator_interface

  last changed: 2001-01-05
  release     : 7.4

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG



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


*****************************************************************************/

/*==========================================================================*
*  INCLUDES                                                                 *
*===========================================================================*/

#include "hgg941.h" // check of PASCAL header vgg941
#include "heo51.h"
#include "hgg01_1.h"
#include "gsp03_1.h"
#include "hsp77.h"

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Streaming.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_RawAllocator.hpp"
#include "RunTime/MemoryManagement/RTEMem_BlockAllocator.hpp"
#include "RunTime/RTE_Crash.hpp"
#include "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"

extern SAPDBTrace_Topic AllocatorTrace;

/*==========================================================================*
*  DEFINES                                                                  *
*===========================================================================*/

#define SMALL_BLOCK_SIZE        8192         // should be Cserv parameters ...
#define SUPPLEMENT_BLOCK_SIZE  (8192 * 16)

/*==========================================================================*.
* FUNCTIONS                                                                 *
*===========================================================================*/

externPascal tsp00_Addr gg941Allocate(tgg00_TransContext &TransContext, int wantedBytes)
{
    SAPDBTRACE_ROUTINE_DEBUG( "gg941Allocate", AllocatorTrace, 5 );
	return REINTERPRET_CAST(tsp00_Addr,
        (REINTERPRET_CAST(SAPDBMem_RawAllocator*,
        TransContext.trAllocator_gg00)->Allocate(wantedBytes)));
}

/*------------------------------------------------------------------*/

externPascal tsp00_Int4 gg941AllocatedByChunkProperty (
    tgg00_TransContext    VAR_VALUE_REF  TransContext,
    integer                              property)
{
    // determines the size of all used chunks having a given chunk property
    tsp00_Int4 allocated = 0;
    SAPDBMem_RawAllocator& allocator = *REINTERPRET_CAST(SAPDBMem_RawAllocator*, TransContext.trAllocator_gg00);
    for (SAPDBMem_RawAllocator::HeapIterator iter = allocator.SAPDBMem_RawAllocator::Begin(); iter; ++iter) 
    {
        if (property == SAPDBMem_RawAllocator::GetChunkProperties(iter()))
        {
            allocated += allocator.GetChunkSize(iter());
        }
    }
    return allocated;
}

/*------------------------------------------------------------------*/

externPascal tsp00_Addr gg941ReAllocate(tgg00_TransContext &TransContext, int wantedBytes, tsp00_Addr hint)
{
    SAPDBTRACE_ROUTINE_DEBUG( "gg941ReAllocate", AllocatorTrace, 5 );
	return REINTERPRET_CAST(tsp00_Addr,
        (REINTERPRET_CAST(SAPDBMem_RawAllocator*,
        TransContext.trAllocator_gg00)->Reallocate(wantedBytes, hint)));
}

/*------------------------------------------------------------------*/

externPascal pasbool gg941CheckConsistency(tgg00_TransContext &TransContext)
{
    return REINTERPRET_CAST(SAPDBMem_RawAllocator*, TransContext.trAllocator_gg00)->CheckConsistency();
}

/*------------------------------------------------------------------*/

externPascal void gg941Deallocate (
    tgg00_TransContext    VAR_VALUE_REF  TransContext,
    tsp00_Addr            VAR_VALUE_REF  p)
{
    SAPDBTRACE_ROUTINE_DEBUG( "gg941Deallocate", AllocatorTrace, 5 );
	REINTERPRET_CAST(SAPDBMem_RawAllocator*, TransContext.trAllocator_gg00)->Deallocate(p);
	p = (tsp00_Addr) 0;
}

/*------------------------------------------------------------------*/

externPascal void gg941CalcStatistics(
        tgg00_TransContext VAR_VALUE_REF TransContext, 
        SAPDB_ULong VAR_VALUE_REF usedbytes, 
        SAPDB_ULong VAR_VALUE_REF maxusedbytes, 
        SAPDB_ULong VAR_VALUE_REF ctrlbytes)
{
	REINTERPRET_CAST(SAPDBMem_RawAllocator*,
        TransContext.trAllocator_gg00)->CalcStatistics(usedbytes, maxusedbytes, ctrlbytes);
}


/*------------------------------------------------------------------*/

externPascal void gg941SetChunkProperty (
    tsp00_Addr                           pChunk,
    integer                              property)
{
    if ((0 <= property) && (property <= 7))
    {
        SAPDBMem_RawAllocator::SetChunkProperties(pChunk, property);
    }
}

