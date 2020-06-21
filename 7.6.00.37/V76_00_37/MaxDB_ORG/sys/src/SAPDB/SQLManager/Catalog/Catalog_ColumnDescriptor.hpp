/*!
  @file           Catalog_ColumnDescriptor.hpp
  @author         ThomasA
  @ingroup        Catalog
  @brief          defines a descriptor for a column

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

#ifndef CATALOG_COLUMNDESCRIPTOR_HPP
#define CATALOG_COLUMNDESCRIPTOR_HPP

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SQLManager/SQLMan_Types.hpp"

struct tak00_columninfo;
struct tgg00_Rec;

/*!
   @brief yields the properties of a column 
 */
class Catalog_ColumnDescriptor
{
public :
    /*!
       @brief constructor
     */
    Catalog_ColumnDescriptor(tak00_columninfo* pColumnInfo)
        : m_pColumnInfo(pColumnInfo)
    {
    }
    /*!
       @brief returns the  
     */
    int GetDataLength() const;
    /*!
       @brief returns the maximal physical length of the column
     */
    int GetIOLength() const;
    /*!
       @brief yields the column name
       @param ColumnName [out] the column name 
     */
    void GetColumnName (SQLMan_Identifier& ColumnName) const;
    /*!
       @brief yields the column name
     */
    SQLMan_Identifier GetColumnName () const;
    /*!
       @brief returns the interpreter descriptot of the column (stack code)
     */
    const SQLMan_StackEntry& GetStackCode() const;
    /*!
       @brief returns true, if the given name and the descriptors column name are equal
     */
    bool IsColumnnameEqual (const SQLMan_Identifier& CompareName) const;
    /*!
      @brief returns true, if the column may contain null values
     */
    bool IsNullable() const;
    /*!
       @brief returns true, if the column is a key column
     */
    bool IsKeyColumn() const;
    /*!
       @brief returns true, if the column is part of an index
     */
    bool IsIndexed() const;
    /*!
       @brief returns true, if the column is marked as dropped
     */
    bool IsDropped() const;
    /*!
       @brief returns true, if the column is invisible
     */
    bool IsVisible() const;
    /*!
       @brief returns true, if the column is the system key column
     */
    bool IsSysKey() const;

	bool IsLongDataType() const;
    /*!
       @brief returns the column number in user defined order
     */
    SAPDB_Int2 ExtColNo() const;
    /*!
       @brief returns the extcolno of the corresponding column of the base table
     */
    SAPDB_Int2 RecColNo() const;
    /*!
       @brief returns the column value for a given row of the table containing the column
       @param Row [in] a row of the table containing the column
       @param pValue [out] a pointer to the column value
       @param ValueLength [out] the length of the column value
     */
    void LocateColumnValue (const tgg00_Rec& Row, const SAPDB_Byte* &pValue, int& ValueLength) const;
private :
    tak00_columninfo* m_pColumnInfo; //!< pointer to the column information
};

#endif
