/*!**********************************************************************

  module: hwd37wses.h

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

#ifndef HWD37WSES_H
#define HWD37WSES_H

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "gwd00.h"
#include "hwd03waapi.h"


#define SESSION_ID_LEN_WD37 32

typedef char twd37SessionId[SESSION_ID_LEN_WD37+1];

void wd37GetSessionId( sapdbwa_WebSessionP  webSession,
                       twd37SessionId  sessionId );

/*!**********************************************************************

  Chapter: twd37WebSessionContP

  description: Container for sapdbwa_WebSession's
  
************************************************************************/

typedef struct st_wa_web_session_cont *twd37WebSessionContP;


/*!**********************************************************************
  Function: wd37CreateWebSessionCont

  description: Create a new Web-Session-Container

  arguments:

  return value: Pointer to new WebSession-Container or NULL if memory
                allocation failed. */

twd37WebSessionContP wd37CreateWebSessionCont();


/*!**********************************************************************
  Function: wd37DestroyWebSessionCont

  description: Destroy a WebSession-container

  arguments:
    sessionCont [in]  The WebSession-container to destroy.
    
  return value: void */

void wd37DestroyWebSessionCont( twd37WebSessionContP sessionCont );


void wd37GetNewSessionId( twd37WebSessionContP  sessionCont,
                          twd37SessionId        sessionId );

/*!**********************************************************************
  Function: wd37GetWebSession

  description: Return the WebSession for a session id.

  arguments:
    sessionCont  [in]    Pointer to the WebSessionContainer
    sessionId    [in]    The session id of the session. Usually this comes
                         from a cookie in the HTTP request.
    timedOut     [out]   Set to true if a web session exists. But this web
                         session was not accessed during the timeOut
                         period. If this is true the function returns NULL.
    
  return value: Pointer to the WebSession or NULL if no WebSession with
                this id exists (the session may be timed out). */

sapdbwa_WebSessionP wd37GetWebSession( twd37WebSessionContP	 sessionCont,
                                  const twd37SessionId	 sessionId,
                                  SAPDB_UInt4			 serviceId,
                                  SAPDB_Bool			*timedOut    );



/*!**********************************************************************
  Function: wd37CheckTimeout

  description: Checks all web sessions timeout value. If a web session
               has been expired, the web session will be close.

  arguments:
    sessionCont  [in]    Pointer to the WebSessionContainer
    
  return value: 
*/

void wd37CheckTimeout( twd37WebSessionContP	 sessionCont );


/*!**********************************************************************
  Function: wd37StartWebSession

  description: Initiate a new WebSession.

  arguments:
    sessionCont  [in]    Pointer to the WebSessionContainer
    sessionId    [in]    The session id of the session. Usually this comes
                         from a cookie in the HTTP request.
    timeOut      [in]    Time out (seconds) after which the web session is
                         implicitely closed.
                         
  return value: Pointer to the WebSession or NULL if the creation of a new
                WebSession failed. */

sapdbwa_WebSessionP wd37StartWebSession( twd37WebSessionContP	sessionCont,
                                         const twd37SessionId	sessionId,
                                         SAPDB_UInt4			serviceId,
                                         SAPDB_UInt4			timeOut      );


/*!**********************************************************************
  Function: wd37CloseWebSession

  description: Close given web session.

  arguments:
    sessionCont		[in]    Pointer to the WebSessionContainer
    wses			[in]	Web session to delete.
	freeUserData	[in]	If true user data will be destroyed by calling
							user's destructor funtion.
                         
  return value: True if session could be closed, otherwise false
*/

SAPDB_Bool wd37CloseWebSession( twd37WebSessionContP sessionCont,
                                sapdbwa_WebSessionP  wses,
								sapdbwa_Bool		 freeUserData );


/*!**********************************************************************
  Function: wd37StartWebSession

  description: Close session according a service id.

  arguments:
    sessionContainer	[in]	Pointer to the WebSessionContainer
    serviceId			[in]    The id of the service
                         
  return value: True if session could be closed, otherwise false
*/

SAPDB_Bool wd37CloseServiceWebSession( twd37WebSessionContP	sessionContainer,
									   SAPDB_UInt4			serviceId         );


/*!**********************************************************************
  Function: wd37LockWebSession

  description: Lock web session. If this returns true further calls to
               wd37LockWebSession will return false until
               wd37UnlockWebSession is called.

               The functions wd37LockWebSession and wd37UnlockWebSession
               are used to sychronize the access from multiple threads
               (see vwd23wa.c).
               
  arguments:
    sessionCont  [in]    Pointer to the WebSessionContainer of the
                         webSession.
    webSession   [in]    The we session to lock.
                         
  return value: true for success or false if the session is already
                locked. */

SAPDB_Bool wd37LockWebSession( twd37WebSessionContP sessionCont,
                               sapdbwa_WebSessionP       webSession );


/*!**********************************************************************
  Function: wd37UnlockWebSession

  description: Unlocks a web session that was locked with
               wd37LockWebSession. It is not checked if the caller is
               allowed to unlock the web session. So the caller has to
               recognize if he himself locked the web session.

  arguments:
    sessionCont  [in]    Pointer to the WebSessionContainer of the
                         webSession.
    webSession   [in]    The we session to unlock.
                         
  return value: void*/

SAPDB_Bool wd37UnlockWebSession( twd37WebSessionContP sessionCont,
                                 sapdbwa_WebSessionP       webSession );

/*!**********************************************************************

  EndChapter: twd37WebSessionContP
  
************************************************************************/

#endif
