/*!**********************************************************************

  module: vwd106pool.c

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  Abstract session pool interface

  see also:

  -----------------------------------------------------------------------

  copyright:            Copyright (c) 1999-2005 SAP AG




    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "gwd00.h"
#include "hwd05wases.h"
#include "hwd32hses.h"
#include "hwd36num.h"
#include "hwd106pool.h"

#define MAX_DBC_CONNECTIONS -1 /* unlimited */

/*!**********************************************************************

  Chapter: twd106ConnP
  
************************************************************************/

typedef enum
{
    sesConnType,
    dbcConnType,
    unknownConnType /* == not connected */
} twd106ConnType;

struct st_conn
{
    twd106ConnType connType;
    
    union
    {
        sapdbwa_DbConnectionP sesConn;
        sapdbwa_DBCP          dbcConn;
    } conn;

    twd106PoolP         pool;

    /* If this is sapdbwa_True the dbc conn gets its own ODBC connection.
     * For ses connections this attribut has no effect. */
    sapdbwa_Bool          exclusive;
    
    /* all twd106ConnP's of one twd106Pool, with exclusive == sapdbwa_False
     * share the same
     * sapdbwa_DBCConnP but each has its own user data. This is the index
     * of the user data. */
    sapdbwa_Int4          userDataId;
}; /* struct st_conn */


struct st_pool
{
    twd106ConnType connType;
    
    union
    {
        sapdbwa_SessionPoolP  sesPool;
        sapdbwa_DBCPoolP      dbcPool;
    } pool;
    
    twd34ConnectParam  connParam; /* only needed for dbcConnType */
    sapdbwa_DBCP             dbcConn;   /* for dbcPool we use only one connection */
    
    /* the resource pool is used to generate the id's for the
     * user data in dbcConn. */
    twd36NumPoolP      userDataRes;
}; /* struct st_conn */

/*===========================================================================*/

sapdbwa_DBCP wd106GetDBCCon( twd106ConnP    hConn )
{

    if (hConn) {
        return hConn->conn.dbcConn;
    } else {
        return NULL;
    }

}

/*===========================================================================*/

twd106ConnP wd106CreateConn( sapdbwa_Bool exclusive )
{
    sapdbwa_Bool allocatOK    = sapdbwa_True;
    twd106ConnP newConn = NULL;

    sapdbwa_SQLALLOCAT( sizeof( struct st_conn ),
                   (sapdbwa_UInt1**) &newConn,
                   &allocatOK );
    if (allocatOK) {
        newConn->connType     = unknownConnType;
        newConn->conn.sesConn = NULL;
        newConn->pool         = NULL;
        newConn->userDataId   = 0;
        newConn->exclusive    = exclusive;
    }; /* if */

    return newConn;
} /* wd106CreateConn */


void wd106DestroyConn( twd106ConnP conn )
{
    sapdbwa_ASSERT( conn->connType == unknownConnType ); /* are we connected ? */
    sapdbwa_SQLFREE( (sapdbwa_UInt1*) conn );
} /* wd106DestroyConn */


sapdbwa_Bool wd106CheckConn( twd106ConnP  conn,
                           sapdbwa_Int4   nativeErr,
                           const char  *sqlState,
                           sapdbwa_Bool  *connectionDown )
{
    sapdbwa_Bool checkOK = sapdbwa_True;
    
    switch (conn->connType) {
    case sesConnType:
        checkOK = sapdbwa_CheckConnection( conn->conn.sesConn,
                                     nativeErr,
                                     sqlState,
                                     connectionDown );
        break;
    case dbcConnType:
        checkOK = sapdbwa_CheckDBC( conn->conn.dbcConn,
                              nativeErr,
                              sqlState,
                              connectionDown );
        break;
    case unknownConnType:
        break;
    default:
        /* illegal connType */
        checkOK = sapdbwa_False;
        break;
    }; /* switch */

    return checkOK;
} /* wd106CheckConn */


sapdbwa_Bool wd106Disconnect( twd106ConnP conn )
{
    sapdbwa_Bool disconnectOK = sapdbwa_True;
    
    switch (conn->connType) {
    case sesConnType:
        sapdbwa_DbDisconnect( conn->conn.sesConn );
        conn->conn.sesConn = NULL;
        conn->connType = unknownConnType;
        break;
    case dbcConnType:
        if (conn->exclusive) {
            /* "close" connection */
            sapdbwa_DBCDisconnect( conn->pool->pool.dbcPool, conn->conn.dbcConn );
            sapdbwa_DestroyDBC( conn->conn.dbcConn );
        } else {
            wd36PutNumber( conn->pool->userDataRes, conn->userDataId );
        }; /* else */
        conn->conn.dbcConn = NULL;
        conn->connType = unknownConnType;
        break;
    case unknownConnType:
        /* not connected */
        break;
    default:
        /* illegal connType */
        disconnectOK = sapdbwa_False;
        break;
    }; /* switch */

    return disconnectOK;
} /* wd106Disconnect */


