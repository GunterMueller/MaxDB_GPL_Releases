/*!
  @file           KernelDBE_Event.hpp
  @author         Alexander
  @@responsible   UweH
  @ingroup        Eventing
  @brief          defines a event handler for an easy access to the runtime function

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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

#include "heo60.h"
#include "hgg01_2.h"
#include "KernelCommon/DatabaseEvents/KernelDBE_Event.hpp"
#include "KernelCommon/DatabaseEvents/KernelDBE_Messages.hpp"  
#include "RunTime/RTE_Message.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp"
#include "SAPDBCommon/SAPDB_ToString.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

SAPDBERR_DEFINE_COMPONENT_EXCEPTION_CLASS(KernelDBE_Event);

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

SAPDB_Bool 
KernelDBE_EventSet::AddEvent(
                             const SAPDB_Int              RelativeThreshold,
                             const SAPDB_Bool             bOverflow,
                             const tsp31_event_prio_Param Priority,
                             const SAPDB_Bool             bWriteAlwaysIntoKnldiag)
{
    ROUTINE_DBG_MSP00 ("KernelDBE_EventSet::AddEvent");
    
    // find position of the new treshold
    SAPDB_Bool bFound;
    SAPDB_Int  Index = FindEventIndex (RelativeThreshold, bFound);
    
    if (!bFound)
        if (! AddEmptyEvent (Index, RelativeThreshold, bWriteAlwaysIntoKnldiag)) // PTS 1124743 UH 2003-10-21
            return (m_NumEvents>0);

    // set the priority and the purpose of the new event
    if (bOverflow)
    {  
        m_Event [Index].m_ReportOverFlow    = true;
        m_Event [Index].m_OverFlowPriority.becomes(Priority);
    }
    else
    {
        m_Event [Index].m_ReportUnderFlow   = true;
        m_Event [Index].m_UnderFlowPriority.becomes(Priority);
    }

    const SAPDB_Real8 auxAbsolute             = SAPDB_Real8( RelativeThreshold ) * SAPDB_Real8( m_BaseValue );
    m_Event [Index].m_AbsolutThreshold        = SAPDB_Int( auxAbsolute / m_UnitOfRelativeThreshold );
    m_Event [Index].m_WriteAlwaysIntoKnldiag |= bWriteAlwaysIntoKnldiag;
    m_NumSingleEvents ++;

    return (m_NumEvents>0);
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
KernelDBE_EventSet::DelEvent (
                              const SAPDB_Int   RelativeThreshold,
                              const SAPDB_Bool  bOverflow)
{
    ROUTINE_DBG_MSP00 ("KernelDBE_EventSet::DelEvent");
    
    // check if there exists any event at all
    if (0 != m_NumEvents) 
    {       
        // find position of the threshold
        SAPDB_Bool bFound; 
        const SAPDB_Int  Index = FindEventIndex (RelativeThreshold, bFound);
        
        if  (bFound)
        {   
            if (bOverflow)
                m_Event [Index].m_ReportOverFlow  = false;
            else
                m_Event [Index].m_ReportUnderFlow = false;

            -- m_NumSingleEvents;
            
            // remove Event if neither the overflow nor the underflow 
            // event should be observed
            DelEventIfItIsEmpty (Index);
        }
    }
    return (true);
}

/*---------------------------------------------------------------------------*/

void 
KernelDBE_EventSet::Rescale (SAPDB_UInt    NewBaseValue)
{
   ROUTINE_DBG_MSP00 ("KernelDBE_EventSet::Rescale");

   m_BaseValue = NewBaseValue;

   for (SAPDB_Int iEvent=0; iEvent<m_NumEvents; ++iEvent)
   {
       const SAPDB_Real8 auxAbsolute = SAPDB_Real8( m_Event [iEvent].m_RelativeThreshold ) * SAPDB_Real8( NewBaseValue );
       m_Event [iEvent].m_AbsolutThreshold = SAPDB_Int( auxAbsolute / m_UnitOfRelativeThreshold );
   }

   for (m_IndexOfFirstActiveEvent = 0; 
        m_IndexOfFirstActiveEvent < m_NumEvents;
        m_IndexOfFirstActiveEvent ++)
    {
            if (m_Event [m_IndexOfFirstActiveEvent].m_AbsolutThreshold >  m_LastValue)
                break;
    }
}


