/*!**********************************************************************

  module: XMLIdx_Main.c

  -----------------------------------------------------------------------

  responsible:  Markus Oezgen

  special area: XML Indexing Engine

  description:  Main startup functionalities

  see also:

  -----------------------------------------------------------------------

                         Copyright (c) 2000-2005 SAP AG




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




************************************************************************/

/***********************************************************************

	Includes

 ***********************************************************************/

#ifdef WIN32
#include <windows.h>
#endif

/*---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/*---------------------------------------------------------------------------*/

#include "SAPDB/SAPDBCommon/SAPDB_Types.h"

#include "SAPDB/XML/Common/XMLCom_Common.h"

/***********************************************************************

	Defines

 ***********************************************************************/

/***********************************************************************

	Structs & Types

 ***********************************************************************/

/*---------------------------------------------------------------------------*/

/***********************************************************************

	Prototypes

 ***********************************************************************/

void Main_PrintHelp();

SAPDB_Bool Main_GetExecDirectory( char          *cmdArg,
                                  char          *directory,
                                  SAPDB_UInt2  size );
int Main_GetOpt( int           argc,
                 char* const  *argv,
                 const char   *optstr,
                 char        **optarg );

static int Main_OptIsErr( int           argc, 
                          char * const *argv,
                          int           oint,
                          const char   *optstr,
                          int           optchr,
                          int           err    );

/***********************************************************************

	Variables

 ***********************************************************************/

/* Options	*/
#define OPTERRCOLON (1)
#define OPTERRNF (2)
#define OPTERRARG (3)

int optind = 1;
int opterr = 1;
int optopt;

/***********************************************************************

	Functions

 ***********************************************************************/

main(int argc, char *argv[])
{

	char  clo = '\0';
	char  execDir[XMLCOM_MAX_FILENAME_LEN+1] = "";
	char *optarg[128];
    char  errorText[512] = "";

	/* Get current directory	*/
	if ( !Main_GetExecDirectory( argv[0], execDir, XMLCOM_MAX_FILENAME_LEN ) ) {
		exit( EXIT_FAILURE );
	}
	execDir[XMLCOM_MAX_FILENAME_LEN] = '\0';

    /*
    DebugBreak();
    */

	/* Check command line arguments	*/
    optarg[0] = NULL;
    optarg[1] = "xxxx";
    optarg[2] = "yyyy";
    if ( (clo = Main_GetOpt( argc, argv, "f::iur:e:dh", optarg )) != -1) {
		switch ( clo ) {

			/* Register default service */
			case 'i':
				if (!Service_InstallAsService(execDir, NULL)) {
					return EXIT_FAILURE;
				} else {
					return EXIT_SUCCESS;
				}

				break;

			/* Un-register default service */
			case 'u':
				if (!Service_RemoveAsService(NULL)) {
					return EXIT_FAILURE;
				} else {
					return EXIT_SUCCESS;
				}

				break;

			/* Register user defined service */
			case 'r':
				if (!Service_InstallAsService(execDir, optarg[0])) {
					return EXIT_FAILURE;
				} else {
					return EXIT_SUCCESS;
				}

				break;

			/* Un-register user defined service */
			case 'e':
				if (!Service_RemoveAsService(optarg[0])) {
					return EXIT_FAILURE;
				} else {
					return EXIT_SUCCESS;
				}

				break;

#ifdef WIN32
			/* Start to debug	*/
			case 'd':
                if (!Service_Start(execDir, SAPDB_TRUE, NULL, NULL, errorText)) {
                    printf("\n");
                    printf(errorText);
                    printf("\n");
					return EXIT_FAILURE;
                } 

				return EXIT_SUCCESS;

				break;
#endif

			/* Specify an alternate registry path*/
			case 'f':
				if ( optarg[0] ) {
                    if(!Service_Start(execDir, SAPDB_TRUE, optarg[0], optarg[1], errorText)) {
                        printf("\n");
                        printf(errorText);
                        printf("\n");
					    return EXIT_FAILURE;
                    } 
				} else {
					Main_PrintHelp();
				}

				return EXIT_SUCCESS;

				break;

			/* Help	*/
			case 'h':
			default:
				Main_PrintHelp();

				return EXIT_SUCCESS;

				break;
		}
    }

#ifdef WIN32
	Service_SetDispatchTable();
#else
    if (!Service_Start(execDir, SAPDB_FALSE, NULL, NULL, errorText)) {
        printf("\n");
        printf(errorText);
        printf("\n");
		return EXIT_FAILURE;
    } 
#endif

	return EXIT_SUCCESS;

}

