/****************************************************************************

  module      : RTESync_SpinlockReader.cpp

  -------------------------------------------------------------------------

  responsible : UlrichJ

  special area: RTE Spinlock Reader
  description : 

  last changed: 2002-08-21
  see also    :

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



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "ggg00.h"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "RunTime/MemoryManagement/RTEMem_AllocatorRegister.hpp"
#include "RunTime/MemoryManagement/RTEMem_AllocatorInfoReader.hpp"
#include "Container/Container_List.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS  (PROTOTYPES)                                     *
 *===========================================================================*/


/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

RTEMem_AllocatorInfoReader::RTEMem_AllocatorInfoReader(SAPDBMem_IRawAllocator &mAlloc)
: m_Allocator(mAlloc)
{
    m_List = NULL;
    m_Current = 0;
}

/*---------------------------------------------------------------------------*/

RTEMem_AllocatorInfoReader::~RTEMem_AllocatorInfoReader()
{
    FreeAll();
}

void RTEMem_AllocatorInfoReader::FreeAll()
{
    if (m_List)
    {
        m_List->Delete();
        m_Current = m_List->End();

        m_Allocator.Deallocate(m_List);
        m_List = NULL;
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEMem_AllocatorInfoReader::First()
{
    RTEMem_AllocatorRegister::StatisticInfo     *pInfo;
    SAPDB_Int4                                  elements;
    RTEMem_AllocatorRegister::StatisticInfo     toAdd;
    SAPDB_UInt8                                 memSuccAllocs;
    SAPDB_UInt8                                 memFailedAllocs;
    SAPDB_UInt8                                 memCommits;
    SAPDB_UInt8                                 memDeallocs;
    SAPDB_ULong                                 memCommitedBytes;
    SAPDB_Int                                   iCount;


    // Initialise internal list:

    FreeAll();
    m_List = new(m_Allocator) RTEMem_AllocatorInfoList(m_Allocator);

    // Generate master element:

    RTE_ISystem::Instance().GetMemoryStatistics (toAdd.AllocatedBytes,
                                                 memCommitedBytes,
                                                 toAdd.UsedBytes,
                                                 toAdd.MaxUsedBytes,
                                                 memSuccAllocs,
                                                 memFailedAllocs,
                                                 memCommits,
                                                 memDeallocs);

    toAdd.CountAlloc       = (SAPDB_ULong)(memSuccAllocs + memFailedAllocs);
    toAdd.CountFailedAlloc = (SAPDB_ULong)memFailedAllocs;
    toAdd.CountFoundError  = 0;
    toAdd.CountDealloc     = (SAPDB_ULong)(memDeallocs);
    toAdd.BaseCountAlloc   = 0;
    toAdd.BaseCountDealloc = 0;

    strcpy((SAPDB_Char *)toAdd.AllocatorName,"SystemHeap");
    strcpy((SAPDB_Char *)toAdd.AllocatorBaseName,"");

    m_List->InsertEnd(toAdd);

    // Get list of allocator statistics:

    if ( RTEMem_AllocatorRegister::Instance().GetStatisticInfo(pInfo, elements) )
    {
        // append allocator statistics array to list:

        for (iCount=0; iCount<elements; iCount ++)
        {
            m_List->InsertEnd(pInfo[iCount]);
        }

        RTEMem_AllocatorRegister::Instance().FreeStatisticInfo (pInfo);

        m_Current = m_List->Begin();

        return (true);
    }
    else
    {
        m_Current = m_List->End();

        return (false);
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTEMem_AllocatorInfoReader::Next(RTEMem_AllocatorRegister::StatisticInfo * &pInfo)
{
    if (m_Current != m_List->End())
    {
        pInfo = (RTEMem_AllocatorRegister::StatisticInfo *)&(*m_Current);
        ++m_Current;
        return (true);
    }
    else
    {
        FreeAll();
        return (false);
    }
}


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
