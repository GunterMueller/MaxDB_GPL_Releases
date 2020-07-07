/*!
  @file           Converter_LeafPage.hpp
  @author         TorstenS
  @author         TillL
  @ingroup        Converter
  @brief          Handler class for converter leaf pages
*/


/*
  ========== licence begin  GPL
  Copyright (c) 2000-2004 SAP AG

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

#ifndef CONVERTER_LEAFPAGE_HPP
#define CONVERTER_LEAFPAGE_HPP


#include "Converter/Converter_Page.hpp"
#include "SAPDBCommon/Fields/SAPDBFields_Field.hpp"

class Converter_BlockAddressIterator;
class Converter_LeafPageEntryIteratorDrop;

/*!
   @class          Converter_LeafPage
   @brief          Specialized converter page class for leaf pages (concrete class)

   Call static method 'SetDevNoBitCount()' once prior to first usage
   of this class.
 */
class Converter_LeafPage : public Converter_Page
{
    friend class Converter_MapEntry;
    friend class Converter_LeafPageEntryIteratorDrop;

public:

    /* -----------------------------------------------------------------------*/

    /*! @name Constructors and initialization */

    /*!
       @brief          Default constructor
       @return         none
     */
    Converter_LeafPage();

    /*!
       @brief          Constructor; assigns frame
                       Does not change frame contents.
       @param          frame [in] page frame
       @return         none
     */
    Converter_LeafPage
        (const PageFrame&               frame);

    /*!
       @brief          Constructor; assigns frame and initializes frame contents
       @param          frame [in] page frame
       @param          version [in] converter version
       @param          parent [in] reference to parent in converter tree parent index
       @param          basepageno [in] base pageno
       @param          rec [in] page recovery mode
       @return         none
     */
    Converter_LeafPage
        (const PageFrame&               frame,
         const Version&                 version,
         const Converter_ParentReference&         parent,
         const Data_PageNo&             basepageno,
         const Data_PageRecoveryMode&   rec);

    /*!
       @brief          Assigns frame and initializes frame contents
       @param          frame [in] page frame
       @param          version [in] converter version
       @param          parent [in] reference to parent in converter tree parent index
       @param          basepageno [in] base pageno
       @param          rec [in] page recovery mode
       @return         none
     */
    void SetAndInitializeFrame
        (const PageFrame&               frame,
         const Version&                 version,
         const Converter_ParentReference&         parent,
         const Data_PageNo&             basepageno,
         const Data_PageRecoveryMode&   rec);

    /*!
       @brief          Initializes frame contents
       @param          version [in] converter version
       @param          parent [in] reference to parent in converter tree parent index
       @param          basepageno [in] base pageno
       @param          rec [in] page recovery mode
       @return         none
    */
    void InitializeFrame
        (const Version&                 version,
         const Converter_ParentReference&         parent,
         const Data_PageNo&             basepageno,
         const Data_PageRecoveryMode&   rec);

    /* -----------------------------------------------------------------------*/

    /*! @name Frame access */

    /*!
       @brief          Reads base pageno from frame
       @return         (Data_PageNo) base data pageno
     */
    Data_PageNo ReadBasePageNo() const;

    /*!
       @brief          Writes recovery mode to frame
       @param          rec [in] page recovery mode
       @return         none
     */
    void WritePageRecoveryMode
        (const Data_PageRecoveryMode&   rec);

    /*!
       @brief          Reads from frame: recovery mode of this converter leaf page
       @return         (Data_PageRecoveryMode) page recovery mode
     */
    Data_PageRecoveryMode ReadPageRecoveryMode() const;

    /*!
       @brief          Checks whether there are reserved entries on the converter page
       @return         (SAPDB_Bool) true if there are reserved entries
     */
    SAPDB_Bool HasReservedEntries() const;

    /*!
       @brief          Reads reserved entry count from frame
       @return         (PageEntryCount) reserved entry count
     */
    PageEntryCount ReadReservedEntryCount() const;

    /*!
       @brief          Scans whole page for reserved entries and rechains them into
                       the free entry chain
       @return         (PageEntryCount) number of reserved page entries found

       - This routine is useful when recovering a database crash.
       - There may be entries on a page which have been reserved
         and marked as used, but are in fact unused. These must be
         rechained and marked as unused.
     */
    PageEntryCount ReintegrateReservedEntries();

    /*!
       @brief          Returns total count of entries this page
       @return         (Converter_PageEntryCount) entry count
     */
    virtual PageEntryCount GetEntryCount() const;

