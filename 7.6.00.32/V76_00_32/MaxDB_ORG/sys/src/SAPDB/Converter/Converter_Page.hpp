/*!
  @file           Converter_Page.hpp
  @author         TorstenS
  @ingroup        Converter
  @brief          Base class for converter page handlers
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


#ifndef CONVERTER_PAGE_HPP
#define CONVERTER_PAGE_HPP

#include "DataAccess/Data_Types.hpp"
#include "IOManager/IOMan_BlockAddress.hpp"
#include "KernelCommon/Kernel_Page74.hpp"
#include "Converter/Converter_Types.hpp"
#include "Converter/Converter_ParentReference.hpp"
#include "Converter/Converter_FlagInfo.hpp"


class Converter_PageEntryIterator;

/*!
  @class    Converter_Page
  @brief    Virtual base class for converter page handlers
            Defines the following pure virtual methods
            - GetEntryCount()
            - InUseFlagsOffset()
            - InitializeFlagInfo()

            Call static method 'SetDevNoBitCount()' once prior to first usage
            of this class.
 */
class Converter_Page : public Kernel_Page74
{
    friend class Converter_PageEntryIterator;
    friend class Converter_Map;

public:

    /* -----------------------------------------------------------------------*/

    /*! @name Type definitions */

    /// any number of converter page entries
    typedef Converter_PageEntryCount            PageEntryCount;

    /// index of a certain page entry
    typedef Converter_PageEntryNo               PageEntryNo;

    /// any number of converter index entries
    typedef Converter_IndexEntryCount           IndexEntryCount;

    /// index of a certain converter index entry
    typedef Converter_IndexEntryNo              IndexEntryNo;

    /// converter version
    typedef Converter_Version                   Version;

    /// block i/o address
    typedef IOMan_BlockAddress                  BlockAddress;

    /// device number bit count
    typedef SAPDB_UInt1                         BitCount;

    /* -----------------------------------------------------------------------*/

protected:

    /* -----------------------------------------------------------------------*/

    /*! @name Protected constructors and initialization */

    /*!
       @brief          Default constructor
       @return         none
     */
    Converter_Page();

    /*!
       @brief          Constructor; assigns frame
       @param          frame [in] page frame
       @return         none

       - Does not change frame contents.
     */
    Converter_Page
    (const PageFrame&               frame);

    /*!
       @brief          Constructor; assigns frame and initializes frame contents
       @param          frame [in] page frame
       @param          version [in] converter version
       @param          parent [in] reference to parent in converter tree parent index
       @return         none
     */
    Converter_Page
    (const PageFrame&               frame,
     const Version&                 version,
     const Converter_ParentReference&         parent);

    /*!
       @brief          Assigns frame and initializes frame contents
       @param          frame [in] page frame
       @param          version [in] converter version
       @param          parent [in] reference to parent in converter tree parent index
       @return         none
     */
    void SetAndInitializeFrame
    (const PageFrame&               frame,
     const Version&                 version,
     const Converter_ParentReference&         parent);

    /*!
       @brief        Initializes frame contents
       @param        version [in]   converter version
       @param        parent  [in]   reference to parent in converter tree parent
                                 index
       @return       none
    */
    void InitializeFrame
    (const Version&                 version,
     const Converter_ParentReference&         parent);
public:

    /*!
       @brief         Initializes static member 'device number bit count'
       @param         devnobitcount [in] number of bits used for persistent device
                                     number representation
       @return        none
    */
    static void SetDevNoBitCount
    (BitCount                       devnobitcount);

    /*!
       @brief          Reads converter page type from frame
       @param          frame [in] page frame
       @return         (Converter_PageType) page type
     */
    static Converter_PageType ReadConverterPageType(
        const PageFrame   &frame)
    {
        return PersistentPageType::ConvertPersistentToInternal
               (reinterpret_cast<const PersistentHeader*>
                (frame.GetPointer(0, sizeof(PersistentHeader)))->m_Info);
    }

    /* -----------------------------------------------------------------------*/
    /*! @name Frame access */

    /*!
       @brief          Writes converter version to frame
       @param          version [in] converter version
       @return         none
     */
    void WriteVersion
    (const Version&                 version);

    /*!
       @brief          Reads converter version from frame
       @return         (Version) converter version
     */
    Version ReadVersion() const;

    /*!
       @brief          Writes block i/o address of the current converter page to frame
       @param          address [in] block i/o address
       @return         none
     */
    void WriteBlockAddress
    (const BlockAddress&            address);

