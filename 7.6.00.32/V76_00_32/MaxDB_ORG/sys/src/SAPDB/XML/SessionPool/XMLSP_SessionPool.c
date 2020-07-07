/*!**********************************************************************

  module: XMLSP_SessionPool.c

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: XML Session Pool

  description:  Session Pool Handling

  see also:

  -----------------------------------------------------------------------

                         Copyright (c) 2003-2005 SAP AG



    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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





************************************************************************/

/****************************************************************************
 ***
 *** Include
 ***
 ****************************************************************************/

#include "SAPDB/SAPDBCommon/SAPDB_Types.h"

/*---------------------------------------------------------------------------*/

#include "SAPDB/XML/SessionPool/XMLSP_SessionPool.h"
#include "SAPDB/XML/Error/XMLError_Error.h"
#include "SAPDB/XML/Semaphore/XMLSem_Semaphore.h"
#include "SAPDB/XML/Lock/XMLLock_Lock.h"
#include "SAPDB/XML/Memory/XMLMem_Memory.h"

/*---------------------------------------------------------------------------*/

/****************************************************************************
 ***
 *** Defines
 ***
 ****************************************************************************/

#define SQL_END_TRANSACTION         (SQLCHAR*)"COMMIT"
#define SQL_CANCEL_TRANSACTION      (SQLCHAR*)"ROLLBACK"

/****************************************************************************
 ***
 *** Types & Structs
 ***
 ****************************************************************************/

struct st_xmlsp_session {
    SAPDB_Bool                      pooledSession;

	SQLHENV	                        hEnv;
	SQLHDBC                         hDBC;

    SQLHSTMT                        hCancelTransaction;
    SQLHSTMT                        hEndTransaction;

    XMLSP_DBServerNodeString        serverNode;
    XMLSP_DBServerNameString        serverName;
    XMLSP_DBUserString              user;
    XMLSP_DBPasswordString          password;
    XMLSP_DBTraceFilenameString     traceFilename;

    void*                           userData;
    XMLLib_Function                 functionDestroyUserData;
};

/*---------------------------------------------------------------------------*/

typedef struct st_xmlsp_session_pool_item *XMLSP_SessionPoolItem;

struct st_xmlsp_session_pool {
    XMLSP_DBServerNodeString       serverNode;
    XMLSP_DBServerNameString       serverName;
    XMLSP_DBUserString             user;
    XMLSP_DBPasswordString         password;
    XMLSP_DBTraceFilenameString    traceFilename;
    XMLSP_SessionPoolSize          sessionPoolSize;
    XMLSP_SessionPoolSize          openSessions;
    XMLSP_SessionPoolItem          hSessionList;

    XMLLock_Lock                   hLock;
    XMLSem_Semaphore               hSemaphore;
};

struct st_xmlsp_session_pool_item {
	SAPDB_Bool             available;
	XMLSP_Session          hSession;
	XMLSP_SessionPoolItem  hNextSessionPoolItem;
};

/****************************************************************************
 ***
 *** Prototypes
 ***
 ****************************************************************************/

SAPDB_Bool Session_Init( XMLSP_Session          hSession,
                         SAPDB_Bool             pooledSession,
                         XMLSP_DBServerNode     serverNode,
                         XMLSP_DBServerName     serverName,
                         XMLSP_DBUser           user,
                         XMLSP_DBPassword       password,
                         XMLSP_DBTraceFilename  traceFilename,
                         XMLError_Error         hError );

SAPDB_Bool Session_Create( XMLSP_Session          *hSession, 
                           XMLSP_DBServerNode      serverNode,
                           XMLSP_DBServerName      serverName,
                           XMLSP_DBUser            user,
                           XMLSP_DBPassword        password,
                           XMLSP_DBTraceFilename   traceFilename,
                           XMLError_Error          hError );

SAPDB_Bool Session_Destroy( XMLSP_Session hSession );

SAPDB_Bool Session_Open( XMLSP_Session    hSession, 
                         XMLError_Error      hError );

SAPDB_Bool Session_Close( XMLSP_Session    hSession );

/*---------------------------------------------------------------------------*/

