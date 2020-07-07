/*

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

  module: StudioWeb_Functions.hpp

  -----------------------------------------------------------------------

  responsible: WolfgangA

  description:  functions for WebQuery

  see also:

  -----------------------------------------------------------------------
 
  copyright:                Copyright (c) 2000-2005 SAP AG

************************************************************************/

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#ifndef STUDIOWEB_FUNCTIONS_HPP
#define STUDIOWEB_FUNCTIONS_HPP

#include "SAPDB/SQLStudio/OAL/StudioOAL_WResult.hpp"				  
#include "SAPDB/SQLStudio/WebQuery/StudioWeb_Definitions.hpp"
#include "SAPDB/SQLStudio/Studio_StoredFileContent.hpp"

#include "hwd08dbc.h"
#include "hwd02waapi.h" 


/*!-------------------------------------------------------------------------
	function:     replaceNoCharWithDot
	description:  replace all chars y ascii 31 with a '.'
                  [in/out] sInAndOut, input/output string				  
				  
				  
	return value: /
--------------------------------------------------------------------------*/
void replaceNoCharWithDot(Tools_DynamicUTF8String &sInAndOut);

/*!-------------------------------------------------------------------------
  function:     initSOFromRequest
  description:  init a stored statement from a request

  arguments:	[in] poSFC, stored file object to be initialized
				[in] req  ,web request object
				
				

  return value: true
-------------------------------------------------------------------------*/    
SAPDB_Bool initSOFromRequest(Studio_StoredFileContent	*poSFC,sapdbwa_HttpRequest  &req);



/*!-------------------------------------------------------------------------
    function:     sendDBCError
    description:  send an error message to sapdbwa rep object
    arguments:    [in] wa, webagent object	
				  [in] rep, webagent http reply object
				  [in] dbc,	webagent dbc handle
    return value: none
    --------------------------------------------------------------------------*/
void sendDBCError(sapdbwa_WebAgent &wa,
				  sapdbwa_HttpReply &rep,
				  sapdbwa_DBCP dbc);

/*!-------------------------------------------------------------------------
	  function:      checkForSystemTable
	  description:   check if a systemtable is installed
	  arguments:	 [in] *dbc, pointer to database connection handle
					 [in] sOwner, owner of systemtable
					 [in] sName, name  of systemtable

	  return value:  true if success
	  --------------------------------------------------------------------------*/
SAPDB_Bool checkForSystemTable(sapdbwa_DBCP pDbc,
							   const Tools_DynamicUTF8String& sOwner,
							   const Tools_DynamicUTF8String& sName);

/*!-------------------------------------------------------------------------
	  function:      checkForSystemTable2
	  description:   check if a systemtable is installed
	  arguments:	 [in] *dbc, pointer to database connection handle
					 [in] sOwner, owner of systemtable
					 [in] sName, name  of systemtable

	  return value:  true if success
	  --------------------------------------------------------------------------*/
SAPDB_Bool checkForSystemTable2(sapdbwa_DBCP pDbc,
							   const Tools_DynamicUTF8String& sOwner,
							   const Tools_DynamicUTF8String& sName);

/*!-------------------------------------------------------------------------
	  function:      checkUserNotExlusive
	  description:   check if a user is not exclusive
	  arguments:	 [in] *dbc, pointer to database connection handle
					 [in] sUser, name of user
					 

	  return value:  true if success
	  --------------------------------------------------------------------------*/
SAPDB_Bool checkUserNotExlusive(sapdbwa_DBCP pDbc,
							   const Tools_DynamicUTF8String& sUser);


/*!-------------------------------------------------------------------------
	  function:     sendUtf8ErrorMessageBox
	  description:  build and sends the frame page for webquery
	  arguments:	[in] wa, webagent object	
					[in] rep, webagent http reply object
					[in] sMessage, pointer to error message text
				  
	  return value: none
 --------------------------------------------------------------------------*/
 void sendUtf8ErrorMessageBox(sapdbwa_WebAgent &wa,
						      sapdbwa_HttpReply &rep,
						      Tools_DynamicUTF8String &sMessage);


 /*!-------------------------------------------------------------------------
	  function:     sendErrorMessageBox
	  description:  build and sends the frame page for webquery
	  arguments:	[in] wa, webagent object	
					[in] rep, webagent http reply object
					[in] pMessage, pointer to error message text
				  
	  return value: none
 --------------------------------------------------------------------------*/
 void sendErrorMessageBox(sapdbwa_WebAgent &wa,
						  sapdbwa_HttpReply &rep,
						  const char *pMessage);

