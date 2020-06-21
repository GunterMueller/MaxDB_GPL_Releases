/*!
@file           Data_PageBasic.hpp
@author         UweH
@ingroup        PageHandling
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
#ifndef DATA_PAGE_BASIC_HPP
#define DATA_PAGE_BASIC_HPP

#include "IOManager/IOMan_IPage.hpp"
#include "DataAccess/Data_PageFrame.hpp"
#include "SAPDBCommon/SAPDB_EnhancedTypes.hpp"
#include "ggg00.h" // pagetypes

/*!
@brief This is the base class for all page handler classes (8 byte header and 2 bytes trailer)

The header and trailer
|ID                 |TYPE|*   |PARI|                     |CHECK    |
+----:----:----:----+----+----+----+ ....................+----:----+
|4                  |1   |1   |1   |                     |2 0xFEFE |

* is 1 byte divided into bits

|Length   |CheckAlgo|
+----:----+----:----+
|4 bit    |4 bit    |

CheckAlgo: 0 is none, 1 is CheckSum
Length   : 0 = 1*8192, 1 = 2*8192, 2 = 3*8192, 3 = 4*8192, ...
*/
class Data_PageBasic : public IOMan_IPage
{
public:
	/// This is the frame which is handled.
    typedef Data_PageFrame                              PageFrame;
	/// size of memory block on a page in bytes
    typedef Data_PageFrameSizeType                      SizeType;
	/// offset on a page in bytes
    typedef Data_PageFrameOffsetType                    OffsetType;
	/// Field for multiple usage by derived classes
    typedef SAPDB_Invalidatable<SAPDB_UInt4,0xFFFFFFFF> Id;
	/// Internal page parity check result type
    typedef SAPDB_UInt2                                 Parity;
	/// Internal page parity check algorithm number
    enum CheckAlgorithm
    {
        NoCheck       = 0,
        CheckSumCheck = 1
    };
	/// Constants for different page categories
    enum PageType
    {
        NilPageType                  = ptNil_egg00,
        DataPageType                 = ptData_egg00,
        DumpPageType                 = ptDump_egg00,
        FBMPageType                  = ptFbm_egg00,
        LogPageType                  = ptLog_egg00,
        SavePageType                 = ptSave_egg00,
        SysPageType                  = ptSys_egg00,
        VTracePageType               = ptVtrace_egg00,
        ConverterPageType            = ptConverter_egg00,
        ConverterBitMapPageType      = ptConverterBitMap_egg00,
        LogInfoPageType              = ptLogInfo_egg00,
        IOManInfoPageType            = ptIOManInfo_egg00,
        FileDirPageType              = ptFileDir_egg00,
        RestartPageType              = ptRestart_egg00,
        BasicPageType                = ptBasic_egg00,
        VarSpacePageType             = ptVarSpace_egg00,
        PrimaryFilePageType          = ptPrimaryFile_egg00,
        PrimaryFileOverflowPageType  = ptPrimaryFileOverflow_egg00,
        PrimaryFileBlobPageType      = ptPrimaryFileBlob_egg00,
        PrimaryFileLocksPageType     = ptPrimaryFileLocks_egg00
    };
	/// Default constructor frame is not assigned
    Data_PageBasic()
    {}
    /*!
    @brief          Constructor assigns empty frame and initializes frame contents
    @param          frame [in]
    @param          id    [in]
    @param          type  [in] page type, default: BasicPageType
    @param          algo  [in] parity algorithm (default: ChecksumCheck)
    */
    Data_PageBasic(const Data_PageFrame &frame,
                         Id              id,
                         PageType        type = BasicPageType,
                         CheckAlgorithm  algo = CheckSumCheck )
    : m_Frame( frame )
	{
        Initialize(id,type,algo);
    }
	/// Constructor which assigns frame. frame must be a valid page frame
    Data_PageBasic( const Data_PageFrame &frame )
    : m_Frame( frame )
    {
        if ( IsAssigned() && ! CheckTrailer() )
            FrameIsNotValid(__CONTEXT__);
    }
    /*!
    @brief          Initializes frame contents
    @param          id   [in] page id
    @param          type [in] page type, default: BasicPageType
    @param          algo [in] parity algorithm id (default: CheckSumCheck)
    */
    void Initialize(Id             id   = Id(),
                    PageType       type = BasicPageType,
                    CheckAlgorithm algo = CheckSumCheck );
	/// Assigns frame
    void Assign( const Data_PageFrame &frame )
    {
        m_Frame = frame;
    }
	/// Deassigns page frame
    void Deassign()
    {
        m_Frame.Deassign();
    }
	/// returns true if a frame is assigned
    bool IsAssigned() const
    {
        return m_Frame.IsAssigned();
    }
	/// Writes multi use value to frame
    void WritePageId (Id id)
    {
        HeaderPtr()->id = id;
    }
	/// Reads pageid from frame
    Id ReadPageId() const
    {
        return HeaderPtr()->id;
    }
    /// Reads page type from frame
    PageType ReadPageType() const
    {
        return static_cast<PageType>(HeaderPtr()->type);
    }
    /// Returns the page type as string
    const SAPDB_Char* ReadPageTypeName() const;
    /// Reads parity check algorithm number from frame
    CheckAlgorithm ReadCheckAlgorithm() const
    {
        // +++++ HeaderPtr()->lengthAndCheckAlgo
        return CheckSumCheck;
    }
	/// Returns the pointer to the proper page frame for the IO Manager
    virtual Data_PageFrame::Pointer GetFramePtr()
    {
        return m_Frame.GetDataPtr();
    }
    /*!
    @brief   Checks page after read from volume
    @return  true if check successful
    Performs parity check on the frame contents if possible.
    It depends on the algo bit in the header
    Recalculates parity from frame contents and compares the result
    with parity bytes kept in page header.
    Applicable immediately after a page is read from device
    */
    virtual SAPDB_Bool CheckAfterRead() const;
    /*!
    @brief          Prepares page for output to volume
    @return         true if check successful
    Reads and check parity information of frame contents
    Updates parity information with actual frame contents
    Applicable immediately before a page is written to device
    */
    virtual SAPDB_Bool PrepareForWrite();
    /// Returns the size in bytes of the page frame
    virtual SAPDB_UInt GetSize() const{
        return m_Frame.GetLength();
    }

protected:

	/// Writes page type to frame
    void WritePageType( PageType type )
    {
        HeaderPtr()->type = type;
    }
    /// Returns size of used page header
    SizeType GetHeaderSize() const
    {
        return sizeof(Header);
    }
    /// Returns size of used page trailer
    SizeType GetTrailerSize() const
    {
        return sizeof(Trailer);
    }

protected:

    /// handle to frame which contains actual page
    Data_PageFrame m_Frame;
    
private:

    /// Persistent layout of data within kernel page header
    struct Header // 8 bytes
    {
        SAPDB_UInt4     id;
        SAPDB_UInt1     type;
        SAPDB_UInt1     lengthAndCheckAlgo; // length = 2 bit, checkAlgo = 2 bit
        SAPDB_UInt2     parity;
    };
    /// Persistent layout of data within kernel page trailer
	typedef SAPDB_UInt2 Trailer;
	/// Returns pointer to page header on frame
    void InitTrailer()
    {
        *(TrailerPtr()) = 0xFEFE;
    }
    /// returns false if trailer is not initialized
    bool CheckTrailer() const
    {
        return *(TrailerPtr()) == 0xFEFE;
    }
	/// Returns pointer to page header on frame
    Header* HeaderPtr()
    {
        return reinterpret_cast<Header*>(m_Frame.GetDataPtr());
    }
    /// Returns const pointer to page header on frame
    const Header * const HeaderPtr() const
    {
        return reinterpret_cast< const Header * const>(m_Frame.GetDataPtr());
    }
	/// Returns pointer to page trailer on frame
    Trailer* TrailerPtr()
    {
        if ( ! IsAssigned() )
            return 0;
        PageFrame::Pointer ptr = m_Frame.GetDataPtr()
                                 +
                                 m_Frame.GetLength()
                                 -
                                 sizeof(Trailer);
        if ( ptr - m_Frame.GetDataPtr() <= 0 )
            ptr = m_Frame.GetDataPtr() + 10;
        return reinterpret_cast<Trailer*>(ptr);
    }
    /// Returns const pointer to page trailer on frame
    const Trailer * const TrailerPtr() const
    {
        if ( ! IsAssigned() )
            return 0;
        return reinterpret_cast<Trailer*>( m_Frame.GetDataPtr()
                                           +
                                           m_Frame.GetLength() - sizeof(Trailer) );
    }
    /// Writes the length of the frame to the frame. If length < 8192 then 8192 is set
    void WriteFrameLength()
    {
        #define MIN_FRAME_LENGTH      8192
        #define MIN_FRAME_LENGTH_CODE 0
        
        SAPDB_UInt1 length = (m_Frame.GetLength() / MIN_FRAME_LENGTH) - 1;
        
        if ( length > 0xF )
            length = 0xF;
        else
            if ( m_Frame.GetLength() < MIN_FRAME_LENGTH )
                length = MIN_FRAME_LENGTH_CODE;

        HeaderPtr()->lengthAndCheckAlgo = (length << 4)
                                          |
                                          (HeaderPtr()->lengthAndCheckAlgo & 0x0F);
    }
    /// Writes multi use value to frame
    void WriteCheckAlgorithm( CheckAlgorithm  algo )
    {
        SAPDB_UInt1 length = HeaderPtr()->lengthAndCheckAlgo & 0xF0;
        HeaderPtr()->lengthAndCheckAlgo = algo <= 0xF ? length | algo : length;
    }
	/// Initalizes page frame with bitmap
    void InitializeParityRaster();
	/// Calculates and returns parity of frame contents
    Parity CalcParity() const;
	/// Calculates parity depending on frame contents
    void UpdateParity()
    {
        if ( ReadCheckAlgorithm() != CheckSumCheck )
            HeaderPtr()->parity = 0;
        else
            HeaderPtr()->parity = CalcParity();
    }
    /// This implements the assertion
    void FrameIsNotValid(SAPDB_Char  const * const  filename,
                         SAPDB_UInt4 const          linenumber);
};
// -----------------------------------------------------------------------------------------------
inline void Data_PageBasic::Initialize(Id             id,
                                       PageType       type,
                                       CheckAlgorithm algo)
{
    WritePageId            (id);
    WritePageType          (type);
    WriteCheckAlgorithm    (algo);
    WriteFrameLength       ();
    InitTrailer            ();
    InitializeParityRaster ();
    UpdateParity           ();
}
// -----------------------------------------------------------------------------------------------
inline const SAPDB_Char* Data_PageBasic::ReadPageTypeName() const
{
    switch( ReadPageType() )
    {
    case NilPageType:
        return ( SAPDB_Char* )"Nil";
    case DataPageType:
        return ( SAPDB_Char* )"Data";
    case DumpPageType:
        return ( SAPDB_Char* )"Dump";
    case FBMPageType:
        return ( SAPDB_Char* )"FBM";
    case LogPageType:
        return ( SAPDB_Char* )"Log";
    case SavePageType:
        return ( SAPDB_Char* )"Save";
    case SysPageType:
        return ( SAPDB_Char* )"Sys";
    case VTracePageType:
        return ( SAPDB_Char* )"Vtrace";
    case ConverterPageType:
        return ( SAPDB_Char* )"Converter";
    case ConverterBitMapPageType:
        return ( SAPDB_Char* )"ConvBitmap";
    case LogInfoPageType:
        return ( SAPDB_Char* )"LogInfo";
    case IOManInfoPageType:
        return ( SAPDB_Char* )"IOManInfo";
    case FileDirPageType:
        return ( SAPDB_Char* )"FDir";
    case RestartPageType:
        return ( SAPDB_Char* )"Restart";
    case BasicPageType:
        return ( SAPDB_Char* )"Basic";
    case VarSpacePageType:
        return ( SAPDB_Char* )"VarSpace";
    case PrimaryFilePageType:
        return ( SAPDB_Char* )"PrimaryFile";
    case PrimaryFileOverflowPageType:
        return ( SAPDB_Char* )"PrimaryFileOverflow";
    case PrimaryFileBlobPageType:
        return ( SAPDB_Char* )"PrimaryFileBlob";
    case PrimaryFileLocksPageType:
        return ( SAPDB_Char* )"PrimaryFileLocks";
    default:
        return ( SAPDB_Char* )"UNKNOWN";
    }
}

#endif // DATA_PAGE_BASIC_HPP
