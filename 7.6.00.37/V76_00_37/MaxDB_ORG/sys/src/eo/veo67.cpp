/*!
  @file           veo67.cpp
  @author         JoergM
  @special area   Event Handling
  @brief          Event reporting, Waiting and wakeup actions
  @see            example.html ...

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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

#include "gsp00.h"
#include "vsp0031.h"
#include "geo00_0.h"
#include "geo00_2.h"
#include "geo50_0.h"
#include "geo007_1.h"
#include "heo52.h" // vdattime
#include "heo56.h"
#include "heo67.h"
#include "geo67.h"
#include "RunTime/RTE_PendingEventHandler.h"
#include "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"
/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define REASON_WAIT_FOR_EVENT_EO67 1
#define NO_EVENT_INDEX_EO67 0

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

#if defined(_WIN32)
#  define INIT_LOCK(_Exclusive) CLEARLOCK(_Exclusive)
#  define TASK_TO_CANCEL(_pTaskCtrl) (_pTaskCtrl)->KernelCtrlRecord.to_cancel
#else
#  define TASK_TO_CANCEL(_pTaskCtrl) (_pTaskCtrl)->rte_comm.to_cancel
#endif
#define ASSIGN_EO67(_l,_r) (_l).becomes(_r)

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

struct teo67_EventDescription
{
    tsp31_event_description m_Event;   // The last event (type is m_Event.sp31ed_ident)

    int                     Priority() const
    {
        return STATIC_CAST( int, 
                   STATIC_CAST( tsp31_event_prio_Enum, m_Event.sp31ed_priority ) );
    }

    tsp00_Int4              Index() const
    {
        return m_Event.sp31ed_eventcnt;
    }

};

/* ----------------------------------- */

struct teo67_EventTask
{
    tsp00_TaskId m_pid;           // Event Task Id
    bool         m_inUse;         // true if in use, false if not in use
    bool         m_isWaiting;     // true if waiting for event, false else
    bool         m_neverGotEvent; // true if never got event before
    tsp00_Int4  m_lastIndex;     // last event retrieved from queue

    void Init(const tsp00_TaskId connectedPid)
    {
        m_pid = connectedPid;
        m_inUse = true;
        m_neverGotEvent = true;
        m_isWaiting = false;
		m_lastIndex = NO_EVENT_INDEX_EO67;
    }

    void Reset()
    {
        m_inUse = false;
    }
};

/* ----------------------------------- */

class ceo67_Base
{
protected:
    teo67_EventDescription * m_DescArray; // Dynamic sized array for event descriptions
    teo67_EventTask *  m_TaskArray;       // Dynamic sized array for event tasks
    tsp00_Int4       m_MaxEventTasks;   // Maximum number of event tasks
    tsp00_Int4       m_MaxEvents;       // Maximum number of different events
    tsp00_Int4       m_TotalConnected;  // Total event tasks connected

private:
#if defined(_WIN32)
    LONG             m_ExclusiveValue; // Interlock
    LONG           * m_Exclusive;      // Interlock
#else
   ALONE_IN_CACHE(dum_before)
    ALONE_UKP_STRUCT m_Exclusive;
   ALONE_IN_CACHE(dum_behind)
#endif

public:
    /*
     * Size in bytes must be calculated as
     *  NumberOfEventTasks * sizeof(tsp00_TaskId) + 
     *  NumberOfEventsQueued * eo67_SizeofElem()
     * Space must point to such a space...
     * 
     * Return true on success, false on error
     */
    bool Initialize( tsp00_Int4 maxNumberOfEventTasks,
                     tsp00_Int4 maxNumberOfEvents );

    /*
     * Release an event task from queuing
     */
    bool ReleaseTask(tsp00_TaskId pid);

    /*
     * Validate that given task id is usefull.
     */
    bool ValidateTask(tsp00_TaskId pid, bool isAdded);

    /*
     * Search a specific task, return task index if found, -1 else
     */
    tsp00_Int4 SearchTask(tsp00_TaskId pid);

/*================================
 * Rest is INLINE Implementation.... 
 */
    static const tsp00_Int4 BaseSize( tsp00_Int4 maxNumberOfEventTasks,
                                      tsp00_Int4 maxNumberOfEvents )
    {
        return ALIGN_8BYTE_EO00( maxNumberOfEventTasks * sizeof(teo67_EventTask) )
             + ALIGN_8BYTE_EO00( maxNumberOfEvents     * sizeof(teo67_EventDescription) );
    }

