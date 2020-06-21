/*****************************************************************************/
/*!
 @file    FileDir_Page.cpp
 @author  MartinKi
 @ingroup FileDirectory_Files

 @brief Implementation of class FileDir_Page.

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

#include "FileDirectory/FileDir_Page.hpp"
#include "FileDirectory/FileDir_RWRegionWrapper.hpp"
#include "FileDirectory/FileDir_Exception.hpp"
#include "FileDirectory/FileDir_Messages.hpp"
#include "FileDirectory/FileDir_Common.hpp"
#include "DataAccess/Data_FileTypes.hpp"

// for Data_PageNo:
#include "DataAccess/Data_Types.hpp"

#include "FrameControl/FrameCtrl_IFileDirectory.hpp"

// to write page:
#include "IOManager/IOMan_IDataIO.hpp"

// to init Data_BasePage:
#include "hbd01_1.h"
#include "hbd13.h"

// PASCAL: GG_basic_constants_and_type
#include "ggg00.h"

// vgetpid():
#include "heo51.h"

#include "KernelCommon/Kernel_VTrace.hpp"

#include "RunTime/System/RTESys_AtomicOperation.hpp"
#include "RunTime/System/RTESys_MemoryBarrier.h"
#include "RunTime/RTE_Crash.hpp"
#include "RunTime/RTE_Types.hpp"

#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp"
#include "SAPDBCommon/Messages/SDBMsg_FileDir.h"
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


FileDir_Page::FileDir_Page(
    SAPDBMem_IRawAllocator& alloc,
    const RTE_TaskId taskId,
    const Data_PageNo& pageNumber,
    const SAPDB_Bool needsWrite)
    : m_allocator(alloc),
      m_initialized(false),
      m_anchor(0),
      m_recyclableEntryCount(0),
      m_freeBlockList(alloc),
      m_pageSize(0),
      m_largestFreeBlock(0),
      m_isDirty(needsWrite),
      m_isInFreePageList( false )
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Page::FileDir_Page",
                             FileDir_Trace, 5 );

    m_Frame = FrameCtrl_IFileDirectory::GetInstance().NewFDirFrame( taskId );

    if ( !m_Frame.IsAssigned() ) {
        return;
    }

    m_anchor = reinterpret_cast<char*>(
        m_Frame.GetPointer(0, m_Frame.GetLength()));
    if ( m_anchor != 0 ) {
        m_pageSize = m_Frame.GetLength();

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
        np.np_ptr()->nd_level() = FileDir_Common::FileDir_Page;
        // end init header

        this->SetEntryCount( 0 );

        m_freeSpace        = PageDataSize;
        m_largestFreeBlock = m_freeSpace;

        // this temporary free block writes directly to the page and
        // is enough to allow InitFreeBlockList() to work properly
        // during initialize:
        FreeBlock fb =
            FreeBlock( &m_anchor[HeaderSize], m_largestFreeBlock );
    }
}

/**************************************************************************/

FileDir_Page::~FileDir_Page()
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Page::~FileDir_Page",
                             FileDir_Trace, 5 );
    if ( !m_freeBlockList.IsEmpty() ) {
        FreeBlockList::Iterator it = m_freeBlockList.Begin();
        while ( it != m_freeBlockList.End() ) {
            FreeBlock* fp = *it++;
            destroy(fp, m_allocator);
        }
    }
    if ( m_Frame.IsAssigned() ) {
        RTE_TaskId taskId;
        vgetpid( taskId );
        FrameCtrl_IFileDirectory::GetInstance().FreeFDirFrame(
            taskId, m_Frame );
    }
}

/**************************************************************************/

bool FileDir_Page::Initialize()
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Page::Initialize",
                             FileDir_Trace, 5 );
    SAPDBERR_ASSERT_ARGUMENT( !m_initialized );

    if ( !m_Frame.IsAssigned() || (m_anchor == 0) ||
         !m_pageIOLock.Initialize() || !m_freeListLock.Initialize() ) {
        // allocation failed
        return false;
    }

    m_initialized = m_freeBlockList.Initialize( 50 );
    if ( m_initialized ) {
        m_freeBlockList.Clear();
        InitFreeBlockList();
    }
    return m_initialized;
}

/**************************************************************************/

char* FileDir_Page::ReserveSpace(
    const RTE_TaskId taskId,
    const int recordLength)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Page::ReserveSpace",
                             FileDir_Trace, 5 );
    SAPDBERR_ASSERT_ARGUMENT(recordLength > 0);
    SAPDBERR_ASSERT_ARGUMENT(m_initialized);

    m_freeListLock.AcquireWriteLock( taskId );

    // find first block in which entry will fit:
    FreeBlockList::Iterator it;
    for (it = m_freeBlockList.Begin();it != m_freeBlockList.End(); ++it) {
        if ( (*it)->GetLength() >= recordLength ) {
            break;
        }
    }

    if ( it == m_freeBlockList.End() ) {
        FileDir_Exception errMsg(
            __CONTEXT__, FILEDIR_INTERNAL_ERROR );
        RTE_Crash(errMsg);
    }

    char* recordAddress = (*it)->GetPos();

    if ( FileDir_Trace.TracesLevel( 6 ) ) {
        Kernel_VTrace()
            << "FileDir_Page::ReserveSpace: pNo = "
            << this->PageNo() << NewLine
            << " requested  : " << recordLength << NewLine
            << " block size : " << (*it)->GetLength() << NewLine
            << " offset     : "
            << static_cast<SAPDB_Int>( (*it)->GetPos() - m_anchor );
    }

    SAPDBTRACE_WRITELN(
        FileDir_Trace, 5,
        "pNo        : " << this->PageNo() << NewLine <<
        "requested  : " << recordLength << NewLine <<
        "block size : " << (*it)->GetLength() );

    // shrink free block or remove it
    if ( (*it)->GetLength() == recordLength ) {
        FreeBlock* fb = *it;
        m_freeBlockList.Delete( it );
        destroy( fb, m_allocator );
    } else {
        (*it)->MoveBy( recordLength );
        (*it)->ShrinkBy( recordLength );
    }

    RecordHeader* eh = reinterpret_cast<RecordHeader*>(recordAddress);
    eh->length     = recordLength;
    eh->isValid    = false;
    eh->lazyDelete = false;

    SAPDBERR_ASSERT_STATE(recordAddress > m_anchor);
    SAPDBERR_ASSERT_STATE(recordAddress < &m_anchor[PageSize]);

    m_freeSpace -= recordLength;

    UpdateLargestFreeBlock();

    m_freeListLock.ReleaseWriteLock( taskId );

    SAPDBERR_ASSERT_STATE(GetLargestFreeBlock() >= 0);

    // at this point a savepoint could flush the
    // page, but this is not critical as the new entry is still invalid
    return recordAddress;
}