/*!-------------------------------------------------------------------------
	function:     sendPageHeader
	description:  send a HTML page header
	arguments:    [in] rep, webagent http reply object
			  
	return value: /
--------------------------------------------------------------------------*/
void sendPageHeader(sapdbwa_HttpReply &rep);

/*!-------------------------------------------------------------------------
	function:     sendPageBegin
	description:  send a HTML page begin sequence
	arguments:    [in] rep, webagent http reply object
				  
	return value: /
--------------------------------------------------------------------------*/
void sendPageBegin(sapdbwa_HttpReply &rep,const char *pTitle);

/*!-------------------------------------------------------------------------
	function:     sendPageEnd
	description:  send a HTML page end sequence
	arguments:    [in] rep, webagent http reply object
			  
	return value: /
--------------------------------------------------------------------------*/
void sendPageEnd(sapdbwa_HttpReply &rep);


 /*!-------------------------------------------------------------------------
	  function:     sendHTMLFile
      description:  send a html file
      arguments:    [in] wa, webagent object	
					[in] rep, webagent http reply object
					[in] sFile,complete file and path
				  
      return value: /
      --------------------------------------------------------------------------*/
void sendHTMLFile(sapdbwa_WebAgent &wa,
				  sapdbwa_HttpReply &rep,
				  const Tools_DynamicUTF8String& sFile);	  


/*!-------------------------------------------------------------------------
	function:      quoteString
	description:   put &quot; before "
	arguments:     [in] sData, string to quote
				   
	return value:  /

--------------------------------------------------------------------------*/
void quoteString(Tools_DynamicUTF8String &sData);


/*!-------------------------------------------------------------------------
	function:      escapeString
	description:   put %20; instead of space
	arguments:     [in] sData, string to escape
				   
	return value:  /

--------------------------------------------------------------------------*/
void escapeString(Tools_DynamicUTF8String &sData);

/*!-------------------------------------------------------------------------
	function:      escapeAsUTF8String
	description:   escape each char in string as unicode char e.g. \u00FF
	arguments:     [in] sData, string to escape
				   
	return value:  /

--------------------------------------------------------------------------*/
SAPDB_Bool escapeAsUTF8String(Tools_DynamicUTF8String &sData);


/*!-------------------------------------------------------------------------
	  function:     isParamQueryRequest
	  description:  is request for a paramterized query
	  arguments:    [in] req, webagent http request object
					
				  
	  return value: true/false
--------------------------------------------------------------------------*/
SAPDB_Bool isParamQueryRequest(sapdbwa_HttpRequest  &req);

/*!-------------------------------------------------------------------------
	  function:     isStoredQueryRequest
	  description:  is request for a stored (from SQL Studio) query
	  arguments:    [in] req, webagent http request object
					
				  
	  return value: true/false
--------------------------------------------------------------------------*/
SAPDB_Bool isStoredSQLStudioQueryRequest(sapdbwa_HttpRequest  &req);


/*!-------------------------------------------------------------------------
	  function:     isCreateFolderRequest
	  description:  is request for creating a new folder in stored objects tree
	  arguments:    [in] req, webagent http request object
					
				  
	  return value: true/false
--------------------------------------------------------------------------*/
SAPDB_Bool isCreateFolderRequest (sapdbwa_HttpRequest  &req);

/*!-------------------------------------------------------------------------
	  function:     isTreeButtonRequest
	  description:  is request for creating a page with buttons for creating folders, ....
	  arguments:    [in] req, webagent http request object
					
				  
	  return value: true/false
--------------------------------------------------------------------------*/
SAPDB_Bool isTreeButtonRequest (sapdbwa_HttpRequest  &req);

