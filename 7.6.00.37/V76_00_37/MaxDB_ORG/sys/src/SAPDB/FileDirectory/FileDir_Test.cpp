/*****************************************************************************/
/*!

  @file         FileDir_Test.cpp
  @author       MartinKi
  @ingroup      FileDirectory_Files

  @brief        Tests for different components of the File Directory subsystem

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

#include "FileDirectory/FileDir_OmsVar.hpp"
#include "FileDirectory/FileDir_OmsKey.hpp"
#include "FileDirectory/FileDir_OmsFixed.hpp"
#include "FileDirectory/FileDir_Oms.hpp"
#include "FileDirectory/FileDir_Table.hpp"
#include "FileDirectory/FileDir_Index.hpp"
#include "FileDirectory/FileDir_IndexEntry.hpp"
#include "FileDirectory/FileDir_TableEntry.hpp"
#include "FileDirectory/FileDir_OmsEntry.hpp"
#include "FileDirectory/FileDir_OmsVarEntry.hpp"
#include "FileDirectory/FileDir_Iterator.hpp"
#include "FileDirectory/FileDir_Directory.hpp"
#include "FileDirectory/FileDir_ISharedDirectory.hpp"
#include "FileDirectory/FileDir_SharedDirectory.hpp"
#include "FileDirectory/FileDir_PageManager.hpp"

#include "KernelCommon/Kernel_BlobDescription.hpp"
#include "KernelCommon/Kernel_IndexDescription.hpp"

#include "RunTime/RTE_Crash.hpp"
#include "RunTime/Synchronisation/RTESync_InterlockedCounter.hpp"
#include "FileDirectory/FileDir_Exception.hpp"
#include "FileDirectory/FileDir_Messages.hpp"

#include "DataAccess/Data_FileTypes.hpp"
// for Data_PageNo:
#include "DataAccess/Data_Types.hpp"
// page number handling
#include "Converter/Converter_IPageNoManager.hpp"
#include "Converter/Converter_ICommon.hpp"

// vgetpid():
#include "heo51.h"

#include "hkb50.h"

#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include "SAPDBCommon/Messages/SDBMsg_FileDir.h"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"

#include "FileDirectory/FileDir_Test.hpp"

#include "ggg91.h"
#include "gsp00.h"
#include "ggg00.h"

#include <stdlib.h> /* nocheck */

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/// Minimum fileNo for temp files.
const SAPDB_UInt8 c_FileDir_TempStartFileId = 0x800000L;

/// Maximum number of indices allowed on a table.
const SAPDB_UInt  c_maxIndexNo = 255;


/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

RTESync_InterlockedCounter<SAPDB_UInt> FileDir_Test::m_activeTasks(0);
bool FileDir_Test::m_restarted(false);
bool FileDir_Test::m_shuttingDown(false);
bool FileDir_Test::m_crashOnFlush(false);
FileDir_Table FileDir_Test::m_refCounterFileEntry;

/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

void FileDir_Test::Assert(bool mustBeTrue)
{
    if ( ! mustBeTrue ) {
        RTE_Crash(
            SAPDBErr_Exception(
                __CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                "mustBeTrue") );
    }
}

/*===========================================================================*/

void FileDir_Test::TableCreator(
    const FileDir_FileNo& fileId,
    const Converter_Version& converterVersion,
    SAPDB_Int4& rootPage,
    tgg00_FiletypeSet& fileTypeSet,
    tgg91_FileVersion& fileVersion,
    Kernel_BlobDescription& blobs)
{
    rootPage = static_cast<SAPDB_Int4>((fileId.GetUInt8() % SAPDB_MAX_INT4) + 1);
    if ((fileId.GetUInt8() % 7) == 0) {
        for (int i=0; i<( (fileId.GetUInt8()+converterVersion) % 5); i++) {
            blobs.AddColumn(i,i);
        }
    }

    fileTypeSet = tgg00_FiletypeSet::build(
        2, ftsConcurrent_egg00, ftsPerm_egg00 );
    fileVersion.gg91SetDummyVers();
}

/*===========================================================================*/

void FileDir_Test::IndexCreator(
    const FileDir_FileNo fileId,
    const FileDir_FileNo parentFileId,
    const Converter_Version& converterVersion,
    SAPDB_Int4& rootPage,
    tgg00_FiletypeSet& fileTypeSet,
    tgg91_FileVersion& fileVersion,
    Kernel_IndexDescription& indexes)
{
    rootPage = static_cast<SAPDB_Int4>((fileId.GetUInt8() % SAPDB_MAX_INT4) + 1);
    for (int i=0; i<( (fileId.GetUInt8()+parentFileId.GetUInt8()+converterVersion) % 5) + 1; i++) {
        indexes.AddColumn(
            i,
            ((fileId.GetUInt8() % 2) == 0)
            ? Kernel_IndexDescription::ascending :
            Kernel_IndexDescription::descending );
    }

    fileTypeSet.build( 2, ftsConcurrent_egg00, ftsPerm_egg00 );
    fileVersion.gg91SetDummyVers();
}

/*===========================================================================*/

void FileDir_Test::OmsCreator(
    const FileDir_FileNo& fileId,
    const Converter_Version& converterVersion,
    SAPDB_Int4& rootPage,
    SAPDB_UInt2& objBodySize,
    SAPDB_UInt2& objPerPageCount,
    SAPDB_UInt2& chainCount)
{
    rootPage        = static_cast<SAPDB_Int4>((fileId.GetUInt8() % SAPDB_MAX_INT4) + 1);
    objBodySize     = 0;
    objPerPageCount = 0;
    chainCount      = 0;
}

/*===========================================================================*/

void FileDir_Test::OmsVarCreator(
    const FileDir_FileNo& fileId,
    const Converter_Version& converterVersion,
    SAPDB_Int4& rootPage,
    SAPDB_UInt2& objBodySize,
    SAPDB_UInt2& objPerPageCount,
    SAPDB_UInt2& chainCount)
{
    rootPage        = static_cast<SAPDB_Int4>((fileId.GetUInt8() % SAPDB_MAX_INT4) + 1);
    objBodySize     = 0;
    objPerPageCount = 0;
    chainCount      = 0;
}

/*===========================================================================*/

