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

  module: StudioWeb_Result.hpp

  -----------------------------------------------------------------------

  responsible: WolfgangA

  special area: WebQuery

  description:  class for a sql window

  see also:		/

  copyright:                Copyright (c) 2000-2005 SAP AG

************************************************************************/

#ifndef STUDIOWEB_RESULT_HPP
#define STUDIOWEB_RESULT_HPP

//some functions
#include "SAPDB/SQLStudio/WebQuery/StudioWeb_Functions.hpp"
//resulttemplate
#include "SAPDB/SQLStudio/WebQuery/StudioWeb_ResultTemplate.hpp"

#include "SAPDB/SQLStudio/Studio_StoredFileContent.hpp"

#include "SAPDB/SQLStudio/Studio_Connection.hpp"

#include "SQLStudio/WebQuery/StudioWeb_TemplateParamQuery.hpp"

//stored object
#include "SAPDB/SQLStudio/Studio_StoredFileContent.hpp"

#include "SAPDB/SQLStudio/Studio_List.hpp"



class StudioWeb_Result
{

public:
	/*!-------------------------------------------------------------------------
		  function:     StudioWeb_Result
		  description:  Constructor
		  arguments:		[in] poConnection, pointer to connection object
		  										
      
		  return value: / 
      --------------------------------------------------------------------------*/
	 StudioWeb_Result(Studio_Connection* poConnection);

	 /*!-------------------------------------------------------------------------
		  function:     ~StudioWeb_Result
		  description:  Destructor
		  arguments:    /

		  return value: / 
      --------------------------------------------------------------------------*/
	~StudioWeb_Result();

	/*!-------------------------------------------------------------------------
		  function:     doService      
		  description:  handle web request

		  arguments:    [in] wa   ,web ggent object
						[in] req  ,web request object
						[in] rep  ,web reply object
						[in] poSFC , pointer to stored file object
						[in] bCloseExecuteConnection, flag if connection for execute has to be changed

          return value: true if success
      -------------------------------------------------------------------------*/    
	  SAPDB_Bool doService (sapdbwa_WebAgent     &wa,
	  				        sapdbwa_HttpRequest  &req,
							sapdbwa_HttpReply    &rep,
							Studio_StoredFileContent	*poSFC,
							SAPDB_Bool bCloseExecuteConnection);

	  

//private functions
private:

		struct StudioWeb_ResultCollection
		{
		    StudioOAL_WResult		  *pResult;
		    StudioWeb_ResultTemplate  *pTable;
		    SAPDB_Int4				  lastResultSize;    
		   //last user action in navigation of query result
			StudioWeb_FetchDirection		lastAction;
			Tools_DynamicUTF8String         sStatmentHeader;
			SAPDB_Bool						bBorderOfResult;
		};

		/*!-------------------------------------------------------------------------
		  function:     getResultNo
		  description:  get number of requested result

		  arguments:	[in] req  ,Web request object
						
		  return value: number of requested result
		-------------------------------------------------------------------------*/    
		SAPDB_Int2 getResultNo(sapdbwa_HttpRequest  &req);

		/*!-------------------------------------------------------------------------
		  function:     getNewResultCollection
		  description:  get a new result collection struct

		  arguments:	/
						
		  return value: pointer to StudioWeb_ResultCollection if success
		-------------------------------------------------------------------------*/    
		StudioWeb_ResultCollection* getNewResultCollection(); 


		/*!-------------------------------------------------------------------------
		  function:     clearResultCollection
		  description:  clear result collection struct

		  arguments:	/
						
		  return value: /
		-------------------------------------------------------------------------*/    
		void clearResultCollection();

		/*!-------------------------------------------------------------------------
		  function:     rollBack
		  description:  roll back all actions on a connection

		  arguments:	[in] poConnection   ,connection for rollback
						
		  return value: true
		-------------------------------------------------------------------------*/    
		void rollBack(Studio_Connection	*poConnection);

		/*!-------------------------------------------------------------------------
		  function:     executeStatements
		  description:  execute statements 

		  arguments:	[in] wa   ,web agent object
						[in] rep  ,web reply object						

		  return value: true
		-------------------------------------------------------------------------*/    
		SAPDB_Bool  executeStatements(sapdbwa_WebAgent    &wa,sapdbwa_HttpReply   &rep);

