/*!**************************************************************************

    module      : Log_ActionRenameFile75.cpp
    -------------------------------------------------------------------------

    author      : JuergenA
    responsible : UweH

    special area: Logging
    description : defines a class to handle log entries for files

    last changed: 2001-03-28

    -------------------------------------------------------------------------

    copyright:    Copyright (c) 2000-2005 SAP AG


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




*****************************************************************************/

/*===========================================================================*
*  INCLUDES                                                                  *
*============================================================================*/

#include "Logging/Log_ActionRenameFile75.hpp"

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "KernelCommon/Kernel_Trace.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "KernelCommon/Kernel_IAdminHotStandby.hpp"
#include "Logging/Log_ActionReassignSecondaryFile.hpp"
#include "Logging/Log_InvDescMap.hpp"
#include "Logging/Log_ReadWriteActionImage.hpp"
#include "Logging/Log_ActionFileHandling.hpp"
#include "Logging/Log_Exceptions.hpp"

#include "FileDirectory/FileDir_ISharedDirectory.hpp"
#include "FileDirectory/FileDir_IDirectory.hpp"
#include "FileDirectory/FileDir_Iterator.hpp"
#include "FileDirectory/FileDir_Table.hpp"
#include "FileDirectory/FileDir_ShortColumn.hpp"
#include "FileDirectory/FileDir_Index.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "DataAccess/Data_Types.hpp"

#include "Converter/Converter_ICommon.hpp"

#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "Messages/Msg_List.hpp"
#include "ggg00.h"


#include "ggg91.h"
#include "ggg00.h"
#include "hbd01.h"
#include "hbd03.h"
#include "hkb64.h"

#include "hbd17.h"
#include "hbd998.h"

//---------------------------------------------------------------------------

class SAPDB_IRawAllocator;

//---------------------------------------------------------------------------


void Log_ActionRenameFile75::ExecuteAction (
    tgg00_TransContext &TransContext,
    tgg00_FileId       *pOldCommandFileId,
    tgg00_FileId       *pNewCommandFileId) const
{
    SAPDBTRACE_METHOD_DEBUG (
        "Log_ActionRenameFile75::ExecuteAction", LogAction_Trace, 5);

    KERNEL_TRACE_BASIS_ERROR (TransContext.trError_gg00, LogAction_Trace, 5);

    if (e_ok != TransContext.trError_gg00) return;

    tgg00_FileId  OldRedoFileId;
    tgg00_FileId  NewRedoFileId;
    tgg00_FileId *pOldExecuteFileId;
    tgg00_FileId *pNewExecuteFileId;

    pOldExecuteFileId = Log_GetFileId (m_Entry.OldFilename, OldRedoFileId, pOldCommandFileId);
    pNewExecuteFileId = Log_GetFileId (m_Entry.NewFilename, NewRedoFileId, pNewCommandFileId);


    if ( (tfnMulti_egg00 != pOldExecuteFileId->fileTfn_gg00())
         && (tfnMulti_egg00 != pNewExecuteFileId->fileTfn_gg00())
         && (tfnShortScol_egg00 != pOldExecuteFileId->fileTfn_gg00())
         && (tfnShortScol_egg00 != pNewExecuteFileId->fileTfn_gg00())
         && (tfnTable_egg00 != pOldExecuteFileId->fileTfn_gg00())
         && (tfnTable_egg00 != pNewExecuteFileId->fileTfn_gg00())
         && (tfnSys_egg00 != pOldExecuteFileId->fileTfn_gg00())
         && (tfnSys_egg00 != pNewExecuteFileId->fileTfn_gg00())
        ) {
        // regular rename, should not conflict with new file directory

        tgg00_Surrogate dummySurrogate;
        SAPDB_MemMoveNoCheck (dummySurrogate, cgg_zero_id, sizeof(dummySurrogate));
        bd01RenameFile (TransContext, dummySurrogate, *pOldExecuteFileId, *pNewExecuteFileId);
    } else {
        // compatibility handling
        if ( tfnAux_egg00 == pNewExecuteFileId->fileTfn_gg00() ) {
            // drop
            this->RenameForDelete(
                TransContext, *pOldExecuteFileId, *pNewExecuteFileId );
        } else if ( tfnAux_egg00 == pOldExecuteFileId->fileTfn_gg00() ) {
            // undo drop
            this->RenameForUndoDelete(
                TransContext, *pOldExecuteFileId, *pNewExecuteFileId );
        } else {
            // reassign of index or short column file
            this->Reassign(
                TransContext, *pOldExecuteFileId, *pNewExecuteFileId );
        }
    }

    if ((e_file_not_found == TransContext.trError_gg00)
        && (NULL == pNewCommandFileId))
    {
        TransContext.trError_gg00 = e_ok;  // ignore undo/redo error
    }
}