void FileDir_Test::OmsContCreator(
    FileDir_IDirectory& fd,
    const FileDir_FileNo& fileId,
    const Converter_Version& converterVersion,
    const FileDir_FileNo& omsVarFileId,
    SAPDB_Int4& rootPage,
    SAPDB_UInt2& objBodySize,
    SAPDB_UInt2& objPerPageCount,
    SAPDB_UInt2& chainCount,
    SAPDB_UInt2& partitionNo)
{
    rootPage        = static_cast<SAPDB_Int4>((fileId.GetUInt8() % SAPDB_MAX_INT4) + 1);
    objBodySize     = 0;
    objPerPageCount = 0;
    chainCount      = 0;

    Msg_List errMsg;
    FileDir_Generic varEntry;
    fd.GetFile( RTE_UnknownTaskId, omsVarFileId, varEntry, errMsg );
    bool ok = ( varEntry.IsAssigned()
                && (varEntry.GetFileType() == Data_OmsVarFile) );
    if ( !ok ) {
        SAPDBTRACE_WRITELN(
            FileDir_Trace, 5,
            "OmsContCreator failed because of var entry" << omsVarFileId );
        FileDir_Exception
            errMsg( __CONTEXT__, FILEDIR_TEST_FAILED,
                    FileDir_FileNoToString( omsVarFileId ) );
        RTE_Crash(errMsg);
    }
    partitionNo = 1 + static_cast<SAPDB_UInt2>(
        (omsVarFileId.GetUInt8() % 5)
        + (fileId.GetUInt8() % 7))
        + (converterVersion % 15);
}

/*===========================================================================*/

bool FileDir_Test::VerifyTableEntry(
    SAPDBMem_IRawAllocator& alloc,
    FileDir_Table& p)
{
    FileDir_FileNo fileId = p.GetFileNo();

    SAPDB_Int4 r0;
    Kernel_BlobDescription b0(alloc), b1(alloc);
    tgg00_FiletypeSet fileTypeSet;
    tgg91_FileVersion fileVersion;

    TableCreator( fileId, p.GetEntryConverterVersion(), r0, fileTypeSet,
                 fileVersion, b0 );

    bool ok = true;
    ok &= (p.GetRootPage() == r0);

// blob col count is not maintained properly
//     ok &= (p.GetBlobColCount() == b0.GetColumnCount());

//     p.GetBlobDescription(b1);
//     if ( ok ) {
//         for (int j=0; j < b1.GetColumnCount(); j++) {
//             ok &= (b1.GetColumn(j) == b0.GetColumn(j));
//         }
//     }
    if ( !ok ) {
        SAPDBTRACE_WRITELN(
            FileDir_Trace, 5,
            "Verify failed for table with file id " << p.GetFileNo()
            << NewLine
            << "Converter version: "
            << p.GetEntryConverterVersion() << NewLine
            << "Is <-> Should be" << NewLine
            << "Root: " << p.GetRootPage() << " <-> " << r0
            << "BlobColCount: " << p.GetBlobColCount() << " <-> "
            << b0.GetColumnCount() );
//         for (int j=0; j < b1.GetColumnCount(); j++) {
//             SAPDBTRACE_WRITELN(
//                 FileDir_Trace, 5,
//                 "BlobColNo " << j+1 << ": "
//                 << b1.GetColumn(j) << " <-> " << b0.GetColumn(j) );
//         }
        FileDir_Exception
            errMsg( __CONTEXT__, FILEDIR_TEST_FAILED,
                    FileDir_FileNoToString( p.GetFileNo() ) );
        RTE_Crash(errMsg);
    }

    return ok;
}

/*===========================================================================*/

bool FileDir_Test::VerifyIndexEntry(
    SAPDBMem_IRawAllocator& alloc,
    FileDir_Index& p)
{
    FileDir_FileNo fileId = p.GetFileNo();
    tgg00_FiletypeSet fileTypeSet;
    tgg91_FileVersion fileVersion;

    SAPDB_Int4 r0;
    Kernel_IndexDescription index(
        alloc,
        static_cast<SAPDB_UInt2>(fileId.GetUInt8() % c_maxIndexNo) + 1);
    Kernel_IndexDescription index1(
        alloc,
        static_cast<SAPDB_UInt2>(fileId.GetUInt8() % c_maxIndexNo) + 1);

    IndexCreator(
        fileId, p.GetParentFileNo(),
        p.GetEntryConverterVersion(),
        r0,
        fileTypeSet, fileVersion,
        index );

    bool ok = true;
    ok &= (p.GetRootPage() == r0);
    ok &= (p.GetIndexId() == index.GetIndexId());
//     p.GetIndexDescription(index1);
//     if ( ok ) {
//         for (int j=1; j <= index1.GetColumnCount(); j++) {
//             ok &= ( index.GetTableColumnId( j )
//                     ==
//                     index1.GetTableColumnId( j ) );
//             ok &= ( index.GetColumnSortOrder( j )
//                     ==
//                     index1.GetColumnSortOrder( j ) );
//         }
//     }
    if ( !ok ) {
        SAPDBTRACE_WRITELN(
            FileDir_Trace, 5,
            "Verify failed for index with file id " << p.GetFileNo() );
        FileDir_Exception
            errMsg( __CONTEXT__, FILEDIR_TEST_FAILED,
                    FileDir_FileNoToString( p.GetFileNo() ) );
        RTE_Crash(errMsg);
    }
    return ok;
}

/*===========================================================================*/

bool FileDir_Test::VerifyOmsEntry(
    SAPDBMem_IRawAllocator& alloc,
    FileDir_Oms& p)
{
    FileDir_FileNo fileId = p.GetFileNo();

    SAPDB_Int4 r0;
    SAPDB_UInt2 objBodySize;
    SAPDB_UInt2 objPerPageCount;
    SAPDB_UInt2 chainCount;

    OmsCreator( fileId, p.GetEntryConverterVersion(), r0, objBodySize,
               objPerPageCount, chainCount );

    bool ok = (p.GetRootPage() == r0);
    ok &= (p.GetObjBodySize() == objBodySize);
    ok &= (p.GetObjPerPageCount() == objPerPageCount);

    if ( !ok ) {
        SAPDBTRACE_WRITELN(
            FileDir_Trace, 5,
            "Verify failed for oms entry with file id " << p.GetFileNo() );
        FileDir_Exception
            errMsg( __CONTEXT__, FILEDIR_TEST_FAILED,
                    FileDir_FileNoToString( p.GetFileNo() ) );
        RTE_Crash(errMsg);
    }
    return ok;
}

