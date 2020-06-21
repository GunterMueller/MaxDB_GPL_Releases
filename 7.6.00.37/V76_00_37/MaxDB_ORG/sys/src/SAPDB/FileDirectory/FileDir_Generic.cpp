/*****************************************************************************/
/*!
 @file    FileDir_Generic.cpp
 @author  MartinKi
 @ingroup FileDirectory_Files

 @brief Implementation of class FileDir_Generic.

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

#include "FileDirectory/FileDir_Generic.hpp"
#include "FileDirectory/FileDir_Entry.hpp"
#include "FileDirectory/FileDir_BaseEntry.hpp"

#include "DataAccess/Data_FileTypes.hpp"
// for Data_PageNo:
#include "DataAccess/Data_Types.hpp"

// for RTE_TaskId:
#include "RunTime/RTE_Types.hpp"

#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"

// for tgg91_TransNo:
#include "ggg91.h"

// vgetpid():
#include "heo51.h"

#include "Converter/Converter_Version.hpp"

struct tgg00_FileId;

/*************************************************************************/

FileDir_Generic::FileDir_Generic(FileDir_BaseEntry* fe)
    : fileEntry(fe),
      m_isPrimaryCounter(true),
      m_selfDestruct(false),
      m_primaryCounter(0),
      m_secondaryRefCounts(0),
      m_allocator(0),
      m_lockState(0),
      m_schemaHashkey(0)
{}

/*************************************************************************/

FileDir_Generic::FileDir_Generic(
    const FileDir_Generic& fe)
    : fileEntry(fe.fileEntry),
      m_isPrimaryCounter(fe.m_isPrimaryCounter),
      m_selfDestruct(false),
      m_primaryCounter(fe.m_primaryCounter),
      m_secondaryRefCounts(0),
      m_allocator(0),
      m_lockState(0),
      m_schemaHashkey(0)
{
    SAPDBERR_ASSERT_STATE(
        m_isPrimaryCounter
        || (m_primaryCounter != 0) );
    if ( ( m_isPrimaryCounter && (fileEntry != 0) )
         || ( m_primaryCounter != 0 ) ) {
        this->IncRefCount();
    }
}

/*************************************************************************/

void FileDir_Generic::IncreaseSecondaryRefCounter()
{
    SAPDBERR_ASSERT_STATE( m_allocator != 0 );
    SAPDBERR_ASSERT_STATE( ! m_selfDestruct );
    ++m_secondaryRefCounts;
}

/*************************************************************************/

void FileDir_Generic::DecreaseSecondaryRefCounter()
{
    SAPDBERR_ASSERT_STATE( m_allocator != 0 );
    SAPDBERR_ASSERT_STATE( m_secondaryRefCounts > 0 );
    --m_secondaryRefCounts;
    if ( m_selfDestruct && (m_secondaryRefCounts == 0) ) {
        this->DecRefCount();
        FileDir_Generic* p = this;
        destroy( p, *m_allocator );
    }
}

/*************************************************************************/

void FileDir_Generic::IncRefCount()
{
    if ( m_isPrimaryCounter ) {
        SAPDBERR_ASSERT_STATE( m_secondaryRefCounts == 0 );
        fileEntry->incRefCounter();
    } else {
        m_primaryCounter->IncreaseSecondaryRefCounter();
    }
}

/*************************************************************************/

void FileDir_Generic::DecRefCount()
{
    SAPDBERR_ASSERT_STATE( fileEntry != 0 );
    if ( m_isPrimaryCounter ) {
        SAPDBERR_ASSERT_STATE( m_secondaryRefCounts == 0 );
        if ( (fileEntry->decRefCounter() == 0) &&
             fileEntry->IsLazyDeleteSet() ) {
            // file was marked for lazy delete and its time has come
            RTE_TaskId taskId;
            vgetpid( taskId );
            fileEntry->Remove( taskId );
            destroy( fileEntry, fileEntry->GetAllocator() );
        }
    } else {
        m_primaryCounter->DecreaseSecondaryRefCounter();
    }
    fileEntry = 0;
}

