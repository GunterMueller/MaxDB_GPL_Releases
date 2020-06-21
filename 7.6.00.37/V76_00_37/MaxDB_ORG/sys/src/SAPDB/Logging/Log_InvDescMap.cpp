
/*!**************************************************************************

    module      : Log_InvDescMap.cpp

    -------------------------------------------------------------------------

    author      : JuergenA
    responsible : UweH

    special area: Logging
    description : defines a class containing the index descriptions

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

#include "Logging/Log_InvDescMap.hpp"

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IncrementalBufferAllocator.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp" // Routine trace macros
#include "SAPDBCommon/SAPDB_RangeCode.hpp"
#include "Logging/Log_Types.hpp"
#include "Logging/Log_IRecordColumnMap.hpp"
#include "Logging/Log_ReadWriteActionImage.hpp"
#include "KernelCommon/Kernel_Trace.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "SAPDBCommon/Fields/SAPDBFields_FieldMap.hpp"
#include "SQLManager/DBProc/DBProc_Handler.hpp"
#include "ConsistentRead/CnsRead_CachedRow.hpp"
#include "ConsistentRead/CnsRead_Manager.hpp"

#include "FileDirectory/FileDir_Index.hpp"
#include "FileDirectory/FileDir_IDirectory.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "Messages/Msg_List.hpp"
#include "ConsistentRead/CnsRead_Manager.hpp"

#include "gsp00.h"
#include "ggg00.h"
#include "ggg01.h"
#include "ggg04.h"
#include "hbd03.h"
#include "hbd06.h"
#include "hgg04.h"
#include "hkb53.h"
#include "hkb62.h"
#include "hsp30.h"
#include "heo58.h"

/*===========================================================================*
*   LOCAL FUNCTIONS                                                          *
*============================================================================*/

static void DuplicateInvKeyError (tgg00_TransContext     &TransContext,
                                  const tgg00_FileId     &InvFileId,
                                  const tgg00_StackEntry &FirstStackEntry)
{
    SAPDBTRACE_ROUTINE_DEBUG ("DuplicateInvKeyError", LogAction_Trace, 5);
    Msg_List errorList;

    if (e_duplicate_invkey == TransContext.trError_gg00)
    {
        tgg04_Err        ErrorMsg;
        tgg00_BasisError DummyErr = e_ok;
        
        FileDir_FileNo indexFileNo (InvFileId.fileTabId_gg00());
        FileDir_FileNo tableFileNo;
        FileDir_Index index;

        FileDir_IDirectory::Instance (TransContext.trIndex_gg00).GetIndexFile(
            TransContext.trTaskId_gg00, indexFileNo, index, errorList);
        SAPDBERR_ASSERT_STATE (index.IsAssigned());        

        tableFileNo = index.GetParentFileNo();
        SAPDB_MemCopyNoCheck(
            &ErrorMsg.errtableid(),
            &tableFileNo,
            sizeof(tgg00_Surrogate) );
        ErrorMsg.errindexno()      = static_cast<SAPDB_UInt1>(index.GetIndexId());
        ErrorMsg.errfiller0()      = 0;
        ErrorMsg.errfiller1()      = 0;
        ErrorMsg.errfiller2()      = 0;
        
        TransContext.trError_gg00  = e_ok;

        b06put_errtxt (TransContext, TransContext.trTaskId_gg00,
            SURROGATE_MXGG00 + STACK_ENTRY_MXGG00 + 3,
            errtext_indexNo, e_duplicate_invkey, ErrorMsg.errt(), DummyErr);

        TransContext.trError_gg00 = e_duplicate_invkey;
    }
}

/*===========================================================================*
*   PRIVATE CLASSES                                                          *
*============================================================================*/

//===================================================================================
/*! class:       Log_InvEntryIter

    description: iterator to access an entry belonging to a specific description
*/
class Log_InvEntryIter: protected Log_InvUtilities
{

protected:
    /// the current entry to which the iterator points
    Log_InvEntryIndex     m_CurrEntry;
    /// the last read entry.
    Log_InvEntryIndex     m_LastEntry;
    /// the map to which this iterator belongs
    const Log_InvDescMap &m_InvDescMap;
    

    //--------------------------------------------------------------Log_InvEntryIter---
    /*! function:    ResetEntryIter
    */
    inline void ResetEntryIter ();

    //--------------------------------------------------------------Log_InvEntryIter---
    /*! function:    Log_InvEntryIter ctor
    */
    inline Log_InvEntryIter (const Log_InvDescMap &InvDescMap);
    
public :

    //--------------------------------------------------------------Log_InvEntryIter---
    /*! function:    Log_InvEntryIter ctor
    */
    inline Log_InvEntryIter (const Log_InvEntryIter &EntryIter);

    //--------------------------------------------------------------Log_InvEntryIter---
    /*! function:     HasMore
    
        return value: true, if a following entry of the index description exists
    */
    bool HasMore() const
    {
        return ( NilIndex() != m_LastEntry );
    }
    
    //--------------------------------------------------------------Log_InvEntryIter---
    /*! function:    Next
    */
    inline void Next ();
    
    //--------------------------------------------------------------Log_InvEntryIter---
    /*! function:     StackEntry
    
        return value: the stack entry specified by the iterator
    */
    const tgg00_StackEntry &StackEntry () const
    {  
        return ( m_InvDescMap.StackEntry(m_CurrEntry) );
    }

};
/*! endclass: Log_InvEntryIter */

//===================================================================================
/*! class:       Log_InvDescIter

    description: iterator to access the different index description
*/
class Log_InvDescIter: private Log_InvEntryIter
{

private:
    Log_InvEntryIndex  m_CurrDesc;
    Log_InvEntryIndex  m_NextDesc;
    Log_InvEntryIndex  m_PrevDesc;
    InvDescIterKind    m_IterKind;
    SAPDB_Int          m_UniqueEntriesHandled;
    SAPDB_Int          m_LateUniqueEntriesHandled;

