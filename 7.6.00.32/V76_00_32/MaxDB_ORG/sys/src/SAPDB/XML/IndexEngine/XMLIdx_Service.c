/*!**********************************************************************

  module: XMLIdx_Service.c

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: XML Index Engine

  description:  Windows Service Control Manager support

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

/****************************************************************************
 ***
 *** Include
 ***
 ****************************************************************************/

#ifdef WIN32
#include <windows.h>
#endif

/*---------------------------------------------------------------------------*/

#include <string.h>
#include <stdio.h>

/*---------------------------------------------------------------------------*/

#include "gsp05.h"

/*---------------------------------------------------------------------------*/

#include "SAPDB/XML/IndexEngine/XMLIdx_Service.h"
#include "SAPDB/XML/IndexEngine/XMLIdx_Engine.h"

#include "SAPDB/XML/Common/XMLCom_Common.h"
#include "SAPDB/XML/Error/XMLError_Error.h"
#include "SAPDB/XML/Memory/XMLMem_Memory.h"
#include "SAPDB/XML/Settings/XMLSet_Settings.h"
#include "SAPDB/XML/SessionPool/XMLSP_SessionPool.h"
#include "SAPDB/XML/IndexingMetadataAPI/XMLIMAPI_Definition.h"

/****************************************************************************
 ***
 *** Defines
 ***
 ****************************************************************************/

/****************************************************************************
 ***
 *** Types and Variables
 ***
 ****************************************************************************/

#ifdef WIN32

SERVICE_STATUS          status;
SERVICE_STATUS_HANDLE   statusHandle;
DWORD                   error = 0;
HANDLE					serverStopEvent = NULL;

#endif

/****************************************************************************
 ***
 *** Prototypes
 ***
 ****************************************************************************/

void Service_Stop();

#ifdef WIN32

char* Service_GetLastErrorText( char          *buffer,
								SAPDB_UInt4  size    );

BOOL Service_ReportStatusToSCMgr( DWORD currentState,
								  DWORD win32ExitCode,
								  DWORD waitHint      );

VOID Service_AddToMessageLog( char *logMsg );

void WINAPI Service_Main( DWORD argc, LPTSTR *argv );
VOID WINAPI Service_Ctrl( DWORD ctrlCode );

#endif

#ifdef WIN32
void Service_WritePIDFile( char  *execDir,
						   DWORD  pid );
#else
void Service_WritePIDFile( char *execDir,
						   int   pid );
#endif

SAPDB_Bool RegisterService( char *regServerNode,
                            char *regServerName,
                            char *regUser,
                            char *regPassword,
                            char *regTraceFilename,
                            char *engineServerNode,
                            char *engineFileName,
                            char *registerName,
                            char *registerDescription,
                            char *registerLogFile,
                            char *indexingServiceName,
                            char *indexingLibFileName,
                            char *indexingLibVersion,
                            char *indexingLibDescription,
                            char *errorText );

/***********************************************************************

	Public Functions

 ***********************************************************************/

int Service_Start( char         *execDir,
                   SAPDB_Bool    debug,
                   char         *iniFile,
                   char         *sectionName,
                   char         *errorText )
{

#ifdef WIN32
    DWORD       pid = 0;
#else
    int         pid = 0;
#endif

#ifdef WIN32
	if ( debug == SAPDB_FALSE ) {
		/* Report the status to the service control manager	*/
		if ( !Service_ReportStatusToSCMgr( SERVICE_START_PENDING, NO_ERROR, 3000) ) {
			/* Try to report the stopped status to the service control manager	*/
			if ( statusHandle ) {
				Service_ReportStatusToSCMgr( SERVICE_STOPPED, error, 0 );
			}

			return 0;
		}
	}
#endif

	/* Init engine	*/
	if (!Engine_Create(execDir, iniFile, sectionName, errorText))
		return 0;

    /* Determine pid and write out */
#ifdef WIN32
    pid = GetCurrentProcessId();
#else
    pid = getpid();
#endif

    printf("[INFO] PID[%d]\n", pid);

    Service_WritePIDFile(execDir, pid);

#ifdef WIN32
	if ( debug == SAPDB_FALSE ) {
		/* Report the status to the service control manager	*/
		if ( !Service_ReportStatusToSCMgr( SERVICE_START_PENDING, NO_ERROR, 3000) ) {
			/* Try to report the stopped status to the service control manager	*/
			if ( statusHandle ) {
				Service_ReportStatusToSCMgr( SERVICE_STOPPED, error, 0 );
			}

			return 0;
		}
	}
#endif

	/* Start engine */
	Engine_Start();

#ifdef WIN32
	if ( debug == SAPDB_FALSE ) {
		/* Report the status to the service control manager	*/
		if ( !Service_ReportStatusToSCMgr( SERVICE_START_PENDING, NO_ERROR, 3000) ) {
			/* Try to report the stopped status to the service control manager	*/
			if ( statusHandle ) {
				Service_ReportStatusToSCMgr( SERVICE_STOPPED, error, 0 );
			}

			return 0;
		}
	}
#endif

	/* Start infinite loop	*/
	Engine_Wait( debug );

    return 1;

}

