
/*!**************************************************************************

    module      : Log_ActionCopyUpdBlob75.cpp
    -------------------------------------------------------------------------

    author      : JuergenA
    responsible : UweH

    special area: Logging
    description : defines a class to handle log entries for binary large objects

    last changed: 2001-03-13

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

#include "Logging/Log_ActionCopyUpdBlob75.hpp"

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp" // Routine trace macros
#include "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"
#include "KernelCommon/Kernel_Trace.hpp"
#include "Logging/Log_ActionFileHandling.hpp"  // Log_GetFileId
#include "Logging/Log_ReadWriteActionImage.hpp"

#include "ggg00.h"
#include "hbd01.h"
#include "hbd05.h"

/*===========================================================================*
*  METHODS of Log_ActionCopyBlob75                                             *
*============================================================================*/

Log_ActionCopyBlob75::Log_ActionCopyBlob75 (const tgg00_Filename &SourceFilename,
                                        const tgg00_Filename &DestinFilename,
                                        SAPDB_Int4            DestinVersion,
                                        SAPDB_UInt4           CopyLength,
                                        SAPDB_UInt4           SourceOffset,
                                        SAPDB_UInt4           DestinOffset,
                                        SAPDB_UInt4           NewTruncatedDestinLen):
        Log_ReadWriteActionImage (Log_CopyBlob)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionCopyBlob75::Log_ActionCopyBlob75", LogAction_Trace, 5);

    m_Entry.SourceFilename        = SourceFilename;
    m_Entry.DestinFilename        = DestinFilename;
    m_Entry.DestinVersion         = DestinVersion;
    m_Entry.CopyLength            = CopyLength;
    m_Entry.SourceOffset          = SourceOffset;
    m_Entry.DestinOffset          = DestinOffset;
    m_Entry.NewTruncatedDestinLen = NewTruncatedDestinLen;

    SAPDBTRACE_WRITELN (LogAction_Trace, 6, "DestinVers: " << DestinVersion <<
                                      ", CopyLength: " << CopyLength <<
                                      ", SourceOffset: " << SourceOffset);
    SAPDBTRACE_WRITELN (LogAction_Trace, 6, "DestinOffset" << DestinOffset <<
                                      "NewTruncLen " << NewTruncatedDestinLen);
}

//---------------------------------------------------------------------------

void Log_ActionCopyBlob75::ExecuteAction (tgg00_TransContext &TransContext,
                                        tgg00_FileId       *pSourceCommandFileId,
                                        tgg00_FileId       *pDestinCommandFileId) const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionCopyBlob75::ExecuteAction", LogAction_Trace, 5);

    KERNEL_TRACE_BASIS_ERROR (TransContext.trError_gg00, LogAction_Trace, 5);

    if (e_ok != TransContext.trError_gg00) return;

    tgg00_FileId  SourceRedoFileId;
    tgg00_FileId  DestinRedoFileId;
    tgg00_FileId *pSourceExecuteFileId;
    tgg00_FileId *pDestinExecuteFileId;

    pSourceExecuteFileId =
        Log_GetFileId (m_Entry.SourceFilename, SourceRedoFileId, pSourceCommandFileId);

    pDestinExecuteFileId =
        Log_GetFileId (m_Entry.DestinFilename, DestinRedoFileId, pDestinCommandFileId);

    tgg00_Surrogate dummySurrogate;
    SAPDB_MemMoveNoCheck (dummySurrogate, cgg_zero_id, sizeof(dummySurrogate));

    b05copy_bytestr (TransContext,
                     *pSourceExecuteFileId, m_Entry.SourceOffset+1,
                     *pDestinExecuteFileId, m_Entry.DestinOffset+1,
                     dummySurrogate,
                     m_Entry.CopyLength,
                     m_Entry.DestinVersion);

    if (tfnAux_egg00 == pSourceExecuteFileId->fileTfn_gg00()) // is undo
    {
        if (e_ok == TransContext.trError_gg00)
        {
            b01pdestroy_perm_file (TransContext, *pSourceExecuteFileId);
        }
        else if (e_file_not_found == TransContext.trError_gg00)
        {
            TransContext.trError_gg00 = e_ok;

            b01filestate (TransContext, *pSourceExecuteFileId);

            if (e_file_not_found == TransContext.trError_gg00)
            {
                TransContext.trError_gg00 = e_ok;             // auxiliary file already dropped
            }
            else
            {
                if (e_ok == TransContext.trError_gg00)  // file not found was caused by
                {                                       // missing destination blob
                    b01pdestroy_perm_file (TransContext, *pSourceExecuteFileId);
                    if (e_ok == TransContext.trError_gg00)
                    {
                        TransContext.trError_gg00 = e_file_not_found;
                    }
                }
            }
        }
    }

    if (e_ok != TransContext.trError_gg00) return;

    if (m_Entry.NewTruncatedDestinLen > 0)
    {
        b05trunc_bytestr (TransContext, *pDestinExecuteFileId,
                          dummySurrogate, m_Entry.NewTruncatedDestinLen, m_Entry.DestinVersion);
    }
}

