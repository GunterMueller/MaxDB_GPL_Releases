/*



    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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





*/
&ifdef xINHOUSEx
/*!**********************************************************************

  module: hwd05wases.h -> sapdbwa_session.h

  -----------------------------------------------------------------------

  responsible:	Holger Bischoff
				Markus Özgen

  special area: SAPDB WebAgent Session Pool C Interface

  description:  Public description of WebAgent session pool C interface

  see also:     

  -----------------------------------------------------------------------

  copyright:          Copyright (c) 2000-2005 SAP AG

************************************************************************/
&endif

#ifndef SAPDBWASESSION_C_H
#define SAPDBWASESSION_C_H

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/
#include <WINDOWS.H>
#include <sql.h>
#include <sqlext.h>

#include <sapdbwa_define.h>
#include <sapdbwa_error.h>

#if defined(__cplusplus)
extern "C" {
#endif

/*!**********************************************************************

  Chapter: sapdbwa_SessionPoolP

  description:
  
************************************************************************/

typedef struct st_wa_sessionpool_handle	*sapdbwa_SessionPoolP;
typedef struct st_wa_connection_handle	*sapdbwa_DbConnectionP;

    
sapdbwa_SessionPoolP WINAPI sapdbwa_CreateSessionPool( char *name );

    
sapdbwa_Bool WINAPI sapdbwa_InitSessionPool( sapdbwa_SessionPoolP  pool,
                                      const char      *datasource,
                                      const char      *driver,
                                      const char      *serverNode,
                                      const char      *serverDb,
                                      const char      *user,
                                      const char      *password,
                                      sapdbwa_UInt4      poolSize,
                                      sapdbwa_Bool       autocommit,
                                      const char      *sqltrace );

    
sapdbwa_Bool WINAPI sapdbwa_InitSqlSessionPool( sapdbwa_SessionPoolP  pool,
                                         const char      *serverNode,
                                         const char      *serverDb,
                                         const char      *user,
                                         const char      *password,
                                         sapdbwa_Int4       isolation,
                                         sapdbwa_UInt4      poolSize );
    
    
void WINAPI sapdbwa_DestroySessionPool( sapdbwa_SessionPoolP pool );

    
sapdbwa_DbConnectionP WINAPI sapdbwa_DbConnect( sapdbwa_SessionPoolP pool );


sapdbwa_ErrP WINAPI sapdbwa_SesGetErr( sapdbwa_SessionPoolP  pool );

    
const char* WINAPI sapdbwa_GetServerNode( sapdbwa_SessionPoolP pool );


const char* WINAPI sapdbwa_GetServerDb( sapdbwa_SessionPoolP pool );


const char* WINAPI sapdbwa_GetUserName( sapdbwa_SessionPoolP pool );


const char* WINAPI sapdbwa_GetPassword( sapdbwa_SessionPoolP pool );


/*!**********************************************************************

  EndChapter: sapdbwa_SessionPoolP

************************************************************************/

/*!**********************************************************************

  Chapter: sapdbwa_DbConnectionP

  description:
  
************************************************************************/


sapdbwa_Bool WINAPI sapdbwa_DbDisconnect( sapdbwa_DbConnectionP conn );


sapdbwa_Bool WINAPI sapdbwa_OdbcHandle( sapdbwa_DbConnectionP  connection,
                                 SQLHENV          *phenv,
                                 SQLHDBC          *phdbc );


sapdbwa_Bool WINAPI sapdbwa_SqlHandle( sapdbwa_DbConnectionP  connection,
                                void            **sqlHandle );

    
void* WINAPI sapdbwa_GetUserData( sapdbwa_DbConnectionP conn );


sapdbwa_Bool WINAPI sapdbwa_SetUserData( sapdbwa_DbConnectionP  conn,
                                  void             *data,
                                  sapdbwa_Destructor     destructor );

    
sapdbwa_Bool WINAPI sapdbwa_CheckConnection( sapdbwa_DbConnectionP  conn,
                                      sapdbwa_Int4        nativeErr,
                                      const char       *sqlState,
                                      sapdbwa_Bool       *connectionDown );

sapdbwa_ErrP WINAPI sapdbwa_ConnGetErr( sapdbwa_DbConnectionP  pool );
    
/*!**********************************************************************

  EndChapter: sapdbwa_DbConnectionP

************************************************************************/

#if defined(__cplusplus)
}
#endif

#endif

