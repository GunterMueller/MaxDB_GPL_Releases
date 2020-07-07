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

#ifndef _DBMCLI_HISTORY_HPP_
#define _DBMCLI_HISTORY_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "DBM/Cli/DBMCli_Common.hpp"
#include "DBM/Cli/DBMCli_DateTime.hpp"
#include "DBM/Cli/DBMCli_Stuff.hpp"
#include "DBM/Cli/DBMCli_ResultBuf.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define HISTORY_COL_SEP '|'

#define HISTORY_COLS 15

#define HISTORY_COL_SYSKEY        0
#define HISTORY_COL_LABEL			    1
#define HISTORY_COL_ACTION		    2
#define HISTORY_COL_LASTSAVEPOINT 3
#define HISTORY_COL_FIRSTCOMMIT	  3
#define HISTORY_COL_LASTCOMMIT	  4
#define HISTORY_COL_STARTDATE		  5
#define HISTORY_COL_STOPDATE		  6
#define HISTORY_COL_LOGPAGE		    7
#define HISTORY_COL_FROMPAGE      7
#define HISTORY_COL_TOPAGE		    8
#define HISTORY_COL_LOGREQUIRED	  9
#define HISTORY_COL_MEDIANAME		  10
#define HISTORY_COL_SIZEPAGES		  11
#define HISTORY_COL_VOLUMES		    12
#define HISTORY_COL_RESULTNUM		  13
#define HISTORY_COL_RESULTTEXT	  14

#define HISMED_COLS 7

#define HISMED_COL_MEDIANAME       0
#define HISMED_COL_TYPE            1
#define HISMED_COL_VERSION         2
#define HISMED_COL_SIZE            3
#define HISMED_COL_BLOCKSIZE       4
#define HISMED_COL_KIND            5
#define HISMED_COL_LOCATION        6

#define HISEXT_COLS 2

#define HISEXT_COL_STATUS          0
#define HISEXT_COL_ID              1

#define HISTORY_OK_TEXT "OK"

#define HISTORY_LABEL_DAT_STR "DAT"
#define HISTORY_LABEL_PAG_STR "PAG"
#define HISTORY_LABEL_LOG_STR "LOG"

#define HISTORY_ACTION_HIS_STR "HIS"
#define HISTORY_ACTION_SAV_STR "SAV"
#define HISTORY_ACTION_RES_STR "RES"

/*
  -----------------------------------------------------------------------------
  forward declarations
  -----------------------------------------------------------------------------
*/
class DBMCli_Database;
class DBMCli_HistoryItem;
class DBMCli_HistoryMediaItem;
class DBMCli_HistoryExternalItem;
class DBMCli_History;

/*
  -----------------------------------------------------------------------------
  typedefs
  -----------------------------------------------------------------------------
*/
typedef enum 
{ 
	HISTORY_LABEL_UNKNOWN = 0,
	HISTORY_LABEL_DATA    = 1,
	HISTORY_LABEL_PAGES   = 2,
	HISTORY_LABEL_LOG     = 3
} DBMCli_HistoryLabel_Constants;

typedef enum 
{ 
	HISTORY_ACTION_UNKNOWN = 0,
	HISTORY_ACTION_SAVE    = 1,
	HISTORY_ACTION_RESTORE = 2,
	HISTORY_ACTION_LOST    = 3
} DBMCli_HistoryAction_Constants;

typedef enum 
{ 
	HISMED_KIND_UNKNOWN = 0,
	HISMED_KIND_SINGLE  = 1,
	HISMED_KIND_MULTI   = 2,
	HISMED_KIND_REPLACE = 3
} DBMCli_HismedKind_Constants;

typedef DBMCli_Array< DBMCli_HistoryItem,         DBMCli_HistoryItem >         DBMCli_HistoryItemArray;
typedef DBMCli_Array< DBMCli_HistoryMediaItem,    DBMCli_HistoryMediaItem >    DBMCli_HistoryMediaItemArray;
typedef DBMCli_Array< DBMCli_HistoryExternalItem, DBMCli_HistoryExternalItem > DBMCli_HistoryExternalItemArray;

