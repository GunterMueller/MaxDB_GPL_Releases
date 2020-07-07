/*!**************************************************************************

    module      : vkb611.cpp

    -------------------------------------------------------------------------

    author      : JuergenA
    responsible : UweH

    special area: Logging
    description : KB_InsDelUpd_interface

    last changed: 2001-04-12

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

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "Container/Container_Vector.hpp"
#include "Logging/Log_CommandInsDelUpd.hpp"
#include "Logging/Log_InvDescMap.hpp"
#include "Logging/Log_ActionCreateDropFile.hpp"
#include "Logging/Log_ActionCreateIndexFile.hpp"
#include "Logging/Log_ActionUnqualifiedDelete.hpp"
#include "SQLManager/Catalog/Catalog_Cache.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "Transaction/Trans_Context.hpp"
#include "gsp00.h"
#include "ggg00.h"
#include "heo52.h"
#include "heo69.h"
#include "hgg01_3.h"
#include "hgg04.h"
#include "hak01.h"
#include "hkb62.h"
#include "hkb64.h"
#include "hkb720.h"
#include "hbd73.h"
#include "hbd01.h"
#include "hbd01_1.h" // b01niltree_id
#include "hsp41.h"

#include "hkb611.h" // check of PASCAL header vkb611

#include "hkb641.h"
#include "FileDirectory/FileDir_Table.hpp"
#include "FileDirectory/FileDir_IDirectory.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "Logging/Log_Types.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "SAPDBCommon/SAPDB_AutoPtr.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*==========================================================================*
* LOCAL FUNCTIONS                                                           *
*===========================================================================*/

/// Used to temporarily store information on index files during
/// kb611delete_all_records.
struct IndexDescription
{
    tgg00_FileId indexId;
    SAPDB_Int    indexNo;
    IndexDescription (
        const tgg00_FileId& indexId,
        const SAPDB_Int     indexNo)
    {
        this->indexId = indexId;
        this->indexNo = indexNo;
    }
};


/*==========================================================================*
* PASCAL INTERFACE of Log_DeleteCmd                                         *
*===========================================================================*/

externPascal void kb611del_AllocateClass (Log_DeleteCmd         *&pDeleteCmd,
                                          tgg00_TransContext     &TransContext,
                                          const tgg00_StackDesc  &StackDesc)
{
    if (e_ok != TransContext.trError_gg00) return;
    
    SAPDBMem_IRawAllocator& Allocator =
        *(REINTERPRET_CAST (SAPDBMem_IRawAllocator*, TransContext.trAllocator_gg00));
    
    pDeleteCmd = new (Allocator) Log_DeleteCmd;
    
    if (NULL == pDeleteCmd)
    {
        TransContext.trError_gg00 = e_no_more_memory;
        return;
    }
    
    pDeleteCmd->AssignInvDescMap (StackDesc);
}

//----------------------------------------------------------------------------

externPascal void kb611del_Init (Log_DeleteCmd      *pDeleteCmd,
                                 const tgg00_FileId &PrimFileId,
                                 const tgg00_Rec    &OldRecord,
                                 pasbool            MarkDeleted)
{
    pDeleteCmd->Init (PrimFileId, &OldRecord, (0 != MarkDeleted));
}

//----------------------------------------------------------------------------

externPascal void kb611del_PostponedExecution (const Log_DeleteCmd *pDeleteCmd,
                                               tgg00_TransContext  &TransContext,
                                               tgg00_FileId        &PrimFileId)
{
    pDeleteCmd->PostponedExecution (TransContext, PrimFileId);
}

//----------------------------------------------------------------------------

externPascal void kb611del_ReleaseClass (Log_DeleteCmd          *&pDeleteCmd,
                                         SAPDBMem_IRawAllocator  *pAllocator)
{ 
    if ( NULL == pDeleteCmd ) return;
    
    destroy (pDeleteCmd, *pAllocator);
    
    pDeleteCmd = NULL;
}

//----------------------------------------------------------------------------

externPascal void kb611del_WriteAfterImage (const Log_DeleteCmd *pDeleteCmd,
                                            tgg00_TransContext  &TransContext)
{
    pDeleteCmd->WriteAfterImage (TransContext);
}

//----------------------------------------------------------------------------

