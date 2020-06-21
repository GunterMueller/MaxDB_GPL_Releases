package com.sap.sapdb.DBMTest.util;

/**
  -----------------------------------------------------------------------------
  module: DBMAnswerTable.java
  -----------------------------------------------------------------------------


    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
import java.io.StreamTokenizer;
import java.io.StringReader;

/**
 * Usefull functions for the DBMServer test.
 * <xmp>
 * -----------------------------------------------------------------------------
 * Class:        DBMAnswerTable
 * -----------------------------------------------------------------------------
 * responsible:  BerndV
 * special area: DBMServer Java Test
 * description:  Usefull functions for the DBMServer test.
 * version:      7.2.*
 * Copyright (c) 1998-2005 SAP AG
 * -----------------------------------------------------------------------------
 * </xmp>
 */
public class DBMAnswerTable
{
  String []   m_szColumnNames;
  String [][] m_szFields;

  /**
   * Constructor.
   * <xmp>
   * -------------------------------------------------------------------------
   * function:     DBMAnswerTable
   * -------------------------------------------------------------------------
   * description:  constructor
   * arguments:    szAnswer [IN] - the DBMServer Answer
   * -------------------------------------------------------------------------
   * </xmp>
   */
  public DBMAnswerTable ()
  {

  } // end DBMAnswerTable

  /**
   * DBMServer answer analysis.
   * <xmp>
   * -------------------------------------------------------------------------
   * function:     analyzeFixColsByHeader
   * -------------------------------------------------------------------------
   * description:  DBMServer answer analysis
   * arguments:    szAnswer [IN] - the DBMServer Answer
   * return value: could analyze or not
   * -------------------------------------------------------------------------
   * </xmp>
   */
  public boolean analyzeFixColsByHeader(String szAnswer)
  {
    m_szColumnNames = null;
    m_szFields      = null;

    String [] szLines = getLines(szAnswer);

    if (szLines.length > 0)
    {
      int [] nColumnPos;

      // Step 1 - read column header and column pos
      m_szColumnNames = getColumns(szLines[0]);
      nColumnPos      = getColumnPos(szLines[0], m_szColumnNames);

      if (szLines.length > 1)
      {
        // Step 2 - read content
        m_szFields = new String[szLines.length - 1][];

        int i;
        for (i = 1; i < szLines.length; ++i)
        {
          m_szFields[i - 1] = getColumns(szLines[i], nColumnPos);
        } // end for
      } // end if
    } // end if

    if ((m_szColumnNames == null) || (m_szFields == null))
    {
      m_szColumnNames = null;
      m_szFields      = null;
    } // end if

    return ((m_szColumnNames != null) && (m_szFields != null));
  } // end analyzeFixColsByHeader

  /**
   * DBMServer answer analysis.
   * <xmp>
   * -------------------------------------------------------------------------
   * function:     analyzeColsByFieldSeparator
   * -------------------------------------------------------------------------
   * description:  DBMServer answer analysis
   * arguments:    szAnswer [IN]   - the DBMServer Answer
                   nSeparator [IN] - the field separator
   * return value: could analyze or not
   * -------------------------------------------------------------------------
   * </xmp>
   */
  public boolean analyzeColsByFieldSeparator(String  szAnswer, int nSeparator)
  {
    m_szColumnNames = null;
    m_szFields      = null;

    String [] szLines = getLines(szAnswer);

    if (szLines.length > 0)
    {
      // Step 1 - read content
      m_szFields = new String[szLines.length][];

      int i;
      for (i = 0; i < szLines.length; ++i)
      {
        m_szFields[i] = getColumns(szLines[i], nSeparator);
      } // end for
    } // end if

    return (m_szFields != null);
  } // end analyzeColsByFieldSeparator

  /**
   * DBMServer answer analysis.
   * <xmp>
   * -------------------------------------------------------------------------
   * function:     analyzeColsByFixColumnWidth
   * -------------------------------------------------------------------------
   * description:  DBMServer answer analysis
   * arguments:    szAnswer [IN]   - the DBMServer Answer
                   nSeparator [IN] - the field separator
   * return value: could analyze or not
   * -------------------------------------------------------------------------
   * </xmp>
   */
  public boolean analyzeColsByFixColumnWidth(String  szAnswer, int [] nColumnPos)
  {
    m_szColumnNames = null;
    m_szFields      = null;

    String [] szLines = getLines(szAnswer);

    if (szLines.length > 0)
    {
      // read content
      m_szFields = new String[szLines.length][];

      int i;
      for (i = 0; i < szLines.length; ++i)
      {
        m_szFields[i] = getColumns(szLines[i], nColumnPos);
      } // end for
    } // end if

    return (m_szFields != null);
  } // end analyzeColsByFieldSeparator

