/****************************************************************************

  module      : Pager_Controller.cpp

  -------------------------------------------------------------------------

  responsible : Henrik

  special area:
  description : This description is currently not used, as
    documentation is only generated from header files

  last changed: 2002-02-26
  see also    :

  -------------------------------------------------------------------------

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




*****************************************************************************/

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "Pager/Pager_Controller.hpp"
#include "Pager/Pager_Exception.hpp"
#include "Pager/Pager_Messages.hpp"
#include "Pager/Pager_Lock.hpp"

#include "DataAccess/Data_BasePage.hpp"
#include "IOManager/IOMan_IDataIO.hpp"
#include "Converter/Converter_ISavePoint.hpp"

#include "KernelCommon/ParameterNames/KernelParam_Pager.hpp"
#include "KernelCommon/Kernel_Dump.hpp"
#include "KernelCommon/Kernel_Trace.hpp"

#include "RunTime/Configuration/RTEConf_ParameterAccess.hpp"
#include "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp"
#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include "RunTime/RTE_Message.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "RunTime/Tasking/RTETask_SuspendReason.h"

#include "ggg00.h"
#include "gbd00.h"          // tbd00_NodePtrList
#include "hgg01.h"          // g01maxdatawriter
#include "heo56.h"          // vsuspend/vsleep
#include "hgg04.h"
#include "hbd20_5.h"
#include "gbd201.h"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define PAGER_WAIT_FOR_FLUSHCACHE 253

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

// singleton
Pager_Controller* Pager_Controller::m_Instance = NULL;

/*---------------------------------------------------------------------------*/

Pager_Controller& Pager_Controller::GetInstance(){

    SAPDBTRACE_ROUTINE_DEBUG("Pager_Controller::GetInstance", Pager_Trace, 5 );

    if ( NULL == m_Instance ) {
        Pager_Controller::CreateInstance();
    }

    return *m_Instance;
}

/*---------------------------------------------------------------------------*/

Pager_IRestart& Pager_IRestart::GetInstance(){

    SAPDBTRACE_ROUTINE_DEBUG("Pager_IRestart::GetInstance", Pager_Trace, 5 );

    return Pager_Controller::GetInstance();
}

Pager_ISavepoint& Pager_ISavepoint::GetInstance(){

    SAPDBTRACE_ROUTINE_DEBUG("Pager_ISavepoint::GetInstance", Pager_Trace, 5 );

    return Pager_Controller::GetInstance();
}

Pager_IDump& Pager_IDump::GetInstance(){

    SAPDBTRACE_ROUTINE_DEBUG("Pager_IDump::GetInstance", Pager_Trace, 5 );

    return Pager_Controller::GetInstance();
}


/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/


