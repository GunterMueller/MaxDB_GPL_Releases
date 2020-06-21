/*!**************************************************************************

    module      : Log_ActionCreateIndexFile.cpp
    -------------------------------------------------------------------------

    author      : UweH

    special area: Logging
    description : defines Log_ActionCreateIndexFile

    last changed: 2003-09-17

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

#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
// #include "SAPDBCommon/SAPDB_ToString.hpp"
#include "KernelCommon/Kernel_Trace.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "Logging/Log_ActionCreateIndexFile.hpp"
#include "Logging/Log_ActionFileHandling.hpp"
#include "hbd01.h" // b01pdestroy_perm_file
#include "hbd03.h" // bd03CreateIndexFile
#include "ggg00.h"

#include "FileDirectory/FileDir_IDirectory.hpp"
#include "FileDirectory/FileDir_Index.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"

#include "Logging/Log_ActionFileHandling.hpp"
#include "Messages/Msg_List.hpp"

//---------------------------------------------------------------------------

void Log_ActionCreateIndexFile::ExecuteAction ( tgg00_TransContext &trans,
                                                tgg00_FileId       *pCommandTableFileId,
                                                tgg00_FileId       *pCommandIndexFileId ) const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionCreateIndexFile::ExecuteAction", LogAction_Trace, 5);
    
    KERNEL_TRACE_BASIS_ERROR (trans.trError_gg00, LogAction_Trace, 5);

    Msg_List errorList;
    if ( e_ok != trans.trError_gg00 )
        return; 
    
    if ( ! m_Head.IsInitialized() )
    {
        trans.trError_gg00 = e_log_error;
        return;
    }

    tgg00_FileId  TableFileId;
    tgg00_FileId  IndexFileId;
    tgg00_FileId *pTableFileId = Log_GetFileId (m_Head.tableFilename, TableFileId, pCommandTableFileId);
    tgg00_FileId *pIndexFileId = Log_GetFileId (m_Head.indexFilename, IndexFileId, pCommandIndexFileId);
    const bool    isUndoRedo   = (NULL == pCommandIndexFileId);

    pIndexFileId->fileType_gg00() = tgg00_FiletypeSet::build(
        2, ftsPerm_egg00, ftsConcurrent_egg00 );
    pIndexFileId->fileVersion_gg00().gg91SetDummyVers();

    const FileDir_FileNo indexFileNo (pIndexFileId->fileTabId_gg00());

    if (isUndoRedo)
    { // only relevant in redo; FIXME: can be removed once snychronisation is ok
        // if entry is in file directory but marked as deleted then
        // delete the file
        FileDir_Generic     fileEntry;
        FileDir_IDirectory& fd =
            FileDir_IDirectory::Instance (trans.trIndex_gg00);
        fd.GcGetFile (trans.trTaskId_gg00, indexFileNo, fileEntry, errorList);
        if (fileEntry.IsAssigned()
            && (Data_FileDeleted == fileEntry.GetFileState()))
        {   // destroy file
            fileEntry.Detach();
            b01pdestroy_perm_file (trans, *pIndexFileId);
        }
    }
    bd03CreateIndexFile( trans, *pIndexFileId, *pTableFileId, m_Head.indexId );
    
    if ( e_duplicate_filename == trans.trError_gg00 && isUndoRedo ){
        trans.trError_gg00 = e_ok;  // ignore redo error
    }
}

//---------------------------------------------------------------------------

void Log_ActionCreateIndexFile::WriteToTrace (const char * Title) const
{
    Kernel_VTrace trace;
    
    if ( Title != 0 )
        trace << Title << FlushLine;
        
	trace << "tableFilename: "
          << (const char*)SAPDB_ToString((const char *)&m_Head.tableFilename, sizeof(m_Head.tableFilename), SAPDB_ToStringClass::hex)
          << FlushLine;
	trace << "indexFilename: "
          << (const char*)SAPDB_ToString((const char *)&m_Head.indexFilename, sizeof(m_Head.indexFilename), SAPDB_ToStringClass::hex)
          << FlushLine;
	trace << "indexId: "             << m_Head.indexId                    << FlushLine;
}