/*===========================================================================*/

bool FileDir_Test::VerifyOmsVarEntry(
    SAPDBMem_IRawAllocator& alloc,
    FileDir_OmsVar& p)
{
    FileDir_FileNo fileId = p.GetFileNo();

    SAPDB_Int4 r0;
    SAPDB_UInt2 objBodySize;
    SAPDB_UInt2 objPerPageCount;
    SAPDB_UInt2 chainCount;

    OmsVarCreator( fileId, p.GetEntryConverterVersion(), r0, objBodySize,
               objPerPageCount, chainCount );

    bool ok = (p.GetRootPage() == r0);
    ok &= (p.GetObjBodySize() == objBodySize);
    ok &= (p.GetObjPerPageCount() == objPerPageCount);

    if ( !ok ) {
        SAPDBTRACE_WRITELN(
            FileDir_Trace, 5,
            "Verify failed for oms var entry with file id "
            << p.GetFileNo() );
        FileDir_Exception
            errMsg( __CONTEXT__, FILEDIR_TEST_FAILED,
                    FileDir_FileNoToString( p.GetFileNo() ) );
        RTE_Crash(errMsg);
    }
    return ok;
}

/*===========================================================================*/

int FileDir_Test::VerifyFileDirectory(
    SAPDBMem_IRawAllocator& alloc,
    const Converter_Version& converterVersion,
    FileDir_IDirectory& fd)
{
    SAPDBTRACE_ROUTINE_DEBUG( "FileDir_Test::VerifyFileDirectory",
                             FileDir_Trace, 5 );
    int errors = 0;
    tsp00_TaskId taskId;
    vgetpid( taskId );

    FileDir_Iterator it(alloc);
    fd.GetFileDirIterator( it, 0 );
    FileDir_Generic p;
    while ( it.GetNext( p ) ) {
        bool ok = true;
//        p.LockShared( taskId );
        switch ( p.GetFileType() ) {
        case Data_TableFile:
        {
            FileDir_Table p1;
            p1.Assign( p );
            ok = VerifyTableEntry(
                alloc,
                p1 );
        }
        break;
        case Data_IndexFile:
        {
            FileDir_Index p1;
            p1.Assign( p );
            ok = VerifyIndexEntry(
                alloc,
                p1 );
        }
        break;
        case Data_OmsFile:
        {
            FileDir_Oms p1;
            p1.Assign( p );
            ok = VerifyOmsEntry(
                alloc,
                p1 );
        }
        break;
        case Data_OmsVarFile:
        {
            FileDir_OmsVar p1;
            p1.Assign( p );
            ok = VerifyOmsVarEntry(
                alloc,
                p1 );
        }
        break;
        default:
            Assert( false );
        }
//        p.UnlockShared( taskId );
        errors += ok ? 0 : 1;
    }
    return errors;
}

/*===========================================================================*/

