/*****************************************************************************/
/*!
  @file         SQLMan_SessionInfo.cpp
  @author       MartinKi
  @ingroup

  @brief        Implementation file for class SQLMan_SessionInfo.

  \if EMIT_LICENCE
  ========== licence begin  GPL
  Copyright (C) 2005 SAP AG

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

#include "SQLManager/SQLMan_SessionInfo.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "SQLManager/SQLMan_Tracing.hpp"
#include "SQLManager/SQLMan_Types.hpp"

#include "RunTime/Tasking/RTETask_LegacyConnectionInterface.hpp"

#include "hkb51_2.h" // kb51GetSessionStart
#include "ggg00.h"
#include "geo003.h"
#if defined(WIN32)   /*&if $OSSPEC == WIN32 */
# include "gos003.h" /* nocheck but_imkdep  */
#else                /*&else                */
# include "hen53.h"  /* nocheck but_imkdep  */
#endif               /*&endif               */

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*****************************************************************************/

SQLMan_SessionInfo::SQLMan_SessionInfo()
    : m_initialized(false),
      m_startDate(0),
      m_startTime(0),
      m_taskId(0),
      m_applPid(-1),
      m_isoLevel(0),
      m_fileDir(0),
      m_Encryption (RTETask_ITaskClientConnection::UnknownEncryption),
      m_SessionType (RTETask_ITaskClientConnection::UnknownConnection),
      m_DefaultCode (0)
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_SessionInfo::SQLMan_SessionInfo", SQLMan_Trace, 1 );
}

/*****************************************************************************/

void SQLMan_SessionInfo::Init(SQLMan_Context& context)
{
    SAPDBTRACE_METHOD_DEBUG("SQLMan_SessionInfo::Init", SQLMan_Trace, 1 );

    kb51GetSessionStart( context.TransContext(), m_startDate, m_startTime );
    m_fileDir        = reinterpret_cast<FileDir_IDirectory*>( context.TransContext().trLocalFileDirPtr_gg00 );
    m_sessionId      = context.TransContext().trSessionId_gg00;
    m_taskId         = context.TransContext().trTaskId_gg00;
    m_sqlMode        = context.GetSessionSqlMode();
    m_userName       = context.GetCurrentUserName();
    m_dateTimeFormat = context.GetDateTimeFormat();
    m_applType       = context.GetApplicationType();
    m_isoLevel       = context.GetIsolationLevel();
    m_userTimeout    = context.GetUserTimeout();
    m_DefaultCode    = context.GetDefaultCode();
    m_Encryption     = RTETask_EncryptionType (m_taskId);
    m_SessionType    = RTETask_ConnectionType (m_taskId);

    context.GetNlsParams( m_nlsParams );

    pasbool isOk;
    vgetapplpid( m_taskId, m_applNode, m_applPid, isOk );
    if ( ! isOk ) {
        m_applNode.SetBlank();
        m_applPid = -1;
    }

    m_initialized = true;
}

/*****************************************************************************/

SAPDB_Bool SQLMan_SessionInfo::IsInitialized() const
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_SessionInfo::IsInitialized", SQLMan_Trace, 1 );

    return m_initialized;
}

/*****************************************************************************/

SQLMan_ApplType SQLMan_SessionInfo::GetApplicationType() const
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_SessionInfo::GetApplicationType", SQLMan_Trace, 1 );

    return m_applType;
}

/*****************************************************************************/

void
SQLMan_SessionInfo::GetUserName(SQLMan_Identifier& userName) const
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_SessionInfo::GetCurrentUserName", SQLMan_Trace, 1 );

    userName = m_userName;
}

/*****************************************************************************/

SAPDB_UInt SQLMan_SessionInfo::GetIsolationLevel() const
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_SessionInfo::GetIsolationLevel", SQLMan_Trace, 1 );

    return m_isoLevel;
}

/*****************************************************************************/

void SQLMan_SessionInfo::SetIsolationLevel(SAPDB_UInt isolationLevel)
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_SessionInfo::setIsolationLevel", SQLMan_Trace, 1 );

    m_isoLevel = isolationLevel;
}

/*****************************************************************************/

SAPDB_Int2 SQLMan_SessionInfo::GetUserTimeout() const
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_SessionInfo::GetUserTimeout", SQLMan_Trace, 1 );

    return m_userTimeout;
}

/*****************************************************************************/

SQLMan_SqlMode SQLMan_SessionInfo::GetSqlMode() const
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_SessionInfo::GetSqlMode", SQLMan_Trace, 1 );

    return m_sqlMode;
}

/*****************************************************************************/

void SQLMan_SessionInfo::GetNlsParams(SQLMan_NlsParams& nlsParams) const
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_SessionInfo::GetNlsParams", SQLMan_Trace, 1 );

    nlsParams =  m_nlsParams;
}

/*****************************************************************************/

SQLMan_DateTimeFormat SQLMan_SessionInfo::GetDateTimeFormat() const
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_SessionInfo::GetDateTimeFormat", SQLMan_Trace, 1 );

    return m_dateTimeFormat;
}

/*****************************************************************************/

SAPDB_Int4 SQLMan_SessionInfo::GetStartDate() const
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_SessionInfo::GetStartDate", SQLMan_Trace, 1 );

    return m_startDate;
}

/*****************************************************************************/

SAPDB_Int4 SQLMan_SessionInfo::GetStartTime() const
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_SessionInfo::GetStartTime", SQLMan_Trace, 1 );

    return m_startTime;
}

/*****************************************************************************/

void SQLMan_SessionInfo::GetSessionId(tgg91_SessionNo &sessionId) const
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_SessionInfo::GetSessionId", SQLMan_Trace, 1 );

    sessionId = m_sessionId;
}

/*****************************************************************************/

RTE_TaskId SQLMan_SessionInfo::GetTaskId() const
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_SessionInfo::GetTaskId", SQLMan_Trace, 1 );

    return m_taskId;
}

/*****************************************************************************/

SAPDB_Bool SQLMan_SessionInfo::GetApplicationInfo(
    tsp00_NodeId& applNode,
    SAPDB_Int4&   applPid) const
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_SessionInfo::GetApplicationInfo", SQLMan_Trace, 1 );

    applNode = m_applNode;
    applPid  = m_applPid;

    return m_applPid != -1;
}

/*****************************************************************************/

FileDir_IDirectory& SQLMan_SessionInfo::GetFileDirectory() const
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_SessionInfo::GetFileDirectory", SQLMan_Trace, 1 );

    return *m_fileDir;
}

/*****************************************************************************/

RTETask_ITaskClientConnection::EncryptionType SQLMan_SessionInfo::GetEncryptionType() const
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_SessionInfo::GetEncryptionType", SQLMan_Trace, 1 );

    return m_Encryption;
}

/*****************************************************************************/

RTETask_ITaskClientConnection::ConnectionType SQLMan_SessionInfo::GetSessionType() const
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_SessionInfo::GetSessionType", SQLMan_Trace, 1 );

    return m_SessionType;
}

/*****************************************************************************/

SAPDB_UInt1 SQLMan_SessionInfo::GetDefaultCode() const
{
    SAPDBTRACE_METHOD_DEBUG(
        "SQLMan_SessionInfo::GetDefaultCode", SQLMan_Trace, 1 );

    return m_DefaultCode;
}
