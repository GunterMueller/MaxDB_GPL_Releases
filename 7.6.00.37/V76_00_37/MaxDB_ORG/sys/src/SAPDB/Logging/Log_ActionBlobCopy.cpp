/*!
  @file           Log_ActionBlobCopy.cpp
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

#include "Logging/Log_ActionBlobCopy.hpp"

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp" // Routine trace macros
#include "KernelCommon/Kernel_Trace.hpp"
#include "Logging/Log_ActionFileHandling.hpp"  // Log_GetFileId
#include "Logging/Log_ReadWriteActionImage.hpp"

#include "ggg00.h"
#include "hbd01.h"
#include "hbd05.h"

//---------------------------------------------------------------------------

Log_ActionBlobCopy::Log_ActionBlobCopy (const tgg00_Surrogate &TableId,
                                        const tgg00_Filename  &SourceFilename,
                                        const tgg00_Filename  &DestinFilename,
                                        SAPDB_Int4             DestinVersion,
                                        SAPDB_UInt4            CopyLength,
                                        SAPDB_UInt4            SourceOffset,
                                        SAPDB_UInt4            DestinOffset,
                                        SAPDB_UInt4            NewTruncatedDestinLen)
: Log_ReadWriteActionImage (Log_CopyBlob)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionBlobCopy::Log_ActionBlobCopy", LogAction_Trace, 5);

    m_Head.TableId               = TableId;
    m_Head.SourceFilename        = SourceFilename;
    m_Head.DestinFilename        = DestinFilename;
    m_Head.DestinVersion         = DestinVersion;
    m_Head.CopyLength            = CopyLength;
    m_Head.SourceOffset          = SourceOffset;
    m_Head.DestinOffset          = DestinOffset;
    m_Head.NewTruncatedDestinLen = NewTruncatedDestinLen;

    SAPDBTRACE_WRITELN (LogAction_Trace, 6, "DestinVers: " << DestinVersion << ", CopyLength: " << CopyLength << ", SourceOffset: " << SourceOffset);
    SAPDBTRACE_WRITELN (LogAction_Trace, 6, "DestinOffset" << DestinOffset <<  ", NewTruncLen " << NewTruncatedDestinLen);
}

//---------------------------------------------------------------------------

void Log_ActionBlobCopy::ExecuteAction (tgg00_TransContext &TransContext,
                                        tgg00_FileId       *pSourceCommandFileId,
                                        tgg00_FileId       *pDestinCommandFileId) const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionBlobCopy::ExecuteAction", LogAction_Trace, 5);

    KERNEL_TRACE_BASIS_ERROR (TransContext.trError_gg00, LogAction_Trace, 5);

    if (e_ok != TransContext.trError_gg00)
        return;

    tgg00_FileId  SourceRedoFileId;
    tgg00_FileId  DestinRedoFileId;
    tgg00_FileId *pSourceExecuteFileId;
    tgg00_FileId *pDestinExecuteFileId;

    pSourceExecuteFileId = Log_GetFileId (m_Head.SourceFilename, SourceRedoFileId, pSourceCommandFileId);
    pDestinExecuteFileId = Log_GetFileId (m_Head.DestinFilename, DestinRedoFileId, pDestinCommandFileId);

    b05copy_bytestr (TransContext,
                     *pSourceExecuteFileId, m_Head.SourceOffset+1,
                     *pDestinExecuteFileId, m_Head.DestinOffset+1,
                     m_Head.TableId,
                     m_Head.CopyLength,
                     m_Head.DestinVersion);

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
                TransContext.trError_gg00 = e_ok;             // auxiliary file already dropped
            else
            {
                if (e_ok == TransContext.trError_gg00)  // file not found was caused by
                {                                       // missing destination blob
                    b01pdestroy_perm_file (TransContext, *pSourceExecuteFileId);
                    if (e_ok == TransContext.trError_gg00)
                        TransContext.trError_gg00 = e_file_not_found;
                }
            }
        }
    }

    if (e_ok != TransContext.trError_gg00)
        return;

    if ( m_Head.NewTruncatedDestinLen > 0 )
        b05trunc_bytestr ( TransContext,
                           *pDestinExecuteFileId,
                           m_Head.TableId,
                           m_Head.NewTruncatedDestinLen,
                           m_Head.DestinVersion );
}
