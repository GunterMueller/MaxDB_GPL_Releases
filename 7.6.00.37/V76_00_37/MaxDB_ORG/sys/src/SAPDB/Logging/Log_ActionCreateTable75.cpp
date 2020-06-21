/*!**************************************************************************

    module      : Log_ActionCreateDropFile.cpp
    -------------------------------------------------------------------------

    author      : JuergenA
    responsible : UweH

    special area: Logging
    description : defines a class to handle log entries for files

    last changed: 2001-03-28

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

#include "Logging/Log_ActionCreateTable75.hpp"

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "KernelCommon/Kernel_Trace.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "KernelCommon/Kernel_IAdminHotStandby.hpp"
#include "Logging/Log_ActionCreateDropFile.hpp"
#include "Logging/Log_ActionCreateShortColumnFile.hpp"
#include "Logging/Log_ActionCreateIndexFile.hpp"
#include "Logging/Log_ActionCreateTable.hpp"
#include "Logging/Log_ReadWriteActionImage.hpp"
#include "Logging/Log_ActionFileHandling.hpp"  // Log_GetFileId
#include "Logging/Log_Exceptions.hpp"

#include "FileDirectory/FileDir_ISharedDirectory.hpp"
#include "FileDirectory/FileDir_IDirectory.hpp"
#include "FileDirectory/FileDir_Table.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "DataAccess/Data_Types.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "Messages/Msg_List.hpp"

#include "ggg91.h"
#include "ggg00.h"

//---------------------------------------------------------------------------
void Log_ActionCreateTable75::ExecuteAction (tgg00_TransContext &TransContext,
                                           tgg00_FileId       *pCommandFileId) const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionCreateTable75::ExecuteAction", LogAction_Trace, 5);

    KERNEL_TRACE_BASIS_ERROR (TransContext.trError_gg00, LogAction_Trace, 5);

    if (e_ok != TransContext.trError_gg00) return;

    tgg00_FileId  RedoFileId;
    tgg00_FileId *pExecuteFileId;

    pExecuteFileId =
        Log_GetFileId( m_Entry.Filename, RedoFileId, pCommandFileId );

    FileDir_FileNo  tableFileNo( pExecuteFileId->fileTabId_gg00() );

    FileDir_IDirectory& fd =
        FileDir_IDirectory::Instance( TransContext.trIndex_gg00 );

    if ( pExecuteFileId->fileTfn_gg00() == tfnShortScol_egg00 )
    {   // we cannot use the given fileName_gg00 because it is the
        // same as for the table the short column file is defined on
        Msg_List        errMsg;
        FileDir_Generic fileEntry;

        fd.GetFile(
            TransContext.trTaskId_gg00,
            tableFileNo,
            fileEntry, errMsg);

        if (fileEntry.IsAssigned()) {
            SAPDBERR_ASSERT_STATE( fileEntry.GetFileType() == Data_TableFile );
            if ( fileEntry.GetFileType() != Data_TableFile ) {
                TransContext.trError_gg00 = e_invalid_filetype;
                return;
            }

            FileDir_Table tableEntry;
            tableEntry.Assign( fileEntry );

            FileDir_ISharedDirectory& sFd =
                FileDir_ISharedDirectory::Instance();
            FileDir_FileNo shortColFileNo( sFd.MigrationGetNewFileNo() );

            Log_ActionCreateShortColumnFile
                createShortColumnAction( tableFileNo, shortColFileNo );

            createShortColumnAction.Execute( TransContext );
        } else {
            TransContext.trError_gg00 = e_file_not_found;
        }
        return;
    } else if ( pExecuteFileId->fileTfn_gg00() == tfnMulti_egg00 ) {
        // during unqualified delete empty trees for indices get
        // recreated via a Log_CreateTable action...
        Msg_List        errMsg;
        FileDir_FileNo  tableFileNo( pExecuteFileId->fileTabId_gg00() );
        FileDir_Table   fileEntry;

        fd.GetFile(
            TransContext.trTaskId_gg00,
            tableFileNo,
            fileEntry, errMsg);

        if (fileEntry.IsAssigned()) {
            SAPDBERR_ASSERT_STATE( fileEntry.GetFileType() == Data_TableFile );
            if ( fileEntry.GetFileType() != Data_TableFile ) {
                TransContext.trError_gg00 = e_invalid_filetype;
                return;
            }

            FileDir_Table tableEntry;
            tableEntry.Assign( fileEntry );

            FileDir_ISharedDirectory& sFd =
                FileDir_ISharedDirectory::Instance();
            FileDir_FileNo indexFileNo( sFd.MigrationGetNewFileNo() );
            const SAPDB_UInt2 indexId = pExecuteFileId->fileTfnNo_gg00()[0];
            pExecuteFileId->fileTabId_gg00().rawAssign(
                indexFileNo.GetCharArray() );

            tgg00_FileId tableFileId;
            tableEntry.BuildFileId( tableFileId );

            Log_ActionCreateIndexFile action(
                tableFileId.fileName_gg00(),
                pExecuteFileId->fileName_gg00(),
                indexId );

            action.Execute( TransContext );
        } else {
            TransContext.trError_gg00 = e_file_not_found;
        }
        return;
    }

    tgg00_ExtendedFiletypeSet eFiletypeSet;
    eFiletypeSet.clear();

    // norm case: create regular table
    Log_ActionCreateTable action(
        tableFileNo,
        m_Entry.FileVersion,
        m_Entry.FiletypeSet,
        eFiletypeSet,
        m_Entry.StrColCnt );

    action.Execute( TransContext );
}

//---------------------------------------------------------------------------

bool Log_ActionCreateTable75::ReadImagePersistent (Log_ActionImageReader &ImageReader,
                                                 SAPDBErr_MessageList  &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionCreateTable75::ReadImagePersistent", LogAction_Trace, 5);

	return ImageReader.CopyImage (REINTERPRET_CAST (SAPDB_Byte*, &(m_Entry)),sizeof(m_Entry),errlist);
}

//---------------------------------------------------------------------------
