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

#include "Logging/Log_ActionCreateIndex75.hpp"

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "KernelCommon/Kernel_Trace.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "KernelCommon/Kernel_IAdminHotStandby.hpp"
#include "Logging/Log_InvDescMap.hpp"
#include "Logging/Log_ReadWriteActionImage.hpp"
#include "Logging/Log_ActionCreateDropFile.hpp"
#include "Logging/Log_ActionFileHandling.hpp"
#include "Logging/Log_Exceptions.hpp"

#include "FileDirectory/FileDir_ISharedDirectory.hpp"
#include "FileDirectory/FileDir_IDirectory.hpp"
#include "FileDirectory/FileDir_Table.hpp"
#include "FileDirectory/FileDir_Index.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "DataAccess/Data_Types.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "Messages/Msg_List.hpp"


#include "ggg91.h"
#include "ggg00.h"
#include "hbd01.h"
#include "hbd03.h"
#include "hkb64.h"

//---------------------------------------------------------------------------

void Log_ActionCreateIndex75::ExecuteAction (
    tgg00_TransContext &TransContext) const
{
    SAPDBTRACE_METHOD_DEBUG(
        "Log_ActionCreateIndex75::ExecuteAction", LogAction_Trace, 5);

    this->ExecuteAll( TransContext, executeCreate );
}

//---------------------------------------------------------------------------

void Log_ActionCreateIndex75::ExecuteSetNotAccessible (
    tgg00_TransContext &TransContext) const
{
    SAPDBTRACE_METHOD_DEBUG (
        "Log_ActionCreateIndex75::ExecuteSetNotAccessible",
        LogAction_Trace, 5);

    this->ExecuteAll( TransContext, executeSetNotAccessible );
}

//---------------------------------------------------------------------------

bool Log_ActionCreateIndex75::ReadImagePersistent (Log_ActionImageReader &ImageReader,
                                                 SAPDBErr_MessageList  &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionCreateIndex75::ReadImagePersistent", LogAction_Trace, 5);


    m_pInvDescMap = &(m_InvDescMapForRead);

    if ( ! ImageReader.CopyImage (REINTERPRET_CAST (SAPDB_Byte*, &(m_EntryHeader)),sizeof(m_EntryHeader),errlist))
        return false;

    if (m_EntryHeader.InvDescMapLen > 0)
    {
        return m_InvDescMapForRead.ReadImagePersistent(ImageReader, m_EntryHeader.InvDescMapLen, errlist);
    }
    return true;
}

//---------------------------------------------------------------------------

bool Log_ActionCreateIndex75::WriteImagePersistent (Log_ActionImageWriter &ImageWriter,
                                                  SAPDBErr_MessageList  &errlist)       const
{
    SAPDBERR_ASSERT_STATE( false );
    return false;
}

//---------------------------------------------------------------------------

void Log_ActionCreateIndex75::ExecuteAll(
    tgg00_TransContext &TransContext,
    const ExecutionType execType) const
{
    SAPDBTRACE_METHOD_DEBUG (
        "Log_ActionCreateIndex75::ExecuteAll", LogAction_Trace, 5);

    KERNEL_TRACE_BASIS_ERROR (TransContext.trError_gg00, LogAction_Trace, 5);

    if (e_ok != TransContext.trError_gg00) return;

    tgg00_FileId       RedoPermFileId;
    tgg00_FileId       RedoInvFileId;

    tgg00_FileId *pExecutePermFileId =
        Log_GetFileId (m_EntryHeader.TabFilename, RedoPermFileId, NULL);
    tgg00_FileId *pExecuteInvFileId =
        Log_GetFileId (m_EntryHeader.TabFilename, RedoInvFileId, NULL);

    // we have to provide a new fileNo for the index entry
    FileDir_ISharedDirectory& sFd = FileDir_ISharedDirectory::Instance();
    FileDir_FileNo indexFileNo    = sFd.MigrationGetNewFileNo();

    pExecuteInvFileId->fileTabId_gg00().rawAssign( indexFileNo.GetCharArray() );
    // not really necessary as we will not be looking at fileTfn and
    // fileTfnNo again, but clearer during debugging...
    pExecutePermFileId->fileTfn_gg00().becomes( tfnTable_egg00 );
    pExecutePermFileId->fileTfnNo_gg00()[0] = '\0';

    // the rest is business as usual
    Log_ActionCreateIndex createIndexAction(
        pExecutePermFileId->fileName_gg00(),
        pExecuteInvFileId->fileName_gg00(),
        m_pInvDescMap,
        pExecutePermFileId->fileTfn_gg00(),
        pExecutePermFileId->fileTfnNo_gg00()[0]);

    if ( execType == executeCreate ) {
        createIndexAction.Execute( TransContext );
    } else {
        createIndexAction.ExecuteSetNotAccessible( TransContext );
    }
}

//---------------------------------------------------------------------------
