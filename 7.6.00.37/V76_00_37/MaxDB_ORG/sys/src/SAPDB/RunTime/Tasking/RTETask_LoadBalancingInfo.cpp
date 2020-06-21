/*!========================================================================

  @file         RTETask_LoadBalancingInfo.cpp
  @ingroup      RTETask_LoadBalancingInfo.cpp
  @author       RaymondR

  @brief        This paragraph appears in overviews.

                The rest of the description appears as detailed description

  @since        2004-06-11  19:43
  @sa           

  ==========================================================================

  \if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2004-2005 SAP AG

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
 ============================================================================*/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include    "RunTime/MemoryManagement/RTEMem_RteAllocator.hpp"
#include    "RunTime/RTE_KGS.hpp"
#include    "geo007_1.h"
#include    "geo002.h"
#include    "RunTime/RTE_MessageList.hpp"
#include    "RunTime/RTE_Message.hpp"
#include    "RunTime/Tasking/RTETask_LoadBalancingInfo.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

RTETask_LoadBalancingInfo::RTETask_MoveInfo::RTETask_MoveInfo(  SAPDB_UInt8                 moveTime,
                                                                SAPDB_UInt4                 taskIndex,
                                                                SAPDB_UInt8                 taskMaxRunnableMicroSeconds,
                                                                SAPDB_UInt4                 srcUKTIndex,
                                                                SAPDB_UInt8                 srcMaxRunnableMicroSeconds,
                                                                SAPDB_UInt4                 dstUKTIndex,
                                                                SAPDB_UInt8                 dstMaxRunnableMicroSeconds )
                                            : m_MoveTime(moveTime),
                                              m_TaskIndex(taskIndex),
                                              m_TaskMaxRunnableMicroSeconds(taskMaxRunnableMicroSeconds),
                                              m_SrcUKTIndex(srcUKTIndex),
                                              m_SrcMaxRunnableMicroSeconds(srcMaxRunnableMicroSeconds),
                                              m_DstUKTIndex(dstUKTIndex),
                                              m_DstMaxRunnableMicroSeconds(dstMaxRunnableMicroSeconds)
{

}

/*---------------------------------------------------------------------------*/

RTETask_LoadBalancingInfo::RTETask_MoveInfo::RTETask_MoveInfo()
                                            : m_MoveTime(0),
                                              m_TaskIndex(0),
                                              m_TaskMaxRunnableMicroSeconds(0),
                                              m_SrcUKTIndex(0),
                                              m_SrcMaxRunnableMicroSeconds(0),
                                              m_DstUKTIndex(0),
                                              m_DstMaxRunnableMicroSeconds(0)
{
}

/*---------------------------------------------------------------------------*/

RTETask_LoadBalancingInfo::RTETask_LoadBalancingInfo() : m_pUKTMoveInfo (0),
                                                         m_CheckIntervalTime(0),
                                                         m_ValueEquality(0),
                                                         m_MinimumDiff(0),
                                                         m_TaskMoveIntervalStartTime(0),
                                                         m_TaskMoveMeasIntervals(0),
                                                         m_CurrTaskMoveInfo(0),
                                                         m_NumOfTaskMoveInfos(0),
                                                         m_CurrUKTMoveInfo(0),
                                                         m_NumOfUKTMoveInfos(0)
{
}

/*---------------------------------------------------------------------------*/

RTETask_LoadBalancingInfo::~RTETask_LoadBalancingInfo()
{
    if ( 0 != m_pUKTMoveInfo )
      RTEMem_RteAllocator::Instance().Deallocate(m_pUKTMoveInfo);

}

/*---------------------------------------------------------------------------*/

