/*!
  @file     FileDir_Wrapper.cpp
  @author   MartinKi
  @ingroup  FileDirectory_Files
  @brief    This module is used access c++ code from pascal code

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SQLManager/SQLMan_Context.hpp"
#include "SQLManager/ErrorHandling/SQLManErr_Instance.hpp"
#include "FileDirectory/FileDir_Table.hpp"
#include "FileDirectory/FileDir_Index.hpp"
#include "FileDirectory/FileDir_ShortColumn.hpp"
#include "FileDirectory/FileDir_Generic.hpp"
#include "FileDirectory/FileDir_IDirectory.hpp"
#include "FileDirectory/FileDir_ISharedDirectory.hpp"
#include "FileDirectory/FileDir_SharedDirectory.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "SAPDBCommon/Messages/SDBMsg_FileDir.h"

// for migration:
#include "FileDirectory/FileDir_BaseEntry.hpp"
#include "FileDirectory/FileDir_Entry.hpp"
#include "FileDirectory/FileDir_TableEntry.hpp"
#include "FileDirectory/FileDir_Exception.hpp"
#include "FileDirectory/FileDir_Messages.hpp"
#include "hbd998.h"

#include "Transaction/Trans_Context.hpp"
#include "Converter/Converter_ICommon.hpp"

#include "KernelCommon/Kernel_VTrace.hpp"

#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include "RunTime/RTE_Message.hpp"
#include "RunTime/RTE_Crash.hpp"

#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Hex.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"


#include "gbd00.h"
#include "ggg00.h"
#include "gsp00.h"
#include "hkb50.h"
#include "hkb57.h"
#include "hkb64.h"
#include "hkb900.h"
#include "hbd30.h"
#include "hbd01_1.h"
#include "hgg06.h"
#include "hgg17.h"
#include "ggg91.h"

// vgetpid():
#include "heo51.h"

#include <stdlib.h> /* nocheck */

/// >bd998CreateTempFile
#include "hbd01.h"
#include "hbd13.h"
#include "hbd17.h"
#include "ggg11.h"
#include "hgg04.h"
#include "hgg01_3.h"
#include "hta01.h"
#include "DataAccess/Data_TempFileTypes.hpp"
#include "Trace/Trace_Entry.hpp"
/// <bd998CreateTempFile

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

const SAPDB_Bool c_only_empty = true;

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/// Helper class to lock/release files on construction/destruction.
class FileDir_WrapperLockedTreeScope
{
private:
    tbd_current_tree& m_current;
    bool              m_lockSet;

public:
    enum LockType {
        LockExclusive,
        LockNonExclusive
    };

    FileDir_WrapperLockedTreeScope(
        tgg00_TransContext& trans,
        tgg00_FileId&       fileId,
        tbd_current_tree&   current,
        LockType            lock)
        : m_current( current )
    {
        if ( trans.trError_gg00 != e_ok ) {
            return;
        }

        bd30GetTree( trans, fileId, m_current, m_select,
                 (lock == LockExclusive), SYNC_TREE_LOCK_BD00 );

        m_lockSet = true; // must be set even if trError_gg00 != 0
    }

    ~FileDir_WrapperLockedTreeScope()
    {
        this->Release();
    }

    void Release()
    {
        if ( m_lockSet ) {
            bd30ReleaseTree( m_current );
        }
        m_lockSet = false;
    }
};



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

static void GetFileinfo(
    FileDir_Generic& fdEntry,
    tbd_fileinfo&    fileInfo);

/*!
 * @brief Maps a FileDir_ReturnCode to a suitable basis error.
 *
 * @param retCode [in] file directory error to be mapped
 * @return suitable basis error
 */
static tgg00_BasisError MapFileDirReturnCode(const FileDir_ReturnCode retCode);

/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/

/// Returns the session local file directory if possible. Otherwise
/// the shared file directory is returned.
inline static FileDir_IDirectory& GetFileDirectory(
    const tgg00_TransContext trans)
{
    if (trans.trLocalFileDirPtr_gg00 != 0) {
        return FileDir_IDirectory::SessionInstance( trans );
    } else {
        return FileDir_IDirectory::Instance( trans.trIndex_gg00 );
    }
}

static char* FileTypeSetToString(
    tgg00_FiletypeSet fileTypeSet,
    tsp00_Line*       line)
{
    const pasbool c_isEditMode = true;
    SAPDB_Int     lineLen = 0;
    char*         pLine   = reinterpret_cast<char*>( line );

    g17filetype_to_line( fileTypeSet, ! c_isEditMode, lineLen, *line );
    pLine[ lineLen ] = '\0';
    return pLine;
}

/// Appends a given message list to the SQL-context if the SQL context
/// is available.
inline static void AppendToContextMessageList(const Msg_List& msgList)
{

/***************************************************************************/
// disabled until resetting of errors and messages in upper layers is
// handled properly
/***************************************************************************/

//     SQLMan_Context* context = SQLMan_Context::GetContext();
//     if ( context ) {
//         context->GetErrorList().AppendNewMessage( msgList );
//     }
}

static tgg00_BasisError MapFileDirReturnCode(const FileDir_ReturnCode retCode)
{
    tgg00_BasisError trError;

    switch ( retCode ) {
        case FileDir_Okay:
            trError = e_ok;
            break;
        case FileDir_FileNoNotFound:
            trError = e_file_not_found;
            break;
        case FileDir_FileNoExists:
            trError = e_duplicate_filename;
            break;
        case FileDir_FileTypeMismatch:
            trError = e_invalid_filetype;
            break;
        case FileDir_OutOfMemory:
            trError = e_no_more_memory;
            break;
        case FileDir_SystemError:
            trError = e_system_error;
            break;
    default:
        SAPDBERR_ASSERT_STATE( false );
        trError = e_system_error;
    }
    return trError;
}

/*************************************************************************/

static SAPDB_Bool
bd998GetFile(
    tgg00_TransContext&    trans,
    const tgg00_Surrogate& tableSurr,
    FileDir_Generic&       tableEntry)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998GetFile", FileDir_Trace, 5 );

    Trans_Context&      transContext = Trans_Context::GetContext( trans );
    FileDir_IDirectory& fdir = FileDir_IDirectory::SessionInstance( trans );

    const FileDir_ReturnCode rc =
        fdir.GcGetFile(
            transContext.TaskId(),
            tableSurr,
            tableEntry,
            transContext.ErrorList() );

    if ( FileDir_Okay !=  rc ) {

        trans.trError_gg00 = MapFileDirReturnCode( rc );
        return false;

    }

    return true;
}

/*************************************************************************/

static SAPDB_Bool
bd998GetTableFile(
    tgg00_TransContext&    trans,
    const tgg00_Surrogate& tableSurr,
    FileDir_Table&         tableEntry)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998GetTableFile", FileDir_Trace, 5 );

    Trans_Context&      transContext = Trans_Context::GetContext( trans );
    FileDir_IDirectory& fdir = FileDir_IDirectory::SessionInstance( trans );

    const FileDir_ReturnCode rc =
        fdir.GetTableFile(
            transContext.TaskId(),
            tableSurr,
            tableEntry,
            transContext.ErrorList() );

    if ( FileDir_Okay !=  rc ) {

        trans.trError_gg00 = MapFileDirReturnCode( rc );
        return false;

    }

    return true;
}

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

/*---------------------------------------------------------------------------*/

/*!
 * @brief Create a new file directory with root page rootPageNo.
 */
externC void bd998CreateFDir( tsp00_TaskId   taskId )
{
    FileDir_ISharedDirectory::Instance().Create( taskId );
}

/*---------------------------------------------------------------------------*/

/*!
 * @brief Restarts the file directory.
 */
externC void bd998RestartFDir(
    tgg00_TransContext    VAR_VALUE_REF  trans,
    tsp00_PageNo   rootPageNo,
    tsp00_Int4     converterVersion,
    tsp00_Int4     restartConverterVersion,
    pasbool        cleanupFileDirPages)
{
    SAPDBTRACE_ROUTINE_DEBUG("bd998RestartFDir", FileDir_Trace, 5 );

    Msg_List msgList;

    FileDir_ReturnCode ret =
        FileDir_ISharedDirectory::Instance().RestartFDir(
            trans,
            rootPageNo,
            converterVersion,
            restartConverterVersion,
            cleanupFileDirPages,
            msgList);

    if ( (FileDir_Okay != ret) && (FileDir_AlreadyRestarted != ret) ) {
        if ( trans.trError_gg00 == e_ok ) {
            trans.trError_gg00 = e_bad_fdir;
        }

        // TODO: attach message list to context
        if ( ! msgList.IsEmpty() ) {
            RTE_Message( msgList );
            msgList.ClearMessageList();
        }
    }
}

/*---------------------------------------------------------------------------*/

/*!
 * @brief Initiates a shutdown of the file directory.
 */
externC void bd998ShutdownFDir( tsp00_TaskId   taskId )
{
    FileDir_ISharedDirectory::Instance().ShutdownSharedFDir( taskId );
}

/*---------------------------------------------------------------------------*/

/*!
 * @brief Flush of file directory
 */

externC void bd998FlushFDir( tsp00_TaskId   taskId )
{
    FileDir_ISharedDirectory::Instance().Flush( taskId );
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/*!
 * @brief Tests if a file name belongs to a temporary file.
 *
 * @param fn [in] file name
 *
 * @return
 * - true, if fn is a file name for a temporary file
 * - false, if fn is a file name for a persistent file
 */
static bool IsTemp(const tgg00_Filename& fn)
{
    return
        ( fn[0] == tfnTemp_egg00 )      ||
        ( fn[0] == tfnTempAux_egg00 )   ||
        ( fn[0] == tfnTempMulti_egg00 ) ||
        ( fn[0] == tfnTempOms_egg00 );
}

/*************************************************************************/

/// Returns true if the file name indicates that the corresponding
/// file should be handled in the new file directory.
static bool UseNewFDir(const tgg00_Filename& fn)
{
    return
        (fn[0] != tfnAux_egg00)
        && (fn[0] != tfnTempAux_egg00);
}

/*************************************************************************/

/*!
 * @brief Returns the file info for fdEntry, if it exists
 */
static void GetFileinfo(
    FileDir_Generic& fdEntry,
    tbd_fileinfo&    fileInfo)
{
    switch ( fdEntry.GetFileType() ) {
    case Data_TableFile:
    {
        FileDir_Table ft;
        ft.Assign( fdEntry );
        ft.FillFileInfo( fileInfo );
        break;
    }
    case Data_IndexFile:
    {
        FileDir_Index ft;
        ft.Assign( fdEntry );
        ft.FillFileInfo( fileInfo );
        break;
    }
    case Data_ShortColumnFile:
    {
        FileDir_ShortColumn fs;
        fs.Assign( fdEntry );
        fs.FillFileInfo( fileInfo );
        break;
    }
    case Data_OmsFixedFile:
    case Data_OmsKeyFile:
    case Data_OmsKeyPartitionFile:
    case Data_OmsVarFile:
    case Data_OmsContFile:
    {
        fileInfo.fi_type    = tgg00_FiletypeSet::build(
            3, ftsObject_egg00, ftsConcurrent_egg00, ftsPerm_egg00);
        fileInfo.fi_state.clear();
        fileInfo.fi_descr.clear();
        fileInfo.fi_root    = fdEntry.GetRootPage();
        fileInfo.fi_col_cnt = cgg_nil_varcol_cnt_off;
        fileInfo.fi_user_ref.gg91SetNilRef();
        fileInfo.fi_var_col_cnt_pos = 0;
        fileInfo.fi_fill1           = 0;
        fileInfo.fi_vers.gg91SetDummyVers();
        break;
    }
    default:
        RTE_Crash(
            SAPDBErr_Exception(
                __CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                "unknown file type") );
    }
}

/*************************************************************************/

/*!
 * @brief Adds an old style file directory entry to the new file directory.
 */
externC void bd998AddToFDir( tgg00_TransContext VAR_VALUE_REF t,
                             tgg00_Filename     VAR_ARRAY_REF filename,
                             tbd_fileinfo       VAR_VALUE_REF fileinfo )
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998AddToFDir", FileDir_Trace, 5 );

    SAPDBERR_ASSERT_STATE( UseNewFDir( filename ) );
    SAPDBERR_ASSERT_STATE( ! fileinfo.fi_type.includes(ftsByteStr_egg00) );

    if ( (filename[0] == tfnMulti_egg00) ||
         (filename[0] == tfnShortScol_egg00) ) {
        // index or short column file cannot be added via vbd17
        return;
    }

    bool isPerm =
        fileinfo.fi_type.includes(ftsPerm_egg00)    ||
        fileinfo.fi_type.includes(ftsObject_egg00);

    tgg00_Surrogate file_surr(
        *reinterpret_cast<tgg00_Surrogate*>( &filename[2] ) );

    if ( isPerm || (!fileinfo.fi_type.includes( ftsShared_egg00 )) ) {
        bd998AddTableToFdir(
            t,
            file_surr,
            Converter_ICommon::Instance().Version(),
            fileinfo.fi_root,
            fileinfo.fi_col_cnt,
            fileinfo.fi_type,
            fileinfo.fi_vers);
    } else {
        bd998AddSharedTempTableToFdir (
            t,
            file_surr,
            fileinfo.fi_root,
            fileinfo.fi_col_cnt,
            fileinfo.fi_type,
            fileinfo.fi_vers);
    }
}

