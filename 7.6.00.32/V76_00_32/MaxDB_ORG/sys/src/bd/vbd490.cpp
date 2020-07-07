/*
  @file         vbd490.cpp
  @author       TorstenS
  @ingroup      create index
  @brief        This modul is used to coordinate the parallel 
                index creation. i.e. the table to be indexed
                is portioned into several parts which will be
                processed by server tasks.

\if EMIT_LICENCE


    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

// Content of include files
#include "gbd490.h"
/* */
#include "gbd491.h"   // CPP   : CreateIndexParticipant
#include "gbd493.h"   // CPP   : cbd493_CreateIndexBlockAllocator
#include "ggg11.h"    // PASCAL: GG_vtrace_types
#include "hbd01.h"    // PASCAL: filesysteminterface_1
#include "hbd03.h"    // CPP   : filesysteminterface_3
#include "hbd20_1.h"  // PASCAL: bd20GetCacheSize
#include "hbd30.h"    // PASCAL: file handling
#include "hbd50.h"    // PASCAL: Indexhandling
#include "hgg01_3.h"  // PASCAL: Configuration_Parameter

#include "DataAccess/Data_Exceptions.hpp"
#include "DataAccess/Data_Messages.hpp"
#include "IOManager/IOMan_IDataInfo.hpp"
#include "KernelCommon/ParameterNames/KernelParam_DataAccess.hpp"
#include "RunTime/RTE_Message.hpp"
#include "RunTime/Configuration/RTEConf_ParameterAccess.hpp"
#include "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp"
#include "Trace/Trace_Entry.hpp"
#include "SAPDBCommon/SAPDB_RangeCode.hpp" // Kernel_move_and_fill
#include "ServerTasks/SrvTasks_JobCreateIndex.hpp"
#include "ServerTasks/SrvTasks_WaitingTask.hpp"
#include "ServerTasks/SrvTasks_JobList.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define MAX_RETRIES_BD490       10


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

class cbd490_Lock
{
public:

    /*!
       @brief   The constructor is used to get the lock.
       @param taskId [in] own task
       @return  none
     */
    cbd490_Lock( const RTE_TaskId taskId ): m_Owner( taskId ){
        vbegexcl( m_Owner, g08config );
    };

    /*!
       @brief   The destructor will release the lock.
       @return  none
     */
    ~cbd490_Lock(){
        vendexcl( m_Owner, g08config );
    };

private:

    // Assignment operator is not supported
    cbd490_Lock& operator=( const cbd490_Lock &myLock );

    // Copy constructor is not supported
    cbd490_Lock( const cbd490_Lock &myLock );

private:
    // Contains the taskId of the lock holder.
    RTE_TaskId    m_Owner;
};

/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/



/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/

RTE_TaskId  cbd490_CreateIndexParallel::m_LockedBy = RTE_UnknownTaskId;
SAPDB_Int4  cbd490_CreateIndexParallel::m_IndexFileListSize = 0;

/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL FUNCTIONS (PROTOTYPES)                                             *
 *===========================================================================*/

inline void
bd490_AddIndexCounter(
    tgg00_IndexCounter       &SourceCounter,
    const tgg00_IndexCounter &AddCounter);

/*---------------------------------------------------------------------------*/

inline SAPDB_UInt
bd490_GetEstimatedRecordsPerLeaf (
    const SAPDB_UInt    NumPrimLeafs,
    const SAPDB_UInt    NumPrimRecords,
    SAPDB_UInt          AvgRecordsPerLeaf);

/*===========================================================================*
 *  GLOBAL FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

void
bd490InitIndexCounter (tgg00_IndexCounter  &IndexCounter)
{
    IndexCounter.idc_prim_leafnodes  = 0;
    IndexCounter.idc_prim_indexnodes = 0;
    IndexCounter.idc_sec_leafnodes   = 0;
    IndexCounter.idc_prim_keycount   = 0;
    IndexCounter.idc_sec_keycount    = 0;
    IndexCounter.idc_nullcount       = 0;
    IndexCounter.idc_prim_mismatch   = 0;
}

/*===========================================================================*
 *  LOCAL FUNCTIONS (CODE)                                                   *
 *===========================================================================*/

