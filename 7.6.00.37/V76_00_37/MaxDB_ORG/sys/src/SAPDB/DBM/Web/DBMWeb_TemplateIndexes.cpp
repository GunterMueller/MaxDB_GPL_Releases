/*!
  -----------------------------------------------------------------------------
  module: DBMWeb_TemplateIndexes.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  Indexes 
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
#include "DBM/Web/DBMWeb_TemplateIndexes.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define ISOBJ(obj)  (szName.Compare(obj) == 0)

#define TEMPLATE_NAME          "DBMIndexes.htm"

#define OBJ_MODEUSE            "ModeUse"
#define OBJ_MODEREC            "ModeRecover"

#define OBJ_SELECT             "Select"
#define OBJ_INDEXES            "Indexes"
#define OBJ_IDXNAMES           "IndexNames"
#define OBJ_IDXDETAILS         "IndexDetails"
#define OBJ_RESULT             "Result"

#define OBJ_OWNER              "Owner"
#define OBJ_TABLE              "Table"
#define OBJ_INDEX              "Index"
#define OBJ_TYPE               "Type"
#define OBJ_DATE               "Date"
#define OBJ_USED               "Used"
#define OBJ_DISABLED           "Disabled"
#define OBJ_BAD                "Bad"

#define OBJ_ROWPAIR            "RowPair"
#define OBJ_EVENROW            "EvenRow*"
#define OBJ_ODDROW             "OddRow*"

#define OBJ_BTNSEL             "BtnSelect"
#define OBJ_BTNENA             "BtnEnable"
#define OBJ_BTNDIS             "BtnDisable"
#define OBJ_BTNDEL             "BtnDelete"
#define OBJ_BTNREC             "BtnRecreate"
#define OBJ_BTNREA             "BtnRecreateAll"
#define OBJ_BTNBAK             "BtnBack"

/*
  =============================================================================
  class:        DBMWeb_TemplateIndexes
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMWeb_TemplateIndexes :: DBMWeb_TemplateIndexes
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateIndexes :: DBMWeb_TemplateIndexes ( sapdbwa_WebAgent & wa,
                                                   DBMCli_Database * pDB,
                                                   DBMWeb_TemplIndexes_Mode nMode )
                        : Tools_Template( wa, _Tools_UTF8Ptr( TEMPLATE_NAME )),
                          m_pDB( pDB ),
                          m_nMode( nMode )
{
  m_bRecoverMode = SAPDB_FALSE;
  m_bDetails = SAPDB_FALSE;
  m_nMaxRows = 1000;
  m_nRow = 0;
} 

/*
  -----------------------------------------------------------------------------
  public destructor DBMWeb_TemplateIndexes :: ~DBMWeb_TemplateIndexes
  -----------------------------------------------------------------------------
*/
DBMWeb_TemplateIndexes :: ~DBMWeb_TemplateIndexes (  )
{
} 

/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateIndexes :: askForWriteCount
  -----------------------------------------------------------------------------