    //--------------------------------------------------------------Log_InvDescIter---
    /*! function:    NextDescAndInitEntryIter
    
        description: sets the iterator to the next description and initializes the entry iterator
    */
    void NextDescAndInitEntryIter ();

    //--------------------------------------------------------------Log_InvDescIter---
    /*! function:    NextSpecificDesc
    
        description: skips to the next description according to the kind of the iterator
    */
    void NextSpecificDesc ();
    
public :

    //--------------------------------------------------------------Log_InvDescIter---
    /*! function:    Log_InvDescIter ctor
    
        description: initializes the iterator to the first specified inv desc of the map
        
        arguments:   IterKind    [in]
                     pInvDescMap [in] pointer to the InvDescMap
    */
    inline Log_InvDescIter (
        InvDescIterKind       IterKind,
        const Log_InvDescMap &InvDescMap);

    //--------------------------------------------------------------Log_InvDescIter---
    /*! function:     HasMore
    
        return value: true, if a following description exists
    */
    bool HasMore() const
    {
        return ( Log_InvEntryIter::HasMore() );
    }

    //--------------------------------------------------------------Log_InvDescIter---
    /*! function:    GetInvFileId
    
        description: builds the index file id
        
        arguments:   PrimFileId [in]  file id of the primary table
                     InvFileId  [out] file id of the index
    */
    void GetInvFileId (
        const tgg00_FileId &PrimFileId,
        tgg00_FileId       &InvFileId) const
    {
        BuildInvFileId (InvDescId(), PrimFileId, InvFileId);
    }
    
    //--------------------------------------------------------------Log_InvDescIter---
    /*! function:    GetNextKeyAndId
    
        description: builds the secondary key and sets the iterator to the next description

        arguments:   pRec          [in] pointer to the record which is used to build the key
                     pSecondaryKey [out]
                     allColumnsAreNull [out]
    */
    void GetNextKeyAndId (
        tgg00_TransContext &TransContext,
        const tgg00_Rec    *pRec,
        tgg00_Lkey         &SecondaryKey,
        bool               &allColumnsAreNull);

    
    //--------------------------------------------------------------Log_InvDescIter---
    /*! function:    GetNextAndCopyUsedEntries
    
        description: copies the entries to NewInvDescMap if they are contained in RecordColumnMap

        arguments:   RecordColumnMap [in]
                     pNewInvDescMap  [in out]
                     NewEntries      [in out]
    */
    void GetNextAndCopyUsedEntries (
        const Log_IRecordColumnMap &RecordColumnMap,
        tgg00_StackEntry           *pNewInvDescMap,
        SAPDB_Int                  &NewEntries);

    //--------------------------------------------------------------Log_InvDescIter---
    /*! function:     InvDescId
    
        return value: the id of the index description according to the last shipped key 
    */
    Log_InvDescId InvDescId () const
    {
        return ( m_InvDescMap.InvDescId(m_PrevDesc) );
    }
    
    //--------------------------------------------------------------Log_InvDescIter---
    /*! function:     FirstStackEntry
    
        return value: the first stack entry according to the last shipped key 
    */
    const tgg00_StackEntry &FirstStackEntry () const
    {
        return ( m_InvDescMap.StackEntry(m_PrevDesc) );
    }
    void StoreNotUpdatedIndexColumn ( Log_IRecordColumnMap &RecordColumnMap,
        const tgg00_Rec &Image);
};
/*! endclass: Log_InvDescIterator */


/*===========================================================================*
*  METHODS of Log_InvUtilities                                               *
*============================================================================*/

void Log_InvUtilities::BuildInvFileId (Log_InvDescId       InvDescId,
                                       const tgg00_FileId &PrimFileId,
                                       tgg00_FileId       &InvFileId) const
{  
    SAPDBTRACE_METHOD_DEBUG ("Log_InvUtilities::BuildInvFileId", LogAction_Trace, 5);
    g04index_tree_build( PrimFileId, InvFileId, InvDescId );
    InvFileId.fileBdUse_gg00().clear();
}


/*===========================================================================*
*  METHODS of Log_InvDescMap                                                  *
*============================================================================*/

Log_InvDescMap::~Log_InvDescMap()
{
    SAPDBTRACE_METHOD_DEBUG ("Log_InvDescMap::~Log_InvDescMap", LogAction_Trace, 5);
    if ((NULL != this->m_pAllocatedInvDescMap) && (this->m_AllocatedEntries > 0))
    {
        destroyarray (
            this->m_pAllocatedInvDescMap,
            this->m_AllocatedEntries,
            *(this->m_pRawAllocator));
    }
}

//----------------------------------------------------------------------------

void Log_InvDescMap::AddCompoundIndexColumnsToBeforeImage (
        Log_IRecordColumnMap &RecordColumnMap,
        const tgg00_Rec      &Image,
        bool                 &IsOk)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_InvDescMap::AddCompoundIndexColumns", LogAction_Trace, 5);
    KERNEL_TRACE_IS_OK (IsOk, LogAction_Trace, 5);
    
    if (this->m_ValidEntries < 1) return;
        
    Log_InvDescIter InvDescIter (EveryDesc, *this);
    while ( InvDescIter.HasMore() )
    {
        InvDescIter.StoreNotUpdatedIndexColumn (RecordColumnMap, Image); 
    }
}

//----------------------------------------------------------------------------

