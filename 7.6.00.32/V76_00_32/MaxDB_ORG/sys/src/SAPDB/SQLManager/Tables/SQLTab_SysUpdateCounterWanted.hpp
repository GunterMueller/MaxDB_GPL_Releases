/*****************************************************************************/
/*!
  @file         SQLTab_SysUpdateCounterWanted.hpp
  @author       MartinKi
  @ingroup      SQLManager

  @brief        Header file for class SQLTab_SysUpdateCounterWanted.

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

#ifndef SQLTAB_SYSUPDATECOUNTERWANTED_HPP
#define SQLTAB_SYSUPDATECOUNTERWANTED_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SQLManager/SQLMan_Types.hpp"

#include "FileDirectory/FileDir_FileNo.hpp"

#include "Container/Container_Vector.hpp"

#include "SAPDBCommon/SAPDB_Types.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

class Catalog_TableDescriptor;
class FileDir_FileNo;
class Msg_List;
class SQLMan_Context;
class SQLMan_Identifier;

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
 * @class SQLTab_SysUpdateCounterWanted
 *
 * @brief Allows access to and manipulation of system table
 * SYSUPDATECOUNTERWANTED.
 *
 * @author MartinKi
 *
 */
class SQLTab_SysUpdateCounterWanted
{
private:
    //no copy-construction or assignment
    SQLTab_SysUpdateCounterWanted(const SQLTab_SysUpdateCounterWanted&);
    SQLTab_SysUpdateCounterWanted operator=(SQLTab_SysUpdateCounterWanted&);

    SQLMan_Context& m_context;

public:
    /// Forward declaration for iterator class.
    class RequestIterator;
    friend class RequestIterator;

    /*!
     * @brief Constructor.
     */
    SQLTab_SysUpdateCounterWanted(SQLMan_Context& context);


    /*!
     * @brief Possible return codes of a delete operation.
     */
    enum DeleteRc {
        delRcOk,
        delRcRowNotFound,
        delRcLockRequestTimeout,
        delRcCancelled,
        delRcSystemError
    };

    /*!
     * @brief Deletes a row in the table.
     *
     * @param requestIt [in] iterator pointing to row to be deleted
     * @param errMsg [out] possible error message
     */
    DeleteRc DeleteRow(
        RequestIterator& requestIt,
        Msg_List&        errMsg);

    /*!
     * @brief Deletes a row in the table.
     *
     * @param requestFileNo [in] fileNo of the file whose
     * corresponding row is to be deleted; the file can be either a
     * table or an index
     * @param errMsg [out] possible error message
     */
    DeleteRc DeleteRow(
        const FileDir_FileNo& requestFileNo,
        Msg_List&             errMsg);

    /*!
     * @brief Deletes a row in the table.
     *
     * @param schemaName [in] qualification for column 'SCHEMANAME'
     * @param tableName  [in] qualification for column 'TABLENAME'
     * @param indexName  [in] qualification for column 'INDEXNAME'
     * @param errMsg [out] possible error message
     */
    DeleteRc DeleteRow(
        const SQLMan_Identifier&  schemaName,
        const SQLMan_Identifier&  tableName,
        const SQLMan_Identifier&  indexName,
        Msg_List&                 errMsg);


    /*!
     * @brief Possible return codes of a GetKey operation.
     */
    enum GetKeyRc {
        gkRcOk          = delRcOk,
        gkRcRowNotFound = delRcRowNotFound,
        gkRcCancelled   = delRcCancelled,
        gkRcSystemError = delRcSystemError
    };

    /*!
     * @brief Returns the key for the current row.
     */
    static GetKeyRc GetRequestKeyValues(
        SQLMan_Context&       context,
        const FileDir_FileNo& requestFileNo,
        SQLMan_Identifier&    schemaName,
        SQLMan_Identifier&    tableName,
        SQLMan_Identifier&    indexName,
        Msg_List&             errMsg);

