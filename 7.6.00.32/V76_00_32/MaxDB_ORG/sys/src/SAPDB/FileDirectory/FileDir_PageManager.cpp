/*****************************************************************************/
/*!
 @file    FileDir_PageManager.cpp
 @author  MartinKi
 @ingroup FileDirectory_Files

 @brief Implementation of class FileDir_PageManager.

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

#include "FileDirectory/FileDir_PageManager.hpp"
#include "FileDirectory/FileDir_Page.hpp"
#include "FileDirectory/FileDir_PageIndexPage.hpp"
#include "FileDirectory/FileDir_Exception.hpp"
#include "FileDirectory/FileDir_Messages.hpp"
#include "FileDirectory/FileDir_RWRegionWrapper.hpp"
#include "FileDirectory/FileDir_Common.hpp"

// for Data_PageNo:
#include "DataAccess/Data_Types.hpp"

// to load pages
#include "IOManager/IOMan_IDataIO.hpp"

// page number handling
#include "Converter/Converter_IPageNoManager.hpp"
#include "Converter/Converter_IVerify.hpp"
#include "Converter/Converter_ICommon.hpp"

#include "RunTime/System/RTESys_AtomicOperation.hpp"
#include "RunTime/RTE_Crash.hpp"
#include "RunTime/RTE_Types.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"

#include "KernelCommon/Kernel_Dump.hpp"
#include "KernelCommon/Kernel_FileIO.hpp"

#include "SAPDBCommon/Messages/SDBMsg_FileDir.h"
#include "Messages/Msg_List.hpp"

#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"

/**************************************************************************/

FileDir_PageManager* FileDir_PageManager::m_instance = 0;

/**************************************************************************/

FileDir_PageManager::FileDir_PageManager(SAPDBMem_IRawAllocator& alloc)
    : m_alloc(alloc),
      m_savepointActive(SavepointNotActive),
      m_pageHash(alloc),
      m_freePageList(alloc),
      m_pageIndexPageList(alloc),
      m_fragmentedPageList(alloc),
      m_restartPageList(alloc)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_PageManager::FileDir_PageManager",
                             FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_instance == 0 );

    m_freePageList.Reserve(c_initialPageListSize);
    m_instance = this;
}

/**************************************************************************/

FileDir_PageManager::~FileDir_PageManager()
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_PageManager::~FileDir_PageManager",
                             FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE(m_pageIndexPageList.GetSize() == 0);

    m_instance = 0;
}

/**************************************************************************/

SAPDB_Bool FileDir_PageManager::Initialize()
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_PageManager::Initialize",
                             FileDir_Trace, 5 );
    return m_freePageList.Reserve(c_initialPageListSize);
}

/**************************************************************************/

FileDir_ReturnCode FileDir_PageManager::Shutdown(
    const RTE_TaskId taskId)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_PageManager::Shutdown",
                             FileDir_Trace, 5 );
    m_lock.AcquireWriteLock(taskId);
    // since m_pageHash and m_freePageList point to the same pages it
    // is enough to go through m_pageHash and destroy all objects:
    PageHash::Iterator it  = m_pageHash.Begin();
    PageHash::Iterator eit = m_pageHash.End();
    while ( it != eit ) {
        FileDir_Page* fp = (*it).value;
        Data_PageFrame frame = fp->GetFrame();
        destroy(fp, m_alloc);
        ++it;
    }
    m_pageHash.Delete();
    m_freePageList.Delete();

    // free all page index pages
    PageIndexPageList::Iterator it1  = m_pageIndexPageList.Begin();
    PageIndexPageList::Iterator eit1 = m_pageIndexPageList.End();
    while ( it1 != eit1 ) {
        FileDir_PageIndexPage* fp = *it1;
        Data_PageFrame frame = fp->GetFrame();
        destroy( fp, m_alloc );
        ++it1;
    }
    m_pageIndexPageList.Clear();

    m_lock.ReleaseWriteLock(taskId);
    return FileDir_Okay;
}

/**************************************************************************/

SAPDB_Bool FileDir_PageManager::CreateRootPage(
    const RTE_TaskId taskId,
    const Data_PageNo& rootPageNo)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_PageManager::CreateRootPage",
                             FileDir_Trace, 5 );

    // only makes sense if no pages exist
    SAPDBERR_ASSERT_ARGUMENT( m_pageIndexPageList.GetSize() == 0 );

    FileDir_PageIndexPage* indexPage =
        new(m_alloc) FileDir_PageIndexPage( rootPageNo );
    if ( (indexPage == 0) || !indexPage->Initialize() ) {
        FileDir_Common::OutOfMemoryCrash( sizeof(FileDir_PageIndexPage) );
    }

    m_pageIndexPageList.InsertEnd(indexPage);
    return true;
}

/**************************************************************************/

Data_PageNo FileDir_PageManager::GetRootPageNo()
{
    SAPDBERR_ASSERT_ARGUMENT( m_pageIndexPageList.GetSize() > 0 );

    return m_pageIndexPageList[0]->PageNo();
}

/**************************************************************************/

