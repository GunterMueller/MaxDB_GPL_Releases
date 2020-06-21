/*****************************************************************************/
/*!
 @file    FileDir_PageIndexPage.cpp
 @author  MartinKi
 @ingroup FileDirectory_Files

 @brief Implementation of class FileDir_PageIndexPage.

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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
/*****************************************************************************/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "FileDirectory/FileDir_PageIndexPage.hpp"
#include "FileDirectory/FileDir_Page.hpp"

// for Data_PageNo:
#include "DataAccess/Data_Types.hpp"
#include "DataAccess/Data_BasePage.hpp"

// to write page:
#include "IOManager/IOMan_IDataIO.hpp"

// PASCAL: GG_basic_constants_and_type
#include "ggg00.h"

// to init Data_BasePage:
#include "hbd01_1.h"
#include "hbd13.h"

#include "FrameControl/FrameCtrl_IFileDirectory.hpp"

// vgetpid():
#include "heo51.h"


#include "RunTime/RTE_Types.hpp"

#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

FileDir_PageIndexPage::FileDir_PageIndexPage(
    const Data_PageNo& pageNumber)
    : m_anchor(0),
      m_currentEntry(0),
      m_isDirty(true),
      m_initialized(false)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_PageIndexPage::FileDir_PageIndexPage",
                             FileDir_Trace, 5 );

    RTE_TaskId taskId;
    vgetpid( taskId );

    m_Frame = FrameCtrl_IFileDirectory::GetInstance().NewFDirFrame( taskId );

    if ( !m_Frame.IsAssigned() ) {
        return;
    }

    m_anchor =
        reinterpret_cast<char*>(m_Frame.GetPointer(0, m_Frame.GetLength()));

    if (m_anchor != 0) {
        // init header/trailer of Data_BasePage:
        tgg00_FileId fileId;
        tbd_node_ptrs np = { reinterpret_cast<tbd_nodeptr>(m_anchor),
                             0 };
        fileId = b01niltree_id;
        fileId.fileType_gg00().clear();
        fileId.fileType_gg00().addElement(ftsPerm_egg00);
        fileId.fileType_gg00().addElement(ftsConcurrent_egg00);
        fileId.fileRoot_gg00() = pageNumber;
        b13init_default_data_page(fileId,
                                  0,
                                  pageNumber,
                                  np);
        np.np_ptr()->nd_header().pageType_gg00().becomes( ptFileDir_egg00 );
        np.np_ptr()->nd_trailer().pageType_gg00().becomes( ptFileDir_egg00 );
        np.np_ptr()->nd_level() = FileDir_Common::FileDir_PageIndexPage;
        // end init header

        Data_PageNo invalidPage;
        this->SetNextPage( invalidPage );
        this->SetEntryCount( 0 );
        m_currentEntry = reinterpret_cast<PageType*>(&m_anchor[HeaderSize]);
    }
}

/*************************************************************************/

FileDir_PageIndexPage::~FileDir_PageIndexPage()
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_PageIndexPage::~FileDir_PageIndexPage",
                             FileDir_Trace, 5 );

    if ( m_Frame.IsAssigned() ) {
        RTE_TaskId taskId;
        vgetpid( taskId );
        FrameCtrl_IFileDirectory::GetInstance().FreeFDirFrame(
            taskId, m_Frame );
    }
}

/*************************************************************************/

SAPDB_Bool FileDir_PageIndexPage::Initialize()
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_PageIndexPage::Initialize",
                             FileDir_Trace, 5 );

    m_initialized = ( m_Frame.IsAssigned() && (m_anchor != 0) );
    return m_initialized;
}

/*************************************************************************/

SAPDB_Bool FileDir_PageIndexPage::ReinitializeFromContent()
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_PageIndexPage::ReinitializeFromContent",
                             FileDir_Trace, 5 );

    SAPDBERR_ASSERT_STATE( m_initialized );

    m_currentEntry =
        reinterpret_cast<PageType*>(&m_anchor[HeaderSize])
        + this->GetEntryCount();

    return true;
}

/*************************************************************************/

SAPDB_Bool FileDir_PageIndexPage::AddPage(const FileDir_Page& page)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_PageIndexPage::AddPage",
                             FileDir_Trace, 5 );

    SAPDBERR_ASSERT_STATE( m_initialized );

    if ( IsFull() ) {
        return false;
    }

    m_isDirty = true;

    *m_currentEntry++ = page.PageNo();
    this->ModifyEntryCount( 1 );
    return true;
}

/*************************************************************************/

SAPDB_Bool FileDir_PageIndexPage::RemovePage(const FileDir_Page& page)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_PageIndexPage::RemovePage",
                             FileDir_Trace, 5 );

    SAPDBERR_ASSERT_STATE( m_initialized );

    PageType* curPage = FirstEntryPos();
    SAPDBErr_MessageList messages;
    while ( curPage <= m_currentEntry ) {
        if ( *curPage == page.PageNo() ) {
            if ( curPage < m_currentEntry ) {
                SAPDB_MemMove(
                    curPage, curPage + 1,
                    (m_currentEntry - curPage) * sizeof(PageType),
                    "FileDirectory", __FILE__, __LINE__, messages );
            }
            m_isDirty = true;
            --m_currentEntry;
            this->ModifyEntryCount( -1 );
            return true;
        }
        ++curPage;
    }
    return false;
}
/*************************************************************************/

