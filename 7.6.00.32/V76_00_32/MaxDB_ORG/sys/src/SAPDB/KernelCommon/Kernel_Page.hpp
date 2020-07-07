/*!
@defgroup       PageHandling Describes persistent layout of pages, frames and their handler
*/
/*!
@file           Kernel_Page.hpp
@author         DataAccess group
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
#ifndef KERNEL_PAGE_HPP
#define KERNEL_PAGE_HPP

#include "ggg00.h"
#include "SAPDBCommon/SAPDB_PersistentTypes.hpp"
#include "IOManager/IOMan_IPage.hpp"
#include "DataAccess/Data_PageFrame.hpp"
/*!
@class Kernel_Page
@brief Virtual base class for all page handler classes in the kernel.

Defines frame handling and access to some header fields.
*/

class Kernel_Page : public IOMan_IPage
{
public:
    /// This is the frame which is handled.
    typedef Data_PageFrame             PageFrame;
    /// size of memory block on a page in bytes
    typedef Data_PageFrameSizeType     SizeType;
    /// offset on a page in bytes
    typedef Data_PageFrameOffsetType   OffsetType;
    /// Field for multiple usage by derived classes
    typedef SAPDB_UInt4                Id;
    /// Internal page parity check algorithm number
    enum ParityAlgorithm
    {
        CheckSum      = 1,
        HeaderTrailer = 2
    };
    /// Internal page parity check result type
    typedef SAPDB_UInt2                Parity;
    /// Constants for different page categories
    enum PageType
    {
        Kernel_InvalidPageType              = ptNil_egg00,      // these are
        Kernel_DataPageType                 = ptData_egg00,     // 7.2 page types
        Kernel_DumpPageType                 = ptDump_egg00,
        Kernel_FBMPageType                  = ptFbm_egg00,
        Kernel_LogPageType                  = ptLog_egg00,
        Kernel_SavePageType                 = ptSave_egg00,
        Kernel_SysPageType                  = ptSys_egg00,
        Kernel_VTracePageType               = ptVtrace_egg00,
        Kernel_ConverterPageType            = ptConverter_egg00,
        Kernel_ConverterBitMapPageType      = ptConverterBitMap_egg00,
        Kernel_LogInfoPageType              = ptLogInfo_egg00,
        Kernel_IOManInfoPageType            = ptIOManInfo_egg00,
        Kernel_FileDirPageType              = ptFileDir_egg00,
        RestartPage                         = ptRestart_egg00,             // UH 2004-05-18
        BasicPage                           = ptBasic_egg00,               // UH 2004-05-18
        VarSpace                            = ptVarSpace_egg00,            // UH 2004-05-18
        PrimaryFile                         = ptPrimaryFile_egg00,         // UH 2004-05-18
        PrimaryFileOverflow                 = ptPrimaryFileOverflow_egg00, // UH 2004-05-18
        PrimaryFileBlob                     = ptPrimaryFileBlob_egg00,     // UH 2004-05-18
        PrimaryFileLocks                    = ptPrimaryFileLocks_egg00     // UH 2004-05-18
    };
    /// Default constructor
    Kernel_Page()
    : m_Hint( 0 )
    {}
    /*!
    @brief  Constructor; assigns frame
    @param  frame [in] kernel page frame

    Does not change frame contents.
    */
    Kernel_Page( const PageFrame    &frame )
    :
    m_Frame( frame ),
    m_Hint ( 0 )
    {}
    /// Assigns frame; does not change frame contents
    void SetFrame( const PageFrame  &frame ){
        m_Frame = frame;
    }
    /// Deassigns page frame
    void ReleaseFrame(){
        m_Frame.Deassign();
    }
    /*!
    @brief  Returns reference to actual frame
    @return (const PageFrame&amp;) reference to frame
    */
    const PageFrame& GetFrame() const{
        return m_Frame;
    }
    /*!
    @brief  Returns reference to actual frame
    @return (PageFrame&amp;) reference to frame
    */
    PageFrame& GetFrame(){
        return m_Frame;
    }
    /// returns true if the page frame is assigned
    virtual SAPDB_Bool IsAssigned() const{
        return m_Frame.IsAssigned();
    }
    /// Reads page type from frame
    PageType ReadPageType() const
    {
        return static_cast<PageType>(HeaderPtr()->pagetype);
    }
    /// Returns the page type as string
    const SAPDB_Char* ReadPageTypeName() const
    {
        switch( ReadPageType())
        {
        case Kernel_InvalidPageType:
            return ( SAPDB_Char* )"nil";
        case Kernel_DataPageType:
            return ( SAPDB_Char* )"data";
        case Kernel_DumpPageType:
            return ( SAPDB_Char* )"dump";
        case Kernel_FBMPageType:
            return ( SAPDB_Char* )"fbm";
        case Kernel_LogPageType:
            return ( SAPDB_Char* )"log";
        case Kernel_SavePageType:
            return ( SAPDB_Char* )"save";
        case Kernel_SysPageType:
            return ( SAPDB_Char* )"sys";
        case Kernel_VTracePageType:
            return ( SAPDB_Char* )"vtrace";
        case Kernel_ConverterPageType:
            return ( SAPDB_Char* )"converter";
        case Kernel_ConverterBitMapPageType:
            return ( SAPDB_Char* )"conv bit map";
        case Kernel_LogInfoPageType:
            return ( SAPDB_Char* )"log info";
        case Kernel_IOManInfoPageType:
            return ( SAPDB_Char* )"ioman info";
        case Kernel_FileDirPageType:
            return ( SAPDB_Char* )"fdir";
        case RestartPage:
            return ( SAPDB_Char* )"Restart";
        case BasicPage:
            return ( SAPDB_Char* )"BasicPage";
        case VarSpace:
            return ( SAPDB_Char* )"VarSpace";
        case PrimaryFile:
            return ( SAPDB_Char* )"PrimaryFile";
        case PrimaryFileOverflow:
            return ( SAPDB_Char* )"PrimaryFileOverflow";
        case PrimaryFileBlob:
            return ( SAPDB_Char* )"PrimaryFileBlob";
        case PrimaryFileLocks:
            return ( SAPDB_Char* )"PrimaryFileLocks";
        default:
            return ( SAPDB_Char* )"UNDEF";
        }
    }
    /// Writes multi use value to frame
    void WritePageId( Id  id )
    {
        HeaderPtr()->pageid = id;
    }
    /// Reads pageid from frame
    Id ReadPageId() const
    {
        return HeaderPtr()->pageid;
    }