/*!-------------------------------------------------------------------------
	  function:     isDSqlRequest
	  description:  is request for a dsql window
	  arguments:    [in] req, webagent http request object
					
				  
	  return value: true/false
--------------------------------------------------------------------------*/
SAPDB_Bool isDSqlRequest (sapdbwa_HttpRequest  &req);

/*!-------------------------------------------------------------------------
	  function:     isHeaderPageRequest
	  description:  is request for the header page
	  arguments:    [in] req, webagent http request object
					
				  
	  return value: true/false
--------------------------------------------------------------------------*/
SAPDB_Bool isHeaderPageRequest (sapdbwa_HttpRequest  &req,SAPDB_Bool &bLogOffButton);

/*!-------------------------------------------------------------------------
	  function:     isLogonMainRequest
	  description:  is request for the logon window
	  arguments:    [in] req, webagent http request object
					
				  
	  return value: true/false
--------------------------------------------------------------------------*/
SAPDB_Bool isLogonMainRequest (sapdbwa_HttpRequest  &req);


/*!-------------------------------------------------------------------------
	  function:     isFrameRequest
	  description:  is request for the inner frame window
	  arguments:    [in] req, webagent http request object
					
				  
	  return value: true/false
--------------------------------------------------------------------------*/
SAPDB_Bool isFrameRequest (sapdbwa_HttpRequest  &req);

/*!-------------------------------------------------------------------------
	  function:     isTreePageRequest
	  description:  is request for tree window
	  arguments:    [in] req, webagent http request object
					
				  
	  return value: true/false
--------------------------------------------------------------------------*/
SAPDB_Bool isTreePageRequest (sapdbwa_HttpRequest  &req);


/*!-------------------------------------------------------------------------
	  function:     isQueryStringValid
	  description:  is request string valid
	  arguments:    [in] req, webagent http request object
					
				  
	  return value: true/false
--------------------------------------------------------------------------*/
SAPDB_Bool isQueryStringValid (sapdbwa_HttpRequest  &req);


/*!-------------------------------------------------------------------------
	  function:     isLogOffRequest
	  description:  is request for logging off session
	  arguments:    [in] req, webagent http request object
					
				  
	  return value: true/false
--------------------------------------------------------------------------*/
SAPDB_Bool isLogOffRequest (sapdbwa_HttpRequest  &req);


/*!-------------------------------------------------------------------------
	  function:     isLogOnRequest
	  description:  is request for logging on session
	  arguments:    [in] req, webagent http request object
					
				  
	  return value: true/false
--------------------------------------------------------------------------*/
SAPDB_Bool isLogOnRequest (sapdbwa_HttpRequest  &req);

/*!-------------------------------------------------------------------------
	  function:     isResultPageRequest
	  description:  is request for a result page
	  arguments:    [in] req, webagent http request object
					
				  
	  return value: true/false
--------------------------------------------------------------------------*/
SAPDB_Bool isResultPageRequest(sapdbwa_HttpRequest  &req);

/*!-------------------------------------------------------------------------
	  function:     isZoomRequest
	  description:  is request for zooming a result
	  arguments:    [in] req, webagent http request object
					
				  
	  return value: true/false
--------------------------------------------------------------------------*/
SAPDB_Bool isZoomRequest(sapdbwa_HttpRequest  &req);

/*!-------------------------------------------------------------------------
	  function:     isZoomBackRequest
	  description:  is request to leave zoom window of result
	  arguments:    [in] req, webagent http request object
					
				  
	  return value: true/false
--------------------------------------------------------------------------*/
SAPDB_Bool isZoomBackRequest(sapdbwa_HttpRequest  &req);


/*!-------------------------------------------------------------------------
	  function:     isDSQLExecuteRequest
	  description:  is request for a executing a sql command from input box
	  arguments:    [in] req, webagent http request object
					
				  
	  return value: true/false
--------------------------------------------------------------------------*/
SAPDB_Bool isDSQLExecuteRequest (sapdbwa_HttpRequest  &req);


SAPDB_Bool isNextStatementRequest (sapdbwa_HttpRequest  &req);
SAPDB_Bool isPrevStatementRequest (sapdbwa_HttpRequest  &req);
SAPDB_Bool isClearStatementRequest (sapdbwa_HttpRequest  &req);
SAPDB_Bool isDSQLRefreshRequest (sapdbwa_HttpRequest  &req);
SAPDB_Bool isSelectResultRequest(sapdbwa_HttpRequest  &req);

