/*! 
  -----------------------------------------------------------------------------
  module: DBMWeb_DBMWeb.cpp
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

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include <stdio.h>
#include "DBM/Web/DBMWeb_DBMWeb.hpp"
#include "DBM/Web/DBMWeb_Web.hpp"
#include "DBM/Web/DBMWeb_TemplateMsgBox.hpp"

/*
  -----------------------------------------------------------------------------
  private macros
  -----------------------------------------------------------------------------
*/

// Events
#define EV_EMPTY                        0
#define EV_DBM_LOGON                    1
#define EV_DBM_LOGOFF                   2 
#define EV_ENUMDATABASES                3
#define EV_DBM_DUMMY                    4
#define EV_DBM_INTERN_TEST              5
#define EV_DB_STATE                     6
#define EV_INFO_BAKHIS                  7
#define EV_INFO_CACHES                  8
#define EV_INFO_DATA                    9
#define EV_INFO_IO                     10 
#define EV_INFO_LOG                    11
#define EV_INFO_LOCKS                  12 
#define EV_INFO_SESSIONS               13 
#define EV_INFO_VERSIONS               14 
#define EV_CHECK_FILES                 15 
#define EV_CHECK_FILE                  16 
#define EV_CHECK_SERVER                17
#define EV_CHECK_SRVSHOW               18
#define EV_CONFIG_PARAMS               19
#define EV_CONFIG_PARAM                20
#define EV_CONFIG_DEVSPACES            21
#define EV_CONFIG_DEVSPACE             22
#define EV_CONFIG_MEDIA                23
#define EV_CONFIG_MEDIUM               24
#define EV_BACKUP                      25
#define EV_DBM_WIZARD                  26
#define EV_WIZARD                      27
#define EV_CONFIG_SYSTABS              28
#define EV_CHECK_COMMAND               29
#define EV_RECOVER_DATABASE            30
#define EV_RECOVER_INDEXES             31
#define EV_RECOVER_DEVSPACES           32
#define EV_CHECK_KNLTRACE              33
#define EV_CHECK_DB                    34
#define EV_TUNING_STAT                 35
#define EV_TUNING_INDEXES              36
#define EV_CONFIG_LOGMODE              37
#define EV_CONFIG_USERS                38
#define EV_CONFIG_USER                 39
#define EV_LAST_ERROR                  40

#define TXT_EMPTY                       ""
#define TXT_DBM_LOGON                   "DBM_LOGON"   
#define TXT_DBM_LOGOFF                  "DBM_LOGOFF"
#define TXT_ENUMDATABASES               "ENUMDATABASES"
#define TXT_DBM_DUMMY                   "DBM_DUMMY"
#define TXT_DBM_INTERN_TEST             "DBM_INTERN_TEST"
#define TXT_DB_STATE                    "DB_STATE"
#define TXT_INFO_BAKHIS                 "INFO_BAKHIS"  
#define TXT_INFO_CACHES                 "INFO_CACHES"  
#define TXT_INFO_DATA                   "INFO_DATA"    
#define TXT_INFO_IO                     "INFO_IO"      
#define TXT_INFO_LOG                    "INFO_LOG"     
#define TXT_INFO_LOCKS                  "INFO_LOCKS"   
#define TXT_INFO_SESSIONS               "INFO_SESSIONS"
#define TXT_INFO_VERSIONS               "INFO_VERSIONS"
#define TXT_CHECK_FILES                 "CHECK_FILES"
#define TXT_CHECK_FILE                  "CHECK_FILE"
#define TXT_CHECK_SERVER                "CHECK_SERVER"
#define TXT_CHECK_SRVSHOW               "CHECK_SRVSHOW"
#define TXT_CONFIG_PARAMS               "CONFIG_PARAMS"
#define TXT_CONFIG_PARAM                "CONFIG_PARAM"
#define TXT_CONFIG_DEVSPACES            "CONFIG_DEVSPACES"
#define TXT_CONFIG_DEVSPACE             "CONFIG_DEVSPACE"
#define TXT_CONFIG_MEDIA                "CONFIG_MEDIA"
#define TXT_CONFIG_MEDIUM               "CONFIG_MEDIUM"
#define TXT_BACKUP                      "BACKUP"
#define TXT_DBM_WIZARD                  "DBM_WIZARD"
#define TXT_WIZARD                      "WIZARD"
#define TXT_CONFIG_SYSTABS              "CONFIG_SYSTABS"
#define TXT_CHECK_COMMAND               "CHECK_COMMAND"
#define TXT_RECOVER_DATABASE            "RECOVER_DB"
#define TXT_RECOVER_INDEXES             "RECOVER_INDEXES"
#define TXT_RECOVER_DEVSPACES           "RECOVER_DEVSPACES"
#define TXT_CHECK_KNLTRACE              "CHECK_KNLTRACE"
#define TXT_CHECK_DB                    "CHECK_DB"
#define TXT_TUNING_STAT                 "TUNING_STAT"
#define TXT_TUNING_INDEXES              "TUNING_INDEXES"
#define TXT_CONFIG_LOGMODE              "CONFIG_LOGMODE"
#define TXT_CONFIG_USERS                "CONFIG_USERS"
#define TXT_CONFIG_USER                 "CONFIG_USER"
#define TXT_LAST_ERROR                  "LAST_ERROR"


