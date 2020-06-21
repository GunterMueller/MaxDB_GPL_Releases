/*!**********************************************************************

  module: vwd40user.c

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  UserDll functions

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


//
//   FreeBSD portions added by Kai Mosebach,
//   For more informations see : http://www.komadev.de/sapdb
//

************************************************************************/

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include <string.h>

#include "heo02x.h" /* sqlLoadLibrary */

#include "hwd09util.h"
#include "hwd25log.h"
#include "hwd41cpp.h"

#include "hwd40user.h"

/*!**********************************************************************

  Chapter: UserDll

************************************************************************/

#define ERR_MSG_LEN 1024 /* max length for LZU errors */

#ifdef WIN32
#define DLL_EXTENSION ".dll"
#elif (defined SUN || defined SOLARIS || defined AIX || defined OSF1 || defined NMP || defined LINUX || defined FREEBSD || defined HP_IA64)
#define DLL_EXTENSION ".so"
#elif defined HPUX && !defined HP_IA64
#define DLL_EXTENSION ".sl"
#endif

struct st_wa_user_dll
{
    sapdbwa_UInt4		 id;
    sapdbwa_Bool		 initialized;							/* user dll init function called ?		*/
    char				 Name[MAX_NAME_LEN_WD00];
    char				 PathPrefix[MAX_NAME_LEN_WD00];
    HANDLE				 hLib;									/* user dll handle						*/
    char				 DllName[MAX_DLL_NAME_LEN_WD00];		/* user dll name						*/
    char				 InitFuncName[MAX_DLL_NAME_LEN_WD00];	/* user initialization function name	*/
    char				 ExitFuncName[MAX_DLL_NAME_LEN_WD00];	/* user exit function name				*/
    char				 ServiceFuncName[MAX_DLL_NAME_LEN_WD00];/* user service function name			*/
    twd30SessionPoolP	 SessionPool;
    sqlPROC				 WAApiInit;								/* user initialization function			*/
    sqlPROC				 WAApiExit;								/* user exit function					*/
    sqlPROC				 WAApiService;							/* user service function				*/
    sapdbwa_Bool		 Loaded;
    void				*WapiArea;
    twd40DllType		 userDllType;
    sapdbwa_Bool		 useWebSessions;						/* does the user-dll use web-session	*/
																/* this must be set in the user dll		*/
																/* init function						*/
    twd25LogP			 log;
	sapdbwa_Bool		 withSSL;								/* SSL protocol desired					*/
	char				 sslURL [MAX_INI_PAR_LEN_WD00];			/* SSL URL								*/
	sapdbwa_UInt4		 webSessionTimeout;						/* Timeout for starting default			*/
																/* web sessions	*/
	sapdbwa_Bool		 useFastCGIForCookiePath;				/* Ignore FastCGI prefix for cookie		*/
																/* path or not							*/
}; /* struct st_wa_user_dll */


sapdbwa_Bool wd40CallExitFunc( twd40UserDllP  userDll,
                             sapdbwa_Handle       wa,
                             sapdbwa_Int2    *exitRet,
                             twd25LogP      log,
                             twd26ErrP      err )
{
    sapdbwa_Bool  execOK = true;

    if (!userDll->initialized) {
        wd26SetErr( err,
                    ERR_USERDLL_NOT_INIT_WD26,
                    "exit", userDll->Name );
        execOK = false;
    } else {
        if (userDll->WAApiExit != NULL) {
            if (userDll->userDllType == DllTypeCPP) {
                execOK = wd41CallCppExitFunc( userDll->Name, log,
                                              userDll->WAApiExit,
                                              wa,
                                              exitRet );
            } else {
                execOK = wd41CallExitFunc( userDll->Name, log,
                                           userDll->WAApiExit,
                                           wa,
                                           exitRet );
            }; /* else */
            userDll->initialized = false;
            if (!execOK) {
                wd26SetErr( err,
                            ERR_EXCEPTION_WD26,
                            "exit", userDll->Name );
            } else if (*exitRet == sapdbwa_RTC_Error) {
                execOK = false;
                wd26SetErr( err,
                            ERR_EXIT_FUNC_ERROR_WD26,
                            userDll->Name, NULL );
            }; /* else */
        } else {
            *exitRet = sapdbwa_RTC_Ok;
        }; /* else */
    }; /* else */

    return execOK;
} /* wd40CallExitFunc */


