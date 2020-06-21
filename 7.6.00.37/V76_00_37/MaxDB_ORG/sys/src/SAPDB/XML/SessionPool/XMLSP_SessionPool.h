/*!**********************************************************************

  module: XMLSP_SessionPool.h

  -----------------------------------------------------------------------

  responsible:	Markus Oezgen

  special area: XML Session Pool

  description:  Session Pool Management

  see also:

  -----------------------------------------------------------------------

                         Copyright (c) 2001-2005 SAP AG



    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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

#ifndef XMLSP_SESSION_POOL_H
#define XMLSP_SESSION_POOL_H

/***********************************************************************

	Includes

 ***********************************************************************/

#include "SAPDB/SAPDBCommon/SAPDB_Types.h"

/*---------------------------------------------------------------------------*/

#include "SAPDB/XML/SessionPool/XMLSP_ErrorCodes.h"

/*---------------------------------------------------------------------------*/

#include "SAPDB/XML/Error/XMLError_Error.h"
#include "SAPDB/XML/Library/XMLLib_Library.h"

/*---------------------------------------------------------------------------*/

/***********************************************************************

	Defines

 ***********************************************************************/

#define XMLSP_MAX_DB_SERVER_NODE_LEN      127
#define XMLSP_MAX_DB_SERVER_NAME_LEN      127
#define XMLSP_MAX_DB_USER_LEN             127
#define XMLSP_MAX_DB_PASSWORD_LEN         127
#define XMLSP_MAX_DB_TRACE_FILENAME_LEN   1023

/***********************************************************************

	Structs & Types

 ***********************************************************************/

typedef char  XMLSP_DBServerNodeString[XMLSP_MAX_DB_SERVER_NODE_LEN+1];
typedef char *XMLSP_DBServerNode;

typedef char  XMLSP_DBServerNameString[XMLSP_MAX_DB_SERVER_NAME_LEN+1];
typedef char *XMLSP_DBServerName;

typedef char  XMLSP_DBUserString[XMLSP_MAX_DB_USER_LEN+1];
typedef char *XMLSP_DBUser;

typedef char  XMLSP_DBPasswordString[XMLSP_MAX_DB_PASSWORD_LEN+1];
typedef char *XMLSP_DBPassword;

typedef char  XMLSP_DBTraceFilenameString[XMLSP_MAX_DB_TRACE_FILENAME_LEN+1];
typedef char *XMLSP_DBTraceFilename;

typedef SAPDB_UInt2 XMLSP_SessionPoolSize;

/*---------------------------------------------------------------------------*/

typedef struct st_xmlsp_session_pool	*XMLSP_SessionPool;
typedef struct st_xmlsp_session	        *XMLSP_Session;

/*---------------------------------------------------------------------------*/

typedef void (*XMLSP_DestroySessionUserData)( void *userData );

/*---------------------------------------------------------------------------*/

/***********************************************************************

	Prototypes

 ***********************************************************************/

SAPDB_Bool SP_Create( XMLSP_SessionPool     *hSessionPool, 
                      XMLSP_DBServerNode     serverNode,
                      XMLSP_DBServerName     serverName,
                      XMLSP_DBUser           user,
                      XMLSP_DBPassword       password,
                      XMLSP_DBTraceFilename  traceFilename,
                      XMLSP_SessionPoolSize  sessionPoolSize,
                      XMLError_Error         hError );

SAPDB_Bool SP_Destroy( XMLSP_SessionPool hSessionPool );

SAPDB_Bool SP_GetSession( XMLSP_SessionPool  hSessionPool,
                          XMLSP_Session     *hSession,
                          XMLError_Error     hError );

SAPDB_Bool SP_PutSession( XMLSP_SessionPool hSessionPool,
                          XMLSP_Session     hSession,
                          XMLError_Error    hError );

SAPDB_Bool SP_OpenSession( XMLSP_Session        *hSession,
                           XMLSP_DBServerNode    serverNode,
                           XMLSP_DBServerName    serverName,
                           XMLSP_DBUser          user,
                           XMLSP_DBPassword      password,
                           XMLSP_DBTraceFilename traceFilename,
                           XMLError_Error        hError );

SAPDB_Bool SP_CloseSession( XMLSP_Session  hSession,
                            XMLError_Error hError );

XMLSP_DBServerName      SP_GetServerNameOfSession   ( XMLSP_Session hSession);
XMLSP_DBServerNode      SP_GetServerNodeOfSession   ( XMLSP_Session hSession);
XMLSP_DBUser            SP_GetUserOfSession         ( XMLSP_Session hSession);
XMLSP_DBPassword        SP_GetPasswordOfSession     ( XMLSP_Session hSession);
XMLSP_DBTraceFilename   SP_GetTraceFilenameOfSession( XMLSP_Session hSession);
SQLHENV					SP_GetHEnv( XMLSP_Session hSession);
SQLHDBC					SP_GetHDBC( XMLSP_Session hSession);
void*                   SP_GetUserData( XMLSP_Session hSession );
void                    SP_SetUserData( XMLSP_Session hSession, void *userData, XMLSP_DestroySessionUserData destroyFunction );

#define SP_CancelTransaction(a,b)   SP_StartTransaction(a,b)
SAPDB_Bool SP_StartTransaction( XMLSP_Session   hSession,
                                XMLError_Error  hError );
SAPDB_Bool SP_EndTransaction( XMLSP_Session  hSession,
                              XMLError_Error hError );

/***********************************************************************

	End

 ***********************************************************************/

#endif