    /*
     * Lock internal structure
     */
    void Lock()
    {
        WAIT_UNTIL_ALONE (m_Exclusive);
    }

    /*
     * Unlock internal structure
     */
    void Unlock()
    {
        CLEARLOCK (m_Exclusive);
    }

};

/* ----------------------------------- */

class teo67_Ring:public ceo67_Base
{
private:
    tsp00_Int4 m_HeadIndex;    // Index of first element 
    tsp00_Int4 m_TailIndex;    // Index of last element in ringbuffer
    tsp00_Int4 m_HeadPos;      // Index in description array of 'head' element
    tsp00_Int4 m_TailPos;      // Index in description array for next insert
    tsp00_Int4 m_TotalStored;  // Number of Events stored in Ringbuffer

    /*
     * This class cannot be constructed. Its space is once allocated from outside
     */
    teo67_Ring() {}

    bool FindNextEvent(tsp00_Int4 &lastIndex,
                        tsp31_event_description &newEvent);

    void UpdateHead();

    void HandleOverflow();

#ifdef TEST
    void DumpRing(char *action, tsp00_Int4 cursor);
#endif /* TEST */

public:
    static const tsp00_Int4 Size( tsp00_Int4 maxNumberOfEventTasks,
                                  tsp00_Int4 maxNumberOfEvents )
	{
        if ( maxNumberOfEventTasks > 0
          && maxNumberOfEvents > 0 )
        {
            return ALIGN_8BYTE_EO00( sizeof(teo67_Ring) )
                  + BaseSize( maxNumberOfEventTasks, maxNumberOfEvents );
        }
        else
        {
            return 0;
        }
	}

	bool Initialize( tsp00_Int4 maxNumberOfEventTasks,
					 tsp00_Int4 maxNumberOfEvents )
	{
        m_HeadIndex = NO_EVENT_INDEX_EO67;
        m_TailIndex = NO_EVENT_INDEX_EO67;
        m_HeadPos = 0;
        m_TailPos = 0;
        m_TotalStored = 0;
        return ceo67_Base::Initialize( maxNumberOfEventTasks, maxNumberOfEvents );
    }

    /*
     * Get an event for the task.
     * Return true if event was filled
     *        false if currently no event available
     */
    bool GetNextEvent( const tsp00_Int4 taskIdx,
                       tsp31_event_description &newEvent);

    /*
     * This enqueues an event into the ringbuffer
     */
    void InsertEvent(tsp31_event_description &newEvent);

    /*
     * Connect an event task to queuing
     */
    bool ConnectTask(tsp00_TaskId pid);
};

class teo67_EventInfo
{
    tsp00_Int4       m_SessionCount;   // Number of USER Sessions ( no EventSessions! ) connected
#if defined(_WIN32)
    LONG             m_ExclusiveValue; // Interlock
    LONG           * m_Exclusive;      // Interlock
#else
   ALONE_IN_CACHE(dum_before)
    ALONE_UKP_STRUCT m_Exclusive;
   ALONE_IN_CACHE(dum_behind)
#endif

public:
    teo67_EventInfo() :m_SessionCount(0)
    {
#ifdef WIN32
        m_Exclusive = &m_ExclusiveValue;
#endif /* WIN32 */
        INIT_LOCK(m_Exclusive);
    }

    /*
     * Lock internal structure
     */
    void Lock()
    {
        WAIT_UNTIL_ALONE (m_Exclusive);
    }

    /*
     * Unlock internal structure
     */
    void Unlock()
    {
        CLEARLOCK (m_Exclusive);
    }

    /* PTS 1104575 Errors always generate Events with high priority */
    /* PTS 1104575 Warnings always generate Events with medium priority */