FileDir_ReturnCode FileDir_PageManager::CheckData(
    RTE_TaskId taskId,
    SAPDB_Bool withUpdate)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_PageManager::CheckData",
                             FileDir_Trace, 5 );

    const RTETask_ITask&  task      = *RTETask_ITask::ByTaskID( taskId );
    Converter_IVerify&    converter = Converter_IVerify::Instance();

    // verify all page index pages
    PageIndexPageList::Iterator ipit  = m_pageIndexPageList.Begin();
    PageIndexPageList::Iterator ipeit = m_pageIndexPageList.End();
    while ( ipit != ipeit ) {
        FileDir_PageIndexPage* fp = *ipit;

        FileDir_ReturnCode ret = fp->CheckData( taskId );
        if ( ret != FileDir_Okay ) {
            return ret;
        }

        if ( withUpdate ) {
            converter.MarkPageNoAsUsed(
                task,
                fp->PageNo(),
                Data_Dynamic);
        }
        ++ipit;
    }

    // verify all regular file directory pages
    PageHash::Iterator it  = m_pageHash.Begin();
    PageHash::Iterator eit = m_pageHash.End();
    while ( it != eit ) {
        FileDir_Page* fp = (*it).value;

        FileDir_ReturnCode ret = fp->CheckData( taskId );
        if ( ret != FileDir_Okay ) {
            return ret;
        }

        if ( withUpdate ) {
            converter.MarkPageNoAsUsed(
                task,
                fp->PageNo(),
                Data_Dynamic);
        }
        ++it;
    }
    return FileDir_Okay;
}

/**************************************************************************/

SAPDB_Bool FileDir_PageManager::MarkPageForWrite(
    const RTE_TaskId   taskId,
    const Data_PageNo& pageNo)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_PageManager::MarkPageForWrite",
                             FileDir_Trace, 5 );

    // first check if page is regular page
    PageHash::Iterator it = m_pageHash.Find( pageNo );
    if ( it.IsValid() ) {
        (*it).value->LockPageForUpdate( taskId );
        (*it).value->UnlockPageForUpdate( taskId );
        return true;
    }

    // check if page is page index page
    PageIndexPageList::Iterator pNoIndexIt    =
        m_pageIndexPageList.Begin();
    PageIndexPageList::Iterator pNoIndexItEnd =
        m_pageIndexPageList.End();
    while ( pNoIndexIt != pNoIndexItEnd ) {
        if ( pageNo == (*pNoIndexIt)->PageNo() ) {
            // index pages always get flushed, no need to mark it dirty
            return true;
        }
        ++pNoIndexIt;
    }

    return false;
}

/**************************************************************************/

char* FileDir_PageManager::ReserveSpace(
    const RTE_TaskId taskId,
    const SAPDB_Int recordLength,
    const SAPDB_Bool keepPageLock,
    FileDir_Page*& entryDataPage,
    const Data_PageNo notOnPageNo)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_PageManager::ReserveSpace",
                             FileDir_Trace, 5 );
    SAPDBERR_ASSERT_ARGUMENT(recordLength < FileDir_Page::PageDataSize);
    SAPDBERR_ASSERT_ARGUMENT(recordLength >= sizeof(
                                 FileDir_Page::RecordHeader));
    SAPDBERR_ASSERT_STATE(m_pageIndexPageList.GetSize() > 0);

    /*
     * The write lock has to be kept until space is reserved on the
     * page because two concurrent reservations might compete
     * for the same chunk of memory (race condition).
     *
     * An alternative would be to hand all data necessary for the
     * allocation to ReserveSpace() and to allow it to fail.
     */
    const RTETask_ITask& task = *RTETask_ITask::ByTaskID( taskId );

    FileDir_RWAutoRegion writeLock =
        FileDir_RWAutoRegion( m_lock, true, taskId );

    // round up requested space to insure proper alignment:
    union alignUnion { SAPDB_Long a; void* b; double c; };
    int alignSize    = sizeof(alignUnion);
    SAPDB_Int recLen = (((recordLength-1) / alignSize) + 1) * alignSize;

    PageList::Iterator pageIterator = m_freePageList.Begin();
    FileDir_Page* dataPage = 0;
    while ( pageIterator != m_freePageList.End() ) {
        SAPDB_UInt2 freeBlockSize = (*pageIterator)->GetLargestFreeBlock();
        if (((*pageIterator)->PageNo() != notOnPageNo) &&
            ( freeBlockSize >= recLen) ) {
                dataPage = *pageIterator;
                break;
        }
        if (freeBlockSize < c_minimalBlockSize) {
            (*pageIterator)->SetIsInFreePageList( false );
            m_freePageList.Delete( pageIterator );
            continue;
        }
        ++pageIterator;
    }

    if ( pageIterator == m_freePageList.End() ) {
        // a new page needs to be allocated
        Data_PageNo pNo;
        Converter_IPageNoManager::Instance().RequestNewPermPageNo(
            task, false, pNo);

        dataPage = new(m_alloc) FileDir_Page(
            m_alloc,
            taskId,
            pNo );
        if ( dataPage == 0 ) {
            return 0;
        }

        if ( !dataPage->Initialize() ||
             !AddPage(taskId, dataPage) ) {
            destroy( dataPage, m_alloc );
            Converter_IPageNoManager::Instance().FreePermPageNo(
                task, pNo, Converter_ICommon::Instance().Version() );
            return 0;
        }
    }

    dataPage->LockPageForUpdate( taskId );
    char* dataPointer = dataPage->ReserveSpace( taskId, recLen );

    if ( !keepPageLock ) {
        dataPage->UnlockPageForUpdate( taskId );
    }

    entryDataPage = dataPage;
    return dataPointer;
}

