/*!***************************************************************************

  module      : SQLMan_MessBlock.cpp

  -------------------------------------------------------------------------

  responsible : ThomasA

  special area: SQL Manager
  description : encapsulation of pascal type tgg00_MessBlock

  last changed: 2000-12-06  19:04
  see also    : example.html ...
  first created:2000-03-09  18:21

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

#include "SQLManager/SQLMan_MessBlock.hpp"
#include "SQLManager/SQLMan_Tracing.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "SAPDBCommon/SAPDB_RangeCode.hpp"
#include "SAPDBCommon/Fields/SAPDBFields_MemorySequence.hpp"
#include "SAPDBCommon/Fields/SAPDBFields_SequenceMemoryCopy.hpp"

tgg00_BasisError SQLMan_MessBlock::Copy (SQLMan_Context& Context, SQLMan_MessBlock& Destination, bool WithoutData, SAPDBMem_IRawAllocator* pAllocator)
{
    Destination.mb_qual_len()  = mb_qual_len();
    Destination.mb_strat_len() = mb_strat_len();
    Destination.mb_src_site()  = mb_src_site();
    Destination.mb_reply()     = mb_reply();
    Destination.mb_type()      = mb_type();
    Destination.mb_type2()     = mb_type2();
    Destination.mb_struct()    = mb_struct();
    Destination.mb_trns()      = &Context.TransContext();
    for (int jx = 0; jx <= cgg_idx_max_valuefieldlist; ++jx)
    {
        Destination.mb_fieldlists()[jx] = 0;
    }
    tgg00_BasisError e = e_ok;
    if (QualLength() > 0)
    {
        if (QualLength() > Destination.QualSize())
        {
            return e_too_small_mb_qual_part;
        }
        SAPDB_RangeMove ("MBLOCK", 1, QualSize(), Destination.QualSize(), 
            QualPtr(), 1, Destination.QualPtr(), 1, QualLength(), e); 
        if (e_ok != e) return e;
        if  (mb_st() && (FirstFree() > 1) &&
            (QualLength() >= sizeof (tgg00_FileId) + sizeof(tgg00_StackDesc)))
        {
            int moveLen = (FirstFree() - 1) * sizeof(tgg00_StackEntry);
            if ((0 == Destination.mb_st()) || (moveLen > Destination.StackSize()))
            {
                return e_too_many_mb_stackentries;
            }
            if (moveLen > 0)
            {
                SAPDB_RangeMove ("MBLOCK", 2, StackSize(), Destination.StackSize(),
                    mb_st(), 1, Destination.mb_st(), 1, moveLen, e);
                if (e_ok != e) return e;
            }
            Destination.mb_qual()->mst_addr() = Destination.mb_st();
            Destination.mb_qual()->mst_max()  = Destination.mb_st_max();
        }
    }
    if (DataLength() > 0 && !WithoutData)
    {
        if (Destination.DataLength() + DataLength() > Destination.DataSize())
        {
            return e_too_many_mb_data;
        }
        SAPDB_RangeMove ("MBLOCK", 3, DataSize(), Destination.DataSize(),
            DataPtr(), 1, Destination.DataPtr(), Destination.DataLength() + 1, 
            DataLength(), e);
        if (e_ok != e) return e ;
        Destination.DataLength() += DataLength();
    }
    if (StratLength() > 0)
    {
        SAPDB_RangeMove ("MBLOCK", 4, StratSize(), Destination.StratSize(),
            StratPtr(), 1, Destination.StratPtr(), 1, StratLength(), e);
        if (e_ok != e) return e;
        Destination.StratLength() = StratLength();
    }
    for (int ix = 0; ix <= cgg_idx_max_valuefieldlist; ++ix)
    {
        SAPDBFields_MemorySequence* pSequence = reinterpret_cast<SAPDBFields_MemorySequence*>(mb_fieldlists()[ix]);
        if (pSequence)
        {
            SAPDBFields_MemorySequence* p;
            if (pAllocator)
            {
                // deep copy
                p = SAPDBFields_SequenceMemoryCopy::Create (*pAllocator, pSequence,
                                    SAPDBFields_MemorySequence::noCount , 
                                    Context.GetErrorList()); 
                if (!p)
                {
                    for (int kx = 0; kx < ix; ++kx)
                    {
                         SAPDBFields_MemorySequence* pFree = reinterpret_cast<SAPDBFields_MemorySequence*>(mb_fieldlists()[kx]);
                         if (pFree)
                         {
                             SAPDBFields_MemorySequence::DropReference(pFree);
                         }
                    }
                    return e_no_more_memory;
                }
            }
            else
            {
                // shallow copy
                p = pSequence->GetReference ();
            }
            Destination.mb_fieldlists()[ix] = reinterpret_cast<tsp00_MoveObjPtr>(p);
        }
        else
        {
            Destination.mb_fieldlists()[ix] = 0;
        }
    }
    return e_ok;
}

//----------------------------------------------------------------------------------

void* SQLMan_MessBlock::CreateCopy(const SQLMan_MessBlock& source, 
                                   SAPDBMem_IRawAllocator& allocator, 
                                   int                     offset,
                                   bool                    deepCopy,
								   SAPDB_UInt4&            memoryUsage)
{
    SAPDBTRACE_ROUTINE_DEBUG ("SQLMan_MessBlock::CreateCopy", SQLMan_Trace, 5);
	memoryUsage = offset + sizeof(tgg00_MessBlock);
    void* p = allocator.Allocate(memoryUsage);
    if (p)
    {
        int   allocatedIdx = 0;
        void* allocatedBlocks[5];
        bool  ok = true;

        allocatedBlocks[0] = p;
        SQLMan_MessBlock* pMessBlock = REINTERPRET_CAST (SQLMan_MessBlock*,
            REINTERPRET_CAST(char*, p) + offset);
        SAPDB_MemCopyNoCheck (pMessBlock, &source, sizeof(source));
        // initialize all pointers and sizes to 0
        pMessBlock->SetData();
        pMessBlock->SetQual();
        pMessBlock->SetStack();
        pMessBlock->SetStrat();
        pMessBlock->SetWorkArea();
        for (int jx = 0; jx <= cgg_idx_max_valuefieldlist; ++jx)
        {
            pMessBlock->mb_fieldlists()[jx] = 0;
        }
        // create qual and stack code part
        if (source.QualLength() > 0)
        {
            allocatedBlocks[allocatedIdx+1] = allocator.Allocate(source.QualLength());
            if (allocatedBlocks[allocatedIdx+1])
            {
				memoryUsage += source.QualLength();
                ++allocatedIdx;
                pMessBlock->SetQual(allocatedBlocks[allocatedIdx], source.QualLength());
                SAPDB_MemCopyNoCheck (allocatedBlocks[allocatedIdx], source.mb_qual(), source.QualLength()); 
                pMessBlock->QualLength() = source.QualLength();
                int stackLength = (source.mb_st()) ? (source.FirstFree() - 1) * sizeof(SQLMan_StackEntry) : 0;
                if (stackLength > 0)
                {
                    allocatedBlocks[allocatedIdx+1] = allocator.Allocate(stackLength);
                    if (allocatedBlocks[allocatedIdx+1])
                    {
						memoryUsage += stackLength;
                        ++allocatedIdx;
                        pMessBlock->SetStack(allocatedBlocks[allocatedIdx], stackLength);
                        SAPDB_MemCopyNoCheck (allocatedBlocks[allocatedIdx], source.mb_st(), stackLength);
                    }
                    else
                    {
                        ok = false;
                    }
                }
            }
            else
            {
                ok = false;
            }
        }
        // create required data part
        if (ok && (source.DataLength() > 0))
        {
            allocatedBlocks[allocatedIdx+1] = allocator.Allocate(source.DataLength());
            if (allocatedBlocks[allocatedIdx+1])
            {
				memoryUsage += source.DataLength();
                ++allocatedIdx;
                pMessBlock->SetData(allocatedBlocks[allocatedIdx], source.DataLength());
                SAPDB_MemCopyNoCheck (allocatedBlocks[allocatedIdx], source.mb_data(), source.DataLength());
                pMessBlock->DataLength() = source.DataLength();
            }
            else
            {
                ok = false;
            }
        }
        // create required strategy part
        if (ok && (source.StratLength() > 0))
        {
            allocatedBlocks[allocatedIdx+1] = allocator.Allocate(source.StratLength());
            if (allocatedBlocks[allocatedIdx+1])
            {
				memoryUsage += source.StratLength();
                ++allocatedIdx;
                pMessBlock->SetStrat(allocatedBlocks[allocatedIdx], source.StratLength());
                SAPDB_MemCopyNoCheck (allocatedBlocks[allocatedIdx], source.mb_strat(), source.StratLength());
                pMessBlock->StratLength() = source.StratLength();
            }
        }
        if (ok)
        {
            for (int ix = 0; ix <= cgg_idx_max_valuefieldlist && ok; ++ix)
            {
                SAPDBFields_MemorySequence* pSequence = reinterpret_cast<SAPDBFields_MemorySequence*>(source.mb_fieldlists()[ix]);
                if (pSequence)
                {
                    SAPDBFields_MemorySequence* p;
                    if (deepCopy)
                    {
                        Msg_List msgList;
                        // deep copy
                        p = SAPDBFields_SequenceMemoryCopy::Create (allocator, pSequence, 
                                SAPDBFields_MemorySequence::sharedCount, msgList);
                    }
                    else
                    {
                        p = pSequence->GetReference(); 
                    }
                    pMessBlock->mb_fieldlists()[ix] = reinterpret_cast<tsp00_MoveObjPtr>(p);
                    ok = p != 0;
                }
            }
        }
        if (!ok)
        {
            while (allocatedIdx >= 0)
            {
                allocator.Deallocate(allocatedBlocks[allocatedIdx]);
                --allocatedIdx;
            }
            for (int ix = 0; ix <= cgg_idx_max_valuefieldlist; ++ix)
            {
                SAPDBFields_MemorySequence* pSequence = reinterpret_cast<SAPDBFields_MemorySequence*>(source.mb_fieldlists()[ix]);
                if (pSequence)
                {
                    SAPDBFields_MemorySequence::DropReference(pSequence);
                    pMessBlock->mb_fieldlists()[ix] = 0;
                }
            }
            p = NULL;
        }
    }
    return p;
}

//----------------------------------------------------------------------------------

void SQLMan_MessBlock::DestroyParts(SAPDBMem_IRawAllocator& allocator)
{
	allocator.Deallocate(mb_data());
    allocator.Deallocate(mb_qual());
	allocator.Deallocate(mb_st());
    allocator.Deallocate(mb_strat());
    for (int ix = 0; ix <= cgg_idx_max_valuefieldlist; ++ix)
    {
        SAPDBFields_MemorySequence* pSequence = reinterpret_cast<SAPDBFields_MemorySequence*>(mb_fieldlists()[ix]);
        if (pSequence)
        {
            SAPDBFields_MemorySequence::DropReference(pSequence);
        }
    }
}

//----------------------------------------------------------------------------------

void SQLMan_MessBlock::InvalidateTransaction()
{
    mb_trns() = NULL;
}
