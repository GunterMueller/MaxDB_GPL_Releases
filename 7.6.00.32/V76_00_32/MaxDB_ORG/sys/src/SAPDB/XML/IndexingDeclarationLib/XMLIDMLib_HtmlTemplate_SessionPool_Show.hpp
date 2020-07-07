#ifndef XMLIDMLIB_HTMLTEMPLATE_SESSIONPOOL_SHOW_HPP
#define XMLIDMLIB_HTMLTEMPLATE_SESSIONPOOL_SHOW_HPP
/****************************************************************************

  module      : XMLIDMLib_HtmlTemplate_SessionPool_Show.hpp

  -------------------------------------------------------------------------

  responsible : Daniel Kirmse

  special area: 
  description : 


  last changed: 2001-07-12  13:45
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2001-2005 SAP AG

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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


*****************************************************************************/

	/*------------------------------------------------------------------------
		Defines
	--------------------------------------------------------------------------*/

	/*===================================================================
	// Defines
	//===================================================================*/
	#define TEMPLATE_SESSIONPOOL_SHOW						"XMLIDMLib_Html_SessionPool_Show.htm"

	/*===================================================================
	// Template Keys
	//===================================================================*/
	#define TEMPLATE_KEY_SP_SHOW_SERVICE					"Service*"
	#define TEMPLATE_KEY_SP_SHOW_NAME						"PoolName"
	#define TEMPLATE_KEY_SP_SHOW_ID							"PoolID"
	#define TEMPLATE_KEY_SP_SHOW_DESCRIPTION				"PoolDesc"
	#define TEMPLATE_KEY_SP_SHOW_DBNODE						"DbNode"
	#define TEMPLATE_KEY_SP_SHOW_DBNAME						"DbName"
	#define TEMPLATE_KEY_SP_SHOW_DBUSER						"DbUser"
	#define TEMPLATE_KEY_SP_SHOW_DBPWD						"DbPwd"
	#define TEMPLATE_KEY_SP_SHOW_POOLSIZE					"PoolSize"
	#define TEMPLATE_KEY_SP_SHOW_TRACEFILE					"TraceFile"
	#define TEMPLATE_KEY_SP_SHOW_DESCRIPTION_SHOW			"PoolDescShow*"
	#define TEMPLATE_KEY_SP_SHOW_DBNODE_SHOW				"DbNodeShow*"
	#define TEMPLATE_KEY_SP_SHOW_DBNAME_SHOW				"DbNameShow*"
	#define TEMPLATE_KEY_SP_SHOW_DBUSER_SHOW				"DbUserShow*"
	#define TEMPLATE_KEY_SP_SHOW_DBPWD_SHOW					"DbPwdShow*"
	#define TEMPLATE_KEY_SP_SHOW_POOLSIZE_SHOW				"PoolSizeShow*"
	#define TEMPLATE_KEY_SP_SHOW_TRACEFILE_SHOW				"TraceFileShow*"
	#define TEMPLATE_KEY_SP_SHOW_WEBDBM						"WebDBM"
	#define TEMPLATE_KEY_SP_SHOW_IDXSERVICE_ID				"ServiceID"
	#define TEMPLATE_KEY_SP_SHOW_IDXSERVICE_NAME			"ServiceName"
	#define TEMPLATE_KEY_SP_SHOW_IDXSERVICE_DESC			"ServiceDesc"
	#define TEMPLATE_KEY_SP_SHOW_IDXSERVICE_STORE			"DocIdxStore"
	#define TEMPLATE_KEY_SP_SHOW_IDXSERVICE_DSSP			"DocStore"
	#define TEMPLATE_KEY_SP_SHOW_IDXSERVICE_ISSP			"IdxStore"
	#define TEMPLATE_KEY_SP_SHOW_SHOW						"SPShow"
	#define TEMPLATE_KEY_SP_SHOW_CHANGE						"SPChange"
	#define TEMPLATE_KEY_SP_SHOW_HEADING					"Heading*"
	#define TEMPLATE_KEY_SP_SHOW_HEADING_SHOW				"HeadingShow*"
	#define TEMPLATE_KEY_SP_SHOW_IDXSERVICE_DSSPNEW			"DSSPNew"
	#define TEMPLATE_KEY_SP_SHOW_IDXSERVICE_ISSPNEW			"ISSPNew"

	/*===================================================================
	// Template Values
	//===================================================================*/
	#define TEMPLATE_VALUE_SP_SHOW_SERVICE					"Service"
	#define TEMPLATE_VALUE_SP_SHOW_NAME						"PoolName"
	#define TEMPLATE_VALUE_SP_SHOW_ID						"PoolID"
	#define TEMPLATE_VALUE_SP_SHOW_DESCRIPTION				"PoolDesc"
	#define TEMPLATE_VALUE_SP_SHOW_DBNODE					"DbNode"
	#define TEMPLATE_VALUE_SP_SHOW_DBNAME					"DbName"
	#define TEMPLATE_VALUE_SP_SHOW_DBUSER					"DbUser"
	#define TEMPLATE_VALUE_SP_SHOW_DBPWD					"DbPwd"
	#define TEMPLATE_VALUE_SP_SHOW_POOLSIZE					"PoolSize"
	#define TEMPLATE_VALUE_SP_SHOW_TRACEFILE				"TraceFile"
	#define TEMPLATE_VALUE_SP_SHOW_WEBDBM					"WebDBM"
	#define TEMPLATE_VALUE_SP_SHOW_IDXSERVICE_ID			"ServiceID"
	#define TEMPLATE_VALUE_SP_SHOW_IDXSERVICE_NAME			"ServiceName"
	#define TEMPLATE_VALUE_SP_SHOW_IDXSERVICE_DESC			"ServiceDesc"
	#define TEMPLATE_VALUE_SP_SHOW_IDXSERVICE_STORE			"DocIdxStore"
	#define TEMPLATE_VALUE_SP_SHOW_IDXSERVICE_DSSP			"DocStore"
	#define TEMPLATE_VALUE_SP_SHOW_IDXSERVICE_ISSP			"IdxStore"
	#define TEMPLATE_VALUE_SP_SHOW_IDXSERVICE_DSSPNEW		"DSSPNew"
	#define TEMPLATE_VALUE_SP_SHOW_IDXSERVICE_ISSPNEW		"ISSPNew"
	#define TEMPLATE_VALUE_SP_SHOW_STORE					"store"

	/*===================================================================
	// Includes
	//===================================================================*/
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Include.hpp"
	#include "SAPDB/XML/IndexingDeclarationLib/XMLIDMLib_Common_Utils.hpp"


	/*===================================================================
	// Class XMLIDMLib_HtmlTemplate_SessionPool_Show
	//===================================================================*/
	class XMLIDMLib_HtmlTemplate_SessionPool_Show : public Tools_Template 
	{
		public:

			XMLIDMLib_HtmlTemplate_SessionPool_Show(sapdbwa_WebAgent & wa,
										   sapdbwa_HttpRequest &req,
										   XMLIMAPI_Handle	&xml);

			~XMLIDMLib_HtmlTemplate_SessionPool_Show();

		private:

			XMLIMAPI_Handle				&_xml;
			sapdbwa_HttpRequest			&_req;
			sapdbwa_WebAgent			&_wa;
			sapdbwa_SessionPool			*_pool;

			XMLIMAPI_SessionPool		_sessionPool;
			XMLIDMLIB_Char				_serviceName[512];
			XMLIDMLIB_Char				_idxServiceID[50];
			XMLIDMLIB_Char				_idxServiceName[130];
			XMLIDMLIB_Char				_idxServiceDesc[520];
			XMLIDMLIB_Char				_store[10];
			XMLIDMLIB_Char				_docstore[130];
			XMLIDMLIB_Char				_idxstore[130];
			XMLIDMLIB_Char				_issync[3];
			XMLIDMLIB_Char				_dsspnew[3];
			XMLIDMLIB_Char				_isspnew[3];

			XMLIDMLIB_Int2 askForWriteCount(const Tools_DynamicUTF8String & szName);

			Tools_DynamicUTF8String askForValue (const Tools_DynamicUTF8String & szName);

			void getSessionPool();

	}; 




#endif