/*************************************************************************/


/*!
 * @brief Retrieves an old style file directory entry from the new
 *        file directory and creates a handle for it.
 */
externC void bd998GetFDirEntry(
    tgg00_TransContext VAR_VALUE_REF t,
    tgg00_Filename     VAR_ARRAY_REF filename,
    tbd_fileinfo       VAR_VALUE_REF fileinfo,
    pasbool                          shared_file,
    tsp00_MoveObjPtr   VAR_VALUE_REF entryhandle)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998GetFDirEntry", FileDir_Trace, 5 );

    SAPDBERR_ASSERT_STATE( UseNewFDir( filename ) );

    t.trError_gg00 = e_ok;

    entryhandle = 0;
    Msg_List msgList;
    bool isPerm = ! IsTemp( filename );

    FileDir_IDirectory& fd = GetFileDirectory( t );

    SAPDBMem_IRawAllocator& alloc = fd.GetAllocator();
    FileDir_Generic fg;
    FileDir_FileNo fileNo (
        *reinterpret_cast<tgg00_Surrogate*>( &filename[2] ) );

    FileDir_ReturnCode retCode =
        fd.GetFile( t.trTaskId_gg00, fileNo, fg, msgList );
    if ( fg.IsAssigned() ) {
        GetFileinfo( fg, fileinfo );
    }

    FileDir_Generic* fgp = new (alloc) FileDir_Generic();
    if ( fgp == 0 ) {
        AppendToContextMessageList( msgList );
        t.trError_gg00 = e_no_more_memory;
        return;
    }
    fgp->Assign( fg );

    if ( fgp->IsAssigned() ) {
        entryhandle = reinterpret_cast<tsp00_MoveObjPtr>(fgp);

        gg06SetNilRef( fileinfo.fi_user_ref );

    } else {
        t.trError_gg00 = MapFileDirReturnCode( retCode );
        destroy( fgp, alloc );
    }
    AppendToContextMessageList( msgList );
}

/*!
 * @brief Retrieves an old style file directory entry from the new
 *        file directory and creates a handle for it.
 *
 * This method should be used, if the file info of a file that might
 * be marked as deleted is to be retrieved.
 */
externC void bd998GcGetFDirEntry(
    tgg00_TransContext VAR_VALUE_REF t,
    tgg00_Filename     VAR_ARRAY_REF filename,
    tbd_fileinfo       VAR_VALUE_REF fileinfo,
    pasbool                          shared_file,
    tsp00_MoveObjPtr   VAR_VALUE_REF entryhandle)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998GetFDirEntry", FileDir_Trace, 5 );

    SAPDBERR_ASSERT_STATE( UseNewFDir( filename ) );

    t.trError_gg00 = e_ok;

    entryhandle = 0;
    Msg_List msgList;
    bool isPerm = ! IsTemp( filename );

    FileDir_IDirectory& fd = GetFileDirectory( t );

    SAPDBMem_IRawAllocator& alloc = fd.GetAllocator();
    FileDir_Generic fg;
    FileDir_FileNo fileNo (
        *reinterpret_cast<tgg00_Surrogate*>( &filename[2] ) );

    FileDir_ReturnCode retCode =
        fd.GetFile( t.trTaskId_gg00, fileNo, fg, msgList );
    if ( fg.IsAssigned() ) {
        GetFileinfo( fg, fileinfo );
    }

    FileDir_Generic* fgp = new (alloc) FileDir_Generic();
    if ( fgp == 0 ) {
        AppendToContextMessageList( msgList );
        t.trError_gg00 = e_no_more_memory;
        return;
    }
    fgp->Assign( fg );

    if ( fgp->IsAssigned() ) {
        entryhandle = reinterpret_cast<tsp00_MoveObjPtr>(fgp);

        gg06SetNilRef( fileinfo.fi_user_ref );

    } else {
        t.trError_gg00 = MapFileDirReturnCode( retCode );
        destroy( fgp, alloc );
    }
    AppendToContextMessageList( msgList );
}

/*************************************************************************/

/*!
 * @brief Releases an old style file directory entry handle.
 */
externC void bd998ReleaseFDirEntry(
    tgg00_TransContext VAR_VALUE_REF t,
    tsp00_MoveObjPtr   VAR_VALUE_REF entryhandle,
    pasbool                          shared_file)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998ReleaseFDirEntry", FileDir_Trace, 5 );

    SAPDBERR_ASSERT_ARGUMENT( entryhandle != 0 );

    if ( t.trError_gg00 != e_ok ) {
        return;
    }

    FileDir_IDirectory& fd = GetFileDirectory( t );

    FileDir_Generic* fb = reinterpret_cast<FileDir_Generic*>(entryhandle);
    SAPDBMem_IRawAllocator& alloc = fd.GetAllocator();
    destroy( fb, alloc );
}

/*************************************************************************/

/*!
 * @brief Set the fileinfo for an old style file directory entry.
 */
externC void bd998SetFDirEntry(
    tgg00_TransContext VAR_VALUE_REF t,
    tbd_fileinfo       VAR_VALUE_REF fileinfo,
    tsp00_MoveObjPtr   VAR_VALUE_REF entryhandle)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998SetFDirEntry", FileDir_Trace, 5 );

    SAPDBERR_ASSERT_ARGUMENT( entryhandle != 0 );

    FileDir_Generic* fb = reinterpret_cast<FileDir_Generic*>( entryhandle );
    switch ( fb->GetFileType() ) {
    case Data_TableFile:
    {
        FileDir_Table ft;
        ft.Assign( *fb );
        ft.SetFileInfo ( t.trTaskId_gg00, fileinfo );
    }
    break;
    case Data_IndexFile:
    {
        FileDir_Index ft;
        ft.Assign( *fb );
        ft.SetFileInfo ( t.trTaskId_gg00, fileinfo );
    }
    break;
    case Data_ShortColumnFile:
    {
        FileDir_ShortColumn ft;
        ft.Assign( *fb );
        ft.SetFileInfo ( t.trTaskId_gg00, fileinfo );
    }
    break;
    default:
        RTE_Crash(
            SAPDBErr_Exception(
                __CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                "unknown file type") );
    }
}

/*************************************************************************/

/*!
 * @brief Retrieves the fileinfo for an old style file directory entry.
 */
externC void bd998GetFDirEntryFileinfo(
    tgg00_TransContext VAR_VALUE_REF t,
    tgg00_Filename     VAR_ARRAY_REF filename,
    tbd_fileinfo       VAR_VALUE_REF fileinfo,
    pasbool                          shared_file)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998GetFDirEntryFileinfo", FileDir_Trace, 5 );

    SAPDBERR_ASSERT_STATE( UseNewFDir( filename ) );

    if ( t.trError_gg00 != e_ok ) {
        return;
    }

    FileDir_IDirectory& fd = GetFileDirectory( t );

    Msg_List msgList;
    FileDir_Generic fg;
    FileDir_FileNo fileNo (
        *reinterpret_cast<tgg00_Surrogate*>( &filename[2] ) );

    FileDir_ReturnCode retCode =
        fd.GetFile( t.trTaskId_gg00, fileNo, fg, msgList );
    if ( fg.IsAssigned() ) {
        GetFileinfo( fg, fileinfo );
    }

    if ( fg.IsAssigned() ) {
        gg06SetNilRef( fileinfo.fi_user_ref );
    } else {
        t.trError_gg00 = MapFileDirReturnCode( retCode );
    }
    AppendToContextMessageList( msgList );
}

/*************************************************************************/

/*!
 * @brief Retrieves the fileinfo for an old style file directory entry.
 *
 * This method should be used, if the file info of a file that might
 * be marked as deleted is to be retrieved.
 */
externC void bd998GcGetFDirEntryFileinfo(
    tgg00_TransContext VAR_VALUE_REF t,
    tgg00_Filename     VAR_ARRAY_REF filename,
    tbd_fileinfo       VAR_VALUE_REF fileinfo,
    pasbool                          shared_file)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998GcGetFDirEntryFileinfo", FileDir_Trace, 5 );

    SAPDBERR_ASSERT_STATE( UseNewFDir( filename ) );

    if ( t.trError_gg00 != e_ok ) {
        return;
    }

    FileDir_IDirectory& fd = GetFileDirectory( t );

    Msg_List msgList;
    FileDir_Generic fg;
    FileDir_FileNo fileNo;
    if ( filename[0] != tfnObj_egg00    &&
         filename[0] != tfnContObj_egg00 )
    {
        fileNo = *reinterpret_cast<tgg00_Surrogate*>( &filename[2] );
    } else {
        fileNo = *reinterpret_cast<tgg00_Surrogate*>( &filename[8] );
    }

    FileDir_ReturnCode retCode =
        fd.GcGetFile( t.trTaskId_gg00, fileNo, fg, msgList );
    if ( fg.IsAssigned() ) {
        GetFileinfo( fg, fileinfo );
    }

    if ( fg.IsAssigned() ) {
        gg06SetNilRef( fileinfo.fi_user_ref );
    } else {
        t.trError_gg00 = MapFileDirReturnCode( retCode );
    }
    AppendToContextMessageList( msgList );
}

/*************************************************************************/

/*!
 * @brief Deletes an old style file directory entry from the file directory.
 */
externC void bd998DelFDirEntry(
    tgg00_TransContext VAR_VALUE_REF t,
    tgg00_Filename     VAR_ARRAY_REF filename,
    tbd_fileinfo       VAR_VALUE_REF fileinfo,
    pasbool                          shared_file)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998DelFDirEntry", FileDir_Trace, 5 );

    SAPDBERR_ASSERT_STATE( UseNewFDir( filename ) );

    if ( t.trError_gg00 != e_ok ) {
        return;
    }

    FileDir_IDirectory& fd = GetFileDirectory( t );

    Msg_List msgList;
    FileDir_Generic fg;
    FileDir_FileNo fileNo (
        *reinterpret_cast<tgg00_Surrogate*>( &filename[2] ) );

    FileDir_ReturnCode retCode =
        fd.GetFile( t.trTaskId_gg00, fileNo, fg, msgList );
    if ( fg.IsAssigned() ) {
        GetFileinfo( fg, fileinfo );

        fg.MarkFileDeleted( t.trWriteTransId_gg00 );
        fd.DelFile( t.trTaskId_gg00, fileNo, msgList );
    } else {
        t.trError_gg00 = MapFileDirReturnCode( retCode );
    }
    AppendToContextMessageList( msgList );
}

/*************************************************************************/

/*!
 * @brief C-version of b17reset_read_only.
 */
externC void bd998ResetReadOnly(tgg00_TransContext VAR_VALUE_REF t)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998ResetReadOnly", FileDir_Trace, 5 );

    FileDir_SharedDirectory& fd = FileDir_SharedDirectory::Instance();
    FileDir_Iterator iter( fd.GetAllocator() );

    iter.AddFileTypeToFilter( Data_TableFile );
    iter.AddFileTypeToFilter( Data_ShortColumnFile );
    fd.GetFileDirIterator( iter, &t );

    FileDir_Generic fg;
    tgg00_FileId fileId;
    tbd_current_tree current;
    tbd_file_state_set s = tbd_file_state_set::build( 1, f_read_only);

    while ( ( iter.GetNext( fg ) ) && ( t.trError_gg00 == e_ok ) ) {
        if ( fg.GetFileState() == Data_FileReadOnly ) {
            fg.BuildFileId( fileId );
            bd30GetTree( t, fileId, current, m_set,
                LOCK_TREE_EXCL_BD00, ! SYNC_TREE_LOCK_BD00);
            if ( t.trError_gg00 == e_ok ) {
                bd30SubFileState( current, s );
                fg.SetFileState( Data_FileOk );
            }
            bd30ReleaseTree( current );
        }
    }
}

/*************************************************************************/

/*!
 * @brief Adds file states to an old style file directory entry.
 */