void Log_InvDescMap::AddInv (tgg00_TransContext &TransContext,
                             bool                IsUndo,
                             bool                IsRedo,
                             bool                inSavepoint,
                             const tgg00_FileId &PrimFileId,
                             const tgg00_Rec    *pRec)           const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_InvDescMap::AddInv", LogAction_Trace, 5);
    KERNEL_TRACE_BASIS_ERROR (TransContext.trError_gg00, LogAction_Trace, 5);
    
    if (e_ok != TransContext.trError_gg00) return;
    
    if (this->m_ValidEntries > 0)
    {    
        tgg00_FileId    InvFileId;
        tgg00_Lkey      SecondaryKey;
        Log_InvDescIter InvDescIter (EveryDesc, *this);
        bool            allColumnsAreNull; // PTS 1120192 UH 2003-01-21
        
        while ( InvDescIter.HasMore() 
                &&
                TransContext.trError_gg00 == e_ok )
        {
            InvDescIter.GetNextKeyAndId (TransContext, pRec, SecondaryKey, allColumnsAreNull); // PTS 1120192 UH 2003-01-21
            InvDescIter.GetInvFileId    (PrimFileId, InvFileId);
            
            if (e_ok != TransContext.trError_gg00) 
                break;

            if ( IsUndo && CnsRead_Manager::IsConsistentRead_Configurated())
            {
                b03del_inv (TransContext,
                    InvFileId, SecondaryKey, const_cast<tgg00_Lkey&>(pRec->recKey_gg00()), dirRemoveDeletedFlag_ebd00);
            }
            else
            {
                b03add_inv (TransContext,
                    InvFileId, SecondaryKey,const_cast<tgg00_Lkey&>(pRec->recKey_gg00()));
            }
            
            if ( IsUndo || IsRedo )
            {
                // PTS 1128514 UH 2004-03-19 changed error handling
                // In principle: The intention of the undo or redo action must be reached.
                if ( TransContext.trError_gg00 != e_ok )
                {
                    Kernel_VTrace() << __FILE__ << __LINE__ << NewLine << "IGNORED ERROR is " << TransContext.trError_gg00;
                    Kernel_TraceBuffer(&PrimFileId.fileName_gg00(), sizeof(PrimFileId.fileName_gg00()),
                                       "PrimFilename", sizeof(PrimFileId.fileName_gg00()));
                    Kernel_TraceBuffer(&InvFileId.fileName_gg00(), sizeof(InvFileId.fileName_gg00()),
                                       "InvFileName", sizeof(InvFileId.fileName_gg00()));
                    Kernel_TraceBuffer(&SecondaryKey.keyVal_gg00(),SecondaryKey.keyLen_gg00(),
                                       "SecKey", SecondaryKey.keyLen_gg00());
                    Kernel_TraceBuffer(&pRec->recKey_gg00().keyVal_gg00(),
                                       pRec->recKey_gg00().keyLen_gg00(),
                                       "PrimKey", pRec->recKey_gg00().keyLen_gg00());
                    TransContext.trError_gg00 = e_ok;
                }
            }
            else
            {
                if ( TransContext.trError_gg00 != e_ok )
                    break;

                if ( IsLateUniqueEntry (InvDescIter.FirstStackEntry()) )
                {
                    k62prepare_late_unique_check (TransContext, InvFileId, SecondaryKey);
                    
                    if (e_ok != TransContext.trError_gg00)
                        break;
                }
            }
        }
    }
}

//----------------------------------------------------------------------------

void Log_InvDescMap::Allocate (SAPDB_Int               Entries,
                               SAPDBMem_IRawAllocator *pRawAllocator,
                               bool                   &IsOk)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_InvDescMap::Allocate", LogAction_Trace, 5);
    this->m_AllocatedEntries = Entries;
    this->m_pRawAllocator    = pRawAllocator;
    
    if (NULL == this->m_pAllocatedInvDescMap)
    {
        newarray (this->m_pAllocatedInvDescMap, this->m_AllocatedEntries, *(this->m_pRawAllocator));
    }
     
    IsOk = (NULL != this->m_pAllocatedInvDescMap);
    
    this->m_pInvDescMap = this->m_pAllocatedInvDescMap;
}

//----------------------------------------------------------------------------

void Log_InvDescMap::Assign (const void *pExistingInvDescMap,
                             SAPDB_Int   Entries,
                             bool        LookForUniqueEntries)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_InvDescMap::Assign", LogAction_Trace, 5);
    if ((this->m_AllocatedEntries > 0) || (NULL != this->m_pInvDescMap)) return;
    
    this->m_pInvDescMap  = REINTERPRET_CAST (const tgg00_StackEntry*, pExistingInvDescMap);
    this->m_ValidEntries = Entries;
    
    if (LookForUniqueEntries) this->FindNumberOfUniqueEntries();
}

//----------------------------------------------------------------------------

void Log_InvDescMap::BuildStackDesc (tgg00_StackDesc &StackDesc) const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_InvDescMap::BuildStackDesc", LogAction_Trace, 5);
    tgg00_StackEntry *pEntry = CONST_CAST (tgg00_StackEntry*, this->m_pInvDescMap);

    StackDesc.mmult_pos     = 1;
    StackDesc.mmult_cnt     = this->m_ValidEntries;
    StackDesc.mfirst_free() = this->m_ValidEntries+1;
    StackDesc.mst_max()     = this->m_ValidEntries;
    StackDesc.mst_addr()    = REINTERPRET_CAST (tgg00_StackListPtr, pEntry);
}

//----------------------------------------------------------------------------

