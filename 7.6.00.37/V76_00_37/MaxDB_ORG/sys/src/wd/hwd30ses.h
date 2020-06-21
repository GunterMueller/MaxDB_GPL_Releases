/*!**********************************************************************

  module: hwd30ses.h

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

#ifndef HWD30SES_H
#define HWD30SES_H

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/
#include <WINDOWS.H>
#include <sql.h>
#include <sqlext.h>

#include "gwd00.h"
#include "hwd03waapi.h"
#include "hwd26err.h"
#include "hwd31conn.h"

#if defined(__cplusplus)
extern "C" {
#endif

/*!**********************************************************************

  Chapter: SessionPoolP

  description:
  
************************************************************************/
/*
typedef struct st_wa_sessionpool *twd30SessionPoolP;
*/
twd30SessionPoolP wd30CreateSessionPool( const char *name, twd26ErrP err );


void wd30DestroySessionPool( twd30SessionPoolP pool );


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
                                twd26ErrP          err );

sapdbwa_Bool wd30InitSqlSessionPool( twd30SessionPoolP  pool,
                                   const char        *serverNode,
                                   const char        *serverDb,
                                   const char        *user,
                                   const char        *password,
                                   sapdbwa_Int4         isolation,
                                   sapdbwa_Int4         poolSize,
                                   twd26ErrP          err );
    
twd31DbConnP wd30DbConnect( twd30SessionPoolP pool, twd26ErrP err );

sapdbwa_Bool wd30DbDisconnect( twd30SessionPoolP pool, twd31DbConnP conn );

sapdbwa_Bool wd30GetConf( twd30SessionPoolP  pool,
                        char              *textBuffer,
                        sapdbwa_UInt4       *bufferLen );
    
SQLHENV wd30GetHenv( twd30SessionPoolP pool );

char* wd30GetName( twd30SessionPoolP pool );

char* wd30GetServerNode( twd30SessionPoolP pool );

char* wd30GetServerDb( twd30SessionPoolP pool );

char* wd30GetUser( twd30SessionPoolP pool );

char* wd30GetPassword( twd30SessionPoolP pool );

sapdbwa_Bool wd30Reconnect( twd30SessionPoolP  pool,
                          twd31DbConnP       conn,
                          twd26ErrP          err );
    
/*!**********************************************************************

  EndChapter: SessionPoolP

************************************************************************/

/*!**********************************************************************

  Chapter: SessionPoolP inline functions

************************************************************************/

/*!**********************************************************************

  EndChapter: SessionPoolP inline functions

************************************************************************/

#if defined(__cplusplus)
} /* extern "C" */
#endif

#endif


