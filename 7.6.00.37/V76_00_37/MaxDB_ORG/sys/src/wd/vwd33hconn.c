/*!**********************************************************************

  module: vwd33hconn.c

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  Connection handle

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
#include "hwd31conn.h"
#include "hwd33hconn.h"

/*!**********************************************************************

  Chapter: sapdbwa_DbConnectionP

************************************************************************/

struct st_wa_connection_handle
{
    sapdbwa_UInt4	userDllId;
    twd31DbConnP	conn;
    twd26ErrP		err;
}; /* struct st_wa_connection_handle */


sapdbwa_Bool WINAPI sapdbwa_DbDisconnect( sapdbwa_DbConnectionP hconn )
{
    if(hconn) {
        wd31DbDisconnect( hconn->conn );
        wd33DestroyConnHandle( hconn );

        return true;
    } else {
        return sapdbwa_False;
    }
} /* sapdbwa_DbDisconnect */


sapdbwa_Bool WINAPI sapdbwa_OdbcHandle( sapdbwa_DbConnectionP  hconn,
                                 SQLHENV         *phenv,
                                 SQLHDBC         *phdbc )
{
    if (!hconn) {
        return sapdbwa_False;
    } else {
        return wd31OdbcHandle( hconn->conn, phenv, phdbc );
    }
} /* sapdbwa_OdbcHandle */


sapdbwa_Bool WINAPI sapdbwa_SqlHandle( sapdbwa_DbConnectionP   hconn,
                                void             **sqlHandle )
{
    if (hconn) {
        return wd31SqlHandle( hconn->conn, sqlHandle );
    } else {
        return sapdbwa_False;
    }
} /* sapdbwa_OdbcHandle */


void* WINAPI sapdbwa_GetUserData( sapdbwa_DbConnectionP hconn )
{
    if (hconn) {
        return wd31GetUserData( hconn->conn, hconn->userDllId );
    } else {
        return NULL;
    }
} /* sapdbwa_GetUserData */


sapdbwa_Bool WINAPI sapdbwa_SetUserData( sapdbwa_DbConnectionP  hconn,
                                  void             *data,
                                  sapdbwa_Destructor     destructor )
{
    if (hconn) {
        return wd31SetUserData( hconn->conn,
                                hconn->userDllId,
                                data,
                                destructor );
    } else {
        return sapdbwa_False;
    }
} /* sapdbwa_SetUserData */


sapdbwa_Bool WINAPI sapdbwa_CheckConnection( sapdbwa_DbConnectionP  conn,
                                      sapdbwa_Int4        nativeErr,
                                      const char       *sqlState,
                                      sapdbwa_Bool       *connectionDown )
{
    if (conn) {
        return wd31CheckConnection( conn->conn, nativeErr, sqlState,
                                    connectionDown, conn->err );
    } else {
        return sapdbwa_False;
    }
} /* sapdbwa_CheckConnection */


sapdbwa_ErrP WINAPI sapdbwa_ConnGetErr( sapdbwa_DbConnectionP conn )
{
    if (conn) {
        return conn->err;
    } else {
        return NULL;
    }
} /* sapdbwa_ConnGetErr */


const char* WINAPI sapdbwa_ConnGetErrorMsg( sapdbwa_DbConnectionP conn )
{
    if (conn) {
        return wd26GetMsg( conn->err );
    } else {
        return NULL;
    }
} /* sapdbwa_ConnGetErrorMsg */

    
void WINAPI sapdbwa_ConnCopyErrorMsg( sapdbwa_DbConnectionP  conn,
                                 char             *buf,
                                 sapdbwa_UInt4       bufLen )
{
    if (conn) {
        wd26CopyMsg( conn->err, buf, bufLen );
    }
} /* sapdbwa_ConnCopyErrorMsg */

    
sapdbwa_UInt4 WINAPI sapdbwa_ConnGetErrorId( sapdbwa_DbConnectionP conn )
{
    if (conn) {
        return wd26GetId( conn->err );
    } else {
        return 0;
    }
} /* sapdbwa_ConnGetErrorId */


sapdbwa_DbConnectionP wd33CreateConnHandle( twd31DbConnP  conn,
                                       sapdbwa_UInt4   userDllId )
{
    sapdbwa_DbConnectionP newConn;
    sapdbwa_Bool allocatOK;

    sapdbwa_SQLALLOCAT( sizeof( struct st_wa_connection_handle ),
                (sapdbwa_UInt1**) &newConn, &allocatOK );
    if (!allocatOK) {
        return NULL;
    }; /* if */
    newConn->err = wd26CreateErr();
    if (newConn->err == NULL) {
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) newConn );
        return NULL;
    }; /* if */
    
    newConn->userDllId = userDllId;
    newConn->conn = conn;
    
    return newConn;
} /* wd33CreateConnHandle */


void wd33DestroyConnHandle( sapdbwa_DbConnectionP hconn )
{
    if (hconn) {
        wd26DestroyErr( hconn->err );
        sapdbwa_SQLFREE( (sapdbwa_UInt1*) hconn );
    }
} /* wd33DestroyConnHandle */


twd31DbConnP wd33GetConnection( sapdbwa_DbConnectionP hconn )
{
    if (hconn) {
        return hconn->conn;
    } else {
        return NULL;
    }
} /* wd33GetConnection */

/*!**********************************************************************

  EndChapter: sapdbwa_DbConnectionP

************************************************************************/
