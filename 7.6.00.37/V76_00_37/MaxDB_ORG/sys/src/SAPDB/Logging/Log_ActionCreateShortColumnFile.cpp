/*!**************************************************************************

    module      : Log_ActionCreateShortColumnFile.cpp
    -------------------------------------------------------------------------

    author      : UweH

    special area: Logging
    description : defines Log_ActionCreateShortColumnFile

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
#include "KernelCommon/Kernel_Trace.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "Logging/Log_ActionCreateShortColumnFile.hpp"

#include "FileDirectory/FileDir_ShortColumn.hpp"
#include "FileDirectory/FileDir_IDirectory.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "Messages/Msg_List.hpp"
#include "ggg91.h" // gg91SetDummyVers
#include "ggg00.h"

#include "hbd01.h"
#include "hbd01_1.h"

//---------------------------------------------------------------------------

void Log_ActionCreateShortColumnFile::ExecuteAction (
    tgg00_TransContext &trans,
    FileDir_FileNo *pCommandTableFileNo,
    FileDir_FileNo *pCommandShortColFileNo ) const
{
    SAPDBTRACE_METHOD_DEBUG (
        "Log_ActionCreateShortColumnFile::ExecuteAction", LogAction_Trace, 5);

    KERNEL_TRACE_BASIS_ERROR (trans.trError_gg00, LogAction_Trace, 5);

    tgg00_BasisError& trError = trans.trError_gg00;
    Msg_List errorList;

    if ( e_ok != trError )
        return;

    if ( ! m_Head.IsInitialized() )
    {
        trError = e_log_error;
        return;
    }

    const bool    isUndoRedo   = 0 == pCommandShortColFileNo;

    FileDir_FileNo shortColFileNo;
    FileDir_FileNo tableFileNo;

    if ( (0 != pCommandTableFileNo) && (0 != pCommandShortColFileNo) )
    { // online execution
        shortColFileNo = *pCommandShortColFileNo;
        tableFileNo    = *pCommandTableFileNo;
    } else {
        shortColFileNo = m_Head.shortColFileNo;
        tableFileNo    = m_Head.tableFileNo;
    }

    tgg00_FileId shortColFileId;
    shortColFileId = b01niltree_id;

    shortColFileId.fileTfn_gg00().becomes (tfnShortScol_egg00);
    shortColFileId.fileTabId_gg00().rawAssign (shortColFileNo.GetCharArray());
    shortColFileId.fileType_gg00() = tgg00_FiletypeSet::build(
        2, ftsPerm_egg00, ftsConcurrent_egg00 );
    shortColFileId.fileVersion_gg00().gg91SetDummyVers();


    if (isUndoRedo)
    { // only relevant in redo; FIXME: can be removed once snychronisation is ok
        // delete file directory entry if it already exists
        FileDir_IDirectory& fd = FileDir_IDirectory::SessionInstance (trans);
        FileDir_Generic     fileEntry;

        fd.GcGetFile (trans.trTaskId_gg00, shortColFileNo, fileEntry, errorList);
        if (fileEntry.IsAssigned()
            && (Data_FileDeleted == fileEntry.GetFileState()))
        {
            fileEntry.Detach();
            b01pdestroy_perm_file (trans, shortColFileId);
        }
    }
    
    tgg00_Surrogate tableSurrogate;
    tableSurrogate.rawAssign( tableFileNo.GetCharArray() );
    
    bd01CreateShortColumnFile( trans, shortColFileId, tableSurrogate);
    
    if(( e_duplicate_filename == trError ) && isUndoRedo ){
        trError = e_ok;  // ignore redo error
    }
}

//---------------------------------------------------------------------------

void Log_ActionCreateShortColumnFile::WriteToTrace (const char * Title) const
{
    Kernel_VTrace trace;

    if ( Title != 0 )
        trace << Title << FlushLine;

	trace << "tableFileNo: " << m_Head.tableFileNo
          << FlushLine;
	trace << "shortColFileNo: " << m_Head.shortColFileNo
          << FlushLine;
}