    /// Returns the pointer to the proper page frame for the IO Manager
    virtual Data_PageFrame::Pointer GetFramePtr(){
        return m_Frame.GetDataPtr();
    }

    /// Sets pointer to associated hint
    void SetHint( void*    hint ){
        m_Hint = hint;
    }
    /// Returns pointer to associated hint
    void* GetHint() const{
        return m_Hint;
    }
    /// Returns the size in bytes of the page frame
    virtual SAPDB_UInt GetSize() const{
        return m_Frame.GetLength();
    }
   
protected:

    /// initializes the page frame
    void InitializeFrame(Id       id,
                         PageType type)
    {
        WritePageId   (id);
        WritePageType (type);
        HeaderPtr()->parity = 0;
        HeaderPtr()->filler = 0;
    }
    /// Writes page type to frame
    void WritePageType( PageType    type ){
        HeaderPtr()->pagetype = type;
    }
    /// Returns size of page header
    SizeType GetHeaderSize() const{
        return sizeof( Header );
    }

protected:

    /// handle to frame which contains actual page
    PageFrame m_Frame;

    /// Hint pointer to additional data (for i/o purposes)
    void *m_Hint;
    
private:

    /// Persistent layout of data within kernel page header
    struct Header // 8 bytes
    {
        SAPDB_UInt4     pageid;    // logical identifier if page
        SAPDB_Byte      pagetype;
        SAPDB_UInt1     parity;    // 7.4: parity algorithm; 7.3: page type 2
        SAPDB_UInt2     filler;
    };

    /// Compile time size check for header
    typedef SAPDB_Persistent<Header,Header,
                             sizeof(SAPDB_UInt4) +
                             sizeof(SAPDB_Byte)  +
                             sizeof(SAPDB_UInt1) +
                             sizeof(SAPDB_UInt2)> CheckHeader;
    /// Returns pointer to page header on frame
    Header* HeaderPtr()
    {
        return reinterpret_cast<Header*>(m_Frame.GetDataPtr());
    }

    /// Returns const pointer to page header on frame
    const Header * HeaderPtr() const
    {
        return reinterpret_cast< const Header *>(m_Frame.GetDataPtr());
    }
};
#endif // KERNEL_PAGE_HPP
