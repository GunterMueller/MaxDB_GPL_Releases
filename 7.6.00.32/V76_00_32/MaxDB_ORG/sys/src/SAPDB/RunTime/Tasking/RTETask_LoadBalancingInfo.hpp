/*!========================================================================

  @file         RTETask_LoadBalancingInfo.hpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Task load balancing move information and statistics.

                The following class can be used to make snapshots of the
                current task load balancing states and statistic. The 
                snapshot information are  fetched via 'GetFirst' and
                'GetNext' methodes.

  @since        2004-06-01  15:59
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


#ifndef RTETASK_LOADBALANCINGINFO_HPP
#define RTETASK_LOADBALANCINGINFO_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#define     KERNEL_LZU
#include    "RunTime/RTE_Types.h"
#include    "geo50_0.h"       // old task control


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


/*!
  @class    RTETask_LoadBalancingInfo
  @brief    Used to access task load balancing move information and statistics.

            To get load balancing information a snapshot via 'SnapShot()' 
            must be made first.
            To fetch the snapshot information data several methodes like
            'CheckIntervalTime', 'TaskMoveIntervalStartTime', 'GetFirstTaskMoveInfo'
            or 'GetNextTaskMoveInfo' can be used.
 */
class RTETask_LoadBalancingInfo
{
public:
    ///! @name Constructor  
    RTETask_LoadBalancingInfo();
    ///! @name Destructor  
    ~RTETask_LoadBalancingInfo();


    /*!
       @brief    Make a snapshot of the task load balancing states
       @return   none
     */
    void       SnapShot();

    /*!
       @brief    Returns the load balancing measurement interval time (seconds)
     */
    SAPDB_Int4  CheckIntervalTime()         { return m_CheckIntervalTime; }

    /*!
       @brief    Returns the start time of the current interval
                 (microseconds since 1.1.1970).
     */
    SAPDB_UInt8 TaskMoveIntervalStartTime() { return m_TaskMoveIntervalStartTime; }
    /*!
       @brief    Returns the number of measurement intervals since kernel startup
     */
    SAPDB_UInt2 TaskMoveMeasIntervals()     { return m_TaskMoveMeasIntervals; }

    /*!
       @brief    Minimum difference (%) of the task runnable time between 
                 two UKTs needed to initiate a task movement.
     */
    SAPDB_UInt2 MinimumDiff()               { return m_MinimumDiff; }

    /*!
       @brief    Allowed jitter (in %) for equality comparisions.
     */
    SAPDB_UInt2 ValueEquality()             { return m_ValueEquality; }

public:
    /*!
      @brief          Task move information
      
      Each load balancing caused task move operation is represented by the
      following class.
     */
    class RTETask_MoveInfo
    {
    public:
        RTETask_MoveInfo();
        RTETask_MoveInfo( SAPDB_UInt8                 moveTime,
                          SAPDB_UInt4                 taskIndex,
                          SAPDB_UInt8                 taskMaxRunnableMicroSeconds,
                          SAPDB_UInt4                 srcUKTIndex,
                          SAPDB_UInt8                 srcMaxRunnableMicroSeconds,
                          SAPDB_UInt4                 dstUKTIndex,
                          SAPDB_UInt8                 dstMaxRunnableMicroSeconds );

        ///! @name Time of the task move operation (microseconds since 1.1.1970)
        SAPDB_UInt8                 GetMoveTime() const                    { return m_MoveTime; }

        ///! @name Index of the task moved between two UKTs
        SAPDB_UInt4                 GetTaskIndex() const                   { return m_TaskIndex; }
        ///! @name The maximum time the moved task was waiting to get the running state (microseconds)
        SAPDB_UInt8                 GetTaskMaxRunnableMicroSeconds() const { return m_TaskMaxRunnableMicroSeconds; }

        ///! @name Index of the source UKT
        SAPDB_UInt4                 GetSrcUKTIndex() const                 { return m_SrcUKTIndex; }
        ///! @name The maximum time a task of the source UKT was waiting to get the running state (microseconds)
        SAPDB_UInt8                 GetSrcMaxRunnableMicroSeconds() const  { return m_SrcMaxRunnableMicroSeconds; }
        ///! @name Index of the destination UKT
        SAPDB_UInt4                 GetDstUKTIndex() const                 { return m_DstUKTIndex; }
        ///! @name The maximum time a task of the destination UKT was waiting to get the running state (microseconds)
        SAPDB_UInt8                 GetDstMaxRunnableMicroSeconds() const  { return m_DstMaxRunnableMicroSeconds; }

