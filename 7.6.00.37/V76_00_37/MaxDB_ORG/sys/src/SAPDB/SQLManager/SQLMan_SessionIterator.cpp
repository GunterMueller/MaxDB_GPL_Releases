/****************************************************************************/
/*!

  @file        SQLMan_SessionIterator.cpp
  @author      MartinKi

  @ingroup
  @brief       Implementation of class SQLMan_SessionIterator.

  @see

*/
/*-------------------------------------------------------------------------

  copyright:    Copyright (c) 2005 SAP AG


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

#include "SQLManager/SQLMan_SessionIterator.hpp"
#include "SQLManager/SQLMan_SessionInfo.hpp"
#include "SQLManager/SQLMan_SessionList.hpp"
#include "SQLManager/SQLMan_ContextSessionMembers.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "SQLManager/SQLMan_Types.hpp"

#include "FileDirectory/FileDir_Generic.hpp"
#include "FileDirectory/FileDir_Iterator.hpp"
#include "FileDirectory/FileDir_IDirectory.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"

#include "hak10.h"

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

SQLMan_SessionIterator::SQLMan_SessionIterator()
    : m_pCurContext(0)
    , m_sessionIt(SQLMan_SessionList::Instance())
    , m_isValid(false)
{
}

/**************************************************************************/

SQLMan_SessionIterator::~SQLMan_SessionIterator()
{
}

/**************************************************************************/

SAPDB_Bool SQLMan_SessionIterator::Next()
{
    m_isValid = m_sessionIt.Next( m_pCurContext );
    return m_isValid;
}

/**************************************************************************/

void SQLMan_SessionIterator::Detach()
{
    while ( m_isValid ) {
        m_isValid = m_sessionIt.Next( m_pCurContext );
    }
}

/**************************************************************************/

bool SQLMan_SessionIterator::Unlock()
{
    // TODO: unlock is currently not supported
    return false;
}

/**************************************************************************/

SAPDB_Int4 SQLMan_SessionIterator::GetStartDate() const
{
    SAPDBERR_ASSERT_STATE( m_isValid );
    if ( ! m_isValid )
        return 0;

    return m_pCurContext->GetSessionInfo().GetStartDate();
}

/**************************************************************************/

SAPDB_Int4 SQLMan_SessionIterator::GetStartTime() const
{
    SAPDBERR_ASSERT_STATE( m_isValid );
    if ( ! m_isValid )
        return 0;

    return m_pCurContext->GetSessionInfo().GetStartTime();;
}

/**************************************************************************/

SAPDB_Bool
SQLMan_SessionIterator::GetSessionId(tgg91_SessionNo &sessionId) const
{
    SAPDBERR_ASSERT_STATE( m_isValid );
    if ( ! m_isValid )
    {
        return false;
    }

    m_pCurContext->GetSessionInfo().GetSessionId( sessionId );

    return true;
}

/**************************************************************************/

SAPDB_Int4
SQLMan_SessionIterator::GetCatalogCacheSize() const
{
    SAPDBERR_ASSERT_STATE( m_isValid );
    if ( ! m_isValid )
    {
        return -1;
    }

    SAPDBERR_ASSERT_STATE( m_pCurContext->a_size_ptr != 0 );
    return m_pCurContext->a_size_ptr->csz_ak_cache;
}

/**************************************************************************/

SAPDB_Int4 SQLMan_SessionIterator::GetTempPageCount(
    SQLMan_Context& context) const
{
    SAPDBERR_ASSERT_STATE( m_isValid );
    if ( ! m_isValid )
    {
        return -1;
    }

    FileDir_IDirectory& fileDir =
        m_pCurContext->GetSessionInfo().GetFileDirectory();

    FileDir_Iterator it(context.GetAllocator());
    FileDir_Generic  fileEntry;
    RTE_TaskId       taskId = context.TransContext().trTaskId_gg00;
    SAPDB_Int4       tempPageCount = 0;

    fileDir.GetPrivateFileDirIterator(it, &context.TransContext());
    while ( it.GetNext( fileEntry ) ) {
        tempPageCount += fileEntry.GetLeafPageCount(taskId)
            + fileEntry.GetIndexPageCount(taskId);
    }

    return tempPageCount;
}

/**************************************************************************/

RTE_TaskId SQLMan_SessionIterator::GetTaskId() const
{
    SAPDBERR_ASSERT_STATE( m_isValid );
    if ( ! m_isValid )
    {
        return 0;
    }

    return m_pCurContext->GetSessionInfo().GetTaskId();
}

/**************************************************************************/

SAPDB_Bool SQLMan_SessionIterator::GetApplicationInfo(
    tsp00_NodeId& applNode,
    SAPDB_Int4&   applPid) const
{
    SAPDBERR_ASSERT_STATE( m_isValid );
    if ( ! m_isValid )
    {
        return false;
    }

    return
        m_pCurContext->GetSessionInfo().GetApplicationInfo(
            applNode,
            applPid);
}

/**************************************************************************/

SAPDB_Bool SQLMan_SessionIterator::GetCatalogCacheStatistics(
    SAPDB_Int& accessCount,
    SAPDB_Int& successful,
    SAPDB_Int& hitRate) const
{
    SAPDBERR_ASSERT_STATE( m_isValid );
    if ( ! m_isValid )
    {
        return false;
    }

    const pasbool pNoReset = false;
    a10GetStatistics( *m_pCurContext, pNoReset, accessCount, successful, hitRate );

    return true;
}

