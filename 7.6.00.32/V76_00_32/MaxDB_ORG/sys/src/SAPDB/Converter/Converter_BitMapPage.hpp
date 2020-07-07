/*!
  @file           Converter_BitMapPage.hpp
  @author         TorstenS
  @author         TillL
  @ingroup        Converter
  @brief          Handler class for converter bitmap pages
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



#ifndef CONVERTER_BITMAPPAGE_HPP
#define CONVERTER_BITMAPPAGE_HPP


#include <memory.h>

#include "SAPDBCommon/SAPDB_BitOperations.hpp"
#include "KernelCommon/Kernel_Page74.hpp"
#include "DataAccess/Data_Types.hpp"
#include "Converter/Converter_Types.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"



/*!
   @class          Converter_BitMapPage
   @brief          Handler class for converter bitmap pages

   Bitmap pages are used for storing the used page numbers of the converter
 */
class Converter_BitMapPage : public Kernel_Page74
{
public:

    /* -----------------------------------------------------------------------*/
    /*! @name Type definitions */


    /// any number of entries on a converter page
    typedef Converter_PageEntryCount                PageEntryCount;

    /// converter version
    typedef Converter_Version                       Version;

    /// page addressing mode
    typedef Data_AddressingMode                     AddressingMode;

    /// slot count on page
    typedef SAPDB_UInt1                             SlotCount;

    /// elementary bit block within slot (actually 32 bits)
    typedef SAPDB_UInt4                             BitBlock;

    /* -----------------------------------------------------------------------*/
    /*! @name Constructors and initialization */
    /*@{*/

    /*!
       @brief          Default constructor
     */
    Converter_BitMapPage();

    /*!
       @brief          Constructor; assigns frame
       @param          frame [in] page frame
       @return         none

       - Does not change frame contents.
       - It is assumed that the frame contains a valid converter bitmap page.
     */
    Converter_BitMapPage
        (const PageFrame&               frame);

    /*!
       @brief          Constructor; assigns frame and initializes frame contents
       @param          frame [in] page frame
       @param          version [in] converter version
       @param          bitcount [in] count of bits to be reserved for each slot
       @param          mode [in] page addressing mode (stat/dyn)
       @return         none

       - It is assumed that the frame contains an empty page                 which may be overwritten.
     */
    Converter_BitMapPage
        (const PageFrame&               frame,
         const Version&                 version,
         const PageEntryCount&          bitcount,
         const AddressingMode&          mode);

    /*!
       @brief          Assigns frame and initializes frame contents
       @param          frame [in] page frame
       @param          version [in] converter version
       @param          bitcount [in] count of bits to be reserved for each slot
       @param          mode [in] page addressing mode (stat/dyn)
       @return         none
     */
    void SetAndInitializeFrame
        (const PageFrame&               frame,
         const Version&                 version,
         const PageEntryCount&          bitcount,
         const AddressingMode&          mode);

    /*!
       @brief          Initializes frame contents
       @param          version [in] converter version
       @param          bitcount [in] count of bits to be reserved for each slot
       @param          mode [in] page addressing mode (stat/dyn)
       @return         none
     */
    void InitializeFrame
        (const Version&                 version,
         const PageEntryCount&          bitcount,
         const AddressingMode&          mode);

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! @name Frame access */
    /*@{*/
    /*!
       @brief          Reads converter version from frame
       @return         (Version) converter version
     */
    Version ReadVersion() const;

    /*!
       @brief          Reads converter leaf page entry count from frame
       @return         (PageEntryCount) entry count == bit count

       - The total count of bits that has been reserved on this page
         for the entries of one converter leaf page is read from frame.
     */
    PageEntryCount ReadBitLoadPerSlot() const;

    /*!
       @brief          Reads actual number of filled frame slots from frame
       @return         (SlotCount) actual number of filled slots
     */
    SlotCount ReadFilledSlotCount() const;

    /*!
       @brief          Reads data page addressing mode from frame
       @return         (AddressingMode) addressing mode (stat/dyn)
     */
    AddressingMode ReadPageAddressingMode() const;

