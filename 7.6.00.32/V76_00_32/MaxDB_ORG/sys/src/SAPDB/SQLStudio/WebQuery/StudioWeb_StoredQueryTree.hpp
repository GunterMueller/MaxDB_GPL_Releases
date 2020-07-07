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

  module: StudioWeb_StoredQueryTree.hpp

  -----------------------------------------------------------------------

  mail to:  sapdb-general@sap.com

  special area: WebQuery

  description:  class for stored webqueries from sqlstudio

  see also:		/

  -----------------------------------------------------------------------

  copyright:                Copyright (c) 2000-2005 SAP AG

************************************************************************/

#ifndef STUDIOWEB_STOREDQUERYTREE_HPP
#define STUDIOWEB_STOREDQUERYTREE_HPP



//some functions
#include "SAPDB/SQLStudio/WebQuery/StudioWeb_Functions.hpp"

//dbfs object
#include "SAPDB/SQLStudio/Studio_DBFSObject.hpp"

//stored object
#include "SAPDB/SQLStudio/Studio_StoredFileContent.hpp"

class StudioWeb_StoredQueryTree
{

public:

		//these types are compatible with the types from sqlstudio
		//see VB CLass SAPDBGC:SAPDBGlobalCom.cls
		enum StudioWeb_StoredItemTypes
		{

			emUnknown = 0,
			emTable = 1,
			emView = 2,
			emResult = 3,
			emSystem = 4,
			emSnapshot = 5,
			emSynonym = 6,
			emUser = 7,
			emUserGroup = 8,
			emTableOwner = 9,
			emWebFolder = 11,
			emWebItem = 12,
			emStoredDSQL = 13,
			emStoredVIQ = 14,
			emStoredQBE = 15,
			emIndex = 16,
			emSequence = 17,
			emSOFolder = 18,
			emStoredDraft = 19,
			emLinkDSQL = 20,
			emLinkVIQ = 21,
			emLinkQBE = 22,
			emLinkDraft = 23
		};


	  /*!-------------------------------------------------------------------------
      function:     StudioWeb_StoredQueryTree
      description:  Constructor
	  arguments:	[in] poDBFS, pointer to dbfs object
					
      
	  return value: / 
      --------------------------------------------------------------------------*/
      StudioWeb_StoredQueryTree(Studio_DBFSObject			*poDBFS);

	  /*!-------------------------------------------------------------------------
		  function:     StudioWeb_StoredQueryTree
		  description:  Destructor
		  arguments:    /

		  return value: / 
      --------------------------------------------------------------------------*/
	  ~StudioWeb_StoredQueryTree();

	  /*!-------------------------------------------------------------------------
		  function:     doService      
		  description:  handle web request

		  arguments:    [in] wa   ,webAgent object
						[in] req  ,web request object
						[in] reply,web reply object

          return value: / 
      -------------------------------------------------------------------------*/    
	  SAPDB_Bool doService (sapdbwa_WebAgent     & wa,
							sapdbwa_HttpRequest  &req,
	  				        sapdbwa_HttpReply    & rep);

	  /*!-------------------------------------------------------------------------
		function:      checkForQueryTree
		description:   check if the WEBQUERIES Folder of sql studio is valid
		arguments:     /
		
		return value:  true if success	

	  --------------------------------------------------------------------------*/
	  SAPDB_Bool checkForQueryTree();

private:
	
      
      SAPDB_Bool storeMimeParts(sapdbwa_HttpRequest     & req, 
                                Tools_DynamicUTF8String & sError,
                                const char              * boundary);
      
      SAPDB_Bool importItem(sapdbwa_WebAgent    & wa,
                            sapdbwa_HttpRequest & req,
                            sapdbwa_HttpReply   & rep);
      
      SAPDB_Bool exportItem(sapdbwa_WebAgent    & wa,
                            sapdbwa_HttpRequest & req,
                            sapdbwa_HttpReply   & rep);
      
	  /*!-------------------------------------------------------------------------
		  function:     createFolder      
		  description:  create a new folder from a request

		  arguments:    [in] wa   ,webAgent object
						[in] req  ,web request object
						[in] reply,web reply object

          return value: true, if success
      -------------------------------------------------------------------------*/    
	  SAPDB_Bool createFolder(sapdbwa_WebAgent     &wa,
							  sapdbwa_HttpRequest  &req,
							  sapdbwa_HttpReply    &rep);