    /* PTS 1104575 Sessions are not counted inside LZU, so we count here... */
    tsp00_Int4 SessionCountModify(tsp00_Int4 SessionCountModification)
    {
        tsp00_Int4 CurrentSessionCount;

        Lock();
            m_SessionCount += SessionCountModification;
            CurrentSessionCount = m_SessionCount;
        Unlock();

        return CurrentSessionCount;
    }
};

/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/

/*
 * Must be global, since Events could be produced even without any EventRingbuffer
 */
static teo67_EventInfo *eo67_pInfo = NULL;

/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/
/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/

static teo67_Ring *eo67_Eventing = NULL;

/*===========================================================================*
 *  PUBLIC FUNCTIONS (CODE)                                                  *
 *===========================================================================*/

/* ==KERN Internal Interfaces ====================================================== */

externC void eo60InsertEvent(const tsp31_event_description &newEvent);

/* ----------------------------------- */

externC void eo67InsertStandbyEvent()
{
    tsp31_event_description Event;
    SAPDB_MemFillNoCheck(&Event, 0, sizeof(tsp31_event_description));

    // this initialization is done as found in a37resultevent function... 
    ASSIGN_EO67(Event.sp31ed_ident,sp31ei_standby);
    ASSIGN_EO67(Event.sp31ed_priority,sp31ep_low);
    sprintf((char *)&(Event.sp31ed_text_value[0]), "" );
	Event.sp31ed_text_len = 0;
    Event.sp31ed_value_1  = 0;
    Event.sp31ed_value_2  = 0;

    vinsert_event( Event );
}

/* ----------------------------------- */

externC void eo67InsertOutOfSessionEvent()
{
    tsp31_event_description Event;
    SAPDB_MemFillNoCheck(&Event, 0, sizeof(tsp31_event_description));

    ASSIGN_EO67(Event.sp31ed_ident,sp31ei_outofsessions);
    ASSIGN_EO67(Event.sp31ed_priority,sp31ep_low);
    sprintf((char *)&(Event.sp31ed_text_value[0]), "Out of sessions" );
	Event.sp31ed_text_len = Event.sp31ed_text_value.length();
    Event.sp31ed_value_1  = MAX_INT4_SP00;
    Event.sp31ed_value_2  = MAX_INT4_SP00;

    vinsert_event( Event );
}

/* ----------------------------------- */

externC void eo67InsertMessageEvent( SAPDB_Int4  msg_no ,
                                     const char *msg_txt )
{
  	tsp31_event_description Event;

    SAPDB_MemFillNoCheck(&Event, 0, sizeof(tsp31_event_description));

    ASSIGN_EO67(Event.sp31ed_ident,sp31ei_error);
    ASSIGN_EO67(Event.sp31ed_priority,sp31ep_high);

    Event.sp31ed_value_1  = msg_no;
    Event.sp31ed_value_2  = MAX_INT4_SP00;

    Event.sp31ed_text_len = (int)strlen(msg_txt) ;
    if ( Event.sp31ed_text_len > sizeof(Event.sp31ed_text_value) )
    {
        Event.sp31ed_text_len = sizeof(Event.sp31ed_text_value);
    }
    SAPDB_MemCopyNoCheck(Event.sp31ed_text_value, msg_txt, Event.sp31ed_text_len ) ;

    vinsert_event(Event);
}

/* ----------------------------------- */

  #undef  MF__
  #define MF__ MOD__"vinsert_event"
externC void vinsert_event(
tsp31_event_description &newEvent)  // event to be inserted
{
#ifdef TEST
    printf("vinsert_event:");
#endif /* TEST */
    if ( eo67_Eventing )
    {
        // implicit lock/unlock
        eo67_Eventing->InsertEvent(newEvent);
    }
    else
    {
        vdattime(newEvent.sp31ed_date, newEvent.sp31ed_time);
        eo60InsertEvent(newEvent);
    }
}

/* ----------------------------------- */

