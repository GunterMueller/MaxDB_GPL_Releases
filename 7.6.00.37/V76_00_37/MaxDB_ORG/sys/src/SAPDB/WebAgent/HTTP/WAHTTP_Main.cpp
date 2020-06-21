/*



    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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
#ifdef WIN32
#include <windows.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sapdbwa_define.h>

#include "SAPDB/WebAgent/HTTP/WAHTTP_Misc.h"
#include "SAPDB/WebAgent/HTTP/WAHTTP_Control.h"
#include "SAPDB/WebAgent/HTTP/WAHTTP_Log.h"
#include "SAPDB/WebAgent/HTTP/WAHTTP_IniPar.h"

void PrintHelp();

/****************************************************************************
 ***
 *** main
 ***
 ****************************************************************************/

int main(int argc, char *argv[])
{

	char                                 clo = '\0';
	char                                 execDir[MAX_FILENAME_LEN+1] = "";
	char                                *optarg = NULL;
    char                                 errorText[512] = "";
    sapdbwa_Bool                         debug = sapdbwa_False;
    sapdbwa_Bool                         optionSet = sapdbwa_False;
    struct wahttp_alternate_settings     alternateSettings;

    /*
    InitAlternateSettings(alternateSettings);
    */
    memset(&alternateSettings, 0, sizeof(struct wahttp_alternate_settings));

#ifndef WIN32
    /* Set signal handler */
    Log_SetSignalHandler();
#endif

    WA_TRY {
	    /* Get current directory	*/
	    if ( !GetExecDirectory( argv[0], execDir, MAX_FILENAME_LEN ) ) {
		    exit( EXIT_FAILURE );
	    }
	    execDir[MAX_FILENAME_LEN] = '\0';

	    /* Check command line arguments	*/
        while ( (clo = GetOpt( argc, argv, "p:r:l:t:f:iudh", &optarg )) != -1) {
		    switch ( clo ) {

#ifdef WIN32
			    /* Install as service	*/
			    case 'i':
				    if ( !InstallAsService( execDir ) ) {
					    return EXIT_FAILURE;
				    } else {
					    return EXIT_SUCCESS;
				    }

				    break;

			    /* Remove as service	*/
			    case 'u':
				    if ( !RemoveAsService() ) {
					    return EXIT_FAILURE;
				    } else {
					    return EXIT_SUCCESS;
				    }

				    break;

			    /* Start to debug	*/
			    case 'd':
                    optionSet = sapdbwa_True;
                    debug = sapdbwa_True;
                    /*
                    if (!Start( execDir, sapdbwa_True, &alternateSettings, errorText )) {
                        printf("\n");
                        printf(errorText);
                        printf("\n");
					    return EXIT_FAILURE;
                    }

				    return EXIT_SUCCESS;
                    */

				    break;
#else
			    case 'i':
			    case 'u':
				    printf( "Not supported on this system!");
				    break;
#endif

                /* Specify an alternate registry path*/
                case 'f':
                    if ( optarg ) {
                        optionSet = sapdbwa_True;
                        sp77sprintf(alternateSettings.iniFile, 1024, "%s", optarg);
                        /*
                        if(!Start( execDir, sapdbwa_True, &alternateSettings, errorText )) {
                            printf("\n");
                            printf(errorText);
                            printf("\n");
                            return EXIT_FAILURE;
                        }
                        */
                    } else {
                        PrintHelp();
                    }

                    /*
                    return EXIT_SUCCESS;
                    */

                    break;

                /* Specify an alternate port */
                case 'p':
                    if ( optarg ) {
                        optionSet = sapdbwa_True;
                        sp77sprintf(alternateSettings.port, 32, "%s", optarg);
                    } else {
                        PrintHelp();
                    }

                    break;

                /* Specify an alternate document root */
                case 'r':
                    if ( optarg ) {
                        optionSet = sapdbwa_True;
                        sp77sprintf(alternateSettings.docRoot, 1024, "%s", optarg);
                    } else {
                        PrintHelp();
                    }

                    break;

                /* Specify an alternate log directory */
                case 'l':
                    if ( optarg ) {
                        optionSet = sapdbwa_True;
                        sp77sprintf(alternateSettings.logDir, 1024, "%s", optarg);
                    } else {
                        PrintHelp();
                    }

                    break;

                /* Specify an alternate max threads */
                case 't':
                    if ( optarg ) {
                        optionSet = sapdbwa_True;
                        sp77sprintf(alternateSettings.threads, 32, "%s", optarg);
                    } else {
                        PrintHelp();
                    }

                    break;

			    /* Help	*/
			    case 'h':
			    default:
				    PrintHelp();

				    return EXIT_SUCCESS;

				    break;
		    }

		    optarg = NULL;
	    }

#ifdef WIN32
        if (optionSet) {
            if (!Start( execDir, debug, &alternateSettings, errorText )) {
                printf("\n");
                printf(errorText);
                printf("\n");
		        return EXIT_FAILURE;
            }
        } else {
            SetDispatchTable();
        }
#else
        if (!Start( execDir, debug, &alternateSettings, errorText )) {
            printf("\n");
            printf(errorText);
            printf("\n");
		    return EXIT_FAILURE;
        }
#endif
    } WA_CATCH;

	return EXIT_SUCCESS;

}

/*========================================================================*/

void PrintHelp()
{

#ifdef WIN32
	printf("Usage: wahttp [-i] [-u] [-d] [-f file] [-p port] [-r root] [-l log] [-t threads] [-h]\n\n");

	printf("Options:\n");
	printf("  -i            : install an SAP DB WWW service\n");
	printf("  -u            : uninstall an SAP DB WWW service\n");
	printf("  -d            : to start SAP DB WWW from command line\n");
	printf("  -f file       : specify an alternate registry path\n");
	printf("  -p port       : specify an alternate port\n");
	printf("  -r root       : specify an alternate document root\n");
	printf("  -l log        : specify an alternate log directory\n");
	printf("  -t threads    : specify an alternate number of threads\n");
	printf("  -h            : this help\n\n");

	printf("Specify registry path in wahttp.conf\n");
	printf("or use option -f\n\n");

	printf("To start service : net start sapdbwww\n");
	printf("To stop service  : net stop sapdbwww\n");
#else
	printf("Usage: wahttp [-f file] [-p port] [-r root] [-l log] [-t threads] [-h]\n\n");

	printf("Options:\n");
	printf("  -f file : specify an alternate ini file\n");
	printf("  -p port       : specify an alternate port\n");
	printf("  -r root       : specify an alternate document root\n");
	printf("  -l log        : specify an alternate log directory\n");
	printf("  -t threads    : specify an alternate number of threads\n");
	printf("  -h      : this help\n\n");

	printf("To start service specify ini file in wahttp.conf\n");
	printf("or use option -f\n");
#endif

}
