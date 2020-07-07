/*!**********************************************************************

  module: vwd31conn.c

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  Connection Type representing a ODBC DB connection

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

#include "hwd30ses.h"
#include "hwd31conn.h"
#include "hwd38data.h"
#include "hwd39sqlconn.h"

sapdbwa_Bool wd31OdbcDisconnect( twd31DbConnP conn, twd26ErrP err );

sapdbwa_Bool wd31SqlDisconnect( twd31DbConnP conn, twd26ErrP err );

/*!**********************************************************************

  Chapter: DbConnectionP

************************************************************************/

#define WEBAGENT_DATASOURCE_NAME "WebAgent"

struct st_wa_db_connection
{
    twd30SessionPoolP		pool;			/* the session-pool this connection	*/
											/* belongs to                       */
    SQLHDBC					hdbc;			/* the ODBC connection handle       */
    twd39SqlConnP			sqlHandle;		/* the sql-connection handle		*/
    sapdbwa_Bool			Alive;			/* Are we already connected?        */
    sapdbwa_UInt4			Id;				/* Unique id (inside pool)          */
    twd38UserDataP			userDataList;	/* List of user data				*/
    sapdbwa_UInt2			allocUserData;	/* size of userDataArray			*/
}; /* struct st_wa_db_connection */


twd31DbConnP wd31CreateConn( sapdbwa_UInt4        connId,
                             twd30SessionPoolP  pool,
                             twd26ErrP          err )
{
    twd31DbConnP newConn;
    sapdbwa_Bool   allocatOK;
    
    sapdbwa_SQLALLOCAT( sizeof(struct st_wa_db_connection),
                   (sapdbwa_UInt1**) &newConn,
                   &allocatOK );
    if (allocatOK) {
        newConn->userDataList	= NULL;
        newConn->pool			= pool;
        newConn->hdbc			= SQL_NULL_HENV;
        newConn->sqlHandle		= NULL;
        newConn->Alive			= false;
        newConn->Id				= connId;
        newConn->allocUserData	= 0;

        return newConn;
    } else {
        wd26SetErr( err, ERR_MEMORY_WD26, NULL, NULL );
        return NULL;
    }; /* else */     
} /* wd31CreateConn */


void wd31DestroyConn( twd31DbConnP conn )
{
    sapdbwa_Bool disconnectOK;
    twd26ErrP err;

    if (!conn) {
        return;
    }

    if (conn->Alive) {
        err = wd26CreateErr();
        if (conn->hdbc != SQL_NULL_HDBC) {
            disconnectOK = wd31OdbcDisconnect( conn, err );
        } else {
            sapdbwa_ASSERT( conn->sqlHandle != NULL);
            disconnectOK = wd31SqlDisconnect( conn, err );
        }; /* else */
        /* no error handling here */
        wd26DestroyErr( err );
    }; /* if */
    wd38FreeAllUserDatas( &(conn->userDataList) );
    
    sapdbwa_SQLFREE( (sapdbwa_UInt1*) conn );
} /* wd31DestroyConn */


sapdbwa_Bool wd31CheckConnection( twd31DbConnP     conn,
                                sapdbwa_Int4       nativeErr,
                                const char      *sqlState,
                                sapdbwa_Bool      *connectionDown,
                                twd26ErrP        err )
{
    sapdbwa_Bool retcode = true;
    
    if (!conn) {
        return sapdbwa_False;
    }

    if ((sqlState != NULL
         && (strcmp( sqlState, "S1T00" ) == 0
             || strcmp( sqlState, "HYT00" ) == 0))
        || (nativeErr == 700 || nativeErr == 800
            || nativeErr == -708 || nativeErr == -807 || nativeErr == -813 )) {
        *connectionDown = true;
        /* the user-data may contain stmt-handles that have become invalid
         * so destroy all user-data */
        wd38FreeAllUserDatas( &(conn->userDataList) );
        retcode = wd30Reconnect( conn->pool, conn, err );
    } else {
        *connectionDown = false;
    }; /* else */

    return retcode;
} /* wd31CheckConnection */

/*===========================================================================*/

