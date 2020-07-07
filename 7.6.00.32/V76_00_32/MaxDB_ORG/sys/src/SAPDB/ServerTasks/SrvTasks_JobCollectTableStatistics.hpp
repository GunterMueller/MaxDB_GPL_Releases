/*****************************************************************************/
/*!
  @file         SrvTasks_JobCollectTableStatistics.hpp
  @author       MartinKi
  @ingroup

  @brief        Header file for class SrvTasks_JobCollectTableStatistics.

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

#ifndef SRVTASKS_JOBCOLLECTTABLESTATISTICS_HPP
#define SRVTASKS_JOBCOLLECTTABLESTATISTICS_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "DataAccess/Data_PrimTreeStatisticsCoordinator.hpp"
#include "DataAccess/Data_Types.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "ServerTasks/SrvTasks_Job.hpp"

#include "Messages/Msg_List.hpp"
#include "RunTime/Tasking/RTETask_Task.h"
#include "SAPDBCommon/SAPDB_Types.hpp"

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

class  FileDir_Table;
class  SAPDBMem_IRawAllocator;
class  Trans_Context;
struct tbd_current_tree;

/*!
 * @class  SrvTasks_JobCollectTableStatistics
 * @author MartinKi
 *
 * @brief Computes statistics for a given single table.
 *
 * @sa Data_PrimTreeStatisticsCoordinator
 */
class SrvTasks_JobCollectTableStatistics : public SrvTasks_Job
{
public:
    /// Possible return codes.
    enum ReturnCode {
        ResultOkay,
        ResultTableFileNotFound,
        ResultOldFileVersion,
        ResultCancelled,
        ResultSystemError
    };

    /// Constructor.
    SrvTasks_JobCollectTableStatistics(
        const Attributes&       jobAttributes,
        const FileDir_FileNo&   tableFileNo,
        Data_PrimTreeStatisticsCoordinator::DistinctValueVector& distinctValues,
        tsp00_Key               startKey,
        SAPDB_Int               startKeyLen,
        tsp00_Key               stopKey,
        SAPDB_Int               stopKeyLen,
        Data_PrimTreeStatisticsCoordinator::StackCodeVector& stackCode,
        const FileDir_FileNo&   auxFileNo,
        const SAPDB_Int4        noOfPagesToSample,
        SrvTasks_WaitingTask&   waitingTask);

    /// Destructor.
    ~SrvTasks_JobCollectTableStatistics();

    /*!
     * @brief Returns the fileno of the table that is being processed.
     */
    FileDir_FileNo GetTableFileNo() const;

    /*!
     * @brief Returns the fileno of the temporary file that is used to
     * store information during the statistics computation process.
     */
    const FileDir_FileNo& GetAuxFileNo() const;

    const Data_PrimTreeStatisticsCoordinator::DistinctValueVector&
    GetDistinctValues() const;

    /*!
     * @brief Called by server task handling to start statistics computation.
     *
     * @copydoc SrvTasks_Jobs::Execute
     */
    Result ExecuteIntern(Trans_Context &trans);

    /*!
     * @brief Returns the result code of the execution.
     *
     * @return
     *   - ResultOkay, on success
     *   - ResultTableFileNotFound, if the table for which statistics
     *     should be computed could not be found (e.g. it might have
     *     been deleted after having been entered in the request file)
     *   - ResultOldFileVersion, if the job was aborted due to a file
     *     version change on the table processed
     *   - ResultCancelled, if the job was cancelled during execution
     *   - ResultSystemError, if an error occured during execution; in this
     *     case errMsg will be set
     */
    ReturnCode GetReturnCode(Msg_List& errMsg);

    Data_PageCount GetLeafPageCount() const;
    Data_PageCount GetRecordCount() const;

    /*!
     * @name Unused methods required by SrvTasks_Job interface
     */
    /*@{*/

    /*!
     * @copydoc SrvTasks_Jobs::CreateCopy
     */
    bool CreateCopy(
        SAPDBMem_IRawAllocator& allocator,
        Msg_List&               errlist,
        SrvTasks_Job*&          jobCopy ) const;

    /*!
     * @copydoc SrvTasks_Jobs::Deallocate
     */
    void Deallocate() {}
    /*@}*/

protected:
    /*!
     * @brief Used to start statistics computation directly.
     *
     * @warning To check whether the computation was successful,
     * GetReturnCode should be used. GetResult will always return
     * StandardResult::ok as long as no SystemError occured. This
     * implies that GetResult will also return ok if the table could
     * not be processed because it could not be found. GetReturnCode
     * will return ResultTableFileNotFound in this case.
     *
     * @copydoc SrvTasks_Jobs::Execute
     */
    void Execute(tgg00_TransContext& trans);

private:
    FileDir_FileNo          m_tableFileNo;
    FileDir_FileNo          m_auxFileNo;
    ReturnCode              m_returnCode;
    SAPDB_Int4              m_noOfPagesToSample;
    Data_PrimTreeStatisticsCoordinator::StackCodeVector&     m_stackCode;
    Data_PrimTreeStatisticsCoordinator::DistinctValueVector& m_distinctVals;
    Data_PageCount          m_leafPageCount;
    Data_PageCount          m_recordCount;
    tsp00_Key               m_startKey;
    tsp00_Key               m_stopKey;
    SAPDB_Int               m_startKeyLen;
    SAPDB_Int               m_stopKeyLen;

    void InitDistinctValueVector(Trans_Context& trans);

    void CollectStatistics(
        tgg00_TransContext& trans,
        FileDir_Table&      tableFile,
        FileDir_Table&      auxFile,
        const SAPDB_Int2&   columnCount,
        Msg_List&           errMsg);

    void DetermineLeafPageAndRecordCount(
        tgg00_TransContext& trans,
        FileDir_Table&      tableFile,
        Msg_List&           errMsg);

    class AutoTreeLock
    {
    private:
        tbd_current_tree& m_current;
        bool              m_lockSet;

    public:
        enum LockType {
            LockExclusive,
            LockNonExclusive
        };

        AutoTreeLock(
            tgg00_TransContext& trans,
            tgg00_FileId&       fileId,
            tbd_current_tree&   current,
            LockType            lock);

        ~AutoTreeLock();

        void Release();
    };
};

/**************************************************************************/

inline FileDir_FileNo
SrvTasks_JobCollectTableStatistics::GetTableFileNo() const
{
    return m_tableFileNo;
}

/**************************************************************************/

inline SrvTasks_JobCollectTableStatistics::ReturnCode
SrvTasks_JobCollectTableStatistics::GetReturnCode(
    Msg_List& errMsg)
{
    this->GetResult( errMsg ); // get message list, mark job done internally
    return m_returnCode;
}

/**************************************************************************/

inline Data_PageCount
SrvTasks_JobCollectTableStatistics::GetLeafPageCount() const
{
    return m_leafPageCount;
}

/**************************************************************************/

inline Data_PageCount
SrvTasks_JobCollectTableStatistics::GetRecordCount() const
{
    return m_recordCount;
}

/**************************************************************************/

inline const Data_PrimTreeStatisticsCoordinator::DistinctValueVector&
SrvTasks_JobCollectTableStatistics::GetDistinctValues() const
{
    return m_distinctVals;
}

/**************************************************************************/

#endif // SRVTASKS_JOBCOLLECTTABLESTATISTICS_HPP
