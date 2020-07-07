/*****************************************************************************/
/*!
  @file         Data_UpdateCounterSrvTask.hpp
  @author       MartinKi

  @brief        Header file for class Data_UpdateCounterSrvTask.

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

#ifndef DATA_UPDATECOUNTERSRVTASK_HPP
#define DATA_UPDATECOUNTERSRVTASK_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "ServerTasks/SrvTasks_Job.hpp"

#include "FileDirectory/FileDir_FileNo.hpp"

#include "DataAccess/Data_Types.hpp"

#include "RunTime/Tasking/RTETask_Task.h"
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

/*!
 * @brief Server task that initializes the file directory statistics
 * information of a file.
 *
 * @sa SQLMan_UpdateCounter
 *
 * @author MartinKi
 */
class Data_UpdateCounterSrvTask : public SrvTasks_Job
{
private:
    //no copy-construction or assignment
    Data_UpdateCounterSrvTask(const Data_UpdateCounterSrvTask&);
    Data_UpdateCounterSrvTask operator=(Data_UpdateCounterSrvTask&);

    SAPDB_UInt8 m_filePageCount;
public:
    /*!
     * @brief Possible return codes of the server task execution.
     */
    enum ReturnCode {
        rcOk,
        rcFileNotFound,
        rcFileBad,
        rcCancelled,
        rcSystemError
    };

    /*!
    * @brief Constructor.
    */
    Data_UpdateCounterSrvTask(
        const Attributes&       jobAttributes,
        const FileDir_FileNo&   fileNo,
        SrvTasks_WaitingTask&   waitingTask);

    /*!
     * @brief Returns the return code of the server task execution.
     */
    ReturnCode GetReturnCode(Msg_List& errMsg);

    /*!
     * @brief Returns the number of pages the file consists of.
     *
     * Not initialized if return code after execute is not rcOk.
     */
    SAPDB_UInt8 GetFilePageCount() const;


    /*!
     * @brief Returns the file no of the file processed.
     */
    FileDir_FileNo GetFileNo() const;

    /*!
     * @copydoc SrvTasks_Jobs::Deallocate
     */
    void Deallocate() {}

protected:
    /*!
     * @brief Called by server task handling to start counter update.
     *
     * @copydoc SrvTasks_Jobs::ExecuteIntern
     */
    Result ExecuteIntern(Trans_Context &trans);

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
    /*@}*/

private:
    FileDir_FileNo m_updateFileNo;
    ReturnCode     m_returnCode;
};

/*****************************************************************************/

inline Data_UpdateCounterSrvTask::ReturnCode
Data_UpdateCounterSrvTask::GetReturnCode(Msg_List& errMsg)
{
    this->GetResult( errMsg ); // get message list, mark job done internally
    return m_returnCode;
}

/*****************************************************************************/

inline SAPDB_UInt8
Data_UpdateCounterSrvTask::GetFilePageCount() const
{
    return m_filePageCount;
}

/*****************************************************************************/

inline FileDir_FileNo
Data_UpdateCounterSrvTask::GetFileNo() const
{
    return m_updateFileNo;
}

/*****************************************************************************/

#endif // DATA_UPDATECOUNTERSRVTASK_HPP
