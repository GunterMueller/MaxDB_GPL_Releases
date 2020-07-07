/*!
  @file           Kernel_Page72.hpp
  @author         Till Luhmann, TorstenS
  @ingroup        Pages
  @brief          page handler base class

    The class kernel_page72 is used to encapsulate the old pages defined with pascal strucures.

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
#ifndef KERNEL_PAGE72_HPP
#define KERNEL_PAGE72_HPP

#include "KernelCommon/Kernel_Page.hpp"
#include "SAPDBCommon/SAPDB_PersistentTypes.hpp"
#include "SAPDBCommon/SAPDB_RangeCode.hpp"

class SAPDBErr_MessageList;
/*!
@class Kernel_Page72
@brief Base class for all 7.2 page handler classes.

Specializes Kernel_Page in using the "multi use field 1" as a "page id"
and the "multi use field 2" for storing the "page type 2" identifier.
*/
class Kernel_Page72 : public Kernel_Page
{
public:
    /// individual page identifier
    typedef SAPDB_Int4                                      PageId;
    /// Constants for different secondary page categories
    typedef tgg00_PageType2_Enum                            PageType2;
	/// Constants for different page check algorithms
    typedef tgg00_PageCheckType_Enum                        PageCheck;
    /// Result of parity algorithmen
    typedef SAPDB_Int4                                      PageChecksum;
	/// Default constructor
    Kernel_Page72()
    {}
    /*!
    @brief          Constructor; assigns frame
    @param          frame [in] kernel page frame
	Does not change frame contents.
    */
    Kernel_Page72( const PageFrame &frame)
    : Kernel_Page( frame )
    {}
    /*!
    @brief          Constructor; assigns frame and initializes frame contents
    @param          frame [in] kernel page frame
    @param          type [in] page type
    @param          type2 [in] page type 2
    */
    Kernel_Page72(
        const PageFrame &frame,
        PageType        type,
        PageType2       type2
    )
    :Kernel_Page( frame )
    {
        InitializeFrame( type, type2 );
    }
    /*!
    @brief          Assigns frame and initializes frame contents
    @param          frame [in] kernel page frame
    @param          type [in] page type
    @param          type2 [in] page type 2
    */
    void SetAndInitializeFrame(
        const PageFrame    &frame,
        PageType           type,
        PageType2          type2)
    {
        Kernel_Page::SetFrame( frame );
        InitializeFrame( type, type2 );
    }
    /*!
    @brief          Initializes frame contents
    @param          type [in] page type
    @param          type2 [in] page type 2
   */
    void InitializeFrame(
        PageType    type,
        PageType2   type2 )
    {
        WritePageType( type );
        SetPageType2( type2 );
    }
    /*!
       @brief          Verifies consistency of page contents
       @return         (SAPDB_Bool) true if verify successful
    */
    virtual SAPDB_Bool Verify(bool                  isAdmin,
                              SAPDBErr_MessageList &errlist) const
    {
        // dummy TODO
        return true;
    }
	/// Reads page id from frame
    PageId GetPageID() const
    {
        return PersistentHeaderPtr()->m_PageId;
    }
    /*!
       @brief          Reads page type2 from frame
       @return         (PageType2) algorithm number
     */
    PageType2 ReadPageType2() const
    {
        return PageType2(PersistentHeaderPtr()->m_PageType2);
    }
    /*!
       @brief          Changes page type2 in frame
       @param          type2 [in] page type2 to be set
       
       Header is copied to the trailer.
     */
    void ResetPageType2( PageType2  type2 )
    {
        PersistentHeaderPtr()->m_PageType2 = type2;
        CopyHeaderToTrailer();
    }

	/// Reads page mode from frame
    tgg00_PageMode GetPageMode() const
    {
        return PersistentHeaderPtr()->m_PageMode;
    }

#if defined(AIX) || defined(HPUX) || defined(SUN) || defined(SOLARIS)
public:
#else
protected:
#endif

    struct PersistentHeader
    {
        PageId         m_PageId;
        SAPDB_UInt1    m_PageType;
        SAPDB_UInt1    m_PageType2;
        SAPDB_UInt1    m_PageCheck;
        tgg00_PageMode m_PageMode;
    };

    typedef PersistentHeader PersistentTrailer;

    struct PersistentChecksumTrailer
    {
        SAPDB_UInt4          m_PageChecksum;
        PersistentTrailer    m_TrailerPageMode;
    };

	/// Compile time size check for header
    typedef SAPDB_Persistent < PersistentHeader, PersistentHeader,
                               sizeof(PageId)      +
                               sizeof(SAPDB_UInt1)    +
                               sizeof(SAPDB_UInt1) +
                               sizeof(SAPDB_UInt1) +
                               sizeof(tgg00_PageMode) > CheckedPersistentHeader;
	/// Compile time size check for trailer
    typedef SAPDB_Persistent < PersistentChecksumTrailer, PersistentChecksumTrailer,
                               sizeof(SAPDB_UInt4)       +
                               sizeof(PersistentTrailer) > CheckedPersistentChecksumTrailer;
protected:

