/*!
  @file           Catalog_TableStatisticsDescriptor.hpp
  @author         ThomasA
  @ingroup        Catalog
  @brief          yields information about table statistics

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

#ifndef CATALOG_TABLESTATISTICSDESCRIPTOR_HPP
#define CATALOG_TABLESTATISTICSDESCRIPTOR_HPP

#include "SQLManager/Catalog/Catalog_Types.hpp"

class Catalog_TableDescriptor;
class Catalog_TableStatisticsObject;
class SQLMan_Context;
class Catalog_TableObject;

/*!
   @brief defines a descriptor which allows to set and get information about table statistics stored in the catalog
 */
class Catalog_TableStatisticsDescriptor
{
public :
    /*!
      @brief default constructor
    */
    Catalog_TableStatisticsDescriptor ();
    /*!
       @brief true, if descriptor is valid; false otherwise
     */
    operator bool() const;
    /*!
       @brief tells whether column statistics for the given column are present
     */
    bool ExistsColumnStatistics (int ColNo) const;
    /*!
    @brief stores the number of distinct values of the column identified by ColNo (external column number).
    */
    bool SetColumnStatistics (int ColNo, SAPDB_Int4 DistinctValues);
     /*!
      @brief stores the number of rows stored in the table
     */
    void SetRowCount (SAPDB_Int4 RowCount);
    /*!
    @brief stores the number of pages occupied by the table
    */
    void SetPageCount (SAPDB_Int4 PageCount);
    /*!
       @brief stores the statistics into the catalog
       @return true, if the statistics have been stored successfully; false if an error occurred. In this
       case the returncode of the context has been set.
     */
    bool Store() const;
    /*!
       @brief destructor
     */
    virtual ~Catalog_TableStatisticsDescriptor();
private :
    /*!
       @brief constructor
     */
    Catalog_TableStatisticsDescriptor (SQLMan_Context& Context, const Catalog_TableObject* pTable, bool LockExclusive); 
    /*!
       @brief copy constructor
     */
    Catalog_TableStatisticsDescriptor (const Catalog_TableStatisticsDescriptor& Source); 
    /*!
       @brief assignment
    */
    void operator= (const Catalog_TableStatisticsDescriptor& Source);


    SQLMan_Context*                 m_pContext;    //!< pointer to the sql manager context
    Catalog_TableStatisticsObject*  m_pStatistics; //!< pointer to the catalog object
    Catalog_ObjectId                m_tableId;     //!< table identifier 
    friend class Catalog_TableDescriptor;
};

#endif