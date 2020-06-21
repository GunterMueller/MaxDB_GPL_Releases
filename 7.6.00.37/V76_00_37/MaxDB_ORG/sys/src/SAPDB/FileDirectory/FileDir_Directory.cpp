/*****************************************************************************/
/*!
 @file    FileDir_Directory.cpp
 @author  MartinKi
 @ingroup FileDir_Directory_Files

 @brief Implementation of class FileDir_Directory.

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

/*!
 * @defgroup FileDirectory_Whole File Directory
 */
/*!
 * @defgroup FileDirectory_Files File Directory source files
 * @ingroup FileDirectory_Whole
 */
/*!
 * @defgroup FileDirectory_Classes File Directory classes
 * @ingroup FileDirectory_Whole
 */
/*!
 * @defgroup FileDirectory_Interfaces Interfaces to the file directory.
 * @ingroup FileDirectory_Whole
 */

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "FileDirectory/FileDir_HashBucket.hpp"
#include "FileDirectory/FileDir_Directory.hpp"
#include "FileDirectory/FileDir_SharedDirectory.hpp"
#include "FileDirectory/FileDir_GenericDirectory.hpp"
#include "FileDirectory/FileDir_TableEntry.hpp"
#include "FileDirectory/FileDir_TempTableEntry.hpp"
#include "FileDirectory/FileDir_IndexEntry.hpp"
#include "FileDirectory/FileDir_ShortColumnEntry.hpp"
#include "FileDirectory/FileDir_OmsKeyEntry.hpp"
#include "FileDirectory/FileDir_OmsKeyPartitionEntry.hpp"
#include "FileDirectory/FileDir_OmsVarEntry.hpp"
#include "FileDirectory/FileDir_OmsFixedEntry.hpp"
#include "FileDirectory/FileDir_OmsEntry.hpp"
#include "FileDirectory/FileDir_Entry.hpp"
#include "FileDirectory/FileDir_BaseEntry.hpp"
#include "FileDirectory/FileDir_PageManager.hpp"
#include "FileDirectory/FileDir_Page.hpp"
#include "FileDirectory/FileDir_Common.hpp"
#include "FileDirectory/FileDir_Types.hpp"

#include "FileDirectory/FileDir_Generic.hpp"
#include "FileDirectory/FileDir_Table.hpp"
#include "FileDirectory/FileDir_Index.hpp"
#include "FileDirectory/FileDir_ShortColumn.hpp"
#include "FileDirectory/FileDir_Oms.hpp"
#include "FileDirectory/FileDir_OmsKey.hpp"
#include "FileDirectory/FileDir_OmsKeyPartition.hpp"
#include "FileDirectory/FileDir_OmsVar.hpp"
#include "FileDirectory/FileDir_OmsFixed.hpp"

#include "FileDirectory/FileDir_Vector.hpp"

#include "Converter/Converter_Types.hpp"
#include "Converter/Converter_ICommon.hpp"
#include "Converter/Converter_IPageNoManager.hpp"

#include "FileDirectory/FileDir_Iterator.hpp"
#include "FileDirectory/FileDir_IteratorObj.hpp"
#include "FileDirectory/FileDir_Exception.hpp"
#include "FileDirectory/FileDir_Messages.hpp"

#include "KernelCommon/Kernel_IndexDescription.hpp"

// vgetpid():
#include "heo51.h"

#include "DataAccess/Data_Types.hpp"
#include "DataAccess/Data_FileTypes.hpp"
#include "RunTime/System/RTESys_AtomicOperation.hpp"
#include "RunTime/RTE_Types.hpp"
#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"

#include "SAPDBCommon/Messages/SDBMsg_FileDir.h"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"

#include "ggg91.h"
#include "ggg00.h"
#include "hgg01_3.h" // g01vtrace

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/

class Kernel_BlobDescription;

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/**************************************************************************/

FileDir_Directory::FileDir_Directory(SAPDBMem_IRawAllocator& alloc)
    : FileDir_GenericDirectory( alloc, FileDir_MinimalLocking),
      m_initialized(false),
      m_status(stOk),
      m_delOpCounter(0),
      m_sharedFd(FileDir_SharedDirectory::Instance()),
      m_pointerCache(alloc),
      m_iteratorCounter(0)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Directory::FileDir", FileDir_Trace, 5 );
}

/**************************************************************************/

