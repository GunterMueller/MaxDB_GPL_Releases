/*****************************************************************************/
/*!
  @file         SQLMan_UpdateCounter.hpp
  @author       MartinKi

  @brief        Header file for class SQLMan_UpdateCounter.

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

#ifndef SQLMAN_UPDATECOUNTER_HPP
#define SQLMAN_UPDATECOUNTER_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "FileDirectory/FileDir_FileNo.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

class Data_UpdateCounterSrvTask;
class FileDir_Generic;
class SQLMan_Context;
class SQLTab_SysUpdateCounterWanted;
class Trans_Context;

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

namespace {
    class JobInfo;
    class JobList;
    class ServerHandling;
}

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/
/*!
 * @brief Handler class for command 'Check Table As Per System Table'
 *
 * @ingroup SQLManager
 *
 * Only one object of this class can be initialized successfully at a
 * time. All other instances will get error
 * e_dbm_command_not_possible.
 *
 * @author MartinKi
 */
class SQLMan_UpdateCounter
{
public:
    /*!
     * @brief Constructor.
     */
    SQLMan_UpdateCounter(SQLMan_Context& context);

    /*!
     * @brief Initialization method. Must be called before calling Execute().
     *
     * Only one object of this class can be initialized successfully
     * at a time. All other instances will get error
     * e_dbm_command_not_possible.
     *
     * @return
     *  - true, on success
     *  - false, on error; in this case a specific error will be set
     *    in context
     */
    SAPDB_Bool Initialize(SQLMan_Context& context);

    /*!
     * @brief Destructor.
     *
     * Other instances can be successfully initialized after
     * destruction.
     */
    ~SQLMan_UpdateCounter();

    /*!
     * @brief Executes the command.
     *
     * @param context [in/out] contains the command's status on return
     */
    void Execute(SQLMan_Context& context);

private:
    //no copy-construction or assignment
    SQLMan_UpdateCounter(const SQLMan_UpdateCounter&);
    SQLMan_UpdateCounter operator=(SQLMan_UpdateCounter&);

    SAPDB_Bool      m_isInitialized;
    SAPDB_Bool      m_throwSystemError;
    SAPDB_Bool      m_isCancelled;
    SQLMan_Context& m_context;

    enum {
        cUpdCntNotRunning = 0,
        cUpdCntIsRunning
    };

    // flag to make sure this command is only running once at a time
    static SAPDB_Int m_isRunning;

    void ProcessFinishedJob(
        SQLMan_Context&                context,
        SQLTab_SysUpdateCounterWanted& requestTable,
        JobList&                       jobList,
        Data_UpdateCounterSrvTask&     finishedSrvJob,
        Msg_List&                      errMsg);

    enum FreeRequestedTablesRc {
        frtRcOk,
        frtRcSystemError
    };

    FreeRequestedTablesRc FreeTablesRequestedByUsers(
        SQLMan_Context& context,
        JobList&        jobList,
        ServerHandling& serverHandling,
        Msg_List&       errMsg);

    void IncrementJobTryCounter(
        JobInfo&                       job,
        SQLTab_SysUpdateCounterWanted& requestTable,
        Msg_List&                      errMsg);

    enum WriteLogEntryRc {
        wleRcOk,
        wleRcCancelled,
        wleRcSystemError
    };

    WriteLogEntryRc WriteLogEntry(
        JobInfo&       job,
        Msg_List&      errMsg);

    WriteLogEntryRc WriteSingleLogEntry(
        const FileDir_FileNo& fileNo,
        FileDir_Generic&      fileEntry,
        Msg_List&             errMsg);
};

/*****************************************************************************/

#endif // SQLMAN_UPDATECOUNTER_HPP