inline void
bd490_AddIndexCounter (
    tgg00_IndexCounter       &SourceCounter,
    const tgg00_IndexCounter &AddCounter)
{
    bd490Inc (SourceCounter.idc_prim_leafnodes,  AddCounter.idc_prim_leafnodes);
    bd490Inc (SourceCounter.idc_prim_indexnodes, AddCounter.idc_prim_indexnodes);
    bd490Inc (SourceCounter.idc_sec_leafnodes,   AddCounter.idc_sec_leafnodes);
    bd490Inc (SourceCounter.idc_prim_keycount,   AddCounter.idc_prim_keycount);
    bd490Inc (SourceCounter.idc_sec_keycount,    AddCounter.idc_sec_keycount);
    bd490Inc (SourceCounter.idc_nullcount,       AddCounter.idc_nullcount);
    bd490Inc (SourceCounter.idc_prim_mismatch,   AddCounter.idc_prim_mismatch);
}

/*---------------------------------------------------------------------------*/

inline SAPDB_UInt
bd490_GetEstimatedRecordsPerLeaf (
    const SAPDB_UInt    numPrimLeafs,
    const SAPDB_UInt    numPrimRecords,
    SAPDB_UInt          avgRecordsPerLeaf)
{
    if(( 0 != numPrimLeafs ) && ( 0 != numPrimRecords )){
        avgRecordsPerLeaf = ( numPrimRecords / numPrimLeafs );
    }
    return( avgRecordsPerLeaf + ( avgRecordsPerLeaf / 10) + 5 ); // +10 % security
}

/*===========================================================================*
 *  DEFINITION OF METHODS DECLARED IN gbd490.h (CODE)                        * 
 *===========================================================================*/


cbd490_CreateIndexParallel::cbd490_CreateIndexParallel(
    Trans_Context&          trans,
    cbd494_SortableInvTree& invTree,
    tgg00_FileId&           primFileId,
    tgg00_StackDesc&        stackDesc,
    tgg00_IndexCounter&     indexCounter,
    tgg00_IndexMode			indexMode )
        :
        m_PrimFileId( primFileId ),
        m_IndexCounter( indexCounter ),
        m_IndexMode( indexMode ),
        m_NumUsableServerTasks( 0 ), // PTS 1104880 TS 1999-12-06
        m_Stack( trans.OldTrans().trError_gg00, stackDesc ),
        m_FileList( trans.Allocator(), invTree ),
        m_ActiveJobs( trans.Allocator() )
{
}

/*---------------------------------------------------------------------------*/

cbd490_CreateIndexParallel::~cbd490_CreateIndexParallel()
{
    const RTETask_ITask  &task  = *RTETask_ITask::Self();

    if( task.ID() == m_LockedBy ){
        m_LockedBy = RTE_UnknownTaskId; // remove lock
    }
}

/*---------------------------------------------------------------------------*/

