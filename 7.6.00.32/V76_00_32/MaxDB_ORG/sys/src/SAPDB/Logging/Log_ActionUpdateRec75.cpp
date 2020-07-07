/*!**************************************************************************

    module      : Log_ActionUpdateRec75.cpp

    -------------------------------------------------------------------------

    author      : JuergenA
    responsible : UweH

    special area: Logging
    description : defines a class to handle update log entries

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

#include "Logging/Log_ActionUpdateRec75.hpp"

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp" // Routine trace macros
#include "KernelCommon/Kernel_VTrace.hpp"
#include "KernelCommon/Kernel_Trace.hpp"
#include "Logging/Log_InvDescMap.hpp"
#include "Logging/Log_ReadWriteActionImage.hpp"
#include "Logging/Log_RecordColumnMap.hpp"
#include "Logging/Log_Transaction.hpp"

#include "FileDirectory/FileDir_IDirectory.hpp"
#include "FileDirectory/FileDir_Table.hpp"
#include "FileDirectory/FileDir_ShortColumn.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"

#include "ggg00.h"
#include "hbd01_1.h"
#include "hbd02.h"

//---------------------------------------------------------------------------

Log_ActionUpdateRec75::Log_ActionUpdateRec75 () :
Log_RecordColumnMap        (Log_UpdateRecord),
m_EntryHeader              (),
m_pKey                     (NULL),
m_pInvDescMap              (NULL),
m_pKeyForRead              (NULL),
m_InvDescMapForRead        (),
m_pKeyAllocator            (NULL),
m_PrevUpdTrans             (),
m_PrevRef                  ()
{
    m_PrevRef.gg91SetDummyRef();
}

//---------------------------------------------------------------------------

Log_ActionUpdateRec75::Log_ActionUpdateRec75(const tgg91_TransNo& prevTrans, const tgg91_PageRef& prevRef) :
Log_RecordColumnMap        (Log_UpdateRecord),
m_EntryHeader              (),
m_pKey                     (NULL),
m_pInvDescMap              (NULL),
m_pKeyForRead              (NULL),
m_InvDescMapForRead        (),
m_pKeyAllocator            (NULL),
m_PrevUpdTrans             (prevTrans),
m_PrevRef                  (prevRef)
{ }

//---------------------------------------------------------------------------

Log_ActionUpdateRec75::~Log_ActionUpdateRec75 ()
{
    if (NULL != m_pKeyAllocator)
    {
        m_pKeyAllocator->Deallocate (m_pKeyForRead);
        m_pKeyForRead = NULL;
    }
}

//---------------------------------------------------------------------------

void Log_ActionUpdateRec75::AddCompoundIndexColumns (
                              Log_InvDescMap  *pOwnWriteAccessInvDescMap,
                              const tgg00_Rec &Image,
                              bool            &IsOk)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionUpdateRec75::AddCompoundIndexColumns", LogAction_Trace, 5);
    KERNEL_TRACE_IS_OK (IsOk, LogAction_Trace, 5);

    if ((NULL == m_pInvDescMap) ||
        (pOwnWriteAccessInvDescMap != m_pInvDescMap)) return;

    pOwnWriteAccessInvDescMap->AddCompoundIndexColumnsToBeforeImage (*this, Image, IsOk);
    m_EntryHeader.ColMapLen = GetPersistentColumnMapLength();
    m_EntryHeader.ImageLen  =
        Log_AlignedImageLen   (sizeof(m_EntryHeader))
        + Log_AlignedImageLen (m_EntryHeader.KeyLen)
        + Log_AlignedImageLen (m_EntryHeader.ColMapLen)
        + Log_AlignedImageLen (m_EntryHeader.InvDescLen);
}

//---------------------------------------------------------------------------

void Log_ActionUpdateRec75::CreateBeforeImage (const tgg00_Rec& currImage, tgg00_Rec& beforeImage)
{
    beforeImage.recLen_gg00() = m_EntryHeader.NewRecLen;

    tgg00_BasisError e = e_ok;
    CreateNewRecord (&currImage, &beforeImage, e);
}

//---------------------------------------------------------------------------

void Log_ActionUpdateRec75::WriteToTrace (const char * Title) const
{
    Kernel_VTrace trace;
    if ( Title != 0 )
        trace << Title << FlushLine;
    Kernel_TraceBuffer(&m_EntryHeader.PrimFilename, sizeof(m_EntryHeader.PrimFilename),
                       "PrimFilename", sizeof(m_EntryHeader.PrimFilename));
	if ( m_pKey != 0 )
        Kernel_TraceBuffer(&m_pKey->keyVal_gg00(), m_pKey->keyLen_gg00(),
                           "Key", m_pKey->keyLen_gg00());
	else
        trace << "m_pKey is null" << FlushLine;
}

//---------------------------------------------------------------------------

void Log_ActionUpdateRec75::Execute (tgg00_TransContext &TransContext) const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionUpdateRec75::Execute", LogAction_Trace, 5);
    KERNEL_TRACE_BASIS_ERROR (TransContext.trError_gg00, LogAction_Trace, 5);

    if (e_ok != TransContext.trError_gg00) return;

    tgg00_Rec    OldRecord;
    const bool   IsUndoOrRedo = true;
    tgg00_FileId PrimFileId   = b01niltree_id;

    SAPDBMem_IRawAllocator& RawAllocator =
        *(REINTERPRET_CAST (SAPDBMem_IRawAllocator*, TransContext.trAllocator_gg00));

    tgg00_Rec *pNewRecord =
        // PTS 1122131 UH 2003-05-14 allocate full buffer
        REINTERPRET_CAST (tgg00_Rec*, RawAllocator.Allocate(sizeof(tgg00_Rec)));
        // REINTERPRET_CAST (tgg00_Rec*, RawAllocator.Allocate(m_EntryHeader.NewRecLen));

    if (NULL == pNewRecord)
    {
        TransContext.trError_gg00 = e_no_more_memory;
        return;
    }

    // get correct file name for short column files
    this->GetPrimaryFile( TransContext, PrimFileId );

    b02get_record (TransContext, PrimFileId, *(m_pKey), OldRecord);

    if ( e_key_not_found == TransContext.trError_gg00
         &&
         reinterpret_cast<Log_Transaction*>(TransContext.trLogContext_gg00)->WasOpenInLastSavepoint() )
    {
        // PTS 1134699 UH 2005-04-04 ignore this error in this case until better checks are available
        TransContext.trError_gg00 = e_ok;
    }
    else
    {
        if (e_ok == TransContext.trError_gg00)
        {
            pNewRecord->recLen_gg00() = m_EntryHeader.NewRecLen;
            
            CreateNewRecord (&OldRecord, pNewRecord, TransContext.trError_gg00);
        }
    
        if (e_ok == TransContext.trError_gg00)
        {
            ExecuteAction (TransContext, PrimFileId, &OldRecord, pNewRecord, IsUndoOrRedo);
        }
    }

    RawAllocator.Deallocate (pNewRecord);
}

//----------------------------------------------------------------------------


void Log_ActionUpdateRec75::ExecuteAction (tgg00_TransContext &TransContext,
                                         tgg00_FileId       &PrimFileId,
                                         const tgg00_Rec    *pOldRecord,
                                         const tgg00_Rec    *pNewRecord,
                                         bool                IsUndoOrRedo) const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionUpdateRec75::ExecuteAction", LogAction_Trace, 5);
    KERNEL_TRACE_BASIS_ERROR (TransContext.trError_gg00, LogAction_Trace, 5);

    bool inSavepoint     = false;
    bool recordIsUpdated = false;

    if ( IsUndoOrRedo )
    {
        inSavepoint = reinterpret_cast<Log_Transaction*>(TransContext.trLogContext_gg00)
                      ->WasOpenInLastSavepoint();

        if ( inSavepoint )
        {
            // PTS 1124375 UH 2003-09-24 recordIsUpdated became always true
            const SAPDB_Byte *oldbody   = reinterpret_cast<const SAPDB_Byte*>(&(pOldRecord->recBody_gg00()))
                                          + pOldRecord->recKeyLen_gg00();
            const SAPDB_Byte *newbody   = reinterpret_cast<const SAPDB_Byte*>(&(pNewRecord->recBody_gg00()))
                                          + pNewRecord->recKeyLen_gg00();
            const SAPDB_UInt  oldlength = pOldRecord->recLen_gg00() - cgg_rec_key_offset -
                                          pOldRecord->recKeyLen_gg00();
            const SAPDB_UInt  newlength = pNewRecord->recLen_gg00() - cgg_rec_key_offset -
                                          pNewRecord->recKeyLen_gg00();

            recordIsUpdated = oldlength == newlength
                              &&
                              0 == memcmp(oldbody, newbody, oldlength);
        }
    }

    // PTS 1128514 UH 2004-03-19 changed error handling
    // In principle: The intention of the undo or redo action must be reached.
    // If a record must be inserted at the end it must be existing with all indexes
    // and it doesn't matter if (e.g. by savepoint/restart) parts of the actions result
    // are already done.
    // Because the trans error can contain any error code only the result can be tested
    // to decide if the acion  was successfull executed or not.

    tgg00_BasisError &result = TransContext.trError_gg00;

    result = e_ok;

    if ( ! recordIsUpdated )
    {
        m_pInvDescMap->DelInv ( TransContext, IsUndoOrRedo, inSavepoint, false /* garbage collection */,
                                PrimFileId, pOldRecord );
        if (e_ok != result && IsUndoOrRedo)
        {
            Kernel_VTrace() << __FILE__ << __LINE__ << NewLine << "IGNORED ERROR is " << result;
            WriteToTrace("Log_ActionUpdateRec75::ExecuteAction error from DelInv");
            result = e_ok;
        }
    }

    if ( e_ok == result && ! recordIsUpdated )
    {
        b02ReplaceRecord (TransContext, PrimFileId, *pNewRecord,
            !m_PrevRef.gg91IsDummyRef(), m_PrevRef, m_PrevUpdTrans);
        //b02repl_record (TransContext, PrimFileId, *pNewRecord);
        if (e_ok != result && IsUndoOrRedo)
        {
            WriteToTrace("Log_ActionUpdateRec75::ExecuteAction error from b02repl_record");
            Kernel_VTrace() << __FILE__ << __LINE__ << NewLine << "IGNORED ERROR is " << result;
            Kernel_TraceBuffer(&pNewRecord, pNewRecord->recLen_gg00(),
                               "NewRecord", pNewRecord->recLen_gg00());
            result = e_ok;
        }
    }

    if ( e_ok == result )
    {
        m_pInvDescMap->AddInv ( TransContext, IsUndoOrRedo, false, inSavepoint, // TODO TH
                                PrimFileId, pNewRecord);
        if (e_ok != result && IsUndoOrRedo)
        {
            WriteToTrace("Log_ActionUpdateRec75::ExecuteAction error from AddInv");
            Kernel_VTrace() << __FILE__ << __LINE__ << NewLine << "IGNORED ERROR is " << result;
            result = e_ok;
        }
    }
}

