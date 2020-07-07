/*!**********************************************************************

  module: StudioWeb_ResultTemplate.cpp

  -----------------------------------------------------------------------

  responsible:  WolfgangA

  special area: WebQuery

  description:  result template class for WebQuery

  see also:		/

  -----------------------------------------------------------------------

  copyright:                Copyright (c) 2000-2005 SAP AG




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




************************************************************************/
/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
#include <stdio.h>
#include "SAPDB/SQLStudio/WebQuery/StudioWeb_Definitions.hpp"
#include "SAPDB/SQLStudio/WebQuery/StudioWeb_ResultTemplate.hpp"

/*
  -----------------------------------------------------------------------------
  private macros
  -----------------------------------------------------------------------------
 */
#define TEMPL_TABLE            "wqresulttemplate.htm"

#define OBJ_HEADLINE           "Headline"

#define OBJ_LOGO               "Logo"
#define OBJ_SIMPLE             "SimpleTable"

#define OBJ_TABLETITLEROW      "TableTitleRow"
#define OBJ_TABLETITLECOL      "TableTitleColumn"

#define OBJ_TABLETITLECOLUMNSPAN   "TableTitleColumnSpan"

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

#define OBJ_VALIDTABLE         "ValidTable"

#define OBJ_RESULTS		       "Results"
#define OBJ_RESULTONE	       "OneResult"

#define OBJ_NAVIGATEFORM		"NavigateForm"

#define SELECTED		    " selected "
#define OPTION				"option" 

#define OBJ_ZOOMTEXT		"ZoomText"


/*
  =============================================================================
  class:        StudioWeb_ResultTemplate
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor StudioWeb_ResultTemplate::StudioWeb_ResultTemplate
  -----------------------------------------------------------------------------
 */
StudioWeb_ResultTemplate ::StudioWeb_ResultTemplate (sapdbwa_WebAgent &wa)
               : Tools_Template(wa, _Tools_UTF8Ptr(TEMPL_TABLE ))
{

  m_nMaxRow = 0;
  m_nMaxCol = 0;

  m_nCurrentCol = 0;
  m_nCurrentRow = 0;

  m_nMaxButton     = 0;
  m_nCurrentButton = 0;

  m_bTitleRow    = SAPDB_FALSE;
  m_bSummaryRow  = SAPDB_FALSE;

  m_nSelectedResult = -1;

  m_bLogo        = SAPDB_FALSE;

  m_nCurrentResultCount = 0;
  m_nMaxResultCount = 0;

  
} // end StudioWeb_ResultTemplate::StudioWeb_ResultTemplate

/*-----------------------------------------------------------------------------
  public destructor StudioWeb_ResultTemplate::~StudioWeb_ResultTemplate
  -----------------------------------------------------------------------------*/
StudioWeb_ResultTemplate ::~StudioWeb_ResultTemplate( )
{

} // end StudioWeb_ResultTemplate::~StudioWeb_ResultTemplate

/*-----------------------------------------------------------------------------
  public function StudioWeb_ResultTemplate::setHeadLine
  -----------------------------------------------------------------------------*/
StudioWeb_ResultTemplate &StudioWeb_ResultTemplate::setHeadLine(const Tools_DynamicUTF8String &sHeadLine )
{
  m_strHeader = sHeadLine;
  return *this;
} // end StudioWeb_ResultTemplate::setHeadLine

/*-----------------------------------------------------------------------------
  public function StudioWeb_ResultTemplate::setTableTitle
  -----------------------------------------------------------------------------*/
StudioWeb_ResultTemplate &StudioWeb_ResultTemplate::setTableTitle(const Tools_DynamicUTF8String &sTableTitle)
{
  m_strTableTitle = sTableTitle;
  return *this;
} // end StudioWeb_ResultTemplate::setHeadLine

/*-----------------------------------------------------------------------------
  public function StudioWeb_ResultTemplate::setColumnTitle
  -----------------------------------------------------------------------------*/
