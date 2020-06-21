/*****************************************************************************/
/*!
  @file         SQLMan_SessionInfo.hpp
  @author       MartinKi
  @ingroup

  @brief        Header file for class SQLMan_SessionInfo.

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

#ifndef SQLMAN_SESSIONINFO_HPP
#define SQLMAN_SESSIONINFO_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SQLManager/SQLMan_Types.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "RunTime/RTE_Types.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "RunTime/Tasking/RTETask_ITaskClientConnection.hpp"


#include "ggg91.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

class SQLMan_Context;
class FileDir_IDirectory;

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
 * @brief  Object containing information about a database session
 * @author MartinKi
 *
 */
class SQLMan_SessionInfo
{
private:
    //no copy-construction or assignment
    SQLMan_SessionInfo(const SQLMan_SessionInfo&);
    SQLMan_SessionInfo operator=(SQLMan_SessionInfo&);

    SAPDB_Bool           m_initialized;
    SAPDB_Int2           m_userTimeout;
    SAPDB_Int4           m_startDate;
    SAPDB_Int4           m_startTime;
    SAPDB_Int4           m_applPid;
    RTE_TaskId           m_taskId;
    SAPDB_UInt           m_isoLevel;
    SQLMan_SqlMode       m_sqlMode;
    SQLMan_ApplType      m_applType;
    tgg00_DateTimeFormat m_dateTimeFormat;
    FileDir_IDirectory*  m_fileDir;
    tgg91_SessionNo      m_sessionId;
    SQLMan_NodeId        m_applNode;
    SQLMan_Identifier    m_userName;
    SQLMan_NlsParams     m_nlsParams;
    RTETask_ITaskClientConnection::EncryptionType          m_Encryption;
    RTETask_ITaskClientConnection::ConnectionType          m_SessionType;
    SAPDB_UInt1          m_DefaultCode;

public:
    /*!
     * @brief Constructor.
     */
    SQLMan_SessionInfo();

    /*!
     * @brief Initializes the object with values from context
     */
    void Init(SQLMan_Context& context);

    /*!
     * @brief Returns true, if the object has been initialized
     */
    SAPDB_Bool IsInitialized() const;

    /*!
     * @brief Returns the start date of the session
     */
    SAPDB_Int4 GetStartDate() const;

    /*!
     * @brief Returns the start time of the session
     */
    SAPDB_Int4 GetStartTime() const;

    /*!
     * @brief Returns the session id
     */
    void GetSessionId(tgg91_SessionNo &sessionId) const;

    /*!
     * @brief Returns the task id of the session
     */
    RTE_TaskId GetTaskId() const;

    /*!
     * @brief Returns information about the application that initiated
     * the session
     */
    SAPDB_Bool GetApplicationInfo(
        tsp00_NodeId& applNode,
        SAPDB_Int4&   applPid) const;

    /*!
     * @brief Returns the session-local file directory
     */
    FileDir_IDirectory& GetFileDirectory() const;

    /*!
     * @brief Returns the application type of the session
     */
    SQLMan_ApplType GetApplicationType() const;

    /*!
     * @brief Returns the name of the user that initiated the session
     */
    void GetUserName(SQLMan_Identifier& userName) const;

    /*!
     * @brief Returns the isolation level of the session
     */
    SAPDB_UInt GetIsolationLevel() const;

    /*!
     * @brief Sets the isolation level of the object (does not change
     * the actual isolation level of the session)
     */
    void SetIsolationLevel(SAPDB_UInt isolationLevel);

    /*!
     * @brief Returns the timeout of the user
     */
    SAPDB_Int2 GetUserTimeout() const;

    /*!
     * @brief Returns the sql mode the session was initiated with
     */
    SQLMan_SqlMode GetSqlMode() const;

    /*!
     * @brief Returns the language parameters of the session
     */
    void GetNlsParams(SQLMan_NlsParams& nlsParams) const;

    /*!
     * @brief Returns the date/time format of the session
     */
    SQLMan_DateTimeFormat GetDateTimeFormat() const;

    /*!
     * @brief Returns the encryption type used for the session
     */
    RTETask_ITaskClientConnection::EncryptionType GetEncryptionType() const;

    /*!
     * @brief Returns the session type (SQL, liveCache)
     */
    RTETask_ITaskClientConnection::ConnectionType GetSessionType() const;

    /*!
     * @brief Returns the default code (ASCII, UNICODE)
     */
    SAPDB_UInt1 GetDefaultCode() const;

};

#endif // SQLMAN_SESSIONINFO_HPP
