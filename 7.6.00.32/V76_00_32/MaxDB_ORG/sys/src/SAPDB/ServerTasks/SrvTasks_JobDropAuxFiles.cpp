/*!
  @file        SrvTasks_JobDropAuxFiles.cpp
  @author      UweH
  @ingroup     ServerTasks
  @brief       This is the implementation of SrvTasks_JobDropAuxFiles
  @since       2004-06-08

\if EMIT_LICENCE
  copyright:    Copyright (c) 2001-2005 SAP AG


    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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
#include "ServerTasks/SrvTasks_JobDropAuxFiles.hpp"
#include "ServerTasks/SrvTasks_WaitingTask.hpp"
#include "ServerTasks/SrvTasks_Task.hpp"
#include "ServerTasks/SrvTasks_Types.hpp"
#include "ServerTasks/SrvTasks_ITaskForJob.hpp"
#include "KernelCommon/Kernel_Trace.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "SAPDBCommon/Messages/SDBMsg_SrvTasks.h"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "RunTime/RTE_Message.hpp"
#include "ggg00.h"  // tgg00_TransContext,tgg00_MessBlock
#include "hkb64.h"  // k64build_aux_fn_prefix
#include "hbd01.h"  // b01prefix_destroy_files
#include "hkb900.h" // pascal interface
#include "heo55k.h" // RTE   : vbegexcl,venexcl 
#include "hgg08.h"  // g08servertask
#include "hbd02.h"  // b02add_record,b02next_record
#include "hbd01_1.h"  // b01niltree_id
#include "hbd998.h" // bd998NewTempFileNo,bd998CreateNamedTempFile
//--------------------------------------------------------------------------------------
/// This class defines a coordinator for the pascal environment to start the jobs.
class SrvTasks_DropAuxFileCoordinator
{
public:
    /// set pending drop counter to 0
    SrvTasks_DropAuxFileCoordinator()
    : m_PendingDrops (0),
      m_ServerStarted(false)
    {
        m_FileId.fileRoot_gg00() = NIL_PAGE_NO_GG00;
    }
    /// adds a record to a temp file and triggers a servertask if necessary
    void DropAuxFiles(tgg00_TransContext &trans)
    {
        Msg_List                 &errlist = *(reinterpret_cast<Msg_List*>(trans.trErrorList_gg00));
        SrvTasks_JobDropAuxFiles  job (trans.trWriteTransId_gg00);

        if ( job.ExecuteByServer(*RTETask_ITask::Self(),errlist) != SrvTasks_Job::executeOK )
        {
            // if no server task available
            AddToFile (trans, trans.trWriteTransId_gg00);
            if ( trans.trError_gg00 != e_ok )
            {
                RTE_Message(Msg_List(Msg_List::Info,SDBMSG_SRVTASKS_DROP_AUX_FILE_EXECUTED_DIRECTLY));
                trans.trError_gg00 = e_ok;
                Trans_Context &context = Trans_Context::GetContext(trans);
                job.ExecuteDirectly(context);
                trans.trError_gg00 = job.GetResult(context.ErrorList());
            }
        }
    }
    /// this adds a record to the temp file which is checked by the timeout task
    void AddToFile ( tgg00_TransContext  &trans,
                     const tgg91_TransNo &fileTransId )
    {
        if ( m_FileId.fileRoot_gg00() == NIL_PAGE_NO_GG00 )
            CreateFile(trans);

        if ( trans.trError_gg00 != e_ok )
            return;
        
        TransIdRec transRec;

        transRec.RecHeader.hrecLen_gg00()          = sizeof (transRec);
        transRec.RecHeader.hrecKeyLen_gg00()       = sizeof (transRec.TransId);
        transRec.RecHeader.hrecVarcolOffset_gg00() = 0;
        transRec.RecHeader.hrecVarcolCnt_gg00()    = 0;
        transRec.TransId                           = fileTransId;
        
        tgg00_RecPtr pRec = reinterpret_cast<tgg00_RecPtr>(&transRec);

        b02add_record (trans, m_FileId, *pRec);
    
        if ( trans.trError_gg00 == e_ok ) // no duplicates
        {
            vbegexcl (trans.trTaskId_gg00, g08server);
            ++m_PendingDrops;
            vendexcl (trans.trTaskId_gg00, g08server);
        }
        else if ( trans.trError_gg00 == e_duplicate_key )
            trans.trError_gg00 = e_ok;
    }

    /// Check for timeout task
    void CheckAndStartDropServer(tsp00_TaskId taskid)
    {
        bool startServer = false;
        vbegexcl (taskid, g08server);
        if ( m_PendingDrops > 0 && ! m_ServerStarted )
        {
            m_ServerStarted = true;
            startServer     = true;
        }
        vendexcl (taskid, g08server);
        if ( startServer )
        {
            Msg_List      dummyerrlist;
            tgg91_TransNo zerotransid;

            zerotransid.gg90SetZero();
            SrvTasks_JobDropAuxFiles  job (zerotransid);
        
            if ( job.ExecuteByServer(*RTETask_ITask::Self(),dummyerrlist) != SrvTasks_Job::executeOK )
                SetServerStopped(taskid);
        }
    }
    /// The server task indicates that it is stopping
    void SetServerStopped(tsp00_TaskId taskid)
    {
        vbegexcl (taskid, g08server);
        m_ServerStarted = false;
        vendexcl (taskid, g08server);
    }
    /// returns the next transid for which aux files should be dropped, false if nothing found
    bool GetNextTransId (tgg00_TransContext &trans,
                         tgg91_TransNo      &transid)
	{
        trans.trError_gg00 = e_ok;

        if ( m_FileId.fileRoot_gg00() == NIL_PAGE_NO_GG00 )
            return false;

        tgg00_Lkey  key;
        tgg00_Rec   rec;
        TransIdRec *transRec = reinterpret_cast<TransIdRec*>(&rec);
        
        key.keyLen_gg00() = 0;
        b02next_record (trans, m_FileId, key, false, rec);

        if ( trans.trError_gg00 == e_ok
             ||
             trans.trError_gg00 == e_key_not_found )
        {
            trans.trError_gg00 = e_ok;
            transid            = transRec->TransId;
            b02del_record (trans, m_FileId, rec.recKey_gg00());
            trans.trError_gg00 = e_ok;  // ignore errors
            vbegexcl (trans.trTaskId_gg00, g08server);
            if ( m_PendingDrops > 0 )
                --m_PendingDrops;
            vendexcl (trans.trTaskId_gg00, g08server);
            return true;
        }
        trans.trError_gg00 = e_ok;  // reset no next record
        vbegexcl (trans.trTaskId_gg00, g08server);
        m_ServerStarted = false;
        vendexcl (trans.trTaskId_gg00, g08server);
        return false;
	}
private:
    /// create temp file
    void CreateFile(tgg00_TransContext &trans)
    {
        trans.trError_gg00 = e_ok;

        vbegexcl (trans.trTaskId_gg00, g08server);

        if ( m_FileId.fileRoot_gg00() != NIL_PAGE_NO_GG00 )
        {
            vendexcl (trans.trTaskId_gg00, g08server);
            return;
        }

        m_FileId                    = b01niltree_id;
        m_FileId.fileTfn_gg00().becomes    (tfnTempAux_egg00);
        m_FileId.fileTfn_gg00().becomes    (tfnTemp_egg00);
        m_FileId.fileTfnTemp_gg00().becomes(ttfnNone_egg00);

        m_FileId.fileTransId_gg00().gg90SetDummy();

        m_FileId.fileType_gg00().addElement(ftsShared_egg00    );
        m_FileId.fileType_gg00().addElement(ftsTemp_egg00      );
        m_FileId.fileType_gg00().addElement(ftsConcurrent_egg00);

        /* FIXME: workaround until tfnTempAux_egg00 will not get mapped to */
        /* CompatEntry in file directory any longer:                       */
        bd998NewTempFileNo (m_FileId.fileTabId_gg00());

        b01filestate (trans, m_FileId);
        
        if ( trans.trError_gg00 == e_file_not_found )
        {
            trans.trError_gg00 = e_ok;
            // file exists until next shutdown:
            bd998CreateNamedTempFile (trans, m_FileId.fileTabId_gg00(),
                                      m_FileId, false, ttfnNone_egg00);
            if ( trans.trError_gg00 != e_ok )
            {
                vendexcl (trans.trTaskId_gg00, g08server);
                return;
            }
        }
        vendexcl (trans.trTaskId_gg00, g08server);
    }
