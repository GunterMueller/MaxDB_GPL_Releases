/*!***************************************************************************

  module:       CnsRead_TransHistoryTab.cpp

  responsible : ThomasA

  special area: Consistent Read

  description:  Defines the transient trans and history information of a record lying in the data cache

  last changed: 2000-08-10  12:27
  see also    :

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


#include "ConsistentRead/CnsRead_TransHistoryTab.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "KernelCommon/MemoryManagement/KernelMem_DataCacheAllocator.hpp"

#include "hkb50.h"

static KernelMem_DataCacheAllocator<CnsRead_TransHistoryTab>* m_pAllocator = 0;

//-----------------------------------------------------------------------------

bool CnsRead_TransHistoryTab::Initialize(Msg_List& MsgList)
{
    SAPDBMEM_STATIC_RAW_ALLOCATE(Space, sizeof(KernelMem_DataCacheAllocator<CnsRead_TransHistoryTab>));
    bool ok;
    m_pAllocator = new ((void*) Space) KernelMem_DataCacheAllocator<CnsRead_TransHistoryTab> (sizeof(CnsRead_TransHistoryTab),
        (SAPDB_UTF8*) "TransHistoryTab", ok, MsgList);
    if (!ok)
    {
        m_pAllocator = 0;
    }
    return ok;
}

//-----------------------------------------------------------------------------

CnsRead_TransHistoryTab::CnsRead_TransHistoryTab(const CnsRead_SeparatedRowHeader& rowHeader)
: m_recordHeader(rowHeader)
, m_transInfo()
{
}

//-----------------------------------------------------------------------------

CnsRead_TransHistoryTab::CnsRead_TransHistoryTab(const tgg00_TransContext& currentTrans, const CnsRead_SeparatedRowHeader& rowHeader)
: m_recordHeader(rowHeader)
, m_transInfo(currentTrans)
{
}

//-----------------------------------------------------------------------------

CnsRead_TransHistoryTab::CnsRead_TransHistoryTab(const CnsRead_TransHistoryInfo& TransHistoryInfo,
                                                 const CnsRead_SeparatedRowHeader& RowHeader)
: m_recordHeader(RowHeader)
, m_transInfo(TransHistoryInfo)
{
}

//-----------------------------------------------------------------------------

CnsRead_TransHistoryTab* CnsRead_TransHistoryTab::Create (RTE_TaskId TaskId, const CnsRead_SeparatedRowHeader& rowHeader)
{
    CnsRead_TransHistoryTab* p = m_pAllocator->Allocate(TaskId);
    if (p)
    {
        new((void*) p) CnsRead_TransHistoryTab (rowHeader);
    }
    return p;
}

//-----------------------------------------------------------------------------

CnsRead_TransHistoryTab* CnsRead_TransHistoryTab::Create (const tgg00_TransContext& currentTrans, const CnsRead_SeparatedRowHeader& rowHeader)
{
    CnsRead_TransHistoryTab* p = m_pAllocator->Allocate(currentTrans.trTaskId_gg00);
    if (p)
    {
        new((void*) p) CnsRead_TransHistoryTab (currentTrans, rowHeader);
    }
    return p;
}

//-----------------------------------------------------------------------------

CnsRead_TransHistoryTab* CnsRead_TransHistoryTab::Create (RTE_TaskId TaskId, const CnsRead_TransHistoryTab& Source)
{
    CnsRead_TransHistoryTab* p = m_pAllocator->Allocate(TaskId);
    if (p)
    {
        *p = Source;
    }
    return p;
}

//-----------------------------------------------------------------------------

CnsRead_TransHistoryTab* CnsRead_TransHistoryTab::Create(RTE_TaskId TaskId, const CnsRead_TransHistoryInfo& TransHistoryInfo, const CnsRead_SeparatedRowHeader& RowHeader)
{
    CnsRead_TransHistoryTab* p = m_pAllocator->Allocate(TaskId);
    if (p)
    {
        new((void*) p) CnsRead_TransHistoryTab (TransHistoryInfo, RowHeader);
    }
    return p;
}

//-----------------------------------------------------------------------------

void CnsRead_TransHistoryTab::Delete(RTE_TaskId TaskId)
{
    m_pAllocator->Deallocate(TaskId, this);
}

//-----------------------------------------------------------------------------

bool CnsRead_TransHistoryTab::IsConsistent(tgg00_TransContext& currentTrans) 
{
    tsp00_Bool bConsistent;  
    tsp00_Bool bCancelled;
    kb50ObjConsistentCheck (currentTrans, m_transInfo.otrUpdTransId_gg00, bConsistent, bCancelled);
    return (bConsistent != 0);
}
