/*****************************************************************************/
/*!
  @file         SQLMan_UpdateStatistics.hpp
  @author       MartinKi

  @brief        Header file for class SQLMan_UpdateStatistics.

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

#ifndef SQLMAN_UPDATESTATISTICS_HPP
#define SQLMAN_UPDATESTATISTICS_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SQLManager/SQLMan_Types.hpp"
#include "DataAccess/Data_UpdStatRequestFile.hpp"
#include "DataAccess/Data_PrimTreeStatisticsCoordinator.hpp"

#include "RunTime/System/RTESys_Time.h"

#include "Container/Container_Vector.hpp"

#include "SAPDBCommon/SAPDB_Types.hpp"

#include "ggg00.h"
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

class Catalog_ColumnDescriptor;
class Catalog_TableDescriptor;
class Catalog_TableStatisticsDescriptor;
class FileDir_TableEntry;
class Msg_List;
class SQLMan_Context;
class SQLMan_Identifier;

/*!
 * @class  SQLMan_UpdateStatistics
 * @author MartinKi
 *
 * @brief Implements parts of the 'Update Statistics' commands.
 *
 * This class currently implements only a small subset of all 'Update
 * statistics' variations.
 *
 * Locking: The base and statistics records of a table will be
 * share-locked while the statistics are being computed.
 */
class SQLMan_UpdateStatistics
{
private:
    class AutoSwitchToInternal;

public:
    class InternalTransInfo;
    friend class InternalTransInfo;

    /*!
     * @class InternalTransInfo
     *
     * Allows to create an internal transaction and to switch between
     * the original and this internal transaction.
     *
     * @warning: The original and the internal transaction share the
     * same session-local file directory. Therefore you must not work
     * in parallel with the internal and original transaction!
     */
    class InternalTransInfo
    {
    private:
        friend class AutoSwitchToInternal;
        friend class SQLMan_UpdateStatistics;

        SQLMan_UpdateStatistics& m_parent;
        SAPDB_Bool          m_transSet;
        SAPDB_Bool          m_internalTransSet;
        tgg00_UnivTrans     m_oldTrans;
        tgg00_TransContext  m_internalTransContext;

        InternalTransInfo(const InternalTransInfo&);
        InternalTransInfo& operator=(const InternalTransInfo&);

        /*!
         * @brief Creates a new internal transaction
         *
         * @warning: The original and the internal transaction share
         * the same session-local file directory. Therefore you must
         * not work in parallel with the internal and original
         * transaction!
         */
        SAPDB_Bool NewTrans();

        SAPDB_Bool Close(SAPDB_Bool doCommit);

        void SwitchToInternal();

        void SwitchToParent();
    public:
        InternalTransInfo(SQLMan_UpdateStatistics& parent)
            : m_parent( parent ),
              m_transSet( false ),
              m_internalTransSet( false )
        {}

        ~InternalTransInfo();

    };

    /// Possible return values.
    enum ReturnCode {
        Okay,        ///< Command completed successfully
        Cancelled,   ///< Command execution was cancelled
        LockTimeout, ///< A required lock resource could not be acquired
        SystemError  ///< A system error occured during command execution
    };

    typedef Data_PrimTreeStatisticsCoordinator::ColumnNumberVector  ColumnNumberVector;
    typedef Data_PrimTreeStatisticsCoordinator::DistinctValueVector DistinctValueVector;
    typedef Data_PrimTreeStatisticsCoordinator::StackCodeVector     StackCodeVector;
    typedef Data_PrimTreeStatisticsCoordinator::StatisticsInfo      StatisticsInfo;

    /*!
     * @brief Constructor.
     *
     */
    SQLMan_UpdateStatistics(
        SQLMan_Context&            context,
        const tsp00_TermId&        termId,
        const SAPDB_Int4           sampleRows,
        const SAPDB_Int4           samplePercentage);

    /*!
     * @brief Destructor.
     */
    ~SQLMan_UpdateStatistics();

    /*!
     * @brief Initializes the object.
     *
     * This method can also be called to reset the object. The start
     * timestamp for statistics operations will be set to the time of
     * the function call.
     */
    SAPDB_Bool Initialize(Msg_List& errMsg);

    /*!
     * @brief Processing of 'Update Statistics Column (...) ...'
     * command
     *
     * The SQLMan_Context this object was initialized with will
     * contain the result code of this operation.
     */
    void UpdateStatisticsColumn(Msg_List& errMsg);

    /*!
     * @brief Adds a compute statistics request for a table to the
     * internal request list.
     *
     * TODO: this method should become private as soon as this class
     * is able to collect the requested tables itself.
     */
    ReturnCode AddTableStatRequest(
        const FileDir_FileNo&   tableFileNo,
        const SAPDB_Int4        noOfPagesToSample,
        Msg_List&               errMsg);

