
/*!**************************************************************************

    module      : Log_CommandInsDelUpd.cpp

    -------------------------------------------------------------------------

    author      : JuergenA
    responsible : UweH

    special area: Logging
    description : defines classes to handle the insert, delete and update commands

    last changed: 2001-04-12

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
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "Logging/Log_CommandInsDelUpd.hpp"
#include "Logging/Log_ActionUpdateRec.hpp"
#include "Logging/Log_InvDescMap.hpp"

#include "ggg00.h"
#include "hbd02.h"

/*===========================================================================*
*  METHODS of Log_InvHandling                                            *
*============================================================================*/

void Log_InvHandling::AddInv (tgg00_TransContext &TransContext) const
{
    if (e_ok != TransContext.trError_gg00) return; 
    
    if ((NULL == this->m_pPrimFileId) || (NULL == this->m_pNewRec))
    {
        TransContext.trError_gg00 = e_invalid;
        return;
    }
    
    this->m_InvDescMap.AddInv (
        TransContext,
        false, // not IsUndoOrRedo
        false, // not redo
        false, // not in savepoint
        *this->m_pPrimFileId,
        this->m_pNewRec);
}

//---------------------------------------------------------------------------

void Log_InvHandling::CheckUniqueIndex (tgg00_TransContext &TransContext) const
{
    if (e_ok != TransContext.trError_gg00) return; 
    
    if ((NULL == this->m_pPrimFileId) || (NULL == this->m_pNewRec))
    {
        TransContext.trError_gg00 = e_invalid;
        return;
    }
    
    this->m_InvDescMap.CheckUniqueIndex (TransContext, *this->m_pPrimFileId, this->m_pNewRec);
}

//---------------------------------------------------------------------------

void Log_InvHandling::DelInv (tgg00_TransContext &TransContext) const
{
    if (e_ok != TransContext.trError_gg00) return; 
    
    if ((NULL == this->m_pPrimFileId) || (NULL == this->m_pOldRec))
    {
        TransContext.trError_gg00 = e_invalid;
        return;
    }
    
    this->m_InvDescMap.DelInv (
        TransContext,
        false, // not IsUndoOrRedo
        false, // not in savepoint
        false, // not garbage collection
        *this->m_pPrimFileId,
        this->m_pOldRec);
}

//---------------------------------------------------------------------------

void Log_InvHandling::LockAndCheckUniqueIndex (tgg00_TransContext  &TransContext,
                                               bool                 forInsert,
                                               tgg00_LockReqMode   &GrantedMode) const
{
    if (e_ok != TransContext.trError_gg00) return; 
    
    if ((NULL == this->m_pPrimFileId) || (NULL == this->m_pNewRec))
    {
        TransContext.trError_gg00 = e_invalid;
        return;
    }
    
    GrantedMode = tgg00_LockReqMode::fromConst (lckFree_egg00);

    if ( !this->m_InvDescMap.ContainsUniqueIndex() ) return;
    
    if (NULL != this->m_pOldRec)
    {
        this->m_InvDescMap.LockUniqueIndex(TransContext, *this->m_pPrimFileId, this->m_pOldRec, GrantedMode);
        
        if (e_ok != TransContext.trError_gg00) return; 
    }
    
    this->m_InvDescMap.LockAndCheckUniqueIndex (TransContext, *this->m_pPrimFileId, this->m_pNewRec, forInsert, GrantedMode);
}

//---------------------------------------------------------------------------

void Log_InvHandling::LockUniqueIndex (tgg00_TransContext  &TransContext,
                                       tgg00_LockReqMode   &GrantedMode,
                                       bool                 BeforeImageOnly) const
{
    if (e_ok != TransContext.trError_gg00) return; 
    
    if ((NULL == this->m_pPrimFileId)
        ||
        ((NULL == this->m_pOldRec) && (NULL == this->m_pNewRec))
        ||
        ((NULL == this->m_pOldRec) && BeforeImageOnly) )
    {
        TransContext.trError_gg00 = e_invalid;
        return;
    }
    
    GrantedMode = tgg00_LockReqMode::fromConst (lckFree_egg00);

    if ( !this->m_InvDescMap.ContainsUniqueIndex() ) return;
    
    if (NULL != this->m_pOldRec)
    {
        this->m_InvDescMap.LockUniqueIndex
            (TransContext, *this->m_pPrimFileId, this->m_pOldRec, GrantedMode);
        
        if (e_ok != TransContext.trError_gg00) return; 
    }
    
    if ( ! BeforeImageOnly && (NULL != this->m_pNewRec))
    {
        this->m_InvDescMap.LockUniqueIndex
            (TransContext, *this->m_pPrimFileId, this->m_pNewRec, GrantedMode);
    }
}