void
cbd490_CreateIndexParallel::Initialize( Trans_Context&    trans )
{
    SAPDBTRACE_ROUTINE_DEBUG( "cbd490_CreateIndexParallel::Init", DataIndex_Trace, 5 );

    const SAPDB_Int4 cNumIndexResultFiles = cbd490_CreateIndexParallel::GetIndexFileListSize();

    SAPDB_Int4 totalPages  = 0;
    bd20GetCacheSize( totalPages );
    {
        cbd490_Lock lock( trans.Task().ID()); // begin critical section

        if( cbd490_CreateIndexParallel::IsActive())
        {
            trans.OldTrans().trError_gg00 = e_no_more_memory;
            RTE_Message( Data_Exception ( __CONTEXT__, DATA_WRN_CREATE_INDEX_IN_USE_1 ));
            return;  // => use sequential, even now one index is build in parallel
        }
        if(( 1 > cNumIndexResultFiles ) || ( cNumIndexResultFiles * 3 ) > totalPages )
        {
            trans.OldTrans().trError_gg00 = e_no_more_memory;
            RTE_Message( Data_Exception( __CONTEXT__, DATA_WRN_CREATE_INDEX_FILELISTSIZE_1,
                                         "_IDXFILE_LIST_SIZE",
                                         SAPDB_ToString( m_IndexFileListSize, _T_d )));
            return; // => use sequential, idxFileListSize is to small
        }
        m_LockedBy = trans.Task().ID();
    }
    if( ! m_FileList.Initialize( cNumIndexResultFiles )){
        trans.OldTrans().trError_gg00 = e_no_more_memory;
        RTE_Message( Data_Exception( __CONTEXT__, DATA_WRN_CREATE_INDEX_OUT_OF_MEMORY_1 ));
        return;
    }
    bd490InitIndexCounter( m_IndexCounter );
    m_NumUsableServerTasks = IOMan_IDataInfo::GetInstance().UsedDataVolumes();

    SAPDBERR_ASSERT_STATE( m_NumUsableServerTasks > 0 );

    if( 1 >= m_NumUsableServerTasks ){
        m_NumUsableServerTasks = 2;
    }
}
/*---------------------------------------------------------------------------*/

SAPDB_Int4
cbd490_CreateIndexParallel::GetIndexFileListSize( const bool bReadFromConfigFile )
{
    RTEConf_Parameter::Integer  paramValue;
    Msg_List                    msgList;

    if( bReadFromConfigFile )
    {
        if( ! RTEConf_ParameterAccess::Instance()->GetInteger(
                    UTF8( KERNELPARAM_IDXFILE_LIST_SIZE ), paramValue, msgList ))
        {
            RTE_Crash( msgList );
        }
        m_IndexFileListSize = ( SAPDB_UInt) paramValue;
    }
    return  m_IndexFileListSize;
}

/*---------------------------------------------------------------------------*/

void
cbd490_CreateIndexParallel::SetIndexFileListSize( const SAPDB_Int4 value )
{
    RTE_Message( Data_Exception( __CONTEXT__, DATA_CREATE_INDEX_FILELISTSIZE_UPD_1,
                                 "_IDXFILE_LIST_SIZE",
                                 SAPDB_ToString( m_IndexFileListSize, _T_d ),
                                 SAPDB_ToString( value, _T_d )));
    m_IndexFileListSize = value;
}

/*---------------------------------------------------------------------------*/

