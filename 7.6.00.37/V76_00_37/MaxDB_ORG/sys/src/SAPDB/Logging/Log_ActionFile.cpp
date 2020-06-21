/*!
   @file           Log_ActionFile.cpp
   @author         UweH
   @ingroup        ActionHandling
   @brief          handle operations on one file using the new file directory

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
#include "Logging/Log_ActionFile.hpp"
#include "Logging/Log_ActionFileHandling.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "KernelCommon/Kernel_Trace.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "Logging/Log_Exceptions.hpp"
#include "FileDirectory/FileDir_IDirectory.hpp"
#include "FileDirectory/FileDir_ISharedDirectory.hpp"
#include "FileDirectory/FileDir_Generic.hpp"
#include "FileDirectory/FileDir_Table.hpp"
#include "FileDirectory/FileDir_ShortColumn.hpp"
#include "FileDirectory/FileDir_Index.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "hbd01.h"
#include "hbd30.h"
#include "gbd00.h"
#include "DataAccess/Data_FileTypes.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "Messages/Msg_List.hpp"

Log_ActionFile::Log_ActionFile (
    tgg00_TransContext   &TransContext,
    Log_ActionType        actionType,
    const tgg00_Filename &filename)
    : Log_ReadWriteActionImage (actionType),
      m_Type                   (actionType),
      m_Head                   (filename)
{
    tgg00_BasisError &error = TransContext.trError_gg00;

    tgg00_FileId fileid;
    Log_GetFileId (m_Head.filename, fileid, 0);
    Msg_List errorList;

    switch (actionType)
    {
        case Log_MarkFileAsDeleted:
        {
            FileDir_IDirectory &filedir =
                FileDir_IDirectory::SessionInstance( TransContext );
            tgg00_Surrogate tableSurrogate = fileid.fileTabId_gg00();
            FileDir_FileNo      fileNo ( tableSurrogate );
            FileDir_Generic     fileentry;
            FileDir_ReturnCode  result = filedir.GetFile (
                    TransContext.trTaskId_gg00, fileNo, fileentry, errorList);

            if (FileDir_Okay != result)
            {
                // PTS 1131871 M.Ki.
                error = e_file_not_found;
                return;
            }

            m_Head.fileState = fileentry.GetFileState();

            if ( Data_TableFile == fileentry.GetFileType() )
            {   // store file version of table file
                FileDir_Table ft;
                tbd_fileinfo fi;

                ft.Assign (fileentry);
                ft.FillFileInfo (fi);
                m_Head.fileVersion = fi.fi_vers;
            }
        }
        break;
    }
}


//---------------------------------------------------------------------------
void Log_ActionFile::Execute (tgg00_TransContext &TransContext,
                              ExecutionType       executionType) const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionFile::Execute", LogAction_Trace, 5);

    tgg00_BasisError &error = TransContext.trError_gg00;

    KERNEL_TRACE_BASIS_ERROR (error, LogAction_Trace, 5);

    if (e_ok != error)
        return;

    Msg_List errorList;
    tgg00_FileId fileid;
    Log_GetFileId (m_Head.filename, fileid, 0);

    switch ( m_Type )
    {
        case Log_MarkFileAsDeleted:
        {
            FileDir_IDirectory &filedir = FileDir_IDirectory::SessionInstance( TransContext );
            FileDir_FileNo      fileNo (fileid.fileTabId_gg00());
            FileDir_Generic     fileentry;
            FileDir_ReturnCode  result = filedir.GcGetFile (
                TransContext.trTaskId_gg00, fileNo, fileentry, errorList);
            if ( FileDir_Okay != result )
            {
                if ( FileDir_FileNoNotFound == result
                     &&
                     executionType == RedoExecution )
                    // redo of redo: file might already have been removed
                    return;

                if ( FileDir_FileNoNotFound == result
                     &&
                     executionType == OnlineExecution )
                    error = e_file_not_found;
                else
                {
                    SAPDBERR_ASSERT_STATE ( false );
                    error = e_bad_fdir;
                }
                return;
            }
            switch ( executionType )
            {
                case UndoExecution:
                    result = fileentry.RemoveFileDeletedMark();
                    if ( FileDir_Okay != result )
                    {
                        SAPDBERR_ASSERT_STATE ( false );
                        error = e_bad_fdir;
                        return;
                    }
                    switch (fileentry.GetFileType())
                    {
                        case Data_TableFile:
                        {
                            FileDir_Table ft;
                            tbd_fileinfo fi;

                            ft.Assign (fileentry);
                            ft.FillFileInfo (fi);

                            fi.fi_vers = m_Head.fileVersion;
                            ft.SetFileInfo (TransContext.trTaskId_gg00, fi);

                            SetFileVersionInRoot (
                                TransContext,
                                ft,
                                fi.fi_vers);
                        }
                        break;
                        case Data_ShortColumnFile:
                        {
                            FileDir_ShortColumn fe;
                            fe.Assign (fileentry);
                            FileDir_Table ft;
                            if (FileDir_Okay != filedir.GetTableFile (
                                    TransContext.trTaskId_gg00,
                                    fe.GetParentFileNo(), ft, errorList))
                            {
                                SAPDBERR_ASSERT_STATE ( false );
                                error = e_bad_fdir;
                                return;
                            }
                            SAPDBERR_ASSERT_STATE (!ft.HasShortColumnFile());
                            ft.SetShortColumnFileNo (fileNo);
                            break;
                        }
                        case Data_IndexFile:
                        {
                            FileDir_Index fe;
                            fe.Assign (fileentry);
                            FileDir_Table ft;
                            if (FileDir_Okay != filedir.GetTableFile (
                                    TransContext.trTaskId_gg00,
                                    fe.GetParentFileNo(), ft, errorList))
                            {
                                SAPDBERR_ASSERT_STATE ( false );
                                error = e_bad_fdir;
                                return;
                            }
                            if (FileDir_Okay !=
                                ft.AddIndex (fileNo, fe.GetIndexId()))
                            {
                                SAPDBERR_ASSERT_STATE ( false );
                                error = e_bad_fdir;
                                return;
                            }
                            if ( Data_FileNotAccessible == m_Head.fileState )
                                FileDir_ISharedDirectory::Instance()
                                    .IncrementBadIndexCount();
                            break;
                        }
                    }
                    break;
                case RedoExecution:
                {
                    result = fileentry.MarkFileDeleted(TransContext.trWriteTransId_gg00);
                    if ( (FileDir_Okay != result)
                         && (FileDir_AlreadyMarkedForDeletion != result) )
                    {
                        SAPDBERR_ASSERT_STATE ( false );
                        error = e_bad_fdir;
                        return;
                    }

                    switch (fileentry.GetFileType())
                    {
                        case Data_TableFile:
                        {
                            FileDir_Table ft;
                            ft.Assign (fileentry);
                            tgg91_FileVersion fileVersion;
                            fileVersion.gg91SetDummyVers();

                            SetFileVersionInRoot (
                                TransContext,
                                ft,
                                fileVersion);
                        }
                        break;
                        case Data_ShortColumnFile:
                        {
                            FileDir_ShortColumn fe;
                            fe.Assign (fileentry);
                            FileDir_Generic fg;
                            if (FileDir_Okay != filedir.GcGetFile (
                                    TransContext.trTaskId_gg00,
                                    fe.GetParentFileNo(), fg, errorList))
                            {
                                SAPDBERR_ASSERT_STATE ( false );
                                error = e_bad_fdir;
                                return;
                            }
                            FileDir_Table ft;
                            ft.Assign (fg);
                            fileNo.Invalidate();
                            ft.SetShortColumnFileNo (fileNo);
                            break;
                        }
                        case Data_IndexFile:
                        {
                            FileDir_Index fe;
                            fe.Assign (fileentry);
                            FileDir_Generic fg;
                            if (FileDir_Okay != filedir.GcGetFile (
                                    TransContext.trTaskId_gg00,
                                    fe.GetParentFileNo(), fg, errorList))
                            {
                                SAPDBERR_ASSERT_STATE ( false );
                                error = e_bad_fdir;
                                return;
                            }
                            FileDir_Table ft;
                            ft.Assign (fg);
                            result = ft.RemoveIndex (fileNo);
                            if ((FileDir_Okay != result)
                                && (FileDir_IndexNotFound != result))
                            {
                                SAPDBERR_ASSERT_STATE ( false );
                                error = e_bad_fdir;
                                return;
                            }
                            if ( Data_FileNotAccessible == m_Head.fileState )
                                FileDir_ISharedDirectory::Instance()
                                    .DecrementBadIndexCount();
                            break;
                        }
                    }
                    Msg_List errMsg;
                    if ( ! this->RemoveGarbage (TransContext, errMsg) )
                    {
                        // ignore errors during drop (possible during
                        // redo of redo)
                        TransContext.trError_gg00 = e_ok;
                        errMsg.ClearMessageList();
                    }
                    break;
                }
                case OnlineExecution:
                    result = fileentry.MarkFileDeleted(TransContext.trWriteTransId_gg00);
                    if ( FileDir_Okay != result )
                    {
                        SAPDBERR_ASSERT_STATE ( false );
                        error = e_bad_fdir;
                        return;
                    }

                    switch (fileentry.GetFileType())
                    {
                        case Data_TableFile:
                        {
                            FileDir_Table ft;
                            ft.Assign (fileentry);
                            tgg91_FileVersion fileVersion;
                            fileVersion.gg91SetDummyVers();

                            SetFileVersionInRoot (
                                TransContext,
                                ft,
                                fileVersion);
                        }
                        break;
                        case Data_ShortColumnFile:
                        {
                            FileDir_ShortColumn fe;
                            fe.Assign (fileentry);
                            FileDir_Table ft;
                            if (FileDir_Okay != filedir.GetTableFile (
                                    TransContext.trTaskId_gg00,
                                    fe.GetParentFileNo(), ft, errorList))
                            {
                                SAPDBERR_ASSERT_STATE ( false );
                                error = e_bad_fdir;
                                return;
                            }
                            fileNo.Invalidate();
                            ft.SetShortColumnFileNo (fileNo);
                            break;
                        }
                        case Data_IndexFile:
                        {
                            FileDir_Index fe;
                            fe.Assign (fileentry);
                            FileDir_Table ft;
                            if (FileDir_Okay != filedir.GetTableFile (
                                    TransContext.trTaskId_gg00,
                                    fe.GetParentFileNo(), ft, errorList))
                            {
                                SAPDBERR_ASSERT_STATE ( false );
                                error = e_bad_fdir;
                                return;
                            }
                            if (FileDir_Okay !=
                                ft.RemoveIndex (fileNo))
                            {
                                SAPDBERR_ASSERT_STATE ( false );
                                error = e_bad_fdir;
                                return;
                            }
                            if ( Data_FileNotAccessible == m_Head.fileState )
                                FileDir_ISharedDirectory::Instance()
                                    .DecrementBadIndexCount();
                            break;
                        }
                    }
                    break;
                default:
                    error = e_not_implemented;
            }
            break;
        }
        default:
            error = e_not_implemented;
    }
}

//---------------------------------------------------------------------------
bool Log_ActionFile::RemoveGarbage (tgg00_TransContext   &trans,
                                    SAPDBErr_MessageList &errList) const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionFile::RemoveGarbage", LogAction_Trace, 5);

    tgg00_BasisError &error = trans.trError_gg00;

    KERNEL_TRACE_BASIS_ERROR (error, LogAction_Trace, 5);

    if (e_ok != error)
    {
        Kernel_AppendBasisError (error,errList,"Log_ActionFile::RemoveGarbage entered with error");
        return false;
    }

    tgg00_FileId fileid;
    Log_GetFileId (m_Head.filename, fileid, 0);

    switch ( m_Type )
    {
        case Log_MarkFileAsDeleted:
        {
            FileDir_IDirectory &filedir = FileDir_IDirectory::SessionInstance( trans );
            FileDir_FileNo      fileNo (fileid.fileTabId_gg00());
            FileDir_Generic     fileentry;
            FileDir_ReturnCode  result = filedir.GcGetFile (
                trans.trTaskId_gg00, fileNo, fileentry, errList);

            if ( FileDir_Okay != result )
            {
                if ( FileDir_FileNoNotFound == result )
                { // PTS 1133429 M.Ki.
                    return true;
                }
                if ( error == e_ok )
                    error = e_bad_fdir;
                errList = Log_Exception(__CONTEXT__,LOG_ACTION_GET_FILE_FAILED,
                                        SAPDB_ToString(result));
                AppendContextInfo(__CONTEXT__,errList);
                Kernel_AppendBasisError (error,errList,"FileDir_IDirectory::GetFile");
                return false;
            }

            if (Data_FileDeleted == fileentry.GetFileState())
            {
                b01pdestroy_perm_file (trans, fileid);
            }
            else
            {
                error = e_bad_fdir;
            }

            if ( error != e_ok )
            {
                errList = Log_Exception(__CONTEXT__,LOG_ACTION_DESTROY_FILE_FAILED);
                AppendContextInfo(__CONTEXT__,errList);
                Kernel_AppendBasisError (error,errList,"FileDir_IDirectory::GetFile");
                return false;
            }
            break;
        }
        default:
            /*nothing*/ ;
    }
    return true;
}

