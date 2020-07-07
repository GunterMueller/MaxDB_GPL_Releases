/*!
  @file           Data_PageSplitSpace.hpp
  @author         UweH
  @brief          Defines classes Data_PageSplitSpace,
                                  Data_PageSplitSpaceIterator,
                                  Data_PageSplitSpaceBackwardRead,
                                  Data_PageSplitSpaceBackwardReadIterator,
                                  Data_PageSplitSpaceForwardReadIterator

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
#ifndef Data_PageSplitSpace_HPP
#define Data_PageSplitSpace_HPP

#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "DataAccess/Data_ChainPage.hpp"
/*!
    @brief This iterator points to records in a page Data_PageSplitSpace.

    There can be only three states for the iterator.
    1. It is invalid: then it does not point to a valid offset within min and max. It cannot be dereferenced.
    2. If page is empty it can be dereferenced, but it returns NULL
    3. If page is empty it points to a record.
 */
class Data_PageSplitSpaceIterator
{
/// The page is a friend of the iterator.
friend class Data_PageSplitSpace;
public:
    /// default contructor sets all members to invalid
    Data_PageSplitSpaceIterator ()
    : m_RecordOffset (0),
      m_pFrame       (NULL)
    {}
    /// returns true, if it can be dereferenced.
    bool IsValid() const
    {
        return m_pFrame       != NULL                        &&
               m_RecordOffset >= Data_Page::MinSpaceOffset() &&
               m_RecordOffset <= Data_Page::MaxSpaceOffset() &&
               ! PageIsEmpty();
    }
    /// Resets all internal members. IsValid will return false.
    void Invalidate()
    {
        m_pFrame       = NULL;
        m_RecordOffset = 0;
    }
    /// The current position (offset in page) is returned.
    Data_PageOffset GetPosition () const
    {
        return m_RecordOffset;
    }
    /// return true, if no record is on the page.
    bool PageIsEmpty() const
    {
        return Data_Page::FirstFreeOffset(m_pFrame) == Data_Page::MinSpaceOffset();
    }
	/// return length of all Split parts of the record from current on.
    Data_SplitRecordLength RestLength ()
    {
        SAPDBERR_ASSERT_STATE( IsValid() && ! PageIsEmpty() );
        return LengthValue (m_RecordOffset);
    }
    /*!
        @brief This returns the record, which is currently referenced.

        If the iterator is not valid NULL is returned.
     */
    virtual SAPDB_Byte* operator * () = 0;
    /// Return length of current part of the Split record.
    virtual Data_RecordLength Length () = 0;
    /// return true, if the determined value of Length() fits into the current page
    virtual bool LengthIsValid() = 0;
private:
    /*!
        @brief The internal frame pointer is assigned and the internal record pointer is set.
        @param pPage [in] a pointer to a frame
        The Offset is set to the first record.
        If the page is empty, the iterator points to the first free space.
        In this case the iterator dereferences to NULL.
     */
    void Begin (SAPDB_Byte* pPage)
    {
        m_pFrame       = pPage;
        m_RecordOffset = Data_Page::MinSpaceOffset();
    }
    /*!
        @brief  The internal frame pointer is assigned and the internal record pointer is set.
        @param  pPage [in] a pointer to a frame

        The Offset is set to the last record.
        If the page is empty, the iterator points to the first free space.
        In this case the iterator dereferences to NULL.
     */
    void End (SAPDB_Byte* pPage)
    {
        m_pFrame       = pPage;
        m_RecordOffset = Data_Page::FirstFreeOffset(pPage);
    }
    /*!
        @brief Only the frame pointer is newly assigned.
        @param pPage [in] a new frame pointer
     */
    void ReAssign (SAPDB_Byte* pPage)
    {
        m_pFrame = pPage;
    }
    /*!
        @brief The internal frame and the current offset are assigned newly.
        @param pPage [in] a new frame pointer
        @param Offset [in] the new offset in the new frame.
     */
    void ReAssign (SAPDB_Byte*     pPage,
                   Data_PageOffset Offset)
    {
        m_pFrame       = pPage;
        m_RecordOffset = Offset;
        SAPDBERR_ASSERT_STATE( IsValid() );
    }
    /// returns the value at the position of the length field.
    virtual Data_SplitRecordLength LengthValue (Data_PageOffset Offset) = 0;
protected:
    /// A page pointer.
    SAPDB_Byte* m_pFrame;
    /// This refers to the current record: (m_pFrame + m_RecordOffset)
    Data_PageOffset m_RecordOffset;
};
/*!
    @class Data_PageSplitSpaceBackwardReadIterator
    @brief This iterator points to records in this page.
    There can be only three states for the iterator
    1. It is invalid: then it does not point to a valid offset within min and max. It cannot be dereferenced.
    2. If page is empty it can be dereferenced, but it returns NULL
    3. If page is empty it points to a record.
 */
