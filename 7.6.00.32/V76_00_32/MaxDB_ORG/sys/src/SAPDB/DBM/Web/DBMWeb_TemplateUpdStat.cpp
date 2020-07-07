/*!
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateUpdStat.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  UpdStat database
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
#include "DBM/Web/DBMWeb_TemplateUpdStat.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define ISOBJ(obj)  (szName.Compare(obj) == 0)

#define TEMPLATE_NAME          "DBMUpdStat.htm"

#define OBJ_SELECT             "Select"
#define OBJ_ESTIMATE           "Estimate" 
#define OBJ_TABLES             "Tables"
#define OBJ_COLUMNS            "Columns"
#define OBJ_RESULT             "Result"

#define OBJ_OWNER              "Owner"
#define OBJ_TABLE              "Table"
#define OBJ_COLUMN             "Column"
#define OBJ_DATE               "Date"

#define OBJ_ROWPAIR            "RowPair"
#define OBJ_EVENROW            "EvenRow*"
#define OBJ_ODDROW             "OddRow*"

#define OBJ_BTNSEL             "BtnSelect"
#define OBJ_BTNEXE             "BtnExecute"
#define OBJ_BTNBAK             "BtnBack"

/*
  =============================================================================
  class:        DBMWeb_TemplateUpdStat
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMWeb_TemplateUpdStat :: DBMWeb_TemplateUpdStat
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateUpdStat :: DBMWeb_TemplateUpdStat ( sapdbwa_WebAgent & wa,
                                                   DBMCli_Database * pDB,
                                                   DBMWeb_TemplUpdStat_Mode nMode )
                        : Tools_Template( wa, _Tools_UTF8Ptr( TEMPLATE_NAME )),
                          m_pDB( pDB ),
                          m_nMode( nMode )
{
  m_nMaxRows = 1000;
  m_nRow = 0;
} 

/*
  -----------------------------------------------------------------------------
  public destructor DBMWeb_TemplateUpdStat :: ~DBMWeb_TemplateUpdStat
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateUpdStat :: ~DBMWeb_TemplateUpdStat (  )
{
} 

/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateUpdStat :: askForWriteCount
  -----------------------------------------------------------------------------
*/
SAPDB_Int2 DBMWeb_TemplateUpdStat :: askForWriteCount  ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;
  Msg_List oMsgList;

  if( ISOBJ( OBJ_SELECT ) ) {
    nReturn = (m_nMode == DBMWEB_TEMPLUPDSTAT_MODE_SELECT) ? 1 : 0;
  } else if( ISOBJ( OBJ_ESTIMATE ) ) {
    nReturn = ( (m_nMode == DBMWEB_TEMPLUPDSTAT_MODE_TABLES)    || 
                (m_nMode == DBMWEB_TEMPLUPDSTAT_MODE_COLUMNS) )    ? 1 : 0;
  } else if( ISOBJ( OBJ_TABLES ) ) {
    nReturn = (m_nMode == DBMWEB_TEMPLUPDSTAT_MODE_TABLES) ? 1 : 0;
  } else if( ISOBJ( OBJ_COLUMNS ) ) {
    nReturn = (m_nMode == DBMWEB_TEMPLUPDSTAT_MODE_COLUMNS) ? 1 : 0;
  } else if( ISOBJ( OBJ_RESULT ) ) {
    nReturn = (m_nMode == DBMWEB_TEMPLUPDSTAT_MODE_RESULT) ? 1 : 0;
  } else if( ISOBJ( OBJ_ROWPAIR ) ) {
    nReturn = -1;
  } else if( ISOBJ( OBJ_EVENROW ) || ISOBJ( OBJ_ODDROW ) ) {
    if( m_nRow < m_nMaxRows ) {
      m_oRow.Clear( );
      if (m_pDB->GetInfo( ).GetRow( m_oRow, oMsgList )) {
        m_oRow.GetField( m_sOwner, "|\n" );
        m_sOwner.Trim( );
        m_oRow.GetField( m_sTable, "|\n" );
        m_sTable.Trim( );
        if( m_nMode == DBMWEB_TEMPLUPDSTAT_MODE_TABLES ) {
          DBMCli_String sDate;
          DBMCli_String sTime;
          DBMCli_DateTime oDateTime;

          m_oRow.GetField( sDate, "|\n" );
          m_oRow.GetField( sTime, "|\n" );

		  sDate.Trim( );
		  sTime.Trim( );

		  if( sDate != "(null)" && sTime != "(null)") {
			oDateTime.Set( sDate, DBMCLI_DT_NUM_DATE_FMT, sTime, DBMCLI_DT_NUM_TIME_FMT );
			m_sDate = oDateTime.Get( DBMCLI_DT_INT_DATETIME_FMT );
		  } else {
            m_sDate = " ";
		  }
        } else {
          m_oRow.GetField( m_sColumn, "|\n" );
          m_sColumn.Trim( );
        }
        nReturn = 1;
      }
      m_nRow += 1;
    }
  } else if( ISOBJ( OBJ_BTNSEL ) ) {
    nReturn = (m_nMode == DBMWEB_TEMPLUPDSTAT_MODE_SELECT) ? 1 : 0;
  } else if( ISOBJ( OBJ_BTNEXE ) ) {
    nReturn = ( (m_nMode == DBMWEB_TEMPLUPDSTAT_MODE_TABLES)    || 
                (m_nMode == DBMWEB_TEMPLUPDSTAT_MODE_COLUMNS) )    ? 1 : 0;
  } else if( ISOBJ( OBJ_BTNBAK ) ) {
    nReturn = ( (m_nMode == DBMWEB_TEMPLUPDSTAT_MODE_TABLES)    || 
                (m_nMode == DBMWEB_TEMPLUPDSTAT_MODE_COLUMNS)   ||
                (m_nMode == DBMWEB_TEMPLUPDSTAT_MODE_RESULT)  )    ? 1 : 0;
  }
  return nReturn;
} 

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateUpdStat :: askForValue
  -----------------------------------------------------------------------------
*/
Tools_DynamicUTF8String DBMWeb_TemplateUpdStat :: askForValue ( const Tools_DynamicUTF8String & szName )
{
  const SAPDB_Char * pReturn = NULL;

  DBMCli_String sValue;

  if( ISOBJ( OBJ_OWNER ) ) {
    pReturn = m_sOwner;
  } else if( ISOBJ( OBJ_TABLE) ) {
    pReturn = m_sTable;
  } else if( ISOBJ( OBJ_COLUMN ) ) {
    pReturn = m_sColumn;
  } else if( ISOBJ( OBJ_DATE) ) {
    pReturn = m_sDate;
  }

  return _Tools_UTF8Ptr(pReturn);
} 

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateInfoTable :: askForContinue
  -----------------------------------------------------------------------------
*/
bool DBMWeb_TemplateUpdStat :: askForContinue ( const Tools_DynamicUTF8String & szName )
{
  bool bRC = false;

  if( ISOBJ( OBJ_ROWPAIR ) ) {
    if( m_nRow < m_nMaxRows ) {
      if( !m_pDB->GetInfo( ).EndOfInfo( ) ) {
        bRC = true;
      }
    }
  }

  return bRC;
}