sapdbwa_Bool wd40CallInitFunc( twd40UserDllP  userDll,
                             sapdbwa_Handle       wa,
                             sapdbwa_Int2    *initRet,
                             twd25LogP      log,
                             twd26ErrP      err )
{
    sapdbwa_Bool  execOK = true;

    if (!userDll->initialized) {
        wd26SetErr( err,
                    ERR_USERDLL_NOT_INIT_WD26,
                    "init", userDll->Name );
        execOK = false;
    } else {
        if (userDll->WAApiInit != NULL) {
            if (userDll->userDllType == DllTypeCPP) {
                execOK = wd41CallCppInitFunc( userDll->Name, log,
                                              userDll->WAApiInit,
                                              wa,
                                              initRet );
            } else {
                execOK = wd41CallInitFunc( userDll->Name, log,
                                           userDll->WAApiInit,
                                           wa,
                                           initRet );
            }; /* else */
            if (!execOK) {
                wd26SetErr( err,
                            ERR_EXCEPTION_WD26,
                            "init", userDll->Name );
            } else if (*initRet == sapdbwa_RTC_Error) {
                execOK = false;
                wd26SetErr( err,
                            ERR_INIT_FUNC_ERROR_WD26,
                            userDll->Name, NULL );
            } else {
                /* init function successfully called */
                userDll->initialized = true;
            }; /* else */
        } else {
            *initRet = sapdbwa_RTC_Ok;
        }; /* else */
    }; /* else */

    return execOK;
} /* wd40CallInitFunc */


sapdbwa_Bool wd40CallServiceFunc( twd40UserDllP   userDll,
                                sapdbwa_Handle        wa,
                                sapdbwa_HttpRequestP  request,
                                sapdbwa_HttpReplyP    reply,
                                sapdbwa_Int2     *svcRet,
                                twd25LogP       log,
                                twd26ErrP       err )
{
    sapdbwa_Bool  execOK = true;

    if (!userDll->initialized) {
        wd26SetErr( err,
                    ERR_USERDLL_NOT_INIT_WD26,
                    "service", userDll->Name );
        execOK = false;
    } else {
        if (userDll->userDllType == DllTypeCPP) {
            execOK = wd41CallCppServiceFunc( userDll->Name, log,
                                             userDll->WAApiService,
                                             wa, request, reply,
                                             svcRet );
        } else {
            execOK = wd41CallServiceFunc( userDll->Name, log,
                                          userDll->WAApiService,
                                          wa, request, reply,
                                          svcRet );
        }; /* else */
        if (!execOK) {
            wd26SetErr( err,
                        ERR_EXCEPTION_WD26,
                        "service", userDll->Name );
        } else if (*svcRet == sapdbwa_RTC_Error) {
            execOK = false;
            wd26SetErr( err,
                        ERR_SERVICE_FUNC_ERROR_WD26,
                        userDll->Name, NULL );
        }; /* else */
    }; /* else */

    return execOK;
} /* wd40CallServiceFunc */


sapdbwa_Bool wd40CallSessionDataDestr( twd40UserDllP   userDll,
                                     sapdbwa_Handle        wa,
                                     sapdbwa_Destructor    destructor,
                                     void           *data,
                                     twd25LogP       log,
                                     twd26ErrP       err )
{
    sapdbwa_Bool  execOK = true;

    if (!userDll->initialized) {
        wd26SetErr( err,
                    ERR_USERDLL_NOT_INIT_WD26,
                    "service", userDll->Name );
        execOK = false;
    } else {
        if (userDll->userDllType == DllTypeCPP) {
            execOK = wd41CallCppSessionDataDestr( userDll->Name,
                                                  log,
                                                  (sqlPROC) destructor,
                                                  data,
                                                  wa );
        } else {
            execOK = wd41CallSessionDataDestr( userDll->Name,
                                               log,
                                               (sqlPROC) destructor,
                                               data,
                                               wa );
        }; /* else */
        if (!execOK) {
            wd26SetErr( err,
                        ERR_EXCEPTION_WD26,
                        "user-data-destructor", userDll->Name );
        }; /* else */
    }; /* else */

    return execOK;
} /* wd40CallSessionDataDestr */