    private:
        SAPDB_UInt8                 m_MoveTime;
        SAPDB_UInt4                 m_TaskIndex;
        SAPDB_UInt8                 m_TaskMaxRunnableMicroSeconds;
        SAPDB_UInt4                 m_SrcUKTIndex;
        SAPDB_UInt8                 m_SrcMaxRunnableMicroSeconds;
        SAPDB_UInt4                 m_DstUKTIndex;
        SAPDB_UInt8                 m_DstMaxRunnableMicroSeconds;
    };

    /*!
       @brief    Fetch the first task move information of the current snapshot.
                 Returns null if none exists.
     */
    RTETask_MoveInfo const * const GetFirstTaskMoveInfo();
    /*!
       @brief    Fetch the next task move information of the current snapshot.
                 Returns null if none exists.
     */
    RTETask_MoveInfo const * const GetNextTaskMoveInfo();


public:
    /*!
      @brief     UKT related task load balancing information
      
      This class describes the UKT releated task load balncing information of the
      currently running measurement interval.
     */
    class RTETask_UKTMoveInfo
    {
    public:
        RTETask_UKTMoveInfo();
        RTETask_UKTMoveInfo( SAPDB_UInt4                 UKTIndex,
                             SAPDB_UInt4                 moveTaskToUKTIndex,
                             SAPDB_UInt4                 numOfMovableTasks,
                             SAPDB_UInt8                 m_TaskMoveIntervalStartTime,
                             SAPDB_UInt8                 UKTSleepTimeMicroSeconds,
                             SAPDB_UInt8                 runnableMicroSeconds,
                             SAPDB_UInt8                 maxRunnableMicroSeconds );

        ///! @name Index of the UKT
        SAPDB_UInt4                 GetUKTIndex() const                   { return m_UKTIndex; }
        ///! @name If not zero, its the destination UKT a task has to be moved to.
        SAPDB_UInt4                 GetMoveTaskToUKTIndex() const         { return m_MoveTaskToUKTIndex; }

        ///! @name Number of movable tasks 
        SAPDB_UInt4                 GetNumOfMovableTasks() const          { return m_NumOfMovableTasks; }

        ///! @name Currently runninig interval start time (microseconds since 1.1.1970)
        SAPDB_UInt8                 GetTaskMoveIntervalStartTime() const  { return m_TaskMoveIntervalStartTime; }

        ///! @name UKTs sleeptime (microseconds).
        SAPDB_UInt8                 GetUKTSleepTimeMicroSeconds() const   { return m_UKTSleepTimeMicroSeconds; }

        ///! @name Sum of runnable time of UKTs tasks (microseconds).
        SAPDB_UInt8                 GetRunnableMicroSeconds() const       { return m_RunnableMicroSeconds; }

        ///! @name The maximum time a task of the UKT was waiting to get the running state (microseconds).
        SAPDB_UInt8                 GetMaxRunnableMicroSeconds() const    { return m_MaxRunnableMicroSeconds; }

    private:
        SAPDB_UInt4                 m_UKTIndex;
        SAPDB_UInt4                 m_MoveTaskToUKTIndex;
        SAPDB_UInt4                 m_NumOfMovableTasks;
        SAPDB_UInt8                 m_TaskMoveIntervalStartTime;
        SAPDB_UInt8                 m_UKTSleepTimeMicroSeconds;
        SAPDB_UInt8                 m_RunnableMicroSeconds;
        SAPDB_UInt8                 m_MaxRunnableMicroSeconds;
    };

    /*!
       @brief    Get the UKT move information of the first UKT.
     */
    RTETask_UKTMoveInfo const * const  GetFirstUKTMoveInfo();

    /*!
       @brief    Get the UKT move information of the next UKT.
                 Returns null of no more UKTs available.
     */
    RTETask_UKTMoveInfo const * const  GetNextUKTMoveInfo ();

private:
   SAPDB_Int4                          m_CheckIntervalTime;
   SAPDB_UInt2                         m_ValueEquality;
   SAPDB_UInt2                         m_MinimumDiff;
   SAPDB_UInt8                         m_TaskMoveIntervalStartTime;
   SAPDB_UInt2                         m_TaskMoveMeasIntervals;

   SAPDB_UInt2                         m_CurrTaskMoveInfo;
   SAPDB_UInt2                         m_NumOfTaskMoveInfos;

   SAPDB_UInt2                         m_CurrUKTMoveInfo;
   SAPDB_UInt2                         m_NumOfUKTMoveInfos;

   RTETask_MoveInfo                    m_TaskMoveInfos[NUM_OF_TASK_MOVE_STAT_RECS];
   RTETask_UKTMoveInfo*                m_pUKTMoveInfo;
};

#endif  /* RTETASK_LOADBALANCINGINFO_HPP */
