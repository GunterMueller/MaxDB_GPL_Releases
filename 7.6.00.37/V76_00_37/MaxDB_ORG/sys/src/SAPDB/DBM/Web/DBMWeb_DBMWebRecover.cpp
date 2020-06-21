/*! 
  -----------------------------------------------------------------------------
  module: DBMWeb_DBMWebRecover.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  "Recover" functions of "the" DBMWeb Class
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
#include "DBM/Web/DBMWeb_TemplateBadDevspaces.hpp"
#include "DBM/Web/DBMWeb_DBMWeb.hpp"
#include "DBM/Web/DBMWeb_Web.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define PARAM_ACTION_VAL_REINTEGRATE "REINTEGRATE"

#define INFO_BADDEVS "BADDEVSPACES"

#define PARAM_ITEM "Item"

/*    
  =============================================================================
  class:        DBMWeb_DBMWeb
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: recoverIndexes
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMWeb_DBMWeb :: recoverIndexes( sapdbwa_WebAgent    & wa,
                                            sapdbwa_HttpRequest & request,
                                            sapdbwa_HttpReply   & reply )
{
  return tuningIndexes( wa, request, reply );
} 

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: recoverDevspaces
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMWeb_DBMWeb :: recoverDevspaces( sapdbwa_WebAgent    & wa,
                                              sapdbwa_HttpRequest & request,
                                              sapdbwa_HttpReply   & reply )
{

  DBMCli_String sAction;

  GetParameterValue(PARAM_ACTION, request, sAction);

  Msg_List oMsgList;

  if( sAction == PARAM_ACTION_VAL_REFRESH ) {

    DBMCli_Info & oInfo = m_Database->GetInfo( );

    DBMCli_String sFrom( INFO_BADDEVS );
    DBMCli_String sWhere( "" );

    if( oInfo.Open( sFrom, sWhere, oMsgList ) ) {
      DBMWeb_TemplateBadDevspaces oTemplate( wa, m_Database, DBMWEB_TEMPLBADDEVSPACES_MODE_BADDEVSPACES );
      oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
      oInfo.Close( oMsgList );
    } else {
      sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
    }

  } else if( sAction == PARAM_ACTION_VAL_REINTEGRATE ) {
    DBMCli_Devspaces & oDevspaces = m_Database->GetDevspaces( );

    SAPDB_Int nItem = 0;
    DBMCli_String sItem;
    SAPDB_Bool bOK = SAPDB_FALSE;

    if( m_Database->UTLConnect( oMsgList ) ) {
      do {
        bOK = SAPDB_FALSE;
        sItem.Empty( );
        GetParameterValueByIndex(PARAM_ITEM, nItem, request, sItem);
        if( !sItem.IsEmpty( ) ) {
          bOK = oDevspaces.Reintegrate( sItem, oMsgList );
          nItem++;
        } else {
          bOK = SAPDB_TRUE;
        }
      } while( !sItem.IsEmpty( ) && bOK );
      m_Database->UTLRelease( oMsgList );
    }

    if( bOK ) {
      DBMWeb_TemplateBadDevspaces oTemplate( wa, m_Database, DBMWEB_TEMPLBADDEVSPACES_MODE_RESULT );
      oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
    } else {
      sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
    }

  }

  return SAPDB_TRUE;
} 