//---------------------------------------------------------------------------

bool Log_ActionCopyBlob75::ReadImagePersistent (Log_ActionImageReader &ImageReader,
                                              SAPDBErr_MessageList  &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionCopyBlob75::ReadImagePersistent", LogAction_Trace, 5);

    return ImageReader.CopyImage ( REINTERPRET_CAST (SAPDB_Byte*, &(m_Entry)),
                                   sizeof(m_Entry),
                                   errlist );
}

/*===========================================================================*
*  METHODS of Log_ActionCopySameBlob75                                             *
*============================================================================*/

Log_ActionCopySameBlob75::Log_ActionCopySameBlob75 (const tgg00_Filename &BlobFilename,
        const tgg00_Filename &AuxBlobFilename,
        SAPDB_Int4            NewBlobVersion,
        SAPDB_UInt4           CopyLength,
        SAPDB_UInt4           SourceOffset,
        SAPDB_UInt4           DestinOffset):
        Log_ReadWriteActionImage (Log_CopySameBlob)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionCopySameBlob75::Log_ActionCopySameBlob75", LogAction_Trace, 5);

    m_Entry.BlobFilename    = BlobFilename;
    m_Entry.AuxBlobFilename = AuxBlobFilename;
    m_Entry.NewBlobVersion  = NewBlobVersion;
    m_Entry.CopyLength      = CopyLength;
    m_Entry.SourceOffset    = SourceOffset;
    m_Entry.DestinOffset    = DestinOffset;

    SAPDBTRACE_WRITELN (LogAction_Trace, 6, "NewBlobVers: " << NewBlobVersion <<
                                      "CopyLength: " << CopyLength);
    SAPDBTRACE_WRITELN (LogAction_Trace, 6, "SourceOffset: " << SourceOffset <<
                                      "DestinOffset: " << DestinOffset);
}

//---------------------------------------------------------------------------