sapdbwa_Bool wd31Connect( twd31DbConnP   conn,
                          const char    *datasource,
                          const char    *driver,
                          const char    *host,
                          const char    *serverdb,
                          const char    *user,
                          const char    *password,
                          sapdbwa_Bool   autocommit,
                          sapdbwa_Int4   isolation,
                          const char    *sqltrace,
                          twd26ErrP      err )
{

    RETCODE      retcode;
    char         tracefile[MAX_FILENAME_LEN_WD00];
    char         connectString[1024];
    char         outConnStr[1024];
    sapdbwa_Int2 outLen;
    int          myHDBC = 0;
    
    if (!conn) {
        return sapdbwa_False;
    }

    retcode = SQLAllocConnect(wd30GetHenv(conn->pool), &conn->hdbc);
    if (retcode != SQL_SUCCESS) {
        wd26SetOdbcError(err, retcode, wd30GetHenv( conn->pool ), SQL_NULL_HDBC, SQL_NULL_HSTMT);
        return false;
    };

    retcode = SQLSetConnectOption(conn->hdbc, SQL_TXN_ISOLATION, isolation);
    if (retcode != SQL_SUCCESS) {
        wd26SetOdbcError(err, retcode, wd30GetHenv(conn->pool), conn->hdbc, SQL_NULL_HSTMT);
        return false;
    };
    
    if (sqltrace != NULL && strlen(sqltrace) != 0) {
        sp77sprintf(tracefile, MAX_FILENAME_LEN_WD00, "%s%p.log", sqltrace, conn->hdbc);
    } else {
        tracefile[0] = '\0';
    };

    wd31MakeConnectString(datasource, driver,
                          host, serverdb, user, password,
                          tracefile, connectString, sizeof( connectString ));

    retcode = SQLDriverConnect(conn->hdbc, NULL,
                              (UCHAR*) connectString, SQL_NTS,
                              (UCHAR*) outConnStr, sizeof(outConnStr),
                              &outLen, SQL_DRIVER_NOPROMPT);
    if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO) {
        wd26SetOdbcError(err, retcode, wd30GetHenv( conn->pool ), conn->hdbc, SQL_NULL_HSTMT);
        return false;
    };

    if (!autocommit) {
        retcode = SQLSetConnectOption(conn->hdbc, SQL_AUTOCOMMIT, SQL_AUTOCOMMIT_OFF);
        if (retcode != SQL_SUCCESS) {
            SQLDisconnect(conn->hdbc);
            wd26SetOdbcError(err, retcode, wd30GetHenv(conn->pool), conn->hdbc, SQL_NULL_HSTMT);

            return false;
        };
    };
    
    conn->Alive = true;
    
    return true;

}

/*===========================================================================*/

sapdbwa_Bool wd31SqlConnect( twd31DbConnP   conn,
                           const char    *host,
                           const char    *serverdb,
                           const char    *user,
                           const char    *password,
                           sapdbwa_Int4     isolation,
                           twd26ErrP      err )
{
    sapdbwa_Bool connectOk = true;

    if (!conn) {
        return sapdbwa_False;
    }

    connectOk = wd39AllocSqlConn( &conn->sqlHandle, err );

    if (connectOk) {
        connectOk = wd39SqlConnect( conn->sqlHandle,
                                    host, serverdb,
                                    user, password,
                                    isolation,
                                    err );
        conn->Alive = connectOk;
    }; /* if */

    return connectOk;
} /* wd31SqlConnect */


sapdbwa_Bool wd31DbDisconnect( twd31DbConnP conn )
{
    if (conn) {
        return wd30DbDisconnect( conn->pool, conn );
    } else {
        return sapdbwa_False;
    }
} /* wd31DbDisconnect */


sapdbwa_Bool wd31OdbcDisconnect( twd31DbConnP conn, twd26ErrP err )
{
    RETCODE retcode;

    if (!conn) {
        return sapdbwa_False;
    }

    /* Destroy user-data before disconnecting. So the stmt-handles
     * are not dropped twice. */
    wd38FreeAllUserDatas( &(conn->userDataList) );
    
    retcode = SQLDisconnect( conn->hdbc );
    if (retcode != SQL_SUCCESS) {
        if (err != NULL) {
            wd26SetOdbcError( err,
                              retcode,
                              wd30GetHenv( conn->pool ),
                              conn->hdbc, SQL_NULL_HSTMT );
        }; /* if */
    }; /* if */
    conn->Alive = false;

    retcode = SQLFreeConnect( conn->hdbc );
    if (retcode != SQL_SUCCESS) {
        if (err != NULL) {
            wd26SetOdbcError( err,
                              retcode,
                              wd30GetHenv( conn->pool ),
                              conn->hdbc, SQL_NULL_HSTMT );
        }; /* if */
    }; /* if */
    
    return true;
} /* wd31OdbcDisconnect */