    /*!
       @brief          Reads block i/o address of the current converter page from frame
       @return         (BlockAddress) block address in internal format
     */
    BlockAddress ReadBlockAddress() const;

    /*!
       @brief          Reads converter page type from frame
       @return         (Converter_PageType) page type
     */
    Converter_PageType ReadConverterPageType() const;

    /*!
       @brief          Reads the count of bits used for storage
                       of logical device numbers from frame
       @return         (BitCount) bit count
     */
    BitCount ReadDeviceNoBitCount() const;

    /*!
       @brief          Writes reference to parent in converter parent index to frame
       @param          parent [in]   reference to parent
       @return         none
     */
    void WriteParentReference
    (const Converter_ParentReference&         parent);

    /*!
       @brief          Reads reference to parent in converter parent index from frame
       @return         (Converter_ParentReference) reference to parent
     */
    Converter_ParentReference ReadParentReference() const;

    /*!
       @brief          Checks whether there is at least one free entry on the page
       @return         (SAPDB_Bool) true if free entry available
     */
    SAPDB_Bool HasFreeEntryAvailable() const;

    /*!
       @brief          Returns total count of entries on converter page
       @return         (PageEntryCount) entry count
     */

    virtual PageEntryCount GetEntryCount() const = 0;

    /*!
       @brief          Returns pointer to block on frame where flag info on specified
                       entry is stored
       @param          flagblockoffs [in] additional page offset (sizeof(FlagInfo) aligned)
       @param          entryno [in] entry number
       @return         ((const)FlagInfo*)
     */
    Converter_FlagInfo GetFlagInfo
    (OffsetType                     flagblockoffs,
     const PageEntryNo&             entryno);

    Converter_FlagInfo GetFlagInfo
    (OffsetType                     flagblockoffs,
     const PageEntryNo&             entryno) const;

    /*!
       @brief          Determines total count of free entries on converter page
       @return         (PageEntryCount) entry count
     */
    PageEntryCount DetermineFreeEntryCount() const;

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
       @brief          Verifies consistency of page contents
       @return         (SAPDB_Bool) true if verify successful
     */
    virtual SAPDB_Bool Verify(bool                  isAdmin,
                              SAPDBErr_MessageList &errlist) const;

    /*!
        @brief  Checks page after reading from volume
        @return (SAPDB_Bool) true if check successful
    */
    virtual SAPDB_Bool CheckAfterRead() const;

    /* -----------------------------------------------------------------------*/

    /*! @name Iterators */

    /*!
       @brief          Returns iterator at first entry on page
       @return         (Converter_PageEntryIterator)
     */
    Converter_PageEntryIterator Begin();

    /*!
       @brief          Returns iterator behind last entry on page
       @return         (Converter_PageEntryIterator)
     */
    Converter_PageEntryIterator End();

    /* -----------------------------------------------------------------------*/

    /*! @name Entry access */

    /*!
       @brief          Writes to entry on frame: i/o block address
       @param          entryno [in] entry position
       @param          addr [in] i/o block address to be written
       @param          oldaddr [out] old i/o address found at the entry
       @return         none
     */
    virtual void WriteBlockAddress
    (const PageEntryNo&             entryno,
     const BlockAddress&            addr,
     BlockAddress&                  oldaddr);

    /*!
       @brief          Reads from entry on frame: i/o block address
       @param          entryno [in] entry position
       @param          bladdr [out] requested i/o address
       @return         (BlockAddress) requested i/o address
     */
    BlockAddress ReadBlockAddress
    (const PageEntryNo&             entryno,
     BlockAddress&                  bladdr) const;

    /*!
       @brief          Reserves a free page entry and returns its entry number
       @return         (PageEntryNo) entry number
     */
    PageEntryNo ReserveEntry();

    /*!
       @brief          Reserves a specified page entry and returns its entry number
       @param          entryno [in] entry position
       @return         (PageEntryNo) entry number

       - Entry 'entryno' must be unused (free).
     */
    PageEntryNo ReserveSpecifiedEntry
    (PageEntryNo                    entryno);

    /*!
       @brief          Chains in free page entry
       @param          entryno [in] entry number
       @return         none
     */
    void ReleaseEntry
    (PageEntryNo                    entryno);

