/*****************************************************************************/
/*!
  @file         SQLTab_SysUpdStatWanted.hpp
  @author       MartinKi
  @ingroup      SQLManager

  @brief        Header file for class SQLTab_SysUpdStatWanted.

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

#ifndef SQLTAB_SYSUPDSTATWANTED_HPP
#define SQLTAB_SYSUPDSTATWANTED_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SQLManager/SQLMan_Context.hpp"
#include "SQLManager/SQLMan_Types.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"

#include "ggg00.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

class Catalog_TableDescriptor;
class Catalog_ColumnDescriptor;
class Msg_List;

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/
/*!
 * @class SQLTab_SysUpdStatWanted
 * @author MartinKi
 *
 * Allows access and manipulation of system table SYSUPDSTATWANTED.
 */
class SQLTab_SysUpdStatWanted
{
public:
    /*!
     * @brief Return codes of insert operation
     */
    enum InsertRc {
        irOkay,
        irNoStatisticsPossible,
        irFailed,
        irCancelled,
        irLockRowFailed,
        irSystemError
    };

    /*!
     * @brief Return codes of delete operation.
     */
    enum DeleteRc {
        delRcOkay,
        delRcRowNotFound,
        delRcCancelled,
        delRcLockRequestTimeout,
        delRcSystemError
    };

private:
    // no copy-construction or assignment
    SQLTab_SysUpdStatWanted(const SQLTab_SysUpdStatWanted&);
    SQLTab_SysUpdStatWanted operator=(const SQLTab_SysUpdStatWanted&);

    SQLMan_Context& m_context;

    static void BuildKey(
        const SQLMan_Identifier& schemaName,
        const SQLMan_Identifier& ownerName,
        const SQLMan_Identifier& tableName,
        const SQLMan_Identifier& columnName,
        tgg00_Lkey&              key);

    static void ParseRecord(
        const tgg00_Rec&    recBuf,
        SQLMan_Identifier&  schemaName,
        SQLMan_Identifier&  ownerName,
        SQLMan_Identifier&  tableName,
        SQLMan_Identifier&  columnName);

    void CheckAndCreateUpdStatEvent();

    /*!
     * @brief Inserts a row into for SYSUPDSTATWANTED. No checks are
     * performed.
     */
    InsertRc InsertRow(
        const SQLMan_Identifier&  schemaName,
        const SQLMan_Identifier&  ownerName,
        const SQLMan_Identifier&  tableName,
        const SQLMan_Identifier&  columnName,
        Msg_List&                 errMsg);

    InsertRc InsertRowWithMessBlock(
        const SQLMan_Identifier&  schemaName,
        const SQLMan_Identifier&  ownerName,
        const SQLMan_Identifier&  tableName,
        const SQLMan_Identifier&  columnName);

public:
    class Iterator;
    friend class Iterator;
    class RequestColumnIterator;
    friend class RequestColumnIterator;

    /// Possible return codes.
    enum ReturnCode {
        Okay,
        SystemError
    };

    /*!
     * @brief Constructor.
     */
    SQLTab_SysUpdStatWanted(SQLMan_Context& context);

    /*!
     * @brief Returns the schema of this table.
     */
    SQLMan_Identifier GetSchema() const;

    /*!
     * @brief Returns the schema of this table.
     */
    SAPDB_Bool GetSchema(SQLMan_Identifier& schema, Msg_List& errMsg);

    /*!
     * @brief Returns the name of this table.
     */
    SQLMan_Identifier GetName() const;

    /*!
     * @brief Returns the name of this table.
     */
    SAPDB_Bool GetName(SQLMan_Identifier& name, Msg_List& errMsg);

    /*!
     * @brief Inserts a row into for SYSUPDSTATWANTED. Basic checks if
     * the table type is ok are performed.
     */
    InsertRc InsertRow(
        Catalog_TableDescriptor& tableDesc,
        Msg_List&                errMsg);

    /*!
     * @brief Inserts a row into for SYSUPDSTATWANTED. Basic checks if
     * the table type is ok are performed.
     */
    InsertRc InsertRow(
        Catalog_TableDescriptor& tableDesc,
        const SQLMan_Identifier& columnName,
        Msg_List&                errMsg);

    /*!
     * @brief Inserts a row into for SYSUPDSTATWANTED. Basic checks if
     * the table and column types are ok are performed.
     */
    InsertRc InsertRow(
        Catalog_TableDescriptor&        tableDesc,
        const Catalog_ColumnDescriptor& columnDesc,
        Msg_List&                       errMsg);

    /*!
     * @brief Returns an iterator over all columnnames of the table
     * described by tableDesc for which an update statistics request
     * exists.
     */
    RequestColumnIterator GetRequestColumnIterator(
        const Catalog_TableDescriptor& tableDesc) const;

    /*!
     * @brief Deletes the row specified by tableDesc and columnDesc
     * from SYSUPDSTATWANTED.
     *
     * @param tableDesc [in]  descriptor of table to be removed
     * @param columnDesc  [in] descriptor of column to be removed
     * @param errMsg     [in] possible error messages
     */
    DeleteRc DeleteRow(
        Catalog_TableDescriptor&        tableDesc,
        const Catalog_ColumnDescriptor& columnDesc,
        Msg_List&                       errMsg);

    /*!
     * @brief Deletes the row specified by tableDesc from
     * SYSUPDSTATWANTED.
     *
     * Rows that also contain a value for COLUMNNAME will not be
     * deleted.
     *
     * @param tableDesc [in]  descriptor of table to be removed
     * @param errMsg     [in] possible error messages
     */
    DeleteRc DeleteRow(
        Catalog_TableDescriptor& tableDesc,
        Msg_List&                errMsg);

    /*!
     * @brief Deletes the row specified from SYSUPDSTATWANTED.
     *
     * @param schemaName [in] qualification for column SCHEMA
     * @param ownerName  [in] qualification for column OWNER
     * @param tableName  [in] qualification for column TABLE
     * @param columnName [in] qualification for column COLUMN
     * @param errMsg     [in] possible error messages
     */
    DeleteRc DeleteRow(
        const SQLMan_Identifier&  schemaName,
        const SQLMan_Identifier&  ownerName,
        const SQLMan_Identifier&  tableName,
        const SQLMan_Identifier&  columnName,
        Msg_List&                 errMsg);

    /*!
     * @brief Provides an iterator over all column names of a table
     * for which an update statistics request exists.
     */
    class RequestColumnIterator
    {
    private:
        friend class SQLTab_SysUpdStatWanted;

        tgg00_Lkey m_curKey;

        RequestColumnIterator(
            SQLMan_TransContext&           transContext,
            const Catalog_TableDescriptor& tableDesc);

        // no assignment
        Iterator& operator=(const Iterator& it);

        SQLMan_TransContext&           m_trans;
        const Catalog_TableDescriptor* m_tableDesc;
        SAPDB_Bool                     m_firstCall;
    public:
        /*!
         * @brief copy-constructor
         */
        RequestColumnIterator(const RequestColumnIterator& it);

        /*!
         * @brief Advances the iterator by one.
         *
         * @param columnName [out] name of column for which statistics
         *                         are requested
         *
         * @return true, if columnName is a valid column name
         */
        SAPDB_Bool Next(SQLMan_Identifier& columnName);
    };
};

#endif // SQLTAB_SYSUPDSTATWANTED_HPP
