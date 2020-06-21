/*! 
  -----------------------------------------------------------------------------
  module: DBMCli_UpdStat.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager Client Library
  description:  Update Statistics Class
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
#include "DBM/Cli/DBMCli_UpdStat.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define US_ALL      "*"

#define US_ESTIMATE "ESTIMATE SAMPLE"
#define US_ESTROWS  "ROWS"
#define US_ESTPCT   "PERCENT"
/*    
  =============================================================================
  class: DBMCli_UpdStat
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMCli_UpdStat :: DBMCli_UpdStat
  -----------------------------------------------------------------------------
*/
DBMCli_UpdStat :: DBMCli_UpdStat ( )
{
} 

/*
  -----------------------------------------------------------------------------
  public destructor DBMCli_UpdStat :: ~DBMCli_UpdStat
  -----------------------------------------------------------------------------
*/
DBMCli_UpdStat :: ~DBMCli_UpdStat ( )
{
} 
 
/*
  -----------------------------------------------------------------------------
  public function DBMCli_UpdStat :: Column
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_UpdStat :: Column ( const DBMCli_String & sItem, 
                                      DBMCli_UpdStateEstimate_Type nEstimate,
                                      const DBMCli_String & sEstimate,
                                      Msg_List & oMsgList )
{
  DBMCli_String sOwner;
  DBMCli_String sTable;
  DBMCli_String sColumn;

  SplitColumnItem( sItem, sOwner, sTable, sColumn );

  return Column( sOwner, sTable, sColumn, nEstimate, sEstimate, oMsgList );
}

SAPDB_Bool DBMCli_UpdStat :: Column ( const DBMCli_String & sOwner, 
                                      const DBMCli_String & sTable,
                                      const DBMCli_String & sColumn,
                                      DBMCli_UpdStateEstimate_Type nEstimate,
                                      const DBMCli_String & sEstimate,
                                      Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_Database & oDB = GetDatabase();

  if( oDB.GetState().Value() == DBMCLI_DBSTATE_WARM ) {

    DBMCli_String sSpec;

    if( sColumn == US_ALL ) {
      sSpec.Format( "COLUMN (*) FOR %s.%s", (const char *)sOwner, (const char *)sTable );
    } else {
      sSpec.Format( "COLUMN %s.%s.%s", (const char *)sOwner, (const char *)sTable, (const char *)sColumn );
    }

    sSpec += " ";
    sSpec += EstimateSpec( nEstimate, sEstimate );

    DBMCli_String sCmd( DBMCLI_CMD_SQLUPDSTAT );
    sCmd += " ";
    sCmd += sSpec;

    if( oDB.Execute( sCmd, oMsgList ) ) {
      bRC = true;
    }
  }

  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_UpdStat :: Table
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_UpdStat :: Table ( const DBMCli_String & sItem, 
                                     DBMCli_UpdStateEstimate_Type nEstimate,
                                     const DBMCli_String & sEstimate,
                                     Msg_List & oMsgList )
{
  DBMCli_String sOwner;
  DBMCli_String sTable;

  SplitTableItem( sItem, sOwner, sTable );

  return Table( sOwner, sTable, nEstimate, sEstimate, oMsgList );
}


SAPDB_Bool DBMCli_UpdStat :: Table ( const DBMCli_String & sOwner, 
                                     const DBMCli_String & sTable,
                                     DBMCli_UpdStateEstimate_Type nEstimate,
                                     const DBMCli_String & sEstimate,
                                     Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_Database & oDB = GetDatabase();

  if( oDB.GetState().Value() == DBMCLI_DBSTATE_WARM ) {

    DBMCli_String sSpec;

    sSpec.Format( "%s.%s", (const char *)sOwner, (const char *)sTable );

    sSpec += " ";
    sSpec += EstimateSpec( nEstimate, sEstimate );

    DBMCli_String sCmd( DBMCLI_CMD_SQLUPDSTAT );
    sCmd += " ";
    sCmd += sSpec;

    if( oDB.Execute( sCmd, oMsgList ) ) {
      bRC = true;
    }
  }

  return bRC;
}


/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/
void DBMCli_UpdStat :: SplitColumnItem( const DBMCli_String & sItem, 
                                        DBMCli_String & sOwner,
                                        DBMCli_String & sTable, 
                                        DBMCli_String & sColumn )
{
  SAPDB_Int iPos1;
  SAPDB_Int iPos2;

  sOwner.Empty( );
  sTable.Empty( );
  sColumn.Empty( );

  if( !sItem.IsEmpty( ) ) {
    iPos1 = 0;
    iPos2 = sItem.Find( '.', iPos1 );
    if( iPos2 >= 0 ) {
      sOwner = sItem.Mid( iPos1, iPos2 - iPos1 );
      iPos1 = iPos2 + 1;
      iPos2 = sItem.Find( '.', iPos1 );
      if( iPos2 >= 0 ) {
        sTable = sItem.Mid( iPos1, iPos2 - iPos1 );
        sColumn = sItem.Mid( iPos2 + 1 );
      }
    }
  }
}
 
void DBMCli_UpdStat :: SplitTableItem( const DBMCli_String & sItem, 
                                       DBMCli_String & sOwner,
                                       DBMCli_String & sTable )
{
  SAPDB_Int iPos1;
  SAPDB_Int iPos2;

  sOwner.Empty( );
  sTable.Empty( );

  if( !sItem.IsEmpty( ) ) {
    iPos1 = 0;
    iPos2 = sItem.Find( '.', iPos1 );
    if( iPos2 >= 0 ) {
      sOwner = sItem.Mid( iPos1, iPos2 - iPos1 );
      sTable = sItem.Mid( iPos2 + 1 );
    }
  }
}

const DBMCli_String DBMCli_UpdStat :: EstimateSpec(const DBMCli_UpdStateEstimate_Type nEstimate,
                                                   const DBMCli_String & sEstimate )
{
    DBMCli_String sSpec;

    sSpec = "";

    switch( nEstimate ) {
      case DBMCLI_UPDSTATESTIMATE_ROWS:
        sSpec.Format( "%s %s %s", US_ESTIMATE, (const char *)sEstimate, US_ESTROWS );
        break;
      case DBMCLI_UPDSTATESTIMATE_PERCENT:
        sSpec.Format( "%s %s %s", US_ESTIMATE, (const char *)sEstimate, US_ESTPCT );
        break;
    }

    return sSpec;
}