	  /*!-------------------------------------------------------------------------
		  function:     handleError
		  description:  if a error in sql execute occured, get error text and send it

		  arguments:	[in] pResult, pointer to result object
						[in] rep  ,web reply object
						

		  return value: true
      -------------------------------------------------------------------------*/    
	  void  handleError(StudioOAL_WResult		 *pResult,
						sapdbwa_HttpReply		 &rep);

	  /*!-------------------------------------------------------------------------
		  function:     getError
		  description:  if a error in sql execute occured, get error text

		  arguments:	[in] pResult, pointer to result object
						[in] sError, buffer to receive
						

		  return value: true
      -------------------------------------------------------------------------*/    
	  void getError(StudioOAL_WResult		 *pResult,
						   Tools_DynamicUTF8String	 &sError);

	  /*!-------------------------------------------------------------------------
		  function:     buildOneRow
		  description:  build one row for result template

		  arguments:	[in/out] pResultCollection, result collection
						[in]     nTemplateRow, row to build in template
						

		  return value: true
      -------------------------------------------------------------------------*/    
	  SAPDB_Bool buildOneRow(StudioWeb_ResultCollection *pResultCollection,SAPDB_Int2 nTemplateRow);

	  /*!-------------------------------------------------------------------------
		  function:     buildInternalModeButtonForm
		  description:  build navigation button for internal mode

		  arguments:	[in/out] oTable, result template
						[in]     disButton, which buttons are disabled
						

		  return value: true
      -------------------------------------------------------------------------*/    
	  void buildInternalModeButtonForm(StudioWeb_ResultTemplate* poTable,StudioWeb_DisableButton disButton);

	  
	  /*!-------------------------------------------------------------------------
		  function:     fillResultList      
		  description:  fill the drop down list of the resulu page
		  arguments:    [in] pResultCollection, pointer to result collection object
						

          return value: /
      -------------------------------------------------------------------------*/    
	  void fillResultList(StudioWeb_ResultCollection  *pResultCollection);

	  /*!-------------------------------------------------------------------------
		  function:     sendGetParamPage      
		  description:  if there are param tags in sql statement send html page to get them

		  arguments:    [in] wa, web agent object
						[in] rep  ,Web reply object
						[in] sStatement, reference to string class with sql statement
						

          return value: true if success
      -------------------------------------------------------------------------*/    
	  SAPDB_Bool sendGetParamPage(sapdbwa_WebAgent    &wa,
								  sapdbwa_HttpReply   &rep,
								  const Tools_DynamicUTF8String &sStatement);

	  /*!-------------------------------------------------------------------------
		  function:     getStatementOfParamPage      
		  description:  get the value for a param request out of hml pafe

		  arguments:    [in] req  ,Web request object
						[in] sOldStatement, reference to string class with parameterized sql statement
						[out] sNewStatement, reference to string class with new sql statement
						

          return value: true if success
      -------------------------------------------------------------------------*/    
	  SAPDB_Bool getStatementOfParamPage(sapdbwa_HttpRequest  &req,
										 Tools_DynamicUTF8String &sOldStatement,
										 Tools_DynamicUTF8String &sNewStatement);


	/*!-------------------------------------------------------------------------
		  function:     sendResultPage
		  description:  build a (html) response of a query

		  arguments:    [in] wa   ,web agent object
						[in] rep  ,web reply object
						[in] pResultCollection, pointer to result collection object
						[in] dir, direction in which result is fetched

		  return value: true
      -------------------------------------------------------------------------*/    
	  SAPDB_Bool  sendResultPage(sapdbwa_WebAgent     &wa,
								    sapdbwa_HttpReply    &rep,
									StudioWeb_ResultCollection *pResultCollection,
								    StudioWeb_FetchDirection dir);


	  /*!-------------------------------------------------------------------------
		  function:     buildResultData
		  description:  build a (html) response of a sql query

		  arguments:    [in] rep  ,web reply object
						[in] pResultCollection, pointer to result collection object
						[in] emFetchDirNow, is result fetched

		  return value: /
      -------------------------------------------------------------------------*/    
	  void buildResultData(sapdbwa_HttpReply		  &rep,
						  StudioWeb_ResultCollection *pResultCollection,
						  StudioWeb_FetchDirection emFetchDirNow);

	  
	  /*!-------------------------------------------------------------------------
		  function:     buildParamResultData
		  description:  build a (html) response of a param query

		  arguments:    [in] pResultCollection, pointer to result collection object

		  return value: /
      -------------------------------------------------------------------------*/    
	  void buildParamResultData(StudioWeb_ResultCollection *pResultCollection);