void Log_InvDescMap::CheckUniqueIndex (tgg00_TransContext  &TransContext,
                                       const tgg00_FileId  &PrimFileId,
                                       const tgg00_Rec     *pRec)        const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_InvDescMap::CheckUniqueIndex", LogAction_Trace, 5);
    KERNEL_TRACE_BASIS_ERROR (TransContext.trError_gg00, LogAction_Trace, 5);
    
    if (e_ok != TransContext.trError_gg00) return;
    
    if (this->m_UniqueEntries > this->m_LateUniqueEntries)
    {    
        tgg00_FileId    InvFileId;
        tgg00_Lkey      SecondaryKey;
        Log_InvDescIter InvDescIter (UniqueDescWithoutLateUnique, *this);
        bool            allColumnsAreNull; // PTS 1120192 UH 2003-01-21
        
        while ( InvDescIter.HasMore() )
        {
            InvDescIter.GetNextKeyAndId (TransContext, pRec, SecondaryKey, allColumnsAreNull); // PTS 1120192 UH 2003-01-21

            if ( SecondaryKey.keyVal_gg00() == 0  || allColumnsAreNull ) // PTS 1120192 UH 2003-01-21
                 continue;
            
            InvDescIter.GetInvFileId (PrimFileId, InvFileId);
            
            if (e_ok == TransContext.trError_gg00)
            {
                b03test_unique_inv (TransContext, InvFileId, SecondaryKey,
                    CONST_CAST (tgg00_Lkey&, pRec->recKey_gg00()) );
            }
            
            if (e_duplicate_invkey == TransContext.trError_gg00)
            {
                DuplicateInvKeyError (TransContext, InvFileId, InvDescIter.FirstStackEntry());
            }
            
            if (e_ok != TransContext.trError_gg00) return;
        }
    }
}

//----------------------------------------------------------------------------


void Log_InvDescMap::DelInv (tgg00_TransContext &TransContext,
                             bool                IsUndoOrRedo,
                             bool                inSavepoint,
                             bool                IsGarbageCollection,
                             const tgg00_FileId &PrimFileId,
                             const tgg00_Rec    *pRec)          const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_InvDescMap::DelInv", LogAction_Trace, 5);
    KERNEL_TRACE_BASIS_ERROR (TransContext.trError_gg00, LogAction_Trace, 5);
    
    if (e_ok != TransContext.trError_gg00) return;
    
    if (this->m_ValidEntries > 0)
    {    
        tgg00_FileId    InvFileId;
        tgg00_Lkey      SecondaryKey;
        Log_InvDescIter InvDescIter (EveryDesc, *this);
        bool            allColumnsAreNull; // PTS 1120192 UH 2003-01-21
        
        while ( InvDescIter.HasMore()
                &&
                e_ok == TransContext.trError_gg00 )
        {
            // PTS 1128514 UH 2004-03-19 changed error handling
            // In principle: The intention of the undo or redo action must be reached.
            InvDescIter.GetNextKeyAndId (TransContext, pRec, SecondaryKey, allColumnsAreNull); // PTS 1120192 UH 2003-01-21
            InvDescIter.GetInvFileId    (PrimFileId, InvFileId);
            
            if (e_ok != TransContext.trError_gg00)
                break;
            if (CnsRead_Manager::IsConsistentRead_Configurated())
            {
                tbd00_DelInvReason delInvCallReasion;
                if (!IsUndoOrRedo)
                {
                    if (IsGarbageCollection)
                    {
                        delInvCallReasion.becomes(dirGarbageCollection_ebd00);
                    }
                    else
                    {
                        delInvCallReasion.becomes(dirSetDeletedFlag_ebd00);
                    }
                    //b03del_inv (TransContext,
                    //    InvFileId, SecondaryKey, const_cast<tgg00_Lkey&>(pRec->recKey_gg00()), delInvCallReasion);
                }
                else
                {
 //                   tgg00_Lkey PrimaryKey;
 //                   CnsRead_Manager::BuildInvKey (pRec->recKey_gg00(), PrimaryKey, TransContext.trWriteTransId_gg00);
                    delInvCallReasion.becomes(dirDeletePrimKey_ebd00);
 //                   b03del_inv (TransContext, InvFileId, SecondaryKey, PrimaryKey, delInvCallReasion);
                }
                b03del_inv (TransContext,
                        InvFileId, SecondaryKey, const_cast<tgg00_Lkey&>(pRec->recKey_gg00()), delInvCallReasion);
 
            }
            else
            {
                b03del_inv (TransContext,
                    InvFileId, SecondaryKey, const_cast<tgg00_Lkey&>(pRec->recKey_gg00()), dirDeletePrimKey_ebd00);
            }
            
            if ( TransContext.trError_gg00 != e_ok )
            {
                if (IsGarbageCollection && 
                    ((e_file_not_found == TransContext.trError_gg00)  || 
                     (e_inv_not_found  == TransContext.trError_gg00)  ||
                     (e_key_not_found  == TransContext.trError_gg00)) // TODO review : ignore key_not_found, 
                                                                      // this is because inside one transaction indexes
                                                                      // are maintained directly, not only marked as deleted
                    )
                {
                    // ignore error, index may have been already dropped
                    TransContext.trError_gg00 = e_ok;
                }
                else
                {
                    if (IsUndoOrRedo)
                    {
                        Kernel_VTrace() << __FILE__ << __LINE__ << NewLine << "IGNORED ERROR is " << TransContext.trError_gg00;
                        Kernel_TraceBuffer(&SecondaryKey.keyVal_gg00(),SecondaryKey.keyLen_gg00(),
                            "SecKey", SecondaryKey.keyLen_gg00());
                        Kernel_TraceBuffer(&pRec->recKey_gg00().keyVal_gg00(),
                            pRec->recKey_gg00().keyLen_gg00(),
                            "PrimKey", pRec->recKey_gg00().keyLen_gg00());
                        TransContext.trError_gg00 = e_ok;
                    }
                }
            }
        }
    }
}

//----------------------------------------------------------------------------

void Log_InvDescMap::FindNumberOfUniqueEntries ()
{
    SAPDBTRACE_METHOD_DEBUG ("Log_InvDescMap::FindNumberOfUniqueEntries", LogAction_Trace, 5);
    this->m_LateUniqueEntries = 0;
    this->m_UniqueEntries     = 0;
    
    for (SAPDB_Int i = 0; i < this->m_ValidEntries; ++i)
    {
        if ( IsLateUniqueEntry (this->StackEntry(i)) ) ++this->m_LateUniqueEntries;
        
        if ( IsUniqueEntry     (this->StackEntry(i)) ) ++this->m_UniqueEntries;
    }
} 