/*************************************************************************/

FileDir_Generic::~FileDir_Generic()
{
    if ( fileEntry != 0 ) {
        SAPDBERR_ASSERT_STATE( m_secondaryRefCounts == 0 );
        this->DecRefCount();
    }
}

/*************************************************************************/

void FileDir_Generic::Detach()
{
    if ( fileEntry != 0 ) {
        this->DecRefCount();
        fileEntry = 0;
    }
}

/*************************************************************************/

void FileDir_Generic::AssignEntry( const FileDir_BaseEntry* entryPtr )
{
    if ( entryPtr == fileEntry ) {
        return;
    }

    if ( fileEntry != 0 ) {
        this->DecRefCount();
    }

    SAPDBERR_ASSERT_STATE( m_secondaryRefCounts == 0 );
    SAPDBERR_ASSERT_STATE( ! m_selfDestruct );

    m_isPrimaryCounter = true;
    m_primaryCounter   = 0;
    fileEntry          = const_cast<FileDir_BaseEntry*>(entryPtr);

    if (fileEntry != 0) {
        this->IncRefCount();
    }
}

/*************************************************************************/

FileDir_FileNo FileDir_Generic::GetFileNo() const
{
    return fileEntry->GetFileNo();
}

/*************************************************************************/

Data_FileType FileDir_Generic::GetFileType() const
{
    return fileEntry->GetFileType();
}

/*************************************************************************/

Data_FileState FileDir_Generic::GetFileState() const
{
    return fileEntry->GetFileState();
}

/*************************************************************************/

SAPDB_Bool FileDir_Generic::IsPersistent() const
{
    return fileEntry->IsPersistent();
}

/*************************************************************************/

void FileDir_Generic::SetFileState(const Data_FileState newFileState)
{
    fileEntry->SetFileState( newFileState );
}

/*************************************************************************/

FileDir_ReturnCode FileDir_Generic::MarkFileDeleted(
    const tgg91_TransNo& transNo)
{
    return fileEntry->MarkFileDeleted( transNo );
}

/*************************************************************************/

FileDir_ReturnCode FileDir_Generic::RemoveFileDeletedMark()
{
    return fileEntry->RemoveFileDeletedMark();
}

/*************************************************************************/

void FileDir_Generic::GetDeleteStamp(tgg91_TransNo& deleteStamp) const
{
    fileEntry->GetDeleteStamp( deleteStamp );
}

/*************************************************************************/

Data_PageNo FileDir_Generic::GetRootPage() const
{
    return fileEntry->GetRootPage();
}

/*************************************************************************/

void FileDir_Generic::SetRootPage(
    const RTE_TaskId taskId,
    const Data_PageNo& rootPage)
{
    fileEntry->SetRootPage( taskId, rootPage );
}

/*************************************************************************/

void FileDir_Generic::SetLeafPageCount(
    const SAPDB_UInt4 pageCount,
    const SAPDB_Bool  setInitialized)
{
    fileEntry->SetLeafPageCount( pageCount, setInitialized );
}

/*************************************************************************/

void FileDir_Generic::SetIndexPageCount(const SAPDB_UInt4 pageCount)
{
    fileEntry->SetIndexPageCount( pageCount );
}

/*************************************************************************/

void FileDir_Generic::SetEntryCount(const SAPDB_UInt8 entryCount)
{
    fileEntry->SetEntryCount( entryCount );
}

/*************************************************************************/

void
FileDir_Generic::InitializeBasicCounters(
    const RTE_TaskId  taskId,
    const SAPDB_UInt4 leafPageCount,
    const SAPDB_UInt4 indexPageCount,
    const SAPDB_UInt8 entryCount)
{
    fileEntry->InitializeBasicCounters(
        taskId, leafPageCount, indexPageCount, entryCount );
}

/*************************************************************************/

SAPDB_Bool FileDir_Generic::AreCountersInitialized() const
{
    return fileEntry->AreCountersInitialized();
}

/*************************************************************************/

