/****************************************************************************

  module      : CnsRead_CachedRow.cpp

  -------------------------------------------------------------------------

  responsible : ThomasA

  special area: Consistent Read
  description : defines a row of a sql table, which is located in the data cache 
  last changed: 2001-02-26  9:49
  see also    : 

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

#include "ConsistentRead/CnsRead_CachedRow.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDBCommon/SAPDB_RangeCode.hpp" // Kernel_move_and_fill
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "DataAccess/Data_Row.hpp"

#include "Logging/Log_History.hpp"

#include "hkb50.h"
#include "hkb04.h"
#include "hkb71.h"

void CnsRead_CachedRow::Copy (bool withTransHistory, SAPDB_Int4 destSize, void* destBuf, SAPDB_Int4& destOffset, tgg00_BasisError& e) const
{
   CnsRead_TransHistoryTab* pTransHistory = m_transHistoryPointer.GetPointer();
   if (pTransHistory)
   {
       SAPDB_Byte* pDestBuf = reinterpret_cast<SAPDB_Byte*>(destBuf);
       pDestBuf += destOffset;
       destSize -= destOffset;
       if (withTransHistory)
       {
           SAPDB_MemCopyNoCheck (pDestBuf, &pTransHistory->TransHistoryInfo(), sizeof(pTransHistory->TransHistoryInfo()));
           pDestBuf += sizeof(pTransHistory->TransHistoryInfo());
           destOffset += sizeof(CnsRead_TransHistoryInfo);
       }
       SAPDB_MemCopyNoCheck (pDestBuf, pTransHistory, sizeof(CnsRead_SeparatedRowHeader));
       pDestBuf += sizeof(CnsRead_SeparatedRowHeader);
       SAPDB_RangeMove( __FILE__,   1,
           pTransHistory->GetRecLen(), destSize - (pDestBuf - reinterpret_cast<SAPDB_Byte*>(destBuf)), 
           this, sizeof(CnsRead_SeparatedRowHeader) + 1, pDestBuf, 1, 
           pTransHistory->GetRecLen() - sizeof(CnsRead_SeparatedRowHeader), e);
       destOffset += pTransHistory->GetRecLen();
   }
   else
   {
       if (withTransHistory)
       {
           new (destBuf) CnsRead_TransHistoryInfo;
           destOffset += sizeof(CnsRead_TransHistoryInfo);
       }
       SAPDB_RangeMove( __FILE__,   1, 
           m_recordHeader.m_separatableHeader.GetRecLen(), destSize, 
           this, 1, destBuf, destOffset + 1, m_recordHeader.m_separatableHeader.GetRecLen(), e);
       destOffset += m_recordHeader.m_separatableHeader.GetRecLen();
   }
}

//-----------------------------------------------------------------------------

void CnsRead_CachedRow::Copy (CnsRead_TransHistoryInfo& TransHistoryInfo, SAPDB_Int4 BufSize, void* destBuf, tgg00_BasisError& e) const
{
   CnsRead_TransHistoryTab* pTransHistory = m_transHistoryPointer.GetPointer();
   if (pTransHistory)
   {
       TransHistoryInfo = pTransHistory->TransHistoryInfo();
       SAPDB_Byte* pDestBuf = reinterpret_cast<SAPDB_Byte*>(destBuf);
       SAPDB_MemCopyNoCheck (pDestBuf, pTransHistory, sizeof(CnsRead_SeparatedRowHeader));
       pDestBuf += sizeof(CnsRead_SeparatedRowHeader);
       SAPDB_RangeMove( __FILE__,   1,
           pTransHistory->GetRecLen(), BufSize - (pDestBuf - reinterpret_cast<SAPDB_Byte*>(destBuf)), 
           this, sizeof(CnsRead_SeparatedRowHeader) + 1, pDestBuf, 1, 
           pTransHistory->GetRecLen() - sizeof(CnsRead_SeparatedRowHeader), e);
   }
   else
   {
       new ((void*) &TransHistoryInfo) CnsRead_TransHistoryInfo;
       reinterpret_cast<const Data_Row*>(this)->CopyRow(*reinterpret_cast<Data_Row*>(destBuf));
   }
}

//-----------------------------------------------------------------------------

bool CnsRead_CachedRow::CopyTransHistoryTab (RTE_TaskId TaskId, CnsRead_CachedRow& DestinationRow)
{
    CnsRead_TransHistoryTab* pTransHistory = m_transHistoryPointer.GetPointer();
    if (pTransHistory)
    {
        CnsRead_TransHistoryTab* p = CnsRead_TransHistoryTab::Create(TaskId, *pTransHistory);
        if (!p)
        {
            return false;
        }
        DestinationRow.m_transHistoryPointer.Assign(p);
    }
    return true;
}

//-----------------------------------------------------------------------------

bool CnsRead_CachedRow::CreateTransHistoryInfo(const tgg00_TransContext& CurrentTrans, tgg91_PageRef& BeforeRef)
{
    CnsRead_TransHistoryTab* p = CnsRead_TransHistoryTab::Create(CurrentTrans, this->m_recordHeader.m_separatableHeader);
    if (!p)
    {
        return false;
    }
    p->SetBeforeRef(BeforeRef);
    m_transHistoryPointer.Assign(p);
    return true;
}

//-----------------------------------------------------------------------------

bool CnsRead_CachedRow::CreateTransHistoryInfo(RTE_TaskId TaskId, const CnsRead_TransHistoryInfo& TransHistoryInfo)
{
    CnsRead_TransHistoryTab* p = CnsRead_TransHistoryTab::Create(TaskId, TransHistoryInfo, this->m_recordHeader.m_separatableHeader);
    if (!p)
    {
        return false;
    }
    m_transHistoryPointer.Assign(p);
    return true;
}

//-----------------------------------------------------------------------------

bool CnsRead_CachedRow::GetConsistentImage (tgg00_TransContext& currentTrans, const tgg91_PageRef& beforeRef, tgg00_Rec& consistentImage)
{
    SAPDB_Int4       destOffset = 0;
    tgg00_BasisError e          = e_ok;
    this->Copy (false, sizeof(consistentImage), &consistentImage, destOffset, e); 
    tgg91_PageRef currRef = beforeRef;
    do
    {
        if (NIL_PAGE_NO_GG00 == currRef.gg91RefPno())
        {
            break;
        }
        else
        {
            tgg91_TransNo        consistentView;
            tgg91_TransNo        updTrans;
            SAPDBErr_MessageList errlist;
            if (Log_History::getImageFound == Log_History::GetBeforeImage (currentTrans, 
                currRef, consistentView, consistentImage, updTrans, errlist))
            {
                tsp00_Bool bConsistent;  
                tsp00_Bool bCancelled;
                kb50ObjConsistentCheck (currentTrans, updTrans, bConsistent, bCancelled);
                if (bConsistent)
                {
                    return true;
                }
            }
            else
            {
                // TODO error handling
                break;
            }
        }
    }
    while (true);
    return false;
}

//-----------------------------------------------------------------------------

const CnsRead_TransHistoryInfo* CnsRead_CachedRow::GetTransHistoryInfo() const
{
    CnsRead_TransHistoryTab* pTransHistory = m_transHistoryPointer.GetPointer();
    if (pTransHistory)
    {
        return &pTransHistory->TransHistoryInfo();
    }
    return 0;
}

//-----------------------------------------------------------------------------

void CnsRead_CachedRow::DelUpdTestQualification (
    tgg00_MessBlock &MessBlock,
    bool             FirstQual,
    bool             ResultWanted,
    bool             CheckNewRow,
    void*            pResult,
    SAPDB_Int4       ResultSize,
    int             &ResultLen)
{
    if (this->IsDeleted())
    {
        MessBlock.mb_trns()->trError_gg00 = e_qual_violation;
        return;
    }
    k71qualification_test (MessBlock, FirstQual, ResultWanted, CheckNewRow, 
        *reinterpret_cast<tgg00_Rec*> (this), reinterpret_cast<tsp00_MoveObjPtr>(pResult), ResultSize, ResultLen);
}

//-----------------------------------------------------------------------------

bool CnsRead_CachedRow::IsQualified (
        tgg00_TransContext&      currentTrans,
        tgg00_SelectFieldsParam& interpreterContext,
        tgg00_StackDesc&         qualification)
{
    pasbool unqualified = true;
    CnsRead_TransHistoryTab* pTransHistory;
    if (CnsReadNone == currentTrans.trConsistReadInfo_gg00)
    {
        pTransHistory = 0;
    }
    else
    {
        if ((CnsReadStatement == currentTrans.trConsistReadInfo_gg00) && currentTrans.trConsistView_gg00.gg90IsNil())
        {
            kb04StartConsistentTrans (currentTrans, false /* isOmsVersion */ );
            if (currentTrans.trError_gg00 != e_ok)
            {
                return false;
            }
        }
        pTransHistory = m_transHistoryPointer.GetPointer();
    }
    
    if (!pTransHistory)
    {
        // no trans history info, current state of the row is assumed to be consistent
        if (!this->IsDeleted())
        {
            k71col_select (currentTrans, interpreterContext, qualification, *reinterpret_cast<tgg00_Rec*>(this), unqualified);
        }
    }
    else
    {
        if (pTransHistory->IsConsistent(currentTrans))
        { 
            // current row state is consistent for the current transaction, check if trans history tab is still required
            // TODO
            if (!this->IsDeleted(pTransHistory))
            {
                k71col_select (currentTrans, interpreterContext, qualification, *reinterpret_cast<tgg00_Rec*>(this), unqualified);
            }
        }
        else
        {
            // determine consistent image of the row
            tgg00_Rec consistentImage;
            if (this->GetConsistentImage(currentTrans, pTransHistory->GetBeforeRef(), consistentImage))
            {
                if (0 == (consistentImage.recVarcolCnt_gg00() & 0x4000))
                {
                    // not deleted
                    k71col_select (currentTrans, interpreterContext, qualification, consistentImage, unqualified);
                }
            }
        }
    }
    if (unqualified)
    {
        currentTrans.trError_gg00 = e_qual_violation;
        return false;
    }
    return true;
}