    /*!
     * @brief Possible return codes of a GetKey operation.
     */
    enum IncrementRc {
        incRcOk                 = delRcOk,
        incRcRowNotFound        = delRcRowNotFound,
        incRcLockRequestTimeout = delRcLockRequestTimeout,
        incRcCancelled          = delRcCancelled,
        incRcSystemError        = delRcSystemError
    };

    /*!
     * @brief Increments the update try count for the key for the
     * requestFileNo.
     */
    IncrementRc IncrementUpdateTryCount(
        const FileDir_FileNo& requestFileNo,
        Msg_List&             errMsg);

    /*!
     * @class RequestIterator
     * @author MartinKi
     *
     * @brief Provides an iterator over all requests in SYSUPDATECOUNTERWANTED.
     *
     * The content of SYSUPDATECOUNTERWANTED is scanned in
     * chunks. Therefore changes to SYSUPDATECOUNTERWANTED after
     * initialisation need not necessarily be visible during
     * iteration.
     */
    class RequestIterator
    {
    public:
        /*!
         * @brief Possible returnc codes of the Initialize() and
         * Reinit() methods.
         */
        enum ReturnCode {
            rcOk = 0,
            rcCancelled,
            rcSystemError,
            rcOutOfMemory
        };

        /*!
         * @brief Constructor.
         *
         * Initialize() needs to be called before the iterator can
         * actually be used.
         */
        RequestIterator(SQLMan_Context& context);

        /*!
         * @brief Initializes the iterator.
         */
        ReturnCode Initialize(Msg_List& errMsg);

        /*!
         * @brief Reinitializes the iterator.
         *
         * Resets the iterator to point to the first row of the table.
         */
        ReturnCode Reinit(Msg_List& errMsg);

        /*!
         * @brief Returns the file no of the next request.
         *
         * Requests that refer to the table 'SYSUPDATECOUNTERWANTED'
         * or an index on this table will be silently deleted and not
         * returned (due to possible deadlocks in
         * SQLMan_UpdateCounter).
         *
         * Requests that refer to non-existing tables or indices will
         * also silently be deleted.
         *
         * @param fileNo [out] the file no of the next request;
         * invalid if there are no further requests
         * @param errMsg [out] possible error messages
         *
         * @return rcOk if ok; rcOk is also returned if the iterator
         * has already reached the end of the file; in this case
         * fileNo will be invalid
         */
        ReturnCode Next(
            FileDir_FileNo& fileNo,
            Msg_List&       errMsg);

        /*!
         * @brief Returns true, if the iterator will not return any files.
         */
        SAPDB_Bool IsEmpty() const;

        /*!
         * @brief Removes the row the iterator is pointing to from the table.
         */
        ReturnCode DeleteBrokenRow(Msg_List& errMsg);

    private:
        friend class SQLTab_SysUpdateCounterWanted;

        // no copy-construction or assignment
        RequestIterator(const RequestIterator&);
        RequestIterator& operator=(const RequestIterator&);

        typedef Container_Vector<FileDir_FileNo> RequestList;

        SAPDB_Bool            m_lastChunkRead;
        FileDir_FileNo        m_curFileNo;
        SQLMan_Context&       m_context;
        RequestList           m_requestFileNos;
        RequestList::Iterator m_iterator;
        SQLMan_Identifier     m_schemaKey;
        SQLMan_Identifier     m_tableKey;
        SQLMan_Identifier     m_indexKey;


        ReturnCode ReadNextChunk(Msg_List& errMsg);

        ReturnCode GetIndexFileNo(
            Catalog_TableDescriptor& tableDesc,
            const SQLMan_Identifier& indexName,
            FileDir_FileNo&          indexFileNo,
            Msg_List&                errMsg);

        ReturnCode RemoveBrokenRow(
            const SQLMan_Identifier& schemaName,
            const SQLMan_Identifier& tableName,
            const SQLMan_Identifier& indexName,
            Msg_List&                errMsg );
    };
};

#endif // SQLTAB_SYSUPDATECOUNTERWANTED_HPP