    /*!
       @brief          Returns total count of entries this page
       @return         (Converter_PageEntryCount) entry count
     */
    static PageEntryCount GetEntryCount (SAPDB_UInt LeafPageSize);

    /*!
       @brief          Returns field handle to entire in-use flags block
       @return         (SAPDBFields_Field) field
     */
    SAPDBFields_Field GetInUseFlagsBlock() const;

    /*!
       @brief           Returns an iterator, which provides all entries
                        linked with a valid block address. Reserved entries
                        are not given, because they have no block address.
                        
       @return         (Converter_BlockAddressIterator)
    */
    Converter_BlockAddressIterator GetUsedBlockIterator();

    /*!
       @brief           Returns an iterator, which provides all entries
                        linked with a valid block address on a given volume
                        
       @return         (Converter_LeafPageEntryIteratorDrop)
    */

    Converter_LeafPageEntryIteratorDrop BeginDrop(const IOMan_DeviceNo volNo);

    /*!
       @brief           Returns the end of an iterator, which provides all entries
                        linked with a valid block address on a given volume
                        
       @return         (Converter_LeafPageEntryIteratorDrop)
    */
    Converter_LeafPageEntryIteratorDrop EndDrop(const IOMan_DeviceNo volNo);

    /// Verifies consistency of page contents, returns true if verify is successful
    virtual SAPDB_Bool Verify(bool                  isAdmin,
                              SAPDBErr_MessageList &errlist) const;

    /*!
        @brief  Checks page after reading from volume
        @return (SAPDB_Bool) true if check successful
    */
    virtual SAPDB_Bool CheckAfterRead() const;

    /* -----------------------------------------------------------------------*/

    /*! @name Entry access */

    /* -----------------------------------------------------------------------*/

    /*!
       @brief          Checks whether specified entry is reserved
       @return         (SAPDB_Bool) true if entry is reserved
     */
    SAPDB_Bool IsReserved
        (const PageEntryNo&             entryno) const;

    /*!
       @brief          Writes to entry on frame: i/o block address
       @param          entryno [in] entry position
       @param          addr [in] i/o block address
       @param          oldaddr [out] old i/o address found at the entry
       @return         none

       - Side effect: If the specified entry contains an invalid
         block address and is now filled with a valid block address
         'addr', it is implicitely! assumed that a PopFreeEntry()
         has taken place in which the reserved entry count has been
         incremented. Therefore, the reserved entry count on the page
         is now being decremented.
     */
    virtual void WriteBlockAddress
        (const PageEntryNo&             entryno,
         const IOMan_BlockAddress&      addr,
         IOMan_BlockAddress&            oldaddr);

    /*!
       @brief          Returns the data page no associated to the given entry number
       @param          entryNo [in] page entry position
       @return         (Data_PageNo) data page no
     */
    Data_PageNo GetPageNo( const PageEntryNo &entryNo )
    {
        if( EntryInUse( entryNo ))
            return( ReadBasePageNo() + entryNo );
        else
        {
            Data_PageNo nilPageNo; // linux could not return( Data_PageNo() );
            return( nilPageNo );
        }
    }

    /*!
       @brief          Sets the "SavePagesRequested"-flag of the specified entry
       @param          entryno [in] entry no
       @return         (SAPDB_Bool) old state of the spr flag (true if set)
     */
    SAPDB_Bool SetSavePagesRequested
        (const PageEntryNo&             entryno);

    /*!
       @brief          Clears the "SavePagesRequested"-flag of the specified entry
       @param          entryno [in] entry no
       @return         (SAPDB_Bool) old state of the spr flag (true if set)
     */
    SAPDB_Bool ClearSavePagesRequested
        (const PageEntryNo&             entryno);

    /*!
       @brief          Checks whether specified entry is requested for savepages
       @return         (SAPDB_Bool) true if entry is in requested for savepages
     */
    SAPDB_Bool SavePagesRequested
        (const PageEntryNo&             entryno) const;

    /*!
       @brief          Sets the "SavePagesPending"-flag of the specified entry
       @param          entryno [in] entry no.
       @return         (SAPDB_Bool) old state of the spp flag (true if set)
     */
    SAPDB_Bool SetSavePagesPending
        (const PageEntryNo&             entryno);

