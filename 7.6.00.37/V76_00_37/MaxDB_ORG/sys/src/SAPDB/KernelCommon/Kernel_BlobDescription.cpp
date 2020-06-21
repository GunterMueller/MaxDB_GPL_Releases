/*!
  @file           Kernel_BlobDescription.cpp
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

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "KernelCommon/Kernel_BlobDescription.hpp"

bool Kernel_BlobDescription::AddColumn(
    ColumnNumber columnId,
    SAPDB_UInt2  maxInlineLength)
{
    if ( columnId.IsInvalid() )
        return false;
    if (m_descriptions.IsFull()) {
        m_descriptions.Reserve( m_descriptions.GetSize() + 1 );
    }
    return m_descriptions.InsertEnd(
        ColumnDescription(columnId,maxInlineLength) );
}

/**********************************************************************/

Kernel_BlobDescription::ColumnNumber 
Kernel_BlobDescription::GetColumn(ColumnNumber columnId) const
{
    ColumnNumber retColumnId;
    
    retColumnId.Invalidate();
    if ( columnId.IsInvalid() )
        return retColumnId;
    
    ColumnDescriptions::ConstIterator it = m_descriptions.Begin();
    while (it != m_descriptions.End()) {
        if (columnId == (*it).columnId) {
            retColumnId = (*it).columnId;
            break;
        }
        ++it;
    }
    return retColumnId;
}        