/*===========================================================================*
*  METHODS of Log_DeleteCmd                                                  *
*============================================================================*/

void Log_DeleteCmd::PostponedExecution (tgg00_TransContext &TransContext,
                                        tgg00_FileId       &PrimFileId) const
{
    const bool NotUndoRedo = false;
    
    if (e_ok != TransContext.trError_gg00) return;
    
    if ( NULL == this->GetOldRec() )
    {
        TransContext.trError_gg00 = e_invalid;
        return;
    }
    
    this->m_BeforeImage.WriteBeforeImage (TransContext);
    
    if (e_ok != TransContext.trError_gg00) return;
    
    this->GetInvDescMap()->DelInv ( TransContext,
                                    NotUndoRedo,
                                    false, // not in savepoint
                                    false, // not garbage collection
                                    PrimFileId,
                                    this->GetOldRec() );
    
    if (e_ok != TransContext.trError_gg00) return;
    
    b02del_record (TransContext, PrimFileId, this->GetOldRec()->recKey_gg00());
}   


/*===========================================================================*
*  METHODS of Log_UpdateCmd                                                  *
*============================================================================*/

void Log_UpdateCmd::CreateNewRec (SAPDBMem_IRawAllocator &RawAllocator,
                                  const tgg00_FileId     &PrimFileId,
                                  const tgg00_Rec        *pOldRecord,
                                  tgg00_Rec              *pNewRecord,
                                  SAPDB_Int4              NewRecSize,
                                  tgg00_BasisError       &Error)
{
    if (e_ok != Error) return;
    
    Log_InvHandling::InitBeforeImage (PrimFileId, pOldRecord);
    
    this->m_AfterImage.CreateNewRecAndInitAfterBeforeImage (
        PrimFileId.fileName_gg00(),
        Log_InvHandling::GetInvDescMap(),
        pOldRecord,
        pNewRecord,
        MAX_RECLEN_GG00,
        this->m_BeforeImage,
        Error);
    
    if (e_ok != Error) return;
    
    Log_InvHandling::InitAfterImage (PrimFileId, pNewRecord);
    
    bool IsOk = true;
    
    SAPDB_UInt4 InvDescLen = this->m_AfterImage.RemoveRedundantInvDescEntries
        (RawAllocator, Log_InvHandling::GetWriteAccessInvDescMap(), IsOk);
    this->m_BeforeImage.SetInvDescLen (InvDescLen);

    if (m_checkCompoundIndexColumns)
    {
        this->m_BeforeImage.AddCompoundIndexColumns (Log_InvHandling::GetWriteAccessInvDescMap(), *pOldRecord, IsOk);
    }

    if ( ! IsOk )
    {
        Error = e_no_more_memory;
        return;
    }
}

//---------------------------------------------------------------------------

void Log_UpdateCmd::PostponedExecution (tgg00_TransContext &TransContext,
                                        tgg00_FileId       &PrimFileId) const
{
    const bool NotUndoRedo = false;
    
    if (e_ok != TransContext.trError_gg00) return;
    
    if ((NULL == this->GetOldRec()) || (NULL == this->GetNewRec()))
    {
        TransContext.trError_gg00 = e_invalid;
        return;
    }
    
    this->m_BeforeImage.WriteBeforeImage (TransContext);
    
    if (e_ok != TransContext.trError_gg00) return;
    
    this->GetInvDescMap()->DelInv ( TransContext,
                                    NotUndoRedo,
                                    false, // not in savepoint
                                    false, // not garbage collection
                                    PrimFileId,
                                    this->GetOldRec() );
    
    if (e_ok != TransContext.trError_gg00) return;
    
    b02repl_record (TransContext, PrimFileId, *(this->GetNewRec()));
}   

/*===========================================================================*
*  END OF CODE                                                               *
*============================================================================*/

