/*!
  @file           Data_PageFixSizeSpace.hpp
  @author         UweH
  @brief          Defines classes Data_PageFixSizeSpace,

\if EMIT_LICENCE
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

\endif
*/
#ifndef Data_PageFixSizeSpace_HPP
#define Data_PageFixSizeSpace_HPP

#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "DataAccess/Data_ChainPage.hpp"
/*!
   @class Data_PageFixSizeSpace
   @brief The page can store Split records.
 */

class Data_PageFixSizeSpace : public Data_ChainPage
{
public:
    /// fix record length
    typedef SAPDB_UInt2 FixRecordLength;
    /// record count per page
    typedef SAPDB_UInt4 RecordCount;
public:
    /*!
        @class Iterator
        @brief This iterator points to records in a page Data_PageFixSizeSpace.
     */
    class Iterator
    {
    /// The page is the friend of its iterator.
    friend class Data_PageFixSizeSpace;
    public:
        /// Default constructor which initializes all values.
        Iterator ()
        : m_pFrame       (NULL),
          m_RecordOffset (0),
          m_RecordSize   (0)
        {}
        /// returns true, if it can be dereferenced.
        bool IsValid() const
        {
            return m_pFrame                    != NULL                         &&
                   m_RecordOffset              >= Data_Page::MinSpaceOffset () &&
                   m_RecordOffset+m_RecordSize <= Data_Page::MaxSpaceOffset () &&       // PTS 1114647 UH 2002-03-05
                   m_RecordOffset+m_RecordSize <= Data_Page::FirstFreeOffset(m_pFrame); // PTS 1114647 UH 2002-03-05
        }
        /// Resets all internal members. IsValid will return false.
        void Invalidate()
        {
            m_pFrame       = NULL;
            m_RecordOffset = 0;
            m_RecordSize   = 0;
        }
        /// Returns current position
        Data_PageOffset GetPosition () const
        {
            return m_RecordOffset;
        }
        /// if the iter is valid the current record id is returned. otherwise maxuin4
        RecordCount GetRecordId () const
        {
            if ( ! IsValid() )
                return SAPDB_MAX_UINT4;
            SAPDBERR_ASSERT_STATE( (m_RecordOffset - Data_Page::MinSpaceOffset()) % m_RecordSize == 0 );
            return ((m_RecordOffset-Data_Page::MinSpaceOffset()) / m_RecordSize)+1;
        }
        /// set record id
        void SetRecordId (RecordCount recordid)
        {
            m_RecordOffset = Data_PageFixSizeSpace::Offset (recordid, m_RecordOffset);
            SAPDBERR_ASSERT_STATE( IsValid() );
        }
        /// This returns the record, which is currently referenced.