    /*!
       @brief          Checks whether specified entry is in use
       @param          entryno [in] entry number
       @return         (SAPDB_Bool) true if entry is in use
     */
    SAPDB_Bool EntryInUse
    (const PageEntryNo&             entryno) const;

    /* -----------------------------------------------------------------------*/

#if defined(AIX) || defined(HPUX) || defined(SUN) || defined(SOLARIS)
public:
#else
private:
#endif

    /*--------------------------------------------------------------------------
    declaration: Persistent types
    description: Persistent type definitions with compile time size check
    --------------------------------------------------------------------------*/
    typedef SAPDB_Persistent         < Version,                  Kernel_Page::Id, 4 >     PersistentVersion;
    typedef SAPDB_Persistent         < IOMan_PackedBlockAddress, SAPDB_UInt4, 4 >         PersistentBlockAddress;
    typedef SAPDB_Persistent         < PageEntryNo,              SAPDB_UInt2, 2 >         PersistentPageEntryNo;
    typedef SAPDB_Persistent         < IndexEntryNo,             SAPDB_UInt2, 2 >         PersistentIndexEntryNo;
    typedef SAPDB_PersistentBitRange < Converter_PageType,       SAPDB_UInt1, 1, 0, 1 >   PersistentPageType;
    typedef SAPDB_PersistentBitRange < SAPDB_UInt1,              SAPDB_UInt1, 1, 1, 4 >   PersistentDevNoBitCount;


    /*--------------------------------------------------------------------------
    declaration: PersistentHeader
    description: Persistent layout of data within kernel page header
    --------------------------------------------------------------------------*/
    // Must do this redefinition to avoid redundancy (compiler reason):
    // PersistentVersion reuses MultiUse1 in Kernel_Page74::PersistentHeader
    typedef Kernel_Page74::PersistentHeader                 PH1;
    typedef PersistentBlockAddress::PersistentFormat        PH2;
    typedef SAPDB_UInt4                                     PH3;
    typedef PersistentPageEntryNo::PersistentFormat         PH4;
    typedef SAPDB_UInt1                                     PH5;
    typedef SAPDB_UInt1                                     PH6;
    typedef PersistentIndexEntryNo::PersistentFormat        PH7;
    typedef PersistentPageEntryNo::PersistentFormat         PH8;

protected:

    /// persistent header structure of converter page on volumes
    struct PersistentHeader
    {
        /// persisten header of default kernel page
        PH1     m_KernelPageHeader;

        /// my block address
        PH2     m_BlockAddress;

        /// unused
        PH3     m_Unused1;

        /// first free entry in page
        PH4     m_FreeEntryAnchorNo;

        /// info structure
        PH5     m_Info;

        /// unused
        PH6     m_Unused2;

        /// entry of parent index page
        PH7     m_ParentIndexEntryNo;

        /// entry of parent page
        PH8     m_ParentPageEntryNo;
    };

    /*--------------------------------------------------------------------------
    declaration: ConverterPagePersistentHeader
    description: Compile time size check for persistent header
    --------------------------------------------------------------------------*/
    typedef SAPDB_Persistent
    < PersistentHeader,
    PersistentHeader,
    sizeof(PH1) +
    sizeof(PH2) +
    sizeof(PH3) +
    sizeof(PH4) +
    sizeof(PH5) +
    sizeof(PH6) +
    sizeof(PH7) +
    sizeof(PH8) >
    CheckedPersistentHeader;

private:

    /*--------------------------------------------------------------------------
    function:     PersistentHeaderPtr()
    description:  Returns pointer to page header on frame
    arguments:    none
    return value: (PersistentHeader*) header pointer
    prototypes:   2
    --------------------------------------------------------------------------*/
    PersistentHeader* PersistentHeaderPtr();

    const PersistentHeader* PersistentHeaderPtr() const;

protected:

    /* -----------------------------------------------------------------------*/
    /*! @name Frame access (protected)  */

    /*!
       @brief       Internal union type for handling of entries.

                    The member m_BlockAddress is unused but required for correct
                    entry alignment on the page.
     */
    union PersistentEntry
    {
        /// linkage to pred and succ
        struct
        {
            PersistentPageEntryNo::PersistentFormat m_Predecessor;
            PersistentPageEntryNo::PersistentFormat m_Successor;
        }

        /// used with free entry chain
        m_Free;

        PersistentBlockAddress::PersistentFormat

        /// used with block addresses
        m_BlockAddress;
    };

