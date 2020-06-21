/*!
  -----------------------------------------------------------------------------
  module: DBMCli_Recover.hpp
  -----------------------------------------------------------------------------
  responsible:  Bernd Vorsprach (Bernd.Vorsprach@sap.com)
  special area: Database Manager Client Library
  description:  Recover Class
  version:      7.3

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

#ifndef _DBMCLI_RECOVER_HPP_
#define _DBMCLI_RECOVER_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "DBM/Cli/DBMCli_History.hpp"
#include "DBM/Cli/DBMCli_Backup.hpp"
#include "DBM/Cli/DBMCli_Media.hpp"

/*
  -----------------------------------------------------------------------------
  forward declarations
  -----------------------------------------------------------------------------
*/
class DBMCli_Database;
class DBMCli_RecoverItem;
class DBMCli_Recover;

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define DBMCLI_RESTARTINFO_INVALID (-1)
/*
  -----------------------------------------------------------------------------
  typedefs
  -----------------------------------------------------------------------------
*/
typedef DBMCli_BackupResult DBMCli_RecoverResult;

/*!
  -----------------------------------------------------------------------------
  Class:       DBMCli_RecoverItem
  -----------------------------------------------------------------------------
  description: Class to handle one backup history media item.
  -----------------------------------------------------------------------------
*/
class DBMCli_RecoverItem
{
  public:

    /*!
      -------------------------------------------------------------------------
      Declaration:  Public types
      Description:  Public types used by public member functions.
      -------------------------------------------------------------------------
    */
    typedef DBMCli_Array< DBMCli_String, DBMCli_String > LocationArray;

    typedef enum
    {
  	  ActionUnknown = 0,
	    ActionStart   = 1,
	    ActionReplace = 2,
	    ActionIgnore  = 3,
	    ActionCancel  = 4
    } RecoverAction;
    typedef enum
    {
  	  TypeUnknown   = 0,
	    TypeData      = 1,
	    TypePages     = 2,
	    TypeLog       = 3
    } RecoverType;
    /*! EndDeclaration:  Public types */

    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_RecoverItem
      -------------------------------------------------------------------------
      description:  Constructor
      arguments:    aAction     [IN] - the action
                    aType       [IN] - the type
                    sLabel      [IN] - the label
                    sMediumName [IN] - the medium
                    oLocations  [IN] - the location
      -------------------------------------------------------------------------
    */
    DBMCli_RecoverItem( );
    DBMCli_RecoverItem( RecoverAction                 aAction,
                        RecoverType                   aType,
                        const DBMCli_String         & sLabel,
                        const DBMCli_String         & sMediumName,
                        const LocationArray         & oLocations,
                        const DBMCli_String         & sEBID,
                        DBMCli_MediumDeviceTypeEnum   m_aDeviceType);

    /*!
      -------------------------------------------------------------------------
      function:     GetAction
      -------------------------------------------------------------------------
      description:  get the action
      -------------------------------------------------------------------------
    */
    RecoverAction GetAction( ) const {return m_aAction;}

    /*!
      -------------------------------------------------------------------------
      function:     SetAction
      -------------------------------------------------------------------------
      description:  set the action
      -------------------------------------------------------------------------
    */
    void SetAction(RecoverAction aAction) {m_aAction = aAction;}

    /*!
      -------------------------------------------------------------------------
      function:     GetType
      -------------------------------------------------------------------------
      description:  get the type
      -------------------------------------------------------------------------
    */
    RecoverType GetType( ) const {return m_aType;}

    /*!
      -------------------------------------------------------------------------
      function:     GetTypeStr
      -------------------------------------------------------------------------
      description:  get the type as string
      -------------------------------------------------------------------------
    */
    DBMCli_String GetTypeStr( ) const;

    /*!
      -------------------------------------------------------------------------
      function:     GetLabel
      -------------------------------------------------------------------------
      description:  get the label
      -------------------------------------------------------------------------
    */
    DBMCli_String GetLabel( ) const {return m_sLabel;}

    /*!
      -------------------------------------------------------------------------
      function:     GetMediumName
      -------------------------------------------------------------------------
      description:  get the medium name
      -------------------------------------------------------------------------
    */
    DBMCli_String GetMediumName( ) const {return m_sMediumName;}

    /*!
      -------------------------------------------------------------------------
      function:     Location
      -------------------------------------------------------------------------
      description:  get a location
      -------------------------------------------------------------------------
    */
    DBMCli_String GetLocation(SAPDB_Int nIndex ) const {return m_oLocations[nIndex];}

