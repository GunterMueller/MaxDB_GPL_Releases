/*!
  @file           Trans_Context.hpp
  @author         UweH
  @ingroup        Transaction
  @since          2004-10-26

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
#ifndef Trans_Context_HPP
#define Trans_Context_HPP
//--------------------------------------------------------------------------------------
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "Messages/Msg_List.hpp"
#include "ggg90.h"          // CPP   : GG_kernel_templates
#include "ggg00.h"
//--------------------------------------------------------------------------------------
class SAPDBMem_IRawAllocator;
class FileDir_IDirectory;
//--------------------------------------------------------------------------------------
/// This context defines transaction related data
class Trans_Context : private tgg00_TransContext
{
public:
    /*!
    @brief default ctor initializes internal structure
    @param trans [in] old trans context
    */
    Trans_Context()
    {}
    /// casts tgg00_TransContext reference to Trans_Context reference 
    static Trans_Context& GetContext(tgg00_TransContext &trans)
    {
        return static_cast<Trans_Context&>(trans);
    }
    /// returns the trans/session allocator
    SAPDBMem_IRawAllocator& Allocator()
    {
        return *reinterpret_cast <SAPDBMem_IRawAllocator*> (this->trAllocator_gg00);
    }
    /// returns the trans/session error list
    Msg_List& ErrorList()
    {
        return *reinterpret_cast <Msg_List*> (this->trErrorList_gg00);
    }
    /// returns the transaction id
    const tgg91_TransNo& TransId() const
    {
        return this->trWriteTransId_gg00;
    }
    /// returns the session id
    const tgg91_SessionNo& SessionId() const{
        return this->trSessionId_gg00;
    }
    // is this a common session
    const bool IsNormalSession() const{
        return NIL_BYTE_GG90 == this->trSessionId_gg00[0];
    }
    /// is this a nil session
    const bool IsNilSession() const{
        return this->trSessionId_gg00.gg90IsNil();
    }
    /// returns the subtrans id
    const tgg00_SubtransNo& SubtransId() const{
        return this->trSubtransId_gg00;
    }
    /// returns task context
    RTETask_ITask& Task()
    {
        return *RTETask_ITask::ByTaskID(this->trTaskId_gg00);
    }
    /// returns the old context pointer
    tgg00_TransContext& OldTrans()
    {
        return *static_cast<tgg00_TransContext*>(this);
    }
    /// initializes the context with some default values for servertasks
    void ServerTaskInit (SAPDB_UInt jobtype,
                         SAPDB_Int  jobno,
                         bool       bIgnoreDBFullSecureLimit );
    /// This initializes the trans context for the first time in Kernel_Main
    void Initialize (RTETask_ITask          &task,
                     Msg_List               &errlist,
                     SAPDBMem_IRawAllocator &allocator,
                     tsp00_RteCommAddr       rteCommPtr );
	/// initializes the context with some default values for servertasks
    void TimeoutTaskInit();
    ///is transaction in state rollback
    bool IsRollback() const
    {
        return this->trState_gg00.includes( tsRollback_egg00 );
    }
    /// stop command execution, if no more temporary pageNo's are available
    bool IsDbFullNoWait() const
	{
        return this->trState_gg00.includes( tsDbFullNowait_egg00 );
    }
    /// use reserved pageNo pool, if data area is nearly full
    bool IsIgnoreDBFullSecureLimit() const
	{
        return this->trState_gg00.includes( tsIgnoreDBSecureLimit_egg00 );
    }
    /// return private temp page count
    SAPDB_Int4 TempPageCount() const
	{
        return this->trTempCount_gg00;
    }
    /// return private temp page count
    SAPDB_Int4& TempPageCount()
	{
        return this->trTempCount_gg00;
    }
    /// Do at least one data page lock exist, which is synchronized with the savepoint
    bool HasSavepointSynchronizationLocks() const
	{
        return 0 != this->trBdExclFileLocks_gg00;
    }
    /// Returns a pointer to the transaction's file directory (might be 0).
    FileDir_IDirectory* FileDirectory() const
    {
        return reinterpret_cast<FileDir_IDirectory*>(this->trLocalFileDirPtr_gg00);
    }
    /// Returns the transaction's task id.
    RTE_TaskId TaskId() const
    {
        return this->trTaskId_gg00;
    }
    /// return true if error code is okay
    bool IsOk() const{
        return e_ok == this->trError_gg00;
    }
    /// set error code to okay and clear message list
    void ResetErrorCode()
    {
        this->trError_gg00 = 0;
        this->ErrorList().Clear();
    }
    /// set error code to the given value
    void SetErrorCode( const tgg00_BasisError error ){
        this->trError_gg00 = error;
    }
};

