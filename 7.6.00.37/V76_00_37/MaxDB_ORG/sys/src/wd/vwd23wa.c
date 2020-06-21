/*!**********************************************************************

  module: vwd23wa.c

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  sapdbwa_Handle functions

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

/****************************************************************************
 ***
 *** Include
 ***
 ****************************************************************************/

#include "hwd03waapi.h"
#include "hwd12hpool.h"
#include "hwd20control.h"
#include "hwd23wa.h"
#include "hwd32hses.h"
#include "hwd40user.h"

#include "SAPDBCommon/SAPDB_string.h"

/****************************************************************************
 ***
 *** Defines
 ***
 ****************************************************************************/

/****************************************************************************
 ***
 *** Types & Structs
 ***
 ****************************************************************************/

struct st_wa_handle
{
    twd40UserDllP         userDll;
    sapdbwa_SessionPoolP  poolHandle;

    /* web session data */
    twd37WebSessionContP  sessionCont; /* container of active WebSessions */
    sapdbwa_WebSessionP   webSession;  /* actual web-session if one exists */
    twd37SessionId        sessionId;
    sapdbwa_Bool          ownedWebSession; /* shows that the websession is "owned"
                                              (locked) by this thread */
    sapdbwa_Bool          timedOut;  /* true if web session timed out.
                                      * webSession is NULL in this case */
    twd26ErrP             err;
}; /* struct st_webagent */

/****************************************************************************
 ***
 *** Prototypes
 ***
 ****************************************************************************/

void * wd23_WAHandleConstructor();

void wd23_WAHandleDestructor( void *waHandle );

void wd23_WAHandleInit( sapdbwa_Handle             wa,
                        twd40UserDllP         userDll,
                        twd37WebSessionContP  sessionCont,
                        sapdbwa_WebSessionP        webSession,
                        char                 *sessionId,
                        sapdbwa_Bool            timedOut );

/****************************************************************************
 ***
 *** Variables
 ***
 ****************************************************************************/

static twd12HandlePoolP  wd23waHandlePool = NULL;

/***********************************************************************

	Public Functions

 ***********************************************************************/

const char* WINAPI sapdbwa_GetServiceName( sapdbwa_Handle   wa )
{
    return wd40GetPathPrefix(wa->userDll);
}

/*===========================================================================*/

/*!**********************************************************************

  Chapter: WAWebAgentP (functions from hwd03waapi.h)

  description:
  
************************************************************************/

sapdbwa_Bool wd23CreateHandlePool()
{
    wd23waHandlePool = wd12CreateHandlePool( wd23_WAHandleConstructor,
                                             wd23_WAHandleDestructor );

    return (wd23waHandlePool != NULL);
} /* wd23CreateWAHandlePool */


void wd23DestroyHandlePool()
{
    wd12DestroyHandlePool( wd23waHandlePool );
} /* wd23DestroyWAHandlePool */

/*===========================================================================*/

sapdbwa_Handle wd23CreateHandle( twd40UserDllP         userDll,
                             twd37WebSessionContP  sessionCont,
                             sapdbwa_WebSessionP         webSession,
                             char                 *sessionId,
                             sapdbwa_Bool            timedOut )
{
    sapdbwa_Handle     newWAHandle = NULL;

    newWAHandle = wd12GetHandle( wd23waHandlePool );
    if (newWAHandle != NULL) {
        wd23_WAHandleInit( newWAHandle,
                           userDll,
                           sessionCont,
                           webSession,
                           sessionId,
                           timedOut );
    };
    
    return newWAHandle;
} /* wd23CreateWAHandle */

/*===========================================================================*/

void wd23DestroyHandle( sapdbwa_Handle wa )
{

    /* Checks */
    if ( !wa )
        return;

    /* Unlock web session */
    if ( wa->ownedWebSession && wa->sessionCont && wa->webSession ) {
        wd37UnlockWebSession( wa->sessionCont, wa->webSession );
        /* unlock passed, so reset the owned flag */
        wa->ownedWebSession = false;
    } /* if */
 
    /* Put handle back into pool */
    wd12PutHandle( wd23waHandlePool, wa );    

} /* wd23DestroyHandle */

/*===========================================================================*/

const char * wd23SessionId( sapdbwa_Handle wa )
{

    /* Checks */
    if ( !wa )
        return NULL;

    return wa->sessionId;

} /* wd23SessionId */

twd26ErrP wd23GetErr( sapdbwa_Handle wa )
{
    return wa->err;
} /* wd23GetErr */

/*===========================================================================*/

sapdbwa_Bool sapdbwa_GetParam( sapdbwa_Handle    wa,
                        const char  *key,
                        char        *valueBuf,
                        sapdbwa_Int4     bufLen )
{

    return wd20GetParam( wd40GetUserDllName( wa->userDll ),
                         key,
                         valueBuf,
                         bufLen,
                         sapdbwa_False );
} /* sapdbwa_GetParam */

/***************************************************************************
 ***
 ***    sapdbwa_GetDocumentRoot
 ***
 ***************************************************************************/

