/*! 
  -----------------------------------------------------------------------------
  module: DBMCli_Info.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager Client Library
  description:  Info Class
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
#include "DBM/Cli/DBMCli_Database.hpp"
#include "DBM/Cli/DBMCli_Info.hpp"

/*    
  =============================================================================
  class: DBMCli_Info
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMCli_Info :: DBMCli_Info
  -----------------------------------------------------------------------------
*/
DBMCli_Info :: DBMCli_Info ( )
{
  Clear( );
} 

/*
  -----------------------------------------------------------------------------
  public destructor DBMCli_Info :: ~DBMCli_Info
  -----------------------------------------------------------------------------
*/
DBMCli_Info :: ~DBMCli_Info ( )
{

} 

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Info :: Open
  -----------------------------------------------------------------------------
*/

SAPDB_Bool DBMCli_Info :: Open ( const DBMCli_String        & sName,
                                       Msg_List & oMsgList )
{
  return Open( sName, "", oMsgList );
}


SAPDB_Bool DBMCli_Info :: Open ( const DBMCli_String        & sName,
                                 const DBMCli_String        & sWhere,
                                       Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_Database & oDB = GetDatabase();
  DBMCli_Result & oResult = oDB.GetResult( ); 

  Clear( );

  if( oDB.SQLConnect( oMsgList ) ) {

    SAPDB_Bool bDesc = false;
    DBMCli_String sCmd( DBMCLI_CMD_INFO );
    sCmd += " ";
    sCmd += sName;
    sCmd += " ";
    if( sWhere.IsEmpty( ) ) {
      sCmd += DBMCLI_OPT_INFO_DEF;
      bDesc = true;
    } else {
      sCmd += DBMCLI_OPT_INFO_WHERE;
      sCmd += " ";
      sCmd += sWhere;
    }

    if( oDB.Execute( sCmd, oMsgList ) ) {
      m_sName = sName;

      if (m_sName.Compare(DBMCLI_INFO_CACHES) == 0) {
        m_sTitle = DBMCLI_TITLE_CACHES;
      } else if (m_sName.Compare(DBMCLI_INFO_DATA) == 0) {
        m_sTitle = DBMCLI_TITLE_DATA;
      } else if (m_sName.Compare(DBMCLI_INFO_IO) == 0) {
        m_sTitle = DBMCLI_TITLE_IO;
      } else if (m_sName.Compare(DBMCLI_INFO_LOG) == 0) {
        m_sTitle = DBMCLI_TITLE_LOG;
      } else if (m_sName.Compare(DBMCLI_INFO_LOCKS) == 0) {
        m_sTitle = DBMCLI_TITLE_LOCKS;
      } else if (m_sName.Compare(DBMCLI_INFO_USERS) == 0) {
        m_sTitle = DBMCLI_TITLE_USERS;
      } else if (m_sName.Compare(DBMCLI_INFO_VERSIONS) == 0) {
        m_sTitle = DBMCLI_TITLE_VERSIONS;
      } else {
        m_sTitle = m_sName;
      }

      DBMCli_String sLine;

      oResult.GetLine( sLine ); 
      m_bContinue = ( sLine == DBMCLI_VAL_CONTINUE );

      DBMCli_String sDesc;
      DBMCli_ResultBuf sHeader;

      if( bDesc ) {
        oResult.GetLine( sDesc );
      }
      oResult.GetLine( sHeader );
      
      GetColumns( sDesc, sHeader );

      oResult.SkipLine(  );

      bRC = true;
    }

  }

  return bRC;
}



/*
  -----------------------------------------------------------------------------
  public function DBMCli_Info :: GetRow
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Info :: GetRow ( DBMCli_String        & sRow,
                                   Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_Database & oDB = GetDatabase();
  DBMCli_Result & oResult = oDB.GetResult( ); 

  if( oResult.GetLine( sRow ) ) {
    bRC = true;
  } else {
    if( m_bContinue ) {
      if ( Next( oMsgList ) ) {
        if( oResult.GetLine( sRow ) ) {
          bRC = true;
        }
      }
    }
  }

  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Info :: EndOfInfo
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Info :: EndOfInfo ( )
{
  return GetDatabase( ).GetResult( ).EndOfBuffer( );
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Info :: Close
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Info :: Close ( Msg_List & oMsgList )
{
  DBMCli_Database & oDB = GetDatabase();

  return oDB.SQLRelease( oMsgList );
}


/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  private function DBMCli_Info :: Clear
  -----------------------------------------------------------------------------
*/
void DBMCli_Info :: Clear ( )
{
  m_sName.Empty( );
  m_bContinue = false;
  m_aInfoColumn.RemoveAll( );
}
/*
  -----------------------------------------------------------------------------
  private function DBMCli_Info :: GetColumns
  -----------------------------------------------------------------------------
*/
void DBMCli_Info :: GetColumns ( DBMCli_String    & sDesc,
                                 DBMCli_ResultBuf & sHeader )
{
  DBMCli_InfoColumn tColumn;
  SAPDB_Int iCol = 0;

  while( sHeader.GetField( tColumn.sName, "|\n" ) ) {
    tColumn.sName.Trim( );
    tColumn.sName.FreeExtra( );

    if( !sDesc.IsEmpty( ) ) {
      switch( sDesc[iCol] ) {
        case 'S': 
          tColumn.nType = DBMCLI_INFOCOLTYPE_STRING;
          break;
        case 'N':
          tColumn.nType = DBMCLI_INFOCOLTYPE_NUM;
          break;
        case 'D':
          tColumn.nType = DBMCLI_INFOCOLTYPE_DATE;
          break;
        case 'B':
          tColumn.nType = DBMCLI_INFOCOLTYPE_BOOL;
          break;
        default:
          tColumn.nType = DBMCLI_INFOCOLTYPE_UNKNOWN;
          break;
      }
    } else {
      tColumn.nType = DBMCLI_INFOCOLTYPE_STRING;
    }

    m_aInfoColumn.Add( tColumn );

    iCol++;
  }
}

/*
  -----------------------------------------------------------------------------
  private function DBMCli_Info :: Next
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Info :: Next ( Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_Database & oDB = GetDatabase();
  DBMCli_Result & oResult = oDB.GetResult( ); 

  DBMCli_String sCmd( DBMCLI_CMD_INFONEXT );

  if( oDB.Execute( sCmd, oMsgList ) ) {
    DBMCli_String sLine;

    oResult.GetLine( sLine ); 
    m_bContinue = ( sLine == DBMCLI_VAL_CONTINUE );

    oResult.SkipLine( );
    oResult.SkipLine( );

    bRC = true;
  }

  return bRC;
}