/**************************************************************************/

SAPDB_UInt2 FileDir_Page::GetLargestFreeBlock() const
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Page::GetLargestFreeBlock",
                             FileDir_Trace, 7 );
    return m_largestFreeBlock;
}

/**************************************************************************/

void FileDir_Page::SetEntryCount(SAPDB_Int4 entryCount)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Page::SetEntryCount",
                             FileDir_Trace, 5 );

    this->NodePtr()->nd_record_cnt() = entryCount;
}
/**************************************************************************/

SAPDB_Int4 FileDir_Page::GetEntryCount() const
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Page::GetEntryCount",
                             FileDir_Trace, 5 );
    return this->NodePtr()->nd_record_cnt();
}

/**************************************************************************/

void FileDir_Page::ModifyEntryCount(SAPDB_Int4 delta)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Page::ModifyEntryCount",
                             FileDir_Trace, 5 );

    RTESys_AtomicModify( this->NodePtr()->nd_record_cnt(), delta );
}

/**************************************************************************/

void FileDir_Page::LockPageForIO(const RTE_TaskId taskId)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Page::LockPageForIO",
                             FileDir_Trace, 6 );
    m_pageIOLock.AcquireWriteLock( taskId );
    m_isDirty = true;
}

/**************************************************************************/

void FileDir_Page::UnlockPageForIO(const RTE_TaskId taskId)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Page::UnlockPageForIO",
                             FileDir_Trace, 6 );
    m_pageIOLock.ReleaseWriteLock( taskId );
}

/**************************************************************************/

void FileDir_Page::LockPageForUpdate(const RTE_TaskId taskId)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Page::LockPageForUpdate",
                             FileDir_Trace, 6 );
    m_pageIOLock.AcquireReadLock( taskId );
    m_isDirty = true;
}

/**************************************************************************/

SAPDB_Bool FileDir_Page::TryLockPageForUpdate(const RTE_TaskId taskId)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Page::TryLockPageForUpdate",
                             FileDir_Trace, 6 );
    if ( m_pageIOLock.TryAcquireReadLock( taskId ) ) {
        m_isDirty = true;
        return true;
    }
    return false;
}

/**************************************************************************/

void FileDir_Page::UnlockPageForUpdate(const RTE_TaskId taskId)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Page::UnlockPageForUpdate",
                             FileDir_Trace, 6 );
    m_pageIOLock.ReleaseReadLock( taskId );
}

/**************************************************************************/

void FileDir_Page::Flush(
    const RTE_TaskId taskId)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Page::Flush",
                             FileDir_Trace, 6 );

    this->LockPageForIO( taskId );
    m_freeListLock.AcquireWriteLock( taskId );

    IOMan_IDataIO::GetInstance().WriteDataPage( taskId, *this );

    /*
     * Remove invalid entries while page is write-locked:
     *
     * It is imperative to do this now, because once the lock is
     * released an entry might get invalidated and we might remove it
     * so it would not get flushed with the next savepoint,
     * possibly destroying the integrity of a back ref chain.
     */
    if ( !m_reinitFreeBlockList ) {
        this->UpdateFreeBlockList();
    } else {
        this->InitFreeBlockList();
    }

    m_isDirty = false;
    m_freeListLock.ReleaseWriteLock( taskId );

    this->UnlockPageForIO( taskId );
}

/**************************************************************************/

SAPDB_UInt2 FileDir_Page::GetEntryLocation(void* p)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Page::GetEntryLocation",
                             FileDir_Trace, 5 );
    SAPDBERR_ASSERT_ARGUMENT(p >= static_cast<void*>(&m_anchor[HeaderSize]));
    SAPDBERR_ASSERT_ARGUMENT(p <= static_cast<void*>(
                                 &m_anchor[PageSize-TrailerSize]));

    return static_cast<SAPDB_UInt2>(static_cast<char*>(p) - m_anchor);;
}

/**************************************************************************/

bool FileDir_Page::IsDirty()
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Page::IsDirty",
                             FileDir_Trace, 6 );
    return m_isDirty;
}

/**************************************************************************/

/*!
 * @warning m_freeBlockList should be read locked when calling this function
 */
void FileDir_Page::UpdateLargestFreeBlock()
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Page::UpdateLargestFreeBlock",
                             FileDir_Trace, 5 );
    FreeBlockList::Iterator it = m_freeBlockList.Begin();

    m_largestFreeBlock = 0;
    while (it != m_freeBlockList.End()) {
        SAPDBERR_ASSERT_STATE((*it)->GetPos() >= &m_anchor[HeaderSize]);
        SAPDBERR_ASSERT_STATE((*it)->GetPos() <= &m_anchor[PageSize - TrailerSize]);
        SAPDBERR_ASSERT_STATE((*it)->GetLength() > 0);
        SAPDBERR_ASSERT_STATE((*it)->GetLength() <= PageDataSize);

        if ((*it)->GetLength() > m_largestFreeBlock) {
            m_largestFreeBlock = (*it)->GetLength();
        }
        it++;
    }
}

