/*
    Copyright (c) 2000-2005 SAP AG

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA



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

/*!**********************************************************************

  module: StudioWeb_WebRequest.cpp

  -----------------------------------------------------------------------

  mail to:  sapdb-general@sap.com

  special area: WebSQL

  description:  functions for web query service

  see also:		for sapdbwa_WebAgent in hwd02waapi.h

  -----------------------------------------------------------------------

  copyright:                Copyright (c) 2000-2005 SAP AG

************************************************************************/


/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

//html code fragments
#include "SAPDB/SQLStudio/WebQuery/StudioWeb_HtmlFragments.h"

//error messages
#include "SAPDB/SQLStudio/WebQuery/StudioWeb_Error.h"
#include "SAPDB/SQLStudio/WebQuery/StudioWeb_WebRequest.hpp"
#include "SAPDB/SQLStudio/WebQuery/StudioWeb_WebQuery.hpp"

#include "ToolsCommon/Tools_TemplateMsgBox.hpp"
#include "ToolsCommon/Tools_DynamicUTF8String.hpp"


#include "ToolsCommon/Tools_Session.hpp"

typedef Tools_Session<StudioWeb_WebQuery> WebSQL_Session;


//global variables
static sapdbwa_DBCPoolP m_pDBCPool = NULL;

Tools_DynamicUTF8String m_sODBCDriver; 


#define WEBODBC		"SAP DB (Unicode)"

/*
  -----------------------------------------------------------------------------
  callback function SessionDataDestructor
  -----------------------------------------------------------------------------
*/
sapdbwa_Bool SessionDataDestructor (sapdbwa_WebAgent &wa, void * pData) 
{
  WebSQL_Session * aObject = (WebSQL_Session *) pData;
  if (aObject != NULL) {
    delete aObject;
  } // end if

  return true;
} // end SessionDataDestructor

//called on closeWebSession
sapdbwa_Bool destroySessionData(sapdbwa_WebAgent &wa,void *pData)
{	

	WebSQL_Session *aContainer = (WebSQL_Session*) pData;
	if (aContainer != NULL){

		delete aContainer;  
		aContainer = NULL;
	}

	return sapdbwa_True;
}



sapdbwa_Int2 WebSQLInit(sapdbwa_WebAgent &wa)
{

	 wa.WriteLogMsg("StudioWeb-WebSQLInit\n");

	 char sValue[32];
	 memset(sValue,0,32);	 

	 int nTimeOut = sapdbwa_DBC_NO_TIMEOUT;
	 //get user defined value for db timeout
	 if (wa.GetParam(WQ_DBTIMEOUT_TAG,sValue,32) == 1) nTimeOut = atoi(sValue);	 

#if defined(WIN32)
	 m_sODBCDriver = WEBODBC;
#endif	 

	 m_pDBCPool = sapdbwa_CreateDBCPool(nTimeOut);    
     if(m_pDBCPool == NULL)  wa.WriteLogMsg(WQ_ERR018);
     
	 wa.SetUseWebSessions( sapdbwa_True );
	 	
    return 1;
}; 

sapdbwa_Int2 WebSQLExit(sapdbwa_WebAgent &wa)
{
	wa.WriteLogMsg("StudioWeb-WebSQLExit\n");

	if(m_pDBCPool != NULL){

		sapdbwa_DestroyDBCPool( m_pDBCPool );	
		m_pDBCPool = NULL;
	}		
    return 1;
}; 




/*!-------------------------------------------------------------------------
    function:     WebSQLService
    description:  called by webagent 
    arguments:	  [in] wa, webagent object	
				  [in] req, webagent http request object
				  [in] rep, webagent http reply object
				  
    return value: SAPDB_TRUE if success
    --------------------------------------------------------------------------*/