//----------------------------------------------------------------------------

void Log_ActionUpdateRec75::GarbageCollection(tgg00_TransContext &TransContext) const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionUpdateRec75::GarbageCollection", LogAction_Trace, 5);
    KERNEL_TRACE_BASIS_ERROR (TransContext.trError_gg00, LogAction_Trace, 5);

    if (e_ok != TransContext.trError_gg00) return;

    if (0 == m_EntryHeader.InvDescLen) return; // no indexes, nothing to do

    const bool IsUndoOrRedo = true;

    tgg00_Rec beforeImage;
    tgg00_Rec image;
    memset (&image, 0xFE, sizeof(image)); // TODO T.A.
    image.recKey_gg00() = *(m_pKey);
    SAPDB_Byte* p = &image.recBody_gg00()[image.recKeyLen_gg00() + image.recVarcolOffset_gg00()];
    for (int ix = 0; ix < image.recVarcolCnt_gg00(); ++ix)
    {
        *p++ = 1;
        *p++ = 0xFF;
    }
    memset (&beforeImage, 0xFE, sizeof(beforeImage)); // TODO T.A.
    beforeImage.recLen_gg00() = m_EntryHeader.NewRecLen;
    CreateNewRecord (&image, &beforeImage, TransContext.trError_gg00);

    tgg00_FileId PrimFileId    = b01niltree_id;
    PrimFileId.fileName_gg00() = m_EntryHeader.PrimFilename;
    PrimFileId.fileType_gg00().clear();
    PrimFileId.fileType_gg00().addElement (ftsPerm_egg00);
    PrimFileId.fileType_gg00().addElement (ftsConcurrent_egg00);
    m_pInvDescMap->DelInv (TransContext, !IsUndoOrRedo, false, true /* garbage collection*/, PrimFileId, &beforeImage); // TODO in savepoint ?
}

