/*****************************************************************************/
/*!
  @file         SQLMan_SessionIterator.hpp
  @author       MartinKi
  @ingroup

  @brief        Header file for class SQLMan_SessionIterator.

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

#ifndef SQLMAN_SESSIONITERATOR_HPP
#define SQLMAN_SESSIONITERATOR_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SQLManager/SQLMan_SessionList.hpp"
#include "SQLManager/SQLMan_Types.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "FileDirectory/FileDir_Types.hpp"
#include "RunTime/RTE_Types.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "RunTime/Tasking/RTETask_ITaskClientConnection.hpp"

#include "gsp00.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

class  SQLMan_Context;
class  FileDir_Generic;
class  tgg91_SessionNo;
struct tak_size_info;

/*!
 * @class  SQLMan_SessionIterator
 * @author MartinKi
 *
 * Provides an iterator over all SQL sessions.
 *
 * @warning While the iterator is valid no session can be created
 * or released! It is therfore important to insure that the iterator
 * has a lifetime that is limited to the time it is actually used, or
 * that its Detach()-method is called as soon as the lock it is holding
 * can be released.
 *
 * If you want to further process the iterator's return values, you
 * should buffer them until the iterator is either invalid or
 * detached. See Sysview_Sessions.cpp as an example.
 */
class SQLMan_SessionIterator
{
private:
    //no copy-construction or assignment
    SQLMan_SessionIterator(const SQLMan_SessionIterator&);
    SQLMan_SessionIterator operator=(SQLMan_SessionIterator&);

    SQLMan_Context*              m_pCurContext;
    SQLMan_SessionList::Iterator m_sessionIt;
    SAPDB_Bool                   m_isValid;
public:
    /*!
     * @brief Constructor. Does not lock the session list.
     */
    SQLMan_SessionIterator();

    /*!
     * @brief Destructor. Releases the lock on the session list if it
     *        should be still set.
     */
    ~SQLMan_SessionIterator();

    /*!
     * @brief Retrieves the next session. The first call of this method
     * will lock the session list. The lock is held until all sessions
     * have been retrieved by further calls to Next(), or until Detach()
     * is called.
     */
    SAPDB_Bool Next();

    /*!
     * @brief Detaches the iterator from the session list and releases
     *        the lock on the session list. The iterator will be
     *        invalid on return.
     */
    void Detach();

    /*!
     * @brief releases the lock, if possible. Returns true, if the
     * lock has been released; false otherwise
     */
    bool Unlock();

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
    SAPDB_Bool GetSessionId(tgg91_SessionNo &sessionId) const;

    /*!
     * @brief Returns the catalog cache size
     */
    SAPDB_Int4 GetCatalogCacheSize() const;

    /*!
     * @brief Returns the number of temp pages in use
     */
    SAPDB_Int4 GetTempPageCount(SQLMan_Context& context) const;

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
     * @brief Returns the catalog cache statistics of the session
     */
    SAPDB_Bool GetCatalogCacheStatistics(
        SAPDB_Int& accessCount,
        SAPDB_Int& successful,
        SAPDB_Int& hitRate) const;

    /*!
     * @brief Resets the catalog cache statistics of the session
     */
    SAPDB_Bool ResetCatalogCacheStatistics();

    /*!
     * @brief Retrieves a file directory entry from the session-local
     * file directory
     */
    FileDir_ReturnCode GetFile(
        FileDir_FileNo   fileNo,
        FileDir_Generic& fileEntry,
        Msg_List&        errMsg) const;

    /*!
     * @brief Returns true if the current session is the same as the
     * session described in context
     */
    SAPDB_Bool IsSameSession(SQLMan_Context& context);

    /*!
     * @brief Returns the application type of the session
     */
    SQLMan_ApplType GetApplicationType() const;

    /*!
     * @brief Returns the name of the user that initiated the session
     */
    SAPDB_Bool GetSessionUserName(SQLMan_Identifier& userName) const;

    /*!
     * @brief Returns the file no of the parse file of the session
     */
    FileDir_FileNo GetParseFileNo() const;

    /*!
     * @brief Returns the isolation level of the session
     */
    SAPDB_UInt GetIsolationLevel() const;

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
    SAPDB_Bool GetNlsParams(SQLMan_NlsParams& nlsParams) const;

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

/**************************************************************************/

#endif // SQLMAN_SESSIONITERATOR_HPP
