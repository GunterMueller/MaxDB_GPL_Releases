/*! 
  -----------------------------------------------------------------------------
  module: DBMCli_History.cpp
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager Client Library
  description:  History Class
  version:      
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
#include "DBM/Cli/DBMCli_History.hpp"

/*
  -----------------------------------------------------------------------------
  constants 
  -----------------------------------------------------------------------------
*/
#define HISTORY_LINE_MEDIA   "M:"
#define HISTORY_LINE_EXTERN  "E:"

#define HISMED_SINGLE   "S"
#define HISMED_MULTI    "M"
#define HISMED_REPLACE  "R"

#define HISMED_FILE     "vftFile"
#define HISMED_TAPE     "vftTapeRew"
#define HISMED_PIPE     "vftFile"

#define EXT_NONE        "NONE"
#define EXT_AVAILABLE   "AVAILABLE"
#define EXT_UNAVAILABLE "UNAVAILABLE"

/*    
  =============================================================================
  class: DBMCli_HistoryMediaItem
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMCli_HistoryMediaItem :: DBMCli_HistoryMediaItem
  -----------------------------------------------------------------------------
*/
DBMCli_HistoryMediaItem :: DBMCli_HistoryMediaItem ( )
{
} // end DBMCli_HistoryMediaItem :: DBMCli_HistoryMediaItem
/*
  -----------------------------------------------------------------------------
*/
DBMCli_HistoryMediaItem :: DBMCli_HistoryMediaItem ( const DBMCli_String & sRow )
                         : m_sRow ( sRow )
{
} // end DBMCli_HistoryMediaItem :: DBMCli_HistoryMediaItem

/*    
  =============================================================================
  class: DBMCli_HistoryExternalItem
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMCli_HistoryExternalItem :: DBMCli_HistoryExternalItem
  -----------------------------------------------------------------------------
*/
DBMCli_HistoryExternalItem :: DBMCli_HistoryExternalItem ( )
{
} // end DBMCli_HistoryExternalItem :: DBMCli_HistoryExternalItem
/*
  -----------------------------------------------------------------------------
*/
DBMCli_HistoryExternalItem :: DBMCli_HistoryExternalItem ( const DBMCli_String & sRow )
                            : m_sRow ( sRow )
{
} // end DBMCli_HistoryExternalItem :: DBMCli_HistoryExternalItem

/*    
  =============================================================================
  class: DBMCli_HistoryItem
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMCli_HistoryItem :: DBMCli_HistoryItem
  -----------------------------------------------------------------------------
*/
DBMCli_HistoryItem :: DBMCli_HistoryItem ( )
{
  m_nResultNum = 0;
} 

DBMCli_HistoryItem :: DBMCli_HistoryItem ( DBMCli_Database * pDatabase, 
                                           DBMCli_History * pHistory,
                                           const DBMCli_String & sRow )
                    : m_pDatabase ( pDatabase ),
                      m_pHistory ( pHistory ),
                      m_sRow ( sRow )
{
  AssignProperties( );
} 

/*
  -----------------------------------------------------------------------------
  public destructor DBMCli_HistoryItem :: ~DBMCli_HistoryItem
  -----------------------------------------------------------------------------
*/
DBMCli_HistoryItem :: ~DBMCli_HistoryItem ( )
{
} 

/*
  -----------------------------------------------------------------------------
  public function DBMCli_HistoryItem :: Col
  -----------------------------------------------------------------------------
*/
DBMCli_String DBMCli_HistoryItem :: Col ( SAPDB_Int nCol ) const
{
  ASSERT( ( nCol >=0 ) && ( nCol < HISTORY_COLS ) );

  DBMCli_History & oHistory = GetHistory();

  DBMCli_String sValue;

  sValue.Empty( );
  sValue = m_sRow.Mid( oHistory.ColPos( nCol ), oHistory.ColLen( nCol ) );
  sValue.Trim( ); 
  
  return sValue;  
} // end DBMCli_HistoryItem :: Col

/*!
  -------------------------------------------------------------------------
  function:     DBMCli_HistoryItem :: MediumKind
  -------------------------------------------------------------------------
*/
DBMCli_HismedKind_Constants DBMCli_HistoryItem :: MediumKind (SAPDB_Int nRow) const

