/*!
   @file           Log_ActionFile.cpp
   @author         MartinKi
   @ingroup        ActionHandling
   @brief          Handles the unqualified delete operation on a table.

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
#include "Logging/Log_ActionUnqualifiedDelete.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "KernelCommon/Kernel_Trace.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "Logging/Log_Exceptions.hpp"
#include "Converter/Converter_ICommon.hpp"
#include "SQLManager/Catalog/Catalog_Cache.hpp"
#include "FileDirectory/FileDir_Iterator.hpp"
#include "FileDirectory/FileDir_IDirectory.hpp"
#include "FileDirectory/FileDir_ISharedDirectory.hpp"
#include "FileDirectory/FileDir_Index.hpp"
#include "FileDirectory/FileDir_ShortColumn.hpp"
#include "FileDirectory/FileDir_Table.hpp"
#include "FileDirectory/FileDir_Generic.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "Transaction/Trans_Context.hpp"
#include "hbd01.h"
#include "hbd30.h"
#include "hbd20_9.h"
#include "hbd13.h"
#include "gbd00.h"
#include "KernelCommon/Kernel_IAdminInfo.hpp"
#include "DataAccess/Data_FileTypes.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Hex.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "Messages/Msg_List.hpp"

/// Helper class to lock/release files on construction/destruction.
class AutoTreeLock
{
private:
    tbd_current_tree& m_current;
    bool              m_lockSet;

public:
    enum LockType {
        LockExclusive,
        LockNonExclusive
    };

    AutoTreeLock(
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

    ~AutoTreeLock()
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

//---------------------------------------------------------------------------

Log_ActionUnqualifiedDelete*
Log_ActionUnqualifiedDelete::CreateObject(
    tgg00_TransContext&   trans,
    const FileDir_FileNo& tableFileNo,
    const FileDir_FileNo& dropFileNo)
{
    SAPDBMem_IRawAllocator& alloc = Trans_Context::GetContext (trans).Allocator();

    return new (alloc) Log_ActionUnqualifiedDelete760007 (trans, tableFileNo, dropFileNo);
}

//---------------------------------------------------------------------------

Log_ActionUnqualifiedDelete*
Log_ActionUnqualifiedDelete::CreateObject(
    SAPDBMem_IRawAllocator& allocator,
    Log_ActionVersion       actionVersion)
{
    Log_ActionUnqualifiedDelete* ptr = 0;
    if ( actionVersion == actionVersionStartingWith760007 )
        ptr = new (allocator) Log_ActionUnqualifiedDelete760007 (allocator);
    else
        ptr = new (allocator) Log_ActionUnqualifiedDelete760000 ();

    return ptr;
}

//---------------------------------------------------------------------------

SAPDB_UInt
Log_ActionUnqualifiedDelete:: GetMinimalLengthToCopyEntry(Log_ActionVersion version)
{
    if ( actionVersionStartingWith760007 == version )
        return Log_ActionUnqualifiedDelete760007::GetMinimalLengthToCopyEntry();
    else
        return Log_ActionUnqualifiedDelete760000::GetMinimalLengthToCopyEntry();
}

//---------------------------------------------------------------------------

/*!
 * @class Log_ActionUnqualifiedDelete760007
 * @author MartinKi
 *
 * <b> Handles the unqualified delete operation on a table. </b>
 *
 * Online/Redo execution:
 *
 *    - add a copy of the current file entry for the table to the file
 *    directory and mark this new entry as deleted (on commit it and
 *    its contents will be removed by the garbage collector)
 *
 *    - reassign short column and index files of original table to
 *    entry to be deleted and mark them as deleted
 *
 *    - set file version in file to be deleted to dummy (needed
 *    because of prepared statements)
 *
 *    - store counter values of original entry in drop file entry and
 *    reset counters in original file directory entry
 *
 *    - create a new root page and set the original file directory
 *    entry's root page to this new root page
 *
 * Undo execution:
 *
 *     - reset the root page of the table entry to its original value
 *     (which is stored in the file entry that was to be dropped)
 *
 *     - reassign short column and index files to original entry and
 *     remove delete marks
 *
 *     - free the page that was reserved during Online/Redo execution
 *
 *     - remove the file directory entry created during Online/Redo
 *     execution
 *
 *    - restore original file version and counter values
 *
 *
 * Garbage collection (only for committed transactions):
 *
 *     - drop the file belonging to the new entry and remove this
 *     entry from the file directory
 */

Log_ActionUnqualifiedDelete760007::Log_ActionUnqualifiedDelete760007 (
    tgg00_TransContext&   trans,
    const FileDir_FileNo& tableFileNo,
    const FileDir_FileNo& dropFileNo)
    : m_Head           (tableFileNo, dropFileNo),
      m_secondaryFiles (Trans_Context::GetContext (trans).Allocator())
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionUnqualifiedDelete760007::Log_ActionUnqualifiedDelete760007",
                             LogAction_Trace, 5);

    tgg00_BasisError& trError = trans.trError_gg00;

    Msg_List errorList;
    FileDir_IDirectory &fileDir =
        FileDir_IDirectory::SessionInstance( trans );
    FileDir_Table tableFile;

    if ( fileDir.GetTableFile (
             trans.trTaskId_gg00, m_Head.tableFileNo,
             tableFile, errorList)
         != FileDir_Okay )
    {
        SAPDBERR_ASSERT_STATE (false);
        trError = e_bad_fdir;
        return;
    }

    // save short column and index file nos
    if ( tableFile.HasShortColumnFile())
    {
        if ( !m_secondaryFiles.Reserve(m_secondaryFiles.GetSize() + 1) )
        {
            trError = e_no_more_memory;
            return;
        }

        m_secondaryFiles.InsertEnd( tableFile.GetShortColumnFileNo() );
        ++m_Head.secondaryFileCount;
    }

    for (int i=1; i <= tableFile.GetMaxIndexId(); ++i)
    {
        FileDir_FileNo indexFileNo (tableFile.GetIndexFileNo (i));
        if (! indexFileNo.IsValid())
            continue;

        if ( !m_secondaryFiles.Reserve(m_secondaryFiles.GetSize() + 1) )
        {
            trError = e_no_more_memory;
            return;
        }

        m_secondaryFiles.InsertEnd( indexFileNo );
        ++m_Head.secondaryFileCount;
    }
}

//---------------------------------------------------------------------------

