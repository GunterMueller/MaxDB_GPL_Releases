/*!**********************************************************************

  module: vwd32hses.c

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  Session pool handle

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

#include "hwd20control.h"
#include "hwd30ses.h"
#include "hwd31conn.h"
#include "hwd32hses.h"
#include "hwd33hconn.h"

#include "SAPDBCommon/SAPDB_string.h"

/*!**********************************************************************

  Chapter: sapdbwa_SessionPoolP

  description:
  
************************************************************************/

struct st_wa_sessionpool_handle
{
    sapdbwa_UInt4		userDllId;
    twd30SessionPoolP	sessionPool;
    twd26ErrP			err;
    sapdbwa_Bool		explicitCreated;	/* are we created by		*/
											/* WACreateSessionPool or	*/
											/* wd32CreatePoolHandle		*/
}; /* struct st_wa_sessionpool_handle */


sapdbwa_SessionPoolP WINAPI sapdbwa_CreateSessionPool( char *name )
{
    twd30SessionPoolP newPool = NULL;
    sapdbwa_SessionPoolP    newPoolHandle = NULL;
    twd26ErrP         err = wd26CreateErr();

    if (err != NULL) {
        newPool = wd30CreateSessionPool( name, err );
        if (newPool != NULL) {
            newPoolHandle = wd32CreatePoolHandle( newPool, 1 );
            if (newPoolHandle != NULL) {
                newPoolHandle->explicitCreated = true;
            } else {
                wd30DestroySessionPool( newPool );
            }; /* else */
        }; /* if */
        wd26DestroyErr( err );
    }; /* if */
    
    return newPoolHandle;
} /* sapdbwa_CreateSessionPool */

    
sapdbwa_Bool WINAPI sapdbwa_InitSessionPool( sapdbwa_SessionPoolP  pool,
                                      const char     *datasource,
                                      const char     *driver,
                                      const char     *serverNode,
                                      const char     *serverDb,
                                      const char     *user,
                                      const char     *password,
                                      sapdbwa_UInt4     poolSize,
                                      sapdbwa_Bool      autocommit,
                                      const char     *sqltrace )
{
    return wd30InitSessionPool( pool->sessionPool,
                                datasource,
                                driver,
                                serverNode,
                                serverDb,
                                user,
                                password,
                                poolSize,
                                autocommit,
                                SQL_TXN_READ_COMMITTED,
                                sqltrace,
                                pool->err );
} /* sapdbwa_InitSessionPool */
    

sapdbwa_Bool WINAPI sapdbwa_InitSqlSessionPool( sapdbwa_SessionPoolP  pool,
                                         const char     *serverNode,
                                         const char     *serverDb,
                                         const char     *user,
                                         const char     *password,
                                         sapdbwa_Int4      isolation,
                                         sapdbwa_UInt4     poolSize )
{
    return wd30InitSqlSessionPool( pool->sessionPool,
                                   serverNode, serverDb,
                                   user, password,
                                   isolation,
                                   poolSize, pool->err );
} /* sapdbwa_InitSqlSessionPool */


void WINAPI sapdbwa_DestroySessionPool( sapdbwa_SessionPoolP pool )
{
    /* implicitly allocated pool-handles are destroyed by the
     * WebAGent with wd32DestroyPoolHandle */
    if (pool->explicitCreated) {
        wd30DestroySessionPool( pool->sessionPool );
        wd32DestroyPoolHandle( pool );
    }; /* if */
} /* sapdbwa_DestroySessionPool */


sapdbwa_ErrP WINAPI sapdbwa_SesGetErr( sapdbwa_SessionPoolP  pool )
{
    return pool->err;
} /* sapdbwa_SesGetErr */


void WINAPI sapdbwa_SesCopyErrorMsg( sapdbwa_SessionPoolP  pool,
                                char            *buf,
                                sapdbwa_UInt4      bufLen )
{
    wd26CopyMsg( pool->err, buf, bufLen );
} /* sapdbwa_SesCopyErrorMsg */