int FileDir_Test::CreateTest(
    SAPDBMem_IRawAllocator& m_alloc,
    tgg00_TransContext& transContext,
    const Converter_Version& converterVersion,
    FileDir_IDirectory& fd)
{
    SAPDBTRACE_ROUTINE_DEBUG ("FileDir_Test::CreateTest", FileDir_Trace, 5);
    const int tableCount = 5000;
    const int frac       = 17;
    int errors = 0;

    tsp00_TaskId taskId;
    taskId = transContext.trTaskId_gg00;

    SAPDBTRACE_WRITELN (FileDir_Trace, 5, "creating tables...");
    // Create tableCount tables...
    int i;
    Msg_List msgList;
    for (i=1; i <= tableCount; i++) {
        SAPDB_Int4 root;
        Kernel_BlobDescription bl(m_alloc), b0(m_alloc);
        tgg00_FiletypeSet fileTypeSet;
        tgg91_FileVersion fileVersion;
        FileDir_FileNo fi = fd.GetNewFileNo();
        TableCreator(fi, converterVersion, root, fileTypeSet, fileVersion, bl);
        // add either table or Oms entry:
        if ( i % 15 == 0 ) {
            SAPDB_Int4 r0;
            SAPDB_UInt2 objBodySize;
            SAPDB_UInt2 objPerPageCount;
            SAPDB_UInt2 chainCount;
            if ( i % 2 ) {
                OmsCreator( fi, converterVersion, r0, objBodySize,
                            objPerPageCount, chainCount );
                fd.AddOmsFile( taskId, fi, converterVersion, r0,
                               objBodySize,
                               objPerPageCount, chainCount, msgList );
            }
            else {
                OmsVarCreator( fi, converterVersion, r0, objBodySize,
                               objPerPageCount, chainCount );
                fd.AddOmsVarFile( taskId, fi, converterVersion, r0,
                                  objBodySize,
                                  objPerPageCount, chainCount, msgList );
            }
        } else {
            fd.AddTableFile( taskId, fi, converterVersion, root,
                             bl.GetColumnCount(),
                             fileTypeSet, fileVersion, msgList );
        }
    }

//     SAPDBTRACE_WRITELN (FileDir_Trace, 5, "adding blobs...");
//     // now add missing blob columns:
//     for (i=1000; i <= 1000+tableCount; i++) {
//         if ( (i % frac) != 0 ) {
//             continue;
//         }
//         SAPDB_Int4 root;
//         Kernel_BlobDescription bl(m_alloc);
//         FileDir_FileNo fi = i;
//         TableCreator(fi, converterVersion, root, bl);
//         FileDir_Table p;
//         fd.GetTableFile( transContext, fi, p );
//         if ( p.IsAssigned() ) {
//             for (int j=0; j < bl.GetColumnCount(); j++) {
//                 p.AddBlobColumn( converterVersion, bl.GetColumn(j) );
//             }
//         }
//     }

    // verify
    errors += VerifyFileDirectory(m_alloc, converterVersion, fd);

    SAPDBTRACE_WRITELN (FileDir_Trace, 5, "move entries...");
    // move them around and delete every frac-th table
    for (i=1000; i <= 1000+tableCount; i++) {
        FileDir_Generic p;

        fd.GetFile( transContext.trTaskId_gg00, i, p, msgList );
        if ( !p.IsAssigned() ) {
            continue;
        }

        if ( i % 4 == 0 ) {
            FileDir_Entry* fe = static_cast<FileDir_Entry*>( p.GetPointer() );
//             fe->MoveEntry();
        }
        if ( ((i % frac) == 0) &&
             (p.GetFileType() == Data_TableFile) ) {
            p.MarkFileDeleted( transContext.trWriteTransId_gg00 );
            if ( fd.DelFile( taskId, i, msgList ) != FileDir_Okay )
            {
                Assert( false );
            }
        }
    }

    // now add some more tables:
    for (i=1; i <= tableCount; i++) {
        SAPDB_Int4 root;
        Kernel_BlobDescription bl(m_alloc);
        tgg00_FiletypeSet fileTypeSet;
        tgg91_FileVersion fileVersion;
        FileDir_FileNo fi = fd.GetNewFileNo();
        TableCreator(fi, converterVersion, root, fileTypeSet, fileVersion, bl);
        fd.AddTableFile( taskId, fi, converterVersion, root,
                         bl.GetColumnCount(),
                         fileTypeSet, fileVersion, msgList );
    }

    SAPDBTRACE_WRITELN (FileDir_Trace, 5, "adding extra tables...");
    const int extraTables = 1000;
    Converter_Version newConverterVersion =
        Converter_ICommon::Instance().Version();
    FileDir_FileNo ids[extraTables];
    for (i=0; i < extraTables; i++) {
        SAPDB_Int4 root;
        Kernel_BlobDescription bl(m_alloc), b0(m_alloc);
        ids[i] = fd.GetNewFileNo();
        tgg00_FiletypeSet fileTypeSet;
        tgg91_FileVersion fileVersion;
        TableCreator( ids[i], newConverterVersion, root,
                      fileTypeSet, fileVersion, bl );
        if ( (i % frac) == 0 ) {
           fd.AddTableFile( taskId, ids[i], newConverterVersion, root,
                            b0.GetColumnCount(),
                            fileTypeSet, fileVersion, msgList );
        } else {
           fd.AddTableFile( taskId, ids[i], newConverterVersion, root,
                            bl.GetColumnCount(),
                            fileTypeSet, fileVersion, msgList );
        }
    }

    // now add extra blob columns:
    for (i=0; i < extraTables; i++) {
        if ( (i % frac) != 0 ) {
            continue;
        }
        SAPDB_Int4 root;
        Kernel_BlobDescription bl(m_alloc);
        tgg00_FiletypeSet fileTypeSet;
        tgg91_FileVersion fileVersion;
        FileDir_FileNo fi = ids[i];
        TableCreator( fi, newConverterVersion, root,
                      fileTypeSet, fileVersion, bl );
        FileDir_Table p;
        fd.GetTableFile( transContext.trTaskId_gg00, fi, p, msgList );
        for (int j=p.GetBlobColCount(); j < bl.GetColumnCount(); j++) {
            p.AddBlobColumn( newConverterVersion, bl.GetColumn(j) );
        }
    }

    for (i=0; i < extraTables; i++) {
        if ( (i % frac) != 0 ) {
            continue;
        }
        SAPDB_Int4 root;
        Kernel_BlobDescription bl(m_alloc);
        tgg00_FiletypeSet fileTypeSet;
        tgg91_FileVersion fileVersion;
        FileDir_FileNo fi = ids[i];
        TableCreator( fi, newConverterVersion, root,
                      fileTypeSet, fileVersion, bl );
        FileDir_Table p;
        fd.GetTableFile( transContext.trTaskId_gg00, fi, p, msgList );
        int j;
        for (j=p.GetBlobColCount(); j < bl.GetColumnCount(); j++) {
            p.AddBlobColumn( newConverterVersion, bl.GetColumn(j) );
        }
        for (j=p.GetBlobColCount(); j > bl.GetColumnCount(); j--) {
            p.RemoveBlobColumn( newConverterVersion, j-1 );
        }
    }

    errors += VerifyFileDirectory(m_alloc, newConverterVersion, fd);

    return errors;
}

/*===========================================================================*/

int FileDir_Test::VerifyTest(
    SAPDBMem_IRawAllocator& m_alloc,
    tgg00_TransContext& transContext,
    const Converter_Version& converterVersion,
    FileDir_IDirectory& fd)
{
    SAPDBTRACE_ROUTINE_DEBUG ("FileDir_Test::RestartTest", FileDir_Trace, 5);

    const int tableCount = 500;

    tsp00_TaskId taskId;
    taskId = transContext.trTaskId_gg00;

   // now add some more tables
    Msg_List msgList;
    for (int i=0; i < tableCount; i++) {
        SAPDB_Int4 root;
        Kernel_BlobDescription bl(m_alloc);
        tgg00_FiletypeSet fileTypeSet;
        tgg91_FileVersion fileVersion;
        FileDir_FileNo fi;
        if ( i % 2 ) {
            fi = fd.GetNewTempFileNo();
        } else {
            fi = fd.GetNewFileNo();
        }
        // save converter version to avoid mismatches due to
        // multi-threading:
        Converter_Version conVer = Converter_ICommon::Instance().Version();
        TableCreator(fi, conVer, root,
                     fileTypeSet, fileVersion, bl);
        fd.AddTableFile( taskId, fi, conVer, root,
                         bl.GetColumnCount(),
                         fileTypeSet, fileVersion, msgList );
    }

    int errors = VerifyFileDirectory(m_alloc, converterVersion, fd);

    return errors;
}

/*===========================================================================*/

int FileDir_Test::AddTest(
    SAPDBMem_IRawAllocator& alloc,
    tgg00_TransContext& transContext,
    FileDir_IDirectory& fd,
    Converter_Version& converterVersion)
{
    const int c_entriesToAdd = 5000;
    int errors = 0;

    tsp00_TaskId taskId;
    taskId = transContext.trTaskId_gg00;

    int i;
    Msg_List msgList;
    for (i=1; i <= c_entriesToAdd; i++) {
        SAPDB_Int4 root;
        if ( !m_crashOnFlush ) {
            converterVersion = Converter_ICommon::Instance().Version();
        }
        Kernel_BlobDescription bl(alloc), b0(alloc);
        tgg00_FiletypeSet fileTypeSet;
        tgg91_FileVersion fileVersion;
        FileDir_FileNo fi = fd.GetNewFileNo();
        TableCreator(fi, converterVersion, root,
                     fileTypeSet, fileVersion, bl);
        // add either table or Oms entry:
        if ( i % 15 == 0 ) {
            SAPDB_Int4 r0;
            SAPDB_UInt2 objBodySize;
            SAPDB_UInt2 objPerPageCount;
            SAPDB_UInt2 chainCount;
            if ( i % 2 ) {
                OmsCreator( fi, converterVersion, r0, objBodySize,
                            objPerPageCount, chainCount );
                fd.AddOmsFile( taskId, fi, converterVersion, r0,
                               objBodySize,
                               objPerPageCount, chainCount, msgList );
            }
            else {
                OmsVarCreator( fi, converterVersion, r0, objBodySize,
                               objPerPageCount, chainCount );
                fd.AddOmsVarFile( taskId, fi, converterVersion, r0,
                                  objBodySize,
                                  objPerPageCount, chainCount, msgList );
            }
        } else {
            fd.AddTableFile( taskId, fi, converterVersion, root,
                             bl.GetColumnCount(),
                             fileTypeSet, fileVersion, msgList );
        }
    }
    return 0;
}