externC void vwait_for_event(
const tsp00_TaskId       pid,           // Task id of event task
tsp31_event_description &returnEvent,   // event to be returned
tsp00_ErrText           &errtext,       // returned error message 
tsp00_Bool              &Ok)            // returned flag for success/failure
{
    tsp00_Int4  taskIdx;
    bool        isValid;
    bool        isWaiting;
#ifdef TEST
    printf("vwait_for_event:");
#endif /* TEST */    
    if ( eo67_Eventing )
    {
        // explicit lock/unlock
        eo67_Eventing->Lock();

            isValid = eo67_Eventing->ValidateTask(pid, true);
            if ( !isValid )
            {
                errtext.c2p("Validation of process ID failed");
                Ok = FALSE;
            }

            isWaiting = true;
            while ( isWaiting )
            {
                // must be inside lock/unlock since idx may change if unlocked...
                taskIdx = eo67_Eventing->SearchTask(pid);
                Ok = ( taskIdx >= 0 ? TRUE : FALSE );
                if ( Ok == TRUE )
                {
                  /* PTS 1105149 */
                  if ( TASK_TO_CANCEL( &(KGS->pFirstTaskCtrl[pid-1]) ) )
                  {
                    errtext.c2p("Eventtask to cancel");
                    Ok = FALSE;
                    TASK_TO_CANCEL( &(KGS->pFirstTaskCtrl[pid-1]) ) = 0;
                    isWaiting = false;
                  }
                  else
                  {
                    errtext.c2p("");
                    if ( eo67_Eventing->GetNextEvent( taskIdx, returnEvent) )
                    {
                        isWaiting = false;
                    }
                    else
                    {
                        eo67_Eventing->Unlock();
                            vstop(pid);
                        eo67_Eventing->Lock();
#                       ifdef TEST
                            break;
#                       endif
                    }
                  }
                }
                else
                {
                    errtext.c2p("Invalid process ID given");
                    isWaiting = false;
                }
            }
        eo67_Eventing->Unlock();
    }
}

/* ==LZU Internal Interfaces ======================================================= */

externC tsp00_Int4 eo67SpaceNeeded( tsp00_Int4 maxNumberOfEventTasks,
                                          tsp00_Int4 maxNumberOfEvents)
{
    tsp00_Int4 space = teo67_Ring::Size(maxNumberOfEventTasks, maxNumberOfEvents);
    return space;
}

/* ----------------------------------- */

externC void eo67InitializeEventInfo( )
{
    /* PTS 1110403 */
    static teo67_EventInfo localEventInfo;
    eo67_pInfo = &localEventInfo;
}

/* ----------------------------------- */

externC bool eo67Initialize( tsp00_Int4 maxNumberOfEventTasks,
                             tsp00_Int4 maxNumberOfEvents )
{
    eo67_Eventing = REINTERPRET_CAST(teo67_Ring *, KGS->Eventing);
    if ( eo67_Eventing )
    {
        eo67_Eventing->Initialize(maxNumberOfEventTasks, maxNumberOfEvents);
        // Initialize pending event handler
        RTE_PendingEventHandler::Instance();
        return true;
    }
    return false;
}

/* ----------------------------------- */

externC bool eo67ConnectEventTask(tsp00_TaskId pid)
{
    // implicit lock/unlock
    return ( eo67_Eventing ? eo67_Eventing->ConnectTask(pid) : false );
}

/* ----------------------------------- */

externC bool eo67ReleaseEventTask(tsp00_TaskId pid)
{
    // implicit lock/unlock
    return ( eo67_Eventing ? eo67_Eventing->ReleaseTask(pid) : false );
}

/* ============================================================
 * teo67_Ring Class implementation
 */

bool teo67_Ring::FindNextEvent(tsp00_Int4 &lastIndex,
                             tsp31_event_description &newEvent)
{
    if ( lastIndex < m_TailIndex )
    {
        tsp00_Int4 currentPos = m_HeadPos;

        do {
            if ( m_DescArray[currentPos].Index() > lastIndex )
            {
                lastIndex = m_DescArray[currentPos].Index();
                newEvent = m_DescArray[currentPos].m_Event;
#ifdef TEST
        DumpRing("Found", currentPos);
#endif /* TEST */
                return true;
            }
            currentPos = ((currentPos + 1)%m_MaxEvents);
        } while ( currentPos != m_TailPos );

        if ( lastIndex < m_TailIndex )
        {
            lastIndex = m_TailIndex;
            newEvent = m_DescArray[m_TailPos].m_Event;
#ifdef TEST
        DumpRing("Found", m_TailPos);
#endif /* TEST */
            return true;
        }
    }

    return false;
}