StudioWeb_ResultTemplate &StudioWeb_ResultTemplate::setColumnTitle(const Tools_DynamicUTF8String &sColumnTitle,
																   const SAPDB_UInt2          nColumn)
{
  m_ColumnTitleMap[nColumn] = sColumnTitle;

  m_nMaxCol = (m_nMaxCol < nColumn) ? nColumn : m_nMaxCol;

  m_bTitleRow    = SAPDB_TRUE;
  return *this;
} // end StudioWeb_ResultTemplate::setColumnTitle

StudioWeb_ResultTemplate &StudioWeb_ResultTemplate::setResults(const Tools_DynamicUTF8String &sResult,
															   const SAPDB_UInt2          nRowNo)
{
  m_Results[nRowNo] = sResult;
  m_nMaxResultCount = (m_nMaxResultCount < nRowNo) ? nRowNo : m_nMaxResultCount;

  return *this;
} // end StudioWeb_ResultTemplate::setColumnTitle

/*-----------------------------------------------------------------------------
  public function StudioWeb_ResultTemplate::setCell
  -----------------------------------------------------------------------------*/
StudioWeb_ResultTemplate &StudioWeb_ResultTemplate::setCell(const Tools_DynamicUTF8String &sCell,
															const SAPDB_UInt2          nRow,
															const SAPDB_UInt2          nColumn)
{
  m_RowMap[nRow][nColumn]= sCell;

  m_nMaxRow = (m_nMaxRow < nRow   ) ? nRow    : m_nMaxRow;
  m_nMaxCol = (m_nMaxCol < nColumn) ? nColumn : m_nMaxCol;
  return *this;
} // end StudioWeb_ResultTemplate::setCell

/*-----------------------------------------------------------------------------
  public function StudioWeb_ResultTemplate::setColumnSummary
  -----------------------------------------------------------------------------*/
StudioWeb_ResultTemplate &StudioWeb_ResultTemplate::setColumnSummary(const Tools_DynamicUTF8String &sColumnSummary,
																	 const SAPDB_UInt2          nColumn)
{	
  m_ColumnSummaryMap[nColumn] = sColumnSummary;

  m_nMaxCol = (m_nMaxCol < nColumn) ? nColumn : m_nMaxCol;

  m_bSummaryRow  = SAPDB_TRUE;
  return *this;
} // end StudioWeb_ResultTemplate::setColumnSummary

/*
  -----------------------------------------------------------------------------
  public function StudioWeb_ResultTemplate::addButton
  -----------------------------------------------------------------------------
 */
StudioWeb_ResultTemplate &StudioWeb_ResultTemplate::addButton(const Tools_DynamicUTF8String &sName,
															  const Tools_DynamicUTF8String &sLink,
														      const Tools_DynamicUTF8String &sTarget,
															  const SAPDB_Bool         bDisabled)
{
	Tools_DynamicUTF8String strName;

	strName = "'";
	strName.Append(sName);
	//strName.Append("','");
	strName.Append("',");
	strName.Append(sLink);
	//strName.Append("','");
	strName.Append(",'");
	strName.Append(sTarget);
	strName.Append("',");
	strName.Append(bDisabled ? DISABLED_TRUE : DISABLED_FALSE);

	++m_nMaxButton;

	m_ButtonMap[m_nMaxButton] = strName;
	return *this;

} // end StudioWeb_ResultTemplate::addButton


/*
  -----------------------------------------------------------------------------
  private function StudioWeb_ResultTemplate::askForWriteCount
  -----------------------------------------------------------------------------
 */