sapdbwa_Bool wd106OdbcHandle( twd106ConnP  conn,
                            SQLHENV     *phenv,
                            SQLHDBC     *phdbc )
{
    sapdbwa_Bool getHandleOK = sapdbwa_True;

    switch (conn->connType) {
    case sesConnType:
        sapdbwa_OdbcHandle( conn->conn.sesConn, phenv, phdbc );
        break;
    case dbcConnType:
        *phenv = sapdbwa_DBCEnv( conn->conn.dbcConn );
        *phdbc = sapdbwa_DBCHandle( conn->conn.dbcConn );
        break;
    case unknownConnType:
        /* not connected */
        getHandleOK = sapdbwa_False;
        break;
    default:
        /* illegal connType */
        getHandleOK = sapdbwa_False;
        break;
    }; /* switch */
    
    return getHandleOK;
} /* wd106OdbcHandle */


void* wd106GetUserData( twd106ConnP conn )
{
    void *userData = NULL;
    
    switch (conn->connType) {
    case sesConnType:
        userData = sapdbwa_GetUserData( conn->conn.sesConn );
        break;
    case dbcConnType:
        /* use userDataId+1 to reserve 0 for "real" userData */
        userData = wd34GetUserData( conn->conn.dbcConn, conn->userDataId+1 );
        break;
    case unknownConnType:
        /* not connected */
        break;
    default:
        /* illegal connType */
        break;
    }; /* switch */
    
    return userData;
} /* wd106GetUserData */


sapdbwa_Bool wd106SetUserData( twd106ConnP   conn,
                             void         *data,
                             sapdbwa_Destructor  destructor )
{
    sapdbwa_Bool setOK = sapdbwa_True;
    
    switch (conn->connType) {
    case sesConnType:
        setOK = sapdbwa_SetUserData( conn->conn.sesConn, data, destructor );
        break;
    case dbcConnType:
        /* use userDataId+1 to reserve 0 for "real" userData */
        setOK = wd34SetUserData( conn->conn.dbcConn,
                                 (sapdbwa_UInt4) conn->userDataId+1,
                                 data, destructor );
        break;
    case unknownConnType:
        /* not connected */
        setOK = sapdbwa_False;
        break;
    default:
        /* illegal connType */
        setOK = sapdbwa_False;
        break;
    }; /* switch */
    
    return setOK;
} /* wd106SetUserData */


void wd106SetDBCCon( twd106PoolP    pool,
                     sapdbwa_DBCP   dbc )
{
    pool->dbcConn = dbc;
}


const char *wd106GetUsername( twd106ConnP   conn )
{
    const char *username;
    
    switch (conn->connType) {
    case sesConnType:
        username = sapdbwa_GetUserName( conn->pool->pool.sesPool );
        break;
    case dbcConnType:
        username = conn->pool->connParam.user;
        break;
    case unknownConnType:
        username = NULL;
        break;
    default:
        /* illegal connType */
        username = NULL;
        break;
    }; /* switch */
    
    return username;
} /* wd106GetUsername */

/*!**********************************************************************

  EndChapter: twd106ConnP

************************************************************************/

/*!**********************************************************************

  Chapter: twd106PoolP
  
************************************************************************/


twd106PoolP wd106CreatePoolFromSes( sapdbwa_SessionPoolP sessionPool )
{
    sapdbwa_Bool allocatOK    = sapdbwa_True;
    twd106PoolP newPool = NULL;
    
    sapdbwa_SQLALLOCAT( sizeof( struct st_pool ),
                   (sapdbwa_UInt1**) &newPool,
                   &allocatOK );
    if (allocatOK) {
        newPool->connType      = sesConnType;
        newPool->dbcConn       = NULL;
        /* we have to make an own copy, because the pool handle is
           destroyed after each request */
        newPool->pool.sesPool  = wd32ClonePoolHandle( sessionPool );
        wd34InitConnectParam( &newPool->connParam );
        newPool->userDataRes   = NULL;
    }; /* if */

    return newPool;
} /* wd106CreatePoolFromSes */


twd106PoolP wd106CreatePoolFromDbc( sapdbwa_DBCPoolP   dbcPool,
                                    const char  *datasource,
                                    const char  *driver,
                                    const char  *serverNode,
                                    const char  *serverDb,
                                    const char  *user,
                                    const char  *password,
                                    const char  *sqltrace )
{
    sapdbwa_Bool allocatOK    = sapdbwa_True;
    twd106PoolP newPool = NULL;
    
    sapdbwa_SQLALLOCAT( sizeof( struct st_pool ),
                   (sapdbwa_UInt1**) &newPool,
                   &allocatOK );
    if (allocatOK) {
        newPool->userDataRes   = wd36CreateNumPool( MAX_DBC_CONNECTIONS );
        if (newPool->userDataRes != NULL) {
            newPool->connType      = dbcConnType;
            newPool->pool.dbcPool  = dbcPool;
            newPool->dbcConn       = NULL;
            wd34InitConnectParam( &newPool->connParam );
            wd34SetConnect( &newPool->connParam,
                            datasource, driver,
                            serverNode, serverDb,
                            user, password,
                            sqltrace );
        } else {
            sapdbwa_SQLFREE( (sapdbwa_UInt1*) newPool );
            newPool = NULL;
        }; /* else */
    }; /* if */

    return newPool;    
} /* wd106CreatePoolFromDbc */