//----------------------------------------------------------------------------

void Log_InvDescMap::GetFirstKey ( tgg00_TransContext &TransContext,
                                   const tgg00_Rec    *pRec,
                                   Log_InvDescId      &InvDescId,
                                   tgg00_Lkey         &SecondaryKey,
                                   bool               &allColumnsAreNull ) // PTS 1121337 UH 2003-03-27
{
    SAPDBTRACE_METHOD_DEBUG ("Log_InvDescMap::GetFirstKey", LogAction_Trace, 5);
    
    Log_InvDescIter InvDescIter (EveryDesc, *this);
    // bool            allColumnsAreNull; // PTS 1120192 UH 2003-01-21
    
    SecondaryKey.keyLen_gg00() = 0;
    
    if ( InvDescIter.HasMore() )
    {
        InvDescIter.GetNextKeyAndId (TransContext, pRec, SecondaryKey, allColumnsAreNull); // PTS 1120192 UH 2003-01-21
        
        InvDescId = InvDescIter.InvDescId();
    }
}

//----------------------------------------------------------------------------

void Log_InvDescMap::LockAndCheckUniqueIndex (tgg00_TransContext  &TransContext,
                                              const tgg00_FileId  &PrimFileId,
                                              const tgg00_Rec     *pRec,
                                              bool                 forInsert,
                                              tgg00_LockReqMode   &GrantedMode) const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_InvDescMap::LockAndCheckUniqueIndex", LogAction_Trace, 5);
    KERNEL_TRACE_BASIS_ERROR (TransContext.trError_gg00, LogAction_Trace, 5);
    
    if (e_ok != TransContext.trError_gg00) return;
    
    GrantedMode = tgg00_LockReqMode::fromConst (lckFree_egg00);

    if (this->m_UniqueEntries > 0)
    {    
        tgg00_FileId    InvFileId;
        tgg00_Lkey      SecondaryKey;
        Log_InvDescIter InvDescIter (UniqueDesc, *this);
        bool            allColumnsAreNull; // PTS 1120192 UH 2003-01-21
        
        while ( InvDescIter.HasMore() )
        {
            InvDescIter.GetNextKeyAndId (TransContext, pRec, SecondaryKey, allColumnsAreNull); // PTS 1120192 UH 2003-01-21
            
            if (e_ok != TransContext.trError_gg00) return;
            
            InvDescIter.GetInvFileId (PrimFileId, InvFileId);
            
            if ((lckSysExcl_egg00 != GrantedMode) && (lckTabExcl_egg00 != GrantedMode))
            {
                k53eot_excl_lock (TransContext, InvFileId, SecondaryKey, forInsert, lckRowExcl_egg00, GrantedMode);
            }
            
            if (e_ok != TransContext.trError_gg00) return;
            
            if ( SecondaryKey.keyLen_gg00() > 0
                 &&
                 IsUniqueEntryWithoutLateUnique (InvDescIter.FirstStackEntry())
                 &&
                 ! allColumnsAreNull ) // PTS 1120192 UH 2003-01-21
            {
                b03test_unique_inv (TransContext, InvFileId, SecondaryKey,
                    CONST_CAST (tgg00_Lkey&, pRec->recKey_gg00()) );
                
                if (e_duplicate_invkey == TransContext.trError_gg00)
                {
                    DuplicateInvKeyError (TransContext, InvFileId, InvDescIter.FirstStackEntry());
                }
                
                if (e_ok != TransContext.trError_gg00) return;
            }
        }
    }
}

//----------------------------------------------------------------------------

void Log_InvDescMap::LockUniqueIndex (tgg00_TransContext  &TransContext,
                                      const tgg00_FileId  &PrimFileId,
                                      const tgg00_Rec     *pRec,
                                      tgg00_LockReqMode   &GrantedMode) const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_InvDescMap::LockUniqueIndex", LogAction_Trace, 5);
    KERNEL_TRACE_BASIS_ERROR (TransContext.trError_gg00, LogAction_Trace, 5);
    
    if (e_ok != TransContext.trError_gg00) return;
    
    GrantedMode = tgg00_LockReqMode::fromConst (lckFree_egg00);
    
    if (this->m_UniqueEntries > 0)
    {    
        tgg00_FileId    InvFileId;
        tgg00_Lkey      SecondaryKey;
        Log_InvDescIter InvDescIter (UniqueDesc, *this);
        bool            allColumnsAreNull; // PTS 1120192 UH 2003-01-21
        
        while ( InvDescIter.HasMore() )
        {
            InvDescIter.GetNextKeyAndId (TransContext, pRec, SecondaryKey, allColumnsAreNull); // PTS 1120192 UH 2003-01-21
            
            if (e_ok != TransContext.trError_gg00) return;
 
            InvDescIter.GetInvFileId (PrimFileId, InvFileId);
            
            k53eot_excl_lock (TransContext, InvFileId, SecondaryKey, false, lckRowExcl_egg00, GrantedMode);
            
            if (e_ok != TransContext.trError_gg00) return;
            
            if ((lckSysExcl_egg00 == GrantedMode) || (lckTabExcl_egg00 == GrantedMode)) return;
        }
    }
} 
//----------------------------------------------------------------------------