class Data_PageSplitSpaceBackwardReadIterator : public Data_PageSplitSpaceIterator
{
public:
    /*!
        @brief  This returns the record, which is currently referenced.
        @return SAPDB_Byte*

        If the iterator is not valid NULL is returned.
     */
    virtual SAPDB_Byte* operator * ()
    {
        SAPDBERR_ASSERT_STATE( IsValid() );
        if ( PageIsEmpty() )
            return NULL;
        else
            return m_pFrame + m_RecordOffset - Length();
    }
    /// returns the Length of current part of the Split record.
    virtual Data_RecordLength Length ()
    {
        SAPDBERR_ASSERT_STATE( IsValid() && ! PageIsEmpty() );
        
        const Data_SplitRecordLength RestRecordLength
            = RestLength();
            
        const Data_SplitRecordLength RestSpaceInPage
            = m_RecordOffset - Data_Page::MinSpaceOffset();

        if ( RestSpaceInPage >= RestRecordLength )
            return RestRecordLength;
        else
            return RestSpaceInPage;
    }
    /// returns true, if the determined value of Length() fits into the current page
    virtual bool LengthIsValid()
    {
        // PTS 1113270 UH 2001-12-28 new
        
        if ( ! IsValid() )
            return false;
        
        const SAPDB_UInt testoffset = m_RecordOffset - Length();
        
        return testoffset >= Data_Page::MinSpaceOffset()
               &&
               testoffset <= Data_Page::MaxSpaceOffset();
    }
    /// The iterator is set to the previous record.
    Data_PageSplitSpaceBackwardReadIterator& operator -- ()
    {
        if ( m_RecordOffset - Length() <= Data_Page::MinSpaceOffset() )
            Invalidate();
        else
            m_RecordOffset -= Length();
        return *this;
    }
private:
    /*!
        @brief  returns the value written at the length position
        @param  Offset [in] 
        @return Length of given record.
     */
    virtual Data_SplitRecordLength LengthValue (Data_PageOffset Offset)
    {
        return *(REINTERPRET_CAST(Data_SplitRecordLength*,(m_pFrame + Offset - sizeof (Data_SplitRecordLength))));
    }
};
/*!
	@class          Data_PageSplitSpaceForwardReadIterator
    @brief          This iterator points to records in this page.
    There can be only three states for the iterator
    1. It is invalid: then it does not point to a valid offset within min and max. It cannot be dereferenced.
    2. If page is empty it can be dereferenced, but it returns NULL
    3. If page is empty it points to a record.
 */
