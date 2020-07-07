/*! 
  -----------------------------------------------------------------------------
  module: DBMWeb_DBMWeb.hpp
  -----------------------------------------------------------------------------
  responsible:  BerndV
  special area: Database Manager WebAgent Extension
  description:  "the" DBMWeb Class
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

#ifndef _DBMWeb_DBMWeb_HPP_
#define _DBMWeb_DBMWeb_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "gsp00.h"
#include "hwd02waapi.h"

#include "DBM/Cli/DBMCli_Database.hpp"
#include "DBM/Cli/DBMCli_Wizard.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/
#define PARAM_EVENT               "Event"
#define PARAM_PAGE                "Page"

#define PARAM_ACTION              "Action"
#define PARAM_ACTION_VAL_REFRESH  "REFRESH"
#define PARAM_ACTION_VAL_VIEW     "VIEW"
#define PARAM_ACTION_VAL_EDIT     "EDIT"
#define PARAM_ACTION_VAL_PUT      "PUT"
#define PARAM_ACTION_VAL_ADD      "ADD"
#define PARAM_ACTION_VAL_DELETE   "DELETE"
#define PARAM_ACTION_VAL_START    "START"
#define PARAM_ACTION_VAL_REPLACE  "REPLACE"
#define PARAM_ACTION_VAL_IGNORE   "IGNORE"
#define PARAM_ACTION_VAL_CANCEL   "CANCEL"
#define PARAM_ACTION_VAL_EXECUTE  "EXECUTE"

#define PARAM_NAME                "Name"
#define PARAM_VALUE               "Value"

#define PARAM_BACK                "Back"

/*!
  -----------------------------------------------------------------------------
  Class:        DBMWeb_DBMWeb
  -----------------------------------------------------------------------------
  description:  An instance of this class represents the DBMWeb
  -----------------------------------------------------------------------------
*/
class DBMWeb_DBMWeb
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMWeb_DBMWeb
      -------------------------------------------------------------------------
      description:  Constructor
      -------------------------------------------------------------------------
    */
    DBMWeb_DBMWeb  ();
 
    /*!
      -------------------------------------------------------------------------
      function:     ~DBMWeb_DBMWeb
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMWeb_DBMWeb ( );

    /*!
      -------------------------------------------------------------------------
      function:     doService
      -------------------------------------------------------------------------
      description:  computing web request

      arguments:    wa             [INOUT]  - WebAgent object
                    request        [INOUT]  - Web request object
                    reply          [INOUT]  - Web reply object
                    bDeleteSession [OUT]    - what the caller with the websession 
                                              should do
      -------------------------------------------------------------------------
    */
    bool doService ( sapdbwa_WebAgent     & wa,
                     sapdbwa_HttpRequest  & request,
                     sapdbwa_HttpReply    & reply );

    /*!
      -------------------------------------------------------------------------
      function:     isValid
      -------------------------------------------------------------------------
      description:  check the object
      returnvalue:  true if valid
      -------------------------------------------------------------------------
    */
    bool isValid () {return ((m_Database != NULL) || (m_Wizard != 0));}
 
    /*!
      -------------------------------------------------------------------------
      function:     setSessionID
      -------------------------------------------------------------------------
    */
    void setSessionID (const char * sSessionID) {m_sSessionID = sSessionID;}

  private:

    bool GetParameterValue(const char * pName,
                           sapdbwa_HttpRequest & request,
                           DBMCli_String & sValue);

    bool GetParameterValueByIndex(const char * pName,
                                  int nIndex,
                                  sapdbwa_HttpRequest & request,
                                  DBMCli_String & sValue);

    void MovetoURL(const DBMCli_String & sURL, 
                   sapdbwa_HttpReply & reply );

    // ------------------------------------------------------------------------
    // CORE functions of the DBMWeb class
    // ------------------------------------------------------------------------
    #include "DBM/Web/DBMWeb_DBMWebCore.hpp"

    // ------------------------------------------------------------------------
    // TEST functions of the DBMWeb class
    // ------------------------------------------------------------------------
    #include "DBM/Web/DBMWeb_DBMWebTest.hpp"

    // ------------------------------------------------------------------------
    // INFO functions of the DBMWeb class
    // ------------------------------------------------------------------------
    #include "DBM/Web/DBMWeb_DBMWebState.hpp"

    // ------------------------------------------------------------------------
    // INFO functions of the DBMWeb class
    // ------------------------------------------------------------------------
    #include "DBM/Web/DBMWeb_DBMWebInfo.hpp"

    // ------------------------------------------------------------------------
    // BACKUP functions of the DBMWeb class
    // ------------------------------------------------------------------------
    #include "DBM/Web/DBMWeb_DBMWebBackup.hpp"

    // ------------------------------------------------------------------------
    // RECOVER functions of the DBMWeb class
    // ------------------------------------------------------------------------
    #include "DBM/Web/DBMWeb_DBMWebRecover.hpp"

    // ------------------------------------------------------------------------
    // TUNING functions of the DBMWeb class
    // ------------------------------------------------------------------------
    #include "DBM/Web/DBMWeb_DBMWebTuning.hpp"

    // ------------------------------------------------------------------------
    // CHECK functions of the DBMWeb class
    // ------------------------------------------------------------------------
    #include "DBM/Web/DBMWeb_DBMWebCheck.hpp"

    // ------------------------------------------------------------------------
    // CONFIGURATION functions of the DBMWeb class
    // ------------------------------------------------------------------------
    #include "DBM/Web/DBMWeb_DBMWebConfig.hpp"

    // ------------------------------------------------------------------------
    // WIZARD functions of the DBMWeb class
    // ------------------------------------------------------------------------
    #include "DBM/Web/DBMWeb_DBMWebWizard.hpp"

    DBMCli_Database   *  m_Database;
    DBMCli_Wizard     *  m_Wizard;

    // parameters for logon screen
    DBMCli_String m_sServer;
    DBMCli_String m_sDatabase;
    DBMCli_String m_sUser;

    // parameters the last URL
    DBMCli_String m_sLastWorkURL;

    // the session ID
    DBMCli_String m_sSessionID;
    DBMCli_String m_sNullSessionID;

    // theglobal message list object
    Msg_List m_oGlbMsgList;
}; 

/*! EndClass: DBMWeb_DBMWeb */

#endif // _DBMWeb_DBMWeb_HPP_