    /*!
       @brief          Clears the "SavePagesPending"-flag of the specified entry
       @param          entryno [in] entry no.
       @return         (SAPDB_Bool) old state of the spp flag (true if set)
     */
    SAPDB_Bool ClearSavePagesPending
        (const PageEntryNo&             entryno);

    /*!
       @brief          Checks whether specified entry is requested for savepages
       @return         (SAPDB_Bool) true if entry is in requested for savepages
     */
    SAPDB_Bool SavePagesPending
        (const PageEntryNo&             entryno) const;

    /* -----------------------------------------------------------------------*/

private:

    /*--------------------------------------------------------------------------
    declaration: Persistent types
    description: Persistent type definitions with compile time size check
    --------------------------------------------------------------------------*/
    typedef SAPDB_Persistent            <Data_PageNo, SAPDB_UInt4, 4>
                                         PersistentPageNo;
    typedef SAPDB_PersistentBitRange    <Data_PageRecoveryMode, SAPDB_UInt1, 1, 0, 2>
                                         PersistentPageRecoveryMode;
    typedef SAPDB_Persistent            <PageEntryCount, SAPDB_UInt2, 2>
                                         PersistentPageEntryCount;

#if defined(AIX) || defined(HPUX) || defined(SUN) || defined(SOLARIS)
public:
#else
private:
#endif

    /*--------------------------------------------------------------------------
    declaration: PersistentHeader
    description: Persistent layout of data within kernel page header
    --------------------------------------------------------------------------*/
    // Must do this redefinition to avoid redundancy (compiler reason):
    typedef Converter_Page::PersistentHeader                    PH1;
    typedef PersistentPageNo::PersistentFormat                  PH2;
    typedef PersistentPageEntryCount::PersistentFormat          PH3;
    typedef SAPDB_UInt1                                         PH4;
    typedef SAPDB_UInt1                                         PH5;

    /// persistent data header structure of converter leaf page on volume
    struct PersistentHeader
    {
        /// persistent page header of converter page
        PH1     m_ConverterPageHeader;

        /// offset page number of stored pages
        PH2     m_BasePageNo;

        /// number of reserved entries in page
        PH3     m_ReservedCount;

        /// unused ..
        PH4     m_Unused;

        /// info data byte / recovery mode
        PH5     m_Info;
    };

private:

    /*--------------------------------------------------------------------------
    declaration: ConverterLeafPagePersistentHeader
    description: Compile time size check for persistent header
    --------------------------------------------------------------------------*/
    typedef SAPDB_Persistent
        < PersistentHeader,
          PersistentHeader,
          sizeof(PH1) +
          sizeof(PH2) +
          sizeof(PH3) +
          sizeof(PH4) +
          sizeof(PH5)
        >
        CheckedPersistentHeader;

    /*--------------------------------------------------------------------------
    function:     PersistentHeaderPtr()
    description:  Returns pointer to page header on frame
    arguments:    none
    return value: (PersistentHeader*) header pointer
    prototypes:   2
    --------------------------------------------------------------------------*/
    const PersistentHeader* PersistentHeaderPtr() const;

    PersistentHeader* PersistentHeaderPtr();

protected:

    /* -----------------------------------------------------------------------*/

    /*! @name Frame access (protected) */

    /*!
       @brief          Returns size of page header in bytes
       @return         (SizeType) header size
     */
    virtual SizeType GetHeaderSize() const;

   /*!
      @brief          Returns size of page trailer in bytes
      @return         (SizeType) header size
    */
    virtual SizeType GetTrailerSize () const
    {
         return sizeof(Kernel_Page74::PersistentTrailer);
    }

    /*!
       @brief          Writes to frame: base pageno of this converter leaf page
       @param          pageno [in] base pageno
       @return         none
     */
    void WriteBasePageNo
        (const Data_PageNo&             pageno);

    /*!
       @brief          Increments reserved entry count on frame
       @return         none
     */
    void IncReservedEntryCount();

    /*!
       @brief          Decrements reserved entry count on frame
       @return         none
     */
    void DecReservedEntryCount();

    /*!
       @brief          Sets reserved entry count on frame to 0
       @return         none
     */
    void ClearReservedEntryCount();

    /*!
       @brief          Initializes all entry flags on the page with 0's.
       @return         none
     */
    virtual void InitializeFlagInfo();

    /* -----------------------------------------------------------------------*/

    /*! @name Entry access (protected) */

    /*!
       @brief          Performs additional tasks when reserving entries
       @param          entryno [in] entry position
       @return         none

       - Implementation detail
     */
    virtual void Reserve
        (const PageEntryNo&             entryno);

