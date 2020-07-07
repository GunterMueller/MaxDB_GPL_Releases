/*****************************************************************************/
/*!
 @file    FileDir_Entry.cpp
 @author  MartinKi
 @ingroup FileDirectory_Files

 @brief Implementation of class FileDir_Entry.

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

#include "FileDirectory/FileDir_SharedDirectory.hpp"
#include "FileDirectory/FileDir_Entry.hpp"
#include "FileDirectory/FileDir_PageManager.hpp"
#include "FileDirectory/FileDir_Page.hpp"
#include "FileDirectory/FileDir_RWRegionWrapper.hpp"
#include "FileDirectory/FileDir_Common.hpp"
#include "FileDirectory/FileDir_Exception.hpp"
#include "FileDirectory/FileDir_Messages.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"

#include "DataAccess/Data_FileTypes.hpp"
// for Data_PageNo:
#include "DataAccess/Data_Types.hpp"

#include "Converter/Converter_Version.hpp"
#include "Converter/Converter_ICommon.hpp"

#include "KernelCommon/Kernel_Dump.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"

#include "RunTime/RTE_Crash.hpp"
#include "RunTime/System/RTESys_AtomicOperation.hpp"
#include "RunTime/System/RTESys_MemoryBarrier.h"
#include "RunTime/RTE_Message.hpp"
#include "RunTime/RTE_Types.hpp"

#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/Fields/SAPDBFields_Field.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"

#include "ggg91.h"
#include "hgg06.h" // gg06SetNilTrans
#include "hgg01_3.h" // g01vtrace

// vgetpid():
#include "heo51.h"

#include "hbd300.h" // bd300SetRootCheck

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/
/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/
/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

FileDir_Entry::FileDir_Entry(SAPDBMem_IRawAllocator& allocator)
    : FileDir_BaseEntry(allocator),
      m_persistentLock(0),
      m_persistentLockVersion(0),
      m_persistentData(0),
      m_leafPageCountDelta(0),
      m_indexPageCountDelta(0),
      m_entryCountDelta(0),
      m_newPersistentState(Data_FileOk),
      m_needsUpdateAfterSavepoint(0),
      m_dataPage(0)
{
    m_node.entry = this;
}

/**************************************************************************/

FileDir_Entry::FileDir_Entry(
    const RTE_TaskId         taskId,
    SAPDBMem_IRawAllocator&  allocator,
    const SAPDB_UInt2        requiredSize,
    const FileDir_FileNo     fileNo,
    const Converter_Version& converterVersion,
    const Data_PageNo&       rootPageNo)
    : FileDir_BaseEntry(allocator),
      m_persistentLock(0),
      m_persistentLockVersion(0),
      m_persistentData(0),
      m_leafPageCountDelta(0),
      m_indexPageCountDelta(0),
      m_entryCountDelta(0),
      m_needsUpdateAfterSavepoint(0),
      m_dataPage(0)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Entry::FileDir_Entry-fromSubclass",
                             FileDir_Trace, 6 );

    m_node.entry = this;

    m_persistentData = reinterpret_cast<PersistentData*>(
        FileDir_PageManager::Instance()->ReserveSpace(
            taskId,
            requiredSize,
            true,
            m_dataPage));

    if ( m_persistentData == 0 ) {
        return;
    }

    PersistentPointer()->header.converterVersion = converterVersion;
    PersistentPointer()->header.filler           = 0;
    PersistentPointer()->header.filler1          = 0;

    PersistentPointer()->fileNo         = fileNo;
    PersistentPointer()->fileState      = Data_FileOk;
    m_newPersistentState                = Data_FileOk;
    PersistentPointer()->fileAttributes = fAttrNone;
    PersistentPointer()->pdFiller       = 0;
    PersistentPointer()->rootPage       = rootPageNo;
    m_newRootPage                       = rootPageNo;
    PersistentPointer()->pdFiller1      = 0;
    PersistentPointer()->leafPageCount  = 0;
    PersistentPointer()->indexPageCount = 0;
    PersistentPointer()->entryCount     = 0;

    PersistentPointer()->header.hasBackref = false;

    //
    // entry is not yet set valid and page is still locked !!!
    //
}

/**************************************************************************/

FileDir_Entry::FileDir_Entry(
    SAPDBMem_IRawAllocator& allocator,
    FileDir_Entry& pageEntry)
    : FileDir_BaseEntry(allocator),
      m_persistentLock(0),
      m_persistentLockVersion(0),
      m_leafPageCountDelta(0),
      m_indexPageCountDelta(0),
      m_entryCountDelta(0),
      m_needsUpdateAfterSavepoint(0),
      m_persistentData(pageEntry.m_persistentData),
      m_dataPage(pageEntry.m_dataPage)
{
    m_node.entry         = this;
    m_newPersistentState =
        static_cast<Data_FileState>( PersistentPointer()->fileState );
    m_newRootPage        = PersistentPointer()->rootPage;
}

/**************************************************************************/

FileDir_Entry::~FileDir_Entry()
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Entry::~FileDir_Entry",
                             FileDir_Trace, 6 );
    if ( m_refCounter != 0 ) {
        if ( m_persistentData != 0 ) {
            FileDir_Exception errMsg(
                __CONTEXT__, FILEDIR_REFCOUNT_NOT_NULL,
                SAPDB_ToString( m_refCounter ),
                FileDir_FileNoToString( m_persistentData->fileNo ) );
            RTE_Crash(errMsg);
        } else {
            FileDir_Exception errMsg(
                __CONTEXT__, FILEDIR_REFCOUNT_NOT_NULL,
                SAPDB_ToString( m_refCounter ),
                SAPDB_ToString(0)  );
            RTE_Crash(errMsg);
        }
    }
}

/**************************************************************************/

FileDir_FileNo FileDir_Entry::GetFileNo() const
{
    SAPDBERR_ASSERT_STATE( m_persistentData != 0 );

    return m_persistentData->fileNo;
}

/**************************************************************************/

Data_FileState FileDir_Entry::GetFileState() const
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Entry::GetFileState",
                             FileDir_Trace, 6 );

    SAPDBERR_ASSERT_STATE( m_persistentData != 0 );

    if ( m_newPersistentState != PersistentPointer()->fileState ) {
        return m_newPersistentState;
    } else {
        return static_cast<Data_FileState>( PersistentPointer()->fileState );
    }
}

/**************************************************************************/