/* ----------------------------------- */

void teo67_Ring::UpdateHead()
{
    tsp00_Int4 taskIdx;

    if ( m_TotalStored > 0 )
    {
        for ( taskIdx = 0; taskIdx < m_TotalConnected; taskIdx++ )
        {
            if ( m_TaskArray[taskIdx].m_lastIndex < m_HeadIndex )
            {
#ifdef TEST
                printf("Task %d Index %d Head %d\n", taskIdx, m_TaskArray[taskIdx].m_lastIndex, m_HeadIndex);
#endif
                return;
            }
        }

        --m_TotalStored;
#ifdef TEST
        DumpRing("UpdateHead", m_HeadPos);
#endif /* TEST */
        m_HeadPos = ((m_HeadPos + 1)%m_MaxEvents);
        m_HeadIndex = m_DescArray[m_HeadPos].Index();
        UpdateHead();
    }
}

/* ----------------------------------- */

bool teo67_Ring::GetNextEvent(
const tsp00_Int4 taskIdx,
tsp31_event_description &newEvent)
{
    bool foundEvent = false;
    teo67_EventTask &currentTask = m_TaskArray[taskIdx];

    if ( m_TotalStored == 0 )
    {
        foundEvent = false;
    }
    else
    {
        foundEvent = FindNextEvent(currentTask.m_lastIndex, newEvent);
#ifdef TEST
        printf("found %s Index %d HeadIndex %d\n",
            foundEvent ? "TRUE" : "FALSE", currentTask.m_lastIndex, m_HeadIndex );
#endif
        if ( foundEvent 
          && (currentTask.m_lastIndex == m_HeadIndex) )
        {
            UpdateHead();
        }
    }

    if ( foundEvent )
    {
        currentTask.m_neverGotEvent = false;
    }
    currentTask.m_isWaiting = !foundEvent;

    return (foundEvent);
}

/* ----------------------------------- */

/* 
 * There is no more space left in ringbuffer. We sacrifice the least priority
 * entry beginning with 'head'...
 *
 * A) DONT_USE_PRIORITY  Search for the oldest AliveEvent or use head element
 * B)      USE_PRIORITY  Search for the oldest AliveEvent or use element with
 *                       lowest priority starting with head element
 * A+B are identical if all elements have the same priority...
 */
void teo67_Ring::HandleOverflow()
{
    tsp00_Int4 victimPos = m_HeadPos;
    tsp00_Int4 currentPos = m_HeadPos;

#ifndef DONT_USE_PRIORITY

    int lowestPrio = m_DescArray[m_HeadPos].Priority();
    currentPos = ((m_HeadPos + 1)%m_MaxEvents);

    /* find least priority victim */
    do {
        if ( lowestPrio > m_DescArray[currentPos].Priority() )
        {
            victimPos = currentPos;
            lowestPrio = m_DescArray[currentPos].Priority();
        }

        currentPos = ((currentPos + 1)%m_MaxEvents);

    } while ( currentPos != m_HeadPos );

#endif /* DONT_USE_PRIORITY */

#ifdef TEST
    DumpRing("Victim", victimPos);
#endif /* TEST */

    /* Shift all element from head up to victim one position */
    currentPos = victimPos;
    tsp00_Int4 previousPos = ((currentPos + m_MaxEvents - 1)%m_MaxEvents);
    while ( currentPos != m_HeadPos )
    {
        m_DescArray[currentPos] = m_DescArray[previousPos];
        currentPos = previousPos;
        previousPos = ((currentPos + m_MaxEvents - 1)%m_MaxEvents);
    }

    /* Head Position update */
    m_HeadPos = ((m_HeadPos + 1)%m_MaxEvents);
    m_HeadIndex = m_DescArray[m_HeadPos].Index();
    --m_TotalStored;
}

/* ----------------------------------- */

