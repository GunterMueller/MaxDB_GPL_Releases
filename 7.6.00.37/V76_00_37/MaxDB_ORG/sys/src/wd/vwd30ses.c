/*!**********************************************************************

  module: vwd30ses.c

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  Session pool for ODBC DB connections

  see also:

  -----------------------------------------------------------------------

                         Copyright (c) 1998-2005 SAP AG




    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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

#include "hwd09util.h"
#include "hwd30ses.h"
#include "hwd35res.h"

/*!**********************************************************************

  prototypes and constants for internal functions

************************************************************************/

#define MIN_POOLSIZE 1
#define MAX_POOLSIZE 128

void wd30_FreeConnections( twd30SessionPoolP pool );

sapdbwa_Bool wd30_AllocConn( twd30SessionPoolP pool,
                           sapdbwa_Int4        poolSize,
                           twd26ErrP         err );

void wd30_InitAttributes( twd30SessionPoolP pool );

void wd30_FreeMembers( twd30SessionPoolP pool );

sapdbwa_Bool wd30_Connect( twd30SessionPoolP pool,
                         twd31DbConnP      conn,
                         twd26ErrP         err );

sapdbwa_Bool wd30_Disconnect( twd30SessionPoolP pool,
                            twd31DbConnP      conn,
                            twd26ErrP         err );

/*!**********************************************************************

  Chapter: SessionPoolP

************************************************************************/

typedef enum
{
    ODBCPoolType,
    SQLPoolType,
    UndefPoolType
} twd30PoolType;

struct st_wa_sessionpool
{
    char               Name[MAX_NAME_LEN_WD00];
    char               Datasource[MAX_INI_PAR_LEN_WD00+1];
    char               Driver[MAX_INI_PAR_LEN_WD00+1];
    char               Host[MAX_INI_PAR_LEN_WD00+1];
    char               ServerDb[MAX_INI_PAR_LEN_WD00+1];
    char               User[MAX_INI_PAR_LEN_WD00+1];
    char               Password[MAX_INI_PAR_LEN_WD00+1];
    sapdbwa_Int4       PoolSize;
    SQLHENV            henv;
    twd35ResourcePoolP connPool;
    sapdbwa_Bool       autocommit;      /* set autocommit connect attribut for
                                         * every connection ?                   */
    sapdbwa_Int4       isolation;       /* odbc isolation for odbc pool,
                                         * 0,1,2,3 for sql-class pool           */
    char               sqltrace[MAX_FILENAME_LEN_WD00]; /* sqltrace filename    */
    sapdbwa_Int4       usedConnections; /* statistical info                     */
    twd30PoolType      poolType;        /* SQLHandle- or HDBC-Pool ?            */
}; /* struct st_sessionpool */



twd30SessionPoolP wd30CreateSessionPool( const char *name, twd26ErrP   err )
{
    sapdbwa_Bool         allocatOK;
    twd30SessionPoolP  newPool;
    sapdbwa_UInt2        nameLen;
    
    if (!name) {
        return NULL;
    }

    sapdbwa_SQLALLOCAT( sizeof(struct st_wa_sessionpool),
                (sapdbwa_UInt1**) &newPool,
                &allocatOK );
    if (allocatOK) {
        if (name != NULL) {
            nameLen = strlen( name );
            if (nameLen < sizeof( newPool->Name )) {
                strcpy( newPool->Name, name );
                wd30_InitAttributes( newPool );
                
                return newPool;
            } else {
                wd26SetErr( err, ERR_NAME_TOO_LONG_WD26, name, NULL );
                return NULL;
            }; /* else */
        } else {
            strcpy( newPool->Name, "" );

            return newPool;
        };
    } else {
        wd26SetErr( err, ERR_MEMORY_WD26, NULL, NULL );
        
        return NULL;
    }; /* else */
} /* wd30CreateSessionPool */


void wd30DestroySessionPool( twd30SessionPoolP pool )
{
    if (!pool) {
        return;
    }

    wd30_FreeMembers( pool );
    
    sapdbwa_SQLFREE( (sapdbwa_UInt1*) pool );
} /* wd30DestroySessionPool */


