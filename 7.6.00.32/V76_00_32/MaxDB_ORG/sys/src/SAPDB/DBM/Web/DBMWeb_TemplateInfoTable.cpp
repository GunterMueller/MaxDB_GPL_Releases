/*!
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateInfoTable.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  a class for creating info tables in SAP MiniApps style
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
#include "DBM/Web/DBMWeb_TemplateInfoTable.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define TEMPLATE_NAME          "DBMInfoTable.htm"

#define OBJ_TITLE              "GroupBoxTitle"

#define OBJ_REFRESHBUTTON      "RefreshButton"

#define OBJ_COLUMNTITLEROW     "ColumnTitleRow"
#define OBJ_COLUMNTITLECOL     "ColumnTitleColumn"

#define OBJ_ROWPAIR            "RowPair"

#define OBJ_EVENROWROW         "EvenRowRow"
#define OBJ_ODDROWROW          "OddRowRow"

#define OBJ_EVENROWCOL         "EvenRowColumn"
#define OBJ_ODDROWCOL          "OddRowColumn"

/*
  =============================================================================
  class:        DBMWeb_TemplateInfoTable
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMWeb_TemplateInfoTable :: DBMWeb_TemplateInfoTable
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateInfoTable :: DBMWeb_TemplateInfoTable ( sapdbwa_WebAgent & wa,
                                                       DBMCli_String & sEvent,
                                                       DBMCli_Info & oInfo )
                        : Tools_Template( wa, _Tools_UTF8Ptr( TEMPLATE_NAME )),
                            m_sEvent( sEvent ),
                            m_oInfo( oInfo )
{
  m_nCurrentCol = 0;
  m_nCurrentRow = 0;
} // end DBMWeb_TemplateInfoTable :: DBMWeb_TemplateInfoTable

/*
  -----------------------------------------------------------------------------
  public destructor DBMWeb_TemplateInfoTable :: ~DBMWeb_TemplateInfoTable
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateInfoTable :: ~DBMWeb_TemplateInfoTable (  )
{
} // end DBMWeb_TemplateInfoTable :: ~DBMWeb_TemplateInfoTable


/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateInfoTable :: askForWriteCount
  -----------------------------------------------------------------------------
*/
SAPDB_Int2 DBMWeb_TemplateInfoTable :: askForWriteCount  ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;

  if (szName.Compare(OBJ_TITLE) == 0) {

    nReturn = 1;

  } else if (szName.Compare(OBJ_REFRESHBUTTON) == 0) {

    nReturn = 1;
    
  } else if (szName.Compare(OBJ_COLUMNTITLEROW) == 0) {

    nReturn = 1;
  
  } else if (szName.Compare(OBJ_COLUMNTITLECOL) == 0) {

    nReturn = m_oInfo.ColumnArray( ).GetSize( );
    m_nCurrentCol = 0;
  
  } else if (szName.Compare(OBJ_ROWPAIR) == 0) {

    nReturn = -1;

  } else if (szName.Compare(OBJ_EVENROWROW) == 0) {
    
    nReturn = (m_oEvenRow.IsEmpty()) ? 0 : 1; 
  
  } else if (szName.Compare(OBJ_EVENROWCOL) == 0) {

    nReturn = m_oInfo.ColumnArray( ).GetSize( );
    m_nCurrentCol = 1;
  
  } else if (szName.Compare(OBJ_ODDROWROW) == 0) {

    nReturn = (m_oOddRow.IsEmpty()) ? 0 : 1; 
  
  } else if (szName.Compare(OBJ_ODDROWCOL) == 0) {

    nReturn = m_oInfo.ColumnArray( ).GetSize( );
    m_nCurrentCol = 1;
  
  } // end if

  return nReturn;
} // end DBMWeb_TemplateInfoTable :: askForWriteCount

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateInfoTable :: askForValue
  -----------------------------------------------------------------------------
*/
Tools_DynamicUTF8String DBMWeb_TemplateInfoTable :: askForValue ( const Tools_DynamicUTF8String & szName )
{
  const SAPDB_Char * pReturn = NULL;
  DBMCli_String sValue;
  
  if (szName.Compare(OBJ_TITLE) == 0) {

    sValue = m_oInfo.Title( );
    pReturn = sValue;  

  } else if (szName.Compare(OBJ_REFRESHBUTTON) == 0) {

    pReturn = m_sEvent;

  } else if (szName.Compare(OBJ_COLUMNTITLECOL) == 0) {

    DBMCli_InfoColumnArray & aCols = m_oInfo.ColumnArray( );

    pReturn = aCols[m_nCurrentCol].sName;
    m_nCurrentCol++;

  } else if (szName.Compare(OBJ_EVENROWCOL) == 0) {

    if( m_oEvenRow.GetField( sValue, "=|\n" ) ) {
      sValue.Trim( );
      pReturn = sValue;
    }

    m_nCurrentCol++;

  } else if (szName.Compare(OBJ_ODDROWCOL) == 0) {

    if( m_oOddRow.GetField( sValue, "=|\n" ) ) {
      sValue.Trim( );
      pReturn = sValue;
    }

    m_nCurrentCol++;
  }

  return _Tools_UTF8Ptr(pReturn);
} // end DBMWeb_TemplateInfoTable :: askForValue

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateInfoTable :: askForContinue
  -----------------------------------------------------------------------------
*/

bool DBMWeb_TemplateInfoTable :: askForContinue ( const Tools_DynamicUTF8String & szName )
{
  bool bRC = false;
  Msg_List oMsgList;

  if (szName.Compare(OBJ_ROWPAIR) == 0) {
    m_oEvenRow.Clear( );
    m_oOddRow.Clear( );

    if (m_oInfo.GetRow( m_oEvenRow, oMsgList )) {
      m_oInfo.GetRow( m_oOddRow, oMsgList );
      bRC = true;
    }
  }

  return bRC;
}