typedef struct DBMWebEvent {
        SAPDB_Int2     nEvent;
  const SAPDB_Char   * szEvent;
        bool           bNeedDatabase;
        bool           bSaveWorkURL;
} DBMWebEvent;

static DBMWebEvent theDBMWebEvents[] =
//  Event-Id               Event-Text              need-DB saveURL
  {{EV_DBM_LOGON         , TXT_DBM_LOGON         , false, false    },
   {EV_DBM_LOGOFF        , TXT_DBM_LOGOFF        , false, false    },
   {EV_ENUMDATABASES     , TXT_ENUMDATABASES     , false, false    },
   {EV_DBM_DUMMY         , TXT_DBM_DUMMY         , false, false    },
   {EV_DBM_INTERN_TEST   , TXT_DBM_INTERN_TEST   , false, false    },
   {EV_DB_STATE          , TXT_DB_STATE          , true , false    },
   {EV_INFO_BAKHIS       , TXT_INFO_BAKHIS       , true , true     },
   {EV_INFO_CACHES       , TXT_INFO_CACHES       , true , true     },
   {EV_INFO_DATA         , TXT_INFO_DATA         , true , true     },
   {EV_INFO_IO           , TXT_INFO_IO           , true , true     },
   {EV_INFO_LOG          , TXT_INFO_LOG          , true , true     },
   {EV_INFO_LOCKS        , TXT_INFO_LOCKS        , true , true     },
   {EV_INFO_SESSIONS     , TXT_INFO_SESSIONS     , true , true     },
   {EV_INFO_VERSIONS     , TXT_INFO_VERSIONS     , true , true     },
   {EV_CHECK_FILES       , TXT_CHECK_FILES       , true , true     },
   {EV_CHECK_FILE        , TXT_CHECK_FILE        , true , true     },
   {EV_CHECK_SERVER      , TXT_CHECK_SERVER      , true , true     },
   {EV_CHECK_SRVSHOW     , TXT_CHECK_SRVSHOW     , true , true     },
   {EV_CONFIG_PARAMS     , TXT_CONFIG_PARAMS     , true , true     },
   {EV_CONFIG_PARAM      , TXT_CONFIG_PARAM      , true , true     },
   {EV_CONFIG_DEVSPACES  , TXT_CONFIG_DEVSPACES  , true , true     },
   {EV_CONFIG_DEVSPACE   , TXT_CONFIG_DEVSPACE   , true , true     },
   {EV_CONFIG_MEDIA      , TXT_CONFIG_MEDIA      , true , true     },
   {EV_CONFIG_MEDIUM     , TXT_CONFIG_MEDIUM     , true , true     },
   {EV_BACKUP            , TXT_BACKUP            , true , true     },
   {EV_DBM_WIZARD        , TXT_DBM_WIZARD        , false, false    },
   {EV_WIZARD            , TXT_WIZARD            , false, false    },
   {EV_CONFIG_SYSTABS    , TXT_CONFIG_SYSTABS    , true,  true     },
   {EV_CHECK_COMMAND     , TXT_CHECK_COMMAND     , true,  true     },
   {EV_RECOVER_DATABASE  , TXT_RECOVER_DATABASE  , true,  true     },
   {EV_RECOVER_INDEXES   , TXT_RECOVER_INDEXES   , true,  true     },
   {EV_RECOVER_DEVSPACES , TXT_RECOVER_DEVSPACES , true,  true     },
   {EV_CHECK_KNLTRACE    , TXT_CHECK_KNLTRACE    , true,  true     },
   {EV_CHECK_DB          , TXT_CHECK_DB          , true,  true     },
   {EV_TUNING_STAT       , TXT_TUNING_STAT       , true,  true     },
   {EV_TUNING_INDEXES    , TXT_TUNING_INDEXES    , true,  true     },
   {EV_CONFIG_LOGMODE    , TXT_CONFIG_LOGMODE    , true,  true     },
   {EV_CONFIG_USERS      , TXT_CONFIG_USERS      , true,  true     },
   {EV_CONFIG_USER       , TXT_CONFIG_USER       , true,  true     },
   {EV_LAST_ERROR        , TXT_LAST_ERROR        , true,  false    },
   {EV_EMPTY             , TXT_EMPTY             , false, false    }};