bool Log_InvDescMap::ReadImagePersistent (Log_ActionImageReader &ImageReader,
                                          SAPDB_UInt4            PersistentLen,
                                          SAPDBErr_MessageList  &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_InvDescMap::LockUniqueIndex", LogAction_Trace, 5);
    
    if (! ImageReader.PersistentRead (REINTERPRET_CAST (SAPDB_Byte*&, this->m_pAllocatedInvDescMap),
        PersistentLen, this->m_pRawAllocator, errlist))
        return false;
    
    this->m_ValidEntries = PersistentLen / sizeof (tgg00_StackEntry);
    this->m_pInvDescMap  = this->m_pAllocatedInvDescMap;
    
    if (NULL == this->m_pRawAllocator)
    {
        this->m_pAllocatedInvDescMap = NULL;
    }
    else
    {
        this->m_AllocatedEntries = this->m_ValidEntries;
    }
    return true;
}

//----------------------------------------------------------------------------

void Log_InvDescMap::RemoveRedundantInvDescEntries (SAPDBMem_IRawAllocator     &RawAllocator,
                                                    const Log_IRecordColumnMap &RecordColumnMap,
                                                    bool                       &IsOk)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_InvDescMap::RemoveRedundantInvDescEntries", LogAction_Trace, 5);
    KERNEL_TRACE_IS_OK (IsOk, LogAction_Trace, 5);
    IsOk = true;
    
    const tgg00_StackEntry *OldInvDescMap   = this->m_pInvDescMap;
    SAPDB_Int               NewValidEntries = 0;
    
    if ((this->m_ValidEntries < 1) || (NULL != this->m_pAllocatedInvDescMap)) return;
    
    this->Allocate (this->m_ValidEntries, &RawAllocator, IsOk);
    
    this->m_pInvDescMap = OldInvDescMap;
    
    if ( ! IsOk ) return;
    
    Log_InvDescIter InvDescIter (EveryDesc, *this);
    
    while ( InvDescIter.HasMore() )
    {
        InvDescIter.GetNextAndCopyUsedEntries
            (RecordColumnMap, this->m_pAllocatedInvDescMap, NewValidEntries);
    }
    
    this->m_pInvDescMap  = this->m_pAllocatedInvDescMap;
    this->m_ValidEntries = NewValidEntries;
    
    this->FindNumberOfUniqueEntries();
}

//----------------------------------------------------------------------------

bool Log_InvDescMap::WriteImagePersistent (Log_ActionImageWriter &ImageWriter,
                                           SAPDBErr_MessageList  &errlist)       const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_InvDescMap::WriteImagePersistent", LogAction_Trace, 5);
    return ImageWriter.PersistentWrite (m_pInvDescMap, GetPersistentLength(), errlist);
}
/*
bool IsNull (const tgg00_StackEntry &stackEntry,
             const tgg00_Rec        *pRecord
             )
{
    // 
    if ( st_object_key == stackEntry.etype )
        return false;

    tgg00_VarColPosList varcolPos;
    SAPDB_Int           columnPosition;
    SAPDB_Int           columnLength;
    
    g04locate_col (stackEntry, pRecord, varcolPos, columnPosition, columnLength);

    if (  0 == columnLength )
        return true;

    const SAPDB_Byte *defineByte = reinterpret_cast<SAPDB_Byte*>(pRecord) + (columnPosition-1);

    if ( csp_undef_byte == *defineByte )
        return true;
    
    return false;
}
*/
/*===========================================================================*
*  METHODS of Log_InvEntryIter                                               *
*============================================================================*/

inline Log_InvEntryIter::Log_InvEntryIter (const Log_InvDescMap &InvDescMap):
    m_InvDescMap (InvDescMap)
{
    ResetEntryIter();
}

//---------------------------------------------------------------------------

inline Log_InvEntryIter::Log_InvEntryIter (const Log_InvEntryIter &EntryIter):
    m_CurrEntry  (EntryIter.m_CurrEntry),
    m_LastEntry  (EntryIter.m_LastEntry),
    m_InvDescMap (EntryIter.m_InvDescMap)
{ }

//---------------------------------------------------------------------------

inline void Log_InvEntryIter::Next ()
{
    if ( ! HasMore() ) return;
    
    ++m_CurrEntry;
    
    if (m_CurrEntry >= m_LastEntry) m_LastEntry = NilIndex();
}

//---------------------------------------------------------------------------

inline void Log_InvEntryIter::ResetEntryIter ()
{
    m_CurrEntry = NilIndex();
    m_LastEntry = NilIndex();
}


/*===========================================================================*
*  METHODS of Log_InvDescIter                                                *
*============================================================================*/

inline Log_InvDescIter::Log_InvDescIter (InvDescIterKind       IterKind,
                                         const Log_InvDescMap &InvDescMap):
    Log_InvEntryIter           (InvDescMap),
    m_CurrDesc                 (NilIndex()),
    m_NextDesc                 (0),
    m_PrevDesc                 (NilIndex()),
    m_IterKind                 (IterKind),
    m_UniqueEntriesHandled     (0),
    m_LateUniqueEntriesHandled (0)
{
    if ( InvDescMap.IsValidIndex(m_NextDesc) )
    {
        NextSpecificDesc();
    }
    else
        m_NextDesc = NilIndex();
}

//---------------------------------------------------------------------------

