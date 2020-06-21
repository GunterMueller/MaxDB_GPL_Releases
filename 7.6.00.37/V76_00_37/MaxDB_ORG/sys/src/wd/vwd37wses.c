/*!**********************************************************************

  module: vwd37wses.c

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  Web-Sessions

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

#include <stdlib.h>
#include <time.h>

#include "hwd20control.h"
#include "hwd27excl.h"
#include "hwd28dict.h"
#include "hwd37wses.h"

#include "SAPDBCommon/SAPDB_string.h"

void wd37_apfgbyte( char *buf, int pos, int len,
                    char *dest, int dpos, int dlen,
                    sapdbwa_Bool  *truncated );

/*!**********************************************************************

  Chapter: sapdbwa_WebSessionP

************************************************************************/

typedef struct st_wa_session_data
{
	sapdbwa_UInt4		 serviceId;
    void			*data;
    sapdbwa_Destructor	 destructor;
} twd37SessionData;


struct st_wa_web_session
{
    twd37SessionData	sessionData;
    twd37SessionId		sessionId;
    sapdbwa_UInt4		timeOut;
    time_t				lastAccessTime;
    sapdbwa_Bool		setNewSessionId;	/* did we call WANewSessionId ?			*/
	twd27ExclP			lock;				/* flag to get exclusive access         */
											/* to the web session for one thread.	*/
}; /* struct st_wa_web_session */


sapdbwa_WebSessionP wd37CreateWebSession( const twd37SessionId  sessionId,
                                     sapdbwa_UInt4		   serviceId,
                                     sapdbwa_UInt4           timeOut )
{
    sapdbwa_Bool     allocatOK = sapdbwa_True;
    sapdbwa_WebSessionP newWebSession = NULL;

    sapdbwa_SQLALLOCAT( sizeof( struct st_wa_web_session ),
                   (sapdbwa_UInt1**) &newWebSession,
                   &allocatOK );
    if (allocatOK) {
        newWebSession->setNewSessionId  = sapdbwa_False;
        newWebSession->timeOut          = timeOut;

		/* Initialize lock object */
		if (!wd27InitExcl( &newWebSession->lock )) {
			sapdbwa_SQLFREE( (sapdbwa_UInt1*) newWebSession );
			return NULL;
		}

        time( &newWebSession->lastAccessTime );
        newWebSession->sessionData.serviceId = serviceId;
        newWebSession->sessionData.data = NULL;
        newWebSession->sessionData.destructor = NULL;
        SAPDB_memcpy( &newWebSession->sessionId[0],
                &sessionId[0], sizeof( twd37SessionId ));
    }; /* if */

    return newWebSession;
} /* wd37CreateWebSession */


void wd37DestroyWebSession( sapdbwa_WebSessionP webSession,
						    sapdbwa_Bool		freeUserData )
{

	if ( freeUserData ) {
		/* Lock web session */
		wd27BegExcl( webSession->lock );

		if (webSession->sessionData.data != NULL) {
			wd20CallSessionDataDestr( webSession->sessionData.serviceId,
									  webSession->sessionData.destructor,
									  webSession->sessionData.data );
		}; /* if */

		/* Unlock web session */
		wd27EndExcl( webSession->lock );
	}

	/* Destroy lock object */
    wd27ExitExcl( webSession->lock );

    sapdbwa_SQLFREE( (sapdbwa_UInt1*) webSession );

}


void* WINAPI sapdbwa_GetSessionData( sapdbwa_WebSessionP wses )
{
    return wses->sessionData.data;
} /* sapdbwa_GetSessionData */


sapdbwa_Bool wd37_SessionTimedOut( sapdbwa_WebSessionP  wses )
{
    time_t actualTime;

    time( &actualTime );

    if (wses->timeOut > 0) {
        return ( actualTime - wses->lastAccessTime >= wses->timeOut );
    } else {
        /* timeOut <= 0 => time-out disabled */
        return sapdbwa_False;
    };
} /* wd37_SessionTimedOut */


sapdbwa_Bool WINAPI sapdbwa_SetSessionData( sapdbwa_WebSessionP  wses,
                                     void           *data,
                                     sapdbwa_CDestructor  destructor )
{
    wses->sessionData.data       = data;
    wses->sessionData.destructor = (sapdbwa_Destructor) destructor;

    return sapdbwa_True;
} /* sapdbwa_SetSessionData */


