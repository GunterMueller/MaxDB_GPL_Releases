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

#include "Logging/Log_ActionDropFile75.hpp"
#include "Logging/Log_ActionCreateDropFile.hpp"

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "KernelCommon/Kernel_Trace.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "KernelCommon/Kernel_IAdminHotStandby.hpp"
#include "Logging/Log_ReadWriteActionImage.hpp"
#include "Logging/Log_Exceptions.hpp"

#include "FileDirectory/FileDir_IDirectory.hpp"
#include "FileDirectory/FileDir_Table.hpp"
#include "FileDirectory/FileDir_ShortColumn.hpp"
#include "FileDirectory/FileDir_Index.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "DataAccess/Data_Types.hpp"
#include "Messages/Msg_List.hpp"

#include "Logging/Log_ActionFileHandling.hpp"

#include "ggg00.h"

//---------------------------------------------------------------------------

void Log_ActionDropFile75::ExecuteAction(
    tgg00_TransContext &TransContext,
    tgg00_FileId       *pCommandFileId) const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionDropFile75::ExecuteAction", LogAction_Trace, 5);

    KERNEL_TRACE_BASIS_ERROR (TransContext.trError_gg00, LogAction_Trace, 5);

    if (e_ok != TransContext.trError_gg00) return;

    tgg00_FileId  RedoFileId;
    tgg00_FileId *pExecuteFileId;

    pExecuteFileId = Log_GetFileId (m_Filename, RedoFileId, pCommandFileId);

    if ( (pExecuteFileId->fileTfn_gg00()    == tfnTable_egg00)
         || (pExecuteFileId->fileTfn_gg00() == tfnSys_egg00)
         || (pExecuteFileId->fileTfn_gg00() == tfnMulti_egg00)
         || (pExecuteFileId->fileTfn_gg00() == tfnShortScol_egg00)
        ) {
        Msg_List errMsg;
        FileDir_FileNo tableFileNo( pExecuteFileId->fileTabId_gg00() );
        FileDir_Table tableEntry;

        FileDir_IDirectory& fd = FileDir_IDirectory::Instance (
            TransContext.trIndex_gg00);

        // if log is from releases < 7.6 table cannot be marked
        // deleted, so it is safe to use GetTableFile instead of
        // GcGetFile
        fd.GetTableFile (
            TransContext.trTaskId_gg00,
            tableFileNo,
            tableEntry, errMsg);

        if ( ! tableEntry.IsAssigned() ) {
            // should only happen during redo of undo
            SAPDBERR_ASSERT_STATE( false ); // FIXME: too strong, but for testing
            return;
        }

        switch ( pExecuteFileId->fileTfn_gg00() ) {
            case tfnTable_egg00:
            case tfnSys_egg00:
            {
                // during the rollback of an unqualified delete there might be
                // indices defined on the table to drop and these indices must
                // not be dropped
                Msg_List errMsg;

                // detach all index entries
                for ( int indexId = 1;
                      indexId <= tableEntry.GetMaxIndexId();
                      ++indexId ) {

                    FileDir_FileNo indexFileNo(
                        tableEntry.GetIndexFileNo( indexId ) );

                    if ( indexFileNo.IsValid() ) {
                        tableEntry.RemoveIndex( indexFileNo );
                    }
                }
                break;
            }

            case tfnShortScol_egg00:
            {
                FileDir_FileNo shortColFileNo;
                shortColFileNo = tableEntry.GetShortColumnFileNo();
                if ( shortColFileNo.IsValid() ) {
                    tableEntry.SetShortColumnFileNo( FileDir_FileNo() );
                    pExecuteFileId->fileTabId_gg00().rawAssign(
                        shortColFileNo.GetCharArray() );
                } else {
                    // should only happen during redo of undo
                    return;
                }
                break;
            }

            case tfnMulti_egg00:
            {
                FileDir_FileNo indexFileNo;
                SAPDB_UInt2    indexId = pExecuteFileId->fileTfnNo_gg00()[0];

                indexFileNo = tableEntry.GetIndexFileNo( indexId );
                if ( indexFileNo.IsValid() ) {
                    tableEntry.RemoveIndex( indexFileNo );
                    pExecuteFileId->fileTabId_gg00().rawAssign(
                        indexFileNo.GetCharArray() );
                } else {
                    // should only happen during redo of undo
                    return;
                }
                break;
            }
        }
    }

    Log_ActionDropFile action( pExecuteFileId->fileName_gg00() );

    action.Execute( TransContext );
}

//---------------------------------------------------------------------------

bool Log_ActionDropFile75::ReadImagePersistent (Log_ActionImageReader &ImageReader,
                                              SAPDBErr_MessageList  &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionDropFile75::ReadImagePersistent", LogAction_Trace, 5);
    return ImageReader.CopyImage (REINTERPRET_CAST (SAPDB_Byte*, &(m_Filename)),sizeof(m_Filename),errlist);
}
