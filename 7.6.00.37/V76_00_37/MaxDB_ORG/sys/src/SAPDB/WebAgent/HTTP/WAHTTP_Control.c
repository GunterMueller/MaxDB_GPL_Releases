/*!**********************************************************************

  module: WAHTTP_Control.c

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: SAP DB WWW

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

#define MODULE_NAME "WAHTTP_Control.c"

/****************************************************************************
 ***
 *** Include
 ***
 ****************************************************************************/

#ifdef WIN32
#include <windows.h>
#endif

#include <string.h>
#include <stdio.h>

#include "SAPDB/WebAgent/HTTP/WAHTTP_Control.h"
#include "SAPDB/WebAgent/HTTP/WAHTTP_Misc.h"
#include "SAPDB/WebAgent/HTTP/WAHTTP_Memory.h"

#include "gsp05.h"

/****************************************************************************
 ***
 *** Defines
 ***
 ****************************************************************************/

/* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1121656 */
#define PID_DIR_1   "/var/spool/sql/pid/serverpids"
#define PID_DIR_2   "/usr/spool/sql/pid/serverpids"

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

void Stop();

#ifdef WIN32

char* GetLastErrorText( char          *buffer,
					    sapdbwa_UInt4  size    );

BOOL ReportStatusToSCMgr( DWORD currentState,
                          DWORD win32ExitCode,
                          DWORD waitHint      );

VOID AddToMessageLog( char *logMsg );

void WINAPI ServiceMain( DWORD argc, LPTSTR *argv );
VOID WINAPI ServiceCtrl( DWORD ctrlCode );

#endif

#ifdef WIN32
void WritePIDFile( char            *execDir,
                   DWORD            pid,
                   sapdbwa_UInt2    port );
#else
void WritePIDFile( char          *execDir,
                   int            pid,
                   sapdbwa_UInt2  port);
#endif

/****************************************************************************
 ***
 *** GetExecDirectory
 ***
 ****************************************************************************/

sapdbwa_Bool GetExecDirectory( char          *cmdArg,
							   char          *directory,
							   sapdbwa_UInt2  size      )
{

	char		tmpPath[200];
	char		execPath[200];
	char		currentDirectory[MAX_FILENAME_LEN+1];
	char		*separator = NULL;

	/* Get current directory	*/
	currentDirectory[0] = '\0';
	if( !getcwd( currentDirectory, sizeof(currentDirectory) ) ) {
		separator = strrchr(cmdArg, DIRPATH_SEP);
		if (!separator)
			execPath[0] = '\0';
		else if (separator - cmdArg + 1 > sizeof(execPath)) {
			printf("%s:%s\nCurrent directory path too long!\n", SERVICE_DISPLAY_NAME, cmdArg);
			return sapdbwa_False;
		} else {
			strncpy(execPath, cmdArg, separator - cmdArg + 1);
			execPath[separator - cmdArg + 1] = '\0';
		}

		if (execPath[1] != ':' && execPath[0] != DIRPATH_SEP) {
			/* this is a relative path */
			sprintf(tmpPath, "%s%c%s", currentDirectory, DIRPATH_SEP, execPath);
		} else
			strcpy(tmpPath, execPath);

		strcpy( currentDirectory, tmpPath );
	}

	/* Set return value	*/
	strncpy( directory, currentDirectory, size );

	return sapdbwa_True;

}

/****************************************************************************
 ***
 *** Start
 ***
 ****************************************************************************/