SAPDB_UInt4 FileDir_Generic::GetLeafPageCount(
    const RTE_TaskId taskId) const
{
    return fileEntry->GetLeafPageCount( taskId );
}

/*************************************************************************/

SAPDB_UInt4 FileDir_Generic::GetIndexPageCount(
    const RTE_TaskId taskId) const
{
    return fileEntry->GetIndexPageCount( taskId );
}

/*************************************************************************/

SAPDB_UInt8 FileDir_Generic::GetEntryCount(
    const RTE_TaskId taskId) const
{
    return fileEntry->GetEntryCount( taskId );
}

/*************************************************************************/

SAPDB_Bool FileDir_Generic::IsReadOnly() const
{
    return fileEntry->IsReadOnly();
}

/*************************************************************************/

void FileDir_Generic::ModifyLeafPageCount(
    const RTE_TaskId        taskId,
    const Converter_Version converterVersion,
    const SAPDB_Int4        delta)
{
    fileEntry->ModifyLeafPageCount(
        taskId,
        converterVersion,
        delta);
}

/*************************************************************************/

void FileDir_Generic::ModifyIndexPageCount(
    const RTE_TaskId        taskId,
    const Converter_Version converterVersion,
    const SAPDB_Int4        delta)
{
    fileEntry->ModifyIndexPageCount(
        taskId,
        converterVersion,
        delta);
}

/*************************************************************************/

void FileDir_Generic::ModifyEntryCount(
    const RTE_TaskId        taskId,
    const Converter_Version converterVersion,
    const SAPDB_Int8        delta)
{
    fileEntry->ModifyEntryCount(
        taskId,
        converterVersion,
        delta);
}

/*************************************************************************/

void FileDir_Generic::BuildFileId(tgg00_FileId& fileId) const
{
    fileEntry->BuildFileId( fileId );
}

/*************************************************************************/

void FileDir_Generic::FillPreparedFileId(tgg00_FileId& fileId) const
{
    fileEntry->FillPreparedFileId( fileId );
}

/*************************************************************************/

void FileDir_Generic::GetFileVersion(tgg91_FileVersion& fileVersion) const
{
    fileEntry->GetFileVersion( fileVersion );
}

/*************************************************************************/

void FileDir_Generic::IncrementCacheHits()
{
    fileEntry->IncrementCacheHits();
}

/*************************************************************************/

void FileDir_Generic::IncrementCacheMisses()
{
    fileEntry->IncrementCacheMisses();
}

/*************************************************************************/

SAPDB_Real8 FileDir_Generic::GetCacheHitrate() const
{
    return fileEntry->GetCacheHitrate();
}

/*************************************************************************/

FileDir_Generic FileDir_Generic::operator=(FileDir_Generic const& fip)
{
    if (this == &fip) {
        return *this;
    }
    if (fileEntry != 0) {
        this->DecRefCount();
    }

    SAPDBERR_ASSERT_STATE( ! m_selfDestruct );

    fileEntry            = fip.fileEntry;
    m_isPrimaryCounter   = fip.m_isPrimaryCounter;
    m_primaryCounter     = fip.m_primaryCounter;
    m_secondaryRefCounts = 0;

    if (fileEntry != 0) {
        this->IncRefCount();
    }
    return *this;
}

/*************************************************************************/

FileDir_BaseEntry* FileDir_Generic::GetPointer() const
{
    return fileEntry;
}

/*************************************************************************/

void FileDir_Generic::Remove(const RTE_TaskId taskId)
{
    fileEntry->Remove( taskId );
}

/*************************************************************************/

void FileDir_Generic::UpdateAfterSavepoint(
    const RTE_TaskId taskId,
    const Converter_Version& converterVersion)
{
    fileEntry->UpdateAfterSavepoint( taskId, converterVersion );
}

/*************************************************************************/

Converter_Version FileDir_Generic::GetEntryConverterVersion()
{
    return fileEntry->GetEntryConverterVersion();
}

/*************************************************************************/

