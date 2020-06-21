/*!
  \file    SAPDBDBE_EventCategory.hpp
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

#if !defined(SAPDBDBE_EventCategory_hpp)
#define SAPDBDBE_EventCategory_hpp

#include "SAPDBCommon/DatabaseEvents/SAPDBDBE_EventCategoryIteratorNode.hpp"
#include "SAPDBCommon/DatabaseEvents/SAPDBDBE_EventDefinition.hpp"

/*!
    @brief accessing and modification for kernel parameters that describe event categories that can contain
           more than one event definition.

    This class is used in the kernel to find out with event definitions must be added or 
    deleted in the kernel parts that actually fire events. Therefore, it has methods for iteration on
    the event category, that return a SAPDBDBE_EventDefinition.

    Manipulation methods of an event category are also facilitated by this class. They are to be used
    by the DBM server.

    All dynamic memory allocation is done with the RTEMem_Allocator.

    Internal information: A kernel parameter that holds an event category, that can contain more than one
    active event definition is composed like this: [i0][c0][i1][c1]...[in][cn], where i0,..,in are 
    numbers and ij<ik, for j<k. And c0,...,cn are either 'L', 'M' or 'H'. In terms of event definitions, 
    ij describes a value and cj a priority.

    @see SAPDBDBE_EventDefinition
*/
class SAPDBDBE_EventCategory {
public:

    /*!
        @brief constructor

        If only errList is passed, an empty event category is constructed. If also newCategoryValue is
        passed, a SAPDBDBE_EventCategory containing exactly the event definitions from it is constructed.
        If in addition to newCategoryValue also oldCategoryValue is passed, a SAPDBDBE_EventCategory is
        constructed, that contains a "merged" list of event definitions, which allows to find out which
        event definition was added (came from newCategoryValue), which was removed (came from oldCategoryValue)
        and which was modified (was provided by both newCategoryValue and oldCategoryValue with different
        priorities). An event definition, which was unchanged, is not contained in the list of event definitions.

        Thus it is possible to iterate on which part of the event category, that is desired:
        * only the old category value (pass errList and the old category value in parameter newCategoryValue)
        * only the new category value (pass errList and the new category value in parameter newCategoryValue)
        * only the "difference" (pass errList, the new category value and the old category value)

        @param errList [OUT] filled in case of errors (is cleared initially)
        @param newCategoryValue [IN] new event category value (strcutured as described in class description)
        @param oldCategoryValue [IN] old event category value (strcutured as described in class description)

        @see SAPDBDBE_EventDefinition
    */
    SAPDBDBE_EventCategory(
            Msg_List& errList,
            const SAPDB_Char* newCategoryValue = NULL,
            const SAPDB_Char* oldCategoryValue = NULL);
    
    /*! @brief destructor */
    ~SAPDBDBE_EventCategory();

    /*!
        @brief check if an event definition is contained in this event category

        This method is used by the manipulator of the event category (e.g. DBM Server).
        @param aEventDefinition [IN] event definition to check for
        @return true, if an event definition with the same value and the same priority is contained
                in this event category (event definition's status is not checked).
        @see SAPDBDBE_EventDefinition
    */
    SAPDB_Bool contains( const SAPDBDBE_EventDefinition& aEventDefinition ) const;

    /*!
        @brief add a new event definition to this event category

        This method is used by the manipulator of the event category (e.g. DBM Server).
        @param errList [OUT] filled in case of errors (is cleared initially)        
        @param aEventDefinition [IN] event definition to add. It is copied.
    */
    void add( Msg_List& errList, const SAPDBDBE_EventDefinition& aEventDefinition );


    /*!
        @brief remove an event definition from this event category

        This method is used by the manipulator of the event category (e.g. DBM Server).
        If an event definition with the same value and the same priority is contained in
        this event category, it is removed (event definition's status is not checked).
        @param aEventDefinition [IN] event definition to remove.
    */
    void remove( const SAPDBDBE_EventDefinition& aEventDefinition );
    
    /*!
        @brief return the size of the string that is returned by getUpdatedCategoryValue (including terminator)

        This method is used by the manipulator of the event category (e.g. DBM Server).
        @return size of string returned by getUpdatedCategoryValue
    */
    size_t getUpdatedCategoryValueSize() const;
    
    /*!
        @brief constructs the updated event category value as a string.
        
        This method is used by the manipulator of the event category (e.g. DBM Server).
        The format of the string is as described in the class description of this class. If the space
        provided by the caller is smaller than necessary, just requiredSize is calculated and nothing else
        is done.
        @param outBuffer [OUT] buffer for the serialized event category
        @param outBufferSize [IN] size of outBuffer
        @param requiredSize [OUT] required size for the serialized event category (including terminator)
        @return true if serialized event category cluld be calculated and enough space was provided, false otherwise
    */
    SAPDB_Bool getUpdatedCategoryValue(SAPDB_Char* outBuffer, size_t outBufferSize, size_t& requiredSize) const;

    /*!
        @brief part of the "iterator interface" of this class

        This method is used by the process, that must handle changes of the event category (e.g. the kernel).
        Calls to this method should not be mixed with calls to methods that are not part of the "iterator interface".
        @returns true, if the next call to getNext() will return a valid SAPDBDBE_EventDefinition, false otherwise
        @see getNext
    */
    SAPDB_Bool hasNext();

    /*!
        @brief part of the "iterator interface" of this class

        This method is used by the process, that must handle changes of the event category (e.g. the kernel).
        Calls to this method should not be mixed with calls to methods that are not part of the "iterator interface".
        @returns a valid SAPDBDBE_EventDefinition, if a previous call to hasNext() returned true,
                 otherwise a dummy event definition. The returned reference is only valid in the scope of the
                 SAPDBDBE_EventCategory that returned it.
        @see hasNext
    */
    const SAPDBDBE_EventDefinition& getNext();

    /*!
        @brief status of this event category object

        If this object is not valid anymore (e.g. memory problems), calls to all methods will return immediately.
        @return true, if no error occurred during the life cycle of this object, false otherwise
    */
    SAPDB_Bool isValid() const {return m_InstanceIsValid;}

private:
    SAPDBDBE_EventCategoryIteratorNode* m_EventDefinitionListRoot;
    SAPDBDBE_EventCategoryIteratorNode* m_EventDefinitionListCurrent;
    SAPDB_Bool m_IterationEnd;
    SAPDB_Bool m_InstanceIsValid;

    void appendToList(
            SAPDBDBE_EventCategoryIteratorNode* aNode,
            SAPDBDBE_EventDefinition::EventDefinitionStatus aStatus);

    SAPDBDBE_EventCategoryIteratorNode* buildInList(
            Msg_List& errList,
            const SAPDB_Char* aList,
            SAPDBDBE_EventDefinition::EventDefinitionStatus aStatus);

    size_t getUpdatedCategoryValueSize(size_t& maxValueSize) const;

    static SAPDB_Char getCharForPriority(tsp31_event_prio_Enum aPriorityVal );
    static tsp31_event_prio_Enum getPriorityForChar(const SAPDB_Char aPriorityChar );    

    static const SAPDBDBE_EventDefinition m_DummyEventDefinition;
    static const SAPDB_Char* const m_ErrorDescriptionTagValue;
};

#endif //SAPDBDBE_EventCategory_hpp