	  /*!-------------------------------------------------------------------------
		  function:     handleExecuteRequest
		  description:  handle a sql query

		  arguments:    [in] wa   ,web agent object
						[in] rep  ,web reply object						
						

		  return value: /
      -------------------------------------------------------------------------*/    
	  void handleExecuteRequest(sapdbwa_WebAgent     &wa,
								sapdbwa_HttpReply    &rep);

	  /*!-------------------------------------------------------------------------
		  function:     getFormatedStatement
		  description:  format the sql statement

		  arguments:    [in] pResultCollection, pointer to result collection object
						[in] lLength, lenth of formated statement
						[out] sStatement, reference to string will receive the formated sql statement

		  return value: /
      -------------------------------------------------------------------------*/    
	  void getFormatedStatement(StudioWeb_ResultCollection *pResultCollection,
							    SAPDB_UInt lLength,
								Tools_DynamicUTF8String &sStatement);

	  /*!-------------------------------------------------------------------------
		  function:     getFetchParams
		  description:  format the sql statement

		  arguments:    [in] pResultCollection, pointer to result collection object
						[in] emFetchDir, fetch direction that will be taken
						[in] emLastAction, fetch direction that was taken
						[out] lSQLParam,	 fetch dir for extended fetch
						[out] lSQLParam2, fetch dir for another extended fetch
						[out] returns start row for fetching

		  return value: 
      -------------------------------------------------------------------------*/    
	  SAPDB_Bool getFetchParams(StudioWeb_ResultCollection *pResultCollection,
									   StudioWeb_FetchDirection emFetchDir,
									   StudioWeb_FetchDirection emLastAction,
									   SAPDB_UInt2 &lSQLParam,
									   SAPDB_UInt2 &lSQLParam2,
									   SAPDB_Int4 &nStartRow);

	  /*!-------------------------------------------------------------------------
		  function:     drawResultNo
		  description:  redraw result 

		  arguments:	[in] nResultNo, number of result to redraw
						[in] fDir, direction in which result is fetched
						[in] wa   ,web agent object							
  						[in] rep  ,web reply object						


		  return value: /
      -------------------------------------------------------------------------*/    
	  void drawResultNo(SAPDB_Int2	nResultNo,
						StudioWeb_FetchDirection fDir,
						sapdbwa_WebAgent     &wa,
						sapdbwa_HttpReply    &rep);

	  /*!-------------------------------------------------------------------------
		  function:     setZoomPos
		  description:  set the position in hte odbc cursor for zooming

		  arguments:    [in] pResultCollection, pointer to result collection object
						[in/out] nRow, in value is desired zoom row, out value is used for zooming
						[in/out] nCol, in value is desired zoom col, out value is used for zooming
						

		  return value: true if success
      -------------------------------------------------------------------------*/    
	  SAPDB_Bool setZoomPos (StudioWeb_ResultCollection* pResultCollection,
							 SAPDB_Int2 &nRow,
							 SAPDB_Int2 &nCol);

	  /*!-------------------------------------------------------------------------
		  function:     doZoom      
		  description:  handle a zoom request

		  arguments:    [in] nResultNo, number of result to redraw
						[in] wa   ,WebAgent object
						[in] req  ,Web request object
						[in] rep  ,Web reply object

          return value: true if success
      -------------------------------------------------------------------------*/    
	  SAPDB_Bool doZoom (SAPDB_Int2	nResultNo,
						 sapdbwa_WebAgent     & wa,
	  					 sapdbwa_HttpRequest  &req,
						 sapdbwa_HttpReply    &rep);



//variables
private:
    Studio_Connection			*m_poConnection;
	Studio_Connection			*m_poExecuteConnection;

    //size of last query result
    SAPDB_Int2					m_CurrentResult;    
	Studio_StoredFileContent	*m_poSFC; 

	//flag if actually connection should be closed
	SAPDB_Bool					m_bCloseExecuteConnection;

	StudioWeb_TemplateParamQuery *m_poParamTemplate;    	        
	StudioWeb_ResultTemplate	 *m_poZoomTemplate;    	        
	Studio_List					 m_ResultList;	

	
};

#endif
