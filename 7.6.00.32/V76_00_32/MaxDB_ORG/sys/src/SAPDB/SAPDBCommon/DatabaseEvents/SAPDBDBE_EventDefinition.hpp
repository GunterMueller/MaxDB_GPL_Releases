/*!
  \file    SAPDBDBE_EventDefinition.hpp
  \author  MarcW
  \ingroup Database Events Management
  \brief   

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


*/

#if !defined(SAPDBDBE_EventDefinition_hpp)
#define SAPDBDBE_EventDefinition_hpp

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "vsp0031.h"

/*!
    @brief descrption for an event definition

    An event definition consists of a value and a priority. To facilitate iteration on a "merged" event
    category, this event definition class also provides means to get the status of an event definition
    (added, modified, removed).
*/
class SAPDBDBE_EventDefinition {
public:

    /*! @brief the event stati */
    enum EventDefinitionStatus {
        EDS_Added,        /*!< comparing the new to the old event category value, this event was added */
        EDS_Modified,     /*!< comparing the new to the old event category value, this event's priority was modified */
        EDS_Removed       /*!< comparing the new to the old event category value, this event was removed */
    };

    /*!
        @brief constructor

        Creates an event definition
        @param aStatus [IN] status of this event definition
        @param aPriority [IN] priority of this event definition
        @param aValue [IN] value of this event definition
    */
    SAPDBDBE_EventDefinition(
            const EventDefinitionStatus aStatus,
            const tsp31_event_prio_Enum aPriority,
            const SAPDB_Int4 aValue);

    /*! @brief empty destructor */
    ~SAPDBDBE_EventDefinition() {};

    /*!
        @brief get this event definition's status
        @return this event definition's status
    */
    EventDefinitionStatus getStatus() const;

    /*!
        @brief get this event definition's priority
        @return this event definition's priority
    */
    tsp31_event_prio_Enum getPriority() const;
    
    /*!
        @brief get this event definition's value
        @return this event definition's value
    */
    SAPDB_Int4 getValue() const;

    /*!
        @brief set this event definition's status
        @param newStatus [IN] new status
    */
    void setStatus(EventDefinitionStatus newStatus);

    /*!
        @brief set this event definition's priority
        @param newPriority [IN] new priority
    */
    void setPriority(tsp31_event_prio_Enum newPriority);

private:
    EventDefinitionStatus m_Status;
    tsp31_event_prio_Enum m_Priority;
    SAPDB_Int4 m_Value;
};

inline SAPDBDBE_EventDefinition::SAPDBDBE_EventDefinition(
        const SAPDBDBE_EventDefinition::EventDefinitionStatus aStatus,
        const tsp31_event_prio_Enum aPriority,
        const SAPDB_Int4 aValue)
        : m_Status(aStatus),
            m_Priority(aPriority),
            m_Value(aValue) {}

inline void SAPDBDBE_EventDefinition::setStatus(SAPDBDBE_EventDefinition::EventDefinitionStatus newStatus){
    m_Status = newStatus;
}

inline void SAPDBDBE_EventDefinition::setPriority(tsp31_event_prio_Enum newPriority) {
    m_Priority = newPriority;
}

inline SAPDBDBE_EventDefinition::EventDefinitionStatus SAPDBDBE_EventDefinition::getStatus() const {
    return m_Status;
}

inline tsp31_event_prio_Enum SAPDBDBE_EventDefinition::getPriority() const {
    return m_Priority;
}

inline SAPDB_Int4 SAPDBDBE_EventDefinition::getValue() const {
    return m_Value;
}

#endif //SAPDBDBE_EventDefinition_hpp