/*===========================================================================*
 *  private methods                                                          *
 *===========================================================================*/

SAPDB_Int
KernelDBE_EventSet::FindEventIndex (
                                    const SAPDB_Int   RelativeThreshold, 
                                    SAPDB_Bool       &bFound)
{   
    ROUTINE_DBG_MSP00 ("KernelDBE_EventSet::FindEventIndex");
    
    if (0 == m_NumEvents) 
    {
        bFound = false;
        return 0;
    }
    else
    {
        if (RelativeThreshold > m_Event [m_NumEvents-1].m_RelativeThreshold)
        {
            bFound = false;
            return m_NumEvents;
        }
        else 
        {
            if (RelativeThreshold < m_Event [0].m_RelativeThreshold)
            {
                bFound = false;
                return 0;
            }
            else
            {
                SAPDB_Int MaxIndex  = m_NumEvents - 1;
                SAPDB_Int MinIndex  = 0;
                SAPDB_Int DiffIndex = MaxIndex - MinIndex;
                SAPDB_Int CurrIndex;
                
                while (DiffIndex > 1)
                {
                    CurrIndex = (MaxIndex + MinIndex)/2;
                    if (m_Event [CurrIndex].m_RelativeThreshold == RelativeThreshold)
                    {
                        bFound = true;
                        return CurrIndex;
                    }
                    else if (m_Event [CurrIndex].m_RelativeThreshold > RelativeThreshold) 
                        MaxIndex = CurrIndex;
                    else
                        MinIndex = CurrIndex;
                    
                    DiffIndex = MaxIndex - MinIndex;
                }
                
                if (RelativeThreshold == m_Event [MinIndex].m_RelativeThreshold)
                {
                    bFound = true;
                    return MinIndex;
                } 
                else
                {
                    if (RelativeThreshold == m_Event [MaxIndex].m_RelativeThreshold)
                        bFound = true;
                    else
                        bFound = false;
                    return MaxIndex;
                }
            }
        }
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool 
KernelDBE_EventSet::AddEmptyEvent (
                                   const SAPDB_Int  Index, 
                                   const SAPDB_Int  RelativeThreshold,
                                   const SAPDB_Bool bWriteAlwaysIntoKnldiag) // PTS 1124743 UH 2003-10-21
{
    ROUTINE_DBG_MSP00 ("KernelDBE_EventSet::AddEmptyEvent");
    
    // create an new entry in the threshold list
    
    // check if there are not too many m_Events     
    if (m_NumEvents >= MAX_NUM_EVENTS) 
        return false;
    
    // update index of the last m_Event
    if (Index < m_IndexOfFirstActiveEvent)
        m_IndexOfFirstActiveEvent ++;
    
    // add space for new m_Event
    for (SAPDB_Int i = m_NumEvents - 1; i >= Index; i--)
        m_Event [i+1] = m_Event [i];
    
    // insert new m_Event
    m_Event [Index]                          = EventDefinition();       // PTS 1124743 UH 2003-10-21
    m_Event [Index].m_RelativeThreshold      = RelativeThreshold;
    m_Event [Index].m_WriteAlwaysIntoKnldiag = bWriteAlwaysIntoKnldiag; // PTS 1124743 UH 2003-10-21
    
    // increase number of Events
    m_NumEvents ++;
    
    //return error status 
    return true;
}

/*---------------------------------------------------------------------------*/

void
KernelDBE_EventSet::ReportEvent (
                                 const tsp31_event_ident_Param  EventId,
                                 const tsp31_event_prio_Param   EventPriority,
                                 const EventDefinition         &Event)
{
    ROUTINE_DBG_MSP00 ("ReportEvent");
    
    if (m_EventReportingEnabled)
    {
        tsp31_event_description EventDescription;
        g01event_init (EventDescription);   
        EventDescription.sp31ed_priority.becomes(EventPriority);
        EventDescription.sp31ed_ident.becomes(EventId);
        EventDescription.sp31ed_text_len = 0;
        EventDescription.sp31ed_eventcnt = 0;
        EventDescription.sp31ed_value_1  = Event.m_AbsolutThreshold;
        EventDescription.sp31ed_value_2  = (Event.m_RelativeThreshold*HUNDERT_PERCENT)/m_UnitOfRelativeThreshold;  
        
        vinsert_event (EventDescription);
    }
    
    if( ! Event.m_WriteAlwaysIntoKnldiag )
        return;
    
    switch (EventId)
    {
    case sp31ei_db_filling_above_limit:
        {          
            if( sp31ep_high == Event.m_OverFlowPriority )
            { // write message with warning label
                KernelDBE_Event_Exception dataBaseOverflow(__CONTEXT__, KERNELDBE_DB_FULL_WARNING, 
                    SAPDB_ToString ((Event.m_RelativeThreshold*HUNDERT_PERCENT)/m_UnitOfRelativeThreshold, _T_d),
                    SAPDB_ToString (m_BaseValue - Event.m_AbsolutThreshold, _T_d));
                
                RTE_Message (dataBaseOverflow);
                dataBaseOverflow.ClearMessageList();
            }
            else
            { // write message with info label
                KernelDBE_Event_Exception dataBaseOverflow(__CONTEXT__, KERNELDBE_DB_FULL_INFO, 
                    SAPDB_ToString ((Event.m_RelativeThreshold*HUNDERT_PERCENT)/m_UnitOfRelativeThreshold, _T_d),
                    SAPDB_ToString (m_BaseValue - Event.m_AbsolutThreshold, _T_d));
                
                RTE_Message (dataBaseOverflow);
                dataBaseOverflow.ClearMessageList();
            }
        }
        break;
        
    case sp31ei_db_filling_below_limit:
        {
            KernelDBE_Event_Exception dataBaseUnderflow(__CONTEXT__, KERNELDBE_DB_UNDERFLOW_INFO, 
                SAPDB_ToString ((Event.m_RelativeThreshold*HUNDERT_PERCENT)/m_UnitOfRelativeThreshold, _T_d),
                SAPDB_ToString (m_BaseValue - Event.m_AbsolutThreshold, _T_d));
            
            RTE_Message (dataBaseUnderflow);
            dataBaseUnderflow.ClearMessageList();
        }
        break;
        
    case sp31ei_log_above_limit: // PTS 1124876 UH 2003-10-23 added
        {
            if ( Event.m_OverFlowPriority == sp31ep_low )
            {
                
                KernelDBE_Event_Exception logAbove(__CONTEXT__, KERNELDBE_LOG_FULL_INFO, 
                    SAPDB_ToString ((Event.m_RelativeThreshold*HUNDERT_PERCENT)/m_UnitOfRelativeThreshold),
                    SAPDB_ToString (Event.m_AbsolutThreshold));
                
                RTE_Message( logAbove );
                logAbove.ClearMessageList();
            }
            else
            {
                KernelDBE_Event_Exception logAbove(__CONTEXT__, KERNELDBE_LOG_FULL_WARNING, 
                    SAPDB_ToString ((Event.m_RelativeThreshold*HUNDERT_PERCENT)/m_UnitOfRelativeThreshold),
                    SAPDB_ToString (Event.m_AbsolutThreshold));
                
                RTE_Message( logAbove );
                logAbove.ClearMessageList();
            }
        }
        break;
    }
};

/*---------------------------------------------------------------------------*/

SAPDB_Bool
KernelDBE_EventSet::DelEventIfItIsEmpty (const SAPDB_Int Index)
{
    ROUTINE_DBG_MSP00 ("KernelDBE_EventSet::DelEventIfItIsEmpty");
    
    if (! m_Event [Index].m_ReportOverFlow  && 
        ! m_Event [Index].m_ReportUnderFlow)
    {
        // update index of the last Event
        if (Index < m_IndexOfFirstActiveEvent)
            m_IndexOfFirstActiveEvent--;
        
        for (SAPDB_Int i = Index; i < m_NumEvents - 1; i++)
            m_Event [i] = m_Event [i+1];        
        
        // decrease number of events
        m_NumEvents --; 

        // Event was successfully removed
        return true;
    }

    // Event was not removed 
    return false;
}

/*---------------------------------------------------------------------------*/

KernelDBE_EventSet::KernelDBE_EventSet (
                                        const tsp31_event_ident_Param  OverFlowEventId, 
                                        const tsp31_event_ident_Param  UnderFlowEventId,
                                        SAPDB_UInt                     BaseValue)
:
m_NumEvents               (0), 
m_NumSingleEvents         (0),
m_IndexOfFirstActiveEvent (-1),
m_LastValue               (0),
m_UnitOfRelativeThreshold (10000),
m_EventReportingEnabled   (false), 
m_BaseValue               (BaseValue)
{
	m_ReActivateEventDiff  = SAPDB_Int ((m_UnitOfRelativeThreshold*SINGLE_VALUE_ACTIVATE_DIFF)/HUNDERT_PERCENT);
	m_OverFlowSingleEventId.becomes (OverFlowEventId);    
	m_UnderFlowSingleEventId.becomes(UnderFlowEventId);
}

/*---------------------------------------------------------------------------*/

void KernelDBE_EventSet::LookForOverFlowEvents (const SAPDB_Int Value)
{		
    // PTS 1124876 UH 2003-10-23
    // rewritten so that bigger value changes are handled too
    // (use while instead of if so all events are triggered)
	if ( m_IndexOfFirstActiveEvent < 0 )
        return;
    while ( Value > m_Event[m_IndexOfFirstActiveEvent].m_AbsolutThreshold
            &&
            m_IndexOfFirstActiveEvent < m_NumEvents )
	{
		if ( m_Event[m_IndexOfFirstActiveEvent].m_ReportOverFlow )
			ReportEvent ( m_OverFlowSingleEventId,
                          m_Event[m_IndexOfFirstActiveEvent].m_OverFlowPriority,
                          m_Event[m_IndexOfFirstActiveEvent] );
		++ m_IndexOfFirstActiveEvent;
	}
	if ( m_IndexOfFirstActiveEvent >= m_NumEvents )
        m_IndexOfFirstActiveEvent = m_NumEvents - 1;
}

/*---------------------------------------------------------------------------*/

void KernelDBE_EventSet::LookForUnderFlowEvents (const SAPDB_Int Value)
{	
    // PTS 1124876 UH 2003-10-23
    // rewritten so that bigger value changes are handled too
    // (use while instead of if so all events are triggered)
	if ( m_IndexOfFirstActiveEvent < 0 )
        return;
	while ( Value < m_Event[m_IndexOfFirstActiveEvent].m_AbsolutThreshold
            &&
            m_IndexOfFirstActiveEvent >= 0 )
	{
		if ( m_Event[m_IndexOfFirstActiveEvent].m_ReportUnderFlow )
		    ReportEvent (m_UnderFlowSingleEventId,
                         m_Event[m_IndexOfFirstActiveEvent].m_UnderFlowPriority,
                         m_Event[m_IndexOfFirstActiveEvent]);
		--m_IndexOfFirstActiveEvent;
	}
	if ( m_IndexOfFirstActiveEvent < 0 )
        m_IndexOfFirstActiveEvent = 0;
}

/*---------------------------------------------------------------------------*/

void KernelDBE_EventSet::LookForNextOverFlowEvent (const SAPDB_Int Value)
{		
	if (m_IndexOfFirstActiveEvent >= m_NumEvents) return ;
	EventDefinition & CurrEvent = m_Event [m_IndexOfFirstActiveEvent];
	if(Value >  CurrEvent.m_AbsolutThreshold + m_ReActivateEventDiff)
	{
		++ m_IndexOfFirstActiveEvent;
		if (CurrEvent.m_ReportOverFlow )
			ReportEvent (m_OverFlowSingleEventId, CurrEvent.m_OverFlowPriority, CurrEvent);
	}
}

/*---------------------------------------------------------------------------*/

void KernelDBE_EventSet::LookForNextUnderFlowEvent (const SAPDB_Int Value)
{	
	if (0 == m_IndexOfFirstActiveEvent) return;
	const EventDefinition & CurrEvent = m_Event [m_IndexOfFirstActiveEvent - 1];
	if(Value < CurrEvent.m_AbsolutThreshold - m_ReActivateEventDiff)
	{
		--m_IndexOfFirstActiveEvent;
		if (CurrEvent.m_ReportUnderFlow)
		    ReportEvent (m_UnderFlowSingleEventId, CurrEvent.m_UnderFlowPriority, CurrEvent);
	}
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