void RTETask_LoadBalancingInfo::SnapShot()
{
    SAPDB_UInt2                     CurrEntry;
    RTE_KGS::KernelGlobalSection*   pKGS = RTE_KGS::Instance().GetKGS();

#if defined (_WIN32)
    WAIT_UNTIL_ALONE(&kgs.LastTaskMoves.lExclusive);

    for ( CurrEntry = NEXT_ENTRY( kgs.LastTaskMoves.CurrEntry, NUM_OF_TASK_MOVE_STAT_RECS), m_NumOfTaskMoveInfos = 0; 
          CurrEntry != kgs.LastTaskMoves.CurrEntry;
          CurrEntry = NEXT_ENTRY( CurrEntry, NUM_OF_TASK_MOVE_STAT_RECS) )
    {
        if (( kgs.LastTaskMoves.Entries[CurrEntry].MoveTime  != 0 )    &&
            ( kgs.LastTaskMoves.Entries[CurrEntry].pTaskCtrl != NULL ) &&
            ( kgs.LastTaskMoves.Entries[CurrEntry].pSrcUKT   != NULL ) &&
            ( kgs.LastTaskMoves.Entries[CurrEntry].pDstUKT   != NULL ))
        {
            m_TaskMoveInfos[m_NumOfTaskMoveInfos] = RTETask_MoveInfo (
                (SAPDB_UInt8)kgs.LastTaskMoves.Entries[CurrEntry].MoveTime * 1000000,
                kgs.LastTaskMoves.Entries[CurrEntry].pTaskCtrl->ulTaskIndex,
                kgs.LastTaskMoves.Entries[CurrEntry].TaskMaxRunnableMicroSeconds,
                kgs.LastTaskMoves.Entries[CurrEntry].pSrcUKT->ulUKTIndex,
                kgs.LastTaskMoves.Entries[CurrEntry].SrcMaxRunnableMicroSeconds,
                kgs.LastTaskMoves.Entries[CurrEntry].pDstUKT->ulUKTIndex,
                kgs.LastTaskMoves.Entries[CurrEntry].DstMaxRunnableMicroSeconds );

            m_NumOfTaskMoveInfos++;
        }
    }
    CLEARLOCK(&kgs.LastTaskMoves.lExclusive);
#else
    WAIT_UNTIL_ALONE(pKGS->lastTaskMoves.exclusive);

    for ( CurrEntry = NEXT_ENTRY( pKGS->lastTaskMoves.currEntry, NUM_OF_TASK_MOVE_STAT_RECS), m_NumOfTaskMoveInfos = 0; 
          CurrEntry != pKGS->lastTaskMoves.currEntry;
          CurrEntry = NEXT_ENTRY( CurrEntry, NUM_OF_TASK_MOVE_STAT_RECS) )
    {
        if (( pKGS->lastTaskMoves.entries[CurrEntry].moveTime  != 0 )    &&
            ( pKGS->lastTaskMoves.entries[CurrEntry].task      != NULL ) &&
            ( pKGS->lastTaskMoves.entries[CurrEntry].srcUKT    != NULL ) &&
            ( pKGS->lastTaskMoves.entries[CurrEntry].dstUKT    != NULL ))
        {
            m_TaskMoveInfos[m_NumOfTaskMoveInfos] = RTETask_MoveInfo (
                pKGS->lastTaskMoves.entries[CurrEntry].moveTime * 1000000,
                pKGS->lastTaskMoves.entries[CurrEntry].task->index,
                pKGS->lastTaskMoves.entries[CurrEntry].taskMaxRunnableMicroSeconds,
                pKGS->lastTaskMoves.entries[CurrEntry].srcUKT->index,
                pKGS->lastTaskMoves.entries[CurrEntry].srcMaxRunnableMicroSeconds,
                pKGS->lastTaskMoves.entries[CurrEntry].dstUKT->index,
                pKGS->lastTaskMoves.entries[CurrEntry].dstMaxRunnableMicroSeconds );
            m_NumOfTaskMoveInfos++;
        }
    }
    CLEARLOCK(pKGS->lastTaskMoves.exclusive);
#endif

    m_CheckIntervalTime         = XPARAM(uLoadBalancingCheck);
    m_ValueEquality             = XPARAM(uLoadBalancingEquality);
    m_MinimumDiff               = XPARAM(uLoadBalancingDiff);
#if defined (_WIN32)
    m_TaskMoveIntervalStartTime = (SAPDB_UInt8)pKGS->TaskMoveIntervalStartTimes[pKGS->CurrTimeVar] * 1000000;
    m_TaskMoveMeasIntervals     = pKGS->TaskMoveMeasIntervals;
#else
    m_TaskMoveIntervalStartTime = (SAPDB_UInt8)pKGS->taskMoveIntervalStartTimes[pKGS->currTimeVar] * 1000000;
    m_TaskMoveMeasIntervals     = pKGS->taskMoveMeasIntervals;
#endif

    SAPDB_UInt4 numOfUKTs = pKGS->ulNumOfUKTs;

    if ( 0 != m_pUKTMoveInfo )
      RTEMem_RteAllocator::Instance().Deallocate(m_pUKTMoveInfo);

    m_pUKTMoveInfo = (RTETask_UKTMoveInfo*) RTEMem_RteAllocator::Instance().Allocate (sizeof(RTETask_UKTMoveInfo) * numOfUKTs);

    // m_pUKTMoveInfo == 0, none fatal error

    if ( 0 != m_pUKTMoveInfo )
    {
#     if defined (_WIN32)
        PUKT_CTRL_REC                       pMoveTaskToUKT;
        PUKT_CTRL_REC                       pUKTCtrl;
        SAPDB_UInt4                         ulUKT;
        SAPDB_UInt2                         PrevTimeVar = PREV_TIME_VAR(kgs.CurrTimeVar);

        for ( pUKTCtrl =  kgs.pFirstUKTCtrl, ulUKT = 0;
              pUKTCtrl <= kgs.pLastUKTCtrl && ulUKT < numOfUKTs;
              pUKTCtrl++ )
        {
            if ( pUKTCtrl->TasksAllowedToMoveTo )
            {
                pMoveTaskToUKT = pUKTCtrl->pMoveTaskToUKT;

                m_pUKTMoveInfo[ulUKT] = RTETask_UKTMoveInfo ( 
                    pUKTCtrl->ulUKTIndex,
                    pMoveTaskToUKT ? pMoveTaskToUKT->ulUKTIndex : 0,
                    pUKTCtrl->NumOfMovableTasks,
                    pUKTCtrl->TaskMoveIntervalStartTime * 1000000,
                    pUKTCtrl->UKTSleepTimeMicroSeconds[PrevTimeVar],
                    pUKTCtrl->RunnableMicroSeconds[PrevTimeVar],
                    pUKTCtrl->MaxRunnableMicroSeconds[PrevTimeVar] );
                ulUKT++;
                m_NumOfUKTMoveInfos++;
            }
        }
#     else
        struct ten50_UKT_Control*           pMoveTaskToUKT;
        struct ten50_UKT_Control*           pUKTCtrl;
        SAPDB_UInt4                         ulUKT;
        SAPDB_UInt2                         PrevTimeVar = PREV_TIME_VAR(pKGS->currTimeVar);

        for ( pUKTCtrl =  pKGS->first_kp, ulUKT = 0;
              pUKTCtrl <= pKGS->last_kp && ulUKT < numOfUKTs;
              pUKTCtrl++ )
        {
            if ( pUKTCtrl->tasksAllowedToMoveTo )
            {
                pMoveTaskToUKT = pUKTCtrl->moveTaskToUKT;

                m_pUKTMoveInfo[ulUKT] = RTETask_UKTMoveInfo ( 
                    pUKTCtrl->index,
                    pMoveTaskToUKT ? pMoveTaskToUKT->index : 0,
                    pUKTCtrl->numOfMovableTasks,
                    pUKTCtrl->taskMoveIntervalStartTime * 1000000,
                    pUKTCtrl->UKTSleepTimeMicroSeconds[PrevTimeVar],
                    pUKTCtrl->runnableMicroSeconds[PrevTimeVar],
                    pUKTCtrl->maxRunnableMicroSeconds[PrevTimeVar] );
                ulUKT++;
                m_NumOfUKTMoveInfos++;
            }
        }
#     endif
    }
}