void FileDir_Entry::SetFileState(
    const Data_FileState newFileState)
{
    SAPDBERR_ASSERT_STATE( m_persistentData != 0 );

    if ( newFileState == this->GetFileState() ) {
        return;
    }

    RTE_TaskId taskId;
    vgetpid( taskId );

    FileDir_SharedDirectory& sharedFDir = FileDir_SharedDirectory::Instance();
    if ( FileDir_SharedDirectory::Instance().IsSavepointInProgress() ) {
        SAPDB_Bool deltaCounterUsed = false;
        SAPDB_Bool useWriteLock = ! m_needsUpdateAfterSavepoint;
        if ( useWriteLock ) {
            sharedFDir.WriteLockNeedUpdateList( taskId );
        } else {
            sharedFDir.ReadLockNeedUpdateList( taskId );
        }
        // now that we have the lock, we know that the file directory
        // converter version can be trusted
        if ( FileDir_SharedDirectory::Instance().IsSavepointInProgress() ) {
            m_newPersistentState = newFileState;
            deltaCounterUsed = true;
            if ( useWriteLock && (! m_needsUpdateAfterSavepoint) ) {
                m_needsUpdateAfterSavepoint = true;
                sharedFDir.AddToNeedUpdateList(
                    taskId, PersistentPointer()->fileNo,
                    FileDir_SharedDirectory::euaUpdateEntry );
            }
        }
        if ( useWriteLock ) {
            sharedFDir.WriteUnlockNeedUpdateList( taskId );
        } else {
            sharedFDir.ReadUnlockNeedUpdateList( taskId );
        }

        if ( deltaCounterUsed ) {
            return;
        }
    }

    //  delta value must be set before persistent part to avoid race
    // condition when querying
    m_newPersistentState        = newFileState;
    RTESys_WriteMemoryBarrier();
    m_dataPage->SetDirty();
    PersistentPointer()->fileState = newFileState;
}

/**************************************************************************/

Data_PageNo FileDir_Entry::GetRootPage() const
{
    SAPDBERR_ASSERT_STATE( m_persistentData != 0 );

    // reading a single value is always safe

    SAPDB_UInt4 rootPage = PersistentPointer()->rootPage;
    if ( m_newRootPage != rootPage ) {
        return m_newRootPage;
    } else {
        return Data_PageNo( rootPage );
    }
}

/**************************************************************************/

void FileDir_Entry::SetLeafPageCount(
    const SAPDB_UInt4 pageCount,
    const SAPDB_Bool  setInitialized)
{
    RTE_TaskId taskId;
    vgetpid( taskId );

    if ( ! setInitialized ) {
        Converter_Version converterVersion = 0;

        this->ModifyPageCounter(
            taskId,
            converterVersion,
            PersistentPointer()->leafPageCount,
            m_leafPageCountDelta,
            -(static_cast<SAPDB_Int4>(PersistentPointer()->leafPageCount)
              + m_leafPageCountDelta)
            + pageCount );
    } else {
        m_dataPage->LockPageForUpdate( taskId );
        this->PersistentPointer()->leafPageCount = pageCount;
        m_leafPageCountDelta                     = 0;
        m_dataPage->UnlockPageForUpdate( taskId );
    }
}

/**************************************************************************/

void FileDir_Entry::SetIndexPageCount(const SAPDB_UInt4 pageCount)
{
    RTE_TaskId taskId;
    vgetpid( taskId );
    Converter_Version converterVersion = 0;

    this->ModifyPageCounter(
        taskId,
        converterVersion,
        PersistentPointer()->indexPageCount,
        m_indexPageCountDelta,
        -(static_cast<SAPDB_Int4>(PersistentPointer()->indexPageCount)
          + m_indexPageCountDelta)
        + pageCount );
}

/**************************************************************************/

void FileDir_Entry::SetEntryCount(const SAPDB_UInt8 entryCount)
{
    RTE_TaskId taskId;
    vgetpid( taskId );
    Converter_Version converterVersion = 0;

    this->ModifyEntryCount(
        taskId,
        converterVersion,
        -(static_cast<SAPDB_Int8>(PersistentPointer()->entryCount)
          + m_entryCountDelta)
        + entryCount );
}

/**************************************************************************/

void
FileDir_Entry::InitializeBasicCounters(
    const RTE_TaskId  taskId,
    const SAPDB_UInt4 leafPageCount,
    const SAPDB_UInt4 indexPageCount,
    const SAPDB_UInt8 entryCount)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_Entry::InitializeBasicCounters", FileDir_Trace, 5 );

    const SAPDB_Bool bKeepPageLocked = true;
    this->InternalInitializeBasicCounters(
        taskId,
        leafPageCount,
        indexPageCount,
        entryCount,
        ! bKeepPageLocked );
}

/**************************************************************************/

void
FileDir_Entry::InternalInitializeBasicCounters(
    const RTE_TaskId  taskId,
    const SAPDB_UInt4 leafPageCount,
    const SAPDB_UInt4 indexPageCount,
    const SAPDB_UInt8 entryCount,
    const SAPDB_Bool  bKeepPageLocked)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_Entry::InternalInitializeBasicCounters", FileDir_Trace, 5 );

    m_dataPage->LockPageForUpdate( taskId );
    this->PersistentPointer()->leafPageCount  = leafPageCount;
    m_leafPageCountDelta                      = 0;
    this->PersistentPointer()->indexPageCount = indexPageCount;
    m_indexPageCountDelta                     = 0;
    this->PersistentPointer()->entryCount     = entryCount;
    m_entryCountDelta                         = 0;

    if ( ! bKeepPageLocked ) {
        m_dataPage->UnlockPageForUpdate( taskId );
    }
}

/**************************************************************************/

SAPDB_Bool FileDir_Entry::AreCountersInitialized() const
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Entry::AreCountersInitialized",
                             FileDir_Trace, 5 );

    return
        (this->PersistentPointer()->leafPageCount + m_leafPageCountDelta) != 0;
}

/**************************************************************************/

SAPDB_UInt8 FileDir_Entry::GetEntryCount(const RTE_TaskId taskId) const
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Entry::GetEntryCount",
                             FileDir_Trace, 5 );

    SAPDBERR_ASSERT_STATE( m_persistentData != 0 );

    // reading a single value is always safe
    return PersistentPointer()->entryCount + m_entryCountDelta;
}

/**************************************************************************/

SAPDB_UInt4 FileDir_Entry::GetLeafPageCount(const RTE_TaskId taskId) const
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Entry::GetLeafPageCount",
                             FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_persistentData != 0 );

    // reading a single value is always safe
    return PersistentPointer()->leafPageCount + m_leafPageCountDelta;
}