#ifdef TEST
void teo67_Ring::DumpRing(char *action, tsp00_Int4 cursor)
{
    tsp00_Int4 i;
    printf("%s: Total %d Head Pos %d Index %d Tail Pos %d Index %d\n", 
        action, m_TotalStored, m_HeadPos, m_HeadIndex, m_TailPos, m_TailIndex );
    for ( i = 0; i < m_MaxEvents; i++ )
    {
        printf("Pos %d%c:Prio %d Index %d\n", 
            i, ( i==cursor ? '!' : ' ' ), m_DescArray[i].Priority(), m_DescArray[i].Index() );
    }

}
#endif /* TEST */
/* ----------------------------------- */

void teo67_Ring::InsertEvent(tsp31_event_description &newEvent)
{
    tsp00_Int4 taskIdx;

    Lock();
        newEvent.sp31ed_eventcnt = ++m_TailIndex;

        vdattime(newEvent.sp31ed_date, newEvent.sp31ed_time);

        eo60InsertEvent(newEvent);

        if ( m_TotalConnected > 0 )
        {
            if ( m_TotalStored >= m_MaxEvents )
            {
                /* Make more space for event... */
                HandleOverflow();
            }

            if ( m_TotalStored == 0 )
            {
                m_HeadPos = m_TailPos;
                m_HeadIndex = m_TailIndex;
            }

            ++m_TotalStored;
            m_DescArray[m_TailPos].m_Event = newEvent;
#ifdef TEST
    DumpRing("Insert", m_TailPos);
#endif /* TEST */
            m_TailPos = ((m_TailPos + 1)%m_MaxEvents);

            for ( taskIdx = 0; taskIdx < m_TotalConnected; taskIdx++ )
            {
                if ( m_TaskArray[taskIdx].m_isWaiting )
                {
                    m_TaskArray[taskIdx].m_isWaiting = false;
                    vcontinue(m_TaskArray[taskIdx].m_pid);
                }
            }
        }
    Unlock();
}

/* ----------------------------------- */

bool teo67_Ring::ConnectTask(tsp00_TaskId pid)
{
    bool connectResult;

    Lock();
        connectResult = ValidateTask(pid, false);

        if ( connectResult )
        {
            if ( m_TotalConnected == m_MaxEventTasks )
            {
                connectResult = false;
            }
            else
            {
                m_TaskArray[m_TotalConnected].Init(pid);

                m_TaskArray[m_TotalConnected].m_lastIndex = m_TailIndex;

                m_TotalConnected++;
            }
        }
    Unlock();

    return connectResult;
}

/* ============================================================
 * ceo67_Base Class implementation
 */
bool ceo67_Base::Initialize( tsp00_Int4 maxNumberOfEventTasks,
                           tsp00_Int4 maxNumberOfEvents )
{
    if ( m_TaskArray )
    {
        return false;
    }

    tsp00_Byte *pSpace = REINTERPRET_CAST(tsp00_Byte *,this);

    m_TaskArray = REINTERPRET_CAST(teo67_EventTask*,
        pSpace + ALIGN_8BYTE_EO00( sizeof(teo67_Ring) ) );

    m_DescArray = REINTERPRET_CAST(teo67_EventDescription *,
        pSpace + ALIGN_8BYTE_EO00( sizeof(teo67_Ring) )
               + ALIGN_8BYTE_EO00( maxNumberOfEventTasks * sizeof(teo67_EventTask) ) );

    m_MaxEventTasks = maxNumberOfEventTasks;
    m_MaxEvents = maxNumberOfEvents;
    m_TotalConnected = 0;
#ifdef WIN32
    m_Exclusive = &m_ExclusiveValue;
#endif /* WIN32 */
    INIT_LOCK(m_Exclusive);

    return true;
}

/* ----------------------------------- */

bool ceo67_Base::ReleaseTask(tsp00_TaskId pid)
{
    bool releaseResult;
    Lock();
        releaseResult = ValidateTask(pid, true);

        if ( releaseResult )
        {
            for ( tsp00_Int4 taskIdx = 0; taskIdx < m_TotalConnected; taskIdx++ )
            {
                if ( !m_TaskArray[taskIdx].m_inUse )
                {
                    releaseResult = false;
                    break;
                }

                if ( m_TaskArray[taskIdx].m_pid == pid )
                {
                    if ( m_TaskArray[taskIdx].m_isWaiting )
                    {
                        vcontinue(pid);
                    }

                    m_TotalConnected--;
    
                    // Compact m_TaskArray Array:
                    // copy only if not last element...
                    if ( taskIdx != (m_TotalConnected+1) )
                    {
                        m_TaskArray[taskIdx] = m_TaskArray[m_TotalConnected];
                    }

                    m_TaskArray[m_TotalConnected].Reset();
                }
            }
        }
    Unlock();

    return releaseResult;
}