*/
SAPDB_Int2 DBMWeb_TemplateIndexes :: askForWriteCount  ( const Tools_DynamicUTF8String & szName )
{
  SAPDB_Int2 nReturn = 0;
  Msg_List oMsgList;

  if( ISOBJ( OBJ_MODEUSE ) ) {
    nReturn = (!m_bRecoverMode) ? 1 : 0;
  } else if( ISOBJ( OBJ_MODEREC ) ) {
    nReturn = (m_bRecoverMode) ? 1 : 0;
  } else if( ISOBJ( OBJ_SELECT ) ) {
    nReturn = (m_nMode == DBMWEB_TEMPLINDEXES_MODE_SELECT) ? 1 : 0;
  } else if( ISOBJ( OBJ_INDEXES ) ) {
    nReturn = (m_nMode == DBMWEB_TEMPLINDEXES_MODE_INDEXES) ? 1 : 0;
  } else if( ISOBJ( OBJ_IDXNAMES ) ) {
    nReturn = (!m_bDetails) ? 1 : 0;
  } else if( ISOBJ( OBJ_IDXDETAILS ) ) {
    nReturn = (m_bDetails) ? 1 : 0;
  } else if( ISOBJ( OBJ_RESULT ) ) {
    nReturn = (m_nMode == DBMWEB_TEMPLINDEXES_MODE_RESULT) ? 1 : 0;
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
        m_oRow.GetField( m_sIndex, "|\n" );
        m_sIndex.Trim( );

        if( m_bDetails ) {

          m_oRow.GetField( m_sType, "|\n" );
          m_sType.Trim( );

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

          m_oRow.GetField( m_sUsed, "|\n" );
          m_sUsed.Trim( );
          m_oRow.GetField( m_sDisabled, "|\n" );
          m_sDisabled.Trim( );
          m_oRow.GetField( m_sBad, "|\n" );
          m_sBad.Trim( );

        }

        nReturn = 1;
      }
      m_nRow += 1;
    }
  } else if( ISOBJ( OBJ_BTNSEL ) ) {
    nReturn = (m_nMode == DBMWEB_TEMPLINDEXES_MODE_SELECT) ? 1 : 0;
  } else if( ISOBJ( OBJ_BTNENA ) ) {
    nReturn = ( (m_nMode == DBMWEB_TEMPLINDEXES_MODE_INDEXES) &&
                (!m_bRecoverMode)                                ) ? 1 : 0;
  } else if( ISOBJ( OBJ_BTNDIS ) ) {
    nReturn = ( (m_nMode == DBMWEB_TEMPLINDEXES_MODE_INDEXES) &&
                (!m_bRecoverMode)                                ) ? 1 : 0;
  } else if( ISOBJ( OBJ_BTNDEL ) ) {
    nReturn = ( (m_nMode == DBMWEB_TEMPLINDEXES_MODE_INDEXES) &&
                (!m_bRecoverMode)                                ) ? 1 : 0;
  } else if( ISOBJ( OBJ_BTNREA ) ) {
    DBMCli_Version & oVersion = m_pDB->GetVersion( );
    nReturn = ( (m_nMode == DBMWEB_TEMPLINDEXES_MODE_SELECT) &&
                (m_bRecoverMode)                             &&
                (oVersion.GetNumber() < 0x07040400)              ) ? 1 : 0;
  } else if( ISOBJ( OBJ_BTNREC ) ) {
    nReturn = ( (m_nMode == DBMWEB_TEMPLINDEXES_MODE_INDEXES) &&
                (m_bRecoverMode)                                ) ? 1 : 0;
  } else if( ISOBJ( OBJ_BTNBAK ) ) {
    nReturn = ( (m_nMode == DBMWEB_TEMPLINDEXES_MODE_INDEXES)   || 
                (m_nMode == DBMWEB_TEMPLINDEXES_MODE_RESULT)  )    ? 1 : 0;
  }
  return nReturn;
} 

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateIndexes :: askForValue
  -----------------------------------------------------------------------------
*/
Tools_DynamicUTF8String DBMWeb_TemplateIndexes :: askForValue ( const Tools_DynamicUTF8String & szName )
{
  const SAPDB_Char * pReturn = NULL;

  DBMCli_String sValue;

  if( ISOBJ( OBJ_OWNER ) ) {
    pReturn = m_sOwner;
  } else if( ISOBJ( OBJ_TABLE) ) {
    pReturn = m_sTable;
  } else if( ISOBJ( OBJ_INDEX ) ) {
    pReturn = m_sIndex;
  } else if( ISOBJ( OBJ_TYPE) ) {
    pReturn = m_sType;
  } else if( ISOBJ( OBJ_DATE) ) {
    pReturn = m_sDate;
  } else if( ISOBJ( OBJ_USED) ) {
    pReturn = m_sUsed;
  } else if( ISOBJ( OBJ_DISABLED) ) {
    pReturn = m_sDisabled;
  } else if( ISOBJ( OBJ_BAD) ) {
    pReturn = m_sBad;
  }

  return _Tools_UTF8Ptr(pReturn);
} 

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_TemplateInfoTable :: askForContinue
  -----------------------------------------------------------------------------
*/
bool DBMWeb_TemplateIndexes :: askForContinue ( const Tools_DynamicUTF8String & szName )
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
