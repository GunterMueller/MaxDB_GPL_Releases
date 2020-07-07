/*! 
  -----------------------------------------------------------------------------
  module: DBMCli_Indexes.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager Client Library
  description:  Indexes Class
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
#include "DBM/Cli/DBMCli_Indexes.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/

/*    
  =============================================================================
  class: DBMCli_Indexes
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMCli_Indexes :: DBMCli_Indexes
  -----------------------------------------------------------------------------
*/
DBMCli_Indexes :: DBMCli_Indexes ( )
{
} 

/*
  -----------------------------------------------------------------------------
  public destructor DBMCli_Indexes :: ~DBMCli_Indexes
  -----------------------------------------------------------------------------
*/
DBMCli_Indexes :: ~DBMCli_Indexes ( )
{
} 
 
/*
  -----------------------------------------------------------------------------
  public function DBMCli_Indexes :: Enable
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Indexes :: Enable ( const DBMCli_String & sItem, 
                                      Msg_List & oMsgList )
{
  DBMCli_String sOwner;
  DBMCli_String sTable;
  DBMCli_String sIndex;

  SplitIndexItem( sItem, sOwner, sTable, sIndex );

  return Enable( sOwner, sTable, sIndex, oMsgList );
}

SAPDB_Bool DBMCli_Indexes :: Enable ( const DBMCli_String & sOwner, 
                                      const DBMCli_String & sTable,
                                      const DBMCli_String & sIndex,
                                      Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_Database & oDB = GetDatabase();

  if( oDB.GetState().Value() == DBMCLI_DBSTATE_WARM ) {

    DBMCli_String sCmd;

    sCmd.Format( "ALTER INDEX %s ENABLE", (const char *)IndexOnSpec( sOwner, sTable, sIndex ) );

    if( oDB.SQLExecute( sCmd, oMsgList ) ) {
      bRC = true;
    }
  }

  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Indexes :: Disable
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Indexes :: Disable ( const DBMCli_String & sItem, 
                                       Msg_List & oMsgList )
{
  DBMCli_String sOwner;
  DBMCli_String sTable;
  DBMCli_String sIndex;

  SplitIndexItem( sItem, sOwner, sTable, sIndex );

  return Disable( sOwner, sTable, sIndex, oMsgList );
}

SAPDB_Bool DBMCli_Indexes :: Disable ( const DBMCli_String & sOwner, 
                                       const DBMCli_String & sTable,
                                       const DBMCli_String & sIndex,
                                       Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_Database & oDB = GetDatabase();

  if( oDB.GetState().Value() == DBMCLI_DBSTATE_WARM ) {

    DBMCli_String sCmd;

    sCmd.Format( "ALTER INDEX %s DISABLE", (const char *)IndexOnSpec( sOwner, sTable, sIndex ) );

    if( oDB.SQLExecute( sCmd, oMsgList ) ) {
      bRC = true;
    }
  }

  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Indexes :: Delete
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Indexes :: Delete ( const DBMCli_String & sItem, 
                                      Msg_List & oMsgList )
{
  DBMCli_String sOwner;
  DBMCli_String sTable;
  DBMCli_String sIndex;

  SplitIndexItem( sItem, sOwner, sTable, sIndex );

  return Delete( sOwner, sTable, sIndex, oMsgList );
}

SAPDB_Bool DBMCli_Indexes :: Delete ( const DBMCli_String & sOwner, 
                                      const DBMCli_String & sTable,
                                      const DBMCli_String & sIndex,
                                      Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_Database & oDB = GetDatabase();

  if( oDB.GetState().Value() == DBMCLI_DBSTATE_WARM ) {

    DBMCli_String sCmd;

    sCmd.Format( "DROP INDEX %s", (const char *)IndexOnSpec( sOwner, sTable, sIndex ) );

    if( oDB.SQLExecute( sCmd, oMsgList ) ) {
      bRC = true;
    }
  }

  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Indexes :: Recreate
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Indexes :: Recreate ( const DBMCli_String & sItem, 
                                      Msg_List & oMsgList )
{
  DBMCli_String sOwner;
  DBMCli_String sTable;
  DBMCli_String sIndex;

  SplitIndexItem( sItem, sOwner, sTable, sIndex );

  return Recreate( sOwner, sTable, sIndex, oMsgList );
}

SAPDB_Bool DBMCli_Indexes :: Recreate ( const DBMCli_String & sOwner, 
                                        const DBMCli_String & sTable,
                                        const DBMCli_String & sIndex,
                                        Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_Database & oDB = GetDatabase();

  if( oDB.GetState().Value() == DBMCLI_DBSTATE_WARM ) {

    DBMCli_String sCmd;

    sCmd.Format( "SQL_RECREATEINDEX \"%s\".\"%s\".\"%s\"", (const char *)sOwner,
                                                           (const char *)sTable,
                                                           (const char *)sIndex );
    if( oDB.Execute( sCmd, oMsgList ) ) {
      bRC = true;
    }
  }

  return bRC;
}

/*
  -----------------------------------------------------------------------------
  public function DBMCli_Indexes :: RecreateAll
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_Indexes :: RecreateAll ( Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_Database & oDB = GetDatabase();

  if( oDB.GetState().Value() == DBMCLI_DBSTATE_WARM ) {

    DBMCli_String sCmd( "SQL_RECREATEINDEX" );

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
void DBMCli_Indexes :: SplitIndexItem( const DBMCli_String & sItem, 
                                       DBMCli_String & sOwner,
                                       DBMCli_String & sTable, 
                                       DBMCli_String & sIndex )
{
  SAPDB_Int iPos1;
  SAPDB_Int iPos2;

  sOwner.Empty( );
  sTable.Empty( );
  sIndex.Empty( );

  if( !sItem.IsEmpty( ) ) {
    iPos1 = 0;
    iPos2 = sItem.Find( '.', iPos1 );
    if( iPos2 >= 0 ) {
      sOwner = sItem.Mid( iPos1, iPos2 - iPos1 );
      iPos1 = iPos2 + 1;
      iPos2 = sItem.Find( '.', iPos1 );
      if( iPos2 >= 0 ) {
        sTable = sItem.Mid( iPos1, iPos2 - iPos1 );
        sIndex = sItem.Mid( iPos2 + 1 );
      }
    }
  }
}

const DBMCli_String DBMCli_Indexes :: IndexOnSpec(const DBMCli_String & sOwner,
                                                  const DBMCli_String & sTable, 
                                                  const DBMCli_String & sIndex )
{
    DBMCli_String sSpec;

    sSpec.Format( "\"%s\" ON \"%s\".\"%s\"",(const char *)sIndex, 
                                            (const char *)sOwner,
                                            (const char *)sTable );
    return sSpec;
}