    /*!
      -------------------------------------------------------------------------
      function:     Location
      -------------------------------------------------------------------------
      description:  set a location
      -------------------------------------------------------------------------
    */
    void SetLocation(DBMCli_String sLocation, SAPDB_Int nIndex = 0) { m_oLocations[nIndex] = sLocation;}

    /*!
      -------------------------------------------------------------------------
      function:     GetLocationName
      -------------------------------------------------------------------------
    */
    DBMCli_String GetLocationName (SAPDB_Int nRow) const;

    /*!
      -------------------------------------------------------------------------
      function:     GetVersion
      -------------------------------------------------------------------------
    */
    DBMCli_String GetVersion (SAPDB_Int nRow) const;

    /*!
      -------------------------------------------------------------------------
      function:     LocationCount
      -------------------------------------------------------------------------
      description:  get the number of locations
      -------------------------------------------------------------------------
    */
    SAPDB_Int LocationCount( ) const {return m_oLocations.GetSize();}

    /*!
      -------------------------------------------------------------------------
      function:     GetEBID
      -------------------------------------------------------------------------
      description:  get a EBID
      -------------------------------------------------------------------------
    */
    DBMCli_String GetEBID( ) const {return m_sEBID;}

    /*!
      -------------------------------------------------------------------------
      function:     GetDeviceType
      -------------------------------------------------------------------------
      description:  get the device type
      -------------------------------------------------------------------------
    */
    DBMCli_MediumDeviceTypeEnum GetDeviceType( ) const {return m_aDeviceType;}

  private:
    RecoverAction               m_aAction;
    RecoverType                 m_aType;
    DBMCli_String               m_sLabel;
    DBMCli_String               m_sMediumName;
    LocationArray               m_oLocations;
    DBMCli_String               m_sEBID;
    DBMCli_MediumDeviceTypeEnum m_aDeviceType;
};

/*! EndClass: DBMCli_RecoverItem */

/*!
  -----------------------------------------------------------------------------
  Class:       DBMCli_Recover
  -----------------------------------------------------------------------------
  description: Class to handle teh recovery of a database
  -----------------------------------------------------------------------------
*/
class DBMCli_Recover
{
  public:

    /*!  
      Chapter: Types and Constants
     */

    /*!
      -------------------------------------------------------------------------
      Declaration:  Public types
      Description:  Public types used by public member functions.
      -------------------------------------------------------------------------
    */
    typedef DBMCli_Array< DBMCli_RecoverItem, DBMCli_RecoverItem > ItemArray;

    typedef enum
    {
  	  RecoverStateUnknown       = 0,
	    RecoverStatePrepared      = 1,
	    RecoverStateRunning       = 2,
	    RecoverStateClosed        = 3,
	    RecoverStateAsking        = 4,
	    RecoverStateReplace       = 5,
	    RecoverStateCancelled     = 6,
	    RecoverStateError         = 7,
	    RecoverStateRestarted     = 8
    } RecoverState;

    typedef enum
    {
  	  MediaInfoNone        = 0,
	    MediaInfoFirstLog    = 1,
	    MediaInfoNextData    = 3,
	    MediaInfoErrNextLog  = 4,
	    MediaInfoErrNextData = 5
    } MediaInfo;

    /*! EndDeclaration:  Public types */

    /*!  
      EndChapter: Types and Constants
     */

    /*!  
      Chapter: Construction and destruction of class
     */

    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_Recover
      -------------------------------------------------------------------------
      description:  Constructor
      -------------------------------------------------------------------------
    */
    DBMCli_Recover ( );


    /*!
      -------------------------------------------------------------------------
      function:     ~DBMCli_Recover
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMCli_Recover ( );

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
      EndChapter: Construction and destruction of class
     */

    /*!  
      Chapter: Preparing Recovery List
     */

    /*!
      -------------------------------------------------------------------------
      function:     PrepareRecover
      -------------------------------------------------------------------------
      description:  create recover items from the input object
      arguments:    oHistoryItems   [IN]  - a list of history items
                    oSingleMedium   [IN]  - a single medium
                    oParallelMedium [IN]  - a parallel medium
                    oUntil          [IN]  - the "recover until" value
                    bCheck          [IN]  - the "backup check" flag
      prototypes:   3
      -------------------------------------------------------------------------
    */
    void PrepareRecover ( const DBMCli_HistoryItemArray & oHistoryItems,
                          const DBMCli_DateTime         & oUntil, 
                          SAPDB_Bool                      bCheck,
                          SAPDB_Bool                      bActivate);
    void PrepareRecover ( const DBMCli_Medium           & oSingleMedium,
                          const DBMCli_DateTime         & oUntil, 
                          SAPDB_Bool                      bCheck,
                          SAPDB_Bool                      bActivate);
    void PrepareRecover ( const DBMCli_MediumParallel   & oParallelMedium,
                          const DBMCli_DateTime         & oUntil, 
                          SAPDB_Bool                      bCheck,
                          SAPDB_Bool                      bActivate);