/*===========================================================================*/

void Main_PrintHelp()
{

#ifdef WIN32
	printf("Usage: sapdbxie [-i] [-u] [-r <registration name>] [-e <registration name>] [-d] [-f <registry path> <section>] [-h]\n\n");

	printf("Options:\n");
	printf("  -i                                 : to register default\n");
	printf("                                       SAP DB XML Index Engine service.\n");
	printf("  -u                                 : to un-register default\n");
	printf("                                       SAP DB XML Index Engine service.\n");
	printf("  -r <registration name>             : to register an user defined\n");
	printf("                                       SAP DB XML Index Engine service.\n");
	printf("                                       This registration name has to be defined\n");
	printf("                                       before in a dedicated registry section.\n");
	printf("  -e <registration name>             : to un-register an user defined\n");
	printf("                                       SAP DB XML Index Engine service.\n");
	printf("  -d                                 : to start SAP DB XML Index Engine\n");
    printf("                                       from command line\n");
	printf("  -f <registry path> <section>       : to specify an alternate\n");
    printf("                                       registry path and section\n");
	printf("  -h                                 : this help\n\n");

	printf("To start service : net start <registration name>\n");
	printf("To stop  service : net stop <registration name>\n\n");

	printf("Specify registry path for default service in sapdbxie.conf\n");
	printf("or use options -f or -r to address a special registration\n\n");

	printf("To start default service : net start sapdbxie\n");
	printf("To stop  default service : net stop sapdbxie\n\n");
#else
	printf("Usage: wahttp [-i <registration name>] [-u <registration name>] [-f <ini file> <section>] [-h]\n\n");

	printf("Options:\n");
	printf("  -i                                 : to register default\n");
	printf("                                       SAP DB XML Index Engine service.\n");
	printf("  -u                                 : to un-register default\n");
	printf("                                       SAP DB XML Index Engine service.\n");
	printf("  -r <registration name>             : to register an user defined\n");
	printf("                                       SAP DB XML Index Engine service.\n");
	printf("                                       This registration name has to be defined\n");
	printf("                                       before in a dedicated ini file section.\n");
	printf("  -e <registration name>             : to un-register an user defined\n");
	printf("                                       SAP DB XML Index Engine service.\n");
	printf("  -f <ini file> <section>            : to specify an alternate\n");
    printf("                                       ini file and section\n");
	printf("  -h                                 : this help\n\n");

	printf("To start default service specify ini file in sapdbxie.conf\n");
	printf("or use options -f or -r to address a special registration\n\n");
#endif

}

/*===========================================================================*/