{ 
  DBMCli_String               sType = InfoCol(Row_Medium, nRow, HISMED_COL_KIND);
  DBMCli_HismedKind_Constants nType = HISMED_KIND_UNKNOWN;

  if (sType == HISMED_SINGLE ) {
    nType = HISMED_KIND_SINGLE;
  } else if (sType == HISMED_MULTI) {
    nType = HISMED_KIND_MULTI;
  } else if (sType == HISMED_REPLACE) {
    nType = HISMED_KIND_REPLACE;
  } // end if

  return nType;
} // end DBMCli_HistoryItem :: MediumKind

/*!
  -------------------------------------------------------------------------
  function:     DBMCli_HistoryItem :: ExternalStateValue
  -------------------------------------------------------------------------
*/
DBMCli_HistoryItem :: ExternalState DBMCli_HistoryItem :: ExternalStateValue (SAPDB_Int nRow) const
{ 
  DBMCli_String sState =  InfoCol(Row_Extern, nRow, HISEXT_COL_STATUS);
  ExternalState nState = Ext_Unknown;

  if (sState == EXT_NONE ) {
    nState = Ext_None;
  } else if (sState == EXT_AVAILABLE) {
    nState = Ext_Available;
  } else if (sState == EXT_UNAVAILABLE) {
    nState = Ext_Unavailable;
  } // end if

  return nState;
} // end DBMCli_HistoryItem :: ExternalStateValue

/*
  -----------------------------------------------------------------------------
  public function DBMCli_HistoryItem :: ResultStr
  -----------------------------------------------------------------------------
*/
DBMCli_String DBMCli_HistoryItem :: ResultStr ( ) const
{
  DBMCli_String sResult;

  if( m_nResultNum == 0 ) {
    if( m_nAction == HISTORY_ACTION_LOST ) {
      sResult.Empty( );
    } else {
      sResult = HISTORY_OK_TEXT;
    }
  } else {
    sResult = Col( HISTORY_COL_RESULTNUM ) + " " + Col( HISTORY_COL_RESULTTEXT );
  }

  return sResult;
} // end DBMCli_HistoryItem :: ResultStr

/*
  -----------------------------------------------------------------------------
  public function DBMCli_HistoryItem :: AddInfoRow
  -----------------------------------------------------------------------------
*/
void DBMCli_HistoryItem :: AddInfoRow ( const DBMCli_String & sRow, 
                                        RowType               nRowType )
{

  // add a "medium row" or a "external tool row" to the HistoryItem
  switch (nRowType) {
    case Row_Medium: 
      {
       DBMCli_HistoryMediaItem oItem( sRow );
       m_aMedia.Add( oItem );
       break;
      }
    case Row_Extern:
      {
       DBMCli_HistoryExternalItem oItem( sRow );
       m_aExternals.Add( oItem );
       break;
      }
  } // end switch


} // end DBMCli_HistoryItem :: AddInfoRow

/*
  -----------------------------------------------------------------------------
  private function DBMCli_HistoryItem :: InfoCol
  -----------------------------------------------------------------------------
*/
DBMCli_String DBMCli_HistoryItem :: InfoCol 
      ( RowType   nRowType, 
        SAPDB_Int nRow, 
        SAPDB_Int nCol ) const
{
  DBMCli_String sValue;
  SAPDB_Int     nMaxCols = 0;
  SAPDB_Int     nMaxRows = 0;
  SAPDB_Int     nInitPos = 0;

  switch (nRowType) {
  case Row_Medium:
    nMaxCols = HISMED_COLS;
    nMaxRows = m_aMedia.GetSize();
    nInitPos = (SAPDB_Int) strlen(HISTORY_LINE_MEDIA);  // skip the "M:"
    break;
  case Row_Extern:
    nMaxCols = HISEXT_COLS;
    nMaxRows = m_aExternals.GetSize();
    nInitPos = (SAPDB_Int) strlen(HISTORY_LINE_EXTERN);  // skip the "E:"
    break;
  } // end switch

  if ( ( nCol >=0 ) && ( nCol < nMaxCols) &&
       ( nRow >=0 ) && ( nRow < nMaxRows)    ) {

    SAPDB_Int     nColStart = nInitPos;
    SAPDB_Int     nColLen   = 0;
    SAPDB_Int     nColCount = 0;
    SAPDB_Int     nIndex    = nColStart;
    SAPDB_Bool    bFound    = false;
    DBMCli_String sRow;

    switch (nRowType) {
    case Row_Medium:
      sRow = m_aMedia[nRow].Row();
      break;
    case Row_Extern:
      sRow = m_aExternals[nRow].Row();
      break;
    } // end switch

    // find column
    for( nIndex = 0; (nIndex < sRow.GetLength()) && !bFound; ++nIndex ) {
      if( sRow[nIndex] == HISTORY_COL_SEP ) {
        nColLen = nIndex - nColStart;
        if (nColCount == nCol) {
          bFound = true;
        } else {
          ++nColCount;
          nColStart = nIndex + 1;
        } // end if
      }  // end if
    } // end for

    if (bFound) {
      sValue = sRow.Mid( nColStart, nColLen );
      sValue.Trim( ); 
    } // end if
    
  } // end if
  
  return sValue;  
} // end DBMCli_HistoryItem :: InfoCol

