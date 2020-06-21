/*!**********************************************************************

  module: hwd40user.h

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  UserDll functions

  see also:

  -----------------------------------------------------------------------

  copyright:          Copyright (c) 1998-2005 SAP AG



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

#ifndef HWD40USER_H
#define HWD40USER_H

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "gwd00.h"
#include "hwd03waapi.h"
#include "hwd21req.h"
#include "hwd22rep.h"
#include "hwd26err.h"
#include "hwd30ses.h"

/*!**********************************************************************

  Chapter: twd40UserDllP

  description: This type is used to handle UserDll's. A UseDll is
               configured in the WebAgent.ini file. The dynamic link
               library for a UserDll must contain at least an
               initialization- and a service function. A session-pool
               can be assigned to a UserDll.
  
************************************************************************/

typedef struct st_wa_user_dll	*twd40UserDllP;

typedef enum {
    DllTypeC,
    DllTypeCPP
} twd40DllType;

/*!**********************************************************************
  Function: wd40CreateUserDll

  description: Create new UserDll description.

  arguments:
    name  [in]  The logical name of the UserDll. This is the unique name
                from the WebAgent.ini file.
    id    [in]  A unique id for the UserDll.
    err   [out] If the function returnes NULL the error type and
                message are set in err.
    
  return value: A pointer to the created UserDll or NULL if the creation
                failed because of insufficient memory resources or if
                the name is too long. */

twd40UserDllP wd40CreateUserDll( const char		*name,
                                 sapdbwa_UInt4	 id,
                                 twd26ErrP		 err );


/*!**********************************************************************
  Function: wd40DestroyUserDll

  description: Free resources of the UserDll.

  arguments:
    userDll  [inout]  Pointer to UserDll that should be freed.
    
  return value: void

  NOTE: The UserDll is NOT unloaded before it is destroyed. This should
        be done with wd40UnloadUserDll before destroying it. */

void wd40DestroyUserDll( twd40UserDllP userDll );


/*!**********************************************************************
  Function: wd40CallExitFunc

  description: Call the exit function in the UserDll.

  arguments:
    userDll  [in]  Pointer to the UserDll
    wa       [in]  sapdbwa_Handle that is passsed to the exit
                   function in the UserDll.
    exitRet  [out] Return code of the exit function.
    log      [in]  If an exception occurs a message is written to this
                   log.
    err      [out] If the function returnes false the error type and
                   message are set in err.
                   
  return value: true if the UserDll-exit-function finished. false if
                the UserDll-init-function could not be called or if it
                returned throwed an exception or if it returned
                sapdbwa_RTC_ERROR. */

sapdbwa_Bool wd40CallExitFunc( twd40UserDllP  userDll,
                             sapdbwa_Handle      wa,
                             sapdbwa_Int2    *exitRet,
                             twd25LogP      log,
                             twd26ErrP      err      );


/*!**********************************************************************
  Function: wd40CallInitFunc

  description: Call the initialization function in the UserDll.

  arguments:
    userDll  [in]  Pointer to the UserDll
    wa       [in]  sapdbwa_Handle that is passsed to the initialization
                   function in the UserDll.
    initRet  [out] Return code of the init function.
    log      [in]  If an exception occurs a message is written to this
                   log.
    err      [out] If the function returnes false the error type and
                   message are set in err.
                   
  return value: true if the UserDll-init-function finished. false if
                the UserDll-init-function could not be called or if it
                throwed an exception or if it returned sapdbwa_RTC_ERROR.*/

sapdbwa_Bool wd40CallInitFunc( twd40UserDllP  userDll,
                             sapdbwa_Handle      wa,
                             sapdbwa_Int2    *initRet,
                             twd25LogP      log,
                             twd26ErrP      err );