    /*!
       @brief          Performs additional tasks when releasing entries
       @param          entryno [in] entry position
       @return         none

       - Implementation detail
     */
    virtual void Release
        (const PageEntryNo&             entryno);

    /* -----------------------------------------------------------------------*/
    /*@)*/
    /* -----------------------------------------------------------------------*/

private:

    /*--------------------------------------------------------------------------
    function:     CalcEntryCount()
    description:  Returns total count of entries on converter leaf page

                  - Preconditions: as = 4 or 8, hs = multiple of 4
                  - usablesize (us) = pagesize(ps) - headersize(hs) - trailersize(ts)
                  - entrysize(es)   = addrsize(as) + flagsize(fs) + off-cut(oc)
                  - as              = sizeof(IOMan_BlockAddress)
                  - fs              = 3/8 (3 bits)
    arguments:    none
    return value: (EntryCount) entry count
    --------------------------------------------------------------------------*/
    PageEntryCount CalcEntryCount() const;

    /*--------------------------------------------------------------------------
    function:     CalcEntryCount()
    description:  Returns total count of entries on converter leaf page

                  - Preconditions: as = 4 or 8, hs = multiple of 4
                  - usablesize (us) = pagesize(ps) - headersize(hs) - trailersize(ts)
                  - entrysize(es)   = addrsize(as) + flagsize(fs) + off-cut(oc)
                  - as              = sizeof(IOMan_BlockAddress)
                  - fs              = 3/8 (3 bits)
    arguments:    none
    return value: (EntryCount) entry count
    --------------------------------------------------------------------------*/
    static PageEntryCount CalcEntryCount (SAPDB_UInt Capacity);

    /*--------------------------------------------------------------------------
    function:     FlagsBlockSize()
    description:  Returns size of entire flag info block

                  - There are 3 blocks of this type on each page (in-use, spr, spd).
                  - The returned size is the same for each block.
    arguments:    none
    return value: (SizeType) block size
    --------------------------------------------------------------------------*/
    SizeType FlagsBlockSize() const;

    /*--------------------------------------------------------------------------
    function:     InUseFlagsOffset()
    description:  Returns offset on page where entire in-use flags block begins
    arguments:    none
    return value: (OffsetType) offset
    --------------------------------------------------------------------------*/
    virtual OffsetType InUseFlagsOffset() const;

    /*--------------------------------------------------------------------------
    function:     SPRFlagsOffset()
    description:  Returns offset on page where entire spr flags block begins
    arguments:    none
    return value: (OffsetType) offset
    --------------------------------------------------------------------------*/
    OffsetType SPRFlagsOffset() const;

    /*--------------------------------------------------------------------------
    function:     SPPFlagsOffset()
    description:  Returns offset on page where entire spd flags block begins
    arguments:    none
    return value: (OffsetType) offset
    --------------------------------------------------------------------------*/
    OffsetType SPPFlagsOffset() const;
};

/*!
   @class          Converter_LeafPageEntryIteratorDrop
   @brief          Implementation of an iterator, which provides all entries
                   linked with a valid block address on a given volume
 */
class Converter_LeafPageEntryIteratorDrop
{
public:

    /*! @name Constructors and initialization */

    /*!
       @brief          constructor
       @return         none
     */

    Converter_LeafPageEntryIteratorDrop
    (Converter_LeafPage&            page,
     const IOMan_DeviceNo           volNo,
     const Converter_PageEntryNo    pos = Converter_PageEntryNo() )
    :
    m_volNo(volNo),
    m_pLeafPage(&page),
    m_pos(pos),
    m_BitCount(page.ReadDeviceNoBitCount())
    {
    }

    /*!
       @brief          default constructor
       @return         none
     */

    Converter_LeafPageEntryIteratorDrop()
    :
    m_pLeafPage(NULL)
    {
    }

    /*!
       @brief          assignment operator
       @param          iter[in] other iterator
       @return         none
     */
    Converter_LeafPageEntryIteratorDrop& operator=
    (const Converter_LeafPageEntryIteratorDrop& iter)
    {
        m_pLeafPage = iter.m_pLeafPage;
        m_volNo = iter.m_volNo;
        m_BitCount = iter.m_BitCount;
        m_pos  = iter.m_pos;

        return *this;
    }


