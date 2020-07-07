/*!**************************************************************************

    module      : Log_ActionInsDelRec.cpp

    -------------------------------------------------------------------------

    author      : JuergenA
    responsible : UweH

    special area: Logging
    description : defines a class to handle record log entries

    last changed: 2001-02-20

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

#include "Logging/Log_ActionInsDelRec.hpp"

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp" // Routine trace macros
#include "SAPDBCommon/SAPDB_RangeCode.hpp"
#include "KernelCommon/Kernel_Trace.hpp"
#include "Logging/Log_InvDescMap.hpp"
#include "Logging/Log_ReadWriteActionImage.hpp"
#include "Logging/Log_Transaction.hpp"
#include "ConsistentRead/CnsRead_Manager.hpp"
#include "ConsistentRead/CnsRead_CachedRow.hpp"
#include "FileDirectory/FileDir_IDirectory.hpp"
#include "FileDirectory/FileDir_Generic.hpp"

#include "ggg00.h"
#include "hbd01_1.h"
#include "hbd02.h"

Log_ActionInsDelRec::Log_ActionInsDelRec (Log_ActionType ActionType,
                                          bool           IsBeforeImage):
Log_ReadWriteActionImage (ActionType),
m_pRec                   (NULL),
m_pInvDescMap            (NULL),

m_bKeyOnly               ((IsBeforeImage  && (Log_InsertRecord == ActionType))
                          ||
                          (!IsBeforeImage && (Log_DeleteRecord == ActionType))),
                          
                          m_pRecForRead            (NULL),
                          m_InvDescMapForRead      (),
                          m_pRecAllocator          (NULL),
                          m_PrevUpdTrans           (),
                          m_PrevRef                ()
{  
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionInsDelRec::Log_ActionInsDelRec", LogAction_Trace, 5);
    m_EntryHeader.ImageLen    = 0;
    m_EntryHeader.KeyOrRecLen = 0;
    m_EntryHeader.InvDescLen  = 0;
}

//---------------------------------------------------------------------------

Log_ActionInsDelRec::Log_ActionInsDelRec (
    Log_ActionType       ActionType,
    const tgg91_TransNo& PrevUpdTrans,
    const tgg91_PageRef& PrevRef)
    : Log_ReadWriteActionImage (ActionType)
    , m_pRec                   (NULL)
    , m_pInvDescMap            (NULL)
    , m_bKeyOnly               (Log_InsertRecord == ActionType)
    , m_pRecForRead            (NULL)
    , m_InvDescMapForRead      ()
    , m_pRecAllocator          (NULL)
    , m_PrevUpdTrans           (PrevUpdTrans)
    , m_PrevRef                (PrevRef)
{  
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionInsDelRec::Log_ActionInsDelRec", LogAction_Trace, 5);
    m_EntryHeader.ImageLen    = 0;
    m_EntryHeader.KeyOrRecLen = 0;
    m_EntryHeader.InvDescLen  = 0;
}

//---------------------------------------------------------------------------

Log_ActionInsDelRec::~Log_ActionInsDelRec ()
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionInsDelRec::~Log_ActionInsDelRec", LogAction_Trace, 5);
    if (NULL != m_pRecAllocator)
    {
        m_pRecAllocator->Deallocate (m_pRecForRead);
        m_pRecForRead = NULL;
    }
}

//---------------------------------------------------------------------------
void Log_ActionInsDelRec::WriteToTrace (const char * Title) const
{
    Kernel_VTrace trace;
    if ( Title != 0 )
        trace << Title << FlushLine;
    Kernel_TraceBuffer(&m_EntryHeader.PrimFilename, sizeof(m_EntryHeader.PrimFilename),
                       "PrimFilename", sizeof(m_EntryHeader.PrimFilename));
	if ( m_pRec != 0 )
        Kernel_TraceBuffer(m_pRec, m_pRec->recLen_gg00(),
                           "Record", m_pRec->recLen_gg00());
	else
        trace << "m_pRec is null" << FlushLine;
}

//---------------------------------------------------------------------------

void Log_ActionInsDelRec::Execute (tgg00_TransContext &TransContext) const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionInsDelRec::Execute", LogAction_Trace, 5);
    
    KERNEL_TRACE_BASIS_ERROR (TransContext.trError_gg00, LogAction_Trace, 5);
    
    if (e_ok != TransContext.trError_gg00 )
        return; 
    
    const bool              IsUndoOrRedo = true; // This Methode is currently used only by undo or redo
    const bool              IsRedo       = Log_Transaction::redoExecute == reinterpret_cast<Log_Transaction*>(TransContext.trLogContext_gg00)
                                           ->GetTransType();
    const bool              IsUndo       = reinterpret_cast<Log_Transaction*>(TransContext.trLogContext_gg00)->IsRollback();
    const bool              inSavepoint  = reinterpret_cast<Log_Transaction*>(TransContext.trLogContext_gg00)
                                           ->WasOpenInLastSavepoint();
          tgg00_BasisError &result       = TransContext.trError_gg00;
          tgg00_FileId      PrimFileId   = b01niltree_id;

    PrimFileId.fileName_gg00() = m_EntryHeader.PrimFilename;
    PrimFileId.fileType_gg00().clear();
    PrimFileId.fileType_gg00().addElement (ftsPerm_egg00);
    PrimFileId.fileType_gg00().addElement (ftsConcurrent_egg00);

    // PTS 1128514 UH 2004-03-19 changed error handling
    // In principle: The intention of the undo or redo action must be reached.
    // If a record must be inserted at the end it must be existing with all indexes
    // and it doesn't matter if (e.g. by savepoint/restart) parts of the actions result
    // are already done.
    // Because the trans error can contain any error code only the result can be tested
    // to decide if the acion  was successfull executed or not.

    if (m_bKeyOnly)
    {
        // key entry ==> delete
        tgg00_Rec PrimRec;
        bool      needToDeleteIndexes = true;

        if (reinterpret_cast<const CnsRead_CachedRow*>(m_pRec)->IsDeleted())
        {
            // update history chain only if required (TODO) 
            b02ReplaceRecord (TransContext, PrimFileId, *m_pRec, true, m_PrevRef, m_PrevUpdTrans);
            return;
        }
        
        b02get_record (TransContext, PrimFileId, m_pRec->recKey_gg00(), PrimRec);
        
        if ( e_key_not_found == result )
        {
            Kernel_VTrace() << __FILE__ << __LINE__ << NewLine << "IGNORED ERROR is " << result;
            WriteToTrace("Log_ActionInsDelRec::Execute delete");
            result = e_ok;
            needToDeleteIndexes = false;
        }

        if ( result != e_ok && IsUndoOrRedo )
        {
            Kernel_VTrace() << __FILE__ << __LINE__ << NewLine << "IGNORED ERROR is " << result;
            WriteToTrace("Log_ActionInsDelRec::Execute delete");
            result = e_ok;
        }

        if ( e_ok == result && needToDeleteIndexes )
        {
            m_pInvDescMap->DelInv (TransContext, IsUndoOrRedo, inSavepoint, false /* no garbage */, PrimFileId, &PrimRec);
            if ( result != e_ok && IsUndoOrRedo )
                result = e_ok;
        }

        if ( e_ok == result )
            b02del_record (TransContext, PrimFileId, m_pRec->recKey_gg00());

        if ( result != e_ok && result && IsUndoOrRedo )
        {
            Kernel_VTrace() << __FILE__ << __LINE__ << NewLine << "IGNORED ERROR is " << result;
            WriteToTrace("Log_ActionInsDelRec::Execute delete b02del_record");
            result = e_ok;
        }
    }
    else
    {
        if (Log_MarkedDeleted == m_Type)
        {
            tgg00_Rec primRec;
            b02MarkRecordDeleted (TransContext, PrimFileId, false, m_PrevRef, m_PrevUpdTrans, m_pRec->recKey_gg00(), &primRec); 
            if ( e_ok == result )
            {
                m_pInvDescMap->AddInv (TransContext, IsUndo, IsRedo, inSavepoint, PrimFileId, &primRec);
            }
            if ( result != e_ok && IsUndoOrRedo )
            {
                result = e_ok;
            }
            return;
        }
    
        // record entry ==> insert
        
        b02add_record (TransContext, PrimFileId, *(m_pRec));
        
        if ( e_duplicate_key == result )
        {
            if ( inSavepoint ) // PTS 1134699 UH 2005-04-04 added
                result = e_ok;
            else
            {
                // check if the record is equal to the new record
                result = e_ok;
                tgg00_Rec PrimRec;
                b02get_record (TransContext, PrimFileId, m_pRec->recKey_gg00(), PrimRec);
                if ( e_ok == result )
                {
                    if ( memcmp(&PrimRec,m_pRec,m_pRec->recLen_gg00()) )
                    {
                        // The record must be equal to that what should be added
                        // Otherwise there is a lock problem or another error.
                        result = e_duplicate_key;
                        return;
                    }
                }
            }
        }
        
        if ( result != e_ok && IsUndoOrRedo )
        {
            Kernel_VTrace() << __FILE__ << __LINE__ << NewLine << "IGNORED ERROR is " << result;
            WriteToTrace("Log_ActionInsDelRec::Execute insert b02add_record");
            result = e_ok;
        }

        if ( e_ok == result )
            m_pInvDescMap->AddInv (TransContext, IsUndo && CnsRead_Manager::IsConsistentRead_Configurated(), IsRedo, inSavepoint, PrimFileId, m_pRec);

        if ( result != e_ok && IsUndoOrRedo )
            result = e_ok;
    }
}
//---------------------------------------------------------------------------

