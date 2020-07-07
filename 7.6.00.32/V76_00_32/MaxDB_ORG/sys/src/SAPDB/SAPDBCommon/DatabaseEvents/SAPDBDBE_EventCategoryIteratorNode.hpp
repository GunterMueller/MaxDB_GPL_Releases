/*!
  \file    SAPDBDBE_EventCategoryIteratorNode.hpp
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

#if !defined(SAPDBDBE_EventCategoryIteratorNode_hpp)
#define SAPDBDBE_EventCategoryIteratorNode_hpp

#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include "SAPDBCommon/DatabaseEvents/SAPDBDBE_EventDefinition.hpp"

/*!
    @brief helper class for building a list of SAPDBDBE_EventDefinition objects.

    This class is used only internally by SAPDBDBE_EventCategory
    Calling the destructor of an object also destructs the payload (the linked SAPDBDBE_EventDefinition object).
    It is required that all linked SAPDBDBE_EventDefinition objects be allocated with the RTEMem_Allocator.
    @see SAPDBDBE_EventCategory
    @see SAPDBDBE_EventDefinition
*/
class SAPDBDBE_EventCategoryIteratorNode {
public:
    /*!
        @brief constructor

        Constructs a new SAPDBDBE_EventCategoryIteratorNode object. The passed pointer to a event
        definition is stored.
        @param aEventDefinition [IN] pointer to a event definition that is stored in this object
    */
    SAPDBDBE_EventCategoryIteratorNode(SAPDBDBE_EventDefinition* aEventDefinition);
    
    /*!
        @brief destructor, the linked SAPDBDBE_EventDefinition object is destroyed
        It is required that all linked SAPDBDBE_EventDefinition objects be allocated with the RTEMem_Allocator.
    */
    ~SAPDBDBE_EventCategoryIteratorNode();
    
    /*!
        @brief returns a pointer to the next SAPDBDBE_EventCategoryIteratorNode
        @return ointer to the next SAPDBDBE_EventCategoryIteratorNode (of NULL, if there is none)
    */
    SAPDBDBE_EventCategoryIteratorNode* getNext() const;

    /*!
        @brief return pointer to the payload (the linked SAPDBDBE_EventDefinition object)
        @return pointer to the payload (the linked SAPDBDBE_EventDefinition object)
    */
    SAPDBDBE_EventDefinition* getData() const;

    /*!
        @brief append another SAPDBDBE_EventCategoryIteratorNode to this one
        @param otherNode [IN] pointer to another SAPDBDBE_EventCategoryIteratorNode to append (NULL can be appended)
    */
    void append(SAPDBDBE_EventCategoryIteratorNode* otherNode);

private:
    SAPDBDBE_EventDefinition* m_Data;
    SAPDBDBE_EventCategoryIteratorNode* m_NextNode;
};

inline SAPDBDBE_EventCategoryIteratorNode::SAPDBDBE_EventCategoryIteratorNode(
        SAPDBDBE_EventDefinition* aEventDefinition)
        : m_Data(aEventDefinition),
        m_NextNode(NULL) {}

inline SAPDBDBE_EventCategoryIteratorNode::~SAPDBDBE_EventCategoryIteratorNode() {
    m_Data->~SAPDBDBE_EventDefinition();
    RTEMem_Allocator::Instance().Deallocate(m_Data);
    m_Data = NULL;
}

inline SAPDBDBE_EventCategoryIteratorNode* SAPDBDBE_EventCategoryIteratorNode::getNext() const {
    return m_NextNode;
}

inline SAPDBDBE_EventDefinition* SAPDBDBE_EventCategoryIteratorNode::getData() const {
    return m_Data;
}

inline void SAPDBDBE_EventCategoryIteratorNode::append(
        SAPDBDBE_EventCategoryIteratorNode* otherNode) {
    m_NextNode = otherNode;
}

#endif //SAPDBDBE_EventCategoryIteratorNode_hpp