SAPDB_Int2 StudioWeb_ResultTemplate::askForWriteCount (const Tools_DynamicUTF8String &sName )
{
  SAPDB_Int2 nReturn = 0;		
	
 if (sName.Compare( OBJ_VALIDTABLE) == 0) {
    nReturn = (m_nMaxRow == 0 && m_nMaxCol == 0 && m_nMaxButton == 0 && m_nMaxResultCount == 0) ? 0 : 1;
  }

  if (sName.Compare( OBJ_LOGO) == 0) {
    nReturn = (m_bLogo) ? 1 : 0;
  }
  else if (sName.Compare( OBJ_SIMPLE) == 0) {
    nReturn = 1;
  }
  else if (sName.Compare( OBJ_HEADLINE) == 0) {
    // print header if header string not Empty
    nReturn = (m_strHeader.Empty()) ? 0 : 1;
  }
  else if (sName.Compare( OBJ_ZOOMTEXT) == 0) {
    // print header if header string not Empty
    nReturn = (m_sZoomText.Empty()) ? 0 : 1;
  }
  
  else if (sName.Compare( OBJ_TABLETITLEROW) == 0) {
    // print table title if table title string not Empty
    nReturn = (m_strTableTitle.Empty()) ? 0 : 1;
  }
  else if (sName.Compare( OBJ_TABLETITLECOL) == 0) {
    // print table title if table title string not Empty
    nReturn = (m_strTableTitle.Empty()) ? 0 : 1;
  }
  else if (sName.Compare( OBJ_COLUMNTITLEROW) == 0) {
    // print column title row if there are columns an column titles
    nReturn = m_bTitleRow ? 1 : 0;
  }
  else if (sName.Compare( OBJ_COLUMNTITLECOL) == 0) {
    // print m_nMaxCol column title columns
    nReturn = m_nMaxCol;
    m_nCurrentCol = 0;
  }
  else if (sName.Compare( OBJ_ROWPAIR) == 0) {
    // print computed row pairs
    nReturn = (m_nMaxRow / 2) + (m_nMaxRow % 2);
  }
  else if (sName.Compare( OBJ_EVENROWROW) == 0) {
    // print this row if there are rows to print
    nReturn = (m_nCurrentRow < m_nMaxRow) ? 1 : 0;
    ++m_nCurrentRow;
  }
  else if (sName.Compare( OBJ_EVENROWCOL) == 0) {
    // print m_nMaxCol columns
    nReturn = m_nMaxCol;
    m_nCurrentCol = 0;
  }
  else if (sName.Compare( OBJ_ODDROWROW) == 0) {
    // print this row if there are rows to print
    nReturn = (m_nCurrentRow < m_nMaxRow) ? 1 : 0;
    ++m_nCurrentRow;
  }
  else if (sName.Compare( OBJ_ODDROWCOL) == 0) {
    // print m_nMaxCol columns
    nReturn = m_nMaxCol;
    m_nCurrentCol = 0;
  }
  else if (sName.Compare( OBJ_COLUMNSUMMARYROW) == 0) {
    // print column summary row if there are columns an column summary
    nReturn = m_bSummaryRow ? 1 : 0;
  }
  else if (sName.Compare( OBJ_COLUMNSUMMARYCOL) == 0) {
    // print m_nMaxCol column summary columns
    nReturn = m_nMaxCol;
    m_nCurrentCol = 0;
  }
  else if (sName.Compare( OBJ_BUTTONBAR) == 0) {
    // print if there are buttons
    nReturn = (m_nMaxButton == 0) ? 0 : 1;
  }
  else if (sName.Compare( OBJ_BUTTON) == 0) {
    
    nReturn = m_nMaxButton;
    m_nCurrentButton = 0;
  }
  else if (sName.Compare( OBJ_TABLETITLECOLUMNSPAN) == 0) {
    
    nReturn = 1;    
  } // end if
  else if (sName.Compare( OBJ_RESULTS) == 0) {
    
	  nReturn = (m_nMaxResultCount > 1)  ? 1: 0;
  } // end if
  else if (sName.Compare( OBJ_NAVIGATEFORM) == 0) {
    
	  nReturn = (m_nMaxResultCount > 1 || m_nMaxButton > 0)  ? 1: 0;
  } // end if
  else if (sName.Compare( OBJ_RESULTONE) == 0) {
      m_nCurrentResultCount = 0;
      nReturn = (m_nMaxResultCount > 1)  ? m_nMaxResultCount: 0;
  } // end if
  

  return nReturn;
} // end StudioWeb_ResultTemplate::askForWriteCount

