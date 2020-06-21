/*!***************************************************************************

  module:       CnsRead_TransHistoryTab.hpp

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

#ifndef CNSREAD_TRANSHISTORYTAB_HPP
#define CNSREAD_TRANSHISTORYTAB_HPP

#include "ggg00.h"
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "ConsistentRead/CnsRead_SeparatedRowHeader.hpp"
#include "ConsistentRead/CnsRead_TransHistoryInfo.hpp"

class CnsRead_CachedRow;
class CnsRead_TransHistoryTabPage;
class CnsRead_TransHistoryTabAllocator;
class Msg_List;

/*!
   @brief defines a transient structure containing a row header and information needed
          for the implementation of consistent read.
          A row lying in the data cache points to an instance of this class, if any changes
          have been made, which have to be noticed by concurrent transactions.
          Since the original header of the row is used to hold the pointer, the content of
          the header is paged out into the CnsRead_TransHistoryTab instance.
 */

class CnsRead_TransHistoryTab 
{
public :
    /*!
       @brief initializes CnsRead_TransHistoryTab allocator
     */
    static bool Initialize(Msg_List& MsgList);
     /*!
       @brief default constructor
     */
    CnsRead_TransHistoryTab() {}
    /*!
       @brief creates a trans history tab
    */
    static CnsRead_TransHistoryTab* Create (RTE_TaskId TaskId, const CnsRead_SeparatedRowHeader& rowHeader);
    /*!
       @brief creates a trans history tab
    */
    static CnsRead_TransHistoryTab* Create (RTE_TaskId TaskId, const CnsRead_TransHistoryTab& Source);
    /*!
       @brief creates a trans history tab
    */
    static CnsRead_TransHistoryTab* Create (const tgg00_TransContext& currentTrans, const CnsRead_SeparatedRowHeader& rowHeader);
    /*!
       @brief creates a trans history tab
    */
    static CnsRead_TransHistoryTab* Create(RTE_TaskId TaskId, const CnsRead_TransHistoryInfo& TransHistoryInfo, const CnsRead_SeparatedRowHeader& RowHeader); 
    /*!
       @brief deletes the trans history tab
    */
    void Delete(RTE_TaskId TaskId);
    /*!
       @brief returns the entry to the history, i.e. to the list of before images 
     */
    inline const tgg91_PageRef& GetBeforeRef() const; 
    /*!
       @brief returns the key length
     */
    inline int GetKeyLen() const;
    /*!
      @brief returns the row length
     */
    inline int GetRecLen() const;
#if defined(BIT64)
    /*!
      @brief returns the offset of columns of variable length
     */
    inline int GetVarColOffset() const;
    /*!
      @brief returns the number of columns of variable length
     */
    inline int GetVarColCount() const;
    /*!
       @brief return true if the deleted flag is set
     */
    inline bool IsDeleted() const;
    /*!
       @brief marks the header as deleted
     */
    inline void MarkDeleted(bool MarkDeleted);
#endif
    /*!
       @brief returns the transaction and history information
     */
    inline CnsRead_TransHistoryInfo& TransHistoryInfo();
    /*!
       @brief return true, if the current state of the row is consistent for the given transaction
     */
    bool IsConsistent(tgg00_TransContext& currentTrans);
    /*!
       @brief copies the paged out header back to the row
     */
    inline void RestoreHeader(CnsRead_SeparatedRowHeader& rowHeader);
    /*!
       @brief stores a given reference to a before image into the m_transInfo
     */
    inline void SetBeforeRef (const tgg91_PageRef& beforeRef);
    /*!
       @brief sets the record length
     */
    inline void SetRecLen(int RecLen);
    /*!
       @brief stores a given trans id into the m_transInfo
     */
    inline void SetUpdateTrans(const tgg91_TransNo& updTrans);
private :
     /*!
       @brief constructor
     */
    CnsRead_TransHistoryTab(const CnsRead_SeparatedRowHeader& rowHeader);
    /*!
       @brief constructor
     */
    CnsRead_TransHistoryTab(const tgg00_TransContext& currentTrans, const CnsRead_SeparatedRowHeader& rowHeader);
    /*!
       @brief constructor
     */
    CnsRead_TransHistoryTab(const CnsRead_TransHistoryInfo& TransHistoryInfo, const CnsRead_SeparatedRowHeader& RowHeader);
    CnsRead_SeparatedRowHeader  m_recordHeader;
    CnsRead_TransHistoryInfo    m_transInfo;
    //friend class CnsRead_TransHistoryTabPage::Entry;
    friend class CnsRead_TransHistoryTabAllocator;
};

inline const tgg91_PageRef& CnsRead_TransHistoryTab::GetBeforeRef() const
{
    return m_transInfo.GetBeforeRef();
}

inline int CnsRead_TransHistoryTab::GetKeyLen() const
{
    return m_recordHeader.GetKeyLen();
}

inline CnsRead_TransHistoryInfo& CnsRead_TransHistoryTab::TransHistoryInfo()
{
    return m_transInfo;
}

inline int CnsRead_TransHistoryTab::GetRecLen() const
{
    return m_recordHeader.GetRecLen();
}

#if defined (BIT64)
inline int CnsRead_TransHistoryTab::GetVarColOffset() const
{
     return m_recordHeader.GetVarColOffset();
}

inline int CnsRead_TransHistoryTab::GetVarColCount() const
{
    return m_recordHeader.GetVarColCount();
}

inline bool CnsRead_TransHistoryTab::IsDeleted() const
{
    return m_recordHeader.IsDeleted();
}

inline void CnsRead_TransHistoryTab::MarkDeleted(bool MarkDeleted)
{
    m_recordHeader.MarkDeleted(MarkDeleted);
}

#endif

inline void CnsRead_TransHistoryTab::RestoreHeader(CnsRead_SeparatedRowHeader& rowHeader)
{
    rowHeader = m_recordHeader;
}

inline void CnsRead_TransHistoryTab::SetBeforeRef (const tgg91_PageRef& beforeRef)
{
    m_transInfo.SetBeforeRef(beforeRef);
}

inline void CnsRead_TransHistoryTab::SetRecLen(int RecLen)
{
    m_recordHeader.SetRecLen(RecLen);
}

inline void CnsRead_TransHistoryTab::SetUpdateTrans(const tgg91_TransNo& updTrans)
{
    m_transInfo.SetUpdateTrans(updTrans);
}

#endif