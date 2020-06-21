/*!***************************************************************************

  module      : RTEMem_AllocatorRegister.hpp

  -------------------------------------------------------------------------

  responsible : JoergM

  special area: RTE
  description : Allocator registration 
  
                This class implements a singleton, that allows to register allocators.
                This allows to collect the memory usage information for all registered
                allocators.

  last changed: 2001-01-10  11:04
  see also    : <class>RTE_ItemRegister</class>
  first created:2000-03-09  18:21

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2001-2005 SAP AG


    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/MemoryManagement/RTEMem_AllocatorRegister.hpp"
#include "RunTime/MemoryManagement/RTEMem_RteAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDBCommon/SAPDB_string.h"

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

RTEMem_AllocatorRegister * RTEMem_AllocatorRegister::m_Instance = 0;
const SAPDB_Int4 RTEMem_AllocatorRegister::m_MaxIdentfierLength = 40;

RTEMem_AllocatorRegister & RTEMem_AllocatorRegister::Instance()
{
    SAPDBMEM_STATIC_RAW_ALLOCATE(Space, sizeof(RTEMem_AllocatorRegister));

    if ( !m_Instance )
    {
#if defined(SAPDB_SLOW)
        m_Instance = new ( Space ) RTEMem_AllocatorRegister(true);
#else
        m_Instance = new ( Space ) RTEMem_AllocatorRegister(false);
#endif
    }

    return *m_Instance;
}

/*!
   Function: GetStatisticInfo
   Description: Get a snapshot of all statistic informations
   Arguments: pInfoArray[out] Pointer to array of StatisticInfo

   Return value: True if info array fill, false if not not enough memory or nothing registered
 */
SAPDB_Bool RTEMem_AllocatorRegister::GetStatisticInfo(StatisticInfo * &pInfoArray, SAPDB_Int4 &itemCount)
{
    SAPDB_Int4 currentCount;
    StatisticInfo *pSnapshotInfoArray = 0;

    while ( 0 == pSnapshotInfoArray )
    {
        currentCount = this->m_Count; // dirty read... this leads to the necessity of 'while()' loop. See below
        if ( 0 == currentCount )
        {
            return false;
        }

        pSnapshotInfoArray = reinterpret_cast<StatisticInfo *>(RTEMem_RteAllocator::Instance().Allocate(currentCount * sizeof(StatisticInfo)));
        if ( 0 == pSnapshotInfoArray )
        {
            return false;
        }

        memset( pSnapshotInfoArray, 0, currentCount * sizeof(StatisticInfo));

        /* This is using a locked scope... */
        if ( !FillStatisticInfo(pSnapshotInfoArray, currentCount, itemCount) )
        {
            /* FillInfoArray() only reason to fail, is that the most unlikely case happened and */
            /* in the meantime more allocators were registered. This is handled with forcing a loop here... */
            RTEMem_RteAllocator::Instance().Deallocate(pSnapshotInfoArray);
            pSnapshotInfoArray = 0;
        }
    }

    pInfoArray = pSnapshotInfoArray;

    return true;
}

/*!
   Function: FreeStatisticInfo
   Description: Free the snapshot of all statistic informations collected by GetStatisticInfo

   Arguments: pInfoArray[in] Pointer to array of StatisticInfo
 */
void RTEMem_AllocatorRegister::FreeStatisticInfo(StatisticInfo *pInfoArray)
{
    RTEMem_RteAllocator::Instance().Deallocate(pInfoArray);
}

SAPDB_Bool RTEMem_AllocatorRegister::FillStatisticInfo(StatisticInfo * &pInfoArray, SAPDB_Int4 maxCount, SAPDB_Int4 &itemCount)
{
    RTESync_LockedScope lock(Spinlock());

    if ( this->m_Count > maxCount )
    {
        return false;
    }

    StatisticInfo *pFill = pInfoArray;

    for ( RTEMem_AllocatorInfo * pInfo = this->m_First; 0 != pInfo; pInfo = pInfo->GetSuccessor() )
    {
        SAPDB_ULong bytesUsed;
        SAPDB_ULong maxbytesUsed;
        SAPDB_ULong bytesAllocated;
        SAPDB_ULong countAlloc;
        SAPDB_ULong baseCountDealloc;
		SAPDB_ULong baseCountAlloc;
        SAPDB_ULong countDealloc;

        pInfo->GetItem().CalcStatistics(bytesUsed, maxbytesUsed, bytesAllocated);
        pFill->UsedBytes = bytesUsed;
        pFill->MaxUsedBytes = maxbytesUsed;
        pFill->AllocatedBytes = bytesAllocated;

        pInfo->GetItem().GetCallStatistics(countAlloc, countDealloc);
        pFill->CountAlloc = countAlloc;
        pFill->CountDealloc = countDealloc;

        pInfo->GetItem().GetBaseAllocatorCallStatistics(baseCountAlloc, baseCountDealloc);
        pFill->BaseCountAlloc = baseCountAlloc;
        pFill->BaseCountDealloc = baseCountDealloc;
        pFill->CountFailedAlloc = pInfo->GetItem().GetAllocFailedCount();
        pFill->CountFoundError = pInfo->GetItem().GetErrorCount();

        SAPDB_strncpy_zero_terminated((char *)pFill->AllocatorName, (char *)pInfo->GetIdentifier(), RTEMem_AllocatorRegister::m_MaxIdentfierLength);
        SAPDB_strncpy_zero_terminated((char *)pFill->AllocatorBaseName, (char *)pInfo->GetBaseIdentifier(), RTEMem_AllocatorRegister::m_MaxIdentfierLength);

        ++pFill;
    }

    itemCount = this->m_Count;

    return true;
}


