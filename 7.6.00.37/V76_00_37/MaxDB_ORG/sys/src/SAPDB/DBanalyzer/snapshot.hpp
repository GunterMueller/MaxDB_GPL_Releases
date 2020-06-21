/*!
  -----------------------------------------------------------------------------
  module: snapshot.hpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: DBanalyzer - The performance analyse tool for SAPDB
  description:  snapshot form odbc results
  version:      7.4 and higher 
  -----------------------------------------------------------------------------

  Copyright (c) 2002-2005 SAP AG


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




  -----------------------------------------------------------------------------
*/

#ifndef _SNAPSHOT_HPP_
#define _SNAPSHOT_HPP_

#include "SAPDB/ToolsCommon/Tools_Array.hpp"
#include "SAPDB/DBanalyzer/types.hpp"

#define DBA_SNAPSHOT_NULLVAL  "?"
#define DBA_SNAPSHOT_NOTEXIST "??"

// array of columns
typedef struct {
  DBA_string sName;
} DBA_snapshot_column;

typedef Tools_Array<DBA_snapshot_column*> DBA_snapshot_column_array;

// array of fields (row)
typedef struct {
  DBA_string sValue;
} DBA_snapshot_field;

typedef Tools_Array<DBA_snapshot_field*> DBA_snapshot_field_array;

// array of field arrays (rows)
typedef Tools_Array<DBA_snapshot_field_array*> DBA_snapshot_row_array;

class DBA_snapshot {
  public:
  DBA_snapshot();
  ~DBA_snapshot();

  DBA_void Clear();
  DBA_int  GetColumnCount();
  DBA_int  GetRowCount();

  DBA_bool AddColumn(const DBA_string & sName);
  DBA_bool AddRow();
  DBA_bool AddField(const DBA_string & sValue);

  DBA_bool Empty();

  DBA_int GetColumnByName(const DBA_string sName);
  DBA_string GetFieldByCondition(const DBA_string & sConditionColumn,
                                 const DBA_string & sConditionValue,
                                 const DBA_string & sColumn);

  DBA_string GetColumn(DBA_int nColumn);
  DBA_string GetField(DBA_int nColumn, DBA_int nRow);

  private:
  DBA_snapshot_column_array  m_aColumns;
  DBA_snapshot_row_array     m_aRows;

  DBA_int m_nColumns;
  DBA_int m_nRows;

  DBA_int m_nCurrentRow;
  DBA_int m_nCurrentCol;
};



#endif 