/**************************************************************************/

char* FileDir_PageManager::RestartReserveSpace(
    const RTE_TaskId taskId,
    const SAPDB_Int  recordLength,
    const SAPDB_Bool keepPageLock,
    FileDir_Page*&   entryDataPage)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_PageManager::RestartReserveSpace",
                             FileDir_Trace, 5 );
    SAPDBERR_ASSERT_ARGUMENT(recordLength < FileDir_Page::PageDataSize);
    SAPDBERR_ASSERT_ARGUMENT(recordLength >= sizeof(
                                 FileDir_Page::RecordHeader));
    SAPDBERR_ASSERT_STATE(m_pageIndexPageList.GetSize() > 0);

    /*
     * The write lock has to be kept until space is reserved on the
     * page because two concurrent reservations might compete
     * for the same chunk of memory (race condition).
     *
     * An alternative would be to hand all data necessary for the
     * allocation to ReserveSpace() and to allow it to fail.
     */
    const RTETask_ITask& task = *RTETask_ITask::ByTaskID( taskId );

    FileDir_RWAutoRegion writeLock =
        FileDir_RWAutoRegion( m_lock, true, taskId );

    // round up requested space to insure proper alignment:
    union alignUnion { SAPDB_Long a; void* b; double c; };
    int alignSize    = sizeof(alignUnion);
    SAPDB_Int recLen = (((recordLength-1) / alignSize) + 1) * alignSize;

    PageList::Iterator pageIterator = m_restartPageList.Begin();
    FileDir_Page* dataPage = 0;
    while ( pageIterator != m_restartPageList.End() ) {
        SAPDB_UInt2 freeBlockSize = (*pageIterator)->GetLargestFreeBlock();
        if ( freeBlockSize >= recLen ) {
            dataPage = *pageIterator;
            break;
        }
        ++pageIterator;
    }

    if ( pageIterator == m_restartPageList.End() && (dataPage == 0) ) {
        // a new page needs to be allocated
        Data_PageNo pNo;
        Converter_IPageNoManager::Instance().RequestNewPermPageNo(
            task, false, pNo);

        dataPage = new(m_alloc) FileDir_Page(
            m_alloc,
            taskId,
            pNo );
        if ( dataPage == 0 ) {
            return 0;
        }

        if ( dataPage->Initialize() ) {
            if ( ! m_restartPageList.InsertEnd( dataPage ) ) {
                destroy( dataPage, m_alloc );
                Converter_IPageNoManager::Instance().FreePermPageNo(
                    task, pNo, Converter_ICommon::Instance().Version() );
                return 0;
            }
        }
    }

    dataPage->LockPageForUpdate( taskId );
    char* dataPointer = dataPage->ReserveSpace( taskId, recLen );

    if ( !keepPageLock ) {
        dataPage->UnlockPageForUpdate( taskId );
    }

    entryDataPage = dataPage;
    return dataPointer;
}

/**************************************************************************/

SAPDB_Bool FileDir_PageManager::RestartFinished(
    const RTE_TaskId taskId,
    Msg_List&        msgList)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_PageManager::RestartFinished",
                             FileDir_Trace, 5 );

    PageList::Iterator pageIterator = m_restartPageList.Begin();
    FileDir_Page* dataPage = 0;
    for ( PageList::Iterator pageItEnd = m_restartPageList.End();
          pageIterator != pageItEnd;
          ++pageIterator ) {

        if ( ! AddPage( taskId, *pageIterator ) ) {
            msgList = Msg_List(
                Msg_List::Error,
                SDBMSG_FILEDIR_PAGEMAN_RESTART_ADDPAGE_FAILED,
                Msg_Arg( SDBMSGTAG_FILEDIR_PAGEMAN_RESTART_ADDPAGE_FAILED__PAGENO,
                         SAPDB_ToString( (*pageIterator)->PageNo() ) ),
                Msg_Arg( SDBMSGTAG_FILEDIR_PAGEMAN_RESTART_ADDPAGE_FAILED__SIZE,
                         SAPDB_ToString( m_freePageList.GetSize() ) ) );
            return false;
        }
    }

    m_restartPageList.Clear();

    return true;
}

/**************************************************************************/

/*!
 * @brief Adds dataPage to list of used data pages. Page lists
 *        need to be locked when calling this method.
 *
 * @param taskId    [in] task id
 * @param dataPage  [in] data page to add
 * @param inRestart [in] if true, dataPage will not be added to index pages
 */
