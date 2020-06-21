/*! 
  -----------------------------------------------------------------------------
  module: DBMWeb_DBMWebCheck.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  Database state funtions of the DBMWeb_DBMWeb class
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
#include "DBM/Web/DBMWeb_TemplateState.hpp"
#include "DBM/Web/DBMWeb_DBMWeb.hpp"
#include "DBM/Web/DBMWeb_Web.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define PARAM_ACTION              "Action"
#define PARAM_ACTION_VAL_REFRESH  "REFRESH"
#define PARAM_ACTION_VAL_VIEW     "VIEW"
#define PARAM_ACTION_VAL_OFFLINE  "OFFLINE"
#define PARAM_ACTION_VAL_COLD     "COLD"
#define PARAM_ACTION_VAL_WARM     "WARM"

/*    
  =============================================================================
  class:        DBMWeb_DBMWeb
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: dbState
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMWeb_DBMWeb :: dbState ( sapdbwa_WebAgent    & wa,
                                      sapdbwa_HttpRequest & request,
                                      sapdbwa_HttpReply   & reply )
{
  sapdbwa_StringSeq values; 

  DBMCli_String sAction;

  if( request.GetParameterValues( PARAM_ACTION, values ) == sapdbwa_True )
    if( values.GetNumElem( ) > 0 ) 
      sAction = values[0];

  Msg_List oMsgList;
  DBMCli_State & oState = m_Database->GetState( );

  SAPDB_Bool bOK = SAPDB_FALSE;

  if( sAction == PARAM_ACTION_VAL_REFRESH ) {
    bOK = SAPDB_TRUE;
  } else if( sAction == PARAM_ACTION_VAL_OFFLINE ) {
    bOK = m_Database->Offline( oMsgList );
  } else if( sAction == PARAM_ACTION_VAL_COLD ) {
    bOK = m_Database->Cold( oMsgList );
  } else if( sAction == PARAM_ACTION_VAL_WARM ) {
    bOK = m_Database->Warm( oMsgList );
  }

  if( bOK ) {
    bOK = oState.Refresh( oMsgList );
  } else {
    Msg_List oTmpMsgList;
    oState.Refresh( oTmpMsgList );
  } // end if

  if( !bOK ) {
    m_oGlbMsgList = oMsgList;
  } // end if
  DBMWeb_TemplateState oTemplate( wa, oState, !bOK );
  oTemplate.writePage( Tools_TemplateWriterWA( reply ) );

  return SAPDB_TRUE;
} 

