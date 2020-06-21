/*!========================================================================

  @file         RTETask_UnconnectedTaskPool.hpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Pool deals with unconnected tasks.

                The requestor uses this pool to fulfil incomming
                connect requests for a specific task type

  @since        2005-07-19  10:44
  @sa           

  ==========================================================================

  \if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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


#ifndef RTETASK_UNCONNECTEDTASKPOOL_HPP
#define RTETASK_UNCONNECTEDTASKPOOL_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include    "RunTime/RTE_KGS.hpp"
#include    "SAPDBCommon/SAPDB_Types.hpp"
#include    "RunTime/Tasking/RTETask_ITask.hpp"
#include    "RunTime/Tasking/RTETask_CommonQueueElement.hpp"
#include    "RunTime/Synchronisation/RTESync_NamedSpinlock.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

struct RTETask_TaskScheduler;

typedef void (* searchUnreleasedConnectionsCallback )( bool ); 


/*!---------------------------------------------------------------------------
  @class   RTETask_UnconnectedTaskPool
  @brief   Pool deals with unconnected tasks.

           The requestor uses this pool to fulfil incomming
           connect requests for a specific task type.

 ----------------------------------------------------------------------------*/

class RTETask_UnconnectedTaskPool
{
public:
    /*! 
    @brief    Connect strategy

              This type determines the strategy choosing a unconnected task.
    */
    enum Strategy 
    {
        Equalize = 1,  // Equally distribute the connect requests to all TaskSchedulers.
        Compress       // Choose the next TaskScheduler only if all tasks are connected.
    };

    /*!
      @brief         constructor.

                     Build up a pool object which deals with unconnected tasks. 
      @param strategy                [in] - the strategy to be used
      @param searchUnrelConnCallback [in] - callback to allow release of laisy released tasks
    */
    RTETask_UnconnectedTaskPool( Strategy strategy, searchUnreleasedConnectionsCallback searchUnrelConnCallback )
        : m_KGS(RTE_KGS::Instance())
        , m_strategy(strategy)
        , m_SearchUnreleasedConnectionsCallback(searchUnrelConnCallback)
        , m_OutOfSessionEventRegistered(false)
        , m_Lock((SAPDB_UTF8 const *)"UnconnectedTaskPool")
    { 
    }

    /*!
      @brief         Get a task from unconnected task pool

                     This methode returns a common queue element of a task
                     which is waiting for a connect request. The queue element 
                     contains the task object pointer. If no unconnected task 
                     is a available this methode returns null. 

                     The returned queue element can be reused to forward the
                     connect request to the tasks.

      @param         taskType  [in]        - requested type of task.

      @return        Pointer to the common queue element of a unconnected task if any
                     else null.
    */
    RTETask_CommonQueueElement*   GetTask( RTETask_ITask::TaskType taskType );

    /*!
      @brief         Called whenever task has been added to unconnected any task queue

      @return none
     */
    void AddTask();

private:
    /*!
      @brief         Find the best suitable TaskScheduler

                     This methode returns the best suitable TaskScheduler with
                     has at least one unconnected task of the requested task type.
                     If no unconnected task is a available this methode returns null.

      @param         taskType [in] - requested type of task.
      @param         almostOutOfSessions [out] - flag set if almost of of sessions ( 1 or 0 free )
      @return        Pointer to the best suitable TaskScheduler object if any
                     else null.
    */
    inline  RTETask_TaskScheduler* FindBestTaskScheduler( RTETask_ITask::TaskType taskType, SAPDB_Bool &almostOutOfSessions );

private:
    RTE_KGS                                &m_KGS;            // Shortcut to kgs
    Strategy                                m_strategy;
    searchUnreleasedConnectionsCallback     m_SearchUnreleasedConnectionsCallback; 
    SAPDB_Bool                              m_OutOfSessionEventRegistered;
    RTESync_NamedSpinlock                   m_Lock;
};


/*!
  @brief  convert RTETask_UnconnectedTaskPool::Strategy into readable string
  @param  state [in] the return value to convert
 */
inline SAPDB_ToStringClass SAPDB_ToString( RTETask_UnconnectedTaskPool::Strategy strategy )
{
    switch (strategy)
    {
    case RTETask_UnconnectedTaskPool::Equalize : return SAPDB_ToStringClass("Equalize");
    case RTETask_UnconnectedTaskPool::Compress : return SAPDB_ToStringClass("Compress");
    }

    return SAPDB_ToStringClass((SAPDB_Int4)strategy);
}

#endif  /* RTETASK_UNCONNECTEDTASKPOOL_HPP */
