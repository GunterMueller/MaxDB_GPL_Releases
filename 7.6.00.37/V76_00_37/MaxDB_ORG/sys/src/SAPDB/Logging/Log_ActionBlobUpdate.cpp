/*!
  @file           Log_ActionBlobUpdate.cpp
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


#include "Logging/Log_ActionBlobUpdate.hpp"

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp" // Routine trace macros
#include "KernelCommon/Kernel_Trace.hpp"
#include "Logging/Log_ActionFileHandling.hpp"  // Log_GetFileId
#include "Logging/Log_ReadWriteActionImage.hpp"

#include "ggg00.h"
#include "hbd01.h"
#include "hbd05.h"

Log_ActionBlobUpdate::Log_ActionBlobUpdate (tgg00_Surrogate &TableId,
                                            const tgg00_Filename  &BlobFilename,
                                            SAPDB_UInt4            InsertOffset,
                                            const SAPDB_Byte*      pBuf,
                                            SAPDB_UInt4            BufLength)
: Log_ReadWriteActionImage (Log_UpdateBlob)
, m_pBuf                   (pBuf)
, m_pBufForRead            (0)
, m_pBufAllocator          (0)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionBlobUpdate::Log_ActionBlobUpdate", LogAction_Trace, 5);

    m_Head.ImageLength  = Log_AlignedImageLen (sizeof(m_Head)) + Log_AlignedImageLen (BufLength);
    m_Head.BufLength    = BufLength;
    m_Head.InsertOffset = InsertOffset;
    m_Head.BlobFilename = BlobFilename;
    m_Head.TableId      = TableId;

    SAPDBTRACE_WRITELN (LogAction_Trace, 6, "ImageLength: " << m_Head.ImageLength <<
                                            ", InsertOffset: " << InsertOffset <<
                                            ", BufLength: " << BufLength);
    SAPDBTRACE_IF      (LogAction_Trace, 6, Kernel_TraceBuffer(pBuf, BufLength));
}
//---------------------------------------------------------------------------

Log_ActionBlobUpdate::~Log_ActionBlobUpdate ()
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionBlobUpdate::~Log_ActionBlobUpdate", LogAction_Trace, 5);
    if (NULL != m_pBufAllocator)
    {
        m_pBufAllocator->Deallocate (m_pBufForRead);
        m_pBufForRead = NULL;
    }
}

//---------------------------------------------------------------------------

void Log_ActionBlobUpdate::ExecuteAction (tgg00_TransContext &TransContext,
        tgg00_FileId       *pCommandBlobFileId) const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionBlobUpdate::ExecuteAction", LogAction_Trace, 5);

    KERNEL_TRACE_BASIS_ERROR (TransContext.trError_gg00, LogAction_Trace, 5);

    if (e_ok != TransContext.trError_gg00) return;

    tgg00_FileId  RedoBlobFileId;
    tgg00_FileId *pExecuteBlobFileId;

    pExecuteBlobFileId = Log_GetFileId (m_Head.BlobFilename, RedoBlobFileId, pCommandBlobFileId);

    b05write_bytestr (TransContext,
                      *pExecuteBlobFileId,
                      m_Head.TableId,
                      m_Head.InsertOffset+1,
                      m_Head.BufLength,
                      m_Head.BufLength,
                      m_pBuf, 1, NIL_STR_VERSION_GG00);
}

//---------------------------------------------------------------------------

bool Log_ActionBlobUpdate::ReadImagePersistent (Log_ActionImageReader &ImageReader,
                                                SAPDBErr_MessageList  &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionBlobUpdate::ReadImagePersistent", LogAction_Trace, 5);

    if (! ImageReader.CopyImage (REINTERPRET_CAST (SAPDB_Byte*, &(m_Head)),
                                 sizeof(m_Head),
                                 errlist) )
        return false;

    if ( ! ImageReader.PersistentRead (m_pBufForRead,m_Head.BufLength,m_pBufAllocator,errlist) )
        return false;

    m_pBuf = m_pBufForRead;
    return true;
}

//---------------------------------------------------------------------------

bool Log_ActionBlobUpdate::WriteImagePersistent (Log_ActionImageWriter &ImageWriter,
                                                 SAPDBErr_MessageList  &errlist)       const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionBlobUpdate::WriteImagePersistent", LogAction_Trace, 5);

    if ( ! ImageWriter.PersistentWrite (&m_Head, sizeof (m_Head), errlist))
        return false;
    return ImageWriter.PersistentWrite (m_pBuf, m_Head.BufLength, errlist);
}