// Pages
#define DBMWEB_PAGE_EMPTY        0
#define DBMWEB_PAGE_HEADER       1
#define DBMWEB_PAGE_MENU         2
#define DBMWEB_PAGE_WIZMENU      3
#define DBMWEB_PAGE_SIZE         4
#define DBMWEB_PAGE_VERSION      5

#define DBMWEB_PAGE_TXT_EMPTY    ""   
#define DBMWEB_PAGE_TXT_HEADER   "HEADER"   
#define DBMWEB_PAGE_TXT_MENU     "MENU"   
#define DBMWEB_PAGE_TXT_WIZMENU  "WIZMENU"   
#define DBMWEB_PAGE_TXT_SIZE     "SIZE"   
#define DBMWEB_PAGE_TXT_VERSION  "VERSION"   

typedef struct DBMWebPage {
        SAPDB_Int2     nPage;
  const SAPDB_Char   * szPage;
} DBMWebPage;

static DBMWebPage theDBMWebPages[] =
  {{DBMWEB_PAGE_HEADER   , DBMWEB_PAGE_TXT_HEADER   },
   {DBMWEB_PAGE_MENU     , DBMWEB_PAGE_TXT_MENU     },
   {DBMWEB_PAGE_WIZMENU  , DBMWEB_PAGE_TXT_WIZMENU  },
   {DBMWEB_PAGE_SIZE     , DBMWEB_PAGE_TXT_SIZE     },
   {DBMWEB_PAGE_VERSION  , DBMWEB_PAGE_TXT_VERSION  },
   {DBMWEB_PAGE_EMPTY    , DBMWEB_PAGE_TXT_EMPTY    }};

/*    
  =============================================================================
  class:        DBMWeb_DBMWeb
  =============================================================================
*/

/*
  -----------------------------------------------------------------------------
  public constructor DBMWeb_DBMWeb :: DBMWeb_DBMWeb
  -----------------------------------------------------------------------------
*/
DBMWeb_DBMWeb :: DBMWeb_DBMWeb () 
{
  m_Database       = NULL;
  m_Wizard         = NULL;
  m_sSessionID     = "0";
  m_sNullSessionID = "0";
} // end DBMWeb_DBMWeb :: DBMWeb_DBMWeb

/*
  -----------------------------------------------------------------------------
  public destructor DBMWeb_DBMWeb :: ~DBMWeb_DBMWeb
  -----------------------------------------------------------------------------
 */