twd40UserDllP wd40CreateUserDll( const char		*name,
                                 sapdbwa_UInt4	 id,
                                 twd26ErrP		 err )
{
    sapdbwa_Bool    allocatOK;
    twd40UserDllP newUserDll;
    sapdbwa_UInt2   nameLen;

    nameLen = strlen( name );
    if (nameLen >= sizeof( newUserDll->Name )) {
        /* name too long */
        wd26SetErr( err, ERR_NAME_TOO_LONG_WD26, name, NULL );
        return false;
    }; /* if */

    sapdbwa_SQLALLOCAT( sizeof(struct st_wa_user_dll),
                (sapdbwa_UInt1**) &newUserDll,
                &allocatOK );
    if (allocatOK) {
        strcpy( newUserDll->Name, name );
        newUserDll->id             = id;
        newUserDll->initialized    = false;
        newUserDll->SessionPool    = NULL;
        newUserDll->hLib           = NULL;
        newUserDll->WAApiInit      = NULL;
        newUserDll->WAApiExit      = NULL;
        newUserDll->WAApiService   = NULL;
        newUserDll->Loaded         = false;
        newUserDll->log            = NULL;
        newUserDll->userDllType    = DllTypeC;
        newUserDll->useWebSessions = false;
        newUserDll->withSSL		   = false;
        newUserDll->sslURL[0]	   = '\0';
        newUserDll->webSessionTimeout	= 0;
        return newUserDll;
    } else {
        return NULL;
    }; /* else */
} /* wd40CreateUserDll */


void wd40DestroyUserDll( twd40UserDllP userDll )
{
    if (userDll->log != NULL) {
        wd25DestroyLog( userDll->log );
    }; /* if */
    sapdbwa_SQLFREE( (sapdbwa_UInt1*) userDll );
} /* wd40DestroyUserDll */


sapdbwa_Bool wd40GetConf( twd40UserDllP  userDll,
                        char          *textBuffer,
                        sapdbwa_UInt4   *bufferLen )
{
    sapdbwa_Bool ok = true;
    const char *typeStr;
	char		webSessionTimeout	[MAX_INI_PAR_LEN_WD00];

    if (userDll->userDllType == DllTypeCPP) {
        typeStr = "CPP";
    } else {
        typeStr = "C";
    }; /* else */
    ok = wd09AddString( textBuffer, "[", bufferLen )
        && wd09AddString( textBuffer, userDll->Name, bufferLen )
        && wd09AddString( textBuffer, "]\n", bufferLen )
        && wd09AddString( textBuffer, "PathPrefix=", bufferLen )
        && wd09AddString( textBuffer, userDll->PathPrefix, bufferLen )
        && wd09AddString( textBuffer, "\n", bufferLen )
        && (userDll->SessionPool == NULL
            || (wd09AddString( textBuffer, "SessionPool=", bufferLen )
                && wd09AddString( textBuffer,
                                  wd30GetName( userDll->SessionPool ),
                                  bufferLen )
                && wd09AddString( textBuffer, "\n", bufferLen )))
        && wd09AddString( textBuffer, "UserDll=", bufferLen )
        && wd09AddString( textBuffer, userDll->DllName, bufferLen )
        && wd09AddString( textBuffer, "\n", bufferLen )
        && wd09AddString( textBuffer, "InitFunc=", bufferLen )
        && wd09AddString( textBuffer, userDll->InitFuncName, bufferLen )
        && wd09AddString( textBuffer, "\n", bufferLen )
        && wd09AddString( textBuffer, "ServiceFunc=", bufferLen )
        && wd09AddString( textBuffer, userDll->ServiceFuncName, bufferLen )
        && wd09AddString( textBuffer, "\n", bufferLen )
        && wd09AddString( textBuffer, "ExitFunc=", bufferLen )
        && wd09AddString( textBuffer, userDll->ExitFuncName, bufferLen )
        && wd09AddString( textBuffer, "\n", bufferLen )
        && wd09AddString( textBuffer, "LogFile=", bufferLen )
        && (userDll->log == NULL
            || (wd09AddString( textBuffer,
                               wd25GetFileName( userDll->log ),
                               bufferLen )))
        && wd09AddString( textBuffer, "\n", bufferLen )
        && wd09AddString( textBuffer, "UserDllType=", bufferLen )
        && wd09AddString( textBuffer, typeStr, bufferLen )
        && wd09AddString( textBuffer, "\n", bufferLen );

	if ( userDll->withSSL ) {
		ok = ok && wd09AddString( textBuffer, "With SSL=True", bufferLen );
	} else {
		ok = ok && wd09AddString( textBuffer, "With SSL=False", bufferLen );
	}

	if ( userDll->useFastCGIForCookiePath ) {
		ok = ok && wd09AddString( textBuffer, "Use FastCGI prefix for cookie path", bufferLen );
	} else {
		ok = ok && wd09AddString( textBuffer, "Do not use FastCGI prefix for cookie path", bufferLen );
	}

    ok = ok && wd09AddString( textBuffer, "\n", bufferLen );

	ok = ok && wd09AddString( textBuffer, "SSL URL=", bufferLen )
			&& userDll->sslURL
			&& wd09AddString( textBuffer, userDll->sslURL, bufferLen )
			&& wd09AddString( textBuffer, "\n", bufferLen );

	webSessionTimeout[0]='\0';
	sp77sprintf(webSessionTimeout, MAX_INI_PAR_LEN_WD00, "%u", userDll->webSessionTimeout);
	ok = ok && wd09AddString( textBuffer, "Default Web Session Timeout=", bufferLen )
			&& wd09AddString( textBuffer, webSessionTimeout, bufferLen )
			&& wd09AddString( textBuffer, "\n", bufferLen );

    return ok;
} /* wd40GetConf */


