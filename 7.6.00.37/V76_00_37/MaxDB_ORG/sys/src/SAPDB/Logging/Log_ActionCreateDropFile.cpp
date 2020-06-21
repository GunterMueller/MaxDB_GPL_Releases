/*!**************************************************************************

    module      : Log_ActionCreateDropFile.cpp
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

#include "Logging/Log_ActionCreateDropFile.hpp"

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "KernelCommon/Kernel_Trace.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "KernelCommon/Kernel_IAdminHotStandby.hpp"
#include "Logging/Log_InvDescMap.hpp"
#include "Logging/Log_ReadWriteActionImage.hpp"
#include "Logging/Log_Transaction.hpp"
#include "Logging/Log_Exceptions.hpp"

#include "FileDirectory/FileDir_IDirectory.hpp"
#include "FileDirectory/FileDir_Table.hpp"
#include "FileDirectory/FileDir_Index.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "DataAccess/Data_Types.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "Messages/Msg_List.hpp"
#include "ggg00.h"

#include "Logging/Log_ActionFileHandling.hpp"

#include "ggg91.h"
#include "ggg00.h"
#include "hbd01.h"
#include "hbd03.h"
#include "hkb64.h"

//---------------------------------------------------------------------------
Log_ActionChangeFileType::Log_ActionChangeFileType (const tgg00_Filename &Filename,
                                                    tgg00_FiletypeSet     OldFiletypeSet,
                                                    tgg00_FiletypeSet     NewFiletypeSet):
Log_ReadWriteActionImage (Log_ChangeFileType)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionChangeFileType::Log_ActionChangeFileType", LogAction_Trace, 5);
    m_Entry.Filename       = Filename;
    m_Entry.OldFiletypeSet = OldFiletypeSet;
    m_Entry.NewFiletypeSet = NewFiletypeSet;
}
//---------------------------------------------------------------------------
void Log_ActionChangeFileType::ExecuteAction (tgg00_TransContext &TransContext,
                                              tgg00_FileId       *pCommandFileId) const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionChangeFileType::ExecuteAction", LogAction_Trace, 5);
    
    KERNEL_TRACE_BASIS_ERROR (TransContext.trError_gg00, LogAction_Trace, 5);
    
    if (e_ok != TransContext.trError_gg00) return; 
    
    tgg00_FileId  RedoFileId;
    
    tgg00_FileId *pExecuteFileId = Log_GetFileId (m_Entry.Filename, RedoFileId, pCommandFileId);
    
    pExecuteFileId->fileType_gg00() = m_Entry.OldFiletypeSet;

    tgg00_FiletypeSet AuxNewSet = m_Entry.NewFiletypeSet;

    b01change_filetype (TransContext, *pExecuteFileId, AuxNewSet);
}
//---------------------------------------------------------------------------
bool Log_ActionChangeFileType::ReadImagePersistent (Log_ActionImageReader &ImageReader,
                                                    SAPDBErr_MessageList  &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionChangeFileType::ReadImagePersistent", LogAction_Trace, 5);
    return ImageReader.CopyImage (REINTERPRET_CAST (SAPDB_Byte*, &(m_Entry)),
                                  sizeof(m_Entry),
                                  errlist);
}
//---------------------------------------------------------------------------
Log_ActionCreateFile::Log_ActionCreateFile (const tgg00_Filename &Filename,
                                            tgg91_FileVersion     FileVersion,
                                            tgg00_FiletypeSet     FiletypeSet):
Log_ReadWriteActionImage (Log_CreateFile)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionCreateFile::Log_ActionCreateFile", LogAction_Trace, 5);
    m_Entry.Filename    = Filename;
    m_Entry.FileVersion = FileVersion;
    m_Entry.FiletypeSet = FiletypeSet;
}
//---------------------------------------------------------------------------
void Log_ActionCreateFile::ExecuteAction (tgg00_TransContext &TransContext,
                                          tgg00_FileId       *pCommandFileId) const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionCreateFile::ExecuteAction", LogAction_Trace, 5);
    
    KERNEL_TRACE_BASIS_ERROR (TransContext.trError_gg00, LogAction_Trace, 5);
    
    if (e_ok != TransContext.trError_gg00) return; 
    
    tgg00_FileId  RedoFileId;
    
    tgg00_FileId *pExecuteFileId =
        Log_GetFileId (m_Entry.Filename, RedoFileId, pCommandFileId);
    
    pExecuteFileId->fileVersion_gg00() = m_Entry.FileVersion;
    pExecuteFileId->fileType_gg00()    = m_Entry.FiletypeSet;
    
    b01bcreate_bytestr (TransContext, *pExecuteFileId);
    
    if ((e_duplicate_filename == TransContext.trError_gg00) && (NULL == pCommandFileId))
    {
        TransContext.trError_gg00 = e_ok;  // ignore redo error
    }
}
//---------------------------------------------------------------------------
bool Log_ActionCreateFile::ReadImagePersistent (Log_ActionImageReader &ImageReader,
                                                SAPDBErr_MessageList  &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionCreateFile::ReadImagePersistent", LogAction_Trace, 5);
    return ImageReader.CopyImage (REINTERPRET_CAST (SAPDB_Byte*, &(m_Entry)),sizeof(m_Entry),errlist);
}
//---------------------------------------------------------------------------

Log_ActionCreateIndex::Log_ActionCreateIndex (const tgg00_Filename &TabFilename,
                                              const tgg00_Filename &InvFilename,
                                              const Log_InvDescMap *pInvDescMap,
                                              tgg00_Tfn             PrimTfn,
                                              SAPDB_UInt1           PrimTfnNo)
    :
Log_ReadWriteActionImage (Log_CreateIndex),
m_InvDescMapForRead      (),
m_pInvDescMap            (pInvDescMap)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionCreateIndex::Log_ActionCreateIndex", LogAction_Trace, 5);
    m_EntryHeader.TabFilename   = TabFilename;
    m_EntryHeader.InvFilename   = InvFilename;
    m_EntryHeader.InvDescMapLen = pInvDescMap->GetPersistentLength();
    m_EntryHeader.PrimTfn       = PrimTfn;
    m_EntryHeader.PrimTfnNo     = PrimTfnNo;
}
//---------------------------------------------------------------------------
void Log_ActionCreateIndex::ExecuteAction (tgg00_TransContext &TransContext,
                                           tgg00_FileId       *pCommandPermFileId,
                                           tgg00_FileId       *pCommandInvFileId,
                                           tgg00_IndexCounter *pCommandIndexCounter) const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionCreateIndex::ExecuteAction", LogAction_Trace, 5);
    
    KERNEL_TRACE_BASIS_ERROR (TransContext.trError_gg00, LogAction_Trace, 5);
    
    if (e_ok != TransContext.trError_gg00) return; 
    
    tgg00_StackDesc    StackDesc;
    tgg00_FileId       RedoPermFileId;
    tgg00_FileId       RedoInvFileId;
    
    tgg00_FileId *pExecutePermFileId =
        Log_GetFileId (m_EntryHeader.TabFilename, RedoPermFileId, pCommandPermFileId);
    tgg00_FileId *pExecuteInvFileId =
        Log_GetFileId (m_EntryHeader.InvFilename, RedoInvFileId, pCommandInvFileId);
    
    m_pInvDescMap->BuildStackDesc (StackDesc);

    const tgg00_IndexMode   IndexMode  = tgg00_IndexMode::fromConst (create_index);
    const SAPDB_UInt        indexId    = m_pInvDescMap->InvDescId(0);
    tgg00_IndexCounter      IndexCounter;

    IndexCounter.idc_prim_keycount  = 0;
    IndexCounter.idc_nullcount      = 0;
    IndexCounter.idc_sec_keycount   = 0;
    IndexCounter.idc_prim_leafnodes = 1;
    IndexCounter.idc_sec_leafnodes  = 1;
    
    bd03BuildIndex (TransContext, IndexMode, *pExecutePermFileId, *pExecuteInvFileId, StackDesc, indexId, IndexCounter);
    
    if (e_ok != TransContext.trError_gg00) return;
    
    if (NULL != pCommandIndexCounter)
    {
        *pCommandIndexCounter = IndexCounter;
    }
}
//---------------------------------------------------------------------------
void Log_ActionCreateIndex::ExecuteSetNotAccessible (tgg00_TransContext &TransContext) const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionCreateIndex::ExecuteSetNotAccessible", LogAction_Trace, 5);
    
    if (e_ok != TransContext.trError_gg00) 
        return; 

    Msg_List           errorList;
    tgg00_StackDesc    StackDesc;
    tgg00_FileId       RedoPermFileId;
    tgg00_FileId       RedoInvFileId;
    
   
    tgg00_FileId *pExecutePermFileId =
        Log_GetFileId (m_EntryHeader.TabFilename, RedoPermFileId, NULL);
    tgg00_FileId *pExecuteInvFileId =
        Log_GetFileId (m_EntryHeader.InvFilename, RedoInvFileId, NULL);
    
    m_pInvDescMap->BuildStackDesc (StackDesc);
    
    pExecutePermFileId->fileTfn_gg00  ()    = m_EntryHeader.PrimTfn;
    pExecutePermFileId->fileTfnNo_gg00()[0] = m_EntryHeader.PrimTfnNo;
    
    const FileDir_FileNo indexFileNo (pExecuteInvFileId->fileTabId_gg00());

    // if entry is in file directory but marked as deleted then delete the file
    FileDir_Generic fileEntry;
    FileDir_IDirectory& fd = FileDir_IDirectory::Instance (TransContext.trIndex_gg00);
    fd.GcGetFile (TransContext.trTaskId_gg00, indexFileNo, fileEntry, errorList);

    if (fileEntry.IsAssigned()) // TODO (TS): use file if exist!
    { 
        fileEntry.Detach();
        b01pdestroy_perm_file (TransContext, *pExecuteInvFileId);
        TransContext.trError_gg00 = e_ok;
    }

    const SAPDB_UInt    indexId    = m_pInvDescMap->InvDescId(0);
    tgg00_IndexCounter  dummyIndexCounter;

    pExecuteInvFileId->fileRoot_gg00() = NIL_PAGE_NO_GG00;

    if ( Kernel_IAdminHotStandby::Instance().RoleIsStandby() ) // PTS 1120249 UH 2003-01-24
    {
        tgg00_IndexMode   indexMode;
        indexMode.becomes( create_index );

        bd03BuildIndex( TransContext, indexMode, *pExecutePermFileId, *pExecuteInvFileId, StackDesc, indexId, dummyIndexCounter );

    }
    else // restart_index
    {
        bd03CreateIndexFile( TransContext, *pExecuteInvFileId, *pExecutePermFileId, indexId );

        if( e_ok == TransContext.trError_gg00 )
            bd03SetToNotAccessible( TransContext, *pExecuteInvFileId );
    }
 
    if ((e_file_not_found      == TransContext.trError_gg00)
        ||
        (e_file_not_accessible == TransContext.trError_gg00))
    {
        TransContext.trError_gg00 = e_ok;
    }
}
//---------------------------------------------------------------------------
bool Log_ActionCreateIndex::ReadImagePersistent (Log_ActionImageReader &ImageReader,
                                                 SAPDBErr_MessageList  &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionCreateIndex::ReadImagePersistent", LogAction_Trace, 5);
    
    m_pInvDescMap = &(m_InvDescMapForRead);
    
    if ( ! ImageReader.CopyImage (REINTERPRET_CAST (SAPDB_Byte*, &(m_EntryHeader)),sizeof(m_EntryHeader),errlist))
        return false;
        
    if (m_EntryHeader.InvDescMapLen > 0)
    {
        return m_InvDescMapForRead.ReadImagePersistent(ImageReader, m_EntryHeader.InvDescMapLen, errlist);
    }
    return true;
}
//---------------------------------------------------------------------------
bool Log_ActionCreateIndex::WriteImagePersistent (Log_ActionImageWriter &ImageWriter,
                                                  SAPDBErr_MessageList  &errlist)       const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionCreateIndex::WriteImagePersistent", LogAction_Trace, 5);
    if ( ! ImageWriter.PersistentWrite (&(m_EntryHeader), sizeof (m_EntryHeader), errlist) )
        return false;
    return m_pInvDescMap->WriteImagePersistent (ImageWriter, errlist);
}
//---------------------------------------------------------------------------
void Log_ActionDropFile::ExecuteAction (tgg00_TransContext &TransContext,
                                        tgg00_FileId       *pCommandFileId) const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionDropFile::ExecuteAction", LogAction_Trace, 5);
    
    KERNEL_TRACE_BASIS_ERROR (TransContext.trError_gg00, LogAction_Trace, 5);
    
    if (e_ok != TransContext.trError_gg00) return; 
    
    tgg00_FileId  RedoFileId;
    tgg00_FileId *pExecuteFileId; 
    
    pExecuteFileId = Log_GetFileId (m_Filename, RedoFileId, pCommandFileId);
    
    b01pdestroy_perm_file (TransContext, *pExecuteFileId);
    
    if ((e_file_not_found == TransContext.trError_gg00) && (NULL == pCommandFileId))
    {
        TransContext.trError_gg00 = e_ok;  // ignore undo error
    }
}
//---------------------------------------------------------------------------
bool Log_ActionDropFile::ReadImagePersistent (Log_ActionImageReader &ImageReader,
                                              SAPDBErr_MessageList  &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionDropFile::ReadImagePersistent", LogAction_Trace, 5);
    return ImageReader.CopyImage (REINTERPRET_CAST (SAPDB_Byte*, &(m_Filename)),sizeof(m_Filename),errlist);
}
//---------------------------------------------------------------------------
Log_ActionRenameFile760002::Log_ActionRenameFile760002 (const tgg00_Filename &OldFilename,
                                            const tgg00_Filename &NewFilename,           
                                            tgg91_FileVersion     NewFileVersion):
Log_ReadWriteActionImage (Log_RenameFile)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionRenameFile760002::Log_ActionRenameFile760002", LogAction_Trace, 5);
    m_Entry.OldFilename    = OldFilename;
    m_Entry.NewFilename    = NewFilename;
    m_Entry.NewFileVersion = NewFileVersion;
}
//---------------------------------------------------------------------------
void Log_ActionRenameFile760002::ExecuteAction (tgg00_TransContext &TransContext,
                                          tgg00_FileId       *pOldCommandFileId,
                                          tgg00_FileId       *pNewCommandFileId) const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionRenameFile760002::ExecuteAction", LogAction_Trace, 5);
    
    KERNEL_TRACE_BASIS_ERROR (TransContext.trError_gg00, LogAction_Trace, 5);
    
    if (e_ok != TransContext.trError_gg00) return; 
    
    tgg00_FileId  OldRedoFileId;
    tgg00_FileId  NewRedoFileId;
    tgg00_FileId *pOldExecuteFileId; 
    tgg00_FileId *pNewExecuteFileId; 
    
    pOldExecuteFileId = Log_GetFileId (m_Entry.OldFilename, OldRedoFileId, pOldCommandFileId);
    pNewExecuteFileId = Log_GetFileId (m_Entry.NewFilename, NewRedoFileId, pNewCommandFileId);

    // the following file types cannot be handled via the rename mechanism:
    SAPDBERR_ASSERT_STATE(
        (tfnMulti_egg00 != pOldExecuteFileId->fileTfn_gg00())
        && (tfnMulti_egg00 != pNewExecuteFileId->fileTfn_gg00())
        && (tfnShortScol_egg00 != pOldExecuteFileId->fileTfn_gg00())
        && (tfnShortScol_egg00 != pNewExecuteFileId->fileTfn_gg00())
        );

    tgg00_Surrogate dummySurrogate;
    SAPDB_MemMoveNoCheck (dummySurrogate, cgg_zero_id, sizeof(dummySurrogate));

    bd01RenameFile (TransContext, dummySurrogate, *pOldExecuteFileId, *pNewExecuteFileId);

    if (e_ok == TransContext.trError_gg00)
    {
        if (pOldExecuteFileId->fileVersion_gg00() != m_Entry.NewFileVersion)
        { 
            const SAPDB_Int2 DummyLongCnt = -1;
            
            b01set_fileversion (TransContext, *pNewExecuteFileId,
                m_Entry.NewFileVersion, DummyLongCnt);
        }
    }
    
    if ((e_file_not_found == TransContext.trError_gg00) && (NULL == pNewCommandFileId))
    {
        TransContext.trError_gg00 = e_ok;  // ignore undo/redo error
    }
}
//---------------------------------------------------------------------------
void Log_ActionRenameFile760002::AppendContextInfo (SAPDB_Char  const * const  filename,
                                              SAPDB_UInt4 const          linenumber,
                                              SAPDBErr_MessageList      &errlist,
                                              SAPDB_Char  const * const  title) const
{
    tgg00_Filename oldfn = m_Entry.OldFilename;
    tgg00_Filename newfn = m_Entry.OldFilename;
    errlist = errlist +
        Log_Exception(filename,linenumber,LOG_ACTION_RENAME_FILE_INFO,
                      title==0?"Log_ActionRenameFile760002":title,
                      SAPDB_ToString(oldfn.asCharp(),sizeof(tgg00_Filename),SAPDB_ToStringClass::hex),
                      SAPDB_ToString(newfn.asCharp(),sizeof(tgg00_Filename),SAPDB_ToStringClass::hex),
                      SAPDB_ToString(m_Entry.NewFileVersion.gg90GetInt4()) );
}
//---------------------------------------------------------------------------
void Log_ActionRenameFile760002::WriteToTrace (const char * Title) const
{
    Kernel_VTrace trace;
    if ( Title != 0 )
        trace << "Log_ActionRenameFile760002::" << Title << FlushLine;
    tgg00_Filename oldfn = m_Entry.OldFilename;
    tgg00_Filename newfn = m_Entry.OldFilename;
	trace << "oldFilename: " << SAPDB_ToString(oldfn.asCharp(),sizeof(tgg00_Filename),SAPDB_ToStringClass::hex) << FlushLine;
	trace << "newFilename: " << SAPDB_ToString(newfn.asCharp(),sizeof(tgg00_Filename),SAPDB_ToStringClass::hex) << ", " << m_Entry.NewFileVersion.gg90GetInt4() << FlushLine;
}
//---------------------------------------------------------------------------
bool Log_ActionRenameFile760002::RemoveGarbage (tgg00_TransContext   &trans,
                                          SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionRenameFile760002::RemoveGarbage", LogAction_Trace, 5);
    KERNEL_TRACE_BASIS_ERROR (trans.trError_gg00, LogAction_Trace, 5);
    
    if (e_ok != trans.trError_gg00)
        return false; 
    
    tgg00_FileId  fileid;
    tgg00_FileId *pFileid; 
    
    pFileid = Log_GetFileId (m_Entry.OldFilename,fileid,0);
    
    b01pdestroy_perm_file (trans, *pFileid);

    if ( e_file_not_found == trans.trError_gg00 )
        trans.trError_gg00 = e_ok;  // ignore error during garbage collection

    if ( e_ok != trans.trError_gg00 )
    {
        WriteToTrace("Log_ActionRenameFile760002::RemoveGarbage");
        errlist = Log_Exception(__CONTEXT__,LOG_REMOVE_FILE_FAILED);
        AppendContextInfo (__CONTEXT__, errlist, "Log_ActionRenameFile760002::RemoveGarbage");
        Kernel_AppendBasisError(trans.trError_gg00,errlist,"b01pdestroy_perm_file");
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
bool Log_ActionRenameFile760002::ReadImagePersistent (Log_ActionImageReader &ImageReader,
                                                SAPDBErr_MessageList  &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionRenameFile760002::ReadImagePersistent", LogAction_Trace, 5);
    return ImageReader.CopyImage (REINTERPRET_CAST (SAPDB_Byte*, &(m_Entry)),sizeof(m_Entry),errlist);
}
//---------------------------------------------------------------------------
Log_ActionRenameFile::Log_ActionRenameFile (const tgg00_Surrogate &TableId,
                                            const tgg00_Filename  &OldFilename,
                                            const tgg00_Filename  &NewFilename,           
                                            tgg91_FileVersion      NewFileVersion)
: Log_ReadWriteActionImage (Log_RenameFile)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionRenameFile::Log_ActionRenameFile", LogAction_Trace, 5);
    m_Entry.TableId        = TableId;
    m_Entry.OldFilename    = OldFilename;
    m_Entry.NewFilename    = NewFilename;
    m_Entry.NewFileVersion = NewFileVersion;
}
//---------------------------------------------------------------------------
void Log_ActionRenameFile::ExecuteAction (tgg00_TransContext &TransContext,
                                          tgg00_FileId       *pOldCommandFileId,
                                          tgg00_FileId       *pNewCommandFileId) const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionRenameFile::ExecuteAction", LogAction_Trace, 5);
    
    KERNEL_TRACE_BASIS_ERROR (TransContext.trError_gg00, LogAction_Trace, 5);
    
    if (e_ok != TransContext.trError_gg00)
        return; 
    
    tgg00_FileId  OldRedoFileId;
    tgg00_FileId  NewRedoFileId;
    tgg00_FileId *pOldExecuteFileId; 
    tgg00_FileId *pNewExecuteFileId; 
    
    pOldExecuteFileId = Log_GetFileId (m_Entry.OldFilename, OldRedoFileId, pOldCommandFileId);
    pNewExecuteFileId = Log_GetFileId (m_Entry.NewFilename, NewRedoFileId, pNewCommandFileId);

    // the following file types cannot be handled via the rename mechanism:
    SAPDBERR_ASSERT_STATE(
        (tfnMulti_egg00 != pOldExecuteFileId->fileTfn_gg00())
        && (tfnMulti_egg00 != pNewExecuteFileId->fileTfn_gg00())
        && (tfnShortScol_egg00 != pOldExecuteFileId->fileTfn_gg00())
        && (tfnShortScol_egg00 != pNewExecuteFileId->fileTfn_gg00())
        );

    bd01RenameFile (TransContext, m_Entry.TableId, *pOldExecuteFileId, *pNewExecuteFileId);

    if (e_ok == TransContext.trError_gg00)
    {
        if (pOldExecuteFileId->fileVersion_gg00() != m_Entry.NewFileVersion)
        { 
            const SAPDB_Int2 DummyLongCnt = -1;
            
            b01set_fileversion (TransContext, *pNewExecuteFileId,
                m_Entry.NewFileVersion, DummyLongCnt);
        }
    }
    
    if ((e_file_not_found == TransContext.trError_gg00) && (NULL == pNewCommandFileId))
    {
        TransContext.trError_gg00 = e_ok;  // ignore undo/redo error
    }
}
//---------------------------------------------------------------------------
void Log_ActionRenameFile::AppendContextInfo (SAPDB_Char  const * const  filename,
                                              SAPDB_UInt4 const          linenumber,
                                              SAPDBErr_MessageList      &errlist,
                                              SAPDB_Char  const * const  title) const
{
    tgg00_Filename oldfn = m_Entry.OldFilename;
    tgg00_Filename newfn = m_Entry.OldFilename;
    errlist = errlist +
        Log_Exception(filename,linenumber,LOG_ACTION_RENAME_FILE_INFO,
                      title==0?"Log_ActionRenameFile":title,
                      SAPDB_ToString(oldfn.asCharp(),sizeof(tgg00_Filename),SAPDB_ToStringClass::hex),
                      SAPDB_ToString(newfn.asCharp(),sizeof(tgg00_Filename),SAPDB_ToStringClass::hex),
                      SAPDB_ToString(m_Entry.NewFileVersion.gg90GetInt4()) );
}
//---------------------------------------------------------------------------
void Log_ActionRenameFile::WriteToTrace (const char * Title) const
{
    Kernel_VTrace trace;
    if ( Title != 0 )
        trace << "Log_ActionRenameFile::" << Title << FlushLine;
    tgg00_Filename oldfn = m_Entry.OldFilename;
    tgg00_Filename newfn = m_Entry.OldFilename;
	trace << "oldFilename: " << SAPDB_ToString(oldfn.asCharp(),sizeof(tgg00_Filename),SAPDB_ToStringClass::hex) << FlushLine;
	trace << "newFilename: " << SAPDB_ToString(newfn.asCharp(),sizeof(tgg00_Filename),SAPDB_ToStringClass::hex) << ", " << m_Entry.NewFileVersion.gg90GetInt4() << FlushLine;
}
//---------------------------------------------------------------------------
bool Log_ActionRenameFile::RemoveGarbage (tgg00_TransContext   &trans,
                                          SAPDBErr_MessageList &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionRenameFile::RemoveGarbage", LogAction_Trace, 5);
    KERNEL_TRACE_BASIS_ERROR (trans.trError_gg00, LogAction_Trace, 5);
    
    if (e_ok != trans.trError_gg00)
        return false; 
    
    tgg00_FileId  fileid;
    tgg00_FileId *pFileid; 
    
    pFileid = Log_GetFileId (m_Entry.OldFilename,fileid,0);
    
    b01pdestroy_perm_file (trans, *pFileid);

    if ( e_file_not_found == trans.trError_gg00 )
        trans.trError_gg00 = e_ok;  // ignore error during garbage collection

    if ( e_ok != trans.trError_gg00 )
    {
        WriteToTrace("Log_ActionRenameFile::RemoveGarbage");
        errlist = Log_Exception(__CONTEXT__,LOG_REMOVE_FILE_FAILED);
        AppendContextInfo (__CONTEXT__, errlist, "Log_ActionRenameFile::RemoveGarbage");
        Kernel_AppendBasisError(trans.trError_gg00,errlist,"b01pdestroy_perm_file");
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
bool Log_ActionRenameFile::ReadImagePersistent (Log_ActionImageReader &ImageReader,
                                                SAPDBErr_MessageList  &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionRenameFile::ReadImagePersistent", LogAction_Trace, 5);
    return ImageReader.CopyImage (REINTERPRET_CAST (SAPDB_Byte*, &(m_Entry)),sizeof(m_Entry),errlist);
}
//---------------------------------------------------------------------------
Log_ActionSuccFileVersion::Log_ActionSuccFileVersion (const tgg00_Filename &Filename,
                                                      tgg91_FileVersion     NewFileVersion,
                                                      SAPDB_Int4            NewLongCnt):
Log_ReadWriteActionImage (Log_SuccFileVersion)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionSuccFileVersion::Log_ActionSuccFileVersion", LogAction_Trace, 5);
    m_Entry.Filename       = Filename;
    m_Entry.NewFileVersion = NewFileVersion;
    m_Entry.NewLongCnt     = NewLongCnt;
}
//---------------------------------------------------------------------------
void Log_ActionSuccFileVersion::ExecuteAction (tgg00_TransContext &TransContext,
                                               tgg00_FileId       *pCommandFileId) const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionSuccFileVersion::ExecuteAction", LogAction_Trace, 5);
    
    KERNEL_TRACE_BASIS_ERROR (TransContext.trError_gg00, LogAction_Trace, 5);
    
    if (e_ok != TransContext.trError_gg00) return; 
    
    tgg00_FileId  RedoFileId;
    tgg00_FileId *pExecuteFileId; 
    
    pExecuteFileId = Log_GetFileId (m_Entry.Filename, RedoFileId, pCommandFileId);
    
    b01set_fileversion (TransContext, *pExecuteFileId,
        m_Entry.NewFileVersion, m_Entry.NewLongCnt);
    if ((e_file_not_found == TransContext.trError_gg00)
        && reinterpret_cast<Log_Transaction*>(TransContext.trLogContext_gg00)
        ->WasOpenInLastSavepoint())
    { // ignore error during redo if file is marked deleted
        Msg_List        errorList;
        FileDir_Generic fileEntry;
        FileDir_IDirectory& fd = FileDir_IDirectory::Instance (TransContext.trIndex_gg00);
        fd.GcGetFile (TransContext.trTaskId_gg00, pExecuteFileId->fileTabId_gg00(), fileEntry, errorList);
        if (fileEntry.IsAssigned() && (fileEntry.GetFileState() == Data_FileDeleted))
            TransContext.trError_gg00 = e_ok;
    }

    if (e_ok != TransContext.trError_gg00) return;
    
    pExecuteFileId->fileVersion_gg00() = m_Entry.NewFileVersion;
}
//---------------------------------------------------------------------------
bool Log_ActionSuccFileVersion::ReadImagePersistent (Log_ActionImageReader &ImageReader,
                                                     SAPDBErr_MessageList  &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionSuccFileVersion::ReadImagePersistent", LogAction_Trace, 5);
    return ImageReader.CopyImage (REINTERPRET_CAST (SAPDB_Byte*, &(m_Entry)),sizeof(m_Entry),errlist);
}
//---------------------------------------------------------------------------
Log_ActionUpdFileStatistic::Log_ActionUpdFileStatistic (const tgg00_Filename &Filename,
                                                        SAPDB_Int4            TreeLeafNodes):
Log_ReadWriteActionImage (Log_UpdFileStatistic)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionUpdFileStatistic::Log_ActionUpdFileStatistic", LogAction_Trace, 5);
    m_Entry.Filename  = Filename;
    m_Entry.LeafNodes = TreeLeafNodes;
}
//---------------------------------------------------------------------------
void Log_ActionUpdFileStatistic::ExecuteAction (tgg00_TransContext &TransContext,
                                                tgg00_FileId       *pCommandFileId) const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionUpdFileStatistic::ExecuteAction", LogAction_Trace, 5);
    
    KERNEL_TRACE_BASIS_ERROR (TransContext.trError_gg00, LogAction_Trace, 5);
    
    if (e_ok != TransContext.trError_gg00) return; 
    
    SAPDB_Int4    DummyLeafNodes;
    tgg00_FileId  RedoFileId;
    tgg00_FileId *pExecuteFileId; 
    
    pExecuteFileId = Log_GetFileId (m_Entry.Filename, RedoFileId, pCommandFileId);
    
    pExecuteFileId->fileLeafNodes_gg00() = m_Entry.LeafNodes;
    
    b01treeleafnodes (TransContext, *pExecuteFileId, m_update, DummyLeafNodes);
}
//---------------------------------------------------------------------------
bool Log_ActionUpdFileStatistic::ReadImagePersistent (Log_ActionImageReader &ImageReader,
                                                      SAPDBErr_MessageList  &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionUpdFileStatistic::ReadImagePersistent", LogAction_Trace, 5);
    return ImageReader.CopyImage (REINTERPRET_CAST (SAPDB_Byte*, &(m_Entry)),sizeof(m_Entry),errlist);
}
