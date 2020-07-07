
/*!***************************************************************************

  module:       CnsRead_MarshaledTransHistoryInfo

  responsible : ThomasA

  special area: Consistent Read

  description:  wites trans and history info into a buffer

  last changed: 2000-08-10  12:27

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


#ifndef CNSREAD_MARSHALEDTRANSHISTORYINFO_HPP
#define CNSREAD_MARSHALEDTRANSHISTORYINFO_HPP

#include "ggg00.h"
#include "ConsistentRead/CnsRead_TransHistoryInfo.hpp"

class SAPDBErr_MessageList;

class CnsRead_MarshaledTransHistoryInfo
{
#if defined(SUN) || defined(SOLARIS)
public :
#endif
    struct OneTransHistoryInfo
    {
        SAPDB_UInt2              m_index;
        CnsRead_TransHistoryInfo m_transInfo;
    };
    struct TransHistoryInfoArray
    {
        SAPDB_UInt4         m_cnt;
        OneTransHistoryInfo m_transHistoryInfos[sizeof(tgg00_Rec) / sizeof(OneTransHistoryInfo)];
    };

    struct MarshalingBuffer
    {
        tgg00_HeaderRec                                          m_header;
        tsp00_PageNo                                             m_pno;
        CnsRead_MarshaledTransHistoryInfo::TransHistoryInfoArray m_transInfo;
    };
public :
    class Iterator {
    public :
         Iterator()
            : m_pMax(0)
            , m_pCurr(0)
        {
        }
        inline void operator++();
        inline operator bool() const; 
        inline int  BodyIndex() const;
        inline const CnsRead_TransHistoryInfo& TransHistoryInfo() const;
    private :
        Iterator (const CnsRead_MarshaledTransHistoryInfo::OneTransHistoryInfo* pInfo, int max)
            : m_pMax (pInfo + max)
            , m_pCurr(pInfo)
        { 
        }
        const CnsRead_MarshaledTransHistoryInfo::OneTransHistoryInfo* m_pMax;
        const CnsRead_MarshaledTransHistoryInfo::OneTransHistoryInfo* m_pCurr;
        friend class CnsRead_MarshaledTransHistoryInfo;
    };
    CnsRead_MarshaledTransHistoryInfo(tsp00_TaskId TaskId, tsp00_PageNo Pno);
    void Append (int Index, const tgg00_ObjTransInfo& TransHistoryInfo);
    inline Iterator Begin() const;
    inline static Iterator Begin(void* pSource);
    void Copy(void* pDest) const;
    inline int  Count() const;
    bool Read(SAPDBErr_MessageList& ErrorList);
    int  Size() const;
    bool Write(SAPDBErr_MessageList& ErrorList);
private :
    
    tsp00_TaskId     m_taskId;
    MarshalingBuffer m_buffer;
    friend class CnsRead_MarshaledTransHistoryInfo::Iterator;
};

//-----------------------------------------------------------------------------

inline void CnsRead_MarshaledTransHistoryInfo::Iterator::operator++()
{
    ++m_pCurr;
}

//-----------------------------------------------------------------------------

inline CnsRead_MarshaledTransHistoryInfo::Iterator::operator bool() const
{
    return m_pCurr < m_pMax;
}

//-----------------------------------------------------------------------------

inline int  CnsRead_MarshaledTransHistoryInfo::Iterator::BodyIndex() const
{
    return m_pCurr->m_index; 
}

//-----------------------------------------------------------------------------

inline const CnsRead_TransHistoryInfo& CnsRead_MarshaledTransHistoryInfo::Iterator::TransHistoryInfo() const
{
    return m_pCurr->m_transInfo; 
}

//-----------------------------------------------------------------------------

inline CnsRead_MarshaledTransHistoryInfo::Iterator CnsRead_MarshaledTransHistoryInfo::Begin() const
{
    return Iterator(&m_buffer.m_transInfo.m_transHistoryInfos[0], m_buffer.m_transInfo.m_cnt);
}

//-----------------------------------------------------------------------------

inline CnsRead_MarshaledTransHistoryInfo::Iterator CnsRead_MarshaledTransHistoryInfo::Begin(void* pSource)
{
    TransHistoryInfoArray* pInfoArray = reinterpret_cast<TransHistoryInfoArray*>(pSource);
    return Iterator(&pInfoArray->m_transHistoryInfos[0], pInfoArray->m_cnt);
}

//-----------------------------------------------------------------------------

inline int CnsRead_MarshaledTransHistoryInfo::Count() const
{
    return m_buffer.m_transInfo.m_cnt; 
}

#endif