externPascal void kb611del_WriteBeforeImage (const Log_DeleteCmd *pDeleteCmd,
                                             tgg00_ObjTransInfo  &TransInfo,
                                             tgg00_TransContext  &TransContext)
{
    pDeleteCmd->WriteBeforeImage (TransContext, TransInfo);
}


/*==========================================================================*
* PASCAL INTERFACE index handling                                           *
*===========================================================================*/

externPascal void kb611IndexGetKey (
    tgg00_TransContext    VAR_VALUE_REF  TransContext,
    tgg00_StEntryAddr                    pStackList,
    tsp00_Int2                           Entries,
    const tgg00_Rec       VAR_VALUE_REF  Rec,
    tsp00_Uint1           VAR_VALUE_REF  InvDescId,
    tgg00_Lkey            VAR_VALUE_REF  SecondaryKey,
    pasbool               VAR_VALUE_REF  allColumnsAreNull) // PTS 1121337 UH 2003-03-27
{
    Log_InvDescMap InvDescMap;
    
    InvDescMap.Assign (
        pStackList, abs (Entries),
        false); // do not LookForUniqueEntries

    bool aux = false;
    InvDescMap.GetFirstKey (TransContext, &Rec, InvDescId, SecondaryKey, aux); // PTS 1121337 UH 2003-03-27
    allColumnsAreNull = aux;
}

//----------------------------------------------------------------------------

