/*****************************************************************************/
/*!
  @file         Log_ActionSetFileAttributes.cpp
  @author       MartinKi
  @ingroup      ActionHandling

  @brief        Implementation file for class Log_ActionSetFileAttributes.

\if EMIT_LICENCE
  ========== licence begin  GPL
  Copyright (C) 2006 SAP AG

  This program is free software; you can redisytribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
  ========== licence end
\endif
*/
/*****************************************************************************/

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "Logging/Log_ActionSetFileAttributes.hpp"

#include "Logging/Log_ReadWriteActionImage.hpp"
#include "Logging/Log_Exceptions.hpp"
#include "Logging/Log_Messages.hpp"
#include "Logging/Log_Types.hpp"
#include "FileDirectory/FileDir_IDirectory.hpp"
#include "FileDirectory/FileDir_Generic.hpp"
#include "FileDirectory/FileDir_Table.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "Transaction/Trans_Context.hpp"
#include "Messages/Msg_List.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"

#include "ggg00.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

//---------------------------------------------------------------------------

SAPDB_Bool
Log_ActionSetFileAttributes::SetPagesClustered(
    const Trans_Context &TransContext,
    const SAPDB_Bool     bPagesClustered)
{
    SAPDBTRACE_METHOD_DEBUG (
        "Log_ActionSetFileAttributes::SetPagesClustered", LogAction_Trace, 5);

    FileDir_Generic fileEntry;
    if (! this->GetFileEntry (TransContext, fileEntry))
    {
        return false;
    }

    const SAPDB_Bool oldValue = fileEntry.ArePagesClustered();
    this->StoreAttributeValues (m_Head.pagesClustered, oldValue, bPagesClustered);
    m_Head.changeMask |= PagesClusteredChanged;
    return true;
}

//---------------------------------------------------------------------------

void
Log_ActionSetFileAttributes::StoreAttributeValues(
    SAPDB_Byte &attribute,
    SAPDB_Bool  bOldValue,
    SAPDB_Bool  bNewValue)
{
    SAPDBTRACE_METHOD_DEBUG (
        "Log_ActionSetFileAttributes::StoreAttributeValues", LogAction_Trace, 5);

    attribute  = bOldValue ? OldValueMask : 0;
    attribute |= bNewValue ? NewValueMask : 0;
}

//---------------------------------------------------------------------------

SAPDB_Bool
Log_ActionSetFileAttributes::GetAttributeValue(
    const SAPDB_Byte &attribute,
    const SAPDB_Bool bGetNewValue) const
{
    SAPDBTRACE_METHOD_DEBUG (
        "Log_ActionSetFileAttributes::GetAttributeValue", LogAction_Trace, 5);

    if (bGetNewValue)
    {
        return (attribute & NewValueMask) != 0;
    }
    else
    {
        return (attribute & OldValueMask) != 0;
    }
}

//---------------------------------------------------------------------------

SAPDB_Bool
Log_ActionSetFileAttributes::GetFileEntry(
    const Trans_Context   &TransContext,
    FileDir_Generic       &fileEntry) const
{
    SAPDBTRACE_METHOD_DEBUG (
        "Log_ActionSetFileAttributes::GetFileEntry", LogAction_Trace, 5);

    Msg_List errorList;
    FileDir_IDirectory &fileDir =
        FileDir_IDirectory::SessionInstance (TransContext);

    const FileDir_ReturnCode fDirRc = fileDir.GetFile (
        TransContext.TaskId(), m_Head.fileNo, fileEntry, errorList);

    return fDirRc == FileDir_Okay;
}

//---------------------------------------------------------------------------

void
Log_ActionSetFileAttributes::Execute (
    tgg00_TransContext &TransContext,
    ExecutionType       executionType) const
{
    SAPDBTRACE_METHOD_DEBUG (
        "Log_ActionSetFileAttributes::Execute", LogAction_Trace, 5);

    const RTE_TaskId  taskId = TransContext.trTaskId_gg00;

    FileDir_Generic fileEntry;
    if (! this->GetFileEntry(Trans_Context::GetContext (TransContext), fileEntry))
    {
        TransContext.trError_gg00 = e_bad_fdir;
        return;
    }

    const SAPDB_Bool bGetNewValue = (executionType != UndoExecution);

    if (m_Head.changeMask & PagesClusteredChanged)
    {
        fileEntry.SetPagesClustered (taskId, this->GetAttributeValue (m_Head.pagesClustered, bGetNewValue));
    }
}