/*
  -----------------------------------------------------------------------------
  private function DBMCli_HistoryItem :: AssignProperties
  -----------------------------------------------------------------------------
*/
void DBMCli_HistoryItem :: AssignProperties( )
{
  AssignLabel( );
  AssignAction( );

  m_nResultNum = atoi( Col(HISTORY_COL_RESULTNUM) );

  m_bActiveForRecovery = false;
} // end DBMCli_HistoryItem :: AssignProperties

/*
  -----------------------------------------------------------------------------
  private function DBMCli_HistoryItem :: AssignLabel
  -----------------------------------------------------------------------------
*/
void DBMCli_HistoryItem :: AssignLabel( )
{
  DBMCli_String sLabel = LabelStr( );

  if( sLabel.GetLength( ) >= 3 ) { 
    if( sLabel.Left( 3 ) == HISTORY_LABEL_DAT_STR ) {
      m_nLabel = HISTORY_LABEL_DATA;
    } else if( sLabel.Left( 3 ) == HISTORY_LABEL_PAG_STR ) {
      m_nLabel = HISTORY_LABEL_PAGES;
    } else if( sLabel.Left( 3 ) == HISTORY_LABEL_LOG_STR ) {
      m_nLabel = HISTORY_LABEL_LOG;
    } else {
      m_nLabel = HISTORY_LABEL_UNKNOWN;
    }
  } else {
    m_nLabel = HISTORY_LABEL_UNKNOWN;
  }
} // end DBMCli_HistoryItem :: AssignLabel

/*
  -----------------------------------------------------------------------------
  private function DBMCli_HistoryItem :: AssignAction
  -----------------------------------------------------------------------------
*/
void DBMCli_HistoryItem :: AssignAction( )
{
  DBMCli_String sAction = ActionStr( );

  if( sAction.GetLength( ) >= 3 ) {
    if( sAction.Left( 3 ) == HISTORY_ACTION_HIS_STR ) {
      m_nAction = HISTORY_ACTION_LOST;
    } else if( sAction.Left( 3 ) == HISTORY_ACTION_SAV_STR ) {
      m_nAction = HISTORY_ACTION_SAVE;
    } else if( sAction.Left( 3 ) == HISTORY_ACTION_RES_STR ) {
      m_nAction = HISTORY_ACTION_RESTORE;
    } else {
      m_nAction = HISTORY_ACTION_UNKNOWN;
    }
  } else {
    m_nAction = HISTORY_ACTION_UNKNOWN;
  }
} // end DBMCli_HistoryItem :: AssignAction


/*    
  =============================================================================
  class: DBMCli_History
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMCli_History :: DBMCli_History
  -----------------------------------------------------------------------------
*/
DBMCli_History :: DBMCli_History ( )
{
  Clear( );
} 

/*
  -----------------------------------------------------------------------------
  public destructor DBMCli_History :: ~DBMCli_History
  -----------------------------------------------------------------------------
*/
DBMCli_History :: ~DBMCli_History ( )
{

} 