/*---------------------------------------------------------------------------*/

RTETask_LoadBalancingInfo::RTETask_MoveInfo const * const  
                    RTETask_LoadBalancingInfo::GetFirstTaskMoveInfo()
{
    m_CurrTaskMoveInfo = 0;

    if (( m_CurrTaskMoveInfo + 1) >= m_NumOfTaskMoveInfos )
        return 0;

    return &m_TaskMoveInfos[m_CurrTaskMoveInfo];
}

/*---------------------------------------------------------------------------*/


RTETask_LoadBalancingInfo::RTETask_MoveInfo const * const 
                    RTETask_LoadBalancingInfo::GetNextTaskMoveInfo()
{
    if (( m_CurrTaskMoveInfo + 1) >= m_NumOfTaskMoveInfos )
        return 0;

    m_CurrTaskMoveInfo++;

    return &m_TaskMoveInfos[m_CurrTaskMoveInfo];
}

/*---------------------------------------------------------------------------*/

RTETask_LoadBalancingInfo::RTETask_UKTMoveInfo const * const  
                    RTETask_LoadBalancingInfo::GetFirstUKTMoveInfo()
{
    m_CurrUKTMoveInfo = 0;

    if (( m_CurrUKTMoveInfo + 1) >= m_NumOfUKTMoveInfos )
        return 0;

    return &m_pUKTMoveInfo[m_CurrUKTMoveInfo];
}

