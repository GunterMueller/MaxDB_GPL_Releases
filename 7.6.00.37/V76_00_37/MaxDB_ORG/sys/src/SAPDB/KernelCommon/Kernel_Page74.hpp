/*!
  @file           Kernel_Page74.hpp
  @author         DataAccessGroup
  @ingroup        PageHandling
  @brief          7.4 page handler base class

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
#ifndef KERNEL_PAGE74_HPP
#define KERNEL_PAGE74_HPP

#include "KernelCommon/Kernel_Page.hpp"

/*!
@class          Kernel_Page74
@brief          Base class for all 7.4 page handler classes.

Specializes Kernel_Page in using the "multi use field 2" for storing the "parity algorithm" identifier.
Yet undefined pure virtual methods inherited from IOMan_IPage
- CheckAfterRead ()
- PrepareForWrite()
*/
class Kernel_Page74 : public Kernel_Page
{
public:

	/// Default constructor
    Kernel_Page74()
    {}
    /*!
    @brief          Constructor; assigns frame
    @param          frame [in] kernel page frame
    Does not change frame contents.
    */
    Kernel_Page74( const PageFrame   &frame ):Kernel_Page( frame )
    {}
    /*!
    @brief          Constructor; assigns frame and initializes frame contents
    @param          frame [in] kernel page frame
    @param          type [in] page type
    @param          algo [in] parity algorithm id (default: 1)
    */
    Kernel_Page74(
        const PageFrame &frame,
        PageType        type,
        ParityAlgorithm algo = CheckSum ):Kernel_Page( frame )
    {
        InitializeFrame( type, algo );
    }
    /*!
    @brief          Assigns frame and initializes frame contents
    @param          frame [in] kernel page frame
    @param          type [in] page type
    @param          algo [in] parity algorithm id (default: 1)
    */
    void SetAndInitializeFrame(
        const PageFrame &frame,
        PageType        type,
        ParityAlgorithm algo = CheckSum )
    {
        Kernel_Page::SetFrame( frame );
        InitializeFrame( type, algo );
    }
    /*!
    @brief          Initializes frame contents
    @param          type [in] page type
    @param          algo [in] parity algorithm id (default: 1)
    */
    void InitializeFrame(
        PageType        type,
        ParityAlgorithm algo = CheckSum )
    {
        InitializeParityRaster();
        InitFiller();
        WritePageType( type );
        WriteParityAlgorithm( algo );
        UpdateParity();
    }
    /*!
    @brief          Prepares page for input from device
    @return         (SAPDB_Bool) true if check successful
    Performs parity check on the frame contents
    Recalculates parity from frame contents and compares the result
    with parity bytes kept in page header and trailer
    Applicable immediately after a page is read from device
    */
    virtual SAPDB_Bool CheckAfterRead() const;
    /*!
    @brief          Prepares page for output to device
    @return         (SAPDB_Bool) true if check successful

    Reads and check parity information of frame contents
    Updates parity information with actual frame contents
    Applicable immediately before a page is written to device
    */
    virtual SAPDB_Bool PrepareForWrite();
    /*!
    @brief          Verifies consistency of page contents
    @return         (SAPDB_Bool) true if verify successful
    */
    virtual SAPDB_Bool Verify(bool                  isAdmin,
                              SAPDBErr_MessageList &errlist) const;
    /// Reads parity check algorithm number from frame
    ParityAlgorithm ReadParityAlgorithm() const
    {
        return ParityAlgorithm(PersistentHeaderPtr()->m_Algorithm);
    }
	/*!
    @brief Calculates and returns parity of frame contents
    Does not perform any changes on the frame contents
    Uses intrinsic parity algorithm of this class
    */
    virtual Parity CalcParity() const;
	/// Reads parity value kept in page header from frame
    Parity ReadParityFromHeader() const
    {
        return PersistentHeaderPtr()->m_Parity;
    }
	/// Reads parity value kept in page trailer from frame
    Parity ReadParityFromTrailer() const
    {
        return PersistentTrailerPtr()->m_Parity;
    }

#if defined(AIX) || defined(HPUX) || defined(SUN) || defined(SOLARIS)
public:
#else
protected:
#endif

    struct PersistentHeader
    {
        Kernel_Page::Id   m_PageId;
        SAPDB_UInt1       m_PageType;
        SAPDB_UInt1       m_Algorithm;
        SAPDB_UInt2       m_Parity;
    };

    struct PersistentTrailer
    {
        SAPDB_UInt2 m_MultiUseT1;
        SAPDB_UInt2 m_Parity;
    };

private:

	/// Compile time size check for header
    typedef SAPDB_Persistent <PersistentHeader, PersistentHeader,
                              sizeof(Kernel_Page::Id) +
                              sizeof(SAPDB_UInt1) +
                              sizeof(SAPDB_UInt1) +
                              sizeof(SAPDB_UInt2) > CheckedPersistentHeader;
	/// Compile time size check for trailer
    typedef SAPDB_Persistent <PersistentTrailer, PersistentTrailer,
                              sizeof(SAPDB_UInt2) +
                              sizeof(SAPDB_UInt2) > CheckedPersistentTrailer;

protected:

	/// Returns size of page header
    SizeType GetHeaderSize() const
    {
        return sizeof(PersistentHeader);
    }
	/// Returns size of page trailer
    SizeType GetTrailerSize() const
    {
        return sizeof(PersistentTrailer);
    }
	/// Returns pointer to page header on frame
    PersistentHeader* PersistentHeaderPtr()
    {
        return reinterpret_cast<PersistentHeader*>(m_Frame.GetDataPtr());
    }
	/// Returns const pointer to page header on frame
    const PersistentHeader* PersistentHeaderPtr() const
    {
        return reinterpret_cast<const PersistentHeader*>(m_Frame.GetDataPtr());
    }
	/// Returns pointer to page trailer on frame
    PersistentTrailer* PersistentTrailerPtr()
    {
        return reinterpret_cast<PersistentTrailer*>
            (m_Frame.GetPointer(m_Frame.GetLength() - sizeof(PersistentTrailer)));
    }
	/// Returns pointer to page trailer on frame
    const PersistentTrailer* PersistentTrailerPtr() const
    {
        return reinterpret_cast<const PersistentTrailer*>
            (m_Frame.GetPointer(m_Frame.GetLength() - sizeof(PersistentTrailer)));
    }

protected:

	/// Calculates parity depending on frame contents
    void UpdateParity()
    {
        PersistentHeaderPtr()->m_Parity  = CalcParity();
        PersistentTrailerPtr()->m_Parity = PersistentHeaderPtr()->m_Parity;
    }

private:

    /// Writes multi use value to frame
    void WriteParityAlgorithm( ParityAlgorithm  algo )
    {
        PersistentHeaderPtr()->m_Algorithm = algo;
    }
	/// Writes zeros to unused header and trailer parts
    void InitFiller()
    {
        WritePageId( 0 );
    }
	/// Initalizes page frame with bitmap
    void InitializeParityRaster();
};
#endif // KERNEL_PAGE74_HPP