char* wd40GetPathPrefix( twd40UserDllP userDll )
{
    return userDll->PathPrefix;
} /* wd40GetPathPrefix */


sapdbwa_Bool wd40UseFastCGIForCookiePath( twd40UserDllP userDll )
{

    return userDll->useFastCGIForCookiePath;

}


twd30SessionPoolP wd40GetSessionPool( twd40UserDllP userDll )
{
    return userDll->SessionPool;
} /* wd40GetSessionPool */


sapdbwa_UInt4 wd40GetUserDllId( twd40UserDllP userDll )
{
    return userDll->id;
} /* wd40GetUserDllId */

HANDLE wd40GetUserDllLibHandle( twd40UserDllP userDll )
{
    return userDll->hLib;
}

sapdbwa_Bool wd40GetPropertyWithSSL( twd40UserDllP userDll )
{
    return userDll->withSSL;
}

void wd40SetPropertyWithSSL( twd40UserDllP	userDll,
							 sapdbwa_Bool		withSSL  )
{
    userDll->withSSL = withSSL;
}

char* wd40GetPropertySSLURL( twd40UserDllP userDll )
{
    return userDll->sslURL;
}

void wd40SetPropertySSLURL( twd40UserDllP	 userDll,
							const char		*sslURL   )
{
    strncpy( userDll->sslURL, sslURL, sizeof(userDll->sslURL) );
	userDll->sslURL[sizeof(userDll->sslURL)-1] = '\0';
}

sapdbwa_UInt4 wd40GetPropertyWebSessionTimeout( twd40UserDllP userDll )
{
    return userDll->webSessionTimeout;
}

void wd40SetPropertyWebSessionTimeout( twd40UserDllP	userDll,
									   sapdbwa_UInt4		timeout  )
{
    userDll->webSessionTimeout = timeout;
}

const char* wd40GetUserDllName( twd40UserDllP userDll )
{
    return userDll->Name;
} /* wd40GetUserDllName */


sapdbwa_Bool wd40GetUseWebSessions( twd40UserDllP   userDll )
{
    return userDll->useWebSessions;
} /* wd40GetUseWebSessions */


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
							  twd26ErrP          err )
{

    if ( strlen(pathPrefix) >= sizeof(userDll->PathPrefix)
         || strlen(dllName) >= sizeof(userDll->DllName)
         || strlen(initFunc) >= sizeof(userDll->InitFuncName)
         || strlen(exitFunc) >= sizeof(userDll->ExitFuncName)
         || strlen(serviceFunc) >= sizeof(userDll->ServiceFuncName)) {
        /* parameter too long */
        wd26SetErr( err, ERR_PARAM_TOO_LONG_WD26, userDll->Name, NULL );
        return sapdbwa_False;
    } else if (strlen( initFunc ) == 0) {
        wd26SetErr( err, ERR_NO_INIT_FUNC_WD26, userDll->Name, NULL );
        return sapdbwa_False;
    } else if (strlen( exitFunc ) == 0) {
        wd26SetErr( err, ERR_NO_EXIT_FUNC_WD26, userDll->Name, NULL );
        return sapdbwa_False;
    } else if (strlen( serviceFunc ) == 0) {
        wd26SetErr( err, ERR_NO_SERVICE_FUNC_WD26, userDll->Name, NULL );
        return sapdbwa_False;
    } else {
        if (logFile != NULL && strlen( logFile ) != 0) {
            userDll->log = wd25CreateLog( logFile );
            if (userDll->log == NULL) {
                wd26SetErr( err, ERR_MEMORY_WD26, NULL, NULL );
                return sapdbwa_False;
            }; /* if */
        }; /* if */
        strcpy( userDll->PathPrefix, pathPrefix );
        strcpy( userDll->DllName, dllName );
        strcpy( userDll->InitFuncName, initFunc );
        strcpy( userDll->ExitFuncName, exitFunc );
        strcpy( userDll->ServiceFuncName, serviceFunc );
        userDll->SessionPool = sessionPool;
        userDll->initialized = true;
        userDll->userDllType = userDllType;
        { /* cut off dll extension */
            sapdbwa_Int4 nameLen = strlen( userDll->DllName );

            if (nameLen > sizeof( DLL_EXTENSION )
               && stricmp(&userDll->DllName[nameLen-sizeof( DLL_EXTENSION )+1],
                          DLL_EXTENSION ) == 0) {
                /* the name contains an extension. cut it off */
                userDll->DllName[nameLen-sizeof( DLL_EXTENSION )+1] = '\0';
            }; /* if */
        };

		userDll->withSSL = withSSL;
		wd40SetPropertySSLURL( userDll, sslURL );
		wd40SetPropertyWebSessionTimeout( userDll, webSessionTimeout );

		userDll->useFastCGIForCookiePath = useFastCGIForCookiePath;

        return sapdbwa_True;
    };

}


