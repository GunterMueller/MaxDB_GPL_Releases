/****************************************************************************/
/*!

  @file        SysView_Files.cpp

  -------------------------------------------------------------------------

  @author      MartinKi

  @ingroup     SystemViews
  @brief       This module implements the "Files" view class.

  @see

*/
/*-------------------------------------------------------------------------

  copyright:    Copyright (c) 2002-2005 SAP AG


    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "ggg00.h"
#include "SystemViews/SysView_Defines.hpp"
#include "SystemViews/SysView_ITableObj.hpp"
#include "SystemViews/SysView_Files.hpp"

#include "SQLManager/SQLMan_Context.hpp"
#include "SQLManager/SQLMan_Types.hpp"
#include "SAPDB/SQLManager/Catalog/Catalog_TableDescriptor.hpp"
#include "SAPDB/SQLManager/Catalog/Catalog_IndexDescriptor.hpp"

#include "FileDirectory/FileDir_OmsKeyPartition.hpp"
#include "FileDirectory/FileDir_ShortColumn.hpp"
#include "FileDirectory/FileDir_Index.hpp"
#include "FileDirectory/FileDir_Table.hpp"
#include "FileDirectory/FileDir_Generic.hpp"
#include "FileDirectory/FileDir_Iterator.hpp"
#include "FileDirectory/FileDir_ISharedDirectory.hpp"
#include "FileDirectory/FileDir_IDirectory.hpp"

#include "Transaction/Trans_Iterator.hpp"
#include "Transaction/Trans_Context.hpp"

#include "Messages/Msg_List.hpp"
#include "DataAccess/Data_Types.hpp"

#include "FrameControl/FrameCtrl_ICommon.hpp"

#include "Container/Container_List.hpp"
#include "SAPDBCommon/SAPDB_string.h"
#include "SAPDBCommon/SAPDB_Types.h"

#include "RunTime/Tasking/RTETask_ITask.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS  (PROTOTYPES)                                     *
 *===========================================================================*/

const char c_unknown[] = "UNKNOWN";

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

SysView_Files::SysView_Files(
    SQLMan_Context  &context,
    tak40_show_glob &a41v)
    : SysView_SysViewBase (context, a41v),
      m_pageSize (FrameCtrl_ICommon::GetInstance().GetPageSize() / 1024)
{}

/*---------------------------------------------------------------------------*/

void SysView_Files::Create()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_Files::Create", SysView_Trace, 5);

	m_Table->AppendCol (ITOCT_CHARBYTE, SV_FILEID, 8);
	m_Table->AppendCol (ITOCT_FIXED, SV_SESSIONID, 10);
	m_Table->AppendCol (ITOCT_FIXED, SV_ROOT, 10);
	m_Table->AppendCol (ITOCT_CHAR, SV_TYPE, 20);
	m_Table->AppendCol (ITOCT_CHARBYTE, SV_PRIMARYFILEID, 8);
	m_Table->AppendCol (ITOCT_CHAR, SV_FILESTATE, 20);
	m_Table->AppendCol (ITOCT_FIXED, SV_ENTRYCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED, SV_TREEINDEXSIZE, 20);
	m_Table->AppendCol (ITOCT_FIXED, SV_TREELEAVESSIZE, 20);
	m_Table->AppendCol (ITOCT_FIXED, SV_LOBSIZE, 20);
	m_Table->AppendCol (ITOCT_CHAR, "CLUSTERED", 3);
	m_Table->AppendCol (ITOCT_CHAR, "DYNAMIC", 3);
	m_Table->AppendCol (ITOCT_CHAR, "PACKED", 3);
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_Files::GetColCount()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_Files::GetColCount", SysView_Trace, 5);

    return SV_CC_FILES;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_Files::EstimateRows()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_Files::EstimateRows", SysView_Trace, 5);

    // ignore session directories
    return FileDir_ISharedDirectory::IFileDirInstance().GetEntryCount();
}

/*---------------------------------------------------------------------------*/