SAPDB_Bool SessionPool_Init( XMLSP_SessionPool      hSessionPool, 
                              XMLSP_DBServerNode     serverNode,
                              XMLSP_DBServerName     serverName,
                              XMLSP_DBUser           user,
                              XMLSP_DBPassword       password,
                              XMLSP_DBTraceFilename  traceFilename,
                              XMLSP_SessionPoolSize  sessionPoolSize,
                              XMLError_Error            hError );

SAPDB_Bool SessionPool_CreatePoolItem( XMLSP_SessionPoolItem  *hSessionPoolItem,
                                        XMLSP_DBServerNode      serverNode,
                                        XMLSP_DBServerName      serverName,
                                        XMLSP_DBUser            user,
                                        XMLSP_DBPassword        password,
                                        XMLSP_DBTraceFilename   traceFilename,
                                        XMLError_Error             hError );

SAPDB_Bool SessionPool_InitPoolItem( XMLSP_SessionPoolItem   hSessionPoolItem,
                                      XMLSP_DBServerNode      serverNode,
                                      XMLSP_DBServerName      serverName,
                                      XMLSP_DBUser            user,
                                      XMLSP_DBPassword        password,
                                      XMLSP_DBTraceFilename   traceFilename,
                                      XMLError_Error             hError );

SAPDB_Bool SessionPool_DestroyPoolItem( XMLSP_SessionPoolItem   hSessionPoolItem );

/*---------------------------------------------------------------------------*/

void MakeConnectString( char        *connectString,
                        const char  *user,
                        const char  *password,
                        const char  *database,
                        const char  *node,
                        const char  *driver,
                        const char  *tracefile );

/****************************************************************************
 ***
 *** Variables
 ***
 ****************************************************************************/

/***********************************************************************

	Public Functions

 ***********************************************************************/