    /*!
      -------------------------------------------------------------------------
      function:     CopyLastItem
      -------------------------------------------------------------------------
      description:  copies the last with the specified action
      -------------------------------------------------------------------------
    */
    void CopyLastItem ( DBMCli_RecoverItem::RecoverAction aAction);

    /*!
      -------------------------------------------------------------------------
      function:     RecoverItemArray
      -------------------------------------------------------------------------
      description:  Get the array of recover items.
      return value: Reference to a array of recover items.
      -------------------------------------------------------------------------
    */
    const ItemArray & RecoverItemArray ( ) const { return m_aItems; };
          ItemArray & RecoverItemArray ( )       { return m_aItems; };

    /*!  
      EndChapter: Preparing Recovery List
     */

    /*!  
      Chapter: Get Current Recovery State
     */

    /*!
      -------------------------------------------------------------------------
      function:     CurrentItem
      -------------------------------------------------------------------------
      description:  Get the index of the current (next to execute or in execution) 
                    item
      return value: the index
      -------------------------------------------------------------------------
    */
    SAPDB_Int CurrentItem ( ) const { return m_nCurrent; };

    /*!
      -------------------------------------------------------------------------
      function:     GetRecoverState
      -------------------------------------------------------------------------
      description:  this function returns true the recover state
      retrun value: recover state
      -------------------------------------------------------------------------
    */
    RecoverState GetRecoverState (  ) const {return m_RecoverState;}

    /*!
      -------------------------------------------------------------------------
      function:     IsCheck
      -------------------------------------------------------------------------
      description:  this function returns the check flag
      retrun value: check flag
      -------------------------------------------------------------------------
    */
    SAPDB_Bool IsCheck (  ) const {return m_bCheck;}

    /*!
      -------------------------------------------------------------------------
      function:     GetUntil
      -------------------------------------------------------------------------
      description:  this function returns the until date/time
      retrun value: check flag
      -------------------------------------------------------------------------
    */
    DBMCli_DateTime GetUntil (  ) const {return m_oUntil;}

    /*!
      -------------------------------------------------------------------------
      function:     AskForMediaInfo
      -------------------------------------------------------------------------
      description:  this function returns which media info is required
      retrun value: teh required info
      -------------------------------------------------------------------------
    */
    MediaInfo AskForMediaInfo (  ) const;

    /*!  
      EndChapter: Get Current Recovery State
     */

    /*!  
      Chapter: Control the recovery
     */

    /*!
      -------------------------------------------------------------------------
      function:     RecoverNextRequest
      -------------------------------------------------------------------------
      description:  executes the next revovery step
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True, if no error occurred, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool RecoverNextRequest ( Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     ReplyAvailable
      -------------------------------------------------------------------------
      description:  check for a backup reply
      arguments:    bAvailable [OUT] - available flag
                    oMsgList   [OUT] - object for indication of errors
      return value: True, if no error occurred, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool ReplyAvailable ( SAPDB_Bool           & bAvailable,
                                Msg_List & oMsgList );


    /*!
      -------------------------------------------------------------------------
      function:     ReplyReceive
      -------------------------------------------------------------------------
      description:  receives the backup reply
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True, if no error occurred, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool ReplyReceive ( Msg_List & oMsgList );

    /*!
      -------------------------------------------------------------------------
      function:     Restart
      -------------------------------------------------------------------------
      description:  restarts the database
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True, if no error occurred, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Restart ( Msg_List & oMsgList );

    /*!  
      EndChapter: Control the recovery
     */

    /*!
      -------------------------------------------------------------------------
      function:     IgnorePossible
      -------------------------------------------------------------------------
      description:  is a recover_ignore possible
      -------------------------------------------------------------------------
    */
    SAPDB_Bool IgnorePossible ( ) const;

    /*!
      -------------------------------------------------------------------------
      function:     State
      -------------------------------------------------------------------------
      description:  retrieves the state of the recover action.
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True, if no error occurred, otherwise false.
      -------------------------------------------------------------------------
    */