/*===========================================================================*/

#ifdef WIN32

SAPDB_Bool Service_SetDispatchTable()
{
/*
    SERVICE_TABLE_ENTRY dispatchTable[] =
    {
        { TEXT(XMLCOM_SERVICE_INTERNAL_NAME), (LPSERVICE_MAIN_FUNCTION)Service_Main },
        { NULL, NULL }
    };
*/


    SERVICE_TABLE_ENTRY dispatchTable[] =
    {
        { TEXT(""), (LPSERVICE_MAIN_FUNCTION)Service_Main },
        { NULL, NULL }
    };

    if (!StartServiceCtrlDispatcher(dispatchTable)) {
        Service_AddToMessageLog( "Start service failed." );

		return SAPDB_FALSE;
	}

	return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool Service_IsRunning()
{

    if ( !Service_ReportStatusToSCMgr( SERVICE_RUNNING, NO_ERROR, 3000) ) {
		/* Try to report the stopped status to the service control manager	*/
		if (statusHandle) {
			Service_ReportStatusToSCMgr( SERVICE_STOPPED, error, 0 );
		}

        return SAPDB_FALSE;
	}

	return SAPDB_TRUE;

}

#endif
/*===========================================================================*/

SAPDB_Bool Service_InstallAsService( char  *execDir,
                                     char  *newRegistrationName )
{

    XMLSet_Settings  hSettings;
	char		    *slash = NULL;
	char		     errorText[XMLERROR_MAX_ERROR_TEXT_LEN+1];
    char             usedIniFile[XMLSET_MAX_VALUE_LEN+1];
    char             engineFilename[XMLSET_MAX_VALUE_LEN+1];
    char             registrationName[XMLSET_MAX_VALUE_LEN+1];
#ifdef WIN32
    SC_HANDLE        schService;
    SC_HANDLE        schSCManager;
#endif

    /* Checks */
    if (!newRegistrationName) {
        strcpy(registrationName, XMLSET_REG_SECTION_DEFAULT);
    } else {
        strcpy(registrationName, newRegistrationName);
    }

	/* Build filenames */
#ifdef WIN32
	slash = execDir+strlen(execDir)-1;
	if (*slash == XMLCOM_DIRPATH_SEP) {
		sp77sprintf( engineFilename, XMLCOM_MAX_FILENAME_LEN, "%ssapdbxie.exe", execDir );
	} else {
		sp77sprintf( engineFilename, XMLCOM_MAX_FILENAME_LEN, "%s%csapdbxie.exe", execDir, XMLCOM_DIRPATH_SEP );
	}
#else
	slash = execDir+strlen(execDir)-1;
	if (*slash == XMLCOM_DIRPATH_SEP) {
		sp77sprintf( engineFilename, XMLCOM_MAX_FILENAME_LEN, "%ssapdbxie", execDir );
	} else {
		sp77sprintf( engineFilename, XMLCOM_MAX_FILENAME_LEN, "%s%csapdbxie", execDir, XMLCOM_DIRPATH_SEP );
	}
#endif

    /* Read settings from registry */
    if (!Settings_Create(&hSettings, execDir, NULL, registrationName, usedIniFile, NULL)) {
        printf("[ERROR] Service installation failed!\n");
#ifdef WIN32
        printf("        Could not read registry: \"%s\\%s\"\n", usedIniFile, registrationName);
#else
        printf("        Could not read registry: \"%s -> %s\"\n", usedIniFile, registrationName);
#endif
		return SAPDB_FALSE;
    }

    if (RegisterService(Settings_GetRegistryNode(hSettings),
                        Settings_GetRegistryName(hSettings),
                        Settings_GetRegistryUser(hSettings),
                        Settings_GetRegistryPassword(hSettings),
                        Settings_GetRegistryTraceFilename(hSettings),
                        Settings_GetEngineNode(hSettings),
                        engineFilename,
                        registrationName,
                        Settings_GetDescription(hSettings),
                        Settings_GetLogFilename(hSettings),
                        Settings_GetIndexingServiceName(hSettings),
                        Settings_GetIndexingLibFile(hSettings),
                        Settings_GetIndexingLibVersion(hSettings),
                        Settings_GetIndexingLibDescription(hSettings),
                        errorText ) == SAPDB_FALSE) {
        printf("[WARNING] Service registration failed!\n");
        printf("          Could not register service at description store:\n");
        printf("          %s\n", errorText);
    } else {
        printf("[INFO] Registration of service \"%s\" successfully\n", registrationName);
    }

#ifdef WIN32
	/* Open service control manager	*/
    schSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );

	/* Create service entry	*/
    if ( schSCManager ) {
        schService = CreateService( schSCManager, registrationName,  registrationName,
									SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,
									SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL, engineFilename,
									NULL, NULL, NULL, NULL, NULL );

        if ( schService ) {
            printf("[INFO] Service \"%s\" installed successfully\n", registrationName);
#ifdef WIN32
            printf("       Service will use \"%s\\%s\"\n", usedIniFile, registrationName);
#else
            printf("       Service will use \"%s -> %s\"\n", usedIniFile, registrationName);
#endif
            CloseServiceHandle(schService);
        } else {
            CloseServiceHandle(schSCManager);
            printf("[ERROR] Service installation failed:\n");
            printf("        %s\n", Service_GetLastErrorText(errorText, XMLERROR_MAX_ERROR_TEXT_LEN));
			return SAPDB_FALSE;
        }

        CloseServiceHandle(schSCManager);
    } else {
        printf("[ERROR] Service installation failed:\n");
        printf("        %s\n", Service_GetLastErrorText(errorText, XMLERROR_MAX_ERROR_TEXT_LEN));
		return SAPDB_FALSE;
	}
#endif

	return SAPDB_TRUE;

}

