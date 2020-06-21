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

  module: StudioWeb_WebQuery.hpp

  -----------------------------------------------------------------------

  mail to:  sapdb-general@sap.com

  special area: WebQuery

  description:  main class for WebQuery

  see also:		/

  -----------------------------------------------------------------------

  copyright:                Copyright (c) 2000-2005 SAP AG

************************************************************************/


#ifndef STUDIOWEB_WEBQUERY_HPP
#define STUDIOWEB_WEBQUERY_HPP


/*=======================================================================
 *  INCLUDES
 *=====================================================================*/


#include "gwd00.h"

//html code fragments
#include "SAPDB/SQLStudio/WebQuery/StudioWeb_HtmlFragments.h"

//some functions
#include "SAPDB/SQLStudio/WebQuery/StudioWeb_StoredQueryTree.hpp"

//
#include "SAPDB/SQLStudio/WebQuery/StudioWeb_SQLWindow.hpp"

#include "SAPDB/SQLStudio/WebQuery/StudioWeb_Result.hpp"

//dbfs object
#include "SAPDB/SQLStudio/Studio_DBFSObject.hpp"

#include "SAPDB/SQLStudio/Studio_Connection.hpp"


class StudioWeb_WebQuery
{

public:
	  /*!-------------------------------------------------------------------------
		  function:     StudioWeb_WebQuery
		  description:  Constructor

		  arguments:    /
      
		  return value: / 
      --------------------------------------------------------------------------*/
      StudioWeb_WebQuery();

	  /*!-------------------------------------------------------------------------
		  function:     StudioWeb_WebQuery
		  description:  Destructor
		  arguments:    /

		  return value: / 
      --------------------------------------------------------------------------*/
	  ~StudioWeb_WebQuery();

	  /*!-------------------------------------------------------------------------
		  function:     doService      
		  description:  handle web request

		  arguments:    [in]wa  ,WebAgent object
						[in]req ,Web request object
						[in]rep ,Web reply object
						[out] bCloseSession, should web session be closed
          return value: true if success
      -------------------------------------------------------------------------*/    
	  SAPDB_Bool doService (sapdbwa_WebAgent     &wa,
	   				        sapdbwa_HttpRequest  &req,
					        sapdbwa_HttpReply    &rep,
							SAPDB_Bool &bCloseSession);

	  
	  /*!-------------------------------------------------------------------------
      function:     setDBCPool
      description:  set dbc pool 
      arguments:    [in] pPool, pointer to DBC Pool

	  return value: / 
      --------------------------------------------------------------------------*/
	  void setDBCPool(const sapdbwa_DBCPoolP pDBCPool){m_pDbcPool = pDBCPool;}

	  /*!-------------------------------------------------------------------------
      function:     setODBCDriver
      description:  set ODBC driver
      arguments:    [in] sODBCDriver, odbc driver

	  return value: / 
      --------------------------------------------------------------------------*/
	  void setODBCDriver(const Tools_DynamicUTF8String &sODBCDriver){m_sODBCDriver = sODBCDriver;}

	  
    /*!-------------------------------------------------------------------------
      function:     setSessionID
      -------------------------------------------------------------------------*/
    void setSessionID (const char * sSessionID) {m_sSessionID = sSessionID;}


	/*!-------------------------------------------------------------------------
      function:     userLoggedOn
      -------------------------------------------------------------------------*/
	SAPDB_Bool userLoggedOn(){ return m_bUserLoggedOn;}

private:

	/*!-------------------------------------------------------------------------
      function:     reloadFrame
      description:  send the frame page for logon to sql studio
      arguments:    [in]rep   ,Web reply object
					[in]sSessionID

	  return value: / 
      --------------------------------------------------------------------------*/
	 void reloadFrame(sapdbwa_HttpReply rep,Tools_DynamicUTF8String sSessionID);

	  /*!-------------------------------------------------------------------------
      function:     sendLogonFrame
      description:  send the frame page for logon to sql studio
      arguments:    [in/out]wa      ,WebAgent object
                    [in/out]reply   ,Web reply object

	  return value: / 
      --------------------------------------------------------------------------*/
	  void sendLogonFrame(sapdbwa_WebAgent &wa, sapdbwa_HttpReply rep);


	  /*!-------------------------------------------------------------------------
      function:     sendLogonFrame
      description:  send the frame windows for logon to sql studio
      arguments:    [in]	wa ,webAgent object
					[in]	req,webagent http request object
                    [in]	rep,web reply object					

	  return value: / 
      --------------------------------------------------------------------------*/
	  void sendLogonWindows(sapdbwa_WebAgent &wa,
							sapdbwa_HttpRequest &req,
							sapdbwa_HttpReply &rep);
	  