    /*!
       @brief          returns next valid pageNo on this converter leaf
       @return         Data_PageNo
     */
    void GetNextPageNo(Data_PageNo &pno, IOMan_BlockNo &blockNo)
    {
        // page is not assigned
        if (m_pLeafPage == NULL)
        {
            pno     = Data_PageNo();
            blockNo = IOMan_BlockNo();
            return;
        }

        // goto next position
        do
        {
            incr(m_pos);
        }
        while((m_pos != (*m_pLeafPage).GetEntryCount()) &&   // end of page
             (!(*m_pLeafPage).EntryInUse(m_pos) ||           // entry not in use
               GetPackedBlockAddress(m_pos).IsReserved() ||  // in use but reserved
               (GetBlockAddress(m_pos).GetDeviceNo() != m_volNo)));  // is on volume x?

        if (m_pos == (*m_pLeafPage).GetEntryCount())    // not found?
        {
            pno     = Data_PageNo();
            blockNo = IOMan_BlockNo();
            return;
        }
        else
        {
            pno     = (*m_pLeafPage).GetPageNo( m_pos );    // return pageNo
            blockNo = GetBlockNo( m_pos );    // return blockNo
            return;
        }
    }

    /*!
       @brief          compare operator
       @param          iter[in] other iterator
       @return         none
     */
   SAPDB_Bool operator==(const Converter_LeafPageEntryIteratorDrop &comp) const
   {
       return (m_volNo == comp.m_volNo) &&
              (m_pos   == comp.m_pos) &&
              (m_pLeafPage == comp.m_pLeafPage);
   }

protected:

    /*!
       @brief          increment position
       @param          pos[in/out] position value
       @return         Converter_PageEntryNo 
     */
    Converter_PageEntryNo incr(Converter_PageEntryNo &pos)
    {
        if (pos.IsValid())
        {
            pos++;
        }
        else  // iterator starts with invalid position, next is '0'
        {
            pos = 0;
        }

        return pos;
    }

    /*!
       @brief          Returns the block address of postition pos 
       @param          pos[in] position value
       @return         (IOMan_BlockAddress) actual block address
     */
    IOMan_BlockAddress GetBlockAddress(const Converter_PageEntryNo &pos) const
    {
        const IOMan_PackedBlockAddress
            packBlockAddr((*m_pLeafPage).PersistentEntryPtr(pos)->m_BlockAddress);

        return packBlockAddr.Unpack(( SAPDB_UInt ) m_BitCount );
    }

    IOMan_PackedBlockAddress GetPackedBlockAddress(Converter_PageEntryNo &pos) const
    {
        return( (*m_pLeafPage).PersistentEntryPtr(pos)->m_BlockAddress );
    }

    /*!
       @brief   Returns the block number for the given page entry.
       @param   pageEntryNo [in] page entry position in the current leaf page
       @return  (IOMan_BlockAddress) block
     */

    IOMan_BlockNo GetBlockNo( const Converter_PageEntryNo &pageEntryNo ) const
    {
        const IOMan_PackedBlockAddress
        packBlockAddr((*m_pLeafPage).PersistentEntryPtr( pageEntryNo )->m_BlockAddress);

        return packBlockAddr.Unpack(( SAPDB_UInt ) m_BitCount ).GetBlockNo();
    }


    /// volume no to look for
    IOMan_DeviceNo         m_volNo;

    /// pointer to current leaf page
    Converter_LeafPage    *m_pLeafPage;

    /// position of iterator
    Converter_PageEntryNo  m_pos;

    /// bit offset in block address
    SAPDB_UInt1                 m_BitCount;
};