sqlPROC wd40_GetProcAddress( twd40UserDllP  userDll,
                             const char    *funcName,
                             twd26ErrP      err );

sapdbwa_Bool wd40LoadUserDll( twd40UserDllP  userDll,
                            twd26ErrP      err )
{
    sapdbwa_Bool retcode = false;
    char error[ERR_MSG_LEN];

    if (userDll->Loaded)
        return true;

    /* load dll */
    userDll->hLib = sqlLoadLibrary( userDll->DllName, error, sizeof( error ));
    if (!userDll->hLib) {
        wd26SetErr( err,
                    ERR_USERDLL_LOAD_FAILED_WD26,
                    userDll->DllName,
                    error );
        return false;
    }; /* if */

    /* get init function */
    userDll->WAApiInit = wd40_GetProcAddress( userDll,
                                              userDll->InitFuncName,
                                              err );
    if (userDll->WAApiInit != NULL) {
        /* get exit function */
        userDll->WAApiExit = wd40_GetProcAddress( userDll,
                                                  userDll->ExitFuncName,
                                                  err );
        if (userDll->WAApiExit != NULL) {
            /* get service function */
            userDll->WAApiService =
                wd40_GetProcAddress( userDll,
                                     userDll->ServiceFuncName,
                                     err );
            if (userDll->WAApiService != NULL) {
                userDll->Loaded = true;
                retcode = true;
            }; /* if */
        }; /* if */
    }; /* if */

    return retcode;
} /* wd40LoadUserDll */


sapdbwa_Bool wd40SetUseWebSessions( twd40UserDllP   userDll,
                                  sapdbwa_Bool      useWebSessions )
{
    sapdbwa_Bool ok = true;

    userDll->useWebSessions = useWebSessions;

    return ok;
} /* wd40SetUseWebSessions */


sapdbwa_Bool wd40UnloadUserDll( twd40UserDllP  userDll,
                              twd26ErrP      err       )
{
    char error[ERR_MSG_LEN];

    if (!userDll->Loaded)
        return(true);

    if (userDll->hLib) {
        if (!sqlFreeLibrary(userDll->hLib, error, sizeof( error ))) {
            wd26SetErr( err,
                        ERR_USERDLL_UNLOAD_FAILED_WD26,
                        userDll->DllName, error );
            return(false);
        }; /* if */
    }; /* if */

    userDll->hLib         = NULL;
    userDll->WAApiInit    = NULL;
    userDll->WAApiExit    = NULL;
    userDll->WAApiService = NULL;
    userDll->Loaded       = false;
    userDll->initialized  = false;

    return true;
} /* wd40UnloadUserDll */


sapdbwa_Bool wd40WriteLogMsg( twd40UserDllP UserDll, const char *msg )
{
    if (UserDll->log != NULL) {
        wd25WriteLogMsg( UserDll->log, (char*) msg );
        return true;
    } else {
        return false;
    }; /* else */
} /* wd40WriteLogMsg */

/*!**********************************************************************

  EndChapter: UserDll

************************************************************************/

sqlPROC wd40_GetProcAddress( twd40UserDllP  userDll,
                             const char    *funcName,
                             twd26ErrP      err )
{
    char     error[ERR_MSG_LEN];
    sqlPROC  procAddr = NULL;

    /* get init function */
    procAddr = sqlGetProcAddress( userDll->hLib,
                                  (char*) funcName,
                                  error,
                                  sizeof( error ));
    if (procAddr == NULL) {
        wd26SetErr( err,
                    ERR_NO_SUCH_FUNCTION_WD26,
                    funcName,
                    userDll->DllName );
        sqlFreeLibrary( userDll->hLib, error, sizeof( error ) );
        userDll->hLib = NULL;
    }; /* if */

    return procAddr;
} /* wd40_GetProcAddress */