//---------------------------------------------------------------------------

void Log_ActionRenameFile75::AppendContextInfo (SAPDB_Char  const * const  filename,
                                              SAPDB_UInt4 const          linenumber,
                                              SAPDBErr_MessageList      &errlist,
                                              SAPDB_Char  const * const  title) const
{
    tgg00_Filename oldfn = m_Entry.OldFilename;
    tgg00_Filename newfn = m_Entry.OldFilename;
    errlist = errlist +
        Log_Exception(filename,linenumber,LOG_ACTION_RENAME_FILE_INFO,
                      title==0?"Log_ActionRenameFile75":title,
                      SAPDB_ToString(oldfn.asCharp(),sizeof(tgg00_Filename),SAPDB_ToStringClass::hex),
                      SAPDB_ToString(newfn.asCharp(),sizeof(tgg00_Filename),SAPDB_ToStringClass::hex),
                      SAPDB_ToString(m_Entry.NewFileVersion.gg90GetInt4()) );
}

//---------------------------------------------------------------------------

void Log_ActionRenameFile75::WriteToTrace (const char * Title) const
{
    Kernel_VTrace trace;

    if ( Title != 0 )
        trace << "Log_ActionRenameFile75::" << Title << FlushLine;
    tgg00_Filename oldfn = m_Entry.OldFilename;
    tgg00_Filename newfn = m_Entry.OldFilename;
	trace << "oldFilename: " << SAPDB_ToString(oldfn.asCharp(),sizeof(tgg00_Filename),SAPDB_ToStringClass::hex) << FlushLine;
	trace << "newFilename: " << SAPDB_ToString(newfn.asCharp(),sizeof(tgg00_Filename),SAPDB_ToStringClass::hex) << ", " << m_Entry.NewFileVersion.gg90GetInt4() << FlushLine;
}

//---------------------------------------------------------------------------

bool Log_ActionRenameFile75::ReadImagePersistent (Log_ActionImageReader &ImageReader,
                                                SAPDBErr_MessageList  &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionRenameFile75::ReadImagePersistent", LogAction_Trace, 5);
    return ImageReader.CopyImage (REINTERPRET_CAST (SAPDB_Byte*, &(m_Entry)),sizeof(m_Entry),errlist);
}

//---------------------------------------------------------------------------