void Log_ActionUnqualifiedDelete760007::Execute (
    tgg00_TransContext &trans,
    ExecutionType       executionType) const
{
    SAPDBTRACE_METHOD_DEBUG (
        "Log_ActionUnqualifiedDelete760007::Execute",
        LogAction_Trace, 5);

    SAPDBTRACE_WRITELN(
        LogAction_Trace, 5,
        "  tableFileNo = " << m_Head.tableFileNo
        << ", dropFileNo = " << m_Head.dropFileNo);

    SAPDBERR_ASSERT_STATE (
        (executionType == OnlineExecution)
        || (executionType == RedoExecution));

    tgg00_BasisError &error = trans.trError_gg00;

    KERNEL_TRACE_BASIS_ERROR (error, LogAction_Trace, 5);

    if (e_ok != error)
        return;

    Msg_List errorList;
    FileDir_IDirectory &fileDir =
        FileDir_IDirectory::SessionInstance( trans );
    FileDir_Table tableFile;
    FileDir_Table dropFile;

    SAPDBERR_ASSERT_STATE (
        (executionType == OnlineExecution)
        || (executionType == RedoExecution));

    if ( fileDir.GetTableFile (
             trans.trTaskId_gg00, m_Head.tableFileNo,
             tableFile, errorList)
         != FileDir_Okay )
    {
        SAPDBERR_ASSERT_STATE (false);
        error = e_bad_fdir;
        return;
    }

    tbd_current_tree current;

    tgg00_FileId newTableFileId;
    tableFile.BuildFileId (newTableFileId);
    newTableFileId.fileRoot_gg00() = NIL_PAGE_NO_GG00;
    bd30BuildCurrent (trans, newTableFileId, current);
    if (e_ok != error)
        return;

    /*
     * Synchronise with savepoint by locking the newly create tree
     * exclusively. This will insure that the file directory entry of
     * dropFile will always exist only in the correct converter
     * version. Counters are secured by not changing them persistently
     * if they are changed during a savepoint (and in case of a crash
     * the Execute method will be called again, so this should not be
     * harmful). MarkDeletes on secondary files can however make it
     * into the savepoint and have to be considered during undo.
     */
    tbd_fileinfo fileInfo;
    tableFile.FillFileInfo (fileInfo);

    b30new_tree (fileInfo, current);
    if (e_ok != error)
        return;

    newTableFileId.fileRoot_gg00()      = current.curr_tree_id.fileRoot_gg00();
    newTableFileId.fileRootCheck_gg00() = current.curr_tree_id.fileRootCheck_gg00();
    AutoTreeLock lockTree( trans, newTableFileId, current,
                           AutoTreeLock::LockExclusive );
    if (e_ok != error )
        return;

    SAPDBTRACE_WRITELN(
        LogAction_Trace, 5,
        "  tableFileVersion: "
        << SAPDBTrace_Hex(&newTableFileId.fileVersion_gg00(),
                          sizeof(newTableFileId.fileVersion_gg00())) << NewLine
        << "  old root: " << tableFile.GetRootPage()
        << ", new root: " << newTableFileId.fileRoot_gg00());

    Converter_Version destConverter =
        bd20GetDestinationConverterVersion(
            trans.trTaskId_gg00, current.currRootNptrs_bd00.np_cbptr() );

    FileDir_ReturnCode retCode =
        fileDir.AddTableFile (
            trans.trTaskId_gg00,
            m_Head.dropFileNo,
            destConverter,
            tableFile.GetRootPage(),
            tableFile.GetBlobColCount(),
            fileInfo.fi_type,
            fileInfo.fi_vers,
            errorList,
            &dropFile);

    if (FileDir_Okay != retCode)
    {
        if ( (FileDir_FileNoExists == retCode)
             && (RedoExecution == executionType)
            )
        { // entry might already exist during redo as add to fdir was
          // not synchronized with savepoint
            fileDir.GcGetFile(
                trans.trTaskId_gg00, m_Head.dropFileNo, dropFile, errorList);
        }
        if ( (! dropFile.IsAssigned())
             || (Data_TableFile != dropFile.GetFileType()) )
        {
            SAPDBERR_ASSERT_STATE (false);
            error = e_bad_fdir;
            return;
        }
        // PTS 1133047 M.Ki.
        // make sure dropFile has correct root page; entry in file
        // directory might contain outdated root page if table was
        // created during redo
        dropFile.SetRootPage( trans.trTaskId_gg00, tableFile.GetRootPage() );
    }

    // store original page and entry count in dropFile
    dropFile.SetLeafPageCount (
        tableFile.GetLeafPageCount (trans.trTaskId_gg00));
    dropFile.SetIndexPageCount (
        tableFile.GetIndexPageCount (trans.trTaskId_gg00));
    dropFile.SetEntryCount (
        tableFile.GetEntryCount (trans.trTaskId_gg00));
    dropFile.SetLobPageCount(
        tableFile.GetLobPageCount ());

    // reassign short column and index files to dropFile
    FileDir_Generic curFileEntry;
    SecondaryFileList::ConstIterator itEnd = m_secondaryFiles.End();
    for (SecondaryFileList::ConstIterator it = m_secondaryFiles.Begin();
         it != itEnd;
         ++it)
    {
        fileDir.GcGetFile (trans.trTaskId_gg00, *it, curFileEntry, errorList);

        if ( !curFileEntry.IsAssigned() )
        {
            SAPDBERR_ASSERT_STATE (false);
            error = e_bad_fdir;
            return;
        }

        FileDir_ReturnCode retCode;
        switch (curFileEntry.GetFileType())
        {
            case Data_ShortColumnFile:
                retCode = fileDir.ReassignShortColumnFile (
                    trans.trTaskId_gg00,
                    curFileEntry.GetFileNo(),
                    dropFile.GetFileNo(), errorList);
                break;
            case Data_IndexFile:
                retCode = fileDir.ReassignIndexFile (
                    trans.trTaskId_gg00,
                    curFileEntry.GetFileNo(),
                    dropFile.GetFileNo(), errorList);
                break;
            default:
                SAPDBERR_ASSERT_STATE( false );
                error = e_bad_fdir;
                return;
        }

        if ( FileDir_Okay != retCode )
        {
            SAPDBERR_ASSERT_STATE (false);
            error = e_bad_fdir;
            return;
        }

        curFileEntry.MarkFileDeleted (trans.trWriteTransId_gg00);
    }

    // set dummy file version in file to make prepared statements fail
    tgg91_FileVersion fileVersion;
    tgg00_FileId      dropTableFileId;
    const SAPDB_Int2  c_dummyNewLongCnt = -1;

    dropFile.BuildFileId (dropTableFileId);
    fileVersion.gg91SetDummyVers();
    b01set_fileversion (
        trans, dropTableFileId, fileVersion, c_dummyNewLongCnt);

    if (e_ok != error)
        return;

    retCode = dropFile.MarkFileDeleted (trans.trWriteTransId_gg00);

    if ((retCode != FileDir_Okay)
        && ((retCode != FileDir_AlreadyMarkedForDeletion)
            || (RedoExecution != executionType))
        )
    {
        SAPDBERR_ASSERT_STATE (false);
        error = e_bad_fdir;
        return;
    }

    const SAPDB_Bool bSetPageCountInitialized = true;
    tableFile.SetRootPage (
        trans.trTaskId_gg00, fileInfo.fi_root);
    tableFile.SetLeafPageCount ( 1, ! bSetPageCountInitialized );
    tableFile.SetIndexPageCount ( 0 );
    tableFile.SetEntryCount ( 0 );
    tableFile.SetLobPageCount ( 0 );

    // +++ end needs to be synchronised with savepoint

    // drop files instantly if in redo:
    if ( RedoExecution == executionType )
    {
        if ( ! this->RemoveGarbage (trans, errorList) )
        {
            // ignore errors during drop
            trans.trError_gg00 = e_ok;
            errorList.ClearMessageList();
        }
    }
}