sapdbwa_Int2 WebSQLService(sapdbwa_WebAgent &wa,sapdbwa_HttpRequest &req,sapdbwa_HttpReply &rep)
{

  sapdbwa_WebSession     webSession;
  WebSQL_Session         * aContainer = NULL;
  bool                   bSessionOK = false;
  SAPDB_Bool			 bCloseSession = SAPDB_FALSE;

  // look for existing Session
  if (wa.GetWebSession( webSession ) == sapdbwa_True) {
    aContainer = (WebSQL_Session *) webSession.GetSessionData();
    bSessionOK = true;
  }
  else {
    if (wa.GetErr().GetErrId() == sapdbwa_Error_WebSessionTimeout) {

      Tools_TemplateMsgBox oMsgBox(wa);		

      oMsgBox.setButtonText(_Tools_UTF8Ptr("OK"))
			 .setMessage(WQ_ERR028)
             .setButtonAction(_Tools_UTF8Ptr(WQ_SERVICEDLL))
             .setButtonTarget(_Tools_UTF8Ptr(WQ_TARGETTOP))
             .writePage(Tools_TemplateWriterWA(rep));
    } 
	else if (wa.StartDefaultWebSession( webSession ) != sapdbwa_True) {

      Tools_TemplateMsgBox oMsgBox(wa);
	  oMsgBox.setButtonText(_Tools_UTF8Ptr("OK"))
			 .setMessage(WQ_ERR016)
             .setButtonAction(_Tools_UTF8Ptr(WQ_SERVICEDLL))
             .setButtonTarget(_Tools_UTF8Ptr(WQ_TARGETTOP))
             .writePage(Tools_TemplateWriterWA(rep));
    } 
	else {
      bSessionOK = true;
    } // end if
  } // end if

  if (bSessionOK) {
    
    aContainer = (aContainer == NULL) ? new WebSQL_Session() : aContainer;

    if (aContainer == NULL) {
      Tools_TemplateMsgBox oMsgBox(wa);
      oMsgBox.setButtonText(_Tools_UTF8Ptr("OK"))
			 .setMessage(WQ_ERR017)
             .setButtonAction(_Tools_UTF8Ptr(WQ_SERVICEDLL))
             .setButtonTarget(_Tools_UTF8Ptr(WQ_TARGETTOP))
             .writePage(Tools_TemplateWriterWA(rep));
    }
	else {
      if (webSession.SetSessionData(aContainer, SessionDataDestructor) == sapdbwa_True) {
        StudioWeb_WebQuery * poWebQuery    = NULL;
        SAPDB_UInt4     nSessionID = 0;

        // get session id
        const char * pBegin = strstr(req.GetRequestURI (), WQ_SERVICEDLL);
        if (pBegin != NULL) {
          pBegin = pBegin + 6;
          if (*pBegin == '/') {
            ++pBegin;
            nSessionID = WebSQL_Session::AsciiToSessionID (pBegin);
          } // end if
        } // end if

        // retrieve object
        if (nSessionID != 0 && aContainer->sessionidvalid(nSessionID)) {			
          poWebQuery = aContainer->get(nSessionID);
        }
		else {
          poWebQuery = aContainer->add(nSessionID);

          poWebQuery->setSessionID(WebSQL_Session::SessionIDToAscii(nSessionID).CharPtr());
		  poWebQuery->setODBCDriver(m_sODBCDriver);
		  poWebQuery->setDBCPool(m_pDBCPool);
        } // end if

        if (poWebQuery == NULL && nSessionID == 0) {
          Tools_TemplateMsgBox oMsgBox(wa);
          oMsgBox.setButtonText(_Tools_UTF8Ptr("OK"))
				 .setMessage(WQ_ERR017)
                 .setButtonAction(_Tools_UTF8Ptr(WQ_SERVICEDLL))
                 .setButtonTarget(_Tools_UTF8Ptr(WQ_TARGETTOP))
                 .writePage(Tools_TemplateWriterWA(rep));
        }
		else if (poWebQuery == NULL && nSessionID != 0) {
			
		 
          Tools_TemplateMsgBox oMsgBox(wa);
          oMsgBox.setButtonText(_Tools_UTF8Ptr("OK"))
				 .setMessage(WQ_ERR019)
                 .setButtonAction(_Tools_UTF8Ptr(WQ_SERVICEDLL))
                 .setButtonTarget(_Tools_UTF8Ptr(WQ_TARGETTOP))
                 .writePage(Tools_TemplateWriterWA(rep));
        }
		else {
          // Handle request
          poWebQuery->doService(wa, req, rep,bCloseSession);          
		  
          if ((!poWebQuery->userLoggedOn() || bCloseSession) && nSessionID != 0) {

			  aContainer->remove(nSessionID);
          } // end if
        } // end if

      }
	  else {
        Tools_TemplateMsgBox oMsgBox(wa);
        oMsgBox.setButtonText(_Tools_UTF8Ptr("OK"))
			   .setMessage(WQ_ERR037)
               .setButtonAction(_Tools_UTF8Ptr(WQ_SERVICEDLL))
               .setButtonTarget(_Tools_UTF8Ptr(WQ_TARGETTOP))
               .writePage(Tools_TemplateWriterWA(rep));
      } // end if
    } // end if

  } // end if

  return sapdbwa_RTC_Accepted;

};











