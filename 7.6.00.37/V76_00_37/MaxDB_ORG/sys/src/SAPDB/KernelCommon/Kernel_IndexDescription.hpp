/*!
  @file           Kernel_IndexDescription.hpp
  @author         UweH
  @brief          This offers s list of index descriptions needed by the table interface.

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


#ifndef Kernel_IndexDescription_HPP
#define Kernel_IndexDescription_HPP

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/SAPDB_EnhancedTypes.hpp"
#include "Container/Container_Vector.hpp"

class SAPDBMem_IRawAllocator;

/*!
  @class     Kernel_IndexDescription
  @brief     This simple list holds descriptions of index columns.
 */
class Kernel_IndexDescription
{
public:
    /// defines IndexNumber with an invalid value of max_uint4
    typedef SAPDB_Invalidatable<SAPDB_UInt4, SAPDB_MAX_UINT4> IndexNumber;
    /// defines ColumnNumber with an invalid value of max_uint4
    typedef SAPDB_Invalidatable<SAPDB_UInt2, SAPDB_MAX_UINT2> ColumnNumber;
    /// defines SortOrder which can be ascending or descending.
    enum SortOrder { ascending, descending, userdefined };
public:
    /*!
        @brief Constructor for the list.
        @param allocator [in/out]
        @param indexId   [in]
        @param udfIdent  [in] default = 0 identifier for user defined function used to build index key
        @param isUnique  [in] default is false
     */
    Kernel_IndexDescription(SAPDBMem_IRawAllocator &allocator,
                            const IndexNumber       indexId,
                            const SAPDB_UInt8       udfIdent = 0,
                            const bool              isUnique = false )
    : m_Allocator   (allocator),
      m_descriptions(allocator),
      m_IndexId     (indexId),
      m_IsUnique    (isUnique),
      m_udfIdent    (udfIdent)
    {};

    /*!
        @brief This adds a new column descrption to the list.
        @param columnId   [in]
        @param sortOrder  [in] default is ascending
     */
    bool AddColumn  (ColumnNumber columnId,
                     SortOrder    sortOrder = ascending);

    /*!
     * @brief Returns id of index
     */
    IndexNumber GetIndexId() const;

    /*!
     * @brief Reserves space for at least NewCapacity index columns.
     *
     * @return True on success, false if memory could not be allocated.
     */
    SAPDB_Bool Reserve(SAPDB_Int capacity);

    /*!
     * @brief Removes all columns from the description.
     */
    void Clear();

    /*!
     * @brief Returns true if index is unique.
     */
    bool IsUnique() const;

    /*!
     * @brief Returns table column number corresponding to
     *        indexColumnId'th index column (1 being the first column).
     *
     * @param indexColumnId [in] index column of interest
     *
     * @return corresponding column number in table
     */
    SAPDB_UInt2 GetTableColumnId(const ColumnNumber indexColumnId) const;

    /*!
     * @brief Returns sort order of indexColumnId'th index column
     *        (1 being the first column).
     *
     * @param indexColumnId [in] index column of interest
     *
     * @return sort order of column (ascending or descending)
     */
    SortOrder GetColumnSortOrder(const ColumnNumber indexColumnId) const;

    /*!
     * @brief Returns number of columns in this index.
     */
    SAPDB_Int GetColumnCount() const;

    /// This explicitly defines this index as unique
    void SetUnique(bool isUnique);
    
private:
    /// This describes one column.
    struct ColumnDescription
    {
        /// The id of the column.
        ColumnNumber columnId;
        /// The sort order of the column.
        SortOrder    sortOrder;
        /// This constructs a column
        ColumnDescription(ColumnNumber newColumnId,
                          SortOrder    newSortOrder = ascending)
        {
            columnId  = newColumnId;
            sortOrder = newSortOrder;
        }
    };
    /// defines a list of descriptions
    typedef Container_Vector<ColumnDescription> ColumnDescriptions;
private:
    /// the internal allocator
    SAPDBMem_IRawAllocator &m_Allocator;
    /// the list of descriptions
    ColumnDescriptions      m_descriptions;
    /// The number of the current index.
    IndexNumber             m_IndexId;
    /// This defines if the column values must be unique.
    bool                    m_IsUnique;
    /// This identifies the UDF which is used to build the secondary key.
    SAPDB_UInt8             m_udfIdent;
};

/*************************************************************************/

inline Kernel_IndexDescription::IndexNumber
Kernel_IndexDescription::GetIndexId() const
{
    return m_IndexId;
}

/*************************************************************************/

inline SAPDB_Bool Kernel_IndexDescription::Reserve(SAPDB_Int capacity)
{
    return m_descriptions.Reserve( capacity );
}

/*************************************************************************/

inline void Kernel_IndexDescription::Clear()
{
    m_descriptions.Clear();
}

/*************************************************************************/

inline bool Kernel_IndexDescription::IsUnique() const
{
    return m_IsUnique;
}

/*************************************************************************/

inline SAPDB_Int Kernel_IndexDescription::GetColumnCount() const
{
    return m_descriptions.GetSize();
}

/*************************************************************************/
inline void Kernel_IndexDescription::SetUnique(bool isUnique)
{
    m_IsUnique = isUnique;
}

#endif // Kernel_IndexDescription_HPP
