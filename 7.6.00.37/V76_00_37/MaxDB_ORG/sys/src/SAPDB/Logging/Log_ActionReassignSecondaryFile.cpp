/*!**************************************************************************

    module      : Log_ActionReassignSecondaryFile.cpp
    -------------------------------------------------------------------------

    author      : UweH

    special area: Logging
    description : defines Log_ActionReassignSecondaryFile

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
#include "Logging/Log_ActionReassignSecondaryFile.hpp"

#include "FileDirectory/FileDir_IDirectory.hpp"
#include "FileDirectory/FileDir_Index.hpp"
#include "FileDirectory/FileDir_ShortColumn.hpp"
#include "FileDirectory/FileDir_Table.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "Logging/Log_ActionReassignSecondaryFile.hpp"
#include "Logging/Log_Transaction.hpp"
#include "Messages/Msg_List.hpp"

//---------------------------------------------------------------------------

void Log_ActionReassignSecondaryFile::ExecuteAction ( tgg00_TransContext &trans ) const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionReassignSecondaryFile::ExecuteAction", LogAction_Trace, 5);

    KERNEL_TRACE_BASIS_ERROR (trans.trError_gg00, LogAction_Trace, 5);
    Msg_List errorList;

    if ( e_ok != trans.trError_gg00 )
        return;

    if ( ! m_Head.IsInitialized() )
    {
        trans.trError_gg00 = e_log_error;
        return;
    }

    const FileDir_FileNo secondaryFile   = m_Head.secondaryFile;
    const FileDir_FileNo toPrimaryFile   = m_Head.toPrimaryFile;
    const FileDir_FileNo fromPrimaryFile = m_Head.fromPrimaryFile;

    FileDir_Generic file;
    if ( FileDir_Okay !=
         FileDir_IDirectory::Instance(trans.trIndex_gg00).GetFile (
             trans.trTaskId_gg00, secondaryFile, file, errorList) )
    {
        trans.trError_gg00 = e_file_not_found;
        return;
    }

    FileDir_ReturnCode retValue;
    switch ( file.GetFileType() )
    {
    case Data_IndexFile:
    {
        FileDir_Index indexFile;
        indexFile.Assign( file );
        if ( fromPrimaryFile != indexFile.GetParentFileNo() )
        {
            bool inSavepoint =
                reinterpret_cast<Log_Transaction*>(trans.trLogContext_gg00)
                ->WasOpenInLastSavepoint();
            if ( inSavepoint
                 &&
                 (toPrimaryFile == indexFile.GetParentFileNo()) )
                // reassign has already been executed
                return;
            trans.trError_gg00 = e_invalid_tablename;
            return;
        }
        retValue =
            FileDir_IDirectory::Instance(trans.trIndex_gg00).ReassignIndexFile (
                trans.trTaskId_gg00,
                secondaryFile,
                toPrimaryFile,
                errorList);
    }
    break;
    case Data_ShortColumnFile:
    {
        FileDir_ShortColumn shortColFile;
        shortColFile.Assign( file );
        if ( fromPrimaryFile != shortColFile.GetParentFileNo() )
        {
            bool inSavepoint =
                reinterpret_cast<Log_Transaction*>(trans.trLogContext_gg00)
                ->WasOpenInLastSavepoint();
            if ( inSavepoint
                 &&
                 (toPrimaryFile == shortColFile.GetParentFileNo()) )
                // reassign has already been executed
                return;
            trans.trError_gg00 = e_invalid_tablename;
            return;
        }
        retValue =
            FileDir_IDirectory::Instance(trans.trIndex_gg00).ReassignShortColumnFile (
                trans.trTaskId_gg00,
                secondaryFile,
                toPrimaryFile,
                errorList);
    }
    break;
    }
    if ( FileDir_Okay != retValue )
    {
        switch (retValue)
        {
        case FileDir_FileNoNotFound:
        case FileDir_FileTypeMismatch:
            trans.trError_gg00 = e_file_not_found;
            break;
        default:
            // all errors should be converted
            SAPDBERR_ASSERT_STATE( false );
            trans.trError_gg00 = e_system_error;
        }
    }
}
//---------------------------------------------------------------------------

void Log_ActionReassignSecondaryFile::WriteToTrace (const char * Title) const
{
    Kernel_VTrace trace;

    if ( Title != 0 )
        trace << Title << FlushLine;

	trace << "reassign index: " << m_Head.secondaryFile
          << " from Table "     << m_Head.fromPrimaryFile
          << " to Table "       << m_Head.fromPrimaryFile
          << "."                << FlushLine;
}
