/*!========================================================================

  @file         RTE_PendingEventHandler.cpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Handling of pending events

                A pending event describe a status that must be reported continously to allow a later
                start event task to be informed. A pending event can be registered and deregistered.
                The registration automatically issues the event for the first time. The deregistration
                does nothing but deregistration... The event itself is opaque, so any kind of event
                may be set as a pending event. Only the event type is used as identifier.

  @since        2005-01-10  14:01
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

#include "RunTime/RTE_Types.h"
#include "heo07.h"
#include "heo60.h"
#include "geo60.h"
#include "geo007_1.h"
#include "RunTime/RTE_PendingEventHandler.h"
#include "RunTime/Synchronisation/RTESync_NamedSpinlock.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "RunTime/MemoryManagement/RTEMem_RteAllocator.hpp"
#include "RunTime/System/RTESys_AtomicOperation.hpp"
#include "geo50_0.h"

#define RTE_PENDING_EVENT_WORKER "PendingEventWorker"

struct PendingEvent
{
    PendingEvent *next;      //< next in list
    PendingEvent *previous;  //< previous in list
    tsp31_event_description event; //< the payload
    SAPDB_UInt4             reissue_interval_in_seconds; //< reissue interval
    SAPDB_UInt8             lastIssue; //< last time issued (seconds since epoch)
};

//----

class RTE_PendingEventList
{
public:
    /*!
      @brief initialize pending event handler
     */
    RTE_PendingEventList() : m_TimeToSleepInSeconds( RTE_PENDING_EVENT_INTERVAL_DEFAULT ),
                             m_Head(0),
                             m_Lock((SAPDB_UTF8 const *)RTE_PENDING_EVENT_WORKER)
    {
    }

    /*!
      @brief insert given event into pending event list
      @param event [in] which event
      @param reissue_interval_in_seconds [in] reissue interval in seconds
      @param now [in] current time
     */
    SAPDB_Bool InsertEvent( tsp31_event_description const &event, 
                            SAPDB_UInt4                   reissue_interval_in_seconds, 
                            SAPDB_UInt8                   now )
    {
        PendingEvent *newEntry = new ( RTEMem_RteAllocator::Instance() ) PendingEvent;

        if ( !newEntry )
        {
            return false;
        }

        newEntry->next  = 0;
        newEntry->previous = 0;
        newEntry->event = event;
        newEntry->reissue_interval_in_seconds = reissue_interval_in_seconds;
        newEntry->lastIssue = now;

        SAPDB_Bool needsReporting = false;
        SAPDB_Bool alreadyRegistered;

        alreadyRegistered = LockedInsert( newEntry, event.sp31ed_ident, needsReporting );

        if ( needsReporting )
        {
            vinsert_event(newEntry->event);
            SetTimeToSleepInSeconds(reissue_interval_in_seconds);
        }

        if ( alreadyRegistered )
        {
            destroy ( newEntry, RTEMem_RteAllocator::Instance() );
        }

        return true;
    }

    /*!
      @brief delete given event from pending event list
      @param eventIdentifier [in] which event
     */
    void DeleteEvent(tsp31_event_ident_Param const eventIdentifier)
    {
        PendingEvent *foundEntry = 0;

        if ( LockedDelete( eventIdentifier, foundEntry ) )
        {
            destroy ( foundEntry, RTEMem_RteAllocator::Instance() );
        }
    }

    /*!
      @brief report all outstanding pending events to event ring buffer

      time jitter due to inserting events is ignored

      @param now [in] time now in seconds since epoch
     */
    void ReportEvents(SAPDB_UInt8 now)
    {
        RTESync_LockedScope lock(m_Lock);
        SAPDB_UInt4 timeToSleep = RTE_PENDING_EVENT_INTERVAL_DEFAULT;
        {
            for ( PendingEvent *pEntry = m_Head; pEntry; pEntry = pEntry->next )
            {
                if ( pEntry->lastIssue < now )
                {
                    if ( (pEntry->lastIssue + pEntry->reissue_interval_in_seconds) <= now )
                    {
                        pEntry->lastIssue = now;
                        vinsert_event( pEntry->event );
                        if ( timeToSleep > pEntry->reissue_interval_in_seconds )
                            timeToSleep = pEntry->reissue_interval_in_seconds;
                    }
                    else
                    {
                        if ( timeToSleep > (SAPDB_UInt4)(now - pEntry->lastIssue) )
                            timeToSleep = (SAPDB_UInt4)(now - pEntry->lastIssue);
                    }
                }
            }
        }
        m_TimeToSleepInSeconds = timeToSleep;
    }


    /*!
      @brief update sleep time interval used by event polling thread
      If given sleep interval is larger than actual interval, call does nothing
      @param timeToSleepInSeconds [in] minimum sleep time interval wanted
     */
    void SetTimeToSleepInSeconds(SAPDB_UInt4 timeToSleepInSeconds)
    {
        SAPDB_UInt4 currentTimeToSleepInSeconds = m_TimeToSleepInSeconds;

        while ( currentTimeToSleepInSeconds > timeToSleepInSeconds 
             && RTESys_CompareAndExchange( m_TimeToSleepInSeconds
                                         , currentTimeToSleepInSeconds
                                         , timeToSleepInSeconds
                                         , currentTimeToSleepInSeconds ) )
                                         ;
    }

    /*!
      @brief return next time to wakeup
      @return wakeup time in seconds since epoch 
     */
    SAPDB_UInt4 GetTimeToSleepInSeconds()
    {
        return RTESys_AtomicRead(m_TimeToSleepInSeconds);
    }