externC void bd998AddFileStatesToEntry(
    tgg00_TransContext    VAR_VALUE_REF  t,
    tgg00_Surrogate       VAR_ARRAY_REF  table_surr,
    tbd_fileinfo          VAR_VALUE_REF  fileInfo,
    pasbool                              shared_file,
    tbd_file_state_set    VAR_ARRAY_REF  addState)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998AddFileStatesFromEntry", FileDir_Trace, 5 );

    if ( t.trError_gg00 != e_ok ) {
        return;
    }

    FileDir_IDirectory& fd = GetFileDirectory( t );

    Msg_List msgList;
    FileDir_Generic fg;
    FileDir_ReturnCode retCode =
        fd.GetFile( t.trTaskId_gg00, table_surr, fg, msgList );
    if ( retCode != FileDir_Okay ) {
        // FIXME: do proper error code mapping
        t.trError_gg00 = e_file_not_found;
        AppendToContextMessageList( msgList );
        return;
    }

    switch ( fg.GetFileType() ) {
    case Data_TableFile:
    {
        FileDir_Table ft;
        ft.Assign( fg );
        ft.FillFileInfo( fileInfo );
        fileInfo.fi_state.addSet( addState );
        ft.SetFileInfo( t.trTaskId_gg00, fileInfo );
    }
    break;
    case Data_IndexFile:
    {
        FileDir_Index ft;
        ft.Assign( fg );
        ft.FillFileInfo( fileInfo );
        fileInfo.fi_state.addSet( addState );
        ft.SetFileInfo( t.trTaskId_gg00, fileInfo );
    }
    break;
    case Data_ShortColumnFile:
    {
        FileDir_ShortColumn ft;
        ft.Assign( fg );
        ft.FillFileInfo( fileInfo );
        fileInfo.fi_state.addSet( addState );
        ft.SetFileInfo( t.trTaskId_gg00, fileInfo );
    }
    break;
    default:
        t.trError_gg00 = e_system_error;
        Kernel_VTrace()
            << "bd998AddFileStatesToEntry: file of unexpected type "
            << SAPDB_ToString( fg.GetFileType() );
        SAPDBERR_ASSERT_STATE( false );
    }

    gg06SetNilRef( fileInfo.fi_user_ref );

    AppendToContextMessageList( msgList );
}

/*************************************************************************/

/*!
 * @brief Removes file states from an old style file directory entry.
 */
externC void bd998SubFileStatesFromEntry(
    tgg00_TransContext    VAR_VALUE_REF  t,
    tgg00_Surrogate       VAR_ARRAY_REF  table_surr,
    tbd_fileinfo          VAR_VALUE_REF  fileInfo,
    pasbool                              shared_file,
    tbd_file_state_set    VAR_ARRAY_REF  subState)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998SubFileStatesFromEntry", FileDir_Trace, 5 );

    if ( t.trError_gg00 != e_ok ) {
        return;
    }

    FileDir_IDirectory& fd = GetFileDirectory( t );

    Msg_List msgList;
    FileDir_Generic fg;
    FileDir_ReturnCode retCode =
        fd.GetFile( t.trTaskId_gg00, table_surr, fg, msgList );
    if ( retCode != FileDir_Okay ) {
        // FIXME: do proper error code mapping
        t.trError_gg00 = e_file_not_found;
        AppendToContextMessageList( msgList );
        return;
    }

    switch ( fg.GetFileType() ) {
    case Data_TableFile:
    {
        FileDir_Table ft;
        ft.Assign( fg );
        ft.FillFileInfo( fileInfo );
        fileInfo.fi_state.delSet( subState );
        ft.SetFileInfo( t.trTaskId_gg00, fileInfo );
    }
    break;
    case Data_IndexFile:
    {
        FileDir_Index ft;
        ft.Assign( fg );
        ft.FillFileInfo( fileInfo );
        fileInfo.fi_state.delSet( subState );
        ft.SetFileInfo( t.trTaskId_gg00, fileInfo );
    }
    break;
    case Data_ShortColumnFile:
    {
        FileDir_ShortColumn ft;
        ft.Assign( fg );
        ft.FillFileInfo( fileInfo );
        fileInfo.fi_state.delSet( subState );
        ft.SetFileInfo( t.trTaskId_gg00, fileInfo );
    }
    break;
    default:
        t.trError_gg00 = e_system_error;
        Kernel_VTrace()
            << "bd998SubFileStatesFromEntry: file of unexpected type "
            << SAPDB_ToString( fg.GetFileType() );
        SAPDBERR_ASSERT_STATE( false );
    }

    gg06SetNilRef( fileInfo.fi_user_ref );

    AppendToContextMessageList( msgList );
}

/*************************************************************************/

/// Adds a table entry to the file directory based on old-style file
/// directory information.
externC void bd998AddFileNoSafetyMargin(
    tsp00_TaskId                     taskId)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998AddFileNoSafetyMargin", FileDir_Trace, 5 );

    FileDir_SharedDirectory& fd   = FileDir_SharedDirectory::Instance();
    fd.AddFileNoSafetyMargin();
}

/*************************************************************************/

/// Adds a table entry to the file directory based on old-style file
/// directory information.
externC void bd998ConvTableFDirEntry(
    tsp00_TaskId                     taskId,
    tgg00_FileId       VAR_VALUE_REF old_file_id,
    tsp00_PageNo       VAR_VALUE_REF rootPageNo,
    tsp00_Int2                       blobColCount,
    tgg00_FiletypeSet  VAR_VALUE_REF fileTypeSet,
    tgg91_FileVersion  VAR_VALUE_REF fileVersion)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998ConvTableFDirEntry", FileDir_Trace, 5 );

    FileDir_SharedDirectory& fd   = FileDir_SharedDirectory::Instance();
    SAPDBMem_IRawAllocator& alloc = fd.GetAllocator();

    FileDir_TableEntry* fe = 0;
    FileDir_FileNo      fileNo( old_file_id.fileTabId_gg00() );

    fd.Restart_SetFileNoHighWaterMark( fileNo );

    fe = new (alloc) FileDir_TableEntry(
            taskId,
            alloc,
            fileNo,
            Converter_ICommon::Instance().Version(),
            rootPageNo,
            blobColCount,
            fileTypeSet,
            fileVersion
            );

    if ( fe == 0 ) {
        FileDir_Common::OutOfMemoryCrash( sizeof(FileDir_TableEntry) );
    }

    const SAPDB_Bool c_lobPageCounterInitialized = true;
    fe->SetLobPageCount( 0, ! c_lobPageCounterInitialized );

    Msg_List msgList;
    FileDir_ReturnCode ret = fd.AddFileEntry(
        taskId, fileNo, fe, msgList,
        FileDir_GenericDirectory::FileDir_NoLocking );

    if ( ret != FileDir_Okay ) {
        RTE_Crash( msgList );
    }
}

/*************************************************************************/

/// Adds an index entry to the file directory based on old-style file
/// directory information.
externC void bd998ConvIndexFDirEntry(
    tsp00_TaskId                     taskId,
    tgg00_FileId       VAR_VALUE_REF old_file_id,
    tsp00_PageNo       VAR_VALUE_REF rootPageNo,
    tgg00_FiletypeSet  VAR_VALUE_REF fileTypeSet,
    tgg91_FileVersion  VAR_VALUE_REF fileVersion)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998ConvIndexFDirEntry", FileDir_Trace, 5 );

    FileDir_SharedDirectory& fd   = FileDir_SharedDirectory::Instance();
    SAPDBMem_IRawAllocator& alloc = fd.GetAllocator();

    FileDir_TableEntry* fe = 0;
    FileDir_FileNo      tableFileNo( old_file_id.fileTabId_gg00() );
    FileDir_FileNo      indexFileNo = fd.MigrationGetNewFileNo();

    SAPDB_UInt2 indexNo = old_file_id.fileTfnNo_gg00()[ 0 ];

    // fixme: use same mechanism as for tables to avoid locking
    Msg_List msgList;
    FileDir_ReturnCode ret;
    if ( old_file_id.fileTfn_gg00() == tfnMulti_egg00 ) {
        ret = fd.AddIndexFile(
            taskId,
            indexFileNo,
            tableFileNo,
            Converter_ICommon::Instance().Version(),
            indexNo,
            rootPageNo,
            fileTypeSet,
            fileVersion,
            msgList);
    } else {
        SAPDBERR_ASSERT_STATE( old_file_id.fileTfn_gg00() == tfnOmsInv_egg00 );
        ret = fd.MigrationAddSingleIndexFile(
            taskId,
            indexFileNo,
            tableFileNo,
            indexNo,
            rootPageNo,
            fileTypeSet,
            fileVersion,
            msgList);
    }

    if ( ret != FileDir_Okay ) {
        // PTS 1132508 M.Ki.
        tsp00_Line line;
        if ( ret == FileDir_FileNoNotFound ) {
            msgList.Overrule(
                Msg_List (
                    Msg_List::Warning,
                    SDBMSG_FILEDIR_MIGRATE_INDEX_TABLE_NOT_FOUND,
                    Msg_Arg("INDEX", SAPDB_ToString( indexNo )),
                    Msg_Arg("FILENO", FileDir_FileNoToString( tableFileNo )),
                    Msg_Arg("FILETYPESET", FileTypeSetToString( fileTypeSet, &line )),
                    Msg_Arg("PAGENO", SAPDB_ToString (rootPageNo))
                    )
                );
            RTE_Message( msgList );
            msgList.ClearMessageList();
        } else {
            msgList.Overrule(
                Msg_List (
                    Msg_List::Warning,
                    SDBMSG_FILEDIR_MIGRATE_INDEX_ERROR,
                    Msg_Arg("INDEX", SAPDB_ToString( indexNo )),
                    Msg_Arg("FILENO", FileDir_FileNoToString( tableFileNo )),
                    Msg_Arg("FILETYPESET", FileTypeSetToString( fileTypeSet, &line )),
                    Msg_Arg("PAGENO", SAPDB_ToString (rootPageNo)),
                    Msg_Arg("ERR",SAPDB_ToString( ret ))
                    )
                );
            RTE_Crash( msgList );
        }
    }
}

/*************************************************************************/

/// Adds a short column entry to the file directory based on old-style
/// file directory information.
externC void bd998ConvShortColFDirEntry(
    tsp00_TaskId                     taskId,
    tgg00_FileId       VAR_VALUE_REF old_file_id,
    tsp00_PageNo       VAR_VALUE_REF rootPageNo,
    tgg00_FiletypeSet  VAR_VALUE_REF fileTypeSet,
    tgg91_FileVersion  VAR_VALUE_REF fileVersion)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998ConvShortColFDirEntry", FileDir_Trace, 5 );

    FileDir_SharedDirectory& fd   = FileDir_SharedDirectory::Instance();
    SAPDBMem_IRawAllocator& alloc = fd.GetAllocator();

    FileDir_TableEntry* fe = 0;
    FileDir_FileNo      tableFileNo( old_file_id.fileTabId_gg00() );
    FileDir_FileNo      shortColFileNo = fd.MigrationGetNewFileNo();

    // fixme: use same mechanism as for tables to avoid locking
    Msg_List msgList;
    FileDir_ReturnCode ret = fd.AddShortColumnFile(
        taskId,
        shortColFileNo,
        tableFileNo,
        Converter_ICommon::Instance().Version(),
        rootPageNo,
        fileTypeSet,
        fileVersion,
        msgList);

    if ( ret != FileDir_Okay ) {
        // PTS 1132508 M.Ki.
        tsp00_Line line;
        if ( ret == FileDir_FileNoNotFound ) {
            msgList.Overrule(
                Msg_List (
                    Msg_List::Warning,
                    SDBMSG_FILEDIR_MIGRATE_SHORT_COLUMN_TABLE_NOT_FOUND,
                    Msg_Arg("FILENO", FileDir_FileNoToString( tableFileNo )),
                    Msg_Arg("FILETYPESET", FileTypeSetToString( fileTypeSet, &line )),
                    Msg_Arg("PAGENO", SAPDB_ToString (rootPageNo))
                    )
                );
            RTE_Message( msgList );
            msgList.ClearMessageList();
        } else {
            msgList.Overrule(
                Msg_List (
                    Msg_List::Warning,
                    SDBMSG_FILEDIR_MIGRATE_SHORT_COLUMN_ERROR,
                    Msg_Arg("FILENO", FileDir_FileNoToString( tableFileNo )),
                    Msg_Arg("FILETYPESET", FileTypeSetToString( fileTypeSet, &line )),
                    Msg_Arg("PAGENO", SAPDB_ToString (rootPageNo)),
                    Msg_OptArg("ERR",SAPDB_ToString( ret ))
                    )
                );
            RTE_Crash( msgList );
        }
    }
}

/*************************************************************************/