externPascal void kb611delete_all_records (tgg00_MessBlock VAR_VALUE_REF m)
{
    typedef Container_Vector<IndexDescription> IndexDescList;

    const SAPDB_Bool c_with_stringcol = true;
    const SAPDB_Bool c_put_strat      = true;
    const SAPDB_Bool c_strat_cnt      = true;

    tgg00_TransContext     &trans       = *m.mb_trns();
    SAPDBMem_IRawAllocator &allocator   = *(reinterpret_cast<SAPDBMem_IRawAllocator*>(trans.trAllocator_gg00));
    tgg00_BasisError       &trError     = trans.trError_gg00;
    tgg00_QualBuf          &mb_qual     = *m.mb_qual();
    tgg00_FileId           &tableFileId = mb_qual.mtree();
    IndexDescList           indexList (allocator);
    SAPDB_Int4              row_count = 0;
    SAPDB_Int4              dummy_count;
    SAPDB_Int4              start_sec;
    SAPDB_Int4              start_microsec;
    SAPDB_Int4              start_phys_ios;
    SAPDB_Int4              start_suspends;
    SAPDB_Int4              start_waits;
    SAPDB_Int4              old_treeleafnodes;
    tgg00_RefInfoIndex      arr_index;
    tgg00_SelectFieldsParam sel;
    SAPDB_Bool              hasShortColFile = false;
    tsp00_NumError          res;

    Msg_List& errorList = SQLMan_Context::GetContext()->GetErrorList();

    if ( a01diag_monitor_on && a01diag_analyze_on )
    {
        vclock (&start_sec, &start_microsec);
        vmonitor (trans.trTaskId_gg00, start_phys_ios,
                  start_suspends, start_waits);
    }
    else 
    {
        start_sec      = 1;
        start_microsec = 0;
        start_phys_ios = 0;
        start_suspends = 0;
        start_waits    = 0;
    }

    trans.trRteCommPtr_gg00->virt_reads = -MAX_INT2_SP00;

    arr_index.becomes (itable_scan);
    sel.sfp_rows_read() = 0;
    sel.sfp_rows_qual() = 0;

    if ( e_ok == trError )
        b01vstate_fileversion (trans, tableFileId);

    if ( trError == e_ok )
        Catalog_Cache::InvalidateTable (trans.trTaskId_gg00, tableFileId.fileTabId_gg00());

    if ( trError == e_ok )
        b01treeleafnodes (trans, tableFileId, m_fetch, old_treeleafnodes);

    FileDir_IDirectory& fdir =
        FileDir_IDirectory::SessionInstance (trans);
    FileDir_FileNo tableFileNo (tableFileId.fileTabId_gg00());
    FileDir_Table tableFile;

    if (FileDir_Okay !=
        fdir.GetTableFile (
            trans.trTaskId_gg00, tableFileNo, tableFile, errorList))
        trans.trError_gg00 = e_file_not_found;

    if ( e_ok == trError 
         &&
         mm_trunc != m.mb_type2() )
    {
        if (tableFile.AreCountersInitialized())
        {        
            if (tableFile.GetEntryCount(trans.trTaskId_gg00) < SAPDB_MAX_INT4)
                row_count = static_cast<SAPDB_Int4>(
                    tableFile.GetEntryCount(trans.trTaskId_gg00));
            else
                row_count = SAPDB_MAX_INT4;
        }
        else
            // file directory counters are not yet initialised, use fallback
            bd01CalculateRecordsAndLeaves (trans, tableFileId,
                                           dummy_count, row_count);
    }

    if ( e_ok == trError 
         ||
         mb_qual.mmult_cnt() > 0 )
    {
        // save index information
        SAPDB_Int    col;
        SAPDB_Int    indexCount;
        SAPDB_Int    last_col   = 0;
        SAPDB_Int    indexId;
        tgg00_FileId indexFileId;
        for ( col = mb_qual.mmult_pos(), indexCount = 1;
              e_ok == trError
              &&
              col < (mb_qual.mmult_pos() + mb_qual.mmult_cnt());
              col = last_col + 1, ++indexCount )
        {
            indexId = (*m.mb_st())[col-1].ecol_tab() [0];
            g04index_tree_build (tableFileId, indexFileId, indexId);
            indexFileId.fileBdUse_gg00().clear();
            k62last_multi_inv_st (mb_qual.mstack_desc(), col, last_col);

            if ( ! indexList.Reserve (indexCount) )
            {
                trError = e_no_more_memory;
                return;
            }
            indexList.InsertEnd ( IndexDescription(indexFileId, indexId) );
        }
    }

    hasShortColFile = tableFile.HasShortColumnFile();

    //
    // switch table file root
    if ( e_ok == trError )
    {
        // rename old tree and mark it as deleted
        FileDir_FileNo movedFileNo = fdir.GetNewFileNo();

        SAPDB_AutoPtr<Log_ActionUnqualifiedDelete> logAction(
            Trans_Context::GetContext( trans ).Allocator(),
            Log_ActionUnqualifiedDelete::CreateObject(
                trans,
                tableFileNo,
                movedFileNo));

        if ( logAction.GetPtr() == 0 )
        {
            trans.trError_gg00 = e_no_more_memory;
        }

        if (e_ok == trans.trError_gg00)
        {
            logAction->WriteBeforeImage (trans);
        }

        if (e_ok == trans.trError_gg00)
        {
            logAction->Execute (trans);
            tableFileId.fileRootCheck_gg00() = NIL_PAGE_NO_GG00;
            tableFileId.fileRoot_gg00()      = tableFile.GetRootPage();
        }

        if ( e_ok == trError )
            logAction->WriteAfterImage (trans);
    }


    if ( e_ok == trError )
    {
        tableFileId.fileLeafNodes_gg00() = old_treeleafnodes;
        b01treeleafnodes (trans, tableFileId, m_update, old_treeleafnodes);
    }

    if ( e_ok == trError && indexList.GetSize() > 0 )
    {
        // recreate empty index files
        tgg00_FileId            indexFileId;
        IndexDescList::Iterator indexDescRef;

        for ( indexDescRef = indexList.Begin(); 
              indexDescRef != indexList.End();
              ++indexDescRef )
        {            
            indexFileId                 = (*indexDescRef).indexId;
            indexFileId.fileRoot_gg00() = NIL_PAGE_NO_GG00;
            indexFileId.fileBdUse_gg00().clear();

            FileDir_FileNo newIndexFileNo =
                FileDir_IDirectory::SessionInstance (trans).GetNewFileNo();

            SAPDB_MemCopyNoCheck(
                &indexFileId.fileTabId_gg00(),
                &newIndexFileNo,
                sizeof(FileDir_FileNo));

            Log_ActionDropFile undoAction ( indexFileId.fileName_gg00() );
            
            undoAction.WriteBeforeImage (trans);
            if (e_ok != trans.trError_gg00)
                break;
            
            Log_ActionCreateIndexFile redoAction ( tableFileId.fileName_gg00(),
                                                   indexFileId.fileName_gg00(),
                                                   (*indexDescRef).indexNo );

            redoAction.ExecuteAction (trans, &tableFileId, &indexFileId);
            if (e_ok != trans.trError_gg00)
            {
                redoAction.WriteToTrace("kb611delete_all_records: ExecuteAction() failed");
                break;
            }
            
            redoAction.WriteAfterImage (trans);
        }
    }
    if ( e_ok == trError && hasShortColFile )
    {
        // recreate empty short column file
        k64short_col_file_create (trans, tableFileId);
    }

    if ( e_ok == trError  )
    {
        m.mb_type().becomes (m_return_result);
        m.mb_type2().becomes (mm_nil);
        m.mb_struct().becomes (mbs_result);
        m.mb_qual_len()        = MB_PART1_HEAD_MXGG00 + sizeof(tsp00_ResNum);
        mb_qual.mr_resnum().rawAssign (g01glob.rescnt_unknown);
        s41p4int (mb_qual.mr_resnum(), 2, row_count, res);
        m.mb_data_len()        = 0;
    }
    else
    {
        m.mb_type().becomes (m_return_error);
        m.mb_type2().becomes (mm_nil);
        if (trError != e_wait_for_lock_release)
        {
            m.mb_qual_len() = 0;
            m.mb_data_len() = 0;
        }
    }
    // PTS 1001518 E.Z.
    if ( start_sec > 0 )
    {
        k720monitor (trans, sel, start_sec,
                     start_microsec, start_phys_ios, start_suspends,
                     start_waits, c_put_strat, arr_index, c_strat_cnt);
    }
}