SAPDB_Bool FileDir_Directory::Initialize(SAPDB_Int4 initialCapacity)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Directory::Initialize",
                             FileDir_Trace, 5 );

    if ( m_initialized ) {
        return true;
    }

    m_initialized = FileDir_GenericDirectory::Initialize( initialCapacity );
    return m_initialized;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_Directory::Shutdown(RTE_TaskId taskId)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Directory::Shutdown", FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized );

    // make sure no iterators are working with this object any more
    m_status = stShuttingDown;

    SAPDBERR_ASSERT_STATE( m_iteratorCounter == 0 );

    // do actual shutdown
    m_initialized = false;
    m_pointerCache.Clear(); // necessary to free all references to entries

    FileDir_GenericDirectory::Shutdown( taskId );

    return FileDir_Okay;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_Directory::AddEntry(
    const RTE_TaskId         taskId,
    const FileDir_FileNo&    fileNo,
    FileDir_BaseEntry*       entry,
    Msg_List&                msgList)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Directory::AddEntry",
                             FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    FileDir_ReturnCode retCode;
    if ( fileNo.IsTempFileNo() ) {
        retCode = FileDir_GenericDirectory::AddFileEntry(
            taskId,
            fileNo,
            entry,
            msgList,
            FileDir_NoLocking);
    } else {
        retCode = m_sharedFd.AddFileEntry(
            taskId,
            fileNo,
            entry,
            msgList,
            FileDir_UseLocking);
    }
    return retCode;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_Directory::AddTableFile(
    RTE_TaskId               taskId,
    const FileDir_FileNo&    fileNo,
    const Converter_Version& converterVersion,
    const Data_PageNo&       rootPage,
    const SAPDB_UInt2        blobColCount,
    const tgg00_FiletypeSet& fileTypeSet,
    const tgg91_FileVersion& fileVersion,
    Msg_List&                msgList,
    FileDir_Table*           pointer)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Directory::AddTableFile",
                             FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    if ( taskId == RTE_UnknownTaskId ) {
        vgetpid( taskId );
    }

    if ( pointer != 0 ) {
        pointer->Detach();
    }

    FileDir_Table fi;
    FileDir_ReturnCode ret;
    if ( fileNo.IsPersistentFileNo() ||
         fileTypeSet.includes(ftsShared_egg00) ) {
        ret = m_sharedFd.AddTableFile(
            taskId,
            fileNo,
            converterVersion,
            rootPage,
            blobColCount,
            fileTypeSet,
            fileVersion,
            msgList,
            &fi);
    } else {
        ret = FileDir_GenericDirectory::AddTableFile(
            taskId,
            fileNo,
            converterVersion,
            rootPage,
            blobColCount,
            fileTypeSet,
            fileVersion,
            msgList,
            &fi);
    }

    if ( ret != FileDir_Okay ) {
        return ret;
    }

    if ( pointer != 0 ) {
        pointer->Assign( fi );
    }

    return FileDir_Okay;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_Directory::AddSharedTempTableFile(
    RTE_TaskId               taskId,
    const FileDir_FileNo&    fileNo,
    const Converter_Version& converterVersion,
    const Data_PageNo&       rootPage,
    const SAPDB_UInt2        blobColCount,
    const tgg00_FiletypeSet& fileTypeSet,
    Msg_List&                msgList,
    FileDir_Table*           pointer)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Directory::AddSharedTempTableFile",
                             FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );
    SAPDBERR_ASSERT_ARGUMENT( fileNo.IsTempFileNo() );

    if ( taskId == RTE_UnknownTaskId ) {
        vgetpid( taskId );
    }

    if ( pointer != 0 ) {
        pointer->Detach();
    }

    tgg91_FileVersion fversion;
    fversion.gg91SetDummyVers();

    FileDir_Table fi;
    FileDir_ReturnCode ret;
    ret = m_sharedFd.AddTableFile(
            taskId,
            fileNo,
            converterVersion,
            rootPage,
            blobColCount,
            fileTypeSet,
            fversion,
            msgList,
            &fi);

    if ( ret != FileDir_Okay ) {
        return ret;
    }

    if ( pointer != 0 ) {
        pointer->Assign( fi );
    }

    return FileDir_Okay;
}

/**************************************************************************/

SAPDB_Bool FileDir_Directory::GetFileEntry(
    const RTE_TaskId      taskId,
    const FileDir_FileNo& fileNo,
    FileDir_Generic&      fileEntry) const
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Directory::GetFileEntry",
                             FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    if ( fileNo.IsPersistentFileNo() ) {
        return m_sharedFd.GetFileEntry( taskId, fileNo, fileEntry );
    } else {
        return FileDir_GenericDirectory::GetFileEntry(
            taskId, fileNo, fileEntry );
    }
}