externPascal void bd998MigSwitchSingleIndexToMulti (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    tgg00_Surrogate       VAR_ARRAY_REF  tableSurrogate,
    tsp00_Int4                           oldSingleIndexId,
    tsp00_Int4                           newMultiIndexId)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998MigSwitchSingleIndexToMulti", FileDir_Trace, 5 );

    trans.trError_gg00 = e_ok;

    Msg_List msgList;
    FileDir_SharedDirectory& fd   = FileDir_SharedDirectory::Instance();

    FileDir_ReturnCode retCode = fd.MigrationSwitchSingleIndexToMulti(
        trans.trTaskId_gg00,
        tableSurrogate,
        oldSingleIndexId,
        newMultiIndexId,
        msgList);

    if ( retCode != FileDir_Okay ) {
        trans.trError_gg00 = MapFileDirReturnCode( retCode );
    }

    AppendToContextMessageList( msgList );
}

/*************************************************************************/

externPascal void bd998MigCleanupSingleIndexes (
    tgg00_TransContext    VAR_VALUE_REF  trans)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998MigCleanupSingleIndexes", FileDir_Trace, 5 );

    trans.trError_gg00 = e_ok;

    Msg_List msgList;
    FileDir_SharedDirectory& fd = FileDir_SharedDirectory::Instance();

    FileDir_ReturnCode retCode = fd.MigrationCleanupSingleIndexes(
        Trans_Context::GetContext( trans), msgList );

    if ( retCode != FileDir_Okay ) {
        trans.trError_gg00 = MapFileDirReturnCode( retCode );
    }

    AppendToContextMessageList( msgList );
}

/*************************************************************************/

/// Returns the root page of the shared file directory.
externC tsp00_PageNo bd998GetFDirRootPageNo()
{
    return FileDir_SharedDirectory::Instance().GetRootPageNo();
}

/*************************************************************************/

/// Wrapper to set the file directory status to migration running.
externC void bd998SetFDirMigrationRunning()
{
    FileDir_SharedDirectory::Instance().SetMigrationRunning( true );
}

/*************************************************************************/

/// Wrapper to set the file directory status to migration not running.
externC void bd998SetFDirMigrationFinished()
{
    FileDir_SharedDirectory::Instance().SetMigrationRunning( false );
}

/*************************************************************************/

/// Wrapper to call the file directory check data methods.
externC void bd998VerifyFdir(
    tgg00_TransContext VAR_VALUE_REF t,
    pasbool                          withUpdate)
{
    FileDir_ISharedDirectory& fd = FileDir_ISharedDirectory::Instance();

    FileDir_ReturnCode retCode;
    if ( withUpdate ) {
        retCode = fd.CheckDataWithUpdate( t.trTaskId_gg00 );
    } else {
        retCode = fd.CheckData( t.trTaskId_gg00 );
    }
    if ( FileDir_Okay != retCode ) {
        t.trError_gg00 = e_bad_fdir;
    }
}

/*************************************************************************/

/// Wrapper to retrieve a new persistent fileNo.
externC void bd998NewFileNo(tgg00_Surrogate VAR_VALUE_REF fileNo)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998NewFileNo", FileDir_Trace, 5 );

    FileDir_FileNo fi = FileDir_ISharedDirectory::Instance().GetNewFileNo();
    SAPDBTRACE_WRITELN(
        FileDir_Trace, 5,
        "file no       : " << fi );

    SAPDB_MemCopyNoCheck( &fileNo, &fi, sizeof(tgg00_Surrogate) );
}

/*************************************************************************/

/// Wrapper to retrieve a new temporary fileNo.
externC void bd998NewTempFileNo(tgg00_Surrogate VAR_VALUE_REF fileNo)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998NewTempFileNo", FileDir_Trace, 5 );

    FileDir_FileNo fi =
        FileDir_ISharedDirectory::Instance().GetNewTempFileNo();
    SAPDB_MemCopyNoCheck( &fileNo, &fi, sizeof(tgg00_Surrogate) );
}

/*************************************************************************/

/// Wrapper to retrieve a new shared temporary fileNo.
externC void bd998NewTempSharedFileNo(tgg00_Surrogate VAR_VALUE_REF fileNo)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998NewTempSharedFileNo", FileDir_Trace, 5 );

    FileDir_FileNo fi =
        FileDir_ISharedDirectory::Instance().GetNewTempSharedFileNo();
    SAPDB_MemCopyNoCheck( &fileNo, &fi, sizeof(tgg00_Surrogate) );
}

/*************************************************************************/

/// Wrapper to add a table file to the file directory.
externPascal void bd998AddTableToFdir (
    tgg00_TransContext VAR_VALUE_REF trans,
    const tgg00_Surrogate VAR_VALUE_REF fileNo,
    tgg00_ConverterVersion converterVersion,
    tsp00_PageNo rootPage,
    tsp00_Int2 blobColCount,
    const tgg00_FiletypeSet VAR_ARRAY_REF fileTypeSet,
    const tgg91_FileVersion VAR_VALUE_REF fileVersion)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998AddTableToFdir", FileDir_Trace, 5 );

    tgg00_BasisError& trError = trans.trError_gg00;
    if ( trError != e_ok ) {
        return;
    }

    Msg_List msgList;
    FileDir_ReturnCode ret;
    FileDir_IDirectory& directory = GetFileDirectory( trans );
    ret = directory.AddTableFile(
        trans.trTaskId_gg00,
        FileDir_FileNo( fileNo ),
        converterVersion,
        rootPage,
        blobColCount,
        fileTypeSet,
        fileVersion,
        msgList);

    if ( ret != FileDir_Okay ) {
        trError = MapFileDirReturnCode( ret );
    }
    AppendToContextMessageList( msgList );
}

/*************************************************************************/

/// Wrapper to add a shared temp table file to the file directory.
externC void bd998AddSharedTempTableToFdir (
    tgg00_TransContext VAR_VALUE_REF trans,
    const tgg00_Surrogate VAR_VALUE_REF surrogate,
    tsp00_PageNo rootPage,
    tsp00_Int2                         blobColCount,
    const tgg00_FiletypeSet VAR_VALUE_REF fileTypeSet,
    const tgg91_FileVersion VAR_VALUE_REF fileVersion)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998AddSharedTempTableToFdir", FileDir_Trace, 5 );

    tgg00_BasisError& trError = trans.trError_gg00;

    if ( trError != e_ok ) {
        return;
    }

    FileDir_IDirectory& directory = GetFileDirectory( trans );

    Msg_List msgList;
    FileDir_FileNo fileNo( surrogate );
    FileDir_ReturnCode ret = directory.AddSharedTempTableFile(
        trans.trTaskId_gg00,
        fileNo,
        Converter_ICommon::Instance().Version(),
        rootPage,
        blobColCount,
        fileTypeSet,
        msgList);

    if ( ret != FileDir_Okay ) {
        trError = MapFileDirReturnCode( ret );
    }
    AppendToContextMessageList( msgList );
}

/*************************************************************************/

/*!
 * @brief Wrapper to retrieve the index file no for a given table file.
 *
 * @param tableSurrogate [in] fileNo of table to retrieve the index for
 * @param indexNo        [in] the id of the index to retrieve
 * @param indexSurrogate [out] fileNo of the requested index; undefined
 *                             if the requested index could not be retrieved
 */
externC void bd998GetIndexFileNoForTable (
    tgg00_Surrogate VAR_ARRAY_REF tableSurrogate,
    SAPDB_Int2                    indexNo,
    tgg00_Surrogate VAR_ARRAY_REF indexSurrogate)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998GetIndexFileNoForTable", FileDir_Trace, 5 );

    RTE_TaskId taskId;
    vgetpid( taskId );
    Msg_List errMsg;

    FileDir_FileNo tableFileNo( tableSurrogate );

    FileDir_Table tableEntry;
    if ( FileDir_SharedDirectory::Instance()
         .GetTableFile( taskId, tableFileNo, tableEntry, errMsg )
         != FileDir_Okay ) {

        SAPDB_MemCopyNoCheck( &indexSurrogate, &cgg_zero_id, sizeof(indexSurrogate) );
        return;
    }

    FileDir_FileNo indexFileNo = tableEntry.GetIndexFileNo( indexNo );

    SAPDBTRACE_WRITELN(
        FileDir_Trace, 5,
        "tableFileNo: " << tableFileNo << NewLine
        << "indexFileNo: " << indexFileNo );

    SAPDB_MemCopyNoCheck( &indexSurrogate, &indexFileNo, sizeof(indexSurrogate) );
}

/*************************************************************************/

/// Wrapper to retrieve the fileNo of the table a given index is defined on.
externPascal void bd998GetTableFileNoForIndex (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    const tgg00_Surrogate  VAR_ARRAY_REF indexSurrogate,
    tgg00_Surrogate       VAR_ARRAY_REF  tableSurrogate,
    tgg00_BasisError    VAR_VALUE_REF    b_err)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998GetTableFileNoForIndex", FileDir_Trace, 5 );


    b_err = e_ok;

    FileDir_FileNo indexFileNo( indexSurrogate );

    Msg_List msgList;
    FileDir_Index indexEntry;
    FileDir_IDirectory::SessionInstance( trans )
        .GetIndexFile( trans.trTaskId_gg00, indexFileNo, indexEntry, msgList );

    if ( ! indexEntry.IsAssigned() ) {
        SAPDB_MemCopyNoCheck(
            &tableSurrogate, &cgg_zero_id, sizeof(tableSurrogate) );
        trans.trError_gg00 = e_ok;
        b_err = e_file_not_found;
        AppendToContextMessageList( msgList );
        return;
    }

    FileDir_FileNo tableFileNo = indexEntry.GetParentFileNo();

    SAPDBTRACE_WRITELN(
        FileDir_Trace, 5,
        "indexFileNo: " << indexFileNo << NewLine
        << "tableFileNo: " << tableFileNo );

    SAPDB_MemCopyNoCheck(
        &tableSurrogate, &tableFileNo, sizeof(tableSurrogate) );
    AppendToContextMessageList( msgList );
}

/*************************************************************************/

/// Wrapper to retrieve the fileNo of the table a given short column
/// file belongs to.
externPascal void bd998GetTableFileNoForShortColumn (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    const tgg00_Surrogate  VAR_ARRAY_REF shortColSurrogate,
    tgg00_Surrogate       VAR_ARRAY_REF  tableSurrogate,
    tgg00_BasisError    VAR_VALUE_REF    b_err)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998GetTableFileNoForShortCol", FileDir_Trace, 5 );

    b_err = e_ok;

    FileDir_FileNo shortColumnFileNo( shortColSurrogate );

    Msg_List msgList;
    FileDir_ShortColumn shortColumnEntry;
    FileDir_IDirectory::SessionInstance( trans )
        .GetShortColumnFile( trans.trTaskId_gg00, shortColumnFileNo,
                             shortColumnEntry, msgList );

    if ( ! shortColumnEntry.IsAssigned() ) {
        SAPDB_MemCopyNoCheck(
            &tableSurrogate, &cgg_zero_id, sizeof(tableSurrogate) );
        trans.trError_gg00 = e_ok;
        b_err = e_file_not_found;
        AppendToContextMessageList( msgList );
        return;
    }

    FileDir_FileNo tableFileNo = shortColumnEntry.GetParentFileNo();

    SAPDBTRACE_WRITELN(
        FileDir_Trace, 5,
        "shortColumnFileNo: " << shortColumnFileNo << NewLine
        << "tableFileNo: " << tableFileNo );

    SAPDB_MemCopyNoCheck(
        &tableSurrogate, &tableFileNo, sizeof(tableSurrogate) );
    AppendToContextMessageList( msgList );
}

/*************************************************************************/

/// Drop all index files that are defined on a given table.
externPascal void bd998DropTableIndexes(
    tgg00_TransContext    VAR_VALUE_REF trans,
    tgg00_Surrogate VAR_ARRAY_REF tableSurrogate)
{
    FileDir_FileNo tableFileNo( tableSurrogate );

    FileDir_IDirectory& fd = FileDir_IDirectory::Instance(trans.trIndex_gg00);

    Msg_List msgList;
    FileDir_Table table;
    FileDir_ReturnCode retValue =
        fd.GetTableFile( trans.trTaskId_gg00, tableFileNo, table, msgList );
    if ( retValue != FileDir_Okay ) {
        trans.trError_gg00 = e_file_not_found;
        AppendToContextMessageList( msgList );
        return;
    }

    SAPDB_Int indexCount = table.GetMaxIndexId();
    FileDir_FileNo indexFileNo;
    SAPDB_Int indexNo;
    for ( indexNo = 1; indexNo <= indexCount; ++indexNo ) {
        indexFileNo = table.GetIndexFileNo( indexNo );
        if ( indexFileNo.IsInvalid() ) {
            continue;
        }
        tgg00_FileId indexId = b01niltree_id;
        indexId.fileTfnNo_gg00()[0] = indexNo;
        indexId.fileTfn_gg00().becomes( tfnMulti_egg00 );
        indexId.fileType_gg00() =
            tgg00_FiletypeSet::build( 2, ftsPerm_egg00, ftsConcurrent_egg00 );
        SAPDB_MemCopyNoCheck( &indexId.fileTabId_gg00(), &indexFileNo,
                sizeof(tgg00_Surrogate) );
        k64drop_tree (trans, indexId);

        if ( retValue != FileDir_Okay ) {
            trans.trError_gg00 = MapFileDirReturnCode( retValue );
            return;
        }
    }
    AppendToContextMessageList( msgList );
}

