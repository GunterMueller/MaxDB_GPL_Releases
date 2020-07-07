#ifndef XMLIMAPI_DEFINITION_H
#define XMLIMAPI_DEFINITION_H
/*!***************************************************************************

  module      : XMLIMAPI_Definition.h

  -------------------------------------------------------------------------

  responsible : Daniel Kirmse

  special area: XML Indexing for WebDAV and IndexEngine
  description : API for modelling of XML Indexing Information. 
				Contains Functions for definition and data retrieval of 
				XML Indices, Document Classes, Session Pools, Service Libraries
				and Indexing Services.


  last changed: 2001-08-02  9:50
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
#include <WINDOWS.H>

	#include "vpa00sql.h"
	#include "vpa00sqlext.h"

	/*------------------------------------------------------------------------
	 |   TYPES                                                               |
	 -------------------------------------------------------------------------*/
	/* Basic types */
	typedef unsigned int		XMLIMAPI_UInt4;
	typedef unsigned short		XMLIMAPI_UInt2;
	typedef unsigned char		XMLIMAPI_UInt1;
	typedef short				XMLIMAPI_Int2;
	typedef int			        XMLIMAPI_Int4;
	typedef unsigned char		XMLIMAPI_Bool;
	typedef char				XMLIMAPI_Char;
	typedef unsigned char		XMLIMAPI_UChar;
	typedef unsigned char		*XMLIMAPI_PChar;
	typedef double				XMLIMAPI_Double;

	/*--------------------------------------------------------------------------
	declaration: sapdbwa_{ULong | Long}
	description: basic long data types for fastest execution
				 (machine-dependent size)
	--------------------------------------------------------------------------*/
	#if defined(WIN32) && defined(_M_IA64)
	typedef unsigned __int64    XMLIMAPI_ULong;      /* long (fast, unsigned) */
	typedef signed __int64      XMLIMAPI_Long;       /* long (fast, signed)   */
	#else
	typedef unsigned long       XMLIMAPI_ULong;      /* long (fast, unsigned) */
	typedef signed long         XMLIMAPI_Long;       /* long (fast, signed)   */
	#endif

	#define XMLIMAPI_True			((XMLIMAPI_Bool)1)
	#define XMLIMAPI_False			((XMLIMAPI_Bool)0)

	typedef void                *XMLIMAPI_Buffer;
	typedef XMLIMAPI_ULong        XMLIMAPI_BufferLength;

	#define XMLIMAPI_MAX_ID_LEN              24
	#define XMLIMAPI_MAX_ID_STRING_LEN       2*XMLIMAPI_MAX_ID_LEN
	#define XMLIMAPI_MAX_ID_DEBUG_STRING_LEN 52  /* See also RTE_UniqueIdBuffer in RTE_UniqueId.h */

	typedef char                            XMLIMAPI_Id[XMLIMAPI_MAX_ID_LEN];
	typedef char                            XMLIMAPI_IdString[XMLIMAPI_MAX_ID_STRING_LEN+1];
	typedef char                            XMLIMAPI_IdDebugString[XMLIMAPI_MAX_ID_DEBUG_STRING_LEN+1];

	#define XMLIMAPI_MAX_ERROR_TEXT_LEN                      1000
	#define XMLIMAPI_MAX_STATE_TEXT_LEN                       100

	/*!-----------------------------------------------------------------------
		Chapter		:	Data Types and Structures	
	--------------------------------------------------------------------------*/
	/*!-----------------------------------------------------------------------
		Chapter		:	API Handle
	--------------------------------------------------------------------------*/
	/*!-----------------------------------------------------------------------
		Declaration	:	XMLIMAPI_Handle
		description	:	API Handle for storing session relevant information 
						(i.e., prepared statements, bound parameter and columns) 
	--------------------------------------------------------------------------*/
	typedef struct st_xmlimapi_handle		*XMLIMAPI_Handle;
	/*!-----------------------------------------------------------------------
		EndDeclaration	:	XMLIMAPI_Handle
	--------------------------------------------------------------------------*/
	/*!-----------------------------------------------------------------------
		EndChapter		:	XMLIMAPI_Handle
	--------------------------------------------------------------------------*/
	/*!-----------------------------------------------------------------------
		Chapter		:	XML IndexEngine Admin
	--------------------------------------------------------------------------*/
	/*!----------------------------------------------------------------------
		Declaration	:	XMLIMAPI_XieAdmin	
		description	:	Structure of a XML IndexEngine Admin record incl. all needed
		                values for a sufficent GUI presentation. Values are 
						stored in DESC DB.
	--------------------------------------------------------------------------*/
	struct st_api_xie_admin {
		XMLIMAPI_Char              ID[55];
		XMLIMAPI_Char              XieName[129];
		XMLIMAPI_Char              description[513];
		XMLIMAPI_Char              Node[257];
		XMLIMAPI_Char              InstRoot[512];
		XMLIMAPI_Char              IndexingServiceID[55];
		XMLIMAPI_Char              IndexingServiceName[129];
		XMLIMAPI_Char              IndexingServiceDesc[513];
		XMLIMAPI_Char              IndexingServiceDocStoreName[129];
		XMLIMAPI_Char              IndexingServiceIdxStoreName[129];
		XMLIMAPI_Char              IndexingLibID[55];
		XMLIMAPI_Char              IndexingLibName[513];
		XMLIMAPI_Char              IndexingLibDesc[513];
		XMLIMAPI_Char              IndexingLibVersion[129];
		XMLIMAPI_Char	           LogFile[513];
		XMLIMAPI_Char              Alive[50];
		XMLIMAPI_Char              Refresh[2];
	};
	typedef	struct st_api_xie_admin		XMLIMAPI_XieAdmin;
	typedef	struct st_api_xie_admin		*XMLIMAPI_XieAdminP;
	/*!-----------------------------------------------------------------------
		EndDeclaration : XMLIMAPI_XieAdmin
	--------------------------------------------------------------------------*/
	/*!-----------------------------------------------------------------------
		Declaration	:	XMLIMAPI_XieAdminListItem
		description	:	List item for simple linked list of XML Indices
	--------------------------------------------------------------------------*/
	struct st_api_xieadmin_list_item {
		XMLIMAPI_XieAdminP                 item;
		struct st_api_xieadmin_list_item    *next;
	};
	typedef struct st_api_xieadmin_list_item	*XMLIMAPI_XieAdminListItem;
	/*!------------------------------------------------------------------------
		EndDeclaration	:	XMLIMAPI_XieAdminListItem 
	--------------------------------------------------------------------------*/
	
	/*!-----------------------------------------------------------------------
		Declaration	:	XMLIMAPI_XieAdminList
		description	:	Simple linked list for XML Indices 
	--------------------------------------------------------------------------*/
	struct st_api_xieadmin_list {
		XMLIMAPI_XieAdminListItem		first;
		XMLIMAPI_XieAdminListItem		last;
		XMLIMAPI_XieAdminListItem		curr;
	};
	typedef struct st_api_xieadmin_list			*XMLIMAPI_XieAdminList;
	/*!-----------------------------------------------------------------------
		EndDeclaration	:	XMLIMAPI_XieAdminList
	--------------------------------------------------------------------------*/
	/*!-----------------------------------------------------------------------
		EndChapter		:	XML IndexEngine Admin
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Chapter		:	Service Description and List
	--------------------------------------------------------------------------*/
	/*!----------------------------------------------------------------------
		Declaration	:	XMLIMAPI_ServiceDesc	
		description	:	Structure of a service description as it is stored within
						the database. All session pools given as ID.
	--------------------------------------------------------------------------*/
	struct st_api_service_desc {
		XMLIMAPI_Char				ServiceID[55];
		XMLIMAPI_Char				ServiceName[129];
		XMLIMAPI_Char				ServiceDesc[513];
		XMLIMAPI_Char				DocStoreID[55];
		XMLIMAPI_Char				IdxStoreID[55];
		XMLIMAPI_Char				isSynchronous[2];
	};
	typedef	struct st_api_service_desc		XMLIMAPI_ServiceDesc;
	typedef	struct st_api_service_desc		*XMLIMAPI_ServiceDescP;
	/*!-----------------------------------------------------------------------
		EndDeclaration 
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Declaration	:	XMLIMAPI_ServiceDescListItem
		description	:	List item for a simple linked list of service descriptions 
	--------------------------------------------------------------------------*/
	struct st_api_service_desc_list_item {
		XMLIMAPI_ServiceDescP                  item;
		struct st_api_service_desc_list_item    *next;
	};
	typedef struct st_api_service_desc_list_item	*XMLIMAPI_ServiceDescListItem;
	/*!-----------------------------------------------------------------------
		EndDeclaration 
	--------------------------------------------------------------------------*/

	/*!------------------------------------------------------------------------
		Declaration	:	XMLIMAPI_ServiceDescList 
		description	:	Simple linked list for Service Descriptions.
						curr is only set via GetFirst and GetNext functions.
	--------------------------------------------------------------------------*/
	struct st_api_service_desc_list {
		XMLIMAPI_ServiceDescListItem           first;
		XMLIMAPI_ServiceDescListItem           last;
		XMLIMAPI_ServiceDescListItem           curr;
	};
	typedef struct st_api_service_desc_list     *XMLIMAPI_ServiceDescList;
	/*!-----------------------------------------------------------------------
		EndDeclaration 
	--------------------------------------------------------------------------*/
	/*!-----------------------------------------------------------------------
		EndChapter		:	Service Description and List
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Chapter		:	Indexing Service
	--------------------------------------------------------------------------*/
	/*!-----------------------------------------------------------------------
		Declaration	:	XMLIMAPI_ISSessionPool
		description	:	complete description of a session pool including informations
						on the service library containing the DestroyUserData callback
						function 
	--------------------------------------------------------------------------*/
	struct st_api_indexing_service_sp {
		XMLIMAPI_Char              PoolID[55];
		XMLIMAPI_Char              PoolName[129];
		XMLIMAPI_Char              DbNode[129];
		XMLIMAPI_Char              DbName[129];
		XMLIMAPI_Char              DbUser[129];
		XMLIMAPI_Char              DbPassword[129];
		XMLIMAPI_Char              TraceFile[257];
		XMLIMAPI_Char			   Assignments[10];
	};
	typedef struct st_api_indexing_service_sp		XMLIMAPI_ISSessionPool;
	typedef struct st_api_indexing_service_sp		*XMLIMAPI_ISSessionPoolP;
	/*!-----------------------------------------------------------------------
		EndDeclaration	:	XMLIMAPI_ISSessionPool 
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Declaration	:	XMLIMAPI_IndexingService
		description	:	complete nested structure for a indexing service including all
						informations for the callback functions (i.e., Service Library and
						Session Pool) 
	--------------------------------------------------------------------------*/
	struct st_api_indexing_service {
		XMLIMAPI_Char				ServiceID[55];
		XMLIMAPI_Char				ServiceName[129];
		XMLIMAPI_Char				ServiceDesc[513];
		XMLIMAPI_Char				DocStoreID[55];
		XMLIMAPI_Char				IdxStoreID[55];
		XMLIMAPI_ISSessionPool		DocStore;
		XMLIMAPI_ISSessionPool		IdxStore;
		XMLIMAPI_Char				isSynchronous[2];
	};
	typedef struct st_api_indexing_service			XMLIMAPI_IndexingService;
	typedef struct st_api_indexing_service			*XMLIMAPI_IndexingServiceP;
	/*!-----------------------------------------------------------------------
		EndDeclaration	:	XMLIMAPI_IndexingService 
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		EndChapter		:	Indexing Service
	--------------------------------------------------------------------------*/

	/*!------------------------------------------------------------------------
		Chapter		:	XML Index and List
	--------------------------------------------------------------------------*/
	/*!------------------------------------------------------------------------
		Declaration	:	XMLIMAPI_XmlIndex
		description	:	short description of XML Index (only informations for indexing) 
	--------------------------------------------------------------------------*/
	struct st_api_xml_index {
		XMLIMAPI_Char              IndexID[55];
		XMLIMAPI_Char              indexName[129];
		XMLIMAPI_Char              description[513];
		XMLIMAPI_Char              xpathBasePath[513];
		XMLIMAPI_Char              xpathValuePath[513];
	};
	typedef struct st_api_xml_index    XMLIMAPI_XmlIndex;
	typedef struct st_api_xml_index    *XMLIMAPI_XmlIndexP;
	/*!-----------------------------------------------------------------------
		EndDeclaration	:	XMLIMAPI_XmlIndex 
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Declaration	:	XMLIMAPI_XmlIndexListItem
		description	:	List item for simple linked list of XML Indices
	--------------------------------------------------------------------------*/
	struct st_api_xmlindex_list_item {
		XMLIMAPI_XmlIndexP                 item;
		struct st_api_xmlindex_list_item    *next;
	};
	typedef struct st_api_xmlindex_list_item	*XMLIMAPI_XmlIndexListItem;
	/*!------------------------------------------------------------------------
		EndDeclaration	:	XMLIMAPI_XmlIndexListItem 
	--------------------------------------------------------------------------*/
	
	/*!-----------------------------------------------------------------------
		Declaration	:	XMLIMAPI_XmlIndexList
		description	:	Simple linked list for XML Indices 
	--------------------------------------------------------------------------*/
	struct st_api_xmlindex_list {
		XMLIMAPI_XmlIndexListItem		first;
		XMLIMAPI_XmlIndexListItem		last;
		XMLIMAPI_XmlIndexListItem		curr;
		XMLIMAPI_UInt4					count;
	};
	typedef struct st_api_xmlindex_list			*XMLIMAPI_XmlIndexList;
	/*!-----------------------------------------------------------------------
		EndDeclaration	:	XMLIMAPI_XmlIndexList
	--------------------------------------------------------------------------*/
	/*!-----------------------------------------------------------------------
		EndChapter		:	XML Index and List
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Chapter		:	Document Class and List
	--------------------------------------------------------------------------*/
	/*!-----------------------------------------------------------------------
		Declaration	:	XMLIMAPI_DocClass
		description	:	Short description of Document Class (without assigned XML Indices) 
	--------------------------------------------------------------------------*/
	struct st_api_doc_class {
		XMLIMAPI_Char				docClassID[55];
		XMLIMAPI_Char				docClassName[129];
		XMLIMAPI_Char				description[513];
	};
	typedef struct st_api_doc_class				XMLIMAPI_DocClass;
	typedef struct st_api_doc_class				*XMLIMAPI_DocClassP;
	/*!-----------------------------------------------------------------------
		EndDeclaration	:	XMLIMAPI_DocClass
	--------------------------------------------------------------------------*/
	/*!-----------------------------------------------------------------------
		Declaration	:	XMLIMAPI_DocClassListItem
		description	:	List item for simple linked list of Document Classes
	--------------------------------------------------------------------------*/
	struct st_api_docclass_list_item {
		XMLIMAPI_DocClassP                 item;
		struct st_api_docclass_list_item    *next;
	};
	typedef struct st_api_docclass_list_item	*XMLIMAPI_DocClassListItem;
	/*!------------------------------------------------------------------------
		EndDeclaration	:	XMLIMAPI_DocClassListItem 
	--------------------------------------------------------------------------*/
	
	/*!-----------------------------------------------------------------------
		Declaration	:	XMLIMAPI_DocClassList
		description	:	Simple linked list for Document Classes 
	--------------------------------------------------------------------------*/
	struct st_api_docclass_list {
		XMLIMAPI_DocClassListItem		first;
		XMLIMAPI_DocClassListItem		last;
		XMLIMAPI_DocClassListItem		curr;
	};
	typedef struct st_api_docclass_list			*XMLIMAPI_DocClassList;
	/*!-----------------------------------------------------------------------
		EndDeclaration	:	XMLIMAPI_DocClassList
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		EndChapter		:	Document Class and List
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Chapter		:	Session Pool and List
	--------------------------------------------------------------------------*/
	/*!------------------------------------------------------------------------
		Declaration	:	XMLIMAPI_SessionPool
		description	:	Structure for a Session Pool as stored in database 
						(DestroyUserData callback function only as ID) 
	--------------------------------------------------------------------------*/
	struct st_api_session_pool {
		XMLIMAPI_Char				PoolID[55];
		XMLIMAPI_Char				PoolName[129];
		XMLIMAPI_Char				description[513];
		XMLIMAPI_Char				DbNode[129];
		XMLIMAPI_Char				DbName[129];
		XMLIMAPI_Char				DbUser[129];
		XMLIMAPI_Char				DbPassword[129];
		XMLIMAPI_Char				TraceFile[257];
		XMLIMAPI_Char				Assignments[10];
	};
	typedef struct st_api_session_pool		XMLIMAPI_SessionPool;
	typedef struct st_api_session_pool		*XMLIMAPI_SessionPoolP;
	/*!-----------------------------------------------------------------------
		EndDeclaration	:	XMLIMAPI_SessionPool 
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Declaration	:	XMLIMAPI_SessionPoolListItem
		description	:	List item for simple linked list of Session Pools 
	--------------------------------------------------------------------------*/
	struct st_api_session_pool_list_item {
		XMLIMAPI_SessionPoolP                  item;
		struct st_api_session_pool_list_item    *next;
	};
	typedef struct st_api_session_pool_list_item	*XMLIMAPI_SessionPoolListItem;
	/*!-----------------------------------------------------------------------
		EndDeclaration	:	XMLIMAPI_SessionPoolListItem 
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Declaration	:	XMLIMAPI_SessionPoolList
		description	:	Simple linked list for Session Pools 
	--------------------------------------------------------------------------*/
	struct st_api_session_pool_list {
		XMLIMAPI_SessionPoolListItem		first;
		XMLIMAPI_SessionPoolListItem		last;
		XMLIMAPI_SessionPoolListItem		curr;
	};
	typedef struct st_api_session_pool_list			*XMLIMAPI_SessionPoolList;
	/*!-----------------------------------------------------------------------
		EndDeclaration	:	XMLIMAPI_SessionPoolList 
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		EndChapter		:	Session Pool and List
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Chapter		:	Error Handling
	--------------------------------------------------------------------------*/
	/*!-----------------------------------------------------------------------
		Declaration	:	Error Handling Types
		description	:	different types used in error handling functions
						(see also: API Helper Functions -> Error Handling) 
	--------------------------------------------------------------------------*/
	typedef char                               XMLIMAPI_ErrorTextString[XMLIMAPI_MAX_ERROR_TEXT_LEN+1];
	typedef char                               XMLIMAPI_ErrorStateString[XMLIMAPI_MAX_STATE_TEXT_LEN+1];
	typedef char                              *XMLIMAPI_ErrorText;
	typedef char                              *XMLIMAPI_ErrorState;
	typedef SQLINTEGER                         XMLIMAPI_ErrorCode;
	/*!-----------------------------------------------------------------------
		EndDeclaration	:	Error Handling Types 
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Declaration	:	XMLIMAPI_ErrorType
		description	:	Error Types used in error handling 
	--------------------------------------------------------------------------*/
	typedef enum en_xmlimapi_error_type {
		XMLIMAPI_ERR_TYPE_UNDEFINED,
		XMLIMAPI_ERR_TYPE_MAPI,
		XMLIMAPI_ERR_TYPE_SQL
	} XMLIMAPI_ErrorType;
	/*!-----------------------------------------------------------------------
		EndDeclaration	:	XMLIMAPI_ErrorType 
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Declaration	:	XMLIMAPI_ErrorCodeList
		description	:	Error Codes used in error handling 
	--------------------------------------------------------------------------*/
	typedef enum en_xmlimapi_error_code {
	    XMLIMAPI_ERR_MAPI_CODE_UNDEFINED,
		XMLIMAPI_ERR_MAPI_CODE_UNKNOWN_ODBC_RETURN_CODE,
		XMLIMAPI_ERR_MAPI_CODE_NO_XMLINDEX_ASSIGNED,
		XMLIMAPI_ERR_MAPI_CODE_NO_MORE_INDICES,
		XMLIMAPI_ERR_MAPI_CODE_NO_XMLINDEX,
		XMLIMAPI_ERR_MAPI_CODE_NO_DOCCLASS,
		XMLIMAPI_ERR_MAPI_CODE_NO_MORE_DOCCLASSES,
		XMLIMAPI_ERR_MAPI_CODE_COULD_NOT_GET_NEXT_UNIQUE_ID,
		XMLIMAPI_ERR_MAPI_CODE_XMLINDEX_ASSIGNED_TO_DOCCLASS,
		XMLIMAPI_ERR_MAPI_CODE_NO_SESSIONPOOL,
		XMLIMAPI_ERR_MAPI_CODE_NO_MORE_SESSIONPOOLS,
		XMLIMAPI_ERR_MAPI_CODE_NO_XIE,
		XMLIMAPI_ERR_MAPI_CODE_NO_MORE_XIES,
		XMLIMAPI_ERR_MAPI_CODE_SESSIONPOOL_USED,
		XMLIMAPI_ERR_MAPI_CODE_SESSIONPOOL_INTERNAL,
		XMLIMAPI_ERR_MAPI_CODE_NO_SERVICE,
		XMLIMAPI_ERR_MAPI_CODE_NO_MORE_SERVICES,
		XMLIMAPI_ERR_MAPI_CODE_XMLINDEX_DOES_NOT_EXIST,
		XMLIMAPI_ERR_MAPI_CODE_XMLINDEX_ALLREADY_EXISTS,
		XMLIMAPI_ERR_MAPI_CODE_DOCCLASS_ALLREADY_EXISTS,
		XMLIMAPI_ERR_MAPI_CODE_DOCCLASS_DOES_NOT_EXIST,
		XMLIMAPI_ERR_MAPI_CODE_SESSIONPOOL_DOES_NOT_EXIST,
		XMLIMAPI_ERR_MAPI_CODE_SESSIONPOOL_ALLREADY_EXISTS,
		XMLIMAPI_ERR_MAPI_CODE_INDEXINGSERVICE_DOES_NOT_EXIST,
		XMLIMAPI_ERR_MAPI_CODE_INDEXINGSERVICE_ALLREADY_EXISTS,
		XMLIMAPI_ERR_MAPI_CODE_XIE_DOES_NOT_EXIST,
		XMLIMAPI_ERR_MAPI_CODE_XIE_ALLREADY_EXISTS
	} XMLIMAPI_ErrorCodeList;
	/*!-----------------------------------------------------------------------
		EndDeclaration	:	XMLIMAPI_ErrorCodes 
	--------------------------------------------------------------------------*/
	typedef struct st_xmlimapi_error_item    *XMLIMAPI_ErrorItem;
	struct st_xmlimapi_error_item {
		XMLIMAPI_ErrorType        type;
		XMLIMAPI_ErrorCode        code;
		XMLIMAPI_ErrorTextString  text;
		XMLIMAPI_ErrorStateString state;
		XMLIMAPI_ErrorItem        nextErrorItem;
	};

	struct st_xmlimapi_error_list {
		XMLIMAPI_ErrorItem  lastErrorItem;
	};
	typedef struct st_xmlimapi_error_list  *XMLIMAPI_ErrorList;


	/*!-----------------------------------------------------------------------
		EndChapter		:	Error Handling
	--------------------------------------------------------------------------*/

	/* SQL error codes */
	#define XMLIMAPI_ERR_SQL_CODE_NO_DATA                        SQL_NO_DATA
	#define XMLIMAPI_ERR_SQL_CODE_INVALID_HANDLE                 SQL_INVALID_HANDLE
	#define XMLIMAPI_ERR_SQL_CODE_DUPLICATE_KEY                  200

	/* SQL states */
	#define XMLIMAPI_ERR_SQL_STATE_NO_ROWS_UPDATED_OR_DELETED            "01S03" /* No rows updated or deleted           */
	#define XMLIMAPI_ERR_SQL_STATE_MORE_THEN_ONE_ROW_UPDATED_OR_DELETED  "01S04" /* More then one row updated or deleted */

	/*!-----------------------------------------------------------------------
		EndChapter		:	Data Types and Structures
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Chapter		: API Functions
	--------------------------------------------------------------------------*/
	
	/*!-----------------------------------------------------------------------
		Chapter		:	API Handle
	--------------------------------------------------------------------------*/
	/*!-----------------------------------------------------------------------
		Function	:	XMLIMAPI_createHandle
		description	:	creates an API Handle thats used for storing prepared 
						statements and similar things
		arguments	:	handle [in/out]	API Handle
						henv [in] ODBC Environment Handle
						hDBC [in] ODBC Connection Handle
		return value:	XMLIMAPI_True if creation was successfull;<br>
						XMLIMAPI_False if not 
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_createHandle(XMLIMAPI_Handle *handle,
	                                      SQLHENV          henv,
	                                      SQLHDBC          hDBC);
	/*!-----------------------------------------------------------------------
		EndFunction	:	XMLIMAPI_createHandle 
	--------------------------------------------------------------------------*/
 
	/*!-----------------------------------------------------------------------
		Function	:	XMLIMAPI_destroyHandle
		description	:	destroys an API Handle
		arguments	:	handle [in]	API Handle
		return value:	XMLIMAPI_True if destruction was successfull;<br>
						XMLIMAPI_False if not 
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_destroyHandle(XMLIMAPI_Handle handle);
	/*!-----------------------------------------------------------------------
		EndFunction	:	XMLIMAPI_destroyHandle 
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		EndChapter		:	API Handle
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Chapter		:	XML Index
	--------------------------------------------------------------------------*/
	/*!-----------------------------------------------------------------------
		Function	:	XMLIMAPI_XmlIndexCreate
		description	:	Insert a record in XML Index table on database. If an error 
						occurs it is posted to the error list of the API handle
		arguments	:	handle [in]	API Handle
						xmlIndex [in]	XML Index Data 
		return value:	XMLIMAPI_True if insert was successfull;<br>
						XMLIMAPI_False if not
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XmlIndexCreate(XMLIMAPI_Handle   handle,
	                                        XMLIMAPI_XmlIndex *xmlIndex);
	/*!-----------------------------------------------------------------------
		EndFunction	:	XMLIMAPI_XmlIndexCreate 
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Function	:	XMLIMAPI_XmlIndexUpdate
		description	:	Update a record with given id in XML Index table on database.
						If an error occurs it is posted to the error list of the API handle.
		arguments	:	handle [in]	API Handle
						xmlIndexid [in]	XML Index data
		return value:	XMLIMAPI_True if update was successfull;<br>
						XMLIMAPI_False if not
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XmlIndexUpdate(XMLIMAPI_Handle   handle,
	                                        XMLIMAPI_XmlIndex *xmlIndex);
	/*!-----------------------------------------------------------------------
		EndFunction	:	XMLIMAPI_XmlIndexUpdate
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Function	:	XMLIMAPI_XmlIndexDelete
		description	:	Delete a record with given id in XML Index table on database.
						If an error occurs it is posted to the error list of the API handle.
		arguments	:	handle [in]	API handle
						id [in]	binary ID of the XML Index to delete
		return value:	XMLIMAPI_True if deletion was successfull;<br>
						XMLIMAPI_False if not
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XmlIndexDelete(XMLIMAPI_Handle handle,
	                                        XMLIMAPI_Id     id,
											XMLIMAPI_Bool   alone);
	/*!-----------------------------------------------------------------------
		EndFunction	:	XMLIMAPI_XmlIndexDelete
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Function	:	XMLIMAPI_XmlIndexCheckAssign
		description	:	Check wether or not a given XML Index is assigned to a 
						Document Class. If an error occurs it is posted to the
						error list of the API handle. If the XML Index is assigned
						to at least one Document Class the error messages contains
						up to 3 names of Document Classes the XML Index is assigned to.
		arguments	:	handle [in]	API handle
						id [in]	binary ID of the XML Index to check
		return value:	XMLIMAPI_True if XML Index is assigned to at least one 
						Document Class; <br>
						XMLIMAPI_False if XML Index is not assigned
						to any Document Class
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XmlIndexCheckAssign(XMLIMAPI_Handle handle,
	                                             XMLIMAPI_Id     id);
	/*!-----------------------------------------------------------------------
		EndFunction	:	XMLIMAPI_XmlIndexCheckAssign
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Function	:	XMLIMAPI_XmlIndexGet
		description	:	returns the data of a given XML Index. If an error occured 
						it is posted to the error list of the API handle.
		arguments	:	handle [in]	API handle
						xmlIndexId [in]	binary ID of XML Index to get
						xmlIndex [out]	container for XML Index data
		return value:	XMLIMAPI_True if getting of data was successfull;<br>
						XMLIMAPI_False if not
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XmlIndexGet(XMLIMAPI_Handle   handle,
	                                     XMLIMAPI_Id       xmlIndexId,
	                                     XMLIMAPI_XmlIndex *xmlIndex);
	/*!-----------------------------------------------------------------------
		EndFunction	:	XMLIMAPI_XmlIndexGet
	--------------------------------------------------------------------------*/
	
	/*!-----------------------------------------------------------------------
		Function	:	XMLIMAPI_XmlIndexGetFirst
		description	:	Get the first Xml Index of all existing Xml Indices. This
						function is paired with XMLIMAPI_XmlIndexGetNext. Which
						must be used to get the second, third and so on XML Index.
						If you want to retrieve all XML Indices allways start with
						this function. If an error occurs it is posted to the error
						list of the API handle.
		arguments	:	handle [in]	API handle
						xmlIndex[out]	container for XML Index data
		return value:	XMLIMAPI_True if function was successfull;<br> 
						XMLIMAPI_False if not
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XmlIndexGetFirst(XMLIMAPI_Handle   handle,
	                                          XMLIMAPI_XmlIndex *xmlIndex);
	/*!-----------------------------------------------------------------------
		EndFunction	:	XMLIMAPI_XmlIndexGetFirst
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Function	:	XMLIMAPI_XmlIndexGetNext
		description	:	Get the next XML Index of all existing XML Indices. This
						function is paired with XMLIMAPI_XmlIndexGetFirst. Which
						must be used to start the iteration. If an error occurs it
						is posted to the error list of the API handle.
		arguments	:	handle [in]	API handle
						xmlIndex [out]	container for XML Index data
		return value:	XMLIMAPI_True if function was successfull;<br>
						XMLIMAPI_False if not, especially if there is no more
						XML Index to get.
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XmlIndexGetNext(XMLIMAPI_Handle   handle,
	                                         XMLIMAPI_XmlIndex *xmlIndex);
	/*!-----------------------------------------------------------------------
		EndFunction	:	XMLIMAPI_XmlIndexGetNext
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Function	:	XMLIMAPI_XmlIndexGetFirstIncl
		description	:	Get the first Xml Index of a list of existing Xml Indices. This
						function is paired with XMLIMAPI_XmlIndexGetNextIncl. Which
						must be used to get the second, third and so on XML Index.
						If you want to retrieve all XML Indices allways start with
						this function. If an error occurs it is posted to the error
						list of the API handle.
		arguments	:	handle [in]	API handle
						xmlIndex[out]	container for XML Index data
						include [in] list of XML Index Id's
		return value:	XMLIMAPI_True if function was successfull;<br> 
						XMLIMAPI_False if not
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XmlIndexGetFirstIncl(XMLIMAPI_Handle   handle,
	                                          XMLIMAPI_XmlIndex *xmlIndex,
											  XMLIMAPI_XmlIndexList include);
	/*!-----------------------------------------------------------------------
		EndFunction	:	XMLIMAPI_XmlIndexGetFirstIncl
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Function	:	XMLIMAPI_XmlIndexGetNextIncl
		description	:	Get the next XML Index of a list of existing XML Indices. This
						function is paired with XMLIMAPI_XmlIndexGetFirstIncl. Which
						must be used to start the iteration. If an error occurs it
						is posted to the error list of the API handle.
		arguments	:	handle [in]	API handle
						xmlIndex [out]	container for XML Index data
		return value:	XMLIMAPI_True if function was successfull;<br>
						XMLIMAPI_False if not, especially if there is no more
						XML Index to get.
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XmlIndexGetNextIncl(XMLIMAPI_Handle   handle,
	                                         XMLIMAPI_XmlIndex *xmlIndex);
	/*!-----------------------------------------------------------------------
		EndFunction	:	XMLIMAPI_XmlIndexGetNextIncl
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Function	:	XMLIMAPI_XmlIndexGetFirstByDocClass
		description	:	Get the first of a list of XML Indices assigned to the given
						Document Class. This function is paired with XMLIMAPI_XmlIndexGetNextByDocClass.
						Which can be used to get the second, third and so on 
						XML Index assigned to the given Document Class. To start
						the iteration XMLIMAPI_XmlIndexGetFirstByDocClass must
						be used. If an error occurs it is posted to the error list
						of the API handle.<br>
						Attention: If there are Document Class specific settings for
						the XML Index they are returned instead of the default settings 
						entered for the XML Index.
		arguments	:	handle [in]	API handle
						docClassId [in]	binary ID of Document Class the XML Indices 
						are assigned to
						xmlIndex [out] container for XML Index data (complete version)
		return value:	XMLIMAPI_True if function was successfull;<br>
						XMLIMAPI_False if not, especially if no XML Index is
						assigned to the given Document Class
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XmlIndexGetFirstByDocClass(XMLIMAPI_Handle       handle,
	                                                    XMLIMAPI_Id           docClassId,
	                                                    XMLIMAPI_XmlIndex *xmlIndex);
	/*!-----------------------------------------------------------------------
		EndFunction	:	XMLIMAPI_XmlIndexGetFirstByDocClass
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Function	:	XMLIMAPI_XmlIndexGetNextByDocClass
		description	:	Get the next of a list of XML Indices assigned to the given
						Document Class. This function is paired with 
						XMLIMAPI_XmlIndexGetFirstByDocClass. Which must be used
						to start the iteration. If an error occurs it is posted 
						to the error list of the API handle.<br>
						Attention: If there are Document Class specific settings for
						the XML Index they are returned instead of the default settings 
						entered for the XML Index.
		arguments	:	handle [in]	API handle
						xmlIndex [out]	container for XML Index data (complete version)
		return value:	XMLIMAPI_True if function was successfull;<br>
						XMLIMAPI_False if not, especially if no more XML Indices
						are assigned to the given Document Class
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XmlIndexGetNextByDocClass(XMLIMAPI_Handle       handle,
	                                                   XMLIMAPI_XmlIndex *xmlIndex);
	/*!-----------------------------------------------------------------------
		EndFunction	:	XMLIMAPI_XmlIndexGetNextByDocClass
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Function	:	XMLIMAPI_XmlIndexGetFirstByDocClassInfoExcl
		description	:	Get the first of a list of XML Indices assigned to the given
						Document Class excluding a number of given XML Indices. 
						This function is paired with XMLIMAPI_XmlIndexGetNextByDocClassInfoExcl.
						Which can be used to get the second, third and so on 
						XML Index assigned to the given Document Class. To start
						the iteration XMLIMAPI_XmlIndexGetFirstByDocClassInfoExcl must
						be used. If an error occurs it is posted to the error list
						of the API handle.<br>
						Attention: If there are Document Class specific settings for
						the XML Index they are NOT considered! 
		arguments	:	handle [in]	API handle
						docClassId [in]	binary ID of Document Class the XML Indices 
						are assigned to
						xmlIndex [out] container for XML Index data (short version)
						xmlIndexExcl [in]	array of XML Indices to exclude from result
						exclCount [in]	number of XML Indices to exclude from result
		return value:	XMLIMAPI_True if function was successfull;<br>
						XMLIMAPI_False if not, especially if no XML Index is
						assigned to the given Document Class
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XmlIndexGetFirstByDocClassInfoExcl(XMLIMAPI_Handle   handle,
	                                                            XMLIMAPI_Id       docClassId,
	                                                            XMLIMAPI_XmlIndex *xmlIndex,
	                                                            XMLIMAPI_XmlIndexList exclude);
	/*!-----------------------------------------------------------------------
		EndFunction	:	XMLIMAPI_XmlIndexGetByDocClassInfoExcl
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Function	:	XMLIMAPI_XmlIndexGetNextByDocClassInfoExcl
		description	:	Get the next of a list of XML Indices assigned to the given
						Document Class excluding a number of XML Indices. 
						This function is paired with 
						XMLIMAPI_XmlIndexGetFirstByDocClassInfoExcl. Which must be used
						to start the iteration. If an error occurs it is posted 
						to the error list of the API handle.<br>
						Attention: If there are Document Class specific settings for
						the XML Index they are NOT considered!
		arguments	:	handle [in]	API handle
						xmlIndex [out]	container for XML Index data (short version)
		return value:	XMLIMAPI_True if function was successfull;<br>
						XMLIMAPI_False if not, especially if no more XML Indices
						are assigned to the given Document Class
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XmlIndexGetNextByDocClassInfoExcl(XMLIMAPI_Handle   handle,
	                                                           XMLIMAPI_XmlIndex *xmlIndex);
	/*!-----------------------------------------------------------------------
		EndFunction	:	XMLIMAPI_XmlIndexGetNextByDocClassInfoExcl
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Function	:	XMLIMAPI_XmlIndexGetFirstForDocClasses
		description	:	Get the first of a list of XML Indices assigned to the given
						Document Classes. 
						This function is paired with XMLIMAPI_XmlIndexGetNextForDocClasses.
						Which can be used to get the second, third and so on 
						XML Index assigned to the given Document Classes. To start
						the iteration XMLIMAPI_XmlIndexGetFirstForDocClasses must
						be used. If an error occurs it is posted to the error list
						of the API handle.<br>
		arguments	:	handle [in]	API handle
						docClasses [in]	list of document classes
						docClass [out] container for Document Class ID and name
						xmlIndex [out] container for XML Index ID and name
		return value:	XMLIMAPI_True if function was successfull;<br>
						XMLIMAPI_False if not, especially if no XML Index is
						assigned to the given Document Class
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XmlIndexGetFirstForDocClasses(XMLIMAPI_Handle		handle,
														 XMLIMAPI_DocClassList	docClasses,
														 XMLIMAPI_DocClass		*docClass,
														 XMLIMAPI_XmlIndex		*xmlIndex);
	/*!-----------------------------------------------------------------------
		EndFunction	:	XMLIMAPI_XmlIndexGetFirstForDocClasses
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Function	:	XMLIMAPI_XmlIndexGetNextForDocClasses
		description	:	Get the next of a list of XML Indices assigned to the given
						Document Classes. 
						This function is paired with 
						XMLIMAPI_XmlIndexGetFirstForDocClasses. Which must be used
						to start the iteration. If an error occurs it is posted 
						to the error list of the API handle.<br>
						Attention: If there are Document Class specific settings for
						the XML Index they are NOT considered!
		arguments	:	handle [in]	API handle
						docClass [out]  container for Document Class ID and name
						xmlIndex [out]	container for XML Index ID and name
		return value:	XMLIMAPI_True if function was successfull;<br>
						XMLIMAPI_False if not, especially if no more XML Indices
						are assigned to the given Document Class
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XmlIndexGetNextForDocClasses(XMLIMAPI_Handle	handle,
														XMLIMAPI_DocClass *docClass,
													    XMLIMAPI_XmlIndex *xmlIndex);
	/*!-----------------------------------------------------------------------
		EndFunction	:	XMLIMAPI_XmlIndexGetNextForDocClasses
	--------------------------------------------------------------------------*/


	/*!-----------------------------------------------------------------------
		EndChapter		:	XML Index
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Chapter		:	Document Class
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Function	:	XMLIMAPI_DocClassCreate
		description	:	Insert a record in Document Class table on database.
						If an error occurs it is posted to the error list of the 
						API handle.<br>
						The array must contain 5 XML Index records. But not all
						of them must be filled with data. If a record does not
						contain a XML Index ID it is assumed empty and will be
						ignored.
		arguments	:	handle [in]	API handle
						name [in]	Document Class name
						description [in]	Document Class description
						xmlIndices [in]	Array of XML Indices
		return value:	XMLIMAPI_True if creation was successfull;<br>
						XMLIMAPI_False if not
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_DocClassCreate(XMLIMAPI_Handle   handle,
	                                        XMLIMAPI_Char     *name,
	                                        XMLIMAPI_Char     *description,
	                                        XMLIMAPI_XmlIndexList assigned);
	/*!-----------------------------------------------------------------------
		EndFunction	:	XMLIMAPI_DocClassCreate
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Function	:	XMLIMAPI_DocClassAssignXmlIndex	
		description	:	Assign up to 5 XML Indices to a Document Class. <br>
						The array must contain 5 XML Index records. But not all
						of them must be filled with data. If a record does not
						contain a XML Index ID it is assumed empty and will be
						ignored.<br>
						If an error occurs it is posted to the error list of the 
						API handle.
		arguments	:	handle [in]	API handle
						docClassId [in]	binary ID of Document Class to assign
						the XML Indices to
						xmlIndices [in]	array of XML Indices
		return value:	XMLIMAPI_True if assignment was successfull;<br>
						XMLIMAPI_False if not
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_DocClassAssignXmlIndex(XMLIMAPI_Handle   handle,
	                                                XMLIMAPI_Id       docClassId,
	                                                XMLIMAPI_XmlIndexList assigned);
	/*!-----------------------------------------------------------------------
		EndFunction	:	XMLIMAPI_DocClassAssignXmlIndex
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Function	:	XMLIMAPI_DocClassUpdate
		description	:	Update a record with given id in Document Class table
						on database. If an error occurs it is postet to the
						error list of the API handle.<br>
						The array must contain 5 XML Index records. But not all
						of them must be filled with data. If a record does not
						contain a XML Index ID it is assumed empty and will be
						ignored.
		arguments	:	handle [in]	API handle
						id [in]	binary ID of Document Class to change
						description [in]	Document Class description
						xmlIndices [in]	array of XML Indices
		return value:	XMLIMAPI_True if update was successfull;<br>
						XMLIMAPI_False if not
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_DocClassUpdate(XMLIMAPI_Handle   handle,
	                                        XMLIMAPI_Id       id,
	                                        XMLIMAPI_Char     *description,
	                                        XMLIMAPI_XmlIndexList assigned);
	/*!-----------------------------------------------------------------------
		EndFunction	:	XMLIMAPI_DocClassUpdate
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Function	:	XMLIMAPI_DocClassDelete
		description	:	Delete a record with given id from Document Class table
						on database. The assignment of XML Indices to that very 
						Document Class is deleted to. If an error occurs it is
						posted to the error list of the API handle.
		arguments	:	handle [in]	API handle
						id [in]	binary ID of Document Class to delete
		return value:	XMLIMAPI_True if deletion was sucessfull;<br>
						XMLIMAPI_False if not
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_DocClassDelete(XMLIMAPI_Handle handle,
	                                        XMLIMAPI_Id     id);
	/*!-----------------------------------------------------------------------
		EndFunction	:	XMLIMAPI_DocClassUpdate
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Function	:	XMLIMAPI_DocClassGetId
		description	:	Get the binary ID of a Document Class given as Document
						Class name. If an error occurs it is posted to the error
						list of the API handle.
		arguments	:	handle [in]	API handle
						docClassName [in]	Document Class name
						docClassId [out]	binary ID of Document Class
		return value:	XMLIMAPI_True if ID was retrieved successfully;<br>
						XMLIMAPI_False if not
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_DocClassGetId(XMLIMAPI_Handle handle,
	                                       XMLIMAPI_Char   *docClassName,
	                                       XMLIMAPI_Id     *docClassId);
	/*!-----------------------------------------------------------------------
		EndFunction	:	XMLIMAPI_DocClassGetId
	--------------------------------------------------------------------------*/
	/*!-----------------------------------------------------------------------
		Function	:	XMLIMAPI_DocClassGetName
		description	:	Get the name of a Document Class given as binary Document
						Class ID. If an error occurs it is posted to the error
						list of the API handle.
		arguments	:	handle [in]	API handle
						id [in]	binary ID of Document Class
						docClassName [out]	Document Class name
		return value:	XMLIMAPI_True if ID was retrieved successfully;<br>
						XMLIMAPI_False if not
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_DocClassGetName(XMLIMAPI_Handle		handle,
										   XMLIMAPI_Id			id,
										   XMLIMAPI_Char		*docClassName);
	/*!-----------------------------------------------------------------------
		EndFunction	:	XMLIMAPI_DocClassGetName
	--------------------------------------------------------------------------*/
	/*!-----------------------------------------------------------------------
		Function	:	XMLIMAPI_DocClassGet
		description	:	returns the data of a given Document Class. If an error occured 
						it is posted to the error list of the API handle.
		arguments	:	handle [in]	API handle
						id [in]	binary ID of Document Class to get
						docClass [out]	container for Document Class data
		return value:	XMLIMAPI_True if getting of data was successfull;<br>
						XMLIMAPI_False if not
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_DocClassGet(XMLIMAPI_Handle   handle,
	                                     XMLIMAPI_Id       id,
	                                     XMLIMAPI_DocClass *docClass);
	/*!-----------------------------------------------------------------------
		EndFunction	:	XMLIMAPI_DocClassGet
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Function	:	XMLIMAPI_DocClassGetFirst
		description	:	Get the first Document Class of all existing Document
						Classes. This function is paired with XMLIMAPI_DocClassGetNext.
						Which must be used to get the second, third and so on 
						Document Class.
						If you want to retrieve all Document Classes allways start with
						this function. If an error occurs it is posted to the error
						list of the API handle.
		arguments	:	handle [in]	API handle
						docClass[out]	container for Document Class data
		return value:	XMLIMAPI_True if function was successfull;<br> 
						XMLIMAPI_False if not
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_DocClassGetFirst(XMLIMAPI_Handle   handle,
	                                          XMLIMAPI_DocClass *docClass);
	/*!-----------------------------------------------------------------------
		EndFunction	:	XMLIMAPI_DocClassGetFirst
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Function	:	XMLIMAPI_DocClassGetNext
		description	:	Get the next Document Class of all existing Document Classes. This
						function is paired with XMLIMAPI_DocClassGetFirst. Which
						must be used to start the iteration. If an error occurs it
						is posted to the error list of the API handle.
		arguments	:	handle [in]	API handle
						docClass [out]	container for Document Class data
		return value:	XMLIMAPI_True if function was successfull;<br>
						XMLIMAPI_False if not, especially if there is no more
						Document Class to get.
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_DocClassGetNext(XMLIMAPI_Handle   handle,
	                                         XMLIMAPI_DocClass *docClass);
	/*!-----------------------------------------------------------------------
		EndFunction	:	XMLIMAPI_DocClassGetNext
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Function	:	XMLIMAPI_DocClassExclGetFirst
		description	:	Get the first Document Class of all existing Document
						Classes excluding the given document classes. 
						This function is paired with XMLIMAPI_DocClassExclGetNext.
						Which must be used to get the second, third and so on 
						Document Class.
						If you want to retrieve all Document Classes allways start with
						this function. If an error occurs it is posted to the error
						list of the API handle.
		arguments	:	handle [in]	API handle
						exclude [in] list of document class ID's to exclude from the result
						docClass[out]	container for Document Class data
		return value:	XMLIMAPI_True if function was successfull;<br> 
						XMLIMAPI_False if not
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_DocClassExclGetFirst(XMLIMAPI_Handle	handle,
												XMLIMAPI_DocClassList exclude,
									  			XMLIMAPI_DocClass *docClass);
	/*!-----------------------------------------------------------------------
		EndFunction	:	XMLIMAPI_DocClassExclGetFirst
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Function	:	XMLIMAPI_DocClassExclGetNext
		description	:	Get the next Document Class of all existing Document Classes 
						excluding the given document classes. This
						function is paired with XMLIMAPI_DocClassExclGetFirst. Which
						must be used to start the iteration. If an error occurs it
						is posted to the error list of the API handle.
		arguments	:	handle [in]	API handle
						docClass [out]	container for Document Class data
		return value:	XMLIMAPI_True if function was successfull;<br>
						XMLIMAPI_False if not, especially if there is no more
						Document Class to get.
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_DocClassExclGetNext(XMLIMAPI_Handle   handle,
											   XMLIMAPI_DocClass *docClass);
	/*!-----------------------------------------------------------------------
		EndFunction	:	XMLIMAPI_DocClassExclGetNext
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Function	:	XMLIMAPI_DocClassInclGetFirst
		description	:	Get the first Document Class of all existing Document
						Classes limited to the document classes whos ID's are provided
						with the include list. 
						This function is paired with XMLIMAPI_DocClassInclGetNext.
						Which must be used to get the second, third and so on 
						Document Class.
						If you want to retrieve all Document Classes allways start with
						this function. If an error occurs it is posted to the error
						list of the API handle.
		arguments	:	handle [in]	API handle
						include [in] list of document class ID's to include into the result
						docClass[out]	container for Document Class data
		return value:	XMLIMAPI_True if function was successfull;<br> 
						XMLIMAPI_False if not
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_DocClassInclGetFirst(XMLIMAPI_Handle	handle,
												XMLIMAPI_DocClassList include,
									  			XMLIMAPI_DocClass *docClass);
	/*!-----------------------------------------------------------------------
		EndFunction	:	XMLIMAPI_DocClassInclGetFirst
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Function	:	XMLIMAPI_DocClassInclGetNext
		description	:	Get the next Document Class of all existing Document Classes 
						limited to the document classes whos ID's were provided with the
						include list. This function is paired with XMLIMAPI_DocClassExclGetFirst.
						Which must be used to start the iteration. If an error occurs it
						is posted to the error list of the API handle.
		arguments	:	handle [in]	API handle
						docClass [out]	container for Document Class data
		return value:	XMLIMAPI_True if function was successfull;<br>
						XMLIMAPI_False if not, especially if there is no more
						Document Class to get.
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_DocClassInclGetNext(XMLIMAPI_Handle   handle,
											   XMLIMAPI_DocClass *docClass);
	/*!-----------------------------------------------------------------------
		EndFunction	:	XMLIMAPI_DocClassInclGetNext
	--------------------------------------------------------------------------*/

	 /*!-----------------------------------------------------------------------
		EndChapter		:	Document Class
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Chapter		:	Session Pool
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Function	:	XMLIMAPI_SessionPoolCreate
		description	:	Insert a record in Session Pool table on database.
						If an error occurs it is posted to the error list of the 
						API handle.
		arguments	:	handle [in]	API handle
						sessionpool [in]	Session Pool data
		return value:	XMLIMAPI_True if creation was successfull;<br>
						XMLIMAPI_False if not
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_SessionPoolCreate(XMLIMAPI_Handle      handle,
	                                           XMLIMAPI_SessionPool *sessionpool);
	/*!-----------------------------------------------------------------------
		EndFunction	:	XMLIMAPI_SessionPoolCreate
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Function	:	XMLIMAPI_SessionPoolUpdate
		description	:	Update a record with given id in Session Pool table
						on database. If an error occurs it is postet to the
						error list of the API handle.
		arguments	:	handle [in]	API handle
						sessionpool [in]	binary ID of Session Pool to change
		return value:	XMLIMAPI_True if update was successfull;<br>
						XMLIMAPI_False if not
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_SessionPoolUpdate(XMLIMAPI_Handle      handle,
	                                           XMLIMAPI_SessionPool *sessionpool);
	/*!-----------------------------------------------------------------------
		EndFunction	:	XMLIMAPI_SessionPoolUpdate
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Function	:	XMLIMAPI_SessionPoolDelete
		description	:	Delete a record with given id from Session Pool table
						on database. 
		arguments	:	handle [in]	API handle
						sessionpool [in]	binary ID of Session Pool to delete
		return value:	XMLIMAPI_True if deletion was sucessfull;<br>
						XMLIMAPI_False if not
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_SessionPoolDelete(XMLIMAPI_Handle      handle,
	                                           XMLIMAPI_SessionPool *sessionpool);
	/*!-----------------------------------------------------------------------
		EndFunction	:	XMLIMAPI_SessionPoolDelete
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Function	:	XMLIMAPI_SessionPoolGet
		description	:	returns the data of a given Session Pool. If an error occured 
						it is posted to the error list of the API handle.
		arguments	:	handle [in]	API handle
						poolId [in]	binary ID of Session Pool to get
						sessionPool [out]	container for Session Pool data
		return value:	XMLIMAPI_True if getting of data was successfull;<br>
						XMLIMAPI_False if not
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_SessionPoolGet(XMLIMAPI_Handle      handle,
	                                        XMLIMAPI_Id          poolId,
	                                        XMLIMAPI_SessionPool *sessionPool);
	/*!-----------------------------------------------------------------------
		EndFunction	:	XMLIMAPI_SessionPoolGet
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Function	:	XMLIMAPI_SessionPoolGetFirst
		description	:	Get the first Session Pool of all existing Session Pools.
						This function is paired with XMLIMAPI_SessionPoolGetNext.
						Which must be used to get the second, third and so on 
						Session Pool.
						If you want to retrieve all Session Pools allways start with
						this function. If an error occurs it is posted to the error
						list of the API handle.
		arguments	:	handle [in]	API handle
						sessionPool[out]	container for Session Pool data
		return value:	XMLIMAPI_True if function was successfull;<br> 
						XMLIMAPI_False if not
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_SessionPoolGetFirst(XMLIMAPI_Handle      handle,
	                                             XMLIMAPI_SessionPool *sessionPool);
	/*!-----------------------------------------------------------------------
		EndFunction	:	XMLIMAPI_SessionPoolGetFirst
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Function	:	XMLIMAPI_SessionPoolGetNext
		description	:	Get the next Session Pool of all existing Session Pools. This
						function is paired with XMLIMAPI_SessionPoolGetFirst. Which
						must be used to start the iteration. If an error occurs it
						is posted to the error list of the API handle.
		arguments	:	handle [in]	API handle
						sessionPool [out]	container for Session Pool data
		return value:	XMLIMAPI_True if function was successfull;<br>
						XMLIMAPI_False if not, especially if there is no more
						Session Pool to get.
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_SessionPoolGetNext(XMLIMAPI_Handle      handle,
	                                            XMLIMAPI_SessionPool *sessionPool);
	/*!-----------------------------------------------------------------------
		EndFunction	:	XMLIMAPI_SessionPoolGetNext
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Function	:	XMLIMAPI_SessionPoolCheckUsage
		description	:	Check wether or not the Session Pool is used with a
						Indexing Service. If an error occurs it
						is posted to the error list of the API handle.
		arguments	:	handle [in]	API handle
						sessionpool [in]	container for Session Pool data - only PoolID must be given
		return value:	XMLIMAPI_True if Session Pool is NOT used with Indexing Services<br>
						XMLIMAPI_False if Session Pool is still in use (deletion forbidden)
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_SessionPoolCheckUsage(XMLIMAPI_Handle      handle,
	                                               XMLIMAPI_SessionPool *sessionpool);
	/*!-----------------------------------------------------------------------
		EndFunction	:	XMLIMAPI_SessionPoolCheckUsage
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Function	:	XMLIMAPI_SessionPoolGetAssignment
		description	:	Get the amount of assignments of the given Session Pool 
						to Indexing services.
		arguments	:	handle [in]	API handle
						sessionpool [in]	container for Session Pool data - only PoolID must be given
		return value:	XMLIMAPI_True if function was successfull;<br>
						XMLIMAPI_False if not.
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_SessionPoolGetAssignment(XMLIMAPI_Handle      handle,
	                                               XMLIMAPI_SessionPool *sessionpool);
	/*!-----------------------------------------------------------------------
		EndFunction	:	XMLIMAPI_SessionPoolGetAssignment
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		EndChapter		:	Session Pool
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Chapter		:	Indexing Service
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Function	:	XMLIMAPI_ServiceCreate
		description	:	Insert a record in Indexing Service table on database.
						If an error occurs it is posted to the error list of the 
						API handle.
		arguments	:	handle [in]	API handle
						service [in]	Indexing Service data
						alone [in]		if alone = true then this function commits 
										its changes otherwise not
		return value:	XMLIMAPI_True if creation was successfull;<br>
						XMLIMAPI_False if not
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_ServiceCreate(XMLIMAPI_Handle      handle,
	                                     XMLIMAPI_ServiceDesc *service,
										 XMLIMAPI_Bool		  alone);
	/*!-----------------------------------------------------------------------
		EndFunction	:	XMLIMAPI_ServiceCreate
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Function	:	XMLIMAPI_ServiceUpdate
		description	:	Update a record with given id in Indexing Service table
						on database. If an error occurs it is postet to the
						error list of the API handle.
		arguments	:	handle [in]	API handle
						service [in]	binary ID of Indexing Service to change
		return value:	XMLIMAPI_True if update was successfull;<br>
						XMLIMAPI_False if not
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_ServiceUpdate(XMLIMAPI_Handle      handle,
	                                     XMLIMAPI_ServiceDesc *service);
	/*!-----------------------------------------------------------------------
		EndFunction	:	XMLIMAPI_ServiceUpdate
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Function	:	XMLIMAPI_ServiceDelete
		description	:	Delete a record with given id from Indexing Service table
						on database. 
		arguments	:	handle [in]	API handle
						service [in]	binary ID of Indexing Service to delete
		return value:	XMLIMAPI_True if deletion was sucessfull;<br>
						XMLIMAPI_False if not
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_ServiceDelete(XMLIMAPI_Handle      handle,
	                                     XMLIMAPI_ServiceDesc *service);
	/*!-----------------------------------------------------------------------
		EndFunction	:	XMLIMAPI_ServiceDelete
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Function	:	XMLIMAPI_ServiceGetId
		description	:	returns the binary ID of a given Indexing Service. 
						If an error occured it is posted to the error list of 
						the API handle.<br>
						Attention: Callback Functions and Session Pools appear only
						as IDs! If you need a complete resolved Service Description
						use XMLIMAPI_ServiceGetIndexingService instead!
		arguments	:	handle [in]	API handle
						ServiceName [in]	name of Indexing Service to get ID for
						ServiceId [out]	binary ID of Indexing Service
		return value:	XMLIMAPI_True if getting of data was successfull;<br>
						XMLIMAPI_False if not
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_ServiceGetId(XMLIMAPI_Handle handle,
	                                    XMLIMAPI_Char   *ServiceName,
	                                    XMLIMAPI_Id     *ServiceId);
	/*!-----------------------------------------------------------------------
		EndFunction	:	XMLIMAPI_ServiceGetId
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Function	:	XMLIMAPI_ServiceGet
		description	:	returns the data of a given Indexing Service. If an error occured 
						it is posted to the error list of the API handle.<br>
						Attention: Callback Functions and Session Pools appear only
						as IDs! If you need a complete resolved Service Description
						use XMLIMAPI_ServiceGetIndexingService instead!
		arguments	:	handle [in]	API handle
						serviceId [in]	binary ID of Indexing Service to get
						service [out]	container for Indexing Service data
		return value:	XMLIMAPI_True if getting of data was successfull;<br>
						XMLIMAPI_False if not
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_ServiceGet(XMLIMAPI_Handle      handle,
	                                  XMLIMAPI_Id          serviceId,
	                                  XMLIMAPI_ServiceDesc *service);
	/*!-----------------------------------------------------------------------
		EndFunction	:	XMLIMAPI_ServiceGet
	--------------------------------------------------------------------------*/

   /*------------------------------------------------------------------------
	Function	:	XMLIMAPI_ServiceGetFirst
	Description	:	Get the first of a list of Indexing Services
	Parameters	:	
	Return		:	
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_ServiceGetFirst(XMLIMAPI_Handle		handle,
										   XMLIMAPI_ServiceDesc	*service);

	/*------------------------------------------------------------------------
	Function	:	XMLIMAPI_ServiceGetNext
	Description	:	Get the next of a list of Indexing Services
	Parameters	:	
	Return		:	
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_ServiceGetNext(XMLIMAPI_Handle		handle,
										  XMLIMAPI_ServiceDesc	*service);


	/*------------------------------------------------------------------------
	Function	:	XMLIMAPI_ServiceGetIndexingService
	Description	:	Get complete information of a Indexing Service (including
					Session Pool Descriptions, Service Library Descriptions ...)
	Parameters	:	
	Return		:	
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_ServiceGetIndexingService(XMLIMAPI_Handle			handle,
													 XMLIMAPI_Char				*serviceName,
													 XMLIMAPI_IndexingService	*service);

	/*!-----------------------------------------------------------------------
		EndChapter		:	Indexing Service
	--------------------------------------------------------------------------*/
	
	/*!-----------------------------------------------------------------------
		Chapter		:	XML IndexEngine Admin
	--------------------------------------------------------------------------*/
	/*!-----------------------------------------------------------------------
		Function	:	XMLIMAPI_XieRegister
		description	:	Registers the given XML Index Engine and its Indexing Library.
						If the XML Index Engine allready is registered, its values are 
						updated to the given ones. If an error occured 
						it is posted to the error list of the API handle.
		arguments	:	handle [in]	API handle
						engineServerNode [in]	- name of the server the xie runs on
						engineFileName [in]		- complete path of the xie on this server
						registerName [in]		- name the xie is registered with
						registerDescription [in] - short description of the registered xie 
												  (will be taken as description of the indexing
												   service, if this is not existing yet)
					    registerLogFile	[in]	- complete path of the log file the xie uses
						indexingServiceName [in]- name of the Indexing Service this xie instance 
												  is configured with
						indexingLibFileName [in] - complete path of the used indexing lib
						indexingLibVersion [in]	- version number of the used indexing lib
						indexingLibDescription [in]  - short description of the indexing lib
		return value:	XMLIMAPI_True if getting of data was successfull;<br>
						XMLIMAPI_False if not
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XieRegister(XMLIMAPI_Handle   handle,
									   XMLIMAPI_Char *engineServerNode,
									   XMLIMAPI_Char *engineFileName,
									   XMLIMAPI_Char *registerName,
									   XMLIMAPI_Char *registerDescription,
									   XMLIMAPI_Char *registerLogFile,
									   XMLIMAPI_Char *indexingServiceName,
									   XMLIMAPI_Char *indexingLibFileName,
									   XMLIMAPI_Char *indexingLibVersion,
									   XMLIMAPI_Char *indexingLibDescription);
	/*!-----------------------------------------------------------------------
		EndFunction	:	XMLIMAPI_XieRegister
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Function	:	XMLIMAPI_XieSetAliveGetRefresh
		description	:	Function for a XML Index Engine to signal that it is alive.
						By the way the Refresh flag is returned to decide whether
						the metadata has to be reloaded or not. If an error occured 
						it is posted to the error list of the API handle.
		arguments	:	handle [in]	API handle
						xiename [in]	Name of the XML Index Engine
						xienode [in]	Node the XML Index Engine runs on
						refresh [out]	Refresh flag
		return value:	XMLIMAPI_True if getting of data was successfull;<br>
						XMLIMAPI_False if not
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XieSetAliveGetRefresh(XMLIMAPI_Handle   handle,
												 XMLIMAPI_Char	   *xiename,
												 XMLIMAPI_Char	   *xienode,
												 XMLIMAPI_Bool	   *refresh);
	/*!-----------------------------------------------------------------------
		EndFunction	:	XMLIMAPI_XieSetAliveGetRefresh
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Function	:	XMLIMAPI_XieUnSetRefresh
		description	:	Function for a XML Index Engine to unset the refresh
						flag after reloading the metadata. If an error occured 
						it is posted to the error list of the API handle.
		arguments	:	handle [in]	API handle
						xiename [in]	Name of the XML Index Engine
						xienode [in]	Node the XML Index Engine runs on
		return value:	XMLIMAPI_True if the flag was unset successfull;<br>
						XMLIMAPI_False if not
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XieUnSetRefresh(XMLIMAPI_Handle   handle,
								 		   XMLIMAPI_Char	 *xiename,
										   XMLIMAPI_Char	 *xienode);
	/*!-----------------------------------------------------------------------
		EndFunction	:	XMLIMAPI_UnSetRefresh
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Function	:	XMLIMAPI_XieUnRegister
		description	:	Function to unregister a XML Index Engine. If an error occured 
						it is posted to the error list of the API handle.
		arguments	:	handle [in]	API handle
						xiename [in]	Name of the XML Index Engine
						xienode [in]	Node the XML Index Engine runs on
		return value:	XMLIMAPI_True if the deregistration was successfull;<br>
						XMLIMAPI_False if not
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XieUnRegister(XMLIMAPI_Handle   handle,
							  		     XMLIMAPI_Char *xiename,
										 XMLIMAPI_Char *xienode);
	/*!-----------------------------------------------------------------------
		EndFunction	:	XMLIMAPI_UnRegister
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Function	:	XMLIMAPI_XieAdminGet
		description	:	returns the data of a given XML Index Engine. If an error occured 
						it is posted to the error list of the API handle.
		arguments	:	handle [in]	API handle
						id [in]	binary ID of the XML Index Engine to get
						xieadmin [out]	container for XML Index Engine data
		return value:	XMLIMAPI_True if getting of data was successfull;<br>
						XMLIMAPI_False if not
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XieAdminGet(XMLIMAPI_Handle   handle,
									   XMLIMAPI_Id		xieadminId,
									   XMLIMAPI_XieAdmin *xieadmin);
	/*!-----------------------------------------------------------------------
		EndFunction	:	XMLIMAPI_XieAdminGet
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Function	:	XMLIMAPI_XieAdminGetFirst
		description	:	gets all XML Index Engines and returns the data of the 
						first XML Index Engine . If an error occured 
						it is posted to the error list of the API handle.
		arguments	:	handle [in]	API handle
						xieadmin [out]	container for XML Index Engine data
		return value:	XMLIMAPI_True if getting of data was successfull;<br>
						XMLIMAPI_False if not
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XieAdminGetFirst(XMLIMAPI_Handle   handle,
										    XMLIMAPI_XieAdmin *xieadmin);
	/*!-----------------------------------------------------------------------
		EndFunction	:	XMLIMAPI_XieAdminGetFirst
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Function	:	XMLIMAPI_XieAdminGetNext
		description	:	gets the next XML Index Engine and returns the data of it.
						This function is paired with XMLIMAPI_XieAdminGetFirst, that
						has to be called to start the iteration. If an error occured 
						it is posted to the error list of the API handle.
		arguments	:	handle [in]	API handle
						xieadmin [out]	container for XML Index Engine data
		return value:	XMLIMAPI_True if getting of data was successfull;<br>
						XMLIMAPI_False if not
	--------------------------------------------------------------------------*/
	XMLIMAPI_Bool XMLIMAPI_XieAdminGetNext(XMLIMAPI_Handle   handle,
										   XMLIMAPI_XieAdmin *xieadmin);
	/*!-----------------------------------------------------------------------
		EndFunction	:	XMLIMAPI_XieAdminGetNext
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		EndChapter		:	XML IndexEngine Admin
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		EndChapter		:	API Functions
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Chapter		:	API Helper Functions
	--------------------------------------------------------------------------*/

	/*!------------------------------------------------------------------------
		Chapter		:	Error Handling
	--------------------------------------------------------------------------*/
	/*! Function	:	XMLIMAPI_ClearErrorList
		description	:	clear errorlist of API handle
		arguments	:	handle	[in]	API Handle 
		return value:	XMLIMAPI_True if clear was successfull; 
						XMLIMAPI_False	if clear failed */
	XMLIMAPI_Bool XMLIMAPI_ClearErrorList(XMLIMAPI_Handle handle);
	/*! EndFunction	:	XMLIMAPI_ClearErrorList */

	/*! Function	:	XMLIMAPI_GetLastError
		description	:	returns the most recent error item storded in error list of API handle
		arguments	:	handle	[in]	API Handle
						errorItem [out]		most recent error item in list
		return value:	XMLIMAPI_True if delivery of error item was successfull; 
						XMLIMAPI_False	if error list is empty  */
	XMLIMAPI_Bool XMLIMAPI_GetLastError(XMLIMAPI_Handle    handle,
	                                      XMLIMAPI_ErrorItem *errorItem);
	/*! EndFunction	:	XMLIMAPI_GetLastError */

	/*! Function	:	XMLIMAPI_GetNextError
		description	:	returns the next error item storded in error list of API handle
		arguments	:	handle	[in]	API Handle
						errorItem [out]		next error item in list
		return value:	XMLIMAPI_True if delivery of error item was successfull; 
						XMLIMAPI_False	if error list is empty  */
	XMLIMAPI_Bool XMLIMAPI_GetNextError(XMLIMAPI_ErrorItem currentItem,
	                                      XMLIMAPI_ErrorItem *errorItem);
	/*! EndFunction	:	XMLIMAPI_GetNextError */

	/*! Function	:	XMLIMAPI_FindError
		description	:	lookup error item with given type and code in error list of API handle
		arguments	:	handle	[in]	API Handle
						errorType [in]		type of error (see also declaration of XMLIMAPI_ErrorType)
						errorCode [in]		code of error (see also declaration of XMLIMAPI_ErrorCode)
						errorItem [out]		error item with given type and code
		return value:	XMLIMAPI_True if error item was found; 
						XMLIMAPI_False	if error item wasn't found */
	XMLIMAPI_Bool XMLIMAPI_FindError(XMLIMAPI_Handle    handle,
	                                   XMLIMAPI_ErrorType errorType,
	                                   XMLIMAPI_ErrorCode errorCode,
	                                   XMLIMAPI_ErrorItem *errorItem);
	/*! EndFunction	:	XMLIMAPI_FindError */

	/*! Function	:	XMLIMAPI_FindError
		description	:	check wether or not the most recent error in error list is of the given
						code
		arguments	:	handle	[in]	API Handle
						errorType [in]		type of error (see also declaration of XMLIMAPI_ErrorType)
						errorCode [in]		code of error (see also declaration of XMLIMAPI_ErrorCode)
		return value:	XMLIMAPI_True if error item is of the given code; 
						XMLIMAPI_False	if not so*/
	XMLIMAPI_Bool XMLIMAPI_IsError(XMLIMAPI_Handle    handle,
	                                 XMLIMAPI_ErrorType errorType,
	                                 XMLIMAPI_ErrorCode errorCode);
	/*! EndFunction	:	XMLIMAPI_IsError */

	/*! Function	:	XMLIMAPI_IsState
		description	:	check wether or not the most recent error in error list is of the given
						state
		arguments	:	handle	[in]	API Handle
						errorType [in]		type of error (see also declaration of XMLIMAPI_ErrorType)
						errorState [in]		state of error (see also declaration of XMLIMAPI_ErrorState)
		return value:	XMLIMAPI_True if error item is of the given state; 
						XMLIMAPI_False	if not so*/
	XMLIMAPI_Bool XMLIMAPI_IsState(XMLIMAPI_Handle     handle,
	                                 XMLIMAPI_ErrorType  errorType,
	                                 XMLIMAPI_ErrorState errorState );
	/*! EndFunction	:	XMLIMAPI_IsState */

	/*! Function	:	XMLIMAPI_GetErrorType
		description	:	get the type of the given error item
		arguments	:	error [in]		error item to get the type for
						errorType [out]		type of error item (see also declaration of XMLIMAPI_ErrorType)	*/
	void  XMLIMAPI_GetErrorType(XMLIMAPI_ErrorItem error,
	                             XMLIMAPI_ErrorType *errorType );
	/*! EndFunction	:	XMLIMAPI_GetErrorType */

	/*! Function	:	XMLIMAPI_GetErrorCode
		description	:	get the code of the given error item
		arguments	:	error [in]		error item to get the type for
						errorCode [out]		code of error item (see also declaration of XMLIMAPI_ErrorCode)	*/
	void  XMLIMAPI_GetErrorCode(XMLIMAPI_ErrorItem error,
	                             XMLIMAPI_ErrorCode *errorCode );
	/*! EndFunction	:	XMLIMAPI_GetErrorCode */

	/*! Function	:	XMLIMAPI_GetErrorText
		description	:	get the error text of the given error item
		arguments	:	error [in]		error item to get the text for
						errorText [out]		text of error item 	*/
	void  XMLIMAPI_GetErrorText(XMLIMAPI_ErrorItem error,
	                             XMLIMAPI_ErrorText *errorText );
	/*! EndFunction	:	XMLIMAPI_GetErrorText */

	/*! Function	:	XMLIMAPI_GetErrorState
		description	:	get the state (odbc error code) of the given error item
		arguments	:	error [in]		error item to get the state for
						errorState [out]	state of error item 	*/
	void  XMLIMAPI_GetErrorState(XMLIMAPI_ErrorItem  error,
	                              XMLIMAPI_ErrorState *errorState );
	/*! EndFunction	:	XMLIMAPI_GetErrorState */

	/*!------------------------------------------------------------------------
		EndChapter		:	Error Handling
	--------------------------------------------------------------------------*/

	/*!-----------------------------------------------------------------------
		Chapter		:	ID Generation
	--------------------------------------------------------------------------*/
	/*! Function	:	XMLIMAPI_IdGetNext
		description	:	generates a new binary 24 byte long ID
		arguments	:	handle [in]		API handle
						id [out]	new ID
		return value:	XMLIMAPI_True if generation was successfull;
						XMLIMAPI_False if not */
	XMLIMAPI_Bool XMLIMAPI_IdGetNext(XMLIMAPI_Handle handle,
	                                   XMLIMAPI_Id     id);
	/*! EndFunction	:	XMLIMAPI_IdGetNext */

	/*! Function	:	XMLIMAPI_IdInitValue
		description	:	initializes a given ID (set value to zero)
		arguments	:	id [in/out]	ID to initialize
		return value:	*/
	void XMLIMAPI_IdInitValue(XMLIMAPI_Id id);
	/*! EndFunction	:	XMLIMAPI_IdInitValue */

	/*! Function	:	XMLIMAPI_IdStringInitValue
		description	:	initializes a given ID String to all zero
		arguments	:	idString [in/out] ID String to initialize
		return value:	*/
	void XMLIMAPI_IdStringInitValue(XMLIMAPI_IdString idString);
	/*! EndFunction	:	XMLIMAPI_IsStringInitValue */

	/*! Function	:	XMLIMAPI_IdAsString
		description	:	converting a binary ID to an ID String
		arguments	:	id [in]	binary ID to convert
						idString [out]	conversion of the binary ID
		return value:	*/
	void XMLIMAPI_IdAsString(XMLIMAPI_Id       id,
	                          XMLIMAPI_IdString idString);
	/*! EndFunction	:	XMLIMAPI_IdAsString*/

	/*! Function	:	XMLIMAPI_IdAsDebugString
		description	:	???
		arguments	:	id [in]	binary ID
						idDebugString [out]	ID as debug string
		return value:	*/
	void XMLIMAPI_IdAsDebugString(XMLIMAPI_Id            id,
	                               XMLIMAPI_IdDebugString idDebugString);
	/*! EndFunction	:	XMLIMAPI_IdAsDebugString */

	/*! Function	:	XMLIMAPI_IdStringAsId
		description	:	converts an ID String into a binary ID
		arguments	:	idString [in]	ID String to convert
						id [out]	conversion of the ID String
		return value:	*/
	void XMLIMAPI_IdStringAsId(XMLIMAPI_IdString idString,
	                            XMLIMAPI_Id       id);
	/*! EndFunction	:	XMLIMAPI_IdStringAsId */

	/*! Function	:	XMLIMAPI_IdIsInitialValue
		description	:	check wether or not a binary ID is initial 
		arguments	:	id [in]	binary ID to check
		return value:	XMLIMAPI_True if id is initial; 
						XMLIMAPI_False if not */
	XMLIMAPI_Bool XMLIMAPI_IdIsInitialValue(XMLIMAPI_Id id);
	/*! EndFunction	:	XMLIMAPI_IsIsInitialValue */

	/*! Function	:	XMLIMAPI_IdStringIsInitialValue
		description	:	check wether or not an ID String is initial
		arguments	:	idString [in]	ID String to check
		return value:	XMLIMAPI_True if value of ID String is initial;
						XMLIMAPI_False if not */
	XMLIMAPI_Bool XMLIMAPI_IdStringIsInitialValue(XMLIMAPI_IdString idString);
	/*! EndFunction	:	XMLIMAPI_IdStringIsInitialValue */

	/*! Function	:	XMLIMAPI_IdsAreIdentical
		description	:	compares two given binary IDs
		arguments	:	id1,id2 [in]	binary IDs to compare
		return value:	XMLIMAPI_True if the id's have identical values;
						XMLIMAPI_False if not */
	XMLIMAPI_Bool XMLIMAPI_IdsAreIdentical(XMLIMAPI_Id id1,
	                                         XMLIMAPI_Id id2);
	/*! EndFunction	:	XMLIMAPI_IdsAreIdentical */

	/*! Function	:	XMLIMAPI_IdStringsAreIdentical
		description	:	compares two given ID Strings
		arguments	:	idString1,idString2 [in]	ID Strings to compare
		return value:	XMLIMAPI_True if the id's have identical values;
						XMLIMAPI_False if not */
	XMLIMAPI_Bool XMLIMAPI_IdStringssAreIdentical(XMLIMAPI_IdString idString1,
	                                                XMLIMAPI_IdString idString2);
	/*! EndFunction	:	XMLIMAPI_IdStringsAreIdentical */
	/*!-----------------------------------------------------------------------
		EndChapter		:	ID Generation
	--------------------------------------------------------------------------*/

	/*!------------------------------------------------------------------------
		Chapter		:	XML IndexEngine List
	--------------------------------------------------------------------------*/
	/*! Function	:	XMLIMAPI_XieAdminListCreate
		description	:	create an empty simple linked list for XML IndexEngine
						Admin records
		arguments	:	list [in]	pointer to a XieAdmin list variable
		return value:	XMLIMAPI_True if creation was successfull; 
						XMLIMAPI_False	if creation failed */
	XMLIMAPI_Bool XMLIMAPI_XieAdminListCreate(XMLIMAPI_XieAdminList *list);
	/*! EndFunction	:	XMLIMAPI_XieAdminListCreate */

	/*! Function	:	XMLIMAPI_XieAdminListDestroy
		description	:	destroys a XieAdmin list (incl. freeing all allocated memory)
		arguments	:	list [in]	XieAdmin list
		return value:	XMLIMAPI_True if destruction was successfull; 
						XMLIMAPI_False	if destruction failed */
	XMLIMAPI_Bool XMLIMAPI_XieAdminListDestroy(XMLIMAPI_XieAdminList list);
	/*! EndFunction	:	XMLIMAPI_XieAdminListDestroy */

	/*! Function	:	XMLIMAPI_XieAdminListInit
		description	:	initializes a XieAdmin list (incl. freeing all allocated memory, if list was not empty)
		arguments	:	list [in]	XieAdmin list
		return value:	XMLIMAPI_True if initialization was successfull; 
						XMLIMAPI_False	if initialization failed */
	XMLIMAPI_Bool XMLIMAPI_XieAdminListInit(XMLIMAPI_XieAdminList list);
	/*! EndFunction	:	XMLIMAPI_XieAdminListInit */

	/*! Function	:	XMLIMAPI_XieAdminListAddItem
		description	:	append a XieAdmin to the list
		arguments	:	list [in]	XieAdmin list
						xmlIndex [in]	XieAdmin record to add to the list
		return value:	XMLIMAPI_True if addition was successfull; 
						XMLIMAPI_False	if addition failed */
	XMLIMAPI_Bool XMLIMAPI_XieAdminListAddItem(XMLIMAPI_XieAdminList list, 
	                                             XMLIMAPI_XieAdmin     *XieAdmin);
	/*! EndFunction	:	XMLIMAPI_XieAdminListAddItem */

	/*! Function	:	XMLIMAPI_XieAdminListGetFirst
		description	:	returns the first XieAdmin record in XieAdmin list
		arguments	:	list [in]	XieAdmin list
						XieAdmin [out]	first XieAdmin record in list
		return value:	XMLIMAPI_True if function was successfull; 
						XMLIMAPI_False	if function failed */
	XMLIMAPI_Bool XMLIMAPI_XieAdminListGetFirst(XMLIMAPI_XieAdminList list, 
	                                              XMLIMAPI_XieAdmin     **XieAdmin);
	/*! EndFunction	:	XMLIMAPI_XieAdminListGetFirst */

	/*! Function	:	XMLIMAPI_XieAdminListGetNext
		description	:	returns the next XieAdmin record in list
						(Do not call XMLIMAPI_XieAdminListGetNext without calling
						XMLIMAPI_XieAdminListGetFirst first!)
		arguments	:	list [in]	Xie Admin list
						XieAdmin [out]	next XieAdmin record in list
		return value:	XMLIMAPI_True if function was successfull; 
						XMLIMAPI_False	if function failed */
	XMLIMAPI_Bool XMLIMAPI_XieAdminListGetNext(XMLIMAPI_XieAdminList	list, 
	                                             XMLIMAPI_XieAdmin     **XieAdmin);
	/*! EndFunction	:	XMLIMAPI_XieAdminGetNext */

	/*!------------------------------------------------------------------------
		EndChapter		:	XML IndexEngine List
	--------------------------------------------------------------------------*/
	/*!------------------------------------------------------------------------
		Chapter		:	XML Index List
	--------------------------------------------------------------------------*/
	/*! Function	:	XMLIMAPI_XmlIndexListCreate
		description	:	create an empty simple linked list for XML Indices
		arguments	:	list [in]	pointer to a XML Index list variable
		return value:	XMLIMAPI_True if creation was successfull; 
						XMLIMAPI_False	if creation failed */
	XMLIMAPI_Bool XMLIMAPI_XmlIndexListCreate(XMLIMAPI_XmlIndexList *list);
	/*! EndFunction	:	XMLIMAPI_XmlIndexListCreate */

	/*! Function	:	XMLIMAPI_XmlIndexListDestroy
		description	:	destroys a XML Index list (incl. freeing all allocated memory)
		arguments	:	list [in]	XML Index list
		return value:	XMLIMAPI_True if destruction was successfull; 
						XMLIMAPI_False	if destruction failed */
	XMLIMAPI_Bool XMLIMAPI_XmlIndexListDestroy(XMLIMAPI_XmlIndexList list);
	/*! EndFunction	:	XMLIMAPI_XmlIndexListDestroy */

	/*! Function	:	XMLIMAPI_XmlIndexListInit
		description	:	initializes a XML Index list (incl. freeing all allocated memory, if list was not empty)
		arguments	:	list [in]	XML Index list
		return value:	XMLIMAPI_True if initialization was successfull; 
						XMLIMAPI_False	if initialization failed */
	XMLIMAPI_Bool XMLIMAPI_XmlIndexListInit(XMLIMAPI_XmlIndexList list);
	/*! EndFunction	:	XMLIMAPI_XmlIndexListInit */

	/*! Function	:	XMLIMAPI_XmlIndexListAddItem
		description	:	append a XML Index to the list
		arguments	:	list [in]	XML Index list
						xmlIndex [in]	XML Index to add to the list
		return value:	XMLIMAPI_True if addition was successfull; 
						XMLIMAPI_False	if addition failed */
	XMLIMAPI_Bool XMLIMAPI_XmlIndexListAddItem(XMLIMAPI_XmlIndexList list, 
	                                             XMLIMAPI_XmlIndex     *xmlIndex);
	/*! EndFunction	:	XMLIMAPI_XmlIndexListAddItem */

	/*! Function	:	XMLIMAPI_XmlIndexListGetFirst
		description	:	returns the first XML Index in XML Index list
		arguments	:	list [in]	XML Index list
						xmlIndex [out]	first XML Index in list
		return value:	XMLIMAPI_True if function was successfull; 
						XMLIMAPI_False	if function failed */
	XMLIMAPI_Bool XMLIMAPI_XmlIndexListGetFirst(XMLIMAPI_XmlIndexList list, 
	                                              XMLIMAPI_XmlIndex     **xmlIndex);
	/*! EndFunction	:	XMLIMAPI_XmlIndexListGetFirst */

	/*! Function	:	XMLIMAPI_XmlIndexListGetNext
		description	:	returns the next XML Index in list
						(Do not call XMLIMAPI_XmlIndexListGetNext without calling
						XMLIMAPI_XmlIndexListGetFirst first!)
		arguments	:	list [in]	XML Index list
						xmlIndex [out]	next XML Index in list
		return value:	XMLIMAPI_True if function was successfull; 
						XMLIMAPI_False	if function failed */
	XMLIMAPI_Bool XMLIMAPI_XmlIndexListGetNext(XMLIMAPI_XmlIndexList	list, 
	                                             XMLIMAPI_XmlIndex     **xmlIndex);
	/*! EndFunction	:	XMLIMAPI_XmlIndexGetNext */

	/*! Function	:	XMLIMAPI_XmlIndexListGetFirstExcl
		description	:	returns the first XML Index in XML Index list, the list does not 
						contain the XML Indexes given in the exclude list
		arguments	:	list [in]	XML Index list
						xmlIndex [out]	first XML Index in list
						exclstring [in] list of XML Index Id's to exclude from the list
		return value:	XMLIMAPI_True if function was successfull; 
						XMLIMAPI_False	if function failed */
	XMLIMAPI_Bool XMLIMAPI_XmlIndexListGetFirstExcl(XMLIMAPI_XmlIndexList list, 
	                                                XMLIMAPI_XmlIndex     **xmlIndex,
													XMLIMAPI_XmlIndexList *exclude);
	/*! EndFunction	:	XMLIMAPI_XmlIndexListGetFirstExcl */

	/*! Function	:	XMLIMAPI_XmlIndexListGetNextExcl
		description	:	returns the next XML Index in list
						(Do not call XMLIMAPI_XmlIndexListGetNextExcl without calling
						XMLIMAPI_XmlIndexListGetFirstExcl first!)
		arguments	:	list [in]	XML Index list
						xmlIndex [out]	next XML Index in list
		return value:	XMLIMAPI_True if function was successfull; 
						XMLIMAPI_False	if function failed */
	XMLIMAPI_Bool XMLIMAPI_XmlIndexListGetNextExcl(XMLIMAPI_XmlIndexList	list, 
	                                             XMLIMAPI_XmlIndex     **xmlIndex);
	/*! EndFunction	:	XMLIMAPI_XmlIndexGetNextExcl */

	/*! Function	:	XMLIMAPI_XmlIndexListGetLength
		description	:	returns the length of the XML Index list
		arguments	:	list [in]	XML Index list
		return value:	the count of elements of the given list; 
						-1 in case of an error */
	XMLIMAPI_UInt4 XMLIMAPI_XmlIndexListGetLength(XMLIMAPI_XmlIndexList		list);
	/*! EndFunction	:	XMLIMAPI_XmlIndexGetLength */

	/*!------------------------------------------------------------------------
		EndChapter		:	XML Index List
	--------------------------------------------------------------------------*/
	/*!------------------------------------------------------------------------
		Chapter		:	Document Class List
	--------------------------------------------------------------------------*/
	/*! Function	:	XMLIMAPI_DocClassListCreate
		description	:	create an empty simple linked list for XML Indices
		arguments	:	list [in]	pointer to a Document Class list variable
		return value:	XMLIMAPI_True if creation was successfull; 
						XMLIMAPI_False	if creation failed */
	XMLIMAPI_Bool XMLIMAPI_DocClassListCreate(XMLIMAPI_DocClassList *list);
	/*! EndFunction	:	XMLIMAPI_DocClassListCreate */

	/*! Function	:	XMLIMAPI_DocClassListDestroy
		description	:	destroys a Document Class list (incl. freeing all allocated memory)
		arguments	:	list [in]	Document Class list
		return value:	XMLIMAPI_True if destruction was successfull; 
						XMLIMAPI_False	if destruction failed */
	XMLIMAPI_Bool XMLIMAPI_DocClassListDestroy(XMLIMAPI_DocClassList list);
	/*! EndFunction	:	XMLIMAPI_DocClassListDestroy */

	/*! Function	:	XMLIMAPI_DocClassListInit
		description	:	initializes a Document Class list (incl. freeing all allocated memory, if list was not empty)
		arguments	:	list [in]	Document Class list
		return value:	XMLIMAPI_True if initialization was successfull; 
						XMLIMAPI_False	if initialization failed */
	XMLIMAPI_Bool XMLIMAPI_DocClassListInit(XMLIMAPI_DocClassList list);
	/*! EndFunction	:	XMLIMAPI_DocClassListInit */

	/*! Function	:	XMLIMAPI_DocClassListAddItem
		description	:	append a Document Class to the list
		arguments	:	list [in]	Document Class list
						docClass [in]	Document Class to add to the list
		return value:	XMLIMAPI_True if addition was successfull; 
						XMLIMAPI_False	if addition failed */
	XMLIMAPI_Bool XMLIMAPI_DocClassListAddItem(XMLIMAPI_DocClassList list, 
	                                             XMLIMAPI_DocClass     *docClass);
	/*! EndFunction	:	XMLIMAPI_DocClassListAddItem */

	/*! Function	:	XMLIMAPI_DocClassListGetFirst
		description	:	returns the first Document Class in Document Class list
		arguments	:	list [in]	Document Class list
						docClass [out]	first Document Class in list
		return value:	XMLIMAPI_True if function was successfull; 
						XMLIMAPI_False	if function failed */
	XMLIMAPI_Bool XMLIMAPI_DocClassListGetFirst(XMLIMAPI_DocClassList list, 
	                                              XMLIMAPI_DocClass     **docClass);
	/*! EndFunction	:	XMLIMAPI_DocClassListGetFirst */

	/*! Function	:	XMLIMAPI_DocClassListGetNext
		description	:	returns the next Document Class in list
						(Do not call XMLIMAPI_DocClassListGetNext without calling
						XMLIMAPI_DocClassListGetFirst first!)
		arguments	:	list [in]	Document Class list
						docClass [out]	next Document Class in list
		return value:	XMLIMAPI_True if function was successfull; 
						XMLIMAPI_False	if function failed */
	XMLIMAPI_Bool XMLIMAPI_DocClassListGetNext(XMLIMAPI_DocClassList	list, 
	                                             XMLIMAPI_DocClass     **docClass);
	/*! EndFunction	:	XMLIMAPI_DocClassGetNext */

	/*!------------------------------------------------------------------------
		EndChapter		:	Document Class List
	--------------------------------------------------------------------------*/
	/*!-----------------------------------------------------------------------
		Chapter		:	Session Pool List
	--------------------------------------------------------------------------*/
	/*! Function	:	XMLIMAPI_SessionPoolListCreate
		description	:	create an empty simple linked list for XML Indices
		arguments	:	list [in]	pointer to a Session Pool list variable
		return value:	XMLIMAPI_True if creation was successfull; 
						XMLIMAPI_False	if creation failed */
	XMLIMAPI_Bool XMLIMAPI_SessionPoolListCreate(XMLIMAPI_SessionPoolList *list);
	/*! EndFunction	:	XMLIMAPI_SessionPoolListCreate */

	/*! Function	:	XMLIMAPI_SessionPoolListDestroy
		description	:	destroys a Session Pool list (incl. freeing all allocated memory)
		arguments	:	list [in]	Session Pool list
		return value:	XMLIMAPI_True if destruction was successfull; 
						XMLIMAPI_False	if destruction failed */
	XMLIMAPI_Bool XMLIMAPI_SessionPoolListDestroy(XMLIMAPI_SessionPoolList list);
	/*! EndFunction	:	XMLIMAPI_SessionPoolListDestroy */

	/*! Function	:	XMLIMAPI_SessionPoolListInit
		description	:	initializes a Session Pool list (incl. freeing all allocated memory, if list was not empty)
		arguments	:	list [in]	Session Pool list
		return value:	XMLIMAPI_True if initialization was successfull; 
						XMLIMAPI_False	if initialization failed */
	XMLIMAPI_Bool XMLIMAPI_SessionPoolListInit(XMLIMAPI_SessionPoolList list);
	/*! EndFunction	:	XMLIMAPI_SessionPoolListInit */

	/*! Function	:	XMLIMAPI_SessionPoolListAddItem
		description	:	append a Session Pool to the list
		arguments	:	list [in]	Session Pool list
						sessionPool [in]	Session Pool to add to the list
		return value:	XMLIMAPI_True if addition was successfull; 
						XMLIMAPI_False	if addition failed */
	XMLIMAPI_Bool XMLIMAPI_SessionPoolListAddItem(XMLIMAPI_SessionPoolList list, 
	                                                XMLIMAPI_SessionPool     *sessionPool);
	/*! EndFunction	:	XMLIMAPI_SessionPoolListAddItem */

	/*! Function	:	XMLIMAPI_SessionPoolListGetFirst
		description	:	returns the first Session Pool in Session Pool list
		arguments	:	list [in]	Session Pool list
						sessionPool [out]	first Session Pool in list
		return value:	XMLIMAPI_True if function was successfull; 
						XMLIMAPI_False	if function failed */
	XMLIMAPI_Bool XMLIMAPI_SessionPoolListGetFirst(XMLIMAPI_SessionPoolList list, 
	                                                 XMLIMAPI_SessionPool     **sessionPool);
	/*! EndFunction	:	XMLIMAPI_SessionPoolListGetFirst */

	/*! Function	:	XMLIMAPI_SessionPoolListGetNext
		description	:	returns the next Session Pool in list
						(Do not call XMLIMAPI_SessionPoolListGetNext without calling
						XMLIMAPI_SessionPoolListGetFirst first!)
		arguments	:	list [in]	Session Pool list
						sessionPool [out]	next Session Pool in list
		return value:	XMLIMAPI_True if function was successfull; 
						XMLIMAPI_False	if function failed */
	XMLIMAPI_Bool XMLIMAPI_SessionPoolListGetNext(XMLIMAPI_SessionPoolList list, 
                                                    XMLIMAPI_SessionPool     **sessionPool);
	/*! EndFunction	:	XMLIMAPI_SessionPoolGetNext */
	/*!-----------------------------------------------------------------------
		EndChapter		:	Session Pool List
	--------------------------------------------------------------------------*/
	/*!-----------------------------------------------------------------------
		Chapter		:	Service Description List
	--------------------------------------------------------------------------*/
	/*! Function	:	XMLIMAPI_ServiceDescListCreate
		description	:	create an empty simple linked list for XML Indices
		arguments	:	list [in]	pointer to a Service Description list variable
		return value:	XMLIMAPI_True if creation was successfull; 
						XMLIMAPI_False	if creation failed */
	XMLIMAPI_Bool XMLIMAPI_ServiceDescListCreate(XMLIMAPI_ServiceDescList *list);
	/*! EndFunction	:	XMLIMAPI_ServiceDescListCreate */

	/*! Function	:	XMLIMAPI_ServiceDescListDestroy
		description	:	destroys a Service Description list (incl. freeing all allocated memory)
		arguments	:	list [in]	Service Description list
		return value:	XMLIMAPI_True if destruction was successfull; 
						XMLIMAPI_False	if destruction failed */
	XMLIMAPI_Bool XMLIMAPI_ServiceDescListDestroy(XMLIMAPI_ServiceDescList list);
	/*! EndFunction	:	XMLIMAPI_ServiceDescListDestroy */

	/*! Function	:	XMLIMAPI_ServiceDescListInit
		description	:	initializes a Service Description list (incl. freeing all allocated memory, if list was not empty)
		arguments	:	list [in]	Service Description list
		return value:	XMLIMAPI_True if initialization was successfull; 
						XMLIMAPI_False	if initialization failed */
	XMLIMAPI_Bool XMLIMAPI_ServiceDescListInit(XMLIMAPI_ServiceDescList list);
	/*! EndFunction	:	XMLIMAPI_ServiceDescListInit */

	/*! Function	:	XMLIMAPI_ServiceDescListAddItem
		description	:	append a Service Description to the list
		arguments	:	list [in]	Service Description list
						service [in]	Service Description to add to the list
		return value:	XMLIMAPI_True if addition was successfull; 
						XMLIMAPI_False	if addition failed */
	XMLIMAPI_Bool XMLIMAPI_ServiceDescListAddItem(XMLIMAPI_ServiceDescList list, 
	                                                XMLIMAPI_ServiceDesc     *service);
	/*! EndFunction	:	XMLIMAPI_ServiceDescListAddItem */

	/*! Function	:	XMLIMAPI_ServiceDescListGetFirst
		description	:	returns the first Service Description in Service Description list
		arguments	:	list [in]	Service Description list
						service [out]	first Service Description in list
		return value:	XMLIMAPI_True if function was successfull; 
						XMLIMAPI_False	if function failed */
	XMLIMAPI_Bool XMLIMAPI_ServiceDescListGetFirst(XMLIMAPI_ServiceDescList list, 
	                                                 XMLIMAPI_ServiceDesc     **service);
	/*! EndFunction	:	XMLIMAPI_ServiceDescListGetFirst */

	/*! Function	:	XMLIMAPI_ServiceDescListGetNext
		description	:	returns the next Service Description in list
						(Do not call XMLIMAPI_ServiceDescListGetNext without calling
						XMLIMAPI_ServiceDescListGetFirst first!)
		arguments	:	list [in]	Service Description list
						service [out]	next Service Description in list
		return value:	XMLIMAPI_True if function was successfull; 
						XMLIMAPI_False	if function failed */
	XMLIMAPI_Bool XMLIMAPI_ServiceDescListGetNext(XMLIMAPI_ServiceDescList list, 
	                                                XMLIMAPI_ServiceDesc     **service);
	/*! EndFunction	:	XMLIMAPI_ServiceDescGetNext */
	/*!-----------------------------------------------------------------------
		EndChapter		:	Service Description List
	--------------------------------------------------------------------------*/
	/*!------------------------------------------------------------------------
		EndChapter		:	API Helper Functions
	--------------------------------------------------------------------------*/

#endif