/**************************************************************************/

SAPDB_UInt4 FileDir_Entry::GetIndexPageCount(const RTE_TaskId taskId) const
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Entry::GetIndexPageCount",
                             FileDir_Trace, 5 );

    SAPDBERR_ASSERT_STATE( m_persistentData != 0 );

    // reading a single value is always safe
    return PersistentPointer()->indexPageCount + m_indexPageCountDelta;
}

/**************************************************************************/

void FileDir_Entry::SetPersistentDataAddress(
    FileDir_Page* dataPage,
    char* address)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Entry::SetPersistentDataAddress",
                             FileDir_Trace, 6 );

    m_dataPage       = dataPage;
    m_persistentData = reinterpret_cast<PersistentData*>(address);
}

/**************************************************************************/

void FileDir_Entry::ModifyPageCounter(
    const RTE_TaskId        taskId,
    const Converter_Version converterVersion,
    PageCount&              pageCounter,
    SAPDB_Int4&             deltaCounter,
    const SAPDB_Int4        delta)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Entry::ModifyPageCounter",
                             FileDir_Trace, 6 );

    SAPDBERR_ASSERT_STATE( m_persistentData != 0 );

    if ( ! this->AreCountersInitialized() ) {
        return;
    }

    if ( m_persistentData == 0 ) {
        FileDir_Exception errMsg(
            __CONTEXT__, FILEDIR_REFCOUNT_NOT_NULL_REMOVE,
            SAPDB_ToString(m_refCounter),
            FileDir_FileNoToString( m_persistentData->fileNo ) );
        RTE_Crash(errMsg);
    }

    FileDir_SharedDirectory& sharedFDir = FileDir_SharedDirectory::Instance();
    if ( ! this->IsWriteToPageAllowed(taskId, converterVersion) ) {
        SAPDB_Bool deltaCounterUsed = false;
        SAPDB_Bool useWriteLock = ! m_needsUpdateAfterSavepoint;
        if ( useWriteLock ) {
            sharedFDir.WriteLockNeedUpdateList( taskId );
        } else {
            sharedFDir.ReadLockNeedUpdateList( taskId );
        }
        // now that we have the lock, we know that the file directory
        // converter version can be trusted
        if ( ! this->IsWriteToPageAllowed(taskId, converterVersion) ) {
            RTESys_AtomicModify( deltaCounter, delta );
            deltaCounterUsed = true;
            if ( useWriteLock && (! m_needsUpdateAfterSavepoint) ) {
                m_needsUpdateAfterSavepoint = true;
                sharedFDir.AddToNeedUpdateList(
                    taskId,
                    PersistentPointer()->fileNo,
                    FileDir_SharedDirectory::euaUpdateEntry );
            }
        }
        if ( useWriteLock ) {
            sharedFDir.WriteUnlockNeedUpdateList( taskId );
        } else {
            sharedFDir.ReadUnlockNeedUpdateList( taskId );
        }

        if ( deltaCounterUsed ) {
            return;
        }
    }

    // this check is dirty and might skew the counter in a worst
    // case scenario
    m_dataPage->SetDirty();
    if ( ( delta < 0 ) &&
         ( pageCounter <
           static_cast<PageCount>( -delta ) ) ) {
        RTESys_AtomicWrite(
            pageCounter,
            static_cast<PageCount>( 0 ) );
    } else {
        RTESys_AtomicModify( pageCounter, delta );
    }
}

/**************************************************************************/

void FileDir_Entry::ModifyLeafPageCount(
    const RTE_TaskId taskId,
    const Converter_Version converterVersion,
    const SAPDB_Int4 delta)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Entry::ModifyLeafPageCount",
                             FileDir_Trace, 6 );

    this->ModifyPageCounter(
        taskId,
        converterVersion,
        PersistentPointer()->leafPageCount,
        m_leafPageCountDelta,
        delta);
}

/**************************************************************************/

void FileDir_Entry::ModifyIndexPageCount(
    const RTE_TaskId taskId,
    const Converter_Version converterVersion,
    const SAPDB_Int4 delta)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Entry::ModifyIndexPageCount",
                             FileDir_Trace, 6 );

    this->ModifyPageCounter(
        taskId,
        converterVersion,
        PersistentPointer()->indexPageCount,
        m_indexPageCountDelta,
        delta);
}

/**************************************************************************/

void FileDir_Entry::ModifyEntryCount(
    const RTE_TaskId        taskId,
    const Converter_Version converterVersion,
    const SAPDB_Int8        delta)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Entry::ModifyEntryCount",
                             FileDir_Trace, 6 );

    SAPDBERR_ASSERT_STATE( m_persistentData != 0 );

    if ( ! this->AreCountersInitialized() ) {
        return;
    }

    if ( m_persistentData == 0 ) {
        FileDir_Exception errMsg(
            __CONTEXT__, FILEDIR_REFCOUNT_NOT_NULL_REMOVE,
            SAPDB_ToString(m_refCounter),
            FileDir_FileNoToString( PersistentPointer()->fileNo ) );
        RTE_Crash(errMsg);
    }

    FileDir_SharedDirectory& sharedFDir = FileDir_SharedDirectory::Instance();
    if ( ! this->IsWriteToPageAllowed(taskId, converterVersion) ) {
        SAPDB_Bool deltaCounterUsed = false;
        SAPDB_Bool useWriteLock = ! m_needsUpdateAfterSavepoint;
        if ( useWriteLock ) {
            sharedFDir.WriteLockNeedUpdateList( taskId );
        } else {
            sharedFDir.ReadLockNeedUpdateList( taskId );
        }
        // now that we have the lock, we know that the file directory
        // converter version can be trusted
        if ( ! this->IsWriteToPageAllowed(taskId, converterVersion) ) {
            RTESys_AtomicModify(
                m_entryCountDelta, static_cast<SAPDB_Int4>( delta ) );
            deltaCounterUsed = true;
            if ( useWriteLock && (! m_needsUpdateAfterSavepoint) ) {
                m_needsUpdateAfterSavepoint = true;
                sharedFDir.AddToNeedUpdateList(
                    taskId,
                    PersistentPointer()->fileNo,
                    FileDir_SharedDirectory::euaUpdateEntry );
            }
        }
        if ( useWriteLock ) {
            sharedFDir.WriteUnlockNeedUpdateList( taskId );
        } else {
            sharedFDir.ReadUnlockNeedUpdateList( taskId );
        }

        if ( deltaCounterUsed ) {
            return;
        }
    }

    // this check is dirty and might skew the counter in a worst
    // case scenario
    m_dataPage->SetDirty();
    if ( ( delta < 0) &&
         ( PersistentPointer()->entryCount <
           static_cast<EntryCount>( -delta ) ) ) {
        RTESys_AtomicWrite(
            PersistentPointer()->entryCount,
            static_cast<EntryCount>( 0 ) );
    } else {
        RTESys_AtomicModify( PersistentPointer()->entryCount, delta );
    }
}

