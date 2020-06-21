/*!
  \file    SAPDBDBE_EventCategory.cpp
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

#include <stdlib.h>
#include <stdio.h>

#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include "SAPDBCommon/DatabaseEvents/SAPDBDBE_EventCategory.hpp"
#include "SAPDBCommon/Messages/SDBMsg_SAPDBMem.h"
#include "SAPDBCommon/SAPDB_string.h"

#define SAPDBDBE_EventCategory_PrioChar_Low    'L'
#define SAPDBDBE_EventCategory_PrioChar_Medium 'M'
#define SAPDBDBE_EventCategory_PrioChar_High   'H'
#define SAPDBDBE_EventCategory_PrioChar_None   'X'

SAPDBDBE_EventCategoryIteratorNode* SAPDBDBE_EventCategory::buildInList(
        Msg_List& errList,
        const SAPDB_Char* aList,
        SAPDBDBE_EventDefinition::EventDefinitionStatus aStatus ) {

    if( aList == NULL )
        return NULL;

    const SAPDB_Char* currentChar(aList);
    SAPDBDBE_EventCategoryIteratorNode* outList(NULL);
    SAPDBDBE_EventCategoryIteratorNode* last(outList);
    while( *currentChar != '\0' ) {
        SAPDB_Int4 value = atoi(currentChar);
        while( getPriorityForChar(*currentChar) == sp31ep_nil )
            ++currentChar;
        SAPDBDBE_EventDefinition* newDef = new (RTEMem_Allocator::Instance()) SAPDBDBE_EventDefinition(
                aStatus,
                getPriorityForChar(*currentChar),
                value);

        if( newDef == NULL ) {
            // no memory allocated
            m_InstanceIsValid = false;
            Msg_List memErr(
                    Msg_List::Error,
                    SDBMSG_SAPDBMEM_ALLOCATOR_OUT_OF_MEMORY,
	                Msg_Arg("BYTES_NEEDED",sizeof(SAPDBDBE_EventDefinition)),
	                Msg_Arg("DESCRIPTION",m_ErrorDescriptionTagValue),
	                Msg_Arg("ALLOCATOR",RTEMem_Allocator::Instance().GetIdentifier()));
            errList.Overrule(memErr);
            return outList;
        }

        ++currentChar;
        SAPDBDBE_EventCategoryIteratorNode* newNode = new (RTEMem_Allocator::Instance()) 
                SAPDBDBE_EventCategoryIteratorNode(newDef);
        if( newNode == NULL ) {
            // no memory allocated
            m_InstanceIsValid = false;
            newDef->~SAPDBDBE_EventDefinition();
            RTEMem_Allocator::Instance().Deallocate(newDef);
            Msg_List memErr(
                    Msg_List::Error,
                    SDBMSG_SAPDBMEM_ALLOCATOR_OUT_OF_MEMORY,
	                Msg_Arg("BYTES_NEEDED",sizeof(SAPDBDBE_EventCategoryIteratorNode)),
	                Msg_Arg("DESCRIPTION",m_ErrorDescriptionTagValue),
	                Msg_Arg("ALLOCATOR",RTEMem_Allocator::Instance().GetIdentifier()));
            errList.Overrule(memErr);
            return outList;
        }

        if( last == NULL ) {
            outList = newNode;
            last = outList;
        }
        else {
            last->append(newNode);
            last = last->getNext();
        }
    }
    return outList;
}

SAPDBDBE_EventCategory::SAPDBDBE_EventCategory(
        Msg_List& errList,
        const SAPDB_Char* newCategoryValue,
        const SAPDB_Char* oldCategoryValue)
        : m_IterationEnd(false),
          m_InstanceIsValid(false),
          m_EventDefinitionListRoot(NULL),
          m_EventDefinitionListCurrent(NULL) {
    
    errList.ClearMessageList();

    // build list of new event definitions
    SAPDBDBE_EventCategoryIteratorNode* newList = 
        buildInList(errList, newCategoryValue, SAPDBDBE_EventDefinition::EDS_Added);

    // build list of old definitions
    SAPDBDBE_EventCategoryIteratorNode* oldList = 
        buildInList(errList, oldCategoryValue, SAPDBDBE_EventDefinition::EDS_Removed);

    if( !errList.IsEmpty() ) {
        // free the inlists
        while( newList != NULL ) {
            SAPDBDBE_EventCategoryIteratorNode* tmp(newList);
            newList = newList->getNext();
            tmp->~SAPDBDBE_EventCategoryIteratorNode();
            RTEMem_Allocator::Instance().Deallocate(tmp);
        }
        while( oldList != NULL ) {
            SAPDBDBE_EventCategoryIteratorNode* tmp(oldList);
            oldList = oldList->getNext();
            tmp->~SAPDBDBE_EventCategoryIteratorNode();
            RTEMem_Allocator::Instance().Deallocate(tmp);
        }
        return;
    }

    // now build resulting list for the iterator interface to work on
    if( oldList == NULL )
        m_EventDefinitionListRoot = newList;
    else {
        while( oldList != NULL || newList != NULL ) {
            // there is something to append
            if( oldList == NULL) {
                // take node from newList as "new"
                SAPDBDBE_EventCategoryIteratorNode* appendee(newList);
                newList = newList->getNext();
                appendToList(appendee, SAPDBDBE_EventDefinition::EDS_Added);
            }
            else if( newList == NULL) {
                // take node from oldList as "deleted"
                SAPDBDBE_EventCategoryIteratorNode* appendee(oldList);
                oldList = oldList->getNext();
                appendToList(appendee, SAPDBDBE_EventDefinition::EDS_Removed);
            }
            else {
                SAPDB_Int4 oldValue = oldList->getData()->getValue();
                SAPDB_Int4 newValue = newList->getData()->getValue();
                if( oldValue < newValue ) {
                    // take node from oldList
                    SAPDBDBE_EventCategoryIteratorNode* appendee(oldList);
                    oldList = oldList->getNext();
                    appendToList(appendee, SAPDBDBE_EventDefinition::EDS_Removed);
                }
                else if( oldValue > newValue ) {
                    // take node from newList
                    SAPDBDBE_EventCategoryIteratorNode* appendee(newList);
                    newList = newList->getNext();
                    appendToList(appendee, SAPDBDBE_EventDefinition::EDS_Added);
                }
                else {
                    // oldValue == newValue
                    tsp31_event_prio_Enum oldPrio = oldList->getData()->getPriority();
                    tsp31_event_prio_Enum newPrio = newList->getData()->getPriority();
                    if( oldPrio == newPrio ) {
                        // values equal, priorities equal, we can delete both
                        SAPDBDBE_EventCategoryIteratorNode* tmp(oldList);
                        oldList = oldList->getNext();
                        tmp->~SAPDBDBE_EventCategoryIteratorNode();
                        RTEMem_Allocator::Instance().Deallocate(tmp);
                        tmp = newList;
                        newList = newList->getNext();
                        tmp->~SAPDBDBE_EventCategoryIteratorNode();
                        RTEMem_Allocator::Instance().Deallocate(tmp);
                    }
                    else {
                        // just the priority was changed, take node from newList as changed, delete oldList
                        SAPDBDBE_EventCategoryIteratorNode* appendee(newList);
                        newList = newList->getNext();
                        appendToList(appendee, SAPDBDBE_EventDefinition::EDS_Modified);
                        SAPDBDBE_EventCategoryIteratorNode* tmp(oldList);
                        oldList = oldList->getNext();
                        tmp->~SAPDBDBE_EventCategoryIteratorNode();
                        RTEMem_Allocator::Instance().Deallocate(tmp);
                    }
                }
            }
        }
    }
    m_InstanceIsValid = true;
    m_EventDefinitionListCurrent = NULL;
}

void SAPDBDBE_EventCategory::appendToList(
        SAPDBDBE_EventCategoryIteratorNode* aNode,
        SAPDBDBE_EventDefinition::EventDefinitionStatus aStatus) {

     aNode->append(NULL);
     aNode->getData()->setStatus(aStatus);
    if( m_EventDefinitionListRoot == NULL) {
        m_EventDefinitionListRoot = aNode;
        m_EventDefinitionListCurrent = aNode;
    }
    else {
        m_EventDefinitionListCurrent->append(aNode);
        m_EventDefinitionListCurrent = m_EventDefinitionListCurrent->getNext();
    }   
}

SAPDB_Char SAPDBDBE_EventCategory::getCharForPriority(
        tsp31_event_prio_Enum aPriorityVal ) {

    switch( aPriorityVal ) {
        case sp31ep_low:
            return SAPDBDBE_EventCategory_PrioChar_Low;
            break;
        case sp31ep_medium:
            return SAPDBDBE_EventCategory_PrioChar_Medium;
            break;
        case sp31ep_high:
            return SAPDBDBE_EventCategory_PrioChar_High;
            break;
        default:
            return SAPDBDBE_EventCategory_PrioChar_None;
            break;
    }
}

tsp31_event_prio_Enum SAPDBDBE_EventCategory::getPriorityForChar(
        const SAPDB_Char aPriorityChar ) {

    switch( aPriorityChar ) {
        case SAPDBDBE_EventCategory_PrioChar_Low:
            return sp31ep_low;
            break;
        case SAPDBDBE_EventCategory_PrioChar_Medium:
            return sp31ep_medium;
            break;
        case SAPDBDBE_EventCategory_PrioChar_High:
            return sp31ep_high;
            break;
        default:
            return sp31ep_nil;
            break;
    }
}

SAPDBDBE_EventCategory::~SAPDBDBE_EventCategory() {
    // just delete the list
    while( m_EventDefinitionListRoot != NULL ) {
        SAPDBDBE_EventCategoryIteratorNode* tmp(m_EventDefinitionListRoot->getNext());
        m_EventDefinitionListRoot->~SAPDBDBE_EventCategoryIteratorNode();
        RTEMem_Allocator::Instance().Deallocate(m_EventDefinitionListRoot);
        m_EventDefinitionListRoot = tmp;
    }
}

// used in dbm server
SAPDB_Bool SAPDBDBE_EventCategory::contains( const SAPDBDBE_EventDefinition& aEventDefinition ) const {
    if( !m_InstanceIsValid )
        return false;

    SAPDB_Int4 checkValue = aEventDefinition.getValue();
    SAPDBDBE_EventCategoryIteratorNode* currentNode(m_EventDefinitionListRoot);
    while( currentNode != NULL && currentNode->getData()->getValue() <= checkValue ) {
        SAPDB_Int4 currentValue = currentNode->getData()->getValue();
        if( checkValue == currentValue ) {
            tsp31_event_prio_Enum checkPrio = aEventDefinition.getPriority();
            tsp31_event_prio_Enum currentPrio = currentNode->getData()->getPriority();
            return checkPrio == currentPrio;
        }
        currentNode = currentNode->getNext();
    }
    return false;
}

void SAPDBDBE_EventCategory::add( Msg_List& errList, const SAPDBDBE_EventDefinition& aEventDefinition ) {
    if( !m_InstanceIsValid )
        return;

    errList.ClearMessageList();
    m_EventDefinitionListCurrent = NULL;
    m_IterationEnd = false;

    SAPDBDBE_EventDefinition* newDefinition(NULL);
    SAPDB_Int4 checkValue = aEventDefinition.getValue();

    if( m_EventDefinitionListRoot == NULL ) {
        // adding the first node
        newDefinition = new (RTEMem_Allocator::Instance()) SAPDBDBE_EventDefinition(
                aEventDefinition.getStatus(),
                aEventDefinition.getPriority(),
                aEventDefinition.getValue());
        if( newDefinition == NULL ) {
            // no memory allocated
            m_InstanceIsValid = false;
            Msg_List memErr(
                    Msg_List::Error,
                    SDBMSG_SAPDBMEM_ALLOCATOR_OUT_OF_MEMORY,
	                Msg_Arg("BYTES_NEEDED",sizeof(SAPDBDBE_EventDefinition)),
	                Msg_Arg("DESCRIPTION",m_ErrorDescriptionTagValue),
	                Msg_Arg("ALLOCATOR",RTEMem_Allocator::Instance().GetIdentifier()));
            errList.Overrule(memErr);
            return;
        }

        m_EventDefinitionListRoot = new (RTEMem_Allocator::Instance()) SAPDBDBE_EventCategoryIteratorNode(newDefinition);
        if( m_EventDefinitionListRoot == NULL ) {
            // no memory allocated
            newDefinition->~SAPDBDBE_EventDefinition();
            RTEMem_Allocator::Instance().Deallocate(newDefinition);
            m_InstanceIsValid = false;
            Msg_List memErr(
                    Msg_List::Error,
                    SDBMSG_SAPDBMEM_ALLOCATOR_OUT_OF_MEMORY,
	                Msg_Arg("BYTES_NEEDED",sizeof(SAPDBDBE_EventCategoryIteratorNode)),
	                Msg_Arg("DESCRIPTION",m_ErrorDescriptionTagValue),
	                Msg_Arg("ALLOCATOR",RTEMem_Allocator::Instance().GetIdentifier()));
            errList.Overrule(memErr);
        }
        return;
    }
    else if( m_EventDefinitionListRoot->getData()->getValue() > checkValue ) {
        // new node is to be appended in front of first node
        newDefinition = new (RTEMem_Allocator::Instance()) SAPDBDBE_EventDefinition(
                aEventDefinition.getStatus(),
                aEventDefinition.getPriority(),
                aEventDefinition.getValue());
        if( newDefinition == NULL ) {
            // no memory allocated
            m_InstanceIsValid = false;
            Msg_List memErr(
                    Msg_List::Error,
                    SDBMSG_SAPDBMEM_ALLOCATOR_OUT_OF_MEMORY,
	                Msg_Arg("BYTES_NEEDED",sizeof(SAPDBDBE_EventDefinition)),
	                Msg_Arg("DESCRIPTION",m_ErrorDescriptionTagValue),
	                Msg_Arg("ALLOCATOR",RTEMem_Allocator::Instance().GetIdentifier()));
            errList.Overrule(memErr);
            return;
        }
        
        SAPDBDBE_EventCategoryIteratorNode* tmp(m_EventDefinitionListRoot);
        m_EventDefinitionListRoot = new (RTEMem_Allocator::Instance()) SAPDBDBE_EventCategoryIteratorNode(newDefinition);
        if( m_EventDefinitionListRoot == NULL ) {
            // no memory allocated
            m_InstanceIsValid = false;
            newDefinition->~SAPDBDBE_EventDefinition();
            RTEMem_Allocator::Instance().Deallocate(newDefinition);
            Msg_List memErr(
                    Msg_List::Error,
                    SDBMSG_SAPDBMEM_ALLOCATOR_OUT_OF_MEMORY,
	                Msg_Arg("BYTES_NEEDED",sizeof(SAPDBDBE_EventCategoryIteratorNode)),
	                Msg_Arg("DESCRIPTION",m_ErrorDescriptionTagValue),
	                Msg_Arg("ALLOCATOR",RTEMem_Allocator::Instance().GetIdentifier()));
            errList.Overrule(memErr);
            return;
        }

        m_EventDefinitionListRoot->append(tmp);
        return;
    }

    // new node's value is either equal or higher than first node

    SAPDBDBE_EventCategoryIteratorNode* currentNode(m_EventDefinitionListRoot);
    while( currentNode != NULL ) {
        SAPDB_Int4 currentValue = currentNode->getData()->getValue();
        if( checkValue == currentValue ) {
            // new node is already contained
            tsp31_event_prio_Enum checkPrio = aEventDefinition.getPriority();
            tsp31_event_prio_Enum currentPrio = currentNode->getData()->getPriority();
            if( checkPrio == currentPrio )
                // even prio equals, we're done
                return;
            else {
                // new node is contained, but with different priority, so change it
                currentNode->getData()->setPriority(checkPrio);
                return;
            }
        }
        else if( checkValue > currentValue ) {
            // new node must be put after current node
            if( currentNode->getNext() == NULL ) {
                // new node will be that last one
                newDefinition = new (RTEMem_Allocator::Instance()) SAPDBDBE_EventDefinition(
                        aEventDefinition.getStatus(),
                        aEventDefinition.getPriority(),
                        aEventDefinition.getValue());
                if( newDefinition == NULL ) {
                    // no memory allocated
                    m_InstanceIsValid = false;
                    Msg_List memErr(
                            Msg_List::Error,
                            SDBMSG_SAPDBMEM_ALLOCATOR_OUT_OF_MEMORY,
	                        Msg_Arg("BYTES_NEEDED",sizeof(SAPDBDBE_EventDefinition)),
	                        Msg_Arg("DESCRIPTION",m_ErrorDescriptionTagValue),
	                        Msg_Arg("ALLOCATOR",RTEMem_Allocator::Instance().GetIdentifier()));
                    errList.Overrule(memErr);
                    return;
                }

                SAPDBDBE_EventCategoryIteratorNode* newNode =
                        new (RTEMem_Allocator::Instance()) SAPDBDBE_EventCategoryIteratorNode(newDefinition); 
                if( newNode == NULL ) {
                    // no memory allocated
                    newDefinition->~SAPDBDBE_EventDefinition();
                    RTEMem_Allocator::Instance().Deallocate(newDefinition);
                    m_InstanceIsValid = false;
                    Msg_List memErr(
                            Msg_List::Error,
                            SDBMSG_SAPDBMEM_ALLOCATOR_OUT_OF_MEMORY,
	                        Msg_Arg("BYTES_NEEDED",sizeof(SAPDBDBE_EventCategoryIteratorNode)),
	                        Msg_Arg("DESCRIPTION",m_ErrorDescriptionTagValue),
	                        Msg_Arg("ALLOCATOR",RTEMem_Allocator::Instance().GetIdentifier()));
                    errList.Overrule(memErr);
                    return;
                }
                currentNode->append(newNode);
                return;
            }
            else {
                SAPDBDBE_EventCategoryIteratorNode* afterCurrent = currentNode->getNext();
                if( checkValue < afterCurrent->getData()->getValue() ) {
                    // put new node between current and its successor
                    newDefinition = new (RTEMem_Allocator::Instance()) SAPDBDBE_EventDefinition(
                            aEventDefinition.getStatus(),
                            aEventDefinition.getPriority(),
                            aEventDefinition.getValue());
                    if( newDefinition == NULL ) {
                        // no memory allocated
                        m_InstanceIsValid = false;
                        Msg_List memErr(
                                Msg_List::Error,
                                SDBMSG_SAPDBMEM_ALLOCATOR_OUT_OF_MEMORY,
	                            Msg_Arg("BYTES_NEEDED",sizeof(SAPDBDBE_EventDefinition)),
	                            Msg_Arg("DESCRIPTION",m_ErrorDescriptionTagValue),
	                            Msg_Arg("ALLOCATOR",RTEMem_Allocator::Instance().GetIdentifier()));
                        errList.Overrule(memErr);
                        return;
                    }

                    SAPDBDBE_EventCategoryIteratorNode* newNode =
                            new (RTEMem_Allocator::Instance()) SAPDBDBE_EventCategoryIteratorNode(newDefinition);
                    if( newNode == NULL ) {
                        // no memory allocated
                        m_InstanceIsValid = false;
                        newDefinition->~SAPDBDBE_EventDefinition();
                        RTEMem_Allocator::Instance().Deallocate(newDefinition);
                        Msg_List memErr(
                                Msg_List::Error,
                                SDBMSG_SAPDBMEM_ALLOCATOR_OUT_OF_MEMORY,
	                            Msg_Arg("BYTES_NEEDED",sizeof(SAPDBDBE_EventCategoryIteratorNode)),
	                            Msg_Arg("DESCRIPTION",m_ErrorDescriptionTagValue),
	                            Msg_Arg("ALLOCATOR",RTEMem_Allocator::Instance().GetIdentifier()));
                        errList.Overrule(memErr);
                        return;
                    }

                    currentNode->append(newNode);
                    currentNode = currentNode->getNext();
                    currentNode->append(afterCurrent);
                    return;
                }
                else
                    // new node will be even behind currentNode's successor; try in next loop
                    currentNode = currentNode->getNext();
            }
        }
    }
}

void SAPDBDBE_EventCategory::remove( const SAPDBDBE_EventDefinition& aEventDefinition ) {

    if( !m_InstanceIsValid )
        return;

    m_EventDefinitionListCurrent = NULL;
    m_IterationEnd = false;

    if( m_EventDefinitionListRoot == NULL )
        // list is already empty
        return;
    
    SAPDBDBE_EventCategoryIteratorNode* currentNode(m_EventDefinitionListRoot);
    SAPDB_Int4 checkValue = aEventDefinition.getValue();
    tsp31_event_prio_Enum checkPrio = aEventDefinition.getPriority();
    
    if( checkValue == m_EventDefinitionListRoot->getData()->getValue() &&
        checkPrio == m_EventDefinitionListRoot->getData()->getPriority() ) {
        // the node to delete is the root node
        currentNode = m_EventDefinitionListRoot->getNext();
        m_EventDefinitionListRoot->~SAPDBDBE_EventCategoryIteratorNode();
        RTEMem_Allocator::Instance().Deallocate(m_EventDefinitionListRoot);
        m_EventDefinitionListRoot = currentNode;
    }
    else {
        // the node to delete is further behind
        SAPDBDBE_EventCategoryIteratorNode* currentNode(m_EventDefinitionListRoot);
        while( currentNode != NULL ) {
            SAPDBDBE_EventCategoryIteratorNode* afterCurrent = currentNode->getNext();
            if( afterCurrent!= NULL &&
                checkValue == afterCurrent->getData()->getValue() &&
                checkPrio == afterCurrent->getData()->getPriority() ) {
                currentNode->append(afterCurrent->getNext());
                afterCurrent->~SAPDBDBE_EventCategoryIteratorNode();
                RTEMem_Allocator::Instance().Deallocate(afterCurrent);
            }
            currentNode = currentNode->getNext();
        }
    }
}

size_t SAPDBDBE_EventCategory::getUpdatedCategoryValueSize() const {
    size_t neverUsed(0);
    return getUpdatedCategoryValueSize(neverUsed);
}

size_t SAPDBDBE_EventCategory::getUpdatedCategoryValueSize(size_t& maxValueSize) const {

    size_t requiredSize(0);

    SAPDBDBE_EventCategoryIteratorNode* currentNode(m_EventDefinitionListRoot);
    maxValueSize = 0;
    size_t currentValueSize(0);

    requiredSize = 0;
    while( currentNode != NULL ) {
        // one SAPDB_Char for the priority
        ++requiredSize;
        
        // calculate length of value
        SAPDB_Int4 currentValue = currentNode->getData()->getValue();
        currentValueSize = 0;
        if( currentValue == 0 )
            ++currentValueSize;
        else {
            if( currentValue < 0 ) {
                ++currentValueSize;
                currentValue = -currentValue;
            }
            while( currentValue > 0 ) {
                ++currentValueSize;
                currentValue = currentValue / 10;
            }
        }
        
        requiredSize += currentValueSize;

        if( currentValueSize > maxValueSize )
            currentValueSize = maxValueSize;

        currentNode = currentNode->getNext();
    }

    // finally add one for the terminator
    ++requiredSize;
    return requiredSize;
}

SAPDB_Bool SAPDBDBE_EventCategory::getUpdatedCategoryValue(
        SAPDB_Char* outBuffer,
        size_t outBufferSize,
        size_t& requiredSize) const {

    if( !m_InstanceIsValid )
        return false;

    size_t maxValueLength(0);
    requiredSize = getUpdatedCategoryValueSize(maxValueLength);

    if( requiredSize > outBufferSize )
        // the buffer we got is too small
        return false;
    
    // if we're here, we have this byte
    outBuffer[0] = '\0';    

    // fill the outBuffer
    
    SAPDB_Char valueBuf[128];  // since placement new for arrays is tricky, I don't do it...
                               // 128 should be big enough for a few years.
    if( maxValueLength > 127 )
        return false;

    SAPDBDBE_EventCategoryIteratorNode* currentNode(m_EventDefinitionListRoot);
    while( currentNode != NULL ) {
        // write the value
        sprintf(valueBuf, "%d", currentNode->getData()->getValue());
        strcat(outBuffer, valueBuf);
        // write the priority
        size_t currentLen(strlen(outBuffer));
        outBuffer[currentLen] = getCharForPriority(currentNode->getData()->getPriority());
        outBuffer[currentLen+1] = '\0';
        currentNode = currentNode->getNext();
    }
    return true;
}

// iterator interface, used in kernel; don't mix calls with dbm server methods
SAPDB_Bool SAPDBDBE_EventCategory::hasNext() {

    if( !m_InstanceIsValid )
        return false;

    if( m_EventDefinitionListRoot == NULL )
        return false;

    if( !m_IterationEnd && m_EventDefinitionListCurrent == NULL ) {
        m_EventDefinitionListCurrent = m_EventDefinitionListRoot;
    }
 
    if( m_IterationEnd ) {
        m_IterationEnd = false;
        return false;
    }
    else {
        return true;
    }
}

const SAPDBDBE_EventDefinition& SAPDBDBE_EventCategory::getNext() {

    if( !m_InstanceIsValid || m_EventDefinitionListRoot == NULL )
        return m_DummyEventDefinition;

    SAPDBDBE_EventDefinition* currentData(m_EventDefinitionListCurrent->getData());
    m_EventDefinitionListCurrent = m_EventDefinitionListCurrent->getNext();
    m_IterationEnd = (m_EventDefinitionListCurrent == NULL);

    return *currentData;
}

const SAPDBDBE_EventDefinition SAPDBDBE_EventCategory::m_DummyEventDefinition(
        SAPDBDBE_EventDefinition::EDS_Added,
        sp31ep_nil,
        0);

const SAPDB_Char* const SAPDBDBE_EventCategory::m_ErrorDescriptionTagValue = "Database Events Management";