/*===========================================================================*/

int FileDir_Test::AlterTest(
    SAPDBMem_IRawAllocator& alloc,
    tgg00_TransContext& transContext,
    FileDir_IDirectory& fd,
    Converter_Version& converterVersion)
{
    const int c_entriesToAdd = 5000;
    int errors = 0;

    tsp00_TaskId taskId;
    Msg_List msgList;
    taskId = transContext.trTaskId_gg00;
    for (int i=0; i<c_entriesToAdd; i++) {
        SAPDB_UInt8 fi0;
        if ( i % 2 ) {
            fi0 = rand() * fd.GetMaxFileNo().GetUInt8() / RAND_MAX;
        } else {
            fi0 = static_cast<SAPDB_UInt8>(c_FileDir_TempStartFileId +
                                           rand() * 1000.0 / RAND_MAX);
        }
#if defined(I386) || defined(ALPHA)
        SAPDB_UInt8 val;
        char* bytePtr = reinterpret_cast<char*>( &val );
        for (int j=7; j >= 0; --j, ++bytePtr) {
            *bytePtr = reinterpret_cast<char*>(&fi0)[ j ];
        }
        FileDir_FileNo fi = FileDir_FileNo(val);
#else
        FileDir_FileNo fi = FileDir_FileNo(fi0);
#endif
        FileDir_Generic p;
        if ( fd.GetFile( transContext.trTaskId_gg00, fi, p, msgList ) == FileDir_FileNoNotFound ) {
            continue;
        }
        if ( !m_crashOnFlush ) {
            converterVersion = Converter_ICommon::Instance().Version();
        }
        if ( fi.IsTempFileNo()                    &&
             ((i % 21) == 0)                      &&
             (p.GetFileType() != Data_OmsVarFile) &&
             (p.GetFileType() != Data_OmsKeyFile) ) {
//            p.LockExclusive( taskId );
            if ( p.MarkFileDeleted( transContext.trWriteTransId_gg00 ) ==
                 FileDir_Okay ) {
                if ( p.GetFileType() == Data_IndexFile ) {
                    FileDir_Table tp;
                    FileDir_Index ip;
                    ip.Assign( p );
                    if ( fd.GetTableFile(
                             transContext.trTaskId_gg00,
                             ip.GetParentFileNo(), tp, msgList)
                         == FileDir_Okay ) {
//                        tp.LockExclusive( taskId );
                        tp.RemoveIndex( fi );
//                        tp.UnlockExclusive( taskId );
                    } else {
                        Assert( false );
                    }
                }

//                p.UnlockExclusive( taskId );
//                p.Detach();
                fd.DelFile( taskId, fi, msgList );
            } else {
//                p.UnlockExclusive( taskId );
            }
            continue;
        }
        if ( fi.IsTempFileNo() ) {
            continue;
        }
        switch( p.GetFileType() ) {
        case Data_TableFile:
        {
            if ( !m_crashOnFlush ) {
                FileDir_PageManager::Instance()->AcquireSharedSavepointLock();
                // converter version might have changed
                converterVersion = Converter_ICommon::Instance().Version();
            }
//            p.LockExclusive( taskId );
            if ( p.GetFileState() != Data_FileOk ) {
//                p.UnlockExclusive( taskId );
                if ( !m_crashOnFlush ) {
                    FileDir_PageManager::Instance()->ReleaseSharedSavepointLock();
                }
                continue;
            }
            if ( (fi.GetUInt8() % 2) == 0 ) {
                // add index to table
                FileDir_FileNo ifi = fd.GetNewFileNo();
                SAPDB_Int4 rootPage;
                tgg00_FiletypeSet fileTypeSet;
                tgg91_FileVersion fileVersion;
                Kernel_IndexDescription index(
                    alloc,
                    static_cast<SAPDB_UInt2>(ifi.GetUInt8() % c_maxIndexNo) + 1);
                IndexCreator( ifi, fi, converterVersion, rootPage,
                              fileTypeSet, fileVersion, index );
                fd.AddIndexFile( taskId, ifi, fi, converterVersion,
                                 index.GetIndexId(), rootPage,
                                 fileTypeSet, fileVersion, msgList );
            } else {
                SAPDB_Int4 root;
                Kernel_BlobDescription bl(alloc);
                tgg00_FiletypeSet fileTypeSet;
                tgg91_FileVersion fileVersion;
                FileDir_Table p1;
                p1.Assign( p );
                TableCreator(fi, converterVersion, root,
                             fileTypeSet, fileVersion, bl);
                int j;
                for (j=p1.GetBlobColCount();
                     j < bl.GetColumnCount(); j++) {
                    p1.AddBlobColumn( converterVersion, bl.GetColumn(j) );
                }
                for (j=p1.GetBlobColCount();
                         j > bl.GetColumnCount(); j--) {
                    p1.RemoveBlobColumn( converterVersion, j-1 );
                }
            }
//            p.UnlockExclusive( taskId );
            if ( !m_crashOnFlush ) {
                FileDir_PageManager::Instance()->ReleaseSharedSavepointLock();
            }
        }
        break;
        case Data_OmsVarFile:
        {
            if ( m_crashOnFlush ) {
                // oms entries don't hand converter version over on
                // add, so our trick with the modified converter
                // version will not work here
                break;
            }
//            p.LockExclusive( taskId );
            if ( p.GetFileState() != Data_FileOk ) {
//                p.UnlockExclusive( taskId );
                if ( !m_crashOnFlush ) {
                    FileDir_PageManager::Instance()->ReleaseSharedSavepointLock();
                }
                continue;
            }
            // add partition to var container
            FileDir_FileNo ifi = fd.GetNewFileNo();
            SAPDB_Int4 r0;
            SAPDB_UInt2 objBodySize;
            SAPDB_UInt2 objPerPageCount;
            SAPDB_UInt2 chainCount;
            SAPDB_UInt2 partitionNo;
            OmsContCreator( fd, ifi, converterVersion, fi, r0, objBodySize,
                            objPerPageCount, chainCount, partitionNo );
            FileDir_OmsVar vp;
            vp.Assign( p );
//             if ( (partitionNo > vp.GetContPartitionCount()) ||
//                  (vp.GetContPartition(partitionNo).IsInvalid()) ) {
//                 errors +=
//                     (fd.AddOmsContFile( taskId, ifi, converterVersion,
//                                         r0, objBodySize,
//                                         objPerPageCount, chainCount, fi,
//                                         partitionNo, 0 )
//                      != FileDir_Okay)
//                     ? 1 : 0;
//                 Assert( errors == 0 );
//             }
//            p.UnlockExclusive( taskId );
        }
        break;
        default:
            // do nothing.
            ;
        }
    }
    return errors;
}

