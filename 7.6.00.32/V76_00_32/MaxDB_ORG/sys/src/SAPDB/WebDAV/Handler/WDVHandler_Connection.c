/****************************************************************************

  module      : WDVHandler_Connection.c

  -------------------------------------------------------------------------

  responsible : Daniel Kirmse

  special area: 
  description : 


  last changed: 2001-03-01  9:35
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

	#include "SAPDB/WebDAV/Handler/WDVHandler_Connection.h"
	#include "SAPDB/WebDAV/Handler/WDVHandler_CommonUtils.h" 


	/*------------------------------------------------------------------------
		Function:	getConnection
	--------------------------------------------------------------------------*/
	sapdbwa_DbConnectionP getConnection(sapdbwa_Handle wa)
	{

		sapdbwa_SessionPoolP	_pSessionPool;
		sapdbwa_DbConnectionP   _pConnection;
		SQLHDBC					odbchandle;
		SQLHENV					henvhandle;
		void					*userData = NULL;
		WDVCAPI_WDV				wdv;
		WDVH_Handle				wdvh;
		XMLIMAPI_Handle			xml;
		WDVH_Char				msg[1024];
		WDVH_Char				value[513];

		if (wa==NULL)
			return NULL;

		/* get session pool from WebAgent */
		#if defined(LOG_LEVEL_1) 
			sapdbwa_WriteLogMsg(wa, "[WDVHandler_Connection.c] Try getting session pool ...\n");
		#endif
		_pSessionPool = sapdbwa_GetSessionPool(wa);
		if (!_pSessionPool) {
			return NULL;
		} 
		#if defined(LOG_LEVEL_1) 
		else {
			sapdbwa_WriteLogMsg(wa, "[WDVHandler_Connection.c] Got session pool succesfully\n");
		}
		#endif

		/* now use this Pool for a DB Connection */
		#if defined(LOG_LEVEL_1) 
			sapdbwa_WriteLogMsg(wa, "[WDVHandler_Connection.c] Try DB connect ...\n");
		#endif
		_pConnection = sapdbwa_DbConnect(_pSessionPool);
		if (!_pConnection) {
			return NULL;
			#if defined(LOG_LEVEL_1) 
				sapdbwa_WriteLogMsg(wa, "[WDVHandler_Connection.c] Try DB connect failed.\n");
			#endif
		} 
		#if defined(LOG_LEVEL_1) 
		else {
			sapdbwa_WriteLogMsg(wa, "[WDVHandler_Connection.c] DB connect successfull\n");
/*			sp77sprintf(msg,1024,"Sessions used in Pool: %d\n",_pSessionPool->sessionPool->usedConnections);
			sapdbwa_WriteLogMsg(wa, msg);
*/		}
		#endif

		/* if there is no user data assigned to this connection yet -> attach one */
		userData = sapdbwa_GetUserData(_pConnection);
		if (userData == NULL) {
			createConnUserData((WDVH_ConnUserData*)(&userData));
			/* set compress flag from registry */
			if (sapdbwa_GetParam(wa,"compress",msg,10))
				if (strncmp(msg,"1",1)==0)
					((WDVH_ConnUserData)userData)->compress = WDVH_True;
				else
					((WDVH_ConnUserData)userData)->compress = WDVH_False;
			else
				((WDVH_ConnUserData)userData)->compress = WDVH_False;
			/* create WDVH Handle */
			createWdvhHandle(wa,_pConnection,&wdvh);
			if (wdvh != NULL) {
				((WDVH_ConnUserData)userData)->wdvhHandle = wdvh;
			} else {
				return NULL;
			}
			/* create WDV Handle */
			odbchandle = getOdbcHandle(wa,_pConnection);
			henvhandle = getHenvHandle(wa,_pConnection);
			WDVCAPI_CreateWDV(&wdv,henvhandle,odbchandle,wdvh->xmlHandle);
			if (wdv !=NULL) {
				((WDVH_ConnUserData)userData)->wdvHandle = wdv;
			} else {
				return NULL;
			}
			/* create QSLib Handle */
			/* get library name from webagent */
			sapdbwa_GetParam(wa,"IDXSearchLib",value,512);
			XMLQSLib_CreateHandle(&((WDVH_ConnUserData)userData)->qsHandle,henvhandle,odbchandle,value);
			sapdbwa_SetUserData(_pConnection,userData, (sapdbwa_Destructor) destroyConnUserData);
		} else {
			/* clear Error list on wdv handle */
			WDVCAPI_ClearErrorList(((WDVH_ConnUserData)userData)->wdvHandle);
			/* reset wdvhHandle */
			resetWdvhHandle(((WDVH_ConnUserData)userData)->wdvhHandle);
			/*sapdbwa_SetUserData(_pConnection,userData,destroyConnUserData);*/
		}

		return _pConnection;
	}

	/*------------------------------------------------------------------------
		Function:	closeConnection
	--------------------------------------------------------------------------*/
	sapdbwa_Bool closeConnection(sapdbwa_Handle wa, sapdbwa_DbConnectionP conn)
	{
		if (wa==NULL || conn==NULL)
			return WDVH_False;

		/* close DB Connection */
		#if defined(LOG_LEVEL_1) 
			sapdbwa_WriteLogMsg(wa, "Try DB disconnect ...\n");
		#endif
		sapdbwa_DbDisconnect(conn);
		#if defined(LOG_LEVEL_1) 
			sapdbwa_WriteLogMsg(wa, "DB disconnect successfull\n");
		#endif
		return WDVH_True;
	}

	/*------------------------------------------------------------------------
		Function:	getOdbcHandle
	--------------------------------------------------------------------------*/
	SQLHDBC getOdbcHandle(sapdbwa_Handle wa, sapdbwa_DbConnectionP conn)
	{

        SQLHDBC					phdbc;
        SQLHENV					phenv;

		if (wa==NULL || conn==NULL)
			return NULL;

		/* get the ODBC Handle from the connection */
		#if defined(LOG_LEVEL_1) 
			sapdbwa_WriteLogMsg(wa, "Try getting ODBC Handle ...\n");
		#endif
		sapdbwa_OdbcHandle( conn, &phenv, &phdbc );
		#if defined(LOG_LEVEL_1) 
			sapdbwa_WriteLogMsg(wa, "Got ODBC Handle successfully\n");
		#endif

		return phdbc;
	}

	/*------------------------------------------------------------------------
		Function:	getHenvHandle
	--------------------------------------------------------------------------*/
	SQLHENV getHenvHandle(sapdbwa_Handle wa, sapdbwa_DbConnectionP conn)
	{

        SQLHDBC					phdbc;
        SQLHENV					phenv;

		if (wa==NULL || conn==NULL)
			return NULL;

		/* get the Henv Handle from the connection */
		#if defined(LOG_LEVEL_1) 
			sapdbwa_WriteLogMsg(wa, "Try getting Henv Handle ...\n");
		#endif
		sapdbwa_OdbcHandle( conn, &phenv, &phdbc );
		#if defined(LOG_LEVEL_1) 
			sapdbwa_WriteLogMsg(wa, "Got Henv Handle successfully\n");
		#endif

		return phenv;
	}

	/*------------------------------------------------------------------------
		Function:	getWdvHandle
	--------------------------------------------------------------------------*/
	WDVCAPI_WDV getWdvHandle(sapdbwa_Handle wa, sapdbwa_DbConnectionP conn)
	{
		WDVH_ConnUserData	userdata;

		if (wa==NULL || conn==NULL)
			return NULL;

		userdata = (WDVH_ConnUserData)(sapdbwa_GetUserData(conn));
        if (!userdata) {
            return NULL;
        }

        /* Clear error list */
        WDVCAPI_ClearErrorList(userdata->wdvHandle);
        

		return userdata->wdvHandle;
	}

	/*------------------------------------------------------------------------
		Function:	getWdvhHandle
	--------------------------------------------------------------------------*/
	WDVH_Handle getWdvhHandle(sapdbwa_Handle wa, sapdbwa_DbConnectionP conn)
	{
		WDVH_ConnUserData	userdata;

		if (wa==NULL || conn==NULL)
			return NULL;

		userdata = (WDVH_ConnUserData)(sapdbwa_GetUserData(conn));
        if (!userdata) {
            return NULL;
        }

        return userdata->wdvhHandle;
	}

	/*------------------------------------------------------------------------
		Function:	getXmlHandle
	--------------------------------------------------------------------------*/
	XMLIMAPI_Handle getXmlHandle(sapdbwa_Handle wa, sapdbwa_DbConnectionP conn)
	{
		WDVH_ConnUserData	userdata;

		if (wa==NULL || conn==NULL)
			return NULL;

		userdata = (WDVH_ConnUserData)(sapdbwa_GetUserData(conn));
        if (!userdata || !userdata->wdvhHandle) {
            return NULL;
        }

        return userdata->wdvhHandle->xmlHandle;
	}

	/*------------------------------------------------------------------------
		Function:	getQsHandle
	--------------------------------------------------------------------------*/
	XMLQSLib_Handle getQsHandle(sapdbwa_Handle wa, sapdbwa_DbConnectionP conn)
	{
		WDVH_ConnUserData	userdata;

		if (wa==NULL || conn==NULL)
			return NULL;

		userdata = (WDVH_ConnUserData)(sapdbwa_GetUserData(conn));
        if (!userdata) {
            return NULL;
        }

        return userdata->qsHandle;
	}

	/*------------------------------------------------------------------------
		Function:	getCompressFlag
	--------------------------------------------------------------------------*/
	WDVH_Bool getCompressFlag(sapdbwa_Handle wa, sapdbwa_DbConnectionP conn)
	{
		WDVH_ConnUserData	userdata;

		if (wa==NULL || conn==NULL)
			return WDVH_False;

		userdata = (WDVH_ConnUserData)(sapdbwa_GetUserData(conn));
        if (!userdata) {
            return WDVH_False;
        }

		return userdata->compress;
	}