void SysView_Files::Execute()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_Files::Execute", SysView_Trace, 5);

    const SAPDB_Bool bOutputSessionId = true;

    m_Table->GetCatalogTable();

    ResultCollector resultCollector( m_Context.GetAllocator(), *this );
    if (m_Context.IsOk())
    {
        FileDir_Iterator it( m_Context.GetAllocator() );
        FileDir_IDirectory* fdir =
            &FileDir_ISharedDirectory::IFileDirInstance();

        fdir->GetPrivateFileDirIterator( it, &m_Context.TransContext() );

        this->OutputFdir(
            it,
            resultCollector,
            ! bOutputSessionId,
            0,
            m_Context.GetErrorList() );

        // output session-local files
        RTETask_ITask& task = *RTETask_ITask::Self();
        Trans_Iterator transIt(
            task,
            Trans_Iterator::HasTempFileDirectory,
            true );
        tgg91_SessionNo sessionNo;
        while ( transIt.Next() && m_Context.IsOk() ) {

            FileDir_IDirectory* pSessionFDir = transIt.GetTempFileDirContext();

            transIt.GetSessionNo( sessionNo );
            SAPDB_Int4 sessionId =
                256 *
                (256 * (256 * sessionNo[0] + sessionNo[1]) + sessionNo[2])
                + sessionNo[3];
            pSessionFDir->GetPrivateFileDirIterator( it, &m_Context.TransContext() );
            this->OutputFdir(
                it,
                resultCollector,
                bOutputSessionId,
                sessionId,
                m_Context.GetErrorList() );

            it.Detach();
        }
        resultCollector.Flush();
    }

    if ( m_Context.IsOk() ) {
        m_Context.GetErrorList().ClearMessageList();
    }
}

/*---------------------------------------------------------------------------*/

void SysView_Files::OutputFdir(
    FileDir_Iterator&               it,
    SysView_Files::ResultCollector& resultCollector,
    const SAPDB_Bool                bOutputSessionId,
    const SAPDB_Int4                outputSessionId,
    Msg_List&                       errMsg)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_Files::OutputFdir", SysView_Trace, 5);

    FileDir_Generic fileEntry;
    RTE_TaskId taskId = m_Context.TransContext().trTaskId_gg00;

    while ( it.GetNext( fileEntry ) ) {
        FilesRow row;

        row.fileNo                 = fileEntry.GetFileNo();
        row.fileType               = fileEntry.GetFileType();
        row.rootPage               = fileEntry.GetRootPage();
        row.fileState              = fileEntry.GetFileState();
        row.bCountersInitialized   = fileEntry.AreCountersInitialized();
        row.entryCount             = fileEntry.GetEntryCount( taskId );
        row.indexPageCount         = -1;
        row.leafPageCount          = fileEntry.GetLeafPageCount( taskId );
        row.bLobCounterInitialized = false;
        row.lobPageCount           = 0;
        row.avClustered            = fileEntry.ArePagesClustered()
            ? avSet : avNotSet;
        row.avDynamic              = avNotPresent;
        row.avPacked               = avNotPresent;

        row.bOutputSessionId = bOutputSessionId;
        row.sessionId        = outputSessionId;

        switch ( fileEntry.GetFileType() ) {
            case Data_TableFile:
            {
                FileDir_Table tableEntry;
                tableEntry.Assign( fileEntry );

                row.bLobCounterInitialized = this->GetLobPageCount(
                    tableEntry, row.lobPageCount, errMsg );

                row.indexPageCount = tableEntry.GetIndexPageCount( taskId );
                row.avPacked        = tableEntry.AreRecordsPacked()
                    ? avSet : avNotSet;
                row.avDynamic       = tableEntry.IsDynamic()
                    ? avSet : avNotSet;
                break;
            }
            case Data_IndexFile:
            {
                FileDir_Index indexEntry;
                indexEntry.Assign( fileEntry );
                row.parentFileNo   = indexEntry.GetParentFileNo();
                row.indexPageCount = indexEntry.GetIndexPageCount( taskId );
                row.avPacked        = indexEntry.AreRecordsPacked()
                    ? avSet : avNotSet;
                break;
            }
            case Data_ShortColumnFile:
            {
                FileDir_ShortColumn shortColumnEntry;
                shortColumnEntry.Assign( fileEntry );
                row.parentFileNo   = shortColumnEntry.GetParentFileNo();
                row.indexPageCount = shortColumnEntry.GetIndexPageCount( taskId );
                break;
            }
            case Data_OmsKeyPartitionFile:
            {
                FileDir_OmsKeyPartition omsKeyPartitionEntry;
                omsKeyPartitionEntry.Assign( fileEntry );
                row.parentFileNo   = omsKeyPartitionEntry.GetParentFileNo();
                row.indexPageCount = omsKeyPartitionEntry.GetIndexPageCount( taskId );
                break;
            }
            default:
                row.parentFileNo.Invalidate();
        }

        if ( fileEntry.GetComment() != 0 ) {
            SAPDB_strncpy_zero_terminated(
                row.comment,
                fileEntry.GetComment(),
                c_tableNameLength );
        } else {
            row.comment[0] = '\0';
        }

        if ( bOutputSessionId ) {
            resultCollector.Append( m_Context, row );
        } else {
            this->AddRow( row );
        }
    }
}