/*!
  -----------------------------------------------------------------------------
  Class:       DBMCli_HistoryMediaItem
  -----------------------------------------------------------------------------
  description: Class to handle one backup history media item.
  -----------------------------------------------------------------------------
*/
class DBMCli_HistoryMediaItem
{
  public:
    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_HistoryMediaItem
      -------------------------------------------------------------------------
      description:  Constructor
      -------------------------------------------------------------------------
    */
    DBMCli_HistoryMediaItem ( );
    DBMCli_HistoryMediaItem ( const DBMCli_String & sRow );

    /*!
      -------------------------------------------------------------------------
      function:     Row
      -------------------------------------------------------------------------
    */
    const DBMCli_String & Row ( ) const { return m_sRow; };

  private:

    DBMCli_String m_sRow;

};

/*! EndClass: DBMCli_HistoryMediaItem */

/*!
  -----------------------------------------------------------------------------
  Class:       DBMCli_HistoryExternalItem
  -----------------------------------------------------------------------------
  description: Class to handle one backup history media item.
  -----------------------------------------------------------------------------
*/
class DBMCli_HistoryExternalItem
{
  public:
    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_HistoryExternalItem
      -------------------------------------------------------------------------
      description:  Constructor
      -------------------------------------------------------------------------
    */
    DBMCli_HistoryExternalItem ( );
    DBMCli_HistoryExternalItem ( const DBMCli_String & sRow );

    /*!
      -------------------------------------------------------------------------
      function:     Row
      -------------------------------------------------------------------------
    */
    const DBMCli_String & Row ( ) const { return m_sRow; };

  private:

    DBMCli_String m_sRow;

};

/*! EndClass: DBMCli_HistoryExternalItem */

/*!
  -----------------------------------------------------------------------------
  Class:       DBMCli_HistoryItem
  -----------------------------------------------------------------------------
  description: Class to handle one backup history item.
  -----------------------------------------------------------------------------
*/
class DBMCli_HistoryItem 
{
  public:

    typedef enum  
    { 
	    Row_Normal,
	    Row_Medium,
	    Row_Extern
    } RowType;

    typedef enum  
    { 
	    Ext_Unknown,
	    Ext_None,
	    Ext_Available,
	    Ext_Unavailable
    } ExternalState;

    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_HistoryItem
      -------------------------------------------------------------------------
      description:  Constructor
      prototypes:   2
      -------------------------------------------------------------------------
    */
    DBMCli_HistoryItem ( );
    DBMCli_HistoryItem ( DBMCli_Database * pDatabase, 
                         DBMCli_History * pHistory,
                         const DBMCli_String & sRow );

    /*!
      -------------------------------------------------------------------------
      function:     ~DBMCli_History
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMCli_HistoryItem ( );

    /*!
      -------------------------------------------------------------------------
      function:     SetDatabase
      -------------------------------------------------------------------------
      description:  Set the database member.
      -------------------------------------------------------------------------
    */
    void SetDatabase ( DBMCli_Database * p ) { m_pDatabase = p; };

    /*!
      -------------------------------------------------------------------------
      function:     GetDatabase
      -------------------------------------------------------------------------
      description:  Get the database member.
      return value: Reference to the database object.
      -------------------------------------------------------------------------
    */
    DBMCli_Database & GetDatabase ( ) const { return *m_pDatabase; };

    /*!
      -------------------------------------------------------------------------
      function:     SetHistory
      -------------------------------------------------------------------------
      description:  Set the History member.
      -------------------------------------------------------------------------
    */
    void SetHistory ( DBMCli_History * p ) { m_pHistory = p; };

    /*!
      -------------------------------------------------------------------------
      function:     GetHistory
      -------------------------------------------------------------------------
      description:  Get the History member.
      return value: Reference to the History object.
      -------------------------------------------------------------------------
    */
    DBMCli_History & GetHistory ( ) const { return *m_pHistory; };

    /*!
      -------------------------------------------------------------------------
      function:     Row
      -------------------------------------------------------------------------
    */
    const DBMCli_String & Row ( ) { return m_sRow; };

    /*!
      -------------------------------------------------------------------------
      function:     Col
      -------------------------------------------------------------------------
    */
    DBMCli_String Col ( SAPDB_Int nCol ) const;

    /*!
      -------------------------------------------------------------------------
      function:     SysKeyStr
      -------------------------------------------------------------------------
    */
    DBMCli_String SysKeyStr ( ) const {return Col( HISTORY_COL_SYSKEY );}