bool FileDir_PageManager::AddPage(
    const RTE_TaskId taskId,
    FileDir_Page* dataPage,
    const bool inRestart)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_PageManager::AddPage",
                             FileDir_Trace, 5 );

    SAPDBTRACE_WRITELN( FileDir_Trace, 5,
                        "adding page #" << dataPage->PageNo() << NewLine <<
                        "inRestart : " << inRestart );

#ifdef SAPDB_SLOW
    PageHash::Iterator it1 = m_pageHash.Find( dataPage->PageNo() );
    SAPDBERR_ASSERT_STATE( !it1.IsValid() );
#endif

    PageHash::Iterator it = m_pageHash.Insert( dataPage->PageNo(), dataPage );
    if ( !it.IsValid() ) {
        return false;
    }

    if ( !inRestart ) {
        // add page to page index pages
        PageIndexPageList::Iterator pNoIndexIt    =
            m_pageIndexPageList.Begin();
        PageIndexPageList::Iterator pNoIndexItEnd =
            m_pageIndexPageList.End();
        while ( pNoIndexIt != pNoIndexItEnd ) {
            if ( !(*pNoIndexIt)->IsFull() ) {
                break;
            }
            ++pNoIndexIt;
        }
        if ( pNoIndexIt == pNoIndexItEnd ) {
            if ( !AddIndexPage( taskId ) ) {
                // adding page to index page failed
                return false;
            }
            pNoIndexIt =
                m_pageIndexPageList.Position(
                    m_pageIndexPageList.GetSize() - 1 );
        }
        (*pNoIndexIt)->AddPage( *dataPage );
    }


    if ( dataPage->GetLargestFreeBlock() > c_minimalBlockSize ) {
        m_freePageList.InsertEnd( dataPage );
        dataPage->SetIsInFreePageList( true );
    } else {
        dataPage->SetIsInFreePageList( false );
    }

    return true;
}

/**************************************************************************/

/*!
 * @brief Removes dataPage from list of used data pages. Page lists
 *        need to be locked when calling this method.
 */
void FileDir_PageManager::RemovePage(
    const RTE_TaskId taskId,
    PageHash::Iterator& iterator)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_PageManager::RemovePage",
                             FileDir_Trace, 5 );

    FileDir_Page* dataPage = (*iterator).value;
    SAPDBTRACE_WRITELN( FileDir_Trace, 5, "Freeing page #"
                        << dataPage->PageNo() << "." );

    // remove from free page list (if present)
    PageList::Iterator listIt  = m_freePageList.Begin();
    PageList::Iterator listEnd = m_freePageList.End();
    while (listIt != listEnd) {
        if ( *listIt == dataPage ) {
            m_freePageList.Delete( listIt );
            break;
        }
        ++listIt;
    }

    // remove from pageHash
    m_pageHash.Delete( iterator );

    // remove page from page index
    PageIndexPageList::Iterator pNoIndexIt    =
        m_pageIndexPageList.Begin();
    PageIndexPageList::Iterator pNoIndexItEnd =
        m_pageIndexPageList.End();
    while ( pNoIndexIt != pNoIndexItEnd ) {
        if ( (*pNoIndexIt)->RemovePage(*dataPage) ) {
            if ( (*pNoIndexIt)->IsEmpty() ) {
                RemoveIndexPage( taskId, *pNoIndexIt );
            }
            break;
        }
        ++pNoIndexIt;
    }
    const RTETask_ITask& task = *RTETask_ITask::ByTaskID( taskId );

    Converter_IPageNoManager::Instance().FreePermPageNo(
        task, dataPage->PageNo(), dataPage->ConverterVersion() );
    destroy( dataPage, m_alloc );
}

/**************************************************************************/

bool FileDir_PageManager::AddIndexPage(
    const RTE_TaskId taskId)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_PageManager::AddIndexPage",
                             FileDir_Trace, 5 );

    const RTETask_ITask& task = *RTETask_ITask::ByTaskID( taskId );
    Data_PageNo          pNo;

    Converter_IPageNoManager::Instance().RequestNewPermPageNo(
        task, false, pNo);
    FileDir_PageIndexPage* indexPage =
        new(m_alloc) FileDir_PageIndexPage( pNo );

    if ( indexPage == 0 ) {
        return false;
    }

    SAPDBTRACE_WRITELN(
        FileDir_Trace, 5,
        "adding index page #" << indexPage->PageNo() );

    if ( !indexPage->Initialize() ) {
        destroy( indexPage, m_alloc );
        Converter_IPageNoManager::Instance().FreePermPageNo(
            task, pNo, Converter_ICommon::Instance().Version() );
        return false;
    }

    PageIndexPageList::Iterator pNoIndexIt    =
        m_pageIndexPageList.Begin();
    PageIndexPageList::Iterator pNoIndexItEnd =
        m_pageIndexPageList.End();
    while ( pNoIndexIt != pNoIndexItEnd ) {
        if ( (*pNoIndexIt)->GetNextPage().IsInvalid() ) {
            (*pNoIndexIt)->SetNextPage(indexPage->PageNo());
            break;
        }
        ++pNoIndexIt;
    }
    m_pageIndexPageList.InsertEnd(indexPage);
    return true;
}