/*!-------------------------------------------------------------------------
	  function:     isNavigateInResultRequest
	  description:  is request for navigation in result window
	  arguments:    [in] req, webagent http request object
					
				  
	  return value: true/false
--------------------------------------------------------------------------*/
SAPDB_Bool isNavigateInResultRequest (sapdbwa_HttpRequest  &req);



/*!-------------------------------------------------------------------------
	  functions:    check request ....
	  description:  is request for a special event
	  arguments:    [in] req, webagent http request object
					
				  
	  return value: true/false
--------------------------------------------------------------------------*/
SAPDB_Bool isDeleteFolderRequest (sapdbwa_HttpRequest  &req);
SAPDB_Bool isRenameItemRequest (sapdbwa_HttpRequest  &req);
SAPDB_Bool isDeleteFileRequest (sapdbwa_HttpRequest  &req);
SAPDB_Bool isMoveToFolderRequest (sapdbwa_HttpRequest  &req);

SAPDB_Bool isImportFolderRequest (sapdbwa_HttpRequest  &req);
SAPDB_Bool isImportFileRequest (sapdbwa_HttpRequest  &req);
SAPDB_Bool isExportFolderRequest (sapdbwa_HttpRequest  &req);
SAPDB_Bool isExportFileRequest (sapdbwa_HttpRequest  &req);

/*!-------------------------------------------------------------------------
	  functions:    isDBFSRequest
	  description:  is request for a database file action
	  arguments:    [in] req, webagent http request object
					
				  
	  return value: true/false
--------------------------------------------------------------------------*/
SAPDB_Bool isDBFSRequest (sapdbwa_HttpRequest  &req);

/*!-------------------------------------------------------------------------
	  function(s):  isNavigateInResult...Request
	  description:  is request for navigation in result window 
	  arguments:    [in] req, webagent http request object
					
				  
	  return value: true/false
--------------------------------------------------------------------------*/
SAPDB_Bool isNavigateInResultFirstRequest (sapdbwa_HttpRequest  &req);
SAPDB_Bool isNavigateInResultNextRequest (sapdbwa_HttpRequest  &req);
SAPDB_Bool isNavigateInResultPrevRequest (sapdbwa_HttpRequest  &req);
SAPDB_Bool isNavigateInResultLastRequest (sapdbwa_HttpRequest  &req);

/*!-------------------------------------------------------------------------
	  function:     getParameterValue
	  description:  get the value of request paramater
	  arguments:    [in] pParamName, pointer to paramname
					[in] req, webagent http request object
					[in/out] sValue, value retreived
					
				  
	  return value: true/false
--------------------------------------------------------------------------*/
SAPDB_Bool getParameterValue(const char *pParamName,
							 sapdbwa_HttpRequest &req,
							 Tools_DynamicUTF8String &sValue);


/*!-------------------------------------------------------------------------
	  function:     sendUTF8DataAsAscii
	  description:  helper function, as long as webagent doesn't take unicode
	  arguments:    [in] req, webagent http request object
					[in] sData, to be sent
					
				  
	  return value: true/false
--------------------------------------------------------------------------*/
SAPDB_Bool sendUTF8DataAsAscii(sapdbwa_HttpReply &rep,
						 const Tools_DynamicUTF8String& sData);

/*!-------------------------------------------------------------------------

	  function: NewSessionId
	  description: Set a new session id for a WebSession to improve security.

				  Since the session id may be transferred as plaintext over non secure
				  connections anybody can use a network sniffer to read the id to get
				  unauthorized access to the webserver. To make this more difficult the
				  methos NewSessionId is used to change the id from time to time.

				  This function should not be used, if pending request for this session
				  may be open (e.g. multiple HTML frames are filled).

	  arguments:    [in] wa, webagent object	
	  return value: true if success
	--------------------------------------------------------------------------*/
SAPDB_Bool sendNewWebSessinID(sapdbwa_WebAgent &wa);





#endif