/*===========================================================================*/

SAPDB_Bool Service_RemoveAsService( char *registrationNameToRemove )
{

	char		    errorText[XMLERROR_MAX_ERROR_TEXT_LEN+1];
    char            registrationName[XMLSET_MAX_VALUE_LEN+1];
#ifdef WIN32
    SC_HANDLE       schService;
    SC_HANDLE       schSCManager;
#endif

    /* Checks */
    if (!registrationNameToRemove) {
        strcpy(registrationName, XMLSET_REG_SECTION_DEFAULT);
    } else {
        strcpy(registrationName, registrationNameToRemove);
    }

#ifdef WIN32
    schSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );
    if ( schSCManager ) {
        schService = OpenService(schSCManager, registrationName, SERVICE_ALL_ACCESS);

        if (schService) {
            /* try to stop the service	*/
            if ( ControlService( schService, SERVICE_CONTROL_STOP, &status ) ) {
                printf( "[INFO] SAPDB XIE stopping \"%s\".", registrationName );
                Sleep( 1000 );

                while( QueryServiceStatus( schService, &status ) ) {
                    if ( status.dwCurrentState == SERVICE_STOP_PENDING ) {
                        printf( "." );
                        Sleep( 1000 );
                    }
                    else
                        break;
                }

                if ( status.dwCurrentState == SERVICE_STOPPED )
                    printf( "\n[INFO] SAPDB XIE \"%s\" stopped.\n", registrationName);
                else
                    printf( "\n[INFO] SAPDB XIE \"%s\" failed to stop.\n", registrationName);

            }

            /* now remove the service	*/
            if( DeleteService(schService) )
                printf( "[INFO] Service \"%s\" removed.\n", registrationName);
            else {
                printf( "[INFO] Removing service failed - %s\n", Service_GetLastErrorText(errorText,XMLERROR_MAX_ERROR_TEXT_LEN) );
				return SAPDB_FALSE;
			}

            CloseServiceHandle(schService);
        } else {
            printf( "[INFO] Removing service failed - %s\n", Service_GetLastErrorText(errorText,XMLERROR_MAX_ERROR_TEXT_LEN) );
			return SAPDB_FALSE;
		}

        CloseServiceHandle(schSCManager);
    } else {
        printf( "[INFO] Removing service failed - %s\n", Service_GetLastErrorText(errorText,XMLERROR_MAX_ERROR_TEXT_LEN) );
		return SAPDB_FALSE;
	}