sapdbwa_Bool wd31SqlDisconnect( twd31DbConnP conn, twd26ErrP err )
{
    sapdbwa_Bool disconnectOk = true;

    if (!conn) {
        return sapdbwa_False;
    }
    
    /* Destroy user-data before diconnecting. So the stmt-handles
     * are not dropped twice. */
    wd38FreeAllUserDatas( &(conn->userDataList) );

    disconnectOk = wd39SqlDisconnect( conn->sqlHandle, err );
    wd39FreeSqlConn( conn->sqlHandle );

    return disconnectOk;
} /* wd31SqlDisconnect */


sapdbwa_UInt4 wd31GetConnId( twd31DbConnP  conn )
{
    if (conn) {
        return conn->Id;
    } else {
        return 0;
    }
} /* wd31GetWaitCount */


void* wd31GetUserData( twd31DbConnP  conn, sapdbwa_UInt4  userDllId )
{
    if (conn) {
        return wd38GetUserData( conn->userDataList, userDllId );
    } else {
        return NULL;
    }
} /* wd31GetUserData */


sapdbwa_Bool wd31OdbcHandle( twd31DbConnP  conn,
                           SQLHENV      *phenv,
                           SQLHDBC      *phdbc  )
{
    if (!phenv || ! phdbc || !conn || !conn->pool) {
        return sapdbwa_False;
    }

    *phenv = wd30GetHenv( conn->pool );
    *phdbc = conn->hdbc;

    return sapdbwa_True;
} /* wd31OdbcHandle */


sapdbwa_Bool wd31SqlHandle( twd31DbConnP  conn,
                          void        **sqlHandle )
{
    if (!conn || !sqlHandle) {
        return sapdbwa_False;
    }

    *sqlHandle = wd39SqlSession( conn->sqlHandle );

    return sapdbwa_True;
} /* wd31SqlHandle */


sapdbwa_Bool wd31IsAlive( twd31DbConnP  conn )
{
    if (!conn) {
        return sapdbwa_False;
    }

    return conn->Alive;
} /* wd31IsAlive */


sapdbwa_Bool wd31SetUserData( twd31DbConnP   conn,
                            sapdbwa_UInt4    userDllId,
                            void          *data,
                            sapdbwa_Destructor  destructor )
{
    if (!conn) {
        return sapdbwa_False;
    }

    return wd38NewUserData( &(conn->userDataList), userDllId, data, destructor );
} /* wd31SetUserData */

/*!**********************************************************************

  EndChapter: DbConnectionP

************************************************************************/

/*!**********************************************************************

  Chapter: internal functions

************************************************************************/

void wd31MakeConnectString( const char      *datasource,
                            const char      *driver,
                            const char      *host,
                            const char      *serverdb,
                            const char      *user,
                            const char      *password,
                            const char      *tracefile,
                            char            *connectString,
                            sapdbwa_UInt4    connectStringLen )
{

    char    myDriver[1024] = "";
    char    myHost[1024] = "";
    char    myServerDB[1024] = "";
    char    myUser[1024] = "";
    char    myPassword[1024] = "";
    char    myTracefile[1024] = "";

    if (datasource != NULL && strlen(datasource)>0) {        
        if (driver != NULL) {
            sp77sprintf(myDriver, 1024, "DSN=%s;DRIVER=%s", datasource, driver);
        } else {
            sp77sprintf(myDriver, 1024, "DSN=%s;DRIVER=", datasource);
        }
    } else {
        if (driver != NULL) {
            sp77sprintf(myDriver, 1024, "DRIVER=%s", driver);
        } else {
            sp77sprintf(myDriver, 1024, "DRIVER=");
        }
    };

    if (tracefile != NULL && strlen(tracefile) > 0) {
        sp77sprintf(myTracefile, 1024, ";TRACEFILENAME=%s", tracefile);
    }; /* if */

    if (host != NULL) {
        sp77sprintf(myHost, 1024, ";SERVERNODE=%s", host);
    }

    if (serverdb != NULL) {
        sp77sprintf(myServerDB, 1024, ";SERVERDB=%s", serverdb);
    }

    if (user != NULL) {
        sp77sprintf(myUser, 1024, ";UID=%s", user);
    }

    if (password != NULL) {
        sp77sprintf(myPassword, 1024, ";PWD=%s", password);
    }

    sp77sprintf(connectString, connectStringLen, "%s%s%s%s%s%s;",
                myDriver, myHost, myServerDB, myUser, myPassword, myTracefile);

}


/*!**********************************************************************

  EndChapter: internal functions

************************************************************************/