    SAPDB_Bool State ( Msg_List & oMsgList );
   /*!
      -------------------------------------------------------------------------
      function:     Result
      -------------------------------------------------------------------------
      description:  Get the recover result.
      return value: Reference to the recover result object.
      -------------------------------------------------------------------------
    */
    DBMCli_RecoverResult & Result ( ) { return m_oResult; };

    /*!
      -------------------------------------------------------------------------
      function:     LastDBReturn
      -------------------------------------------------------------------------
      description:  this function returns the last DB return code
      -------------------------------------------------------------------------
    */
    SAPDB_Int LastDBReturn (  ) const {return m_nLastDBReturn;}

    /*!
      -------------------------------------------------------------------------
      function:     LastDBTxt
      -------------------------------------------------------------------------
      description:  this function returns the last DB return text
      -------------------------------------------------------------------------
    */
    DBMCli_String LastDBTxt (  ) const {return m_sLastDBTxt;}

  private:
    void       Init               (  );
    SAPDB_Bool NextMediumRequired ( Msg_List  & oMsgList);
    SAPDB_Bool Connect            ( DBMCli_Database       & oDB, 
                                    SAPDB_Bool              bCheck,
                                    Msg_List  & oMsgList) const;
    SAPDB_Bool Release            ( DBMCli_Database       & oDB, 
                                    SAPDB_Bool              bCheck,
                                    Msg_List  & oMsgList) const;

    typedef enum
    {
  	  RecoverModeUnknown   = 0,
	    RecoverModeHistory   = 1,
	    RecoverModeMedium    = 2
    } RecoverMode;

    RecoverMode               m_RecoverMode;
    DBMCli_DateTime           m_oUntil;
    SAPDB_Bool                m_bCheck;
    SAPDB_Bool                m_bActivate;
    RecoverState              m_RecoverState;

    DBMCli_Database         * m_pDatabase;
    ItemArray                 m_aItems;
    SAPDB_Int                 m_nCurrent;
    SAPDB_Int                 m_nIgnoreCount;
    SAPDB_Int                 m_nLastDBReturn;
    DBMCli_String             m_sLastDBTxt;

    DBMCli_RecoverResult      m_oResult;
};

/*! EndClass: DBMCli_Recover */

/*!
  -----------------------------------------------------------------------------
  Class:       DBMCli_RestartInfo
  -----------------------------------------------------------------------------
  description: Class to handle the restart info
  -----------------------------------------------------------------------------
*/
class DBMCli_RestartInfo
{
  public:
    const static SAPDB_Int MAX_LOG_PAGE;

    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_RestartInfo
      -------------------------------------------------------------------------
      description:  Constructor
      -------------------------------------------------------------------------
    */
    DBMCli_RestartInfo ( );

    /*!
      -------------------------------------------------------------------------
      function:     ~DBMCli_RestartInfo
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMCli_RestartInfo ( );

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
      function:     Refresh
      -------------------------------------------------------------------------
      description:  refreshes the restart info
      return value: true or false
      -------------------------------------------------------------------------
    */
    void Refresh ( );

    /*!
      -------------------------------------------------------------------------
      function:     GetUsedLogPage
      -------------------------------------------------------------------------
      description:  Get the used log page
      return value: the value
      -------------------------------------------------------------------------
    */
    SAPDB_Int GetUsedLogPage ( ) const { return m_nUsedLogPage; };

    /*!
      -------------------------------------------------------------------------
      function:     GetFirstLogPage
      -------------------------------------------------------------------------
      description:  Get the first log page
      return value: the value
      -------------------------------------------------------------------------
    */
    SAPDB_Int GetFirstLogPage ( ) const { return m_nFirstLogPage; };

    /*!
      -------------------------------------------------------------------------
      function:     Restartable
      -------------------------------------------------------------------------
      description:  Get the restartable flag
      return value: the value
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Restartable ( ) const { return m_bRestartable; };

    /*!
      -------------------------------------------------------------------------
      function:     IsLogEmpty
      -------------------------------------------------------------------------
      description:  ask for empty log
      return value: empty or not
      -------------------------------------------------------------------------
    */
    SAPDB_Bool IsLogEmpty ( ) const { return (m_nFirstLogPage == 0x7FFFFFFF); };

  private:

    DBMCli_Database         * m_pDatabase;
    SAPDB_Int                 m_nUsedLogPage;
    SAPDB_Int                 m_nFirstLogPage;
    SAPDB_Bool                m_bRestartable;

};

/*! EndClass: DBMCli_RestartInfo */

#endif // _DBMCLI_RECOVER_HPP_