/**************************************************************************/

FileDir_ReturnCode FileDir_Directory::GetTableFile(
    const RTE_TaskId      taskId,
    const FileDir_FileNo& fileNo,
    FileDir_Table&        p,
    Msg_List&             msgList)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Directory::GetTableFile",
                             FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    FileDir_ReturnCode retCode = GetEntryPointer( taskId, fileNo, p, msgList );

    if ( (retCode == FileDir_Okay) && (p.GetFileType() != Data_TableFile) ) {
        msgList =
            Msg_List(
                Msg_List::Error,
                SDBMSG_FILEDIR_FILETYPE_MISMATCH,
                Msg_Arg("FILENO", FileDir_FileNoToString( fileNo )),
                Msg_Arg("NO", SAPDB_ToString( Data_TableFile )),
                Msg_Arg("TYPE", SAPDB_ToString( p.GetFileType() ))
                );
        p.Detach();
        retCode = FileDir_FileTypeMismatch;
    }

    return retCode;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_Directory::GetFile(
    const RTE_TaskId&     taskId,
    const FileDir_FileNo& fileNo,
    FileDir_Generic&      p,
    Msg_List&             msgList)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Directory::GetFile", FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    FileDir_ReturnCode retCode =
        GetEntryPointer( taskId, fileNo, p, msgList );

    return retCode;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_Directory::GetEntry(
    const RTE_TaskId      taskId,
    const FileDir_FileNo& fileNo,
    FileDir_Generic&      p,
    Msg_List&             msgList) const
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_Directory::GetEntry",
        FileDir_Trace, 5 );

    FileDir_ReturnCode ret;
    if ( fileNo.IsTempFileNo() ) {
        ret = FileDir_GenericDirectory::GetEntry(
            taskId,
            fileNo,
            p,
            msgList);
        if ( ret == FileDir_FileNoNotFound ) {
            // maybe file is a shared temp file
            ret = m_sharedFd.GetEntry(
                taskId,
                fileNo,
                p,
                msgList);
        }
    } else {
        ret = m_sharedFd.GetEntry(
            taskId,
            fileNo,
            p,
            msgList );
    }
#ifdef SAPDB_SLOW
    if ( ret != FileDir_Okay ) {
        SAPDBTRACE_WRITELN(
            FileDir_Trace, 1,
            "fileNo not found: " << fileNo );
    }