/*==========================================================================*
* PASCAL INTERFACE of Log_InsertCmd                                         *
*===========================================================================*/

externPascal void kb611ins_AllocateClass (Log_InsertCmd         *&pInsertCmd,
                                          tgg00_TransContext     &TransContext,
                                          const tgg00_StackDesc  &StackDesc)
{
    if (e_ok != TransContext.trError_gg00) return;
    
    SAPDBMem_IRawAllocator& Allocator =
        *(REINTERPRET_CAST (SAPDBMem_IRawAllocator*, TransContext.trAllocator_gg00));
    
    pInsertCmd = new (Allocator) Log_InsertCmd;
    
    if (NULL == pInsertCmd)
    {
        TransContext.trError_gg00 = e_no_more_memory;
        return;
    }
    
    pInsertCmd->AssignInvDescMap (StackDesc);
}

//----------------------------------------------------------------------------

externPascal void kb611ins_AssignBeforeImage (Log_InsertCmd      *pInsertCmd,
                                              const tgg00_Rec    &BeforeImage)
{
    pInsertCmd->AssignBeforeImage(&BeforeImage);
}

//----------------------------------------------------------------------------

externPascal void kb611ins_Init (Log_InsertCmd      *pInsertCmd,
                                 const tgg00_FileId &PrimFileId,
                                 const tgg00_Rec    &NewRecord)
{
    pInsertCmd->Init (PrimFileId, &NewRecord);
}

//----------------------------------------------------------------------------

externPascal void kb611ins_ReleaseClass (Log_InsertCmd          *&pInsertCmd,
                                         SAPDBMem_IRawAllocator  *pAllocator)
{ 
    if ( NULL == pInsertCmd ) return;
    
    destroy (pInsertCmd, *pAllocator);
    
    pInsertCmd = NULL;
}

//----------------------------------------------------------------------------

externPascal void kb611ins_WriteAfterImage (const Log_InsertCmd *pInsertCmd,
                                            tgg00_TransContext  &TransContext)
{
    pInsertCmd->WriteAfterImage (TransContext);
}

//----------------------------------------------------------------------------

externPascal void kb611ins_WriteBeforeImage (const Log_InsertCmd *pInsertCmd,
                                             tgg00_ObjTransInfo  &TransHistoryInfo,
                                             tgg00_TransContext  &TransContext)
{
    pInsertCmd->WriteBeforeImage (TransContext, TransHistoryInfo);
}


/*==========================================================================*
* PASCAL INTERFACE of Log_InvHandling                                       *
*===========================================================================*/

externPascal void kb611inv_AddInv (const Log_InvHandling *pInvHandling,
                                   tgg00_TransContext    &TransContext)
{
    pInvHandling->AddInv (TransContext);
}