class Data_PageSplitSpaceForwardReadIterator : public Data_PageSplitSpaceIterator
{
public:
    /*!
        @brief  This returns the record, which is currently referenced.
        @return SAPDB_Byte*
        If the iterator is not valid NULL is returned.
     */
    virtual SAPDB_Byte* operator * ()
    {
        SAPDBERR_ASSERT_STATE( IsValid() );
        if ( PageIsEmpty() )
            return NULL;
        else
            return m_pFrame + m_RecordOffset + sizeof(Data_SplitRecordLength);
    }
    /// returns the length of current part of the Split record.
    virtual Data_RecordLength Length ()
    {
        SAPDBERR_ASSERT_STATE( IsValid() && ! PageIsEmpty() );
        const Data_SplitRecordLength RestLen = RestLength();

        if ( m_RecordOffset + RestLen >= Data_Page::MaxSpaceOffset() )
            return Data_Page::MaxSpaceOffset() - m_RecordOffset;
        
        return RestLen;
    }
    /// returns true, if the determined value of Length() fits into the current page
    virtual bool LengthIsValid()
    {
        // PTS 1113270 UH 2001-12-28 new
        
        if ( ! IsValid() )
            return false;
        
        const SAPDB_UInt testoffset = m_RecordOffset + Length();
        
        return testoffset >= Data_Page::MinSpaceOffset()
               &&
               testoffset <= Data_Page::MaxSpaceOffset();
    }
    /// The iterator is set to the next record.
    Data_PageSplitSpaceForwardReadIterator& operator ++ ()
    {
        if ( m_RecordOffset + Length() >= Data_Page::FirstFreeOffset(m_pFrame) )
            Invalidate();
        else
            m_RecordOffset += Length();
        return *this;
    }
private:
    /// Returns the value at length position.
    virtual Data_SplitRecordLength LengthValue (Data_PageOffset Offset)
    {
        return *(REINTERPRET_CAST(Data_SplitRecordLength*,(m_pFrame + Offset)));
    }
};
/*!
	@class Data_PageSplitSpace
    @brief The page can store Split records.
 */
