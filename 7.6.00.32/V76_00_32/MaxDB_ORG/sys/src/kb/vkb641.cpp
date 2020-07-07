/*!**************************************************************************

    module      : vkb641.cpp

    -------------------------------------------------------------------------

    author      : JuergenA
    responsible : UweH

    special area: Logging
    description : KB_CreateDropFile_interface

    last changed: 2001-03-12

    -------------------------------------------------------------------------

    copyright:    Copyright (c) 2001-2005 SAP AG

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



*****************************************************************************/

/*==========================================================================*
*  INCLUDES                                                                 *
*===========================================================================*/

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "Logging/Log_ActionFile.hpp"
#include "Logging/Log_ActionCreateDropFile.hpp"
#include "Logging/Log_ActionCreateIndexFile.hpp"
#include "Logging/Log_ActionCreateTable.hpp"
#include "Logging/Log_ActionCreateShortColumnFile.hpp"
#include "Logging/Log_ActionReorganizeFile.hpp"
#include "Logging/Log_InvDescMap.hpp"
#include "Logging/Log_Transaction.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "gsp00.h"
#include "ggg91.h"
#include "ggg00.h"
#include "hbd01.h"
#include "hkb560.h"
#include "hkb641.h" // check of PASCAL header vkb641


/*-----------------------------------------------------------------------------------*/

static inline Log_Transaction&
GetLogTrans (tgg00_TransContext &Trans,
                   const char *        Msg)
{
    Log_Transaction* LogTrans = REINTERPRET_CAST(Log_Transaction*,Trans.trLogContext_gg00);
    if ( LogTrans == NULL )
        RTE_Crash( SAPDBErr_Exception(__FILE__, __LINE__,
                                      SAPDBERR_ASSERT_STATE_FAILED, Msg) );
    return *LogTrans;
}

//----------------------------------------------------------------------------

externPascal void kb641BeginLoad (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    tgg00_FileId          VAR_VALUE_REF  fileid,
    tgg00_Lkey            VAR_VALUE_REF  lastkey)
{
    SAPDB_Byte* pKey = reinterpret_cast<SAPDB_Byte*>( &(lastkey.keyVal_gg00()) );
    Log_ActionReorganizeFile beforeimage ( fileid.fileName_gg00(),
                                           lastkey.keyLen_gg00(),
                                           pKey );
    beforeimage.WriteBeforeImage (trans);
}

//----------------------------------------------------------------------------

externPascal void kb641ChangeFileType (tgg00_TransContext      &TransContext,
                                       tgg00_FileId            &FileId,
                                       const tgg00_FiletypeSet &NewFiletypeSet)
{
    Log_ActionChangeFileType
        BeforeImage (FileId.fileName_gg00(), NewFiletypeSet, FileId.fileType_gg00());
    
    Log_ActionChangeFileType
        AfterImage (FileId.fileName_gg00(), FileId.fileType_gg00(), NewFiletypeSet);
    
    BeforeImage.WriteBeforeImage (TransContext);
    
    if (e_ok != TransContext.trError_gg00) return;
    
    AfterImage.ExecuteAction (TransContext, &FileId);
    
    if (e_ok != TransContext.trError_gg00) return;
    
    AfterImage.WriteAfterImage (TransContext);
}

//----------------------------------------------------------------------------

externPascal void kb641CreateFile (tgg00_TransContext &TransContext,
                                   tgg00_FileId       &FileId)
{
    Log_ActionDropFile BeforeImage (FileId.fileName_gg00());
    
    Log_ActionCreateFile
        AfterImage (FileId.fileName_gg00(), FileId.fileVersion_gg00(), FileId.fileType_gg00());
    
    BeforeImage.WriteBeforeImage (TransContext);
    
    if (e_ok != TransContext.trError_gg00) return;
    
    AfterImage.ExecuteAction (TransContext, &FileId);
    
    if (e_ok != TransContext.trError_gg00) return;
    
    AfterImage.WriteAfterImage (TransContext);
}

//----------------------------------------------------------------------------