	  /*!-------------------------------------------------------------------------
	  function:     dbLogon
	  description:  create dbc pool and logon to database
	  arguments:	  [in] wa, webagent object	
					  [in] req, webagent http request object
					  [in] rep, webagent http reply object
					  
	  return value: true, if success
      --------------------------------------------------------------------------*/
	  SAPDB_Bool dbLogon(sapdbwa_WebAgent &wa,
	  					 sapdbwa_HttpRequest &req,
						 sapdbwa_HttpReply &rep);

	  /*!-------------------------------------------------------------------------
	  function:     dbLogOff
	  description:  log off from database and send new logon frame
	  arguments:	[in] wa, webagent object	
					[in] rep, webagent http reply object
					  
	  return value: true, if success
      --------------------------------------------------------------------------*/
	  SAPDB_Bool dbLogOff(sapdbwa_WebAgent &wa,sapdbwa_HttpReply &rep);    

	  /*!-------------------------------------------------------------------------
	  function:     sendLogOffTemplate
	  description:  send logoff template
	  arguments:	[in] wa, webagent object	
					[in] rep, webagent http reply object
					  
	  return value: /
      --------------------------------------------------------------------------*/
	  void sendLogOffTemplate(sapdbwa_WebAgent &wa,sapdbwa_HttpReply &rep);
	    

	  /*!-------------------------------------------------------------------------
	  function:     sendMainFrame
	  description:  build and sends the frame page for webquery
	  arguments:    [in] wa, webagent object	
				    [in] rep, webagent http reply object
				  
	  return value: none
	  --------------------------------------------------------------------------*/
	  void sendMainFrame(sapdbwa_WebAgent &wa,
						 sapdbwa_HttpReply &rep);


	/*!-------------------------------------------------------------------------
	  function:     sendHeaderPage
	  description:  send page with logo and logoff button
	  arguments:    [in] wa, webagent object	
	  				[in] rep, webagent http reply object					
					[in] bLogOff, is logg off button visible
					
				  
	  return value: /
	  --------------------------------------------------------------------------*/
	void sendHeaderPage(sapdbwa_WebAgent &wa,
						sapdbwa_HttpReply &rep,
						SAPDB_Bool bLogOff);

	/*!-------------------------------------------------------------------------
	  function:     checkRequestUserConnected/checkRequestUserNotConnected
	  description:  this function is only used to better arrange all request that have to be handeled
	  arguments:    [in] wa, webagent object	
	  				[in] req ,webagent request object
				    [in] rep, webagent http reply object			
					[out] bCloseSession, should web session be closed					
				  
	  return value: /
	  --------------------------------------------------------------------------*/
	 SAPDB_Bool checkRequestUserConnected(sapdbwa_WebAgent		&wa,
		  	   					   sapdbwa_HttpRequest  &req,
								   sapdbwa_HttpReply	&rep,
								   SAPDB_Bool			&bCloseSession);

	/*!-------------------------------------------------------------------------
	  function:     checkRequestUserNotConnected
	  description:  this function is only used to better arrange all request that have to be handeled
	  arguments:    [in] wa, webagent object	
	  				[in] req ,webagent request object
				    [in] rep, webagent http reply object								
				  
	  return value: /
	  --------------------------------------------------------------------------*/

	 SAPDB_Bool checkRequestUserNotConnected(sapdbwa_WebAgent	  &wa,
		  	   						  sapdbwa_HttpRequest &req,
									  sapdbwa_HttpReply   &rep);

//variables
private:

   StudioWeb_StoredQueryTree *m_pQueryTree;   
   StudioWeb_SQLWindow		 *m_pSQLWindow;			// class to handle sql window
   StudioWeb_Result			 *m_pResult;			// class to handle result window	
   Studio_DBFSObject	   	 *m_poDBFS;				// class to access database file system 
   Studio_Connection		 *m_poConnection;		
   

   Tools_DynamicUTF8String	m_sODBCDriver;   

   //data base connection pool
   sapdbwa_DBCPoolP m_pDbcPool;

   //frame window is sent with tree
   SAPDB_Bool					m_bFrameWithTree;
   //user is connected to database	
   SAPDB_Bool					m_bUserLoggedOn;

   Tools_DynamicUTF8String m_sServer;
   Tools_DynamicUTF8String m_sDatabase;
   Tools_DynamicUTF8String m_sUser;

   // the session ID
   Tools_DynamicUTF8String m_sSessionID;   

};


#endif