//-----------------------------------------------------------------------------

void CnsRead_CachedRow::Delete (tgg00_TransContext& currentTrans, const tgg91_PageRef& beforeRef, const tgg91_TransNo& updTrans, bool markDeleted)
{
    CnsRead_TransHistoryTab* pTransHistory = m_transHistoryPointer.GetPointer();
    if (!pTransHistory)
    {
        pTransHistory = CnsRead_TransHistoryTab::Create(currentTrans, this->m_recordHeader.m_separatableHeader);
        if (!pTransHistory) // TODO
        {
        }
        else
        {
            m_transHistoryPointer.Assign(pTransHistory);
        }
    }
    else
    {
        pTransHistory->SetUpdateTrans(updTrans);
    }
    pTransHistory->SetBeforeRef(beforeRef);
#if defined(BIT64)
    pTransHistory->MarkDeleted(markDeleted);
#else
    if (markDeleted)
    {
        m_recordHeader.m_varColCount |= 0x4000;
    }
    else
    {
        m_recordHeader.m_varColCount -= 0x4000;
    }
#endif
}

//-----------------------------------------------------------------------------

void  CnsRead_CachedRow::ReleaseTransHistoryInfo (RTE_TaskId TaskId)
{
    CnsRead_TransHistoryTab* pTransHistory = m_transHistoryPointer.GetPointer();
    if (pTransHistory)
    {
        pTransHistory->Delete(TaskId);
    }
}

