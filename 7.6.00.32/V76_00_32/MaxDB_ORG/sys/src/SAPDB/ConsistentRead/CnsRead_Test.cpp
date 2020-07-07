/*

    ========== licence begin  GPL
    Copyright (c) 2005 SAP AG

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
#include "ConsistentRead/CnsRead_Test.hpp"
#include "ConsistentRead/CnsRead_CachedRow.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "KernelCommon/MemoryManagement/KernelMem_DataCacheAllocator.hpp"

static KernelMem_DataCacheAllocator<int>* m_pAllocator = 0;

void CnsRead_Test::Execute (int TestNo)
{
    SQLMan_Context* pContext = SQLMan_Context::GetContext();
    tgg00_Rec rec;
    Msg_List msgList;
    bool ok;
    if (0 == m_pAllocator)
    {
        m_pAllocator = new KernelMem_DataCacheAllocator<int> (sizeof(int), (SAPDB_UTF8*) "MyAllocator", ok, msgList);
    }
    int* pInt = m_pAllocator->Allocate(pContext->a_transinf.tri_trans.trTaskId_gg00);
    if (pInt)
    {
        m_pAllocator->Deallocate(pContext->a_transinf.tri_trans.trTaskId_gg00, pInt);
    }
    for (int recLen = 0; recLen < 8192; ++recLen)
    {
        for (int keyLen = 1; keyLen <= 1024; ++keyLen)
        {
            rec.recLen_gg00()          = recLen;
            rec.recKeyLen_gg00()       = keyLen;
            rec.recVarcolOffset_gg00() = 21;
            rec.recVarcolCnt_gg00()    = 7;
            CnsRead_CachedRow* pRow = reinterpret_cast<CnsRead_CachedRow*>(&rec);
            tgg91_PageRef beforeRef;
            pRow->CreateTransHistoryInfo(pContext->a_transinf.tri_trans, beforeRef);
            if (pRow->GetRecLen() != recLen)
            {
                pContext->ThrowError(e_invalid);
            }
            if (pRow->GetKeyLen() != keyLen) 
            {
                pContext->ThrowError(e_invalid);
            }
            pRow->RestoreRowHeader(pContext->a_transinf.tri_trans.trTaskId_gg00, true);
            if (rec.recLen_gg00() != recLen)
            {
                pContext->ThrowError(e_invalid);
            }
            if (rec.recKeyLen_gg00() != keyLen)  
            {
                pContext->ThrowError(e_invalid);
            }
            if (!pContext->IsOk())
            {
                return;
            }
        }
    }
}