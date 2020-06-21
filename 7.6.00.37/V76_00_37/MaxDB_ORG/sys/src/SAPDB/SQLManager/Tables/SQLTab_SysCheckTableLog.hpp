/*****************************************************************************/
/*!
  @file         SQLTab_SysCheckTableLog.hpp
  @author       MartinKi
  @ingroup

  @brief        Header file for class SQLTab_SysCheckTableLog.

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
*/
/*****************************************************************************/

#ifndef SQLTAB_SYSCHECKTABLELOG_HPP
#define SQLTAB_SYSCHECKTABLELOG_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SQLManager/KernelSQL/KSQL_Statement.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

class Msg_List;
class SAPDBFields_IFieldSequence;
class SQLMan_Context;
class SQLMan_Identifier;

/*!
 * @class  SQLTab_SysCheckTableLog
 * @author MartinKi
 *
 * This class provides access to the system table SYSUPDSTATLOG.
 */
class SQLTab_SysCheckTableLog
{
public:
    /// Possible return codes.
    enum ReturnCode {
        Okay,
        LockRowFailed,
        DuplicateKey,
        InsertFailed,
        Cancelled,
        SystemError
    };

    /*!
     * @brief Constructor.
     *
     * Does not create the table but simply provides a handle to access it.
     */
    SQLTab_SysCheckTableLog(SQLMan_Context& context);

    /*!
     * @brief Inserts a new row into the table. Null-values cannot be
     * inserted using this method.
     *
     * @param values           [in] field sequence of values to insert
     * @param schemaName       [in] value for column SCHEMANAME
     * @param tableName        [in] value for column TABLENAME
     * @param executionTimeStamp [in] value for column EXECUTION_AT
     * @param msgNo            [in] value for column MSGNO
     * @param errMsg           [out] possible error messages
     *
     * @return
     *   - Okay, on success
     *   - DuplicateKey, if <em>updateDuplicates == false</em> and key exists
     *   - InsertFailed, if processing of the internal insert SQL statement
     *     failed; the SQL return code can be obtained by calling
     *     GetReturnCode
     *   - LockRowFailed, if the row could not be inserted because of a
     *     lock collision
     *   - SystemError
     */
    ReturnCode InsertRow(
        const SQLMan_Identifier&   schemaName,
        const SQLMan_Identifier&   tableName,
        const RTESys_SQLTimestamp& executionTimeStamp,
        const SAPDB_Int            msgNo,
        Msg_List&                  errMsg);

    /*!
     * @brief Inserts a new row into the table. Null-values cannot be
     * inserted using this method. A timestamp value for column
     * EXECUTED_AT will be automatically generated with the current
     * time.
     *
     * @param values           [in] field sequence of values to insert
     * @param schemaName       [in] value for column SCHEMANAME
     * @param tableName        [in] value for column TABLENAME
     * @param msgNo            [in] value for column MSGNO
     * @param errMsg           [out] possible error messages
     *
     * @return
     *   - Okay, on success
     *   - DuplicateKey, if <em>updateDuplicates == false</em> and key exists
     *   - InsertFailed, if processing of the internal insert SQL statement
     *     failed; the SQL return code can be obtained by calling
     *     GetReturnCode
     *   - LockRowFailed, if the row could not be inserted because of a
     *     lock collision
     *   - SystemError
     */
    ReturnCode InsertRow(
        const SQLMan_Identifier&   schemaName,
        const SQLMan_Identifier&   tableName,
        const SAPDB_Int            msgNo,
        Msg_List&                  errMsg);

private:
    // no copy-construction or assignment
    SQLTab_SysCheckTableLog(const SQLTab_SysCheckTableLog&);
    SQLTab_SysCheckTableLog operator=(const SQLTab_SysCheckTableLog&);

    SQLMan_Context& m_context;

    /*!
     * @brief Inserts a new row into the table.
     *
     * @param values           [in] field sequence of values to insert
     * @param bTryUpdate       [in] if true, first an attempt to update the
     *                              timestamp of an existing row is made and
     *                              only if that fails a new row will be
     *                              inserted
     * @param errMsg           [out] possible error messages
     *
     * @return
     *   - Okay, on success
     *   - DuplicateKey, if <em>updateDuplicates == false</em> and key exists
     *   - InsertFailed, if processing of the internal insert SQL statement
     *     failed; the SQL return code can be obtained by calling
     *     GetReturnCode
     *   - LockRowFailed, if the row could not be inserted because of a
     *     lock collision
     *   - SystemError
     */
    ReturnCode InsertRow(
        const SAPDBFields_IFieldSequence& values,
        const SAPDB_Bool                  bTryUpdate,
        Msg_List&                         errMsg);
};

/**************************************************************************/

#endif // SQLTAB_SYSCHECKTABLELOG_HPP