//---------------------------------------------------------------------------

void Log_ActionUnqualifiedDelete760007::Undo(tgg00_TransContext &TransContext, Log_IImage &Image) const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionUnqualifiedDelete760007::Undo", LogAction_Trace, 5);

    SAPDBTRACE_WRITELN( LogAction_Trace, 5, "  tableFileNo = " << m_Head.tableFileNo
                                                               << ", dropFileNo = " << m_Head.dropFileNo);

    tgg00_BasisError &error   = TransContext.trError_gg00;
    Msg_List         &errList = Trans_Context::GetContext(TransContext).ErrorList();

    KERNEL_TRACE_BASIS_ERROR (error, LogAction_Trace, 5);

    if (e_ok != error)
        return;

    FileDir_IDirectory &fileDir = FileDir_IDirectory::SessionInstance( TransContext );

    FileDir_Table   tableFile;
    FileDir_Generic gcDropFile;
    FileDir_Table   dropFile;

    FileDir_ReturnCode retCode = fileDir.GetTableFile (TransContext.trTaskId_gg00, m_Head.tableFileNo, tableFile, errList);

    if (FileDir_Okay != retCode)
    {
        // SAPDBERR_ASSERT_STATE (false);

        errList = Log_Exception(__CONTEXT__, LOG_ACTION_GET_FILE_FAILED,SAPDB_ToString(retCode));
        AppendContextInfo(__CONTEXT__,errList);
        Kernel_AppendBasisError (error,errList,"FileDir_IDirectory::GetFile");

        // FIXME: this error can happen until the DelFile
        // operations here are synchronised with the savepoint
        error = e_bad_fdir;
        return;
    }

    // remove deleted marks from secondary files
    FileDir_Generic curFileEntry;
    SecondaryFileList::ConstIterator itEnd = m_secondaryFiles.End();
    for (SecondaryFileList::ConstIterator it = m_secondaryFiles.Begin();
         it != itEnd;
         ++it)
    {
        fileDir.GcGetFile (TransContext.trTaskId_gg00, *it, curFileEntry, errList);

        if ( !curFileEntry.IsAssigned() )
        {
            SAPDBERR_ASSERT_STATE (false);
            error = e_bad_fdir;
            return;
        }

        curFileEntry.RemoveFileDeletedMark();
        switch (curFileEntry.GetFileType())
        { // we might reassign entries to the file they are already
          // assigned to but that should not be a problem
            case Data_ShortColumnFile:
                retCode = fileDir.ReassignShortColumnFile (
                    TransContext.trTaskId_gg00,
                    curFileEntry.GetFileNo(),
                    tableFile.GetFileNo(), errList);
                if ( retCode == FileDir_FileNoNotFound )
                    retCode = FileDir_Okay;
                break;
            case Data_IndexFile:
                retCode = fileDir.ReassignIndexFile (
                    TransContext.trTaskId_gg00,
                    curFileEntry.GetFileNo(),
                    tableFile.GetFileNo(), errList);
                if ( retCode == FileDir_IndexExists )
                {
                    FileDir_Index indexEntry;
                    indexEntry.Assign( curFileEntry );
                    if ( tableFile.GetIndexFileNo( indexEntry.GetIndexId() ) == indexEntry.GetFileNo() )
                        retCode = FileDir_Okay;
                } else if ( retCode == FileDir_IndexNotFound )
                    // index was not attached to dropFile, ignore
                    retCode = FileDir_Okay;

                break;
            default:
                SAPDBERR_ASSERT_STATE( false );
                error = e_bad_fdir;
                return;
        }

        if ( FileDir_Okay != retCode )
        {
            SAPDBERR_ASSERT_STATE (false);
            error = e_bad_fdir;
            return;
        }
    }

    retCode = fileDir.GcGetFile (TransContext.trTaskId_gg00, m_Head.dropFileNo, gcDropFile, errList);

    if ( retCode == FileDir_Okay )
	{
        dropFile.Assign (gcDropFile);

        if (Data_TableFile != dropFile.GetFileType())
        {
            errList = Log_Exception(__CONTEXT__, LOG_ACTION_UNQUALIFIED_DELETE_WRONG_FILETYPE, SAPDB_ToString (dropFile.GetFileType()));
            AppendContextInfo(__CONTEXT__,errList);
            Kernel_AppendBasisError (error, errList,"FileDir_IDirectory::GetFile");

            error = e_bad_fdir;
            return;
        }

        Data_PageNo originalTableRoot      = dropFile.GetRootPage();
        SAPDB_UInt4 originalLeafPageCount  = dropFile.GetLeafPageCount(TransContext.trTaskId_gg00);
        SAPDB_UInt4 originalIndexPageCount = dropFile.GetIndexPageCount(TransContext.trTaskId_gg00);
        SAPDB_UInt8 originalEntryCount     = dropFile.GetEntryCount(TransContext.trTaskId_gg00);
        SAPDB_UInt8 originalLobPageCount   = dropFile.GetLobPageCount();

        tbd_current_tree current;
        tbd_node_ptrs    nptrs;
        tgg00_FileId     newTableFileId;

        tableFile.BuildFileId (newTableFileId);
        // don't take any risks with the file version as we are about to
        // free the page:
        newTableFileId.fileVersion_gg00().gg91SetDummyVers();

        bd30BuildCurrent (TransContext, newTableFileId, current);
        if (e_ok != error)
        {
            SAPDBERR_ASSERT_STATE (false);
            return;
        }

        bd13GetNode ( current, tableFile.GetRootPage(), plmLock_ebd00, nr_for_update, nptrs);

        if (e_ok != error)
        {
            SAPDBERR_ASSERT_STATE (false);
            return;
        }

        // FIXME: synchronise with savepoint
        // dropFile is already marked deleted from online/redo execution
        fileDir.DelFile (TransContext.trTaskId_gg00, dropFile.GetFileNo(), errList);

        const SAPDB_Bool bSetPageCountInitialized = true;
        tableFile.SetRootPage        ( TransContext.trTaskId_gg00, originalTableRoot);
        tableFile.SetLeafPageCount   ( originalLeafPageCount, ! bSetPageCountInitialized );
        tableFile.SetIndexPageCount  ( originalIndexPageCount );
        tableFile.SetEntryCount      ( originalEntryCount );
        tableFile.SetLobPageCount    ( originalLobPageCount );

        b13free_node (nptrs, current);
    }

    // restore old file version in tree
    tgg00_FileId      tableFileId;
    const SAPDB_Int2  c_dummyNewLongCnt = -1;

    tableFile.BuildFileId (tableFileId);
    tgg91_FileVersion originalFileVersion = tableFileId.fileVersion_gg00();
    tableFileId.fileVersion_gg00().gg91SetDummyVers();

    b01set_fileversion ( TransContext, tableFileId, originalFileVersion, c_dummyNewLongCnt);

    SAPDBTRACE_WRITELN( LogAction_Trace, 5, "  tableFileVersion: " << SAPDBTrace_Hex(&originalFileVersion,
                                                                                      sizeof(originalFileVersion)) << NewLine
                                                                   << "  restored root: " << tableFile.GetRootPage() );

    // catalog cache might still hold entry referencing outdated root
    Catalog_Cache::InvalidateTable( TransContext.trTaskId_gg00, tableFileId.fileTabId_gg00() );
}