/**************************************************************************/

/*!
 * @brief Goes through list of free blocks and checks sort order and
 *        that no adjacent blocks exist. Does not check whether free
 *        block list and page contents match.
 *
 * @warning List needs to be write locked when calling this function!
 */
void FileDir_Page::VerifyFreeBlockList(bool compareWithPage)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Page::VerifyFreeBlockList",
                             FileDir_Trace, 5 );

    if ( m_freeBlockList.GetSize() == 0 ) {
        return;
    }

    FreeBlockList::Iterator it = m_freeBlockList.Begin();

    FreeBlock* block = (*it);
    ++it;
    while (it != m_freeBlockList.End()) {
        // make sure sorting of list is ok
        if ( block->GetPos() >= (*it)->GetPos() ) {
            RTE_Crash(
                SAPDBErr_Exception(
                    __CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED,
                    "sort order of free block list") );
        }

        if ( block->CanBeMergedWith(*(*it)) ) {
            RTE_Crash(
                SAPDBErr_Exception(
                    __CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED,
                    "all adjacent free blocks must be merged") );
        }
        block = (*it);
        it++;
    }

    if ( compareWithPage && (!m_reinitFreeBlockList) ) {
        // compare list against list built from scratch:
        FreeBlockList saveList(m_allocator);
        it = m_freeBlockList.Begin();
        while (it != m_freeBlockList.End()) {
            saveList.InsertEnd( *it );
            ++it;
        }
        m_freeBlockList.Clear();
        this->InitFreeBlockList();


        FreeBlockList::Iterator saveIt = saveList.Begin();
        it = m_freeBlockList.Begin();
        while (it != m_freeBlockList.End()) {
            SAPDBTRACE_WRITELN(
                FileDir_Trace, 6,
                "save.pos " << ToStr((const void*)((*saveIt)->GetPos())) << NewLine <<
                "real.pos " << ToStr((const void*)((*it)->GetPos())) << NewLine <<
                "save.len " << (*saveIt)->GetLength() << NewLine <<
                "real.len " << (*it)->GetLength() );
            if ( ((*it)->GetPos()    != (*saveIt)->GetPos()) ||
                 ((*it)->GetLength() != (*saveIt)->GetLength()) ) {
                RTE_Crash(
                    SAPDBErr_Exception(
                        __CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED,
                        "free block list not ok") );
            }
            ++it;
            ++saveIt;
        }
        SAPDBERR_ASSERT_STATE( saveIt == saveList.End() );
    }
}

/**************************************************************************/

/*!
 * @brief Inserts a free block on the page into the free block
 *        list. If the block is adjacent to an already existing free
 *        block, the two blocks will be merged.
 *
 * @warning List needs to be write locked when calling this function!
 */
void FileDir_Page::InsertIntoFreeList(FreeBlock* freeBlock)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Page::InsertIntoFreeList",
                             FileDir_Trace, 5 );

    bool done = false;

    SAPDBERR_ASSERT_ARGUMENT(freeBlock->GetPos() >= &m_anchor[HeaderSize]);
    SAPDBERR_ASSERT_ARGUMENT(freeBlock->GetPos() <= &m_anchor[PageSize - TrailerSize]);

#ifdef SAPDB_SLOW
    SAPDBErr_MessageList messages;
    SAPDB_MemFill(
        freeBlock->GetPos()   + sizeof(RecordHeader), 0,
        freeBlock->GetLength()- sizeof(RecordHeader),
        "FileDirectory", __FILE__, __LINE__, messages );
#endif

    if ( FileDir_Trace.TracesLevel( 6 ) ) {
        Kernel_VTrace()
            << "FileDir_Page::InsertIntoFreeList: pNo = "
            << this->PageNo() << NewLine
            << " block size: " << freeBlock->GetLength() << NewLine
            << " offset    : "
            << static_cast<SAPDB_Int>( freeBlock->GetPos() - m_anchor );
    }

    m_freeSpace += freeBlock->GetLength();
    for (FreeBlockList::Iterator it = m_freeBlockList.Begin();
         it != m_freeBlockList.End();
         ++it) {
        SAPDBERR_ASSERT_STATE( freeBlock->GetPos() != (*it)->GetPos() );
        if ( (*it)->CanBeMergedWith(*freeBlock) ) {
            (*it)->MergeWith( *freeBlock );
            destroy( freeBlock, m_allocator );
            FreeBlockList::Iterator nextIt = it + 1;
            if ( (nextIt != m_freeBlockList.End()) &&
                 ((*it)->CanBeMergedWith( *(*nextIt) )) ) {
                FreeBlock* nextBlock = *nextIt;
                (*it)->MergeWith( *nextBlock );
                 m_freeBlockList.Delete( nextIt );
                 destroy( nextBlock, m_allocator );
            }
            done = true;
            break;
        } else if ( freeBlock->GetPos() < (*it)->GetPos() ) {
            if ( m_freeBlockList.AssertSpace( 1, it ) ) {
                m_freeBlockList.Insert( it, freeBlock );
            } else {
                // could not insert block because we ran out
                // of memory; ignore this block for now but
                // reinitialize free block list after
                // savepoint
                m_reinitFreeBlockList = true;
                destroy( freeBlock, m_allocator );
            }
            done = true;
            break;
        }
    }
    if ( !done ) {
        if ( !m_freeBlockList.InsertEnd( freeBlock ) ) {
            // could not insert block because we ran out of memory;
            // ignore this block for now but reinitialize free block
            // list after savepoint
            m_reinitFreeBlockList = true;
            destroy( freeBlock, m_allocator );
        }
    }

