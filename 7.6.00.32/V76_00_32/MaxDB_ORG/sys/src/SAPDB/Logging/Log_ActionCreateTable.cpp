/**************************************************************************/
/*!

    @file  Log_ActionCreateDTable.cpp

    @brief  defines a class to handle log entries for files
    @author  UweH, MartinKi

\if EMIT_LICENCE

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


\endif
*/
/**************************************************************************/

/*===========================================================================*
*  INCLUDES                                                                  *
*============================================================================*/

#include "Logging/Log_ActionCreateTable.hpp"

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "KernelCommon/Kernel_Trace.hpp"
#include "Logging/Log_ReadWriteActionImage.hpp"
#include "Logging/Log_Exceptions.hpp"
#include "Logging/Log_ActionFileHandling.hpp"

#include "FileDirectory/FileDir_IDirectory.hpp"
#include "FileDirectory/FileDir_Table.hpp"
#include "FileDirectory/FileDir_Index.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "DataAccess/Data_Types.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "Messages/Msg_List.hpp"

#include "ggg00.h"
#include "ggg91.h"
#include "ggg00.h"
#include "hbd01.h"
#include "hbd01_1.h"

//---------------------------------------------------------------------------

Log_ActionCreateTable::Log_ActionCreateTable (
        const FileDir_FileNo            TableFileNo,
        const tgg91_FileVersion         FileVersion,
        const tgg00_FiletypeSet         FiletypeSet,
        const tgg00_ExtendedFiletypeSet eFiletypeSet,
        const SAPDB_Int2                LobColCnt)
    :
    Log_ReadWriteActionImage (Log_CreateTable)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionCreateTable::Log_ActionCreateTablet", LogAction_Trace, 5);

    m_Entry.TableFileNo  = TableFileNo;
    m_Entry.FileVersion  = FileVersion;
    m_Entry.FiletypeSet  = FiletypeSet;
    m_Entry.Filler       = 0;
    m_Entry.eFiletypeSet = eFiletypeSet;
    m_Entry.Filler1      = 0;
    m_Entry.LobColCnt    = LobColCnt;
}

//---------------------------------------------------------------------------

void Log_ActionCreateTable::ExecuteAction (
    tgg00_TransContext &TransContext,
    const SAPDB_Bool    IsRedo) const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionCreateTable::ExecuteAction", LogAction_Trace, 5);

    KERNEL_TRACE_BASIS_ERROR (TransContext.trError_gg00, LogAction_Trace, 5);

    if (e_ok != TransContext.trError_gg00) return;

    tgg00_FileId  TableFileId;
    Msg_List      errorList;

    FileDir_IDirectory& fd =
        FileDir_IDirectory::Instance (TransContext.trIndex_gg00);

    fd.BuildNewTableFileId (m_Entry.TableFileNo, m_Entry.FiletypeSet,
                            m_Entry.FileVersion, TableFileId);

    if ( IsRedo )
    {
        // delete file directory entry if it already exists
        FileDir_Generic fileEntry;
        fd.GcGetFile (TransContext.trTaskId_gg00, m_Entry.TableFileNo, fileEntry, errorList);

        if (fileEntry.IsAssigned())
        {  // destroy file
            // FIXME: until the add file action of the file directory
            // receives the correct converter version it can happen that
            // the table entry and index entries that are defined on the
            // table are already in the file directory although they
            // belong to the next savepoint...; we have to take care of
            // that: remove all secondary files defined on this table
            // otherwise MarkFileDeleted will return an error
            FileDir_Table ft;
            ft.Assign( fileEntry );
            FileDir_FileNo childFileNo;
            for (int indexNo = 1; indexNo <= 255; ++indexNo)
            {
                childFileNo = ft.GetIndexFileNo (indexNo);
                if ( childFileNo.IsValid() )
                    ft.RemoveIndex (childFileNo);
            }
            childFileNo.Invalidate();
            ft.SetShortColumnFileNo (childFileNo);
            ft.Detach();
            fileEntry.Detach();
            b01pdestroy_perm_file (TransContext, TableFileId);
        }
    }

    tgg00_ExtendedFiletypeSet eFiletypeSet = m_Entry.eFiletypeSet;
    bd01CreatePermTableFile( TransContext, TableFileId,
                             eFiletypeSet, m_Entry.LobColCnt );

    if ((e_duplicate_filename == TransContext.trError_gg00)
        && IsRedo)
    {
        TransContext.trError_gg00 = e_ok;  // ignore redo error
    }
}

//---------------------------------------------------------------------------

void Log_ActionCreateTable::Execute (tgg00_TransContext &TransContext) const
{
    const SAPDB_Bool bIsRedo = true;
    ExecuteAction (TransContext, bIsRedo);
}