twd31DbConnP wd30DbConnect( twd30SessionPoolP pool, twd26ErrP err )
{
    twd35_ResourceId  connId;
    twd31DbConnP      conn = NULL;

    if (!pool) {
        return NULL;
    }
    
    if (wd35LockElem( pool->connPool, (void**) &conn, &connId )) {
        if (conn == NULL) {
            /* create new connection */
            conn = wd31CreateConn( (sapdbwa_UInt4) connId, pool, err );
            if (conn != NULL) {
                wd35SetElemByIndex( pool->connPool, conn, connId );
                pool->connPool->openConnections++;
            } else {
                /* creation failed */
                wd35UnlockElem( pool->connPool, connId );
            }; /* else */
        }; /* if */
    } else {
        /* max connections limit reached */
        wd26SetErr( err, ERR_CANT_GET_DBCONNECTION_WD26, NULL, NULL );
        return NULL;
    }; /* else */
    
    if (conn != NULL && !wd31IsAlive( conn )) {
        sapdbwa_Bool connOk = wd30_Connect( pool, conn, err );
        if (!connOk) {
            /* don't store broken connections */
            /* call disconnect for the broken connection to clean up
             * stmt-handles and user-data properly */
            wd30_Disconnect( pool, conn, NULL );
            wd31DestroyConn( conn );
            wd35SetElemByIndex( pool->connPool, NULL, connId );
            wd35UnlockElem( pool->connPool, connId );
            return NULL;
        }; /* if */
    }; /* if */

    if ((connId + 1) > pool->usedConnections) { /* statistics */
        pool->usedConnections = connId + 1;
    }; /* if */
    
    return conn;
} /* wd30DbConnect */


sapdbwa_Bool wd30DbDisconnect( twd30SessionPoolP pool, twd31DbConnP conn )
{
    sapdbwa_Int4 connId;

    if (!conn) {
        return sapdbwa_False;
    }

    connId = wd31GetConnId( conn );

    /* we don't want to keep broken connections */
    if (!wd31IsAlive( conn )) {
        /* call disconnect for the broken connection to clean up
         * stmt-handles and user-data properly */
        wd30_Disconnect( pool, conn, NULL );
        wd31DestroyConn( conn );
        wd35SetElemByIndex( pool->connPool, NULL, connId );
    }; /* if */
    
    wd35UnlockElem( pool->connPool, connId );
    
    return sapdbwa_True;
} /* wd30DbDisconnect */