#ifdef SAPDB_SLOW
    this->VerifyFreeBlockList(false);
#endif
    this->UpdateLargestFreeBlock();
}

/**************************************************************************/

/**
 * adjacent blocks have to be merged already!
 *
 * @warning Should only be called during restart. No locking is used!
 */
void FileDir_Page::Restart_Defragment()
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Page::Restart_Defragment",
                             FileDir_Trace, 5 );

    char* freePos;
    int   freeSize = 0;

    if ( m_freeBlockList.GetSize() <= 1  ) {
        return;
    }

    if ( FileDir_Trace.TracesLevel( 6 ) ) {
        Kernel_VTrace()
            << "FileDir_Page::Restart_Defragment: pNo"
            << this->PageNo() << NewLine
            << "  free block list size: " << m_freeBlockList.GetSize();
    }

    m_isDirty = true;

    FreeBlockList::Iterator it = m_freeBlockList.Begin();
    FreeBlockList::Iterator currBlock;
    freePos = (*it)->GetPos();
    SAPDBErr_MessageList messages;
    while ( it != m_freeBlockList.End() ) {
        char* startPos = (*it)->GetPos() + (*it)->GetLength();
        SAPDBERR_ASSERT_STATE(startPos > freePos);
        currBlock = it++;
        freeSize += (*currBlock)->GetLength();
        if ( it != m_freeBlockList.End() ) {
            int moveLen = reinterpret_cast<char*>((*it)->GetPos()) - startPos;
            SAPDBERR_ASSERT_STATE((startPos + moveLen) <
                   (&m_anchor[PageSize-TrailerSize]));

            SAPDB_MemMove( freePos, startPos, moveLen,
                           "FileDirectory", __FILE__, __LINE__, messages );
#ifdef SAPDB_SLOW
            if ((startPos - freePos) >= moveLen) {
                SAPDB_MemFill(
                    startPos, 0, moveLen,
                    "FileDirectory", __FILE__, __LINE__, messages );
            } else {
                SAPDB_MemFill(freePos + moveLen, 0,
                       startPos - freePos,
                       "FileDirectory", __FILE__, __LINE__, messages );
            }
#endif
            freePos  += moveLen;
            // release this block, but keep it in list (to not disturb
            // iterator)
            FreeBlock* fb = *currBlock;
            destroy( fb, m_allocator );
        }
    }

    // currBlock is last and only block in list
    SAPDBERR_ASSERT_STATE(freeSize == m_freeSpace);
    FreeBlock* block = *currBlock;
    block->SetPos(freePos);
    block->SetLength(freeSize);
    m_freeBlockList.Clear();
    if ( !m_freeBlockList.InsertEnd(block) ) {
        // could not insert block because we ran out of memory;
        // ignore this block for now but reinitialize free block
        // list after savepoint
        m_reinitFreeBlockList = true;
        destroy( block, m_allocator );
    }
#ifdef SAPDB_SLOW
    SAPDB_MemFill(
        freePos + sizeof(RecordHeader), 0,
        freeSize - sizeof(RecordHeader),
        "FileDirectory", __FILE__, __LINE__, messages );
    this->VerifyFreeBlockList( false );
#endif
    this->UpdateLargestFreeBlock();
}

/**************************************************************************/

FileDir_Page::Iterator::Iterator(FileDir_Page& page)
    : m_page(&page)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Page::Iterator::Iterator",
                             FileDir_Trace, 5 );
    SAPDBTRACE_WRITELN(
        FileDir_Trace, 5,
        "page no: " << m_page->PageNo() << " @ " << ToStr(m_page) );
    m_currentEntry = &m_page->m_anchor[HeaderSize];
    if ( m_page->IsValid( m_currentEntry ) &&
         m_page->IsLazyDeleteSet( m_currentEntry ) ) {
        m_page->SetInvalid( m_currentEntry );
    }

    if ( !reinterpret_cast<RecordHeader*>(m_currentEntry)->isValid ) {
        ++*this;
    }

    SAPDBERR_ASSERT_ARGUMENT(m_currentEntry > m_page->m_anchor);
    SAPDBERR_ASSERT_ARGUMENT(m_currentEntry < &m_page->m_anchor[PageSize]);
}

/**************************************************************************/

FileDir_Page::Iterator::Iterator(FileDir_Page& page, bool)
    : m_page(&page)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Page::Iterator::Iterator-End",
                             FileDir_Trace, 5 );
    m_currentEntry = &m_page->m_anchor[PageSize - TrailerSize];
    SAPDBTRACE_WRITELN(
        FileDir_Trace, 8,
        "page no: " << m_page->PageNo() << NewLine <<
        "offset : " << SAPDB_Int(m_currentEntry - m_page->m_anchor) );
}

/**************************************************************************/

FileDir_Page::Iterator::~Iterator()
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Page::Iterator::~Iterator",
                             FileDir_Trace, 5 );
}

/**************************************************************************/

FileDir_Page::Iterator& FileDir_Page::Iterator::operator=(const Iterator& it)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Page::Iterator::operator=",
                             FileDir_Trace, 8 );
    if (*this == it)
        return *this;

    m_page         = it.m_page;
    m_currentEntry = it.m_currentEntry;

    SAPDBERR_ASSERT_ARGUMENT(m_currentEntry > m_page->m_anchor);
    SAPDBERR_ASSERT_ARGUMENT(m_currentEntry < &m_page->m_anchor[PageSize]);
    SAPDBTRACE_WRITELN(
        FileDir_Trace, 8,
        "page no: " << m_page->PageNo() << " @ " << ToStr(m_page) );

    return *this;
}

/**************************************************************************/