//----------------------------------------------------------------------------

bool Log_ActionUpdateRec75::ReadImagePersistent (Log_ActionImageReader &ImageReader,
                                               SAPDBErr_MessageList  &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionUpdateRec75::ReadImagePersistent", LogAction_Trace, 5);

    m_pInvDescMap = &(m_InvDescMapForRead);

    if ( ! ImageReader.CopyImage (REINTERPRET_CAST (SAPDB_Byte*, &(m_EntryHeader)),sizeof(m_EntryHeader),errlist))
        return false;

    if (! ImageReader.PersistentRead (REINTERPRET_CAST (SAPDB_Byte*&, m_pKeyForRead),
                                      m_EntryHeader.KeyLen, m_pKeyAllocator, errlist))
        return false;

    m_pKey = m_pKeyForRead;

    if (!ReadColumnMapPersistent (ImageReader, m_EntryHeader.ColMapLen, errlist))
        return false;

    if (m_EntryHeader.InvDescLen > 0)
    {
        if (!m_InvDescMapForRead.ReadImagePersistent(ImageReader, m_EntryHeader.InvDescLen, errlist))
            return false;
    }
    return true;
}

//----------------------------------------------------------------------------

SAPDB_UInt4
Log_ActionUpdateRec75::RemoveRedundantInvDescEntries (SAPDBMem_IRawAllocator &RawAllocator,
                                                    Log_InvDescMap         *pOwnWriteAccessInvDescMap,
                                                    bool                   &IsOk)

