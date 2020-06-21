/*! 
  -----------------------------------------------------------------------------
  module: DBMWeb_DBMWebTuning.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager WebAgent Extension
  description:  "Tuning" functions of "the" DBMWeb Class
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
#include "DBM/Web/DBMWeb_TemplateIndexes.hpp"
#include "DBM/Web/DBMWeb_DBMWeb.hpp"
#include "DBM/Web/DBMWeb_Web.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define PARAM_EVENT_VAL_RECOVERINDEXES "RECOVER_INDEXES"

#define PARAM_ACTION_VAL_SELECT       "SELECT"
#define PARAM_ACTION_VAL_UPDSTATCOL   "UPDSTATCOL"
#define PARAM_ACTION_VAL_UPDSTATTAB   "UPDSTATTAB"
#define PARAM_ACTION_VAL_ENABLE       "ENABLE"
#define PARAM_ACTION_VAL_DISABLE      "DISABLE"
#define PARAM_ACTION_VAL_DELETE       "DELETE"
#define PARAM_ACTION_VAL_RECREATEALL  "RECREATEALL"
#define PARAM_ACTION_VAL_RECREATE     "RECREATE"

#define PARAM_OWNER               "Owner"
#define PARAM_TABLE               "Table"
#define PARAM_COLUMN              "Column"
#define PARAM_INDEX               "Index"
#define PARAM_FROM                "From"
#define PARAM_MAXROWS             "MaxRows"

#define INFO_UPDSTATWANTED        "UPDSTATW"
#define INFO_UPDSTAT              "UPDSTAT"
#define INFO_COLUMNS              "COLUMNS"
#define INFO_INDEXES              "INDEXES"
#define INFO_DISABLEIDX           "DISABLEDIDX"
#define INFO_UNUSEDIDX            "UNUSEDIDX"
#define INFO_BADIDX               "BADIDX"


#define PARAM_ESTIMATE            "Estimate"
#define PARAM_ESTIMATETYPE        "EstimateType"
#define PARAM_ESTIMATEROWS        "EstimateRows"
#define PARAM_ESTIMATEPCT         "EstimatePercent"
#define PARAM_INCCOLS             "IncCols"

#define ESTIMATE_ROWS             "ROWS"
#define ESTIMATE_PERCENT          "PERCENT"

#define PARAM_INCCOLS             "IncCols"
#define YES_VAL                   "YES"
#define NO_VAL                    "NO"

#define PARAM_ITEM                "Item"

//


/*    
  =============================================================================
  class:        DBMWeb_DBMWeb
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: tuningStat
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMWeb_DBMWeb :: tuningStat( sapdbwa_WebAgent    & wa,
                                        sapdbwa_HttpRequest & request,
                                        sapdbwa_HttpReply   & reply )
{
  DBMCli_String sAction;

  GetParameterValue(PARAM_ACTION, request, sAction);

  Msg_List oMsgList;

  if( sAction == PARAM_ACTION_VAL_REFRESH ) {

    DBMWeb_TemplateUpdStat oTemplate( wa, m_Database, DBMWEB_TEMPLUPDSTAT_MODE_SELECT );
    oTemplate.writePage( Tools_TemplateWriterWA( reply ) );

  } else if( sAction == PARAM_ACTION_VAL_SELECT ) {

    DBMCli_String sOwner;
    DBMCli_String sTable;
    DBMCli_String sFrom;
    DBMCli_String sMaxRows;

    DBMWeb_TemplUpdStat_Mode nMode( DBMWEB_TEMPLUPDSTAT_MODE_COLUMNS );

    sFrom = INFO_UPDSTATWANTED;

    GetParameterValue( PARAM_OWNER, request, sOwner );
    GetParameterValue( PARAM_TABLE, request, sTable );
    GetParameterValue( PARAM_FROM, request, sFrom );
    GetParameterValue( PARAM_MAXROWS, request, sMaxRows );

    // get where clause
    DBMCli_String sWhere = "";

    if( !sOwner.IsEmpty( ) ) {
      if( sOwner != "*" ) {
        if( m_Database->NodeInfo().Version().GetNumber() < 0x7060000 ) {
          sWhere += " \"Owner\" LIKE '";
        } else {
          sWhere += " \"Schema Name\" LIKE '";
        }
        sWhere += sOwner;
        sWhere += "'";
      }
    }

    if( !sTable.IsEmpty( ) ) {
      if( sTable != "*" ) {

        if( !sWhere.IsEmpty( ) ) {
           sWhere += " AND";
        }

        sWhere += " \"Table Name\" LIKE '";
        sWhere += sTable;
        sWhere += "'";
      }
    }

    if( sFrom == INFO_UPDSTAT ) {
      nMode = DBMWEB_TEMPLUPDSTAT_MODE_TABLES;
    }

    SAPDB_Int4 nMaxRows = atol(sMaxRows);

    DBMCli_Info & oInfo = m_Database->GetInfo( );

    if( oInfo.Open( sFrom, sWhere, oMsgList ) ) {

      DBMWeb_TemplateUpdStat oTemplate( wa, m_Database, nMode );
      oTemplate.SetMaxRows( nMaxRows );
      oTemplate.writePage( Tools_TemplateWriterWA( reply ) );

      oInfo.Close( oMsgList );

    } else {
      sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
    }

  } else if( ( sAction == PARAM_ACTION_VAL_UPDSTATCOL ) ||
             ( sAction == PARAM_ACTION_VAL_UPDSTATTAB )    ) {

    DBMCli_String sEstimate;
    DBMCli_String sEstimateType;
    DBMCli_String sEstimateRows;
    DBMCli_String sEstimatePercent;
    DBMCli_String sIncludeCols;

    GetParameterValue( PARAM_ESTIMATE, request, sEstimate );
    GetParameterValue( PARAM_ESTIMATETYPE, request, sEstimateType );
    GetParameterValue( PARAM_ESTIMATEROWS, request, sEstimateRows );
    GetParameterValue( PARAM_ESTIMATEPCT, request, sEstimatePercent );
    GetParameterValue( PARAM_INCCOLS, request, sIncludeCols );

    DBMCli_UpdStateEstimate_Type nEstimate = DBMCLI_UPDSTATESTIMATE_NONE;

    if( sEstimate == YES_VAL ) {
      sEstimate.Empty( );
      if ( sEstimateType == ESTIMATE_ROWS ) {
        nEstimate = DBMCLI_UPDSTATESTIMATE_ROWS;
        sEstimate = sEstimateRows;
      } else if ( sEstimateType == ESTIMATE_PERCENT ) {
        nEstimate = DBMCLI_UPDSTATESTIMATE_PERCENT;
        sEstimate = sEstimatePercent;
      }
    }
    
    DBMCli_UpdStat & oUpdStat = m_Database->GetUpdStat( );

    SAPDB_Int nItem = 0;
    DBMCli_String sItem;
    SAPDB_Bool bOK = SAPDB_FALSE;

    do {
      bOK = SAPDB_FALSE;
      sItem.Empty( );
      GetParameterValueByIndex(PARAM_ITEM, nItem, request, sItem);
      if( !sItem.IsEmpty( ) ) {
        if( sAction == PARAM_ACTION_VAL_UPDSTATCOL ) {
          bOK = oUpdStat.Column( sItem, nEstimate, sEstimate, oMsgList );
        } else if ( sAction == PARAM_ACTION_VAL_UPDSTATTAB ) {
          bOK = oUpdStat.Table( sItem, nEstimate, sEstimate, oMsgList );
          if( bOK ) {
            if( sIncludeCols == YES_VAL ) {
              sItem += ".*";
              bOK = oUpdStat.Column( sItem, nEstimate, sEstimate, oMsgList );
            }
          }
        }
        nItem++;
      } else {
        bOK = SAPDB_TRUE;
      }
    } while( !sItem.IsEmpty( ) && bOK );

    if( bOK ) {
      DBMWeb_TemplateUpdStat oTemplate( wa, m_Database, DBMWEB_TEMPLUPDSTAT_MODE_RESULT );
      oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
    } else {
      sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
    }


  } 

  return SAPDB_TRUE;
} 

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: tuningIndexes
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMWeb_DBMWeb :: tuningIndexes( sapdbwa_WebAgent    & wa,
                                           sapdbwa_HttpRequest & request,
                                           sapdbwa_HttpReply   & reply )
{
  DBMCli_String sEvent;
  DBMCli_String sAction;

  GetParameterValue(PARAM_EVENT, request, sEvent);
  GetParameterValue(PARAM_ACTION, request, sAction);

  Msg_List oMsgList;

  SAPDB_Bool bRecoverMode = SAPDB_FALSE;

  if( sEvent == PARAM_EVENT_VAL_RECOVERINDEXES ) {
    bRecoverMode = SAPDB_TRUE;
  }

  if( sAction == PARAM_ACTION_VAL_REFRESH ) {

    DBMWeb_TemplateIndexes oTemplate( wa, m_Database, DBMWEB_TEMPLINDEXES_MODE_SELECT );
    oTemplate.SetRecoverMode( bRecoverMode );
    oTemplate.writePage( Tools_TemplateWriterWA( reply ) );

  } else if( sAction == PARAM_ACTION_VAL_SELECT ) {

    DBMCli_String sOwner;
    DBMCli_String sTable;
    DBMCli_String sIndex;
    DBMCli_String sFrom;
    DBMCli_String sMaxRows;

    sFrom = INFO_INDEXES;

    GetParameterValue( PARAM_OWNER, request, sOwner );
    GetParameterValue( PARAM_TABLE, request, sTable );
    GetParameterValue( PARAM_INDEX, request, sIndex );
    GetParameterValue( PARAM_FROM, request, sFrom );
    GetParameterValue( PARAM_MAXROWS, request, sMaxRows );

    // get where clause
    DBMCli_String sWhere = "";

    if( !sOwner.IsEmpty( ) ) {
      if( sOwner != "*" ) {
        if( m_Database->NodeInfo().Version().GetNumber() < 0x7060000 ) {
          sWhere += " \"Owner\" LIKE '";
        } else {
          sWhere += " \"Schema Name\" LIKE '";
        }
        sWhere += sOwner;
        sWhere += "'";
      }
    }

    if( !sTable.IsEmpty( ) ) {
      if( sTable != "*" ) {

        if( !sWhere.IsEmpty( ) ) {
           sWhere += " AND";
        }

        sWhere += " \"Table Name\" LIKE '";
        sWhere += sTable;
        sWhere += "'";
      }
    }

    if( !sIndex.IsEmpty( ) ) {
      if( sIndex != "*" ) {

        if( !sWhere.IsEmpty( ) ) {
           sWhere += " AND";
        }

        sWhere += " \"Index Name\" LIKE '";
        sWhere += sIndex;
        sWhere += "'";
      }
    }

    SAPDB_Bool bDetails = SAPDB_FALSE;

    if( sFrom == INFO_INDEXES ) {
      bDetails = SAPDB_TRUE;
    }

    SAPDB_Int4 nMaxRows = atol(sMaxRows);

    DBMCli_Info & oInfo = m_Database->GetInfo( );

    if( oInfo.Open( sFrom, sWhere, oMsgList ) ) {

      DBMWeb_TemplateIndexes oTemplate( wa, m_Database, DBMWEB_TEMPLINDEXES_MODE_INDEXES );
      oTemplate.SetRecoverMode( bRecoverMode );
      oTemplate.SetDetails( bDetails );
      oTemplate.SetMaxRows( nMaxRows );
      oTemplate.writePage( Tools_TemplateWriterWA( reply ) );

      oInfo.Close( oMsgList );

    } else {
      sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
    }

  } else if( ( sAction == PARAM_ACTION_VAL_ENABLE )   ||
             ( sAction == PARAM_ACTION_VAL_DISABLE )  || 
             ( sAction == PARAM_ACTION_VAL_DELETE )   || 
             ( sAction == PARAM_ACTION_VAL_RECREATE )    ) {

    DBMCli_Indexes & oIndexes = m_Database->GetIndexes( );

    SAPDB_Int nItem = 0;
    DBMCli_String sItem;
    SAPDB_Bool bOK = SAPDB_FALSE;

    if( m_Database->SQLConnect( oMsgList ) ) {
      do {
        bOK = SAPDB_FALSE;
        sItem.Empty( );
        GetParameterValueByIndex(PARAM_ITEM, nItem, request, sItem);
        if( !sItem.IsEmpty( ) ) {

          if( sAction == PARAM_ACTION_VAL_ENABLE ) {
            bOK = oIndexes.Enable( sItem, oMsgList );
          } else if ( sAction == PARAM_ACTION_VAL_DISABLE ) {
            bOK = oIndexes.Disable( sItem, oMsgList );
          } else if ( sAction == PARAM_ACTION_VAL_DELETE ) {
            bOK = oIndexes.Delete( sItem, oMsgList );
          } else if ( sAction == PARAM_ACTION_VAL_RECREATE ) {
            bOK = oIndexes.Recreate( sItem, oMsgList );
          }

          nItem++;

        } else {
          bOK = SAPDB_TRUE;
        }
      } while( !sItem.IsEmpty( ) && bOK );
      m_Database->SQLRelease( oMsgList );
    }

    if( bOK ) {
      DBMWeb_TemplateIndexes oTemplate( wa, m_Database, DBMWEB_TEMPLINDEXES_MODE_RESULT );
      oTemplate.SetRecoverMode( bRecoverMode );
      oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
    } else {
      sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
    }

  } else if( sAction == PARAM_ACTION_VAL_RECREATEALL ) {

    DBMCli_Indexes & oIndexes = m_Database->GetIndexes( );

    SAPDB_Int nItem = 0;
    DBMCli_String sItem;
    SAPDB_Bool bOK = SAPDB_FALSE;

    if( m_Database->SQLConnect( oMsgList ) ) {
      bOK =  oIndexes.RecreateAll( oMsgList );
      m_Database->SQLRelease( oMsgList );
    }

    if( bOK ) {
      DBMWeb_TemplateIndexes oTemplate( wa, m_Database, DBMWEB_TEMPLINDEXES_MODE_RESULT );
      oTemplate.SetRecoverMode( bRecoverMode );
      oTemplate.writePage( Tools_TemplateWriterWA( reply ) );
    } else {
      sendMsgListError( wa, request, reply, oMsgList, m_Database->DatabaseName() );
    }

  } 

  return SAPDB_TRUE;
} 