int Start( char                             *execDir,
           sapdbwa_Bool                      debug,
           struct wahttp_alternate_settings *alternateSettings,
           char                             *errorText )
{

#ifdef WIN32
    DWORD       pid = 0;
#else
    int         pid = 0;
#endif
sapdbwa_UInt2   port = 0;

#ifdef WIN32
	if ( debug == sapdbwa_False ) {
		/* Report the status to the service control manager	*/
		if ( !ReportStatusToSCMgr( SERVICE_START_PENDING, NO_ERROR, 3000) ) {
			/* Try to report the stopped status to the service control manager	*/
			if ( statusHandle ) {
				ReportStatusToSCMgr( SERVICE_STOPPED, error, 0 );
			}

			return 0;
		}
	}
#endif

	/* Init web server	*/
	if (!InitWebServer( execDir, alternateSettings, errorText ))
		return 0;

    /* Determine pid and port and write out */
#ifdef WIN32
    pid = GetCurrentProcessId();
#else
    pid = getpid();
#endif
    port = GetPort();

    printf("PID[%d]\n", pid);
    printf("Port[%d]\n", port);

    WritePIDFile(execDir, pid, port);

#ifdef WIN32
	if ( debug == sapdbwa_False ) {
		/* Report the status to the service control manager	*/
		if ( !ReportStatusToSCMgr( SERVICE_START_PENDING, NO_ERROR, 3000) ) {
			/* Try to report the stopped status to the service control manager	*/
			if ( statusHandle ) {
				ReportStatusToSCMgr( SERVICE_STOPPED, error, 0 );
			}

			return 0;
		}
	}
#endif

	/* Start web server	*/
	StartWebServer();

#ifdef WIN32
	if ( debug == sapdbwa_False ) {
		/* Report the status to the service control manager	*/
		if ( !ReportStatusToSCMgr( SERVICE_START_PENDING, NO_ERROR, 3000) ) {
			/* Try to report the stopped status to the service control manager	*/
			if ( statusHandle ) {
				ReportStatusToSCMgr( SERVICE_STOPPED, error, 0 );
			}

			return 0;
		}
	}
#endif

	/* Start infinite loop	*/
	WaitWebServer( debug );

    return 1;

}

/****************************************************************************
 ***
 *** Stop
 ***
 ****************************************************************************/

void Stop() 
{

	StopWebServer();

#ifdef WIN32
	ReportStatusToSCMgr( SERVICE_STOPPED, NO_ERROR, 1000);
#endif

	return;

}

#ifdef WIN32

/****************************************************************************
 ***
 *** SetDispatchTable
 ***
 ****************************************************************************/

sapdbwa_Bool SetDispatchTable()
{

    SERVICE_TABLE_ENTRY dispatchTable[] =
    {
        { TEXT(SERVICE_INTERNAL_NAME), (LPSERVICE_MAIN_FUNCTION)ServiceMain },
        { NULL, NULL }
    };

    if (!StartServiceCtrlDispatcher(dispatchTable)) {
        AddToMessageLog( "Start service failed." );

		return sapdbwa_False;
	}

	return sapdbwa_True;

}

/****************************************************************************
 ***
 *** ServiceMain
 ***
 ****************************************************************************/

void WINAPI ServiceMain( DWORD argc, LPTSTR *argv )
{

    SC_HANDLE                            schService;
    SC_HANDLE                            schSCManager;
	QUERY_SERVICE_CONFIG                *serviceConfig = NULL;
	char		                         execDir[MAX_FILENAME_LEN+1] = "";
	char				                *dirEnd = NULL;
	sapdbwa_UInt4		                 bytesNeeded = 0;
	sapdbwa_Bool		                 ok = sapdbwa_False;
    char                                 errorText[512];
    struct wahttp_alternate_settings     alternateSettings;

    memset(&alternateSettings, 0, sizeof(struct wahttp_alternate_settings));

    /* register our service control handler	*/
    statusHandle = RegisterServiceCtrlHandler( SERVICE_INTERNAL_NAME, ServiceCtrl );
    if (!statusHandle) {
        return;
	}

    status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    status.dwServiceSpecificExitCode = 0;


    /* Report the status to the service control manager	*/
    if ( !ReportStatusToSCMgr( SERVICE_START_PENDING, NO_ERROR, 3000) ) {
		/* Try to report the stopped status to the service control manager	*/
		if (statusHandle) {
			ReportStatusToSCMgr( SERVICE_STOPPED, error, 0 );
		}

        return;
	}

	/* Open service control manager	*/
    wahttp_SQLALLOCAT( 1024, &serviceConfig, &ok );
    if ( ok == sapdbwa_False ) {
		AddToMessageLog( "Service - Memory allocation failed!" );
        return;
    };
	
    schSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );
    if ( schSCManager ) {
        schService = OpenService(schSCManager, SERVICE_INTERNAL_NAME, SERVICE_ALL_ACCESS);

        if (schService) {
			if ( !QueryServiceConfig( schService, serviceConfig, 1024, &bytesNeeded ) ) {
				error = GetLastError();
				switch ( error ) {
				case ERROR_INSUFFICIENT_BUFFER:
					AddToMessageLog( "Service - Insufficient buffer!" );
					break;
				case ERROR_ACCESS_DENIED:
					AddToMessageLog( "Service - Access denied!" );
					break;
				case ERROR_INVALID_HANDLE:
					AddToMessageLog( "Service - Invalid handle!" );
					break;
				default:
					break;
				}
				AddToMessageLog( "Cannot determine binary path!" );

				CloseServiceHandle( schService );
				CloseServiceHandle( schSCManager );

				return;
			} else {
				strncpy( execDir, serviceConfig->lpBinaryPathName, MAX_FILENAME_LEN );
				execDir[MAX_FILENAME_LEN] = '\0';

				dirEnd = strrchr( execDir, DIRPATH_SEP );
				if ( !dirEnd ) {
					execDir[0] = '\0';
				} else {
					*dirEnd = '\0';
				}
			} 

			CloseServiceHandle( schService );
		}

		CloseServiceHandle( schSCManager );
	}
	
	/* Our real start	*/
    if (!Start( execDir, sapdbwa_False, &alternateSettings, errorText )) {
		AddToMessageLog(errorText);
    }

    return;

}