sapdbwa_Bool wd30GetConf( twd30SessionPoolP      pool,
                        char                  *textBuffer,
                        sapdbwa_UInt4           *bufferLen )
{
    sapdbwa_Bool ok = true;
    char sizeStr[20];
    const char *autocommitStr;
    
    if (!pool || !textBuffer || !bufferLen ) {
        return sapdbwa_False;
    }

    sp77sprintf(sizeStr, 20, "%d", pool->PoolSize );
    if (pool->autocommit ) {
        autocommitStr = "SQL_ATTR_AUTOCOMMIT_ON";
    } else {
        autocommitStr = "SQL_ATTR_AUTOCOMMIT_OFF";
    }; /* else */
    ok = wd09AddString( textBuffer, "[", bufferLen )
        && wd09AddString( textBuffer, pool->Name, bufferLen )
        && wd09AddString( textBuffer, "]\n", bufferLen );
    if (ok) {
        if (pool->poolType == ODBCPoolType) {
            ok = wd09AddString( textBuffer, "SessionPoolType=ODBC", bufferLen )
                && wd09AddString( textBuffer, "\n", bufferLen )
                && wd09AddString( textBuffer, "Datasource=", bufferLen )
                && wd09AddString( textBuffer, pool->Datasource, bufferLen )
                && wd09AddString( textBuffer, "\n", bufferLen )
                && wd09AddString( textBuffer, "Driver=", bufferLen )
                && wd09AddString( textBuffer, pool->Driver, bufferLen )
                && wd09AddString( textBuffer, "\n", bufferLen )
                && wd09AddString( textBuffer, "Autocommit=", bufferLen )
                && wd09AddString( textBuffer, autocommitStr, bufferLen )
                && wd09AddString( textBuffer, "\n", bufferLen )
                && wd09AddString( textBuffer, "SqlTrace=", bufferLen )
                && wd09AddString( textBuffer, pool->sqltrace, bufferLen )
                && wd09AddString( textBuffer, "\n", bufferLen );
        } else if (pool->poolType == SQLPoolType) {
            ok = wd09AddString( textBuffer, "SessionPoolType=SQLCLASS",
                                bufferLen )
                && wd09AddString( textBuffer, "\n", bufferLen );

        } else {
            ok = wd09AddString( textBuffer, "<Not initialized!!!>",
                                bufferLen )
                && wd09AddString( textBuffer, "\n", bufferLen );
        }; /* else */
    }; /* if */

    if (ok) {
        ok = wd09AddString( textBuffer, "ServerNode=", bufferLen )
            && wd09AddString( textBuffer, pool->Host, bufferLen )
            && wd09AddString( textBuffer, "\n", bufferLen )
            && wd09AddString( textBuffer, "ServerDb=", bufferLen )
            && wd09AddString( textBuffer, pool->ServerDb, bufferLen )
            && wd09AddString( textBuffer, "\n", bufferLen )
            && wd09AddString( textBuffer, "User=", bufferLen )
            && wd09AddString( textBuffer, pool->User, bufferLen )
            && wd09AddString( textBuffer, "\n", bufferLen )
            && wd09AddString( textBuffer, "Password=xxx\n", bufferLen )
            && wd09AddString( textBuffer, "SessionPoolSize=", bufferLen )
            && wd09AddString( textBuffer, sizeStr, bufferLen )
            && wd09AddString( textBuffer, "\n", bufferLen );
    }; /* if */
    
    return ok;
} /* wd30GetConf */

SQLHENV wd30GetHenv( twd30SessionPoolP pool )
{
    if (!pool) {
        return 0;
    }

    return pool->henv;
} /* wd30GetHenv */


char* wd30GetName( twd30SessionPoolP pool )
{
    if (!pool) {
        return NULL;
    }

    return pool->Name;
} /* wd30GetName */


char* wd30GetServerNode( twd30SessionPoolP pool )
{
    if (!pool) {
        return NULL;
    }

    return pool->Host;
} /* wd30GetServerNode */


char* wd30GetServerDb( twd30SessionPoolP pool )
{
    if (!pool) {
        return NULL;
    }

    return pool->ServerDb;
} /* wd30GetServerDb */


char* wd30GetUser( twd30SessionPoolP pool )
{
    if (!pool) {
        return NULL;
    }

    return pool->User;
} /* wd30GetUser */


char* wd30GetPassword( twd30SessionPoolP pool )
{
    if (!pool) {
        return NULL;
    }

    return pool->Password;
} /* wd30GetUser */