FileDir_Page::Iterator& FileDir_Page::Iterator::operator++()
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Page::Iterator::operator++",
                             FileDir_Trace, 8 );

    SAPDBTRACE_WRITELN(
        FileDir_Trace, 8,
        "page no: " << m_page->PageNo() << NewLine <<
        "offset : " << SAPDB_Int(m_currentEntry - m_page->m_anchor) );

    if ( m_currentEntry >= &m_page->m_anchor[PageSize - TrailerSize] ) {
        return *this;
    }

    do {
        m_currentEntry +=
            reinterpret_cast<RecordHeader*>(m_currentEntry)->length;

        SAPDBTRACE_WRITELN(
            FileDir_Trace, 8,
            "offset : " << SAPDB_Int(m_currentEntry - m_page->m_anchor) );
    } while ( !reinterpret_cast<RecordHeader*>(m_currentEntry)->isValid &&
              m_currentEntry < &m_page->m_anchor[PageSize - TrailerSize]);

    return *this;
}

/**************************************************************************/

char* FileDir_Page::Iterator::operator*()
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Page::Iterator::operator*",
                             FileDir_Trace, 8 );

    SAPDBTRACE_WRITELN(
        FileDir_Trace, 8,
        "page no: " << m_page->PageNo() << " @ " << ToStr(m_page) << NewLine <<
        "offset :" << SAPDB_Int(m_currentEntry - m_page->m_anchor) );

    char* entryPtr = m_currentEntry;

    SAPDBERR_ASSERT_ARGUMENT(entryPtr > m_page->m_anchor);
    SAPDBERR_ASSERT_ARGUMENT(entryPtr < &m_page->m_anchor[PageSize]);

    return entryPtr;
}

/**************************************************************************/

bool FileDir_Page::Iterator::operator==(const Iterator& it) const
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Page::Iterator::operator==",
                             FileDir_Trace, 8 );

    return
        (m_page == it.m_page)
        &&
        (m_currentEntry == it.m_currentEntry);
}

/**************************************************************************/

bool FileDir_Page::Iterator::operator!=(const Iterator& it) const
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Page::Iterator::operator!=",
                             FileDir_Trace, 8 );

    return
        (m_page != it.m_page)
        ||
        (m_currentEntry != it.m_currentEntry);

}

/**************************************************************************/

char* FileDir_Page::GetEntryAddress(SAPDB_UInt2 entryLocation)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Page::GetEntryAddress",
                             FileDir_Trace, 5 );

    SAPDBERR_ASSERT_ARGUMENT(entryLocation >= HeaderSize);
    SAPDBERR_ASSERT_ARGUMENT(entryLocation <= (PageSize-TrailerSize));

    return &m_anchor[entryLocation];
}

/**************************************************************************/

/*!
 * @brief Determine free blocks on a page read from disk during restart.
 *
 * Adjacent free blocks will be merged.
 *
 * @warning Does not lock m_freeBlockList!
 */
void FileDir_Page::InitFreeBlockList()
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Page::InitFreeBlockList",
                             FileDir_Trace, 5 );

    m_reinitFreeBlockList = false;

    FreeBlockList::Iterator it = m_freeBlockList.Begin();
    while ( it != m_freeBlockList.End() ) {
        FreeBlock* fb = *it++;
        destroy( fb, m_allocator );
    }

    m_freeBlockList.Clear();
    m_freeSpace = PageDataSize;

    SAPDB_UInt4 entryCount = 0;
    char* posOnPage = &m_anchor[HeaderSize];
    while ( posOnPage < &m_anchor[PageSize - TrailerSize] ) {
        RecordHeader* eh = reinterpret_cast<RecordHeader*>(posOnPage);

        SAPDBERR_ASSERT_STATE(eh->length > 0);

        if ( !eh->isValid ) {
            FreeBlock* fb = new(m_allocator)
                FreeBlock(posOnPage, eh->length);
            if ( fb == 0 ) {
                m_reinitFreeBlockList = true;
            } else {
                if ( (m_freeBlockList.GetSize() > 0)
                     &&
                     fb->CanBeMergedWith(
                         *m_freeBlockList[m_freeBlockList.GetSize()-1]) ) {
                    m_freeBlockList[m_freeBlockList.GetSize()-1]->MergeWith(*fb);
                destroy(fb, m_allocator);
                } else {
                    if ( !m_freeBlockList.InsertEnd(fb) ) {
                        // could not insert block because we ran out
                        // of memory; ignore this block for now but
                        // reinitialize free block list after
                        // savepoint
                        m_reinitFreeBlockList = true;
                        destroy( fb, m_allocator );
                    }
                }
            }
        } else {
            m_freeSpace -= eh->length;
            ++entryCount;
        }
        posOnPage += eh->length;
    }

    SAPDBERR_ASSERT_STATE( this->GetEntryCount() == entryCount );
    // silently correct any differences for fast kernel
    this->SetEntryCount( entryCount );

#ifdef SAPDB_SLOW
    this->VerifyFreeBlockList(false);
#endif
    this->UpdateLargestFreeBlock();
}

/**************************************************************************/

void FileDir_Page::GetStatistics(
    SAPDB_Int& freeSpace,
    SAPDB_Int& freeBlockCount,
    SAPDB_Int& largestFreeBlock)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Page::GetStatistics",
                             FileDir_Trace, 5 );

    freeSpace        = m_freeSpace;
    freeBlockCount   = m_freeBlockList.GetSize();
    largestFreeBlock = m_largestFreeBlock;
}

/**************************************************************************/