  /**
   * Count of columns.
   * <xmp>
   * -------------------------------------------------------------------------
   * function:     getColumnCount
   * -------------------------------------------------------------------------
   * description:  Count of columns.
   * return value: the columns
   * -------------------------------------------------------------------------
   * </xmp>
   */
  public int getColumnCount()
  {
    int nColumns = 0;

    if (m_szFields != null)
    {
      nColumns = m_szFields[1].length;
    } // end if

   return nColumns;
  } // end getColumnCount

  /**
   * Count of rows.
   * <xmp>
   * -------------------------------------------------------------------------
   * function:     getRowCount
   * -------------------------------------------------------------------------
   * description:  Count of rows.
   * return value: the rows
   * -------------------------------------------------------------------------
   * </xmp>
   */
  public int getRowCount()
  {
    int nRows = 0;

    if (m_szFields != null)
    {
       nRows = m_szFields.length;
    } // end if

    return nRows;
  } // end getRowCount

  /**
   * Get a whole row.
   * <xmp>
   * -------------------------------------------------------------------------
   * function:     getRow
   * -------------------------------------------------------------------------
   * description:  Get a whole row
   * arguments:    nRow [IN] the requested row
   * return value: array of columns of the requested row (check for null!)
   * -------------------------------------------------------------------------
   * </xmp>
   */
  public String [] getRow(int nRow)
  {
    String [] szRow = null;

    if (m_szFields != null) {
      if (nRow >= 0 && nRow < m_szFields.length)
      {
        szRow = m_szFields[nRow];
      } // end if
    } // end if

    return szRow;
  } // end getRow

  /**
   * Get the header row.
   * <xmp>
   * -------------------------------------------------------------------------
   * function:     getHeaderRow
   * -------------------------------------------------------------------------
   * description:  Get a the header
   * return value: array of columnnames
   * -------------------------------------------------------------------------
   * </xmp>
   */
  public String [] getHeaderRow()
  {
    return m_szColumnNames;
  } // end getHeaderRow

  /**
   * Get a field.
   * <xmp>
   * -------------------------------------------------------------------------
   * function:     getField
   * -------------------------------------------------------------------------
   * description:  Get a field by row number and column number
   * arguments:    nRow    [IN] the requested row
   *               nColumn [IN] the requeted column
   * return value: the field as string
   * -------------------------------------------------------------------------
   * </xmp>
   */
  public String getField(int nRow, int nColumn)
  {
    String szField = "";

    if (m_szFields != null)
    {
      if (nRow >= 0 && nRow < m_szFields.length)
      {
        if (nColumn >= 0 && nColumn < m_szFields[nRow].length)
        {
          szField = m_szFields[nRow][nColumn];
        } // end if
      } // end if
    } // end if

    return szField;
  } // end getField

  /**
   * Get a field.
   * <xmp>
   * -------------------------------------------------------------------------
   * function:     getField
   * -------------------------------------------------------------------------
   * description:  Get a field by row number and column name
   * arguments:    nRow     [IN] the requested row
   *               szColumn [IN] name/title of the requested column
   * return value: the field as string
   * -------------------------------------------------------------------------
   * </xmp>
   */
  public String getField(int nRow, String szColumn)
  {
    return getField(nRow, indexOfColumn(szColumn));
  } // end getField

  /**
   * Get a field.
   * <xmp>
   * -------------------------------------------------------------------------
   * function:     getField
   * -------------------------------------------------------------------------
   * description:  Get a field by a column value and a column number
   * arguments:    szRowValue   [IN] the value for the requested row
   *               nValueColumn [IN] the column for the row value
   *               nColumn      [IN] the requested column
   * return value: the field as string
   * -------------------------------------------------------------------------
   * </xmp>
   */
  public String getField(String szRowValue, int nValueColumn, int nColumn)
  {
    return getField(indexOfRow(szRowValue, nValueColumn), nColumn);
  } // end getField