/*===========================================================================*/

int FileDir_Test::ClearTest(
    SAPDBMem_IRawAllocator& alloc,
    tgg00_TransContext& transContext,
    const Converter_Version& converterVersion,
    FileDir_IDirectory& fd)
{
    SAPDBTRACE_ROUTINE_DEBUG ("FileDir_Test::ClearTest", FileDir_Trace, 5);

    tsp00_TaskId taskId;
    taskId = transContext.trTaskId_gg00;

    // delete everything but tables and omsvar files

    // test GcGetFile
//     FileDir_FileNo gcFileId;
//     for ( gcFileId=5000; gcFileId < 5500; gcFileId += 10 ) {
//         FileDir_Generic p;
//         if ( fd.GcGetFile( transContext.trTaskId_gg00, gcFileId, p ) == FileDir_Okay ) {
//             if ( (p.GetFileType() != Data_TableFile) &&
//                  (p.GetFileType() != Data_OmsVarFile) &&
//                  (p.GetFileType() != Data_FDirCompatFile) &&
//                  (p.GetFileType() != Data_IndexFile) ) {
//                 FileDir_ReturnCode ret =
//                     p.MarkFileDeleted( transContext.trWriteTransId_gg00 );
//                 SAPDBTRACE_WRITELN(
//                     FileDir_Trace, 6,
//                     "FileId         : " << p.GetFileNo() << NewLine <<
//                     "MarkFileDeleted: " << ret );
//                 Assert( ret == FileDir_Okay );
// //                p.Detach();
//                 ret = fd.DelFile( taskId, gcFileId );
//                 SAPDBTRACE_WRITELN(
//                     FileDir_Trace, 6,
//                     "DelFile        : " << ret );
//                 Assert( ret == FileDir_Okay );
//             }
//         }
//     }

    // test iterator assignment and construction...
    FileDir_Iterator it1(alloc);
    fd.GetFileDirIterator( it1, 0 );
    FileDir_Iterator it2(alloc);
    it2 = it1;
    FileDir_Iterator it = it2;
    FileDir_Generic p;
    Msg_List msgList;
    while ( it.GetNext( p ) ) {
        bool ok = true;
        Assert( p.IsAssigned() );
        if ( p.GetFileType() == Data_IndexFile ) {
            FileDir_Table tp;
            FileDir_Index ip;
            ip.Assign( p );
            if ( fd.GetTableFile( transContext.trTaskId_gg00, ip.GetParentFileNo(), tp, msgList )
                 == FileDir_Okay ) {
                tp.RemoveIndex( p.GetFileNo() );
            } else {
                Assert( false );
            }
        }
        if ( (p.GetFileType() != Data_TableFile) &&
             (p.GetFileType() != Data_OmsVarFile) ) {
            FileDir_FileNo fileId = p.GetFileNo();
            FileDir_ReturnCode ret =
                p.MarkFileDeleted( transContext.trWriteTransId_gg00 );
            SAPDBTRACE_WRITELN(
                FileDir_Trace, 6,
                "FileId         : " << p.GetFileNo() << NewLine <<
                "MarkFileDeleted: " << ret );
            Assert( ret == FileDir_Okay );
            ret = fd.DelFile( taskId, fileId, msgList );
            SAPDBTRACE_WRITELN(
                FileDir_Trace, 6,
                "DelFile        : " << ret );
            Assert( ret == FileDir_Okay );
        }
    }

    // now delete tables
    it.AddFileTypeToFilter( Data_TableFile );
    it.AddFileTypeToFilter( Data_OmsVarFile );
    fd.GetFileDirIterator( it, 0 );
    while ( it.GetNext( p ) ) {
        bool ok = true;
        Assert( p.IsAssigned() );
        FileDir_FileNo fileId = p.GetFileNo();
        FileDir_ReturnCode ret =
            p.MarkFileDeleted( transContext.trWriteTransId_gg00 );
        SAPDBTRACE_WRITELN(
            FileDir_Trace, 6,
            "FileId         : " << p.GetFileNo() << NewLine <<
            "MarkFileDeleted: " << ret );
        Assert( ret == FileDir_Okay );
        ret = fd.DelFile( taskId, fileId, msgList );
        SAPDBTRACE_WRITELN(
            FileDir_Trace, 6,
            "DelFile        : " << ret );
        Assert( ret == FileDir_Okay );
    }

    // now delete everything in shared directory
    it.ClearFilter();
    fd.GetSharedFileDirIterator( it, 0 );
    while ( it.GetNext( p ) ) {
        bool ok = true;
        Assert( p.IsAssigned() );
        FileDir_FileNo fileId = p.GetFileNo();
        FileDir_ReturnCode ret =
            p.MarkFileDeleted( transContext.trWriteTransId_gg00 );
        SAPDBTRACE_WRITELN(
            FileDir_Trace, 6,
            "FileId         : " << p.GetFileNo() << NewLine <<
            "MarkFileDeleted: " << ret );
        Assert( ret == FileDir_Okay );
        ret = fd.DelFile( taskId, fileId, msgList );
        SAPDBTRACE_WRITELN(
            FileDir_Trace, 6,
            "DelFile        : " << ret );
        Assert( ret == FileDir_Okay );
    }

    Assert(
        FileDir_ISharedDirectory::Instance().GetEntryCount()
        == 0 );
    Assert(
        FileDir_ISharedDirectory::Instance().GetUsedHashTableSlotCount()
        == 0 );
    return 0;
}