void Log_ActionInsDelRec::GetPrimarayFile (tgg00_FileId& PrimFileId) const
{
    PrimFileId                 = b01niltree_id;
    PrimFileId.fileName_gg00() = m_EntryHeader.PrimFilename;
    PrimFileId.fileType_gg00().clear();
    PrimFileId.fileType_gg00().addElement (ftsPerm_egg00);
    PrimFileId.fileType_gg00().addElement (ftsConcurrent_egg00);
}
//---------------------------------------------------------------------------

void Log_ActionInsDelRec::Init (const tgg00_Filename &PrimFilename,
                                const tgg00_Lkey     *pKey,
                                const Log_InvDescMap *pInvDescMap)
{ 
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionInsDelRec::Init1", LogAction_Trace, 5);
    SAPDBERR_ASSERT_STATE (m_bKeyOnly || CnsRead_Manager::IsConsistentRead_Configurated());
    
    m_EntryHeader.PrimFilename = PrimFilename;
    m_pRec                     = REINTERPRET_CAST (const tgg00_Rec*, pKey);
    m_pInvDescMap              = pInvDescMap;
    m_EntryHeader.KeyOrRecLen  = sizeof(tgg00_HeaderRec) + pKey->keyLen_gg00();
    
    InitHeader ();
}  

