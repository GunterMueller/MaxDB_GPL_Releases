/*!
   @file           vkb431.cpp
   @author         JuergenA
   @author         UweH
   @ingroup        Logging
   @brief          Blob handling interface

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
 */


/*==========================================================================*
*  INCLUDES                                                                 *
*===========================================================================*/

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"
#include "KernelCommon/Kernel_Trace.hpp"
#include "Logging/Log_ActionBlobCopy.hpp"
#include "Logging/Log_ActionBlobCopySame.hpp"
#include "Logging/Log_ActionBlobExpand.hpp"
#include "Logging/Log_ActionBlobTruncate.hpp"
#include "Logging/Log_ActionBlobUpdate.hpp"
#include "Logging/Log_ActionCreateDropFile.hpp"
#include "gsp00.h"
#include "ggg00.h"
#include "hbd01.h"
#include "hbd01_1.h" // b01niltree_id
#include "hbd05.h"
#include "ggg92.h"  // required for hkb50.h
#include "hkb50.h"
#include "hkb431.h" // check of PASCAL header vkb431

#ifdef SAPDB_SLOW
#include "hta01.h"
#endif


/*==========================================================================*
* LOCAL FUNCTIONS                                                           *
*===========================================================================*/

static inline void BuildAuxiliaryBlobId (const tgg00_TransContext &TransContext,
                                         tgg00_FileId             &AuxBlobId)
{
    AuxBlobId = b01niltree_id;
    
    AuxBlobId.fileType_gg00().clear();
    AuxBlobId.fileType_gg00().addElement (ftsByteStr_egg00);
    AuxBlobId.fileType_gg00().addElement (ftsConcurrent_egg00);
    
    AuxBlobId.fileTfn_gg00()     = tgg00_Tfn::fromConst (tfnAux_egg00);
    AuxBlobId.fileTransId_gg00() = TransContext.trWriteTransId_gg00;
    
    const SAPDB_Bool1 bPerm = SAPDB_TRUE;
    k50aux_fn_no (TransContext.trIndex_gg00, bPerm, AuxBlobId.fileAuxCnt_gg00());
}

//----------------------------------------------------------------------------

static inline void WriteBeforeImageUsingAuxBlob
    (tgg00_TransContext &TransContext,
     tgg00_Surrogate    &tableId,
     tgg00_FileId       &BlobId,
     SAPDB_Int4         BlobVersion,
     SAPDB_Int4         OffsetOverwriteArea,
     SAPDB_Int4         LengthOverwriteArea,
     SAPDB_Int4         LenTruncatedBlob)
{
	SAPDBTRACE_ROUTINE_DEBUG ("WriteBeforeImageUsingAuxBlob", LogAction_Trace, 5);

    KERNEL_TRACE_BASIS_ERROR(TransContext.trError_gg00, LogAction_Trace, 5);
    
    #ifdef SAPDB_SLOW
    t01treeid (kb_log, "BlobId      ", BlobId);
    t01p2int4 (kb_log, "BlobVersion ", BlobVersion
        ,              "OffOverwrite", OffsetOverwriteArea);
    t01p2int4 (kb_log, "LenOverwrite", LengthOverwriteArea
        ,              "LenTruncated", LenTruncatedBlob);
    #endif

    // before image: copy old contents of destination blob to a before image file

    tgg00_FileId AuxBlobId;
    
    BuildAuxiliaryBlobId (TransContext, AuxBlobId);
    
    // create an action that will restore the overwritten image of the blob in case of undo
    // restoring is done by copying the original bytes from auxiliary blob to the blob
    
    Log_ActionBlobCopy BeforeImage ( tableId,                   // primary table for statistics
                                     AuxBlobId.fileName_gg00(), // Source
                                     BlobId.fileName_gg00(),    // Destin
                                     BlobVersion,               // DestinVersion
                                     LengthOverwriteArea,       // CopyLength
                                     0,                         // SourceOffset
                                     OffsetOverwriteArea,       // DestinOffset
                                     LenTruncatedBlob );
    
    // the before image entry has to be written before the auxiliary blob is created
    BeforeImage.WriteBeforeImage (TransContext);
    
    if (e_ok != TransContext.trError_gg00)
        return;
    
    // in case of rollback the auxiliary blob will be destoyed by BeforeImage.Execute
    // in case of commit   the auxiliary blob will be destoyed by PrefixDestoyFiles
    b01bcreate_bytestr (TransContext, AuxBlobId);
    
    if (e_ok != TransContext.trError_gg00)
        return;
    
    AuxBlobId.fileBdUse_gg00().clear();
    
    tgg00_Surrogate dummySurrogate;
    SAPDB_MemMoveNoCheck (dummySurrogate, cgg_zero_id, SURROGATE_MXGG00);

    // copy the original bytes contained in the overwrite area of the blob to the auxiliary blob
    b05copy_bytestr ( TransContext,
                      BlobId,    OffsetOverwriteArea+1,
                      AuxBlobId, 1,
                      dummySurrogate,
                      LengthOverwriteArea,
                      BlobVersion );
}