//-----------------------------------------------------------------------------

void CnsRead_CachedRow::RestoreRowHeader(RTE_TaskId TaskId, bool releaseTransHistoryInfo)
{
   CnsRead_TransHistoryTab* pTransHistory = m_transHistoryPointer.GetPointer();
   if (pTransHistory)
   {
       SAPDB_MemCopyNoCheck (this, pTransHistory, sizeof(CnsRead_SeparatedRowHeader));
       if (releaseTransHistoryInfo)
       {
           pTransHistory->Delete(TaskId);
       }
   }
}

//-----------------------------------------------------------------------------

bool CnsRead_CachedRow::UpdateTransHistoryInfo(RTE_TaskId TaskId, const CnsRead_CachedRow& oldImage, const tgg91_PageRef& beforeRef, const tgg91_TransNo& updTrans)
{
    CnsRead_TransHistoryTab* pTransHistory = oldImage.m_transHistoryPointer.GetPointer();
    if (beforeRef.gg91IsNilRef())
    {
        // this may happen in the course of a rollback
        if (pTransHistory)
        {
            SAPDB_MemCopyNoCheck (this, pTransHistory, sizeof(CnsRead_SeparatedRowHeader));
            pTransHistory->Delete(TaskId);
        }
    }
    else
    {
        if (!pTransHistory)
        {
            // row does not have a trans history info yet, create a new one
            pTransHistory = CnsRead_TransHistoryTab::Create(TaskId, this->m_recordHeader.m_separatableHeader);
            if (!pTransHistory)
            {
                return false;
            }
        }
        else
        {
            // row already owns a trans history info, set the new row length
            SAPDBERR_ASSERT_ARGUMENT(0 == m_transHistoryPointer.GetPointer());
            pTransHistory->SetRecLen(m_recordHeader.m_separatableHeader.GetRecLen());
        }
        pTransHistory->SetUpdateTrans(updTrans);
        pTransHistory->SetBeforeRef(beforeRef);
        m_transHistoryPointer.Assign(pTransHistory);
    }
    return true;
}