inline void Trans_Context::TimeoutTaskInit()
{
    this->trIndex_gg00     = cgg_nil_transindex;
    this->trTempCount_gg00 = 0;
    this->trError_gg00     = e_ok;
    this->trState_gg00.clear();
    this->trWarning_gg00.clear();
    ErrorList().ClearMessageList();
}

inline void Trans_Context::ServerTaskInit (SAPDB_UInt jobtype,
                                           SAPDB_Int  jobno,
                                           bool       bIgnoreDBFullSecureLimit )
{
    this->trError_gg00     = e_ok;
    this->trIndex_gg00     = cgg_nil_transindex;
    this->trSessionId_gg00.gg91BuildServerTaskTraceSession (jobtype,jobno);
    this->trWriteTransId_gg00.gg90SetNil();
    this->trTransId_gg00.gg90SetNil();
    this->trConsistView_gg00.gg90SetNil();
    this->trSubtransId_gg00        = cgg_zero_subtrans;
    this->trState_gg00.clear();
    this->trWarning_gg00.clear();
    this->trChild_gg00             = jobno;
    this->trObjReqTimeout_gg00     = 0;
    this->trSessionAuxCounter_gg00 = 0;
    this->trTempCount_gg00         = 0;
    this->trIoCount_gg00           = 0;

    if ( bIgnoreDBFullSecureLimit )
        this->trState_gg00.addElement( tsIgnoreDBSecureLimit_egg00 );
}

inline void Trans_Context::Initialize (RTETask_ITask          &task,
                                       Msg_List               &errlist,
                                       SAPDBMem_IRawAllocator &allocator,
                                       tsp00_RteCommAddr       rteCommPtr )
{
    this->trSessionId_gg00.gg90SetNil();
    this->trTransId_gg00.gg90SetZero();
    this->trSubtransId_gg00            = cgg_zero_subtrans;
    this->trChild_gg00                 = cgg_nil_child;

    this->trWriteTransId_gg00.gg90SetNil();
    this->trConsistView_gg00.gg90SetNil();
    this->trConsistReadInfo_gg00.becomes(CnsReadNone);
    this->trFiller1_gg00               = false;

    this->trState_gg00.clear();
    this->trError_gg00                 = e_ok;
    this->trWarning_gg00.clear();
    this->trObjReqTimeout_gg00         = 100; // msec

    this->trSessionAuxCounter_gg00     = 0;
    this->trTempCount_gg00             = 0;

    this->trIoCount_gg00               = 0;

    this->trTaskId_gg00                = task.ID();
    this->trIndex_gg00                 = cgg_nil_transindex;
    this->trBdExclFileLocks_gg00       = 0;

    this->trRowLocksPerSqlCmd_gg00     = 0;

    this->trRteCommPtr_gg00            = rteCommPtr;
    
    this->trBdTcachePtr_gg00           = 0;
    this->trAllocator_gg00             = &allocator;
    this->trLogContext_gg00            = 0;

    this->trSeqInfoPtr_gg00            = 0;
    this->trAcvPtr_gg00                = 0;
    this->trLocalFileDirPtr_gg00       = 0;
	this->trFreePagesForIns_gg00       = 0;
    this->trSchemaShrLockHash_gg00     = 0;

    this->trClusterIOPtr_gg00          = 0;
    this->trClusterIOSize_gg00         = 0;

    this->trErrorList_gg00             = &errlist;
    this->trSyncParallelServers_gg00   = false;
}
#endif // Trans_Context_HPP
