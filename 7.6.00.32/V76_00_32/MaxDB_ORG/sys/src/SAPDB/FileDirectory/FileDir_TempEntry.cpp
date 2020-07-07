/*****************************************************************************/
/*!
 @file    FileDir_TempEntry.cpp
 @author  MartinKi
 @ingroup FileDirectory_Files

 @brief Implementation of class FileDir_TempEntry.

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

#include "FileDirectory/FileDir_TempEntry.hpp"
#include "FileDirectory/FileDir_Common.hpp"
#include "FileDirectory/FileDir_Exception.hpp"
#include "FileDirectory/FileDir_Messages.hpp"

#include "DataAccess/Data_FileTypes.hpp"
// for Data_PageNo:
#include "DataAccess/Data_Types.hpp"

#include "Converter/Converter_Version.hpp"

#include "KernelCommon/Kernel_Dump.hpp"

#include "RunTime/RTE_Crash.hpp"
#include "RunTime/RTE_Types.hpp"
#include "RunTime/System/RTESys_AtomicOperation.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/Fields/SAPDBFields_Field.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"

#include "ggg91.h"
#include "hgg06.h" // gg06SetNilTrans

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

FileDir_TempEntry::~FileDir_TempEntry() {
    SAPDBTRACE_METHOD_DEBUG( "FileDir_TempEntry::~FileDir_TempEntry",
                             FileDir_Trace, 6 );
    if ( m_refCounter != 0 ) {
        FileDir_Exception errMsg(
            __CONTEXT__, FILEDIR_REFCOUNT_NOT_NULL,
            SAPDB_ToString( m_refCounter ),
            FileDir_FileNoToString( m_fileNo ) );
        RTE_Crash(errMsg);
    }
}

/**************************************************************************/

FileDir_FileNo FileDir_TempEntry::GetFileNo() const
{
    return m_fileNo;
}

/**************************************************************************/

Data_FileState FileDir_TempEntry::GetFileState() const
{
    return m_fileState;
}

/**************************************************************************/

void FileDir_TempEntry::SetFileState(
    const Data_FileState newFileState)
{
    m_fileState = newFileState;
}

/**************************************************************************/

Data_PageNo FileDir_TempEntry::GetRootPage() const
{
    return m_rootPage;
}

/**************************************************************************/

SAPDB_UInt8 FileDir_TempEntry::GetEntryCount(const RTE_TaskId taskId) const
{
    return m_entryCount;
}

/**************************************************************************/

SAPDB_UInt4 FileDir_TempEntry::GetLeafPageCount(const RTE_TaskId taskId) const
{
    return m_leafPageCount;
}

/**************************************************************************/

SAPDB_UInt4 FileDir_TempEntry::GetIndexPageCount(const RTE_TaskId taskId) const
{
    return m_indexPageCount;
}

/**************************************************************************/

/**************************************************************************/

void FileDir_TempEntry::SetLeafPageCount(
    const SAPDB_UInt4 pageCount,
    const SAPDB_Bool  setInitialized)
{
    m_leafPageCount = static_cast<PageCount>( pageCount );
}

/**************************************************************************/

void FileDir_TempEntry::SetIndexPageCount(const SAPDB_UInt4 pageCount)
{
    m_indexPageCount = static_cast<PageCount>( pageCount );
}

/**************************************************************************/

void FileDir_TempEntry::SetEntryCount(const SAPDB_UInt8 entryCount)
{
    m_entryCount = static_cast<EntryCount>( entryCount );
}

/**************************************************************************/

void
FileDir_TempEntry::InitializeBasicCounters(
    const RTE_TaskId  taskId,
    const SAPDB_UInt4 leafPageCount,
    const SAPDB_UInt4 indexPageCount,
    const SAPDB_UInt8 entryCount)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_TempEntry::InitializeBasicCounters", FileDir_Trace, 5 );

    m_leafPageCount  = static_cast<PageCount>( leafPageCount );
    m_indexPageCount = static_cast<PageCount>( indexPageCount );
    m_entryCount     = static_cast<EntryCount>( entryCount );
}

/**************************************************************************/

SAPDB_Bool FileDir_TempEntry::AreCountersInitialized() const
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_TempEntry::AreCountersInitialized",
                             FileDir_Trace, 5 );
    return true;
}

/**************************************************************************/

void FileDir_TempEntry::ModifyLeafPageCount(
    const RTE_TaskId taskId,
    const Converter_Version converterVersion,
    const SAPDB_Int4 delta)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_TempEntry::ModifyLeafPageCount",
                             FileDir_Trace, 6 );

    if ( ( delta < 0 ) &&
         ( m_leafPageCount < static_cast<PageCount>( -delta ) ) ) {
        RTESys_AtomicWrite( m_leafPageCount, static_cast<PageCount>( 0 ) );
    } else {
        RTESys_AtomicModify( m_leafPageCount, delta );
    }
}

/**************************************************************************/

void FileDir_TempEntry::ModifyIndexPageCount(
    const RTE_TaskId taskId,
    const Converter_Version converterVersion,
    const SAPDB_Int4 delta)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_TempEntry::ModifyIndexPageCount",
                             FileDir_Trace, 6 );

    if ( ( delta < 0 ) &&
         ( m_leafPageCount < static_cast<PageCount>( -delta ) ) ) {
        RTESys_AtomicWrite( m_indexPageCount, static_cast<PageCount>( 0 ) );
    } else {
        RTESys_AtomicModify( m_indexPageCount, delta );
    }
}