/**************************************************************************/

void FileDir_PageManager::RemoveIndexPage(
    const RTE_TaskId taskId,
    FileDir_PageIndexPage* indexPage)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_PageManager::RemoveIndexPage",
                             FileDir_Trace, 5 );

    SAPDBTRACE_WRITELN(
        FileDir_Trace, 5,
        "removing index page #" << indexPage->PageNo() );

    PageIndexPageList::Iterator pNoIndexIt    =
        m_pageIndexPageList.Begin();
    PageIndexPageList::Iterator pNoIndexItEnd =
        m_pageIndexPageList.End();
    if ( (*pNoIndexIt) == indexPage ) {
        SAPDBTRACE_WRITELN( FileDir_Trace, 5, "remove skipped" );
        // never remove first page as restart record is pointing to it
        return;
    }

    // remove from index page chain
    while ( pNoIndexIt != pNoIndexItEnd ) {
        if ( (*pNoIndexIt)->GetNextPage() == indexPage->PageNo() ) {
            (*pNoIndexIt)->SetNextPage(indexPage->GetNextPage());
            break;
        }
        ++pNoIndexIt;
    }

    // remove from index page list
    PageIndexPageList::Iterator listIt  = m_pageIndexPageList.Begin();
    PageIndexPageList::Iterator listEnd = m_pageIndexPageList.End();
    while (listIt != listEnd) {
        if ( *listIt == indexPage ) {
            m_pageIndexPageList.Delete(listIt);
            break;
        }
        ++listIt;
    }
    const RTETask_ITask& task = *RTETask_ITask::ByTaskID( taskId );

    Converter_IPageNoManager::Instance().FreePermPageNo(
        task, indexPage->PageNo(), indexPage->ConverterVersion() );
    destroy( indexPage, m_alloc );
}

/**************************************************************************/

void FileDir_PageManager::FlushDirtyPages(
    const RTE_TaskId taskId)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_PageManager::FlushDirtyPages",
                             FileDir_Trace, 5 );

    m_lock.AcquireWriteLock( taskId );

    // list with fragmented pages is created from scratch
    m_fragmentedPageList.Delete();

    // flush index pages
    PageIndexPageList::Iterator pNoIndexIt    =
        m_pageIndexPageList.Begin();
    PageIndexPageList::Iterator pNoIndexItEnd =
        m_pageIndexPageList.End();
    while ( pNoIndexIt != pNoIndexItEnd ) {
        (*pNoIndexIt)->Flush( taskId );
        ++pNoIndexIt;
    }

    // flush content pages
    int flushedPages = 0;
    PageHash::Iterator pageIt  = m_pageHash.Begin();
    PageHash::Iterator pageEnd = m_pageHash.End();
    while ( pageIt != pageEnd ) {
        if ( pageIt->value->IsDirty() ) {
            pageIt->value->Flush( taskId );
            ++flushedPages;
            if ( pageIt->value->IsEmpty() ) {
                RemovePage( taskId, pageIt );
                continue;
            } else if ( pageIt->value->GetFragmentationMeasure() > 50 ) {
                m_fragmentedPageList.InsertEnd( pageIt->value );
                if ( pageIt->value->GetIsInFreePageList() ) {
                    // page is fragmented, remove from free page list
                    PageList::Iterator freePageIt = m_freePageList.Begin();
                    while ( freePageIt != m_freePageList.End() ) {
                        if ( pageIt->value->PageNo()
                             ==
                             (*freePageIt)->PageNo() ) {
                            m_freePageList.Delete( freePageIt );
                            break;
                        }
                        ++freePageIt;
                    }
                    pageIt->value->SetIsInFreePageList( false );
                }
            } else if ( (pageIt->value->GetLargestFreeBlock()
                        > c_minimalBlockSize) &&
                !pageIt->value->GetIsInFreePageList() ) {
                // insert page into free page list
#ifdef SAPDB_SLOW
                // make sure page is not already in list
                PageList::Iterator freePageIt = m_freePageList.Begin();
                while ( freePageIt != m_freePageList.End() ) {
                    SAPDBERR_ASSERT_STATE( pageIt->value != *freePageIt );
                    ++freePageIt;
                }
#endif
                m_freePageList.InsertEnd( pageIt->value );
                pageIt->value->SetIsInFreePageList( true );
            }
        }
        ++pageIt;
    }
    m_lock.ReleaseWriteLock(taskId);

    SAPDBTRACE_WRITELN(
        FileDir_Trace, 5,
        "Flushed " << flushedPages << " of " << m_pageHash.GetSize()
        << " pages." );
}

/**************************************************************************/

void FileDir_PageManager::BeginSavepoint(const RTE_TaskId taskId)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_PageManager::BeginSavepoint",
                             FileDir_Trace, 5 );

    SAPDBERR_ASSERT_STATE( m_savepointActive == SavepointNotActive );

    RTESys_AtomicWrite( m_savepointActive, SavepointActive );

    m_savepointLock.AcquireWriteLock( taskId );
}