/**************************************************************************/

void FileDir_Entry::Remove(const RTE_TaskId taskId)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Entry::Remove",
                             FileDir_Trace, 6 );

    // need not be atomic, only GC should be able to call this
    if ( m_persistentData != 0 ) {
        if ( m_refCounter != 0 ) {
            FileDir_Exception errMsg(
                __CONTEXT__, FILEDIR_REFCOUNT_NOT_NULL_REMOVE,
                SAPDB_ToString(m_refCounter),
                FileDir_FileNoToString( PersistentPointer()->fileNo ) );
            RTE_Crash(errMsg);
        }

        m_dataPage->LockPageForUpdate( taskId );
        this->SetInvalid();
        m_dataPage->FreeEntry(
            taskId,
            reinterpret_cast<char*>(PersistentPointer()) );
        m_dataPage->UnlockPageForUpdate( taskId );
        m_persistentData = 0;
    }
}

/**************************************************************************/

void FileDir_Entry::Restart_SetHasNoBackref()
{
    if ( !m_persistentData->header.hasBackref ) {
        return;
    }

    m_persistentData->header.hasBackref = false;
    m_dataPage->Restart_ShrinkEntry(
        reinterpret_cast<char*>( m_persistentData ),
        sizeof(Backref) );
}

/**************************************************************************/

void FileDir_Entry::SetValid()
{
    // !!! m_dataPage must be locked !!!
    if ( m_dataPage->IsValid(
             reinterpret_cast<char*>(PersistentPointer())) ) {
        return;
    }

    m_dataPage->SetDirty();
    m_dataPage->SetValid( reinterpret_cast<char*>(PersistentPointer()) );
}

/**************************************************************************/

void FileDir_Entry::SetInvalid()
{
    // !!! m_dataPage must be locked !!!
    if ( !m_dataPage->IsValid(
             reinterpret_cast<char*>(PersistentPointer())) ) {
        return;
    }

    m_dataPage->SetDirty();
    m_dataPage->SetInvalid( reinterpret_cast<char*>(PersistentPointer()) );
}

/**************************************************************************/

void FileDir_Entry::SetLazyDelete(const RTE_TaskId taskId)
{
    if ( m_dataPage->IsLazyDeleteSet(
             reinterpret_cast<char*>(PersistentPointer())) ) {
        return;
    }

    m_dataPage->LockPageForUpdate( taskId );
    m_dataPage->SetLazyDelete(
        reinterpret_cast<char*>(PersistentPointer()) );
    m_dataPage->UnlockPageForUpdate( taskId );
}

/**************************************************************************/

SAPDB_Bool FileDir_Entry::IsLazyDeleteSet()
{
    return m_dataPage->IsLazyDeleteSet(
        reinterpret_cast<char*>(PersistentPointer()));
}

/**************************************************************************/

void FileDir_Entry::CopyBody(PersistentData* dst, PersistentData* src)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Entry::CopyBody",
                             FileDir_Trace, 6 );

    int byteCount =
        (dst->header.recordHeader.length > src->header.recordHeader.length)
        ?
        (src->header.recordHeader.length - sizeof(EntryHeader))
        :
        (dst->header.recordHeader.length - sizeof(EntryHeader));

    SAPDBErr_MessageList message;
    SAPDB_MemMove(
        reinterpret_cast<char*>(dst) + sizeof(EntryHeader),
        reinterpret_cast<char*>(src) + sizeof(EntryHeader),
        byteCount,
        "FileDirectory", __FILE__, __LINE__,
        message);
}

/**************************************************************************/

SAPDB_Bool FileDir_Entry::IsPersistent() const
{
    return this->GetFileNo().IsPersistentFileNo();
}

/**************************************************************************/

void FileDir_Entry::RechainPersistentPointer(
    const RTE_TaskId taskId,
    FileDir_Page* newDataPage,
    PersistentData* newAddress)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Entry::RechainPersistentPointer",
                             FileDir_Trace, 5 );

    FileDir_Exception errMsg(
        __CONTEXT__, FILEDIR_REFCOUNT_NOT_NULL,
        SAPDB_ToString( m_refCounter ),
        FileDir_FileNoToString( m_persistentData->fileNo ) );
    RTE_Crash(errMsg);

//     m_rechainAndDeltaLock.AcquireWriteLock( taskId );
    this->SetPersistentLock();

    m_dataPage->LockPageForUpdate( taskId );
    SAPDBERR_ASSERT_STATE(
        m_dataPage->IsValid(reinterpret_cast<char*>(PersistentPointer())) );
    SetInvalid();                     // invalidate old entry

    // set pointer to new location
    RTESys_WriteMemoryBarrier();
    m_persistentData = newAddress;

    // SetValid() uses m_dataPage, so save old page
    FileDir_Page* oldDataPage = m_dataPage;
    m_dataPage = newDataPage;
    SetValid();                                // this is already the new entry
    oldDataPage->UnlockPageForUpdate( taskId ); // unlock old data page

    this->ClearPersistentLock();
//     m_rechainAndDeltaLock.ReleaseWriteLock( taskId );
}

/**************************************************************************/

Converter_Version FileDir_Entry::GetEntryConverterVersion() {
    SAPDB_UInt4 cv = m_persistentData->header.converterVersion;
    return Converter_Version(cv);
}

/**************************************************************************/

