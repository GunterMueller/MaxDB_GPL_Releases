/*!
  @file     RTE_ConsoleSemaphoreTimeoutList.hpp
  @author   StefanP,Raymond
  @ingroup  Console
  @brief    Timeout support for Semaphores between different Processes (System Scope).
            This Module provides a Class for a List of Semaphores to be observed for a Timeout.

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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



#ifndef RTE_CONSOLESEMAPHORETIMEOUTLIST_HPP
#define RTE_CONSOLESEMAPHORETIMEOUTLIST_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
# if !defined (_WIN32)

#include    "heo07.h"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include    "RunTime/MemoryManagement/RTEMem_RteAllocator.hpp" 


extern SAPDBTrace_Topic Console_Trace; ///< global console trace object

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
#define CONSOLE_SEMAPHORE_TIMEOUT_WORKER_PRIO               ((tsp00_Int4)1)
#define CONSOLE_SEMAPHORE_TIMEOUT_WORKER_STACK_SIZE         (32*1024)
/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

struct TimeoutListElement;

/*!
  @class        RTE_ConsoleSemaphoreTimeoutList
  @description  Singleton timeout list for console semaphores (RTE_ConsoleSemaphore)
                This list can be used to monitor semaphores for a given timeout.
                Methods for enqueue and dequeue a semaphore are offered. 
*/

class RTE_ConsoleSemaphoreTimeoutList 
{
public:
    /*!
       @description This method creates the singleton. It is assumed 
                    that this method will be callled only one time and therefore 
                    no synchonization mechanism is used.
       @param       maxSemaphores   [in]    - Maximal number of semaphores to be monitored
       @return      true means instance could be created; else false
     */

    static SAPDB_Bool CreateInstance        (SAPDB_UInt4 const          maxSemaphores,
                                             SAPDB_UInt8                maxSleeptime,
                                             SAPDBErr_MessageList &     messageList);
    
    /*!
       @description This method returns the access to the timeout list. It is 
                    assumed that the singleton already exists! No CreateInstance call 
                    is be done.
       @return      Instance
     */

    static RTE_ConsoleSemaphoreTimeoutList& GetInstance ()
            {SAPDBTRACE_ROUTINE_DEBUG ("RTE_ConsoleSemaphoreTimeoutList::GetInstance", 
                                      Console_Trace, 9);
             return( *m_Instance );}

    
    /*!
       @description Allocation of the timeout list       
       @return      true/false
     */

    SAPDB_Bool      Initialize              (SAPDBErr_MessageList &     messageList);

    /*!
       @description Destructor
                    Destruction of the timeout list.
    */

    ~RTE_ConsoleSemaphoreTimeoutList            ();


    /*!
       @description Posting all semaphore for which a timeout has expired
       @param       nextSleeptime [in/out]  - in:   Maximum of time seconds when the next timeout will occur
                                              out:  Time interval in seconds for the next timeout
                                                    if smaller than given value. If no timeout has to be monitored
                                                    the given value is returned.
       @param       messageList [in/out]    - Message list

       @return     true/false
    */

    SAPDB_Bool      PostTimedOutSemaphores      (SAPDB_UInt8 &                          nextSleeptime,
                                                 SAPDBErr_MessageList &                 messageList);  

    /*!
       @description Enqueue a semaphore in the timeout list.     
       @param       pSemaphore [in]         - Semaphore to be enqueued
       @param       timeout    [in]         - Timeout for the semaphore
       @param       index      [in]         - Index in the timeout list (= Index of the current worker thread)
       @param       finalWait  [in]         - true: A timeout to the current wait is final, i.e. no further
                                                    wait for this semaphore will be done. 
       @param       messageList [in/out]    - Message list

       @return      true/false   
    */

    SAPDB_Bool      EnqueueSemaphore            (RTE_ConsoleSemaphore * const           pSemaphore,
                                                 SAPDB_UInt8 const                      timeout,
                                                 SAPDB_UInt4 const                      index,
                                                 SAPDB_Bool const                       finalWait,
                                                 SAPDBErr_MessageList &                 messageList);

    /*!
        @description Dequeue a semaphore from the timeout list.
        @param       index      [in]        - Index in the timeout list (= Index of the current worker thread)
        @param       timedOut   [out]       - Semaphore has timed out
        @param       messageList [in/out]    - Message list
        @return    true/false
    */

    SAPDB_Bool      DequeueSemaphore            (SAPDB_UInt4 const                      index,
                                                 SAPDB_Bool &                           timedOut,
                                                 SAPDBErr_MessageList &                 messageList);



private:
    /*!
        @description     The default constructor is privat to this class to avoid 
                         instantiation outside of the class.
    */

    RTE_ConsoleSemaphoreTimeoutList             (SAPDB_UInt4                            maxSemaphores)
        :   m_MaxSemaphores (maxSemaphores),
            m_pTimeoutList   (NULL)
            {SAPDBTRACE_METHOD_DEBUG ("RTE_ConsoleSemaphoreTimeoutList::RTE_ConsoleSemaphoreTimeoutList", 
                                      Console_Trace, 9);
                sqlcreatemutex (&m_TimeoutListMutex);}

    void            Lock   ()
        {SAPDBTRACE_METHOD_DEBUG ("RTE_ConsoleSemaphoreTimeoutList::Lock", 
                                      Console_Trace, 9);
         sqlbeginmutex(&m_TimeoutListMutex);}


    void            Unlock ()
        {SAPDBTRACE_METHOD_DEBUG ("RTE_ConsoleSemaphoreTimeoutList::Unlock", 
                                      Console_Trace, 9);
         sqlendmutex(&m_TimeoutListMutex);}


    static  RTE_ConsoleSemaphoreTimeoutList *   m_Instance;

    TimeoutListElement *                        m_pTimeoutList;
    teo07_Mutex                                 m_TimeoutListMutex;
    SAPDB_UInt4                                 m_MaxSemaphores;
};


#endif

#endif  /* RTE_CONSOLESEMAPHORETIMEOUTLIST_HPP */