void FileDir_Generic::AssignSecondary(FileDir_Generic& p)
{
    if ( fileEntry != 0 ) {
        this->DecRefCount();
    }

    SAPDBERR_ASSERT_STATE( p.m_selfDestruct == false );
    SAPDBERR_ASSERT_STATE( m_secondaryRefCounts == 0 );

    m_isPrimaryCounter   = false;
    m_selfDestruct       = false;
    m_primaryCounter     = p.m_isPrimaryCounter ? &p : p.m_primaryCounter;
    m_secondaryRefCounts = 0;
    if ( p.IsAssigned() ) {
        fileEntry = p.fileEntry;
        this->IncRefCount();
    }
}

/*************************************************************************/

void FileDir_Generic::TransferReference(
    FileDir_Generic& to,
    FileDir_Generic& from)
{
    SAPDBERR_ASSERT_STATE( ! from.IsSecondaryPointer() );
    SAPDBERR_ASSERT_STATE( (! to.IsAssigned())
                           || (! to.IsSecondaryPointer()) );

    SAPDBERR_ASSERT_STATE( from.fileEntry != 0 );
    SAPDBERR_ASSERT_STATE( to.fileEntry == 0 );

    SAPDBERR_ASSERT_STATE( from.m_secondaryRefCounts == 0 );
    SAPDBERR_ASSERT_STATE( to.m_secondaryRefCounts == 0 );

    to.m_isPrimaryCounter = true;
    to.m_primaryCounter   = 0;
    to.fileEntry          = from.fileEntry;
    from.fileEntry        = 0;
}

/*************************************************************************/

void FileDir_Generic::SetSelfDestruct()
{
    SAPDBERR_ASSERT_STATE( m_isPrimaryCounter );
    m_selfDestruct = true;
    if ( m_secondaryRefCounts == 0 ) {
        this->DecRefCount();
        FileDir_Generic* p = this;
        destroy( p, *m_allocator );
    }
}

/*************************************************************************/

void FileDir_Generic::SetAllocator(SAPDBMem_IRawAllocator& allocator)
{
    m_allocator = &allocator;
}

/*************************************************************************/

void FileDir_Generic::SetComment(const char* comment)
{
    fileEntry->SetComment( comment );
}

/*************************************************************************/

const char* FileDir_Generic::GetComment() const
{
    return fileEntry->GetComment( );
}

/*************************************************************************/

void FileDir_Generic::SetLockState(const SAPDB_UInt2 lockState)
{
    if ( m_isPrimaryCounter ) {
        m_lockState = lockState;
    } else if ( m_primaryCounter != 0 ) {
        m_primaryCounter->m_lockState = lockState;
    }
}

/*************************************************************************/

SAPDB_UInt2 FileDir_Generic::GetLockState() const
{
    if ( m_isPrimaryCounter || ( m_primaryCounter == 0 ) ) {
        return m_lockState;
    } else {
        return m_primaryCounter->m_lockState;
    }
}

/*************************************************************************/

void FileDir_Generic::SetSchemaHashkey(const SAPDB_UInt2 SchemaHashkey)
{
    if ( m_isPrimaryCounter ) {
        m_schemaHashkey = SchemaHashkey;
    } else if ( m_primaryCounter != 0 ) {
        m_primaryCounter->m_schemaHashkey = SchemaHashkey;
    }
}

/*************************************************************************/

SAPDB_UInt2 FileDir_Generic::GetSchemaHashkey() const
{
    if ( m_isPrimaryCounter || ( m_primaryCounter == 0 ) ) {
        return m_schemaHashkey;
    } else {
        return m_primaryCounter->m_schemaHashkey;
    }
}

/*************************************************************************/

void
FileDir_Generic::SetPagesClustered(
    const RTE_TaskId taskId,
    const SAPDB_Bool pagesClustered)
{
    fileEntry->SetPagesClustered(
        taskId,
        pagesClustered );
}

/**************************************************************************/

SAPDB_Bool
FileDir_Generic::ArePagesClustered() const
{
    return fileEntry->ArePagesClustered();
}

/**************************************************************************/