/*******************************************************************************

   Persistent I/O of class Data_PageRecoveryMode on converter pages

*******************************************************************************/
template<>
inline SAPDB_UInt1 SAPDB_Persistent
<Data_PageRecoveryMode, SAPDB_UInt1, 1>::ConvertInternalToPersistent
    (const Data_PageRecoveryMode&   mode)
{
    SAPDB_UInt1 res = mode.PageIsRecoverable() ? 2 : 0;
    return res + (mode.UseDynamicPageAddressing() ? 1 : 0);
}
// -----------------------------------------------------------------------------
template<>
inline Data_PageRecoveryMode SAPDB_Persistent
<Data_PageRecoveryMode, SAPDB_UInt1, 1>::ConvertPersistentToInternal
    (const SAPDB_UInt1&             persmode)
{
    return Data_PageRecoveryMode
              (((persmode & 0x01) == 0) ? Data_Static : Data_Dynamic,
               ((persmode & 0x02) == 0) ? Data_NotRecoverable : Data_Recoverable);
}
// -----------------------------------------------------------------------------
template<>
inline SAPDB_UInt1 SAPDB_PersistentBitRange
<Data_PageRecoveryMode, SAPDB_UInt1, 1, 0, 2>::ConvertInternalToPersistent
    (const Data_PageRecoveryMode&   mode)
{
    return SAPDB_Persistent<Data_PageRecoveryMode, SAPDB_UInt1, 1>
              ::ConvertInternalToPersistent(mode);
}
// -----------------------------------------------------------------------------
template<>
inline Data_PageRecoveryMode SAPDB_PersistentBitRange
<Data_PageRecoveryMode, SAPDB_UInt1, 1, 0, 2>::ConvertPersistentToInternal
    (const SAPDB_UInt1&             persmode)
{
    return SAPDB_Persistent<Data_PageRecoveryMode, SAPDB_UInt1, 1>
              ::ConvertPersistentToInternal(persmode);
}

// -----------------------------------------------------------------------------

template<>
inline SAPDB_Bool SAPDB_PersistentBitRange
<Data_PageRecoveryMode, SAPDB_UInt1, 1, 0, 2>::InternalValueFitsBitFormat
      (const Data_PageRecoveryMode&   mode)
{
    return true;
}

// -----------------------------------------------------------------------------

inline Converter_LeafPage::Converter_LeafPage()
{}

//------------------------------------------------------------------------------

inline Converter_LeafPage::Converter_LeafPage
    (const PageFrame&               frame)
    :
     Converter_Page(frame)
{}

//------------------------------------------------------------------------------

inline Converter_LeafPage::Converter_LeafPage
    (const PageFrame&               frame,
     const Version&                 version,
     const Converter_ParentReference&         parent,
     const Data_PageNo&             basepageno,
     const Data_PageRecoveryMode&   rec)
    :
    Converter_Page(frame)
{
    InitializeFrame(version, parent, basepageno, rec);
}

//------------------------------------------------------------------------------

inline void
    Converter_LeafPage::SetAndInitializeFrame
    (const PageFrame&               frame,
     const Version&                 version,
     const Converter_ParentReference&         parent,
     const Data_PageNo&             basepageno,
     const Data_PageRecoveryMode&   rec)
{
    SetFrame(frame);
    InitializeFrame(version, parent, basepageno, rec);
}

//------------------------------------------------------------------------------

inline void
    Converter_LeafPage::InitializeFrame
    (const Version&                 version,
     const Converter_ParentReference&         parent,
     const Data_PageNo&             basepageno,
     const Data_PageRecoveryMode&   rec)
{
    Converter_Page::InitializeFrame(version, parent);
    WriteConverterPageType(Converter_LeafPageType);
    WriteBasePageNo(basepageno);
    WritePageRecoveryMode(rec);
    ClearReservedEntryCount();
    PersistentHeaderPtr()->m_Unused = 0; // PTS 1125335 TS 2003-12-03
    #if defined(SAPDB_SLOW) || defined (SAPDB_QUICK)
    SAPDBErr_MessageList dummyerrlist;
    // The messages are printed directly until this code (the caller)
    // uses message lists. UH 2004-11-15
    #endif
    SAPDBERR_ASSERT_ARGUMENT (Verify(false,dummyerrlist));
}

//------------------------------------------------------------------------------

inline Data_PageNo
    Converter_LeafPage::ReadBasePageNo() const
{
    const PersistentHeader& hp = *PersistentHeaderPtr();

    return PersistentPageNo::ConvertPersistentToInternal(hp.m_BasePageNo);
}

//------------------------------------------------------------------------------

inline void
    Converter_LeafPage::WritePageRecoveryMode
    (const Data_PageRecoveryMode&   rec)
{
    PersistentHeader& hp = *PersistentHeaderPtr();

    PersistentPageRecoveryMode pt(rec);
    pt.AssignPersistentValueTo(hp.m_Info);
}

//------------------------------------------------------------------------------

inline Data_PageRecoveryMode
    Converter_LeafPage::ReadPageRecoveryMode() const
{
    const PersistentHeader& hp = *PersistentHeaderPtr();
    return PersistentPageRecoveryMode::ConvertPersistentToInternal(hp.m_Info);
}

//------------------------------------------------------------------------------

