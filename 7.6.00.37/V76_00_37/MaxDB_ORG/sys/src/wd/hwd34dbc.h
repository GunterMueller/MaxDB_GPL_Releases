/*!**********************************************************************

  module: hwd34dbc.c

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  Persistent ODBC connections

  see also:

  -----------------------------------------------------------------------

						Copyright (c) 1999-2005 SAP AG



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

#ifndef WD34DBC_H
#define WD34DBC_H

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "hwd08dbc.h"

typedef struct st_wa_dbc_connect_param
{
    sapdbwa_Bool  paramSet;
    char       *datasource;
    char       *driver;
    char       *serverNode;
    char       *serverDb;
    char       *user;
    char       *password;
    char       *sqltrace;
} twd34ConnectParam;

void wd34InitConnectParam( twd34ConnectParam *connectParam );

sapdbwa_Bool wd34Connect( sapdbwa_DBCPoolP       dbcPool,
                          sapdbwa_DBCP           dbc,
                          const char            *datasource,
                          const char            *driver,
                          const char            *serverNode,
                          const char            *serverDb,
                          const char            *user,
                          const char            *password,
                          const char            *sqltrace,
                          sapdbwa_UInt2          driverCompletion,
                          SQLHWND                windowHandle,
                          sapdbwa_DBCExclType    exclusive,
                          SAPDB_Bool             ignoreUser,
                          twd26ErrP              err );

sapdbwa_Bool wd34SetConnect( twd34ConnectParam *connectParam,
                           const char       *datasource,
                           const char       *driver,
                           const char       *serverNode,
                           const char       *serverDb,
                           const char       *user,
                           const char       *password,
                           const char       *sqltrace );

void wd34FreeConnectParam( twd34ConnectParam *connectParam );

    
void* WINAPI wd34GetUserData( sapdbwa_DBCP conn, sapdbwa_UInt4 id );


sapdbwa_Bool WINAPI wd34SetUserData( sapdbwa_DBCP           conn,
                                   sapdbwa_UInt4       id,
                                   void             *data,
                                   sapdbwa_Destructor     destructor );

sapdbwa_Bool WINAPI wd34IsPoolElemNull(  sapdbwa_DBCP  conn );

/* destroys the UserData of all free connections */
/* void wd34DestroyUserData( WADBCPoolP dbcPool ); */

#endif
