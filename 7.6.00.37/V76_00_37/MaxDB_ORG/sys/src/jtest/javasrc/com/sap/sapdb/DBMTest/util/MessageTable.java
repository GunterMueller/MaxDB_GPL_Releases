package com.sap.sapdb.DBMTest.util;
/**
  -----------------------------------------------------------------------------
  module: DBMAnswerTable.java
  -----------------------------------------------------------------------------


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



*/

import java.util.StringTokenizer;

public class MessageTable {

  public MessageTable() {
  }
  private String message = null;
  private String[] rows = null;
  private String[][] cells = null;
  private int rowCount = 0;
  private int colCount = 0;
  public String getMessage() {
    return message;
  }
  public void setMessage(String aMessage) {

      message = aMessage;
      rows = null;
      cells = null;
      rowCount = 0;
      colCount = 0;

      if ( message == null ) {
          message = "";
      }

      // count rows and cols count
      StringTokenizer stRows = new StringTokenizer(message,"\n");
      StringTokenizer stCols = null;
      while (stRows.hasMoreTokens()) {
         rowCount ++;
         stCols = new StringTokenizer(stRows.nextToken(),"\t");
         while (stCols.hasMoreTokens()) {
            colCount ++;
            stCols.nextToken();
         }
      }
      // fill the array
      if ( ( rowCount > 0 ) && ( colCount > 0 ) ) {
         rows = new String[ rowCount ];
         cells = new String[ rowCount ][colCount];
         stRows = new StringTokenizer(message,"\n");
         int r = 0;
         while (stRows.hasMoreTokens()) {
            rows[ r ] = stRows.nextToken();
            stCols = new StringTokenizer( rows[ r ],"\t");
            int c = 0;
            while (stCols.hasMoreTokens()) {
               cells[r][c++] = stCols.nextToken();
            }
            r++;
         }
      }
  }
  public void setRowCount(int rowCount) {
    this.rowCount = rowCount;
  }
  public int getRowCount() {
    return rowCount;
  }
  public void setColCount(int colCount) {
    this.colCount = colCount;
  }
  public int getColCount() {
    return colCount;
  }

  public String getRow( int row ) {
      if (( row >=0 ) && ( row < getRowCount() )) {
          return rows[row];
      }
      return null;
  }

  public String getCell( int row, int col ) {
      if (( row >=0 ) && ( row < getRowCount() ) && ( col >=0 ) && ( row < getColCount() )) {
          return cells[row][col];
      }
      return null;
  }

}