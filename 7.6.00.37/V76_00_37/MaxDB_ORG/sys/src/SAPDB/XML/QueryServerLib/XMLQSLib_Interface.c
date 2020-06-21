/****************************************************************************

  module      : XMLQSLib_Interface.c

  -------------------------------------------------------------------------

  responsible : Daniel Kirmse

  special area: 
  description : 


  last changed: 2001-06-20  15:53
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
	/*------------------------------------------------------------------------
		Includes
	--------------------------------------------------------------------------*/
	#include "SAPDB/XML/QueryServerLib/XMLQSLib_Definition.h"
	#include "SAPDB/XML/QueryServerLib/XMLQSLib_Interface.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_Types.h"


	/*------------------------------------------------------------------------
		FreeSearch (providing a where clause for the select)
	--------------------------------------------------------------------------*/
	SAPDB_Bool XMLQSLIB_FreeSearchEmbedded(XMLQSLib_Handle			qsHandle,
										   XMLQSLib_WhereClause		whereClause, 
										   XMLError_Error			hError)
	{
		SQLHDBC					__hdbc;
		SQLHENV					__henv;
		XMLLib_Library			__hLibrary;
		XMLLib_Function			__retrieveDocs;
		XMLLib_Function			__getRetrievedDocs;
		XMLQSLIB_Bool			__rc;

		if (!whereClause) {
			/* add appropriate error message */
			/* ... */
			return XMLQSLIB_False;
		}

		/* get all document classes from the where clause */
		/* --> needed, once the document classes are assigned to indexing services explicitly */

		/* get the indexing services these document classes are assigned to */
		/* --> needed, once the document classes are assigned to indexing services explicitly */

		/* get a list of the document stores (connection parameters) of these indexing services */
		/* --> needed, once the document classes are assigned to indexing services explicitly */

		/* perform the query on all of these connections and collect the results in hResult */
		/* --> needed, once the document classes are assigned to indexing services explicitly */

		/* BY NOW: Search is limited to the all-in-one-database model. all indexing services that have
		           a document and or index store that is not equal with the webdav store are not 
				   included in this search!!!
		*/
        /* if not yet loaded load search lib */
		if (qsHandle->hLibrary == NULL) {
			if (!Library_Load(&qsHandle->hLibrary,qsHandle->libraryName,"SAPDBXML_Init","SAPDBXML_Exit",hError)) {
				return XMLQSLIB_False;
			}
			/* get the search function of the lib */
			if (!Library_GetFunction(qsHandle->hLibrary,"SAPDBXML_RetrieveDocuments",&qsHandle->retrieveDocs,hError)) {
				return XMLQSLIB_False;
			}
			if (!Library_GetFunction(qsHandle->hLibrary,"SAPDBXML_GetRetrievedDocument",&qsHandle->getRetrievedDocs,hError)) {
				return XMLQSLIB_False;
			}
		}
		/* call the search function of the search lib */
		(*qsHandle->retrieveDocs)(qsHandle->hdbc,qsHandle->henv,whereClause,&qsHandle->userData,&qsHandle->funcDestroyUserData,hError);
		if (hError->code != WDVXIS_ERR_CODE_UNDEFINDE && hError->code != 0) {
			return XMLQSLIB_False;
		}

		/* return hResult */
		return XMLQSLIB_True;
	}

	/*------------------------------------------------------------------------
		FreeSearch Get (providing a where clause for the select)
	--------------------------------------------------------------------------*/
	SAPDB_Bool XMLQSLIB_FreeSearchEmbeddedGetDocs(XMLQSLib_Handle		qsHandle,
												 XMLQSLIB_Char			*docUri,
												 SAPDB_UInt				len,
												 XMLError_Error			hError)
	{
		SQLHDBC					__hdbc;
		SQLHENV					__henv;
		XMLLib_Library			__hLibrary;
		XMLLib_Function			__retrieveDocs;
		XMLLib_Function			__getRetrievedDocs;

		/* get a document */
		(*qsHandle->getRetrievedDocs)(qsHandle->hdbc,qsHandle->henv,qsHandle->userData,docUri,len,hError);
		if (hError->code != WDVXIS_ERR_CODE_UNDEFINDE && hError->code != 0) {
			return XMLQSLIB_False;
		}
		return XMLQSLIB_True;
	}

	XMLQSLIB_Bool XMLQSLib_CreateHandle(XMLQSLib_Handle *handle, SQLHENV henv, SQLHDBC hDBC,XMLQSLIB_Char *libname)
	{
		XMLQSLIB_Bool	ok = XMLQSLIB_False;
		XMLQSLib_Handle	newHandle = NULL;

		if ( !handle ) {
			return XMLQSLIB_False;
		}
    
		sqlallocat( sizeof(struct st_xmlqslib_handle), (XMLQSLIB_UInt1**) &newHandle, &ok );
		if (ok == XMLQSLIB_True) {

			/* call the SearchLibs Handle creator */
			newHandle->hLibrary = NULL;
			newHandle->getRetrievedDocs = NULL;
			newHandle->retrieveDocs = NULL;
			newHandle->hdbc = hDBC;
			newHandle->henv = henv;
			newHandle->userData = NULL;
			newHandle->funcDestroyUserData = NULL;
			strcpy(newHandle->libraryName,libname);

			*handle = newHandle;

			return XMLQSLIB_True;
		} else {
			*handle = NULL;

			return XMLQSLIB_False;
		}

	}

	XMLQSLIB_Bool XMLQSLib_DestroyHandle(XMLQSLib_Handle handle)
	{
		if ( handle == NULL ) {
			return XMLQSLIB_False;
		}

		/* destroy search user data */
		if (handle->userData != NULL)
			(*handle->funcDestroyUserData)(handle->userData);

		/* unload library */
		if (handle->hLibrary != NULL)
			Library_Unload(handle->hLibrary);


		sqlfree((XMLIMAPI_UInt1*)handle);
		return XMLIMAPI_True;
	}