/**************************************************************************/

void FileDir_PageManager::EndSavepoint(const RTE_TaskId taskId)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_PageManager::EndSavepoint",
                             FileDir_Trace, 5 );

    SAPDBERR_ASSERT_STATE( m_savepointActive == SavepointActive );

    RTESys_AtomicWrite( m_savepointActive, SavepointNotActive );

    m_savepointLock.ReleaseWriteLock( taskId );
}

/**************************************************************************/

SAPDB_Int FileDir_PageManager::GetFragmentedPagesPercentage() const
{
    return m_pageHash.GetSize() > 0 ?
        (m_fragmentedPageList.GetSize()*100) / (m_pageHash.GetSize()*100)
        :
        0;
}

/**************************************************************************/

SAPDB_Bool FileDir_PageManager::IsSavepointActive()
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_PageManager::IsSavepointActive",
                             FileDir_Trace, 5 );
    return m_savepointActive == SavepointActive;
}

/**************************************************************************/
/**************************************************************************/

FileDir_PageManager::EntryIterator::EntryIterator(
    FileDir_PageManager& pageManager)
    : m_pageManager(pageManager),
      m_pageIterator(pageManager.m_pageHash.Begin()),
      m_lastPagePos(pageManager.m_pageHash.End()),
      m_currentEntry(0)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_PageManager::EntryIterator::EntryIterator",
        FileDir_Trace, 5 );
    if ( m_pageIterator == m_lastPagePos ) {
        return;
    }

    m_entryIterator = (*pageManager.m_pageHash.Begin()).value->Begin();
    m_lastEntryPos  = (*m_pageIterator).value->End();
    if ( m_entryIterator != m_lastEntryPos  ) {
        m_currentEntry = *m_entryIterator;
        m_currentPage  = (*m_pageIterator).value;
    } else {
        ++*this;
    }
}

/**************************************************************************/

FileDir_PageManager::EntryIterator::EntryIterator(
    FileDir_PageManager& pageManager, bool)
    : m_pageManager(pageManager),
      m_pageIterator(pageManager.m_pageHash.End()),
      m_lastPagePos(pageManager.m_pageHash.End()),
      m_currentEntry(0)
{
}

/**************************************************************************/

FileDir_PageManager::EntryIterator::~EntryIterator()
{
}

/**************************************************************************/

/*!
 * @brief Returns the entry the iterator is pointing to.
 */
FileDir_PageManager::EntryAddress*
FileDir_PageManager::EntryIterator::operator*()
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_PageManager::EntryIterator::operator*",
        FileDir_Trace, 7 );
    SAPDBTRACE_WRITELN(
        FileDir_Trace, 7,
        "m_currentPage  : " << m_currentPage->PageNo() << NewLine <<
        "m_currentEntry : " << ToStr(m_currentEntry) );

    m_currentEntryAddress.dataPage     = m_currentPage;
    m_currentEntryAddress.entryAddress = m_currentEntry;

    return
        (m_currentEntryAddress.entryAddress != 0) ? &m_currentEntryAddress : 0;
}

/**************************************************************************/

/*!
 * @brief Returns the entry the iterator is pointing to and advances the
 * iterator by one.
 *
 * @warning The entry's converter version is not checked!
 */
FileDir_PageManager::EntryIterator&
FileDir_PageManager::EntryIterator::operator++()
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_PageManager::EntryIterator::operator++",
        FileDir_Trace, 7 );

    if ( m_pageIterator != m_lastPagePos ) {
        // advance to next valid entry
        if ( (*m_pageIterator).value != 0 ) {
            ++m_entryIterator;
            while ( m_entryIterator == m_lastEntryPos ) {
                ++m_pageIterator;
                if ( m_pageIterator == m_lastPagePos ) {
                    break;
                }
                m_entryIterator = (*m_pageIterator).value->Begin();
                m_lastEntryPos  = (*m_pageIterator).value->End();
            }
        }
    }
    if ( m_pageIterator != m_lastPagePos ) {
        m_currentEntry = *m_entryIterator;
        m_currentPage  = (*m_pageIterator).value;
    } else {
        m_currentEntry = 0;
    }

    return *this;
}

/**************************************************************************/

/*!
 * @brief Advance iterator by one.
 */
FileDir_PageManager::EntryIterator
FileDir_PageManager::EntryIterator::operator++(int)
{
    EntryIterator it = *this;
    ++*this;
    return it;
}

/**************************************************************************/

bool FileDir_PageManager::EntryIterator::operator==(
    const EntryIterator& mm) const
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_PageManager::EntryIterator::operator==",
        FileDir_Trace, 7 );
    return m_currentEntry == mm.m_currentEntry;
}

/**************************************************************************/

bool FileDir_PageManager::EntryIterator::operator!=(
    const EntryIterator& mm) const
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_PageManager::EntryIterator::operator!=",
        FileDir_Trace, 7 );
    return m_currentEntry != mm.m_currentEntry;
}