    /*!
       @brief          Returns pointer to page entry on frame
       @param          entryno [in] entry position
       @return         (PersistentEntry*) header pointer
     */
    PersistentEntry* PersistentEntryPtr
    (PageEntryNo                        entryno);

    const PersistentEntry* PersistentEntryPtr
    (PageEntryNo                        entryno) const;


private:
    /*--------------------------------------------------------------------------
    declaration: ConverterPagePersistentEntry
    description: Compile time size check for persistent entry
    --------------------------------------------------------------------------*/
    typedef SAPDB_Persistent
    < PersistentEntry,
    PersistentEntry,
    sizeof(PersistentBlockAddress::PersistentFormat)
    >
    ConverterPagePersistentEntry;

protected:

    /*!
       @brief          Returns size of page entry in bytes
       @return         (SizeType) entry size
     */
    static SizeType GetEntrySize();

    /*!
       @brief          Returns size of page header in bytes
       @return         (SizeType) header size
     */
    virtual SizeType GetHeaderSize() const;

    /*!
       @brief          Writes converter page type to frame
       @param          type [in] converter page type
       @return         none
     */
    void WriteConverterPageType
    (const Converter_PageType&      type);

    /*!
       @brief          Returns offset on page where entire in-use flags block begins
       @return         (OffsetType) offset
     */
    virtual OffsetType InUseFlagsOffset() const = 0;

    /*!
       @brief          Initializes all entry flags on the page with 0's.
       @return         none
     */
    virtual void InitializeFlagInfo() = 0;

    /* -----------------------------------------------------------------------*/

    /*! @name Entry access (protected) */

    /*!
       @brief          Performs additional tasks when reserving entries
       @param          entryno [in] entry position
       @return         none
     */
    virtual void Reserve
    (const PageEntryNo&             entryno);

    /*!
       @brief          Performs additional tasks when releasing entries
       @param          entryno [in] entry position
       @return         none
     */
    virtual void Release
    (const PageEntryNo&             entryno);

    /*!
       @brief          Sets block address of specified entry to "reserved"
       @param          entryno [in] entry position
       @return         none
     */
    void SetBlockAddressAsReserved
    (const PageEntryNo&             entryno);

    /*!
       @brief          Sets block address of specified entry to "invalid"
       @param          entryno [in] entry position
       @return         none
     */
    void SetBlockAddressAsInvalid
    (const PageEntryNo&             entryno);

    /*!
       @brief          Sets the "InUse"-flag of the specified entry
       @param          entryno [in] entry no.
       @return         none
     */
    void SetEntryInUse
    (const PageEntryNo&             entryno);

    /*!
       @brief          Clears the "InUse"-flag of the specified entry
       @param          entryno [in] entry no.
       @return         none
     */
    virtual void ClearEntryInUse
    (const PageEntryNo&             entryno);

    /* -----------------------------------------------------------------------*/

private:

    /*--------------------------------------------------------------------------
    function:     BuildFreeEntryChain()
    description:  Constructs the chain of free entries on a "fresh" converter page
    arguments:    none
    return value: none
    --------------------------------------------------------------------------*/
    void BuildFreeEntryChain();

    /*--------------------------------------------------------------------------
    function:     ConvertUInt4ToBlockAddress()
    description:  Converts uint4 to block io address (persistent format)
    arguments:    addr [in]          block io address
                  devnobitcount [in] number of bits used for persistent device
                                     number representation
    return value: (BlockAddress) block io address
    --------------------------------------------------------------------------*/
    static BlockAddress ConvertUInt4ToBlockAddress
    (SAPDB_UInt4                    persaddr,
     SAPDB_UInt                     devnobitcount);

    /*--------------------------------------------------------------------------
    function:     WriteDeviceNoBitCount()
    description:  Writes number of bits used for device number to frame

    arguments:    none
    return value: none
    --------------------------------------------------------------------------*/
    void WriteDeviceNoBitCount();

    /*--------------------------------------------------------------------------
    function:     WriteFreeEntryAnchor()
    description:  Writes entry position of free entry anchor to frame
    arguments:    entryno [in]   entry position
    return value: none
    --------------------------------------------------------------------------*/
    void WriteFreeEntryAnchor
    (const PageEntryNo&                 entryno);

    /*--------------------------------------------------------------------------
    function:     ReadFreeEntryAnchor()
    description:  Reads entry position of free entry anchor from frame
    arguments:    none
    return value: (PageEntryNo) entry position
    --------------------------------------------------------------------------*/
    PageEntryNo ReadFreeEntryAnchor() const;