externPascal void kb641CreateIndex (tgg00_TransContext    &TransContext,
                                    tgg00_FileId          &PrimFileId,
                                    tgg00_FileId          &InvFileId,
                                    const tgg00_StackDesc &StackDesc,
                                    tgg00_IndexCounter    &IndexCounter)
{
    Log_InvDescMap InvDescMap;
    
    InvDescMap.Assign (
        &((*StackDesc.mst_addr())[StackDesc.mmult_pos-1]),
        StackDesc.mmult_cnt, 
        true); // LookForUniqueEntries
    
    Log_ActionDropFile BeforeImage (InvFileId.fileName_gg00());
    
    Log_ActionCreateIndex AfterImage (
        PrimFileId.fileName_gg00(),
        InvFileId.fileName_gg00(),
        &InvDescMap,
        PrimFileId.fileTfn_gg00(),
        PrimFileId.fileTfnNo_gg00()[0]);

    BeforeImage.WriteBeforeImage (TransContext);
    
    if (e_ok != TransContext.trError_gg00) return;
    
    AfterImage.ExecuteAction (TransContext, &PrimFileId, &InvFileId, &IndexCounter);
    
    if (e_ok != TransContext.trError_gg00) return;
    
    AfterImage.WriteAfterImage (TransContext);
}

//----------------------------------------------------------------------------

externPascal void kb641CreateIndexFile (tgg00_TransContext &trans,
                                        tgg00_FileId       &tableFileId,
                                        tgg00_FileId       &indexFileId,
                                        const SAPDB_Int2    indexId )
{

    Log_ActionDropFile undoAction ( indexFileId.fileName_gg00() );

    undoAction.WriteBeforeImage (trans);
    if (e_ok != trans.trError_gg00)
        return;

    Log_ActionCreateIndexFile redoAction ( tableFileId.fileName_gg00(),
                                           indexFileId.fileName_gg00(),
                                           indexId );

    redoAction.ExecuteAction (trans, &tableFileId, &indexFileId);
    if (e_ok != trans.trError_gg00)
    {
        redoAction.WriteToTrace("kb641CreateIndexFile: ExecuteAction() failed");
        return;
    }

    redoAction.WriteAfterImage (trans);
}

//----------------------------------------------------------------------------

externPascal void kb641CreateTable (
    tgg00_TransContext &TransContext,
    tgg00_FileId       &FileId,
    tgg00_ExtendedFiletypeSet  VAR_ARRAY_REF  eFiletypeSet,
    tsp00_Int2          StrColCnt,
    tsp00_Int2          VarColCntOff)
{
    Log_ActionDropFile    BeforeImage (FileId.fileName_gg00());
    
    FileDir_FileNo tableFileNo( FileId.fileTabId_gg00() );
 
    Log_ActionCreateTable AfterImage (
        tableFileNo,
        FileId.fileVersion_gg00(),
        FileId.fileType_gg00(),
        eFiletypeSet,
        StrColCnt);
    
    BeforeImage.WriteBeforeImage (TransContext);
    
    if (e_ok != TransContext.trError_gg00) return;
    
    AfterImage.ExecuteAction (TransContext, &FileId);
    
    if (e_ok != TransContext.trError_gg00) return;
    
    AfterImage.WriteAfterImage (TransContext);
} 

//----------------------------------------------------------------------------

externPascal void kb641CreateShortColumnFile (tgg00_TransContext &TransContext,
                                              tgg00_FileId       &shortColId,
                                              tgg00_Surrogate    &tableSurr)
{
    Log_ActionDropFile    BeforeImage (shortColId.fileName_gg00());

    FileDir_FileNo shortColFileNo (shortColId.fileTabId_gg00());
    FileDir_FileNo tableFileNo (tableSurr);

    Log_ActionCreateShortColumnFile AfterImage (tableFileNo, shortColFileNo);

    BeforeImage.WriteBeforeImage (TransContext);

    if (e_ok != TransContext.trError_gg00) return;

    AfterImage.ExecuteAction (TransContext, &tableFileNo, &shortColFileNo);

    if (e_ok != TransContext.trError_gg00) return;

    AfterImage.WriteAfterImage (TransContext);
}

//----------------------------------------------------------------------------