/**************************************************************************/

SAPDB_Bool SQLMan_SessionIterator::ResetCatalogCacheStatistics()
{
    SAPDBERR_ASSERT_STATE( m_isValid );
    if ( ! m_isValid )
    {
        return false;
    }

    a10ResetStatistics( *m_pCurContext );
    return true;
}

/**************************************************************************/

SAPDB_Bool SQLMan_SessionIterator::IsSameSession(SQLMan_Context& context)
{
    SAPDBERR_ASSERT_STATE( m_isValid );
    if ( ! m_isValid )
    {
        return false;
    }

    return &context == m_pCurContext;
}

/**************************************************************************/

FileDir_ReturnCode SQLMan_SessionIterator::GetFile(
    FileDir_FileNo   fileNo,
    FileDir_Generic& fileEntry,
    Msg_List&        errMsg) const
{
    SAPDBERR_ASSERT_STATE( m_isValid );
    if ( ! m_isValid )
    {
        return FileDir_FileNoNotFound;
    }

    return
        m_pCurContext->GetSessionInfo().GetFileDirectory()
        .GetFileByOtherSession( fileNo, fileEntry, errMsg );
}

/**************************************************************************/

SQLMan_ApplType SQLMan_SessionIterator::GetApplicationType() const
{
    SAPDBERR_ASSERT_STATE( m_isValid );
    if ( ! m_isValid )
    {
        return SQLMan_ApplType::fromConst( at_unknown );
    }

    return m_pCurContext->GetSessionInfo().GetApplicationType();
}

/**************************************************************************/

SAPDB_Bool SQLMan_SessionIterator::GetSessionUserName(SQLMan_Identifier& userName) const
{
    SAPDBERR_ASSERT_STATE( m_isValid );
    if ( ! m_isValid )
    {
        return false;
    }

    m_pCurContext->GetSessionInfo().GetUserName( userName );
    return true;
}

/**************************************************************************/

FileDir_FileNo SQLMan_SessionIterator::GetParseFileNo() const
{
    SAPDBERR_ASSERT_STATE( m_isValid );
    if ( ! m_isValid )
    {
        return FileDir_FileNo();
    }

    return m_pCurContext->a_pars_curr.fileTabId_gg00();
}

/**************************************************************************/

SAPDB_UInt SQLMan_SessionIterator::GetIsolationLevel() const
{
    SAPDBERR_ASSERT_STATE( m_isValid );
    if ( ! m_isValid )
    {
        return 0;
    }

    return m_pCurContext->GetSessionInfo().GetIsolationLevel();
}

/**************************************************************************/

SAPDB_Int2 SQLMan_SessionIterator::GetUserTimeout() const
{
    SAPDBERR_ASSERT_STATE( m_isValid );
    if ( ! m_isValid )
    {
        return 0;
    }

    return m_pCurContext->GetSessionInfo().GetUserTimeout();
}
/**************************************************************************/

SQLMan_SqlMode SQLMan_SessionIterator::GetSqlMode() const
{
    SAPDBERR_ASSERT_STATE( m_isValid );
    if ( ! m_isValid )
    {
        return SQLMan_SqlMode::fromConst( sqlm_internal );
    }

    return m_pCurContext->GetSessionInfo().GetSqlMode();
}

/**************************************************************************/

SAPDB_Bool SQLMan_SessionIterator::GetNlsParams(SQLMan_NlsParams& nlsParams) const
{
    SAPDBERR_ASSERT_STATE( m_isValid );
    if ( ! m_isValid )
    {
        return 0;
    }

    m_pCurContext->GetSessionInfo().GetNlsParams( nlsParams );
    return true;
}

/**************************************************************************/

SQLMan_DateTimeFormat SQLMan_SessionIterator::GetDateTimeFormat() const
{
    SAPDBERR_ASSERT_STATE( m_isValid );
    if ( ! m_isValid )
    {
        return SQLMan_DateTimeFormat::fromConst( dtf_none );
    }

    return m_pCurContext->GetSessionInfo().GetDateTimeFormat();
}

/**************************************************************************/

RTETask_ITaskClientConnection::EncryptionType SQLMan_SessionIterator::GetEncryptionType() const
{
    SAPDBERR_ASSERT_STATE( m_isValid );
    if ( ! m_isValid )
    {
        return RTETask_ITaskClientConnection::UnknownEncryption;
    }

    return m_pCurContext->GetSessionInfo().GetEncryptionType();
}

/**************************************************************************/

RTETask_ITaskClientConnection::ConnectionType SQLMan_SessionIterator::GetSessionType() const
{
    SAPDBERR_ASSERT_STATE( m_isValid );
    if ( ! m_isValid )
    {
        return RTETask_ITaskClientConnection::UnknownConnection;
    }

    return m_pCurContext->GetSessionInfo().GetSessionType();
}

/*****************************************************************************/

SAPDB_UInt1 SQLMan_SessionIterator::GetDefaultCode() const
{
        SAPDBERR_ASSERT_STATE( m_isValid );
    if ( ! m_isValid )
    {
        return 0;
    }

    return m_pCurContext->GetSessionInfo().GetDefaultCode();
}

/**************************************************************************/
