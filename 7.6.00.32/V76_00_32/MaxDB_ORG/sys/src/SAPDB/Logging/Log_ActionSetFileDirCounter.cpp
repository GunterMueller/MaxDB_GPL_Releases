/*****************************************************************************/
/*!
  @file         Log_ActionSetFileDirCounter.cpp
  @author       MartinKi
  @ingroup      ActionHandling

  @brief        Implementation file for class Log_ActionSetFileDirCounter.

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

#include "Logging/Log_ActionSetFileDirCounter.hpp"

#include "Logging/Log_ReadWriteActionImage.hpp"
#include "Logging/Log_Exceptions.hpp"
#include "Logging/Log_Messages.hpp"
#include "Logging/Log_Types.hpp"
#include "FileDirectory/FileDir_IDirectory.hpp"
#include "FileDirectory/FileDir_Generic.hpp"
#include "FileDirectory/FileDir_Table.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
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

void
Log_ActionSetFileDirCounter::Execute (
    tgg00_TransContext &TransContext,
    ExecutionType       executionType) const
{
    SAPDBTRACE_METHOD_DEBUG (
        "Log_ActionSetFileDirCounter::Execute", LogAction_Trace, 5);

    SAPDBERR_ASSERT_STATE( executionType != UndoExecution );

    tgg00_BasisError &bErr = TransContext.trError_gg00;

    Msg_List errorList;
    FileDir_IDirectory &fileDir =
        FileDir_IDirectory::SessionInstance( TransContext );

    FileDir_Generic          fileEntry;
    const FileDir_ReturnCode fDirRc = fileDir.GetFile (
        TransContext.trTaskId_gg00, m_Head.fileNo, fileEntry, errorList);

    if ( fDirRc != FileDir_Okay )
    {
        bErr = e_bad_fdir;
        return;
    }

    if (fileEntry.GetFileType() != Data_TableFile)
    {
        fileEntry.InitializeBasicCounters (
            TransContext.trTaskId_gg00,
            m_Head.leafPageCount,
            m_Head.indexPageCount,
            m_Head.entryCount);
    }
    else
    {
        FileDir_Table tableEntry;
        tableEntry.Assign (fileEntry);
        tableEntry.InitializeTableCounters (
            TransContext.trTaskId_gg00,
            m_Head.leafPageCount,
            m_Head.indexPageCount,
            m_Head.entryCount,
            m_Head.lobPageCount);
    }

    return;
}

//---------------------------------------------------------------------------

void
Log_ActionSetFileDirCounter::Undo (
    tgg00_TransContext &TransContext,
    Log_IImage         &Image) const
{
    SAPDBTRACE_METHOD_DEBUG (
        "Log_ActionSetFileDirCounter::Undo", LogAction_Trace, 5);

    TransContext.trError_gg00 = e_not_implemented;
}

//---------------------------------------------------------------------------

void
Log_ActionSetFileDirCounter::Redo (
    tgg00_TransContext   &TransContext,
    Log_IImage           &Image,
    Msg_List &Errlist) const
{
    SAPDBTRACE_METHOD_DEBUG (
        "Log_ActionSetFileDirCounter::Redo", LogAction_Trace, 5);

    Execute (TransContext, RedoExecution);
}

//---------------------------------------------------------------------------

void
Log_ActionSetFileDirCounter::Execute (tgg00_TransContext &TransContext) const
{
    SAPDBTRACE_METHOD_DEBUG (
        "Log_ActionSetFileDirCounter::Execute-Online", LogAction_Trace, 5);

    Execute (TransContext, OnlineExecution);
}

//---------------------------------------------------------------------------

SAPDB_UInt
Log_ActionSetFileDirCounter::GetPersistentLength () const
{
    return GetMinimalLengthToCopyEntry();
}

//---------------------------------------------------------------------------

SAPDB_UInt
Log_ActionSetFileDirCounter::GetMinimalPersistentLength () const
{
    return GetMinimalLengthToCopyEntry();
}

//---------------------------------------------------------------------------

bool
Log_ActionSetFileDirCounter::ReadImagePersistent (
    Log_ActionImageReader &ImageReader,
    Msg_List              &errlist)
{
    SAPDBTRACE_METHOD_DEBUG (
        "Log_ActionSetFileDirCounter::ReadImagePersistent", LogAction_Trace, 5);

    return ImageReader.CopyImage( reinterpret_cast<SAPDB_Byte*>(&m_Head),
                                  sizeof(m_Head),
                                  errlist );
}

//---------------------------------------------------------------------------

bool
Log_ActionSetFileDirCounter::WriteImagePersistent (
    Log_ActionImageWriter &ImageWriter,
    Msg_List              &errlist) const
{
    SAPDBTRACE_METHOD_DEBUG (
        "Log_ActionSetFileDirCounter::WriteImagePersistent", LogAction_Trace, 5);

    return ImageWriter.PersistentWrite (&m_Head, sizeof (m_Head), errlist);
}

//---------------------------------------------------------------------------

void
Log_ActionSetFileDirCounter::WriteToTrace (const char * Title) const
{
    Kernel_VTrace trace;

    if ( Title != 0 )
        trace << Title << FlushLine;

    trace << "tableFileNo: " << FileDir_FileNoToString (m_Head.fileNo);
    trace << " indexPageCount: " << m_Head.indexPageCount;
    trace << " leafPageCount : " << m_Head.leafPageCount;
    trace << " entryCount    : " << m_Head.entryCount;
    trace << " lobPageCount  : " << m_Head.lobPageCount;
}

//---------------------------------------------------------------------------

void
Log_ActionSetFileDirCounter::AppendContextInfo (
    SAPDB_Char  const * const  filename,
    SAPDB_UInt4 const          linenumber,
    Msg_List                  &errList,
    SAPDB_Char  const * const  title) const
{
    SAPDBTRACE_METHOD_DEBUG (
        "Log_ActionSetFileDirCounter::AppendContextInfo", LogAction_Trace, 5);

    errList = errList + Log_Exception(
        filename, linenumber, LOG_ACTION_SETFILEDIRCOUNTER,
        (title!=0 ? title : "Log_ActionSetFileDirCounter"),
        FileDir_FileNoToString (m_Head.fileNo));
}

//---------------------------------------------------------------------------