SAPDB_Bool Main_GetExecDirectory( char          *cmdArg,
                                  char          *directory,
                                  SAPDB_UInt2    size )
{

	char		 tmpPath[200];
	char		 execPath[200];
	char		 currentDirectory[XMLCOM_MAX_FILENAME_LEN+1];
	char		*separator = NULL;

	/* Get current directory	*/
	currentDirectory[0] = '\0';
	if( !getcwd( currentDirectory, sizeof(currentDirectory) ) ) {
		separator = strrchr(cmdArg, XMLCOM_DIRPATH_SEP);
		if (!separator)
			execPath[0] = '\0';
		else if (separator - cmdArg + 1 > sizeof(execPath)) {
			printf("%s:%s\nCurrent directory path too long!\n", XMLCOM_SERVICE_DISPLAY_NAME, cmdArg);
			return SAPDB_FALSE;
		} else {
			strncpy(execPath, cmdArg, separator - cmdArg + 1);
			execPath[separator - cmdArg + 1] = '\0';
		}

		if (execPath[1] != ':' && execPath[0] != XMLCOM_DIRPATH_SEP) {
			/* this is a relative path */
			sprintf(tmpPath, "%s%c%s", currentDirectory, XMLCOM_DIRPATH_SEP, execPath);
		} else
			strcpy(tmpPath, execPath);

		strcpy( currentDirectory, tmpPath );
	}

	/* Set return value	*/
	strncpy( directory, currentDirectory, size );

	return SAPDB_TRUE;

}

/*===========================================================================*/
    
int Main_GetOpt( int           argc,
                 char* const  *argv,
                 const char   *optstr,
                 char        **optarg )
{

    static int optchr = 0;
    static int dash = 0; /* have already seen the - */

    char *cp;
    int   optArgNo;

    if (optind >= argc)
        return(EOF);

    if (!dash && (argv[optind][0] !=  '-'))
        return(EOF);

    if (!dash && (argv[optind][0] ==  '-') && !argv[optind][1]) {
        /*
         * use to specify stdin. Need to let pgm process this and
         * the following args
         */
        return(EOF);
    }

    if ((argv[optind][0] == '-') && (argv[optind][1] == '-')) {
        /* -- indicates end of args */
        optind++;
        return(EOF);
    }

    if (!dash) {
        assert((argv[optind][0] == '-') && argv[optind][1]);
        dash = 1;
        optchr = 1;
    }

    /* Check if the guy tries to do a -: kind of flag */
    assert(dash);
    if (argv[optind][optchr] == ':') {
        dash = 0;
        optind++;
        return(Main_OptIsErr(argc, argv, optind-1, optstr, optchr, OPTERRCOLON));
    }

    if (!(cp = strchr(optstr, argv[optind][optchr]))) {
        int errind = optind;
        int errchr = optchr;

        if (!argv[optind][optchr+1]) {
            dash = 0;
            optind++;
        }
        else
            optchr++;
        return(Main_OptIsErr(argc, argv, errind, optstr, errchr, OPTERRNF));
    }

    if (cp[1] == ':') {
        optArgNo = 1;
        while(cp[optArgNo] == ':') {
            dash = 0;
            optind++;

            if(optind == argc)
                return(Main_OptIsErr(argc, argv, optind-1, optstr, optchr, OPTERRARG));
            optarg[optArgNo-1] = argv[optind];
            optArgNo++;
        }
        optind++;
        optarg[optArgNo-1] = NULL;
        return(*cp);
    } else {
        if (!argv[optind][optchr+1]) {
            dash = 0;
            optind++;
        }
        else
            optchr++;
        return(*cp);
    }

}

/*===========================================================================*/

static int Main_OptIsErr( int           argc, 
                          char * const *argv,
                          int           oint,
                          const char   *optstr,
                          int           optchr,
                          int           err    )
{

    if (opterr) {
        fprintf(stderr, "Error in argument %d, char %d: ", oint, optchr + 1);

        switch (err) {
        case OPTERRCOLON:
            fprintf(stderr, ": in flags\n");
            break;
        case OPTERRNF:
            fprintf(stderr, "option not found %c\n", argv[oint][optchr]);
            break;
        case OPTERRARG:
            fprintf(stderr, "no argument for option %c\n", argv[oint][optchr]);
            break;
        default:
            fprintf(stderr, "unknown\n");
            break;
        }
    }

    optopt = argv[oint][optchr];
    return('?');

}

/***********************************************************************

	End

 ***********************************************************************/