    /*!
     * @brief Processes the list of requests collected.
     *
     * This method initiates the statistics computation for all
     * requests in the internal request list and also updates the
     * system table SYSUPDSTATLOG.
     */
    ReturnCode ProcessRequests(Msg_List& errMsg);

    /*!
     * @name Kernel parameter/Number of server tasks to be used
     *
     */
    /*@{*/

    /*!
     * @brief Initizalizes maximum number of parallel servers from
     * database parameter.
     */
    static void InitKernelParameters(Msg_List& errMsg);

    /*!
     * @brief Returns true if parallel update statistics is enabled.
     */
    static SAPDB_Bool IsParallel();

    /*! @brief Sets the maximum number of servers to use at a time
     *  during parallel update statistcs.
     */
    static void SetNoOfParallelServers(const SAPDB_Int noOfServers);

    /*! @brief Returns the maximum number of servers to use at a time
     *  during parallel update statistcs.
     */
    static SAPDB_Int GetNoOfParallelServers();

    /*!
     * @brief Sets the sample algorithm to be used during update
     * statistics.
     */
    static void SetSampleAlgoNo(const SAPDB_Int sampleAlgoNo);

    /*!
     * @brief Returns the sample algorithm to be used during update
     * statistics.
     */
    static SAPDB_Int GetSampleAlgoNo();

    /// Possible return codes of GetRequestInfoAndLockTable.
    enum RequestReturnCode {
        RequestOkay,
        RequestTableNotFound,
        RequestLockTimeout,
        RequestCancelled,
        RequestNewTransFailed,
        RequestSystemError
    };

    /*!
     * @brief Locks the catalog records for tableFileNo and prepares
     *        all information to required to compute the table statistics.
     *
     * @param tableFileNo [in] file no of the table for which
     *                         statistics are to be computed
     * @param statisticsDesc [out] handle that holds the lock on the
     *        statistics records in the catalog; this handle should be used
     *        to later update the table statistics
     * @param columnNumbers [out] vector containing the column numbers
     *        of the columns for which statistics should be computed; if the
     *        vector is empty then only pure table statistics should be
     *        computed (page and record count)
     * @param stackCode [out] vector containing the stack entries for
     *        the column for which statistics should be computed; there is a
     *        one-to-one relationship with columnNumbers,
     *        i.e. columnNumbers[i] contains the column number for
     *        stackCode[i]
     * @param errMsg [out] message list with possible error messages
     *
     * @return
     *    - RequestOkay, on success
     *    - RequestTableNotFound, if no catalog entry was found for tableFileNo
     *    - RequestSystemError, on system errors
     */
    RequestReturnCode GetRequestInfoAndLockTable(
        const FileDir_FileNo&              tableFileNo,
        InternalTransInfo&                 internalTrans,
        Catalog_TableStatisticsDescriptor& statisticsDesc,
        ColumnNumberVector&                columnNumbers,
        StackCodeVector&                   stackCode,
        Msg_List&                          errMsg);

    /*!
     * @brief Updates statistics information for a table.
     *
     * This method also will write information into the system table
     * SYSUPDSTATLOG and remove corresponding entries from
     * SYSUPDSTATWANTED.
     *
     * In case the update was successful an implicit 'COMMIT' is performed.
     *
     * @param statisticsDesc [in] handle refering to the statistic records
     *        to be updated
     * @param statisticsInfo [in] contains new statistics information
     * @param errMsg [out] message list with possible error messages
     *
     * @return
     *    - Okay, on success (even if table could not be found)
     *    - SystemError, on system errors
     */
    ReturnCode UpdateStatisticRecords(
        InternalTransInfo&                 internalTrans,
        Catalog_TableStatisticsDescriptor& statisticsDesc,
        const StatisticsInfo&              statisticsInfo,
        Msg_List&                          errMsg);

    /*@}*/

private:
    /// vector of kernel identifiers.
    typedef Container_Vector<SQLMan_Identifier> IdentifierVector;

    /*!
     * @brief Structure used to store the names of the columns whose
     * statistics should be determined during 'Update Statistics
     * Column...'
     */
    struct UpdateStatColumnInfo;
    friend struct UpdateStatColumnInfo;
    struct UpdateStatColumnInfo {
        FileDir_FileNo   tableFileNo;
        IdentifierVector columnNames;

        UpdateStatColumnInfo(SAPDBMem_IRawAllocator& allocator)
            : columnNames( allocator )
        {}

        void Init()
        {
            tableFileNo.Invalidate();
            columnNames.Clear();
        }
    };