void Log_ActionCopySameBlob75::Execute (tgg00_TransContext &TransContext) const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionCopySameBlob75::Execute", LogAction_Trace, 5);

    KERNEL_TRACE_BASIS_ERROR (TransContext.trError_gg00, LogAction_Trace, 5);

    if (e_ok != TransContext.trError_gg00) return;

    tgg00_FileId  BlobId;
    tgg00_FileId  AuxBlobId;
    SAPDB_Int4    CurrBlobVersion      = 0;
    SAPDB_Int4    CurrAuxBlobVersion   = 0;
    SAPDB_Int4    CurrAuxBlobLength    = 0;
    SAPDB_Int4    CurrAuxBlobPages     = 0;
    SAPDB_UInt4   AbsCurrAuxBlobLength = 0;


    Log_GetFileId (m_Entry.BlobFilename,    BlobId,    NULL);
    Log_GetFileId (m_Entry.AuxBlobFilename, AuxBlobId, NULL);

    b05get_str_version (TransContext, BlobId, CurrBlobVersion);

    if (e_ok != TransContext.trError_gg00) return;

    if (CurrBlobVersion != m_Entry.NewBlobVersion)
    {
        b01bcreate_bytestr (TransContext, AuxBlobId);

        if (e_duplicate_filename == TransContext.trError_gg00)
        {
            b05length_bytestr (TransContext, AuxBlobId, CurrAuxBlobLength, 
                CurrAuxBlobPages ,CurrAuxBlobVersion);
        }

        if (e_ok != TransContext.trError_gg00) return;

        AbsCurrAuxBlobLength = abs(CurrAuxBlobLength);

        tgg00_Surrogate dummySurrogate;
        SAPDB_MemMoveNoCheck (dummySurrogate, cgg_zero_id, sizeof(dummySurrogate));


        if (AbsCurrAuxBlobLength < m_Entry.CopyLength)
        {
            b05copy_bytestr (TransContext,
                             BlobId, m_Entry.SourceOffset+1,
                             AuxBlobId, 1,
                             dummySurrogate,
                             m_Entry.CopyLength,
                             m_Entry.NewBlobVersion);

            if (e_ok != TransContext.trError_gg00) return;
        }

        b05copy_bytestr (TransContext,
                         AuxBlobId, 1,
                         BlobId, m_Entry.DestinOffset+1,
                         dummySurrogate,
                         m_Entry.CopyLength,
                         m_Entry.NewBlobVersion);

        if (e_ok != TransContext.trError_gg00) return;
    }

    b01pdestroy_perm_file (TransContext, AuxBlobId);

    if (e_file_not_found == TransContext.trError_gg00) TransContext.trError_gg00 = e_ok;
}

//---------------------------------------------------------------------------

void Log_ActionCopySameBlob75::ExecuteAction (tgg00_TransContext &TransContext,
        tgg00_FileId       &BlobId,
        tgg00_FileId       &AuxBlobId) const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionCopySameBlob75::ExecuteAction", LogAction_Trace, 5);

    KERNEL_TRACE_BASIS_ERROR (TransContext.trError_gg00, LogAction_Trace, 5);

    b01bcreate_bytestr (TransContext, AuxBlobId);

    if (e_ok != TransContext.trError_gg00) return;

    tgg00_Surrogate dummySurrogate;
    SAPDB_MemMoveNoCheck (dummySurrogate, cgg_zero_id, sizeof(dummySurrogate));

    b05copy_bytestr (TransContext,
                     BlobId, m_Entry.SourceOffset+1,
                     AuxBlobId, 1,
                     dummySurrogate,
                     m_Entry.CopyLength,
                     m_Entry.NewBlobVersion);

    if (e_ok != TransContext.trError_gg00) return;

    b05copy_bytestr (TransContext,
                     AuxBlobId, 1,
                     BlobId, m_Entry.DestinOffset+1,
                     dummySurrogate,
                     m_Entry.CopyLength,
                     m_Entry.NewBlobVersion);

    if (e_ok != TransContext.trError_gg00) return;

    b01pdestroy_perm_file (TransContext, AuxBlobId);
}

//---------------------------------------------------------------------------

bool Log_ActionCopySameBlob75::ReadImagePersistent (Log_ActionImageReader &ImageReader,
                                                  SAPDBErr_MessageList  &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionCopySameBlob75::ReadImagePersistent", LogAction_Trace, 5);

    return ImageReader.CopyImage (REINTERPRET_CAST (SAPDB_Byte*, &(m_Entry)),
                                  sizeof(m_Entry),
                                  errlist );
}


/*===========================================================================*
*  METHODS of Log_ActionExpandBlob75                                           *
*============================================================================*/

Log_ActionExpandBlob75::Log_ActionExpandBlob75 (const tgg00_Filename &BlobFilename,
        SAPDB_UInt4           NewBlobLength,
        SAPDB_Byte            ExpandByte):
        Log_ReadWriteActionImage (Log_ExpandBlob)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionExpandBlob75::Log_ActionExpandBlob75", LogAction_Trace, 5);

    m_Entry.BlobFilename  = BlobFilename;
    m_Entry.NewBlobLength = NewBlobLength;
    m_Entry.ExpandByte    = ExpandByte;

    SAPDBTRACE_WRITELN (LogAction_Trace, 6, "NewBlobLength: " << NewBlobLength <<
                                      ", ExpandByte: " << ExpandByte);
}

