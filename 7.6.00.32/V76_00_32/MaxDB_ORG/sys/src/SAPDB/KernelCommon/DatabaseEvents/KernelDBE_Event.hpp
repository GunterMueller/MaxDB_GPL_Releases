/*!
  @file           KernelDBE_Event.hpp
  @author         Alexander
  @responsible    UweH
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

#ifndef KERNELDBE_EVENTSET_HPP
#define KERNELDBE_EVENTSET_HPP

#include "vsp0031.h"
#include "SAPDBCommon/SAPDB_Types.hpp"

/// more events are not definable
#define MAX_NUM_EVENTS              101
/// This is the threshold for reactivation of an already triggered event.
#define SINGLE_VALUE_ACTIVATE_DIFF  0.25  // in % 
/// just to have no magic number in the code
#define HUNDERT_PERCENT             100

/*!
    @class KernelDBE_EventSet
	@brief Using this class one can watch a parameter.

    Whenever the method Monitor is called it is checked whether the parameter passed predefined values.
	If this is the case an event is triggered which can be writing a message into the knldiag
    or inserting an event into the event queue.
 */
class KernelDBE_EventSet
{	
public:  // methods

	/*!
    @brief          adds an event to the set of events
    @param          RelativeThreshold [in] critical value in 0.01 percent with respect to the basis unit defined 
                    in the constructor or via rescale. If this value is passed by the parameter to watch and 
                    bOverflow is true an event is triggered, i.e. an event is  inserted
                    into the event queue, if bReportAlwaysIntoKnldiag is true an event is always
                    written to the knldiag too, even is the eventing is disabled
    @param          bOverflow [in] defines if an event should be triggerd of the value to be monitored exceeds
                    or falls below the threshold
    @param          Priority [in] priority of the event
    @param          bWriteAlwaysIntoKnldiag [in] if this is true the event is always reported in the knldiag 
                    even if the eventing is disabled.
    @return         (SAPDB_Bool) true if the event set is not empty

    - With this function an event can be defined which is triggered when by calling the watch-function it is realized that the input value has passed  the given threshold
    */
	SAPDB_Bool AddEvent      (const SAPDB_Int                RelativeThreshold, 
		                      const SAPDB_Bool               bOverflow, 
							  const tsp31_event_prio_Param   Priority,
							  const SAPDB_Bool               bWriteAlwaysIntoKnldiag);
	/*!
    @brief removes an event from the set of events
    @param RelativeThreshold [in] defines the critical value to be removed. The value is 
           given in units of 0.01 percent with respect to the base unit
    @param bOverflow [in] displays whether the event of exceeding or falling below is to remove
    @return (SAPDB_Bool) true if the event set is not empty
  */

	SAPDB_Bool DelEvent      (const SAPDB_Int    RelativeThreshold,
		                      const SAPDB_Bool   bOverflow);
	/*!
    @brief monitors a parameter
    @param Value [in] value (given in its absolut size) which is to watch
    @param onlyNextEvent [in] default = true if this is false all defined values
           which are greater are watched, else not.

    Whenever this function is cold it is checked whether the parameter has passed
    any of the thresholds defined in the event set since the command was called the last time.
    If this is the case the events corresponding to the passed values are triggered.
  */
	void Watch (const SAPDB_Int Value,
                const bool      onlyNextEvent = true)
	{	
		if (Value > m_LastValue)
        {
            if ( onlyNextEvent )
                LookForNextOverFlowEvent(Value);
            else
                LookForOverFlowEvents(Value);
		}
        else
            if (Value < m_LastValue)
            {
                if ( onlyNextEvent )
                    LookForNextUnderFlowEvent(Value);
                else
                    LookForUnderFlowEvents(Value);
            }
		m_LastValue = Value;
	}
	/*!
    @brief  disables eventing

    - After the eventing is disabled, no events are triggered but passing critical values        is reported into the knldiag file for those critical values, for which this was explicitely ordered (see AddEvent)
    */
    void DisableEventing()
	{
		m_EventReportingEnabled = false;
	}
	/// enables eventing,i.e each time a critical value is passed an event is triggered
	void EnableEventing()
	{
		m_EventReportingEnabled = true;
	}
	/*!
    @brief          rescales all event thresholds with repsect to the new base unit
    @param          NewBaseValue [in] new base unit which defines the unit for all relative thresholds
    */
    void Rescale (SAPDB_UInt    NewBaseValue);
	/*!
    @brief          constructor
    @param          OverFlowEventId [in] id of the event when the monitored parameter exceeds a critical value
    @param          UnderFlowEventId [in] id of the event when the monitored parameter falls below a critical value
    @param          BaseValue [in] all events added to the set must define their critical value
                                                        in percent with repect to this unit
    */
	KernelDBE_EventSet(
                     const tsp31_event_ident_Param  OverFlowEventId, 
	        	     const tsp31_event_ident_Param  UnderFlowEventId,
					 SAPDB_UInt                     BaseValue = 100);

private: // types

    struct EventDefinition
    {
        SAPDB_Int        m_AbsolutThreshold;   
        SAPDB_Int        m_RelativeThreshold;   
        tsp31_event_prio m_OverFlowPriority;
        tsp31_event_prio m_UnderFlowPriority;
        SAPDB_Bool       m_ReportOverFlow;
        SAPDB_Bool       m_ReportUnderFlow;
        SAPDB_Bool       m_WriteAlwaysIntoKnldiag;
    
        EventDefinition()
        :m_AbsolutThreshold      (0),
         m_RelativeThreshold     (0),
         m_ReportOverFlow        (false),
         m_ReportUnderFlow       (false),
         m_WriteAlwaysIntoKnldiag(false)
        {
            m_OverFlowPriority.becomes(sp31ep_nil),
            m_UnderFlowPriority.becomes(sp31ep_nil);
        }
    };

private: // methods
    SAPDB_Bool AddEmptyEvent         (const SAPDB_Int   Index,
                                      const SAPDB_Int   RelativeThreshold,
                                      const SAPDB_Bool  bWriteAlwaysIntoKnldiag); // PTS 1124743 UH 2003-10-21
	SAPDB_Bool DelEventIfItIsEmpty   (const SAPDB_Int   Index);
	SAPDB_Int  FindEventIndex        (const SAPDB_Int   RelativeThreshold, 
                                            SAPDB_Bool &bFound);
	void LookForOverFlowEvents        (const SAPDB_Int   Value);
	void LookForUnderFlowEvents       (const SAPDB_Int   Value);
	void LookForNextOverFlowEvent     (const SAPDB_Int   Value);
	void LookForNextUnderFlowEvent    (const SAPDB_Int   Value);
	void ReportEvent                  (const tsp31_event_ident_Param  EventId, 
                                       const tsp31_event_prio_Param   EventPriority, 
                                       const EventDefinition         &Event);
	
private: 

	SAPDB_Int               m_IndexOfFirstActiveEvent;
	SAPDB_Int               m_LastValue;               
	SAPDB_Int               m_BaseValue;
	SAPDB_Int               m_NumEvents;
	SAPDB_Int               m_NumSingleEvents;
	SAPDB_Int               m_ReActivateEventDiff;    
	SAPDB_Int               m_UnitOfRelativeThreshold;
	SAPDB_Bool              m_EventReportingEnabled;
	tsp31_event_ident       m_OverFlowSingleEventId; 
	tsp31_event_ident       m_UnderFlowSingleEventId;
	EventDefinition         m_Event  [MAX_NUM_EVENTS - 1];
};

#endif  /* KERNELDBE_EVENTSET_HPP */