    // no copy-construction or assignment
    SQLMan_UpdateStatistics(const SQLMan_UpdateStatistics&);
    SQLMan_UpdateStatistics operator=(const SQLMan_UpdateStatistics&);

    SQLMan_Context&         m_context;
    UpdateStatColumnInfo    m_updStatColInfo;
    Data_UpdStatRequestFile m_requestFile;
    tsp00_TermId            m_termId;
    RTESys_SQLTimestamp     m_startTimeStamp;
    SAPDB_Int4              m_sampleRowCount;
    SAPDB_Int4              m_samplePercentage;

    /*!
     * maxmimum number of parallel servers; default of 0 means 'use
     * parallel update statistics and determine number of servers
     * automatically', n > 0: use a maximum of n servers; n = -1: do
     * not use code for parallel update statistics
     */
    static SAPDB_Int4 m_noParallelServers;

    /*!
     * determines which sample algorithm to use during update statistics
     */
    static SAPDB_Int m_sampleAlgoNo;

    SAPDB_Bool AreStatisticsUnnecessary(
        const Catalog_TableDescriptor&  tableDesc,
        const Catalog_ColumnDescriptor& columnDesc) const;

    SAPDB_Bool AreStatisticsPossible(
        const Catalog_ColumnDescriptor& columnDesc,
        SAPDB_Bool                      bThrowErrorIfNot = false,
        SAPDB_Int                       stmtPos = 1);

    void AnalyzeUpdateStatisticsColumnCommand(Msg_List& errMsg);

    ReturnCode UpdateSysTables(
        Catalog_TableDescriptor&        tableDesc,
        const Catalog_ColumnDescriptor* columnDesc,
        const StatisticsInfo&           statisticsInfo,
        SAPDB_Int                       seqNo,
        Msg_List&                       errMsg);

    ReturnCode UpdateStatisticsLogTable(
        const Catalog_TableDescriptor& tableDesc,
        const StatisticsInfo&          statisticsInfo,
        const SQLMan_Identifier&       columnName,
        SAPDB_Int                      seqNo,
        Msg_List&                      errMsg);

    enum {
        cStatValUndefined = -1,
        cStatValNotSet    = 0
    };

    SAPDB_Bool DetermineSamplePageCount(
        const Catalog_TableDescriptor& tableDesc,
        SAPDB_Int4&                    noOfPagesToSample,
        Msg_List&                      errMsg);

    SAPDB_Bool GetTablePageCount(
        const FileDir_FileNo& tableFileNo,
        SAPDB_Int&            pageCount,
        Msg_List&             errMsg);

    SAPDB_Bool IgnoreTable(const Catalog_TableDescriptor& tableDesc);

    SAPDB_Bool InsertIntoColumnList(
        const Catalog_ColumnDescriptor& colDesc,
        ColumnNumberVector&             columnNumbers,
        StackCodeVector&                stackCode,
        Msg_List&                       errMsg);

    SAPDB_Bool IsInColumnList(
        const Catalog_ColumnDescriptor& colDesc,
        const ColumnNumberVector&       columnNumbers);

    SAPDB_Bool UpdateTreeLeafNodes(
        const FileDir_Table& tableFile,
        const SAPDB_Int      leafPageCount);

    class AutoSwitchToInternal
    {
    private:
        InternalTransInfo& m_internalTransInfo;

        AutoSwitchToInternal(const AutoSwitchToInternal&);
        AutoSwitchToInternal& operator=(const AutoSwitchToInternal&);
    public:
        AutoSwitchToInternal(InternalTransInfo& internalTransInfo)
            : m_internalTransInfo(internalTransInfo)
        {
            m_internalTransInfo.SwitchToInternal();
        }

        ~AutoSwitchToInternal()
        {
            m_internalTransInfo.SwitchToParent();
        }
    };
};

/**************************************************************************/

inline SAPDB_Bool SQLMan_UpdateStatistics::IsParallel()
{
    return m_noParallelServers >= 0;
}

/**************************************************************************/

inline void SQLMan_UpdateStatistics::SetNoOfParallelServers(
    const SAPDB_Int noOfServers)
{
    m_noParallelServers = noOfServers;
}

/**************************************************************************/

inline SAPDB_Int SQLMan_UpdateStatistics::GetNoOfParallelServers()
{
    return m_noParallelServers;
}

/**************************************************************************/

inline void SQLMan_UpdateStatistics::SetSampleAlgoNo(
    const SAPDB_Int sampleAlgoNo)
{
    m_sampleAlgoNo = sampleAlgoNo;
}

/**************************************************************************/

inline SAPDB_Int SQLMan_UpdateStatistics::GetSampleAlgoNo()
{
    return m_sampleAlgoNo;
}

/**************************************************************************/

#endif // SQLMAN_UPDATESTATISTICS_HPP