//---------------------------------------------------------------------------
void Log_ActionFile::WriteToTrace (const char * Title) const
{
    if ( Title != 0 )
        Kernel_VTrace() << Title;

    Kernel_TraceBuffer (&m_Head.filename, sizeof(m_Head.filename),
                        "Filename", sizeof(m_Head.filename));

    Kernel_VTrace() << "Current ActionType: " << Log_GetActionTypeString(m_Type);
}

//---------------------------------------------------------------------------
void Log_ActionFile::AppendContextInfo (SAPDB_Char  const * const  filename,
                                        SAPDB_UInt4 const          linenumber,
                                        SAPDBErr_MessageList      &errList,
                                        SAPDB_Char  const * const  title) const
{
    errList = errList + Log_Exception( filename, linenumber, LOG_ACTION_FILE_INFO,
                                       (title!=0 ? title : "Log_ActionFile"),
                                       Log_GetActionTypeString(m_Type),
                                       SAPDB_ToString(&m_Head.filename,sizeof(m_Head.filename)));
}


//---------------------------------------------------------------------------

SAPDB_Bool Log_ActionFile::SetFileVersionInRoot (
    tgg00_TransContext&   trans,
    FileDir_Table&        fileEntry,
    tgg91_FileVersion     newFileVersion) const
{
    tbd_current_tree current;
    tgg00_FileId fileId = b01niltree_id;

    tbd_fileinfo fi;
    fileEntry.FillFileInfo( fi );
    fileId.fileTabId_gg00().rawAssign (fileEntry.GetFileNo().GetCharArray());

    b30fdir_to_treeid (fi, fileId);
    bd30GetTree (trans, fileId, current, m_set,
            LOCK_TREE_EXCL_BD00, ! SYNC_TREE_LOCK_BD00);
    if (e_ok == trans.trError_gg00)
        bd30SetFileVersion (current, newFileVersion);
    bd30ReleaseTree (current);

    if (e_ok != trans.trError_gg00)
        return false;
    return true;
}

//---------------------------------------------------------------------------

void Log_ActionFile::SetFileVersion(const tgg91_FileVersion fileVersion)
{
    m_Head.fileVersion = fileVersion;
}

//---------------------------------------------------------------------------