private:

    SAPDB_Bool LockedInsert( PendingEvent             *newEntry,
                             tsp31_event_ident_Param  eventIdentifier,
                             SAPDB_Bool               &needsReporting )
    {
        RTESync_LockedScope lock(m_Lock);

        if ( m_Head )
        {
            if ( m_Head->event.sp31ed_ident == eventIdentifier )
            {
                m_Head->reissue_interval_in_seconds = newEntry->reissue_interval_in_seconds;
                m_Head->lastIssue = newEntry->lastIssue;
                needsReporting = true;
                return true;
            }
            else
            {
                for ( PendingEvent *pEntry = m_Head->next; pEntry; pEntry = pEntry->next )
                {
                    if ( pEntry->event.sp31ed_ident == eventIdentifier )
                    {
                        pEntry->reissue_interval_in_seconds = newEntry->reissue_interval_in_seconds;
                        pEntry->lastIssue = newEntry->lastIssue;
                        needsReporting = true;
                        return true;
                    }
                }
            }
        }

        newEntry->next = m_Head;
        m_Head         = newEntry;
        if ( newEntry->next )
        {
            newEntry->next->previous = newEntry;
        }

        needsReporting = true;
        return false; // not already registered !
    }

    //-----------

    SAPDB_Bool LockedDelete( tsp31_event_ident_Param const eventIdentifier, 
                             PendingEvent *                &dequeuedEntry )
    {
        RTESync_LockedScope lock(m_Lock);

        if ( m_Head )
        {
            if ( m_Head->event.sp31ed_ident == eventIdentifier )
            {
                dequeuedEntry = m_Head;
                m_Head = m_Head->next;
                return true;
            }
            else
            {
                for ( PendingEvent *pEntry = m_Head->next; pEntry; pEntry = pEntry->next )
                {
                    if ( pEntry->event.sp31ed_ident == eventIdentifier )
                    {
                        dequeuedEntry = pEntry;
                        pEntry->previous->next = pEntry->next;
                        pEntry->next->previous = pEntry->previous;
                        return true;
                    }
                }
            }
        }
        return false;
    }

    SAPDB_UInt4            m_TimeToSleepInSeconds;
    PendingEvent          *m_Head;
    RTESync_NamedSpinlock  m_Lock;
};

//-----------

RTE_PendingEventHandler* RTE_PendingEventHandler::m_Instance = 0;

//-----------

RTE_PendingEventHandler & RTE_PendingEventHandler::Instance()
{
    SAPDBMEM_STATIC_RAW_ALLOCATE(Space, sizeof(RTE_PendingEventHandler));

    if ( !m_Instance )
    {
        m_Instance = new (Space) RTE_PendingEventHandler();
    }
    return *m_Instance;
}

//-----------

RTE_PendingEventHandler::RTE_PendingEventHandler() 
                        : RTEThread_KernelThread(RTE_PENDING_EVENT_WORKER)
{
    SAPDBErr_MessageList                   messageList;

    // If the thread creation fails it will be ignored because
    // event handling is not a core feature of the database kernel 
    if ( Create( RTE_PendingEventHandler::Normal, 64*1024,
                 messageList ) != RTE_PendingEventHandler::NoError )
    {
        RTE_Message( messageList );
        return;
    }

    Forget();
}
 
//-----------

/*
   This thread sleeps for one second, checks if reporting event is needed and 
   if needed insert event copies into event queue.
*/

SAPDB_Int4  RTE_PendingEventHandler::KernelThreadMain()
{
    SAPDB_UInt4             timeWaited = 0;

    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // We use a local variable for the event list because this thread
    // never returns
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    RTE_PendingEventList    eventList;
    m_pList = &eventList;

    for ( ; true ; )
    {
        if ( m_pList->GetTimeToSleepInSeconds() <= timeWaited )
        {
            timeWaited = 0;
#if defined(WIN32)
            m_pList->ReportEvents(kgs.ulCurrTime);
#else
            m_pList->ReportEvents(KGS->current_time);
#endif
        }
        else
        {
            ++timeWaited;
        }
#if defined(WIN32)
        Sleep( 1000 );
#else
        sleep( 1 );
#endif
    }

    return 0;
}

//-----------

void RTE_PendingEventHandler::Register( tsp31_event_description const &event, 
                                        SAPDB_UInt4                   reissue_interval_in_seconds )
{
    if ( m_pList )
    {
        if ( 0 == reissue_interval_in_seconds )
        {
            reissue_interval_in_seconds = RTE_PENDING_EVENT_INTERVAL_DEFAULT;
        }

#if defined(WIN32)
        SAPDB_UInt8 now = (SAPDB_UInt8)(kgs.ulCurrTime);
#else
        SAPDB_UInt8 now = (SAPDB_UInt8)(KGS->current_time);
#endif

        (void)m_pList->InsertEvent(event, reissue_interval_in_seconds, now);
    }
}

//-----------

void RTE_PendingEventHandler::Deregister( tsp31_event_ident_Param const eventIdentifier )
{
    if ( m_pList )
    {
        m_pList->DeleteEvent(eventIdentifier);
    }
}

//-----------

extern "C" void RTE_RegisterPendingEvent( tsp31_event_description const &event, 
                                          SAPDB_UInt4                   reissue_interval_in_seconds )
{
    RTE_PendingEventHandler::Instance().Register(event, reissue_interval_in_seconds);
}

//-----------

extern "C" void RTE_DeregisterPendingEvent( tsp31_event_ident_Param const eventIdentifier )
{
    RTE_PendingEventHandler::Instance().Deregister(eventIdentifier);
}

