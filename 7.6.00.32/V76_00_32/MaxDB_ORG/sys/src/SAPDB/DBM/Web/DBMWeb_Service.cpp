/*! 
  -----------------------------------------------------------------------------
  module: DBMWeb_Service.cpp
  -----------------------------------------------------------------------------
  responsible: MartinR
  special area: 
  description:  Database Manager WebAgent Extension
  see also:
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
  INCLUDES
  -----------------------------------------------------------------------------
*/
#include <time.h>

#include "gsp00.h"
#include "geo47.h"

#include "DBM/Web/DBMWeb_DBMWeb.hpp"
#include "DBM/Web/DBMWeb_TemplateMsgBox.hpp"
#include "ToolsCommon/Tools_TemplateMsgBox.hpp"
#include "ToolsCommon/Tools_Session.hpp"

typedef Tools_Session<DBMWeb_DBMWeb> DBMWeb_Session;

/*
  -----------------------------------------------------------------------------
  callback function SessionDataDestructor
  -----------------------------------------------------------------------------
*/
sapdbwa_Bool SessionDataDestructor (sapdbwa_WebAgent &wa, void * pData) 
{
//  wa.WriteLogMsg(_T("DBMWeb_Service::SessionDataDestructor\n"));

  DBMWeb_Session * aObject = (DBMWeb_Session *) pData;

  if (aObject != NULL) {
    delete aObject;
  } // end if

  return true;
} // end SessionDataDestructor

/*
  -----------------------------------------------------------------------------
  callback function DBMWeb_Service_Init
  -----------------------------------------------------------------------------
 */
externC sapdbwa_Int2 DBMWeb_Service_Init(sapdbwa_WebAgent &wa)
{
  wa.WriteLogMsg(_T("DBMWeb_Service::DBMWeb_Service_Init\n"));

  return ((wa.SetUseWebSessions(sapdbwa_True) == sapdbwa_True) ? sapdbwa_RTC_Ok : sapdbwa_RTC_Error);

} // end DBMWeb_Service_Init

/*
  -----------------------------------------------------------------------------
  callback function DBMWeb_Service_Exit
  -----------------------------------------------------------------------------
 */
externC sapdbwa_Int2 DBMWeb_Service_Exit(sapdbwa_WebAgent &wa)
{
  wa.WriteLogMsg(_T("DBMWeb_Service::DBMWeb_Service_Exit\n"));

  return sapdbwa_RTC_Ok;
} // DBMWeb_Service_Exit
                         
/*
  -----------------------------------------------------------------------------
  callback function DBMWeb_Service_Service
  -----------------------------------------------------------------------------
*/
externC sapdbwa_Int2 DBMWeb_Service_Service(sapdbwa_WebAgent      &wa,
                                            sapdbwa_HttpRequest   &request,
                                            sapdbwa_HttpReply     &reply)
{
  sapdbwa_WebSession       webSession;
  DBMWeb_Session         * aContainer = NULL;
  bool                     bSessionOK = false;
  SAPDB_UInt4              nSessionID = 0;

  // get session id
  const char * pBegin = strstr(request.GetRequestURI (), "webdbm");
  if (pBegin != NULL) {
    pBegin = pBegin + 6;
    if (*pBegin == '/') {
      ++pBegin;
      nSessionID = DBMWeb_Session::AsciiToSessionID (pBegin);
    } // end if
  } // end if

  // look for existing Session
  if (wa.GetWebSession( webSession ) == sapdbwa_True) {
    aContainer = (DBMWeb_Session *) webSession.GetSessionData();
    bSessionOK = true;
  } else {
    if ((wa.GetErr().GetErrId() == sapdbwa_Error_WebSessionTimeout) && (nSessionID != 0)) {
      DBMWeb_TemplateMsgBox oMsgBox(wa,DBMWEB_TEMPLMSGBOX_ERROR,"","Sorry, your Database Manager session timed out!");
      oMsgBox.SetButtonText("OK")
             .SetButtonAction("/webdbm")
             .SetButtonTarget("_parent")
             .writePage(Tools_TemplateWriterWA(reply));
    } else if (wa.StartDefaultWebSession( webSession ) != sapdbwa_True) {
      DBMWeb_TemplateMsgBox oMsgBox(wa,DBMWEB_TEMPLMSGBOX_ERROR,"","Can not create Web DBM session!");
      oMsgBox.SetButtonText("OK")
             .SetButtonAction("/webdbm")
             .SetButtonTarget("_parent")
             .writePage(Tools_TemplateWriterWA(reply));
    } else {
      bSessionOK = true;
    } // end if
  } // end if

  if (bSessionOK) {
    char szTimeout[20];
    wa.GetParam("webSessionTimeout", szTimeout, 20);

    aContainer = (aContainer == NULL) ? new DBMWeb_Session(atoi(szTimeout)) : aContainer;

    if (aContainer == NULL) {
      DBMWeb_TemplateMsgBox oMsgBox(wa,DBMWEB_TEMPLMSGBOX_ERROR,"","Can not create Web DBM object!");
      oMsgBox.SetButtonText("OK")
             .SetButtonAction("/webdbm")
             .SetButtonTarget("_parent")
             .writePage(Tools_TemplateWriterWA(reply));
    } else {
      if (webSession.SetSessionData(aContainer, SessionDataDestructor) == sapdbwa_True) {
        DBMWeb_DBMWeb * aDBMWeb    = NULL;

        // retrieve object
        if (nSessionID != 0) {
          aDBMWeb = aContainer->get(nSessionID);
        } else {
          aDBMWeb = aContainer->add(nSessionID);
          aDBMWeb->setSessionID(DBMWeb_Session::SessionIDToAscii(nSessionID).CharPtr());
        } // end if

        if (aDBMWeb == NULL && nSessionID == 0) {
          DBMWeb_TemplateMsgBox oMsgBox(wa,DBMWEB_TEMPLMSGBOX_ERROR,"","Can not create Web DBM object!");
          oMsgBox.SetButtonText("OK")
                 .SetButtonAction("/webdbm")
                 .SetButtonTarget("_parent")
                 .writePage(Tools_TemplateWriterWA(reply));
        } else if (aDBMWeb == NULL && nSessionID != 0) {
          DBMWeb_TemplateMsgBox oMsgBox(wa,DBMWEB_TEMPLMSGBOX_ERROR,"","Invalid Session ID!");
          oMsgBox.SetButtonText("OK")
                 .SetButtonAction("/webdbm")
                 .SetButtonTarget("_parent")
                 .writePage(Tools_TemplateWriterWA(reply));
        } else {
          // Handle request
          aDBMWeb->doService(wa, request, reply);
          if (!aDBMWeb->isValid() && (nSessionID != 0)) {
            aContainer->remove(nSessionID);
          } // end if
        } // end if

      } else {
        DBMWeb_TemplateMsgBox oMsgBox(wa,DBMWEB_TEMPLMSGBOX_ERROR,"","Can not set Web DBM session data!");
        oMsgBox.SetButtonText("OK")
               .SetButtonAction("/webdbm")
               .SetButtonTarget("_parent")
               .writePage(Tools_TemplateWriterWA(reply));
      } // end if
    } // end if

  } // end if

  return sapdbwa_RTC_Accepted;
} // DBMWeb_Service_Service


