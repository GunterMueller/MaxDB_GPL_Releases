
/*!***************************************************************************

  module:       CnsRead_CachedRow.hpp

  responsible : ThomasA

  special area: Consistent Read

  description:  represents a row of a sql table whisch is lying in a page in the data cache

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


#ifndef CNSREAD_CACHEDROW_HPP
#define CNSREAD_CACHEDROW_HPP
#include "ConsistentRead/CnsRead_SwizzledPointer.hpp"
#include "ConsistentRead/CnsRead_TransHistoryTab.hpp"
#include "ConsistentRead/CnsRead_SeparatedRowHeader.hpp"

/*!
   @brief represents a row of a sql table when located in the data cache. Each row consists of
          a row header followed by the key and the non key columns.
          The row header may be substituted by a swizzled pointer to a structure containing
          the real row header and transaction an history information. This information is needed
          for consistent read implementaion
 */
class CnsRead_CachedRow
{
public :
    /*!
       @brief creates a transaction and history information for the row. The row header is substituted
              by a swizzled pointer to a structure containing 
              the real row header and transaction and history information.
    */
    bool CreateTransHistoryInfo(const tgg00_TransContext& CurrentTrans, tgg91_PageRef& BeforeRef);
    bool CreateTransHistoryInfo(RTE_TaskId TaskId, const CnsRead_TransHistoryInfo& TransHistoryInfo);
    /*!
       @brief copies the row into a buffer. The row header is reconstructed using the swizzled pointer, if
              necessary. If required, the transaction and history information is copied to the
              left of the record.
     */
    void Copy (bool withTransHistory, SAPDB_Int4 bufSize, void* destBuf, SAPDB_Int4& destOffset, tgg00_BasisError& e) const;
    /*!
       @brief copies the row into a buffer. The row header is reconstructed using the swizzled pointer, if
              necessary. If required, the transaction and history information is copied to the
              left of the record.
     */
    void Copy (CnsRead_TransHistoryInfo& TransHistoryInfo, SAPDB_Int4 bufSize, void* destBuf, tgg00_BasisError& e) const;
    /*!
       @brief copies the current trans history tab to the destination row
     */
    bool CopyTransHistoryTab (RTE_TaskId TaskId, CnsRead_CachedRow& DestinationRow);
    /*!
       @brief adds or removes deleted flag from current row 
     */
    void Delete (tgg00_TransContext& currentTrans, const tgg91_PageRef& beforeRef, const tgg91_TransNo& updTrans, bool markDeleted);

    /*
      @brief deletes the trans history information of the row, if present
     */
    inline void DeleteTransHistoryInfo(RTE_TaskId TaskId);
    /*!
      @brief checks whether the row fulfills a given qualification for delete and update row
     */
    void DelUpdTestQualification (
        tgg00_MessBlock &MessBlock,
        bool             FirstQual,
        bool             ResultWanted,
        bool             CheckNewRow,
        void*            pResult,
        SAPDB_Int4       ResultSize,
        int             &ResultLen);
    /*!
       @brief checks whether the row fulfills a given qualification
     */
    bool IsQualified (
        tgg00_TransContext&      currentTrans,
        tgg00_SelectFieldsParam& interpreterContext,
        tgg00_StackDesc&         qualification );
    /*!
       @brief returns the key length
     */
    inline int GetKeyLen() const;
    /*!
       @brief returns the key length
     */
    inline static int GetKeyLen(const tgg00_Rec* pRec);
    /*!
       @brief returns then row length
     */
    inline int GetRecLen() const;
    /*!
       @brief returns the record length
     */
    inline static int GetRecLen(const tgg00_Rec* pRec);

    inline static void GetRecAndKeyLen (const tgg00_Rec* pRec, int& RecLen, int& KeyLen);

    inline const CnsRead_SeparatedRowHeader& GetSeparatedRowHeader() const;
    /*!
      @brief returns a pointer to the trans and history information of the row or the 0 pointer, if information does not exist.
    */
    const CnsRead_TransHistoryInfo* GetTransHistoryInfo() const;
    /*!
       @brief returns the offset of columns of variable length
     */
    inline int GetVarColOffset() const;
    /*!
       @brief returns the number of columns of variable length
     */
    inline int GetVarColCount() const;
    /*!
       @brief returns true, if the is marked as deleted. Caution : the row must not contain
             a swizzled pointer !
     */
    inline bool IsDeleted() const;
    /*!
       @brief releases the trans history info of the row (if preset). Memory is returned to the corresponding allocator
     */
    void ReleaseTransHistoryInfo (RTE_TaskId TaskId);
    /*!
       @brief restores the row header, if the header is a swizzled pointer
       @param releaseTransHistoryInfo if true, the trans history info is no longer used and will be released
     */
    void RestoreRowHeader(RTE_TaskId TaskId, bool releaseTransHistoryInfo);