/*-----------------------------------------------------------------------------
  private function StudioWeb_ResultTemplate::askForValue
  -----------------------------------------------------------------------------*/
Tools_DynamicUTF8String StudioWeb_ResultTemplate::askForValue (const Tools_DynamicUTF8String &sName )
{

  if (sName.Compare( OBJ_HEADLINE) == 0) {

    return  m_strHeader;
  }
  else if (sName.Compare( OBJ_TABLETITLECOL) == 0) {

    return  m_strTableTitle;
  }
  else if (sName.Compare( OBJ_ZOOMTEXT) == 0) {

    return  m_sZoomText;
  }
  
  else if (sName.Compare( OBJ_COLUMNTITLECOL) == 0) {

	++m_nCurrentCol;
    return  m_ColumnTitleMap[m_nCurrentCol];
  }
  else if((sName.Compare( OBJ_EVENROWCOL) == 0) ||
          (sName.Compare( OBJ_ODDROWCOL ) == 0)     ){

	++m_nCurrentCol;
    return m_RowMap[m_nCurrentRow][m_nCurrentCol];
  }
  else if (sName.Compare( OBJ_COLUMNSUMMARYCOL) == 0) {

	 ++m_nCurrentCol;
    return  m_ColumnSummaryMap[m_nCurrentCol];

  }
  else if (sName.Compare( OBJ_BUTTON) == 0) {

	++m_nCurrentButton;
    return m_ButtonMap[m_nCurrentButton];
  } // end if
  else if (sName.Compare( OBJ_TABLETITLECOLUMNSPAN) == 0) {

	char sBuf[8];
	sprintf(sBuf,"%d",m_nMaxCol);
	return Tools_DynamicUTF8String(sBuf);
  } // end if  
  else if (sName.Compare( OBJ_RESULTONE) == 0) {
    
	  ++m_nCurrentResultCount;
	  Tools_DynamicUTF8String sBuffer;
	  sBuffer = "<";
	  sBuffer += OPTION;
	  if(m_nSelectedResult == m_nCurrentResultCount)
			sBuffer += SELECTED;
	  sBuffer += ">";
	  sBuffer += m_Results[m_nCurrentResultCount];
	  sBuffer += "<";
	  sBuffer += "/";
	  sBuffer += OPTION;
	  sBuffer += ">";

	  return sBuffer;
  } // end if

   return "";
} // end StudioWeb_ResultTemplate::askForValue


/*!-------------------------------------------------------------------------
    resetTemplate
-------------------------------------------------------------------------*/
void StudioWeb_ResultTemplate::resetTemplate ()
{
	m_bLogo = SAPDB_FALSE;
	m_bTitleRow = SAPDB_FALSE;
	m_bSummaryRow = SAPDB_FALSE;

	m_strHeader.Erase();
    m_strTableTitle.Erase();	
	
	m_nMaxRow = 0;
    m_nMaxCol = 0;
    m_nCurrentRow = 0;
    m_nCurrentCol = 0;
    m_nMaxButton = 0;
    m_nCurrentButton = 0;
	m_nSelectedResult = -1;

	m_nCurrentResultCount = 0;
	m_nMaxResultCount = 0;
     

	return;
}

void StudioWeb_ResultTemplate::resetStartValues ()
{	
	m_nCurrentResultCount = 0;
	m_nCurrentRow = 0;
    m_nCurrentCol = 0;
    m_nCurrentButton = 0;     

	return;
}