    /*--------------------------------------------------------------------------
    declaration : DevNoBitCount
    description:  number of bits used for persistent representation of
                  device numbers
    --------------------------------------------------------------------------*/
    static BitCount DevNoBitCount;

};


/*!
   @class          Converter_PageEntryIterator
   @brief          Steps over all entries on a page

   This iterator makes no difference between used and unused entries
 */
class Converter_PageEntryIterator
{
public:

    /*!
       @brief          Constructor
       @param          page [in] reference to converter page
       @param          pos [in] entry number defining start position on the page (default: 0)
       @return         none
     */
    Converter_PageEntryIterator
    (Converter_Page&                page,
     Converter_PageEntryNo          pos   = 0)
    :
    m_pPage(&page),
    m_Pos(pos),
    m_BitCount(page.ReadDeviceNoBitCount())
    {}

    Converter_PageEntryIterator()
    :
    m_pPage(NULL)
    {
    }

    /*!
       @brief          Assigns other iterator
       @param          iter [in] iterator to be assigned
       @return         none
     */
    Converter_PageEntryIterator& operator=
    (const Converter_PageEntryIterator& iter)
    {
        //if (iter != *this) // PTS 1113242 TS 2001-12-20
        //{
        m_pPage          = iter.m_pPage;
        m_Pos            = iter.m_Pos;
        m_BitCount       = iter.m_BitCount;
        //}
        return *this;
    }

    /*!
       @brief          Compares with other iterator
       @param          iter [in] other
       @return         (SAPDB_Bool) true if unequal
     */
    SAPDB_Bool operator!=(const Converter_PageEntryIterator &iter) const
    {
        SAPDBERR_ASSERT_ARGUMENT(m_pPage == iter.m_pPage);

        return m_Pos != iter.m_Pos;
    }

    /*!
       @brief          Compares with other iterator
       @param          iter [in] other
       @return         (SAPDB_Bool) true if equal
     */
    SAPDB_Bool operator==(const Converter_PageEntryIterator &iter) const
    {
        SAPDBERR_ASSERT_ARGUMENT(m_pPage == iter.m_pPage);

        return m_Pos == iter.m_Pos;
    }


    /*!
       @brief          Prefix increment; advances iterator
       @return         (Converter_PageEntryIterator&amp;) this instance
     */
    Converter_PageEntryIterator& operator++()
    {
        ++m_Pos;
        return *this;
    }

    /*!
       @brief          Returns the packed block address the iterator points to
       @return         (IOMan_PackedBlockAddress) actual block address in packed format
     */
    IOMan_PackedBlockAddress GetPackedBlockAddress() const
    {
        return( (*m_pPage).PersistentEntryPtr(m_Pos)->m_BlockAddress );
    }

    /*!
       @brief          Returns the block address the iterator points to
       @return         (IOMan_BlockAddress) actual block address
     */
    IOMan_BlockAddress GetBlockAddress() const
    {
        const IOMan_PackedBlockAddress
            packBlockAddr((*m_pPage).PersistentEntryPtr(m_Pos)->m_BlockAddress);

        return packBlockAddr.Unpack(( SAPDB_UInt ) m_BitCount );
    }

    /*!
       @brief          Checks whether actual entry is in use
       @return         (SAPDB_Bool) true if entry is in use
     */
    SAPDB_Bool EntryInUse() const
    {
        return( (*m_pPage).EntryInUse(m_Pos));
    }

    /*!
       @brief          Returns actual entry number iter points to
       @return         (Converter_PageEntryNo) actual entry number
     */
    Converter_PageEntryNo GetPosition() const
    {
        return m_Pos;
    }

protected:
    /// pointer to converter page
    Converter_Page             *m_pPage; // PTS 1113242 TS 2001-12-21

    /// current postion
    Converter_PageEntryNo       m_Pos;

    /// bit offset in block address
    SAPDB_UInt1                 m_BitCount;
};


// #if defined(SUN) || defined(SOLARIS)

/// conversion of internal data to persistent format
template<>
inline SAPDB_UInt1 SAPDB_Persistent
<Converter_PageType, SAPDB_UInt1, 1>::ConvertInternalToPersistent
(const Converter_PageType&   type)
{
    return type;
}

// -----------------------------------------------------------------------------