    inline void SetSeparatedRowHeader(const CnsRead_SeparatedRowHeader& header);
	/*!
       @brief sets and updates the trans and history information 
     */
    bool UpdateTransHistoryInfo(RTE_TaskId TaskId, const CnsRead_CachedRow& oldImage, const tgg91_PageRef& beforeRef, const tgg91_TransNo& updTrans);
#if defined (SUN)
public : // otherwise sun compiler will not allow usage in union below
#else
private :
#endif
    struct RowHeader 
    {
        CnsRead_SeparatedRowHeader m_separatableHeader; //!< header part that may be overwritten by swizzled pointer
#if !defined(BIT64)
        SAPDB_Int2                 m_varColOffset;      //!< on 32 bit platforms this part will not be overwritten by a pointer
        SAPDB_Int2                 m_varColCount;       //!< on 32 bit platforms this part will not be overwritten by a pointer
#endif
    };
private :
    /*!
       @brief defines the row header which is either a swizzled pointer or a header
              containing row length, key length, offset of variable columns and
              number of variable columns.
    */
    union
    {
        CnsRead_SwizzledPointer<CnsRead_TransHistoryTab> m_transHistoryPointer;
        RowHeader                                        m_recordHeader;
    };
    /*!
       @brief returns true, if the is marked as deleted. Caution : the row must contain
             a swizzled pointer !
     */
    inline bool IsDeleted(CnsRead_TransHistoryTab*) const;
    /*!
       @brief determines the valid view of the row for the given transaction. This view is
              copied into the given buffer (consistentImage).
     */
    bool GetConsistentImage (tgg00_TransContext& currentTrans, const tgg91_PageRef& beforeRef, tgg00_Rec& consistentImage);

};

//-----------------------------------------------------------------------------

inline void CnsRead_CachedRow::DeleteTransHistoryInfo(RTE_TaskId TaskId)
{
    this->RestoreRowHeader(TaskId, true);
}

//-----------------------------------------------------------------------------

inline int CnsRead_CachedRow::GetKeyLen() const
{
    CnsRead_TransHistoryTab* p = m_transHistoryPointer.GetPointer();
    if (p)
    {
        return p->GetKeyLen();
    }
    else
    {
        return m_recordHeader.m_separatableHeader.GetKeyLen();
    }
}

//-----------------------------------------------------------------------------

inline int CnsRead_CachedRow::GetKeyLen(const tgg00_Rec* pRec)
{
    return reinterpret_cast<const CnsRead_CachedRow*>(pRec)->GetKeyLen();
}

//-----------------------------------------------------------------------------

inline int CnsRead_CachedRow::GetRecLen() const
{
    CnsRead_TransHistoryTab* p = m_transHistoryPointer.GetPointer();
    if (p)
    {
        return p->GetRecLen();
    }
    else
    {
        return m_recordHeader.m_separatableHeader.GetRecLen();
    }
}

//-----------------------------------------------------------------------------

inline int CnsRead_CachedRow::GetRecLen(const tgg00_Rec* pRec)
{
    return reinterpret_cast<const CnsRead_CachedRow*>(pRec)->GetRecLen();
}

//-----------------------------------------------------------------------------

inline void CnsRead_CachedRow::GetRecAndKeyLen (const tgg00_Rec* pRec, int& RecLen, int& KeyLen)
{
    const CnsRead_CachedRow* pRow = reinterpret_cast<const CnsRead_CachedRow*>(pRec);
    CnsRead_TransHistoryTab* p = pRow->m_transHistoryPointer.GetPointer();
    if (p)
    {
        RecLen = p->GetRecLen();
        KeyLen = p->GetKeyLen();
    }
    else
    {
        RecLen = pRow->m_recordHeader.m_separatableHeader.GetRecLen();
        KeyLen = pRow->m_recordHeader.m_separatableHeader.GetKeyLen();
    }
}

//-----------------------------------------------------------------------------

inline const CnsRead_SeparatedRowHeader& CnsRead_CachedRow::GetSeparatedRowHeader() const
{
    return m_recordHeader.m_separatableHeader;
}

//-----------------------------------------------------------------------------

inline int CnsRead_CachedRow::GetVarColOffset() const
{
#if defined(BIT64)
    CnsRead_TransHistoryTab* p = m_transHistoryPointer.GetPointer();
    if (p)
    {
        return p->GetVarColOffset();
    }
    else
    {
        return m_recordHeader.m_separatableHeader.GetVarColOffset();
    }
#else
    return m_recordHeader.m_varColOffset;
#endif
}

//-----------------------------------------------------------------------------

inline int CnsRead_CachedRow::GetVarColCount() const
{
#if defined(BIT64)
    CnsRead_TransHistoryTab* p = m_transHistoryPointer.GetPointer();
    if (p)
    {
        return p->GetVarColCount();
    }
    else
    {
        return m_recordHeader.m_separatableHeader.GetVarColCount();
    }
#else
    return m_recordHeader.m_varColCount;
#endif
}

//-----------------------------------------------------------------------------

inline bool CnsRead_CachedRow::IsDeleted() const
{
#if defined(BIT64)
    return m_recordHeader.m_separatableHeader.IsDeleted();
#else
    return ((m_recordHeader.m_varColCount & 0x4000) > 0);
#endif
}

//-----------------------------------------------------------------------------

inline bool CnsRead_CachedRow::IsDeleted(CnsRead_TransHistoryTab* pTransHistory) const
{
#if defined BIT64
    return pTransHistory->IsDeleted();
#else
    return ((m_recordHeader.m_varColCount & 0x4000) > 0);
#endif
}

//-----------------------------------------------------------------------------

inline void CnsRead_CachedRow::SetSeparatedRowHeader(const CnsRead_SeparatedRowHeader& header)
{
    m_recordHeader.m_separatableHeader = header;
}

#endif