SAPDB_Bool FileDir_Entry::MoveEntry()
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Entry::MoveEntry", FileDir_Trace, 5 );

    RTE_TaskId taskId;
    vgetpid( taskId );

    FileDir_Page* oldDataPage = m_dataPage;
    PersistentData* oldRecord = m_persistentData;

    FileDir_Page* newDataPage;
    // DDL - no internal locking should be necessary
    PersistentData* newRecord = reinterpret_cast<PersistentData*>(
        FileDir_PageManager::Instance()->ReserveSpace(
            taskId,
            PersistentPointer()->header.recordHeader.length,
            true,
            newDataPage,
            m_dataPage->PageNo()));

    if ( newRecord == 0 ) {
        return false;
    }

    CopyBody( newRecord, PersistentPointer() );

    newRecord->header.converterVersion =
        PersistentPointer()->header.converterVersion;

    // copy backref of original entry if it exists
    newRecord->header.hasBackref =
        PersistentPointer()->header.hasBackref;
    if ( newRecord->header.hasBackref ) {
        Backref* br = GetBackrefAddress();
        Backref* newbr               = GetBackrefAddress(newRecord);
        newbr->backrefPNo            = br->backrefPNo;
        newbr->backrefEntryLocation  = br->backrefEntryLocation;
    }

    RechainPersistentPointer( taskId, newDataPage, newRecord );

    m_dataPage->UnlockPageForUpdate( taskId ); // this unlocks the 'new' data page

    // copy is complete, now remove old entry:
    oldDataPage->LockPageForUpdate( taskId );
    oldDataPage->FreeEntry( taskId, reinterpret_cast<char*>(oldRecord) );
    oldDataPage->UnlockPageForUpdate( taskId );

    return true;
}

/**************************************************************************/

/*!
 * @warning newRecord needs to be locked for update!
 */
void FileDir_Entry::AddBackrefToNewRecord(PersistentData* newRecordData) const
{
    newRecordData->header.hasBackref = true;

    Backref* br = GetBackrefAddress(newRecordData);
    br->backrefPNo           = m_dataPage->PageNo();
    br->backrefEntryLocation = m_dataPage->GetEntryLocation(m_persistentData);
}

/**************************************************************************/

SAPDB_UInt2 FileDir_Entry::GetBackrefOffset() const
{
    return GetBackrefOffset( m_persistentData );
}

/**************************************************************************/

SAPDB_UInt2 FileDir_Entry::GetBackrefOffset(
    PersistentData* persistentData) const
{
    SAPDB_UInt2 off = static_cast<SAPDB_UInt2>(
        persistentData->header.recordHeader.length
        - sizeof(Backref));
    return off;
}

/**************************************************************************/

SAPDB_Int FileDir_Entry::GetBackrefPageNumber() const {
    SAPDBERR_ASSERT_ARGUMENT(m_persistentData->header.hasBackref);

    return
        reinterpret_cast<Backref*>(
            reinterpret_cast<char*>(m_persistentData)
            + GetBackrefOffset())
        ->backrefPNo;
}

/**************************************************************************/

SAPDB_Int FileDir_Entry::GetBackrefEntryLocation() const
{
    SAPDBERR_ASSERT_ARGUMENT(m_persistentData->header.hasBackref);

    return
        reinterpret_cast<Backref*>(
            reinterpret_cast<char*>(m_persistentData)
            + GetBackrefOffset())
        ->backrefEntryLocation;
}

/**************************************************************************/

FileDir_Entry::Backref* FileDir_Entry::GetBackrefAddress() const
{
    return GetBackrefAddress( m_persistentData );
}

/**************************************************************************/

FileDir_Entry::Backref* FileDir_Entry::GetBackrefAddress(
    PersistentData* persistentData) const
{
    if (persistentData->header.hasBackref) {
        Backref* br = reinterpret_cast<Backref*>(
            reinterpret_cast<char*>(persistentData)
            + GetBackrefOffset(persistentData));
        return br;
    }
    return 0;
}

/**************************************************************************/

SAPDB_Bool FileDir_Entry::IsReadOnly() const
{
    return this->GetFileState() == Data_FileDeleted;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_Entry::MarkFileDeleted(
    const tgg91_TransNo& transNo)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_Entry::MarkFileDeleted", FileDir_Trace, 5 );

    // not synchronized!
    if ( g01vtrace.vtrAny_gg00 ) {
        Kernel_VTrace() << "MarkFileDeleted "
                        << this->GetFileNo()
                        << ", " << this->GetRootPage();
    }

    if ( this->GetFileState() == Data_FileDeleted ) {
        return FileDir_AlreadyMarkedForDeletion;
    }

    RTE_TaskId taskId;
    vgetpid( taskId );

    FileDir_SharedDirectory& sharedFDir = FileDir_SharedDirectory::Instance();
    if ( FileDir_SharedDirectory::Instance().IsSavepointInProgress() ) {
        SAPDB_Bool deltaCounterUsed = false;
        SAPDB_Bool useWriteLock = ! m_needsUpdateAfterSavepoint;
        if ( useWriteLock ) {
            sharedFDir.WriteLockNeedUpdateList( taskId );
        } else {
            sharedFDir.ReadLockNeedUpdateList( taskId );
        }
        // now that we have the lock, we know that the file directory
        // converter version can be trusted
        if ( FileDir_SharedDirectory::Instance().IsSavepointInProgress() ) {
            m_newPersistentState = Data_FileDeleted;
            deltaCounterUsed = true;
            if ( FileDir_Trace.TracesLevel( 1 ) ) {
                Kernel_VTrace() << "MarkFileDeleted: delay persistent change "
                                << this->GetFileNo()
                                << ", " << this->GetRootPage();
            }
            if ( useWriteLock && (! m_needsUpdateAfterSavepoint) ) {
                m_needsUpdateAfterSavepoint = true;
                sharedFDir.AddToNeedUpdateList(
                    taskId,
                    PersistentPointer()->fileNo,
                    FileDir_SharedDirectory::euaUpdateEntry );
            }
        }
        if ( useWriteLock ) {
            sharedFDir.WriteUnlockNeedUpdateList( taskId );
        } else {
            sharedFDir.ReadUnlockNeedUpdateList( taskId );
        }

        if ( deltaCounterUsed ) {
            return FileDir_Okay;
        }
    }

    SetFileState( Data_FileDeleted );
    return FileDir_Okay;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_Entry::RemoveFileDeletedMark()
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_Entry::RemoveFileDeletedMark", FileDir_Trace, 5 );


    if ( g01vtrace.vtrAny_gg00 ) {
        Kernel_VTrace() << "RemoveFileDeletedMark "
                        << this->GetFileNo()
                        << ", " << this->GetRootPage();
    }

    RTE_TaskId taskId;
    vgetpid( taskId );

    FileDir_SharedDirectory& sharedFDir = FileDir_SharedDirectory::Instance();
    if ( FileDir_SharedDirectory::Instance().IsSavepointInProgress() ) {
        SAPDB_Bool deltaCounterUsed = false;
        SAPDB_Bool useWriteLock = ! m_needsUpdateAfterSavepoint;
        if ( useWriteLock ) {
            sharedFDir.WriteLockNeedUpdateList( taskId );
        } else {
            sharedFDir.ReadLockNeedUpdateList( taskId );
        }
        // now that we have the lock, we know that the file directory
        // converter version can be trusted
        if ( FileDir_SharedDirectory::Instance().IsSavepointInProgress() ) {
            m_newPersistentState = Data_FileOk;
            deltaCounterUsed = true;
            if ( FileDir_Trace.TracesLevel( 1 ) ) {
                Kernel_VTrace() << "RemoveFileDeletedMark: delay persistent change "
                                << this->GetFileNo()
                                << ", " << this->GetRootPage();
            }
            if ( useWriteLock && (! m_needsUpdateAfterSavepoint) ) {
                m_needsUpdateAfterSavepoint = true;
                sharedFDir.AddToNeedUpdateList(
                    taskId,
                    PersistentPointer()->fileNo,
                    FileDir_SharedDirectory::euaUpdateEntry );
            }
        }
        if ( useWriteLock ) {
            sharedFDir.WriteUnlockNeedUpdateList( taskId );
        } else {
            sharedFDir.ReadUnlockNeedUpdateList( taskId );
        }

        if ( deltaCounterUsed ) {
            return FileDir_Okay;
        }
    }

    this->SetFileState( Data_FileOk );

    return FileDir_Okay;
}

