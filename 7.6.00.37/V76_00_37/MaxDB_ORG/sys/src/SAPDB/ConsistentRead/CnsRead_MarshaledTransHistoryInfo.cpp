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
#include "ConsistentRead/CnsRead_MarshaledTransHistoryInfo.hpp"
#include "ConsistentRead/CnsRead_Manager.hpp"
#include "ConsistentRead/CnsRead_Messages.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"

CnsRead_MarshaledTransHistoryInfo::CnsRead_MarshaledTransHistoryInfo(tsp00_TaskId TaskId, tsp00_PageNo Pno)
: m_taskId(TaskId)
{
    m_buffer.m_header.hrecKeyLen_gg00() = sizeof(m_buffer.m_pno);
    m_buffer.m_transInfo.m_cnt          = 0;
    m_buffer.m_pno                      = Pno;
};

//-----------------------------------------------------------------------------

void CnsRead_MarshaledTransHistoryInfo::Append 
(
 int                       Index, 
 const tgg00_ObjTransInfo& TransHistoryInfo
 )
{
    m_buffer.m_transInfo.m_transHistoryInfos[m_buffer.m_transInfo.m_cnt].m_index     = Index;
    m_buffer.m_transInfo.m_transHistoryInfos[m_buffer.m_transInfo.m_cnt].m_transInfo = TransHistoryInfo;
    ++m_buffer.m_transInfo.m_cnt;
}

//-----------------------------------------------------------------------------

void CnsRead_MarshaledTransHistoryInfo::Copy(void* pDest) const
{
    SAPDB_MemCopyNoCheck (pDest, &m_buffer.m_transInfo, this->Size()); 
}

//-----------------------------------------------------------------------------

bool CnsRead_MarshaledTransHistoryInfo::Read(SAPDBErr_MessageList& ErrorList)
{
    return CnsRead_Manager::ReadTransHistoryInfo(m_taskId, *reinterpret_cast<tgg00_Rec*>(&m_buffer), ErrorList);
}

//-----------------------------------------------------------------------------

int CnsRead_MarshaledTransHistoryInfo::Size() const
{
    return sizeof(m_buffer.m_transInfo.m_cnt) + m_buffer.m_transInfo.m_cnt * sizeof(m_buffer.m_transInfo.m_transHistoryInfos[0]);
}

//-----------------------------------------------------------------------------

bool CnsRead_MarshaledTransHistoryInfo::Write(SAPDBErr_MessageList& ErrorList)
{
    m_buffer.m_header.hrecLen_gg00() = sizeof(m_buffer.m_header) +
        sizeof(m_buffer.m_pno) + sizeof(m_buffer.m_transInfo.m_cnt) +
        m_buffer.m_transInfo.m_cnt * sizeof(m_buffer.m_transInfo.m_transHistoryInfos[0]);
    return CnsRead_Manager::WriteTransHistoryInfo(m_taskId, *reinterpret_cast<tgg00_Rec*>(&m_buffer), ErrorList); 
}

//-----------------------------------------------------------------------------