const char* WINAPI sapdbwa_SesGetErrorMsg( sapdbwa_SessionPoolP pool )
{
    return wd26GetMsg( pool->err );
} /* sapdbwa_SesGetErrorMsg */


sapdbwa_UInt4 WINAPI sapdbwa_SesGetErrorId( sapdbwa_SessionPoolP pool )
{
    return wd26GetId( pool->err );
} /* sapdbwa_GetErrorId */


sapdbwa_SessionPoolP wd32CreatePoolHandle( twd30SessionPoolP pool,
                                      sapdbwa_UInt4       userDllId )
{
    sapdbwa_SessionPoolP newSes;
    sapdbwa_Bool     allocatOK;
    twd26ErrP      err = wd26CreateErr();
    
    sapdbwa_SQLALLOCAT( sizeof( struct st_wa_sessionpool_handle ),
                (sapdbwa_UInt1**) &newSes, &allocatOK );
    if (!allocatOK) {
        return NULL;
    }; /* if */

    newSes->userDllId = userDllId;
    newSes->sessionPool = pool;
    newSes->err = err;
    newSes->explicitCreated = false;
    
    return newSes;
} /* wd32CreateSessionPoolHandle */


void wd32DestroyPoolHandle( sapdbwa_SessionPoolP pool )
{
    wd26DestroyErr( pool->err );
    sapdbwa_SQLFREE( (sapdbwa_UInt1*) pool );
} /* wd32DestroySessionPoolHandle */


sapdbwa_SessionPoolP wd32ClonePoolHandle( sapdbwa_SessionPoolP source )
{
    sapdbwa_SessionPoolP newSes;
    sapdbwa_Bool allocatOK;

    sapdbwa_SQLALLOCAT( sizeof( struct st_wa_sessionpool_handle ),
                   (sapdbwa_UInt1**) &newSes, &allocatOK );
    if (!allocatOK) {
        return NULL;
    }; /* if */
    SAPDB_memcpy( newSes, source, sizeof( struct st_wa_sessionpool_handle ));
    /* the cloned handle is a copied one */
    newSes->explicitCreated = false;
    /* don't  share error object */
    newSes->err = wd26CreateErr();
    /* newSes->pool is shared with source */
    
    return newSes;
} /* wd32ClonePoolHandle */


sapdbwa_DbConnectionP WINAPI sapdbwa_DbConnect( sapdbwa_SessionPoolP pool )
{
    twd31DbConnP conn;
    sapdbwa_DbConnectionP newConn = NULL;

    conn = wd30DbConnect( pool->sessionPool, pool->err );
    if (conn==NULL) {
        return NULL;
    }; /* if */
    newConn = wd33CreateConnHandle( conn, pool->userDllId );

    return newConn;
} /* sapdbwa_DbConnect */


sapdbwa_DbConnectionP wd32DbConnect( sapdbwa_SessionPoolP pool, twd26ErrP err )
{
    twd31DbConnP conn;
    sapdbwa_DbConnectionP newConn = NULL;

    conn = wd30DbConnect( pool->sessionPool, err );
    if (conn==NULL) {
        return NULL;
    }; /* if */
    newConn = wd33CreateConnHandle( conn, pool->userDllId );

    return newConn;
} /* wd32DbConnect */


const char* WINAPI sapdbwa_GetServerNode( sapdbwa_SessionPoolP pool )
{
    return wd30GetServerNode( pool->sessionPool );
} /* sapdbwa_GetServerNode */


const char* WINAPI sapdbwa_GetServerDb( sapdbwa_SessionPoolP pool )
{
    return wd30GetServerDb( pool->sessionPool );
} /* sapdbwa_GetServerDb */


const char* WINAPI sapdbwa_GetUserName( sapdbwa_SessionPoolP pool )
{
    return wd30GetUser( pool->sessionPool );
} /* sapdbwa_GetUserName */


const char* WINAPI sapdbwa_GetPassword( sapdbwa_SessionPoolP pool )
{
    return wd30GetPassword( pool->sessionPool );
} /* sapdbwa_GetUserName */


/*!**********************************************************************

  EndChapter: SessionPoolHandleP

************************************************************************/