/*************************************************************************/

/// Wrapper to retrieve the number of bad indices registered in the
/// file directory.
externPascal tsp00_Int4 bd998GetBadIndexCount()
{
    return static_cast<tsp00_Int4> (
        FileDir_ISharedDirectory::Instance().GetBadIndexCount() );
}

/*************************************************************************/

/// Wrapper to add a short column file entry to the file directory.
externPascal void bd998AddShortColumnFile (
    tgg00_TransContext    VAR_VALUE_REF t,
    tgg00_ConverterVersion                 converterVersion,
    const tgg00_FileId    VAR_VALUE_REF shortColFile,
    const tgg00_Surrogate VAR_VALUE_REF parentFileNo)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998AddShortColumnFile", FileDir_Trace, 5 );

    tgg00_BasisError& trError = t.trError_gg00;

    if ( trError != e_ok ) {
        return;
    }

    FileDir_IDirectory& fd = FileDir_IDirectory::Instance(t.trIndex_gg00);

    Msg_List msgList;
    FileDir_FileNo shortColFileNo( shortColFile.fileTabId_gg00() );
    FileDir_ReturnCode ret = fd.AddShortColumnFile(
        t.trTaskId_gg00,
        shortColFileNo,
        parentFileNo,
        converterVersion,
        shortColFile.fileRoot_gg00(),
        shortColFile.fileType_gg00(),
        shortColFile.fileVersion_gg00(),
        msgList);

    if ( ret != FileDir_Okay ) {
        trError = MapFileDirReturnCode( ret );
    }
    AppendToContextMessageList( msgList );
}

/*************************************************************************/

/// Wrapper to retrieve the short column fileNo for a given table.
externPascal void bd998GetShortColFileNoForTable (
    tgg00_Surrogate  VAR_VALUE_REF tableSurrogate,
    tgg00_Surrogate  VAR_VALUE_REF shortColFileSurrogate,
    tgg00_BasisError VAR_VALUE_REF b_err)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998GetShortColFileNoForTable", FileDir_Trace, 5 );

    b_err = e_ok;

    SAPDBTRACE_WRITELN(
        FileDir_Trace, 5,
        "table fileno : " << static_cast<FileDir_FileNo>(tableSurrogate) );

    FileDir_IDirectory& fd = FileDir_SharedDirectory::Instance();
    RTE_TaskId taskId;
    vgetpid( taskId );
    Msg_List errMsg;

    FileDir_Table tableEntry;
    if ( FileDir_SharedDirectory::Instance()
        .GetTableFile( taskId, tableSurrogate, tableEntry, errMsg )
         != FileDir_Okay ) {

        SAPDB_MemCopyNoCheck( &shortColFileSurrogate, &cgg_zero_id,
                sizeof(shortColFileSurrogate) );
        b_err = e_file_not_found;
        return;
    }

    if ( tableEntry.HasShortColumnFile() ) {
        FileDir_FileNo shortColFileNo;
        tableEntry.GetShortColumnFileNo( shortColFileNo );
        SAPDB_MemCopyNoCheck(
            &shortColFileSurrogate, &shortColFileNo,
            sizeof(shortColFileSurrogate) );
    } else {
        SAPDB_MemCopyNoCheck(
            &shortColFileSurrogate, &cgg_zero_id,
            sizeof(shortColFileSurrogate) );
        b_err = e_file_not_found;
        return;
    }
}

/*************************************************************************/

/// Wrapper to mark a file as deleted.
externPascal void bd998MarkFileDeleted(
    tgg00_TransContext VAR_VALUE_REF t,
    const tgg00_Surrogate    VAR_ARRAY_REF fileSurrogate)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998MarkFileDeleted", FileDir_Trace, 5 );

    t.trError_gg00 = e_ok;
    Msg_List msgList;
    FileDir_Generic fg;
    if ( ! bd998GetFile( t, fileSurrogate, fg ) ) {
        return;
    }

    FileDir_ReturnCode retCode = fg.MarkFileDeleted( t.trWriteTransId_gg00 );
    if ( (retCode != FileDir_Okay)
         && (retCode != FileDir_AlreadyMarkedForDeletion) ) {
        RTE_Crash(
            SAPDBErr_Exception(
                __CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED,
                "MarkForDeletion must succeed") );
    }
}

/*************************************************************************/

/// Wrapper to check if a file is marked as deleted.
externPascal pasbool bd998IsFileMarkedDeleted(
    tgg00_TransContext VAR_VALUE_REF t,
    const tgg00_Surrogate    VAR_ARRAY_REF fileSurrogate)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998RemoveFileDeletedMark", FileDir_Trace, 5 );

    t.trError_gg00 = e_ok;
    Msg_List msgList;
    FileDir_Generic fg;
    if ( ! bd998GetFile( t, fileSurrogate, fg ) ) {
        return false;
    }

    return fg.GetFileState() == Data_FileDeleted;
}

/*************************************************************************/

/// Wrapper to mark a file as deleted.
externPascal void bd998RemoveFileDeletedMark(
    tgg00_TransContext VAR_VALUE_REF t,
    const tgg00_Surrogate    VAR_ARRAY_REF fileSurrogate)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998RemoveFileDeletedMark", FileDir_Trace, 5 );

    t.trError_gg00 = e_ok;
    Msg_List msgList;
    FileDir_Generic fg;
    if ( ! bd998GetFile( t, fileSurrogate, fg ) ) {
        return;
    }

    fg.RemoveFileDeletedMark();
}

/*************************************************************************/

/// Wrapper to unconditionally remove a file directory entry (without
/// having to mark it as deleted first).
externPascal void bd998DelDirectFDirEntry(
    tgg00_TransContext VAR_VALUE_REF t,
    tgg00_Surrogate    VAR_ARRAY_REF fileSurrogate,
    tbd_fileinfo       VAR_VALUE_REF fileinfo)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998DelDirectFDirEntry", FileDir_Trace, 5 );

    t.trError_gg00 = e_ok;

    Msg_List msgList;
    FileDir_Generic fg;
    FileDir_FileNo fileId ( fileSurrogate );
    FileDir_IDirectory& fd = GetFileDirectory( t );
    fd.GcGetFile( t.trTaskId_gg00, fileId, fg, msgList );
    if ( fg.IsAssigned() ) {
        GetFileinfo( fg, fileinfo );
        FileDir_ReturnCode retCode = fg.MarkFileDeleted( t.trWriteTransId_gg00 );
        if ( (retCode != FileDir_Okay)
             && (retCode != FileDir_AlreadyMarkedForDeletion) ) {
            RTE_Crash(
                SAPDBErr_Exception(
                    __CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED,
                    "MarkForDeletion must succeed") );
        }
        fd.DelFile( t.trTaskId_gg00, fileId, msgList );
    } else {
        t.trError_gg00 = e_file_not_found;
    }
    AppendToContextMessageList( msgList );
}

/*************************************************************************/

void  bd998CreateGroupedTempFile(
    tgg00_TransContext&   trans,
    FileDir_FileNo&       fileNo,
    tgg00_FileId&         fileId,
    const Data_TempFileTypes::FileType tempFileType)
{
    tgg00_BasisError&     trError = trans.trError_gg00;
    tbd_current_tree      current;
    tbd_fileinfo          fi;
    FileDir_Table         ft;

//    g04build_temp_tree_id( fileId, trans ); -- can't be done because file handling would be reset
    /* FIXME: at some point in time, file types in fileName_gg00
     * should not matter any more and the following line can be
     * uncommented:
     * file_id.fileName_gg00 := cgg_zero_fn;
     */
    FileDir_IDirectory& directory = GetFileDirectory( trans );

    Msg_List msgList;
    if ( fileNo.IsInvalid() ) {
        fileNo = directory.GetNewTempFileNo();
    }

    SAPDB_MemCopyNoCheck(
        &fileId.fileTabId_gg00(),
        &fileNo,
        sizeof( fileNo ) );
    if ( g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdPrim_gg00 ) {
        tgg11_BdFilenameTrace fnTrace;
        fnTrace.bdfTrType_gg11.becomes( bdtrFilename_egg11 );
        fnTrace.bdfFn_gg11     = fileId.fileName_gg00();
        Trace_CommonEntry Entry(
            tgg00_Debug::fromConst(bd),
            tgg00_VtraceType::fromConst(b01t_create),
            reinterpret_cast<SAPDB_Byte*>(&fnTrace), sizeof(fnTrace) );
    }
#if defined(TRACE)
    t01name( bi, "====> bdCreateTemp");
    t01treeid( bi, "treeid req  ", fileId);
#endif
    if ( b01downfilesystem ) {
        trError = e_shutdown;
    } else {
        trError = e_ok;
        fileId.fileResultSite_gg00().rawAssign( cgg_zero_c2 );
        fi.fi_type = fileId.fileType_gg00();
        fi.fi_state.clear();
        fi.fi_fill1 = false;
        fi.fi_descr.clear();
        fi.fi_root = NIL_PAGE_NO_GG00;
        fi.fi_vers.gg91SetDummyVers();
        fi.fi_col_cnt = 0;
        gg06SetNilRef( fi.fi_user_ref );
        b30fdir_to_treeid( fi, fileId );
        bd30BuildCurrent( trans, fileId, current );
        if ( e_ok == trError ) {
            b30new_tree( fi, current );
            if ( fi.fi_root == 0 ) {
                // we cannot use root 0 because then
                // fileRootCheck_gg00 would still be ROOT_CHECK_GG00
                // and we could not distinguish logical and instance
                // file ids
                //
                // PTS 1132072 M.Ki.
                // request new pno while still holding root 0 to make
                // sure it won't be returned again
                tbd_current_tree  zeroCurrent = current;
                const Data_PageNo c_zeroRoot( 0 );

                current.curr_tree_id.fileRoot_gg00() = NIL_PAGE_NO_GG00;
                b30new_tree( fi, current );

                SAPDBERR_ASSERT_STATE( fi.fi_root != 0 );

                tgg00_BasisError auxErr = trans.trError_gg00;
                bd13FreeTempPageNo(
                    c_zeroRoot, LEAF_LEVEL_BD00, zeroCurrent );
                SAPDBERR_ASSERT_STATE( trans.trError_gg00 == e_ok );
                trans.trError_gg00 = auxErr;
            }
        }

        if ( e_ok == trError ) {
            // add file to file directory
            FileDir_ReturnCode ret;
            ret = directory.AddTableFile(
                trans.trTaskId_gg00,
                fileNo,
                Converter_ICommon::Instance().Version(),
                fi.fi_root,
                0,
                fi.fi_type,
                fi.fi_vers,
                msgList,
                &ft);

            if ( ret == FileDir_Okay ) {
                if ( tempFileType != Data_TempFileTypes::None ) {
                    ft.SetComment(
                        Data_TempFileTypes::GetTypeName( tempFileType ) );
                }
                ft.SetLeafPageCount( 1 );
                ft.SetIndexPageCount( 0 );
                fileId = current.curr_tree_id;
                // store temp fileType in in fileTempCnt_gg00 for easy
                // retrieval without fdir lookup
                fileId.fileName_gg00()[ 15 ] =
                    Data_TempFileTypes::MapFileTypeToTfnTemp( tempFileType );
            } else {
                bd13FreeTempPageNo( fi.fi_root,  LEAF_LEVEL_BD00, current );
                trError = MapFileDirReturnCode( ret );
            }
        }
    }

    if ( g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdPrim_gg00 ) {
        tgg11_BdRootTrace rootTrace;
        rootTrace.bdrTrType_gg11.becomes( bdtrRoot_egg11 );
        rootTrace.bdrRoot_gg11   = fi.fi_root;
        rootTrace.bdrError_gg11  = trError;
        Trace_CommonEntry Entry(
            tgg00_Debug::fromConst(bd),
            tgg00_VtraceType::fromConst(b01t_create),
            reinterpret_cast<SAPDB_Byte*>(&rootTrace), sizeof(rootTrace) );
    }

#if defined(TRACE)
    t01treeid      (bi, "new treeid  ", fileId);
    t01basis_error (bi, "create err  ", trError);
#endif
    AppendToContextMessageList( msgList );
}