sapdbwa_Bool sapdbwa_GetDocumentRoot( sapdbwa_Handle  wa,
                                      char           *valueBuf,
                                      sapdbwa_Int4    bufLen )
{

    return wd20GetDocumentRoot( valueBuf, bufLen );

}

/***************************************************************************
 ***
 ***
 ***
 ***************************************************************************/

sapdbwa_SessionPoolP WINAPI sapdbwa_GetSessionPool( sapdbwa_Handle wa )
{
    twd30SessionPoolP pool;

    wd26ResetErr( wa->err );
    if (wa->poolHandle != NULL) {
        /* we already allocated a pool-handle */
        return wa->poolHandle;
    }; /* if */
    
    pool = wd40GetSessionPool( wa->userDll );
    if (pool == NULL) {
        return NULL;
    }; /* if */
    
    wa->poolHandle = wd32CreatePoolHandle( pool,
                                           wd40GetUserDllId( wa->userDll ));

    return wa->poolHandle;
} /* sapdbwa_GetSessionPool */

/*===========================================================================*/

sapdbwa_Bool WINAPI sapdbwa_WriteLogMsg( sapdbwa_Handle wa, const char *msg )
{
    wd26ResetErr( wa->err );
    return wd40WriteLogMsg( wa->userDll, msg );
} /* sapdbwa_WriteLogMsg */

/*===========================================================================*/

sapdbwa_WebSessionP WINAPI sapdbwa_StartWebSession( sapdbwa_Handle      wa,
                                          sapdbwa_UInt4   timeOut )
{
    wd26ResetErr( wa->err );
    if (wd40GetUseWebSessions( wa->userDll )) {
        if (wa->webSession == NULL) {
            /* no WebSession started yet. */
            wa->webSession =
                wd37StartWebSession( wa->sessionCont,
                                     wa->sessionId,
                                     wd40GetUserDllId( wa->userDll ),
                                     timeOut );
            /* Lock session explicitely */
            if ((wa->webSession != NULL) && !wa->ownedWebSession) {
                if ( !wd37LockWebSession ( wa->sessionCont, wa->webSession )) {
                    wd37CloseWebSession( wa->sessionCont, wa->webSession, sapdbwa_True );
                    return NULL;
                } else {
                    wa->ownedWebSession = true;
                } /* else */

            }; /* if */
        }; /* if */
    } else {
        wa->webSession = NULL;
        wd26SetErr( wa->err, ERR_WEBSESSIONS_DISABLED_WD26,
                    wd40GetUserDllName( wa->userDll ), NULL );
    }; /* else */

    return wa->webSession;
} /* sapdbwa_StartWebSession */

/*===========================================================================*/

sapdbwa_WebSessionP WINAPI sapdbwa_StartNewWebSession( sapdbwa_Handle     wa,
                                                       sapdbwa_HttpReplyP rep,
                                                       sapdbwa_UInt4      timeOut )
{
  wd26ResetErr( wa->err );

    if (wd40GetUseWebSessions( wa->userDll )) {
        if (wa->webSession != NULL) {
            /* Oops, there is a websession. I don't need it, but perhaps
               somebody else needs this session later.
               -> only unlock                                        
             */
            if (wa->ownedWebSession) {
                wd37UnlockWebSession( wa->sessionCont, wa->webSession );
                wa->ownedWebSession = false;
            } /* if */
            wa->webSession = NULL;
        } /* if */

        /* create new session id */
        wd37GetNewSessionId( NULL, wa->sessionId );

        /* set the cookie with new session id */
        if (sapdbwa_ReplaceCookieValue(rep, SID_COOKIE_WD00, wa->sessionId)) {
        
            /* create new session object */
            wa->webSession =
                wd37StartWebSession( wa->sessionCont,
                                     wa->sessionId,
                                     wd40GetUserDllId( wa->userDll ),
                                     timeOut );
            /* lock session now */
            if (wa->webSession != NULL) {
                if ( !wd37LockWebSession ( wa->sessionCont, wa->webSession )) {
                    wd37CloseWebSession( wa->sessionCont, wa->webSession, sapdbwa_True );
                    return NULL;
                } else {
                    wa->ownedWebSession = true;
                } /* else */
            } /* if */
        } /* if */
    } else {
        wa->webSession = NULL;
        wd26SetErr( wa->err, ERR_WEBSESSIONS_DISABLED_WD26,
                    wd40GetUserDllName( wa->userDll ), NULL );
    } /* else */

    return wa->webSession;
} /* sapdbwa_StartNewWebSession */

/*===========================================================================*/

sapdbwa_WebSessionP WINAPI sapdbwa_StartDefaultWebSession( sapdbwa_Handle  wa )
{

    sapdbwa_UInt4   timeout = 0;

    /* Get timeout */
    timeout = wd40GetPropertyWebSessionTimeout( wa->userDll );

    /* Start web session */
    return sapdbwa_StartWebSession( wa, timeout );

} /* sapdbwa_StartDefaultWebSession */

/*===========================================================================*/