	  /*!-------------------------------------------------------------------------
		  function:     deleteFolder      
		  description:  delete a folder 

		  arguments:    [in] wa   ,webAgent object
						[in] req  ,web request object
						[in] reply,web reply object

          return value: true, if success
      -------------------------------------------------------------------------*/    
	  SAPDB_Bool deleteFolder(sapdbwa_WebAgent     &wa,
						      sapdbwa_HttpRequest  &req,
						      sapdbwa_HttpReply    &rep);

	  /*!-------------------------------------------------------------------------
		  function:    renameItem      
		  description: rename a item

		  arguments:    [in] wa   ,webAgent object
						[in] req  ,web request object
						[in] reply,web reply object

          return value: true, if success
      -------------------------------------------------------------------------*/    
	  SAPDB_Bool renameItem(sapdbwa_WebAgent     &wa,
							  sapdbwa_HttpRequest  &req,
							  sapdbwa_HttpReply    &rep);


	  /*!-------------------------------------------------------------------------
		  function:    deleteFile      
		  description: delete a folder or file

		  arguments:    [in] wa   ,webAgent object
						[in] req  ,web request object
						[in] reply,web reply object

          return value: true, if success
      -------------------------------------------------------------------------*/    
	  SAPDB_Bool deleteFile(sapdbwa_WebAgent     &wa,
							sapdbwa_HttpRequest  &req,
							sapdbwa_HttpReply    &rep);

	  /*!-------------------------------------------------------------------------
		  function:    moveToFolder      
		  description: move a item to a folder 

		  arguments:    [in] wa   ,webAgent object
						[in] req  ,web request object
						[in] reply,web reply object

          return value: true, if success
      -------------------------------------------------------------------------*/    
	  SAPDB_Bool moveToFolder(sapdbwa_WebAgent     &wa,
							  sapdbwa_HttpRequest  &req,
							  sapdbwa_HttpReply    &rep);

	  /*!-------------------------------------------------------------------------
		  function:    copyToFolder      
		  description: copy a item to a folder 

		  arguments:    [in] wa   ,webAgent object
						[in] req  ,web request object
						[in] rep,  web reply object

          return value: true, if success
      -------------------------------------------------------------------------*/    
	  SAPDB_Bool copyToFolder(sapdbwa_WebAgent     &wa,
							  sapdbwa_HttpRequest  &req,
							  sapdbwa_HttpReply    &rep);

	  /*!-------------------------------------------------------------------------
		  function:    saveFile      
		  description: save a statement from the dsql window

		  arguments:    [in] wa   ,webAgent object
						[in] req  ,web request object
						[in] rep  ,web reply object

          return value: true, if success
      -------------------------------------------------------------------------*/    
	  SAPDB_Bool saveFile(sapdbwa_WebAgent     &wa,
						  sapdbwa_HttpRequest  &req,
						  sapdbwa_HttpReply    &rep);

	  /*!-------------------------------------------------------------------------
		  function:    importFile      
		  description: import a stored statement from the client's filesystem 
                       via HTTP file upload

		  arguments:    [in] wa   ,webAgent object
						[in] req  ,web request object
						[in] rep  ,web reply object

          return value: true, if success
      -------------------------------------------------------------------------*/    
	  SAPDB_Bool importFile(sapdbwa_WebAgent     &wa,
						    sapdbwa_HttpRequest  &req,
						    sapdbwa_HttpReply    &rep);


      /*!-------------------------------------------------------------------------
		  function:    importFolder      
		  description: import a directory tree containing stored statements
                       from the client's filesystem via HTTP file upload

		  arguments:    [in] wa   ,webAgent object
						[in] req  ,web request object
						[in] rep  ,web reply object

          return value: true, if success
      -------------------------------------------------------------------------*/    
	  SAPDB_Bool importFolder(sapdbwa_WebAgent     &wa,
						      sapdbwa_HttpRequest  &req,
						      sapdbwa_HttpReply    &rep);