inline SAPDB_Bool
    Converter_LeafPage::HasReservedEntries() const
{
    return ReadReservedEntryCount() > 0;
}

//------------------------------------------------------------------------------

inline Converter_LeafPage::PageEntryCount
    Converter_LeafPage::GetEntryCount() const
{
    static PageEntryCount count = CalcEntryCount();
    return count;
}

//------------------------------------------------------------------------------

inline Converter_LeafPage::PageEntryCount
    Converter_LeafPage::GetEntryCount (SAPDB_UInt LeafPageSize)
{
    static PageEntryCount count = CalcEntryCount (LeafPageSize);
    return count;
}

//------------------------------------------------------------------------------

inline SAPDBFields_Field
    Converter_LeafPage::GetInUseFlagsBlock() const
{
    return SAPDBFields_Field(m_Frame.GetPointer(InUseFlagsOffset(), FlagsBlockSize()),
                             FlagsBlockSize());
}

//------------------------------------------------------------------------------

inline SAPDB_Bool
    Converter_LeafPage::IsReserved
    (const PageEntryNo&             entryno) const
{
    IOMan_PackedBlockAddress ba = PersistentEntryPtr(entryno)->m_BlockAddress;
    return ba.IsReserved();
}

//------------------------------------------------------------------------------

inline void
    Converter_LeafPage::WriteBlockAddress
    (const PageEntryNo&             entryno,
     const IOMan_BlockAddress&      addr,
     IOMan_BlockAddress&            oldaddr)
{
    if (IsReserved(entryno))
        DecReservedEntryCount();

    Converter_Page::WriteBlockAddress(entryno, addr, oldaddr);
}

//------------------------------------------------------------------------------

inline SAPDB_Bool
    Converter_LeafPage::SetSavePagesRequested
    (const PageEntryNo&             entryno)
{
    Converter_FlagInfo   FlagInfo = GetFlagInfo(SPRFlagsOffset(), entryno);
    SAPDB_Bool oldstate = FlagInfo.FlagIsSet(entryno);
    FlagInfo.SetFlag(entryno);
    return oldstate;
}

//------------------------------------------------------------------------------

inline SAPDB_Bool
    Converter_LeafPage::ClearSavePagesRequested
    (const PageEntryNo&             entryno)
{
    Converter_FlagInfo   FlagInfo = GetFlagInfo(SPRFlagsOffset(), entryno);
    SAPDB_Bool oldstate = FlagInfo.FlagIsSet(entryno);
    FlagInfo.DelFlag(entryno);
    return oldstate;
}

//------------------------------------------------------------------------------

inline SAPDB_Bool
    Converter_LeafPage::SavePagesRequested
    (const PageEntryNo&             entryno) const
{
    return GetFlagInfo(SPRFlagsOffset(), entryno).FlagIsSet(entryno);
}

//------------------------------------------------------------------------------

inline SAPDB_Bool
    Converter_LeafPage::SetSavePagesPending
    (const PageEntryNo&             entryno)
{
    Converter_FlagInfo   FlagInfo = GetFlagInfo(SPPFlagsOffset(), entryno);
    SAPDB_Bool oldstate = FlagInfo.FlagIsSet(entryno);
    FlagInfo.SetFlag(entryno);
    return oldstate;
}

//------------------------------------------------------------------------------

inline SAPDB_Bool
    Converter_LeafPage::ClearSavePagesPending
    (const PageEntryNo&             entryno)
{
    Converter_FlagInfo   FlagInfo = GetFlagInfo(SPPFlagsOffset(), entryno);
    SAPDB_Bool oldstate = FlagInfo.FlagIsSet(entryno);
    FlagInfo.DelFlag(entryno);
    return oldstate;
}

//------------------------------------------------------------------------------

inline SAPDB_Bool
    Converter_LeafPage::SavePagesPending
    (const PageEntryNo&             entryno) const
{
    return GetFlagInfo(SPPFlagsOffset(), entryno).FlagIsSet(entryno);
}

//------------------------------------------------------------------------------

inline const Converter_LeafPage::PersistentHeader*
    Converter_LeafPage::PersistentHeaderPtr() const
{
    return reinterpret_cast<const PersistentHeader*>
            (m_Frame.GetPointer(0, sizeof(PersistentHeader)));
}

//------------------------------------------------------------------------------

inline Converter_LeafPage::PersistentHeader*
    Converter_LeafPage::PersistentHeaderPtr()
{
    return reinterpret_cast<PersistentHeader*>
            (m_Frame.GetPointer(0, sizeof(PersistentHeader)));
}

