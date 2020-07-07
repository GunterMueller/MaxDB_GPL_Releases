/*!**********************************************************************

  module: hwd31conn.h

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

#ifndef HWD31CONN_H
#define HWD31CONN_H

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/
#include <WINDOWS.H>
#include <sql.h>
#include <sqlext.h>

#include "gwd00.h"
#include "hwd05wases.h"
#include "hwd26err.h"

#if defined(__cplusplus)
extern "C" {
#endif

/*!**********************************************************************

  Chapter: twd31DbConnP

  description:
  
************************************************************************/

typedef struct st_wa_db_connection	*twd31DbConnP;

typedef struct st_wa_sessionpool	*twd30SessionPoolP;

twd31DbConnP wd31CreateConn( sapdbwa_UInt4        connId,
                             twd30SessionPoolP  pool,
                             twd26ErrP          err );


void wd31DestroyConn( twd31DbConnP conn );

sapdbwa_Bool  wd31CheckConnection( twd31DbConnP     conn,
                                 sapdbwa_Int4       nativeErr,
                                 const char      *sqlState,
                                 sapdbwa_Bool      *connectionDown,
                                 twd26ErrP        err );
    
/* open ODBC connection */    
sapdbwa_Bool wd31Connect( twd31DbConnP   conn,
                        const char    *datasource,
                        const char    *driver,
                        const char    *host,
                        const char    *serverdb,
                        const char    *user,
                        const char    *password,
                        sapdbwa_Bool     autocommit,
                        sapdbwa_Int4     isolation,
                        const char    *sqltrace,
                        twd26ErrP      err );


/* open SQL connection */    
sapdbwa_Bool wd31SqlConnect( twd31DbConnP   conn,
                           const char    *host,
                           const char    *serverdb,
                           const char    *user,
                           const char    *password,
                           sapdbwa_Int4     isolation,
                           twd26ErrP      err );

sapdbwa_Bool wd31DbDisconnect( twd31DbConnP conn );

sapdbwa_UInt4 wd31GetConnId( twd31DbConnP  conn );

void* wd31GetUserData( twd31DbConnP  conn, sapdbwa_UInt4  userDllId );


sapdbwa_Bool wd31IsAlive( twd31DbConnP  conn );

sapdbwa_Bool wd31OdbcHandle( twd31DbConnP  conn,
                           SQLHENV      *phenv,
                           SQLHDBC      *phdbc  );

sapdbwa_Bool wd31SqlHandle( twd31DbConnP  conn,
                          void        **sqlHandle );

sapdbwa_Bool wd31SetUserData( twd31DbConnP   conn,
                            sapdbwa_UInt4    userDllId,
                            void          *data,
                            sapdbwa_Destructor  destructor );


/* helper function */
void wd31MakeConnectString( const char  *datasource,
                            const char  *driver,
                            const char  *host,
                            const char  *serverdb,
                            const char  *user,
                            const char  *password,
                            const char  *tracefile,
                            char        *connectString,
                            sapdbwa_UInt4  len );

/*!**********************************************************************

  EndChapter: twd31DbConnP

************************************************************************/

#if defined(__cplusplus)
} /* extern "C" */
#endif

#endif