sapdbwa_WebSessionP WINAPI sapdbwa_GetWebSession( sapdbwa_Handle wa )
{

    wd26ResetErr( wa->err );
    if (wa->webSession != NULL) {
        /* Lock web session */
        if (!wa->ownedWebSession) {
            if ( wd37LockWebSession( wa->sessionCont, wa->webSession )) {
                wa->ownedWebSession = true;
                return wa->webSession;
            } else {
                wd26SetErr( wa->err, ERR_WEBSESSION_LOCKED_WD26, NULL, NULL );
            }; /* else */
        } else {
            return wa->webSession;
        }; /* else */
    } else if (wa->timedOut) {        
        wd26SetErr( wa->err, ERR_WEBSESSION_TIMEOUT_WD26, NULL, NULL );
    } else {
        wd26SetErr( wa->err, ERR_NO_WEBSESSION_WD26, NULL, NULL );
    }; /* else */
    
    return NULL;

} /* sapdbwa_GetWebSession */

/*===========================================================================*/

void WINAPI sapdbwa_ReleaseWebSession( sapdbwa_Handle wa )
{

    wd26ResetErr( wa->err );
    if (wa->webSession != NULL) {
        /* Unlock web session */
        if (wa->ownedWebSession) {
            if ( wd37UnlockWebSession( wa->sessionCont, wa->webSession )) {
                wa->ownedWebSession = false;
            } /* if */
        } /* if */
    } /* if */

} /* sapdbwa_ReleaseWebSession */

/*===========================================================================*/

sapdbwa_Bool WINAPI sapdbwa_SetUseWebSessions( sapdbwa_Handle wa, sapdbwa_Bool useWebSessions )
{
    wd26ResetErr( wa->err );
    return wd40SetUseWebSessions( wa->userDll, useWebSessions );
} /* sapdbwa_SetUseWebSessions */

/*===========================================================================*/

sapdbwa_Bool WINAPI sapdbwa_CloseWebSession( sapdbwa_Handle      wa,
											 sapdbwa_WebSessionP wses,
											 sapdbwa_Bool		 freeUserData )
{

    wd26ResetErr( wa->err );

    if ( !wd37CloseWebSession( wa->sessionCont, wses, freeUserData ))
        return sapdbwa_False;

    /* Web session is undef */
    sapdbwa_ASSERT( wa->webSession == wses);
    wa->webSession = NULL;

    return sapdbwa_True;

}

/*===========================================================================*/

sapdbwa_ErrP WINAPI sapdbwa_HandleGetErr( sapdbwa_Handle wa )
{
    return wa->err;
} /* sapdbwa_HandleGetErr */

/*!**********************************************************************

  EndChapter: WAWebAgentP (functions from hwd03waapi.h)

************************************************************************/

void * wd23_WAHandleConstructor()
{
    sapdbwa_Handle     newWAHandle;
    sapdbwa_Bool   allocatOK;
    
    sapdbwa_SQLALLOCAT( sizeof(struct st_wa_handle),
                (sapdbwa_UInt1**) &newWAHandle,
                &allocatOK );
    if (allocatOK) {
        newWAHandle->poolHandle      = NULL;
        newWAHandle->err        = wd26CreateErr();
        if (newWAHandle->err == NULL) {
            sapdbwa_SQLFREE( (sapdbwa_UInt1*) newWAHandle );
            return NULL;
        }; /* if */
        return newWAHandle;
    } else {
        return NULL;
    }; /* else */
} /* wd23_WAHandleConstructor */

/*===========================================================================*/

void wd23_WAHandleDestructor( void *waHandle )
{

    sapdbwa_Handle wa = (sapdbwa_Handle) waHandle;
    
    wd26DestroyErr( wa->err );
    if (wa->poolHandle != NULL) {
        wd32DestroyPoolHandle( wa->poolHandle );
    }; /* if */

    /* Unlock web session */
    if ( wa->ownedWebSession && wa->sessionCont && wa->webSession ) {
        wd37UnlockWebSession( wa->sessionCont, wa->webSession );
        wa->ownedWebSession = false;
    } /* if */

    sapdbwa_SQLFREE( (sapdbwa_UInt1*) wa );

} /* wd23_WAHandleDestructor */

/*===========================================================================*/

void wd23_WAHandleInit( sapdbwa_Handle              wa,
                        twd40UserDllP         userDll,
                        twd37WebSessionContP  sessionCont,
                        sapdbwa_WebSessionP         webSession,
                        char                 *sessionId,
                        sapdbwa_Bool            timedOut )
{
    wa->userDll         = userDll;
    wa->sessionCont     = sessionCont;
    wa->webSession      = webSession;
    wa->timedOut        = timedOut;
    wa->ownedWebSession = false;

    if (sessionId != NULL) {
        SAPDB_memcpy( &wa->sessionId[0],
                &sessionId[0], sizeof( twd37SessionId ));
    } else {
        wa->sessionId[0] = '\0';
    }; /* else */
    if (wa->poolHandle != NULL) {
        wd32DestroyPoolHandle( wa->poolHandle );
        wa->poolHandle  = NULL;
    }; /* if */
    wd26ResetErr( wa->err );
} /* wd23_WAHandleInit */

/*===========================================================================*/

/***********************************************************************

	End

 ***********************************************************************/