    /*!
       @brief          Increments actual number of filled frame slots on frame
       @return         none
     */

    void IncFilledSlotCount();

    /*!
       @brief          Checks whether all page slots are occupied
       @return         (SAPDB_Bool) true if all page slots are occupied
     */
    SAPDB_Bool IsFull() const;

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/


    /*!
       @brief          Verifies consistency of page contents
       @return         (SAPDB_Bool) true if verify successful
     */
    virtual SAPDB_Bool Verify(bool                  isAdmin,
                              SAPDBErr_MessageList &errlist) const
    {
        return true; // dummy
    }
    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/

    /* -----------------------------------------------------------------------*/
    /*! @name ConstIterator subclass */
    /*@{*/

    /*!
       @brief          Simple forward const iterator which steps from slot to slot

       - The physical layout of each slot is:
           one data pageno (sizeof(Bitblock) aligned) followed by an array
           of bitmaps of type BitBlock (also sizeof(BitBlock) aligned).
       - SizeType counts in sizeof(BitBlock).
       - It is assumed that sizeof(BitBlock) >= 4.
       - It is further assumed that Data_PageNo can be written
         persistently with 4 byte alignment.
     */
    class ConstIterator
    {
    public:

        /// pointer to constant bit blocks within a slot
        typedef const BitBlock*                 ConstPointer;

        /// const iterator of bit blocks within a slot
        typedef ConstPointer                    ConstBlockIterator;

        /// slot size in bit blocks
        typedef SAPDB_UInt                      BlockCount;

        /*!
           @brief          Constructor
           @param          ptr [in] pointer to slot (default: 0)
           @param          slotsize [in] slot size (default: 0)
           @return         (ConstIterator&amp;) *this

           - The iterator obtains a pointer 'ptr' to the beginning
             of the slot and a slot size in sizeof(BitBlock).
           - It is assumed that slot size is equal to step size.
         */
        ConstIterator
            (ConstPointer                   ptr      = 0,
             BlockCount                     slotsize = 0);

        /*!
           @brief          Compares position of present iterator with argument
           @param          iter [in] iterator to be compared
           @return         (SAPDB_Bool) true if both iterators are at different locations
         */
        SAPDB_Bool operator!=
            (const ConstIterator&                iter);

        /*!
           @brief          Advances the iterator to the next slot
           @return         (ConstIterator&amp;) *this
         */
        ConstIterator& operator++();

        /*!
           @brief          Returns iterator to first bit block
           @return         (ConstBlockIterator)
         */
        ConstBlockIterator DataBegin() const;

        /*!
           @brief          Returns iterator behind last bitblock
           @return         (ConstBlockIterator)
         */
        ConstBlockIterator DataEnd() const;

        /*!
           @brief          Reads data pageno from slot frame
           @return         (Data_PageNo) page no

           - It is assumed that Data_PageNo can be read                 persistently with 4 byte alignment.
         */
        Data_PageNo ReadDataPageNo() const;

    private:
        /// compile time size check
        typedef SAPDB_Persistent < Data_PageNo, SAPDB_UInt4, 4 >
                                   PersistentDataPageNo;

        ConstPointer    m_Ptr;
        BlockCount      m_SlotSize;
    };
    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/


    /* -----------------------------------------------------------------------*/
    /*! @name Iterator subclass */
    /*@{*/

    /*!
       @brief          Simple forward iterator which steps from slot to slot

       - see ConstIterator
     */
    class Iterator
    {
    public:


        /// pointer to bit blocks within a slot
        typedef BitBlock*                       Pointer;

        /// iterator of bit blocks within a slot
        typedef Pointer                         BlockIterator;

        /// slot size in bit blocks
        typedef SAPDB_UInt                      BlockCount;