	/// Returns size of page header
    SizeType GetHeaderSize() const
    {
        return sizeof( PersistentHeader );
    }
	/// Returns size of page trailer, without the checksum value
    SizeType GetTrailerSize() const
    {
        return sizeof( PersistentTrailer );
    }

    /*!
       @brief   compares the header with the trailer
       @param   msgList [out] in case of errors message list is filled
       @return  (bool) true if header equal trailer
     */

    bool CheckHeaderTrailer( SAPDBErr_MessageList &msgList ) const;

	/// CopyHeaderToTrailer()
    void CopyHeaderToTrailer()
    {
        SAPDB_MemCopyNoCheck( Kernel_Page72::PersistentTrailerPtr(),
                              Kernel_Page72::PersistentHeaderPtr(),
                              Kernel_Page72::GetHeaderSize());
    }
    /*!
       @brief          Writes page id to frame
       @param          pageid [in] Page id to be set
     */
    void SetPageID( PageId    pageid )
    {
        PersistentHeaderPtr()->m_PageId = pageid;
    }
    /*!
       @brief          Writes page type2 to frame
       @param          type2 [in] page type2 to be set
     */
    void SetPageType2( PageType2  type2 )
    {
        PersistentHeaderPtr()->m_PageType2 = type2;
    }
    /*!
       @brief          Reads page check algorithmen id from frame
       @return         (PageCheck) page check algorithmen id
     */
    PageCheck GetPageCheck() const
    {
        return PageCheck(PersistentHeaderPtr()->m_PageCheck);
    }
    /*!
       @brief          Writes page check algorithmen id to frame
       @param          check [in] page check algorithmen id to be set
     */
    void SetPageCheck( PageCheck  check )
    {
        PersistentHeaderPtr()->m_PageCheck = check;
    }
    /*!
       @brief          Reads page checksum value from frame
       @return         (PageChecksum) page checksum
     */
    PageChecksum GetPageChecksum() const
    {
        return PersistentChecksumTrailerPtr()->m_PageChecksum;
    }
    /*!
       @brief          Writes page mode to frame
       @param          mode [in] page mode to be set
     */
    void SetPageChecksum( PageChecksum  Checksum )
    {
        PersistentChecksumTrailerPtr()->m_PageChecksum = Checksum;
    }


private:

    /// Returns pointer to page header on frame
    PersistentHeader* PersistentHeaderPtr()
    {
        return reinterpret_cast<PersistentHeader*>(m_Frame.GetDataPtr());
    }
    /// Returns const pointer to page header on frame
    const PersistentHeader* PersistentHeaderPtr() const
    {
        return reinterpret_cast< const PersistentHeader *>(m_Frame.GetDataPtr());
    }
	/// Returns pointer to page trailer (without checksum) on frame
    PersistentTrailer* PersistentTrailerPtr()
    {
        return reinterpret_cast<PersistentTrailer*>
                (m_Frame.GetPointer(m_Frame.GetLength() - sizeof( PersistentTrailer )));
    }
	/// Returns const pointer to page trailer (without checksum) on frame
    const PersistentTrailer* PersistentTrailerPtr() const
    {
        return reinterpret_cast<const PersistentTrailer*>
                (m_Frame.GetPointer(m_Frame.GetLength() - sizeof( PersistentTrailer )));
    }
	/// Returns pointer to page trailer (with checksum) on frame
    PersistentChecksumTrailer* PersistentChecksumTrailerPtr()
    {
        return reinterpret_cast<PersistentChecksumTrailer*>
                (m_Frame.GetPointer(m_Frame.GetLength() - sizeof( PersistentChecksumTrailer )));
    }
	/// Returns const pointer to page trailer (with checksum) on frame
    const PersistentChecksumTrailer* PersistentChecksumTrailerPtr() const
    {
        return reinterpret_cast<const PersistentChecksumTrailer*>
                (m_Frame.GetPointer(m_Frame.GetLength() - sizeof( PersistentChecksumTrailer )));
    }
protected:
    /// Reads trailer page id from frame
    PageId GetTrailerPageID() const
    {
        return PersistentTrailerPtr()->m_PageId;
    }
	/// Reads trailer page type 2 from frame
    PageType2 ReadTrailerPageType2() const
    {
        return PageType2(PersistentTrailerPtr()->m_PageType2);
    }
	/// Reads trailer page check algorithmen number from frame
    PageCheck GetTrailerPageCheck() const
    {
        return PageCheck(PersistentTrailerPtr()->m_PageCheck);
    }
	/// Reads trailer page type from frame
    PageType ReadTrailerPageType() const
    {
        return PageType(PersistentTrailerPtr()->m_PageType);
    }
    /// Reads trailer page mode from frame
    tgg00_PageMode GetTrailerPageMode() const
    {
        return PersistentTrailerPtr()->m_PageMode;
    }
};
#endif // KERNEL_PAGE72_HPP