void Log_ActionRenameFile75::RenameForDelete(
    tgg00_TransContext &TransContext,
    tgg00_FileId& oldFileId,
    tgg00_FileId& newFileId) const
{
    SAPDBTRACE_METHOD_DEBUG (
        "Log_ActionRename75::RenameForUndoDelete", LogAction_Trace, 5);

    Msg_List       errMsg;
    FileDir_FileNo tableFileNo( oldFileId.fileTabId_gg00() );
    FileDir_Table  tableEntry;
    tgg00_Surrogate dummySurrogate;
    SAPDB_MemMoveNoCheck (dummySurrogate, cgg_zero_id, sizeof(dummySurrogate));

    FileDir_IDirectory& fd = FileDir_IDirectory::Instance(cgg_nil_transindex);
    fd.GetTableFile (
        TransContext.trTaskId_gg00, tableFileNo, tableEntry, errMsg);

    if ( tfnShortScol_egg00 == oldFileId.fileTfn_gg00() ) {
        if ( tableEntry.IsAssigned() ) {
            // should only happen as part of unqualified delete
            // because in 7.5 a new table file has already been
            // created, but it should not have any short column file
            // associated with it
            SAPDBERR_ASSERT_STATE( ! tableEntry.HasShortColumnFile() );
            if ( tableEntry.HasShortColumnFile() ) {
                TransContext.trError_gg00 = e_system_error;
                return;
            }
        }
    } else if ( tfnMulti_egg00 == oldFileId.fileTfn_gg00() ) {
        // during 'drop table' table and index have already been
        // deleted at this point
        if ( tableEntry.IsAssigned() ) {
            // 'drop index' or unqualified delete
            SAPDB_UInt2 indexId = oldFileId.fileTfnNo_gg00()[0];

            FileDir_FileNo indexFileNo =
                tableEntry.GetIndexFileNo( indexId );

            FileDir_Index indexEntry;
            if ( indexFileNo.IsValid() ) {
                fd.GetIndexFile (
                    TransContext.trTaskId_gg00,
                    indexFileNo, indexEntry, errMsg);
            }

            if ( ! indexEntry.IsAssigned() ) {
                // two possibilities:
                //
                // 1. index has already been dropped when dropping table
                // during unqualified delete
                // 2. might happen during redo of redo
                return;
            }

            tgg00_FileId indexFileId;
            indexEntry.BuildFileId( indexFileId );

            bd01RenameFile( TransContext, dummySurrogate, indexFileId, newFileId );
        }
    } else if ( (tfnTable_egg00  == oldFileId.fileTfn_gg00())
                || (tfnSys_egg00 == oldFileId.fileTfn_gg00())
        ) {
        // drop table; rename all dependent files
        if ( tableEntry.HasShortColumnFile() ) {
            FileDir_ShortColumn shortColEntry;
            fd.GetShortColumnFile(
                TransContext.trTaskId_gg00,
                tableEntry.GetShortColumnFileNo(),
                shortColEntry,
                errMsg);

            SAPDBERR_ASSERT_STATE( shortColEntry.IsAssigned() );
            if ( ! shortColEntry.IsAssigned() ) {
                TransContext.trError_gg00 = e_file_not_found;
                return;
            }

            // rename short column file so it can be deleted by a
            // server tasks later on
            tgg00_FileId scolFileId;
            tgg00_FileId droppedScolFileId;
            shortColEntry.BuildFileId( scolFileId );
            shortColEntry.BuildFileId( droppedScolFileId );

            SAPDB_Int dummyLen;
            k64build_aux_fn_prefix (
                TransContext.trWriteTransId_gg00, true, droppedScolFileId,
                dummyLen);
            droppedScolFileId.fileOldTfn_gg00().becomes(
                tfnShortScol_egg00 );
            droppedScolFileId.fileTfn_gg00().becomes( tfnAux_egg00 );

            bd01RenameFile( TransContext, dummySurrogate, scolFileId, droppedScolFileId );
        }

        // delete indices if there are any
        int i;
        for (i = 1; i <= tableEntry.GetMaxIndexId(); ++i ) {
            FileDir_FileNo indexFileNo( tableEntry.GetIndexFileNo( i ) );

            if ( indexFileNo.IsInvalid() ) {
                continue;
            }

            FileDir_Index  indexEntry;
            fd.GetIndexFile(
                TransContext.trTaskId_gg00,
                indexFileNo,
                indexEntry,
                errMsg);

            SAPDBERR_ASSERT_STATE( indexEntry.IsAssigned() );
            if ( ! indexEntry.IsAssigned() ) {
                TransContext.trError_gg00 = e_file_not_found;
                return;
            }

            // rename index file so it can be deleted by a server
            // tasks later on
            tgg00_FileId indexFileId;
            tgg00_FileId droppedIndexFileId;
            indexEntry.BuildFileId( indexFileId );
            indexEntry.BuildFileId( droppedIndexFileId );

            SAPDB_Int dummyLen;
            k64build_aux_fn_prefix (
                TransContext.trWriteTransId_gg00, true,
                droppedIndexFileId,
                dummyLen);
            droppedIndexFileId.fileOldTfn_gg00().becomes(
                tfnMulti_egg00 );
            droppedIndexFileId.fileTfnNo_gg00()[0] =
                static_cast<SAPDB_Byte>( indexEntry.GetIndexId() );
            droppedIndexFileId.fileTfn_gg00().becomes( tfnAux_egg00 );

            bd01RenameFile( TransContext, dummySurrogate, indexFileId, droppedIndexFileId );
            SAPDBERR_ASSERT_STATE( TransContext.trError_gg00 == e_ok );
        }

        /* rename table */
        bd01RenameFile( TransContext, dummySurrogate, oldFileId, newFileId );
    }
}

//---------------------------------------------------------------------------