//---------------------------------------------------------------------------

SAPDB_UInt Log_ActionCreateTable::GetPersistentLength () const
{
    return GetMinimalLengthToCopyEntry();
}

//---------------------------------------------------------------------------

SAPDB_UInt Log_ActionCreateTable::GetMinimalPersistentLength () const
{
    return GetMinimalLengthToCopyEntry();
}

//---------------------------------------------------------------------------

bool Log_ActionCreateTable::ReadImagePersistent (Log_ActionImageReader &ImageReader,
                                                 SAPDBErr_MessageList  &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionCreateTable::ReadImagePersistent", LogAction_Trace, 5);

	return ImageReader.CopyImage (REINTERPRET_CAST (SAPDB_Byte*, &(m_Entry)),sizeof(m_Entry),errlist);
}

//---------------------------------------------------------------------------

bool Log_ActionCreateTable::WriteImagePersistent (
    Log_ActionImageWriter &ImageWriter,
    SAPDBErr_MessageList  &errlist) const
{
    return ImageWriter.PersistentWrite (&m_Entry, sizeof (m_Entry), errlist);
}

//---------------------------------------------------------------------------

 Log_ActionVersion Log_ActionCreateTable::GetPersistentVersion() const
{
    return actionVersionStartingWith760018;
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void Log_ActionCreateTable01::ExecuteAction (
    tgg00_TransContext &TransContext,
    tgg00_FileId       *pCommandFileId) const
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionCreateTable01::ExecuteAction", LogAction_Trace, 5);

    KERNEL_TRACE_BASIS_ERROR (TransContext.trError_gg00, LogAction_Trace, 5);

    if (e_ok != TransContext.trError_gg00) return;

    tgg00_FileId  RedoFileId;
    tgg00_FileId *pExecuteFileId;
    Msg_List      errorList;

    pExecuteFileId = Log_GetFileId (m_Entry.Filename, RedoFileId, pCommandFileId);

    pExecuteFileId->fileVersion_gg00() = m_Entry.FileVersion;
    pExecuteFileId->fileType_gg00()    = m_Entry.FiletypeSet;

    if (NULL == pCommandFileId)
    { // only relevant in redo; FIXME: can be removed once snychronisation is ok
        // delete file directory entry if it already exists
        FileDir_FileNo      fileNo (pExecuteFileId->fileTabId_gg00());
        FileDir_IDirectory& fd = FileDir_IDirectory::Instance (TransContext.trIndex_gg00);
        FileDir_Generic fileEntry;
        fd.GcGetFile (TransContext.trTaskId_gg00, fileNo, fileEntry, errorList);

        if (fileEntry.IsAssigned())
        {  // destroy file
            // FIXME: until the add file action of the file directory
            // receives the correct converter version it can happen that
            // the table entry and index entries that are defined on the
            // table are already in the file directory although they
            // belong to the next savepoint...; we have to take care of
            // that: remove all secondary files defined on this table
            // otherwise MarkFileDeleted will return an error
            FileDir_Table ft;
            ft.Assign( fileEntry );
            FileDir_FileNo childFileNo;
            for (int indexNo = 1; indexNo <= 255; ++indexNo)
            {
                childFileNo = ft.GetIndexFileNo (indexNo);
                if ( childFileNo.IsValid() )
                    ft.RemoveIndex (childFileNo);
            }
            childFileNo.Invalidate();
            ft.SetShortColumnFileNo (childFileNo);
            ft.Detach();
            fileEntry.Detach();
            b01pdestroy_perm_file (TransContext, *pExecuteFileId);
        }
    }

    tgg00_ExtendedFiletypeSet eFiletypeSet;
    eFiletypeSet.clear();

    bd01CreatePermTableFile( TransContext, *pExecuteFileId,
                             eFiletypeSet, m_Entry.StrColCnt);

    if ((e_duplicate_filename == TransContext.trError_gg00)
        && (NULL == pCommandFileId))
    {
        TransContext.trError_gg00 = e_ok;  // ignore redo error
    }
}

//---------------------------------------------------------------------------

bool Log_ActionCreateTable01::ReadImagePersistent (Log_ActionImageReader &ImageReader,
                                                 SAPDBErr_MessageList  &errlist)
{
    SAPDBTRACE_METHOD_DEBUG ("Log_ActionCreateTable01::ReadImagePersistent", LogAction_Trace, 5);
	return ImageReader.CopyImage (REINTERPRET_CAST (SAPDB_Byte*, &(m_Entry)),sizeof(m_Entry),errlist);
}

//---------------------------------------------------------------------------
