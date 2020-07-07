/*! 
  -----------------------------------------------------------------------------
  module: DBMWeb_DBMWebInfo.cpp
  -----------------------------------------------------------------------------
  responsible:  BerndV
  special area: Database Manager WebAgent Extension
  description:  "Info" functions of "the" DBMWeb Class
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
#include "DBM/Web/DBMWeb_TemplateHistory.hpp"
#include "DBM/Web/DBMWeb_DBMWeb.hpp"
#include "DBM/Web/DBMWeb_Web.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define PARAM_EVENT               "Event"

#define PARAM_ACTION              "Action"
#define PARAM_ACTION_VAL_REFRESH  "REFRESH"

/*    
  =============================================================================
  class:        DBMWeb_DBMWeb
  =============================================================================
 */

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: infoBackupHistory
  -----------------------------------------------------------------------------
 */
SAPDB_Bool DBMWeb_DBMWeb :: infoBackupHistory ( sapdbwa_WebAgent    & wa,
                                                sapdbwa_HttpRequest & request,
                                                sapdbwa_HttpReply   & reply )
{
  DBMCli_String sAction;

  GetParameterValue(PARAM_ACTION, request, sAction);

  Msg_List oMsgList;
  DBMCli_History & oHistory = m_Database->GetHistory( );

  if( sAction == PARAM_ACTION_VAL_REFRESH ) {

    if( oHistory.Refresh( oMsgList ) ) {

      DBMWeb_TemplateHistory oTemplate( wa, oHistory, oHistory.HistoryItemArray( ) );
    
      oTemplate.writePage( Tools_TemplateWriterWA( reply ) );

    } else {
      sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
    }
  }

  return SAPDB_TRUE;
} // end DBMWeb_DBMWeb :: infoBackupHistory

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: infoCaches
  -----------------------------------------------------------------------------
 */
SAPDB_Bool DBMWeb_DBMWeb :: infoCaches ( sapdbwa_WebAgent    & wa,
                                         sapdbwa_HttpRequest & request,
                                         sapdbwa_HttpReply   & reply )
{
  return info(DBMCLI_INFO_CACHES, wa, request, reply); 
} // end DBMWeb_DBMWeb :: infoCaches

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: infoData
  -----------------------------------------------------------------------------
 */
SAPDB_Bool DBMWeb_DBMWeb :: infoData ( sapdbwa_WebAgent    & wa,
                                       sapdbwa_HttpRequest & request,
                                       sapdbwa_HttpReply   & reply )
{
  return info(DBMCLI_INFO_DATA, wa, request, reply); 
} // end DBMWeb_DBMWeb :: infoData

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: infoIO
  -----------------------------------------------------------------------------
 */
SAPDB_Bool DBMWeb_DBMWeb :: infoIO ( sapdbwa_WebAgent    & wa,
                                     sapdbwa_HttpRequest & request,
                                     sapdbwa_HttpReply   & reply )
{
  return info(DBMCLI_INFO_IO, wa, request, reply); 
} // end DBMWeb_DBMWeb :: infoIO

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: infoLog
  -----------------------------------------------------------------------------
 */
SAPDB_Bool DBMWeb_DBMWeb :: infoLog ( sapdbwa_WebAgent    & wa,
                                      sapdbwa_HttpRequest & request,
                                      sapdbwa_HttpReply   & reply )
{
  return info(DBMCLI_INFO_LOG, wa, request, reply); 
} // end DBMWeb_DBMWeb :: infoLog

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: infoLocks
  -----------------------------------------------------------------------------
 */
SAPDB_Bool DBMWeb_DBMWeb :: infoLocks ( sapdbwa_WebAgent    & wa,
                                        sapdbwa_HttpRequest & request,
                                        sapdbwa_HttpReply   & reply )
{
  return info(DBMCLI_INFO_LOCKS, wa, request, reply); 
} // end DBMWeb_DBMWeb :: infoLocks

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: infoSessions
  -----------------------------------------------------------------------------
 */
SAPDB_Bool DBMWeb_DBMWeb :: infoSessions ( sapdbwa_WebAgent    & wa,
                                           sapdbwa_HttpRequest & request,
                                           sapdbwa_HttpReply   & reply )
{
  return info(DBMCLI_INFO_USERS, wa, request, reply); 
} // end DBMWeb_DBMWeb :: infoSessions

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: infoVersions
  -----------------------------------------------------------------------------
 */
SAPDB_Bool DBMWeb_DBMWeb :: infoVersions ( sapdbwa_WebAgent    & wa,
                                           sapdbwa_HttpRequest & request,
                                           sapdbwa_HttpReply   & reply )
{
  return info(DBMCLI_INFO_VERSIONS, wa, request, reply); 
} // end DBMWeb_DBMWeb :: infoVersions

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: info
  -----------------------------------------------------------------------------
 */
SAPDB_Bool DBMWeb_DBMWeb :: info ( const DBMCli_String         & sName,
                                         sapdbwa_WebAgent      & wa,
                                         sapdbwa_HttpRequest   & request,
                                         sapdbwa_HttpReply     & reply )
{
  DBMCli_String sEvent;
  DBMCli_String sAction;

  GetParameterValue(PARAM_EVENT, request, sEvent);
  GetParameterValue(PARAM_ACTION, request, sAction);

  Msg_List oMsgList;

  DBMCli_Info & oInfo = m_Database->GetInfo( );

  if( sAction == PARAM_ACTION_VAL_REFRESH ) {
    if( oInfo.Open( sName, oMsgList ) ) {

      DBMWeb_TemplateInfoTable oTable( wa, sEvent, oInfo );

      oTable.writePage( Tools_TemplateWriterWA( reply ) );

      oInfo.Close( oMsgList );

    } else {
      sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
    }
  }

  return SAPDB_TRUE;
} // end DBMWeb_DBMWeb :: info