void Log_ActionRenameFile75::RenameForUndoDelete(
    tgg00_TransContext &TransContext,
    tgg00_FileId& oldFileId,
    tgg00_FileId& newFileId) const
{
    SAPDBTRACE_METHOD_DEBUG (
        "Log_ActionRename75::RenameForDelete", LogAction_Trace, 5);


    const pasbool  c_shared = true;
    tgg91_FileVersion newFileVersion = m_Entry.NewFileVersion;

    FileDir_FileNo tableFileNo( newFileId.fileTabId_gg00() );

    FileDir_ISharedDirectory& sharedFd =
        FileDir_ISharedDirectory::Instance();

    Msg_List errMsg;

    tbd_fileinfo fileInfo;
    // during redo of undo this can fail...
    b17state_fdir(
        oldFileId.fileName_gg00(),
        fileInfo,
        c_shared,
        TransContext );

    if ( TransContext.trError_gg00 == e_file_not_found ) {
        TransContext.trError_gg00 = e_ok;
        return;
    }

    switch ( newFileId.fileTfn_gg00() ) {

        case tfnShortScol_egg00:
        {
            // we know that there can't be a valid table for this
            // short column file (because during drop table and
            // unqualified delete the table gets deleted first), so
            // add a loose entry
            FileDir_FileNo shortColFileNo = sharedFd.MigrationGetNewFileNo();

            FileDir_ReturnCode retCode = sharedFd.AddLooseShortColumnFile(
                TransContext.trTaskId_gg00,
                shortColFileNo,
                tableFileNo,
                fileInfo.fi_root,
                fileInfo.fi_type,
                newFileVersion,
                errMsg );

            if ( retCode != FileDir_Okay ) {
                SAPDBERR_ASSERT_STATE( false );
                TransContext.trError_gg00 = e_bad_fdir;
                return;
            }
            break;
        }
        case tfnMulti_egg00:
        {
            FileDir_FileNo indexFileNo = sharedFd.MigrationGetNewFileNo();
            SAPDB_UInt2 indexId        = newFileId.fileTfnNo_gg00()[0];

            // we cannot distinguish whether this is a rename for a
            // 'Drop Index' or a rename during an unqualified delete
            // or 'Drop Table', therefore we just have to try to
            // attach to the table and if we can't find the table, add
            // a loose entry
            FileDir_Table tableEntry;

            FileDir_IDirectory& fd =
                FileDir_IDirectory::Instance( cgg_nil_transindex );

            fd.GetTableFile (
                TransContext.trTaskId_gg00, tableFileNo, tableEntry, errMsg );

            FileDir_ReturnCode retCode;
            if ( tableEntry.IsAssigned() ) {

                retCode = fd.AddIndexFile(
                    TransContext.trTaskId_gg00,
                    indexFileNo,
                    tableFileNo,
                    Converter_ICommon::Instance().Version(),
                    indexId,
                    fileInfo.fi_root,
                    fileInfo.fi_type,
                    newFileVersion,
                    errMsg );
            } else {
                retCode = sharedFd.AddLooseIndexFile(
                    TransContext.trTaskId_gg00,
                    indexFileNo,
                    tableFileNo,
                    indexId,
                    fileInfo.fi_root,
                    fileInfo.fi_type,
                    newFileVersion,
                    errMsg );
            }
            if ( retCode != FileDir_Okay ) {
                SAPDBERR_ASSERT_STATE( false );
                TransContext.trError_gg00 = e_bad_fdir;
                return;
            }
            break;
        }
        case tfnTable_egg00:
        case tfnSys_egg00:
        {
            bd998ConvTableFDirEntry(
                TransContext.trTaskId_gg00,
                newFileId,
                fileInfo.fi_root,
                fileInfo.fi_col_cnt,
                fileInfo.fi_type,
                newFileVersion );

            // check if loose entries exists in file directory
            FileDir_Table  tableEntry;
            FileDir_IDirectory& fd =
                FileDir_IDirectory::Instance( cgg_nil_transindex );

            fd.GetTableFile (
                TransContext.trTaskId_gg00, tableFileNo, tableEntry, errMsg );
            SAPDBERR_ASSERT_STATE( tableEntry.IsAssigned() ); // FIXME: also in fast knl
            FileDir_Iterator iter(
                *reinterpret_cast<SAPDBMem_IRawAllocator*>(
                    TransContext.trAllocator_gg00 ) );

            iter.AddFileTypeToFilter( Data_IndexFile );

            if ( fileInfo.fi_col_cnt > 0 ) {
                iter.AddFileTypeToFilter( Data_ShortColumnFile );
            }

            fd.GetSharedFileDirIterator( iter, &TransContext );
            FileDir_Generic     fileEntry;
            FileDir_Index       indexEntry;
            FileDir_ShortColumn shortColEntry;
            while ( iter.GetNext( fileEntry ) ) {
                switch ( fileEntry.GetFileType() ) {
                    case Data_IndexFile:
                    {
                        indexEntry.Assign( fileEntry );
                        if ( indexEntry.GetParentFileNo() == tableFileNo ) {
                            FileDir_ReturnCode indRetCode =
                                tableEntry.AddIndex(
                                    indexEntry.GetFileNo(),
                                    indexEntry.GetIndexId() );
                            if ( indRetCode != FileDir_Okay ) {
                                SAPDBERR_ASSERT_STATE( false );
                                TransContext.trError_gg00 = e_bad_fdir;
                                return;
                            }
                        }
                        break;
                    }
                    case Data_ShortColumnFile:
                    {
                        shortColEntry.Assign( fileEntry );
                        if ( shortColEntry.GetParentFileNo() == tableFileNo ) {
                            tableEntry.SetShortColumnFileNo(
                                shortColEntry.GetFileNo() );
                        }
                    break;
                    }
                }
            }
            break;
        }
        default:
            SAPDBERR_ASSERT_STATE( false );
    }

    if ( oldFileId.fileVersion_gg00() != newFileVersion )
    {
        const SAPDB_Int2 DummyLongCnt = -1;

        b01set_fileversion (
            TransContext, oldFileId,
            newFileVersion, DummyLongCnt);
    }

    if ( TransContext.trError_gg00 == e_ok ) {
        b17del_fdir(
            oldFileId.fileName_gg00(),
            c_shared,
            TransContext );
    }
}

