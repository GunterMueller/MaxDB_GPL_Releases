/*
  @file         gbd490.h
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

#ifndef GBD490_H
#define GBD490_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

// Content of include files

#include "gsp00.h"   // PASCAL: SP_basic_constants_and_types
#include "ggg00.h"   // PASCAL: GG_kernel_constants_and_types
#include "gbd00.h"   // PASCAL: BD_kernel_constants_and_types
#include "gbd494.h"  // CPP   : FunnelHandling
#include "gbd496.h"  // CPP   : FileHandling
#include "gbd497.h"  // CPP   : StackDescInfo

#include "KernelCommon/Kernel_Common.hpp"
#include "RunTime/RTE_Types.hpp"
#include "ServerTasks/SrvTasks_JobList.hpp"
#include "ServerTasks/SrvTasks_JobCreateIndex.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  FORWARD DECLARATION                                                      *
 *===========================================================================*/


class Msg_List;
class Trans_Context;
class SrvTasks_WaitingTask;

/*===========================================================================*
 *  GLOBAL FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

externCpp void
bd490InitIndexCounter( tgg00_IndexCounter  &IndexCounter );

/*---------------------------------------------------------------------------*/

inline void
bd490Inc (
    SAPDB_Int4&         baseCounter,
    const SAPDB_UInt    addCounter)
{
    if (MAX_INT4_SP00 > (baseCounter + addCounter)){
        baseCounter += addCounter;
    }
}

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

class cbd490_CreateIndexParallel
{
public:

    cbd490_CreateIndexParallel(
        Trans_Context&          trans,
        cbd494_SortableInvTree& invTree,
        tgg00_FileId&           primFileId,
        tgg00_StackDesc&        stackDesc,
        tgg00_IndexCounter&     indexCounter,
        tgg00_IndexMode			indexMode );

    ~cbd490_CreateIndexParallel();

    void Initialize( Trans_Context&    trans );

    void BuildIndex( Trans_Context& trans );

    static bool IsActive(){
        return( RTE_UnknownTaskId != m_LockedBy );
    }

    static SAPDB_Int4 GetIndexFileListSize( const bool bReadFromConfigFile = false );

    static void SetIndexFileListSize( const SAPDB_Int4 value );

private:

    void ProcessKeyRangeByCoordinator(
        Trans_Context&      trans,
        const SAPDB_UInt    CurrFileIndex,
        const SAPDB_UInt    NumLeavesToScan,
        const SAPDB_UInt    AvgRecordsPerLeaf,
        SAPDB_Byte* const   pStartPrimKey,
        const SAPDB_UInt    StartPrimKeyLen,
        SAPDB_Byte* const   pStopPrimKey,
        const SAPDB_UInt    StopPrimKeyLen,
        SAPDB_UInt&         RetryCount,
        tgg00_FileId&       QueueId,
        const Data_PageNo   PrimQueuePno,
        const Data_PageNo   SecQueuePno );

    void SendKeyRangeToServer(
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
        Msg_List&               msgList );

    void WaitForReply(
        Trans_Context&          trans,
        const pasbool           bNoMorePrimData,
        SAPDB_UInt&             NumSendOrders,
        SrvTasks_WaitingTask&   waitingTask,
        Msg_List&               msgList );

    void DetermineKeyRangeForServer(
        Trans_Context&      trans,
        SAPDB_Byte* const   pStartPrimKey,
        SAPDB_Int&          startPrimKeyLen,
        SAPDB_Byte* const   pStopPrimKey,
        SAPDB_Int&          stopPrimKeyLen,
        SAPDB_Int&          numLeavesToScan,
        SAPDB_Int&          avgRecordsPerLeaf,
        pasbool&            bNoMorePrimData );

private:

    // Assignment operator is not supported
    cbd490_CreateIndexParallel& operator=( const cbd490_CreateIndexParallel &cip );

    // Copy constructor is not supported
    cbd490_CreateIndexParallel( const cbd490_CreateIndexParallel &cip );

private:

    // global member data to ensure that only one parallel index creation is executed
    static  RTE_TaskId  m_LockedBy;

    // global member data which gives the value of the configuration
    // paramter IDXFILE_LIST_SIZE
    static  SAPDB_Int4  m_IndexFileListSize;

    tgg00_IndexCounter      &m_IndexCounter;

    tgg00_FileId                                m_PrimFileId;
    tgg00_IndexMode                             m_IndexMode;
    SAPDB_UInt                                  m_NumUsableServerTasks;
    cbd497_StackDescInfo	                    m_Stack;
    cbd496_FileHandling                         m_FileList;
    SrvTasks_JobList<SrvTasks_JobCreateIndex>   m_ActiveJobs;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class cbd490_CreateIndexSequential
{
public:

    cbd490_CreateIndexSequential(
        Trans_Context&          trans,
        cbd300_InvCurrent&      invCurrent,
        tgg00_FileId&           primFileId,
        tgg00_StackDesc&        stackDesc,
        tgg00_IndexCounter&     indexCounter,
        tgg00_IndexMode			indexMode )
    :
    m_InvCurrent( invCurrent ),
    m_PrimFileId( primFileId ),
    m_IndexCounter( indexCounter ),
    m_IndexMode( indexMode ),
    m_Stack( trans.OldTrans().trError_gg00, stackDesc )
    {
        bd490InitIndexCounter( m_IndexCounter );
    }

    ~cbd490_CreateIndexSequential()
    {
        if( e_ok == m_InvCurrent.curr_trans->trError_gg00 ){
            m_IndexCounter.idc_sec_leafnodes = m_InvCurrent.bd300GetRootLeafCount();
        }
    }

    void BuildIndex( Trans_Context&  trans );

private:

    // Assignment operator is not supported
    cbd490_CreateIndexSequential& operator=( const cbd490_CreateIndexSequential &cis );

    // Copy constructor is not supported
    cbd490_CreateIndexSequential( const cbd490_CreateIndexSequential &cis );

private:

    cbd300_InvCurrent       &m_InvCurrent;
    tgg00_IndexCounter      &m_IndexCounter;

    tgg00_FileId            m_PrimFileId;
    tgg00_IndexMode         m_IndexMode;
    cbd497_StackDescInfo	m_Stack;
};

/*===========================================================================*
 *  DEFINITION OF METHODS                                                    *
 *===========================================================================*/

#endif  /* GBD490_H */