/**************************************************************************/

void FileDir_Entry::GetDeleteStamp(tgg91_TransNo& deleteStamp) const
{
    gg06SetNilTrans( deleteStamp );
}

/**************************************************************************/

void FileDir_Entry::UpdateAfterSavepoint(
    const RTE_TaskId taskId,
    const Converter_Version& converterVersion)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Entry::UpdateAfterSavepoint",
                             FileDir_Trace, 6 );

    /*
     * No SQL-locks are set here, so use extra lock to prevent
     * rechaining during update.
     *
     * This lock also prevents the loss of any deltas in the quite
     * unlikely case that a statistics counter still thinks it is
     * being updated during the last savepoint and tries to use a
     * delta counter while UpdateAfterSavepoint() is already being
     * called and resetting the delta counters...
     */
//    m_rechainAndDeltaLock.AcquireWriteLock( taskId );

    m_dataPage->SetDirty();

    m_needsUpdateAfterSavepoint = false;

    if ( m_leafPageCountDelta != 0 ) {
        this->ModifyLeafPageCount(
            taskId, converterVersion, m_leafPageCountDelta );
        m_leafPageCountDelta  = 0;
    }

    if ( m_indexPageCountDelta != 0 ) {
        this->ModifyIndexPageCount(
            taskId, converterVersion, m_indexPageCountDelta );
        m_indexPageCountDelta  = 0;
    }

    if ( m_entryCountDelta != 0 ) {
        this->ModifyEntryCount(
            taskId, converterVersion, m_entryCountDelta );
        m_entryCountDelta = 0;
    }

    PersistentPointer()->fileState = m_newPersistentState;
    this->SetRootPage( taskId, m_newRootPage );

//    m_rechainAndDeltaLock.ReleaseWriteLock( taskId );
}

/**************************************************************************/

FileDir_ReturnCode FileDir_Entry::Initialize()
{
//     if ( !m_rechainAndDeltaLock.Initialize() ) {
//         return FileDir_OutOfMemory;
//     }
    return FileDir_Okay;
}

/**************************************************************************/

void FileDir_Entry::AddToNeedUpdateList(const RTE_TaskId taskId)
{
    if ( !m_needsUpdateAfterSavepoint ) {
        // setting m_needsUpdateAfterSavepoint is not properly
        // synchronized, so an entry may show up several times in the
        // needUpdate list
        m_needsUpdateAfterSavepoint = true;
        FileDir_SharedDirectory::Instance().AddToNeedUpdateList(
            taskId,
            PersistentPointer()->fileNo,
            FileDir_SharedDirectory::euaUpdateEntry );
    }
}

/**************************************************************************/

bool FileDir_Entry::IsWriteToPageAllowed(
    const RTE_TaskId taskId,
    const Converter_Version converterVersion) const
{
    // this is a workaround until the correct converter version will
    // be passed down to us so we can reliably compare the converter
    // version of the change to the converter version of the file
    // directory; until then we do not allow writes to pages during
    // the whole savepoint phase
    return ! FileDir_SharedDirectory::Instance().IsSavepointInProgress();

    // this is how things should go once we can rely on converterVersion:

//     Converter_Version fileDirConverterVersion =
//         FileDir_SharedDirectory::Instance().GetConverterVersion();

//     if ( abs(static_cast<SAPDB_Int>(converterVersion-fileDirConverterVersion))
//          > 1 ) {
//         FileDir_Exception warningMsg(
//             __CONTEXT__, FILEDIR_CONVERTER_VERSION_OUT_OF_SYNC,
//             SAPDB_ToString(fileDirConverterVersion),
//             SAPDB_ToString(converterVersion) );
//         RTE_Message( warningMsg );
//     }

//     if ( converterVersion < fileDirConverterVersion ) {
//         FileDir_Exception warningMsg(
//             __CONTEXT__, FILEDIR_CONVERTER_VERSION_TOO_SMALL,
//             SAPDB_ToString(converterVersion) );
//         RTE_Message( warningMsg );
//         /*
//          * having too small a converter version implies that the
//          * change to the data page and update of the file directory
//          * did not happen atomically; therefore it cannot be ruled out
//          * that the page is being flushed to disk at this time -> do
//          * not write to page
//          */
//         return false;
//     }

//     /*
//      * To avoid having to synchronize the converter version within the
//      * file directory, we accept that a delta counter will be used if
//      * the converter version is being changed while we are working
//      * here; to make sure we are not losing any delta, an update of
//      * the persistent entries should be done sufficiently late after
//      * changing the converter version of the file directory (e.g. at
//      * the beginning of the next savepoint). However, without proper
//      * synchronisation it can never be ruled out that a delta will be
//      * applied in too high a converter version, although the
//      * probability for this to occur should be arbitrarily small.
//      */
//     return
//         m_persistentData->fileId.IsTempFileNo()
//         ||
//         (converterVersion <= fileDirConverterVersion );
}

/**************************************************************************/