//---------------------------------------------------------------------------

void Log_ActionInsDelRec::Init (const tgg00_Filename &PrimFilename,
                                const tgg00_Rec      *pRec,
                                const Log_InvDescMap *pInvDescMap,
                                bool                  MarkDeleted)
                                
{ 
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionInsDelRec::Init2", LogAction_Trace, 5);
    SAPDBERR_ASSERT_STATE (! m_bKeyOnly);
    
    m_EntryHeader.PrimFilename = PrimFilename;
    m_pRec                     = pRec;
    m_pInvDescMap              = pInvDescMap;
    m_EntryHeader.KeyOrRecLen  = (MarkDeleted && (0 == pInvDescMap)) ? cgg_rec_key_offset + pRec->recKeyLen_gg00() : pRec->recLen_gg00();
    
    InitHeader ();
}

//---------------------------------------------------------------------------

void Log_ActionInsDelRec::InitHeader ()
{                                      
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionInsDelRec::InitHeader", LogAction_Trace, 5);
    m_EntryHeader.InvDescLen   = 
        (NULL == m_pInvDescMap) ? 0 : m_pInvDescMap->GetPersistentLength();
    
    m_EntryHeader.ImageLen = 
        Log_AlignedImageLen   (sizeof(m_EntryHeader))
        + Log_AlignedImageLen (m_EntryHeader.KeyOrRecLen)
        + Log_AlignedImageLen (m_EntryHeader.InvDescLen);
}

//---------------------------------------------------------------------------

