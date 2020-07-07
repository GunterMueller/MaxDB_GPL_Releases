/*!
  -----------------------------------------------------------------------------

  module: Tools_TemplateSimpleTable.cpp

  -----------------------------------------------------------------------------

  responsible:  BerndV

  special area: universal stuff for web access

  description:  a class for creating simple Tables in SAP MiniApps style

  version:      7.3 and higher
  -----------------------------------------------------------------------------

                          Copyright (c) 2000-2005 SAP AG

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

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
#include <string.h>
#include "ToolsCommon/Tools_TemplateSimpleTable.hpp"

/*
  -----------------------------------------------------------------------------
  private macros
  -----------------------------------------------------------------------------
 */
#define TEMPL_TABLE            "SAPDBSimpleTable.htm"

#define OBJ_HEADLINE           "Headline"

#define OBJ_LOGO               "Logo"
#define OBJ_GROUP              "Groupbox"
#define OBJ_SIMPLE             "SimpleTable"

#define OBJ_TABLETITLEROW      "TableTitleRow"
#define OBJ_TABLETITLECOL      "TableTitleColumn"

#define OBJ_COLUMNTITLEROW     "ColumnTitleRow"
#define OBJ_COLUMNTITLECOL     "ColumnTitleColumn"

#define OBJ_ROWPAIR            "RowPair"

#define OBJ_EVENROWROW         "EvenRowRow"
#define OBJ_ODDROWROW          "OddRowRow"

#define OBJ_EVENROWCOL         "EvenRowColumn"
#define OBJ_ODDROWCOL          "OddRowColumn"

#define OBJ_COLUMNSUMMARYROW   "ColumnSummaryRow"
#define OBJ_COLUMNSUMMARYCOL   "ColumnSummaryColumn"

#define OBJ_BUTTONBAR          "ButtonBar"
#define OBJ_BUTTON             "Button"
 
/*
  =============================================================================
  class:        Tools_TemplateSimpleTable
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor Tools_TemplateSimpleTable :: Tools_TemplateSimpleTable
  -----------------------------------------------------------------------------
 */
Tools_TemplateSimpleTable  :: Tools_TemplateSimpleTable( sapdbwa_WebAgent & wa )
                            : Tools_Template ( wa, _Tools_UTF8Ptr(TEMPL_TABLE) ) 
{

  m_nMaxRow = 0;
  m_nMaxCol = 0;

  m_nCurrentCol = 0;
  m_nCurrentRow = 0;

  m_nMaxButton     = 0;
  m_nCurrentButton = 0;

  m_bTitleRow    = false;
  m_bSummaryRow  = false;

  m_bLogo        = false;
  m_bGroup       = false;

} // end Tools_TemplateSimpleTable :: Tools_TemplateSimpleTable

/*
  -----------------------------------------------------------------------------
  public destructor Tools_TemplateSimpleTable :: ~Tools_TemplateSimpleTable
  -----------------------------------------------------------------------------
 */
Tools_TemplateSimpleTable  :: ~Tools_TemplateSimpleTable (  )
{

} // end Tools_TemplateSimpleTable :: ~Tools_TemplateSimpleTable

/*
  -----------------------------------------------------------------------------
  public function Tools_TemplateSimpleTable :: setHeadLine
  -----------------------------------------------------------------------------
 */
Tools_TemplateSimpleTable & Tools_TemplateSimpleTable :: setHeadLine ( const SAPDB_UTF8 * szHeadLine )
{
  m_strHeader = szHeadLine;
  return *this;
} // end Tools_TemplateSimpleTable :: setHeadLine

/*
  -----------------------------------------------------------------------------
  public function Tools_TemplateSimpleTable :: setTableTitle
  -----------------------------------------------------------------------------
 */
Tools_TemplateSimpleTable & Tools_TemplateSimpleTable :: setTableTitle ( const SAPDB_UTF8 * szTableTitle)
{
  m_strTableTitle = szTableTitle;
  return *this;
} // end Tools_TemplateSimpleTable :: setHeadLine

