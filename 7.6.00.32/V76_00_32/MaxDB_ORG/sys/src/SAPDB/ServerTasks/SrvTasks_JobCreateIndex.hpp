/*!
  @file           SrvTasks_JobCreateIndex.hpp
  @author         UweH
  @ingroup        ServerTasks
  @brief          This defines what a Job is.

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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
#ifndef SrvTasks_JobCreateIndex_HPP
#define SrvTasks_JobCreateIndex_HPP
//--------------------------------------------------------------------------------------
#include "ServerTasks/SrvTasks_Job.hpp"
#include "ServerTasks/SrvTasks_ITaskForJob.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "SAPDBCommon/Messages/SDBMsg_SrvTasks.h"
#include "KernelCommon/Kernel_Trace.hpp"
#include "ggg00.h" // tgg00_TransContext,tgg00_MessBlock
#include "hbd03.h" // bd03ParticipantCreateIndex
//--------------------------------------------------------------------------------------
/// This job is used for parallel Create Index jobs.
class SrvTasks_JobCreateIndex : public SrvTasks_Job
{
public:
    /// This describes the input data for this job.
    struct Input
    {
        tsp00_PageNo	 PrimQueuePno;		///< ...
        tsp00_PageNo     SecQueuePno;		///< ...
        tsp00_Int4		 NumLeavesToScan;	///< ...
        tsp00_Int4		 AvgRecordsPerLeaf;	///< ...
        tsp00_Int4		 StartPrimKeyLen;	///< ...
        tsp00_Int4		 StopPrimKeyLen;	///< ...
        tgg00_IndexMode  IndexMode;			///< ...
        tsp00_Bool       UseMemorySort;		///< ...
        tsp00_Int2       StackDescLen;		///< ...
        tgg00_FileId     PrimFileId;		///< ...
        tgg00_FileId     InvFileId;			///< ...
        tsp00_KeyPtr	 pStartPrimKey;		///< ...
        tsp00_KeyPtr	 pStopPrimKey;		///< ...
        tgg00_StackDesc *pStackDesc;  		///< ...
    };
    /// This describes the output data offered at the jobs end.
    struct Output
    {
        tgg00_IndexCounter IndexCounter;			///< ...
        tsp00_Int4         TempLeafPageCount;       /// temp file size in pages
        tsp00_PageNo       FirstLeafPno;			///< ...
        pasbool            EntriesExist;			///< ...
        /// initializes all members
        Output()
        {
            IndexCounter.idc_prim_leafnodes = 0;
            IndexCounter.idc_sec_leafnodes  = 0;
            IndexCounter.idc_prim_keycount  = 0;
            IndexCounter.idc_sec_keycount   = 0;
            IndexCounter.idc_nullcount      = 0;
            TempLeafPageCount               = 0;
            FirstLeafPno                    = NIL_PAGE_NO_GG00;
            EntriesExist                    = 0;
        }
    };
public:
    /// constructor
    SrvTasks_JobCreateIndex( SAPDB_UInt indexFileNo )
    : SrvTasks_Job(jobCreateIndex, Attributes(indexFileNo)),
      m_pAllocator(0)
    {}
    /// this must be called to allocate memory in the job
    bool Initialize(SAPDBMem_IRawAllocator &allocator,
                    Input                   input,
                    Msg_List               &errlist)
    {
        m_Input      = input;  
        m_pAllocator = &allocator;
        // ---------------------------------------------------------------------------
        m_Input.pStartPrimKey = reinterpret_cast<tsp00_KeyPtr>(allocator.Allocate(input.StartPrimKeyLen));
        if ( m_Input.pStartPrimKey == 0 )
        {
            errlist = Msg_List(Msg_List::Error,SDBMSG_SRVTASKS_ALLOCATION_FAILED);
            return false;
        }
        if ( ! SAPDB_MemCopy( m_Input.pStartPrimKey,
                              input.pStartPrimKey,
                              input.StartPrimKeyLen,
                              "ServerTasks", __CONTEXT__, errlist) )
        {
            return false;
        }
        // ---------------------------------------------------------------------------
        m_Input.pStopPrimKey = reinterpret_cast<tsp00_KeyPtr>(allocator.Allocate(input.StopPrimKeyLen));
        if ( m_Input.pStopPrimKey == 0 )
        {
            errlist = Msg_List(Msg_List::Error,SDBMSG_SRVTASKS_ALLOCATION_FAILED);
            return false;
        }
        if ( ! SAPDB_MemCopy( m_Input.pStopPrimKey,
                              input.pStopPrimKey,
                              input.StopPrimKeyLen,
                              "ServerTasks", __CONTEXT__, errlist) )
        {
            return false;
        }
        // ---------------------------------------------------------------------------
        m_Input.pStackDesc = reinterpret_cast<tgg00_StackDesc*>(allocator.Allocate(input.StackDescLen));
        if ( m_Input.pStackDesc == 0 )
        {
            errlist = Msg_List(Msg_List::Error,SDBMSG_SRVTASKS_ALLOCATION_FAILED);
            return false;
        }
        if ( ! SAPDB_MemCopy( m_Input.pStackDesc,
                              input.pStackDesc,
                              input.StackDescLen,
                              "ServerTasks", __CONTEXT__, errlist) )
        {
            return false;
        }
        return true;
    }
    /// this is used to access the m_Output parameters
    const Output& GetOutput() const
    {
        return m_Output;
    }
public:
    virtual ~SrvTasks_JobCreateIndex()
    {
        Deallocate();
    }
    virtual void Deallocate()
    {
        if ( m_pAllocator != 0 )
        {
            m_pAllocator->Deallocate(m_Input.pStackDesc);
            m_pAllocator->Deallocate(m_Input.pStopPrimKey);
            m_pAllocator->Deallocate(m_Input.pStartPrimKey);
            m_pAllocator = 0;
        }
    }
protected:
    virtual Result ExecuteIntern (Trans_Context &trans)
    {
        bd03ParticipantCreateIndex ( trans.OldTrans(),
                                     // --------------------
                                     m_Input.PrimFileId,
                                     m_Input.InvFileId,
                                     *(m_Input.pStackDesc),
                                     m_Input.pStartPrimKey,
                                     m_Input.pStopPrimKey,
                                     m_Input.StartPrimKeyLen,
                                     m_Input.StopPrimKeyLen,
                                     m_Input.NumLeavesToScan,
                                     m_Input.AvgRecordsPerLeaf,
                                     m_Input.PrimQueuePno,
                                     m_Input.SecQueuePno,
                                     m_Input.UseMemorySort,
                                     m_Input.IndexMode,
                                     // --------------------
                                     m_Output.IndexCounter,
                                     m_Output.TempLeafPageCount,
                                     m_Output.FirstLeafPno,
                                     m_Output.EntriesExist);

        tgg00_BasisError &trError = trans.OldTrans().trError_gg00;
        
        if ( trError != e_ok  )
        {
            if ( trError != e_duplicate_key
                 &&
                 trError != e_duplicate_invkey)
            {
                AppendContextInfo(__CONTEXT__, trans.ErrorList(), "Error occured");
                Kernel_AppendBasisError (trError,trans.ErrorList(),"Error occured");
            }
        }
        return trError;
    }
    virtual bool CreateCopy ( SAPDBMem_IRawAllocator &allocator, 
                              Msg_List               &errlist,
                              SrvTasks_Job*          &jobCopy ) const
    {
        SrvTasks_JobCreateIndex *pNewJob = new (allocator) SrvTasks_JobCreateIndex(m_Attributes.Id);
        if ( 0 == pNewJob )
        {
            AppendContextInfo (__CONTEXT__, errlist, "This job could not be copied");
            return false;
        }
        if ( ! pNewJob->Initialize(allocator, m_Input, errlist) )
            return false;
        jobCopy = pNewJob;
        return true;
    }
private:
    SAPDBMem_IRawAllocator *m_pAllocator;
    Input                   m_Input;
    Output                  m_Output;
};
#endif // SrvTasks_JobCreateIndex_HPP