void
cbd490_CreateIndexParallel::BuildIndex( Trans_Context& trans )
{
    SAPDBTRACE_ROUTINE_DEBUG( "cbd490_CreateIndexParallel::Build", DataIndex_Trace, 5 );

    if(  trans.Task().ID() != m_LockedBy ){
        trans.OldTrans().trError_gg00 = e_no_more_memory; // is not locked by this task use sequential index creation
        return;
    }

    pasbool       bNoMorePrimData   = false;
    SAPDB_UInt    CurrFileIndex     = 0;
    SAPDB_Int     AvgRecordsPerLeaf = 0;
    SAPDB_Int     NumLeavesToScan   = 0;
    SAPDB_UInt    NumSendOrders     = 0;
    SAPDB_UInt    RetryCount        = 0;
    SAPDB_Int     StartPrimKeyLen   = 0;
    SAPDB_Int     StopPrimKeyLen    = 0;

    Data_PageNo  PrimQueuePno;
    Data_PageNo  SecQueuePno;

    tgg00_FileId  QueueId;

    tsp00_Key     StartPrimKey;
    tsp00_Key	  StopPrimKey;

    SAPDB_Byte* const  pStartPrimKey = reinterpret_cast< SAPDB_Byte* >( &StartPrimKey );
    SAPDB_Byte* const  pStopPrimKey  = reinterpret_cast< SAPDB_Byte* >( &StopPrimKey );

    SrvTasks_WaitingTask                waitingTask( trans.Task());
    cbd493_CreateIndexBlockAllocator&   blockAllocator = cbd493_CreateIndexBlockAllocator::GetInstance();

    while
    (
        ((e_ok == trans.OldTrans().trError_gg00) &&
         (! bNoMorePrimData)) || ((e_no_server_task_available == trans.OldTrans().trError_gg00))
    )
    {
        if(( e_ok == trans.OldTrans().trError_gg00 ) && ( create_index == m_IndexMode )){
            m_FileList.CreateFile( trans, CurrFileIndex, QueueId,	PrimQueuePno, SecQueuePno);
        }

        if( e_ok == trans.OldTrans().trError_gg00 )
        {
            this->DetermineKeyRangeForServer( trans, pStartPrimKey, StartPrimKeyLen,
                                              pStopPrimKey, StopPrimKeyLen, NumLeavesToScan,
                                              AvgRecordsPerLeaf, bNoMorePrimData );
        }

        if(( e_ok == trans.OldTrans().trError_gg00 ) || ( e_no_server_task_available == trans.OldTrans().trError_gg00 ))
        {
            this->SendKeyRangeToServer( trans, CurrFileIndex, NumLeavesToScan,
                                        AvgRecordsPerLeaf, pStartPrimKey, StartPrimKeyLen,
                                        pStopPrimKey, StopPrimKeyLen, NumSendOrders, RetryCount,
                                        QueueId, PrimQueuePno, SecQueuePno, waitingTask, trans.ErrorList() );
        }
        if  (
            (
                (e_ok == trans.OldTrans().trError_gg00) &&
                ((NumSendOrders == m_NumUsableServerTasks) || (bNoMorePrimData))
            )
            ||
            ((e_file_limit == trans.OldTrans().trError_gg00) && (NumSendOrders > 0))
            ||
            ((e_no_server_task_available == trans.OldTrans().trError_gg00) && (NumSendOrders > 0))
        )
        {
            this->WaitForReply( trans, bNoMorePrimData, NumSendOrders, waitingTask, trans.ErrorList());
        }

        if ((e_no_server_task_available == trans.OldTrans().trError_gg00 ) &&
                (0 == NumSendOrders                      ) &&
                (MAX_RETRIES_BD490 <= RetryCount        ))
        {
            this->ProcessKeyRangeByCoordinator( trans, CurrFileIndex, NumLeavesToScan, AvgRecordsPerLeaf,
                                                pStartPrimKey, StartPrimKeyLen, pStopPrimKey, StopPrimKeyLen,
                                                RetryCount, QueueId, PrimQueuePno, SecQueuePno);
        }
        if (
            (create_index == m_IndexMode) &&
            (
                ((e_ok == trans.OldTrans().trError_gg00) && (bNoMorePrimData)) ||
                (e_file_limit == trans.OldTrans().trError_gg00               )
            )
        )
        {
            blockAllocator.EmptyAll();
            m_FileList.MergeFiles( trans, bNoMorePrimData, m_Stack, m_IndexCounter );
        }
        if(
            ( e_ok == trans.OldTrans().trError_gg00 ) &&
            ( ! bNoMorePrimData                     ) &&
            ( trans.Task().CancelPending()         ))
        {
            trans.OldTrans().trError_gg00 = e_cancelled;
            RTE_Message( Data_Exception( __CONTEXT__, DATA_CREATE_INDEX_CANCELED_2 ));
        }
    }
    if( e_ok != trans.OldTrans().trError_gg00 )
    {
        m_ActiveJobs.AbortAll( waitingTask );
        blockAllocator.EmptyAll();
        if( create_index == m_IndexMode ){
            m_FileList.DeleteAllFiles(  trans );
        }
    }
   
}

/*---------------------------------------------------------------------------*/