void Log_InvDescIter::GetNextKeyAndId (tgg00_TransContext &TransContext,
                                       const tgg00_Rec    *pRec,
                                       tgg00_Lkey         &SecondaryKey,
                                       bool               &allColumnsAreNull)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_InvDescIter::GetNextKeyAndId", LogAction_Trace, 5);

    tsp00_KeyPtr        pSecondaryKey   = (SecondaryKey.keyVal_gg00())+0;
    SAPDB_Int2         &SecondaryKeyLen = SecondaryKey.keyLen_gg00();

    Log_InvEntryIter    InvEntryIter (*this);
    tgg00_VarColPosList VarColPos;
    tgg00_BasisError    DummyErr;
    SAPDB_Byte          DefinedByte;
    SAPDB_Int2          i;
    SAPDB_Int           ColOffset  = 0;
    SAPDB_Int           ColLen     = 0;
    SAPDB_Int           FixLen     = 0;
    SAPDB_Int           PrevKeyLen = 0;
    
    SecondaryKeyLen    = 0;
    VarColPos.vpl_last = -1;
    
    allColumnsAreNull  = true;

    if ( ! InvEntryIter.HasMore() )
        return;
    
    do
    {
        InvEntryIter.Next ();

        PrevKeyLen  = SecondaryKeyLen;
        DefinedByte = csp_undef_byte;
        DummyErr    = e_ok;

        const tgg00_Rec *pValueBuf;
        int             ValBufSize;
        bool            IsDescendingEntry = IsDescendingInvEntry (InvEntryIter.StackEntry());
        if (st_func == InvEntryIter.StackEntry().etype()) // PTS 1120019
        {
            // the current index is function based, i.e. the secondary key has to be
            // calculated as the result of a user defined function.
            // The input parameters of the function call are values of the current row (pRec)
            // The parameters are transferred via a fieldmap, which is allocated on the
            // stack via an incremental buffer allocator.

            SAPDBMem_IRawAllocator::AlignType buffer[(MAX_COL_PER_INDEX_GG00 + 2) * sizeof(InvEntryIter.StackEntry()) /
                sizeof(SAPDBMem_IRawAllocator::AlignType )];
            SAPDBMem_IncrementalBufferAllocator allocator(&buffer[0], sizeof(buffer));
            SAPDBFields_FieldMap::IndexType paramCount = InvEntryIter.StackEntry().epos();
            SAPDBFields_FieldMap funcParams(allocator, paramCount);
            InvEntryIter.Next();

            // construct fieldmap of function input parameters 
            const tgg00_StackEntry& functionId = InvEntryIter.StackEntry();
            DBProc_Handler::ParameterDescriptors parameterDescriptors;
            for (SAPDBFields_FieldMap::IndexType paramIdx = 0; paramIdx < paramCount; ++paramIdx)
            {
                InvEntryIter.Next();
                g04locate_col (InvEntryIter.StackEntry(), pRec, VarColPos, ColOffset, ColLen);
                void* const fieldBytes = const_cast<void*>
                    (reinterpret_cast<const void*>(&pRec->recBuf_gg00()[ColOffset -1]));
                SAPDBFields_AttributedField field(fieldBytes, ColLen);
                funcParams.Fill(paramIdx, paramIdx, field);
                parameterDescriptors[paramIdx].m_fixedLength = InvEntryIter.StackEntry().ecol_pos();
                parameterDescriptors[paramIdx].m_toUnicode   = (op_dbyte_translate == InvEntryIter.StackEntry().eop());
            }
            // calculate the function result
            DBProc_Handler::CalculateFunctionResult (TransContext, 
                functionId, funcParams, parameterDescriptors, pValueBuf, ColLen);
            if (e_ok != TransContext.trError_gg00)
            {
                this->NextSpecificDesc();
                TransContext.trError_gg00 = e_dbfunc_index_error;
                return;
            }
            ValBufSize = ColLen;
            FixLen     = ColLen;
            ColOffset  = 1;
        }
        else
        {
            pValueBuf = pRec;
            g04locate_col (InvEntryIter.StackEntry(), pRec, VarColPos, ColOffset, ColLen);
            FixLen     = InvEntryIter.StackEntry().elen_var();
            ValBufSize = reinterpret_cast<const CnsRead_CachedRow*>(pValueBuf)->GetRecLen();
        }

        --ColOffset;
        
        if  (ColLen > 0)
        {
            DefinedByte = pValueBuf->recBuf_gg00()[ColOffset];
            
            if (csp_undef_byte == DefinedByte )
                ColLen = 0;
            else
            {
                allColumnsAreNull = false;
                if ( (ColLen >1) && ! InvEntryIter.HasMore() )
                {
                    // get length of last column without trailing defined bytes 
                    ColLen = 1 + s30lnr_defbyte (pValueBuf, DefinedByte, ColOffset+2, ColLen-1);
                }
            }
        }
        
        if (ColLen <= 0)
        {
            pSecondaryKey [SecondaryKeyLen] = csp_undef_byte;
            
            ++SecondaryKeyLen;
            
            if ( InvEntryIter.HasMore() || IsDescendingEntry )
            {
                SAPDB_RangeFill (__FILE__, 1, sizeof (tsp00_Key),
                    pSecondaryKey, SecondaryKeyLen+1, FixLen-1, csp_defined_byte, DummyErr);
                
                SecondaryKeyLen += (FixLen - 1);
            }
        }
        else
        {
            SAPDB_RangeMove (__FILE__, 2, ValBufSize, sizeof (tsp00_Key),
                pValueBuf, ColOffset+1,
                pSecondaryKey, SecondaryKeyLen+1,
                ColLen, DummyErr);
            
            SecondaryKeyLen += ColLen;
            
            if ((ColLen < FixLen)
                &&
                (InvEntryIter.HasMore() || IsDescendingEntry ))
            {
                if (csp_unicode_def_byte == DefinedByte)
                {
                    SAPDB_RangeUnicodeFill (__FILE__, 3, sizeof (tsp00_Key),
                        pSecondaryKey, SecondaryKeyLen+1, FixLen-ColLen,
                        csp_unicode_blank, DummyErr);
                }
                else
                {
                    SAPDB_RangeFill (__FILE__, 4, sizeof (tsp00_Key),
                        pSecondaryKey, SecondaryKeyLen+1, FixLen-ColLen,
                        DefinedByte, DummyErr);
                }
                
                SecondaryKeyLen += (FixLen - ColLen);
            }
        }
        
        if ( IsDescendingEntry )
        {
            for (i = PrevKeyLen; i < SecondaryKeyLen; ++i)
            {
                pSecondaryKey[i] = (255 - pSecondaryKey[i]);
            }
        }
    }
    while ( InvEntryIter.HasMore() );

    this->NextSpecificDesc();
}