/*!**********************************************************************
  Function: wd40CallServiceFunc

  description: Call the service function in the UserDll.

  arguments:
    userDll  [in]    Pointer to the UserDll
    wa       [in]    sapdbwa_Handle that is passsed to the service
                     function in the UserDll.
    request  [in]    Description of the incoming http-request.
    reply    [inout] Reply-handle used to send data to the webserver.
    svcRet   [out]   Return-code of the service function (sapdbwa_RTC_IGNORED,
                     sapdbwa_RTC_ACCEPTED or sapdbwa_RTC_ERROR).
    log      [in]    If an exception occurs a message is written to this
                     log.
    err      [out]   Error information if function returns false.
    
  return value: true if the service functions finished. false if the
                service function could not be called or throwd an
                exception or if it returned sapdbwa_RTC_ERROR. If the return
                code is false err contains the reason for the failure. */

sapdbwa_Bool wd40CallServiceFunc( twd40UserDllP   userDll,
                                sapdbwa_Handle       wa,
                                sapdbwa_HttpRequestP request,
                                sapdbwa_HttpReplyP   reply,
                                sapdbwa_Int2     *svcRet,
                                twd25LogP       log,
                                twd26ErrP       err );


sapdbwa_Bool wd40CallSessionDataDestr( twd40UserDllP   userDll,
                                     sapdbwa_Handle       wa,
                                     sapdbwa_Destructor   destructor,
                                     void           *data,
                                     twd25LogP       log,
                                     twd26ErrP       err );


/* Get configuration of user-dll */
sapdbwa_Bool wd40GetConf( twd40UserDllP  userDll,
                        char          *textBuffer,
                        sapdbwa_UInt4   *bufferLen );


/*!**********************************************************************
  Function: wd40GetPathPrefix

  description: Return the path-prefix that describes which reuqests are
               handled by the UserDll.

  arguments:
    userDll  [in]    Pointer to the UserDll

  return value: Pointer to path-prefix. */
  
char* wd40GetPathPrefix( twd40UserDllP userDll );


/*!**********************************************************************
  Function: wd40GetUserDllId

  description: Return the unique id of the UserDll.

  arguments:
    userDll  [in]    Pointer to the UserDll

  return value: The id of the UserDll. */

sapdbwa_UInt4 wd40GetUserDllId( twd40UserDllP userDll );


/*!**********************************************************************
  Function: wd40GetUserDllName

  description: Return the logical name of the UserDll.

  arguments:
    userDll  [in]    Pointer to the UserDll

  return value: Pointer to the name. */

const char* wd40GetUserDllName( twd40UserDllP userDll );


sapdbwa_Bool wd40GetUseWebSessions( twd40UserDllP   userDll );


/*!**********************************************************************
  Function: wd40GetUserDllLibHandle

  description: Returns the library handle.

  arguments:
    userDll  [in]    Pointer to the UserDll

  return value: Library handle. */

HANDLE wd40GetUserDllLibHandle( twd40UserDllP userDll );


/*!**********************************************************************
  Function: wd40GetPropertyWithSSL

  description: Returns the secure type.

  arguments:
    userDll  [in]    Pointer to the UserDll

  return value: secure type. */

sapdbwa_Bool wd40GetPropertyWithSSL( twd40UserDllP userDll );


/*!**********************************************************************
  Function: wd40SetPropertyWithSSL

  description: Sets the secure type.

  arguments:
    userDll  [in]    Pointer to the UserDll
    withSSL  [in]    If true SSL is desired

  return value: no. */

void wd40SetPropertyWithSSL( twd40UserDllP	userDll,
							 sapdbwa_Bool		withSSL  );


/*!**********************************************************************
  Function: wd40GetPropertySSLURL

  description: Returns the start URL for a SSL comunication

  arguments:
    userDll  [in]    Pointer to the UserDll

  return value: SSL URL. */

char* wd40GetPropertySSLURL( twd40UserDllP userDll );


/*!**********************************************************************
  Function: wd40SetPropertySSLURL

  description: Sets the URL for a SSL comunication.

  arguments:
    userDll [in]    Pointer to the UserDll
    sslURL	[in]	URL

  return value: not. */

void wd40SetPropertySSLURL( twd40UserDllP	 userDll,
							const char		*sslURL   );


/*!**********************************************************************
  Function: wd40GetPropertyWebSessionTimeout

  description:		Returns the timeout value for web sessions.

  arguments:
    userDll  [in]   Pointer to the UserDll

  return value:		timeout value. */

