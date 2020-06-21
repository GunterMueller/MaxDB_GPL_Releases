/*!
  @file           Kernel_DumpPage.hpp
  @author         TorstenS
  @ingroup        PageHandling
  @brief          This class is used to store main memory structures on a dump page.

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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
#ifndef KERNEL_DUMPPAGE_HPP
#define KERNEL_DUMPPAGE_HPP

#include "KernelCommon/Kernel_Common.hpp"
#include "KernelCommon/Kernel_Page.hpp"
#include "SAPDBCommon/Fields/SAPDBFields_Field.hpp"

#define KERNEL_DUMP_LABEL_LENGTH    8
#define KERNEL_DUMP_FILL_BYTE       0xFE

/*!
@class  Kernel_DumpPage
@brief  This class is used to store main memory structures on a dump page.
*/
class Kernel_DumpPage : public Kernel_Page
{
public:
    ///
    typedef SAPDBFields_Field   Entry;
	/// This is the default constructor of a dump page..
    Kernel_DumpPage()
    :m_InsertPosition( 0 )
    {}
    /*!
       @brief          Construct a dump page with a given page image.  No initialization is done.
       @param          Frame [in] dump page
       @return         none
     */
    Kernel_DumpPage( const PageFrame &Frame )
    :
    Kernel_Page( Frame ),
    m_InsertPosition( 0 )
    {}

    /*!
       @brief          Initializes frame contents of a dump page. The dump page
              consists of a eight byte ordinary page header followed by
              a eight byte dump label, a dump code and a counter which 
              specifies who often the dump entry with the same dump 
              label occurs on the current page.
       @param          PageNo [in] consecutive numbering of the dump pages
       @param          DumpLabel [in] string identifier for the given dump entry
       @param          DumpCode [in] numeric identifier for the given dump entry
     */
    void InitializeFrame(
        SAPDB_UInt4     PageNo,
        SAPDB_Char      *DumpLabel,
        SAPDB_UInt2     DumpCode )
    {
        Kernel_Page::InitializeFrame(PageNo, Kernel_DumpPageType);
        memcpy( HeaderPtr()->m_DumpLabel, DumpLabel,
                sizeof( HeaderPtr()->m_DumpLabel ));
        HeaderPtr()->m_DumpCode = DumpCode;
        HeaderPtr()->m_Counter = 0;

        m_InsertPosition = GetHeaderSize();
    }
    /*!
       @brief          Initializes page without writing dump page specific 
              informations on it. This is necessary for page dumps.
       @return         none
     */
    void InitializeFrame()
    {
        m_InsertPosition = 0;
    }
    virtual SAPDB_Bool CheckAfterRead() const
    {
        return( Kernel_DumpPageType == ReadPageType() );
    }
    /*!
       @brief Prepares page for output to device
       @return true if check successful
       - Fills the remaining space of the dump page with a special symbol
       - Applicable immediately before a page is written to device
     */
    virtual SAPDB_Bool PrepareForWrite()
    {
        if( 0 == RemainingSpace() )
            return( true );

        SAPDB_MemFillNoCheck( m_Frame.GetPointer( m_InsertPosition, RemainingSpace() ),
                              KERNEL_DUMP_FILL_BYTE, RemainingSpace() );

        m_InsertPosition = m_Frame.GetLength();
        return( true );
    }
    /*!
       @brief          Verifies consistency of page contents
       @return         true if verify successful
     */
    SAPDB_Bool Verify(bool                  isAdmin,
                      SAPDBErr_MessageList &errlist) const
    {
        return( CheckAfterRead() );
    }

    /*!
       @brief          This method appends a dump entry to the current dump page.
              It is a assumed that the given dump entry belongs to the 
              dump entries already store within the page.
              All stored dump entries on this page titled with the same
              dump label and dump code! After each successful insert the
              counter for the stored dump entries will be incremented.
              Note that the writing of dump entries greater than a dump 
              page less the header size is not supported by this method!
       @param          DumpEntry [in] given dump entry
       @return         true means insert was successful; else false

     */
    SAPDB_Bool Insert( const Entry  &DumpEntry )
    {
        SAPDBERR_ASSERT_STATE( DumpEntry.IsAssigned() );

        const SAPDB_UInt4 EntrySize = DumpEntry.GetLength();

        if( ! IsSpaceAvailable( EntrySize ))
            return( false );

        memcpy( m_Frame.GetPointer( m_InsertPosition, RemainingSpace() ),
                DumpEntry.GetPointer( 0, EntrySize ), EntrySize );

        m_InsertPosition += DumpEntry.GetLength();

        // Count the number of equal dump entries on this page.
        // This is necessary for the analyzer of the dump.
        HeaderPtr()->m_Counter = HeaderPtr()->m_Counter + 1;

        return( true );
    }
    /*!
       @brief          This method inserts a given page in the current dump page.
              It is expected that the current dump page is empty!
       @param          Page [in] kernel page handler
       @return         true if insert was successfully; else false

     */
    SAPDB_Bool InsertPage( const Kernel_Page  &Page )
    {
        SAPDBERR_ASSERT_STATE( 0 == m_InsertPosition );
        SAPDBERR_ASSERT_STATE( Page.IsAssigned() );
        SAPDBERR_ASSERT_STATE( m_Frame.GetLength() == Page.GetFrame().GetLength() );

        if( ! IsSpaceAvailable( Page.GetFrame().GetLength() ))
            return  false;

        if ( ! m_Frame.Copy(Page.GetFrame()) ) 
            return false;
            
        m_InsertPosition = m_Frame.GetLength();
        return true;
    }
    /*!
       @brief          This method examines whether the given space for the new dump
              entry is still available on the dump page.
       @param          WantedSpace [in] size in bytes of the requested space.
       @return         true means space is available; else false
     */
    SAPDB_Bool IsSpaceAvailable( SAPDB_UInt4 WantedSpace ) const
    {
        return( RemainingSpace() >= WantedSpace );
    }
    /*!
       @brief          This method indicates whether a dump page contains dump entries.
       @return         true means page contains dump entries; else false
     */
    SAPDB_Bool IsUsed() const
    {
        return(( IsAssigned() ) && ( m_InsertPosition > 0 ));
    }

protected:

    /// Returns size of page header
    SizeType GetHeaderSize() const
    {
        return Kernel_Page::GetHeaderSize() + sizeof(Header);
    }

private:

    struct Header
    {
        SAPDB_Char   m_DumpLabel[ KERNEL_DUMP_LABEL_LENGTH ];
        SAPDB_UInt2  m_DumpCode;
        SAPDB_UInt2  m_Counter;
    };

    Header* HeaderPtr()
    {
        return reinterpret_cast< Header* >( m_Frame.GetDataPtr() + Kernel_Page::GetHeaderSize());
    }

    const Header* HeaderPtr() const
    {
        return reinterpret_cast< const Header* >( m_Frame.GetDataPtr() + Kernel_Page::GetHeaderSize());
    }

    SAPDB_UInt4 RemainingSpace() const
    {
        return( m_Frame.GetLength() - m_InsertPosition );
    }

private:

    /// Position within the page at which the next dump entry
    /// will be materialized. This is the first unused byte!
    SAPDB_UInt4 m_InsertPosition;
};
#endif  /* KERNEL_DUMPPAGE_HPP */