class Data_PageSplitSpace : public Data_ChainPage
{
public:
    /// This destructor can call verify on error abort.
    ~Data_PageSplitSpace();
    /// Initializes all used fields in the frame.
    virtual void InitializeFrame ()
    {
        Data_ChainPage::InitializeFrame();
        NodePtr()->nd_left() = SAPDB_MAX_INT4;
    }
    /// The RecordSize is properly aligned.
    static Data_SplitRecordLength AlignRecordSize (Data_SplitRecordLength Length)
    {
        return ( (Length + sizeof(Data_SplitRecordLength) - 1)
                 / sizeof(Data_SplitRecordLength) ) * sizeof(Data_SplitRecordLength);
    }
    /*!
        @brief  The page id of the prev page is returned.
        @return PageNo
        If no prev page is available, PageId.IsValid() returns false.
     */
    Data_PageNo PrevPageNo ()
    {
        SAPDBERR_ASSERT_STATE(IsAssigned());
        return STATIC_CAST(Data_PageNo,NodePtr()->nd_left());
    }
    /// Write the previous page no into the page frame.
    void SetPrevPageNo (const Data_PageNo& PageNo)
    {
        SAPDBERR_ASSERT_STATE(IsAssigned());
        NodePtr()->nd_left() = (tsp00_PageNo)PageNo;
    }
	/// Append the given page to the current page
	bool Append (Data_PageSplitSpace&  NewPage,
                 SAPDBErr_MessageList &errlist)
    {
        if ( ! Data_ChainPage::Append (NewPage, errlist) )
            return false;

        NewPage.SetPrevPageNo (PageNo());

        if ( NewPage.NextPageNo().IsValid() )
        {
            // THIS<->NEW<->NEXT
            Data_PageSplitSpace NextPage;
            
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
    /// The iterator should now use this page with its current settings.
    void Use (Data_PageSplitSpaceIterator& Iter)
    {
        Iter.ReAssign (FramePtr());
    }
    /// The iterator should now use this page with its current settings.
    void Use (Data_PageSplitSpaceIterator& Iter,
              Data_PageOffset          Offset)
    {
        Iter.ReAssign (FramePtr(), Offset);
    }
    /// The iterator is set to first record on this page.
    void Begin (Data_PageSplitSpaceIterator& Iter)
    {
        Iter.Begin (FramePtr());
    }
    /// The iterator is set to the first free record on this page.
    void End (Data_PageSplitSpaceIterator& Iter)
    {
        Iter.End (FramePtr());
    }
    /// This is used to get the correct page locks.
    void SpaceInfo (Data_SplitRecordLength  WantedSize,
                    Data_RecordLength       MinimumSize,
                    bool                   &FirstPartFits,
                    bool                   &AllFits);
    /*!
        @brief The changes done by the last reserve are un-done.
        @param LastReservedSize [in] - the size which was reserved before
     */
    void UndoLastReservedSpace (Data_RecordLength LastReservedSize)
    {
        // PTS 1115979 UH 2002-05-29 new
        if ( 0 == LastReservedSize ) return;
        

        SetFirstFreeOffset ( Data_Page::FirstFreeOffset(this->FramePtr())
                             -
							 // PTS 1117126 UH 2002-08-07 added alignment
                             (AlignRecordSize(LastReservedSize)
                              + sizeof(Data_SplitRecordLength)) );
    }
    /*!
        @brief Everything from this position on is deleted.
        @param iter [in/out] points to the new first free position.
     */
    void Truncate (Data_PageSplitSpaceIterator& iter)
    {
        Data_Page::SetFirstFreeOffset( iter.GetPosition() );
    }
    /// returns the alignment
    static SAPDB_UInt Alignment()
    {
        return sizeof(Data_SplitRecordLength);
    }
};
/*!
    @class Data_PageSplitSpaceBackwardRead
    @brief This page is optimized for backward reading (undo entries)
 */
class Data_PageSplitSpaceBackwardRead : public Data_PageSplitSpace
{
public:
    /// Writes important values from this page to the knltrace
    virtual void WriteToTrace (const SAPDB_Char *title = 0) const;
    /*!
        @brief  Verifies consistency of page contents
        @return true if verify successful
     */
    virtual SAPDB_Bool Verify(bool                  isAdmin,
                              SAPDBErr_MessageList &errlist) const;
    /*!
       @brief The given WantedSize is used to allocate space.
       @param ReservedSize [in] - the size which is already reserved starting with 0
       @param WantedSize [in] - the size which is wanted
       @param MinimumSize [in] - the size which must be available
       @param CurrentReservedSize [in/out] - the size which is allocated
       @param CurrentSpace [out] - the pointer to the reserved space
       If CurrentReservedSize is 0 then no space is available in current page.
       Precondition: page is assigned
     */
    void ReserveSpace (Data_SplitRecordLength &ReservedSize,
                       Data_SplitRecordLength &WantedSize,
                       Data_RecordLength       MinimumSize,
                       Data_RecordLength      &CurrentReservedSize,
                       SAPDB_Byte*            &CurrentSpace);
    /*!
        @brief Return the offset of the given part ptr

        This depends on the page type (backward-read/forward-read)
     */
    Data_PageOffset GetOffset (SAPDB_Byte* partptr,
                               SAPDB_UInt  partlength,
                               bool        bCheck = true) const;

    Data_PageOffset GetOffset (SAPDB_Byte* Reference) const
    {
        return Data_Page::GetOffset(Reference);
    }
};
/*!
    @class Data_PageSplitSpaceForwardRead
    @brief This page is optimized for forward reading (redo entries)
 */
class Data_PageSplitSpaceForwardRead : public Data_PageSplitSpace
{
public:
    /// Writes important values from this page to the knltrace
    virtual void WriteToTrace (const SAPDB_Char *title = 0) const;
    /*!
        @brief  Verifies consistency of page contents
        @return true if verify successful
     */
    virtual SAPDB_Bool Verify(bool                  isAdmin,
                              SAPDBErr_MessageList &errlist) const;
    /*!
        @brief The given WantedSize is used to allocate space.
        @param WantedSize [in] - the size which is wanted
        @param FirstPartMiniumSize [in] - if set to 0, no minimum is given
        @param CurrentReservedSize [in/out] - the size which is allocated
        @param CurrentSpace [out] - the pointer to the reserved space
        If CurrentReservedSize is 0 then no space is available in current page.
        Precondition: page is assigned
     */
    void ReserveSpace (Data_SplitRecordLength &WantedSize,
                       Data_RecordLength       FirstPartMiniumSize,
                       Data_RecordLength      &CurrentReservedSize,
                       SAPDB_Byte*            &CurrentSpace);
    /*!
        @brief          Return the offset of the given part ptr

        This depends on the page type (backward-read/forward-read)
     */
    Data_PageOffset GetOffset (SAPDB_Byte* partptr,
                               SAPDB_UInt  partlength,
                               bool        bCheck = true) const;

    Data_PageOffset GetOffset (SAPDB_Byte* Reference) const
    {
        return Data_Page::GetOffset(Reference);
    }
};
#undef ALIGNMENT
#endif // Data_PageSplitSpace_HPP