void
cbd490_CreateIndexParallel::ProcessKeyRangeByCoordinator(
    Trans_Context&         trans,
    const SAPDB_UInt       CurrFileIndex,
    const SAPDB_UInt       NumLeavesToScan,
    const SAPDB_UInt       AvgRecordsPerLeaf,
    SAPDB_Byte* const      pStartPrimKey,
    const SAPDB_UInt       StartPrimKeyLen,
    SAPDB_Byte* const      pStopPrimKey,
    const SAPDB_UInt       StopPrimKeyLen,
    SAPDB_UInt&            RetryCount,
    tgg00_FileId&          QueueId,
    const Data_PageNo      PrimQueuePno,
    const Data_PageNo      SecQueuePno )
{
    SAPDBTRACE_ROUTINE_DEBUG( "cbd490_CreateIndexParallel::ProcessKeyRange", DataIndex_Trace, 5 );

    if( e_no_server_task_available == trans.OldTrans().trError_gg00 ){
        trans.OldTrans().trError_gg00 = e_ok;
    }
    if( e_ok != trans.OldTrans().trError_gg00 ){
        return;
    }

    pasbool             bEntriesExist;
    tsp00_PageNo        FirstLeafPno = NIL_PAGE_NO_GG00; // no Data_PageNo, because of C++/Pascal mix
    tgg00_IndexCounter  AuxCounter;
    tgg00_StackDesc     AuxStackDesc =  m_Stack.bd497GetStackDesc(); // reduce code check errors
    SAPDB_Int4          tempLeafPageCount = 0;

    RetryCount = 0;

    RTE_Message( Data_Exception( __CONTEXT__, DATA_CREATE_INDEX_SELF_PROC_1 ));

    bd03ParticipantCreateIndex( trans.OldTrans(), m_PrimFileId, QueueId,
                                 AuxStackDesc, pStartPrimKey, pStopPrimKey, StartPrimKeyLen, StopPrimKeyLen,
                                 NumLeavesToScan, AvgRecordsPerLeaf, PrimQueuePno, SecQueuePno,
                                 (create_index == m_IndexMode), m_IndexMode, AuxCounter, 
                                 tempLeafPageCount, FirstLeafPno, bEntriesExist );

    if( create_index == m_IndexMode )
    {
        m_FileList.SetFilePageCount( CurrFileIndex, tempLeafPageCount );
        
        if( NIL_PAGE_NO_GG00 != FirstLeafPno ){
            m_FileList.ConvertFile( trans, CurrFileIndex, FirstLeafPno );
        }
        if( ! bEntriesExist ){
            m_FileList.DeleteFile( trans, CurrFileIndex );
        }
        if( e_ok == trans.OldTrans().trError_gg00 ){
            bd490_AddIndexCounter( m_IndexCounter, AuxCounter );
        }

        if( g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdIndex_gg00 || DataIndex_Trace.TracesLevel( 5 ))
        {
            tgg11_BdRootTrace RootTrace;

            RootTrace.bdrTrType_gg11.becomes( bdtrRoot_egg11 );
            RootTrace.bdrError_gg11  = trans.OldTrans().trError_gg00;
            RootTrace.bdrRoot_gg11   = m_IndexCounter.idc_prim_keycount;

            Trace_CommonEntry entry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(b03create),
                                     reinterpret_cast< SAPDB_Byte* >( &RootTrace ), sizeof (RootTrace) );
        }
    }
    RTE_Message( Data_Exception( __CONTEXT__, DATA_CREATE_INDEX_SELF_PROC_2 ));
}

/*---------------------------------------------------------------------------*/