sapdbwa_Bool wd30InitSessionPool( twd30SessionPoolP  pool,
                                const char        *datasource,
                                const char        *driver,
                                const char        *serverNode,
                                const char        *serverDb,
                                const char        *user,
                                const char        *password,
                                sapdbwa_Int4         poolSize,
                                sapdbwa_Bool         autocommit,
                                sapdbwa_Int4         isolation,
                                const char        *sqltrace,
                                twd26ErrP          err )
{
    sapdbwa_Bool    initOK = true;
    RETCODE       retcode; /* retcode for ODBC-functions */

    if (!pool || !datasource || !driver || !serverNode || !serverDb || !user || !password || !sqltrace) {
        return sapdbwa_False;
    }

    pool->poolType = ODBCPoolType;
    if ( strlen(serverNode) >= sizeof( pool->Host )
         || strlen(datasource) >= sizeof( pool->Datasource )
         || strlen(driver) >= sizeof( pool->Driver )
         || strlen(serverDb) >= sizeof( pool->ServerDb )
         || strlen(user) >= sizeof( pool->User )
         || strlen(password) >= sizeof( pool->Password )) {
        /* string too long */
        wd26SetErr( err, ERR_PARAM_TOO_LONG_WD26, pool->Name, NULL );
        initOK = false;
    } else if (poolSize < MIN_POOLSIZE || poolSize > MAX_POOLSIZE) {
        /* wrong poolsize */
        wd26SetErr( err, ERR_POOLSIZE_WD26, pool->Name, NULL );
        initOK = false;
    }; /* if */

    retcode = SQLAllocEnv( &pool->henv );
    if (!wd00Success(retcode)) {
        initOK = false;
    }; /* if */
    if (initOK) {
        strcpy( pool->Datasource, datasource );
        strcpy( pool->Driver, driver );
        strcpy( pool->Host, serverNode );
        strcpy( pool->ServerDb, serverDb );
        strcpy( pool->User, user );
        strcpy( pool->Password, password );
        strcpy( pool->sqltrace, sqltrace );
    
        pool->PoolSize = poolSize;
        pool->autocommit = autocommit;
        pool->isolation  = isolation;
        
        pool->connPool = wd35CreateResourcePool( poolSize );
        if (pool->connPool == NULL) {
            wd26SetErr( err, ERR_MEMORY_WD26, NULL, NULL );
            initOK = false;
        }; /* if */
    }; /* if */                                

    if (!initOK) {
        wd30_FreeMembers( pool );
        wd30_InitAttributes( pool );        
    }; /* else */
    
    return initOK;
} /* wd30InitSessionPool */


sapdbwa_Bool wd30InitSqlSessionPool( twd30SessionPoolP  pool,
                                   const char        *serverNode,
                                   const char        *serverDb,
                                   const char        *user,
                                   const char        *password,
                                   sapdbwa_Int4         isolation,
                                   sapdbwa_Int4         poolSize,
                                   twd26ErrP          err )
{
    sapdbwa_Bool    initOK = true;
    
    if (!pool || !serverNode || !serverDb || !user || !password) {
        return sapdbwa_False;
    }

    pool->poolType = SQLPoolType;
    if ( strlen(serverNode) >= sizeof( pool->Host )
         || strlen(serverDb) >= sizeof( pool->ServerDb )
         || strlen(user) >= sizeof( pool->User )
         || strlen(password) >= sizeof( pool->Password )) {
        /* string too long */
        wd26SetErr( err, ERR_PARAM_TOO_LONG_WD26, pool->Name, NULL );
        initOK = false;
    } else if (poolSize < MIN_POOLSIZE || poolSize > MAX_POOLSIZE) {
        /* wrong poolsize */
        wd26SetErr( err, ERR_POOLSIZE_WD26, pool->Name, NULL );
        initOK = false;
    }; /* if */

    if (initOK) {
        strcpy( pool->Host, serverNode );
        strcpy( pool->ServerDb, serverDb );
        strcpy( pool->User, user );
        strcpy( pool->Password, password );

        pool->isolation = isolation;
        pool->PoolSize = poolSize;
    
        pool->connPool = wd35CreateResourcePool( poolSize );
        if (pool->connPool == NULL) {
            wd26SetErr( err, ERR_MEMORY_WD26, NULL, NULL );
            initOK = false;
        }; /* if */
    }; /* if */                                

    if (!initOK) {
        wd30_FreeMembers( pool );
        wd30_InitAttributes( pool );        
    }; /* else */
    
    return initOK;
} /* wd30InitSqlSessionPool */
    

sapdbwa_Bool wd30Reconnect( twd30SessionPoolP pool,
                          twd31DbConnP      conn,
                          twd26ErrP         err )
{
    sapdbwa_Bool reconnectOk = true;
    
    if (!pool) {
        return sapdbwa_False;
    }

    switch (pool->poolType) {
    case ODBCPoolType:
        wd31OdbcDisconnect( conn, err );
        break;
    case SQLPoolType:
        wd31SqlDisconnect( conn, err );
        break;
    default:
        /* Unknown or illegal poolType */
        wd26SetErr( err, ERR_INTERNAL_WD26,
                    "Unknown or illegal poolType in wd30Reconnect", NULL );
        reconnectOk = false;
        break;
    }; /* switch */
    reconnectOk = wd30_Connect( pool, conn, err );

    return reconnectOk;
} /* wd30Reconnect */

