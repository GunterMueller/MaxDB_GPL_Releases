/*!
  @file           Kernel_IndexDescription.cpp
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

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/SAPDB_EnhancedTypes.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"

#include "KernelCommon/Kernel_IndexDescription.hpp"

/*************************************************************************/

SAPDB_Bool Kernel_IndexDescription::AddColumn(
    ColumnNumber columnId,
    SortOrder    sortOrder)
{
    if ( columnId.IsInvalid() )
        return false;
    if (m_descriptions.IsFull()) {
        m_descriptions.Reserve( m_descriptions.GetSize() + 1 );
    }
    return m_descriptions.InsertEnd(
        ColumnDescription(columnId,sortOrder) );
}

/*************************************************************************/

SAPDB_UInt2 Kernel_IndexDescription::GetTableColumnId(
     const ColumnNumber indexColumnId) const
{
    SAPDBERR_ASSERT_ARGUMENT( (indexColumnId >= 1) &&
                              (indexColumnId <= m_descriptions.GetSize()) );

    return m_descriptions[indexColumnId-1].columnId;
}

/*************************************************************************/

Kernel_IndexDescription::SortOrder
Kernel_IndexDescription::GetColumnSortOrder(
    const ColumnNumber indexColumnId) const
{
    SAPDBERR_ASSERT_ARGUMENT( (indexColumnId >= 1) &&
                              (indexColumnId <= m_descriptions.GetSize()) );

    return m_descriptions[indexColumnId-1].sortOrder;
}

/*************************************************************************/