/**************************************************************************/


/**************************************************************************/

void FileDir_PageManager::Verify()
{
    // FIXME: actually verify something
    // e.g.: are all pages registered on an index page and vice versa,
    //       do all pages exist in converter

}

/**************************************************************************/

FileDir_PageManager::EntryAddress FileDir_PageManager::GetEntry(
    const SAPDB_Int4 pageNumber,
    const SAPDB_UInt2 entryLocation)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_PageManager::GetEntry",
                             FileDir_Trace, 5 );

    PageHash::Iterator it = m_pageHash.Find(pageNumber);
    if ( !it.IsValid() ) {
        FileDir_Exception errMsg(
            __CONTEXT__, FILEDIR_WRONG_PAGE_NUMBER,
            SAPDB_ToString(pageNumber) );
        RTE_Crash(errMsg);
    }

    FileDir_Page* p = (*it).value;
    char* ea = p->GetEntryAddress(entryLocation);

    return EntryAddress(p, ea);
}

/**************************************************************************/

void FileDir_PageManager::Restart_InitializePages(
    const RTE_TaskId taskId,
    const SAPDB_Bool cleanupPages)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_PageManager::Restart_CleanUpPages",
                             FileDir_Trace, 5 );

    // locking should not be necessary during restart, but should not
    // be a problem either
    m_lock.AcquireWriteLock(taskId);

    PageHash::Iterator pageIterator = m_pageHash.Begin();
    while ( pageIterator != m_pageHash.End() ) {
        (*pageIterator).value->Restart_ReinitializeFromContent();
        if ( cleanupPages ) {
            (*pageIterator).value->Restart_Defragment();
        }
        if ( (*pageIterator).value->IsEmpty() ) {
            RemovePage( taskId, pageIterator );
            continue;
        }
        if ( !(*pageIterator).value->GetIsInFreePageList() &&
             ((*pageIterator).value->GetLargestFreeBlock() > c_minimalBlockSize)
            ) {
            m_freePageList.InsertEnd( (*pageIterator).value );
            (*pageIterator).value->SetIsInFreePageList( true );
        }
        ++pageIterator;
    }

    m_lock.ReleaseWriteLock(taskId);
}

/**************************************************************************/

FileDir_ReturnCode FileDir_PageManager::Restart_LoadPages(
    const RTE_TaskId taskId,
    const Data_PageNo& startPageNo)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_PageManager::Restart_LoadPages",
                             FileDir_Trace, 5 );
    SAPDBERR_ASSERT_ARGUMENT( startPageNo.IsValid() );
    SAPDBERR_ASSERT_STATE( m_pageIndexPageList.GetSize() == 0 );

    if ( m_pageIndexPageList.GetSize() > 0 ) {
        return FileDir_AlreadyRestarted;
    }

    Data_PageNo pageNo(startPageNo);
    FileDir_ReturnCode retCode = FileDir_Okay;

    // load all page index pages
    while ( !pageNo.IsInvalid() ) {
        FileDir_PageIndexPage* page =
            new(m_alloc) FileDir_PageIndexPage( pageNo );
        if ( (page == 0) || !page->Initialize() ) {
            FileDir_Common::OutOfMemoryCrash(
                sizeof(FileDir_PageIndexPage) );
        }

        IOMan_ReturnCode ret =
            IOMan_IDataIO::GetInstance().ReadDataPage( taskId, *page, pageNo );

        if ( ret != IOMan_Okay ) {
            FileDir_Exception errMsg(
                __CONTEXT__, FILEDIR_LOAD_PAGE_FAILED,
                SAPDB_ToString(pageNo),
                SAPDB_ToString(ret) );
            RTE_Crash(errMsg);
        }

        page->ReinitializeFromContent();

        m_pageIndexPageList.InsertEnd(page);

        pageNo = page->GetNextPage();
    } /* while */


    // now load all content pages
    PageNoIterator pNoIt  = PageNoListBegin();
    PageNoIterator pNoEnd = PageNoListEnd();
    while ( pNoIt != pNoEnd ) {
        pageNo = pNoIt.Next( taskId );

        FileDir_Page* page = new(m_alloc) FileDir_Page(
            m_alloc,
            taskId,
            pageNo,
            false);
        if ( (page == 0) || !page->Initialize() ) {
            FileDir_Common::OutOfMemoryCrash( sizeof(FileDir_Page) );
        }

        IOMan_ReturnCode ret =
            IOMan_IDataIO::GetInstance().ReadDataPage( taskId, *page, pageNo );

        if ( ret != IOMan_Okay ) {
            FileDir_Exception errMsg(
                __CONTEXT__, FILEDIR_LOAD_PAGE_FAILED,
                SAPDB_ToString(pageNo),
                SAPDB_ToString(ret) );
            RTE_Crash(errMsg);
        }

        // page can only be initialized from content once all valid
        // entries have been determined and the page has been
        // defragmented

        AddPage(taskId, page, true);
    }
    return retCode;
}

/**************************************************************************/