sapdbwa_Bool WINAPI sapdbwa_NewSessionId( sapdbwa_WebSessionP  wses )
{
    wses->setNewSessionId = sapdbwa_True;

    return sapdbwa_True;
} /* sapdbwa_NewSessionId */


void wd37SetSessionId( sapdbwa_WebSessionP  wses,
                       twd37SessionId  sessionId )
{
    /* our dictionary is not sorted so we may change our sessionId */
    SAPDB_memcpy( &wses->sessionId[0],
            &sessionId[0], sizeof( twd37SessionId ));
} /* wd37SetSessionId */


void wd37GetSessionId( sapdbwa_WebSessionP   webSession,
                       twd37SessionId   sessionId )
{
    if (webSession->setNewSessionId) {
        wd37GetNewSessionId( NULL, webSession->sessionId );
        webSession->setNewSessionId = sapdbwa_False;
    }; /* if */
    SAPDB_memcpy( &sessionId[0], &webSession->sessionId[0], sizeof( twd37SessionId ));
} /* wd37GetSessionId */


void wd37UpdateTimestamp( sapdbwa_WebSessionP   webSession )
{
    time( &webSession->lastAccessTime );
} /* wd37UpdateTimestamp */

/*!**********************************************************************

  EndChapter: sapdbwa_WebSessionP

************************************************************************/

/*!**********************************************************************

  Chapter: twd37WebSessionContP

************************************************************************/

/* check the time out for all sessions after the creation of */
#define CHECK_TIMEOUT_INTERVAL 20 /* new web sessions */

struct st_wa_web_session_cont
{
    twd27ExclP         excl;    /* lock container when it it modified */
    
    sapdbwa_UInt4        sessionCount; /* number of sessions in container */
    twd28DictionaryP   activeSessions;

    sapdbwa_Int4         checkTimeoutCounter; /* counts the calls of
                                               wd37StartWebSession after
                                               last wd37CheckTimeout */
}; /* struct st_wa_web_session_cont */


/* you may call this only if you have exclusive access to the sessionCont */
void wd37CheckTimeout( twd37WebSessionContP sessionCont )
{
    sapdbwa_UInt4     iWebSession;
    sapdbwa_WebSessionP  webSession = NULL;
    const char     *dummy;
    
    wd27BegExcl( sessionCont->excl );
    for (iWebSession = 0;
         iWebSession < wd28NumEntries( sessionCont->activeSessions );
         iWebSession++ ) {

        if (wd28GetEntryByIndex( sessionCont->activeSessions,
                                 iWebSession,
                                 &dummy, (char**) &webSession )) {
			if (webSession != NULL && wd37_SessionTimedOut( webSession )) {
				wd28RemoveEntryByIndex( sessionCont->activeSessions,
										iWebSession );
				wd37DestroyWebSession( webSession, sapdbwa_True );
			}; /* if */
        }; /* if */
    }; /* for */
    wd28Reorganize( sessionCont->activeSessions );
    wd27EndExcl( sessionCont->excl );
} /* wd37CheckTimeout */
    
twd37WebSessionContP wd37CreateWebSessionCont()
{
    sapdbwa_Bool            allocatOK;
    twd37WebSessionContP  newCont = NULL;
    
    sapdbwa_SQLALLOCAT( sizeof( struct st_wa_web_session_cont ),
                   (sapdbwa_UInt1**) &newCont,
                   &allocatOK );
    if (allocatOK) {
        newCont->sessionCount = 0;
        if (!wd27InitExcl( &newCont->excl )) {
            sapdbwa_SQLFREE( (sapdbwa_UInt1*) newCont );
            newCont = NULL;
        } else {
            newCont->checkTimeoutCounter = 0;
            newCont->activeSessions = wd28CreateDictionary();
            if (newCont->activeSessions == NULL) {
                /* memory allocation failure */
                wd27ExitExcl( newCont->excl );
                sapdbwa_SQLFREE( (sapdbwa_UInt1*) newCont );
                newCont = NULL;
            }; /* if */
        }; /* else */
    }; /* if */

    return newCont;
} /* wd37CreateWebSessionCont */