/*==========================================================================*
* PASCAL INTERFACE                                                          *
*===========================================================================*/

externPascal void kb431CopyBlob (
    tgg00_TransContext    VAR_VALUE_REF  TransContext,
    tgg00_Surrogate       VAR_ARRAY_REF  DestinTabId,
    tgg00_FileId          VAR_VALUE_REF  SourceBlob,
    tgg00_FileId          VAR_VALUE_REF  DestinBlob,
    tsp00_Int4                           OldDestinVersion,
    tsp00_Int4                           OldDestinLength,
    tsp00_Int4                           SourceOffset,
    tsp00_Int4                           DestinOffset,
    tsp00_Int4                           CopyLength)
{
	SAPDBTRACE_ROUTINE_DEBUG ("kb431CopyBlob", LogAction_Trace, 5);

    KERNEL_TRACE_BASIS_ERROR(TransContext.trError_gg00, LogAction_Trace, 5);
    
    #ifdef SAPDB_SLOW
    t01surrogate (kb_log, "DestinTabId ", DestinTabId);
    t01treeid    (kb_log, "SourceBlob  ", SourceBlob);
    t01treeid    (kb_log, "DestinBlob  ", DestinBlob);
    t01p2int4    (kb_log, "OldDestVers ", OldDestinVersion, "SourceOffset", SourceOffset);
    t01p2int4    (kb_log, "OldDestLen  ", OldDestinLength,  "DestinOffset", DestinOffset);
    t01int4      (kb_log, "CopyLength  ", CopyLength);
    #endif

    const SAPDB_Int4 NewDestinVersion = OldDestinVersion < SAPDB_MAX_INT4 && OldDestinVersion > 0
                                        ? OldDestinVersion + 1
                                        : 1;
    
    const SAPDB_Int4 LengthOverwriteArea = DestinOffset + CopyLength <= OldDestinLength
                                           ? CopyLength
                                           : OldDestinLength - DestinOffset;
    
    if ( LengthOverwriteArea > 0 )
    {
        // before image: copy old contents of destination blob to an auxiliary blob        
        const SAPDB_Int4 LenOldTruncatedBlob = LengthOverwriteArea < CopyLength ? OldDestinLength : 0;

        WriteBeforeImageUsingAuxBlob ( TransContext, DestinTabId, DestinBlob, OldDestinVersion,
                                       DestinOffset, LengthOverwriteArea, LenOldTruncatedBlob);
    }
    else
    {
        // before image: truncation of destination blob
        
        Log_ActionBlobTruncate BeforeImage (DestinTabId, DestinBlob.fileName_gg00(), OldDestinVersion, OldDestinLength);
        
        BeforeImage.WriteBeforeImage (TransContext);
    }
    
    if (e_ok != TransContext.trError_gg00)
        return;
    
    if ( SourceBlob.fileTabId_gg00() != DestinBlob.fileTabId_gg00() )
    {
        Log_ActionBlobCopy AfterImage ( DestinTabId,
                                        SourceBlob.fileName_gg00(),
                                        DestinBlob.fileName_gg00(),
                                        NewDestinVersion,
                                        CopyLength,
                                        SourceOffset,
                                        DestinOffset,
                                        0);         // without any truncation (redo: DestinBlob may be expanded)
        
        AfterImage.ExecuteAction (TransContext, &SourceBlob, &DestinBlob);
        
        if (e_ok != TransContext.trError_gg00)
            return;
        
        AfterImage.WriteAfterImage (TransContext);
    }
    else
    {
        tgg00_FileId AuxBlobId;
        
        BuildAuxiliaryBlobId (TransContext, AuxBlobId);
        
        Log_ActionDropFile BeforeImageAuxBlobId (AuxBlobId.fileName_gg00());
        
        BeforeImageAuxBlobId.WriteBeforeImage (TransContext);
        
        if (e_ok != TransContext.trError_gg00)
            return;
        
        Log_ActionBlobCopySame AfterImage ( DestinTabId,
                                            DestinBlob.fileName_gg00(),
                                            AuxBlobId.fileName_gg00(),
                                            NewDestinVersion,
                                            CopyLength,
                                            SourceOffset,
                                            DestinOffset );
        
        AfterImage.ExecuteAction (TransContext, DestinBlob, AuxBlobId);
        
        if (e_ok != TransContext.trError_gg00)
            return;
        
        AfterImage.WriteAfterImage (TransContext);
    }
}

//----------------------------------------------------------------------------