void Log_ActionInsDelRec::GarbageDelete (tgg00_TransContext &TransContext, tgg91_PageRef &wantedPageref, Msg_List &msgList)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionInsDelRec::GarbageDelete", LogAction_Trace, 5);
    tgg00_Rec BeforeImage;
    this->MaterializeRecord(sizeof(BeforeImage), &BeforeImage, TransContext.trError_gg00);
    tgg00_FileId fileId;
    this->GetPrimarayFile(fileId);
   
    FileDir_IDirectory& fdir = FileDir_IDirectory::SessionInstance(TransContext);
    FileDir_Generic pFileDir;
    FileDir_ReturnCode rc = fdir.GcGetFile(TransContext.trTaskId_gg00, 
        *reinterpret_cast<FileDir_FileNo*>(&fileId.fileTabId_gg00()), pFileDir, msgList);
    if (FileDir_Okay != rc)
    {
        TransContext.trError_gg00 = e_file_not_found;
        return;
    }
    pFileDir.BuildFileId (fileId);
    fileId.fileVersion_gg00().gg91SetDummyVers(); // TODO review 
    m_pInvDescMap->DelInv (TransContext, false /*no undo/redo */, false, true /* garbage collection*/, fileId, &BeforeImage); // TODO in savepoint ?
    Kernel_VTrace trace;
    if (e_ok == TransContext.trError_gg00)
    {
        b02DelGarbage (TransContext, fileId, BeforeImage.recKey_gg00(), wantedPageref);
        if (e_ok != TransContext.trError_gg00)
        {
            trace << "b02DelGarbage failed with " << TransContext.trError_gg00 << " root " << fileId.fileRoot_gg00() << FlushLine;
        }
    }
    else
    {
        trace << "GarbageDelete(inv) failed " << TransContext.trError_gg00 << " root " << fileId.fileRoot_gg00() << FlushLine;
    }
}

//---------------------------------------------------------------------------

void Log_ActionInsDelRec::MaterializeRecord (SAPDB_UInt4       DestinSize,
                                             void             *pDestin,
                                             tgg00_BasisError &Error) const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionInsDelRec::MaterializeRecord", LogAction_Trace, 5);
    
    if ((e_ok != Error) || (NULL == m_pRec)) return;
    
    if (m_pRec->recLen_gg00() > 0)
    {
         SAPDB_RangeMove (__FILE__, 1,
            m_pRec->recLen_gg00(), DestinSize,
            m_pRec, 1,
            pDestin, 1,
            m_pRec->recLen_gg00(), Error);
    }
}

//----------------------------------------------------------------------------

bool Log_ActionInsDelRec::ReadImagePersistent (Log_ActionImageReader &ImageReader,
                                               SAPDBErr_MessageList  &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionInsDelRec::ReadImagePersistent", LogAction_Trace, 5);
    
    m_pInvDescMap = &(m_InvDescMapForRead);
    
    if ( ! ImageReader.CopyImage (REINTERPRET_CAST (SAPDB_Byte*, &(m_EntryHeader)),sizeof(m_EntryHeader),errlist))
        return false;
    
    if ( ! ImageReader.PersistentRead (REINTERPRET_CAST (SAPDB_Byte*&, m_pRecForRead),
                                       m_EntryHeader.KeyOrRecLen, m_pRecAllocator, errlist))
        return false;
    
    m_pRec = m_pRecForRead;
    
    if (m_EntryHeader.InvDescLen > 0)
    {
        if (! m_InvDescMapForRead.ReadImagePersistent(ImageReader, m_EntryHeader.InvDescLen, errlist))
            return false;
    }
    return true;
}

//----------------------------------------------------------------------------

bool Log_ActionInsDelRec::WriteImagePersistent (Log_ActionImageWriter &ImageWriter,
                                                SAPDBErr_MessageList  &errlist)       const
                                                
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionInsDelRec::WriteImagePersistent", LogAction_Trace, 5);

    if (! ImageWriter.PersistentWrite (&(m_EntryHeader), sizeof (m_EntryHeader), errlist) )
        return false;
    
    if (! ImageWriter.PersistentWrite (m_pRec, m_EntryHeader.KeyOrRecLen, errlist) )
        return false;
    
    if (m_EntryHeader.InvDescLen > 0)
    { 
        if (! m_pInvDescMap->WriteImagePersistent (ImageWriter, errlist))
            return false;
    }
    return true;
}
