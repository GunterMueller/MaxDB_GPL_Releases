/*!***************************************************************************

  module      : Catalog_TableStatisticsObject.cpp

  -------------------------------------------------------------------------

  responsible : ThomasA

  special area: Catalog Manager
  description : Catalog Table Statistics Object

  last changed: 2000-12-06  19:04
  see also    : example.html ...
  first created:2000-03-09  18:21

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG




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




*****************************************************************************/


#include "SQLManager/Catalog/Catalog_TableStatisticsObject.hpp"

Catalog_TableStatisticsObject::Catalog_Key::Catalog_Key (const Catalog_ObjectId& TableId)
: Catalog_Object::Catalog_Key()
{
    SetSurrogate(TableId);
    SetEntryType(TYPE_TABLESTATISTICS);
    slinkage()[1] = 1;
}

//-----------------------------------------------------------------------------

 void Catalog_TableStatisticsObject::SetColumnStatistics (int ColNo, SAPDB_Int4 DistinctValues)
 {
 }

//-----------------------------------------------------------------------------

 void Catalog_TableStatisticsObject::SetRowCount (SAPDB_Int4 RowCount)
 {
     sti_rows = RowCount; 
 }

 //-----------------------------------------------------------------------------

void Catalog_TableStatisticsObject::SetPageCount (SAPDB_Int4 PageCount)
 {
     sti_pages = PageCount;
 }

 //-----------------------------------------------------------------------------

 void Catalog_TableStatisticsObject::SetTimestamp()
 {
     Catalog_Timestamp ts;
     ts.ToInt (sti_date, sti_time);
 }
