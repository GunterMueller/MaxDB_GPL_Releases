/*****************************************************************************/
/*!
  @file         SQLTab_SysUpdStatLog.hpp
  @author       MartinKi
  @ingroup

  @brief        Header file for class SQLTab_SysUpdStatLog.

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

#ifndef SQLTAB_SYSUPDSTATLOG_HPP
#define SQLTAB_SYSUPDSTATLOG_HPP

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
class Catalog_Timestamp;
//struct tsp00_TermId;

/*!
 * @class  SQLTab_SysUpdStatLog
 * @author MartinKi
 *
 * This class provides access to the system table SYSUPDSTATLOG.
 */
class SQLTab_SysUpdStatLog
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

    /// Update duplicates or don't
    enum UpdateBehaviour {
        UpdateDuplicates,
        DoNotUpdateDuplicates
    };

    /// Do implicit commit or don't
    enum CommitBehaviour {
        WithCommit,
        WithoutCommit
    };

    /*!
     * @brief Constructor.
     *
     * Does not create the table but simply provides a handle to access it.
     */
    SQLTab_SysUpdStatLog(SQLMan_Context& context);

    /*!
     * @brief Inserts a new row into the table. Null-values cannot be
     * inserted using this method.
     *
     * @param values           [in] field sequence of values to insert
     * @param updateBehaviour  [in] update existing records or don't
     * @param commitBehaviour  [in] perform explicit commit or don't
     * @param schemaName       [in] value for column SCHEMANAME
     * @param tableName        [in] value for column TABLENAME
     * @param columnName       [in] value for column COLUMNNAME
     * @param indexName        [in] value for column INDEXNAME
     * @param tableFileNo      [in] value for column TABLEID
     * @param executionTimeStamp [in] value for column EXECUTION_AT
     * @param samplePercent    [in] value for column SAMPLE_PCT
     * @param sampleRows       [in] value for column SAMPLE_ROW
     * @param beginTimeStamp   [in] value for column EXECUTION_START
     * @param endTimeStamp     [in] value for column EXECUTION_END
     * @param sessionId        [in] value for column SESSION
     * @param termId           [in] value for column TERMID
     * @param seqNo            [in] value for column SEQNO
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
        const UpdateBehaviour      updateBehaviour,
        const CommitBehaviour      commitBehaviour,
        const SQLMan_Identifier&   schemaName,
        const SQLMan_Identifier&   tableName,
        const SQLMan_Identifier&   columnName,
        const SQLMan_Identifier&   indexName,
        const FileDir_FileNo&      tableFileNo,
        const RTESys_SQLTimestamp& executionTimeStamp,
        const SAPDB_Int            samplePercent,
        const SAPDB_Int            sampleRows,
        const RTESys_SQLTimestamp& beginTimeStamp,
        const RTESys_SQLTimestamp& endTimeStamp,
        const SAPDB_Int            sessionId,
        const tsp00_TermId&        termId,
        const SAPDB_Int            seqNo,
        Msg_List&                  errMsg);

    /*!
     * @brief Inserts a new row into the table.
     *
     * @param values           [in] field sequence of values to insert
     * @param updateDuplicates [in] update an existing record if true
     * @param doCommit         [in] commit changes if true
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
        const UpdateBehaviour             updateBehaviour,
        const CommitBehaviour             commitBehaviour,
        Msg_List&                         errMsg);

private:
    // no copy-construction or assignment
    SQLTab_SysUpdStatLog(const SQLTab_SysUpdStatLog&);
    SQLTab_SysUpdStatLog operator=(const SQLTab_SysUpdStatLog&);

    SQLMan_Context& m_context;
};

/**************************************************************************/

inline SAPDB_ToStringClass SAPDB_ToString(
    SQLTab_SysUpdStatLog::ReturnCode const rc)
{
    switch ( rc ) {
        case SQLTab_SysUpdStatLog::Okay:
            return SAPDB_ToStringClass( "Okay" );
        case SQLTab_SysUpdStatLog::LockRowFailed:
            return SAPDB_ToStringClass( "LockRowFailed" );
        case SQLTab_SysUpdStatLog::DuplicateKey:
            return SAPDB_ToStringClass( "DuplicateKey" );
        case SQLTab_SysUpdStatLog::InsertFailed:
            return SAPDB_ToStringClass( "InsertFailed" );
        case SQLTab_SysUpdStatLog::Cancelled:
            return SAPDB_ToStringClass( "Cancelled" );
        case SQLTab_SysUpdStatLog::SystemError:
            return SAPDB_ToStringClass( "SystemError" );
    }
    return SAPDB_ToStringClass( rc );
}


#endif // SQLTAB_SYSUPDSTATLOG_HPP