void
cbd490_CreateIndexParallel::SendKeyRangeToServer(
    Trans_Context&          trans,
    const SAPDB_UInt        CurrFileIndex,
    const SAPDB_UInt        NumLeavesToScan,
    const SAPDB_UInt        AvgRecordsPerLeaf,
    SAPDB_Byte* const       pStartPrimKey,
    const SAPDB_UInt        StartPrimKeyLen,
    SAPDB_Byte* const       pStopPrimKey,
    const SAPDB_UInt        StopPrimKeyLen,
    SAPDB_UInt&             NumSendOrders,
    SAPDB_UInt&             RetryCount,
    tgg00_FileId&           TempFileId,
    const Data_PageNo       PrimQueuePno,
    const Data_PageNo       SecQueuePno,
    SrvTasks_WaitingTask&   waitingTask,
    Msg_List&               msgList )
{
    SAPDBTRACE_ROUTINE_DEBUG( "cbd490_CreateIndexParallel::Send", DataIndex_Trace, 5 );

    const tgg00_BasisError auxError = (e_ok == trans.OldTrans().trError_gg00) ? e_ok : trans.OldTrans().trError_gg00;

    if( e_ok != trans.OldTrans().trError_gg00 ){
        trans.OldTrans().trError_gg00 = e_ok; // m_TrError could be only e_no_server_task_available
    }

    SrvTasks_JobCreateIndex *pNewJob = new( m_ActiveJobs.GetAllocator()) SrvTasks_JobCreateIndex( CurrFileIndex );

    if ( 0 == pNewJob )
    {
        trans.OldTrans().trError_gg00 = e_no_more_memory;
        return;
    }

    SrvTasks_JobCreateIndex::Input  jobInputData;

    jobInputData.PrimQueuePno      = PrimQueuePno;
    jobInputData.SecQueuePno       = SecQueuePno;
    jobInputData.NumLeavesToScan   = NumLeavesToScan;
    jobInputData.AvgRecordsPerLeaf = AvgRecordsPerLeaf;
    jobInputData.StartPrimKeyLen   = StartPrimKeyLen;
    jobInputData.StopPrimKeyLen    = StopPrimKeyLen;
    jobInputData.IndexMode         = m_IndexMode;
    jobInputData.UseMemorySort     = (create_index == m_IndexMode);
    jobInputData.PrimFileId        = m_PrimFileId;
    jobInputData.InvFileId         = TempFileId;
    jobInputData.StackDescLen      = m_Stack.bd497GetStackDescSize();
    jobInputData.pStartPrimKey     = pStartPrimKey;
    jobInputData.pStopPrimKey      = pStopPrimKey;
    jobInputData.pStackDesc        = &m_Stack.bd497GetStackDesc();

    if( ! pNewJob->Initialize( m_ActiveJobs.GetAllocator(), jobInputData, msgList )){
        trans.OldTrans().trError_gg00 = e_no_more_memory; // job memory is released below
    }

    if( e_ok == trans.OldTrans().trError_gg00 )
    {
        switch ( pNewJob->ExecuteByServer( waitingTask.Task(), msgList, &waitingTask ))
        {
        case SrvTasks_Job::executeOK:
            if ( ! m_ActiveJobs.InsertEnd( pNewJob ))
                trans.OldTrans().trError_gg00 = e_no_more_memory;
            break;
        case SrvTasks_Job::executeNoServerAvailable:
            trans.OldTrans().trError_gg00 = e_no_server_task_available;
            break;
        case SrvTasks_Job::executeError:
            trans.OldTrans().trError_gg00 = e_start_server_task_error;
            break;
        default:
            trans.OldTrans().trError_gg00 = e_system_error;
            break;
        }
        if( e_ok == trans.OldTrans().trError_gg00 ){
            ++NumSendOrders;
        }
        else if( e_no_server_task_available == trans.OldTrans().trError_gg00 )
        {
            RTE_Message( Data_Exception( __CONTEXT__, DATA_WRN_CREATE_INDEX_SEND_1 ));
            ++RetryCount;
            waitingTask.Task().Sleep( 1 );
        }
    }
    if( e_ok != trans.OldTrans().trError_gg00 ){
        destroy( pNewJob, m_ActiveJobs.GetAllocator());
    }

    if(( e_ok == trans.OldTrans().trError_gg00 ) && ( e_no_server_task_available == auxError ))
    {
        RetryCount = 0;
        trans.OldTrans().trError_gg00  = e_ok;
    }
}

/*---------------------------------------------------------------------------*/