/****************************************************************************
 ***
 *** ServiceCtrl
 ***
 ****************************************************************************/

VOID WINAPI ServiceCtrl( DWORD ctrlCode )
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
            ReportStatusToSCMgr( SERVICE_STOP_PENDING, NO_ERROR, 0 );
            Stop();
            return;

        /* Update the service status	*/
        case SERVICE_CONTROL_INTERROGATE:
            break;

        /* Unknown control code	*/
        default:
            break;

    }

    ReportStatusToSCMgr( status.dwCurrentState, NO_ERROR, 0 );

	return;

}

/****************************************************************************
 ***
 *** ServiceIsRunning
 ***
 ****************************************************************************/

sapdbwa_Bool ServiceIsRunning()
{

    if ( !ReportStatusToSCMgr( SERVICE_RUNNING, NO_ERROR, 3000) ) {
		/* Try to report the stopped status to the service control manager	*/
		if (statusHandle) {
			ReportStatusToSCMgr( SERVICE_STOPPED, error, 0 );
		}

        return sapdbwa_False;
	}

	return sapdbwa_True;

}

/****************************************************************************
 ***
 *** GetLastErrorText
 ***
 ****************************************************************************/

char* GetLastErrorText( char          *buffer,
					    sapdbwa_UInt4  size    )
{

    sapdbwa_UInt4   rc;
    LPVOID		    temp;
    char           *msg = NULL;
    DWORD           msgLen = 0;
    DWORD           errorId = GetLastError();

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

/****************************************************************************
 ***
 *** InstallAsService
 ***
 ****************************************************************************/

sapdbwa_Bool InstallAsService( char *execDir )
{

	char		 fileName[MAX_FILENAME_LEN+1] = "";
	char		*slash = NULL;
	char		 err[MAX_ERROR_TEXT_LEN+1];
    SC_HANDLE    schService;
    SC_HANDLE    schSCManager;

	/* Build filename	*/
	slash = execDir+strlen(execDir)-1;
	if (*slash==DIRPATH_SEP) {
		sp77sprintf( fileName, MAX_FILENAME_LEN, "%swahttp.exe", execDir );
	} else {
		sp77sprintf( fileName, MAX_FILENAME_LEN, "%s%cwahttp.exe", execDir, DIRPATH_SEP );
	}

	/* Open service control manager	*/
    schSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );

	/* Create service entry	*/
    if ( schSCManager ) {
        schService = CreateService( schSCManager, SERVICE_INTERNAL_NAME,  SERVICE_DISPLAY_NAME,
									SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,
									SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL, fileName,
									NULL, NULL, NULL, NULL, NULL );

        if ( schService ) {
			slash = execDir+strlen(execDir)-1;
			if (*slash==DIRPATH_SEP) {
				sp77sprintf( fileName, MAX_FILENAME_LEN, "%swahttp.conf", execDir );
			} else {
				sp77sprintf( fileName, MAX_FILENAME_LEN, "%s%cwahttp.conf", execDir, DIRPATH_SEP );
			}
            printf("SAP DB WWW service installed successfully\nService will use %s\n", fileName );
            CloseServiceHandle(schService);
        } else {
            printf("SAP DB WWW service installation failed - %s\n", GetLastErrorText(err, MAX_ERROR_TEXT_LEN) );
			return sapdbwa_False;
        }

        CloseServiceHandle(schSCManager);
    } else {
		printf("SAP DB WWW service installation failed - %s\n", GetLastErrorText(err, MAX_ERROR_TEXT_LEN) );
		return sapdbwa_False;
	}

	return sapdbwa_True;

}

/****************************************************************************
 ***
 *** RemoveAsService
 ***
 ****************************************************************************/