FileDir_PageManager::PageNoIterator::PageNoIterator(
    FileDir_PageManager& pageManager)
    : m_pageManager(pageManager),
      m_pageIterator(pageManager.m_pageIndexPageList.Begin()),
      m_pNoIterator((*pageManager.m_pageIndexPageList.Begin())->Begin())
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_PageManager::PageNoIterator",
        FileDir_Trace, 5 );
    if ( m_pNoIterator == (*m_pageIterator)->End() ) {
        // handle empty first page index page (should only happen with
        // if no data pages have been saved)
        ++m_pageIterator;
        if ( m_pageIterator != m_pageManager.m_pageIndexPageList.End() ) {
            m_pNoIterator = (*m_pageIterator)->Begin();
        }
    }
}

/**************************************************************************/

FileDir_PageManager::PageNoIterator::PageNoIterator(
    FileDir_PageManager& pageManager,
    bool)
    : m_pageManager(pageManager),
      m_pageIterator(pageManager.m_pageIndexPageList.End()),
      m_pNoIterator((*pageManager.m_pageIndexPageList.Begin())->Begin())
{}

/**************************************************************************/

FileDir_PageManager::PageNoIterator::PageNoIterator(
    const PageNoIterator& mm)
    : m_pageManager(mm.m_pageManager),
      m_pageIterator(mm.m_pageIterator),
      m_pNoIterator(mm.m_pNoIterator)
{}

/**************************************************************************/

Data_PageNo FileDir_PageManager::PageNoIterator::Next(
    const RTE_TaskId taskId)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_PageManager::PageNoIterator::Next",
        FileDir_Trace, 7 );

    m_lock.AcquireWriteLock( taskId );
    Data_PageNo currentPage = *m_pNoIterator;
    ++m_pNoIterator;
    if ( m_pNoIterator == (*m_pageIterator)->End() ) {
        ++m_pageIterator;
        if ( m_pageIterator != m_pageManager.m_pageIndexPageList.End() ) {
            m_pNoIterator = (*m_pageIterator)->Begin();
        }
    }
    m_lock.ReleaseWriteLock( taskId );
    return currentPage;
}

/**************************************************************************/

SAPDB_Bool FileDir_PageManager::PageNoIterator::operator==(
    const PageNoIterator& mm) const
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_PageManager::PageNoIterator::operator==",
        FileDir_Trace, 7 );

    if ( (m_pageIterator == mm.m_pageIterator) &&
         (m_pageIterator == m_pageManager.m_pageIndexPageList.End()) ) {
        return true;
    }

    return ( m_pageIterator == mm.m_pageIterator ) &&
        (m_pNoIterator == mm.m_pNoIterator);
}

/**************************************************************************/

SAPDB_Bool FileDir_PageManager::PageNoIterator::operator!=(
    const PageNoIterator& mm) const
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_PageManager::PageNoIterator::operator!=",
        FileDir_Trace, 7 );
    return !operator==(mm);
}

/**************************************************************************/

/// To be used by FileDir_Test only.
void FileDir_PageManager::AcquireSharedSavepointLock()
{
    m_savepointLock.AcquireReadLock();
}

/**************************************************************************/

/// To be used by FileDir_Test only.
void FileDir_PageManager::ReleaseSharedSavepointLock()
{
    m_savepointLock.ReleaseReadLock();
}

/**************************************************************************/

void FileDir_PageManager::Dump(const RTE_TaskId taskId, Kernel_Dump& dump)
{
    if ( m_pageIndexPageList.GetSize() == 0 ) {
        return;
    }

    // dump page index pages
    PageIndexPageList::Iterator ipit  = m_pageIndexPageList.Begin();
    PageIndexPageList::Iterator ipeit = m_pageIndexPageList.End();
    while ( ipit != ipeit ) {
        FileDir_PageIndexPage* fp = *ipit;
        dump.InsertPage( *fp );
        ++ipit;
    }

    // dump regular pages
    PageHash::Iterator it  = m_pageHash.Begin();
    PageHash::Iterator eit = m_pageHash.End();
    while ( it != eit ) {
        FileDir_Page* fp = (*it).value;
        dump.InsertPage( *fp );
        ++it;
    }
}

/**************************************************************************/

void FileDir_PageManager::Extract(
    const RTE_TaskId taskId,
    Kernel_FileIO&   ioFile)
{
    if ( m_pageIndexPageList.GetSize() == 0 ) {
        return;
    }

    // dump page index pages
    PageIndexPageList::Iterator ipit  = m_pageIndexPageList.Begin();
    PageIndexPageList::Iterator ipeit = m_pageIndexPageList.End();
    while ( ipit != ipeit ) {
        FileDir_PageIndexPage* fp = *ipit;
        ioFile.Write( *fp );
        ++ipit;
    }

    // dump regular pages
    PageHash::Iterator it  = m_pageHash.Begin();
    PageHash::Iterator eit = m_pageHash.End();
    while ( it != eit ) {
        FileDir_Page* fp = (*it).value;
        ioFile.Write( *fp );
        ++it;
    }
}

/**************************************************************************/
