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
#include "ConsistentRead/CnsRead_CachedLeafPage.hpp"
#include "ConsistentRead/CnsRead_Manager.hpp"
#include "ConsistentRead/CnsRead_CachedRow.hpp"
#include "ConsistentRead/CnsRead_MarshaledTransHistoryInfo.hpp"

class CnsRead_LeafPageIterator
{
public :
    CnsRead_LeafPageIterator(CnsRead_CachedLeafPage& Page)
        : m_maxIndex(Page.nd_record_cnt())
        , m_curr(FIRST_REC_INDEX_BD00)
        , m_page(Page)
    {
        if (!CnsRead_Manager::IsConsistentEnabled() || (pt2Tab_egg00 != Page.nd_pt2()))
        {
            m_maxIndex = 0;
        }
    }
    operator bool() const
    {
        return m_curr < m_maxIndex;
    }
    CnsRead_CachedRow* operator->()
    {
        int pos = m_page.nd_pointer_list()[MAX_POINTERINDEX_BD00 - m_curr - POS_OFF_DIFF_BD00];
        return reinterpret_cast<CnsRead_CachedRow*>(&m_page.nd_body()[pos - POS_OFF_DIFF_BD00 - NODE_HEADER_MXBD00]);
    }
    void operator++()
    {
        ++m_curr;
    }
private :
    int                     m_maxIndex;
    int                     m_curr;
    CnsRead_CachedLeafPage& m_page;
};

//-----------------------------------------------------------------------------

inline bool CnsRead_CachedLeafPage::IsLeafPage() const
{
    return ((ptData_egg00 == nd_pt()) && (pt2Tab_egg00 == nd_pt2()) && CnsRead_Manager::IsConsistentEnabled());
}

//-----------------------------------------------------------------------------

void CnsRead_CachedLeafPage::FreeTransHistoryInfo(tsp00_TaskId TaskId)
{
    if (!IsLeafPage())
    {
        return;
    }
    CnsRead_LeafPageIterator iter(*this);
    while (iter)
    {
        iter->ReleaseTransHistoryInfo (TaskId);
        ++iter;
    }
}

//-----------------------------------------------------------------------------

void CnsRead_CachedLeafPage::RemoveTransHistroyInfo(tsp00_TaskId TaskId)
{
    if (!IsLeafPage())
    {
        return;
    }
    const bool cReleaseTransHistoryInfo = true;
    
    CnsRead_LeafPageIterator iter(*this);
    while (iter)
    {
        iter->RestoreRowHeader(TaskId, !cReleaseTransHistoryInfo);
        ++iter;
    }
}

//-----------------------------------------------------------------------------

void CnsRead_CachedLeafPage::RestoreTransHistoryInfo (
    tsp00_TaskId      TaskId,
    tgg00_BasisError& TrError)
{
    if (!IsLeafPage())
    {
        return;
    }
    CnsRead_MarshaledTransHistoryInfo::Iterator iter;
    CnsRead_MarshaledTransHistoryInfo marshaler (TaskId, nd_id());
    if (nd_history_info().includes(c_historyInPagingFile_ebd00))
    {

        Msg_List msgList;
        if (marshaler.Read(msgList))
        {
            iter = marshaler.Begin();
        }
    }
    else
    {
        if (nd_history_info().includes(c_historyOnPage_ebd00))
        {
            int bottom = ((nd_bottom() + 3) >> 2) << 2;
            iter = CnsRead_MarshaledTransHistoryInfo::Begin((void*) &nd_body()[bottom - NODE_HEADER_MXBD00]);
        }
    }
    while (iter)
    {
        if (iter.TransHistoryInfo().IsRequired())
        {
            const tsp00_Int4 recPos =
                nd_pointer_list()[MAX_POINTERINDEX_BD00 - iter.BodyIndex() - POS_OFF_DIFF_BD00];
            CnsRead_CachedRow* pRow =
                reinterpret_cast<CnsRead_CachedRow*>( &nd_body()[recPos - POS_OFF_DIFF_BD00 - NODE_HEADER_MXBD00]);
            if (!pRow->CreateTransHistoryInfo(TaskId, iter.TransHistoryInfo()))
            {
                TrError = e_no_more_memory; // TODO rollback of transaction ??
                break;
            }
        }
        ++iter;
    }
}

//-----------------------------------------------------------------------------

void CnsRead_CachedLeafPage::SaveTransHistoryInfo (
    tsp00_TaskId  TaskId,
    bool       ReleaseTransHistoryInfo,
    tbd_node&  copyArea,
    bool&      pageCopied)

{
    pageCopied = false;
    if (ReleaseTransHistoryInfo)
    {
        nd_history_info().clear();
    }
    else
    {
        nd_history_info().delElement(c_historyInPagingFile_ebd00);
        nd_history_info().delElement(c_historyOnPage_ebd00);
    }
    if (!IsLeafPage())
    {
        return;
    }
  
    tbd_nodeptr pNode = this;
    CnsRead_MarshaledTransHistoryInfo marshaler(TaskId, nd_id());
    const tsp00_Int4 maxRecIndex = nd_record_cnt() - 1;

    for( tsp00_Int4 recIndex = FIRST_REC_INDEX_BD00; recIndex <= maxRecIndex; ++recIndex )
    {

        const tsp00_Int4 recPos = nd_pointer_list()[MAX_POINTERINDEX_BD00 - recIndex - POS_OFF_DIFF_BD00];

        CnsRead_CachedRow* pRow =
            reinterpret_cast<CnsRead_CachedRow*>( &pNode->nd_body()[recPos - POS_OFF_DIFF_BD00 - NODE_HEADER_MXBD00]);
        const CnsRead_TransHistoryInfo* pInfo = pRow->GetTransHistoryInfo();
        if (pInfo)
        {
            if (pInfo->IsRequired())
            {
                if (!pageCopied && !ReleaseTransHistoryInfo)
                {
                    pageCopied = true;
                    SAPDB_MemCopyNoCheck (&copyArea, this, sizeof(*this));
                    pNode = &copyArea;
                    pRow  = reinterpret_cast<CnsRead_CachedRow*>(
                                &pNode->nd_body()[recPos - POS_OFF_DIFF_BD00 - NODE_HEADER_MXBD00]);
                    pNode->nd_history_info().clear();

                }
                marshaler.Append(recIndex, *pInfo);
            }
            pRow->RestoreRowHeader(TaskId, ReleaseTransHistoryInfo);
        }
    }
    if (marshaler.Count() > 0)
    {
        int bottom = ((pNode->nd_bottom() + 3) >> 2) << 2;
        int freeSpaceInPage = MAX_BOTTOM_BD00 - ((pNode->nd_record_cnt()+1)*POINTERSIZE_BD00) - bottom - 1;
        if (freeSpaceInPage > marshaler.Size())
        {
            pNode->nd_history_info().addElement(c_historyOnPage_ebd00);
            marshaler.Copy (&pNode->nd_body()[bottom - NODE_HEADER_MXBD00]);
            CnsRead_Manager::IncHistorySavedInPageCount();
        }
        else
        {
            Msg_List messageList;
            if (!marshaler.Write(messageList))
            {
                // TODO
                RTE_Crash(messageList);
            }
            else
            {
                pNode->nd_history_info().addElement(c_historyInPagingFile_ebd00);
            }
        }
    }
}

//-----------------------------------------------------------------------------