/*
  -----------------------------------------------------------------------------
  public function Tools_TemplateSimpleTable :: setColumnTitle
  -----------------------------------------------------------------------------
 */
Tools_TemplateSimpleTable & Tools_TemplateSimpleTable :: setColumnTitle ( const SAPDB_UTF8 * szColumnTitle,
                                                                          const int          nColumn)
{
  m_ColumnTitleMap[nColumn] = szColumnTitle;

  m_nMaxCol = (m_nMaxCol < nColumn) ? nColumn : m_nMaxCol;

  m_bTitleRow    = true;
  return *this;
} // end Tools_TemplateSimpleTable :: setColumnTitle

/*
  -----------------------------------------------------------------------------
  public function Tools_TemplateSimpleTable :: setCell
  -----------------------------------------------------------------------------
 */
Tools_TemplateSimpleTable & Tools_TemplateSimpleTable :: setCell ( const SAPDB_UTF8 * szCell,
                                                                   const int          nRow,
                                                                   const int          nColumn)
{
  m_RowMap[nRow][nColumn]= szCell;

  m_nMaxRow = (m_nMaxRow < nRow   ) ? nRow    : m_nMaxRow;
  m_nMaxCol = (m_nMaxCol < nColumn) ? nColumn : m_nMaxCol;
  return *this;
} // end Tools_TemplateSimpleTable :: setCell

/*
  -----------------------------------------------------------------------------
  public function Tools_TemplateSimpleTable :: setColumnSummary
  -----------------------------------------------------------------------------
 */
Tools_TemplateSimpleTable & Tools_TemplateSimpleTable :: setColumnSummary ( const SAPDB_UTF8 * szColumnSummary,
                                                                            const int          nColumn)
{
  m_ColumnSummaryMap[nColumn] = szColumnSummary;

  m_nMaxCol = (m_nMaxCol < nColumn) ? nColumn : m_nMaxCol;

  m_bSummaryRow  = true;
  return *this;
} // end Tools_TemplateSimpleTable :: setColumnSummary

/*
  -----------------------------------------------------------------------------
  public function Tools_TemplateSimpleTable :: addButton
  -----------------------------------------------------------------------------
 */
Tools_TemplateSimpleTable & Tools_TemplateSimpleTable :: addButton ( const SAPDB_UTF8 * szName,  
                                                                     const SAPDB_UTF8 * szLink,
                                                                     const SAPDB_UTF8 * szTarget,
                                                                     const bool         bDisabled)
{
  Tools_DynamicUTF8String strName;

  strName.Assign("'")
         .Append(szName)
         .Append("','")
         .Append(szLink)
         .Append("','")
         .Append(szTarget)
         .Append("',")
         .Append(bDisabled ? "true" : "false");

  ++m_nMaxButton;

  m_ButtonMap[m_nMaxButton] = strName;
  return *this;
} // end Tools_TemplateSimpleTable :: addButton

/*
  -----------------------------------------------------------------------------
  private function Tools_TemplateSimpleTable :: askForWriteCount
  -----------------------------------------------------------------------------
 */