/*---------------------------------------------------------------------------*/

SAPDB_Bool SysView_Files::AddRow(
    SysView_Files::FilesRow& row)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_Files::AddRow", SysView_Trace, 5);

    // FILEID
    m_Table->MoveToCol( ITOVT_SURROGATE, row.fileNo.GetCharArray(), 0 );

    // TASKID
    if ( row.bOutputSessionId ) {
        m_Table->MoveToCol (ITOVT_INT4, &row.sessionId, 0);
    } else {
        m_Table->MoveToCol (ITOVT_NULL, NULL, 0);
    }

    // ROOT
    SAPDB_Int4 rootPage = row.rootPage;
    m_Table->MoveToCol (ITOVT_INT4, &rootPage, 0);

    // TYPE
    switch ( row.fileType ) {

        case Data_TableFile:
            if ( row.comment[0] == '\0' ) {
                m_Table->MoveToCol (ITOVT_CHARPTR, "TABLE", 5);
            } else {
                for ( char* p = row.comment; *p != '\0'; ++p ) {
                    *p = toupper( *p ); // output needs to be uppercase
                }
                m_Table->MoveToCol (
                    ITOVT_CHARPTR,
                    row.comment,
                    SAPDB_strlen( row.comment ) );
            }
            break;
        case Data_IndexFile:
            m_Table->MoveToCol (ITOVT_CHARPTR, "INDEX", 5);
            break;
        case Data_ShortColumnFile:
            m_Table->MoveToCol (ITOVT_CHARPTR, "SHORT COLUMN FILE", 17);
            break;
        case Data_OmsFixedFile:
            m_Table->MoveToCol (ITOVT_CHARPTR, "FIXED OBJECT", 12);
            break;
        case Data_OmsVarFile:
            m_Table->MoveToCol (ITOVT_CHARPTR, "VARIABLE OBJECT", 15);
            break;
        case Data_OmsKeyFile:
            m_Table->MoveToCol (ITOVT_CHARPTR, "KEYED OBJECT", 12);
            break;
        case Data_OmsKeyPartitionFile:
            m_Table->MoveToCol (ITOVT_CHARPTR, "KEYED OBJECT INDEX", 18);
            break;
        default:
            m_Table->MoveToCol (ITOVT_CHARPTR, "UNKNOWN", 7);
    }

    // PRIMARYFILEID
    switch ( row.fileType ) {
        case Data_ShortColumnFile:
        case Data_IndexFile:
        case Data_OmsKeyPartitionFile:
            m_Table->MoveToCol(
                ITOVT_SURROGATE,
                row.parentFileNo.GetCharArray(), 0 );
            break;
        default:
            m_Table->MoveToCol (ITOVT_NULL, NULL, 0);
    }

    // FILESTATE
    switch ( row.fileState ) {
        case Data_FileOk:
            m_Table->MoveToCol (ITOVT_CHARPTR, "OK", 2);
            break;
        case Data_FileDeleted:
            m_Table->MoveToCol (ITOVT_CHARPTR, "DELETED", 7);
            break;
        case Data_FileNotAccessible:
        case Data_FileBad:
            m_Table->MoveToCol (ITOVT_CHARPTR, "BAD", 3);
            break;
        case Data_FileReadOnly:
            m_Table->MoveToCol (ITOVT_CHARPTR, "READ ONLY", 9);
            break;
        default:
            m_Table->MoveToCol (ITOVT_CHARPTR, "UNKNOWN", 7);
    }

    if ( row.bCountersInitialized ) {
        m_Table->MoveToCol (ITOVT_UINT8, &row.entryCount, 0);

        SAPDB_Int8 kbSize;
        if ( (row.fileType != Data_OmsFixedFile)
             && (row.fileType != Data_OmsVarFile)
             && (row.fileType != Data_OmsKeyFile) ) {
            SAPDB_Int8 indexPageCount = row.indexPageCount;
            kbSize = indexPageCount * m_pageSize;
            m_Table->MoveToCol (ITOVT_INT8, &kbSize, 0);
        } else {
            m_Table->MoveToCol (ITOVT_NULL, NULL, 0);
        }

        kbSize = row.leafPageCount * m_pageSize;
        m_Table->MoveToCol (ITOVT_INT8, &kbSize, 0);
    } else {
        m_Table->MoveToCol (ITOVT_NULL, NULL, 0); // entry count
        m_Table->MoveToCol (ITOVT_NULL, NULL, 0); // index page size
        m_Table->MoveToCol (ITOVT_NULL, NULL, 0); // leaf page size
    }

    // LOBSIZE
    if ( row.bLobCounterInitialized ) {
        SAPDB_UInt8 lobKbSize = row.lobPageCount * m_pageSize;
        m_Table->MoveToCol (ITOVT_UINT8, &lobKbSize, 0);
    } else {
        m_Table->MoveToCol (ITOVT_NULL, NULL, 0);
    }

    // CLUSTERED
    this->AddAttributeValue( row.avClustered );

    // DYNAMIC
    this->AddAttributeValue( row.avDynamic );

    // PACKED
    this->AddAttributeValue( row.avPacked );

    return m_Context.IsOk();
}