#endif
    return ret;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_Directory::GcGetFile(
    const RTE_TaskId      taskId,
    const FileDir_FileNo& fileNo,
    FileDir_Generic&      p,
    Msg_List&             msgList)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Directory::GcGetFile", FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    FileDir_ReturnCode retCode = m_pointerCache.GetFile( fileNo, p );

    if ( retCode == FileDir_Okay ) {
        return FileDir_Okay;
    }

    if ( fileNo.IsTempFileNo() ) {
        if ( ! FileDir_GenericDirectory::GetFileEntry( taskId, fileNo, p ) ) {
            // might be a shared temp table
            m_sharedFd.GcGetFile( taskId, fileNo, p, msgList );
        }
    } else {
        m_sharedFd.GcGetFile( taskId, fileNo, p, msgList );
    }

    if ( p.IsAssigned() ) {
        if ( Data_FileDeleted != p.GetFileState() ) {
            m_pointerCache.AddEntryToCache( p );
        }
        retCode = FileDir_Okay;
    } else {
        if ( g01vtrace.vtrAny_gg00 ) {
            Kernel_VTrace()
                << "FileDir_Directory::GcGetFile fileNo not found: " << fileNo;
        }
        retCode = FileDir_FileNoNotFound;
    }
    return retCode;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_Directory::GetShortColumnFile(
    const RTE_TaskId      taskId,
    const FileDir_FileNo& fileNo,
    FileDir_ShortColumn&  p,
    Msg_List&             msgList)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Directory::GetShortColumnFile",
                             FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    FileDir_ReturnCode retCode = GetEntryPointer( taskId, fileNo, p, msgList );

    if ( (retCode == FileDir_Okay)
         && (p.GetFileType() != Data_ShortColumnFile) ) {
        msgList =
            Msg_List(
                Msg_List::Error,
                SDBMSG_FILEDIR_FILETYPE_MISMATCH,
                Msg_Arg("FILENO", FileDir_FileNoToString( fileNo )),
                Msg_Arg("NO", SAPDB_ToString( Data_ShortColumnFile )),
                Msg_Arg("TYPE", SAPDB_ToString( p.GetFileType() ))
                );
        p.Detach();
        retCode = FileDir_FileTypeMismatch;
    }

    return retCode;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_Directory::GetIndexFile(
    const RTE_TaskId      taskId,
    const FileDir_FileNo& fileNo,
    FileDir_Index&        p,
    Msg_List&             msgList)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Directory::GetIndexFile",
                             FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    FileDir_ReturnCode retCode = GetEntryPointer( taskId, fileNo, p, msgList );

    if ((retCode == FileDir_Okay) && 
        (p.GetFileType() != Data_IndexFile) && 
        (p.GetFileType() != Data_OmsKeyPartitionFile)) {
        msgList =
            Msg_List(
                Msg_List::Error,
                SDBMSG_FILEDIR_FILETYPE_MISMATCH,
                Msg_Arg("FILENO", FileDir_FileNoToString( fileNo )),
                Msg_Arg("NO", SAPDB_ToString( Data_IndexFile )),
                Msg_Arg("TYPE", SAPDB_ToString( p.GetFileType() ))
                );
        p.Detach();
        retCode = FileDir_FileTypeMismatch;
    }

    return retCode;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_Directory::GetOmsFile(
    const RTE_TaskId      taskId,
    const FileDir_FileNo& fileNo,
    FileDir_Oms&          p,
    Msg_List&             msgList)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Directory::GetOmsFile",
                             FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    FileDir_ReturnCode retCode =
        GetEntryPointer( taskId, fileNo, p, msgList );

    if ( (retCode == FileDir_Okay)
         && (   (p.GetFileType() != Data_OmsFixedFile)
             || (p.GetFileType() != Data_OmsVarFile)) ) {
        msgList =
            Msg_List(
                Msg_List::Error,
                SDBMSG_FILEDIR_FILETYPE_MISMATCH,
                Msg_Arg("FILENO", FileDir_FileNoToString( fileNo )),
                Msg_Arg("NO", SAPDB_ToString( Data_OmsFile )),
                Msg_Arg("TYPE", SAPDB_ToString( p.GetFileType() ))
                );
        p.Detach();
        retCode = FileDir_FileTypeMismatch;
    }

    return retCode;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_Directory::GetOmsKeyFile(
    const RTE_TaskId      taskId,
    const FileDir_FileNo& fileNo,
    FileDir_OmsKey&       p,
    Msg_List&             msgList)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Directory::GetOmsKeyFile",
                             FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    FileDir_ReturnCode retCode =
        GetEntryPointer( taskId, fileNo, p, msgList );

    if ( (retCode == FileDir_Okay) && (p.GetFileType() != Data_OmsKeyFile) ) {
        msgList =
            Msg_List(
                Msg_List::Error,
                SDBMSG_FILEDIR_FILETYPE_MISMATCH,
                Msg_Arg("FILENO", FileDir_FileNoToString( fileNo )),
                Msg_Arg("NO", SAPDB_ToString( Data_OmsKeyFile )),
                Msg_Arg("TYPE", SAPDB_ToString( p.GetFileType() ))
                );
        p.Detach();
        retCode = FileDir_FileTypeMismatch;
    }

    return retCode;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_Directory::GetOmsVarFile(
    const RTE_TaskId      taskId,
    const FileDir_FileNo& fileNo,
    FileDir_OmsVar&       p,
    Msg_List&             msgList)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Directory::GetOmsVarFile",
                             FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    FileDir_ReturnCode retCode =
        GetEntryPointer( taskId, fileNo, p, msgList );

    if ( (retCode == FileDir_Okay) && (p.GetFileType() != Data_OmsVarFile) ) {
        msgList =
            Msg_List(
                Msg_List::Error,
                SDBMSG_FILEDIR_FILETYPE_MISMATCH,
                Msg_Arg("FILENO", FileDir_FileNoToString( fileNo )),
                Msg_Arg("NO", SAPDB_ToString( Data_OmsVarFile )),
                Msg_Arg("TYPE", SAPDB_ToString( p.GetFileType() ))
                );
        p.Detach();
        retCode = FileDir_FileTypeMismatch;
    }

    return retCode;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_Directory::GetOmsFixedFile(
    const RTE_TaskId      taskId,
    const FileDir_FileNo& fileNo,
    FileDir_OmsFixed&     p,
    Msg_List&             msgList)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Directory::GetOmsFixedFile",
                             FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    FileDir_ReturnCode retCode =
        GetEntryPointer( taskId, fileNo, p, msgList );

    if ( (retCode == FileDir_Okay) && (p.GetFileType() != Data_OmsFixedFile) ) {
        msgList =
            Msg_List(
                Msg_List::Error,
                SDBMSG_FILEDIR_FILETYPE_MISMATCH,
                Msg_Arg("FILENO", FileDir_FileNoToString( fileNo )),
                Msg_Arg("NO", SAPDB_ToString( Data_OmsFixedFile )),
                Msg_Arg("TYPE", SAPDB_ToString( p.GetFileType() ))
                );
        p.Detach();
        retCode = FileDir_FileTypeMismatch;
    }

    return retCode;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_Directory::AddIndexFile(
    RTE_TaskId               taskId,
    const FileDir_FileNo&    fileNo,
    const FileDir_FileNo&    parentFileNo,
    const Converter_Version& converterVersion,
    const SAPDB_UInt2        indexId,
    const Data_PageNo        rootPage,
    const tgg00_FiletypeSet& fileTypeSet,
    const tgg91_FileVersion& fileVersion,
    Msg_List&                msgList,
    FileDir_Index*           indexPointer)
{

    SAPDBTRACE_METHOD_DEBUG( "FileDir_Directory::AddIndexFile",
                             FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    if ( taskId == RTE_UnknownTaskId ) {
        vgetpid( taskId );
    }

    if ( indexPointer != 0 ) {
        indexPointer->Detach();
    }

    FileDir_Index fi;
    FileDir_ReturnCode ret;
    ret = m_sharedFd.AddIndexFile(
        taskId,
        fileNo,
        parentFileNo,
        converterVersion,
        indexId,
        rootPage,
        fileTypeSet,
        fileVersion,
        msgList,
        &fi );

    if ( ret != FileDir_Okay ) {
        return ret;
    }

    if ( indexPointer != 0 ) {
        indexPointer->Assign( fi );
    }

    return FileDir_Okay;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_Directory::AddShortColumnFile(
    RTE_TaskId               taskId,
    const FileDir_FileNo&    fileNo,
    const FileDir_FileNo&    parentFileNo,
    const Converter_Version& converterVersion,
    const Data_PageNo        rootPage,
    const tgg00_FiletypeSet& fileTypeSet,
    const tgg91_FileVersion& fileVersion,
    Msg_List&                msgList,
    FileDir_ShortColumn*     shortColPointer)
{

    SAPDBTRACE_METHOD_DEBUG( "FileDir_Directory::AddShortColumnFile",
                             FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    if ( taskId == RTE_UnknownTaskId ) {
        vgetpid( taskId );
    }

    if ( shortColPointer != 0 ) {
        shortColPointer->Detach();
    }

    FileDir_ShortColumn fi;
    FileDir_ReturnCode ret;
    ret = m_sharedFd.AddShortColumnFile(
        taskId,
        fileNo,
        parentFileNo,
        converterVersion,
        rootPage,
        fileTypeSet,
        fileVersion,
        msgList,
        &fi );

    if ( ret != FileDir_Okay ) {
        return ret;
    }

    if ( shortColPointer != 0 ) {
        shortColPointer->Assign( fi );
    }

    return FileDir_Okay;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_Directory::AddOmsFile(
    RTE_TaskId               taskId,
    const FileDir_FileNo&    fileNo,
    const Converter_Version& converterVersion,
    const Data_PageNo&       rootPage,
    const SAPDB_UInt2        objBodySize,
    const SAPDB_UInt2        objPerPageCount,
    const SAPDB_UInt2        chainCount,
    Msg_List&                msgList,
    FileDir_Oms*             pointer)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Directory::AddOmsFile",
                             FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    if ( taskId == RTE_UnknownTaskId ) {
        vgetpid( taskId );
    }

    if ( pointer != 0 ) {
        pointer->Detach();
    }

    FileDir_Oms fi;
    FileDir_ReturnCode ret;
    if ( fileNo.IsPersistentFileNo() ) {
        ret = m_sharedFd.AddOmsFile(
            taskId,
            fileNo,
            converterVersion,
            rootPage,
            objBodySize,
            objPerPageCount,
            chainCount,
            msgList,
            &fi);
    } else {
        ret = FileDir_GenericDirectory::AddOmsFile(
            taskId,
            fileNo,
            converterVersion,
            rootPage,
            objBodySize,
            objPerPageCount,
            chainCount,
            msgList,
            &fi);
    }

    if ( ret != FileDir_Okay ) {
        return ret;
    }

    if ( pointer != 0 ) {
        pointer->Assign( fi );
    }

    return FileDir_Okay;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_Directory::AddOmsKeyFile(
    RTE_TaskId               taskId,
    const FileDir_FileNo&    fileNo,
    const Converter_Version& converterVersion,
    const Data_PageNo&       rootPage,
    const SAPDB_UInt2        objBodySize,
    const SAPDB_UInt2        objPerPageCount,
    const SAPDB_UInt2        chainCount,
    const SAPDB_UInt2        keyPartitionCount,
    Msg_List&                msgList,
    FileDir_OmsKey*          pointer)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Directory::AddOmsKeyFile",
                             FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    if ( taskId == RTE_UnknownTaskId ) {
        vgetpid( taskId );
    }

    if ( pointer != 0 ) {
        pointer->Detach();
    }

    FileDir_OmsKey fi;
    FileDir_ReturnCode ret;
    if ( fileNo.IsPersistentFileNo() ) {
        ret = m_sharedFd.AddOmsKeyFile(
            taskId,
            fileNo,
            converterVersion,
            rootPage,
            objBodySize,
            objPerPageCount,
            chainCount,
            keyPartitionCount,
            msgList,
            &fi);
    } else {
        ret = FileDir_GenericDirectory::AddOmsKeyFile(
            taskId,
            fileNo,
            converterVersion,
            rootPage,
            objBodySize,
            objPerPageCount,
            chainCount,
            keyPartitionCount,
            msgList,
            &fi);
    }

    if ( ret != FileDir_Okay ) {
        return ret;
    }

    if ( pointer != 0 ) {
        pointer->Assign( fi );
    }

    return FileDir_Okay;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_Directory::AddOmsKeyPartitionFile(
    RTE_TaskId               taskId,
    const FileDir_FileNo&    fileNo,
    const FileDir_FileNo&    parentFileNo,
    const Converter_Version& converterVersion,
    const SAPDB_UInt2        partitionNo,
    const Data_PageNo&       rootPage,
    Msg_List&                msgList,
    FileDir_OmsKeyPartition* omsKeyPartitionPointer)
{

    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_Directory::AddOmsKeyPartitionFile",
        FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    if ( taskId == RTE_UnknownTaskId ) {
        vgetpid( taskId );
    }

    if ( omsKeyPartitionPointer != 0 ) {
        omsKeyPartitionPointer->Detach();
    }

    FileDir_OmsKeyPartition fi;
    FileDir_ReturnCode ret;
    ret = m_sharedFd.AddOmsKeyPartitionFile(
        taskId,
        fileNo,
        parentFileNo,
        converterVersion,
        partitionNo,
        rootPage,
        msgList,
        &fi );

    if ( ret != FileDir_Okay ) {
        return ret;
    }

    if ( omsKeyPartitionPointer != 0 ) {
        omsKeyPartitionPointer->Assign( fi );
    }

    return FileDir_Okay;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_Directory::AddOmsVarFile(
    RTE_TaskId               taskId,
    const FileDir_FileNo&    fileNo,
    const Converter_Version& converterVersion,
    const Data_PageNo&       rootPage,
    const SAPDB_UInt2        objBodySize,
    const SAPDB_UInt2        objPerPageCount,
    const SAPDB_UInt2        chainCount,
    Msg_List&                msgList,
    FileDir_OmsVar*          pointer)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Directory::AddOmsVarFile",
                             FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    if ( taskId == RTE_UnknownTaskId ) {
        vgetpid( taskId );
    }

    if ( pointer != 0 ) {
        pointer->Detach();
    }

    FileDir_OmsVar fi;
    FileDir_ReturnCode ret;
    if ( fileNo.IsPersistentFileNo() ) {
        ret = m_sharedFd.AddOmsVarFile(
            taskId,
            fileNo,
            converterVersion,
            rootPage,
            objBodySize,
            objPerPageCount,
            chainCount,
            msgList,
            &fi);
    } else {
        ret = FileDir_GenericDirectory::AddOmsVarFile(
            taskId,
            fileNo,
            converterVersion,
            rootPage,
            objBodySize,
            objPerPageCount,
            chainCount,
            msgList,
            &fi);
    }

    if ( ret != FileDir_Okay ) {
        return ret;
    }

    if ( pointer != 0 ) {
        pointer->Assign( fi );
    }

    return FileDir_Okay;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_Directory::AddOmsFixedFile(
    RTE_TaskId               taskId,
    const FileDir_FileNo&    fileNo,
    const Converter_Version& converterVersion,
    const Data_PageNo&       rootPage,
    const SAPDB_UInt2        objBodySize,
    const SAPDB_UInt2        objPerPageCount,
    const SAPDB_UInt2        chainCount,
    Msg_List&                msgList,
    FileDir_OmsFixed*        pointer)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Directory::AddOmsFixedFile",
                             FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    if ( taskId == RTE_UnknownTaskId ) {
        vgetpid( taskId );
    }

    if ( pointer != 0 ) {
        pointer->Detach();
    }

    FileDir_OmsFixed fi;
    FileDir_ReturnCode ret;
    if ( fileNo.IsPersistentFileNo() ) {
        ret = m_sharedFd.AddOmsFixedFile(
            taskId,
            fileNo,
            converterVersion,
            rootPage,
            objBodySize,
            objPerPageCount,
            chainCount,
            msgList,
            &fi);
    } else {
        ret = FileDir_GenericDirectory::AddOmsFixedFile(
            taskId,
            fileNo,
            converterVersion,
            rootPage,
            objBodySize,
            objPerPageCount,
            chainCount,
            msgList,
            &fi);
    }

    if ( ret != FileDir_Okay ) {
        return ret;
    }

    if ( pointer != 0 ) {
        pointer->Assign( fi );
    }

    return FileDir_Okay;
}

/**************************************************************************/

FileDir_ReturnCode FileDir_Directory::DelFile(
    RTE_TaskId            taskId,
    const FileDir_FileNo& fileNo,
    Msg_List&             msgList)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Directory::DelFile", FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    if ( taskId == RTE_UnknownTaskId ) {
        vgetpid( taskId );
    }

    FileDir_ReturnCode retCode = FileDir_FileNoNotFound;
    if ( fileNo.IsTempFileNo() ) {
        retCode = FileDir_GenericDirectory::DelFile( taskId, fileNo, msgList );
    }

    if ( retCode != FileDir_Okay ) {
        retCode = m_sharedFd.DelFile( taskId, fileNo, msgList );
    }

    if ( retCode == FileDir_Okay ) {
        m_pointerCache.RemoveEntryFromCache( fileNo );
        RTESys_AtomicModify( m_delOpCounter, 1 );
    }
    return retCode;
}

/**************************************************************************/

SAPDB_Bool FileDir_Directory::GetFileDirIterator(
    FileDir_Iterator&   iterator,
    tgg00_TransContext* transContext)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Directory::GetFileDirIterator",
                             FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    if ( ! m_initialized ) {
        return false;
    }

    if ( !iterator.CreateIteratorObj() ) {
        FileDir_Common::OutOfMemoryCrash( sizeof(FileDir_IteratorObj) );
    }

    iterator.GetIteratorObj()->Clear();
    iterator.GetIteratorObj()->SetTransContext( transContext );

    return iterator.GetIteratorObj()->Initialize( *this, &m_sharedFd, this );
}

/**************************************************************************/

SAPDB_Bool FileDir_Directory::GetPrivateFileDirIterator(
    FileDir_Iterator&   iterator,
    tgg00_TransContext* transContext)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Directory::GetPrivateFileDirIterator",
                             FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    if ( ! m_initialized ) {
        return false;
    }

    if ( !iterator.CreateIteratorObj() ) {
        FileDir_Common::OutOfMemoryCrash( sizeof(FileDir_IteratorObj) );
    }

    iterator.GetIteratorObj()->Clear();
    iterator.GetIteratorObj()->SetTransContext( transContext );

    return iterator.GetIteratorObj()->Initialize( *this, 0, this );
}