/* ----------------------------------- */

tsp00_Int4 ceo67_Base::SearchTask(tsp00_TaskId pid)
{
    tsp00_Int4 returnIdx = -1;
    tsp00_Int4 taskIdx;

    for ( taskIdx = 0; taskIdx < m_TotalConnected; taskIdx++ )
    {
        if ( !m_TaskArray[taskIdx].m_inUse )
        {
            break;
        }

        if ( m_TaskArray[taskIdx].m_pid == pid )
        {
            returnIdx = taskIdx;
            break;
        }
    }
    return returnIdx;
}

/* ----------------------------------- */

bool ceo67_Base::ValidateTask(tsp00_TaskId pid, bool isAdded)
{
    /* Using errors would produce a deadlock here... */
    if ( pid < 1 || ((unsigned)pid) > KGS->ulNumOfTasks )
    {
        MSGCD (( INFO_EVENTING_BAD_PID, pid ));
        return false;
    }

    if ( TASK_TYPE(KGS->pFirstTaskCtrl + pid - 1) != TT_EV_EO00 )
    {
        MSGCD (( INFO_EVENTING_WRONG_TASK_TYPE, TASK_TYPE(KGS->pFirstTaskCtrl + pid - 1) ));
        return false;
    }

    if ( isAdded )
    {
        if ( SearchTask(pid) < 0 )
        {
            MSGCD (( INFO_EVENTING_TASK_MISSING, pid ));
            return false;
        }
    }
    else
    {
        if ( SearchTask(pid) >= 0 )
        {
            MSGCD (( INFO_EVENTING_TASK_EXISTS, pid ));
            return false;
        }
    }
    return true;
}

/*===========================================================================*
 *  LOCAL/PRIVATE/PROTECTED FUNCTIONS (CODE)                                 *
 *===========================================================================*/

#ifdef TEST
#include <stdio.h>

externC BOOL sql76_interlock ( LONG * plLockAddr )
{
    *plLockAddr = 1;
    return false;
}

externC void sql76_clearlock ( LONG * plLockAddr )
{
    *plLockAddr = 0;
}


void vdattime(tsp00_Date &date, tsp00_Time &time)
{
    static int my_time = 90420;
    char time_stamp[9];
    sprintf(time_stamp, "%08d", my_time++);

    date.c2p("19990428");
    time.c2p(time_stamp);
}

externC void vcontinue(tsp00_TaskId pid)
{
    printf("Continue task %d\n", pid);
}

externC void vstop(tsp00_TaskId pid)
{
    printf("Stopping task %d\n", pid);
}

#define SPRINT sprintf

externC void eo60InsertEvent(const tsp31_event_description &newEvent)
{
#ifdef TEST_OUTPUT
      /* add some extra space for ascii conversion */
	  char EventTxt [ sizeof(tsp31_event_description) + 50] ;
	  char EventTxtFormat [ 50 ] ;
	  int EventTxtLen, FieldValueLen ;

      EventTxtLen = SPRINTF (EventTxt, "%8.8s %8.8s %10u %2u %1u ",
                                       &newEvent.sp31ed_date,
									   &newEvent.sp31ed_time,
                                       newEvent.sp31ed_eventcnt,
									   newEvent.sp31ed_ident,
                   STATIC_CAST( tsp31_event_prio_Enum, newEvent.sp31ed_priority) );

      if ( newEvent.sp31ed_value_1 != MAX_INT4_SP00 )
      {
		    FieldValueLen = SPRINTF( EventTxt + EventTxtLen, "%10u ", newEvent.sp31ed_value_1);
      }
      else
      {
		    FieldValueLen = SPRINTF( EventTxt + EventTxtLen, "%10.10s ", "");
      }
	  EventTxtLen += FieldValueLen ;                       			

      if ( newEvent.sp31ed_value_2 != MAX_INT4_SP00 )
      {
		    FieldValueLen = SPRINTF( EventTxt + EventTxtLen, "%10u ", newEvent.sp31ed_value_2);
      }
      else
      {
		    FieldValueLen = SPRINTF( EventTxt + EventTxtLen, "%10.10s ", "");
      }
	  EventTxtLen += FieldValueLen ;                       			

      /* restrict length of string output (not ZERO terminated pascal string was input... */
      SPRINTF( EventTxtFormat,"%%%d.%ds", newEvent.sp31ed_text_len, newEvent.sp31ed_text_len);
      EventTxtLen += SPRINTF( EventTxt + EventTxtLen, EventTxtFormat, &newEvent.sp31ed_text_value );

      printf( "%s\n", EventTxt );
#endif
}

