/****************************************************************************/
/*!

  @file        SysView_MemoryAllocatorStatistics.cpp

  -------------------------------------------------------------------------

  @author      ElkeZ

  @ingroup     SystemViews
  @brief       This module implements the "MemoryAllocatorStatistics" view class.

  @see        

*/
/*-------------------------------------------------------------------------

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
#include "SystemViews/SysView_Defines.hpp"
#include "SystemViews/SysView_ITableObj.hpp"
#include "SystemViews/SysView_MemoryAllocatorStatistics.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "RunTime/MemoryManagement/RTEMem_AllocatorInfoReader.hpp"


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

void SysView_MemoryAllocatorStatistics::Create()
{

	m_Table->AppendCol (ITOCT_CHAR,  SV_ALLOCATORNAME, 40);
	m_Table->AppendCol (ITOCT_FIXED, SV_USEDSIZE, 20);
	m_Table->AppendCol (ITOCT_FIXED, SV_MAXUSEDSIZE, 20);
	m_Table->AppendCol (ITOCT_FIXED, SV_ALLOCATEDSIZE, 20);
	m_Table->AppendCol (ITOCT_FIXED, SV_ALLOCATECOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED, SV_DEALLOCATECOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED, SV_FAILEDALLOCATECOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED, SV_FOUNDERRORCOUNT, 10);    
	m_Table->AppendCol (ITOCT_CHAR,  SV_BASEALLOCATORNAME, 40);
	m_Table->AppendCol (ITOCT_FIXED, SV_BASEALLOCATECOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED, SV_BASEDEALLOCATECOUNT, 20);
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_MemoryAllocatorStatistics::GetColCount()
{
	return SV_CC_MEMORYALLOCATORSTATISTICS;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_MemoryAllocatorStatistics::EstimateRows()
{
	return SV_ER_MEMORYALLOCATORSTATISTICS;
}

/*---------------------------------------------------------------------------*/

void SysView_MemoryAllocatorStatistics::Execute()
{
    RTEMem_AllocatorInfoReader                  reader(m_Context.GetAllocator());
    RTEMem_AllocatorRegister::StatisticInfo    *pInfo;

    m_Table->GetCatalogTable();

    if (m_Context.IsOk())
    {

        if (reader.First())
        {
            while (reader.Next (pInfo) )
            {
#ifndef BIT64 /* The RTEMem_AllocatorRegister::StatisticInfo and SAPDBMem_IAllocatorInfo contain only 'SAPDB_ULong'.... */
                SAPDB_Int8 statisticConverter;
                m_Table->MoveToCol (ITOVT_CHARPTR, &pInfo->AllocatorName[0], strlen((SAPDB_Char *)&pInfo->AllocatorName[0]));
                statisticConverter = (SAPDB_Int8)(SAPDB_UInt8)pInfo->UsedBytes;
                m_Table->MoveToCol (ITOVT_INT8, &statisticConverter, 0);
                statisticConverter = (SAPDB_Int8)(SAPDB_UInt8)pInfo->MaxUsedBytes;
                m_Table->MoveToCol (ITOVT_INT8, &statisticConverter, 0);
                statisticConverter = (SAPDB_Int8)(SAPDB_UInt8)pInfo->AllocatedBytes;
                m_Table->MoveToCol (ITOVT_INT8, &statisticConverter, 0);
                statisticConverter = (SAPDB_Int8)(SAPDB_UInt8)pInfo->CountAlloc;
                m_Table->MoveToCol (ITOVT_INT8, &statisticConverter, 0);
                statisticConverter = (SAPDB_Int8)(SAPDB_UInt8)pInfo->CountDealloc;
                m_Table->MoveToCol (ITOVT_INT8, &statisticConverter, 0);
                statisticConverter = (SAPDB_Int8)(SAPDB_UInt8)pInfo->CountFailedAlloc;
                m_Table->MoveToCol (ITOVT_INT8, &statisticConverter, 0);
                statisticConverter = (SAPDB_Int8)(SAPDB_UInt8)pInfo->CountFoundError;
                m_Table->MoveToCol (ITOVT_INT8, &statisticConverter, 0);
                m_Table->MoveToCol (ITOVT_CHARPTR, &pInfo->AllocatorBaseName[0], (SAPDB_Int)strlen((SAPDB_Char *)&pInfo->AllocatorBaseName[0]));
                statisticConverter = (SAPDB_Int8)(SAPDB_UInt8)pInfo->BaseCountAlloc;
                m_Table->MoveToCol (ITOVT_INT8, &statisticConverter, 0);
                statisticConverter = (SAPDB_Int8)(SAPDB_UInt8)pInfo->BaseCountDealloc;
                m_Table->MoveToCol (ITOVT_INT8, &statisticConverter, 0);
#else
                m_Table->MoveToCol (ITOVT_CHARPTR, &pInfo->AllocatorName[0], strlen((SAPDB_Char *)&pInfo->AllocatorName[0]));
                m_Table->MoveToCol (ITOVT_INT8, (SAPDB_Int8 *)&pInfo->UsedBytes, 0);
                m_Table->MoveToCol (ITOVT_INT8, (SAPDB_Int8 *)&pInfo->MaxUsedBytes, 0);
                m_Table->MoveToCol (ITOVT_INT8, (SAPDB_Int8 *)&pInfo->AllocatedBytes, 0);
                m_Table->MoveToCol (ITOVT_INT8, (SAPDB_Int8 *)&pInfo->CountAlloc, 0);
                m_Table->MoveToCol (ITOVT_INT8, (SAPDB_Int8 *)&pInfo->CountDealloc, 0);
                m_Table->MoveToCol (ITOVT_INT8, (SAPDB_Int8 *)&pInfo->CountFailedAlloc, 0);
                m_Table->MoveToCol (ITOVT_INT8, (SAPDB_Int8 *)&pInfo->CountFoundError, 0);
                m_Table->MoveToCol (ITOVT_CHARPTR, &pInfo->AllocatorBaseName[0], (SAPDB_Int)strlen((SAPDB_Char *)&pInfo->AllocatorBaseName[0]));
                m_Table->MoveToCol (ITOVT_INT8, (SAPDB_Int8 *)&pInfo->BaseCountAlloc, 0);
                m_Table->MoveToCol (ITOVT_INT8, (SAPDB_Int8 *)&pInfo->BaseCountDealloc, 0);
#endif
            }
        }
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool SysView_MemoryAllocatorStatistics::ResetCounter(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_MemoryAllocatorStatistics::ResetCounter", SysView_Trace, 5);

    m_Context.ThrowError (e_not_implemented, 2);
    return ( true );
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