SAPDB_Bool FileDir_Entry::FillDumpEntry(
    const RTE_TaskId taskId,
    SAPDBFields_Field& dumpBuffer,
    SAPDB_Int& startPos)
{
    struct Dump {
        FileDir_FileNo     fileNo;
        Data_PageNo        rootPage;
        Data_PageNo        pageNo;
        EntryCount         entryCount;
        PageCount          leafPageCount;
        PageCount          indexPageCount;
        SAPDB_UInt1        fileState;
        SAPDB_UInt1        fileType;
        SAPDB_UInt1        fileAttr;
        SAPDB_Int1         filler;
        SAPDB_Int4         filler1;
        SAPDB_Bool         isPerm;
        SAPDB_Bool         needsUpdate;
        SAPDB_Int2         filler2;
        SAPDB_Int4         leafCountDelta;
        SAPDB_Int4         indexCountDelta;
        SAPDB_Int4         entryCountDelta;
        CacheAccessCounter cacheHits;
        CacheAccessCounter cacheMisses;
    };
    Dump dump;

    dump.fileType        = m_persistentData->fileType;
    dump.fileAttr        = m_persistentData->fileAttributes;
    dump.fileNo          = m_persistentData->fileNo;
    dump.rootPage        = this->GetRootPage();
    dump.entryCount      = m_persistentData->entryCount;
    dump.leafPageCount   = m_persistentData->leafPageCount;
    dump.indexPageCount  = m_persistentData->indexPageCount;
    dump.fileState       = m_persistentData->fileState;
    // cache hit counters are not yet in use and undefined to save memory
//     dump.cacheHits       = m_cacheHitCounter;
//     dump.cacheMisses     = m_cacheMissCounter;
    dump.cacheHits       = 0;
    dump.cacheMisses     = 0;
    dump.entryCountDelta = m_entryCountDelta;
    dump.leafCountDelta  = m_leafPageCountDelta;
    dump.indexCountDelta = m_indexPageCountDelta;
    dump.pageNo          = m_dataPage->PageNo();
    dump.needsUpdate     = m_needsUpdateAfterSavepoint;
    dump.isPerm          = m_persistentData->fileNo.IsPersistentFileNo();
    dump.filler          = 0;
    dump.filler1         = 0;
    dump.filler2         = 0;

    if ( static_cast<SAPDB_Int>(dumpBuffer.GetLength())
          - startPos - sizeof(dump) <= 0 ) {
        return false;
    }

    void* pointer = dumpBuffer.GetPointer( startPos, sizeof(dump) );
    SAPDB_MemCopyNoCheck ( pointer, &dump, sizeof(dump) );
    startPos += sizeof(dump);

    return true;
}

/**************************************************************************/

void FileDir_Entry::SetRootPage(
    const RTE_TaskId taskId,
    const Data_PageNo& rootPage)
{

    FileDir_SharedDirectory& sharedFDir = FileDir_SharedDirectory::Instance();
    if ( FileDir_SharedDirectory::Instance().IsSavepointInProgress() ) {
        SAPDB_Bool deltaCounterUsed = false;
        SAPDB_Bool useWriteLock = ! m_needsUpdateAfterSavepoint;
        if ( useWriteLock ) {
            sharedFDir.WriteLockNeedUpdateList( taskId );
        } else {
            sharedFDir.ReadLockNeedUpdateList( taskId );
        }
        // now that we have the lock, we know that the file directory
        // converter version can be trusted
        if ( FileDir_SharedDirectory::Instance().IsSavepointInProgress() ) {
            m_newRootPage = rootPage;
            deltaCounterUsed = true;
            if ( useWriteLock && (! m_needsUpdateAfterSavepoint) ) {
                m_needsUpdateAfterSavepoint = true;
                sharedFDir.AddToNeedUpdateList(
                    taskId,
                    PersistentPointer()->fileNo,
                    FileDir_SharedDirectory::euaUpdateEntry );
            }
        }
        if ( useWriteLock ) {
            sharedFDir.WriteUnlockNeedUpdateList( taskId );
        } else {
            sharedFDir.ReadUnlockNeedUpdateList( taskId );
        }

        if ( deltaCounterUsed ) {
            return;
        }
    }

    m_newRootPage = rootPage; // must be set before persistent part to
                              // avoid race condition when querying
    m_dataPage->LockPageForUpdate( taskId );
    PersistentPointer()->rootPage = rootPage;
    m_dataPage->UnlockPageForUpdate( taskId );
}

/**************************************************************************/

void
FileDir_Entry::SetPagesClustered(
    const RTE_TaskId taskId,
    const SAPDB_Bool pagesClustered)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_Entry::SetPagesClustered", FileDir_Trace, 5 );

    m_dataPage->LockPageForUpdate( taskId );

    if ( pagesClustered ) {
        PersistentPointer()->fileAttributes |= fAttrPagesClustered;
    } else {
        PersistentPointer()->fileAttributes &= ! fAttrPagesClustered;
    }

    m_dataPage->UnlockPageForUpdate( taskId );
}

/**************************************************************************/

SAPDB_Bool
FileDir_Entry::ArePagesClustered() const
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_Entry::ArePagesClustered", FileDir_Trace, 5 );

    return
        (PersistentPointer()->fileAttributes & fAttrPagesClustered) != 0;
}

/**************************************************************************/

void FileDir_Entry::BuildFileId(tgg00_FileId& fileId) const
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_Entry::BuildFileId", FileDir_Trace, 5 );

    fileId.fileRootCheck_gg00() = ROOT_CHECK_GG00;
    fileId.fileLeafNodes_gg00() = cgg_nil_leafnodes;
    fileId.fileUserRef_gg00().gg91SetNilRef ();
    fileId.fileVersion_gg00().gg91SetDummyVers();
    fileId.fileRoot_gg00()      = this->GetRootPage();
    fileId.fileHandling_gg00().clear();
    fileId.fileBdUse_gg00().clear();
    fileId.fileType_gg00().clear();

    fileId.fileName_gg00().rawAssign( cgg_zero_fn );
    fileId.fileTabId_gg00().rawAssign( this->GetFileNo().GetCharArray() );

    bd300SetRootCheck( fileId );
}

/**************************************************************************/

void FileDir_Entry::FillPreparedFileId(tgg00_FileId& fileId) const
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_Entry::FillPreparedFileId", FileDir_Trace, 5 );

    fileId.fileRoot_gg00() = this->GetRootPage();
    fileId.fileType_gg00().clear();

    if ( this->GetRootPage().IsValid() ) {
        bd300SetRootCheck( fileId );
    } else {
        fileId.fileRootCheck_gg00() = ROOT_CHECK_GG00;
    }
}

/**************************************************************************/