void
cbd490_CreateIndexParallel::WaitForReply(
    Trans_Context&          trans,
    const pasbool           bNoMorePrimData,
    SAPDB_UInt&             NumSendOrders,
    SrvTasks_WaitingTask&   waitingTask,
    Msg_List&               msgList )
{
    SAPDBTRACE_ROUTINE_DEBUG( "cbd490_CreateIndexParallel::Wait", DataIndex_Trace, 5 );

    tgg00_BasisError    auxError = (e_ok == trans.OldTrans().trError_gg00) ? e_ok : trans.OldTrans().trError_gg00;
    SAPDB_UInt          OldNumSendOrders = NumSendOrders;

    SrvTasks_JobCreateIndex::Output jobOutputData;
    tgg00_BasisError                jobError;
    SAPDB_UInt                      currFileNo;

    if( e_ok != trans.OldTrans().trError_gg00 ){
        trans.OldTrans().trError_gg00 = e_ok; // Real error is stored within auxError
    }

#ifdef SAPDB_SLOW 
    Kernel_VTrace vTrace;
    if( DataIndex_Trace.TracesLevel(6))
    {
        vTrace << "> cbd490_CreateIndexParallel:Wait()"
        << " Finish:  "      <<  ( bNoMorePrimData ? "yes" : "no" )
        << " NumSendOrder: " << NumSendOrders;
    }
    SAPDBERR_ASSERT_STATE( NumSendOrders == m_ActiveJobs.GetSize());
#endif

    while
    (
        (( e_ok == trans.OldTrans().trError_gg00) &&
         (
             ((0 < NumSendOrders) && (bNoMorePrimData)                        ) ||
             ((NumSendOrders == m_NumUsableServerTasks) && (! bNoMorePrimData))
         ))
        ||
        (( e_file_limit == auxError ) && ( 0 < NumSendOrders ))
        ||
        (( e_no_server_task_available == auxError ) && ( NumSendOrders == OldNumSendOrders ))
    )
    {
        SrvTasks_JobList<SrvTasks_JobCreateIndex>::Iterator finishedJobIter;

        if ( m_ActiveJobs.WaitForAnyJobFinished( waitingTask, finishedJobIter ) )
        {
            jobOutputData = (*finishedJobIter)->GetOutput();

            Msg_List jobMsgList;
            jobError   = (*finishedJobIter)->GetResult( jobMsgList );
            currFileNo = (*finishedJobIter)->GetId();

            if( jobError != e_ok )
            {
                msgList.AppendNewMessage( jobMsgList );
                RTE_Message( msgList );
                msgList.ClearMessageList();
            }
            m_ActiveJobs.RemoveJob( finishedJobIter );
        }
        else{ // because the other counters should ensure not to wait on empty list
            trans.OldTrans().trError_gg00 = e_no_more_jobs_to_wait_for;
        }

        if( e_ok != trans.OldTrans().trError_gg00 ){
            return;
        }
        --NumSendOrders;

        if( e_ok != jobError )
        {
            trans.OldTrans().trError_gg00 = jobError;
            return;
        }
        bd490_AddIndexCounter (m_IndexCounter, jobOutputData.IndexCounter);

        if( create_index == m_IndexMode )
        {
            m_FileList.SetFilePageCount( currFileNo, jobOutputData.TempLeafPageCount );
            
            if ( NIL_PAGE_NO_GG00 != jobOutputData.FirstLeafPno ){
                m_FileList.ConvertFile( trans, currFileNo, jobOutputData.FirstLeafPno );
            }

            if ( jobOutputData.EntriesExist == 0 )
            {
                // Result file from servertask is empty
                m_FileList.DeleteFile( trans, currFileNo );

                if( e_file_limit == auxError )
                {
                    // Don't merge if a ServerTask returns an empty file and
                    // the original error says that no empty file is available
                    auxError = e_ok;
                }
            }
            if( g01vtrace.vtrAll_gg00 || g01vtrace.vtrBdIndex_gg00 || DataIndex_Trace.TracesLevel( 5 ))
            {
                tgg11_BdRootTrace RootTrace;

                RootTrace.bdrTrType_gg11.becomes(bdtrRoot_egg11);
                RootTrace.bdrError_gg11  = trans.OldTrans().trError_gg00;
                RootTrace.bdrRoot_gg11   = m_IndexCounter.idc_prim_keycount;

                Trace_CommonEntry entry( tgg00_Debug::fromConst(bd), tgg00_VtraceType::fromConst(b03create),
                                         reinterpret_cast< SAPDB_Byte* >( &RootTrace ), sizeof (RootTrace) );
            }
        }
    }
    if(( e_ok == trans.OldTrans().trError_gg00 ) && ( e_ok != auxError )){
        trans.OldTrans().trError_gg00 = auxError;
    }
#ifdef SAPDB_SLOW 
    if( DataIndex_Trace.TracesLevel(6))
    {
        vTrace << "> cbd490_CreateIndexParallel:Wait()"
        << " NumPrimLeafNodes: " << m_IndexCounter.idc_prim_leafnodes;
    }
#endif
}

