/*!========================================================================

  @file         RTETask_CommonQueue.hpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Queue handling for common tasking queue elements

                This class is designed for queues dealing with
                common tasking queue elements.

  @since        2005-01-20  13:36
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


#ifndef RTETASK_COMMONQUEUE_HPP
#define RTETASK_COMMONQUEUE_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include    "RunTime/RTE_Types.hpp"
#include    "RunTime/Tasking/RTETask_DoublyLinkedList.hpp"
#include    "RunTime/Synchronisation/RTESync_Spinlock.hpp"
#include    "RunTime/Tasking/RTETask_ICommonQueue.hpp"

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
  @class   RTETask_CommonQueue
  @brief   Queue handling for common tasking queue elements

           This class is designed for queues dealing with
           common tasking queue elements.

           A special constructor allows to create a freelist
           for common queue elements. A free list is a container 
           holding the unused queue elements.

           THIS QUEUE IS THE NONE LOCKED VARIANT WHICH MEANS IT USES
           NO LOCKED SCOPES DURING ENQUEUE AND DEQUEUE OPERATIONS.

           A LOCKED VARIANT FOLLOWS!!!!

 ----------------------------------------------------------------------------*/

class RTETask_CommonQueue : public RTETask_ICommonQueue
{
public:
    /*!
    @brief         Constructor.

                   Contructor to create an empty queue which is filled
                   during runtime.

    @param         queueName         [in] - Give the queue a name for error messages
    */
    RTETask_CommonQueue( SAPDB_Char const * const  queueName ) 
                     : m_QueueName ( queueName ),
                       m_DBGFirstQueueElement(0),
                       m_ItsAFreeList( false ) {;}

    /*!
    @brief         Constructor.

                   This special constructor is used to create a queue acting as
                   free list for common queue elements. A dequeued element can
                   be returned to its freelist without knowing the queue instance
                   by calling the queue elements 'RelaseToFreeList' methode 

    @param         queueElements    [in] - Array of queue elements.
    @param         numberOfElements [in] - Number of array elements.
    @param         ListName         [in] - Give the list a name for error messages
    */
   RTETask_CommonQueue( RTETask_CommonQueueElement*  pQueueElements,
                        SAPDB_UInt4                  numberOfElements,
                        SAPDB_Char const * const     listName );

    /*!
    @brief         Dequeue a common queue element.
    @return        Common queue element.
    */
    virtual RTETask_CommonQueueElement* Dequeue();

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
    virtual void Enqueue( RTETask_CommonQueueElement &queueElement );


   /*!
    @brief         Number of queue element in common queue.
    */
    virtual SAPDB_UInt4 NumberOfQueueElements()  const;

   /*!
    @brief         Check if queue is empty.
    @return        Returns true if queue is empty.
    */
    virtual SAPDB_Bool QueueIsEmpty() const;

protected:
    // don't allow copies
    inline   RTETask_CommonQueue( const RTETask_CommonQueue &c ) 
               : m_QueueName(c.m_QueueName) {;}

private:
    RTETask_DoublyLinkedList<DummyLockedScopeDLList,DummyLockDLList> m_DLList;
    SAPDB_Char const * const                                         m_QueueName;
    SAPDB_Bool                                                       m_ItsAFreeList;
    RTETask_CommonQueueElement*                                      m_DBGFirstQueueElement; // for debug purposes
};


/*!---------------------------------------------------------------------------
  @class   RTETask_SynchronizedCommonQueue
  @brief   Queue handling for common tasking queue elements

           This class is designed for queues dealing with
           common tasking queue elements.

           A special constructor allows to create a freelist
           for common queue elements. A free list is a container 
           holding the unused queue elements.

           THIS FREE LIST IS THREAD SAVE BECAUSE ITS USING A LOCKED SCOPE

 ----------------------------------------------------------------------------*/

class RTETask_SynchronizedCommonQueue : public RTETask_ICommonQueue
{
public:
    /*!
    @brief         Constructor.

                   Contructor to create an empty queue which is filled
                   during runtime.

    @param         queueName         [in] - Give the queue a name for error messages
    */
   RTETask_SynchronizedCommonQueue( SAPDB_Char const * const     queueName )
                     : m_QueueName ( queueName ),
                       m_DBGFirstQueueElement(0),
                       m_ItsAFreeList( false ) {;}

    /*!
    @brief         Constructor.

                   This special constructor is used to create a queue acting as
                   free list for common queue elements. A dequeued element can
                   be returned to its freelist without knowing the queue instance
                   by calling the queue elements 'RelaseToFreeList' methode 

    @param         queueElements    [in] - Array of queue elements.
    @param         numberOfElements [in] - Number of array elements.
    @param         ListName         [in] - Give the list a name for error messages
    */
   RTETask_SynchronizedCommonQueue( RTETask_CommonQueueElement*  pQueueElements,
                                    SAPDB_UInt4                  numberOfElements,
                                    SAPDB_Char const * const     listName );

    /*!
    @brief         Dequeue a common queue element.
    @return        Common queue element.
    */
    virtual RTETask_CommonQueueElement* Dequeue();

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
    virtual void Enqueue( RTETask_CommonQueueElement &queueElement );


   /*!
    @brief         Number of queue element in common queue.
    */
    virtual SAPDB_UInt4 NumberOfQueueElements() const;

   /*!
    @brief         Check if queue is empty.
    @return        Returns true if queue is empty.
    */
    virtual SAPDB_Bool QueueIsEmpty() const;

protected:
    // don't allow copies
    inline   RTETask_SynchronizedCommonQueue( const RTETask_SynchronizedCommonQueue &c )
               : m_QueueName(c.m_QueueName) {;}
private:
    RTETask_DoublyLinkedList<RTESync_LockedScope,RTESync_Spinlock> m_DLList;
    SAPDB_Char const * const                                       m_QueueName;
    SAPDB_Bool                                                     m_ItsAFreeList;
    RTETask_CommonQueueElement*                                    m_DBGFirstQueueElement; // for debug purposes
};

#endif  /* RTETASK_COMMONQUEUE_HPP */