SAPDB_Bool SP_Create( XMLSP_SessionPool     *hSessionPool, 
                                XMLSP_DBServerNode     serverNode,
                                XMLSP_DBServerName     serverName,
                                XMLSP_DBUser           user,
                                XMLSP_DBPassword       password,
                                XMLSP_DBTraceFilename  traceFilename,
                                XMLSP_SessionPoolSize  sessionPoolSize,
                                XMLError_Error            hError )
{

    SAPDB_Bool ok;

    /* Construct */
	XMLMem_Allocat(sizeof(struct st_xmlsp_session_pool), (SAPDB_UInt1**) hSessionPool, &ok );
	if (!ok) {
        *hSessionPool = NULL;

        return SAPDB_FALSE;
	}

	/* Init */
    if (!SessionPool_Init(*hSessionPool, serverNode, serverName, user, password,
                           traceFilename, sessionPoolSize, hError )) {
        XMLMem_Free(*hSessionPool);
        *hSessionPool = NULL;

        return SAPDB_FALSE;
    }

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool SP_Destroy( XMLSP_SessionPool hSessionPool )
{

    XMLSP_SessionPoolItem  hCurrentSessionPoolItem = NULL;
    XMLSP_SessionPoolItem  hNextSessionPoolItem = NULL;

    /* Checks */
    if (!hSessionPool) {
        return SAPDB_TRUE;
    }

    /* Free session list */
    hCurrentSessionPoolItem = hSessionPool->hSessionList;
    while(hCurrentSessionPoolItem) {
        hNextSessionPoolItem = hCurrentSessionPoolItem->hNextSessionPoolItem;

        /* Destroy pool item */
        SessionPool_DestroyPoolItem(hCurrentSessionPoolItem);

        hCurrentSessionPoolItem = hNextSessionPoolItem;
    }

    /* Destroy lock */
    Lock_Destroy(hSessionPool->hLock);

    /* Destroy semaphore */
    Semaphore_Destroy(hSessionPool->hSemaphore);

    /* Free session pool itself */
    XMLMem_Free(hSessionPool);

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool SP_GetSession( XMLSP_SessionPool  hSessionPool,
                                    XMLSP_Session     *hSession,
                                    XMLError_Error        hError )
{

    XMLSP_SessionPoolItem  hCurrentSessionPoolItem = NULL;

    /* Inits */
    if (hSession) {
        *hSession = NULL;
    }

    /* Checks */
    if (!hSessionPool) {
        Error_Set(XMLERROR_POS, hError, XMLERROR_ERR_CODE_INTERNAL_ERROR,
                                        XMLERROR_ERR_TEXT_INTERNAL_ERROR);

        return SAPDB_FALSE;
    }

    /* Lock list */
    if (!Lock_Begin(hSessionPool->hLock)) {
        return SAPDB_FALSE;
    }

    /* All sessions open? */
    if (hSessionPool->openSessions == hSessionPool->sessionPoolSize) {
        /* Go into the queue          */
        Lock_End(hSessionPool->hLock);

        /* Wait in queue for available session */
        if (!Semaphore_Begin(hSessionPool->hSemaphore)) {
            return SAPDB_FALSE;
        }
    
        Lock_Begin(hSessionPool->hLock);
    }

    /* Is there an available session in list? */
    hCurrentSessionPoolItem = hSessionPool->hSessionList;
    while(hCurrentSessionPoolItem) {
        if (hCurrentSessionPoolItem->available == SAPDB_TRUE) {
            *hSession = hCurrentSessionPoolItem->hSession;
            hCurrentSessionPoolItem->available = SAPDB_FALSE;

            /* Unlock list */
            Lock_End(hSessionPool->hLock);

            return SAPDB_TRUE;
        }

        hCurrentSessionPoolItem = hCurrentSessionPoolItem->hNextSessionPoolItem;
    }

    /* If there is no available session, create a new one */
    if (!SessionPool_CreatePoolItem(&hCurrentSessionPoolItem,
                                     hSessionPool->serverNode,
                                     hSessionPool->serverName,
                                     hSessionPool->user,
                                     hSessionPool->password,
                                     hSessionPool->traceFilename,
                                     hError)) {
        /* Unlock list */
        Lock_End(hSessionPool->hLock);

        return SAPDB_FALSE;
    }

    /* Add new item to list */
    hCurrentSessionPoolItem->hNextSessionPoolItem = hSessionPool->hSessionList;
    hSessionPool->hSessionList = hCurrentSessionPoolItem;

    /* Return new session */
    *hSession = hCurrentSessionPoolItem->hSession;
    hCurrentSessionPoolItem->available = SAPDB_FALSE;

    hSessionPool->openSessions++;

    /* Unlock list */
    Lock_End(hSessionPool->hLock);

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool SP_PutSession( XMLSP_SessionPool hSessionPool,
                                    XMLSP_Session     hSession,
                                    XMLError_Error       hError )
{

    XMLSP_SessionPoolItem  hCurrentSessionPoolItem = NULL;

    /* Checks */
    if (!hSessionPool || !hSession) {
        Error_Set(XMLERROR_POS, hError, XMLERROR_ERR_CODE_INTERNAL_ERROR,
                                        XMLERROR_ERR_TEXT_INTERNAL_ERROR);

        return SAPDB_FALSE;
    }

    /* Is there an available session in list? */
    hCurrentSessionPoolItem = hSessionPool->hSessionList;
    while(hCurrentSessionPoolItem) {
        if (hCurrentSessionPoolItem->hSession->hDBC == hSession->hDBC) {
            /* Lock list */
            if (!Lock_Begin(hSessionPool->hLock)) {
                return SAPDB_FALSE;
            }

            hCurrentSessionPoolItem->available = SAPDB_TRUE;

            if (hSessionPool->openSessions == hSessionPool->sessionPoolSize) {
                /* Signal free connection */
                Semaphore_End(hSessionPool->hSemaphore);
            }

            /* Unlock list */
            Lock_End(hSessionPool->hLock);

            return SAPDB_TRUE;
        }

        hCurrentSessionPoolItem = hCurrentSessionPoolItem->hNextSessionPoolItem;
    }

    /* Session not found */
    Error_Set(XMLERROR_POS, hError, XMLSP_ERR_CODE_SESSION_NOT_FOUND,
                                    XMLSP_ERR_TEXT_SESSION_NOT_FOUND);

    return SAPDB_FALSE;

}

/*===========================================================================*/

SAPDB_Bool SP_StartTransaction( XMLSP_Session   hSession,
                                XMLError_Error  hError )
{

	SQLRETURN   sqlRC = 0;

	if (!hSession) {
		return SAPDB_FALSE;
	}

    if (!hSession->hCancelTransaction) {
	    /* Allocat statement handle */
	    sqlRC = SQLAllocStmt(hSession->hDBC, &hSession->hCancelTransaction);
	    if ( sqlRC != 0 ) {
            Error_SetODBC(XMLERROR_POS, hError, hSession->hEnv, hSession->hDBC, hSession->hCancelTransaction, sqlRC);

		    return SAPDB_FALSE;
	    }

	    /* Prepare statement */
	    sqlRC = SQLPrepare(hSession->hCancelTransaction, SQL_CANCEL_TRANSACTION, SQL_NTS);
	    if ( sqlRC != 0 ) {
            Error_SetODBC(XMLERROR_POS, hError, hSession->hEnv, hSession->hDBC, hSession->hCancelTransaction, sqlRC);

		    SQLFreeStmt(hSession->hCancelTransaction, SQL_DROP);
            hSession->hCancelTransaction = 0;

            return SAPDB_FALSE;
	    }
    } else {
        SQLFreeStmt(hSession->hCancelTransaction, SQL_CLOSE);
    }

    /* Cancel transaction */
    sqlRC = SQLExecute(hSession->hCancelTransaction);
	if ( sqlRC != 0 ) {
        Error_SetODBC(XMLERROR_POS, hError, hSession->hEnv, hSession->hDBC, hSession->hCancelTransaction, sqlRC);

        SQLFreeStmt(hSession->hCancelTransaction, SQL_DROP);
        hSession->hCancelTransaction = 0;

		return SAPDB_FALSE;
	}

	return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool SP_EndTransaction( XMLSP_Session   hSession,
                              XMLError_Error  hError )
{

	SQLRETURN   sqlRC = 0;

    if ( !hSession ) {
		return SAPDB_FALSE;
	}

    if (!hSession->hEndTransaction) {
	    /* Allocat statement handle */
	    sqlRC = SQLAllocStmt(hSession->hDBC, &hSession->hEndTransaction);
	    if ( sqlRC != 0 ) {
            Error_SetODBC(XMLERROR_POS, hError, hSession->hEnv, hSession->hDBC, hSession->hEndTransaction, sqlRC);

		    return SAPDB_FALSE;
	    }

	    /* Prepare statement */
	    sqlRC = SQLPrepare(hSession->hEndTransaction, SQL_END_TRANSACTION, SQL_NTS);
	    if ( sqlRC != 0 ) {
            Error_SetODBC(XMLERROR_POS, hError, hSession->hEnv, hSession->hDBC, hSession->hEndTransaction, sqlRC);

		    SQLFreeStmt(hSession->hEndTransaction, SQL_DROP);
            hSession->hEndTransaction = 0;

            return SAPDB_FALSE;
	    }
    } else {
        SQLFreeStmt(hSession->hEndTransaction, SQL_CLOSE);
    }

    /* Commit transaction */
    sqlRC = SQLExecute(hSession->hEndTransaction);
	if ( sqlRC != 0 ) {
        Error_SetODBC(XMLERROR_POS, hError, hSession->hEnv, hSession->hDBC, hSession->hEndTransaction, sqlRC);

        SQLFreeStmt(hSession->hEndTransaction, SQL_DROP);
        hSession->hEndTransaction = 0;

		return SAPDB_FALSE;
	}

	return SAPDB_TRUE;

}

/*===========================================================================*/

void*   SP_GetUserData( XMLSP_Session hSession )
{

    return hSession->userData;

}

/*===========================================================================*/

void    SP_SetUserData( XMLSP_Session hSession, void *userData, XMLSP_DestroySessionUserData destroyFunction )
{
    XMLLib_Function functionDestroyUserData = hSession->functionDestroyUserData;

    if (hSession->userData) {
        if (functionDestroyUserData) {
            functionDestroyUserData(hSession->userData);
        }
    }

    hSession->userData = userData;
    hSession->functionDestroyUserData = destroyFunction;

}

/*===========================================================================*/

SAPDB_Bool SP_OpenSession( XMLSP_Session        *hSession,
                           XMLSP_DBServerNode    serverNode,
                           XMLSP_DBServerName    serverName,
                           XMLSP_DBUser          user,
                           XMLSP_DBPassword      password,
                           XMLSP_DBTraceFilename traceFilename,
                           XMLError_Error        hError )
{

    /* Create session */
    if (!Session_Create(hSession,
                        serverNode, serverName,
                        user, password, traceFilename, hError)) {
        return SAPDB_FALSE;
    }

    /* Open session */
    if (!Session_Open(*hSession, hError)) {
        Session_Destroy(*hSession);
        hSession = NULL;
        
        return SAPDB_FALSE;
    }

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool SP_CloseSession( XMLSP_Session  hSession,
                            XMLError_Error hError )
{

    if (!hSession) {
        SAPDB_FALSE;
    }

    /* Close session */
    Session_Close(hSession);

    /* Destruct session */
    Session_Destroy(hSession);

    return SAPDB_TRUE;
}

/*===========================================================================*/

SQLHENV SP_GetHEnv( XMLSP_Session hSession)
{

    return hSession->hEnv;

}

/*===========================================================================*/

SQLHDBC SP_GetHDBC( XMLSP_Session hSession)
{

    return hSession->hDBC;

}

/***********************************************************************

	Private Functions

 ***********************************************************************/

SAPDB_Bool Session_Init( XMLSP_Session          hSession,
                         SAPDB_Bool             pooledSession,
                         XMLSP_DBServerNode     serverNode,
                         XMLSP_DBServerName     serverName,
                         XMLSP_DBUser           user,
                         XMLSP_DBPassword       password,
                         XMLSP_DBTraceFilename  traceFilename,
                         XMLError_Error         hError )
{

    hSession->pooledSession = pooledSession;

    hSession->hEnv = 0;
    hSession->hDBC = 0;
    hSession->userData = NULL;
    hSession->functionDestroyUserData = NULL;

    hSession->hCancelTransaction = 0;
    hSession->hEndTransaction = 0;

    Com_StrMaxCopy(hSession->serverNode, serverNode, XMLSP_MAX_DB_SERVER_NODE_LEN);
    Com_StrMaxCopy(hSession->serverName, serverName, XMLSP_MAX_DB_SERVER_NAME_LEN);
    Com_StrMaxCopy(hSession->user, user, XMLSP_MAX_DB_USER_LEN);
    Com_StrMaxCopy(hSession->password, password, XMLSP_MAX_DB_PASSWORD_LEN);
    Com_StrMaxCopy(hSession->traceFilename, traceFilename, XMLSP_MAX_DB_TRACE_FILENAME_LEN);

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool Session_Create( XMLSP_Session          *hSession, 
                           XMLSP_DBServerNode      serverNode,
                           XMLSP_DBServerName      serverName,
                           XMLSP_DBUser            user,
                           XMLSP_DBPassword        password,
                           XMLSP_DBTraceFilename   traceFilename,
                           XMLError_Error          hError )
{

    SAPDB_Bool ok;

    /* Construct */
	XMLMem_Allocat(sizeof(struct st_xmlsp_session), (SAPDB_UInt1**) hSession, &ok );
	if (!ok) {
        *hSession = NULL;

        return SAPDB_FALSE;
	}

	/* Init */
    if (!Session_Init(*hSession, SAPDB_TRUE, serverNode, serverName, user, password, traceFilename, hError )) {
        XMLMem_Free(*hSession);
        *hSession = NULL;

        return SAPDB_FALSE;
    }

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool Session_Destroy( XMLSP_Session hSession )
{

    if (!hSession) {
        return SAPDB_TRUE;
    }

    /* Destroy session user data */
    if (hSession->userData) {
        if (hSession->functionDestroyUserData) {
            hSession->functionDestroyUserData(hSession->userData);

            hSession->userData = NULL;
            hSession->functionDestroyUserData = NULL;
        }
    }

    /* Close and free session */
    Session_Close(hSession);

    /* Destroy session itself */
    XMLMem_Free(hSession);

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool Session_Open( XMLSP_Session    hSession, 
                         XMLError_Error   hError )
{

    SQLRETURN   sqlRC = 0;
	SQLHWND	    hWnd = 0;
	SQLSMALLINT connectStrOutLen = 0;
	char        connectStrIn[256] = "";
	char        connectStrOut[256] = "";

    /* Open db session */
	sqlRC = SQLAllocEnv(&hSession->hEnv);
	if ( sqlRC ) {
        Error_Set(XMLERROR_POS, hError, XMLSP_ERR_CODE_COULD_NOT_ALLOCAT_ODBC_ENV,
                                        XMLSP_ERR_TEXT_COULD_NOT_ALLOCAT_ODBC_ENV);

		return SAPDB_FALSE;
	}

	/* Allocat ODBC Connection */
	sqlRC = SQLAllocConnect(hSession->hEnv, &hSession->hDBC);
	if ( sqlRC ) {
        Error_Set(XMLERROR_POS, hError, XMLSP_ERR_CODE_COULD_NOT_ALLOCAT_ODBC_CONNECT,
                                        XMLSP_ERR_TEXT_COULD_NOT_ALLOCAT_ODBC_CONNECT);

		SQLFreeEnv(hSession->hEnv);
        hSession->hEnv = 0;

		return SAPDB_FALSE;
	}

	/* Make connect string */
	MakeConnectString(connectStrIn,
                      hSession->user,
                      hSession->password,
                      hSession->serverName,
                      hSession->serverNode,
                     "SAP DB", hSession->traceFilename);

	/* Connect to database */
	sqlRC = SQLDriverConnect(hSession->hDBC, hWnd,
	                         (SQLCHAR*)connectStrIn, SQL_NTS,
							 (SQLCHAR*)connectStrOut, 256, &connectStrOutLen, SQL_DRIVER_NOPROMPT);
	if ( sqlRC ) {
        Error_SetODBC(XMLERROR_POS, hError, hSession->hEnv, hSession->hDBC, NULL, sqlRC);
        Error_Set(XMLERROR_POS, hError, XMLSP_ERR_CODE_COULD_NOT_CONNECT,
                                        XMLSP_ERR_TEXT_COULD_NOT_CONNECT);

		SQLFreeConnect(hSession->hDBC);
		SQLFreeEnv(hSession->hEnv);

        hSession->hDBC = 0;
        hSession->hEnv = 0;

        return SAPDB_FALSE;
    }

    /* Set autocommit off */
    sqlRC = SQLSetConnectOption(hSession->hDBC, SQL_AUTOCOMMIT, SQL_AUTOCOMMIT_OFF);
	if ( sqlRC ) {
        Error_Set(XMLERROR_POS, hError, XMLSP_ERR_CODE_COULD_NOT_SET_AUTOCOMMIT_OFF,
                                        XMLSP_ERR_TEXT_COULD_NOT_SET_AUTOCOMMIT_OFF);

		SQLDisconnect(hSession->hDBC);
		SQLFreeConnect(hSession->hDBC);
		SQLFreeEnv(hSession->hEnv);

        hSession->hDBC = 0;
        hSession->hEnv = 0;

		return SAPDB_FALSE;
	}

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool Session_Close( XMLSP_Session    hSession )
{

    if (!hSession) {
        return SAPDB_TRUE;
    }

    /* Close and free statement handles */
    if (hSession->hCancelTransaction) {
        SQLFreeStmt(hSession->hCancelTransaction, SQL_DROP);
        hSession->hCancelTransaction = 0;
    }

    if (hSession->hEndTransaction) {
        SQLFreeStmt(hSession->hEndTransaction, SQL_DROP);
        hSession->hEndTransaction = 0;
    }

    /* Close and free session */
    if (hSession->hDBC) {
	    SQLDisconnect(hSession->hDBC);
	    SQLFreeConnect(hSession->hDBC);
        hSession->hDBC = 0;
    }

    if (hSession->hEnv) {
	    SQLFreeEnv(hSession->hEnv);
        hSession->hEnv = 0;
    }

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool SessionPool_Init( XMLSP_SessionPool      hSessionPool, 
                              XMLSP_DBServerNode     serverNode,
                              XMLSP_DBServerName     serverName,
                              XMLSP_DBUser           user,
                              XMLSP_DBPassword       password,
                              XMLSP_DBTraceFilename  traceFilename,
                              XMLSP_SessionPoolSize  sessionPoolSize,
                              XMLError_Error            hError )
{

    Com_StrMaxCopy(hSessionPool->serverNode, serverNode, XMLSP_MAX_DB_SERVER_NODE_LEN);
    Com_StrMaxCopy(hSessionPool->serverName, serverName, XMLSP_MAX_DB_SERVER_NAME_LEN);
    Com_StrMaxCopy(hSessionPool->user, user, XMLSP_MAX_DB_USER_LEN);
    Com_StrMaxCopy(hSessionPool->password, password, XMLSP_MAX_DB_PASSWORD_LEN);
    Com_StrMaxCopy(hSessionPool->traceFilename, traceFilename, XMLSP_MAX_DB_TRACE_FILENAME_LEN);

    hSessionPool->sessionPoolSize = sessionPoolSize;
    hSessionPool->openSessions = 0;
    hSessionPool->hSessionList = NULL;

    if (!Lock_Create(&hSessionPool->hLock)) {
        return SAPDB_FALSE;
    }

    if (!Semaphore_Create(&hSessionPool->hSemaphore)) {
        Lock_Destroy(hSessionPool->hLock);

        return SAPDB_FALSE;
    }

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool SessionPool_CreatePoolItem( XMLSP_SessionPoolItem  *hSessionPoolItem,
                                        XMLSP_DBServerNode      serverNode,
                                        XMLSP_DBServerName      serverName,
                                        XMLSP_DBUser            user,
                                        XMLSP_DBPassword        password,
                                        XMLSP_DBTraceFilename   traceFilename,
                                        XMLError_Error             hError )
{

    SAPDB_Bool ok;

    /* Construct */
	XMLMem_Allocat(sizeof(struct st_xmlsp_session_pool_item), (SAPDB_UInt1**) hSessionPoolItem, &ok );
	if (!ok) {
        *hSessionPoolItem = NULL;

        return SAPDB_FALSE;
	}

	/* Init */
    if (!SessionPool_InitPoolItem(*hSessionPoolItem,
                                   serverNode, serverName,
                                   user, password, traceFilename, hError)) {
        XMLMem_Free(*hSessionPoolItem);
        *hSessionPoolItem = NULL;

        return SAPDB_FALSE;
    }

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool SessionPool_InitPoolItem( XMLSP_SessionPoolItem   hSessionPoolItem,
                                     XMLSP_DBServerNode      serverNode,
                                     XMLSP_DBServerName      serverName,
                                     XMLSP_DBUser            user,
                                     XMLSP_DBPassword        password,
                                     XMLSP_DBTraceFilename   traceFilename,
                                     XMLError_Error          hError )
{

    hSessionPoolItem->available = SAPDB_TRUE;
	hSessionPoolItem->hNextSessionPoolItem = NULL;

    /* Create session */
    if (!Session_Create(&hSessionPoolItem->hSession,
                         serverNode, serverName,
                         user, password, traceFilename, hError)) {
        return SAPDB_FALSE;
    }

    /* Open session */
    if (!Session_Open(hSessionPoolItem->hSession, hError)) {
        Session_Destroy(hSessionPoolItem->hSession);
        hSessionPoolItem->hSession = NULL;
        
        return SAPDB_FALSE;
    }

    return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool SessionPool_DestroyPoolItem( XMLSP_SessionPoolItem   hSessionPoolItem )
{

    if (!hSessionPoolItem) {
        SAPDB_FALSE;
    }

    /* Close session */
    /*
    Session_Close(hSessionPoolItem->hSession);
    */

    /* Destruct session */
    Session_Destroy(hSessionPoolItem->hSession);

    /* Free item itself */
    XMLMem_Free(hSessionPoolItem);

    return SAPDB_TRUE;

}

/*===========================================================================*/

void MakeConnectString( char        *connectString,
                        const char  *user,
                        const char  *password,
                        const char  *database,
                        const char  *node,
                        const char  *driver,
                        const char  *tracefile )
{

    sprintf( connectString, "DRIVER=%s", driver );

    if (node != NULL && strlen(node) > 0) {
        strcat( connectString, ";SERVERNODE=" );
        strcat( connectString, node );
    };

    if (database != NULL && strlen(database) > 0) {
        strcat( connectString, ";SERVERDB=" );
        strcat( connectString, database );
    };

    if (user != NULL && strlen(user) > 0) {
        strcat( connectString, ";UID=" );
        strcat( connectString, user );
    };

    if (password != NULL && strlen(password) > 0) {
        strcat( connectString, ";PWD=" );
        strcat( connectString, password );
    };

    if (tracefile != NULL && strlen(tracefile) > 0) {
        strcat( connectString, ";TRACEFILENAME=" );
        strcat( connectString, tracefile );
    };

    strcat( connectString, ";" );

}

/*===========================================================================*/

/***********************************************************************

	End

 ***********************************************************************/