sapdbwa_UInt4 wd40GetPropertyWebSessionTimeout( twd40UserDllP userDll );


/*!**********************************************************************
  Function: wd40SetPropertyWebSessionTimeout

  description:		Sets the timeout value for web sessions.

  arguments:
    userDll [in]    Pointer to the UserDll
    sslURL	[in]	timeout value

  return value: not. */

void wd40SetPropertyWebSessionTimeout( twd40UserDllP	userDll,
									   sapdbwa_UInt4		timeout );

/*!**********************************************************************
  Function: wd40GetSessionPool

  description: Return the configured session-pool.

  arguments:
    userDll  [in]    Pointer to the UserDll

  return value: Pointer to the session-pool or NULL if no session-pool
                was configured for this UserDll. */

twd30SessionPoolP wd40GetSessionPool( twd40UserDllP userDll );


/*!**********************************************************************
  Function: wd40InitUserDll

  description: Set properties of the UserDll.

  arguments:
    userDll					[in]    Pointer to the UserDll
    pathPrefix				[in]    The pathPrefix that describes which requests are
			                        handled by this UserDll.
    dllName					[in]    The filename (including the absolute path) of
									the UserDll's dynamic link library.
    sessionPool				[in]    Pointer to the session-pool that should be used
									by the UserDll.
    initFunc				[in]    Name of the initialization function in the
									UserDll.
    serviceFunc				[in]    Name of the service-function in the UserDll.
	withSSL					[in]	Secure type.
	sslURL					[in]	Start URL for a secure comunication.
	useFastCGIForCookiePath	[in]	Ignore FastCGI prefix for cookie path or not.
    err						[out]   When an error occurs the error code and message
									are stored in err.
                        
  return value: true if the UserDll could be initialized false if not.
                If false is returned the error information can be
                retrieved from err. */

sapdbwa_Bool wd40InitUserDll( twd40UserDllP      userDll,
							  char              *pathPrefix,
							  char              *dllName,
							  twd30SessionPoolP  sessionPool,
							  char              *initFunc,
							  char              *exitFunc,
							  char              *serviceFunc,
							  char              *logFile,
							  twd40DllType       userDllType,
							  sapdbwa_Bool		 withSSL,
							  const char		*sslURL,
							  sapdbwa_UInt4		 webSessionTimeout,
							  sapdbwa_Bool	     useFastCGIForCookiePath,
							  twd26ErrP          err );


/*!**********************************************************************
  Function: wd40LoadUserDll

  description: Try to load the dynamic link library.

  arguments:
    userDll  [in]    Pointer to the UserDll
    err      [out]   When an error occurs the error code and message
                     are stored in err.

  return value: true if loading was succesful false if not.
                If false is returned the error information can be
                retrieved from err. */

sapdbwa_Bool wd40LoadUserDll( twd40UserDllP   UserDll,
                            twd26ErrP       err );


sapdbwa_Bool wd40SetUseWebSessions( twd40UserDllP   UserDll,
                                  sapdbwa_Bool      useWebSessions );

/*!**********************************************************************
  Function: wd40UnloadUserDll

  description: Try to unload the dynamic link library.

  arguments:
    userDll  [in]    Pointer to the UserDll
    err      [out]   When an error occurs the error code and message
                     are stored in err.

  return value: true if loading was succesful false if not.
                If false is returned the error information can be
                retrieved from err. */

sapdbwa_Bool wd40UnloadUserDll( twd40UserDllP userDll,
                              twd26ErrP     err       );


/*!**********************************************************************
  Function: wd40WriteLogMsg

  description: Writes a log message to the file that was configured in
               webagent.ini file.

  arguments:
    userDll  [in]    Pointer to the UserDll
    msg      [in]    The message that should be written to the log file.
    
  return value: true if the message could be written false else.*/

sapdbwa_Bool wd40WriteLogMsg( twd40UserDllP userDll, const char *msg );


/*!**********************************************************************

  EndChapter: twd40UserDllP

  description:
  
************************************************************************/
#endif

