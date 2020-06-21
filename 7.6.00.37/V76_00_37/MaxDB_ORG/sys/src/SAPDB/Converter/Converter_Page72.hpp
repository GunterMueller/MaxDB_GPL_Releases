/*!
  @file           Converter_Page72.hpp
  @author         TorstenS
  @ingroup        Converter
  @brief          Page handler for old converter pages (&lt; 7.4) needed for migration
*/

/*
    ========== licence begin  GPL
    Copyright (c) 2001-2004 SAP AG

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


#ifndef CONVERTER_PAGE72_HPP
#define CONVERTER_PAGE72_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 **/

#include "DataAccess/Data_Types.hpp"
#include "KernelCommon/Kernel_Page72.hpp"
#include "KernelCommon/Kernel_Common.hpp"
#include "IOManager/IOMan_BlockAddress.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 **/


/*===========================================================================*
 *  MACROS                                                                   *
 **/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 **/

class Converter_Page72EntryIterator;

/*!
   @class          Converter_Page72
   @brief          This class is used for data base migration, inparticular
                   the migration of converter pages from release 7.2/7.3 to
                   7.4 layout.
 */
class Converter_Page72 : public Kernel_Page72
{
    friend class Converter_Page72EntryIterator;
public:

    /* -----------------------------------------------------------------------*/

    /*! @name Constructors and initialization */

    /* -----------------------------------------------------------------------*/

    static SAPDB_UInt4  GetEntryCount()
    {
        return( 1855 ); // MAX_CONV_REC_BD00 from 7.2/7.3
    }

    /*!
       @brief          Construct an old converter page with a given page image.
       @param          frame [in] old converter page
       @return         none
     */
    Converter_Page72( const PageFrame &frame ):Kernel_Page72( frame ){}

    /* -----------------------------------------------------------------------*/

    /*! @name Converter_Page72 I/O management access */
    
    /* -----------------------------------------------------------------------*/

    /*!
       @brief          Prepares page for input from device
       @return         true if check successful

              - Performs parity check on the frame contents
              - Recalculates parity from frame contents and compares
                the result with parity bytes kept in page header and trailer
              - Applicable immediately after a page is read from device
     */
    virtual SAPDB_Bool CheckAfterRead() const;

    /*!
       @brief          not needed, because the old converter pages will never
                       be written onto a devspace. This will be done by the new
                       IOManager methods
       @return         (SAPDB_Bool) true if check successful
     */
    virtual SAPDB_Bool PrepareForWrite()
    {
        return( false );
    }

    /* -----------------------------------------------------------------------*/

    /*! @name Iterators */
    
    /* -----------------------------------------------------------------------*/

    /*!
       @brief          Returns iterator at first entry on page
       @return         (Converter_Page72EntryIterator)
     */
    Converter_Page72EntryIterator Begin();

    /*!
       @brief          Returns iterator behind last entry on page
       @return         (Converter_Page72EntryIterator)
     */
    Converter_Page72EntryIterator End();

    /* -----------------------------------------------------------------------*/

    /*! @name Miscellaneous */
    
    /* -----------------------------------------------------------------------*/

    Data_PageNo GetDataBaseNo() const
    {
        return( Kernel_Page72::GetPageID() * GetEntryCount() );
    }

    /* -----------------------------------------------------------------------*/

#if defined(AIX) || defined(HPUX) || defined(SUN) || defined(SOLARIS)
public:
#else
private:
#endif

    /// Persistent layout of pascal converter page node defined in gbd00
    typedef Kernel_Page72::PersistentHeader     PH1; // page72 header ( 8 bytes )

    /// Persistent layout of pascal converter page node defined in gbd00
    typedef struct 
    {
        /// write counter
        SAPDB_Int4  m_WriteCount;       // from byte  9 to byte 12
        /// current converter version of page
        SAPDB_Int4  m_ConverterVersion; // from byte 13 to byte 16
    } PH2;        // layout of tbd00_ConvPage Header

protected:

    /// Persistent layout of pascal converter page node defined in gbd00
    struct PersistentHeader
    {
        /// persistent header of all pages
        PH1     m_KernelPage72Header;
        /// persistent header of this page type
        PH2     m_RemainingConverterHeader;
    };

protected:

    /* -----------------------------------------------------------------------*/
    /*! @name Interface (protected) */
    
    /*!
       @brief          Returns size of page header
       @return         (SizeType) header size
     */
    virtual SizeType GetHeaderSize() const
    {
        SAPDBERR_ASSERT_STATE( 16 == sizeof( PersistentHeader ));

        return( sizeof( PersistentHeader )); // CVP_HEADER_MXBD00 from 7.2/7.3
    }

    /* -----------------------------------------------------------------------*/

private:

    /*--------------------------------------------------------------------------
    function:     PersistentHeaderPtr()
    description:  Returns pointer to page header on frame
    arguments:    none
    prototypes:   2
    return value: (PersistentHeader*) header pointer
    --------------------------------------------------------------------------*/

    PersistentHeader* PersistentHeaderPtr()
    {
        return
            reinterpret_cast< PersistentHeader* >
                              ( m_Frame.GetPointer( 0, sizeof( PersistentHeader )));
    }

    const PersistentHeader* PersistentHeaderPtr() const
    {
        return
            reinterpret_cast< const PersistentHeader* >
                              ( m_Frame.GetPointer( 0, sizeof( PersistentHeader )));
    }

    /*--------------------------------------------------------------------------
    function:     GetWriteCount()
    description:  This method provides the write count. This indicates how
                  often the page was written in the past.
    arguments:    none
    return value: SAPDB_Int4
    --------------------------------------------------------------------------*/