//---------------------------------------------------------------------------

void Log_ActionExpandBlob75::ExecuteAction (tgg00_TransContext &TransContext,
        tgg00_FileId       *pCommandBlobFileId) const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionExpandBlob75::ExecuteAction", LogAction_Trace, 5);

    KERNEL_TRACE_BASIS_ERROR (TransContext.trError_gg00, LogAction_Trace, 5);

    if (e_ok != TransContext.trError_gg00) return;

    tgg00_FileId  RedoBlobFileId;
    tgg00_FileId *pExecuteBlobFileId;

    pExecuteBlobFileId =
        Log_GetFileId (m_Entry.BlobFilename, RedoBlobFileId, pCommandBlobFileId);

    b05expand_bytestr (TransContext, *pExecuteBlobFileId, m_Entry.NewBlobLength,
                       m_Entry.ExpandByte, NIL_STR_VERSION_GG00);
}

//---------------------------------------------------------------------------

bool Log_ActionExpandBlob75::ReadImagePersistent (Log_ActionImageReader &ImageReader,
                                                SAPDBErr_MessageList  &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionExpandBlob75::ReadImagePersistent", LogAction_Trace, 5);

    return ImageReader.CopyImage (REINTERPRET_CAST (SAPDB_Byte*, &(m_Entry)),
                                  sizeof(m_Entry),
                                  errlist);
}


/*===========================================================================*
*  METHODS of Log_ActionTruncateBlob75                                           *
*============================================================================*/

Log_ActionTruncateBlob75::Log_ActionTruncateBlob75 (const tgg00_Filename &BlobFilename,
        SAPDB_Int4            BlobVersion,
        SAPDB_UInt4           NewTruncatedBlobLen):
        Log_ReadWriteActionImage (Log_TruncateBlob)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionTruncateBlob75::Log_ActionTruncateBlob75", LogAction_Trace, 5);

    m_Entry.BlobFilename        = BlobFilename;
    m_Entry.BlobVersion         = BlobVersion;
    m_Entry.NewTruncatedBlobLen = NewTruncatedBlobLen;

    SAPDBTRACE_WRITELN (LogAction_Trace, 6, "BlobVersion: " << BlobVersion <<
                                      ", NewTruncatedBlobLen: " << NewTruncatedBlobLen);
}

//---------------------------------------------------------------------------

void Log_ActionTruncateBlob75::ExecuteAction (tgg00_TransContext &TransContext,
        tgg00_FileId       *pCommandBlobFileId) const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionTruncateBlob75::ExecuteAction", LogAction_Trace, 5);

    KERNEL_TRACE_BASIS_ERROR (TransContext.trError_gg00, LogAction_Trace, 5);

    if (e_ok != TransContext.trError_gg00) return;

    tgg00_FileId  RedoBlobFileId;
    tgg00_FileId *pExecuteBlobFileId;

    pExecuteBlobFileId =
        Log_GetFileId (m_Entry.BlobFilename, RedoBlobFileId, pCommandBlobFileId);

    tgg00_Surrogate dummySurrogate;
    SAPDB_MemMoveNoCheck (dummySurrogate, cgg_zero_id, sizeof(dummySurrogate));

    b05trunc_bytestr (
        TransContext,
        *pExecuteBlobFileId,
        dummySurrogate,
        m_Entry.NewTruncatedBlobLen,
        m_Entry.BlobVersion);
}

//---------------------------------------------------------------------------

bool Log_ActionTruncateBlob75::ReadImagePersistent (Log_ActionImageReader &ImageReader,
                                                  SAPDBErr_MessageList  &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionTruncateBlob75::ReadImagePersistent", LogAction_Trace, 5);

    return ImageReader.CopyImage (REINTERPRET_CAST (SAPDB_Byte*, &(m_Entry)),
                                  sizeof(m_Entry),
                                  errlist);
}


/*===========================================================================*
*  METHODS of Log_ActionUpdateBlob75                                           *
*============================================================================*/