/// conversion of internal data to persistent format
template<>
inline SAPDB_UInt1 SAPDB_PersistentBitRange
<Converter_PageType, SAPDB_UInt1, 1, 0, 1>::ConvertInternalToPersistent
(const Converter_PageType&   type)
{
    return SAPDB_Persistent<Converter_PageType, SAPDB_UInt1, 1>
           ::ConvertInternalToPersistent(type);
}

// #endif

//------------------------------------------------------------------------------
//  Converter_Page
//------------------------------------------------------------------------------

inline Converter_Page::Converter_Page()
{}

//------------------------------------------------------------------------------

inline Converter_Page::Converter_Page
(const PageFrame&               frame)
        :
        Kernel_Page74(frame)
{}

//------------------------------------------------------------------------------

inline Converter_Page::Converter_Page
(const PageFrame&               frame,
 const Version&                 version,
 const Converter_ParentReference&         parent)
        :
        Kernel_Page74(frame)
{
    InitializeFrame(version, parent);
}

//------------------------------------------------------------------------------

inline void
Converter_Page::SetAndInitializeFrame
(const PageFrame&               frame,
 const Version&                 version,
 const Converter_ParentReference&         parent)
{
    SetFrame(frame);
    InitializeFrame(version, parent);
}

//------------------------------------------------------------------------------

inline void
Converter_Page::InitializeFrame
(const Version&                 version,
 const Converter_ParentReference&         parent)
{
    Kernel_Page74::InitializeFrame(Kernel_ConverterPageType);
    WriteVersion(version);
    WriteBlockAddress(BlockAddress());
    PersistentHeaderPtr()->m_Unused1 = 0;  // PTS 1125335 TS 2003-12-03
    WriteDeviceNoBitCount();
    PersistentHeaderPtr()->m_Unused2 = 0; // PTS 1125335 TS 2003-12-03
    WriteParentReference(parent);
    BuildFreeEntryChain();

    #ifdef SAPDB_QUICK
    const bool           dummyIsAdmin = true;
    SAPDBErr_MessageList errlist;
    SAPDBERR_ASSERT_ARGUMENT ( Converter_Page::Verify(dummyIsAdmin,errlist) );
    #endif
}

//------------------------------------------------------------------------------

inline void
Converter_Page::SetDevNoBitCount
(BitCount                       devnobitcount)
{
    DevNoBitCount = devnobitcount;
}

//------------------------------------------------------------------------------

inline void
Converter_Page::WriteVersion
(const Version&                 version)
{
    WritePageId(version);
}

//------------------------------------------------------------------------------

inline Converter_Page::Version
Converter_Page::ReadVersion() const
{
    return ReadPageId();
}

//------------------------------------------------------------------------------

inline void
Converter_Page::WriteBlockAddress
(const BlockAddress&            address)
{
    PersistentHeaderPtr()->m_BlockAddress =
        PersistentBlockAddress::PersistentFormat( address.Pack( DevNoBitCount ));
}

//------------------------------------------------------------------------------

inline Converter_Page::BlockAddress
Converter_Page::ReadBlockAddress() const
{
    const IOMan_PackedBlockAddress packedBlockAddr( PersistentHeaderPtr()->m_BlockAddress );

    return packedBlockAddr.Unpack( DevNoBitCount );
}

//------------------------------------------------------------------------------

inline Converter_PageType
Converter_Page::ReadConverterPageType() const
{
    return PersistentPageType::ConvertPersistentToInternal
           (PersistentHeaderPtr()->m_Info);
}

//------------------------------------------------------------------------------

inline Converter_Page::BitCount
Converter_Page::ReadDeviceNoBitCount() const
{
    return PersistentDevNoBitCount::ConvertPersistentToInternal
           (PersistentHeaderPtr()->m_Info);
}

//------------------------------------------------------------------------------

inline void
Converter_Page::WriteParentReference
(const Converter_ParentReference&         parent)
{
    PersistentHeaderPtr()->m_ParentIndexEntryNo =
        PersistentIndexEntryNo::ConvertInternalToPersistent(parent.GetIndexEntryNo());
    PersistentHeaderPtr()->m_ParentPageEntryNo  =
        PersistentPageEntryNo::ConvertInternalToPersistent(parent.GetPageEntryNo());
}

//------------------------------------------------------------------------------