SAPDB_Int2 Tools_TemplateSimpleTable :: askForWriteCount  ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;

  if (szName.Compare(OBJ_LOGO) == 0) {
    nReturn = (m_bLogo) ? 1 : 0;
  } else if (szName.Compare(OBJ_GROUP) == 0) {
    nReturn = (m_bGroup) ? 1 : 0;
  } else if (szName.Compare(OBJ_SIMPLE) == 0) {
    nReturn = (m_bGroup) ? 0 : 1;
  } else if (szName.Compare(OBJ_HEADLINE) == 0) {
    // print header if header string not empty
    nReturn = (m_strHeader.Empty()) ? 0 : 1;
  } else if (szName.Compare(OBJ_TABLETITLEROW) == 0) {
    // print table title if table title string not empty
    nReturn = (m_strTableTitle.Empty()) ? 0 : 1;
  } else if (szName.Compare(OBJ_TABLETITLECOL) == 0) {
    // print table title if table title string not empty
    nReturn = (m_strTableTitle.Empty()) ? 0 : 1;
  } else if (szName.Compare(OBJ_COLUMNTITLEROW) == 0) {
    // print column title row if there are columns an column titles
    nReturn = m_bTitleRow ? 1 : 0;
  } else if (szName.Compare(OBJ_COLUMNTITLECOL) == 0) {
    // print m_nMaxCol column title columns
    nReturn = m_nMaxCol;
    m_nCurrentCol = 1;
  } else if (szName.Compare(OBJ_ROWPAIR) == 0) {
    // print computed row pairs
    nReturn = (m_nMaxRow / 2) + (m_nMaxRow % 2);
  } else if (szName.Compare(OBJ_EVENROWROW) == 0) {
    // print this row if there are rows to print
    nReturn = (m_nCurrentRow < m_nMaxRow) ? 1 : 0;
    ++m_nCurrentRow;
  } else if (szName.Compare(OBJ_EVENROWCOL) == 0) {
    // print m_nMaxCol columns
    nReturn = m_nMaxCol;
    m_nCurrentCol = 1;
  } else if (szName.Compare(OBJ_ODDROWROW) == 0) {
    // print this row if there are rows to print
    nReturn = (m_nCurrentRow < m_nMaxRow) ? 1 : 0;
    ++m_nCurrentRow;
  } else if (szName.Compare(OBJ_ODDROWCOL) == 0) {
    // print m_nMaxCol columns
    nReturn = m_nMaxCol;
    m_nCurrentCol = 1;
  } else if (szName.Compare(OBJ_COLUMNSUMMARYROW) == 0) {
    // print column summary row if there are columns an column summary
    nReturn = m_bSummaryRow ? 1 : 0;
  } else if (szName.Compare(OBJ_COLUMNSUMMARYCOL) == 0) {
    // print m_nMaxCol column summary columns
    nReturn = m_nMaxCol;
    m_nCurrentCol = 1;
  } else if (szName.Compare(OBJ_BUTTONBAR) == 0) {
    // print if there are buttons
    nReturn = (m_nMaxButton == 0) ? 0 : 1;
  } else if (szName.Compare(OBJ_BUTTON) == 0) {
    // print button count
    nReturn = m_nMaxButton;
    m_nCurrentButton = 1;
  } // end if

  return nReturn;
} // end Tools_TemplateSimpleTable :: askForWriteCount

/*
  -----------------------------------------------------------------------------
  private function Tools_TemplateSimpleTable :: askForValue
  -----------------------------------------------------------------------------
 */
Tools_DynamicUTF8String Tools_TemplateSimpleTable :: askForValue  ( const Tools_DynamicUTF8String & szName )
{
  Tools_DynamicUTF8String strReturn;

  if (szName.Compare(OBJ_HEADLINE) == 0) {

    strReturn.Assign(m_strHeader);

  } else if (szName.Compare(OBJ_TABLETITLECOL) == 0) {

    strReturn.Assign(m_strTableTitle);

  } else if (szName.Compare(OBJ_COLUMNTITLECOL) == 0) {

    strReturn.Assign(m_ColumnTitleMap[m_nCurrentCol]);
    ++m_nCurrentCol;

  } else if ( (szName.Compare(OBJ_EVENROWCOL) == 0) ||
              (szName.Compare(OBJ_ODDROWCOL ) == 0)     ){

    strReturn.Assign(m_RowMap[m_nCurrentRow][m_nCurrentCol]);
    ++m_nCurrentCol;

  } else if (szName.Compare(OBJ_COLUMNSUMMARYCOL) == 0) {

    strReturn.Assign(m_ColumnSummaryMap[m_nCurrentCol]);
    ++m_nCurrentCol;

  } else if (szName.Compare(OBJ_BUTTON) == 0) {

    strReturn.Assign(m_ButtonMap[m_nCurrentButton]);
    ++m_nCurrentButton;

  } // end if

  return strReturn;
} // end Tools_TemplateSimpleTable :: askForValue