/*************************************************************************/

/*!
 * Creates a temp file. Will overwrite fileId.fileTabId_gg00 with a
 * new temporary fileno.
 */
externPascal void bd998CreateTempFile (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    tgg00_FileId          VAR_VALUE_REF  fileId)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998CreateTempFile", FileDir_Trace, 5 );

    SAPDBERR_ASSERT_STATE( fileId.fileTfn_gg00() != tfnColumn_egg00 );

    FileDir_FileNo fileNo = GetFileDirectory( trans ).GetNewTempFileNo();

    bool isExtendedTempFile = bd998IsExtendedTempFile( trans, fileId );
    tgg00_TfnTemp tempFileType;
    tempFileType.becomes( ttfnNone_egg00 );

    if ( isExtendedTempFile ) {
        tempFileType.becomes( bd998GetExtendedTempFileType( trans, fileId ) );
        // store file type in last byte:
        fileId.fileName_gg00()[ 15 ] = tempFileType;
    }

    bd998CreateNamedTempFile(
        trans,
        *reinterpret_cast<tgg00_Surrogate*>( &fileNo ),
        fileId,
        isExtendedTempFile,
        tempFileType);
}

/*************************************************************************/

/// Creates a new temp file with a given fileNo.
externPascal void bd998CreateNamedTempFile (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    const tgg00_Surrogate  VAR_ARRAY_REF  fileSurrogate,
    tgg00_FileId          VAR_VALUE_REF  fileId,
    pasbool                              isExtendedTempFile,
    tgg00_TfnTemp_Param                  tempFileType)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998CreateNamedTempFile", FileDir_Trace, 5 );

    tgg00_BasisError& trError = trans.trError_gg00;
    tbd_current_tree      current;
    FileDir_Table         ft;

    SAPDBERR_ASSERT_STATE( fileId.fileTfn_gg00() != tfnColumn_egg00 );

    // determine temp file type if it is an extended temp file
//     Data_TempFileTypes::FileType tempFileType = Data_TempFileTypes::None;
//     if ( bd998IsExtendedTempFile( trans, fileId ) ) {
//         tempFileType =
//             bd998GetExtendedTempFileType( trans, fileId ) );
//     }

    SAPDBTRACE_WRITELN(
        FileDir_Trace, 5,
        "logical id : "
        << SAPDBTrace_Hex(&fileId.fileTabId_gg00(),
                          sizeof(tgg00_Surrogate))
        );

    FileDir_IDirectory& directory = GetFileDirectory( trans );
    fileId.fileTabId_gg00() = fileSurrogate;
    const FileDir_FileNo fileNo( fileSurrogate );
    Msg_List msgList;

    SAPDBTRACE_WRITELN(
        FileDir_Trace, 5,
        "actual id : " << fileNo );

    if ( g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdPrim_gg00 ) {
        tgg11_BdFilenameTrace fnTrace;
        fnTrace.bdfTrType_gg11.becomes( bdtrFilename_egg11 );
        fnTrace.bdfFn_gg11     = fileId.fileName_gg00();
        Trace_CommonEntry Entry(
            tgg00_Debug::fromConst(bd),
            tgg00_VtraceType::fromConst(b01t_create),
            reinterpret_cast<SAPDB_Byte*>(&fnTrace), sizeof(fnTrace) );
    }
#if defined(TRACE)
    t01name( bi, "====> bdCreateTemp");
    t01treeid( bi, "treeid req  ", fileId);
#endif
    if ( b01downfilesystem ) {
        trError = e_shutdown;
    } else {
        trError = e_ok;
        tbd_fileinfo fi;
        fileId.fileResultSite_gg00().rawAssign( cgg_zero_c2 );
        fi.fi_type = fileId.fileType_gg00();
        fi.fi_state.clear();
        fi.fi_fill1 = false;
        fi.fi_descr.clear();
        fi.fi_root = NIL_PAGE_NO_GG00;
        fi.fi_vers.gg91SetDummyVers();
        fi.fi_col_cnt = 0;
        gg06SetNilRef( fi.fi_user_ref );
        b30fdir_to_treeid( fi, fileId );
        bd30BuildCurrent( trans, fileId, current );
        if ( e_ok == trError ) {
            b30new_tree( fi, current );
            if ( fi.fi_root == 0 ) {
                // we cannot use root 0 because then
                // fileRootCheck_gg00 would still be ROOT_CHECK_GG00
                // and we could not distinguish logical and instance
                // file ids
                //
                // PTS 1132072 M.Ki.
                // request new pno while still holding root 0 to make
                // sure it won't be returned again
                tbd_current_tree  zeroCurrent = current;
                const Data_PageNo c_zeroRoot( 0 );

                current.curr_tree_id.fileRoot_gg00() = NIL_PAGE_NO_GG00;
                b30new_tree( fi, current );

                SAPDBERR_ASSERT_STATE( fi.fi_root != 0 );

                tgg00_BasisError auxErr = trans.trError_gg00;
                bd13FreeTempPageNo(
                    c_zeroRoot, LEAF_LEVEL_BD00, zeroCurrent );
                SAPDBERR_ASSERT_STATE( trans.trError_gg00 == e_ok );
                trans.trError_gg00 = auxErr;
            }
        }

        if ( e_ok == trError ) {
            // add file to file directory
            FileDir_ReturnCode ret;
            if ( ! fileId.fileType_gg00().includes( ftsShared_egg00 ) ) {
                ret = directory.AddTableFile(
                    trans.trTaskId_gg00,
                    fileNo,
                    Converter_ICommon::Instance().Version(),
                    fi.fi_root,
                    0,
                    fi.fi_type,
                    fi.fi_vers,
                    msgList,
                    &ft);
            } else {
                ret = directory.AddSharedTempTableFile(
                    trans.trTaskId_gg00,
                    fileNo,
                    Converter_ICommon::Instance().Version(),
                    fi.fi_root,
                    0,
                    fi.fi_type,
                    msgList,
                    &ft);
            }

            if ( ret == FileDir_Okay ) {
                ft.SetLeafPageCount( 1 );
                ft.SetIndexPageCount( 0 );
                fileId = current.curr_tree_id;
                if ( isExtendedTempFile ) {
                    // store temp fileType in last byte of filename for
                    // easy retrieval without fdir lookup
                    fileId.fileName_gg00()[ 15 ] = tempFileType;
                    if ( tempFileType != ttfnNone_egg00 ) {
                        ft.SetComment(
                            Data_TempFileTypes::GetTypeName(
                                Data_TempFileTypes::MapTfnTempToFileType(
                                    tempFileType ) )
                            );
                    }
                }
            } else {
                bd13FreeTempPageNo( fi.fi_root,  LEAF_LEVEL_BD00, current );
                trError = MapFileDirReturnCode( ret );
            }
        }
    }

    if ( g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdPrim_gg00 ) {
        tgg11_BdRootTrace rootTrace;
        rootTrace.bdrTrType_gg11.becomes( bdtrRoot_egg11 );

        rootTrace.bdrRoot_gg11   = fileId.fileRoot_gg00();
        rootTrace.bdrError_gg11  = trError;
        Trace_CommonEntry Entry(
            tgg00_Debug::fromConst(bd),
            tgg00_VtraceType::fromConst(b01t_create),
            reinterpret_cast<SAPDB_Byte*>(&rootTrace), sizeof(rootTrace) );
    }

#if defined(TRACE)
    t01treeid      (bi, "new treeid  ", fileId);
    t01basis_error (bi, "create err  ", trError);
#endif
    AppendToContextMessageList( msgList );
}

/*************************************************************************/

/// Destroys a given temp file.
externPascal void bd998DestroyTempFile (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    const tgg00_Surrogate  VAR_ARRAY_REF  fileSurrogate)
{

#if defined(TRACE)
    t01name( bi, "====> bd998DestroyTempFile");
    t01surrogate( bi, "treeid req  ", fileSurrogate);
#endif

    trans.trError_gg00 = e_ok;

    Msg_List msgList;
    FileDir_Generic fg;
    FileDir_FileNo fileNo( fileSurrogate );
    FileDir_IDirectory& fd = GetFileDirectory( trans );

    fd.GcGetFile( trans.trTaskId_gg00, fileNo, fg, msgList );
    if ( ! fg.IsAssigned() ) {
        trans.trError_gg00 = e_file_not_found;

        if ( g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdPrim_gg00 ) {
            Kernel_VTrace()
                << "bd998DestroyTempFile " << fileNo << "; "
                << SQLManErr_Interface::GetInstance().GetBaseErrorText(
                    trans.trError_gg00);
        }
        return;
    }

    if ( g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdPrim_gg00 ) {
        Kernel_VTrace() << "bd998DestroyTempFile " << fileNo
                        << "  root = " << fg.GetRootPage();
    }

    tgg00_FileId fileId;
    fg.BuildFileId( fileId );
    fileId.fileBdUse_gg00().addElement( bd_release_acc );

    SAPDB_Int blobColCount = 0;
    switch ( fg.GetFileType() ) {
        case Data_TableFile:
        {
            FileDir_Table ft;
            tbd_fileinfo fileInfo;
            ft.Assign( fg );
            ft.FillFileInfo( fileInfo );
            blobColCount = fileInfo.fi_col_cnt;
            break;
        }
    }

    FileDir_ReturnCode retCode = fg.MarkFileDeleted( trans.trWriteTransId_gg00 );
    if ( (retCode != FileDir_Okay)
         && (retCode != FileDir_AlreadyMarkedForDeletion) ) {
        RTE_Crash(
            SAPDBErr_Exception(
                __CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED,
                "MarkForDeletion must succeed") );
    }

    if ( trans.trError_gg00 == e_ok ) {
        fd.DelFile( trans.trTaskId_gg00, fileNo, msgList );
    }

    tbd_current_tree current;
    bd30BuildCurrent( trans, fileId, current );

    bd30GetTree( trans, fileId, current, m_drop, LOCK_TREE_EXCL_BD00, ! SYNC_TREE_LOCK_BD00 );

    bd30DropTree( current, blobColCount, !c_only_empty );

    bd30ReleaseTree( current );

    if ( trans.trError_gg00 == e_bad_datapage ) {
        trans.trError_gg00 = e_ok;
    }

//     if ( g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdPrim_gg00 ) {
//         tgg11_BdRootTrace rootTrace;
//         rootTrace.bdrTrType_gg11.becomes( bdtrRoot_egg11 );
//         rootTrace.bdrRoot_gg11   = fileId.fileRoot_gg00();
//         rootTrace.bdrError_gg11  = trans.trError_gg00;
//         Trace_CommonEntry Entry(
//             tgg00_Debug::fromConst(bd),
//             tgg00_VtraceType::fromConst(b01destroy),
//             reinterpret_cast<SAPDB_Byte*>(&rootTrace), sizeof(rootTrace) );
//     }

    if ( g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdPrim_gg00 ) {
        Kernel_VTrace()
            << "bd998DestroyTempFile root = "
            << fileId.fileRoot_gg00() << "; "
            << SQLManErr_Interface::GetInstance().GetBaseErrorText(
                trans.trError_gg00);
    }
    AppendToContextMessageList( msgList );
}

/*************************************************************************/

/// Wrapper to set the leaf and index page count as well as the record
/// count.
externPascal void bd998SetPageAndRecordCount (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    tgg00_Surrogate       VAR_ARRAY_REF  fileTabId,
    SAPDB_Int4                           leafPageCount,
    SAPDB_Int4                           indexPageCount,
    SAPDB_Int4                           entryCount)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998SetPageAndRecordCount", FileDir_Trace, 5 );

    SAPDBERR_ASSERT_ARGUMENT( leafPageCount >= 0 );
    SAPDBERR_ASSERT_ARGUMENT( indexPageCount >= 0 );

    SAPDBERR_ASSERT_ARGUMENT( entryCount >= 0 );

    if ( ( leafPageCount  < 0 ) ||
         ( indexPageCount < 0 ) ||
         ( entryCount     < 0 ) ) {
        return;
    }

    FileDir_IDirectory& fd = GetFileDirectory( trans );

    Msg_List msgList;
    FileDir_Generic fg;
    FileDir_FileNo fileNo( fileTabId );
    FileDir_ReturnCode retCode =
        fd.GetFile( trans.trTaskId_gg00, fileNo, fg, msgList );

    if ( retCode != FileDir_Okay ) {
        trans.trError_gg00 = MapFileDirReturnCode( retCode );
        return;
    }

    fg.SetLeafPageCount( leafPageCount );
    fg.SetIndexPageCount( indexPageCount );
    fg.SetEntryCount( entryCount );
    AppendToContextMessageList( msgList );
}