/*---------------------------------------------------------------------------*/

void SysView_Files::AddAttributeValue(const AttributeValue av)
{
    switch ( av ) {
        case avNotPresent:
            m_Table->MoveToCol (ITOVT_NULL, NULL, 0);
            break;
        case avNotSet:
            m_Table->MoveToCol (ITOVT_CHARPTR, "NO", 2);
            break;
        case avSet:
            m_Table->MoveToCol (ITOVT_CHARPTR, "YES", 3);
            break;
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool SysView_Files::GetLobPageCount(
    FileDir_Table& tableEntry,
    SAPDB_UInt8&   lobPageCount,
    Msg_List&      errMsg)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_Files::GetLobPageCount", SysView_Trace, 5);

    if ( (! tableEntry.IsLobPageCounterInitialized())
        || (tableEntry.GetFileState() == Data_FileDeleted) ) {
        return false;
    }

    lobPageCount = tableEntry.GetLobPageCount();

    FileDir_FileNo shortColumnFileNo = tableEntry.GetShortColumnFileNo();
    if ( shortColumnFileNo.IsInvalid() ) {
        // no short column file
        return true;
    }

    RTE_TaskId taskId = m_Context.TransContext().trTaskId_gg00;

    FileDir_IDirectory& fDir =
        FileDir_IDirectory::SessionInstance( m_Context.TransContext() );

    FileDir_ShortColumn shortColumnEntry;
    fDir.GetShortColumnFile(
        taskId, shortColumnFileNo, shortColumnEntry, errMsg );

    if ( shortColumnEntry.IsAssigned() ) {
        lobPageCount += shortColumnEntry.GetIndexPageCount( taskId )
            + shortColumnEntry.GetLeafPageCount( taskId );
    } else {
        // short column file might have been deleted
        SAPDBTRACE_WRITELN(
            SysView_Trace, 5,
            "Missing short column file for table " << tableEntry.GetFileNo() );
        return false;
    }

    return true;
}

/*---------------------------------------------------------------------------*/

SysView_Files::ResultCollector::ResultCollector (
    SAPDBMem_IRawAllocator& allocator,
    SysView_Files&          filesView)
    :m_filesView( filesView ),
     m_rowList( allocator )
{
}

/*---------------------------------------------------------------------------*/

SysView_Files::ResultCollector::~ResultCollector()
{
}

/*---------------------------------------------------------------------------*/

void SysView_Files::ResultCollector::Append (
    SQLMan_Context&                context,
    const SysView_Files::FilesRow& row)
{
    if ( ! context.IsOk() ) {
        return;
    }

    if ( ! m_rowList.InsertEnd(row).IsValid() )
    {
        Msg_List errMsg;
        context.SetNoMoreMemoryError( errMsg );
    }
}

/*---------------------------------------------------------------------------*/

void SysView_Files::ResultCollector::Flush()
{
    FilesRowList::Iterator iter = m_rowList.Begin();
    for ( FilesRowList::Iterator end = m_rowList.End();
          iter != end;
          ++iter ) {

        if ( ! m_filesView.AddRow( *iter ) ) {
            break;
        }
    }

    m_rowList.Delete();
}

/*---------------------------------------------------------------------------*/

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