SAPDB_Int Pager_Controller::RegisterNextPagerWriter(const tsp00_TaskId taskId,
        Pager_Writer *writer){

    SAPDBTRACE_ROUTINE_DEBUG("Pager_Controller::RegisterNextPagerWriter", Pager_Trace, 5 );

    Pager_Lock lock(taskId);

    SAPDBERR_ASSERT_ARGUMENT(m_registeredPagerWriter < m_numPagerWriter);

    m_pagerWriter[m_registeredPagerWriter] = writer;
    m_activePagerWriter[m_registeredPagerWriter] = true;

    return m_registeredPagerWriter++;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
updateClusterWriteParameter (
	RTEConf_Parameter::Name parameterName,
	RTEConf_Parameter::Type parameterKind,
	void                  * newValue,
	void                  * context,
	SAPDBErr_MessageList  & msgList)
{
	if (parameterKind != RTEConf_Parameter::t_Integer) {
		return false;
	}
	*((SAPDB_UInt *)context) = (SAPDB_UInt)(*(RTEConf_Parameter::Integer *)newValue);
	return true;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool Pager_Controller::RegisterParameters (
	SAPDBErr_MessageList  & msgList)
{
	return RTEConf_ParameterAccess::Instance ()->RegisterUpdateFunction (
	    KERNELPARAM_CLUSTER_WRITE_THRESHOLD, updateClusterWriteParameter,
	    &m_clusterWriteThreshold, msgList);
}

/*---------------------------------------------------------------------------*/

void Pager_Controller::EndSavepointUnSynch ( const tsp00_TaskId    taskId){

	SAPDBERR_ASSERT_STATE(m_svpActive);

	m_preparePagerWriter  = false;
	m_svpActive          = false;
	m_pagerOrder         = poNil;
}

/*---------------------------------------------------------------------------*/

void Pager_Controller::EndSavepoint ( const tsp00_TaskId    taskId){

    SAPDBTRACE_ROUTINE_DEBUG("Pager_Controller::EndSavepoint", Pager_Trace, 5 );

    Pager_Lock lock(taskId);

	EndSavepointUnSynch(taskId);
}

/*---------------------------------------------------------------------------*/

void Pager_Controller::EndSavepointForShutdown ( const tsp00_TaskId    taskId){

	SAPDBTRACE_ROUTINE_DEBUG("Pager_Controller::EndSavepointForShutdown", Pager_Trace, 5 );

	Pager_Lock lock(taskId);

	EndSavepointUnSynch(taskId);
	m_pagerWriterDisabled = true;
}

/*---------------------------------------------------------------------------*/

void Pager_Controller::FlushDataCacheBeforePrepare (const tsp00_TaskId taskId,
        SAPDB_UInt &numPagesWritten,
        SAPDB_UInt &numIOActions){

    SAPDBTRACE_ROUTINE_DEBUG("Pager_Controller::FlushDataCacheBeforePrepare", Pager_Trace, 5 );

    SAPDBERR_ASSERT_STATE(m_svpActive);
    SAPDBERR_ASSERT_STATE(poNil == m_pagerOrder);

    m_pagerOrder   = poFlushDataCacheBeforePrepare;

    m_dataIOTotal.SetValue(0);
    m_dataPagesTotal.SetValue(0);

    Pager_Exception StartWriteData(__CONTEXT__, PAGER_SVP_START_WRITE);
    RTE_Message (StartWriteData);

    m_clusterer.PrepareFlush(taskId);
    
    ResumeAllPagerWriter (taskId, !SET_CHECK_FBM, false);
    WaitForPagerWriterReply (taskId);

    Pager_Exception StopDataIO(__CONTEXT__, PAGER_SVP_STOP_DATA_IO,
                               SAPDB_ToString(m_dataPagesTotal.GetValue(), _T_d),
                               SAPDB_ToString(m_dataIOTotal.GetValue(), _T_d));
    RTE_Message (StopDataIO);

    numPagesWritten = m_dataPagesTotal.GetValue();
    numIOActions =    m_dataIOTotal.GetValue();
}

/*---------------------------------------------------------------------------*/

void Pager_Controller::WaitForLastSvpSyncOperation (
    const tsp00_TaskId taskId,
    SAPDB_UInt         &numPagesWritten,
    SAPDB_UInt         &numIOActions){

    SAPDBTRACE_ROUTINE_DEBUG("Pager_Controller::WaitForLastSvpSyncOperation", Pager_Trace, 5 );

    SAPDB_Bool1     dummy;
    SAPDB_Bool      bSplitLocksExist;
    SAPDB_Int2      pageCnt;
    SAPDB_Int       currCache;
    SAPDB_Int       loopCnt;
    SAPDB_UInt      totalPages;
    SAPDB_UInt      totalIO;

    tbd00_NodePtrList       NodePtrList;
    tbd2_data_cb_flush_list cbptrList;
    tgg00_BasisError        trError = e_ok;

    bSplitLocksExist = true;

    loopCnt          = 0;
    totalIO          = 0;
    totalPages       = 0;

    Pager_Exception WaitForLastSvpSyncOperation(__CONTEXT__, PAGER_SVP_WAIT_LAST_SPLIT,
            SAPDB_ToString(taskId, _T_d));

    RTE_Message (WaitForLastSvpSyncOperation);

    while ((e_ok == trError) && bSplitLocksExist){

        if  (bd20ExclusiveLocks(taskId)) {
            bd20FullestDataCacheSegment(taskId, currCache);

            pageCnt = 1;

            bd20GetPageForIO (taskId, currCache, trError, pageCnt,
                              NodePtrList, cbptrList, dummy);

            if (e_ok == trError) {
                if (pageCnt > 0) {
                    totalIO++;
                    totalPages += pageCnt;
                    WriteDataPage (taskId, NodePtrList[0], cbptrList[0]);
                    if (e_ok == trError) {
                        bd20ResetIOState (taskId, currCache, pageCnt, cbptrList);
                    }
                }
                else {
                    if (loopCnt >= 5) {
                        vsleep (taskId, 1);
                        loopCnt = 0;
                    }
                    else {
                        loopCnt++;
                        vsleep (taskId, 0);
                    }
                }
            }
        }
        else {
            bSplitLocksExist = false;
        }
    }   // END while
    Pager_Exception StopWaitLastSplit(__CONTEXT__, PAGER_SVP_STOP_WAIT_LS,
                                      SAPDB_ToString(totalPages, _T_d),
                                      SAPDB_ToString(totalIO, _T_d));
    RTE_Message (StopWaitLastSplit);

    numPagesWritten = totalPages;
    numIOActions =    totalIO;

    if (e_ok != trError){
        Pager_Exception Split_Error(__CONTEXT__, PAGER_ERROR_IN_SPLIT,
                                    SAPDB_ToString(trError, _T_d));
        RTE_Crash (Split_Error);
    }
}

/*---------------------------------------------------------------------------*/

void Pager_Controller::FlushDataCacheAfterPrepare (const tsp00_TaskId taskId,
        SAPDB_UInt &numPagesWritten,
        SAPDB_UInt &numIOActions){


    SAPDBTRACE_ROUTINE_DEBUG("Pager_Controller::FlushDataCacheAfterPrepare", Pager_Trace, 5 );

    SAPDBERR_ASSERT_STATE(m_svpActive);

    m_pagerOrder   = poFlushDataCacheAfterPrepare;

    m_dataIOTotal.SetValue(0);
    m_dataPagesTotal.SetValue(0);

    Pager_Exception StartWriteData(__CONTEXT__, PAGER_SVP_START_AFTER_PREPARE);
    RTE_Message (StartWriteData);

	m_clusterer.PrepareFlush(taskId);
    
    ResumeAllPagerWriter (taskId, !SET_CHECK_FBM, false);
    WaitForPagerWriterReply (taskId);

    Pager_Exception StopAfterPrepare(__CONTEXT__, PAGER_SVP_STOP_AFTER_PREPARE,
                                     SAPDB_ToString(m_dataPagesTotal.GetValue(), _T_d),
                                     SAPDB_ToString(m_dataIOTotal.GetValue(), _T_d));
    RTE_Message (StopAfterPrepare);

    numPagesWritten = m_dataPagesTotal.GetValue();
    numIOActions =    m_dataIOTotal.GetValue();
}

/*---------------------------------------------------------------------------*/

void Pager_Controller::FlushConverterParallel ( const tsp00_TaskId TaskId,
        SAPDB_UInt &numPagesWritten,
        SAPDB_UInt &numIOActions)
{
    SAPDBTRACE_ROUTINE_DEBUG("Pager_Controller::FlushConverterParallel", Pager_Trace, 5 );

    SAPDBERR_ASSERT_STATE(m_svpActive);
    
    m_pagerOrder = poFlushConverterCache;
    m_convIOTotal.SetValue(0);
    m_convPagesTotal.SetValue(0);

    Pager_Exception StartFlushConverter(__CONTEXT__, PAGER_SVP_START_CONV_AFTER_PREPARE);
    RTE_Message (StartFlushConverter);

    ResumeAllPagerWriter(TaskId, !SET_CHECK_FBM, true);
    WaitForPagerWriterReply (TaskId);

    Pager_Exception StopAfterFlushConverter(__CONTEXT__, PAGER_SVP_STOP_CONV_AFTER_PREPARE,
                                            SAPDB_ToString(m_convPagesTotal.GetValue(), _T_d),
                                            SAPDB_ToString(m_convIOTotal.GetValue(), _T_d));

    RTE_Message (StopAfterFlushConverter);

    numPagesWritten = m_convPagesTotal.GetValue();
    numIOActions =    m_convIOTotal.GetValue();

    // done
    m_pagerOrder = poNil;
}

/*---------------------------------------------------------------------------*/

void Pager_Controller::BeginSavepoint (const tsp00_TaskId taskId) {

    SAPDBTRACE_ROUTINE_DEBUG("Pager_Controller::BeginSavepoint", Pager_Trace, 5 );

    SAPDB_Bool doSuspend = false;
    { // locked scope
        Pager_Lock lock(taskId);

        if (m_preparePagerWriter) { // don't call me twice!
            Pager_Exception SVP_Error(__CONTEXT__, PAGER_ERROR_SVP_ACTIVE,
                                      SAPDB_ToString(taskId, _T_d));
            RTE_Crash (SVP_Error);
        }
        m_preparePagerWriter = true;

        // wait until flushDataCache is finished
        if (m_flushActive) {
            m_waitingForFlush = taskId;
            doSuspend = true;
        }
    }

    if(doSuspend) {
        vsuspend (taskId, PAGER_WAIT_FOR_FLUSHCACHE);
    }

    m_svpActive = true;
}

/*---------------------------------------------------------------------------*/

void Pager_Controller::Dump( const tsp00_TaskId taskId,
                             Kernel_Dump        &dump )
{
    struct PagerDump pcd;

    pcd.dmpNumPagerWriter        = m_numPagerWriter;
    pcd.dmpRegisteredPagerWriter = m_registeredPagerWriter;
    pcd.dmpSuspendPagerWriter    = m_suspendCnt;
    pcd.dmpPagerOrder            = m_pagerOrder;
    pcd.dmpSVPIsActive           = m_svpActive;
    pcd.dmpFlushIsActive         = m_flushActive;
    pcd.dmpPreparePagerWriter    = m_preparePagerWriter;
    pcd.dmpPagerWriterDisabled   = m_pagerWriterDisabled;

    dump.InsertEntry( Kernel_Dump::DmpPagerController,
                      Kernel_DumpPage::Entry( &pcd, sizeof( pcd )));
};

/*---------------------------------------------------------------------------*/

void Pager_Controller::ReadConverter(
    const tsp00_TaskId    taskId,
    const SAPDB_Bool      bReadSnapShotConverter )
{

    SAPDBTRACE_ROUTINE_DEBUG("Pager_Controller::ReadConverter", Pager_Trace, 5 );

    if (m_pagerOrder != poNil) { // idle ?
        Pager_Exception CVT_Error(__CONTEXT__, PAGER_ERROR_CVT_ACTIVE,
                                  SAPDB_ToString(taskId, _T_d));
        RTE_Crash (CVT_Error);
    }

    m_convIOTotal.SetValue(0);
    m_convPagesTotal.SetValue(0);

    if( bReadSnapShotConverter ){
        m_pagerOrder = poReadSnapShotConverter;
        Pager_Exception StartReadConverter(__CONTEXT__, PAGER_START_READ_SNAPSHOT_CONVERTER);
        RTE_Message (StartReadConverter);
    }
    else{
        m_pagerOrder = poReadConverterCache;
        Pager_Exception StartReadConverter(__CONTEXT__, PAGER_START_READ_CONVERTER);
        RTE_Message (StartReadConverter);
    }

    ResumeAllPagerWriter(taskId, !SET_CHECK_FBM, true);
    WaitForPagerWriterReply (taskId);


    Pager_Exception StopAfterReadConverter(__CONTEXT__, PAGER_STOP_READ_CONVERTER,
                                           SAPDB_ToString(m_convPagesTotal.GetValue(), _T_d),
                                           SAPDB_ToString(m_convIOTotal.GetValue(), _T_d));

    RTE_Message (StopAfterReadConverter);

    // done
    m_pagerOrder = poNil;
}

/*---------------------------------------------------------------------------*/

void Pager_Controller::SuspendPagerWriter(SAPDB_UInt    pagerIndex,
        tsp00_TaskId  taskId){

    SAPDBTRACE_ROUTINE_DEBUG("Pager_Controller::SuspendPagerWriter", Pager_Trace, 5 );

    tsp00_TaskId resumeTaskId = cgg_nil_pid;

    { // locked scope
        Pager_Lock lock(taskId);

        SAPDBERR_ASSERT_STATE(m_activePagerWriter[pagerIndex]);
        SAPDBERR_ASSERT_STATE(!visresume(taskId));

        m_activePagerWriter[pagerIndex] = false;
        m_suspendCnt.Increment();

        if(m_suspendCnt.GetValue() == m_numPagerWriter) {
            if (m_svpCoordinator != cgg_nil_pid) {
            // -> sl_prepare_data_writer is true
            resumeTaskId      = m_svpCoordinator;
            m_svpCoordinator = cgg_nil_pid;
            }
            
            if ((poFlushDataCache == m_pagerOrder) ||
			    (poFlushDataCacheWithTempFromLRURechain == m_pagerOrder))
            {

//                Pager_Exception StopDataIO(__CONTEXT__, PAGER_PRED_STOP_DATA_IO,
//                                            SAPDB_ToString(m_dataPagesTotal.GetValue(), _T_d),
//                                            SAPDB_ToString(m_dataIOTotal.GetValue(), _T_d));
//                RTE_Message (StopDataIO);

                m_pagerOrder   = poNil;
                m_flushActive = false;

                // wake up BeginSavepoint
                if (m_waitingForFlush != cgg_nil_pid) {
                    resumeTaskId      = m_waitingForFlush;
                    m_waitingForFlush = cgg_nil_pid;
                }
            }
        }
    }

    if(resumeTaskId != cgg_nil_pid) {
        vresume (resumeTaskId);
    }

    vsuspend (taskId, TSK_VSUSPEND_REASON_NO_WORK);
}

/*---------------------------------------------------------------------------*/

void Pager_Controller::EnablePagerWriter (){

    SAPDBTRACE_ROUTINE_DEBUG("Pager_Controller::EnablePagerWriter", Pager_Trace, 5 );

    RTESync_LockedScope Lock(m_SpinLock);

    m_pagerWriterDisabled = false;
}

/*---------------------------------------------------------------------------*/

void Pager_Controller::FlushDataCacheWithTempFromLRURechain(const tsp00_TaskId taskId){

    SAPDBTRACE_ROUTINE_DEBUG("Pager_Controller::FlushDataCacheWithTempFromLRURechain", Pager_Trace, 5 );

    tsp00_TaskId resumeTaskId = cgg_nil_pid;

    { // locked scope
        Pager_Lock lock(taskId);

        if ((m_pagerOrder != poNil) || m_flushActive || m_svpActive || m_preparePagerWriter) { // idle ?
            return;
        }

        m_flushActive = true;

        m_pagerOrder   = poFlushDataCacheWithTempFromLRURechain;
        m_dataIOTotal.SetValue(0);
        m_dataPagesTotal.SetValue(0);
    }

    ResumeAllPagerWriter (taskId, !SET_CHECK_FBM, false);
}

/*---------------------------------------------------------------------------*/

void Pager_Controller::FlushDataCache( const tsp00_TaskId taskId) {

    SAPDBTRACE_ROUTINE_DEBUG("Pager_Controller::FlushDataCache", Pager_Trace, 5 );

    tsp00_TaskId resumeTaskId = cgg_nil_pid;

    { // locked scope
        Pager_Lock lock(taskId);

        if ((m_pagerOrder != poNil) || m_flushActive || m_svpActive || m_preparePagerWriter) { // idle ?
            return;
        }

        m_flushActive = true;

        m_pagerOrder   = poFlushDataCache;
		m_clusterer.PrepareFlush(taskId);
        
        m_dataIOTotal.SetValue(0);
        m_dataPagesTotal.SetValue(0);
    }

    ResumeAllPagerWriter (taskId, !SET_CHECK_FBM, false);
}

/*---------------------------------------------------------------------------*/

// private:

Pager_Controller::Pager_Controller(SAPDBMem_IRawAllocator &allocator):
        m_SpinLock((SAPDB_UTF8*)"Pager_Controller::m_SpinLock"),
        m_pagerWriter(allocator),
        m_activePagerWriter(allocator),
        m_numPagerWriter(g01maxdatawriter()),
        m_registeredPagerWriter(0),
        m_svpCoordinator(cgg_nil_pid),
        m_waitingForFlush(cgg_nil_pid),
        m_svpActive(false),
        m_preparePagerWriter(false),
        m_flushActive(false),
        m_pagerWriterDisabled(SAPDB_TRUE),
        m_isClusteredIO(cioUndef),
        m_pagerOrder(poNil),
		m_clusterer(bd201_Clusterer(allocator))
{
}

/*---------------------------------------------------------------------------*/

void Pager_Controller::CreateInstance(){

    SAPDBTRACE_ROUTINE_DEBUG("Pager_Controller::CreateInstance", Pager_Trace, 5 );

    if ( NULL == m_Instance ) {
        m_Instance = new( RTEMem_Allocator::Instance() ) Pager_Controller(RTEMem_Allocator::Instance());
        if (NULL == m_Instance ){
            RTE_Crash(SAPDBErr_Exception(__FILE__, __LINE__, SAPDBERR_ASSERT_STATE_FAILED, "Pager_Controller::CreateInstance() alloc failed"));
        }
        m_Instance->Initialize();
    }
}

/*---------------------------------------------------------------------------*/

void Pager_Controller::Initialize() {
	SAPDBErr_MessageList msgList;
	RTEConf_Parameter::Integer  paramValue;
	
    SAPDBTRACE_ROUTINE_DEBUG("Pager_Controller::Initialize", Pager_Trace, 5 );

    m_pagerWriter.Initialize(m_numPagerWriter);
    m_activePagerWriter.Initialize(m_numPagerWriter,false);
	
	m_clusterWriteThreshold = 80;
	if( ! RTEConf_ParameterAccess::Instance()->GetInteger(KERNELPARAM_CLUSTER_WRITE_THRESHOLD, paramValue, msgList))
	{
		RTE_Message(msgList);
	}
	else
	{
		m_clusterWriteThreshold = ( SAPDB_Int )paramValue;
	}
	
	m_clusterer.Initialize();
	
	if (!RegisterParameters(msgList))
	{
		RTE_Message(msgList);
	}
}

/*---------------------------------------------------------------------------*/

void Pager_Controller::ResumeAllPagerWriter (
    const tsp00_TaskId  taskId,
    const SAPDB_Bool1   set_check_fbm,
    const SAPDB_Bool    resumeForConverter){

    SAPDBTRACE_ROUTINE_DEBUG("Pager_Controller::ResumeAllPagerWriter", Pager_Trace, 5 );

    SAPDB_Bool doResume = false;

    for (SAPDB_UInt pagerIndex = 0; pagerIndex < m_numPagerWriter; pagerIndex++) {
        doResume = false;

        { // locked scope
            Pager_Lock lock(taskId);

            if(resumeForConverter || !m_pagerWriterDisabled){
                if (!m_activePagerWriter[pagerIndex]) {
                    m_activePagerWriter[pagerIndex] = true;
                    m_suspendCnt.Decrement();
                    vresume (m_pagerWriter[pagerIndex]->GetTaskId());
                }
            }
        }
    }
}


/*---------------------------------------------------------------------------*/

void Pager_Controller::WaitForPagerWriterReply (const tsp00_TaskId taskId){

    SAPDBTRACE_ROUTINE_DEBUG("Pager_Controller::WaitForPagerWriterReply", Pager_Trace, 5 );

    SAPDB_Bool doSuspend = false;

    { // locked scope
        Pager_Lock lock(taskId);

        if(m_suspendCnt.GetValue() != m_numPagerWriter) {
            SAPDBERR_ASSERT_STATE(m_svpCoordinator == cgg_nil_pid);
            SAPDBERR_ASSERT_STATE(!visresume(taskId));

            m_svpCoordinator = taskId;
            doSuspend    = true;
        }
    }

    if(doSuspend) {
        vsuspend (taskId, 54);
    }
}

/*---------------------------------------------------------------------------*/

void Pager_Controller::WriteDataPage (const tsp00_TaskId    taskId,
                                      tbd_nodeptr           pNode,
                                      tbd02_pDataCBlock     pCBlock) const{

    SAPDBTRACE_ROUTINE_DEBUG("Pager_Controller::WriteDataPage", Pager_Trace, 5 );

    Data_BasePage   Page( pNode,  !gg04IsStaticPage( pNode->nd_pt2() ),
                          pNode->nd_pmode().includes( pmTemp_egg00 ), pCBlock );

    IOMan_IDataIO::GetInstance().WriteDataPage( taskId, Page );
};

/*===========================================================================*
*  END OF CODE                                                              *
*===========================================================================*/