inline Converter_ParentReference
Converter_Page::ReadParentReference() const
{
    return Converter_ParentReference
           (PersistentIndexEntryNo::ConvertPersistentToInternal
            (PersistentHeaderPtr()->m_ParentIndexEntryNo),
            PersistentPageEntryNo::ConvertPersistentToInternal
            (PersistentHeaderPtr()->m_ParentPageEntryNo));
}

//------------------------------------------------------------------------------

inline SAPDB_Bool
Converter_Page::HasFreeEntryAvailable() const
{
    return ReadFreeEntryAnchor().IsValid();
}

//------------------------------------------------------------------------------

inline void
Converter_Page::WriteBlockAddress
(const PageEntryNo&             entryno,
 const BlockAddress&            addr,
 BlockAddress&                  oldaddr)
{
    PersistentEntry                *pentry = PersistentEntryPtr(entryno);
    const IOMan_PackedBlockAddress packedBlockAddr( pentry->m_BlockAddress );

    if( packedBlockAddr.IsReserved() )
        oldaddr.Invalidate();
    else
        oldaddr = packedBlockAddr.Unpack( DevNoBitCount );

    pentry->m_BlockAddress =
        PersistentBlockAddress::PersistentFormat( addr.Pack( DevNoBitCount ));
}

//------------------------------------------------------------------------------

inline Converter_Page::BlockAddress
Converter_Page::ReadBlockAddress
(const PageEntryNo&             entryno,
 BlockAddress&                  bladdr) const
{
    if (EntryInUse(entryno))
    {
        const IOMan_PackedBlockAddress packedBlockAddr( PersistentEntryPtr(entryno)->m_BlockAddress );

        bladdr = packedBlockAddr.Unpack( DevNoBitCount);
    }
    else
        bladdr.Invalidate();

    return bladdr;
}

//------------------------------------------------------------------------------

inline Converter_Page::PersistentHeader*
Converter_Page::PersistentHeaderPtr()
{
    return reinterpret_cast<PersistentHeader*>
           (m_Frame.GetPointer(0, sizeof(PersistentHeader)));
}

//------------------------------------------------------------------------------

inline const Converter_Page::PersistentHeader*
Converter_Page::PersistentHeaderPtr() const
{
    return reinterpret_cast<const PersistentHeader*>
           (m_Frame.GetPointer(0, sizeof(PersistentHeader)));
}

//------------------------------------------------------------------------------

inline Converter_Page::PersistentEntry*
Converter_Page::PersistentEntryPtr
(PageEntryNo                        entryno)
{
    SAPDBERR_ASSERT_ARGUMENT(entryno < GetEntryCount());

    return reinterpret_cast<PersistentEntry*>
           (m_Frame.GetPointer
            (GetHeaderSize() + entryno * sizeof(PersistentEntry),
             sizeof(PersistentEntry)));
}

//------------------------------------------------------------------------------

inline const Converter_Page::PersistentEntry*
Converter_Page::PersistentEntryPtr
(PageEntryNo                        entryno) const
{
    SAPDBERR_ASSERT_ARGUMENT(entryno < GetEntryCount());

    return reinterpret_cast<const PersistentEntry*>
           (m_Frame.GetPointer
            (GetHeaderSize() + entryno * sizeof(PersistentEntry),
             sizeof(PersistentEntry)));
}

//------------------------------------------------------------------------------

inline Converter_Page::SizeType
Converter_Page::GetEntrySize()
{
    return sizeof(PersistentEntry);
}

//------------------------------------------------------------------------------

inline Converter_Page::SizeType
Converter_Page::GetHeaderSize() const
{
    return sizeof(PersistentHeader);
}

//------------------------------------------------------------------------------


inline void
Converter_Page::WriteConverterPageType
(const Converter_PageType&      type)
{
    PersistentPageType pt(type);
    pt.AssignPersistentValueTo(PersistentHeaderPtr()->m_Info);
}

//------------------------------------------------------------------------------

inline void
Converter_Page::Reserve
(const PageEntryNo&             entryno)
{
    SetBlockAddressAsReserved(entryno);
    SetEntryInUse(entryno);
}

//------------------------------------------------------------------------------

inline void
Converter_Page::Release
(const PageEntryNo&             entryno)
{
    ClearEntryInUse(entryno);
}

//------------------------------------------------------------------------------

inline void
Converter_Page::SetBlockAddressAsReserved
(const PageEntryNo&             entryno)
{
    // set block address on frame to "reserved"
    IOMan_PackedBlockAddress addr;

    addr.SetAsReserved();
    PersistentEntryPtr(entryno)->m_BlockAddress =
        PersistentBlockAddress::ConvertInternalToPersistent(addr);
}