    /*!
      -------------------------------------------------------------------------
      function:     LabelStr
      -------------------------------------------------------------------------
    */
    DBMCli_String LabelStr ( ) const {return Col( HISTORY_COL_LABEL );} 
    /*!
      -------------------------------------------------------------------------
      function:     ActionStr
      -------------------------------------------------------------------------
    */
    DBMCli_String ActionStr ( ) const {return Col( HISTORY_COL_ACTION );}
    /*!
      -------------------------------------------------------------------------
      function:     LastSavePointStr
      -------------------------------------------------------------------------
    */
    DBMCli_String LastSavePointStr ( ) const {return Col( HISTORY_COL_LASTSAVEPOINT );}
    /*!
      -------------------------------------------------------------------------
      function:     FirstCommitStr
      -------------------------------------------------------------------------
    */
    DBMCli_String FirstCommitStr ( ) const {return Col( HISTORY_COL_FIRSTCOMMIT );}
    /*!
      -------------------------------------------------------------------------
      function:     LastCommitStr
      -------------------------------------------------------------------------
    */
    DBMCli_String LastCommitStr ( ) const {return Col( HISTORY_COL_LASTCOMMIT );}
    /*!
      -------------------------------------------------------------------------
      function:     StartDateStr
      -------------------------------------------------------------------------
    */
    DBMCli_String StartDateStr ( ) const {return Col( HISTORY_COL_STARTDATE );}
    /*!
      -------------------------------------------------------------------------
      function:     StopDateStr
      -------------------------------------------------------------------------
    */
    DBMCli_String StopDateStr ( ) const {return Col( HISTORY_COL_STOPDATE );}
    /*!
      -------------------------------------------------------------------------
      function:     LogPageStr
      -------------------------------------------------------------------------
    */
    DBMCli_String LogPageStr ( ) const {return Col( HISTORY_COL_LOGPAGE );}
    /*!
      -------------------------------------------------------------------------
      function:     LogPage
      -------------------------------------------------------------------------
    */
    SAPDB_Int     LogPage    ( ) const {return atoi(Col( HISTORY_COL_LOGPAGE ));}
    /*!
      -------------------------------------------------------------------------
      function:     FromPageStr
      -------------------------------------------------------------------------
    */
    DBMCli_String FromPageStr ( ) const {return Col( HISTORY_COL_FROMPAGE );}
    /*!
      -------------------------------------------------------------------------
      function:     FromPage
      -------------------------------------------------------------------------
    */
    SAPDB_Int     FromPage    ( ) const {return atoi(Col( HISTORY_COL_FROMPAGE ));}
    /*!
      -------------------------------------------------------------------------
      function:     ToPageStr
      -------------------------------------------------------------------------
    */
    DBMCli_String ToPageStr ( ) const {return Col( HISTORY_COL_TOPAGE );}
    /*!
      -------------------------------------------------------------------------
      function:     ToPage
      -------------------------------------------------------------------------
    */
    SAPDB_Int     ToPage    ( ) const {return atoi(Col( HISTORY_COL_TOPAGE ));}
    /*!
      -------------------------------------------------------------------------
      function:     LogRequiredStr
      -------------------------------------------------------------------------
    */
    DBMCli_String LogRequiredStr ( ) const {return Col( HISTORY_COL_LOGREQUIRED );}
    /*!
      -------------------------------------------------------------------------
      function:     MediaNameStr
      -------------------------------------------------------------------------
    */
    DBMCli_String MediaNameStr ( ) const {return Col( HISTORY_COL_MEDIANAME );}
    /*!
      -------------------------------------------------------------------------
      function:     SizePagesStr
      -------------------------------------------------------------------------
    */
    DBMCli_String SizePagesStr ( ) const {return Col( HISTORY_COL_SIZEPAGES );}
    /*!
      -------------------------------------------------------------------------
      function:     VolumesStr
      -------------------------------------------------------------------------
    */
    DBMCli_String VolumesStr ( ) const {return Col( HISTORY_COL_VOLUMES );}
    /*!
      -------------------------------------------------------------------------
      function:     ResultNumStr
      -------------------------------------------------------------------------
    */
    DBMCli_String ResultNumStr ( ) const {return Col( HISTORY_COL_RESULTNUM );}
    /*!
      -------------------------------------------------------------------------
      function:     ResultTextStr
      -------------------------------------------------------------------------
    */
    DBMCli_String ResultTextStr ( ) const {return Col( HISTORY_COL_RESULTTEXT );}
    /*!
      -------------------------------------------------------------------------
      function:     ResultStr
      -------------------------------------------------------------------------
    */
    DBMCli_String ResultStr ( ) const;

