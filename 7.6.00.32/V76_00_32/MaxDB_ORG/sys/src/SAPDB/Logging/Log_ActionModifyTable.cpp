
/*!**************************************************************************

    module      : Log_ActionModifyTable.cpp

    -------------------------------------------------------------------------

    author      : JuergenA
    responsible : UweH

    special area: Logging
    description : defines a class to handle alter table log entries

    last changed: 2001-03-30

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

#include "Logging/Log_ActionModifyTable.hpp"

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp" // Routine trace macros
#include "KernelCommon/Kernel_Trace.hpp"
#include "Logging/Log_Types.hpp"
#include "Logging/Log_ActionFileHandling.hpp"  // Log_GetFileId
#include "Logging/Log_InvDescMap.hpp"
#include "Logging/Log_ReadWriteActionImage.hpp"
#include "FileDirectory/FileDir_IDirectory.hpp"
#include "FileDirectory/FileDir_Table.hpp"

#include "ggg00.h"
#include "hbd01.h"
#include "hbd37.h"

Log_ActionModifyTable::Log_ActionModifyTable():
Log_ReadWriteActionImage  (Log_ModifyTable),
m_pSourceInvDescMap       (NULL),
m_pDestinInvDescMap       (NULL),
m_SourceInvDescMapForRead (),
m_DestinInvDescMapForRead ()
{ }

//---------------------------------------------------------------------------

Log_ActionModifyTable::Log_ActionModifyTable (const tgg00_Filename &SourceFilename,
                                              const tgg00_Filename &DestinFilename,
                                              const Log_InvDescMap *pSourceInvDescMap,
                                              const Log_InvDescMap *pDestinInvDescMap):
Log_ReadWriteActionImage  (Log_ModifyTable),
m_pSourceInvDescMap       (pSourceInvDescMap),
m_pDestinInvDescMap       (pDestinInvDescMap),
m_SourceInvDescMapForRead (),
m_DestinInvDescMapForRead ()
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionModifyTable::Log_ActionModifyTable", LogAction_Trace, 5);
    m_EntryHeader.InvDescMapLen  = pSourceInvDescMap->GetPersistentLength();
    m_EntryHeader.SourceFilename = SourceFilename;
    m_EntryHeader.DestinFilename = DestinFilename;
    
    m_EntryHeader.ImageLen =
        Log_AlignedImageLen (sizeof (m_EntryHeader)     ) +
        Log_AlignedImageLen (m_EntryHeader.InvDescMapLen) +
        Log_AlignedImageLen (m_EntryHeader.InvDescMapLen);
}

//---------------------------------------------------------------------------

void Log_ActionModifyTable::ExecuteAction (tgg00_TransContext  &TransContext,
                                           tgg00_FileId        *pSourceCommandFileId,
                                           tgg00_FileId        *pIndexCommandFileId,
                                           tgg00_FileId        *pDestinCommandFileId,
                                           bool                 IsSorted,
                                           SAPDB_Int            TruncCheckPos,
                                           SAPDB_Int            TruncCheckCnt,
                                           tgg00_SampleInfoPtr  ResultSampleInfo) const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionModifyTable::ExecuteAction", LogAction_Trace, 5);
    
    KERNEL_TRACE_BASIS_ERROR (TransContext.trError_gg00, LogAction_Trace, 5);
    
    if (e_ok != TransContext.trError_gg00) return; 
    
    bool IgnoreRedoError = (NULL == pSourceCommandFileId);
    
    tgg00_FileId  NilIndexFileId   = b01niltree_id;
    
    tgg00_FileId  SourceRedoFileId;
    tgg00_FileId  DestinRedoFileId;
    
    tgg00_FileId *pSourceExecuteFileId;
    tgg00_FileId *pIndexExecuteFileId; 
    tgg00_FileId *pDestinExecuteFileId;
    
    pSourceExecuteFileId =
        Log_GetFileId (m_EntryHeader.SourceFilename, SourceRedoFileId, pSourceCommandFileId);
    
    pIndexExecuteFileId  = (NULL == pIndexCommandFileId) ? &NilIndexFileId : pIndexCommandFileId;
    
    pDestinExecuteFileId =
        Log_GetFileId (m_EntryHeader.DestinFilename, DestinRedoFileId, pDestinCommandFileId);
    
    b37table_modify (
        TransContext,
        IgnoreRedoError,
        IsSorted,
        *pSourceExecuteFileId,
        *pIndexExecuteFileId,
        *pDestinExecuteFileId,
        m_pSourceInvDescMap->GetInvDescMap(),
        m_pSourceInvDescMap->GetValidEntries(),
        m_pDestinInvDescMap->GetInvDescMap(),
        TruncCheckPos,
        TruncCheckCnt,
        ResultSampleInfo);

    // copy LOB Counter to new file
    FileDir_IDirectory& fileDir = FileDir_IDirectory::Instance( TransContext.trIndex_gg00 );

    Msg_List msgList;
   
    FileDir_Table srcFile;
    FileDir_Table destFile;

    FileDir_FileNo fileNoSrc( pSourceExecuteFileId->fileTabId_gg00() );
    
    if ( fileDir.GetTableFile( TransContext.trTaskId_gg00, fileNoSrc, srcFile, msgList )
        != FileDir_Okay )
    {
        SAPDBERR_ASSERT_STATE (false);
        TransContext.trError_gg00 = e_bad_fdir;
        return;
    }

    FileDir_FileNo fileNoDest( pDestinExecuteFileId->fileTabId_gg00() );
    
    if ( fileDir.GetTableFile( TransContext.trTaskId_gg00, fileNoDest, destFile, msgList )
        != FileDir_Okay )
    {
        SAPDBERR_ASSERT_STATE (false);
        TransContext.trError_gg00 = e_bad_fdir;
        return;
    }

    destFile.SetLobPageCount(srcFile.GetLobPageCount());
}

//----------------------------------------------------------------------------

bool Log_ActionModifyTable::ReadImagePersistent (Log_ActionImageReader &ImageReader,
                                                 SAPDBErr_MessageList  &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionModifyTable::ReadImagePersistent", LogAction_Trace, 5);
    
    m_pSourceInvDescMap = &(m_SourceInvDescMapForRead);
    m_pDestinInvDescMap = &(m_DestinInvDescMapForRead);
    
    if(!ImageReader.CopyImage (REINTERPRET_CAST (SAPDB_Byte*, &(m_EntryHeader)),sizeof(m_EntryHeader),errlist))
        return false;
    if (m_EntryHeader.InvDescMapLen > 0)
    {
        if(!m_SourceInvDescMapForRead.ReadImagePersistent(ImageReader, m_EntryHeader.InvDescMapLen, errlist))
            return false;
        if(!m_DestinInvDescMapForRead.ReadImagePersistent(ImageReader, m_EntryHeader.InvDescMapLen, errlist))
            return false;
    }
    return true;
}

//----------------------------------------------------------------------------

bool Log_ActionModifyTable::WriteImagePersistent (Log_ActionImageWriter &ImageWriter,
                                                  SAPDBErr_MessageList  &errlist)       const                         
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionModifyTable::WriteImagePersistent", LogAction_Trace, 5);
    if(!ImageWriter.PersistentWrite (&(m_EntryHeader), sizeof (m_EntryHeader), errlist))
        return false;
    if(!m_pSourceInvDescMap->WriteImagePersistent (ImageWriter, errlist))
        return false;
    if(!m_pDestinInvDescMap->WriteImagePersistent (ImageWriter, errlist))
        return false;
	return true;
}