  /**
   * Get a field.
   * <xmp>
   * -------------------------------------------------------------------------
   * function:     getField
   * -------------------------------------------------------------------------
   * description:  Get a field by a column value and a column name
   * arguments:    szRowValue   [IN] the value for the requested row
   *               nValueColumn [IN] the column for the row value
   *               szColumn     [IN] name/title of the requested column
   * return value: the field as string
   * -------------------------------------------------------------------------
   * </xmp>
   */
  public String getField(String szValue, int nValueColumn, String szColumn)
  {
    return getField(indexOfRow(szValue, nValueColumn), indexOfColumn(szColumn));
  } // end getField

  /**
   * Get a field.
   * <xmp>
   * -------------------------------------------------------------------------
   * function:     getField
   * -------------------------------------------------------------------------
   * description:  Get a field by a column value and a column name
   * arguments:    szRowValue    [IN] the value for the requested row
   *               szValueColumn [IN] name/title of the column for the row value
   *               szColumn      [IN] name/title of the requested column
   * return value: the field as string
   * -------------------------------------------------------------------------
   * </xmp>
   */
  public String getField(String szValue, String szValueColumn, String szColumn)
  {
    return getField(indexOfRow(szValue, indexOfColumn(szValueColumn)), indexOfColumn(szColumn));
  } // end getField

  /**
   * Get a field.
   * <xmp>
   * -------------------------------------------------------------------------
   * function:     getField
   * -------------------------------------------------------------------------
   * description:  Get a field by a column value and a column name
   * arguments:    szRowValue    [IN] the value for the requested row
   *               szValueColumn [IN] name/title of the column for the row value
   *               nColumn       [IN] the requested column
   * return value: the field as string
   * -------------------------------------------------------------------------
   * </xmp>
   */
  public String getField(String szValue, String szValueColumn,int nColumn)
  {
    return getField(indexOfRow(szValue, indexOfColumn(szValueColumn)), nColumn);
  } // end getField

  /* -------------------------------------------------------------------------
   * function:     getLines
   * -------------------------------------------------------------------------
   */
  private static String [] getLines(String szBuffer)
  {
    StringReader    oReader    = new StringReader(szBuffer);
    StreamTokenizer oTokenizer = new StreamTokenizer(oReader);
    int             nLines     = getLineCount(szBuffer);
    int             nCurrent   = 0;
    String []       szLines    = new String[nLines];

    oTokenizer.resetSyntax();
    oTokenizer.wordChars(0, 255);
    oTokenizer.whitespaceChars('\n', '\n');

    try
    {
      while (oTokenizer.nextToken() != StreamTokenizer.TT_EOF)
      {
        szLines[nCurrent] = oTokenizer.sval.trim();
        ++nCurrent;
      } // end while
    }
    catch (Exception oException)
    {
      // do nothing
    } // end try-catch

    return  szLines;
  } // end function getLines

  /* -------------------------------------------------------------------------
   * function:     getLineCount
   * -------------------------------------------------------------------------
   */
  private static int getLineCount(String szBuffer)
  {
    StringReader    oReader    = new StringReader(szBuffer);
    StreamTokenizer oTokenizer = new StreamTokenizer(oReader);
    int             nLines     = 0;

    oTokenizer.resetSyntax();
    oTokenizer.wordChars(0, 255);
    oTokenizer.whitespaceChars('\n', '\n');

    try {
      while (oTokenizer.nextToken() != StreamTokenizer.TT_EOF)
      {
        ++nLines;
      }  // end while
    }
    catch (Exception oException)
    {
      // do nothing
    } // end try-catch

    return  nLines;
  } // end function getLineCount