void FileDir_Page::FreeEntry(
    const RTE_TaskId taskId,
    char* p)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Page::FreeEntry",
                             FileDir_Trace, 5 );

    SAPDBERR_ASSERT_ARGUMENT( p > m_anchor );
    SAPDBERR_ASSERT_ARGUMENT( p < &m_anchor[PageSize] );
    SAPDBERR_ASSERT_ARGUMENT( !reinterpret_cast<RecordHeader*>(p)->isValid );
    SAPDBERR_ASSERT_ARGUMENT( m_isDirty );

    FileDir_RWAutoRegion writeLock(m_freeListLock, true, taskId );

    FreeBlock* fb = new(m_allocator) FreeBlock(p);
    if ( fb == 0 ) {
        // could not create free block object because we ran out
        // of memory; ignore this block for now but
        // reinitialize free block list after
        // savepoint
        m_reinitFreeBlockList = true;
    } else {
        InsertIntoFreeList( fb );
    }
}

/**************************************************************************/

SAPDB_Int FileDir_Page::GetFragmentationMeasure()
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Page::GetFragmentationMeasure",
                             FileDir_Trace, 5 );

    return
        (m_largestFreeBlock > 0)
        ?
        m_freeSpace * m_freeBlockList.GetSize() / m_largestFreeBlock
        :
        1;
}

/**************************************************************************/
/**************************************************************************/

FileDir_Page::FreeBlock::FreeBlock(char* p, SAPDB_UInt2 length)
            : pos(reinterpret_cast<RecordHeader*>(p))
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Page::FreeBlock::FreeBlock",
                             FileDir_Trace, 7 );

    pos->length  = length;
    pos->isValid = false;
}

/**************************************************************************/

/*!
 * @brief Construct new FreeBlock based on an already existing block
 *        at address p. The block at p has to be marked as invalid
 *        already!
 */
FileDir_Page::FreeBlock::FreeBlock(char* p)
    : pos(reinterpret_cast<RecordHeader*>(p))
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Page::FreeBlock::FreeBlock",
                             FileDir_Trace, 7 );

    SAPDBERR_ASSERT_ARGUMENT( !pos->isValid );
}

/**************************************************************************/

/*!
 * @return address of block in memory
 */
char* FileDir_Page::FreeBlock::GetPos() {
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Page::FreeBlock::GetPos",
                             FileDir_Trace, 7 );

    return reinterpret_cast<char*>(pos);
}

/**************************************************************************/

/*!
 * @return length of block in bytes
 */
SAPDB_UInt2 FileDir_Page::FreeBlock::GetLength() const {
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Page::FreeBlock::GetLength",
                             FileDir_Trace, 7 );

    return pos->length;
}

/**************************************************************************/

/*!
 * @brief  sets position (address) of block to newPos
 *
 * @param  newPos new address of block in memory
 * @return none
 */
void FileDir_Page::FreeBlock::SetPos(const char* newPos) {
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Page::FreeBlock::SetPos",
                             FileDir_Trace, 7 );

    SAPDBERR_ASSERT_STATE(pos->length > 0);

    reinterpret_cast<RecordHeader*>(
        const_cast<char*>(newPos))->length =
        pos->length;
    reinterpret_cast<RecordHeader*>(
        const_cast<char*>(newPos))->isValid     = false;
    pos = reinterpret_cast<RecordHeader*>(const_cast<char*>(newPos));
}

/**************************************************************************/

/*!
 * @brief  sets length of block to newLength
 *
 * @param  newLength new length of block
 * @return none
 */
void FileDir_Page::FreeBlock::SetLength(const SAPDB_UInt2 newLength) {
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Page::FreeBlock::SetLength",
                             FileDir_Trace, 7 );

    SAPDBERR_ASSERT_ARGUMENT(newLength > 0);
    pos->length = newLength;
}

/**************************************************************************/

bool FileDir_Page::FreeBlock::CanBeMergedWith(
    const FreeBlock& candidate) const {

    SAPDBTRACE_METHOD_DEBUG( "FileDir_Page::FreeBlock::CanBeMergedWith",
                             FileDir_Trace, 7 );

    char* myPos = reinterpret_cast<char*>(pos);
    char* candidatePos = reinterpret_cast<char*>(candidate.pos);
    if (myPos < candidatePos) {
        return (myPos + pos->length) == candidatePos;
    } else {
        return myPos == (candidatePos + candidate.pos->length);
    }
}

/**************************************************************************/

void FileDir_Page::FreeBlock::MergeWith(const FreeBlock& candidate) {

    SAPDBTRACE_METHOD_DEBUG( "FileDir_Page::FreeBlock::MergeWith",
                             FileDir_Trace, 7 );

    SAPDBERR_ASSERT_STATE(CanBeMergedWith(candidate));
    SAPDB_UInt2 length = pos->length;
    if (pos > candidate.pos) {
        pos = candidate.pos;
    }
    pos->length = length + candidate.pos->length;
}

/**************************************************************************/

void FileDir_Page::FreeBlock::ShrinkBy(const int shrinkBy) {
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Page::FreeBlock::ShrinkBy",
                             FileDir_Trace, 7 );

    SAPDBERR_ASSERT_ARGUMENT(pos->length > shrinkBy);
    pos->length -= shrinkBy;
}

/**************************************************************************/

void FileDir_Page::FreeBlock::MoveBy(const int moveBy) {
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Page::FreeBlock::MoveBy",
                             FileDir_Trace, 7 );

    SetPos(reinterpret_cast<char*>(pos) + moveBy);
}

/**************************************************************************/

FileDir_Page::FreeBlock* FileDir_Page::FreeBlock::Split(char* splitAddress) {
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Page::FreeBlock::Split",
                             FileDir_Trace, 7 );

    SAPDBERR_ASSERT_ARGUMENT(splitAddress > reinterpret_cast<char*>(pos));
    SAPDBERR_ASSERT_ARGUMENT(splitAddress < (reinterpret_cast<char*>(pos)
                                             + pos->length));
    int newBlockLength = pos->length -
        (splitAddress - reinterpret_cast<char*>(pos));
    pos->length -= newBlockLength;
    return new FreeBlock(splitAddress, newBlockLength);
}