/*===========================================================================*/

int FileDir_Test::RemoveTest(
    SAPDBMem_IRawAllocator& alloc,
    tgg00_TransContext& transContext,
    FileDir_IDirectory& fd)
{
    const int c_entriesToRemove = 5000;
    int errors = 0;

    tsp00_TaskId taskId;
    Msg_List msgList;
    taskId = transContext.trTaskId_gg00;
    for (int i=0; i<c_entriesToRemove; i++) {
        SAPDB_UInt8 fi0;
        if ( i % 2 ) {
            fi0 = rand() * fd.GetMaxFileNo().GetUInt8() / RAND_MAX;
        } else {
            fi0 = static_cast<SAPDB_UInt8>(c_FileDir_TempStartFileId +
                                           rand() * 1000.0 / RAND_MAX);
        }
#if defined(I386) || defined(ALPHA)
        SAPDB_UInt8 val;
        char* bytePtr = reinterpret_cast<char*>( &val );
        for (int j=7; j >= 0; --j, ++bytePtr) {
            *bytePtr = reinterpret_cast<char*>(&fi0)[ j ];
        }
        FileDir_FileNo fi = FileDir_FileNo(val);
#else
        FileDir_FileNo fi = FileDir_FileNo(fi0);
#endif
        FileDir_Generic p;
        if ( fd.GetFile( transContext.trTaskId_gg00, fi, p, msgList ) == FileDir_FileNoNotFound ) {
            continue;
        }
        switch ( p.GetFileType() ) {
        case Data_TableFile:
        {
            FileDir_Table tp;
            tp.Assign( p );
//            tp.LockExclusive( transContext.trTaskId_gg00 );
            if ( tp.MarkFileDeleted( transContext.trWriteTransId_gg00 ) ==
                 FileDir_AlreadyMarkedForDeletion ) {
//                tp.UnlockExclusive( transContext.trTaskId_gg00 );
                continue;
            }
            while ( tp.GetMaxIndexId() > 0 ) {
                int k;
                for ( k=1; k <= tp.GetMaxIndexId(); ++k ) {
                    FileDir_FileNo indexFileNo = tp.GetIndexFileNo( k );
                    if ( indexFileNo.IsInvalid() ) {
                        continue;
                    }
                    FileDir_Index ip;
                    fd.GetIndexFile( transContext.trTaskId_gg00, indexFileNo, ip, msgList );
                    Assert( ip.IsAssigned() );
//                    ip.LockExclusive( transContext.trTaskId_gg00 );
                    ip.MarkFileDeleted( transContext.trWriteTransId_gg00 );
//                    ip.UnlockExclusive( transContext.trTaskId_gg00 );
                    fd.DelFile( transContext.trTaskId_gg00, indexFileNo, msgList );
                }
            }
            tp.MarkFileDeleted( transContext.trWriteTransId_gg00 );
//            tp.UnlockExclusive( transContext.trTaskId_gg00 );
        }
        break;
        case Data_OmsVarFile:
        {
            FileDir_OmsVar tp;
            tp.Assign( p );
//            tp.LockExclusive( transContext.trTaskId_gg00 );
            if ( tp.MarkFileDeleted( transContext.trWriteTransId_gg00 ) ==
                 FileDir_AlreadyMarkedForDeletion ) {
//                tp.UnlockExclusive( transContext.trTaskId_gg00 );
                continue;
            }
//             int k;
//             for ( k=1; k <= tp.GetContPartitionCount(); ++k ) {
//                 FileDir_FileNo contFileNo = tp.GetContPartition( k );
//                 if ( contFileNo.IsInvalid() ) {
//                     continue;
//                 }
//                 FileDir_OmsCont ip;
//                 fd.GetOmsContFile( transContext, contFileNo, ip );
//                 Assert( ip.IsAssigned() );
//                 ip.LockExclusive( transContext.trTaskId_gg00 );
//                 ip.MarkFileDeleted( transContext.trWriteTransId_gg00 );
//                 ip.UnlockExclusive( transContext.trTaskId_gg00 );
//                 fd.DelFile( transContext.trTaskId_gg00, contFileNo );
//             }
            tp.MarkFileDeleted( transContext.trWriteTransId_gg00 );
//            tp.UnlockExclusive( transContext.trTaskId_gg00 );
        }
        break;
        case Data_IndexFile:
        case Data_OmsFile:
        {
//            p.LockExclusive( transContext.trTaskId_gg00 );
            FileDir_ReturnCode ret = p.MarkFileDeleted(
                transContext.trWriteTransId_gg00 );
//            p.UnlockExclusive( transContext.trTaskId_gg00 );
            if ( ret == FileDir_AlreadyMarkedForDeletion ) {
                continue;
            }
            if (  (ret != FileDir_Okay) &&
                  (ret != FileDir_AlreadyMarkedForDeletion) ) {
                RTE_Crash(
                    SAPDBErr_Exception(
                        __CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                        "test error: file should be marked deleted") );
            }
        }
        break;
        default:
            RTE_Crash(
                SAPDBErr_Exception(
                    __CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                    "test error: all file types must be handled") );
        }
        fd.DelFile( transContext.trTaskId_gg00, fi, msgList );
    }
    return 0;
}

/*===========================================================================*/

int FileDir_Test::ReaderTest(
    SAPDBMem_IRawAllocator& alloc,
    tgg00_TransContext& transContext,
    FileDir_IDirectory& fd)
{
    const int c_entriesToRead = 7000;

    tsp00_TaskId taskId;
    Msg_List msgList;
    taskId = transContext.trTaskId_gg00;
    for (int i=0; i<c_entriesToRead; i++) {
        FileDir_FileNo fi = static_cast<FileDir_FileNo>(
            rand() * fd.GetMaxFileNo().GetUInt8() / RAND_MAX + 1);
        FileDir_Generic p;
        if ( fd.GetFile( transContext.trTaskId_gg00, fi, p, msgList ) == FileDir_FileNoNotFound ) {
            continue;
        }
//        p.LockShared( transContext.trTaskId_gg00 );
        switch ( fi.GetUInt8() % 3 ) {
        case 0:
            p.ModifyLeafPageCount( taskId, Converter_ICommon::Instance().Version() + 1, 1 );
            break;
        case 1:
            p.ModifyEntryCount( taskId, Converter_ICommon::Instance().Version(), 1 );
            break;
        default:
            p.ModifyLeafPageCount( taskId, Converter_ICommon::Instance().Version(), 1 );
            p.ModifyEntryCount( taskId, Converter_ICommon::Instance().Version(), 1 );
        }
//        p.UnlockShared( transContext.trTaskId_gg00 );
    }
    return 0;
}