externPascal void kb641MarkFileAsDeleted (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    tgg00_FileId          VAR_VALUE_REF  fileid)
{
    Log_ActionFile action (
        trans, Log_MarkFileAsDeleted, fileid.fileName_gg00());

    if (e_ok != trans.trError_gg00)
    {
        SAPDBErr_MessageList errlist;
        action.AppendContextInfo(__CONTEXT__,errlist);
        RTE_Message(errlist);
        return;
    }

    action.SetFileVersion (fileid.fileVersion_gg00());

    action.WriteBeforeImage(trans);

    if (e_ok != trans.trError_gg00)
    {
        SAPDBErr_MessageList errlist;
        action.AppendContextInfo(__CONTEXT__,errlist);
        RTE_Message(errlist);
        return;
    }

    action.Execute(trans);

    if (e_ok != trans.trError_gg00)
    {
        SAPDBErr_MessageList errlist;
        action.AppendContextInfo(__CONTEXT__,errlist);
        RTE_Message(errlist);
        return;
    }

    action.WriteAfterImage(trans);

    if (e_ok != trans.trError_gg00)
    {
        SAPDBErr_MessageList errlist;
        action.AppendContextInfo(__CONTEXT__,errlist);
        RTE_Message(errlist);
        return;
    }
}
//----------------------------------------------------------------------------

externPascal void kb641RenameLobFile (
    tgg00_TransContext    VAR_VALUE_REF  TransContext,
    tgg00_Surrogate       VAR_ARRAY_REF  TableId,
    tgg00_FileId          VAR_VALUE_REF  OldFileId,
    tgg00_FileId          VAR_VALUE_REF  NewFileId)
{
    Log_ActionRenameFile BeforeImage (TableId, NewFileId.fileName_gg00(), OldFileId.fileName_gg00(), OldFileId.fileVersion_gg00());
    
    Log_ActionRenameFile AfterImage  (TableId, OldFileId.fileName_gg00(), NewFileId.fileName_gg00(), NewFileId.fileVersion_gg00());
    
    BeforeImage.WriteBeforeImage (TransContext);
    
    if (e_ok != TransContext.trError_gg00) return;
    
    AfterImage.ExecuteAction (TransContext, &OldFileId, &NewFileId);
    
    if (e_ok != TransContext.trError_gg00) return;
    
    AfterImage.WriteAfterImage (TransContext);
}

//----------------------------------------------------------------------------

externPascal void kb641SuccFileVersion (tgg00_TransContext      &TransContext,
                                        tgg00_FileId            &FileId,
                                        const tgg91_FileVersion &NewFileVersion,
                                        tsp00_Int4               OldLongCnt,
                                        tsp00_Int4               NewLongCnt)
{
    Log_ActionSuccFileVersion
        BeforeImage (FileId.fileName_gg00(), FileId.fileVersion_gg00(), OldLongCnt);
    
    Log_ActionSuccFileVersion
        AfterImage (FileId.fileName_gg00(), NewFileVersion, NewLongCnt);
    
    BeforeImage.WriteBeforeImage (TransContext);
    
    if (e_ok != TransContext.trError_gg00) return;
    
    AfterImage.ExecuteAction (TransContext, &FileId);
    
    if (e_ok != TransContext.trError_gg00) return;
    
    AfterImage.WriteAfterImage (TransContext);
}

//----------------------------------------------------------------------------

externPascal void kb641UpdFileStatistic (tgg00_TransContext &TransContext,
                                         tgg00_FileId       &FileId,
                                         tsp00_Int4          OldLeafNodes)
{
    Log_ActionUpdFileStatistic BeforeImage (FileId.fileName_gg00(), OldLeafNodes);
    
    Log_ActionUpdFileStatistic AfterImage  (FileId.fileName_gg00(), FileId.fileLeafNodes_gg00());
    
    BeforeImage.WriteBeforeImage (TransContext);
    
    if (e_ok != TransContext.trError_gg00) return;
    
    AfterImage.ExecuteAction (TransContext, &FileId);
    
    if (e_ok != TransContext.trError_gg00) return;
    
    AfterImage.WriteAfterImage (TransContext);
}

/*===========================================================================*
*  END OF CODE                                                               *
*============================================================================*/