/*---------------------------------------------------------------------------*/

RTETask_LoadBalancingInfo::RTETask_UKTMoveInfo const * const  
                    RTETask_LoadBalancingInfo::GetNextUKTMoveInfo ()
{
    if (( m_CurrUKTMoveInfo + 1) >= m_NumOfUKTMoveInfos )
        return 0;

    m_CurrUKTMoveInfo++;

    return &m_pUKTMoveInfo[m_CurrUKTMoveInfo];
}

/*---------------------------------------------------------------------------*/

RTETask_LoadBalancingInfo::RTETask_UKTMoveInfo::RTETask_UKTMoveInfo(  SAPDB_UInt4                 UKTIndex,
                                                                      SAPDB_UInt4                 moveTaskToUKTIndex,
                                                                      SAPDB_UInt4                 numOfMovableTasks,
                                                                      SAPDB_UInt8                 m_TaskMoveIntervalStartTime,
                                                                      SAPDB_UInt8                 UKTSleepTimeMicroSeconds,
                                                                      SAPDB_UInt8                 runnableMicroSeconds,
                                                                      SAPDB_UInt8                 maxRunnableMicroSeconds )
                                            : m_UKTIndex(UKTIndex),
                                              m_MoveTaskToUKTIndex(moveTaskToUKTIndex),
                                              m_NumOfMovableTasks(numOfMovableTasks),
                                              m_TaskMoveIntervalStartTime(m_TaskMoveIntervalStartTime),
                                              m_UKTSleepTimeMicroSeconds(UKTSleepTimeMicroSeconds),
                                              m_RunnableMicroSeconds(runnableMicroSeconds),
                                              m_MaxRunnableMicroSeconds(maxRunnableMicroSeconds)
{
}

/*---------------------------------------------------------------------------*/

RTETask_LoadBalancingInfo::RTETask_UKTMoveInfo::RTETask_UKTMoveInfo()
                                            : m_UKTIndex(0),
                                              m_MoveTaskToUKTIndex(0),
                                              m_NumOfMovableTasks(0),
                                              m_TaskMoveIntervalStartTime(0),
                                              m_UKTSleepTimeMicroSeconds(0),
                                              m_RunnableMicroSeconds(0),
                                              m_MaxRunnableMicroSeconds(0)
{
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/