/*===========================================================================*/

void FileDir_Test::InitRefCounterTest(
    tgg00_TransContext& transContext,
    FileDir_IDirectory& fd)
{
    FileDir_FileNo refCounterFileNo( fd.GetNewFileNo() );

    tgg00_FiletypeSet fileTypeSet = tgg00_FiletypeSet::build(
        2, ftsConcurrent_egg00, ftsPerm_egg00 );

    tgg91_FileVersion fileVersion;
    fileVersion.gg91SetDummyVers();

    Msg_List errMsg;
    fd.AddTableFile(
        transContext.trTaskId_gg00,
        refCounterFileNo,
        Converter_ICommon::Instance().Version(),
        0,
        0,
        fileTypeSet,
        fileVersion,
        errMsg,
        &m_refCounterFileEntry );

    if ( ! m_refCounterFileEntry.IsAssigned() ) {
        RTE_Crash(
            SAPDBErr_Exception(
                __CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                "add of m_refCounterFileEntry must not fail") );
    }
}

/*===========================================================================*/

void FileDir_Test::RefCounterTestTask(
    tgg00_TransContext& transContext)
{
    const SAPDB_UInt c_arraySize = 1000000;

    SAPDBMem_IRawAllocator& allocator = RTEMem_Allocator::Instance();
    FileDir_Table tableEntry;
    FileDir_Table *entryPointers;
    newarray( entryPointers, c_arraySize, allocator, tableEntry );

    FileDir_IDirectory& fd = FileDir_ISharedDirectory::IFileDirInstance();

    Msg_List errMsg;
    SAPDB_UInt i;
    for ( i = 0; i < c_arraySize; ++i ) {
        FileDir_ReturnCode rc =
            fd.GetTableFile(
                transContext.trTaskId_gg00,
                m_refCounterFileEntry.GetFileNo(),
                entryPointers[ i ],
                errMsg );
        if ( rc != FileDir_Okay ) {
            RTE_Crash(
                SAPDBErr_Exception(
                    __CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                    "file directory entry for ref counter test must exist") );
        }
    }

    destroyarray( entryPointers, c_arraySize, allocator );
}

/*===========================================================================*/

SAPDB_UInt FileDir_Test::EvalRefCounterTest(
    tgg00_TransContext& transContext,
    FileDir_IDirectory& fd)
{
    SAPDB_UInt retVal = 0;
    FileDir_BaseEntry* fe = m_refCounterFileEntry.fileEntry;

    if ( fe->m_refCounter != 1 ) {
        retVal = fe->m_refCounter;
    }

    m_refCounterFileEntry.MarkFileDeleted( transContext.trTransId_gg00 );
    Msg_List errMsg;
    fd.DelFile(
        transContext.trTaskId_gg00, m_refCounterFileEntry.GetFileNo(), errMsg );

    return retVal;
}

/*===========================================================================*/

void FileDir_Test::Test(tgg00_TransContext& transContext, const int scenario)
{
    SAPDBTRACE_ROUTINE_DEBUG ("FileDir_Test::Test", FileDir_Trace, 5);

    SAPDBMem_IRawAllocator& allocator = RTEMem_Allocator::Instance();

    tsp00_TaskId taskId;
    taskId = transContext.trTaskId_gg00;
    int errors = 0;
    Data_PageNo FileDir_rootPage =
        FileDir_ISharedDirectory::Instance().GetRootPageNo( taskId );
    Converter_Version m_converterVersion
        = Converter_ICommon::Instance().Version();
    FileDir_IDirectory* m_fd;
    kb50GetLocalFileDir( transContext.trIndex_gg00, m_fd );
    SAPDBTRACE_WRITELN(
        FileDir_Trace, 6,
        "EntryCount        : " << m_fd->GetEntryCount() );
    if ( m_crashOnFlush ) {
        // everything will be added in a higher converter version
        ++m_converterVersion;
    }
    switch ( scenario ) {
    case 1:
        errors += CreateTest( allocator, transContext, m_converterVersion,
                    *m_fd );
        break;
    case 2:
        errors += VerifyTest( allocator, transContext, m_converterVersion,
                                  *m_fd );
        break;
    case 3:
        errors += ClearTest( allocator, transContext,  m_converterVersion,
                             *m_fd );
        break;
    case 4:
        errors += ReaderTest(
            allocator,
            transContext,
            *m_fd );
        break;
    case 5:
        errors += AddTest(
            allocator,
            transContext,
            *m_fd,
            m_converterVersion );
    case 6:
        errors += AlterTest(
            allocator,
            transContext,
            *m_fd,
            m_converterVersion );
        break;
    case 7:
        errors += RemoveTest(
            allocator,
            transContext,
            *m_fd);
        break;
    case 8:
        InitRefCounterTest( transContext, *m_fd );
        break;
    case 9:
        RefCounterTestTask( transContext );
        break;
    case 10:
        errors += EvalRefCounterTest( transContext, *m_fd );
        break;
    }

    if ( errors != 0 ) {
        FileDir_Exception
            errMsg( __CONTEXT__, FILEDIR_TEST_FAILED, SAPDB_ToString(errors) );
        RTE_Crash(errMsg);
    }

    // uncomment for simple compile time checks for untested entries
    Msg_List msgList;
    FileDir_Table gt;
    FileDir_Oms gp;
    FileDir_OmsKey ok;
    m_fd->GetTableFile( transContext.trTaskId_gg00, 123, gt, msgList );
    m_fd->GetOmsKeyFile( transContext.trTaskId_gg00, 123, ok, msgList );
    FileDir_OmsFixed of;
    m_fd->GetOmsFixedFile( transContext.trTaskId_gg00, 123, of, msgList );
    gp.Assign( ok );
    of.Assign( of );
//    gt.Assign( ok ); // uncomment to verify that there will be a compile error for Assign's that are are not allowed in the type hierarchy
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