private:
    /// record structure for temp records
    struct TransIdRec
    {
        tgg00_HeaderRec RecHeader;
        tgg91_TransNo   TransId;
    };
    /// if greater 0 there exist aux files to drop
    SAPDB_UInt   m_PendingDrops;
    /// this is the file which contains drop orders
    tgg00_FileId m_FileId;
    /// if this is set to true a server tasks is triggered to remove aux files
    bool         m_ServerStarted;
};
//--------------------------------------------------------------------------------------
/// the drop handler
SrvTasks_DropAuxFileCoordinator SrvTasks_DropAuxFileHandler;
//--------------------------------------------------------------------------------------
/// wrapper to start a job
externPascal void kb900DropAuxFiles (
    tgg00_TransContext    VAR_VALUE_REF  trans)
{
    SAPDBTRACE_ROUTINE_DEBUG ("kb900DropAuxFiles", SrvTasks_Trace, 5);
    SrvTasks_DropAuxFileHandler.DropAuxFiles(trans);
}
//--------------------------------------------------------------------------------------
/// wrapper to fill a record into the list of to dropp files
externPascal void kb900AddToPrefixDestroyFile (
    tgg00_TransContext    VAR_VALUE_REF  trans,
    tgg91_TransNo         VAR_VALUE_REF  fileTransId)
{
    SAPDBTRACE_ROUTINE_DEBUG ("kb900AddToPrefixDestroyFile", SrvTasks_Trace, 5);
    SrvTasks_DropAuxFileHandler.AddToFile(trans, fileTransId);
}
//--------------------------------------------------------------------------------------
/// If something should be dropped start a server and drop it
void SrvTasks_JobDropAuxFiles::CheckAndStartDropServer (tsp00_TaskId taskid)
{
    SAPDBTRACE_ROUTINE_DEBUG ("SrvTasks_JobDropAuxFiles::CheckAndStartDropServer", SrvTasks_Trace, 5);
    SrvTasks_DropAuxFileHandler.CheckAndStartDropServer(taskid);
}
//--------------------------------------------------------------------------------------
SrvTasks_JobDropAuxFiles::SrvTasks_JobDropAuxFiles(const tgg91_TransNo &transno)
: SrvTasks_Job(jobDropAuxfiles, SrvTasks_Job::Attributes(0)),
  m_TransId (transno)
{
    SAPDBTRACE_METHOD_DEBUG ("SrvTasks_JobDropAuxFiles::SrvTasks_JobDropAuxFiles", SrvTasks_Trace, 5);
}
//--------------------------------------------------------------------------------------
bool SrvTasks_JobDropAuxFiles::CreateCopy ( SAPDBMem_IRawAllocator &allocator, 
                                            Msg_List               &errlist,
                                            SrvTasks_Job*          &jobCopy ) const
{
    SAPDBTRACE_METHOD_DEBUG ("SrvTasks_JobDropAuxFiles::CreateCopy", SrvTasks_Trace, 5);
    // This is only called if nobody wants to wait for this job.
    // If nobody is waiting then no output field is needed - and copied
    jobCopy = new (allocator) SrvTasks_JobDropAuxFiles(m_TransId);
    if ( 0 == jobCopy )
    {
        AppendContextInfo (__CONTEXT__, errlist, "This job could not be copied");
        return false;
    }
    jobCopy->SetAttributes(m_Attributes);
    return true;
}
//--------------------------------------------------------------------------------------
SrvTasks_Job::Result SrvTasks_JobDropAuxFiles::ExecuteIntern (Trans_Context &trans)
{
    SAPDBTRACE_METHOD_DEBUG ("SrvTasks_JobDropAuxFiles::ExecuteIntern", SrvTasks_Trace, 5);
    
    tgg00_BasisError &trError = trans.OldTrans().trError_gg00;
    
    const bool dropFilesFromFile = m_TransId.gg90IsZero();
    
    do
    {
        if ( dropFilesFromFile )
        {
            if ( ! SrvTasks_DropAuxFileHandler.GetNextTransId(trans.OldTrans(), trans.OldTrans().trWriteTransId_gg00) )
                break;
        }
        else
        {
            trans.OldTrans().trWriteTransId_gg00 = m_TransId;
        }
        
        SAPDB_Int    prefixLength;
        tgg00_FileId auxFileId;
    
        trError = e_ok;
    
        k64build_aux_fn_prefix (trans.OldTrans().trWriteTransId_gg00, true, auxFileId, prefixLength);
        b01prefix_destroy_files (trans.OldTrans(), auxFileId.fileName_gg00(), prefixLength);
    
        if ( trError == e_file_not_found )
            trError = e_ok;
        
        if ( trError != e_ok )
        {
            GetTransContext().ErrorList().ClearMessageList();
            Kernel_AppendBasisError (trError,GetTransContext().ErrorList(),"b01prefix_destroy_files");
            m_Result = trError;
        }
    }
    while (dropFilesFromFile);

    SrvTasks_DropAuxFileHandler.SetServerStopped(trans.Task().ID());

    trError = e_ok;

    return m_Result;
}
//--------------------------------------------------------------------------------------
SrvTasks_Job::Result SrvTasks_JobDropAuxFiles::ExecuteByRedoTask (tgg00_TransContext &trans,
                                                                  Msg_List           &errlist)
{
    SAPDBTRACE_ROUTINE_DEBUG ("SrvTasks_JobDropAuxFiles::ExecuteByRedoTask", SrvTasks_Trace, 5);

    SrvTasks_JobDropAuxFiles job ( trans.trWriteTransId_gg00 );

    job.ExecuteDirectly (Trans_Context::GetContext(trans));

    return job.GetResult(errlist);
}