/**************************************************************************/

SAPDB_Bool FileDir_Directory::GetSharedFileDirIterator(
    FileDir_Iterator&   iterator,
    tgg00_TransContext* transContext)
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Directory::GetSharedFileDirIterator",
                             FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    if ( ! m_initialized ) {
        return false;
    }

    if ( !iterator.CreateIteratorObj() ) {
        FileDir_Common::OutOfMemoryCrash( sizeof(FileDir_IteratorObj) );
    }

    iterator.GetIteratorObj()->Clear();
    iterator.GetIteratorObj()->SetTransContext( transContext );

    return iterator.GetIteratorObj()->Initialize( m_sharedFd, 0, this );
}

/**************************************************************************/

FileDir_Directory::IteratorGetRc
FileDir_Directory::IteratorGetFile(
    tgg00_TransContext*   transContext,
    const FileDir_FileNo& fileNo,
    FileDir_Generic&      p) const
{
    SAPDBTRACE_METHOD_DEBUG( "FileDir_Directory::IteratorGetFile",
                             FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    if ( m_status != stOk ) {
        p.Detach();
        return igFileDirShuttingDown;
    }

    RTE_TaskId taskId;
    if ( transContext != 0 ) {
        taskId = transContext->trTaskId_gg00;
    } else {
        vgetpid( taskId );
    }

    /*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
    /* must not use pointer cache here because this might result in */
    /* concurrent access to cache and smart pointer stored there    */
    /*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

    IteratorGetRc retCode = igFileNoNotFound;
    if ( fileNo.IsTempFileNo() ) {
        retCode = FileDir_GenericDirectory::IteratorGetFile(
            transContext,
            fileNo,
            p );
    }

    if ( retCode != FileDir_Okay ) {
        retCode = m_sharedFd.IteratorGetFile( transContext, fileNo, p );
    }

    return retCode;
}

/**************************************************************************/

SAPDBMem_IRawAllocator&
FileDir_Directory::GetEntryAllocator(FileDir_FileNo fileNo) const
{
    return
        fileNo.IsPersistentFileNo()
        ?
        m_sharedFd.GetAllocator()
        :
        FileDir_GenericDirectory::GetAllocator();
}

/**********************************************************************/

void FileDir_Directory::Restart_SetFileNoHighWaterMark(
    FileDir_FileNo highestFileNo)
{
    m_sharedFd.Restart_SetFileNoHighWaterMark( highestFileNo );
}

/**************************************************************************/

SAPDB_UInt FileDir_Directory::GetDeleteOpCounter() const
{
    return m_delOpCounter;
}

/**************************************************************************/

SAPDBMem_IRawAllocator& FileDir_Directory::GetAllocator() const
{
    return FileDir_GenericDirectory::GetAllocator();
}

/**************************************************************************/

FileDir_IDirectory& FileDir_IDirectory::Instance(
    const tgg00_TransIndex trIndex)
{
    if ( trIndex == cgg_nil_transindex ) {
        return FileDir_SharedDirectory::IFileDirInstance();
    }

    FileDir_IDirectory* localFd = 0;
    kb50GetLocalFileDir( trIndex, localFd );

    return (localFd != 0) ?
        *localFd : FileDir_SharedDirectory::IFileDirInstance();
}

/**************************************************************************/

FileDir_ReturnCode FileDir_Directory::GetEntryPointer(
    const RTE_TaskId      taskId,
    const FileDir_FileNo& fileNo,
    FileDir_Generic&      p,
    Msg_List&             msgList)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_Directory::GetEntryPointer",
        FileDir_Trace, 5 );
    SAPDBERR_ASSERT_STATE( m_initialized == true );

    FileDir_ReturnCode retCode = m_pointerCache.GetFile( fileNo, p );

    if ( retCode == FileDir_Okay ) {
        return FileDir_Okay;
    }

    retCode = GetEntry( taskId, fileNo, p, msgList );
    if ( retCode != FileDir_Okay ) {
        return retCode;
    }

    m_pointerCache.AddEntryToCache( p );
    return FileDir_Okay;
}