//---------------------------------------------------------------------------

void Log_InvDescIter::GetNextAndCopyUsedEntries (const Log_IRecordColumnMap &RecordColumnMap,
                                                 tgg00_StackEntry           *pNewInvDescMap,
                                                 SAPDB_Int                  &NewEntries)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_InvDescIter::GetNextAndCopyUsedEntries", LogAction_Trace, 5);
    
    const SAPDB_Int  StartNewEntries = NewEntries;
    bool             EntryFound      = false;
    Log_InvEntryIter InvEntryIter (*this);
        
    while ( InvEntryIter.HasMore() )
    {
        InvEntryIter.Next ();
        
        if ( RecordColumnMap.ExistsEntry(InvEntryIter.StackEntry()) ) EntryFound = true;
        
        *(pNewInvDescMap + NewEntries) = InvEntryIter.StackEntry();
        
        ++NewEntries;
    }
    
    this->NextSpecificDesc();
    
    if ( ! EntryFound ) NewEntries = StartNewEntries;
}

//----------------------------------------------------------------------------

void Log_InvDescIter::StoreNotUpdatedIndexColumn ( Log_IRecordColumnMap &RecordColumnMap,
        const tgg00_Rec &Image)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_InvDescIter::StoreNotUpdatedIndexColumn", LogAction_Trace, 5);
    
    tgg00_VarColPosList varColPos;
    SAPDB_Int           colOffset  = 0;
    SAPDB_Int           colLen     = 0;
    tgg00_BasisError    error;

    varColPos.vpl_last = -1;
    Log_InvEntryIter InvEntryIter (*this);
    
    while ( InvEntryIter.HasMore() )
    {
        InvEntryIter.Next ();
        
        if ( !RecordColumnMap.ExistsEntry(InvEntryIter.StackEntry()) ) 
        {
            RecordColumnMap.AppendColumn (InvEntryIter.StackEntry(), Image, error);
        }
    }
    
    this->NextSpecificDesc();
}

//----------------------------------------------------------------------------

void Log_InvDescIter::NextDescAndInitEntryIter()
{
    SAPDBTRACE_METHOD_DEBUG ("Log_InvDescIter::NextDescAndInitEntryIter", LogAction_Trace, 5);

    this->ResetEntryIter ();

    if ( NilIndex() == this->m_NextDesc )
    {
        this->m_CurrDesc = NilIndex();
        return;
    }        
    
    this->m_CurrDesc  = this->m_NextDesc;
    this->m_CurrEntry = this->m_CurrDesc - 1;
    
    do
    {
        if (EveryDesc != this->m_IterKind)
        {
            if ( IsUniqueEntry     (this->m_InvDescMap.StackEntry(this->m_NextDesc)) )
                ++this->m_UniqueEntriesHandled;
            
            if ( IsLateUniqueEntry (this->m_InvDescMap.StackEntry(this->m_NextDesc)) )
                ++this->m_LateUniqueEntriesHandled;
        }
        
        ++this->m_NextDesc;
        
        if ( ! this->m_InvDescMap.IsValidIndex (this->m_NextDesc) )
        {
            this->m_LastEntry = this->m_NextDesc - 1;
            this->m_NextDesc  = NilIndex();
        }
        else
        {
            int funcIndexEntries = this->m_InvDescMap.FunctionBasedIndexDescCnt(this->m_CurrDesc); // PTS 1120019
            if (funcIndexEntries > 0)
            {
                // next entries descibe input parameters, skip them here
               this->m_NextDesc += funcIndexEntries;
            }
            else
            {
                if (this->m_InvDescMap.InvDescId(this->m_NextDesc) != 
                    this->m_InvDescMap.InvDescId(this->m_CurrDesc)    )
                {
                    this->m_LastEntry = this->m_NextDesc - 1;
                }
            }
        }
    }
    while ( NilIndex() == this->m_LastEntry );
}

//---------------------------------------------------------------------------

void Log_InvDescIter::NextSpecificDesc ()
{  
    SAPDBTRACE_METHOD_DEBUG ("Log_InvDescIter::NextSpecificDesc", LogAction_Trace, 5);
    
    bool found = false;
    
    this->m_PrevDesc = this->m_CurrDesc;
    this->ResetEntryIter ();
    
    switch (this->m_IterKind)
    {
    case UniqueDesc:
    case UniqueDescWithoutLateUnique:
        {   
            if (this->m_UniqueEntriesHandled >= this->m_InvDescMap.UniqueEntries())
            {
                m_NextDesc = NilIndex();
                return;
            }
            break;
        }   
    case LateUnique:
        {
            if (this->m_LateUniqueEntriesHandled >= this->m_InvDescMap.LateUniqueCheckEntries())
            {
                m_NextDesc = NilIndex();
                return;
            }
            break;
        }
    }    
    
    while ( this->m_InvDescMap.IsValidIndex(this->m_NextDesc) && ! found )
    {
        this->NextDescAndInitEntryIter();
        
        switch (this->m_IterKind)
        {
        case EveryDesc:
            {
                found = true;
                break;
            }
        case UniqueDesc:
            {
                found = IsUniqueEntry (this->m_InvDescMap.StackEntry(this->m_CurrDesc));
                break;
            }
        case UniqueDescWithoutLateUnique:
            {
                found = 
                    IsUniqueEntryWithoutLateUnique (this->m_InvDescMap.StackEntry(this->m_CurrDesc));
                break;
            }
            
        case LateUnique:
            {
                found = IsLateUniqueEntry (this->m_InvDescMap.StackEntry(this->m_CurrDesc));
                break;
            }
        }
    }
}

/*===========================================================================*
*  END OF CODE                                                               *
*============================================================================*/