/*
  -----------------------------------------------------------------------------
  public function DBMCli_History :: Refresh
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_History :: Refresh ( Msg_List  & oMsgList )
{
  DBMCli_String   szKey;
  DBMCli_DateTime oUntil;

  return Refresh(Mode_Default, szKey, oUntil, oMsgList);
} // DBMCli_History :: Refresh
/*
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_History :: Refresh ( Mode                    nMode,
                                       Msg_List  & oMsgList )
{
  DBMCli_String   szKey;
  DBMCli_DateTime oUntil;

  return Refresh(nMode, szKey, oUntil, oMsgList);
} // DBMCli_History :: Refresh
/*
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_History :: Refresh ( Mode                    nMode,
                                       const DBMCli_DateTime & oUntil,
                                       Msg_List  & oMsgList )
{
  DBMCli_String   szKey;

  return Refresh(nMode, szKey, oUntil, oMsgList);
} // DBMCli_History :: Refresh
/*
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_History :: Refresh ( Mode                    nMode,
                                       const DBMCli_String   & szKey,
                                       Msg_List  & oMsgList )
{
  DBMCli_DateTime oUntil;

  return Refresh(nMode, szKey, oUntil, oMsgList);
} // DBMCli_History :: Refresh
/*
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_History :: Refresh ( Mode                    nMode,
                                       const DBMCli_String   & szKey,
                                       const DBMCli_DateTime & oUntil,
                                       Msg_List  & oMsgList )
{
  SAPDB_Bool bRC = false;

  if( Open( nMode, szKey, oUntil, oMsgList ) ) {
    DBMCli_String sRow;

    while( GetRow( sRow, oMsgList ) ) {

       DBMCli_HistoryItem oHistoryItem( m_pDatabase, this, sRow );

       sRow.Empty( ); 
       while (GetInfoRow(sRow, DBMCli_HistoryItem::Row_Medium)) {
         oHistoryItem.AddInfoRow(sRow, DBMCli_HistoryItem::Row_Medium);
         sRow.Empty( ); 
       } // end while
       sRow.Empty( ); 
       while (GetInfoRow(sRow, DBMCli_HistoryItem::Row_Extern)) {
         oHistoryItem.AddInfoRow(sRow, DBMCli_HistoryItem::Row_Extern);
         sRow.Empty( ); 
       } // end while

       m_aHistoryItem.Add( oHistoryItem );
       sRow.Empty( ); 
    } // end while

    bRC = true;
  } // end if

  return bRC;
} // DBMCli_History :: Refresh

/*
  -----------------------------------------------------------------------------
  public function DBMCli_History :: Close
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_History :: Close ( Msg_List & oMsgList )
{
  DBMCli_Database & oDB = GetDatabase();

  return oDB.Execute( DBMCLI_CMD_HISTORYCLOSE, oMsgList );
}



/*
  -----------------------------------------------------------------------------
  public function DBMCli_History :: InitForRecovery
  -----------------------------------------------------------------------------
*/
void DBMCli_History :: InitForRecovery( )
{

  SAPDB_Int  nCurrentItem = 0;
  SAPDB_Int  nLastItem    = 0;
    
  // activate all data and page saves
  for (nCurrentItem = 0; nCurrentItem < HistoryItemArray().GetSize(); ++nCurrentItem) {
    if ( (HistoryItemArray()[nCurrentItem].Label() == HISTORY_LABEL_PAGES) ||
         (HistoryItemArray()[nCurrentItem].Label() == HISTORY_LABEL_DATA )    ) {
      nLastItem = nCurrentItem;
      HistoryItemArray()[nCurrentItem].ActivateForRecovery();
    } // end if
  } // end for

  // activate all logs after the last item where the last logpage of the log save
  // greather than the next log page of the data/page save
  for (nCurrentItem = nLastItem + 1; nCurrentItem < HistoryItemArray().GetSize(); ++nCurrentItem) {
    if (HistoryItemArray()[nCurrentItem].ToPage() > HistoryItemArray()[nLastItem].LogPage()) {
      HistoryItemArray()[nCurrentItem].ActivateForRecovery();
    } // end if
  } // end for
  
/*
  
  SAPDB_Int  nCurrentItem = HistoryItemArray().GetSize();
  SAPDB_Bool bSkipLog = false;

  while (nCurrentItem > 0) {
    --nCurrentItem;
    if (HistoryItemArray()[nCurrentItem].Action() == HISTORY_ACTION_SAVE) {
      if (HistoryItemArray()[nCurrentItem].Label() == HISTORY_LABEL_LOG) {
        HistoryItemArray()[nCurrentItem].ActivateForRecovery(!bSkipLog);
      } else if (HistoryItemArray()[nCurrentItem].Label() == HISTORY_LABEL_PAGES) {
        HistoryItemArray()[nCurrentItem].ActivateForRecovery();
        bSkipLog = true;
      } else if (HistoryItemArray()[nCurrentItem].Label() == HISTORY_LABEL_DATA) {
        HistoryItemArray()[nCurrentItem].ActivateForRecovery();
        bSkipLog = true;
        break;
      } // end if
    } // end if
  } // end while
*/

} // end DBMCli_History :: InitForRecovery