        /*!
           @brief          Constructor
           @param          ptr [in] pointer to slot (default: 0)
           @param          slotsize [in] slot size (default: 0)
           @return         (Iterator&amp;) *this

           - The iterator obtains a pointer 'ptr' to the beginning
             of the slot and a slot size in sizeof(BitBlock).
           - It is assumed that slot size is equal to step size.
         */
        Iterator
            (Pointer                        ptr      = 0,
             BlockCount                     slotsize = 0);

        /*!
           @brief          Compares position of present iterator with argument
           @param          iter [in] iterator to be compared
           @return         (SAPDB_Bool) true if both iterators are at different locations
         */
        SAPDB_Bool operator!=
            (const Iterator&                iter);

        /*!
           @brief          Advances the iterator to the next slot
           @return         (Iterator&amp;) *this
         */
        Iterator& operator++();

        /*!
           @brief          Returns iterator to first bit block
           @return         (BlockIterator)
         */
        BlockIterator DataBegin() const;

        /*!
           @brief          Returns iterator behind last bitblock
           @return         (BlockIterator)
         */
        BlockIterator DataEnd() const;

        /*!
           @brief          Writes out a data pageno into slot frame
           @param          pno [in] data pageno to be written
           @return         none

           - It is assumed that Data_PageNo can be written
             persistently with 4 byte alignment.
         */
        void WriteDataPageNo
            (const Data_PageNo&             pno);

        /*!
           @brief          Reads data pageno from slot frame
           @return         (Data_PageNo) page no

           - It is assumed that Data_PageNo can be read
             persistently with 4 byte alignment.
         */
        Data_PageNo ReadDataPageNo() const;

        /*!
           @brief          Writes out a bitmap into slot frame
           @param          map [in] bitmap to be written
           @return         none

           - It is assumed that 'map' exactly fits the slot bit capacity.
         */
        void WriteBitMap
            (const SAPDBFields_Field&       map);

    private:

        Pointer         m_Ptr;
        BlockCount      m_SlotSize;
    };
    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/


    /* -----------------------------------------------------------------------*/
    /*! @name Iterator support */

    /*@{*/
    /*!
       @brief          Returns iterator to first slot on the page
       @return         (Iterator/ConstIterator)
     */
    Iterator Begin();

    ConstIterator Begin() const;

    /*!
       @brief          Returns iterator behind last filled slot on the page
       @return         (Iterator/ConstIterator)
     */
    Iterator End();

    ConstIterator End() const;

    /*!
       @brief          Returns iterator behind last slot on the page
       @return         (Iterator/ConstIterator)
     */
    Iterator PageEnd();

    ConstIterator PageEnd() const;

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/

private:

    /// Persistent type definitions with compile time size check
    typedef SAPDB_Persistent         < Version,        Kernel_Page::Id, 4 >
                                       PersistentVersion;
    typedef SAPDB_Persistent         < PageEntryCount, SAPDB_UInt2, 2 >
                                       PersistentBitLoadPerSlot;
    typedef SAPDB_Persistent         < SAPDB_UInt1,    SAPDB_UInt1, 1 >
                                       PersistentFilledSlotCount;
    typedef SAPDB_PersistentBitRange < AddressingMode, SAPDB_UInt1, 1, 0, 1 >
                                       PersistentPageAddressingMode;

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

    /// persistent data header structure of converter bitmap page on volume
    typedef Kernel_Page74::PersistentHeader                 PH1;
    typedef PersistentBitLoadPerSlot::PersistentFormat      PH2;
    typedef PersistentFilledSlotCount::PersistentFormat     PH3;
    typedef SAPDB_UInt1                                     PH4;
    typedef SAPDB_UInt4                                     PH5;

protected:

    /// persistent data header structure of converter bitmap page on volume
    struct PersistentHeader
    {
        /// persistent page header of kernel page
        PH1     m_KernelPage74Header;
        /// number of bit reserved
        PH2     m_BitLoadPerSlot;
        /// number of filled frame slots
        PH3     m_FilledSlotCount;
        /// addressing mode
        PH4     m_AddressingMode;
        ///  unused header
        PH5     m_Unused;
    };

private:

    /*--------------------------------------------------------------------------
    declaration: CheckedPersistentHeader
    description: Compile time size check for persistent header
    --------------------------------------------------------------------------*/
    typedef SAPDB_Persistent
        < PersistentHeader,
          PersistentHeader,
          sizeof(PH1) +
          sizeof(PH2) +
          sizeof(PH3) +
          sizeof(PH4) +
          sizeof(PH5) >
        CheckedPersistentHeader;

    /*--------------------------------------------------------------------------
    function:     PersistentHeaderPtr()
    description:  Returns pointer to page header on frame
    arguments:    none
    return value: (PersistentHeader*) header pointer
    prototypes:   2
    --------------------------------------------------------------------------*/
    PersistentHeader* PersistentHeaderPtr();

    const PersistentHeader* PersistentHeaderPtr() const;

    /*--------------------------------------------------------------------------
    function:     PersistentFirstSlotPtr()
    description:  Returns pointer to first slot on frame
    arguments:    none
    return value: (BitBlock*) slot pointer
    prototypes:   2
    --------------------------------------------------------------------------*/
    BitBlock* PersistentFirstSlotPtr();

    const BitBlock* PersistentFirstSlotPtr() const;

protected:

    /* -----------------------------------------------------------------------*/
    /*! @name Frame access (protected) */
    /*@{*/

    /*!
       @brief          Returns size of page header
       @return         (SizeType) header size
     */
    virtual SizeType GetHeaderSize() const;

    /*!
       @brief          Writes converter version to frame
       @param          version [in] converter version
       @return         none
     */
    void WriteVersion
        (const Version&       version);

    /*!
       @brief          Writes converter leaf page entry count to frame
       @param          count [in] entry count == bit count
       @return         none

       - The total count of bits to be reserved on this page for the
         entries of one converter leaf page is written to frame.
     */
    void WriteBitLoadPerSlot
        (const PageEntryCount&    count);

    /*!
       @brief          Writes actual number of filled frame slots to frame
       @param          count [in] actual number of filled slots
       @return         none
     */
    void WriteFilledSlotCount
        (const SlotCount&               count);

    /*!
       @brief          Writes page addressing mode to frame
       @param          mode [in] page addressing mode
       @return         none
     */
    void WritePageAddressingMode
        (const AddressingMode&   mode);

    /* -----------------------------------------------------------------------*/
    /*@}*/
    /* -----------------------------------------------------------------------*/


private:

    /*--------------------------------------------------------------------------
    function:     TotalNoOfSlots()
    description:  Calculates maximum number of slots on present page


                  - Slots are sizeof(BitBlock) aligned.
                  - It is assumed that frame is sizeof(BitBlock) aligned.
                  - It is assumed that sizeof(frame) is a multiple of
                    sizeof(BitBlock).
    arguments:    none
    return value: (SlotCount) max. number of slots
    --------------------------------------------------------------------------*/
    SlotCount TotalNoOfSlots() const;

    /*--------------------------------------------------------------------------
    function:     SlotSize()
    description:  Calculates size of a single slot on the present page

                  - Slots are sizeof(BitBlock) aligned.
    arguments:    none
    return value: (SizeType) slot size
    --------------------------------------------------------------------------*/
    SizeType SlotSize() const;
};



//------------------------------------------------------------------------------

inline Converter_BitMapPage::Converter_BitMapPage()
{}

//------------------------------------------------------------------------------

inline Converter_BitMapPage::Converter_BitMapPage
    (const PageFrame&               frame)
    :
     Kernel_Page74(frame)
{}

//------------------------------------------------------------------------------

inline Converter_BitMapPage::Converter_BitMapPage
    (const PageFrame&               frame,
     const Version&                 version,
     const PageEntryCount&          bitcount,
     const AddressingMode&          mode)
    :
     Kernel_Page74(frame, Kernel_ConverterBitMapPageType)
{
    InitializeFrame(version, bitcount, mode);
}


//------------------------------------------------------------------------------

