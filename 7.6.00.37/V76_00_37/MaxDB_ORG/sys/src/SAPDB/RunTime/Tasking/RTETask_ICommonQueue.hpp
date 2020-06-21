/*!========================================================================

  @file         RTETask_ICommonQueue.hpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Queue handling for common tasking queue elements

                This is a interface class for queues dealing with
                common tasking queue elements

  @since        2003-09-25  12:05
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


#ifndef RTETASK_ICOMMONQUEUE_HPP
#define RTETASK_ICOMMONQUEUE_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include    "RunTime/RTE_Types.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

struct RTETask_CommonQueueElement;

/*!---------------------------------------------------------------------------
  @class   RTETask_ICommonQueue
  @brief   Interface class for common queue element queues

           This is a interface class for queues dealing with
           common tasking queue elements
 ----------------------------------------------------------------------------*/
class RTETask_ICommonQueue
{
public:
    /*!
    @brief         Enqueue a common queue element.

                   If a queue is created as freelist a dequeued element can be
                   returned to its freelist without knowing the queue instance.
                   This is done by calling the queue elements 'RelaseToFreeList'
                   methode. So it's not usefull to enque elements to there freelist
                   by  calling this 'Enqueue' methode directly. The queue element 
                   class 'RTETask_CommonQueueElement' calls this methode
                   to relase itself to the free list if 'RelaseToFreeList'
                   is called.

    @param         queueElement [in] - Common queue element.
    */
    virtual void  Enqueue( RTETask_CommonQueueElement &queueElement ) = 0;

    /*!
    @brief         Dequeue a common queue element.
    @return        Common queue element.
    */
    virtual RTETask_CommonQueueElement*  Dequeue() = 0;

   /*!
    @brief         Get number of queue element in queue.
    @return        Number of queue element in queue.
    */
    virtual SAPDB_UInt4 NumberOfQueueElements() const = 0;

   /*!
    @brief         Check if queue is empty.
    @return        Returns true if queue is empty.
    */
    virtual SAPDB_Bool QueueIsEmpty() const = 0;
};



#endif  /* RTETASK_ICOMMONQUEUE_HPP */
