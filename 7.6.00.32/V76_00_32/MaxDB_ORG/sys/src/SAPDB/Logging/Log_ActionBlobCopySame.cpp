/*!
   @file           Log_ActionBlobCopySame.cpp
   @author         UweH
   @ingroup        Logging
   @brief          defines a class to handle log entries for binary large objects

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
 */


#include "Logging/Log_ActionBlobCopySame.hpp"

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp" // Routine trace macros
#include "KernelCommon/Kernel_Trace.hpp"
#include "Logging/Log_ActionFileHandling.hpp"  // Log_GetFileId
#include "Logging/Log_ReadWriteActionImage.hpp"

#include "ggg00.h"
#include "hbd01.h"
#include "hbd05.h"

Log_ActionBlobCopySame::Log_ActionBlobCopySame ( const tgg00_Surrogate &TableId,
                                                 const tgg00_Filename  &BlobFilename,
                                                 const tgg00_Filename  &AuxBlobFilename,
                                                 SAPDB_Int4             NewBlobVersion,
                                                 SAPDB_UInt4            CopyLength,
                                                 SAPDB_UInt4            SourceOffset,
                                                 SAPDB_UInt4            DestinOffset)
: Log_ReadWriteActionImage (Log_CopySameBlob)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionBlobCopySame::Log_ActionBlobCopySame", LogAction_Trace, 5);

    m_Head.TableId         = TableId;
    m_Head.BlobFilename    = BlobFilename;
    m_Head.AuxBlobFilename = AuxBlobFilename;
    m_Head.NewBlobVersion  = NewBlobVersion;
    m_Head.CopyLength      = CopyLength;
    m_Head.SourceOffset    = SourceOffset;
    m_Head.DestinOffset    = DestinOffset;

    SAPDBTRACE_WRITELN (LogAction_Trace, 6, "NewBlobVers: " << NewBlobVersion <<
                                      "CopyLength: " << CopyLength);
    SAPDBTRACE_WRITELN (LogAction_Trace, 6, "SourceOffset: " << SourceOffset <<
                                      "DestinOffset: " << DestinOffset);
}

//---------------------------------------------------------------------------

void Log_ActionBlobCopySame::Execute (tgg00_TransContext &TransContext) const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionBlobCopySame::Execute", LogAction_Trace, 5);

    KERNEL_TRACE_BASIS_ERROR (TransContext.trError_gg00, LogAction_Trace, 5);

    if (e_ok != TransContext.trError_gg00) return;

    tgg00_FileId  BlobId;
    tgg00_FileId  AuxBlobId;
    tgg00_FileId *pDummyFileId;
    SAPDB_Int4    CurrBlobVersion      = 0;
    SAPDB_Int4    CurrAuxBlobVersion   = 0;
    SAPDB_Int4    CurrAuxBlobLength    = 0;
    SAPDB_Int4    CurrAuxBlobPages     = 0;
    SAPDB_UInt4   AbsCurrAuxBlobLength = 0;


    pDummyFileId = Log_GetFileId (m_Head.BlobFilename,    BlobId,    NULL);
    pDummyFileId = Log_GetFileId (m_Head.AuxBlobFilename, AuxBlobId, NULL);

    b05get_str_version (TransContext, BlobId, CurrBlobVersion);

    if (e_ok != TransContext.trError_gg00) return;

    if (CurrBlobVersion != m_Head.NewBlobVersion)
    {
        b01bcreate_bytestr (TransContext, AuxBlobId);

        if (e_duplicate_filename == TransContext.trError_gg00)
        {
            b05length_bytestr (TransContext, AuxBlobId, CurrAuxBlobLength, 
                CurrAuxBlobPages ,CurrAuxBlobVersion);
        }

        if (e_ok != TransContext.trError_gg00) return;

        AbsCurrAuxBlobLength = abs(CurrAuxBlobLength);

        if (AbsCurrAuxBlobLength < m_Head.CopyLength)
        {
            b05copy_bytestr (TransContext,
                             BlobId, m_Head.SourceOffset+1,
                             AuxBlobId, 1,
                             m_Head.TableId,
                             m_Head.CopyLength,
                             m_Head.NewBlobVersion);

            if (e_ok != TransContext.trError_gg00) return;
        }

        b05copy_bytestr (TransContext,
                         AuxBlobId, 1,
                         BlobId, m_Head.DestinOffset+1,
                         m_Head.TableId,
                         m_Head.CopyLength,
                         m_Head.NewBlobVersion);

        if (e_ok != TransContext.trError_gg00) return;
    }

    b01pdestroy_perm_file (TransContext, AuxBlobId);

    if (e_file_not_found == TransContext.trError_gg00) TransContext.trError_gg00 = e_ok;
}

//---------------------------------------------------------------------------

void Log_ActionBlobCopySame::ExecuteAction (tgg00_TransContext &TransContext,
        tgg00_FileId       &BlobId,
        tgg00_FileId       &AuxBlobId) const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionBlobCopySame::ExecuteAction", LogAction_Trace, 5);

    KERNEL_TRACE_BASIS_ERROR (TransContext.trError_gg00, LogAction_Trace, 5);

    b01bcreate_bytestr (TransContext, AuxBlobId);

    if (e_ok != TransContext.trError_gg00) return;

    b05copy_bytestr (TransContext,
                     BlobId, m_Head.SourceOffset+1,
                     AuxBlobId, 1,
                     m_Head.TableId,
                     m_Head.CopyLength,
                     m_Head.NewBlobVersion);

    if (e_ok != TransContext.trError_gg00) return;

    b05copy_bytestr (TransContext,
                     AuxBlobId, 1,
                     BlobId, m_Head.DestinOffset+1,
                     m_Head.TableId,
                     m_Head.CopyLength,
                     m_Head.NewBlobVersion);

    if (e_ok != TransContext.trError_gg00) return;

    b01pdestroy_perm_file (TransContext, AuxBlobId);
}
