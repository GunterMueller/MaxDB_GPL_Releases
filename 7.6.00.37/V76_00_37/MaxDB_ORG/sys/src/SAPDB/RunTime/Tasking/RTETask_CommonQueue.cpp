/*!========================================================================

  @file         RTETask_CommonQueue.cpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Queue handling for common tasking queue elements

                This class is designed for queues dealing with
                common tasking queue elements


  @since        2005-01-20  13:37
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



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#include    "RunTime/Tasking/RTETask_CommonQueue.hpp"
#include    "RunTime/RTE_Crash.hpp"
#include    "RunTime/RTE_MessageList.hpp"
#include    "RunTime/RTE_Task_Messages.hpp"
#include    "RunTime/RTE_Types.hpp"
#include    "RunTime/Tasking/RTETask_CommonQueueElement.hpp"


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


RTETask_CommonQueue ::RTETask_CommonQueue( RTETask_CommonQueueElement*  pQueueElements,
                                           SAPDB_UInt4                  numberOfElements,
                                           SAPDB_Char const * const     listName )
        : m_QueueName ( listName ),
          m_DBGFirstQueueElement(pQueueElements), // It might help to locate all queue elements during debugging
          m_ItsAFreeList( true )
{
    for ( SAPDB_UInt4 i = 0; i < numberOfElements; i++ )
    {
        pQueueElements[i].SetFreeList(*this);
        m_DLList.InsertTail( (pQueueElements[i]) );
    }
}

/*---------------------------------------------------------------------------*/

void RTETask_CommonQueue::Enqueue( RTETask_CommonQueueElement &queueElement )
{
    m_DLList.InsertTail( queueElement );
}


/*---------------------------------------------------------------------------*/

RTETask_CommonQueueElement* RTETask_CommonQueue::Dequeue()
{
    RTETask_CommonQueueElement* queueElement;

    queueElement  = reinterpret_cast<RTETask_CommonQueueElement*>( m_DLList.RemoveHead());

    if (( m_ItsAFreeList ) && ( 0 == queueElement ))
    {
        RTE_Crash(SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_TASK_FREE_LIST_IS_EMPTY, m_QueueName ));
    }

    return queueElement;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4 RTETask_CommonQueue::NumberOfQueueElements() const
{
    return m_DLList.ListLen();
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTETask_CommonQueue::QueueIsEmpty() const
{
    return 0 == m_DLList.ListLen();
}

/*---------------------------------------------------------------------------*/

RTETask_SynchronizedCommonQueue
       ::RTETask_SynchronizedCommonQueue( RTETask_CommonQueueElement*  pQueueElements,
                                          SAPDB_UInt4                  numberOfElements,
                                          SAPDB_Char const * const     listName )
        : m_QueueName ( listName ),
          m_DBGFirstQueueElement(pQueueElements), // It might help to locate all queue elements during debugging
          m_ItsAFreeList( true )
{
    for ( SAPDB_UInt4 i = 0; i < numberOfElements; i++ )
    {
        pQueueElements[i].SetFreeList(*this);
        m_DLList.InsertTail( (pQueueElements[i]) );
    }
}

/*---------------------------------------------------------------------------*/

void RTETask_SynchronizedCommonQueue::Enqueue( RTETask_CommonQueueElement &queueElement )
{
    m_DLList.InsertTail( queueElement );
}


/*---------------------------------------------------------------------------*/

RTETask_CommonQueueElement* RTETask_SynchronizedCommonQueue::Dequeue()
{
    RTETask_CommonQueueElement* queueElement;

    queueElement  = reinterpret_cast<RTETask_CommonQueueElement*>( m_DLList.RemoveHead());

    if (( m_ItsAFreeList ) && ( 0 == queueElement ))
    {
        RTE_Crash(SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_TASK_FREE_LIST_IS_EMPTY, m_QueueName ));
    }

    return queueElement;
}

/*---------------------------------------------------------------------------*/

SAPDB_UInt4 RTETask_SynchronizedCommonQueue::NumberOfQueueElements() const
{
    return m_DLList.ListLen();
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool RTETask_SynchronizedCommonQueue::QueueIsEmpty() const
{
    return 0 == m_DLList.ListLen();
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/