//---------------------------------------------------------------------------

bool Log_ActionUnqualifiedDelete760007::RemoveGarbage (
    tgg00_TransContext   &trans,
    SAPDBErr_MessageList &errList) const
{
    SAPDBTRACE_METHOD_DEBUG (
        "Log_ActionUnqualifiedDelete760007::RemoveGarbage", LogAction_Trace, 5);

    tgg00_BasisError &error = trans.trError_gg00;

    KERNEL_TRACE_BASIS_ERROR (error, LogAction_Trace, 5);

    if (e_ok != error)
    {
        Kernel_AppendBasisError (
            error, errList,
            "Log_ActionUnqualifiedDelete760007::RemoveGarbage entered with error");
        return false;
    }

    FileDir_IDirectory &filedir = FileDir_IDirectory::SessionInstance (trans);
    FileDir_Generic     gcDropFile;
    FileDir_Table       dropFile;
    FileDir_ReturnCode  result = filedir.GcGetFile (
                trans.trTaskId_gg00, m_Head.dropFileNo, gcDropFile, errList);

    if ( FileDir_Okay != result )
    {
        if ( FileDir_FileNoNotFound == result )
        { // PTS 1133429 M.Ki.
            return true;
        }
        if ( error == e_ok )
            error = e_bad_fdir;
        // FIXME: add correct message
        errList = Log_Exception(
            __CONTEXT__, LOG_ACTION_GET_FILE_FAILED,
            SAPDB_ToString(result));
        AppendContextInfo(__CONTEXT__,errList);
        Kernel_AppendBasisError (error,errList,"FileDir_IDirectory::GetFile");
        return false;
    }

    if (Data_TableFile != gcDropFile.GetFileType())
    {
        errList = Log_Exception(
            __CONTEXT__,
            LOG_ACTION_UNQUALIFIED_DELETE_WRONG_FILETYPE,
            SAPDB_ToString (dropFile.GetFileType()));
            AppendContextInfo(__CONTEXT__,errList);
        Kernel_AppendBasisError (error, errList,"FileDir_IDirectory::GetFile");

        error = e_bad_fdir;
        return false;
    }
    dropFile.Assign (gcDropFile);

    if (Data_FileDeleted != dropFile.GetFileState())
    {
        errList = Log_Exception(
            __CONTEXT__,
            LOG_ACTION_FILE_NOT_MARKED_DELETED,
            FileDir_FileNoToString (dropFile.GetFileNo()),
            SAPDB_ToString (dropFile.GetFileState()));
            AppendContextInfo(__CONTEXT__,errList);
        Kernel_AppendBasisError (error, errList,"FileDir_IDirectory::GetFile");

        error = e_bad_fdir;
        return false;
    }

    tgg00_FileId fileId;
    FileDir_Generic curFileEntry;
    SecondaryFileList::ConstIterator itEnd = m_secondaryFiles.End();
    for (SecondaryFileList::ConstIterator it = m_secondaryFiles.Begin();
         it != itEnd;
         ++it)
    {
        filedir.GcGetFile (trans.trTaskId_gg00, *it, curFileEntry, errList);

        if ( ! curFileEntry.IsAssigned() )
            // might have already been deleted during a previous gc call
            continue;

        curFileEntry.BuildFileId (fileId);
        b01pdestroy_perm_file (trans, fileId);

        if ( error != e_ok )
        {
            // FIXME: add correct message
            errList = Log_Exception(
                __CONTEXT__, LOG_ACTION_DESTROY_FILE_FAILED);
            AppendContextInfo(__CONTEXT__,errList);
            Kernel_AppendBasisError (
                error, errList,"FileDir_IDirectory::GetFile");
            return false;
        }
    }

    dropFile.BuildFileId (fileId);
    b01pdestroy_perm_file (trans, fileId);

    if ( error != e_ok )
    {
        // FIXME: add correct message
        errList = Log_Exception(
            __CONTEXT__, LOG_ACTION_DESTROY_FILE_FAILED);
        AppendContextInfo(__CONTEXT__,errList);
        Kernel_AppendBasisError (error, errList,"FileDir_IDirectory::GetFile");
        return false;
    }
    return true;
}

//---------------------------------------------------------------------------

void Log_ActionUnqualifiedDelete760007::WriteToTrace (const char * Title) const
{
    if ( Title != 0 )
        Kernel_VTrace() << Title;

    Kernel_VTrace() << "tableFileNo: " << m_Head.tableFileNo << NewLine
                    << "dropFileNo : " << m_Head.dropFileNo;
}

//---------------------------------------------------------------------------

SAPDB_UInt Log_ActionUnqualifiedDelete760007::GetPersistentLength () const
{
    return sizeof(m_Head) + m_Head.secondaryFileCount * sizeof(SecondaryFileList::ValueType);
}

//---------------------------------------------------------------------------

bool Log_ActionUnqualifiedDelete760007::WriteImagePersistent (
    Log_ActionImageWriter &ImageWriter,
    SAPDBErr_MessageList  &errlist) const
{
    SAPDBTRACE_METHOD_DEBUG (
        "Log_ActionUnqualifiedDelete760007::WriteImagePersistent", LogAction_Trace, 5);
    if (! ImageWriter.PersistentWrite (&(m_Head), sizeof (m_Head), errlist) )
        return false;

    SecondaryFileList::ConstIterator itEnd = m_secondaryFiles.End();
    for (SecondaryFileList::ConstIterator it = m_secondaryFiles.Begin();
         it != itEnd;
         ++it)
    {
        if ( ! ImageWriter.PersistentWrite (&(*it), sizeof (SecondaryFileList::ValueType), errlist) )
            return false;
    }

    return true;
}