        /// If the iterator is not valid NULL is returned.
        SAPDB_Byte* operator * ()
        {
            if ( ! IsValid() )
                return NULL;
            else
                return m_pFrame + m_RecordOffset;
        }
        /*!
            @brief Sets the iterator to the next record in page.

            If it points after end or before begin it is invalid.
         */
        Iterator& operator ++ ()
        {
            SAPDBERR_ASSERT_STATE( IsValid() );
            m_RecordOffset += m_RecordSize;
            return *this;
        }
        /*!
            @brief Sets the iterator to the next record in page.

            If it points after end or before begin it is invalid.
         */
        Iterator& operator -- ()
        {
            SAPDBERR_ASSERT_STATE( IsValid() );
            m_RecordOffset -= m_RecordSize;
            return *this;
        }
    protected:
        /// A page pointer.
        SAPDB_Byte* m_pFrame;
        /// This refers to the current record: (m_pFrame + m_RecordOffset)
        Data_PageOffset m_RecordOffset;
        /// The fix record length in the page.
        Data_RecordLength m_RecordSize;
    };
public:
    /// Initialize all used entries on the frame.
    virtual void InitializeFrame ()
    {
        SAPDBERR_ASSERT_STATE( IsAssigned() );
        Data_ChainPage::InitializeFrame();
        NodePtr()->nd_left() = SAPDB_MAX_INT4;
        SetFixRecordLength (0);
        SetRecordCount     (0);
    }
    /// return length of record in the page
    FixRecordLength GetFixRecordLength () const
    {
        SAPDBERR_ASSERT_STATE( IsAssigned() );
        return NodePtr()->ndPageFixSizeSpace_bd00().fixRecordLength;
    }
    /// Write the fix space length into the page frame
    void SetFixRecordLength (FixRecordLength Length)
    {
        SAPDBERR_ASSERT_STATE( IsAssigned() );
        FixRecordLength* aux
            = REINTERPRET_CAST(FixRecordLength*,&(NodePtr()->ndPageFixSizeSpace_bd00().fixRecordLength));
        *aux = Length;
    }
    /// returns the actual used record count.
    RecordCount GetRecordCount () const
    {
        SAPDBERR_ASSERT_STATE( IsAssigned() );
        return NodePtr()->nd_record_cnt();
    }
    /// Change the record count in the frame.
    void DecrementRecordCount ()
    {
        SAPDBERR_ASSERT_STATE( IsAssigned() );
        --(NodePtr()->nd_record_cnt());
        if ( GetRecordCount() == 0 )
            SetFirstFreeOffset (Data_Page::MinSpaceOffset());
    }
    /*!
        @brief The page id of the prev page is returned.

        If no prev page is available, PageId.IsValid() returns false.
     */
    Data_PageNo PrevPageNo ()
    {
        SAPDBERR_ASSERT_STATE(IsAssigned());
        return STATIC_CAST(Data_PageNo,NodePtr()->nd_left());
    }
    /// Write the previous page no into the frame
    void SetPrevPageNo (const Data_PageNo& PageNo)
    {
        SAPDBERR_ASSERT_STATE(IsAssigned());
        NodePtr()->nd_left() = (tsp00_PageNo)PageNo;
    }
    /*!
        @brief The given new page is appended to the current page.
        
        If the new page has no next page then LastPageNo is set to this.
        The chain is double linked.
     */
    bool Append (Data_PageFixSizeSpace &NewPage,
                 SAPDBErr_MessageList  &errlist)
    {
        if ( ! Data_ChainPage::Append (NewPage, errlist) )
            return false;

        NewPage.SetPrevPageNo (PageNo());

        if ( NewPage.NextPageNo().IsValid() )
        {
            // THIS<->NEW<->NEXT
            Data_PageFixSizeSpace NextPage;
            
            if ( ! m_pAccessManager->GetPage (NextPage,
                                              NewPage.NextPageNo(),
                                              errlist) )
            {
                return false;
            }
            NextPage.SetPrevPageNo( NextPageNo() );
        }
        return true;
    }
    /*!
        @brief The iterator is set to the specified record on this page.

        If RecordId is not in the valid range, then the iterator becomes invalid.
     */
    void SetToRecord (RecordCount RecordId,
                      Iterator&   Iter)
    {
        SAPDBERR_ASSERT_STATE( IsAssigned() );
        SAPDBERR_ASSERT_STATE( Iter.m_pFrame == FramePtr() );
        Iter.m_RecordOffset = Offset (RecordId, GetFixRecordLength());
    }
    /// The iterator is set to first record on this page.
    void Begin (Iterator& Iter)
    {
        Iter.m_pFrame     = FramePtr();
        Iter.m_RecordSize = GetFixRecordLength();
        SetToRecord (1, Iter);
    }
    /// The iterator is set to last record on this page.
    void End (Iterator& Iter)
    {
        Iter.m_pFrame       = FramePtr();
        Iter.m_RecordSize   = GetFixRecordLength();
        Iter.m_RecordOffset = FirstFreeOffset(Iter.m_pFrame) - GetFixRecordLength();
    }
    /// space is reserved on current page and the iter is set to it.
    /// If no space is available the iter becomes invalid.
    void ReserveSpace (Iterator& Iter)
    {
        SAPDBERR_ASSERT_STATE( IsAssigned() && GetFixRecordLength() > 0 );

        Iter.Invalidate();
        
        const FixRecordLength LengthToReserve = GetFixRecordLength();
        
        if ( LengthToReserve  > FreeSpace() )
            return;

        RecordCount aux = GetRecordCount();
        ++aux;
        SetRecordCount(aux);

        Data_Page::ReserveSpace (LengthToReserve);

        End (Iter);
    }
    /*!
        @brief  This returns the offset of a give record id.
        @return Offset to given RecordId.
     */
    static Data_PageOffset Offset (RecordCount     RecordId,
                                   FixRecordLength RecordLength)
    {
        return Data_Page::MinSpaceOffset() + (RecordLength * (RecordId-1));
    }
private:
    /// Write the count of records to the page frame.
    void SetRecordCount (RecordCount Count)
    {
        SAPDBERR_ASSERT_STATE( IsAssigned() );
        RecordCount* aux
            = REINTERPRET_CAST(RecordCount*,&(NodePtr()->nd_record_cnt()));
        *aux = Count;
    }
};
#undef ALIGNMENT
#endif // Data_PageFixSizeSpace_HPP