void wd106DestroyPool( twd106PoolP pool )
{
    wd34FreeConnectParam( &pool->connParam );
    if (pool->connType == sesConnType ) {
        wd32DestroyPoolHandle( pool->pool.sesPool );
    } else { /* dbcConnType */
        if (pool->dbcConn != NULL) {
            sapdbwa_DBCDisconnect( pool->pool.dbcPool, pool->dbcConn );
            sapdbwa_DestroyDBC( pool->dbcConn );
            pool->dbcConn = NULL;
        }; /* if */
        wd36DestroyNumPool( pool->userDataRes );
    }; /* else */
    
    sapdbwa_SQLFREE( (sapdbwa_UInt1*) pool );
} /* wd106DestroyPool */


sapdbwa_Bool wd106Connect( twd106PoolP  pool,
                         twd106ConnP  conn,
                         twd26ErrP    err )
{
    sapdbwa_Bool connectOK = sapdbwa_True;

    if (conn->connType != unknownConnType) {
        /* we are already connected */
        connectOK = sapdbwa_False;
    } else {
        switch (pool->connType) {
        case sesConnType:
            conn->conn.sesConn = wd32DbConnect( pool->pool.sesPool, err );
            connectOK = (conn->conn.sesConn != NULL);
            if (connectOK) {
                conn->connType = sesConnType;
                conn->pool     = pool;
            } else {
                conn->connType = unknownConnType;
                conn->pool     = NULL;
            }; /* else */
            break;
        case dbcConnType: {
            if (conn->exclusive) {
                /* get new dbc connection */
                sapdbwa_ASSERT( conn->conn.dbcConn == NULL );
                conn->conn.dbcConn = sapdbwa_CreateDBC();
                if (conn->conn.dbcConn != NULL) {
                    connectOK = wd34Connect( pool->pool.dbcPool,
                                             conn->conn.dbcConn,
                                             pool->connParam.datasource,
                                             pool->connParam.driver,
                                             pool->connParam.serverNode,
                                             pool->connParam.serverDb,
                                             pool->connParam.user,
                                             pool->connParam.password,
                                             pool->connParam.sqltrace, 0,
                                             NULL,
                                             sapdbwa_DBC_EXCL, SAPDB_FALSE,
                                             err );
                }; /* if */
                if (!connectOK) {
                    sapdbwa_DestroyDBC( conn->conn.dbcConn );
                    conn->conn.dbcConn = NULL;
                } else {
                    conn->connType      = dbcConnType;
                    conn->pool          = pool;
                }; /* else */    
            } else {
                /* use the shared connection from pool */
                if (wd34IsPoolElemNull(pool->dbcConn) == sapdbwa_True) {
                    /* create new connection */
                    /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1136541 */
                    /* pool->dbcConn = sapdbwa_CreateDBC(); */
                    connectOK = wd34Connect( pool->pool.dbcPool,
                                             pool->dbcConn,
                                             pool->connParam.datasource,
                                             pool->connParam.driver,
                                             pool->connParam.serverNode,
                                             pool->connParam.serverDb,
                                             pool->connParam.user,
                                             pool->connParam.password,
                                             pool->connParam.sqltrace, 0,
                                             NULL,
                                             sapdbwa_DBC_SHARED, SAPDB_FALSE,
                                             err );
                    if (!connectOK) {
                        sapdbwa_DestroyDBC( pool->dbcConn );
                        pool->dbcConn  = NULL;
                    }; /* else */
                }; /* if */
                conn->conn.dbcConn = pool->dbcConn;
                if (conn->conn.dbcConn != NULL) {
                    if (wd36GetNumber( pool->userDataRes,
                                       &conn->userDataId )) {
                        conn->connType      = dbcConnType;
                        conn->pool          = pool;
                    } else {
                        /* cant get free id */
                        sapdbwa_DBCDisconnect( pool->pool.dbcPool, pool->dbcConn );
                        sapdbwa_DestroyDBC( pool->dbcConn );
                        pool->dbcConn      = NULL;
                        conn->conn.dbcConn = NULL;
                        conn->connType     = unknownConnType;
                        conn->pool         = NULL;
                        connectOK          = sapdbwa_False;
                    }; /* else */
                } else {
                    conn->connType      = unknownConnType;
                    conn->pool          = NULL;
                }; /* else */
            }; /* else */
            break;
        }
        default:
            /* unknown connType */
            connectOK = sapdbwa_False;
        }; /* switch */
    }; /* else */
    
    return connectOK;
} /* wd106Connect */

/*!**********************************************************************

  EndChapter: twd106Pool

************************************************************************/
