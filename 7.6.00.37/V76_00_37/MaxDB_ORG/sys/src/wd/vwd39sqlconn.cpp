/*!**********************************************************************

  module: vwd39sqlconn.h

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  C-Wrapper for connect/disconnect with SQL-Class

  see also:

  -----------------------------------------------------------------------

  copyright:                Copyright (c) 1999-2005 SAP AG




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

/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1118259
#define USE_SQLCLASSES
*/

#ifdef USE_SQLCLASSES
#include <sqlcppclass.h>
#endif

#if defined(AIX)
#undef CONST_CAST
#undef REINTERPRET_CAST
#undef STATIC_CAST
#undef true
#undef false
#endif

#include "hwd39sqlconn.h"

struct st_sqlconn
{
#ifdef USE_SQLCLASSES
    SqlSession *session;
#else
    void *session; /* dummy */
#endif
}; /* struct sql_conn */


void wd39SqlInit()
{
#ifdef USE_SQLCLASSES
    SqlInit();
#endif
}; /* wd39SqlInit */


void wd39SqlFinish()
{
#ifdef USE_SQLCLASSES
    SqlFinish();
#endif
}; /* wd39SqlFinish */


void *wd39SqlSession( twd39SqlConnP conn )
{
    if (!conn) {
        return NULL;
    }

    return conn->session;
}; /* wd39SqlSession */


tsp00_Bool wd39AllocSqlConn( twd39SqlConnP *pconn, twd26ErrP err )
{
    if (!pconn) {
        return false;
    }

    tsp00_Bool allocOk = true;
    *pconn = new st_sqlconn();
    if (*pconn != NULL) {
        (*pconn)->session = NULL;
    } else { /* memory allocation error */
        allocOk = false;
        wd26SetErr( err, ERR_MEMORY_WD26, NULL, NULL );
    }; /* else */

    return allocOk;
}; /* wd39AllocSqlConn */


tsp00_Bool wd39FreeSqlConn( twd39SqlConnP conn )
{
    tsp00_Bool freeOk = true;

#ifdef USE_SQLCLASSES
    if (conn) {
        if (conn->session) {
            delete conn->session; // may be NULL
        }

        delete conn;
    }
#endif

    return freeOk;
}; /* wd39FreeSqlConn */


tsp00_Bool wd39SqlConnect( twd39SqlConnP  conn,
                           const char    *host,
                           const char    *serverdb,
                           const char    *user,
                           const char    *password,
                           SAPDB_Int4     isolation,
                           twd26ErrP      err )
{
    tsp00_Bool connectOk = true;

    if (!conn) {
        return false;
    }

#ifdef USE_SQLCLASSES
    try {
        conn->session = new SqlSession( user, password,
                                        serverdb, host,
                                        (SqlSession::IsoLevel) isolation );
    }
    catch (DbpError& dbpErr) {
        /* connect failed */
        char errNum[255];

        connectOk = false;
        conn->session = NULL;
        sp77sprintf(errNum, 255, "[%d]", dbpErr.dbpError());
        wd26SetErr( err, ERR_SQLCLASS_WD26, errNum, dbpErr.dbpErrorText() );
    }; /* catch */
#else
    connectOk = false;
#endif

    return connectOk;
}; /* wd39SqlConnect */


tsp00_Bool wd39SqlDisconnect( twd39SqlConnP conn, twd26ErrP err )
{
    tsp00_Bool disconnectOk = true;

    if (!conn) {
        return false;
    }

#ifdef USE_SQLCLASSES
    delete conn->session;
    conn->session = NULL;
#endif

    return disconnectOk;
}; /* wd39SqlDisconnect */