/**************************************************************************/

SAPDB_UInt2 FileDir_Page::GetAllFreeSpace(const RTE_TaskId taskId)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Page::GetAllFreeSpace",
                             FileDir_Trace, 5 );

    SAPDB_UInt2 fs = m_freeSpace;  // should be atomic
    return fs;
}

/**************************************************************************/

void FileDir_Page::Restart_ReinitializeFromContent()
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Page::Restart_ReinitializeFromContent",
                             FileDir_Trace, 5 );

    InitFreeBlockList();
}

/**************************************************************************/

void FileDir_Page::Restart_ShrinkEntry(
    char* p,
    SAPDB_UInt2 shrinkBy)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Page::Restart_ShrinkEntry",
                             FileDir_Trace, 5 );

    SAPDBERR_ASSERT_ARGUMENT( p > m_anchor );
    SAPDBERR_ASSERT_ARGUMENT( p < &m_anchor[PageSize] );
    SAPDBERR_ASSERT_ARGUMENT( shrinkBy
                              <
                              reinterpret_cast<RecordHeader*>(p)->length );

    m_isDirty = true;

    reinterpret_cast<RecordHeader*>(p)->length -= shrinkBy;
    // create temporary free block to 'stamp' free space
    FreeBlock fb =
        FreeBlock(
            p + reinterpret_cast<RecordHeader*>(p)->length,
            shrinkBy );
}

/**************************************************************************/

void FileDir_Page::SetValid(char* p)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Page::SetValid",
                             FileDir_Trace, 5 );

    SAPDBERR_ASSERT_ARGUMENT( p > m_anchor );
    SAPDBERR_ASSERT_ARGUMENT( p < &m_anchor[PageSize] );

    if ( reinterpret_cast<RecordHeader*>(p)->isValid != true ) {
        // during restart no locking is used and therefore page might
        // not be marked as dirty yet:
        SetDirty();

        reinterpret_cast<RecordHeader*>(p)->isValid = true;
        this->ModifyEntryCount( 1 );
    }
 }

/**************************************************************************/

void FileDir_Page::SetInvalid(char* p)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Page::SetInvalid",
                             FileDir_Trace, 5 );

    SAPDBERR_ASSERT_ARGUMENT( p > m_anchor );
    SAPDBERR_ASSERT_ARGUMENT( p < &m_anchor[PageSize] );
    SAPDBERR_ASSERT_STATE( this->GetEntryCount() > 0 );

    if ( reinterpret_cast<RecordHeader*>(p)->isValid != false ) {
        // during restart no locking is used and therefore page might
        // not be marked as dirty yet:
        SetDirty();

        reinterpret_cast<RecordHeader*>(p)->isValid = false;
        this->ModifyEntryCount( -1 );
    }
}

/**************************************************************************/

void FileDir_Page::SetLazyDelete(char* p)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Page::SetLazyDelete",
                             FileDir_Trace, 5 );

    SAPDBERR_ASSERT_ARGUMENT( p > m_anchor );
    SAPDBERR_ASSERT_ARGUMENT( p < &m_anchor[PageSize] );
    SAPDBERR_ASSERT_STATE( this->IsValid( p ) );
    SAPDBERR_ASSERT_STATE( this->GetEntryCount() > 0 );

    if ( ! this->IsLazyDeleteSet( p ) ) {
        // during restart no locking is used and therefore page might
        // not be marked as dirty yet:
        SetDirty();

        reinterpret_cast<RecordHeader*>(p)->lazyDelete = true;
        RTESys_WriteMemoryBarrier();
    }
}

/**************************************************************************/

SAPDB_Bool FileDir_Page::IsLazyDeleteSet(char* p)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Page::IsLazyDeleteSet",
                             FileDir_Trace, 5 );

    SAPDBERR_ASSERT_ARGUMENT( p > m_anchor );
    SAPDBERR_ASSERT_ARGUMENT( p < &m_anchor[PageSize] );

    RTESys_ReadMemoryBarrier();
    return reinterpret_cast<RecordHeader*>(p)->lazyDelete == true;
}

/**************************************************************************/

/*!
 * @brief Enter all free blocks on a page into free list if they are
 *        not already in there. New free blocks adjacent to existing
 *        blocks will be merged.
 *
 * @warning Does not lock m_freeBlockList!
 */
