/****************************************************************************/
/*!

  @file        SysView_FileDirectories.cpp

  -------------------------------------------------------------------------

  @author      MartinKi

  @ingroup     SystemViews
  @brief       This module implements the "FILEDIRECTORIES" view class.

  @see

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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

*****************************************************************************/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SystemViews/SysView_Defines.hpp"
#include "SystemViews/SysView_ITableObj.hpp"
#include "SystemViews/SysView_FileDirectories.hpp"

#include "SQLManager/SQLMan_Context.hpp"
#include "FileDirectory/FileDir_ISharedDirectory.hpp"
#include "FileDirectory/FileDir_IDirectory.hpp"

#include "Transaction/Trans_Iterator.hpp"
#include "Transaction/Trans_Context.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "Container/Container_List.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"

#include "ggg91.h" // tgg91_SessionNo

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



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

void SysView_FileDirectories::Create()
{
    SAPDBTRACE_METHOD_DEBUG (
        "SysView_FileDirectories::Create", SysView_Trace, 5);

	m_Table->AppendCol (ITOCT_FIXED, SV_SESSIONID, 10);
	m_Table->AppendCol (ITOCT_FIXED, SV_TASKID, 10);
	m_Table->AppendCol (ITOCT_CHARBYTE,  SV_MAXUSEDFILEID, 8);
	m_Table->AppendCol (ITOCT_FIXED, SV_ENTRYCOUNT, 10);
	m_Table->AppendCol (ITOCT_FIXED, SV_CACHEENTRYCOUNT, 10);
	m_Table->AppendCol (ITOCT_FIXED, SV_HASHTABLESLOTCOUNT, 10);
	m_Table->AppendCol (ITOCT_FIXED, SV_USEDHASHTABLESLOTCOUNT, 10);
	m_Table->AppendCol (ITOCT_FIXED, SV_MAXHASHTABLECHAINENTRYCOUNT, 10);
	m_Table->AppendCol (ITOCT_FIXED, SV_RESIZEHASHTABLECOUNT, 10);
	m_Table->AppendCol (ITOCT_FIXED, SV_USEDPAGECOUNT, 10);
	m_Table->AppendCol (ITOCT_FIXED, SV_STATISTICSDELTACOUNT, 10);
	m_Table->AppendCol (ITOCT_FIXED, SV_CURRENTSTATISTICSDELTACOUNT, 10);
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_FileDirectories::GetColCount()
{
    SAPDBTRACE_METHOD_DEBUG (
        "SysView_FileDirectories::GetColCount", SysView_Trace, 5);

	return SV_CC_FILEDIRECTORIES;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_FileDirectories::EstimateRows()
{
    SAPDBTRACE_METHOD_DEBUG (
        "SysView_FileDirectories::EstimateRows", SysView_Trace, 5);

	return SV_ER_FILEDIRECTORIES;
}

/*---------------------------------------------------------------------------*/

void SysView_FileDirectories::Execute()
{
    SAPDBTRACE_METHOD_DEBUG (
        "SysView_FileDirectories::Execute", SysView_Trace, 5);

    m_Table->GetCatalogTable();

    if (m_Context.IsOk())
    {
        this->QuerySharedFileDirectory();
        this->QuerySessionFileDirectories();
    }
}

/*---------------------------------------------------------------------------*/

void SysView_FileDirectories::QuerySharedFileDirectory()
{
    SAPDBTRACE_METHOD_DEBUG (
        "SysView_FileDirectories::QuerySharedFileDirectory",
        SysView_Trace, 5);

    FileDir_ISharedDirectory& directory = FileDir_ISharedDirectory::Instance();

    SAPDB_UInt4 i;

    m_Table->MoveToCol (ITOVT_NULL, NULL, 0); // session id
    m_Table->MoveToCol (ITOVT_NULL, NULL, 0); // task id
    m_Table->MoveToCol (
        ITOVT_SURROGATE, directory.GetMaxFileNo().GetCharArray(), 0 );
    i = directory.GetEntryCount();
    m_Table->MoveToCol (ITOVT_UINT4, &i, 0);

    m_Table->MoveToCol (ITOVT_NULL, NULL, 0); // cache entry count

    i = directory.GetHashTableSlotCount();
    m_Table->MoveToCol (ITOVT_UINT4, &i, 0);

    i = directory.GetUsedHashTableSlotCount();
    m_Table->MoveToCol (ITOVT_UINT4, &i, 0);

    i = directory.GetMaxHashTableChainEntryCount();
    m_Table->MoveToCol (ITOVT_UINT4, &i, 0);

    i = directory.GetResizeHashTableCount();
    m_Table->MoveToCol (ITOVT_UINT4, &i, 0);

    i = directory.GetPageCount();
    m_Table->MoveToCol (ITOVT_UINT4, &i, 0);

    i = directory.GetStatisticsDeltaCount();
    m_Table->MoveToCol (ITOVT_UINT4, &i, 0);

    i = directory.GetCurrentStatisticsDeltaCount();
    m_Table->MoveToCol (ITOVT_UINT4, &i, 0);
}

/*---------------------------------------------------------------------------*/

void SysView_FileDirectories::QuerySessionFileDirectories()
{
    SAPDBTRACE_METHOD_DEBUG (
        "SysView_FileDirectories::QuerySessionFileDirectory",
        SysView_Trace, 5);

    RTETask_ITask& task = *RTETask_ITask::Self();
    Trans_Iterator transIt(
        task,
        Trans_Iterator::HasTempFileDirectory,
        true );
    ResultCollector        resultCollector( m_Context.GetAllocator(), *this );

    tgg91_SessionNo sessionNo;
    while ( transIt.Next() && m_Context.IsOk() ) {

        FileDir_IDirectory* pSessionFDir = transIt.GetTempFileDirContext();
        transIt.GetSessionNo( sessionNo );

        FDirRow row;
        row.sessionId                   =
            256 *
            (256 * (256 * sessionNo[0] + sessionNo[1]) + sessionNo[2])
            + sessionNo[3];
        row.taskId                      = transIt.GetTaskId();
        row.maxFileNo                   = pSessionFDir->GetMaxTempFileNo();
        row.entryCount                  = pSessionFDir->GetEntryCount();
        row.cacheEntryCount             = pSessionFDir->GetCacheEntryCount();
        row.hashTableSlotCount          = pSessionFDir->GetHashTableSlotCount();
        row.usedHashTableSlotCount      = pSessionFDir->GetUsedHashTableSlotCount();
        row.maxHashTableChainEntryCount = pSessionFDir->GetMaxHashTableChainEntryCount();
        row.resizeHashTableCount        = pSessionFDir->GetResizeHashTableCount();

        resultCollector.Append( m_Context, row );
    }
    resultCollector.Flush();
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
SysView_FileDirectories::AddRow(
    SysView_FileDirectories::FDirRow& row)
{
    m_Table->MoveToCol( ITOVT_INT4, &row.sessionId, 0 );
    m_Table->MoveToCol( ITOVT_INT4, &row.taskId, 0 );
    m_Table->MoveToCol( ITOVT_SURROGATE, row.maxFileNo.GetCharArray(), 0 );
    m_Table->MoveToCol( ITOVT_UINT4, &row.entryCount, 0 );
    m_Table->MoveToCol( ITOVT_UINT4, &row.cacheEntryCount, 0 );
    m_Table->MoveToCol( ITOVT_UINT4, &row.hashTableSlotCount, 0 );
    m_Table->MoveToCol( ITOVT_UINT4, &row.usedHashTableSlotCount, 0 );
    m_Table->MoveToCol( ITOVT_UINT4, &row.maxHashTableChainEntryCount, 0 );
    m_Table->MoveToCol( ITOVT_UINT4, &row.resizeHashTableCount, 0 );

    m_Table->MoveToCol( ITOVT_NULL, NULL, 0 );
    m_Table->MoveToCol( ITOVT_NULL, NULL, 0 );
    m_Table->MoveToCol( ITOVT_NULL, NULL, 0 );

    return m_Context.IsOk();
}

/*---------------------------------------------------------------------------*/

SysView_FileDirectories::ResultCollector::ResultCollector (
    SAPDBMem_IRawAllocator&  allocator,
    SysView_FileDirectories& fDirView)
    : m_fDirView( fDirView ),
      m_rowList( allocator )
{
}

/*---------------------------------------------------------------------------*/

SysView_FileDirectories::ResultCollector::~ResultCollector()
{
}

/*---------------------------------------------------------------------------*/

void SysView_FileDirectories::ResultCollector::Append (
    SQLMan_Context&                         context,
    const SysView_FileDirectories::FDirRow& row)
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

void SysView_FileDirectories::ResultCollector::Flush()
{
    FDirRowList::Iterator iter = m_rowList.Begin();
    for ( FDirRowList::Iterator end = m_rowList.End();
          iter != end;
          ++iter ) {

        if ( ! m_fDirView.AddRow( *iter ) ) {
            break;
        }
    }

    m_rowList.Delete();
}

/*---------------------------------------------------------------------------*/

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