/**************************************************************************/

void FileDir_TempEntry::ModifyEntryCount(
    const RTE_TaskId taskId,
    const Converter_Version converterVersion,
    const SAPDB_Int8 delta)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_TempEntry::ModifyEntryCount",
                             FileDir_Trace, 6 );

    if ( ( delta < 0 ) &&
         ( m_entryCount < static_cast<EntryCount>( -delta ) ) ) {
        RTESys_AtomicWrite( m_entryCount, static_cast<EntryCount>( 0 ) );
    } else {
        RTESys_AtomicModify( m_entryCount, delta );
    }
}

/**************************************************************************/

void FileDir_TempEntry::Remove(const RTE_TaskId taskId)
{
    this->SetInvalid();
}

/**************************************************************************/

void FileDir_TempEntry::SetValid()
{
    m_isValid = true;
}

/**************************************************************************/

void FileDir_TempEntry::SetInvalid()
{
    m_isValid = false;
}

/**************************************************************************/

void FileDir_TempEntry::SetLazyDelete(const RTE_TaskId taskId)
{
    m_lazyDelete = true;
}

/**************************************************************************/

SAPDB_Bool FileDir_TempEntry::IsLazyDeleteSet()
{
    return m_lazyDelete;
}

/**************************************************************************/

SAPDB_Bool FileDir_TempEntry::IsPersistent() const
{
    return SAPDB_FALSE;
}

/**************************************************************************/

Converter_Version FileDir_TempEntry::GetEntryConverterVersion() {
    return m_converterVersion;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_TempEntry::MarkFileDeleted(
    const tgg91_TransNo& transNo)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_TempEntry::MarkFileDeleted", FileDir_Trace, 5 );

    // File needs to be locked exclusively !!!

    if ( GetFileState() == Data_FileDeleted ) {
        return FileDir_AlreadyMarkedForDeletion;;
    }

    SetFileState( Data_FileDeleted );
    return FileDir_Okay;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_TempEntry::RemoveFileDeletedMark()
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_TempEntry::RemoveFileDeletedMark", FileDir_Trace, 5 );

    // File needs to be locked exclusively !!!

    SetFileState( Data_FileOk );
    return FileDir_Okay;
}

/**************************************************************************/

void FileDir_TempEntry::GetDeleteStamp(tgg91_TransNo& deleteStamp) const
{
    gg06SetNilTrans( deleteStamp );
}

/**************************************************************************/

SAPDB_Bool FileDir_TempEntry::IsReadOnly() const
{
    return m_fileState == Data_FileDeleted;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_TempEntry::Initialize()
{
    return FileDir_BaseEntry::Initialize();
}

/**************************************************************************/

SAPDB_Bool FileDir_TempEntry::FillDumpEntry(
        const RTE_TaskId   taskId,
        SAPDBFields_Field& dumpBuffer,
        SAPDB_Int&         startPos)
{
    struct Dump {
        FileDir_FileNo     fileNo;
        Data_PageNo        rootPage;
        Data_PageNo        pageNo;
        EntryCount         entryCount;
        PageCount          leafPageCount;
        PageCount          indexPageCount;
        Data_FileState     fileState;
        Data_FileType      fileType;
        SAPDB_Bool         isPerm;
        SAPDB_Bool         needsUpdate;
        SAPDB_Int2         filler;
        SAPDB_Int4         leafCountDelta;
        SAPDB_Int4         indexCountDelta;
        SAPDB_Int4         entryCountDelta;
        CacheAccessCounter cacheHits;
        CacheAccessCounter cacheMisses;
    };
    Dump dump;

    dump.fileNo          = m_fileNo;
    dump.rootPage        = this->GetRootPage();
    dump.pageNo          = NIL_PAGE_NO_GG00;
    dump.entryCount      = m_entryCount;
    dump.leafPageCount   = m_leafPageCount;
    dump.indexPageCount  = m_indexPageCount;
    dump.fileState       = m_fileState;
    dump.fileType        = this->GetFileType();
    dump.isPerm          = false;
    dump.needsUpdate     = false;
    dump.leafCountDelta  = 0;
    dump.indexCountDelta = 0;
    dump.entryCountDelta = 0;
    // cache hit counters are not yet in use and undefined to save memory
//     dump.cacheHits       = m_cacheHitCounter;
//     dump.cacheMisses     = m_cacheMissCounter;
    dump.cacheHits       = 0;
    dump.cacheMisses     = 0;
    dump.filler          = 0;

    if ( static_cast<SAPDB_Int>(dumpBuffer.GetLength() - startPos - sizeof(dump))
         <= 0 ) {
        return false;
    }

    void* pointer = dumpBuffer.GetPointer( startPos, sizeof(dump) );
    SAPDB_MemCopyNoCheck ( pointer, &dump, sizeof(dump) );
    startPos += sizeof(dump);

    return true;
}

/**************************************************************************/

void FileDir_TempEntry::SetRootPage(
    const RTE_TaskId taskId,
    const Data_PageNo& rootPage)
{
    m_rootPage = rootPage;
}

/**************************************************************************/

void
FileDir_TempEntry::SetPagesClustered(
    const RTE_TaskId taskId,
    const SAPDB_Bool pagesClustered)
{
}

/**************************************************************************/

SAPDB_Bool
FileDir_TempEntry::ArePagesClustered() const
{
    return false;
}

/**************************************************************************/