externC void sql60_msg_con_diag ( LONG           lMsgID,
                                  ULONG          ulEventType,
                                  PSZ            pszFormatStr, ...)
{
  #undef  MF__
  #define MF__ MOD__"sql60_msg_con_diag"
  va_list     args;

  va_start ( args, pszFormatStr);
  printf("MessageID %d Type %d ", lMsgID, ulEventType);
  vprintf(pszFormatStr, args);
  printf("\n");
  va_end ( args );

  return;
  }


externC void sp36_c2p (
    void * instanceData,
    int    instanceSize,
    const char * str)
{
    ROUTINE_DBG_MSP00 ("sp36_c2p");
    int len = strlen (str);

    if (len > instanceSize)
        len = instanceSize;
    SAPDB_MemCopyNoCheck (instanceData, str, len);
    if (len < instanceSize)
        SAPDB_MemFillNoCheck ((char *) instanceData + len, ' ', instanceSize - len);
}

TASK_CTRL_REC     tasks[3];
KERNEL_GLOBAL_SEC kgs;

int main( int argc, char *argv)
{
    tsp31_event_description event_in;
    tsp31_event_description event_out;
    tsp00_ErrText errtext;
    tsp00_Bool    ok;
    tsp00_TaskId  pid = 2;
    tsp00_TaskId  pid2 = 1;
    tsp00_Int4    size;

    KGS->ulNumOfTasks = 3;
    KGS->pFirstTaskCtrl = tasks;
    KGS->Eventing = NULL;
    TASK_TYPE(&tasks[pid-1]) = TT_EV_EO00;
    TASK_TYPE(&tasks[pid2-1]) = TT_EV_EO00;

    size = eo67SpaceNeeded(2,3);
    KGS->Eventing = malloc(size);
    SAPDB_MemFillNoCheck(KGS->Eventing, 0, size);

    eo67Initialize(2,3);

    eo67ConnectEventTask(pid);

    vwait_for_event(pid, event_out, errtext, ok);

    vwait_for_event(pid, event_out, errtext, ok);

    SAPDB_MemFillNoCheck(&event_in, 0, sizeof(event_in));
    ASSIGN_EO67(event_in.sp31ed_ident,sp31ei_error);
    ASSIGN_EO67(event_in.sp31ed_priority,sp31ep_high);
    event_in.sp31ed_value_1 = 4711;
    event_in.sp31ed_value_2 = MAX_INT4_SP00;

    vinsert_event(event_in);

    vwait_for_event(pid, event_out, errtext, ok);

    event_in.sp31ed_value_1 = 4712;
    vinsert_event(event_in);
    event_in.sp31ed_value_1 = 4713;
    vinsert_event(event_in);

    eo67ConnectEventTask(pid2);

    event_in.sp31ed_value_1 = 4714;
    ASSIGN_EO67(event_in.sp31ed_priority,sp31ep_low);
    vinsert_event(event_in);

    event_in.sp31ed_value_1 = 4715;
    vinsert_event(event_in);

    vwait_for_event(pid, event_out, errtext, ok);

    vwait_for_event(pid, event_out, errtext, ok);

    event_in.sp31ed_value_1 = 4716;
    vinsert_event(event_in);

    vwait_for_event(pid, event_out, errtext, ok);

    vwait_for_event(pid, event_out, errtext, ok);

    vwait_for_event(pid2, event_out, errtext, ok);

    return 0;
}
#endif /* TEST */
/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