{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionUpdateRec75::RemoveRedundantInvDescEntries", LogAction_Trace, 5);
    KERNEL_TRACE_IS_OK (IsOk, LogAction_Trace, 5);

    if ((NULL == m_pInvDescMap) ||
        (pOwnWriteAccessInvDescMap != m_pInvDescMap)) return (0);

    pOwnWriteAccessInvDescMap->RemoveRedundantInvDescEntries (RawAllocator, *this, IsOk);

    m_EntryHeader.InvDescLen = pOwnWriteAccessInvDescMap->GetPersistentLength ();
    return (m_EntryHeader.InvDescLen);
}

//----------------------------------------------------------------------------

bool Log_ActionUpdateRec75::WriteImagePersistent (Log_ActionImageWriter &ImageWriter,
                                                SAPDBErr_MessageList  &errlist)       const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionUpdateRec75::WriteImagePersistent", LogAction_Trace, 5);
    SAPDBTRACE_WRITELN (LogAction_Trace, 6, "ImageLen:" << m_EntryHeader.ImageLen);

    if (!ImageWriter.PersistentWrite (&(m_EntryHeader), sizeof (m_EntryHeader), errlist))
        return false;

    if(!ImageWriter.PersistentWrite (m_pKey, m_EntryHeader.KeyLen, errlist))
        return false;

    if(!WriteColumnMapPersistent (ImageWriter, errlist))
        return false;

    if (m_EntryHeader.InvDescLen > 0)
    {
        if (!m_pInvDescMap->WriteImagePersistent (ImageWriter, errlist))
            return false;
    }
    return true;
}

//----------------------------------------------------------------------------

void Log_ActionUpdateRec75::GetPrimaryFile (
    tgg00_TransContext& TransContext,
    tgg00_FileId& PrimFileId) const
{
    PrimFileId.fileName_gg00() = m_EntryHeader.PrimFilename;

    if ( PrimFileId.fileTfn_gg00() == tfnShortScol_egg00 ) {
        // determine correct fileId for short column file; otherwise
        // record would be inserted into the table itself
        FileDir_FileNo tableFileNo( PrimFileId.fileTabId_gg00() );
        FileDir_Table tableEntry;

        FileDir_IDirectory& fd = FileDir_IDirectory::Instance(cgg_nil_transindex);
        Msg_List errorList;
        fd.GetTableFile (
            TransContext.trTaskId_gg00, tableFileNo, tableEntry, errorList);

        if ( ! tableEntry.IsAssigned() ) {
            TransContext.trError_gg00 = e_file_not_found;
            return;
        }

        FileDir_FileNo shortColumnFileNo( tableEntry.GetShortColumnFileNo() );

        if ( ! shortColumnFileNo.IsValid() ) {
            TransContext.trError_gg00 = e_file_not_found;
            return;
        }

        PrimFileId.fileTabId_gg00().rawAssign(
            shortColumnFileNo.GetCharArray() );
    }
}