void FileDir_Page::UpdateFreeBlockList()
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Page::UpdateFreeBlockList",
                             FileDir_Trace, 5 );

    FreeBlockList::Iterator it = m_freeBlockList.Begin();

    char* posOnPage = &m_anchor[HeaderSize];
    while (posOnPage < &m_anchor[PageSize - TrailerSize]) {
        RecordHeader* eh = reinterpret_cast<RecordHeader*>(posOnPage);

        SAPDBTRACE_WRITELN(
            FileDir_Trace, 6,
            "pNo      : " << this->PageNo() << NewLine <<
            "m_anchor : " << ToStr((const void*)(m_anchor)) << NewLine <<
            "posOnPage: " << ToStr((const void*)(posOnPage)) << NewLine <<
            "length   : " << eh->length << NewLine <<
            "isValid  : " << eh->isValid );

        SAPDBERR_ASSERT_STATE(eh->length > 0);

        if ( !eh->isValid ) {
            while ( (it != m_freeBlockList.End()) &&
                    ((*it)->GetPos() < posOnPage) ) {
                ++it;
            }

            // check if free block must be inserted
            if ( (it == m_freeBlockList.End()) ||
                 (posOnPage != (*it)->GetPos()) ) {

                m_freeSpace += eh->length;

                // insert block
                FreeBlock* fb = new(m_allocator)
                    FreeBlock( posOnPage, eh->length );
                if ( fb == 0 ) {
                    m_reinitFreeBlockList = true;
                } else {

                    SAPDBERR_ASSERT_STATE(fb->GetPos() >= &m_anchor[HeaderSize]);
                    SAPDBERR_ASSERT_STATE(fb->GetPos() <= &m_anchor[PageSize - TrailerSize]);
                    SAPDBERR_ASSERT_STATE(fb->GetLength() > 0);
                    SAPDBERR_ASSERT_STATE(fb->GetLength() <= PageDataSize);
                    if ( (fb->GetLength() == 0)
                         || (fb->GetLength() > PageDataSize) ) {
                        Msg_List errMsg = Msg_List(
                            Msg_List::Error,
                            SDBMSG_FILEDIR_FPAGE_CORRUPTED,
                            Msg_Arg( SDBMSGTAG_FILEDIR_FPAGE_CORRUPTED__PAGENO,
                                    this->PageNo() ),
                            Msg_Arg(SDBMSGTAG_FILEDIR_FPAGE_CORRUPTED__POSITION,
                                    fb->GetPos() ),
                            Msg_Arg(SDBMSGTAG_FILEDIR_FPAGE_CORRUPTED__LENGTH,
                                    fb->GetLength() )
                            );

                        RTE_Crash( errMsg );
                    }

                    if ( it == m_freeBlockList.End() ) {
                        SAPDBTRACE_WRITELN(
                            FileDir_Trace, 6,
                            "at end of iterator list" );

                        if ( (it != m_freeBlockList.Begin()) &&
                             ((*(it-1))->CanBeMergedWith(*fb)) ) {
                            SAPDBTRACE_WRITELN(
                                FileDir_Trace, 6, "Merge End" );
                            (*(it-1))->MergeWith( *fb );
                            destroy( fb, m_allocator );
                            SAPDBERR_ASSERT_STATE(
                                (*(it-1))->GetLength() <= PageDataSize);
                        } else {
                            if ( !m_freeBlockList.InsertEnd( fb ) ) {
                                // could not insert block because we ran out
                                // of memory; ignore this block for now but
                                // reinitialize free block list after
                                // savepoint
                                m_reinitFreeBlockList = true;
                                destroy( fb, m_allocator );
                            }
                        }
                    } else {
                        SAPDBTRACE_WRITELN(
                            FileDir_Trace, 6,
                            "iterator pos: "
                            << ToStr((const void*)((*it)->GetPos())) << NewLine <<
                            "iterator len: " << (*it)->GetLength() );

                        bool consumed = false;
                        if ( fb->CanBeMergedWith( *(*it) ) ) {
                            SAPDBTRACE_WRITELN(
                                FileDir_Trace, 6, "Merge 1" );
                            (*it)->MergeWith( *fb );
                            destroy( fb, m_allocator );
                            consumed = true;
                            fb = *it;
                        }
                        if ( (it != m_freeBlockList.Begin()) &&
                             ((*(it-1))->CanBeMergedWith(*fb)) ) {
                            SAPDBTRACE_WRITELN(
                                FileDir_Trace, 6, "Merge 2" );
                            (*(it-1))->MergeWith( *fb );
                            if ( ! consumed ) {
                                destroy( fb, m_allocator );
                                consumed = true;
                            } else {
                                FreeBlock* fb1 = *it;
                                m_freeBlockList.Delete( it );
                                destroy ( fb1, m_allocator );
                            }
                            --it;
                        }
                        if ( ! consumed ) {
                            if ( m_freeBlockList.AssertSpace( 1, it ) ) {
                                m_freeBlockList.Insert( it, fb );
                            } else {
                                // could not insert block because we ran out
                                // of memory; ignore this block for now but
                                // reinitialize free block list after
                                // savepoint
                                m_reinitFreeBlockList = true;
                                destroy( fb, m_allocator );
                            }
                        } else {
                        SAPDBTRACE_WRITELN(
                            FileDir_Trace, 6,
                            "now iterator: " << NewLine
                            << "iterator pos: "
                            << ToStr((const void*)((*it)->GetPos())) << NewLine <<
                            "iterator len: " << (*it)->GetLength() );
                        }

                        // merging might have changed our position on the page
                        if ( it != m_freeBlockList.End() ) {
                            posOnPage = (*it)->GetPos();
                        } else {
                            posOnPage = (*--it)->GetPos();
                        }
                        eh = reinterpret_cast<RecordHeader*>(posOnPage);
                    }
                }
            }
        }
        posOnPage += eh->length;
    }

#ifdef SAPDB_SLOW
    this->VerifyFreeBlockList(true);
    this->VerifyEntryCount();
#endif
    this->UpdateLargestFreeBlock();
}

/**************************************************************************/

void FileDir_Page::VerifyEntryCount()
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Page::VerifyEntryCount",
                             FileDir_Trace, 5 );

    char* posOnPage = &m_anchor[HeaderSize];
    SAPDB_UInt2 entryCount = 0;
    while (posOnPage < &m_anchor[PageSize - TrailerSize]) {
        RecordHeader* eh = reinterpret_cast<RecordHeader*>(posOnPage);
        if ( eh->isValid ) {
            entryCount++;
        }
        posOnPage += eh->length;
    }

    if ( this->GetEntryCount() != entryCount ) {
        RTE_Crash(
            SAPDBErr_Exception(
                __CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED,
                "page header entryCount equals actual entry count") );
    }
}

/**************************************************************************/

FileDir_ReturnCode FileDir_Page::CheckData(RTE_TaskId taskId)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Page::CheckData",
                             FileDir_Trace, 5 );

    // FIXME: call VerifyEntryCount but rewrite it to prevent if from crashing in case of an error

    return FileDir_Okay;
}

/**************************************************************************/
