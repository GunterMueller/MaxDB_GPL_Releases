/*!**********************************************************************

  module: hwd106pool.h

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

#ifndef WD106POOL_H
#define WD106POOL_H

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "hwd33hconn.h"
#include "hwd34dbc.h"

/*!**********************************************************************

  Chapter: twd106ConnP

  description:
  
************************************************************************/

typedef struct st_conn *twd106ConnP;

/* exclusive is only used for dbc-connections. If it is true this
 * twd106ConnP gets an exclusive ODBC connection. This is importend
 * for write Operations to have non overlapping transactions.
 * For read operations exclusive may be false. */
twd106ConnP wd106CreateConn( sapdbwa_Bool exclusive );

void wd106DestroyConn( twd106ConnP conn );


sapdbwa_Bool wd106CheckConn( twd106ConnP  conn,
                           sapdbwa_Int4   nativeErr,
                           const char  *sqlState,
                           sapdbwa_Bool  *connectionDown );


sapdbwa_Bool wd106Disconnect( twd106ConnP conn );


sapdbwa_Bool wd106OdbcHandle( twd106ConnP     conn,
                            SQLHENV        *henv,
                            SQLHDBC        *hdbc );


void* wd106GetUserData( twd106ConnP conn );


sapdbwa_Bool wd106SetUserData( twd106ConnP   conn,
                             void         *data,
                             sapdbwa_Destructor  destructor );



const char *wd106GetUsername( twd106ConnP   conn );

sapdbwa_DBCP wd106GetDBCCon( twd106ConnP    hConn );

/*!**********************************************************************

  EndChapter: twd106ConnP

************************************************************************/

/*!**********************************************************************

  Chapter: twd106PoolP

  description:
  
************************************************************************/

typedef struct st_pool *twd106PoolP;

twd106PoolP wd106CreatePoolFromSes( sapdbwa_SessionPoolP sessionPool );


twd106PoolP wd106CreatePoolFromDbc( sapdbwa_DBCPoolP   dbcPool,
                                    const char  *datasource,
                                    const char  *driver,
                                    const char  *serverNode,
                                    const char  *serverDb,
                                    const char  *user,
                                    const char  *password,
                                    const char  *sqltrace );


void wd106DestroyPool( twd106PoolP pool );


void wd106SetDBCCon( twd106PoolP    pool,
                     sapdbwa_DBCP   dbc );

sapdbwa_Bool wd106Connect( twd106PoolP pool,
                         twd106ConnP conn,
                         twd26ErrP   err );

/*!**********************************************************************

  EndChapter: twd106Pool

************************************************************************/

#endif