    SAPDB_Int4 GetWriteCount() const
    {
        return( PersistentHeaderPtr()->m_RemainingConverterHeader.m_WriteCount );
    }

    /*--------------------------------------------------------------------------
    function:     CheckPageForIO()
    description:  This method  verifies a converter page, i.e. the 
                  header/trailer check is executed. The page type and sub type
                  is checked and if the "write count" check algorithmn is active
                  the write counter and the check sum counter will be compared.
                  If all checks are executed succesful true is returned.
    arguments:    none
    return value: SAPDB_Bool
    --------------------------------------------------------------------------*/

    SAPDB_Bool CheckPageForIO() const;

    /*--------------------------------------------------------------------------
    function:     CalculateChecksum()
    description:  This method is used to calculate the check sum over the given
                  page. The calculate check sum is return.
    arguments:    none
    return value: Kernel_Page72::PageChecksum
    --------------------------------------------------------------------------*/

    Kernel_Page72::PageChecksum CalculateChecksum() const;

    /*--------------------------------------------------------------------------
    function:     GetPersistentBlockAddress()
    description:  This method returns the persistent block address of for a
                  given converter entry.
    arguments:    entryNo [in] logical number of the wanted converter entry
    return value: SAPDB_UInt4
    --------------------------------------------------------------------------*/

    SAPDB_UInt4 GetPersistentBlockAddress( const SAPDB_UInt4 entryNo )
    {
        SAPDBERR_ASSERT_STATE( entryNo < GetEntryCount() );

        const SAPDB_UInt4 *pblock =
            reinterpret_cast<SAPDB_UInt4*>( GetFramePtr() + GetHeaderSize() );

        return( *( pblock + entryNo ));
    }

    /*--------------------------------------------------------------------------
    function:     GetPointerToConverterFlagField()
    description:  This method returns a pointer to the beginning of the converter
                  flag field, which contains the recoverability and the save
                  pages information.
    arguments:    none
    return value: const SAPDB_Int4* 
    --------------------------------------------------------------------------*/

    const SAPDB_Int4* GetPointerToConverterFlagField()
    {
        const SAPDB_Int4 *pFlag =
            reinterpret_cast<SAPDB_Int4*>( GetFramePtr() + GetHeaderSize() +
                                           GetEntryCount() * sizeof( SAPDB_UInt4 ));
        return( pFlag );
    }

private:

    // Copy and assignment operator and default constructor are not supported

    Converter_Page72();

    Converter_Page72& operator=( const Converter_Page72 &convPage );

    Converter_Page72( const Converter_Page72 &convPage );
};



/*!
   @class          Converter_Page72EntryIterator
   @brief          Steps over all entries on an old converter page This iterator 
                   makes no difference between used and unused entries
 */
class Converter_Page72EntryIterator
{
public:

    /*!
       @brief          Constructor
       @param          page [in] reference to converter page
       @param          entryNo [in] entry number defining start position on the page (default: 0)
       @return         none
     */
    Converter_Page72EntryIterator(
        Converter_Page72   &page,
        SAPDB_UInt4         entryNo = 0)
    :
    m_pPage( &page ),
    m_EntryNo( entryNo )
    {}

    /*!
       @brief          Assigns other iterator
       @param          iter [in] iterator to be assigned
       @return         none
     */
    Converter_Page72EntryIterator& operator=( const Converter_Page72EntryIterator& iter )
    {
        m_pPage   = iter.m_pPage;
        m_EntryNo = iter.m_EntryNo;
        return( *this );
    }

    /*!
       @brief          Compares with other iterator
       @param          iter [in] iterator to be compared with
       @return         (SAPDB_Bool) true if unequal
     */
    SAPDB_Bool operator!=( const Converter_Page72EntryIterator &iter ) const
    {
        SAPDBERR_ASSERT_STATE( m_pPage == iter.m_pPage );

        return( m_EntryNo != iter.m_EntryNo );
    }

    /*!
       @brief          Prefix increment
       @return         (Converter_Page72EntryIterator&amp;) this instance
     */
    Converter_Page72EntryIterator& operator++()
    {
        ++m_EntryNo;
        return( *this );
    }

    /*!
       @brief          Returns the block address the iterator points to
       @return         (IOMan_BlockAddress) actual block address
     */
    IOMan_BlockAddress GetBlockAddress() const
    {
        SAPDBERR_ASSERT_STATE( m_pPage->IsAssigned() );

        const SAPDB_UInt4    packedAddr = m_pPage->GetPersistentBlockAddress( m_EntryNo );
        const IOMan_BlockNo  blockNo    = packedAddr & 0xFFFFFFU;
        const IOMan_DeviceNo devNo      = packedAddr >> 24;

        IOMan_BlockAddress  block( devNo, blockNo );

        return( block );
    }

    /*!
       @brief          Checks whether actual entry is in permanent use 
                       data page
       @return         (SAPDB_Bool) true if entry is in use
     */
    SAPDB_Bool IsPermanentUsed() const;

    /*!
       @brief          Provides the data page no of the current converter entry
       @return         (Data_PageNo)
     */
    Data_PageNo GetDataPageNo() const
    {
        SAPDBERR_ASSERT_STATE( m_pPage->IsAssigned() );

        return( m_pPage->GetDataBaseNo() + m_EntryNo );
    }

private:

    /// Pointer the old converter page the iterator has to work on
    Converter_Page72    *m_pPage;

    /// logical number of a converter entry on an old converter page. Note that this number is not the data page no of the entry!
    SAPDB_UInt4 m_EntryNo;
};


#endif  /* CONVERTER_PAGE72_HPP */