//---------------------------------------------------------------------------

void Log_ActionRenameFile75::Reassign(
    tgg00_TransContext &TransContext,
    tgg00_FileId& oldFileId,
    tgg00_FileId& newFileId) const
{
    SAPDBTRACE_METHOD_DEBUG (
        "Log_ActionRename75::RenameForDelete", LogAction_Trace, 5);


    Msg_List errMsg;
    FileDir_FileNo tableFileNo( oldFileId.fileTabId_gg00() );
    FileDir_IDirectory& fd =
        FileDir_IDirectory::Instance( cgg_nil_transindex );

    FileDir_Table tableEntry;
    fd.GetTableFile (
        TransContext.trTaskId_gg00, tableFileNo, tableEntry, errMsg );

    SAPDBERR_ASSERT_STATE( tableEntry.IsAssigned() );
    if ( ! tableEntry.IsAssigned() ) {
        TransContext.trError_gg00 = e_bad_fdir;
        return;
    }

    if ( (oldFileId.fileTfn_gg00()    == tfnMulti_egg00)
         && (newFileId.fileTfn_gg00() == tfnMulti_egg00) ) {
        SAPDB_UInt2 indexId = oldFileId.fileTfnNo_gg00()[0];

        const FileDir_FileNo indexFileNo =
            tableEntry.GetIndexFileNo( indexId );

        SAPDBERR_ASSERT_STATE( indexFileNo.IsValid() );
        if ( ! indexFileNo.IsValid() ) {
            TransContext.trError_gg00 = e_bad_fdir;
            return;
        }

        tgg00_Surrogate indexSurrogate;
        indexSurrogate.rawAssign( indexFileNo.GetCharArray() );

        Log_ActionReassignSecondaryFile action(
            indexSurrogate,
            oldFileId.fileTabId_gg00(),
            newFileId.fileTabId_gg00() );

        action.ExecuteAction( TransContext );
    } else if ( (oldFileId.fileTfn_gg00()    == tfnShortScol_egg00)
                && (newFileId.fileTfn_gg00() == tfnShortScol_egg00) ) {
        const FileDir_FileNo shortColFileNo =
            tableEntry.GetShortColumnFileNo();

        SAPDBERR_ASSERT_STATE( shortColFileNo.IsValid() );
        if ( ! shortColFileNo.IsValid() ) {
            TransContext.trError_gg00 = e_bad_fdir;
            return;
        }

        tgg00_Surrogate shortColSurrogate;
        shortColSurrogate.rawAssign( shortColFileNo.GetCharArray() );

        Log_ActionReassignSecondaryFile action(
            shortColSurrogate,
            oldFileId.fileTabId_gg00(),
            newFileId.fileTabId_gg00() );

        action.ExecuteAction( TransContext );
    } else {
        SAPDBERR_ASSERT_STATE( false );
        TransContext.trError_gg00 = e_system_error;
    }
}