externPascal void kb431TruncateBlob (
    tgg00_TransContext    VAR_VALUE_REF  TransContext,
    tgg00_Surrogate       VAR_ARRAY_REF  tableId,
    tgg00_FileId          VAR_VALUE_REF  Blob,
    tsp00_Int4                           BlobVersion,
    tsp00_Int4                           OldBlobLength,
    tsp00_Int4                           NewTruncatedBlobLen)
{    
	SAPDBTRACE_ROUTINE_DEBUG ("kb431TruncateBlob", LogAction_Trace, 5);

    KERNEL_TRACE_BASIS_ERROR(TransContext.trError_gg00, LogAction_Trace, 5);
    
    #ifdef SAPDB_SLOW
    t01treeid (kb_log, "Blob        ", Blob);
    t01int4   (kb_log, "BlobVersion ", BlobVersion);
    t01p2int4 (kb_log, "OldBlobLen  ", OldBlobLength, "NewTruncated", NewTruncatedBlobLen);
    #endif

    if (OldBlobLength == NewTruncatedBlobLen) return;

    WriteBeforeImageUsingAuxBlob ( TransContext,
                                   tableId,
                                   Blob, BlobVersion,
                                   NewTruncatedBlobLen,                 // OffsetOverwriteArea
                                   OldBlobLength - NewTruncatedBlobLen, // LengthOverwriteArea
                                   0 );                                 // without any truncation (undo: the blob is expanded)
    
    if (e_ok != TransContext.trError_gg00)
        return;
    
    Log_ActionBlobTruncate AfterImage (tableId, Blob.fileName_gg00(), BlobVersion, NewTruncatedBlobLen);
    
    AfterImage.ExecuteAction (TransContext, &Blob); 
    
    if (e_ok != TransContext.trError_gg00)
        return;
    
    AfterImage.WriteAfterImage (TransContext);
}

//----------------------------------------------------------------------------

externPascal void kb431UpdateBlob (
    tgg00_TransContext    VAR_VALUE_REF  TransContext,
    tgg00_Surrogate       VAR_ARRAY_REF  tableId,
    tgg00_FileId          VAR_VALUE_REF  Blob,
    tsp00_Int4                           BlobVersion,
    tsp00_Int4                           OldBlobLength,
    tsp00_Int4                           InsertOffset,
    tsp00_MoveObjPtr                     pBuf,
    tsp00_Int4                           BufOffset,
    tsp00_Int4                           BufLength)
{
	SAPDBTRACE_ROUTINE_DEBUG ("kb431UpdateBlob", LogAction_Trace, 5);

    KERNEL_TRACE_BASIS_ERROR(TransContext.trError_gg00, LogAction_Trace, 5);
    
    #ifdef SAPDB_SLOW
    t01treeid  (kb_log, "Blob        ", Blob);
    t01int4    (kb_log, "BlobVersion ", BlobVersion);
    t01p2int4  (kb_log, "OldBlobLen  ", OldBlobLength, "InsertOffset", InsertOffset);
    t01p2int4  (kb_log, "BufOffset   ", BufOffset,     "BufLength   ", BufLength);
    t01moveobj (kb_log, pBuf, BufOffset+1, BufOffset+BufLength);
    #endif

    if (BufLength <= 0) return;
    
    const SAPDB_Int4 LengthOverwriteArea = InsertOffset + BufLength <= OldBlobLength
                                           ? BufLength
                                           : OldBlobLength - InsertOffset;
    
    if ( LengthOverwriteArea > 0 )
    {
        // before image: copy old contents of blob to an auxiliary blob 
        
        const SAPDB_Int4 LenOldTruncatedBlob = InsertOffset + BufLength <= OldBlobLength
                                               ? 0
                                               : OldBlobLength;
        
        WriteBeforeImageUsingAuxBlob ( TransContext,
                                       tableId,
                                       Blob, BlobVersion,
                                       InsertOffset, LengthOverwriteArea,
                                       LenOldTruncatedBlob);
    }
    else
    {
        // before image: truncation of destination blob
        
        Log_ActionBlobTruncate BeforeImage (tableId, Blob.fileName_gg00(), BlobVersion, OldBlobLength);
        
        BeforeImage.WriteBeforeImage (TransContext);
    }
    
    if (e_ok != TransContext.trError_gg00)
        return;
    
    Log_ActionBlobUpdate AfterImage ( tableId, Blob.fileName_gg00(), InsertOffset,
                                      (REINTERPRET_CAST (SAPDB_Byte*, pBuf))+BufOffset, BufLength);
    
    AfterImage.ExecuteAction (TransContext, &Blob); 
    
    if (e_ok != TransContext.trError_gg00)
        return;
    
    AfterImage.WriteAfterImage (TransContext);
}