//------------------------------------------------------------------------------

inline Converter_LeafPage::SizeType
    Converter_LeafPage::GetHeaderSize()  const
{
    return sizeof(PersistentHeader);
}

//------------------------------------------------------------------------------

inline void
    Converter_LeafPage::WriteBasePageNo
    (const Data_PageNo&             pageno)
{
    PersistentHeader& hp = *PersistentHeaderPtr();

    hp.m_BasePageNo = PersistentPageNo::ConvertInternalToPersistent(pageno);
}

//------------------------------------------------------------------------------

inline void
    Converter_LeafPage::IncReservedEntryCount()
{
    PageEntryCount& count = PersistentHeaderPtr()->m_ReservedCount;

    SAPDBERR_ASSERT_STATE(count < GetEntryCount());

    ++count;
}

//------------------------------------------------------------------------------

inline void
    Converter_LeafPage::DecReservedEntryCount()
{
    PageEntryCount& count = PersistentHeaderPtr()->m_ReservedCount;

    SAPDBERR_ASSERT_STATE(count > 0);

    --count;
}

//------------------------------------------------------------------------------

inline void
    Converter_LeafPage::ClearReservedEntryCount()
{
    PersistentHeaderPtr()->m_ReservedCount = 0;
}

//------------------------------------------------------------------------------

inline Converter_LeafPage::PageEntryCount
    Converter_LeafPage::ReadReservedEntryCount() const
{
    return PersistentPageEntryCount::ConvertPersistentToInternal
           (PersistentHeaderPtr()->m_ReservedCount);
}

//------------------------------------------------------------------------------

inline void
    Converter_LeafPage::InitializeFlagInfo()
{
    Converter_FlagInfo::FlagInfoData* flaginfoptr    = GetFlagInfo(InUseFlagsOffset(), 0).FlagInfoDataPtr();
    Converter_FlagInfo::FlagInfoData* flaginfoendptr = GetFlagInfo(SPPFlagsOffset(), GetEntryCount() -1).FlagInfoDataPtr();

    while (flaginfoptr <= flaginfoendptr)
    {
        *flaginfoptr = 0;
        ++flaginfoptr;
    }
}

//------------------------------------------------------------------------------

inline void
    Converter_LeafPage::Reserve
    (const PageEntryNo&             entryno)
{
    Converter_Page::Reserve(entryno);
    IncReservedEntryCount();
}

//------------------------------------------------------------------------------

inline void
    Converter_LeafPage::Release
    (const PageEntryNo&             entryno)
{
    if (IsReserved(entryno))
        DecReservedEntryCount();
    ClearSavePagesRequested (entryno);
    ClearSavePagesPending   (entryno);
    Converter_Page::Release(entryno);
}

//------------------------------------------------------------------------------

inline Converter_LeafPage::SizeType
    Converter_LeafPage::FlagsBlockSize() const
{
    const  PageEntryCount count = GetEntryCount();
    const  SizeType SizeOfFlagInfoData = sizeof(Converter_FlagInfo::FlagInfoData);
    static SizeType size
        =  (count / (SAPDB_BITS_PER_BYTE * SizeOfFlagInfoData) +
               (count % (SAPDB_BITS_PER_BYTE * SizeOfFlagInfoData) != 0 ? 1 : 0))
             * SizeOfFlagInfoData;
    return size;
}

//------------------------------------------------------------------------------

inline Converter_LeafPage::OffsetType
    Converter_LeafPage::InUseFlagsOffset() const
{
    static OffsetType offs
           = GetHeaderSize() + GetEntryCount() * GetEntrySize();
    return offs;
}

//------------------------------------------------------------------------------

inline Converter_LeafPage::OffsetType
    Converter_LeafPage::SPRFlagsOffset() const
{
    static OffsetType offs =
        GetHeaderSize() + GetEntryCount() * GetEntrySize()
        + 1 * FlagsBlockSize();
    return offs;
}

//------------------------------------------------------------------------------

inline Converter_LeafPage::OffsetType
    Converter_LeafPage::SPPFlagsOffset() const
{
    static OffsetType offs =
        GetHeaderSize() + GetEntryCount() * GetEntrySize()
        + 2 * FlagsBlockSize();
    return offs;
}

//------------------------------------------------------------------------------

#endif // CONVERTER_LEAFPAGE_HPP