/*************************************************************************/

/// Wrapper to modify the leaf and index page count as well as the
/// record count.
externPascal void bd998ModifyPageAndRecordCount (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    tgg00_ConverterVersion               converterVersion,
    tgg00_Surrogate       VAR_ARRAY_REF  fileTabId,
    SAPDB_Int4                           leafPageCountDelta,
    SAPDB_Int4                           indexPageCountDelta,
    SAPDB_Int4                           entryCountDelta)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998ModifyPageAndRecordCount", FileDir_Trace, 5 );

    FileDir_IDirectory& fd = GetFileDirectory( trans );

    Msg_List msgList;
    FileDir_Generic fg;
    FileDir_FileNo fileNo( fileTabId );

    if ( ! fileNo.IsValid() ) {
        // this is necessary for the migration from the old to the new
        // file directory during the drop operation of the old style
        // file directory tree
        return;
    }

    // this wrapper also gets called in the course of undo/redo,
    // therefore GcGetFile has to be used:
    FileDir_ReturnCode retCode = fd.GcGetFile(
        trans.trTaskId_gg00, fileNo, fg, msgList );

    if ( retCode != FileDir_Okay ) {
        trans.trError_gg00 = MapFileDirReturnCode( retCode );
        return;
    }

    if ( fg.GetLeafPageCount( trans.trTaskId_gg00 ) == 0 ) {
        // page and entry count values of this file have not yet been
        // initialized; the page count has to be set to a non-zero
        // value before we will modify it
        return;
    }

    if ( leafPageCountDelta ) {
        fg.ModifyLeafPageCount(
            trans.trTaskId_gg00, converterVersion, leafPageCountDelta );
    }

    if ( indexPageCountDelta ) {
        fg.ModifyIndexPageCount(
            trans.trTaskId_gg00, converterVersion, indexPageCountDelta );
    }

    if ( entryCountDelta ) {
        fg.ModifyEntryCount(
            trans.trTaskId_gg00, converterVersion, entryCountDelta );
    }
    AppendToContextMessageList( msgList );
}

/*************************************************************************/

/// Wrapper to retrieve the leaf page, index page and record count.
externPascal void bd998GetPageAndRecordCount (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    tgg00_Surrogate       VAR_ARRAY_REF  fileTabId,
    SAPDB_Int4            VAR_VALUE_REF  leafPageCount,
    SAPDB_Int4            VAR_VALUE_REF  indexPageCount,
    SAPDB_Int4            VAR_VALUE_REF  entryCount)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998GetLeafPageAndRecordCount", FileDir_Trace, 5 );

    trans.trError_gg00 = e_ok;

    FileDir_IDirectory& fd = GetFileDirectory( trans );

    Msg_List msgList;
    FileDir_Generic fg;
    FileDir_FileNo fileNo( fileTabId );
    FileDir_ReturnCode retCode =
        fd.GetFile( trans.trTaskId_gg00, fileNo, fg, msgList );

    if ( retCode != FileDir_Okay ) {
        trans.trError_gg00 = MapFileDirReturnCode( retCode );
        return;
    }

    leafPageCount = static_cast<SAPDB_Int4> (
        fg.GetLeafPageCount( trans.trTaskId_gg00 ) );

    indexPageCount = static_cast<SAPDB_Int4> (
        fg.GetIndexPageCount( trans.trTaskId_gg00 ) );

    entryCount = static_cast<SAPDB_Int4> (
        fg.GetEntryCount( trans.trTaskId_gg00 ) );

    if ( leafPageCount < 0 ) {
        leafPageCount = 0;
    }

    if ( indexPageCount < 0 ) {
        indexPageCount = 0;
    }

    if ( entryCount < 0 ) {
        entryCount = 0;
    }
    AppendToContextMessageList( msgList );
}

/*************************************************************************/

/// Wrapper to set the lob page count of a table this will implicitely
/// mark the lob page count of that table initialized
externPascal void bd998SetLobPageCount (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    tgg00_Surrogate       VAR_ARRAY_REF  fileTabId,
    SAPDB_Int4                           lobPageCount)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998SetLobPageCount", FileDir_Trace, 5 );

    FileDir_IDirectory& fd = GetFileDirectory( trans );

    Msg_List msgList;
    FileDir_Generic fg;
    FileDir_FileNo fileNo( fileTabId );

    // this wrapper also gets called in the course of undo/redo,
    // therefore GcGetFile has to be used:
    FileDir_ReturnCode retCode = fd.GcGetFile(
        trans.trTaskId_gg00, fileNo, fg, msgList );

    if ( retCode != FileDir_Okay ) {
        trans.trError_gg00 = MapFileDirReturnCode( retCode );
        return;
    }

    if ( fg.GetFileType() != Data_TableFile ) {
        trans.trError_gg00 = MapFileDirReturnCode( FileDir_FileTypeMismatch );
        return;
    }

    FileDir_Table tableEntry;
    tableEntry.Assign( fg );
    const SAPDB_Bool bSetInitialized = true;
    tableEntry.SetLobPageCount( lobPageCount, bSetInitialized );

    AppendToContextMessageList( msgList );
}

/*************************************************************************/

/// Wrapper to modify the leaf and index page count as well as the
/// record count.
externPascal void bd998ModifyLobPageCount (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    tgg00_ConverterVersion               converterVersion,
    const tgg00_Surrogate VAR_ARRAY_REF  fileTabId,
    SAPDB_Int4                           lobPageCountDelta)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998ModifyLobPageCount", FileDir_Trace, 5 );

    FileDir_IDirectory& fd = GetFileDirectory( trans );

    Msg_List msgList;
    FileDir_Generic fg;
    FileDir_FileNo fileNo( fileTabId );

    if ( ! fileNo.IsValid() ) {
        // this is necessary for the migration from the old to the new
        // file directory during the drop operation of the old style
        // file directory tree
        return;
    }

    // this wrapper also gets called in the course of undo/redo,
    // therefore GcGetFile has to be used:
    FileDir_ReturnCode retCode = fd.GcGetFile(
        trans.trTaskId_gg00, fileNo, fg, msgList );

    if ( retCode != FileDir_Okay ) {
        trans.trError_gg00 = MapFileDirReturnCode( retCode );
        return;
    }

    if ( fg.GetFileType() != Data_TableFile ) {
        trans.trError_gg00 = MapFileDirReturnCode( FileDir_FileTypeMismatch );
        return;
    }

    FileDir_Table tableEntry;
    tableEntry.Assign( fg );
    if ( ! tableEntry.IsLobPageCounterInitialized() ) {
        // lob page count of this file has not yet been initialized
        return;
    }

    tableEntry.ModifyLobPageCount(
        trans.trTaskId_gg00, converterVersion, lobPageCountDelta );

    AppendToContextMessageList( msgList );
}

/*************************************************************************/

/// Wrapper to create an iterator over the session and shared file
/// directory.
externPascal void bd998NewFileDirIterator (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    FileDir_Iterator*&                   iteratorPtr)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998NewFileDirIterator", FileDir_Trace, 5 );

    FileDir_IDirectory& fd = GetFileDirectory( trans );

    iteratorPtr =
        new ( fd.GetAllocator() ) FileDir_Iterator( fd.GetAllocator() );

    if ( iteratorPtr == 0 ) {
        trans.trError_gg00 = e_no_more_memory;
    }
}

/*************************************************************************/

/// Wrapper to destroy an iterator created with bd998NewFileDirIterator.
externPascal void bd998DestroyFileDirIterator (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    FileDir_Iterator*&                   iteratorPtr)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998DestroyFileDirIterator", FileDir_Trace, 5 );

    FileDir_IDirectory& fd = GetFileDirectory( trans );

    destroy( iteratorPtr, fd.GetAllocator() );
}

/*************************************************************************/

/// Wrapper to add a file type to the filter of a file dir iterator
/// created with bd998NewFileDirIterator.
externPascal void bd998AddToFileDirIteratorFilter (
    FileDir_Iterator*&                   iteratorPtr,
    tgg00_Tfn_Param                      fileType)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998AddToFileDirIteratorFilter", FileDir_Trace, 5 );

    if ( ! iteratorPtr ) {
        return;
    }

    switch ( fileType ) {
        case tfnSys_egg00:
        case tfnTable_egg00:
            iteratorPtr->AddFileTypeToFilter( Data_TableFile );
            break;
        case tfnMulti_egg00:
            iteratorPtr->AddFileTypeToFilter( Data_IndexFile );
            break;
        case tfnShortScol_egg00:
            iteratorPtr->AddFileTypeToFilter( Data_ShortColumnFile );
            break;
        case tfnObj_egg00:
            iteratorPtr->AddFileTypeToFilter( Data_OmsFile );
            iteratorPtr->AddFileTypeToFilter( Data_OmsFixedFile );
            iteratorPtr->AddFileTypeToFilter( Data_OmsKeyFile );
            iteratorPtr->AddFileTypeToFilter( Data_OmsKeyPartitionFile );
            iteratorPtr->AddFileTypeToFilter( Data_OmsVarFile );
            iteratorPtr->AddFileTypeToFilter( Data_OmsContFile );
            break;
    }
}

/*************************************************************************/

/// Wrapper to initialize a file dir iterator created with
/// bd998NewFileDirIterator.
externPascal void bd998InitFileDirIterator (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    pasbool                              showDeletedFiles,
    FileDir_Iterator*&                   iteratorPtr)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998InitFileDirIterator", FileDir_Trace, 5 );

    FileDir_IDirectory& fd = GetFileDirectory( trans );

    if ( iteratorPtr ) {
        if ( ! showDeletedFiles ) {
            iteratorPtr->AddHideDeletedFilesFilter();
        }
        fd.GetFileDirIterator( *iteratorPtr, &trans );
    }
}

/*************************************************************************/

/// Wrapper to initialize a file dir iterator created with
/// bd998NewFileDirIterator.
externPascal void bd998InitPrivateFileDirIterator (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    pasbool                              showDeletedFiles,
    FileDir_Iterator*&                   iteratorPtr)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998InitFileDirIterator", FileDir_Trace, 5 );

    FileDir_IDirectory& fd = GetFileDirectory( trans );

    if ( iteratorPtr ) {
        if ( ! showDeletedFiles ) {
            iteratorPtr->AddHideDeletedFilesFilter();
        }
        fd.GetPrivateFileDirIterator( *iteratorPtr, &trans );
    }
}

/*************************************************************************/

/// Wrapper to advance a file dir iterator created with
/// bd998NewFileDirIterator by one.
externPascal void bd998AdvanceFileDirIterator (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    FileDir_Iterator*&                   iteratorPtr,
    tgg00_FileId          VAR_VALUE_REF  fileId)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998AdvanceFileDirIterator", FileDir_Trace, 5 );

    if ( iteratorPtr == 0 ) {
        trans.trError_gg00 = e_system_error;
        return;
    }

    FileDir_Generic entry;
    iteratorPtr->GetNext( entry);

    if ( ! entry.IsAssigned() ) {
        trans.trError_gg00 = e_file_not_found;
        return;
    }

    entry.BuildFileId( fileId );
}

/*************************************************************************/

/// Wrapper to create a new iterator over all index files marked as bad.
externPascal void bd998NewBadIndexIterator (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    FileDir_Iterator*&                   iteratorPtr)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998NewBadIndexIterator", FileDir_Trace, 5 );

    FileDir_IDirectory& fd = GetFileDirectory( trans );

    iteratorPtr =
        new ( fd.GetAllocator() ) FileDir_Iterator( fd.GetAllocator() );

    if ( iteratorPtr ) {
        iteratorPtr->AddFileTypeToFilter( Data_IndexFile );
        iteratorPtr->AddHideDeletedFilesFilter();
        fd.GetFileDirIterator( *iteratorPtr, &trans );
    } else {
        trans.trError_gg00 = e_no_more_memory;
    }
}

/*************************************************************************/

/// Wrapper to destroy an iterator created with
/// bd998NewBadIndexIterator.
externPascal void bd998DestroyBadIndexIterator (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    FileDir_Iterator*&                   iteratorPtr)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998DestroyBadIndexIterator", FileDir_Trace, 5 );

    FileDir_IDirectory& fd = GetFileDirectory( trans );

    destroy( iteratorPtr, fd.GetAllocator() );
}

/*************************************************************************/