inline void
    Converter_BitMapPage::SetAndInitializeFrame
    (const PageFrame&               frame,
     const Version&                 version,
     const PageEntryCount&          bitcount,
     const AddressingMode&          mode)
{
    SetFrame(frame);
    InitializeFrame(version, bitcount, mode);
}

//------------------------------------------------------------------------------

inline void
    Converter_BitMapPage::InitializeFrame
    (const Version&                 version,
     const PageEntryCount&          bitcount,
     const AddressingMode&          mode)
{
    Kernel_Page74::InitializeFrame(Kernel_ConverterBitMapPageType);
    WriteVersion(version);
    WritePageAddressingMode(mode);
    WriteBitLoadPerSlot(bitcount);
    WriteFilledSlotCount(0);
    PersistentHeaderPtr()->m_Unused = 0; // PTS 1125335 TS 2003-12-03
}

//------------------------------------------------------------------------------

inline Converter_BitMapPage::Version
    Converter_BitMapPage::ReadVersion() const
{
    return ReadPageId();
}

//------------------------------------------------------------------------------

inline Converter_BitMapPage::PageEntryCount
    Converter_BitMapPage::ReadBitLoadPerSlot() const
{
    return PersistentBitLoadPerSlot::ConvertPersistentToInternal
           (PersistentHeaderPtr()->m_BitLoadPerSlot);
}

//------------------------------------------------------------------------------

inline Converter_BitMapPage::SlotCount
    Converter_BitMapPage::ReadFilledSlotCount() const
{
    return PersistentFilledSlotCount::ConvertPersistentToInternal
           (PersistentHeaderPtr()->m_FilledSlotCount);
}

//------------------------------------------------------------------------------

inline Converter_BitMapPage::AddressingMode
    Converter_BitMapPage::ReadPageAddressingMode() const
{
    return PersistentPageAddressingMode::ConvertPersistentToInternal
           (PersistentHeaderPtr()->m_AddressingMode);
}

//------------------------------------------------------------------------------

inline void
    Converter_BitMapPage::IncFilledSlotCount()
{
    SlotCount count = ReadFilledSlotCount();

    PersistentHeaderPtr()->m_FilledSlotCount =
        PersistentFilledSlotCount::ConvertInternalToPersistent(count + 1);
}

//------------------------------------------------------------------------------

inline SAPDB_Bool
    Converter_BitMapPage::IsFull() const
{
    return ReadFilledSlotCount() == TotalNoOfSlots();
}

//------------------------------------------------------------------------------

inline Converter_BitMapPage::Iterator
    Converter_BitMapPage::Begin()
{
    return Iterator(PersistentFirstSlotPtr(), SlotSize() / sizeof(BitBlock));
}

//------------------------------------------------------------------------------

inline Converter_BitMapPage::ConstIterator
    Converter_BitMapPage::Begin() const
{
    return ConstIterator(PersistentFirstSlotPtr(), SlotSize() / sizeof(BitBlock));
}

//------------------------------------------------------------------------------

inline Converter_BitMapPage::Iterator
    Converter_BitMapPage::End()
{
    return Iterator(PersistentFirstSlotPtr()
                    + SlotSize() / sizeof(BitBlock) * ReadFilledSlotCount(),
                    SlotSize() / sizeof(BitBlock));
}

//------------------------------------------------------------------------------

inline Converter_BitMapPage::ConstIterator
    Converter_BitMapPage::End() const
{
    return ConstIterator(PersistentFirstSlotPtr()
                         + SlotSize() / sizeof(BitBlock) * ReadFilledSlotCount(),
                         SlotSize() / sizeof(BitBlock));
}

//------------------------------------------------------------------------------

inline Converter_BitMapPage::Iterator
    Converter_BitMapPage::PageEnd()
{
    return Iterator(PersistentFirstSlotPtr()
                    + SlotSize() / sizeof(BitBlock) * TotalNoOfSlots(),
                    SlotSize() / sizeof(BitBlock));
}

