/*****************************************************************************/
/*!
  @file         Data_PrimTreeStatisticsCoordinator.hpp
  @author       MartinKi
  @ingroup

  @brief        Header file for class Data_PrimTreeStatisticsCoordinator.

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

#ifndef DATA_PRIMTREESTATISTICSCOORDINATOR_HPP
#define DATA_PRIMTREESTATISTICSCOORDINATOR_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SQLManager/SQLMan_Types.hpp"

#include "FileDirectory/FileDir_FileNo.hpp"
#include "DataAccess/Data_Types.hpp"

#include "Container/Container_Vector.hpp"

#include "SAPDBCommon/SAPDB_Types.hpp"

#include "RunTime/System/RTESys_Time.h"
#include "RunTime/RTE_Types.hpp"

#include "ggg00.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

class Msg_List;
class SrvTasks_JobCollectTableStatistics;
class SQLMan_UpdateStatistics;
class Data_UpdStatRequestFile;

// the following classes are locally defined in
// Data_PrimTreeStatisticsCoordinator:
namespace Data_PrimTree_StatisticsCoordinator {
    class JobQueue;
    class JobInfo;
    class TableInfo;
}

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
 * @class  Data_PrimTreeStatisticsCoordinator
 * @author MartinKi
 *
 * @brief Coordinates the computation of a set of table statistics
 *        given in a request file.
 *
 */
class Data_PrimTreeStatisticsCoordinator
{
public:
    friend class Data_PrimTree_StatisticsCoordinator::JobInfo;
    friend class Data_PrimTree_StatisticsCoordinator::JobQueue;

    typedef Data_PrimTree_StatisticsCoordinator::JobQueue  JobQueue;
    typedef Data_PrimTree_StatisticsCoordinator::JobInfo   JobInfo;
    typedef Data_PrimTree_StatisticsCoordinator::TableInfo TableInfo;

    /// Possible return codes.
    enum ReturnCode {
        Okay,
        Cancelled,
        SystemError
    };

    typedef Container_Vector<SQLMan_StackEntry> StackCodeVector;
    typedef Container_Vector<SAPDB_Int>         ColumnNumberVector;
    typedef Container_Vector<SAPDB_Int4>        DistinctValueVector;

    /*!
     * @brief Constructor.
     */
    Data_PrimTreeStatisticsCoordinator(
        tgg00_TransContext&      transContext,
        SQLMan_UpdateStatistics& updStatObj,
        Data_UpdStatRequestFile& updStatRequestFile);

    /*!
     * @brief Destructor.
     */
    ~Data_PrimTreeStatisticsCoordinator();


    /*!
     * @brief Processes all requests.
     */
    ReturnCode UpdateStatistics(
        Msg_List& errMsg);

    class StatisticsInfo
    {
    private:
        friend class Data_PrimTree_StatisticsCoordinator::TableInfo;

        FileDir_FileNo                    m_tableFileNo;
        SAPDB_Bool                        m_wholeTableProcessed;
        RTESys_SQLTimestamp               m_beginTimeStamp;
        RTESys_SQLTimestamp               m_endTimeStamp;
        Data_PageCount                    m_leafPageCount;
        Data_PageCount                    m_recordCount;
        ColumnNumberVector*               m_columnNumbers;
        DistinctValueVector*              m_distinctValues;

        StatisticsInfo()
            : m_leafPageCount( 0 ),
              m_recordCount( 0 ),
              m_columnNumbers( 0 ),
              m_distinctValues( 0 )
        {}
    public:
        const FileDir_FileNo& GetTableFileNo() const
        {
            return m_tableFileNo;
        }

        SAPDB_Bool IsWholeTableProcessed() const
        {
            return m_wholeTableProcessed;
        }

        const RTESys_SQLTimestamp& GetBeginTimeStamp() const
        {
            return m_beginTimeStamp;
        }

        const RTESys_SQLTimestamp& GetEndTimeStamp() const
        {
            return m_endTimeStamp;
        }

        Data_PageCount GetLeafPageCount() const
        {
            return m_leafPageCount;
        }

        Data_PageCount GetRecordCount() const
        {
            return m_recordCount;
        }

        SAPDB_UInt GetDistinctValueCount() const
        {
            return m_columnNumbers->GetSize();
        }

        const ColumnNumberVector&
        GetColumnNumbers() const
        {
            return *m_columnNumbers;
        }

        const DistinctValueVector&
        GetDistinctValues() const
        {
            return *m_distinctValues;
        }
    };

private:
    // no copy-construction of assignment
    Data_PrimTreeStatisticsCoordinator(
        const Data_PrimTreeStatisticsCoordinator&);
    Data_PrimTreeStatisticsCoordinator operator=(
        const Data_PrimTreeStatisticsCoordinator&);

    friend class SrvTasks_JobCollectTableStatistics;

    void BuildTempFileId(
        tgg00_FileId& fileId,
        SAPDB_Int4    currFileIndex);

    ReturnCode SelfProcessJob(
        JobQueue& jobQueue,
        JobInfo&  job,
        Msg_List& errMsg);

    ReturnCode ProcessResult(
        JobQueue&                           jobQueue,
        SrvTasks_JobCollectTableStatistics& job,
        Msg_List&                           errMsg);

    SAPDB_Bool ReduceUsableServerCount(Msg_List& errMsg);

    SAPDB_Bool               m_resourceShortage;
    SAPDB_UInt               m_usableServerCount;
    tgg00_TransContext&      m_trans;
    Data_UpdStatRequestFile& m_updStatRequestFile;
    SQLMan_UpdateStatistics& m_updStatObj;
};

#endif // DATA_PRIMTREESTATISTICSCOORDINATOR_HPP
