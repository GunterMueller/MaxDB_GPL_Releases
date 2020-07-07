/*!
  @file           Converter_LeafReference.hpp
  @author         Henrik
  @brief          container for temporary stored blockAddr <-> parentReference aggregation
*/


/*
    ========== licence begin  GPL
    Copyright (c) 2002-2004 SAP AG

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

#ifndef CONVERTER_LEAFREFERENCE_HPP
#define CONVERTER_LEAFREFERENCE_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 **/

#include "IOManager/IOMan_BlockAddress.hpp"
#include "Converter/Converter_ParentReference.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 **/


/*===========================================================================*
 *  MACROS                                                                   *
 **/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 **/

/*!
  @class          Converter_LeafReference
  @brief          container for temporary stored blockAddr <-> parentReference aggregation
 */
class Converter_LeafReference
{
public:

/*!
    @name Constructors
*/

    /*!
       @brief     Constructor
       @param     blockPos [in] block address
       @param     parentRef [in] parent reference
    */
    Converter_LeafReference
    (const IOMan_BlockAddress & blockPos,
     const Converter_ParentReference & parentRef)
    :
    m_Block(blockPos),
    m_ParentReference(parentRef)
    {}

    /*!
       @brief     Constructor
    */
    Converter_LeafReference()
    {}

    /*!
       @brief     copy Constructor
       @param     leaf [in] other Converter_LeafReference
    */
    Converter_LeafReference(const Converter_LeafReference& leaf)
    :
    m_Block(leaf.m_Block),
    m_ParentReference(leaf.m_ParentReference)
    {}

    /*!
       @brief          Compares with other LeafIndex
       @param          idx [in] other index
       @return         (SAPDB_Bool) true if unequal
     */
    SAPDB_Bool operator!=(const Converter_LeafReference &idx) const
    {
        return (m_Block.GetBlockNo() != idx.m_Block.GetBlockNo());
    }

    /*!
       @brief          Compares with other LeafIndex
       @param          idx [in] other index
       @return         (SAPDB_Bool) true if equal
     */
    SAPDB_Bool operator==(const Converter_LeafReference &idx) const
    {
        return (m_Block.GetBlockNo() == idx.m_Block.GetBlockNo());
    }

    /*!
       @brief          Compares with other LeafIndex
       @param          idx [in] other index
       @return         (SAPDB_Bool) true if less or equal
     */
    SAPDB_Bool operator<=(const Converter_LeafReference &idx) const
    {
        return (m_Block.GetBlockNo() <= idx.m_Block.GetBlockNo());
    }

    /*!
       @brief          Compares with other LeafIndex
       @param          idx [in] other index
       @return         (SAPDB_Bool) true if less
     */
    SAPDB_Bool operator<(const Converter_LeafReference &idx) const
    {
        return (m_Block.GetBlockNo() < idx.m_Block.GetBlockNo());
    }

    /*!
       @brief          Compares with other LeafIndex
       @param          idx [in] other index
       @return         (SAPDB_Bool) true if greater or equal
     */
    SAPDB_Bool operator>=(const Converter_LeafReference &idx) const
    {
        return (m_Block.GetBlockNo() >= idx.m_Block.GetBlockNo());
    }

    /*!
       @brief          Compares with other LeafIndex
       @param          idx [in] other index
       @return         (SAPDB_Bool) true if greater
     */
    SAPDB_Bool operator>(const Converter_LeafReference &idx) const
    {
        return (m_Block.GetBlockNo() > idx.m_Block.GetBlockNo());
    }

    /*!
       @brief        get stored block address
       @return       (IOMan_BlockAddress) block address
    */
    IOMan_BlockAddress  GetBlockAddress() const {
        return m_Block;
    }

    /*!
       @brief        get stored parent reference 
       @return       (Converter_ParentRefence) parent reference
    */
    Converter_ParentReference GetParentReference() const {
        return m_ParentReference;
    }

private:

    IOMan_BlockAddress        m_Block;
    Converter_ParentReference m_ParentReference;

};


#endif  /* CONVERTER_LEAFREFERENCE_HPP */