FileDir_Entry::FileDir_Entry(
    const RTE_TaskId        taskId,
    SAPDBMem_IRawAllocator& allocator,
    FileDir_Entry&          persistentEntry,
    const SAPDB_UInt2       requiredSize,
    FileDir_EntryLayouts    currentPersistentLayout,
    SAPDB_Byte*&            curPersistentPos)
     : FileDir_BaseEntry(allocator),
      m_persistentLock(0),
      m_persistentLockVersion(0),
      m_persistentData(0),
      m_leafPageCountDelta(0),
      m_indexPageCountDelta(0),
      m_entryCountDelta(0),
      m_needsUpdateAfterSavepoint(0),
      m_dataPage(0)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Entry::FileDir_Entry-migration",
                             FileDir_Trace, 6 );

    m_node.entry = this;

    m_persistentData = reinterpret_cast<PersistentData*>(
        FileDir_PageManager::Instance()->RestartReserveSpace(
            taskId,
            requiredSize,
            true,
            m_dataPage));

    if ( m_persistentData == 0 ) {
        return;
    }

    curPersistentPos =
        reinterpret_cast<SAPDB_Byte*>( persistentEntry.PersistentPointer() );

    // contains all fields that have ever been in use by this entry
    struct MigrationHelpStruct {
        SAPDB_Bool     hdHasBackref;
        SAPDB_Bool     hdFiller;
        SAPDB_Int2     hdFiller1;
        SAPDB_UInt4    hdConverterVersion;
        SAPDB_UInt1    fileType;    ///< File type.
        SAPDB_UInt1    fileState;   ///< File state of this entry.
        SAPDB_UInt1    fileAttributes; ///< Filler.
        FileDir_FileNo fileNo;      ///< File no.
        SAPDB_UInt4    rootPage;    ///< Root page of this entry.
        EntryCount     entryCount;  ///< Number of entries.
        PageCount      leafPageCount; ///< Number of leaf pages of this entry.
        PageCount      indexPageCount; ///< Number of index pages of this entry.
    };

    MigrationHelpStruct helpStruct;
    SAPDB_Bool bPersistentDataParsed = false;
    SAPDB_Bool bNoMigration = true;

    if ( currentPersistentLayout < fdelWithFileAttributes ) {
        // migrate to layout that contains file attributes
        bNoMigration = false;

        if ( ! bPersistentDataParsed ) {
            struct LayoutWithoutFileAttributes {
                RecordHeader   recordHeader; // needed to have correct alignment
                SAPDB_Bool     hdHasBackref;
                SAPDB_Bool     hdFiller;
                SAPDB_Int2     hdFiller1;
                SAPDB_UInt4    hdConverterVersion;
                Data_FileType  fileType;
                FileDir_FileNo fileNo;
                SAPDB_UInt4    rootPage;
                Data_FileState fileState;
                EntryCount     entryCount;
                PageCount      leafPageCount;
                PageCount      indexPageCount;
            };

            LayoutWithoutFileAttributes* oldLayout =
                reinterpret_cast<LayoutWithoutFileAttributes*>(
                    curPersistentPos );

            helpStruct.hdHasBackref       = oldLayout->hdHasBackref;
            helpStruct.hdConverterVersion = oldLayout->hdConverterVersion;
            helpStruct.fileType           =
                static_cast<SAPDB_UInt1>( oldLayout->fileType );
            helpStruct.fileState          =
                static_cast<SAPDB_UInt1>( oldLayout->fileState );
            helpStruct.fileNo             = oldLayout->fileNo;
            helpStruct.rootPage           = oldLayout->rootPage;
            helpStruct.entryCount         = oldLayout->entryCount;
            helpStruct.leafPageCount      = oldLayout->leafPageCount;
            helpStruct.indexPageCount     = oldLayout->indexPageCount;

            bPersistentDataParsed = true;
            curPersistentPos     += sizeof( LayoutWithoutFileAttributes );
        }

        // new entry fileAttributes
        helpStruct.fileAttributes     = fAttrNone;
    }

    if ( bNoMigration ) {

        // just copy content if there is nothing to migrate, but do not
        // overwrite new record header
        SAPDB_MemCopyNoCheck(
            reinterpret_cast<char*>( m_persistentData )
            + FileDir_Entry::GetFirstByteOffset(),
            reinterpret_cast<char*>( persistentEntry.m_persistentData )
            + FileDir_Entry::GetFirstByteOffset(),
            sizeof( PersistentData ) - FileDir_Entry::GetFirstByteOffset());

        curPersistentPos += sizeof( PersistentData );

    } else {

        PersistentPointer()->header.hasBackref       =
            helpStruct.hdHasBackref;
        PersistentPointer()->header.filler           = 0;
        PersistentPointer()->header.filler1          = 0;
        PersistentPointer()->header.converterVersion =
            helpStruct.hdConverterVersion;
        PersistentPointer()->fileType                = helpStruct.fileType;
        PersistentPointer()->fileState               = helpStruct.fileState;
        PersistentPointer()->fileAttributes          =
            helpStruct.fileAttributes;
        PersistentPointer()->pdFiller                = 0;
        PersistentPointer()->fileNo                  = helpStruct.fileNo;
        PersistentPointer()->rootPage                = helpStruct.rootPage;
        PersistentPointer()->pdFiller1               = 0;
        PersistentPointer()->entryCount              = helpStruct.entryCount;
        PersistentPointer()->leafPageCount           = helpStruct.leafPageCount;
        PersistentPointer()->indexPageCount          = helpStruct.indexPageCount;

    }

    // init variables
    m_newPersistentState =
        static_cast<Data_FileState>( PersistentPointer()->fileState );
    m_newRootPage        = PersistentPointer()->rootPage;

    //
    // entry is not yet set valid and page is still locked !!!
    //
}

/**************************************************************************/

Data_FileType
FileDir_Entry::GetFileType(
    const FileDir_EntryLayouts currentPersistentLayout) const
{
    struct DataWithoutFileAttr {
        EntryHeader    header;
        Data_FileType  fileType;
    };

    if ( currentPersistentLayout >= fdelWithFileAttributes ) {

        return this->GetFileType();

    } else {

        return reinterpret_cast<DataWithoutFileAttr*>(
            this->PersistentPointer() )->fileType;
    }
}

/**************************************************************************/

SAPDB_UInt FileDir_Entry::GetFirstByteOffset()
{
    return sizeof( RecordHeader );
}

/**************************************************************************/

Data_PageNo FileDir_Entry::GetFileDirPageNo() const
{
    return m_dataPage->PageNo();
}

/**************************************************************************/