DBMWeb_DBMWeb  :: ~DBMWeb_DBMWeb (  ) 
{
  if (m_Database != NULL) {
    delete m_Database;
    m_Database = NULL;
  } // end if
  if (m_Wizard != NULL) {
    delete m_Wizard;
    m_Wizard = NULL;
  } // end if

} // end DBMWeb_DBMWeb :: ~DBMWeb_DBMWeb

/*
  -----------------------------------------------------------------------------
  public function DBMWeb_DBMWeb :: doService
  -----------------------------------------------------------------------------
 */
bool DBMWeb_DBMWeb :: doService ( sapdbwa_WebAgent      &wa,
                                  sapdbwa_HttpRequest   &request,
                                  sapdbwa_HttpReply     &reply)
{
  SAPDB_Bool    bReturn = SAPDB_TRUE;
  DBMCli_String sValue;

  if (GetParameterValue(PARAM_PAGE, request, sValue)) {

    int nIndex = 0;

    for (nIndex = 0; 
         (theDBMWebPages[nIndex].nPage                   != DBMWEB_PAGE_EMPTY) && 
         (strcmp(theDBMWebPages[nIndex].szPage, sValue)  != 0       ); 
         ++nIndex);

    switch (theDBMWebPages[nIndex].nPage) {
      case DBMWEB_PAGE_HEADER:
        bReturn = pageHeader(wa, request, reply);
        break;
      case DBMWEB_PAGE_MENU:
        bReturn = pageMenu(wa, request, reply);
        break;
      case DBMWEB_PAGE_WIZMENU:
        bReturn = pageWizMenu(wa, request, reply);
        break;
      case DBMWEB_PAGE_SIZE:
        bReturn = pageSize(wa, request, reply);
        break;
      case DBMWEB_PAGE_VERSION:
        bReturn = pageVersion(wa, request, reply);
        break;
      default: {
          DBMWeb_TemplateMsgBox oMsgBox(wa,DBMWEB_TEMPLMSGBOX_ERROR,"","Unknown service request!");
          oMsgBox.writePage(Tools_TemplateWriterWA(reply));
        }  // end block
        bReturn = SAPDB_TRUE;
        break;
    } // end switch

  } else if (GetParameterValue(PARAM_EVENT, request, sValue)) {

    int nIndex = 0;

    for (nIndex = 0; 
         (theDBMWebEvents[nIndex].nEvent                   != EV_EMPTY) && 
         (strcmp(theDBMWebEvents[nIndex].szEvent, sValue)  != 0       ); 
         ++nIndex);

    if (theDBMWebEvents[nIndex].bNeedDatabase && m_Database == NULL) {
      DBMWeb_TemplateMsgBox oMsgBox(wa,DBMWEB_TEMPLMSGBOX_ERROR,"","Database object unexpectly destroyed!");
      oMsgBox.SetButtonText("Logoff")
             .SetButtonAction("javascript:parent.GotoWebDBMURL(this, \\'Event=DBM_LOGOFF\\')")
             .SetButtonTarget("Main")
             .writePage(Tools_TemplateWriterWA(reply));
      bReturn = SAPDB_TRUE;
    } else {
      if (theDBMWebEvents[nIndex].bSaveWorkURL) {
        // save the last URL
        m_sLastWorkURL = request.GetRequestURI( );
        if (request.GetQueryString( ) != NULL) {
          m_sLastWorkURL = m_sLastWorkURL + "?" + request.GetQueryString( );
        } // end if
      } // end if

      if (theDBMWebEvents[nIndex].nEvent != EV_LAST_ERROR) {
        m_oGlbMsgList.ClearMessageList();
      } // end if


      switch (theDBMWebEvents[nIndex].nEvent) {
        // ------------------------------------------------------------------------
        // CORE functions of the DBMWeb class
        // ------------------------------------------------------------------------
        case EV_DBM_LOGON:
          bReturn = dbmLogon(wa, request, reply);
          break;
        case EV_DBM_LOGOFF:  
          bReturn = dbmLogoff(wa, request, reply);
          break;
        case EV_ENUMDATABASES:  
          bReturn = enumDatabases(wa, request, reply);
          break;
        case EV_DBM_DUMMY:  
          bReturn = sendNotImpl(wa, request, reply);
          break;
        case EV_DBM_WIZARD:  
          bReturn = dbmWizard(wa, request, reply);
          break;
        case EV_LAST_ERROR:  
          bReturn = sendMsgListError( wa, request, reply, m_oGlbMsgList, m_Database->DatabaseName(), "/WARoot/HTML/DBMEmpty.htm");
          break;
        // ------------------------------------------------------------------------
        // STATE function of the DBMWeb class
        // ------------------------------------------------------------------------
        case EV_DB_STATE:  
          bReturn = dbState(wa, request, reply);
          break;  
        // ------------------------------------------------------------------------
        // INFO function of the DBMWeb class
        // ------------------------------------------------------------------------
        case EV_INFO_BAKHIS:  
          bReturn = infoBackupHistory(wa, request, reply);
          break;  
        case EV_INFO_CACHES:  
          bReturn = infoCaches(wa, request, reply);
          break;  
        case EV_INFO_DATA:  
          bReturn = infoData(wa, request, reply);
          break;    
        case EV_INFO_IO:  
          bReturn = infoIO(wa, request, reply);
          break;      
        case EV_INFO_LOG:  
          bReturn = infoLog(wa, request, reply);
          break;     
        case EV_INFO_LOCKS:  
          bReturn = infoLocks(wa, request, reply);
          break;         
        case EV_INFO_SESSIONS:  
          bReturn = infoSessions(wa, request, reply);
          break;      
        case EV_INFO_VERSIONS: 
          bReturn = infoVersions(wa, request, reply);
          break;      
        // ------------------------------------------------------------------------
        // TUNING function of the DBMWeb class
        // ------------------------------------------------------------------------
        case EV_TUNING_STAT:
          bReturn = tuningStat(wa, request, reply);
          break;      
        case EV_TUNING_INDEXES:
          bReturn = tuningIndexes(wa, request, reply);
          break;      
        // ------------------------------------------------------------------------
        // CHECK function of the DBMWeb class
        // ------------------------------------------------------------------------
        case EV_CHECK_DB:
          bReturn = checkDB(wa, request, reply);
          break;      
        case EV_CHECK_FILES:
          bReturn = checkFiles(wa, request, reply);
          break;      
        case EV_CHECK_FILE:
          bReturn = checkFile(wa, request, reply);
          break;      
        case EV_CHECK_SERVER:
          bReturn = checkServer(wa, request, reply);
          break;      
        case EV_CHECK_SRVSHOW:
          bReturn = checkSrvShow(wa, request, reply);
          break;      
        case EV_CHECK_COMMAND:
          bReturn = checkCommand(wa, request, reply);
          break;      
        case EV_CHECK_KNLTRACE:
          bReturn = checkKernelTrace(wa, request, reply);
          break;      
        // ------------------------------------------------------------------------
        // CONFIGURATION function of the DBMWeb class
        // ------------------------------------------------------------------------
        case EV_CONFIG_PARAMS:
          bReturn = configParameters(wa, request, reply);
          break;      
        case EV_CONFIG_PARAM:
          bReturn = configParameter(wa, request, reply);
          break;      

        case EV_CONFIG_DEVSPACES:
          bReturn = configDevspaces(wa, request, reply);
          break;      
        case EV_CONFIG_DEVSPACE:
          bReturn = configDevspace(wa, request, reply);
          break;      

        case EV_CONFIG_MEDIA:
          bReturn = configMedia(wa, request, reply);
          break;      
        case EV_CONFIG_MEDIUM:
          bReturn = configMedium(wa, request, reply);
          break;      

        case EV_CONFIG_SYSTABS:
          bReturn = configSysTabs(wa, request, reply);
          break;      

        case EV_CONFIG_LOGMODE:
          bReturn = configLogMode(wa, request, reply);
          break;      

        case EV_CONFIG_USERS:
          bReturn = configUsers(wa, request, reply);
          break;      

        case EV_CONFIG_USER:
          bReturn = configUser(wa, request, reply);
          break;      

          // ------------------------------------------------------------------------
        // BACKUP function of the DBMWeb class
        // ------------------------------------------------------------------------
        case EV_BACKUP:
          bReturn = backupDB(wa, request, reply);
          break;      

        case EV_RECOVER_DATABASE:
          bReturn = recoverDB(wa, request, reply);
          break;      

        // ------------------------------------------------------------------------
        // RECOVER function of the DBMWeb class
        // ------------------------------------------------------------------------

        case EV_RECOVER_INDEXES:
          bReturn = recoverIndexes(wa, request, reply);
          break;      

        case EV_RECOVER_DEVSPACES:
          bReturn = recoverDevspaces(wa, request, reply);
          break;      

        // ------------------------------------------------------------------------
        // TEST function of the DBMWeb class
        // ------------------------------------------------------------------------
        case EV_DBM_INTERN_TEST:  
          bReturn = internTest(wa, request, reply);
          break;

        // ------------------------------------------------------------------------
        // WIZARD function of the DBMWeb class
        // ------------------------------------------------------------------------
        case EV_WIZARD:
          bReturn = wizardDB(wa, request, reply);
          break;      

        // ------------------------------------------------------------------------
        default: {
            DBMWeb_TemplateMsgBox oMsgBox(wa,DBMWEB_TEMPLMSGBOX_ERROR,"","Unknown service request!");
            oMsgBox.writePage(Tools_TemplateWriterWA(reply));
          }  // end block
          bReturn = SAPDB_TRUE;
          break;
      } // end switch

    } // end if

  } else {
    bReturn = sendFrame(wa, request, reply);
  } // end if

  return ( bReturn == SAPDB_TRUE ); // wegen compiler warning C4800
} // end DBMWeb_DBMWeb :: doService

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: GetParameterValue
  -----------------------------------------------------------------------------
 */