/*
  -----------------------------------------------------------------------------
  private functions ...
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  private function DBMCli_History :: GetRow
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_History :: GetRow ( DBMCli_String        & sRow,
                                      Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_Database & oDB = GetDatabase();
  DBMCli_Result & oResult = oDB.GetResult( ); 

  if( oResult.GetLine( sRow ) ) {
    
    if( m_bLayout == false ) {
      GetColumnLayout( sRow );
      m_bLayout = true;
    }
    
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
} // DBMCli_History :: GetRow

/*
  -----------------------------------------------------------------------------
  private function DBMCli_History :: GetInfoRow
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_History :: GetInfoRow ( DBMCli_String               & sRow,
                                          DBMCli_HistoryItem::RowType   nRowType)
{
  SAPDB_Bool        bRC     = false;
  DBMCli_Database & oDB     = GetDatabase();
  DBMCli_Result   & oResult = oDB.GetResult( ); 

  SAPDB_Int nPos = oResult.GetPos();

  if( oResult.GetLine( sRow ) ) {
    if (GetRowType(sRow) == nRowType) {
      bRC = true;
    } else {
      oResult.SetPos(nPos);
    } // end if
  } // end if

  return bRC;
} // DBMCli_History :: GetInfoRow

/*
  -----------------------------------------------------------------------------
  private function DBMCli_History :: Clear
  -----------------------------------------------------------------------------
*/
void DBMCli_History :: Clear ( )
{
  m_bContinue = false;

  m_aHistoryItem.RemoveAll( );

  m_bLayout = false;

  for(SAPDB_Int i = 0; i < HISTORY_COLS; i++) {
    m_nColPos[i] = 0;
    m_nColLen[i] = 0;
  }

  m_nColCount = 0;
} // end DBMCli_History :: Clear

/*
  -----------------------------------------------------------------------------
  private function DBMCli_History :: Open
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_History :: Open ( Mode                    nMode,
                                    const DBMCli_String   & szKey,
                                    const DBMCli_DateTime & oUntil,
                                    Msg_List  & oMsgList )
{
  SAPDB_Bool bRC = false;

  Clear( );

  DBMCli_Database & oDB = GetDatabase();
  DBMCli_Result & oResult = oDB.GetResult( ); 


  // execute backup_history_open ...
  DBMCli_String sCmd( DBMCLI_CMD_HISTORYOPEN );
  if (nMode == Mode_RestLast || nMode == Mode_RestSelected) {
    sCmd = sCmd + " " + DBMCLI_OPT_HISTORY_EXTERN;
  } // end if
  if( oDB.Execute( sCmd, oMsgList ) ) {

    // prepare backup_history_list command
    sCmd = DBMCLI_CMD_HISTORYLIST;

    switch (nMode) {
      	    case Mode_RestAll: // -r *
        sCmd = sCmd                       + " " + 
               DBMCLI_OPT_HISTORY_RESTORE + " " +
               DBMCLI_OPT_HISTORY_RESTALL;
        break;
      
      case Mode_RestLast:     // -r LAST
        sCmd = sCmd                       + " " +  
               DBMCLI_OPT_HISTORY_MEDIA   + " " +
               DBMCLI_OPT_HISTORY_EXTERN  + " " +
               DBMCLI_OPT_HISTORY_RESTORE + " " +
               DBMCLI_OPT_HISTORY_RESTLAST;
        break;
      case Mode_RestSelected: // -r <key>
        sCmd = sCmd                       + " " +  
               DBMCLI_OPT_HISTORY_MEDIA   + " " +
               DBMCLI_OPT_HISTORY_EXTERN  + " " +
               DBMCLI_OPT_HISTORY_RESTORE + " " +
               szKey;
        break;
      case Mode_RestContinue: // -r CONTINUE
        sCmd = sCmd                       + " " +  
               DBMCLI_OPT_HISTORY_MEDIA   + " " +
               DBMCLI_OPT_HISTORY_EXTERN  + " " +
               DBMCLI_OPT_HISTORY_RESTORE + " " +
               DBMCLI_OPT_HISTORY_RESTCONT;
      case Mode_Default:
      default:
        // do nothing
        break;
    } // end switch

    // -u <yyyymmddhhmmss>
    if (!oUntil.IsNull()) {
      sCmd = sCmd                      + " " + 
             DBMCLI_OPT_HISTORY_UNTIL  + " " + 
             oUntil.Get(DBMCLI_DT_NUM_DATETIME_FMT);
    } // end if

    // execute backup_history_list ...
    if( oDB.Execute( sCmd, oMsgList ) ) {

      DBMCli_String sLine;

      oResult.GetLine( sLine ); 
      m_bContinue = ( sLine == DBMCLI_VAL_CONTINUE );

      bRC = true;
    }
  }

  return bRC;
} // end DBMCli_History :: Open

/*
  -----------------------------------------------------------------------------
  private function DBMCli_History :: Next
  -----------------------------------------------------------------------------
*/
SAPDB_Bool DBMCli_History :: Next ( Msg_List & oMsgList )
{
  SAPDB_Bool bRC = false;

  DBMCli_Database & oDB = GetDatabase();
  DBMCli_Result & oResult = oDB.GetResult( ); 

  DBMCli_String sCmd( DBMCLI_CMD_HISTORYNEXT );

  if( oDB.Execute( sCmd, oMsgList ) ) {
    DBMCli_String sLine;

    oResult.GetLine( sLine ); 
    m_bContinue = ( sLine == DBMCLI_VAL_CONTINUE );

    bRC = true;
  }

  return bRC;
}