void wd37DestroyWebSessionCont( twd37WebSessionContP sessionCont )
{
    sapdbwa_UInt4     iWebSession;
    sapdbwa_WebSessionP  webSession = NULL;
    const char     *dummy;
    
    wd27BegExcl( sessionCont->excl );
    for (iWebSession = 0;
         iWebSession < wd28NumEntries( sessionCont->activeSessions );
         iWebSession++ ) {
        if (wd28GetEntryByIndex( sessionCont->activeSessions,
                                 iWebSession,
                                 &dummy, (char**) &webSession )) {
            wd37DestroyWebSession( webSession, sapdbwa_True );
        }; /* if */
    }; /* for */
    wd28DestroyDictionary( sessionCont->activeSessions );
    wd27EndExcl( sessionCont->excl );
    wd27ExitExcl( sessionCont->excl );
    sapdbwa_SQLFREE( (sapdbwa_UInt1*) sessionCont );
} /* wd37DestroyWebSessionCont */


sapdbwa_Bool wd37CloseWebSession( twd37WebSessionContP  sessionCont,
                                  sapdbwa_WebSessionP   wses,
								  sapdbwa_Bool			freeUserData )
{
    sapdbwa_Bool sessionExists = sapdbwa_True;

    if (wses != NULL) {
        wd27BegExcl( sessionCont->excl );
        sessionExists = wd28RemoveEntryByKey( sessionCont->activeSessions,
                                              wses->sessionId );
        wd27EndExcl( sessionCont->excl );
        wd37DestroyWebSession( wses, freeUserData );
    };
    
    return sessionExists;
} /* WACloseWebSession */


sapdbwa_Bool wd37CloseServiceWebSession( twd37WebSessionContP	sessionContainer,
									   sapdbwa_UInt4			serviceId         )
{

	sapdbwa_UInt4		 count = 0;
	sapdbwa_UInt4		 i = 0;
	sapdbwa_Bool		 found = sapdbwa_False;
	const char		*key;
	sapdbwa_WebSessionP	 webSession;

    wd27BegExcl( sessionContainer->excl );
	if ( sessionContainer ) {
		count = wd28NumEntries( sessionContainer->activeSessions );

		for( i = 0; i < count && !found; i++ ) {
			if ( wd28GetEntryByIndex( sessionContainer->activeSessions, i, &key, (char**) &webSession )) {
				if ( webSession != NULL
                     && webSession->sessionData.serviceId == serviceId ) {
					found = sapdbwa_True;
				}
			}
		}

		if ( found ) {
            wd27EndExcl( sessionContainer->excl );
            
			return wd37CloseWebSession( sessionContainer, webSession, sapdbwa_True );
		}
	}
    wd27EndExcl( sessionContainer->excl );

	return sapdbwa_False;

} /* wd37CloseServiceWebSession */



void wd37GetNewSessionId( twd37WebSessionContP  sessionCont,
                          twd37SessionId        sessionId )
{
    sapdbwa_Bool   truncated;
    sapdbwa_UInt2  id[8];

	srand( (unsigned)time( NULL ) );

    id[0] = (sapdbwa_UInt2) rand();
    id[1] = (sapdbwa_UInt2) rand();
    id[2] = (sapdbwa_UInt2) rand();
    id[3] = (sapdbwa_UInt2) rand();
    id[4] = (sapdbwa_UInt2) rand();
    id[5] = (sapdbwa_UInt2) rand();
    id[6] = (sapdbwa_UInt2) rand();
    id[7] = (sapdbwa_UInt2) rand();
    wd37_apfgbyte( (char*) &id[0],
                   1, sizeof( id ),
                   sessionId, 1, SESSION_ID_LEN_WD37,
                   &truncated );
    sessionId[SESSION_ID_LEN_WD37] = '\0';
} /* wd37GetNewSessionId */


sapdbwa_WebSessionP wd37GetWebSession( twd37WebSessionContP	 sessionCont,
                                  const twd37SessionId	 sessionId,
                                  sapdbwa_UInt4			 serviceId,
                                  sapdbwa_Bool			*timedOut )
{
    sapdbwa_WebSessionP webSession = NULL;

    *timedOut = sapdbwa_False;
    wd27BegExcl( sessionCont->excl );    
    webSession = (sapdbwa_WebSessionP)
        wd28GetValueByKey( sessionCont->activeSessions, sessionId );

	if ( webSession && webSession->sessionData.serviceId != serviceId ) {
		webSession = NULL;
	}

    if (webSession != NULL) {
        if (wd37_SessionTimedOut( webSession )) {
            *timedOut = sapdbwa_True;
            wd28RemoveEntryByKey( sessionCont->activeSessions,
                                  webSession->sessionId );
            wd37DestroyWebSession( webSession, sapdbwa_True );
            webSession = NULL;
        } else {
            wd37UpdateTimestamp( webSession );
        }; /* else */
    }; /* if */
    
    wd27EndExcl( sessionCont->excl );

    return webSession;
} /* wd37GetWebSession */