Log_ActionUpdateBlob75::Log_ActionUpdateBlob75 (const tgg00_Filename &BlobFilename,
        SAPDB_UInt4           InsertOffset,
        const SAPDB_Byte*     pBuf,
        SAPDB_UInt4           BufLength):
        Log_ReadWriteActionImage (Log_UpdateBlob),
        m_pBuf                   (pBuf),
        m_pBufForRead            (NULL),
        m_pBufAllocator          (NULL)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionUpdateBlob75::Log_ActionUpdateBlob75", LogAction_Trace, 5);

    m_EntryHeader.ImageLength  =
        Log_AlignedImageLen (sizeof(m_EntryHeader)) + Log_AlignedImageLen (BufLength);
    m_EntryHeader.BufLength    = BufLength;
    m_EntryHeader.InsertOffset = InsertOffset;
    m_EntryHeader.BlobFilename = BlobFilename;

    SAPDBTRACE_WRITELN (LogAction_Trace, 6, "ImageLength: " << m_EntryHeader.ImageLength <<
                                      ", InsertOffset: " << InsertOffset <<
                                      ", BufLength: " << BufLength);
    SAPDBTRACE_IF      (LogAction_Trace, 6, Kernel_TraceBuffer(pBuf, BufLength));
}
//---------------------------------------------------------------------------

Log_ActionUpdateBlob75::~Log_ActionUpdateBlob75 ()
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionUpdateBlob75::~Log_ActionUpdateBlob75", LogAction_Trace, 5);
    if (NULL != m_pBufAllocator)
    {
        m_pBufAllocator->Deallocate (m_pBufForRead);
        m_pBufForRead = NULL;
    }
}

//---------------------------------------------------------------------------

void Log_ActionUpdateBlob75::ExecuteAction (tgg00_TransContext &TransContext,
        tgg00_FileId       *pCommandBlobFileId) const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionUpdateBlob75::ExecuteAction", LogAction_Trace, 5);

    KERNEL_TRACE_BASIS_ERROR (TransContext.trError_gg00, LogAction_Trace, 5);

    if (e_ok != TransContext.trError_gg00) return;

    tgg00_FileId  RedoBlobFileId;
    tgg00_FileId *pExecuteBlobFileId;

    pExecuteBlobFileId =
        Log_GetFileId (m_EntryHeader.BlobFilename, RedoBlobFileId, pCommandBlobFileId);

    tgg00_Surrogate dummySurrogate;
    SAPDB_MemMoveNoCheck (dummySurrogate, cgg_zero_id, sizeof(dummySurrogate));

    b05write_bytestr (TransContext,
                      *pExecuteBlobFileId,
                      dummySurrogate,
                      m_EntryHeader.InsertOffset+1,
                      m_EntryHeader.BufLength,
                      m_EntryHeader.BufLength,
                      m_pBuf, 1, NIL_STR_VERSION_GG00);
}

//---------------------------------------------------------------------------

bool Log_ActionUpdateBlob75::ReadImagePersistent (Log_ActionImageReader &ImageReader,
                                                SAPDBErr_MessageList  &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionUpdateBlob75::ReadImagePersistent", LogAction_Trace, 5);

    if (! ImageReader.CopyImage (REINTERPRET_CAST (SAPDB_Byte*, &(m_EntryHeader)),
                                 sizeof(m_EntryHeader),
                                 errlist) )
        return false;

    if ( ! ImageReader.PersistentRead (m_pBufForRead,m_EntryHeader.BufLength,m_pBufAllocator,errlist) )
        return false;

    m_pBuf = m_pBufForRead;
    return true;
}

//---------------------------------------------------------------------------

bool Log_ActionUpdateBlob75::WriteImagePersistent (Log_ActionImageWriter &ImageWriter,
                                                 SAPDBErr_MessageList  &errlist)       const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionUpdateBlob75::WriteImagePersistent", LogAction_Trace, 5);

    if ( ! ImageWriter.PersistentWrite (&m_EntryHeader, sizeof (m_EntryHeader), errlist))
        return false;
    return ImageWriter.PersistentWrite (m_pBuf, m_EntryHeader.BufLength, errlist);
}
