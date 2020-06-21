/*!
  @file           Kernel_BlobDescription.hpp
  @author         UweH
  @brief          This offers s list of blob descriptions needed by
                  the table interface.

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


#ifndef Kernel_BlobDescription_HPP
#define Kernel_BlobDescription_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/SAPDB_EnhancedTypes.hpp"
#include "Container/Container_Vector.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/// This simple list holds descriptions of blob columns.
class Kernel_BlobDescription
{
public:
    /// Defines a blob with an invalid value of max_uint4.
    typedef SAPDB_Invalidatable<SAPDB_UInt2, SAPDB_MAX_UINT2> ColumnNumber;

private:
    /// the description of one blob column
    struct ColumnDescription
    {
        /// the id of the column
        ColumnNumber columnId;

        /// The maximum length. If the column becomes larger it is
        /// written to extra pages.
        SAPDB_UInt2  maxInlineLength;

        /// This contructs one blob column description.
        ColumnDescription(ColumnNumber newColumnId,
                          SAPDB_UInt2  newMaxInlineLength = 0)
        {
            columnId        = newColumnId;
            maxInlineLength = newMaxInlineLength;
        }
    };

    /// Defines a list blob columns.
    typedef Container_Vector<ColumnDescription> ColumnDescriptions;

public:
    /// Size type for descriptions container.
    typedef Container_Vector<ColumnDescription>::SizeType SizeType;

    /// Constructor for the list.
    Kernel_BlobDescription(SAPDBMem_IRawAllocator &allocator)
    : m_Allocator   (allocator),
      m_descriptions(allocator)
    {}

    /*!
     * @brief Constructor for the list.
     * @param allocator   [in] the allocator to use
     * @param initialSize [in] reserve space for initialSize blob columns
     */
    Kernel_BlobDescription(SAPDBMem_IRawAllocator &allocator,
                           SAPDB_Int               initialSize)
    : m_Allocator   (allocator),
      m_descriptions(allocator)
    {
        m_descriptions.Reserve(initialSize);
    }

    /******************************************************************/

    /// Adds a new blob column.
    bool AddColumn  (ColumnNumber columnId,
                     SAPDB_UInt2  maxInlineLength = 0);

    /******************************************************************/

    /*!
     * @brief Returns ColumnNumber for columnId. 
     *
     * @param columnId [in] the column number of the blob column to be
     *        returned
     * @return the internal column number of columnId; if columnId
     *         could not be found, ColumnNumber is Invalid.
     */
    ColumnNumber GetColumn(ColumnNumber columnId) const;

    /******************************************************************/

    /*!
     * @brief Returns the number of blob columns in this description.
     */
    SAPDB_Int GetColumnCount() const
    {
        return m_descriptions.GetSize();
    }

    /******************************************************************/

    /*!
     * @brief Removes all columns from the description.
     */
    void Clear()
    {
        m_descriptions.Clear();
    }

    /******************************************************************/

    /*!
     * @brief Reserve space for at least NewCapacity blob columns.
     *
     * @return True on success, false if memory could not be allocated.
     */
    SAPDB_Bool Reserve(SizeType NewCapacity)
    {
        return m_descriptions.Reserve(NewCapacity);
    }

private:
    /// The internally used allocator.
    SAPDBMem_IRawAllocator &m_Allocator;

    /// The list itself of all blob columns.
    ColumnDescriptions      m_descriptions;
};
#endif // Kernel_BlobDescription_HPP
