/*!
  @file           Log_ActionBlobExpand.cpp
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


#include "Logging/Log_ActionBlobExpand.hpp"

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp" // Routine trace macros
#include "KernelCommon/Kernel_Trace.hpp"
#include "Logging/Log_ActionFileHandling.hpp"  // Log_GetFileId
#include "Logging/Log_ReadWriteActionImage.hpp"

#include "ggg00.h"
#include "hbd01.h"
#include "hbd05.h"


Log_ActionBlobExpand::Log_ActionBlobExpand (const tgg00_Surrogate &TableId,
                                            const tgg00_Filename  &BlobFilename,
                                            SAPDB_UInt4            NewBlobLength,
                                            SAPDB_Byte             ExpandByte)
: Log_ReadWriteActionImage (Log_ExpandBlob)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionBlobExpand::Log_ActionBlobExpand", LogAction_Trace, 5);

    m_Head.TableId       = TableId;
    m_Head.BlobFilename  = BlobFilename;
    m_Head.NewBlobLength = NewBlobLength;
    m_Head.ExpandByte    = ExpandByte;

    SAPDBTRACE_WRITELN (LogAction_Trace, 6, "NewBlobLength: " << NewBlobLength << ", ExpandByte: " << ExpandByte);
}

//---------------------------------------------------------------------------

void Log_ActionBlobExpand::ExecuteAction (tgg00_TransContext &TransContext,
        tgg00_FileId       *pCommandBlobFileId) const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionBlobExpand::ExecuteAction", LogAction_Trace, 5);

    KERNEL_TRACE_BASIS_ERROR (TransContext.trError_gg00, LogAction_Trace, 5);

    if (e_ok != TransContext.trError_gg00) return;

    tgg00_FileId  RedoBlobFileId;
    tgg00_FileId *pExecuteBlobFileId;

    pExecuteBlobFileId =
        Log_GetFileId (m_Head.BlobFilename, RedoBlobFileId, pCommandBlobFileId);

    b05expand_bytestr (TransContext, *pExecuteBlobFileId, m_Head.NewBlobLength,
                       m_Head.ExpandByte, NIL_STR_VERSION_GG00);
}