//------------------------------------------------------------------------------

inline Converter_BitMapPage::ConstIterator
    Converter_BitMapPage::PageEnd() const
{
    return ConstIterator(PersistentFirstSlotPtr()
                         + SlotSize() / sizeof(BitBlock) * TotalNoOfSlots(),
                         SlotSize() / sizeof(BitBlock));
}

//------------------------------------------------------------------------------

inline Converter_BitMapPage::SizeType
    Converter_BitMapPage::GetHeaderSize() const
{
    return sizeof(PersistentHeader);
}

//------------------------------------------------------------------------------

inline Converter_BitMapPage::PersistentHeader*
    Converter_BitMapPage::PersistentHeaderPtr()
{
    return reinterpret_cast<PersistentHeader*>
           (m_Frame.GetPointer(0, sizeof(PersistentHeader)));
}

//------------------------------------------------------------------------------

inline const Converter_BitMapPage::PersistentHeader*
    Converter_BitMapPage::PersistentHeaderPtr() const
{
    return reinterpret_cast<const PersistentHeader*>
           (m_Frame.GetPointer(0, sizeof(PersistentHeader)));
}

//------------------------------------------------------------------------------

inline Converter_BitMapPage::BitBlock*
    Converter_BitMapPage::PersistentFirstSlotPtr()
{
    SAPDB_UInt almnt = sizeof(BitBlock);

    return reinterpret_cast<BitBlock*>
           (m_Frame.GetPointer(((GetHeaderSize() + almnt - 1) / almnt) * almnt,
                             SlotSize()));
}

//------------------------------------------------------------------------------

inline const Converter_BitMapPage::BitBlock*
    Converter_BitMapPage::PersistentFirstSlotPtr() const
{
    SAPDB_UInt almnt = sizeof(BitBlock);

    return reinterpret_cast<const BitBlock*>
           (m_Frame.GetPointer(((GetHeaderSize() + almnt - 1) / almnt) * almnt,
                             SlotSize()));
}

//------------------------------------------------------------------------------

inline void
    Converter_BitMapPage::WriteVersion
    (const Version&       version)
{
    WritePageId(version);
}

//------------------------------------------------------------------------------

inline void
    Converter_BitMapPage::WriteBitLoadPerSlot
    (const PageEntryCount&    count)
{
    PersistentHeaderPtr()->m_BitLoadPerSlot =
        PersistentBitLoadPerSlot::ConvertInternalToPersistent(count);
}

//------------------------------------------------------------------------------

inline void
    Converter_BitMapPage::WriteFilledSlotCount
    (const SlotCount&               count)
{
    PersistentHeaderPtr()->m_FilledSlotCount =
        PersistentFilledSlotCount::ConvertInternalToPersistent(count);
}

//------------------------------------------------------------------------------

inline void
    Converter_BitMapPage::WritePageAddressingMode
    (const AddressingMode&   mode)
{
    PersistentPageAddressingMode pt(mode);
    pt.AssignPersistentValueTo(PersistentHeaderPtr()->m_AddressingMode);
}

//------------------------------------------------------------------------------

inline Converter_BitMapPage::SlotCount
    Converter_BitMapPage::TotalNoOfSlots() const
{
    SAPDB_UInt almnt = sizeof(BitBlock);

    SAPDB_UInt totalspace = m_Frame.GetLength()
                          - ((GetHeaderSize() + almnt - 1) / almnt) * almnt
                          - ((GetTrailerSize() + almnt - 1) / almnt) * almnt;
    return totalspace / SlotSize();
}

//------------------------------------------------------------------------------

inline Converter_BitMapPage::SizeType
    Converter_BitMapPage::SlotSize() const
{
    SAPDB_UInt almnt = sizeof(BitBlock);

    return  (Converter_BitMapPage::SizeType )((sizeof(Data_PageNo) + almnt - 1) / almnt) * almnt
          + ((ReadBitLoadPerSlot() + SAPDB_BITS_PER_BYTE * almnt - 1)
             / (SAPDB_BITS_PER_BYTE * almnt)) * almnt;
}

