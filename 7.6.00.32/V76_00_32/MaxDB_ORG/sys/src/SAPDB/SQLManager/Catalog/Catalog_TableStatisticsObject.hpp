/*!
  @file           Catalog_TableStatisticsObject.hpp
  @author         ThomasA
  @brief          transient representation of table statistics 
  @ingroup        Catalog

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

#ifndef CATALOG_TABLESTATISTICSOBJECT_HPP
#define CATALOG_TABLESTATISTICSOBJECT_HPP

#include "SQLManager/Catalog/Catalog_Types.hpp"
#include "SQLManager/Catalog/Catalog_Object.hpp"

class Catalog_TableStatisticsObject : public Catalog_TableStatisticsObjectPascal
{
public :
    /*!
       @brief key in the catalog of a record decribing a table statistics object
     */
    class Catalog_Key : public Catalog_Object::Catalog_Key
    {
    public :
        /*!
           @brief constructs the key of an table statistics  object
         */
        Catalog_Key(const Catalog_ObjectId& TableId);
    };
    /*!
      @brief stores the number of distinct values of the column identified by ColNo (external column number).
     */
    void SetColumnStatistics (int ColNo, SAPDB_Int4 DistinctValues);
    /*!
      @brief stores the number of rows stored in the table
     */
    void SetRowCount (SAPDB_Int4 RowCount);
    /*!
       @brief stores the number of pages occupied by the table
     */
    void SetPageCount (SAPDB_Int4 PageCount);
    /*!
      @brief writes the current date and time into the object
     */
    void SetTimestamp();
};

#endif