sapdbwa_Bool RemoveAsService()
{

	char		    err[MAX_ERROR_TEXT_LEN+1];
    SC_HANDLE       schService;
    SC_HANDLE       schSCManager;

    schSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );
    if ( schSCManager ) {
        schService = OpenService(schSCManager, SERVICE_INTERNAL_NAME, SERVICE_ALL_ACCESS);

        if (schService) {
            /* try to stop the service	*/
            if ( ControlService( schService, SERVICE_CONTROL_STOP, &status ) ) {
                printf( "Stopping %s.", SERVICE_DISPLAY_NAME );
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
                    printf( "\n%s stopped.\n", SERVICE_DISPLAY_NAME );
                else
                    printf( "\n%s failed to stop.\n", SERVICE_DISPLAY_NAME );

            }

            /* now remove the service	*/
            if( DeleteService(schService) )
                printf( "%s removed.\n", SERVICE_DISPLAY_NAME );
            else {
                printf( "Removing service failed - %s\n", GetLastErrorText(err,MAX_ERROR_TEXT_LEN) );
				return sapdbwa_False;
			}

            CloseServiceHandle(schService);
        } else {
            printf( "Removing service failed - %s\n", GetLastErrorText(err,MAX_ERROR_TEXT_LEN) );
			return sapdbwa_False;
		}

        CloseServiceHandle(schSCManager);
    } else {
        printf( "Removing service failed - %s\n", GetLastErrorText(err,MAX_ERROR_TEXT_LEN) );
		return sapdbwa_False;
	}

	return sapdbwa_True;

}

/****************************************************************************
 ***
 *** ReportStatusToSCMgr
 ***
 ****************************************************************************/

BOOL ReportStatusToSCMgr( DWORD currentState,
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
        AddToMessageLog( "SetServiceStatus" );
    }

    return result;
}

/****************************************************************************
 ***
 *** AddToMessageLog
 ***
 ****************************************************************************/

VOID AddToMessageLog( char *logMsg )
{

    char     msg[256];
    HANDLE   eventSource;
    char    *strings[2];
	DWORD	 error = 0;


    error = GetLastError();

    /* Use event logging to log the error	*/
    eventSource = RegisterEventSource( NULL, SERVICE_INTERNAL_NAME );

	sp77sprintf( msg, 256, "%s error: %d", SERVICE_INTERNAL_NAME, error );
    strings[0] = msg;
    strings[1] = logMsg;

    if ( eventSource != NULL ) {
        ReportEvent( eventSource, EVENTLOG_ERROR_TYPE, 0, 0, NULL, 2, 0, strings, NULL );
        DeregisterEventSource( eventSource );
    }

}

#endif

/****************************************************************************
 ***
 *** WritePIDFile
 ***
 ****************************************************************************/

#ifdef WIN32
void WritePIDFile( char            *execDir,
                   DWORD            pid,
                   sapdbwa_UInt2    port )
#else
void WritePIDFile( char          *execDir,
                   int            pid,
                   sapdbwa_UInt2  port)
#endif
{

    sapdbwa_Int4        fileDesc = -1;
    tsp05_RteFileError  err;
    char                errorText[1024] = "";
    char                fileName[1024] = "";
    char                pidString[64] = "";

    /* Build PID string */
    sp77sprintf(pidString, 64, "%d\n%d", pid, port);

    /* Build file name */
    /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1121656 */
#ifdef WIN32
    sp77sprintf(fileName, 1024, "%s/wahttp%d.pid", execDir, port);
#else
    sp77sprintf(fileName, 1024, "%s/wahttp%d.pid", PID_DIR_1, port);
#endif

    /* Open PID file */
    sqlfopenc( fileName, sp5vf_text, sp5vf_write, sp5bk_unbuffered, &fileDesc, &err );
    if ( err.sp5fe_result != vf_ok ) {
        /* http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1121656 */
#ifdef WIN32
        /* Error handling */
        sp77sprintf( errorText, 1024, "Cannot open file: %s", fileName );
        printf(errorText);

        return;
#else
        /* Second try */
        sp77sprintf(fileName, 1024, "%s/wahttp%d.pid", PID_DIR_2, port);
        sqlfopenc( fileName, sp5vf_text, sp5vf_write, sp5bk_unbuffered, &fileDesc, &err );
        if ( err.sp5fe_result != vf_ok ) {
            sp77sprintf( errorText, 1024, "Cannot open file: %s", fileName );
            printf(errorText);

            return;
        }
#endif
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