//---------------------------------------------------------------------------

void
Log_ActionSetFileAttributes::Undo (
    tgg00_TransContext &TransContext,
    Log_IImage         &Image) const
{
    SAPDBTRACE_METHOD_DEBUG (
        "Log_ActionSetFileAttributes::Undo", LogAction_Trace, 5);

    Execute (TransContext, UndoExecution);
}

//---------------------------------------------------------------------------

void
Log_ActionSetFileAttributes::Redo (
    tgg00_TransContext   &TransContext,
    Log_IImage           &Image,
    Msg_List &Errlist) const
{
    SAPDBTRACE_METHOD_DEBUG (
        "Log_ActionSetFileAttributes::Redo", LogAction_Trace, 5);

    Execute (TransContext, RedoExecution);
}

//---------------------------------------------------------------------------

void
Log_ActionSetFileAttributes::Execute (tgg00_TransContext &TransContext) const
{
    SAPDBTRACE_METHOD_DEBUG (
        "Log_ActionSetFileAttributes::Execute-Online", LogAction_Trace, 5);

    Execute (TransContext, OnlineExecution);
}

//---------------------------------------------------------------------------

SAPDB_UInt
Log_ActionSetFileAttributes::GetPersistentLength () const
{
    return GetMinimalLengthToCopyEntry();
}

//---------------------------------------------------------------------------

SAPDB_UInt
Log_ActionSetFileAttributes::GetMinimalPersistentLength () const
{
    return GetMinimalLengthToCopyEntry();
}

//---------------------------------------------------------------------------

bool
Log_ActionSetFileAttributes::ReadImagePersistent (
    Log_ActionImageReader &ImageReader,
    Msg_List              &errlist)
{
    SAPDBTRACE_METHOD_DEBUG (
        "Log_ActionSetFileAttributes::ReadImagePersistent", LogAction_Trace, 5);

    return ImageReader.CopyImage( reinterpret_cast<SAPDB_Byte*>(&m_Head),
                                  sizeof(m_Head),
                                  errlist );
}

//---------------------------------------------------------------------------

bool
Log_ActionSetFileAttributes::WriteImagePersistent (
    Log_ActionImageWriter &ImageWriter,
    Msg_List              &errlist) const
{
    SAPDBTRACE_METHOD_DEBUG (
        "Log_ActionSetFileAttributes::WriteImagePersistent", LogAction_Trace, 5);

    return ImageWriter.PersistentWrite (&m_Head, sizeof (m_Head), errlist);
}

//---------------------------------------------------------------------------

void
Log_ActionSetFileAttributes::WriteToTrace (const char * Title) const
{
    Kernel_VTrace trace;

    if ( Title != 0 )
        trace << Title << FlushLine;

    trace << "fileNo: " << FileDir_FileNoToString (m_Head.fileNo);
    trace << " changeMask: " << m_Head.changeMask;
    trace << " pagesClustered: " << m_Head.pagesClustered;
}

//---------------------------------------------------------------------------

void
Log_ActionSetFileAttributes::AppendContextInfo (
    SAPDB_Char  const * const  filename,
    SAPDB_UInt4 const          linenumber,
    Msg_List                  &errList,
    SAPDB_Char  const * const  title) const
{
    SAPDBTRACE_METHOD_DEBUG (
        "Log_ActionSetFileAttributes::AppendContextInfo", LogAction_Trace, 5);

    errList = errList + Log_Exception(
        filename, linenumber, LOG_ACTION_SETFILEDIRCOUNTER,
        (title!=0 ? title : "Log_ActionSetFileAttributes"),
        FileDir_FileNoToString (m_Head.fileNo));
}

//---------------------------------------------------------------------------