#endif

    printf("[INFO] Un-registration of SAP DB XIE service \"%s\" successfully\n", registrationName);

	return SAPDB_TRUE;

}

/*===========================================================================*/

/***********************************************************************

	Private Functions

 ***********************************************************************/

#ifdef WIN32

char* Service_GetLastErrorText( char          *buffer,
                                SAPDB_UInt4   size    )
{

    SAPDB_UInt4     rc;
    LPVOID           temp;
    char            *msg = NULL;
    DWORD            msgLen = 0;
    DWORD            errorId = GetLastError();

    /*
    rc = FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |FORMAT_MESSAGE_ARGUMENT_ARRAY,
                        NULL, errorId, LANG_NEUTRAL, (LPTSTR)&temp, 0, NULL );
    */

    rc = FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
                        NULL, errorId, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&temp, 0, NULL );

    /* Is buffer large enough?	*/
    msg = (char*)temp;
    if ( !rc || !msg) {
		sp77sprintf( buffer, size, "[%d] (No error text is available [%d])", errorId, GetLastError() );
	} else  {
        msgLen = lstrlen(msg);
        if (msgLen >= 2) {
            msg[msgLen-2] = '\0';	/* We don't need cr and newline	*/
        }
		sp77sprintf( buffer, size, "%s [%d]", msg, errorId );
    }

    if ( temp )
        LocalFree((HLOCAL) temp );

    return buffer;

}

/*===========================================================================*/

BOOL Service_ReportStatusToSCMgr( DWORD currentState,
                                  DWORD win32ExitCode,
                                  DWORD waitHint      )
{

    BOOL         result = TRUE;
    static DWORD checkPoint = 1;

    if ( currentState == SERVICE_START_PENDING )
        status.dwControlsAccepted = 0;
    else
        status.dwControlsAccepted = SERVICE_ACCEPT_STOP;

    status.dwCurrentState = currentState;
    status.dwWin32ExitCode = win32ExitCode;
    status.dwWaitHint = waitHint;

    if ( ( currentState == SERVICE_RUNNING ) ||
         ( currentState == SERVICE_STOPPED ) )
        status.dwCheckPoint = 0;
    else
        status.dwCheckPoint = checkPoint++;


    /* Report the status of the service to the service control manager	*/
    if ( ! ( result = SetServiceStatus( statusHandle, &status ) ) ) {
        Service_AddToMessageLog( "SetServiceStatus" );
    }

    return result;
}

/*===========================================================================*/

VOID Service_AddToMessageLog( char *logMsg )
{

    char     msg[256];
    HANDLE   eventSource;
    char    *strings[2];
	DWORD	 error = 0;


    error = GetLastError();

    /* Use event logging to log the error	*/
    eventSource = RegisterEventSource( NULL, "WinApp" );

	sp77sprintf( msg, 256, "%s error: %d", XMLCOM_SERVICE_DISPLAY_NAME, error );
    strings[0] = msg;
    strings[1] = logMsg;

    if ( eventSource != NULL ) {
        ReportEvent( eventSource, EVENTLOG_ERROR_TYPE, 0, 0, NULL, 2, 0, strings, NULL );
        DeregisterEventSource( eventSource );
    }

}