//------------------------------------------------------------------------------

inline void
Converter_Page::SetBlockAddressAsInvalid
(const PageEntryNo&             entryno)
{
    // set block address on frame to "invalid"
    PersistentEntryPtr(entryno)->m_BlockAddress =
        PersistentBlockAddress::ConvertInternalToPersistent
        (IOMan_PackedBlockAddress()); // implicitely within constructor
}

//------------------------------------------------------------------------------

inline SAPDB_Bool
Converter_Page::EntryInUse
(const PageEntryNo&             entryno) const
{
    return GetFlagInfo(InUseFlagsOffset(), entryno).FlagIsSet(entryno);
}

//------------------------------------------------------------------------------

inline void
Converter_Page::SetEntryInUse
(const PageEntryNo&             entryno)
{
    GetFlagInfo(InUseFlagsOffset(), entryno).SetFlag(entryno);
}

//------------------------------------------------------------------------------

inline void
Converter_Page::ClearEntryInUse
(const PageEntryNo&             entryno)
{
    GetFlagInfo(InUseFlagsOffset(), entryno).DelFlag(entryno);
}

//------------------------------------------------------------------------------

inline Converter_Page::BlockAddress
Converter_Page::ConvertUInt4ToBlockAddress
(SAPDB_UInt4                    persaddr,
 SAPDB_UInt                     devnobitcount)
{
    if (persaddr != SAPDB_MAX_UINT4)
    {
        return BlockAddress(
                   IOMan_DeviceNo
                   (persaddr & ~(SAPDB_MAX_UINT4 << devnobitcount)),
                   IOMan_BlockNo
                   ((persaddr & (SAPDB_MAX_UINT4 << devnobitcount))
                    >> devnobitcount));
    }
    else
        return BlockAddress();
}

//------------------------------------------------------------------------------

inline void
Converter_Page::WriteDeviceNoBitCount()
{
    SAPDBERR_ASSERT_STATE(DevNoBitCount != 0);

    PersistentDevNoBitCount bc(DevNoBitCount);
    bc.AssignPersistentValueTo(PersistentHeaderPtr()->m_Info);
}

//------------------------------------------------------------------------------

inline void
Converter_Page::WriteFreeEntryAnchor
(const PageEntryNo&                 entryno)
{
#ifdef SAPDB_QUICK
    if( entryno.IsValid() )
    {
        SAPDBERR_ASSERT_STATE( entryno < GetEntryCount());
    }
#endif

    PersistentHeaderPtr()->m_FreeEntryAnchorNo =
        PersistentPageEntryNo::ConvertInternalToPersistent(entryno);
}

//------------------------------------------------------------------------------

inline Converter_Page::PageEntryNo
Converter_Page::ReadFreeEntryAnchor() const
{
    return PersistentPageEntryNo::ConvertPersistentToInternal
           (PersistentHeaderPtr()->m_FreeEntryAnchorNo);
}

//------------------------------------------------------------------------------

inline Converter_FlagInfo
Converter_Page::GetFlagInfo
(OffsetType                     flagblockoffs,
 const PageEntryNo&             entryno)
{
    SAPDBERR_ASSERT_ARGUMENT(flagblockoffs % sizeof(Converter_FlagInfo::FlagInfoData) == 0);

    return Converter_FlagInfo(reinterpret_cast<Converter_FlagInfo::FlagInfoData*>
                              (m_Frame.GetPointer(flagblockoffs, sizeof(Converter_FlagInfo::FlagInfoData))), entryno);
}

//------------------------------------------------------------------------------

inline Converter_FlagInfo
Converter_Page::GetFlagInfo
(OffsetType                     flagblockoffs,
 const PageEntryNo&             entryno) const
{
    SAPDBERR_ASSERT_ARGUMENT(flagblockoffs % sizeof(Converter_FlagInfo::FlagInfoData) == 0);

    return Converter_FlagInfo(reinterpret_cast<Converter_FlagInfo::FlagInfoData*>
                              (const_cast <unsigned char*>(m_Frame.GetPointer(flagblockoffs, sizeof(Converter_FlagInfo::FlagInfoData)))), entryno);
}

//------------------------------------------------------------------------------


#endif // CONVERTER_PAGE_HPP