/// Wrapper to advance an iterator created with
/// bd998NewBadIndexIterator by one.
externPascal void bd998AdvanceBadIndexIterator (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    FileDir_Iterator*&                   iteratorPtr,
    tgg00_FileId          VAR_VALUE_REF  fileId)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998AdvanceFileDirIterator", FileDir_Trace, 5 );

    if ( iteratorPtr == 0 ) {
        trans.trError_gg00 = e_system_error;
        return;
    }

    FileDir_Generic entry;
    while ( iteratorPtr->GetNext( entry)
            && ( entry.GetFileState() != Data_FileNotAccessible ) ) {
        ;
    }

    if ( ! entry.IsAssigned() ) {
        trans.trError_gg00 = e_file_not_found;
        return;
    }

    entry.BuildFileId( fileId );
}

/*************************************************************************/

externPascal void bd998GetFileInfo (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    const tgg00_Surrogate  VAR_ARRAY_REF  surrogate,
    tgg00_FileId          VAR_VALUE_REF  fileId,
    tgg91_FileVersion     VAR_VALUE_REF  fileVersion,
    tbd_file_state_set    VAR_ARRAY_REF  fileState)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998BuildFileId", FileDir_Trace, 5 );

    FileDir_IDirectory& fileDir = GetFileDirectory( trans );

    Msg_List msgList;
    FileDir_Generic fileEntry;
    FileDir_FileNo fileNo( surrogate );
    FileDir_ReturnCode retCode =
        fileDir.GetFile(
            trans.trTaskId_gg00,
            fileNo,
            fileEntry,
            msgList);

    if ( retCode == FileDir_Okay ) {
        fileEntry.FillPreparedFileId( fileId );
        fileEntry.GetFileVersion( fileVersion );

        fileState.clear();
        switch ( fileEntry.GetFileState() ) {
        case Data_FileBad:
            fileState.addElement( f_bad );
            break;
        case Data_FileNotAccessible:
            fileState.addElement( f_not_accessible );
            break;
        case Data_FileReadOnly:
            fileState.addElement( f_read_only );
            break;
        }
        bd998IsTempFile( trans, fileId ); // FIXME: remove
    } else {
        trans.trError_gg00 = MapFileDirReturnCode( retCode );
    }
}

/*************************************************************************/

externPascal void bd998GetTableBlobColCount(
    tgg00_TransContext    VAR_VALUE_REF  trans,
    const tgg00_Surrogate VAR_ARRAY_REF  tableSurrogate,
    tsp00_Int4            VAR_VALUE_REF  blobColCount,
    tgg00_BasisError      VAR_VALUE_REF  bErr)
{
    SAPDBTRACE_ROUTINE_DEBUG( "bd998GetTableBlobColCount", FileDir_Trace, 5 );

    bErr = e_ok;

    FileDir_FileNo tableFileNo( tableSurrogate );
    FileDir_IDirectory& fileDir = GetFileDirectory( trans );

    Msg_List msgList;
    FileDir_Table tableEntry;
    FileDir_ReturnCode retCode =
        fileDir.GetTableFile(
            trans.trTaskId_gg00,
            tableFileNo,
            tableEntry,
            msgList);

    if ( retCode == FileDir_Okay ) {
        blobColCount = tableEntry.GetBlobColCount();
    } else {
        bErr = MapFileDirReturnCode( retCode );
    }
}

/*************************************************************************/

externPascal
void bd998ResetAllCachedLockStates (tsp00_Addr  VAR_VALUE_REF  LocalFileDir)
{
    reinterpret_cast<FileDir_IDirectory*>(LocalFileDir)->SetAllCachedLockStates(0);
};

/*************************************************************************/

externPascal
void bd998IncrementIndexUsageCount (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    const tgg00_Surrogate  VAR_ARRAY_REF  indexFileSurr)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998IncrementIndexUsageCount", FileDir_Trace, 5 );

    trans.trError_gg00 = e_ok;

    Trans_Context&      transContext = Trans_Context::GetContext( trans );
    FileDir_IDirectory& fdir = FileDir_IDirectory::SessionInstance( trans );
    FileDir_Index       indexEntry;

    const FileDir_ReturnCode rc =
        fdir.GetIndexFile(
            transContext.TaskId(),
            indexFileSurr,
            indexEntry,
            transContext.ErrorList() );

    if ( FileDir_Okay ==  rc ) {
        indexEntry.IncrementUsageCount( transContext.TaskId() );
    } else {
        if ( FileDir_FileNoNotFound == rc ) {
            trans.trError_gg00 = e_inv_not_found;
        } else {
            trans.trError_gg00 = e_system_error;
        }
    }
}

/*************************************************************************/

externPascal
void bd998ResetIndexUsageCount (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    const tgg00_Surrogate  VAR_ARRAY_REF  indexFileSurr)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998ResetIndexUsageCount", FileDir_Trace, 5 );

    trans.trError_gg00 = e_ok;

    Trans_Context&      transContext = Trans_Context::GetContext( trans );
    FileDir_IDirectory& fdir = FileDir_IDirectory::SessionInstance( trans );
    FileDir_Index       indexEntry;

    const FileDir_ReturnCode rc =
        fdir.GetIndexFile(
            transContext.TaskId(),
            indexFileSurr,
            indexEntry,
            transContext.ErrorList() );

    if ( FileDir_Okay ==  rc ) {
        indexEntry.ResetUsageCount( transContext.TaskId() );
    } else {
        if ( FileDir_FileNoNotFound == rc ) {
            trans.trError_gg00 = e_inv_not_found;
        } else {
            trans.trError_gg00 = e_system_error;
        }
    }
}

/*************************************************************************/

externPascal
tsp00_Int4 bd998GetIndexUsageCount (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    const tgg00_Surrogate  VAR_ARRAY_REF  indexFileSurr)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998IGetIndexUsageCount", FileDir_Trace, 5 );

    trans.trError_gg00 = e_ok;

    Trans_Context&      transContext = Trans_Context::GetContext( trans );
    FileDir_IDirectory& fdir = FileDir_IDirectory::SessionInstance( trans );
    FileDir_Index       indexEntry;

    const FileDir_ReturnCode rc =
        fdir.GetIndexFile(
            transContext.TaskId(),
            indexFileSurr,
            indexEntry,
            transContext.ErrorList() );

    if ( FileDir_Okay !=  rc ) {
        if ( FileDir_FileNoNotFound == rc ) {
            trans.trError_gg00 = e_inv_not_found;
        } else {
            trans.trError_gg00 = e_system_error;
        }
        return -1;
    }

    return indexEntry.GetUsageCount();
}

/*************************************************************************/

externPascal pasbool bd998GetNewSysKeyForFile (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    const tgg00_Surrogate  VAR_ARRAY_REF  tableSurr,
    tgg00_Surrogate       VAR_ARRAY_REF  sysKey)
{
    Trans_Context&      transContext = Trans_Context::GetContext( trans );
    FileDir_IDirectory& fdir = FileDir_IDirectory::SessionInstance( trans );

    FileDir_Table tableEntry;

    const FileDir_ReturnCode rc =
        fdir.GetTableFile(
            transContext.TaskId(),
            tableSurr,
            tableEntry,
            transContext.ErrorList() );

    if ( FileDir_Okay !=  rc ) {
        if ( FileDir_FileNoNotFound == rc ) {
            trans.trError_gg00 = e_file_not_found;
        } else {
            trans.trError_gg00 = e_system_error;
        }
        return false;
    }

    if ( tableEntry.GetNewSysKey( sysKey ) == FileDir_Table::skOkay ) {
        return true;
    } else {
        return false;
    }
}

/*************************************************************************/

externPascal pasbool bd998AddAndGetNewSysKeyForFile (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    const tgg00_Surrogate  VAR_ARRAY_REF  tableSurr,
    tgg00_Surrogate       VAR_ARRAY_REF  sysKey)
{
    Trans_Context&      transContext = Trans_Context::GetContext( trans );
    FileDir_IDirectory& fdir = FileDir_IDirectory::SessionInstance( trans );

    FileDir_Table tableEntry;

    const FileDir_ReturnCode rc =
        fdir.GetTableFile(
            transContext.TaskId(),
            tableSurr,
            tableEntry,
            transContext.ErrorList() );

    if ( FileDir_Okay !=  rc ) {
        if ( FileDir_FileNoNotFound == rc ) {
            trans.trError_gg00 = e_file_not_found;
        } else {
            trans.trError_gg00 = e_system_error;
        }
        return false;
    }

    if ( tableEntry.InitAndGetNewSysKey( sysKey ) == FileDir_Table::skOkay ) {
        return true;
    } else {
        return false;
    }
}

/*************************************************************************/

externPascal void bd998SetRecordsPacked (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    const tgg00_Surrogate  VAR_ARRAY_REF  surr,
    pasbool                              recordsPacked)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998SetRecordsPacked", FileDir_Trace, 5 );

    trans.trError_gg00 = e_ok;

    FileDir_Generic fileEntry;

    if ( ! bd998GetFile( trans, surr, fileEntry ) ) {
        return;
    }

    switch ( fileEntry.GetFileType() ) {
        case Data_TableFile:
        {
            FileDir_Table tableEntry;
            tableEntry.Assign( fileEntry );

            tableEntry.SetRecordsPacked( trans.trTaskId_gg00, recordsPacked );
            break;
        }
        case Data_IndexFile:
        {
            FileDir_Index indexEntry;
            indexEntry.Assign( fileEntry );

            indexEntry.SetRecordsPacked( trans.trTaskId_gg00, recordsPacked );
            break;
        }
        default:
            trans.trError_gg00 = e_invalid_filetype;
    }
}

/*************************************************************************/

externPascal pasbool bd998AreRecordsPacked (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    const tgg00_Surrogate  VAR_ARRAY_REF  surr)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998AreRecordsPacked", FileDir_Trace, 5 );

    trans.trError_gg00 = e_ok;

    FileDir_Generic fileEntry;

    if ( ! bd998GetFile( trans, surr, fileEntry ) ) {
        return false;
    }

    switch ( fileEntry.GetFileType() ) {
        case Data_TableFile:
        {
            FileDir_Table tableEntry;
            tableEntry.Assign( fileEntry );

            return tableEntry.AreRecordsPacked();
        }
        case Data_IndexFile:
        {
            FileDir_Index indexEntry;
            indexEntry.Assign( fileEntry );

            return indexEntry.AreRecordsPacked();
        }
        default:
            trans.trError_gg00 = e_invalid_filetype;
    }
    return false;
}

/*************************************************************************/

externPascal void bd998SetPagesClustered (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    const tgg00_Surrogate  VAR_ARRAY_REF  surr,
    pasbool                              pagesClustered)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998SetPagesClustered", FileDir_Trace, 5 );

    trans.trError_gg00 = e_ok;

    FileDir_Generic fileEntry;

    if ( ! bd998GetFile( trans, surr, fileEntry ) ) {

        return;

    }

    fileEntry.SetPagesClustered( trans.trTaskId_gg00, pagesClustered );
}

/*************************************************************************/

externPascal pasbool bd998ArePagesClustered (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    const tgg00_Surrogate  VAR_ARRAY_REF  surr)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998ArePagesClustered", FileDir_Trace, 5 );

    trans.trError_gg00 = e_ok;

    FileDir_Generic fileEntry;

    if ( ! bd998GetFile( trans, surr, fileEntry ) ) {

        return false;

    }

    return fileEntry.ArePagesClustered();
}

/*************************************************************************/

externPascal void bd998GetExtendedFiletypeSet (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    const tgg00_Surrogate  VAR_ARRAY_REF  surrogate,
    tgg00_ExtendedFiletypeSet  VAR_ARRAY_REF  eFiletypeSet)
{
    SAPDBTRACE_ROUTINE_DEBUG(
        "bd998GetExtendedFiletypeSet", FileDir_Trace, 5 );

    trans.trError_gg00 = e_ok;

    FileDir_Generic fileEntry;

    if ( ! bd998GetFile( trans, surrogate, fileEntry ) ) {

        return;

    }

    eFiletypeSet.clear();
    if ( fileEntry.ArePagesClustered() ) {
        eFiletypeSet.addElement( eftsClustered_egg00 );
    }

    switch ( fileEntry.GetFileType() ) {
        case Data_TableFile:
        {
            FileDir_Table tableEntry;
            tableEntry.Assign( fileEntry );

            if ( tableEntry.AreRecordsPacked() ) {
                eFiletypeSet.addElement( eftsRecordsPacked_egg00 );
            }
            break;
        }
        case Data_IndexFile:
        {
            FileDir_Index indexEntry;
            indexEntry.Assign( fileEntry );

            if ( indexEntry.AreRecordsPacked() ) {
                eFiletypeSet.addElement( eftsRecordsPacked_egg00 );
            }
            break;
        }
    }
}

/*************************************************************************/