/*===========================================================================*/

void WINAPI Service_Main( DWORD argc, LPTSTR *argv )
{

    SC_HANDLE             schService;
    SC_HANDLE             schSCManager;
	QUERY_SERVICE_CONFIG *serviceConfig = NULL;
	char		          execDir[XMLCOM_MAX_FILENAME_LEN+1] = "";
	char				 *dirEnd = NULL;
	SAPDB_UInt4		      bytesNeeded = 0;
	SAPDB_Bool            ok = SAPDB_FALSE;
    char                  errorText[512];
    char                  extendedRegPath[XMLCOM_MAX_FILENAME_LEN+1] = "";

    /* register our service control handler	*/
    statusHandle = RegisterServiceCtrlHandler( "", Service_Ctrl );

    if (!statusHandle) {
        return;
	}

    status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    status.dwServiceSpecificExitCode = 0;


    /* Report the status to the service control manager	*/
    if ( !Service_ReportStatusToSCMgr( SERVICE_START_PENDING, NO_ERROR, 3000) ) {
		/* Try to report the stopped status to the service control manager	*/
		if (statusHandle) {
			Service_ReportStatusToSCMgr( SERVICE_STOPPED, error, 0 );
		}

        return;
	}

	/* Open service control manager	*/
    XMLMem_Allocat( 1024, &serviceConfig, &ok );
    if ( ok == SAPDB_FALSE ) {
		Service_AddToMessageLog( "Service - Memory allocation failed!" );
        return;
    };
	
    schSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );
    if ( schSCManager ) {
        schService = OpenService(schSCManager, argv[0], SERVICE_ALL_ACCESS);

        if (schService) {
			if ( !QueryServiceConfig( schService, serviceConfig, 1024, &bytesNeeded ) ) {
				error = GetLastError();
				switch ( error ) {
				case ERROR_INSUFFICIENT_BUFFER:
					Service_AddToMessageLog( "Service - Insufficient buffer!" );
					break;
				case ERROR_ACCESS_DENIED:
					Service_AddToMessageLog( "Service - Access denied!" );
					break;
				case ERROR_INVALID_HANDLE:
					Service_AddToMessageLog( "Service - Invalid handle!" );
					break;
				default:
					break;
				}
				Service_AddToMessageLog( "Cannot determine binary path!" );

				CloseServiceHandle( schService );
				CloseServiceHandle( schSCManager );

				return;
			} else {
				strncpy( execDir, serviceConfig->lpBinaryPathName, XMLCOM_MAX_FILENAME_LEN );
				execDir[XMLCOM_MAX_FILENAME_LEN] = '\0';

				dirEnd = strrchr( execDir, XMLCOM_DIRPATH_SEP );
				if ( !dirEnd ) {
					execDir[0] = '\0';
				} else {
					*dirEnd = '\0';
				}
			} 

			CloseServiceHandle( schService );
        } else {
            if (argv[0]) {
                sp77sprintf(errorText, 511, "Cannot open service: %s!", argv[0]);
                Service_AddToMessageLog(errorText);
            } else {
                Service_AddToMessageLog("No service to open");
            }

			CloseServiceHandle( schSCManager );

			return;
        }

		CloseServiceHandle( schSCManager );
	} else {
        Service_AddToMessageLog( "Cannot open service control manager");

		return;
    }
	

	/* Our real start	*/
    errorText[0] = '\0';
    if (!Service_Start(execDir, SAPDB_FALSE, NULL, argv[0], errorText )) {
		Service_AddToMessageLog(errorText);
    }

    return;

}

/*===========================================================================*/

VOID WINAPI Service_Ctrl( DWORD ctrlCode )
{

    /* Handle the requested control code	*/
    switch( ctrlCode ) {
        /* Stop the service.											*/
        /*																*/
        /* SERVICE_STOP_PENDING should be reported before				*/
        /* setting the Stop Event - hServerStopEvent - in				*/
        /* ServiceStop().  This avoids a race condition					*/
        /* which may result in a 1053 - The Service did not respond...	*/
        /* error.														*/
        case SERVICE_CONTROL_STOP:
            Service_ReportStatusToSCMgr( SERVICE_STOP_PENDING, NO_ERROR, 0 );
            Service_Stop();
            return;

        /* Update the service status	*/
        case SERVICE_CONTROL_INTERROGATE:
            break;

        /* Unknown control code	*/
        default:
            break;

    }

    Service_ReportStatusToSCMgr( status.dwCurrentState, NO_ERROR, 0 );

	return;

}

