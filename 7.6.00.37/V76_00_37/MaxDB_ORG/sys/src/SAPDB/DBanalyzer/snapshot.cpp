/*!
  -----------------------------------------------------------------------------
  module: snapshot.cpp
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

#include "SAPDB/DBanalyzer/snapshot.hpp"
#include "SAPDB/DBanalyzer/error.hpp"

extern DBA_error gError;

DBA_snapshot::DBA_snapshot () 
{
  m_nColumns = 0;
  m_nRows    = 0;

  m_nCurrentCol = 0;
  m_nCurrentRow = 0;
}

DBA_snapshot::~DBA_snapshot () 
{
}

DBA_void DBA_snapshot::Clear()
{
  DBA_int nColumn;
  DBA_int nRow;

  for (nRow = 0; nRow < m_nRows; nRow++) {
    if (m_aRows[nRow]) {
      for (nColumn = 0; nColumn < m_nColumns; nColumn++) {
        if ((*m_aRows[nRow])[nColumn]) {
          delete (*m_aRows[nRow])[nColumn];
        }
      }
      delete m_aRows[nRow];
    }
  }

  for (nColumn = 0; nColumn < m_nColumns; nColumn++) {
    if (m_aColumns[nColumn]) {
      delete m_aColumns[nColumn];
    }
  }

  m_nColumns = 0;
  m_nRows    = 0;

  m_nCurrentCol = 0;
  m_nCurrentRow = 0;
}

DBA_int  DBA_snapshot::GetColumnCount()
{
  return m_nColumns;
}

DBA_int DBA_snapshot::GetRowCount()
{
  return m_nRows;
}

DBA_bool DBA_snapshot::AddColumn(const DBA_string & sName)
{
  DBA_snapshot_column * pColumn = new DBA_snapshot_column;
  if (!pColumn) {
    gError.SetError(DBA_ERR_NOMEM);
    return false;
  } 

  pColumn->sName = sName;
  m_aColumns[m_nColumns] = pColumn;
  m_nColumns++;

  return true;
}

DBA_bool DBA_snapshot::AddRow()
{
  DBA_snapshot_field_array * pRow = new DBA_snapshot_field_array;
  if (!pRow) {
    gError.SetError(DBA_ERR_NOMEM);
    return false;
  }

  m_aRows[m_nRows] = pRow;

  m_nCurrentCol = 0;
  m_nCurrentRow = m_nRows;

  m_nRows++;

  return true;
}

DBA_bool DBA_snapshot::AddField(const DBA_string & sValue)
{
  DBA_snapshot_field * pField = new DBA_snapshot_field;
  if (!pField) {
    gError.SetError(DBA_ERR_NOMEM);
    return false;
  } 

  pField->sValue = sValue;

  if (m_aRows[m_nCurrentRow]) {
    (*m_aRows[m_nCurrentRow])[m_nCurrentCol] = pField;
    m_nCurrentCol++;
  }

  return true;
}

DBA_bool DBA_snapshot::Empty()
{
  return ((m_nColumns <= 0) || (m_nRows <= 0));
}

//
// GetFieldByCondition
//
// get a field by a condition
// if no condition is present then look for the 
// given column in row 0
//
DBA_string DBA_snapshot::GetFieldByCondition(const DBA_string & sConditionColumn,
                                             const DBA_string & sConditionValueIn,
                                             const DBA_string & sColumn)
{
  if (!sConditionColumn.Empty()) {
    DBA_string sConditionValue;
    if ((sConditionValueIn[0] == '\'') &&
        (sConditionValueIn[sConditionValueIn.Length()-1] == '\'')) {
      sConditionValue = sConditionValueIn.SubStr(1, sConditionValueIn.Length()-2);
    } else {
      sConditionValue = sConditionValueIn;
    } // end if

    DBA_int nConditionColumn = GetColumnByName(sConditionColumn);
    if (nConditionColumn >= 0) {
      DBA_int nColumn = GetColumnByName(sColumn);
      if (nColumn >= 0) {
        DBA_int nRow;
        for(nRow = 0; nRow < m_nRows; nRow++) {
          if (GetField(nConditionColumn, nRow) == sConditionValue) {
            return GetField(nColumn, nRow);
          }
        }
      }
    } 
  } else {
    DBA_int nColumn = GetColumnByName(sColumn);
    if (nColumn >= 0) {
      return GetField(nColumn, 0);
    }
  }

  return DBA_SNAPSHOT_NOTEXIST;
}

DBA_int DBA_snapshot::GetColumnByName(const DBA_string sName) {
  DBA_int nColumn;

  for(nColumn = 0; nColumn < m_nColumns; nColumn++) {
    if (m_aColumns[nColumn]->sName == sName) {
      return nColumn;
    }
  }

  return -1;
}

DBA_string DBA_snapshot::GetColumn(DBA_int nColumn)
{
  if (nColumn < m_nColumns) {
    return m_aColumns[nColumn]->sName;
  } 
  return DBA_string("");
}

DBA_string DBA_snapshot::GetField(DBA_int nColumn, DBA_int nRow)
{
  if ((nColumn < m_nColumns) && (nRow < m_nRows) && 
      (nColumn >= 0        ) && (nRow >= 0     )    ) {
    return (*m_aRows[nRow])[nColumn]->sValue;
  } 
  return DBA_string("");
}