//------------------------------------------------------------------------------
//  Converter_BitMapPage::ConstIterator
//------------------------------------------------------------------------------

inline Converter_BitMapPage::ConstIterator::ConstIterator
    (ConstPointer                   ptr,
     BlockCount                     slotsize)
    :
     m_Ptr(ptr),
     m_SlotSize(slotsize)
{}

//------------------------------------------------------------------------------

inline SAPDB_Bool
    Converter_BitMapPage::ConstIterator::operator!=
    (const ConstIterator&           iter)
{
    return (m_Ptr != iter.m_Ptr);
}

//------------------------------------------------------------------------------

inline Converter_BitMapPage::ConstIterator&
    Converter_BitMapPage::ConstIterator::operator++()
{
    SAPDBERR_ASSERT_ARGUMENT(m_Ptr != 0);
    SAPDBERR_ASSERT_ARGUMENT(m_SlotSize != 0);
    m_Ptr += m_SlotSize;
    return *this;
}

//------------------------------------------------------------------------------

inline Converter_BitMapPage::ConstIterator::ConstBlockIterator
    Converter_BitMapPage::ConstIterator::DataBegin() const
{
    return m_Ptr + 1;
}

//------------------------------------------------------------------------------

inline Converter_BitMapPage::ConstIterator::ConstBlockIterator
    Converter_BitMapPage::ConstIterator::DataEnd() const
{
    return m_Ptr + m_SlotSize;
}

//------------------------------------------------------------------------------

inline Data_PageNo
    Converter_BitMapPage::ConstIterator::ReadDataPageNo() const
{
    return *m_Ptr;
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Converter_BitMapPage::Iterator
//------------------------------------------------------------------------------

inline Converter_BitMapPage::Iterator::Iterator
    (Pointer                        ptr,
     BlockCount                     slotsize)
    :
     m_Ptr(ptr),
     m_SlotSize(slotsize)
{}

//------------------------------------------------------------------------------

inline SAPDB_Bool
    Converter_BitMapPage::Iterator::operator!=
    (const Iterator&                iter)
{
    return (m_Ptr != iter.m_Ptr);
}

//------------------------------------------------------------------------------

inline Converter_BitMapPage::Iterator&
    Converter_BitMapPage::Iterator::operator++()
{
    SAPDBERR_ASSERT_ARGUMENT(m_Ptr != 0);
    SAPDBERR_ASSERT_ARGUMENT(m_SlotSize != 0);
    m_Ptr += m_SlotSize;
    return *this;
}

//------------------------------------------------------------------------------

inline void
    Converter_BitMapPage::Iterator::WriteDataPageNo
    (const Data_PageNo&             pno)
{
    *m_Ptr = pno;
}

//------------------------------------------------------------------------------

inline Data_PageNo
    Converter_BitMapPage::Iterator::ReadDataPageNo() const
{
    return *m_Ptr;
}

//------------------------------------------------------------------------------

inline void
    Converter_BitMapPage::Iterator::WriteBitMap
    (const SAPDBFields_Field&       map)
{
    SAPDBERR_ASSERT_ARGUMENT(map.GetLength() == (reinterpret_cast <SAPDB_ULong> (DataEnd()) -
                                                 reinterpret_cast <SAPDB_ULong> (DataBegin())));

    SAPDB_MemCopyNoCheck(DataBegin(), map.Begin(), map.GetLength());
}

//------------------------------------------------------------------------------

inline Converter_BitMapPage::Iterator::BlockIterator
    Converter_BitMapPage::Iterator::DataBegin() const
{
    return m_Ptr + 1;
}

//------------------------------------------------------------------------------

inline Converter_BitMapPage::Iterator::BlockIterator
    Converter_BitMapPage::Iterator::DataEnd() const
{
    return m_Ptr + m_SlotSize;
}

//------------------------------------------------------------------------------






#endif // CONVERTER_BITMAPPAGE_HPP