/**************************************************************************/

void FileDir_Directory::CleanUpPointerCache()
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_Directory::CleanUpPointerCache",
        FileDir_Trace, 5 );

    m_pointerCache.CleanUpCache(
        m_delOpCounter + m_sharedFd.GetDeleteOpCounter());
}

/**********************************************************************/

SAPDB_UInt FileDir_Directory::GetCacheEntryCount() const
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_Directory::GetCacheEntryCount", FileDir_Trace, 5 );

    return m_pointerCache.GetSize();
}

/**************************************************************************/

SAPDB_Bool FileDir_Directory::RegisterIterator(FileDir_IteratorObj& iterator)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_Directory::RegisterIterator", FileDir_Trace, 5 );

    RTE_TaskId taskId;
    vgetpid( taskId );

    if ( m_status != stOk ) {
        return false;
    }

    RTESys_AtomicModify( m_iteratorCounter, 1 );

    return true;
}

/**************************************************************************/

void FileDir_Directory::UnregisterIterator(FileDir_IteratorObj& iterator)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_Directory::UnregisterIterator", FileDir_Trace, 5 );

    RTE_TaskId taskId;
    vgetpid( taskId );

    RTESys_AtomicModify( m_iteratorCounter, -1 );
}

/**************************************************************************/

void FileDir_Directory::SetAllCachedLockStates(const SAPDB_Int lockState)
{
    SAPDBTRACE_METHOD_DEBUG(
        "FileDir_Directory::SetAllCachedLockStates", FileDir_Trace, 5 );

    m_pointerCache.SetAllLockStates( lockState );
}

/**************************************************************************/