	  /*!-------------------------------------------------------------------------
		  function:    exportFile      
		  description: export a stored statement to the client's filesystem

		  arguments:    [in] wa   ,webAgent object
						[in] req  ,web request object
						[in] rep  ,web reply object

          return value: true, if success
      -------------------------------------------------------------------------*/    
	  SAPDB_Bool exportFile(sapdbwa_WebAgent     &wa,
						    sapdbwa_HttpRequest  &req,
						    sapdbwa_HttpReply    &rep);

	  /*!-------------------------------------------------------------------------
		  function:    exportFolder      
		  description: export a directory tree containing stored statements 
                       to the client's filesystem

		  arguments:    [in] wa   ,webAgent object
						[in] req  ,web request object
						[in] rep  ,web reply object

          return value: true, if success
      -------------------------------------------------------------------------*/    
	  SAPDB_Bool exportFolder(sapdbwa_WebAgent     &wa,
						      sapdbwa_HttpRequest  &req,
						      sapdbwa_HttpReply    &rep);

	/*!-------------------------------------------------------------------------
		  function:    storeDSQLObject      
		  description: store a statement

		  arguments:    [in] wa   ,webAgent object
						[in] rep  ,web reply object
						[in] sCompletePath, path of file to store dsql object
						[in] poSFC, pointer to sqlstudio object
						[in] bOverWrite, overwrite exisitng file

          return value: true, if success
      -------------------------------------------------------------------------*/    
	  SAPDB_Bool storeDSQLObject(sapdbwa_WebAgent     &wa,
								 sapdbwa_HttpReply    &rep,
								 const Tools_DynamicUTF8String  &sCompletePath,
								 Studio_StoredFileContent *poSFC,
								 SAPDB_Bool bOverWrite);


	  /*!-------------------------------------------------------------------------
		function:      buildTree
		description:   build a js tree from the content of the sql studio webfolder
		arguments:     [in] sResponse, container for HTTP response
			
		return value:  true if success

	  --------------------------------------------------------------------------*/
	  SAPDB_Bool buildTree(Tools_DynamicUTF8String &sResponse);
	  

	  /*!-------------------------------------------------------------------------
		function:      readFirstLevelTree
		description:   read the first level of the sql studio webfolder
		arguments:     [in] sResponse, container for HTTP response
			
		return value:  true if success

	  --------------------------------------------------------------------------*/
		SAPDB_Bool readFirstLevelTree(Tools_DynamicUTF8String &sResponse);

	  /*!-------------------------------------------------------------------------
		function:      makeTreeItem
		description:    make files of webquery folder to tree items 

		// parent.Navigations.items[0].nodes[0].addNode(new parent.node(\"SP\", \"waadmin\", \"Details\"));
		// for lLevel = 0 add build JS addNode
		// parent.Navigations.items[0].addNode(new parent.node(\"View Logfile\", \"waadmin.wa?Service=ShowLogFile\", \"Details\"));\n\

		arguments:     [in] sPath,	   path of file
					   [in] sJSPath,   path for the java script
					   [in] sFileName, name of file
					   [in] lLevel,	   level in hierarchy
					   [out] sResponse,code for tree item

		return value:  true if success
	   --------------------------------------------------------------------------*/
	    SAPDB_Bool makeTreeItem(const Tools_DynamicUTF8String &sPath,
								const Tools_DynamicUTF8String &sJSPath,
								const Tools_DynamicUTF8String &sFileName,				  
								SAPDB_Int2 nLevel,
								Tools_DynamicUTF8String &sResponse);

		/*!-------------------------------------------------------------------------
			function:      readSubDir
			description:    make files of webquery folder to tree items 
			
			arguments:     [in] sParentPath, parent of node
						   [in/out] sJSPath,   path for the java script
						   [in] nNodeCount,  ID of Node
						   [out] sSubTree,    built subtree answer
						   

			return value:  true if success
		--------------------------------------------------------------------------*/
		SAPDB_Bool readSubDir(const Tools_DynamicUTF8String &strParentPath,
							  Tools_DynamicUTF8String &sJSPath,
							  const SAPDB_Int2 nNodeCount,
							  Tools_DynamicUTF8String &sSubTree);

//variables
private:
   
   Studio_DBFSObject			*m_poDBFS;      
   Tools_DynamicUTF8String		m_sRefreshPath;

};


#endif