void FileDir_PageIndexPage::SetEntryCount(SAPDB_Int4 entryCount)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_PageIndexPage::SetEntryCount", FileDir_Trace, 5 );
    this->NodePtr()->nd_record_cnt() = entryCount;
}

/*************************************************************************/

void FileDir_PageIndexPage::ModifyEntryCount(SAPDB_Int4 delta)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_PageIndexPage::ModifyEntryCount", FileDir_Trace, 5 );

    this->NodePtr()->nd_record_cnt() += delta;
}

/*************************************************************************/

SAPDB_Int4 FileDir_PageIndexPage::GetEntryCount() const
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_PageIndexPage::ModifyEntryCount", FileDir_Trace, 5 );

    return this->NodePtr()->nd_record_cnt();
}

/*************************************************************************/

void FileDir_PageIndexPage::SetNextPage(const Data_PageNo& nextPage)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_PageIndexPage::SetNextPage", FileDir_Trace, 5 );

    this->NodePtr()->nd_right() = nextPage;
    m_isDirty = true;
}

/*************************************************************************/

Data_PageNo FileDir_PageIndexPage::GetNextPage() const
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_PageIndexPage::GetNextPage", FileDir_Trace, 5 );

    SAPDBERR_ASSERT_STATE( m_initialized );

    return Data_PageNo( this->NodePtr()->nd_right() );
}

/*************************************************************************/

SAPDB_Bool FileDir_PageIndexPage::IsFull() const
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_PageIndexPage::IsFull", FileDir_Trace, 5 );

    SAPDBERR_ASSERT_STATE( m_initialized );

    return
        (m_currentEntry + 1)
        >=
        reinterpret_cast<PageType*>(&m_anchor[ PageSize - TrailerSize ]);
}

/*************************************************************************/

SAPDB_Bool FileDir_PageIndexPage::IsEmpty() const
{
    SAPDBERR_ASSERT_STATE( m_initialized );

    return
        m_currentEntry
        ==
        reinterpret_cast<PageType*>(&m_anchor[ HeaderSize ]);
}

/*************************************************************************/

SAPDB_Bool FileDir_PageIndexPage::MergeRecommended() const
{
    SAPDBERR_ASSERT_STATE( m_initialized );

    return
        m_currentEntry
        <
        reinterpret_cast<PageType*>(
            &m_anchor[ HeaderSize + PageDataSize / 2 ]);
}

/*************************************************************************/

SAPDB_UInt FileDir_PageIndexPage::Capacity() const
{
    return PageDataSize / sizeof(PageType);
}

/*************************************************************************/

void FileDir_PageIndexPage::Flush(const RTE_TaskId taskId)
{
    SAPDBERR_ASSERT_STATE( m_initialized );

    IOMan_IDataIO::GetInstance().WriteDataPage( taskId, *this );
}

/*************************************************************************/
/*************************************************************************/

FileDir_PageIndexPage::Iterator&
FileDir_PageIndexPage::Iterator::operator++()
{
    if ( ((m_current+1) == m_indexPage->m_currentEntry)
         || (m_current == 0) ) {
        m_current = 0;
    } else {
        ++m_current;
    }
    return *this;
}

/*************************************************************************/

FileDir_PageIndexPage::Iterator
FileDir_PageIndexPage::Iterator::operator++(int)
{
    Iterator i = *this;
    ++(*this);
    return i;
}

/*************************************************************************/

SAPDB_Bool
FileDir_PageIndexPage::Iterator::operator==(const Iterator& i) const
{
    return
        (m_indexPage == i.m_indexPage)
        &&
        (m_current == i.m_current);
}

/*************************************************************************/

FileDir_PageIndexPage::Iterator&
FileDir_PageIndexPage::Iterator::operator=(const Iterator& i)
{
    if (*this == i)
        return *this;

    m_indexPage = i.m_indexPage;
    m_current   = i.m_current;

    return *this;
}

/*************************************************************************/

FileDir_PageIndexPage::Iterator::Iterator(FileDir_PageIndexPage& indexPage)
    : m_indexPage(&indexPage),
      m_current(indexPage.FirstEntryPos())
{
    if ( m_current == indexPage.m_currentEntry ) {
        m_current = 0;
    }
}

/*************************************************************************/

FileDir_PageIndexPage::Iterator::Iterator(
    FileDir_PageIndexPage& indexPage,
    bool)
    : m_indexPage(&indexPage),
      m_current(0)
{}

/**************************************************************************/

FileDir_ReturnCode FileDir_PageIndexPage::CheckData(RTE_TaskId taskId)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_PageIndexPage::CheckData",
                             FileDir_Trace, 5 );
    // FIXME: verify page

    return FileDir_Okay;
}

/*************************************************************************/