//----------------------------------------------------------------------------

externPascal void kb611inv_CheckUniqueIndex (const Log_InvHandling *pInvHandling,
                                             tgg00_TransContext    &TransContext)
{
    pInvHandling->CheckUniqueIndex (TransContext);
}

//----------------------------------------------------------------------------

externPascal pasbool kb611inv_ContainsLateUniqueCheck (const Log_InvHandling *pInvHandling)
{
    return ( pInvHandling->ContainsLateUniqueCheck() );
}

//----------------------------------------------------------------------------

externPascal pasbool kb611inv_ContainsUniqueIndex (const Log_InvHandling *pInvHandling)
{
    return ( pInvHandling->ContainsUniqueIndex() );
}

//----------------------------------------------------------------------------

externPascal void kb611inv_DelInv (const Log_InvHandling *pInvHandling,
                                   tgg00_TransContext    &TransContext)
{
    pInvHandling->DelInv (TransContext);
}

//----------------------------------------------------------------------------

externPascal pasbool kb611inv_ExecuteOutsideBd (Log_InvHandling *pInvHandling)
{
    return ( pInvHandling->ExecuteOutsideBd() );
}

//----------------------------------------------------------------------------

externPascal pasbool kb611inv_ExistsAnyIndex (const Log_InvHandling *pInvHandling)
{
    return ( pInvHandling->ExistsAnyIndex() );
}

//----------------------------------------------------------------------------

externPascal pasbool kb611inv_IsExecutionPostponed (const Log_InvHandling *pInvHandling)
{
    return ( pInvHandling->IsExecutionPostponed() );
}

//----------------------------------------------------------------------------

externPascal void kb611inv_LockAndCheckUniqueIndex (
    const Log_InvHandling*                 pInvHandling,
    tgg00_TransContext    VAR_VALUE_REF  TransContext,
    pasbool                              forInsert,
    tgg00_LockReqMode     VAR_VALUE_REF  GrantedMode)
{
    pInvHandling->LockAndCheckUniqueIndex (TransContext, (forInsert!=0?true:false), GrantedMode);
}

//----------------------------------------------------------------------------

externPascal void kb611inv_LockUniqueBeforeImageIndex (const Log_InvHandling *pInvHandling,
                                                       tgg00_TransContext    &TransContext,
                                                       tgg00_LockReqMode     &GrantedMode)
{
    const bool BeforeImageOnly = true;
    
    pInvHandling->LockUniqueIndex (TransContext, GrantedMode, BeforeImageOnly);
}

//----------------------------------------------------------------------------

externPascal void kb611inv_LockUniqueIndex (const Log_InvHandling *pInvHandling,
                                            tgg00_TransContext    &TransContext,
                                            tgg00_LockReqMode     &GrantedMode)
{
    pInvHandling->LockUniqueIndex (TransContext, GrantedMode);
}


/*==========================================================================*
* PASCAL INTERFACE of Log_UpdateCmd                                         *
*===========================================================================*/

externPascal void kb611upd_AllocateClass (Log_UpdateCmd         *&pUpdateCmd,
                                          tgg00_TransContext     &TransContext,
                                          const tgg00_StackDesc  &StackDesc)
{
    if (e_ok != TransContext.trError_gg00) return;
    
    SAPDBMem_IRawAllocator& Allocator =
        *(REINTERPRET_CAST (SAPDBMem_IRawAllocator*, TransContext.trAllocator_gg00));
    
    pUpdateCmd = new (Allocator) Log_UpdateCmd;
    
    if (NULL == pUpdateCmd)
    {
        TransContext.trError_gg00 = e_no_more_memory;
        return;
    }
    
    pUpdateCmd->AssignInvDescMap (StackDesc);
}

//----------------------------------------------------------------------------

externPascal void kb611upd_AllocateColumnMap (Log_UpdateCmd      *pUpdateCmd,
                                              tgg00_TransContext &TransContext, 
                                              tsp00_Int4          MaxEntries,
                                              pasbool             CheckCompoundIndexColumns)
{
    if (e_ok != TransContext.trError_gg00) return;
    
    SAPDBMem_IRawAllocator& Allocator =
        *(REINTERPRET_CAST (SAPDBMem_IRawAllocator*, TransContext.trAllocator_gg00));
    
    if (CheckCompoundIndexColumns)
    {
        pUpdateCmd->SetCheckCompoundIndexColumnsFlag();
    }

    if(!pUpdateCmd->AllocateColumnMap (Allocator, MaxEntries))
        TransContext.trError_gg00 = e_no_more_memory;
}