/*
  -----------------------------------------------------------------------------
  private function DBMCli_History :: GetColumnLayout
  -----------------------------------------------------------------------------
*/
void DBMCli_History :: GetColumnLayout ( const DBMCli_String & sRow )
{
  SAPDB_Int i = 0;
  SAPDB_Int nCol = 0;

  m_nColPos[nCol] = 0;
  
  for( i = 0; i < sRow.GetLength(); i++ ) {
    if( sRow[i] == HISTORY_COL_SEP ) {
      m_nColLen[nCol] = i - m_nColPos[nCol];
      nCol++;
      if( nCol < HISTORY_COLS ) {
        m_nColPos[nCol] = i + 1;
      }
    }
  }

  m_nColCount = nCol;
} // end DBMCli_History :: GetColumnLayout

/*
  -----------------------------------------------------------------------------
  private function DBMCli_History :: GetRowType
  -----------------------------------------------------------------------------
*/
DBMCli_HistoryItem::RowType DBMCli_History :: GetRowType ( const DBMCli_String & sRow )
{
  DBMCli_HistoryItem::RowType       nRowType = DBMCli_HistoryItem::Row_Normal;
  DBMCli_String sMedia(HISTORY_LINE_MEDIA);
  DBMCli_String sExtern(HISTORY_LINE_EXTERN);

  if (sRow.Left(sMedia.GetLength()) == sMedia) {
    nRowType = DBMCli_HistoryItem::Row_Medium;
  } else if (sRow.Left(sExtern.GetLength()) == sExtern) {
    nRowType = DBMCli_HistoryItem::Row_Extern;
  } // end if

  return nRowType;
} // end DBMCli_History :: GetRowType

/*
  -----------------------------------------------------------------------------
  private function DBMCli_History :: ColPos
  -----------------------------------------------------------------------------
*/
SAPDB_Int DBMCli_History :: ColPos( const SAPDB_Int nCol )
{
  ASSERT( ( nCol >=0 ) && ( nCol < HISTORY_COLS ) );

  return m_nColPos[nCol];
} // end DBMCli_History :: ColPos

/*
  -----------------------------------------------------------------------------
  private function DBMCli_History :: ColLen
  -----------------------------------------------------------------------------
*/
SAPDB_Int DBMCli_History :: ColLen( const SAPDB_Int nCol )
{
  ASSERT( ( nCol >=0 ) && ( nCol < HISTORY_COLS ) );

  return m_nColLen[nCol];
} // end DBMCli_History :: ColLen