  /* -------------------------------------------------------------------------
   * function:     getColumns
   * -------------------------------------------------------------------------
   */
  private static String [] getColumns(String szLine)
  {
    return getColumns(szLine, 0);
  } // end getColumns
  /* -------------------------------------------------------------------------
   */
  private static String [] getColumns(String szLine, int nSeparator)
  {
    StringReader    oReader    = new StringReader(szLine);
    StreamTokenizer oTokenizer = new StreamTokenizer(oReader);
    int             nColumns   = getColumnCount(szLine, nSeparator);
    int             nCurrent   = 0;
    String []       szColumns  = new String[nColumns];

    oTokenizer.resetSyntax();
    oTokenizer.wordChars(0, 255);
    if (nSeparator == 0)
    {
      // use whitespace
      oTokenizer.whitespaceChars(' ', ' ');
      oTokenizer.whitespaceChars('\t', '\t');
    }
    else
    {
      // use separator
      oTokenizer.whitespaceChars(nSeparator, nSeparator);
    } // end if

    try {
      while (oTokenizer.nextToken() != StreamTokenizer.TT_EOF)
      {
        szColumns[nCurrent] = oTokenizer.sval.trim();
        ++nCurrent;
      } // end while
    }
    catch (Exception oException)
    {
      // do nothing
    } // end try-catch

    return  szColumns;
  } // end function getColumns
  /* -------------------------------------------------------------------------
   */
  private static String [] getColumns(String szLine, int [] nColumnPos)
  {
    String [] szColumns = new String [nColumnPos.length];
    int    i         = 0;

    for (i = 0; i < nColumnPos.length; ++i)
    {
      if ((i + 1) < nColumnPos.length)
      {
        if (nColumnPos[i + 1] < szLine.length()) {
          szColumns[i] = szLine.substring(nColumnPos[i], nColumnPos[i + 1]).trim();
        } else if (nColumnPos[i] < szLine.length()) {
          szColumns[i] = szLine.substring(nColumnPos[i]).trim();
        } else {
          szColumns[i] = "";
        } // end if
      }
      else
      {
        if (nColumnPos[i] < szLine.length()) {
          szColumns[i] = szLine.substring(nColumnPos[i]).trim();
        } else {
          szColumns[i] = "";
        } // end if
      } // end if
    } // end for
    return szColumns;
  } // end getColumns

  /* -------------------------------------------------------------------------
   * function:     getColumnCount
   * -------------------------------------------------------------------------
   */
  private static int getColumnCount(String szLine, int nSeparator)
  {
    StringReader    oReader    = new StringReader(szLine );
    StreamTokenizer oTokenizer = new StreamTokenizer(oReader);
    int             nColumns   = 0;

    oTokenizer.resetSyntax();
    oTokenizer.wordChars(0, 255);
    if (nSeparator == 0)
    {
      // use whitespace
      oTokenizer.whitespaceChars(' ', ' ');
      oTokenizer.whitespaceChars('\t', '\t');
    }
    else
    {
      // use separator
      oTokenizer.whitespaceChars(nSeparator, nSeparator);
    } // end if

    try
    {
      while (oTokenizer.nextToken() != StreamTokenizer.TT_EOF)
      {
        ++nColumns;
      }  // end while
    }
    catch (Exception oException)
    {
      // do nothing
    } // end try-catch

    return  nColumns;
  } // end function  getColumnCount

  /* -------------------------------------------------------------------------
   * function:     getColumnPos
   * -------------------------------------------------------------------------
   */
  private static int [] getColumnPos(String szLine, String [] szColumns)
  {
    int [] nColumnPos = new int[szColumns.length];
    int i             = 0;
    int nFromIndex    = 0;

    for (i = 0; i < szColumns.length; ++i)
    {
      nColumnPos   [i]   = szLine.indexOf(szColumns[i], nFromIndex);
      nFromIndex         = nColumnPos[i] + szColumns[i].length();
    } // end for

    return nColumnPos;
  } // end function  getColumnPos

  /* -------------------------------------------------------------------------
   * function:     indexOfColumn
   * -------------------------------------------------------------------------
   */
  private int indexOfColumn(String szColumnName)
  {
    int    nColumn = 0;
    if (m_szColumnNames != null)
    {
      for (nColumn = 0; nColumn < m_szColumnNames.length; ++nColumn)
      {
        if (m_szColumnNames[nColumn].compareToIgnoreCase(szColumnName) == 0) {
          break;
        } // end if
      } // end for
    } // end if
    return nColumn;
  } // end indexOfColumn

  /* -------------------------------------------------------------------------
   * function:     indexOfRow
   * -------------------------------------------------------------------------
   */
  private int indexOfRow(String szValue, int nValueColumn)
  {
    int    nRow    = 0;
    if (m_szFields != null) {
      for (nRow = 0; nRow < m_szFields.length; ++nRow)
      {
        if (szValue.compareToIgnoreCase(getField(nRow, nValueColumn)) == 0)
        {
          break;
        } // end if
      } // end for
    } // end if
    return nRow;
  } // end indexOfRow
  /* -------------------------------------------------------------------------
   */
  private int indexOfRow(String szValue, String szValueColumn)
  {
    return indexOfRow(szValue, indexOfColumn(szValueColumn));
  } // end indexOfRow

} // end class DBMAnswerTable

/** EndClass: DBMAnswerTable */