sapdbwa_WebSessionP wd37StartWebSession( twd37WebSessionContP  sessionCont,
                                    const twd37SessionId  sessionId,
                                    sapdbwa_UInt4			  serviceId,
                                    sapdbwa_UInt4           timeOut )
{
    sapdbwa_Bool       allocatOK = sapdbwa_True;
    sapdbwa_WebSessionP   newWebSession = NULL;
    
    newWebSession = wd37CreateWebSession( sessionId, serviceId, timeOut );
    if (newWebSession != NULL) {
        wd27BegExcl( sessionCont->excl );
        /* check timeout for all sessions from time to time */
        sessionCont->checkTimeoutCounter++;
        if (sessionCont->checkTimeoutCounter >= CHECK_TIMEOUT_INTERVAL) {
            sessionCont->checkTimeoutCounter = 0;
            wd37CheckTimeout( sessionCont );
        }; /* if */
        if (!wd28AddEntry( sessionCont->activeSessions,
                           newWebSession->sessionId,
                           (char*) newWebSession )) {
            wd37DestroyWebSession( newWebSession, sapdbwa_True );
            newWebSession = NULL;
        }; /* if */
        wd27EndExcl( sessionCont->excl );
    }; /* if */
    
    return newWebSession;
} /* wd37StartWebSession */


sapdbwa_Bool wd37LockWebSession( twd37WebSessionContP  sessionCont,
                               sapdbwa_WebSessionP        webSession )
{

	/* Checks */
	if ( !webSession )
		return sapdbwa_False;

	/* Lock web session */
	if ( !wd27BegExcl( webSession->lock )) {
		return sapdbwa_False;
    };

    return sapdbwa_True;

} /* wd37LockWebSession */


sapdbwa_Bool wd37UnlockWebSession( twd37WebSessionContP  sessionCont,
                                 sapdbwa_WebSessionP        webSession )
{

	/* Checks */
	if ( !webSession )
		return sapdbwa_False;

	/* Unlock web session */
    if ( !wd27EndExcl( webSession->lock )) {
		return sapdbwa_False;
	}

    return sapdbwa_True;

} /* wd37UnlockWebSession */

/*!**********************************************************************

  EndChapter: twd37WebSessionContP
  
************************************************************************/


void wd37_apfgbyte( char *buf, int pos, int len,
                    char *dest, int dpos, int dlen,
                    sapdbwa_Bool *truncated)
{
    int di;
    int si;
    int lo_dig;
    int hi_dig;
    *truncated = 0;
    di = 0;
    si = 0;
    dpos--;
    pos--;
    while (!*truncated && si < len) {
        if (dlen < di + 2)
            {
                *truncated = 1;
            }
        else
            {
                UCHAR c = *(buf+pos+si);
                sapdbwa_UInt1 l = 0x0f;
                sapdbwa_UInt1 h = 0xf0;
                hi_dig = c & l;
                lo_dig = c & h;
                hi_dig = *(buf+pos + si) >> 4 & 0x000f;
                lo_dig = (*(buf+pos + si) >= 0) ?
                    *(buf+pos+si) & 15
                    : (*(buf+pos+si) % 16) ? 16 + *(buf+pos+si) & 15 : 0;
                if (hi_dig <= 9)
                    *(dest+dpos+di) = (char)(hi_dig + '0');
                else
                    *(dest+dpos+di) = (char)(hi_dig - 10 + 'A');
                di++;
                if (lo_dig <= 9)
                    *(dest+dpos+di) = (char)(lo_dig + '0');
                else
                    *(dest+dpos+di) = (char)(lo_dig - 10 + 'A');
                di++;
                si++;
            }
    }
    memset( dest+dpos+di, '0', dlen-di);
} /* wd37_apfgbyte */