    /*!
      -------------------------------------------------------------------------
      function:     Label
      -------------------------------------------------------------------------
    */
    DBMCli_HistoryLabel_Constants Label (  ) const { return m_nLabel; };

    /*!
      -------------------------------------------------------------------------
      function:     Action
      -------------------------------------------------------------------------
    */
    DBMCli_HistoryAction_Constants Action (  ) const { return m_nAction; };

    /*!
      -------------------------------------------------------------------------
      function:     ResultNum
      -------------------------------------------------------------------------
    */
    SAPDB_Int ResultNum (  ) const { return m_nResultNum; };

    /*!
      -------------------------------------------------------------------------
      function:     AddInfoRow
      -------------------------------------------------------------------------
    */
    void AddInfoRow ( const DBMCli_String & sRow, 
                      RowType               nRowType );

    /*!
      -------------------------------------------------------------------------
      function:     IsActiveForRecovery
      -------------------------------------------------------------------------
    */
    SAPDB_Bool IsActiveForRecovery ( ) const { return m_bActiveForRecovery;}

    /*!
      -------------------------------------------------------------------------
      function:     ActivateForRecovery
      -------------------------------------------------------------------------
    */
    void ActivateForRecovery (SAPDB_Bool bValue = true) { m_bActiveForRecovery = bValue;}

    /*!
      -------------------------------------------------------------------------
      function:     MediaCount
      -------------------------------------------------------------------------
    */
    SAPDB_Int MediaCount () const { return m_aMedia.GetSize();}

    /*!
      -------------------------------------------------------------------------
      function:     MediumLocationStr
      -------------------------------------------------------------------------
    */
    DBMCli_String MediumLocationStr (SAPDB_Int nRow) const { return InfoCol(Row_Medium, nRow, HISMED_COL_LOCATION);}

    /*!
      -------------------------------------------------------------------------
      function:     MediumKind
      -------------------------------------------------------------------------
    */
    DBMCli_HismedKind_Constants MediumKind (SAPDB_Int nRow) const;

    /*!
      -------------------------------------------------------------------------
      function:     MediumDeviceTypeStr
      -------------------------------------------------------------------------
    */
    DBMCli_String MediumDeviceTypeStr (SAPDB_Int nRow) const { return InfoCol(Row_Medium, nRow, HISMED_COL_TYPE);}

    /*!
      -------------------------------------------------------------------------
      function:     ExternalCount
      -------------------------------------------------------------------------
    */
    SAPDB_Int ExternalCount () const { return m_aExternals.GetSize();}

    /*!
      -------------------------------------------------------------------------
      function:     ExternalStateValue
      -------------------------------------------------------------------------
    */
    ExternalState ExternalStateValue (SAPDB_Int nRow) const;
    
    /*!
      -------------------------------------------------------------------------
      function:     ExternalID
      -------------------------------------------------------------------------
    */
    DBMCli_String ExternalID (SAPDB_Int nRow) const { return InfoCol(Row_Extern, nRow, HISEXT_COL_ID);}

  private:
    void AssignProperties( );
    void AssignLabel( );
    void AssignAction( );

    DBMCli_String InfoCol (RowType nRowType, SAPDB_Int nRow, SAPDB_Int nCol) const;

  private:
    DBMCli_Database * m_pDatabase;
    DBMCli_History * m_pHistory;

    DBMCli_String m_sRow;

    DBMCli_HistoryLabel_Constants  m_nLabel;
    DBMCli_HistoryAction_Constants m_nAction;
    SAPDB_Int                      m_nResultNum;

    SAPDB_Bool                     m_bActiveForRecovery;

    DBMCli_HistoryMediaItemArray    m_aMedia;
    DBMCli_HistoryExternalItemArray m_aExternals;

}; 

/*! EndClass: DBMCli_HistoryItem */

/*!
  -----------------------------------------------------------------------------
  Class:       DBMCli_History
  -----------------------------------------------------------------------------
  description: Class to handle backup history.
  -----------------------------------------------------------------------------
*/
class DBMCli_History 
{
  public:

    typedef enum  
    { 
	    Mode_Default,
	    Mode_RestAll,
	    Mode_RestLast,
	    Mode_RestContinue,
	    Mode_RestSelected
    } Mode;

    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_History
      -------------------------------------------------------------------------
      description:  Constructor
      -------------------------------------------------------------------------
    */
    DBMCli_History ( );

    /*!
      -------------------------------------------------------------------------
      function:     ~DBMCli_History
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMCli_History ( );

    /*!
      -------------------------------------------------------------------------
      function:     SetDatabase
      -------------------------------------------------------------------------
      description:  Set the database member.
      -------------------------------------------------------------------------
    */
    void SetDatabase ( DBMCli_Database * p ) { m_pDatabase = p; };

    /*!
      -------------------------------------------------------------------------
      function:     GetDatabase
      -------------------------------------------------------------------------
      description:  Get the database member.
      return value: Reference to the database object.
      -------------------------------------------------------------------------
    */
    DBMCli_Database & GetDatabase ( ) { return *m_pDatabase; };

    /*!
      -------------------------------------------------------------------------
      function:     Refresh
      -------------------------------------------------------------------------
      description:  Refresh the array of history items.
      arguments:    nMode    [IN]  - list mode
                    szKey    [IN]  - history key to list
                    oUntil   [IN]  - until date and time
                    oMsgList [OUT] - object for indication of errors
      return value: True if success, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Refresh             ( Msg_List  & oMsgList );
    SAPDB_Bool Refresh             ( Mode                    nMode,
                                     Msg_List  & oMsgList );
    SAPDB_Bool Refresh             ( Mode                    nMode,
                                     const DBMCli_DateTime & oUntil,
                                     Msg_List  & oMsgList );
    SAPDB_Bool Refresh             ( Mode                    nMode,
                                     const DBMCli_String   & szKey,
                                     Msg_List  & oMsgList );
    SAPDB_Bool Refresh             ( Mode                    nMode,
                                     const DBMCli_String   & szKey,
                                     const DBMCli_DateTime & oUntil,
                                     Msg_List  & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     Close
      -------------------------------------------------------------------------
      description:  Close a history.
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True if success, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Close ( Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     HistoryItemArray 
      -------------------------------------------------------------------------
      description:  Get the array of history items.
      return value: Reference to a array of history items.
      -------------------------------------------------------------------------
    */
    DBMCli_HistoryItemArray & HistoryItemArray ( ) { return m_aHistoryItem; }; 

    /*!
      -------------------------------------------------------------------------
      function:     InitForRecovery
      -------------------------------------------------------------------------
      description:  initalize the array for recovery
      -------------------------------------------------------------------------
    */
    void InitForRecovery ( ); 

  private:
    void                           Clear           ( );

    SAPDB_Bool                     Open            ( Mode nMode,
                                                     const DBMCli_String & szKey,
                                                     const DBMCli_DateTime & oUntil,
                                                     Msg_List & oMsgList );

    SAPDB_Bool                     Next            ( Msg_List & oMsgList );
    
    void                           GetColumnLayout ( const DBMCli_String & sRow );
    
    DBMCli_HistoryItem::RowType    GetRowType      ( const DBMCli_String & sRow );
    
    SAPDB_Bool                     GetInfoRow      ( DBMCli_String & sRow, 
                                                     DBMCli_HistoryItem::RowType nRowType );
    
    SAPDB_Bool                     GetRow          ( DBMCli_String & sRow,
                                                     Msg_List & oMsgList );

    SAPDB_Int                      ColPos          ( const SAPDB_Int nCol );
    
    SAPDB_Int                      ColLen          ( const SAPDB_Int nCol );
    
    SAPDB_Int                      ColCount        ( ) { return m_nColCount; };

  private:
    DBMCli_Database * m_pDatabase;

    SAPDB_Bool m_bContinue;

    DBMCli_HistoryItemArray m_aHistoryItem;

    SAPDB_Bool m_bLayout;

    SAPDB_Int m_nColPos[HISTORY_COLS];
    SAPDB_Int m_nColLen[HISTORY_COLS];
    SAPDB_Int m_nColCount;

    friend class DBMCli_HistoryItem;
}; 

/*! EndClass: DBMCli_History */

#endif // _DBMCLI_HISTORY_HPP_