bool DBMWeb_DBMWeb :: GetParameterValue(const char * pName,
                                        sapdbwa_HttpRequest & request,
                                        DBMCli_String & sValue)
{
  bool              bReturn = false;
  sapdbwa_StringSeq values; 

//  sValue="";

  if (request.GetParameterValues(pName, values) == sapdbwa_True) {
    if (values.GetNumElem() > 0) {
      if (strlen(values[0]) < 512) {
        sValue = values[0];
        bReturn = true;
      } // end if
    } // end if
  } // end if
  return bReturn;
} // DBMWeb_DBMWeb :: GetParameterValue

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: GetParameterValueByIndex
  -----------------------------------------------------------------------------
 */
bool DBMWeb_DBMWeb :: GetParameterValueByIndex(const char * pName,
                                               int nIndex,
                                               sapdbwa_HttpRequest & request,
                                               DBMCli_String & sValue)
{
  bool              bReturn = false;
  sapdbwa_StringSeq values; 

  if (request.GetParameterValues(pName, values) == sapdbwa_True) {
    if ((values.GetNumElem() > 0)      && 
        (nIndex < values.GetNumElem())    ) {
      sValue = values[nIndex];
      bReturn = true;
    } 
  }
  return bReturn;
} // end DBMWeb_DBMWeb :: GetParameterValueByIndex

/*
  -----------------------------------------------------------------------------
  private function DBMWeb_DBMWeb :: MovetoURL
  -----------------------------------------------------------------------------
 */
void DBMWeb_DBMWeb :: MovetoURL(const DBMCli_String & sURL, 
                                sapdbwa_HttpReply & reply )
{
  DBMCli_String sTemp = sURL;
  sTemp.Trim('"');
  reply.InitHeader(sapdbwa_Status_Moveto, "text/html", NULL, NULL, NULL, NULL);
  reply.SetHeader("Location", sTemp);
  reply.SendHeader();
} // end DBMWeb_DBMWeb :: MovetoURL