#endif

/*===========================================================================*/

#ifdef WIN32
void Service_WritePIDFile( char   *execDir,
                           DWORD   pid )
#else
void Service_WritePIDFile( char *execDir,
                           int   pid )
#endif
{

    SAPDB_Int4          fileDesc = -1;
    tsp05_RteFileError  err;
    char                errorText[1024] = "";
    char                fileName[1024] = "";
    char                pidString[64] = "";

    /* Build file name */
    sp77sprintf(fileName, 1024, "%s/sapdbxie.pid", execDir);

    /* Build PID string */
    sp77sprintf(pidString, 64, "%d\n", pid);

    /* Open PID file */
    sqlfopenc( fileName, sp5vf_text, sp5vf_write, sp5bk_unbuffered, &fileDesc, &err );
    if ( err.sp5fe_result != vf_ok ) {
        sp77sprintf( errorText, 1024, "Cannot open file: %s", fileName );
        printf(errorText);

        return;
    }

    /* Write out PID */
    sqlfwritec ( fileDesc, pidString, strlen(pidString), &err );
    if ( err.sp5fe_result != vf_ok ) {
        sp77sprintf( errorText, 1024, "Cannot write file: %s", fileName );
        printf(errorText);
    }

    /* Close file   */
    sqlfclosec( fileDesc, sp5vf_close_normal, &err );

}

/*===========================================================================*/

void Service_Stop() 
{

	Engine_Destroy();

#ifdef WIN32
	Service_ReportStatusToSCMgr( SERVICE_STOPPED, NO_ERROR, 1000);
#endif

	return;

}

/*===========================================================================*/

SAPDB_Bool RegisterService( char *regServerNode,
                            char *regServerName,
                            char *regUser,
                            char *regPassword,
                            char *regTraceFilename,
                            char *engineServerNode,
                            char *engineFileName,
                            char *registerName,
                            char *registerDescription,
                            char *registerLogFile,
                            char *indexingServiceName,
                            char *indexingLibFileName,
                            char *indexingLibVersion,
                            char *indexingLibDescription,
                            char *errorText )
{

    XMLSP_Session   hSessionRegistry = NULL;
    XMLIMAPI_Handle hIMApi = NULL;

    /* Open session to registry */
	if (!SP_OpenSession(&hSessionRegistry, regServerNode, regServerName, regUser, regPassword, regTraceFilename, NULL)) {
        strcpy(errorText, "Could not open session to registry DB");

        return SAPDB_FALSE;
	}

    /* Create xml modelling api handle */
    if (!XMLIMAPI_createHandle(&hIMApi,
                                SP_GetHEnv(hSessionRegistry),
                                SP_GetHDBC(hSessionRegistry))) {
        SP_CloseSession(hSessionRegistry, NULL);
        strcpy(errorText, "Could not connect to registry DB");

        return SAPDB_FALSE;
    }
    
    /* Register service at registry DB */
	if (XMLIMAPI_XieRegister(hIMApi, engineServerNode, engineFileName,
                                     registerName, registerDescription, registerLogFile,
                                     indexingServiceName, indexingLibFileName, indexingLibVersion,
                                     indexingLibDescription) == SAPDB_FALSE) {
        XMLIMAPI_destroyHandle(hIMApi);
        SP_CloseSession(hSessionRegistry, NULL);

        strcpy(errorText, "Could not register service at registry DB");

        return SAPDB_FALSE;
    }

    /* Destroy xml modelling api handle */
    XMLIMAPI_destroyHandle(hIMApi);

    /* Disconnect to registry DB */
    SP_CloseSession(hSessionRegistry, NULL);

    return SAPDB_TRUE;

}

/*===========================================================================*/

/***********************************************************************

	End

 ***********************************************************************/