/*---------------------------------------------------------------------------*/

void
cbd490_CreateIndexParallel::DetermineKeyRangeForServer(
    Trans_Context&      trans,
    SAPDB_Byte* const   pStartPrimKey,
    SAPDB_Int&          startPrimKeyLen,
    SAPDB_Byte* const   pStopPrimKey,
    SAPDB_Int&          stopPrimKeyLen,
    SAPDB_Int&          numLeavesToScan,
    SAPDB_Int&          avgRecordsPerLeaf,
    pasbool&            bNoMorePrimData )
{
    tbd_current_tree primCurrent;

    bd30GetTree( trans.OldTrans(), m_PrimFileId, primCurrent, m_create_index_parallel,
                 ! LOCK_TREE_EXCL_BD00, ! SYNC_TREE_LOCK_BD00 );

    if ( e_ok == trans.OldTrans().trError_gg00 )
    {
        bd50DetermineKeyRangeForServer(
            primCurrent, pStartPrimKey, startPrimKeyLen, pStopPrimKey, stopPrimKeyLen,
            numLeavesToScan, avgRecordsPerLeaf, bNoMorePrimData );
    }
    bd30ReleaseTree( primCurrent );

    if( e_ok != trans.OldTrans().trError_gg00 ){
        return;
    }
    // file directory statistic count migration
    bd490Inc( m_IndexCounter.idc_prim_indexnodes, 1);

    avgRecordsPerLeaf = bd490_GetEstimatedRecordsPerLeaf(
                            m_IndexCounter.idc_prim_leafnodes,
                            m_IndexCounter.idc_prim_keycount, avgRecordsPerLeaf );
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void
cbd490_CreateIndexSequential::BuildIndex( Trans_Context&  trans )
{
    SAPDBTRACE_ROUTINE_DEBUG( "cbd490_CreateIndexSequential::Build", DataIndex_Trace, 5 );

    pasbool            bEntriesExist;

    SAPDB_UInt         startPrimKeyLen = 0;;
    SAPDB_UInt         stopPrimKeyLen  = PERM_KEY_MXSP00;

    SAPDB_Byte* const        pStartPrimKey = NULL;
    const SAPDB_Byte* const  pStopPrimKey  = reinterpret_cast< SAPDB_Byte* >( &b01fullkey );

    tgg00_StackDesc    stackDesc =  m_Stack.bd497GetStackDesc(); // reduce code check errors
    tbd_current_tree   primCurrent;

    bd30GetTree (trans.OldTrans(), m_PrimFileId, primCurrent, m_create_index_parallel,
                 ! LOCK_TREE_EXCL_BD00, ! SYNC_TREE_LOCK_BD00);

    if ( e_ok == trans.OldTrans().trError_gg00 )
    {
        cbd491_CreateIndexParticipant participant( trans, primCurrent, stackDesc, m_IndexCounter, false );

        participant.InsertionSort( trans, m_InvCurrent, m_IndexMode, pStartPrimKey, pStopPrimKey,
                                   startPrimKeyLen, stopPrimKeyLen, bEntriesExist );
    }
    bd30ReleaseTree( primCurrent );
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/

