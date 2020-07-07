/*****************************************************************************/
/*!
  @file         Data_UpdateCounterSrvTask.cpp
  @author       MartinKi

  @brief        Implementation file for class Data_UpdateCounterSrvTask.

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

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "DataAccess/Data_UpdateCounterSrvTask.hpp"

#include "SQLManager/ErrorHandling/SQLManErr_Interface.hpp"

#include "ServerTasks/SrvTasks_ITaskForJob.hpp"
#include "ServerTasks/SrvTasks_JobTypes.hpp"
#include "SAPDBCommon/Messages/SDBMsg_SrvTasks.h"

#include "FileDirectory/FileDir_IDirectory.hpp"
#include "FileDirectory/FileDir_Table.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"

#include "DataAccess/Data_Types.hpp"
#include "SAPDBCommon/Messages/SDBMsg_Data.h"
#include "KernelCommon/Kernel_VTrace.hpp"

#include "Messages/Msg_List.hpp"
#include "RunTime/Tasking/RTETask_Task.h"
#include "SAPDBCommon/SAPDB_Types.hpp"

#include "hbd01.h"
#include "hbd03.h"
#include "gbd00.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

Data_UpdateCounterSrvTask::Data_UpdateCounterSrvTask(
    const Attributes&       jobAttributes,
    const FileDir_FileNo&   updateFileNo,
    SrvTasks_WaitingTask&   waitingTask)
    : SrvTasks_Job( jobUpdateCounter, jobAttributes, &waitingTask ),
      m_updateFileNo( updateFileNo ),
      m_filePageCount( 0 )
{}

/*****************************************************************************/

bool Data_UpdateCounterSrvTask::CreateCopy(
    SAPDBMem_IRawAllocator& allocator,
    Msg_List&               errlist,
    SrvTasks_Job*&          jobCopy ) const
{
    jobCopy = new (allocator) Data_UpdateCounterSrvTask(
        m_Attributes,
        m_updateFileNo,
        *m_WaitingTask );

    if ( 0 == jobCopy )
    {
        AppendContextInfo (__CONTEXT__, errlist, "This job could not be copied");
        return false;
    }

    jobCopy->SetAttributes(m_Attributes);
    return true;
}

/*****************************************************************************/

SrvTasks_Job::Result
Data_UpdateCounterSrvTask::ExecuteIntern(Trans_Context &trans)
{
    SAPDBTRACE_METHOD_DEBUG(
        "Data_UpdateCounterSrvTask::ExecuteIntern",
        DataTree_Trace, 5 );

    Msg_List& errMsg         = trans.ErrorList();
    FileDir_IDirectory& fDir = FileDir_IDirectory::SessionInstance( trans.OldTrans() );

    /*
     * check if table and aux file exist
     */
    if ( DataTree_Trace.TracesLevel( 5 ) ) {
        Kernel_VTrace()
            << "Data_UpdateCounterSrvTask:" << NewLine
            << "  processing file: " << m_updateFileNo
            << ", jobId = " << this->GetId();
    }

    FileDir_Generic fileEntry;
    FileDir_ReturnCode fDirRc = fDir.GetFile(
        trans.TaskId(),
        m_updateFileNo,
        fileEntry,
        errMsg );

    if ( fDirRc != FileDir_Okay ) {
        switch ( fDirRc ) {
            case FileDir_FileNoNotFound:
                m_returnCode = rcFileNotFound;
                break;
            default:
                m_returnCode = rcSystemError;
        }
        return error;
    }

    tgg00_FileId fileId;
    fileEntry.BuildFileId( fileId );

    m_filePageCount = 0;

    switch ( fileEntry.GetFileType() ) {
        case Data_TableFile:
        {
            FileDir_Table tableEntry;
            tableEntry.Assign( fileEntry );
            const SAPDB_Bool cExtendedCheck     = true;
            const SAPDB_Bool cCheckLobExistence = true;
            const SAPDB_Bool cBeQuiet           = true; // no output to knldiag
            const SAPDB_Bool cUpdateLeaves      = true;
            const SAPDB_Int lobColCount = tableEntry.GetBlobColCount();
            b01check_table(
                trans.OldTrans(),
                fileId,
                lobColCount,
                lobColCount
                - ( (lobColCount > 0)
                    && tableEntry.HasShortColumnFile() ? 1 : 0),
                ! cExtendedCheck,
                cCheckLobExistence,
                cBeQuiet,
                ! cUpdateLeaves );

            m_filePageCount += tableEntry.GetLobPageCount();

            break;
        }
        case Data_IndexFile:
        {
            tgg00_SampleInfo statInfo;
            const SAPDB_Bool cWithSelectivity = true;

            bd03InvStatistics(
                trans.OldTrans(),
                fileId,
                ! cWithSelectivity,
                statInfo );

            if ( trans.OldTrans().trError_gg00 == e_ok ) {

                const SAPDB_Bool bSetInitialized = true;

                // start with leaf pages to make sure counters are
                // marked as initialized:
                fileEntry.SetLeafPageCount(
                    statInfo.leafnodes, bSetInitialized );
                fileEntry.SetIndexPageCount( statInfo.indexnodes );
                fileEntry.SetEntryCount( statInfo.sec_key_cnt );

            }

            if ( trans.OldTrans().trError_gg00 == e_file_not_accessible ) {
                trans.OldTrans().trError_gg00 = e_ok;
            }
            break;
        }
        default:
            m_returnCode = rcSystemError;
            return error;
    }

    if ( trans.OldTrans().trError_gg00 != e_ok ) {
        switch ( trans.OldTrans().trError_gg00 ) {
            case e_cancelled:
                m_returnCode = rcCancelled;
                break;
            case e_file_not_found:
                m_returnCode = rcFileNotFound;
                break;
            case e_file_not_accessible:
            case e_bad_datapage:
                m_returnCode = rcFileBad;
                break;
            default:
                trans.ErrorList().Overrule(
                    Msg_List(
                        Msg_List::Error,
                        SDBMSG_DATA_UPDCNT_COLLECTION_SYSTEM_ERROR,
                        Msg_Arg( SDBMSGTAG_DATA_UPDCNT_COLLECTION_SYSTEM_ERROR__BASIS_ERROR,
                                 SQLManErr_Interface::GetInstance()
                                 .GetBaseErrorText(
                                     trans.OldTrans().trError_gg00 )),
                        Msg_Arg( SDBMSGTAG_DATA_UPDCNT_COLLECTION_SYSTEM_ERROR__FILENO,
                                 FileDir_FileNoToString( m_updateFileNo )) )
                    );

                m_returnCode = rcSystemError;
        }
        return error;
    }

    m_filePageCount +=
        fileEntry.GetLeafPageCount( trans.TaskId() )
        + fileEntry.GetIndexPageCount( trans.TaskId() );

    m_returnCode = rcOk;
    return ok;
}