//---------------------------------------------------------------------------

bool Log_ActionUnqualifiedDelete760007::ReadImagePersistent (
    Log_ActionImageReader &ImageReader,
    SAPDBErr_MessageList  &errlist)
{
    SAPDBTRACE_METHOD_DEBUG (
        "Log_ActionUnqualifiedDelete760007::ReadImagePersistent", LogAction_Trace, 5);
    if ( ! ImageReader.CopyImage (reinterpret_cast<SAPDB_Byte*>(&m_Head),sizeof(m_Head), errlist) )
        return false;

    if ( m_Head.secondaryFileCount == 0 )
        return true;

    m_secondaryFiles.Clear();
    if ( ! m_secondaryFiles.Reserve( m_Head.secondaryFileCount ) )
        return false;

    for (int i = 0; i < m_Head.secondaryFileCount; ++i)
    {
        FileDir_FileNo fileNo;
        if ( ! ImageReader.CopyImage( reinterpret_cast<SAPDB_Byte*>(&fileNo),
                                   sizeof(fileNo),
                                      errlist ) )
            return false;
        m_secondaryFiles.InsertEnd (fileNo);
    }

    return true;
}

//---------------------------------------------------------------------------

void Log_ActionUnqualifiedDelete760007::AppendContextInfo (SAPDB_Char  const * const  filename,
                                        SAPDB_UInt4 const          linenumber,
                                        SAPDBErr_MessageList      &errList,
                                        SAPDB_Char  const * const  title) const
{
    Log_IAction::AppendContextInfo (filename,linenumber,errList,title);
    errList = errList + Log_Exception( filename, linenumber, LOG_ACTION_FILE_INFO,
                                       (title!=0 ? title : "Log_ActionUnqualifiedDelete760007"),
                                       Log_GetActionTypeString(m_Type),
                                       FileDir_FileNoToString(m_Head.tableFileNo));
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

/*!
 * @class Log_ActionUnqualifiedDelete760000
 * @author MartinKi
 *
 * Suffers from the following problem: all secondary files are marked
 * deleted during execution; during a possible restart these files
 * will not be attached to the dropTable entry any more and therefore
 * not removed/restored during garbage collection/undo.
 */

void Log_ActionUnqualifiedDelete760000::Execute (
    tgg00_TransContext &trans,
    ExecutionType       executionType) const
{
    SAPDBTRACE_METHOD_DEBUG (
        "Log_ActionUnqualifiedDelete760000::Execute",
        LogAction_Trace, 5);

    SAPDBTRACE_WRITELN(
        LogAction_Trace, 5,
        "  tableFileNo = " << m_Head.tableFileNo
        << ", dropFileNo = " << m_Head.dropFileNo);

    SAPDBERR_ASSERT_STATE (
        (executionType == OnlineExecution)
        || (executionType == RedoExecution));

    tgg00_BasisError &error = trans.trError_gg00;

    KERNEL_TRACE_BASIS_ERROR (error, LogAction_Trace, 5);

    if (e_ok != error)
        return;

    Msg_List errorList;
    FileDir_IDirectory &fileDir =
        FileDir_IDirectory::SessionInstance( trans );
    FileDir_Table tableFile;
    FileDir_Table dropFile;

    SAPDBERR_ASSERT_STATE (
        (executionType == OnlineExecution)
        || (executionType == RedoExecution));

    if ( fileDir.GetTableFile (
             trans.trTaskId_gg00, m_Head.tableFileNo,
             tableFile, errorList)
         != FileDir_Okay )
    {
        SAPDBERR_ASSERT_STATE (false);
        error = e_bad_fdir;
        return;
    }

    tbd_current_tree current;

    tgg00_FileId newTableFileId;
    tableFile.BuildFileId (newTableFileId);
    newTableFileId.fileRoot_gg00() = NIL_PAGE_NO_GG00;
    bd30BuildCurrent (trans, newTableFileId, current);
    if (e_ok != error)
        return;

    /*
     * Synchronise with savepoint by locking the newly create tree
     * exclusively. This will insure that the file directory entry of
     * dropFile will always exist only in the correct converter
     * version. Counters are secured by not changing them persistently
     * if they are changed during a savepoint (and in case of a crash
     * the Execute method will be called again, so this should not be
     * harmful). MarkDeletes on secondary files can however make it
     * into the savepoint and have to be considered during undo.
     */
    tbd_fileinfo fileInfo;
    tableFile.FillFileInfo (fileInfo);

    b30new_tree (fileInfo, current);
    if (e_ok != error)
        return;

    newTableFileId.fileRoot_gg00()      = current.curr_tree_id.fileRoot_gg00();
    newTableFileId.fileRootCheck_gg00() = current.curr_tree_id.fileRootCheck_gg00();
    AutoTreeLock lockTree( trans, newTableFileId, current,
                           AutoTreeLock::LockExclusive );
    if (e_ok != error )
        return;

    SAPDBTRACE_WRITELN(
        LogAction_Trace, 5,
            "  tableFileVersion: "
            << SAPDBTrace_Hex(&newTableFileId.fileVersion_gg00(),
                              sizeof(newTableFileId.fileVersion_gg00())) << NewLine
            << "  old root: " << tableFile.GetRootPage()
            << ", new root: " << newTableFileId.fileRoot_gg00());

    Converter_Version destConverter =
        bd20GetDestinationConverterVersion(
            trans.trTaskId_gg00, current.currRootNptrs_bd00.np_cbptr() );

    FileDir_ReturnCode retCode =
        fileDir.AddTableFile (
            trans.trTaskId_gg00,
            m_Head.dropFileNo,
            destConverter,
            tableFile.GetRootPage(),
            tableFile.GetBlobColCount(),
            fileInfo.fi_type,
            fileInfo.fi_vers,
            errorList,
            &dropFile);

    if (FileDir_Okay != retCode)
    {
        if ( (FileDir_FileNoExists == retCode)
             && (RedoExecution == executionType)
            )
        { // entry might already exist during redo as add to fdir was
          // not synchronized with savepoint
            fileDir.GcGetFile(
                trans.trTaskId_gg00, m_Head.dropFileNo, dropFile, errorList);
        }
        if ( (! dropFile.IsAssigned())
             || (Data_TableFile != dropFile.GetFileType()) )
        {
            SAPDBERR_ASSERT_STATE (false);
            error = e_bad_fdir;
            return;
        }
        // PTS 1133047 M.Ki.
        // make sure dropFile has correct root page; entry in file
        // directory might contain outdated root page if table was
        // created during redo
        dropFile.SetRootPage( trans.trTaskId_gg00, tableFile.GetRootPage() );
    }

    // store original page and entry count in dropFile
    dropFile.SetLeafPageCount (
        tableFile.GetLeafPageCount (trans.trTaskId_gg00));
    dropFile.SetIndexPageCount (
        tableFile.GetIndexPageCount (trans.trTaskId_gg00));
    dropFile.SetEntryCount (
        tableFile.GetEntryCount (trans.trTaskId_gg00));

    // reassign short column and index files to dropFile
    if ( tableFile.HasShortColumnFile())
    {
        if ( FileDir_Okay != fileDir.ReassignShortColumnFile (
                 trans.trTaskId_gg00,
                 tableFile.GetShortColumnFileNo(),
                 dropFile.GetFileNo(), errorList)
            )
        {
            SAPDBERR_ASSERT_STATE (false);
            error = e_bad_fdir;
            return;
        }
        FileDir_ShortColumn shortColumnFile;
        fileDir.GetShortColumnFile (
            trans.trTaskId_gg00,
            dropFile.GetShortColumnFileNo(),
            shortColumnFile, errorList);
        if (! shortColumnFile.IsAssigned())
        {
            SAPDBERR_ASSERT_STATE (false);
            error = e_bad_fdir;
            return;
        }
        shortColumnFile.MarkFileDeleted (trans.trWriteTransId_gg00);
    }

    for (int i=1; i <= tableFile.GetMaxIndexId(); ++i)
    {
        FileDir_FileNo indexFileNo (tableFile.GetIndexFileNo (i));
        if (! indexFileNo.IsValid())
            continue;

        if ( FileDir_Okay != fileDir.ReassignIndexFile (
                 trans.trTaskId_gg00,
                 indexFileNo,
                 dropFile.GetFileNo(), errorList)
            )
        {
            SAPDBERR_ASSERT_STATE (false);
            error = e_bad_fdir;
            return;
        }
        FileDir_Index indexFile;
        fileDir.GetIndexFile (
            trans.trTaskId_gg00,
            dropFile.GetIndexFileNo (i),
            indexFile, errorList);
        if (!indexFile.IsAssigned())
        {
            SAPDBERR_ASSERT_STATE (false);
            error = e_bad_fdir;
            return;
        }
        indexFile.MarkFileDeleted (trans.trWriteTransId_gg00);
    }

    // set dummy file version in file to make prepared statements fail
    tgg91_FileVersion fileVersion;
    tgg00_FileId      dropTableFileId;
    const SAPDB_Int2  c_dummyNewLongCnt = -1;

    dropFile.BuildFileId (dropTableFileId);
    fileVersion.gg91SetDummyVers();
    b01set_fileversion (
        trans, dropTableFileId, fileVersion, c_dummyNewLongCnt);

    if (e_ok != error)
        return;

    retCode = dropFile.MarkFileDeleted (trans.trWriteTransId_gg00);

    if ((retCode != FileDir_Okay)
        && ((retCode != FileDir_AlreadyMarkedForDeletion)
            || (RedoExecution != executionType))
        )
    {
        SAPDBERR_ASSERT_STATE (false);
        error = e_bad_fdir;
        return;
    }

    const SAPDB_Bool bSetPageCountInitialized = true;
    tableFile.SetRootPage (
        trans.trTaskId_gg00, fileInfo.fi_root);
    tableFile.SetLeafPageCount ( 1, ! bSetPageCountInitialized );
    tableFile.SetIndexPageCount ( 0 );
    tableFile.SetEntryCount ( 0 );

    // +++ end needs to be synchronised with savepoint

    // drop files instantly if in redo:
    if ( RedoExecution == executionType )
    {
        if ( ! this->RemoveGarbage (trans, errorList) )
        {
            // ignore errors during drop
            trans.trError_gg00 = e_ok;
            errorList.ClearMessageList();
        }
    }
}

//---------------------------------------------------------------------------

void Log_ActionUnqualifiedDelete760000::Undo(tgg00_TransContext &TransContext, Log_IImage &Image) const
{
    SAPDBTRACE_METHOD_DEBUG (
        "Log_ActionUnqualifiedDelete760000::Undo", LogAction_Trace, 5);

    SAPDBTRACE_WRITELN(LogAction_Trace, 5,"  tableFileNo = " << m_Head.tableFileNo << ", dropFileNo = " << m_Head.dropFileNo);

    tgg00_BasisError &error   = TransContext.trError_gg00;
    Msg_List         &errList = Trans_Context::GetContext(TransContext).ErrorList();

    KERNEL_TRACE_BASIS_ERROR (error, LogAction_Trace, 5);

    if (e_ok != error)
        return;

    FileDir_IDirectory &fileDir = FileDir_IDirectory::SessionInstance( TransContext );

    FileDir_Table   tableFile;
    FileDir_Generic gcDropFile;
    FileDir_Table   dropFile;

    FileDir_ReturnCode retCode = fileDir.GetTableFile (TransContext.trTaskId_gg00, m_Head.tableFileNo, tableFile, errList);

    if (FileDir_Okay != retCode)
    {
        // SAPDBERR_ASSERT_STATE (false);

        errList = Log_Exception(__CONTEXT__, LOG_ACTION_GET_FILE_FAILED,SAPDB_ToString(retCode));
        AppendContextInfo(__CONTEXT__,errList);
        Kernel_AppendBasisError (error,errList,"FileDir_IDirectory::GetFile");

        // FIXME: this error can happen until the DelFile
        // operations here are synchronised with the savepoint
        error = e_bad_fdir;
        return;
    }

    retCode = fileDir.GcGetFile (TransContext.trTaskId_gg00, m_Head.dropFileNo, gcDropFile, errList);
    if (FileDir_Okay != retCode)
    {
        // drop file creation did not make it into the current
        // converter version, but dependent files could still be
        // marked deleted
        // FIXME: MarkAsDeleted should use converter
        // version, then this could go away:

        // remove deleted mark from  short column and index files
        if ( tableFile.HasShortColumnFile())
        {
            FileDir_Generic shortColumnFile;
            fileDir.GcGetFile (
                TransContext.trTaskId_gg00,
                tableFile.GetShortColumnFileNo(),
                shortColumnFile, errList);
            if (shortColumnFile.IsAssigned())
                shortColumnFile.RemoveFileDeletedMark();
        }

        for (int i=1; i <= tableFile.GetMaxIndexId(); ++i)
        {
            FileDir_FileNo indexFileNo (tableFile.GetIndexFileNo (i));
            if (! indexFileNo.IsValid())
                continue;

            FileDir_Generic indexFile;
            fileDir.GcGetFile (
                TransContext.trTaskId_gg00,
                tableFile.GetIndexFileNo (i),
                indexFile, errList);
            if (indexFile.IsAssigned())
                indexFile.RemoveFileDeletedMark();
        }
        return;
    }
    dropFile.Assign (gcDropFile);

    if (Data_TableFile != dropFile.GetFileType())
    {
        errList = Log_Exception(__CONTEXT__,LOG_ACTION_UNQUALIFIED_DELETE_WRONG_FILETYPE, SAPDB_ToString (dropFile.GetFileType()));
            AppendContextInfo(__CONTEXT__,errList);
        Kernel_AppendBasisError (error, errList,"FileDir_IDirectory::GetFile");

        error = e_bad_fdir;
        return;
    }

    Data_PageNo originalTableRoot      = dropFile.GetRootPage();
    SAPDB_UInt4 originalLeafPageCount  = dropFile.GetLeafPageCount(TransContext.trTaskId_gg00);
    SAPDB_UInt4 originalIndexPageCount = dropFile.GetIndexPageCount(TransContext.trTaskId_gg00);
    SAPDB_UInt8 originalEntryCount     = dropFile.GetEntryCount(TransContext.trTaskId_gg00);

    // reassign short column and index files to tableFile
    if ( dropFile.HasShortColumnFile())
    {
        FileDir_Generic shortColumnFile;
        fileDir.GcGetFile (
            TransContext.trTaskId_gg00,
            dropFile.GetShortColumnFileNo(),
            shortColumnFile, errList);
        if (shortColumnFile.IsAssigned())
            shortColumnFile.RemoveFileDeletedMark();

        if ( FileDir_Okay != fileDir.ReassignShortColumnFile (
                 TransContext.trTaskId_gg00,
                 dropFile.GetShortColumnFileNo(),
                 tableFile.GetFileNo(), errList)
            )
        {
            SAPDBERR_ASSERT_STATE (false);
            error = e_bad_fdir;
            return;
        }
    }

    for (int i=1; i <= dropFile.GetMaxIndexId(); ++i)
    {
        FileDir_FileNo indexFileNo (dropFile.GetIndexFileNo (i));
        if (! indexFileNo.IsValid())
            continue;

        FileDir_Generic indexFile;
        fileDir.GcGetFile (
            TransContext.trTaskId_gg00,
            dropFile.GetIndexFileNo (i),
            indexFile, errList);
        if (indexFile.IsAssigned())
            indexFile.RemoveFileDeletedMark();

        if ( FileDir_Okay != fileDir.ReassignIndexFile (
                 TransContext.trTaskId_gg00,
                 indexFileNo,
                 tableFile.GetFileNo(), errList)
            )
        {
            SAPDBERR_ASSERT_STATE (false);
            error = e_bad_fdir;
            return;
        }
    }

    if ( Kernel_IAdminInfo::Instance().KernelStateIsRedoLogProcessing() ) {
        // needs to be done because secondary files that are marked
        // deleted will not be attached to table during file directory
        // restart
        this->UndoReassignDuringRestart( TransContext, errList );
    }

    tbd_current_tree current;
    tbd_node_ptrs    nptrs;
    tgg00_FileId     newTableFileId;

    tableFile.BuildFileId (newTableFileId);
    // don't take any risks with the file version as we are about to
    // free the page:
    newTableFileId.fileVersion_gg00().gg91SetDummyVers();

    bd30BuildCurrent (TransContext, newTableFileId, current);
    if (e_ok != error)
    {
        SAPDBERR_ASSERT_STATE (false);
        return;
    }

    bd13GetNode (current,tableFile.GetRootPage(),plmLock_ebd00,nr_for_update,nptrs);

    if (e_ok != error)
    {
        SAPDBERR_ASSERT_STATE (false);
        return;
    }

    // FIXME: synchronise with savepoint
    // dropFile is already marked deleted from online/redo execution
    fileDir.DelFile (TransContext.trTaskId_gg00, dropFile.GetFileNo(), errList);

    const SAPDB_Bool bSetPageCountInitialized = true;
    tableFile.SetRootPage (TransContext.trTaskId_gg00, originalTableRoot);
    tableFile.SetLeafPageCount ( originalLeafPageCount, ! bSetPageCountInitialized );
    tableFile.SetIndexPageCount ( originalIndexPageCount );
    tableFile.SetEntryCount ( originalEntryCount );

    b13free_node (nptrs, current);

    // restore old file version in tree
    tgg00_FileId      tableFileId;
    const SAPDB_Int2  c_dummyNewLongCnt = -1;

    tableFile.BuildFileId (tableFileId);
    tgg91_FileVersion originalFileVersion = tableFileId.fileVersion_gg00();
    tableFileId.fileVersion_gg00().gg91SetDummyVers();

    b01set_fileversion (TransContext, tableFileId, originalFileVersion, c_dummyNewLongCnt);

    SAPDBTRACE_WRITELN(LogAction_Trace, 5, "  tableFileVersion: " << SAPDBTrace_Hex(&originalFileVersion,
                                                                                    sizeof(originalFileVersion)) << NewLine
                                                                  << "  restored root: " << tableFile.GetRootPage() );

    // catalog cache might still hold entry referencing outdated root
    Catalog_Cache::InvalidateTable(TransContext.trTaskId_gg00, tableFileId.fileTabId_gg00() );
}

//---------------------------------------------------------------------------

bool Log_ActionUnqualifiedDelete760000::RemoveGarbage (
    tgg00_TransContext   &trans,
    SAPDBErr_MessageList &errList) const
{
    SAPDBTRACE_METHOD_DEBUG (
        "Log_ActionUnqualifiedDelete760000::RemoveGarbage", LogAction_Trace, 5);

    tgg00_BasisError &error = trans.trError_gg00;

    KERNEL_TRACE_BASIS_ERROR (error, LogAction_Trace, 5);

    if (e_ok != error)
    {
        Kernel_AppendBasisError (
            error, errList,
            "Log_ActionUnqualifiedDelete760000::RemoveGarbage entered with error");
        return false;
    }


    FileDir_IDirectory &filedir = FileDir_IDirectory::SessionInstance (trans);
    FileDir_Generic     gcDropFile;
    FileDir_Table       dropFile;
    FileDir_ReturnCode  result = filedir.GcGetFile (
                trans.trTaskId_gg00, m_Head.dropFileNo, gcDropFile, errList);

    if ( FileDir_Okay != result )
    {
        if ( FileDir_FileNoNotFound == result )
        { // PTS 1133429 M.Ki.
            return true;
        }
        if ( error == e_ok )
            error = e_bad_fdir;
        // FIXME: add correct message
        errList = Log_Exception(
            __CONTEXT__, LOG_ACTION_GET_FILE_FAILED,
            SAPDB_ToString(result));
        AppendContextInfo(__CONTEXT__,errList);
        Kernel_AppendBasisError (error,errList,"FileDir_IDirectory::GetFile");
        return false;
    }

    if (Data_TableFile != gcDropFile.GetFileType())
    {
        errList = Log_Exception(
            __CONTEXT__,
            LOG_ACTION_UNQUALIFIED_DELETE_WRONG_FILETYPE,
            SAPDB_ToString (dropFile.GetFileType()));
            AppendContextInfo(__CONTEXT__,errList);
        Kernel_AppendBasisError (error, errList,"FileDir_IDirectory::GetFile");

        error = e_bad_fdir;
        return false;
    }
    dropFile.Assign (gcDropFile);

    if (Data_FileDeleted != dropFile.GetFileState())
    {
        errList = Log_Exception(
            __CONTEXT__,
            LOG_ACTION_FILE_NOT_MARKED_DELETED,
            FileDir_FileNoToString (dropFile.GetFileNo()),
            SAPDB_ToString (dropFile.GetFileState()));
            AppendContextInfo(__CONTEXT__,errList);
        Kernel_AppendBasisError (error, errList,"FileDir_IDirectory::GetFile");

        error = e_bad_fdir;
        return false;
    }

    tgg00_FileId fileId;
    if ( dropFile.HasShortColumnFile() )
    {
        FileDir_Generic shortColumnFile;
        if ( FileDir_Okay != filedir.GcGetFile (
                 trans.trTaskId_gg00,
                 dropFile.GetShortColumnFileNo(),
                 shortColumnFile, errList) )
        {
            SAPDBERR_ASSERT_STATE (false);
            error = e_bad_fdir;
            return false;
        }
        shortColumnFile.BuildFileId (fileId);
        b01pdestroy_perm_file (trans, fileId);
        if ( error != e_ok )
        {
            // FIXME: add correct message
            errList = Log_Exception(
                __CONTEXT__, LOG_ACTION_DESTROY_FILE_FAILED);
            AppendContextInfo(__CONTEXT__,errList);
            Kernel_AppendBasisError (
                error, errList,"FileDir_IDirectory::GetFile");
            return false;
        }
    }

    for (int i=1; i <= dropFile.GetMaxIndexId(); ++i)
    {
        FileDir_FileNo indexFileNo (dropFile.GetIndexFileNo (i));
        if (! indexFileNo.IsValid())
            continue;

        FileDir_Generic indexFile;
        if ( FileDir_Okay != filedir.GcGetFile (
                 trans.trTaskId_gg00,
                 dropFile.GetIndexFileNo (i),
                 indexFile, errList) )
        {
            SAPDBERR_ASSERT_STATE (false);
            error = e_bad_fdir;
            return false;
        }
        indexFile.BuildFileId (fileId);
        b01pdestroy_perm_file (trans, fileId);
        if ( error != e_ok )
        {
            // FIXME: add correct message
            errList = Log_Exception(
                __CONTEXT__, LOG_ACTION_DESTROY_FILE_FAILED);
            AppendContextInfo(__CONTEXT__,errList);
            Kernel_AppendBasisError (
                error, errList,"FileDir_IDirectory::GetFile");
            return false;
        }
    }

    dropFile.BuildFileId (fileId);
    b01pdestroy_perm_file (trans, fileId);

    if ( error != e_ok )
    {
        // FIXME: add correct message
        errList = Log_Exception(
            __CONTEXT__, LOG_ACTION_DESTROY_FILE_FAILED);
        AppendContextInfo(__CONTEXT__,errList);
        Kernel_AppendBasisError (error, errList,"FileDir_IDirectory::GetFile");
        return false;
    }
    return true;
}

//---------------------------------------------------------------------------

void Log_ActionUnqualifiedDelete760000::WriteToTrace (const char * Title) const
{
    if ( Title != 0 )
        Kernel_VTrace() << Title;

    Kernel_VTrace() << "tableFileNo: " << m_Head.tableFileNo << NewLine
                    << "dropFileNo : " << m_Head.dropFileNo;
}

//---------------------------------------------------------------------------

void Log_ActionUnqualifiedDelete760000::AppendContextInfo (SAPDB_Char  const * const  filename,
                                        SAPDB_UInt4 const          linenumber,
                                        SAPDBErr_MessageList      &errList,
                                        SAPDB_Char  const * const  title) const
{
    Log_IAction::AppendContextInfo (filename,linenumber,errList,title);
    errList = errList + Log_Exception( filename, linenumber, LOG_ACTION_FILE_INFO,
                                       (title!=0 ? title : "Log_ActionUnqualifiedDelete760000"),
                                       Log_GetActionTypeString(m_Type),
                                       FileDir_FileNoToString(m_Head.tableFileNo));
}


//---------------------------------------------------------------------------

void Log_ActionUnqualifiedDelete760000::UndoReassignDuringRestart (
    tgg00_TransContext   &trans,
    SAPDBErr_MessageList &errList) const
{
    SAPDBTRACE_METHOD_DEBUG (
        "Log_ActionUnqualifiedDelete760000::UndoReassignDuringRestart", LogAction_Trace, 5);

    tgg00_BasisError &error = trans.trError_gg00;
    FileDir_IDirectory& fDir = FileDir_ISharedDirectory::IFileDirInstance();

    FileDir_Iterator it(
        *reinterpret_cast<SAPDBMem_IRawAllocator*>(trans.trAllocator_gg00) );

    it.AddFileTypeToFilter( Data_ShortColumnFile );
    it.AddFileTypeToFilter( Data_IndexFile );
    fDir.GetSharedFileDirIterator( it, &trans );

    FileDir_Generic fileEntry;
    while ( it.GetNext( fileEntry ) ) {

        if ( fileEntry.GetFileState() != Data_FileDeleted ) {
            continue;
        }

        switch ( fileEntry.GetFileType() ) {
            case Data_ShortColumnFile:
            {
                FileDir_ShortColumn shortColEntry;
                shortColEntry.Assign( fileEntry );
                if ( shortColEntry.GetParentFileNo() != m_Head.dropFileNo )
                    continue;

                fDir.ReassignShortColumnFile (
                    trans.trTaskId_gg00,
                    fileEntry.GetFileNo(),
                    m_Head.tableFileNo, errList);
                fileEntry.RemoveFileDeletedMark();
                break;
            }
            case Data_IndexFile:
            {
                FileDir_Index indexEntry;
                indexEntry.Assign( fileEntry );
                if ( indexEntry.GetParentFileNo() != m_Head.dropFileNo )
                    continue;

                fDir.ReassignIndexFile (
                    trans.trTaskId_gg00,
                    fileEntry.GetFileNo(),
                    m_Head.tableFileNo, errList);
                fileEntry.RemoveFileDeletedMark();
                break;
            }
        }
    }
}

//---------------------------------------------------------------------------

bool Log_ActionUnqualifiedDelete760000::WriteImagePersistent (
    Log_ActionImageWriter &ImageWriter,
    SAPDBErr_MessageList  &errlist) const
{
    SAPDBERR_ASSERT_STATE( false );
    return false;
}

//---------------------------------------------------------------------------