/*!**********************************************************************

  EndChapter: SessionPoolP

************************************************************************/


sapdbwa_Bool wd30_Connect( twd30SessionPoolP pool,
                         twd31DbConnP      conn,
                         twd26ErrP         err )
{
    sapdbwa_Bool connOK = true;
    
    if (!pool) {
        return sapdbwa_False;
    }
    
    switch (pool->poolType) {
    case ODBCPoolType:
        connOK = wd31Connect( conn,
                              pool->Datasource,
                              pool->Driver,
                              pool->Host,
                              pool->ServerDb,
                              pool->User,
                              pool->Password,
                              pool->autocommit,
                              pool->isolation,
                              pool->sqltrace,
                              err );
        break;
    case SQLPoolType:
        connOK = wd31SqlConnect( conn,
                                 pool->Host,
                                 pool->ServerDb,
                                 pool->User,
                                 pool->Password,
                                 pool->isolation,
                                 err );
        break;
    default:
        /* Unknown or illegal poolType */
        wd26SetErr( err, ERR_INTERNAL_WD26,
                    "Unknown or illegal poolType in wd30_Connect", NULL );
        connOK = false;
        break;
    }; /* switch */

    return connOK;
} /* wd30_Connect */


sapdbwa_Bool wd30_Disconnect( twd30SessionPoolP pool,
                            twd31DbConnP      conn,
                            twd26ErrP         err )
{
    sapdbwa_Bool disconnOK = true;
    
    if (!pool) {
        return sapdbwa_False;
    }
    
    switch (pool->poolType) {
    case ODBCPoolType:
        disconnOK = wd31OdbcDisconnect( conn, err );
        break;
    case SQLPoolType:
        disconnOK = wd31SqlDisconnect( conn, err );
        break;
    default:
        /* Unknown or illegal poolType */
        wd26SetErr( err, ERR_INTERNAL_WD26,
                    "Unknown or illegal poolType in wd30_Disconnect", NULL );
        disconnOK = false;
        break;
    }; /* switch */

    return disconnOK;
} /* wd30_Disconnect */


void wd30_FreeConnections( twd30SessionPoolP pool )
{
    sapdbwa_UInt2   iConn;
    twd31DbConnP  conn = NULL;
    
    if (!pool) {
        return;
    }
    
    for (iConn=0; iConn < wd35GetNumAllocElems( pool->connPool ); iConn++ ) {
        if (wd35GetElemByIndex( pool->connPool, iConn, (void**) &conn )) {
			if ( conn ) {
				/* disconnects if connection is open */
				wd31DestroyConn( conn );
				wd35SetElemByIndex( pool->connPool, NULL, iConn );
			}
        }; /* if */
    }; /* for */
} /* wd30_FreeConnections */


void wd30_InitAttributes( twd30SessionPoolP pool )
{
    
    if (!pool) {
        return;
    }

    pool->henv        = SQL_NULL_HENV;
    pool->PoolSize    = 0;
    pool->connPool    = NULL;
    pool->usedConnections = 0;
    pool->autocommit      = true;
    pool->isolation       = SQL_TXN_READ_COMMITTED;
    pool->Datasource[0]   = '\0';
    pool->Driver[0]       = '\0';
    pool->Host[0]         = '\0';
    pool->ServerDb[0]     = '\0';
    pool->User[0]         = '\0';
    pool->Password[0]     = '\0';            
    pool->sqltrace[0]     = '\0';
    pool->poolType        = UndefPoolType;
} /* wd30InitAttributes */


void wd30_FreeMembers( twd30SessionPoolP pool )
{
    if (!pool) {
        return;
    }

    if (pool->connPool != NULL) {
        /* close and destroy connections */
        wd30_FreeConnections( pool );
        wd35DestroyResourcePool( pool->connPool );
        pool->connPool = NULL;
    }; /* if */

    if (pool->henv != SQL_NULL_HENV) {
        SQLFreeEnv( pool->henv );
    }; /* if */
} /* wd30_FreeMembers */