//----------------------------------------------------------------------------

externPascal void kb611upd_CreateNewRec (Log_UpdateCmd      *pUpdateCmd,
                                         tgg00_TransContext &TransContext,
                                         const tgg00_FileId &PrimFileId,
                                         const tgg00_Rec    &OldRecord,
                                         tgg00_Rec          &NewRecord)
{
    SAPDBMem_IRawAllocator& Allocator =
        *(REINTERPRET_CAST (SAPDBMem_IRawAllocator*, TransContext.trAllocator_gg00));
    
    pUpdateCmd->CreateNewRec
        (Allocator, PrimFileId, &OldRecord, &NewRecord, sizeof (NewRecord), TransContext.trError_gg00);
}

//----------------------------------------------------------------------------

externPascal pasbool kb611upd_ExistsEntry (Log_UpdateCmd          *pUpdateCmd,
                                           const tgg00_StackEntry &StackEntry)
{
    return ( pUpdateCmd->ExistsEntry (StackEntry) );
}

//----------------------------------------------------------------------------

externPascal void kb611upd_InitAfterImageWithExistingNewRec (Log_UpdateCmd      *pUpdateCmd,
                                                             const tgg00_FileId &PrimFileId,
                                                             const tgg00_Rec    &NewRecord)
{
    pUpdateCmd->InitAfterImageWithExistingNewRec (PrimFileId, &NewRecord);
}

//----------------------------------------------------------------------------

externPascal void kb611upd_InitBeforeImageForExistingNewRec (Log_UpdateCmd      *pUpdateCmd,
                                                             tgg00_TransContext &TransContext,
                                                             const tgg00_Rec    &OldRecord)
{
    SAPDBMem_IRawAllocator& Allocator =
        *(REINTERPRET_CAST (SAPDBMem_IRawAllocator*, TransContext.trAllocator_gg00));
    
    pUpdateCmd->InitBeforeImageForExistingNewRec (Allocator, &OldRecord, TransContext.trError_gg00);
}

//----------------------------------------------------------------------------

externPascal void kb611upd_InsertColumn (Log_UpdateCmd          *pUpdateCmd,
                                         const tgg00_StackEntry &StackEntry,
                                         const tsp00_BytePtr     pValue,
                                         tsp00_Int4              ValueLength,
                                         tgg00_BasisError       &Error)
{
    pUpdateCmd->InsertColumn (StackEntry,  pValue, ValueLength, Error);
}

//----------------------------------------------------------------------------

externPascal void kb611upd_PostponedExecution (const Log_UpdateCmd *pUpdateCmd,
                                               tgg00_TransContext  &TransContext,
                                               tgg00_FileId        &PrimFileId)
{
    pUpdateCmd->PostponedExecution (TransContext, PrimFileId);
}

//----------------------------------------------------------------------------

externPascal void kb611upd_ReleaseClass (Log_UpdateCmd          *&pUpdateCmd,
                                         SAPDBMem_IRawAllocator  *pAllocator)
{ 
    if ( NULL == pUpdateCmd ) return;
    
    destroy (pUpdateCmd, *pAllocator);
    
    pUpdateCmd = NULL;
}

//----------------------------------------------------------------------------

externPascal void kb611upd_WriteAfterImage (const Log_UpdateCmd *pUpdateCmd,
                                            tgg00_TransContext  &TransContext)
{
    pUpdateCmd->WriteAfterImage (TransContext);
}

//----------------------------------------------------------------------------


externPascal void kb611upd_WriteBeforeImage (const Log_UpdateCmd *pUpdateCmd,
                                             tgg00_ObjTransInfo  &TransInfo,
                                             tgg00_TransContext  &TransContext)
{
    pUpdateCmd->WriteBeforeImage (TransContext, TransInfo);
}

/*===========================================================================*
*  END OF CODE                                                               *
*============================================================================*/
